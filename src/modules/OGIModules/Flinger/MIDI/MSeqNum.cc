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

#include "MSeqNum.h"
#include <sstream>
using namespace std;

const long MetaSequenceNumberEvent::WC_NUMBER = 0x0fffffff;

const unsigned long MetaSequenceNumberEvent::wc_number = (1 << 1);

MetaSequenceNumberEvent::MetaSequenceNumberEvent() : number(0)
{
}

MetaSequenceNumberEvent::MetaSequenceNumberEvent(unsigned long t, long num) :
    MetaEvent(t), number(num)
{

	if (num == WC_NUMBER)
		SetWildcard(wc_number);
}

MetaSequenceNumberEvent::MetaSequenceNumberEvent(
    const MetaSequenceNumberEvent &e) : MetaEvent(e), number(e.number)
{
}

MetaSequenceNumberEvent &
MetaSequenceNumberEvent::operator=(const MetaSequenceNumberEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	number = e.number;
	return (*this);
}

string
MetaSequenceNumberEvent::GetEventStr(void) const
{
	ostringstream buf;
	string tbuf;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Number: ";
	if (GetWildcard(wc_number))
		buf << "*";
	else
		buf << number;
	buf << ends;
	//delete tbuf;
	return (buf.str());
}

const char *
MetaSequenceNumberEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	// get and throw away length
	if (t.GetVarValue() != 2)
		return ("Incomplete MetaSequenceNumberEvent - bad length");

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSequenceNumberEvent");
	number = (short)*ptr << 8;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSequenceNumberEvent");
	number |= *ptr;
	return (0);
}

const char *
MetaSequenceNumberEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(2))
		return ("Out of memory");
	if (!t.PutByte((number >> 8) & 0x00ff))
		return ("Out of memory");
	if (!t.PutByte(number & 0x00ff))
		return ("Out of memory");
	return (0);
}

int
MetaSequenceNumberEvent::Equal(const Event *e) const
{
	MetaSequenceNumberEvent *eptr = (MetaSequenceNumberEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_number) && !GetWildcard(wc_number) &&
	    number != eptr->number)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const MetaSequenceNumberEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
