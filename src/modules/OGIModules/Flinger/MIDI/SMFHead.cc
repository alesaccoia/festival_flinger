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

#include <string.h>
#include "SMFHead.h"
#include "SMFUtils.h"

SMFHead::SMFHead() : format(0), num_tracks(1), division(120)
{
}

SMFHead::SMFHead(short form, short num, short div) : format(form),
    num_tracks(num), division(div)
{
}

SMFHead::SMFHead(const SMFHead &h) : format(h.format), num_tracks(h.num_tracks),
    division(h.division)
{
}

SMFHead &
SMFHead::operator=(const SMFHead &h)
{

	format = h.format;
	num_tracks = h.num_tracks;
	division = h.division;
	return (*this);
}

int
SMFHead::Read(int fd)
{
	long length;
	char id[4];

	if (MRead(fd, id, 4) != 4)
		return (0);
	if (strncmp(id, "MThd", 4) != 0)
		return (0);
	if (MRead(fd, (char *)&length, 4) != 4)
		return (0);
	length = mtohl(length);
	/* length should be 6, but who really cares */
	if (MRead(fd, (char *)&format, 2) != 2)
		return (0);
	format = mtohs(format);
	if (MRead(fd, (char *)&num_tracks, 2) != 2)
		return (0);
	num_tracks = mtohs(num_tracks);
	if (MRead(fd, (char *)&division, 2) != 2)
		return (0);
	division = mtohs(division);
	/* a little sanity checking */
	if (format == 0 && num_tracks != 1)
		return (0);
	return (1);
}

int
SMFHead::Write(int fd) const
{
	long length;
	short s;

	if (MWrite(fd, "MThd", 4) != 4)
		return (0);
	length = htoml(6L);
	if (MWrite(fd, (char *)&length, 4) != 4)
		return (0);
	s = htoms(format);
	if (MWrite(fd, (char *)&s, 2) != 2)
		return (0);
	s = htoms(num_tracks);
	if (MWrite(fd, (char *)&s, 2) != 2)
		return (0);
	s = htoms(division);
	if (MWrite(fd, (char *)&s, 2) != 2)
		return (0);
	return (1);
}

/* MWM
int
SMFHead::Read(Tcl_Channel channel)
{
	long length;
	char id[4];

	if (MRead(channel, id, 4) != 4)
		return (0);
	if (strncmp(id, "MThd", 4) != 0)
		return (0);
	if (MRead(channel, (char *)&length, 4) != 4)
		return (0);
	length = mtohl(length);
	// length should be 6, but who really cares 
	if (MRead(channel, (char *)&format, 2) != 2)
		return (0);
	format = mtohs(format);
	if (MRead(channel, (char *)&num_tracks, 2) != 2)
		return (0);
	num_tracks = mtohs(num_tracks);
	if (MRead(channel, (char *)&division, 2) != 2)
		return (0);
	division = mtohs(division);
	// a little sanity checking 
	if (format == 0 && num_tracks != 1)
		return (0);
	return (1);
}

int
SMFHead::Write(Tcl_Channel channel) const
{
	long length;
	short s;

	if (MWrite(channel, "MThd", 4) != 4)
		return (0);
	length = htoml(6L);
	if (MWrite(channel, (char *)&length, 4) != 4)
		return (0);
	s = htoms(format);
	if (MWrite(channel, (char *)&s, 2) != 2)
		return (0);
	s = htoms(num_tracks);
	if (MWrite(channel, (char *)&s, 2) != 2)
		return (0);
	s = htoms(division);
	if (MWrite(channel, (char *)&s, 2) != 2)
		return (0);
	return (1);
}
MWM */
ostream &
operator<<(ostream &os, const SMFHead &h)
{

	os << "Format: " << h.format << " Num. Tracks: " << h.num_tracks
	    << " Division: " << h.division;
	return (os);
}
