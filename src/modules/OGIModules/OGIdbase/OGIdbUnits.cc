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

#include "OGIdbUnits.h"

//static bool allsame(OGI_BoolVector &in);
static OGIbool allsame(EST_TVector<OGIbool> &in);
static OGIbool allsame(EST_IVector &in);
static int get_num_lines(EST_TokenStream &ts);
static void ckeoln( EST_TokenStream &ls);

OGIdbUnits::OGIdbUnits(){
  is_gLindx = FALSE;
  is_gRindx = FALSE; 
  is_gLbnd = FALSE;  
  is_gRbnd = FALSE;
  is_gsegB = FALSE; 
}
OGIdbUnits::~OGIdbUnits(){  
  cout << "delete OGIdbUnits obj\n";
  int k;
  for (k=0; k<p_seg_names.length(); k++){
    delete (EST_StrVector *)p_seg_names.a(k);
    delete (EST_IVector *)p_seg_ends.a(k); // trust there's same # of them
  }
}

void OGIdbUnits::save_grp(FILE *gfd){
  int k,Nseg_k;
  int Nun = Nunits();

  // length
  fwrite(&Nun, sizeof(int), 1, gfd);
  
  // condense() flags
  fwrite(&is_gLbnd,  sizeof(OGIbool), 1, gfd);
  fwrite(&is_gRbnd,  sizeof(OGIbool), 1, gfd);
  fwrite(&is_gLindx, sizeof(OGIbool), 1, gfd);
  fwrite(&is_gRindx, sizeof(OGIbool), 1, gfd);
  fwrite(&is_gsegB,  sizeof(OGIbool), 1, gfd);
  
  // names (writes whole StrVect)
  OGIfwrite(p_name, gfd); // OGI_file.cc
  
  // L,Rindx
  if (!(is_gLindx))
    OGIfwrite(p_Lindx, gfd);
  else 
    fwrite(&gLindx, sizeof(int), 1, gfd);
  
  if (!(is_gRindx))
    OGIfwrite(p_Rindx, gfd);
  else 
    fwrite(&gRindx, sizeof(int), 1, gfd);
  

  // seg bnd flags
  if (!(is_gLbnd))
    OGIfwrite(p_Lbnd, gfd);
  else 
    fwrite(&gLbnd, sizeof(OGIbool), 1, gfd);
  
  if (!(is_gRbnd))
    OGIfwrite(p_Rbnd, gfd);
  else 
    fwrite(&gRbnd, sizeof(OGIbool), 1, gfd);
  
  // 1st seg begin
  if (!(is_gsegB))
    OGIfwrite(p_segB, gfd);
  else 
    fwrite(&gsegB, sizeof(int), 1, gfd);

  // Nsegs, seg_names, seg_ends  (add new feats here)
  for (k=0; k<Nun; k++){
    Nseg_k = Nseg(k);
    fwrite(&Nseg_k, sizeof(int), 1, gfd);

    OGIfwrite(*((EST_StrVector *)p_seg_names(k)), gfd);
    OGIfwrite(*((EST_IVector *)p_seg_ends(k)), gfd);
  }
}


void OGIdbUnits::load_grp(FILE *gfd, OGIbool swap){
  int k,Nsegs_k;
  int N=0;

  // length
  OGIfread(&N, sizeof(int), 1, gfd, swap ? true : false);
  resize(N);
  
  // condense() flags
  OGIfread(&is_gLbnd,  sizeof(OGIbool), 1, gfd, swap ? true : false);
  OGIfread(&is_gRbnd,  sizeof(OGIbool), 1, gfd, swap ? true : false);
  OGIfread(&is_gLindx, sizeof(OGIbool), 1, gfd, swap ? true : false);
  OGIfread(&is_gRindx, sizeof(OGIbool), 1, gfd, swap ? true : false);
  OGIfread(&is_gsegB,  sizeof(OGIbool), 1, gfd, swap ? true : false);
    
  // names
  OGIfread(p_name, gfd);
  
  // L,Rindx
  if (!(is_gLindx))
    OGIfread(p_Lindx, N, gfd, swap ? true : false);
  else 
    OGIfread(&gLindx, sizeof(int), 1, gfd, swap ? true : false);

  if (!(is_gRindx))
    OGIfread(p_Rindx, N, gfd, swap ? true : false);
  else 
    OGIfread(&gRindx, sizeof(int), 1, gfd, swap ? true : false);
 

  // seg bnd flags
  if (!(is_gLbnd))
    OGIfread(p_Lbnd, N, gfd, swap ? true : false);
  else 
    OGIfread(&gLbnd, sizeof(OGIbool), 1, gfd, swap ? true : false);

  if (!(is_gLbnd))
    OGIfread(p_Rbnd, N, gfd, swap ? true : false);
  else 
    OGIfread(&gRbnd, sizeof(OGIbool), 1, gfd, swap ? true : false);


  // 1st seg begin
  if (!(is_gsegB))
    OGIfread(p_segB, N, gfd, swap ? true : false);
  else 
    OGIfread(&gsegB, sizeof(int), 1, gfd, swap ? true : false);


  // seg names, ends  (add new feats here)
  for (k=0; k<N; k++){
    OGIfread(&Nsegs_k, sizeof(int), 1, gfd, swap ? true : false);
    alloc_Nseg(k, Nsegs_k);

    OGIfread(*((EST_StrVector *)p_seg_names(k)), gfd);
    OGIfread(*((EST_IVector *)p_seg_ends(k)), Nsegs_k, gfd, swap ? true : false);
  }

  // free unused vectors
  free_condensed();

}


void OGIdbUnits::load_file(EST_String &unitdic_file, float Fs){
  //  form of each unitdic entry:
  //   name file Lindx Rindx Lbnd Rbnd segB seg_name1 seg_end1 seg_name2 seg_end2 ...
  //   name file Lindx Rindx Lbnd Rbnd segB seg_name1 seg_end1 seg_name2 seg_end2 ...
  //   name file Lindx Rindx Lbnd Rbnd segB seg_name1 seg_end1 seg_name2 seg_end2 ...
  //
  //    where
  //      Lindx, Rindx = (int)
  //      Lbnd, Rbnd   = T | F 
  //      segB         = (float msec) 
  //      seg_endN     = (float msec) 
  //
  EST_TokenStream ts;
  int i,k;
  EST_String line;
  EST_String tmpstr;
  EST_StrList seginfo;
  int Nseg,Nunits_max;

  if (ts.open(unitdic_file) == -1){
    EST_error("OGIdbase.cc: Can't open unitdic_file " + unitdic_file + "\n");
  }
  ts.set_WhiteSpaceChars(" \t\n");
  ts.set_SingleCharSymbols("");
  ts.set_PunctuationSymbols("");
  
  Nunits_max = get_num_lines(ts);
  resize(Nunits_max); 

  for (i=0; (!ts.eof()) && (i<Nunits_max);  ){
    line = (const EST_String)ts.get_upto_eoln();
    //        line = ts.get_upto_eoln();
    if ((line.length() > 0) && (line[0] != ';')){
      EST_TokenStream ls;
      ls.set_WhiteSpaceChars(" \t\n");
      ls.set_SingleCharSymbols("");
      ls.set_PunctuationSymbols("");
      
      // read in this line token by token
      ls.open_string(line);
      name(i)  = wstrdup(ls.get().string());
      file(i)  = wstrdup(ls.get().string());
      Lindx(i) = atoi(ls.get().string());
      Rindx(i) = atoi(ls.get().string());                       ckeoln(ls);
      Lbnd(i)  = (OGIbool) atobool(wstrdup(ls.get().string())); ckeoln(ls);
      Rbnd(i)  = (OGIbool) atobool(wstrdup(ls.get().string())); ckeoln(ls); 
      segB(i)  = (int) (atof(ls.get().string())/1000 * Fs);     ckeoln(ls);
	           // read in msec, converted to samples
      
      // read in the (segname, segdur) pairs
      seginfo.clear();
      while (!ls.eoln())  // get the rest of the tokens
	seginfo.append(ls.get().string());

      // put into dbUnits
      Nseg = seginfo.length()/2;
      alloc_Nseg(i,Nseg);
      for (k=0; k<Nseg; k++){
	seg_name(i,k) = seginfo.nth(2*k);
	seg_end(i,k)  = (int) (atof(seginfo.nth(2*k+1))/1000 * Fs); 
	                      // read in msec, converted to samples

	if (k > 0){
	  if (seg_end(i,k) <= seg_end(i,k-1)){
	    EST_error("OGIdbase.cc: segments out of order in unit %d (line %d of unitdic)\n", i, i+1);
	  }
	}
      }
      
      ls.close();
      i++;
    }
  }
  
  if (i != Nunits_max)    {
    EST_error("OGIdbase.cc: something wrong w/ # of lines?? i: %d, Nunits_max: %d\n",i,Nunits_max);
  }
  
  // close unitdic file token stream
  ts.close();
}

static void ckeoln( EST_TokenStream &ls){
  if (ls.eoln())
     EST_error("OGIdbase.cc: is there something wrong w/ # of fields on each line of unitdic ??\n");
}

void OGIdbUnits::print(EST_String &filename){
  int i,k;
  FILE *fp = fopen(filename, "w");

  fprintf(fp,"# num \tname\tfile    \tLindx\tRindx\tLbnd\tRbnd\tsegB\tsegname\tsegend\t...\n");
  fprintf(fp,"# --- \t----\t----    \t-----\t-----\t----\t----\t----\t-------\t-------\t---\n");

  for (i=0; i<Nunits(); i++){
    fprintf(fp,"%4d\t",i);
    fprintf(fp,"%-6s\t",name(i).str());
    fprintf(fp,"%8s\t",file(i).str());
    fprintf(fp,"%5d\t",Lindx(i));
    fprintf(fp,"%5d\t",Rindx(i));
    fprintf(fp,"%-4s\t",(Lbnd(i) ? "T" : "F"));
    fprintf(fp,"%-4s\t",(Rbnd(i) ? "T" : "F"));
    fprintf(fp,"%4d\t",segB(i));

    for (k=0; k<Nseg(i); k++){
      fprintf(fp,"%s\t",seg_name(i,k).str());
      fprintf(fp,"%d\t",seg_end(i,k));
    }
    fprintf(fp,"\n");
  }

  fclose(fp);
}


void OGIdbUnits::condense(void){
  // check for all values of a vector are same, if so, just save 1 global val
  //   (but can't add any new ones after that!)
  if (allsame(p_Lbnd)){
    gLbnd = p_Lbnd[0];
    is_gLbnd = TRUE;
  }
  if (allsame(p_Rbnd)){
    gRbnd = p_Rbnd[0];
    is_gRbnd = TRUE;
  }
  if (allsame(p_Lindx)){
    gLindx = p_Lindx[0];
    is_gLindx = TRUE;
  }
  if (allsame(p_Rindx)){
    gRindx = p_Rindx[0];
    is_gRindx = TRUE;
  }
  if (allsame(p_segB)){
    gsegB = p_segB[0];
    is_gsegB = TRUE;
  }
  free_condensed();
}

void OGIdbUnits::free_condensed(void){
  if (is_gLbnd)
    p_Lbnd.resize(0);
  if (is_gRbnd)
    p_Rbnd.resize(0);
  if (is_gLindx)
    p_Lindx.resize(0);
  if (is_gRindx)
    p_Rindx.resize(0);
  if (is_gsegB)
    p_segB.resize(0);
}

void OGIdbUnits::resize(int N){
     p_name.resize(N);
     p_Lindx.resize(N);     
     p_Rindx.resize(N);
     p_Lbnd.resize(N);     
     p_Rbnd.resize(N);
     p_segB.resize(N);
     p_seg_names.resize(N);
     p_seg_ends.resize(N);
     p_file.resize(N);
}

static OGIbool allsame(EST_TVector<OGIbool> &in){
  int k=0;
  OGIbool in0 = in[0];
  while ((k<in.length()) && (in[k]==in0))
    k++;
  if (k==in.length())
    return TRUE;
  else
    return FALSE;
}
static OGIbool allsame(EST_IVector &in){
  int k=0;
  int in0 = in[0];
  while ((k<in.length()) && (in[k]==in0))
    k++;
  if (k==in.length())
    return TRUE;
  else
    return FALSE;
}

static int get_num_lines(EST_TokenStream &ts){
  int n=0;
  while (!ts.eof()){
    ts.get_upto_eoln();
    n++;
  }
  ts.restart();
  return n;
}


/*****************************************************/
/* To avoid RE-instantiation, I've cleaned up these  */
/* templates and move them to ../OGIcommon/inst_tmpl */
/* Vector of all types of pointers are combined into */
/* EST_TVector<void *> now.                          */
/*                            -- niuxc               */
/*****************************************************/

///////////////////////////////////////////////////////////////
/// Instantiations of EST_TVector templates for various types
///////////////////////////////////////////////////////////////
//#if defined(INSTANTIATE_TEMPLATES)
//#include "../../../../speech_tools/base_class/EST_TVector.cc"
//template class EST_TVector<OGIbool>; // aec - explicit instantiate not advised
//template class EST_TVector<EST_IVector *>;
//template class EST_TVector<EST_StrVector *>;
//#endif            

//
// bool 
//
// aec - type bool is an integral type RJC already instantiates the
// integer type of EST_TVector in vector_i_t.cc 
// the following causes a naming conflict
// solutions: don't declare these or don't compile vector_i_t.cc
#if 0
template ostream& operator << (ostream &st, const EST_TVector<OGIbool> &v);
static OGIbool const def_val_OGIbool = FALSE;
OGIbool const *EST_TVector<OGIbool>::def_val = &def_val_OGIbool;
static OGIbool error_return_OGIbool = FALSE;
OGIbool *EST_TVector<OGIbool>::error_return = &error_return_OGIbool;
#endif

//
// IVector *
//
//static EST_IVector * const def_val_IVectVect = NULL;
//EST_IVector * const *EST_TVector<EST_IVector *>::def_val = &def_val_IVectVect;
//static EST_IVector *error_return_IVectVect = NULL;
//EST_IVector * *EST_TVector<EST_IVector *>::error_return = &error_return_IVectVect;
 
 
//
// StrVector *
//
//static EST_StrVector * const def_val_StrVectVect = NULL;
//EST_StrVector * const *EST_TVector<EST_StrVector *>::def_val = &def_val_StrVectVect;
//static EST_StrVector *error_return_StrVectVect = NULL;
//EST_StrVector * *EST_TVector<EST_StrVector *>::error_return = &error_return_StrVectVect;

