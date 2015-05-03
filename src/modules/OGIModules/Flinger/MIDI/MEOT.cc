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


#include "MEOT.h"

MetaEndOfTrackEvent::MetaEndOfTrackEvent()
{
}

MetaEndOfTrackEvent::MetaEndOfTrackEvent(unsigned long t) : MetaEvent(t)
{
}

MetaEndOfTrackEvent::MetaEndOfTrackEvent(const MetaEndOfTrackEvent &e) :
    MetaEvent(e)
{
}

MetaEndOfTrackEvent &
MetaEndOfTrackEvent::operator=(const MetaEndOfTrackEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	return (*this);
}

string
MetaEndOfTrackEvent::GetEventStr(void) const
{

	return (MetaEvent::GetEventStr());
}

const char *
MetaEndOfTrackEvent::SMFRead(SMFTrack &t)
{

	// get length and throw away - will be zero
	if (t.GetVarValue() != 0)
		return ("Incomplete MetaEndOfTrackEvent - bad length");
	return (0);
}

const char *
MetaEndOfTrackEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(0))
		return ("Out of memory");
	return (0);
}

ostream &
operator<<(ostream &os, const MetaEndOfTrackEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
