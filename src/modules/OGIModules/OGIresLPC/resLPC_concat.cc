/****************************<--OGI-->*************************************/
/*                                                                        */
/*             Center for Spoken Language Understanding                   */
/*        Oregon Graduate Institute of Science & Technology               */
/*                         Portland, OR USA                               */
/*                        Copyright (c) 2000                              */
/*                                                                        */
/*      This module is not part of the CSTR/University of Edinburgh       */
/*               release of the Festival TTS system.                      */
/*                                                                        */
/*  In addition to any conditions disclaimers below, please see the file  */
/*  "license_ogi_tts.txt" distributed with this software for information  */
/*  on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  */
/*                                                                        */
/****************************<--OGI-->*************************************/
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "festival.h"
#include "resLPC.h"
#include "resLPC_io.h"
#include "resLPC_concat.h"
#include "OGI_Track.h"
#include "OGI_WaveChunk.h"
#include "OGIdbase.h"
#include "OGIdbase_scm.h"
#include "OGIresLPC_db.h"


static void adjust_cutpoints(EST_Item *newUnit,OGIresLPC_SRC *srcdata, EST_Track *pm_buf, int excB, int *pm_adjust, int *exc_adjust);
static void append_join_info(EST_Item *newUnit, OGIresLPC_SRC *srcdata, EST_Track *pm_buf, int pm_adjust, int exc_adjust);
static void update_srcseg(EST_Utterance *u, EST_Item *dbUnit, float Fs, int pm_adjust, int exc_adjust);
static bool join_voicing(OGI_Track &pm, float wav_end_sec, EST_IList &vjoin_list, int pNpm);
static bool join_is_voiced(OGI_Track &Lpm, EST_Track &Rpm, float LsegE);


//
//  Residual-excited LPC top level synthesis routine
//
void resLPC_concat(
	EST_Utterance *u,
	OGIresLPC_SRC *srcdata // container for all retrieved db units
	){

  OGIdbase *dbase = get_db();  // ptr to current unit inventory
  OGIresLPC_db *udb = NULL;

  // make sure it's a resLPC_db and cast the pointer
  if ( dbase->udata_type == d_resLPC) {
    udb = (OGIresLPC_db *)dbase;
  } else {
    EST_error("resLPC_concat.cc: needs resLPC data base \n");
  }

  //OGIresLPC_db *udb = (OGIresLPC_db *) dbase->udata;

  // temp buffers for db access
  EST_Wave  *exc_buf = NULL;
  EST_Track *pm_buf = NULL;
  EST_Track *lpc_buf = NULL;
  int excB,excE,indx,pNpm, pm_adjust, exc_adjust;
  float pm_offset;

  EST_IList vjoin_list;
  vjoin_list.clear();

  EST_Item *newUnit;

  //srcdata->Fs = dbase->Fs;
  srcdata->Fs = udb->Fs;
  srcdata->exc.set_sample_rate((int) srcdata->Fs);
  EST_Features *gl=get_resLPC_params();
  int T0_max = (int)(gl->F("T0_UV_thresh")*dbase->Fs+1);

  // Check for existence of dbUnit relation in utt
  if (!u->relation_present("dbUnit")){
    cerr << "OGIresLPC:  no dbUnit stream present in utterance" << endl;
    festival_error();
  }

  // create SrcSeg relation
  //    - carries characteristics (durations, etc)  of concatenated segments
  u->create_relation("SrcSeg");

  // Loop through the units to get size of WaveChunk -- pre-alloc
  int  appx_wave_size = 0; // samples
  for (newUnit=u->relation("dbUnit")->head(); newUnit != 0; newUnit=newUnit->next()){
    indx = newUnit->f("db_indx");
    appx_wave_size += udb->load_unit_udata(indx); // returns size
    appx_wave_size += T0_max;  // possible extra zero samples needed around each
  }
  srcdata->exc.prealloc(appx_wave_size);


  // Loop through the units
  for (newUnit=u->relation("dbUnit")->head(); newUnit != 0; newUnit=newUnit->next()){
    //  Items in the dbUnit relation have the following features:
    //    - "db_indx" into dbase
    //    - "isNatNbL" - is the prev unit this unit's natural neighbor?
    //    - "isSegBndL" - is the prev unit in the same segment as this unit?
    //    -  a set of daughters
    //            each with at least the feature "dur"

    // retreive new unit from database -- excitation, pitchmarks, and lpc
    indx = newUnit->f("db_indx");
    udb->get_dbUnit(indx, &exc_buf, &excB, &excE, &pm_buf, &lpc_buf);

    // adjust T0 at join
    adjust_cutpoints(newUnit, srcdata, pm_buf, excB, &pm_adjust, &exc_adjust);

    // Update join info
    append_join_info(newUnit, srcdata, pm_buf, pm_adjust, exc_adjust);

    // Update segment info in SrcSeg
    update_srcseg(u, newUnit, srcdata->Fs, pm_adjust, exc_adjust);

    // record join point (in samples)  - exc.length() already corrected
    newUnit->set("exc", srcdata->exc.length());

    // time offset for pitchmarks
    pm_offset = (srcdata->exc.length() - pm_adjust)/srcdata->Fs;

    // copy excitation WAV
    srcdata->exc.append_chunk(*exc_buf, excB + pm_adjust,
			      excE, newUnit->I("isNatNbL"));

    // copy PITCHMARKS
    pNpm = srcdata->pm.num_frames();
    srcdata->pm.offset_append(*pm_buf, pm_offset);

    /// add info to mark last pmark before join point as V/UV
    join_voicing(srcdata->pm, srcdata->exc.length()/srcdata->Fs, vjoin_list, pNpm);

    // copy LPC COEFFS
    srcdata->lpc.offset_append(*lpc_buf, pm_offset);
  }

  // optionally free unit database (if "ondemand" mode)
  if  (dbase->access_mode == a_ondemand){
    udb->free_all_units();
  }

  // Add channels to pmark track
  add_src_channels(srcdata);

  // mark voiced joins
  EST_Litem *p;
  for (p = vjoin_list.head(); p != 0; p = p->next()){
    srcdata->pm.a(vjoin_list(p), "v/uv") = _V_;
  }

}

static void adjust_cutpoints(
			    EST_Item *newUnit,
			    OGIresLPC_SRC *src,
			    EST_Track *pm_buf,
			    int excB,
			    int *pm_offset,
			    int *exc_adjust){
  // adjust cutpoints to make T0 at joins approx
  // equal to T0 on left side of joins
  *pm_offset  = 0;
  *exc_adjust = 0;

  //if (same_item(newUnit->first(), newUnit))
  if(newUnit->first() == newUnit)
    return;  // do nothing if first unit
  if (newUnit->I("isNatNbL"))
    return;  // do nothing if natural neighbor
  if ((src->pm.num_frames() < 2) || (pm_buf->num_frames() < 2))
    return;  // do nothing if <  2 pmarks on both sides
  if (!(join_is_voiced(src->pm, *pm_buf, src->exc.length()/src->Fs)))
    return;  // do nothing if is unvoiced

  // adjust exc to left of join to be approx recent T0
  int local_T0 = (int)(src->pm.last_T0()*src->Fs + 0.5);  // T0 on left
  *exc_adjust = (int)(local_T0 - (src->exc.length() - src->pm.last_t()*src->Fs));
  *exc_adjust = max(min(src->exc.last_extra(), *exc_adjust),0);

  // do exc adjustment here
  //  (adjust SrcSeg later)
  src->exc.adjust_last_segE(*exc_adjust);

  //  Remaining correction:
  //  make (exc_end - last_pm) + (first_pm) - pm_offset = local_T0
  *pm_offset = (int)(-local_T0 + src->exc.length() - src->pm.last_t()*src->Fs
		        + pm_buf->t(0)*src->Fs + 0.5);

  // don't move fwd past 1st pmark
  *pm_offset = min(*pm_offset, (int)(pm_buf->t(0)*src->Fs));

  // don't move back past sig_band
  *pm_offset = max(*pm_offset, -excB);
}

static void append_join_info(
			     EST_Item *newUnit,
			     OGIresLPC_SRC *srcdata,
			     EST_Track *pm_buf,
			     int pm_adjust,
			     int exc_adjust
			     ){
  //  adds features to newUnit

  // If this is the 1st unit of utt
//  if (same_item(newUnit->first(), newUnit)){
  if (newUnit->first() == newUnit) {
    newUnit->set("isFirstUnit",1);
  }

  // first of new unit
  newUnit->set("pm", srcdata->pm.num_frames());
  newUnit->set("lpcpm", srcdata->lpc.num_frames());
  newUnit->set("pm_adjust", pm_adjust/srcdata->Fs);
  newUnit->set("prev_exc_adjust", exc_adjust/srcdata->Fs);
}


static void update_srcseg(EST_Utterance *u, EST_Item *this_unit, float Fs, int pm_adjust, int exc_adjust){

  EST_Item *srcSeg,*dbSeg;
  EST_Item scopy;
  EST_Relation *SrcSegRel = u->relation("SrcSeg");

  // get ptr to 1st daughter seg of the new dbUnit (dbSeg)
  dbSeg = daughter1(this_unit);

  // if join is not a segment boundary, just add to dur of prev one
  //   (note: assumes other features remain unchanged)
  if (!(this_unit->I("isSegBndL") || this_unit->I("isFirstUnit",0))){
    if (dbSeg!=0){
      srcSeg = SrcSegRel->last();
      srcSeg->set("dur", srcSeg->F("dur") + dbSeg->F("dur") + exc_adjust/Fs - pm_adjust/Fs);
      srcSeg->append_daughter(dbSeg);
      dbSeg = next_sibling(dbSeg);
    }
    else
      cerr << "   **dbSeg=NULL! \n";
  }

  // append new item(s) for remaining segments in unit
  for ( ; dbSeg!=0; dbSeg=next_sibling(dbSeg)){
    // append to SrcSeg relation
    srcSeg = SrcSegRel->append();
    srcSeg->set_name(dbSeg->name());
    srcSeg->set("dur",dbSeg->f("dur").Float());
    srcSeg->append_daughter(dbSeg);
  }
}

static bool join_voicing(OGI_Track &pm, float wav_end_sec, EST_IList &vjoin_list, int prev_Npm){
  EST_Features *gl=get_resLPC_params();
  float T0_UV_thresh = gl->F("T0_UV_thresh");
  bool prev_empty = FALSE;
  static float wav_begin = 0.0;

  if (pm.num_frames() > 1 && pm.num_frames() > prev_Npm){ // JW: added condition in case new unit has no pms
    // if signal prior to first pm is UV, then
    //   make sure the previous unit's last pm not V
    if (pm.t(prev_Npm) - wav_begin > T0_UV_thresh){          // 1st is UV
      if ((vjoin_list.length() > 0 ) &&
	  (vjoin_list.last() == prev_Npm-1)){   // prev last declared V
	vjoin_list.remove(vjoin_list.tail());   // remove it
      }
    }
    else {
      if ((vjoin_list.length() > 0 ) && /// if a long empty unit was inserted
	  (wav_begin - pm.t(vjoin_list.last()) > T0_UV_thresh)){
	vjoin_list.remove(vjoin_list.tail());   // remove it
      }
    }

    // if last pm in unit appears to be V
    if (wav_end_sec - pm.last_t() < T0_UV_thresh){  // last is V
      vjoin_list.append(pm.num_frames()-1);   // append it
    }
    wav_begin = wav_end_sec;
    return TRUE;
  }
  else {
    prev_empty = TRUE;
  }

  // do nothing if last is UV
  wav_begin = wav_end_sec;
  return FALSE;
}

void add_src_channels(OGIresLPC_SRC *src){

  int ch;
  src->pm.resize(EST_ALL, 3);

  ch = 0;
  src->pm.set_channel_name("v/uv",ch++);
  src->pm.set_channel_name("T0_src",ch++);
  src->pm.set_channel_name("beta",ch++);
}

static bool join_is_voiced(OGI_Track &Lpm, EST_Track &Rpm, float LsegE){
  // NOTE: this method will give poor results if we have a voiced unit
  //       with < 2 pitchmarks!
  //   This will be fixed if we decide to store pmarks marked as V or UV -
  //   then we can just look at a V/UV field in the Track.
  EST_Features *gl=get_resLPC_params();
  float T0_UV_thresh = gl->F("T0_UV_thresh");

  if ((Rpm.num_frames() > 1) && (Lpm.num_frames() > 1)){
    float L_T0 = Lpm.last_T0();
    float R_T0 = Rpm.t(1) - Rpm.t(0);

    if ((Rpm.t(0) < T0_UV_thresh)
	&& ((LsegE - Lpm.last_t()) < T0_UV_thresh)){
      if (L_T0 != OGI_UNDEFINED){
	if ((L_T0 < T0_UV_thresh) && (R_T0 < T0_UV_thresh))
	  return TRUE;
      }
    }
  }
  return FALSE;
}
