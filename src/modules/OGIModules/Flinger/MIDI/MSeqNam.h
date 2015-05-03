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

#ifndef METASEQNAME_H
#define METASEQNAME_H

#include "MText.h"

class MetaSequenceNameEvent : public MetaTextEvent {
	friend ostream &operator<<(ostream &os, const MetaSequenceNameEvent &e);
public:
	MetaSequenceNameEvent();
	MetaSequenceNameEvent(unsigned long t, const char *str);
	MetaSequenceNameEvent(const MetaSequenceNameEvent &e);
	virtual ~MetaSequenceNameEvent();
	virtual Event *Dup(void) const
	    {return (new MetaSequenceNameEvent(*this));}

	virtual EventType GetType(void) const {return (METASEQUENCENAME);}
	virtual char *GetTypeStr(void) const {return ("MetaSequenceNameEvent");}
	virtual string GetEventStr(void) const;

	MetaSequenceNameEvent &operator=(const MetaSequenceNameEvent &e);

	virtual const char *SMFRead(SMFTrack &t)
	    {return (MetaTextEvent::SMFRead(t));}
	virtual const char *SMFWrite(SMFTrack &t) const
	    {return (MetaTextEvent::SMFWrite(t));}
protected:
	virtual int Equal(const Event *e) const
	    {return (MetaTextEvent::Equal(e));}
};
#endif
