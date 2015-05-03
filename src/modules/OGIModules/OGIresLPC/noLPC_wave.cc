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
#include "resLPC_vqual.h"
#include "resLPC_wave.h"
#include "OGI_Wave.h"
#include "OGI_Track.h"
#include "OGI_Macro.h"
#include "OGI_Vector.h"
#include "OGI_LPC.h"

//
// Time-domain modification of speech
//   Does not currently allow energy interpolation
// 
void noLPC_wave_synth(
		       OGIresLPC_SRC *src, 
		       OGIresLPC_MOD *mod, 
		       OGI_Wave *output_wave){
  ////////////////////////////////////////////////////////////
  //  SETUP
  ////////////////////////////////////////////////////////////
  if (mod->pm.num_frames() == 0)
    return;  // nothing to do

  int k;
  float G=1.0;
  bool clipflag = FALSE;
  EST_Features *gl = get_resLPC_params();
  float EXCmap_k, V_UV_k;
  float Fs = src->Fs;
  float post_gain = gl->F("post_gain");
  float uv_gain   = gl->F("uv_gain");
  OGIwin_t wintype = get_window_type(gl->S("window_type"));

  // intermediate signal buffers
  int maxbuf = (int) (2 * src->pm.max_T0()*Fs + 1);
  maxbuf = max(maxbuf, (int)(2 * mod->pm.max_T0()*Fs + 1));  // size
  maxbuf = max(maxbuf, (int)((mod->warp.last_targ_end()-mod->pm.last_t())*Fs + 0.5));

  OGI_PitchSyncBuffer spbuf(maxbuf);   // speech pulse
  OGI_PitchSyncBuffer olabuf(maxbuf);  // overlap-add result
  
  // window
  OGI_PitchSyncBuffer window(maxbuf);

  // sanity check
  if (!(mod->pm.has_channel("exc_map")))
    EST_error("pmark mapping tracks not present! \n");
  if (!(mod->pm.has_channel("v/uv")))
    EST_error("v/uv track not present! \n");

  // subtracks
  EST_Track EXCmap;
  mod->pm.sub_track(EXCmap, 0, EST_ALL, mod->pm.channel_position("exc_map"), 1);
  EST_Track V_UV;
  mod->pm.sub_track(V_UV, 0, EST_ALL, mod->pm.channel_position("v/uv"), 1);

  // output wave buffer
  int outbuf_incr = (int) max(100, gl->I("outbuf_incr",300));
  output_wave->set_incr(outbuf_incr);

  // setup voice quality transformations
  bool DO_vqual_mod = setup_vqual_mods(gl);

  ////////////////////////////////////////////////////////////
  //   SYNTHESIS
  ////////////////////////////////////////////////////////////

 
  // fundamental periods to left and right of current ts
  int T0_right, T0_left = 0;
  if (mod->pm.num_frames() > 0)
    T0_left = (int)(mod->pm.t(0)*Fs + 0.5);

  // set size of output buffer for 0th frame
  olabuf.clear();
  olabuf.set_mid_end(0, T0_left);

  int ts_int = 0;
  int next_ts_int = 0;

  for (k=0; k<mod->pm.num_frames(); k++){ 

    // fundamental period to right of current pmark
   if (k < mod->pm.num_frames()-1){
      next_ts_int = (int)(mod->pm.t(k+1)*Fs + 0.5);
      T0_right = next_ts_int - ts_int;
   }
    else
      T0_right = (int)(mod->warp.last_targ_end()*Fs + 0.5) - ts_int; 

    // index mappings
    EXCmap_k = EXCmap.a_no_check(k);
    V_UV_k   = V_UV.a_no_check(k);

    // extract excitation pulse
    extract_exc_pulse(EXCmap_k, src->exc, src->pm, spbuf, T0_left, T0_right, Fs);

    // transform wave
    if (DO_vqual_mod)
      vqual_mod_wave(spbuf, Fs/T0_right, V_UV_k);


    // create OLA windows
    window.set_mid_end(spbuf.mid(), spbuf.end());
    create_window(window, wintype);
    
    // apply gain factor
    G = 1.0;
    if (V_UV_k == _UV_)
      G *= uv_gain;    // user-specified gain for unvoiced
    G *= post_gain;    // user-specified overall gain 

    // add first half of new pulse to output buffer, apply window, gain
    add_new_pulse(spbuf, window, G, olabuf);
    
    // clip, quantize, copy outbuf to final output vector
    clipflag = clipflag | output_wave->append_clip(olabuf.buf, olabuf.end());

    // set size of output buffer for NEXT frame
    olabuf.set_mid_end(0, T0_right);

    // copy second half of old pulse to empty buffer, apply window, gain
    copy_old_pulse(spbuf, window, G, olabuf);

    // move ahead one pitchmark
    T0_left = T0_right;    
    ts_int = next_ts_int;
  }
  
  // clip, quantize, copy outbuf to final output vector
  clipflag = clipflag | output_wave->append_clip(olabuf.buf, olabuf.end());

  // free tmp mem
  src->exc.end_fast_get();

}

//// 
//// void noLPC_wave_synth(
//// 			    OGIresLPC_SRC *src, 
//// 			    OGIresLPC_MOD *mod, 
//// 			    OGI_Wave *output_wave){
////   int k;
////   float G=1.0;
////   bool clipflag = FALSE;
////   
////   EST_Features *gl = get_resLPC_params();
////   float Fs = src->Fs;
////   float post_gain = gl->F("post_gain");
////   OGIwin_t wintype = get_window_type(gl->S("window_type"));
//// 
////   // intermediate signal buffers
////   int maxbuf = (int) (2 * src->pm.max_T0()*Fs + 1);
////   maxbuf = max(maxbuf, (int)(2 * mod->pm.max_T0()*Fs + 1));  // size
////   maxbuf = max(maxbuf, (int)((mod->warp.last_targ_end()-mod->pm.last_t())*Fs + 0.5));
//// 
////   OGI_PitchSyncBuffer spbuf(maxbuf);   // speech pulse
////   OGI_PitchSyncBuffer olabuf(maxbuf);  // overlap-add result
////   
////   // window
////   OGI_PitchSyncBuffer window(maxbuf);
//// 
////   // subtracks
////   EST_Track EXCmap;
////   mod->pm.sub_track(EXCmap, 0, EST_ALL, mod->pm.channel_position("exc_map"), 1);
//// 
////   // output wave buffer
////   int outbuf_incr = (int) max(300, gl->I("outbuf_incr"));
////   output_wave->set_incr(outbuf_incr);
//// 
////   // fundamental periods to left and right of current ts
////   int T0_right;
////   int T0_left = (int)(mod->pm.t(0)*Fs + 0.5);
//// 
////   // set size of output buffer for 0th frame
////   olabuf.clear();
////   olabuf.set_mid_end(0, T0_left);
//// int next_ts_int=0;
//// int ts_int=0;
//// 
////   for (k=0; k<mod->pm.num_frames(); k++){ 
//// 
//// 	 // fundamental period to right of current pmark
//// 	 next_ts_int = (int)(mod->pm.t(k+1)*Fs + 0.5);
//// 	 if (k < mod->pm.num_frames()-1)
//// 	   T0_right = next_ts_int - ts_int;
//// 	 else
//// 	   T0_right = (int)(mod->warp.last_targ_end()*Fs + 0.5) - ts_int; 
//// 
//// 	 // extract excitation pulse
//// 	 extract_exc_pulse(EXCmap(k), src->exc, src->pm, spbuf, T0_left, T0_right, Fs);
//// 
//// 	 // create OLA windows
//// 	 window.set_mid_end(spbuf.mid(), spbuf.end());
//// 	 create_window(window, wintype);
//// 	 
//// 	 // add first half of new pulse to output buffer, apply window, gain
//// 	 add_new_pulse(spbuf, window, post_gain, olabuf);
//// 	 
//// 	 // clip, quantize, copy outbuf to final output vector
//// 	 clipflag = clipflag | output_wave->append_clip(olabuf.buf, olabuf.end());
//// 
//// 	 // set size of output buffer for NEXT frame
//// 	 olabuf.set_mid_end(0, T0_right);
//// 
//// 	 // copy second half of old pulse to empty buffer, apply window, gain
//// 	 copy_old_pulse(spbuf, window, G, olabuf);
//// 
//// 	 // move ahead one pitchmark
//// 	 T0_left = T0_right;    
//// 	 ts_int = next_ts_int;
////   }
////   
////   // clip, quantize, copy outbuf to final output vector
////   clipflag = clipflag | output_wave->append_clip(olabuf.buf, olabuf.end());
//// 
//// }
//// 
