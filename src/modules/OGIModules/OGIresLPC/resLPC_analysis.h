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
#ifndef __RESLPC_ANALYSIS_H__
#define __RESLPC_ANALYSIS_H__
#include "EST.h"
#include "OGI_Track.h"

LISP OGIresLPC_LPC_Analysis(LISP params);
void compute_residual( EST_Wave &sp, OGI_Track &lpc, EST_Features &g, EST_Wave &res);
void compute_log_power( EST_Wave &sp, OGI_Track &lpc, EST_Features &g);
void compute_lpc( EST_Wave &sp, OGI_Track &lpc, EST_Features &g);


#endif

