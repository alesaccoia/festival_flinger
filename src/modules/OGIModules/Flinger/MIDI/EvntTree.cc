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

#include <assert.h>
#include <stdlib.h>
#include "EvntTree.h"
#include "Note.h"
#include "MEOT.h"


EventTree::EventTree() : curr_event(0)
{

	head = make_rb();
	assert(head != 0);
}

EventTree::EventTree(const EventTree &t)
{

	CopyTree(t);
}

EventTree::~EventTree()
{

	DeleteTree();
}

/*
 * returns node for time, or node with smallest time greter than time
 * returns 0 if there are no nodes with time >= specified time
 */
Event *
EventTree::GetEvents(unsigned long time)
{
	EventTreeNode *n;
	int valid_event;

	n = rb_find_ukey(head, time);
	if (n == head) {
		curr_event = 0;
		return (0);
	}

	curr_event = (Event *)n->v.val;
	/*
	 * We need to check if the events we found are all the
	 * NoteOff half of Note events.  If so we have to discard
	 * it and find the next non-NoteOff event.
	 */
	valid_event = 0;
	do {
		/* check event type(s) */
		while (curr_event != 0) {
			/*
			 * It is a valid event if the following
			 * conditions do *not* occur.
			 * The event is a NoteOff event or a
			 * NoteOn event with a velocity of 0,
			 * and it has a NotePair.
			 */
			if (!((curr_event->GetType() == NOTEOFF ||
			    (curr_event->GetType() == NOTEON &&
			    ((NoteEvent *)curr_event)->GetVelocity() == 0)) &&
			    ((NoteEvent *)curr_event)->GetNotePair() != 0)) {
				valid_event = 1;
				break;
			}
			curr_event = curr_event->GetNextEvent();
		}
		/*
		 * get the next node
		 */
		if (!valid_event) {
			n = rb_next(n);
			if (n != head->c.list.flink && n != head)
				curr_event = (Event *)n->v.val;
			else {
				curr_event = 0;
				return (0);
			}
		}
	} while (!valid_event);
	return (curr_event);
}

Event *
EventTree::GetEventsNoMod(unsigned long time) const
{
	EventTreeNode *n;

	n = rb_find_ukey(head, time);
	if (n == head)
		return (0);

	return ((Event *)n->v.val);
}

Event *
EventTree::NextEvent(void)
{
	EventTreeNode *n;

	if (curr_event == 0) {
		if ((n = head->c.list.flink) == 0)
			return (0);
		curr_event = (Event *)n->v.val;
	} else if (curr_event->GetNextEvent() != 0) {
		curr_event = curr_event->GetNextEvent();
	} else {
		// move to next node
		n = rb_next(curr_event->GetEventTreeNode());
		if (n != head->c.list.flink && n != head)
			curr_event = (Event *)n->v.val;
		else
			curr_event = 0;
	}
	return (curr_event);
}

Event *
EventTree::NextEvent(const Event *event) const
{
	Event *e;
	const EventTreeNode *place;

	if (event == 0)
		return ((Event *)head->c.list.flink->v.val);

	// no-brainer if it isn't the last event in a node
	if ((e = event->GetNextEvent()) != 0)
		return (e);

	// otherwise we need to find it's node
	place = rb_next(event->GetEventTreeNode());
	if (place == 0 || place == head || place == head->c.list.flink)
		return (0);
	return ((Event *)place->v.val);
}

Event *
EventTree::NextEvents(void)
{
	EventTreeNode *n;

	if (curr_event == 0) {
		// at very beginning
		n = head->c.list.flink;
		if (n != 0)
			curr_event = (Event *)n->v.val;
	} else {
		// move to next node
		n = rb_next(curr_event->GetEventTreeNode());
		if (n != head->c.list.flink && n != head)
			curr_event = (Event *)n->v.val;
		else {
			// reached end
			curr_event = 0;
			return (curr_event);
		}
	}
	return (curr_event);
}

Event *
EventTree::NextEvents(const Event *event) const
{
	Event *ret;
	const EventTreeNode *place, *next_node;

	if (event == 0) {
		if (head->c.list.flink == head)
			ret = 0;
		else
			ret = (Event *)head->c.list.flink->v.val;
	} else {
		place = event->GetEventTreeNode();
		if (place == 0)
			ret = 0;
		else {
			next_node = rb_next(place);
			if (next_node == head->c.list.flink ||
			    next_node == head)
				ret = 0;
			ret = (Event *)next_node->v.val;
		}
	}
	return (ret);
}

Event *
EventTree::PrevEvent(void)
{
	EventTreeNode *n;

	if (curr_event == 0) {
		n = head->c.list.blink;
		if (n == head)
			return (0);
		for (curr_event = (Event *)n->v.val;
		    curr_event->GetNextEvent() != 0;
		    curr_event = curr_event->GetNextEvent());
	} else if (curr_event->GetPrevEvent() != 0) {
		curr_event = curr_event->GetPrevEvent();
	} else {
		n = rb_prev(curr_event->GetEventTreeNode());
		if (n == head || n == head->c.list.blink)
			curr_event = 0;
		else {
			for (curr_event = (Event *)n->v.val;
			    curr_event->GetNextEvent() != 0;
			    curr_event = curr_event->GetNextEvent());
		}
	}
	return (curr_event);
}

/*
 * assume that event is an event in the the EventTree, not a pointer
 * to an event with similar parameters.
 */
Event *
EventTree::PrevEvent(const Event *event) const
{
	Event *prev;
	const EventTreeNode *place;

	if (event == 0) {
		if (head->c.list.blink == head)
			return (0);
		else
			return ((Event *)head->c.list.blink->v.val);
	}

	// a no-brainer if it's not the first event
	if ((prev = event->GetPrevEvent()) != 0)
		return (prev);

	// we're going to need to find the node to back up
	place = event->GetEventTreeNode();
	if (place == 0)
		return (0);
	place = rb_prev(place);
	if (place == head || place == head->c.list.blink)
		// we wrapped
		return (0);
	// find last event in new node
	for (prev = (Event *)place->v.val; prev->GetNextEvent() != 0;
	    prev = prev->GetNextEvent());
	return (prev);
}

Event *
EventTree::PrevEvents(void)
{
	EventTreeNode *n;

	if (curr_event == 0) {
		n = head->c.list.blink;
		if (n == head)
			return (0);
		curr_event = (Event *)n->v.val;
	} else {
		n = rb_prev(curr_event->GetEventTreeNode());
		if (n == head || n == head->c.list.blink)
			// we wrapped
			curr_event = 0;
		else
			curr_event = (Event *)n->v.val;
	}
	return (curr_event);
}

Event *
EventTree::PrevEvents(const Event *event) const
{
	const EventTreeNode *place;

	if (event == 0) {
		if (head->c.list.blink == head)
			return (0);
		else
			return ((Event *)head->c.list.blink->v.val);
	}

	place = event->GetEventTreeNode();
	place = rb_prev(place);
	if (place == head->c.list.blink || place == head)
		return (0);

	return ((Event *)place->v.val);
}

Event *
EventTree::GetFirstEvent(void)
{
	EventTreeNode *n;

	n = head->c.list.flink;
	if (n == 0 || n == head)
		curr_event = 0;
	else
		curr_event = (Event *)n->v.val;
	return (curr_event);
}

Event *
EventTree::GetFirstEvents(void)
{

	return (GetFirstEvent());
}

Event *
EventTree::GetLastEvent(void)
{
	EventTreeNode *n;

	n = head->c.list.blink;

	if (n == 0 || n == head)
		curr_event = 0;
	else {
		for (curr_event = (Event *)n->v.val;
		    curr_event->GetNextEvent() != 0;
		    curr_event = curr_event->GetNextEvent());
	}
	return (curr_event);
}

Event *
EventTree::GetLastEvents(void)
{
	EventTreeNode *n;

	n = head->c.list.blink;
	if (n == 0 || n == head)
		curr_event = 0;
	else
		curr_event = (Event *)n->v.val;
	return (curr_event);
}

Event *
EventTree::GetFirstEvent(void) const
{
	EventTreeNode *n;
	Event *ret;

	n = head->c.list.flink;
	if (n == 0 || n == head)
		ret = 0;
	else
		ret = (Event *)n->v.val;
	return (ret);
}

Event *
EventTree::GetFirstEvents(void) const
{

	return (GetFirstEvent());
}

Event *
EventTree::GetLastEvent(void) const
{
	EventTreeNode *n;
	Event *ret;

	n = head->c.list.blink;

	if (n == 0 || n == head)
		ret = 0;
	else {
		for (ret = (Event *)n->v.val; ret->GetNextEvent() != 0;
		    ret = ret->GetNextEvent());
	}
	return (ret);
}

Event *
EventTree::GetLastEvents(void) const
{
	EventTreeNode *n;
	Event *ret;

	n = head->c.list.blink;
	if (n == 0 || n == head)
		ret = 0;
	else
		ret = (Event *)n->v.val;
	return (ret);
}

unsigned long
EventTree::GetStartTime(void)
{
	unsigned long ret;

	if (head->c.list.flink == 0 || head->c.list.flink == head)
		ret = 0;
	else
		ret = head->c.list.flink->k.ukey;
	return (ret);
}

unsigned long
EventTree::GetEndTime(void)
{
	unsigned long ret;

	if (head->c.list.blink == 0 || head->c.list.blink == head)
		ret = 0;
	else
		ret = head->c.list.blink->k.ukey;
	return (ret);
}

int
EventTree::Add(const EventTree &event_tree, unsigned long start, double scalar)
{
	EventType etype;
	EventTree *et = (EventTree *)&event_tree;
	Event *e, *new_e, *mod_e, *off, *new_off;

	for (e = et->GetFirstEvent(); e != 0; e = et->NextEvent(e)) {
		etype = e->GetType();
		// skip note off side of note pairs since they've
		// already been inserted
		if ((etype == NOTEOFF || (etype == NOTEON &&
		    ((NoteEvent *)e)->GetVelocity() == 0)) &&
		    ((NoteEvent *)e)->GetNotePair() != 0)
			continue;
		if ((mod_e = e->Dup()) == 0)
			return (0);
		mod_e->SetTime((long)(e->GetTime() * scalar) + start);
		new_e = PutEvent(*mod_e);
		delete mod_e;
		/* don't die just because there is a duplicate event */
		if (new_e == 0)
			continue;
		off = 0;
		if (etype == NOTEON)
			off = ((NoteEvent *)e)->GetNotePair();
		if (off != 0) {
			if ((mod_e = off->Dup()) == 0)
				return (0);
			mod_e->SetTime((long)(off->GetTime() * scalar) + start);
			new_off = PutEvent(*mod_e);
			delete mod_e;
			if (new_off == 0)
				return (0);
			((NoteEvent *)new_e)->SetNotePair((NoteEvent *)new_off);
			((NoteEvent *)new_off)->SetNotePair((NoteEvent *)new_e);
		}
	}
	return (1);
}

EventTree *
EventTree::GetRange(unsigned long start, unsigned long end) const
{
	EventType etype;
	EventTree *nt, *this_ptr;
	Event *e, *tmp_e;
	NoteEvent *np, *new_e1, *new_e2, *tmp_np;

	if (start >= end)
		return (0);
	this_ptr = (EventTree *)this;
	nt = new EventTree;
	if (nt == 0)
		return (0);
	e = this_ptr->GetEvents(start);
	for (e = this_ptr->GetEvents(start); e != 0 && e->GetTime() < end;
	    e = this_ptr->NextEvent(e)) {
		tmp_e = e->Dup();
		assert(tmp_e != 0);
		tmp_e->SetTime(e->GetTime() - start);
		etype = e->GetType();
		// skip note off halfs
		if ((etype == NOTEOFF || (etype == NOTEON &&
		    ((NoteEvent *)e)->GetVelocity() == 0)) &&
		    (np = ((NoteEvent *)e)->GetNotePair()) != 0) {
			delete tmp_e;
			continue;
		}
		np = 0;
		if (etype == NOTEON && ((NoteEvent *)e)->GetVelocity() != 0)
			np = ((NoteEvent *)e)->GetNotePair();
		if (np != 0) {
			// copy in note off half and link
			tmp_np = (NoteEvent *)np->Dup();
			assert(tmp_np != 0);
			tmp_np->SetTime(np->GetTime() - start);
			if ((new_e1 = (NoteEvent *)nt->PutEvent(*tmp_e)) == 0)
				return (0);
			if ((new_e2 = (NoteEvent *)nt->PutEvent(*tmp_np)) == 0)
					return (0);
			new_e1->SetNotePair(new_e2);
			new_e2->SetNotePair(new_e1);
			delete tmp_e;
			delete tmp_np;
		} else {
			if (nt->PutEvent(*tmp_e) == 0)
				return (0);
			delete tmp_e;
		}
	}
	return (nt);
}

int
EventTree::DeleteRange(unsigned long start, unsigned long end)
{
	EventType etype;
	Event *e, *next, *np;
	int cont;

	if (start >= end)
		return (0);

	e = GetEvents(start);
	while (e != 0 && e->GetTime() < end) {
		etype = e->GetType();
		cont = 0;
		if ((etype == NOTEOFF || (etype == NOTEON &&
		    ((NoteEvent *)e)->GetVelocity() == 0))) {
			np = ((NoteEvent *)e)->GetNotePair();
			if (np != 0 && np->GetTime() < start) {
				// don't remove noteoff halfs when note
				// starts before range
				e = NextEvent(e);
				cont = 1;
			}
		} else if (etype == NOTEON && ((NoteEvent *)e)->GetVelocity()
		    != 0) {
			np = ((NoteEvent *)e)->GetNotePair();
			if (np != 0 && np->GetTime() >= end) {
				// remove note off even though it falls outside
				// of range
				if (!DeleteEvent(*np))
					return (0);
			}
		}
		if (cont)
			continue;
		next = NextEvent(e);
		if (!DeleteEvent(*e))
			return (0);
		e = next;
	}
	return (1);
}

void
EventTree::Grep(Event **events, int num_event, Event ***matched,
    int *num_matched) const
{
	int i, j;
	Event *e, **found, **new_found;
	int num_found;

	found = 0;
	num_found = 0;
	/*
	 * If all the events we are trying to match do not have
	 * wildcard times, we can skip searching the entire track.
	 */
	for (i = 0; i < num_event; i++)
		if (events[i]->GetTime() == Event::WC_TIME)
			break;
	if (i == num_event) {
		Event *e;
		// They all have fixed times
		for (i = 0; i < num_event; i++) {
			e = GetEventsNoMod(events[i]->GetTime());
			while (e != 0) {
				if (*e == *events[i]) {
					new_found = new Event *[num_found + 1];
					assert(new_found != 0);
					for (j = 0; j < num_found; j++)
						new_found[j] = found[j];
					new_found[j] = e;
					delete found;
					found = new_found;
					num_found++;
				}
				e = e->GetNextEvent();
			}
		}
	} else {
		/*
		 * At least one event is has a wildcard time, thus we
		 * must search the entire track.
		 */
		for (e = GetFirstEvent(); e != 0; e = NextEvent(e)) {
			for (i = 0; i < num_event; i++) {
				if (*e == *events[i]) {
					new_found = new Event *[num_found + 1];
					assert(new_found != 0);
					for (j = 0; j < num_found; j++)
						new_found[j] = found[j];
					new_found[j] = e;
					delete found;
					found = new_found;
					num_found++;
				}
			}
		}
	}
	*matched = found;
	*num_matched = num_found;
}

Event *
EventTree::PutEvent(const Event &event)
{
	unsigned long time;
	EventTreeNode *place;
	Event *e, *new_event, *first, *next, *prev, *last;

	time = event.GetTime();
	/* find where it is or where it should go */
	place = rb_find_ukey(head, time);
	assert(place != 0);
	e = (Event *)place->v.val;
	if (e != 0 && e->GetTime() == time) {
		/* find end of event list or duplicate */
		first = e;
		for (; e->GetNextEvent() != 0 && !(*e == event);
		    e = e->GetNextEvent());
		/* don't insert duplicate events */
		if (*e == event)
			return (0);
		last = e;
		new_event = event.Dup();
		assert(new_event != 0);
		switch (new_event->GetType()) {
		case NOTEOFF:
			/* at beginning, but after other existing NoteOffs */
			for (e = first; e != 0 && e->GetType() == NOTEOFF;
			    e = e->GetNextEvent());
			if (e == 0) {
				new_event->SetNextEvent(0);
				new_event->SetPrevEvent(last);
				last->SetNextEvent(new_event);
			} else {
				prev = e->GetPrevEvent();
				new_event->SetNextEvent(e);
				new_event->SetPrevEvent(prev);
				e->SetPrevEvent(new_event);
				if (prev == 0)
					place->v.val = (char *)new_event;
				else
					prev->SetNextEvent(new_event);
			}
			break;
		case NOTEON:
			/* at end, but before EOT if there is one */
			if (last->GetType() == METAENDOFTRACK) {
				prev = last->GetPrevEvent();
				new_event->SetNextEvent(last);
				new_event->SetPrevEvent(prev);
				last->SetPrevEvent(new_event);
				if (prev == 0)
					place->v.val = (char *)new_event;
				else
					prev->SetNextEvent(new_event);
			} else {
				new_event->SetNextEvent(0);
				new_event->SetPrevEvent(last);
				last->SetNextEvent(new_event);
				break;
			}
			break;
		case METAENDOFTRACK:
			/* always at very end */
			new_event->SetNextEvent(0);
			new_event->SetPrevEvent(last);
			last->SetNextEvent(new_event);
			break;
		default:
			/* all others go in middle */
			/* search from rear to preserve order */
			for (e = last; e != 0 &&
			    (e->GetType() == METAENDOFTRACK ||
			    e->GetType() == NOTEON); e = e->GetPrevEvent());
			if (e == 0) {
				new_event->SetPrevEvent(0);
				new_event->SetNextEvent(first);
				place->v.val = (char *)new_event;
				first->SetPrevEvent(new_event);
			} else {
				next = e->GetNextEvent();
				new_event->SetPrevEvent(e);
				new_event->SetNextEvent(next);
				e->SetNextEvent(new_event);
				if (next != 0)
					next->SetPrevEvent(new_event);
			}
			break;
		}
	} else {
		new_event = event.Dup();
		assert(new_event != 0);
		place = rb_insert_b(place, (char *)time, (char *)new_event);
	}
	curr_event = new_event;
	new_event->SetEventTreeNode(place);
	return (new_event);
}

int
EventTree::DeleteEvent(Event &event)
{
	Event *e, *events, *prev;
	EventTreeNode *n, *next_n;

	n = (EventTreeNode *)event.GetEventTreeNode();
	if (n != 0) {
		e = &event;
		prev = e->GetPrevEvent();
	} else {
		// not real event, just a copy
		n = rb_find_ukey(head, event.GetTime());
		if (n == 0)
			return (0);
		events = (Event *)n->v.val;
		if (events == 0 || events->GetTime() != event.GetTime())
			return (0);
		/* find matching event */
		for (e = events, prev = 0; e != 0 && !(*e == event);
		    prev = e, e = e->GetNextEvent());
		if (e == 0)
			return (0);
	}
	if (prev != 0 || e->GetNextEvent() != 0) {
		// there are other events in this node
		if (e->GetNextEvent() != 0)
			e->GetNextEvent()->SetPrevEvent(prev);

		if (prev != 0)
			prev->SetNextEvent(e->GetNextEvent());
		else
			n->v.val = (char *)e->GetNextEvent();
		// check to see if curr_event needs to move
		if (curr_event == e) {
			if ((curr_event = e->GetNextEvent()) == 0) {
				next_n = rb_next(e->GetEventTreeNode());
				if (next_n != head->c.list.flink &&
				    next_n != head)
					curr_event = (Event *)next_n->v.val;
				else
					curr_event = 0;
			}
			
		}
	} else {
		// This is the only event in the node
		if (curr_event == e) {
			next_n = rb_next(e->GetEventTreeNode());
			if (next_n != head->c.list.flink && n != head)
				curr_event = (Event *)next_n->v.val;
			else
				curr_event = 0;
		}
		rb_delete_node(n);
	}
	delete e;
	return (1);
}

EventTree &
EventTree::operator=(const EventTree &t)
{

	CopyTree(t);
	return (*this);
}

/* private functions */
void
EventTree::DeleteTree(void)
{
	Event *e;

	RewindEvents();
	/* get each event in order and delete it */
	while ((e = NextEvent()) != 0)
		DeleteEvent(*e);
		
	/* then free head */
	free(head);
	head = 0;
	curr_event = 0;
}

void
EventTree::CopyTree(const EventTree &t)
{
	Event *e, *new_e;

	/* we must clear out the current tree */
	DeleteTree();

	head = make_rb();
	if (t.head->c.list.flink == 0)
		return;
	for (e = t.GetFirstEvent(); e != 0; e = t.NextEvent(e)) {
		new_e = PutEvent(*e);
		assert(new_e != 0);
		if (t.curr_event == e)
			curr_event = new_e;
	}
}

ostream &
operator<<(ostream &os, const EventTree &t)
{
	Event *e;

	for (e = t.GetFirstEvent(); e != 0; e = t.NextEvent(e))
		os << *e << endl;
	return (os);
}
