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
#ifndef __OGI_VECTOR_H__
#define __OGI_VECTOR_H__

#include "EST.h"

#define I_CAN_GET_TEMPLATES_TO_WORK 1

//
// very simple vector classes: just a C array that knows its size
//
#if I_CAN_GET_TEMPLATES_TO_WORK

template <class X> class OGI_Vect {
private:
  //
public:
  X *a;
  int N;

  // constructor
  OGI_Vect(void){
    N=0;
  }
  OGI_Vect(int n){
    N=n;
    a = walloc(X, N);
  }

  void resize(int n){
    N=n;
    a = walloc(X, N);
  }

  void clear(void){
    memset(a, 0, N*sizeof(X));
  }

  ~OGI_Vect(){
    wfree(a);
  }

  INLINE int length(){
    return N;
  }
};


typedef OGI_Vect<double> OGI_VectDouble;
typedef OGI_Vect<float> OGI_VectFloat;
typedef OGI_Vect<int> OGI_VectInt;

#else


class OGI_VectDouble {
private:
  //
public:
  double *a;
  int N;

  OGI_VectDouble(void){  // constructor
    N=0;
  }
  OGI_VectDouble(int n){ // constructor
    N=n;
    a = walloc(double, N);
  }
  void clear(void){
    memset(a, 0, N*sizeof(double));
  }
  void resize(int n){
    N=n;
    a = walloc(double, N);
  }
  ~OGI_VectDouble(){
    wfree(a);
  }
  INLINE int length(){
    return N;
  }

};


class OGI_VectFloat {
private:
  //
public:
  float *a;
  int N;

  OGI_VectFloat(void){  // constructor
    N=0;
  }
  OGI_VectFloat(int n){ // constructor
    N=n;
    a = walloc(float, N);
  }
  void clear(void){
    memset(a, 0, N*sizeof(float));
  }
  void resize(int n){
    N=n;
    a = walloc(float, N);
  }
  ~OGI_VectFloat(){
    wfree(a);
  }
  INLINE int length(){
    return N;
  }

};
#endif


void f2d_convert(OGI_VectFloat &f, OGI_VectDouble &d);
void d2f_convert(OGI_VectDouble &d, OGI_VectFloat &f);

#endif

