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

// external functions visible to the MIDI library
//  wanted to avoid  including EST stuff in MIDI code
/* Modified by DJLB 28/4/04 to add the gliss and portamento functions.*/
void FL_add_note( unsigned long tstart,
					int channel, int pitch,
						unsigned long duration,
							int velocity,
								bool boLyricAssigned);
/*
DJLB 03-05-06 Overloaded function - this is used for adding stored notes to utt.
	The additional parameters flOnTime and lfLength are required in case there
	are any mid-song tempo changes. Values for these parameters are stored with
	a note, having been calculated under the accumulated tempo conditions at the
	ticks where the note has to be located. These will be different when the note
	is retrieved from store, if a tempo change is located between the store and
	retrieve actions.
*/
void FL_add_note(unsigned long uintOnTime, 
                    int channel,
						int pitch,
							unsigned long duration,
								int velocity,
                                    int inChordRootScaleTone,
                                        string stChordName,
								            bool boLyricAssigned);

void FL_add_lyric_token(unsigned long ltime, const char *lyrstr, bool boType);

void FL_update_tempo(unsigned long time, short bpm);
void FL_add_pitchbend(unsigned long tstart, int channel, unsigned long value);
void FL_add_vibrato(unsigned long tstart, int channel, unsigned long value);
void FL_add_gliss(unsigned long tstart,	int channel );
void FL_add_Volume(unsigned long tstart, int channel,
						unsigned long value, unsigned int boCrescendo );
void FL_add_notebend(unsigned long tstart, int channel, unsigned long value );
void FL_add_portamento_time(unsigned long tstart, int channel, long value );
//DJLB 06-05-06
float MIDIticks_to_sec(unsigned long ulTicks);
float MIDIdeltaticks_to_deltasec(unsigned long ulDeltaticks, unsigned long ulTimefromStart);
