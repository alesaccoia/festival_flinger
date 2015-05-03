/****************************<--OGI-->*************************************
*                                                                        *
*             Center for Spoken Language Understanding                   *
*        Oregon Graduate Institute of Science & Technology               *
*                         Portland, OR USA                               *
*                        Copyright (c) 2000                              *
*                                                                        *
*      This module is not part of the CSTR/University of Edinburgh       *
*               release of the Festival TTS system.                      *
*                                                                        *
*  In addition to any conditions disclaimers below, please see the file  *
*  "license_ogi_tts.txt" distributed with this software for information  *
*  on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  *
*                                                                        *
****************************<--OGI-->*************************************/
#ifndef __OGI_TRACK_H__
#define __OGI_TRACK_H__

#include "EST.h"
#include "OGI_file.h"
#include "OGI_Buffer.h"
#include "OGI_Macro.h"


#define OGI_UNDEFINED -1.0
#define OGI_HUGE_VAL 1.0e30

class OGI_Track : public EST_Track {
private:
  bool setup_done;
  int prev_indx; // used to cache index_below searches
  int Nbuf_used;
public:
  OGI_Track(){
    setup_done=FALSE;
    prev_indx = -1;
    Nbuf_used = 0;
  }

  // append a single time point
  int append_t(float t_in){
    // slow way
    int N=num_frames();
    set_num_frames(N+1);
    t(N) = t_in;
    return N;
  }

  // (buffered) append of a single time point
  #define OGI_TRACK_BUF_INCR 500
  INLINE int append_t_buf(float t_in){
    if (Nbuf_used >= num_frames()){
      set_num_frames(Nbuf_used + OGI_TRACK_BUF_INCR);
    }
    t(Nbuf_used) = t_in;
    Nbuf_used++;
    return Nbuf_used-1;
  }
  // must call this before starting buffered appends
  void append_t_buf_start(void){
    Nbuf_used = num_frames();
  }
  // must call this when finished with buffered appends
  void append_t_buf_finish(void){
    set_num_frames(Nbuf_used);
  }

  // insert a set of time points
  int insert_t(OGI_BuffFVect &vect);

  // append whole track, with time offset
  //  return number of frames cut from end of track
  //   (usually 0)
  int offset_append(EST_Track &in, float offset_t);

  // shift time points after t_in by offset_t
  void offset_t_after(float t_in, float offset_t){
    int k;
    for (k=0; k<num_frames(); k++){
      if (t(k) > t_in)
	t(k) += offset_t;
    }
    //    verify_t_order("debug_OGI_Track_offset_t_after.est");
  }


  INLINE float last_t(void){
    if (num_frames() == 0)
      return OGI_UNDEFINED;  // not sure of a better way
    return t(num_frames()-1);
  }


  INLINE float T0(int k){
    if (num_frames() < 2)
      return OGI_UNDEFINED;  // not sure of a better way

    // (don't count on these for access into arrays!)
    if (k > num_frames()-2)
      return t(num_frames()-1)-t(num_frames()-2);
    if (k < 0)
      return t(1)-t(0);

    return t(k+1)-t(k);
  }
  float max_T0(){
    float maxval = 0.0;
    int k;
    for (k=0; k<num_frames()-1; k++){
      maxval = max(maxval, t(k+1)-t(k));
    }
    return maxval;
  }

  INLINE float last_T0(void){
    if (num_frames() < 2)
      return OGI_UNDEFINED;  // not sure of a better way

    return t(num_frames()-1)-t(num_frames()-2);
  }


  // return time of min value in channel ch
  //   between times tB and tE (sec)
  float min_chan_t(int ch, float tB, float tE){
    float minC,minT;
    min_chan(ch, tB, tE, &minT, &minC);
    return minT;
  }

  // return min value in channel ch
  //   between times tB and tE (sec)
  float min_chan_val(int ch, float tB, float tE){
    float minC,minT;
    min_chan(ch, tB, tE, &minT, &minC);
    return minC;
  }
  void min_chan(int ch, float tB, float tE, float *minT, float *minC);

  // value nearest time t_in
  INLINE float nearest(float t_in, int ch){
    return a_no_check(nearest_indx(t_in), ch);
  }

  INLINE float interp(float t_in, int ch){
    float indxG =  nearest_indx_f(t_in);
    int i = (int) indxG;
    float G = indxG - i;
    if (i < num_frames()-1)
      return (1-G)*a_no_check(i, ch) + G*a_no_check(i+1, ch);
    else
      return a_no_check(i, ch);

  }


  //////////////////////////////////////////////////////////////////
  // finding index closest to float t_in
  //

  // index_below - returns -1 if x < t(0),  returns num_frames()-1 if x > t(num_frames()-1)
  //    if x == t(k), returns k-1
  //    check for -1 before using for array access!
  int index_below_cached(float x);

  //
  // index_above - returns 0 if x < t(0),  returns num_frames() if x > t(num_frames()-1)
  //    if x == t(k), returns k
  //    check for num_frames() before using for array access!
  int index_above(float x){
    return index_below_cached(x)+1;
  }

  INLINE float nearest_indx_f(float t_in){
    // floating point part is interp factor between the two indices
    float g=0.0;
    int i = index_below_cached(t_in);
    if (i < 0)
      return 0.0;

    if ((i < num_frames()-1) && (t_in > t(i))) // prevent /0
      g  = (t_in - t(i))/(t(i+1) - t(i));
    else
      g = 0.0;
    return (i+g);
  }

  INLINE int nearest_indx(float x){
    int i = index_below_cached(x);
    if (i < 0)
      return 0;
    if (i < num_frames()-1){
      if (fabs(x - t(i+1)) < fabs(x - t(i)))
	return i+1;
    }
    return i;
  }

  INLINE float nearest_t(float t_in){
    return t(nearest_indx(t_in));
  }

  INLINE double mean(int ch){
    int k;
    double meanE=0.0;
    for (k=0; k<num_frames(); k++)
      meanE += a_no_check(k,ch);
    meanE /= (float) num_frames();
    return meanE;
  }

  ////////////////////////////////////////////////////////////////
  // smoothing
  //

  // moving average with antisymmetric extension of endpoints
  //   - keeps endpoints of vector same after filtering
  void smooth_ma(int frB, int frE, int chB, int Nchan, int smoothlen);


  // add an offset to each channel, increasing linearly from frB-->frM and frM<--frE,
  //  such that left and right sides meet at frame frM-0.5
  void linear_match(int frB, int frM, int frE, int chB, int Nchan);

  // same as above, but replace instead of adding an offset
  void linear_replace(int frB, int frM, int frE, int chB, int Nchan);




  void verify_t_order(EST_String filename){
    int k;
    for (k=1; k<num_frames(); k++){
      if (t(k) <= t(k-1)){
	save(filename);
	fprintf(stderr,"  t[%d]=%f t[%d]=%f \n", k-1,t(k-1), k, t(k));
	EST_error("OGI_Track::verify_t_order:  track time instants out of order!\n   dumped to \"" + filename + "\" \n");
      }
    }
  }

  // save pmarks as a waveform
  void save_pm_as_wav(EST_String &filename, float Fs);

};


// read/write to file pointer -- change these, now that there are EST equivalents.
size_t OGIfread_track(EST_Track &tr, FILE *fp);
size_t OGIfwrite_track(EST_Track &tr, FILE *fp, bool compress=0);
size_t OGIfread_track(OGI_Track &tr, FILE *fp);
size_t OGIfwrite_track(OGI_Track &tr, FILE *fp, bool compress=0);



#endif  // #ifndef __OGI_TRACK_H__
