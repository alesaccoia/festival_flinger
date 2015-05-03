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


#include "MSMPTE.h"
#include <sstream>
using namespace std;

const int MetaSMPTEEvent::WC_HOUR = -1;
const int MetaSMPTEEvent::WC_MINUTE = -1;
const int MetaSMPTEEvent::WC_SECOND = -1;
const int MetaSMPTEEvent::WC_FRAME = -1;
const int MetaSMPTEEvent::WC_FRACTIONAL_FRAME = -1;

const unsigned long MetaSMPTEEvent::wc_hour = (1 << 1);
const unsigned long MetaSMPTEEvent::wc_minute = (1 << 2);
const unsigned long MetaSMPTEEvent::wc_second = (1 << 3);
const unsigned long MetaSMPTEEvent::wc_frame = (1 << 4);
const unsigned long MetaSMPTEEvent::wc_fractional_frame = (1 << 5);

MetaSMPTEEvent::MetaSMPTEEvent() : hour(0), minute(0), second(0), frame(0),
    fractional_frame(0)
{
}

MetaSMPTEEvent::MetaSMPTEEvent(unsigned long t, int h, int m, int s, int f,
    int ff) : MetaEvent(t), hour(h), minute(m), second(s), frame(f),
    fractional_frame(ff)
{

	if (h == WC_HOUR)
		SetWildcard(wc_hour);
	if (m == WC_MINUTE)
		SetWildcard(wc_minute);
	if (s == WC_SECOND)
		SetWildcard(wc_second);
	if (f == WC_FRAME)
		SetWildcard(wc_frame);
	if (ff == WC_FRACTIONAL_FRAME)
		SetWildcard(wc_fractional_frame);
}

MetaSMPTEEvent::MetaSMPTEEvent(const MetaSMPTEEvent &e) : MetaEvent(e),
    hour(e.hour), minute(e.minute), second(e.second), frame(e.frame),
    fractional_frame(e.fractional_frame)
{
}

MetaSMPTEEvent &
MetaSMPTEEvent::operator=(const MetaSMPTEEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	hour = e.hour;
	minute = e.minute;
	second = e.second;
	frame = e.frame;
	fractional_frame = e.fractional_frame;
	return (*this);
}

string
MetaSMPTEEvent::GetEventStr(void) const
{
	ostringstream buf;
	string tbuf;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Hour: ";
	if (GetWildcard(wc_hour))
		buf << "*";
	else
		buf << (int)hour;
	buf << " Minute: ";
	if (GetWildcard(wc_minute))
		buf << "*";
	else
		buf << (int)minute;
	buf << " Second: ";
	if (GetWildcard(wc_second))
		buf << "*";
	else
		buf << (int)second;
	buf << " Frame: ";
	if (GetWildcard(wc_frame))
		buf << "*";
	else
		buf << (int)frame;
	buf << " Fractional Frame: ";
	if (GetWildcard(wc_fractional_frame))
		buf << "*";
	else
		buf << (int)fractional_frame;
	buf << ends;
	//delete tbuf;
	return (buf.str());
}

const char *
MetaSMPTEEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	// get and throw away length
	if (t.GetVarValue() != 5)
		return ("Incomplete metaSMPTEEvent - bad length");

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSMPTEEvent - missing hour");
	hour = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSMPTEEvent - missing minute");
	minute = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSMPTEEvent - missing second");
	second = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSMPTEEvent - missing frame");
	frame = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSMPTEEvent - missing fractional "
		    "frame");
	fractional_frame = *ptr;
	return (0);
}

const char *
MetaSMPTEEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(5))
		return ("Out of memory");
	if (!t.PutByte(hour))
		return ("Out of memory");
	if (!t.PutByte(minute))
		return ("Out of memory");
	if (!t.PutByte(second))
		return ("Out of memory");
	if (!t.PutByte(frame))
		return ("Out of memory");
	if (!t.PutByte(fractional_frame))
		return ("Out of memory");
	return (0);
}

int
MetaSMPTEEvent::Equal(const Event *e) const
{
	MetaSMPTEEvent *eptr = (MetaSMPTEEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_hour) && !GetWildcard(wc_hour) &&
	    hour != eptr->hour)
		return (0);
	if (!eptr->GetWildcard(wc_minute) && !GetWildcard(wc_minute) &&
	    minute != eptr->minute)
		return (0);
	if (!eptr->GetWildcard(wc_second) && !GetWildcard(wc_second) &&
	    second != eptr->second)
		return (0);
	if (!eptr->GetWildcard(wc_frame) && !GetWildcard(wc_frame) &&
	    frame != eptr->frame)
		return (0);
	if (!eptr->GetWildcard(wc_fractional_frame) &&
	    !GetWildcard(wc_fractional_frame) &&
	    fractional_frame != eptr->fractional_frame)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const MetaSMPTEEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
