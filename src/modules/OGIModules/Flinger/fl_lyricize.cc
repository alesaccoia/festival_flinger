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
//   Many functions in this file adapted from
//                   festival/src/modules/base/word.cc
//

/*
Comment by DJLB 29-08-2006.
	Changes made to this file and fl_token.cpp (hopefully) provide for
	proper pronunciation of hyphenated lyric tokens. Prior to these changes,
	lyrics would sometimes be mispronounced. For instance, if the lyricist
	splits the word "baby" so that the first syllable 'ba' is sung
	on one note, whilst the second - 'by'- is sung on the following note,
	this would written in lyrics as "... 'ba-'   'by' ...". Originally,
	Flinger would have sung that as 'ba' (as in 'bat') followed by 'by'
	(as in 'bye'). The modification I have incorporated detects the hyphen,
	using that detection to cause the two halves of the word to be catenated
	before getting the phoneset from the lexicon. The phones thus obtained
	are then used to build new phonesets for each lyric token, before the
	syllables and their segments are added to utt.
	Some of the code dealing with the Lisp items may appear rather clunky.
	Not being well versed in Scheme and the siod interface, it is the best I
	can do at this stage, whilst still retaining SOME understanding of what
	is going on. (A plee for better documentation on this aspect!)
*/
#include <stdlib.h>
#include "festival.h"
#include "lexicon.h"

static void check_multisyllabic(LISP entry);
static EST_Item *add_syllable(EST_Utterance *u,  LISP syl);
static EST_Item *add_segment(EST_Utterance *u, const EST_String &s);
static LISP specified_word_pronunciation(EST_Item *w, LISP lpos);
static LISP make_monosyllabic(LISP entry);
static LISP flsGetLexiconEntry(EST_Item *lyr);
static void fvdIncorporateLyric(EST_Utterance *u, EST_Item *lyr,
																														LISP entry);

/*-----------------------------------------------------------------------*/
//
// Note->LyricToken association to get timing
//
static void relate_notes_to_lyrictokens(EST_Utterance *u )
{
  EST_Item *l,*note;
  float next_lyr_time, note_on;

  note=u->relation("Note")->first();
  for (l=u->relation("LyricToken")->first(); l != 0; l=l->next())
	{
    u->relation("NoteWordAssoc")->append(l);

    if (l->next() != NULL)
      next_lyr_time = l->next()->F("time");
    else
      next_lyr_time = u->relation("Note")->last()->F("off");

    note_on = note->F("on");
    while (note_on < next_lyr_time)
		{
      append_daughter(l,"NoteWordAssoc", note);
      if ((note = note->next()) == NULL)
				break;
      note_on = note->F("on");
    }
  }
}
LISP FL_Lyricize_Utt(LISP utt)
{
  // Look up words in lexicon or use letter-to-sound rules
  //  and create syllable and segment relations
  EST_Utterance *u = get_c_utt(utt);
  EST_Item *lyr;
	LISP entry;
	relate_notes_to_lyrictokens(u);

  // Add silence at the beginning of the utt
  add_segment(u, ph_silence());

  for (lyr=u->relation("Word")->first(); lyr != 0; lyr = lyr->next())
	{
		/*
		DJLB 27-08-2006. Getting pronunciation right for hyphenated tokens:
				'lyr' may be the leading token of a group that need to be put
				together to get the pronunciation right. (The lexicon return for
				an individual syllable of a multi-syllable word will often be
				different to the phonem makeup for that syllable in the whole
				word entry.)
				Hyphenation is recognised by 'lyr's feature "post-punc"
				containing a hyphen (that was stored there when FL_Tokenize_Utt()
				found a hyphen as the last character in a lyric token).
		*/
		EST_String esWholeWord;
		EST_Item *pLyr, * pSyl;
		int inPartsCount = 0;
		esWholeWord = "";
		//Gather any split word.
		for(pLyr = lyr; pLyr != 0; pLyr = pLyr->next())
		{
			inPartsCount ++;
			esWholeWord += pLyr->name();
			pSyl = parent(pLyr->as_relation("SylStructure"));
			if(pSyl->S("post-punc") != "-")
				break;
		}

		if(inPartsCount > 1)//More than one part of the word in lyric tokens.
		{
			/* esWholeWord now contains a word built from several tokens.
			Get the lexicon entry for it, so that the phones and accents are
			correct. */
			EST_Item eiWholeWord;
			LISP lsWholeWordEntry, lsNewEntry;
			eiWholeWord.set("name", esWholeWord);
			lsWholeWordEntry = flsGetLexiconEntry(&eiWholeWord);

			/* Now take the syllables from WholeWordEntry, and use them
					as the lexicon entry for each part of the word, in turn.
			*/
			LISP lsSyllable;
			lsSyllable = car(cdr(cdr(lsWholeWordEntry)));
			for(pLyr = lyr; pLyr != 0; pLyr = pLyr->next())
			{
				//Make a lexicon-style 'entry' from the syllable.
				lsNewEntry = cons(strintern(pLyr->name()),
												cons(NIL,
														cons(cons(car(lsSyllable), NIL),NIL)));

// The next four lines were commented out, but perhaps they should not have been EAJ

				//if (!fvdIncorporateLyric(u, pLyr, lsNewEntry))
				//{
				//	EST_Item * pLastLyr = prev(pLyr);
				//}

                                fvdIncorporateLyric(u, pLyr, lsNewEntry);

				//Move on to the next syllable.
				lsSyllable = cdr(lsSyllable);
				inPartsCount --;				if(inPartsCount <= 0)
					break;
			}//for pLyr

			// Move the main loop pointer on.
			lyr = pLyr;
		}//if InPartsCount > 1
		else
		{	// A lyric token that is not part of a hyphenated word.
			entry = flsGetLexiconEntry(lyr);
			fvdIncorporateLyric(u, lyr, entry);
		}//else if InPartsCount > 1
  }//for 'lyr through Words

  // add silence at the end of the utt
  add_segment(u, ph_silence());

  return utt;
}

/*-----------------------------------------------------------------------*/

static void fvdIncorporateLyric(EST_Utterance *u, EST_Item *lyr,
																														LISP entry)
{
/* DJLB comment.
		NOTE: entry, s, p are LISP thingohs.
		For the syllable contained in the lyric token 'entry' (a LISP thingoh),
		extract each segment, add the syllable as a SylStructure to the
		'Word' item in utt, and add the segments as a 'seg' list in
		that SylStructure.
		Also append SylStructure and NoteStruct as daughters to
		the SylStructure in the 'Word' item.*/

	// Make the entry monosyllabilic.
	entry = make_monosyllabic(entry);
	// Ensure that it is monosyllabic.
	check_multisyllabic(entry);

	EST_Item *lyr_tok,*syl,*seg,*note;
	LISP s,p;
	if(car(car(car(car(cdr(cdr(entry)))))) == NIL)
	{
		cout << endl <<	"WARNING: The word that contains the "
										"lyric {" << lyr->name() << "} "
										"has less syllables than the number of "
										"hyphenated parts shown in the lyrics."
										<< endl << endl;
		//Stretch the previous syllable to cover this note:-
		// The note associated with the current lyric has to be
		//  associated with the previous syllable.
		lyr_tok = parent(lyr->as_relation("SylStructure"));
		EST_Item * pLastLyricToken;
		pLastLyricToken = lyr_tok->prev();
		note = daughter1(lyr_tok->as_relation("NoteWordAssoc"));
		append_daughter(pLastLyricToken,"NoteStruct",note);

	//	return true;
	}

	for (s=car(cdr(cdr(entry))); s != NIL; s=cdr(s))
	{
		syl = add_syllable(u, s);
		// append syl as daughter to Word in SylStructure
		append_daughter(lyr,"SylStructure",syl);

		// append syl as daughter to Word in NoteStruct
		append_daughter(lyr,"NoteStruct",syl);

		// append segments as daughters of Syllable
		//    in SylStructure only
		for (p=car(car(s)); p != NIL; p=cdr(p))
		{
			seg = add_segment(u,get_c_string(car(p)));
			append_daughter(syl,"SylStructure",seg);
		}// for p

		// find notes that are associated with this syllable
		//  and append as daughters in NoteStruct only
		lyr_tok = parent(parent(syl->as_relation("SylStructure")));
		note = daughter1(lyr_tok->as_relation("NoteWordAssoc"));
		for ( ; note != NULL; note=next_sibling(note))
		{
			append_daughter(syl,"NoteStruct",note);
		}//for note
	}//for s
	//return true;
}

/*-----------------------------------------------------------------------*/

static void check_multisyllabic(LISP entry){
  int k = 0;
  LISP s,p;

  for (s=car(cdr(cdr(entry))); s != NIL; s=cdr(s))
    k++;

  if (k>1)
	{
    cerr << "Can't have multisyllable word on one note:  (";
    for (s=car(cdr(cdr(entry))); s != NIL; s=cdr(s))
		{
      cerr << "(";
      for (p=car(car(s)); p != NIL; p=cdr(p))
			{
				cerr << get_c_string(car(p)) << " ";
      }
      cerr << ")";
    }
    cerr << ")";
    EST_error("...");
  }
}

/*-----------------------------------------------------------------------*/

static LISP make_monosyllabic(LISP entry)
{
  //  strip off phones in each syllable and merge into one big syl
  //
  int k = 0;
  LISP s,p;
  LISP phlist=NIL;
  for (s=car(cdr(cdr(entry))); s != NIL; s=cdr(s))
	{
    k++;
	}
  if (k==1)
    return entry;

  for (s=car(cdr(cdr(entry))); s != NIL; s=cdr(s))
	{
    for (p=car(car(s)); p != NIL; p=cdr(p))
		{
      /// append
      phlist = append(phlist, cons(car(p),NIL));
    }
  }

  phlist = append(cons(car(entry),NIL),append(cons(NIL,NIL),
									cons(cons(cons(phlist, cons(flocons(1), NIL)),NIL),NIL)));
  return phlist;
}

/*-----------------------------------------------------------------------*/

static EST_Item *add_syllable(EST_Utterance *u,  LISP entry)
{
  EST_Item *item = u->relation("Syllable")->append();
  int stress = get_c_int(car(cdr(car(entry))));

  // make syl name for debug
  LISP p = car(car(entry));
  EST_String name = get_c_string(car(p));
  for (p=cdr(p); p != NIL; p=cdr(p))
    name = name + "_" + get_c_string(car(p));

  item->set_name(name);
  item->set("stress",stress);

  return item;
}

/*-----------------------------------------------------------------------*/

static EST_Item *add_segment(EST_Utterance *u, const EST_String &s)
{
  EST_Item *item = u->relation("Segment")->append();
  item->set_name(s);
  return item;
}

/*-----------------------------------------------------------------------*/

static LISP specified_word_pronunciation(EST_Item *w, LISP lpos)
{
  //  If there is a phoneme feature on w or the LyricToken related to
  //  w use that as the pronunciation.  Note the value will be a string
  //  from which a list can be read.
  EST_String p;

  if (((p = (EST_String)ffeature(w,"phonemes")) != "0") ||
					((p = (EST_String)ffeature(w,"R:SylStructure.parent.phonemes")) != "0"))
	{
		LISP phones = read_from_lstring(strintern(p));

		return cons(strintern(w->name()),
													cons(lpos, cons(lex_syllabify(phones),NIL)));
  }
  else
    return NIL;
}

/*-----------------------------------------------------------------------*/

static LISP flLastResort(EST_String esLyric, LISP lpos, LISP entry)
{	/* By DJLB 04-09-2006
		If the 2nd last char is an apostrophe, just removing the apostrophe
		does not always produce the desired result e.g. "she's" becomes "shes"
		from which the vowel is pronounced e as in 'egg', instead of the
		desired e as in 'she'.
		The 'last resort' for such cases and where the lexicon fails to return
		anything, is to get phones for before the apostrophe, then
		add a phone for the letter after it, as found in the whole word with
		the apostrophe removed. In the above example, the phones from CMU for
		"she" are (S i:). The phones for "shes" are (S E z). The desired
		phones for the word "She's" are then (S i: z).
	*/
		LISP entry1, entry2, phones, syllable, last_phone, phlist;
		EST_String esTemp1, esTemp2;
	if(esLyric.at(esLyric.length() - 2, 1) == "'")
	{
		esTemp1 = esLyric.before("'");//Pre-apostrophe part of the word.
		esTemp2 = esTemp1 + esLyric.after("'");//Word with apostrophe removed.
		entry1 = lex_lookup_word(esTemp1,lpos);
		entry2 = lex_lookup_word(esTemp2,lpos);

		//Get the phones for the pre-apostrophe part as a word.
   	phlist = NIL;
		syllable = car(cdr(cdr(entry1)));
		for( phones = car(car(syllable)); phones != NIL; phones = cdr(phones))
		{	//Append each phone to a list.
			phlist = append(phlist, cons(car(phones),NIL));
		}

		//Get the last phone for the word with the apostrophe removed.
		// This involves running through all the phones, saving the last one
		// (because siod does not seem to have a 'reverse phones' function).
		syllable = car(cdr(cdr(entry2)));
		for (phones = car(car(syllable)); phones != NIL; phones = cdr(phones))
		{	//Store the phones. The last time this is done, 'phones' will
			// contain only the last phone in the syllable.
			last_phone = phones;
    	}
		//Add the last phone to the list.
		phlist = append(phlist, cons(car(last_phone),NIL));

		//Now make a new 'entry' for the original word.
		return cons(strintern(esLyric),
													cons(lpos, cons(lex_syllabify(phlist),NIL)));
	}
	else
		return entry;
}

/*-----------------------------------------------------------------------*/

static LISP flsGetLexiconEntry(EST_Item *lyr)
{	//Gets the phonetic interpretation of the lyric token input.
	LISP lpos, entry;
	EST_String pos;

	  lpos = NIL;
    pos = (EST_String)ffeature(lyr,"hg_pos"); // explicit homograph pos disambiguation
    if (pos == "0")
      pos = (EST_String)ffeature(lyr,"pos");
    if (pos != "0")
      lpos = rintern(pos);
    //  Check if there is an explicitly given pronunciation attached
    //   to the Word before going to the lexicon.
    //    --  also use this later for the "syllabified orthography" case
    if ((entry = specified_word_pronunciation(lyr,lpos)) == NIL)
		{
      entry = lex_lookup_word(lyr->name(),lpos);
			// DJLB 04-09-2006.
			if(car(cdr((cdr(entry)))) == NIL)
			{
				//Maybe a leading or trailing apostrophe on this token is unrecognised.
				EST_String esTemp(lyr->name());
				if(esTemp.at(0,1) == "'")
				{
					esTemp = esTemp.after(0);
					entry = lex_lookup_word(esTemp,lpos);
				}
				else if( esTemp.at(esTemp.length()-1,1) == "'")
				{
					esTemp = esTemp.before(esTemp.length()-1);
					entry = lex_lookup_word(esTemp,lpos);
				}

				if((car(cdr((cdr(entry)))) == NIL) &&
					 (esTemp.at(0,1) == "'") &&
					 (esTemp.at(esTemp.length()-1,1) == "'"))
				{
					esTemp = esTemp.after(0);
					esTemp = esTemp.before(esTemp.length()-1);
					entry = lex_lookup_word(esTemp,lpos);
				}
			}
			if(car(cdr((cdr(entry)))) == NIL)
				entry = flLastResort(lyr->name(), lpos, entry);
		}
    // give a POS if we have one
    if (lpos == NIL)
      lyr->set("pos",get_c_string(car(cdr(entry))));

		return entry;
}

/*-----------------------------------------------------------------------*/
