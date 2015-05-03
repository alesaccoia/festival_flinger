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

#ifndef CHANPRESEVENT_H
#define CHANPRESEVENT_H

#include "NormEvnt.h"
using namespace std;

class ChannelPressureEvent : public NormalEvent {
	friend ostream &operator<<(ostream &os, const ChannelPressureEvent &e);
public:
	ChannelPressureEvent();
	ChannelPressureEvent(unsigned long t, int chan, int pres);
	ChannelPressureEvent(const ChannelPressureEvent &e);
	virtual Event *Dup(void) const
	    {return (new ChannelPressureEvent(*this));}

	virtual EventType GetType(void) const {return (CHANNELPRESSURE);}
	virtual char *GetTypeStr(void) const {return ("ChannelPressureEvent");}
	virtual string GetEventStr(void) const;
	int GetPressure(void) const {
		if (GetWildcard(wc_pressure))
			return (WC_PRESSURE);
		else
			return (pressure);
	}

	void SetPressure(int pres) {
		if (pres == WC_PRESSURE)
			SetWildcard(wc_pressure);
		else {
			pressure = pres;
			ClearWildcard(wc_pressure);
		}
	}

	ChannelPressureEvent &operator=(const ChannelPressureEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_PRESSURE;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_pressure;
	unsigned char pressure;
};
#endif
