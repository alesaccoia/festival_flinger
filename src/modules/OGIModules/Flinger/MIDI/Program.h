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

#ifndef PROGRAMEVENT_H
#define PROGRAMEVENT_H

#include "NormEvnt.h"

class ProgramEvent : public NormalEvent {
	friend ostream &operator<<(ostream &os, const ProgramEvent &e);
public:
	ProgramEvent();
	ProgramEvent(unsigned long t, int chan, int val);
	ProgramEvent(const ProgramEvent &e);
	virtual Event *Dup(void) const {return (new ProgramEvent(*this));}

	virtual EventType GetType(void) const {return (PROGRAM);}
	virtual char *GetTypeStr(void) const {return ("ProgramEvent");}
	virtual string GetEventStr(void) const;
	int GetValue(void) const {
		if (GetWildcard(wc_value))
			return (WC_VALUE);
		else
			return (value);
	}

	void SetValue(int val) {
		if (val == WC_VALUE)
			SetWildcard(wc_value);
		else {
			value = val;
			ClearWildcard(wc_value);
		}
	}

	ProgramEvent &operator=(const ProgramEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_VALUE;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_value;
	unsigned char value;
};
#endif
