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

#ifndef METAKEY_H
#define METAKEY_H

#include "MEvent.h"

typedef enum {KEY_CFLAT = -7, KEY_GFLAT = -6, KEY_DFLAT = -5, KEY_AFLAT = -4,
    KEY_EFLAT = -3, KEY_BFLAT = -2, KEY_F = -1, KEY_C = 0, KEY_G = 1,
    KEY_D = 2, KEY_A = 3, KEY_E = 4, KEY_B = 5, KEY_FSHARP = 6,
    KEY_CSHARP = 7, KEY_WC} Key;

typedef enum {MODE_MAJOR = 0, MODE_MINOR = 1, MODE_WC} Mode;

Key IntToKey(int i);
int KeyToInt(Key k);
Mode IntToMode(int i);
int ModeToInt(Mode m);

extern Key StrToKey(const char *str, int *match);
extern Mode StrToMode(const char *str, int *match);

class MetaKeyEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaKeyEvent &e);
public:
	MetaKeyEvent();
	MetaKeyEvent(unsigned long t, Key k = KEY_C, Mode m = MODE_MAJOR);
	MetaKeyEvent(const MetaKeyEvent &e);
	virtual Event *Dup(void) const {return (new MetaKeyEvent(*this));}

	virtual EventType GetType(void) const {return (METAKEY);}
	virtual char *GetTypeStr(void) const {return ("MetaKeyEvent");}
	virtual string GetEventStr(void) const;
	Key GetKey(void) const {
		if (GetWildcard(wc_key))
			return (KEY_WC);
		else
			return (key);
	}
	const char *GetKeyStr(void) const;
	Mode GetMode(void) const {
		if (GetWildcard(wc_mode))
			return (MODE_WC);
		else
			return (mode);
	}
	const char *GetModeStr(void) const;

	void SetKey(Key k) {
		if (k == KEY_WC)
			SetWildcard(wc_key);
		else {
			key = k;
			ClearWildcard(wc_key);
		}
	}
	void SetMode(Mode m) {
		if (m == MODE_WC)
			SetWildcard(wc_mode);
		else {
			mode = m;
			ClearWildcard(wc_mode);
		}
	}

	MetaKeyEvent &operator=(const MetaKeyEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const Key WC_KEY;
	static const Mode WC_MODE;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_key;
	static const unsigned long wc_mode;
	Key key;
	Mode mode;
};
#endif
