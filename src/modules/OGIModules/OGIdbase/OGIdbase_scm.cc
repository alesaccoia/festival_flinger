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

/*
** Interface to OGIdbase Scheme and management of
**   multiple loaded OGIdbase instances.
**
*/

#include "OGIdbase_scm.h"
#include "OGIresLPC_db.h"
#include "OGIunitsel_diphone.h"

static OGIdbase *CurrentDB = NULL;  // current dbase in use
static int tc_OGIdbase = -1;        // lisp typed cell
static LISP DBlist = NIL;            // list of all loaded dbases

static LISP OGIdbase_init(LISP params);
static LISP siod_make_OGIdbase(OGIdbase *s);
static LISP OGIdbase_activate(LISP name);
static LISP OGIdbase_group(LISP name,LISP lfilename);
static LISP OGIdbase_list(void);
static LISP OGIdbase_get_data_type(void);
static LISP OGIdbase_lookup_info(LISP lindx);
static LISP OGIdbase_unitsel_name(LISP lname);
static LISP OGIdbase_print(LISP lfile);
static void add_OGIdbase(OGIdbase *db_in);
static OGIdbase *get_c_OGIdbase(LISP x);

// OGIdbase.init
//  Top level database initialization function called from scheme
//    - call OGIdbase.init with the parameter "group_file" specified
//       to attempt to load a group file, use "group_encoding" to specify
//       raw or compressed
//    - call OGIdbase.init without "group_file" specified to load directly from
//       ungrouped data files
//
static LISP OGIdbase_init(LISP params){
  EST_String datatype, groupfile;
  OGIdbase *new_db = NULL;

  datatype = wstrdup(get_param_str("data_type",params,"unset (get_param failed)"));

	//cerr << datatype << endl;
  if ( datatype== "resLPC")  new_db = new OGIresLPC_db;
//  else if (datatype == "LPC")  new_db = new OGILPC_db;
  //else if (datatype == "sinLPC")  new_db = new OGIsine_db;
  else {
    EST_error("OGIdbase_scm.cc: Unknown data type \"" + datatype + "\"\n");
  }

  groupfile = wstrdup(get_param_str("groupfile",params,""));

  if (groupfile == "")
    new_db->load_ungrouped(params);
  else
    new_db->load_grouped(groupfile, params);

  add_OGIdbase(new_db);    // add to db list, activates it too

  return NIL;
}

// Activate speech database by name
static LISP OGIdbase_activate(LISP name){
  LISP lpair = siod_assoc_str(get_c_string(name),DBlist);

  if (lpair == NIL){
     EST_error("OGIdbase: no database named " + EST_String(get_c_string(name)) + " defined\n");
  }
  else {
    OGIdbase *db = get_c_OGIdbase(car(cdr(lpair)));
    CurrentDB = db;
  }

  return name;
}

// Save named db in a group file
static LISP OGIdbase_group(LISP name,LISP lfilename){

  OGIdbase_activate(name);  // make it current
  EST_String grp_filename(get_c_string(lfilename));
  CurrentDB->save_grouped(grp_filename);
  cerr << "finished building groupfile \n";

  return NIL;
}

// List names of currently loaded unit databases
static LISP OGIdbase_list(void){
  LISP names,n;

  for (names=NIL,n=DBlist; n != NIL; n=cdr(n))
    names = cons(car(car(n)),names);
  return reverse(names);
}

// Return data_type of currently loaded unit databases
static LISP OGIdbase_get_data_type(void){

  if (db_is_loaded())
    return CurrentDB->lispify_data_type();
  else{
    return NIL;
  }
}

// Return a LISP structure containing info about a unit
static LISP OGIdbase_lookup_info(LISP lindx){

  int indx = get_c_int(lindx);
  if (indx < CurrentDB->unitdic.Nunits())
    if (db_is_loaded())
      return CurrentDB->lispify_unit_info(indx);
    else{
      cerr << "OGIdbase_lookup_info: no database loaded\n";
    }
  else{
    EST_error("OGIdbase_lookup_info: index out of range\n");
  }
  return NIL;
}



static LISP OGIdbase_Nunits(void){
  return flocons( CurrentDB->unitdic.Nunits());
}

// Return a LISP structure containing index to unit
//  with name lname

static LISP OGIdbase_unitsel_name(LISP lname){
  int indx;
  EST_String name(get_c_string(lname));
  if (db_is_loaded()){
    //indx = CurrentDB->name_lookup(name);
    indx = CurrentDB->name_lookup_hash(name);
    if (indx >= 0)
      return flocons(indx);
  }
  else
  	//cerr << name << endl;
    cerr << "OGIdbase_unitsel_name: no database loaded\n";
  return NIL;
}

// Return index to unit with name "name"
int OGIdbase_unitsel_name(EST_String &name){
  if (db_is_loaded())
    return CurrentDB->name_lookup_hash(name);
  return -1;
}

// print CurrentDB dbUnits info to a file
static LISP OGIdbase_print(LISP lfile){

  EST_String filename(get_c_string(lfile));
  if (db_is_loaded())
    CurrentDB->unitdic.print(filename);
  else
    cerr << "OGIdbase_print: no database loaded\n";
  return NIL;
}

// Add this to list of loaded databases and activate it
static void add_OGIdbase(OGIdbase *db_in){
  LISP lpair;
  OGIdbase *old_db;

  if (DBlist == NIL){
    gc_protect(&DBlist);
    tc_OGIdbase = siod_register_user_type("OGIdbase");
  }

  // see if already have one of this name
  lpair = siod_assoc_str(db_in->name, DBlist);
  if (lpair == NIL){
    // add new database of this name
    DBlist = cons(cons(rintern(db_in->name),
		       cons(siod_make_OGIdbase(db_in),NIL)),
		  DBlist);
  }
  else{
    // already one of this name, free old one and overwrite
    cerr << "OGIdbase: warning redefining speech database "
	 << db_in->name << endl;
    old_db = get_c_OGIdbase(car(cdr(lpair)));
    delete old_db;
    setcar(cdr(lpair),siod_make_OGIdbase(db_in));
  }
  CurrentDB = db_in;  // set as current one
}


static OGIdbase *get_c_OGIdbase(LISP x){
  if (TYPEP(x,tc_OGIdbase))
    return (OGIdbase *)USERVAL(x);
  else
    err("wta to get_c_OGIdbase",x);
  return NULL;  // err doesn't return
}
static LISP siod_make_OGIdbase(OGIdbase *s){
  if (s==0)
    return NIL;
  else
    return siod_make_typed_cell(tc_OGIdbase,s);
}

// Just check to see dbase is NULL
bool db_is_loaded(void){
  if (CurrentDB == NULL)
    return(FALSE);
  else
    return(TRUE);
}

// Get ptr to currently loaded database
OGIdbase *get_db(void){
  return(CurrentDB);
}

void festival_OGIdbase_init(void){

    init_subr_1("OGIdbase.lookup_info",OGIdbase_lookup_info,
    "(OGIdbase.lookup_info INDX)\n\
  Return a LISP structure containing info about the unit referenced\n\
  by INDX. Used to create the dbUnit relation after unit selection.");

    init_subr_1("OGIdbase.unitsel_name",OGIdbase_unitsel_name,
    "(OGIdbase.unitsel_name NAME)\n\
  Return index of first unit in active database having name NAME,\n\
  or NIL if none is found with that name.");

    init_subr_1("OGIdbase.print",OGIdbase_print,
    "(OGIdbase.print FILENAME)\n\
  Print the unitdic info to FILENAME for debugging purposes.");

    init_subr_0("OGIdbase.list",OGIdbase_list,
    "(OGIdbase.list)\n\
  List the names of the currently loaded speech databases.");

    init_subr_0("OGIdbase.Nunits",OGIdbase_Nunits,
    "(OGIdbase.Nunits)\n\
  Number of units in the currently loaded speech database.");

    init_subr_0("OGIdbase.get_data_type",OGIdbase_get_data_type,
    "(OGIdbase.get_data_type)\n\
  Return the data type of the active OGIdbase (resLPC, LPC, sine, none, NIL).");

    init_subr_1("OGIdbase.activate",OGIdbase_activate,
    "(OGIdbase.activate DB_NAME)\n\
  Activate a preloaded speech database named DB_NAME.  Databases are\n\
  identified by their name parameter.");

    init_subr_1("OGIdbase.init",OGIdbase_init,
    "(OGIdbase.init PARAMS)\n\
  Initialize an OGI unit database.  PARAMS is an assoc list\n\
  of parameter name and value.");

    init_subr_2("OGIdbase.group",OGIdbase_group,
    "(OGIdbase.group DB_NAME GROUPFILE)\n\
  Create a group file for DB_NAME and save in GROUPFILE.  A group\n\
  file is a saved image of the database containing only the necessary\n\
  information.  This is the recommended format for normal use.");

    init_subr_1("OGIdbase.unitsel_diphone",OGIdbase_UnitSel_Diphone,
  "(OGIdbase.unitsel_diphone UTT)\n\
  Selects diphone units by looking at Segment relation, puts results \n\
  into dbUnit relation.");


}
