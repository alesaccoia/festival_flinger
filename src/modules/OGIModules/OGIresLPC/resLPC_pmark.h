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
#include <stdlib.h>
#include <iostream>
//#include <fstream>
#include "EST.h"
#include "OGI_Track.h"
#include "OGI_Buffer.h"
#include "OGI_Macro.h"

void fill_UV_pmarks(OGI_Track &pm, int Nsig, float Fs, EST_Features &g, bool l);

void pmark_V_UV( OGI_Track &pm, EST_IList &Bvsect, EST_IList &Evsect,
int Nexc, float Fs, EST_Features &gl);

void insert_UVsection ( EST_IList &Bvsect, EST_IList &Evsect, int
UVsectB, int UVsectE, float incr, OGI_Track &pm, int Nexc, float Fs,
bool label_vuv );

void make_UVpmarks( OGI_Track &pm, EST_IList &Bvsect, EST_IList
&Evsect, int Nexc, float Fs, EST_Features &gl, bool label_vuv );

void make_Vsection (int pmB, int pmE, OGI_Track &pm, bool label);
