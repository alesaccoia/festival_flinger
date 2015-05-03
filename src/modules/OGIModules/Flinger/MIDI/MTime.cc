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

/*
NOTE by DJLB: Great confusion is caused by the naming of the last part of
the MetaTimeEvent as '32nd_notes_per_quarter_note'. The term is totally wrong.
There are ALWAYS eight 32nd notes per quarter note.
The value that is written into MIDI files is 32nd notes per 24 MIDI ticks.
The default value of this is 8, but it may change.
I have changed all instances of '32nd_notes_per_quarter_note' to
'32nd_notes_per_24_miditicks'.
(Although I prefer the European terminology for the 32nd note - demisemiquaver.)
ALSO, the term 'thirty_seconds' is confusing - it looks like a count of
30-second time intervals. I have renamed this 'DemiSemiQuavers'.
*/
#include "MTime.h"
#include <sstream>
using namespace std;

const int MetaTimeEvent::WC_NUMERATOR = 0;
const int MetaTimeEvent::WC_DENOMINATOR = 0;
const int MetaTimeEvent::WC_CLOCKS_PER_BEAT = 0;
const int MetaTimeEvent::WC_32ND_NOTES_PER_24_MIDITICKS = 0;

const unsigned long MetaTimeEvent::wc_numerator = (1 << 1);
const unsigned long MetaTimeEvent::wc_denominator = (1 << 2);
const unsigned long MetaTimeEvent::wc_clocks_per_beat = (1 << 3);
const unsigned long MetaTimeEvent::wc_32nd_notes_per_24_miditicks = (1 << 4);

MetaTimeEvent::MetaTimeEvent() : numerator(4), denominator(4), clocks(24),
    DemiSemiQuavers(8)
{
}

MetaTimeEvent::MetaTimeEvent(unsigned long time, int n, int d, int c, int t)
    : MetaEvent(time), numerator(n), denominator(d), clocks(c),
    DemiSemiQuavers(t)
{

	if (n == WC_NUMERATOR)
		SetWildcard(wc_numerator);
	if (d == WC_DENOMINATOR)
		SetWildcard(wc_denominator);
	if (c == WC_CLOCKS_PER_BEAT)
		SetWildcard(wc_clocks_per_beat);
	if (t == WC_32ND_NOTES_PER_24_MIDITICKS)
		SetWildcard(wc_32nd_notes_per_24_miditicks);
}

MetaTimeEvent::MetaTimeEvent(const MetaTimeEvent &e) : MetaEvent(e),
    numerator(e.numerator), denominator(e.denominator), clocks(e.clocks),
    DemiSemiQuavers(e.DemiSemiQuavers)
{
}

MetaTimeEvent &
MetaTimeEvent::operator=(const MetaTimeEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	numerator = e.numerator;
	denominator = e.denominator;
	clocks = e.clocks;
	DemiSemiQuavers = e.DemiSemiQuavers;
	return (*this);
}

string
MetaTimeEvent::GetEventStr(void) const
{
	ostringstream buf;
	string tbuf;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Numerator: ";
	if (GetWildcard(wc_numerator))
		buf << "*";
	else
		buf << (int)numerator;
	buf << " Denominator: ";
	if (GetWildcard(wc_denominator))
		buf << "*";
	else
		buf << (int)denominator;
	buf << " Clocks Per Metronome Beat: ";
	if (GetWildcard(wc_clocks_per_beat))
		buf << "*";
	else
		buf << (int)clocks;
	buf << " 32nd Notes Per 24 MIDI ticks: ";
	if (GetWildcard(wc_32nd_notes_per_24_miditicks))
		buf << "*";
	else
		buf << (int)DemiSemiQuavers;
	buf << ends;
	//delete tbuf;
	return (buf.str());
}

const char *
MetaTimeEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;
	unsigned char i, powof2;

	// get and throw away length
	if (t.GetVarValue() != 4)
		return ("Incomplete MetaTimeEvent - bad length");
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTimeEvent - missing numerator");
	numerator = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTimeEvent - missing denominator");
	powof2 = *ptr;
	denominator = 1;
	for (i = 0; i < powof2; i++)
		denominator *= 2;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTimeEvent - missing clocks");
	clocks = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTimeEvent - missing 32nds");
	DemiSemiQuavers = *ptr;
	return (0);
}

const char *
MetaTimeEvent::SMFWrite(SMFTrack &t) const
{
	unsigned char i, powof2;

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(4))
		return ("Out of memory");
	if (!t.PutByte(numerator))
		return ("Out of memory");
	for (i = 0, powof2 = 1; powof2 <= denominator; powof2 *= 2, i++);
	i--;
	if (!t.PutByte(i))
		return ("Out of memory");
	if (!t.PutByte(clocks))
		return ("Out of memory");
	if (!t.PutByte(DemiSemiQuavers))
		return ("Out of memory");
	return (0);
}

int
MetaTimeEvent::Equal(const Event *e) const
{
	MetaTimeEvent *eptr = (MetaTimeEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_numerator) && !GetWildcard(wc_numerator) &&
	    numerator != eptr->numerator)
		return (0);
	if (!eptr->GetWildcard(wc_denominator) && !GetWildcard(wc_denominator)
	    && denominator != eptr->denominator)
		return (0);
	if (!eptr->GetWildcard(wc_clocks_per_beat) &&
	    !GetWildcard(wc_clocks_per_beat) && clocks != eptr->clocks)
		return (0);
	if (!eptr->GetWildcard(wc_32nd_notes_per_24_miditicks) &&
	    !GetWildcard(wc_32nd_notes_per_24_miditicks) &&
	    DemiSemiQuavers != eptr->DemiSemiQuavers)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const MetaTimeEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
