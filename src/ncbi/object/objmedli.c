/*  objmedli.c
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
* File Name:  objmedli.c
*
* Author:  James Ostell
*   
* Version Creation Date: 1/1/91
*
* $Revision: 2.2 $
*
* File Description:  Object manager for module NCBI-Medline
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

#include <objmedli.h>
#include <asnmedli.h>

static Boolean loaded = FALSE;

/*****************************************************************************
*
*   Medline Object routines
*
*****************************************************************************/

/*****************************************************************************
*
*   MedlineAsnLoad()
*
*****************************************************************************/
Boolean LIBCALL MedlineAsnLoad ()
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
    if (! AsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    return TRUE;
}

/*****************************************************************************
*
*   MedlineMeshNew()
*
*****************************************************************************/
MedlineMeshPtr LIBCALL MedlineMeshNew ()
{
	MedlineMeshPtr mmp;

	mmp = (MedlineMeshPtr)MemNew(sizeof(MedlineMesh));
	return mmp;
}

/*****************************************************************************
*
*   MedlineMeshFree()
*
*****************************************************************************/
MedlineMeshPtr LIBCALL MedlineMeshFree (MedlineMeshPtr mmp)
{
    if (mmp == NULL)
        return mmp;

    MemFree(mmp->term);
    ValNodeFreeData(mmp->qual);
    
	return (MedlineMeshPtr)MemFree(mmp);
}

/*****************************************************************************
*
*   MedlineMeshAsnRead(aip, atp)
*
*****************************************************************************/
MedlineMeshPtr LIBCALL MedlineMeshAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	MedlineMeshPtr mmp=NULL;
	DataVal av;
	AsnTypePtr atp, oldatp;
    ValNodePtr first, anp;

	if (! loaded)
	{
		if (! MedlineAsnLoad())
			return mmp;
	}

	if (aip == NULL)
		return mmp;

	if (orig == NULL)           /* MedlineMesh ::= */
		atp = AsnReadId(aip, amp, MEDLINE_MESH);
	else
		atp = AsnLinkType(orig, MEDLINE_MESH);
    oldatp = atp;
    if (atp == NULL)
        return mmp;

	mmp = MedlineMeshNew();
    if (mmp == NULL) goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the SEQUENCE */
    first = NULL;
    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL) goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        if (atp == MEDLINE_MESH_qual)     /* SET OF medline-qual */
        {
           while ((atp = AsnReadId(aip, amp, atp)) != MEDLINE_MESH_qual)
           {
               if (atp == NULL) goto erret;
               if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
               if ((atp == MEDLINE_MESH_qual_E) && (av.intvalue == START_STRUCT))
               {
                   anp = ValNodeNew(first);
                   if (anp == NULL) goto erret;
                   if (first == NULL)
                    first = anp;
               }
               else if (atp == MEDLINE_QUAL_mp)
                    anp->choice = (Uint1)av.boolvalue;
                else if (atp == MEDLINE_QUAL_subh)
                    anp->data.ptrvalue = av.ptrvalue;
           }
           mmp->qual = first;
           if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end of SET OF */
        }
        else if (atp == MEDLINE_MESH_mp)
            mmp->mp = av.boolvalue;
        else if (atp == MEDLINE_MESH_term)
            mmp->term = (CharPtr)av.ptrvalue;
    }

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the END STRUCT */
ret:
	AsnUnlinkType(orig);
	return mmp;
erret:
    mmp = MedlineMeshFree(mmp);
    goto ret;
}

/*****************************************************************************
*
*   MedlineMeshAsnWrite(mmp, aip, atp) 
*
*****************************************************************************/
Boolean LIBCALL MedlineMeshAsnWrite (MedlineMeshPtr mmp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    ValNodePtr anp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! MedlineAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, MEDLINE_MESH);
    if (atp == NULL) return FALSE;

	if (mmp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)mmp)) goto erret;
    if (mmp->mp)
    {
        av.boolvalue = TRUE;
        if (! AsnWrite(aip, MEDLINE_MESH_mp, &av)) goto erret;
    }
    av.ptrvalue = mmp->term;
    if (! AsnWrite(aip, MEDLINE_MESH_term, &av)) goto erret;
    if (mmp->qual != NULL)
    {						    /* start SET OF */
        if (! AsnOpenStruct(aip, MEDLINE_MESH_qual, (Pointer)mmp->qual))
            goto erret;
        anp = mmp->qual;
        while (anp != NULL)
        {
            if (! AsnOpenStruct(aip, MEDLINE_MESH_qual_E, (Pointer)anp))
                goto erret;
            if (anp->choice != 0)
            {
                av.boolvalue = TRUE;
                if (! AsnWrite(aip, MEDLINE_QUAL_mp, &av)) goto erret;
            }
            av.ptrvalue = anp->data.ptrvalue;
            if (! AsnWrite(aip, MEDLINE_QUAL_subh, &av)) goto erret;
            if (! AsnCloseStruct(aip, MEDLINE_MESH_qual_E, (Pointer)anp))
                goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, MEDLINE_MESH_qual, (Pointer)mmp->qual))
            goto erret;
    }

    if (! AsnCloseStruct(aip, atp, (Pointer)mmp)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   MedlineRnNew()
*
*****************************************************************************/
MedlineRnPtr LIBCALL MedlineRnNew (void)
{
	MedlineRnPtr mrp;

	mrp = (MedlineRnPtr)MemNew(sizeof(MedlineRn));
	return mrp;
}

/*****************************************************************************
*
*   MedlineRnFree()
*
*****************************************************************************/
MedlineRnPtr LIBCALL MedlineRnFree (MedlineRnPtr mrp)
{
    if (mrp == NULL)
        return mrp;

    MemFree(mrp->cit);
    MemFree(mrp->name);
    
	return (MedlineRnPtr)MemFree(mrp);
}

/*****************************************************************************
*
*   MedlineRnAsnRead(aip, atp)
*
*****************************************************************************/
MedlineRnPtr LIBCALL MedlineRnAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	MedlineRnPtr mrp=NULL;
	DataVal av;
	AsnTypePtr atp;

	if (! loaded)
	{
		if (! MedlineAsnLoad())
			return mrp;
	}

	if (aip == NULL)
		return mrp;

	if (orig == NULL)           /* MedlineRn ::= */
		atp = AsnReadId(aip, amp, MEDLINE_RN);
	else
		atp = AsnLinkType(orig, MEDLINE_RN);
    if (atp == NULL) return mrp;

	mrp = MedlineRnNew();
    if (mrp == NULL) goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the SEQUENCE */
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;  /* type */
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    mrp->type = (Uint1)av.intvalue;
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (atp == MEDLINE_RN_cit)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        mrp->cit = (CharPtr)av.ptrvalue;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    mrp->name = (CharPtr)av.ptrvalue;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the END STRUCT */
ret:
	AsnUnlinkType(orig);
	return mrp;
erret:
    mrp = MedlineRnFree(mrp);
    goto ret;
}

/*****************************************************************************
*
*   MedlineRnAsnWrite(mrp, aip, atp) 
*
*****************************************************************************/
Boolean LIBCALL MedlineRnAsnWrite (MedlineRnPtr mrp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = TRUE;

	if (! loaded)
	{
		if (! MedlineAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, MEDLINE_RN);
    if (atp == NULL) return FALSE;

	if (mrp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)mrp)) goto erret;
    av.intvalue = (Int4)mrp->type;
    if (! AsnWrite(aip, MEDLINE_RN_type, &av)) goto erret;
    if (mrp->cit != NULL)
    {
        av.ptrvalue = mrp->cit;
        if (! AsnWrite(aip, MEDLINE_RN_cit, &av)) goto erret;
    }
    av.ptrvalue = mrp->name;
    if (! AsnWrite(aip, MEDLINE_RN_name, &av)) goto erret;
    if (! AsnCloseStruct(aip, atp, (Pointer)mrp)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   MedlineSiAsnRead(aip, atp)
*
*****************************************************************************/
ValNodePtr LIBCALL MedlineSiAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	ValNodePtr msp=NULL;
	DataVal av;
	AsnTypePtr atp;

	if (! loaded)
	{
		if (! MedlineAsnLoad())
			return msp;
	}

	if (aip == NULL)
		return msp;

	if (orig == NULL)           /* MedlineSi ::= */
		atp = AsnReadId(aip, amp, MEDLINE_SI);
	else
		atp = AsnLinkType(orig, MEDLINE_SI);
    if (atp == NULL) return msp;

	msp = ValNodeNew(NULL);
    if (msp == NULL) goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the SEQUENCE */
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;  /* type */
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    msp->choice = (Uint1)av.intvalue;
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (atp == MEDLINE_SI_cit)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        msp->data.ptrvalue = av.ptrvalue;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;  /* end struct */
ret:
	AsnUnlinkType(orig);
	return msp;
erret:
    msp = ValNodeFreeData(msp);
    goto ret;
}

/*****************************************************************************
*
*   MedlineSiAsnWrite(msp, aip, atp) 
*
*****************************************************************************/
Boolean LIBCALL MedlineSiAsnWrite (ValNodePtr msp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! MedlineAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, MEDLINE_SI);
    if (atp == NULL) return FALSE;

	if (msp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)msp)) goto erret;
    av.intvalue = (Int4)msp->choice;
    if (! AsnWrite(aip, MEDLINE_SI_type, &av)) goto erret;
    if (msp->data.ptrvalue != NULL)
    {
        av.ptrvalue = msp->data.ptrvalue;
        if (! AsnWrite(aip, MEDLINE_SI_cit, &av)) goto erret;
    }
    if (! AsnCloseStruct(aip, atp, (Pointer)msp)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   MedlineEntryNew()
*
*****************************************************************************/
MedlineEntryPtr LIBCALL MedlineEntryNew (void)
{
	MedlineEntryPtr mep;

	mep = (MedlineEntryPtr)MemNew(sizeof(MedlineEntry));
	return mep;
}

/*****************************************************************************
*
*   MedlineEntryFree()
*
*****************************************************************************/
MedlineEntryPtr LIBCALL MedlineEntryFree (MedlineEntryPtr mep)
{
    MedlineMeshPtr mmp, mmpnext;
    MedlineRnPtr mrp, mrpnext;

    if (mep == NULL)
        return mep;

    DateFree(mep->em);
    CitArtFree(mep->cit);
    MemFree(mep->abstract);
    mmp = mep->mesh;
    while (mmp != NULL)
    {
        mmpnext = mmp->next;
        MedlineMeshFree(mmp);
        mmp = mmpnext;
    }
    mrp = mep->substance;
    while (mrp != NULL)
    {
        mrpnext = mrp->next;
        MedlineRnFree(mrp);
        mrp = mrpnext;
    }
    ValNodeFreeData(mep->xref);
    ValNodeFreeData(mep->idnum);
    ValNodeFreeData(mep->gene);

	return (MedlineEntryPtr)MemFree(mep);
}

/*****************************************************************************
*
*   MedlineEntryAsnRead(aip, atp)
*
*****************************************************************************/
MedlineEntryPtr LIBCALL MedlineEntryAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	MedlineEntryPtr mep=NULL;
	DataVal av;
	AsnTypePtr atp, oldatp;
    MedlineMeshPtr mmp, mmplast;
    MedlineRnPtr mrp, mrplast;
    ValNodePtr anp, anplast;

	if (! loaded)
	{
		if (! MedlineAsnLoad())
			return mep;
	}

	if (aip == NULL)
		return mep;

	if (orig == NULL)           /* MedlineEntry ::= */
		atp = AsnReadId(aip, amp, MEDLINE_ENTRY);
	else
		atp = AsnLinkType(orig, MEDLINE_ENTRY);
    oldatp = atp;
    if (atp == NULL) return mep;

	mep = MedlineEntryNew();
    if (mep == NULL) goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the SEQUENCE */
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;  /* uid */
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    mep->uid = av.intvalue;
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;  /* entry month */
    mep->em = DateAsnRead(aip, atp);
    if (mep->em == NULL) goto erret;
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;   /* citation */
    mep->cit = CitArtAsnRead(aip, atp);
    if (mep->cit == NULL) goto erret;
    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL) goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        if (atp == MEDLINE_ENTRY_abstract)
        {
            mep->abstract = (CharPtr)av.ptrvalue;
        }
        else if (atp == MEDLINE_ENTRY_mesh)
        {
            mmplast = NULL;
            while ((atp = AsnReadId(aip, amp, atp)) != MEDLINE_ENTRY_mesh)
            {
                if (atp == NULL) goto erret;
               mmp = MedlineMeshAsnRead(aip, atp);
                if (mmp == NULL) goto erret;
               if (mmplast == NULL)
                    mep->mesh = mmp;
                else
                    mmplast->next = mmp;
                mmplast = mmp;
            }
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end of SET OF */
        }
        else if (atp == MEDLINE_ENTRY_substance)
        {
            mrplast = NULL;
            while ((atp = AsnReadId(aip, amp, atp)) != MEDLINE_ENTRY_substance)
            {
                if (atp == NULL) goto erret;
                mrp = MedlineRnAsnRead(aip, atp);
                if (mrp == NULL) goto erret;
                if (mrplast == NULL)
                    mep->substance = mrp;
                else
                    mrplast->next = mrp;
                mrplast = mrp;
            }
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end SET OF */
        }
        else if (atp == MEDLINE_ENTRY_xref)
        {
            anplast = NULL;
            while ((atp = AsnReadId(aip, amp, atp)) == MEDLINE_ENTRY_xref_E)
            {
                anp = MedlineSiAsnRead(aip, atp);
                if (anp == NULL) goto erret;
                if (mep->xref == NULL)
                    mep->xref = anp;
                else
                    anplast->next = anp;
                anplast = anp;
            }
            if (atp == NULL) goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        }
        else if (atp == MEDLINE_ENTRY_idnum)
        {
            anp = NULL;
            while ((atp = AsnReadId(aip, amp, atp)) != MEDLINE_ENTRY_idnum)
            {
                if (atp == NULL) goto erret;
                if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
                anp = ValNodeNew(anp);
                if (anp == NULL) goto erret;
                if (mep->idnum == NULL)
                    mep->idnum = anp;
                anp->data.ptrvalue = av.ptrvalue;   /* VisibleString */
            }
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end SET OF */
        }
        else if (atp == MEDLINE_ENTRY_gene)
        {
            anp = NULL;
            while ((atp = AsnReadId(aip, amp, atp)) != MEDLINE_ENTRY_gene)
            {
                if (atp == NULL) goto erret;
                if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
                anp = ValNodeNew(anp);
                if (anp == NULL) goto erret;
                if (mep->gene == NULL)
                    mep->gene = anp;
                anp->data.ptrvalue = av.ptrvalue;   /* VisibleString */
            }
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end SET OF */
        }
    }

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the END STRUCT */
ret:
	AsnUnlinkType(orig);
	return mep;
erret:
    mep = MedlineEntryFree(mep);
    goto ret;
}

/*****************************************************************************
*
*   MedlineEntryAsnWrite(mep, aip, atp) 
*
*****************************************************************************/
Boolean LIBCALL MedlineEntryAsnWrite (MedlineEntryPtr mep, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    MedlineMeshPtr mmp;
    MedlineRnPtr mrp;
    ValNodePtr anp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! MedlineAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, MEDLINE_ENTRY);
    if (atp == NULL) return FALSE;

	if (mep == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)mep)) goto erret;
    av.intvalue = mep->uid;
    if (! AsnWrite(aip, MEDLINE_ENTRY_uid, &av)) goto erret;
    if (! DateAsnWrite(mep->em, aip, MEDLINE_ENTRY_em)) goto erret;
    if (! CitArtAsnWrite(mep->cit, aip, MEDLINE_ENTRY_cit)) goto erret;
    if (mep->abstract != NULL)
    {
        av.ptrvalue = mep->abstract;
        if (! AsnWrite(aip, MEDLINE_ENTRY_abstract, &av)) goto erret;
    }
    if (mep->mesh != NULL)
    {
        if (! AsnOpenStruct(aip, MEDLINE_ENTRY_mesh, (Pointer)mep->mesh))
            goto erret;
        mmp = mep->mesh;
        while (mmp != NULL)
        {
            if (! MedlineMeshAsnWrite(mmp, aip, MEDLINE_ENTRY_mesh_E))
                goto erret;
            mmp = mmp->next;
        }
        if (! AsnCloseStruct(aip, MEDLINE_ENTRY_mesh, (Pointer)mep->mesh))
            goto erret;
    }
    if (mep->substance != NULL)
    {
        if (! AsnOpenStruct(aip, MEDLINE_ENTRY_substance, (Pointer)mep->substance))
            goto erret;
        mrp = mep->substance;
        while (mrp != NULL)
        {
            if (! MedlineRnAsnWrite(mrp, aip, MEDLINE_ENTRY_substance_E))
                goto erret;
            mrp = mrp->next;
        }
        if (! AsnCloseStruct(aip, MEDLINE_ENTRY_substance, (Pointer)mep->substance))
            goto erret;
    }
    if (mep->xref != NULL)
    {
        if (! AsnOpenStruct(aip, MEDLINE_ENTRY_xref, (Pointer)mep->xref))
            goto erret;
        anp = mep->xref;
        while (anp != NULL)
        {
            if (! MedlineSiAsnWrite(anp, aip, MEDLINE_ENTRY_xref_E))
                goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, MEDLINE_ENTRY_xref, (Pointer)mep->xref))
            goto erret;
    }
    if (mep->idnum != NULL)
    {
        if (! AsnOpenStruct(aip, MEDLINE_ENTRY_idnum, (Pointer)mep->idnum))
            goto erret;
        anp = mep->idnum;
        while (anp != NULL)
        {
            av.ptrvalue = anp->data.ptrvalue;
            if (! AsnWrite(aip, MEDLINE_ENTRY_idnum_E, &av)) goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, MEDLINE_ENTRY_idnum, (Pointer)mep->idnum))
            goto erret;
    }
    if (mep->gene != NULL)
    {
        if (! AsnOpenStruct(aip, MEDLINE_ENTRY_gene, (Pointer)mep->gene))
            goto erret;
        anp = mep->gene;
        while (anp != NULL)
        {
            av.ptrvalue = anp->data.ptrvalue;
            if (! AsnWrite(aip, MEDLINE_ENTRY_gene_E, &av)) goto erret;
            anp = anp->next;
        }
        if (! AsnCloseStruct(aip, MEDLINE_ENTRY_gene, (Pointer)mep->gene))
            goto erret;
    }

    if (! AsnCloseStruct(aip, atp, (Pointer)mep)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);
	return retval;
}

