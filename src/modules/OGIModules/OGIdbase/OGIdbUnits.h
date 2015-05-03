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
#ifndef __OGI_DBUNITS_H_
#define __OGI_DBUNITS_H_


#include "OGI_file.h"
//#define INLINE 

class OGIdbUnits {
  // 
private:
  OGIbool is_gLindx;  // use these global vals?
  OGIbool is_gRindx; 
  OGIbool is_gLbnd;
  OGIbool is_gRbnd;
  OGIbool is_gsegB; 

  int gLindx;  // left neighbor 
  int gRindx;  // right neighbor 
  OGIbool gLbnd;  // left boundary is a segment boundary (when all same)
  OGIbool gRbnd;  // left boundary is a segment boundary (when all same)
  int gsegB;   // beginning of each segment (in samples) when all are same  

  EST_StrVector p_name;
  EST_IVector p_Lindx;
  EST_IVector p_Rindx;
  
  EST_TVector<OGIbool>  p_Lbnd;
  EST_TVector<OGIbool>  p_Rbnd;
  EST_IVector p_segB;  // beginning of 1st segment in unit (samples)

                    // (could replace these with list of EST_Items?)
  //EST_TVector<EST_StrVector *> p_seg_names;
  //EST_TVector<EST_IVector *> p_seg_ends; 
  EST_TVector<void *> p_seg_names;  // (EST_StrVector *)
  EST_TVector<void *> p_seg_ends;   // (EST_IVector *)

  EST_StrVector p_file;

public:
  
  OGIdbUnits();
  ~OGIdbUnits();

  // read/write non-const access operators: without bounds checking  
  INLINE int Nunits() { return p_name.length(); }
  INLINE EST_String &name(int n) { return p_name.a(n); }

  // added for cronk
  INLINE EST_String &Rname(int n) { return p_name.a(Rindx(n)); }

  INLINE EST_String &file(int n) { return p_file.a(n); }
  INLINE int &Lindx(int n) { return p_Lindx.a(n); }
  INLINE int &Rindx(int n) { return p_Rindx.a(n); }
  INLINE OGIbool &Lbnd(int n) { return p_Lbnd.a(n); }
  INLINE OGIbool &Rbnd(int n) { return p_Rbnd.a(n); }
  INLINE int &segB(int n) { return p_segB.a(n); }

  // begin and end of whole unit
  INLINE int unitB(int n) { return seg_beg(n,0); }
  INLINE int unitE(int n) { return seg_end(n,Nseg(n)-1); }

  INLINE int Nseg(int n) { return ((EST_StrVector *)p_seg_names(n))->length(); }
  INLINE void alloc_Nseg(int l, int Nsegs) { 
    //p_seg_names.a(l) = new EST_StrVector(Nsegs);
    //p_seg_ends.a(l)  = new EST_IVector(Nsegs);
    //(EST_StrVector *)p_seg_names.a(l) = new EST_StrVector(Nsegs);
    //(EST_IVector *)p_seg_ends.a(l)  = new EST_IVector(Nsegs);
    p_seg_names.a(l) = new EST_StrVector(Nsegs);
    p_seg_ends.a(l)  = new EST_IVector(Nsegs);
  }
  INLINE void resize_seg(int l, int newNsegs) { 
    ((EST_StrVector *)p_seg_names.a(l))->resize(newNsegs);
    ((EST_IVector *)p_seg_ends.a(l))->resize(newNsegs);
  }

  INLINE EST_String &seg_name(int n, int m) { 
    return ((EST_StrVector *)p_seg_names[n])->a(m);
  }
  
  INLINE int &seg_end(int n, int m) { 
    // this is the one you should set (not set_beg)
    return ((EST_IVector *)p_seg_ends[n])->a(m);
  }
  INLINE int seg_beg(int n, int m) { 
    if (m==0){  return p_segB.a(n);}
    return seg_end(n,m-1);
  }  
  INLINE int seg_dur(int n, int m) { 
    return seg_end(n,m) - seg_beg(n,m); // in samples!
  }

  // resize all vectors
  void resize(int N);

  // check for all values of a vector are same, if so, just save 1 global val
  void condense(void);
  void free_condensed(void);

  void load_grp(FILE *gfd, OGIbool swap);  //  load unitdic info
  void save_grp(FILE *gfd);             //  save unitdic info
  void load_file(EST_String &unitdic_file, float Fs);  //  load unitdic info from unitdic file

  void print(EST_String &filename);  // for debug

};


#endif
