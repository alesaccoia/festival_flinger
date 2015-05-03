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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

char *db_alloc_print(int size, int lineno, char *file){
  static int Firsttime=1;
  char *p=NULL;
  FILE *fp;
   
  if (Firsttime){
    fp = fopen("malloc.dat","w");
    Firsttime = 0;
  }
  else
    fp = fopen("malloc.dat","a");

  fprintf(fp,"%p\t%d\t%s\t%d ",(p),lineno,file,size); 
  
  p = (char *) malloc(size);

  if (p == NULL)
    {
      fprintf(stderr,"WALLOC: failed to malloc %d bytes\n",size);
      exit(-1);  
    }
  fprintf(fp," ok\n");

  fclose(fp); 
  return p;
}
char *db_calloc_print(int Nelem, int sizeelem, int lineno, char *file){
  char *p;
  FILE *fp;
  
  p = (char *) calloc(Nelem, sizeelem);

  if (p == NULL)
    {
      fprintf(stderr,"WALLOC: failed to malloc %d bytes\n",Nelem*sizeelem);
      exit(-1); 
    }
  
  fp = fopen("malloc.dat","a");

  fprintf(fp,"%p\t%d\t%s\t%d\n",(p),lineno,file,sizeelem*Nelem); 
  fclose(fp);
  
  return p;
}

char *db_realloc_print(char *p, int size, int lineno, char *file){
  FILE *fp;
  char *oldp;

  oldp = p;

  p = (char *) realloc(p,size);

  if (p == NULL)
    {
      fprintf(stderr,"WALLOC: failed to realloc %d bytes\n",size);
      exit(-1);  /* I'd rather not do this but this is the only safe */
      /* thing to do */
    }
  
  if (oldp != p){
    fp = fopen("free.dat","a");
    fprintf(fp,"%p\t%d\t%s\n",oldp,lineno,file); 
    fclose(fp);
   
    fp = fopen("malloc.dat","a");
    fprintf(fp,"%p\t%d\t%s\t%d\n",p,lineno,file,size); 
    fclose(fp);
  }
  return p;
}


void db_free_print(int lineno, char *file, char *p){
  static int Firsttime=1;
  FILE *fp;

  if (Firsttime){
    fp = fopen("free.dat","w");
    Firsttime = 0;
  }
  else
    fp = fopen("free.dat","a");

  fprintf(fp,"%p\t%d\t%s\n",(p),lineno,file); 
  fclose(fp);
   
  if (p != NULL)
    free(p);
}
