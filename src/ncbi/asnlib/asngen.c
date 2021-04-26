/*  asngen.c
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
* File Name: asngen.c
*
* Author:  James Ostell
*
* Version Creation Date: 3/4/91
*
* $Revision: 2.6 $
*
* File Description:
*   General application interface routines for ASNlib
*      using these has the advantage that your routine will read binary
*        and print value messages with no changes
*      it has the disadvantage that you link both binary and text routines
*        whether you use them or not
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 3/4/91   Kans        Stricter typecasting for GNU C and C++
* 04-20-93 Schuler     LIBCALL calling convention
* ==========================================================================
*/

#include "asnbuild.h"

/*****************************************************************************
*
*   AsnReadId()
*   	generalized read ident
*
*****************************************************************************/
AsnTypePtr LIBCALL  AsnReadId (AsnIoPtr aip, AsnModulePtr amp, AsnTypePtr atp)

{
	if (aip->read_id)
		AsnIoErrorMsg(aip, 104, aip->linenumber);
	aip->read_id = TRUE;
	
    if (aip->type & ASNIO_TEXT)
		return AsnTxtReadId(aip, amp, atp);
    else if (aip->type & ASNIO_BIN)
		return AsnBinReadId(aip, atp);

	return NULL;
}

/*****************************************************************************
*
*   AsnReadVal()
*   	generalized readvalue
*
*****************************************************************************/
Int2 LIBCALL AsnReadVal (AsnIoPtr aip, AsnTypePtr atp, DataValPtr vp)

{
	if (! aip->read_id)
		AsnIoErrorMsg(aip, 103, aip->linenumber);
	aip->read_id = FALSE;
	
	if (aip->type & ASNIO_TEXT)
		return AsnTxtReadVal(aip, atp, vp);
	else if (aip->type & ASNIO_BIN)
		return AsnBinReadVal(aip, atp, vp);

	return -1;
}

/*****************************************************************************
*
*   AsnWrite()
*   	generalized write value
*
*****************************************************************************/
Boolean LIBCALL AsnWrite (AsnIoPtr aip, AsnTypePtr atp, DataValPtr dvp)

{
	Boolean retval = FALSE;
	
	if (aip->aeop != NULL)    /* exploring nodes */
		AsnCheckExpOpt(aip, atp, dvp);

	if (aip->type & ASNIO_TEXT)
		retval = AsnTxtWrite(aip, atp, dvp);
	else if (aip->type & ASNIO_BIN)
		retval = AsnBinWrite(aip, atp, dvp);

	if (aip->io_failure)
		return FALSE;
	return retval;
}

/*****************************************************************************
*
*   AsnSkipValue(aip, atp)
*   	Assumes that a read id has just been done
*   	Assumes all values to be encountered are linked to atp
*   	will skip values until it returns to the former level
*   	returns TRUE if no errors
*       ends with a ReadVal()
*
*****************************************************************************/
Boolean LIBCALL AsnSkipValue (AsnIoPtr aip, AsnTypePtr atp)

{
	Int2 level;

	level = AsnGetLevel(aip);
	if (! AsnReadVal(aip, atp, NULL))
		return FALSE;

	while (level < AsnGetLevel(aip))
	{
		if ((atp = AsnReadId(aip, NULL, atp)) == NULL)
			return FALSE;
		if (! AsnReadVal(aip, atp, NULL))
			return FALSE;
	}
	return TRUE;
}

/*****************************************************************************
*
*   AsnOpenStruct(aip, atp, ptr)
*      write atp as a START_STRUCT
*
*****************************************************************************/
Boolean LIBCALL AsnOpenStruct (AsnIoPtr aip, AsnTypePtr atp, Pointer ptr)

{
	DataVal av;
	Boolean retval;

	av.intvalue = START_STRUCT;
	if(aip->aeosp != NULL)
		aip->aeosp->the_struct = ptr;
	retval = AsnWrite(aip, atp, &av);
	if(aip->aeosp != NULL)
		aip->aeosp->the_struct = NULL;
	return retval;
}

/*****************************************************************************
*
*   AsnCloseStruct(aip, atp, ptr)
*      writes an END_STRUCT for atp
*
*****************************************************************************/
Boolean LIBCALL AsnCloseStruct (AsnIoPtr aip, AsnTypePtr atp, Pointer ptr)

{
	DataVal av;
    Int2 i;
	Boolean retval;

    i = aip->type_indent;
    if (! i)
	{
        AsnIoErrorMsg(aip, 20 );
		return FALSE;
	}

    i--;
    while ((i) &&
        (AsnFindBaseIsa(aip->typestack[i].type) == CHOICE_TYPE))
        i--;
    if (aip->typestack[i].type != atp)
	{
        AsnIoErrorMsg(aip, 21, atp->name, aip->typestack[i].type->name);
		return FALSE;
	}

	av.intvalue = END_STRUCT;
	if(aip->aeosp != NULL)
		aip->aeosp->the_struct = ptr;
	retval = AsnWrite(aip, atp, &av);
	if(aip->aeosp != NULL)
		aip->aeosp->the_struct = NULL;
	return retval;
}

/*****************************************************************************
*
*   AsnWriteChoice(aip, atp, the_choice, the_value)
*
*****************************************************************************/
Boolean LIBCALL AsnWriteChoice (AsnIoPtr aip, AsnTypePtr atp, Int2 the_choice, DataValPtr the_value)

{
	Boolean retval;
	DataVal av;

	if (the_value == NULL)
	{
		av.ptrvalue = NULL;
		the_value = &av;
	}
	if(aip->aeosp != NULL)
		aip->aeosp->the_choice = the_choice;
	retval = AsnWrite(aip, atp, the_value);
	if(aip->aeosp != NULL)
		aip->aeosp->the_choice = NO_CHOICE_SET;
	return retval;
}

/*****************************************************************************
*
*   Int2 AsnGetLevel(aip)
*       returns current level in parse stack
*       to traverse a struct (SEQUENCE, SET, SET OF, SEQUENCE OF)
*       atp = AsnReadId(aip, ...)    id for struct
*       level = AsnGetLevel(aip);    record level
*       AsnReadVal(aip, ...)         read open bracket to indent a level
*       while (AsnGetLevel(aip) != level)
*       {
*           AsnReadId()
*           AsnReadVal()
*       }
*       At this point you have read the close bracket for the struct
*   
*****************************************************************************/
Int2 LIBCALL AsnGetLevel (AsnIoPtr aip)

{
    return (Int2)aip->type_indent;
}

/*****************************************************************************
*
*   AsnCheckExpOpt(aip, atp, dvp)
*   	see if atp is a node we are looking for
*       call user callback if it is
*
*****************************************************************************/
void LIBCALL AsnCheckExpOpt (AsnIoPtr aip, AsnTypePtr atp, DataValPtr dvp)

{
	AsnExpOptPtr aeop;
	AsnTypePtr base;
	Boolean got_one;
	Int2 i, j, isa, isa_end_struct;

	aeop = aip->aeop;
	base = AsnFindBaseType(atp);
	while (aeop != NULL)
	{
		got_one = FALSE;
		if (aeop->numtypes == 1)    /* checking one type */
		{
			if ((aeop->types[0] == atp) ||
				(aeop->types[0] == base))
			{
				got_one = TRUE;
			}
		}
		else if (aeop->numtypes == 0)   /* checking all types */
			got_one = TRUE;
		else
		{
		  	if (aeop->types[aeop->numtypes - 1] == atp)
			{
				isa = base->type->isa;

				isa_end_struct = 0;
				if ((isa == SEQ_TYPE) || (isa == SET_TYPE) ||
					 (isa == SEQOF_TYPE) || (isa == SETOF_TYPE))
				{
					 if (dvp->intvalue != START_STRUCT)
						isa_end_struct = 1;
				}

				if ((Int2)aip->type_indent >= (aeop->numtypes - 1 - isa_end_struct))
				{
					if (isa_end_struct)
						i = aip->type_indent - 2;
					else
		 				i = aip->type_indent - 1;
		 			j = aeop->numtypes - 2;
					while ((j > 0) && (i > 0))
					{
						if (aip->typestack[i].type == aeop->types[j])
						{
							j--; i--;
						}
						else
							break;
					}
					if (! j)    /* got through it all */
					{
						if ((aip->typestack[i].type == aeop->types[0]) ||
							(AsnFindBaseType(aip->typestack[i].type) ==
								aeop->types[0]))
								got_one = TRUE;
					}
				}
			}
		}

		if (got_one)   /* found it */
		{
			aip->aeosp->atp = atp;
			aip->aeosp->dvp = dvp;
			aip->aeosp->data = aeop->user_data;
			(* aeop->user_callback)(aip->aeosp);
		}
		aeop = aeop->next;
	}
	return;
}

/*****************************************************************************
*
*   AsnExpOptNew(aip, path, ptr, func)
*
*****************************************************************************/
AsnExpOptPtr LIBCALL AsnExpOptNew (AsnIoPtr aip, CharPtr path, Pointer ptr, AsnExpOptFunc func)   /* user supplied callback */

{
	AsnExpOptPtr curr, prev;
	AsnTypePtr PNTR typeptr = NULL;
	Int2 typecount = 0;

	if (path != NULL)     /* get type array */
	{
		typeptr = AsnTypePathFind(NULL, path, &typecount);
		if (typeptr == NULL)    /* path not found */
			return NULL;
	}
	curr = MemNew(sizeof(AsnExpOpt));
	curr->user_data = ptr;
	curr->user_callback = func;
	curr->types = typeptr;
	curr->numtypes = typecount;
	if (aip->aeop == NULL)   /* first one */
	{
		aip->aeop = curr;
		aip->aeosp = MemNew(sizeof(AsnExpOptStruct));
		aip->aeosp->aip = aip;
		aip->aeosp->the_choice = NO_CHOICE_SET;
	}
	else
	{
		prev = aip->aeop;
		while (prev->next != NULL)
			prev = prev->next;
		prev->next = curr;
	}
	return curr;
}

/*****************************************************************************
*
*   AsnExpOptFree(aip, aeop)
*   	if aeop == NULL, free them all
*
*****************************************************************************/
AsnExpOptPtr LIBCALL AsnExpOptFree (AsnIoPtr aip, AsnExpOptPtr aeop)

{
	AsnExpOptPtr curr, next, prev;

	curr = aip->aeop;
	prev = NULL;
	while (curr != NULL)
	{
		if ((aeop == NULL) || (aeop == curr))
		{
			next = curr->next;
			if (prev != NULL)
				prev->next = next;
			else
				aip->aeop = next;
			MemFree(curr->types);
			MemFree(curr);
			curr = next;
		}
		else
		{
			prev = curr;
			curr = curr->next;
		}
	}
	if (prev == NULL)    /* nothing left */
		aip->aeosp = MemFree(aip->aeosp);   /* free the AsnExpOptStruct */
	return NULL;
}

/*****************************************************************************
*
*   AsnNullValueMsg(aip, node)
*   	posts an error message when a NULL value is passed to the object
*   loaders for node.
*
*****************************************************************************/
void LIBCALL AsnNullValueMsg (AsnIoPtr aip, AsnTypePtr node)

{
	char * tmpbuf;
	CharPtr str;

	tmpbuf = malloc(512);       /* tempory local buffer for mswindows */
	str = StringMove(tmpbuf, "NULL value passed to object loader for ");
	str = AsnTypeDumpStack(str, aip);
	if (node != NULL)
	{
		if (*(str - 1) == '>')    /* already read or written */
		{
			while (*str != '<')
				str--;
		}
		else
		{		
			*str = '.';
			str++;
		}
		StringMove(str, node->name);
	}
	ErrPost(CTX_NCBIOBJ, 1, tmpbuf);
	free(tmpbuf);
	return;
}



