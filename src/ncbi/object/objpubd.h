/*  objpubd.h
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
* File Name:  objpubd.h
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.1 $
*
* File Description:  Object manager interface for type Pubdesc from
*                    NCBI-Sequence.  This is separate to avoid typedef
*                    order problems with NCBI-Sequence and NCBI-Seqfeat
*                    which both reference Pubdesc
*   				   Numbering and Heterogen have now also been added
*   				for the same reason.  Heterogen is just a string,
*   				so no special typedefs are required.
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_Pubdesc_
#define _NCBI_Pubdesc_

#ifndef _ASNTOOL_
#include <asn.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   Pubdesc
*
*****************************************************************************/
typedef struct pd {
    ValNodePtr pub;          /* points to Pub-equiv */
    CharPtr name,
        fig;
    ValNodePtr num;          /* points to Numbering */
    Boolean numexc,
        poly_a;
    Uint1 align_group;       /* 0 = not part of a group */
    CharPtr maploc,
        seq_raw,
		comment;
} Pubdesc, PNTR PubdescPtr;

PubdescPtr LIBCALL PubdescNew PROTO((void));
Boolean    LIBCALL PubdescAsnWrite PROTO((PubdescPtr pdp, AsnIoPtr aip, AsnTypePtr atp));
PubdescPtr LIBCALL PubdescAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
PubdescPtr LIBCALL PubdescFree PROTO((PubdescPtr pdp));

typedef ValNode Numbering, FAR *NumberingPtr;

/*****************************************************************************
*
*   Numbering uses an ValNode with choice = 
    1 = cont Num-cont ,              -- continuous numbering
    2 = enum Num-enum ,              -- enumerated names for residues
    3 = ref Num-ref, type 1 sources  -- by reference to another sequence
    4 = ref Num-ref, type 2 aligns  (SeqAlign in data.ptrvalue)
    5 = real Num-real     -- for maps etc
*
*****************************************************************************/

#define Numbering_cont 1
#define Numbering_enum 2
#define Numbering_ref_source 3
#define Numbering_ref_align 4
#define Numbering_real 5

Boolean      LIBCALL NumberingAsnWrite PROTO((NumberingPtr anp, AsnIoPtr aip, AsnTypePtr atp));
NumberingPtr LIBCALL NumberingAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
NumberingPtr LIBCALL NumberingFree PROTO((NumberingPtr anp));

/*****************************************************************************
*
*   NumCont - continuous numbering system
*
*****************************************************************************/
typedef struct numcont {
    Int4 refnum;
    Boolean has_zero,
        ascending;
} NumCont, PNTR NumContPtr;

NumContPtr LIBCALL NumContNew PROTO((void));
Boolean    LIBCALL NumContAsnWrite PROTO((NumContPtr ncp, AsnIoPtr aip, AsnTypePtr atp));
NumContPtr LIBCALL NumContAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
NumContPtr LIBCALL NumContFree PROTO((NumContPtr ncp));

/*****************************************************************************
*
*   NumEnum - enumerated numbering system
*
*****************************************************************************/
typedef struct numenum {
    Int4 num;               /* number of names */
    CharPtr buf;            /* a buffer for the names */
    CharPtr PNTR names;     /* array of pointers to names */
} NumEnum, PNTR NumEnumPtr;

NumEnumPtr LIBCALL NumEnumNew PROTO((void));
Boolean    LIBCALL NumEnumAsnWrite PROTO((NumEnumPtr nep, AsnIoPtr aip, AsnTypePtr atp));
NumEnumPtr LIBCALL NumEnumAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
NumEnumPtr LIBCALL NumEnumFree PROTO((NumEnumPtr nep));

/*****************************************************************************
*
*   NumReal - float type numbering system
*
*****************************************************************************/
typedef struct numreal {
    FloatHi a, b;        /* number in "units" = ax + b */
    CharPtr units;
} NumReal, PNTR NumRealPtr;

NumRealPtr LIBCALL NumRealNew PROTO((void));
Boolean    LIBCALL NumRealAsnWrite PROTO((NumRealPtr ncp, AsnIoPtr aip, AsnTypePtr atp));
NumRealPtr LIBCALL NumRealAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
NumRealPtr LIBCALL NumRealFree PROTO((NumRealPtr ncp));

#ifdef __cplusplus
}
#endif

#endif
