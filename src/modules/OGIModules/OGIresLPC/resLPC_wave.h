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
#ifndef __RESLPC_WAVE_H__
#define __RESLPC_WAVE_H__
#include "OGI_Buffer.h"
#include "OGI_Wave.h"
#include "resLPC.h"


enum OGIwin_t {w_trapezoid, w_triangle};
 

void resLPC_wave_synth( OGIresLPC_SRC *src, OGIresLPC_MOD *mod, OGI_Wave *outwav);
void resLPC_LSFonly( OGIresLPC_SRC *src, OGIresLPC_MOD *mod);
void create_window(OGI_PitchSyncBuffer &window, OGIwin_t wintype);
void copy_old_pulse(OGI_PitchSyncBuffer &spbuf, OGI_PitchSyncBuffer &win, float G, OGI_PitchSyncBuffer &outbuf);
void add_new_pulse(OGI_PitchSyncBuffer &spbuf, OGI_PitchSyncBuffer &win, float G, OGI_PitchSyncBuffer &outbuf);
void extract_exc_pulse(float map, OGI_WaveChunk &exc, OGI_Track &srcpm, OGI_PitchSyncBuffer &excbuf, int T0_sL, int T0_sR, float Fs);
OGIwin_t get_window_type(const EST_String &sval);

#endif

