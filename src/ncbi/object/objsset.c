/*  objsset.c
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
* File Name:  objsset.c
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.7 $
*
* File Description:  Object manager for module NCBI-Seqset
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 05-13-93 Schuler     All public functions are now declared LIBCALL.
*
*
* ==========================================================================
*/
#include <objsset.h>		   /* the Bioseq-set interface */
#include <sequtil.h>           /* used by SeqEntryAsnGet() */
#include <asnsset.h>        /* the AsnTool header */

static Boolean loaded = FALSE;

/*****************************************************************************
*
*   SeqSetAsnLoad()
*
*****************************************************************************/
Boolean LIBCALL SeqSetAsnLoad ()
{
    if (loaded)
        return TRUE;
    loaded = TRUE;

    if (! GeneralAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    if (! SeqAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    if (! AsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    return TRUE;
}


/*****************************************************************************
*
*   Bioseq-set Routines
*
*****************************************************************************/

/*****************************************************************************
*
*   BioseqSetNew()
*
*****************************************************************************/
BioseqSetPtr LIBCALL BioseqSetNew ()
{
    BioseqSetPtr bsp;

    bsp = (BioseqSetPtr)MemNew(sizeof(BioseqSet));
	if (bsp == NULL) return bsp;
    bsp->level = INT2_MIN;
    return bsp;
}
/*****************************************************************************
*
*   BioseqSetFree(bsp)
*       Frees one BioseqSet and associated data
*
*****************************************************************************/
BioseqSetPtr LIBCALL BioseqSetFree (BioseqSetPtr bsp)
{
    SeqAnnotPtr sp, spnext;
    SeqEntryPtr sep, sepnext;

    if (bsp == NULL)
        return bsp;

    ObjectIdFree(bsp->id);
    DbtagFree(bsp->coll);
    MemFree(bsp->release);
    DateFree(bsp->date);
    SeqDescrFree(bsp->descr);
    sep = bsp->seq_set;
    while (sep != NULL)
    {
        sepnext = sep->next;
        SeqEntryFree(sep);
        sep = sepnext;
    }
    sp = bsp->annot;
    while (sp != NULL)
    {
        spnext = sp->next;
        SeqAnnotFree(sp);
        sp = spnext;
    }
	return (BioseqSetPtr)MemFree(bsp);
}
/*****************************************************************************
*
*   BioseqSetAsnWrite(bsp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (BioseqSet ::=)
*
*****************************************************************************/
Boolean LIBCALL BioseqSetAsnWrite (BioseqSetPtr bsp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    SeqEntryPtr sep;
	Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqSetAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, BIOSEQ_SET);   /* link local tree */
	if (atp == NULL) return FALSE;

	if (bsp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

	if (! AsnOpenStruct(aip, atp, (Pointer)bsp)) goto erret;
    
    if (bsp->id != NULL)
	{
        if (! ObjectIdAsnWrite(bsp->id, aip, BIOSEQ_SET_id)) goto erret;
	}
    if (bsp->coll != NULL)
	{
        if (! DbtagAsnWrite(bsp->coll, aip, BIOSEQ_SET_coll)) goto erret;
	}
    if (bsp->level != INT2_MIN)
    {
        av.intvalue = bsp->level;
        if (! AsnWrite(aip, BIOSEQ_SET_level, &av)) goto erret;
    }
    if (bsp->_class != 0)
    {
        av.intvalue = bsp->_class;
        if (! AsnWrite(aip, BIOSEQ_SET_class, &av)) goto erret;
    }
    if (bsp->release != NULL)
    {
        av.ptrvalue = bsp->release;
        if (! AsnWrite(aip, BIOSEQ_SET_release, &av)) goto erret;
    }
    if (bsp->date != NULL)
	{
        if (! DateAsnWrite(bsp->date, aip, BIOSEQ_SET_date)) goto erret;
	}
    if (bsp->descr != NULL)              /* Seq-descr optional */
	{
        if (! SeqDescrAsnWrite(bsp->descr, aip, BIOSEQ_SET_descr)) goto erret;
	}

    if (! AsnOpenStruct(aip, BIOSEQ_SET_seq_set, (Pointer)bsp->seq_set)) goto erret;
    sep = bsp->seq_set;
    while (sep != NULL)
    {
        if (! SeqEntryAsnWrite(sep, aip, BIOSEQ_SET_seq_set_E)) goto erret;
        sep = sep->next;
    }
    if (! AsnCloseStruct(aip, BIOSEQ_SET_seq_set, (Pointer)bsp->seq_set)) goto erret;
    if (bsp->annot != NULL)              /* annotation optional */
	{
        if (! SeqAnnotSetAsnWrite(bsp->annot, aip, BIOSEQ_SET_annot, BIOSEQ_SET_annot_E)) goto erret;
	}

    if (! AsnCloseStruct(aip, atp, (Pointer)bsp)) goto erret;
	retval = TRUE;
erret:
	AsnUnlinkType(orig);        /* unlink local tree */
	return retval;
}

static void SeqDescrPack PROTO((ValNodePtr PNTR to, ValNodePtr PNTR from));

/*****************************************************************************
*
*   SeqDescrPack(to, from)
*       address of asnodeptr at head of list to add to
*       address of asnodeptr at head of list to add from.
*       sets *from to NULL on completion.
*       deletes extra titles
*****************************************************************************/
static void SeqDescrPack (ValNodePtr PNTR to, ValNodePtr PNTR from)
{
    ValNodePtr currdescr, newdescr, tmpdescr;
    Boolean have_title = FALSE;

    currdescr = *to;
    if (currdescr == NULL)   /* nothing to merge with */
    {
        *to = *from;
        *from = NULL;
        return;
    }

    while (currdescr->next != NULL)
    {
        if (currdescr->choice == Seq_descr_title)
            have_title = TRUE;
        currdescr = currdescr->next;
    }
    if (currdescr->choice == Seq_descr_title)
         have_title = TRUE;
    newdescr = *from;
    while (newdescr != NULL)
    {
        if ((have_title) && (newdescr->choice == Seq_descr_title))
        {
            tmpdescr = newdescr;
            newdescr = tmpdescr->next;
            MemFree(tmpdescr->data.ptrvalue);
            MemFree(tmpdescr);
        }
        else
        {
            currdescr->next = newdescr;
            newdescr = newdescr->next;
            currdescr = currdescr->next;
        }
    }
    *from = NULL;

    return;
}

/*****************************************************************************
*
*   BioseqSetAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
BioseqSetPtr LIBCALL BioseqSetAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    BioseqSetPtr bsp=NULL, tmp;
    SeqEntryPtr curr, next, hold = NULL;
    SeqAnnotPtr sap;
	AsnOptionPtr aop;
	Op_objssetPtr oop = NULL;
	Op_objseqPtr osp = NULL;
	Boolean this_one = TRUE,
		get_bioseq = FALSE,
		check_set = FALSE,
		got_it = FALSE,
		old_in_right_set = FALSE;

	if (! loaded)
	{
		if (! SeqSetAsnLoad())
			return bsp;
	}

	if (aip == NULL)
		return bsp;

	if ((aop = AsnIoOptionGet(aip, OP_NCBIOBJSSET, SEQENTRY_OPTION_MAX_COMPLEX, NULL)) != NULL)
	{
		this_one = FALSE;
		check_set = TRUE;
		oop = (Op_objssetPtr) aop->data.ptrvalue;
		old_in_right_set = oop->in_right_set;
		if (oop->retcode == 1)    /* get bioseq */
			get_bioseq = TRUE;
		else
		{
			if((aop = AsnIoOptionGet(aip, OP_NCBIOBJSEQ, BIOSEQ_CHECK_ID, NULL))!=NULL)
				osp = (Op_objseqPtr)aop->data.ptrvalue;
		}
	}

	if (orig == NULL)           /* BioseqSet ::= (self contained) */
		atp = AsnReadId(aip, amp, BIOSEQ_SET);
	else
		atp = AsnLinkType(orig, BIOSEQ_SET);    /* link in local tree */
    oldatp = atp;
	if (atp == NULL) return bsp;

	bsp = BioseqSetNew();
	if (bsp == NULL) goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */
    curr = NULL;

    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
		if (atp == NULL) goto erret;
        if (atp == BIOSEQ_SET_id)
		{
            bsp->id = ObjectIdAsnRead(aip, atp);
			if (bsp->id == NULL) goto erret;
		}
        else if (atp == BIOSEQ_SET_coll)
		{
            bsp->coll = DbtagAsnRead(aip, atp);
			if (bsp->coll == NULL) goto erret;
		}
        else if (atp == BIOSEQ_SET_date)
		{
            bsp->date = DateAsnRead(aip, atp);
			if (bsp->date == NULL) goto erret;
		}
        else if (atp == BIOSEQ_SET_descr)
		{
            bsp->descr = SeqDescrAsnRead(aip, atp);
			if (bsp->descr == NULL) goto erret;
		}
        else if (atp == BIOSEQ_SET_seq_set_E)
        {
			if (got_it)    /* already have the entry we want */
				AsnSkipValue(aip, atp);
            else
			{
				if ((next = SeqEntryAsnRead(aip, atp)) != NULL)
				{
					if (get_bioseq)      /* will only be here if got it */
					{
						got_it = TRUE;
						hold = next;
					}
					else if (check_set)
					{
						if (oop->working_on_set == 2)  /* found set in lower level */
							got_it = TRUE;
						else if ((oop->retcode == 2) && (! oop->in_right_set))
						{
							if (osp->found_it)   /* found the component Bioseq */
							{
								got_it = TRUE;
								oop->working_on_set = 2;   /* all done */
							}
						}

						if (got_it)
						{
							hold = next;
							curr = bsp->seq_set;      /* free sibling sets - not needed */
							while (curr != NULL)
							{
						 		next = curr->next;
								SeqEntryFree(curr);
								curr = next;
							}
							bsp->seq_set = NULL;
						}
					}

					if (! got_it)
					{
			            if (curr == NULL)
    			            bsp->seq_set = next;
        			    else
            			    curr->next = next;
	            		curr = next;
					}
				}
			}
        }
        else if (atp == BIOSEQ_SET_annot)
        {
			if ((get_bioseq) && (! got_it))
				AsnSkipValue(aip, atp);
			else
			{
	            bsp->annot = SeqAnnotSetAsnRead(aip, atp, BIOSEQ_SET_annot_E);
				if (bsp->annot == NULL) goto erret;
			}
        }
        else
        {
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* takes care of everything else */
            if (atp == BIOSEQ_SET_level)
                bsp->level = (Int2)av.intvalue;
            else if (atp == BIOSEQ_SET_class)
			{
                bsp->_class = (Uint1)av.intvalue;
				if ((! get_bioseq) && (check_set))       /* looking for a set */
				{
					switch (oop->retcode)
					{
						case 2:   /* seg-set */
							if (bsp->_class == 2)
								this_one = TRUE;
							break;
						case 3:   /* nuc-prot */
							if (bsp->_class == 1)
								this_one = TRUE;
							break;
						case 4:   /* pub-set */
							if (bsp->_class == 9)
								this_one = TRUE;
							break;
					}
					if (this_one)
						oop->in_right_set = TRUE;
				}
			}
            else if (atp == BIOSEQ_SET_release)
                bsp->release = (CharPtr)av.ptrvalue;
        }
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end BioseqSet */

	if (check_set)      /* check sets */
	{
		if (! got_it)
		{
			if (get_bioseq)   /* can't use anything at this level */
				return BioseqSetFree(bsp);

			if (osp->found_it)    /* found the contained Bioseq */
			{
				if (! this_one)
					oop->working_on_set = 1;
				else
					oop->working_on_set = 2;
				oop->in_right_set = old_in_right_set;
				return bsp;
			}
		}

		if (got_it)
		{
			if ((this_one) && (oop->working_on_set != 2))
			{
				oop->working_on_set = 2;
				bsp->seq_set = hold;
			}
			/* copy the annot and descr if lower level is BioseqSet */
			else if (IS_Bioseq_set(hold))
			{                              /* make this smarter */
				tmp = (BioseqSetPtr)hold->data.ptrvalue;
				hold->data.ptrvalue = NULL;
				SeqEntryFree(hold);
				if (tmp->annot == NULL)
					tmp->annot = bsp->annot;
				else
                {
                    sap = tmp->annot;
                    while (sap->next != NULL)
                        sap = sap->next;
                    sap->next = bsp->annot;
                }
				bsp->annot = NULL;
                SeqDescrPack(&tmp->descr, &bsp->descr);
				BioseqSetFree(bsp);
				bsp = tmp;
			}
			else
				bsp->seq_set = hold;
		}
	}

	if (check_set)
		oop->in_right_set = old_in_right_set;
ret:
    AsnUnlinkType(orig);     /*  unlink local tree */
	return bsp;
erret:
	bsp = BioseqSetFree(bsp);
	goto ret;
}

static SeqEntryPtr PNTR SElist;
static Int2 SEnum;

/*****************************************************************************
*
*   SeqEntryInMem(&snum)
*
*****************************************************************************/
SeqEntryPtr PNTR LIBCALL SeqEntryInMem (Int2Ptr numptr)
{
	*numptr = SEnum;
	if (! SEnum)
		return (SeqEntryPtr PNTR)NULL;
	else
		return SElist;
}

/*****************************************************************************
*
*   SeqEntryNew()
*
*****************************************************************************/
SeqEntryPtr LIBCALL SeqEntryNew (void)
{
    SeqEntryPtr sep;
    SeqEntryPtr PNTR tmp;
    Int2 i;

    sep = ValNodeNew(NULL);
	if (sep == NULL) return sep;

    for (i = 0; i < SEnum; i++)    /* add to seqentry list */
    {
        if (SElist[i] == NULL)
        {
            SElist[i] = sep;
            return sep;
        }
    }
    tmp = SElist;                 /* expand list as necessary */
    SElist = (SeqEntryPtr PNTR)MemNew((sizeof(SeqEntryPtr) * (SEnum + 20)));
	if (SElist == NULL)
	{
		SElist = tmp;
		return (SeqEntryPtr)(MemFree(sep));
	}
    MemCopy(SElist, tmp, (size_t)(sizeof(SeqEntryPtr) * SEnum));
    MemFree(tmp);
    SElist[SEnum] = sep;
    SEnum += 20;

    return sep;
}

/*****************************************************************************
*
*   SeqEntryFree(sep)
*       Frees one SeqEntry and associated data
*
*****************************************************************************/
SeqEntryPtr LIBCALL SeqEntryFree (SeqEntryPtr sep)
{
    Int2 i;

    if (sep == NULL)
        return sep;

    if (sep->choice == 1)
        BioseqFree((BioseqPtr)sep->data.ptrvalue);
    else if (sep->choice == 2)
        BioseqSetFree((BioseqSetPtr)sep->data.ptrvalue);
    for (i = 0; i < SEnum; i++)
    {
        if (SElist[i] == sep)
        {
            SElist[i] = NULL;
            return (SeqEntryPtr)MemFree(sep);
        }
    }
    Message(MSG_ERROR, "SeqEntryFree: pointer not in SElist");
	return (SeqEntryPtr)MemFree(sep);
}

/*****************************************************************************
*
*   SeqEntryAsnWrite(sep, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SeqEntry ::=)
*
*****************************************************************************/
Boolean LIBCALL SeqEntryAsnWrite (SeqEntryPtr sep, AsnIoPtr aip, AsnTypePtr orig)
{
    AsnTypePtr atp;
	DataVal av;
	Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqSetAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, SEQ_ENTRY);   /* link local tree */
	if (atp == NULL) return FALSE;

	if (sep == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

	av.ptrvalue = (Pointer)sep;
    if (! AsnWriteChoice(aip, atp, (Int2)sep->choice, &av)) goto erret;
    if (sep->choice == 1)
	{
        if (! BioseqAsnWrite((BioseqPtr)sep->data.ptrvalue, aip, SEQ_ENTRY_seq))
			goto erret;
	}
    else if (sep->choice == 2)
	{
        if (! BioseqSetAsnWrite((BioseqSetPtr)sep->data.ptrvalue, aip, SEQ_ENTRY_set))
			goto erret;
	}
	retval = TRUE;
erret:
    AsnUnlinkType(orig);
    return retval;
}

/*****************************************************************************
*
*   SeqEntryAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
SeqEntryPtr LIBCALL SeqEntryAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    SeqEntryPtr sep=NULL;

	if (! loaded)
	{
		if (! SeqSetAsnLoad())
			return sep;
	}

	if (aip == NULL)
		return sep;

	if (orig == NULL)           /* SeqEntry ::= (self contained) */
		atp = AsnReadId(aip, amp, SEQ_ENTRY);
	else
		atp = AsnLinkType(orig, SEQ_ENTRY);    /* link in local tree */
	if (atp == NULL) return sep;

	sep = SeqEntryNew();
	if (sep == NULL) goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the CHOICE */
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;   /* which choice? */
    if (atp == SEQ_ENTRY_seq)
    {
        sep->choice = 1;
        sep->data.ptrvalue = (Pointer) BioseqAsnRead(aip, atp);
    }
    else if (atp == SEQ_ENTRY_set)
    {
        sep->choice = 2;
        sep->data.ptrvalue = (Pointer) BioseqSetAsnRead(aip, atp);
    }

	if (sep->data.ptrvalue == NULL)
		goto erret;
ret:
    AsnUnlinkType(orig);      /*  unlink local tree */
	return sep;
erret:
	sep = SeqEntryFree(sep);
	goto ret;
}

/*****************************************************************************
*
*   Used by SeqEntryAsnGet to clear unwanted features
*
*****************************************************************************/

typedef struct secf {
	Int2 numseqids;
	SeqIdPtr PNTR seqids;
} Secf, PNTR SecfPtr;

static void SeqEntryGetIds PROTO((SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent));
static void SeqEntryDumpFeats PROTO((SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent));
static void SeqEntryClearFeatures PROTO((SeqEntryPtr sep));

/*****************************************************************************
*
*   SeqEntryAsnGet(aip, atp, sip, retcode)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*       looks for entry with sip=SeqId
*       returns a collection of maximum complexity = retcode
*         where retcode =
        entry (0) ,             -- the "natural" entry for this (nuc-prot)
        bioseq (1) ,            -- only the bioseq identified
        bioseq-set (2) ,        -- any seg-set it may be part of
        nuc-prot (3) ,            -- any nuc-prot it may be part of
        pub-set (4) } DEFAULT entry }
*
*****************************************************************************/
SeqEntryPtr LIBCALL SeqEntryAsnGet (AsnIoPtr aip, AsnTypePtr orig, SeqIdPtr sip, Int2 retcode)
{
	AsnOptionPtr aop;
	Op_objssetPtr oop;
	Op_objseqPtr osp;
    SeqEntryPtr sep=NULL;
	BioseqSetPtr bssp;
	BioseqPtr bsp;
    SeqAnnotPtr sap;
    DataVal av;
	Boolean pack_it = FALSE;

	if ((aip == NULL) || (sip == NULL) || (retcode < 0) || (retcode > 4))
		return sep;

	if (retcode >= 3) retcode = 0;   /* only nuc-prots for now */
	av.realvalue = 0.0;    /* just zeros it out to prevent debugger fuss */
	oop = (Op_objssetPtr)MemNew(sizeof(Op_objsset));
	if (oop == NULL) return sep;

	oop->sip = sip;
	oop->retcode = retcode;
	av.ptrvalue = (Pointer) oop;
	aop = AsnIoOptionNew(aip, OP_NCBIOBJSSET, SEQENTRY_OPTION_MAX_COMPLEX, av, DefAsnOptionFree);
	if (aop == NULL)
		return sep;

	osp = (Op_objseqPtr)MemNew(sizeof(Op_objseq));
	if (osp == NULL) return sep;

	osp->sip = sip;
    if (retcode == 1)   /* just after a Bioseq */
        osp->load_by_id = TRUE;
	av.ptrvalue = (Pointer) osp;
	aop = AsnIoOptionNew(aip, OP_NCBIOBJSEQ, BIOSEQ_CHECK_ID, av, DefAsnOptionFree);
	if (aop == NULL)
		return sep;

	sep = SeqEntryAsnRead(aip, orig);
	if (sep == NULL) goto erret;

	if ((retcode == 1) && (IS_Bioseq_set(sep)))
		pack_it = TRUE;
	else if ((retcode == 2) && (IS_Bioseq_set(sep)))
	{							/* got Bioseq, but not part of seg-set */
		bssp = (BioseqSetPtr)sep->data.ptrvalue;
		if (bssp->_class != 2)   /* not a seg-set */
			pack_it = TRUE;
	}


	if (! osp->found_it)
	{
		sep = SeqEntryFree(sep);
	}
	else if (pack_it)  /* want Bioseq out of Bioseq-set */
	{
		bssp = (BioseqSetPtr)sep->data.ptrvalue;
		sep->data.ptrvalue = NULL;
		SeqEntryFree(sep);
		sep = bssp->seq_set;
		bssp->seq_set = NULL;
		bsp = (BioseqPtr)sep->data.ptrvalue;
        SeqDescrPack(&bsp->descr, &bssp->descr);
		if (bsp->annot == NULL)
			bsp->annot = bssp->annot;
		else
        {
    	    sap = bsp->annot;
        	while (sap->next != NULL)
            	sap = sap->next;
	        sap->next = bssp->annot;
        }
		bssp->annot = NULL;
		BioseqSetFree(bssp);
	}

	if (retcode)
		SeqEntryClearFeatures(sep);      /* clear unwanted features */

erret:
	AsnIoOptionFree(aip, OP_NCBIOBJSSET, SEQENTRY_OPTION_MAX_COMPLEX);
	AsnIoOptionFree(aip, OP_NCBIOBJSEQ, BIOSEQ_CHECK_ID);
	return sep;
}

/*****************************************************************************
*
*   void SeqEntryClearFeatures(sep)
*      clears features which do not refer to a Bioseq in sep
*
*****************************************************************************/
static void SeqEntryClearFeatures (SeqEntryPtr sep)
{
	SecfPtr sp;
	Int2 bioseq_count;

	if (sep == NULL)
		return;

	sp = (SecfPtr)MemNew(sizeof(Secf));
	if (sp == NULL) return;
	bioseq_count = (Int2) SeqEntryCount(sep);  /* overestimate */
	sp->numseqids = 0;
	sp->seqids = (SeqIdPtr PNTR)MemNew(bioseq_count * sizeof(SeqIdPtr));
	if (sp->seqids == NULL) return;

	SeqEntryExplore(sep, (Pointer)sp, SeqEntryGetIds);
	SeqEntryExplore(sep, (Pointer)sp, SeqEntryDumpFeats);
    MemFree(sp->seqids);
    MemFree(sp);
    return;
}

/*****************************************************************************
*
*   SeqEntryGetIds()
*   	SeqEntryExplore function to make a list of seqids of all bioseqs in
*       a seqentry.
*
*****************************************************************************/
static void SeqEntryGetIds (SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent)
{
	SecfPtr sfp;

	if (IS_Bioseq(sep))
	{
		sfp = (SecfPtr)data;
        sfp->seqids[sfp->numseqids] = ((BioseqPtr)sep->data.ptrvalue)->id;
		sfp->numseqids++;
	}
    return;
}

/*****************************************************************************
*
*   SeqEntryDumpFeats()
*   	SeqEntryExplore function to clear features which do not match a
*       list of seqids.
*
*****************************************************************************/
static void SeqEntryDumpFeats (SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent)
{
	SecfPtr sp;
	Int2 numids, i;
	SeqIdPtr PNTR sip;
	SeqIdPtr cursip;
	ValNode an;
	SeqAnnotPtr sap, prevsap, sapnext;
	SeqFeatPtr curr, next, prev;
	Boolean got_it;

	sp = (SecfPtr)data;
	numids = sp->numseqids;
	sip = sp->seqids;
	an.choice = SEQLOC_WHOLE;
	if (IS_Bioseq(sep))
		sap = ((BioseqPtr)sep->data.ptrvalue)->annot;
	else
		sap = ((BioseqSetPtr)sep->data.ptrvalue)->annot;
    prevsap = NULL;
	while (sap != NULL)
	{
		if (sap->type == 1)  /* feature table */
		{
			prev = NULL;
			curr = (SeqFeatPtr)sap->data;
			while (curr != NULL)
			{
				got_it = FALSE;
				for (i = 0; i < numids; i++)
				{
					cursip = sip[i];
					while (cursip != NULL)
					{
						an.data.ptrvalue = (Pointer)cursip;
						if (SeqLocCompare(&an, curr->product))
						{
							got_it = TRUE;
							break;
						}
						if (SeqLocCompare(&an, curr->location))
						{
							got_it = TRUE;
							break;
						}
						cursip = cursip->next;
					}
					if (got_it)
						break;
				}
				next = curr->next;
				if (! got_it)    /* delete it */
				{
					if (prev == NULL)
						sap->data = (Pointer)next;
					else
						prev->next = next;
					SeqFeatFree(curr);
				}
				else
					prev = curr;
				curr = next;
			}
			/* delete feature table if empty */
            if (sap->data == NULL)   /* deleted them all */
            {
                sapnext = sap->next;
                SeqAnnotFree(sap);
                if (prevsap == NULL)
                {
                	if (IS_Bioseq(sep))
		                ((BioseqPtr)sep->data.ptrvalue)->annot = sapnext;
                	else
		                ((BioseqSetPtr)sep->data.ptrvalue)->annot = sapnext;
                }
                sap = sapnext;
            }
            else
                sap = sap->next;
		}
        else
    		sap = sap->next;
	}
    return;
}


/*****************************************************************************
*
*   Boolean SeqEntryLoad()
*
*****************************************************************************/
Boolean LIBCALL SeqEntryLoad (void)
{
    if (! SeqSetAsnLoad())
        return FALSE;

    return BioseqLoad();   /* need all the Bioseq stuff */
}



