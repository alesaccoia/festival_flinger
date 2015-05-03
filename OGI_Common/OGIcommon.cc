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

#include "festival.h"
#include "OGI_file.h"
#include "OGIgain.h"
#include "OGIcommon.h"

static LISP OGIsplit_string(LISP lstr);
static LISP utt_relation_load_ogi(LISP utt, LISP lrelname, LISP lfilename);  
static LISP utt_relation_save_ogi(LISP utt, LISP rname, LISP fname);
static LISP OGIutt_gain_targ(LISP utt, LISP params);

// Add output wave into utterance 
void output_wave(EST_Utterance *u, EST_Wave *wav){
  u->create_relation("Wave");
  EST_Item *item = u->relation("Wave")->append();
  item->set_val("wave", est_val(wav));
}


static LISP OGIsplit_string(LISP lstr){
  EST_String str;
  
  EST_String tok;
  EST_TokenStream us;
  us.open_string(get_c_string(lstr));
  LISP outlist = NIL;

  while (!us.eof()){
    tok = us.get().string();
    outlist = append(outlist, cons(strintern(tok),NIL));
  }

  return outlist;
}

static LISP utt_relation_load_ogi(LISP utt, LISP lrelname, LISP lfilename)
{
    EST_Utterance *u;
    if (utt == NIL)
        u = new EST_Utterance;
    else
        u = utterance(utt);
    EST_String filename = get_c_string(lfilename);
    EST_String relname = get_c_string(lrelname);
    EST_Relation *rel = u->create_relation(relname);
    
    if (rel->load(filename,"ogi") != 0){
      if (rel->load(filename,"esps") != 0){
        cerr << "utt.load.relation: loading from \"" << filename << 
	  "\" failed" << endl;
        festival_error();
      }
    }

    if (utt == NIL)
        return siod(u);
    else
        return utt;
}
 

static LISP OGIutt_gain_targ(LISP utt, LISP params){

  EST_Utterance *u;
  u = utterance(utt);

  float B,E,Bval,Eval,samp;
  EST_String method;

  // get info from scm
  B    = get_param_float("begin",     params, 0.0);
  E    = get_param_float("end",       params, 0.0);
  Bval = get_param_float("begin_val", params, 1.0);
  Eval = get_param_float("end_val",   params, 1.0);
  samp = get_param_float("samp",      params, (float)0.020);
  method = wstrdup(get_param_str("method", params, "sin"));

  OGIgain_targ(u,B,E,Bval,Eval,samp,method);

  return utt;
}

  
static LISP utt_relation_save_ogi(LISP utt, LISP rname, LISP fname){
  EST_Utterance *u = utterance(utt);
  EST_String relname = get_c_string(rname);
  EST_String filename = get_c_string(fname);
  EST_Item *i0;

  if (fname == NIL)
    filename = "save.phn";
  EST_Relation *r = u->relation(relname);
 
  EST_FVector y(r->length());
  EST_StrList names;

  // loop through items
  int n=0;
  for (i0=r->first(); i0 != 0; i0=i0->next()) {
    names.append(i0->name());
    y[n] = i0->F("end");
    n++;
  }

  // save it
  save_OGI_label(y, names, filename, OGI_ALL);

  return utt;
}
  

void festival_OGIcommon_init(void){ 

  proclaim_module("OGIcommon");
    
  init_subr_1("OGIsplit_string",OGIsplit_string,
   "(OGIsplit_string STRING)\n\
   Split whitespace-separated tokens in STRING into a list of \n\
   strings, like Perl 'split' function.");

  init_subr_2("OGIadd_gain_targets",OGIutt_gain_targ,
   "(OGIadd_gain_targets UTT PARAMS)\n\
   Add gain targets to UTT.  PARAMS is assoc list with members\n\
   begin, end, begin_val, end_val, method, samp");

  init_subr_3("utt.relation.load_ogi",utt_relation_load_ogi,
 "(utt.relation.load UTT RELATIONNAME FILENAME)\n\
  Loads (and creates) RELATIONNAME from FILENAME into UTT.  FILENAME\n\
  should contain simple OGI label format:  begin end name   on each line.\n\
  Puts 'begin' and 'end' features on all items.");



  init_subr_3("utt.save_ogi.relation",utt_relation_save_ogi,
 "(utt.save_ogi.relation UTT RELATIONNAME FILENAME)\n\
  Saves RELATIONNAME from UTT into FILENAME in \n\
  OGI label format:  begin end name   on each line.\n\
  warning:  Always uses 0.0 for 1st begin and always uses\n\
  previous 'end' for next 'begin -- DOES NOT LOOK AT 'begin' feature on items!");

}

