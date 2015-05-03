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

#include "MUnknown.h"

const unsigned char *MetaUnknownEvent::WC_DATA = 0;
const long MetaUnknownEvent::WC_LENGTH = 0;
const int MetaUnknownEvent::WC_META_TYPE = -1;

const unsigned long MetaUnknownEvent::wc_data = (1 << 1);
const unsigned long MetaUnknownEvent::wc_meta_type = (1 << 2);

MetaUnknownEvent::MetaUnknownEvent() : length(0L), data(0), type(0x60)
{
}

MetaUnknownEvent::MetaUnknownEvent(int ty) : length(0L), data(0),
    type(ty)
{

	if (ty == WC_META_TYPE)
		SetWildcard(wc_meta_type);
}

MetaUnknownEvent::MetaUnknownEvent(unsigned long t, const unsigned char *dat,
    long len, int ty) : MetaEvent(t), length(len), type(ty)
{

	if (ty == WC_META_TYPE)
		SetWildcard(wc_meta_type);
	if (dat == WC_DATA || len == WC_LENGTH) {
		SetWildcard(wc_data);
		data = 0;
		length = -1;
		return;
	}
	data = new unsigned char[len];
	assert(data != 0);
	memcpy(data, dat, len);
}

MetaUnknownEvent::MetaUnknownEvent(const MetaUnknownEvent &e) : MetaEvent(e),
    length(e.length), type(e.type)
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

MetaUnknownEvent::~MetaUnknownEvent()
{

	delete data;
}

void
MetaUnknownEvent::SetData(const unsigned char *dat, long len)
{

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

MetaUnknownEvent &
MetaUnknownEvent::operator=(const MetaUnknownEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	delete data;
	type = e.type;
	if (e.GetWildcard(wc_data)) {
		data = 0;
		length = -1;
		return (*this);
	}
	length = e.length;
	data = new unsigned char[e.length];
	assert(data != 0);
	memcpy(data, e.data, e.length);
	return (*this);
}

string
MetaUnknownEvent::GetEventStr(void) const
{
	ostringstream buf;
	long i;
	string tbuf;

	tbuf =  MetaEvent::GetEventStr();
	buf.setf(ios::showbase | ios::internal);
	buf << tbuf << " Type: ";
	if (GetWildcard(wc_meta_type))
		buf << "*";
	else
		buf << hex << setw(4) << setfill('0') << (int)type;
	buf << " Data:";
	if (GetWildcard(wc_data))
		buf << " *";
	else {
		for (i = 0; i < length; i++)
			buf << " " << hex << setw(4) << setfill('0') <<
			    (int)data[i];
	}
	buf << ends;
	//delete tbuf;
	return (buf.str());
}

const char *
MetaUnknownEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if (data != 0)
		delete data;
	if ((length = t.GetVarValue()) == -1)
		return ("Incomplete MetaUnknownEvent - bad length");
	data = new unsigned char[length];
	if (data == 0)
		return ("Out of memory");
	if ((ptr = t.GetData(length)) == 0)
		return ("Incomplete MetaUnknownEvent");
	memcpy(data, ptr, length);
	return (0);
}

const char *
MetaUnknownEvent::SMFWrite(SMFTrack &t) const
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
MetaUnknownEvent::Equal(const Event *e) const
{
	long i;
	MetaUnknownEvent *eptr = (MetaUnknownEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_meta_type) && !GetWildcard(wc_meta_type) &&
	    type != eptr->type)
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
operator<<(ostream &os, const MetaUnknownEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
