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

#include "NoteOn.h"

const unsigned long NoteOnEvent::WC_DURATION = 0xffffffff;

const unsigned long NoteOnEvent::wc_duration = (1 << 4);

NoteOnEvent::NoteOnEvent()
{
}

NoteOnEvent::NoteOnEvent(unsigned long t, int chan, int pit, int vel,
    const NoteEvent *np) : NoteEvent(t, chan, pit, vel, np)
{

	if (np != 0 && np->GetTime() != WC_TIME && t != WC_TIME)
		duration = np->GetTime() - t;
}

NoteOnEvent::NoteOnEvent(unsigned long t, int chan, int pit, int vel,
    unsigned long dur, const NoteEvent *np) : NoteEvent(t, chan, pit, vel, np),
    duration(dur)
{

	if (dur == WC_DURATION)
		SetWildcard(wc_duration);
}

NoteOnEvent::NoteOnEvent(const NoteOnEvent &e) : NoteEvent(e)
{
}

NoteOnEvent &
NoteOnEvent::operator=(const NoteOnEvent &e)
{

	(NoteEvent)*this = (NoteEvent)e;
	return (*this);
}

string
NoteOnEvent::GetEventStr(void) const
{

	return (NoteEvent::GetEventStr());
}

int
NoteOnEvent::Equal(const Event *e) const
{
	NoteOnEvent *eptr = (NoteOnEvent *)e;

	if (!NoteEvent::Equal(e))
		return (0);
	if (GetNotePair() != 0 && eptr->GetNotePair() != 0) {
		if (!eptr->GetWildcard(wc_duration) &&
		    !GetWildcard(wc_duration) && duration != eptr->duration)
			return (0);
	}
	return (1);
}

ostream &
operator<<(ostream &os, const NoteOnEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
