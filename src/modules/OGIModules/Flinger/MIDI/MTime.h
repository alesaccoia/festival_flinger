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

#ifndef METATIME_H
#define METATIME_H
/*
NOTE by DJLB: Great confusion is caused by the naming of the last part of
the MetaTimeEvent as '32nd_notes_per_quarter_note'. The term is totally wrong.
There are ALWAYS eight 32nd notes per quarter note.
The value that is written into MIDI files is 32nd notes per 24 MIDI ticks.
The default value of this is 8, but it may change.
I have changed all instances of '32nd_notes_per_quarter_note' to
'32nd_notes_per_24_miditicks'.
(Although I prefer the European terminology for the note - demisemiquaver.)
ALSO, the term 'thirty_seconds' is confusing - it looks like a count of
30-second time intervals. I have renamed this 'DemiSemiQuavers'.
*/
#include "MEvent.h"

class MetaTimeEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaTimeEvent &e);
public:
	MetaTimeEvent();
	MetaTimeEvent(unsigned long time, int n = 4, int d = 4, int c = 24,
	    int t = 8);
	MetaTimeEvent(const MetaTimeEvent &e);
	virtual Event *Dup(void) const {return (new MetaTimeEvent(*this));}

	virtual EventType GetType(void) const {return (METATIME);}
	virtual char *GetTypeStr(void) const {return ("MetaTimeEvent");}
	virtual string GetEventStr(void) const;
	int GetNumerator(void) const {
		if (GetWildcard(wc_numerator))
			return (WC_NUMERATOR);
		else
			return (numerator);
	}
	int GetDenominator(void) const {
		if (GetWildcard(wc_denominator))
			return (WC_DENOMINATOR);
		else
			return (denominator);
	}
	int GetClocksPerBeat(void) const {
		if (GetWildcard(wc_clocks_per_beat))
			return (WC_CLOCKS_PER_BEAT);
		else
			return (clocks);
	}
	int Get32ndNotesPer24Ticks(void) const {
		if (GetWildcard(wc_32nd_notes_per_24_miditicks))
			return (WC_32ND_NOTES_PER_24_MIDITICKS);
		else
			return (DemiSemiQuavers);
	}

	void SetNumerator(int n) {
		if (n == WC_NUMERATOR)
			SetWildcard(wc_numerator);
		else {
			numerator = n;
			ClearWildcard(wc_numerator);
		}
	}
	void SetDenominator(int d) {
		if (d == WC_DENOMINATOR)
			SetWildcard(wc_denominator);
		else {
			denominator = d;
			ClearWildcard(wc_denominator);
		}
	}
	void SetClocksPerBeat(int c) {
		if (c == WC_CLOCKS_PER_BEAT)
			SetWildcard(wc_clocks_per_beat);
		else {
			clocks = c;
			ClearWildcard(wc_clocks_per_beat);
		}
	}
	void Set32ndNotesPer24Ticks(int t) {
		if (t == WC_32ND_NOTES_PER_24_MIDITICKS)
			SetWildcard(wc_32nd_notes_per_24_miditicks);
		else {
			DemiSemiQuavers = t;
			ClearWildcard(wc_32nd_notes_per_24_miditicks);
		}
	}

	MetaTimeEvent &operator=(const MetaTimeEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_NUMERATOR;
	static const int WC_DENOMINATOR;
	static const int WC_CLOCKS_PER_BEAT;
	static const int WC_32ND_NOTES_PER_24_MIDITICKS;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_numerator;
	static const unsigned long wc_denominator;
	static const unsigned long wc_clocks_per_beat;
	static const unsigned long wc_32nd_notes_per_24_miditicks;
	unsigned char numerator;
	unsigned char denominator;
	unsigned char clocks;
	unsigned char DemiSemiQuavers;
};
#endif
