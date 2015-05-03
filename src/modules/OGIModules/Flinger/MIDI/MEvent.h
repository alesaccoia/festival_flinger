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

#ifndef METAEVENT_H
#define METAEVENT_H

#include "Event.h"

class MetaEvent : public Event {
	friend ostream &operator<<(ostream &os, const MetaEvent &e);
public:
	MetaEvent();
	MetaEvent(unsigned long t);
	MetaEvent(const MetaEvent &e);
	virtual Event *Dup(void) const {return (new MetaEvent(*this));}

	virtual EventType GetType(void) const {return (META);}
	virtual char *GetTypeStr(void) const {return ("MetaEvent");}
	virtual string GetEventStr(void) const;

	MetaEvent &operator=(const MetaEvent &e);

	virtual const char *SMFRead(SMFTrack &t) {
		// shut up a warning
		SMFTrack *dummy;
		dummy = &t;
		return (0);
	}
	virtual const char *SMFWrite(SMFTrack &t) const {
		// shut up a warning
		SMFTrack *dummy;
		dummy = &t;
		return (0);
	}
protected:
	virtual int Equal(const Event *e) const {return (Event::Equal(e));}
};
#endif
