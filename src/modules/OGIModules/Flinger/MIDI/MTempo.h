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

#ifndef METATEMPO_H
#define METATEMPO_H

#include "MEvent.h"

class MetaTempoEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaTempoEvent &e);
public:
	MetaTempoEvent();
	MetaTempoEvent(unsigned long t, short temp = 120);
	MetaTempoEvent(const MetaTempoEvent &e);
	virtual Event *Dup(void) const {return (new MetaTempoEvent(*this));}

	virtual EventType GetType(void) const {return (METATEMPO);}
	virtual char *GetTypeStr(void) const {return ("MetaTempoEvent");}
	virtual string GetEventStr(void) const;
	short GetTempo(void) const {
		if (GetWildcard(wc_tempo))
			return (WC_TEMPO);
		else
			return (tempo);
	}

	void SetTempo(short temp) {
		if (temp == WC_TEMPO)
			SetWildcard(wc_tempo);
		else {
			tempo = temp;
			ClearWildcard(wc_tempo);
		}
	}

	MetaTempoEvent &operator=(const MetaTempoEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const short WC_TEMPO;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_tempo;
	short tempo;
};
#endif
