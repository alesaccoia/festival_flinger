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
/*                                                                        */
/* Instantiate pointer basic vector classes.                              */
/*                                                                        */
/**************************************************************************/


#include "EST_TVector.h"

static void * const def_val_VoidVect = NULL;
template <> void * const *EST_TVector<void *>::def_val = &def_val_VoidVect;
static void *error_return_VoidVect = NULL;
template <> void * *EST_TVector<void *>::error_return = &error_return_VoidVect;

#include "../../../../../speech_tools/base_class/EST_TVector.cc"
template class EST_TVector<void *>;

