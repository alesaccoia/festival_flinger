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

#include "ChanPres.h"
#include <sstream>
using namespace std;


const int ChannelPressureEvent::WC_PRESSURE = -1;
const unsigned long ChannelPressureEvent::wc_pressure = (1 << 2);

ChannelPressureEvent::ChannelPressureEvent() : pressure(0)
{
}

ChannelPressureEvent::ChannelPressureEvent(unsigned long t, int chan,
    int pres) : NormalEvent(t, chan), pressure(pres)
{

	if (pres == WC_PRESSURE)
		SetWildcard(wc_pressure);
}

ChannelPressureEvent::ChannelPressureEvent(const ChannelPressureEvent &e) : NormalEvent(e),
    pressure(e.pressure)
{
}

ChannelPressureEvent &
ChannelPressureEvent::operator=(const ChannelPressureEvent &e)
{

	(NormalEvent)*this = (NormalEvent)e;
	pressure = e.pressure;
	return (*this);
}

string
ChannelPressureEvent::GetEventStr(void) const
{
	ostringstream buf;
	string tbuf;

	tbuf = NormalEvent::GetEventStr();
	buf << tbuf << " Pressure: ";
	if (GetWildcard(wc_pressure))
		buf << "*";
	else
		buf << (int)pressure;
	buf << ends;
	//delete tbuf;
	return (buf.str());
}

const char *
ChannelPressureEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete ChannelPressureEvent");
	pressure = *ptr;
	return (0);
}

const char *
ChannelPressureEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutByte(pressure))
		return ("Out of memory");
	return (0);
}

int
ChannelPressureEvent::Equal(const Event *e) const
{
	ChannelPressureEvent *eptr = (ChannelPressureEvent *)e;

	if (!NormalEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_pressure) && !GetWildcard(wc_pressure) &&
	    pressure != eptr->pressure)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const ChannelPressureEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
