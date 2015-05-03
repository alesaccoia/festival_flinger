#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

#ifndef MAX_SHORT
#define MAX_SHORT 32767
#endif

enum t_effect_order {doREVERB=1, doECHO, doSLAPBACK};

typedef struct {
  // this will change once I get a more sophisticated reverb model
  int Ntaps;
  int *dtime;  // tap echo times
  float *w;    // tap weights
  float *r;    // tap channel LPF's (reflectance of walls)
  
} t_reverb;

typedef struct {
  int dtime;
  float w;
} t_echo;

typedef struct {
  int dtime;
  float w;
} t_slapback;


typedef struct {
  float pre_gain;  // gain to apply before effects
  float post_gain;  // gain to apply after

  t_reverb reverb;
  t_echo echo;
  t_slapback slapback;
  t_effect_order *effect_order;
  int Neffect;

} OGIeffectParams;
 
