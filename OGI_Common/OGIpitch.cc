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

#include "OGIpitch.h"

// Functions for setting up duration modification.
// Assumes we have 
//     - "Target" relation with "pos" and "f0" features
//     - OGI_TimeWarp previously constructed
// 


//
// Get Fo targets from utterance
//
void get_target_F0(
		   EST_Utterance *u,
		   OGI_SingleTrack &F0targ,
		   OGI_TimeWarp &warp,
		   float F0_default
		   ){
  EST_Item *targ;
  const EST_Relation *targrel = u->relation("Target");

  // start efficient append
  F0targ.begin_append(1000);

  // Ensure there is a target at the start: 
  if (targrel->length() == 0){   // create a target if there are none
    F0targ.append(0.0, F0_default);
  }
  else {  // prepend a target if the time of the 1st target is not at time=0
    targ = targrel->first_leaf();    
    if  (!(targ->f_present("pos") && targ->f_present("f0"))){
      F0targ.prepend(0.0, F0_default);
    }
    else if (targ->F("pos") != 0){
      F0targ.prepend(0.0, targrel->first_leaf()->F("f0"));
    }
  }
  
  // loop through targets
  for (targ=targrel->first_leaf(); targ != 0; targ=next_leaf(targ)) {
    if (targ->f_present("pos") && targ->f_present("f0")){
      F0targ.append(targ->F("pos"), targ->F("f0"));
    }
  }
      
  // Ensure there is a target at the end 
  if (targrel->length() == 0){  // create a target if there are none
    F0targ.append(warp.last_targ_end(), F0_default);
  }
  else { // if last one not at end
    targ = targrel->last_leaf();
    if (!(targ->f_present("pos") && targ->f_present("f0"))){
      F0targ.append(warp.last_targ_end(), F0_default);
    }
    else if (targ->F("pos") < u->relation("Segment")->last()->F("end")){
      F0targ.append(warp.last_targ_end(), targ->F("f0"));
    }
  }

  // end append
  F0targ.end_append();

}

