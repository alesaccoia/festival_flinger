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


#include <stdio.h>    
#include <stdlib.h>

#include <memory.h>
#include "GenList.h"



#define SIZEOFPOINTER 8	//(8 bytes = 32 bits). 

/*==========================================================================*/



tstGenList *fpstMakeGenList(int inSizeOfData)	

{	// Start a new list.

	tstGenList *pl;

	// Allocate memory for the sentinal node. This only contains two pointers.

	pl = (tstGenList *)malloc(2 * SIZEOFPOINTER);

	//Set both pointers to the sentinal (ie the list is empty.)

	pl->pstHead = pl;

	pl->pstTail = pl;

	pl->inSizeOfData = inSizeOfData;

	pl->pstData = NULL;

	// return a pointer to the sentinal.

	return pl;

}



/*==========================================================================*/



void fvdAddAtHead(tstGenList *pstItem, tstGenList *pstThisList)

{
	tstGenList *pstOldHead, *pstNewHead;

	pstOldHead = pstThisList->pstHead;



	// Create storage space for the new node.

	pstNewHead = (tstGenList *)malloc(2 * SIZEOFPOINTER + 

												pstThisList->inSizeOfData);

	// Treat it as a structure that has tstGenList characteristics (but with

	// additional space for the data.

	// Load it with the item's data.

	pstNewHead->pstData = (tstGenList *)( (char *)(pstNewHead) + 

														2 * SIZEOFPOINTER );

	memcpy( pstNewHead->pstData, pstItem, pstThisList->inSizeOfData);

	// Set it's pointers so that it becomes the new head.

	pstNewHead->pstPreviousAddedItem = pstOldHead;

	pstNewHead->pstNextAddedItem = pstThisList;

	// Reset the old head's back pointer.

	pstOldHead->pstNextAddedItem = pstNewHead;

	// Reset the sentinal's head pointer.

	pstThisList->pstHead = pstNewHead;

}



/*==========================================================================*/



tstGenList *fpstGetFromHead( tstGenList *pstItem, tstGenList *pstThisList )

{

	tstGenList *pToReturn;



	if( pstThisList->pstHead != pstThisList)

	{

		if( pstThisList->pstHead->pstData != NULL )

		{

			memcpy(pstItem, pstThisList->pstHead->pstData, 

												pstThisList->inSizeOfData);

			pToReturn = pstThisList;

		}

		else

			pToReturn = NULL;

	}

	else

		pToReturn = NULL;



	return pToReturn;

}



/*==========================================================================*/



tstGenList *fpstGetFromTail( tstGenList *pstItem, tstGenList *pstThisList )

{

	tstGenList *pToReturn;



	if( pstThisList->pstTail != pstThisList)

	{

		if( pstThisList->pstTail->pstData != NULL )

		{

			memcpy(pstItem, pstThisList->pstTail->pstData, 

												pstThisList->inSizeOfData);

			pToReturn = pstThisList;

		}

		else

			pToReturn = NULL;

	}

	else

		pToReturn = NULL;



	return pToReturn;

}



/*==========================================================================*/



void fvdDeleteFromTail(tstGenList *pstThisList)

{

	tstGenList *pstOldTail, *pstNewTail;

	// Get a pointer to the current tail.

	pstOldTail = pstThisList->pstTail;

	// Get a pointer to the node added after the current tail. (This node will 

	// become the new tail.)

	pstNewTail = pstOldTail->pstNextAddedItem; 

	

	// Reset the forward pointer in what will be the new tail.

	pstNewTail->pstPreviousAddedItem = pstThisList;

	

	// Reset the tail pointer in the sentinal.

	pstThisList->pstTail = pstNewTail;

	// free the memory allocated to the old tail.

	free((void *)pstOldTail);

}



/*==========================================================================*/


void fvdDestroyList(tstGenList **ppstThisList)
{
	tstGenList *pstThisList;
	pstThisList = *ppstThisList;
	if(pstThisList != NULL)
	{
		tstGenList stTemp;
		do
		{
			if(fpstGetFromTail(&stTemp,pstThisList) == NULL)
			{
				free(pstThisList);
				*ppstThisList = NULL;
				break;
			}
			fvdDeleteFromTail(pstThisList);
		}while(true);
	}
}

/*==========================================================================*/


