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

#ifndef SMFUTILS_H
#define SMFUTILS_H

extern "C" {
#include "tcl.h"
}

/*
 * convert midi order to host order and vice-versa
 * Trying a new inline function version.  This generates silly warnings,
 * but I believe it is clearer and faster.
 */
#if WORDS_BIGENDIAN

inline long
mtohl(long l)
{

	return (l);
}

inline long
htoml(long l)
{

	return (l);
}

inline short
mtohs(short s)
{

	return (s);
}

inline short
htoms(short s)
{

	return (s);
}

#else

inline long
mtohl(long l)
{
	union {
		long l;
		char c[4];
	} swap;
	char tmp_c;

	swap.l = l;
	tmp_c = swap.c[0];
	swap.c[0] = swap.c[3];
	swap.c[3] = tmp_c;
	tmp_c = swap.c[1];
	swap.c[1] = swap.c[2];
	swap.c[2] = tmp_c;
	return (swap.l);
}

inline long
htoml(long l)
{

	union {
		long l;
		char c[4];
	} swap;
	char tmp_c;

	swap.l = l;
	tmp_c = swap.c[0];
	swap.c[0] = swap.c[3];
	swap.c[3] = tmp_c;
	tmp_c = swap.c[1];
	swap.c[1] = swap.c[2];
	swap.c[2] = tmp_c;
	return (swap.l);
}

inline short
mtohs(short s)
{
	union {
		short s;
		char c[2];
	} swap;
	char tmp_c;

	swap.s = s;
	tmp_c = swap.c[0];
	swap.c[0] = swap.c[1];
	swap.c[1] = tmp_c;
	return (swap.s);
}

inline short
htoms(short s)
{

	union {
		short s;
		char c[2];
	} swap;
	char tmp_c;

	swap.s = s;
	tmp_c = swap.c[0];
	swap.c[0] = swap.c[1];
	swap.c[1] = tmp_c;
	return (swap.s);
}
#endif


extern long MRead(int fd, char *data, long len);
extern long MWrite(int fd, char *data, long len);
extern long VarToFix(unsigned char *var, int *len);
extern int FixToVar(long fix, unsigned char *var);
#endif
