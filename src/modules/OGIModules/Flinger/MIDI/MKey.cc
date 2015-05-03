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

#include <ctype.h>
#include <sstream>
#include <string.h>
using namespace std;
#include "MKey.h"

const Key MetaKeyEvent::WC_KEY = KEY_WC;
const Mode MetaKeyEvent::WC_MODE = MODE_WC;
const unsigned long MetaKeyEvent::wc_key = (1 << 1);
const unsigned long MetaKeyEvent::wc_mode = (1 << 2);

Key
IntToKey(int i)
{

	switch (i) {
	case -7:
		return (KEY_CFLAT);
	case -6:
		return (KEY_GFLAT);
	case -5:
		return (KEY_DFLAT);
	case -4:
		return (KEY_AFLAT);
	case -3:
		return (KEY_EFLAT);
	case -2:
		return (KEY_BFLAT);
	case -1:
		return (KEY_F);
	case 0:
		return (KEY_C);
	case 1:
		return (KEY_G);
	case 2:
		return (KEY_D);
	case 3:
		return (KEY_A);
	case 4:
		return (KEY_E);
	case 5:
		return (KEY_B);
	case 6:
		return (KEY_FSHARP);
	case 7:
		return (KEY_CSHARP);
	default:
		return (KEY_WC);
	}
}

int
KeyToInt(Key k)
{

	switch (k) {
	case KEY_CFLAT:
		return (-7);
	case KEY_GFLAT:
		return (-6);
	case KEY_DFLAT:
		return (-5);
	case KEY_AFLAT:
		return (-4);
	case KEY_EFLAT:
		return (-3);
	case KEY_BFLAT:
		return (-2);
	case KEY_F:
		return (-1);
	case KEY_C:
		return (0);
	case KEY_G:
		return (1);
	case KEY_D:
		return (2);
	case KEY_A:
		return (3);
	case KEY_E:
		return (4);
	case KEY_B:
		return (5);
	case KEY_FSHARP:
		return (6);
	case KEY_CSHARP:
		return (7);
	default:
		return (8);
	}
}

Mode
IntToMode(int i)
{

	switch (i) {
	case 0:
		return (MODE_MAJOR);
	case 1:
		return (MODE_MINOR);
	default:
		return (MODE_WC);
	}
}

int
ModeToInt(Mode m)
{

	switch (m) {
	case MODE_MAJOR:
		return (0);
	case MODE_MINOR:
		return (1);
	default:
		return (-1);
	}
}

MetaKeyEvent::MetaKeyEvent() : key(KEY_C), mode(MODE_MAJOR)
{
}

MetaKeyEvent::MetaKeyEvent(unsigned long t, Key k, Mode m) : MetaEvent(t),
    key(k), mode(m)
{

	if (k == WC_KEY)
		SetWildcard(wc_key);
	if (m == WC_MODE)
		SetWildcard(wc_mode);
}

MetaKeyEvent::MetaKeyEvent(const MetaKeyEvent &e) : MetaEvent(e),
    key(e.key), mode(e.mode)
{
}

const char *
MetaKeyEvent::GetKeyStr(void) const
{

	switch (key) {
	case KEY_CFLAT:
		return ("C Flat");
	case KEY_GFLAT:
		return ("G Flat");
	case KEY_DFLAT:
		return ("D Flat");
	case KEY_AFLAT:
		return ("A Flat");
	case KEY_EFLAT:
		return ("E Flat");
	case KEY_BFLAT:
		return ("B Flat");
	case KEY_F:
		return ("F");
	case KEY_C:
		return ("C");
	case KEY_G:
		return ("G");
	case KEY_D:
		return ("D");
	case KEY_A:
		return ("A");
	case KEY_E:
		return ("E");
	case KEY_B:
		return ("B");
	case KEY_FSHARP:
		return ("F Sharp");
	case KEY_CSHARP:
		return ("C Sharp");
	case KEY_WC:
		return ("*");
	}
	return ("");
}

const char *
MetaKeyEvent::GetModeStr(void) const
{

	switch (mode) {
	case MODE_MAJOR:
		return ("major");
	case MODE_MINOR:
		return ("minor");
	case MODE_WC:
		return ("*");
	}
	return ("");
}

MetaKeyEvent &
MetaKeyEvent::operator=(const MetaKeyEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	key = e.key;
	mode = e.mode;
	return (*this);
}

string
MetaKeyEvent::GetEventStr(void) const
{
	ostringstream buf;
	string tbuf;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Key: " << GetKeyStr() << " Mode: " << GetModeStr()
	    << ends;
	//delete tbuf;
	return (buf.str());
}

const char *
MetaKeyEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	// get and throw away length
	if (t.GetVarValue() != 2)
		return ("Incomplete MetaKeyEvent - bad length");

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaKeyEvent - missing key");
	key = IntToKey((signed char)*ptr);
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaKeyEvent - missing mode");
	mode = IntToMode((signed char)*ptr);
	return (0);
}

const char *
MetaKeyEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(2))
		return ("Out of memory");
	if (!t.PutByte((unsigned char)KeyToInt(key)))
		return ("Out of memory");
	if (!t.PutByte((unsigned char)ModeToInt(mode)))
		return ("Out of memory");
	return (0);
}

int
MetaKeyEvent::Equal(const Event *e) const
{
	MetaKeyEvent *eptr = (MetaKeyEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_key) && !GetWildcard(wc_key) &&
	    key != eptr->key)
		return (0);
	if (!eptr->GetWildcard(wc_mode) && !GetWildcard(wc_mode) &&
	    mode != eptr->mode)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const MetaKeyEvent &e)
{
	const char *str = e.GetEventStr().c_str();
	os << str;
	delete str;
	return (os);
}

Key
StrToKey(const char *str, int *match)
{
	Key key;
	char *keystr;
	int badkey, i, keylen;

	keylen = strlen(str);
	keystr = new char[keylen + 1];
	for (i = 0; i < keylen; i++)
		keystr[i] = tolower(str[i]);
	keystr[i] = '\0';

	/* shut up warning */
	key = KEY_C;
	badkey = 0;
	switch (keystr[0]) {
	case 'a':
		if (strcmp(keystr, "a") == 0)
			key = KEY_A;
		else if (strcmp(keystr, "a flat") == 0)
			key = KEY_AFLAT;
		else if (strcmp(keystr, "a sharp") == 0)
			key = KEY_BFLAT;
		else
			badkey = 1;
		break;
	case 'b':
		if (strcmp(keystr, "b") == 0)
			key = KEY_B;
		else if (strcmp(keystr, "b flat") == 0)
			key = KEY_BFLAT;
		else if (strcmp(keystr, "b sharp") == 0)
			key = KEY_C;
		else
			badkey = 1;
		break;
	case 'c':
		if (strcmp(keystr, "c") == 0)
			key = KEY_C;
		else if (strcmp(keystr, "c flat") == 0)
			key = KEY_CFLAT;
		else if (strcmp(keystr, "c sharp") == 0)
			key = KEY_CSHARP;
		else
			badkey = 1;
		break;
	case 'd':
		if (strcmp(keystr, "d") == 0)
			key = KEY_D;
		else if (strcmp(keystr, "d flat") == 0)
			key = KEY_DFLAT;
		else if (strcmp(keystr, "d sharp") == 0)
			key = KEY_EFLAT;
		else
			badkey = 1;
		break;
	case 'e':
		if (strcmp(keystr, "e") == 0)
			key = KEY_E;
		else if (strcmp(keystr, "e flat") == 0)
			key = KEY_EFLAT;
		else if (strcmp(keystr, "e sharp") == 0)
			key = KEY_F;
		else
			badkey = 1;
		break;
	case 'f':
		if (strcmp(keystr, "f") == 0)
			key = KEY_F;
		else if (strcmp(keystr, "f flat") == 0)
			key = KEY_E;
		else if (strcmp(keystr, "f sharp") == 0)
			key = KEY_FSHARP;
		else
			badkey = 1;
		break;
	case 'g':
		if (strcmp(keystr, "g") == 0)
			key = KEY_G;
		else if (strcmp(keystr, "g flat") == 0)
			key = KEY_GFLAT;
		else if (strcmp(keystr, "g sharp") == 0)
			key = KEY_AFLAT;
		else
			badkey = 1;
		break;
	case '*':
		key = KEY_WC;
		break;
	default:
		badkey = 1;
	}

	delete keystr;
	if (badkey)
		*match = 0;
	else
		*match = 1;

	return (key);
}

Mode
StrToMode(const char *str, int *match)
{
	Mode mode;
	char *modestr;
	int i, modelen;

	modelen = strlen(str);
	modestr = new char[modelen + 1];
	for (i = 0; i < modelen; i++)
		modestr[i] = tolower(str[i]);
	modestr[i] = '\0';

	*match = 1;
	if (strcmp(modestr, "minor") == 0)
		mode = MODE_MINOR;
	else if (strcmp(modestr, "major") == 0)
		mode = MODE_MAJOR;
	else if (strcmp(modestr, "*") == 0)
		mode = MODE_WC;
	else {
		mode = MODE_MAJOR;
		*match = 0;
	}

	return (mode);
}
