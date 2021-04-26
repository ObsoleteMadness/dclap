/*  tofasta.c
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
* File Name:  tofasta.c
*
* Author:  James Ostell
*   
* Version Creation Date: 7/12/91
*
* $Revision: 2.9 $
*
* File Description:  various sequence objects to fasta output
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#include <tofasta.h>

typedef struct tofasta {
    Boolean is_na;
    Boolean got_one;
    FILE * fp;
	Boolean group_segs;
	Int2 last_indent;
} FastaDat, PNTR FastaPtr;

static FastaDat tofasta;

/*****************************************************************************
*
*   Traversal routine for SeqEntryToFasta
*
*****************************************************************************/
static void SeqEntryFasta (SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent)

{
    FastaPtr tfa;
    BioseqPtr bsp;
    FILE * fp;
    Boolean is_na;

    if (! IS_Bioseq(sep))
        return;

    bsp = (BioseqPtr)sep->data.ptrvalue;
    tfa = (FastaPtr) data;
    fp = tfa->fp;
    is_na = tfa->is_na;

	if (tfa->last_indent != -1)   /* putting out segments together */
	{
		if (indent > tfa->last_indent)
			return;
		tfa->last_indent = -1;
	}

	if (! tfa->group_segs)
	{
		if (BioseqRawToFasta(bsp, fp, is_na))
			tfa->got_one = TRUE;
	}
	else
	{
		if (BioseqToFasta(bsp, fp, is_na))
		{
			tfa->got_one = TRUE;
			if (bsp->repr == Seq_repr_seg)
				tfa->last_indent = indent;
		}
	}
    return;
}


/*****************************************************************************
*
*   SeqEntryToFasta(sep, fp, is_na)
*
*****************************************************************************/
Boolean SeqEntryToFasta (SeqEntryPtr sep, FILE *fp, Boolean is_na)
{
	return SeqEntrysToFasta(sep, fp, is_na, FALSE);
}

/*****************************************************************************
*
*   SeqEntrysToFasta(sep, fp, is_na, group_segs)
*
*****************************************************************************/
Boolean SeqEntrysToFasta (SeqEntryPtr sep, FILE *fp, Boolean is_na, Boolean group_segs)

{
    FastaDat tfa;

	if ((sep == NULL) || (fp == NULL))
		return FALSE;

    tfa.fp = fp;
    tfa.is_na = is_na;
	tfa.group_segs = group_segs;
	tfa.last_indent = -1;
	tfa.got_one = FALSE;
    SeqEntryExplore(sep, (Pointer)&tfa, SeqEntryFasta);
	return tfa.got_one;
}

/*****************************************************************************
*
*   Boolean BioseqRawToFasta(bsp, fp, is_na)
*
*****************************************************************************/
Boolean BioseqRawToFasta (BioseqPtr bsp, FILE *fp, Boolean is_na)

{
    Uint1 repr;

    if ((bsp == NULL) || (fp == NULL))
        return FALSE;

    repr = Bioseq_repr(bsp);
    if (! ((repr == Seq_repr_raw) || (repr == Seq_repr_const)))
        return FALSE;

	return BioseqToFasta(bsp, fp, is_na);
}


/*****************************************************************************
*
*   Boolean BioseqToFasta(bsp, fp, is_na)
*
*****************************************************************************/
Boolean BioseqToFasta (BioseqPtr bsp, FILE *fp, Boolean is_na)

{
    SeqPortPtr spp;
	char idbuf[129];
	char seqbuf[61]; /* dimension of seqbuf is 1 more than output line length */
    Int2 i;
    Uint1 repr, code, residue;
    CharPtr tmp;
	BioseqContextPtr bcp;
	PdbBlockPtr pbp;
	ValNodePtr vnp;
	PatentSeqIdPtr psip;
	Int4 ctr;

    if ((bsp == NULL) || (fp == NULL))
        return FALSE;

    repr = Bioseq_repr(bsp);

    if (ISA_na(bsp->mol))
    {
        if (! is_na)
            return FALSE;
    }
    else if (is_na)
        return FALSE;

	bcp = BioseqContextNew(bsp);
    SeqIdPrint(bsp->id, idbuf, PRINTID_FASTA_LONG);
    fprintf(fp, ">%s ", idbuf);
	if ((tmp = BioseqContextGetTitle(bcp)) != NULL) {
	    fputs(tmp, fp);
		fputc('\n', fp);
	}
	else
		if ((vnp = BioseqContextGetSeqDescr(bcp, Seq_descr_pdb, NULL, NULL)) != NULL)
		{
		pbp = (PdbBlockPtr)(vnp->data.ptrvalue);
		fputs((CharPtr)(pbp->compound->data.ptrvalue), fp);
		fputc('\n', fp);
	}
	else
	{
		for (vnp = bsp->id; vnp != NULL; vnp = vnp->next)
		{
			if (vnp->choice == SEQID_PATENT)
			{
				psip = (PatentSeqIdPtr)(vnp->data.ptrvalue);
				fprintf(fp, "Sequence %d from Patent %s %s\n",
					(int)psip->seqid, psip->cit->country, psip->cit->number);
				break;
			}
		}
		if (vnp == NULL)
			fputs("No definition line found\n", fp);
	}
	
	BioseqContextFree(bcp);

    if (is_na)
        code = Seq_code_iupacna;
    else
        code = Seq_code_ncbieaa;

    spp = SeqPortNew(bsp, 0, -1, 0, code);
    SeqPortSeek(spp, 0, SEEK_SET);
    i = 0;
	ctr = 1;
    while ((residue = SeqPortGetResidue(spp)) != SEQPORT_EOF)
    {
		if (! IS_residue(residue))
		{
			if (residue == INVALID_RESIDUE)
			{
				if (is_na)
					residue = 'N';
				else
					residue = 'X';
				ErrPost(CTX_NCBIOBJ, 1, "ToFastA: Invalid residue at position %ld in %s",
					ctr, idbuf);
			}
			else
			{
				if (residue == SEQPORT_VIRT)  /* gap */
				{
	    	        seqbuf[i] = '\0';
    	    	    fprintf(fp, "%s\n-\n", seqbuf);
        	    	i = 0;
				}

				residue = '\0';
			}

		}

		if (residue != '\0')
		{
	        if (i == DIM(seqbuf)-1)
    	    {
        	    seqbuf[i] = '\0';
            	fprintf(fp, "%s\n", seqbuf);
	            i = 0;
    	    }
			
		    seqbuf[i] = residue;
    	    i++; ctr++;
		}
    }
    if (i)
    {
        seqbuf[i] = '\0';
        fprintf(fp, "%s\n", seqbuf);
    }
    SeqPortFree(spp);
    return TRUE;
}
