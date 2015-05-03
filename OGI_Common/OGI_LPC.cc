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

#include "OGI_LPC.h"

static int set_vector_channel_names(EST_Track &a, int N, EST_String base, int offset);

void OGI_LSF_to_LPC::convert(
           OGI_VectDouble &lsfs,
           OGI_VectDouble &lpcs
           ){
  int j;
  double *P, *Q, *Ptmp;

  P = LSF_P;
  Q = LSF_Q;
  Ptmp = LSF_Ptmp;

  /* build P(z) from root angles */
  pconv(lsfs.a, order/2, Ptmp);

  /* factor in z=1 root */
  for (j=1; j<=order; j++)
    P[j] = Ptmp[j] - Ptmp[j-1];
  P[0] = 1.0;

  /* build Q(z) from root angles */
  pconv(&(lsfs.a[order/2]), order/2, Ptmp);
  for (j=1; j<=order; j++)  /* factor in z=-1 root */
    Q[j] = Ptmp[j] + Ptmp[j-1];
  Q[0] = 1.0;

  /* Add P(z) and Q(z), divide by 2 */
  for (j=0; j<=order; j++)
    lpcs.a[j] = (*P++ + *Q++)/2.0;

  // remove later
  //  check_order_lsf(lsfs.N);
  //  check_order_lpc(lpcs.N);

}

// overloaded definition for EST_DVector arguments
void OGI_LSF_to_LPC::convert(EST_FVector &lsfs_in, EST_FVector &lpcs_in){
  int j;
  double *P, *Q, *Ptmp;

  P = LSF_P;
  Q = LSF_Q;
  Ptmp = LSF_Ptmp;

  // convert to double
  EST_DVector lpcs(lpcs_in.length()), lsfs(lsfs_in.length());
  for (int i=0; i<lsfs_in.length(); i++)
    lsfs.a_no_check(i) = (double)(lsfs_in.a_no_check(i));

  /* build P(z) from root angles */
  pconv(lsfs.memory(), order/2, Ptmp);

  /* factor in z=1 root */
  for (j=1; j<=order; j++)
    P[j] = Ptmp[j] - Ptmp[j-1];
  P[0] = 1.0;

  /* build Q(z) from root angles */
  pconv(&(lsfs[order/2]), order/2, Ptmp);
  for (j=1; j<=order; j++)  /* factor in z=-1 root */
    Q[j] = Ptmp[j] + Ptmp[j-1];
  Q[0] = 1.0;

  /* Add P(z) and Q(z), divide by 2 */
  for (j=0; j<=order; j++)
    lpcs[j] = (*P++ + *Q++)/2.0;

  // remove later
  //  check_order_lsf(lsfs.N);
  //  check_order_lpc(lpcs.N);

  // convert to float
  for (int i=0; i<lpcs_in.length(); i++)
    lpcs_in.a_no_check(i) = (float)(lpcs.a_no_check(i));
}


/************************************************************
** pconv - polynomial convolution from root input vector
**     (creates P(z) and Q(z) polynomials)
*************************************************************/
void  OGI_LSF_to_LPC::pconv( double *root_angles,
                   int Nra,
                   double *poly){

  int k,m,n;
  double *ptmp;
  int current_ord=2;
  double p_in[3];

  ptmp = PCONV_ptmp;  // use static mem
  memset(ptmp, 0, (Nra*2+1)*sizeof(double));

  ptmp[0] = 1.0;
  ptmp[1] = -2*cos(root_angles[0]);
  ptmp[2] = 1.0;

  for (k=1; k<Nra; k++){
    current_ord += 2;

    p_in[0] = 1.0;
    p_in[1] = -2*cos(root_angles[k]);
    p_in[2] = 1.0;

    for (n=0; n <= current_ord; n++){
      poly[n] = 0.0;
      for (m=0; m<3; m++){
        if ((n-m >= 0) && (n-m <= current_ord-2))
          poly[n] += p_in[m]*ptmp[n-m];
      }
    }
    for (n=0; n <= current_ord; n++)
      ptmp[n] = poly[n];
  }
}

/************************************************************
**  LPC_TO_LSF -
**     pass the LENGTH of the LPC vector - this is the LPC
**     order plus 1.  Must pre-allocate lsfs to length+1
************************************************************/
int OGI_LPC_to_LSF::convert(
           OGI_VectDouble &lpcs,
           OGI_VectDouble &lsfs){

  int ret1,ret2;
  int j,cnt=0;
  lsfs.clear();
  double *lsf_ans = lsfs.a;

  lpcs.a[0] = 1.0;

  // already allocated memory
  fcomplex *pqa     = P_pqa;
  fcomplex *pqroots = P_pqroots;
  double *r_ang = P_angs;

  /* P(z) */
  pqa[0].r = 1.;
  pqa[order+1].r = -1.;
  pqa[0].i = pqa[order+1].i = 0.;
  for(j=1; j<=order; j++) {
    pqa[j].r =  lpcs.a[j] - lpcs.a[order+1-j];
    pqa[j].i = 0.;
  }
  ret1 = zroots_sorted_angles(pqa,order+1,pqroots,r_ang);       /* find the roots */

  for(j=order/2-1; j>=0; j--){
    lsf_ans[cnt] = r_ang[j];

    cnt++;
  }

  /* Q(z) */
  pqa[0].r = 1.;
  pqa[order+1].r = 1.;
  pqa[0].i = pqa[order+1].i = 0.;
  for(j=1; j<=order; j++) {
    pqa[j].r = lpcs.a[order+1-j] + lpcs.a[j];
    pqa[j].i = 0.;
  }
  ret2 = zroots_sorted_angles(pqa,order+1,pqroots,r_ang);      /* find the roots */

  for(j=order/2-1; j>=0; j--){
    lsf_ans[cnt] = r_ang[j];

    cnt++;
  }

  return (ret1 && ret2);

}

// overloaded definition for EST_FVector arguments
int OGI_LPC_to_LSF::convert(EST_FVector &lpcs_in, EST_FVector &lsfs_in){

  // convert to double
  EST_DVector lpcs(lpcs_in.length()), lsfs(lsfs_in.length());
  for (int i=0; i<lpcs_in.length(); i++)
    lpcs.a_no_check(i) = (double)(lpcs_in.a_no_check(i));

  int ret1,ret2;
  int j,cnt=0;
  int N = lpcs.length();

  if (lsfs.length()+1 != N) {
    cerr << "OGI_LPC_to_LSF::convert : lsfs.length()+1 " << lsfs.length()+1 << " != lpcs.length() " << lpcs.length() << endl;
    lsfs.resize(N-1);
  }
  lpcs[0] = 1.0;

  // already allocated memory
  fcomplex *pqa     = P_pqa;
  fcomplex *pqroots = P_pqroots;
  double *r_ang = P_angs;

  /* P(z) */
  pqa[0].r = 1.;
  pqa[order+1].r = -1.;
  pqa[0].i = pqa[order+1].i = 0.;
  for(j=1; j<=order; j++) {
    pqa[j].r =  lpcs[j] - lpcs[order+1-j];
    pqa[j].i = 0.;
  }
  ret1 = zroots_sorted_angles(pqa,order+1,pqroots,r_ang);       /* find the roots */

  for(j=order/2-1; j>=0; j--){
    lsfs[cnt] = r_ang[j];

    cnt++;
  }

  /* Q(z) */
  pqa[0].r = 1.;
  pqa[order+1].r = 1.;
  pqa[0].i = pqa[order+1].i = 0.;
  for(j=1; j<=order; j++) {
    pqa[j].r = lpcs[order+1-j] + lpcs[j];
    pqa[j].i = 0.;
  }
  ret2 = zroots_sorted_angles(pqa,order+1,pqroots,r_ang);      /* find the roots */

  for(j=order/2-1; j>=0; j--){
    lsfs[cnt] = r_ang[j];

    cnt++;
  }

  // convert to float
  for (int i=0; i<lsfs_in.length(); i++)
    lsfs_in.a_no_check(i) = (float)(lsfs.a_no_check(i));

  return (ret1 && ret2);
}

int OGI_LPC_to_LSF::convert(OGI_VectFloat &flpc, OGI_VectFloat &flsf) {
  OGI_VectDouble dlpc(flpc.N);
  OGI_VectDouble dlsf(flpc.N-1);

  f2d_convert(flpc, dlpc);
  int ret = convert(dlpc, dlsf);
  d2f_convert(dlsf, flsf);
  return ret;
}

void OGI_LSF_to_LPC::convert(
			     OGI_VectFloat &flsf,
			     OGI_VectFloat &flpc){
  OGI_VectDouble dlsf(flsf.N);
  OGI_VectDouble dlpc(flsf.N+1);

  f2d_convert(flsf, dlsf);
  convert(dlsf, dlpc);
  d2f_convert(dlpc, flpc);
}

void LSF_to_LPC_convert(EST_Track &lsfs, EST_Track &lpcs){
  // NOTE: this assumes format like the output of LPC_to_LSF_convert:
  //    channels = {LSF_0...LSF_N E}
  // used in resLPC_analysis
  int k=0;
  OGI_LSF_to_LPC toLPC(lsfs.num_channels());
  OGI_VectFloat flpc(lsfs.num_channels()+1);
  OGI_VectFloat flsf(lsfs.num_channels());
  OGI_VectFloat tmplsf(lsfs.num_channels()+1);

  lpcs.resize(lsfs.num_frames(), lsfs.num_channels());

  for (k=0; k< lsfs.num_frames(); k++){
    lpcs.t(k) = lsfs.t(k);
    lsfs.copy_frame_out(k, tmplsf.a);
    memcpy(flsf.a, tmplsf.a, flsf.N*sizeof(float)); // get rid of E

    toLPC.convert(flsf, flpc);
    lpcs.copy_frame_in(k, flpc.a);
  }

  // copy over 0th channel with E
  lpcs.set_channel_name("E", 0);

  // copy in energy values
  int Echan = lsfs.channel_position("E");
  for (k=0; k< lsfs.num_frames(); k++){
    lpcs.a(k, 0) = lsfs.a(k,Echan);
  }
}

void OGI_LPC_bwexp::expand(OGI_VectDouble &lpcs){
  if (lpcs.N > order+1)
    EST_error("OGI_LPC_bwexp::expand - order disagrees \n");

  for (int k=1; k<lpcs.N; k++)
    lpcs.a[k] *= BWvect[k];
}


void OGI_LPC_bwexp::expand(OGI_VectFloat &flpc){
  OGI_VectDouble dlpc(flpc.N);

  f2d_convert(flpc, dlpc);
  expand(dlpc);
  d2f_convert(dlpc, flpc);
}


void LPC_to_LSF_convert(EST_Track &lpcs, EST_Track &lsfs){
  // NOTE: this assumes that the LPC order is lpcs.num_channels()-1
  //       and that the energy is in channel 0 of lpcs
  //
  // used in resLPC_analysis
  int k,ret=0;
  OGI_LPC_to_LSF toLSF(lpcs.num_channels()-1);
  OGI_VectFloat flpc(lpcs.num_channels());
  OGI_VectFloat flsf(lpcs.num_channels()-1);
  OGI_VectFloat prev_good_lsf(lpcs.num_channels()-1);
  prev_good_lsf.clear();

  lsfs.resize(lpcs.num_frames(), lpcs.num_channels()-1);
  for (k=0; k< lpcs.num_frames(); k++){
    lsfs.t(k) = lpcs.t(k);
    lpcs.copy_frame_out(k, flpc.a);

    ret = toLSF.convert(flpc, flsf);

    if (ret==0){  // LPC root solver didn't converge!
      //      cout << "did not converge at frame " << k << endl;
      lsfs.copy_frame_in(k, prev_good_lsf.a);
    }
    else {
      lsfs.copy_frame_in(k, flsf.a);
      memcpy(prev_good_lsf.a, flsf.a, flsf.N*sizeof(float));
    }
  }

  // name LSF channels
  set_vector_channel_names(lsfs, lsfs.num_channels() , "LSF_", 0);

  // copy in energy values
  lsfs.set_num_channels(lsfs.num_channels() + 1);
  lsfs.set_channel_name("E", lsfs.num_channels()-1);
  for (k=0; k< lpcs.num_frames(); k++){
    lsfs.a(k, "E") = lpcs.a(k,0);
  }
}


static int set_vector_channel_names(EST_Track &a, int N, EST_String base, int offset){

  int k;
  EST_String name;

  for (k=0; k<N-1; k++){
    name = base + itoString(k);
    a.set_channel_name(name, k+offset);
  }
  name = base + "N";
  a.set_channel_name(name, k+offset);

  return offset+N;
}
