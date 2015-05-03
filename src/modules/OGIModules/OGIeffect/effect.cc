#include <stdlib.h>
#include "festival.h"
#include "OGIeffect.h"
#include "effect_io.h"
#include "delay.h"
#include "reverb.h"

static void apply_effects(EST_Wave *ww);
  

LISP OGIeffect_Utt(LISP utt){
  EST_Utterance *u = get_c_utt(utt);
  EST_Item *witem = u->relation("Wave")->head();
  EST_Wave *ww = wave(witem->f("wave"));
  apply_effects(ww);

  return utt;
}

static void apply_effects(EST_Wave *ww){
  
  int k;
  
  if (!(OGIeffect_params_loaded()))
    return;
  OGIeffectParams *prm = get_OGIeffect_params();

  ww->rescale(prm->pre_gain);  
  resize_wave_smartly(ww);
  
  for (k=0; k<prm->Neffect; k++){
    switch (prm->effect_order[k]) {
    case doREVERB:
      //cout << "applying reverb\n";
      reverb(ww);
      break;

    case doECHO:
      //    cout << "applying echo\n";
      echo(ww);
      break;
      
    case doSLAPBACK:
      //    cout << "applying slap\n";
      slapback(ww);
      break;
      
    default:
      // skip
      break;
    }
  }

  ww->rescale(prm->post_gain);
  
}


LISP Test_Utt(LISP utt){
  EST_Utterance *u = get_c_utt(utt);
  EST_Wave *ww = wave(u->relation("Wave")->head()->f("wave"));
  short x = 1;
  ww->values().def_val = &x;
  cout << ww->values().def_val << "   " <<  *(ww->values().def_val) <<  endl;
  ww->resize(ww->length()*5);
  cout << ww->values().def_val << "   " <<  *(ww->values().def_val) <<  endl;
  return utt;
}

 
void festival_OGIeffect_init(void){

  proclaim_module("OGIeffect");

  init_subr_1("Test.Utt",Test_Utt,
	      "(Test.Utt UTT)");

  init_subr_1("OGIeffect",OGIeffect_Utt,
	      "(OGIeffect UTT) \n\
Routine for adding various effects like reverb, etc. to Wave output. \n\
Requires Wave relation.  Initialize with OGIeffect.init.");


  init_subr_1("OGIeffect.init",OGIeffect_Init,
    "(OGIeffect.init PARAMS)\n\
   Initialize effects. \n\
   PARAMS is an assoc list of parameter name and value.");
  
}


