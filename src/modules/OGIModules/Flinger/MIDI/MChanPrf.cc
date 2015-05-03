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

#include "MChanPrf.h"

const unsigned char *MetaChannelPrefixEvent::WC_DATA = 0;
const long MetaChannelPrefixEvent::WC_LENGTH = -1;

const unsigned long MetaChannelPrefixEvent::wc_data = (1 << 1);

MetaChannelPrefixEvent::MetaChannelPrefixEvent() : data(0), length(0L)
{
}

MetaChannelPrefixEvent::MetaChannelPrefixEvent(unsigned long t,
    const unsigned char *dat, long len) : MetaEvent(t), length(len)
{

	if (dat == WC_DATA || len == WC_LENGTH) {
		SetWildcard(wc_data);
		data = 0;
		len = -1;
		return;
	}
	data = new unsigned char[len];
	assert(data != 0);
	memcpy(data, dat, len);
}

MetaChannelPrefixEvent::MetaChannelPrefixEvent(const MetaChannelPrefixEvent &e) :
    MetaEvent(e), length(e.length)
{

	if (e.GetWildcard(wc_data)) {
		data = 0;
		length = -1;
		return;
	}
	data = new unsigned char[e.length];
	assert(data != 0);
	memcpy(data, e.data, e.length);
}

MetaChannelPrefixEvent::~MetaChannelPrefixEvent()
{

	delete data;
}

void
MetaChannelPrefixEvent::SetData(const unsigned char *dat, long len)
{

	if (data != 0)
		delete data;
	if (dat == WC_DATA || len == WC_LENGTH) {
		SetWildcard(wc_data);
		data = 0;
		length = -1;
		return;
	}
	data = new unsigned char[len];
	assert(data != 0);
	memcpy(data, dat, len);
	ClearWildcard(wc_data);
}

MetaChannelPrefixEvent &
MetaChannelPrefixEvent::operator=(const MetaChannelPrefixEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	if (data != 0)
		delete data;
	if (e.GetWildcard(wc_data)) {
		data = 0;
		length = -1;
		return (*this);
	}
	data = new unsigned char[e.length];
	length = e.length;
	assert(data != 0);
	memcpy(data, e.data, e.length);
	return (*this);
}

string
MetaChannelPrefixEvent::GetEventStr(void) const
{
	ostringstream buf;
	string tbuf;
	long i;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Data:";
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
MetaChannelPrefixEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if (data != 0)
		delete data;
	if ((length = t.GetVarValue()) == -1)
		return ("Incomplete MetaChannelPrefixEvent - bad length");
	data = new unsigned char[length];
	if (data == 0)
		return ("Out of memory");
	if ((ptr = t.GetData(length)) == 0)
		return ("Incomplete MetaChannelPrefixEvent - bad data");
	memcpy(data, ptr, length);
	return (0);
}

const char *
MetaChannelPrefixEvent::SMFWrite(SMFTrack &t) const
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
MetaChannelPrefixEvent::Equal(const Event *e) const
{
	long i;
	MetaChannelPrefixEvent *eptr = (MetaChannelPrefixEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (eptr->GetWildcard(wc_data) || GetWildcard(wc_data))
		return (1);
	if (length != eptr->length)
		return (0);
	for (i = 0; i < length; i++)
		if (data[i] != eptr->data[i])
			return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const MetaChannelPrefixEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
