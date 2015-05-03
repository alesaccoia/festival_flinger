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

#include "NoteOff.h"

NoteOffEvent::NoteOffEvent()
{
}

NoteOffEvent::NoteOffEvent(unsigned long t, int chan, int pit, int vel,
    const NoteEvent *np) : NoteEvent(t, chan, pit, vel, np)
{
}

NoteOffEvent::NoteOffEvent(const NoteOffEvent &e) : NoteEvent(e)
{
}

NoteOffEvent &
NoteOffEvent::operator=(const NoteOffEvent &e)
{

	(NoteEvent)*this = (NoteEvent)e;
	return (*this);
}

string
NoteOffEvent::GetEventStr(void) const
{

	return (NoteEvent::GetEventStr());
}

ostream &
operator<<(ostream &os, const NoteOffEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
