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

#include "MTempo.h"
#include <sstream>
using namespace std;

const short MetaTempoEvent::WC_TEMPO = -1;
const unsigned long MetaTempoEvent::wc_tempo = (1 << 1);

MetaTempoEvent::MetaTempoEvent() : tempo(120)
{
}

MetaTempoEvent::MetaTempoEvent(unsigned long t, short temp) :
    MetaEvent(t), tempo(temp)
{

	if (temp == WC_TEMPO)
		SetWildcard(wc_tempo);
}

MetaTempoEvent::MetaTempoEvent(
    const MetaTempoEvent &e) : MetaEvent(e), tempo(e.tempo)
{
}

MetaTempoEvent &
MetaTempoEvent::operator=(const MetaTempoEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	tempo = e.tempo;
	return (*this);
}

string
MetaTempoEvent::GetEventStr(void) const
{
	ostringstream buf;
	string tbuf;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Tempo: ";
	if (GetWildcard(wc_tempo))
		buf << "*";
	else
		buf << (int)tempo;
	buf << ends;
	//delete tbuf;
	return (buf.str());
}

const char *
MetaTempoEvent::SMFRead(SMFTrack &t)
{
	long usec;
	const unsigned char *ptr;

	// get and throw away length
	if (t.GetVarValue() != 3)
		return  ("Incomplete MetaTempoEvent - bad length");

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTempoEvent");
	usec = *ptr * 0x10000;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTempoEvent");
	usec += *ptr * 0x100;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTempoEvent");
	usec += *ptr;

	tempo = (short)(60000000L / usec);
	return (0);
}

const char *
MetaTempoEvent::SMFWrite(SMFTrack &t) const
{
	long usec;

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(3))
		return ("Out of memory");
	usec = 60000000L / tempo;
	if (!t.PutByte((unsigned char)(usec / 0x10000)))
		return ("Out of memory");
	usec %= 0x10000;
	if (!t.PutByte((unsigned char)(usec / 0x100)))
		return ("Out of memory");
	usec %= 0x100;
	if (!t.PutByte((unsigned char)usec))
		return ("Out of memory");
	return (0);
}

int
MetaTempoEvent::Equal(const Event *e) const
{
	MetaTempoEvent *eptr = (MetaTempoEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_tempo) && !GetWildcard(wc_tempo) &&
	    tempo != eptr->tempo)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const MetaTempoEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
