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


#include "OGI_file_compr.h"

/////////////////////////////////////////////////////////
// compressed wave
/////////////////////////////////////////////////////////
int OGIfread_compr_wav(short *s, int Ns, FILE *fp, bool swap){
  
  short maxval;
  unsigned char *ulawbuf;
  int N;

  fread(maxval, sizeof(short), 1, fp);
  if (swap) 
    swap_bytes_short(maxval, 1);

  ulawbuf = walloc(unsigned char, Ns);
  N = fread(ulawbuf, sizeof(unsigned char), Ns, fp);
  if (N != Ns){
    EST_error("OGIfread_compr_Wave: problem reading wave\n");
  }
  ulaw_to_short(ulawbuf, s, Ns);
  scale_short_vect(s, maxval/(float) OGI_MAXSHORT, Ns);
  wfree(ulawbuf);

  return N;
} 

int OGIfwrite_compr_wav(short *s, int Ns, FILE *fp){
  //  Find max/min of signal, scale to max dyn range.
  //  Save max ampl and ulaw compressed scaled version.
  short maxval;
  unsigned char *ulawbuf;
  int N;

  // Find scale factor, scale signal, dump
  maxval = short_vect_maxabs(s, Ns);
  scale_short_vect(s, OGI_MAXSHORT/(float)maxval, Ns);
  fwrite(&maxval, sizeof(short), 1, fp);

  // Convert to ulaw, dump
  ulawbuf = walloc(unsigned char, Ns);
  short_to_ulaw(s, ulawbuf, Ns);
  N = fwrite(ulawbuf, sizeof(unsigned char), Ns, fp);
  if (N != Ns){
    EST_error("OGIfwrite_compr_Wave: problem writing wave\n");
  }
  wfree(ulawbuf);

  return N;
}


/////////////////////////////////////////////////////////
// simple utilities 
/////////////////////////////////////////////////////////

static short short_vect_maxabs(
			       short *v1,
			       int len) {
  short smax;                               /* The maximum.                 */
  smax=  abs(*v1);                          /* Initial maximum value.       */
  for (; len-->0; v1++)                   
    smax = smax < abs(*v1) ? abs(*v1) : smax;  /* Update max if necessary.  */
  return smax;                              /* Return the max value.        */
}

static void scale_short_vect(
			     short *v,
			     float sc,
			     int len){
  for (; len-->0; v++)
    *v = (short) (*v * sc);
}

static void scale_float_to_ushort(
				 float *f,
				 unsigned short *s,
				 float sc,
				 int len){
  for (; len-->0; )
    *s++ = (unsigned short) (*f++ * sc);
}
static void scale_ushort_to_float(
				  unsigned short *s,
				  float *f,
				  float sc,
				  int len){
  for (; len-->0; )
    *f++ = (float) (*s++ * sc);
}


#define check_ov(A) if (A < 0){ cerr << "di_compress: diff pitchmarks < 0!\n";abort();} else if (A > MAXUSHORT){ cerr << "di_compress: diff pitchmarks > MAXUSHORT !\n";abort();}


static void int_to_ushort_diff(
			  int *ibuf,
			  unsigned short *usbuf, 
			  int N){
  
  int k;
  int diff=0;


  if (N > 0){
    check_ov(*ibuf);
    *usbuf++ = (unsigned short) *ibuf++;

    for (k=1; k<N; k++){
      diff =  *ibuf - *(ibuf-1);
      check_ov(diff); // check, since this could be hard to track down
      *usbuf = (unsigned short) diff;
      usbuf++, ibuf++;
    }
  }
}

static void ushort_diff_to_int(
			  unsigned short *usbuf, 
			  int *ibuf,
			  int N){
  int k,tot=0;

  if (N>0){
    for (k=0; k<N; k++){
      tot += (int) *usbuf++;
      *ibuf++ = tot;
    }
  }
}

