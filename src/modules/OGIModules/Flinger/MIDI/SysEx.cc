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
#include <sstream>
#include <string.h>
using namespace std;


#include "SysEx.h"

const unsigned char *SystemExclusiveEvent::WC_DATA = 0;
const long SystemExclusiveEvent::WC_LENGTH = -1;

const unsigned long SystemExclusiveEvent::wc_data = (1 << 1);

SystemExclusiveEvent::SystemExclusiveEvent() : length(0L), continued(0),
    data(0)
{
}

SystemExclusiveEvent::SystemExclusiveEvent(unsigned char c) : length(0L),
    continued(c), data(0)
{
}

SystemExclusiveEvent::SystemExclusiveEvent(unsigned long t,
    const unsigned char *dat, long len) : Event(t), length(len),
    continued(0)
{

	if (dat == WC_DATA || len == WC_LENGTH) {
		SetWildcard(wc_data);
		data = 0;
		length = -1;
	} else {
		data = new unsigned char[len];
		assert(data != 0);
		memcpy(data, dat, len);
	}
}

SystemExclusiveEvent::SystemExclusiveEvent(const SystemExclusiveEvent &e) :
    Event(e), length(e.length), continued(e.continued)
{

	if (e.GetWildcard(wc_data)) {
		data = 0;
		length = -1;
	} else {
		data = new unsigned char[e.length];
		assert(data != 0);
		memcpy(data, e.data, e.length);
	}
}

SystemExclusiveEvent::~SystemExclusiveEvent()
{

	delete data;
}

void
SystemExclusiveEvent::SetData(const unsigned char *dat, long len)
{

	if (data != 0)
		delete data;
	if (len == WC_LENGTH || dat == WC_DATA) {
		SetWildcard(wc_data);
		data = 0;
		return;
	}
	data = new unsigned char[len];
	assert(data != 0);
	memcpy(data, dat, len);
	ClearWildcard(wc_data);
}

SystemExclusiveEvent &
SystemExclusiveEvent::operator=(const SystemExclusiveEvent &e)
{

	(Event)*this = (Event)e;
	if (data != 0)
		delete data;
	continued = e.continued;
	length = e.length;
	if (e.GetWildcard(wc_data)) {
		data = 0;
		return (*this);
	}
	data = new unsigned char[e.length];
	assert(data != 0);
	memcpy(data, e.data, e.length);
	return (*this);
}

string
SystemExclusiveEvent::GetEventStr(void) const
{
	ostringstream buf;
	long i;
	string tbuf;

	tbuf = Event::GetEventStr();
	buf << tbuf << " Continued: " << (int)continued << " Data:";
	if (GetWildcard(wc_data))
		buf << " *";
	else {
		buf.setf(ios::showbase | ios::internal);
		for (i = 0; i < length; i++)
			buf << " " << hex << setw(4) << setfill('0') <<
			    (int)data[i];
	}
	buf << ends;
	//delete tbuf;
	return (buf.str());
}

const char *
SystemExclusiveEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if (data != 0)
		delete data;
	if ((length = t.GetVarValue()) == -1)
		return ("Incomplete SystemExclusiveEvent - bad length");
	data = new unsigned char[length];
	if (data == 0)
		return ("Out of memory");
	if ((ptr = t.GetData(length)) == 0)
		return ("Incomplete SystemExclusiveEvent");
	memcpy(data, ptr, length);
	return (0);
}

const char *
SystemExclusiveEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(length))
		return ("Out of memory");
	if (!t.PutData(data, length))
		return ("Out of memory");
	return (0);
}

int
SystemExclusiveEvent::Equal(const Event *e) const
{
	long i;
	SystemExclusiveEvent *eptr = (SystemExclusiveEvent *)e;

	if (!Event::Equal(e))
		return (0);
	if (eptr->GetWildcard(wc_data) || GetWildcard(wc_data))
		return (1);
	if (continued != eptr->continued)
		return (0);
	if (length != eptr->length)
		return (0);
	for (i = 0; i < length; i++)
		if (data[i] != eptr->data[i])
			return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const SystemExclusiveEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
