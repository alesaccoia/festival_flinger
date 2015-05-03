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

#include "KeyPres.h"
#include <sstream>
using namespace std;

const int KeyPressureEvent::WC_PITCH = -1;
const int KeyPressureEvent::WC_PRESSURE = -1;
const unsigned long KeyPressureEvent::wc_pitch = (1 << 2);
const unsigned long KeyPressureEvent::wc_pressure = (1 << 3);

KeyPressureEvent::KeyPressureEvent() : pitch(0), pressure(0)
{
}

KeyPressureEvent::KeyPressureEvent(unsigned long t, int chan, int pit,
    int pres) : NormalEvent(t, chan), pitch(pit), pressure(pres)
{

	if (pit == WC_PITCH)
		SetWildcard(wc_pitch);
	if (pres == WC_PRESSURE)
		SetWildcard(wc_pressure);
}

KeyPressureEvent::KeyPressureEvent(const KeyPressureEvent &e) : NormalEvent(e),
    pitch(e.pitch), pressure(e.pressure)
{
}

KeyPressureEvent &
KeyPressureEvent::operator=(const KeyPressureEvent &e)
{

	(NormalEvent)*this = (NormalEvent)e;
	pitch = e.pitch;
	pressure = e.pressure;
	return (*this);
}

string
KeyPressureEvent::GetEventStr(void) const
{
	ostringstream buf;
	string tbuf;

	tbuf = NormalEvent::GetEventStr();
	buf << tbuf << " Pitch: ";
	if (GetWildcard(wc_pitch))
		buf << "*";
	else
		buf << (int)pitch;
	buf << " Pressure: ";
	if (GetWildcard(wc_pressure))
		buf << "*";
	else
		buf << (int)pressure;
	buf << ends;
	//delete tbuf;
	return (buf.str());
}

const char *
KeyPressureEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete KeyPressureEvent - missing pitch");
	pitch = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete KeyPressureEvent - missing pressure");
	pressure = *ptr;
	return (0);
}

const char *
KeyPressureEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutByte(pitch))
		return ("Out of memory");
	if (!t.PutByte(pressure))
		return ("Out of memory");
	return (0);
}

int
KeyPressureEvent::Equal(const Event *e) const
{
	KeyPressureEvent *eptr = (KeyPressureEvent *)e;

	if (!NormalEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_pitch) && !GetWildcard(wc_pitch) &&
	    pitch != eptr->pitch)
		return (0);
	if (!eptr->GetWildcard(wc_pressure) && !GetWildcard(wc_pressure)
	    && pressure != eptr->pressure)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const KeyPressureEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
