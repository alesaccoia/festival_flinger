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
#include "festival.h"
#include "EST.h"
#include "fl_midi_in.h"

#include "fl_duration.h"
#include "fl_lyricize.h"
#include "fl_pitch.h"

#include "fl_token.h"
#include "fl_io.h"
#include "fl_globals.h"


LISP FL_synth_and_save_midifile(LISP lfilename);

LISP FL_sing_midifile(LISP filename);
static void sing_utt(LISP utt);
static LISP new_song_utt(void);
static LISP Flinger_Init(LISP params);

LISP FL_sing_midifile(LISP lfilename,LISP mode){
//DJLB addition.
/*
	Redirect stdout and stderr to a text file with same pathname as 
	the .scm file being processed. That way, a readable output, with errors
	printed as they occur, is obtained.
*/
EST_String filename = get_c_string(lfilename);

/*
DJLB 25-05-06. 
Need to have different files for different tracks, if this instance is
	one of many handling a multi-track midi file.
*/
EST_Features *gl = get_Flinger_params(); //DJLB 13-05-06
EST_String esTrackName(gl->S("voice_track_name")); //DJLB 10-08-08
int inTrackNumber(gl->I("voice_track_number"));//DJLB 10-08-08
int inChannel(gl->I("channel_number")); //DJLB 20-07-08
EST_String esOutfile;

    
    //If the track name is nominated, use it. If not, use the track number.
    // If neither are given, the 1st track on which notes are found will be used..
    if(esTrackName.length() <= 0)
        if(inTrackNumber != DEF_TRACKNUMBER)
        {
            char chNum[18];
           // itoa(inTrackNumber,chNum,10);
            sprintf(chNum, "%d", inTrackNumber);
            esTrackName = "Track ";
            esTrackName += chNum; 
        }
        else
            esTrackName = "First Note Track";
    else
        esTrackName = esTrackName + " Track";

    esOutfile = filename.before(".") + " Build Details " + esTrackName;
    if((inChannel > 0) && (inChannel <=16))
    {
        char chChannel[3];
        //itoa(inChannel, chChannel, 10);
        sprintf(chChannel, "%d", inChannel);
        esOutfile += " Channel "; 
        esOutfile += chChannel; 
    }
	esOutfile = esOutfile + ".txt";
  
  //
  // sing with a "mode" -- maybe we could use this later
  //  to say "sing with a jazzy feel"  etc...
  //
  // YK 2011-07-25 moved this code up and added debug mode.
  //
  LISP user_modes;
  user_modes = siod_get_lval("fling_modes",NULL);

  Flinger_set_debug_file_flag(0);

  if ((mode == NIL) || (streq(get_c_string(mode),"default")))
    ;  // cout << "\tsing_midifile: default mode \n";

  else if (streq(get_c_string(mode),"debug")) {
    // YK 2011-07-25:
	//
    // To properly redirect, the code should have a redirect func
	// in siod/io/etc and we get access to it via EST_h.h or
    // festival.h. I don't know enough about this program to make the
    // changes. DJLB should have done this. That's why the debug
    // mode hangs -- there's no easy access to restore the redirect.
	Flinger_set_debug_file_flag(1);

  }else {
    cerr << "sing_midifile: can't find mode description \"" 
	 << get_c_string(mode) << "\" using default mode 'nil instead" << endl;
  }

    FILE *fout, *ferr;

	if (Flinger_get_debug_file_flag()) {
		remove(esOutfile);
		fout = freopen(esOutfile,"a+",stdout);
		ferr = freopen(esOutfile,"a+",stderr);
	}

  //  cout << "* sing_midifile\n";
  LISP lutt=NIL;

  // if user has never called Flinger.init, set defaults
  if (!(Flinger_params_loaded()))
    Flinger_init_params(NIL);

  //
  // create empty utterance
  //
  lutt = new_song_utt();

  //
  // read midifile
  // parse midi data and put into items of utterance
  //
//DJLB modification
//   Original code simply called midifile_to_utt(), sing_utt() and returned lutt.
	if( midifile_to_utt(filename, lutt) != NULL )
	{
		sing_utt(lutt);

		if (Flinger_get_debug_file_flag()) {
			fflush(fout);
			fflush(ferr);
			cout << endl << "    *********** Finished ***********" << endl;
			// return stdout and stderr to their normal handles.
		}

		return(lutt);
	}
	else
	{
		if (Flinger_get_debug_file_flag()) {
			cout << endl << "Cannot proceed due to errors in MIDI file." << endl;
		}
	}
//End DJLB modification

	return(lutt);
}


static void sing_utt(LISP utt){
  //  cout << "* sing_utt\n";
  // Sing this utt
  LISP lutt;

  // check whether utterance is empty
  if ((utt == NIL) ||
      (get_c_utt(utt)->relation("Note")->length() == 0))
    return;   // if so, do nothing
  
  lutt = quote(utt);
  
  // apply sing_hooks to utterance - synth and play it
  lutt = cons(rintern("apply_hooks"),
			cons(rintern("sing_hooks"),
				cons(lutt,NIL)));
  lutt = cons(rintern("set!"),
			cons(rintern("utt_sing"),
				cons(lutt,NIL)));
  // sing it
  lutt = leval(lutt,NIL);
  user_gc(NIL); // this gives "bad env" error in lisp (?)
}


static LISP new_song_utt(void){

    // An empty utterance of type Song, ready to be filled from MIDI relation
    LISP lutt;

    // Use the LISP routine to do it
    lutt =  cons(rintern("Utterance"),
		 cons(rintern("Song"),
		      cons(NIL,NIL)));
    lutt = leval(lutt,NIL);
    EST_Utterance *u = get_c_utt(lutt);

    // linear relations
    u->create_relation("LyricToken");// like festival "Token"
    u->create_relation("Word");     // like festival "Word"
    u->create_relation("Syllable");
    u->create_relation("Segment");
    u->create_relation("Note");
    u->create_relation("F0_Mod"); // pitch bend wheel and vibrato (modulation)
    u->create_relation("Target"); // F0 contour -- I don't like the name, but 
                                  // it keeps us compatible with OGIresLPC 
                                  // for now.
	u->create_relation("GainMod"); // Addition by DJLB

    //
    // tree/multilinear relations across the above:
    //

    // LyricToken->Word->Syllable->Note->F0_Mod - used to get pitch contour, timing
    u->create_relation("NoteStruct"); 

    // LyricToken->Note - used to make Note, MIDI Lyric event association
    u->create_relation("NoteWordAssoc"); 

    // LyricToken->Word->Syllable->Segment - used to get pronunciations, timing
    u->create_relation("SylStructure");

    //
    // future
    //
    // u->create_relation("VQuality");  // (linear relation) voice quality, vocal effort
    // u->create_relation("DynStruct"); // Syllable->Note->Dyn_Mod
    //
    // u->create_relation("Feel");  // preciseness of hitting downbeats, note off's, etc.
    // u->create_relation("FeelStruct"); // Syllable->Note->Feel

    return lutt;
}


LISP FL_Initialize_Utt(LISP utt){
  cout << "* FL_Initialize_Utt\n";
    // Main utterance initialization
    EST_Utterance *u = get_c_utt(utt);
    EST_String type;
 
    type = utt_type(*u);
 
    utt_cleanup(*u);  // delete all relationss except IForm
 
    if (type == "Song"){
      ;  // nothing special to do yet...
    }
    else{
      // error
      cerr << "Unknown utterance type \"" << type << "\" for initialization "
	   << endl;
      EST_error("..");
    }
    
    return utt;
}
 


//
// Init called from Scheme.
//
static LISP Flinger_Init(LISP params){
  // extract LISP params, clear old ones first
  Flinger_init_params(params);
  
  return NIL;
}
LISP Flinger_AddInit(LISP params){
  // if user has never called Flinger.init, set defaults
  if (!(Flinger_params_loaded()))
    Flinger_init_params(NIL);

  // extract LISP params, don't clear old ones
  Flinger_addto_params(params);
  
  return NIL;
}


void festival_Flinger_init(void){
  //
  //  Initialization
  //
  proclaim_module("Flinger");

  festival_def_utt_module("FL_Initialize",FL_Initialize_Utt,
			  "(FL_Initialize UTT)\n\
This module should be called first on all utterances.");

 festival_def_utt_module("FL_Tokenize",FL_Tokenize_Utt,
			  "(FL_Tokenize UTT)    \n\
Strips off (and keeps track of) punctuation symbols, etc.\n\
in LyricTokens.  Results in Word relation containing\n\
pronouncable words/syllables, no punctuation or symbols.");

 festival_def_utt_module("FL_Lyricize",FL_Lyricize_Utt,
			 "(FL_Lyricize UTT)    \n\
Figures out how to pronounce the lyrics.\n\
Turn the Word relation into Segments and Syllables: use\n\
- lexicon lookup\n\
- LtS rules\n\
- syllabification if needed\n\
- eventually fancier things like putting split\n\
words back together to lookup pronunciation,\n\\n\
then split again into syllables.\n\
Results in Segment relation with related Syllables.\n\
");

 festival_def_utt_module("FL_NoteDur",FL_NoteDur_Utt,
			 "(FL_NoteDur UTT)    \n\
Use relations between Segments, Syllables, Notes to get a \n\
duration for each Segment.  Also use default durations of\n\
Segments, other rules.   Result is duration for each Segment\n\
streamitem.\n\
");
 
 festival_def_utt_module("FL_NoteF0",FL_NoteF0_Utt,
			 "(FL_NoteF0 UTT)    \n\
Use relations between Segments, Notes, and ControlEvents to\n\
create an F0_Target relation.  This specifies target pitch, with\n\
attached features to describe vibrato, drift, etc.\n\
");
 
 init_subr_1("Flinger.init",Flinger_Init,
    "(Flinger_Init PARAMS)\n\
  Initialize parameters of vocalist (things that are not dynamically controllable). \n\
   PARAMS are an assoc list of parameter name and value.");
  
 init_subr_2("Flinger.sing",FL_sing_midifile,
	     "(Flinger.sing MIDIFILE MODE)\n\
Render MIDI data in MIDIFILE as singing.  Respect MODE.\n\
Modes will be defined through the variable fling_modes.\n\
A mode of 'debug will create a debug file but it hangs\n\
the system when done. No other modes are defined.");

 init_subr_2("builtin_lyrictoken_to_lyric",l_lyric_it,
	     "(builtin_lyrictoken_to_lyric LYRICTOKEN_RELATION LYRICTOKEN_NAME)\n\
Returns a list of words expanded from LYRICTOKEN_NAME.  Note that as this\n\
function may be called recursively. LYRICTOKEN_NAME might not be the name of\n\
LYRICTOKEN_RELATION depending on the rules applied.");
 }


