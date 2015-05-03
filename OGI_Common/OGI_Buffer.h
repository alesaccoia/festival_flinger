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
#ifndef __OGI_BUFFER_H__
#define __OGI_BUFFER_H__

#include "EST.h"
  
class OGI_BuffFVect : public EST_FVector {

  // same as EST_FVector, but with
  //   buffered append() to reduce # of mallocs
private:
  int buff_incr;
  int pNelem;  // length of full part of buffer
public:

  OGI_BuffFVect(int buflen){
    buff_incr = buflen;
    pNelem = 0;
  }

  // append a single time point
  void append(float v_in){
    if (pNelem+1 > length()){
      resize(length() + buff_incr);
    }
    a_no_check(pNelem) = v_in;
    pNelem++;
  }

  // this is the length of the part in use
  int Nelem(void){
    return pNelem;
  }

  float last(void){
    if (pNelem < 1)
      EST_error(" OGI_BuffFVect asked for last of 0 element vector\n");
    return a_no_check(pNelem-1);
  }
};


//////////////////////
class OGI_PitchSyncBuffer{
private:
  int p_Nbuf;
  int p_mid;
  int p_end;
public:

  double *buf;


  // constructor
  OGI_PitchSyncBuffer(int n){
    p_Nbuf=n;
    buf = walloc(double, p_Nbuf);
  }
  ~OGI_PitchSyncBuffer(){
    wfree(buf);
  }

  INLINE void clear(){
    memset(buf, 0, (p_Nbuf)*sizeof(double));
  }

  INLINE void fill(double x){
    int k;
    for (k=0; k<p_Nbuf; k++)
      buf[k] = x;
  }

  INLINE void reverse(void){
    // just between 0 and p_end
    double *tmpbuf = walloc(double, p_end);
    memcpy(tmpbuf, buf, p_end*sizeof(double));
    int k;
    for (k=0; k<p_end; k++)
      buf[k] = tmpbuf[p_end-k-1];
    wfree(tmpbuf);
  }
	
  INLINE void set_mid_end(int m, int e){
    p_mid = m;
    p_end = e;
    //cerr << "end: " << p_end << " Nbuf: " << p_Nbuf << endl;
 
    if (p_end > p_Nbuf){
      cerr << "OGI_PitchSyncBuffer: setting end=" << p_end << " beyond Nbuf=" << p_Nbuf << endl;
    }
  }

  INLINE int end(void){
    return p_end;
  }
  INLINE int mid(void){
    return p_mid;
  }
  INLINE int Nbuf(void){
    return p_Nbuf;
  }

  void copy_from( OGI_PitchSyncBuffer &from){
    if (from.end() <= p_Nbuf){
      clear();
      memcpy(buf, from.buf, from.end()*sizeof(double));
      set_mid_end(from.mid(), from.end());
    }
    else {
      EST_error("OGI_PitchSyncBuffer: copy_from size %d to buffer of size %d\n",from.end(),p_Nbuf);
    }    
  }


  // log RMS power after triangular window
  double RMSpower_triangle(void);
  
  
  // log RMS power after trapezoid window
  double RMSpower_trapezoid(void);


  // save to 'riff' wav file -- for debug
  void save(EST_String &fname, int Fs, float gain);
};

#endif  // #ifndef __OGI_BUFFER_H__

