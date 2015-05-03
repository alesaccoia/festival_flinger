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

#include "festival.h"
#include "OGIgain.h"


static EST_Item *new_gitem(EST_Relation *g, EST_Item *gitem);

void OGIgain_targ(
		  EST_Utterance *u,
		  float B,
		  float E,
		  float Bval,
		  float Eval,
		  float samp,
		  const EST_String &method){
  float t,M,v;
  float utt_end = u->relation("Segment")->tail()->F("end");
  EST_Item *gitem,  *p_gitem;
  if (!(u->relation_present("Gain"))){
    u->create_relation("Gain");
  }

  EST_Relation *grel = u->relation("Gain");

  if (grel->length() < 2){
    grel->clear();
    gitem = grel->append();
    gitem->set("pos", 0.0);
    gitem->set("gain", 1.0);
    gitem = grel->append();
    gitem->set("pos", utt_end);
    gitem->set("gain", 1.0);
  }

  // remove any in region where we will insert
  p_gitem = NULL;
  for (gitem=grel->head(); gitem!=0; gitem=gitem->next()){
    if (p_gitem){
      p_gitem->unref_all();
    }

    if ((gitem->F("pos") > 0.0) && (gitem->F("pos") >= B) && (gitem->F("pos") <= E)){
      p_gitem = gitem;
    }
    else {
      p_gitem = NULL;
    }
  }

  p_gitem = NULL;
  for (gitem=grel->head(); gitem!=0; gitem=gitem->next()){
    if (gitem->F("pos") >= B){
      break;
    }
    p_gitem = gitem;
  }
  gitem = p_gitem;

  if (method == "sin"){
    M = (float)PI/((float)2.0*(E-B));
    for (t=B; ((t<E) && (t<utt_end)); t+=samp){
      gitem = new_gitem(grel, gitem);
      gitem->set("pos", t);
      v = (Eval-Bval)*sin(M*(t-B)) + Bval;
      gitem->set("gain",v);
    }
  }
  else if (method == "cos"){
    M = (float)PI/((float)2.0*(E-B));
    for (t=B; ((t<E) && (t<utt_end)); t+=samp){
      gitem = new_gitem(grel, gitem);
      gitem->set("pos", t);
      v = (Eval-Bval)*(1 - cos(M*(t-B))) + Bval;
      gitem->set("gain",v);
    }
  }
  else if (method == "linear"){
    M = (Eval-Bval)/(E-B);
    for (t=B; ((t<E) && (t<utt_end)); t+=samp){
      gitem = new_gitem(grel, gitem);
      gitem->set("pos", t);
      v = M*(t-B) + Bval;
      gitem->set("gain",v);
    }
  }
  else {
    EST_error("OGIadd_gain_targets: don't recognize interp type %s\n",method.str());
  }
}

static EST_Item *new_gitem(EST_Relation *g, EST_Item *gitem){
  if (gitem != NULL)
    return gitem->insert_after();
  else if (g->head())
    return g->head()->insert_before();
  else {
    EST_error("new_gitem problem");
  }
  return NULL;
}
    

float seg_gain_target(
		      EST_Utterance *u,
		      EST_Item *seg,
		      int side){
  // return gain target nearest the left(side=0) or right(side=1)
  //  of seg.  Return 1.0 if no Gain targets exist.

  if ((seg == NULL) || (!(u->relation_present("Gain"))))
    return 1.0;

  EST_Item *gitem;
  float targ_out=1.0;
  float B = ffeature(seg, "start");
  float E = ffeature(seg, "end");

  for (gitem=u->relation("Gain")->head(); gitem!=0; gitem=gitem->next()){
    if (gitem->f_present("pos")){
      if (gitem->F("pos") >= B) {
	if (gitem->F("pos") <= E){
	  targ_out = gitem->F("gain");
	  if (side == 0)
	    break;
	}
      }
    }
  }
  return targ_out;
}
