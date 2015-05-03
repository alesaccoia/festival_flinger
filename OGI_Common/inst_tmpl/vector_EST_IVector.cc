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
/* Instantiate EST_IVector                                                */
/*                                                                        */
/**************************************************************************/

#include "EST_types.h"


//
// IVector
//

static EST_IVector const def_val_IVectVect = 0;
template <> const EST_IVector *EST_TVector<EST_IVector>::def_val = &def_val_IVectVect;
static EST_IVector error_return_IVectVect = 0;
template <> EST_IVector *EST_TVector<EST_IVector>::error_return = &error_return_IVectVect;
 


#if defined(INSTANTIATE_TEMPLATES)
#include "../../../speech_tools/base_class/EST_TVector.cc"
template class EST_TVector<EST_IVector>;
#endif


