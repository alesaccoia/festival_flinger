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

#include "OGI_Macro.h"
#include "OGI_Track.h"
#include "OGIresLPC_db.h"


//#ifdef INSTANTIATE_TEMPLATES
//#include "../OGIcommon/OGI_Track_file.cc" // for OGIfread_track
//#endif

static void time_shift_track(EST_Track &tr, float ts);

OGIresLPC_db::OGIresLPC_db(){
  udata_type=d_resLPC;
  loaded_units.clear();
}

OGIresLPC_db::~OGIresLPC_db(){
  if (isGrouped && (gfd != NULL))
  {
    cout << "closed groupfile\n";
    fclose(gfd);
  }
}
////////////////////////////////////////////////////
//////  runtime access funtions for synthesis  /////
////////////////////////////////////////////////////

//
// Get all the data needed to synthesize a unit 
//    - called from main synthesis functions in resLPC.cc
//    - both grouped and ungrouped access
//    - just sets pointers, does NOT copy data, so don't
//      try to change values w/o first copying in synthesis routine
//    - t() arrays in pm and lpc are relative to excB, NOT begin of Wave
//
void OGIresLPC_db::get_dbUnit( 
	 int db_indx,  /// index of the one we want
	 EST_Wave **exc_out,   // lpc residual
	 int *excB_out,        // begin and end samples of unit,
	 int *excE_out,        //   relative to exc
	 EST_Track **pm_out,   // pmarks in exc
	 EST_Track **lpc_out   // lpc coeffs (indpt time points from pm)
	 ){

  // load it
  if (!(isLoaded(db_indx))){
    EST_error("OGIresLPC_db::get_dbUnit - tried to get unit %d, which is not loaded!", db_indx);
  }

  // point to excitation
  *exc_out  = exc(db_indx);
  *excB_out = excB(db_indx);
  *excE_out = excE(db_indx);
  
  // point to pitchmarks
  *pm_out  = pm(db_indx);
  
  // point to lpc coeffs 
  *lpc_out  = lpc(db_indx);  
}

void OGIresLPC_db::free_all_units(void){
  EST_Litem *p; 
  for (p = loaded_units.head(); p != 0; p = p->next()){
    free_unit(loaded_units(p));
  }
  loaded_units.clear();
}

//
// Loads speech data for ONE UNIT from 
//             (already opened) groupfile
//
int OGIresLPC_db::load_unit_udata(int indx){

  if ((access_mode == a_dynamic) || (access_mode == a_ondemand)){ 
    if (!(isLoaded(indx))){
      
      loaded_units.append(indx);
      
      if (isGrouped){
	load_grp_exc(indx);
	load_grp_pm(indx);
	load_grp_lpc(indx);
      }
      else{  // ungrouped 
	load_ungrp_exc(indx);
	load_ungrp_pm(indx);
	load_ungrp_lpc(indx);
	
	// trim 
	int sig_band_samp = (int)(dbglobal.fval("sig_band")*Fs +0.5); 
	trim_unit(indx, sig_band_samp, sig_band_samp, Fs);
	
	modify_gain(indx);
      }
    }
  }
  return excE(indx)-excB(indx); // return number of samples   
}


////////////////////////////////////////////////////
//////   grouped load/save  ////////////////////////
////////////////////////////////////////////////////
void OGIresLPC_db::load_grp_offsets(){ 

  int k;
  int N = length();

  // temp storage
  long* offsets = new long [N];
  if (offsets == NULL) cerr << "OGIresLPC_db::load_grp_offsets - insufficient memory" 	                           << endl, exit(-1);

  // get byte offsets
  OGIfread(offsets,  sizeof(long), N, gfd, swap ? true : false);
  for (k=0; k<N; k++)
    p_exc_offset[k] = offsets[k];

  OGIfread(offsets,  sizeof(long), N, gfd, swap ? true : false);
  for (k=0; k<N; k++)
    p_pm_offset[k] = offsets[k];

  OGIfread(offsets,  sizeof(long), N, gfd, swap ? true : false);
  for (k=0; k<N; k++)
    p_lpc_offset[k] = offsets[k];

  delete offsets;
}

void OGIresLPC_db::load_grp_exc(int n){ 
  
  int itmp[2];

  // find offset into groupfile
  fseek(gfd, p_exc_offset(n), SEEK_SET);

  // load bound info
  OGIfread(itmp, sizeof(int), 2, gfd, swap ? true : false);
  p_excB[n] = itmp[0];
  p_excE[n] = itmp[1];
  
  // alloc new obj
  p_exc[n] = new EST_Wave;

  // read the wave
  OGIfread(* (EST_Wave *) (p_exc[n]), gfd); // OGI_file.cc

  // mark as loaded
  p_exc_isLoaded[n] = TRUE;

}
void OGIresLPC_db::load_grp_pm(int n){ 

  // find offset into groupfile
  fseek(gfd, p_pm_offset(n), SEEK_SET);

  // alloc new obj
  p_pm[n]  = new EST_Track;

  // read the track
  OGIfread_track(* (EST_Track *) (p_pm[n]), gfd); // OGI_Track_file.cc

  // mark as loaded
  p_pm_isLoaded[n]  = TRUE;

}

void OGIresLPC_db::load_grp_lpc(int n){ 
  
  // find offset into groupfile
  fseek(gfd, p_lpc_offset(n), SEEK_SET);
  
  // alloc new obj
  p_lpc[n]  = new EST_Track;

  // read the track
  OGIfread_track(*((EST_Track *) p_lpc[n]), gfd); // OGI_file.cc

  // mark as loaded
  p_lpc_isLoaded[n]  = TRUE;
}


//
// Saves ALL UNITS' speech data in (already opened) groupfile
//
void  OGIresLPC_db::save_grp_udata(){
  int k;
  int N = length(); 

  long* offsets = new long [N];  // tmp
  if (offsets == NULL) cerr << "OGIresLPC_db::save_grp_udata - insuffient memory"
	                          << endl, exit(-1);
  int excinfo[2];   // tmp
  long fp_done,fp_off;

  // write the size
  fwrite(&N, sizeof(int), 1, gfd);

  // reserve a place in the file for the offsets
  fp_off = ftell(gfd);
  memset(offsets, 0, sizeof(long)*N);
  fwrite(offsets, sizeof(long), N, gfd);  // exc
  fwrite(offsets, sizeof(long), N, gfd);  // pm
  fwrite(offsets, sizeof(long), N, gfd);  // lpc

  // write data
  for (k=0; k<N; k++){
    // be sure this one is loaded
    load_unit_udata(k);

    // write speech data, remember byte offsets
    p_exc_offset[k] = ftell(gfd);
    excinfo[0] = excB(k);
    excinfo[1] = excE(k);
    fwrite(excinfo, sizeof(int), 2, gfd);
    OGIfwrite(*(exc(k)), gfd, isCompressed ? true : false);
    
    p_pm_offset[k]  = ftell(gfd);
    OGIfwrite_track(*(pm(k)),  gfd, isCompressed ? true : false);

    p_lpc_offset[k] = ftell(gfd);
    OGIfwrite_track(*(lpc(k)), gfd, isCompressed ? true : false);
  }
  fp_done = ftell(gfd);  // remember end


  // go back and fill in offsets
  fseek(gfd, fp_off, SEEK_SET);
  for (k=0; k<N; k++)
    offsets[k] = exc_offset(k);
  fwrite(offsets,  sizeof(long), N, gfd);
  for (k=0; k<N; k++)
    offsets[k] = pm_offset(k);
  fwrite(offsets,  sizeof(long), N, gfd);
  for (k=0; k<N; k++)
    offsets[k] = lpc_offset(k);
  fwrite(offsets,  sizeof(long), N, gfd);
  fseek(gfd, fp_done, SEEK_SET);  // move to end

  delete offsets;
}

void  OGIresLPC_db::load_grp_udata(){

  int k,N;
 
  // read the size
  OGIfread(&N, sizeof(int), 1, gfd, swap ? true : false);
  if (N != unitdic.Nunits())   // check
    EST_error("udata->length() != unitdic.Nunits() !!! \n");

  // realloc
  resize(N);

  // read byte offsets
  load_grp_offsets();

  // if a_direct mode, read all data 
  if (access_mode == a_direct){
    for (k=0; k<length(); k++){
      load_grp_exc(k);
      load_grp_pm( k);
      load_grp_lpc(k);
    }
  }
}


////////////////////////////////////////////////////
////// ungrouped load/save  ////////////////////////
////////////////////////////////////////////////////
void OGIresLPC_db::load_ungrp_exc(int n){ 

  // alloc new objs
  p_exc[n] = new EST_Wave;

  // get excB, excE from unitdic
  p_excB[n] = unitdic.unitB(n);
  p_excE[n] = unitdic.unitE(n);

  // create filename string
  EST_String excfile = fileinfo.sval("base_dir")
    + fileinfo.sval("exc_dir") + unitdic.file(n) + fileinfo.sval("exc_ext");

  EST_read_status ret = ((EST_Wave *)p_exc[n])->load(excfile);  // load file
  if (ret != read_ok)
    EST_error(EST_String("OGIresLPC_db.cc:  could not read exc file ") + excfile + EST_String("\n"));

  p_exc_isLoaded[n] = TRUE;  // mark as loaded
}

void OGIresLPC_db::load_ungrp_pm(int n){ 
     
  // alloc new objs
  p_pm[n]  = new EST_Track;

  // create filename
  EST_String pmfile = fileinfo.sval("base_dir")
    + fileinfo.sval("pm_dir") + unitdic.file(n) + fileinfo.sval("pm_ext");

  EST_read_status ret = ((EST_Track *)p_pm[n])->load(pmfile);  // load file
  if (ret != read_ok)
    EST_error(EST_String("OGIresLPC_db.cc:  could not read pm file ") + pmfile + EST_String("\n"));

  // adjust so pmarks are measured relative to excB instead of 0
  time_shift_track(*(EST_Track *)p_pm[n], (-excB(n))/Fs);

  p_pm_isLoaded[n] = TRUE;  // mark as loaded
}

void OGIresLPC_db::load_ungrp_lpc(int n){ 

  // alloc new objs
  p_lpc[n]  = new EST_Track;

  // create filename
  EST_String lpcfile = fileinfo.sval("base_dir")
    + fileinfo.sval("lpc_dir") + unitdic.file(n) + fileinfo.sval("lpc_ext");

  EST_read_status ret = ((EST_Track *)p_lpc[n])->load(lpcfile); // load file
  if (ret != read_ok)
    EST_error(EST_String("OGIresLPC_db.cc:  could not read lpc file ") + lpcfile + EST_String("\n"));

  // adjust so times are measured relative to excB instead of 0
  time_shift_track(*(EST_Track *)p_lpc[n], (-excB(n))/Fs);

  p_lpc_isLoaded[n] = TRUE;  // mark as loaded
}


#define chk_clip(A)  if ((A)  > (float) OGI_MAXSHORT) { clip = 1; A = min(OGI_MAXSHORT, max(-OGI_MAXSHORT, A));}
void OGIresLPC_db::modify_gain(int n){ 

  if ((p_gainL[n]==1.0) && (p_gainR[n]==1.0))
    return;
  
  int k;

  // EXC wave
  int clip=0;
  double ftmp; 
  double gain = p_gainL[n];
  double incr = (p_gainR[n]-p_gainL[n])/(excE(n) - excB(n));

  EST_Wave *wav = exc(n);
  for (k=0; k<excB(n); k++){
    ftmp = wav->a(k) * gain;
    chk_clip(ftmp);
    wav->a(k) = (short) ftmp;
  }
  for ( ; k<excE(n); k++){
    ftmp = wav->a(k) * gain;
    chk_clip(ftmp);
    wav->a(k) = (short) ftmp;
    gain += incr;
  }
  for ( ; k<wav->length(); k++){
    ftmp = wav->a(k) * gain;
    chk_clip(ftmp);
    wav->a(k) = (short) ftmp;
    gain += incr;
  }

  if (clip == 1)
    cerr << "clipping in gain modification for unit " << n << endl;

  
  // power adjustment to reflect what we did to the excitation above
  //   assumes that these are log(root mean square ampl) over the frame
  int chE = lpc(n)->channel_position("E");  
  double M = wav->sample_rate() * (p_gainR[n] - p_gainL[n])/(excE(n)-excB(n));
  double B = p_gainL[n];
  double E = p_gainR[n];

  EST_Track *l = lpc(n);

  for (k=0; k<max(0,l->index_below((float)excB(n))); k++)
    l->a(k,chE) += (float)log(B);
  for ( ; k<min(l->index_below((float)excE(n))+1, l->num_frames()); k++)
    l->a(k,chE) += (float)log(l->t(k)*M + B);
  for ( ; k<l->num_frames(); k++)
    l->a(k,chE) += (float)log(E);
}


void  OGIresLPC_db::load_ungrp_udata(){
  int k;

  // get size from unitdic, realloc
  resize(unitdic.Nunits());

  // load gain factors
  load_ungrp_gains();
  
  // if a_direct mode, load speech data 
  if (access_mode == a_direct){
    for (k=0; k<length(); k++){
      load_ungrp_exc(k);
      load_ungrp_pm (k);
      load_ungrp_lpc(k);

      modify_gain(k);
    }
  }
}

void OGIresLPC_db::load_ungrp_gains(){ 
  // Load multiplicative gain factors associated with each unit boundary,
  // applied later when ungrouped signals are read in.  This is done with 
  // a search every time so the order/number of gain factors and units in 
  // the index file can differ.
  EST_TokenStream ts;
  int i,k;
  EST_String line;
  EST_String diph;
  float gL,gR;  

  p_gainL.resize(unitdic.Nunits());
  p_gainR.resize(unitdic.Nunits());

  // fill with 1.0
  p_gainL.fill(1.0);
  p_gainR.fill(1.0);

  if (fileinfo.sval("gain_file") == "none"){
    cerr << "no gain file specified\n";
    return;  // defaults to 1.0 have been set
  }

  if (ts.open(fileinfo.sval("gain_file")) == -1){
    EST_error(EST_String("OGIresLPC_db: Can't open gain factor file ") + fileinfo.sval("gain_file"));
  }

  ts.set_WhiteSpaceChars(" \t\n");
  ts.set_SingleCharSymbols("");
  ts.set_PunctuationSymbols("");
  
  for (i=0; (!ts.eof()) && (i<unitdic.Nunits());){
    line = (const EST_String)ts.get_upto_eoln();
    if ((line.length() > 0) && (line[0] != ';')){

      EST_TokenStream ls;
      ls.set_WhiteSpaceChars(" \t\n");
      ls.set_SingleCharSymbols("");
      ls.set_PunctuationSymbols("");

      // parse the line
      ls.open_string(line);
      diph = ls.get().string();
      gL = (float)atof(ls.get().string());
      gR = (float)atof(ls.get().string());
      
      // search for matching unit
      k = name_lookup(diph);
      if (k == -1)
	cerr << "no match found for gain file unit " <<  diph << endl;
      else {
	p_gainL[k] = gL;
	p_gainR[k] = gR;
      }

      // end the line
      ls.close();
      i++;
    }
  }

  printf(" %d units in index file, %d in gain file\n",unitdic.Nunits(),i);
  ts.close();
}


////////////////////////////////////////////////////
////// utilities    ////////////////////////////////
////////////////////////////////////////////////////


void OGIresLPC_db::resize(int N){
  p_exc.resize(N);
  p_excB.resize(N); 
  p_excE.resize(N); 
  p_exc_isLoaded.resize(N);
  p_exc_offset.resize(N); 
  p_pm.resize(N);
  p_pm_isLoaded.resize(N);
  p_pm_offset.resize(N);
  p_lpc.resize(N);
  p_lpc_isLoaded.resize(N);
  p_lpc_offset.resize(N); 
}

// cut unused portions of data
void OGIresLPC_db::trim_unit(
			     int i,      // unit indx
			     int extraL, // samples
			     int extraR, // samples
			     float Fs    // samp freq
			     ){

  // NOTE:  at this point, t() of each Track has already been shifted 
  //   to be relative to excB.
  int newexcB = max(0, excB(i)-extraL);
  int newexcE = min((*exc(i)).length(), excE(i)+extraR);

  //// PM
  // find frames inside unit
  int pm_frB = (*pm(i)).index_below(0.0) + 1;
  int pm_frE = (*pm(i)).index_below((excE(i)-excB(i))/Fs);
  if ((pm_frB < 0) || (pm_frE >= (*pm(i)).num_frames()))
    EST_error("how can this be so?\n");

  // create a smaller one, delete old one
  EST_Track *newpm = new EST_Track;
  (*pm(i)).copy_sub_track(*newpm, pm_frB, pm_frE-pm_frB+1);
  delete (EST_Track *) p_pm[i];
  p_pm[i] = newpm;
  
  //// LPC
  // find frames inside unit
  int lpc_frB = (*lpc(i)).index_below(0.0) + 1;
  int lpc_frE = (*lpc(i)).index_below((excE(i)-excB(i))/Fs);
  if ((lpc_frB < 0) || (lpc_frE >= (*lpc(i)).num_frames()))
    EST_error("how can this be so?\n");

  // create a smaller one, delete old one
  EST_Track *newlpc = new EST_Track;
  (*lpc(i)).copy_sub_track(*newlpc, lpc_frB, lpc_frE-lpc_frB+1);
  delete (EST_Track *) p_lpc[i];
  p_lpc[i] = newlpc;

  //// EXC  
  EST_Wave *newwav = new EST_Wave(newexcE-newexcB, 1, ((EST_Wave *)p_exc[i])->sample_rate());
  short *s_old = ((EST_Wave *)p_exc[i])->values().memory();
  newwav->set_channel(0,  &(s_old[newexcB]), 0, newexcE-newexcB);
  delete (EST_Wave *) p_exc[i];
  p_exc[i] = newwav;

  // EXC bounds info to reflect part we cut
  p_excB[i] = excB(i) - newexcB;
  p_excE[i] = excE(i) - newexcB;
}

////////////////////////////////////////////
// Implement functions of the base class  //
////////////////////////////////////////////
//
// return data type of loaded db
//
LISP OGIresLPC_db::lispify_data_type(void){
  return strintern(EST_String("resLPC"));
}

//
// parse the parameters of the db, record the database information
//
void OGIresLPC_db::parse_lisp_parameters(LISP params, OGIbool isGrp){
  EST_String sval;

  // used for either grp or ungrp
  name = wstrdup(get_param_str("dbname",params,"db"));
  get_access_mode(params);
  dbglobal.override_fval("preemph", get_param_float("preemph",params,(float)0.96));
  dbglobal.override_val("phoneset", wstrdup(get_param_str("phoneset",params,"none")));

  if (!isGrp){  // for ungrouped only (necessary ones will all be loaded already in grp case)
    fileinfo.override_val("unitdic_file",wstrdup(get_param_str("unitdic_file",params,"")));
    fileinfo.override_val("gain_file", wstrdup(get_param_str("gain_file",params,"none")));
    fileinfo.override_val("base_dir", wstrdup(get_param_str("base_dir",params,"./")));

    // generic one
    fileinfo.override_val("data_dir", wstrdup(get_param_str("data_dir",params,"data/")));
    fileinfo.override_val("data_ext", wstrdup(get_param_str("data_ext",params,".out")));

    // specific case for resLPC
    fileinfo.override_val("lpc_dir", wstrdup(get_param_str("lpc_dir",params,"lpc/")));
    fileinfo.override_val("lpc_ext", wstrdup(get_param_str("lpc_ext",params,".lpc")));
    fileinfo.override_val("exc_dir", wstrdup(get_param_str("exc_dir",params,"lpc/")));
    fileinfo.override_val("exc_ext", wstrdup(get_param_str("exc_ext",params,".res")));
    fileinfo.override_val("pm_dir", wstrdup(get_param_str("pm_dir",params,"pm/")));
    fileinfo.override_val("pm_ext", wstrdup(get_param_str("pm_ext",params,".pmv")));
    dbglobal.override_val("phoneset", wstrdup(get_param_str("phoneset",params,"none")));
    dbglobal.override_ival("lpc_order", get_param_int("lpc_order",params,0));
    dbglobal.override_fval("sig_band", get_param_float("sig_band",params,0)); // xtra samps saved around unit (in seconds!)
    Fs = get_param_float("samp_freq",params,16000.0); // in Hz
    isCompressed = (OGIbool) atobool(wstrdup(get_param_str("isCompressed",params,"Y"))); // compress when saved
    //udata_type = get_data_type(params);  // resLPC, LPC, sine

  }
}

//////////////////////////////////////////////
////// ungrouped data access /////////////////
//////////////////////////////////////////////

void OGIresLPC_db::load_ungrouped(LISP params)
{
  isGrouped = FALSE;

  // parse params passed in from scm by OGIdb.init
  parse_lisp_parameters(params,isGrouped);

  // Load the unit info from the unitdic file
  EST_String unitdic_file = fileinfo.sval("unitdic_file");
  unitdic.load_file(unitdic_file, Fs);

  // build hash access table for unit names
  build_hash();

  // -load the data
  load_ungrp_udata();
}

//////////////////////////////////////////////
////// grouped data access ///////////////////
//////////////////////////////////////////////
void OGIresLPC_db::load_grouped(EST_String &groupfile, LISP usr_params){
  isGrouped = TRUE;

  // open grp file, read magic #
  gfd = open_groupfile_read(groupfile);
  swap = check_groupfile_swap(gfd);

  // load header info in grp file
  load_grp_params();

  // passed in by user
  parse_lisp_parameters(usr_params,isGrouped);

  // load unitdic info
  unitdic.load_grp(gfd, swap);

  // build hash access table for unit names
  build_hash();

  // -read default params as specified in the groupfile
  // -overwrite with user-specified params
  // -create model-specific data obj
  // -load the data
  load_grp_udata();
}

void OGIresLPC_db::save_grouped(EST_String &groupfile){

  isGrouped = FALSE;
  unsigned int magic = OGI_DBASE_MAGIC;

  // open file, write magic num
  gfd = open_groupfile_write(groupfile);
  fwrite(&magic,sizeof(int), 1, gfd);

  // write header, dbase params
  save_grp_params();

  // write unitdic info
  unitdic.save_grp(gfd);

  // save data
  save_grp_udata();
  
  fclose(gfd);
}

static void time_shift_track(EST_Track &tr, float ts){
  int k;
  float t_new;
  for (k=0; k<tr.num_frames(); k++){
    t_new = tr.t(k) + ts;
    tr.t(k) = t_new;
  }
}

/*****************************************************/
/* To avoid RE-instantiation, I've cleaned up these  */
/* templates and move them to ../OGIcommon/inst_tmpl */
/* Vector of all types of pointers are combined into */
/* EST_TVector<void *> now.                          */
/*                            -- niuxc               */
/*****************************************************/

//#if defined(INSTANTIATE_TEMPLATES)
//#include "../../../../speech_tools/base_class/EST_TVector.cc"
//template class EST_TVector<long>;
//template class EST_TVector<EST_Wave *>;
//template class EST_TVector<EST_Track *>;
//template class EST_TVector<void *>;
//#endif

//
// long
//
//template ostream& operator << (ostream &st, const EST_TVector<long> &v);
//static long const def_val_long = -1;
//long const *EST_TVector<long>::def_val = &def_val_long;
//static long error_return_long = -1;
//long *EST_TVector<long>::error_return = &error_return_long;

//
// void *
//
//static void * const def_val_VoidVect = NULL;
//void * const *EST_TVector<void *>::def_val = &def_val_VoidVect;
//static void *error_return_VoidVect = NULL;
//void * *EST_TVector<void *>::error_return = &error_return_VoidVect;

//
// Wave *
//
//static EST_Wave * const def_val_WaveVect = NULL;
//EST_Wave * const *EST_TVector<EST_Wave *>::def_val = &def_val_WaveVect;
//static EST_Wave *error_return_WaveVect = NULL;
//EST_Wave * *EST_TVector<EST_Wave *>::error_return = &error_return_WaveVect;
 
//
// Track *
//
//static EST_Track * const def_val_TrackVect = NULL;
//EST_Track * const *EST_TVector<EST_Track *>::def_val = &def_val_TrackVect;
//static EST_Track *error_return_TrackVect = NULL;
//EST_Track * *EST_TVector<EST_Track *>::error_return = &error_return_TrackVect;
