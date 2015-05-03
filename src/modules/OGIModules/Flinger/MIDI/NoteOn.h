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

#ifndef NOTEONEVENT_H
#define NOTEONEVENT_H

#include "Note.h"

class NoteOnEvent : public NoteEvent {
	friend ostream &operator<<(ostream &os, const NoteOnEvent &e);
public:
	NoteOnEvent();
	NoteOnEvent(unsigned long t, int chan, int pit, int velocity,
	    const NoteEvent *np = 0);
	NoteOnEvent(unsigned long t, int chan, int pit, int velocity,
	    unsigned long duration, const NoteEvent *np = 0);
	NoteOnEvent(const NoteOnEvent &e);
	virtual Event *Dup(void) const {return (new NoteOnEvent(*this));}

	virtual EventType GetType(void) const {return (NOTEON);}
	virtual char *GetTypeStr(void) const {return ("NoteOnEvent");}
	virtual string GetEventStr(void) const;

	unsigned long GetDuration(void) const {
		if (GetWildcard(wc_duration))
			return (WC_DURATION);
		else
			return (duration);
	}

	void SetDuration(unsigned long dur) {
		if (dur == WC_DURATION)
			SetWildcard(wc_duration);
		else {
			duration = dur;
			ClearWildcard(wc_duration);
		}
	}

	virtual void SetNotePair(NoteEvent *np) {
		NoteEvent::SetNotePair(np);
		if (GetTime() != WC_TIME && np->GetTime() != WC_TIME) {
			duration = np->GetTime() - GetTime();
			ClearWildcard(wc_duration);
		}
	}

	NoteOnEvent &operator=(const NoteOnEvent &e);

	virtual const char *SMFRead(SMFTrack &t)
	    {return (NoteEvent::SMFRead(t));}
	virtual const char *SMFWrite(SMFTrack &t) const
	    {return (NoteEvent::SMFWrite(t));}

	static const unsigned long WC_DURATION;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_duration;
	unsigned long duration;
};
#endif
