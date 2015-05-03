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
#ifndef __OGI_TIMEWARP_H__
#define __OGI_TIMEWARP_H__

#include "OGI_file.h"


#define TIMEWARP_BUF_INCR 8
class OGI_TimeWarp {
private:
  EST_FVector x; // ends of src segs
  EST_FVector y; // ends of targ segs
  int N; // number of segs (also # of warp pts, since 0,0 assumed)
  int Lindx;
  EST_StrList names;// names of src segs
public:
  OGI_TimeWarp(){
    x.resize(TIMEWARP_BUF_INCR);
    y.resize(TIMEWARP_BUF_INCR);
    N = 0;
    Lindx = -1;
  }
  ~OGI_TimeWarp() {}

  void append(float x_new, float y_new, const EST_String &name){
    //cout << x_new << " " << y_new << " " << name << "\n";
    append(x_new, y_new);
    names.append(name);
  }

  void append(float x_new, float y_new);
  void insert_before(int n, float x_new, float y_new);

  int Nseg(){
    return N;
  }

  // begin, end, dur of source
  float src_end(int i){
    if (i >= N)
      cerr << "OGI_TimeWarp: asking for src_end(" << i << ") of " << N << endl;
    return x(i);
  }
  float src_begin(int i){
    if (i >= N)
      cerr << "OGI_TimeWarp: asking for src_begin(" << i << ") of " << N << endl;

     if (i==0)
      return 0.0;
    else // (i > 1)
      return x(i-1);
  }

  float src_dur(int i){
    if (i >= N)
      cerr << "OGI_TimeWarp: asking for src_dur(" << i << ") of " << N << endl;

   if (i==0)
      return x(0);
    else // (i > 1)
      return x(i)-x(i-1);
  }

  // begin, end, dur of target
  float targ_end(int i){
    if (i >= N)
      cerr << "OGI_TimeWarp: asking for targ_end(" << i << ") of " << N << endl;


    return y(i);
  }
  float targ_begin(int i){
    if (i >= N)
      cerr << "OGI_TimeWarp: asking for targ_begin(" << i << ") of " << N << endl;

    if (i==0)
      return y(0);
    else // (i > 1)
      return y(i-1);
  }
  float targ_dur(int i){
    if (i >= N)
      cerr << "OGI_TimeWarp: asking for targ_dur(" << i << ") of " << N << endl;

   if (i==0)
      return y(0);
    else // (i > 1)
      return y(i)-y(i-1);
  }
  float last_targ_end(void){
   if (N > 0)
     return y(N-1);
   else
     return 0.0;
  }
  void modify_src_dur(int indx, float expand){
    int k;
    for (k=indx; k<N; k++){
      x[k] += expand;
    }
    Lindx = -1; // reset invmap search
  }

  void nonuniform_stretch(int nseg, float modfactor, float breakpt, int half);

  float modfactor(int i){
    if (i==0)
      return y(0)/x(0);
    else // (i > 1)
      return (y(i)-y(i-1))/(x(i)-x(i-1));
  }

  // given y_in, return corresponding interpolated x
  //   also return index info as float:  (indxL).(g)  where .00 <= g <= .9999..
  float invmap(float y_in, float *x_indx);
  float invmap(float y_in){
    float xtmp;
    return invmap(y_in, &xtmp);
  }

  // given x_in, return corresponding interpolated y
  //   also return index info as float:  (indxL).(g)  where .00 <= g <= .9999..
  float fwdmap(float x_in, float *y_indx);
  float fwdmap(float x_in){
    float ytmp;
    return fwdmap(x_in, &ytmp);
  }

  void save_src(const EST_String &filename, const EST_String &type="ogi"){
    if (type == "ogi")
      save_OGI_label(x, names, filename, N);
    else
      EST_error("OGI_TimeWarp::save_src - only support OGI format for now\n");
  }

  void save_targ(const EST_String &filename, const EST_String &type="ogi"){
    if (type == "ogi")
      save_OGI_label(y, names, filename, N);
    else
      EST_error("OGI_TimeWarp::save_targ - only support OGI format for now\n");
  }
};

#endif

