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

#ifndef TCLMINTERP_H
#define TCLMINTERP_H

#include <iostream>

#include "Song.h"
#include "MidiDev.h"
#include "GusPatch.h"

extern "C" {
#include "tcl.h"
}

class TclmInterp {
public:
	TclmInterp();
	TclmInterp(const TclmInterp &ti);
	~TclmInterp();

	Song *GetSong(const char *key) const;
	char *AddSong(const Song *song);
	int DeleteSong(const char *key);
	MidiDevice *GetDevice(const char *key) const;
	char *AddDevice(const MidiDevice *dev);
	int DeleteDevice(const char *key);
	GusPatchFile *GetPatch(const char *key) const;
	char *AddPatch(const GusPatchFile *dev);
	int DeletePatch(const char *key);

	TclmInterp &operator=(const TclmInterp &ti);
private:
	Tcl_HashTable song_ht;
	Tcl_HashTable dev_ht;
	Tcl_HashTable patch_ht;
	int current_song;
	int current_dev;
	int current_patch;
	Event *next_event;
};
#endif
