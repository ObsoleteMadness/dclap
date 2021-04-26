/*  objsset.h
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
* File Name:  objsset.h
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.1 $
*
* File Description:  Object manager interface for module NCBI-Seqset
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_Seqset_
#define _NCBI_Seqset_

#ifndef _ASNTOOL_
#include <asn.h>
#endif
#ifndef _NCBI_General_
#include <objgen.h>
#endif
#ifndef _NCBI_Seq_
#include <objseq.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef ValNode SeqEntry, FAR *SeqEntryPtr;

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
Boolean LIBCALL SeqSetAsnLoad PROTO((void));

/*****************************************************************************
*
*   internal structures for NCBI-Seqset objects
*
*****************************************************************************/
/*****************************************************************************
*
*   BioseqSet - a collection of sequences
*
*****************************************************************************/
typedef struct seqset {
    ObjectIdPtr id;
    DbtagPtr coll;
    Int2 level;            /* set to INT2_MIN (ncbilcl.h) for not used */
    Uint1 _class;
    CharPtr release;
    DatePtr date;
    ValNodePtr descr;
    SeqEntryPtr seq_set;
    SeqAnnotPtr annot;
} BioseqSet, PNTR BioseqSetPtr;

BioseqSetPtr LIBCALL BioseqSetNew PROTO((void));
Boolean      LIBCALL BioseqSetAsnWrite PROTO((BioseqSetPtr bsp, AsnIoPtr aip, AsnTypePtr atp));
BioseqSetPtr LIBCALL BioseqSetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
BioseqSetPtr LIBCALL BioseqSetFree PROTO((BioseqSetPtr bsp));

/*****************************************************************************
*
*   SeqEntry - implemented as an ValNode
*     choice:
*       1 = Bioseq
*       2 = Bioseq-set
*
*****************************************************************************/

SeqEntryPtr LIBCALL SeqEntryNew PROTO((void));
Boolean     LIBCALL SeqEntryAsnWrite PROTO((SeqEntryPtr sep, AsnIoPtr aip, AsnTypePtr atp));
SeqEntryPtr LIBCALL SeqEntryAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
SeqEntryPtr LIBCALL SeqEntryFree PROTO((SeqEntryPtr sep));
SeqEntryPtr PNTR LIBCALL SeqEntryInMem PROTO((Int2Ptr numptr));

/*****************************************************************************
*
*   Options for SeqEntryAsnRead()
*
*****************************************************************************/
SeqEntryPtr LIBCALL SeqEntryAsnGet PROTO((AsnIoPtr aip, AsnTypePtr atp, SeqIdPtr sip, Int2 retcode));

#define SEQENTRY_OPTION_MAX_COMPLEX 1   /* option type to use with OP_NCBIOBJSSET */

    /* values for retcode, implemented with AsnIoOptions */
#define SEQENTRY_READ_BIOSEQ 1    /* read only Bioseq identified by sip */
#define SEQENTRY_READ_SEG_SET 2   /* read any seg-set it may be part of */
#define SEQENTRY_READ_NUC_PROT 3   /* read any nuc-prot set it may be in */
#define SEQENTRY_READ_PUB_SET 4    /* read pub-set it may be part of */

typedef struct objsset_option {
	SeqIdPtr sip;              /* seq-id to find */
	Int2 retcode;              /* type of set/seq to return */
	Boolean in_right_set;
	Uint1 working_on_set;      /* 2, if in first set of retcode type */
	                           /* 1, if found Bioseq, but not right set */
	                           /* 0, if Bioseq not yet found */
} Op_objsset, PNTR Op_objssetPtr;


#define IS_Bioseq(a) ((a)->choice == 1)
#define IS_Bioseq_set(a)  ((a)->choice == 2)

/*****************************************************************************
*
*   loader for ObjSeqSet and Sequence Codes
*
*****************************************************************************/
Boolean LIBCALL SeqEntryLoad PROTO((void));


#ifdef __cplusplus
}
#endif

#endif
