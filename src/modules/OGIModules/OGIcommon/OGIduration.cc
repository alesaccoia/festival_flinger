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

#include "OGIduration.h"

// Functions for setting up duration modification.
// Assumes we have
//     - "SrcSeg" relation with "dur" feature
//     - "Segment" relation with "end" feature
//


//
// Generate segment time correspondences (warp).
//
void make_time_warp(
		    EST_Utterance *u,
		    OGI_TimeWarp &warp){

  EST_Item *useg,*sseg;
  float src_end=0.0, targ_end=0.0, stretch=1.0;

  // Loop thru segments
  for (useg=u->relation("Segment")->head(), sseg=u->relation("SrcSeg")->head();
       ((useg!=0) && (sseg!=0));
       useg=useg->next(), sseg=sseg->next()) {
	   //cerr << useg->name() << " " << sseg->name() << "\n";



    // could do something clever here if labeling is not same
    if (sseg->name() != useg->name()){
      //      cout << "\tchanging name " << sseg->name() << " to match " << useg->name() << " " << endl;
      sseg->set_name(useg->name());
    }

    src_end  += sseg->F("dur");
    targ_end  = useg->F("end");

    warp.append(src_end, targ_end, sseg->name());

    if (useg->f_present("enh_Tstretch")){
      stretch=useg->F("enh_Tstretch")*useg->F("enh_orig_dur")/sseg->F("dur");
      if (useg->f_present("enh_Rbkpt")){     // this is phone to right of transition
	      warp.nonuniform_stretch(warp.Nseg(), stretch, useg->F("enh_Rbkpt"), 1);
      }
      else if (useg->f_present("enh_Lbkpt")){// this is phone to left of transition
	      warp.nonuniform_stretch(warp.Nseg(), stretch, useg->F("enh_Lbkpt"), 2);
      }
    }
  }

  if ((useg != 0) || (sseg != 0)) {
    cerr << "OGIduration: warning: different number of segments in utt than in selected units\n";
  }
}

//
// no time scale modification -- just use what is in Segments
//
void make_default_time_warp(
			    EST_Utterance *u,
			    OGI_TimeWarp &warp){


  EST_Item *useg,*sseg;
  float src_end=0.0;

  // Loop thru segments
  for (useg=u->relation("Segment")->head(), sseg=u->relation("SrcSeg")->head();
       ((useg!=0) && (sseg!=0));
       useg=useg->next(), sseg=sseg->next()) {

    if (sseg->name() != useg->name())
      sseg->set_name(useg->name());

    src_end  += sseg->F("dur");

    warp.append(src_end, src_end, sseg->name());
  }
}

