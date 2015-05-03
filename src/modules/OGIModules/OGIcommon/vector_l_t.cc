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
/* Instantiate long basic vector classes.                                 */
/*                                                                        */
/**************************************************************************/

#include "EST_TVector.h"
#include "EST_TSimpleVector.h"

Declare_TVector_Base(long, 0, -123456)
Declare_TSimpleVector(long)

#include "../../../../../speech_tools/base_class/EST_TSimpleVector.cc"
#include "../../../../../speech_tools/base_class/EST_TVector.cc"

Instantiate_TVector(long)
Instantiate_TSimpleVector(long)

