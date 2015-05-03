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

#ifndef METAEOT_H
#define METAEOT_H

#include "MEvent.h"

class MetaEndOfTrackEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaEndOfTrackEvent &e);
public:
	MetaEndOfTrackEvent();
	MetaEndOfTrackEvent(unsigned long t);
	MetaEndOfTrackEvent(const MetaEndOfTrackEvent &e);
	virtual Event *Dup(void) const
	    {return (new MetaEndOfTrackEvent(*this));}

	virtual EventType GetType(void) const {return (METAENDOFTRACK);}
	virtual char *GetTypeStr(void) const {return ("MetaEndOfTrackEvent");}
	virtual string GetEventStr(void) const;

	MetaEndOfTrackEvent &operator=(const MetaEndOfTrackEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;
protected:
	virtual int Equal(const Event *e) const {return (MetaEvent::Equal(e));}
};
#endif
