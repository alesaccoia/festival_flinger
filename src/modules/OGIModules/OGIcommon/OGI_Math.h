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
#ifndef __OGI_MATH_H__
#define __OGI_MATH_H__

#include "EST.h"


typedef struct {
  double r, i;
} fcomplex;
 
 
fcomplex Complex(double a, double b);
fcomplex Cadd(fcomplex a, fcomplex b);
fcomplex Csub(fcomplex a, fcomplex b);
fcomplex Cmul(fcomplex a, fcomplex b);
fcomplex Cdiv(fcomplex a, fcomplex b);
fcomplex RCmul(double a, fcomplex b);
double Cabs(fcomplex a);
double Cangle(fcomplex a); /* interval [-PI,PI] */
fcomplex Csqrt(fcomplex a);
fcomplex Cp2r(double mag, double ang);
 
int zroots(
            fcomplex a[],
            int m,
            fcomplex roots[],
            int polish);
int laguer(
            fcomplex a[],
            int m,
            fcomplex *x,
            int *its);


int zroots_sorted_angles(
		   fcomplex a[],
		   int m,
		   fcomplex roots[],
		   double ang[]);
#endif
