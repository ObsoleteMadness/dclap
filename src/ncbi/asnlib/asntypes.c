/* asntypes.c
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
* File Name: asntypes.c
*
* Author:  James Ostell
*
* Version Creation Date: 3/4/91
*
* $Revision: 2.12 $
*
* File Description:
*   Routines to deal with internal operations on AsnType objects.
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 3/4/91   Kans        Stricter typecasting for GNU C and C++
* 04-20-93 Schuler     LIBCALL calling convention
* 02-24-94 Schuler     Make AsnTypeStringToHex LIBCALL too
*
* ==========================================================================
*/

/*****************************************************************************
*
*   asntypes.c
*   	basic routines for processing asn1 types
*   	does not support lexxing of type definitions - those are in
*   		asnlext.c
*
*****************************************************************************/

#include "asnbuild.h"

/*****************************************************************************
*
*   This stores all loaded parse trees and links them together
*
*****************************************************************************/
static ValNodePtr
    amps ,                /* the AsnModulePtr blocks */
    names ;               /* the load file names */

/*****************************************************************************
*
*   CharPtr AsnFindPrimName(atp)
*   	returns the string name of an ASN.1 primitive type
*
*****************************************************************************/
CharPtr LIBCALL  AsnFindPrimName (AsnTypePtr atp)
{
	AsnTypePtr atp2;

	atp2 = AsnFindBaseType(atp);
	if (atp2 == NULL)
		return NULL;

	return atp2->type->name;
}

/*****************************************************************************
*
*   CharPtr AsnFindBaseName(atp)
*   	returns the string name of an ASN.1 basic entity
*       returns NULL if not resolved
*       returns NULL if resolves to a primitive type (use AsnFindPrimName)
*
*****************************************************************************/
CharPtr LIBCALL  AsnFindBaseName (AsnTypePtr atp)
{
	AsnTypePtr atp2;

	atp2 = AsnFindBaseType(atp);
	if (atp2 == NULL)
		return NULL;

	if (atp2->name == NULL)
		return NULL;
	else if (IS_UPPER(*atp2->name))
		return atp2->name;
	else
		return NULL;
}

/*****************************************************************************
*
*   void AsnKillValue(atp, dvp)
*   	eliminates stored values such as strings
*
*****************************************************************************/
void LIBCALL  AsnKillValue (AsnTypePtr atp, DataValPtr dvp)
{
	Int2 isa;

	isa = AsnFindBaseIsa(atp);
	if (ISA_STRINGTYPE(isa))
		MemFree(dvp->ptrvalue);
	else if ((isa == OCTETS_TYPE) || (isa == STRSTORE_TYPE))
		BSFree((ByteStorePtr) dvp->ptrvalue);
	return;
}

/*****************************************************************************
*
*   void AsnTypeSetIndent(increase, aip, atp)
*   	set the typestack type_indent
*
*****************************************************************************/
void AsnTypeSetIndent (Boolean increase, AsnIoPtr aip, AsnTypePtr atp)
{
	Int1 curr_indent;
	PstackPtr tmptype;
	

	if (increase)
	{
		aip->typestack[aip->type_indent].type = atp;     /* save type */
		aip->type_indent++;
		curr_indent = aip->type_indent;
		if (curr_indent == aip->max_type)   /* expand indent levels */
		{
			tmptype = aip->typestack;
			aip->typestack = (PstackPtr) MemNew((sizeof(Pstack) * (aip->max_type + 10)));
			MemCopy(aip->typestack, tmptype, (size_t)(sizeof(Pstack) * aip->max_type));
			MemFree(tmptype);
			aip->max_type += 10;
		}
		aip->typestack[curr_indent].type = NULL;     /* set to first time */
	}
	else
	{
		do
		{
			aip->typestack[aip->type_indent].type = NULL;
			if (aip->type_indent)
				aip->type_indent--;
		} while ((aip->type_indent) &&
			(AsnFindBaseIsa(aip->typestack[aip->type_indent-1].type) == CHOICE_TYPE));
		/* this is to pop out of choice nests */
	}
	return;
}

/*****************************************************************************
*
*   AsnTypePtr AsnFindBaseType(atp)
*   	find the primitive type for atp
*
*****************************************************************************/
AsnTypePtr LIBCALL AsnFindBaseType (AsnTypePtr atp)
{
	if (atp == NULL) return NULL;

	if (atp->type == NULL) return NULL;

	while (! ISA_BASETYPE(atp->type->isa))
	{
		atp = atp->type;
		if (atp->type == NULL)   /* not found */
			return NULL;
	}
	return atp;
}

/*****************************************************************************
*
*   Int2 AsnFindBaseIsa(atp)
*   	find the primitive type for atp
*
*****************************************************************************/
Int2 LIBCALL AsnFindBaseIsa (AsnTypePtr atp)
{
	if (atp == NULL)
		return -1;

	if (ISA_BASETYPE(atp->isa))
		return atp->isa;

	if (atp->type == NULL)
		return -1;

	while (! ISA_BASETYPE(atp->type->isa))
	{
		atp = atp->type;
		if (atp->type == NULL)
			return -1;
	}
	return atp->type->isa;
}

/*****************************************************************************
*
*   Boolean AsnTypeValidateOut(aip, atp, dvp)
*   	if not open or closing a SET, SEQ, SETOF, SEQOF, will
*   		put atp in aip->typestack if it validates.
*
*   KNOWN DEFECT:  SET should check for 1 of each non-optional element
*   	It does not right now, it only checks that an element is a valid
*   	member of the SET.
*
*****************************************************************************/
Boolean AsnTypeValidateOut (AsnIoPtr aip, AsnTypePtr atp, DataValPtr dvp)
{
	Int2 isa;
	AsnTypePtr atp2, parent_type, base_type, curr_type;
	Boolean foundit;
	AsnValxNodePtr avnp;

	if ((aip == NULL) || (atp == NULL))
		return FALSE;
		
	curr_type = aip->typestack[aip->type_indent].type;
	base_type = AsnFindBaseType(atp);
	if (base_type == NULL)
	{
		AsnIoErrorMsg(aip, 10, atp->name, "AsnTypeValidateOut");
		return FALSE;
	}
		
	isa = base_type->type->isa;

	if (((isa == SEQ_TYPE) || (isa == SET_TYPE) ||
		 (isa == SEQOF_TYPE) || (isa == SETOF_TYPE))
		 && (dvp->intvalue == END_STRUCT))
	{
		switch (isa)
		{
			case SEQOF_TYPE:
			case SETOF_TYPE:
				break;
			case SEQ_TYPE:       /* check that no more waiting */
				if (curr_type != NULL)   /* check next one */
					atp2 = curr_type->next;
				else                     /* empty sequence written */
					atp2 = base_type->branch;
				while (atp2 != NULL)
				{
					if (! (atp2->optional || atp2->hasdefault))
					{
						AsnIoErrorMsg(aip, 7, atp2->name, atp->name);
						return FALSE;
					}
					atp2 = atp2->next;
				}
				break;
			default:
				break;
		}
		return TRUE;
	}

	if (aip->type_indent)       /* part of a larger struct */
	{
		foundit = FALSE;
		if (aip->type_indent < 1)
			return FALSE;
			
		parent_type = aip->typestack[aip->type_indent - 1].type;
		base_type = AsnFindBaseType(parent_type);
		if (base_type == NULL)
			return FALSE;
		isa = base_type->type->isa;
		atp2 = (AsnTypePtr) base_type->branch;
		while (atp2 != NULL)
		{
			if (atp2 == atp)
			{
				foundit = TRUE;
				break;
			}
			atp2 = atp2->next;
		}
		if (! foundit)
		{
			atp2 = AsnFindBaseType(atp);
			AsnIoErrorMsg(aip, 22, atp->name, atp2->name, parent_type->name);
			return FALSE;
		}
		switch (isa)
		{
			case SETOF_TYPE:       /* just has to be right type */
			case SEQOF_TYPE:
				break;
			case CHOICE_TYPE:      /* can only have one value */
				if (curr_type != NULL)
				{
					AsnIoErrorMsg(aip, 23, parent_type->name, atp->name);
					return FALSE;
				}
				break;
			case SET_TYPE:         /* SET should check for 1 of each */
				                   /* non-optional element, but doesn't */
				if (curr_type == atp)
				{
					AsnIoErrorMsg(aip, 24, atp->name, parent_type->name);
					return FALSE;
				}
				break;
			case SEQ_TYPE:
				if (curr_type == atp)
				{
					AsnIoErrorMsg(aip, 24, atp->name, parent_type->name);
					return FALSE;
				}
				 				 /* check preceeding elements */
				if (curr_type != NULL)
				{
					atp2 = (AsnTypePtr) base_type->branch;
					while (atp2 != curr_type->next)
					{
						if (atp == atp2)
						{
							AsnIoErrorMsg(aip, 6, atp->name, parent_type->name);
							return FALSE;
						}
						atp2 = atp2->next;
					}
				}
				else
					atp2 = (AsnTypePtr) base_type->branch;
				while ((atp2 != NULL) && (atp != atp2))
				{
					if (! (atp2->optional || atp2->hasdefault))
					{					/* skipped a non-optional element */
						AsnIoErrorMsg(aip, 7, atp2->name, parent_type->name);
						return FALSE;
					}
					atp2 = atp2->next;
				}
				if (atp2 == NULL)  /* element out of order */
				{
					AsnIoErrorMsg(aip, 6, atp->name, parent_type->name);
					return FALSE;
				}
				break;
			default:
				AsnIoErrorMsg(aip, 25, parent_type->name, parent_type->isa);
				return FALSE;
		}
		base_type = AsnFindBaseType(atp);
		isa = base_type->type->isa;
	}

	if (ISA_STRINGTYPE(isa))
		isa = GENERALSTRING_TYPE;

				/******************* maintain typestack ****************/
	switch (isa)          /* set aip->typestack for non-struct types */
	{					  /* this is done for them in AsnTypeSetIndent() */
		case SEQ_TYPE:
		case SET_TYPE:
		case SEQOF_TYPE:
		case SETOF_TYPE:
		case CHOICE_TYPE:
		default:				   /* terminal value */
			aip->typestack[aip->type_indent].type = atp;
			break;
	}

	switch (isa)          /* check ranges and values */
	{
		case ENUM_TYPE:
			avnp = (AsnValxNodePtr) base_type->branch;
			while (avnp != NULL)
			{
				if (avnp->intvalue == dvp->intvalue)
					return TRUE;
				avnp = avnp->next;
			}
			AsnIoErrorMsg(aip, 12, dvp->intvalue, atp->name);
			return FALSE;
		default :
			break;
	}
	return TRUE;	
}

/*****************************************************************************
*
*   AsnTypePathFind(amp, str, countptr)
*   	like AsnTypeFind() but allocates an array of AsnTypePtr and fills
*       it in with the proper path.  Sets countptr to sizeof array.
*       Returns array or NULL on failure
*
*****************************************************************************/
AsnTypePtr PNTR LIBCALL  AsnTypePathFind (AsnModulePtr amp, CharPtr str, Int2Ptr countptr)
{
	AsnModulePtr amp2;
	AsnTypePtr atp;
	Int2 count;
	AsnTypePtr PNTR typeptr;
	CharPtr ptr;

	*countptr = 0;
	count = 1;
	ptr = str;
	while (*ptr != '\0')
	{
		if (*ptr == '.')
			count++;
		ptr++;
	}
	typeptr = MemNew(sizeof(AsnTypePtr) * count);

    if (amp == NULL)
	{
		if (amps == NULL)
		{
			AsnIoErrorMsg(NULL, (Int2)105);
			return NULL;
		}
        amp = (AsnModulePtr) amps->data.ptrvalue;
	}

	amp2 = amp;
	atp = NULL;

	while (amp2 != NULL)
	{
		atp = AsnTypeFindType(amp2->types, str, typeptr, count, FALSE);
		if (atp != NULL)
		{
			*countptr = count;
			return typeptr;
		}
		else
			amp2 = amp2->next;
	}

	MemFree(typeptr);
	return NULL;       /* not found */
}


/*****************************************************************************
*
*   AsnTypePtr AsnTypeFind(amp, str)
*   	finds a type in a module or set of modules
*   	str is a path with dot separation.
*   	E is used to indicate an element of SEQOF or SETOF
*   	looks for partial paths as well
*
*   if called with amp == NULL
*       searches all loaded modules
*   returns a single node at the end of the path
*
*****************************************************************************/
AsnTypePtr LIBCALL  AsnTypeFind (AsnModulePtr amp, CharPtr str)
{
	AsnModulePtr amp2;
	AsnTypePtr atp;

    if (amp == NULL)
	{
		if (amps == NULL)
		{
			AsnIoErrorMsg(NULL, (Int2)105);
			return NULL;
		}
        amp = (AsnModulePtr) amps->data.ptrvalue;
	}

	amp2 = amp;
	atp = NULL;

	while (amp2 != NULL)
	{
		atp = AsnTypeFindType(amp2->types, str, NULL, 0, FALSE);
		if (atp != NULL)
			return atp;
		else
			amp2 = amp2->next;
	}

	return atp;       /* not found */
}

/*****************************************************************************
*
*   AsnTypePtr AsnTypeFindType(atp, str, typeptr, count, in_it)
*
*****************************************************************************/
AsnTypePtr AsnTypeFindType (AsnTypePtr atp, CharPtr str, AsnTypePtr PNTR typeptr, Int2 count, Boolean in_it)
{
	AsnTypePtr curr, next;
	CharPtr beg;
	Int2 len, isa;
	Boolean is_element,       /* (unamed) element of setof seqof */
		is_type,              /* if a type, must find the original def */
		got_it;
	AsnTypePtr PNTR typenext;
	Int2 newcount;

	if (count < 0)      /* off array ... error condition */
		return NULL;

	beg = str;
	len = 0;
	if (typeptr == NULL)
	{
		typenext = NULL;
		newcount = 0;
	}
	else
	{
		newcount = count - 1;
		if (newcount >= 0)
			typenext = typeptr + 1;
		else
			typenext = NULL;
	}
	is_element = FALSE;
	is_type = FALSE;
	while ((*beg != '.') && (*beg != '\0'))
	{
		beg++; len++;
	}

	if ((*str == 'E') && (len == 1))
		is_element = TRUE;
	else if (IS_UPPER(*str))
		is_type = TRUE;

	curr = atp;			  	/* look at this level */
	while (curr != NULL)
	{
		got_it = FALSE;
		if (is_element)
			got_it = TRUE;
		else if (StrMatch(curr->name, str, len))   /* found it */
		{
			if (! is_type)
				got_it = TRUE;
			else                    /* check that this is the real def */
			{
				if (! curr->imported)
					got_it = TRUE;
			}
		}

		if (got_it)
		{
			if (typeptr != NULL)
				*typeptr = curr;

			if (*beg == '\0')    /* that's it */
				return curr;
			else                 /* go down the path */
			{
				next = AsnFindBaseType(curr);
				isa = next->type->isa;
				if ((next->branch != NULL)   /* go further */
					&& (isa != INTEGER_TYPE)
					&& (isa != ENUM_TYPE))
				{
					next = AsnTypeFindType((AsnTypePtr) next->branch, (beg + 1), typenext, newcount, TRUE);
					if (next != NULL)
						return next;
				}
			}
		}
		curr = curr->next;
	}
	
	if (in_it)     /* processing a path and can't find next node here */
		return NULL;

	curr = atp;           /* start down a level */
	while (curr != NULL)
	{
		if (curr->branch != NULL)
		{
			next = AsnFindBaseType(curr);
			
			if ((next != NULL) && (next->type != NULL))
			{
				isa = next->type->isa;
				if ((next->branch != NULL)   /* go further */
					&& (isa != INTEGER_TYPE)
					&& (isa != ENUM_TYPE))
				{
					next = AsnTypeFindType((AsnTypePtr) next->branch, str, typenext, newcount, FALSE);
					if (next != NULL)
					{
						if (typeptr != NULL)
							*typeptr = curr;
						return next;
					}
				}
			}
			/** else, an error has occurred, unresolved branch **/
		}
		curr = curr->next;
	}
	return NULL;
}

/*****************************************************************************
*
*   CharPtr AsnTypeDumpStack(str, aip)
*   	dump a typestack into str
*       returns a pointer to the '\0' at the end of the str
*
*****************************************************************************/
CharPtr LIBCALL  AsnTypeDumpStack (CharPtr str, AsnIoPtr aip)
{
    Int2 level, isa, lastnochoice;

    lastnochoice = -1;
    *str = '\0';

	for(level = 0; level <= aip->type_indent; level++)
	{
		if (level == aip->type_indent)
		{
			if ((aip->type & ASNIO_OUT) ||
				((aip->type & ASNIO_IN) && (aip->read_id == FALSE)))
				str = StringMove(str, "<");
		}
		if (aip->typestack[level].type == NULL)
			str = StringMove(str, " ");
		else
		{
			isa = AsnFindBaseIsa(aip->typestack[level].type);
            if (aip->typestack[level].type->name != NULL)
                str = StringMove(str, aip->typestack[level].type->name);
            else if ((lastnochoice == SEQOF_TYPE) ||
                        (lastnochoice == SETOF_TYPE))
                str = StringMove(str, "E");
            else
                str = StringMove(str, "?");
            if (isa != CHOICE_TYPE)
                lastnochoice = isa;
		}
        if (level != aip->type_indent)
            str = StringMove(str, ".");
		else
		{
			if ((aip->type & ASNIO_OUT) ||
				((aip->type & ASNIO_IN) && (aip->read_id == FALSE)))
				str = StringMove(str, ">");
		}
	}
	return str;
}

/*****************************************************************************
*
*   Boolean AsnTypeStringToHex(from, len, to)
*   	converts an octet string to binary
*   	returns TRUE if ok
*
*****************************************************************************/
Boolean LIBCALL AsnTypeStringToHex (Pointer from, Int4 len, Pointer to)
{
	BytePtr f, t;
	Byte octet, value;
	int i;

	f = (BytePtr) from;
	t = (BytePtr) to;

	while (len)
	{
		octet = 0;
		for (i = 16; i >= 1; i -= 15)
		{
			value = TO_UPPER(*f);
			if ((value >= 'A') && (value <= 'F'))
				octet += (((value - 'A') + 10) * i);
			else if ((value >= '0') && (value <= '9'))
				octet += ((value - '0') * i);
			else
				return FALSE;
			f++;
		}
		*t = octet;
		t++; len -= 2;
		if (len < 0)
			return FALSE;
	}
	return TRUE;
}

static Int2 tstack_max;
static AsnTypePtr PNTR tstack;
static Int2Ptr prev;
/*****************************************************************************
*
*   AsnLinkType(type, localtype)
*      provides for linkage of imported types
*      if (type != NULL)
*         it's an element of type localtype
*         so, set type->type to localtype
*      else
*         it's localtype itself
*         so, return localtype
*      return pointer to correct type
*
*****************************************************************************/
AsnTypePtr LIBCALL  AsnLinkType (AsnTypePtr type, AsnTypePtr localtype)
{
    Int2 i, j;
    Int2Ptr oldprev;
    AsnTypePtr PNTR oldtstack;

	if (type == NULL)
		return localtype;
	else
	{
        for (i = 0; i < tstack_max; i++)
		{
	        if (tstack[i] == NULL)
                break;
		}

        if (i >= tstack_max)   /* increase stack size */
        {
            oldtstack = tstack;
            oldprev = prev;
            tstack = MemNew(((tstack_max + 20) * sizeof(AsnTypePtr)));
            prev = MemNew(((tstack_max + 20) * sizeof(Int2)));
            MemCopy(tstack, oldtstack, (size_t)(tstack_max * sizeof(AsnTypePtr)));
            MemCopy(prev, oldprev, (size_t)(tstack_max * sizeof(Int2)));
            MemFree(oldtstack);
            MemFree(oldprev);
            tstack_max += 20;
        }

        j = type->tmp;
        if (j)    /* previous type linked */
            prev[i] = j;
        tstack[i] = type->type;
        type->tmp =  (i + 1);
		if (type != localtype)   /* don't link to itself */
			type->type = localtype;
		return type;
	}
}

/*****************************************************************************
*
*   AsnUnlinkType(type)
*     disconnects local tree.
*
*****************************************************************************/
void LIBCALL  AsnUnlinkType (AsnTypePtr type)
{
    Int2 j;

	if (type != NULL)
	{
        j = type->tmp;
        j--;
		type->type = tstack[j];
        if (prev[j])
            type->tmp = prev[j];
        else
    		type->tmp = 0;
        tstack[j] = NULL;
        prev[j] = 0;
	}
	return;
}

/*****************************************************************************
*
*   void AsnModuleLink(amp)
*   	links IMPORT types with EXPORTS from other modules
*   	if cannot link an IMPORT, no errors are generated
*
*****************************************************************************/
void LIBCALL  AsnModuleLink (AsnModulePtr amp)
{
	AsnTypePtr atp, atp2;
	AsnModulePtr currmod, mod;
	Boolean found;

	currmod = amp;
	while (currmod != NULL)
	{
		atp = currmod->types;
		while (atp != NULL)
		{
			if ((atp->imported) && (atp->type == NULL)) /* link imported types */
			{
				mod = amp;
				if (mod == currmod)
					mod = mod->next;
				found = FALSE;
				while ((mod != NULL) && (! found))
				{
					atp2 = mod->types;
					while ((atp2 != NULL) && (! found))
					{
						if ((! atp2->imported) &&
                        	(! StringCmp(atp2->name, atp->name)))
						{
							if (atp2->exported)
							{
								atp->type = atp2;
								found = TRUE;
							}
							else
							{
								AsnIoErrorMsg(NULL, 84,	atp2->name, mod->modulename);
								atp2 = atp2->next;
							}
						}
						else
							atp2 = atp2->next;
					}
					mod = mod->next;
					if (mod == currmod)
						mod = mod->next;
				}
			}
			atp = atp->next;
		}
		currmod = currmod->next;
	}
	return;
}

/*****************************************************************************
*
*   AsnStoreTree(file, amp)
*
*****************************************************************************/
static void AsnStoreTree (CharPtr file, AsnModulePtr amp)
{
    ValNodePtr anp, last;
    AsnModulePtr tamp, tamp2, newamp;
    Int2 numamp;

                    /* check to see if we already have it (could happen) */

    anp = names;
    last = NULL;
    while (anp != NULL)
    {
        if (! StringCmp((CharPtr)anp->data.ptrvalue, file))
            return;   /* have it */
        last = anp;
        anp = anp->next;
    }

                    /* add the name */
    anp = ValNodeNew(last);
    if (last == NULL)
        names = anp;
    anp->data.ptrvalue = (Pointer) StringSave(file);

    last = NULL;
    anp = amps;
    while (anp != NULL)
    {
        last = anp;
        anp = anp->next;
    }
    anp = ValNodeNew(last);
    if (last == NULL)
        amps = anp;

    tamp = amp;
    numamp = 0;
    while (tamp != NULL)
    {
        numamp++;
        tamp = tamp->next;
    }
    newamp = MemNew(numamp * sizeof(AsnModule));
    anp->data.ptrvalue = (Pointer) newamp;
    tamp = amp;
    tamp2 = newamp;
    while (numamp)
    {
        MemCopy(tamp2, tamp, sizeof(AsnModule));
        tamp2++; tamp++; numamp--;
    }
    if (last != NULL)
    {
        tamp = (AsnModulePtr)last->data.ptrvalue;
        while (tamp->next != NULL)
            tamp = tamp->next;
        tamp->next = newamp;    /* link it in to list copy */
    }

    tamp = (AsnModulePtr) amps->data.ptrvalue;
    AsnModuleLink(tamp);              /* connect the tree */
    return;
}

extern Pointer PointerDecode PROTO((Int2 index, AsnValxNodePtr avn, AsnTypePtr at));
/*****************************************************************************
*
*   AsnTreeLoad(file, &avn, &at, &amp)
*
*****************************************************************************/
Boolean LIBCALL  AsnTreeLoad (char *file, AsnValxNodePtr *avnptr, AsnTypePtr *atptr, AsnModulePtr *ampptr)
{
    AsnValxNodePtr avn = NULL, avncurr;
    AsnTypePtr at = NULL, atcurr;
    AsnModulePtr amp = NULL, ampcurr;
    ValNodePtr anp, ampanp;
    int numval, numtype, nummod, i;
    FILE * fp;
    int isa, next;
    long intvalue;
    double realvalue;
    char name[PATH_MAX], fname[60];
    CharPtr ptr;
    int bools[5], tagclass, tagnumber, defaultvalue, type, branch;

                    /* check to see if we already have it (could happen) */

    anp = names;
    ampanp = amps;
    while (anp != NULL)
    {
        if (! StringCmp((CharPtr)anp->data.ptrvalue, file))
        {                               /* have it */
            if (* ampptr == NULL)       /* fill dynamic load */
            {
                * ampptr = (AsnModulePtr) ampanp->data.ptrvalue;
                * atptr = (* ampptr)->types;
                     /* don't really need avnptr filled in */
            }
            return TRUE;
        }
        ampanp = ampanp->next;
        anp = anp->next;
    }

	if (! ProgMon("Load ASN.1 Trees"))
		return FALSE;

    if (* ampptr != NULL)      /* static load, add to list */
    {
        AsnStoreTree(file, (* ampptr));
        return TRUE;        /* already loaded */
    }

    if (! FindPath("ncbi", "ncbi", "asnload", name, sizeof (name)))
	{
		AsnIoErrorMsg(NULL, 85);
        return FALSE;
	}

    FileBuildPath (name, NULL, file);/* add file to path */

    fp = FileOpen(name, "r");
    if (fp == NULL)
    {
        AsnIoErrorMsg(NULL, 83, (CharPtr) name);
        return FALSE;
    }

    fscanf(fp, "%d %d %d", &numval, &numtype, &nummod);

    if (numval)
        avn = MemNew(numval * sizeof(AsnValxNode));
    at = MemNew(numtype * sizeof(AsnType));
    amp = MemNew(nummod * sizeof(AsnModule));

    *avnptr = avn;
    *atptr = at;
    *ampptr = amp;

    avncurr = avn;
    for (i = 0; i < numval; i++)
    {
        fscanf(fp, "%d %s %ld %lf %d", &isa, name, &intvalue, &realvalue, &next);
        avncurr->valueisa = isa;
        if (*name != '-')
            avncurr->name = StringSave(name);
        avncurr->intvalue = (Int4)intvalue;
        avncurr->realvalue = (FloatHi)realvalue;
        avncurr->next = (AsnValxNodePtr) PointerDecode((Int2)next, avn, at);
        avncurr++;
    }

    atcurr = at;
    for (i = 0; i < numtype; i++)
    {
        fscanf(fp, "%d %s %d %d %d %d %d %d %d %d %d %d %d",
            &isa, name, &tagclass, &tagnumber, bools, bools+1, bools+2,
            bools+3, bools+4, &defaultvalue, &type, &branch, &next);
        atcurr->isa = isa;
        if (*name != '-')
        {
            atcurr->name = StringSave(name);
            ptr = atcurr->name;
            while (*ptr != '\0')
            {
                if (*ptr == '!')
                    *ptr = ' ';
                ptr++;
            }
        }
        atcurr->tagclass = (Uint1)tagclass;
        atcurr->tagnumber = tagnumber;
        atcurr->implicit = (Boolean)bools[0];
        atcurr->optional = (Boolean)bools[1];
        atcurr->hasdefault = (Boolean)bools[2];
        atcurr->exported = (Boolean)bools[3];
        atcurr->imported = (Boolean)bools[4];
        atcurr->defaultvalue = (AsnValxNodePtr)PointerDecode((Int2)defaultvalue, avn, at);
        atcurr->type = (AsnTypePtr)PointerDecode((Int2)type, avn, at);
        atcurr->branch = PointerDecode((Int2)branch, avn, at);
        atcurr->next = (AsnTypePtr)PointerDecode((Int2)next, avn,at);
        atcurr++;
    }

    ampcurr = amp;
    for (i = 0; i < nummod; i++)
    {
        fscanf(fp, "%s %s %d %d", name, fname, &type, &next);
        if (*name != '-')
            ampcurr->modulename = StringSave(name);
		if (*fname != '-')
			ampcurr->filename = StringSave(fname);
        ampcurr->types = (AsnTypePtr)PointerDecode((Int2)type, avn, at);
        if (next != -32000)
            ampcurr->next = &amp[next];
        ampcurr++;
    }

    FileClose(fp);
                                  /* store it in list */
    AsnStoreTree(file, (* ampptr));

    return TRUE;
}

/*****************************************************************************
*
*   Pointer PointerDecode(index, avn, at)
*
*****************************************************************************/
Pointer PointerDecode (Int2 index, AsnValxNodePtr avn, AsnTypePtr at)
{
    if (index == -32000)
        return NULL;
    if (index == -32001)
        return (Pointer) avn;
    if (index < 0)
    {
        index *= -1;
        return (Pointer) &avn[index];
    }
    return (Pointer) &at[index];
}

/*****************************************************************************
*
*   AsnEnumStr(type, val)
*
*****************************************************************************/
CharPtr LIBCALL  AsnEnumStr (CharPtr type, Int2 val)
{
    AsnTypePtr atp;

	if (amps == NULL)
	{
		AsnIoErrorMsg(NULL, (Int2)105);
		return NULL;
	}
    atp = AsnTypeFind((AsnModulePtr)amps->data.ptrvalue, type);
    return AsnEnumTypeStr(atp, val);
}

/*****************************************************************************
*
*   AsnEnumTypeStr(atp, val)
*       returns string for ennumerated type val
*
*****************************************************************************/
CharPtr LIBCALL  AsnEnumTypeStr (AsnTypePtr atp, Int2 val)
{
    AsnValxNodePtr avnp;

    atp = AsnFindBaseType(atp);
    if (atp == NULL)
        return NULL;

    if (atp->type->isa != ENUM_TYPE)
        return NULL;

	avnp = (AsnValxNodePtr) atp->branch;
	while (avnp != NULL)
	{
		if (val == (Int2) avnp->intvalue)
            return avnp->name;
		else
			avnp = avnp->next;
	}
    return NULL;
}

/*****************************************************************************
*
*   AsnAllModPtr()
*      returns a pointer to the start of all modules in memory
*
*****************************************************************************/
AsnModulePtr LIBCALL  AsnAllModPtr (void)
{
    if (amps == NULL)
        return NULL;
    return (AsnModulePtr)amps->data.ptrvalue;
}


