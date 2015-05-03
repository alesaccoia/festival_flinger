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

#ifndef METATEXT_H
#define METATEXT_H

#include "MEvent.h"

class MetaTextEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaTextEvent &e);
public:
	MetaTextEvent();
	MetaTextEvent(unsigned long t, const char *str);
	MetaTextEvent(const MetaTextEvent &e);
	virtual ~MetaTextEvent();
	virtual Event *Dup(void) const {return (new MetaTextEvent(*this));}

	virtual EventType GetType(void) const {return (METATEXT);}
	virtual char *GetTypeStr(void) const {return ("MetaTextEvent");}
	virtual string GetEventStr(void) const;
	const char *GetString(void) const {
		if (GetWildcard(wc_string))
			return (WC_STRING);
		else
			return (fl_string);
	}
	long GetLength(void) const {
		if (GetWildcard(wc_string))
			return (WC_LENGTH);
		else
			return (length);
	}

	void SetString(const char *str);

	MetaTextEvent &operator=(const MetaTextEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const char *WC_STRING;
	static const long WC_LENGTH;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_string;
	char *fl_string;
	long length;
};
#endif
