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
#include "OGIdbase.h"
#include "OGIdbase_scm.h"
#include "OGIunitsel_diphone.h"

static void set_dbUnit_info(int indx, OGIdbase *db, EST_Relation *dbUnit_rel,  EST_Relation *dbSeg_rel);

LISP OGIdbase_UnitSel_Diphone(LISP utt){
  EST_Utterance *u = get_c_utt(utt);
  LISP llisp;
  EST_Item *lseg,*rseg;
  EST_String Lnm,Rnm,diphname;
  int indx;
  OGIdbase *db = NULL;

  if (db_is_loaded())
    db = get_db();
  else
    EST_error("OGIdbase_UnitSel_Diphone: no OGIdbase loaded.");

  u->create_relation("dbUnit");
  u->create_relation("dbSeg");
  EST_Relation *dbUnit_rel = u->relation("dbUnit");
  EST_Relation *dbSeg_rel = u->relation("dbUnit");

  /// loop through Segment items
  for (lseg=u->relation("Segment")->head(); lseg!=0; lseg=lseg->next()) {

    rseg = lseg->next();
    if (rseg != 0){
      
      // Left phone name
      if (lseg->f_present("allophone_name"))
	Lnm = (const EST_String&)lseg->f("allophone_name");
      else
	Lnm = lseg->name();	
      
      // Right phone name
      if (rseg->f_present("allophone_name"))
	Rnm = (const EST_String&)rseg->f("allophone_name");
      else
	Rnm = rseg->name();	
      
      // diphone unit 
      diphname = Lnm + "-" + Rnm;
      indx = db->name_lookup_hash(diphname);
      //      cout << diphname << " ---> " << indx;

      // if lookup failed, try alternates function
      if (indx < 0){
	llisp = cons(rintern("OGI_di_alt"),
		     cons(strintern(Lnm),
			  cons(strintern(Rnm),NIL)));
	llisp = leval(llisp,NIL);
	indx = get_c_int(llisp);
	//	cout << "  FAILED: backoff to " << indx;
      }
      // cout << endl;

      // Set info in dbUnit relation
      set_dbUnit_info(indx, db, dbUnit_rel, dbSeg_rel);

    }
  }    
  
  return utt;
}

static void set_dbUnit_info(int indx, 
			    OGIdbase *db,
			    EST_Relation *dbUnit_rel,
			    EST_Relation *dbSeg_rel){
  int n;
  // append a new dbUnit
  EST_Item *dbUnit = dbUnit_rel->append();
  EST_Item *seg;
  OGIdbUnits *U = &(db->unitdic);

  dbUnit->set_name(U->name(indx));
  dbUnit->set("db_indx",indx);
  dbUnit->set("Lindx",U->Lindx(indx));
  dbUnit->set("Rindx",U->Rindx(indx));
  dbUnit->set("isSegBndL", U->Lbnd(indx));
  dbUnit->set("isSegBndR", U->Rbnd(indx));
  
  // is previous unit this unit's natural neighbor?
  if (ffeature(dbUnit, "p.db_indx").Int() == U->Lindx(indx))
    dbUnit->set("isNatNbL", 1);
  else
    dbUnit->set("isNatNbL", 0);
    
  // append daughters to the dbUnit for each segment in the unit
  for (n=0; n<U->Nseg(indx); n++){
    seg = dbSeg_rel->append();
    seg->set_name(U->seg_name(indx,n));
    seg->set("dur",U->seg_dur(indx,n)/db->Fs);
    dbUnit->append_daughter(seg);
  }
}



