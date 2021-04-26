/*  objpub.c
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
* File Name:  objpub.c
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.3 $
*
* File Description:  Object manager for module NCBI-Pub
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
#include <objpub.h>		   /* the pub interface */
#include <asnpub.h>        /* the AsnTool header */

static Boolean loaded = FALSE;

/*****************************************************************************
*
*   PubAsnLoad()
*
*****************************************************************************/
Boolean LIBCALL PubAsnLoad ()
{
    if (loaded)
        return TRUE;
    loaded = TRUE;

    if (! GeneralAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    if (! BiblioAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    if (! MedlineAsnLoad())
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
*   Pub Routines
*
*****************************************************************************/
/*****************************************************************************
*
*   Pub is a choice using an ValNode
*   choice:
*   0 = not set
    1 = gen Cit-gen ,        -- general or generic unparsed
    2 = sub Cit-sub ,        -- submission
    3 = medline Medline-entry ,
    4 = muid INTEGER ,       -- medline uid
    5 = article Cit-art ,
    6 = journal Cit-jour ,
    7 = book Cit-book ,
    8 = proc Cit-proc ,      -- proceedings of a meeting
    9 = patent Cit-pat ,
    10 = pat-id Id-pat ,      -- identify a patent
    11 = man Cit-let ,        -- manuscript or letter
    12 = equiv Pub-equiv      -- set of equivalent citation forms for 1 pub
*
*****************************************************************************/
/*****************************************************************************
*
*   PubFree(anp)
*       Frees one pub and associated data
*
*****************************************************************************/
ValNodePtr LIBCALL PubFree (ValNodePtr anp)
{
    Pointer pnt;

    if (anp == NULL)
        return anp;
    
    pnt = anp->data.ptrvalue;
    switch (anp->choice)
    {
        case 1:                   /* gen */
            CitGenFree((CitGenPtr)pnt);
			break;
        case 2:                   /* sub */
            CitSubFree((CitSubPtr)pnt);
            break;
        case 3:                   /* medline */
            MedlineEntryFree((MedlineEntryPtr)pnt);
            break;
        case 4:                   /* uid in intvalue */
            break;
        case 5:                   /* article */
            CitArtFree((CitArtPtr)pnt);
            break;
        case 6:                   /* journal */
            CitJourFree((CitJourPtr)pnt);
            break;
        case 7:                   /* book */
        case 8:                   /* proceedings */
        case 11:                  /* manuscript */
            CitBookFree((CitBookPtr)pnt);
            break;
        case 9:                   /* patent */
            CitPatFree((CitPatPtr)pnt);
            break;
        case 10:                  /* patent id */
            IdPatFree((IdPatPtr)pnt);
            break;
        case 12:
            PubEquivFree((ValNodePtr)pnt);
            break;
    }
	return (ValNodePtr)MemFree(anp);
}

/*****************************************************************************
*
*   PubAsnWrite(anp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (Pub ::=)
*
*****************************************************************************/
Boolean LIBCALL PubAsnWrite (ValNodePtr anp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, writetype = NULL;
    Pointer pnt;
    AsnWriteFunc func = NULL;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! PubAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, PUB);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

	av.ptrvalue = (Pointer)anp;
	if (! AsnWriteChoice(aip, atp, (Int2)anp->choice, &av)) goto erret;

    pnt = anp->data.ptrvalue;
    switch (anp->choice)
    {
        case 1:                   /* gen */
            writetype = PUB_gen;
            func = (AsnWriteFunc) CitGenAsnWrite;
            break;
        case 2:                   /* sub */
            writetype = PUB_sub;
            func = (AsnWriteFunc) CitSubAsnWrite;
            break;
        case 3:                   /* medline */
            writetype = PUB_medline;
            func = (AsnWriteFunc) MedlineEntryAsnWrite;
            break;
        case 4:                   /* uid in intvalue */
            av.intvalue = anp->data.intvalue;
            if (! AsnWrite(aip, PUB_muid, &av)) goto erret;
            break;
        case 5:                   /* article */
            writetype = PUB_article;
            func = (AsnWriteFunc) CitArtAsnWrite;
            break;
        case 6:                   /* journal */
            writetype = PUB_journal;
            func = (AsnWriteFunc) CitJourAsnWrite;
            break;
        case 7:                   /* book */
            writetype = PUB_book;
            func = (AsnWriteFunc) CitBookAsnWrite;
            break;
        case 8:                   /* proceedings */
            writetype = PUB_proc;
            func = (AsnWriteFunc) CitProcAsnWrite;
            break;
        case 11:                  /* manuscript */
            writetype = PUB_man;
            func = (AsnWriteFunc) CitLetAsnWrite;
            break;
        case 9:                   /* patent */
            writetype = PUB_patent;
            func = (AsnWriteFunc) CitPatAsnWrite;
            break;
        case 10:                  /* patent id */
            writetype = PUB_pat_id;
            func = (AsnWriteFunc) IdPatAsnWrite;
            break;
        case 12:                  /* equiv */
            writetype = PUB_equiv;
            func = (AsnWriteFunc) PubEquivAsnWrite;
            break;
    }
    if (writetype != NULL)
    {
        if (! (*func)(pnt, aip, writetype)) goto erret;
    }
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}
/*****************************************************************************
*
*   PubAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
ValNodePtr LIBCALL PubAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    ValNodePtr anp=NULL;
    Uint1 choice;
    AsnReadFunc func;

	if (! loaded)
	{
		if (! PubAsnLoad())
			return anp;
	}

	if (aip == NULL)
		return anp;

	if (orig == NULL)           /* Pub ::= (self contained) */
		atp = AsnReadId(aip, amp, PUB);
	else
		atp = AsnLinkType(orig, PUB);    /* link in local tree */
    if (atp == NULL) return anp;

	anp = ValNodeNew(NULL);
    if (anp == NULL) goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the CHOICE value (nothing) */
	atp = AsnReadId(aip, amp, atp);  /* find the choice */
    if (atp == NULL) goto erret;

	if (atp == PUB_gen)
    {
        choice = 1;
        func = (AsnReadFunc) CitGenAsnRead;
    }
    else if (atp == PUB_sub)
    {
        choice = 2;
        func = (AsnReadFunc) CitSubAsnRead;
    }
    else if (atp == PUB_medline)
    {
        choice = 3;
        func = (AsnReadFunc) MedlineEntryAsnRead;
    }
    else if (atp == PUB_muid)
    {
        choice = 4;
        if (AsnReadVal(aip, atp, &anp->data) <= 0) goto erret;
    }
    else if (atp == PUB_article)
    {
        choice = 5;
        func = (AsnReadFunc) CitArtAsnRead;
    }
    else if (atp == PUB_journal)
    {
        choice = 6;
        func = (AsnReadFunc) CitJourAsnRead;
    }
    else if (atp == PUB_book)
    {
        choice = 7;
        func = (AsnReadFunc) CitBookAsnRead;
    }
    else if (atp == PUB_proc)
    {
        choice = 8;
        func = (AsnReadFunc) CitProcAsnRead;
    }
    else if (atp == PUB_patent)
    {
        choice = 9;
        func = (AsnReadFunc) CitPatAsnRead;
    }
    else if (atp == PUB_pat_id)
    {
        choice = 10;
        func = (AsnReadFunc) IdPatAsnRead;
    }
    else if (atp == PUB_man)
    {
        choice = 11;
        func = (AsnReadFunc) CitLetAsnRead;
    }
    else if (atp == PUB_equiv)
    {
        choice = 12;
        func = (AsnReadFunc) PubEquivAsnRead;
    }

    anp->choice = choice;
    if (choice != 4)
    {
        anp->data.ptrvalue = (* func)(aip, atp);
        if (anp->data.ptrvalue == NULL) goto erret;
    }
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return anp;
erret:
    anp = PubFree(anp);
    goto ret;
}

/*****************************************************************************
*
*   PubSet is a choice using an ValNode
*   choice:
*   0 = not set
    1 = pub Pub ,        -- mixed pubs
    3 = medline Medline-entry ,    -- use same mapping as Pub for these
    5 = article Cit-art ,
    6 = journal Cit-jour ,
    7 = book Cit-book ,
    8 = proc Cit-proc ,      -- proceedings of a meeting
    9 = patent Cit-pat ,
*
*   PubSet->data.ptrvalue points to chain of Pub (even if not mixed) so
*      each Pub is self identifying anyway and Pub routines can be used
*****************************************************************************/
/*****************************************************************************
*
*   PubSetFree(anp)
*       Frees one Pub-set and associated data
*
*****************************************************************************/
ValNodePtr LIBCALL PubSetFree (ValNodePtr anp)
{
    if (anp == NULL)
        return anp;

    PubEquivFree((ValNodePtr)anp->data.ptrvalue);
    return (ValNodePtr)MemFree(anp);
}

/*****************************************************************************
*
*   PubSetAsnWrite(anp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (PubSet ::=)
*
*****************************************************************************/
Boolean LIBCALL PubSetAsnWrite (ValNodePtr anp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, settype, elementtype;
    Pointer pnt;
    Uint1 choice;
    AsnWriteFunc func;
	ValNodePtr oldanp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! PubAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, PUB_SET);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

	av.ptrvalue = (Pointer)anp;
	if (! AsnWriteChoice(aip, atp, (Int2)anp->choice, &av)) goto erret;
    choice = anp->choice;              /* type of set */
    switch (choice)    
    {
        case 1:              
            settype = PUB_SET_pub;
            elementtype = PUB_SET_pub_E;
            func = (AsnWriteFunc) PubAsnWrite;
            break;
        case 3:              
            settype = PUB_SET_medline;
            elementtype = PUB_SET_medline_E;
            func = (AsnWriteFunc) MedlineEntryAsnWrite;
            break;
        case 5:              
            settype = PUB_SET_article;
            elementtype = PUB_SET_article_E;
            func = (AsnWriteFunc) CitArtAsnWrite;
            break;
        case 6:              
            settype = PUB_SET_journal;
            elementtype = PUB_SET_journal_E;
            func = (AsnWriteFunc) CitJourAsnWrite;
            break;
        case 7:  
            settype = PUB_SET_book;
            elementtype = PUB_SET_book_E;
            func = (AsnWriteFunc) CitBookAsnWrite;
            break;
        case 8:
            settype = PUB_SET_proc;
            elementtype = PUB_SET_proc_E;
            func = (AsnWriteFunc) CitBookAsnWrite;
            break;
        case 9:
            settype = PUB_SET_patent;
            elementtype = PUB_SET_patent_E;
            func = (AsnWriteFunc) CitPatAsnWrite;
            break;
    }

	oldanp = anp;
    if (! AsnOpenStruct(aip, settype, oldanp->data.ptrvalue))   /* start SET OF */
        goto erret;

    anp = (ValNodePtr)anp->data.ptrvalue;

    while (anp != NULL)
    {
        if (choice == 1)
            pnt = (Pointer)anp;
        else
            pnt = anp->data.ptrvalue;
        if (! (*func)(pnt, aip, elementtype)) goto erret;
        anp = anp->next;
    }
    
    if (! AsnCloseStruct(aip, settype, oldanp->data.ptrvalue))  /* end SET OF */
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   PubSetAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
ValNodePtr LIBCALL PubSetAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, settype;
    Pointer pnt;
    ValNodePtr anp=NULL, cit, curr;
    Uint1 choice;
    AsnReadFunc func;
    Boolean first;

	if (! loaded)
	{
		if (! PubAsnLoad())
			return anp;
	}

	if (aip == NULL)
		return anp;

	if (orig == NULL)           /* PubSet ::= (self contained) */
		atp = AsnReadId(aip, amp, PUB_SET);
	else
		atp = AsnLinkType(orig, PUB_SET);    /* link in local tree */
    if (atp == NULL) return anp;

	anp = ValNodeNew(NULL);
    if (anp == NULL) goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0)    /* read the CHOICE value (nothing) */
        goto erret;
	settype = AsnReadId(aip, amp, atp);  /* find the choice */
    if (settype == NULL) goto erret;
    if (AsnReadVal(aip, settype, &av) <= 0)    /* read START_STRUCT */
        goto erret;

	if (settype == PUB_SET_pub)
    {
        choice = 1;
        func = (AsnReadFunc) PubAsnRead;
    }
    else if (settype == PUB_SET_medline)
    {
        choice = 3;
        func = (AsnReadFunc) MedlineEntryAsnRead;
    }
    else if (settype == PUB_SET_article)
    {
        choice = 5;
        func = (AsnReadFunc) CitArtAsnRead;
    }
    else if (settype == PUB_SET_journal)
    {
        choice = 6;
        func = (AsnReadFunc) CitJourAsnRead;
    }
    else if (settype == PUB_SET_book)
    {
        choice = 7;
        func = (AsnReadFunc) CitBookAsnRead;
    }
    else if (settype == PUB_SET_proc)
    {
        choice = 8;
        func = (AsnReadFunc) CitBookAsnRead;
    }
    else if (settype == PUB_SET_patent)
    {
        choice = 9;
        func = (AsnReadFunc) CitPatAsnRead;
    }

    anp->choice = choice;
    first = TRUE;
    curr = NULL;
    while ((atp = AsnReadId(aip, amp, settype)) != settype)
    {
        if (atp == NULL) goto erret;
        pnt = (* func)(aip, atp);
        if (pnt == NULL) goto erret;
        if (settype == PUB_SET_pub)   /* already a Pub */
        {
            cit = (ValNodePtr)pnt;
        }
        else                          /* make into a Pub */
        {
            cit = ValNodeNew(NULL);
            if (cit == NULL) goto erret;
            cit->data.ptrvalue = pnt;
            cit->choice = choice;
        }

        if (first)
        {
            anp->data.ptrvalue = (Pointer)cit;
            first = FALSE;
        }
        else
        {
            curr->next = cit;
        }
        curr = cit;
    }
    if (AsnReadVal(aip, settype, &av) <= 0)   /* read END_STRUCT for SET OF */
        goto erret;
	if (anp == NULL)
		ErrPost(CTX_NCBIOBJ, 1, "Empty SET OF Pub.  line %ld", aip->linenumber);
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return anp;
erret:
    anp = PubSetFree(anp);
    goto ret;
}

/*****************************************************************************
*
*   PubEquiv is just a chain of Pubs (ValNodes)
*   
*****************************************************************************/
/*****************************************************************************
*
*   PubEquivFree(anp)
*       Frees a chain of Pubs
*
*****************************************************************************/
ValNodePtr LIBCALL PubEquivFree (ValNodePtr anp)
{
    ValNodePtr next;

    while (anp != NULL)
    {
        next = anp->next;
        PubFree(anp);  /* each node individually coded as Pub anyway */
        anp = next;
    }
	return anp;
}

/*****************************************************************************
*
*   PubEquivAsnWrite(anp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (PubEquiv ::=)
*
*****************************************************************************/
Boolean LIBCALL PubEquivAsnWrite (ValNodePtr anp, AsnIoPtr aip, AsnTypePtr orig)
{
	AsnTypePtr atp;
	ValNodePtr oldanp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! PubAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, PUB_EQUIV);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

	oldanp = anp;
    if (! AsnOpenStruct(aip, atp, (Pointer)oldanp))   /* start SET OF */
        goto erret;

    while (anp != NULL)
    {
        if (! PubAsnWrite(anp, aip, PUB_EQUIV_E)) goto erret;
        anp = anp->next;
    }

    if (! AsnCloseStruct(aip, atp, (Pointer)oldanp))     /* end SET OF */
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   PubEquivAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
ValNodePtr LIBCALL PubEquivAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldtype;
    ValNodePtr anp=NULL, cit, curr;
    Boolean first;

	if (! loaded)
	{
		if (! PubAsnLoad())
			return anp;
	}

	if (aip == NULL)
		return anp;

	first = TRUE;

	if (orig == NULL)           /* PubEquiv ::= (self contained) */
		oldtype = AsnReadId(aip, amp, PUB_EQUIV);
	else
		oldtype = AsnLinkType(orig, PUB_EQUIV);    /* link in local tree */
    if (oldtype == NULL) return anp;

    if (AsnReadVal(aip, oldtype, &av) <= 0)    /* read START_STRUCT */
        goto erret;
    atp = oldtype;

    curr = NULL;
    anp = NULL;
    while ((atp = AsnReadId(aip, amp, atp)) != oldtype)
    {
        if (atp == NULL) goto erret;

        cit = PubAsnRead(aip, atp);
        if (cit == NULL) goto erret;

        if (first)
        {
            anp = cit;
            first = FALSE;
        }
        else
        {
            curr->next = cit;
        }
        curr = cit;
    }
    if (AsnReadVal(aip, oldtype, &av) <= 0)   /* read END_STRUCT for SET OF */
        goto erret;
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return anp;
erret:
    anp = PubEquivFree(anp);
    goto ret;
}

/*****************************************************************************
*
*   Int2 PubMatch(a, b)
*   	returns 0 if can determine pubs POINT TO SAME CITATION (not that
*   		they are identical)
*   	returns positive or negative number if not the same,
*   		for arbitrary ordering
*       -2, +2  = different types
*       -1, +1  = different values, same type
*
*****************************************************************************/
Int2 LIBCALL PubMatch (ValNodePtr a, ValNodePtr b)
{
	ValNode vn1;
	ValNodePtr ap, bp, tp, vnp[2];
	Int2 retval, i;
	Int4 muid[2];
	CitArtPtr cap[2];
	CharPtr country[2],   /* for patents */
		number[2],
		app_number[2];

	if (a == NULL) return 2;
	if (b == NULL) return -2;

	                        /* default return for different pub types */
	if (a->choice > b->choice)
		retval = 2;
	else
		retval = -2;

							/* if either is a Pub-equiv, treat as both */
	if ((a->choice == PUB_Equiv) || (b->choice == PUB_Equiv))
	{
		ap = (ValNodePtr)(a->data.ptrvalue);
		bp = (ValNodePtr)(b->data.ptrvalue);
		tp = NULL;
		if (a->choice != PUB_Equiv)
		{
			tp = a;
			ap = NULL;
		}
		else if (b->choice != PUB_Equiv)
		{
			tp = b;
			bp = NULL;
		}
		if (tp != NULL)   /* convert one */
		{
			MemCopy((Pointer)&vn1, tp, sizeof(ValNode));  /* make a copy of the Pub */
			vn1.next = NULL;                   /* remove from any chain */
			tp = (ValNodePtr)&vn1;
			if (ap == NULL)
				ap = tp;
			else
				bp = tp;
		}
		return PubEquivMatch(ap, bp);   /* use the PubEquivMatch() */
	}
                                   /** here we are just matching two pubs */
							/* handle Medline/CitArt combinations */
	                        /* handle CitPat, IdPat combinations */
	vnp[0] = a;
	vnp[1] = b;
	for (i = 0; i < 2; i++)
	{
		cap[i] = NULL;
		muid[i] = 0;
		country[i] = NULL;
		number[i] = NULL;
		app_number[i] = NULL;

		switch (vnp[i]->choice)
		{
			case PUB_Medline:
				muid[i] = ((MedlineEntryPtr)(vnp[i]->data.ptrvalue))->uid;
				cap[i] = ((MedlineEntryPtr)(vnp[i]->data.ptrvalue))->cit;
				break;
			case PUB_Muid:
				muid[i] = vnp[i]->data.intvalue;
				break;
			case PUB_Article:
				cap[i] = (CitArtPtr)(vnp[i]->data.ptrvalue);
				break;
			case PUB_Patent:
				country[i] = ((CitPatPtr)(vnp[i]->data.ptrvalue))->country;
				number[i] = ((CitPatPtr)(vnp[i]->data.ptrvalue))->number;
				app_number[i] = ((CitPatPtr)(vnp[i]->data.ptrvalue))->app_number;
				break;
			case PUB_Pat_id:
				country[i] = ((IdPatPtr)(vnp[i]->data.ptrvalue))->country;
				number[i] = ((IdPatPtr)(vnp[i]->data.ptrvalue))->number;
				app_number[i] = ((IdPatPtr)(vnp[i]->data.ptrvalue))->app_number;
				break;
		}
	}

	if ((muid[0] != 0) && (muid[1] != 0))   /* got 2 muids */
	{
		if (muid[0] == muid[1])
			return (Int2)0;
		else if (muid[0] < muid[1])
			return (Int2)-1;
		else
			return (Int2) 1;
	}

	if ((cap[0] != NULL) && (cap[1] != NULL))  /* 2 articles */
	{
		return CitArtMatch(cap[0], cap[1]);
	}

	if ((country[0] != NULL) && (country[1] != NULL))  /* 2 patents */
	{
		retval = (Int2)StringICmp(country[0], country[1]);
		if (retval < 0)   /* different countries */
			return (Int2) -1;
		else if (retval > 0)
			return (Int2) 1;

		if ((number[0] != NULL) && (number[1] != NULL))
		{
			retval = (Int2)StringICmp(number[0], number[1]);
			if (retval < 0)   /* different number */
				return (Int2) -1;
			else if (retval > 0)
				return (Int2) 1;
			else
				return retval;
		}
		if ((app_number[0] != NULL) && (app_number[1] != NULL))
		{
			retval = (Int2)StringICmp(app_number[0], app_number[1]);
			if (retval < 0)   /* different appl number */
				return (Int2) -1;
			else if (retval > 0)
				return (Int2) 1;
			else
				return retval;
		}
		if (number[0] != NULL)
			return (Int2) -1;
		return (Int2) 1;
	}

	if (a->choice != b->choice)    /* all others must be same type */
	{
		return retval;
	}
							/* compare other types */
	switch (a->choice)
	{
		case PUB_Gen:        /* generic */
			return CitGenMatch((CitGenPtr)a->data.ptrvalue,
					(CitGenPtr)b->data.ptrvalue, TRUE);
		case PUB_Sub:        /* Submission */
		 	return CitSubMatch((CitSubPtr)a->data.ptrvalue,
					(CitSubPtr)b->data.ptrvalue);
		case PUB_Journal:
			return CitJourMatch((CitJourPtr)a->data.ptrvalue,
					(CitJourPtr)b->data.ptrvalue);
		case PUB_Book:
		case PUB_Proc:
		case PUB_Man:
			return CitBookMatch((CitBookPtr)a->data.ptrvalue,
					(CitBookPtr)b->data.ptrvalue);
	}

	return retval;
}

/*****************************************************************************
*
*   Int2 PubEquivMatch(a, b)
*   	returns 0 if can determine pubs POINT TO SAME CITATION (not that
*   		they are identical)
*   	returns +1 or -1 if not the same,
*   		for arbitrary ordering
*
*****************************************************************************/
Int2 LIBCALL PubEquivMatch (ValNodePtr a, ValNodePtr b)
{
	Int2 retval = 0, tmp;
	ValNodePtr vnp;

	while (a != NULL)
	{
		vnp = b;
		while (vnp != NULL)
		{
			tmp = PubMatch(a, vnp);
			if (! tmp)	   /* a match */
			{
				return tmp;
			}
			if (! retval)   /* first one */
				retval = tmp;
			else if ((retval == 2) || (retval == -2))  /* type diffs only */
				retval = tmp;

			vnp = vnp->next;
		}
		a = a->next;
	}
	if (retval < 0)
		retval = -1;
	else
		retval = 1;
	return retval;
}

