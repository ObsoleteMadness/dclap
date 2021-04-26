/*  simple.h
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
* File Name:  simple.h
*
* Author:  James Ostell
*   
* Version Creation Date: 7/12/91
*
* $Revision: 2.0 $
*
* File Description:  Simple Sequence Objects
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/
#ifndef _ASNTOOL_
#include <asn.h>
#endif

#ifndef _NCBI_Simple_
#define _NCBI_Simple_

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
extern Boolean AllAsnLoad PROTO((void));

/*****************************************************************************
*
*   internal structures for simple objects
*
*****************************************************************************/
typedef struct simpleseq {
    CharPtr id[10];       /* room for 10 id strings */
    Int2 numid,
         bestid;          /* most stable id in set */
    CharPtr title;
    ByteStorePtr seq;
    Int4 seqlen;
} SimpleSeq, PNTR SimpleSeqPtr;

SimpleSeqPtr SimpleSeqNew PROTO((void));
SimpleSeqPtr SimpleSeqAsnRead PROTO((AsnIoPtr aip));
SimpleSeqPtr SimpleSeqFree PROTO((SimpleSeqPtr ssp));
Boolean SimpleSeqPrint PROTO((SimpleSeqPtr ssp, FILE * fp, Boolean as_fasta));

#ifdef __cplusplus
}
#endif

#endif

