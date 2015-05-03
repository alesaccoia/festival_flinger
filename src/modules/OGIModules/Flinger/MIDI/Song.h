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


#ifndef SONG_H
#define SONG_H

extern "C" {
#include "tcl.h"
}
#include "EvntTree.h"

class Song {
	friend ostream &operator<<(ostream &os, const Song &s);
public:
	Song();
	Song(const Song &s);
	Song(short num);
	Song(short form, short div, short num);
	~Song();

	Event *GetEvents(short track, unsigned long time);
	Event *NextEvent(short track);
	Event *PrevEvent(short track);
	short GetFormat(void) const {return (format);}
	short GetDivision(void) const {return (division);}
	short GetNumTracks(void) const {return (num_tracks);}
	EventTree &GetTrack(short track);

	Event *PutEvent(short track, const Event &event);
	void RewindEvents(void);
	void RewindEvents(short track);
	int DeleteEvent(short track, Event &event);
	void SetFormat(short form) {format = form;}
	void SetDivision(short div) {division = div;}
	void SetNumTracks(short num);

	int Add(short track, EventTree &et, unsigned long start,
	    double scalar = 1.0);
	EventTree *GetRange(short track, unsigned long start,
	    unsigned long end) const;
	int DeleteRange(short track, unsigned long start, unsigned long end);

	int Grep(short track, Event **events, int num_event, Event ***matched,
	    int *num_matched);

	int Merge(short dest_track, const Song &song, short src_track);
	int Split(short src_track, Song &meta_song, short meta_track,
	    Song &normal_song, short normal_track) const;

	Song &operator=(const Song &s);

	int SMFRead(int fd);
	int SMFWrite(int fd);
	const char *GetError(void) const;

	void SetNotePair(int track_num, Event *event);
private:
	short format;
	short division;
	short num_tracks;
	EventTree **tracks;
	const char *errstr;
};
#endif
