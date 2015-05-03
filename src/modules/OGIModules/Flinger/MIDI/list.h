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

/* This is the header file for the list manipulation routines in list.c.
 * Any struct can be turned into a list as long as its first two fields are
 * flink and blink. */
#ifndef MIDILIST_H
#define MIDILIST_H

typedef struct list {
  struct list *flink;
  struct list *blink;
} *List;

/* Nil, first, next, and prev are macro expansions for list traversal 
 * primitives. */

#ifndef nil
#define nil(l) (l)
#endif
#define first(l) (l->flink)
#define last(l) (l->blink)
#define next(n) (n->flink)
#define prev(n) (n->blink)

#define mklist(t) ((t *) make_list (sizeof(t)))
 
/* These are the routines for manipluating lists */

/* void insert(node list);     Inserts a node to the end of a list */
/* void delete_item(node);     Deletes an arbitrary node */
/* List make_list(node_size);  Creates a new list */
/* List get_node(list);        Allocates a node to be inserted into the list */
/* void free_node(node, list); Deallocates a node from the list */

#ifndef __P
#if defined(__STDC__) || defined(__cplusplus)
#define __P(protos)	protos
#else 
#define __P(protos)	()
#endif
#endif

extern void insert __P((List, List));
extern void delete_item __P((List));
extern List make_list __P((int));
extern List get_node __P((List));
extern void free_node __P((List, List));

#endif

