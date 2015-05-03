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
#ifndef __VC_H_
#define __VC_H_

// Alexander Kain, 1998

#include "matrix.h"
#include "gmm.h"
#include "EST.h"

static Gmm G; // Voice Conversion Gmm Model


void vc_init(char filename[]);
//void vc_do(float **inarray, int n, int N);
void vc_do(EST_Track &inarray, EST_Track &V_UV);


#endif // __VC_H_
