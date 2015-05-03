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

#include "OGI_Filter.h"
#include "OGI_Macro.h"


void filtlist_to_vector(EST_String lstring, EST_DVector &b, EST_DVector &a){
  LISP lfilt = read_from_lstring(strintern(lstring));
  filtlist_to_vector(lfilt, b, a);
}

void filtlist_to_vector(LISP lfilt, EST_DVector &b, EST_DVector &a){

  LISP bl = car(lfilt);
  LISP al = car(cdr(lfilt));

  a.resize(max(siod_llength(bl), siod_llength(al)));
  b.resize(max(siod_llength(bl), siod_llength(al)));

  a.fill(0.0);
  b.fill(0.0);

  int n=0;
  for (; bl != 0; bl=cdr(bl)){
    b[n] = (double)FLONM(car(bl));
    n++;
  }

  n=0;
  for (; al != 0; al=cdr(al)){
    a[n] = (double)FLONM(car(al));
    n++;
  }

  //  cout << "a=" << a;
  //  cout << "b=" << b;
}

void OGIfiltfilt(EST_DVector &b, EST_DVector &a, EST_Wave &sig){
  OGIfilter(b, a, sig);
  reverse(sig);
  OGIfilter(b, a, sig);
  reverse(sig);
}

void OGIfiltfilt(EST_DVector &b, EST_DVector &a, EST_DVector &sig){
  OGIfilter(b, a, sig);
  reverse(sig);
  OGIfilter(b, a, sig);
  reverse(sig);
}

void reverse(EST_DVector &sig)
{
  int i, n;
  double t;
  n = (int)floor((float)(sig.length())/2.0);

  for (i = 0; i < n; ++i)
  {
    t = sig.a_no_check(i);
    sig.a_no_check(i) = sig.a_no_check(sig.length() - 1 -i);
    sig.a_no_check(sig.length() - 1 -i) = t;
  }
}

void OGIfilter(EST_DVector &b_in, EST_DVector &a_in, EST_Wave &sig){
  int Nsig = sig.num_samples();
  int Ntaps = max(a_in.length(),b_in.length());
  EST_DVector w(Ntaps); w.fill(0.0);
  EST_DVector ans(sig.length());
  double in;
  int n;

  EST_DVector av = a_in;
  EST_DVector bv = b_in;
  double x=0.0;
  av.def_val = &x;
  av.resize(Ntaps);
  bv.def_val = &x;
  bv.resize(Ntaps);

  if (Ntaps == 0)
    return;
  if (Ntaps == 1){
    for (n=0; n<Nsig; n++){
      sig.a_no_check(n) = (short) clip(sig.a_no_check(n) * bv(0)/av(0));
    }
    return;
  }

  for (n=0; n<Nsig; n++){
    in = sig.a_no_check(n);
    w[0] = w[1] + bv(0)*in;
    int m=1;
    for ( ; m<Ntaps-1; m++)
      w[m] = w(m+1) + bv(m)*in - av(m)*w[0];

    w[m] = bv(m)*in - av(m)*w[0];

    sig.a_no_check(n) = (short) clip(w[0]); // this makes it slower
  }
}

void OGIfilter(EST_DVector &b_in, EST_DVector &a_in, EST_DVector &sig){
  int Nsig = sig.length();
  int Ntaps = max(a_in.length(),b_in.length());
  EST_DVector w(Ntaps); w.fill(0.0);
  EST_DVector ans(sig.length());
  double in;
  int n;

  EST_DVector av = a_in;
  EST_DVector bv = b_in;
  double x=0.0;
  av.def_val = &x;
  av.resize(Ntaps);
  bv.def_val = &x;
  bv.resize(Ntaps);

  if (Ntaps == 0)
    return;
  if (Ntaps == 1){
    for (n=0; n<Nsig; n++){
      sig.a_no_check(n) = sig.a_no_check(n) * bv.a_no_check(0)/av.a_no_check(0);
    }
    return;
  }

  for (n=0; n<Nsig; n++){
    in = sig.a_no_check(n);
    w.a_no_check(0) = w.a_no_check(1) + bv.a_no_check(0)*in;
    int m=1;
    for ( ; m<Ntaps-1; m++)
      w.a_no_check(m) = w.a_no_check(m+1) + bv.a_no_check(m)*in - av.a_no_check(m)*w.a_no_check(0);

    w.a_no_check(m) = bv.a_no_check(m)*in - av.a_no_check(m)*w.a_no_check(0);

    sig.a_no_check(n) = w.a_no_check(0);
  }
}

void OGIzerophase_FIRfilter(EST_DVector &b, EST_Wave &sig){

  // filter length should be odd (not enforced though)
  int Nsig = sig.num_samples();
  int Ntaps = b.length();
  int N2=Ntaps/2;
  EST_DVector w(Ntaps); w.fill(0.0);
  double in;
  int n,k;

  if (Ntaps == 0)
    return;
  if (Ntaps == 1){
    for (n=0; n<Nsig; n++){
      sig.a_no_check(n) = (short) clip(sig.a_no_check(n) * b(0));
    }
    return;
  }

  for (n=0; n<N2; n++){
    in = sig.a_no_check(n);
    int m=1;
    for ( ; m<Ntaps-1; m++)
      w[m] =  w(m+1) + b(m)*in;
    w[m] = b(m)*in;
  }
  for ( k=0; n<Nsig; n++,k++){
    in = sig.a_no_check(n);
    sig.a_no_check(k) = (short) clip((in*b(0)+w(1)));
    int m=1;
    for ( ; m<Ntaps-1; m++)
      w[m] =  w(m+1) + b(m)*in;
    w[m] = b(m)*in;
  }
  for ( ; k<Nsig; k++){
    sig.a_no_check(k) = (short) clip(w(1));
    int m=1;
    for ( ; m<Ntaps-1; m++)
      w[m] =  w(m+1);
    w[m] = 0.0;
  }
}

void OGIzerophase_FIRfilter(EST_DVector &b, EST_DVector &sig){

  // filter length should be odd (not enforced though)
  int Nsig = sig.length();
  int Ntaps = b.length();
  int N2=Ntaps/2;
  EST_DVector w(Ntaps); w.fill(0.0);
  double in;
  int n,k;

  if (Ntaps == 0)
    return;
  if (Ntaps == 1){
    for (n=0; n<Nsig; n++){
      sig.a_no_check(n) = sig.a_no_check(n) * b(0);
    }
    return;
  }

  for (n=0; n<N2; n++){
    in = sig.a_no_check(n);
    int m=1;
    for ( ; m<Ntaps-1; m++)
      w[m] =  w(m+1) + b(m)*in;
    w[m] = b(m)*in;
  }
  for ( k=0; n<Nsig; n++,k++){
    in = sig.a_no_check(n);
    sig.a_no_check(k) = (in*b(0)+w(1));
    int m=1;
    for ( ; m<Ntaps-1; m++)
      w[m] =  w(m+1) + b(m)*in;
    w[m] = b(m)*in;
  }
  for ( ; k<Nsig; k++){
    sig.a_no_check(k) = w(1);
    int m=1;
    for ( ; m<Ntaps-1; m++)
      w[m] =  w(m+1);
    w[m] = 0.0;
  }
}



void OGIzerophase_FIRfilter(EST_DVector &b, EST_Track &sig){
  // filter length should be odd (not enforced though)
  int Nsig = sig.num_frames();
  int Ntaps = b.length();
  int N2=Ntaps/2;
  EST_DVector w(Ntaps); w.fill(0.0);
  double in;
  int n,k;

  if (Ntaps == 0)
    return;
  if (Ntaps == 1){
    for (n=0; n<Nsig; n++){
      sig.a(n,0) = sig.a(n,0) * (float)b(0);
    }
    return;
  }

  for (n=0; n<N2; n++){
    in = sig.a(n,0);
    int m=1;
    for ( ; m<Ntaps-1; m++)
      w[m] =  w(m+1) + b(m)*in;
    w[m] = b(m)*in;
  }
  for ( k=0; n<Nsig; n++,k++){
    in = sig.a(n,0);
    sig.a_no_check(k) = (float)(in*b(0)+w(1));
    int m=1;
    for ( ; m<Ntaps-1; m++)
      w[m] =  w(m+1) + b(m)*in;
    w[m] = b(m)*in;
  }
  for ( ; k<Nsig; k++){
    sig.a_no_check(k) = (float)w(1);
    int m=1;
    for ( ; m<Ntaps-1; m++)
      w[m] =  w(m+1);
    w[m] = 0.0;
  }

}



void design_notch_filter(
			 double angle,
			 double bw,
			 EST_DVector &B, // num
			 EST_DVector &A  // den
			 ){
  B.resize(3);
  A.resize(3);

  double r = bw;
  r = 0.9;  /////// NOT FINISHED
  cout << "design_notch_filter not finished\n";

  B[0] = 1.0;
  B[1] = -2*cos(angle);
  B[2] = 1;

  A[0] = 1.0;
  A[1] = -2*r*cos(angle);
  A[2] = r*r;

}



//
//  All-pole convolution
//
#define ar_dotprod(X,A,N,Yout) {        \
  ysum=*X; 				\
  a=A; 					\
  yn=Yout; 				\
  a++; 					\
  yn--; 				\
  for (k=1; k<N; k++) 			\
	 ysum-=(*yn--)*(*a++);          \
  *Yout=ysum;                           \
}

void ar_convolve(double *x, double *lpc, int Nlpc, double *out, int Nout){
  // Nlpc is length of lpc, NOT lpc _order_  (len = ord+1)
  //  skips multiply by 1st coeff, assumes it's 1.0
  int k;
  int n;
  double *a,*yn;
  double ysum;

  ///y
  for (n=1; n<=Nlpc; n++){
    ar_dotprod(x, lpc, n, out);
    out++;
    x++;
  }
  for (n=Nlpc; n<Nout; n++){
    ar_dotprod(x, lpc, Nlpc, out);
    out++;
    x++;
  }
}



//
//  FIR convolution
//
#define fir_dotprod(X,A,N,Y) {  \
  a=A;                          \
  xn=X;                         \
  xsum=0.0;                     \
  for (k=0; k<N;k++)            \
    xsum+=(*xn--)*(*a++);       \
  *Y=xsum;                      \
}

void fir_convolve(double *x, double *lpc, int Nlpc, double *out, int Nout){
  // Nlpc is length of lpc, NOT lpc _order_  (len = ord+1)
  int k;
  int n;
  double *a,*xn;
  double xsum;

  for (n=1; n<=Nlpc; n++){
    fir_dotprod(x, lpc, n, out);
    out++;
    x++;
  }

  for (n=Nlpc; n<Nout; n++){
    fir_dotprod(x, lpc, Nlpc, out);
    out++;
    x++;
  }
}

// normalize by 1/(1+a) so that no clipping can be introduced
void OGIpreemphasis(EST_Wave &sig, float a)
{
    float x = 0.0;
    float x_1 = 0.0;
    float sc = 1/(1+a);
    float a2 = a*sc;

    for (int j = 0; j < sig.num_channels(); ++j)
      for (int i = 0; i < sig.num_samples(); i++)
      {
        x = sig.a_no_check(i, j);
        sig.a_no_check(i, j) = (short int)(sc*sig.a_no_check(i, j) - (a2 * x_1));
        x_1 = x;
      }
}
