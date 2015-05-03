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


#ifndef METAINSTNAME_H
#define METAINSTNAME_H

#include "MText.h"

class MetaInstrumentNameEvent : public MetaTextEvent {
	friend ostream &operator<<(ostream &os,
	    const MetaInstrumentNameEvent &e);
public:
	MetaInstrumentNameEvent();
	MetaInstrumentNameEvent(unsigned long t, const char *str);
	MetaInstrumentNameEvent(const MetaInstrumentNameEvent &e);
	virtual ~MetaInstrumentNameEvent();
	virtual Event *Dup(void) const
	    {return (new MetaInstrumentNameEvent(*this));}

	virtual EventType GetType(void) const {return (METAINSTRUMENTNAME);}
	virtual char *GetTypeStr(void) const {return ("MetaInstrumentName");}
	virtual string GetEventStr(void) const;

	MetaInstrumentNameEvent &operator=(const MetaInstrumentNameEvent &e);

	virtual const char *SMFRead(SMFTrack &t)
	    {return (MetaTextEvent::SMFRead(t));}
	virtual const char *SMFWrite(SMFTrack &t) const
	    {return (MetaTextEvent::SMFWrite(t));}
protected:
	virtual int Equal(const Event *e) const
	    {return (MetaTextEvent::Equal(e));}
};
#endif
