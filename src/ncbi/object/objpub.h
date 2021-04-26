/*  objpub.h
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
* File Name:  objpub.h
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.2 $
*
* File Description:  Object manager interface for module NCBI-Pub
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_Pub_
#define _NCBI_Pub_

#ifndef _ASNTOOL_
#include <asn.h>
#endif
#ifndef _NCBI_Biblio_
#include <objbibli.h>
#endif
#ifndef _NCBI_Medline_
#include <objmedli.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
Boolean LIBCALL PubAsnLoad PROTO((void));

/*****************************************************************************
*
*   internal structures for NCBI-Pub objects
*
*****************************************************************************/

/*****************************************************************************
*
*   Pub is a choice using an ValNode, most types in data.ptrvalue
*   choice:
*   0 = not set
    1 = gen Cit-gen ,        -- general or generic unparsed
    2 = sub Cit-sub ,        -- submission
    3 = medline Medline-entry ,
    4 = muid INTEGER ,       -- medline uid (stored in data.intvalue)
    5 = article Cit-art ,
    6 = journal Cit-jour ,
    7 = book Cit-book ,
    8 = proc Cit-proc ,      -- proceedings of a meeting
    9 = patent Cit-pat ,
    10 = pat-id Id-pat ,      -- identify a patent
    11 = man Cit-let         -- manuscript or letter
    12 = equiv Pub-equiv      -- set of equivalent citation forms for 1 pub
*
*****************************************************************************/
Boolean    LIBCALL PubAsnWrite PROTO((ValNodePtr anp, AsnIoPtr aip, AsnTypePtr atp));
ValNodePtr LIBCALL PubAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
ValNodePtr LIBCALL PubFree PROTO((ValNodePtr anp));

#define PUB_Gen 1
#define PUB_Sub 2
#define PUB_Medline 3
#define PUB_Muid 4
#define PUB_Article 5
#define PUB_Journal 6
#define PUB_Book 7
#define PUB_Proc 8
#define PUB_Patent 9
#define PUB_Pat_id 10
#define PUB_Man 11
#define PUB_Equiv 12
/****
*  Pub and PubEquiv Matching functions (same citation, not same form)
*   PubMatch() returns
*   	0 = point to same citation
*       1,-1 = same pub type, but different
*       2,-2 = different put types, don't match
*   PubEquivMatch() returns
*   	0 = point to same citation
*   	1,-1 = point to different citations
*****/
Int2 LIBCALL PubMatch PROTO((ValNodePtr a, ValNodePtr b));
Int2 LIBCALL PubEquivMatch PROTO((ValNodePtr a, ValNodePtr b));

/*****************************************************************************
*
*   PubSet is a choice using an ValNode, PubSet->data.ptrvalue is chain of
*       Pubs (ValNodes) holding data for set for all types.
*   PubSet->choice:
*   0 = not set
    1 = pub Pub    -- set of real Pubs
                   -- the rest are implemented as Pubs anyway
    3 = medline Medline-entry ,
    5 = article Cit-art ,
    6 = journal Cit-jour ,
    7 = book Cit-book ,
    8 = proc Cit-proc ,      -- proceedings of a meeting
    9 = patent Cit-pat ,
*
*****************************************************************************/
Boolean    LIBCALL PubSetAsnWrite PROTO((ValNodePtr anp, AsnIoPtr aip, AsnTypePtr atp));
ValNodePtr LIBCALL PubSetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
ValNodePtr LIBCALL PubSetFree PROTO((ValNodePtr anp));

/*****************************************************************************
*
*   PubEquiv is just a chain of Pubs (ValNodes)
*
*****************************************************************************/
Boolean    LIBCALL PubEquivAsnWrite PROTO((ValNodePtr anp, AsnIoPtr aip, AsnTypePtr atp));
ValNodePtr LIBCALL PubEquivAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
ValNodePtr LIBCALL PubEquivFree PROTO((ValNodePtr anp));

#ifdef __cplusplus
}
#endif

#endif
