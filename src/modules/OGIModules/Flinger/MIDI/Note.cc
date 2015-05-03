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

#include "Note.h"
//#include <strstream>
#include <sstream>
using namespace std;

//Changes by DJLB 20-06-2008.
// 'chord_root_scale_tone' etc. and 'chord_name etc. as all new, 
//  providing new characteristics of the note event.

const unsigned long NoteEvent::wc_pitch = (1 << 2);
const unsigned long NoteEvent::wc_velocity = (1 << 3);
const int NoteEvent::wc_chord_root_scale_tone = (1 << 3);

const int NoteEvent::WC_PITCH = -1;
const int NoteEvent::WC_VELOCITY = -1;
const int NoteEvent::WC_CHORD_ROOT_SCALE_TONE = -2;

NoteEvent::NoteEvent() : pitch(0), velocity(0), note_pair(0), chord_root_scale_tone(-1)
{
}

NoteEvent::NoteEvent(unsigned long t, int chan, int pit, int vel,
    const NoteEvent *np) : NormalEvent(t, chan), pitch(pit), velocity(vel),
    note_pair((NoteEvent *)np)
{
	if (pit == WC_PITCH)
		SetWildcard(wc_pitch);
	if (vel == WC_VELOCITY)
		SetWildcard(wc_velocity);
}

NoteEvent::NoteEvent(unsigned long t, int chan, int pit, int vel, int crst, 
                     string cn,
    const NoteEvent *np) : NormalEvent(t, chan), pitch(pit), velocity(vel),
    chord_root_scale_tone(crst), chord_name(cn), note_pair((NoteEvent *)np)
{
	if (pit == WC_PITCH)
		SetWildcard(wc_pitch);
	if (vel == WC_VELOCITY)
		SetWildcard(wc_velocity);
    if (crst == WC_CHORD_ROOT_SCALE_TONE)
        SetWildcard(wc_chord_root_scale_tone);
 }

//NoteEvent::NoteEvent(const NoteEvent &e) : NormalEvent(e),
//    pitch(e.pitch), velocity(e.velocity), 
//   note_pair(e.note_pair)
//{
//}

NoteEvent::NoteEvent(const NoteEvent &e) : NormalEvent(e),
    pitch(e.pitch), velocity(e.velocity), 
    chord_root_scale_tone(e.chord_root_scale_tone),
    chord_name(e.chord_name),
    note_pair(e.note_pair)
{
}

NoteEvent &
NoteEvent::operator=(const NoteEvent &e)
{

	(NormalEvent)*this = (NormalEvent)e;
	pitch = e.pitch;
	velocity = e.velocity;
    chord_root_scale_tone = e.chord_root_scale_tone;
    chord_name = e.chord_name;
	note_pair = e.note_pair;
	return (*this);
}

//char *string
string
NoteEvent::GetEventStr(void) const
{
	//ostrstream buf;
	ostringstream buf;
	string tbuf;

	tbuf = NormalEvent::GetEventStr();
	buf << tbuf << " Pitch: ";
	if (GetWildcard(wc_pitch))
		buf << "*";
	else
		buf << (int)pitch;
	buf << " Velocity: ";
	if (GetWildcard(wc_velocity))
		buf << "*";
	else
		buf << (int)velocity;

    buf << " ChordRootScaleTone: ";
	if (GetWildcard(wc_chord_root_scale_tone))
		buf << "*";
	else
		buf << (int)chord_root_scale_tone;

    buf << " ChordName: ";
	buf << chord_name;

	if (note_pair != 0) {
		buf << " NotePair Time: ";
		if (note_pair->GetTime() == WC_TIME)
			buf << "*";
		else
			buf << note_pair->GetTime();
	}
	buf << ends;
	//delete tbuf;
	return (buf.str());
}

const char *
NoteEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete NoteEvent - missing pitch");
	pitch = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete NoteEvent - missing velocity");
	velocity = *ptr;
	return (0);
}

const char *
NoteEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutByte(pitch))
		return ("Out of memory");
	if (!t.PutByte(velocity))
		return ("Out of memory");
	return (0);
}

int
NoteEvent::Equal(const Event *e) const
{
	NoteEvent *eptr = (NoteEvent *)e;

	// make sure note isn't = to note on
	if ((note_pair == 0 && eptr->note_pair != 0) ||
	    (note_pair != 0 && eptr->note_pair == 0))
		return (0);
	if (!NormalEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_pitch) && !GetWildcard(wc_pitch) &&
	    pitch != eptr->pitch)
		return (0);
	if (!eptr->GetWildcard(wc_velocity) && !GetWildcard(wc_velocity) &&
	    velocity != eptr->velocity)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const NoteEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
