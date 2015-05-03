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
#include "OGI_Vector.h"


void f2d_convert(OGI_VectFloat &f, OGI_VectDouble &d){
  // trust that they are same size
  float *ff  = f.a;
  double *dd = d.a;
  int k;
  for (k=0; k<f.N; k++){
    *dd = *ff;
    ff++; dd++;
  }

}
void d2f_convert(OGI_VectDouble &d, OGI_VectFloat &f){
  // trust that they are same size
  float *ff  = f.a;
  double *dd = d.a;
  int k;
  for (k=0; k<f.N; k++){
    *ff = (float)*dd;
    ff++; dd++;
  }
}
