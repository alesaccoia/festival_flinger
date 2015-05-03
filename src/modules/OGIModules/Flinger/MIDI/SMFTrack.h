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

#ifndef SMFTRACK_H
#define SMFTRACK_H

extern "C" {
#include "tcl.h"
}

#include <iostream>
#include <iomanip>

using namespace std;

const long StreamBlockSize = 256;

class SMFTrack {
	friend ostream &operator<<(ostream &os, const SMFTrack &t);
public:
	SMFTrack();
	SMFTrack(unsigned char *data, long len);
	SMFTrack(const SMFTrack &t);
	~SMFTrack();

	void StaticBuffer(unsigned char *data, long len, int init);
	void DynamicBuffer(void);

	long GetLength(void) const {return (length);}
	unsigned char GetRunningState(void) const {return (run_state);}
	const unsigned char *GetByte(void);
	const unsigned char *PeekByte(void) const;
	const unsigned char *GetData(long len);
	long GetVarValue(void);

	void SetRunningState(unsigned char rs) {run_state = rs;}
	int PutByte(unsigned char b);
	int PutData(unsigned char *data, long len);
	int PutFixValue(long val);

	void Empty(void);
	int Read(int fd);
	int Write(int fd) const;

	SMFTrack &operator=(const SMFTrack &t);
private:
	int IncreaseSize(long len);

	long allocated;
	long length;
	int static_buf;
	unsigned char run_state;
	unsigned char *start;
	unsigned char *pos;
	unsigned char *end;
};
#endif
