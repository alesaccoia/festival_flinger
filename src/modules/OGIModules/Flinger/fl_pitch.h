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


/*++++++ New code, by DJLB. Replaces original file +++++*/
/*
All code has been re-written with Hungarian notation for function names,
variables etc.
Conversion of scm file settings from semitones (in lieu of the original
% of frequency- which was actually 'fraction of frequency') has been incorporated.
*/
#ifndef __FL_PITCH_H__
#define __FL_PITCH_H__

#include "db_alloc.h"
#include "fl_io.h"
#include "../OGIeffect/OGIeffect.h"

LISP FL_NoteF0_Utt(LISP utt);

#define TWOPI 6.2831853

typedef int BOOL;

// Declare & define a class object.
class F0mod_obj
{
	// Declare variables.
	//	1. Those fixed for the entire piece:
	float sfDriftF1;				// drift freq 1 (Hz)
	float sfDriftF2;				// drift freq 2 (Hz)
	float sfDriftF3;				// drift freq 3 (Hz)
	float sfDriftPeakFraction;		// drift p-p amplitude (fraction of Fo)
	bool  boDrift;					// Indicates that drift is active.

	float sfVibratoHz;				// vibrato frequency (Hz)
	float sfVibratoAmplitude;		// vibrato p-p amplitude (semitones peak-peak)
	float sfPitchEffectsMax;	// pitch effects base value (semitones)

	//	2. Those controllable by MIDI events:
	float sfVibratoPeakFraction;	// vibrato amplitude (fraction of Fo)
	float sfPitchBendRatio;		// pitchbend shift (multiplier of Fo)
	float sfNoteBendRatio;		// pitchbend shift for 1 note. (multiplier of Fo)

public:
	F0mod_obj(void)
	{
		EST_Features *gl = get_Flinger_params();
		// Store frequencies directly.
		sfDriftF1	= gl->F("drift_frequency_1");
		sfDriftF2	= gl->F("drift_frequency_2");
		sfDriftF3	= gl->F("drift_frequency_3");
		sfVibratoHz = gl->F("vibrato_frequency");
		// Convert drift amplitude from semitones to fraction of frequency.
		sfDriftPeakFraction = (float) pow(2.0, gl->F("drift_amplitude")/12.0) - (float)1.0;
		// Get base values for vibrato & pitch bend.
		sfPitchEffectsMax	= gl->F("pitch_effects_max");
		//Preset controllable variables.
		sfVibratoAmplitude		= 0.0;
		sfVibratoPeakFraction	= 0.0;
		sfPitchBendRatio		= 1.0;
		sfNoteBendRatio			= 1.0;
		boDrift = ((sfDriftF1 > 0.0) || (sfDriftF2 > 0.0) || (sfDriftF3 > 0.0)) &&
					(sfDriftPeakFraction > 0.0);
	}

// Declare & define functions.
	/*-------------------------------------------------------------------------*/

	float fsfGetModulation(float t)
	{	// returns a factor with which to multiply Fo.
		return sfPitchBendRatio * sfNoteBendRatio *
								((float)1.0 + sfDriftPeakFraction*fsfDrift(t)) *
									((float)1.0 + sfVibratoPeakFraction*fsfVibrato(t));
	}

	/*-------------------------------------------------------------------------*/

	float fsfNyquistPeriod(void)
	{	// returns necessary no-alias sampling period in seconds
		return (float)1.0/
		((float)2.0*max( max(max(max(sfDriftF1,sfDriftF2),sfDriftF3),sfVibratoHz), (float)0.01 ));
	}

	/*-------------------------------------------------------------------------*/

	float fsfDrift(float t)
	{	/* This is arranged so that the peak drift is 1, regardless of how
			many of the three drift frequencies are active. */
		float fsDrift = 0.0;
		int inDivider = 0;

		if(boDrift)
		{
			if(sfDriftF1 > 0)
			{
				fsDrift = (float)cos(TWOPI*sfDriftF1*t);
				inDivider += 1;
			}
			if(sfDriftF2 > 0)
			{
				fsDrift += (float)cos(TWOPI*sfDriftF2*t);
				inDivider += 1;
			}
			if(sfDriftF3 > 0)
			{
				fsDrift += (float)cos(TWOPI*sfDriftF3*t);
				inDivider += 1;
			}
		}
		if(inDivider > 0)
			fsDrift = fsDrift/inDivider;

		return fsDrift;
	}

	/*-------------------------------------------------------------------------*/

	float fsfVibrato(float t)
	{
		return (float)cos(TWOPI*sfVibratoHz*t);
	}

	/*-------------------------------------------------------------------------*/

	float fsfGetVibratoAmplitude(void)
	{
		return sfVibratoAmplitude;
	}

	/*-------------------------------------------------------------------------*/

	float fsfGetVibratoFrequency(void)
	{
		return sfVibratoHz;
	}

	/*-------------------------------------------------------------------------*/

	BOOL fboGetDriftOn(void)
	{
		return boDrift;
	}

	/*-------------------------------------------------------------------------*/
	void fvdUpdateAppliedVibrato(float sfVibratoAmplitude_in)
	{
		sfVibratoAmplitude = sfVibratoAmplitude_in * sfPitchEffectsMax;
		sfVibratoPeakFraction = (float) pow(2.0, sfVibratoAmplitude/12.0) - (float)1.0;
		// 12 semitones in one octave. One octave requires a ration of 2.0.
		// This gives peak-peak pitch variation of 2 * sfVibratoAmplitude.
	}

	/*-------------------------------------------------------------------------*/

	void fvdUpdatePitchBend(float sfBendAmplitude_in )
	{
		static float sfPitchBend; // semitones.
		sfPitchBend = sfBendAmplitude_in * sfPitchEffectsMax;
		sfPitchBendRatio = (float) pow(2.0, sfPitchBend/12.0);
		// 12 semitones in one octave. One octave requires a ration of 2.0.
	}

	/*-------------------------------------------------------------------------*/

	void fvdUpdateNoteBend(float sfBendAmplitude_in )
	{
		float sfNoteBend; // semitones.
		sfNoteBend = sfBendAmplitude_in * sfPitchEffectsMax;
		sfNoteBendRatio = (float) pow(2.0, sfNoteBend/12.0);
		// 12 semitones in one octave. One octave requires a ration of 2.0.
	}

	/*-------------------------------------------------------------------------*/
};
#endif
