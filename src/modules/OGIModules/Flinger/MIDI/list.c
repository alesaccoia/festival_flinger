/* 
 * $Source: /u1/CVS_Repository/festival/src/modules/Flinger/MIDI/list.c,v $
 * $Revision: 1.1 $
 * $Date: 1999/06/22 01:12:29 $
 * $Author: macon $
 */

#include <stdio.h>    /* Basic includes and definitions */
#include <stdlib.h>
#include "list.h"

#define boolean int
#define TRUE 1
#define FALSE 0


/*---------------------------------------------------------------------*
 * PROCEDURES FOR MANIPULATING DOUBLY LINKED LISTS 
 * Each list contains a sentinal node, so that     
 * the first item in list l is l->flink.  If l is  
 * empty, then l->flink = l->blink = l.            
 * The sentinal contains extra information so that these operations
 * can work on lists of any size and type.
 * Memory management is done explicitly to avoid the slowness of
 * malloc and free.  The node size and the free list are contained
 * in the sentinal node.
 *---------------------------------------------------------------------*/

typedef struct int_list {  /* Information held in the sentinal node */
  struct int_list *flink;
  struct int_list *blink;
  int size;
  List free_list;
} *Int_list;

void insert(List item, List list)	/* Inserts to the end of a list */
{
  List last_node;

  last_node = list->blink;

  list->blink = item;
  last_node->flink = item;
  item->blink = last_node;
  item->flink = list;
}

void delete_item(List item)		/* Deletes an arbitrary iterm */
{
  item->flink->blink = item->blink;
  item->blink->flink = item->flink;
}

List make_list(int size)	/* Creates a new list */
{
  Int_list l;

  l = (Int_list) malloc(sizeof(struct int_list));
  l->flink = l;
  l->blink = l;
  l->size = size;
  l->free_list = (List) malloc (sizeof(struct list));
  l->free_list->flink = l->free_list;
  return (List) l;
}
  
List get_node(List list)   /* Allocates a node to be inserted into the list */
{
  Int_list l;
  List to_return;

  l = (Int_list) list;
  if (l->free_list->flink == l->free_list) {
    return (List) malloc(l->size);
  } else {
    to_return = l->free_list;
    l->free_list = to_return->flink;
    return to_return;
  }
}

void free_node(List node, List list)    /* Deallocates a node from the list */
{
  Int_list l;
  
  l = (Int_list) list;
  node->flink = l->free_list;
  l->free_list = node;
}
