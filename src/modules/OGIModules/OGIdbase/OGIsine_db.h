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
#ifndef __OGISINE_DB_H_
#define __OGISINE_DB_H_

#include "EST.h"
#include "OGI_SineTrack.h"

static void OGIsine_db_err(EST_String str,int n);

class OGIsine_db {
  // 
private:
  EST_TVector<OGI_SineTrack *> p_sine;
  EST_IVector p_sineB; // begin and end for ungrouped mode
  EST_IVector p_sineE; 
  EST_TVector<OGIbool> p_sine_isLoaded; // flags for dynamic loading
  EST_TVector<long> p_sine_offset;      // byte offsets into groupfile
  
public:
  
  OGIsine_db();
  ~OGIsine_db();

  // read access operators
  INLINE OGI_SineTrack *sine(int n) { 
    if (!(sine_isLoaded(n))) OGIsine_db_err("sine",n);
    return p_sine.a(n);
  }
  INLINE int sineB(int n) { 
    if (!(sine_isLoaded(n))) OGIsine_db_err("sineB",n);
    return p_sineB.a(n);
  }
  INLINE int sineE(int n) { 
    if (!(sine_isLoaded(n))) OGIsine_db_err("sineE",n);
    return p_sineE.a(n);
  }

  INLINE long sine_offset(int n) { 
    return p_sine_offset.a(n);
  }
   
  INLINE OGIbool isLoaded(int n) { 
    if (n < length()){
      if (p_sine_isLoaded.a(n))
	return TRUE;
    }
    return FALSE;
  }
  INLINE OGIbool sine_isLoaded(int n) { 
    return p_sine_isLoaded.a(n);
  }

  // resize all vectors
  void resize(int N);

  // cut unused portions of data (ungrouped mode)
  void trim_unit(int i, float Fs);
 
  // number of units in db
  int length(void){
    return p_sine.length();
  }
  
  // retrieve one unit at synthesis runtime
  void get_dbUnit( OGIdbase &db, int db_indx, OGI_SineTrack **sine, int *sine_len);

  // loading one unit
  void load_unit_udata(int indx,  OGIdbase &db);
  void load_grp_sine(int n, OGIdbase &db); 
  void load_ungrp_sine(int n, OGIdbase &db); 

  // load/save groupfile data
  void load_grp_offsets(OGIdbase &db); 
  void save_grp_udata(OGIdbase &db);
  void load_grp_udata(OGIdbase &db);

  // loads ungrouped files
  void load_ungrp_udata(OGIdbase &db);


  INLINE void free_sine(int n) { 
    if (sine_isLoaded(n)){
      delete p_sine.a(n);
      p_sine_isLoaded[n] = FALSE;
    }
    else                  
      OGIsine_db_err("free_sine",n);
  }
  INLINE void free_unit(int n) { 
    free_sine(n);
  }

  //  void update_cache(int n);
  
};

static void OGIsine_db_err(EST_String str,int n){
  cerr << "OGIsine_db: request for unloaded " 
       << str << " unit - db_indx=" << n << endl;
  EST_error("---");
}

#endif
