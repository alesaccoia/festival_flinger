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

#include "MSeqSpec.h"

const unsigned char *MetaSequencerSpecificEvent::WC_DATA = 0;
const long MetaSequencerSpecificEvent::WC_LENGTH = -1;

const unsigned long MetaSequencerSpecificEvent::wc_data = (1 << 1);

MetaSequencerSpecificEvent::MetaSequencerSpecificEvent() : data(0), length(0L)
{
}

MetaSequencerSpecificEvent::MetaSequencerSpecificEvent(unsigned long t,
    const unsigned char *dat, long len) : MetaEvent(t), length(len)
{

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

MetaSequencerSpecificEvent::MetaSequencerSpecificEvent(
    const MetaSequencerSpecificEvent &e) : MetaEvent(e), length(e.length)
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

MetaSequencerSpecificEvent::~MetaSequencerSpecificEvent()
{

	delete data;
}

void
MetaSequencerSpecificEvent::SetData(const unsigned char *dat, long len)
{

	if (data != 0)
		delete data;
	if (dat == WC_DATA || len == WC_LENGTH) {
		SetWildcard(wc_data);
		data = 0;
		length = -1;
	} else {
		data = new unsigned char[len];
		assert(data != 0);
		memcpy(data, dat, len);
		length = len;
		ClearWildcard(wc_data);
	}
}

MetaSequencerSpecificEvent &
MetaSequencerSpecificEvent::operator=(const MetaSequencerSpecificEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	if (data != 0)
		delete data;
	if (e.GetWildcard(wc_data)) {
		data = 0;
		length = -1;
		return (*this);
	}
	length = e.length;
	data = new unsigned char[e.length];
	assert(data != 0);
	memcpy(data, e.data, e.length);
	ClearWildcard(wc_data);
	return (*this);
}

string
MetaSequencerSpecificEvent::GetEventStr(void) const
{
	ostringstream buf;
	long i;
	string tbuf;

	tbuf =  MetaEvent::GetEventStr();
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
MetaSequencerSpecificEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if (data != 0)
		delete data;
	if ((length = t.GetVarValue()) == -1)
		return ("Incomplete MetaSequenceSpecificEvent - bad length");
	data = new unsigned char[length];
	if (data == 0)
		return ("Out of memory");
	if ((ptr = t.GetData(length)) == 0)
		return ("Incomplete MetaSequencerSpecificEvent");
	memcpy(data, ptr, length);
	return (0);
}

const char *
MetaSequencerSpecificEvent::SMFWrite(SMFTrack &t) const
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
MetaSequencerSpecificEvent::Equal(const Event *e) const
{
	long i;
	MetaSequencerSpecificEvent *eptr = (MetaSequencerSpecificEvent *)e;

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
operator<<(ostream &os, const MetaSequencerSpecificEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
