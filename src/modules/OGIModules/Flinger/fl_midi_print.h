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

#include "MIDI/tcl.h"
#include "MIDI/AllEvent.h"

extern void Tclm_PrintEvent(ostream &buf, Event *e, bool boCommonEvent);

extern string Tclm_PrintNoteOff(NoteOffEvent *e);
extern string Tclm_PrintNoteOn(NoteOnEvent *e);
extern string Tclm_PrintNote(NoteOnEvent *e);
extern string Tclm_PrintKeyPressure(KeyPressureEvent *e);
extern string Tclm_PrintParameter(ParameterEvent *e);
extern string Tclm_PrintProgram(ProgramEvent *e);
extern string Tclm_PrintChannelPressure(ChannelPressureEvent *e);
extern string Tclm_PrintPitchWheel(PitchWheelEvent *e);
extern string Tclm_PrintSystemExclusive(SystemExclusiveEvent *e);
extern string Tclm_PrintMetaSequenceNumber(MetaSequenceNumberEvent *e);
extern string Tclm_PrintMetaText(MetaTextEvent *e);
extern string Tclm_PrintMetaCopyright(MetaCopyrightEvent *e);
extern string Tclm_PrintMetaSequenceName(MetaSequenceNameEvent *e);
extern string Tclm_PrintMetaInstrumentName(MetaInstrumentNameEvent *e);
extern string Tclm_PrintMetaLyric(MetaLyricEvent *e);
extern string Tclm_PrintMetaMarker(MetaMarkerEvent *e);
extern string Tclm_PrintMetaCue(MetaCueEvent *e);
extern string Tclm_PrintMetaChannelPrefix(MetaChannelPrefixEvent *e);
extern string Tclm_PrintMetaPortNumber(MetaPortNumberEvent *e);
extern string Tclm_PrintMetaEndOfTrack(MetaEndOfTrackEvent *e);
extern string Tclm_PrintMetaTempo(MetaTempoEvent *e);
extern string Tclm_PrintMetaSMPTE(MetaSMPTEEvent *e);
extern string Tclm_PrintMetaTime(MetaTimeEvent *e);
extern string Tclm_PrintMetaKey(MetaKeyEvent *e);
extern string Tclm_PrintMetaSequencerSpecific(MetaSequencerSpecificEvent *e);
extern string Tclm_PrintMetaUnknown(MetaUnknownEvent *e);

// from tclmidi.h  MWM

extern void Tclm_PrintData(ostream &buf, const unsigned char *data, long length);

//DJLB addition
void fvdUpdateTimeSig(unsigned long lnTicksFromEdge, int inNumerator,
													int inDenominator );
const string fpchGetBarAndBeats(long lnTicksFromEdge );

void fvdResetTimeSignatureStores( void );
//End DJLB addition
