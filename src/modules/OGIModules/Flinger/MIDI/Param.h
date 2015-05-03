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

#ifndef PARAMEVENT_H
#define PARAMEVENT_H

#include "NormEvnt.h"

class ParameterEvent : public NormalEvent {
	friend ostream &operator<<(ostream &os, const ParameterEvent &e);
public:
	ParameterEvent();
	ParameterEvent(unsigned long t, int chan, int param, int val);
	ParameterEvent(const ParameterEvent &e);
	virtual Event *Dup(void) const {return (new ParameterEvent(*this));}

	virtual EventType GetType(void) const {return (PARAMETER);}
	virtual char *GetTypeStr(void) const {return ("ParameterEvent");}
	virtual string GetEventStr(void) const;
	int GetParameter(void) const {
		if (GetWildcard(wc_parameter))
			return (WC_PARAMETER);
		else
			return (parameter);
	}
	int GetValue(void) const {
		if (GetWildcard(wc_value))
			return (WC_VALUE);
		else
			return (value);
	}

	void SetParameter(int param) {
		if (param == WC_PARAMETER)
			SetWildcard(wc_parameter);
		else {
			parameter = param;
			ClearWildcard(wc_parameter);
		}
	}
	void SetValue(int val) {
		if (val == WC_VALUE)
			SetWildcard(wc_value);
		else {
			value = val;
			ClearWildcard(wc_value);
		}
	}

	ParameterEvent &operator=(const ParameterEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_PARAMETER;
	static const int WC_VALUE;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_parameter;
	static const unsigned long wc_value;
	unsigned char parameter;
	unsigned char value;
};
#endif
