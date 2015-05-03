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
#ifndef __OGI_WAVE_H__
#define __OGI_WAVE_H__

#include "EST.h"
#include "OGI_Macro.h"

class OGI_Wave {
  // not as ambitious as EST_Wave, but has
  //   buffered append() to reduce # of mallocs
  //
  // Can access memory to create EST_Wave without copy.
  // DON'T free if you do this!
  //
private:
  short *buf;
  int buf_incr;
  int pNbuf;  // total length
  int pNused;  // length of already-filled part of buffer
public:

  OGI_Wave(){
    buf_incr = 1;
    buf = walloc(short, buf_incr);
    pNbuf = buf_incr;
    pNused = 0;
  }

  OGI_Wave(int incr){
    buf_incr = incr;
    buf = walloc(short, incr);
    pNbuf = incr;
    pNused = 0;
  }
  ~OGI_Wave(){
    //NOTE:  OGI_Wave does _not_ free memory: EST_Wave will do this in utterance free
  }

  void set_incr(int incr){
    buf_incr = incr;
  }
  int Nbuf(void){
    return pNbuf;
  }

  // append a double vector, quantize & clip
  bool append_clip(double *v_in, int Nv_in){

    if (pNused+Nv_in > pNbuf){
      pNbuf = pNused + Nv_in + buf_incr;
      buf  = wrealloc(buf, short, pNbuf);
    }

    bool clip = FALSE;
    int k;
    for (k=0; k < Nv_in; k++){
      if (fabs(v_in[k]) > (double) OGI_MAXSHORT){
	clip = TRUE;
	buf[k+pNused] = (short) (max(min(v_in[k], OGI_MAXSHORT), OGI_MINSHORT));
      }
      else
	buf[k+pNused] = (short) v_in[k];

      //      printf("\t buf[%d] (%p) = clip(v[%d])=%f (%p)\n",k+pNused, &(buf[k+pNused]), k, v_in[k], &(v_in[k]));
    }
    pNused += Nv_in;
    return clip;
  }

  // this is the length of the part in use
  int Nused(void){
    return pNused;
  }

  // this returns a pointer to the memory
  short *mem(void){
    return buf;
  }

  // DON'T free if you pass ptr to EST_Wave
  void memfree(void){
    wfree(buf);
  }



  void save(const EST_String &filename){
    cout << "Saving with default sampling rate of 16kHz\n";
    save(filename, "nist",16000.0);
  }

  void save(const EST_String &filename, const EST_String &ftype, float Fs){
    EST_Wave *tmpwav = new EST_Wave(pNused, 1, buf, 0, (int)Fs, 0);
    tmpwav->save(filename, ftype);
    delete tmpwav;
  }
    
};

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

class OGI_Wave_D {
  // same as above, but stores wave as doubles
  //
private:
  double *buf;
  int buf_incr;
  int pNbuf;  // total length
  int pNused;  // length of already-filled part of buffer
public:

  OGI_Wave_D(){
    buf_incr = 1;
    buf = walloc(double, buf_incr);
    pNbuf = buf_incr;
    pNused = 0;
  }

  OGI_Wave_D(int incr){
    buf_incr = incr;
    buf = walloc(double, incr);
    pNbuf = incr;
    pNused = 0;
  }
  ~OGI_Wave_D(){
    // NOTE:  OGI_Wave_D *DOES* free memory, unlike above
    if (buf != NULL)
      wfree(buf);
  }

  void set_incr(int incr){
    buf_incr = incr;
  }
  int Nbuf(void){
    return pNbuf;
  }

  // append a double vector
  double append(double *v_in, int Nv_in){
    double max_v = 0.0;
    if (pNused+Nv_in > pNbuf){
      pNbuf = pNused + Nv_in + buf_incr;
      buf  = wrealloc(buf, double, pNbuf);
    }

    int k;
    for (k=0; k < Nv_in; k++){
      buf[k+pNused] = (double) v_in[k];
      max_v = max(fabs(v_in[k]), max_v);
    }
    pNused += Nv_in;
    return max_v;
  }
  
  void rescale(double g){
    for (int k=0; k < pNused; k++){
      buf[k] *= g;
    }
  }

  // this is the length of the part in use
  int Nused(void){
    return pNused;
  }

  // this returns a pointer to the memory
  double *mem(void){
    return buf;
  }

  void memfree(void){
    wfree(buf);
  }

};




#endif 

