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

#ifndef METAPORTNUM_H
#define METAPORTNUM_H

#include "MEvent.h"

class MetaPortNumberEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaPortNumberEvent &e);
public:
	MetaPortNumberEvent();
	MetaPortNumberEvent(unsigned long t, int p);
	MetaPortNumberEvent(const MetaPortNumberEvent &e);
	virtual ~MetaPortNumberEvent();
	virtual Event *Dup(void) const
	    {return (new MetaPortNumberEvent(*this));}

	virtual EventType GetType(void) const {return (METAPORTNUMBER);}
	virtual char *GetTypeStr(void) const
	    {return ("MetaPortNumberEvent");}
	virtual string GetEventStr(void) const;
	int GetPort(void) const {
		if (GetWildcard(wc_port))
			return (WC_PORT);
		else
			return (port);
	}

	void SetPort(int p) {
		if (p == WC_PORT)
			SetWildcard(wc_port);
		else {
			port = p;
			ClearWildcard(wc_port);
		}
	}

	MetaPortNumberEvent &operator=(const MetaPortNumberEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_PORT;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_port;
	unsigned char port;
};
#endif
