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
Modified by DJLB to correct mid-file tempo event problem, and to provide a
	tolerance of one demisemiquaver on lyric/note timing.
 These modifications will be compiled instead of the original code they replace,
 if DJLB_ADDITIONS is defined.
*/

#include "MIDI/tcl.h"
#include "MIDI/AllEvent.h"
#include "EST_String.h"

extern void Tclm_ParseEvent(Event *e, bool boCommonEvent);//DJLB modification
extern void Tclm_ParseNoteOff(NoteOffEvent *e);
extern void Tclm_ParseNoteOn(NoteOnEvent *e);
extern void Tclm_ParseNote(NoteOnEvent *e);
extern void Tclm_ParseKeyPressure(KeyPressureEvent *e);
extern void Tclm_ParseParameter(ParameterEvent *e);
extern void Tclm_ParseProgram(ProgramEvent *e);
extern void Tclm_ParseChannelPressure(ChannelPressureEvent *e);
extern void Tclm_ParsePitchWheel(PitchWheelEvent *e);
extern void Tclm_ParseSystemExclusive(SystemExclusiveEvent *e);
extern void Tclm_ParseMetaSequenceNumber(MetaSequenceNumberEvent *e);
extern void Tclm_ParseMetaText(MetaTextEvent *e);
extern void Tclm_ParseMetaCopyright(MetaCopyrightEvent *e);
extern void Tclm_ParseMetaSequenceName(MetaSequenceNameEvent *e);
extern void Tclm_ParseMetaInstrumentName(MetaInstrumentNameEvent *e);
extern void Tclm_ParseMetaLyric(MetaLyricEvent *e);
extern void Tclm_ParseMetaMarker(MetaMarkerEvent *e);
extern void Tclm_ParseMetaCue(MetaCueEvent *e);
extern void Tclm_ParseMetaChannelPrefix(MetaChannelPrefixEvent *e);
extern void Tclm_ParseMetaPortNumber(MetaPortNumberEvent *e);
extern void Tclm_ParseMetaEndOfTrack(MetaEndOfTrackEvent *e);
extern void Tclm_ParseMetaTempo(MetaTempoEvent *e);
extern void Tclm_ParseMetaSMPTE(MetaSMPTEEvent *e);
extern void Tclm_ParseMetaTime(MetaTimeEvent *e);
extern void Tclm_ParseMetaKey(MetaKeyEvent *e);
extern void Tclm_ParseMetaSequencerSpecific(MetaSequencerSpecificEvent *e);
extern void Tclm_ParseMetaUnknown(MetaUnknownEvent *e);

void ParseAnyStoredNote();

#define LYRIC_TO_NOTE_DEFAULT_TIME_TOLERANCE 32 //demisemiquaver, or '1/32 note'.
// from tclmidi.h  MWM

extern void Tclm_ParseData(const unsigned char *data, long length);

/*
DJLB additions to fix mid-song tempo change problem.
*/

#include "GenList.h"

class StoredLyric : public MetaLyricEvent
{
public:
	StoredLyric();
	virtual ~StoredLyric();
	bool boType;// Intended to indicate common (true) or track-specific (false).
};

class StoredNote : public NoteOnEvent
{
public:
	StoredNote();
	virtual ~StoredNote();
};

typedef struct stTempoEvent
{
	unsigned long	ulTime;
	unsigned short	usTempo;
}tstTempoEvent;
extern tstTempoEvent *plstTempoEvents;

typedef struct stTimeSigEvent
{
	unsigned long	ulTime;
	int				inNumerator;
	int				inDenominator;
}tstTimeSigEvent;

extern tstTimeSigEvent *plstTimeSigEvents;

//End DJLB addition
