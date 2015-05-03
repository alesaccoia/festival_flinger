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
#include "resLPC_vc.h"
#include "EST.h"

// Alexander Kain, 1998

//
// Voice Conversion Init
//
void vc_init(char filename[]) {
  if(!G.load(filename))
    EST_error("could not load voice conversion mapping file " + EST_String(filename));
}


//
// Voice Conversion Procedure
//
void vc_do(EST_Track &intrack, EST_Track &V_UV) {
  int n = intrack.num_channels();
  int N = intrack.num_frames();

  double K = 8000.0/PI;

  Matrix in(n,N), out(n,N);
  double temp;
  int x,y;

  //  bool CHECK_UV = (V_UV.num_frames() > 0);
  bool CHECK_UV = FALSE;
  
  //Bark
  for(x=0;x<n;x++) {
    for(y=0;y<N;y++) {
      temp=intrack.a(y,x)*K;
      temp/=1200.0;
      in(x,y)=6*log(temp+sqrt(temp*temp+1));
    }
  }
  
  //Map
  G.mapping(in,out);
  
  //Unbark
  for(y=0;y<N;y++) {
    if (CHECK_UV && (V_UV.a_no_check(y) < 0.5)){
      ;
    }
    else {
      for(x=0;x<n;x++) {
	temp=exp(out(x,y)/6);
	temp=600*temp-600/temp;
	intrack.a(y,x) = (float)(temp/K);
      }
    }
  }
}




