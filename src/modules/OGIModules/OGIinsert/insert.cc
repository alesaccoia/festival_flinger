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
#include "OGI_Macro.h"

static LISP OGIinsert_wave(    LISP utt, LISP lfilename, LISP ltime);
static LISP OGIbackground_wave(LISP utt, LISP lfilename, LISP ltime);
static LISP OGIoverwrite_wave( LISP utt, LISP lfilename, LISP ltime);

static LISP OGIinsert_wave(LISP utt,LISP lfilename,LISP ltime){
  // Main utterance initialization
  EST_String filename = get_c_string(lfilename);
  float time = get_c_float(ltime);
  int ins_dur,sr,ins_samp;

  EST_Utterance *u = get_c_utt(utt);
  EST_Item *waveitem =  u->relation("Wave")->head();
  EST_Wave *utt_wave = wave(waveitem->f("wave"));
  EST_Wave ins_wave = EST_Wave();
  ins_wave.load(filename,0,0);

  // resample
  if (ins_wave.sample_rate() != utt_wave->sample_rate())
    ins_wave.resample(utt_wave->sample_rate());

  sr = utt_wave->sample_rate();      // sample rate
  ins_samp = (int) (time*sr);        // where to insert
  if (ins_samp <= 0)    ins_samp = 1;
  ins_dur = ins_wave.num_samples();  // dur of inserted part

  EST_Wave tmpwav;
  EST_Wave *new_wave = new EST_Wave();

  // if time to insert is past end, append 0's
  if (ins_samp < utt_wave->num_samples())
    utt_wave->sub_wave(tmpwav, 0, ins_samp-1);
  else {
    tmpwav = *utt_wave;
    tmpwav.resize(ins_samp, EST_ALL, 1);
  }

  *new_wave = tmpwav;
  *new_wave += ins_wave;

  if (ins_samp < utt_wave->num_samples()){
    utt_wave->sub_wave(tmpwav, ins_samp, EST_ALL);
    *new_wave += tmpwav;
  }

  // clear the old one (is this enough?)
  utt_wave->clear();

  // set new wave into utterance struct
  waveitem->set_val("wave", est_val(new_wave));

  return flocons((float) ins_dur/sr);
}



//
// add background sounds
//
static LISP OGIbackground_wave(LISP utt,LISP lfilename,LISP ltime){
  // Main utterance initialization
  EST_String filename = get_c_string(lfilename);
  float time_on, time_off;
  int k,l;

  if (siod_llength(ltime) > 1){
    time_on  = get_c_float(car(ltime));
    time_off = get_c_float(car(cdr(ltime)));
  }
  else{
    time_on  = get_c_float(ltime);
    time_off = 0.0;
  }
    
  int ins_dur,sr,ins_samp;

  EST_Utterance *u = get_c_utt(utt);
  EST_Item *waveitem =  u->relation("Wave")->head();
  EST_Wave *utt_wave = wave(waveitem->f("wave"));
  EST_Wave ins_wave = EST_Wave();
  EST_Wave add_wave = EST_Wave();
  ins_wave.load(filename,0,0);

  // resample
  if (ins_wave.sample_rate() != utt_wave->sample_rate())
    ins_wave.resample(utt_wave->sample_rate());

  sr = utt_wave->sample_rate();      // sample rate
  ins_samp = (int) (time_on*sr);        // where to insert
  if (ins_samp <= 0)    ins_samp = 1;
  ins_dur = ins_wave.num_samples();  // dur of inserted part


  // looping
  if (time_off == -1.0)
    time_off = utt_wave->num_samples()/(float)sr;

  add_wave = ins_wave;
  int t = ins_samp + ins_dur;  
  if (time_off > time_on) {
    // loop it
    while (t < time_off*sr) {
      add_wave += ins_wave;
      t += ins_dur;
    }
  }

  // add together
  /// bug workaround
  short x = 1;
  utt_wave->values().def_val = &x;
  utt_wave->resize(max(add_wave.num_samples() + ins_samp,  utt_wave->num_samples()), EST_ALL, 1);

  for (k=ins_samp,l=0; k<utt_wave->num_samples(); k++,l++){
    if (l >= add_wave.num_samples())
      break;
    utt_wave->a(k) = clip( utt_wave->a(k) + add_wave.a(l));
  }
    
  return utt;
}

static LISP OGIoverwrite_wave(LISP utt,LISP lfilename,LISP ltime){
  // Main utterance initialization
  EST_String filename = get_c_string(lfilename);
  float time = get_c_float(ltime);
  int ins_dur,sr,ins_samp;

  EST_Utterance *u = get_c_utt(utt);
  EST_Item *waveitem =  u->relation("Wave")->head();
  EST_Wave *utt_wave = wave(waveitem->f("wave"));
  EST_Wave ins_wave = EST_Wave();
  ins_wave.load(filename,0,0);

  // resample
  if (ins_wave.sample_rate() != utt_wave->sample_rate())
    ins_wave.resample(utt_wave->sample_rate());

  sr = utt_wave->sample_rate();      // sample rate
  ins_samp = (int) (time*sr);        // where to insert
  if (ins_samp <= 0)    ins_samp = 1;
  ins_dur = ins_wave.num_samples();  // dur of inserted part

  EST_Wave tmpwav;
  EST_Wave *new_wave = new EST_Wave();

  // if time to insert is past end, append 0's
  if (ins_samp < utt_wave->num_samples())
    utt_wave->sub_wave(tmpwav, 0, ins_samp-1);
  else {
    tmpwav = *utt_wave;
    tmpwav.resize(ins_samp, EST_ALL, 1);
  }

  *new_wave = tmpwav;
  *new_wave += ins_wave;

  // JW: replacing part of old waveform, hence shift ins_sample with ins_dur 
	ins_samp += ins_dur;
	
  if (ins_samp < utt_wave->num_samples()){
    utt_wave->sub_wave(tmpwav, ins_samp, EST_ALL);
    *new_wave += tmpwav;
  }

  // clear the old one (is this enough?)
  utt_wave->clear();

  // set new wave into utterance struct
  waveitem->set_val("wave", est_val(new_wave));

  return flocons((float) ins_dur/sr);
}

 
void festival_OGIinsert_init(void){
  proclaim_module("OGIinsert");

  init_subr_3("utt.wave.insert",OGIinsert_wave,
  "(utt.wave.insert UTT FILENAME TIME) \n\
   Load waveform in FILENAME and insert it into synthesized \n\
   wave of UTT at point TIME. Returns duration of inserted wave.");

  init_subr_3("utt.wave.overwrite",OGIoverwrite_wave,
  "(utt.wave.overwrite UTT FILENAME TIME) \n\
   Load waveform in FILENAME and overwrite synthesized \n\
   wave of UTT at point TIME, for duration of FILENAME.");

  init_subr_3("utt.wave.background",OGIbackground_wave,
  "(utt.wave.background UTT FILENAME TIME) \n\
   Load waveform in FILENAME and insert into synthesized \n\
   wave of UTT at point TIME, for duration of file. If TIME is \n\
   a list of 2 times, loop the wavefile to fill from first to last times.\n\
   Make TIME = (0.0 -1.0) to loop from begin to end of utt");
}


