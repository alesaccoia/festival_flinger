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

#ifndef EVENTTREE_H
#define EVENTTREE_H


#include <iostream>

#include <iomanip>

using namespace std;


#include <assert.h>

#include "Event.h"
#include "EvTrDefs.h"

extern "C" {
#include "rb.h"
}


class EventTree {
	friend ostream &operator<<(ostream &os, const EventTree &t);

public:
	EventTree();
	EventTree(const EventTree &t);
	virtual ~EventTree();

	Event *GetEvents(unsigned long time);
	Event *GetEventsNoMod(unsigned long time) const;
	Event *NextEvent(void);
	Event *NextEvent(const Event *e) const;
	Event *NextEvents(void);
	Event *NextEvents(const Event *e) const;
	Event *PrevEvent(void);
	Event *PrevEvent(const Event *e) const;
	Event *PrevEvents(void);
	Event *PrevEvents(const Event *e) const;
	Event *GetFirstEvent(void);
	Event *GetFirstEvents(void);
	Event *GetLastEvent(void);
	Event *GetLastEvents(void);
	Event *GetFirstEvent(void) const;
	Event *GetFirstEvents(void) const;
	Event *GetLastEvent(void) const;
	Event *GetLastEvents(void) const;
	unsigned long GetStartTime(void);
	unsigned long GetEndTime(void);

	int Add(const EventTree &et, unsigned long start, double scalar = 1);
	EventTree *GetRange(unsigned long start, unsigned long end) const;
	int DeleteRange(unsigned long start, unsigned long end);

	void Grep(Event **events, int num_event, Event ***matched,
	    int *num_matched) const;

	Event *PutEvent(const Event &event);
	void RewindEvents(void) {curr_event = 0;}
	int DeleteEvent(Event &event);

	EventTree &operator=(const EventTree &t);
private:
	void DeleteTree(void);
	void CopyTree(const EventTree &t);

	EventTreeHead *head;
	Event *curr_event;
};

#endif
