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

#include "NormEvnt.h"
#include <sstream>
using namespace std;


const int  NormalEvent::WC_CHANNEL = -1;
const unsigned long NormalEvent::wc_channel = (1 << 1);


//NormalEvent::NormalEvent() : channel(0)
//{
//}

NormalEvent::NormalEvent(unsigned long t, int chan) : Event(t), channel(chan)
{

	if (chan == WC_CHANNEL)
		SetWildcard(wc_channel);
}

NormalEvent::NormalEvent(const NormalEvent &e) : Event(e), channel(e.channel)
{
}

NormalEvent &
NormalEvent::operator=(const NormalEvent &e)
{

	(Event)*this = (Event)e;
	channel = e.channel;
	return (*this);
}

string
NormalEvent::GetEventStr(void) const
{
	ostringstream buf;
	string tbuf;

	tbuf = Event::GetEventStr();
	buf << tbuf << " Channel: ";
	if (GetWildcard(wc_channel))
		buf << "*";
	else
		buf << (int)channel;
	buf << ends;
	//delete tbuf;
	return (buf.str());
}

int
NormalEvent::Equal(const Event *e) const
{
	NormalEvent *eptr = (NormalEvent *)e;

	if (!Event::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_channel) && !GetWildcard(wc_channel) &&
	    channel != eptr->channel)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const NormalEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
