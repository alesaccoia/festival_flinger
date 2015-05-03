/****************************<--OGI-->*************************************/
/*                                                                        */
/*             Center for Spoken Language Understanding                   */
/*        Oregon Graduate Institute of Science & Technology               */
/*                         Portland, OR USA                               */
/*                        Copyright (c) 2000                              */
/*                                                                        */
/*      This module is not part of the CSTR/University of Edinburgh       */
/*               release of the Festival TTS system.                      */
/*                                                                        */
/*  In addition to any conditions disclaimers below, please see the file  */
/*  "license_ogi_tts.txt" distributed with this software for information  */
/*  on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  */
/*                                                                        */
/****************************<--OGI-->*************************************/
#include <iostream>
#include <fstream>
#include "EST.h"
#include "OGI_Track.h"
#include "OGI_file.h"
#include "OGI_Macro.h"


static unsigned int OGI_TRACK_MAGIC=0xf149473e; // magic number for file read
template <class XXX_Track> static void read_track_data(XXX_Track &tr, FILE *fp, bool swap);
template <class XXX_Track> static void write_track_data(XXX_Track &tr, FILE *fp);
static bool check_track_swap(FILE *gfd);


static bool check_track_swap(FILE *gfd){
  unsigned int magic;
 
  fread(&magic,sizeof(unsigned int), 1, gfd);
  if (magic == SWAPINT(OGI_TRACK_MAGIC))
    return TRUE;
  else if (magic == OGI_TRACK_MAGIC)
    return FALSE;
  else {
    EST_error("OGI_file.cc:   not a valid OGI_Track file \n");
  }
  return FALSE;
}


template <class XXX_Track>
int OGIfread_track(XXX_Track &tr, FILE *fp){
  EST_TokenStream ts;
  EST_Option op;
  char chnum[20];
  int fps=ftell(fp); 
  ts.open(fp,0);
  ts.seek(ftell(fp));
  int i;

  // header
  if (ts.get().string() != "OGI_Track")
    EST_error(" OGIfread_Track:  Couldn't read OGI_Track format \n");

  // header params
  OGIfread(op, fp);

  // features
  OGIfread(tr.f, fp);

  // check if byte swapped
  bool swap = check_track_swap(fp);

  // resize
  tr.resize(op.ival("Nfr"), op.ival("Nch"));

  // set channel names
  for (i = 0; i < tr.num_channels(); ++i){
    sprintf(chnum, "ch_%d", i);
    EST_String ChNum = chnum;
    tr.set_channel_name( op.val(ChNum), i);
  }

  // end header
  if (ts.get().string() != "OGI_Track_End")
    EST_error(" OGIfread_Track:  Couldn't read OGI_Track end of header \n");
  
  if (op.val("Compressed") == "true"){
    cout << "compressed track mode not done\n";
    read_track_data(tr, fp, swap);
  }
  else {
    read_track_data(tr, fp, swap);
  }

  return ftell(fp)-fps;
}

template <class XXX_Track>
int OGIfwrite_track(XXX_Track &tr, FILE *fp, bool compress){
  int i;
  char chnum[100];
  int fps = ftell(fp);

  // header to check 
  fprintf(fp, "OGI_Track ");

  // create an EST_Option list for Nchannel, break info
  //  save it with OGIfwrite_KVL()
  EST_Option op;
  op.override_ival("Nch", tr.num_channels());
  op.override_ival("Nfr", tr.num_frames());
  if (compress)
    op.override_val("Compressed", "true");
  else
    op.override_val("Compressed", "false");
  for (i = 0; i < tr.num_channels(); ++i){
    sprintf(chnum, "ch_%d", i);
    EST_String ChNum = chnum;
    op.override_val(ChNum, tr.channel_name(i));
  }
  OGIfwrite(op, fp);   // save

  // save features
  OGIfwrite(tr.f, fp);

  // save to check if byte swapped
  fwrite(&OGI_TRACK_MAGIC, sizeof(unsigned int), 1, fp);

  // end header
  fprintf(fp, "OGI_Track_End ");
  
  if (compress){
  //      compress mode:   
  //      - save differential pmarks
  //      - look for names we recognize (LSF, E) then just save
  //         others as floats

    cout << "compressed track mode not done\n";
    write_track_data(tr, fp);
  }
  else {
    write_track_data(tr, fp);
  }

  return  ftell(fp)-fps;

}


template <class XXX_Track>
static void write_track_data(XXX_Track &tr, FILE *fp){
  
  int i,j;

  float* ff = new float [tr.num_frames()];
  if (ff == NULL) cerr << "write_track_data - insufficient memory" << endl,exit(-1);

  
  for (i = 0; i < tr.num_frames(); ++i)
    ff[i] = tr.t(i);
  fwrite(ff, sizeof(float), tr.num_frames(), fp);

  OGIbool* bb = new OGIbool [tr.num_frames()];
  if (bb == NULL) cerr << "write_track_data - insufficient memory" << endl,exit(-1);
  
  for (i = 0; i < tr.num_frames(); ++i)
    bb[i] = tr.val(i);
  fwrite(bb, sizeof(OGIbool), tr.num_frames(), fp);
  
  for (j = 0; j < tr.num_channels(); ++j){
    for (i = 0; i < tr.num_frames(); ++i)
      ff[i] = tr.a(i, j);
    fwrite(ff, sizeof(float), tr.num_frames(), fp);
  }
}

template <class XXX_Track>
static void read_track_data(XXX_Track &tr, FILE *fp, bool swap){
  
  int i,j;

  float* ff = new float [tr.num_frames()];
  if (ff == NULL) cerr << "read_track_data - insufficient memory" << endl,exit(-1);
  OGIbool* bb = new OGIbool [tr.num_frames()];
  if (bb == NULL) cerr << "read_track_data - insufficient memory" << endl,exit(-1);

  OGIfread(ff, sizeof(float), tr.num_frames(), fp, swap);  
  for (i = 0; i < tr.num_frames(); ++i)
    tr.t(i) = ff[i];
  
  OGIfread(bb, sizeof(OGIbool), tr.num_frames(), fp, swap);
  for (i = 0; i < tr.num_frames(); ++i){
    if (bb[i])
      tr.set_value(i);
    else 
      tr.set_break(i);
  }
  
  for (j = 0; j < tr.num_channels(); ++j){
    OGIfread(ff, sizeof(float), tr.num_frames(), fp, swap);
    for (i = 0; i < tr.num_frames(); ++i)
      tr.a(i, j) = ff[i];
  }
}

