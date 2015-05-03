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
#ifndef __OGIFILTER_H__
#define __OGIFILTER_H__

#include "EST.h"

void OGIfiltfilt(EST_DVector &b, EST_DVector &a, EST_Wave &sig);
void OGIfiltfilt(EST_DVector &b, EST_DVector &a, EST_DVector &sig);
void reverse(EST_DVector &sig);
void OGIfilter(EST_DVector &b_in, EST_DVector &a_in, EST_Wave &sig);
void OGIfilter(EST_DVector &b_in, EST_DVector &a_in, EST_DVector &sig);
void OGIzerophase_FIRfilter(EST_DVector &b, EST_Wave &sig);
void OGIzerophase_FIRfilter(EST_DVector &b, EST_DVector &sig);
void OGIzerophase_FIRfilter(EST_DVector &b, EST_Track &sig);
void design_notch_filter( float angle, float bw, EST_DVector &B, EST_DVector &A);
void filtlist_to_vector(EST_String lstring, EST_DVector &b, EST_DVector &a);
void filtlist_to_vector(LISP lfilt, EST_DVector &b, EST_DVector &a);

void ar_convolve(double *x, double *lpc, int Nlpc, double *out, int Nout);
void fir_convolve(double *x, double *lpc, int Nlpc, double *out, int Nout);
void OGIpreemphasis(EST_Wave &sig, float a);
#endif

