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
#ifndef __OGI_WAVECHUNK_H__
#define __OGI_WAVECHUNK_H__

#include "EST.h"
#include "OGI_Buffer.h"
#include "OGI_Macro.h"
#include "OGI_file.h"

#define OGI_CHUNK_INCR 8000

class OGI_WaveChunk {

  //  WaveChunk - maintains a set of concatenated waveforms without committing to the 
  //   exact join points or throwing anything away.
  //
  //   chunks        = the whole wave for each concat segment
  //   offset        = markers for beginning of each chunk
  //   active_chunks = the part we intend to use
  //   start         = markers for beginnings of each active_chunk
  //   end           = markers for ends of each active_chunk
  //   true_samp(i)  = \sum_{k=0}^{i-1} (end(k)-start(k))   
  //     (offset, start, end are all measured in samples, relative to begin of the chunks wave)
  
private:
  
  EST_Wave chunks;
  EST_IList start; // start of active_chunk relative to begin of the EST_Wave
  EST_IList end;   //   end of active_chunk relative to begin of the EST_Wave
  EST_IList true_samp; // true_samp(i) = \sum_{k=0}^{i-1} (end(k)-start(k))
  EST_IList offset; // start of chunks relative to begin of the EST_Wave

  int used_length; // amount actually used by all concatenated chunks

  // use in fast get()
  int *startV; 
  int *endV;   
  int *true_sampV;
  int *offsetV;   
  int NchunkV;
  bool done_setup_get;

public:
  OGI_WaveChunk(){
    start.clear();
    end.clear();
    true_samp.clear();
    offset.clear();

    true_samp.append(0);
    offset.append(0);
    used_length = 0;

    done_setup_get = FALSE;
  }
  
  void set_sample_rate(int Fs){
    chunks.set_sample_rate(Fs);
  }
  float Fs (void){
    return (float) (chunks.sample_rate());
  }
  void prealloc(int N){
    chunks.resize(N, EST_ALL, 0);
  }

  void append_chunk(EST_Wave &wav, int B, int E, int combineflag);
  void buf_append_chunk(int chk_cut, EST_Wave &wav, int B);

  // which chunk is sample 'samp' in?
  INLINE int which_chunk(int samp);
  INLINE int Nchunks(void){
    return start.length();
  }

  int tot_chunk_length(){
    return used_length;
  }
  
  int tot_chunk_alloc(){
    return chunks.length();
  }
  
  // sum of lengths of all the active_chunks
  INLINE int length(){
    return true_samp.last();
  }
  
  // last "sig_band" length
  INLINE int last_extra(){
    return (used_length - end.last());
  }
  
  void adjust_last_segE(int adj);


  // get
  //  - get waveform from a single chunk, from center-back to center+forward  
  //     (back should be >= 0)
  //  - allows to get portion running into "sig_band", then inserts zeros
  //  - always gives back a waveform of size asked for
  //
  void get(int center, int back, int forward, OGI_PitchSyncBuffer &outbuf);

  void save_chunks(const EST_String &filename, const EST_String &ftype){
    chunks.save(filename, ftype);
  }
  void save_chunks(const EST_String &filename){
    chunks.save(filename, "nist");
  }
  void load_chunk(const EST_String &filename);

  // optimized versions
  int which_chunk_fast(int samp);
  void setup_fast_get(void);
  void end_fast_get(void);

  //VP  void get_fast(int center, int back, int forward, OGI_PitchSyncBuffer &outbuf);
  void get_fast(int center, int back, int forward, int back_with_zeros, int forward_with_zeros, OGI_PitchSyncBuffer &outbuf);

  //
  // dump functions that dump chunks and a label file with offset, start, end, true_samp
  //
  void dump_chunks(const EST_String &basename);
  void dump_trimmed(const EST_String &basename);



  void verify(void){
    cout << "\n-------------\n";
    cout << "length\t" << used_length << endl;
    cout << "start\t" << start << endl;
    cout << "end  \t" << end << endl;
    cout << "trues\t" << true_samp << endl;
    cout << "offset\t" << offset<< endl;
    cout << "-------------\n";

    int acc=0;
    int k=0;
    for (k=0;k<start.length(); k++){
      if (true_samp.nth(k) != acc){
	printf("*** true_samp(%d) down't agree: %d != acc=%d \n",k,  true_samp.nth(k), acc);
      }
      acc += end.nth(k) - start.nth(k);
    }
  }
};



#endif

