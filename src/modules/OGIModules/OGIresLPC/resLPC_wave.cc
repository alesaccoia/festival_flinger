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

// Voice Conversion
#include "resLPC_vc.h" // Gmm for voice conversion, Alexander Kain, 1998

static void interp_lsf(float map, EST_Track &LSF, OGI_VectDouble &lsf);
static void deemphasize(OGI_PitchSyncBuffer &spbuf, float deemph);
static float pulse_power_normalize(OGI_PitchSyncBuffer &spbuf, EST_Track &E, float map, OGIwin_t wintype);
static void convolve_exc_lpc( OGI_PitchSyncBuffer &excbuf,OGI_VectDouble &lpc, OGI_PitchSyncBuffer &spbuf);
static void add_LPC_channels(OGI_Track &a, int N);
static void add_LSF_channels(OGI_Track &a, int N);
static void dither_uv( OGI_PitchSyncBuffer &spbuf,  EST_Track &EXCmap, EST_Track &V_UV, int k);

//
// Time-domain modification of residual and LPC filtering both at once
// 
void resLPC_wave_synth(
		       OGIresLPC_SRC *src, 
		       OGIresLPC_MOD *mod, 
		       OGI_Wave *output_wave){

  ////////////////////////////////////////////////////////////
  //  SETUP
  ////////////////////////////////////////////////////////////
  if (mod->pm.num_frames() == 0)
    return;  // nothing to do

  // facility to dump every pitchmark pulse to a separate file,
  //  plus LSF, LPC, gain coeff
  bool DUMP_pulses=FALSE;
  EST_Features *gl = get_resLPC_params();
  LISP dumplist = read_from_lstring(strintern(gl->S("dump","NIL")));
  if (dumplist != NIL){
    if (get_param_int("pulses", dumplist, 0)){
      DUMP_pulses = TRUE;
    }
  }

  bool DO_dither = gl->I("uv_dither") ? true : false;

  int k;
  float G=1.0;
  bool clipflag = FALSE;

  float LPCmap_k, EXCmap_k, V_UV_k;
  float Fs = src->Fs;
  float post_gain = gl->F("post_gain");
  float uv_gain   = gl->F("uv_gain");
  float deemph    = gl->F("deemphasis");
  OGIwin_t wintype = get_window_type(gl->S("window_type"));

  // intermediate signal buffers
  int maxbuf = (int) (2 * src->pm.max_T0()*Fs + 1);
  maxbuf = max(maxbuf, (int)(2 * mod->pm.max_T0()*Fs + 1));  // size
  maxbuf = max(maxbuf, (int)((mod->warp.last_targ_end()-mod->pm.last_t())*Fs + 0.5));

  OGI_PitchSyncBuffer excbuf(maxbuf);  // excitation
  OGI_PitchSyncBuffer spbuf(maxbuf);   // speech pulse
  OGI_PitchSyncBuffer olabuf(maxbuf);  // overlap-add result
  
  // window
  OGI_PitchSyncBuffer window(maxbuf);

  // sanity check
  if (!(src->lpc.has_channel("LSF_N") && src->lpc.has_channel("LSF_0")))
    EST_error("LSF tracks not present! \n");
  if (!(src->lpc.has_channel("E")))
    EST_error("Energy track not present! \n");
  if (!(mod->pm.has_channel("lpc_map") && mod->pm.has_channel("exc_map")))
    EST_error("pmark mapping tracks not present! \n");
  if (!(mod->pm.has_channel("v/uv")))
    EST_error("v/uv track not present! \n");

  if (DUMP_pulses){ ////////////////////////////// debug//////////////////////
    add_LPC_channels(mod->pm, src->lpc.channel_position("LSF_N")-src->lpc.channel_position("LSF_0")+2);
    add_LSF_channels(mod->pm, src->lpc.channel_position("LSF_N")-src->lpc.channel_position("LSF_0")+1);
  }

  // subtracks
  EST_Track LSF;
  int c0 = src->lpc.channel_position("LSF_0");
  src->lpc.sub_track(LSF, 0, EST_ALL, c0, src->lpc.channel_position("LSF_N") - c0 + 1);  
  EST_Track E;
  src->lpc.sub_track(E, 0, EST_ALL, src->lpc.channel_position("E"), 1);  
  EST_Track LPCmap;
  mod->pm.sub_track(LPCmap, 0, EST_ALL, mod->pm.channel_position("lpc_map"), 1);  
  EST_Track EXCmap;
  mod->pm.sub_track(EXCmap, 0, EST_ALL, mod->pm.channel_position("exc_map"), 1);
  EST_Track V_UV;
  mod->pm.sub_track(V_UV, 0, EST_ALL, mod->pm.channel_position("v/uv"), 1);

  EST_Track LPCout;   /////////////////debug//////////////////////
  EST_Track LSFout;
  EST_Track Eout;
  EST_String fname;            
  int l;
  if (DUMP_pulses){
    c0 = mod->pm.channel_position("LPC_0");
    mod->pm.sub_track(LPCout, 0, EST_ALL, c0, mod->pm.channel_position("LPC_N") - c0 + 1);  
    c0 = mod->pm.channel_position("LSF_0");
    mod->pm.sub_track(LSFout, 0, EST_ALL, c0, mod->pm.channel_position("LSF_N") - c0 + 1);   
    mod->pm.sub_track(Eout, 0, EST_ALL, mod->pm.channel_position("Elsf"), 1);  
  }  ////////////////////////////////////////////////////////////////

  // lpc coefficients (1 more than # of LSF)
  OGI_VectDouble lpc(LSF.num_channels()+1);
  OGI_VectDouble lsfvect(LSF.num_channels());

  // lsf to lpc conversion object
  OGI_LSF_to_LPC LSF_to_LPC(LSF.num_channels());

  // output wave buffer
  int outbuf_incr = (int) max(100, gl->I("outbuf_incr",300));
  output_wave->set_incr(outbuf_incr);

  // setup voice quality transformations
  bool DO_vqual_mod = setup_vqual_mods(gl);

  ////////////////////////////////////////////////////////////
  //   SYNTHESIS
  ////////////////////////////////////////////////////////////

  // optional voice conversion
  if(gl->I("doVC",0))
    vc_do(LSF, V_UV);

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
    LPCmap_k = LPCmap.a_no_check(k); 
    EXCmap_k = EXCmap.a_no_check(k);
    V_UV_k   = V_UV.a_no_check(k);

    // interpolate between nearest LSFs
    interp_lsf(LPCmap_k, LSF, lsfvect);

    // transform filter coefficients
    if (DO_vqual_mod)
      vqual_mod_lsf(lsfvect, Fs/T0_right, V_UV_k);

    // create new LPC vector from [mapped] LSF    
    LSF_to_LPC.convert(lsfvect, lpc);

    if (DUMP_pulses){ /////////////////debug//////////////////////
      for (l=0; l<LSF.num_channels(); l++)
	LSFout.a(k,l) = (float)lsfvect.a[l];    // store interpolated LSFs 
      for (l=0; l<LSF.num_channels()+1; l++)
	LPCout.a(k,l) = (float)lpc.a[l];        // store interpolated LPCs 
    }
    
    // extract excitation pulse
    extract_exc_pulse(EXCmap_k, src->exc, src->pm, excbuf, T0_left, T0_right, Fs);

    // convolve excitation with LPC
    convolve_exc_lpc(excbuf, lpc, spbuf);

    if (DUMP_pulses){ /////////////////debug//////////////////////
      fname = "exc" + itoString(k) + ".wav"; 
      // Modified by Qi Miao for debug
      //excbuf.save(fname, Fs, 1.0);
      excbuf.save(fname, (int)Fs, 1.0);
      fname = "sp" + itoString(k) + ".wav"; 
      //spbuf.save(fname, Fs, 1.0);
      spbuf.save(fname, (int)Fs, 1.0);
    }

    // transform wave
    if (DO_vqual_mod)
      vqual_mod_wave(spbuf, Fs/T0_right, V_UV_k);

    // apply de-emphasis filter
    deemphasize(spbuf, deemph);

    if (DUMP_pulses){ /////////////////debug//////////////////////
      fname = "d" + itoString(k) + ".wav";
      // Modified by Qi Miao for debug
      //spbuf.save(fname, Fs, 1.0);
      spbuf.save(fname, (int)Fs, 1.0);
    }

    // create OLA windows
    window.set_mid_end(spbuf.mid(), spbuf.end());
    create_window(window, wintype);
    
    // measure signal power in pulse, return gain factor
    G = pulse_power_normalize(spbuf, E, LPCmap_k, wintype);

    if (V_UV_k == _UV_)
      G *= uv_gain;    // user-specified gain for unvoiced
    G *= post_gain;    // user-specified overall gain 

    if (DUMP_pulses) /////////////////debug//////////////////////
       Eout.a(k) = G;    // store output GAIN coeff (not E!!!)

    if ((DO_dither) & (V_UV_k == _UV_))
      dither_uv(spbuf, EXCmap, V_UV, k);

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


//
// generate only the interpolated LSF coefficients, put into mod->pm track
// 
void resLPC_LSFonly(
		    OGIresLPC_SRC *src, 
		    OGIresLPC_MOD *mod){
  int k,l,indxL,indxR;  
  EST_Features *gl = get_resLPC_params();
  float Fs = src->Fs;
  double g;

  float EXCmap_k,LPCmap_k;

  // subtracks
  EST_Track LSF;
  int c0 = src->lpc.channel_position("LSF_0");
  src->lpc.sub_track(LSF, 0, EST_ALL, c0, src->lpc.channel_position("LSF_N") - c0 + 1);  
  EST_Track E;
  src->lpc.sub_track(E, 0, EST_ALL, src->lpc.channel_position("E"), 1);  
  EST_Track LPCmap;
  mod->pm.sub_track(LPCmap, 0, EST_ALL, mod->pm.channel_position("lpc_map"), 1);  
  EST_Track EXCmap;
  mod->pm.sub_track(EXCmap, 0, EST_ALL, mod->pm.channel_position("exc_map"), 1);

  // lpc coefficients (1 more than # of LSF)
  OGI_VectDouble lpc(LSF.num_channels()+1);
  OGI_VectDouble lsfvect(LSF.num_channels());

  // lsf to lpc conversion object
  OGI_LSF_to_LPC LSF_to_LPC(LSF.num_channels());

  // optional voice conversion
  EST_Track V_UV;
  if(gl->I("doVC",0))
    vc_do(LSF, V_UV);

  // add extra channels to mod object
  add_LSF_channels(mod->pm, LSF.num_channels());
  EST_Track LSFout;
  c0 = mod->pm.channel_position("LSF_0");
  mod->pm.sub_track(LSFout, 0, EST_ALL, c0, mod->pm.channel_position("LSF_N") - c0 + 1);  
  EST_Track Eout;
  mod->pm.sub_track(Eout, 0, EST_ALL, mod->pm.channel_position("Elsf"), 1);  

  // fundamental periods to left and right of current ts
  int T0_right;
  int T0_left = (int)(mod->pm.t(0)*Fs + 0.5);

  for (k=0; k<mod->pm.num_frames(); k++){ 
    
    // fundamental period to right of current pmark
   if (k < mod->pm.num_frames()-1)
      T0_right = (int)((mod->pm.t(k+1)-mod->pm.t(k))*Fs + 0.5);
    else
      T0_right = (int)((mod->warp.last_targ_end()-mod->pm.t(k))*Fs + 0.5); 

    // index mappings
    LPCmap_k = LPCmap.a_no_check(k); 
    EXCmap_k = EXCmap.a_no_check(k);

    // interpolate between nearest LSFs
    interp_lsf(LPCmap_k, LSF, lsfvect);

    // store interpolated LSFs     
    for (l=0; l<LSF.num_channels(); l++)
      LSFout(k,l) = (float)lsfvect.a[l];
    
    // store interpolated target power   
    indxL = (int) LPCmap_k;
    indxR = min(E.num_frames()-1, (int) (LPCmap_k+1));
    g = LPCmap_k-indxL;
    Eout(k) = (float)((1-g)*E(indxL) + g*E(indxR));

    // move ahead one pitchmark
    T0_left = T0_right;    
  }
}


static void interp_lsf(
		float map, 
		EST_Track &LSF, 
		OGI_VectDouble &lsfvect){
  // fetch LSF vector (with interpolation between closest pair) 
  int indxL = (int) map;
  int indxR = min(LSF.num_frames()-1, indxL+1);
  double g = map-indxL;
  double omg = 1-g;

  int k;

  for (k=0; k<LSF.num_channels(); k++){
    lsfvect.a[k] = (omg)*LSF.a_no_check(indxL, k) + g*LSF.a_no_check(indxR, k);
  }
}

static void convolve_exc_lpc(
		      OGI_PitchSyncBuffer &excbuf, 
		      OGI_VectDouble &lpc, 
		      OGI_PitchSyncBuffer &spbuf){

  // nice debug tool.
  //       spbuf.copy_from(excbuf);  
  spbuf.set_mid_end(excbuf.mid(), excbuf.end());
  ar_convolve(excbuf.buf, lpc.a, lpc.N, spbuf.buf, spbuf.end());

}

void create_window(
		    OGI_PitchSyncBuffer &window, 
		    OGIwin_t wintype){

  int k,N=0,b;
  double n, incr, *win;
  win = &(window.buf[0]);
  window.clear();

  if (wintype == w_triangle){
  
    // positive-going side 
    N = window.mid();
    n = 0.0;
    if (N > 0){
      incr = 1.0/N;
      for (k=0; k<N; k++){
	*win++ = n;
	n += incr;
      }  
    }

    // negative-going side
    N = window.end() - window.mid();
    n = 1.0;
    if (N > 0){
      incr = -1.0/N;
      for (k=0; k<N; k++){
	*win++ = n;
	n += incr;
      }  
    }
  }
  else if (wintype == w_trapezoid){
    // positive-going side 
    N = window.mid()/2; 
    if (N > 0){
      b = window.mid() - N;
      n = 0.0;
      incr = 1.0/N;
      win = &(window.buf[b]);
      for (k=b; k<b+N; k++){
	*win++ = n;
	n += incr;
      }  
    }

    // flat part
    b = window.mid();
    N =  window.end() - (window.end() - window.mid())/2;
    win = &(window.buf[b]);
    for (k=b; k<N; k++){
      *win++ = 1.0;
    }  
    
    // negative-going side
    N = (window.end() - window.mid())/2;
    b = window.end() - N;
    n = 1.0;
    if (N > 0){
      incr = -1.0/N;
      win = &(window.buf[b]);
      for (k=b; k<b+N; k++){
	*win++ = n;
	n += incr;
      }  
    }
  }
  else {
    EST_error("unknown window type\n");  
  }

}

void copy_old_pulse(
	       OGI_PitchSyncBuffer &spbuf, 
	       OGI_PitchSyncBuffer &win, 
	       float G, 
	       OGI_PitchSyncBuffer &olabuf
	       ){
  // copy 2nd half of old spbuf pulse to empty olabuf buffer, apply window, gain
  int Nola = min(olabuf.end(), spbuf.end()-spbuf.mid());
  int k_ol = 0;
  int k_sp = spbuf.mid();

  olabuf.clear();
  while (k_ol<Nola){
    olabuf.buf[k_ol] = spbuf.buf[k_sp] * G * win.buf[k_sp];
    k_ol++; k_sp++;
  }

}

void add_new_pulse(
	  OGI_PitchSyncBuffer &spbuf, 
	  OGI_PitchSyncBuffer &win, 
	  float G, 
	  OGI_PitchSyncBuffer &olabuf
	  ){
  // add 1st half of new (spbuf) pulse to output buffer, apply window, gain
  int Nola = max(spbuf.mid(), olabuf.end());
  int k_ol = max(0, olabuf.end()-spbuf.mid());
  int k_sp = max(0, spbuf.mid()-olabuf.end());

  while (k_ol < Nola){
    olabuf.buf[k_ol] += spbuf.buf[k_sp] * G * win.buf[k_sp];
    k_ol++; k_sp++;
  }
}


void extract_exc_pulse(
			      float map, 
			      OGI_WaveChunk &exc, 
			      OGI_Track &srcpm, 
			      OGI_PitchSyncBuffer &excbuf,
			      int T0_sL,
			      int T0_sR,
			      float Fs)
{
  int sindx = (int) (map + 0.5); // no interpolation
  int ta = (int) (srcpm.t(sindx)*Fs + 0.5);  ///<<< rounding occurs here
  int T0_aL, T0_aR; 
  
  // T0 around analysis windows
  T0_aL = (int)(srcpm.T0(sindx-1) * Fs + 0.5); 
  T0_aR = (int)(srcpm.T0(sindx)   * Fs + 0.5);
  
  // VP T0_aL= min(T0_aL, T0_sL);
  // VP T0_aR= min(T0_aR, T0_sR);
  
  // shorten exc window if raising pitch
  if (T0_sL < T0_aL)
    T0_aL= T0_sL;

  if (T0_sR < T0_aR)
    T0_aR = T0_sR;
  
  // get exc pulse: back T0_aL and fwd T0_aR
  // VP exc.get_fast(ta, T0_aL, T0_aR, T0_sL, T0_sR, excbuf);

  exc.get_fast(ta, T0_aL, T0_aR, T0_sL, T0_sR, excbuf);
}

static void deemphasize(OGI_PitchSyncBuffer &spbuf, float a){
  double b = (1-a);
  double ym1 = 0.0;  // y[n-1]
  double *y = spbuf.buf;
  int len = spbuf.end();

  for ( ; len > 0; len--){
    *y = b*(*y) + a*ym1;
    ym1 = *y;
    y++;  
  }
}

static float pulse_power_normalize(
	       OGI_PitchSyncBuffer &spbuf, 
	       EST_Track &E, 
	       float map,
	       OGIwin_t wintype){

  int indxL = (int) map;
  int indxR = min(E.num_frames()-1, (int) (map+1));
  double g = map-indxL;

  // powers are all RMS in log domain
  double Ps, P_out;

  // measure power in spbuf
  if (wintype == w_trapezoid)
    Ps = spbuf.RMSpower_trapezoid();    
  else
    Ps = spbuf.RMSpower_triangle();

  // find (interpolated) target power   
  P_out = (1-g)*E.a_no_check(indxL) + g*E.a_no_check(indxR);

  // return gain factor needed to make power_in == power_out
  return (float)exp(P_out - Ps);
}


OGIwin_t get_window_type(const EST_String &sval){
  if (sval == "trapezoid")           return w_trapezoid;
  else if (sval == "triangle")         return w_triangle;
  else if (sval == "")        return w_triangle; // default
  else {
    EST_error("resLPC_wave: Unknown window type \"" + sval + "\"\n");
    return w_triangle; // dummy
  }
}


static void add_LPC_channels(OGI_Track &tr, int N){
 
  int k;
  EST_String name;
  EST_String base = "LPC_";
  int offset = tr.num_channels();
  tr.resize(tr.num_frames(), tr.num_channels() + N + 1);

  // LPC
  for (k=0; k<N-1; k++){ 
    name = base + itoString(k);
    tr.set_channel_name(name, k+offset);
  }
  name = base + "N";
  tr.set_channel_name(name, k+offset);
  k++;

  // E
  name = "Elpc";
  tr.set_channel_name(name, k+offset);
}

static void dither_uv(  
		      OGI_PitchSyncBuffer &spbuf, 
		      EST_Track &EXCmap, 	
		      EST_Track &V_UV, 
		      int k){
  
  if (k > 0){
    int pma = (int) (EXCmap.a_no_check(k) + 0.5);
    int l;
    for (l=k; l > 0; l--){
      if ((int)(EXCmap.a_no_check(l) + 0.5) != pma)
	break;
    }
    if (fmod(k-l-1, 2.0)){ // if odd
      spbuf.reverse();
    }
  }
}

static void add_LSF_channels(OGI_Track &tr, int N){
 
  int k;
  EST_String name;
  EST_String base = "LSF_";
  int offset = tr.num_channels();
  tr.resize(tr.num_frames(), tr.num_channels() + N + 1);

  // LSF
  for (k=0; k<N-1; k++){ 
    name = base + itoString(k);
    tr.set_channel_name(name, k+offset);
  }
  name = base + "N";
  tr.set_channel_name(name, k+offset);
  k++;

  // E
  name = "Elsf";
  tr.set_channel_name(name, k+offset);
}

