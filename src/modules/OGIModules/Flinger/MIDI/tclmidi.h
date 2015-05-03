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

#ifndef TCLMIDI_H
#define TCLMIDI_H

extern "C" {
#include "tcl.h"
}

#if defined(__WIN32__)
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   undef WIN32_LEAN_AND_MEAN

/*
 * VC++ has an alternate entry point called DllMain, so we need to rename
 * our entry point.
 */

#   if defined(_MSC_VER)
#	define EXPORT(a,b) __declspec(dllexport) a b
#	define DllEntryPoint DllMain
#   else
#	if defined(__BOLANDC__)
#	    define EXPORT(a,b) a _export b
#	else
#	    define EXPORT(a,b) a b
#	endif
#   endif
#else
#   define EXPORT(a,b) a b
#   undef Tcl_Ckfree
#   define Tcl_Ckfree free
#endif

#include "TclmIntp.h"

EXTERN EXPORT(int,Tclmidi_Init) _ANSI_ARGS_((Tcl_Interp *interp));
extern int Tclm_PlayInit(Tcl_Interp *interp, TclmInterp *tclm_interp);
extern int Tclm_PatchInit(Tcl_Interp *interp, TclmInterp *tclm_interp);
extern int Tcl_GetLong(Tcl_Interp *interp, const char *string, long *longPtr);
extern int Tclm_ParseDataByte(Tcl_Interp *interp, const char *str, int *val);
extern void Tclm_PrintData(ostream &buf, const unsigned char *data,
    long length);
#endif
