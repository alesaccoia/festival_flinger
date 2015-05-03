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

#ifndef __RESLPC_H__
#define __RESLPC_H__

#include "EST.h"
#include "OGI_Track.h"
#include "OGI_SingleTrack.h"
#include "OGI_TimeWarp.h"
#include "OGI_WaveChunk.h"
#include "OGI_Macro.h"


//////////////////////////////////////////////////////////////
/// main container objects
//////////////////////////////////////////////////////////////

// resLPC units brought in from database
class OGIresLPC_SRC {
private:
  //
public:
  OGI_Track pm;   // voiced (input) pitchmarks only
  OGI_Track lpc;   // pitch-sync LPC and energy (indpt of pm)
  OGI_WaveChunk exc; // (exc) waves stored in unconcat form
  EST_IList Bvsect; // begin voiced sections (in pmark indices)
  EST_IList Evsect; // end voiced sections (in pmark indices)
                    //  "end" is the last voiced frame, not 1st UV
  float Fs;  // avoid passing this everywhere

  // constructor
  OGIresLPC_SRC(){
    Bvsect.clear();
    Evsect.clear();
  }
  ~OGIresLPC_SRC(){
    
  }
  
};



// resLPC target data (modified src) 
class OGIresLPC_MOD {
private:
  //
public:
  OGI_Track pm;           // output speech pmarks, mappings to src 
  OGI_SingleTrack F0targ; // F0 targets for the utt
  OGI_TimeWarp warp;

  OGIresLPC_MOD(){

  }
  ~OGIresLPC_MOD(){

  }
};

#endif

