/*  objcode.h
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
* File Name:  objcode.h
*
* Author:  James Ostell
*   
* Version Creation Date: 8/10/92
*
* $Revision: 2.1 $
*
* File Description:  Object manager interface for module NCBI-SeqCode
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_SeqCode_
#define _NCBI_SeqCode_

#ifndef _ASNTOOL_
#include <asn.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
Boolean LIBCALL SeqCodeAsnLoad PROTO((void));

/*****************************************************************************
*
*   SeqMapTable - Table from mapping sequence codes to each other
*      Codes ALWAYS start from 0 and increase continuously
*      IUPAC has 65 empty rows
*
*****************************************************************************/
typedef struct seqmaptable {
    Uint1 from,                 /* as ENUMERATED in Seq-code-type */
        to;
    Uint1 num;
    Uint1 start_at;
    Uint1Ptr table;
    struct seqmaptable PNTR next;
} SeqMapTable, PNTR SeqMapTablePtr;

SeqMapTablePtr LIBCALL SeqMapTableNew PROTO((void));
Boolean        LIBCALL SeqMapTableAsnWrite PROTO((SeqMapTablePtr smp, AsnIoPtr aip, AsnTypePtr atp));
SeqMapTablePtr LIBCALL SeqMapTableAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
SeqMapTablePtr LIBCALL SeqMapTableFree PROTO((SeqMapTablePtr smp));

SeqMapTablePtr LIBCALL SeqMapTableFind PROTO((Uint1 to, Uint1 from));

/*****************************************************************************
*
*   SeqCodeTable - Table of sequence codes
*       in code order, starting with 0 and increasing continuously
*
*****************************************************************************/
typedef struct seqcodetable {
    Uint1 code;                        /* as ENUMERATED in Seq-code-type */
    Uint1 num;                          /* number of codes */
    Boolean one_letter;                  /* one letter codes? */
    Uint1 start_at;                     /* index offset of first code */
    CharPtr letters;                   /* one letter codes */
    CharPtr PNTR symbols;              /* multi-length codes */
    CharPtr PNTR names;                /* explanatory names */
    Uint1Ptr comps;                    /* maps to complements */
    struct seqcodetable PNTR next;
} SeqCodeTable, PNTR SeqCodeTablePtr;

SeqCodeTablePtr LIBCALL SeqCodeTableNew PROTO((void));
Boolean         LIBCALL SeqCodeTableAsnWrite PROTO((SeqCodeTablePtr scp, AsnIoPtr aip, AsnTypePtr atp));
SeqCodeTablePtr LIBCALL SeqCodeTableAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
SeqCodeTablePtr LIBCALL SeqCodeTableFree PROTO((SeqCodeTablePtr scp));

SeqCodeTablePtr LIBCALL SeqCodeTableFind PROTO((Uint1 code));

/*****************************************************************************
*
*   SeqCodeSet - Set of sequence codes and maps
*
*****************************************************************************/
typedef struct seqcodeset {
    SeqCodeTablePtr codes;
    SeqMapTablePtr maps;
} SeqCodeSet, PNTR SeqCodeSetPtr;

SeqCodeSetPtr LIBCALL SeqCodeSetNew PROTO((void));
Boolean       LIBCALL SeqCodeSetAsnWrite PROTO((SeqCodeSetPtr ssp, AsnIoPtr aip, AsnTypePtr atp));
SeqCodeSetPtr LIBCALL SeqCodeSetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
SeqCodeSetPtr LIBCALL SeqCodeSetFree PROTO((SeqCodeSetPtr ssp));

/*****************************************************************************
*
*   SeqCodeSetLoad()
*       loads the standard sequence codes from "seqcode.val" in "data"
*
*****************************************************************************/
SeqCodeSetPtr LIBCALL SeqCodeSetLoad PROTO((void));

#ifdef __cplusplus
}
#endif

#endif
