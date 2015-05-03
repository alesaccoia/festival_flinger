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

#ifndef NOTEEVENT_H
#define NOTEEVENT_H

#include "NormEvnt.h"

class NoteEvent : public NormalEvent {
	friend ostream &operator<<(ostream &os, const NoteEvent &e);
public:
	NoteEvent();
	NoteEvent(unsigned long t, int chan, int pit, int vel, 
        const NoteEvent *np = 0);
    //DJLB 20-06-2008. Enharmonic mod.
	NoteEvent(unsigned long t, int chan, int pit, int vel, int crst, string cn,
	    const NoteEvent *np = 0);
    // End enharmonic mod.
	NoteEvent(const NoteEvent &e);

	virtual Event *Dup(void) const {return (new NoteEvent(*this));}

	virtual EventType GetType(void) const {return (NOTE);}
	virtual char *GetTypeStr(void) const {return ("NoteEvent");}
	//virtual char *GetEventStr(void) const;
	virtual string GetEventStr(void) const;
	int GetPitch(void) const {
		if (GetWildcard(wc_pitch))
			return (WC_PITCH);
		else
			return (pitch);
	}
	int GetVelocity(void) const {
		if (GetWildcard(wc_velocity))
			return (WC_VELOCITY);
		else
			return (velocity);
	}
	NoteEvent *GetNotePair(void) const {
		return (note_pair);
	}

	void SetPitch(int pit) {
		if (pit == WC_PITCH)
			SetWildcard(wc_pitch);
		else {
			pitch = pit;
			ClearWildcard(wc_pitch);
		}
	}
	void SetVelocity(int vel) {
		if (vel == WC_VELOCITY)
			SetWildcard(wc_velocity);
		else {
			velocity = vel;
			ClearWildcard(wc_velocity);
		}
	}
	virtual void SetNotePair(NoteEvent *np) {
		note_pair = np;
	}

    //DJLB 20-06-2008. Enharmonic mod.
    int GetChordRootScaleTone(void) const 
    {
		if (GetWildcard(wc_chord_root_scale_tone))
			return (WC_CHORD_ROOT_SCALE_TONE);
		else
			return (chord_root_scale_tone);
	}

    void SetChordRootScaleTone(int given_chord_root_scale_tone) 
       {
		if (given_chord_root_scale_tone == WC_CHORD_ROOT_SCALE_TONE)
			SetWildcard(wc_chord_root_scale_tone);
		else 
        {
			chord_root_scale_tone = given_chord_root_scale_tone;
			ClearWildcard(wc_chord_root_scale_tone);
		}
	}
    static const int WC_CHORD_ROOT_SCALE_TONE;
private:
    static const int wc_chord_root_scale_tone;
    int chord_root_scale_tone;

public:
    string GetChordName(void) const 
    {
		//if (GetWildcard(wc_chord_name))
		//	return (WC_CHORD_NAME);
		//else
			return (chord_name);
	}

    void SetChordName(string given_chord_name) 
       {
		//if (given_chord_name == WC_CHORD_NAME)
		//	SetWildcard(wc_chord_name);
		//else 
        //{
			chord_name = given_chord_name;
		//	ClearWildcard(wc_chord_name);
		//}
	}
    //static const string WC_CHORD_NAME;
private:
    //static const string wc_chord_name;
    string chord_name;
    //End Enharmonic mod.
public:
	NoteEvent &operator=(const NoteEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_PITCH;
	static const int WC_VELOCITY;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_pitch;
	static const unsigned long wc_velocity;
	unsigned char pitch;
	unsigned char velocity;
	NoteEvent *note_pair;
};
#endif
