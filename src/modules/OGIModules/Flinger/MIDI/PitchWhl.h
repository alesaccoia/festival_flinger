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

#ifndef PITCHWHEEL_H
#define PITCHWHEEL_H

#include "NormEvnt.h"

class PitchWheelEvent : public NormalEvent {
	friend ostream &operator<<(ostream &os, const PitchWheelEvent &e);
public:
	PitchWheelEvent();
	PitchWheelEvent(unsigned long t, int chan, long val);
	PitchWheelEvent(const PitchWheelEvent &e);
	virtual Event *Dup(void) const {return (new PitchWheelEvent(*this));}

	virtual EventType GetType(void) const {return (PITCHWHEEL);}
	virtual char *GetTypeStr(void) const {return ("PitchWheelEvent");}
	virtual string GetEventStr(void) const;
	long GetValue(void) const {
		if (GetWildcard(wc_value))
			return (WC_VALUE);
		else
			return (value);
	}

	void SetValue(long val) {
		if (val == WC_VALUE)
			SetWildcard(wc_value);
		else {
			value = val;
			ClearWildcard(wc_value);
		}
	}

	PitchWheelEvent &operator=(const PitchWheelEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const long WC_VALUE;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_value;
//DJLB correction 8/09/03
//	short value;
	long value;
};
#endif
