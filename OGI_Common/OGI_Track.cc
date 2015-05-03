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

#include "OGI_Track.h"


int OGI_Track::insert_t(OGI_BuffFVect &vect){
  if (vect.Nelem() < 1)
    return 0;
  
  EST_Track endv;
  int k,n,ch;   
  int indx_ins_before,indx_ins_after;
  
  
  // this overwrites all the stray ones in between
  indx_ins_after = index_below_cached(vect(0));
  indx_ins_before = index_above(vect.last());
  
  
  // copy end of track to a temp location
  if (indx_ins_before < num_frames()){
    endv.resize(num_frames()-indx_ins_before, num_channels());

    for (k=indx_ins_before; k<num_frames(); k++){
      endv.t(k-indx_ins_before) = t(k);
      for (ch=0; ch<num_channels(); ch++)
	endv.a_no_check(k-indx_ins_before,ch) = a_no_check(k,ch);
    }
  }

  // resize the track
  set_num_frames(indx_ins_after+1 + vect.Nelem() + num_frames()-indx_ins_before);
  
  // copy in the new time values
  n=indx_ins_after+1;
  for(k=0; k<vect.Nelem(); k++){
    t(n) = vect(k);
    for (ch=0; ch<num_channels(); ch++){
      a_no_check(n,ch) = 0.0;
    }
    n++;
  }
  
  // copy in the remaining part of orig vector
  for(k=0; k<endv.num_frames(); k++){
    t(n) = endv.t(k);
    for (ch=0; ch<num_channels(); ch++){
      a_no_check(n,ch) = endv.a_no_check(k,ch);
    }
    n++;
  }
  
  // verify
  //verify_t_order("debug_OGI_Track_insert_t.est");
  
  return indx_ins_before;  // return start index of new sect
}


// append whole track, with time offset
int OGI_Track::offset_append(EST_Track &in, float offset_t){
  static float pad=(float)0.001;
  
  if (num_channels() != in.num_channels())
    resize(EST_CURRENT,in.num_channels());

  if (!setup_done){
    copy_setup(in);
    setup_done = TRUE;
  }
  
  int Norig = num_frames();
  int N = Norig;
  if ((Norig > 0) && (in.num_frames() > 0)){
    if (in.t(0) + offset_t < last_t() + pad){ 	// must cut some existing ones
      N = index_below_cached(in.t(0) + offset_t - pad)+1;
    }
  }
  
  resize(N + in.num_frames(), EST_CURRENT);
  
  int k;
  if (num_channels() > 0){
    for (k=0; k<in.num_frames(); k++){
      copy_frame_in(N+k, in, k);   // @1.4
      t(N+k) = in.t(k) + offset_t;
    }
  }
  else {
    for (k=0; k<in.num_frames(); k++){
      t(N+k) = in.t(k) + offset_t;
    }
  }
  


  //  verify_t_order("debug_OGI_Track_offset_append.est");
  return Norig- N;
  
}


void  OGI_Track::min_chan(int ch, float tB, float tE, float *minT, float *minC){
    int n;
    int iB = index_above(tB);

    if (iB >= num_frames()){
      *minC = a_no_check(num_frames()-1,ch);
      *minT = t(num_frames()-1);
      return;
    }

    *minC = a_no_check(max(iB,0),ch);
    *minT = t(max(iB,0));

    for (n=iB; n<num_frames(); n++){
      if (t(n) > tE)
	break;
      
      if (a_no_check(n, ch) > *minC){
	*minC = a_no_check(n, ch);
	*minT = t(n);
      }
    }
  }
      

int OGI_Track::index_below_cached(float x){
  // end result is cached in member variable 'prev_indx'
  int B=0;

  if (num_frames() == 0){
    prev_indx=-1;
    return prev_indx;
  }
  
  if ((num_frames() == 1) && (x > t(0))){
    prev_indx=0;
    return prev_indx;
  }

  if (x <= t(0)){
    prev_indx=-1;
    return prev_indx;
  }

  if (x > t(num_frames()-1)){
    prev_indx=num_frames()-1;
    return prev_indx;
  }
    
  if ((prev_indx > -1) && (prev_indx < num_frames())){
    if (x > t(prev_indx))
      B = prev_indx;
  }
  else // full search
    B = 1;

  int i;
  for (i=B; i<num_frames(); i++){
    if (x <= p_times.a_no_check(i)){
      prev_indx = i-1;
      return prev_indx;
    }
  }
  return num_frames()-1;
}


//
// moving average with antisymmetric extension of endpoints 
//   - keeps endpoints of vector same after filtering 
//
void OGI_Track::smooth_ma(int frB, int frE, int chB, int Nchan, int smoothlen){
  int ch,n,l;
  EST_FVector y(frE-frB+1);
  int sl2 = (int) min(frE-frB-1, (smoothlen-1)/2);
  smoothlen = 2*sl2+1;
  double accum, a_offset;
    
  if (smoothlen < 2) 
    return;
  
  // for each channel
  for (ch=chB; ch<chB+Nchan; ch++){
    a_offset = a_no_check(frB, ch);

    // smooth frames using antisymmetric extensions
    for (n=frB; n<=frE; n++){
      accum = 0.0;
      for (l=-sl2; l<=sl2; l++){
	if (n+l < frB)
	  accum += 2*a_no_check(frB,ch) - a_no_check(2*frB-(n+l), ch);
	else if (n+l > frE)
	  accum += 2*a_no_check(frE,ch) - a_no_check(2*frE-(n+l), ch);
	else
	  accum += a_no_check(n+l,ch);
      }      
      y[n-frB] = (float) (accum/smoothlen);
    }

    // put result back into channel
    for (n=frB; n<=frE; n++)
      a_no_check(n,ch) = y(n-frB);
    
  }
}


//
// add an offset to each channel, increasing linearly from frB-->frM and frM<--frE,
//  such that left and right sides meet at frame frM-0.5
//
void OGI_Track::linear_match(int frB, int frM, int frE, int chB, int Nchan){
 
  int ch,k;
  double g,incr, midtarg;
  
  // VP In cas of non symetric smoothing area
  int avail_right= frE - frM + 1;
  int avail_left = frM - frB;
  int total_frame= avail_right + avail_left;

  // Nothing to do?
  if (total_frame<2)
    return;
  
  // for each channel
  for (ch=chB; ch<chB+Nchan; ch++)
    {
      // VP pick center of gravity instead of average
      // avg of M-1, M  
      // midtarg = 0.5 * ( a_no_check(frM-1, ch) + a_no_check(frM, ch));
      midtarg = ( a_no_check(frM-1, ch) * avail_right + a_no_check(frM, ch) * avail_left)/ total_frame ;
  
      // LEFT
      incr = (midtarg-a_no_check(frM-1, ch)) / (frM-frB+0.5);
      g = incr;
      
      for (k=frB; k<frM; k++)
	{      
	  a_no_check(k,ch) = (float) (a_no_check(k,ch) + g);
	  g += incr;
	}

      // RIGHT
      incr = (midtarg-a_no_check(frM, ch)) / (frE-frM+1.5);
      g = incr;
      for (k=frE; k>=frM; k--)
	{      
	  a_no_check(k,ch) = (float) (a_no_check(k,ch) + g);
	  g += incr;
	}
    }
}

//
// replace frB-->frM and frM-->frE with linear functions
//  such that left and right sides meet at frame frM-0.5
//
void OGI_Track::linear_replace(int frB, int frM, int frE, int chB, int Nchan){
 
  int ch,k;
  double g,incr, midtarg;

  // for each channel
  for (ch=chB; ch<chB+Nchan; ch++){
    
    // avg of M-1, M  
    midtarg = 0.5 * (a_no_check(frM-1, ch) + a_no_check(frM, ch));

    // LEFT
    incr = (midtarg-a_no_check(frM-1, ch)) / (frM-frB+0.5);
    g = incr + a_no_check(max(0, frB-1), ch);
    for (k=frB; k<frM; k++){      
      a_no_check(k,ch) = (float) g;
      g += incr;
    }

    // RIGHT
    incr = (midtarg-a_no_check(frM, ch)) / (frE-frM+1.5);
    g = incr + a_no_check(min(num_frames()-1, frE+1), ch);
    for (k=frE; k>=frM; k--){      
      a_no_check(k,ch) = (float) g;
      g += incr;
    }
  }
}


// save pmarks as a waveform
void  OGI_Track::save_pm_as_wav(EST_String &filename, float Fs){
  int n,k;
  int len = (int) (Fs*last_t() + 2);
  EST_Wave wav(len ,1, (int) Fs);
  wav.fill();

  for (k=0; k < num_frames(); k++){
    n = (int)(t(k)*Fs + 0.5);
    wav.a_no_check(n) = 10000;
  }
  
  wav.save(filename);
}

