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

#ifndef EVENTUTIL_H
#define EVENTUTIL_H

#include "Event.h"

//DJLB addition/modification
#define GOOD 1
#define FATAL 0
#define EVENT_READ_FAULTY 2
extern int finReadEventFromSMFTrack(Event **pEvent, 
								SMFTrack &track, unsigned long &last_t,
									int use_time, const char *&errstr);
//End DJLB addition/modification
extern int WriteEventToSMFTrack(SMFTrack &track, unsigned long &last_t,
    const Event *event, int use_time, const char *&errstr);
#endif
