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

#include "Param.h"
#include <sstream>
using namespace std;


const int ParameterEvent::WC_PARAMETER = -1;
const int ParameterEvent::WC_VALUE = -1;
const unsigned long ParameterEvent::wc_parameter = (1 << 2);
const unsigned long ParameterEvent::wc_value = (1 << 3);

ParameterEvent::ParameterEvent() : parameter(0), value(0)
{
}

ParameterEvent::ParameterEvent(unsigned long t, int chan, int param,
    int val) : NormalEvent(t, chan), parameter(param), value(val)
{

	if (param == WC_PARAMETER)
		SetWildcard(wc_parameter);
	if (val == WC_VALUE)
		SetWildcard(wc_value);
}

ParameterEvent::ParameterEvent(const ParameterEvent &e) : NormalEvent(e),
    parameter(e.parameter), value(e.value)
{
}

ParameterEvent &
ParameterEvent::operator=(const ParameterEvent &e)
{

	(NormalEvent)*this = (NormalEvent)e;
	parameter = e.parameter;
	value = e.value;
	return (*this);
}

string
ParameterEvent::GetEventStr(void) const
{
	ostringstream buf;
	string tbuf;

	tbuf = NormalEvent::GetEventStr();
	buf << tbuf << " Parameter: ";
	if (GetWildcard(wc_parameter))
		buf << "*";
	else
		buf << (int)parameter;
	buf << " Value: ";
	if (GetWildcard(wc_value))
		buf << "*";
	else
		buf << (int)value;
	buf << ends;
	//delete tbuf;
	return (buf.str());
}

const char *
ParameterEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete ParameterEvent - missing parameter");
	parameter = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete ParameterEvent - missing value");
	value = *ptr;
	return (0);
}

const char *
ParameterEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutByte(parameter))
		return ("Out of memory");
	if (!t.PutByte(value))
		return ("Out of memory");
	return (0);
}

int
ParameterEvent::Equal(const Event *e) const
{
	ParameterEvent *eptr = (ParameterEvent *)e;

	if (!NormalEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_parameter) && !GetWildcard(wc_parameter)
	    && parameter != eptr->parameter)
		return (0);
	if (!eptr->GetWildcard(wc_value) && !GetWildcard(wc_value) &&
	    value != eptr->value)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const ParameterEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
