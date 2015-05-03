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

#include "OGI_file.h"


static int OGI_REASONABLE_LENGTH=10000;



#define fread_check(A,B,C,D) if (fread((A),(B),(C),(D)) != (unsigned int) (C)) { fprintf(stderr,"OGI_file:%s:line %d: group file read failed to read %d items of size %d bytes \n",__FILE__,__LINE__,(C),(B)); EST_error(" OGI_file.cc: file read failed\n "); }


const EST_String DEF_WAVFILE_TYPE = "snd"; // this one has the smallest header

void save_OGI_label(
		    const EST_FVector &y,
		    const EST_StrList &names,
		    const EST_String &filename,
		    int N){

  FILE *fp;
  float mpf = 1.0;  // hard-wired now
  float B;
  EST_String lab;
  int Nname;

  int k;
  if (N==OGI_ALL){
    N = y.length();
  }
  N = min(N, y.length());
  Nname = names.length();

  fp = fopen(filename,"w");

  // header
  fprintf(fp,"MillisecondsPerFrame: %1.6f\n",mpf);
  fprintf(fp,"END OF HEADER\n");

  B = 0.0;
  for (k=0; k<N; k++){
    if (k < Nname)
      lab = names.nth(k);
    else
      lab = "NULL";

    fprintf(fp,"%.1f %.1f %s\n", B*1000/mpf, y(k)*1000/mpf, lab.str());

    B = y(k);
  }
  fclose(fp);
}

//  StrVect - saved as
//     "StrVect" Nitems str1 str2 str3 ...
//
//  **Don't use:
//      (a)  the ENDKEY alone as a String
//      (b)  whitespace inside a String
//
//  Yes, I know this is not as robust as it could be...

static EST_String ENDKEY = "|%";
size_t OGIfread(EST_StrVector &strvect, FILE *fp){

  EST_TokenStream ts;
  EST_String s_in;
  int i=0;
  int Nstr;
  size_t fps = ftell(fp);

  // open ts and set to postition of file pointer
  ts.open(fp,0); // 0 = "don't close file when done"
  ts.seek(ftell(fp));

  // check that this is really a StrVector
  if (ts.get().string() != "StrV")
    EST_error(" OGIfread_StrVector: failed reading StrVector\n");

  // expected length
  Nstr = atoi(ts.get().string());

  if (Nstr > OGI_REASONABLE_LENGTH)
    cerr << " OGIfread_StrVector: asked to read " << Nstr << " items!\n";

  // resize
  strvect.resize(Nstr);

  s_in = ts.get().string();
  while (s_in != ENDKEY){
    strvect.a(i) = s_in;
    i++;
    if (i > Nstr)
      EST_error(" OGIfread_StrVector: failed reading StrVector - too many tokens found\n");
    s_in = ts.get().string();
  }

  if (i < Nstr)
    EST_error(" OGIfread_StrVector: failed reading StrVector - too few tokens found\n");

  // set file pointer to end of StrVector
  fseek(fp, ts.tell(), SEEK_SET);

  return ftell(fp)-fps;
}
size_t OGIfwrite(EST_StrVector &strvect, FILE *fp){

  int l;
  size_t fps = ftell(fp);

  fprintf(fp, "StrV %d ", strvect.length());
  for (l=0; l<strvect.length(); l++){
    fprintf(fp, "%s ", strvect.a(l).str());
  }
  fprintf(fp, "%s ", ENDKEY.str());

  return ftell(fp)-fps;
}

//  KVL - saved as
//     KVL Nitems k1 v1 k2 v2 ... kN vN |||
//
//  **Don't use:
//      (a)  ENDKEY alone as a key or value
//      (b)  whitespace inside a key or value
size_t OGIfread(EST_Option &kvl, FILE *fp){
  EST_TokenStream ts;
  EST_String k,v;
  int i=0;
  size_t fps = ftell(fp);

  // open ts and set to postition of file pointer
  ts.open(fp,0); // 0 = "don't close file when done"
  ts.seek(ftell(fp));

  // check that this is really a KVL
  EST_String tempString = ts.get().string();
  if (tempString != "KVL"){
    cerr << " OGIfread_KVL: expected KVL got: " << tempString << endl;
    EST_error(" OGIfread_KVL: failed reading KVL\n");
  }

  // expected length
  int Nkv = atoi(ts.get().string());
  if (Nkv > OGI_REASONABLE_LENGTH)
    cerr << " OGIfread_KVL: asked to read " << Nkv << " items!\n";

  while ((k = ts.get().string()) != ENDKEY){
    v = ts.get().string();
    kvl.override_val(k,v);
    i++;
    if (i > Nkv)
      EST_error(" OGIfread_KVL: failed reading KVL - too many tokens found\n");
  }

  if (i < Nkv)
    EST_error(" OGIfread_KVL: failed reading KVL - too few tokens found\n");

  // set file pointer to end of KVL
  fseek(fp, ts.tell(), SEEK_SET);

  return ftell(fp)-fps;
}
size_t OGIfwrite(EST_Option &kvl, FILE *fp){

  EST_Litem *l;
  size_t fps = ftell(fp);

  fprintf(fp, "KVL %d ", kvl.length());
  for (l=kvl.head(); l!=0; l=next(l)){
    fprintf(fp, "%s %s ", kvl.key(l).str(), kvl.val(l).str());
  }
  fprintf(fp, "%s ", ENDKEY.str());

  return ftell(fp)-fps;
}

//
// read/write for EST_Features
//
size_t  OGIfread(EST_Features &estft, FILE *fp){
  EST_TokenStream ts;
  EST_String k,v;
  int i=0;
  size_t fps = ftell(fp);

  // open ts and set to postition of file pointer
  ts.open(fp,0); // 0 = "don't close file when done"
  ts.seek(ftell(fp));

  // check that this is really a Features
  if (ts.get().string() != "Feat")
	EST_error(" OGIfread_Features: failed reading Features\n");

  // expected length
  int Nkv = atoi(ts.get().string());

  if (Nkv > OGI_REASONABLE_LENGTH)
	cerr << " OGIfread_Features: asked to read " << Nkv << " items!\n";

  while ((k = ts.get().string()) != ENDKEY){
    v = ts.get().string();
    estft.set(k,v);
    i++;
    if (i > Nkv) {
      fprintf(stderr,"%d > %d\n", i, Nkv);
      EST_error("OGIfread_Features: failed reading Features - too many tokens found\n");
    }
  }

  if (i < Nkv)
	EST_error(" OGIfread_Features: failed reading Features - too few tokens found\n");

  // set file pointer to end of Features
  fseek(fp, ts.tell(), SEEK_SET);

  return ftell(fp)-fps;
}

size_t OGIfwrite(EST_Features &estft, FILE *fp){

  EST_Features::Entries q;
  int N=0;
  size_t fps=ftell(fp);
  for (q.begin(estft); q ; ++q){
    N++;
  }

  fprintf(fp, "Feat %d ", N);
  for (q.begin(estft); q ; ++q){
    fprintf(fp, "%s %s ", (q->k).str(), (q->v).string().str());
  }
  fprintf(fp, "%s ", ENDKEY.str());

  return ftell(fp)-fps;
}


//
// read/write for EST_Featured objects
//
size_t  OGIfread_EST_Featured(EST_Track &tr, FILE *fp){
  EST_TokenStream ts;
  EST_String k,v;
  int i=0;
  size_t fps = ftell(fp);

  // open ts and set to postition of file pointer
  ts.open(fp,0); // 0 = "don't close file when done"
  ts.seek(ftell(fp));

  // check that this is really a Features
  if (ts.get().string() != "Feat")
	EST_error(" OGIfread_Features: failed reading Features\n");

  // expected length
  int Nkv = atoi(ts.get().string());

  if (Nkv > OGI_REASONABLE_LENGTH)
	cerr << " OGIfread_Featured: asked to read " << Nkv << " items!\n";

  while ((k = ts.get().string()) != ENDKEY){
	v = ts.get().string();
	tr.f_set(k,v);
	i++;
	if (i > Nkv)
	  EST_error(" OGIfread_Features: failed reading Features - too many tokens found\n");
  }

  if (i < Nkv)
	EST_error(" OGIfread_Features: failed reading Features - too few tokens found\n");

  // set file pointer to end of Features
  fseek(fp, ts.tell(), SEEK_SET);

  return ftell(fp)-fps;
}
size_t OGIfwrite_EST_Featured(EST_Track &tr, FILE *fp){

  EST_Track::FeatEntries q;
  int N=0;
  size_t fps=ftell(fp);
  for (q.begin(tr); q ; ++q){
    N++;
  }

  fprintf(fp, "Feat %d ", N);
  for (q.begin(tr); q ; ++q){
    fprintf(fp, "%s %s ", (q->k).str(), (q->v).string().str());
  }
  fprintf(fp, "%s ", ENDKEY.str());

  return ftell(fp)-fps;
}


/////////////////// wave ///////////////////////
size_t OGIfread(EST_Wave &wav, FILE *fp){
  EST_TokenStream ts;
  size_t fps=ftell(fp);

  ts.open(fp, 0);
  ts.seek(ftell(fp));

  if (wav.load(ts) != read_ok)
    EST_error(" OGIfread_Wave:  can't load wav\n");

  return ftell(fp)-fps;
}

size_t OGIfwrite(EST_Wave &wav, FILE *fp, bool compress){
  EST_write_status ret;
  size_t fps = ftell(fp);

  if (compress)
    ret = wav.save_file(fp,  DEF_WAVFILE_TYPE, "mulaw", EST_NATIVE_BO);
  else
    ret = wav.save_file(fp,  DEF_WAVFILE_TYPE, "short", EST_NATIVE_BO);

  if (ret != write_ok)
    EST_error(" OGIfwrite_Wave:  can't write wav\n");

  return ftell(fp)-fps; // return # of bytes written
}






// no swap needed
size_t OGIfread(char *ptr, size_t sz, size_t N, FILE *fp, bool swap=0){
  swap = 0;
  fread_check(ptr, sz, N, fp);
  return N;
}
size_t OGIfread(unsigned char *ptr, size_t sz, size_t N, FILE *fp, bool swap=0){
  swap = 0;
  fread_check(ptr, sz, N, fp);
  return N;
}
// don't need as long as OGIbool==char
//size_t OGIfread(OGIbool *ptr, size_t sz, size_t N, FILE *fp, bool swap=0){
//  swap = 0;
//  fread_check(ptr, sz, N, fp);
//  return N;
//}


//
// EST_TVector fread/fwrite do NOT save their size or swap info (you do this)
//

// CHANGE FUNCTIONS BELOW HERE TO BE FASTER -- use p_memory pointer to avoid copy


size_t OGIfread(EST_TVector<OGIbool> &bv, size_t N, FILE *fp, bool swap){

  OGIbool* tmp = new OGIbool [N];
  if (tmp == NULL) cerr << "OGIfread - insufficient memory" << endl,exit(-1);

  size_t k;
  bv.resize(N);

  OGIfread(tmp, sizeof(OGIbool), N, fp, swap);
  for (k=0; k<N; k++)
    bv[k] = tmp[k];
  return N;
}
size_t OGIfwrite(EST_TVector<OGIbool> &bv, FILE *fp){
  int N = bv.length();
  OGIbool* tmp = new OGIbool [N];
  if (tmp == NULL) cerr << "OGIfwrite - insufficient memory" << endl,exit(-1);

  int k;
  for (k=0; k<N; k++)
    tmp[k] = bv(k);
  fwrite(tmp, sizeof(OGIbool), N, fp);
  return N;
}


size_t OGIfread(EST_IVector &iv, size_t N, FILE *fp, bool swap){
  int* tmp = new int [N];
  if (tmp == NULL) cerr << "OGIfread - insufficient memory" << endl,exit(-1);

  size_t k;
  iv.resize(N);
  OGIfread(tmp, sizeof(int), N, fp, swap);
  for (k=0; k<N; k++)
    iv[k] = tmp[k];
  return N;
}

size_t OGIfwrite(EST_IVector &iv, FILE *fp){
  int N = iv.length();
  int* tmp = new int [N];
  if (tmp == NULL) cerr << "OGIfwrite - insufficient memory" << endl,exit(-1);

  int k;
  for (k=0; k<N; k++)
    tmp[k] = iv(k);
  fwrite(tmp, sizeof(int), N, fp);
  return N;
}

size_t OGIfread(EST_FVector &fv, size_t N, FILE *fp, bool swap){
  float* tmp = new float [N];
  if (tmp == NULL) cerr << "OGIfread - insufficient memory" << endl,exit(-1);

  size_t k;
  fv.resize(N);
  OGIfread(tmp, sizeof(float), N, fp, swap);
  for (k=0; k<N; k++)
    fv[k] = tmp[k];
  return N;
}

// read into subvector
size_t OGIfread(EST_FVector &fv, FILE *fp, bool swap, int offset, int num){
  float* tmp = new float [num];
  if (tmp == NULL) cerr << "OGIfread - insufficient memory" << endl,exit(-1);

  int k,l;
  OGIfread(tmp, sizeof(float), num, fp, swap);
  for (k=offset,l=0; k<offset+num; k++,l++)
    fv[k] = tmp[l];
  return num;
}

size_t OGIfwrite(EST_FVector &fv, FILE *fp){
  int N = fv.length();
  float* tmp = new float [N];
  if (tmp == NULL) cerr << "OGIfwrite - insufficient memory" << endl,exit(-1);

  int k;
  for (k=0; k<N; k++)
    tmp[k] = fv(k);
  fwrite(tmp, sizeof(float), N, fp);
  return N;
}

// write matrix
size_t OGIfwrite(EST_FMatrix &fv, FILE *fp){
  int M = fv.num_rows();
  int N = fv.num_columns();
  float* tmp = new float [N];
  if (tmp == NULL) cerr << "OGIfwrite - insufficient memory" << endl,exit(-1);

  int k;
  for (k=0; k<M; k++) {
    fv.copy_row(k,tmp,0,N);
    fwrite(tmp, sizeof(float), N, fp);
  }
  return M*N;
}

// read into submatrix
size_t OGIfread(EST_FMatrix &fv, FILE *fp, bool swap, int r, int numr, int c, int numc){
  int M = numr;
  int N = numc;
  if (N < 0)
    cerr << "OGIfread: cannot allocate " << N << " floats\n", exit(-1);
  float* tmp = new float [N];
  if (tmp == NULL) cerr << "OGIfread - insufficient memory" << endl,exit(-1);

  int k;
  for (k=0; k<M; k++) {
    OGIfread(tmp, sizeof(float), N, fp, swap);
    fv.set_row(k,tmp,c,N);
  }

  return M*N;
}


// swap needed
size_t OGIfread(int *ptr, size_t sz, size_t N, FILE *fp, bool swap){
  fread_check(ptr, sz, N, fp);
  if (swap)
    swap_bytes_int(ptr, N);
  return N;
}
size_t OGIfread(short *ptr, size_t sz, size_t N, FILE *fp, bool swap){
  fread_check(ptr, sz, N, fp);
  if (swap)
    swap_bytes_short(ptr, N);
  return N;
}
size_t OGIfread(unsigned short *ptr, size_t sz, size_t N, FILE *fp, bool swap){
  fread_check(ptr, sz, N, fp);
  if (swap)
    swap_bytes_ushort(ptr, N);
  return N;
}
size_t OGIfread(long *ptr, size_t sz, size_t N, FILE *fp, bool swap){
  fread_check(ptr, sz, N, fp);
  if (swap)
    swap_bytes_long(ptr, N);
  return N;
}
size_t OGIfread(float *ptr, size_t sz, size_t N, FILE *fp, bool swap){
  fread_check(ptr, sz, N, fp);
  if (swap)
    swap_bytes_float(ptr, N);
  return N;
}
size_t OGIfread(double *ptr, size_t sz, size_t N, FILE *fp, bool swap){
  fread_check(ptr, sz, N, fp);
  if (swap)
    swap_bytes_double(ptr, N);
  return N;
}


void swap_bytes_long(long *data, int length){
  /* Swap longs in an array */
  int i;
  int *d = (int *) data;
  length *= sizeof(long)/sizeof(int);
  for (i=0; i<length; i++){
    *d = SWAPINT(*d);
    d++;
  }
}


void swaplong(long *d){
  int *ii = (int *)d;
  ii[0] = SWAPINT(ii[0]);
  ii[1] = SWAPINT(ii[1]);
}


bool atobool(char *a){
  EST_String s(a);
  if ((downcase(s) == "t") || (downcase(s) == "y"))
    return TRUE;
  else
    return FALSE;
}
