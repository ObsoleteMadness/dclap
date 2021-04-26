/*  sequtil.c
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
* File Name:  sequtil.c
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.55 $
*
* File Description:  Sequence Utilities for objseq and objsset
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/

#include <sequtil.h>

static NumberingPtr stdnum = NULL;  /* std Numbering object (start at 1) */

/*****************************************************************************
*
*   BioseqFind(SeqIdPtr)
*   	Just checks in object loaded memory
*
*****************************************************************************/
BioseqPtr BioseqFind (SeqIdPtr sid)

{
    Int2 i, Bnum;
	BioseqPtr PNTR Blist;

	if (sid == NULL)
		return NULL;

	Blist = BioseqInMem(&Bnum);
    for (i = 0; i < Bnum; i++)
    {
        if (BioseqMatch(Blist[i], sid))
            return Blist[i];
    }
    return NULL;
}


/*****************************************************************************
*
*   Boolean BioseqMatch(bsp, seqid)
*       returns TRUE if bsp points to the Bioseq identified by seqid
*
*****************************************************************************/
Boolean BioseqMatch (BioseqPtr bsp, SeqIdPtr seqid)

{
    SeqIdPtr sip;

    if ((bsp == NULL) || (seqid == NULL))
        return FALSE;

	for (sip = bsp->id; sip != NULL; sip = sip->next)
    {
        if (SeqIdMatch(seqid, sip))
            return TRUE;
    }
    return FALSE;
}

typedef struct findse {
	SeqIdPtr sip;
	Boolean found;
} fse, PNTR fseptr;

/*****************************************************************************
*
*   FindSE()
*      SeqEntryExplore function used by SeqEntryFind()
*
*****************************************************************************/
void FindSE PROTO((SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent));
void FindSE (SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent)
{
	fseptr fep;
	BioseqPtr bsp;

	fep = (fseptr)data;
	if (fep->found)   /* already found it */
		return;

    if (! IS_Bioseq(sep))
        return;

	bsp = (BioseqPtr)(sep->data.ptrvalue);
	if (BioseqMatch(bsp, fep->sip))
		fep->found = TRUE;

	return;
}

/*****************************************************************************
*
*   SeqEntryFind(sip)
*
*****************************************************************************/
SeqEntryPtr SeqEntryFind (SeqIdPtr sip)
{
	fse fe;
	Int2 i, SEnum;
	SeqEntryPtr PNTR SElist;

	if (sip == NULL)
		return NULL;

	fe.found = FALSE;
	fe.sip = sip;

	SElist = SeqEntryInMem(&SEnum);    /* get all active SeqEntries */
	if (! SEnum)
		return NULL;

	for (i = 0; i < SEnum; i++)
	{
		SeqEntryExplore(SElist[i], (Pointer)(&fe), FindSE);
		if (fe.found)
			return SElist[i];
	}
	return (SeqEntryPtr)NULL;
}

/*****************************************************************************
*
*   FindContext()
*      SeqEntryExplore function used by BioseqContextNew()
*
*****************************************************************************/
void FindContext PROTO((SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent));
void FindContext (SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent)
{
	BioseqContextPtr bcp;

	bcp = (BioseqContextPtr)data;
	if (bcp->hit)   /* already found it */
		return;

	if (indent >= BIOSEQCONTEXTMAX)
	{
		ErrPost(CTX_NCBIOBJ, 1, "Exceeded BIOSEQCONTEXTMAX");
		return;
	}

	bcp->count = indent + 1;	      /* store context */
	bcp->context[indent] = sep;
    if (! IS_Bioseq(sep))
        return;

	if (bcp->bsp == (BioseqPtr)sep->data.ptrvalue)   /* found it */
		bcp->hit = TRUE;

	return;
}

/*****************************************************************************
*
*   BioseqContextPtr BioseqContextNew (bsp)
*
*****************************************************************************/
BioseqContextPtr BioseqContextNew (BioseqPtr bsp)
{
	Int2 i, SEnum;
	SeqEntryPtr PNTR SElist;
	BioseqContextPtr bcp;

	if (bsp == NULL)
		return NULL;

	bcp = MemNew(sizeof(BioseqContext));
	bcp->bsp = bsp;
	bcp->se.choice = 1;   /* bioseq */
	bcp->se.data.ptrvalue = bsp;

	SElist = SeqEntryInMem(&SEnum);    /* get all active SeqEntries */
	if (! SEnum)
		return bcp;

	for (i = 0; i < SEnum; i++)
	{
		SeqEntryExplore(SElist[i], (Pointer)bcp, FindContext);
		if (bcp->hit)
			return bcp;
		bcp->count = 0;
	}
	return bcp;
}

/*****************************************************************************
*
*   BioseqContextFree(bcp)
*
*****************************************************************************/
BioseqContextPtr BioseqContextFree(BioseqContextPtr bcp)
{
	return MemFree(bcp);
}

/*****************************************************************************
*
*   BioseqContextGetSeqDescr(bcp, type, curr, SeqEntryPtr PNTR sep)
*       returns pointer to the next SeqDescr of this type
*       type gives type of Seq-descr
*       if (type == 0)
*          get them all
*       curr is NULL or previous node of this type found
*       moves up from bsp
*		if (sep != NULL) sep set to SeqEntryPtr containing the SeqDescr.
*
*****************************************************************************/
ValNodePtr BioseqContextGetSeqDescr (BioseqContextPtr bcp, Int2 type, ValNodePtr curr, SeqEntryPtr PNTR the_sep)    /* the last one you used */
{
	Int2 i;
	ValNodePtr tmp;
	Boolean found = FALSE;
	BioseqPtr bsp;
	BioseqSetPtr bssp;

	if (bcp == NULL) return NULL;
	
	if (the_sep != NULL)
		*the_sep = NULL;
		
	if (bcp->count == 0)   /* just a Bioseq */
	{
		tmp = BioseqGetSeqDescr(bcp->bsp, type, curr);
		if (the_sep != NULL) *the_sep = bcp->context[1];
		return tmp;
	}

	i = bcp->count - 1;
	if (curr != NULL)   /* find where we are */
	{
		while ((i >= 0) && (! found))
		{
			if (IS_Bioseq(bcp->context[i]))
			{
				bsp = (BioseqPtr)((bcp->context[i])->data.ptrvalue);
				tmp = bsp->descr;
			}
			else
			{
				bssp = (BioseqSetPtr)((bcp->context[i])->data.ptrvalue);
				tmp = bssp->descr;
			}
			while ((tmp != curr) && (tmp != NULL))
				tmp = tmp->next;
			if (tmp == curr)
			{
				found = TRUE;
				tmp = tmp->next;
			}
			else
				i--;
		}
		if (! found)   /* can't find it! */
			return NULL;
	}
	else              /* get first one */
	{
		tmp = bcp->bsp->descr;
	}
		
	while (i >= 0)
	{
		while (tmp != NULL)
		{
			if ((! type) || ((Int2)tmp->choice == type))
			{
				if (the_sep != NULL) *the_sep = bcp->context[i];
				return tmp;
			}
			tmp = tmp->next;
		}
		i--;
		if (i >= 0)
		{
			if (IS_Bioseq(bcp->context[i]))
			{
				bsp = (BioseqPtr)((bcp->context[i])->data.ptrvalue);
				tmp = bsp->descr;
			}
			else
			{
				bssp = (BioseqSetPtr)((bcp->context[i])->data.ptrvalue);
				tmp = bssp->descr;
			}
		}
	}
    return NULL;
}

/*****************************************************************************
*
*   BioseqContextGetSeqFeat(bcp, type, curr, sapp)
*       returns pointer to the next Seq-feat of this type
*       type gives type of Seq-descr
*       if (type == 0)
*          get them all
*       curr is NULL or previous node of this type found
*       moves up from bsp
*   	if (sapp != NULL) is filled with SeqAnnotPtr containing the SeqFeat
*   	in:
*   		0 = sfp->location only
*   		1 = sfp->product only
*   		2 = either of above
*
*****************************************************************************/
SeqFeatPtr BioseqContextGetSeqFeat (BioseqContextPtr bcp, Int2 type,
	SeqFeatPtr curr, SeqAnnotPtr PNTR sapp, Int2 in)    /* the last one you used */
{
	SeqEntryPtr sep;

	if (bcp == NULL) return NULL;
	
	if (sapp != NULL)
		*sapp = NULL;

	if (bcp->count == 0)    /* just a BioseqSeq */
		sep = &(bcp->se);
	else
		sep = bcp->context[0];

	return SeqEntryGetSeqFeat (sep, type, curr, sapp, in, bcp->bsp);
}

typedef struct getseqfeat {
	Boolean hit;
	SeqFeatPtr last,
		this;
	SeqAnnotPtr sap;
	SeqLocPtr slp1, slp2;
	Int2 in, type;
} GetSeqFeat, PNTR GetSeqFeatPtr;

void GetSeqFeatCallback (SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent);

/*****************************************************************************
*
*   SeqEntryGetSeqFeat(sep, type, curr, sapp)
*       returns pointer to the next Seq-feat of this type
*       type gives type of SeqFeat
*       if (type == 0)
*          get them all
*       curr is NULL or previous node of this type found
*       moves up from bsp
*   	if (sapp != NULL) is filled with SeqAnnotPtr containing the SeqFeat
*       if (bsp != NULL) then for that Bioseq match on location by "in"
*   	in:
*   		0 = sfp->location only
*   		1 = sfp->product only
*   		2 = either of above
*
*****************************************************************************/
SeqFeatPtr SeqEntryGetSeqFeat (SeqEntryPtr sep, Int2 type,
	SeqFeatPtr curr, SeqAnnotPtr PNTR sapp, Int2 in, BioseqPtr bsp)    /* the last one you used */
{
	GetSeqFeat gsf;
	ValNode vn1, vn2;
	
	if (sep == NULL) return NULL;
	
	if (sapp != NULL)
		*sapp = NULL;

	if (curr == NULL)
		gsf.hit = TRUE;
	else
		gsf.hit = FALSE;
	gsf.last = curr;
	gsf.this = NULL;
	gsf.sap = NULL;
	gsf.type = type;
	gsf.in = in;
	if (bsp != NULL)   /* matching by Bioseq */
	{
		if ((bsp->repr == Seq_repr_seg) || (bsp->repr == Seq_repr_ref))
		{
			vn2.choice = SEQLOC_MIX;
			vn2.data.ptrvalue = bsp->seq_ext;
			gsf.slp2 = (SeqLocPtr)(&vn2);
		}
		else
			gsf.slp2 = NULL;

		vn1.choice = SEQLOC_WHOLE;
		vn1.data.ptrvalue = (Pointer) bsp->id;
		gsf.slp1 = (SeqLocPtr)(&vn1);
	}
	else
		gsf.slp1 = NULL;

	SeqEntryExplore (sep, (Pointer)(&gsf), GetSeqFeatCallback);

	if (sapp != NULL)
		*sapp = gsf.sap;

	return gsf.this;
}

void GetSeqFeatCallback (SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent)
{
	GetSeqFeatPtr gsfp;
	BioseqPtr bsp;
	BioseqSetPtr bssp;
	SeqAnnotPtr sap;
	SeqFeatPtr sfp, last;
	Boolean hit, gotit = FALSE;
	Uint1 type;
	SeqLocPtr slp1, slp2, slp;
	Int2 i, in, retval;

	gsfp = (GetSeqFeatPtr)data;
	if (gsfp->this != NULL)   /* got it */
		return;

	last = gsfp->last;
	hit = gsfp->hit;
	type = (Uint1)(gsfp->type);
	if (gsfp->slp1 != NULL)   /* matching by Bioseq */
	{
		slp1 = gsfp->slp1;
		slp2 = gsfp->slp2;
		in = gsfp->in;
	}
	else
		slp1 = NULL;

	if (IS_Bioseq(sep))
	{
		bsp = (BioseqPtr)(sep->data.ptrvalue);
		sap = bsp->annot;
	}
	else
	{
		bssp = (BioseqSetPtr)(sep->data.ptrvalue);
		sap = bssp->annot;
	}

	while (sap != NULL)
	{
		if (sap->type == 1)  /* feature table */
		{
			for (sfp = (SeqFeatPtr)(sap->data); sfp != NULL; sfp = sfp->next)
			{
				if (! hit)	   /* still looking */
				{
					if (sfp == last)
					{
						hit = TRUE;
						gsfp->hit = TRUE;
					}
				}
				else
				{
					if ((! type) || (type == sfp->data.choice))
					{
						if (slp1 != NULL)   /* look for feats on a bioseq */
						{
							for (i = 0; i < 2; i++)
							{
								if ((i == 0) && (in != 1))
									slp = sfp->location;
								else if ((i==1) && (in != 0))
									slp = sfp->product;
								else
									slp = NULL;
								if (slp != NULL)
								{
									retval = SeqLocCompare(slp, slp1);
									if (retval)
									{
										gotit = TRUE;
										break;
									}

									if (slp2 != NULL)
									{
										retval = SeqLocCompare(slp, slp2);
										if (retval)
										{
											gotit = TRUE;
											break;
										}
									}
								}
							}
						}
						else
							gotit = TRUE;
						if (gotit)
						{
							gsfp->this = sfp;
							gsfp->sap = sap;
							return;
						}
					}
				}
			}
		}

		sap = sap->next;
	}

	return;
}

/*****************************************************************************
*
*   BioseqContextGetTitle(bcp)
*     returns first title for Bioseq in this context
*
*****************************************************************************/
CharPtr BioseqContextGetTitle(BioseqContextPtr bcp)
{
	CharPtr title = NULL;
	ValNodePtr vnp;

	vnp = BioseqContextGetSeqDescr(bcp, Seq_descr_title, NULL, NULL);
	if (vnp != NULL)
		title = (CharPtr)vnp->data.ptrvalue;
	return title;
}

/*****************************************************************************
*
*   BioseqGetSeqDescr(bsp, type, curr)
*       returns pointer to the next SeqDescr of this type
*       type gives type of Seq-descr
*       if 0, gets them all
*       curr is NULL or previous node of this type found
*
*****************************************************************************/
ValNodePtr BioseqGetSeqDescr (BioseqPtr bsp, Int2 type, ValNodePtr curr)    /* the last one you used */

{
	if (bsp == NULL) return NULL;
	
    if (curr == NULL)
            curr = bsp->descr;
    else
        curr = curr->next;     /* move past last one */

    while (curr != NULL)
    {
        if ((! type) || ((Int2)curr->choice == type))
            return curr;
        else
            curr = curr->next;
    }
    return NULL;
}

/*****************************************************************************
*
*   BioseqGetTitle(bsp)
*       returns pointer to the first title of this Bioseq
*
*****************************************************************************/
CharPtr BioseqGetTitle (BioseqPtr bsp)

{
    ValNodePtr ptr;

    ptr = BioseqGetSeqDescr(bsp, Seq_descr_title, NULL);
    if (ptr != NULL)
        return (CharPtr)ptr->data.ptrvalue;
    else
        return NULL;
}

/*****************************************************************************
*
*   BioseqGetNumbering(bsp)
*   	Gets either user supplied, or default number for a Bioseq
*   	looks first for num Seqdescr, then in Pubdesc, then returns
*         default numbering
*
*****************************************************************************/
NumberingPtr BioseqGetNumbering (BioseqPtr bsp)

{
	NumberingPtr np = NULL;
	ValNodePtr anp;
	PubdescPtr pdp;

	if (bsp == NULL)
		return NULL;

	anp = BioseqGetSeqDescr(bsp, Seq_descr_num, NULL);
	if (anp != NULL)    /* Numbering on this Bioseq */
		np = (NumberingPtr)anp->data.ptrvalue;
	else do				/* look for Pubdesc  */
	{
		anp = BioseqGetSeqDescr(bsp, Seq_descr_pub, anp);
		if (anp != NULL)
		{
			pdp = (PubdescPtr)anp->data.ptrvalue;
			np = pdp->num;
		}
	} while ((anp != NULL) && (np == NULL));

	if (np == NULL)   /* no numbering found */
		np = NumberingDefaultGet();   /* fallback position */

	return np;
}


/*****************************************************************************
*
*   Bioseq_repr (BioseqPtr bsp)
*
*****************************************************************************/
Uint1 Bioseq_repr (BioseqPtr bsp)

{
    return bsp->repr;
}

/*****************************************************************************
*
*   Int4 BioseqGetLen (bsp)
*       returns total length of sequence in residues
*       if segmented:
*          includes length of virtual sequences with fixed length
*          does not include lengths of NULL gaps
*       returns -1 for error
*
*****************************************************************************/
Int4 BioseqGetLen (BioseqPtr bsp)

{
    if (bsp == NULL)
        return -1;

    return bsp->length;
}

/*****************************************************************************
*
*   Int4 BioseqGetGaps (bsp)
*       returns total number of NULL gaps in sequence
*       virtual sequence with length set does not count as a gap
*       returns -1 for error
*
*****************************************************************************/
Int4 BioseqGetGaps (BioseqPtr bsp)

{
    SeqLocPtr anp;
    Int4 gaps = 0;

    if (bsp == NULL)
        return -1;

    if (Bioseq_repr(bsp) != Seq_repr_seg)  /* not segmented */
        return gaps;   /* no gaps */

    anp = (SeqLocPtr)bsp->seq_ext;
    while (anp != NULL)    /* go through Seq-loc chain */
    {
		gaps = SeqLocGetSegLens(anp, NULL, gaps, TRUE);
        anp = anp->next;
    }
    return gaps;
}

/*****************************************************************************
*
*   Int4 BioseqGetSegLens (bsp, lens)
*       returns total number of segments in sequence including NULLS
*       returns -1 for error
*       if lens != NULL fills with lengths of segments, 0 = NULL
*
*****************************************************************************/
Int4 BioseqGetSegLens (BioseqPtr bsp, Int4Ptr lens)

{
    SeqLocPtr anp;
    Int4 segs = 0;

    if (bsp == NULL)
        return -1;

    if (Bioseq_repr(bsp) != Seq_repr_seg)  /* not segmented */
    {
        if (lens != NULL)
            lens[0] = BioseqGetLen(bsp);
        return 1;   /* one segment */
    }

    anp = (SeqLocPtr)bsp->seq_ext;
    while (anp != NULL)    /* go through Seq-loc chain */
    {
		segs = SeqLocGetSegLens(anp, lens, segs, FALSE);
        anp = anp->next;
    }
    return segs;
}

/*****************************************************************************
*
*   BioseqGetCode(bsp)
*       returns type of code for data in sequence
*       if not bioseq or not raw returns 0
*       otherwise returns #defines from objseq.h
*
*****************************************************************************/
Uint1 BioseqGetCode (BioseqPtr bsp)

{
    if (bsp == NULL)
        return 0;

    if ((Bioseq_repr(bsp) == Seq_repr_raw) ||
		(Bioseq_repr(bsp) == Seq_repr_const))
	    return bsp->seq_data_type;
	else
		return 0;
}

/*****************************************************************************
*
*   Boolean BioseqRawConvert(bsp, newcode)
*      converts a raw or const bioseq to a new sequence code
*
*****************************************************************************/
Boolean BioseqRawConvert (BioseqPtr bsp, Uint1 newcode)

{
    ByteStorePtr to;
    Int4 seqlen;
    Uint1 oldcode;

	if (bsp == NULL) return FALSE;
	
    if (! ((Bioseq_repr(bsp) == Seq_repr_raw) ||
		(Bioseq_repr(bsp) == Seq_repr_const)))
        return FALSE;

    oldcode = BioseqGetCode(bsp);
    if (! oldcode)   /* not a coded sequence */
        return FALSE;

    seqlen = BioseqGetLen(bsp);

    to = BSConvertSeq(bsp->seq_data, newcode, oldcode, seqlen);
    if (to == NULL)
        return FALSE;

    bsp->seq_data = to;
    bsp->seq_data_type = newcode;

    return TRUE;
}

/*****************************************************************************
*
*   Boolean BioseqRawPack(bsp)
*      converts a raw or const bioseq to it's densist possible code
*
*****************************************************************************/
Boolean BioseqRawPack (BioseqPtr bsp)

{
    ByteStorePtr to, from;
    Int4 seqlen;
    Uint1 oldcode, newcode;
    Uint1 byte_from, residue_from, bitctr_from, mask_from, lshift_from,
          rshift_from, bc_from;
	Uint1 ok[4];
	Int2 i;
	Boolean found;

	if (bsp == NULL) return FALSE;
	
    if (! ((Bioseq_repr(bsp) == Seq_repr_raw) ||
		(Bioseq_repr(bsp) == Seq_repr_const)))
        return FALSE;

	if (! ISA_na(bsp->mol))    /* can only pack nucleic acids */
		return FALSE;

    oldcode = BioseqGetCode(bsp);
    if (! oldcode)  /* not a coded sequence */
        return FALSE;

	if (oldcode == Seq_code_ncbi2na)   /* already packed */
		return TRUE;

    seqlen = BioseqGetLen(bsp);
	from = bsp->seq_data;

	BSSeek(from, 0L, SEEK_SET);
	newcode = Seq_code_ncbi2na;    /* go for broke */

	switch (oldcode)
    {
	    case Seq_code_ncbi4na:
            bc_from = 2;
    	    rshift_from = 4;
        	lshift_from = 4;
	        mask_from = 240;
			ok[0] = 1;
			ok[1] = 2;
			ok[2] = 4;
			ok[3] = 8;
            break;
    	case Seq_code_iupacna:
        	bc_from = 1;
	        rshift_from = 0;
            lshift_from = 0;
    	    mask_from = 255;
			ok[0] = 'A';
			ok[1] = 'C';
			ok[2] = 'G';
			ok[3] = 'T';
        	break;
		default:
			return FALSE;
	}

    bitctr_from = 0;

	while (seqlen)
    {
    	if (! bitctr_from)        /* need a new byte */
	    {
            byte_from = (Uint1) BSGetByte(from);
    	    bitctr_from = bc_from;
	    }	
        residue_from = byte_from & mask_from;
    	residue_from >>= rshift_from;
	    byte_from <<= lshift_from;
        bitctr_from--;
	    seqlen--;
		found = FALSE;
		for (i = 0; i < 4; i++)
		{
			if (ok[i] == residue_from)
			{
				found = TRUE;
				break;
			}
		}
		if (! found)
		{
			newcode = Seq_code_ncbi4na;
			break;
		}
	}

    seqlen = BioseqGetLen(bsp);

    to = BSConvertSeq(bsp->seq_data, newcode, oldcode, seqlen);
    if (to == NULL)
        return FALSE;

    bsp->seq_data = to;
    bsp->seq_data_type = newcode;

    return TRUE;
}

/*****************************************************************************
*
*   BSConvertSeq(bytestoreptr, newcode, oldcode, len)
*       converts a bytestore to a new sequence representation
*       frees old bytestore
*       returns pointer to new one, or NULL on fail.
*       len is residues
*
*****************************************************************************/
ByteStorePtr BSConvertSeq (ByteStorePtr from, Uint1 newcode, Uint1 oldcode, Int4 len)

{
    ByteStorePtr to;
    Int4 storelen;
    Uint1 byte_from, residue_from, bitctr_from, mask_from, lshift_from,
          rshift_from, bc_from ,
          byte_to, bitctr_to, lshift_to[5], bc_to, byte_tmp,
		  tbuf[100];
    SeqMapTablePtr smtp;
	Int2 tlen;

    if ((from == NULL) || (! oldcode) || (! newcode) || (len <= 0))
        return NULL;

    if (oldcode == newcode)
        return from;

    if ((smtp = SeqMapTableFind(newcode, oldcode)) == NULL)
        return NULL;

    if (newcode == Seq_code_ncbi2na)
        storelen = (len / 4) + 1;
    else if (newcode == Seq_code_ncbi4na)
        storelen = (len / 2) + 1;
    else
        storelen = len;

    to = BSNew((Uint4)storelen);
    if (to == NULL)
        return NULL;

    BSSeek(from, 0, 0);
    BSSeek(to, 0, 0);
	tlen = 100;
    
    switch (oldcode)
    {
        case Seq_code_ncbi2na:
            bc_from = 4;            /* bit shifts needed */
            rshift_from = 6;
            lshift_from = 2;
            mask_from = 192;
            break;
        case Seq_code_ncbi4na:
            bc_from = 2;
            rshift_from = 4;
            lshift_from = 4;
            mask_from = 240;
            break;
        default:
            bc_from = 1;
            rshift_from = 0;
            lshift_from = 0;
            mask_from = 255;
            break;
    }

    lshift_to[1] = 0;
    switch (newcode)
    {
        case Seq_code_ncbi2na:
            bc_to = 4;            /* bit shifts needed */
            lshift_to[2] = 2;
            lshift_to[3] = 4;
            lshift_to[4] = 6;
            break;
        case Seq_code_ncbi4na:
            bc_to = 2;
            lshift_to[2] = 4;
            break;
        default:
            bc_to = 1;
            break;
    }

    bitctr_to = bc_to;
    byte_to = 0;
    bitctr_from = 0;

    while (len)
    {
		if (tlen == 100)
		{
			tlen = (Int2) BSRead(from, (VoidPtr)tbuf, (Int4)100);
			tlen = 0;
		}

        if (! bitctr_from)        /* need a new byte */
        {
            byte_from = tbuf[tlen];
			tlen++;
            bitctr_from = bc_from;
        }
        residue_from = byte_from & mask_from;
        residue_from >>= rshift_from;
        byte_from <<= lshift_from;
        bitctr_from--;

        byte_tmp = SeqMapTableConvert(smtp, residue_from);
		if (byte_tmp == INVALID_RESIDUE)
		{
			ErrPost(CTX_NCBIOBJ, 1, "BSConvertSeq: invalid residue [%d=%c]",
				(int)residue_from, (char)residue_from);
			BSFree(to);
			return NULL;
		}
        byte_tmp <<= lshift_to[bitctr_to];
		byte_to |= byte_tmp;
        bitctr_to--;
        if (! bitctr_to)
        {
            BSPutByte(to, byte_to);
            bitctr_to = bc_to;
            byte_to = 0;
        }
    
        len--;
    }

    if (bitctr_to != bc_to)      /* partial byte not written */
        BSPutByte(to, byte_to);

    BSFree(from);
    return to;
}


/*****************************************************************************
*
*   Int4 NumberingOffset(np, value)
*       returns an offset to the sequence based on value
*       returns -1 if invalid
*       does NOT deal with Num-ref types
*       does NOT deal with specified ranges on the sequence
*
*****************************************************************************/
Int4 NumberingOffset (NumberingPtr np, DataValPtr vp)

{
    Int4 offset = -1, i, num;
    NumContPtr ncp;
    NumEnumPtr nep;
    NumRealPtr nrp;
    CharPtr PNTR ptr;
    CharPtr name;
    FloatHi foffset;

	if ((np == NULL) || (vp == NULL)) return -1;
	
    switch (np->choice)
    {
        case Numbering_cont:
            ncp = (NumContPtr)np->data.ptrvalue;
            if (ncp->ascending)
            {
                offset = vp->intvalue - ncp->refnum;
                if ((ncp->refnum  < 0) && (! ncp->has_zero) &&
                    (vp->intvalue > 0))
                    offset--;
            }
            else
            {
                offset = ncp->refnum - vp->intvalue;
                if ((ncp->refnum > 0) && (! ncp->has_zero) &&
                    (vp->intvalue < 0))
                    offset--;
            }
            break;
        case Numbering_enum:
            nep = (NumEnumPtr)np->data.ptrvalue;
            name = (CharPtr)vp->ptrvalue;
            num = nep->num;
            ptr = nep->names;
            for (i = 0; i < num; i++, ptr++)
            {
                if (! StringCmp(name, *ptr))
                {
                    offset = i;
                    break;
                }
            }
            break;
        case Numbering_ref_source:
		case Numbering_ref_align:
            ErrPost(CTX_NCBIOBJ, 1, "Num-ref not supported yet");
            break;
        case Numbering_real:
            nrp = (NumRealPtr)np->data.ptrvalue;
            foffset = (vp->realvalue - nrp->b) / nrp->a;
            offset = (Int4) foffset;
            if ((foffset - (FloatHi)offset) >= 0.5)
                offset++;
            break;
    }
    return offset;
}

/*****************************************************************************
*
*   NumberingValue (np, offset, value)
*       fills value with the display value of offset
*       return type indicates type of value
*       0 = failed
*       1 = intvalue
*       2 = realvalue
*       3 = ptrvalue (string)
*
*****************************************************************************/
Int2 NumberingValue (NumberingPtr np, Int4 offset, DataValPtr vp)

{
    NumContPtr ncp;
    NumEnumPtr nep;
    NumRealPtr nrp;
    Int2 type = 0;
    Int4 intval;
    FloatHi fval;

	if ((np == NULL) || (vp == NULL)) return -1;
	
    switch (np->choice)
    {
        case Numbering_cont:
            ncp = (NumContPtr)np->data.ptrvalue;
            if (ncp->ascending)
            {
                intval = offset + ncp->refnum;
                if ((ncp->refnum  < 0) && (! ncp->has_zero) &&
                    (intval >= 0))
                    intval++;
            }
            else
            {
                intval = ncp->refnum - offset;
                if ((ncp->refnum > 0) && (! ncp->has_zero) &&
                    (intval <= 0))
                    intval--;
            }
            vp->intvalue = intval;
            type = 1;
            break;
        case Numbering_enum:
            nep = (NumEnumPtr)np->data.ptrvalue;
            if (offset < nep->num)
            {
                vp->ptrvalue = nep->names[offset];
                type = 3;
            }
            break;
        case Numbering_ref_source:
		case Numbering_ref_align:
            ErrPost(CTX_NCBIOBJ, 1, "Num-ref not supported yet");
            break;
        case Numbering_real:
            nrp = (NumRealPtr)np->data.ptrvalue;
            fval = ((FloatHi)offset * nrp->a) + nrp->b;
            type = 2;
            vp->realvalue = fval;
            break;
    }

    return type;
}

/*****************************************************************************
*
*   NumberingValueBySeqId(sip, offset, vp)
*
*****************************************************************************/
Int2 NumberingValueBySeqId (SeqIdPtr sip, Int4 offset, DataValPtr vp)

{
	BioseqPtr bsp;
	NumberingPtr np = NULL;

	if ((sip == NULL) || (vp == NULL)) return -1;
	
	bsp = BioseqFind(sip);
	if (bsp == NULL)
		np = NumberingDefaultGet();
	else
		np = BioseqGetNumbering(bsp);

	return NumberingValue(np, offset, vp);
}

/*****************************************************************************
*
*   NumberingDefaultLoad()
*
*****************************************************************************/
void NumberingDefaultLoad (void)

{
    NumContPtr ncp;

    if (stdnum != NULL)
        return;

    stdnum = ValNodeNew(NULL);   /* set up numbering from 1 */
    stdnum->choice = Numbering_cont;
    ncp = NumContNew();
    ncp->refnum = 1;   /* number from one */
    ncp->ascending = TRUE;
    stdnum->data.ptrvalue = (Pointer) ncp;
    return;
}

/*****************************************************************************
*
*   NumberingDefaultGet()
*       returns a default numbering object (start at 1, ascending, no 0)
*
*****************************************************************************/
NumberingPtr NumberingDefaultGet (void)

{
    if (stdnum == NULL)
        NumberingDefaultLoad();
    return stdnum;
}

/*****************************************************************************
*
*   SeqMapTableConvert(smtp, from)
*       returns conversion of from using SeqMapTable smtp
*
*****************************************************************************/
Uint1 SeqMapTableConvert (SeqMapTablePtr smtp, Uint1 from)

{
	Int2 index;

	if (smtp == NULL) return (Uint1)(INVALID_RESIDUE);
	
	index = (Int2)from - (Int2)(smtp->start_at);
	if ((index >= 0) && (index < (Int2)(smtp->num)))
		return (Uint1)(smtp->table[index]);
	else
		return (Uint1)(INVALID_RESIDUE);
}

/*****************************************************************************
*
*   SeqCodeTableComp(sctp, residue)
*       returns complement of residue if possible
*       or residue, if not
*       assumes residue is in the same code as sctp
*
*****************************************************************************/
Uint1 SeqCodeTableComp (SeqCodeTablePtr sctp, Uint1 residue)

{
	Int2 index;
	
    if ((sctp == NULL) || (sctp->comps == NULL))   /* no complement table */
        return INVALID_RESIDUE;

	index = (Int2)residue - (Int2)(sctp->start_at);
	if ((index < 0 ) || (index >= (Int2)(sctp->num)))
		return INVALID_RESIDUE;
	else
        return sctp->comps[index];
}

/*****************************************************************************
*
*   SeqEntryList(sep, mydata, mycallback, index, indent)
*       traverses all Seq-entry nodes beginning with sep
*       calls mycallback() at each node
*
*****************************************************************************/
Int4 SeqEntryList (SeqEntryPtr sep, Pointer mydata, SeqEntryFunc mycallback, Int4 index, Int2 indent)

{
    if (sep == NULL)
        return index;

    if (mycallback != NULL)
        (*mycallback)(sep, mydata, index, indent);
    index++;

    if (IS_Bioseq(sep))    /* bioseq, no contained sequences */
        return index;

    sep = ((BioseqSetPtr)sep->data.ptrvalue)->seq_set;
    indent++;
    while (sep != NULL)
    {
        index = SeqEntryList(sep, mydata, mycallback, index, indent);
        sep = sep->next;
    }
    return index;
}

/*****************************************************************************
*
*   BioseqList(sep, mydata, mycallback, index, indent)
*       traverses all Seq-entry nodes beginning with sep
*       calls mycallback() at each node that is a Bioseq
*       Does NOT enter BioseqSets of _class "parts"
*       Does NOT increment indent
*
*****************************************************************************/
Int4 BioseqList (SeqEntryPtr sep, Pointer mydata, SeqEntryFunc mycallback, Int4 index, Int2 indent)

{
    if (sep == NULL)
        return index;

    if (IS_Bioseq(sep))    /* bioseq, no contained sequences */
    {
        if (mycallback != NULL)
            (*mycallback)(sep, mydata, index, indent);
        return index+1;
    }

    if (Bioseq_set_class(sep) == 4)    /* parts, do not enter */
        return index;

    sep = ((BioseqSetPtr)sep->data.ptrvalue)->seq_set;
    while (sep != NULL)
    {
        index = BioseqList(sep, mydata, mycallback, index, indent);
        sep = sep->next;
    }
    return index;
}

/*****************************************************************************
*
*   SeqEntryGetSeqDescr(sep, type, curr)
*       returns pointer to the next SeqDescr of this type
*       type gives type of Seq-descr
*        if 0, gives all types
*       curr is NULL or previous node of this type found
*
*****************************************************************************/
ValNodePtr SeqEntryGetSeqDescr (SeqEntryPtr sep, Int2 type, ValNodePtr curr)    /* the last one you used */

{

	if (sep == NULL) return NULL;
	
    if (curr == NULL)
    {
        if (IS_Bioseq(sep))
            curr = ((BioseqPtr)sep->data.ptrvalue)->descr;
        else
            curr = ((BioseqSetPtr)sep->data.ptrvalue)->descr;
    }
    else
        curr = curr->next;     /* move past last one */

    while (curr != NULL)
    {
        if ((! type) || ((Int2)curr->choice == type))
            return curr;
        else
            curr = curr->next;
    }
    return NULL;
}
/*****************************************************************************
*
*   SeqEntryGetTitle(sep)
*       returns pointer to the first title of this SeqEntry
*
*****************************************************************************/
CharPtr SeqEntryGetTitle (SeqEntryPtr sep)

{
    ValNodePtr ptr;

    ptr = SeqEntryGetSeqDescr(sep, Seq_descr_title, NULL);
    if (ptr != NULL)
        return (CharPtr)ptr->data.ptrvalue;
    else
        return NULL;
}

/*****************************************************************************
*
*   Bioseq_set_class (SeqEntryPtr sep)
*       returns class of set as is enumerated in ASN.1 spec
*       returns 0 if not a Bioseq-set
*
*****************************************************************************/
Uint1 Bioseq_set_class (SeqEntryPtr sep)

{
	if (sep == NULL) return 0;
	
    if (IS_Bioseq_set(sep))
        return ((BioseqSetPtr)sep->data.ptrvalue)->_class;
    else
        return 0;
}

/*****************************************************************************
*
*   SeqEntryDoConvert(sep, newcode, index, indent)
*       converts a seqentry which is a raw bioseq to newcode
*       callback used by SeqEntryConvert()
*
*****************************************************************************/
void SeqEntryDoConvert PROTO((SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent));
void SeqEntryDoConvert (SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent)

{
    if (! IS_Bioseq(sep))
        return;

	if (((Uint1Ptr)data)[0] != 0)
	{
	    if (BioseqRawConvert((BioseqPtr)sep->data.ptrvalue, * ((Uint1Ptr)data)))
    	    ((Uint1Ptr)data)[1]++;
	}
	else
	{
		if (BioseqRawPack((BioseqPtr)sep->data.ptrvalue))
			((Uint1Ptr)data)[1]++;
	}
    return;
}

/*****************************************************************************
*
*   SeqEntryConvert(sep, newcode)
*       converts any seqentry to newcode
*   	if (newcode == 0)
*   		calls BioseqRawPack instead of BioseqRawConvert
*
*****************************************************************************/
Boolean SeqEntryConvert (SeqEntryPtr sep, Uint1 newcode)

{
    Uint1 tbuf[2];
    tbuf[0] = newcode;
    tbuf[1] = 0;

	if (sep == NULL) return FALSE;
	
    SeqEntryExplore(sep, (Pointer)tbuf, SeqEntryDoConvert);
    if (tbuf[1])
        return TRUE;    /* at least one success */
    else
        return FALSE;
}

#define NUM_ORDER 16		/* number of Seq-ids to order below */

/*****************************************************************************
*
*   SeqIdBestRank(buf, num)
*   	fill buf of length num with std ranks used by SeqIdFindBest
*   	returns full length of list (useful if num is too small)
*   	std ranks always between 50 and 100
*   	rank < 50 guarantees SeqIdSelect() chooses over std rank
*   	rank > 100 guarantees SeqIdSelect() never chooses over std rank
*   	rank = 255 guarantees SeqIdSelect() never choses
*   	if buf == NULL, just returns count of supported Seq-ids
*
*****************************************************************************/
Int2 SeqIdBestRank (Uint1Ptr buf, Int2 num)
{
	static Uint1 std_order[NUM_ORDER] = {
 	83, /* 0 = not set */
	80, /* 1 = local Object-id */
	55,  /* 2 = gibbsq */
	56,  /* 3 = gibbmt */
	70, /* 4 = giim Giimport-id */
	60, /* 5 = genbank */
	60, /* 6 = embl */
	60, /* 7 = pir */
	60, /* 8 = swissprot */
	55,  /* 9 = patent */
	80, /* 10 = other TextSeqId */
	80, /* 11 = general Dbtag */
	51,  /* 12 = gi */
	60, /* 13 = ddbj */
	60, /* 14 = prf */
	60  /* 15 = pdb */
    };

	if (buf == NULL) return NUM_ORDER;

	if (num > NUM_ORDER)
		num = NUM_ORDER;
	MemCopy(buf, std_order, (size_t)(num * sizeof(Uint1)));
	return NUM_ORDER;
}

/*****************************************************************************
*
*   SeqIdFindBest(sip)
*       Find the most reliable SeqId in a chain
*
*****************************************************************************/
SeqIdPtr SeqIdFindBest (SeqIdPtr sip, Uint1 target)
{
	Uint1 order[NUM_ORDER];

	if (sip == NULL)
		return NULL;

	SeqIdBestRank(order, NUM_ORDER);
	if ((target > 0) && (target < NUM_ORDER))
		order[target] = 0;    /* select target */
	else if (target >= NUM_ORDER)
		ErrPost(CTX_NCBIOBJ, 1, "SeqIdFindBest: target [%d] out of range [%d]",
			(int)target, (int)NUM_ORDER);

	return SeqIdSelect (sip, order, NUM_ORDER);
}

/*****************************************************************************
*
*   SeqIdPtr SeqIdSelect (sip, order, num)
*   	takes an array (order) num long.
*   	goes down chain starting with sip.
*       finds lowest value of order[sip->choice] and returns it.
*       if order[] == 255, it is skipped.
*       if nothing is found < 255, NULL is returned
*   	ErrorMessage if sip->choice >= num
*
*****************************************************************************/
SeqIdPtr SeqIdSelect (SeqIdPtr sip, Uint1Ptr order, Int2 num)
{
    SeqIdPtr bestid;

	if ((sip == NULL) || (order == NULL))
		return NULL;

    for ( bestid = NULL; sip != NULL; sip = sip -> next) 
    {
		if ((Int2)sip->choice < num)
		{
			if (order[sip->choice] < 255)
			{
				if (bestid == NULL)
					bestid = sip;
				else if (order[sip->choice] < order[bestid->choice])
					bestid = sip;
			}
		}
		else
			ErrPost(CTX_NCBIOBJ, 1, "SeqIdSelect: choice [%d] out of range [%d]",
				(int)(sip->choice), (int)num);
    }

    return bestid;
}

	static char * delim = "|";
	static char * txtid [16] = {		  /* FASTA_LONG formats */
		"???" ,		/* not-set = ??? */
		"lcl",		/* local = lcl|integer or string */
		"bbs",     /* gibbsq = bbs|integer */
		"bbm",		/* gibbmt = bbm|integer */
		"gim",		/* giim = gim|integer */
		"gb",		/* genbank = gb|accession|locus */
		"emb",		/* embl = emb|accession|locus */
		"pir",		/* pir = pir|accession|name */
		"sp",		/* swissprot = sp|accession|name */
		"pat",		/* patent = pat|country|patent number (string)|seq number (integer) */
		"oth",		/* other = oth|accession|name|release */
		"gnl",		/* general = gnl|database(string)|id (string or number) */
		"gi",		/* gi = gi|integer */
		"dbj",		/* ddbj = dbj|accession|locus */
		"prf",		/* prf = prf|accession|name */
		"pdb" };	/* pdb = pdb|entry name (string)|chain id (char) */

/*****************************************************************************
*
*   SeqIdPrint(sip, buf, format)
*   	PRINTID_FASTA_LONG treats sip as a chain, printing gi|other id
*   		other id is as given in the comments for txtid. Empty fields
*   		do not eliminate | delimiters
*   	PRINTID_FASTA_SHORT prints only the sip.
*   		same format as FASTA_LONG (for other id)
*
*   	PRINTID_TEXTID_LOCUS or ACCESSION
*  --------------------------------------------------------
*  | OLDWAY:                                              |
*  |      TextSeqId types- fills request or first char in |
*  |      buffer \0 if cannot be filled                   |
*  |        gibbmt, gibbsq = fills with _M or _S [number] |
*  |      other types- fills in as FASTA_SHORT            |
*  --------------------------------------------------------
*      CURRENTLY:
*      for SEQID_GENBANK,SEQID_EMBL,SEQID_DDBJ, takes accession
*        or locus field; for SEQID_LOCAL, takes str 
*              as accession only
*       ALL others as FASTA_SHORT
*
*       PRINTID_REPORT- similar to FASTA_SHORT but removes extra optional
*         fields and | to make more human readable (but less parseable)
*   
*   if format is in the range ' ' to 127 (32-12y) ASCII, then the character
*     given is used as a separator instead of '|' and the format is
*     PRINTID_FASTA_SHORT. 127 is translated as TAB (ASCII 9)
*     This makes this function flexible for bulk
*     data processing. Note that this invalidates SeqIdParse() and may create
*     conflicts with names. Use with caution.
*     
*   return value points to \0 at end of buf
*   
*****************************************************************************/
CharPtr SeqIdPrint (SeqIdPtr isip, CharPtr buf, Uint1 format)

{
	SeqIdPtr sip;
    char localbuf[15];    /* for MS Windows */
	char *ldelim;
	char d [2];
    CharPtr tmp;
	static Uint1 fasta_order[NUM_ORDER] = {  /* order for other id FASTA_LONG */
 	33, /* 0 = not set */
	20, /* 1 = local Object-id */
	15,  /* 2 = gibbsq */
	16,  /* 3 = gibbmt */
	30, /* 4 = giim Giimport-id */
	10, /* 5 = genbank */
	10, /* 6 = embl */
	10, /* 7 = pir */
	10, /* 8 = swissprot */
	15,  /* 9 = patent */
	20, /* 10 = other TextSeqId */
	20, /* 11 = general Dbtag */
	255,  /* 12 = gi */
	10, /* 13 = ddbj */
	10, /* 14 = prf */
	12  /* 15 = pdb */
    };
    TextSeqIdPtr tsip;
	PDBSeqIdPtr psip;
	ObjectIdPtr oip;
	PatentSeqIdPtr patsip;
	Boolean got_gi = FALSE;

	buf[0] = '\0';
	tmp = buf;
	if (isip == NULL)
		return tmp;

	d [0] = *delim;
	d [1] = '\0';
	ldelim = &(d [0]);
	if ((format >= ' ') && (format <= 127))  /* change delimiter */
	{
		if (format == 127)
			d [0] = '\t';
		else
			d [0] = (char) format;
		format = PRINTID_FASTA_SHORT;
	}
	
	localbuf[0] = '\0';
							/* error on input, return ??? */
	if ( (! (isip -> choice)) || (format < PRINTID_FASTA_SHORT)
		|| (format > PRINTID_REPORT))
	{
		return StringMove(tmp, txtid[0]);
	}

	if (format == PRINTID_FASTA_LONG)   /* find the ids in the chain */
	{
		for (sip = isip; sip != NULL; sip = sip->next)  /* GI present? */
		{
			if (sip->choice == SEQID_GI)
			{
				sprintf(localbuf, "%s%s%ld", txtid[SEQID_GI], ldelim,
					(long)(sip->data.intvalue));
				tmp = StringMove(tmp, localbuf);
				got_gi = TRUE;
				break;
			}
		}
		sip = SeqIdSelect(isip, fasta_order, NUM_ORDER);
		if (sip == NULL)   /* only GI */
			return tmp;
		else if (got_gi)
			tmp = StringMove(tmp, ldelim);
	}
	else
		sip = isip;          /* only one id processed */

							 /* deal with LOCUS and ACCESSION */
	if ((format == PRINTID_TEXTID_ACCESSION) || (format == PRINTID_TEXTID_LOCUS))
	{
		switch (sip->choice)   /* get the real TextSeqId types */
		{
	        case SEQID_GENBANK:
    	    case SEQID_EMBL:
    	    case SEQID_DDBJ:
    	        tsip = (TextSeqIdPtr)sip->data.ptrvalue;
				if ((format == PRINTID_TEXTID_LOCUS) && (tsip->name != NULL))
                    tmp = StringMove(tmp, tsip->name);
                else if ((format == PRINTID_TEXTID_ACCESSION)
					&& (tsip->accession != NULL))
                    tmp = StringMove(tmp, tsip->accession);
				return tmp;
					case SEQID_LOCAL:
					if (format == PRINTID_TEXTID_ACCESSION)
						if ((((ObjectIdPtr)sip->data.ptrvalue)->str)
							 != NULL){
						 tmp = StringMove(tmp, 
							((ObjectIdPtr)sip->data.ptrvalue)->str);
							return tmp;
						}
					break;
			default:
				break;
		}
	}

						/* FASTA_SHORT or REPORT */
	tmp = StringMove(tmp, txtid[sip->choice]);
	tmp = StringMove(tmp, ldelim);

    switch (sip->choice) 
    {
        case SEQID_LOCAL:           /* object id */
            if ((((ObjectIdPtr)sip->data.ptrvalue)->str) == NULL)
			{
                sprintf(localbuf, "%ld", 
							(long)((ObjectIdPtr)sip->data.ptrvalue)->id);
				tmp = StringMove(tmp, localbuf);
			}
            else
                tmp = StringMove(tmp, ((ObjectIdPtr)sip->data.ptrvalue)->str);
            break;
        case SEQID_GIBBSQ:         
        case SEQID_GIBBMT:
		case SEQID_GI:
			sprintf(localbuf, "%ld", (long)sip->data.intvalue);
			tmp = StringMove(tmp, localbuf);
            break;
        case SEQID_GIIM:
            sprintf(localbuf, "%ld", (long)((GiimPtr)sip->data.ptrvalue)->id);
			tmp = StringMove(tmp, localbuf);
            break;
        case SEQID_GENBANK:
        case SEQID_EMBL:
        case SEQID_PIR:
        case SEQID_SWISSPROT:
        case SEQID_DDBJ:
		case SEQID_PRF:
        case SEQID_OTHER:
            tsip = (TextSeqIdPtr)sip->data.ptrvalue;
            if (tsip->accession != NULL)
			{
               tmp = StringMove(tmp, tsip->accession);
			   if (format == PRINTID_REPORT)
			 	break;
			}
			if (format != PRINTID_REPORT)
				tmp = StringMove(tmp, ldelim);
			if (tsip->name != NULL)
				tmp = StringMove(tmp, tsip->name);
            break;
        case SEQID_PATENT:
			patsip = (PatentSeqIdPtr)(sip->data.ptrvalue);
			tmp = StringMove(tmp, patsip->cit->country);
			tmp = StringMove(tmp, ldelim);
			tmp = StringMove(tmp, patsip->cit->number);
			tmp = StringMove(tmp, ldelim);
			sprintf(localbuf, "%d", (int)patsip->seqid);
			tmp = StringMove(tmp, localbuf);
            break;
        case SEQID_GENERAL:
			oip = ((DbtagPtr)sip->data.ptrvalue)->tag;
			tmp = StringMove(tmp, ((DbtagPtr)sip->data.ptrvalue)->db);
			tmp = StringMove(tmp, ldelim);
			if (oip->str == NULL)
			{
				sprintf(localbuf, "%ld", oip->id);
				tmp = StringMove(tmp, localbuf);
			}
			else
				tmp = StringMove(tmp, oip->str);
            break;
        case SEQID_PDB:
			psip = (PDBSeqIdPtr) sip->data.ptrvalue;
			tmp = StringMove(tmp, psip->mol);
			tmp = StringMove(tmp, ldelim);
			if (psip->chain != '\0')
				*tmp = psip->chain;
			else
				*tmp = ' ';
			tmp++;
			*tmp = '\0';
            break;
    }
    return tmp;
}

/*****************************************************************************
*
*   SeqIdPtr SeqIdParse(buf)
*   	parses a string containing SeqIds formated by SeqIdPrint using
*   	FASTA_LONG or FASTA_SHORT, separated by |
*   	returns a SeqId linked list for them
*   	or NULL on failure for any SeqId
*
*****************************************************************************/
SeqIdPtr SeqIdParse(CharPtr buf)
{
	char localbuf[42];
	char * tmp, *strt, * tokens[6];
	char d;
	long num;
	CharPtr tp;
	Int2 numtoken, i, type, j;
	SeqIdPtr sip = NULL, head = NULL, last = NULL;
	ObjectIdPtr oip;
	DbtagPtr dp;
	TextSeqIdPtr tsip;
	PatentSeqIdPtr patsip;
	IdPatPtr ipp;
	PDBSeqIdPtr psip;
	GiimPtr gim;
	Boolean done = FALSE;
	static Uint1 expect_tokens[NUM_ORDER] = {  /* number of tokens to expect */
 	0, /* 0 = not set */
	1, /* 1 = local Object-id */
	1,  /* 2 = gibbsq */
	1,  /* 3 = gibbmt */
	1, /* 4 = giim Giimport-id */
	2, /* 5 = genbank */
	2, /* 6 = embl */
	2, /* 7 = pir */
	2, /* 8 = swissprot */
	3,  /* 9 = patent */
	3, /* 10 = other TextSeqId */
	2, /* 11 = general Dbtag */
	1,  /* 12 = gi */
	2, /* 13 = ddbj */
	2, /* 14 = prf */
	2  /* 15 = pdb */
	};

	if ((buf == NULL) || (*buf == '\0'))
		return NULL;

	d = *delim;   /* delimiter */
	while (! done)
	{
						/* set all tokens pointing to \0 */
		localbuf[41] = '\0';
		for (i = 0; i < 6; i++)
			tokens[i] = &localbuf[41];
		tp = buf;		/* save start of string */
						/* copy and tokenize - token\0token\0\n */
		for (tmp=localbuf, i=0; ((*buf != d) && (*buf != '\0') && (i < 40));
				i++,buf++,tmp++)
			*tmp = *buf;
		if (*buf != d) goto erret;  /* didn't get delimiter */
		*tmp = '\0';
		tmp++;
		buf++;
		for (j = 0, type = 0; j < NUM_ORDER; j++)
		{
			if (! StringCmp(localbuf, txtid[j]))
			{
				type = j;
				break;
			}
		}

		if (! type) goto erret;

						/* copy and tokenize - token\0token\0\n */
		for (numtoken=0, strt=tmp;
			((i < 40) && (numtoken < (Int2)(expect_tokens[type])));
			i++,buf++,tmp++)
		{
			if ((*buf == d) || (*buf == '\0'))
			{
				*tmp = '\0';
				tokens[numtoken] = strt;
				numtoken++;
				if (*buf == '\0')
				{
					if (numtoken < (Int2)(expect_tokens[type]))
						goto erret;
					else
						done = TRUE;
				}
				strt = tmp+1;
			}
			else
				*tmp = *buf;
		}
		if (i == 40) goto erret;

		sip = ValNodeNew(head);
		if (head == NULL) head = sip;
		sip->choice = (Uint1) type;
		switch (type)
    	{
        	case SEQID_LOCAL:           /* object id */
				if (*tokens[0] == '\0') goto erret;
				oip = ObjectIdNew();
				sip->data.ptrvalue = oip;
				for (tmp = tokens[0]; *tmp != '\0'; tmp++)
				{
					if (! IS_DIGIT(*tmp))   /* string type */
					{
						oip->str = StringSave(tokens[0]);
						break;
					}
				}
				if (oip->str == NULL)
				{
					sscanf(tokens[0], "%ld", &num);
					oip->id = (Int4)num;
				}
				break;
	        case SEQID_GIBBSQ:         
    	    case SEQID_GIBBMT:
			case SEQID_GI:
				if (! IS_DIGIT(*tokens[0]))
					goto erret;
				sscanf(tokens[0], "%ld", &num);
				sip->data.intvalue = (Int4)num;
    	        break;
        	case SEQID_GIIM:
				if (! IS_DIGIT(*tokens[0])) goto erret;
				gim = GiimNew();
				sip->data.ptrvalue = gim;
				sscanf(tokens[0], "%ld", &num);
				gim->id = (Int4)num;
	            break;
    	    case SEQID_GENBANK:
        	case SEQID_EMBL:
	        case SEQID_PIR:
    	    case SEQID_SWISSPROT:
        	case SEQID_DDBJ:
			case SEQID_PRF:
    	    case SEQID_OTHER:
				if ((*tokens[0] == '\0') && (*tokens[1] == '\0'))
					goto erret;
	            tsip = TextSeqIdNew();
				sip->data.ptrvalue = tsip;
				if (*tokens[0] != '\0')
					tsip->accession = StringSave(tokens[0]);
				if (*tokens[1] != '\0')
					tsip->name = StringSave(tokens[1]);
        	    break;
	        case SEQID_PATENT:
				if ((*tokens[0] == '\0') || (*tokens[1] == '\0')) goto erret;
				if (! IS_DIGIT(*tokens[2])) goto erret;
				patsip = PatentSeqIdNew();
				sip->data.ptrvalue = patsip;
				ipp = IdPatNew();
				patsip->cit = ipp;
				ipp->country = StringSave(tokens[0]);
				ipp->number = StringSave(tokens[1]);
				sscanf(tokens[2], "%ld", &num);
				patsip->seqid = (Int2)num;
            	break;
	        case SEQID_GENERAL:
				if ((*tokens[0] == '\0') || (*tokens[1] == '\0')) goto erret;
				dp = DbtagNew();
				sip->data.ptrvalue = dp;
				oip = ObjectIdNew();
				dp->tag = oip;
				dp->db = StringSave(tokens[0]);
				for (tmp = tokens[1]; *tmp != '\0'; tmp++)
				{
					if (! IS_DIGIT(*tmp))   /* string type */
					{
						oip->str = StringSave(tokens[1]);
						break;
					}
				}
				if (oip->str == NULL)
				{
					sscanf(tokens[1], "%ld", &num);
					oip->id = (Int4)num;
				}
        	    break;
	        case SEQID_PDB:
				if (*tokens[0] == '\0') goto erret;
				psip = PDBSeqIdNew();
				sip->data.ptrvalue = psip;
				psip->mol = StringSave(tokens[0]);
				psip->chain = *tokens[1];
        	    break;
	    }
		last = sip;
		sip = NULL;
	}
ret:
	return head;
erret:
	StringNCpy(localbuf, tp, 40);
	localbuf[40] = '\0';
	ErrPost(CTX_NCBIOBJ, 1, "SeqIdParse Failure at %s", localbuf);
	if (sip == head)
		head = NULL;
	else
	{
		if (last != NULL)
			last->next = NULL;
		head = SeqIdSetFree(head);
	}
	ValNodeFree(sip);
	goto ret;
}


/*****************************************************************************
*
*   Boolean SeqIdMatch(a, b)
*
*  The code here must work the same is in two idloader
*  context: function id_flatten_seq_obj (idsybase.c)
*  and proc id_id_flatten_seq_obj
*
*****************************************************************************/
Boolean SeqIdMatch (SeqIdPtr a, SeqIdPtr b)

{
    Uint1 choice;
    TextSeqIdPtr at, bt;

    if ((a == NULL) || (b == NULL))
        return FALSE;

	choice = a->choice;
	if (choice != b->choice)
	{
		switch (choice)
		{
			case SEQID_GENBANK:          /* these could be confused */
			case SEQID_EMBL:
			case SEQID_DDBJ:
				switch (b->choice)
				{
					case SEQID_GENBANK:   /* its ok */
					case SEQID_EMBL:
					case SEQID_DDBJ:
						break;  
					default:
						return FALSE;
				}
				break;
			default:
				return FALSE;
		}
	}

    switch (choice)
    {
        case SEQID_NOT_SET:
            return FALSE;
        case SEQID_LOCAL:   
            return ObjectIdMatch((ObjectIdPtr)a->data.ptrvalue, (ObjectIdPtr)b->data.ptrvalue);
        case SEQID_GIBBSQ:   /* gibbsq */
        case SEQID_GIBBMT:   /* gibbmt */
        case SEQID_GI:  /* gi */
            return (Boolean)(a->data.intvalue == b->data.intvalue);
        case SEQID_GIIM:   /* giim */
            return (Boolean)(((GiimPtr)a->data.ptrvalue)->id == ((GiimPtr)b->data.ptrvalue)->id);
        case SEQID_PATENT:   /* patent seq */
            if (((PatentSeqIdPtr)a->data.ptrvalue)->seqid !=
                ((PatentSeqIdPtr)b->data.ptrvalue)->seqid)
                return FALSE;
            return IdPatMatch(((PatentSeqIdPtr)a->data.ptrvalue)->cit,
                ((PatentSeqIdPtr)b->data.ptrvalue)->cit);
		case SEQID_PDB:     /* pdb */
            if ( StringCmp(((PDBSeqIdPtr)a->data.ptrvalue)->mol,
                ((PDBSeqIdPtr)b->data.ptrvalue)->mol))
                return FALSE;
            if (((PDBSeqIdPtr)a->data.ptrvalue)->chain !=
                ((PDBSeqIdPtr)b->data.ptrvalue)->chain)
                return FALSE;
			return TRUE;
		case SEQID_GENERAL:  /* general */
			return DbtagMatch((DbtagPtr)a->data.ptrvalue,
				(DbtagPtr)b->data.ptrvalue);
		case SEQID_OTHER:  /* if both accession and name, use both */
            at = (TextSeqIdPtr)a->data.ptrvalue;
            bt = (TextSeqIdPtr)b->data.ptrvalue;
            if ((at->accession != NULL) && (bt->accession != NULL)) {
                if (! StringICmp(at->accession, 
                        bt->accession)){
                     if ((at->name != NULL) && 
                           (bt->name != NULL)) {
                         if (! StringICmp(at->name, bt->name))
                             return TRUE;
                         else
                             return FALSE;
                      }else if ((at->name != NULL) || 
                           (bt->name != NULL)) {
               			   return FALSE;  /* if one has name, both must */
                      }else{
                        return TRUE;
                      }
                }else
                    return FALSE;
            }
            else if ((at->name != NULL) && (bt->name != NULL))
            {
                if (! StringICmp(at->name, bt->name))
                    return TRUE;
                else
                    return FALSE;
            }
            else
                return FALSE;
        case SEQID_GENBANK:
		case SEQID_EMBL:
		case SEQID_DDBJ:
		case SEQID_PIR:
		case SEQID_SWISSPROT:
		case SEQID_PRF:
            at = (TextSeqIdPtr)a->data.ptrvalue;
            bt = (TextSeqIdPtr)b->data.ptrvalue;
            if ((at->accession != NULL) && (bt->accession != NULL))
            {
                if (! StringICmp(at->accession, bt->accession))
                    return TRUE;
                else
                    return FALSE;
            }
            else if ((at->name != NULL) && (bt->name != NULL))
            {
                if (! StringICmp(at->name, bt->name))
                    return TRUE;
                else
                    return FALSE;
            }
            else
                return FALSE;
		default:
			ErrPost(CTX_NCBIOBJ, 1, "SeqIdMatch: unsupported type [%d]",
				(int)choice);
			return FALSE;
     }
}

/*****************************************************************************
*
*   Boolean SeqIdIn(a, b)
*     Looks for single SeqId, "a" in chain of SeqIds, "b"
*
*****************************************************************************/
Boolean SeqIdIn (SeqIdPtr a, SeqIdPtr b)

{
		SeqIdPtr now;

    if ((a == NULL) || (b == NULL))
        return FALSE;

		for ( now =b; now != NULL; now = now -> next){
			if (SeqIdMatch(a,now)){
				return TRUE;
			}
		}

		return FALSE;
}

/*****************************************************************************
*
*   SeqIdForSameBioseq(a,b)
*
*****************************************************************************/
Boolean SeqIdForSameBioseq (SeqIdPtr a, SeqIdPtr b)

{
    BioseqPtr bsp;

	if ((a == NULL) || (b == NULL)) return FALSE;
	
    if (SeqIdMatch(a,b))   /* if match, all set */
        return TRUE;

    if ((bsp = BioseqFind(a)) != NULL)
    {
        return SeqIdIn(b, bsp->id);
    }

    return FALSE;
}

/*****************************************************************************
*
*   Traversal routine for SeqLocFindNext
*
*****************************************************************************/
static SeqLocPtr SeqLocNext (SeqLocPtr seqlochead, SeqLocPtr currseqloc)

{
    SeqLocPtr currloc, retval;

    while (seqlochead != NULL)
    {
        if (IS_one_loc(seqlochead))
        {
            if (currseqloc == NULL)
                return seqlochead;
            else if (currseqloc == seqlochead)   /* found it */
						if (seqlochead -> next != NULL){
							if (IS_one_loc(seqlochead->next))
								 return seqlochead->next;
							else
								return SeqLocNext(seqlochead->next, NULL);
							}else{
								return NULL;
							}
            else
                seqlochead = seqlochead->next;
        }
        else
        {
            currloc = (SeqLocPtr)seqlochead->data.ptrvalue;
            while (currloc != NULL)
            {
                if ((retval = SeqLocNext(currloc, currseqloc)) != NULL)
                    return retval;
                else
                    currloc = currloc->next;
            }
				if ( currloc == NULL)
					break;
        }
    }
    return NULL;
}

/*****************************************************************************
*
*   SeqLocFindNext(seqlochead, currseqloc)
*       finds the next Seq-loc after currseqloc
*       seqlochead is the first of a chain of Seq-locs
*
*****************************************************************************/
SeqLocPtr SeqLocFindNext (SeqLocPtr seqlochead, SeqLocPtr currseqloc)

{
	SeqLocPtr tmp;

	if (seqlochead == NULL) return NULL;

	if (IS_one_loc(seqlochead))    /* not a chain */
	{
		if (currseqloc == NULL)	   /* first call */
			return seqlochead;
		else if (currseqloc == seqlochead)	 /* second call */
			return NULL;
		else						   /* oops */
			goto erret;
	}

	if (currseqloc != NULL)
	{
		if (! IS_one_loc(currseqloc)) /* oops */
			goto erret;
		tmp = currseqloc->next;
		if (tmp != NULL)
		{
			if (IS_one_loc(tmp))
				return tmp;
		}
	}
   
    return SeqLocNext(seqlochead, currseqloc);

erret:
    ErrPost(CTX_NCBIOBJ, 1, "Invalid arguments to SeqLocFindNext");
    return NULL;
}

/*****************************************************************************
*
*   IS_one_loc(anp)
*       returns TRUE if is a sequence location which refers to one piece
*   	of sequence
*       used for moving through complicated Seq-locs
*
*****************************************************************************/
Boolean IS_one_loc (SeqLocPtr anp)      /* a SeqLoc */

{
	Boolean retval = FALSE;

	if (anp == NULL) return FALSE;
	
	switch (anp->choice)
    {
        case SEQLOC_NULL:      /* null - not a valid single region */
        case SEQLOC_EMPTY:      /* empty */
        case SEQLOC_WHOLE:      /* whole */
        case SEQLOC_INT:      /* int */
        case SEQLOC_PNT:      /* pnt */
        case SEQLOC_PACKED_PNT:      /* packed-pnt   */
        case SEQLOC_BOND:      /* bond */
			retval = TRUE;
			break;

        case SEQLOC_EQUIV:     /* equiv */
        case SEQLOC_PACKED_INT:      /* packed seqint */
        case SEQLOC_MIX:      /* mix */
        case SEQLOC_FEAT:
			retval = FALSE;
            break;

		default:
			ErrPost(CTX_NCBIOBJ, 1, "IS_one_seq: unsupported seqloc [%d]",
				(int)(anp->choice));
			retval = FALSE;
            break;
	}
	return retval;
}
/*****************************************************************************
*
*   SeqLocId(loc)
*
*****************************************************************************/
SeqIdPtr SeqLocId (SeqLocPtr anp)

{
    SeqIdPtr seqid = NULL, currseqid = NULL;
	SeqLocPtr loc;

	if (anp == NULL) return NULL;
	
    switch (anp->choice)
    {
        case SEQLOC_NULL:    /* NULL */
        case SEQLOC_BOND:   /* bond -- 2 seqs */
        case SEQLOC_FEAT:   /* feat -- can't track yet */
            break;
        case SEQLOC_EMPTY:    /* empty */
        case SEQLOC_WHOLE:    /* whole */
            seqid = (SeqIdPtr)anp->data.ptrvalue;
            break;
        case SEQLOC_INT:    /* int */
            seqid = ((SeqIntPtr)anp->data.ptrvalue)->id;
			break;
        case SEQLOC_PACKED_INT:    /* packed int */
        case SEQLOC_MIX:    /* mix -- could be more than one seq */
        case SEQLOC_EQUIV:    /* equiv -- ditto */
			loc = (SeqLocPtr)anp->data.ptrvalue;
			while (loc != NULL)
			{
				currseqid =	SeqLocId(loc);
				if (seqid == NULL)
					seqid = currseqid;
				else
				{
					if (! SeqIdMatch(seqid, currseqid))
					{
						seqid = NULL;
						loc = NULL;
						break;
					}
				}
				loc = loc->next;
			}
            break;
        case SEQLOC_PNT:    /* pnt */
            seqid = ((SeqPntPtr)anp->data.ptrvalue)->id;
            break;
        case SEQLOC_PACKED_PNT:    /* packed pnt */
            seqid = ((PackSeqPntPtr)anp->data.ptrvalue)->id;
            break;
        default:
            break;
    }
    return seqid;
}

/*****************************************************************************
*
*   SeqLocStart(loc)
*   	returns lowest number position for Seq-loc all on one bioseq
*   	returns -1 if impossible to meet that condition
*
*****************************************************************************/
Int4 SeqLocStart (SeqLocPtr anp)   /* seqloc */

{
    Int4 pos = -1L, tpos, numpnt;
	SeqIdPtr sip;
	SeqLocPtr slp;

    if (anp == NULL)
        return pos;

    switch (anp->choice)
    {
        case SEQLOC_BOND:   /* bond -- 2 seqs */
        case SEQLOC_FEAT:   /* feat -- can't track yet */
        case SEQLOC_NULL:    /* NULL */
        case SEQLOC_EMPTY:    /* empty */
			break;
        case SEQLOC_WHOLE:    /* whole */
			pos = 0L;
			break;
        case SEQLOC_MIX:    /* mix -- more than one seq */
        case SEQLOC_EQUIV:    /* equiv -- ditto */
        case SEQLOC_PACKED_INT:    /* packed int */
			sip = SeqLocId(anp);
			if (sip != NULL)      /* all on one Bioseq */
			{
				slp = (SeqLocPtr)anp->data.ptrvalue;
				while (slp != NULL)
				{
					tpos = SeqLocStart(slp);
					if (pos < 0)
						pos = tpos;
					else if (tpos < pos)
						pos = tpos;
					slp = slp->next;
				}
			}
            break;
        case SEQLOC_INT:    /* int */
            pos = ((SeqIntPtr)anp->data.ptrvalue)->from;
            break;
        case SEQLOC_PNT:    /* pnt */
            pos = ((SeqPntPtr)anp->data.ptrvalue)->point;
            break;
        case SEQLOC_PACKED_PNT:    /* packed pnt */
			numpnt = PackSeqPntNum((PackSeqPntPtr)anp->data.ptrvalue);
			while (numpnt)
			{
				tpos = PackSeqPntGet((PackSeqPntPtr)anp->data.ptrvalue, numpnt);
				if (pos < 0)
					pos = tpos;
				else if (tpos < pos)
					pos = tpos;
				numpnt--;
			}
            break;
        default:
            break;
    }
    return pos;
}

/*****************************************************************************
*
*   SeqLocStop(loc)
*   	looks for highest position number on loc if on one Bioseq
*   	if fails, returns -1
*
*****************************************************************************/
Int4 SeqLocStop (SeqLocPtr anp)   /* seqloc */

{
    BioseqPtr bsp;
    Int4 pos = -1L, tpos, numpnt;
	SeqIdPtr sip;
	SeqLocPtr slp;


    if (anp == NULL)
        return pos;

    switch (anp->choice)
    {
        case SEQLOC_BOND:   /* bond -- 2 seqs */
        case SEQLOC_FEAT:   /* feat -- can't track yet */
        case SEQLOC_NULL:    /* NULL */
        case SEQLOC_EMPTY:    /* empty */
			break;
        case SEQLOC_WHOLE:    /* whole */
            bsp = BioseqFind((SeqIdPtr)anp->data.ptrvalue);
            pos = BioseqGetLen(bsp) - 1;
			break;
        case SEQLOC_MIX:    /* mix -- more than one seq */
        case SEQLOC_EQUIV:    /* equiv -- ditto */
        case SEQLOC_PACKED_INT:    /* packed int */
			sip = SeqLocId(anp);
			if (sip != NULL)      /* all on one Bioseq */
			{
				slp = (SeqLocPtr)anp->data.ptrvalue;
				while (slp != NULL)
				{
					tpos = SeqLocStop(slp);
					if (pos < 0)
						pos = tpos;
					else if (tpos > pos)
						pos = tpos;
					slp = slp->next;
				}
			}
            break;
        case SEQLOC_INT:    /* int */
            pos = ((SeqIntPtr)anp->data.ptrvalue)->to;
            break;
        case SEQLOC_PNT:    /* pnt */
            pos = ((SeqPntPtr)anp->data.ptrvalue)->point;
            break;
        case SEQLOC_PACKED_PNT:    /* packed pnt */
			numpnt = PackSeqPntNum((PackSeqPntPtr)anp->data.ptrvalue);
			while (numpnt)
			{
				tpos = PackSeqPntGet((PackSeqPntPtr)anp->data.ptrvalue, numpnt);
				if (pos < 0)
					pos = tpos;
				else if (tpos > pos)
					pos = tpos;
				numpnt--;
			}
            break;
        default:
            break;
    }
    return pos;
}

/*****************************************************************************
*
*   SeqLocStrand(loc)
*       see objloc.h for strand value defines
*       returns Seq_strand_other when series of locs on different strands
*
*****************************************************************************/
Uint1 SeqLocStrand (SeqLocPtr anp)   /* seqloc */

{
	SeqIdPtr sip;
	SeqLocPtr slp;
    Uint1 strand = Seq_strand_unknown, tstrand;

    if (anp == NULL)
        return strand;

    switch (anp->choice)
    {
        case SEQLOC_BOND:   /* bond -- 2 seqs */
        case SEQLOC_FEAT:   /* feat -- can't track yet */
        case SEQLOC_NULL:    /* NULL */
        case SEQLOC_EMPTY:    /* empty */
			break;
        case SEQLOC_WHOLE:    /* whole */
			strand = Seq_strand_both;
			break;
        case SEQLOC_MIX:    /* mix -- more than one seq */
        case SEQLOC_EQUIV:    /* equiv -- ditto */
        case SEQLOC_PACKED_INT:    /* packed int */
			sip = SeqLocId(anp);
			if (sip != NULL)      /* all on one Bioseq */
			{
				for (slp = (SeqLocPtr)anp->data.ptrvalue, 
						strand = SeqLocStrand(slp), slp = slp -> next;
						slp != NULL ; slp = slp->next)
				{
					tstrand = SeqLocStrand(slp);
					if (strand != tstrand)
					{
						strand = Seq_strand_other;
						break;
					}
				}
			}
            break;
        case SEQLOC_INT:    /* int */
            strand = ((SeqIntPtr)anp->data.ptrvalue)->strand;
            break;
        case SEQLOC_PNT:    /* pnt */
            strand = ((SeqPntPtr)anp->data.ptrvalue)->strand;
            break;
        case SEQLOC_PACKED_PNT:    /* packed pnt */
			strand = ((PackSeqPntPtr)anp->data.ptrvalue)->strand;
            break;
        default:
            break;
    }
    return strand;
}


/*****************************************************************************
*
*   Int4 SeqLocGetSegLens (slp, lens, ctr, gaps)
*       returns total number of segments in SeqLoc including NULLS
*       returns -1 for error
*       if lens != NULL fills with lengths of segments, 0 = NULL
*
*****************************************************************************/
Int4 SeqLocGetSegLens (SeqLocPtr slp, Int4Ptr lens, Int4 ctr, Boolean gaps)
{
	SeqLocPtr slp2;
	BioseqPtr bsp;

	if (slp == NULL)
		return -1;

    switch (slp->choice)
    {
        case SEQLOC_BOND:   /* bond -- 2 seqs */
        case SEQLOC_FEAT:   /* feat -- can't track yet */
			break;
        case SEQLOC_NULL:    /* NULL */
        case SEQLOC_EMPTY:    /* empty */
			if (lens != NULL)
				lens[ctr] = 0;
			ctr++;
			break;
        case SEQLOC_WHOLE:    /* whole */
			if (gaps)
				break;
			if (lens != NULL)
			{
	            bsp = BioseqFind((SeqIdPtr)slp->data.ptrvalue);
    	        lens[ctr] = BioseqGetLen(bsp);
			}
			ctr++;
			break;
        case SEQLOC_MIX:    /* mix -- more than one seq */
        case SEQLOC_EQUIV:    /* equiv -- ditto */
        case SEQLOC_PACKED_INT:    /* packed int */
			slp2 = (SeqLocPtr)slp->data.ptrvalue;
			while (slp2 != NULL)
			{
				ctr += SeqLocGetSegLens(slp2, lens, ctr, gaps);
				slp2 = slp2->next;
			}
            break;
        case SEQLOC_INT:    /* int */
			if (gaps) break;
			if (lens != NULL)
            	lens[ctr] = ((SeqIntPtr)slp->data.ptrvalue)->to - ((SeqIntPtr)slp->data.ptrvalue)->from + 1;
			ctr++;
            break;
        case SEQLOC_PNT:    /* pnt */
			if (gaps) break;
			if (lens != NULL)
	            lens[ctr] = 1;
			ctr++;
            break;
        case SEQLOC_PACKED_PNT:    /* packed pnt */
			if (gaps) break;
			if (lens != NULL)
				lens[ctr] = SeqLocStop(slp) - SeqLocStart(slp) + 1;
			ctr++;
            break;
        default:
            break;
    }
    return ctr;
}

/*****************************************************************************
*
*   SeqLocLen(loc)
*   	returns total length in residues of loc
*   	if fails, returns -1
*
*****************************************************************************/
Int4 SeqLocLen (SeqLocPtr anp)   /* seqloc */

{
    BioseqPtr bsp;
    Int4 len = -1L, tmp;
	SeqLocPtr slp;


    if (anp == NULL)
        return len;

    switch (anp->choice)
    {
        case SEQLOC_BOND:   /* bond -- 2 seqs */
        case SEQLOC_FEAT:   /* feat -- can't track yet */
			break;
        case SEQLOC_NULL:    /* NULL */
        case SEQLOC_EMPTY:    /* empty */
			len = 0;
			break;
        case SEQLOC_WHOLE:    /* whole */
            bsp = BioseqFind((SeqIdPtr)anp->data.ptrvalue);
            len = BioseqGetLen(bsp);
			break;
        case SEQLOC_MIX:    /* mix -- more than one seq */
        case SEQLOC_EQUIV:    /* equiv -- ditto */
        case SEQLOC_PACKED_INT:    /* packed int */
			slp = (SeqLocPtr)anp->data.ptrvalue;
			len = 0;
			while (slp != NULL)
			{
				tmp = SeqLocLen(slp);
				if (tmp == -1)
					return -1;
				len += tmp;
				slp = slp->next;
			}
            break;
        case SEQLOC_INT:    /* int */
            len = ((SeqIntPtr)anp->data.ptrvalue)->to - ((SeqIntPtr)anp->data.ptrvalue)->from + 1;
            break;
        case SEQLOC_PNT:    /* pnt */
            len = 1;
            break;
        case SEQLOC_PACKED_PNT:    /* packed pnt */
			len = SeqLocStop(anp) - SeqLocStart(anp) + 1;
            break;
        default:
            break;
    }
    return len;
}

/*****************************************************************************
*
*   SeqLocRevCmp(loc)
*   	reverse complements a SeqLoc
*   	NO Check to be sure its on a nucleic acid
*
*****************************************************************************/
Boolean SeqLocRevCmp (SeqLocPtr anp)   /* seqloc */

{
	SeqLocPtr slp, first, curr, prev;
	SeqPntPtr spp;


    if (anp == NULL)
        return FALSE;

    switch (anp->choice)
    {
        case SEQLOC_BOND:   /* bond -- 2 seqs */
			spp = ((SeqBondPtr)anp->data.ptrvalue)->a;
			spp->strand = StrandCmp(spp->strand);
			spp = ((SeqBondPtr)anp->data.ptrvalue)->b;
			if (spp != NULL)
				spp->strand = StrandCmp(spp->strand);
			break;
        case SEQLOC_FEAT:   /* feat -- can't track yet */
        case SEQLOC_NULL:    /* NULL */
        case SEQLOC_EMPTY:    /* empty */
        case SEQLOC_WHOLE:    /* whole */
			break;
        case SEQLOC_MIX:    /* mix -- more than one seq */
        case SEQLOC_EQUIV:    /* equiv -- ditto */
        case SEQLOC_PACKED_INT:    /* packed int */
			slp = (SeqLocPtr)anp->data.ptrvalue;
			while (slp != NULL)
			{
				SeqLocRevCmp(slp);     /* RevCmp subparts */
				slp = slp->next;
			}
			first = NULL;
			curr = NULL;
			prev = (SeqLocPtr)anp->data.ptrvalue;
			while (prev != NULL)  /* reverse order of parts */
			{					  /* no effect on meaning of SEQLOC_EQUIV */
				slp = (SeqLocPtr)anp->data.ptrvalue;
				prev = NULL;
				while (slp->next != NULL)
				{
					prev = slp;
					slp = slp->next;
				}
				if (prev != NULL) 
		   			prev->next = NULL;
				if (first == NULL)
					first = slp;
				else
					curr->next = slp;
				slp->next = NULL;
				curr = slp;
			}
			anp->data.ptrvalue = first;
            break;
        case SEQLOC_INT:    /* int */
            ((SeqIntPtr)anp->data.ptrvalue)->strand = StrandCmp(((SeqIntPtr)anp->data.ptrvalue)->strand);
            break;
        case SEQLOC_PNT:    /* pnt */
            ((SeqPntPtr)anp->data.ptrvalue)->strand = StrandCmp(((SeqPntPtr)anp->data.ptrvalue)->strand);
            break;
        case SEQLOC_PACKED_PNT:    /* packed pnt */
            ((PackSeqPntPtr)anp->data.ptrvalue)->strand = StrandCmp(((PackSeqPntPtr)anp->data.ptrvalue)->strand);
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

/*****************************************************************************
*
*   Uint1 StrandCmp(strand)
*   	returns the complement of a Strand
*
*****************************************************************************/
Uint1 StrandCmp (Uint1 strand)

{
	switch(strand)
	{
		case Seq_strand_unknown:     /* default to plus for this */
		case Seq_strand_plus:
			return (Uint1) Seq_strand_minus;
		case Seq_strand_minus:
			return (Uint1) Seq_strand_plus;
		case Seq_strand_both:
			return (Uint1) Seq_strand_both_rev;
		case Seq_strand_both_rev:
			return (Uint1) Seq_strand_both;
	}
	return strand;
}

/*****************************************************************************
*
*   SeqLocCompare(a, b)
*   	returns
*   	0 = no overlap
*   	1 = a is completely contained in b
*   	2 = b is completely contained in a
*   	3 = a == b
*   	4 = a and b overlap, but neither completely contained in the other
*   
*
*****************************************************************************/
Int2 SeqLocCompare (SeqLocPtr a, SeqLocPtr b)   /* seqloc */

{
    BioseqPtr bsp;
    Int4 len = -1L, i, j, num, num2, point, hits;
	SeqLocPtr slp, slp2;
	ValNode tmp;
	SeqBondPtr sbp;
	SeqIntPtr sip, sip2;
	SeqIdPtr sidp;
	PackSeqPntPtr pspp, pspp2;
	Boolean got_one, missed_one;
	Int2 retval = SLC_NO_MATCH,
		 retval2 = SLC_NO_MATCH;
	static Uint1 rettable [5][5] = {      /* for developing return values */
		{ 0,4,2,2,4 } ,					  /* when a is longer than b */
		{ 4,1,4,1,4 } ,
		{ 2,4,2,2,4 } ,
		{ 2,1,2,3,4 } ,
		{ 4,4,4,4,4 }};
	static Uint1 rettable2 [5][5] = {	  /* for developing return values */
		{ 0,1,4,1,4 } ,					  /* when b is longer than a */
		{ 1,1,4,1,4 } ,
		{ 4,4,2,2,4 } ,
		{ 1,1,4,3,4 } ,
		{ 4,4,4,4,4 }};

    if ((a == NULL) || (b == NULL))
        return retval;

    switch (a->choice)
    {
        case SEQLOC_MIX:    /* mix -- more than one seq */
        case SEQLOC_EQUIV:    /* equiv -- ditto */
        case SEQLOC_PACKED_INT:    /* packed int */
			if ((b->choice == SEQLOC_MIX) ||  /* check for identity */
				(b->choice == SEQLOC_EQUIV) ||
				(b->choice == SEQLOC_PACKED_INT))
			{
				got_one = FALSE;   /* for any overlap */
				slp = (SeqLocPtr)a->data.ptrvalue;  /* check for identity */
				slp2 = (SeqLocPtr)b->data.ptrvalue;
				retval = SeqLocCompare(slp, slp2);
				slp = slp->next;
				slp2 = slp2->next;
				while ((slp != NULL) && (slp2 != NULL) && (retval == SLC_A_EQ_B))
				{
					retval = SeqLocCompare(slp, slp2);
					slp = slp->next;
					slp2 = slp2->next;
				}
				if ((slp == NULL) && (slp2 == NULL) && (retval == SLC_A_EQ_B))
					return retval;

				slp = (SeqLocPtr)a->data.ptrvalue;	/* check for a in b */
				slp2 = (SeqLocPtr)b->data.ptrvalue;
				while ((slp != NULL) && (slp2 != NULL))
				{
					retval2 = SeqLocCompare(slp, slp2);
					if (retval2 > SLC_NO_MATCH)
						got_one = TRUE;
					switch (retval2)
					{
						case SLC_NO_MATCH:
							slp2 = slp2->next;
							break;
						case SLC_A_EQ_B:
							slp2 = slp2->next;
							slp = slp->next;
							break;
						case SLC_A_IN_B:
							slp = slp->next;
							break;
						case SLC_B_IN_A:
						case SLC_A_OVERLAP_B:
							slp2 = NULL;
							break;
					}
				}
				if (slp == NULL)    /* a all in b */
					return SLC_A_IN_B;

				slp2 = (SeqLocPtr)a->data.ptrvalue;	/* check for b in a */
				slp = (SeqLocPtr)b->data.ptrvalue;
				while ((slp != NULL) && (slp2 != NULL))
				{
					retval2 = SeqLocCompare(slp, slp2);
					if (retval2 > SLC_NO_MATCH)
						got_one = TRUE;
					switch (retval2)
					{
						case SLC_NO_MATCH:
							slp2 = slp2->next;
							break;
						case SLC_A_EQ_B:
							slp2 = slp2->next;
							slp = slp->next;
							break;
						case SLC_A_IN_B:
							slp = slp->next;
							break;
						case SLC_B_IN_A:
						case SLC_A_OVERLAP_B:
							slp2 = NULL;
							break;
					}
				}
				if (slp == NULL)    /* a all in b */
					return SLC_B_IN_A;

				if (got_one)
					return SLC_A_OVERLAP_B;
			}

			slp = (SeqLocPtr)a->data.ptrvalue; /* check for any overlap */
			retval = SeqLocCompare(slp, b);
			slp = slp->next;
			while (slp != NULL)
			{
				retval2 = SeqLocCompare(slp, b);
				retval = (Int2) rettable[retval][retval2];
				slp = slp->next;
			}
            return retval;
		default:
			break;
	}
    switch (b->choice)
    {
        case SEQLOC_MIX:    /* mix -- more than one seq */
		case SEQLOC_EQUIV:    /* equiv -- ditto */
        case SEQLOC_PACKED_INT:    /* packed int */
			slp = (SeqLocPtr)b->data.ptrvalue;
			retval = SeqLocCompare(a, slp);
			slp = slp->next;
			while (slp != NULL)
			{
				retval2 = SeqLocCompare(a, slp);
				retval = (Int2)rettable2[retval][retval2];
				slp = slp->next;
			}
		    return retval;
		default:
			break;
	}

	tmp.next = NULL;
	switch (a->choice)
	{
        case SEQLOC_NULL:    /* NULL, can't match */
			if (b->choice == SEQLOC_NULL)
				retval = SLC_A_EQ_B;
			break;
        case SEQLOC_FEAT:   /* feat -- can't track yet */
			break;
        case SEQLOC_EMPTY:    /* empty */
			if (b->choice == SEQLOC_EMPTY)
			{
				if (SeqIdForSameBioseq((SeqIdPtr)a->data.ptrvalue, (SeqIdPtr)b->data.ptrvalue))
					retval = SLC_A_EQ_B;
			}
			break;
        case SEQLOC_BOND:   /* bond -- 2 seqs */
			sbp = (SeqBondPtr)a->data.ptrvalue;
			tmp.choice = SEQLOC_PNT;    /* check the points */
			tmp.data.ptrvalue = (Pointer)sbp->a;
			retval = SeqLocCompare(&tmp, b);
			if (sbp->b != NULL)
			{
				tmp.data.ptrvalue = (Pointer)sbp->b;
				retval2 = SeqLocCompare(&tmp, b);
				retval = (Int2) rettable[retval][retval2];
			}
			break;
        case SEQLOC_WHOLE:    /* whole */
			sidp = (SeqIdPtr)a->data.ptrvalue;
			switch (b->choice)
			{
		        case SEQLOC_BOND:   /* bond -- 2 seqs */
					sbp = (SeqBondPtr)b->data.ptrvalue;
					if (SeqIdForSameBioseq(sbp->a->id, sidp))
						retval = SLC_B_IN_A;
					if (sbp->b != NULL)
					{
						if (SeqIdForSameBioseq(sbp->b->id, sidp))
							retval2 = SLC_B_IN_A;
						retval = (Int2) rettable[retval][retval2];
					}
					break;
        		case SEQLOC_WHOLE:    /* whole */
					if (SeqIdForSameBioseq(sidp, (SeqIdPtr)b->data.ptrvalue))
						retval = SLC_A_EQ_B;
					break;
        		case SEQLOC_INT:    /* int */
					sip = (SeqIntPtr)b->data.ptrvalue;
					if (SeqIdForSameBioseq(sidp, sip->id))
					{
						retval = SLC_B_IN_A;
            			bsp = BioseqFind(sidp);
						if (bsp != NULL)
						{
						    len = BioseqGetLen(bsp);
							if ((sip->from == 0) && (sip->to == (len - 1)))
								retval = SLC_A_EQ_B;
						}
					}
		            break;
        		case SEQLOC_PNT:    /* pnt */
					if (SeqIdForSameBioseq(sidp, ((SeqPntPtr)b->data.ptrvalue)->id))
						retval = SLC_B_IN_A;
		            break;
        		case SEQLOC_PACKED_PNT:    /* packed pnt */
					if (SeqIdForSameBioseq(sidp, ((PackSeqPntPtr)b->data.ptrvalue)->id))
						retval = SLC_B_IN_A;
		            break;
        		default:
		            break;
			}
			break;
        case SEQLOC_INT:    /* int */
			sip = (SeqIntPtr)a->data.ptrvalue;
			sidp = sip->id;
			switch (b->choice)
			{
		        case SEQLOC_BOND:   /* bond -- 2 seqs */
					sbp = (SeqBondPtr)b->data.ptrvalue;
					if (SeqIdForSameBioseq(sbp->a->id, sidp))
					{
						if ((sip->from <= sbp->a->point) &&
							(sip->to >= sbp->a->point))
							retval = SLC_B_IN_A;
					}
					if (sbp->b != NULL)
					{
						if (SeqIdForSameBioseq(sbp->b->id, sidp))
						{
							if ((sip->from <= sbp->b->point) &&
								(sip->to >= sbp->b->point))
								retval = SLC_B_IN_A;
						}
						retval = (Int2) rettable[retval][retval2];
					}
					break;
        		case SEQLOC_WHOLE:    /* whole */
					if (SeqIdForSameBioseq(sidp, (SeqIdPtr)b->data.ptrvalue))
					{
						retval = SLC_A_IN_B;
						bsp = BioseqFind((SeqIdPtr)b->data.ptrvalue);
						if (bsp != NULL)
						{
							len = BioseqGetLen(bsp);
							if ((sip->from == 0) && (sip->to == (len - 1)))
								retval = SLC_A_EQ_B;
						}
					}
					break;
        		case SEQLOC_INT:    /* int */
					sip2 = (SeqIntPtr)b->data.ptrvalue;
					if (SeqIdForSameBioseq(sidp, sip2->id))
					{
						if ((sip->from == sip2->from) && (sip->to == sip2->to))
							retval = SLC_A_EQ_B;
						else if ((sip->from <= sip2->from) && (sip->to >= sip2->to))
							retval = SLC_B_IN_A;
						else if ((sip->from >= sip2->from) && (sip->to <= sip2->to))
							retval = SLC_A_IN_B;
						else if ((sip->from >= sip2->from) && (sip->from <= sip2->to))
							retval = SLC_A_OVERLAP_B;
						else if ((sip->to >= sip2->from) && (sip->to <= sip2->to))
							retval = SLC_A_OVERLAP_B;
					}
		            break;
        		case SEQLOC_PNT:    /* pnt */
					if (SeqIdForSameBioseq(sidp, ((SeqPntPtr)b->data.ptrvalue)->id))
					{
						point = ((SeqPntPtr)b->data.ptrvalue)->point;
						if ((point >= sip->from) && (point <= sip->to))
							retval = SLC_B_IN_A;
					}
		            break;
        		case SEQLOC_PACKED_PNT:    /* packed pnt */
					pspp = (PackSeqPntPtr)b->data.ptrvalue;
					if (SeqIdForSameBioseq(sidp, pspp->id))
					{
						got_one = FALSE;
						missed_one = FALSE;
						num = PackSeqPntNum(pspp);
						for (i = 0; i < num; i++)
						{
							point = PackSeqPntGet(pspp, i);
							if ((point < sip->from) || (point > sip->to))
							{
								missed_one = TRUE;
								if (got_one)
									i = num;
							}
							else
							{
								got_one = TRUE;
								if (missed_one)
									i = num;
							}
						}
						if (got_one)
						{
							if (missed_one)
								retval = SLC_A_OVERLAP_B;
							else
								retval = SLC_B_IN_A;
						}
					}
		            break;
        		default:
		            break;
			}
            break;
        case SEQLOC_PNT:    /* pnt */
			sidp = ((SeqPntPtr)a->data.ptrvalue)->id;
			point = ((SeqPntPtr)a->data.ptrvalue)->point;
			switch (b->choice)
			{
		        case SEQLOC_BOND:   /* bond -- 2 seqs */
					sbp = (SeqBondPtr)b->data.ptrvalue;
					if (SeqIdForSameBioseq(sbp->a->id, sidp))
					{
						if (point == sbp->a->point)
							retval = SLC_A_EQ_B;
					}
					if (sbp->b != NULL)
					{
						if (SeqIdForSameBioseq(sbp->b->id, sidp))
						{
							if (point == sbp->b->point)
								retval = SLC_A_EQ_B;
						}
						retval = (Int2) rettable[retval][retval2];
					}
					break;
        		case SEQLOC_WHOLE:    /* whole */
					if (SeqIdForSameBioseq(sidp, (SeqIdPtr)b->data.ptrvalue))
						retval = SLC_A_IN_B;
					break;
        		case SEQLOC_INT:    /* int */
					sip2 = (SeqIntPtr)b->data.ptrvalue;
					if (SeqIdForSameBioseq(sidp, sip2->id))
					{
						if ((point == sip2->from) && (point == sip2->to))
							retval = SLC_A_EQ_B;
						else if ((point >= sip2->from) && (point <= sip2->to))
							retval = SLC_A_IN_B;
					}
		            break;
        		case SEQLOC_PNT:    /* pnt */
					if (SeqIdForSameBioseq(sidp, ((SeqPntPtr)b->data.ptrvalue)->id))
					{
						if (point == ((SeqPntPtr)b->data.ptrvalue)->point)
							retval = SLC_A_EQ_B;
					}
		            break;
        		case SEQLOC_PACKED_PNT:    /* packed pnt */
					pspp = (PackSeqPntPtr)b->data.ptrvalue;
					if (SeqIdForSameBioseq(sidp, pspp->id))
					{
						num = PackSeqPntNum(pspp);
						for (i = 0; i < num; i++)
						{
							if (point == PackSeqPntGet(pspp, i))
							{
								retval = SLC_A_IN_B;
								i = num;     /* only check one */
							}
						}
					}
		            break;
        		default:
		            break;
			}
            break;
        case SEQLOC_PACKED_PNT:    /* packed pnt */
			pspp = (PackSeqPntPtr)a->data.ptrvalue;
			num = PackSeqPntNum(pspp);
			sidp = pspp->id;
			switch (b->choice)
			{
		        case SEQLOC_BOND:   /* bond -- 2 seqs */
					sbp = (SeqBondPtr)b->data.ptrvalue;
					if (SeqIdForSameBioseq(sbp->a->id, sidp))
					{
						point = sbp->a->point;
						for (i = 0; i < num; i++)
						{
							if (point == PackSeqPntGet(pspp, i))
							{
								retval = SLC_B_IN_A;
								i = num;
							}
						}
					}
					if (sbp->b != NULL)
					{
						if (SeqIdForSameBioseq(sbp->b->id, sidp))
						{
							point = sbp->b->point;
							for (i = 0; i < num; i++)
							{
								if (point == PackSeqPntGet(pspp, i))
								{
									if (retval != SLC_B_IN_A)
										retval = SLC_A_OVERLAP_B;
									i = num + 1;
								}
							}
							if ((i != num) && (retval == SLC_B_IN_A))
								retval = SLC_A_OVERLAP_B;
						}
					}
					break;
        		case SEQLOC_WHOLE:    /* whole */
					if (SeqIdForSameBioseq(sidp, (SeqIdPtr)b->data.ptrvalue))
						retval = SLC_A_IN_B;
					break;
        		case SEQLOC_INT:    /* int */
					sip = (SeqIntPtr)b->data.ptrvalue;
					if (SeqIdForSameBioseq(sidp, sip->id))
					{
						got_one = FALSE;
						missed_one = FALSE;
						for (i = 0; i < num; i++)
						{
							point = PackSeqPntGet(pspp, i);
							if ((point < sip->from) || (point > sip->to))
							{
								missed_one = TRUE;
								if (got_one)
									i = num + 1;
							}
							else
							{
								got_one = TRUE;
								if (missed_one)
									i = num + 1;
							}
						}
						if (got_one)
						{
							if (missed_one)
								retval = SLC_A_OVERLAP_B;
							else
								retval = SLC_A_IN_B;
						}
					}
		            break;
        		case SEQLOC_PNT:    /* pnt */
					if (SeqIdForSameBioseq(sidp, ((SeqPntPtr)b->data.ptrvalue)->id))
					{
						point = ((SeqPntPtr)b->data.ptrvalue)->point;
						for (i = 0; i < num; i++)
						{
							if (point == PackSeqPntGet(pspp, i))
							{
								retval = SLC_B_IN_A;
								i = num + 1;
							}
						}
					}
		            break;
        		case SEQLOC_PACKED_PNT:    /* packed pnt */
					pspp2 = (PackSeqPntPtr)b->data.ptrvalue;
					if (SeqIdForSameBioseq(sidp, pspp->id))
					{
						num2 = PackSeqPntNum(pspp2);
						if (num == num2)   /* check for identity */
						{
							for (i = 0; i < num; i++)
							{
								if ( PackSeqPntGet(pspp, i) !=
									 PackSeqPntGet(pspp2, i))
									i = num + 1;
							}
							if (i == num)
								retval = SLC_A_EQ_B;
						}
						if (retval != SLC_A_EQ_B)
						{
							hits = 0;
							for (i = 0; i < num; i++)
							{
								point = PackSeqPntGet(pspp, i);
								for (j = 0; j < num2; j++)
								{
									if (point == PackSeqPntGet(pspp2, j))
										hits++;
								}
							}
							if (hits == num)
								retval = SLC_A_IN_B;
							else if (hits == num2)
								retval = SLC_B_IN_A;
						}
					}
		            break;
        		default:
		            break;
			}
            break;
        default:
            break;
    }
    return retval;
}

/*****************************************************************************
*
*   Boolean SeqIntCheck(sip)
*       checks that a seq interval is valid
*
*****************************************************************************/
Boolean SeqIntCheck (SeqIntPtr sip)

{
    Int4 len = INT4_MAX;
    BioseqPtr bsp;

	if (sip == NULL) return TRUE;  /* makes it ok to pass a NULL */
	
    bsp = BioseqFind(sip->id);
    if (bsp != NULL)
        len = BioseqGetLen(bsp);

    if ((sip->from < 0) || (sip->from > sip->to) || (sip->to >= len))
    {
        return FALSE;
    }
    else
        return TRUE;
}

/*****************************************************************************
*
*   Boolean SeqPntCheck(SeqPntPtr spp)
*       checks that a seq point is valid
*
*****************************************************************************/
Boolean SeqPntCheck (SeqPntPtr spp)

{
    Int4 len = INT4_MAX;
    BioseqPtr bsp;

	if (spp == NULL) return TRUE;   /* cant compare */
	
    bsp = BioseqFind(spp->id);
    if (bsp != NULL)
        len = BioseqGetLen(bsp);

    if ((spp->point < 0) || (spp->point >= len))
    {
        return FALSE;
    }
    else
        return TRUE;
}

/*****************************************************************************
*
*   PackSeqPntCheck (pspp)
*
*****************************************************************************/
Boolean PackSeqPntCheck (PackSeqPntPtr pspp)
{
	Int4 len = INT4_MAX;
	BioseqPtr bsp;
	Int4 num, index, point;

	if (pspp == NULL) return TRUE;   /* cant compare */
	
    bsp = BioseqFind(pspp->id);
    if (bsp != NULL)
        len = BioseqGetLen(bsp);

	num = PackSeqPntNum(pspp);   /* total number of points */
    for (index = 0; index < num; index++)
    {
        point = PackSeqPntGet(pspp, index);

	    if ((point < 0) || (point >= len))
        	return FALSE;
	}

    return TRUE;

}


/*****************************************************************************
*
*   SeqLocCheck (slp)
*
*****************************************************************************/
Boolean SeqLocCheck (SeqLocPtr slp)
{
	SeqLocPtr tmp;

	if (slp == NULL) return TRUE;

	tmp = NULL;
	while ((tmp = SeqLocFindNext(slp, tmp)) != NULL)
	{
		switch (tmp->choice)
		{
			case SEQLOC_INT:
				if (! SeqIntCheck ((SeqIntPtr)(tmp->data.ptrvalue)))
					return FALSE;
				break;
			case SEQLOC_PNT:
				if (! SeqPntCheck ((SeqPntPtr)(tmp->data.ptrvalue)))
					return FALSE;
				break;
			case SEQLOC_PACKED_PNT:
				if (! PackSeqPntCheck ((PackSeqPntPtr)(tmp->data.ptrvalue)))
					return FALSE;
				break;
			default:
				break;
		}
	}

	return TRUE;
}


/*****************************************************************************
*
*   SeqLocPartialCheck(head)
*       sets bits for incomplete location and/or errors
*       incomplete defined as Int-fuzz on start or stop with
*         lim.unk, lim.gt, or lim.lt set
*   
*   returns defined in header file
*   
*****************************************************************************/
Uint2 SeqLocPartialCheck(SeqLocPtr head)
{
	SeqLocPtr slp = NULL, first = NULL, last = NULL;
	Uint2 retval = 0;
	BioseqPtr bsp;
	SeqIntPtr sip;
	SeqPntPtr spp;
	PackSeqPntPtr pspp;
	IntFuzzPtr ifp;
	Boolean miss_end;
	BioseqContextPtr bcp;
	ValNodePtr vnp, vnp2;

	if (head == NULL) return retval;

	while ((slp = SeqLocFindNext(head, slp)) != NULL)
	{
		if (first == NULL)
			first = slp;
		last = slp;
	}

	if (first == NULL) return retval;

	slp = NULL;
	while ((slp = SeqLocFindNext(head, slp)) != NULL)
	{
		switch (slp->choice)
		{
			case SEQLOC_NULL:
				if (slp == first)
					retval |= SLP_START;
				else if (slp == last)
					retval |= SLP_STOP;
				else
					retval |= SLP_INTERNAL;
				break;
			case SEQLOC_INT:
				sip = (SeqIntPtr)(slp->data.ptrvalue);
				ifp = sip->if_from;
				if (ifp != NULL)
				{
					if (ifp->choice == 4)  /* lim */
					{
						if (ifp->a == 1)       /* gt */
							retval |= SLP_LIM_WRONG;
						else if ((ifp->a == 2) || (ifp->a == 0)) /* lt,unk */
						{
							if (sip->strand == Seq_strand_minus) /* stop */
							{
								if (slp == last)
									retval |= SLP_STOP;
								else
									retval |= SLP_INTERNAL;
								if (sip->from != 0)
								{
									if (slp == last)
										retval |= SLP_NOSTOP;
									else
										retval |= SLP_NOINTERNAL;
								}
							}
							else                                /* start */
							{
								if (slp == first)
									retval |= SLP_START;
								else
									retval |= SLP_INTERNAL;
								if (sip->from != 0)
								{
									if (slp == first)
										retval |= SLP_NOSTART;
									else
										retval |= SLP_NOINTERNAL;
								}
							}
						}
					}

				}
				ifp = sip->if_to;
				if (ifp != NULL)
				{
					if (ifp->choice == 4)  /* lim */
					{
						if (ifp->a == 2)       /* lt */
							retval |= SLP_LIM_WRONG;
						else if ((ifp->a == 1) || (ifp->a == 0)) /* gt,unk */
						{
							bsp = BioseqFind(sip->id);
							miss_end = FALSE;
							if (bsp != NULL)
							{
								if (sip->to != (bsp->length - 1))
									miss_end = TRUE;
							}

							if (sip->strand == Seq_strand_minus) /* start */
							{
								if (slp == first)
									retval |= SLP_START;
								else
									retval |= SLP_INTERNAL;
								if (miss_end)
								{
									if (slp == last)
										retval |= SLP_NOSTART;
									else
										retval |= SLP_NOINTERNAL;
								}
							}
							else                                /* stop */
							{
								if (slp == last)
									retval |= SLP_STOP;
								else
									retval |= SLP_INTERNAL;
								if (miss_end)
								{
									if (slp == last)
										retval |= SLP_NOSTOP;
									else
										retval |= SLP_NOINTERNAL;
								}
							}
						}
					}
				}
				break;
			case SEQLOC_PNT:
				spp = (SeqPntPtr)(slp->data.ptrvalue);
				ifp = spp->fuzz;
				if (ifp != NULL)
				{
					if (ifp->choice == 4)  /* lim */
					{
						if ((ifp->a >= 0) || (ifp->a <= 2))  /* gt, lt,unk */
						{
							if (slp == first)
								retval |= SLP_START;
							if (slp == last)
								retval |= SLP_STOP;
							if ((slp != first) && (slp != last))
								retval |= SLP_INTERNAL;
						}
					}
				}
				break;
			case SEQLOC_PACKED_PNT:
				pspp = (PackSeqPntPtr)(slp->data.ptrvalue);
				ifp = pspp->fuzz;
				if (ifp != NULL)
				{
					if (ifp->choice == 4)  /* lim */
					{
						if ((ifp->a >= 0) || (ifp->a <= 2)) /* gt, lt, unk */
						{
							if (slp == first)
								retval |= SLP_START;
							if (slp == last)
								retval |= SLP_STOP;
							if ((slp != first) && (slp != last))
								retval |= SLP_INTERNAL;
						}
					}
				}
				break;
			case SEQLOC_WHOLE:
				bsp = BioseqFind((SeqIdPtr)(slp->data.ptrvalue));
				if (bsp == NULL) break;
				bcp = BioseqContextNew(bsp);
				if (bcp == NULL) break;
				vnp = NULL;
				while ((vnp = BioseqContextGetSeqDescr(bcp, Seq_descr_modif, vnp, NULL)) != NULL)
				{
					for (vnp2 = (ValNodePtr)(vnp->data.ptrvalue); vnp2 != NULL; vnp2 = vnp2->next)
					{
						switch (vnp2->data.intvalue)
						{
							case 16:	/* no left */
								if (slp == first)
									retval |= SLP_START;
								else
									retval |= SLP_INTERNAL;
								break;
							case 17:    /* no right */
								if (slp == last)
									retval |= SLP_STOP;
								else
									retval |= SLP_INTERNAL;
								break;
							case 10:    /* partial */
								retval |= SLP_OTHER;
								break;
						}
					}
				}
				BioseqContextFree(bcp);
				break;
			default:
				break;
			
		}
	}

	return retval;
}

/*****************************************************************************
*
*   StringForSeqMethod(Int2 method)
*   	returns a descriptive string for sequencing method.
*
*****************************************************************************/
CharPtr StringForSeqMethod (Int2 method)
{
#define MAX_METHOD 6
	static char * methods[MAX_METHOD] = {
		"conceptual translation",
		"direct peptide sequencing",
		"conceptual translation with partial peptide sequencing",
		"sequenced peptide, ordered by overlap",
		"sequenced peptide, ordered by homology",
		"conceptual translation supplied by author" };

	if ((method < 1) || (method > MAX_METHOD))
		return NULL;

	return methods[method - 1];
}

Boolean GetThePointForOffset PROTO((SeqLocPtr of, SeqPntPtr target, Uint1 which_end));
Int4 CheckOffsetInLoc PROTO((SeqLocPtr in, Int4 pos, BioseqPtr bsp, SeqIdPtr the_id));
Int4 CheckPointInBioseq PROTO((SeqPntPtr sp, BioseqPtr in));

/*****************************************************************************
*
* Int4 GetOffsetInLoc (SeqLocPtr of, SeqLocPtr in, Uint1 which_end)
*   returns -1 if of not in, in
*
*****************************************************************************/
Int4 GetOffsetInLoc (SeqLocPtr of, SeqLocPtr in, Uint1 which_end)
{
	SeqPnt sp;
	BioseqPtr bsp;

	if ((in == NULL) || (of == NULL))
		return -1L;

	if (! GetThePointForOffset(of, &sp, which_end))
		return -1L;

	if (! IS_one_loc(in))    /* optimize for multiple hits */
		bsp = BioseqFind(sp.id);
	else
		bsp = NULL;

	return CheckOffsetInLoc(in, sp.point, bsp, sp.id);
}

/*****************************************************************************
*
* Int4 GetOffsetInBioseq (SeqLocPtr of, BioseqPtr in, Uint1 which_end)
*   return -1 if of not in "in"
*
*****************************************************************************/
Int4 GetOffsetInBioseq (SeqLocPtr of, BioseqPtr in, Uint1 which_end)
{
	SeqPnt sp;

	if ((of == NULL) || (in == NULL))
		return -1;

	if (! GetThePointForOffset(of, &sp, which_end))
		return -1L;

	return CheckPointInBioseq(&sp, in);
}

/*****************************************************************************
*
*   CheckPointInBioseq(pnt, in)
*
*****************************************************************************/
Int4 CheckPointInBioseq (SeqPntPtr sp, BioseqPtr in)
{
	ValNode sl;
	BioseqPtr bsp;
	Int4 retval;
	SeqLocPtr slp, curr;
	Int4 offset, offset2, strt, stp;
	SeqIdPtr sip;

	if (SeqIdIn(sp->id, in->id))   /* in this one */
		return sp->point;

	switch (in->repr)
	{
		case Seq_repr_virtual:
		case Seq_repr_raw:
		case Seq_repr_const:
		case Seq_repr_map:
			return -1;    /* nothing more can be done */

		case Seq_repr_ref:
			slp = in->seq_ext;
			break;

		case Seq_repr_seg:
			sl.choice = SEQLOC_MIX;
			sl.data.ptrvalue = in->seq_ext;
			slp = &sl;
			break;
		default:
			return -1;
	}

	bsp = BioseqFind(sp->id);
	retval = CheckOffsetInLoc(slp, sp->point, bsp, sp->id);

	if (retval >= 0) return retval;     /* got it on segments */

	                                    /* look for segmented segments */
	offset = 0;
	curr = NULL;
	while ((curr = SeqLocFindNext(slp, curr)) != NULL)
	{
		sip = SeqLocId(curr);
		if (sip != NULL)
		{
			bsp = BioseqFind(sip);
			if (bsp != NULL)
			{
				switch (bsp->repr)
				{
					case Seq_repr_ref:   /* could have more levels */
					case Seq_repr_seg:
						offset2 = CheckPointInBioseq(sp, bsp);
						if (offset2 >= 0)   /* got it */
						{
							strt = SeqLocStart(curr);
							stp = SeqLocStop(curr);
							if ((offset2 >= strt) && (offset2 <= stp))
							{
								if (SeqLocStrand(curr) == Seq_strand_minus)
									offset2 = stp - offset2;
								else
									offset2 -= strt;
								retval = offset2 + offset;
								return retval;
							}
						}
						break;
					default:		   /* one level, already checked */
						break;
				}
			}
		}
		offset += SeqLocLen(curr);
	}
	
	return retval;    /* all failed */
}

/*****************************************************************************
*
*   Boolean GetThePointForOffset(SeqLocPtr of, SeqPntPtr target, Uint1 which_end)
*
*****************************************************************************/
Boolean GetThePointForOffset(SeqLocPtr of, SeqPntPtr target, Uint1 which_end)
{
	SeqLocPtr tmp, pnt, first=NULL, last=NULL;
	Uint1 ofstrand;
	Boolean getstart;

	pnt = NULL;    /* get first or last single span type in "of"*/
	tmp = NULL;
	while ((pnt = SeqLocFindNext(of, pnt)) != NULL)
	{
		last = pnt;
		if (first == NULL)
			first = pnt;
	}				   /* otherwise, get last */
	if (first == NULL)
		return FALSE;
	ofstrand = SeqLocStrand(first);
	getstart = TRUE;   /* assume we are getting SeqLocStart() */
	switch (which_end)
	{
		case SEQLOC_LEFT_END:
			if (ofstrand == Seq_strand_minus)
				tmp = last;
			else
				tmp = first;
			break;
		case SEQLOC_RIGHT_END:
			if (ofstrand == Seq_strand_minus)
				tmp = first;
			else
				tmp = last;
			getstart = FALSE;
			break;
		case SEQLOC_START:
			tmp = first;
			if (ofstrand == Seq_strand_minus)
				getstart = FALSE;
			break;
		case SEQLOC_STOP:
			tmp = last;
			if (ofstrand != Seq_strand_minus)
				getstart = FALSE;
			break;
		default:
			return FALSE;   /* error */
	}


	if (getstart)
		target->point = SeqLocStart(tmp);
	else
		target->point = SeqLocStop(tmp);
	target->id = SeqLocId(tmp);

	if ((target->point < 0) || (target->id == NULL))
		return FALSE;

	return TRUE;
}

/*****************************************************************************
*
*   CheckOffsetInLoc()
*
*****************************************************************************/
Int4 CheckOffsetInLoc(SeqLocPtr in, Int4 pos, BioseqPtr bsp, SeqIdPtr the_id)
{
	SeqIdPtr tsip, sip;
	SeqLocPtr tmp;
	SeqIntPtr sipp;
	Boolean checkin, doit;
	Int4 ctr = 0, len;

	if (bsp != NULL)
	{
		tsip = bsp->id;
		checkin = 1;
	}
	else
	{
		tsip = the_id;
		checkin = 0;
	}

	tmp = NULL;
	while ((tmp = SeqLocFindNext(in, tmp)) != NULL)
	{
		sip = SeqLocId(tmp);
		if (checkin)    /* optimizer */
			doit = SeqIdIn(sip, tsip);
		else
			doit = SeqIdForSameBioseq(sip, tsip);
		switch (tmp->choice)
		{
			case SEQLOC_PNT:
				if (doit)
				{
					if (pos == ((SeqPntPtr)(tmp->data.ptrvalue))->point)
						return ctr;
				}
				ctr++;
				break;
			case SEQLOC_INT:
				sipp = (SeqIntPtr)(tmp->data.ptrvalue);
				if (doit)
				{
					if ((pos >= sipp->from) && (pos <= sipp->to))
					{
						if (sipp->strand == Seq_strand_minus)
							ctr += (sipp->to - pos);
						else
							ctr += (pos - sipp->from);
						return ctr;
					}
				}
				ctr += (sipp->to - sipp->from + 1);
				break;
			case SEQLOC_WHOLE:
				if (doit)
				{
					ctr += pos;
					return ctr;
				}
			default:
				len = SeqLocLen(tmp);
				if (len > 0) ctr += len;
				break;
		}
	}

	return -1;    /* failed */
}


/*****************************************************************************
*
*   Int2 SeqLocOrder PROTO((SeqLocPtr a, SeqLocPtr b, BioseqPtr in));
*   	This function is used to sort SeqLocs into ascending order by
*   location on a Bioseq (segmented or otherwise)
*   	The first position is the point sorted on.
*   Returns
*   	0   a and b start at same offset
*   	1   a > b
*   	-1  a < b
*   	-2  a in bsp, b not
*       2   b in bsp, a not
*       3   neither a nor b in bsp
*        This function will attempt to sort locs not in bsp to the end of
*   the list.  Values -2,2,3 can also be used to detect error conditions.
*
*****************************************************************************/
Int2 SeqLocOrder (SeqLocPtr a, SeqLocPtr b, BioseqPtr in)
{
	Int4 aoffset, boffset;


	if ((a == NULL) || (b == NULL) || (in == NULL))
		return 3;

	aoffset = GetOffsetInBioseq(a, in, SEQLOC_LEFT_END);
	boffset = GetOffsetInBioseq(b, in, SEQLOC_LEFT_END);

	if ((aoffset == -1) && (boffset >= 0))
		return 2;
	else if ((aoffset >= 0) && (boffset == -1))
		return -2;
	else if ((aoffset == -1) && (boffset == -1))
		return 3;
	else if (aoffset == boffset)
		return 0;
	else if (aoffset < boffset)
		return -1;
	else
		return 1;
}

/*****************************************************************************
*
*   Int2 SeqLocMol(seqloc)
*   	returns Seq-inst.mol for all Bioseqs this seqloc points to.
*   	if all Seq-inst.mol the same, returns that.
*   	if mixed dna,rna, returns na
*   	if mixed na,aa or can't find any Bioseq, or bsp->mol = 0, or 255
*   			returns 0
*
*****************************************************************************/
Int2 SeqLocMol (SeqLocPtr seqloc)
{
	SeqLocPtr slp = NULL;
	SeqIdPtr sip;
	static Uint1 cases[5][4] = {
		{ 1,2,3,4 } ,    /* was 0, not-set */
		{ 1,4,0,4 } ,    /* was 1, dna */
		{ 4,2,0,4 } ,    /* was 2, rna */
		{ 0,0,3,0 } ,    /* was 3, aa */
		{ 4,4,0,4 }};    /* was 4, na */
	Int2 the_mol = 0, tmp;
	BioseqPtr bsp;

	while ((slp = SeqLocFindNext(seqloc, slp)) != NULL)
	{
		sip = SeqLocId(slp);
		if (sip != NULL)
		{
			bsp = BioseqFind(sip);
			if (bsp == NULL)
				return 0;
			tmp = (Int2)bsp->mol;
			if ((tmp == 0) || (tmp == Seq_mol_other))
				return 0;
			the_mol = (Int2)cases[the_mol][tmp-1];
			if (! the_mol)
				return 0;
		}
	}
	return the_mol;
}

static SeqIdPtr SeqLocPrintProc PROTO((SeqLocPtr slp, ByteStorePtr bsp, Boolean first, SeqIdPtr lastid));
void BSstring PROTO((ByteStorePtr bsp, CharPtr str));

/*****************************************************************************
*
*   SeqLocPrint(slp)
*
*****************************************************************************/
CharPtr SeqLocPrint(SeqLocPtr slp)
{
	ByteStorePtr bsp;
	CharPtr str;
	SeqLocPtr tmp;

	if (slp == NULL) return NULL;

	bsp = BSNew(80);

	tmp = slp->next;    /* save possible chain */
	slp->next = NULL;   /* take out of possible chain */

	SeqLocPrintProc(slp, bsp, TRUE, NULL);

	slp->next = tmp;    /* replace possible chain */
	str = BSMerge(bsp, NULL);
	BSFree(bsp);

	return str;
}

SeqIdPtr SeqPointPrint PROTO((SeqPntPtr spp, CharPtr buf, SeqIdPtr lastid));
void IntFuzzPrint PROTO((IntFuzzPtr ifp, Int4 pos, CharPtr buf, Boolean right));
static char strandsymbol[5] = { '\0', '\0', 'c', 'b', 'r' };

/*****************************************************************************
*
*   SeqLocPrintProc(slp, bsp, first, lastid)
*   	print traversal routine
*   	goes down slp chain
*
*****************************************************************************/
static SeqIdPtr SeqLocPrintProc(SeqLocPtr slp, ByteStorePtr bsp, Boolean first, SeqIdPtr lastid)
{
	Char buf[40];
	SeqBondPtr sbp;
	PackSeqPntPtr pspp;
	SeqIntPtr sip;
	IntFuzzPtr ifp1, ifp2;
	Int4 from, to;
	Int2 delim, delim2;

	while (slp != NULL)
	{
		if (! first)
		{
			BSPutByte(bsp, ',');
			BSPutByte(bsp, ' ');
		}
		first = FALSE;

		delim = 0;
	    switch (slp->choice)
    	{
        	case SEQLOC_BOND:   /* bond -- 2 seqs */
				sbp = (SeqBondPtr)(slp->data.ptrvalue);
				if (sbp->a != NULL)
				{
					lastid = SeqPointPrint(sbp->a, buf, lastid);
					BSstring(bsp, buf);
				}
				else
					BSPutByte(bsp, '?');

				BSPutByte(bsp, '=');

				if (sbp->b != NULL)
				{
					lastid = SeqPointPrint(sbp->b, buf, lastid);
					BSstring(bsp, buf);
				}
				else
					BSPutByte(bsp, '?');
				break;
    	    case SEQLOC_FEAT:   /* feat -- can't track yet */
				BSstring(bsp, "(feat)");
				break;
	        case SEQLOC_NULL:    /* NULL */
				BSPutByte(bsp, '~');
				break;
	        case SEQLOC_EMPTY:    /* empty */
				BSPutByte(bsp, '{');
				SeqIdPrint((SeqIdPtr)(slp->data.ptrvalue), buf, PRINTID_FASTA_SHORT);
				BSstring(bsp, buf);
				BSPutByte(bsp, '}');
				break;
	        case SEQLOC_WHOLE:    /* whole */
				SeqIdPrint((SeqIdPtr)(slp->data.ptrvalue), buf, PRINTID_FASTA_SHORT);
				BSstring(bsp, buf);
				break;
	        case SEQLOC_MIX:    /* mix -- more than one seq */
    	    case SEQLOC_PACKED_INT:    /* packed int */
				delim = '(';
				delim2 = ')';
	        case SEQLOC_EQUIV:    /* equiv -- ditto */
				if (! delim)
				{
					delim = '[';
					delim2 = ']';
				}
				BSPutByte(bsp, delim);
				lastid = SeqLocPrintProc((SeqLocPtr)(slp->data.ptrvalue), bsp, TRUE, lastid);
				BSPutByte(bsp, delim2);
	            break;
    	    case SEQLOC_INT:    /* int */
				sip = (SeqIntPtr)(slp->data.ptrvalue);
				if (! SeqIdMatch(sip->id, lastid))
				{
					SeqIdPrint(sip->id, buf, PRINTID_FASTA_SHORT);
					BSstring(bsp, buf);
					BSPutByte(bsp, ':');
				}
				lastid = sip->id;
				if (strandsymbol[sip->strand])
					BSPutByte(bsp, (Int2)strandsymbol[sip->strand]);
				if ((sip->strand == Seq_strand_minus) ||
					(sip->strand == Seq_strand_both_rev))
				{
					ifp1 = sip->if_to;
					ifp2 = sip->if_from;
					to = sip->from;
					from = sip->to;
				}
				else
				{
					ifp1 = sip->if_from;
					ifp2 = sip->if_to;
					to = sip->to;
					from = sip->from;

				}
				IntFuzzPrint(ifp1, from, buf, FALSE);
				BSstring(bsp, buf);
				BSPutByte(bsp, '-');
				IntFuzzPrint(ifp2, to, buf, TRUE);
				BSstring(bsp, buf);

        	    break;
	        case SEQLOC_PNT:    /* pnt */
				lastid = SeqPointPrint((SeqPntPtr)(slp->data.ptrvalue), buf, lastid);
				BSstring(bsp, buf);
	            break;
    	    case SEQLOC_PACKED_PNT:    /* packed pnt */
				pspp = (PackSeqPntPtr)(slp->data.ptrvalue);
				if (pspp != NULL)
					BSstring(bsp, "PackSeqPnt");
	            break;
    	    default:
            	BSstring(bsp, "(\?\?)");
				break;
	    }
		slp = slp->next;
	}
	return lastid;
}


/*****************************************************************************
*
*   BSstring(bsp, str)
*
*****************************************************************************/
void BSstring(ByteStorePtr bsp, CharPtr str)
{
	BSWrite(bsp, str, (Int4)(StringLen(str)));
	return;
}

/*****************************************************************************
*
*   SeqPointPrint(spp, buf, lastid)
*
*****************************************************************************/
SeqIdPtr SeqPointPrint(SeqPntPtr spp, CharPtr buf, SeqIdPtr lastid)
{
	CharPtr tmp;

	if ((spp == NULL) || (buf == NULL)) return NULL;
	
	tmp = buf;
	*tmp = '\0';
	if (! SeqIdMatch(spp->id, lastid))
	{
		SeqIdPrint(spp->id, tmp, PRINTID_FASTA_SHORT);
		while (*tmp != '\0') tmp++;
		*tmp = ':';
		tmp++; *tmp = '\0';
	}
	if (strandsymbol[spp->strand])
	{
		*tmp = strandsymbol[spp->strand];
		tmp++; *tmp = '\0';
	}
	IntFuzzPrint(spp->fuzz, spp->point, tmp, TRUE);
	
	return spp->id;
}

/*****************************************************************************
*
*   IntFuzzPrint(ifp, pos, buf, right)
*
*****************************************************************************/
void IntFuzzPrint(IntFuzzPtr ifp, Int4 pos, CharPtr buf, Boolean right)
{
	Char lim=0;
	CharPtr tmp;
	Char tbuf[40];

	if (buf == NULL) return;
	pos++;  /* number from 1 */
	tmp = buf;
	*tmp = '\0';
	*tbuf = '\0';
	if (ifp != NULL)
	{
		switch (ifp->choice)
		{
			case 1:     /* plus minus */
				sprintf(tbuf, "<+-%ld>", (long)ifp->a);
				break;
			case 2:     /* range */
				sprintf(tbuf, "<%ld.%ld>", (long)ifp->b, (long)ifp->a);
				break;
			case 3:     /* percent */
				sprintf(tbuf, "<%ld%%>", (long)ifp->a);
				break;
			case 4:     /* limit */
				switch (ifp->a)
				{
					case 0:    /* unknown */
					case 255:  /* other */
						sprintf(tbuf, "<?>");
						break;
					case 1:    /* gt */
						lim = '>';
						break;
					case 2:    /* lt */
						lim = '<';
						break;
					case 3:
						lim = 'r';
						break;
					case 4:
						lim = '^';
						break;
				}
				break;
		}
	}

	if ((lim) && (lim != 'r'))
	{
		*tmp = lim;
		tmp++; *tmp = '\0';
		lim = 0;
	}

	if (right)
	{
		sprintf(tmp, "%ld", (long)pos);
		while (*tmp != '\0') tmp++;
	}
	if (lim == 'r')
	{
		*tmp = '^';
		tmp++;
		*tmp = '\0';
	}
	if (tbuf[0] != '\0')
	{
		tmp = StringMove(tmp, tbuf);
	}
	if (! right)
		sprintf(tmp, "%ld", (long)pos);

	return;

}
/*****************************************************************************
*
*   TaxNameFromCommon(common)
*
*****************************************************************************/
typedef struct sturct_Nlm_taxcommon {
	char * common;
	char * taxname;
} Nlm_TaxCommon, PNTR Nlm_TaxCommonPtr;

CharPtr TaxNameFromCommon (CharPtr common)
{
	CharPtr taxname = NULL;
	CharPtr query = MemNew(StringLen(common) + 2);
	int try, dex;

	static Nlm_TaxCommon taxcommon[40] = {
	"Chinese hamsters", "Cricetulus sp."
	,"Syrian golden hamsters", "Mesocricetus auratus"
	,"Syrian hamsters", "Mesocricetus sp."
	,"barley", "Hordeum sp."
	,"carrots", "Daucus sp."
	,"cats", "Felis sp."
	,"cattles", "Bos sp."
	,"chickens", "Gallus sp."
	,"chimpanzees", "Pan sp."
	,"chimpanzes", "Pan sp."
	,"corn", "Zea sp."
	,"cucumber", "Cucumis sativus"
	,"dogs", "Canis sp."
	,"goats", "Capra sp."
	,"gorillas", "Gorilla sp."
	,"guinea pigs", "Cavia sp."
	,"hamsters", "Cricetidae gen. sp."
	,"horses", "Equus sp."
	,"humans", "Homo sapiens"
	,"maize", "Zea sp."
	,"mice", "Mus sp."
	,"mouse", "Mus sp."
	,"peas", "Pisum sp."
	,"potatoes", "Solanum sp."
	,"potato", "Solanum sp."
	,"quails", "Phasianidae gen. sp."
	,"rabbits", "Oryctolagus sp."
	,"rats", "Rattus sp."
	,"rices", "Oryza sp."
	,"sheeps", "Ovis sp."
	,"sorghums", "Sorghum sp."
	,"soybeans", "Glycine sp."
	,"spinach", "Spinacia sp."
	,"swine", "Sus sp."
	,"tobacco", "Nicotiania sp."
	,"tomatoes", "Lycopersicon sp."
	,"tomato", "Lycopersicon sp."
	,"turkeys", "Meleagris sp."
	,"wheat", "Triticum sp."
	,"zebrafish", "Brachydanio sp."
};

	if (common == NULL) return NULL;
	
	StringCpy(query,common);  /* space for 's' is at end */
	for (try = 0; try < 2; try ++){
		for (dex = 0; dex < 40; dex ++ ){
			if (StringICmp(query,taxcommon[dex].common) == 0)
				break;
		}
		if ( dex < 40)
			break;
		if (try == 0)
			StringCat(query,"s");
	}
	MemFree (query);
	if (dex < 40)
		taxname = StringSave (taxcommon[dex].taxname);

	return taxname;
}
