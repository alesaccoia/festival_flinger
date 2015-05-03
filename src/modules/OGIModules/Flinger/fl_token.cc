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
//
//  Some functions in this file adapted from
//                   festival/src/modules/base/word.cc
//                   festival/modules/Text/token.cc
//


//  Strips off (and keeps track of) punctuation symbols, etc.
//  in LyricTokens.  Results in Lyric relation containing
//  pronouncable words/syllables, no punctuation or symbols.
#include <stdlib.h>
#include "festival.h"
#include "modules.h"
#include "lexicon.h"
#include "fl_io.h"
static void relate_notes_to_lyrictokens(EST_Utterance *u);

static LISP lyric_it(EST_Item *t,const EST_String tok);
static LISP builtin_lyric_it(EST_Item *token, EST_String tok);

static LISP user_lyrictoken_to_lyric_func = NIL;
static char *RXnotpunctuation = "[^-.,]";
static EST_String remove_lead_trail_whitespace(const EST_String &tok);
static EST_String return_post_punct(const EST_String &tok);
static EST_String return_pre_punct(const EST_String &tok);
static EST_Item *add_lyric(EST_Utterance *u, const EST_String &name);
static EST_Item *add_lyric(EST_Utterance *u, LISP lyric);
static EST_Regex RXUnWantedChars("[^A-Za-z'-]");
static EST_String fesRemoveUnWantedCharacters(const EST_String &tok);
static EST_String fesRemoveQuotationMarks(const EST_String &tok);
static EST_String fesRemoveTrailingUnderScores(const EST_String &tok);
static EST_String fesRemoveMultipleTrailingHyphens(const EST_String &tok);

static EST_String PhoneDelim;
static EST_String SylContin;

/*-----------------------------------------------------------------------*/

/*
By DJLB 11-09-2006 re. puncuation
		Although puncuation may have effects during normal speech, in singing
		the only 'puncuation' (and it is not really punctuation) of concern is
		the trailing hyphen, indicating a split-up of a word (always used to
		accommodate allocating parts of a word to particular notes). Huphens
		within a single lyric token should however be removed, joining the parts
		together to make one word.
		Lyrics often include trailing strings of hyphens or underscores, indicating
		a lyric token carrying on over two or more notes. These may be useful to
		the real singer, but have no meaning here.
		So the handling of punctuation in this file and fl_lyricize.cpp has been
		simplified.
*/

LISP FL_Tokenize_Utt(LISP utt)
{ // Uses user-specified token to lyric function (defined in scheme)
  // or a set of builtin rules.   User specified one can also call
  // the built-in set.
  EST_Utterance *u = get_c_utt(utt);
  LISP lyrics,w;
  EST_Item *t;
  EST_Item *new_lyric;

  // relate notes to lyric tokens
  //relate_notes_to_lyrictokens(u);

  // pull lyrictoken_to_lyric function definition out of scm environment
  user_lyrictoken_to_lyric_func =
						siod_get_lval("lyrictoken_to_lyric_func",NULL);

  for (t=u->relation("LyricToken")->first(); t != 0; t = t->next())
	{
		t->set("name", fesRemoveUnWantedCharacters(t->name()));
		t->set("name", remove_lead_trail_whitespace(t->name()));
		// Get rid of any trailing hyphons other than one on its own.
		t->set("name", fesRemoveMultipleTrailingHyphens(t->name()));
		// Store any post-punctuation left. (This will probably be "-",
		//	and will be used in FL_Lyricize() for detecting split words.)
    t->set("post-punc", return_post_punct(t->name()));

    // if detect "-", then do fancy re-assemble, lex lookup, dis-assemble
    //   if this is successful, make result look like "phonemes" inputs
    //   on LyricTokens.
		/*
		By DJLB 28-08-2006. This has never been done. I have accomplished
			it, but feel that the best place for it is in FL_Lyricize_Utt(),
			which does the next step - producing the phones, stress etc. from
			the lexicon entry for the 'words' contained in the lyric tokens.
		*/

   // convert lyrictoken name to 1 or more lyrics
    lyrics = lyric_it(t,t->name());
    // create item for each lyric, make daughter to LyricToken
    //   in both NoteStruct and SylStructure
    for (w=lyrics; w != NIL; w=cdr(w))
		{
      new_lyric = add_lyric(u,car(w)); //removes any punctuation.
      append_daughter(t,"SylStructure", new_lyric);
      append_daughter(t,"NoteStruct",  new_lyric);
    }
  }
  user_lyrictoken_to_lyric_func = NIL;  // reset this

  return utt;
}

/*-----------------------------------------------------------------------*/

LISP l_lyric_it(LISP token, LISP tok)
{
	// Lisp wrap around for lyric_it
  EST_Item *t = get_c_item(token);
  EST_String tok_name = get_c_string(tok);

  return builtin_lyric_it(t,tok_name);
}

/*-----------------------------------------------------------------------*/

static LISP lyric_it(EST_Item *token, const EST_String tok)
{
    // User may specify own additional token to lyric rules
    // through the variable user_lyrictoken_to_lyric_func. If so we must
    // call that, which may or may not call the builtin version
    // The builtin version is bound in LISP so that recursion works
    // properly
    // This takes a LISP utt as an argument as creating a new wraparound
    // will cause gc to fail.
	LISP tok_string = strcons(tok.length(),tok);

  if (user_lyrictoken_to_lyric_func != NIL)            // check user's rules
		return leval(cons(user_lyrictoken_to_lyric_func,
																			cons(siod(token),
																					cons(tok_string,NIL))),NIL);
  else
		return builtin_lyric_it(token,tok);
}

/*-----------------------------------------------------------------------*/

static LISP builtin_lyric_it(EST_Item *token, EST_String tok)
{
  // Return a list of lyrics for this token
  remove_lead_trail_whitespace(tok);
  if (tok == "")
    return NIL;

  ///////////////////////////////////////////////////////
  else if (tok.contains("-"))
	{
		/*	Hyphenation. If the hyphen is in front or at end,
				return the lyric without the hyphen. If it is
				in the middle somewhere, join the bits on either
				side of it and return that.
				DJLB. Changed to accommodate a leading AND trailing
				hyphen in one lyric token.
		*/
		EST_String esTemp(tok);
    if (esTemp.before("-") == "")
		{
      if (esTemp.after("-") == "")
				return NIL;	//There was only a hyphen!
      else
				esTemp = esTemp.after("-"); // There was a leading hyphen.
    }

    if (esTemp.after("-") == "")
				return lyric_it(token, esTemp.before("-"));
      else
				return append(lyric_it(token, esTemp.before("-")),
														lyric_it(token, esTemp.after("-")));
  }


  ///////////////////////////////////////////////////////
  else if (tok.contains(RXwhite))
	{ //  whitespace inside a lyric:
    // separate into 2 lyric items

    return append(lyric_it(token, tok.before(RXwhite)),
											lyric_it(token, tok.after(RXwhite)));
  }

  ///////////////////////////////////////////////////////
  else
	{
    if (tok.contains("/"))
		if (Flinger_get_debug_file_flag()) {
			cout << "WARNING:  lyrics contain '/' - input will"
								" be treated as text, ignoring '/'."<< endl;
		}

    // last resort: just take the lyric, drop all other punct
    //return cons(strintern(downcase(remove_punct(tok))),NIL);
    return cons(strintern(downcase(tok)),NIL);
  }
}

/*-----------------------------------------------------------------------*/

static EST_String fesRemoveUnWantedCharacters(const EST_String &tok)
{
  EST_String np(tok);
  np.make_updatable();
  np.gsub(RXUnWantedChars, "");
  return np;
}

/*-----------------------------------------------------------------------*/

static EST_String remove_lead_trail_whitespace(const EST_String &tok)
{
  // removes leading and trailing whitespace
  EST_String np(tok);
  np.make_updatable();
  if (np.contains(RXwhite,0))
    np = np.after(RXwhite);
  if (np.contains(RXwhite,np.length()))
    np = np.before(RXwhite);
  return np;
}

/*-----------------------------------------------------------------------*/

static EST_String fesRemoveMultipleTrailingHyphens(const EST_String &tok)
{	//Added by DJLB 04-09-2006.
	// (basic_string is used because of the convenience of its
	//   'find_last_of' member function.)
	if(tok.contains("-"))
	{
		int inPosition;
		string stTemp(tok);
		inPosition = stTemp.find_last_not_of("-");
		if((int)stTemp.length() > inPosition + 2)
		{
			stTemp = stTemp.substr(0, inPosition + 1);
			return stTemp.c_str();
		}
		else
			return tok;
	}
	else
			return tok;
}

/*-----------------------------------------------------------------------*/

static EST_String return_post_punct(const EST_String &tok)
{
  //cout << "return_post_punct:  this is WRONG!\n";
  //EST_String punc = tok.after(RXnotpunctuation);
  //return punc;
	/*
By DJLB 27-08-2006. I have fixed this. As originally written, it
	returned everything in the string after the first char that is
	not in the defined (punctuation) list. What it should do, and
	now does, is return any punctuation after the last character that
	is not in the defined (punctuation) list.
(It is unfortunate that EST_String does not have a 'reverse-find' function!)
*/
	// Do a reverse search of the string for something not in the defined list.
	int inPosition;
	string stTemp(tok);
	inPosition = (int)stTemp.find_last_not_of(RXnotpunctuation);
	stTemp = stTemp.substr(inPosition + 1);
	return stTemp.c_str();
}

/*-----------------------------------------------------------------------*/

// the following 2 functions are based on festival/modules/base/word.cc

EST_Item *add_lyric(EST_Utterance *u, const EST_String &name)
{
  EST_Item *item = u->relation("Word")->append();
  item->set_name(name);
  return item;
}

/*-----------------------------------------------------------------------*/

EST_Item *add_lyric(EST_Utterance *u, LISP lyric)
{
  // Build a Lyric item from the Lisp description, which may
  // contain other features
  LISP f;
  EST_Item *si_lyric;
  int has_name = FALSE;

  if (consp(lyric))
  {
		// feature form
		si_lyric = add_lyric(u,"");
		for (f=lyric; f != NIL; f=cdr(f))
		{
	    if (streq("name",get_c_string(car(car(f)))))
	    {
				has_name = TRUE;
				si_lyric->set_name(get_c_string(car(cdr(car(f)))));
	    }
	    else
				si_lyric->set(get_c_string(car(car(f))),
														get_c_string(car(cdr(car(f)))));
		}
		if (!has_name)
		{
	    cerr << "add_lyric: lyric has description but no name" << endl;
	    cerr << "  " << siod_sprint(lyric) << endl;
	    EST_error("...");
		}
  }
  else // just the name
		si_lyric = add_lyric(u,get_c_string(lyric));

  return si_lyric;
}

/*-----------------------------------------------------------------------*/

