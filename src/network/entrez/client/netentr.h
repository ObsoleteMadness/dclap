/*   netentr.h
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
* File Name:  netentr.h
*
* Author:  Ostell, Kans, Epstein
*
* Version Creation Date:   10/15/91
*
* $Revision: 4.0 $
*
* File Description: 
*       entrez index access library for Entrez network version
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 8-16-94  Brylawski   Added declarations for on-the-fly neighboring,
*                      medline explosion, and mesh tree browsing.
*
* 11-20-94 Brylawski   Modified NetEntDoNeighborText to pass user preferences.
*
* ==========================================================================
*
*
* RCS Modification History:
* $Log: netentr.h,v $
 * Revision 4.0  1995/07/26  13:54:59  ostell
 * force revision to 4.0
 *
 * Revision 1.18  1995/06/29  16:36:46  epstein
 * add biostruc-complexity
 *
 * Revision 1.17  95/05/17  17:53:13  epstein
 * add RCS log revision history
 * 
*/

#ifndef _NETENTREZ_
#define _NETENTREZ_

#ifndef _ENTREZ_
#include <accentr.h>
#endif

#define TERM_MAX 80

struct termresp {
    CharPtr term;
    Int4    special_count;
    Int4    total_count;
};

typedef struct {
    Int4    num_terms;
    Int4    first_page;
    Int4    num_pages_read;
    struct termresp PNTR termresp;
} TermResp, PNTR TermRespPtr;

typedef struct {
    DocType   type;
    DocField  field;
    CharPtr   term;
} TermData, PNTR TermDataPtr;


/**** Initialize and close session *********************/

extern Boolean CDECL NetEntrezInit PROTO((CharPtr appl_id, Boolean no_warnings));

extern void CDECL NetEntrezFini PROTO((void));

extern EntrezInfoPtr NetEntrezGetInfo PROTO((void));
extern CharPtr NetEntrezDetailedInfo PROTO((void));

/**** Get Types and Terms ******************************/
/**** prototypes are in netlib.h *********************/

/**** Get Links and Neighbors **************************/

Int4 NetEntGetMaxLinks PROTO((void));

LinkSetPtr CDECL NetUidLinks PROTO((DocType type, DocUid uid, DocType link_to_type));
Int2 CDECL NetLinkUidList PROTO((LinkSetPtr PNTR result, DocType type,
DocType link_to_type, Int2 numuid, DocUidPtr uids, Boolean mark_missing));

/**** Get Summaries ************************************/

extern Int2 CDECL NetDocSumListGet PROTO((DocSumPtr PNTR result, Int2 numuid, DocType type, DocUidPtr uids, Int2 defer_count));
extern DocSumPtr CDECL NetDocSum PROTO((DocType type, DocUid uid));

/**** Get Term List ************************************/

extern Int2 CDECL NetTermListByPage PROTO((DocType type, DocField field, Int2 page, Int2 numpage, TermListProc proc));
extern Int2 CDECL NetTermListByTerm PROTO((DocType type, DocField field, CharPtr term, Int2 numterms, TermListProc proc, Int2Ptr first_page));

extern Boolean CDECL NetEntrezFindTerm PROTO((DocType type, DocField field, CharPtr term, Int4Ptr spcl, Int4Ptr totl));

/**** Creates a term node from the uid parameter ********/
extern void CDECL NetEntrezCreateNamedUidList PROTO((CharPtr term, DocType type, DocField field, Int4 num, DocUidPtr uids));
extern void CDECL NetEntrezCreateNamedUidListX PROTO((CharPtr term, DocType type, DocField field, ByteStorePtr bsp));


/**** Look up terms with Boolean operations ************/

extern ValNodePtr CDECL NetEntTLNew PROTO((DocType type));
extern ValNodePtr CDECL NetEntTLAddTerm PROTO((ValNodePtr elst, CharPtr term, DocType type, DocField field, Boolean special));
extern ValNodePtr CDECL NetEntTLFree PROTO((ValNodePtr elst));
extern LinkSetPtr CDECL NetEntTLEval PROTO((ValNodePtr elst));
extern ByteStorePtr CDECL NetEntTLEvalX PROTO((ValNodePtr elst));
extern Int4 CDECL NetEntTLEvalCount PROTO((ValNodePtr elst));

/**** Get the Data **************************************/

Int2 CDECL NetEntMedlineEntryListGet PROTO((MedlineEntryPtr PNTR result, Int2 numuid, DocUidPtr uids, Boolean mark_missing));

Int2 CDECL NetEntSeqEntryListGet PROTO((SeqEntryPtr PNTR result, Int2 numuid,
        DocUidPtr uids, Int2 retcode, Boolean mark_missing));

Int2 CDECL NetEntrezBiostrucListGet PROTO((BiostrucPtr PNTR result, Int4 mdlLvl, Int2 numuid,
                                    DocUidPtr uids, Boolean mark_missing));


/**** Get a SeqId for a GI ***************************/

SeqIdPtr NetSeqIdForGI PROTO((Int4 gi));
Int4 NetEntrezFindSeqId PROTO((SeqIdPtr sip));
EntrezHierarchyPtr NetEntMeshHierarchyGet PROTO((CharPtr term));

/**** Neighbor some text on-the-fly ! ************/
LinkSetPtr CDECL NetEntDoNeighborText PROTO((EntrezNeighborTextPtr entp));
Boolean CDECL NetEntCanNeighborText PROTO((void));

/**** BLAST a sequence on-the-fly ! ************/
Boolean CDECL NetEntCanBlast PROTO((void));
LinkSetPtr LIBCALL NetEntBlastBioseq PROTO((BioseqPtr bsp, DocType db, CharPtr program, CharPtr database, CharPtr options, Boolean usemonitor));



Boolean CDECL NetEntExpandedMedlineFeatures PROTO((void));

/* # of retries to get a server */
#define ENT_SERV_RETRIES 2


#ifndef _NETROMLIB_
#include <netlib.h>
#endif

#endif

