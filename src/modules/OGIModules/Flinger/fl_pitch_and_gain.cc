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

/*++++++ New code, by DJLB. Replaces original file fl_pitch.cpp +++++*/
/* 
The main function fvdAddNoteTarget() replaces the original add_note_target().
The other functions are relatively unchanged.
All code has been re-written with Hungarian notation for function names, 
variables etc.
This code gets pitchbend, vibrato, drift working, and adds glissando.
	Volume control according to note velocity is implemented here, as a 
	convenient place where the notes are being stepped through. 
In order to separate flinger operations from OGI, the gain functions previously used
	to implement volume control have been copied from OGIgain.cpp and added to
	the code in fl_pitch.cpp. The file resulting from this action has been
	renamed fl_pitch_and_gain.cpp, which replaces fl_pitch.cpp in the project.
*/
#include <stdlib.h>
#include "festival.h"
#include "fl_io.h"
#include "fl_pitch.h"

#define EPSILON 0.000001

static void fvdRelateF0ModToNote(EST_Utterance *u);
static void fvdAddNoteTarget(EST_Utterance *u, EST_Item *note, F0mod_obj &modOsc);

static void flSetGainTargets(EST_Utterance *u, float flBeginTime, float flEndTime, 
										float flNoteVelocity, float flDeltaTime);

static float sfSamplePeriod;   // sampling rate for F0 contour
static float sfPortamentoTime;  // Portamento time

/*------------------------------------------------------------------------------*/
/*============================== Pitch Control =================================*/

//								Public function

LISP FL_NoteF0_Utt(LISP utt)
{
EST_Item *note;
EST_Utterance *u = get_c_utt(utt);
EST_Features *gl = get_Flinger_params();
//Create the pitch modulation object.
F0mod_obj mod;
	// set globals
	sfSamplePeriod = mod.fsfNyquistPeriod();
	sfPortamentoTime = (float)gl->F("portamento_time");

	// relate events in the F0_Mod relation to the Note relation
	fvdRelateF0ModToNote(u);

	// loop through notes, set F0 Targets at begin and end of each for synthesis
	for (note=u->relation("Note")->first(); note != 0; note = note->next())
		fvdAddNoteTarget(u, note, mod);

	return utt;
}

/*--------------------------------------------------------------------------*/
//							Private functions

static void fvdRelateF0ModToNote(EST_Utterance *u)
{
EST_Item *f0mod,*n;
float next_ntime,t_vibr;
  
	f0mod=u->relation("F0_Mod")->first();
	for (n=u->relation("Note")->first(); n != 0; n=n->next()) 
	{
		next_ntime = n->F("off");

		if (f0mod != NULL)
		{
			t_vibr = f0mod->F("start");
			while (t_vibr < next_ntime)
			{
				append_daughter(n, "NoteStruct", f0mod);
				if ((f0mod = f0mod->next()) == NULL)
					break;
				t_vibr = f0mod->F("start");
			}
		}
	}
}

/*--------------------------------------------------------------------------*/

void fvdSetPitchTarget( EST_Relation *pesRelation, F0mod_obj &modOsc, 
											float sfPitch, float sfTime )
{
EST_Item *pesFoTarget;
float sfFrequency;
	sfFrequency = sfPitch * modOsc.fsfGetModulation(sfTime);
	pesFoTarget = pesRelation->append();
	pesFoTarget->set("f0", sfFrequency);
	pesFoTarget->set("pos", sfTime);
}

/*--------------------------------------------------------------------------*/

static void fvdAddNoteTarget(EST_Utterance *u, EST_Item *pThisNote, 
														F0mod_obj &modOsc)
{
// General variables.
static BOOL boVibratoOn = FALSE;
BOOL boGlissOn = FALSE;			
EST_Item *pNote;
EST_Relation *TargetRelation=NULL;
// Time variables
float t, /*t_set,*/ t_NoteOn, t_NoteOff, /*t_EventEnd,*/ t_NextEvent, t_End;
//Frequency variables.
EST_Item *pf0mod;
float sfPitch;	
// Volume variables.
float sfVolume;
static float flLastNoteOn = 0;
	/**** This function is called each time a note is found in u.****/

	pNote = pThisNote->as_relation("NoteStruct");

	// Get the timing, pitch and volume for this note.
	if(pNote == NULL)
	{
		if (Flinger_get_debug_file_flag()) {
			cout << endl << "WARNING: The note following that with On time = "
					<< flLastNoteOn << " has no 'NoteStruct' relation." 
					<< " It will be ignored." << endl;
		}
		return;
	}

	t_NoteOn  = pNote->F("on");
	flLastNoteOn = t_NoteOn;
	t_NoteOff = pNote->F("off");
	t_End = max(t_NoteOff - sfPortamentoTime, t_NoteOn + (float)EPSILON);
	sfPitch = pNote->F("freq");
	sfVolume = pNote->F("volume"); 
	/* Insert the gain targets for this note. 
		The 'samp' parameter for flPlaceGainTarget() controls the rate at which
		any gain change is applied. 20 msec. seems reasonable. */
	flSetGainTargets( u, t_NoteOn, t_NoteOff - (float)0.001, sfVolume, (float)0.02 );

	// Now deal with pitch.
	// Get the first frequency-control event related to this note.
	pf0mod = daughter1(pNote);

	/* Loop through time from the start to the end of this note, setting 
		frequency targets and taking account of frequency modulation events.*/
	t = t_NoteOn;

	/* Get a target relation of u, into which pitch events 
		that occur during this note will be set. */
	TargetRelation = u->relation("Target");

	while (t < t_End) 
	{	
		if (pf0mod != NULL)
			t_NextEvent = (pf0mod)->F("start");
		else
			t_NextEvent = t_End;

		if(t == t_NextEvent) 
		{	// Note that if pf0mod was NULL, t cannot = t_NextEvent.
			/* Each change to pitch must be preset by inserting a target that
				carries the current pitch, so that the synthesiser will not
				'gliss' between the last target and the new one. */
			if (pf0mod->name() == "pitchbend")
			{	// Set the current pitch before starting the bend.
				if(t > t_NoteOn)//One at t_NoteOn has already been set.
					fvdSetPitchTarget( TargetRelation, modOsc, sfPitch, t );
				// Set the target 'bent' frequency.
				modOsc.fvdUpdatePitchBend( pf0mod->F("size"));
				fvdSetPitchTarget( TargetRelation, modOsc, sfPitch, t );
			}
			else if (pf0mod->name() == "notebend")
			{	// Set the current pitch before starting the bend.
				if(t > t_NoteOn)//One at t_NoteOn has already been set.
					fvdSetPitchTarget( TargetRelation, modOsc, sfPitch, t );
				// Set the target 'bent' frequency.
				modOsc.fvdUpdateNoteBend( pf0mod->F("size"));
				fvdSetPitchTarget( TargetRelation, modOsc, sfPitch, t );
			}
			else if(pf0mod->name() == "gliss")
			{	// Set the current pitch before starting the glissando.
				if(t > t_NoteOn)//One at t_NoteOn has already been set.
					fvdSetPitchTarget( TargetRelation, modOsc, sfPitch, t );
				// Move to the end of the note.
				t = t_End;
				// No need to set frequency target here. The next note's pitch
				// is the target.
				boGlissOn = TRUE;
			}
			else if (pf0mod->name() == "vibrato")
			{
			float sfVibratoAmplitude = pf0mod->F("size");
				if( modOsc.fsfGetVibratoFrequency() > 1.0)
				{
					if( sfVibratoAmplitude > 0.0 )
					{	// Update the modulation model.
						modOsc.fvdUpdateAppliedVibrato(sfVibratoAmplitude);
						boVibratoOn = TRUE;
					}
					else if( modOsc.fsfGetVibratoAmplitude() > 0.0)
					{
						boVibratoOn = TRUE;
					}
				}// else do nothing.
			}
			else if (pf0mod->name() == "portamento")
			{
				sfPortamentoTime = pf0mod->F("size");
				t_End = max(t_NoteOff - sfPortamentoTime, t_NoteOn + (float)EPSILON);
			}
			// Move on to the next event in this note.		
			pf0mod = pf0mod->next();
			if (pf0mod != NULL)	  
				t_NextEvent = pf0mod->F("start");
			else
				t_NextEvent = t_End;
		
		}
		else if (t == t_NoteOn)
			// insert pitch target at the beginning of the note, if no
			// event has done so.
			fvdSetPitchTarget( TargetRelation, modOsc, sfPitch, t_NoteOn );

		/* If vibrato is on, add frequency modulation target events as required 
			at all other times.*/
		if( boVibratoOn || modOsc.fboGetDriftOn() )
		{	
			/* Ensure that t is an integer multiple of
				sfSamplePeriod, so that vibrato does not get distorted. */
			{
			float flRemainder = fmod(t, sfSamplePeriod);
				if(flRemainder > sfSamplePeriod/10.0)
					t = t - flRemainder + sfSamplePeriod;
			}

			while( (t < t_NextEvent) && (t < t_End) )
			{
				if( t > t_NoteOn)
					fvdSetPitchTarget( TargetRelation, modOsc, sfPitch, t );
				if(sfSamplePeriod == 0.0) 
					break;
				t += sfSamplePeriod;
			}// End while(t <= t_NextEvent) loop
		}

		t = t_NextEvent;
	}// End of while(t < t_End) loop
  
	if( t <= t_End ) 
	{	// Set a pitch target at the end of the note, unless gliss is on.
		if(	!boGlissOn )
			fvdSetPitchTarget( TargetRelation, modOsc, sfPitch, t_End );
		// Terminate any notebend effect by setting notebend to 0.
		modOsc.fvdUpdateNoteBend(0.0);
	}
}

/*-----------------------------------------------------------------------------*/

/*============================== Gain Control =================================*/

static EST_Item *new_gitem(EST_Relation *g, EST_Item *gitem)
{
	if (gitem != NULL)
		return gitem->insert_after();
	else if (g->head())
		return g->head()->insert_before();
	else 
		EST_error("new_gitem problem");
	return NULL;
}

/*--------------------------------------------------------------------*/

static float fsfChannelGain( EST_Utterance *u, float flNow, 
								float flNoteEndTime, float flUttEndTime)
{
	static struct stChannelVolumeData
	{
		float flStartTime;
		float flEndTime;
		float flStartVolume;
		float flEndVolume;
		BOOL boCrescendoAtStart;
		BOOL boCrescendoAtEnd;
		float flGradient;
		EST_Item *pStartMark;
		EST_Item *pEndMark;
	}stCh = {-1.0, 0.0, 1.0, 1.0, false, false, 0.0, NULL, NULL};

	EST_Item *n;
	float flGain = 1.0;

	if( !u->relation_present("GainMod") )
	{ //No Flinger gain control stream exists. Exit returning a gain of 1.
		return flGain;
	}

	if(stCh.pStartMark == NULL)
	{	// No volume mark is in force.
		//	Look for the first volume mark prior to or during the current note.
		for (n = u->relation("GainMod")->first(); n != 0; n = n->next())
		{
			if(n->F("time") < flNoteEndTime) 
			{
				stCh.flStartTime = n->F("time");
				stCh.boCrescendoAtStart = (n->name() == "CrescMark");
				//If not a crescendo mark, or a crescendo mark with a value not
				//	zero, set start volume to the mark's value. Otherwise, leave
				// the current setting in force.
				if( !stCh.boCrescendoAtStart || (n->F("volume") > 0))
					stCh.flStartVolume = n->F("volume");
				else
					stCh.flStartVolume = 1.0;

				stCh.pStartMark = n;
				break;
			}
		}

		if(stCh.pStartMark != NULL)
		{	//Look for the next volume mark.
			for (n=stCh.pStartMark->next(); n != 0; n=n->next())
			{
				stCh.flEndTime = n->F("time");
				stCh.flEndVolume = n->F("volume");
				stCh.boCrescendoAtEnd = (n->name() == "CrescMark");
				if(stCh.boCrescendoAtStart)
					stCh.flGradient = (stCh.flEndVolume - stCh.flStartVolume)/
											(stCh.flEndTime - stCh.flStartTime);
				else
					stCh.flGradient = 0.0;
				stCh.pEndMark = n;
				break;
			}
			if(n == 0)
			{
				// No further volume marks.
				stCh.flEndTime = flUttEndTime;
				// Can't have a crescendo with no end:-
				stCh.flGradient = 0.0;
				stCh.boCrescendoAtStart = false;
				stCh.pEndMark = NULL;
			}
		}				
	}

	if(flNow >= stCh.flEndTime)
	{
	/* There is a GainMod relation, and the current time is after the 
		currently held gain info.
	  stCh contains information from the last volume mark. Make it the start
		of new data, and look for a new one that occurs during or after this time. */
		for(n = stCh.pEndMark->next(); n != 0; n = n->next())
		{
			stCh.pStartMark = stCh.pEndMark;
			stCh.pEndMark = n;

			stCh.flStartTime = stCh.flEndTime;
			stCh.flEndTime = n->F("time");

			stCh.flStartVolume = stCh.flEndVolume;
			stCh.flEndVolume = n->F("volume");

			stCh.boCrescendoAtStart = stCh.boCrescendoAtEnd;		
			stCh.boCrescendoAtEnd = (n->name() == "CrescMark");

			if(stCh.boCrescendoAtEnd && (stCh.flEndVolume == 0.0))
				stCh.flEndVolume = stCh.flStartVolume;

			if(stCh.boCrescendoAtStart)
				stCh.flGradient = (stCh.flEndVolume - stCh.flStartVolume)/
											(stCh.flEndTime - stCh.flStartTime);
			else
				stCh.flGradient = 0.0;

			if(n->F("time") >= flNow)
				break;
		}
		if(n == 0)
		{
			// No further volume marks.
			// Can't have a crescendo with no end:-
			stCh.flStartTime = stCh.flEndTime;
			stCh.flStartVolume = stCh.flEndVolume;
			stCh.boCrescendoAtStart = false;		
			stCh.flEndTime = flUttEndTime;
			stCh.flGradient = 0.0;
		}
	}
	//Calculate the required gain.
	if (stCh.boCrescendoAtStart)	
		flGain = stCh.flStartVolume + stCh.flGradient * (flNow - stCh.flStartTime);
	else
		flGain = stCh.flStartVolume;

	return flGain;
}

/*--------------------------------------------------------------------*/

static void flSetGainTargets(EST_Utterance *u, 
								float flBeginTime, float flEndTime, 				
									float flNoteVelocity, float flDeltaTime)
{
	/* 
	This function determines the 'gain' required during the current note, and 
	inserts a 'Gain'relation in utt, wherever the required gain changes. 
	The required 'gain'is the product of the note velocity setting and 
	channel gain, which can be changed by Channel Gain or Crescendo events.
	*/
	// Don't do anyting if the time intervals are too small.
	if(( (flEndTime - flBeginTime) < 0.001 ) || ( flDeltaTime < 0.001 ))
		return;

	float t, flVelocity;
	float flUttEndTime = u->relation("Segment")->tail()->F("end");
	EST_Item *gitem, *p_gitem;

	if (!(u->relation_present("Gain")))
		u->create_relation("Gain");

	EST_Relation *grel = u->relation("Gain");

	if (grel->length() < 2)
	{
		grel->clear();
		gitem = grel->append();
		gitem->set("pos", 0.0);
		gitem->set("gain", 1.0);
		gitem = grel->append();
		gitem->set("pos", flUttEndTime);
		gitem->set("gain", 1.0);
	}

	p_gitem = NULL;
	for (gitem=grel->head(); gitem!=0; gitem=gitem->next())
	{
		if (gitem->F("pos") >= flBeginTime)
			break;
		p_gitem = gitem;
	}
	gitem = p_gitem;

  /*	Apply volume changes. A new gain node is needed at beginning and
		end of the note, and if a crescendo is in effect, multiple nodes
		close together, with linearly adjusted gain values.
		A nasty overshoot effect occurs during crescendo, if the last two nodes
		during the note do not have the same gain setting, so make tehm equal.*/
	float flLastVelocity = 0;
	for (t=flBeginTime; ((t < flEndTime) && (t<flUttEndTime)); t += flDeltaTime)
	{
		flVelocity = flNoteVelocity * fsfChannelGain(u, t, flEndTime, flUttEndTime);
		if((t == flBeginTime) || !(flVelocity == flLastVelocity))
		{
			gitem = new_gitem(grel, gitem);
			gitem->set("pos", t);
			gitem->set("gain",flVelocity);
			flLastVelocity = flVelocity;
		}
	}
	gitem = new_gitem(grel, gitem);
	gitem->set("pos", flEndTime);
	gitem->set("gain",flVelocity);
}

