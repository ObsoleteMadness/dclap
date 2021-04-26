/*   cdentrez.h
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
* File Name:  cdentrez.h
*
* Author:  Ostell, Kans
*
* Version Creation Date:   10/15/91
*
* $Revision: 2.9 $
*
* File Description: 
*   	entrez index access library for Entrez CDROM
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/

#ifndef _CDENTREZ_
#define _CDENTREZ_

#ifndef _ACCENTR_
#include <accentr.h>
#endif
#ifndef _CDROMLIB_
#include <cdromlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**** Initialize and close session *********************/

Boolean  CdEntrezInit PROTO((Boolean no_warnings));
void  CdEntrezFini PROTO((void));

/**** Get Types and Terms ******************************/
/**** prototypes are in cdromlib.h *********************/

#define CdEntrezGetInfo CdGetInfo
#define CdEntrezDetailedInfo CdDetailedInfo

/**** Get Links and Neighbors **************************/

Int4 CdEntGetMaxLinks PROTO((void));

LinkSetPtr  CdUidLinks PROTO((DocType type, DocUid uid, DocType link_to_type));
Int2  CdLinkUidList PROTO((LinkSetPtr PNTR result, DocType type,
		DocType link_to_type, Int2 numuid, Int4Ptr uids, Boolean mark_missing));

/**** Get Summaries ************************************/

DocSumPtr  CdDocSum PROTO((DocType type, DocUid uid));
Int2  CdEntMlSumListGet PROTO((DocSumPtr PNTR result, Int2 numuid, Int4Ptr uids));
Int2  CdEntSeqSumListGet PROTO((DocSumPtr PNTR result, Int2 numuid, DocType db, Int4Ptr uids));

/**** Get Term List ************************************/

Int2  CdTermListByPage PROTO((DocType type, DocField field,
				Int2 page, Int2 numpage, TermListProc proc));
Int2  CdTermListByTerm PROTO((DocType type, DocField field,
				CharPtr term, Int2 numterms, TermListProc proc, Int2Ptr first_page));

Boolean  CdEntrezFindTerm PROTO((DocType type, DocField field, CharPtr term, Int4Ptr spcl, Int4Ptr totl));

/**** Creates a term node from the uid parameter ********/
void  CdEntrezCreateNamedUidList PROTO((CharPtr term, DocType type, DocField field, Int4 num, DocUidPtr uids));

/**** Look up terms with Boolean operations ************/

ValNodePtr  CdEntTLNew PROTO((DocType type));
ValNodePtr  CdEntTLAddTerm PROTO((ValNodePtr elst, CharPtr term, DocType type, DocField field, Boolean special));
ValNodePtr  CdEntTLFree PROTO((ValNodePtr elst));
LinkSetPtr  CdEntTLEval PROTO((ValNodePtr elst));

ByteStorePtr  CdEntTLEvalX PROTO((ValNodePtr elst));

/**** Retrieve Data ***********************************/

Int2  CdEntMedlineEntryListGet PROTO((MedlineEntryPtr PNTR result, Int2 numuid,
		Int4Ptr uids, Boolean mark_missing));
Int2  CdEntSeqEntryListGet PROTO((SeqEntryPtr PNTR result, Int2 numuid,
		Int4Ptr uids, Int2 retcode, Boolean mark_missing));

#ifdef __cplusplus
}
#endif


#endif


