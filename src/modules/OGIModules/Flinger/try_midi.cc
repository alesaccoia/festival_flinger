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

#include "festival.h"
#include "MIDI/Song.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "fl_midi_parse.h"
#include "fl_midi_print.h"

void read_midifile(const char *fname){

  int fd;
  short track;
  Event *event;
  Song mf;

  if ((fd = _open (fname, O_RDONLY, 0)) == -1){
    cout << "couldn't open file \n";
    festival_error();
  }

  mf.SMFRead(fd);



  cout << "*** opened MIDI fd = " << fd << endl;
  cout << "\tformat  = " << mf.GetFormat() << endl;
  cout << "\t#tracks = " << mf.GetNumTracks() << endl;
  cout << "\tdivision= " << mf.GetDivision() << endl;

  for (track=0; track<mf.GetNumTracks(); track++){
    cout << "track " << track << ":\n";
    for (event=mf.NextEvent(track); event!=NULL; event=mf.NextEvent(track)){
      Tclm_PrintEvent(cout, event,true);
      cout << endl;
    }
  }
}
