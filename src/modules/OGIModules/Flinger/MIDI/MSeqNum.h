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

#ifndef METASEQNUM_H
#define METASEQNUM_H

#include "MEvent.h"

class MetaSequenceNumberEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os,
	    const MetaSequenceNumberEvent &e);
public:
	MetaSequenceNumberEvent();
	MetaSequenceNumberEvent(unsigned long t, long num);
	MetaSequenceNumberEvent(const MetaSequenceNumberEvent &e);
	virtual Event *Dup(void) const
	    {return (new MetaSequenceNumberEvent(*this));}

	virtual EventType GetType(void) const {return (METASEQUENCENUMBER);}
	virtual char *GetTypeStr(void) const
	    {return ("MetaSequenceNumberEvent");}
	virtual string GetEventStr(void) const;
	long GetNumber(void) const {
		if (GetWildcard(wc_number))
			return (WC_NUMBER);
		else
			return (number);
	}

	void SetNumber(long num) {
		if (num == WC_NUMBER)
			SetWildcard(wc_number);
		else {
			number = num;
			ClearWildcard(wc_number);
		}
	}

	MetaSequenceNumberEvent &operator=(const MetaSequenceNumberEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const long WC_NUMBER;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_number;
//DJLB correction 8/09/03
//	short number;
	long number;
};
#endif
