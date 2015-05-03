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

#ifdef SYSTEM_IS_WIN32
#define HAVE_UNISTD_H 0  
#else
#define HAVE_UNISTD_H 1  
#endif


#include <stdlib.h>
#if HAVE_UNISTD_H
# include <sys/types.h>
# include <unistd.h>
#else
# ifdef _MSC_VER
#  include <io.h>
# endif
#endif
#include "SMFUtils.h"


long
MRead(int fd, char *data, long len)
{
	int num_read;
	int total_read;

	total_read = 0;
	do {
		if ((num_read = read(fd, data, len - total_read)) == -1)
			return (-1);
		if (num_read == 0)
			break;
		total_read += num_read;
		data += num_read;
	} while (len > total_read);
	return (total_read);
}

long
MWrite(int fd, char *data, long len)
{
	int num_written;
	int total_written;

	total_written = 0;
	do {
		if ((num_written = write(fd, data, len - total_written)) == -1)
			return (-1);
		if (num_written == 0)
			break;
		total_written += num_written;
		data += num_written;
	} while (len > total_written);
	return (total_written);
}

/* MWM
long
MRead(Tcl_Channel channel, char *data, long len)
{
	int num_read;
	int total_read;

	total_read = 0;
	do {
		if ((num_read = Tcl_Read(channel, data, len - total_read))
		    == -1)
			return (-1);
		if (num_read == 0)
			break;
		total_read += num_read;
		data += num_read;
	} while (len > total_read);
	return (total_read);
}

long
MWrite(Tcl_Channel channel, char *data, long len)
{
	int num_written;
	int total_written;

	total_written = 0;
	do {
		if ((num_written = Tcl_Write(channel, data,
		    len - total_written)) == -1)
			return (-1);
		if (num_written == 0)
			break;
		total_written += num_written;
		data += num_written;
	} while (len > total_written);
	return (total_written);
}
MWM */

long
VarToFix(unsigned char *var, int *len)
{
	long fix;

	fix = 0;
	*len = 0;
	if (*var & 0x80)
		do {
			fix = (fix << 7) + (*var & 0x7f);
			(*len)++;
		} while (*var++ & 0x80);
	else {
		fix = *var++;
		(*len)++;
	}

	return (fix);
}


int
FixToVar(long fix, unsigned char *var)
{
	int i;
	unsigned char buf[4];
	unsigned char *bptr;

	buf[0] = buf[1] = buf[2] = buf[3] = 0;
	bptr = buf;
	*bptr++ = fix & 0x7f;
	while ((fix >>= 7) > 0) {
		*bptr |= 0x80;
		*bptr++ += (fix & 0x7f);
	}

	i = 0;
	do {
		*var++ = *--bptr;
		i++;
	} while (bptr != buf);

	return (i);
}

