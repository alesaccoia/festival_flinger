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

#include "MText.h"

const char *MetaTextEvent::WC_STRING = 0;
const long MetaTextEvent::WC_LENGTH = -1;

const unsigned long MetaTextEvent::wc_string = (1 << 1);

MetaTextEvent::MetaTextEvent() : fl_string(0), length(0)
{
}

MetaTextEvent::MetaTextEvent(unsigned long t, const char *str) :
    MetaEvent(t)
{

	if (str == WC_STRING) {
		SetWildcard(wc_string);
		fl_string = 0;
		length = -1;
		return;
	}
	length = strlen(str);
	if (length == 0)
		fl_string = 0;
	else {
		fl_string = new char[length + 1];
		assert(fl_string != 0);
		strcpy(fl_string, str);
	}
}

MetaTextEvent::MetaTextEvent(const MetaTextEvent &e) :
    MetaEvent(e), length(e.length)
{

	if (e.GetWildcard(wc_string)) {
		fl_string = 0;
		length = -1;
		return;
	}
	if (e.length == 0)
		fl_string = 0;
	else {
		fl_string = new char[e.length + 1];
		assert(fl_string != 0);
		strcpy(fl_string, e.fl_string);
	}
}

MetaTextEvent::~MetaTextEvent()
{

	delete fl_string;
}

void
MetaTextEvent::SetString(const char *str)
{

	delete fl_string;
	if (str == WC_STRING) {
		fl_string = 0;
		length = -1;
		SetWildcard(wc_string);
		return;
	}
	length = strlen(str);
	fl_string = new char[length + 1];
	assert(fl_string != 0);
	strcpy(fl_string, str);
	ClearWildcard(wc_string);
}

MetaTextEvent &
MetaTextEvent::operator=(const MetaTextEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	length = e.length;
	delete fl_string;
	if (e.GetWildcard(wc_string)) {
		fl_string = 0;
		return (*this);
	}
	fl_string = new char[length + 1];
	assert(fl_string != 0);
	strcpy(fl_string, e.fl_string);
	return (*this);
}

string
MetaTextEvent::GetEventStr(void) const
{
	ostringstream buf;
	string tbuf;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Text: ";
	if (GetWildcard(wc_string))
		buf << "*";
	else
		buf << fl_string;
	buf << ends;
	//delete tbuf;
	return (buf.str());
}

const char *
MetaTextEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if (fl_string != 0)
		delete fl_string;
	if ((length = t.GetVarValue()) == -1)
		return ("Incomplete MetaTextEvent - bad length");
	fl_string = new char[length + 1];
	if ((ptr = t.GetData(length)) == 0)
		return ("Incomplete MetaTextEvent - bad data");
	memcpy(fl_string, ptr, length);
	fl_string[length] = '\0';
	return (0);
}

const char *
MetaTextEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(length))
		return ("Out of memory");
	if (!t.PutData((unsigned char *)fl_string, length))
		return ("Out of memory");
	return (0);
}

int
MetaTextEvent::Equal(const Event *e) const
{
	MetaTextEvent *eptr = (MetaTextEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (eptr->GetWildcard(wc_string) || GetWildcard(wc_string))
		return (1);
	if (length != eptr->length)
		return (0);
	return (strcmp(fl_string, eptr->fl_string) == 0);
}

ostream &
operator<<(ostream &os, const MetaTextEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
