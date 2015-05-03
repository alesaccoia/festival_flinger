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

#include "OGIdbase.h"

//#ifdef INCLUDE_OGISINE
//#include "OGIsine_db.h"   // sinusoidal
//#endif

                                                    
// return a LISP representation of info about a unit
LISP OGIdbase::lispify_unit_info(int i){
  LISP segs = NIL;
  int n;

  // bounds_check_unit(i, unitdic.Nunits());

  // Segs: Each member is of the form expected by utterance.cc
  //  to create new items with item.append_daughter:
  //     ("uu" ((featname1 val1) (featname2 val2) ... ))
  for (n=0; n<unitdic.Nseg(i); n++){
    segs = append(segs,
           cons(
	   cons(strintern(unitdic.seg_name(i,n)),
	   cons(append(
	     cons(make_param_float("dur",unitdic.seg_dur(i,n)/Fs),NIL),
                 //  add new feats here
	     NIL),NIL)),NIL));
    // ** to add new features -- see NOTES (2) at bottom
  }

  return
    cons(make_param_str("name",unitdic.name(i)),
    cons(make_param_int("db_indx",i),
    cons(make_param_int("Lindx",unitdic.Lindx(i)),
    cons(make_param_int("Rindx",unitdic.Rindx(i)),
    cons(make_param_int("Lbnd",unitdic.Lbnd(i)),
    cons(make_param_int("Rbnd",unitdic.Rbnd(i)),
    cons(make_param_lisp("segs",segs),
    NIL)))))));
}
                               
// build a hush table for units
void OGIdbase::build_hash(void){
  for(int i=0; i<unitdic.Nunits(); i++) {
    unamehash.add_item(unitdic.name(i),i);
  }
}                              

// use a hash table for fast lookup of unit indices
// return first indx with name name, or -1 if can't find it
int OGIdbase::name_lookup_hash(const EST_String &unitname)
{
  int found=0;
  int r;

  r = unamehash.val(unitname, found);
  if (found)
    return r;
  else
    return -1;
}

// old, slower(?) way
int OGIdbase::name_lookup(const EST_String &unitname)
{
  for(int i=0; i<unitdic.Nunits(); i++)
  {
    if(unitdic.name(i) == unitname)
    {
      return i;
    }
  }
  return -1;
}

// get access mode from LISP parameters
void OGIdbase::get_access_mode(LISP params){
  EST_String sval = wstrdup(get_param_str("access_mode",params,"unset"));
  if (sval == "direct")           access_mode = a_direct;
  else if (sval == "dynamic")     access_mode = a_dynamic;
  else if (sval == "ondemand")    access_mode = a_ondemand;
  else {
    EST_error("OGIdbase.cc: Unknown access mode: \"" + sval + "\" -- using direct access.\n");
  }
}


// utility functions
FILE * OGIdbase::open_groupfile_read(EST_String &fname){
  //      - open file,
  //      - get magic number
  //      - return fd
  FILE *gfd;

  if ((gfd=fopen(fname,"rb")) == NULL){
    EST_error("OGIdbase.cc: cannot open group file " + fname + " for reading\n");
  }
  return gfd;
}

FILE * OGIdbase::open_groupfile_write(EST_String &fname){
  //      - open file,
  //      - get magic number
  //      - return fd
  FILE *gfd;

  if ((gfd=fopen(fname,"wb")) == NULL){
    EST_error("OGIdbase.cc: cannot open group file " + fname + " for writing\n");
  }

  return gfd;
}

void OGIdbase::load_grp_params(void){
	OGIfread(&isCompressed, sizeof(OGIbool), 1, gfd, swap ? true : false);
  OGIfread(&Fs, sizeof(float), 1, gfd, swap ? true : false);
  OGIfread(dbglobal, gfd);

  int x;
  OGIfread(&x, sizeof(int), 1, gfd, swap ? true : false);
  if (x != udata_type ) {
    EST_error("wrong data type in the grouped file\n");
  }
}

void OGIdbase::save_grp_params(void){
  fwrite(&isCompressed, sizeof(OGIbool), 1, gfd);
  fwrite(&Fs, sizeof(float), 1, gfd);
  OGIfwrite(dbglobal, gfd);
  int x;
  x = (int) udata_type;
  fwrite(&x, sizeof(int), 1, gfd);
}

OGIbool OGIdbase::check_groupfile_swap(FILE *gfd){
  unsigned int magic;

  fread(&magic,sizeof(int), 1, gfd);
  if (magic == SWAPINT(OGI_DBASE_MAGIC))
    return TRUE;
  else if (magic == OGI_DBASE_MAGIC)
    return FALSE;
  else {
    EST_error("OGIdbase.cc:   not a valid OGI format group file\n");
  }
  return FALSE;
}

//static void bounds_check_unit(int i, int N){
//  if (i >= N){
//    cerr << "OGIdbase ERROR: request for unit " << i << " in " << N << " unit dbase.\n";
//    EST_error("exiting\n");
//  }
//}



#if 0
/*
NOTES:
 
(1) for lispify unit info:  
 
    // for future reference: example returning 3 features in segs
    segs = append(segs, 
           cons(
	   cons(strintern(unitdic.seg_name(i,n)),
	   cons(append(
	     cons(make_param_float("dur",unitdic.seg_dur(i,n)/Fs),NIL),
	     append(
	     cons(make_param_float("xxx",unitdic.seg_dur(i,n)/Fs),NIL),
	     cons(make_param_float("yyy",unitdic.seg_dur(i,n)/Fs),NIL)
	     )),NIL)),NIL));  
 
*/
#endif


