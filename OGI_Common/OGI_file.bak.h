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

#ifndef __OGI_FILE_H__
#define __OGI_FILE_H__

#include "EST.h"

#define OGI_ALL -99

// note: sizeof(bool)=4 on MS VC++ 5.0 because they define it as int
//       sizeof(bool)=1 in gcc (much more reasonable)
typedef char OGIbool;

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

void save_OGI_label(const EST_FVector &y, const EST_StrList &names, const EST_String &filename, int N=OGI_ALL);




// all return size of data read/written (in bytes)

size_t OGIfread(EST_StrVector &strvect, FILE *fp);
size_t OGIfwrite(EST_StrVector &strvect, FILE *fp); // returns size

size_t OGIfread(EST_Option &kvl, FILE *fp);
size_t OGIfwrite(EST_Option &kvl, FILE *fp); // returns size

size_t OGIfread(EST_Features &estft, FILE *fp);
size_t OGIfwrite(EST_Features &estft, FILE *fp); // returns size

size_t OGIfread_EST_Featured(EST_Track &tr, FILE *fp);
size_t OGIfwrite_EST_Featured(EST_Track &tr, FILE *fp);

size_t OGIfread(EST_Wave &wav, FILE *fp);
size_t OGIfwrite(EST_Wave &wav, FILE *fp, bool compress=0);

size_t OGIfread(EST_TVector<OGIbool> &bv, size_t N, FILE *fp, bool swap=0);
size_t OGIfwrite(EST_TVector<OGIbool> &bv, FILE *fp);

size_t OGIfread(EST_IVector &iv, size_t N, FILE *fp, bool swap);
size_t OGIfwrite(EST_IVector &iv, FILE *fp);

size_t OGIfread(EST_FVector &fv, size_t N, FILE *fp, bool swap);
size_t OGIfwrite(EST_FVector &fv, FILE *fp);
size_t OGIfread(EST_FVector &fv, FILE *fp, bool swap, int offset, int num); // JW: subvector

size_t OGIfread(EST_FMatrix &fv, FILE *fp, bool swap, int r, int numr, int c, int numc); // JW: (sub)matrix
size_t OGIfwrite(EST_FMatrix &fv, FILE *fp); // JW: matrix

size_t OGIfread(char *ptr, size_t sz, size_t N, FILE *fp, bool swap);
size_t OGIfread(unsigned char *ptr, size_t sz, size_t N, FILE *fp, bool swap);
//size_t OGIfread(OGIbool *ptr, size_t sz, size_t N, FILE *fp, bool swap);
size_t OGIfread(int *ptr, size_t sz, size_t N, FILE *fp, bool swap);
size_t OGIfread(short *ptr, size_t sz, size_t N, FILE *fp, bool swap);
size_t OGIfread(unsigned short *ptr, size_t sz, size_t N, FILE *fp, bool swap);
size_t OGIfread(long *ptr, size_t sz, size_t N, FILE *fp, bool swap);
size_t OGIfread(float *ptr, size_t sz, size_t N, FILE *fp, bool swap);
size_t OGIfread(double *ptr, size_t sz, size_t N, FILE *fp, bool swap);

void swap_bytes_long(long *data, int length);
void swaplong(long *d);
bool atobool(char *a);

#endif
