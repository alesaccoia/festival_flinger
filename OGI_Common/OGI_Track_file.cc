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

static void read_track_data(EST_Track &tr, FILE *fp, bool swap);
static void write_track_data(EST_Track &tr, FILE *fp);
static void write_compressed_track_data(EST_Track &tr, FILE *fp);
static void read_compressed_track_data(EST_Track &tr, FILE *fp, bool swap);
static float min_channel_val(EST_Track &tr, int ch);
static float max_channel_val(EST_Track &tr, int ch);

//
// can get rid of these redundancies with templates...  
//                        but that gave me a lot of headaches
//
static void write_compressed_track_data(OGI_Track &tr, FILE *fp);
static void read_compressed_track_data(OGI_Track &tr, FILE *fp, bool swap);
static void read_track_data(OGI_Track &tr, FILE *fp, bool swap);
static void write_track_data(OGI_Track &tr, FILE *fp);
static float min_channel_val(OGI_Track &tr, int ch);
static float max_channel_val(OGI_Track &tr, int ch);

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


int OGIfread_track(EST_Track &tr, FILE *fp){
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
  OGIfread_EST_Featured(tr, fp);

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
    read_compressed_track_data(tr, fp, swap);
  }
  else {
    read_track_data(tr, fp, swap);
  }

  return ftell(fp)-fps;
}

int OGIfwrite_track(EST_Track &tr, FILE *fp, bool compress){
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
  OGIfwrite_EST_Featured(tr, fp); //@1.4

  // save to check if byte swapped
  fwrite(&OGI_TRACK_MAGIC, sizeof(unsigned int), 1, fp);

  // end header
  fprintf(fp, "OGI_Track_End ");
  
  if (compress){
    write_compressed_track_data(tr, fp);
  }
  else {
    write_track_data(tr, fp);
  }

  return  ftell(fp)-fps;

}


///////////// 
static void write_track_data(EST_Track &tr, FILE *fp){
  
  int i,j;

  float* ff = new float [tr.num_frames()];
  
  for (i = 0; i < tr.num_frames(); ++i)
    ff[i] = tr.t(i);
  fwrite(ff, sizeof(float), tr.num_frames(), fp);

  OGIbool* bb = new OGIbool [tr.num_frames()];
  
  for (i = 0; i < tr.num_frames(); ++i)
    bb[i] = tr.val(i);
  fwrite(bb, sizeof(OGIbool), tr.num_frames(), fp);
  
  for (j = 0; j < tr.num_channels(); ++j){
    for (i = 0; i < tr.num_frames(); ++i)
      ff[i] = tr.a_no_check(i, j);
    fwrite(ff, sizeof(float), tr.num_frames(), fp);
  }

  delete ff;
  delete bb;
}

static void read_track_data(EST_Track &tr, FILE *fp, bool swap){
  
  int i,j;

  float* ff = new float [tr.num_frames()];
  OGIbool* bb = new OGIbool [tr.num_frames()];

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
      tr.a_no_check(i, j) = ff[i];
  }

  delete ff;
  delete bb;
}

static void write_compressed_track_data(EST_Track &tr, FILE *fp){
  int i,j;
  float ftmp,sc,minCh,maxCh;
  
  // times
  float* ff = new float [tr.num_frames()];
  for (i = 0; i < tr.num_frames(); ++i)
    ff[i] = tr.t(i);
  fwrite(ff, sizeof(float), tr.num_frames(), fp);
  delete ff;

  // EST thing - v/uv info?
  OGIbool* bb = new OGIbool [tr.num_frames()];
  for (i = 0; i < tr.num_frames(); ++i)
    bb[i] = tr.val(i);
  fwrite(bb, sizeof(OGIbool), tr.num_frames(), fp);
  delete bb;

  // channel data
  unsigned short *us = new unsigned short [tr.num_frames()];
  for (j = 0; j < tr.num_channels(); ++j){

    // get min, max of this channel - save min,sc
    minCh = min_channel_val(tr, j);
    maxCh = max_channel_val(tr, j);
    if (maxCh-minCh < (float)10e-10)      maxCh = minCh+(float)10e-10;
    sc = ((float)OGI_MAXUSHORT-1)/(maxCh-minCh);
    fwrite(&minCh, sizeof(float), 1, fp);
    fwrite(&sc,    sizeof(float), 1, fp);

    if (sc < 1.0){
      cerr << "OGI_Track warning: scale factor of track channel " << j << " is " << sc;
      cerr << " -- compression results may be poor " << endl;
    }
    
    // scale by min,max and convert to ushort
    for (i = 0; i < tr.num_frames(); ++i){
      ftmp = (tr.a_no_check(i, j)-minCh)*sc;
      check_ov_ushort(ftmp);
      
      us[i] = (unsigned short) ftmp;
    }
    // save
    fwrite(us, sizeof(unsigned short), tr.num_frames(), fp);
  }
  delete us;
}

static void read_compressed_track_data(EST_Track &tr, FILE *fp, bool swap){
  int i,j;
  float minCh,sc;

  // times
  float* ff = new float [tr.num_frames()];
  OGIfread(ff, sizeof(float), tr.num_frames(), fp, swap);  
  for (i = 0; i < tr.num_frames(); ++i)
    tr.t(i) = ff[i];
  delete ff;

  // "breaks"
  OGIbool* bb = new OGIbool [tr.num_frames()];  
  OGIfread(bb, sizeof(OGIbool), tr.num_frames(), fp, swap);
  for (i = 0; i < tr.num_frames(); ++i){
    if (bb[i])
      tr.set_value(i);
    else 
      tr.set_break(i);
  }
  delete bb;

  // channel data
  unsigned short *us = new unsigned short [tr.num_frames()];
  for (j = 0; j < tr.num_channels(); ++j){
    OGIfread(&minCh, sizeof(float), 1, fp, swap);
    OGIfread(&sc,    sizeof(float), 1, fp, swap);
    OGIfread(us, sizeof(unsigned short), tr.num_frames(), fp, swap);  
    // scale by min,max
    for (i = 0; i < tr.num_frames(); ++i)
      tr.a_no_check(i, j) = ((float)us[i])/sc + minCh;
  }
  delete us;
}

////////////////////////////////////////////////
// OGI_Track versions -- this will not be needed when templates work properly
////////////////////////////////////////////////

static void write_track_data(OGI_Track &tr, FILE *fp){
  
  int i,j;

  float* ff = new float [tr.num_frames()];  
  for (i = 0; i < tr.num_frames(); ++i)
    ff[i] = tr.t(i);
  fwrite(ff, sizeof(float), tr.num_frames(), fp);

  OGIbool* bb = new OGIbool [tr.num_frames()];
  
  for (i = 0; i < tr.num_frames(); ++i)
    bb[i] = tr.val(i);
  fwrite(bb, sizeof(OGIbool), tr.num_frames(), fp);
  
  for (j = 0; j < tr.num_channels(); ++j){
    for (i = 0; i < tr.num_frames(); ++i)
      ff[i] = tr.a_no_check(i, j);
    fwrite(ff, sizeof(float), tr.num_frames(), fp);
  }

  delete ff;
  delete bb;
}

static void read_track_data(OGI_Track &tr, FILE *fp, bool swap){
  
  int i,j;

  float* ff = new float [tr.num_frames()];
  OGIbool* bb = new OGIbool [tr.num_frames()];

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
      tr.a_no_check(i, j) = ff[i];
  }

  delete ff;
  delete bb;
}


static void write_compressed_track_data(OGI_Track &tr, FILE *fp){
  int i,j;
  float ftmp,sc,minCh,maxCh;
  
  // times
  float* ff = new float [tr.num_frames()];
  for (i = 0; i < tr.num_frames(); ++i)
    ff[i] = tr.t(i);
  fwrite(ff, sizeof(float), tr.num_frames(), fp);
  delete ff;

  // EST thing - v/uv info?
  OGIbool* bb = new OGIbool [tr.num_frames()];
  for (i = 0; i < tr.num_frames(); ++i)
    bb[i] = tr.val(i);
  fwrite(bb, sizeof(OGIbool), tr.num_frames(), fp);
  delete bb;

  // channel data
  unsigned short *us = new unsigned short [tr.num_frames()];
  for (j = 0; j < tr.num_channels(); ++j){

    // get min, max of this channel - save min,sc
    minCh = min_channel_val(tr, j);
    maxCh = max_channel_val(tr, j);
    if (maxCh-minCh < (float)10e-10)      maxCh = minCh+(float)10e-10;
    sc = ((float)OGI_MAXUSHORT-1)/(maxCh-minCh);
    fwrite(&minCh, sizeof(float), 1, fp);
    fwrite(&sc,    sizeof(float), 1, fp);

    if (sc < 1.0){
      cerr << "OGI_Track warning: scale factor of track channel " << j << " is " << sc;
      cerr << " -- compression results may be poor " << endl;
    }
    
    // scale by min,max and convert to ushort
    for (i = 0; i < tr.num_frames(); ++i){
      ftmp = (tr.a_no_check(i, j)-minCh)*sc;
      check_ov_ushort(ftmp);
      
      us[i] = (unsigned short) ftmp;
    }
    // save
    fwrite(us, sizeof(unsigned short), tr.num_frames(), fp);
  }
  delete us;
}

static void read_compressed_track_data(OGI_Track &tr, FILE *fp, bool swap){
  int i,j;
  float minCh,sc;

  // times
  float* ff = new float [tr.num_frames()];
  OGIfread(ff, sizeof(float), tr.num_frames(), fp, swap);  
  for (i = 0; i < tr.num_frames(); ++i)
    tr.t(i) = ff[i];
  delete ff;

  // "breaks"
  OGIbool* bb = new OGIbool [tr.num_frames()];  
  OGIfread(bb, sizeof(OGIbool), tr.num_frames(), fp, swap);
  for (i = 0; i < tr.num_frames(); ++i){
    if (bb[i])
      tr.set_value(i);
    else 
      tr.set_break(i);
  }
  delete bb;

  // channel data
  unsigned short *us = new unsigned short [tr.num_frames()];
  for (j = 0; j < tr.num_channels(); ++j){
    OGIfread(&minCh, sizeof(float), 1, fp, swap);
    OGIfread(&sc,    sizeof(float), 1, fp, swap);
    OGIfread(us, sizeof(unsigned short), tr.num_frames(), fp, swap);  
    // scale by min,max
    for (i = 0; i < tr.num_frames(); ++i)
      tr.a_no_check(i, j) = ((float)us[i])/sc + minCh;
  }
  delete us;
}

int OGIfread_track(OGI_Track &tr, FILE *fp){
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
  OGIfread_EST_Featured(tr, fp);

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
    read_compressed_track_data(tr, fp, swap);
  }
  else {
    read_track_data(tr, fp, swap);
  }

  return ftell(fp)-fps;
}

int OGIfwrite_track(OGI_Track &tr, FILE *fp, bool compress){
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
  OGIfwrite_EST_Featured(tr, fp);

  // save to check if byte swapped
  fwrite(&OGI_TRACK_MAGIC, sizeof(unsigned int), 1, fp);

  // end header
  fprintf(fp, "OGI_Track_End ");
  
  if (compress){
    write_compressed_track_data(tr, fp);
  }
  else {
    write_track_data(tr, fp);
  }

  return  ftell(fp)-fps;

}

static float min_channel_val(EST_Track &tr, int ch){
  float minVal=(float)10e20;
  int k;
  for (k=0; k<tr.num_frames(); k++)
    minVal = min(tr.a_no_check(k,ch), minVal);
  return minVal;
}

static float max_channel_val(EST_Track &tr, int ch){
  float maxVal=(float)-10e20;
  int k;
  for (k=0; k<tr.num_frames(); k++)
    maxVal = max(tr.a_no_check(k,ch), maxVal);
  return maxVal;
}


static float min_channel_val(OGI_Track &tr, int ch){
  float minVal=(float)10e20;
  int k;
  for (k=0; k<tr.num_frames(); k++)
    minVal = min(tr.a_no_check(k,ch), minVal);
  return minVal;
}

static float max_channel_val(OGI_Track &tr, int ch){
  float maxVal=(float)-10e20;
  int k;
  for (k=0; k<tr.num_frames(); k++)
    maxVal = max(tr.a_no_check(k,ch), maxVal);
  return maxVal;
}
