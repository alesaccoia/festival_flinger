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

#include "Program.h"
#include <sstream>
using namespace std;


const int ProgramEvent::WC_VALUE = -1;
const unsigned long ProgramEvent::wc_value = (1 << 2);

ProgramEvent::ProgramEvent() : value(0)
{
}

ProgramEvent::ProgramEvent(unsigned long t, int chan, int val) :
    NormalEvent(t, chan), value(val)
{

	if (val == WC_VALUE)
		SetWildcard(wc_value);
}

ProgramEvent::ProgramEvent(const ProgramEvent &e) : NormalEvent(e),
    value(e.value)
{
}

ProgramEvent &
ProgramEvent::operator=(const ProgramEvent &e)
{

	(NormalEvent)*this = (NormalEvent)e;
	value = e.value;
	return (*this);
}

string
ProgramEvent::GetEventStr(void) const
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
	return (buf.str());
}

const char *
ProgramEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete ProgramEvent");
	value = *ptr;
	return (0);
}

const char *
ProgramEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutByte(value))
		return ("Out of memory");
	return (0);
}

int
ProgramEvent::Equal(const Event *e) const
{
	ProgramEvent *eptr = (ProgramEvent *)e;

	if (!NormalEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_value) && !GetWildcard(wc_value) &&
	    value != eptr->value)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const ProgramEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
