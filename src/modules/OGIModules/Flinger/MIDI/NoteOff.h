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


#ifndef NOTEOFFEVENT_H
#define NOTEOFFEVENT_H

#include "Note.h"

class NoteOffEvent : public NoteEvent {
	friend ostream &operator<<(ostream &os, const NoteOffEvent &e);
public:
	NoteOffEvent();
	NoteOffEvent(unsigned long t, int chan, int pit, int velocity = 0,
	    const NoteEvent *np = 0);
	NoteOffEvent(const NoteOffEvent &e);
	virtual Event *Dup(void) const {return (new NoteOffEvent(*this));}

	virtual EventType GetType(void) const {return (NOTEOFF);}
	virtual char *GetTypeStr(void) const {return ("NoteOffEvent");}
	virtual string GetEventStr(void) const;

	NoteOffEvent &operator=(const NoteOffEvent &e);

	virtual const char *SMFRead(SMFTrack &t)
	    {return (NoteEvent::SMFRead(t));}
	virtual const char *SMFWrite(SMFTrack &t) const
	    {return (NoteEvent::SMFWrite(t));}
protected:
	virtual int Equal(const Event *e) const {return (NoteEvent::Equal(e));}
};
#endif
