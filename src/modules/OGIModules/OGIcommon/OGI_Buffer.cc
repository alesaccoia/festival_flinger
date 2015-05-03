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
#include "OGI_Buffer.h"

static double tri_win_normsq(double *v, int N, int signflag);  
static double rect_win_normsq(double *v, int N);

// log RMS power after triangular window
double OGI_PitchSyncBuffer::RMSpower_triangle(void){
  
  double Ps;
  
  Ps  = tri_win_normsq(buf, mid(), 1);
  Ps += tri_win_normsq(&(buf[mid()]), end()-mid(), -1);
  
  if (Ps < (double) p_end)
    return 1.0; // threshold for silence
  
  // mult by 2 - ASSUMES TRIANGULAR WINDOW
  // log(sqrt(mean(s^2))) = log(RMS(s))
  Ps =  0.5*log(2*Ps/p_end); 
  
  return Ps;
}

// log RMS power after trapezoid window
double OGI_PitchSyncBuffer::RMSpower_trapezoid(void){
  int N,b;
  double Ps=0.0;
  int win_len = 0;
  
  // positive-going side 
  N = mid()/2; 
  b = mid() - N;
  Ps += 2*tri_win_normsq(&(buf[b]), N, 1);
  win_len += N;

  // flat part
  b = mid();
  N = (end() - mid())/2;
  Ps += rect_win_normsq(&(buf[b]), N);
  win_len += N;
  
  // negative-going side
  b = b + N;
  N = end()-b;
  Ps += 2*tri_win_normsq(&(buf[b]), N, -1);
  win_len += N;
  
  if (Ps < (double) p_end)
    return 1.0; // threshold for silence
  
  // log(sqrt(mean(s^2))) = log(RMS(s))
  Ps =  0.5*log(Ps/win_len); 
  
  return Ps;
}


static double tri_win_normsq(double *v, int N, int signflag){
  //  use half of a triangular window 
  double incr;
  double win;
  double accum = 0.0; 

  if (N > 0){
    incr = 1.0/N;
    if (signflag > 0)
      win = 0.0;
    else
      win = 1.0;
    incr *= signflag;
    
    // NOTE: window applied _after_ squaring
    while(N-->0) { 
      accum += win * (*v)*(*v); 
      v++;
      win += incr;
    }    
  }
  return accum;
}

static double rect_win_normsq(double *v, int N){
  double accum = 0.0;   
  while(N-->0) { 
    accum += (*v)*(*v); 
    v++;
  }    
  return accum;
}


void OGI_PitchSyncBuffer::save(EST_String &fname, int Fs, float gain){

  short *sbuf = (short *) malloc(p_end * sizeof(short));
  
  //  cout << "\t" << fname << " " << p_Nbuf << " " << p_mid << " " << p_end;
  //  printf("\t%p -- %p     %p -- %p \n", buf, buf+p_Nbuf, sbuf, sbuf+p_end);

  for (int k=0; k<p_end; k++){
    sbuf[k] = (short) (buf[k] * gain);
  }

  EST_Wave *out_EST_Wave = new EST_Wave(p_end, 1, sbuf, 0, Fs, 1);
  out_EST_Wave->save(fname, "riff");  
  delete out_EST_Wave;

}
