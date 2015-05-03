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
#include "EST.h"
#include "resLPC.h"
#include "OGI_Wave.h"
#include "resLPC_io.h"
#include "resLPC_wave.h"
#include "noLPC_wave.h"

#define WAVE_OUT_TYPE "riff"
static void dump_target_joins(EST_Utterance *u, OGIresLPC_SRC *src, OGIresLPC_MOD *mod, EST_String &filename);
  
#define OGIdumpmsg(A,B) cout << "\tsaving " << A << " to \"" << B << "\"" << endl;

void resLPC_dump(
		 EST_Utterance *u,
		 OGIresLPC_SRC *src,
		 OGIresLPC_MOD *mod,
		 OGI_Wave *outwav){

  EST_Features *gl = get_resLPC_params();
  LISP dumplist = read_from_lstring(strintern(gl->S("dump","NIL")));

  if (dumplist == NIL)
    return;

  EST_String filename, what;
  

  //
  // src data including extra "sig_band"
  //     
  what = "chunks";
  filename = EST_String(wstrdup(get_param_str(what, dumplist, "")));
  if (filename != "") {
    OGIdumpmsg(what, filename);
    src->exc.dump_chunks(filename); 
  }

  //  // src
  //     
  what = "srcseg";
  filename = EST_String(wstrdup(get_param_str(what, dumplist, "")));
  if (filename != "") {
    OGIdumpmsg(what, filename);
    mod->warp.save_src(filename); 
  }

  what = "srcexc";
  filename = EST_String(wstrdup(get_param_str(what, dumplist, "")));
  if (filename != "") {
    OGIdumpmsg(what, filename);
    src->exc.dump_trimmed(filename);  
  }

  what = "srcpm";
  filename = EST_String(wstrdup(get_param_str(what, dumplist, "")));
  if (filename != "") {
    OGIdumpmsg(what, filename);
    src->pm.save(filename); 
  }

  what = "srcpm_as_wav";
  filename = EST_String(wstrdup(get_param_str(what, dumplist, "")));
  if (filename != "") {
    OGIdumpmsg(what, filename);
    src->pm.save_pm_as_wav(filename, src->exc.Fs()); 
  }

  what = "srclpc";
  filename = EST_String(wstrdup(get_param_str(what, dumplist, "")));
  if (filename != "") {
    OGIdumpmsg(what, filename);
    src->lpc.save(filename); 
  }

  //
  // target
  //    
  what = "targseg";
  filename = EST_String(wstrdup(get_param_str(what, dumplist, "")));
  if (filename != "") {
    OGIdumpmsg(what, filename);
    mod->warp.save_targ(filename); 
  }

  what = "targjoin";
  filename = EST_String(wstrdup(get_param_str(what, dumplist, "")));
  if (filename != "") {
    OGIdumpmsg(what, filename);
    dump_target_joins(u, src, mod, filename);
  }

  what = "targexc";
  filename = EST_String(wstrdup(get_param_str(what, dumplist, "")));
  if (filename != "") {
    OGIdumpmsg(what, filename);

    OGI_Wave exconly;
    noLPC_wave_synth(src, mod, &exconly);
    EST_Wave *out_EST_Wave = new EST_Wave(exconly.Nused(), 1, exconly.mem(), 0, (int)(src->Fs), 1);
    out_EST_Wave->save(filename,WAVE_OUT_TYPE);
    delete out_EST_Wave;
  }

  what = "targpm";
  filename = EST_String(wstrdup(get_param_str(what, dumplist, "")));
  if (filename != "") {
    OGIdumpmsg(what, filename);
    mod->pm.save(filename); 
  }

  what = "targpm_as_wav";
  filename = EST_String(wstrdup(get_param_str(what, dumplist, "")));
  if (filename != "") {
    OGIdumpmsg(what, filename);
    mod->pm.save_pm_as_wav(filename, src->exc.Fs()); 
  }

  what = "pulses";
  filename = "individual files and 'targlpc_all.est'";
  if (get_param_int(what, dumplist, 0)){
    OGIdumpmsg(what, filename);
    mod->pm.save("targlpc_all.est"); 
  }

  what = "targlsf";
  filename = EST_String(wstrdup(get_param_str(what, dumplist, "")));
  if (filename != "") {
    if (get_param_int("pulses", dumplist, 0)){
      cout << "LSFs already dumped in 'targlpc_all.est' for pulses.\n";
    }      
    else {
      OGIdumpmsg(what, filename);
      resLPC_LSFonly(src, mod); // puts in mod->pm
      mod->pm.save(filename); 
    }
  }

  //  final output  (might be more convenient than getting from utt.save.wave)
  what = "output";
  filename = EST_String(wstrdup(get_param_str(what, dumplist, "")));
  if (filename != "") {
    OGIdumpmsg(what, filename);
    outwav->save(filename, WAVE_OUT_TYPE, (float)(int)(src->Fs)); 
  }
}


static void dump_target_joins(EST_Utterance *u,
			      OGIresLPC_SRC *src,
			      OGIresLPC_MOD *mod,
			      EST_String &filename){
  
  
  EST_Item *dbunit;
  EST_StrList names;
  EST_FVector nums(u->relation("dbUnit")->length());
  int srcpmR,srcpmL;
  int l=0;
  names.clear();
  
  // Loop thru SrcSeg, record 1st pmark of each segment
  dbunit=u->relation("dbUnit")->head();
  names.append(EST_String(dbunit->name()));
  dbunit = dbunit->next(); // skip 1st one

  for ( ; dbunit!=0; dbunit=dbunit->next()){
    
    // pmarks
    srcpmR = dbunit->I("lpcpm");
    srcpmL = max(0, srcpmR-1);
    
    // join point (middle of adjacent pmarks)
    nums[l++] = (float)0.5*(mod->warp.fwdmap(src->lpc.t(srcpmL)) + mod->warp.fwdmap(src->lpc.t(srcpmR)));
    
    // create labels
    names.append(EST_String(dbunit->name()));
  }
  if (l == 0) {
    cerr << "\tno joins\n";
    return;
  }

  nums[l] = src->exc.length()/src->Fs;
  
  // save labels
  save_OGI_label(nums, names, filename);
}



