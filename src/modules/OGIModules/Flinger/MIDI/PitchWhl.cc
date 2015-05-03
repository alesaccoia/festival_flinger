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

#include "PitchWhl.h"
#include <sstream>
using namespace std;


const long PitchWheelEvent::WC_VALUE = 0x0fffffff;
const unsigned long PitchWheelEvent::wc_value = (1 << 2);

PitchWheelEvent::PitchWheelEvent() : value(0)
{
}

PitchWheelEvent::PitchWheelEvent(unsigned long t, int chan, long val) :
    NormalEvent(t, chan), value(val)
{

	if (val == WC_VALUE)
		SetWildcard(wc_value);
}

PitchWheelEvent::PitchWheelEvent(const PitchWheelEvent &e) : NormalEvent(e),
    value(e.value)
{
}

PitchWheelEvent &
PitchWheelEvent::operator=(const PitchWheelEvent &e)
{

	(NormalEvent)*this = (NormalEvent)e;
	value = e.value;
	return (*this);
}

string
PitchWheelEvent::GetEventStr(void) const
{
	ostringstream buf;
	string tbuf;

	tbuf = NormalEvent::GetEventStr();
	buf << tbuf << " Value: ";
	if (GetWildcard(wc_value))
		buf << "*";
	else
		buf << (int)value;
	buf << ends;
	//delete tbuf;
	return(buf.str());
}

const char *
PitchWheelEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete PitchWheelEvent");
	value = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete PitchWheelEvent");
	value |= (short)*ptr << 7;
	return (0);
}

const char *
PitchWheelEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutByte(value & 0x7f))
		return ("Out of memory");
	if (!t.PutByte((value >> 7) & 0x7f))
		return ("Out of memory");
	return (0);
}

int
PitchWheelEvent::Equal(const Event *e) const
{
	PitchWheelEvent *eptr = (PitchWheelEvent *)e;

	if (!NormalEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_value) && !GetWildcard(wc_value) &&
	    value != eptr->value)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const PitchWheelEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
