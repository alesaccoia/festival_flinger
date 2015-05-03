#include <stdlib.h>
#include "EST.h"
#include "OGIeffect.h"

static void create_OGIeffect_params(void);
static void delete_params(void);
static void unlisp_reverb_params(LISP prm, t_reverb *r);
static void unlisp_echo_params(LISP prm, t_echo *d);
static void unlisp_slapback_params(LISP prm, t_slapback *d);
static void unlisp_effect_order(LISP prm, OGIeffectParams *eff);

// STATIC struct containing info from Scheme layer
static OGIeffectParams *In=NULL;

LISP OGIeffect_Init(LISP params){

  LISP lispprm;

  // Alloc new struct (free first if already exists)
  //  and assign to static In
  create_OGIeffect_params();

  lispprm   = get_param_lisp("effect_order",params,NIL);

  if (lispprm != NIL)
    unlisp_effect_order(lispprm, In);

  lispprm   = get_param_lisp("reverb",params,NIL);
  if (lispprm != NIL)
    unlisp_reverb_params(lispprm, &(In->reverb));

  lispprm   = get_param_lisp("echo",params,NIL);
  if (lispprm != NIL)
    unlisp_echo_params(lispprm, &(In->echo));

  lispprm   = get_param_lisp("slapback",params,NIL);
  if (lispprm != NIL)
    unlisp_slapback_params(lispprm, &(In->slapback));

  In->post_gain = get_param_float("post_gain",params,1.0);
  In->pre_gain = get_param_float("pre_gain",params,1.0);

  return NIL;

}

static void create_OGIeffect_params(void){

  OGIeffectParams *new_in;

  // Alloc a new struct
  new_in = walloc(OGIeffectParams, 1);

  // Defaults are reset in OGIeffect_Init 
  //  -- these are only used if OGIeffect_Init is never called
  //     (i.e., if scheme never calls OGIeffect_Init)
  new_in->pre_gain = 1.0;				// YK 2011-07-25 added
  new_in->post_gain = 1.0;
  new_in->Neffect = 0;
  new_in->reverb.Ntaps = 0;
  new_in->reverb.dtime = NULL;
  new_in->reverb.w = NULL;
  new_in->reverb.r = NULL;
  new_in->echo.dtime = 0;
  new_in->echo.w = 0.0;
  new_in->slapback.dtime = 0;
  new_in->slapback.w = 0.0;
  
  if (In != NULL){
    delete_params();
  }

  // Assign to static   
  In = new_in;
}

static void delete_params(void){
  
  if (In->Neffect > 0)
    wfree(In->effect_order);

  if (In->reverb.Ntaps > 0){
    wfree(In->reverb.dtime);
    wfree(In->reverb.w);
    wfree(In->reverb.r);
  }
  In = NULL;
}

// Get ptr to struct
OGIeffectParams *get_OGIeffect_params(void){
    return(In);
}

// Get ptr to struct
int OGIeffect_params_loaded(void){
  if (In == NULL)    return(0);
  else               return(1);
}

static void unlisp_reverb_params(LISP prm, t_reverb *rev){

  LISP l;
  int i=0;

  rev->Ntaps = siod_llength(prm);
  
  rev->dtime = walloc(int, rev->Ntaps);
  rev->w = walloc(float, rev->Ntaps);
  rev->r = walloc(float, rev->Ntaps);

  for (l=prm; l != NIL; l=cdr(l)){
    rev->dtime[i]= (int) get_c_int(car(car(l)));
    rev->w[i]  = (float) get_c_float(car(cdr(car(l))));
    rev->r[i]  = (float) get_c_float(car(cdr(cdr(car(l)))));
    i++;
  }
}

static void unlisp_echo_params(LISP prm, t_echo *d){
  d->dtime = get_param_int("dtime",prm,0);
  d->w = get_param_float("feedback",prm,0.0);
}

static void unlisp_slapback_params(LISP prm, t_slapback *d){
  d->dtime = get_param_int("dtime",prm,0);
  d->w = get_param_float("feedback",prm,0.0);
}

static void unlisp_effect_order(LISP prm, OGIeffectParams *eff){
  LISP l;
  int i = 0;
  
  eff->Neffect = siod_llength(prm);
  eff->effect_order = walloc(t_effect_order, eff->Neffect);

  for (l=prm; l != NIL; l=cdr(l)){
    EST_String x = get_c_string(car(l));
    if (x == "reverb")
      eff->effect_order[i++] = doREVERB;
    else if (x == "echo")
      eff->effect_order[i++] = doECHO;
    else if (x == "slapback")
      eff->effect_order[i++] = doSLAPBACK;
    else {
      cerr << "OGIeffect:  don't know effect \"" << x << "\"  (skipping) \n";
    }
  }
}

