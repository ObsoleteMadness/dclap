/*  objfeat.c
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
* File Name:  objfeat.c
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.10 $
*
* File Description:  Object manager for module NCBI-SeqFeat
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
#include <objfeat.h>		   /* the features interface */
#include <asnfeat.h>        /* the AsnTool header */

static Boolean loaded = FALSE;

/*****************************************************************************
*
*   SeqFeatAsnLoad()
*      requires SeqAsnLoad() to be called first
*
*****************************************************************************/
Boolean LIBCALL SeqFeatAsnLoad ()
{
    if (loaded)
        return TRUE;
    loaded = TRUE;

    if (! GeneralAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    if (! SeqLocAsnLoad())      /* SeqLoc refers to Seqfeat and vice versa */
    {
        loaded = FALSE;
        return FALSE;
    }
    if (! PubAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    if ( ! AsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    return TRUE;
}

/*****************************************************************************
*
*   SeqFeat Routines
*
*****************************************************************************/
/*****************************************************************************
*
*   SeqFeatNew()
*
*****************************************************************************/
SeqFeatPtr LIBCALL SeqFeatNew ()
{
    return (SeqFeatPtr)MemNew(sizeof(SeqFeat));
}

/*****************************************************************************
*
*   SeqFeatFree(sfp)
*       Frees one SeqFeat and associated data
*
*****************************************************************************/
SeqFeatPtr LIBCALL SeqFeatFree (SeqFeatPtr sfp)
{
    if (sfp == NULL)
        return (SeqFeatPtr)NULL;

    SeqFeatIdFree(&sfp->id);
	SeqFeatDataFree(&sfp->data);
    MemFree(sfp->title);
    MemFree(sfp->comment);
    UserObjectFree(sfp->ext);
    SeqLocFree(sfp->product);
    SeqLocFree(sfp->location);
    GBQualFree(sfp->qual);
    PubSetFree(sfp->cit);
	SeqFeatXrefFree(sfp->xref);
	return (SeqFeatPtr)MemFree(sfp);
}

/*****************************************************************************
*
*   SeqFeatAsnWrite(sfp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SeqFeat ::=)
*
*****************************************************************************/
Boolean LIBCALL SeqFeatAsnWrite (SeqFeatPtr sfp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    GBQualPtr gbp;
    Boolean retval = FALSE;
	SeqFeatXrefPtr sfxp;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, SEQ_FEAT);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (sfp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)sfp))
        goto erret;

    if (sfp->id.choice)              /* feat-id present */
    {
        if (! SeqFeatIdAsnWrite(&sfp->id, aip, SEQ_FEAT_id))
            goto erret;
    }

	if (! SeqFeatDataAsnWrite(&sfp->data, aip, SEQ_FEAT_data)) goto erret;


    av.boolvalue = TRUE;
    if (sfp->partial)
    {
        if (! AsnWrite(aip, SEQ_FEAT_partial, &av)) goto erret;
    }
    if (sfp->excpt)
    {
        if (! AsnWrite(aip, SEQ_FEAT_except, &av)) goto erret;
    }
    if (sfp->comment != NULL)
    {
        av.ptrvalue = sfp->comment;
        if (! AsnWrite(aip, SEQ_FEAT_comment, &av)) goto erret;
    }
    if (sfp->product != NULL)
    {
        if (! SeqLocAsnWrite(sfp->product, aip, SEQ_FEAT_product))
            goto erret;
    }
    if (sfp->location != NULL)
    {
        if (! SeqLocAsnWrite(sfp->location, aip, SEQ_FEAT_location))
            goto erret;
    }
    if (sfp->qual != NULL)
    {							      /* SEQUENCE OF */
        if (! AsnOpenStruct(aip, SEQ_FEAT_qual, (Pointer)sfp->qual))
            goto erret;
        gbp = sfp->qual;
        while (gbp != NULL)
        {
            if (! GBQualAsnWrite(gbp, aip, SEQ_FEAT_qual_E))
                goto erret;
            gbp = gbp->next;
        }
        if (! AsnCloseStruct(aip, SEQ_FEAT_qual, (Pointer)sfp->qual))
            goto erret;
    }
    if (sfp->title != NULL)
    {
        av.ptrvalue = (Pointer) sfp->title;
        if (! AsnWrite(aip, SEQ_FEAT_title, &av)) goto erret;
    }
    if (sfp->ext != NULL)
    {
        if (! UserObjectAsnWrite(sfp->ext, aip, SEQ_FEAT_ext))
            goto erret;
    }

    if (sfp->cit != NULL)
    {
        if (! PubSetAsnWrite(sfp->cit, aip, SEQ_FEAT_cit))
            goto erret;
    }

	if (sfp->exp_ev)
	{
		av.intvalue = (Int4)sfp->exp_ev;
		if (! AsnWrite(aip, SEQ_FEAT_exp_ev, &av)) goto erret;
	}

	if (sfp->xref != NULL)
	{
        if (! AsnOpenStruct(aip, SEQ_FEAT_xref, (Pointer)sfp->xref))
            goto erret;
        sfxp = sfp->xref;
        while (sfxp != NULL)
        {
            if (! SeqFeatXrefAsnWrite(sfxp, aip, SEQ_FEAT_xref_E))
                goto erret;
            sfxp = sfxp->next;
        }
        if (! AsnCloseStruct(aip, SEQ_FEAT_xref, (Pointer)sfp->xref))
            goto erret;
	}

    if (! AsnCloseStruct(aip, atp, (Pointer)sfp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqFeatAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
SeqFeatPtr LIBCALL SeqFeatAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldtype;
    SeqFeatPtr sfp;
    GBQualPtr gbq, gbqlast = NULL;
	SeqFeatXrefPtr sfxp, sfxplast = NULL;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return (SeqFeatPtr)NULL;
	}

	if (aip == NULL)
		return (SeqFeatPtr)NULL;

	if (orig == NULL)           /* SeqFeat ::= (self contained) */
		atp = AsnReadId(aip, amp, SEQ_FEAT);
	else
		atp = AsnLinkType(orig, SEQ_FEAT);    /* link in local tree */
    oldtype = atp;
    if (atp == NULL)
        return (SeqFeatPtr)NULL;

	sfp = SeqFeatNew();
    if (sfp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */
    while ((atp = AsnReadId(aip, amp, atp)) != oldtype)
    {
        if (atp == NULL)
            goto erret;
        if (atp == SEQ_FEAT_id)
        {
            if (! SeqFeatIdAsnRead(aip, atp, &sfp->id))
                goto erret;
        }
        else if (atp == SEQ_FEAT_data)
        {
			if (! SeqFeatDataAsnRead(aip, atp, &sfp->data))
				goto erret;
		}
        else if (atp == SEQ_FEAT_product)
        {
            sfp->product = SeqLocAsnRead(aip, atp);
            if (sfp->product == NULL) goto erret;
        }
        else if (atp == SEQ_FEAT_location)
        {
            sfp->location = SeqLocAsnRead(aip, atp);
            if (sfp->location == NULL) goto erret;
        }
        else if (atp == SEQ_FEAT_qual_E)
        {
			gbq = GBQualAsnRead(aip, atp);
			if (gbq == NULL) goto erret;
			if (gbqlast == NULL)
				sfp->qual = gbq;
			else
				gbqlast->next = gbq;
			gbqlast = gbq;
        }
        else if (atp == SEQ_FEAT_ext)
        {
            sfp->ext = UserObjectAsnRead(aip, atp);
            if (sfp->ext == NULL)
                goto erret;
        }
        else if (atp == SEQ_FEAT_cit)
        {
            sfp->cit = PubSetAsnRead(aip, atp);
            if (sfp->cit == NULL)
                goto erret;
        }
		else if (atp == SEQ_FEAT_xref_E)
		{
			sfxp = SeqFeatXrefAsnRead(aip, atp);
			if (sfxp == NULL) goto erret;
			if (sfxplast == NULL)
				sfp->xref = sfxp;
			else
				sfxplast->next = sfxp;
			sfxplast = sfxp;
		}
        else
        {
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
            if (atp == SEQ_FEAT_partial)
                sfp->partial = av.boolvalue;
            else if (atp == SEQ_FEAT_except)
                sfp->excpt = av.boolvalue;
            else if (atp == SEQ_FEAT_comment)
                sfp->comment = (CharPtr) av.ptrvalue;
            else if (atp == SEQ_FEAT_title)
                sfp->title = (CharPtr) av.ptrvalue;
			else if (atp == SEQ_FEAT_exp_ev)
				sfp->exp_ev = (Uint1) av.intvalue;
        }
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return sfp;
erret:
    sfp = SeqFeatFree(sfp);
    goto ret;
}

/*****************************************************************************
*
*   SeqFeatSetAsnWrite(sfp, aip, set, element)
*
*****************************************************************************/
Boolean LIBCALL SeqFeatSetAsnWrite (SeqFeatPtr sfp, AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element)
{
	AsnTypePtr atp;
	SeqFeatPtr oldsfp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(element, SEQ_FEAT);   /* link local tree */
    if (atp == NULL)
        return FALSE;
	oldsfp = sfp;

    if (! AsnOpenStruct(aip, set, (Pointer)oldsfp))
        goto erret;

    while (sfp != NULL)
    {
        if (! SeqFeatAsnWrite(sfp, aip, atp))
            goto erret;
        sfp = sfp->next;
    }

    if (! AsnCloseStruct(aip, set, (Pointer)oldsfp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(element);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqFeatSetAsnRead(aip, set, element)
*
*****************************************************************************/
SeqFeatPtr LIBCALL SeqFeatSetAsnRead (AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element)
{
	DataVal av;
	AsnTypePtr atp;
    SeqFeatPtr sfp, curr = NULL, first = NULL;

	if (aip == NULL)
		return (SeqFeatPtr)NULL;

    AsnLinkType(element, SEQ_FEAT);    /* link in local tree */
	atp = set;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */
    while ((atp = AsnReadId(aip, amp, atp)) == element)
    {
        sfp = SeqFeatAsnRead(aip, atp);
        if (sfp == NULL)
            goto erret;
        if (first == NULL)
            first = sfp;
        else
            curr->next = sfp;
        curr = sfp;
    }
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
	if (first == NULL)
		ErrPost(CTX_NCBIOBJ, 1, "Empty SET OF Seq-feat. line %ld", aip->linenumber);
ret:
	AsnUnlinkType(element);       /* unlink local tree */
	return first;
erret:
    while (first != NULL)
    {
        curr = first;
        first = curr->next;
        SeqFeatFree(curr);
    }
    goto ret;
}

/*****************************************************************************
*
*   SeqFeatDataFree(cp)
*
*****************************************************************************/
void LIBCALL SeqFeatDataFree(ChoicePtr cp)
{
	Pointer pnt;

    pnt = cp->value.ptrvalue;
    switch (cp->choice)
    {
        case 1:
            GeneRefFree((GeneRefPtr)pnt);
            break;
        case 2:
            OrgRefFree((OrgRefPtr)pnt);
            break;
        case 3:
            CdRegionFree((CdRegionPtr)pnt);
            break;
        case 4:
            ProtRefFree((ProtRefPtr)pnt);
            break;
        case 5:
            RnaRefFree((RnaRefPtr)pnt);
            break;
        case 6:
            PubdescFree((PubdescPtr)pnt);
            break;
        case 7:         /* seq */
            SeqLocFree((ValNodePtr)pnt);
            break;
        case 8:
            ImpFeatFree((ImpFeatPtr)pnt);
            break;
        case 9:        /* region */
		case 18:       /* non-std residue */
		case 19:       /* heterogen */
            MemFree(pnt);
            break;
        case 10:       /* comment - nothing */
        case 11:       /* bond - int */
        case 12:       /* site - int */
		case 17:       /* psec_struct - int */
            break;
        case 13:
            RsiteRefFree((RsiteRefPtr)pnt);
            break;
        case 14:
            UserObjectFree((UserObjectPtr)pnt);
            break;
        case 15:
            TxinitFree((TxinitPtr)pnt);
            break;
		case 16:
			NumberingFree((NumberingPtr)pnt);
			break;
    }
	return;
}

/*****************************************************************************
*
*   SeqFeatDataAsnWrite(cp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SeqFeatData ::=)
*
*****************************************************************************/
Boolean LIBCALL SeqFeatDataAsnWrite (ChoicePtr cp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Pointer pnt;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, SEQFEATDATA);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (cp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    pnt = cp->value.ptrvalue;
	av.ptrvalue = pnt;
    if (! AsnWriteChoice(aip, atp, (Int2)cp->choice, &av)) goto erret;

    switch (cp->choice)
    {
        case 1:
            if (! GeneRefAsnWrite((GeneRefPtr)pnt, aip, SEQFEATDATA_gene))
                goto erret;
            break;
        case 2:
            if (! OrgRefAsnWrite((OrgRefPtr)pnt, aip, SEQFEATDATA_org))
                goto erret;
            break;
        case 3:
            if (! CdRegionAsnWrite((CdRegionPtr)pnt, aip, SEQFEATDATA_cdregion))
                goto erret;
            break;
        case 4:
            if (! ProtRefAsnWrite((ProtRefPtr)pnt, aip, SEQFEATDATA_prot))
                goto erret;
            break;
        case 5:
            if (! RnaRefAsnWrite((RnaRefPtr)pnt, aip, SEQFEATDATA_rna))
                goto erret;
            break;
        case 6:
            if (! PubdescAsnWrite((PubdescPtr)pnt, aip, SEQFEATDATA_pub))
                goto erret;
            break;
        case 7:         /* seq reference */
            if (! SeqLocAsnWrite((ValNodePtr)pnt, aip, SEQFEATDATA_seq))
                goto erret;
            break;
        case 8:
            if (! ImpFeatAsnWrite((ImpFeatPtr)pnt, aip, SEQFEATDATA_imp))
                goto erret;
            break;
        case 9:        /* region */
            av.ptrvalue = pnt;
            if (! AsnWrite(aip, SEQFEATDATA_region, &av)) goto erret;
            break;
        case 10:       /* comment - nothing */
            if (! AsnWrite(aip, SEQFEATDATA_comment, &av)) goto erret;
            break;
        case 11:       /* bond - int */
            av.intvalue = cp->value.intvalue;
            if (! AsnWrite(aip, SEQFEATDATA_bond, &av)) goto erret;
            break;
        case 12:       /* site - int */
            av.intvalue = cp->value.intvalue;
            if (! AsnWrite(aip, SEQFEATDATA_site, &av)) goto erret;
            break;
        case 13:       /* rsite-ref */
            if (! RsiteRefAsnWrite((RsiteRefPtr)pnt, aip, SEQFEATDATA_rsite))
                goto erret;
            break;
        case 14:       /* user object */
            if (! UserObjectAsnWrite((UserObjectPtr)pnt, aip, SEQFEATDATA_user))
                goto erret;
            break;
        case 15:
            if (! TxinitAsnWrite((TxinitPtr)pnt, aip, SEQFEATDATA_txinit))
                goto erret;
            break;
        case 16:
            if (! NumberingAsnWrite((NumberingPtr)pnt, aip, SEQFEATDATA_num))
                goto erret;
            break;
        case 17:       /* psec-struct - int */
            av.intvalue = cp->value.intvalue;
            if (! AsnWrite(aip, SEQFEATDATA_psec_str, &av)) goto erret;
            break;
        case 18:        /* non-std residue */
            av.ptrvalue = pnt;
            if (! AsnWrite(aip, SEQFEATDATA_non_std_residue, &av)) goto erret;
            break;
        case 19:        /* heterogen */
            av.ptrvalue = pnt;
            if (! AsnWrite(aip, SEQFEATDATA_het, &av)) goto erret;
            break;
    }

    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqFeatDataAsnRead(aip, atp, cp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
Boolean LIBCALL SeqFeatDataAsnRead (AsnIoPtr aip, AsnTypePtr orig, ChoicePtr cp)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return retval;
	}

	if (aip == NULL)
		return retval;

	if (orig == NULL)           /* SeqFeatData ::= (self contained) */
		atp = AsnReadId(aip, amp, SEQFEATDATA);
	else
		atp = AsnLinkType(orig, SEQFEATDATA);    /* link in local tree */
    if (atp == NULL)
        return retval;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read CHOICE */
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (atp == SEQFEATDATA_gene)
    {
        cp->choice = 1;
        cp->value.ptrvalue = (Pointer)GeneRefAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == SEQFEATDATA_org)
    {
        cp->choice = 2;
        cp->value.ptrvalue = (Pointer)OrgRefAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == SEQFEATDATA_cdregion)
    {
        cp->choice = 3;
        cp->value.ptrvalue = (Pointer)CdRegionAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == SEQFEATDATA_prot)
    {
        cp->choice = 4;
        cp->value.ptrvalue = (Pointer)ProtRefAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == SEQFEATDATA_rna)
    {
        cp->choice = 5;
        cp->value.ptrvalue = (Pointer)RnaRefAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == SEQFEATDATA_pub)
    {
        cp->choice = 6;
        cp->value.ptrvalue = (Pointer)PubdescAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == SEQFEATDATA_seq)
    {
        cp->choice = 7;
        cp->value.ptrvalue = (Pointer)SeqLocAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == SEQFEATDATA_imp)
    {
        cp->choice = 8;
        cp->value.ptrvalue = (Pointer)ImpFeatAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == SEQFEATDATA_region)
    {
        cp->choice = 9;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        cp->value.ptrvalue = av.ptrvalue;
    }
    else if (atp == SEQFEATDATA_comment)
    {
        cp->choice = 10;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    }
    else if (atp == SEQFEATDATA_bond)
    {
        cp->choice = 11;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        cp->value.intvalue = av.intvalue;
    }
    else if (atp == SEQFEATDATA_site)
    {
        cp->choice = 12;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        cp->value.intvalue = av.intvalue;
    }
    else if (atp == SEQFEATDATA_rsite)
    {
        cp->choice = 13;
        cp->value.ptrvalue = (Pointer)RsiteRefAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == SEQFEATDATA_user)
    {
        cp->choice = 14;
        cp->value.ptrvalue = (Pointer)UserObjectAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == SEQFEATDATA_txinit)
    {
        cp->choice = 15;
        cp->value.ptrvalue = (Pointer)TxinitAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == SEQFEATDATA_num)
    {
        cp->choice = 16;
        cp->value.ptrvalue = (Pointer)NumberingAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == SEQFEATDATA_psec_str)
    {
        cp->choice = 17;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        cp->value.intvalue = av.intvalue;
    }
    else if (atp == SEQFEATDATA_non_std_residue)
    {
        cp->choice = 18;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        cp->value.ptrvalue = av.ptrvalue;
    }
    else if (atp == SEQFEATDATA_het)
    {
        cp->choice = 19;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        cp->value.ptrvalue = av.ptrvalue;
    }

    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   GBQualNew()
*
*****************************************************************************/
GBQualPtr LIBCALL GBQualNew (void)
{
    return (GBQualPtr)MemNew(sizeof(GBQual));
}

/*****************************************************************************
*
*   GBQualFree(gbp)
*       Frees a chain of GBQual and associated data
*
*****************************************************************************/
GBQualPtr LIBCALL GBQualFree (GBQualPtr gbp)
{
    GBQualPtr next;

    while (gbp != NULL)
    {
        next = gbp->next;
        MemFree(gbp->qual);
        MemFree(gbp->val);
        MemFree(gbp);
        gbp = next;
    }
    return (GBQualPtr)NULL;
}

/*****************************************************************************
*
*   GBQualAsnWrite(gbp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (GBQual ::=)
*
*****************************************************************************/
Boolean LIBCALL GBQualAsnWrite (GBQualPtr gbp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, GB_QUAL);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (gbp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)gbp))
        goto erret;

    av.ptrvalue = (Pointer)gbp->qual;
    if (! AsnWrite(aip, GB_QUAL_qual, &av)) goto erret;
    av.ptrvalue = (Pointer)gbp->val;
    if (! AsnWrite(aip, GB_QUAL_val, &av)) goto erret;

    if (! AsnCloseStruct(aip, atp, (Pointer)gbp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   GBQualAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
GBQualPtr LIBCALL GBQualAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    GBQualPtr gbp=NULL;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return gbp;
	}

	if (aip == NULL)
		return gbp;

	if (orig == NULL)           /* GBQual ::= (self contained) */
		atp = AsnReadId(aip, amp, GB_QUAL);
	else
		atp = AsnLinkType(orig, GB_QUAL);    /* link in local tree */
    if (atp == NULL)
        return gbp;

	gbp = GBQualNew();
    if (gbp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    gbp->qual = (CharPtr)av.ptrvalue;
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    gbp->val = (CharPtr)av.ptrvalue;
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return gbp;
erret:
    gbp = GBQualFree(gbp);
    goto ret;
}

/*****************************************************************************
*
*   SeqFeatXrefNew()
*
*****************************************************************************/
SeqFeatXrefPtr LIBCALL SeqFeatXrefNew (void)
{
    return (SeqFeatXrefPtr)MemNew(sizeof(SeqFeatXref));
}

/*****************************************************************************
*
*   SeqFeatToXref(sfp)
*   	Copy a SeqFeat to an Xref
*   	Currently only works on Gene-ref and Prot-ref
*
*****************************************************************************/
SeqFeatXrefPtr LIBCALL SeqFeatToXref (SeqFeatPtr sfp)
{
	SeqFeatXrefPtr sfxp;

	if (sfp == NULL)
		return (SeqFeatXrefPtr)NULL;

	sfxp = SeqFeatXrefNew();
	if (sfxp == NULL) return NULL;
	SeqFeatIdDup(&sfxp->id, &sfp->id);
	switch (sfp->data.choice)
	{
        case 1:
            sfxp->data.value.ptrvalue = (Pointer) GeneRefDup((GeneRefPtr)sfp->data.value.ptrvalue);
			sfxp->data.choice = sfp->data.choice;
            break;
        case 4:
            sfxp->data.value.ptrvalue = (Pointer) ProtRefDup((ProtRefPtr)sfp->data.value.ptrvalue);
			sfxp->data.choice = sfp->data.choice;
            break;
		default:
			ErrPost(CTX_NCBIOBJ, 0, "Unsupported SeqFeatToXref");
			break;
	}

    return sfxp;
}

/*****************************************************************************
*
*   SeqFeatXrefFree(sfxp)
*       Frees a chain of SeqFeatXref and associated data
*
*****************************************************************************/
SeqFeatXrefPtr LIBCALL SeqFeatXrefFree (SeqFeatXrefPtr sfxp)
{
    SeqFeatXrefPtr next;

    while (sfxp != NULL)
    {
        next = sfxp->next;
		SeqFeatIdFree(&sfxp->id);
		SeqFeatDataFree(&sfxp->data);
        MemFree(sfxp);
        sfxp = next;
    }
    return (SeqFeatXrefPtr)NULL;
}

/*****************************************************************************
*
*   SeqFeatXrefAsnWrite(sfxp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SeqFeatXref ::=)
*
*****************************************************************************/
Boolean LIBCALL SeqFeatXrefAsnWrite (SeqFeatXrefPtr sfxp, AsnIoPtr aip, AsnTypePtr orig)
{
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, SEQFEATXREF);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (sfxp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)sfxp))
        goto erret;

    if (sfxp->id.choice)              /* feat-id present */
    {
        if (! SeqFeatIdAsnWrite(&sfxp->id, aip, SEQFEATXREF_id))
            goto erret;
    }

	if (! SeqFeatDataAsnWrite(&sfxp->data, aip, SEQFEATXREF_data)) goto erret;

    if (! AsnCloseStruct(aip, atp, (Pointer)sfxp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqFeatXrefAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
SeqFeatXrefPtr LIBCALL SeqFeatXrefAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    SeqFeatXrefPtr sfxp;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return (SeqFeatXrefPtr)NULL;
	}

	if (aip == NULL)
		return (SeqFeatXrefPtr)NULL;

	if (orig == NULL)           /* SeqFeatXref ::= (self contained) */
		atp = AsnReadId(aip, amp, SEQFEATXREF);
	else
		atp = AsnLinkType(orig, SEQFEATXREF);    /* link in local tree */
    if (atp == NULL)
        return (SeqFeatXrefPtr)NULL;

	sfxp = SeqFeatXrefNew();
    if (sfxp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;

    if (atp == SEQFEATXREF_id)
    {
        if (! SeqFeatIdAsnRead(aip, atp, &sfxp->id))
            goto erret;
	    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }

	if (! SeqFeatDataAsnRead(aip, atp, &sfxp->data))
		goto erret;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return sfxp;
erret:
    sfxp = SeqFeatXrefFree(sfxp);
    goto ret;
}

/*****************************************************************************
*
*   CdRegionNew()
*
*****************************************************************************/
CdRegionPtr LIBCALL CdRegionNew (void)
{
    return (CdRegionPtr)MemNew(sizeof(CdRegion));
}

/*****************************************************************************
*
*   CdRegionFree(cdp)
*       Frees a CdRegion and associated data
*
*****************************************************************************/
CdRegionPtr LIBCALL CdRegionFree (CdRegionPtr cdp)
{
    if (cdp == NULL)
        return cdp;

    GeneticCodeFree(cdp->genetic_code);
    CodeBreakFree(cdp->code_break);
    return (CdRegionPtr)MemFree(cdp);
}

/*****************************************************************************
*
*   CdRegionAsnWrite(cdp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (CdRegion ::=)
*
*****************************************************************************/
Boolean LIBCALL CdRegionAsnWrite (CdRegionPtr cdp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    CodeBreakPtr cbp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, CDREGION);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (cdp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)cdp))
        goto erret;
    
    if (cdp->orf)
    {
        av.boolvalue = cdp->orf;
        if (! AsnWrite(aip, CDREGION_orf, &av)) goto erret;
    }
    if (cdp->frame)
    {
        av.intvalue = cdp->frame;
        if (! AsnWrite(aip, CDREGION_frame, &av)) goto erret;
    }
    if (cdp->conflict)
    {
        av.boolvalue = cdp->conflict;
        if (! AsnWrite(aip, CDREGION_conflict, &av)) goto erret;
    }
    if (cdp->gaps)
    {
        av.intvalue = cdp->gaps;
        if (! AsnWrite(aip, CDREGION_gaps, &av)) goto erret;
    }
    if (cdp->mismatch)
    {
        av.intvalue = cdp->mismatch;
        if (! AsnWrite(aip, CDREGION_mismatch, &av)) goto erret;
    }
	if (cdp->genetic_code != NULL)
	{
		if (! GeneticCodeAsnWrite(cdp->genetic_code, aip, CDREGION_code))
			goto erret;
	}
    if (cdp->code_break != NULL)
    {
        if (! AsnOpenStruct(aip, CDREGION_code_break, (Pointer)cdp->code_break))
            goto erret;
        cbp = cdp->code_break;
        while (cbp != NULL)
        {
            if (! CodeBreakAsnWrite(cbp, aip, CDREGION_code_break_E))
                goto erret;
            cbp = cbp->next;
        }
        if (! AsnCloseStruct(aip, CDREGION_code_break, (Pointer)cdp->code_break))
            goto erret;
    }
    if (cdp->stops)
    {
        av.intvalue = cdp->stops;
        if (! AsnWrite(aip, CDREGION_stops, &av)) goto erret;
    }

    if (! AsnCloseStruct(aip, atp, (Pointer)cdp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   CdRegionAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
CdRegionPtr LIBCALL CdRegionAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    CdRegionPtr cdp;
    CodeBreakPtr curr, next;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return (CdRegionPtr)NULL;
	}

	if (aip == NULL)
		return (CdRegionPtr)NULL;

	if (orig == NULL)           /* CdRegion ::= (self contained) */
		atp = AsnReadId(aip, amp, CDREGION);
	else
		atp = AsnLinkType(orig, CDREGION);    /* link in local tree */
    oldatp = atp;
    if (atp == NULL)
        return (CdRegionPtr)NULL;

	cdp = CdRegionNew();
    if (cdp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */
    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL)
            goto erret;
        if (atp == CDREGION_code)
        {
            cdp->genetic_code = GeneticCodeAsnRead(aip, atp);
			if (cdp->genetic_code == NULL)
				goto erret;
        }
        else
        {
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
            if (atp == CDREGION_orf)
                cdp->orf = av.boolvalue;
            else if (atp == CDREGION_conflict)
                cdp->conflict = av.boolvalue;
            else if (atp == CDREGION_gaps)
            {
                if (av.intvalue > 255)
                    av.intvalue = 255;
                cdp->gaps = (Uint1)av.intvalue;
            }
            else if (atp == CDREGION_frame)
            {
                if (av.intvalue > 255)
                    av.intvalue = 255;
                cdp->frame = (Uint1)av.intvalue;
            }
            else if (atp == CDREGION_mismatch)
            {
                if (av.intvalue > 255)
                    av.intvalue = 255;
                cdp->mismatch = (Uint1)av.intvalue;
            }
            else if (atp == CDREGION_stops)
            {
                if (av.intvalue > 255)
                    av.intvalue = 255;
                cdp->stops = (Uint1)av.intvalue;
            }
            else if (atp == CDREGION_code_break)
            {
				curr = NULL;
                while ((atp = AsnReadId(aip, amp, atp)) == CDREGION_code_break_E)
                {
                    next = CodeBreakAsnRead(aip, atp);
                    if (next == NULL)
                        goto erret;
                    if (cdp->code_break == NULL)
                        cdp->code_break = next;
                    else
                        curr->next = next;
                    curr = next;
                }
                if (atp == NULL)
                    goto erret;
                if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
            }

        }
    }

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return cdp;
erret:
    cdp = CdRegionFree(cdp);
    goto ret;
}

static GeneticCodePtr genetic_codes;    /* array of ValNodes */
static Boolean GeneticCodeAdd PROTO((GeneticCodePtr gc));

/*****************************************************************************
*
*   GeneticCodeNew()
*   	returns a ValNodePtr for the head of the chain
*   	first ValNode has the choice 254, 
*       The chain of GeneticCode elements is attached to data.ptrvalue of
*         this first node.
*
*****************************************************************************/
GeneticCodePtr LIBCALL GeneticCodeNew ()
{
	GeneticCodePtr gc;

	gc = ValNodeNew(NULL);
	if (gc == NULL) return gc;
	gc->choice = 254;

	return gc;
}

/*****************************************************************************
*
*   GeneticCodeAdd()
*       Adds to genetic_codes global list
*   	This should only be used by GeneticCodeTableRead()
*
*****************************************************************************/
static Boolean GeneticCodeAdd (GeneticCodePtr gc)
{
	GeneticCodePtr tmp;

	if (gc == NULL)
		return FALSE;

	if (genetic_codes == NULL)
		genetic_codes = gc;
	else
	{
		tmp = genetic_codes;
		while (tmp->next != NULL)
			tmp = tmp->next;
		tmp->next = gc;
	}

	return TRUE;
}

/*****************************************************************************
*
*   GeneticCodeFree(gcp)
*       Frees a GeneticCode and associated data
*
*****************************************************************************/
GeneticCodePtr LIBCALL GeneticCodeFree (GeneticCodePtr gcp)
{
	ValNodePtr prev, next;

	if (gcp == NULL)
		return (GeneticCodePtr)NULL;

	if (gcp->choice == 254)   /* head of chain */
	{
		next = genetic_codes;
		prev = NULL;
		while ((next != NULL) && (gcp != next))
		{
			prev = next;
			next = prev->next;
		}

		if (next != NULL)      /* found it */
		{
			if (prev != NULL)   /* not the first one */
				prev->next = next->next;
			else
				genetic_codes = next->next;
			gcp->next = NULL;
		}

		prev = gcp;
		gcp = (GeneticCodePtr) prev->data.ptrvalue;
		MemFree(prev);
	}

	while (gcp != NULL)
	{
		next = gcp->next;
		switch (gcp->choice)
		{
			case 1:   /* name */
			case 3:   /* ncbieaa */
			case 6:   /* sncbieaa */
				MemFree(gcp->data.ptrvalue);
				break;
			case 4:   /* ncbi8aa */
			case 5:	  /* ncbistdaa */
			case 7:   /* sncbi8aa */
			case 8:   /* sncbistdaa */
				BSFree((ByteStorePtr)gcp->data.ptrvalue);
				break;
			default:
				break;
		}
		MemFree(gcp);
		gcp = next;
	}
	return (GeneticCodePtr)NULL;
}

/*****************************************************************************
*
*   GeneticCodeAsnWrite(gcp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (GeneticCode ::=)
*
*****************************************************************************/
Boolean LIBCALL GeneticCodeAsnWrite (GeneticCodePtr gcp, AsnIoPtr aip, AsnTypePtr orig)
{
	GeneticCodePtr tmp;
	DataVal av;
	AsnTypePtr atp, gtp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, GENETIC_CODE);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (gcp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)gcp))
        goto erret;

	tmp = (ValNodePtr) gcp->data.ptrvalue;
	while (tmp != NULL)
	{   
		switch (tmp->choice)
		{
			case 1:   /* name */
				gtp = GENETIC_CODE_E_name;
				break;
			case 2:
				gtp = GENETIC_CODE_E_id;
				break;
			case 3:   /* ncbieaa */
				gtp = GENETIC_CODE_E_ncbieaa;
				break;
			case 4:	  /* ncbi8aa */
				gtp = GENETIC_CODE_E_ncbi8aa;
				break;
			case 5:   /* ncbistdaa */
				gtp = GENETIC_CODE_E_ncbistdaa;
				break;
			case 6:   /* sncbieaa */
				gtp = GENETIC_CODE_E_sncbieaa;
				break;
			case 7:	  /* sncbi8aa */
				gtp = GENETIC_CODE_E_sncbi8aa;
				break;
			case 8:   /* sncbistdaa */
				gtp = GENETIC_CODE_E_sncbistdaa;
				break;
			default:
				ErrPost(CTX_NCBIOBJ, 1, "Unsupported GeneticCode Type = %d", (int)gcp->choice);
				goto erret;
		}
		
		av.ptrvalue = tmp;
    	if (! AsnWriteChoice(aip, GENETIC_CODE_E, (Int2)tmp->choice, &av)) goto erret;
		if (! AsnWrite(aip, gtp, &tmp->data)) goto erret;
		tmp = tmp->next;
	}

    if (! AsnCloseStruct(aip, atp, (Pointer)gcp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}
/*****************************************************************************
*
*   GeneticCodePtr GeneticCodeAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
GeneticCodePtr LIBCALL GeneticCodeAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    GeneticCodePtr gcp = NULL, tmp = NULL;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return 0;
	}

	if (aip == NULL)
		return 0;

	if (orig == NULL)           /* GeneticCode ::= (self contained) */
		atp = AsnReadId(aip, amp, GENETIC_CODE);
	else
		atp = AsnLinkType(orig, GENETIC_CODE);    /* link in local tree */
    if (atp == NULL)
        return 0;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */

	gcp = GeneticCodeNew();
	if (gcp == NULL) goto erret;

    while((atp = AsnReadId(aip, amp, atp)) == GENETIC_CODE_E)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;    /* what kind? */

		tmp = ValNodeNew(tmp);
		if (tmp == NULL) goto erret;
		if (gcp->data.ptrvalue == NULL)
			gcp->data.ptrvalue = tmp;

        if (AsnReadVal(aip, atp, &tmp->data) <= 0) goto erret;

        if (atp == GENETIC_CODE_E_name)
            tmp->choice = 1;
        else if (atp == GENETIC_CODE_E_id)
            tmp->choice = 2;
        else if (atp == GENETIC_CODE_E_ncbieaa)
            tmp->choice = 3;
        else if (atp == GENETIC_CODE_E_ncbi8aa)
            tmp->choice = 4;
		else if (atp == GENETIC_CODE_E_ncbistdaa)
            tmp->choice = 5;
        else if (atp == GENETIC_CODE_E_sncbieaa)
            tmp->choice = 6;
        else if (atp == GENETIC_CODE_E_sncbi8aa)
            tmp->choice = 7;
		else if (atp == GENETIC_CODE_E_sncbistdaa)
            tmp->choice = 8;
		else
			tmp->choice = 255;   /* unrecognized */
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
	goto ret;

erret:
	gcp = GeneticCodeFree(gcp);
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return gcp;
}

/*****************************************************************************
*
*   GeneticCodePtr GeneticCodeTableLoad()
*       loads all current genetic codes
*       looks for "gc.val" in the "data" directory
*
*****************************************************************************/
GeneticCodePtr LIBCALL GeneticCodeTableLoad (void)
{
    Char buf[80];
    AsnIoPtr aip;

	if (genetic_codes != NULL)
		return genetic_codes;

    if (! FindPath("ncbi", "ncbi", "data", buf, sizeof (buf)))
	{
		ErrPost(CTX_NCBIOBJ, 1, "FindPath failed in GeneticCodeTableLoad");
        return genetic_codes;
	}

    StringCat(buf, "gc.val");
    if ((aip = AsnIoOpen(buf, "rb")) == NULL)
	{
		ErrPost(CTX_NCBIOBJ, 1, "Couldn't open [%s]", buf);
        return genetic_codes;
	}

    GeneticCodeTableAsnRead(aip, NULL);

    AsnIoClose(aip);
    return genetic_codes;
}

/*****************************************************************************
*
*   GeneticCodeFind (id, name)
*   	if (name == NULL)  finds by id
*       returns NULL, if can't find anything
*
*****************************************************************************/
GeneticCodePtr LIBCALL GeneticCodeFind (Int4 id, CharPtr name)
{
	GeneticCodePtr gcp;
	ValNodePtr tmp;

	if (GeneticCodeTableLoad() == NULL)
		return (GeneticCodePtr)NULL;

	for (gcp = genetic_codes; gcp != NULL; gcp = gcp->next)
	{
		for (tmp = (ValNodePtr) gcp->data.ptrvalue; tmp != NULL; tmp = tmp->next)
		{
			switch (tmp->choice)
			{
				case 1:   /* name */
					if (name != NULL)
					{
						if (! StringICmp(name, (CharPtr)tmp->data.ptrvalue))
							return gcp;
					}
					break;
				case 2:   /* id */
					if (name == NULL)
					{
						if (id == tmp->data.intvalue)
							return gcp;
					}
					break;
				default:
					break;
			}
		}
	}

	return (GeneticCodePtr)NULL;
}


/*****************************************************************************
*
*   GeneticCodeTableAsnWrite(gcp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (Genetic-code-table ::=)
*
*****************************************************************************/
Boolean LIBCALL GeneticCodeTableAsnWrite (GeneticCodePtr gcp, AsnIoPtr aip, AsnTypePtr orig)
{
	GeneticCodePtr tmp;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, GENETIC_CODE_TABLE);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (gcp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)gcp))
        goto erret;
    
	tmp = gcp;
	while (tmp != NULL)
	{
		if (! GeneticCodeAsnWrite(tmp, aip, GENETIC_CODE_TABLE_E))
			goto erret;
		tmp = tmp->next;
	}

    if (! AsnCloseStruct(aip, atp, (Pointer)gcp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   GeneticCodePtr GeneticCodeTableAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
GeneticCodePtr LIBCALL GeneticCodeTableAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    GeneticCodePtr gcp = NULL, tmp = NULL;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return 0;
	}

	if (aip == NULL)
		return 0;

	if (orig == NULL)           /* Genetic-code-table ::= (self contained) */
		atp = AsnReadId(aip, amp, GENETIC_CODE_TABLE);
	else
		atp = AsnLinkType(orig, GENETIC_CODE_TABLE);    /* link in local tree */
    if (atp == NULL)
        return 0;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */

    while((atp = AsnReadId(aip, amp, atp)) == GENETIC_CODE_TABLE_E)
    {
		if ((tmp = GeneticCodeAsnRead(aip, atp)) == NULL) goto erret;
		if (gcp == NULL)     /* keep first one */
			gcp = tmp;
		GeneticCodeAdd(tmp);   /* add to global genetic_codes */
	}
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
	goto ret;

erret:
	while (gcp != NULL)
	{
		tmp = gcp->next;
		gcp = GeneticCodeFree(gcp);
		gcp = tmp;
	}
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return gcp;
}

/*****************************************************************************
*
*   CodeBreakNew()
*
*****************************************************************************/
CodeBreakPtr LIBCALL CodeBreakNew (void)
{
    return (CodeBreakPtr)MemNew(sizeof(CodeBreak));
}

/*****************************************************************************
*
*   CodeBreakFree(cbp)
*       Frees a chain of CodeBreak and associated data
*
*****************************************************************************/
CodeBreakPtr LIBCALL CodeBreakFree (CodeBreakPtr cbp)
{
    CodeBreakPtr next;

    while (cbp != NULL)
    {
        next = cbp->next;
        SeqLocFree(cbp->loc);
        MemFree(cbp);
        cbp = next;
    }
    return (CodeBreakPtr)NULL;
}

/*****************************************************************************
*
*   CodeBreakAsnWrite(cbp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (CodeBreak ::=)
*
*****************************************************************************/
Boolean LIBCALL CodeBreakAsnWrite (CodeBreakPtr cbp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, tmp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, CODE_BREAK);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (cbp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)cbp))
        goto erret;

    if (! SeqLocAsnWrite(cbp->loc, aip, CODE_BREAK_loc))
        goto erret;
    av.intvalue = cbp->aa.value.intvalue;
    if (! AsnWriteChoice(aip, CODE_BREAK_aa, (Int2)cbp->aa.choice, &av)) goto erret;
    switch(cbp->aa.choice)
    {
        case 1:
            tmp = CODE_BREAK_aa_ncbieaa;
            break;
        case 2:
            tmp = CODE_BREAK_aa_ncbi8aa;
            break;
        case 3:
            tmp = CODE_BREAK_aa_ncbistdaa;
            break;
    }
    if (! AsnWrite(aip, tmp, &av)) goto erret;
    if (! AsnCloseStruct(aip, atp, (Pointer)cbp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   CodeBreakAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
CodeBreakPtr LIBCALL CodeBreakAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    CodeBreakPtr cbp;
    Uint1 choice = 0;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return (CodeBreakPtr)NULL;
	}

	if (aip == NULL)
		return (CodeBreakPtr)NULL;

	if (orig == NULL)           /* CodeBreak ::= (self contained) */
		atp = AsnReadId(aip, amp, CODE_BREAK);
	else
		atp = AsnLinkType(orig, CODE_BREAK);    /* link in local tree */
    if (atp == NULL)
        return (CodeBreakPtr)NULL;

	cbp = CodeBreakNew();
    if (cbp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;   /* location */
    cbp->loc = SeqLocAsnRead(aip, atp);
    if (cbp->loc == NULL)
        goto erret;
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;    /* read the CHOICE */
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;    /* the aa */
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    cbp->aa.value.intvalue = av.intvalue;
    if (atp == CODE_BREAK_aa_ncbieaa)
        choice = 1;
    else if (atp == CODE_BREAK_aa_ncbi8aa)
        choice = 2;
    else if (atp == CODE_BREAK_aa_ncbistdaa)
        choice = 3;
    cbp->aa.choice = choice;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return cbp;
erret:
    cbp = CodeBreakFree(cbp);
    goto ret;
}

/*****************************************************************************
*
*   ImpFeatNew()
*
*****************************************************************************/
ImpFeatPtr LIBCALL ImpFeatNew (void)
{
    return (ImpFeatPtr)MemNew(sizeof(ImpFeat));
}

/*****************************************************************************
*
*   ImpFeatFree(ifp)
*       Frees an ImpFeat and associated data
*
*****************************************************************************/
ImpFeatPtr LIBCALL ImpFeatFree (ImpFeatPtr ifp)
{
    if (ifp == NULL)
        return (ImpFeatPtr)NULL;

    MemFree(ifp->key);
    MemFree(ifp->loc);
    MemFree(ifp->descr);
    return (ImpFeatPtr)MemFree(ifp);
}

/*****************************************************************************
*
*   ImpFeatAsnWrite(ifp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (ImpFeat ::=)
*
*****************************************************************************/
Boolean LIBCALL ImpFeatAsnWrite (ImpFeatPtr ifp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, IMP_FEAT);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (ifp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)ifp))
        goto erret;

    av.ptrvalue = ifp->key;
    if (! AsnWrite(aip, IMP_FEAT_key, &av)) goto erret;
    if (ifp->loc != NULL)
    {
        av.ptrvalue = ifp->loc;
        if (! AsnWrite(aip, IMP_FEAT_loc, &av)) goto erret;
    }
    if (ifp->descr != NULL)
    {
        av.ptrvalue = ifp->descr;
        if (! AsnWrite(aip, IMP_FEAT_descr, &av)) goto erret;
    }
    if (! AsnCloseStruct(aip, atp, (Pointer)ifp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   ImpFeatAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
ImpFeatPtr LIBCALL ImpFeatAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    ImpFeatPtr ifp;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return (ImpFeatPtr)NULL;
	}

	if (aip == NULL)
		return (ImpFeatPtr)NULL;

	if (orig == NULL)           /* ImpFeat ::= (self contained) */
		atp = AsnReadId(aip, amp, IMP_FEAT);
	else
		atp = AsnLinkType(orig, IMP_FEAT);    /* link in local tree */
    oldatp = atp;

    if (atp == NULL)
        return (ImpFeatPtr)NULL;

	ifp = ImpFeatNew();
    if (ifp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */

    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL)
            goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        if (atp == IMP_FEAT_key)
            ifp->key = (CharPtr)av.ptrvalue;
        else if (atp == IMP_FEAT_loc)
            ifp->loc = (CharPtr)av.ptrvalue;
        else if (atp == IMP_FEAT_descr)
            ifp->descr = (CharPtr)av.ptrvalue;
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return ifp;
erret:
    ifp = ImpFeatFree(ifp);
    goto ret;
}

/*****************************************************************************
*
*   RnaRefNew()
*
*****************************************************************************/
RnaRefPtr LIBCALL RnaRefNew (void)
{
    return (RnaRefPtr)MemNew(sizeof(RnaRef));
}

/*****************************************************************************
*
*   RnaRefFree(rrp)
*       Frees an RnaRef and associated data
*
*****************************************************************************/
RnaRefPtr LIBCALL RnaRefFree (RnaRefPtr rrp)
{
    if (rrp == NULL)
        return (RnaRefPtr)NULL;
    
    if (rrp->ext.choice == 1)  /* name */
        MemFree(rrp->ext.value.ptrvalue);
    else if (rrp->ext.choice == 2)   /* tRNA */
        MemFree(rrp->ext.value.ptrvalue);
    return (RnaRefPtr)MemFree(rrp);
}

/*****************************************************************************
*
*   RnaRefAsnWrite(rrp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (RnaRef ::=)
*
*****************************************************************************/
Boolean LIBCALL RnaRefAsnWrite (RnaRefPtr rrp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, tmp;
    tRNAPtr trna;
    Int2 i;
    Boolean first = TRUE, retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, RNA_REF);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (rrp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)rrp))
        goto erret;

    av.intvalue = rrp->type;
    if (! AsnWrite(aip, RNA_REF_type, &av)) goto erret;
    if (rrp->pseudo)
    {
        av.boolvalue = rrp->pseudo;
        if (! AsnWrite(aip, RNA_REF_pseudo, &av)) goto erret;
    }
    if (rrp->ext.choice)
    {
		av.ptrvalue = rrp->ext.value.ptrvalue;
        if (! AsnWriteChoice(aip, RNA_REF_ext, (Int2)rrp->ext.choice, &av)) goto erret;
        if (rrp->ext.choice == 1)   /* name */
        {
            if (! AsnWrite(aip, RNA_REF_ext_name, &av)) goto erret;
        }
        else if (rrp->ext.choice == 2)   /* trna */
        {
            trna = (tRNAPtr)rrp->ext.value.ptrvalue;
            if (! AsnOpenStruct(aip, RNA_REF_ext_tRNA, (Pointer)trna))
                goto erret;
			if (trna->aatype)
			{
	            av.intvalue = trna->aa;
    	        if (! AsnWriteChoice(aip, TRNA_EXT_aa, (Int2)trna->aatype, &av)) goto erret;
        	    switch (trna->aatype)
            	{
                	case 1:
                    	tmp = TRNA_EXT_aa_iupacaa;
	                    break;
    	            case 2:
        	            tmp = TRNA_EXT_aa_ncbieaa;
            	        break;
                	case 3:
	                    tmp = TRNA_EXT_aa_ncbi8aa;
    	                break;
                	case 4:
	                    tmp = TRNA_EXT_aa_ncbistdaa;
    	                break;
        	    }
            	if (! AsnWrite(aip, tmp, &av)) goto erret;
			}
            for (i = 0; i < 6; i++)
            {
                if (trna->codon[i] != 255)
                {
                    if (first)
                    {
                        if (! AsnOpenStruct(aip, TRNA_EXT_codon, (Pointer)trna->codon))
                            goto erret;
                        first = FALSE;
                    }
                    av.intvalue = trna->codon[i];
                    if (! AsnWrite(aip, TRNA_EXT_codon_E, &av)) goto erret;
                }
            }
            if (! first)    /* wrote one */
                if (! AsnCloseStruct(aip, TRNA_EXT_codon, (Pointer)trna->codon))
                    goto erret;
            if (! AsnCloseStruct(aip, RNA_REF_ext_tRNA, (Pointer)trna))
                goto erret;
        }
    }
    if (! AsnCloseStruct(aip, atp, (Pointer)rrp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   RnaRefAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
RnaRefPtr LIBCALL RnaRefAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    RnaRefPtr rrp;
    tRNAPtr trna = NULL;
    Int2 i;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return (RnaRefPtr)NULL;
	}

	if (aip == NULL)
		return (RnaRefPtr)NULL;

	if (orig == NULL)           /* RnaRef ::= (self contained) */
		atp = AsnReadId(aip, amp, RNA_REF);
	else
		atp = AsnLinkType(orig, RNA_REF);    /* link in local tree */
    oldatp = atp;
    if (atp == NULL)
        return (RnaRefPtr)NULL;

	rrp = RnaRefNew();
    if (rrp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */

    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL)
            goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        if (atp == RNA_REF_type)
            rrp->type = (Uint1)av.intvalue;
        else if (atp == RNA_REF_pseudo)
            rrp->pseudo = av.boolvalue;
        else if (atp == RNA_REF_ext_name)
        {
            rrp->ext.choice = 1;
            rrp->ext.value.ptrvalue = av.ptrvalue;
        }
        else if (atp == RNA_REF_ext_tRNA)   /* its a trna */
        {
			if (trna == NULL)   /* first time */
			{
	            rrp->ext.choice = 2;
    	        trna = (tRNAPtr)MemNew(sizeof(tRNA));
        	    if (trna == NULL)
            	    goto erret;
	            rrp->ext.value.ptrvalue = trna;
    	        for (i = 0; i < 6; i++)
        	        trna->codon[i] = 255;   /* not used */
            	i = 0;
			}
        }
        else if (atp == TRNA_EXT_aa_iupacaa)
        {
            trna->aatype = 1;
            trna->aa = (Uint1)av.intvalue;
        }
        else if (atp == TRNA_EXT_aa_ncbieaa)
        {
            trna->aatype = 2;
            trna->aa = (Uint1)av.intvalue;
        }
        else if (atp == TRNA_EXT_aa_ncbi8aa)
        {
            trna->aatype = 3;
            trna->aa = (Uint1)av.intvalue;
        }
        else if (atp == TRNA_EXT_aa_ncbistdaa)
        {
            trna->aatype = 4;
            trna->aa = (Uint1)av.intvalue;
        }
        else if (atp == TRNA_EXT_codon_E)
        {
            trna->codon[i] = (Uint1)av.intvalue;
            i++;
        }
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return rrp;
erret:
    rrp = RnaRefFree(rrp);
    goto ret;
}

/*****************************************************************************
*
*   GeneRefNew()
*
*****************************************************************************/
GeneRefPtr LIBCALL GeneRefNew (void)
{
    return (GeneRefPtr)MemNew(sizeof(GeneRef));
}

/*****************************************************************************
*
*   GeneRefFree(grp)
*       Frees an GeneRef and associated data
*
*****************************************************************************/
GeneRefPtr LIBCALL GeneRefFree (GeneRefPtr grp)
{
    ValNodePtr anp, next;

    if (grp == NULL)
        return (GeneRefPtr)NULL;

    MemFree(grp->locus);
    MemFree(grp->allele);
    MemFree(grp->desc);
    MemFree(grp->maploc);
    anp = grp->db;
    while (anp != NULL)
    {
        next = anp->next;
        DbtagFree((DbtagPtr)anp->data.ptrvalue);
        MemFree(anp);
        anp = next;
    }
    anp = grp->syn;
    while (anp != NULL)
    {
        next = anp->next;
        MemFree(anp->data.ptrvalue);
        MemFree(anp);
        anp = next;
    }
    return (GeneRefPtr)MemFree(grp);
}

/*****************************************************************************
*
*   GeneRefDup(grp)
*       Duplicates an GeneRef and associated data
*
*****************************************************************************/
GeneRefPtr LIBCALL GeneRefDup (GeneRefPtr grp)
{
    ValNodePtr anp, vnp;
	GeneRefPtr ngp;

    if (grp == NULL)
        return (GeneRefPtr)NULL;

	ngp = GeneRefNew();
	if (ngp == NULL) return ngp;

	if (grp->locus != NULL)
		ngp->locus = StringSave(grp->locus);
	if (grp->allele != NULL)
		ngp->allele = StringSave(grp->allele);
	if (grp->desc != NULL)
		ngp->desc = StringSave(grp->desc);
	if (grp->maploc != NULL)
		ngp->maploc = StringSave(grp->maploc);
    anp = grp->db;
    while (anp != NULL)
    {
		vnp = ValNodeNew(ngp->db);
		if (vnp == NULL) return GeneRefFree(ngp);
		if (ngp->db == NULL)
			ngp->db = vnp;
        vnp->data.ptrvalue = (Pointer)DbtagDup((DbtagPtr)anp->data.ptrvalue);
        anp = anp->next;
    }
    anp = grp->syn;
    while (anp != NULL)
    {
		vnp = ValNodeNew(ngp->syn);
		if (vnp == NULL) return GeneRefFree(ngp);
		if (ngp->syn == NULL)
			ngp->syn = vnp;
        vnp->data.ptrvalue = (Pointer)StringSave((CharPtr)anp->data.ptrvalue);
        anp = anp->next;
    }
    return ngp;
}

/*****************************************************************************
*
*   GeneRefAsnWrite(grp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (GeneRef ::=)
*
*****************************************************************************/
Boolean LIBCALL GeneRefAsnWrite (GeneRefPtr grp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    ValNodePtr anp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, GENE_REF);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (grp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)grp))
        goto erret;

    if (grp->locus != NULL)
    {
        av.ptrvalue = grp->locus;
        if (! AsnWrite(aip, GENE_REF_locus, &av)) goto erret;
    }
    if (grp->allele != NULL)
    {
        av.ptrvalue = grp->allele;
        if (! AsnWrite(aip, GENE_REF_allele, &av)) goto erret;
    }
    if (grp->desc != NULL)
    {
        av.ptrvalue = grp->desc;
        if (! AsnWrite(aip, GENE_REF_desc, &av)) goto erret;
    }
    if (grp->maploc != NULL)
    {
        av.ptrvalue = grp->maploc;
        if (! AsnWrite(aip, GENE_REF_maploc, &av)) goto erret;
    }
    if (grp->pseudo)
    {
        av.boolvalue = grp->pseudo;
        if (! AsnWrite(aip, GENE_REF_pseudo, &av)) goto erret;
    }
    if (grp->db != NULL)
    {
        anp = grp->db;
        if (! AsnOpenStruct(aip, GENE_REF_db, (Pointer)grp->db))
            goto erret;
        while (anp != NULL)
        {
            if (! DbtagAsnWrite((DbtagPtr)anp->data.ptrvalue, aip, GENE_REF_db_E))
                goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, GENE_REF_db, (Pointer)grp->db))
            goto erret;
    }
    if (grp->syn != NULL)
    {
        anp = grp->syn;
        if (! AsnOpenStruct(aip, GENE_REF_syn, (Pointer)grp->syn))
            goto erret;
        while (anp != NULL)
        {
            if (! AsnWrite(aip, GENE_REF_syn_E, &anp->data)) goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, GENE_REF_syn, (Pointer)grp->syn))
            goto erret;
    }
    if (! AsnCloseStruct(aip, atp, (Pointer)grp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   GeneRefAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
GeneRefPtr LIBCALL GeneRefAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    GeneRefPtr grp;
    ValNodePtr anp;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return (GeneRefPtr)NULL;
	}

	if (aip == NULL)
		return (GeneRefPtr)NULL;

	if (orig == NULL)           /* GeneRef ::= (self contained) */
		atp = AsnReadId(aip, amp, GENE_REF);
	else
		atp = AsnLinkType(orig, GENE_REF);    /* link in local tree */
    oldatp = atp;
    if (atp == NULL)
        return (GeneRefPtr)NULL;

	grp = GeneRefNew();
    if (grp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */

    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL)
            goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        if (atp == GENE_REF_locus)
            grp->locus = (CharPtr)av.ptrvalue;
        else if (atp == GENE_REF_allele)
            grp->allele = (CharPtr)av.ptrvalue;
        else if (atp == GENE_REF_desc)
            grp->desc = (CharPtr)av.ptrvalue;
        else if (atp == GENE_REF_maploc)
            grp->maploc = (CharPtr)av.ptrvalue;
        else if (atp == GENE_REF_pseudo)
            grp->pseudo = av.boolvalue;
        else if (atp == GENE_REF_db)
        {
            while ((atp = AsnReadId(aip, amp, atp)) == GENE_REF_db_E)
            {
                anp = ValNodeNew(grp->db);
                if (anp == NULL)
                    goto erret;
                if (grp->db == NULL)
                    grp->db = anp;
                anp->data.ptrvalue = (Pointer)DbtagAsnRead(aip, atp);
                if (anp->data.ptrvalue == NULL)
                    goto erret;
            }
            if (atp == NULL)
                goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;  /* end SET OF */
        }
        else if (atp == GENE_REF_syn)
        {
            while ((atp = AsnReadId(aip, amp, atp)) == GENE_REF_syn_E)
            {
                anp = ValNodeNew(grp->syn);
                if (anp == NULL)
                    goto erret;
                if (grp->syn == NULL)
                    grp->syn = anp;
                if (AsnReadVal(aip, atp, &anp->data) <= 0) goto erret;
            }
            if (atp == NULL)
                goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;  /* end SET OF */
        }
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return grp;
erret:
    grp = GeneRefFree(grp);
    goto ret;
}

/*****************************************************************************
*
*   OrgRefNew()
*
*****************************************************************************/
OrgRefPtr LIBCALL OrgRefNew (void)
{
    return (OrgRefPtr)MemNew(sizeof(OrgRef));
}

/*****************************************************************************
*
*   OrgRefFree(orp)
*       Frees an OrgRef and associated data
*
*****************************************************************************/
OrgRefPtr LIBCALL OrgRefFree (OrgRefPtr orp)
{
    ValNodePtr anp, next;

    if (orp == NULL)
        return (OrgRefPtr)NULL;

    MemFree(orp->taxname);
    MemFree(orp->common);
    ValNodeFreeData(orp->mod);
    anp = orp->db;
    while (anp != NULL)
    {
        next = anp->next;
        DbtagFree((DbtagPtr)anp->data.ptrvalue);
        MemFree(anp);
        anp = next;
    }
    ValNodeFreeData(orp->syn);
    return (OrgRefPtr)MemFree(orp);
}

/*****************************************************************************
*
*   OrgRefAsnWrite(orp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (OrgRef ::=)
*
*****************************************************************************/
Boolean LIBCALL OrgRefAsnWrite (OrgRefPtr orp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    ValNodePtr anp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, ORG_REF);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (orp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)orp))
        goto erret;

    if (orp->taxname != NULL)
    {
        av.ptrvalue = orp->taxname;
        if (! AsnWrite(aip, ORG_REF_taxname, &av)) goto erret;
    }
    if (orp->common != NULL)
    {
        av.ptrvalue = orp->common;
        if (! AsnWrite(aip, ORG_REF_common, &av)) goto erret;
    }
    if (orp->mod != NULL)
    {
        anp = orp->mod;
        if (! AsnOpenStruct(aip, ORG_REF_mod, (Pointer)orp->mod))
            goto erret;
        while (anp != NULL)
        {
            av.ptrvalue = anp->data.ptrvalue;
            if (! AsnWrite(aip, ORG_REF_mod_E, &av)) goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, ORG_REF_mod, (Pointer)orp->mod))
            goto erret;
    }
    if (orp->db != NULL)
    {
        anp = orp->db;
        if (! AsnOpenStruct(aip, ORG_REF_db, (Pointer)orp->db))
            goto erret;
        while (anp != NULL)
        {
            if (! DbtagAsnWrite((DbtagPtr)anp->data.ptrvalue, aip, ORG_REF_db_E))
                goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, ORG_REF_db, (Pointer)orp->db))
            goto erret;
    }
    if (orp->syn != NULL)
    {
        anp = orp->syn;
        if (! AsnOpenStruct(aip, ORG_REF_syn, (Pointer)orp->syn))
            goto erret;
        while (anp != NULL)
        {
            av.ptrvalue = anp->data.ptrvalue;
            if (! AsnWrite(aip, ORG_REF_syn_E, &av)) goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, ORG_REF_syn, (Pointer)orp->syn))
            goto erret;
    }
    if (! AsnCloseStruct(aip, atp, (Pointer)orp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   OrgRefAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
OrgRefPtr LIBCALL OrgRefAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    OrgRefPtr orp;
    ValNodePtr anp;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return (OrgRefPtr)NULL;
	}

	if (aip == NULL)
		return (OrgRefPtr)NULL;

	if (orig == NULL)           /* OrgRef ::= (self contained) */
		atp = AsnReadId(aip, amp, ORG_REF);
	else
		atp = AsnLinkType(orig, ORG_REF);    /* link in local tree */
    oldatp = atp;
    if (atp == NULL)
        return (OrgRefPtr)NULL;

	orp = OrgRefNew();
    if (orp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */

    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL)
            goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        if (atp == ORG_REF_taxname)
            orp->taxname = (CharPtr)av.ptrvalue;
        else if (atp == ORG_REF_common)
            orp->common = (CharPtr)av.ptrvalue;
        else if (atp == ORG_REF_mod_E)
        {
            anp = ValNodeNew(orp->mod);
            if (anp == NULL)
                goto erret;
            anp->data.ptrvalue = av.ptrvalue;
            if (orp->mod == NULL)
                orp->mod = anp;
        }
        else if (atp == ORG_REF_db)
        {
            while ((atp = AsnReadId(aip, amp, atp)) == ORG_REF_db_E)
            {
                anp = ValNodeNew(orp->db);
                if (anp == NULL)
                    goto erret;
                if (orp->db == NULL)
                    orp->db = anp;
                anp->data.ptrvalue = (Pointer)DbtagAsnRead(aip, atp);
                if (anp->data.ptrvalue == NULL)
                    goto erret;
            }
            if (atp == NULL)
                goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;  /* end SET OF */
        }
        else if (atp == ORG_REF_syn_E)
        {
            anp = ValNodeNew(orp->syn);
            if (anp == NULL)
                goto erret;
            anp->data.ptrvalue = av.ptrvalue;
            if (orp->syn == NULL)
                orp->syn = anp;
        }
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return orp;
erret:
    orp = OrgRefFree(orp);
    goto ret;
}

/*****************************************************************************
*
*   ProtRefNew()
*
*****************************************************************************/
ProtRefPtr LIBCALL ProtRefNew (void)
{
    return (ProtRefPtr)MemNew(sizeof(ProtRef));
}

/*****************************************************************************
*
*   ProtRefFree(prp)
*       Frees an ProtRef and associated data
*
*****************************************************************************/
ProtRefPtr LIBCALL ProtRefFree (ProtRefPtr prp)
{
    ValNodePtr anp, next;

    if (prp == NULL)
        return (ProtRefPtr)NULL;

    MemFree(prp->desc);
    anp = prp->name;
    while (anp != NULL)
    {
        next = anp->next;
        MemFree(anp->data.ptrvalue);
        MemFree(anp);
        anp = next;
    }
    anp = prp->ec;
    while (anp != NULL)
    {
        next = anp->next;
        MemFree(anp->data.ptrvalue);
        MemFree(anp);
        anp = next;
    }
    anp = prp->activity;
    while (anp != NULL)
    {
        next = anp->next;
        MemFree(anp->data.ptrvalue);
        MemFree(anp);
        anp = next;
    }
    anp = prp->db;
    while (anp != NULL)
    {
        next = anp->next;
        DbtagFree((DbtagPtr)anp->data.ptrvalue);
        MemFree(anp);
        anp = next;
    }
    return (ProtRefPtr)MemFree(prp);
}

/*****************************************************************************
*
*   ProtRefDup(prp)
*       Duplicates an ProtRef and associated data
*
*****************************************************************************/
ProtRefPtr LIBCALL ProtRefDup (ProtRefPtr prp)
{
    ValNodePtr anp, vnp;
	ProtRefPtr npp;

    if (prp == NULL)
        return (ProtRefPtr)NULL;

	npp = ProtRefNew();
	if (npp == NULL) return npp;

	if (prp->desc != NULL)
		npp->desc = StringSave(prp->desc);
    anp = prp->name;
    while (anp != NULL)
    {
		vnp = ValNodeNew(npp->name);
		if (vnp == NULL) return ProtRefFree(prp);
		if (npp->name == NULL)
			npp->name = vnp;
		vnp->data.ptrvalue = StringSave((CharPtr)anp->data.ptrvalue);
        anp = anp->next;
    }
    anp = prp->ec;
    while (anp != NULL)
    {
		vnp = ValNodeNew(npp->ec);
		if (vnp == NULL) return ProtRefFree(prp);
		if (npp->ec == NULL)
			npp->ec = vnp;
		vnp->data.ptrvalue = StringSave((CharPtr)anp->data.ptrvalue);
        anp = anp->next;
    }
    anp = prp->activity;
    while (anp != NULL)
    {
		vnp = ValNodeNew(npp->activity);
		if (vnp == NULL) return ProtRefFree(prp);
		if (npp->activity == NULL)
			npp->activity = vnp;
		vnp->data.ptrvalue = StringSave((CharPtr)anp->data.ptrvalue);
        anp = anp->next;
    }
    anp = prp->db;
    while (anp != NULL)
    {
		vnp = ValNodeNew(npp->db);
		if (vnp == NULL) return ProtRefFree(prp);
		if (npp->db == NULL)
			npp->db = vnp;
		vnp->data.ptrvalue = DbtagDup((DbtagPtr)anp->data.ptrvalue);
        anp = anp->next;
    }
    return npp;
}

/*****************************************************************************
*
*   ProtRefAsnWrite(prp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (ProtRef ::=)
*
*****************************************************************************/
Boolean LIBCALL ProtRefAsnWrite (ProtRefPtr prp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    ValNodePtr anp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, PROT_REF);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (prp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)prp))
        goto erret;

    if (prp->name != NULL)
    {
        if (! AsnOpenStruct(aip, PROT_REF_name, (Pointer)prp->name))
            goto erret;
        anp = prp->name;
        while (anp != NULL)
        {
            if (! AsnWrite(aip, PROT_REF_name_E, &anp->data))
                goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, PROT_REF_name, (Pointer)prp->name))
            goto erret;
    }
    if (prp->desc != NULL)
    {
        av.ptrvalue = prp->desc;
        if (! AsnWrite(aip, PROT_REF_desc, &av)) goto erret;
    }
    if (prp->ec != NULL)
    {
        if (! AsnOpenStruct(aip, PROT_REF_ec, (Pointer)prp->ec))
            goto erret;
        anp = prp->ec;
        while (anp != NULL)
        {
            if (! AsnWrite(aip, PROT_REF_ec_E, &anp->data))
                goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, PROT_REF_ec, (Pointer)prp->ec))
            goto erret;
    }
    if (prp->activity != NULL)
    {
        if (! AsnOpenStruct(aip, PROT_REF_activity, (Pointer)prp->activity))
            goto erret;
        anp = prp->activity;
        while (anp != NULL)
        {
            if (! AsnWrite(aip, PROT_REF_activity_E, &anp->data))
                goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, PROT_REF_activity, (Pointer)prp->activity))
            goto erret;
    }
    if (prp->db != NULL)
    {
        anp = prp->db;
        if (! AsnOpenStruct(aip, PROT_REF_db, (Pointer)prp->db))
            goto erret;
        while (anp != NULL)
        {
            if (! DbtagAsnWrite((DbtagPtr)anp->data.ptrvalue, aip, PROT_REF_db_E))
                goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, PROT_REF_db, (Pointer)prp->db))
            goto erret;
    }
    if (! AsnCloseStruct(aip, atp, (Pointer)prp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   ProtRefAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
ProtRefPtr LIBCALL ProtRefAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    ProtRefPtr prp;
    ValNodePtr anp;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return (ProtRefPtr)NULL;
	}

	if (aip == NULL)
		return (ProtRefPtr)NULL;

	if (orig == NULL)           /* ProtRef ::= (self contained) */
		atp = AsnReadId(aip, amp, PROT_REF);
	else
		atp = AsnLinkType(orig, PROT_REF);    /* link in local tree */
    oldatp = atp;
    if (atp == NULL)
        return (ProtRefPtr)NULL;

	prp = ProtRefNew();
    if (prp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */

    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL)
            goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        if (atp == PROT_REF_desc)
            prp->desc = (CharPtr)av.ptrvalue;
        else if (atp == PROT_REF_name)
        {
            while ((atp = AsnReadId(aip, amp, atp)) == PROT_REF_name_E)
            {
                anp = ValNodeNew(prp->name);
                if (anp == NULL)
                    goto erret;
                if (prp->name == NULL)
                    prp->name = anp;
                if (AsnReadVal(aip, atp, &anp->data) <= 0) goto erret;
            }
            if (atp == NULL)
                goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;  /* end SET OF */
        }
        else if (atp == PROT_REF_ec)
        {
            while ((atp = AsnReadId(aip, amp, atp)) == PROT_REF_ec_E)
            {
                anp = ValNodeNew(prp->ec);
                if (anp == NULL)
                    goto erret;
                if (prp->ec == NULL)
                    prp->ec = anp;
                if (AsnReadVal(aip, atp, &anp->data) <= 0) goto erret;
            }
            if (atp == NULL)
                goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;  /* end SET OF */
        }
        else if (atp == PROT_REF_activity)
        {
            while ((atp = AsnReadId(aip, amp, atp)) == PROT_REF_activity_E)
            {
                anp = ValNodeNew(prp->activity);
                if (anp == NULL)
                    goto erret;
                if (prp->activity == NULL)
                    prp->activity = anp;
                if (AsnReadVal(aip, atp, &anp->data) <= 0) goto erret;
            }
            if (atp == NULL)
                goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;  /* end SET OF */
        }
        else if (atp == PROT_REF_db)
        {
            while ((atp = AsnReadId(aip, amp, atp)) == PROT_REF_db_E)
            {
                anp = ValNodeNew(prp->db);
                if (anp == NULL)
                    goto erret;
                if (prp->db == NULL)
                    prp->db = anp;
                anp->data.ptrvalue = (Pointer)DbtagAsnRead(aip, atp);
                if (anp->data.ptrvalue == NULL)
                    goto erret;
            }
            if (atp == NULL)
                goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;  /* end SET OF */
        }
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return prp;
erret:
    prp = ProtRefFree(prp);
    goto ret;
}

/*****************************************************************************
*
*   RsiteRefFree(rrp)
*       Frees an RsiteRef and associated data
*
*****************************************************************************/
RsiteRefPtr LIBCALL RsiteRefFree (RsiteRefPtr rrp)
{
    if (rrp == NULL)
        return (RsiteRefPtr)NULL;

    if (rrp->choice == 1)   /* str */
        MemFree(rrp->data.ptrvalue);
    else
        DbtagFree((DbtagPtr)rrp->data.ptrvalue);
    return (RsiteRefPtr)MemFree(rrp);
}

/*****************************************************************************
*
*   RsiteRefAsnWrite(rrp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (RsiteRef ::=)
*
*****************************************************************************/
Boolean LIBCALL RsiteRefAsnWrite (RsiteRefPtr rrp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, RSITE_REF);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (rrp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnWrite(aip, atp, &av)) goto erret;     /* CHOICE */
    switch (rrp->choice)
    {
        case 1:
            if (! AsnWrite(aip, RSITE_REF_str, &rrp->data))
                goto erret;
            break;
        case 2:
            if (! DbtagAsnWrite((DbtagPtr)rrp->data.ptrvalue, aip, RSITE_REF_db))
                goto erret;
            break;
    }
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   RsiteRefAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
RsiteRefPtr LIBCALL RsiteRefAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    RsiteRefPtr rrp;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return (RsiteRefPtr)NULL;
	}

	if (aip == NULL)
		return (RsiteRefPtr)NULL;

	if (orig == NULL)           /* RsiteRef ::= (self contained) */
		atp = AsnReadId(aip, amp, RSITE_REF);
	else
		atp = AsnLinkType(orig, RSITE_REF);    /* link in local tree */
    if (atp == NULL)
        return (RsiteRefPtr)NULL;

	rrp = ValNodeNew(NULL);
    if (rrp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the CHOICE */
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;   /* which CHOICE */
    if (atp == NULL)
        goto erret;
    if (atp == RSITE_REF_str)
    {
        rrp->choice = 1;
        if (AsnReadVal(aip, atp, &rrp->data) <= 0)
            goto erret;
    }
    else if (atp == RSITE_REF_db)
    {
        rrp->choice = 2;
        rrp->data.ptrvalue = (Pointer) DbtagAsnRead(aip, atp);
        if (rrp->data.ptrvalue == NULL)
            goto erret;
    }
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return rrp;
erret:
    rrp = RsiteRefFree(rrp);
    goto ret;
}

/*****************************************************************************
*
*   SeqFeatIdFree(cp)
*       Frees one SeqFeatId's associated data
*       Does NOT free the Choice itself
*
*****************************************************************************/
void LIBCALL SeqFeatIdFree (ChoicePtr cp)
{
    Pointer pnt;

    if (cp == NULL)
        return;

    pnt = cp->value.ptrvalue;
    switch (cp->choice)
    {
        case 2:     /* giim id */
            GiimFree((GiimPtr)pnt);
            break;
        case 3:
            ObjectIdFree((ObjectIdPtr)pnt);
            break;
        case 4:
            DbtagFree((DbtagPtr)pnt);
            break;
    }
    return;
}

/*****************************************************************************
*
*   SeqFeatIdDup(dest, src)
*       Duplicates one SeqFeatId's associated data
*
*****************************************************************************/
Boolean LIBCALL SeqFeatIdDup (ChoicePtr dest, ChoicePtr src)
{
	GiimPtr ga, gb;

    if (src == NULL)
        return FALSE;

	dest->choice = src->choice;
    switch (src->choice)
    {
		case 1:     /* gibb */
			dest->value.intvalue = src->value.intvalue;
			break;
        case 2:     /* giim id */
			ga = (GiimPtr) src->value.ptrvalue;
			gb = GiimNew();
			if (gb == NULL) return FALSE;
			gb->id = ga->id;
			gb->db = StringSave(ga->db);
			gb->release = StringSave(ga->release);
			dest->value.ptrvalue = gb;
			break;
        case 3:
			dest->value.ptrvalue = ObjectIdDup((ObjectIdPtr)src->value.ptrvalue);
            break;
        case 4:
			dest->value.ptrvalue = DbtagDup((DbtagPtr)src->value.ptrvalue);
            break;
    }
    return TRUE;
}

/*****************************************************************************
*
*   SeqFeatIdAsnWrite(cp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SeqFeatId ::=)
*
*****************************************************************************/
Boolean LIBCALL SeqFeatIdAsnWrite (ChoicePtr cp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, FEAT_ID);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (cp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (cp->choice)              /* feat-id present */
    {
        if (! AsnWrite(aip, atp, &av)) goto erret;   /* CHOICE */
        if (cp->choice == 1)           /* gibb */
        {
            av.intvalue = cp->value.intvalue;
            if (! AsnWrite(aip, FEAT_ID_gibb, &av)) goto erret;
        }
        else if (cp->choice == 2)      /* giim */
        {
            if (! GiimAsnWrite((GiimPtr)cp->value.ptrvalue, aip, FEAT_ID_giim))
                goto erret;
        }
        else if (cp->choice == 3)      /* local */
        {
            if (! ObjectIdAsnWrite((ObjectIdPtr)cp->value.ptrvalue, aip, FEAT_ID_local))
                goto erret;
        }
        else if (cp->choice == 4)      /* general */
        {
            if (! DbtagAsnWrite((DbtagPtr)cp->value.ptrvalue, aip, FEAT_ID_general))
                goto erret;
        }
    }
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqFeatIdAsnRead(aip, atp, cp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
Boolean LIBCALL SeqFeatIdAsnRead (AsnIoPtr aip, AsnTypePtr orig, ChoicePtr cp)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return retval;
	}

	if (aip == NULL)
		return retval;

	if (orig == NULL)           /* SeqFeatId ::= (self contained) */
		atp = AsnReadId(aip, amp, FEAT_ID);
	else
		atp = AsnLinkType(orig, FEAT_ID);    /* link in local tree */
    if (atp == NULL)
        return retval;

    if (AsnReadVal(aip, atp, &av) <= 0)    /* read CHOICE */
        goto erret;
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (atp == FEAT_ID_gibb)
    {
        cp->choice = 1;
        if (AsnReadVal(aip, atp, &av) <= 0)
            goto erret;
        cp->value.intvalue = av.intvalue;
    }
    else if (atp == FEAT_ID_giim)
    {
        cp->choice = 2;
        cp->value.ptrvalue = (Pointer)GiimAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == FEAT_ID_local)
    {
        cp->choice = 3;
        cp->value.ptrvalue = (Pointer)ObjectIdAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    else if (atp == FEAT_ID_general)
    {
        cp->choice = 4;
        cp->value.ptrvalue = (Pointer)DbtagAsnRead(aip, atp);
        if (cp->value.ptrvalue == NULL)
            goto erret;
    }
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   TxinitNew()
*
*****************************************************************************/
TxinitPtr LIBCALL TxinitNew (void)
{
    TxinitPtr txp;

    txp = (TxinitPtr)MemNew(sizeof(Txinit));
	if (txp == NULL) return txp;
    txp->inittype = 255;
    return txp;
}

/*****************************************************************************
*
*   TxinitFree(txp)
*       Frees a Txinit and associated data
*
*****************************************************************************/
TxinitPtr LIBCALL TxinitFree (TxinitPtr txp)
{
    ValNodePtr anp, next;
    TxEvidencePtr txep, txnext;

    if (txp == NULL)
        return (TxinitPtr)NULL;

    MemFree(txp->name);
    ValNodeFreeData(txp->syn);
    anp = txp->gene;
    while (anp != NULL)
    {
        next = anp->next;
        GeneRefFree((GeneRefPtr)anp->data.ptrvalue);
        MemFree(anp);
        anp = next;
    }
    anp = txp->protein;
    while (anp != NULL)
    {
        next = anp->next;
        ProtRefFree((ProtRefPtr)anp->data.ptrvalue);
        MemFree(anp);
        anp = next;
    }
    ValNodeFreeData(txp->rna);
    MemFree(txp->expression);
    MemFree(txp->txdescr);
    OrgRefFree(txp->txorg);
    txep = txp->evidence;
    while (txep != NULL)
    {
        txnext = txep->next;
        MemFree(txep);
        txep = txnext;
    }

    return (TxinitPtr)MemFree(txp);
}

/*****************************************************************************
*
*   TxinitAsnWrite(txp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (Txinit ::=)
*
*****************************************************************************/
Boolean LIBCALL TxinitAsnWrite (TxinitPtr txp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    ValNodePtr anp;
    TxEvidencePtr txep;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, TXINIT);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (txp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)txp))
        goto erret;

    av.ptrvalue = (Pointer)txp->name;
    if (! AsnWrite(aip, TXINIT_name, &av)) goto erret;
    if (txp->syn != NULL)
    {
        anp = txp->syn;
        if (! AsnOpenStruct(aip, TXINIT_syn, (Pointer)txp->syn))
            goto erret;
        while (anp != NULL)
        {
            av.ptrvalue = anp->data.ptrvalue;
            if (! AsnWrite(aip, TXINIT_syn_E, &av)) goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, TXINIT_syn, (Pointer)txp->syn))
            goto erret;
    }
    if (txp->gene != NULL)
    {
        anp = txp->gene;
        if (! AsnOpenStruct(aip, TXINIT_gene, (Pointer)txp->gene))
            goto erret;
        while (anp != NULL)
        {
            if (! GeneRefAsnWrite((GeneRefPtr)anp->data.ptrvalue, aip, TXINIT_gene_E))
                goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, TXINIT_gene, (Pointer)txp->gene))
            goto erret;
    }
    if (txp->protein != NULL)
    {
        anp = txp->protein;
        if (! AsnOpenStruct(aip, TXINIT_protein, (Pointer)txp->protein))
            goto erret;
        while (anp != NULL)
        {
            if (! ProtRefAsnWrite((ProtRefPtr)anp->data.ptrvalue, aip, TXINIT_protein_E))
                goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, TXINIT_protein, (Pointer)txp->protein))
            goto erret;
    }
    if (txp->rna != NULL)
    {
        anp = txp->rna;
        if (! AsnOpenStruct(aip, TXINIT_rna, (Pointer)txp->rna))
            goto erret;
        while (anp != NULL)
        {
            av.ptrvalue = anp->data.ptrvalue;
            if (! AsnWrite(aip, TXINIT_rna_E, &av)) goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, TXINIT_rna, (Pointer)txp->rna))
            goto erret;
    }
    if (txp->expression != NULL)
    {
        av.ptrvalue = (Pointer)txp->expression;
        if (! AsnWrite(aip, TXINIT_expression, &av)) goto erret;
    }

    av.intvalue = (Int4)txp->txsystem;
    if (! AsnWrite(aip, TXINIT_txsystem, &av)) goto erret;

    if (txp->txdescr != NULL)
    {
        av.ptrvalue = (Pointer)txp->txdescr;
        if (! AsnWrite(aip, TXINIT_txdescr, &av)) goto erret;
    }

    if (txp->txorg != NULL)
    {
        if (! OrgRefAsnWrite(txp->txorg, aip, TXINIT_txorg))
            goto erret;
    }

    if (txp->mapping_precise)
    {
        av.boolvalue = TRUE;
        if (! AsnWrite(aip, TXINIT_mapping_precise, &av)) goto erret;
    }

    if (txp->location_accurate)
    {
        av.boolvalue = TRUE;
        if (! AsnWrite(aip, TXINIT_location_accurate, &av)) goto erret;
    }

    if (txp->inittype != 255)
    {
        av.intvalue = (Int4) txp->inittype;
        if (! AsnWrite(aip, TXINIT_inittype, &av)) goto erret;
    }

    if (txp->evidence != NULL)
    {
        txep = txp->evidence;
        if (! AsnOpenStruct(aip, TXINIT_evidence, (Pointer)txp->evidence))
            goto erret;
        while (txep != NULL)
        {
            if (! AsnOpenStruct(aip, TXINIT_evidence_E, (Pointer)txep))
                goto erret;
            av.intvalue = (Int4) txep->exp_code;
            if (! AsnWrite(aip, TX_EVIDENCE_exp_code, &av)) goto erret;
            if (txep->exp_sys != 1)
            {
                av.intvalue = (Int4) txep->exp_sys;
                if (! AsnWrite(aip, TX_EVIDENCE_expression_system, &av)) goto erret;
            }
            if (txep->low_prec_data)
            {
                av.boolvalue = TRUE;
                if (! AsnWrite(aip, TX_EVIDENCE_low_prec_data, &av)) goto erret;
            }
            if (txep->from_homolog)
            {
                av.boolvalue = TRUE;
                if (! AsnWrite(aip, TX_EVIDENCE_from_homolog, &av)) goto erret;
            }
            if (! AsnCloseStruct(aip, TXINIT_evidence_E, (Pointer)txep))
                goto erret;
            txep = txep->next;
        }
        if (! AsnCloseStruct(aip, TXINIT_evidence, (Pointer)txp->evidence))
            goto erret;
    }

    if (! AsnCloseStruct(aip, atp, (Pointer)txp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   TxinitAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
TxinitPtr LIBCALL TxinitAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    TxinitPtr txp;
    ValNodePtr curr, last;
    TxEvidencePtr txep, txlast;

	if (! loaded)
	{
		if (! SeqFeatAsnLoad())
			return (TxinitPtr)NULL;
	}

	if (aip == NULL)
		return (TxinitPtr)NULL;

	if (orig == NULL)           /* Txinit ::= (self contained) */
		atp = AsnReadId(aip, amp, TXINIT);
	else
		atp = AsnLinkType(orig, TXINIT);    /* link in local tree */
    oldatp = atp;
    if (atp == NULL)
        return (TxinitPtr)NULL;

	txp = TxinitNew();
    if (txp == NULL)
        return txp;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */

    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL)
            goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        if (atp == TXINIT_name)
            txp->name = (CharPtr)av.ptrvalue;
        else if (atp == TXINIT_syn)
        {
            last = NULL;
            while ((atp = AsnReadId(aip, amp, atp)) == TXINIT_syn_E)
            {
                curr = ValNodeNew(last);
                if (curr == NULL)
                    goto erret;
                if (AsnReadVal(aip, atp, &curr->data) <= 0) goto erret;
                if (last == NULL)
                    txp->syn = curr;
                last = curr;
            }
            if (atp == NULL)
                goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        }
        else if (atp == TXINIT_gene)
        {
            last = NULL;
            while ((atp = AsnReadId(aip, amp, atp)) == TXINIT_gene_E)
            {
                curr = ValNodeNew(last);
                if (curr == NULL)
                    goto erret;
                curr->data.ptrvalue = (Pointer)GeneRefAsnRead(aip, atp);
                if (curr->data.ptrvalue == NULL)
                    goto erret;
                if (last == NULL)
                    txp->gene = curr;
                last = curr;
            }
            if (atp == NULL)
                goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        }
        else if (atp == TXINIT_protein)
        {
            last = NULL;
            while ((atp = AsnReadId(aip, amp, atp)) == TXINIT_protein_E)
            {
                curr = ValNodeNew(last);
                if (curr == NULL)
                    goto erret;
                curr->data.ptrvalue = (Pointer)ProtRefAsnRead(aip, atp);
                if (curr->data.ptrvalue == NULL)
                    goto erret;
                if (last == NULL)
                    txp->protein = curr;
                last = curr;
            }
            if (atp == NULL)
                goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        }
        else if (atp == TXINIT_rna)
        {
            last = NULL;
            while ((atp = AsnReadId(aip, amp, atp)) == TXINIT_rna_E)
            {
                curr = ValNodeNew(last);
                if (curr == NULL)
                    goto erret;
                if (AsnReadVal(aip, atp, &curr->data) <= 0) goto erret;
                if (last == NULL)
                    txp->rna = curr;
                last = curr;
            }
            if (atp == NULL)
                goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        }
        else if (atp == TXINIT_expression)
            txp->expression = (CharPtr)av.ptrvalue;
        else if (atp == TXINIT_txsystem)
            txp->txsystem = (Uint1)av.intvalue;
        else if (atp == TXINIT_txdescr)
            txp->txdescr = (CharPtr)av.ptrvalue;
        else if (atp == TXINIT_txorg)
        {
            txp->txorg = OrgRefAsnRead(aip, atp);
            if (txp->txorg == NULL)
                goto erret;
        }
        else if (atp == TXINIT_mapping_precise)
            txp->mapping_precise = av.boolvalue;
        else if (atp == TXINIT_location_accurate)
            txp->location_accurate = av.boolvalue;
        else if (atp == TXINIT_inittype)
            txp->inittype = (Uint1)av.intvalue;
        else if (atp == TXINIT_evidence)
        {
            txlast = NULL;
            txep = NULL;
            while ((atp = AsnReadId(aip, amp, atp)) != TXINIT_evidence)
            {
                if (atp == NULL)
                    goto erret;
                if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
                if ((atp == TXINIT_evidence_E) && (av.intvalue == START_STRUCT))
                {
                    txep = (TxEvidencePtr)MemNew(sizeof(TxEvidence));
                    if (txep == NULL)
                        goto erret;
                    txep->exp_sys = 1;    /* default */
                    if (txlast != NULL)
                        txlast->next = txep;
                    else
                        txp->evidence = txep;
                    txlast = txep;
                }
                else if (atp == TX_EVIDENCE_exp_code)
                    txep->exp_code = (Uint1) av.intvalue;
                else if (atp == TX_EVIDENCE_expression_system)
                    txep->exp_sys = (Uint1) av.intvalue;
                else if (atp == TX_EVIDENCE_low_prec_data)
                    txep->low_prec_data = av.boolvalue;
                else if (atp == TX_EVIDENCE_from_homolog)
                    txep->from_homolog = av.boolvalue;
            }
            if (atp == NULL)
                goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
        }
    }
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return txp;
erret:
    txp = TxinitFree(txp);
    goto ret;
}


