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

#include "Event.h"
#include <sstream>
using namespace std;

const unsigned long Event::WC_TIME = 0xffffffff;
const unsigned long Event::wc_time = (1 << 0);

//Event::Event() : time(0L), wildcard(0), next_event(0), prev_event(0), node(0)
//{
//}

Event::Event(unsigned long t) : time(t), wildcard(0), next_event(0),
    prev_event(0), node(0)
{

	if (t == WC_TIME)
		SetWildcard(wc_time);
}

Event::Event(const Event &e) : time(e.time), wildcard(e.wildcard),
    next_event(0), prev_event(0), node(0)
{
}

int
Event::operator==(const Event &e) const
{

	if (this == &e)
		return (1);
	if (GetType() != e.GetType())
		return (0);
	return (this->Equal(&e));
}

Event &
Event::operator=(const Event &e)
{

	time = e.time;
	wildcard = e.wildcard;
	return (*this);
}

string
Event::GetEventStr(void) const
{
	ostringstream buf;

	buf << "Time: ";
	if (GetWildcard(wc_time))
		buf << "*";
	else
		buf << time;
	buf << " Type: " << this->GetTypeStr() << ends;
	return (buf.str());
}

void
Event::Print(ostream &os) const
{
	const char *str = this->GetEventStr().c_str();
	os << str;
	delete str;
}

int
Event::Equal(const Event *e) const
{

	if (!e->GetWildcard(wc_time) && !GetWildcard(wc_time) &&
	    time != e->time)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const Event &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);

}
