#include <stdlib.h>
#include "EST.h"
#include "OGIeffect.h"
#include "effect_io.h"
#include "OGI_Macro.h"
 

int reverb(EST_Wave *wave){

  int k,t;
  double *branchreg;
  double acc,b,r,reg1;
  int d;
  int sz = wave->num_samples();
  int clip=0;

  if (!(OGIeffect_params_loaded()))
    return 0;

  OGIeffectParams *prm = get_OGIeffect_params();
  short *y  = wave->values().memory();

  branchreg = wcalloc(double, prm->reverb.Ntaps);

  for (k=0; k<sz; k++){
    acc = 0.0;
    for (t=0; t<prm->reverb.Ntaps; t++){
      d = prm->reverb.dtime[t];
      r = prm->reverb.r[t];
      if ((k - d > 0) && (k - d < sz)){
	b = prm->reverb.w[t]*(1-r);
	reg1 = b * y[k - d];
	branchreg[t] = reg1 + r * branchreg[t];
	acc += branchreg[t];
      }
    }
    if (fabs(acc) > (double) MAX_SHORT)
      clip = 1;
    
    y[k] = (short) clip(y[k]+acc);
  }

  wfree(branchreg);

  return clip;   // clip check not implemented yet

}
