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

#include <assert.h>
#include <sstream>
using namespace std;

#include "MPortNum.h"

const int MetaPortNumberEvent::WC_PORT = -1;
const unsigned long MetaPortNumberEvent::wc_port = (1 << 1);

MetaPortNumberEvent::MetaPortNumberEvent() : port(0)
{
}

MetaPortNumberEvent::MetaPortNumberEvent(unsigned long t, int p) :
    MetaEvent(t), port(p)
{

	if (p == WC_PORT)
		SetWildcard(wc_port);
}

MetaPortNumberEvent::MetaPortNumberEvent(const MetaPortNumberEvent &e) :
    MetaEvent(e), port(e.port)
{
}

MetaPortNumberEvent::~MetaPortNumberEvent()
{
}

MetaPortNumberEvent &
MetaPortNumberEvent::operator=(const MetaPortNumberEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	port = e.port;
	return (*this);
}

string
MetaPortNumberEvent::GetEventStr(void) const
{
	ostringstream buf;
	string tbuf;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Port: ";
	if (GetWildcard(wc_port))
		buf << "*";
	else
		buf << port;
	buf << ends;
	//delete tbuf;
	return (buf.str());
}

const char *
MetaPortNumberEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if (t.GetVarValue() != 1)
		return ("Bad length for MetaPortNumberEvent");
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaPortNumberEvent - missing port");
	port = *ptr;
	return (0);
}

const char *
MetaPortNumberEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(1))
		return ("Out of memory");
	if (!t.PutByte(port))
		return ("Out of memory");
	return (0);
}

int
MetaPortNumberEvent::Equal(const Event *e) const
{
	MetaPortNumberEvent *eptr = (MetaPortNumberEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_port) && !GetWildcard(wc_port) &&
	    port != eptr->port)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const MetaPortNumberEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
