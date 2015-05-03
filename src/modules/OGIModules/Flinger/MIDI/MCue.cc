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

#include "MCue.h"

MetaCueEvent::MetaCueEvent()
{
}

MetaCueEvent::MetaCueEvent(unsigned long t, const char *str) :
    MetaTextEvent(t, str)
{
}

MetaCueEvent::MetaCueEvent(const MetaCueEvent &e) :
    MetaTextEvent(e)
{
}

MetaCueEvent::~MetaCueEvent()
{
}

MetaCueEvent &
MetaCueEvent::operator=(const MetaCueEvent &e)
{

	(MetaTextEvent)*this = (MetaTextEvent)e;
	return (*this);
}

string
MetaCueEvent::GetEventStr(void) const
{

	return (MetaTextEvent::GetEventStr());
}

ostream &
operator<<(ostream &os, const MetaCueEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}
