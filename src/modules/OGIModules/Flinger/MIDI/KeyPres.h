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

#ifndef KEYPRESEVENT_H
#define KEYPRESEVENT_H

#include "NormEvnt.h"

extern const unsigned long WC_PITCH;
extern const unsigned long WC_PRESSURE;

class KeyPressureEvent : public NormalEvent {
	friend ostream &operator<<(ostream &os, const KeyPressureEvent &e);
public:
	KeyPressureEvent();
	KeyPressureEvent(unsigned long t, int chan, int pit, int pres);
	KeyPressureEvent(const KeyPressureEvent &e);
	virtual Event *Dup(void) const {return (new KeyPressureEvent(*this));}

	virtual EventType GetType(void) const {return (KEYPRESSURE);}
	virtual char *GetTypeStr(void) const {return ("KeyPressureEvent");}
	virtual string GetEventStr(void) const;
	int GetPitch(void) const {
		if (GetWildcard(wc_pitch))
			return (WC_PITCH);
		else
			return (pitch);
	}
	int GetPressure(void) const {
		if (GetWildcard(wc_pressure))
			return (WC_PRESSURE);
		else
			return (pressure);
	}

	void SetPitch(int pit) {
		if (pit == WC_PITCH)
			SetWildcard(wc_pitch);
		else {
			pitch = pit;
			ClearWildcard(wc_pitch);
		}
	}
	void SetPressure(int pres) {
		if (pres == WC_PRESSURE)
			SetWildcard(wc_pressure);
		else {
			pressure = pres;
			ClearWildcard(wc_pressure);
		}
	}

	KeyPressureEvent &operator=(const KeyPressureEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_PITCH;
	static const int WC_PRESSURE;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_pitch;
	static const unsigned long wc_pressure;
	unsigned char pitch;
	unsigned char pressure;
};
#endif
