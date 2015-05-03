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
#include <string.h>

#include "SMFTrack.h"
#include "SMFUtils.h"

using namespace std;

SMFTrack::SMFTrack() : allocated(0L), length(0L), static_buf(0), run_state(0),
    start(0), pos(0), end(0)
{
}

SMFTrack::SMFTrack(unsigned char *data, long len) : length(len), static_buf(0),
    run_state(0)
{

	allocated = (len / StreamBlockSize + 1) * StreamBlockSize;
	start = new unsigned char[allocated];
	assert (start != 0);
	pos = start;
	memcpy(start, data, len);
	end = &start[len];
}

SMFTrack::SMFTrack(const SMFTrack &t) : allocated(t.allocated),
    length(t.length), static_buf(t.static_buf), run_state(t.run_state)
{

	if (t.static_buf)
		start = t.start;
	else {
		start = new unsigned char[allocated];
		assert (start != 0);
		memcpy(start, t.start, length);
	}
	pos = start + (t.pos - t.start);
	end = start + (t.end - t.start);
}

SMFTrack::~SMFTrack()
{

	if (start != 0 && !static_buf)
		delete start;
}

/*
 * SGI's C++ compiler can't handle inline function with
 * ifs in them, thus these are outlined
 */
const unsigned char *
SMFTrack::GetByte(void)
{

	if (pos == end)
		return (0);
	else
		return (pos++);
}

const unsigned char *
SMFTrack::PeekByte(void) const
{

	if (pos == end)
		return (0);
	else
		return (pos);
}

void
SMFTrack::StaticBuffer(unsigned char *buf, long len, int init)
{

	if (!static_buf)
		delete start;
	start = buf;
	allocated = len;
	pos = start;
	if (init) {
		length = len;
		end = &start[length];
	} else {
		length = 0;
		end = start;
	}
	static_buf = 1;
}

void
SMFTrack::DynamicBuffer(void)
{

	start = 0;
	length = 0;
	pos = 0;
	end = 0;
	allocated = 0;
	static_buf = 0;
}

const unsigned char *
SMFTrack::GetData(long len)
{
	unsigned char *retptr;

	if (pos + len > end)
		return (0);
	else {
		retptr = pos;
		pos += len;
		return (retptr);
	}
}

long
SMFTrack::GetVarValue(void)
{
	long fix;
	unsigned char *init_pos;

	fix = 0;
	init_pos = pos;
	if (pos == end)
		return (-1);
	if (*pos & 0x80)
		do {
			if (pos == end) {
				pos = init_pos;
				return (-1);
			}
			fix = (fix << 7) + (*pos &0x7f);
		} while (*pos++ & 0x80);
	else
		fix = *pos++;
	return (fix);
}

int
SMFTrack::PutByte(unsigned char b)
{

	if (length + 1 > allocated) {
		if (!IncreaseSize(1))
			return (0);
	}
	*end++ = b;
	length++;
	return (1);
}

int
SMFTrack::PutData(unsigned char *data, long len)
{

	if (length + len > allocated) {
		if (static_buf)
			return (0);
		else {
			if (!IncreaseSize(len))
				return (0);
		}
	}
	memcpy(end, data, len);
	end += len;
	length += len;
	return (1);
}

int
SMFTrack::PutFixValue(long val)
{
	unsigned char buf[4];
	unsigned char *bptr;

	buf[0] = buf[1] = buf[2] = buf[3] = 0;
	bptr = buf;
	*bptr++ = val & 0x7f;
	while ((val >>= 7) > 0) {
		*bptr |= 0x80;
		*bptr++ += (val & 0x7f);
	}
	do {
		if (!PutByte(*--bptr))
			return (0);
	} while (bptr != buf);
	return (1);
}

SMFTrack &
SMFTrack::operator=(const SMFTrack &t)
{

	if (start != 0 && !static_buf)
		delete start;

	allocated = t.allocated;
	length = t.length;
	run_state = t.run_state;
	static_buf = t.static_buf;
	if (t.static_buf)
		start = t.start;
	else {
		start = new unsigned char[allocated];
		assert (start != 0);
		memcpy(start, t.start, length);
	}
	pos = start + (t.pos - t.start);
	end = start + (t.end - t.start);
	return (*this);
}

void
SMFTrack::Empty(void)
{

	if (start != 0 && !static_buf)
		delete start;
	allocated = 0;
	length = 0;
	start = 0;
	pos = 0;
	end = 0;
	run_state = 0;
}

int
SMFTrack::Read(int fd)
{
	char id[4];

	Empty();
	if (MRead(fd, id, 4) != 4)
		return (0);
	if (strncmp(id, "MTrk", 4) != 0)
		return (0);
	if (MRead(fd, (char *)&length, 4) != 4)
		return (0);
	length = mtohl(length);
	if (!static_buf)
		if (!IncreaseSize(length))
			return (0);
	if (MRead(fd, (char *)start, length) != length)
		return (0);
	pos = start;
	end = &start[length];
	return (1);
}

int
SMFTrack::Write(int fd) const
{
	long mlength;

	if (MWrite(fd, "MTrk", 4) != 4)
		return (0);
	mlength = htoml(length);
	if (MWrite(fd, (char *)&mlength, 4) != 4)
		return (0);
	if (MWrite(fd, (char *)start, length) != length)
		return (0);
	return (1);
}

/* MWM
int
SMFTrack::Read(Tcl_Channel channel)
{
	char id[4];

	Empty();
	if (MRead(channel, id, 4) != 4)
		return (0);
	if (strncmp(id, "MTrk", 4) != 0)
		return (0);
	if (MRead(channel, (char *)&length, 4) != 4)
		return (0);
	length = mtohl(length);
	if (!static_buf)
		if (!IncreaseSize(length))
			return (0);
	if (MRead(channel, (char *)start, length) != length)
		return (0);
	pos = start;
	end = &start[length];
	return (1);
}

int
SMFTrack::Write(Tcl_Channel channel) const
{
	long mlength;

	if (MWrite(channel, "MTrk", 4) != 4)
		return (0);
	mlength = htoml(length);
	if (MWrite(channel, (char *)&mlength, 4) != 4)
		return (0);
	if (MWrite(channel, (char *)start, length) != length)
		return (0);
	return (1);
}
MWM */
int
SMFTrack::IncreaseSize(long len)
{
	unsigned char *new_start;

	if (static_buf)
		return (0);
	allocated += (len / StreamBlockSize + 1) * StreamBlockSize;
	new_start = new unsigned char [allocated];
	if (new_start == 0)
		return (0);
	if (start == 0) {
		pos = new_start;
		end = new_start;
	} else {
		memcpy(new_start, start, length);
		pos = new_start + (pos - start);
		end = new_start + (end - start);
		delete start;
	}
	start = new_start;
	return (1);
}

ostream &
operator<<(ostream &os, const SMFTrack &t)
{
	long i;
        ios_base::fmtflags prev_flags;
	int prev_width;
	unsigned char *ptr;

	os << "Length: " << t.length << "Running State: " << (int)t.run_state
	    << "\n";
	prev_flags = os.setf(ios::showbase | ios::hex | ios::internal);
	prev_width = os.width();

	ptr = t.pos;
	while (ptr != t.end) {
		for (i = 0; i < 16 && ptr != t.end; i++)
			os << hex << setw(4) << setfill('0')
			    << (int)*ptr++ << " ";
		os << "\n";
	}
	os.flags(prev_flags);
	os.width(prev_width);
	return (os);
}
