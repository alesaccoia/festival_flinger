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
/*  Author:                     David Brown                               */
/*                                                                        */
/****************************<--CSLU-->************************************/

/*
	This file has been added to the Flinger festival module.
	In it, I have written code to analyse a 'Song' class representation 
	of a multitrack MIDI file - specifically built as for four-part singing
    e.g. a Barbershop Quartet - that contains four tracks with notes.

	The time-coincident notes from all note tracks are analysed to
	determine the chord they represent. Information enabling the calculation of
    pitch adjustments to the Just scale are placed in each Note event, so that
    chords are sung in just pitch.
	Adjustments to volume are also be made e.g. to slightly lower the 
	volume when singing the 3rd or 7th in the chord, and/or increase 
	the volume when singing the root or 5th.

	Calling the function fvdSetJustTuneData() results in each note's NoteOnEvent value 
	'chord_root_scale_tone' being set to the scale tone of the root of the chord 
    in which the note occurs.

	Pitch adjustments are made in the file fl_midi_in.cpp, where the pitch scale
	will be set according to the chord root, when each note's frequency is calculated. 

    NOTEs: The JustScale tuning modification, of which this file is a part, handles most usual
            situations. 
            (1) If a note group occurs that it does not recognise, it does nothing.
                If a note is held whilst other track notes change, the held note is
                retuned according to the new chord.
            (2) Testing needed to determine if the held note re-tuning is not
                upset by other events withing the time-slot of the held note.
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
#include "MIDI/EvntUtil.h"
#include "fl_pitch.h"
#include "fl_midi_in.h"

// An array of structures to hold the library of chords.
static struct
{
    string  stName;
    int     inNote[4]; // maximum of 4 notes per chord.
}LibChord[12][16]; //12 roots, 16 chord types for each root.

/* NOTES ON OVERALL STRATEGY.
    Each chord array has 4 possible notes. If it has only 3 notes, the fourth
			has a value of -1. (Chords with less than 3 notes or more than 4
            are not compared to the library.)
		Indeces of the array LibChord[i][j][k] are:-
			i: Scale tone of the root (0-11). These represent 
                    scale tone  0  1   2  3   4  5  6   7  8   9  10  11  
					notes       C, C#, D, D#, E, F, F#, G, G#, A, A#, B 

			j: Chord type	(0-15). These represent the chords
                  Type    Name    
					0   major triad
					1   minor triad
					2   augmented triad
					3   diminished triad
					4   6th	(5th absent)          
					5   minor 6th (5th absent)   
					6   7th (barbershop 7th)
					7   minor 7th
					8   1/2 diminished 7th     (07)
					9  fully diminished 7th    (o7)
					10  augmented 7th
					11  major 7th
					12  9th
					13  7-9	(no root)
                Note: 6th with 5th present, and minor 6th with 5th present are equivalent 
                        to other chords listed above, e.g.
                        C6 with 5th is equal to Am7, Cm6 with 5th is equal to A07.
                        
			k: Chord voice(0-3).
					The chord 'voice', in order of increasing scale tone. (This does NOT
					equate to the part of the chord represented e.g. 5th, 7th etc. OR
                    the MIDI file's channel (sometimes referred to as 'voice'). 
		Content of LibChord.inNote[i] is the scale tone of the voice i e.g.
			LibChord[4][11].inNote[2] = 10. This is the scale tone (== the note A# i.e. Bb) of one
			voice in the chord Eo7th.

    References to 'track' through this file may in fact be to groups of events 
     categorised by either the 'track' in the MIDI file from which they come, and/or
     the MIDI 'channel' nominated within the event structure.
*/

extern FILE *fPitchDetails;
static Event **pNotes[4]; //Provides reference to four lists of events.

static int inSongTrackNumber[4];
static int inSongChannelNumber[4];
static int inNoteGroup; //The index to the group of notes taken from one song track or channel.
extern void MIDIpitch_to_freq_and_name(int note_num, float &freq, EST_String &name);
void fvdListNotes(int inNumberOfTracks, int inProcessChannel);
void fvdGetChords(int inProcessTrack, int inProcessChannel);
void fvdAdjustNotes(NoteOnEvent* pChordNoteEvent[4], long lnChordTick, 
                                int inProcessTrack, int inProcessChannel);
void fvdBuildChordLibrary(void);
int finFindChord(int inScaleTone[4], int *inChordType);
Song *pSong;

/*===========================================================================*/
/*===========================================================================*/

void fvdSetJustTuneData(Song *pmf, int inProcessTrack, int inProcessChannel )
{	// mf is a Song class object that has been built from a MIDI file.
    pSong = pmf;
	// Build the chord library.
	fvdBuildChordLibrary();
    
	/* Find the tracks with notes, and put them into an array of lists,
        one list for each track containing notes. The notes are held as
        note events, one for each node of the list for its track. */

	for (int i=0; i<4; i++)
    {
		pNotes[i] = MakeList(Event*);
        inSongChannelNumber[i] = -1;
    }

    inNoteGroup = -1;
	int inNumberOfTracks = pmf->GetNumTracks();

	fvdListNotes(inNumberOfTracks, inProcessChannel);
	pmf->RewindEvents();
	// The array T[] in stTracks now contains the numbers of the note tracks,
	// and the four event trees contain the notes, in order for each track.
	
	//Find the chords and adjust notes in each chord.
	fvdGetChords(inProcessTrack, inProcessChannel);
    pmf->RewindEvents();
}

/*===========================================================================*/

void fvdListNotes(int inNumberOfTracks, int inProcessChannel)
{
	bool boNoteFound = false, boNewChannel = false;
	int inTrack; //The track number in the song.
    int inChannel; //Used if note identification is to be by channel as well as track. 
	Event *pEvent/*, *pNextEvent*/;
    /*If a channel is nominated (inProcessChannel >= 0) then all notes must be 
        identified with a channel as well as a MIDI track. */
	for (inTrack = 1; inTrack < inNumberOfTracks; inTrack++)
	{
        boNoteFound = false;
        boNewChannel = true;
		for (pEvent= pSong->NextEvent(inTrack); pEvent!=NULL; 
			                        pEvent = pSong->NextEvent(inTrack))
		{
			if(pEvent->GetType() == NOTEON) 
			{
                if(inProcessChannel >= 0)
                {
                    inChannel = ((NoteEvent*)pEvent)->GetChannel();
                    int i;
                    for( i=0; i<=inNoteGroup; i++)
                    {   //Has this channel been found before in this track?
                        if(inChannel == inSongChannelNumber[i])
                        {
                            boNewChannel = false;
                            break;
                        }
                    }
                    if(boNewChannel)
                    {
                        inNoteGroup++;
                        inSongTrackNumber[inNoteGroup] = inTrack;
                        inSongChannelNumber[inNoteGroup] = inChannel;
                        AddAtListHead( pEvent, pNotes[inNoteGroup]);
                    }
                    else
                    {   //Find the note track to which this event belongs.
                        int i;
                        for( i=0; i<=inNoteGroup; i++)
                        {
                            if(inChannel == inSongChannelNumber[i])
                            {
                                AddAtListHead( pEvent, pNotes[i]);
                                 break;
                            }
                        }
                    }
                }
                else
                {
                    if(!boNoteFound)
                    {
                        inNoteGroup++;
                        inSongTrackNumber[inNoteGroup] = inTrack;
                        inSongChannelNumber[inNoteGroup] = -1;					
                        boNoteFound = true;
                    }
				    AddAtListHead( pEvent, pNotes[inNoteGroup]);
                }
			}
		}
	}
}
	
/*===========================================================================*/

void fvdGetChords(int inProcessTrack, int inProcessChannel)
{	/* Collect note events from the four note lists, then if any note has changed
        and there are at least 3 notes collected, call fvdAdjustNotes() to find
        the chord and store chord information in the note events in the track
        being processed.
        The chord notes are held in the array pChordNoteEvent[] as pointers to
        the event structures, in Song, that represent them. A note will not be
        in the array if it is not 'on' at the time of the last note change.
       The search for notes is done at every time of a note chnage, by checking
       the end time of each note and setting the next chord time accordingly.
    */
/* NOTES: inGroup is NOT the track number in the song 
            (although it might have the same value!). It has value 0->3,
            representing the (internal to this file) note 'track' number.
          All instances of chord changes must be addressed. A chord change 
            can happen because of a number of changes:
                (a) one or more notes in a track change;
                (b) one or more notes end and are not immediately replaced;
                (c) all notes end, followed by silence until one or more
                     notes start.           
*/
bool boChordChange;
int inNoteCount, inGroup, inGroupCount;
long lnChordTick = -1, lnNoteEndTime, lnNextNoteEndTime, lnNextNoteStartTime;
NoteOnEvent *pNote;
NoteOnEvent *pChordNoteEvent[4] = {NULL,NULL,NULL,NULL}; 
	
    inGroupCount = 0;
	do
	{
        lnNextNoteStartTime = 0x7fffffff;
        lnNextNoteEndTime = 0x7fffffff;
		boChordChange = false;
		inNoteCount = 0;
		
		for(inGroup = 0; inGroup < 4; inGroup++)
		{
GET_NEXT_EVENT:
 			if(GetFromListTail(&pNote, pNotes[inGroup]) != NULL)
            {//Put note in chord if it is not already there, in this track's position.
				lnNoteEndTime = pNote->GetTime() + ((NoteOnEvent*)pNote)->GetDuration();

                if(lnNoteEndTime <= lnChordTick)
                {   /* The note has terminated. Delete it from its list and from the chord.
                        Notify a chord change is occurring, and get the next event
                        from the current list of notes.*/
                    DeleteFromTail(pNotes[inGroup]);
                    if(pChordNoteEvent[inGroup] == pNote)
                        pChordNoteEvent[inGroup] = NULL;
                    boChordChange = true;
                    goto GET_NEXT_EVENT;
                }

                if(lnChordTick == -1)
                    lnChordTick = pNote->GetTime();

                if(pNote->GetTime() <= (unsigned long)lnChordTick)
                {   /*The note is current. If it is not already in the chord
                        i.e. it is not hanging, put it in the chord and notify
                        a chord change.*/
                    if(pChordNoteEvent[inGroup] != pNote)
                    {
                        pChordNoteEvent[inGroup] = pNote;
                        boChordChange = true;
                    }
                }

                if((pNote->GetTime() < (unsigned long)lnNextNoteStartTime) &&
                        (pChordNoteEvent[inGroup] != pNote))
                    lnNextNoteStartTime = pNote->GetTime();

                if(lnNoteEndTime < lnNextNoteEndTime)
                    lnNextNoteEndTime = lnNoteEndTime;			    
			}
			else 
			{   //No more notes on this track.
				pChordNoteEvent[inGroup] = NULL;
				inGroupCount ++;
				//break;
			}
		}

		if(boChordChange) 
        {  // Determine how many notes exist at the current time.
		    for(inGroup = 0; inGroup < 4; inGroup++)
		    {
			    if(pChordNoteEvent[inGroup] != NULL) 
				    inNoteCount++;
		    }
            if(inNoteCount > 2)
		    {
			    fvdAdjustNotes(pChordNoteEvent, lnChordTick, 
                                            inProcessTrack, inProcessChannel );
                lnChordTick = lnNextNoteEndTime;
		    }
            else
                lnChordTick = lnNextNoteStartTime;
        }

	}while(inGroupCount < 4);

	for(inGroup = 0; inGroup < 4; inGroup++)
		DestroyList(pNotes[inGroup]);
}

/*===========================================================================*/

void fvdAdjustNotes(NoteOnEvent* pChordNoteEvent[4], long lnChordTick, 
                    int inProcessTrack, int inProcessChannel)
{
	//Get the scale tone for each note in the chord.
	int inScaleTone[4], inRootScaleTone, inChordType, i, j, 
            inDifferentNotesCount = 0, inSameNotesCount = 0, inPossibleNotesCount = 4;
    
	for(i = 0; i < 4; i++)
	{
		if(pChordNoteEvent[i] == NULL)
			inScaleTone[i] = -1;
		else
        {
			inScaleTone[i] = ((NoteOnEvent*)pChordNoteEvent[i])->GetPitch() %12;
        }
	}
	//Determine the number of different notes in the chord.
    for(i = 0; i < 4; i++)
	{
        if(inScaleTone[i] >= 0)
        {
            for(j = i+1; j < 4; j++)
            {
                if( inScaleTone[j] == inScaleTone[i])
                    inSameNotesCount ++;
            }
        }
        else
            inPossibleNotesCount --;
    }
    inDifferentNotesCount = inPossibleNotesCount - inSameNotesCount;
	// inScaleTone now contains the scale tones of the notes collected.
    // Only look for a chord if 3 or more notes are different.
    if(inDifferentNotesCount >= 3 )
    {
	    inRootScaleTone = finFindChord(inScaleTone, &inChordType);
    }
    else
    {   
        inRootScaleTone = -1;
        inChordType = 0;
    }

	// Store the root in and the chord name in the note event's chord name variable.
	for(i = 0; i < 4; i++)
    {
        if((inSongTrackNumber[i] == inProcessTrack) &&
           ((inProcessChannel < 0) || (inSongChannelNumber[i] == inProcessChannel)))
            break;
    }
	{   //Don't overwrite if already set. 
        // (i.e. when note is held through more than one chord.)
        if(pChordNoteEvent[i] != NULL)
        {   //Don't overwrite if already set. 
            // (i.e. when note is held through more than one chord.)
            if(pChordNoteEvent[i]->GetChordRootScaleTone() < 0)
		    {   // Set the root scale tone.
			    pChordNoteEvent[i]->SetChordRootScaleTone(inRootScaleTone);
                // Set the chord name.
                pChordNoteEvent[i]->SetChordName( 
                            LibChord[inRootScaleTone][inChordType].stName );
            }
            else
            {   /*Note is held from a previous chord. Insert two new note pairs to replace
                 the original pair, and adjust durations and tick times.
                 The first new pair take the characteristics of the old pair (with
                 a shortened duration) and the new pair take the characteristics
                 of the old pair, with time set to the current tick, duration to take
                 it to the old pair's termination time, and chord data set to the
                 new chord information.*/
                int inSongTrack = inSongTrackNumber[i];                
                Event *peOldOn,*peOldOff, *peNewOn1, *peNewOff1, *peNewOn2, *peNewOff2,
                        *pePutOn1, *pePutOff1, *pePutOn2, *pePutOff2;
                peOldOn = pChordNoteEvent[i];
                peOldOff = ((NoteEvent*)peOldOn)->GetNotePair();

                peNewOn1 = new NoteOnEvent();
                peNewOff1 = new NoteOffEvent();
                peNewOn2 = new NoteOnEvent();
                peNewOff2 = new NoteOffEvent();
                //Set the new NoteOn events' parameters.
                peNewOn1->SetTime(peOldOn->GetTime());
                ((NormalEvent*)peNewOn1)->SetChannel(((NormalEvent*)peOldOn)->GetChannel());
                ((NoteEvent*)peNewOn1)->SetPitch(((NoteEvent*)peOldOn)->GetPitch());
                ((NoteEvent*)peNewOn1)->SetVelocity(((NoteEvent*)peOldOn)->GetVelocity()); 
                ((NoteEvent*)peNewOn1)->SetChordName(((NoteEvent*)peOldOn)->GetChordName() );
                ((NoteEvent*)peNewOn1)->SetChordRootScaleTone(((NoteEvent*)peOldOn)->GetChordRootScaleTone());
                ((NoteOnEvent*)peNewOn1)->SetDuration(lnChordTick - peOldOn->GetTime());
                
                peNewOn2->SetTime(lnChordTick);
                ((NormalEvent*)peNewOn2)->SetChannel(((NormalEvent*)peOldOn)->GetChannel());
                ((NoteEvent*)peNewOn2)->SetPitch(((NoteEvent*)peOldOn)->GetPitch());
                ((NoteEvent*)peNewOn2)->SetVelocity(((NoteEvent*)peOldOn)->GetVelocity()); 
                ((NoteEvent*)peNewOn2)->SetChordName(LibChord[inRootScaleTone][inChordType].stName );
                ((NoteEvent*)peNewOn2)->SetChordRootScaleTone(inRootScaleTone);
                ((NoteOnEvent*)peNewOn2)->SetDuration(peOldOff->GetTime() - lnChordTick);
 
                //Set the new NoteOff events' parameters.
                 peNewOff1->SetTime(lnChordTick);
                ((NormalEvent*)peNewOff1)->SetChannel(((NormalEvent*)peNewOn1)->GetChannel());
                ((NoteEvent*)peNewOff1)->SetPitch(((NoteEvent*)peNewOn1)->GetPitch());
                ((NoteEvent*)peNewOff1)->SetVelocity(((NoteEvent*)peNewOn1)->GetVelocity());

                 peNewOff2->SetTime(peOldOff->GetTime());
                ((NormalEvent*)peNewOff2)->SetChannel(((NormalEvent*)peNewOn2)->GetChannel());
                ((NoteEvent*)peNewOff2)->SetPitch(((NoteEvent*)peNewOn2)->GetPitch());
                ((NoteEvent*)peNewOff2)->SetVelocity(((NoteEvent*)peNewOn2)->GetVelocity());
                
                //Remove the old event pair from the song.
                pSong->DeleteEvent(inSongTrack, *peOldOn);
                pSong->DeleteEvent(inSongTrack, *peOldOff);

                //Put the new events into pSong.
                pePutOn1 = pSong->PutEvent(inSongTrack, *peNewOn1);
                //PutEvent does not copy the duration parameter.
                ((NoteOnEvent*)pePutOn1)->SetDuration(((NoteOnEvent*)peNewOn1)->GetDuration());
                pePutOff1 = pSong->PutEvent(inSongTrack, *peNewOff1);
                //Set the note pair.
                pSong->SetNotePair(inSongTrack, pePutOff1);

                pePutOn2 = pSong->PutEvent(inSongTrack, *peNewOn2);
                //PutEvent does not copy the duration parameter.
                ((NoteOnEvent*)pePutOn2)->SetDuration(((NoteOnEvent*)peNewOn2)->GetDuration());
                pePutOff2 = pSong->PutEvent(inSongTrack, *peNewOff2);
                //Set the note pair.
                pSong->SetNotePair(inSongTrack, pePutOff2);
                //Replace the old note listings with the new, 2nd. note on.
                pChordNoteEvent[i] = (NoteOnEvent*)pePutOn2;                
                ((tstGenList*)pNotes[i])->pstTail->pstData->pstHead = (tstGenList*)pePutOn2;
                      // (Not very elegant, but it works and saves messing with GenList.)
                //Clean up.
                delete(peNewOn1);
                delete(peNewOff1);
                delete(peNewOn2);
                delete(peNewOff2);
            }
		}
	}
} // fvdGetChords()

/*===========================================================================*/

int finFindChord(int inScaleTone[4], int *inChordType)
{
	//Search the chord library for a chord whose parts match the parts of the
	// chord represented in the argument.
	/* The fully diminished minor 7th chord (chord index 12 in this library) 
			presents a problem. It has an indeterminate root (any note in the chord 
			may be its root). This is OK in the tempered scale, but in the Just scale, 
			the pitches of all the notes change slightly depending upon which note 
			is called the root e.g. when Eo7 (E,G,Bb, Db) is called Db07, the pitch 
			of the E sharpens by approximately 1/2 semitone! This might sound 
			a little weird if it follows an chord with a definite root of E, 
			particularly if the same voice is singing the E in both chords.
			A (hopeful) fix is built in here, by checking the root against the last
			chord determined, and going back to the library until the right root
			is selected.*/
	static int inLastChordRoot = 0;
	bool boRootRequired;
	int i, ii, j, k, m;
    int inMatchCount = 0, inLoopCount = 0;
	boRootRequired = true;

SEARCH_AGAIN:

	for(ii = 0; ii < 4; ii++) 	
    {   //Step through chord roots, using only notes that are in the given chord.
        if(inScaleTone[ii] >= 0)
        {
            i = inScaleTone[ii];

		    for(j = 0; j < 14; j++) 
            {   //Step through the types of chords.
		    	//  Check the given chord parts against the selected library chord.
    GET_CHORD_VOICES:
			    for(m = 0; m < 4; m++)
                {   /* Step through the parts of the given chord.
			            The given chord might contain only 3 notes. In this case,
                        one of its cells will be set to -1, which will match the -1 in a 
                        cell of any library chord with only 3 notes.
                       Further, the given chord may contain two notes of the same base
                        pitch. In order that these be counted as two matches, the cycling 
                        is done by cycling the library cells inside the loop that cycles 
                        the given chord's cells.*/
			        for(k = 0; k < 4; k++)//Step through the parts of the library chord.
				    {
                        if(inScaleTone[m] == LibChord[i][j].inNote[k]) 
                            inMatchCount++;
				    }
			    }
			    inLoopCount++;
			    if(inLoopCount > 2000)
			    {
				    cout << "ERROR FINDING CHORD: i = " << i << ", j = " << j << endl;
				    goto NO_CHORD_FOUND;
			    }
			    if(inMatchCount == 4) 
			    {/* Chord was matched. 
                    (Note that 3-note chords are catered for by an absent note being
                        represented in the unknown chord and the library chords that
                        require only 3 notes, by -1.) */
				    if(j == 9) //Fully diminished 7th chord.
				    { /* If the root of the last chord is in this chord, make it
							    the root of this chord. (The fully diminished 7th is a 
                                'universal' chord. Any one of its notes may be its root.)*/
					    if(i == inLastChordRoot)
						    goto CHORD_FOUND;
					    else if( (inLastChordRoot == inScaleTone[0]) ||
									     (inLastChordRoot == inScaleTone[1]) ||
									     (inLastChordRoot == inScaleTone[2]) ||
									     (inLastChordRoot == inScaleTone[3]))
					    {
						    i = inLastChordRoot;
						    inMatchCount = 0;
						    goto GET_CHORD_VOICES;  //of the fully diminished 7th whose root
                                                    // is the root of the last chord.
					    }
					    else
						    goto CHORD_FOUND;
				    }
				    else
					    goto CHORD_FOUND;
			    }
			    else //Chord was not matched. Try another library chord.
                {
				    inMatchCount = 0;
                }
            }// for chord types.
		}// if note in given chord exists.
	}// for possible roots.
	//No chord was matched. 
	if(boRootRequired)
	{	// Search again, this time not requiring the root to be in the chord.
		boRootRequired = false;
		goto SEARCH_AGAIN;
	}
	// else no matching chord was found. Return -1.
NO_CHORD_FOUND:
	*inChordType = 0;
	return -1;
CHORD_FOUND:
	// Chord has been matched. Store the root scale tone in case the next chord 
    // is a o7.
	inLastChordRoot = i;
    // Send the chord type back via the argument *inChordType.
	*inChordType = j;
    //Return the root scale tone.
	return i;
}

/*===========================================================================*/

void fvdBuildChordLibrary(void)
{
	/*The library is based upon 16 chords with root = C. Three-note chords have
        one note set to -1.
        Chords with other routes are derived by adding the scale tone of the root 
            to all notes, then reducing the results to base scale tones e.g.
			D 7th is derived by adding d's scale tone (2) to all notes:-
				C 7th + 2:
					LibChord[0][6][0] + 2 = 2;
					LibChord[0][6][1] + 2 = 6;
					LibChord[0][6][2] + 2 = 9;
					LibChord[0][6][3] + 2 = 12;
				Each note %12 gives the structure of D 7th:
					LibChord[0][6][0] %12 = 2;
					LibChord[0][6][1] %12 = 6;
					LibChord[0][6][2] %12 = 9;
					LibChord[0][6][3] %12 = 0;
			NOTE: The procedure is NOT applied to any note with a base scale of -1.
static struct
{
    string  stName;
    int     inNotes[4]; // maximum of 4 notes per chord.}
}LibChord[12][16]; //12 roots, 16 chord types for each root.	*/

	int i, j, k;
	// C Major
    LibChord[0][0].stName = "";
	LibChord[0][0].inNote[0] = 0;
	LibChord[0][0].inNote[1] = 4;
	LibChord[0][0].inNote[2] = 7;
	LibChord[0][0].inNote[3] = -1;
	// C Minor
    LibChord[0][1].stName = "m";
	LibChord[0][1].inNote[0] = 0;
	LibChord[0][1].inNote[1] = 3;
	LibChord[0][1].inNote[2] = 7;
	LibChord[0][1].inNote[3] = -1;
	// C Augmented
    LibChord[0][2].stName = "aug";
	LibChord[0][2].inNote[0] = 0;
	LibChord[0][2].inNote[1] = 5;
	LibChord[0][2].inNote[2] = 7;
	LibChord[0][2].inNote[3] = -1;
	// C dim
    LibChord[0][3].stName = "dim";
	LibChord[0][3].inNote[0] = 0;
	LibChord[0][3].inNote[1] = 3;
	LibChord[0][3].inNote[2] = 6;
	LibChord[0][3].inNote[3] = -1;
	// C 6th
    LibChord[0][4].stName = "6";
	LibChord[0][4].inNote[0] = 0;
	LibChord[0][4].inNote[1] = 4;
	LibChord[0][4].inNote[2] = 9;
	LibChord[0][4].inNote[3] = -1;
	// C min 6th
    LibChord[0][5].stName = "m6";
	LibChord[0][5].inNote[0] = 0;
	LibChord[0][5].inNote[1] = 3;
	LibChord[0][5].inNote[2] = 9;
	LibChord[0][5].inNote[3] = -1;
	// C 7th
    LibChord[0][6].stName = "7";
	LibChord[0][6].inNote[0] = 0;
	LibChord[0][6].inNote[1] = 4;
	LibChord[0][6].inNote[2] = 7;
	LibChord[0][6].inNote[3] = 10;
	// C min 7th
    LibChord[0][7].stName = "m7";
	LibChord[0][7].inNote[0] = 0;
	LibChord[0][7].inNote[1] = 3;
	LibChord[0][7].inNote[2] = 7;
	LibChord[0][7].inNote[3] = 10;
	// C 1/2dim 7th
    LibChord[0][8].stName = "07";
	LibChord[0][8].inNote[0] = 0;
	LibChord[0][8].inNote[1] = 3;
	LibChord[0][8].inNote[2] = 6;
	LibChord[0][8].inNote[3] = 10;
	// C dim 7th
    LibChord[0][9].stName = "o7";
	LibChord[0][9].inNote[0] = 0;
	LibChord[0][9].inNote[1] = 3;
	LibChord[0][9].inNote[2] = 6;
	LibChord[0][9].inNote[3] = 9;
	// C aug 7th
    LibChord[0][10].stName = "aug7";
	LibChord[0][10].inNote[0] = 0;
	LibChord[0][10].inNote[1] = 5;
	LibChord[0][10].inNote[2] = 7;
	LibChord[0][10].inNote[3] = 10;
	// C maj 7th
    LibChord[0][11].stName = "maj7";
	LibChord[0][11].inNote[0] = 0;
	LibChord[0][11].inNote[1] = 4;
	LibChord[0][11].inNote[2] = 7;
	LibChord[0][11].inNote[3] = 11;
	// C 9th
    LibChord[0][12].stName = "9";
	LibChord[0][12].inNote[0] = 0;
	LibChord[0][12].inNote[1] = 4;
	LibChord[0][12].inNote[2] = 7;
	LibChord[0][12].inNote[3] = 2;
	// C 7-9 (no root).
    LibChord[0][13].stName = "79";
	LibChord[0][13].inNote[0] = 4;
	LibChord[0][13].inNote[1] = 7;
	LibChord[0][13].inNote[2] = 10;
	LibChord[0][13].inNote[3] = 2;
	//Build chords with root = other notes in the scale.
	for(i = 1; i < 12; i++)
	{
		for(j = 0; j < 14; j++)
		{
            LibChord[i][j].stName = LibChord[0][j].stName;
			for(k = 0; k < 4; k++)
			{
                if(LibChord[0][j].inNote[k] != -1)
				    LibChord[i][j].inNote[k] = (LibChord[0][j].inNote[k] + i) %12;
                else
                    LibChord[i][j].inNote[k] = -1;
			}
		}
	}
}

/*===========================================================================*/















