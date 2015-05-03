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

#include <stdlib.h>
#include "EST.h"
#include "festival.h"
#include "fl_io.h"
#include "OGI_Macro.h"



static void set_segment_default_durations(EST_Utterance *u);
static void get_syllable_default_durations(EST_Utterance *u, EST_Item *syl);
static void get_syllable_target_durations(EST_Utterance *u, EST_Item *syl);
static void set_segment_target_durations(EST_Item *syl);
static void insert_rest(EST_Item *syl, float endtime);
static float onset_stretch(EST_Item *syl);
static void onset_relax(EST_Item *syl);
static float vowel_stretch(EST_Item *syl);
static float coda_stretch( EST_Item *syl);
static void set_final_silence_target_duration(EST_Utterance *u);
static EST_Item *find_nucleus(EST_Item *seg);



LISP FL_NoteDur_Utt(LISP utt){
  //  cout << "* FL_NoteDur\n";

   EST_Utterance *u = get_c_utt(utt);
   EST_Item *syl;

  // For each Syllable,
  //     predict durations of all consonants from phonetic
  //       context - just use avg durs for now

   //   what about cases like "ch-ch-ch-changes...." -- cons-only 'syllables'?
   //   maybe just set a new feature "psuedo-vowel" and check for
   //       this, then treat as V-only syllable -- or insert schwa

  // predict default duration of segments
  set_segment_default_durations(u);

//DJLB addition
  if (Flinger_get_debug_file_flag()) {
	cout << endl << "GENERATED SYLLABLE DETAILS:" << endl;
	cout << "ON and OFF times are seconds from start." << endl;
  }

//End DJLB addition


  for (syl=u->relation("Syllable")->first(); syl != 0; syl = syl->next()){

    // vowel, coda, next onset default durations (from phonemes)
    get_syllable_default_durations(u, syl);

    // vowel, coda, next onset target durations (from notes)
    get_syllable_target_durations(u, syl);

    // derive target duration of segments
    set_segment_target_durations(syl);
  }

  // final silence
  set_final_silence_target_duration(u);

  return utt;
}

static void set_segment_default_durations(EST_Utterance *u){
  // Predict default duration of segments from phonetic properties only.
  EST_Item *s;
  float dur;
  LISP ph_durs,ldur;
  float stretch;

  ph_durs = siod_get_lval("fl_phoneme_durations","no phoneme durations");

  EST_Features *gl = get_Flinger_params();

  for (s=u->relation("Segment")->first(); s != 0; s = s->next()){
    ldur = siod_assoc_str(s->name(),ph_durs);

    if (ph_is_consonant(s->name()))
      stretch = gl->F("consonant_stretch");  // make this controlled by "feel" later
    else
      stretch = 1.0;

    if (ldur == NIL){
			cerr << "WARNING: Phoneme '" << s->name()
							<< "' has no default duration listed, so set to 0.1 sec." << endl;
      dur = (float)0.100;
    }
    else
      dur = get_c_float(car(cdr(ldur)));
    s->set("def_dur", dur*stretch);
  }
}


static void get_syllable_default_durations(
				    EST_Utterance *u,
				    EST_Item *syl){
  float Vdd=0.0,Cdd=0.0,Odd = 0.0; // vowel,coda,onset default dur
  EST_Item *seg,*next_syl;

  // special case for onset of 1st syllable in utt
  if (syl == u->relation("Syllable")->first()){
    for (seg=daughter1(syl,"SylStructure"); seg != 0; seg=seg->next()){
      if (ph_is_vowel(seg->name()))
	break;
      Odd += seg->F("def_dur");
    }
    syl->set("onset_def_dur",Odd);
  }

  // loop thru segs in VOWEL, CODA, NEXT_ONSET to sum default durs
  seg = find_nucleus(syl);
  Vdd = seg->F("def_dur");      // vowel
  seg = seg->next();
  for ( ; seg != 0; seg=seg->next()){
    Cdd += seg->F("def_dur");    // coda
  }
  syl->set("vowel_def_dur",Vdd);
  syl->set("coda_def_dur",Cdd);


  // onset of next syllable
  Odd = 0.0;
  next_syl = as(as(syl,"Syllable")->next(),"SylStructure");
  if (next_syl != NULL){
    for (seg=daughter1(next_syl); seg != 0; seg=seg->next()){
      if (ph_is_vowel(seg->name()))
	break;
      Odd += seg->F("def_dur");
    }
    next_syl->set("onset_def_dur",Odd);
  }
}


static void get_syllable_target_durations(
					  EST_Utterance *u,
					  EST_Item *syl){

  float Vt=0., Ct=0., Ot=0., Rt=0.;  // target durations
  float next1stON=0.0;
  int last_one = FALSE;

  // special case for onset of 1st syllable in utt
  if (syl == u->relation("Syllable")->first()){
    Ot = syl->F("onset_def_dur");
    syl->set("onset_targ_dur",Ot);
  }
   else if (syl == u->relation("Syllable")->last()){ // last syll of utt
    last_one = TRUE;
  }

// DJLB 28-05-06 if (print_durs)  cout << "\n" << syl->name() << endl;
  if (Flinger_get_debug_file_flag()) {
	cout << "\nLyric {" << parent(as(syl,"SylStructure"))->name() << "}: ";
	cout << "Syllable generated {" << syl->name() << "}" << endl;
 }

  // normal case
  float Vdd = syl->F("vowel_def_dur");  // vowel default duration
  float Cdd = syl->F("coda_def_dur");   // coda ...
  float Odd = 0.0;
  if (!last_one)
    Odd = as(syl,"Syllable")->next()->F("onset_def_dur");// onset ...

  float this1stON   = daughter1(as(syl,"NoteStruct"))->F("on");
  float thislastOFF = daughtern(as(syl,"NoteStruct"))->F("off");

//DJLB addition
  if (Flinger_get_debug_file_flag()) {
	cout << daughter1(as(syl,"NoteStruct"))->S("BarTime") << " ";
  }
//End DJLB addition

  if (last_one)
    next1stON   = thislastOFF;
  else // 1st noteON of next syllable
    next1stON = daughter1(as(as(syl,"Syllable")->next(),"NoteStruct"))->F("on");
//DJLB Changed printout to more suit user analysis of the interpretation of the music.
  if (Flinger_get_debug_file_flag()) {
	cout << "Note: " << daughter1(as(syl,"NoteStruct"))->name()
					<< ". Times (sec.): On= " << this1stON
							<< " Off= " << thislastOFF
									<< " Next On=" << next1stON << "\n";
  }

  if (Vdd + Cdd + Odd < next1stON - this1stON){
    if ((Vdd+Cdd <= thislastOFF - this1stON) && (Odd <= next1stON - thislastOFF)){
      // stretch vowel only to fill note, enough room for others, insert REST
      Ct = Cdd;      Ot = Odd;
      Vt = thislastOFF - this1stON - Cdd;
      //printf("CASE %d:\tVt=%1.3f Ct=%1.3f Ot=%1.3f \tVdd=%1.3f Cdd=%1.3f Odd=%1.3f \n",
	     //1, Vt,Ct,Ot, Vdd,Cdd,Odd);
    }

    else if ((Vdd < thislastOFF - this1stON) && (Vdd+Cdd > thislastOFF - this1stON)) {
      // enough room, no stretch needed anywhere, insert REST
      Ct = Cdd;      Ot = Odd;      Vt = Vdd;
      //printf("CASE %d:\tVt=%1.3f Ct=%1.3f Ot=%1.3f \tVdd=%1.3f Cdd=%1.3f Odd=%1.3f \n",
	    // 2, Vt,Ct,Ot, Vdd,Cdd,Odd);
    }

    else if (Odd > next1stON - thislastOFF) {
      // stretch vowel only, enough room for others, no rest
      Ct = Cdd;      Ot = Odd;
           // wrong:     Vt = thislastOFF - this1stON - (Ct+Ot);
      Vt = next1stON - this1stON - (Ct+Ot);
      //printf("CASE %d:\tVt=%1.3f Ct=%1.3f Ot=%1.3f \tVdd=%1.3f Cdd=%1.3f Odd=%1.3f \n",
	    // 3, Vt,Ct,Ot, Vdd,Cdd,Odd);
    }

    else if (Vdd > thislastOFF - this1stON) {
      // squish vowel, keep C and O same, insert REST
      Ct = Cdd;      Ot = Odd;
      Vt = thislastOFF - this1stON;
      //printf("CASE %d:\tVt=%1.3f Ct=%1.3f Ot=%1.3f \tVdd=%1.3f Cdd=%1.3f Odd=%1.3f \n",
	    // 4, Vt,Ct,Ot, Vdd,Cdd,Odd);
    }
    else
      EST_error("fl_duration: fell through if's!\n");
  }
  else { // (Vdd + Cdd + Odd > next1stON - this1stON)
    float r;
    if (Vdd > thislastOFF - this1stON) {

      if (next1stON - thislastOFF > 0.2*(Cdd+Odd)){ // enough room to squish C+O
	if (Cdd + Odd < next1stON - thislastOFF){
	  // squish V to meet noteOFF, leave C and O, insert REST
	  Ct = Cdd;	Ot = Odd;
	  Vt = thislastOFF - this1stON;
	  //printf("CASE %d:\tVt=%1.3f Ct=%1.3f Ot=%1.3f \tVdd=%1.3f Cdd=%1.3f Odd=%1.3f \n",
	  //    	5, Vt,Ct,Ot, Vdd,Cdd,Odd);
	}
	else {
	  // squish V to meet noteOFF, squish C and O uniformly to fit remaining
	  Vt = thislastOFF - this1stON;
	  //	r = (next1stON-this1stON - Vt)/(Cdd+Odd);
	  r = (next1stON-thislastOFF)/(Cdd+Odd);
	  Ct = Cdd*r;  Ot = Odd*r;
	  //printf("CASE %d:\tVt=%1.3f Ct=%1.3f Ot=%1.3f \tVdd=%1.3f Cdd=%1.3f Odd=%1.3f \n",
	  //      6, Vt,Ct,Ot, Vdd,Cdd,Odd);
	}
      }
      else {
	// squish all 3 uniformly
	r = (next1stON-this1stON)/(Vdd+Cdd+Odd);
	Ct = r*Cdd;   Ot = r*Odd;    Vt = r*Vdd;
	//printf("CASE %dA:\tVt=%1.3f Ct=%1.3f Ot=%1.3f \tVdd=%1.3f Cdd=%1.3f Odd=%1.3f \n",
	 //      7, Vt,Ct,Ot, Vdd,Cdd,Odd);
      }
    }
    else {  /// could do better on this one
      // squish all 3 uniformly
      r = (next1stON-this1stON)/(Vdd+Cdd+Odd);
      Ct = r*Cdd;   Ot = r*Odd;    Vt = r*Vdd;
      //printf("CASE %dB:\tVt=%1.3f Ct=%1.3f Ot=%1.3f \tVdd=%1.3f Cdd=%1.3f Odd=%1.3f \n",
			//     7, Vt,Ct,Ot, Vdd,Cdd,Odd);
    }
  }

  EST_Features *gl = get_Flinger_params();

  Rt = max((float)0.0, next1stON-this1stON - (Vt+Ct+Ot));
  if (Rt < gl->F("min_rest_dur")){
    Rt = 0.0;
  }

  syl->set("vstart",this1stON);  // start time of vowel
  syl->set("vowel_targ_dur",Vt);
  syl->set("coda_targ_dur",Ct);
  syl->set("post_rest_dur",Rt);
  if (!last_one)
    as(syl,"Syllable")->next()->set("onset_targ_dur",Ot);

}

static void set_segment_target_durations( EST_Item *syl){

  float endtime=0.0,targ;

  //  ONSET
  endtime = onset_stretch(syl);

  // if end is negative (i.e., there was no rest at beginning),
  //   then relax vstart constraint - make Onset+Vowel fit in V slot.
  if (endtime < 0.0){
    onset_relax(syl);

    // try again
    endtime = onset_stretch(syl);
    if (endtime < 0.0)
      cerr << "onset_relax didn't work!  endtime=" << endtime << endl;
  }

  //  VOWEL
  vowel_stretch(syl);

  //  CODA
  endtime = coda_stretch(syl);

  // insert silence for REST if there is one
  if ((targ = syl->F("post_rest_dur")) != 0.0){
    insert_rest(syl, endtime+targ);
  }
}

static void insert_rest(EST_Item *syl, float endt){
  EST_Item *silseg;

  // insert into Segment relation
  silseg = as(daughtern(as(syl,"SylStructure")),"Segment")->insert_after();
  silseg->set_name(ph_silence());
  silseg->set("end",endt);
}

static float onset_stretch(EST_Item *syl)
{
	float endtime=0.0,stretch,def,targ;
	EST_Item *seg,*prev_seg;

	if ((def = syl->F("onset_def_dur")) != 0.0)
	{
		targ = syl->F("onset_targ_dur");
		stretch = targ/def;
		syl->set("onset_stretch",stretch); // debug
	}
	else
		stretch = 1.0;

	// loop backwards from vowel (nucleus) start time
	endtime = syl->F("vstart");
	seg=find_nucleus(syl)->prev();
	for ( ; seg != 0; seg = seg->prev())
	{
		seg->set("end",endtime);
		def = seg->F("def_dur");
		endtime -= stretch*def;
	}

	// if silence before 1st seg in syll, set end of sil
	//  else check for agreement of times
	seg = daughter1(syl->as_relation("SylStructure"));
	prev_seg = as(seg,"Segment")->prev();

	if (prev_seg != 0)
	{
  		if (ph_is_silence(prev_seg->name()))
		{
			prev_seg->set("end",endtime); // set end of sil
		}
    //    else{
    //      if (endtime - prev_seg->F("end") > 0.01){ // not close enough
    //	cerr << "alignment error:  endtime=" << endtime << " prevseg_endtime="
    //	     << prev_seg->F("end") << " in " << prev_seg->name() << endl;
    //	     EST_error("...");
    //     }

	/*DJLB 20-08-2006. Glottal stop is not recognised as a silence,
	   and it has no duration. This was resulting in an error
	   (prev_seg's endtime not defined) when stretching the next vowel.
	   when the previous segment is a glottal stop, set its end time to
	   that of the segment before it. */
		if(prev_seg->name() == "?")
		{
		EST_Item * p_prevseg;
			p_prevseg = prev_seg->prev();
			endtime = p_prevseg->F("end");
			//Delete the glottal stop segment
			/*	p_prevseg->set("end",endtime);
				prev_seg->unref_all();*/
			// OR
			// change it to silence, and insert it in place of a short
			// bit of the previous segment.
			p_prevseg->set("end",endtime - 0.002);
			prev_seg->set("name", ph_silence());
			prev_seg->set("dur", 0.002);
			prev_seg->set("end",endtime);
		}
	}
	else
	{// no prev segment -- Hmmm, should at least be a silence!
		cerr << "NO previous segment!   endtime=" << endtime << endl;
		EST_error("...");
	}

	return endtime;
}

#define TINY_SIL 0.010  // length of mandatory silence at beginning when no rest
static void onset_relax(EST_Item *syl){
  //  Change onset_target_duration, vowel_target_duration, and vowel_start
  //  so that we can fit [tiny_silence + onset + vowel] into 1st note.
  float onset_def,onset_targ,squish,vowel_def,vowel_targ;

  vowel_def  = syl->F("vowel_def_dur");
  vowel_targ = syl->F("vowel_targ_dur");

  // set onset_targ to min of itself and its default dur
  onset_def  = syl->F("onset_def_dur");
  onset_targ = syl->F("onset_targ_dur");
  syl->set("onset_targ_dur",(float) min(onset_def,onset_targ));

  if (TINY_SIL+onset_targ+vowel_def < vowel_targ){ // fits
     syl->set("vstart",(float) (onset_targ+TINY_SIL));
     syl->set("vowel_targ_dur",(float) (vowel_targ-(onset_targ+TINY_SIL)));
     return;
  }
  else {  // find squish_factor and change vstart, vowel_targ, onset_targ so they fit
    squish = vowel_targ/((float)TINY_SIL+onset_targ+vowel_targ);
    syl->set("vstart", (float) (squish * (onset_targ+TINY_SIL)));
    syl->set("onset_targ_dur",(float) (squish*onset_targ));
    syl->set("vowel_targ_dur",(float) (squish*vowel_targ));
  }
}

static float vowel_stretch(EST_Item *syl){
  float endtime=0.0,stretch,def,targ;

  EST_Item *seg = find_nucleus(syl);
  EST_Item *prev_seg = as(seg,"Segment")->prev();

  if (prev_seg != 0)
    endtime = prev_seg->F("end");  // end of onset
  else
    endtime = 0.0;

  if ((def = syl->F("vowel_def_dur")) != 0.0){
    targ = syl->F("vowel_targ_dur");
    stretch = targ/def;
    syl->set("vowel_stretch",stretch); // for debug
    endtime += targ;
    seg->set("end",endtime);
  }
  else
    EST_error("no vowel target duration! \n");

  return endtime;
}


static float coda_stretch(EST_Item *syl){
float endtime,stretch,def,targ;

  EST_Item *seg = find_nucleus(syl);
  endtime = seg->F("end");

  seg = seg->next();
  if (seg == 0) { // there isn't a coda
    return endtime;
  }
  // CODA
  if ((def = syl->F("coda_def_dur")) != 0.0){
    targ = syl->F("coda_targ_dur");
    stretch = targ/def;
    syl->set("coda_stretch",stretch);
  }
  else
    stretch = 1.0;
  for ( ; seg != 0; seg = seg->next()){
    def = seg->F("def_dur");
    endtime += stretch*def;
    seg->set("end",endtime);
  }
  return endtime;
}

static void set_final_silence_target_duration(EST_Utterance *u){
  // merge multiple silences
  //EST_Item *s, *ns;
/*
  for (s=u->relation("Segment")->first(); s != 0; ){
    ns = next(s);
    // make sure same type of silence
    if ((ns != 0) &&
	(ph_is_silence(s->name())) && (s->name() == ns->name())) {
      // delete this segment
      remove_item(s,"Segment");
      remove_item(s,"SylStructure");
    }
    s = ns;
  }*/

  // make last silence correspond to last noteOFF
  float lastNoteOFF = u->relation("Note")->last()->F("off");

  // penultimate one is last spoken phone
  (u->relation("Segment")->tail())->prev()->set("end",lastNoteOFF);

  // last one is a silence
  u->relation("Segment")->tail()->set("end",lastNoteOFF + 0.1);

}

static EST_Item *find_nucleus(EST_Item *syl){
  EST_Item *seg = daughter1(syl->as_relation("SylStructure"));

  for ( ; seg != 0; seg = seg->next()){
    if (ph_is_vowel(seg->name()))
      break;
  }
  if (seg == 0){
    cerr << "find_nucleus: no vowel found in " << syl->name() << endl;
    return daughtern(syl->as_relation("SylStructure"));
  }
  return seg;
}

