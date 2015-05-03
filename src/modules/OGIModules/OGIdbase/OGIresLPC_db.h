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
#ifndef __OGIRESLPC_DB_H_
#define __OGIRESLPC_DB_H_

#include "OGIdbase.h"

static void OGIresLPC_db_err(EST_String str,int n);

class OGIresLPC_db : public OGIdbase{

public:

private:
  //EST_TVector<EST_Wave *> p_exc;
  EST_TVector<void *> p_exc;  // (EST_Wave *)
  EST_IVector p_excB; 
  EST_IVector p_excE; 
  EST_TVector<OGIbool> p_exc_isLoaded;
  EST_TVector<long> p_exc_offset; 
  
  //EST_TVector<EST_Track *> p_pm;
  EST_TVector<void *> p_pm;  // (EST_Track *)
  EST_TVector<OGIbool> p_pm_isLoaded;
  EST_TVector<long> p_pm_offset;

  //EST_TVector<EST_Track *> p_lpc;
  EST_TVector<void *> p_lpc; // (EST_Track *)
  EST_TVector<OGIbool> p_lpc_isLoaded;
  EST_TVector<long> p_lpc_offset; 

  EST_FVector p_gainL;
  EST_FVector p_gainR;

  EST_IList loaded_units;

  void parse_lisp_parameters(LISP params, OGIbool isGrp);
  // load ungrouped files
  void load_ungrp_udata();
  void load_ungrp_exc(int n); 
  void load_ungrp_pm(int n); 
  void load_ungrp_lpc(int n); 
  void load_ungrp_gains();
  // load groupfile data
  void load_grp_udata();
  void load_grp_offsets(); 
  void load_grp_exc(int n); 
  void load_grp_pm(int n); 
  void load_grp_lpc(int n); 
  
public:
  
  OGIresLPC_db();
  ~OGIresLPC_db();

  // read access operators
  INLINE EST_Wave *exc(int n) { 
    if (!(exc_isLoaded(n))) OGIresLPC_db_err("exc",n);
    return (EST_Wave *) p_exc.a(n);
  }
  INLINE int excB(int n) { 
    if (!(exc_isLoaded(n))) OGIresLPC_db_err("excB",n);
    return p_excB.a(n);
  }
  INLINE int excE(int n) { 
    if (!(exc_isLoaded(n))) OGIresLPC_db_err("excE",n);
    return p_excE.a(n);
  }
  INLINE EST_Track *pm(int n) { 
    if (!(pm_isLoaded(n))) OGIresLPC_db_err("pm",n);
    return (EST_Track *) p_pm.a(n);
  }
  INLINE EST_Track *lpc(int n) { 
    if (!(lpc_isLoaded(n))) OGIresLPC_db_err("lpc",n);
    return (EST_Track *) p_lpc.a(n);
  }

  INLINE long exc_offset(int n) { 
    return p_exc_offset.a(n);
  }
  INLINE long pm_offset(int n) { 
    return p_pm_offset.a(n);
  }
  INLINE long lpc_offset(int n) { 
    return p_lpc_offset.a(n);
  }

   
  INLINE OGIbool isLoaded(int n) { 
    if (n < length()){
      if (p_exc_isLoaded.a(n) && p_pm_isLoaded.a(n) && p_lpc_isLoaded.a(n))
	return TRUE;
    }
    return FALSE;
  }
  INLINE OGIbool exc_isLoaded(int n) { 
    return p_exc_isLoaded.a(n);
  }
  INLINE OGIbool pm_isLoaded(int n) { 
    return p_pm_isLoaded.a(n);
  }
  INLINE OGIbool lpc_isLoaded(int n) { 
    return p_lpc_isLoaded.a(n);
  }


  // resize all vectors
  void resize(int N);

  // cut unused portions of data
  void trim_unit(int i, int extraL, int extraR, float Fs);
 
  // number of units in db
  int length(void){
    // assume all same
    return p_exc.length();
  }
  
  void free_all_units(void);

  // retrieve one unit at synthesis runtime
  void get_dbUnit(int db_indx, EST_Wave **exc, int *excB, int *excE, EST_Track **pm, EST_Track **lpc);

  // loading one unit
  int load_unit_udata(int indx); // returns size of wav
  void modify_gain(int n);

  // load/save groupfile data
  void save_grp_udata();


  INLINE void free_exc(int n) { 
    if (exc_isLoaded(n)){
      delete (EST_Wave *) p_exc.a(n);
      p_exc_isLoaded[n] = FALSE;
    }
    else                  
      OGIresLPC_db_err("free_exc",n);
  }
  INLINE void free_pm(int n) { 
    if (pm_isLoaded(n)){
      delete (EST_Track *) p_pm.a(n);
      p_pm_isLoaded[n] = FALSE;
    }
    else                  
      OGIresLPC_db_err("free_pm",n);
  }
  INLINE void free_lpc(int n) { 
    if (lpc_isLoaded(n)){
      delete (EST_Track *) p_lpc.a(n);
      p_lpc_isLoaded[n] = FALSE;
    }
    else                  
      OGIresLPC_db_err("free_lpc",n);
  }
  INLINE void free_unit(int n) { 
    free_exc(n);
    free_pm(n);
    free_lpc(n);
  }

  //  void update_cache(int n);

  // Implement functions of the base class
  LISP lispify_data_type(void);
  void load_grouped(EST_String &groupfile, LISP usr_params);
  void save_grouped(EST_String &groupfile);
  void load_ungrouped(LISP params);
  
  //virtual void load_grp_header(void);
  //virtual void save_grp_header(void);
  //virtual void load_grp_unitdic(void);
  //virtual void save_grp_unitdic(void);

  
};

static void OGIresLPC_db_err(EST_String str,int n){
  cerr << "OGIresLPC_db: request for unloaded " 
       << str << " unit - db_indx=" << n << endl;
  EST_error("---");
}

#endif
