/****************************<--CSLU-->************************************/
/*                                                                        */
/*             Center for Spoken Language Understanding                   */
/*               Division of Biomedical Computer Science                  */
/*               Oregon Health and Science University                     */
/*                         Portland, OR USA                               */
/*                        Copyright (c) 1999 - 2009                       */
/*                                                                        */
/*      This module is not part of the CSTR/University of Edinburgh       */
/*               release of the Festival TTS system.                      */
/*                                                                        */
/*  In addition to any conditions disclaimers below, please see the file  */
/*  "license_ohsu_tts.txt" distributed with this software for information */
/*  on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  */
/*                                                                        */
/*      Authors: Mike Macon, Esther Klabbers, Alexander Kain              */
/*                          and David Brown                               */
/*                                                                        */
/****************************<--CSLU-->************************************/

/*-----------------------------------------------------------------------*/
/* Gets synthesis parameters from the lisp structs specified in Scheme   */
/*=======================================================================*/
#include <stdlib.h>
#include "festival.h"
#include "db_alloc.h"
#include "fl_io.h"
//DJLB changes made to default names, values. Now moved to fl_globals.h (15-05-06)
#include "fl_globals.h"

// STATIC struct containing info from Scheme layer
static EST_Features *In=NULL;

void Flinger_init_params(LISP params){

  // Alloc new struct (free first if already exists)
  //  and assign to static In
  create_Flinger_input();

  // Get params from Scheme layer
  In->set("volume_scalebase",  get_param_float("volume_scalebase",params,DEF_VOLUME_SCALEBASE));//DJLB addition
  In->set("vibrato_frequency",  get_param_float("vibrato_frequency",params,DEF_VIBRATO_FREQ));
  In->set("pitch_effects_max", get_param_float("pitch_effects_max", params,DEF_PITCH_EFFECTS_MAX));
  In->set("portamento_time", get_param_float("portamento_time", params,(float)DEF_PORTAMENTO_TIME));
  In->set("drift_frequency_1", get_param_float("drift_frequency_1",params,(float)DEF_DRIFT_FREQ1));
  In->set("drift_frequency_2", get_param_float("drift_frequency_2",params,DEF_DRIFT_FREQ2));
  In->set("drift_frequency_3", get_param_float("drift_frequency_3",params,DEF_DRIFT_FREQ3));
  In->set("drift_amplitude", get_param_float("drift_amplitude", params,(float)DEF_DRIFT_AMPL));
  In->set("transpose", get_param_float("transpose", params,DEF_TRANSPOSE));
  In->set("consonant_stretch", get_param_float("consonant_stretch", params,DEF_CONSONANT_STRETCH));
  In->set("min_rest_dur", get_param_float("min_rest_dur", params,(float)DEF_MIN_REST));
  In->set("phone_delimiter", wstrdup(get_param_str("phone_delimiter",params, "/")));
  In->set("syl_continuation", wstrdup(get_param_str("syl_continuation",params, "-")));
  In->set("voice_track_name", wstrdup(get_param_str("voice_track_name",params, DEF_TRACKNAME)));//DJLB 10-08-08
  In->set("voice_track_number", get_param_int("voice_track_number",params, DEF_TRACKNUMBER));//DJLB 10-08-08
  //DJLB 19-06-08. Channel selection modification.
  In->set("channel_number", get_param_int("channel_number", params,DEF_CHANNELNUMBER));
  In->set("common_events_track_name", wstrdup(get_param_str("common_events_track_name",params, "")));//DJLB common lyrics mod.
  In->set("common_events_track_number", get_param_int("common_events_track_number",params, DEF_COMMONEVENTSTRACKNUMBER));//DJLB common lyrics mod.
  //DJLB 21-06-08. Just Tuning mod.
  In->set("use_just_tuning", wstrdup(get_param_str("use_just_tuning",params, "no")));//DJLB 11-11-06.
}


void Flinger_addto_params(LISP params){
  if (In == NULL)
    create_Flinger_input();

  //LISP l,plisp;
  LISP l;
  EST_String thing;
  for (l=params; l !=0; l=cdr(l)){
    thing = get_c_string(car(car(l)));

    // check to see if it's new
    if (!(In->present(thing))){
      In->set(thing, get_c_float(car(cdr(car((l))))));
      cout << "new input param \"" << thing << "\" added\n";
    }
    else { // else  must be float type
      In->set(thing, get_c_float(car(cdr(car((l))))));
    }
  }
}

EST_Features *get_Flinger_params(void){
  if (In == NULL)
    create_Flinger_input();

  return In;
}

void create_Flinger_input(void){

  EST_Features *new_in;

  new_in = new EST_Features;

  if (In != NULL){
    delete In;
  }

  // Assign to static
  In = new_in;
}

// check to see if Flinger_init_params() has been called
bool Flinger_params_loaded(void){
  if (In == NULL)
    return(FALSE);
  else
    return(TRUE);
}

// YK 2011-07-25 API to manage a debug file switch
static int flinger_debug_file_flag;

void Flinger_set_debug_file_flag(const int val) {
	flinger_debug_file_flag = val;
}

int Flinger_get_debug_file_flag(void) {
	return flinger_debug_file_flag;
}
