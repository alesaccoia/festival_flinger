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

#include "OGI_Math.h"

fcomplex Complex(double a, double b)
{
  fcomplex x;
  x.r = a;
  x.i = b;
  return(x);
}

fcomplex Cadd(fcomplex a, fcomplex b)
{
  return(Complex(a.r+b.r,a.i+b.i));
}

fcomplex Csub(fcomplex a, fcomplex b)
{
  return(Complex(a.r-b.r,a.i-b.i));
}

fcomplex Cmul(fcomplex a, fcomplex b)
{
  return(Complex(a.r*b.r-a.i*b.i,a.r*b.i+a.i*b.r));
}

fcomplex Cdiv(fcomplex a, fcomplex b)
{
  double mag2b;

  mag2b = b.r*b.r + b.i*b.i;
  b.r /= mag2b;
  b.i /= -mag2b;           /* b is now 1/b */
  return(Cmul(a,b));
}

fcomplex RCmul(double a, fcomplex b)
{
  b.r *= a;
  b.i *= a;
  return(b);
}

double Cabs(fcomplex a)
{
  return(sqrt(a.r*a.r+a.i*a.i));
}

double Cangle(fcomplex a) /* interval [-PI,PI] */
{
  if ( a.r == 0. && a.i == 0. ) return(0.);
  else if ( a.r >= 0. ) return(atan(a.i/a.r));
  else if ( a.i >= 0. ) return(atan(a.i/a.r)+PI);
  else return(atan(a.i/a.r)-PI);
}

fcomplex Csqrt(fcomplex a)
{
  fcomplex x;
  double srmagx = sqrt(sqrt(a.r*a.r+a.i*a.i));

  if ( a.r > 0. ){
    x.r = srmagx * cos(atan(a.i/a.r)/2.);
    x.i = srmagx * sin(atan(a.i/a.r)/2.);
    return(x);
  }
  else if ( a.r < 0.){
    x.r = -srmagx * sin(atan(a.i/a.r)/2.);
    x.i = srmagx * cos(atan(a.i/a.r)/2.);
    return(x);
  }
  else {
    x.r = srmagx / sqrt(2.);
    if ( a.i >= 0. ) x.i = x.r;
    else x.i = -x.r;
    return(x);
  }
}

fcomplex Cp2r(double mag, double ang)
{
  return(Complex(mag*cos(ang),mag*sin(ang)));
}

int zroots_sorted_angles(
		   fcomplex a[],
		   int m,
		   fcomplex roots[],
		   double ang_out[]){
  int j,k,ret;
  ret = zroots(a, m, roots, 1);

  EST_DList r_ang;
  for (j=1; j<m+1; j++)
    r_ang.append(fabs(Cangle(roots[j])));

  sort(r_ang);
  for (j=0,k=1; j<m/2; j++,k+=2)
    ang_out[j] = r_ang.nth(k);

  return ret;
}


#define EPS 2.0e-7
#define EPSS 1.0e-8     /* Here EPSS is the estimated fractional roundoff  */
#define MR 13           /* error. We try to break (rare) limit cycles with */
#define MT 50           /* MR different fractional values, once every MT   */
#define MAXIT (MT*MR)   /* steps, for MAXIT total allowed iterations.      */
#define MAXM 100        /* A small number and max. anticipated value of "m"  */

// #define EPS 2.0e-6
// #define EPSS 1.0e-7     /* Here EPSS is the estimated fractional roundoff  */
// #define MR 8            /* error. We try to break (rare) limit cycles with */
// #define MT 10           /* MR different fractional values, once every MT   */
// #define MAXIT (MT*MR)   /* steps, for MAXIT total allowed iterations.      */
// #define MAXM 100      /* A small number and max. anticipated value of "m"  */

#define FMAX(a,b) ((a) > (b) ? (a) : (b))
#define ogi_warn(A) {printf("OGI_Math.cc warning: "); printf(A); }
//#define ogi_warn(A) { ; }

int zroots(
            fcomplex a[],
            int m,
            fcomplex roots[],
            int polish){

/*
  Given the degree "m" and the "m+1" complex coefficients "a[0...m]" of
  the polynomial "a(0) + a(1)*x + a(2)*x^2 + ... + a(m)*x^m" , this
  routine successively calls "laguer()" and finds all m complex roots in
  "roots[1...m]".  The boolean variable "polish" should be input as true
  (1) if polishing (also by Laguerre's method) is desired, false (0) if
  the roots will be subsequently polished by other means.

  (from "Numerical Recipes in C", p.374)
*/

  int its, j, jj, retval=1;
  fcomplex x, b, c, ad[MAXM];

  for(j=0; j<=m; j++) ad[j] = a[j];  /* Copy of coefficients for succesive */
                                     /* deflation                          */
  for(j=m; j>=1; j--){                  /* Loop over each root to be found */
    x = Complex(.0,.0);     /* Start at 0 to favor convergence to smallest */
    retval = retval && laguer(ad,j,&x,&its);   /* remaining root, and find the root           */

    if (fabs(x.i) <= 2.0*EPS*fabs(x.r)) x.i = 0.0;
    roots[j] = x;
    b = ad[j];                /* Forward deflation */
    for(jj=j-1; jj>=0; jj--){
      c = ad[jj];
      ad[jj] = b;
      b = Cadd(Cmul(x,b),c);
    }
  }
  roots[0] = Complex(.0000000001,.0);         /* Never used anyway --- bfn */
  if(polish && retval)
    for(j=1; j<=m; j++) {                     /* Polish the roots using the */
      x = roots[j];
      retval = laguer(a,m,&roots[j],&its);            /* undeflated coefficients.   */
      if (retval == 0)
        roots[j] = x;                         /* restore to prev value if polishing failed */
    }

  return retval;
}

int laguer(
            fcomplex a[],
            int m,
            fcomplex *x,
            int *its){

/*
  Given the degree "m" and the "m+1" complex coefficients "a[0...m]" of
  the polynomial "a(0) + a(1)*x + a(2)*x^2 + ... + a(m)*x^m" , and given a
  complex value "x", this routine improves "x" by Laguerre's method until
  it converges, within the achievable roundoff limit, to a root of the
  given polynomial. The number of iterations taken is returned as "its".

  (from "Numerical Recipes in C", pp.373-374)
*/

  int iter, j;
  double abx, abp, abm, err;
  fcomplex dx, x1, b, d, f, g, h, sq, gp, gm, g2;
  static double frac[MR+1] = {.0,.5,.25,.75,.13,.29,.71,.87,.43,.05,.38,.62,.88,1.0};
                                 /* Fractions used to break a limit cycle. */

  for(iter=1; iter<=MAXIT; iter++){  /* Loop over iterations up to allowed */
    *its = iter;                     /* maximum.                           */
    b = a[m];
    err = Cabs(b);
    d = f = Complex(.0,.0);
    abx = Cabs(*x);
    for(j=m-1; j>=0; j--){      /* Efficient computation of the polynomial */
      f = Cadd(Cmul(*x,f),d);   /* and its first two derivatives.          */
      d = Cadd(Cmul(*x,d),b);
      b = Cadd(Cmul(*x,b),a[j]);
      err = Cabs(b)+abx*err;
    }
    err *= EPSS;   /* Estimate of roundoff error in evaluating polynomial. */
    if(Cabs(b) <= err) return 1;                      /* We are on the root. */
    g = Cdiv(d,b);            /* The generic case: use Laguerre's formula. */
    g2 = Cmul(g,g);
    h = Csub(g2,RCmul(2.0,Cdiv(f,b)));
    sq = Csqrt(RCmul((double) (m-1),Csub(RCmul((double) m,h),g2)));
    gp = Cadd(g,sq);
    gm = Csub(g,sq);
    abp = Cabs(gp);
    abm = Cabs(gm);
    if(abp < abm) gp = gm;
    dx = ((FMAX(abp,abm)>.0 ? Cdiv(Complex((double)m,.0),gp)
         : RCmul(exp(log(1.+abx)),
                 Complex(cos((double)iter),sin((double)iter)))));
    x1 = Csub(*x,dx);
    if(x->r==x1.r && x->i==x1.i) return 1;                     /* Converged. */
    if(iter % MT) *x = x1;
    else *x = Csub(*x,RCmul(frac[iter/MT],dx));
    /* Every so often we take a fractional step, to break any limit cycle  */
    /* (itself a rare occurence).                                          */
  }
  ogi_warn("too many iterations in laguer \n");
  /* Very unusual -- can occur only for complex roots. Try a different     */
  /* starting guess for the root.                                          */
  return 0;
}

