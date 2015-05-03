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
#ifndef __RESLPC_VQUAL_H__
#define __RESLPC_VQUAL_H__
#include "EST.h"
#include "OGI_Wave.h"
#include "OGI_Track.h"
#include "OGI_Macro.h"
#include "OGI_Vector.h"
#include "OGI_Filter.h"
#include "OGI_LPC.h"
#include "OGI_Buffer.h"


bool setup_vqual_mods(EST_Features *gl);
void vqual_mod_wave(OGI_PitchSyncBuffer &spbuf, float F0, float v_uv);
void vqual_mod_lsf(OGI_VectDouble &lsfvect, float F0, float v_uv);

#endif

