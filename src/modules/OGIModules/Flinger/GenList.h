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

/* This is the header file for GenList.

	GenList provides linked list storage for any structure. Those structures
	do not require any elements other than their own data elements, and all
	memory allocation and de-allocation is handled by GenList.

	For clarity in the code, alternative names for the two pointers that form
	the list are provided.
	pstHead and pstTail will be used in the sentinal, whilst
	pstPreviousAddedItem and pstNextAddedItem will be used in the data nodes,
	but they are actually the same quantities, respectively.
	The size of the data to be listed is also held in the sentinal, along with
	a pointer to data (only so that the sentinal's structure type can be used
	to access the data in a node - the sentinal pointer to Data is always NULL).
	Each node consists of a structure containing the pointers specified for
	the sentinal, plus the data.

	Note that all GenLists created are identified by a pointer of the same type
	as the items to be stored.
	*/
#ifndef GENLIST_H
#define GENLIST_H


typedef struct stGenList
{
	union
	{
		stGenList *pstPreviousAddedItem;
		stGenList *pstHead;
	};
	union
	{
		stGenList *pstNextAddedItem;
		stGenList *pstTail;
	};
	stGenList *pstData;
	int inSizeOfData;
} tstGenList;


/* These are the routines for manipluating GenLists */

extern tstGenList *fpstMakeGenList(int inSizeOfStructure);
//And a convenient macro:-
#define MakeList(ItemType) ( (ItemType *)fpstMakeGenList(sizeof(ItemType)) )

/*	Usage:-
tstBoxes	*plstBoxList;
	plstBoxList = MakeList(tstBoxes));
*/

extern void fvdAddAtHead(tstGenList *pstItem, tstGenList *pstTheList);
//And a convenient macro:-
#define AddAtListHead( item, plist ) ( fvdAddAtHead((tstGenList *)&item, (tstGenList *)plist) )
/* Usage:-
	tstBoxes stBox_n;
	AddAtListHead( stBox_n, plstBoxList );
*/

extern tstGenList *fpstGetFromHead( tstGenList *pstItem, tstGenList *pstTheList);
//And a convenient macro:-
#define GetFromListHead(pitem, plist) (fpstGetFromHead((tstGenList *)pitem, (tstGenList *)plist) )
/* Usage:-
	tstBoxes stBox_n;
	GetFromListTail( &stBox_n, plstBoxList );
*/

extern tstGenList *fpstGetFromTail( tstGenList *pstItem, tstGenList *pstTheList);
//And a convenient macro:-
#define GetFromListTail(pitem, plist) (fpstGetFromTail((tstGenList *)pitem, (tstGenList *)plist) )
/* Usage:-
	tstBoxes stBox_n;
	GetFromListHead( &stBox_n, plstBoxList );
*/


extern void fvdDeleteFromTail(tstGenList *pstTheList);
//And a convenient macro:-
#define DeleteFromTail(plist) ( fvdDeleteFromTail((tstGenList *) plist) )
/* Usage:-
	DeleteFromTail( plstBoxList );
*/

extern void fvdDestroyList(tstGenList **pstThisList);
//And a convenient macro:-
#define DestroyList(plist) ( fvdDestroyList((tstGenList **) &plist) )
/* Usage:-
	DestroyList( plstBoxList );
*/

#endif
