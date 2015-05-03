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
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "EST.h"
#include "OGIdbase.h"
#include "OGIsine_db.h"
#include "OGI_file.h"
#include "OGI_SineTrack.h"

static void time_shift_track(EST_Track &tr, float ts);

OGIsine_db::OGIsine_db(){
  cout << "alloc new OGIsine_db obj\n";
}
OGIsine_db::~OGIsine_db(){  
  cout << "delete OGIsine_db obj - free anyting?\n";
}
////////////////////////////////////////////////////
//////  runtime access funtions for synthesis  /////
////////////////////////////////////////////////////

//
// Get all the data needed to synthesize a unit 
//    - called from main synthesis functions in sine.cc
//    - both grouped and ungrouped access
//    - just sets pointers, does NOT copy data, so don't
//      try to change values w/o first copying in synthesis routine
//
void OGIsine_db::get_dbUnit( 
	 OGIdbase &db, // need this for grp_flag and access_mode
	 int db_indx,  /// index of the one we want
	 OGI_SineTrack **sine_out,    //
	 int *sine_len   // length of unit in samples
	 ){

  // load it
  load_unit_udata(db_indx, db);

  // set pointers to unit
  *sine_out  = sine(db_indx);

  // assume sineB is 0 and that unit has been trimmed
  *sine_len = sineE(db_indx);
}


//
// Loads speech data for ONE UNIT from 
//             (already opened) groupfile
//
void OGIsine_db::load_unit_udata(int indx,  OGIdbase &db){

  static int prev_indx = -1;

  // free previous one
  if  (db.access_mode == a_ondemand){
    if ((prev_indx >= 0) && (isLoaded(prev_indx))){
      free_unit(prev_indx);
    }
  }

  ///////////
  // a_dynamic: could make dynamic mode have a FIFO cache here
  //     -- just maintain an EST_IList 
  //   create a function: update_cache(prev_indx);
  ///////////

  if ((db.access_mode == a_dynamic) || (db.access_mode == a_ondemand)){ 
    if (!(isLoaded(indx))){
      if (db.isGrouped){
	load_grp_sine(indx, db);
      }
      else{  // ungrouped 
	load_ungrp_sine(indx, db);

	// trim (don't take any extra frames for now)
	trim_unit(indx, db.Fs);
      }
    }
  }

  // else direct mode, assume already loaded
  prev_indx = indx;
}


////////////////////////////////////////////////////
//////   grouped load/save  ////////////////////////
////////////////////////////////////////////////////
void OGIsine_db::load_grp_offsets(OGIdbase &db){ 

  int k;
  int N = length();

  // temp storage
  long* offsets = new long [N];
  if (offsets == NULL) cerr << "OGIsine_db::load_grp_offsets - insufficient memory" 
	                          << endl, exit(-1);

  // get byte offsets
  OGIfread(offsets,  sizeof(long), N, db.gfd, db.swap);
  for (k=0; k<N; k++)
    p_sine_offset[k] = offsets[k];

  delete offsets;
}

void OGIsine_db::load_grp_sine(int n, OGIdbase &db){ 
  
  int itmp[2];

  // find offset into groupfile
  fseek(db.gfd, p_sine_offset(n), SEEK_SET);

  // load bound info
  fread(itmp, sizeof(int), 2, db.gfd);
  p_sineB[n] = itmp[0];
  p_sineE[n] = itmp[1];
  
  // alloc new obj
  p_sine[n] = new OGI_SineTrack;

  // read the wave
  (*(p_sine[n])).load_fp(db.gfd); // see OGI_SineTrack.h

  // mark as loaded
  p_sine_isLoaded[n] = TRUE;

}

//
// Saves ALL UNITS' speech data in (already opened) groupfile
//
void  OGIsine_db::save_grp_udata(OGIdbase &db){
  int k;
  int N = length(); 

  long* offsets = new long [N];
  if (offsets == NULL) cerr << "OGIsine_db::save_grp_udata - insuffient memory"
	                          << endl, exit(-1);  

  int sineinfo[2];   // tmp
  long fp_done,fp_off;

  // write the size
  fwrite(&N, sizeof(int), 1, db.gfd);

  // reserve a place in the file for the offsets
  fp_off = ftell(db.gfd);
  memset(offsets, 0, sizeof(long)*N);
  fwrite(offsets, sizeof(long), N, db.gfd); 

  // write data
  for (k=0; k<N; k++){
    // be sure this one is loaded
    load_unit_udata(k, db);

    // write speech data, remember byte offsets
    p_sine_offset[k] = ftell(db.gfd);
    sineinfo[0] = sineB(k);
    sineinfo[1] = sineE(k);
    fwrite(sineinfo, sizeof(int), 2, db.gfd);
    (*(sine(k))).save_fp(db.gfd, db.isCompressed); // see OGI_SineTrack.h
    
  }
  fp_done = ftell(db.gfd);  // remember end


  // go back and fill in offsets
  fseek(db.gfd, fp_off, SEEK_SET);
  for (k=0; k<N; k++)
    offsets[k] = sine_offset(k);
  fwrite(offsets,  sizeof(long), N, db.gfd);
  fseek(db.gfd, fp_done, SEEK_SET);  // move to end

  delete offsets;
}

void  OGIsine_db::load_grp_udata(OGIdbase &db){

  int k,N;
 
  // read the size
  OGIfread(&N, sizeof(int), 1, db.gfd, db.swap);
  if (N != db.unitdic.Nunits())   // check
    EST_error("udata->length() != db.unitdic.Nunits() !!! \n");

  // realloc
  resize(N);

  // read byte offsets
  load_grp_offsets(db);

  // if a_direct mode, read all data 
  if (db.access_mode == a_direct){
    for (k=0; k<length(); k++){
      load_grp_sine(k, db);
    }
  }
}


////////////////////////////////////////////////////
////// ungrouped load/save  ////////////////////////
////////////////////////////////////////////////////
void OGIsine_db::load_ungrp_sine(int n, OGIdbase &db){ 
  //  cout << "load_ungrp_sine: " << n << endl;

  // alloc new objs
  p_sine[n] = new OGI_SineTrack;

  // get sineB, sineE from unitdic
  p_sineB[n] = db.unitdic.unitB(n);
  p_sineE[n] = db.unitdic.unitE(n);

  // create filename
  EST_String sinefile = db.fileinfo.sval("base_dir")
    + db.fileinfo.sval("data_dir") + db.unitdic.file(n) + db.fileinfo.sval("data_ext");

  EST_read_status ret = (*p_sine[n]).load(sinefile);  // load file
  if (ret != read_ok)
    EST_error(EST_String("OGIsine_db.cc:  could not read sine file ") + sinefile + EST_String("\n"));

  // adjust so times are measured relative to sineB instead of 0
  (*p_sine[n]).time_shift(-p_sineB[n]/db.Fs);

  p_sine_isLoaded[n] = TRUE;  // mark as loaded
}

void  OGIsine_db::load_ungrp_udata(OGIdbase &db){
  int k;

  // get size from unitdic, realloc
  resize(db.unitdic.Nunits());

  // if a_direct mode, load speech data 
  if (db.access_mode == a_direct){
    for (k=0; k<length(); k++){
      load_ungrp_sine(k, db);
    }
  }
}



////////////////////////////////////////////////////
////// utilities    ////////////////////////////////
////////////////////////////////////////////////////


void OGIsine_db::resize(int N){
  p_sine.resize(N);
  p_sineB.resize(N); 
  p_sineE.resize(N); 
  p_sine_isLoaded.resize(N);
  p_sine_offset.resize(N); 
}

// cut unused portions of data
void OGIsine_db::trim_unit(
			   int i,      // unit indx
			   float Fs    // samp freq
			   ){

  // At this point, t() of each SineTrack has already been shifted 
  //   to be relative to sineB.

  // find frames within [0, sineE(i)-sineB(i)]
  int iB = (*p_sine[i]).frames().index_above(0.0);
  int iE = (*p_sine[i]).frames().index_below((sineE(i)-sineB(i))/Fs);

  // create a smaller one, delete old one
  OGI_SineTrack *newSine = new OGI_SineTrack;
  (*p_sine[i]).copy_to(*newSine, iB, iE-iB+1);
  delete p_sine[i];
  p_sine[i] = newSine;

  // change bounds info to reflect part we cut
  p_sineE[i] = sineE(i)-sineB(i);
  p_sineB[i] = 0;
  /*  printf("trim: [%1.4f %1.4f %1.4f %1.4f]\n", 
	 p_sineB[i]/Fs, (p_sine[i])->t(0), (p_sine[i])->last_t(),  p_sineE[i]/Fs);*/
}

static void time_shift_track(EST_Track &tr, float ts){
  int k;
  float t_new;
  for (k=0; k<tr.num_frames(); k++){
    t_new = tr.t(k) + ts;
    tr.t(k) = t_new;
  }
}


#if defined(INSTANTIATE_TEMPLATES)
#include "../../../../speech_tools/base_class/EST_TVector.cc"
template class EST_TVector<OGI_SineTrack *>;
#endif            

//
// OGI_SineTrack *
//
static OGI_SineTrack * const def_val_OGI_SineTrackVect = NULL;
OGI_SineTrack * const *EST_TVector<OGI_SineTrack *>::def_val = &def_val_OGI_SineTrackVect;
static OGI_SineTrack *error_return_OGI_SineTrackVect = NULL;
OGI_SineTrack * *EST_TVector<OGI_SineTrack *>::error_return = &error_return_OGI_SineTrackVect;

