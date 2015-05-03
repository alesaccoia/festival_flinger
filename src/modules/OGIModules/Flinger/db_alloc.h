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

#ifndef __DB_ALLOC_H__
#define __DB_ALLOC_H__

#include "EST_walloc.h"

#define DEBUG_ALLOC 0

#if DEBUG_ALLOC

#define dballoc(TYPE,SIZE) ((TYPE *) db_alloc_print(sizeof(TYPE)*(SIZE),__LINE__,__FILE__))
#define dbcalloc(TYPE,SIZE) ((TYPE *) db_calloc_print((SIZE), sizeof(TYPE),__LINE__,__FILE__))
#define dbrealloc(P,SIZE) (db_realloc_print((char *)(P),(SIZE),__LINE__,__FILE__))
#define dbfree(P) db_free_print(__LINE__,__FILE__,(char *)(P))

char *db_alloc_print(int sz, int lineno, char *file);
char *db_calloc_print(int Nelem, int szelem, int lineno, char *file);
char *db_realloc_print(char *p, int sz, int lineno, char *file);
void db_free_print(int lineno, char *file, char *p);

#else

#define dballoc(TYPE,SIZE) walloc(TYPE,(SIZE))
#define dbcalloc(TYPE,SIZE) wcalloc(TYPE,(SIZE))
#define dbrealloc(P,SIZE) realloc((P),(SIZE))
#define dbfree(P) wfree(P)

#endif

#ifndef group_fread
#define group_fread(A,B,C,D) if (fread(A,B,C,D) != (unsigned int) (C)) {fprintf(stderr,"OGIresLPC:%s:line %d:  group file read failed\n",__FILE__,__LINE__); festival_error();}
#endif

#define OGI_check_realloc(A,B) {if ((A) == NULL){ cout << "\n resLPC: failed to realloc " << (B) << " bytes \n"; exit(-1);}}


#endif
