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
#ifndef __OGI_SINGLETRACK_H__
#define __OGI_SINGLETRACK_H__

#include "EST.h"
#include "OGI_Track.h"


#define OGI_SINGLETRACK_INCR 300

//
// Special case of a Track with only (t,val) pairs (one channel)
//
class OGI_SingleTrack : public OGI_Track {
  
private:
  bool setup_done;
  int Nused;
  bool append_in_progress;
public:
  OGI_SingleTrack(){
    set_num_channels(1);
    Nused = 0;
    append_in_progress = FALSE;
  }

  void prepend(float t_in, float val_in){
    // prepend a single (t,val) pair
    int N=num_frames();
    int k;
    EST_Track tmp;
    copy_sub_track(tmp);

    set_num_frames(N+1);
    t(0) = t_in;
    a(0,0) = val_in;
    
    for (k=1; k<N+1; k++)
      copy_frame_in(k, tmp, k-1);  //@1.4
  }

  void begin_append(int size_guess){
    Nused = num_frames();
    set_num_frames(Nused + size_guess);
    append_in_progress = TRUE;
  }
  void end_append(void){
    set_num_frames(Nused);
    append_in_progress = FALSE;
  }
  
  void append(float t_in, float val_in){
    if (!append_in_progress){
      begin_append(length()+OGI_SINGLETRACK_INCR);
    }

    // append a single (t,val) pair
    if (Nused + 1 > length()){
      set_num_frames(length() + OGI_SINGLETRACK_INCR);
    }

    t(Nused) = t_in;
    a_no_check(Nused,0) = val_in;
    Nused++;
  }
  

  float linterp(float t_in){
    return interp(t_in, 0);
  }
    
};



#endif  

