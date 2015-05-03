/****************************<--OGI-->*************************************
*                                                                        *
*             Center for Spoken Language Understanding                   *
*        Oregon Graduate Institute of Science & Technology               *
*                         Portland, OR USA                               *
*                        Copyright (c) 2000                              *
*                                                                        *
*      This module is not part of the CSTR/University of Edinburgh       *
*               release of the Festival TTS system.                      *
*                                                                        *
*  In addition to any conditions disclaimers below, please see the file  *
*  "license_ogi_tts.txt" distributed with this software for information  *
*  on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  *
*                                                                        *
****************************<--OGI-->*************************************/
#ifndef __OGI_LPC_H__
#define __OGI_LPC_H__

//#include <stdlib.h>
//#include <iostream>
//#include <fstream>
#include "EST.h"
#include "OGI_Vector.h"
#include "OGI_Math.h"


///////////////////////////////////////////////////
//  LSF_TO_LPC conversion function object
//   - configure once by instantiating, then
//     call convert() to do efficient conversion
class OGI_LSF_to_LPC{
private:
  double *LSF_Ptmp;
  double *LSF_P;
  double *LSF_Q;
  double *PCONV_ptmp;
  int order;
public:
  OGI_LSF_to_LPC(int order_in, float bwexp_in=1.0){
    order = order_in;
    LSF_Ptmp =   walloc(double, (order+1));
    LSF_P =      walloc(double, (order+1));
    LSF_Q =      walloc(double, (order+1));
    PCONV_ptmp = walloc(double, (order+1));
  }

  ~OGI_LSF_to_LPC(){
    wfree(LSF_Ptmp);
    wfree(LSF_P);
    wfree(LSF_Q);
    wfree(PCONV_ptmp);
  }

  void check_order_lsf(int N){
    if (N != order){
      EST_error("OGI_LSF_to_LPC: input LSF vector (len=%d) does not match order %d\n", N, order);
    }
  }
  void check_order_lpc(int N){
    if (N != order+1){
      EST_error("OGI_LSF_to_LPC: output LPC vector (len=%d) does not match necessary length %d (order+1)\n", N, order+1);
    }
  }

  void pconv(double *root_angles, int Nra, double *poly);
  void convert(OGI_VectDouble &lsfs, OGI_VectDouble &lpcs);
  void convert(OGI_VectFloat &lsfs, OGI_VectFloat &lpcs);

  // JW: added these to accept EST_FVector arguments
  void convert(EST_FVector &lsfs, EST_FVector &lpcs);

};
/////////////////////////////////////////////////////////



class OGI_LPC_bwexp{
 private:
  int order;
  double bwexp;
  double *BWvect;
 public:

  OGI_LPC_bwexp(int order_in, double bwexp_in){
    order = order_in;
    bwexp = bwexp_in;
    BWvect = walloc(double, order+1);
    BWvect[0] = 1.0;
    for (int k=1; k<=order; k++){
      BWvect[k] = BWvect[k-1]*bwexp;
    }
  }

  void expand(OGI_VectDouble &lpcs);
  void expand(OGI_VectFloat &lpcs);

  ~OGI_LPC_bwexp(){
    wfree(BWvect);
  }
};


///////////////////////////////////////////////////
//  LPC_TO_LSF conversion function object
//   - configure once by instantiating, then
//     call convert() to do efficient conversion
class OGI_LPC_to_LSF{
private:
  fcomplex *P_pqa;
  fcomplex *P_roots;
  fcomplex *P_pqroots;
  double *P_angs;
  int order;
public:
  OGI_LPC_to_LSF(int order_in){
    order = order_in;
    P_pqa     = (fcomplex *) walloc(fcomplex, order+2);
    P_roots   = (fcomplex *) walloc(fcomplex, order+1);
    P_pqroots = (fcomplex *) walloc(fcomplex, order+2);
    P_angs    = (double *) walloc(double, order/2);
  }

  ~OGI_LPC_to_LSF(){
    wfree(P_pqa);
    wfree(P_roots);
    wfree(P_pqroots);
  }

  int convert(OGI_VectDouble &lpcs, OGI_VectDouble &lsfs);
  int convert(OGI_VectFloat &lpcs, OGI_VectFloat &lsfs);

  // JW: added these to accept EST_FVector arguments
  int convert(EST_FVector &lpcs, EST_FVector &lsfs);

};
/////////////////////////////////////////////////////////

void LPC_to_LSF_convert(EST_Track &lpcs, EST_Track &lsfs);
void LSF_to_LPC_convert(EST_Track &lsfs, EST_Track &lpcs);



#endif
