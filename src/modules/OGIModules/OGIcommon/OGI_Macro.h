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

#ifndef __OGI_MACRO_H__
#define __OGI_MACRO_H__


#ifndef PI
#define PI 3.141592653589793
#endif

#ifndef _UV_
#define _UV_ 0
#define _V_  1
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif



#ifndef OGI_MAXSHORT
#define OGI_MAXSHORT ((short) 32767)
#endif

#ifndef OGI_MINSHORT
#define OGI_MINSHORT ((short) -32767)
#endif

#ifndef OGI_MAXUSHORT
#define OGI_MAXUSHORT ((unsigned short) 65535)
#endif

#define clip(A)  ((short) (max(min((A), OGI_MAXSHORT), OGI_MINSHORT)))

#define check_ov_ushort(A)  if (A > OGI_MAXUSHORT){ cerr << __FILE__  << ": line " << __LINE__ << " overflow of ushort cast:" << A << endl; exit(-1);}
 


#endif
