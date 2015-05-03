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
#include "EST.h"
#include "OGI_Track.h"
#include "OGIcommon.h"
#include "resLPC.h"
#include "resLPC_io.h"
#include "resLPC_concat.h"
#include "resLPC_mod.h"
#include "resLPC_dump.h"
#include "resLPC_wave.h"
#include "noLPC_wave.h"
#include "OGIresLPC_db.h"


//
//  Resynthesize from wave/pm files
//    (clean this up!)
LISP OGIresLPC_Resynth(LISP utt, LISP files){
  EST_Utterance *u = get_c_utt(utt);
  EST_Wave *out_EST_Wave;
  EST_String pmfile, wavfile, lsffile, modlsffile;

  // if user has not called OGIresLPC.init, set defaults
  if (!(resLPC_params_loaded()))
    OGIresLPC_init_params(NIL);

  // alloc memory
  OGIresLPC_SRC *srcdata = new OGIresLPC_SRC;
  OGIresLPC_MOD *moddata = new OGIresLPC_MOD;
  OGI_Wave      outwave;

  // parse input file list
  pmfile   = wstrdup(get_param_str("pm_file", files, ""));
  wavfile  = wstrdup(get_param_str("wav_file",files, ""));
  lsffile  = wstrdup(get_param_str("lsf_file",files, ""));
  modlsffile = wstrdup(get_param_str("modlsf_file",files, ""));

  // retrieve 'unit' from files
  srcdata->exc.load_chunk(wavfile);   // wave
  srcdata->Fs = srcdata->exc.Fs();    // set Fs
  
  if (pmfile != ""){
    srcdata->pm.load(pmfile);           // pitchmarks
    rm_unvoiced_pmarks(&(srcdata->pm)); // remove unvoiced pmarks (for now)
  }
  else{
    srcdata->pm.append_t(0.0);
    srcdata->pm.append_t(srcdata->exc.tot_chunk_length()/srcdata->Fs);
  }

  if (lsffile != "")
    srcdata->lpc.load(lsffile);
  else{
    srcdata->lpc.append_t(0.0);
    srcdata->lpc.append_t(srcdata->exc.tot_chunk_length()/srcdata->Fs);
  }

  // secondary lsf file (for modified spectra)
  if (modlsffile != ""){
    OGI_Track modlpc;
    modlpc.load(modlsffile);
    cout << "mod lsf features not yet implemented \n";
    // add_mod_lsf_tracks(src->lpc, modlpc);
  }


  // make sure SrcSeg "dur" agrees with length of wave
  float wavend = srcdata->exc.tot_chunk_length()/srcdata->Fs;
  float src_end=0.0;
  EST_Item *sseg;

  // set "end" of srcseg from input "dur"
  for (sseg=u->relation("SrcSeg")->head(); sseg!=0; sseg=sseg->next()){
    src_end += sseg->F("dur");
    sseg->set("end", src_end);    
  }
  
  // readjust: start from end of wave and shrink segs that are 
  //   beyond the end
  float next_end = wavend;
  for (sseg=u->relation("SrcSeg")->tail(); sseg!=u->relation("SrcSeg")->head(); sseg=sseg->prev()){
    if (sseg->F("end") > next_end){
      sseg->set("end", next_end);
      next_end -= (float)0.001;
    }
    if (next_end < 0.0)
      EST_error("Can't make sense of SrcSeg input.\n");
  }

  // re-set "dur" 
  float prev_end=0.0;
  for (sseg=u->relation("SrcSeg")->head(); sseg != 0; sseg=sseg->next()){
    sseg->set("dur", sseg->F("end")-prev_end);
    prev_end = sseg->F("end");
  }  

  // modify prosody to realize target output    
  EST_String mod_method(get_resLPC_params()->S("mod_method"));
  if (mod_method == "none")
    resLPC_NOmod(u, srcdata, moddata);
  else
    resLPC_mod(u, srcdata, moddata); 

  // synthesize output wave
  if (lsffile == "")
    noLPC_wave_synth(srcdata, moddata, &outwave);
  else
    resLPC_wave_synth(srcdata, moddata, &outwave);


  // dump intermediate info if desired
  resLPC_dump(u, srcdata, moddata, &outwave);

  // put outwave into utterance
  out_EST_Wave = new EST_Wave(outwave.Nused(), 1, outwave.mem(), 0, (int)(srcdata->Fs), 1);
  output_wave(u, out_EST_Wave);

  // free memory
  delete srcdata;
  delete moddata;

  return utt;
}
