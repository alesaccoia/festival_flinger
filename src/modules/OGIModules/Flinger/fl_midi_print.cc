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

extern "C" {
#include "MIDI/tcl.h"
}
//DJLB 17/09/2005. Change to standardised stream stuff.
/* Damn Microsoft compiler can't deal with the real name of the include file */
/*#ifndef _MSC_VER
#include <strstream.h>
#else
//#include <strstrea.h>
#endif
*/
//#include <strstream>
#include <sstream>

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "MIDI/AllEvent.h"
#include "fl_midi_print.h"
#include "fl_midi_parse.h"
#include "fl_io.h"						// Flinger_get_debug_file_flag

// DJLB Additions to provide more useful info. to the user.
#include "EST_String.h"
#include "stdlib.h"

/*
Determine the bar/beat timing from MIDI file ticks and previous MetaTime events.
MetaTime events don't actually have any effect on Flinger. But they do provide
data to enable information relating to the sheet music to be generated i.e.
bar and beat numbers.
HOWEVER, the situation is complicated because in Form 1 MIDI files, these
events are in the 'master' track, and are thus read before note, lyric etc.
events. So they have to be stored in a list, and 'used' as these latter events
turn up (just like the MetatTempo events).
*/
static long lnTicksBeforeLastMetaTimeEvent = 0;
static int	inBarBeforeLastMetaTimeEvent = 0,
			inBarsAfterLastMetaTimeEvent = 0,
			inTicksPerBar = 960, inBeatsPerBar = 4;
tstTimeSigEvent *plstTimeSigEvents = NULL;
void fvdCheckTimeSignatureEventList( unsigned long ulNoteTime );
extern bool boPrintMIDIEvents;
extern int Division; // from fl_midi_in.cc

/*==============================================================================*/

void fvdResetTimeSignatureStores( void )
{
	lnTicksBeforeLastMetaTimeEvent = 0;
	inBarBeforeLastMetaTimeEvent = 0;
	inBarsAfterLastMetaTimeEvent = 0;
	inTicksPerBar = 960, inBeatsPerBar = 4;
}

/*==============================================================================*/

void fvdCheckTimeSignatureEventList( unsigned long ulEventTime )
{
/*
First, check the list of time signature events for one occurring before or
coincident with this event. If there is one, it will be at the tail of the list,
because the events have been added to the head of the list in chronological
order, and any that have been used have been removed from the tail.
*/
tstTimeSigEvent stTimeSignature;
CHECK_LIST:
	if( GetFromListTail(&stTimeSignature, plstTimeSigEvents) !=NULL )
	{	// The list is not empty. Get the structure at the tail of the list.
		if(stTimeSignature.ulTime <= ulEventTime)
		{	// The tempo change event is set for this, or an earlier time. Use it.

            /* Remove the tail of the list. (Has to be done before calling 
            fpchGetBarAndBeats(), as this function is called within fpchGetBarAndBeats().
            If the tail is not removed, an infinite reiteration could occur.*/
			DeleteFromTail(plstTimeSigEvents);

			if (boPrintMIDIEvents)
			{
				if (Flinger_get_debug_file_flag()) {
					cout << stTimeSignature.ulTime << " "
							<< fpchGetBarAndBeats(stTimeSignature.ulTime).c_str()
							<< " Using Time Signature. New signature = "
							<< stTimeSignature.inNumerator << "/"
							<< stTimeSignature.inDenominator << " time." << endl;
				}
			}
			fvdUpdateTimeSig(stTimeSignature.ulTime,
											stTimeSignature.inNumerator,
											stTimeSignature.inDenominator );
            goto CHECK_LIST;
		}
	}
}

const string fpchGetBarAndBeats(long lnTicksFromEdge )
{
	int inBar, inTicksInBar;
	double dfBeat;
	ostringstream buf;
    //DJLB 09/06/2008. Moved from FL_add_note()
/*
DJLB addition to handle mid-song time signature changes and common events.
*/
	fvdCheckTimeSignatureEventList(lnTicksFromEdge);

	inBarsAfterLastMetaTimeEvent =
		(lnTicksFromEdge - lnTicksBeforeLastMetaTimeEvent)/inTicksPerBar;

	inBar = inBarBeforeLastMetaTimeEvent + inBarsAfterLastMetaTimeEvent;

	inTicksInBar = lnTicksFromEdge - lnTicksBeforeLastMetaTimeEvent -
						inBarsAfterLastMetaTimeEvent * inTicksPerBar;

	dfBeat = 1.0 + (double)(inBeatsPerBar * inTicksInBar)/(double)inTicksPerBar;

	buf << "Bar:" << (inBar+1) << " Beat:" << dfBeat << ends;
	return buf.str();
}

/*==============================================================================*/

void fvdUpdateTimeSig(unsigned long lnTicksFromEdge, int inNumerator,
														int inDenominator )
{
/*
	'Division' is MIDIticks per crochet (quarter note), and comes from
	the header chunk of the MIDI file. It does not vary through the file.
	Its value is obtained from code in fl_midi_in.cc.
*/
    /* Adjust lnTicksFromEdge (the time for this signature event) to the nearest
        Bar. (Some music software does not precisely set this signature event 
        at a Bar.)*/
    long lnLastBarTicksFromEdge =
        lnTicksBeforeLastMetaTimeEvent + inBarsAfterLastMetaTimeEvent * inTicksPerBar;
    if( (int)(lnTicksFromEdge - lnLastBarTicksFromEdge) > inTicksPerBar/2)
        lnTicksFromEdge = lnLastBarTicksFromEdge + inTicksPerBar;
    else
        lnTicksFromEdge = lnLastBarTicksFromEdge;

    inTicksPerBar = inNumerator * (Division/8) * (32/inDenominator);
	inBeatsPerBar = inNumerator;
    
	lnTicksBeforeLastMetaTimeEvent = lnTicksFromEdge;

	inBarBeforeLastMetaTimeEvent += inBarsAfterLastMetaTimeEvent;// i.e. this bar.

	inBarsAfterLastMetaTimeEvent = 0;
}
//End DJLB additions
/*==============================================================================*/

void Tclm_PrintEvent(ostream &buf, Event *e, bool boCommonEvent)
{
	//char *str;
	string str;
	switch (e->GetType()) {
	case NOTEOFF:
		if (((NoteEvent *)e)->GetNotePair() != 0)
		{
			buf << ends;
			return;
		}
		str = Tclm_PrintNoteOff((NoteOffEvent *)e);
		break;
	case NOTEON:
		if (((NoteEvent *)e)->GetNotePair() == 0)
			str = Tclm_PrintNoteOn((NoteOnEvent *)e);
		else {
			if ((int)((NoteEvent *)e)->GetVelocity() == 0)
			{
				buf << ends;
				return;
			}
			str = Tclm_PrintNote((NoteOnEvent *)e);
		}
		break;
	case KEYPRESSURE:
		str = Tclm_PrintKeyPressure((KeyPressureEvent *)e);
		break;
	case PARAMETER:
		str = Tclm_PrintParameter((ParameterEvent *)e);
		break;
	case PROGRAM:
		str = Tclm_PrintProgram((ProgramEvent *)e);
		break;
	case CHANNELPRESSURE:
		str = Tclm_PrintChannelPressure((ChannelPressureEvent *)e);
		break;
	case PITCHWHEEL:
		str = Tclm_PrintPitchWheel((PitchWheelEvent *)e);
		break;
	case SYSTEMEXCLUSIVE:
		str = Tclm_PrintSystemExclusive((SystemExclusiveEvent *)e);
		break;
	case METASEQUENCENUMBER:
		str = Tclm_PrintMetaSequenceNumber(
		    (MetaSequenceNumberEvent *)e);
		break;
	case METATEXT:
		str = Tclm_PrintMetaText((MetaTextEvent *)e);
		break;
	case METACOPYRIGHT:
		str = Tclm_PrintMetaCopyright((MetaCopyrightEvent *)e);
		break;
	case METASEQUENCENAME:
		str = Tclm_PrintMetaSequenceName((MetaSequenceNameEvent *)e);
		break;
	case METAINSTRUMENTNAME:
		str = Tclm_PrintMetaInstrumentName(
		    (MetaInstrumentNameEvent *)e);
		break;
	case METALYRIC:
		buf << endl; //DJLB addition: endl added here to improve readability.
		str = Tclm_PrintMetaLyric((MetaLyricEvent *)e);
		break;
	case METAMARKER:
		str = Tclm_PrintMetaMarker((MetaMarkerEvent *)e);
		break;
	case METACUE:
		str = Tclm_PrintMetaCue((MetaCueEvent *)e);
		break;
	case METACHANNELPREFIX:
		str = Tclm_PrintMetaChannelPrefix((MetaChannelPrefixEvent *)e);
		break;
	case METAPORTNUMBER:
		str = Tclm_PrintMetaPortNumber((MetaPortNumberEvent *)e);
		break;
	case METAENDOFTRACK:
		str = Tclm_PrintMetaEndOfTrack((MetaEndOfTrackEvent *)e);
		break;
	case METATEMPO:
		str = Tclm_PrintMetaTempo((MetaTempoEvent *)e);
		break;
	case METASMPTE:
		str = Tclm_PrintMetaSMPTE((MetaSMPTEEvent *)e);
		break;
	case METATIME:
		str = Tclm_PrintMetaTime((MetaTimeEvent *)e);
		break;
	case METAKEY:
        str = Tclm_PrintMetaKey((MetaKeyEvent *)e);
		break;
	case METASEQUENCERSPECIFIC:
		str = Tclm_PrintMetaSequencerSpecific(
		    (MetaSequencerSpecificEvent *)e);
		break;
	case METAUNKNOWN:
		str = Tclm_PrintMetaUnknown((MetaUnknownEvent *)e);
		break;
	default:
		str = '0';
		break;
	}
    
	if (e->GetTime() == Event::WC_TIME)
		buf << "* " << str << ends;
	else
	{
//DJLB modification
/* Re-arrangement of output to improve readability.
	A lyric event is preceded by a line feed (see above), so it appears
	coupled to the associated note event that follows.
*/
		buf << endl << e->GetTime() << " "
					<< fpchGetBarAndBeats(e->GetTime());
		if(boCommonEvent)
			buf << " Adding common ";
		else
			buf << " Adding track ";

		buf << str << ends << endl;
		// By DJLB. endl is moved to here from fl_midi_in.cc,
		//	so that non-printed events don't cause a line-feed.
//End DJLB modification
	}
	//delete str;
}

/*==============================================================================*/

//char *Tclm_PrintNoteOff(NoteOffEvent *e)
string Tclm_PrintNoteOff(NoteOffEvent *e)
{
	ostringstream buf;
	int p, v;

	p = e->GetPitch();
	v = e->GetVelocity();

	buf << "NoteOff Event";

	buf << ", Pitch No. ";
	if (p == NoteOffEvent::WC_PITCH)
		buf << "*";
	else
		buf << p;
	buf << ", Velocity ";
	if (v == NoteOffEvent::WC_VELOCITY)
		buf << "*";
	else
		buf << v;
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintNoteOn(NoteOnEvent *e)
string Tclm_PrintNoteOn(NoteOnEvent *e)
{
	ostringstream buf;
	int p, v;

	p = e->GetPitch();
	v = e->GetVelocity();
	buf << "NoteOn Event";

	buf << ", Pitch No. ";
	if (p == NoteOnEvent::WC_PITCH)
		buf << "*";
	else
		buf << p;
	buf << ", Velocity ";
	if (v == NoteOnEvent::WC_VELOCITY)
		buf << "*";
	else
		buf << v;
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/
//char *Tclm_PrintNote(NoteOnEvent *e)
string Tclm_PrintNote(NoteOnEvent *e)
{
	ostringstream buf;
	int p, v;
	unsigned long d;

	p = e->GetPitch();
	v = e->GetVelocity();
	d = e->GetDuration();
	buf << "Note Event";

	buf << ", Pitch No. ";
	if (p == NoteOnEvent::WC_PITCH)
		buf << "*";
	else
		buf << p;
	buf << ", Velocity ";
	if (v == NoteOnEvent::WC_VELOCITY)
		buf << "*";
	else
		buf << v;
	buf << ", Duarion (ticks) ";
	if (d == NoteOnEvent::WC_DURATION)
		buf << "*";
	else
		buf << d;
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/
//char *Tclm_PrintKeyPressure(KeyPressureEvent *e)
string Tclm_PrintKeyPressure(KeyPressureEvent *e)
{
	ostringstream buf;
	int p;

	p = e->GetPitch();
	buf << "KeyPressure Event";

	buf << ", Pitch No. ";
	if (p == KeyPressureEvent::WC_PITCH)
		buf << "*";
	else
		buf << p;
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintParameter(ParameterEvent *e)
string Tclm_PrintParameter(ParameterEvent *e)
{
	ostringstream buf;
	int p, v;

	p = e->GetParameter();
	v = e->GetValue();

	buf << "Parameter Event";

	buf << ", No. ";
	if (p == ParameterEvent::WC_PARAMETER)
		buf << "*";
	else
		buf << p;
	buf << ", value ";
	if (v == ParameterEvent::WC_VALUE)
		buf << "*";
	else
		buf << v;
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintProgram(ProgramEvent *e)
string Tclm_PrintProgram(ProgramEvent *e)
{
	ostringstream buf;
	int v;

	v = e->GetValue();
	buf << "Program Event";

	buf << ", Value ";
	if (v == ProgramEvent::WC_VALUE)
		buf << "*";
	else
		buf << v;
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintChannelPressure(ChannelPressureEvent *e)
string Tclm_PrintChannelPressure(ChannelPressureEvent *e)
{
	ostringstream buf;
	int p;

	p = e->GetPressure();

	buf << "ChannelPressure Event";

	buf << ", Pressure ";
	if (p == ChannelPressureEvent::WC_PRESSURE)
		buf << "*";
	else
		buf << p;
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintPitchWheel(PitchWheelEvent *e)
string Tclm_PrintPitchWheel(PitchWheelEvent *e)
{
	ostringstream buf;
	long v;

	v = e->GetValue();

	buf << "PitchWheel Event";

	buf << ", Value ";
	if (v == PitchWheelEvent::WC_VALUE)
		buf << "*";
	else
		buf << v;
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintSystemExclusive(SystemExclusiveEvent *e)
string Tclm_PrintSystemExclusive(SystemExclusiveEvent *e)
{
	ostringstream buf;
	const unsigned char *d;
	long l;

	d = e->GetData();
	l = e->GetLength();

	buf << "SystemExclusive Event ";
	if (e->GetContinued() == 1)
		buf << "continued ";
	if (d == SystemExclusiveEvent::WC_DATA)
		buf << "*";
	else {
		buf << "{";
		Tclm_PrintData(buf, e->GetData(), e->GetLength());
		buf << "}";
	}
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaSequenceNumber(MetaSequenceNumberEvent *e)
string Tclm_PrintMetaSequenceNumber(MetaSequenceNumberEvent *e)
{
	ostringstream buf;
	long l;

	l = e->GetNumber();

	buf << "MetaSequenceNumber Event ";
	if (l == MetaSequenceNumberEvent::WC_NUMBER)
		buf << "*";
	else
		buf << l;
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaText(MetaTextEvent *e)
string Tclm_PrintMetaText(MetaTextEvent *e)
{
	ostringstream buf;
	const char *s;

	s = e->GetString();

	buf << "MetaText Event ";
	if (s == MetaTextEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaCopyright(MetaCopyrightEvent *e)
string Tclm_PrintMetaCopyright(MetaCopyrightEvent *e)
{
	ostringstream buf;
	const char *s;

	s = e->GetString();
	buf << "MetaCopyright Event ";
	if (s == MetaCopyrightEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaSequenceName(MetaSequenceNameEvent *e)
string Tclm_PrintMetaSequenceName(MetaSequenceNameEvent *e)
{
	ostringstream buf;
	const char *s;

	s = e->GetString();
	buf << "MetaSequenceName Event ";
	if (s == MetaSequenceNameEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaInstrumentName(MetaInstrumentNameEvent *e)
string Tclm_PrintMetaInstrumentName(MetaInstrumentNameEvent *e)
{
	ostringstream buf;
	const char *s;

	s = e->GetString();
	buf << "MetaInstrumentName Event ";
	if (s == MetaInstrumentNameEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaLyric(MetaLyricEvent *e)
string Tclm_PrintMetaLyric(MetaLyricEvent *e)
{
	ostringstream buf;
	const char *s;

	s = e->GetString();
	buf << "MetaLyric Event ";
	if (s == MetaLyricEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaMarker(MetaMarkerEvent *e)
string Tclm_PrintMetaMarker(MetaMarkerEvent *e)
{
	ostringstream buf;
	const char *s;

	s = e->GetString();
	buf << "MetaMarker Event ";
	if (s == MetaMarkerEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaCue(MetaCueEvent *e)
string Tclm_PrintMetaCue(MetaCueEvent *e)
{
	ostringstream buf;
	const char *s;

	s = e->GetString();
	buf << "MetaCue Event ";
	if (s == MetaCueEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaChannelPrefix(MetaChannelPrefixEvent *e)
string Tclm_PrintMetaChannelPrefix(MetaChannelPrefixEvent *e)
{
	ostringstream buf;
	const unsigned char *d;

	d = e->GetData();
	buf << "MetaChannelPrefix Event ";
	if (d == MetaChannelPrefixEvent::WC_DATA)
		buf << "*";
	else {
		buf << "{";
		Tclm_PrintData(buf, d, e->GetLength());
		buf << "}";
	}
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaPortNumber(MetaPortNumberEvent *e)
string Tclm_PrintMetaPortNumber(MetaPortNumberEvent *e)
{
	ostringstream buf;
	int p;

	p = e->GetPort();
	buf << "MetaPortNumber Event ";
	if (p == MetaPortNumberEvent::WC_PORT)
		buf << "*";
	else
		buf << p;
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaEndOfTrack(MetaEndOfTrackEvent *e)
string Tclm_PrintMetaEndOfTrack(MetaEndOfTrackEvent *e)
{
	ostringstream buf;
	MetaEndOfTrackEvent *dummy;

	// shut up a warning
	dummy = e;

	buf << "MetaEndOfTrack Event" << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaTempo(MetaTempoEvent *e)
string Tclm_PrintMetaTempo(MetaTempoEvent *e)
{
	ostringstream buf;
	short t;

	t = e->GetTempo();
	buf << "MetaTempo Event, Value ";
	if (t == MetaTempoEvent::WC_TEMPO)
		buf << "*";
	else
		buf << t;
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaSMPTE(MetaSMPTEEvent *e)
string Tclm_PrintMetaSMPTE(MetaSMPTEEvent *e)
{
	ostringstream buf;
	int h, m, s, f, ff;

	h = e->GetHour();
	m = e->GetMinute();
	s = e->GetSecond();
	f = e->GetFrame();
	ff = e->GetFractionalFrame();
	buf << "MetaSMPTE Event, Time ";
	if (h == MetaSMPTEEvent::WC_HOUR)
		buf << "*";
	else
		buf << h;
	buf << " ";
	if (m == MetaSMPTEEvent::WC_MINUTE)
		buf << "*";
	else
		buf << m;
	buf << " ";
	if (s == MetaSMPTEEvent::WC_SECOND)
		buf << "*";
	else
		buf << s;
	buf << " ";
	if (f == MetaSMPTEEvent::WC_FRAME)
		buf << "*";
	else
		buf << f;
	buf << " ";
	if (ff == MetaSMPTEEvent::WC_FRACTIONAL_FRAME)
		buf << "*";
	else
		buf << ff;
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaTime(MetaTimeEvent *e)
string Tclm_PrintMetaTime(MetaTimeEvent *e)
{
	ostringstream buf;
	int n, d, c, t;

	n = e->GetNumerator();
	d = e->GetDenominator();
	c = e->GetClocksPerBeat();
	t = e->Get32ndNotesPer24Ticks();

	buf << "MetaTime Event ";
	if (n == MetaTimeEvent::WC_NUMERATOR)
		buf << "*";
	else
		buf << n;
	buf << "/";
	if (d == MetaTimeEvent::WC_DENOMINATOR)
		buf << "*";
	else
		buf << d;
	buf << ", Clocks per Beat ";
	if (c == MetaTimeEvent::WC_CLOCKS_PER_BEAT)
		buf << "*";
	else
		buf << c;
	buf << ", ";
	if (t == MetaTimeEvent::WC_32ND_NOTES_PER_24_MIDITICKS)
		buf << "*";
	else
		buf << t << " demisemiquavers per 24 ticks" ;
	buf << ends;

	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaKey(MetaKeyEvent *e)
string Tclm_PrintMetaKey(MetaKeyEvent *e)
{
	ostringstream buf;

	buf << "MetaKey Event ";
	if (e->GetKey() == MetaKeyEvent::WC_KEY)
		buf << "*";
	else
		buf << "{" << e->GetKeyStr() << "}";
	buf << " ";
	if (e->GetMode() == MetaKeyEvent::WC_MODE)
		buf << "*";
	else
		buf << e->GetModeStr();
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaSequencerSpecific(MetaSequencerSpecificEvent *e)
string Tclm_PrintMetaSequencerSpecific(MetaSequencerSpecificEvent *e)
{
	ostringstream buf;
	const unsigned char *d;

	d = e->GetData();

	buf << "MetaSequencerSpecific Event ";
	if (d == MetaSequencerSpecificEvent::WC_DATA)
		buf << "*";
	else {
		buf << "{";
		Tclm_PrintData(buf, d, e->GetLength());
		buf << "}";
	}
	buf << ends;
	return (buf.str());
}

/*==============================================================================*/

//char *Tclm_PrintMetaUnknown(MetaUnknownEvent *e)
string Tclm_PrintMetaUnknown(MetaUnknownEvent *e)
{
	ostringstream buf;
	int t;
	const unsigned char *d;

	t = e->GetMetaType();
	d = e->GetData();

	buf << "MetaUnknown Event ";
	if (t == MetaUnknownEvent::WC_META_TYPE)
		buf << "*";
	else
		buf << t;
	buf << " ";
	if (d == MetaUnknownEvent::WC_DATA)
		buf << "*";
	else {
		buf << "{";
		Tclm_PrintData(buf, d, e->GetLength());
		buf << "}";
	}
	buf << ends;
	return (buf.str());
}

//// from tclmutil.cxx  MWM

/*==============================================================================*/

void Tclm_PrintData(ostream &buf, const unsigned char *data, long length)
{
        long i;

        buf.setf(ios::showbase | ios::internal);
        buf << hex << setw(4) << setfill('0') << (int)data[0];
        for (i = 1; i < length; i++)
                buf << " " << hex << setw(4) << setfill('0') << (int)data[i];
}

/*==============================================================================*/

