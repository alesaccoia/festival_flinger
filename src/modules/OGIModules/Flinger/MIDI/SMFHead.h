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


#ifndef SMFHEAD_H
#define SMFHEAD_H

extern "C" {
#include "tcl.h"
}
#include <iostream>
using namespace std;

class SMFHead {
	friend ostream &operator<<(ostream &os, const SMFHead &h);
public:
	SMFHead();
	SMFHead(short form, short num, short div);
	SMFHead(const SMFHead &h);

	short GetFormat(void) const {return (format);}
	short GetNumTracks(void) const {return (num_tracks);}
	short GetDivision(void) const {return (division);}

	void SetFormat(short form) {format = form;}
	void SetNumTracks(short num) {num_tracks = num;}
	void SetDivision(short div) {division = div;}

	int Read(int fd);
	int Write(int fd) const;

	SMFHead &operator=(const SMFHead &h);
private:
	short format;
	short num_tracks;
	short division;
};
#endif
