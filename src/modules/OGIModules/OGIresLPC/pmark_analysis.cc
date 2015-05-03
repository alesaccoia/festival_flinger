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
#include "resLPC_pmark.h"
#include "resLPC_analysis.h"

static void parse_init_params(LISP params, EST_Features &In);
static void compute_pmarks2(EST_Wave &lx, EST_Features &g, OGI_Track &pmarks);
static void retune_pmarks(EST_Wave &sp, EST_Features &g, OGI_Track &pmarks);
/* not used static void design_ma_filter(int N, EST_DVector &b);*/
static void cluster_2class(OGI_Track &fv_track, EST_StrList &fnames, float Vbias);
static float innerprod(EST_FVector &x, EST_FVector &y);
static void rescaleV(EST_FVector &x, float G);
/* not used: static int find_min(EST_Wave &x, int B, int E);*/
static int find_min_w(EST_Wave &x, int B, int M, int E);
/* not used: static void find_pgzc(EST_Wave &x, int B, int E, EST_IList &pgzc);*/
static void find_ngzc(EST_Wave &x, int B, int E, EST_IList &pgzc);
static void find_vpm2( EST_Wave &lx, EST_Wave &lxdiff, EST_FList &Blist, EST_FList &Elist, EST_IList &pm_cand, EST_Features &g);
static void IList2Track(EST_IList &pmList, float Fs, OGI_Track &pm);
static void Track2IList(OGI_Track &pm, float Fs, EST_IList &pmList);
static void purge_pm_list(EST_IList &pm, EST_IList &rmlist);
static bool rm_suspicious_pm(EST_Wave &lx, EST_IList &pmList, EST_Features &g);
static void max_V_xcorr_shift( OGI_Track &pm, EST_Wave &sig, EST_Features &g);
static void measure_at_pm(OGI_Track &pm, EST_Wave &s, const EST_String &fname,int sign_ch);
static float maxV(EST_FVector &x);
static void pm_xcorr( OGI_Track &pm, EST_Wave &lx, const EST_String &fname, EST_Features &g);
static void pm_power( OGI_Track &pm, EST_Wave &lx, const EST_String &fname, EST_Features &g);
static float win_xcorr(EST_Wave &x, int cX, int cY, int winsz);
static float std_track(OGI_Track &x, int ch);
  
#define MAX_ITER 30
bool pm_debug_print = 0;
bool pm_debug_shifts = 0;
bool pm_debug_dump  = 0;

// can use (directory-entries ".") to get file list when called in scm

LISP OGIresLPC_Pmark_Analysis(LISP params){
  EST_Features g;
  parse_init_params(params, g);
  LISP l;
  
  LISP lfiles = get_param_lisp("infile_list", params, NIL);

  if (g.present("run_quietly")){ pm_debug_print = (!(g.I("run_quietly"))); }
  if (g.present("print_shifts")){ pm_debug_shifts = g.I("print_shifts") ? true : false; }
  if (g.present("dump_intermediate_files")){ pm_debug_dump  = g.I("dump_intermediate_files") ? true : false; }

  if (lfiles == NIL){
    EST_error("OGIresLPC pmark analysis:  infile_list empty - no input files to process");
  }

  EST_Pathname f,spfile,lxfile,outfile;
  EST_String basename;
  EST_String sp_path  = wstrdup(g.S("sp_path"));
  EST_String lx_path  = wstrdup(g.S("lx_path"));
  EST_String out_path = wstrdup(g.S("out_path"));
  EST_String out_ext  = wstrdup(g.S("out_ext"));
  EST_String sp_ext   = wstrdup(g.S("sp_ext"));
  EST_String lx_ext   = wstrdup(g.S("lx_ext"));
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

    lxfile  = lx_path + basename + lx_ext;
    spfile  = sp_path + basename + sp_ext;
    outfile = out_path   + basename + out_ext;
    if (pm_debug_print) cout << " loading " << lxfile << " --> output to " << outfile << endl;
    if (pm_debug_shifts) cout << basename.after("_") << "\t";

    EST_Wave lx;    
    EST_Wave sp;    
    OGI_Track pmarks;
    if (lx.load(lxfile) != read_ok){
      cout << "Could not load lx file " << lxfile << "  ... skipping." << endl;
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
      // optional resampling
      if (g.present("downsample_factor")){
	lx.resample((int) (lx.sample_rate()/g.F("downsample_factor")));
      }

      //      compute_pmarks(lx, g, pmarks);
      compute_pmarks2(lx, g, pmarks); ///////////////////////////<<<<<<<<<<<<
      retune_pmarks( sp, g, pmarks);
      pmarks.save(outfile);
    }
  }

  return NIL;
}


//
//  Compute pitchmarks from laryngograph signal
//
static void compute_pmarks2(
			   EST_Wave &lx, 
			   EST_Features &g, 
			   OGI_Track &pmarks){

  if ((g.S("lx_sign") == "down_is_closed_glottis") || (g.S("lx_sign") == "up_is_open_glottis"))
    ;// nothing
  else if ((g.S("lx_sign") == "down_is_open_glottis") || (g.S("lx_sign") == "up_is_closed_glottis")){
    invert(lx);
  }
  else 
    EST_error("pmark_analysis.cc:  don't understand your 'lx_sign setting...\n");

  EST_DVector a,b;
  if (pm_debug_dump) lx.save("lx.wav","riff");

  ////
  //// Pre-filter to get rid of low-freq drift on signal
  ////
  filtlist_to_vector(g.S("pre_filt1"), b, a);
  OGIfiltfilt(b, a, lx);
  if (pm_debug_dump) lx.save("lx_prefilt.wav","riff");
  
  ////
  //// Compute derivative of Lx
  ////
  EST_Wave lxdiff = lx;
  filtlist_to_vector(g.S("diff_filt"), b, a);
  OGIzerophase_FIRfilter(b, lxdiff);
  if (pm_debug_dump) lxdiff.save("lxdiff.wav","riff");
 
  ////
  //// Make V/UV decision
  ////
  // filter to isolate voicing band
  EST_Wave lx_vuvfilt = lx;
  filtlist_to_vector(g.S("vuv_filt"), b, a);
  OGIfiltfilt(b, a, lx_vuvfilt);
  if (pm_debug_dump) lx_vuvfilt.save("lx_vuv.wav","riff");

  /// legacy from old method ...... rm later
  EST_FList Vbegin, Vend; // in seconds
  Vbegin.append(0);
  Vend.append(lxdiff.length()/(float)lx.sample_rate());

  ////
  //// Find candidate pitchmarks
  ////    
  EST_IList pm_cand;
  find_vpm2(lx_vuvfilt, lxdiff, Vbegin, Vend, pm_cand, g);

  //// 
  //// Remove cands that don't have large derivative
  ////   and/or large correlation with others nearby
  ////
  IList2Track(pm_cand, (float) lx.sample_rate(), pmarks);

  // find mag of dlx at cands (normalized)
  measure_at_pm(pmarks, lxdiff, "dlx", -1);

  // find pm-to-pm correlation in lx signal (max over 3 to L and R) (normalize)
  pm_xcorr(pmarks, lx, "xcorr", g);

  // find power in low-freq region of lx signal
  pm_power(pmarks, lx_vuvfilt, "power", g);  

  // nn-cluster on these features
  EST_StrList flist;
  flist.append("dlx");  flist.append("xcorr");  flist.append("power");
  cluster_2class(pmarks,  flist, g.F("Vbias"));
  
  if (pm_debug_dump) pmarks.save("pm_unmod.est");

  // prune
  Track2IList(pmarks, (float) lx.sample_rate(), pm_cand);

  ////
  //// Remove stray and double pitchmarks 
  ////
  bool there_are_suspects = TRUE;
  int n=0;
  while ((there_are_suspects) && (n<MAX_ITER)){
    there_are_suspects = rm_suspicious_pm(lx, pm_cand, g);
    n++;
  }

  ////
  //// Output
  ////
  IList2Track(pm_cand, (float) lx.sample_rate(), pmarks);
  if (pm_debug_dump) pmarks.save("pm_prune.est");
}


//
//  Shift to get rid of wave propagation delay by aligning 
//  to peak in LPC residual. 
//
static void retune_pmarks(
			  EST_Wave &sp, 
			  EST_Features &g, 
			  OGI_Track &pmarks){
  
  // fill in UV parts with default pmarks (to keep LPC well-behaved)
  OGI_Track lpcpm;
  lpcpm = pmarks;
  fill_UV_pmarks(lpcpm, sp.length(), (float)sp.sample_rate(), g, 0);  

  // compute LPC
  compute_lpc(sp, lpcpm, g);

  // compute residual
  EST_Wave res;
  compute_residual(sp, lpcpm, g, res);

  // shift pmarks to max xcorr point
  max_V_xcorr_shift(pmarks, res, g);

  // make sure all are still within bounds of wave
  EST_IList pmlist;
  EST_Litem *p; 
  Track2IList(pmarks, (float) sp.sample_rate(), pmlist);
  for (p = pmlist.head(); p != 0; p = p->next()){
    if ((pmlist(p) < 0) || pmlist(p) > sp.length())
      p = pmlist.remove(p);
  }
  IList2Track(pmlist, (float) sp.sample_rate(), pmarks);
}



static void max_V_xcorr_shift( OGI_Track &pm, EST_Wave &sig, EST_Features &g){
  
  float sigFs = (float) sig.sample_rate();
  int N = g.I("shift_search_range");
  int t,t_shift, best_shift=0;
  double acc,max_acc=0.0;
  int Nsig = sig.length();
  int k,n;

  for (k=-N; k<=N; k++){
    t_shift = g.I("shift_guess") + k;
    acc = 0.;
    for (n=0; n<pm.length(); n++){
      t = (int)(pm.t(n)*sigFs) + t_shift;      
      if ((t < Nsig) && (t > 0))
	acc += sig.a_no_check(t);
    }
    if (fabs(acc) > max_acc){
      max_acc = fabs(acc);
      best_shift = t_shift;
    }
  }
  
  if (pm_debug_print) cout << "\tbest shift = " << best_shift << " samples \n";
  if (pm_debug_shifts) cout << best_shift << endl;

  float fbs = (float) best_shift / sigFs;
  for (n=0; n<pm.length(); n++)
    pm.t(n) = pm.t(n) + fbs;
}

/* not used:
static void design_ma_filter(int N, EST_DVector &b){
  b.resize(N);
  int k;
  for (k=0; k<N; k++)
    b[k] = 1.0/N;
}
*/
   
#define EXIT_THRESH 0.0001

static void cluster_2class(OGI_Track &fv_track, EST_StrList &fnames, float Vbias){
  // clusters based on Euclidean distance of features, rescaled by 1/std
  //  in named channels of track fv.
  //  puts results in cluster_output field

  int j,k,Nv,Nuv;
  float v2,u2;
  fv_track.resize(EST_CURRENT, fv_track.num_channels()+1);
  fv_track.set_channel_name("cluster_output",fv_track.num_channels()-1);
  int ClustOut = fv_track.num_channels()-1;
  EST_FVector fvk;

  if (fv_track.num_frames() < 2)
    return;

  int Nfeats = fnames.length();
  int Nfr = fv_track.num_frames();
  EST_IVector fieldnum(Nfeats);
  EST_FVector Cv(Nfeats),Cuv(Nfeats),prevCv(Nfeats),prevCuv(Nfeats), dv(Nfeats), du(Nfeats), std(Nfeats);
  
  for (k=0; k<Nfeats; k++){
    fieldnum.a(k) = fv_track.channel_position(fnames.nth(k));
    if (fieldnum(k) < 0)
      EST_error("class_2cluster: no such field name\n");
  }
    
  EST_FMatrix FV(Nfeats, Nfr);  

  // measure std dev
  for (k=0; k<Nfeats; k++){
    std.a(k) = std_track(fv_track, fieldnum.a(k));
  }

  // fill matrix
  for (k=0; k<FV.num_columns(); k++){  
    for (j=0; j<FV.num_rows(); j++){  
      FV(j,k) = fv_track.a(k, fieldnum(j)) / std(j);
    }
  }

  // initialize centroids to max/min norm vectors
  int min_vk=0,max_vk=1;
  float max_v = (float)-10e20;
  for (k=0; k<FV.num_columns(); k++){
    FV.column(fvk, k);
    v2 = innerprod(fvk, fvk); 
    if (v2 > max_v){
      max_v = v2;
      max_vk = k;
    }
  }
  for (j=0; j<FV.num_rows(); j++){  
    Cv.a(j) = FV(j, max_vk);
  }

  float min_v = (float)10e20;
  for (k=0; k<FV.num_columns(); k++){
    FV.column(fvk, k);
    v2 = innerprod(fvk, fvk); 
    if (v2 < min_v){
      min_v = v2;
      min_vk = k;
    }
  }
  for (j=0; j<FV.num_rows(); j++){  
    Cuv.a(j) = FV(j,min_vk);
  }

  for (j=0; j<MAX_ITER; j++){
    // decision based on Euclidean dist (w/ bias)
    for (k=0; k<FV.num_columns(); k++){
      FV.column(fvk, k);
      dv = fvk - Cv;
      du = fvk - Cuv;
      v2 = innerprod(dv, dv);
      u2 = innerprod(du, du);
      fv_track(k, ClustOut) = (float)(Vbias*v2 < u2);
    }

    // compute centroids
    prevCv = Cv;
    prevCuv = Cuv;
    Cv.fill(0.0); Cuv.fill(0.0); Nv=0; Nuv=0;
    for (k=0; k<FV.num_columns(); k++){
      if (fv_track(k,ClustOut) > 0.5){
	FV.column(fvk, k);
	Cv  = Cv + fvk;
	Nv++;
      }
      else{
	FV.column(fvk, k);
	Cuv  = Cuv + fvk;
	Nuv++;
      }
    }
    
    rescaleV(Cv, 1/(float)Nv);
    rescaleV(Cuv, 1/(float)Nuv);
      
    //    printf("%2d> Nv=%3d Cv=[%f %f]\tNuv=%3d Cuv=[%f %f]\n", j, Nv, Cv(0), Cv(1), Nuv, Cuv(0), Cuv(1));

    dv = Cv - prevCv; 
    if (innerprod(dv, dv) < EXIT_THRESH)
      break;
  }
}

static float innerprod(EST_FVector &x, EST_FVector &y){
  float z=0.0;
  int k;
  for (k=0; k<x.length(); k++){
    z+=x(k)*y(k);
  }
  return z;
}

static void rescaleV(EST_FVector &x, float G){
  int k;
  for (k=0; k<x.length(); k++)
    x.a(k) = x(k)*G;
}


static void find_vpm2( 
		      EST_Wave &lx, 
		      EST_Wave &lxdiff, 
		      EST_FList &Blist, 
		      EST_FList &Elist, 
		      EST_IList &pm_cand,
		      EST_Features &g){

  //     minima of Lx derivative near negative-going
  //         zero crossings of Lx in voiced regions
  
  EST_IList NZClist;
  int B,E,min_indx,srch_win;
  int n,m;
  pm_cand.clear();
  srch_win = (int)(g.F("dlx_search_factor")/g.F("guess_F0")*lx.sample_rate());

  for (n = 0; n < Blist.length(); n++){
    B = (int) (Blist.nth(n) * lx.sample_rate());
    E = (int) (Elist.nth(n) * lx.sample_rate());
    B = max(B, 0);                  // safety
    E = min(E, lx.length());         

    find_ngzc(lx, B, E, NZClist);

    // dump for debug
    OGI_Track tmpt;
    IList2Track(NZClist, (float)lx.sample_rate(), tmpt);    
    if (pm_debug_dump) 
      tmpt.save("ngzc.est");

    // min of derivative near NGZC
    for (m=0; m<NZClist.length(); m++){
      B = max(0, NZClist.nth(m)-srch_win);
      E = min(lx.length(), NZClist.nth(m)+srch_win);
      min_indx = find_min_w(lxdiff, B, NZClist.nth(m), E);
      pm_cand.append(min_indx);
    }
  }
}

/* not used:
static void find_pgzc(EST_Wave &x, int B, int E, EST_IList &pgzc){
  pgzc.clear();
  int k;
  for (k=B+1; k<E; k++){
    if ((x(k-1) <= 0) && (x(k) > 0)){
      pgzc.append(k);
    }
  }
}
*/
static void find_ngzc(EST_Wave &x, int B, int E, EST_IList &ngzc){
  ngzc.clear();
  int k;
  for (k=B+1; k<E; k++){
    if ((x(k-1) >= 0) && (x(k) < 0)){
      ngzc.append(k);
    }
  }
}

/* not used
static int find_min(EST_Wave &x, int B, int E){
  int k;
  int min_i = B;
  short minval=32767;
  for (k=B; k<E; k++){
    if (x(k) < minval){
      min_i = k;
      minval = x(k);
    }
  }
  return min_i;
}
*/

static int find_min_w(EST_Wave &x, int B, int M, int E){
  int k;
  int min_i = B;
  double minval=32767.0;
  double xw;
  if (M > B){
    for (k=B; k<M; k++){
      xw = x(k)*(k-B)/(double)(M-B);
      if (xw < minval){
	min_i = k;
	minval = xw;
      }
    }
  }
  if (E > M){
    for (k=M; k<E; k++){
      xw = x(k)*(1.0 - ((k-M)/(double)(E-M)));
      if (xw < minval){
	min_i = k;
	minval = xw;
      }
    }
  }
  return min_i;
}



static bool rm_suspicious_pm(EST_Wave &lx, EST_IList &pmList, EST_Features &g){
  int k,l;  
  EST_IList suspicious;
  int prevpm=0,nextpm=0;
  int T0_min = (int) (lx.sample_rate()/(g.F("guess_F0")*g.F("F0_range_factor")));
  int T0_max = (int) (lx.sample_rate()/(g.F("guess_F0")/g.F("F0_range_factor")));

  ////
  //// stray?
  ////
  EST_IList rmlist;
  for (k=0; k<pmList.length(); k++){
    prevpm = ((k==0) ? -100000 : pmList.nth(k-1));
    nextpm = ((k==pmList.length()-1) ?  pmList.last() + 10000 :  pmList.nth(k+1));
    
    // neither neighb is < T0_max away    
    if (((pmList.nth(k) - prevpm) > T0_max) && ((nextpm - pmList.nth(k)) > T0_max))
      rmlist.append(k);  // remove it
  }

  purge_pm_list(pmList, rmlist);
  

  ////
  //// double?
  ////
  for (k=0; k<pmList.length(); k++){
    prevpm = ((k==0) ? -100000 : pmList.nth(k-1));
    nextpm = ((k==pmList.length()-1) ?  pmList.last() + 10000 :  pmList.nth(k+1));
    
    // nearest neighb is < T0_min away
    if (((pmList.nth(k) - prevpm) < T0_min) || ((nextpm - pmList.nth(k)) < T0_min)){
      suspicious.append(k);
    }
  }
  bool retval = suspicious.length() ? true : false;

  // consider pairs of pmarks that are too close together
  k=0;
  rmlist.clear();
  while (k<suspicious.length()){
    EST_IList pair;
    pair.clear();
    int first_of_pair = pmList.nth(suspicious.nth(k));
    if(k<suspicious.length()){
      pair.append(suspicious.nth(k));
      k++;
    }
    if((k<suspicious.length()) 
       && (pmList.nth(suspicious.nth(k))-first_of_pair < T0_min)){
      pair.append(suspicious.nth(k));
      k++;
    }

    if (pair.length() > 1){
      // measure: closeness to 1/2 way between pmarks just before and just after pair   
      int best_sc = 10000;
      int best_ix = 0;
      prevpm =  ((pair.first()==0) ? pmList.nth(0)-T0_min : pmList.nth(pair.first()-1));
      nextpm =  ((pair.last()==pmList.length()-1) ? pmList.last()+T0_min : pmList.nth(pair.last()+1));

      for (l=0; l<pair.length(); l++){
	int sc = abs((pmList.nth(pair.nth(l)) - prevpm) - (nextpm - pmList.nth(pair.nth(l))));
	if (sc < best_sc){
	  best_sc = sc;
	  best_ix = l;
	}
      }
      
      // reduce each pair to 1 pmark
      for (l=0; l<pair.length(); l++){
	if (l != best_ix){
	  rmlist.append(pair.nth(l));
	}
      }
    }
  }  

  purge_pm_list(pmList, rmlist);

  return retval;
}

static void purge_pm_list(EST_IList &pm, EST_IList &rmlist){
  // in pm, remove INDICES listed in rmlist
  EST_Litem *p;
  for (p=rmlist.tail(); p != 0; p = p->prev()){
    pm.remove_nth(rmlist(p));
  }
}
static void pm_power(
		     OGI_Track &pm, 
		     EST_Wave &lx, 
		     const EST_String &fname,
		     EST_Features &g){
  int k,cX;
  pm.resize(EST_CURRENT, pm.num_channels()+1);
  pm.set_channel_name(fname, pm.num_channels()-1);
  int M = pm.num_channels()-1;
  float Fs = (float) lx.sample_rate();

  int winsz=100;
  if (g.present("xcorr_winsz_msec"))
    winsz = (int) (g.F("xcorr_winsz_msec")/1000.0 * Fs+0.5);  

  for (k=0; k<pm.num_frames(); k++){
    cX = (int) (pm.t(k)*Fs+0.5);
    pm(k,M) = sqrt(win_xcorr(lx, cX, cX, winsz) / (float) winsz);
  }  
}

static void pm_xcorr(
		     OGI_Track &pm, 
		     EST_Wave &lx, 
		     const EST_String &fname,
		     EST_Features &g){
  int k,j,cX,cY;
  float XX, XY, YY;
  pm.resize(EST_CURRENT, pm.num_channels()+1);
  pm.set_channel_name(fname, pm.num_channels()-1);
  int M = pm.num_channels()-1;
  int Nxc=3;
  float Fs = (float) lx.sample_rate();
  if (g.present("xcorr_max_range"))
    Nxc = g.I("xcorr_max_range");  
  int winsz=100;
  if (g.present("xcorr_winsz_msec"))
    winsz = (int) (g.F("xcorr_winsz_msec")/1000.0 * Fs+0.5);  
  EST_FVector xcval(2*Nxc);

  for (k=0; k<pm.num_frames(); k++){
    xcval.fill(0.0);
    cX = (int) (pm.t(k)*Fs+0.5);
    XX = win_xcorr(lx, cX, cX, winsz);

    for (j=-Nxc; j<Nxc; j++){
      if ((j != 0) && (k+j < pm.num_frames()) && (k+j >= 0)){
	cY = (int) (pm.t(k+j)*Fs+0.5);	
	XY = win_xcorr(lx, cX, cY, winsz);	
	YY = win_xcorr(lx, cY, cY, winsz);
	xcval.a(j+Nxc) = 2*XY/(XX+YY);
      }
    }  
    pm(k,M) = maxV(xcval);
  }
}

static float win_xcorr(EST_Wave &x, int cX, int cY, int winsz){
  int k,n;
  winsz = min(min(winsz, cX), cY);
  winsz = min(min(winsz, x.length()-1-cX), x.length()-1-cY);
  float xc_out=0.0;
  double xc=0.0;
  double w=0.0;
  double incr=0.0;
  if (winsz > 0){
    incr=(double)(1/(winsz+1));
    for (k=cX-winsz,n=cY-winsz; k<cX; k++,n++){
      w += incr;
      xc += w * x(k) * x(n);
    }
    w = 1.0;
    incr=(double)(1/(winsz+1));
    for (; k<=cX+winsz; k++,n++){
      xc += w * x(k) * x(n);
      w -= incr;
    }
  }
  xc_out = (float)xc/(float)(2*winsz+1);
  return xc_out;
}

static float maxV(EST_FVector &x){
  int k;
  float v=(float)-10e20;
  for (k=0; k<x.length(); k++)
    v = max(v, x.a(k));
  return v;
}

static float std_track(OGI_Track &x, int ch){
  int k;
  double x2 = 0.0, ux=0.0, v=0.0;
  
  for (k=0; k<x.num_frames(); k++){
    v = x.a(k,ch);
    x2 += v*v;
    ux += v;
  }
  ux /= (float) x.num_frames();
  x2 /= (float) x.num_frames();    
  return (float) sqrt(x2 - ux*ux);
}

static void measure_at_pm(
			  OGI_Track &pm, 
			  EST_Wave &s, 
			  const EST_String &fname,
			  int sign_ch){
  int k;
  pm.resize(EST_CURRENT, pm.num_channels()+1);
  pm.set_channel_name(fname, pm.num_channels()-1);
  int M = pm.num_channels()-1;
  float Fs = (float) s.sample_rate();
  int i;
  int Ns = s.length();

  for (k=0; k<pm.num_frames(); k++){
    i = min(Ns-1,max(0, (int) (pm.t(k)*Fs+0.5)));
    pm(k,M) = (float)(s(i) * sign_ch);
  }  
}
  

static void IList2Track(EST_IList &pmList, float Fs, OGI_Track &pm){
  int k;
  pm.resize(pmList.length(), 0);
  for ( k=0; k<pmList.length(); k++){
    pm.t(k) = pmList.nth(k)/Fs;
  }
}


static void Track2IList(OGI_Track &pm, float Fs, EST_IList &pmList){
  // also prunes unvoiced ones
  int k;
  pmList.clear();
  bool doPRUNE=pm.has_channel("cluster_output");
  
  for (k=0; k<pm.num_frames(); k++){  
    if (doPRUNE){
      if (pm.a(k,"cluster_output") > 0.5)
	pmList.append((int)(pm.t(k)*Fs + 0.5));
    }
    else
      pmList.append((int)(pm.t(k)*Fs + 0.5));
  }
}


static void parse_init_params(LISP params, EST_Features &In){
  LISP l;
  EST_String thing;
  LISP plisp=NIL;
  thing = "";

  // Get params from Scheme layer
  In.set("lx_path",  wstrdup(get_param_str("lx_path", params, ".")));
  In.set("sp_path",  wstrdup(get_param_str("sp_path", params, ".")));
  In.set("lx_ext",   wstrdup(get_param_str("lx_ext",  params, ".lar")));
  In.set("sp_ext",   wstrdup(get_param_str("sp_ext",  params, ".wav")));
  In.set("in_path",  wstrdup(get_param_str("in_path", params, ".")));
  In.set("out_path", wstrdup(get_param_str("out_path",params, ".")));
  In.set("out_ext",  wstrdup(get_param_str("out_ext", params, ".pmv")));
  In.set("lx_sign",  wstrdup(get_param_str("lx_sign", params, "up_is_closed_glottis")));
  In.set("sex",      wstrdup(get_param_str("sex",     params, "male")));

  // NEED SPEECH_TOOLS PATCH....
  plisp = get_param_lisp("pre_filt1", params, NIL);
  if (plisp != NIL){
    In.set("pre_filt1", siod_sprint(plisp));
  }

  plisp = get_param_lisp("vuv_filt", params, NIL);
  if (plisp != NIL){
    In.set("vuv_filt", siod_sprint(plisp));
  }

  plisp = get_param_lisp("diff_filt", params, NIL);
  if (plisp != NIL){
    In.set("diff_filt", siod_sprint(plisp));
  }


  // other float parameters - no defaults to set
  for (l=params; l !=0; l=cdr(l)){
    thing = get_c_string(car(car(l)));
    if (!((In.present(thing)) || thing == "infile_list")){ 
      In.set(thing, get_c_float(car(cdr(car((l))))));
    }
  }

  // these are used in the rough LPC analysis for getting rid of lx-pmark delay
  In.set("ola_window_type","trapezoid");
  In.set("pre_emphasis",0.96);
  In.set("lpc_anl_frame_len",3.0);
  In.set("T0_UV_thresh", In.F("F0_range_factor") / In.F("guess_F0"));
  In.set("T0_UV_pm",0.01);

}




///////////// old stuff ///////////////////
#if 0 
static void compute_pmarks( EST_Wave &lx, EST_Features &g, OGI_Track &pmarks);
static void compute_E(EST_Wave &sig, OGI_Track &E, EST_Features &g);
static void max_limit_curve(OGI_Track &E);
static void cluster_2class(OGI_Track &fv, float Vbias, EST_FList &B, EST_FList &E);
static void find_vpm( EST_Wave &lx, EST_Wave &lxdiff, EST_FList &Blist, EST_FList &Elist, EST_IList &pm_cand);
static void s2p(EST_FVector &frame, float &power);




static void compute_E(EST_Wave &sig, OGI_Track &E, EST_Features &g){
  // actually power
  float guessF0 = g.F("guess_F0");
  float pppw = g.F("periods_per_Ewin");   // pitch periods per window
  int winsz = (int)(pppw * sig.sample_rate()/guessF0);
  int num_frames= (int)ceil(g.F("Eest_ppm")*sig.length()/(float)winsz); 
  E.resize(num_frames, 1); 
  E.fill_time(winsz/(float)(g.F("Eest_ppm")*sig.sample_rate()));

  EST_FVector frame;
  int window_start,pos, k;  
  EST_WindowFunc *wf =  EST_Window::creator("hamming");
  
  for (k=0; k < E.num_frames(); k++) {
    pos = (int)(E.t(k) * sig.sample_rate() + 0.5);
    window_start = pos - winsz/2;
    EST_Window::window_signal(sig, wf, window_start, winsz, frame, 1);
    s2p(frame, E.a(k));
  }
}


static void find_vpm( EST_Wave &lx, EST_Wave &lxdiff, EST_FList &Blist, EST_FList &Elist, EST_IList &pm_cand){
  //     minima of Lx derivative between positive-going
  //         zero crossings of Lx in voiced regions
  
  EST_IList PZClist;
  int B,E,min_indx;
  int n,m;
  pm_cand.clear();
  for (n = 0; n < Blist.length(); n++){
    B = (int) (Blist.nth(n) * lx.sample_rate());
    E = (int) (Elist.nth(n) * lx.sample_rate());

    // safety
    B = max(B, 0); 
    E = min(E, lx.length()); 

    find_pgzc(lx, B, E, PZClist);
    PZClist.prepend(B);  // add beginning of voiced section

    OGI_Track tmpt;
    IList2Track(PZClist, lx.sample_rate(), tmpt);    

    if (pm_debug_dump) 
      tmpt.save("pgzc.est");

    for (m=0; m<PZClist.length()-1; m++){
      min_indx = find_min(lxdiff, PZClist.nth(m),  PZClist.nth(m+1));
      pm_cand.append(min_indx);
    }
  }
}


//
//  Compute pitchmarks from laryngograph signal
//
static void compute_pmarks(
			   EST_Wave &lx, 
			   EST_Features &g, 
			   OGI_Track &pmarks){

  if ((g.S("lx_sign") == "down_is_closed_glottis") || (g.S("lx_sign") == "up_is_open_glottis"))
    ;// nothing
  else if ((g.S("lx_sign") == "down_is_open_glottis") || (g.S("lx_sign") == "up_is_closed_glottis")){
    invert(lx);
  }
  else 
    EST_error("pmark_analysis.cc:  don't understand your 'lx_sign setting...\n");

  EST_DVector a,b;
  if (pm_debug_dump) lx.save("lx.wav","riff");

  ////
  //// Pre-filter to get rid of low-freq drift on signal
  ////
  filtlist_to_vector(g.S("pre_filt1"), b, a);
  OGIfiltfilt(b, a, lx);
  if (pm_debug_dump) lx.save("lx_prefilt.wav","riff");
  
  ////
  //// Compute derivative of Lx
  ////
  EST_Wave lxdiff = lx;
  filtlist_to_vector(g.S("diff_filt"), b, a);
  OGIzerophase_FIRfilter(b, lxdiff);
  if (pm_debug_dump) lxdiff.save("lxdiff.wav","riff");
 
  ////
  //// Make V/UV decision
  ////
  // filter to isolate voicing band
  EST_Wave lx_vuvfilt = lx;
  filtlist_to_vector(g.S("vuv_filt"), b, a);
  OGIfiltfilt(b, a, lx_vuvfilt);
  if (pm_debug_dump) lx_vuvfilt.save("lx_vuv.wav","riff");

  // Square and do limiting on lx_vuvfilt
  OGI_Track lxE;
  compute_E(lx_vuvfilt, lxE, g);
  max_limit_curve( lxE );

  // 1 (guessed) pitch period smoother (two passes)
  float tt = g.F("Eest_ppm");
  design_ma_filter((int)(tt/2.0+0.5), b); 
  OGIzerophase_FIRfilter(b, lxE);

  // Find optimal threshold between V and UV based on energy
  EST_FList Vbegin, Vend; // in seconds
  cluster_2class(lxE,  g.F("Vbias"), Vbegin, Vend);
  if (pm_debug_dump) lxE.save("lxE.est");

  ////
  //// Find voiced pitchmarks: 
  ////    
  EST_IList pm_cand;
  find_vpm(lx_vuvfilt, lxdiff, Vbegin, Vend, pm_cand);
  if (pm_debug_dump){
    IList2Track(pm_cand, (float) lx.sample_rate(), pmarks);
    pmarks.save("pm_unmod.est");
  }

  ////
  //// Remove stray and double pitchmarks 
  ////
  bool there_are_suspects = TRUE;
  int n=0;
  while ((there_are_suspects) && (n<MAX_ITER)){
    there_are_suspects = rm_suspicious_pm(lx, pm_cand, g);
    n++;
  }

  ////
  //// Output
  ////
  IList2Track(pm_cand, (float) lx.sample_rate(), pmarks);
  if (pm_debug_dump) pmarks.save("pm_prune.est");
}
static void cluster_2class(OGI_Track &fv, float Vbias, EST_FList &B, EST_FList &E){
  
  int j,k,Nv,Nuv;
  double prevCv,Cv=0.0,prevCuv,Cuv=0.0,du,dv;
  fv.resize(EST_CURRENT, fv.num_channels()+1);
  fv.set_channel_name("smoothed_energy",0);
  fv.set_channel_name("vuv",1);

  for (k=0; k<fv.num_frames(); k++){
    Cv = max(Cv,fv(k,0));
  }
  
  for (j=0; j<MAX_ITER; j++){
    for (k=0; k<fv.num_frames(); k++){
      dv = fv(k,0) - Cv;
      du = fv(k,0) - Cuv;
      fv(k,1) = (float)(Vbias*(dv*dv) < (du*du));
    }

    prevCv = Cv;
    prevCuv = Cuv;
    Cv = 0; Cuv = 0; Nv=0; Nuv=0;
    for (k=0; k<fv.num_frames(); k++){
      if ((int) fv(k,1)){
	Cv  += fv(k,0);
	Nv++;
      }
      else{
	Cuv += fv(k,0);
	Nuv++;
      }
    }
    Cv/=(float)Nv;
    Cuv/=(float)Nuv;

    if (fabs(Cv - prevCv) < EXIT_THRESH)
      break;
  }

  int wasVOICED = (int) fv(0,1);
  if (wasVOICED)
    B.append(0);
  
  for (k=0; k<fv.num_frames(); k++){
    if (((int) fv(k,1)) && (!wasVOICED))
      B.append(fv.t(k));
    else if ((!((int) fv(k,1))) && wasVOICED)
      E.append(fv.t(k));
    
    wasVOICED = (int) fv(k,1);
  }

  if (wasVOICED){
    E.append(fv.t(fv.num_frames()));
  }
}


static void s2p(EST_FVector &frame, float &power){
  int i;
  power = 0.0;
  for (i=0; i < frame.length(); i++)
    power += frame(i)*frame(i);
  power /= frame.length();
}

static void max_limit_curve(OGI_Track &E){
  double meanE = E.mean(0);
  int k;
  for (k=0; k<E.num_frames(); k++)
    E.a(k,0) = min(meanE, E.a(k,0));
}
  
#endif
