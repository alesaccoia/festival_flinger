/****************************<--CSLU-->************************************/
/*                                                                        */
/*             Center for Spoken Language Understanding                   */
/*               Division of Biomedical Computer Science                  */
/*               Oregon Health and Science University                     */
/*                         Portland, OR USA                               */
/*                        Copyright (c) 1999 - 2009                       */
/*                                                                        */
/*      This module is not part of the CSTR/University of Edinburgh       */
/*               release of the Festival TTS system.                      */
/*                                                                        */
/*  In addition to any conditions disclaimers below, please see the file  */
/*  "license_ohsu_tts.txt" distributed with this software for information */
/*  on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  */
/*                                                                        */
/*      Authors: Mike Macon, Esther Klabbers, Alexander Kain              */
/*                          and David Brown                               */
/*                                                                        */
/****************************<--CSLU-->************************************/


typedef struct {
  unsigned red : 1 ;
  unsigned internal : 1 ;
  unsigned left : 1 ;
  unsigned root : 1 ;
  unsigned head : 1 ;
} status;

typedef struct rb_node {
  union {
    struct {
      struct rb_node *flink;
      struct rb_node *blink;
    } list;
    struct {
      struct rb_node *left;
      struct rb_node *right;
    } child;
  } c;
  union {
    struct rb_node *parent;
    struct rb_node *root;
  } p;
  status s;
  union {
    unsigned long ukey;
    int ikey;
    char *key;
    struct rb_node *lext;
  } k;
  union {
    char *val;
    struct rb_node *rext;
  } v;
} *Rb_node;

#ifndef __P
#if defined(__STDC__) || defined(__cplusplus)
#define __P(protos)	protos
#else
#define __P(protos)	()
#endif
#endif

#ifdef __cplusplus
#define TCLMIDI_EXTERN extern "C"
#else
#define TCLMIDI_EXTERN extern
#endif

TCLMIDI_EXTERN Rb_node make_rb __P(());
TCLMIDI_EXTERN Rb_node rb_insert_b __P((Rb_node node, char *key, char *value));

TCLMIDI_EXTERN Rb_node rb_find_key __P((Rb_node tree, char *key));
TCLMIDI_EXTERN Rb_node rb_find_ikey __P((Rb_node tree, int ikey));
TCLMIDI_EXTERN Rb_node rb_find_ukey __P((Rb_node tree, unsigned long ukey));
TCLMIDI_EXTERN Rb_node rb_find_gkey __P((Rb_node tree, char *key, int (*fxn)(char *key1, char *key2)));

TCLMIDI_EXTERN Rb_node rb_find_key_n __P((Rb_node tree, char *key, int *found));
TCLMIDI_EXTERN Rb_node rb_find_ikey_n __P((Rb_node tree, int ikey, int *found));
TCLMIDI_EXTERN Rb_node rb_find_ukey_n __P((Rb_node tree, unsigned long ukey,
    int *found));
//DJLB added params to (*fxn)()
TCLMIDI_EXTERN Rb_node rb_find_gkey_n __P((Rb_node tree, char *key, int (*fxn)(char*, char*),
    int *found));
TCLMIDI_EXTERN void rb_delete_node __P((Rb_node node));
TCLMIDI_EXTERN void rb_free_tree __P((Rb_node node));  /* Deletes and frees an entire tree */
TCLMIDI_EXTERN char *rb_val __P((Rb_node node));  /* Returns node->v.val
					     (this is to shut lint up */

#define rb_insert_a(n, k, v) rb_insert_b(n->c.list.flink, k, v)
#define rb_insert(t, k, v) rb_insert_b(rb_find_key(t, k), k, v)
#define rb_inserti(t, k, v) rb_insert_b(rb_find_ikey(t, k), (char *) k, v)
#define rb_insertu(t, k, v) rb_insert_b(rb_find_ukey(t, k), (char *) k, v)
#define rb_insertg(t, k, v, f) rb_insert_b(rb_find_gkey(t, k, f), k, v)
#define rb_first(n) (n->c.list.flink)
#define rb_last(n) (n->c.list.blink)
#define rb_next(n) (n->c.list.flink)
#define rb_prev(n) (n->c.list.blink)
#define rb_empty(t) (t->c.list.flink == t)
#ifndef nil
#define nil(t) (t)
#endif

#define rb_traverse(ptr, lst) \
  for(ptr = rb_first(lst); ptr != nil(lst); ptr = rb_next(ptr))

TCLMIDI_EXTERN void recolor __P((Rb_node));
TCLMIDI_EXTERN void single_rotate __P((Rb_node y, int l));
