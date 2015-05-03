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
#include <iostream>
#include <fstream>
#include "EST.h"
#include "OGI_Track.h"
#include "OGI_Buffer.h"
#include "OGI_Macro.h"
#include "resLPC_pmark.h"

// wrapper used in analysis routines
void fill_UV_pmarks(OGI_Track &pm, int Nsig, float Fs, EST_Features &g, bool label_vuv){
  EST_IList Bv,Ev;
  // Modified by Qi Miao, unsed variables *b *e
  //EST_Litem *b,*e;

  pmark_V_UV(pm, Bv, Ev, Nsig, Fs, g);
  make_UVpmarks(pm, Bv, Ev, Nsig, Fs, g, label_vuv);
}


//
// Make voicing decision based on pmarks
//   creates VOICED sections
//
void pmark_V_UV(
		       OGI_Track &pm,
		       EST_IList &Bvsect,
		       EST_IList &Evsect,
		       int Nexc,
		       float Fs,
		       EST_Features &gl){
  int k;
  int state = _UV_;
  float To;
  float T0_UV_thresh = gl.F("T0_UV_thresh");
  int Nfr = pm.num_frames();
  int beg_pm=0,end_pm=0;

  bool CK_JOIN = FALSE;
  if (pm.has_channel("v/uv"))
    CK_JOIN = TRUE;

  // Find voiced sections
  if (pm.length() > 1){
    for (k=0; k<Nfr; k++){
      if (k == Nfr-1)
	To = Nexc/Fs - pm.last_t();
      else
	To = pm.T0(k);
      
      if ((To < T0_UV_thresh) || // if now V
	  (CK_JOIN && ((int)(pm.a(k, "v/uv") + 0.5) == _V_))){  
	if (state == _UV_)
	  beg_pm = k;
	else
	  end_pm = k;
	state = _V_;
      }
      else{                 // if now UV
	if (state == _V_){
	  end_pm = k;
	  Bvsect.append(beg_pm);
	  Evsect.append(end_pm);
	  beg_pm = k;
	}
	state = _UV_;
      }
    }
  }

  if (state == _V_){    // last section
    end_pm = pm.length()-1;
    Bvsect.append(beg_pm);
    Evsect.append(end_pm);
  }  
}

void make_UVpmarks(
			  OGI_Track &pm,
			  EST_IList &Bvsect,
			  EST_IList &Evsect, 
			  int Nexc,
			  float Fs,
			  EST_Features &gl,
			  bool label_vuv
			  ){
  
  float To_UV_sec = gl.F("T0_UV_pm");
  EST_Litem *b,*e;

  if (Bvsect.length() == 0){  // if all-unvoiced utterance
    insert_UVsection(Bvsect, Evsect, -1, Bvsect.length(), 
		     To_UV_sec, pm, Nexc, Fs, label_vuv);
    return;
  }

  /// part from end of last voiced section thru end
  insert_UVsection(Bvsect, Evsect, Evsect(Evsect.tail()), pm.num_frames(),
		   To_UV_sec, pm, Nexc, Fs, label_vuv);
  
  for (b=Bvsect.tail(),e=Evsect.tail(); 
                  ((b!=0)||(e!=0)); b=b->prev(),e=e->prev()){

    make_Vsection(Bvsect(b), Evsect(e), pm, label_vuv);

    // make next UNVOICED section
    if (e->prev()){
      insert_UVsection(Bvsect, Evsect, Evsect(e->prev()), Bvsect(b),
		       To_UV_sec, pm, Nexc, Fs, label_vuv);
    }
  }


  // before 1st voiced section
  insert_UVsection(Bvsect, Evsect, -1, Bvsect.first(), 
		   To_UV_sec, pm, Nexc, Fs, label_vuv);

}


void insert_UVsection (
			      EST_IList &Bvsect,
			      EST_IList &Evsect,
			      int UVsectB,
			      int UVsectE,
			      float incr,
			      OGI_Track &pm,
			      int Nexc,
			      float Fs,
			      bool label_vuv
			      ){  

  OGI_BuffFVect tvect(300); // construct w/ buffer size
  float t, begin_sec, end_sec, last_end;
  int k,n;
  EST_Litem *b,*e;

  int Nstrays = UVsectE - UVsectB - 1;
  float pad = incr/(float)2.5;

  if (UVsectB > 0)
    begin_sec = pm.t(UVsectB) + incr;
  else  
    begin_sec = 0.0;

  if (UVsectE < pm.num_frames())
    last_end = pm.t(UVsectE) - pad;
  else        // go to end of signal
    last_end = Nexc/Fs;

  // start time
  t = begin_sec;

  for (k=UVsectB+1; k<UVsectE; k++){
    end_sec = pm.t(k);

    // add new ones
    while (t < end_sec - pad){
      tvect.append(t);
      t += incr;
    }

    // add existing stray UV pm
    tvect.append(end_sec);
    t = end_sec + incr;
  }
  // the rest up until voiced section
  while (t < last_end){ 
    tvect.append(t);
    t += incr;
  }

  // insert into track
  pm.insert_t(tvect);

  // update V/UV information to reflect added pmarks
  for (b=Bvsect.head(),e=Evsect.head(); 
                  ((b!=0)||(e!=0)); b=b->next(),e=e->next()){

    if (Bvsect(b) >= UVsectE){      
      Bvsect(b) +=  tvect.Nelem()-Nstrays;
      Evsect(e) +=  tvect.Nelem()-Nstrays;
    }      
  }

  if (label_vuv){
    for (n=UVsectB+1; n < UVsectE+tvect.Nelem()-Nstrays; n++){
      // label as UNvoiced
      pm.a(n, "v/uv") = _UV_;
    }
  }
  
  return;
}



void make_Vsection ( 
			     int pmB,  // pmark indices
			     int pmE,  // pmark indices
			     OGI_Track &pm,
			     bool label
			     ){  
  if (label){
    int n;
    for (n=pmB; n<=pmE; n++){ 
      // label as voiced
      pm.a(n, "v/uv") = _V_;
    }
  }
}
