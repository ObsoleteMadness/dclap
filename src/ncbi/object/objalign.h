/*  objalign.h
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
* File Name:  objalign.h
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.1 $
*
* File Description:  Object manager interface for module NCBI-Seqalign
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_Seqalign_
#define _NCBI_Seqalign_

#ifndef _ASNTOOL_
#include <asn.h>
#endif
#ifndef _NCBI_General_
#include <objgen.h>
#endif
#ifndef _NCBI_Seqloc_
#include <objloc.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
Boolean LIBCALL SeqAlignAsnLoad PROTO((void));

/*****************************************************************************
*
*   internal structures for NCBI-Seqalign objects
*
*****************************************************************************/

/*****************************************************************************
*
*   Score
*     NOTE: read, write, and free always process GROUPS of scores
*
*****************************************************************************/
typedef struct score {
    ObjectIdPtr id;
    Uint1 choice;          /* 0=not set, 1=int, 2=real */
    DataVal value;
    struct score PNTR next;    /* for sets of scores */
} Score, PNTR ScorePtr;

ScorePtr LIBCALL ScoreNew PROTO((void));
Boolean  LIBCALL ScoreSetAsnWrite PROTO((ScorePtr sp, AsnIoPtr aip, AsnTypePtr settype, AsnTypePtr elementtype));
ScorePtr LIBCALL ScoreSetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr settype, AsnTypePtr elementtype));
ScorePtr LIBCALL ScoreSetFree PROTO((ScorePtr anp));

/*****************************************************************************
*
*   SeqAlign
*   type =  type of alignment
        not-set (0) ,
        global (1) ,
        diags (2) ,
        partial (3) ,           -- mapping pieces together
        other (255) } ,
    segtype = type of segs structure
        not-set 0
        dendiag 1
        denseq 2
        std 3
*   
*
*****************************************************************************/
typedef struct seqalign {
    Uint1 type,
        segtype;
    Int2 dim;
    ScorePtr score;
    Pointer segs;
    struct seqalign PNTR next;
} SeqAlign, PNTR SeqAlignPtr;

SeqAlignPtr LIBCALL SeqAlignNew PROTO((void));
Boolean     LIBCALL SeqAlignAsnWrite PROTO((SeqAlignPtr anp, AsnIoPtr aip, AsnTypePtr atp));
SeqAlignPtr LIBCALL SeqAlignAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
SeqAlignPtr LIBCALL SeqAlignFree PROTO((SeqAlignPtr anp));

/*****************************************************************************
*
*   SeqAlignSet
*
*****************************************************************************/
Boolean     LIBCALL SeqAlignSetAsnWrite PROTO((SeqAlignPtr anp, AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element));
SeqAlignPtr LIBCALL SeqAlignSetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element));

/*****************************************************************************
*
*   DenseDiag
*   
*
*****************************************************************************/
typedef struct dendiag {
    Int2 dim;                   /* this is a convenience, not in asn1 */
    SeqIdPtr id;
    Int4Ptr starts;
    Int4 len;
    Uint1Ptr strands;
    ScorePtr scores;
    struct dendiag PNTR next;
} DenseDiag, PNTR DenseDiagPtr;

DenseDiagPtr LIBCALL DenseDiagNew PROTO((void));
Boolean      LIBCALL DenseDiagAsnWrite PROTO((DenseDiagPtr ddp, AsnIoPtr aip, AsnTypePtr atp));
DenseDiagPtr LIBCALL DenseDiagAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
DenseDiagPtr LIBCALL DenseDiagFree PROTO((DenseDiagPtr ddp));

/*****************************************************************************
*
*   DenseSeg
*   
*
*****************************************************************************/
typedef struct denseg {
    Int2 dim,
        numseg;
    SeqIdPtr ids;
    Int4Ptr starts;
    Int4Ptr lens;
    Uint1Ptr strands;
    ScorePtr scores;
} DenseSeg, PNTR DenseSegPtr;

DenseSegPtr LIBCALL DenseSegNew PROTO((void));
Boolean     LIBCALL DenseSegAsnWrite PROTO((DenseSegPtr dsp, AsnIoPtr aip, AsnTypePtr atp));
DenseSegPtr LIBCALL DenseSegAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
DenseSegPtr LIBCALL DenseSegFree PROTO((DenseSegPtr dsp));

/*****************************************************************************
*
*   StdSeg
*   
*
*****************************************************************************/
typedef struct stdseg {
    Int2 dim;
    SeqIdPtr ids;    /* SeqId s */
    SeqLocPtr loc;    /* SeqLoc s */
    ScorePtr scores;
    struct stdseg PNTR next;
} StdSeg, PNTR StdSegPtr;

StdSegPtr LIBCALL StdSegNew PROTO((void));
Boolean   LIBCALL StdSegAsnWrite PROTO((StdSegPtr ssp, AsnIoPtr aip, AsnTypePtr atp));
StdSegPtr LIBCALL StdSegAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
StdSegPtr LIBCALL StdSegFree PROTO((StdSegPtr ssp));

#ifdef __cplusplus
}
#endif

#endif
