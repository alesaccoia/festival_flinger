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
#ifndef __RESLPC_MOD_H__
#define __RESLPC_MOD_H__

#include "EST.h"
#include "resLPC.h"
#include "OGI_Wave.h"

void resLPC_mod( EST_Utterance *u, OGIresLPC_SRC *src, OGIresLPC_MOD *mod);
void resLPC_NOmod( EST_Utterance *u, OGIresLPC_SRC *src, OGIresLPC_MOD *mod);

void rm_unvoiced_pmarks(OGI_Track *pm);

#endif
