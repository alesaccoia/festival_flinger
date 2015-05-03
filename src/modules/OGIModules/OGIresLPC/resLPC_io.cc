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
#include "resLPC.h"
#include "resLPC_io.h"
#include "OGI_file.h"


// STATIC struct containing info from Scheme layer
static EST_Features *In=NULL;

void OGIresLPC_init_params(LISP params){

  LISP plisp;

  // Alloc new struct (free first if already exists)
  //  and assign to static In
  create_resLPC_input();
 
  // Get params from Scheme layer
  In->set("F0_default",  get_param_float("F0_default",  params, 50.0));
  In->set("T0_UV_thresh",get_param_float("T0_UV_thresh",params, (float)0.020));
  In->set("T0_UV_pm",    get_param_float("T0_UV_pm",    params, (float)0.010));
  In->set("post_gain",   get_param_float("post_gain",   params, 1.0));
  In->set("deemphasis",  get_param_float("deemphasis",  params, (float)0.94));
  In->set("uv_gain",     get_param_float("uv_gain",     params, 1.0));
  In->set("uv_dither",   get_param_int("uv_dither",     params, 1));

  // triangle, trapezoid
  In->set("window_type", wstrdup(get_param_str("window_type",params, "triangle")));
  In->set("outbuf_incr",  get_param_int("outbuf_incr", params, 1024));

  // direct, none
  In->set("mod_method",  wstrdup(get_param_str("mod_method",params, "direct")));

  // direct, natural
  In->set("dur_method",  wstrdup(get_param_str("dur_method",params, "direct")));

  // direct, natural, soft
  In->set("pitch_method",  wstrdup(get_param_str("pitch_method",params, "direct")));
   
  // Voice conversion/quality mod
  In->set("vc_file", wstrdup(get_param_str("vc_file",params, "")));
 
  plisp = get_param_lisp("vqual_mod", params, NIL);
  if (plisp != NIL){
    In->set("vqual_mod",  siod_sprint(plisp));
  }

  // Smoothing
  plisp = get_param_lisp("spectra_smooth", params, NIL);
  if (plisp != NIL){
    In->set("spectra_smooth",  siod_sprint(plisp));
  }

  plisp = get_param_lisp("power_smooth", params, NIL);
  if (plisp != NIL){
    In->set("power_smooth", siod_sprint(plisp));
  }

  // "Y" or "N" - applies to power and spectra
  In->set("smooth_cross_ph_join", 
        (int) atobool(wstrdup(get_param_str("smooth_cross_ph_join",params, "N"))));
   
  In->set("spectra_match_or_replace", 
        wstrdup(get_param_str("spectra_match_or_replace",params, "match")));   
  In->set("power_match_or_replace", 
        wstrdup(get_param_str("power_match_or_replace",params, "match")));
   
  // list of dump info for debugging outputs
  plisp = get_param_lisp("dump", params, NIL);
  if (plisp != NIL){
    In->set("dump", siod_sprint(plisp));
  }
}

// YK 2011-07-25 Let's give access to every param that's init'd by 
//               OGIresLPC_init_params. Then scripts can modify 
//               any of the params, not just some of them. This
//               proc does the list-type params first, then the
//               string-types, the int-types, and finally the
//               float-types.
//
void OGIresLPC_addto_params(LISP params){
  if (In == NULL)
    create_resLPC_input();
 
  LISP l,plisp;
  EST_String thing;
  for (l=params; l !=0; l=cdr(l)){
    thing = get_c_string(car(car(l)));
    if ((thing == "spectra_smooth") ||
	(thing == "power_smooth") ||
	(thing == "vqual_mod") ||
	(thing == "dump")){
      plisp = get_param_lisp(thing, params, NIL);
      In->set(thing, siod_sprint(plisp));
    }
    else if ((thing == "window_type") ||
	     (thing == "mod_method") ||
	     (thing == "dur_method") ||
	     (thing == "pitch_method") ||
	     (thing == "spectra_match_or_replace") ||
	     (thing == "power_match_or_replace") ||
	     (thing == "smooth_cross_ph_join") ||
	     (thing == "vc_file")){
      In->set(thing,  wstrdup(get_c_string(car(cdr(car((l)))))));
    }

	// YK 2011-07-25 Added int block to do the int-type params.
	//               The float block is sloppy. Whoever wrote it
	//               should process known names, then report
	//               unknowns as a warning that they'll be
	//               added but unreferenced.
	//
	else if ((thing == "uv_dither") ||
		(thing == "outbuf_incr")) {
      In->set(thing, get_c_int(car(cdr(car((l))))));
	}

    // check to see if it's new
    else if (!(In->present(thing))){ 
      In->set(thing, get_c_float(car(cdr(car((l))))));
      cout << "new input param \"" << thing << "\" added\n";
    }
    else { // else  must be float type
      In->set(thing, get_c_float(car(cdr(car((l))))));
    }
  }
}

EST_Features *get_resLPC_params(void){
  if (In == NULL)
    create_resLPC_input();

  return In;
}

void create_resLPC_input(void){
 
  EST_Features *new_in;

  new_in = new EST_Features;
  
  if (In != NULL){
    delete In;
  }
  
  // Assign to static   
  In = new_in;
}

// check to see if OGIresLPC_init_params() has been called
bool resLPC_params_loaded(void){
  if (In == NULL)
    return(FALSE);
  else
    return(TRUE);
}
