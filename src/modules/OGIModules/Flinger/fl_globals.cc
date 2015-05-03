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
#include "fl_globals.h"
//DJLB changes made to default names, values.Now moved to fl_globals.h (15-05-06)

static void delete_input(void);

// STATIC struct containing config info from Scheme layer
static FL_GLOBAL *In=NULL;

void FL_get_globals_from_lisp(LISP params){

  // Alloc new struct (free first if already exists)
  //  and assign to static In
  create_FL_GLOBAL_struct();
 
  // Get params from Scheme layer
  In->volume_scalebase = (int)get_param_float("volume_scalebase",params,DEF_VOLUME_SCALEBASE);//DJLB addition
  In->vibrato_frequency = get_param_float("vibrato_frequency",params,DEF_VIBRATO_FREQ);
  In->pitch_effects_max  = get_param_float("pitch_effects_max", params,DEF_PITCH_EFFECTS_MAX);
  In->portamento_time  = get_param_float("portamento_time", params,(float)DEF_PORTAMENTO_TIME);
  In->drift_frequency_1 = get_param_float("drift_frequency_1",params,(float)DEF_DRIFT_FREQ1);
  In->drift_frequency_2 = get_param_float("drift_frequency_2",params,DEF_DRIFT_FREQ2);
  In->drift_frequency_3 = get_param_float("drift_frequency_3",params,DEF_DRIFT_FREQ3);
  In->drift_amplitude  = get_param_float("drift_amplitude", params,(float)DEF_DRIFT_AMPL); 
  In->transpose   = get_param_float("transpose", params,DEF_TRANSPOSE);
  In->consonant_stretch = get_param_float("consonant_stretch", params,DEF_CONSONANT_STRETCH);
  In->esReqdTrackName = get_param_str("voice_track_name", params, DEF_TRACKNAME);//DJLB 10-08-08
  In->inReqdTrackNumber = get_param_int("voice_track_number", params, DEF_TRACKNUMBER);//DJLB 10-08-08
  //DJLB 19-06-08. Channel selection modification.
  In->inReqdChannelNumber = get_param_int("channel_number", params,DEF_CHANNELNUMBER);
  In->esCommonEventsTrackName = get_param_str("common_events_track_name", params,DEF_COMMONEVENTSTRACKNAME);//DJLB common lyrics mod.
  In->inCommonEventsTrackNumber = get_param_int("common_events_track_number", params,DEF_COMMONEVENTSTRACKNUMBER);//DJLB common lyrics mod.
  //DJLB 21-06-08. Just Tuning mod.
  In->esUseJustTuning = get_param_str("use_just_tuning", params, DEF_USEJUSTTUNING); //DJLB 11-11-06
}

void create_FL_GLOBAL_struct(void){

  FL_GLOBAL *new_in;

  // Alloc a new struct
  new_in = dballoc(FL_GLOBAL, 1);
  memset(new_in,0,sizeof(FL_GLOBAL));  // zero out

  // Defaults only
  //  -- these are only used if FL_get_globals_from_lisp is never called
  //     (i.e., if scheme layer never calls Flinger_Init)
  new_in->volume_scalebase  = DEF_VOLUME_SCALEBASE;//DJLB addition
  new_in->vibrato_frequency  = DEF_VIBRATO_FREQ;
  new_in->pitch_effects_max = DEF_PITCH_EFFECTS_MAX;
  new_in->portamento_time = (float)DEF_PORTAMENTO_TIME;
  new_in->drift_frequency_1 = (float)DEF_DRIFT_FREQ1;
  new_in->drift_frequency_2 = DEF_DRIFT_FREQ2;
  new_in->drift_frequency_3 = DEF_DRIFT_FREQ3;
  new_in->drift_amplitude  = (float)DEF_DRIFT_AMPL;
  new_in->transpose   = DEF_TRANSPOSE;
  new_in->consonant_stretch = DEF_CONSONANT_STRETCH;
  new_in->esReqdTrackName = DEF_TRACKNAME; //DJLB 10-08-08
  new_in->inReqdTrackNumber = DEF_TRACKNUMBER; //DJLB 10-08-08
  new_in->esCommonEventsTrackName = DEF_COMMONEVENTSTRACKNAME; //DJLB common lyrics mod.
  new_in->inCommonEventsTrackNumber = DEF_COMMONEVENTSTRACKNUMBER; //DJLB common lyrics mod.

    //DJLB 19-06-08. Channel selection modification.
  new_in->inReqdChannelNumber = DEF_CHANNELNUMBER;
    //DJLB 21-06-08. Just Tuning mod.
  new_in->esUseJustTuning = DEF_USEJUSTTUNING;
  if (In != NULL){
    delete_input();
  }

  // Assign to static   
  In = new_in;
}

static void delete_input(void){
  dbfree(In);
  In = NULL;
}

// Get ptr to struct
FL_GLOBAL *get_global_info(void){
    return(In);
}

int FL_GLOBAL_struct_exists(void){
  if (In != NULL)
    return 1;
  else
    return 0;
}

