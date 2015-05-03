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
#include "EST.h"
#include "EST_Pathname.h"
#include "OGI_Track.h"
#include "OGI_Macro.h"
#include "OGI_Filter.h"
#include "OGI_Buffer.h"
#include "OGI_Vector.h"
#include "OGI_Wave.h"
#include "OGI_LPC.h"
#include "resLPC_wave.h"
#include "resLPC_pmark.h"
#include "resLPC_analysis.h"

static void parse_init_params(LISP params, EST_Features &In);
static void inverse_filter_lpc( OGI_PitchSyncBuffer &spbuf, OGI_VectDouble &lpc, OGI_PitchSyncBuffer &excbuf);
/* not used: static void max_normalize(EST_Wave &sig);*/
static void bwexp_lpc(OGI_Track &lpc,EST_Features &g);
static void deemph(OGI_PitchSyncBuffer &olabuf, float a);

bool lpc_debug_dump  = 0;
bool lpc_debug_print = 0;

// can use (directory-entries ".") to get file list when called in scm

LISP OGIresLPC_LPC_Analysis(LISP params){
  EST_Features g;
  parse_init_params(params, g);
  LISP l;
  
  LISP lfiles = get_param_lisp("infile_list", params, NIL);
 
if (lfiles == NIL){
    EST_error("OGIresLPC pmark analysis:  infile_list empty - no input files to process");
  }


  lpc_debug_print = (!(g.I("run_quietly")));
  lpc_debug_dump  = g.I("dump_intermediate_files") ? true : false;

  EST_Pathname f,spfile,pmfile,lpc_outfile,res_outfile;
  EST_String basename;
  EST_String sp_path  = wstrdup(g.S("sp_path"));
  EST_String pm_path  = wstrdup(g.S("pm_path"));
  EST_String out_path = wstrdup(g.S("out_path"));
  EST_String res_ext  = wstrdup(g.S("res_ext"));
  EST_String lpc_ext  = wstrdup(g.S("lpc_ext"));
  EST_String sp_ext   = wstrdup(g.S("sp_ext"));
  EST_String pm_ext   = wstrdup(g.S("pm_ext"));

  for (l=lfiles; l != 0; l=cdr(l)){
    bool files_loaded = TRUE;
    f = EST_Pathname(get_c_string(car(l)));
    EST_String ff = f.basename(0);
 
    basename = "";
    while(ff.contains(".")){
      basename += ff.before(".") + ".";
      ff = ff.after(".");
    }
    basename = basename.before(basename.length()-1);

    pmfile  = pm_path + basename + pm_ext;
    spfile  = sp_path + basename + sp_ext;
    lpc_outfile = out_path   + basename + lpc_ext;
    res_outfile = out_path   + basename + res_ext;
    if (lpc_debug_print) cout << " loading " << pmfile << " --> output to " << res_outfile << "  " << lpc_outfile << endl;

    EST_Wave sp;    
    OGI_Track lpcpm;
    if (lpcpm.load(pmfile) != read_ok){
      cout << "Could not load pm file " << pmfile << "  ... skipping." << endl;
      files_loaded = FALSE;
    }
    if (sp.load(spfile) != read_ok){
      cout << "Could not load speech file " << spfile << "  ... skipping." << endl;
      files_loaded = FALSE;
    }

    if (files_loaded){
      //
      // do it
      //
      fill_UV_pmarks(lpcpm, sp.length(), (float)sp.sample_rate(), g, 0);  

      OGI_Track tmppm = lpcpm; tmppm.resize(EST_CURRENT, 1);
      compute_log_power(sp, tmppm, g);
      
      compute_lpc(sp, lpcpm, g);
      lpcpm.copy_channel_in(0, tmppm, 0);   // copy over energy channel

      // bandwidth expansion of LPCs
      if (g.present("bw_expansion"))
	bwexp_lpc(lpcpm, g);
	
      // convert to and from LSFs to account for quantizer
      EST_Track lsf;
      LPC_to_LSF_convert(lpcpm, lsf);
      /////// quantize/unquantize here ///////////
      lpcpm.resize(0,0);
      LSF_to_LPC_convert(lsf, lpcpm);

      EST_Wave res;
      compute_residual(sp, lpcpm, g, res);

      res.save(res_outfile, g.S("res_format"));

      if (downcase(g.S("lpc_format")) == "lsf"){
	LPC_to_LSF_convert(lpcpm, lsf);
	lsf.save(lpc_outfile);
      }
      else
	lpcpm.save(lpc_outfile);
    }
  }

  return NIL;
}

/* not used 
static void max_normalize(EST_Wave &sig){
  int n;
  int N=sig.length();
  short mm = 1;
  for (n=0; n<N; n++)
    mm = max(abs(sig.a_no_check(n)), mm);

  double s = OGI_MAXSHORT/(double)mm;

  for (n=0; n<N; n++)
    sig.a_no_check(n) =  (short)(s * sig.a_no_check(n));
}
*/

static void bwexp_lpc(
		      OGI_Track &lpc,
		      EST_Features &g){
  int n,k;

  //  lpc channel 0 is energy
  int lpc_ord = lpc.num_channels()-1;

  OGI_VectDouble lpcvect(lpc_ord+1);
  OGI_LPC_bwexp BWexp(lpc_ord, g.F("bw_expansion"));
  
  for (k=0; k<lpc.num_frames(); k++){
    
    // extract LPC vector from track    
    for (n=0; n<=lpc_ord; n++)
      lpcvect.a[n] = lpc.a_no_check(k,n);
    
    BWexp.expand(lpcvect);
    
    // put back in track
    for (n=1; n<=lpc_ord; n++)
      lpc.a_no_check(k,n) = (float) lpcvect.a[n];
  }
}

void compute_lpc(
		 EST_Wave &sp,
		 OGI_Track &lpc,
		 EST_Features &g){

  // in-place --> sp is changed from now on
  pre_emphasis(sp, g.F("pre_emphasis"));

  EST_WindowFunc *wf =  EST_Window::creator("hamming");
  lpc.resize(lpc.num_frames(), g.I("lpc_order")+1);
  sig2coef(sp, lpc, "lpc", g.F("lpc_anl_frame_len"), wf);
  int k,n;

  for (k=0; k<lpc.num_frames(); k++){
    lpc.a_no_check(k,0) = 1.0;
    for (n=1; n<lpc.num_channels(); n++){
      lpc.a_no_check(k,n) = -lpc.a_no_check(k,n); // opposite sign convention
    }
  }
}


void compute_log_power(
		      EST_Wave &sp,
		      OGI_Track &lpc,
		      EST_Features &g){

  // overwrites channel 0 of lpc, with log power estimated 
  //   with same window as used for OLA
  int k;
  int ts_int,next_ts;
  OGIwin_t wintype = get_window_type(g.S("ola_window_type"));
  float Fs = (float) sp.sample_rate();
 
  // intermediate signal buffers
  //int maxbuf = (int) (2 * lpc.max_T0()*Fs + 1);
  int maxbuf = (int) (2 * max(lpc.max_T0()*Fs, sp.length()-lpc.last_t()*Fs) + 1);

 // cerr << "resLPC_analysis.cc: maxbuf: " << maxbuf << endl;
  OGI_PitchSyncBuffer spbuf(maxbuf);   // speech pulse

  // use OGI_WaveChunk for convenience
  OGI_WaveChunk sp_in;
  sp_in.append_chunk(sp, 0, sp.length(), 0);
  
  // fundamental periods to left and right of current ts
  ts_int = (int)(lpc.t(0)*Fs + 0.5);
  next_ts = (int)(lpc.t(1)*Fs + 0.5);
  int T0_right;
  int T0_left = ts_int;

  for (k=0; k<lpc.num_frames(); k++){ 
        
    // fundamental period to right of current pmark
    if (k < lpc.num_frames()-1)
      next_ts = (int)(lpc.t(k+1)*Fs + 0.5);
    else
      next_ts = (int)(sp.length());     
    T0_right = next_ts - ts_int;
    
    // extract 2-pitch period pulse from sp
    sp_in.get(ts_int, T0_left, T0_right, spbuf); 

    // compute log power
    if (wintype == w_trapezoid)
      lpc.a_no_check(k,0) = (float)spbuf.RMSpower_trapezoid();
    else
      lpc.a_no_check(k,0) = (float)spbuf.RMSpower_triangle();

    // update for next iteration
    ts_int = next_ts;
    T0_left = T0_right;	      
  }
}

void compute_residual(
		      EST_Wave &sp,
		      OGI_Track &lpc,
		      EST_Features &g,
		      EST_Wave &res){

  // also overwrites channel 0 of lpc, with log power estimated 
  //   with same window as used for OLA
  int k,n;
  // Modified by Qi Miao for debug, unused bool clipflag
  //bool clipflag = FALSE;

  int ts_int,next_ts;
  OGI_VectDouble lpcvect(lpc.num_channels());
  OGIwin_t wintype = get_window_type(g.S("ola_window_type"));
  float Fs = (float) sp.sample_rate();
  double d_coeff = 0.0;
  bool DO_deemph = FALSE;

  // intermediate signal buffers
  int maxbuf = (int) (2 * max(lpc.max_T0()*Fs, sp.length()-lpc.last_t()*Fs) + 1);
   // cerr << "resLPC_analysis.cc: maxbuf2: " << maxbuf << endl;

  OGI_PitchSyncBuffer excbuf(maxbuf);  // excitation
  OGI_PitchSyncBuffer spbuf(maxbuf);   // speech pulse
  OGI_PitchSyncBuffer olabuf(maxbuf);  // overlap-add result

  if (g.present("deemph_residual")){
    if (g.I("deemph_residual") != 0){
      DO_deemph = TRUE;
      d_coeff = g.F("pre_emphasis");
      olabuf.clear(); // init deemph tap
      olabuf.set_mid_end(0, 2);
      deemph(olabuf, 0.0);
    }
  }
  
  // window
  OGI_PitchSyncBuffer window(maxbuf);

  // fundamental periods to left and right of current ts
  ts_int = (int)(lpc.t(0)*Fs + 0.5);
  next_ts = (int)(lpc.t(1)*Fs + 0.5);
  int T0_right;
  int T0_left = ts_int;

  // set size of output buffer for 0th frame
  olabuf.clear();
  olabuf.set_mid_end(0, T0_left);

  // output wave buffer
  OGI_Wave output_res;
  OGI_Wave_D output_res_d;
  double max_v, max_all = 0.0;
  int outbuf_incr = 1000;
  output_res_d.set_incr(outbuf_incr);
  output_res.set_incr(10);

  // use OGI_WaveChunk for convenience
  OGI_WaveChunk sp_in;
  sp_in.append_chunk(sp, 0, sp.length(), 0);

  for (k=0; k<lpc.num_frames(); k++){ 
        
    // fundamental period to right of current pmark
    if (k < lpc.num_frames()-1)
      next_ts = (int)(lpc.t(k+1)*Fs + 0.5);
    else
      next_ts = (int)(sp.length());     
    T0_right = next_ts - ts_int;
    
    // extract LPC vector from track    
    for (n=1; n<lpc.num_channels(); n++)
      lpcvect.a[n] = lpc.a_no_check(k,n);
    lpcvect.a[0] = 1.0;
    
    // extract 2-pitch period pulse from sp
    sp_in.get(ts_int, T0_left, T0_right, spbuf); 

    // inverse filter
    inverse_filter_lpc(spbuf, lpcvect, excbuf);

    // generate OLA window
    window.set_mid_end(excbuf.mid(), excbuf.end());
    create_window(window, wintype);

    // add first half of new pulse to OLA buffer, apply window, gain
    add_new_pulse(excbuf, window, 1.0, olabuf);

    if (DO_deemph) // deemphasis filter
      deemph(olabuf, (float)d_coeff);

    // clip, quantize, copy outbuf to final output vector
    max_v = output_res_d.append(olabuf.buf, olabuf.end());
    max_all = max(max_all, max_v);

    // set size of output buffer for NEXT frame
    olabuf.set_mid_end(0, T0_right);

    // copy second half of old pulse to empty buffer, apply window, gain
    copy_old_pulse(excbuf, window, 1.0, olabuf);

    // update for next iteration
    ts_int = next_ts;
    T0_left = T0_right;	      
  }

  output_res_d.rescale(OGI_MAXSHORT/max_all);
  output_res.append_clip(output_res_d.mem(), output_res_d.Nused());

  // put output_res into EST_Wave
  res.resize(output_res.Nused());
  res.set_sample_rate((int)Fs);
  res.set_channel(0, output_res.mem()); 
  output_res.memfree(); 
}
  
static void deemph(OGI_PitchSyncBuffer &olabuf, float a){
  // DIFFERENT from function in resLPC_wave:
  // carries over state from previous frame

  double b = (1-a);
  static double ym1  =0.0;  // y[n-1]
  double *y = olabuf.buf;
  int len = olabuf.end();

  for ( ; len > 0; len--){
    *y = b*(*y) + a*ym1;
    ym1 = *y;
    y++;  
  }
}


static void inverse_filter_lpc(
			       OGI_PitchSyncBuffer &spbuf, 
			       OGI_VectDouble &lpc, 
			       OGI_PitchSyncBuffer &excbuf){

  excbuf.set_mid_end(spbuf.mid(), spbuf.end());
  fir_convolve(spbuf.buf, lpc.a, lpc.N, excbuf.buf, excbuf.end());
}


static void parse_init_params(LISP params, EST_Features &In){

  // Get params from Scheme layer
  In.set("pm_path",  wstrdup(get_param_str("pm_path", params, ".")));
  In.set("sp_path",  wstrdup(get_param_str("sp_path", params, ".")));
  In.set("pm_ext",   wstrdup(get_param_str("pm_ext",  params, ".pmv")));
  In.set("sp_ext",   wstrdup(get_param_str("sp_ext",  params, ".wav")));
  In.set("in_path",  wstrdup(get_param_str("in_path", params, ".")));
  In.set("out_path", wstrdup(get_param_str("out_path",params, ".")));
  In.set("res_format",  wstrdup(get_param_str("res_format", params, "nist")));
  In.set("lpc_format",  wstrdup(get_param_str("lpc_format", params, "lsf")));
  In.set("res_ext",  wstrdup(get_param_str("res_ext", params, ".res")));
  In.set("lpc_ext",  wstrdup(get_param_str("lpc_ext", params, ".lpc")));
  In.set("ola_window_type", wstrdup(get_param_str("ola_window_type", params, "trapezoid")));

  // other float parameters - no defaults to set
  LISP l;
  EST_String thing;
  for (l=params; l !=0; l=cdr(l)){
    thing = get_c_string(car(car(l)));
    if (!((In.present(thing)) || thing == "infile_list")){ 
      In.set(thing, get_c_float(car(cdr(car((l))))));
    }
  }
}


