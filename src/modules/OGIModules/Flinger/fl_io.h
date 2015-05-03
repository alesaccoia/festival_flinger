/****************************<--CSLU-->************************************/
/*                                                                        */
/*             Center for Spoken Language Understanding                   */
/*               Division of Biomedical Computer Science                  */
/*               Oregon Health and Science University                     */
/*                         Portland, OR USA                               */
/*                        Copyright (c) 1999 - 2009                       */
/*                                                                        */
/*      This module is not part of the CSTR/University of Edinburgh       */
/*               release of the Festival TTS system.                      */
/*                                                                        */
/*  In addition to any conditions disclaimers below, please see the file  */
/*  "license_ohsu_tts.txt" distributed with this software for information */
/*  on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  */
/*                                                                        */
/*      Authors: Mike Macon, Esther Klabbers, Alexander Kain              */
/*                          and David Brown                               */
/*                                                                        */
/****************************<--CSLU-->************************************/

#ifndef __FLINGER_IO_H__
#define __FLINGER_IO_H__

#include "EST.h"

void Flinger_init_params(LISP params);
void Flinger_addto_params(LISP params);
void create_Flinger_input(void);
EST_Features *get_Flinger_params(void);
bool Flinger_params_loaded(void);

// YK 2011-07-25 API to manage a debug file switch
void Flinger_set_debug_file_flag(const int val);
int  Flinger_get_debug_file_flag(void);
#endif
