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
#include "EvntUtil.h"
#include "Song.h"
#include "SMFHead.h"
#include "SMFTrack.h"
#include "Note.h"

//DJLB addition
#include "festival.h"
#include "MLyric.h"
//End DJLB addition

Song::Song() : format(0), division(120), num_tracks(0), tracks(0), errstr(0)
{
}

Song::Song(const Song &s) : format(s.format), division(s.division),
    num_tracks(s.num_tracks), errstr(0)
{
	int i;

	if (s.num_tracks > 0) {
		tracks = new EventTree *[s.num_tracks];
		assert(tracks != 0);
	}
	for (i = 0; i < s.num_tracks; i++) {
		tracks[i] = new EventTree(*s.tracks[i]);
		assert(tracks[1] != 0);
	}
}

Song::Song(short num) : division(120), num_tracks(num), errstr(0)
{
	int i;

	if (num <= 0) {
		num = 0;
		format = 0;
		tracks = 0;
	} else if (num > 1)
		format = 1;
	else
		format = 0;

	if (num > 0) {
		tracks = new EventTree *[num];
		assert(tracks != 0);
		for (i = 0; i < num; i++) {
			tracks[i] = new EventTree;
			assert(tracks[i] != 0);
		}
	}
}

Song::Song(short form, short div, short num) : format(form), division(div),
    num_tracks(num), errstr(0)
{
	int i;

	if (num <= 0) {
		num = 0;
		tracks = 0;
	} else if (num > 0) {
		tracks = new EventTree *[num];
		assert(tracks != 0);
		for (i = 0; i < num; i++) {
			tracks[i] = new EventTree;
			assert(tracks[i] != 0);
		}
	}
}

Song::~Song()
{
	int i;

	for (i = 0; i < num_tracks; i++)
		delete tracks[i];
	delete tracks;
}

Event *
Song::GetEvents(short track, unsigned long time)
{
	Event *ret;

	if (track < 0 || track >= num_tracks)
		return (0);
	else {
		ret = tracks[track]->GetEvents(time);
		return (ret);
	}
}

Event *
Song::NextEvent(short track)
{
	Event *ret;

	if (track < 0 || track >= num_tracks)
		return (0);
	else {
		ret = tracks[track]->NextEvent();
		return (ret);
	}
}

Event *
Song::PrevEvent(short track)
{
	Event *ret;

	if (track < 0 || track >= num_tracks)
		return (0);
	else {
		ret = tracks[track]->PrevEvent();
		return (ret);
	}
}

EventTree &
Song::GetTrack(short track)
{

	assert(track >= 0 && track < num_tracks);
	return (*tracks[track]);
}

Event *
Song::PutEvent(short track, const Event &event)
{
	Event *ret;

	if (track < 0 || track >= num_tracks)
		return (0);
	else {
		ret = tracks[track]->PutEvent(event);
		return (ret);
	}
}

void
Song::RewindEvents(void)
{
	int i;

	for (i = 0; i < num_tracks; i++)
		tracks[i]->RewindEvents();
}

void
Song::RewindEvents(short track)
{

	if (track < 0 || track >= num_tracks)
		return;
	tracks[track]->RewindEvents();
}

int
Song::DeleteEvent(short track, Event &event)
{
	int ret;

	if (track < 0 || track >= num_tracks)
		ret = 0;
	else
		ret = tracks[track]->DeleteEvent(event);
	return (ret);
}

void
Song::SetNumTracks(short num)
{
	EventTree **new_tracks;
	int i;

	new_tracks = 0;
	if (num > 0) {
		new_tracks = new EventTree *[num];
		assert(new_tracks != 0);
	}
	/* copy or create new tracks */
	for (i = 0; i < num; i++) {
		if (i < num_tracks)
			new_tracks[i] = tracks[i];
		else
			new_tracks[i] = new EventTree;
	}
	/* free any extra old tracks */
	if (i < num_tracks) {
		for (; i < num_tracks; i++)
			delete tracks[i];
	}
	delete tracks;
	tracks = new_tracks;
	num_tracks = num;
}

int
Song::Add(short track, EventTree &et, unsigned long start, double scalar)
{
	int ret;

	if (track < 0 || track >= num_tracks)
		return (0);
	ret = tracks[track]->Add(et, start, scalar);
	return (ret);
}

EventTree *
Song::GetRange(short track, unsigned long start, unsigned long end) const
{
	EventTree *ret;

	if (track < 0 || track >= num_tracks)
		ret = 0;
	ret = tracks[track]->GetRange(start, end);
	return (ret);
}

int
Song::DeleteRange(short track, unsigned long start, unsigned long end)
{
	int ret;

	if (track < 0 || track >= num_tracks)
		ret = 0;
	ret = tracks[track]->DeleteRange(start, end);
	return (ret);
}

Song &
Song::operator=(const Song &s)
{
	int i;

	if (num_tracks != 0) {
		for (i = 0; i < num_tracks; i++)
			delete tracks[i];
		delete tracks;
	}
	division = s.division;
	format = s.format;
	num_tracks = s.num_tracks;
	if (num_tracks > 0) {
		tracks = new EventTree *[num_tracks];
		assert(tracks != 0);
	}
	for (i = 0; i < num_tracks; i++) {
		tracks[i] = new EventTree(*s.tracks[i]);
		assert(tracks[i] != 0);
	}
	return (*this);
}

int
Song::Grep(short track, Event **events, int num_events, Event ***matched,
    int *num_matched)
{

	if (track < 0 || track >= num_tracks)
		return (0);
	tracks[track]->Grep(events, num_events, matched, num_matched);
	return (1);
}

int
Song::Merge(short dest_track, const Song &s, short src_track)
{
	double scalar;
	int ret;

	if (dest_track < 0 || dest_track >= num_tracks)
		return (0);
	if (src_track < 0 || src_track >= s.num_tracks)
		return (0);
	scalar = (double)division / s.division;

	ret = tracks[dest_track]->Add(*s.tracks[src_track], 0, scalar);
	return (ret);
}

int
Song::Split(short src_track, Song &meta_song, short meta_track,
    Song &normal_song, short normal_track) const
{
	double meta_scalar, normal_scalar;
	EventType etype;
	EventTree *tree;
	Event *e, *new_e, *new_e_ptr, *off, *new_off, *new_off_ptr;

	if (src_track < 0 || src_track >= num_tracks)
		return (0);
	if (meta_track < 0 || meta_track >= meta_song.num_tracks)
		return (0);
	if (normal_track < 0 || normal_track >= normal_song.num_tracks)
		return (0);

	meta_scalar = (double)meta_song.division / division;
	normal_scalar = (double)normal_song.division / division;

	tree = tracks[src_track];
	for (e = tree->GetFirstEvent(); e != 0; e = tree->NextEvent(e)) {
		etype = e->GetType();
		switch (etype) {
		case NOTEOFF:
		case NOTEON:
		case KEYPRESSURE:
		case PARAMETER:
		case PROGRAM:
		case CHANNELPRESSURE:
		case PITCHWHEEL:
		case SYSTEMEXCLUSIVE:
			/*
			 * skip note off side of note pairs since they've
			 * already been inserted
			 */
			if ((etype == NOTEOFF || (etype == NOTEON &&
			    ((NoteEvent *)e)->GetVelocity() == 0)) &&
			    ((NoteEvent *)e)->GetNotePair() != 0)
				break;

			new_e = e->Dup();
			if (normal_scalar != 1)
				new_e->SetTime((long)(e->GetTime() *
				    normal_scalar));
			new_e_ptr = normal_song.PutEvent(normal_track, *new_e);
			delete new_e;
			if (new_e_ptr == 0)
				break;

			off = 0;
			if (etype == NOTEON)
				off = ((NoteEvent *)e)->GetNotePair();
			if (off != 0) {
				new_off = off->Dup();
				if (normal_scalar != 1)
					new_off->SetTime((long)(off->GetTime()
					    * normal_scalar));
				new_off_ptr = normal_song.PutEvent(
				    normal_track, *new_off);
				delete new_off;
				if (new_off_ptr == 0)
					break;
				((NoteEvent *)new_e_ptr)->SetNotePair(
				    (NoteEvent *)new_off_ptr);
				((NoteEvent *)new_off_ptr)->SetNotePair(
				    (NoteEvent *)new_e_ptr);
			}
			break;
		case METAENDOFTRACK:
			new_e = e->Dup();
			if (normal_scalar != 1)
				new_e->SetTime((long)(e->GetTime() *
				    normal_scalar));
			normal_song.PutEvent(normal_track, *new_e);
			delete new_e;
			new_e = e->Dup();
			if (meta_scalar != 1)
				new_e->SetTime((long)(e->GetTime() *
				    meta_scalar));
			meta_song.PutEvent(meta_track, *new_e);
			delete new_e;
			break;
		default:
			new_e = e->Dup();
			if (meta_scalar != 1)
				new_e->SetTime((long)(e->GetTime() *
				    meta_scalar));
			meta_song.PutEvent(meta_track, *new_e);
			delete new_e;
			break;
		}
	}
	return (1);
}

int
Song::SMFRead(int fd)
{
	SMFHead head;
	SMFTrack track;
	unsigned long last_t;
	int i;
	EventType etype;
	Event *event;

	if (!head.Read(fd))
		return (0);
	if (num_tracks != 0) {
		for (i = 0; i < num_tracks; i++)
			delete tracks[i];
		delete tracks;
	}
	format = head.GetFormat();
	division = head.GetDivision();
	num_tracks = head.GetNumTracks();
	if (num_tracks == 0)
		tracks = 0;
	else {
		tracks = new EventTree *[num_tracks];
		assert(tracks != 0);
	}

//DJLB addition
		
		EST_String stLastEventType, stLastEventLyric;
		unsigned long ulnLastEventTime;
		int inReadStatus;
//End DJLB addition

		for (i = 0; i < num_tracks; i++) {
		if (!track.Read(fd))
			return (0);
		last_t = 0;
		tracks[i] = new EventTree;
		assert(tracks[i] != 0);

//DJLB modification
		bool boRepeatFailure = FALSE;
		while( (inReadStatus = finReadEventFromSMFTrack(
								&event, track, last_t, 1, errstr))!= FATAL )
		{
			if(inReadStatus == EVENT_READ_FAULTY)
			{
				if( !boRepeatFailure )
				{
					cout << "Error reading MIDI file after " << ulnLastEventTime
						<< " ticks on Track " << i << ":" << endl
						<< "   " << errstr << endl;
					cout << "The last successfully read lyric was '"
						<< stLastEventLyric << "'." << endl;
					boRepeatFailure = TRUE;
				}
			}
			else
			{
				boRepeatFailure = FALSE;
//End DJLB modification			
				Event *event_ptr;
														
			if ((event_ptr = tracks[i]->PutEvent(*event)) == 0) {
				delete event;
				continue;
			}
			delete event;
			etype = event_ptr->GetType();
			// put links on noteoffs
			if ((etype == NOTEON &&
			    ((NoteEvent *)event_ptr)->GetVelocity() == 0) ||
			    etype == NOTEOFF)
				SetNotePair(i, event_ptr);
//DJLB addition
				if(etype == METALYRIC)
					stLastEventLyric = ((MetaLyricEvent *)event_ptr)->GetString(); 

				stLastEventType = event_ptr->GetTypeStr();
				ulnLastEventTime = event_ptr->GetTime();
			}
//End DJLB addition
		}

		if (errstr != 0)
//DJLB addition/modification
		{
			cout << errstr << endl;
			cout << "Last event was type " << stLastEventType << " timed for "
					 << ulnLastEventTime << " ticks on Track " << i << endl;
			return (inReadStatus);
		}
	}
	RewindEvents();
	Event *pEvent;
	for (int trackNr=0; trackNr<num_tracks; trackNr++)
  {
    for (pEvent=tracks[trackNr]->NextEvent(); pEvent!=0; pEvent=tracks[trackNr]->NextEvent()) {
      std::cout << "Track " << trackNr << " event at time: " << pEvent->GetTime() << " type " << pEvent->GetType()  << endl;
    }
  }
  RewindEvents();
	return(GOOD);
//End DJLB addition/modification
}

int
Song::SMFWrite(int fd)
{
	SMFHead head;
	SMFTrack track;
	unsigned long last_t;
	int i;
	Event *e;

	head.SetFormat(format);
	head.SetDivision(division);
	head.SetNumTracks(num_tracks);

	if (!head.Write(fd))
		return (0);

	for (i = 0; i < num_tracks; i++) {
		track.Empty();
		last_t = 0;
		for (e = tracks[i]->GetFirstEvent(); e != 0;
		    e = tracks[i]->NextEvent(e))
			if (!WriteEventToSMFTrack(track, last_t, e, 1, errstr))
				return (0);
		if (!track.Write(fd))
			return (0);
	}
	return (1);
}

/* MWM
int
Song::SMFRead(Tcl_Channel channel)
{
	SMFHead head;
	SMFTrack track;
	unsigned long last_t;
	int i;
	EventType etype;
	Event *event;

	if (!head.Read(channel))
		return (0);
	if (num_tracks != 0) {
		for (i = 0; i < num_tracks; i++)
			delete tracks[i];
		delete tracks;
	}
	format = head.GetFormat();
	division = head.GetDivision();
	num_tracks = head.GetNumTracks();
	if (num_tracks == 0)
		tracks = 0;
	else {
		tracks = new EventTree *[num_tracks];
		assert(tracks != 0);
	}
	for (i = 0; i < num_tracks; i++) {
		if (!track.Read(channel))
			return (0);
		last_t = 0;
		tracks[i] = new EventTree;
		assert(tracks[i] != 0);
		while ((event = ReadEventFromSMFTrack(track, last_t, 1, errstr))
		    != 0) {
			Event *event_ptr;

			if ((event_ptr = tracks[i]->PutEvent(*event)) == 0) {
				delete event;
				continue;
			}
			delete event;
			etype = event_ptr->GetType();
			// put links on noteoffs
			if ((etype == NOTEON &&
			    ((NoteEvent *)event_ptr)->GetVelocity() == 0) ||
			    etype == NOTEOFF)
				SetNotePair(i, event_ptr);
		}
		if (errstr != 0)
			return (0);
	}
	RewindEvents();
	return (1);
}

int
Song::SMFWrite(Tcl_Channel channel)
{
	SMFHead head;
	SMFTrack track;
	unsigned long last_t;
	int i;
	Event *e;

	head.SetFormat(format);
	head.SetDivision(division);
	head.SetNumTracks(num_tracks);

	if (!head.Write(channel))
		return (0);

	for (i = 0; i < num_tracks; i++) {
		track.Empty();
		last_t = 0;
		for (e = tracks[i]->GetFirstEvent(); e != 0;
		    e = tracks[i]->NextEvent(e))
			if (!WriteEventToSMFTrack(track, last_t, e, 1, errstr))
				return (0);
		if (!track.Write(channel))
			return (0);
	}
	return (1);
}
MWM */

const char *
Song::GetError(void) const
{

	return (errstr);
}

void
Song::SetNotePair(int track_num, Event *e)
{
	unsigned char chan;
	unsigned char pitch;
	Event *eptr;

	chan = ((NormalEvent *)e)->GetChannel();
	pitch = ((NoteEvent *)e)->GetPitch();

	for (eptr = tracks[track_num]->PrevEvent(e); eptr != 0;
	    eptr = tracks[track_num]->PrevEvent(eptr)) {
		if (eptr->GetType() == NOTEON &&
		    ((NoteEvent *)eptr)->GetChannel() == chan &&
		    ((NoteEvent *)eptr)->GetPitch() == pitch &&
		    ((NoteEvent *)eptr)->GetNotePair() == 0)
			break;
	}
	if (eptr != 0) {
		((NoteEvent *)eptr)->SetNotePair((NoteEvent *)e);
		((NoteEvent *)e)->SetNotePair((NoteEvent *)eptr);
	}
}

ostream &
operator<<(ostream &os, const Song &s)
{
	int i;

	os << "Format: " << (int)s.format << " Division: " << (int)s.division
	    << " Number of Tracks: " << (int)s.num_tracks << "\n";
	for (i = 0; i < s.num_tracks; i++)
		os << "Track: " << i << "\n" << *s.tracks[i] << endl;
	return (os);
}
