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
#include "resLPC.h"
#include "resLPC_io.h"
#include "resLPC_wave.h"
#include "resLPC_vqual.h"
#include "OGI_Wave.h"
#include "OGI_WaveChunk.h"
#include "OGI_Track.h"
#include "OGI_Macro.h"
#include "OGI_Vector.h"
#include "OGI_Filter.h"
#include "OGI_LPC.h"


///      
///      (set! mwmdi_vqual
///            '(vqual_mod
///      	 ((vt_global_warp_wave 1.0)
///      	  (vt_voiced_warp_wave 0.94)
///      	  (vt_global_warp_lsf 1.0)
///      	  (vt_voiced_warp_lsf 1.0)
///      	  )))


static float VQUAL_global_warp_wave;
static float VQUAL_v_warp_wave;
static float VQUAL_global_warp_lsf;
static float VQUAL_v_warp_lsf;

static void resample_warp(OGI_PitchSyncBuffer &spbuf, float factor);

bool setup_vqual_mods(EST_Features *gl){

  LISP params = NIL;
  if (gl->present("vqual_mod")){
    params = read_from_lstring(strintern(gl->S("vqual_mod")));
    
    VQUAL_global_warp_wave = get_param_float("vt_global_warp_wave",params, 1.0);
    VQUAL_v_warp_wave = get_param_float("vt_voiced_warp_wave",params, 1.0);

    VQUAL_global_warp_lsf  = get_param_float("vt_global_warp_lsf", params, 1.0);
    VQUAL_v_warp_lsf  = get_param_float("vt_voiced_warp_lsf", params, 1.0);

    return TRUE;
  }

  return FALSE;
}



// transform wave
void vqual_mod_wave(OGI_PitchSyncBuffer &spbuf, float F0, float v_uv){

  float v_warp  = VQUAL_global_warp_wave * VQUAL_v_warp_wave;
  float uv_warp = VQUAL_global_warp_wave;

  // add F0-dependent scaling here
  if (v_uv > 0.5) 
    resample_warp(spbuf, v_warp);
  else
    resample_warp(spbuf, uv_warp);
}


// transform wave
void vqual_mod_lsf(OGI_VectDouble &lsfvect, float F0, float v_uv){

}


static void resample_warp(OGI_PitchSyncBuffer &spbuf, float factor){
  /// could rewrite this to NOT realloc each time -- just use scratchpad memory
  /// alloced ahead of time...

  if (factor == 1.0)
    return;

  OGI_PitchSyncBuffer *outbuf = new OGI_PitchSyncBuffer(spbuf.Nbuf());
  outbuf->set_mid_end(spbuf.mid(),spbuf.end());
  outbuf->clear();

  int t_int,k;
  float g,t;
  for (k=outbuf->mid(), t=(float)spbuf.mid(); k<outbuf->end(); t+=factor,k++){
    t_int = (int) t;
    g = t - t_int;
    outbuf->buf[k] = (1-g)*spbuf.buf[t_int] + g*spbuf.buf[t_int+1]; 

    if (t >= spbuf.end()-1)
      break;
  }
  
  for (k=outbuf->mid(), t=(float)spbuf.mid(); k>0; t-=factor,k--){
    t_int = (int) t;
    g = t - t_int;
    outbuf->buf[k] = (1-g)*spbuf.buf[t_int] + g*spbuf.buf[t_int+1]; 

    if (t < 0)
      break;
  }
  
  // copy and return, outbuf is freed
  spbuf.copy_from(*outbuf);
}
