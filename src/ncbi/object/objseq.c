/*  objseq.c
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
* File Name:  objseq.c
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.6 $
*
* File Description:  Object manager for module NCBI-Seq
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
#include <objseq.h>		   /* the pub interface */
#include <asnseq.h>        /* the AsnTool header */
#include <sequtil.h>

static Boolean loaded = FALSE;

static BioseqPtr PNTR Blist = NULL;   /* loaded Bioseqs  */
static Int2 Bnum = 0;              /* sizeof Blist */

/*****************************************************************************
*
*   SeqAsnLoad()
*
*****************************************************************************/
Boolean LIBCALL SeqAsnLoad ()
{
    if (loaded)
        return TRUE;
    loaded = TRUE;

    if (! GeneralAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    if (! SeqLocAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    if (! SeqAlignAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    if (! SeqFeatAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    if (! SeqResAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    if (! PubAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    if (! AccessAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
	if (! SeqCodeAsnLoad())
	{
		loaded = FALSE;
		return FALSE;
	}
	if (! SeqBlockAsnLoad())
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
*   Bioseq Routines
*
*****************************************************************************/

/*****************************************************************************
*
*   BioseqNew()
*
*****************************************************************************/
BioseqPtr LIBCALL BioseqNew ()
{
    BioseqPtr bsp;
    BioseqPtr PNTR tmp;
    Int2 i;

    bsp = (BioseqPtr)MemNew(sizeof(Bioseq));
	if (bsp == NULL) return bsp;

    bsp->length = -1;    /* not set */
	bsp->topology = 1;   /* DEFAULT = linear */

    for (i = 0; i < Bnum; i++)    /* add to Bioseq list */
    {
        if (Blist[i] == NULL)
        {
            Blist[i] = bsp;
            return bsp;
        }
    }
    tmp = Blist;                 /* expand list as necessary */
    Blist = (BioseqPtr PNTR)MemNew((sizeof(BioseqPtr) * (Bnum + 20)));
	if (Blist == NULL)
	{
		Blist = tmp;
		bsp = (BioseqPtr)(MemFree(bsp));
		return bsp;
	}
    MemCopy(Blist, tmp, (size_t)(sizeof(BioseqPtr) * Bnum));
    MemFree(tmp);
    Blist[Bnum] = bsp;
    Bnum += 20;

    return bsp;
}

/*****************************************************************************
*
*   BioseqFree(bsp)
*       Frees one Bioseq and associated data
*
*****************************************************************************/
BioseqPtr LIBCALL BioseqFree (BioseqPtr bsp)
{
    Int2 i;
    SeqAnnotPtr sp, spnext;
    SeqFeatPtr sfp, sfpnext;

    if (bsp == NULL)
        return bsp;

    SeqIdSetFree(bsp->id);
    SeqDescrFree(bsp->descr);
    BSFree(bsp->seq_data);
    IntFuzzFree(bsp->fuzz);
    switch (bsp->seq_ext_type)
    {
        case 1:    /* seg-ext */
            SeqLocSetFree((ValNodePtr)bsp->seq_ext);
            break;
        case 2:   /* reference */
            SeqLocFree((ValNodePtr)bsp->seq_ext);
            break;
        case 3:   /* map */
            sfp = (SeqFeatPtr)bsp->seq_ext;
            while (sfp != NULL)
            {
                sfpnext = sfp->next;
                SeqFeatFree(sfp);
                sfp = sfpnext;
            }
            break;
    }
    sp = bsp->annot;
    while (sp != NULL)
    {
        spnext = sp->next;
        SeqAnnotFree(sp);
        sp = spnext;
    }
	SeqHistFree(bsp->hist);

    for (i = 0; i < Bnum; i++)
    {
        if (Blist[i] == bsp)
        {
            Blist[i] = NULL;
            return (BioseqPtr)MemFree(bsp);
        }
    }
    ErrPost(CTX_NCBIOBJ, 1, "BioseqFree: pointer not in Blist");
	return (BioseqPtr)MemFree(bsp);
}

/*****************************************************************************
*
*   BioseqInMem(&bnum)
*
*****************************************************************************/
BioseqPtr PNTR LIBCALL BioseqInMem (Int2Ptr numptr)
{
	*numptr = Bnum;
	if (! Bnum)
		return (BioseqPtr PNTR) NULL;
	else
		return Blist;
}

/*****************************************************************************
*
*   BioseqAsnWrite(bsp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (Bioseq ::=)
*
*****************************************************************************/
Boolean LIBCALL BioseqAsnWrite (BioseqPtr bsp, AsnIoPtr aip, AsnTypePtr orig)
{
	AsnTypePtr atp;
    ValNodePtr anp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	if (! ProgMon("Write Bioseq"))
		return FALSE;

	atp = AsnLinkType(orig, BIOSEQ);   /* link local tree */
    if (atp == NULL) goto erret;

	if (bsp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

	if (! AsnOpenStruct(aip, atp, (Pointer)bsp)) goto erret;
    
    if (! AsnOpenStruct(aip, BIOSEQ_id, (Pointer)bsp->id)) goto erret;    /* ids required */
    anp = bsp->id;
    while (anp != NULL)
    {
        if (! SeqIdAsnWrite(anp, aip, BIOSEQ_id_E)) goto erret;
        anp = anp->next;
    }
    if (! AsnCloseStruct(aip, BIOSEQ_id, (Pointer)bsp->id)) goto erret;

    if (bsp->descr != NULL)              /* Seq-descr optional */
    {
        if (! SeqDescrAsnWrite(bsp->descr, aip, BIOSEQ_descr)) goto erret;
    }

	if (! BioseqInstAsnWrite(bsp, aip, BIOSEQ_inst)) goto erret;

    if (bsp->annot != NULL)              /* annotation optional */
    {
        if (! SeqAnnotSetAsnWrite(bsp->annot, aip, BIOSEQ_annot, BIOSEQ_annot_E))
            goto erret;
    }

    if (! AsnCloseStruct(aip, atp, (Pointer)bsp)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   BioseqAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
BioseqPtr LIBCALL BioseqAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    BioseqPtr bsp=NULL;
	AsnOptionPtr aop;
	Op_objseqPtr osp;
	Boolean check_seqid = FALSE;
	Int2 level;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return bsp;
	}

	if (aip == NULL)
		return bsp;

	if (! ProgMon("Read Bioseq"))
		return bsp;

	if ((aop = AsnIoOptionGet(aip, OP_NCBIOBJSEQ, BIOSEQ_CHECK_ID, NULL)) != NULL)
	{
		osp = (Op_objseqPtr)aop->data.ptrvalue;
		if ((osp->found_it) && (osp->load_by_id))   /* already got it */
		{
			AsnSkipValue(aip, orig);
			return bsp;
		}
		check_seqid = TRUE;
	}

	if (orig == NULL)           /* Bioseq ::= (self contained) */
		atp = AsnReadId(aip, amp, BIOSEQ);
	else
		atp = AsnLinkType(orig, BIOSEQ);    /* link in local tree */
    if (atp == NULL) return bsp;

	bsp = BioseqNew();
    if (bsp == NULL) goto erret;

	level = AsnGetLevel(aip);     /* for skipping */

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the start struct */

	atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;  /* id required, start struct */
    bsp->id = SeqIdSetAsnRead(aip, atp, BIOSEQ_id_E);
    if (bsp->id == NULL) goto erret;
	if (check_seqid)
	{
		if (SeqIdIn(osp->sip, bsp->id))
			osp->found_it = TRUE;
		else if (osp->load_by_id)   /* don't load it */
		{
			while (AsnGetLevel(aip) > level)  /* skip everything else */
			{
				atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
				if (AsnReadVal(aip, atp, NULL) <= 0) goto erret;
			}
            goto erret;      /* didn't find it -- not really an error */
		}
	}

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (atp == BIOSEQ_descr)           /* descr optional */
    {
        bsp->descr = SeqDescrAsnRead(aip, atp);
        if (bsp->descr == NULL) goto erret;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }

	if (! BioseqInstAsnRead(bsp, aip, atp)) goto erret;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;

    if (atp == BIOSEQ_annot)
	{
        bsp->annot = SeqAnnotSetAsnRead(aip, atp, BIOSEQ_annot_E);
        if (bsp->annot == NULL) goto erret;
		atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
	}

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end Bioseq */
ret:
    AsnUnlinkType(orig);       /* unlink local tree */
	return bsp;
erret:
    bsp = BioseqFree(bsp);
    goto ret;
}

/*****************************************************************************
*
*   BioseqInstAsnWrite(bsp, aip, orig)
*
*****************************************************************************/
Boolean LIBCALL BioseqInstAsnWrite (BioseqPtr bsp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, tmp;
	Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	if (! ProgMon("Write SeqInst"))
		return FALSE;

	atp = AsnLinkType(orig, SEQ_INST);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (bsp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)bsp)) goto erret; /* instance required */
    av.intvalue = bsp->repr;
    if (! AsnWrite(aip, SEQ_INST_repr, &av)) goto erret;
    av.intvalue = bsp->mol;
    if (! AsnWrite(aip, SEQ_INST_mol, &av)) goto erret;
    if (bsp->length >= 0)
    {
        av.intvalue = bsp->length;
        if (! AsnWrite(aip, SEQ_INST_length, &av)) goto erret;
    }
    if (bsp->fuzz != NULL)
    {
        if (! IntFuzzAsnWrite(bsp->fuzz, aip, SEQ_INST_fuzz)) goto erret;
    }
    if (bsp->topology != 1)
    {
        av.intvalue = bsp->topology;
        if (! AsnWrite(aip, SEQ_INST_topology, &av)) goto erret;
    }
    if (bsp->strand)
    {
        av.intvalue = bsp->strand;
        if (! AsnWrite(aip, SEQ_INST_strand, &av)) goto erret;
    }
    if (bsp->seq_data != NULL)
    {
        av.ptrvalue = bsp->seq_data;
        if (! AsnWriteChoice(aip, SEQ_INST_seq_data, (Int2)bsp->seq_data_type, &av)) goto erret;   /* CHOICE */
        tmp = NULL;
        switch (bsp->seq_data_type)
        {
            case Seq_code_iupacna:
                tmp = SEQ_DATA_iupacna;
                break;
            case Seq_code_iupacaa:
                tmp = SEQ_DATA_iupacaa;
                break;
            case Seq_code_ncbi2na:
                tmp = SEQ_DATA_ncbi2na;
                break;
            case Seq_code_ncbi4na:
                tmp = SEQ_DATA_ncbi4na;
                break;
            case Seq_code_ncbi8na:
                tmp = SEQ_DATA_ncbi8na;
                break;
            case Seq_code_ncbipna:
                tmp = SEQ_DATA_ncbipna;
                break;
            case Seq_code_ncbi8aa:
                tmp = SEQ_DATA_ncbi8aa;
                break;
            case Seq_code_ncbieaa:
                tmp = SEQ_DATA_ncbieaa;
                break;
            case Seq_code_ncbipaa:
                tmp = SEQ_DATA_ncbipaa;
                break;
            case Seq_code_ncbistdaa:
                tmp = SEQ_DATA_ncbistdaa;
                break;
        }
        if (! AsnWrite(aip, tmp, &av)) goto erret;
    }
    if (bsp->seq_ext != NULL)
    {
		av.ptrvalue = (Pointer)bsp->seq_ext;
        if (! AsnWriteChoice(aip, SEQ_INST_ext, (Int2)bsp->seq_ext_type, &av)) goto erret;
        switch (bsp->seq_ext_type)
        {
            case 1:		/* seg */
                if (! SeqLocSetAsnWrite((ValNodePtr)bsp->seq_ext, aip, SEQ_EXT_seg, SEG_EXT_E))
                    goto erret;
                break;
            case 2:	   /* ref */
                if (! SeqLocAsnWrite((ValNodePtr)bsp->seq_ext, aip, SEQ_EXT_ref)) goto erret;
                break;
            case 3:              /* map ext */
                if (! SeqFeatSetAsnWrite((SeqFeatPtr)bsp->seq_ext, aip, SEQ_EXT_map, MAP_EXT_E)) goto erret;
                break;
        }
    }

    if (bsp->hist != NULL)              /* history optional */
    {
        if (! SeqHistAsnWrite(bsp->hist, aip, SEQ_INST_hist))
            goto erret;
    }

    if (! AsnCloseStruct(aip, atp, (Pointer)bsp)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   BioseqInstAsnRead(bsp, aip, atp)   
*
*****************************************************************************/
Boolean LIBCALL BioseqInstAsnRead (BioseqPtr bsp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Uint1 choice;
	Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return FALSE;
	}

	if ((aip == NULL) || (bsp == NULL))
		return FALSE;

	if (! ProgMon("Read SeqInst"))
		return FALSE;

	if (orig == NULL)           /* Seq-inst ::= (self contained) */
		atp = AsnReadId(aip, amp, SEQ_INST);
	else
		atp = AsnLinkType(orig, SEQ_INST);    /* link in local tree */
    if (atp == NULL) goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;         /* inst required */

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    bsp->repr = (Uint1) av.intvalue;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    bsp->mol = (Uint1) av.intvalue;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;

    if (atp == SEQ_INST_length)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        bsp->length = av.intvalue;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }

    if (atp == SEQ_INST_fuzz)
    {
        bsp->fuzz = IntFuzzAsnRead(aip, atp);
        if (bsp->fuzz == NULL) goto erret;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }

    if (atp == SEQ_INST_topology)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        bsp->topology = (Uint1) av.intvalue;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }

    if (atp == SEQ_INST_strand)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        bsp->strand = (Uint1) av.intvalue;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }

    if (atp == SEQ_INST_seq_data)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* CHOICE */
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;   /* what kind of choice? */
        choice = 0;
        if (atp == SEQ_DATA_iupacna)
            choice = Seq_code_iupacna;
        else if (atp == SEQ_DATA_iupacaa)
            choice = Seq_code_iupacaa;
        else if (atp == SEQ_DATA_ncbi2na)
            choice = Seq_code_ncbi2na;
        else if (atp == SEQ_DATA_ncbi4na)
            choice = Seq_code_ncbi4na;
        else if (atp == SEQ_DATA_ncbi8na)
            choice = Seq_code_ncbi8na;
        else if (atp == SEQ_DATA_ncbipna)
            choice = Seq_code_ncbipna;
        else if (atp == SEQ_DATA_ncbi8aa)
            choice = Seq_code_ncbi8aa;
        else if (atp == SEQ_DATA_ncbieaa)
            choice = Seq_code_ncbieaa;
        else if (atp == SEQ_DATA_ncbipaa)
            choice = Seq_code_ncbipaa;
        else if (atp == SEQ_DATA_ncbistdaa)
            choice = Seq_code_ncbistdaa;
        bsp->seq_data_type = choice;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        bsp->seq_data = (ByteStorePtr)av.ptrvalue;

        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }

    if (atp == SEQ_INST_ext)            /* extensions */
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* CHOICE */
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;   /* what choice? */
        choice = 0;
        if (atp == SEQ_EXT_seg)
            choice = 1;
        else if (atp == SEQ_EXT_ref)
            choice = 2;
        else if (atp == SEQ_EXT_map)
            choice = 3;
        bsp->seq_ext_type = choice;
        switch (choice)
        {
            case 1:
                bsp->seq_ext = (Pointer) SeqLocSetAsnRead(aip, atp, SEG_EXT_E);
                break;
            case 2:
                bsp->seq_ext = (Pointer) SeqLocAsnRead(aip, atp);
                break;
            case 3:
                bsp->seq_ext = (Pointer) SeqFeatSetAsnRead(aip, atp, MAP_EXT_E);
                break;
        }
        if (bsp->seq_ext == NULL) goto erret;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }

    if (atp == SEQ_INST_hist)
	{
        bsp->hist = SeqHistAsnRead(aip, atp);
        if (bsp->hist == NULL) goto erret;
		atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
	}

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct inst */
    retval = TRUE;
erret:
    AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}
/*****************************************************************************
*
*   SeqDescrFree(anp)
*       Frees a chain of SeqDescr and associated data
*
*****************************************************************************/
ValNodePtr LIBCALL SeqDescrFree (ValNodePtr anp)
{
    ValNodePtr next;
    Pointer pnt;

    while (anp != NULL)
    {
        next = anp->next;
        pnt = anp->data.ptrvalue;
        switch (anp->choice)
        {
            case 1:     /* mol-type */
            case 3:      /* method */
                break;
            case 2:     /* modif */
                ValNodeFree((ValNodePtr)pnt);
                break;
            case 4:      /* name */
            case 5:      /* title */
            case 7:      /* comment */
            case 13:     /* region */
			case 22:     /* heterogen */
                MemFree(pnt);
                break;
            case 6:      /* org */
                OrgRefFree((OrgRefPtr)pnt);
                break;
            case 8:      /* num */
                NumberingFree((ValNodePtr)pnt);
                break;
            case 9:      /* maploc */
                DbtagFree((DbtagPtr)pnt);
                break;
            case 10:     /* pir */
                PirBlockFree((PirBlockPtr)pnt);
                break;
            case 11:     /* genbank */
                GBBlockFree((GBBlockPtr)pnt);
                break;
            case 12:     /* pub */
                PubdescFree((PubdescPtr)pnt);
                break;
            case 14:     /* user */
                UserObjectFree((UserObjectPtr)pnt);
                break;
            case 15:     /* SP */
                SPBlockFree((SPBlockPtr)pnt);
                break;
            case 16:     /* neighbors */
                LinkSetFree((LinkSetPtr)pnt);
                break;
            case 17:     /* EMBL */
                EMBLBlockFree((EMBLBlockPtr)pnt);
                break;
			case 18:     /* create-date */
			case 19:     /* update-date */
				DateFree((DatePtr)pnt);
				break;
            case 20:     /* PRF */
                PrfBlockFree((PrfBlockPtr)pnt);
                break;
            case 21:     /* PDB */
                PdbBlockFree((PdbBlockPtr)pnt);
                break;
        }
        MemFree(anp);
        anp = next;
    }
	return anp;
}

/*****************************************************************************
*
*   SeqDescrAsnWrite(anp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SeqDescr ::=)
*
*****************************************************************************/
Boolean LIBCALL SeqDescrAsnWrite (ValNodePtr anp, AsnIoPtr aip, AsnTypePtr orig)
{
	AsnTypePtr atp;
    ValNodePtr oldanp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	if (! ProgMon("Write SeqDescr"))
		return FALSE;

	atp = AsnLinkType(orig, SEQ_DESCR);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

	oldanp = anp;
    if (! AsnOpenStruct(aip, atp, (Pointer)oldanp)) goto erret; /* SET OF */

    while (anp != NULL)
    {
		if (! SeqDescrEAsnWrite (anp, aip, SEQ_DESCR_E)) goto erret;
        anp = anp->next;
    }
    if (! AsnCloseStruct(aip, atp, oldanp)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqDescrEAsnWrite(anp, aip, orig)
*   	Writes one Seqdescr Element
*
*****************************************************************************/
Boolean LIBCALL SeqDescrEAsnWrite (ValNodePtr anp, AsnIoPtr aip, AsnTypePtr orig)
{
    DataValPtr dvp;
	AsnTypePtr atp;
    Pointer pnt;
    ValNodePtr tmp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, SEQ_DESCR_E);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

   dvp = &anp->data;
		
   if (! AsnWriteChoice(aip, atp, (Int2)anp->choice, dvp)) goto erret;
   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
       case 1:                   /* moltype */
           if (! AsnWrite(aip, SEQ_DESCR_E_mol_type, dvp)) goto erret;
           break;
       case 2:                   /* modif */
           if (! AsnOpenStruct(aip, SEQ_DESCR_E_modif, anp->data.ptrvalue)) goto erret;
           tmp = (ValNodePtr)anp->data.ptrvalue;
           while (tmp != NULL)
           {
               if (! AsnWrite(aip, SEQ_DESCR_E_modif_E, &tmp->data)) goto erret;
               tmp = tmp->next;
           }
           if (! AsnCloseStruct(aip, SEQ_DESCR_E_modif, anp->data.ptrvalue)) goto erret;
           break;
       case 3:                   /* method */
           if (! AsnWrite(aip, SEQ_DESCR_E_method, dvp)) goto erret;
           break;
       case 4:                   /* name */
           if (! AsnWrite(aip, SEQ_DESCR_E_name, dvp)) goto erret;
           break;
       case 5:                   /* title */
           if (! AsnWrite(aip, SEQ_DESCR_E_title, dvp)) goto erret;
           break;
       case 6:                   /* org */
           if (! OrgRefAsnWrite((OrgRefPtr)pnt, aip, SEQ_DESCR_E_org)) goto erret;
           break;
       case 7:                   /* comment */
           if (! AsnWrite(aip, SEQ_DESCR_E_comment, dvp)) goto erret;
           break;
       case 8:                   /* num */
           if (! NumberingAsnWrite((ValNodePtr)pnt, aip, SEQ_DESCR_E_num)) goto erret;
           break;
       case 9:                  /* maploc */
           if (! DbtagAsnWrite((DbtagPtr)pnt, aip, SEQ_DESCR_E_maploc)) goto erret;
           break;
       case 10:                   /* pir */
           if (! PirBlockAsnWrite((PirBlockPtr)pnt, aip, SEQ_DESCR_E_pir)) goto erret;
           break;
       case 11:                  /* genbank */
           if (! GBBlockAsnWrite((GBBlockPtr)pnt, aip, SEQ_DESCR_E_genbank)) goto erret;
           break;
       case 12:                  /* pubdesc */
           if (! PubdescAsnWrite((PubdescPtr)pnt, aip, SEQ_DESCR_E_pub)) goto erret;
           break;
       case 13:                  /* region */
           if (! AsnWrite(aip, SEQ_DESCR_E_region, dvp)) goto erret;
           break;
       case 14:                   /* user */
           if (! UserObjectAsnWrite((UserObjectPtr)pnt, aip, SEQ_DESCR_E_user)) goto erret;
           break;
       case 15:                   /* SP */
           if (! SPBlockAsnWrite((SPBlockPtr)pnt, aip, SEQ_DESCR_E_sp)) goto erret;
           break;
       case 16:                   /* neighbors */
           if (! LinkSetAsnWrite((LinkSetPtr)pnt, aip, SEQ_DESCR_E_neighbors)) goto erret;
           break;
       case 17:                   /* EMBL */
           if (! EMBLBlockAsnWrite((EMBLBlockPtr)pnt, aip, SEQ_DESCR_E_embl)) goto erret;
           break;
	case 18:                   /* create-date */
		if (! DateAsnWrite((DatePtr)pnt, aip, SEQ_DESCR_E_create_date)) goto erret;
		break;
	case 19:                   /* update-date */
		if (! DateAsnWrite((DatePtr)pnt, aip, SEQ_DESCR_E_update_date)) goto erret;
		break;
       case 20:                   /* PRF */
           if (! PrfBlockAsnWrite((PrfBlockPtr)pnt, aip, SEQ_DESCR_E_prf)) goto erret;
           break;
       case 21:                   /* PDB */
           if (! PdbBlockAsnWrite((PdbBlockPtr)pnt, aip, SEQ_DESCR_E_pdb)) goto erret;
           break;
	case 22:                   /* Heterogen */
           if (! AsnWrite(aip, SEQ_DESCR_E_het, dvp)) goto erret;
           break;
   }
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqDescrAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
ValNodePtr LIBCALL SeqDescrAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    ValNodePtr anp, first=NULL, tmp, tmp2;
    Uint1 choice;
    AsnReadFunc func;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return first;
	}

	if (aip == NULL)
		return first;

	if (! ProgMon("Read SeqDescr"))
		return first;

	if (orig == NULL)           /* SeqDescr ::= (self contained) */
		atp = AsnReadId(aip, amp, SEQ_DESCR);
	else
		atp = AsnLinkType(orig, SEQ_DESCR);    /* link in local tree */
    if (atp == NULL) return first;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* read the start struct */

    while ((atp = AsnReadId(aip, amp, atp)) == SEQ_DESCR_E)
    {
		if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the CHOICE value (nothing) */
		atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;  /* find the choice */

        func = NULL;
        choice = 0;
		if (atp == SEQ_DESCR_E_mol_type)
        {
            choice = 1;
        }
        else if (atp == SEQ_DESCR_E_modif)
        {
            choice = 2;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* start struct */
            tmp = NULL;
            while ((atp = AsnReadId(aip, amp, atp)) == SEQ_DESCR_E_modif_E)
            {
                if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
                tmp2 = ValNodeNew(tmp);
                if (tmp2 == NULL) goto erret;
                if (tmp == NULL)
                    tmp = tmp2;
                tmp2->data.intvalue = av.intvalue;
            }
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
        }
        else if (atp == SEQ_DESCR_E_method)
        {
            choice = 3;
        }
        else if (atp == SEQ_DESCR_E_name)
        {
            choice = 4;
        }
        else if (atp == SEQ_DESCR_E_title)
        {
            choice = 5;
        }
        else if (atp == SEQ_DESCR_E_org)
        {
            choice = 6;
            func = (AsnReadFunc) OrgRefAsnRead;
        }
        else if (atp == SEQ_DESCR_E_comment)
        {
            choice = 7;
        }
        else if (atp == SEQ_DESCR_E_num)
        {
            choice = 8;
            func = (AsnReadFunc) NumberingAsnRead;
        }
        else if (atp == SEQ_DESCR_E_maploc)
        {
            choice = 9;
            func = (AsnReadFunc) DbtagAsnRead;
        }
        else if (atp == SEQ_DESCR_E_pir)
        {
            choice = 10;
            func = (AsnReadFunc) PirBlockAsnRead;
        }
        else if (atp == SEQ_DESCR_E_genbank)
        {
            choice = 11;
            func = (AsnReadFunc) GBBlockAsnRead;
        }
        else if (atp == SEQ_DESCR_E_pub)
        {
            choice = 12;
            func = (AsnReadFunc) PubdescAsnRead;
        }
        else if (atp == SEQ_DESCR_E_region)
        {
            choice = 13;
        }
        else if (atp == SEQ_DESCR_E_user)
        {
            choice = 14;
            func = (AsnReadFunc) UserObjectAsnRead;
        }
        else if (atp == SEQ_DESCR_E_sp)
        {
            choice = 15;
            func = (AsnReadFunc) SPBlockAsnRead;
        }
        else if (atp == SEQ_DESCR_E_neighbors)
        {
            choice = 16;
            func = (AsnReadFunc) LinkSetAsnRead;
        }
        else if (atp == SEQ_DESCR_E_embl)
        {
            choice = 17;
            func = (AsnReadFunc) EMBLBlockAsnRead;
        }
		else if (atp == SEQ_DESCR_E_create_date)
		{
			choice = 18;
			func = (AsnReadFunc) DateAsnRead;
		}
		else if (atp == SEQ_DESCR_E_update_date)
		{
			choice = 19;
			func = (AsnReadFunc) DateAsnRead;
		}
        else if (atp == SEQ_DESCR_E_prf)
        {
            choice = 20;
            func = (AsnReadFunc) PrfBlockAsnRead;
        }
        else if (atp == SEQ_DESCR_E_pdb)
        {
            choice = 21;
            func = (AsnReadFunc) PdbBlockAsnRead;
        }
        else if (atp == SEQ_DESCR_E_het)
        {
            choice = 22;
        }

    	anp = ValNodeNew(first);
        if (anp == NULL) goto erret;
        anp->choice = choice;
        if (func != NULL)
        {
            anp->data.ptrvalue = (* func)(aip, atp);
            if (anp->data.ptrvalue == NULL) goto erret;
        }
        else if (choice != 2)
        {
            if (AsnReadVal(aip, atp, &anp->data) <= 0) goto erret;
        }
        else
            anp->data.ptrvalue = (Pointer)tmp;

        if (first == NULL)
            first = anp;
    }
    if (atp == NULL) goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return first;
erret:
    first = SeqDescrFree(first);
    goto ret;
}

/*****************************************************************************
*
*   NumberingFree(anp)
*       Frees a chain of Numbering and associated data
*
*****************************************************************************/
NumberingPtr LIBCALL NumberingFree (NumberingPtr anp)
{
    Pointer pnt;

    if (anp == NULL)
        return anp;
    
    pnt = anp->data.ptrvalue;
    switch (anp->choice)
    {
        case 1:     /* num-cont */
            NumContFree((NumContPtr)pnt);
            break;
        case 2:      /* num-enum */
            NumEnumFree((NumEnumPtr)pnt);
            break;
        case 3:     /* ref, sources */
        case 4:      /* ref, aligns */
            if (pnt != NULL)
                SeqAlignFree((SeqAlignPtr)pnt);
            break;
        case 5:     /* real */
            NumRealFree((NumRealPtr)pnt);
            break;
    }
    
	return (NumberingPtr)MemFree(anp);
}
/*****************************************************************************
*
*   NumberingAsnWrite(anp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (Numbering ::=)
*
*****************************************************************************/
Boolean LIBCALL NumberingAsnWrite (NumberingPtr anp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Pointer pnt;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, NUMBERING);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

	av.ptrvalue = (Pointer)anp;
    if (! AsnWriteChoice(aip, atp, (Int2)anp->choice, &av)) goto erret;

    pnt = anp->data.ptrvalue;
    switch (anp->choice)
    {
        case 1:                   /* num-cont */
            if (! NumContAsnWrite((NumContPtr)pnt, aip, NUMBERING_cont)) goto erret;
            break;
        case 2:                   /* num-enum */
            if (! NumEnumAsnWrite((NumEnumPtr)pnt, aip, NUMBERING_enum)) goto erret;
            break;
        case 3:                   /* num-ref, sources */
        case 4:                   /* num-ref, aligns */
            if (! AsnOpenStruct(aip, NUMBERING_ref, pnt)) goto erret;
            av.intvalue = anp->choice - 2;
            if (! AsnWrite(aip, NUM_REF_type, &av)) goto erret;
            if (pnt != NULL)
            {
                if (! SeqAlignAsnWrite((SeqAlignPtr)pnt, aip, NUM_REF_aligns)) goto erret;
            }
            if (! AsnCloseStruct(aip, NUMBERING_ref, pnt)) goto erret;
            break;
        case 5:                   /* real */
            if (! NumRealAsnWrite((NumRealPtr)pnt, aip, NUMBERING_real)) goto erret;
            break;
    }
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}
/*****************************************************************************
*
*   NumberingAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
NumberingPtr LIBCALL NumberingAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    NumberingPtr anp = NULL;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return anp;
	}

	if (aip == NULL)
		return anp;

	if (orig == NULL)           /* Numbering ::= (self contained) */
		atp = AsnReadId(aip, amp, NUMBERING);
	else
		atp = AsnLinkType(orig, NUMBERING);    /* link in local tree */
    if (atp == NULL) return anp;

    anp = ValNodeNew(NULL);
    if (anp == NULL) goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* read the CHOICE */
    
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;   /* kind of choice */
    if (atp == NUMBERING_cont)
    {
        anp->choice = 1;
        anp->data.ptrvalue = (Pointer) NumContAsnRead(aip, atp);
    }
    else if (atp == NUMBERING_enum)
    {
        anp->choice = 2;
        anp->data.ptrvalue = (Pointer) NumEnumAsnRead(aip, atp);
    }
    else if (atp == NUMBERING_real)
    {
        anp->choice = 5;
        anp->data.ptrvalue = (Pointer) NumRealAsnRead(aip, atp);
    }
    else if (atp == NUMBERING_ref)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* start struct */
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* type */
        if (av.intvalue == 1)
            anp->choice = 3;
        else if (av.intvalue == 2)
            anp->choice = 4;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
        if (atp == NUM_REF_aligns)
            anp->data.ptrvalue = (Pointer) SeqAlignAsnRead(aip, atp);
    }
    if (anp->data.ptrvalue == NULL) goto erret;
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return anp;
erret:
    anp = NumberingFree(anp);
    goto ret;
}

/*****************************************************************************
*
*   NumContNew()
*
*****************************************************************************/
NumContPtr LIBCALL NumContNew ()
{
    NumContPtr ncp;
    ncp = (NumContPtr)MemNew(sizeof(NumCont));
	if (ncp == NULL) return NULL;

    ncp->ascending = TRUE;   /* default states */
    ncp->refnum = 1;
    return ncp;
}

/*****************************************************************************
*
*   NumContFree(ncp)
*       Frees a NumCont and associated data
*
*****************************************************************************/
NumContPtr LIBCALL NumContFree (NumContPtr ncp)
{
    if (ncp == NULL)
        return ncp;

	return (NumContPtr)MemFree(ncp);
}
/*****************************************************************************
*
*   NumContAsnWrite(ncp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (NumCont ::=)
*
*****************************************************************************/
Boolean LIBCALL NumContAsnWrite (NumContPtr ncp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, NUM_CONT);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (ncp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)ncp)) goto erret;
    if (ncp->refnum != 1)
    {
        av.intvalue = ncp->refnum;
        if (! AsnWrite(aip, NUM_CONT_refnum, &av)) goto erret;
    }
    if (ncp->has_zero)
    {
        av.boolvalue = ncp->has_zero;
        if (! AsnWrite(aip, NUM_CONT_has_zero, &av)) goto erret;
    }
    if (! ncp->ascending)
    {
        av.boolvalue = ncp->ascending;
        if (! AsnWrite(aip, NUM_CONT_ascending, &av)) goto erret;
    }

    if (! AsnCloseStruct(aip, atp, (Pointer)ncp)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   NumContAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
NumContPtr LIBCALL NumContAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    NumContPtr ncp=NULL;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return ncp;
	}

	if (aip == NULL)
		return ncp;

	if (orig == NULL)           /* NumCont ::= (self contained) */
		atp = AsnReadId(aip, amp, NUM_CONT);
	else
		atp = AsnLinkType(orig, NUM_CONT);    /* link in local tree */
    oldatp = atp;
    if (atp == NULL) return ncp;

    ncp = NumContNew();
    if (ncp == NULL) goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* read the start struct */
    
    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL) goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        if (atp == NUM_CONT_refnum)
            ncp->refnum = av.intvalue;
        else if (atp == NUM_CONT_has_zero)
            ncp->has_zero = av.boolvalue;
        else if (atp == NUM_CONT_ascending)
            ncp->ascending = av.boolvalue;
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return ncp;
erret:
    ncp = NumContFree(ncp);
    goto ret;
}

/*****************************************************************************
*
*   NumEnumNew()
*
*****************************************************************************/
NumEnumPtr LIBCALL NumEnumNew (void)
{
    NumEnumPtr nep;
    nep = (NumEnumPtr)MemNew(sizeof(NumEnum));
    return nep;
}

/*****************************************************************************
*
*   NumEnumFree(nep)
*       Frees a NumEnum and associated data
*
*****************************************************************************/
NumEnumPtr LIBCALL NumEnumFree (NumEnumPtr nep)
{
    if (nep == NULL)
        return nep;
    
    MemFree(nep->buf);
    MemFree(nep->names);
	return (NumEnumPtr)MemFree(nep);
}

/*****************************************************************************
*
*   NumEnumAsnWrite(nep, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (NumEnum ::=)
*
*****************************************************************************/
Boolean LIBCALL NumEnumAsnWrite (NumEnumPtr nep, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Int4 i, num;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, NUM_ENUM);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (nep == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)nep)) goto erret;

    num = nep->num;
    av.intvalue = num;
    if (! AsnWrite(aip, NUM_ENUM_num, &av)) goto erret;
    if (! AsnOpenStruct(aip, NUM_ENUM_names, (Pointer)nep->names)) goto erret;
    for (i = 0; i < num; i++)
    {
        av.ptrvalue = (Pointer) nep->names[i];
        if (! AsnWrite(aip, NUM_ENUM_names_E, &av)) goto erret;
    }
    if (! AsnCloseStruct(aip, NUM_ENUM_names, (Pointer)nep->names)) goto erret;

    if (! AsnCloseStruct(aip, atp, (Pointer)nep)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   NumEnumAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
NumEnumPtr LIBCALL NumEnumAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    NumEnumPtr nep=NULL;
    Int4 num, i, total, used, oldtotal, j;
    CharPtr pnt, tmp, tmp2;
    CharPtr PNTR names2;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return nep;
	}

	if (aip == NULL)
		return nep;

	if (orig == NULL)           /* NumEnum ::= (self contained) */
		atp = AsnReadId(aip, amp, NUM_ENUM);
	else
		atp = AsnLinkType(orig, NUM_ENUM);    /* link in local tree */
    if (atp == NULL) return nep;

    nep = NumEnumNew();
    if (nep == NULL) goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* read the start struct */
    
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;   /* get num */
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    num = av.intvalue;
    nep->num = num;
    i = 0;
    total = num * 4;
    used = 0;
    nep->buf = (CharPtr)MemNew((size_t)total);   /* assume an average of 3 chars/name */
    if (nep->buf == NULL) goto erret;
    nep->names = (CharPtr PNTR)MemNew((size_t)(num * sizeof(CharPtr)));
    if (nep->names == NULL) goto erret;
    pnt = nep->buf;
    names2 = nep->names;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;   /* name list */
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;

    while ((atp = AsnReadId(aip, amp, atp)) == NUM_ENUM_names_E)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        tmp = (CharPtr)av.ptrvalue;
        used += StringLen(tmp) + 1;
        if (i == num)       /* out of pointers -- num is in ERROR */
        {
            num += 10;
            names2 = nep->names;
            nep->names = (CharPtr PNTR)MemNew((size_t)(num * sizeof(CharPtr)));
            if (nep->names == NULL)
            {
                nep->names = names2;
                goto erret;
            }
            MemCopy(nep->names, names2, (size_t)(i * sizeof(CharPtr)));
            MemFree(names2);
            names2 = nep->names;
        }
        if (used > total)     /* out of buffer space */
        {
            oldtotal = total;
            total += 100;
            tmp2 = nep->buf;
            nep->buf = (CharPtr)MemNew((size_t)total);
            if (nep->buf == NULL)
            {
                nep->buf = tmp2;
                goto erret;
            }
            MemCopy(nep->buf, tmp2, (size_t)oldtotal);
            MemFree(tmp2);
            pnt = nep->buf;
            for (j = 0; j < i; j++)
            {
                names2[j] = pnt;
                while (* pnt != '\0')
                    pnt++;
                pnt++;
            }
        }
        names2[i] = pnt;
        pnt = StringMove(pnt, tmp);
        pnt++;   /* move past \0 */
        i++;
        MemFree(tmp);
    }
    if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end struct names */

    nep->num = i;    /* record real count of names -- self correcting */

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return nep;
erret:
    nep = NumEnumFree(nep);
    goto ret;
}

/*****************************************************************************
*
*   NumRealNew()
*
*****************************************************************************/
NumRealPtr LIBCALL NumRealNew (void)
{
    return (NumRealPtr)MemNew(sizeof(NumReal));
}

/*****************************************************************************
*
*   NumRealFree(nrp)
*       Frees a NumReal and associated data
*
*****************************************************************************/
NumRealPtr LIBCALL NumRealFree (NumRealPtr nrp)
{
    if (nrp == NULL)
        return nrp;

    MemFree(nrp->units);
	return (NumRealPtr)MemFree(nrp);
}

/*****************************************************************************
*
*   NumRealAsnWrite(nrp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (NumReal ::=)
*
*****************************************************************************/
Boolean LIBCALL NumRealAsnWrite (NumRealPtr nrp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, NUM_REAL);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (nrp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)nrp)) goto erret;

    av.realvalue = nrp->a;
    if (! AsnWrite(aip, NUM_REAL_a, &av)) goto erret;

    av.realvalue = nrp->b;
    if (! AsnWrite(aip, NUM_REAL_b, &av)) goto erret;

    if (nrp->units != NULL)
    {
        av.ptrvalue = nrp->units;
        if (! AsnWrite(aip, NUM_REAL_units, &av)) goto erret;
    }

    if (! AsnCloseStruct(aip, atp, (Pointer)nrp)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   NumRealAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
NumRealPtr LIBCALL NumRealAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    NumRealPtr nrp=NULL;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return nrp;
	}

	if (aip == NULL)
		return nrp;

	if (orig == NULL)           /* NumReal ::= (self contained) */
		atp = AsnReadId(aip, amp, NUM_REAL);
	else
		atp = AsnLinkType(orig, NUM_REAL);    /* link in local tree */
    oldatp = atp;
    if (atp == NULL) return nrp;

    nrp = NumRealNew();
    if (nrp == NULL) goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* read the start struct */
    
    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL) goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        if (atp == NUM_REAL_a)
            nrp->a = av.realvalue;
        else if (atp == NUM_REAL_b)
            nrp->b = av.realvalue;
        else if (atp == NUM_REAL_units)
            nrp->units = (CharPtr)av.ptrvalue;
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return nrp;
erret:
    nrp = NumRealFree(nrp);
    goto ret;
}

/*****************************************************************************
*
*   PubdescNew()
*
*****************************************************************************/
PubdescPtr LIBCALL PubdescNew (void)
{
    return (PubdescPtr)MemNew(sizeof(Pubdesc));
}

/*****************************************************************************
*
*   PubdescFree(pdp)
*       Frees a Pubdesc and associated data
*
*****************************************************************************/
PubdescPtr LIBCALL PubdescFree (PubdescPtr pdp)
{
    if (pdp == NULL)
        return pdp;

    PubEquivFree(pdp->pub);
    MemFree(pdp->name);
    MemFree(pdp->fig);
    NumberingFree(pdp->num);
    MemFree(pdp->maploc);
    MemFree(pdp->seq_raw);
	MemFree(pdp->comment);
	return (PubdescPtr)MemFree(pdp);
}

/*****************************************************************************
*
*   PubdescAsnWrite(pdp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (Pubdesc ::=)
*
*****************************************************************************/
Boolean LIBCALL PubdescAsnWrite (PubdescPtr pdp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, PUBDESC);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (pdp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)pdp)) goto erret;
    if (! PubEquivAsnWrite(pdp->pub, aip, PUBDESC_pub)) goto erret;

    if (pdp->name != NULL)
    {
        av.ptrvalue = pdp->name;
        if (! AsnWrite(aip, PUBDESC_name, &av)) goto erret;
    }
    if (pdp->fig != NULL)
    {
        av.ptrvalue = pdp->fig;
        if (! AsnWrite(aip, PUBDESC_fig, &av)) goto erret;
    }
    if (pdp->num != NULL)
    {
        if (! NumberingAsnWrite(pdp->num, aip, PUBDESC_num)) goto erret;
    }
    if (pdp->numexc)
    {
        av.boolvalue = pdp->numexc;
        if (! AsnWrite(aip, PUBDESC_numexc, &av)) goto erret;
    }
    if (pdp->poly_a)
    {
        av.boolvalue = pdp->poly_a;
        if (! AsnWrite(aip, PUBDESC_poly_a, &av)) goto erret;
    }
    if (pdp->maploc != NULL)
    {
        av.ptrvalue = pdp->maploc;
        if (! AsnWrite(aip, PUBDESC_maploc, &av)) goto erret;
    }
    if (pdp->seq_raw != NULL)
    {
        av.ptrvalue = pdp->seq_raw;
        if (! AsnWrite(aip, PUBDESC_seq_raw, &av)) goto erret;
    }
    if (pdp->align_group)
    {
        av.intvalue = (Int4)pdp->align_group;
        if (! AsnWrite(aip, PUBDESC_align_group, &av)) goto erret;
    }
    if (pdp->comment != NULL)
    {
        av.ptrvalue = pdp->comment;
        if (! AsnWrite(aip, PUBDESC_comment, &av)) goto erret;
    }

    if (! AsnCloseStruct(aip, atp, (Pointer)pdp)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   PubdescAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
PubdescPtr LIBCALL PubdescAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    PubdescPtr pdp=NULL;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return pdp;
	}

	if (aip == NULL)
		return pdp;

	if (orig == NULL)           /* Pubdesc ::= (self contained) */
		atp = AsnReadId(aip, amp, PUBDESC);
	else
		atp = AsnLinkType(orig, PUBDESC);    /* link in local tree */
    oldatp = atp;
    if (atp == NULL) return pdp;

    pdp = PubdescNew();
    if (pdp == NULL) goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* read the start struct */
    
    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL) goto erret;
        if (atp == PUBDESC_pub)
        {
            pdp->pub = PubEquivAsnRead(aip, atp);
            if (pdp->pub == NULL) goto erret;
        }
        else if (atp == PUBDESC_num)
        {
            pdp->num = NumberingAsnRead(aip, atp);
            if (pdp->num == NULL) goto erret;
        }
        else
        {
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
            if (atp == PUBDESC_name)
                pdp->name = (CharPtr)av.ptrvalue;
            else if (atp == PUBDESC_fig)
                pdp->fig = (CharPtr)av.ptrvalue;
            else if (atp == PUBDESC_numexc)
                pdp->numexc = av.boolvalue;
            else if (atp == PUBDESC_poly_a)
                pdp->poly_a = av.boolvalue;
            else if (atp == PUBDESC_maploc)
                pdp->maploc = (CharPtr)av.ptrvalue;
            else if (atp == PUBDESC_seq_raw)
                pdp->seq_raw = (CharPtr)av.ptrvalue;
            else if (atp == PUBDESC_align_group)
                pdp->align_group = (Uint1)av.intvalue;
			else if (atp == PUBDESC_comment)
				pdp->comment = (CharPtr)av.ptrvalue;
        }
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return pdp;
erret:
    pdp = PubdescFree(pdp);
    goto ret;
}

/*****************************************************************************
*
*   SeqAnnotNew()
*
*****************************************************************************/
SeqAnnotPtr LIBCALL SeqAnnotNew (void)
{
    return (SeqAnnotPtr)MemNew(sizeof(SeqAnnot));
}

/*****************************************************************************
*
*   SeqAnnotFree(sap)
*       Frees a SeqAnnot and associated data
*
*****************************************************************************/
SeqAnnotPtr LIBCALL SeqAnnotFree (SeqAnnotPtr sap)
{
    SeqFeatPtr sfp, sfpnext;
    SeqAlignPtr sp, spnext;
    SeqGraphPtr sgp, sgpnext;

    if (sap == NULL)
        return sap;

    ObjectIdFree(sap->id);
    DbtagFree(sap->db);
    MemFree(sap->name);
    MemFree(sap->desc);
    switch (sap->type)
    {
        case 1:   /* ftable */
            sfp = (SeqFeatPtr)sap->data;
            while (sfp != NULL)
            {
				sfpnext = sfp->next;
                SeqFeatFree(sfp);
                sfp = sfpnext;
            }
            break;
        case 2:   /* alignments */
            sp = (SeqAlignPtr)sap->data;
            while (sp != NULL)
            {
				spnext = sp->next;
                SeqAlignFree(sp);
                sp = spnext;
            }
            break;
        case 3:
            sgp = (SeqGraphPtr)sap->data;
            while (sgp != NULL)
            {
				sgpnext = sgp->next;
                SeqGraphFree(sgp);
                sgp = sgpnext;
            }
            break;
    }
	return (SeqAnnotPtr)MemFree(sap);
}

/*****************************************************************************
*
*   SeqAnnotAsnWrite(sap, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SeqAnnot ::=)
*
*****************************************************************************/
Boolean LIBCALL SeqAnnotAsnWrite (SeqAnnotPtr sap, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	if (! ProgMon("Write SeqAnnot"))
		return FALSE;

	atp = AsnLinkType(orig, SEQ_ANNOT);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (sap == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)sap)) goto erret;

    if (sap->id != NULL)
    {
        if (! ObjectIdAsnWrite(sap->id, aip, SEQ_ANNOT_id)) goto erret;
    }
    if (sap->db != NULL)
    {
        if (! DbtagAsnWrite(sap->db, aip, SEQ_ANNOT_db)) goto erret;
    }
    if (sap->name != NULL)
    {
        av.ptrvalue = sap->name;
        if (! AsnWrite(aip, SEQ_ANNOT_name, &av)) goto erret;
    }
    if (sap->desc != NULL)
    {
        av.ptrvalue = sap->desc;
        if (! AsnWrite(aip, SEQ_ANNOT_desc, &av)) goto erret;
    }
	av.ptrvalue = sap->data;
    if (! AsnWriteChoice(aip, SEQ_ANNOT_data, (Int2)sap->type, &av)) goto erret;
    switch (sap->type)
    {
        case 1:
            if (! SeqFeatSetAsnWrite((SeqFeatPtr)sap->data, aip, SEQ_ANNOT_data_ftable, SEQ_ANNOT_data_ftable_E)) goto erret;
            break;
        case 2:
            if (! SeqAlignSetAsnWrite((SeqAlignPtr)sap->data, aip, SEQ_ANNOT_data_align, SEQ_ANNOT_data_align_E)) goto erret;
            break;
        case 3:
            if (! SeqGraphSetAsnWrite((SeqGraphPtr)sap->data, aip, SEQ_ANNOT_data_graph, SEQ_ANNOT_data_graph_E)) goto erret;
            break;
    }

    if (! AsnCloseStruct(aip, atp, (Pointer)sap)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqAnnotAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
SeqAnnotPtr LIBCALL SeqAnnotAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    SeqAnnotPtr sap=NULL;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return sap;
	}

	if (aip == NULL)
		return sap;

	if (! ProgMon("Read SeqAnnot"))
		return sap;

	if (orig == NULL)           /* SeqAnnot ::= (self contained) */
		atp = AsnReadId(aip, amp, SEQ_ANNOT);
	else
		atp = AsnLinkType(orig, SEQ_ANNOT);    /* link in local tree */
    if (atp == NULL) return sap;

    sap = SeqAnnotNew();
    if (sap == NULL) goto erret;
    
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* read the start struct */
    
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (atp == SEQ_ANNOT_id)
    {
        sap->id = ObjectIdAsnRead(aip, atp);
        if (sap->id == NULL) goto erret;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }
    if (atp == SEQ_ANNOT_db)
    {
        sap->db = DbtagAsnRead(aip, atp);
        if (sap->db == NULL) goto erret;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }
    if (atp == SEQ_ANNOT_name)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        sap->name = (CharPtr)av.ptrvalue;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }
    if (atp == SEQ_ANNOT_desc)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        sap->desc = (CharPtr)av.ptrvalue;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* CHOICE */
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;    /* type of choice */
    if (atp == SEQ_ANNOT_data_ftable)
    {
        sap->type = 1;
        sap->data = (Pointer) SeqFeatSetAsnRead(aip, atp, SEQ_ANNOT_data_ftable_E);
    }
    else if (atp == SEQ_ANNOT_data_align)
    {
        sap->type = 2;
        sap->data = (Pointer) SeqAlignSetAsnRead(aip, atp, SEQ_ANNOT_data_align_E);
    }
    else if (atp == SEQ_ANNOT_data_graph)
    {
        sap->type = 3;
        sap->data = (Pointer) SeqGraphSetAsnRead(aip, atp, SEQ_ANNOT_data_graph_E);
    }
	if (sap->data == NULL) goto erret;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;   /* end struct */
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return sap;
erret:
    sap = SeqAnnotFree(sap);
    goto ret;
}

/*****************************************************************************
*
*   SeqAnnotSetAsnWrite(sap, aip, set, element)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SeqAnnot ::=)
*
*****************************************************************************/
Boolean LIBCALL SeqAnnotSetAsnWrite (SeqAnnotPtr sap, AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element)
{
	AsnTypePtr atp;
	SeqAnnotPtr oldsap;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(element, SEQ_ANNOT);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (sap == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

	oldsap = sap;
    if (! AsnOpenStruct(aip, set, (Pointer)oldsap)) goto erret;

    while (sap != NULL)
    {
        if (! SeqAnnotAsnWrite(sap, aip, atp)) goto erret;
        sap = sap->next;
    }
    if (! AsnCloseStruct(aip, set, (Pointer)oldsap)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(element);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqAnnotSetAsnRead(aip, set, element)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
SeqAnnotPtr LIBCALL SeqAnnotSetAsnRead (AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element)
{
	DataVal av;
	AsnTypePtr atp;
    SeqAnnotPtr sap, first=NULL, curr;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return first;
	}

	if (aip == NULL)
		return first;

    first = NULL;
    curr = NULL;
	AsnLinkType(element, SEQ_ANNOT);    /* link in local tree */

    if (AsnReadVal(aip, set, &av) <= 0) goto erret;   /* read the start struct */
    
    atp = set;

    while ((atp = AsnReadId(aip, amp, atp)) != set)
    {
        if (atp == NULL) goto erret;
        sap = SeqAnnotAsnRead(aip, atp);
        if (sap == NULL) goto erret;
        if (first == NULL)
            first = sap;
        else
            curr->next = sap;
        curr = sap;
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end SET OF */
	if (first == NULL)
		ErrPost(CTX_NCBIOBJ, 1, "Empty SET OF Seq-annot.  line %ld", aip->linenumber);
ret:
	AsnUnlinkType(element);       /* unlink local tree */
	return first;
erret:
    while (first != NULL)
    {
        curr = first;
        first = curr->next;
        SeqAnnotFree(curr);
    }
    goto ret;
}

/*****************************************************************************
*
*   BioseqLoad()
*       sets up a program to read/write and manipulate Bioseqs
*
*****************************************************************************/
Boolean LIBCALL BioseqLoad (void)
{
    if (! SeqAsnLoad())
        return FALSE;

    if (SeqCodeSetLoad() == NULL)
    {
        ErrPost(CTX_NCBIOBJ, 1,  "SeqCodeSetLoad failed");
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************
*
*   SeqHistNew()
*
*****************************************************************************/
SeqHistPtr LIBCALL SeqHistNew (void)
{
    SeqHistPtr shp;
    shp = (SeqHistPtr)MemNew(sizeof(SeqHist));
    return shp;
}

/*****************************************************************************
*
*   SeqHistFree(shp)
*       Frees a SeqHist and associated data
*
*****************************************************************************/
SeqHistPtr LIBCALL SeqHistFree (SeqHistPtr shp)
{
	SeqAlignPtr sap, sapnext;

    if (shp == NULL)
        return shp;

	for (sap = shp->assembly; sap != NULL; sap = sapnext)
	{
		sapnext = sap->next;
		SeqAlignFree(sap);
	}
	DateFree(shp->replace_date);
	SeqIdSetFree(shp->replace_ids);
	DateFree(shp->replaced_by_date);
	SeqIdSetFree(shp->replaced_by_ids);
	DateFree(shp->deleted_date);
		
	return (SeqHistPtr)MemFree(shp);
}

/*****************************************************************************
*
*   SeqHistAsnWrite(shp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SeqHist ::=)
*
*****************************************************************************/
Boolean LIBCALL SeqHistAsnWrite (SeqHistPtr shp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal dv;
	AsnTypePtr atp;
    Boolean retval = FALSE;
	SeqAlignPtr sap;
	Int2 choice;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, SEQ_HIST);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (shp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)shp)) goto erret;
    if (shp->assembly != NULL)
    {
       if (! AsnOpenStruct(aip, SEQ_HIST_assembly, (Pointer)shp->assembly)) goto erret;
       sap = shp->assembly;
       while (sap != NULL)
       {
           if (! SeqAlignAsnWrite(sap, aip, SEQ_HIST_assembly_E)) goto erret;
           sap = sap->next;
       }
       if (! AsnCloseStruct(aip, SEQ_HIST_assembly, (Pointer)shp->assembly)) goto erret;
    }

    if ((shp->replace_date != NULL) || (shp->replace_ids != NULL))
    {
       if (! AsnOpenStruct(aip, SEQ_HIST_replaces, (Pointer)shp)) goto erret;
		if (shp->replace_date != NULL)
		{
			if (! DateAsnWrite(shp->replace_date, aip, SEQ_HIST_REC_date))
				goto erret;
		}
		if (shp->replace_ids != NULL)
		{
			if (! SeqIdSetAsnWrite(shp->replace_ids, aip, SEQ_HIST_REC_ids, SEQ_HIST_REC_ids_E))
				goto erret;
		}
       if (! AsnCloseStruct(aip, SEQ_HIST_replaces, (Pointer)shp)) goto erret;
    }

    if ((shp->replaced_by_date != NULL) || (shp->replaced_by_ids != NULL))
    {
       if (! AsnOpenStruct(aip, SEQ_HIST_replaced_by, (Pointer)shp)) goto erret;
		if (shp->replaced_by_date != NULL)
		{
			if (! DateAsnWrite(shp->replaced_by_date, aip, SEQ_HIST_REC_date))
				goto erret;
		}
		if (shp->replaced_by_ids != NULL)
		{
			if (! SeqIdSetAsnWrite(shp->replaced_by_ids, aip, SEQ_HIST_REC_ids, SEQ_HIST_REC_ids_E))
				goto erret;
		}
       if (! AsnCloseStruct(aip, SEQ_HIST_replaced_by, (Pointer)shp)) goto erret;
    }

	if ((shp->deleted) || (shp->deleted_date != NULL))
	{
		dv.ptrvalue = (Pointer)shp;
		if (shp->deleted)
			choice = 0;
		else
			choice = 1;
        if (! AsnWriteChoice(aip, SEQ_HIST_deleted, choice, &dv)) goto erret;   /* CHOICE */
		if (shp->deleted)
		{
			dv.boolvalue = TRUE;
			if (! AsnWrite(aip, SEQ_HIST_deleted_bool, &dv)) goto erret;
		}
		else
		{
			if (! DateAsnWrite(shp->deleted_date, aip, SEQ_HIST_deleted_date))
				goto erret;
		}
	}
    if (! AsnCloseStruct(aip, atp, (Pointer)shp)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqHistAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
SeqHistPtr LIBCALL SeqHistAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal dv;
	AsnTypePtr atp, oldatp;
    SeqHistPtr shp=NULL;
	SeqAlignPtr sap, saplast = NULL;
	Int2 in_replace = -1;
	DatePtr date;
	SeqIdPtr sip;

	if (! loaded)
	{
		if (! SeqAsnLoad())
			return shp;
	}

	if (aip == NULL)
		return shp;

	if (orig == NULL)           /* SeqHist ::= (self contained) */
		atp = AsnReadId(aip, amp, SEQ_HIST);
	else
		atp = AsnLinkType(orig, SEQ_HIST);    /* link in local tree */
    oldatp = atp;
    if (atp == NULL) return shp;

    shp = SeqHistNew();
    if (shp == NULL) goto erret;

    if (AsnReadVal(aip, atp, &dv) <= 0) goto erret;   /* read the start struct */
    
    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL) goto erret;
		if (atp == SEQ_HIST_assembly_E)
		{
			sap = SeqAlignAsnRead(aip, atp);
			if (sap == NULL) goto erret;
			if (saplast == NULL)
				shp->assembly = sap;
			else
				saplast->next = sap;
			saplast = sap;
		}
		else if (atp == SEQ_HIST_REC_date)
		{
			date = DateAsnRead(aip, atp);
			if (date == NULL) goto erret;
			if (in_replace == 1)
				shp->replace_date = date;
			else
				shp->replaced_by_date = date;
		}
		else if (atp == SEQ_HIST_REC_ids)
		{
			sip = SeqIdSetAsnRead(aip, atp, SEQ_HIST_REC_ids_E);
			if (sip == NULL) goto erret;
			if (in_replace == 1)
				shp->replace_ids = sip;
			else
				shp->replaced_by_ids = sip;
		}
		else if (atp == SEQ_HIST_deleted_date)
		{
			date = DateAsnRead(aip, atp);
			if (date == NULL) goto erret;
			shp->deleted_date = date;
		}
        else
        {
            if (AsnReadVal(aip, atp, &dv) <= 0) goto erret;
            if (atp == SEQ_HIST_replaces)
                in_replace *= -1;
			else if (atp == SEQ_HIST_deleted_bool)
				shp->deleted = dv.boolvalue;
        }
    }
    if (AsnReadVal(aip, atp, &dv) <= 0) goto erret;    /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return shp;
erret:
    shp = SeqHistFree(shp);
    goto ret;
}

