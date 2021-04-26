/*  objres.h
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
* File Name:  objres.h
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.1 $
*
* File Description:  Object manager interface for module NCBI-Seqres
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_Seqres_
#define _NCBI_Seqres_

#ifndef _ASNTOOL_
#include <asn.h>
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
Boolean LIBCALL SeqResAsnLoad PROTO((void));

/*****************************************************************************
*
*   internal structures for NCBI-SeqRes objects
*
*****************************************************************************/

/*****************************************************************************
*
*   SeqGraph
*
*****************************************************************************/
typedef struct seqgraph {
    CharPtr title,
        comment;
    SeqLocPtr loc;     /* SeqLoc */
    CharPtr titlex,
        titley;
    Uint1 flags[3];   /* [0]-comp used?,[1]-a,b used?, [2] graphtype */
    Int4 compl;                       /* 1=real, 2=int, 3=byte */
    FloatHi a, b;
    Int4 numval;
    DataVal max,
        min,
        axis;
    Pointer values;  /* real=array of FloatHi, int=array of Int4, */
    struct seqgraph PNTR next;              /* byte = ByteStore */
} SeqGraph, PNTR SeqGraphPtr;

SeqGraphPtr LIBCALL SeqGraphNew PROTO((void));
Boolean     LIBCALL SeqGraphAsnWrite PROTO((SeqGraphPtr sgp, AsnIoPtr aip, AsnTypePtr atp));
SeqGraphPtr LIBCALL SeqGraphAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
SeqGraphPtr LIBCALL SeqGraphFree PROTO((SeqGraphPtr sgp));

/*****************************************************************************
*
*   SeqGraphSet
*
*****************************************************************************/
Boolean     LIBCALL SeqGraphSetAsnWrite PROTO((SeqGraphPtr sgp, AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element));
SeqGraphPtr LIBCALL SeqGraphSetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element));

#ifdef __cplusplus
}
#endif

#endif
