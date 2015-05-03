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

#ifndef METASMPTE_H
#define METASMPTE_H

#include "MEvent.h"

class MetaSMPTEEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaSMPTEEvent &e);
public:
	MetaSMPTEEvent();
	MetaSMPTEEvent(unsigned long t, int h, int m, int s, int f, int ff);
	MetaSMPTEEvent(const MetaSMPTEEvent &e);
	virtual Event *Dup(void) const {return (new MetaSMPTEEvent(*this));}

	virtual EventType GetType(void) const {return (METASMPTE);}
	virtual char *GetTypeStr(void) const {return ("MetaSMPTEEvent");}
	virtual string GetEventStr(void) const;
	int GetHour(void) const {
		if (GetWildcard(wc_hour))
			return (WC_HOUR);
		else
			return (hour);
	}
	int GetMinute(void) const {
		if (GetWildcard(wc_minute))
			return (WC_MINUTE);
		else
			return (minute);
	}
	int GetSecond(void) const {
		if (GetWildcard(wc_second))
			return (WC_SECOND);
		else
			return (second);
	}
	int GetFrame(void) const {
		if (GetWildcard(wc_frame))
			return (WC_FRAME);
		else
			return (frame);
	}
	int GetFractionalFrame(void) const {
		if (GetWildcard(wc_fractional_frame))
			return (WC_FRACTIONAL_FRAME);
		else
			return (fractional_frame);
	}

	void SetHour(int h) {
		if (h == WC_HOUR)
			SetWildcard(wc_hour);
		else {
			hour = h;
			ClearWildcard(wc_hour);
		}
	}
	void SetMinute(int m) {
		if (m == WC_MINUTE)
			SetWildcard(wc_minute);
		else {
			minute = m;
			ClearWildcard(wc_minute);
		}
	}
	void SetSecond(int s) {
		if (s == WC_SECOND)
			SetWildcard(wc_second);
		else {
			second = s;
			ClearWildcard(wc_second);
		}
	}
	void SetFrame(int f) {
		if (f == WC_FRAME)
			SetWildcard(wc_frame);
		else {
			frame = f;
			ClearWildcard(wc_frame);
		}
	}
	void SetFractionalFrame(int ff) {
		if (ff == WC_FRACTIONAL_FRAME)
			SetWildcard(wc_fractional_frame);
		else {
			fractional_frame = ff;
			ClearWildcard(wc_fractional_frame);
		}
	}

	MetaSMPTEEvent &operator=(const MetaSMPTEEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_HOUR;
	static const int WC_MINUTE;
	static const int WC_SECOND;
	static const int WC_FRAME;
	static const int WC_FRACTIONAL_FRAME;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_hour;
	static const unsigned long wc_minute;
	static const unsigned long wc_second;
	static const unsigned long wc_frame;
	static const unsigned long wc_fractional_frame;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned char frame;
	unsigned char fractional_frame;
};
#endif
