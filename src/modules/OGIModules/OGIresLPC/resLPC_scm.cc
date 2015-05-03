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
#include "resLPC_resyn.h"
#include "noLPC_wave.h"
#include "OGIresLPC_db.h"
#include "OGI_file.h"
#include "resLPC_analysis.h"
#include "pmark_analysis.h"

// Voice Conversion
#include "resLPC_vc.h" // AK

static void init_voice_conv(void);

//
//  Top level synthesis called from Scheme.
//
LISP OGIresLPC_Synthesize(LISP utt){
  EST_Utterance *u = get_c_utt(utt);
  EST_Wave *out_EST_Wave;
  
  // alloc memory
  OGIresLPC_SRC *srcdata = new OGIresLPC_SRC;
  OGIresLPC_MOD *moddata = new OGIresLPC_MOD;
  OGI_Wave      outwave;

  // retrieve units from database
  resLPC_concat(u, srcdata); 

  // modify to realize target output
  EST_String mod_method(get_resLPC_params()->S("mod_method"));
  if (mod_method == "none")
    resLPC_NOmod(u, srcdata, moddata);
  else
    resLPC_mod(u, srcdata, moddata);

  // synthesize output wave
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


//
// Top level Synth init called from Scheme.
//
LISP OGIresLPC_Init(LISP params){
  // extract LISP params, clear old ones first
  OGIresLPC_init_params(params);
  
  // Voice Conversion initialization
  init_voice_conv();
  
  return NIL;
}

LISP OGIresLPC_AddInit(LISP params){
  // if user has never called OGIresLPC.init, set defaults
  if (!(resLPC_params_loaded()))
    OGIresLPC_init_params(NIL);

  // extract LISP params, don't clear old ones
  OGIresLPC_addto_params(params);
  
  // Voice Conversion initialization
  init_voice_conv();
  
  return NIL;
}


static void init_voice_conv(void){
  // Voice Conversion initialization
  EST_Features *gl = get_resLPC_params();
  EST_String vcfile;

  if (gl->S("vc_file") != ""){
    vcfile = gl->S("vc_file");
    vc_init(vcfile.updatable_str()); // avoid const complaints
    gl->set("doVC",1);
  }
}


void festival_OGIresLPC_init(void){
  proclaim_module("OGIresLPC");


     init_subr_1("OGIresLPC.init",OGIresLPC_Init,
    "(OGIresLPC.init PARAMS)\n\
  Initialize and set parameters of resLPC synthesizer. \n\
   PARAMS is an assoc list of parameter name and value.");

     init_subr_1("OGIresLPC.init++",OGIresLPC_AddInit,
    "(OGIresLPC.init++ PARAMS)\n\
  Add to initialization parameters of resLPC synthesizer \n\
  without destroying existing settings. PARAMS is an assoc list \n\
  of parameter name and value.");

    init_subr_1("OGIresLPC.synth",OGIresLPC_Synthesize,
    "(OGIresLPC.synth UTT)\n\
  Synthesize a waveform using the currently selected resLPC database.");

    init_subr_1("OGIresLPC.lpc_analysis",OGIresLPC_LPC_Analysis,
    "(OGIresLPC.lpc_analysis PARAMS)\n\
   Do LPC analysis on speech files to build an OGIresLPC database. \n\
   PARAMS is an assoc list of parameter name and value.");

    init_subr_1("OGIresLPC.pmark_analysis",OGIresLPC_Pmark_Analysis,
    "(OGIresLPC.pmark_analysis PARAMS)\n\
   Do pitchmark analysis on speech/laryngograph files to build an OGIresLPC database. \n\
   PARAMS is an assoc list of parameter name and value.");

    init_subr_2("OGIresLPC.resynth",OGIresLPC_Resynth,
    "(OGIresLPC.resynth UTT FILELIST)\n\
  Resynthesize wavefile using prosodic modification\n\
  info in UTT.  Must have set Segment, Target, and SrcSeg relations. \n\
  FILELIST is an assoc list of keys and associated filenames.");
}

