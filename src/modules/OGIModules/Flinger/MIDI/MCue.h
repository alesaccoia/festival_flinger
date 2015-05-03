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

#ifndef METACUE_H
#define METACUE_H

#include "MText.h"

class MetaCueEvent : public MetaTextEvent {
	friend ostream &operator<<(ostream &os, const MetaCueEvent &e);
public:
	MetaCueEvent();
	MetaCueEvent(unsigned long t, const char *str);
	MetaCueEvent(const MetaCueEvent &e);
	virtual ~MetaCueEvent();
	virtual Event *Dup(void) const {return (new MetaCueEvent(*this));}

	virtual EventType GetType(void) const {return (METACUE);}
	virtual char *GetTypeStr(void) const {return ("MetaCueEvent");}
	virtual string GetEventStr(void) const;

	MetaCueEvent &operator=(const MetaCueEvent &e);

	virtual const char *SMFRead(SMFTrack &t)
	    {return (MetaTextEvent::SMFRead(t));}
	virtual const char *SMFWrite(SMFTrack &t) const
	    {return (MetaTextEvent::SMFWrite(t));}
protected:
	virtual int Equal(const Event *e) const
	    {return (MetaTextEvent::Equal(e));}
};
#endif
