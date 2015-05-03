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
#ifndef __OGIDBASE_H__
#define __OGIDBASE_H__

#include "EST.h"
#include "OGIdbUnits.h"

#define OGI_DBASE_MAGIC 0xf149474e

enum OGIaccess_t {a_direct, a_dynamic, a_ondemand};
enum OGIdata_t {d_unset, d_resLPC, d_LPC, d_sine, d_sinLPC};


// make it abstract, niuxc, 7/2004
class OGIdbase {

protected:

  // data available to derived classes
  OGIbool swap;                 // byte order flag
  OGIbool isGrouped;            // grouped or ungrouped
  OGIbool isCompressed;         // compressed or not
  EST_Option dbglobal;          // container for misc signal processing constants
                                /* includes: lpc_order; preemph; def_f0; phoneset */

  // specific to grouped case
  FILE *gfd;                    // the handle to the group file

  // specific to ungrouped case
  EST_Option fileinfo;  // KVL of info...  might change to EST_Features
                        /* includes.... unitdic_file, signal_dir; signal_ext; signal_type; pitch_dir;
                           pitch_ext; lpc_dir; lpc_ext; lpc_res_dir; lpc_res_ext; */

  
  // utility functions available to derived classes
  FILE *open_groupfile_read(EST_String &fname);
  FILE *open_groupfile_write(EST_String &fname);
  void load_grp_params(void);
  void save_grp_params(void);
  OGIbool check_groupfile_swap(FILE *gfd);

public:

  EST_String name;              // name of the dbase
  float Fs;                     // sampling freq
  enum OGIaccess_t access_mode; // ondemand, dynamic, direct
  enum OGIdata_t udata_type;    // type of data base

  // index into this to get each unit in dbase
  OGIdbUnits unitdic;             // unit table
  EST_TStringHash<int> unamehash; // unit name hashtable for quick access


  // con/destructor
  OGIdbase():unamehash(5000) {};  
  virtual ~OGIdbase() {}; 

  // functions
  LISP lispify_unit_info(int indx);
  void build_hash(void);
  int name_lookup_hash(const EST_String &name);
  int name_lookup(const EST_String &name);
  void get_access_mode(LISP params);
  
  // pure virtual functions (must be defined in derived classes)
  virtual LISP lispify_data_type(void)=0;
  virtual void load_grouped(EST_String &groupfile, LISP usr_params)=0;
  virtual void save_grouped(EST_String &groupfile)=0;
  virtual void load_ungrouped(LISP params)=0;
  
};

#endif
