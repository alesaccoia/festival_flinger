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

#ifndef EVENT_H
#define EVENT_H


#include <iostream>
#include <iomanip>


using namespace std;

#include <stdlib.h>
#include "SMFTrack.h"
#include "EvTrDefs.h"

typedef enum {NOTYPE, NORMAL, META, NOTE, NOTEOFF, NOTEON, KEYPRESSURE,
    PARAMETER, PROGRAM, CHANNELPRESSURE, PITCHWHEEL, SYSTEMEXCLUSIVE,
    METASEQUENCENUMBER, METATEXT, METACOPYRIGHT, METASEQUENCENAME,
    METAINSTRUMENTNAME, METALYRIC, METAMARKER, METACUE, METACHANNELPREFIX,
    METAPORTNUMBER, METAENDOFTRACK, METATEMPO, METASMPTE, METATIME, METAKEY,
    METASEQUENCERSPECIFIC, METAUNKNOWN} EventType;

class Event {
	friend ostream &operator<<(ostream &os, const Event &e);

public:
        Event() : time(0L), wildcard(0), next_event(0), prev_event(0), node(0) {}

        virtual ~Event() {};
	Event(unsigned long t);
	Event(const Event &e);
	virtual Event *Dup(void) const {return new Event(*this);}

	int IsWildcard(void) const {
		return (wildcard != 0);
	}

	static const unsigned long WC_TIME;
protected:
	virtual int Equal(const Event *e) const;
	void SetWildcard(unsigned long bit) {
			wildcard |= bit;
	}
	void ClearWildcard(unsigned long bit) {
			wildcard &= ~bit;
	}
	int GetWildcard(int bit) const {
		return (wildcard & bit);
	}
public:
	unsigned long GetTime(void) const {
		if (GetWildcard(wc_time))
			return (WC_TIME);
		else
			return (time);
	}
	virtual EventType GetType(void) const {return (NOTYPE);}
	virtual char *GetTypeStr(void) const {return ("NoType");}
	virtual string GetEventStr(void) const;
	Event *GetNextEvent(void) const {return (next_event);}
	Event *GetPrevEvent(void) const {return (prev_event);}
	const EventTreeNode *GetEventTreeNode(void) const {return (node);}

	void SetTime(unsigned long t) {
		if (t == WC_TIME)
			SetWildcard(wc_time);
		else
			time = t;
	}
	void SetNextEvent(Event *n) {next_event = n;}
	void SetPrevEvent(Event *p) {prev_event = p;}
	void SetEventTreeNode(const EventTreeNode *n) {node = n;}

	Event &operator=(const Event &e);
	int operator==(const Event &e) const;
	void Print(ostream &os) const;

	virtual const char *SMFRead(SMFTrack &t) {
		// shut up a warning
		SMFTrack *dummy;
		dummy = &t;
		return (0);
	}
	virtual const char *SMFWrite(SMFTrack &t) const {
		// shut up a warning
		SMFTrack *dummy;
		dummy = &t;
		return (0);
	}

	unsigned long time;
	unsigned long wildcard;
	static const unsigned long wc_time;
	Event *next_event;
	Event *prev_event;
	const EventTreeNode *node;
};
#endif
