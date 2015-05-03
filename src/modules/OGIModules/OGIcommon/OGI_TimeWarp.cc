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
#include "OGI_TimeWarp.h"

float OGI_TimeWarp::invmap(float y_in, float *x_indx) {
  // returns interpolated, inverse-mapped value v(*x_indx)
  // int part of *x_indx is index to left in x array
  // float part of *x_indx is weight to apply to indx to RIGHT in x array
  // round *x_indx to get nearest int x value
  //
  // **implicitly assumes time pair (x[-1],y[-1]) = (0,0)
  //
  // Lindx is cached to try to save search time
  // N is number of src/targ segments
  float v;   // invmapped return value 
  float gR;  // floating pt part of x_indx returned

  // degenerate cases
  if ((y_in < 0) || (N < 1)){
    *x_indx = 0.;
    Lindx = 0;
    return 0;
  } 
  if (y_in <= y[0]){
    if (y[0] > 0.0)
      gR = y_in/y[0];
    else
      gR = 1.0;
    v  = gR*x[0];
    *x_indx = 0.;
    Lindx = 0;
    return v;
  }  
  if (y_in >= y[N-1]){
    v  = x[N-1];
    gR = 0.0;
    *x_indx = (float)(N-1);
    return v;
  }

  // reset Lindx if out of bounds or too large
  if ((Lindx > N-1) || (Lindx < 0))
    Lindx = 0;
  if (y[Lindx] > y_in)  
    Lindx = 0;

  // now search for correct L & R neighbors
  for ( ; Lindx<N-1; Lindx++){
    if (y[Lindx+1] > y_in)
      break;
  }

  // verify...
  if (!((y[Lindx] <= y_in) && (y_in <= y[Lindx+1]))){
    save_src("debug_OGI_Timewarp_src.phn");
    save_targ("debug_OGI_Timewarp_targ.phn");
    fprintf(stderr,"OGI_TimeWarp::invmap() problem, Lindx=%d, y_in=%f \n  dumped src and targ info to debug_OGI_TimeWarp_*.phn files.",Lindx,y_in);
    EST_error("exiting...");
  }

  gR = (y_in - y[Lindx]) / (y[Lindx+1]-y[Lindx]);
  v  =  (1-gR)*x[Lindx] + gR*x[Lindx+1];
  *x_indx = Lindx + gR;

  //  printf("invmap: y=%f ==> x=%f (x_indx=%f)\n", y_in, v,*x_indx);

  return v;
}

void OGI_TimeWarp::nonuniform_stretch(
				      int n,  // do it between points n-1 and n-2 unless 1st seg
				      float modfactor, // desired stretch
				      float breakpt,   // proportion of segment
				      int half         // which half (1 or 2)
				      ){
  float x0,y0,x1,y1,src,targ=0;
  // check n valid
  if ((n > Nseg()) || (n < 1))
    EST_error("OGI_TimeWarp::nonuniform_stretch requested illegal segment\n");
    
  if (modfactor < 0.0001)
    EST_error("OGI_TimeWarp::nonuniform_stretch - illegal mod factor input\n");
    
  breakpt = max(min(breakpt, (float)0.9999), (float)0.0001);
  
  if (n > 1){
    x0 = x.a(n-2);
    y0 = y.a(n-2);
  }
  else{
    x0 = 0.0;
    y0 = 0.0;
  }
  x1 = x.a(n-1);
  y1 = y.a(n-1);
  
  if (half == 2){
    src  = x1 - breakpt*(x1 - x0);
    targ = y1 - modfactor*(x1 - src);
  }
  else{
    src  = x0 + breakpt*(x1 - x0);  
    targ = y0 + modfactor*(src - x0);
  }

  // make sure it is a legal warp
  targ = min(max(targ, y0 + (float)0.0001*(y1-y0)), y1-(float)0.0001*(y1-y0));
  
  // insert a point into time warp function
  insert_before(n-1, src, targ);
}


void OGI_TimeWarp::append(float x_new, float y_new){
  if (N+1 > x.length()){    // realloc if needed
    x.resize(x.length() + TIMEWARP_BUF_INCR);
    y.resize(y.length() + TIMEWARP_BUF_INCR);
  }
  
  // check that it's a function
  if ((N >0) && (!(y_new >= y(N-1)) || !(x_new >= x(N-1))))
    EST_error("OGI_TimeWarp::append: inputs (%f,%f) are out of order (%f,%f) \n",x_new, y_new, x(N-1), y(N-1));
  else {
    x.a(N) = x_new; 
    y.a(N) = y_new; 
    N++;
  }
  
  Lindx = -1; // reset invmap search
}

void OGI_TimeWarp::insert_before(int n, float x_new, float y_new){

  int k;
  if ((n < 0) || (n >= x.length()))
    EST_error("OGI_TimeWarp::insert_before out of range");
  
  // realloc if needed
  if (N+1 > x.length()){    // realloc if needed
    x.resize(x.length() + TIMEWARP_BUF_INCR);
    y.resize(y.length() + TIMEWARP_BUF_INCR);
  }
  
  // check that it's a function
  if ((n >0) && (!(y_new >= y(n-1)) || !(x_new >= x(n-1))))
    EST_error("OGI_TimeWarp::insert_before: inputs (%f,%f) are out of order (%f,%f) \n",x_new, y_new, x(N-1), y(N-1));
  else {
    
    // shift right: n to end
    for (k=N-1; k>=n; k--){
      x.a(k+1) = x.a(k);
      y.a(k+1) = y.a(k);
    }
    
    // new point
    x.a(n) = x_new; 
    y.a(n) = y_new; 
    N++;

    EST_String tmp = names.nth(n) + "_s";
    names.insert_before(names.nth_pointer(n), tmp);
  }
  
  Lindx = -1; // reset invmap search
}

float OGI_TimeWarp::fwdmap(float x_in, float *y_indx) {
  // returns interpolated, fwderse-mapped value v(*y_indx)
  // int part of *y_indx is index to left in y array
  // float part of *y_indx is weight to apply to indx to RIGHT in y array
  // round *y_indx to get nearest int y value
  //
  // **implicitly assumes time pair (y[-1],x[-1]) = (0,0)
  //
  // Lindx is cached to try to save search time
  // N is number of src/targ segments
  float v;   // fwdmapped return value 
  float gR;  // floating pt part of y_indx returned

  // degenerate cases
  if ((x_in < 0) || (N < 1)){
    *y_indx = 0.;
    Lindx = 0;
    return 0;
  } 
  if (x_in <= x[0]){
    if (x[0] > 0.0)
      gR = x_in/x[0];
    else
      gR = 1.0;
    v  = gR*y[0];
    *y_indx = 0.;
    Lindx = 0;
    return v;
  }  
  if (x_in >= x[N-1]){
    v  = y[N-1];
    gR = 0.0;
    *y_indx = (float)(N-1);
    return v;
  }

  // reset Lindx if out of bounds or too large
  if ((Lindx > N-1) || (Lindx < 0))
    Lindx = 0;
  if (x[Lindx] > x_in)  
    Lindx = 0;

  // now search for correct L & R neighbors
  for ( ; Lindx<N-1; Lindx++){
    if (x[Lindx+1] > x_in)
      break;
  }

  // verify...
  if (!((x[Lindx] <= x_in) && (x_in <= x[Lindx+1]))){
    save_src("debug_OGI_Timewarp_src.phn");
    save_targ("debug_OGI_Timewarp_targ.phn");
    fprintf(stderr,"OGI_TimeWarp::fwdmap() problem, Lindx=%d, x_in=%f \n  dumped src and targ info to debug_OGI_TimeWarp_*.phn files.",Lindx,x_in);
    EST_error("exiting...");
  }

  gR = (x_in - x[Lindx]) / (x[Lindx+1]-x[Lindx]);
  v  =  (1-gR)*y[Lindx] + gR*y[Lindx+1];
  *y_indx = Lindx + gR;

  //  printf("fwdmap: x=%f ==> y=%f (y_indx=%f)\n", x_in, v,*y_indx);

  return v;
}


