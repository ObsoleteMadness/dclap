/*   ncbimisc.h
* ===========================================================================
*
*                            PUBLIC DOMAIN NOTICE                          
*               National Center for Biotechnology Information
*                                                                          
*  This software/database is a "United States Government Work" under the
*  terms of the United States Copyright Act.  It was written as part of
*  the author's official duties as a United States Government employee and 
*  thus cannot be copyrighted.  This software/database is freely available 
*  to the public for use. The National Library of Medicine and the U.S.
*  Government have not placed any restriction on its use or reproduction.
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the NLM and the U.S.
*  Government do not and cannot warrant the performance or results that
*  may be obtained by using this software or data. The NLM and the U.S.
*  Government disclaim all warranties, express or implied, including
*  warranties of performance, merchantability or fitness for any particular
*  purpose.
*
*  Please cite the author in any work or product based on this material.
*
* ===========================================================================
*
* File Name:  ncbimisc.h
*
* Author:  Gish, Kans, Ostell, Schuler
*
* Version Creation Date:   10/23/91
*
* $Revision: 2.11 $
*
* File Description: 
*   	prototypes of miscellaneous functions
*
* Modifications:
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 04-15-93 Schuler     Changed _cdecl to LIBCALL
* 06-15-93 Schuler     SGML functions moved to ncbisgml.c
* 06-15-93 Schuler     Added macros for Gestalt functins.
*
* ==========================================================================
*/

#ifndef _NCBIMISC_
#define _NCBIMISC_

#ifdef __cplusplus
extern "C" {
#endif

/* Obtain length of ASCII representation of signed and unsigned long integers */
/* opts&1 ==> use commas before thousands, millions, etc. when |value| >=10000*/
/* opts&2 ==> use commas before thousands, millions, etc. when |value| >=1000 */
/* opts&4 ==> prepend + sign to any positive values */
int LIBCALL Nlm_Lwidth PROTO((long x, int opts));
int LIBCALL Nlm_Ulwidth PROTO((unsigned long x, int opts));

/* convert integers to ASCII in static storage */
/* Same 1,2,4 opts as Nlm_Lwidth and Nlm_Ulwidth */
char * LIBCALL Nlm_Ltostr PROTO((long x, int opts));
char * LIBCALL Nlm_Ultostr PROTO((unsigned long x, int opts));

/* Sorting */
void LIBCALL Nlm_HeapSort PROTO((VoidPtr base, size_t nel, size_t width, int (LIBCALLBACK *cmp) (VoidPtr, VoidPtr) ));


/*****************************************************************************
*
*   DataVal = a universal data type
*   ValNode = a linked list of DataVal
*
*****************************************************************************/


typedef union dataval { 
	Nlm_VoidPtr ptrvalue;
	Nlm_Int4 intvalue;
	Nlm_FloatHi realvalue;
	Nlm_Boolean boolvalue;
}	DataVal, PNTR DataValPtr;

typedef struct valnode {     
	Nlm_Uint1 choice;            /* to pick a choice */
	DataVal data;            /* attached data */
	struct valnode PNTR next;   /* next in linked list */
} ValNode, PNTR ValNodePtr;

/*****************************************************************************
*
*   ValNodeNew(vnp)
*      adds after last node in list if vnp not NULL
*   
*   ValNodeAdd(head)
*      adds after last node in list if *head not NULL
*      If *head is NULL, sets it to the new ValNode
*      returns pointer to the NEW node added
*
*   ValNodeLink(head, newnode)
*      adds newnode at end of chain
*      if (*head == NULL) *head = newnode
*      ALWAYS returns pointer to START of chain
*
*   ValNodeAddStr (head, choice, str)
*      adds like ValNodeAdd()
*      sets newnode->choice = choice (if choice does not matter, use 0)
*      sets newnode->data.ptrvalue = str
*         does NOT copy str
*      if str == NULL, does NOT add a ValNode
*
*   ValNodeCopyStr (head, choice, str)
*      adds like ValNodeAdd()
*      sets newnode->choice = choice (if choice does not matter, use 0)
*      sets newnode->data.ptrvalue = str
*         makes a COPY of str
*      if str == NULL, does NOT add a ValNode
*   
*   ValNodeAddInt (head, choice, value)
*      adds like ValNodeAdd()
*      sets newnode->choice = choice (if choice does not matter, use 0)
*      sets newnode->data.intvalue = value
*
*   ValNodeAddBoolean (head, choice, value)
*      adds like ValNodeAdd()
*      sets newnode->choice = choice (if choice does not matter, use 0)
*      sets newnode->data.boolvalue = value
*
*   ValNodeAddFloat (head, choice, value)
*      adds like ValNodeAdd()
*      sets newnode->choice = choice (if choice does not matter, use 0)
*      sets newnode->data.realvalue = value
*
*   ValNodeAddPointer (head, choice, value)
*      adds like ValNodeAdd()
*      sets newnode->choice = choice (if choice does not matter, use 0)
*      sets newnode->data.realvalue = value
*
*   ValNodeFree(vnp)
*   	frees whole chain of ValNodes
*       Does NOT free associated data pointers
*
*   ValNodeFreeData(vnp)
*   	frees whole chain of ValNodes
*       frees associated data pointers - BEWARE of this if these are not
*           allocated single memory block structures.
*
*   ValNodePtr ValNodeExtract(headptr, choice)
*       removes first node in chain where ->choice == choice
*       rejoins chain after removing the node
*       sets node->next to NULL
*
*   ValNodePtr ValNodeExtractList(headptr, choice)
*       removes ALL nodes in chain where ->choice == choice
*       rejoins chain after removing the nodes
*       returns independent chain of extracted nodes
*
*   ValNodeFindNext (head, curr, choice)
*   	Finds next ValNode with vnp->choice == choice after curr
*       If curr == NULL, starts at head of list
*       If choice < 0 , returns all ValNodes
*       Returns NULL, when no more found
*   
*****************************************************************************/
ValNodePtr LIBCALL ValNodeNew PROTO((ValNodePtr vnp));
ValNodePtr LIBCALL ValNodeAdd PROTO((ValNodePtr PNTR head));
ValNodePtr LIBCALL ValNodeLink PROTO((ValNodePtr PNTR head, ValNodePtr newnode));
ValNodePtr LIBCALL ValNodeAddStr PROTO((ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_CharPtr str));
ValNodePtr LIBCALL ValNodeCopyStr PROTO((ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_CharPtr str));
ValNodePtr LIBCALL ValNodeAddInt PROTO((ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_Int4 value));
ValNodePtr LIBCALL ValNodeAddBoolean PROTO((ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_Boolean value));
ValNodePtr LIBCALL ValNodeAddFloat PROTO((ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_FloatHi value));
ValNodePtr LIBCALL ValNodeAddPointer PROTO((ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_VoidPtr value));
ValNodePtr LIBCALL ValNodeFree PROTO((ValNodePtr vnp));
ValNodePtr LIBCALL ValNodeFreeData PROTO((ValNodePtr vnp));
ValNodePtr LIBCALL ValNodeExtract PROTO((ValNodePtr PNTR headptr, Nlm_Int2 choice));
ValNodePtr LIBCALL ValNodeExtractList PROTO((ValNodePtr PNTR headptr, Nlm_Int2 choice));
ValNodePtr LIBCALL ValNodeFindNext PROTO((ValNodePtr head, ValNodePtr curr, Nlm_Int2 choice));

/*** old prototypes ******
ValNodePtr LIBCALL ValNodeLink PROTO((ValNodePtr vnp, ValNodePtr newnode));
ValNodePtr LIBCALL ValNodeExtract PROTO((ValNodePtr PNTR headptr, Nlm_Uint1 choice));
**************************/

ValNodePtr  LIBCALL NodeListNew PROTO((void));
ValNodePtr  LIBCALL NodeListFree PROTO((ValNodePtr head));
Nlm_Int2    LIBCALL NodeListLen PROTO((ValNodePtr head));
ValNodePtr  LIBCALL NodeListFind PROTO((ValNodePtr head, Nlm_Int2 item, Nlm_Boolean extend));
Nlm_Boolean LIBCALL NodeListRead PROTO((ValNodePtr head, Nlm_Int2 item, Nlm_VoidPtr ptr, size_t size));
Nlm_Boolean LIBCALL NodeListWrite PROTO((ValNodePtr head, Nlm_Int2 item, Nlm_VoidPtr ptr, size_t size));
Nlm_Boolean LIBCALL NodeListAppend PROTO((ValNodePtr head, Nlm_VoidPtr ptr, size_t size));
Nlm_Boolean LIBCALL NodeListInsert PROTO((ValNodePtr head, Nlm_Int2 item, Nlm_VoidPtr ptr, size_t size));
Nlm_Boolean LIBCALL NodeListReplace PROTO((ValNodePtr head, Nlm_Int2 item, Nlm_VoidPtr ptr, size_t size));
Nlm_Boolean LIBCALL NodeListDelete PROTO((ValNodePtr head, Nlm_Int2 item));


    /****** Choice is a compact variant of ValNode **********/

typedef union _IntPnt_ {
    Int4 intvalue;
    Pointer ptrvalue;
} IntPnt;

typedef struct _Choice_ {
    Uint1 choice;
    IntPnt value;
} Choice, PNTR ChoicePtr;


#define Lwidth	Nlm_Lwidth
#define Ulwidth	Nlm_Ulwidth
#define Ltostr	Nlm_Ltostr
#define Ultostr	Nlm_Ultostr
#define HeapSort	Nlm_HeapSort


/* Error codes for the CTX_NCBIMISC context */

#ifdef __cplusplus
}
#endif

#endif /* !_NCBIMISC_ */
