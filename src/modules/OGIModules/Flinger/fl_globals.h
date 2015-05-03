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

#ifndef __FL_GLOBALS_H__
#define __FL_GLOBALS_H__

//These moved here from two other files fl_globals.cpp and fl_io.cpp
#define DEF_VOLUME_SCALEBASE 10	
#define DEF_VIBRATO_FREQ 5.5
#define DEF_PITCH_EFFECTS_MAX 1.0
#define DEF_PORTAMENTO_TIME 0.01
#define DEF_DRIFT_FREQ1 0.1
#define DEF_DRIFT_FREQ2 0.5
#define DEF_DRIFT_FREQ3 2.5
#define DEF_TRANSPOSE 0.0
#define DEF_DRIFT_AMPL 0.02
#define DEF_CONSONANT_STRETCH 1.0
#define DEF_MIN_REST 0.020
#define DEF_TRACKNAME "" //DJLB 13-05-06
#define DEF_TRACKNUMBER -1 //DJLB 10-08-08
#define DEF_COMMONEVENTSTRACKNAME "" //DJLB common lyrics mod.
#define DEF_COMMONEVENTSTRACKNUMBER -1 //DJLB common lyrics mod.
//DJLB 19-06-08. Channel selection modification.
#define DEF_CHANNELNUMBER -1
// DJLB 21-06-08. Just Tuning mod.
#define DEF_USEJUSTTUNING "no"

typedef struct {
  	int volume_scalebase;	// DJLB addition.
  float vibrato_frequency;
  float pitch_effects_max;
  float portamento_time;

  float drift_frequency_1;
  float drift_frequency_2;
  float drift_frequency_3;
  float drift_amplitude;

  float transpose; // in semitones
  float consonant_stretch;
  EST_String esReqdTrackName;   //DJLB 10-08-08
  int inReqdTrackNumber;        //DJLB 10-08-08
  EST_String esCommonEventsTrackName; //DJLB common lyrics mod.
  int inCommonEventsTrackNumber;    //DJLB common lyrics mod.
  //DJLB 19-06-08. Channel selection modification.
  int inReqdChannelNumber;
  //DJLB 21-06-08. Just Tuning mod.
  EST_String esUseJustTuning;		
} FL_GLOBAL; 

void FL_get_globals_from_lisp(LISP params);
void create_FL_GLOBAL_struct(void);
FL_GLOBAL *get_global_info(void);
int FL_GLOBAL_struct_exists(void);


#endif
