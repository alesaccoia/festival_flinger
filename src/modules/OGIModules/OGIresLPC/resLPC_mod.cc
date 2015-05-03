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
#include "EST.h"
#include "festival.h"
#include "resLPC.h"
#include "resLPC_io.h"
#include "resLPC_mod.h"
#include "resLPC_concat.h"
#include "OGI_Track.h"
#include "OGI_TimeWarp.h"
#include "OGI_WaveChunk.h"
#include "OGI_Buffer.h"
#include "OGI_Macro.h"
#include "OGIduration.h"
#include "OGIpitch.h"
#include "OGIgain.h"
#include "Phone.h"
#include "resLPC_pmark.h"

static void add_mod_channels(OGIresLPC_MOD *mod);
static void mod_silences( EST_Utterance *u, OGIresLPC_SRC *src, OGIresLPC_MOD *mod);
static void make_T0_src_contour(OGIresLPC_SRC *src);
static void make_output_pmarks_direct( OGIresLPC_SRC *src, OGIresLPC_MOD *mod);
static void ramp_sil_energy(OGI_Track &lpc, float Bt, float Et, int direction);
static void update_srcseg_pmark_info( EST_Utterance *u, OGIresLPC_SRC *src);
static int get_smooth_region(LISP smoothTable, EST_Item *Lph, EST_Item *Rph);
static void match_track_joins( EST_Utterance *u, OGI_Track &srclpc, int chB, int Nch, LISP smoothTable, bool doCrossPh, bool doMatch);
static void match_joins_power( EST_Utterance *u, OGIresLPC_SRC *src );
static void match_joins_spectra( EST_Utterance *u, OGIresLPC_SRC *src);
static void apply_ext_gain( EST_Utterance *u, OGIresLPC_SRC *src, OGI_TimeWarp &warp );


static void make_output_pmarks_copy_srcpm(
					OGIresLPC_SRC *src,
					OGIresLPC_MOD *mod
					);

//
// "direct" method:  interpolate within target F0 contour to
//    get target pitchmarks, use T0 of original pmarks in UV regions
//
void resLPC_mod( 
		EST_Utterance *u, 
		OGIresLPC_SRC *src, 
		OGIresLPC_MOD *mod){

  EST_Features *gl = get_resLPC_params();

  // Add needed channels into Tracks
  add_mod_channels(mod);
  if (!src->pm.has_channel("v/uv"))
    add_src_channels(src);

  // Generate time warping function from durations
  if (gl->S("dur_method") == "natural")
    make_default_time_warp(u, mod->warp);
  else
    make_time_warp(u, mod->warp);

  // time stretch silences if needed
  mod_silences(u, src, mod);

  // Make voicing decision 
  pmark_V_UV(src->pm, src->Bvsect, src->Evsect, src->exc.length(), src->Fs,  *gl);
  
  // Add UV pmarks
  make_UVpmarks(src->pm, src->Bvsect, src->Evsect, src->exc.length(), src->Fs,  *gl, 1);

  // Bookkeeping for pmark locations relative to SrcSeg
  update_srcseg_pmark_info(u, src);

  // Get F0 targets
  get_target_F0(u, mod->F0targ, mod->warp, gl->F("F0_default"));

  // smooth spectral features
  match_joins_spectra(u, src);

  // smooth power
  match_joins_power(u, src);

  // external gain adjustment
  apply_ext_gain(u, src, mod->warp);

  // calc T0 for each pitchmark
  make_T0_src_contour(src);    

  // create synthesis pitchmarks
  if (gl->S("pitch_method") == "natural")
    make_output_pmarks_copy_srcpm(src, mod);    
  else
    make_output_pmarks_direct(src, mod);
}


//
// "none" method:  don't modify duration or F0 at all
//
void resLPC_NOmod( 
		   EST_Utterance *u, 
		   OGIresLPC_SRC *src, 
		   OGIresLPC_MOD *mod){

  EST_Features *gl = get_resLPC_params();

  // Add needed channels into Tracks
  add_mod_channels(mod);
  if (!src->pm.has_channel("v/uv"))
    add_src_channels(src);

  // Generate time warping function from durations
  make_default_time_warp(u, mod->warp);

  // Make voicing decision 
  pmark_V_UV(src->pm, src->Bvsect, src->Evsect, src->exc.length(), src->Fs,  *gl);

  // Add UV pmarks
  make_UVpmarks(src->pm, src->Bvsect, src->Evsect, src->exc.length(), src->Fs,  *gl, 1);

  // calc T0 for each pitchmark
  make_T0_src_contour(src);    

  // Bookkeeping for pmark locations relative to SrcSeg
  update_srcseg_pmark_info(u, src);

  // create synthesis pitchmarks
  make_output_pmarks_copy_srcpm(src, mod);

}

static void add_mod_channels(OGIresLPC_MOD *mod){
  int ch=0;
  mod->pm.resize(EST_ALL, 3);

  mod->pm.set_channel_name("lpc_map",ch++);
  mod->pm.set_channel_name("exc_map",ch++);
  mod->pm.set_channel_name("v/uv",ch++);  
}

//
// Make silences more silent 
//
static void mod_silences(
			 EST_Utterance *u,
			 OGIresLPC_SRC *src,
			 OGIresLPC_MOD *mod
			 ){
  float tB=0,tE=0,GtargL=1.0,GtargR=1.0,incr=0,mid=0;
  EST_Item *useg;
  EST_Item *firstseg = u->relation("Segment")->head();
  EST_Item *lastseg  = u->relation("Segment")->tail();

  // Loop thru segments
  for (useg=u->relation("Segment")->head(); useg != 0; useg=useg->next()) {

    if (ph_is_silence(useg->name())){

      // set insert point and ramp energy to 0 in initial/final silences
//      if (useg->same_item(firstseg)){ // initial silence 
	if (useg == firstseg) { // initial silence
        tB = mod->warp.invmap(0.0);
	tE = mod->warp.invmap(firstseg->F("end"));
	ramp_sil_energy(src->lpc, tB, tE, 1);
      
	incr = ffeature(useg,"segment_duration").Float()/(float)5.0;
	GtargL = seg_gain_target(u, useg, 1);
	if (GtargL == 1.0)
	  GtargL = seg_gain_target(u, useg->next(), 0);	  
	OGIgain_targ(u, 0.0, firstseg->F("end"),
		     0.0, GtargL,
		     incr,"sin");
      }
//      else if (useg->same_item(lastseg)){ // final silence 
      else if (useg == lastseg) { // final silence
	tB = mod->warp.invmap(ffeature(lastseg, "start").Float());
	tE = mod->warp.invmap(lastseg->F("end"));
	ramp_sil_energy(src->lpc, tB, tE, -1);

	incr = ffeature(useg,"segment_duration").Float()/(float)5.0;
	GtargR = seg_gain_target(u, useg, 0);
	if (GtargR == 1.0)
	  GtargR = seg_gain_target(u, useg->prev(), 1);	  
	OGIgain_targ(u,ffeature(lastseg,"start").Float(),lastseg->F("end"),
		     GtargR, 0.0, 
		     incr,"sin");
      }
      else {  // medial silence
	tB = mod->warp.invmap(ffeature(useg,"start").Float());
	mid = ffeature(useg,"start").Float() 
	      + ffeature(useg,"segment_duration").Float()/(float)2.0;
	tE = mod->warp.invmap(mid);
	ramp_sil_energy(src->lpc, tB, tE, -1);
	tB = tE;
	tE = mod->warp.invmap(useg->F("end"));
	ramp_sil_energy(src->lpc, tB, tE, 1);

	incr = ffeature(useg,"segment_duration").Float()/(float)5.0;
	GtargL = seg_gain_target(u, useg, 1);
	if (GtargL == 1.0)
	  GtargL = seg_gain_target(u, useg->prev(), 1);	

	GtargR = seg_gain_target(u, useg, 0);
	if (GtargR == 1.0)
	  GtargR = seg_gain_target(u, useg->next(), 0);	    

	OGIgain_targ(u,ffeature(useg,"start").Float(), mid,
		     GtargL, 0.0,
		     incr,"sin");
	OGIgain_targ(u, mid, useg->F("end"),
		     0.0, GtargR,
		     incr,"sin");
      }
    }
  }
}

static void ramp_sil_energy(OGI_Track &lpc, float Bt, float Et, int direction){
  
  int chE = lpc.channel_position("E");
  int B,E,k;
  float g,incr;

  B = lpc.index_above(Bt);
  E = lpc.index_below(Et);
  if (direction == 1) // down to end
    g = 0;  // == gain of 1.0
  else                // down to beginning
    g = -10; // == near 0
  
  if (E > B){
    incr = -direction * 10/(float)(E-B);
    for (k=B; k<=E; k++){
      lpc.a_no_check(k,chE) = lpc.a_no_check(k,chE) + g;
      g += incr;
    }
  }
} 

//
// Generate T0_src for each pmark
//
static void make_T0_src_contour(OGIresLPC_SRC *src){
  int k;
  EST_Track T0_src;
  src->pm.sub_track(T0_src, 0, EST_ALL, src->pm.channel_position("T0_src"), 1);
  int Npm = src->pm.num_frames();

  for (k=0; k<Npm-1; k++){
    // source To contour
    T0_src.a_no_check(k) = src->pm.T0(k);
  }

  // last one
  if (Npm > 1){
    T0_src(Npm-1) = T0_src(Npm-2);
  }
}

//
//  Keep track of which pmarks in which unit
//
static void update_srcseg_pmark_info(
				     EST_Utterance *u, 
				     OGIresLPC_SRC *src){
  int k_pm=0; 
  int k_lpcpm=0;
  int Npm=src->pm.num_frames();
  int Nlpcpm=src->lpc.num_frames();
  float src_end=0.0;
  EST_Item *sseg;
  
  // Loop thru SrcSeg, record 1st pmark of each segment
  for (sseg=u->relation("SrcSeg")->head(); sseg!=0; sseg=sseg->next()){
    
    src_end  += sseg->F("dur");

    // pmarks
    sseg->set("pm", k_pm);
    while ((k_pm < Npm) && (src->pm.t(k_pm) < src_end )){
      k_pm++;
    }
  
    // lpc pmarks
    sseg->set("lpcpm", k_lpcpm);
    while ( (k_lpcpm < Nlpcpm) && (src->lpc.t(k_lpcpm) < src_end )){
      k_lpcpm++;
    }
  }
}

//
// Apply correction to smooth LSF trajectories at joins
//
static void match_joins_spectra(
				 EST_Utterance *u,
				 OGIresLPC_SRC *src
				 ){  
  LISP smoothTable = NIL;
  EST_Features *gl = get_resLPC_params();
  if (gl->present("spectra_smooth")){
    smoothTable = read_from_lstring(strintern(gl->S("spectra_smooth")));
	bool doCrossPh = gl->I("smooth_cross_ph_join") ? true : false;
    bool doMatch = TRUE;
    
    if (gl->S("spectra_match_or_replace") == "replace")
      doMatch = FALSE;
    
    int chLSF0 = src->lpc.channel_position("LSF_0");
    int Nlsf   = src->lpc.channel_position("LSF_N")-chLSF0+1;
    
    match_track_joins(u, src->lpc, chLSF0, Nlsf, smoothTable, doCrossPh, doMatch);
  }
}

//
// Apply correction to smooth power at joins
//
static void match_joins_power(
			      EST_Utterance *u,
			      OGIresLPC_SRC *src
			      ){    
  EST_Features *gl = get_resLPC_params();
  LISP smoothTable = NIL;
  if (gl->present("power_smooth")){
    smoothTable = read_from_lstring(strintern(gl->S("power_smooth")));
	bool doCrossPh = gl->I("smooth_cross_ph_join") ? true : false;
    bool doMatch = TRUE;
    
    if (gl->S("power_match_or_replace") == "replace")
      doMatch = FALSE;
    
    match_track_joins(u, src->lpc, src->lpc.channel_position("E"), 1, 
		      smoothTable, doCrossPh, doMatch);
  }
}

static void match_track_joins(
			      EST_Utterance *u,
			      OGI_Track &srclpc,
			      int chB,  // begin channel in track
			      int Nch,  // number of channels to smooth
			      LISP smoothTable, // smoothing info
			      bool doCrossPh,
			      bool doMatch){    
  EST_Item  *Lph,*Rph;
  int smoothlen=0,B=0,M=0,E=0,nextpm=0;
  
  EST_Item *dbUnit=u->relation("dbUnit")->head();    

  // last one in the first unit
  Lph = parent(as(daughtern(dbUnit),"SrcSeg"));

  for  (dbUnit=dbUnit->next(); dbUnit != 0; dbUnit=dbUnit->next()){
    // first phone in this unit
    Rph = parent(as(daughter1(dbUnit),"SrcSeg"));

    smoothlen = 0;
    if (!(dbUnit->I("isNatNbL"))){  // don't smooth if nat neighbor to prev

      // smooth if this is intra-phone join or if cross-phone smoothing enabled
      if (!(dbUnit->I("isSegBndL")) || doCrossPh){
	
	// iterate through lisp list of context features to match against phones
	smoothlen = get_smooth_region(smoothTable, Lph, Rph);
      }
    }
    
    // join point
    M = dbUnit->I("lpcpm");
    
    // begin point: limit to begin of unit and begin of segment the join is in
    B = max(dbUnit->prev()->I("lpcpm"), M - smoothlen);
    B = max(B, Lph->I("lpcpm"));
        
    // end point: limit to end of unit and end of segment the join is in
    if (dbUnit->next())
      nextpm = dbUnit->next()->I("lpcpm");
    else
      nextpm =  srclpc.num_frames();
    E = min(nextpm, M + smoothlen);
    if (Rph->next())
      E = min(E, Rph->next()->I("lpcpm")-1);
    
    // do smoothing
    if (doMatch)
      srclpc.linear_match(B, M, E, chB, Nch);   // adjustment of track
    else
      srclpc.linear_replace(B, M, E, chB, Nch); // replacement of track
        
    // last phone in this unit
    Lph = parent(as(daughtern(dbUnit),"SrcSeg"));
  }
}


static int get_smooth_region(LISP smoothTable, EST_Item *Lph, EST_Item *Rph){
  EST_String phfeat,targ_featval;
  LISP l;
  int smoothlen = 0;

  if ((Lph != 0) && (Rph != 0)){
    for (l=smoothTable; l != NIL; l=cdr(l)){
      phfeat       = EST_String(wstrdup(get_c_string(car(car(l)))));
      targ_featval = EST_String(wstrdup(get_c_string(car(cdr(car(l))))));
      
      if ((targ_featval == ph_feat(Lph->name(), phfeat)) 
	  && (targ_featval == ph_feat(Rph->name(), phfeat))){
	smoothlen = get_c_int(car(cdr(cdr(car(l)))));
	break;
      }
    }
  }
  return smoothlen;
}


//
// apply external gain modification
//
static void apply_ext_gain(
			   EST_Utterance *u,
			   OGIresLPC_SRC *src,
			   OGI_TimeWarp &warp
			   ){    

  if (!(u->relation_present("Gain")))
    return;

  OGI_SingleTrack Gtarg;
  EST_Item *targ;
  const EST_Relation *utt_targ = u->relation("Gain");

  // Exit if there are no gain targets given
  if (utt_targ->length() == 0)
    return;
  
  // prepend a target if the time of the 1st target is not at time=0
  targ = utt_targ->first();

  if ((!(targ->f_present("pos"))) || (targ->F("pos") != 0.0))
    Gtarg.prepend(0.0, 1.0);

  // start append
  Gtarg.begin_append(utt_targ->length());

  // loop through targets
  for (targ=utt_targ->first(); targ != 0; targ=targ->next()) {
    if (targ->f_present("pos"))
      Gtarg.append(targ->F("pos"), targ->F("gain"));
  }  
  // Ensure there is a target at the end 
  targ = utt_targ->last();
  if ((!(targ->f_present("pos"))) ||(targ->F("pos") < u->relation("Segment")->last()->F("end"))){
    Gtarg.append(warp.last_targ_end(), utt_targ->last()->F("gain"));
  }
  Gtarg.end_append();

  // Loop through src lpc pitchmarks, map fwd to apply gain value
  int ch = src->lpc.channel_position("E");
  int N = src->lpc.num_frames();
  for (int k=0; k<N; k++){
    src->lpc.a_no_check(k, ch) += (float)log(10e-10 + Gtarg.linterp(warp.fwdmap(src->lpc.t(k))));
  }
}



static void make_output_pmarks_direct(
				      OGIresLPC_SRC *src,
				      OGIresLPC_MOD *mod
				      ){  
  int n=0;
  int chT0_SRC = src->pm.channel_position("T0_src");
  int chEXCMAP = mod->pm.channel_position("exc_map");
  int chLPCMAP = mod->pm.channel_position("lpc_map");
  int chVUV    = mod->pm.channel_position("v/uv");
  int exc_map;

  EST_Track v_uv;
  src->pm.sub_track(v_uv, 0, EST_ALL, src->pm.channel_position("v/uv"), 1);
  
  OGI_Track *srcpm = &(src->pm);
  OGI_Track *modpm = &(mod->pm);
  float modT0=(float)0.01, ta, modF0=100.0, ts=0.0;

  // generate pitchmarks through end of last target Segment
  modpm->append_t_buf_start();
  while (ts < mod->warp.last_targ_end()){

    // (buffered) append to modified pmarks
    modpm->append_t_buf(ts);

    // mapping from ts to src time ta
    ta = mod->warp.invmap(ts);

    // map to nearest exc pmark indx
    modpm->a_no_check(n, chEXCMAP) = srcpm->nearest_indx_f(ta);

    // map to nearest LPC pmark indx
    modpm->a_no_check(n, chLPCMAP) = src->lpc.nearest_indx_f(ta);

    // modified T0: use original T0 in UV regions
    exc_map = (int)(modpm->a_no_check(n, chEXCMAP) + 0.5);
    if (v_uv.a_no_check(exc_map) == _V_){
      modpm->a_no_check(n, chVUV) = _V_;
      modF0 = mod->F0targ.linterp(ts);
      if (modF0 > 1.0)
	modT0 = 1/modF0;
    }
    else {
      modpm->a_no_check(n, chVUV) = _UV_;
      modT0 = srcpm->nearest(ta, chT0_SRC);
    }

    ts += modT0;
    n++;
  }
  modpm->append_t_buf_finish();
}


//
// Copy src pmarks - used to preserve F0 contour
//
static void make_output_pmarks_copy_srcpm(
					OGIresLPC_SRC *src,
					OGIresLPC_MOD *mod
					){  
  
  int n=0;
  int chT0_SRC = src->pm.channel_position("T0_src");
  int chEXCMAP = mod->pm.channel_position("exc_map");
  int chLPCMAP = mod->pm.channel_position("lpc_map");
  int chVUV    = mod->pm.channel_position("v/uv");
  
  EST_Track v_uv;
  src->pm.sub_track(v_uv, 0, EST_ALL, src->pm.channel_position("v/uv"), 1);

  OGI_Track *srcpm = &(src->pm);
  OGI_Track *modpm = &(mod->pm);
  float modT0=0.0, ta=0.0, ts=0.0;
  int exc_map;

  // generate pitchmarks through end of last target Segment
  modpm->append_t_buf_start();
  while (ts < mod->warp.last_targ_end()){

    // append to targ_pm Track
    modpm->append_t_buf(ts);

    // mapping from ts to src time ta
    ta = mod->warp.invmap(ts);

    // map to nearest wav pmark indx
    modpm->a_no_check(n, chEXCMAP) = srcpm->nearest_indx_f(ta);

    // map to nearest LPC pmark indx
    modpm->a_no_check(n, chLPCMAP) = src->lpc.nearest_indx_f(ta);

    // keep track of voicing
    exc_map = (int)(modpm->a_no_check(n, chEXCMAP) + 0.5);
    if (v_uv(exc_map) == _V_)
      modpm->a_no_check(n, chVUV) = _V_;
    else 
      modpm->a_no_check(n, chVUV) = _UV_;

    // modified T0: (T0_src nearest to ta)
    modT0 = srcpm->nearest(ta, chT0_SRC);
    ts += modT0;
    n++;
  }
  modpm->append_t_buf_finish();
}


void rm_unvoiced_pmarks(OGI_Track *pm){
  // tied to these specific definitions of "v/uv" and _UV_
  //  this is a very slow way to do it
  OGI_Track *newpm = new OGI_Track;
  int chV = pm->channel_position("v/uv");
  if (chV < 0)
    return;

  int n,k;

  newpm->resize(pm->num_frames(), pm->num_channels());
  for (n=0,k=0; n < pm->num_frames(); n++){
    if (pm->a_no_check(n, chV) != _UV_){
      newpm->copy_frame_in(k, *pm, n);
      k++;
    }
  }

  delete pm;
  pm = newpm;
}
