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
#include "OGI_WaveChunk.h"


#define CHUNK_OUT_OF_RANGE -1

  // which chunk is sample 'samp' in?
int OGI_WaveChunk::which_chunk(int samp){
  int out=-1;
  EST_Litem *s;
  for (s=true_samp.head(); s!=0; s=s->next()){
    if (true_samp(s) > samp)
      break;
    out++;
  }
  
  if ((out==-1) || (out >= Nchunks())){
    return CHUNK_OUT_OF_RANGE;
    EST_error("OGI_WaveChunk couldn't locate sample %d in OGI_WaveChunk (size=%d)\n",samp,length());
  }
  return out;
}

// which chunk is sample 'samp' in?
int OGI_WaveChunk::which_chunk_fast(int samp){
  int out=-1;
  int k;

  for (k=0; k<NchunkV; k++){
    if (true_sampV[k] > samp)
      break;
    out++;
  }
  
  if ((out==-1) || (out >= NchunkV)){
    return CHUNK_OUT_OF_RANGE;
    EST_error("OGI_WaveChunk couldn't locate sample %d in OGI_WaveChunk (size=%d)\n",samp,length());
  }
  return out;
}

void  OGI_WaveChunk::append_chunk(EST_Wave &wav, int B, int E, int combineflag){

  // if combine flag set, just merge wav with last chunk
  //   and discard samples after end.last() and before B
  if (combineflag){
    buf_append_chunk(used_length-end.last(), wav,B);
    
    end.last() = end.last() + E-B;
    true_samp.last() = true_samp.last() + E-B;
    offset.last() = used_length;
  }
  else{
    start.append(B + offset.last());
    end.append(E + offset.last());
    true_samp.append(true_samp.last() + (end.last()-start.last()));
    
    // append wave and record offset
    set_sample_rate(wav.sample_rate());
    buf_append_chunk(0,wav,0);
    offset.append(used_length);
  }
}

void OGI_WaveChunk::buf_append_chunk(int chk_cut, EST_Wave &wav, int B){

  short *s = chunks.values().memory();
  short *w = wav.values().memory();

  if (B >= wav.length())
    return;

  // REALLOC if not enough room to append
  if (used_length + wav.length()-B > chunks.length()){
    chunks.resize(used_length + wav.length()-B + OGI_CHUNK_INCR, EST_ALL, 1);
    s = chunks.values().memory();
    memset(&(s[used_length+wav.length()-B]), 0, 
	   (chunks.length()-(used_length + wav.length()-B))*sizeof(short));    
  }
  
  if (B >= 0){
    memcpy(&(s[used_length-chk_cut]), &(w[B]), (wav.length()-B)*sizeof(short));
  }
  else {
    memset(&(s[used_length-chk_cut]), 0, B);
    memcpy(&(s[used_length+B-chk_cut]), w, wav.length()*sizeof(short));
  }

  used_length += wav.length()-B-chk_cut;
}

// adjust end of last segment (for concatenation)
void OGI_WaveChunk::adjust_last_segE(int adj){

  short *s = NULL;

  if (adj != 0){
    if (end.length() > 0){
      // insert zeros if we need more samples
      if ((end.last() + adj) > chunks.length()){
	chunks.resize(used_length + adj + OGI_CHUNK_INCR, EST_ALL, 0);
	s = chunks.values().memory();
	memset(&(s[used_length]), 0, 
	   (chunks.length()-(used_length + adj)*sizeof(short)));    
	used_length += adj;
	offset.last() = end.last() + adj;
      }
      end.last() = end.last() + adj;
      true_samp.last() = true_samp.last() + adj;
    }
    else
      cerr << "OGI_WaveChunk: can't adjust segE before appending something !\n";
  }
}
void OGI_WaveChunk::setup_fast_get(void){
  EST_Litem *s;
  int k;

  NchunkV = true_samp.length();
  true_sampV = walloc(int, true_samp.length());
  for (k=0,s=true_samp.head(); s!=0; s=s->next()){
    true_sampV[k++] = true_samp(s);
  }
 
  startV = walloc(int, start.length());
  for (k=0,s=start.head(); s!=0; s=s->next()){
    startV[k++] = start(s);
  }
 
  endV = walloc(int, end.length());
  for (k=0,s=end.head(); s!=0; s=s->next()){
    endV[k++] = end(s);
  }
 
  offsetV = walloc(int, offset.length());
  for (k=0,s=offset.head(); s!=0; s=s->next()){
    offsetV[k++] = offset(s);
  }
  done_setup_get = TRUE;
}
void OGI_WaveChunk::end_fast_get(void){
 
  if (done_setup_get){
    wfree(true_sampV);
    wfree(startV);
    wfree(endV);
    wfree(offsetV);
  }
  done_setup_get = FALSE;
}

// VP: changed function interface
void OGI_WaveChunk::get_fast(int center, int back, int forward, int back_with_zeros, int forward_with_zeros, OGI_PitchSyncBuffer &outbuf){
  int next_offset;
  
  if (!done_setup_get){
    setup_fast_get();
  }

  int chnk = which_chunk_fast(center);
  if (chnk == CHUNK_OUT_OF_RANGE){
    outbuf.clear();  // just give back empty buffer
    return;
  }

  if (chnk == offset.length())
    next_offset = used_length;
  else
    next_offset = offsetV[chnk+1];
  
  // available # samples to back and forward of center
  int avail_back = center - true_sampV[chnk] + startV[chnk] - offsetV[chnk];
  int avail_fwd  = true_sampV[chnk] + endV[chnk]-startV[chnk] - center + next_offset - endV[chnk];

  // begin, mid, end relative to begin of chunks array
  int Mid = offsetV[chnk] + avail_back;
  int Begin = Mid - min(back, avail_back);
  int End   = Mid + min(forward, avail_fwd);
  
  outbuf.clear();
  
  // insert zeros to make sure you get the length asked for
  // VP outbuf.set_mid_end(back, back+forward);
  // VP double *out = &(outbuf.buf[ back_with_zeros - (Mid-Begin)]);  
  outbuf.set_mid_end(back_with_zeros, back_with_zeros + forward_with_zeros);
  double *out = &(outbuf.buf[ back_with_zeros - (Mid-Begin)]);  

  int k;
  for (k=Begin; k<End; k++){
    *out++ = (double) chunks.a_no_check(k,0);
  }
}

void OGI_WaveChunk::get(int center, int back, int forward, OGI_PitchSyncBuffer &outbuf){
  
  static int prev_chunk  = -1;
  static int next_offset = -1;
  int true_samp_ck=0;  // big speedup
  int start_ck=0;
  int offset_ck=0;
  int end_ck=0;

  int chnk = which_chunk(center);
  if (chnk == CHUNK_OUT_OF_RANGE){
    outbuf.clear();  // just give back empty buffer
    return;
  }

  if (chnk != prev_chunk){
    true_samp_ck = true_samp.nth(chnk);
    start_ck = start.nth(chnk);
    offset_ck = offset.nth(chnk);
    end_ck = end.nth(chnk);

    if (chnk == offset.length())
      next_offset = used_length;
    else
      next_offset = offset.nth(chnk+1);
  }
  
  // available # samples to back and forward of center
  int avail_back = center - true_samp_ck + start_ck - offset_ck;
  int avail_fwd  = true_samp_ck + end_ck-start_ck - center + next_offset - end_ck;

  // begin, mid, end relative to begin of chunks array
  int Mid = offset_ck + avail_back;
  int Begin = Mid - min(back, avail_back);
  int End   = Mid + min(forward, avail_fwd);
  
  outbuf.clear();
  
  // insert zeros to make sure you get the length asked for
  outbuf.set_mid_end(back, back+forward);

  double *out = &(outbuf.buf[back - (Mid-Begin)]);  
  int k;
  for (k=Begin; k<End; k++){
    *out++ = (double) chunks.a_no_check(k,0);
  }
}

// void OGI_WaveChunk::get(int center, int back, int forward, OGI_PitchSyncBuffer &outbuf){
//   
//   static int prev_chunk  = -1;
//   static int next_offset = -1;
//   int true_samp_ck;  // big speedup
//   int start_ck;
//   int offset_ck;
//   int end_ck;
// 
//   int chnk = which_chunk(center);
//   if (chnk == CHUNK_OUT_OF_RANGE){
//     outbuf.clear();  // just give back empty buffer
//     return;
//   }
// 
//   if (chnk != prev_chunk){
//     true_samp_ck = true_samp.nth(chnk);
//     start_ck = start.nth(chnk);
//     offset_ck = offset.nth(chnk);
//     end_ck = end.nth(chnk);
// 
//     if (chnk == offset.length())
// 	 next_offset = used_length;
//     else
// 	 next_offset = offset.nth(chnk+1);
//   }
//   
//   // available # samples to back and forward of center
//   int avail_back = center - true_samp_ck + start_ck - offset_ck;
//   int avail_fwd  = true_samp_ck + end_ck-start_ck - center + next_offset - end_ck;
// 
//   // begin, mid, end relative to begin of chunks array
//   int Mid = offset_ck + avail_back;
//   int Begin = Mid - min(back, avail_back);
//   int End   = Mid + min(forward, avail_fwd);
//   
//   outbuf.clear();
//   
//   // insert zeros to make sure you get the length asked for
//   outbuf.set_mid_end(back, back+forward);
// 
//   double *out = &(outbuf.buf[back - (Mid-Begin)]);  
//   int k;
//   for (k=Begin; k<End; k++){
//     *out++ = (double) chunks.a_no_check(k,0);
//   }
// }

void OGI_WaveChunk::load_chunk(const EST_String &filename){
  EST_read_status ret = chunks.load(filename);
  if (ret == read_error)
    cout << "error reading file " << filename << endl;
  used_length = chunks.length();

  start.append(offset.last());
  end.append(used_length + offset.last());
  true_samp.append(true_samp.last() + (end.last()-start.last()));
  offset.append(used_length);    
}



//
// dump function that dumps chunks and a label file with offset, start, end, true_samp
//
void OGI_WaveChunk::dump_chunks(const EST_String &basename){
  
  int k,l;
  float Fs = (float)chunks.sample_rate();
  char cc[100];
  EST_StrList names;
  EST_FVector nums(3*start.length());
  EST_String fname;
  
  names.clear();
    
  // create labels
  for (k=0,l=0; k<start.length(); k++){      
    sprintf(cc, "offset");
    names.append(EST_String(cc));
    nums[l++] = start.nth(k)/Fs;
    
    sprintf(cc, "true_samp=%.1f",true_samp.nth(k)/Fs*1000.);
    names.append(EST_String(cc));
    nums[l++] = end.nth(k)/Fs;
    
    sprintf(cc, "end");
    names.append(EST_String(cc));
    nums[l++] = offset.nth(k+1)/Fs;
  }
  
  // wave
  fname = basename + ".wav";
  save_chunks(fname);
  
  // labels
  fname = basename + ".phn";
  save_OGI_label(nums, names, fname);
}


// dump function that dumps "trimmed" chunks and a label file
//   -- just does raw concatenation with no overlap
void OGI_WaveChunk::dump_trimmed(const EST_String &basename){
  
  int k,l;
  float Fs = (float)chunks.sample_rate();
  char cc[100];
  EST_StrList names;
  EST_FVector nums(start.length());
  EST_String fname;
  EST_Wave wav,totwav;
  totwav.set_sample_rate((int)Fs);

  names.clear();
  
  // for each chunk
  for (k=0,l=0; k<start.length(); k++){      
    // create labels
    sprintf(cc, "chunk%d",k);
    names.append(EST_String(cc));
    nums[l++] = (true_samp.nth(k) + end.nth(k)-start.nth(k))/Fs;
    
    // dump active chunks
    chunks.sub_wave(wav, start.nth(k), end.nth(k)-start.nth(k));
    totwav += wav;
  }

  // open out wave
  fname = basename + ".wav";
  totwav.save(fname,"riff");
  
  // labels
  fname = basename + ".phn";
  save_OGI_label(nums, names, fname);
}

