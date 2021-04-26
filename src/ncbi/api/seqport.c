/*  seqport.c
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
* File Name:  seqport.c
*
* Author:  James Ostell
*   
* Version Creation Date: 7/13/91
*
* $Revision: 2.22 $
*
* File Description:  Ports onto Bioseqs
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/
#include <seqport.h>

/*****************************************************************************
*
*   SeqPort Routines
*
*****************************************************************************/

/*****************************************************************************
*
*   SeqPortFree(spp)
*
*****************************************************************************/
SeqPortPtr SeqPortFree (SeqPortPtr spp)

{
    SeqPortPtr tspp, nextspp;

    if (spp == NULL)
        return NULL;

    tspp = spp->segs;
    while (tspp != NULL)
    {
        nextspp = tspp->next;
        SeqPortFree(tspp);
        tspp = nextspp;
    }

    return MemFree(spp);
}


/*****************************************************************************
*
*   SeqPortNew(bsp, start, stop, strand, newcode)
*       if bsp == NULL, creates an empty port
*       see objloc.h for strand defines
*
*****************************************************************************/
SeqPortPtr SeqPortNew (BioseqPtr bsp, Int4 start, Int4 stop, Uint1 strand, Uint1 newcode)

{
    SeqPortPtr spp, spps, sppcurr = NULL, spprev, prev, curr;
    Uint1 curr_code, repr, tstrand;
    SeqLocPtr the_segs, currseg;
    Int4 len, ctr, tlen, tfrom, tto, xfrom, xto, tstart, tstop;
	Char errbuf[40];
	ValNode fake;
	Boolean done, started;
	BioseqPtr tbsp;

    spp = MemNew(sizeof(SeqPort));

    if (bsp == NULL)     /* a NULL section */
        return spp;

    spp->bsp = bsp;					/* get ready for error msgs */
	SeqIdPrint(SeqIdFindBest(bsp->id, 0), errbuf, PRINTID_FASTA_SHORT);
    len = BioseqGetLen(bsp);
    if (start < 0)
    {
        ErrPost(CTX_NCBISEQPORT, 1  ,
				 "SeqPortNew: %s start (%ld)< 0", errbuf, start);
        return NULL;
    }
    if (start >= len)
    {
        ErrPost(CTX_NCBISEQPORT, 1  ,
				 "SeqPortNew: %s start(%ld) >= len(%ld)",
					errbuf, start, len);
        return NULL;
    }
    spp->start = start;
    if (stop == LAST_RESIDUE)
        stop = len - 1;
    else if (stop < start)
    {
        ErrPost(CTX_NCBISEQPORT, 1  ,
				 "SeqPortNew: %s stop(%ld) < start(%ld)",
					errbuf, stop, start);
        return NULL;
    }
    else if (stop >= len)
    {
        ErrPost(CTX_NCBISEQPORT, 1  ,
				 "SeqPortNew: %s stop(%ld) >= len(%ld)",
					errbuf, stop, len);
        return NULL;
    }
    spp->stop = stop;
    spp->strand = strand;
    spp->curpos = -1;    /* not set */
    spp->totlen = stop - start + 1;
    spp->newcode = newcode;
    spp->sctp = SeqCodeTableFind(newcode);

    spp->currnum = BioseqGetSeqDescr(bsp, Seq_descr_num, NULL);
    if (spp->currnum == NULL)   /* no numbering set */
        spp->currnum = NumberingDefaultGet();   /* use default */

    repr = Bioseq_repr(bsp);
    if ((repr == Seq_repr_virtual) ||    /* virtual sequence */
		(repr == Seq_repr_map ))         /* map sequence */
    {
        spp->do_virtual = TRUE;
        spp->curpos = 0;
    }
    else if ((repr == Seq_repr_seg) ||   /* segmented */
        (repr == Seq_repr_ref))     /* reference */
    {
        spp->oldcode = 0;        /* no code, not raw */

        if (repr == Seq_repr_seg)  /* segmented */
		{
			fake.choice = SEQLOC_MIX;   /* make SEQUENCE OF Seq-loc, into one */
			fake.data.ptrvalue = bsp->seq_ext;
			the_segs = (SeqLocPtr)&fake;
		}
		else                      /* reference: is a Seq-loc */
	        the_segs = (SeqLocPtr)bsp->seq_ext;

        currseg = NULL;
		ctr = 0;
		done = FALSE;
		started = FALSE;
        while ((! done) && ((currseg = SeqLocFindNext(the_segs, currseg)) != NULL))
        {
			tlen = SeqLocLen(currseg);
			tstrand = SeqLocStrand(currseg);
			tfrom = SeqLocStart(currseg);
			tto = SeqLocStop(currseg);

			if (! started)
			{
				if ((ctr + tlen - 1) >= start)
				{
					tstart = start - ctr;
					started = TRUE;
				}
				else
					tstart = -1;
			}
			else
				tstart = 0;

			if (tstart >= 0)   /* have a start */
			{
				if ((ctr + tlen - 1) >= stop)
				{
					done = TRUE;   /* hit the end */
					tstop = ((ctr + tlen - 1) - stop);
				}
				else
					tstop = 0;

				if (tstrand == Seq_strand_minus)
				{
					xfrom = tfrom + tstop;
					xto = tto - tstart;
				}
				else
				{
					xfrom = tfrom + tstart;
					xto = tto - tstop;
				}

				tbsp = BioseqFind(SeqLocId(currseg));
		
	    		spps = SeqPortNew(tbsp, xfrom, xto, tstrand, newcode);
            	if (spps == NULL)
	            {
			        ErrPost(CTX_NCBISEQPORT, 1  ,
					 "SeqPortNew: %s unexpected null during recursion", errbuf);
            	    return NULL;
	            }
    	        if (sppcurr == NULL)
        	        spp->segs = spps;
            	else
                	sppcurr->next = spps;
	            sppcurr = spps;
			}

			ctr += tlen;
        }
        if (strand == Seq_strand_minus)  /* reverse seqport order */
        {
            prev = spp->segs;
            spprev = spp->segs;
            spp->segs = NULL;
            sppcurr = NULL;
            while (prev != NULL)
            {
                curr = spprev;
                prev = NULL;
                while (curr->next != NULL)  /* end of chain */
                {
                    prev = curr;
                    curr = curr->next;
                }
                if (prev != NULL)
                    prev->next = NULL;
                if (sppcurr == NULL)
                    spp->segs = curr;
                else
                    sppcurr->next = curr;
                sppcurr = curr;
            }
            curr->next = NULL;   /* last one in chain */
        }
        spp->curr = spp->segs;
    }
    else if ((repr == Seq_repr_raw) ||   /* sequence not by reference */
        (repr == Seq_repr_const))
    {
        curr_code = BioseqGetCode(bsp);
        spp->oldcode = curr_code;
        spp->sctp = SeqCodeTableFind(curr_code);

        switch (curr_code)
        {
            case Seq_code_ncbi2na:
                spp->bc = 4;            /* bit shifts needed */
                spp->rshift = 6;
                spp->lshift = 2;
                spp->mask = 192;
                break;
            case Seq_code_ncbi4na:
                spp->bc = 2;
                spp->rshift = 4;
                spp->lshift = 4;
                spp->mask = 240;
                break;
            default:
                spp->bc = 1;
                spp->mask = 255;
                break;
        }

        if ((newcode) && (newcode != curr_code))    /* conversion alphabet */
        {
            if ((spp->smtp = SeqMapTableFind(newcode, curr_code)) != NULL)
                spp->sctp = SeqCodeTableFind(newcode);
        }
    }

    SeqPortSeek(spp, 0, SEEK_SET);
    return spp;
}

/*****************************************************************************
*
*   SeqPortNewByLoc(loc, code)
*       builds a new seqport based on a SeqLoc
*
*****************************************************************************/
SeqPortPtr SeqPortNewByLoc (SeqLocPtr loc, Uint1 code)

{
    BioseqPtr bsp = NULL;
	SeqPortPtr spp = NULL, sppcurr, spps;
    Int4 start, stop;
    Uint1 strand;
	SeqLocPtr currloc = NULL;

    if (loc == NULL)
        return spp;

	bsp = BioseqFind(SeqLocId(loc));
    start = SeqLocStart(loc);
    stop = SeqLocStop(loc);
    strand = SeqLocStrand(loc);


    switch (loc->choice)
    {
        case SEQLOC_EMPTY:      /* empty */
        case SEQLOC_EQUIV:     /* equiv */
        case SEQLOC_BOND:      /* bond */
			break;

        case SEQLOC_NULL:      /* null */
			spp = SeqPortNew(NULL, FIRST_RESIDUE, LAST_RESIDUE, 0, code);
			break;

        case SEQLOC_WHOLE:      /* whole */
    		spp = SeqPortNew(bsp, FIRST_RESIDUE, LAST_RESIDUE, 0, code);
			break;

        case SEQLOC_INT:      /* int */
        case SEQLOC_PNT:      /* pnt */
        case SEQLOC_PACKED_PNT:      /* packed-pnt   */
    		spp = SeqPortNew(bsp, start, stop, strand, code);
			break;

        case SEQLOC_PACKED_INT:      /* packed seqint */
        case SEQLOC_MIX:      /* mix */
		    spp = MemNew(sizeof(SeqPort));
		    spp->totlen = SeqLocLen(loc);
		    spp->start = 0;
		    spp->stop = spp->totlen - 1;
		    spp->curpos = -1;    /* not set */
        	spp->currnum = NULL;   /* use numbering from parts */
	        currloc = NULL;
			sppcurr = NULL;
    	    while ((currloc = SeqLocFindNext(loc, currloc)) != NULL)
        	{
            	spps = SeqPortNewByLoc(currloc, code);
	            if (spps == NULL)
    	        {
			        ErrPost(CTX_NCBISEQPORT, 1  ,
				 "SeqPortNewByLoc unexpected null during recursion");
            	    return NULL;
	            }
	            if (sppcurr == NULL)
    	            spp->segs = spps;
        	    else
            	    sppcurr->next = spps;
	            sppcurr = spps;
	        }
    	    spp->curr = spp->segs;
            break;
        case SEQLOC_FEAT:
	        ErrPost(CTX_NCBISEQPORT, 1  ,
				 "SeqLocNewByLoc: Seq-loc.feat not supported");
            break;
	}

    SeqPortSeek(spp, 0, SEEK_SET);

    return spp;
}

/*****************************************************************************
*
*   SeqPortSeek(spp, offset, origin)
*       works like fseek()
*           returns 0 on success   (weird but true)
*           non-zero on fail
*       uses coordinates 0-(len - 1)  no matter what region seqport covers
*       
*
*****************************************************************************/
Int2 SeqPortSeek (SeqPortPtr spp, Int4 offset, Int2 origin)

{
	Int4 sp, curpos, left;
	Boolean plus_strand;
    Uint1 the_byte, the_residue;
    Int2 bitctr;
    SeqPortPtr curspp;

    if (spp == NULL)
        return 1;
                                /* get position as positive offset from 0 */
    if (spp->strand == Seq_strand_minus)
        plus_strand = FALSE;
    else
        plus_strand = TRUE;

    sp = spp->curpos;    /* current offset, 0 - (totlen - 1)  */
	switch (origin)
	{
		case SEEK_SET:
			if ((offset > spp->totlen) || (offset < 0))
				return 1;
			sp = offset;
			break;
		case SEEK_CUR:
			if (((sp + offset) > spp->totlen) ||
				((sp + offset) < 0 ))
            {
                if (! spp->is_circle)
    				return 1;
            }
            else
    			sp += offset;
            if (spp->is_circle)
            {
                while (sp >= spp->totlen)   /* circle adjustments */
                    sp -= spp->totlen;
                while (sp < 0)
                    sp += spp->totlen;
            }
			break;
		case SEEK_END:
			if ((ABS(offset) > spp->totlen) || (offset > 0))
				return 1;
			sp = spp->totlen + offset;
			break;
		default:
			return 1;
	}

    if (sp == spp->curpos)     /* already in right position */
        return 0;

	if (sp == spp->totlen)    /* seek to EOF */
	{
        spp->curpos = sp;
        spp->byte = SEQPORT_EOF;    /* set to nothing */
        return 0;
    }

    if (spp->oldcode)       /* has data, is raw or const type */
    {
        if (plus_strand)
            curpos = sp + spp->start;
        else
            curpos = spp->stop - sp;
        spp->bytepos = curpos / (Int4) (spp->bc);   /* for compressed codes */
        left = curpos % (Int4) (spp->bc);
        BSSeek(spp->bsp->seq_data, spp->bytepos, SEEK_SET);
        the_byte = (Uint1) BSGetByte(spp->bsp->seq_data);
        if ((plus_strand) || (spp->bc == 1))
            the_residue = the_byte;
        else        /* reverse compressed bit orders */
        {
            left = spp->bc - 1 - left;
            the_residue = 0;
            bitctr = spp->bc;
            while (bitctr)
            {
                the_residue |= the_byte & spp->mask;
                bitctr--;
				if (bitctr)
				{
	                the_residue >>= spp->lshift;
    	            the_byte <<= spp->lshift;
				}
            }
        }
        bitctr = spp->bc;
        while (left)
        {
            the_residue <<= spp->lshift;
            left--; bitctr--;
        }
        spp->byte = the_residue;
        spp->bitctr = (Uint1) bitctr;
    	spp->curpos = sp;
        return 0;
    }
    else if (spp->segs == NULL)   /* virtual or NULL */
    {
        spp->curpos = sp;
        return 0;
    }
    else                    /* segmented, reference sequences */
    {
        curpos = 0;
        curspp = spp->segs;
        while ((curpos + curspp->totlen) <= sp)
        {
            curpos += curspp->totlen;
            curspp = curspp->next;
            if (curspp == NULL)
                return 1;
        }
        if (plus_strand)
            curpos = sp - curpos;
        else
            curpos = (curspp->totlen - 1) - (sp - curpos);
        if (! SeqPortSeek(curspp, curpos, SEEK_SET))
        {
            spp->curr = curspp;
        	spp->curpos = sp;
            return 0;
        }
        else
            return 1;
    }
}

/*****************************************************************************
*
*   Int4 SeqPortTell(spp)
*
*****************************************************************************/
Int4 SeqPortTell (SeqPortPtr spp)

{
    if (spp == NULL)
        return -1L;

    return spp->curpos;
}

/*****************************************************************************
*
*   SeqPortGetResidue(spp)
*       returns residue at current location in requested codeing
*       SEQPORT_EOF = end of file
*
*****************************************************************************/
Uint1 SeqPortGetResidue (SeqPortPtr spp)

{
    Uint1 residue, the_byte, the_residue, the_code;
    Boolean plus_strand = TRUE;
    Int2 bitctr, index;

    if ((spp == NULL) || ((spp->bsp == NULL) && (spp->oldcode)) || (spp->eos))
        return SEQPORT_EOF;

	if ((spp->bsp == NULL) && (spp->segs == NULL))  /* NULL interval */
		return SEQPORT_VIRT;

    if (spp->curpos == spp->totlen)
    {
        if (spp->is_circle)
        {
            SeqPortSeek(spp, 0, SEEK_SET);  /* go to start */
            if (spp->is_seg)   /* give EOS? */
                return SEQPORT_EOS;
        }
        else
            return SEQPORT_EOF;         /* EOF really */
    }

    if (spp->strand == Seq_strand_minus)
        plus_strand = FALSE;

    if (spp->oldcode)    /* its a raw or const sequence */
    {
        residue = spp->byte & spp->mask;
        residue >>= spp->rshift;
        spp->byte <<= spp->lshift;
        spp->bitctr--;
        if (spp->curpos < (spp->totlen - 1))  /* curpos not incremented yet */
        {
            if (spp->bitctr == 0)
            {
                if (! plus_strand) /* need previous byte */
					spp->bytepos--;
				else				/* need next byte */
					spp->bytepos++;
                BSSeek(spp->bsp->seq_data, spp->bytepos, SEEK_SET);
                the_byte = (Uint1) BSGetByte(spp->bsp->seq_data);
                if ((plus_strand) || (spp->bc == 1))
                    the_residue = the_byte;
                else        /* reverse compressed bit orders */
                {
                    the_residue = 0;
                    bitctr = spp->bc;
                    while (bitctr)
                    {
                        the_residue |= the_byte & spp->mask;
                        bitctr--;
						if (bitctr)
						{
	                        the_residue >>= spp->lshift;
    	                    the_byte <<= spp->lshift;
						}
                    }       
                }
                spp->byte = the_residue;
                spp->bitctr = spp->bc;
            }
        }

		if (spp->smtp == NULL)   /* no conversion, check now */
		{
			index = (Int2)residue - (Int2)(spp->sctp->start_at);
			if ((index < 0) || (index >= (Int2)(spp->sctp->num)))
				residue = INVALID_RESIDUE;
			else if (*(spp->sctp->names[index]) == '\0')
				residue = INVALID_RESIDUE;
		}
    }
    else if (spp->segs == NULL)  /* virtual */
    {
        if (spp->do_virtual)
        {
			if (spp->newcode)
				the_code = spp->newcode;
			else
				the_code = spp->oldcode;
			residue = GetGapCode (the_code);
			spp->curpos++;
			return residue;
        }
        else
        {
            return SEQPORT_VIRT;
        }
    }
    else              /* segmented or reference sequence */
    {
        while (! IS_residue((residue = SeqPortGetResidue(spp->curr))))
        {
            spp->curr->eos = FALSE;   /* just in case was set */

            if (spp->curr->next != NULL)
                spp->curr = spp->curr->next;
            else if (spp->is_circle)
                spp->curr = spp->segs;
            else
                return SEQPORT_EOF;

            if (plus_strand)
                SeqPortSeek(spp->curr, 0, SEEK_SET);
            else
                SeqPortSeek(spp->curr, -1, SEEK_END);

            if (spp->is_seg)
                return SEQPORT_EOS;

			if (residue == SEQPORT_VIRT)
				return residue;
        }

        if (! plus_strand)
        {
            if (SeqPortSeek(spp->curr, -2, SEEK_CUR))  /* back up to "next" */
                spp->curr->eos = TRUE;
        }
    }
    
    if (spp->smtp != NULL)
        residue = SeqMapTableConvert(spp->smtp, residue);

    if (! plus_strand)
        residue = SeqCodeTableComp(spp->sctp, residue);

	spp->curpos++;
    return residue;
}

/*****************************************************************************
*
*   GetGapCode(seqcode)
*   	returns code to use for virtual sequence residues for sequence
*         code seqcode
*       returns INVALID_RESIDUE if seqcode invalid
*
*****************************************************************************/
Uint1 GetGapCode (Uint1 seqcode)
{
	Uint1 residue = INVALID_RESIDUE;
	
	switch (seqcode)
	{
		case Seq_code_iupacna:
			residue = 'N';
			break;
		case Seq_code_iupacaa:
		case Seq_code_ncbieaa:
			residue = 'X';
			break;
		case Seq_code_ncbi2na:    /* there isn't ambiguity */
			break;
		case Seq_code_ncbi8na:
		case Seq_code_ncbi4na:
			residue = 15;
			break;
		case Seq_code_iupacaa3:  /* no 1 letter character */
		case Seq_code_ncbipna:
		case Seq_code_ncbipaa:
			break;
		case Seq_code_ncbistdaa:
			residue = 21;
			break;

	}

	return residue;
}


/*****************************************************************************
*
*   SeqPortRead(spp, buf, len)
*       returns bytes read
*       if returns a negative number, then ABS(return value) gives the
*         same codes as SeqPortGetResidue for EOF or EOS
*
*****************************************************************************/
Int2 SeqPortRead (SeqPortPtr spp, Uint1Ptr buf, Int2 len)

{
    Int2 ctr = 0;
    Uint1 retval;

    if ((spp == NULL) || (buf == NULL) || (len <= 0))
        return 0;

    if (spp->lastmsg)    /* previous EOF or EOS saved */
    {
        ctr = spp->lastmsg;
        spp->lastmsg = 0;
        ctr *= -1;
        return ctr;
    }

    while (ctr < len)
    {                              /* not elegant but works for now */
        retval = SeqPortGetResidue(spp);
        if (IS_residue(retval))
        {
            *buf = retval;
            buf++;
            ctr++;
        }
        else
        {
            if (! ctr)   /* first one */
            {
                ctr = retval;   /* send return as negative number */
                ctr *= -1;
                return ctr;
            }
            else
            {
                spp->lastmsg = retval;
                return ctr;
            }
        }
    }
    return ctr;
}

/*****************************************************************************
*
*   ProteinFromCdRegion(sfp, include_stop)
*   	produces a ByteStorePtr containing the protein sequence in
*   ncbieaa code for the CdRegion sfp.  If include_stop, will translate
*   through stop codons.  If NOT include_stop, will stop at first stop
*   codon and return the protein sequence NOT including the terminating
*   stop.  Supports reading frame, alternate genetic codes, and code breaks
*   in the CdRegion
*
*****************************************************************************/
ByteStorePtr ProteinFromCdRegion( SeqFeatPtr sfp, Boolean include_stop)
{
	SeqPortPtr spp = NULL;
	ByteStorePtr bs = NULL;
	Uint1 residue;
	Int4 pos1, pos2, pos, len;
	Int4Ptr the_breaks = NULL;
	Uint1Ptr the_residues = NULL;
	Int2 num_code_break = 0, use_break;
	SeqLocPtr tmp;
	Int2 i, index0, index1, index2, j, k;
	Uint1 codon[3], aa, taa;
	CdRegionPtr crp;
	ValNodePtr vnp;
	GeneticCodePtr gcp;
	CharPtr vals, codes;
	CodeBreakPtr cbp;
	static Uint1 mapping[4] = { 8,     /* T in ncbi4na */
							    2,     /* C */
						        1,     /* A */
						        4 };   /* G */
	Boolean bad_base, no_start;
	Uint2 part_prod = 0, part_loc = 0;

	if ((sfp == NULL) || (sfp->data.choice != 3))
		return NULL;

	crp = (CdRegionPtr) sfp->data.value.ptrvalue;

	num_code_break = 0;
	if (crp->code_break != NULL)
	{
		cbp = crp->code_break;
		while (cbp != NULL)
		{
			num_code_break++;
			cbp = cbp->next;
		}
		the_breaks = MemNew((size_t)(num_code_break * sizeof(Int4)));
		the_residues = MemNew((size_t)(num_code_break * sizeof(Uint1)));

		num_code_break = 0;
		cbp = crp->code_break;
		while (cbp != NULL)
		{
			pos1 = INT4_MAX;
			pos2 = -10;
			tmp = NULL;
			while ((tmp = SeqLocFindNext(cbp->loc, tmp)) != NULL)
			{
				pos = GetOffsetInLoc(tmp, sfp->location, SEQLOC_START);
				if (pos < pos1)
					pos1 = pos;
				pos = GetOffsetInLoc(tmp, sfp->location, SEQLOC_STOP);
				if (pos > pos2)
					pos2 = pos;
			}
			if ((pos2 - pos1) == 2)   /*  a codon */
			{
				the_breaks[num_code_break] = pos1;
				the_residues[num_code_break] = (Uint1) cbp->aa.value.intvalue;
				num_code_break++;
			}
			else
			{
				ErrPost(CTX_NCBIOBJ, 1, "Invalid Code-break.loc");
			}

			cbp = cbp->next;
		}
	}

	gcp = NULL;
	if (crp->genetic_code != NULL)
	{
		vnp = (ValNodePtr)(crp->genetic_code->data.ptrvalue);
		while ((vnp != NULL) && (gcp == NULL))
		{
			switch (vnp->choice)
			{
			case 1:   /* name */
				gcp = GeneticCodeFind(0, (CharPtr)vnp->data.ptrvalue);
				break;
			case 2:   /* id */
				gcp = GeneticCodeFind(vnp->data.intvalue, NULL);
				break;
			case 3:   /* ncbieaa */
			case 6:   /* sncbieaa */
			case 4:   /* ncbi8aa
			case 5:	  /* ncbistdaa */
			case 7:   /* sncbi8aa
			case 8:   /* sncbistdaa */
			default:
				break;
			}
			vnp = vnp->next;
		}
	}
	if (gcp == NULL)
		gcp = GeneticCodeFind(1, NULL);   /* use universal */
	if (gcp == NULL)
		goto erret;

	vals = NULL;
	codes = NULL;
	for (vnp = (ValNodePtr)gcp->data.ptrvalue; vnp != NULL; vnp = vnp->next)
	{
		if (vnp->choice == 6)   /* sncbieaa */
			vals = (CharPtr)vnp->data.ptrvalue;
		else if (vnp->choice == 3)  /* ncbieaa */
			codes = (CharPtr)vnp->data.ptrvalue;
	}
	if (codes == NULL)
		goto erret;

	no_start = FALSE;
	part_loc = SeqLocPartialCheck(sfp->location);
	part_prod = SeqLocPartialCheck(sfp->product);
	if ((part_loc & SLP_START) || (part_prod & SLP_START))
		no_start = TRUE;

	if ((vals == NULL) || (no_start) || (crp->frame > 1))  /* no special starts */
		vals = codes;

	spp = SeqPortNewByLoc(sfp->location, Seq_code_ncbi4na);
	if (spp == NULL)
		goto erret;

	len = SeqLocLen(sfp->location);    /* size of coding region */
	len /= 3;						   /* size of protein */
	len += 1;						   /* allow partial codon at end */
	bs = BSNew(len);
	if (bs == NULL)
		goto erret;

	if (crp->frame == 2)     /* skip partial first codon */
		pos = 1;
	else if (crp->frame == 3)
		pos = 2;
	else
		pos = 0;
	SeqPortSeek(spp, pos, SEEK_SET);
	do
	{
		use_break = -1;
		for (i = 0; i < num_code_break; i++)
		{
			if (pos == the_breaks[i])
			{
				use_break = i;
				i = num_code_break;
			}
		}

		bad_base = FALSE;
		for (i = 0; i < 3; i++)
		{
			residue = SeqPortGetResidue(spp);
			if (residue == SEQPORT_EOF)
				break;
			if (residue == INVALID_RESIDUE)
				bad_base = TRUE;
			codon[i] = residue;
		}
		if (! i)   /* no bases */
			break;
		while (i < 3)      /* incomplete last codon */
		{
			codon[i] = 15;   /* N */
			i++;
		}

		pos += 3;
		if (use_break >= 0)
			aa = the_residues[use_break];
		else if (bad_base)
			aa = 'X';
		else
		{
			aa = 0;
			for (i = 0; i < 4; i++)
			{
				if (codon[0] & mapping[i])
				{
					index0 = i * 16;
					for (j = 0; j < 4; j++)
					{
						if (codon[1] & mapping[j])
						{
							index1 = index0 + (j * 4);
							for (k = 0; k < 4; k++)
							{
								if (codon[2] & mapping[k])
								{
									index2 = index1 + k;
									taa = vals[index2];
									if (! aa)
										aa = taa;
									else
									{
										if (taa != aa)
										{
											aa = 'X';
											break;
										}
									}
								}
								if (aa == 'X')
									break;
							}
						}
						if (aa == 'X')
							break;
					}
				}
				if (aa == 'X')
					break;
			}
		}

		if ((! include_stop) && (aa == '*'))
			break;

		BSPutByte(bs, (Int2)aa);

		vals = codes;     /* not a start codon anymore */

	} while (residue != SEQPORT_EOF);

ret:
	SeqPortFree(spp);
	MemFree(the_breaks);
	MemFree(the_residues);
	return bs;
erret:
	BSFree(bs);
	goto ret;
}
static	Uint1 codon_xref [4] = {   /* mapping from NCBI2na to codon codes */
		2,  /* A */
		1,  /* C */
		3,  /* G */
		0 }; /* T */

/*****************************************************************************
*
*   Uint1 IndexForCodon (codon, code)
*   	returns index into genetic codes codon array, give 3 bases of the
*       codon in any alphabet
*       returns INVALID_RESIDUE on failure
*   
*****************************************************************************/
Uint1 IndexForCodon (Uint1Ptr codon, Uint1 code)
{
	Int2 i, j;
	SeqMapTablePtr smtp;
	Uint1 residue, index = 0;

	smtp = SeqMapTableFind(Seq_code_ncbi2na, code);
	if (smtp == NULL) return INVALID_RESIDUE;

	for (i=0, j=16; i < 3; i++, j /= 4)
	{
		residue = SeqMapTableConvert(smtp, codon[i]);
		if (residue > 3) return INVALID_RESIDUE;
		residue = codon_xref[residue];
		index += (Uint1)(residue * j);
	}

	return index;
}

/*****************************************************************************
*
*   Boolean CodonForIndex (index, code, codon)
*   	Fills codon (3 Uint1 array) with codon corresponding to index,
*       in sequence alphabet code.
*       Index is the Genetic code index.
*       returns TRUE on success.
*
*****************************************************************************/
Boolean CodonForIndex (Uint1 index, Uint1 code, Uint1Ptr codon)
{
	Int2 i, j, k;
	SeqMapTablePtr smtp;
	Uint1 residue;
	
	if (codon == NULL) return FALSE;
	if (index > 63) return FALSE;

	smtp = SeqMapTableFind(code, Seq_code_ncbi2na);
	if (smtp == NULL) return FALSE;

	for (i = 0, j = 16; i < 3; i++, j /= 4)
	{
		residue = (Uint1)((Int2)index / j);
		index -= (Uint1)(residue * j);
		for (k = 0; k < 4; k++)
		{
			if (codon_xref[k] == residue)
			{
				residue = (Uint1)k;
				break;
			}
		}
		residue = SeqMapTableConvert(smtp, residue);
		codon[i] = residue;
	}

	return TRUE;
}

