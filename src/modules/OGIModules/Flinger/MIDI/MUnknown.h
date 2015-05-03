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

#ifndef METAUNKNOWN_H
#define METAUNKNOWN_H

#include "MEvent.h"

class MetaUnknownEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaUnknownEvent &e);
public:
	MetaUnknownEvent();
	MetaUnknownEvent(int ty);
	MetaUnknownEvent(unsigned long t, const unsigned char *data,
	    long length, int ty);
	MetaUnknownEvent(const MetaUnknownEvent &e);
	virtual ~MetaUnknownEvent();
	virtual Event *Dup(void) const {return (new MetaUnknownEvent(*this));}

	virtual EventType GetType(void) const {return (METAUNKNOWN);}
	virtual char *GetTypeStr(void) const {return ("MetaUnknownEvent");}
	virtual string GetEventStr(void) const;
	const unsigned char *GetData(void) const {
		if (GetWildcard(wc_data))
			return (WC_DATA);
		else
			return (data);
	}
	long GetLength(void) const {
		if (GetWildcard(wc_data))
			return (WC_LENGTH);
		else
			return (length);
	}
	int GetMetaType(void) const {
		if (GetWildcard(wc_meta_type))
			return (WC_META_TYPE);
		else
			return (type);
	}

	void SetData(const unsigned char *data, long length);
	void SetMetaType(unsigned char t) {
		if (t == WC_META_TYPE)
			SetWildcard(wc_meta_type);
		else {
			type = t;
			ClearWildcard(wc_meta_type);
		}
	}

	MetaUnknownEvent &operator=(const MetaUnknownEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const unsigned char *WC_DATA;
	static const long WC_LENGTH;
	static const int WC_META_TYPE;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_data;
	static const unsigned long wc_meta_type;
	long length;
	unsigned char *data;
	unsigned char type;
};
#endif
