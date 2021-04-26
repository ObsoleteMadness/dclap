/*  seqport.h
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
* File Name:  seqport.h
*
* Author:  James Ostell
*   
* Version Creation Date: 7/13/91
*
* $Revision: 2.7 $
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

#ifndef _NCBI_Seqport_
#define _NCBI_Seqport_

#include <sequtil.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   SeqPort
*       will attach only to a Bioseq (SeqPortNew) or to a Seq-loc
*          (SeqPortNewByLoc) in any selected alphabet
*       You can then treat the sequence or location as a single contiguous
*          piece. You can Seek (SeqPortSeek) to any location. You can
*          SeqPortGetResidue, which get the residue at the current position
*          and seeks to the next residue. You can read a buffer of residues.
*   
*       Special characters are returned from SeqPortGetResidue
*   		SEQPORT_EOF   (end of sequence reached)
*           SEQPORT_VIRT  (hit a virtual sequence or gap)
*           INVALID_RESIDUE (residue not valid in original Bioseq)
*   		SEQPORT_EOS   (end of segment, not normally seen)
*
*       Some defined values are provided for the Int4 values passed as
*           start or stop
*       FIRST_RESIDUE   0	(first residue of sequence)
*       LAST_RESIDUE    -1  (last residue of sequence.. interpreted as
*                              length - 1)
*       APPEND_RESIDUE  -2  (interpreted as length.. off the end of the
*                              sequence. Not valid for SeqPort.. only
*                              used by editing functions )
*            
*          
*
*****************************************************************************/
#define SEQPORT_EOF 253       /* end of sequence data */
#define SEQPORT_EOS 252       /* end of segment */
#define SEQPORT_VIRT 251      /* skipping virtual sequence or gap */
#define IS_residue(x) (x <= 250)

#define FIRST_RESIDUE 0
#define LAST_RESIDUE -1
#define APPEND_RESIDUE -2

typedef struct seqport {
    BioseqPtr bsp;             /* 1 seqentry per port */
    Int4 start, stop,            /* region of bsp covered */
        curpos,                  /* current position 0-(totlen-1) */
        totlen,                  /* total length of covered region */
		bytepos;                 /* current byte position in bsp->data */
    NumberingPtr currnum;        /* current numbering info */
    Uint1 strand,                /* as in seqloc */
        lastmsg;                 /* used by SeqPortRead() */
    Boolean is_circle ,          /* go around the end of a circle? */
            is_seg ,             /* return EOS at the end of segments? */
            do_virtual,          /* deliver 'N''X' over virtual seqs */
            eos;                 /* set when comp strand tries to back off */
    SeqMapTablePtr smtp;         /* for mapping to requested alphabet */
    SeqCodeTablePtr sctp;        /* for getting symbols */
    Uint1 newcode,               /* requested output code */
          oldcode;               /* current input seq code (0 if not raw) */
    Uint1 byte,                    /* current byte in buf */
        bc,                      /* value to start bitctr */
        bitctr,                  /* current shift */
        lshift,                  /* amount to left shift on decompact */
        rshift,                  /* amount to right shift residue value */
        mask;                    /* mask for compact byte */
    struct seqport PNTR curr ,   /* current active seqport if seg or ref */
        PNTR segs,                    /* segments if seg or ref */
        PNTR next;                    /* if part of a segment chain */
} SeqPort, PNTR SeqPortPtr;

SeqPortPtr SeqPortNew PROTO((BioseqPtr bsp, Int4 start, Int4 stop, Uint1 strand, Uint1 code));
SeqPortPtr SeqPortNewByLoc PROTO((SeqLocPtr seqloc, Uint1 code));
SeqPortPtr SeqPortFree PROTO((SeqPortPtr spp));
Int4 SeqPortTell PROTO((SeqPortPtr spp));
Int2 SeqPortSeek PROTO((SeqPortPtr spp, Int4 offset, Int2 origin));
Int4 SeqPortLen PROTO((SeqPortPtr spp));
Uint1 SeqPortGetResidue PROTO((SeqPortPtr spp));
Int2 SeqPortRead PROTO((SeqPortPtr spp, BytePtr buf, Int2 len));
Uint1 GetGapCode PROTO((Uint1 seqcode));

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
ByteStorePtr ProteinFromCdRegion PROTO(( SeqFeatPtr sfp, Boolean include_stop));

/*****************************************************************************
*
*   Uint1 IndexForCodon (codon, code)
*   	returns index into genetic codes codon array, give 3 bases of the
*       codon in any alphabet
*       returns INVALID_RESIDUE on failure
*   
*****************************************************************************/
Uint1 IndexForCodon PROTO((Uint1Ptr codon, Uint1 code));

/*****************************************************************************
*
*   Boolean CodonForIndex (index, code, codon)
*   	Fills codon (3 Uint1 array) with codon corresponding to index,
*       in sequence alphabet code.
*       Index is the Genetic code index.
*       returns TRUE on success.
*
*****************************************************************************/
Boolean CodonForIndex PROTO((Uint1 index, Uint1 code, Uint1Ptr codon));

#ifdef __cplusplus
}
#endif

#endif
