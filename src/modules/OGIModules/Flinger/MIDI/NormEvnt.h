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

#ifndef NORMALEVENT_H
#define NORMALEVENT_H

#include "Event.h"

class NormalEvent : public Event {
	friend ostream &operator<<(ostream &os, const NormalEvent &e);
public:
	NormalEvent() : channel(0) {};
        virtual ~NormalEvent() {};
	NormalEvent(unsigned long t, int chan);
	NormalEvent(const NormalEvent &e);
	virtual Event *Dup(void) const {return (new NormalEvent(*this));}

	virtual EventType GetType(void) const {return (NORMAL);}
	virtual char *GetTypeStr(void) const {return ("NormalEvent");}
	virtual string GetEventStr(void) const;
	int GetChannel(void) const {
		if (GetWildcard(wc_channel))
			return (WC_CHANNEL);
		else
			return (channel);
	}

	void SetChannel(int chan) {
		if (chan == WC_CHANNEL)
			SetWildcard(wc_channel);
		else {
			channel = chan;
			ClearWildcard(wc_channel);
		}
	}

	NormalEvent &operator=(const NormalEvent &e);

	virtual const char *SMFRead(SMFTrack &t) {
		// shut up a warning
		SMFTrack *dummy;
		dummy = &t;
		return (0);
	}
	virtual const char *SMFWrite(SMFTrack &t) const {
		// shut up a warning
		SMFTrack *dummy;
		dummy = &t;
		return (0);
	}

	static const int WC_CHANNEL;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_channel;
	unsigned char channel;
};
#endif
