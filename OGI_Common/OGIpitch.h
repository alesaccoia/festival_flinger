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
#ifndef __OGIPITCH_H__
#define __OGIPITCH_H__

#include "EST.h"
#include "OGI_TimeWarp.h"
#include "OGI_SingleTrack.h"

void get_target_F0( EST_Utterance *u, OGI_SingleTrack &F0targ, OGI_TimeWarp &warp, float F0_default );

#endif
