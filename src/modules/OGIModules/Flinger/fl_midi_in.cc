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
        Modified by DJLB to correct mid-file tempo event problem,
        to get pitchbend, volume control working, and to add the gliss operation.
        For these modifications, search for 'DJLB'.
 */
#include <stdlib.h>
#include "festival.h"
#include "MIDI/Song.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "fl_midi_parse.h"
#include "fl_midi_print.h"
#include "fl_midi_ext.h"
#include "fl_io.h"
#include "fl_globals.h" //DJLB 10-08-08
// DJLB addition
#include "MIDI/EvntUtil.h"
//End DJLB addition
//DJLB 14/03/06
#include "fl_pitch.h"
#include "fl_midi_in.h" //DJLB 16-05-06

static EST_String NoteName[12] = {"C", "C#", "D", "D#", "E", "F",
	                          "F#","G", "G#", "A", "A#", "B"};

EST_Utterance *Utt;  // this avoids including all the EST stuff in the MIDI lib

#ifndef O_BINARY
#define O_BINARY        0       /* Flag to my_open for binary files */
#endif
#ifndef O_RDONLY
#define O_RDONLY        0
#endif

void update_tempo(unsigned long time, int bpm);

//DJLB 20-06-2008 Just Tuning mod.
extern void fvdSetJustTuneData(Song *pmf, int inTrack, int inChannel );
void MIDIpitch_to_freq_and_name(int inChordRootNoteScale, int note_num, float &freq, EST_String &name);
//void MIDIpitch_to_freq_and_name(int note_num, float &freq, EST_String &name);
// The 'Just' scale.
static EST_String esUseJustTuning;
static double ScaleRatio[12] = {1.0, 1.066667, 1.125, 1.2, 1.25, 1.33333,
	                        1.4, 1.5, 1.6, 1.66667, 1.77778, 1.875};
static double dfC0 = 8.25; //Hz. This makes A5 in the key of C = 440 Hz.
FILE *fPitchDetails = NULL;
// End Just Tuning mod.

static EST_Utterance *get_current_utt(void);
static void set_current_utt(EST_Utterance *u);
// DJLB addition/alteration
// These variables apply to DJLB additions.
// Originally defined as a short int., BaseFreq needs more precision.
static double BaseFreq[12] = {8.17575, 8.662, 9.177, 9.72275, 10.30075, 10.9135,
	                      11.56225, 12.24975, 12.97825, 13.75, 14.5675, 15.43375};
static unsigned long sulCurrentTempo = 120;  // NOTE: CROCHETS per minute.
static unsigned long sulPreviousTempo = 120;
static unsigned long sulLastTempoEventTicks = 0;
static float ssfLastTempoEventSeconds = 0.0;
int Division = 120;  // NOTE: ticks per CROCHET (quarter note).
// NumTracks & Format should only be defined by reading the MIDI file.
static int NumTracks;  // Number of tracks in MIDI file
static int Format;  // MIDI format code
static EST_String esLastLyric("");
extern bool boNoteAvailable;

// YK 2011-07-25 move this to here so front proc has access to it
static unsigned long sulPreviousOffTime = 0;

//DJLB 17-05-06 Common lyrics mod.
Event **plstpCommonEvents = NULL;
EST_String fesStripSpaces( const char *chString);
bool fboStringComp(const char * chOne, const char * chTwo);
static int uinVolumeScalebase;
/*
   DJLB additions to fix mid-song tempo changes problem.
 */
extern bool boPrintMIDIEvents;

tstTempoEvent *plstTempoEvents = NULL;
bool fboCheckTempoEventList( unsigned long ulNoteTime );
float fsfDuration(unsigned long ulNoteOnTicks, unsigned long ulDurationTicks);

//DJLB 09/06/2008.
ParameterEvent **plstpParameterEvents = NULL;
//DJLB 19/06/2008. Channel identification mod.
string esTrackName[16];
/*==============================================================================*/

//
// parse midi data and put into stream items of utterance
//

LISP midifile_to_utt(EST_String filename, LISP utt)
{
	int fd;
	short track;
	Event *pEvent;
	Song mf;

	EST_Utterance *u = get_c_utt(utt);
	set_current_utt(u);  // keep ptr to current utt as static global

	//DJLB 5-06-06. Initialise global statics for each instance of this function.
	sulCurrentTempo = 120;
	Division = 120;
	esLastLyric = "";
	boNoteAvailable = false;
	fvdResetTimeSignatureStores();

	// YK 2011-07-25  Re-init this so that replaying a file won't generate an
	// overlapping note error.
	sulPreviousOffTime = 0;

	cout << "1" << endl;
	/* DJLB 13/09/03
	   The MIDI file must be opened in binary format, otherwise some data values
	   will be interpreted as EOF etc.
	 */
	if ((fd = open (filename.str(), O_RDONLY | O_BINARY)) == -1)
	{
		cout << endl << "Could not open midifile " << filename  << endl;
		EST_error("...");
	}
	cout << "2" << endl;
	//DJLB modification to catch file read errors.
	// A read error here should be caught.
	if( mf.SMFRead(fd) != 1) //GOOD )  // read file
	{
		cout << endl << "Could not read input MIDI file '" << filename  << "'" << endl;
		exit(1);
	}
	cout << "3" << endl;
	// End DJLB modification

	//DJLB 5-06-06 2 statements moved here from fl_midi_parse.cpp.
	plstTempoEvents = MakeList(tstTempoEvent);
	plstTimeSigEvents = MakeList(tstTimeSigEvent);

	//DJLB 09/06/2008
	plstpParameterEvents = MakeList(ParameterEvent *);

	EST_Features *gl = get_Flinger_params(); //DJLB 13-05-06
	//bool boIsNumber;
	//DJLB 10-08-08. Provision of alternative track identification, by name or number.
	EST_String esReqdTrackName(gl->S("voice_track_name")); //DJLB 10-08-08
	int inReqdTrackNumber(gl->I("voice_track_number")); //DJLB 10-08-08
	EST_String esLocation;
	//If the track name is nominated, use it. If not, use the track number.
	// If neither are given, the 1st track on which notes are found will be used.

	cout << "4" << endl;
	if(esReqdTrackName.length() <= 0)
	{
		if(inReqdTrackNumber != DEF_TRACKNUMBER)
		{
			if (inReqdTrackNumber <= 0)
			{
				inReqdTrackNumber = DEF_TRACKNUMBER;
			}
			else
			{
				char chNum[18];
				esLocation = "Track ";
				//esLocation += itoa(inReqdTrackNumber,chNum,10);;
				sprintf(chNum, "%d", inReqdTrackNumber);
				esLocation += chNum;
			}
		}
	}
	else
	{
		inReqdTrackNumber = DEF_TRACKNUMBER;
		esLocation = esReqdTrackName + " Track";
	}

	cout << "5" << endl;
	/*NOTE: MIDI channel numbers are known as integers from 1 to 16, but
	        are stored in the MIDI file as integers 0-15.*/
	int inReqdChannelNumber(gl->I("channel_number") - 1);
	if (!((inReqdChannelNumber < 16) && (inReqdChannelNumber >= 0))) // YK 2011-07-25 chgd "> 0" to ">= 0"
	{
		inReqdChannelNumber = DEF_CHANNELNUMBER;
	}
	else
	{
		char chNum[18];
		esLocation += " Channel ";
		//esLocation += itoa(inReqdChannelNumber + 1,chNum,10);
		sprintf(chNum, "%d", inReqdChannelNumber + 1);
		esLocation += chNum;
	}

	EST_String esCommonEventsTrackName(gl->S("common_events_track_name"));
	int inCommonEventsTrackNumber(gl->I("common_events_track_number"));
	//DJLB 19-06-08. Channel selection modification & recognition of reqd. track number.

	cout << "6" << endl;
	if(esCommonEventsTrackName.length() <= 0)
	{
		if(inCommonEventsTrackNumber != DEF_COMMONEVENTSTRACKNUMBER)
		{
			if (!((inCommonEventsTrackNumber < 16) && (inCommonEventsTrackNumber >= 0))) // YK 2011-07-25 chgd "> 0" to ">= 0"
			{
				inCommonEventsTrackNumber = DEF_COMMONEVENTSTRACKNUMBER;
			}
			else
			{
				char chNum[18];
				esLocation = "Track ";
				//esLocation += itoa(inCommonEventsTrackNumber,chNum,10);
				sprintf(chNum, "%d", inCommonEventsTrackNumber);
				esLocation += chNum;
			}
		}
	}
	else
	{
		inCommonEventsTrackNumber = DEF_COMMONEVENTSTRACKNUMBER;
		esLocation = esCommonEventsTrackName + " Track";
	}

	cout << "7" << endl;
	//DJLB. 20-06-2008 Just Tuning mod.
	esUseJustTuning = gl->S("use_just_tuning");
	uinVolumeScalebase = gl->I("volume_scalebase");

	if(esUseJustTuning == "yes")
	{   //Set up a file for note information. This will be added to as notes are processed.
		EST_String esPitchfile(filename.before(".") + " Pitch Details " + esLocation);
		esPitchfile += ".txt";
		fPitchDetails = fopen(esPitchfile,"w");
		//Put a heading on the file.
		if(fPitchDetails != NULL)
		{
			fprintf(fPitchDetails,"      Just Tuning Details for %s in Song file %s.\n\n"
			        "          Lyric      Note    Chord    Frequency     On Time     Duration\n"
			        "                                          (Hz)       (sec.)       (sec.)\n\n",
			        esLocation.str(), filename.str());
		}
	}
	//End Just Tuning mod.

	// Track 0 configuration info - store as static globals
	Format    = mf.GetFormat();
	NumTracks = mf.GetNumTracks();
	Division  = mf.GetDivision();

	cout << "8" << endl;
	// while there are new MIDI events, read the event,
	//  and add it to the proper relation - uses functions below
	bool found_notes = FALSE;
//DJLB addition
/*
   Flinger will handle MIDI files in Format 0 or 1 only.
        In Format 0, there is only one track (0) and all events are in it in their
        chronological order.
        In Format 1, a number of tracks may be present. Track 0 is commonly unnamed,
                and contains file descriptors and some MIDI events - notably tempo events,
                with info. about their intended time location.
        Tracks 1 and up may contain notes and/or lyrics, volume changes etc. Each track
                may be named, the track name being provided by the METASEQUENCENAME event.
        The following code allows Flinger to:-
        (a) reject a file of Format > 1;
        (b) process the track in a Format 0 file;
        (c) process the tracks in a Format 1 file with the following provisos:-
                (1) events on track 0 are stored and added to utt when appropriate, during
                        the later processing of a track that contains notes;
                (2) if a track name has not been provided by the user (this can be done
                        through Flinger parameters) all tracks up to and including the next
                        track to contain note events are processed. Tracks following one with
                        note events are ignored;
                (3) if a track name has been provided, only the track so named is
                        processed. All others are ignored, and
                (4) if a track name for a track containing events intended to be common
                        to all note-bearing tracks has been provided, events on that track
                        are stored in a list, and merged with the note-bearing track when
                        that track's events are handled.
 */

	if(Format > 1)
	{
		cout << endl << "MIDI file '" << filename
		     << "' is not Format 0 or 1. Terminating." << endl;
		cerr << endl << "MIDI file '" << filename
		     << "' is not Format 0 or 1. Terminating." << endl;
		close(fd);
		EST_error("...");
	}

	cout << "9" << endl;
	if (Flinger_get_debug_file_flag()) {
		cout << endl
		     << "LIST OF EVENTS FROM THE MIDI FILE '" << filename
		     << "' - one per line." << endl
		     << "  Numbers that start a line give timing in MIDI ticks." << endl;
	}

	/* Common events modification. Provides for lyric, parameter, pitchwheel and
	                channel pressure events on a 'common' track, to be merged with the
	                nominated note-carrying track. */
	if( NumTracks > 0)
		cout << "NumTracks " << NumTracks << endl;
	{         // Find out if there is a track of common events and collect track names.
		for (track=1; track<NumTracks; track++)
		{

			cout << "track " << track << endl;
			esTrackName[track] = "";
			if(track == inCommonEventsTrackNumber)
			{
				if (Flinger_get_debug_file_flag()) {
					cout << endl << "Processing Track " << track << ": (Common Events track)" << endl;
				}
			}
			for (pEvent=mf.NextEvent(track); pEvent!=0;
			     pEvent=mf.NextEvent(track))
			{
				cout << "accessing " <<flush;
				cout << "event time " << pEvent->GetTime() << " type " << pEvent->GetType()  << endl;
				if(pEvent->GetType() == METASEQUENCENAME)
				{           // Collect track names for later use.
					cout << "inside if " << pEvent->GetType()  << endl;
					esTrackName[track] = ((MetaSequenceNameEvent *)pEvent)->GetString();
					if(inCommonEventsTrackNumber == DEF_COMMONEVENTSTRACKNUMBER)
					{ //Seek a common events track via its name.
						cout << "fboStringComp " << pEvent->GetType()  << endl;
						if(fboStringComp( esTrackName[track].c_str(), esCommonEventsTrackName))
						{       // This is it.
							inCommonEventsTrackNumber = track;
							if (Flinger_get_debug_file_flag()) {
								cout << endl << "Processing " << track
								     << ": (Common Events Track, named "
								     << esCommonEventsTrackName << ")" << endl;
							}
						}
						else
							break;     // This is not it. Go to the next track.
					}
				}
				if(track == inCommonEventsTrackNumber)
				{
					if(plstpCommonEvents == NULL) {
						// Create a list in which the events can be stored.
						cout << "make list " << pEvent->GetType()  << endl;
						plstpCommonEvents = MakeList(Event *);
					}

					// Store appropriate common events in the common events list.
					if( (plstpCommonEvents != NULL) &&
					    ( (pEvent->GetType() == METALYRIC) ||
					      (pEvent->GetType() == PARAMETER) ||
					      (pEvent->GetType() == PITCHWHEEL) ||
					      (pEvent->GetType() == KEYPRESSURE) ||
					      (pEvent->GetType() == CHANNELPRESSURE)
					    ))
					{
						AddAtListHead(pEvent, plstpCommonEvents);
						cout << "before flag " << pEvent->GetType()  << endl;

						if (Flinger_get_debug_file_flag()) {
							cout << "after flag " << pEvent->GetType()  << endl;

							if(pEvent->GetType() == METALYRIC)
							{
								cout << "Stored common lyric {"
								     << ((MetaLyricEvent *)pEvent)->GetString()
								     << "} timed for " << pEvent->GetTime()
								     << " midi ticks." << endl;
							}
							else if(pEvent->GetType() == PARAMETER)
							{
								cout << "Stored common Parameter event No. "
								     << ((ParameterEvent *)pEvent)->GetParameter()
								     << " timed for " << pEvent->GetTime()
								     << " midi ticks." << endl;
							}
							else
							{
								cout << "Stored common " << pEvent->GetTypeStr()
								     << " timed for " << pEvent->GetTime()
								     << " midi ticks." << endl;
							}
						}

					} //if appropriate common event.
				}        //if this is common events track
			}        //for events in track
		}        //for tracks in song
	}        //if number of tracks in song > 0
	         // Rewind.
	cout << "101" << endl;
	mf.RewindEvents();
	/* End Common Events mod. */
	cout << "11" << endl;

// Now process the tempo etc. map track (0) and the nominated note-carrying track.
	bool boNoteTrackNotice = false;
	if (Flinger_get_debug_file_flag()) {
		cout << endl;
	}

	for (track=0; track<NumTracks; track++)
	{
//DJLB addition
		if(found_notes)
		{
			if (!boNoteTrackNotice)
			{
				if (Flinger_get_debug_file_flag()) {
					cerr << endl <<
					"One track with notes has been processed."
					     << " Any remaining tracks will be ignored." << endl;
				}
				boNoteTrackNotice = true;
				break;
			}
		}
//End DJLB addition
		if (!found_notes && (track != inCommonEventsTrackNumber))
		{
			/*DJLB changes.
			                Deal with Track 0 (in a Format 1 MIDI, this contains a tempo and time
			                    event map). Then deal only with the track and/or channel identified by
			    Flinger input parameters, if that identification exists. Otherwise,
			                deal with all tracks other than those following one with notes.
			 */
			if( (track == 0) ||
			    ( (esReqdTrackName != DEF_TRACKNAME) &&
			      ((esTrackName[track].c_str() == esReqdTrackName) || (track == inReqdTrackNumber)) ) ||
			    (esReqdTrackName == DEF_TRACKNAME) )
			{ //Process this track.

				if (Flinger_get_debug_file_flag()) {
					cout << endl << "Processing Track " << track << ":" << endl;
				}

				if((track != 0) && (esUseJustTuning == "yes"))
					fvdSetJustTuneData(&mf, track, inReqdChannelNumber);

				for (pEvent=mf.NextEvent(track); pEvent!=0;
				     pEvent=mf.NextEvent(track))
				{
					bool boUseEvent = true;
					if (inReqdChannelNumber >= 0)
					{ // Channel is specified. Use channel events that match it.
						if(  (pEvent->GetType() == NOTEOFF) ||
						     (pEvent->GetType() == NOTEON) ||
						     (pEvent->GetType() == PROGRAM) ||
						     (pEvent->GetType() == PARAMETER) ||
						     (pEvent->GetType() == PITCHWHEEL) ||
						     (pEvent->GetType() == KEYPRESSURE) ||
						     (pEvent->GetType() == CHANNELPRESSURE)  )
						{
							boUseEvent = (((NoteEvent*)(pEvent))->GetChannel() ==
							              inReqdChannelNumber);
						}
					}
					if (boUseEvent)
					{
						if (pEvent->GetType() == NOTEON)
							found_notes = TRUE;
						/*DJLB Common events mod. There may be a note left in store,
						    that is the last of a set of slurred notes, or with a matching
						    event (probably a lyric) in the common events list.*/
						if(pEvent->GetType() == METAENDOFTRACK)
							ParseAnyStoredNote();
						Tclm_ParseEvent(pEvent, track == 0);
					}
				} // for next event.
			} // if track 0 or required track.
			else
			{
				/*Don't process a track > 0 unless it matches the required track
				             by name or there is no required track name defined.*/
				if (Flinger_get_debug_file_flag()) {
					cout << "Skipping Track " <<  track << "." << endl;
				}
			} // if not track 0 or required track.
		} //if not common events track.
	} //for next track.

//DJLB addition
	DestroyList(plstTempoEvents);
	DestroyList(plstTimeSigEvents);
	DestroyList(plstpCommonEvents);
	DestroyList(plstpParameterEvents);
//End DJLB addition

	return utt;
}

/*==============================================================================*/

bool fboStringComp(const char * chOne, const char * chTwo)
{
	EST_String esOne(chOne);
	EST_String esTwo(chTwo);

	esOne = fesStripSpaces(chOne);
	esTwo = fesStripSpaces(chTwo);

	return (esOne == esTwo) ? true : false;
}

/*==============================================================================*/

EST_String fesStripSpaces( const char *chString)
{
	int inPosition;
	EST_String esString(chString);
	// Take out any leading spaces.
	while(esString.index(" ") == 0)
		esString = esString.after(" ");

	// Remove any trailing spaces.
	inPosition = esString.length() - 1;
	while(inPosition > 0)
	{
		if(esString.at(inPosition,1) == " ")
		{
			esString = esString.before(inPosition);
			inPosition -= 1;
		}
		else
			break;
	}

	return esString;
}

/*==============================================================================*/

void FL_store_parameter_event(ParameterEvent *pEvent)
{
	AddAtListHead(pEvent, plstpParameterEvents);
}

/*==============================================================================*/

void FL_add_parameter_event(ParameterEvent *e, EST_String esType)
{
	int p = e->GetParameter();
	if (Flinger_get_debug_file_flag()) {
		cout << endl << e->GetTime() << " " << fpchGetBarAndBeats(e->GetTime()).c_str()
		     << " Adding " << esType << " Parameter event, No. "
		     << p << ", value " << e->GetValue() << endl;
	}

	switch (p)
	{
	case 1:   // Modulation
		FL_add_vibrato( e->GetTime(), e->GetChannel(), e->GetValue());
		break;
	case 5:
		FL_add_portamento_time(e->GetTime(), e->GetChannel(), e->GetValue());
		break;
	case 7:   // Volume for track.
		FL_add_Volume(e->GetTime(), e->GetChannel(), e->GetValue(), FALSE);
		break;
	case 20:        // DJLB addition - Gliss
		FL_add_gliss(e->GetTime(), e->GetChannel());
		break;
	case 21:    // DJLB addition - 'notebend' - a pitchbend for current note only.
		FL_add_notebend(e->GetTime(), e->GetChannel(), (long)e->GetValue());
		break;  //YK 2011-07-25 added
	case 30:   // DJLB addition - Volume crescendo/diminuendo effect.
		FL_add_Volume(e->GetTime(), e->GetChannel(), e->GetValue(), TRUE);
		break;
	}
}

/*==============================================================================*/

void FL_add_lyric_token(unsigned long ulTicksFromStart, const char *lyrstr,
                        bool boCommonLyric )
{
//DJLB 03-05-06. Version for stored lyrics.
	// Add an item to the end of LyricToken relation
	// Also put in SylStructure relation and NoteStruct relation
	EST_Utterance *u = get_current_utt();
	EST_Item *p;
	EST_String lyrString(lyrstr);
	EST_String esType;
	if(boCommonLyric)
		esType = "common";
	else
		esType = "track";

	//Check against co-incident lyric tokens.
	static unsigned long ulLastTicksFromStart = -1;
	if(ulTicksFromStart == ulLastTicksFromStart)
	{
		if (Flinger_get_debug_file_flag()) {
			cout << endl << "ERROR: Two lyric tokens set for same note. Discarding {"
			     << lyrstr << "}." << endl;
		}
		return;
	}
	ulLastTicksFromStart = ulTicksFromStart;
	// Store the lyric for printout.
	esLastLyric = lyrstr;

	if (lyrString.contains(RXalphanum)) // skip non-alphanum
	{
		p = u->relation("LyricToken")->append();
		p->set("name", lyrString);
		p->set("time",MIDIticks_to_sec(ulTicksFromStart));
		//DJLB 10/06/2008. print output moved to hear so that any use of tempo events
		//  that affect this lyric's timing will appear before insertion of the lyric.
		if (Flinger_get_debug_file_flag()) {
			cout << endl << ulTicksFromStart << " "
			     << fpchGetBarAndBeats(ulTicksFromStart).c_str()
			     << " Added " << esType << " Lyric: {" << lyrstr << "}" << endl;
		}

		u->relation("SylStructure")->append(p);
		u->relation("NoteStruct")->append(p);
	}
}

/*==============================================================================*/
//DJLB 20-06-2008. Enharmonic mod. adds a parameter inChordRootScaleTone.
void FL_add_note(       unsigned long ulNoteOnTime, // time start in ticks
                        int inChannel,                                          // MIDI channel
                        int inPitch,                                                    // pitch in range [0,127]
                        unsigned long ulDuration,                       // duration in ticks
                        int inVelocity,                                         // amplitude
                        int inChordRootScaleTone, //used in enharmonic adjustements
                        string stChordName,
                        bool boLyricAssigned                            // prevents adding another lyric																						//		for this note.
                        )
{
	float flDuration, flOnTime;

	EST_Utterance *u = get_current_utt();

	//Check for overlapping notes.
	if (sulPreviousOffTime > ulNoteOnTime)
	{       // Last note timed to end after the start on this note.
		if( sulPreviousOffTime > ulNoteOnTime + ulDuration)
		{ // Last note timed to end after the end of this note.
			cerr << endl << "FATAL ERROR: Overlapping Notes at "
			     << fpchGetBarAndBeats(ulNoteOnTime).c_str() << "." << endl;
			EST_error("...");
		}
		else
		{ //Last note timed to end during this note.
			// Adjust this note's 'on' time and duration.
			if (Flinger_get_debug_file_flag()) {
				cout << endl << "ERROR: Overlapping Notes at "
				     << fpchGetBarAndBeats(ulNoteOnTime).c_str()
				     << ". Adjusting the 'On' time and duration "
				     << "of the 2nd note." << endl;
			}

			ulDuration = ulDuration - (sulPreviousOffTime - ulNoteOnTime);
			ulNoteOnTime = sulPreviousOffTime;
		}
	}
	//Store this note's off time.
	sulPreviousOffTime = ulNoteOnTime + ulDuration;
/*
   DJLB addition to handle mid-song time signature changes and common events.
 */
	Event *pe;
	ParameterEvent * pTPe;

CheckCommonEventsList:
	if(plstpCommonEvents != NULL)
	{
		if( GetFromListTail(&pe, plstpCommonEvents) !=NULL )
		{
			// Deal with a lyric event. This must be close to the note 'on' time,
			// and any time error must be corrected.
			if(pe->GetType() == METALYRIC )
			{       //DJLB Common lyrics mod.
				/* Common events are stored in the common list in chronological
				                order, with the earliest not yet used at the tail of the list.
				        Check the one at the tail, to see if it is for this note. If
				                it is, add it to utt before adding the note to utt, then
				                remove it from the list tail. This way, the one at the
				                tail is always the next to be used.
				 */
				unsigned long ulTimeTolerance;
				if(duration > 0)
					ulTimeTolerance = ulDuration/2;
				else
					ulTimeTolerance = LYRIC_TO_NOTE_DEFAULT_TIME_TOLERANCE;

				if(abs((int)(  ((MetaLyricEvent*)pe)->GetTime() - ulNoteOnTime))
				   < (long)ulTimeTolerance)
				{
					if(!boLyricAssigned)
					{
						FL_add_lyric_token(ulNoteOnTime,
						                   ((MetaLyricEvent*)pe)->GetString(), true);
					}
					else
					{ //Trying to add a common lyric when a track lyric has been assigned.
						if (Flinger_get_debug_file_flag()) {
							cout << endl
							     << "ERROR: Two lyric tokens set for the note at "
							     << fpchGetBarAndBeats(ulNoteOnTime).c_str()
							     << ". Discarding the lyric {"
							     << ((MetaLyricEvent*)pe)->GetString() << "}."
							     << endl;
						}
					}
					// Remove the tail of the list.
					DeleteFromTail(plstpCommonEvents);
					//Check for other common events.
					goto CheckCommonEventsList;
				}
			}
			// Deal with other common events. These may occur at any time, but
			// handle only those that are still in the list and are timed for
			// before or at the time of this note.
			else // DJLB 08/06/2008 Redundant: if(pe->GetType() != METALYRIC)
			{
				if( pe->GetTime() <= ulNoteOnTime)
				{
					/*DJLB 09/06/2008. Also deal with stored track parameter events.
					   These must be inserted in chronological order w.r.t. the current
					    common event.
					 */
					if( GetFromListTail(&pTPe, plstpParameterEvents) !=NULL )
					{
						while(pTPe->GetTime() < pe->GetTime())
						{
							FL_add_parameter_event(pTPe, "Track");
							DeleteFromTail(plstpParameterEvents);
							if (GetFromListTail(&pTPe, plstpParameterEvents) == NULL)
								break;
						};
					}
					FL_add_parameter_event((ParameterEvent *)pe, "Common");

					// Remove the tail of the common events list.
					DeleteFromTail(plstpCommonEvents);
					//Check for other common events.
					goto CheckCommonEventsList;
				}
			}
		}
	}
	/*DJLB 09/06/2008. Also deal with stored track parameter events.
	    These must be inserted in chronological order w.r.t. the current note.
	 */
	if( GetFromListTail(&pTPe, plstpParameterEvents) !=NULL )
	{
		while(pTPe->GetTime() <= ulNoteOnTime)
		{
			FL_add_parameter_event(pTPe, "Track");
			DeleteFromTail(plstpParameterEvents);
			if (GetFromListTail(&pTPe, plstpParameterEvents) == NULL)
				break;
		};
	}
	/*================ Now deal with the note. ======================*/


	// Add a stream item to the end of Note stream
	// assume a monophonic track
	EST_Item *ni;
	float sfFrequency, sfVolume; //DJLB added sfVolume.
	EST_String esNoteName;
	// DJLB adjustment to velocity. A volume_scalebase value of 10, for
	//	instance, provides a useful scale of 1-10. However, MIDI files can
	//   carry a value up to 0xFF (255) so this has to be allowed for.
	if(inVelocity > uinVolumeScalebase)
		inVelocity = uinVolumeScalebase;
	sfVolume = (float)inVelocity/(float)uinVolumeScalebase;

	ni = u->relation("Note")->append();

	//DJLB 20-06-2008. Just Tuning mod.
	MIDIpitch_to_freq_and_name(inChordRootScaleTone, inPitch,
	                           sfFrequency, esNoteName);
	//MIDIpitch_to_freq_and_name(pitch, freq, name);
	//End Just Tuning mod.

	// set features
	ni->set("name",esNoteName); // musical note name

	//DJLB addition
	ni->set("BarTime", fpchGetBarAndBeats(ulNoteOnTime).c_str());

	//Convert the note on-time ticks to seconds.
	flOnTime = MIDIticks_to_sec(ulNoteOnTime);

	//Convert the note duration ticks to seconds. This function also
	//	checks the tempo events list and activates any tempo event
	// set to occur during this note.
	flDuration = fsfDuration(ulNoteOnTime, ulDuration);
	//End DJLB addition

	ni->set("on", flOnTime);
	ni->set("dur", flDuration);
	ni->set("off", flOnTime + flDuration);
	ni->set("freq",sfFrequency);
	ni->set("volume", sfVolume);
	ni->set("chan", inChannel);
	//DJLB 10/06/2008. Moved print output to here to be comparable to lyric printout.
	if (Flinger_get_debug_file_flag()) {
		cout << ulNoteOnTime << " " << fpchGetBarAndBeats(ulNoteOnTime).c_str()
		     << " Added Note: " << esNoteName << " velocity: " << inVelocity
		     << " duration: " << flDuration << "." << endl;
	}

	//DJLB 22/06/2008. Output data to special data file if enharmonic is in use.
	if( fPitchDetails != NULL)
	{
		EST_String esChordTitle;
		if(inChordRootScaleTone >= 0)
			esChordTitle = NoteName[inChordRootScaleTone];
		else
			esChordTitle = "-";

		esChordTitle += stChordName.c_str();
		fprintf(fPitchDetails,"%15s  %8s %7s %12.3f %12.3f %12.3f\n",
		        esLastLyric.str(), esNoteName.str(), esChordTitle.str(),
		        sfFrequency, flOnTime, flDuration);
	}

	esLastLyric = "";
}

/*==============================================================================*/

void FL_add_pitchbend(  unsigned long ulTicksFromStart,   // time start in ticks
                        int channel,                                    // MIDI channel
                        unsigned long value                             // range [-8192,8192]
                        //	(after correction)
                        )
{ // Add item to the end of F0_Mod stream
	//   - assumes a monophonic track
	EST_Item *item;
	float sfValue;
	EST_Utterance *u = get_current_utt();
//EST_Features *gl = get_Flinger_params();

	// figure out bend value
	/* Max. value = 0x3FFF	must represent ~ +1
	        Min. value = 0x00	must represent -1
	        Mid-point value = 0x2000 must represent +/- 0.0 */
	sfValue = (float)( ((double)value - 0x2000)/(double)0x2000 );

	// set features
	item = u->relation("F0_Mod")->append();
	item->set("name", "pitchbend");
	item->set("size", sfValue);
	item->set("start", MIDIticks_to_sec(ulTicksFromStart));
	item->set("chan", channel);                                     // keep for future use?
	//DJLB 10/06/2008 Added to increase information output.
	if (Flinger_get_debug_file_flag()) {
		cout << ulTicksFromStart << " " << fpchGetBarAndBeats(ulTicksFromStart).c_str()
		     << " Added Pitchbend event, value " << value
		     << ", to bend by (" << sfValue << " * pitch effects scalebase) semitones." << endl;
	}
}

/*==============================================================================*/
//DJLB addition
void FL_add_notebend(   unsigned long ulTicksFromStart,   // time start in ticks
                        int channel,                                    // MIDI channel
                        unsigned long value                                                     // range
                        )
{ // Add item to the end of F0_Mod stream
	//   - assumes a monophonic track
	EST_Item *item;
	float sfValue;
	EST_Utterance *u = get_current_utt();
//EST_Features *gl = get_Flinger_params();

	// figure out bend value
	/* Max. value = 0x7F	must represent ~ +1
	        Min. value = 0x00	must represent -1
	        Mid-point value = 0x40 must represnt +/- 0.0 */
	sfValue = (float)( ((double)value - 0x40)/(double)0x40 );

	// set features
	item = u->relation("F0_Mod")->append();
	item->set("name", "notebend");
	item->set("size", sfValue);
	item->set("start", MIDIticks_to_sec(ulTicksFromStart));
	item->set("chan", channel);                                     // keep for future use?
	//DJLB 10/06/2008 Added to increase information output.
	if (Flinger_get_debug_file_flag()) {
		cout << ulTicksFromStart << " " << fpchGetBarAndBeats(ulTicksFromStart).c_str()
		     << " Added Notebend event, value " << value
		     << ", to bend by (" << sfValue
		     << " * pitch effects scalebase) semitones." << endl;
	}
}

/*==============================================================================*/

void FL_add_gliss(unsigned long ulTicksFromStart,               // time start in ticks
                  int channel )                                         // MIDI channel
{
// Add item to the end of F0_Mod stream
	//   - assumes a monophonic track
	EST_Item *item;
	EST_Utterance *u = get_current_utt();
//EST_Features *gl = get_Flinger_params();

	// set features
	item = u->relation("F0_Mod")->append();
	item->set("name", "gliss");
	item->set("start", MIDIticks_to_sec(ulTicksFromStart));
	item->set("chan", channel);                                     // keep for future use?
	//DJLB 10/06/2008 Added to increase information output.
	if (Flinger_get_debug_file_flag()) {
		cout << endl << ulTicksFromStart << " " <<
		fpchGetBarAndBeats(ulTicksFromStart).c_str() <<
		" Added Glissando event." << endl;
	}
}

/*==============================================================================*/

void FL_add_Volume(unsigned long ulTicksFromStart,    // time start in ticks
                   int inChannel,                              // MIDI channel.
                   unsigned long ulValue,                                     // Value set in event.
                   unsigned int boCrescendo)                                             // Start of a crescendo
// or Diminuendo.
{
	EST_Item *pesItem;
	EST_Utterance *u = get_current_utt();
	float sfValue;
	EST_String esType;
	// YK 2011-07-25 Removed screwy code. Midi cc events 7 and 30 have a range
	//               of 0-127. Instead, normalize it to 0.0 .. 1.0. Volume
	//               scalebase is now used exclusively by the just tuning
	//               feature. As it should be.
	sfValue = (float)ulValue / (float)127.0;

	////
	//// An adjustment to velocity. A velocity_scale value of 10, for instance,
	////	provides a useful scale of 1-10. However, MIDI files can carry a value
	////   up to 0xFF (255) so this has to be allowed for.
	//sfValue = (float)ulValue;
	//if(sfValue > uinVolumeScalebase)
	//	sfValue = (float)uinVolumeScalebase;
	//// Now convert sfValue to a fraction of its maximum value.
	//sfValue = sfValue/(float)uinVolumeScalebase;

	pesItem = u->relation("GainMod")->append();

	if( boCrescendo)
	{
		// YK 2011-07-25 Double sfValue, so that it's range is now 0.0 .. 2.0
		//               because when it's used to modify gain, a value of
		//				 1.0 means 'no modification'. If we want to crescendo
		//				 above the current volume, we need values above 1.0.
		//				 This now means midi input value bounds of 0=quiet,
		//               64=no change and 127=loud.
		sfValue *= 2.0;
		pesItem->set("name", "CrescMark");
		esType = "Crescendo";
	}
	else
	{
		// YK 2011-07-25 Hey, is this implemented? Can't find "VolumeMark"
		//               anywhere but here!
		pesItem->set("name", "VolumeMark");
		esType = "Volume";
	}

	pesItem->set("time", MIDIticks_to_sec(ulTicksFromStart));
	pesItem->set("volume", sfValue);
	pesItem->set("chan", inChannel); // keep for future use?
	//DJLB 10/06/2008 Added to increase information output.
	if (Flinger_get_debug_file_flag()) {
		cout << ulTicksFromStart << " " << fpchGetBarAndBeats(ulTicksFromStart).c_str()
		     << " Added " << esType << " event, value " << ulValue
		     << ", to set volume at " << sfValue << "." << endl;
	}
}

/*==============================================================================*/

void FL_add_vibrato(unsigned long ulTicksFromStart,   // time start in ticks
                    int channel,            // MIDI channel
                    unsigned long value     // range [0,255]
                    )
{ // Add item to the end of F0_Mod stream
	// assume a monophonic track
	EST_Item *item;
	float sfValue;
	EST_Utterance *u = get_current_utt();
	//EST_Features *gl = get_Flinger_params();

	// figure out vibrato value
	/* Max. value = 0x7F	must represent ~ +1
	        Min. value = 0x00	must represent 0.0 */
	sfValue = ((float)value)/0X7F;

	// set features
	item = u->relation("F0_Mod")->append();
	item->set("name", "vibrato");
	item->set("size",sfValue);         // in % of current freq
	item->set("start", MIDIticks_to_sec(ulTicksFromStart));
	item->set("chan", channel);           // keep for future use?
	//DJLB 10/06/2008 Added to increase information output.
	if (Flinger_get_debug_file_flag()) {
		cout << ulTicksFromStart << " " << fpchGetBarAndBeats(ulTicksFromStart).c_str()
		     << " Added Modwheel (Vibrato) event, value " << value
		     << ", to set variation at (" << sfValue
		     << " * pitch effects scalebase) semitones." << endl;
	}
}

/*==============================================================================*/

void FL_add_portamento_time(unsigned long ulTicksFromStart, int channel, long value )
{ // Add item to the end of F0_Mod stream
	//   - assumes a monophonic track
	EST_Item *item;
	EST_Utterance *u = get_current_utt();
//EST_Features *gl = get_Flinger_params();
	float sfPortamentoTime;
	// convert input value (milliseconds) to seconds.
	//DJLB 10/06/2008 Correction to scaling.
	sfPortamentoTime = (float)value/(float)1000.0; // 100.0;
	// set features
	item = u->relation("F0_Mod")->append();
	item->set("name", "portamento");
	item->set("start", MIDIticks_to_sec(ulTicksFromStart));
	item->set("size", sfPortamentoTime);
	item->set("chan", channel);                                     // keep for future use?
	//DJLB 10/06/2008 Added to increase information output.
	if (Flinger_get_debug_file_flag()) {
		cout << ulTicksFromStart << " " << fpchGetBarAndBeats(ulTicksFromStart).c_str()
		     << " Added Portamento event, value " << value << " millisecs." << endl;
	}
}
/*==============================================================================*/
// 22-10-2006. Tempo event handling modifications.
/*
   DJLB Change.
   The original code did not allow for mid-song tempo changes. These can
        be placed anywhere in the song time-span, regardless of note and lyric times.
        New functions (and re-coded old ones to handle this eventuality are:-
                fsfDuration(unsigned long ulNoteOnTicks, unsigned long ulDurationTicks)
                        Returns the duration (in sec.) of a note;

                fboCheckTempoEventList( unsigned long ulNoteOffTime )
                        Applies tempo events timed to occur within the current note, and
                        returns true if this action was taken, otherwise false;

                FL_update_tempo(unsigned long ulTicksFromStart, short bpm)
                        Updates static globals related to timing:-
                                sulCurrentTempo, sulPreviousTempo,
                                sulLastTempoEventTicks, ssfLastTempoEventSeconds;

                MIDIticks_to_sec(unsigned long ulTicksFromStart)
                        Returns the time (in sec.) from the start, represented by the ticks
                        from the start.
 */
float fsfDuration(unsigned long ulNoteOnTicks, unsigned long ulDurationTicks)
{
	unsigned long ulNoteOffTicks;
	float sfDeltaSeconds = 0.0;
	ulNoteOffTicks = ulNoteOnTicks + ulDurationTicks;
	// Calculate the duration (in sec.) at the current tempo.
	sfDeltaSeconds = (float)(ulDurationTicks * 60.0 / (float)(Division * sulCurrentTempo));

	//Check the tempo events list for any tempo event timed to occur
	// before the end of  this note.
	//There may be one or more tempo events listed for action during
	// the time of this note.
	while(fboCheckTempoEventList(ulNoteOffTicks))
	{
		if(sulLastTempoEventTicks < ulNoteOffTicks)
		{       //Last ticks anchor is before the end of this note.
			if( ulNoteOnTicks <= sulLastTempoEventTicks)
			{ //A tempo event has been set during this note.
				// The effect of multiple tempo events during the note accumulate.
				sfDeltaSeconds += (float)((ulNoteOffTicks - sulLastTempoEventTicks)
				                          * 60.0/(float)Division *
				                          (1.0/sulCurrentTempo - 1.0/sulPreviousTempo));
			}
			else
			{       //A tempo event has been set before this note.
				// The effect of multiple tempo events before the note do not
				// accumulate. The last before the note starts is the only one to
				// affect the note's duration. Recalculate the duration.
				sfDeltaSeconds = (float)(ulDurationTicks * 60.0/(float)(Division * sulCurrentTempo));
			}
		}
	};

	return sfDeltaSeconds;
}

/*==============================================================================*/

bool fboCheckTempoEventList( unsigned long ulTicks )
{

/*
   First, check the list of tempo events for one occurring before the end of this note.
   If there is one, it will be at the tail of the list, because events have been added
   to the head of the list in chronological order, and any that have been used have
   been removed from the tail.
   Thus any tempo event in the list that occurs before the end of this note is
   timed to occur DURING this note.
 */
	bool boTempoChanged = false;
	tstTempoEvent stTempoChange;
	if( GetFromListTail(&stTempoChange, plstTempoEvents) !=NULL )
	{       // The list is not empty. Get the structure at the tail of the list.
		if(stTempoChange.ulTime < ulTicks)
		{       // The tempo change event is set for an earlier time. Use it.
			if (Flinger_get_debug_file_flag()) {
				cout << stTempoChange.ulTime << " "
				     << fpchGetBarAndBeats(stTempoChange.ulTime).c_str()
				     << " Using Tempo Event. New tempo = "
				     << stTempoChange.usTempo << " Beats per Minute." << endl;
			}

			FL_update_tempo(stTempoChange.ulTime, stTempoChange.usTempo);
			boTempoChanged = true;
			// Remove the tail of the list.
			DeleteFromTail(plstTempoEvents);
		}
	}
	return boTempoChanged;
}

/*==============================================================================*/

void FL_update_tempo(unsigned long ulTicksFromStart, short bpm)
{ // It gets a little tricky if the tempo changes during the song
	//   - maintain an "anchor" from most recent tempo change
	//DJLB 22-10-2006. Redo of timing system.
	//Calculate the time (in seconds) of this tempo event, and store it.
	ssfLastTempoEventSeconds += (float)( (ulTicksFromStart - sulLastTempoEventTicks)/
	                                     (Division * sulCurrentTempo/60.0) );
	//Store the time (in ticks) of this tempo event.
	sulLastTempoEventTicks = ulTicksFromStart;
	//Store the current tempo as previous, and set the new current tempo.
	sulPreviousTempo = sulCurrentTempo;
	sulCurrentTempo = bpm;
}

/*==============================================================================*/

float MIDIticks_to_sec(unsigned long ulTicks)
{ // Division = clock ticks per beat (I don't think this can vary in MIDI)
	// Tempo = beats per minute (This can vary during the song)
	// re-write of old function.

	//DJLB 09/06/2008. Was in FL_add_lyric_token()
	//Check the tempo list, and adjust tempo, time anchors etc.
	while(fboCheckTempoEventList(ulTicks))
	{
	};

	float sfSecondsFromStart;
	if(ulTicks < sulLastTempoEventTicks)
	{ // Use the tempo data that applied before the current anchor.
		sfSecondsFromStart = (float)(ssfLastTempoEventSeconds +
		                             ((float)ulTicks - (float)sulLastTempoEventTicks)*60.0/
		                             (float)(Division * sulPreviousTempo));
	}
	else
	{ // Use the current tempo data.
		sfSecondsFromStart = (float)(ssfLastTempoEventSeconds +
		                             (float)(ulTicks - sulLastTempoEventTicks)*60.0/
		                             (float)(Division * sulCurrentTempo));
	}
	return sfSecondsFromStart;
}

/*==============================================================================*/


/*==============================================================================*/
//End DJLB tempo event handling modifications.

//DJLB 20-06-2008. EnHarmonic mod.  - This function is re-defined.
void MIDIpitch_to_freq_and_name(int inChordRootNoteScale,
                                int note_num, float &freq, EST_String &name)
{       // DJLB 09-09-2006 Corrections made so that conventional
	//      MIDI-frequency-notename relationships are used.
	EST_Features *gl = get_Flinger_params();
	unsigned int uiTranspose;
	uiTranspose = (unsigned int)gl->F("transpose"); // +/- semitones.
	note_num += uiTranspose;
	int octave = note_num / 12 - 1;
	EST_String s_octave;
	int scale_tone = note_num % 12;
	float base_freq;

	/* DJLB 22-07-2008. Use of the Just Scale to set note frequency introduced.
	         A switch from a new Festival parameter (use_just_tuning) selects the old
	        (Tempered Scale) setting - false - or the Just Scale settings - true. */
	if(esUseJustTuning == "yes")
	{
		static int inlastChordRootNoteScale = 0;
		int inAdjustedScaleTone, inAdjustedOctave;
		double dfRootToneFreq;

		if((inChordRootNoteScale < 0) || (inChordRootNoteScale >= 12))
		{ //Use the last chord root as the basis for the note.
			inChordRootNoteScale = inlastChordRootNoteScale;
		}

		inAdjustedScaleTone = scale_tone - inChordRootNoteScale;
		if(inAdjustedScaleTone < 0)
		{
			inAdjustedScaleTone += 12;
			inAdjustedOctave = octave - 1;
		}
		else
			inAdjustedOctave = octave;

		dfRootToneFreq = ScaleRatio[inChordRootNoteScale] * dfC0;

		if ((inAdjustedScaleTone >= 0) && (inAdjustedScaleTone < 12))
		{
			float sfNoteBaseFrequency = (float)
			                            ( dfRootToneFreq * ScaleRatio[inAdjustedScaleTone]);
			freq = sfNoteBaseFrequency * (2 << inAdjustedOctave);
			s_octave = (char) (48 + octave);
			name = NoteName[scale_tone] + s_octave;
		}
		else
		{
			cerr << "fl_midi_in:  scale tone=" << scale_tone << " out of range - how??\n.";
			EST_error("...");
		}
		inlastChordRootNoteScale = inChordRootNoteScale;
	}
	else
	{ // Use the Tempered Scale system.
		if ((scale_tone >= 0) && (scale_tone < 12))
		{
			//DJLB addition of 'float' cast.
			base_freq = (float)BaseFreq[scale_tone];
			freq = base_freq * (2 << octave);
			s_octave = (char) (48 + octave);
			name = NoteName[scale_tone] + s_octave;
		}
		else
		{
			cerr << "fl_midi_in:  scale tone=" << scale_tone << " out of range - how??\n.";
			EST_error("...");
		}
	}
}

/*==============================================================================*/

static EST_Utterance *get_current_utt(void)
{
	if (Utt != NULL)
		return Utt;
	else
	{
		EST_error("fl_midi_in:  utterance disappeared!\n");
		return Utt;
	}
}

/*==============================================================================*/

static void set_current_utt(EST_Utterance *u)
{
	Utt = u;  return;
}

/*==============================================================================*/
