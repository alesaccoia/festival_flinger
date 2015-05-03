#include <stdlib.h>
#include "festival.h"
#include "OGIeffect.h"
#include "effect_io.h"
 

int echo(EST_Wave *wave){

  int k;
  int clip=0;

  if (!(OGIeffect_params_loaded()))
    return 0;

  OGIeffectParams *prm = get_OGIeffect_params();

  short *s = wave->values().memory();
  int sz = wave->num_samples();
  int d = prm->echo.dtime;
  float tap = prm->echo.w;
  for (k = 0; k < sz; k++){
    if ((k-d > 0) && (k-d < sz)){
      s[k] += (short) (tap*s[k-d]);
    }
  }

  return clip;   // clip check not implemented yet

}

int slapback(EST_Wave *wave){
  int k;
  int clip=0;
  
  if (!(OGIeffect_params_loaded()))
    return 0;
  
  OGIeffectParams *prm = get_OGIeffect_params();

  short *y = wave->values().memory();
  int sz = wave->num_samples();

  // create temp copy of input
  short *x = walloc(short, sz);
  memcpy(x,y,sz*sizeof(short));

  int d = prm->slapback.dtime;
  float tap = prm->slapback.w;

  for (k = 0; k < sz; k++){
    if ((k-d > 0) && (k-d < sz)){
      y[k] += (short) (tap*x[k-d]);
    }
  }

  wfree(x);

  return clip;   // clip check not implemented yet

}
  

void resize_wave_smartly(EST_Wave *wave){

  int k,newsz;
  int max_revtime=0;
  int max_indx = 0;

  if (!(OGIeffect_params_loaded()))
    return;
  
  OGIeffectParams *prm = get_OGIeffect_params();

  for (k=0; k<prm->reverb.Ntaps; k++){
    if (prm->reverb.dtime[k] > max_revtime){
      max_revtime = prm->reverb.dtime[k];
      max_indx = k;
    }
  }

  // take slapback_dtime + echo_dtime * Niter_until_w^N=0.01)
  newsz = (int) (wave->num_samples() + prm->slapback.dtime 
		 + prm->echo.dtime * max(log(0.01)/log(prm->echo.w)+1,1));

  if (prm->reverb.Ntaps > 0)
    newsz += (int) (prm->reverb.dtime[max_indx] * max(log(0.01)/log(prm->reverb.w[max_indx])+1,1));
  
  /// bug workaround
  short x = 1;
  wave->values().def_val = &x;
  wave->resize(newsz,EST_ALL,1);
}

  

