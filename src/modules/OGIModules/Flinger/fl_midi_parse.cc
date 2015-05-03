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
 Modified by DJLB to 
	(a) correct mid-file tempo event problem; 
	(b) allow a tolerance on lyric/note timing; 
	(c) get volume control working;
	(d) add the gliss operation;
	(e) handle multiple track MIDIs properly.
 
 To find these modifications, search for 'DJLB'.
*/

// modified from original distrib   Print-->Parse


extern "C" 
{
#include "MIDI/tcl.h"
}
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "MIDI/AllEvent.h"
#include "fl_midi_parse.h"
#include "fl_midi_ext.h"
#include "fl_io.h"						// Flinger_get_debug_file_flag

//DJLB addition
#include "fl_midi_print.h"
bool boPrintMIDIEvents;
/*
DJLB additions to ease note/lyric time relationship.
*/
#define True	1
#define False	0
	bool boNoteAvailable = False;
	bool boLyricAvailable = False;
#include "EST_String.h"

//DJLB 09/06/2008
extern void FL_store_parameter_event(ParameterEvent *pEvent);


//DJLB 03-05-06. All stored events must be added to utt with their times & durations
// set according to the tempo at the their tick time.
StoredLyric	clStoredLyric = StoredLyric();
StoredNote  clStoredNote = StoredNote();

StoredLyric::StoredLyric() 
{
}

StoredLyric::~StoredLyric()
{
}

StoredNote::StoredNote() 
{
}

StoredNote::~StoredNote()
{
}

/*==============================================================================*/

void Tclm_ParseEvent(Event *e, bool boCommonEvent)//DJLB modification
{
	boPrintMIDIEvents = true;//DJLB addition

	int i;
	i=e->GetType();
	switch (i)//(e->GetType()) 
	{
	case NOTEOFF: //FL
		if (((NoteEvent *)e)->GetNotePair() != 0) 
			return;
		Tclm_ParseNoteOff((NoteOffEvent *)e);
		break;
	case NOTEON: //FL
		if (((NoteEvent *)e)->GetNotePair() == 0)
			Tclm_ParseNoteOn((NoteOnEvent *)e);
		else 
		{
			if ((int)((NoteEvent *)e)->GetVelocity() == 0) 
				return;
			Tclm_ParseNote((NoteOnEvent *)e);
			boPrintMIDIEvents = false; //DJLB addition. Tclm_ParseNote() handles printing.
		}
		break;
	case KEYPRESSURE: /// TO-DO - vocal effort
		Tclm_ParseKeyPressure((KeyPressureEvent *)e);
		break;
	case PARAMETER:
		Tclm_ParseParameter((ParameterEvent *)e);
        boPrintMIDIEvents = false;//DJLB 09/06/2008. 
		break;
	case PROGRAM:
		Tclm_ParseProgram((ProgramEvent *)e);
		break;
	case CHANNELPRESSURE:
		Tclm_ParseChannelPressure((ChannelPressureEvent *)e);
		break;
	case PITCHWHEEL:
		Tclm_ParsePitchWheel((PitchWheelEvent *)e);
		break;
	case SYSTEMEXCLUSIVE:
		Tclm_ParseSystemExclusive((SystemExclusiveEvent *)e);
		break;
	case METASEQUENCENUMBER:
		Tclm_ParseMetaSequenceNumber((MetaSequenceNumberEvent *)e);
		break;
	case METATEXT:
		Tclm_ParseMetaText((MetaTextEvent *)e);
		break;
	case METACOPYRIGHT:
		Tclm_ParseMetaCopyright((MetaCopyrightEvent *)e);
		break;
	case METASEQUENCENAME:
		Tclm_ParseMetaSequenceName((MetaSequenceNameEvent *)e);
		break;
	case METAINSTRUMENTNAME:
		Tclm_ParseMetaInstrumentName((MetaInstrumentNameEvent *)e);
		break;
	case METALYRIC: //FL
		Tclm_ParseMetaLyric((MetaLyricEvent *)e);
		boPrintMIDIEvents = false; //DJLB addition. Tclm_ParseMetaLyric() handles printing. 
		break;
	case METAMARKER:
		Tclm_ParseMetaMarker((MetaMarkerEvent *)e);
		break;
	case METACUE:
		Tclm_ParseMetaCue((MetaCueEvent *)e);
		break;
	case METACHANNELPREFIX:
		Tclm_ParseMetaChannelPrefix((MetaChannelPrefixEvent *)e);
		break;
	case METAPORTNUMBER:
		Tclm_ParseMetaPortNumber((MetaPortNumberEvent *)e);
		break;
	case METAENDOFTRACK:
		Tclm_ParseMetaEndOfTrack((MetaEndOfTrackEvent *)e);
        boPrintMIDIEvents = false;
		break;
	case METATEMPO: //FL
		Tclm_ParseMetaTempo((MetaTempoEvent *)e);
		break;
	case METASMPTE:
		Tclm_ParseMetaSMPTE((MetaSMPTEEvent *)e);
		break;
	case METATIME:
		Tclm_ParseMetaTime((MetaTimeEvent *)e);
		break;
	case METAKEY: //FL
        /*DJLB 16-07-2008. The METAKEY event is useless to Flinger. 
            The notes specified by the note events in the MIDI file
            have been set by the MIDI file builder according to the
            last METAKEY event before the note time. */
		//Tclm_ParseMetaKey((MetaKeyEvent *)e);
        boPrintMIDIEvents = false;
		break;
	case METASEQUENCERSPECIFIC:
		Tclm_ParseMetaSequencerSpecific(
		    (MetaSequencerSpecificEvent *)e);
		break;
	case METAUNKNOWN:
		Tclm_ParseMetaUnknown((MetaUnknownEvent *)e);
		break;
	default:
	  cerr << "fl_midi_parse: unrecognized MIDI event\n";
	  break;
	}
//DJLB addition
	if (boPrintMIDIEvents)
			Tclm_PrintEvent(cout, e, boCommonEvent);
//End DJLB addition
}

/*==============================================================================*/

void Tclm_ParseNoteOff(NoteOffEvent *e)
{	
	int c, p, v;

	c = e->GetChannel();
	p = e->GetPitch();
	v = e->GetVelocity();
	cerr << "NoteOff without NoteOn?";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseNoteOn(NoteOnEvent *e)
{	
	int c, p, v;
	c = e->GetChannel();
	p = e->GetPitch();
	v = e->GetVelocity();
	cerr << "NoteOn without NoteOff?";
	//cerr << "NoteOn ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseNote(NoteOnEvent *e)
{
/*
DJLB addition to ease note/lyric time relationship sensitivity.
*/
bool boLyricAssigned = false;

	if(boLyricAvailable)
	{	// Add stored lyric at this note's time (in MIDI ticks).
        
		if (clStoredLyric.GetTime() != e->GetTime())
		{
			if (Flinger_get_debug_file_flag()) {
				if (Flinger_get_debug_file_flag()) {
					cout << "  Correcting time of lyric {" 
					<< clStoredLyric.GetString() << "} from "
						<< clStoredLyric.GetTime() 
						<< " to " << e->GetTime() << " MIDI ticks." << endl;
				}
			}
		}
		FL_add_lyric_token(e->GetTime(), clStoredLyric.GetString(),
                                                            clStoredLyric.boType);
		//FL_add_lyric_token(clStoredLyric.GetTime(), clStoredLyric.GetString(),
        //                                                    clStoredLyric.boType);
		boLyricAssigned = true;
		boLyricAvailable = False;

		// Add this note.
		FL_add_note( e->GetTime(), 
						e->GetChannel(), 
							e->GetPitch(),  
								e->GetDuration(), 
									e->GetVelocity(),
                                        e->GetChordRootScaleTone(),
                                            e->GetChordName(),
										        boLyricAssigned);
		boNoteAvailable = False;
	}
	else
	{
		ParseAnyStoredNote();
		//Store this note.
		clStoredNote.SetTime(e->GetTime());
		clStoredNote.SetChannel(e->GetChannel());
		clStoredNote.SetPitch(e->GetPitch()); 
		clStoredNote.SetDuration(e->GetDuration());
		clStoredNote.SetVelocity(e->GetVelocity());
        clStoredNote.SetChordRootScaleTone(e->GetChordRootScaleTone());
        clStoredNote.SetChordName(e->GetChordName());
		boNoteAvailable = True;
	}

//End DJLB modification
	return;
}

/*==============================================================================*/
// DJLB common lyrics mod - additional function.
void ParseAnyStoredNote()
{
	if(boNoteAvailable)
	{	//Add stored note at its own time.
        int i;
        i = clStoredNote.GetVelocity();
		FL_add_note( clStoredNote.GetTime(), 
						  clStoredNote.GetChannel(),
							clStoredNote.GetPitch(),
							  clStoredNote.GetDuration(),
							      clStoredNote.GetVelocity(),
                                        clStoredNote.GetChordRootScaleTone(), 
                                            clStoredNote.GetChordName(),
											    false );
	}
}

/*==============================================================================*/

void Tclm_ParseMetaLyric(MetaLyricEvent *e)
{
/*
DJLB addition to ease note/lyric time relationship sensitivity.
*/
bool boLyricAssigned = false;

	if(boNoteAvailable)	
	{	// Determine a suitable tolerance within which to act.
		int inTimeTolerance;
		if( clStoredNote.GetDuration() > 0 )
			inTimeTolerance = clStoredNote.GetDuration()/2;
		else
			inTimeTolerance = LYRIC_TO_NOTE_DEFAULT_TIME_TOLERANCE;

		if( (e->GetTime() - clStoredNote.GetTime()) >= (unsigned long)inTimeTolerance)
		{	// Store this lyric.
			clStoredLyric.SetString(e->GetString());
			clStoredLyric.SetTime(e->GetTime());
			clStoredLyric.boType = false;//track-specific lyric.
			boLyricAvailable = True;
		}
		else
		{	// Add this lyric at the stored note's time.
			if (boPrintMIDIEvents && (e->GetTime() != clStoredNote.GetTime()) )
			{
				if (Flinger_get_debug_file_flag()) {
					if (Flinger_get_debug_file_flag()) {
						cout << endl << "Correcting time of lyric {" 
						<< e->GetString() << "} from " << e->GetTime() << " to "
						<< clStoredNote.GetTime() << " MIDI ticks." << endl;
					}
				}
			}

			e->SetTime(clStoredNote.GetTime());
			FL_add_lyric_token(e->GetTime(), e->GetString(), false);
			boLyricAssigned = true;
		}
		// Add stored note at its own time.
		FL_add_note(clStoredNote.GetTime(), 
					    clStoredNote.GetChannel(),
						  clStoredNote.GetPitch(), 
						    clStoredNote.GetDuration(),
								clStoredNote.GetVelocity(),
                                    clStoredNote.GetChordRootScaleTone(),
                                        clStoredNote.GetChordName(),
								            boLyricAssigned);
		boNoteAvailable = False;
	}
	else
	{	//If a lyric is already stored, discard it.
		if(boLyricAvailable)
		{ //Discard this lyric.
			if (Flinger_get_debug_file_flag()) {
				if (Flinger_get_debug_file_flag()) {
					cout << endl << "Discarding lyric {" << e->GetString() << "} timed for "
						<< e->GetTime() << " MIDI ticks." 
						<< " No note is available for it." << endl;
				}
			}
		}
		else
		{
			// Store this lyric.
			clStoredLyric.SetString(e->GetString());
			clStoredLyric.SetTime(e->GetTime());
			clStoredLyric.boType = false;//track-specific lyric.
			boLyricAvailable = True;
		}
	}

//End DJLB modification	return;
}

/*==============================================================================*/

void Tclm_ParseParameter(ParameterEvent *e)
{
    //DJLB 09/06/2008
    FL_store_parameter_event(e);
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaTempo(MetaTempoEvent *e)
{
//DJLB modification
/* 
DJLB Additions for tempo change placement correction.
Store the tempo event in the tempo event list.
*/
tstTempoEvent stThisEvent;

	stThisEvent.usTempo = e->GetTempo();
	stThisEvent.ulTime = e->GetTime();

	if( stThisEvent.ulTime == 0 )
	{
		FL_update_tempo(stThisEvent.ulTime, stThisEvent.usTempo);
	}
	else
	{
		if (Flinger_get_debug_file_flag()) {

			cout << "Storing " << stThisEvent.usTempo 
							<< " bpm. Tempo event timed for " 
									<< stThisEvent.ulTime << " MIDI ticks." << endl;
		}
		boPrintMIDIEvents = false;
		// Add this event to the head of the list.
		AddAtListHead(stThisEvent, plstTempoEvents);
        boPrintMIDIEvents = false;
	}
//End DJLB modification
	return;
}

/*==============================================================================*/

//DJLB modification
/* 
DJLB Additions for Time signature change placement correction.
These do not effect Flinger, but do change the MIDI tick-to-bar/beat conversion.
Store the metatime event in the TimeSig event list.
*/

void Tclm_ParseMetaTime(MetaTimeEvent *e)
{	
tstTimeSigEvent stThisEvent;

	stThisEvent.ulTime = e->GetTime();
	stThisEvent.inNumerator = e->GetNumerator();
	stThisEvent.inDenominator = e->GetDenominator();
	if( stThisEvent.ulTime == 0 )
	{
		fvdUpdateTimeSig(stThisEvent.ulTime, stThisEvent.inNumerator, 
											stThisEvent.inDenominator );
	}
	else
	{
		if (boPrintMIDIEvents)
		{
			if (Flinger_get_debug_file_flag()) {
				cout << "Storing " << stThisEvent.inNumerator << "/" 
							<< stThisEvent.inDenominator << " Time Signature event timed for " 
								<< stThisEvent.ulTime << " MIDI ticks." << endl;
			}
		}
		// Add this event to the head of the list.
		AddAtListHead(stThisEvent, plstTimeSigEvents);
        boPrintMIDIEvents = false;
	}
	return;
}
//End DJLB modification

/*==============================================================================*/

void Tclm_ParsePitchWheel(PitchWheelEvent *e)
{
	//cerr << "PitchWheel ";
	FL_add_pitchbend(
		    e->GetTime(),
		    e->GetChannel(),
		    e->GetValue()
		    );
	return;
}

/*==============================================================================*/

///////////// below this not used yet ///////////////

void Tclm_ParseKeyPressure(KeyPressureEvent *e)
{
	
	int c, p;

	c = e->GetChannel();
	p = e->GetPitch();
	//cerr << "KeyPressure ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseProgram(ProgramEvent *e)
{	
	int c, v;

	c = e->GetChannel();
	v = e->GetValue();
	//cerr << "Program ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseChannelPressure(ChannelPressureEvent *e)
{	
	int c, p;

	c = e->GetChannel();
	p = e->GetPressure();

	//cerr << "ChannelPressure ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseSystemExclusive(SystemExclusiveEvent *e)
{	
	const unsigned char *d;
	long l;

	d = e->GetData();
	l = e->GetLength();

	//cerr << "SystemExclusive ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaSequenceNumber(MetaSequenceNumberEvent *e)
{	
	long l;

	l = e->GetNumber();

	//cerr << "MetaSequenceNumber ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaText(MetaTextEvent *e)
{	
	const char *s;

	s = e->GetString();

	//cerr << "MetaText ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaCopyright(MetaCopyrightEvent *e)
{	
	const char *s;

	s = e->GetString();
	//cerr << "MetaCopyright ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaSequenceName(MetaSequenceNameEvent *e)
{	
	const char *s;

	s = e->GetString();
	//cerr << "MetaSequenceName ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaInstrumentName(MetaInstrumentNameEvent *e)
{	
	const char *s;

	s = e->GetString();
	//cerr << "MetaInstrumentName ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaMarker(MetaMarkerEvent *e)
{	
	const char *s;

	s = e->GetString();
	//cerr << "MetaMarker ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaCue(MetaCueEvent *e)
{	
	const char *s;

	s = e->GetString();
	//cerr << "MetaCue ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaChannelPrefix(MetaChannelPrefixEvent *e)
{	
	const unsigned char *d;

	d = e->GetData();
	//cerr << "MetaChannelPrefix ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaPortNumber(MetaPortNumberEvent *e)
{	
	int p;

	p = e->GetPort();
	//cerr << "MetaPortNumber ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaEndOfTrack(MetaEndOfTrackEvent *e)
{	
	MetaEndOfTrackEvent *dummy;

	// shut up a warning
	dummy = e;
	if (Flinger_get_debug_file_flag()) {
		cout << endl << "End of Track detected." << endl << endl;
	}
	//cerr << "MetaEndOfTrack" << ends;
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaSMPTE(MetaSMPTEEvent *e)
{	
	int h, m, s, f, ff;

	h = e->GetHour();
	m = e->GetMinute();
	s = e->GetSecond();
	f = e->GetFrame();
	ff = e->GetFractionalFrame();
	//cerr << "MetaSMPTE ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaKey(MetaKeyEvent *e)
{
  //	cerr << "MetaKey ";
  //	cerr << "{" << e->GetKeyStr() << "}";
	//cerr << e->GetModeStr();
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaSequencerSpecific(MetaSequencerSpecificEvent *e)
{	
	const unsigned char *d;

	d = e->GetData();

	//cerr << "MetaSequencerSpecific ";
	
	return;
}

/*==============================================================================*/

void Tclm_ParseMetaUnknown(MetaUnknownEvent *e)
{	
	int t;
	const unsigned char *d;

	t = e->GetMetaType();
	d = e->GetData();

	//cerr << "MetaUnknown ";
	
	return;
}

/*==============================================================================*/
//// from tclmutil.cxx  MWM
void Tclm_ParseData(const unsigned char *data, long length)
{
    long i;
	for (i = 1; i < length; i++)
	  cerr << " " << (int)data[i];
	
	//buf.setf(ios::showbase | ios::internal);
        //buf << hex << setw(4) << setfill('0') << (int)data[0];
        //for (i = 1; i < length; i++)
	//       buf << " " << hex << setw(4) << setfill('0') << (int)data[i];
}

/*==============================================================================*/
