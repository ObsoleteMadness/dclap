/*  objneten.h
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
* File Name:  objneten.h
*
* Author:  Epstein
*   
* Version Creation Date: 6/3/92
*
* $Revision: 1.5 $
*
* File Description:  Object loader interface for module NCBI-Entrez
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_NetEntrez_
#define _NCBI_NetEntrez_

#ifndef _ASNTOOL_
#include <asn.h>
#endif

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
extern Boolean NetEntAsnLoad PROTO((void));

/*****************************************************************************
*
*   internal structures for NCBI-Entrez objects
*
*****************************************************************************/

/*****************************************************************************
*
*   Entrez-ids
*
*****************************************************************************/


typedef struct {
	Int4 numid;
	DocUidPtr ids;
} EntrezIds, PNTR EntrezIdsPtr;

extern EntrezIdsPtr CDECL EntrezIdsNew PROTO((void));
extern EntrezIdsPtr CDECL EntrezIdsFree PROTO(( EntrezIdsPtr ufp));
extern EntrezIdsPtr CDECL EntrezIdsAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL EntrezIdsAsnWrite PROTO((EntrezIdsPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


/*****************************************************************************
*
*   Marked-link-set
*
*****************************************************************************/
typedef struct {
	LinkSetPtr link_set;
	Int4 uids_processed;
	EntrezIdsPtr  marked_missing;
} MarkedLinkSet, PNTR MarkedLinkSetPtr;


extern MarkedLinkSetPtr CDECL MarkedLinkSetNew PROTO((void));
extern MarkedLinkSetPtr CDECL MarkedLinkSetFree PROTO(( MarkedLinkSetPtr ufp));
extern MarkedLinkSetPtr CDECL MarkedLinkSetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL MarkedLinkSetAsnWrite PROTO((MarkedLinkSetPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


/*****************************************************************************
*
*   Named-List
*
*****************************************************************************/

typedef struct {
	DocType type;
	DocField fld;
	CharPtr term;
	EntrezIdsPtr uids;
} NamedList, PNTR NamedListPtr;

extern NamedListPtr CDECL NamedListNew PROTO((void));
extern NamedListPtr CDECL NamedListFree PROTO(( NamedListPtr ufp));
extern NamedListPtr CDECL NamedListAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL NamedListAsnWrite PROTO((NamedListPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


/*****************************************************************************
*
*   Term-by-Page
*
*****************************************************************************/

typedef struct {
	DocType type;
	DocField fld;
	Int4 page;
	Int4 num_pages;
} TermByPage, PNTR TermByPagePtr;

extern TermByPagePtr CDECL TermByPageNew PROTO((void));
extern TermByPagePtr CDECL TermByPageFree PROTO(( TermByPagePtr ufp));
extern TermByPagePtr CDECL TermByPageAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL TermByPageAsnWrite PROTO((TermByPagePtr ufp, AsnIoPtr aip, AsnTypePtr atp));


/*****************************************************************************
*
*   Term-by-Term
*
*****************************************************************************/

typedef struct {
	DocType type;
	DocField fld;
	CharPtr term;
	Int4 num_terms;
} TermByTerm, PNTR TermByTermPtr;

extern TermByTermPtr CDECL TermByTermNew PROTO((void));
extern TermByTermPtr CDECL TermByTermFree PROTO(( TermByTermPtr ufp));
extern TermByTermPtr CDECL TermByTermAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL TermByTermAsnWrite PROTO((TermByTermPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Term-Lookup
*
*****************************************************************************/

typedef struct {
	DocType type;
	DocField fld;
	CharPtr term;
} TermLookup, PNTR TermLookupPtr;

extern TermLookupPtr CDECL TermLookupNew PROTO((void));
extern TermLookupPtr CDECL TermLookupFree PROTO(( TermLookupPtr ufp));
extern TermLookupPtr CDECL TermLookupAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL TermLookupAsnWrite PROTO((TermLookupPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


/*****************************************************************************
*
*   Term-Page-Info
*
*****************************************************************************/

typedef struct {
	CharPtr term;
	Int4 spec_count;
	Int4 tot_count;
} TermPageInfo, PNTR TermPageInfoPtr;

extern TermPageInfoPtr CDECL TermPageInfoNew PROTO((void));
extern TermPageInfoPtr CDECL TermPageInfoFree PROTO(( TermPageInfoPtr ufp));
extern TermPageInfoPtr CDECL TermPageInfoAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL TermPageInfoAsnWrite PROTO((TermPageInfoPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


/*****************************************************************************
*
*   Term-Counts
*
*****************************************************************************/

typedef struct {
	Boolean found;
	Int4 spec_count;
	Int4 tot_count;
} TermCounts, PNTR TermCountsPtr;

extern TermCountsPtr CDECL TermCountsNew PROTO((void));
extern TermCountsPtr CDECL TermCountsFree PROTO(( TermCountsPtr ufp));
extern TermCountsPtr CDECL TermCountsAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL TermCountsAsnWrite PROTO((TermCountsPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Link-SetGet
*
*****************************************************************************/

typedef struct {
	Int4 max;      /* maximum Ids to return */
	DocType link_to_cls;
	DocType query_cls;
	Boolean mark_missing;
	Int4 query_size;
	DocUidPtr query;
} LinkSetGet, PNTR LinkSetGetPtr;

extern LinkSetGetPtr CDECL LinkSetGetNew PROTO((void));
extern LinkSetGetPtr CDECL LinkSetGetFree PROTO(( LinkSetGetPtr ufp));
extern LinkSetGetPtr CDECL LinkSetGetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL LinkSetGetAsnWrite PROTO((LinkSetGetPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

extern DocSumPtr CDECL MlSumNew PROTO((void));
extern DocSumPtr CDECL MlSumFree PROTO(( DocSumPtr ufp));
extern DocSumPtr CDECL MlSumAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL MlSumAsnWrite PROTO((DocSumPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

extern DocSumPtr CDECL SeqSumNew PROTO((void));
extern DocSumPtr CDECL SeqSumFree PROTO(( DocSumPtr ufp));
extern DocSumPtr CDECL SeqSumAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL SeqSumAsnWrite PROTO((DocSumPtr ufp, AsnIoPtr aip, AsnTypePtr atp));



/*****************************************************************************
*
*   EntrezDocGet
*
*****************************************************************************/


typedef struct {
	DocType cls;
	Boolean mark_missing;
	EntrezIdsPtr ids;
	Int4 defer_count;
} EntrezDocGet, PNTR EntrezDocGetPtr;

extern EntrezDocGetPtr CDECL EntrezDocGetNew PROTO((void));
extern EntrezDocGetPtr CDECL EntrezDocGetFree PROTO(( EntrezDocGetPtr ufp));
extern EntrezDocGetPtr CDECL EntrezDocGetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL EntrezDocGetAsnWrite PROTO((EntrezDocGetPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


/*****************************************************************************
*
*   Medline-list
*
*****************************************************************************/
typedef struct {
	Int4 num;
	MedlineEntryPtr PNTR data; /* vector */
	EntrezIdsPtr marked_missing; /* marked UIDs, optional */
} MedlineEntryList, PNTR MedlineEntryListPtr;


extern MedlineEntryListPtr CDECL MedlineEntryListNew PROTO((void));
extern MedlineEntryListPtr CDECL MedlineEntryListFree PROTO(( MedlineEntryListPtr ufp));
extern MedlineEntryListPtr CDECL MedlineEntryListAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL MedlineEntryListAsnWrite PROTO((MedlineEntryListPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Ml-summary-list
*
*****************************************************************************/
typedef struct {
	Int4 num;
	DocSumPtr PNTR data; /* vector */
} MlSummaryList, PNTR MlSummaryListPtr;


extern MlSummaryListPtr CDECL MlSummaryListNew PROTO((void));
extern MlSummaryListPtr CDECL MlSummaryListFree PROTO(( MlSummaryListPtr ufp));
extern MlSummaryListPtr CDECL MlSummaryListAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL MlSummaryListAsnWrite PROTO((MlSummaryListPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   SeqEntryGet
*
*****************************************************************************/

typedef struct {
	EntrezIdsPtr ids;
	Int4 retype;
	Boolean mark_missing;
} EntrezSeqGet, PNTR EntrezSeqGetPtr;

extern EntrezSeqGetPtr CDECL EntrezSeqGetNew PROTO((void));
extern EntrezSeqGetPtr CDECL EntrezSeqGetFree PROTO(( EntrezSeqGetPtr ufp));
extern EntrezSeqGetPtr CDECL EntrezSeqGetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL EntrezSeqGetAsnWrite PROTO((EntrezSeqGetPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Seq-summary-list
*
*****************************************************************************/
typedef struct {
	Int4 num;
	DocSumPtr PNTR data; /* vector */
} SeqSummaryList, PNTR SeqSummaryListPtr;


extern SeqSummaryListPtr CDECL SeqSummaryListNew PROTO((void));
extern SeqSummaryListPtr CDECL SeqSummaryListFree PROTO(( SeqSummaryListPtr ufp));
extern SeqSummaryListPtr CDECL SeqSummaryListAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL SeqSummaryListAsnWrite PROTO((SeqSummaryListPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


/*****************************************************************************
*
*   Seq-entry-list
*
*****************************************************************************/
typedef struct {
	Int4 num;
	SeqEntryPtr PNTR data; /* vector */
	EntrezIdsPtr marked_missing; /* marked UIDs, optional */
} PNTR SeqEntryListPtr;
/* note that SeqEntryList is not provided because the name conflicted with */
/* the name of a function on sequtil.c; the typedef was not needed anyhow  */


extern SeqEntryListPtr CDECL SeqEntryListNew PROTO((void));
extern SeqEntryListPtr CDECL SeqEntryListFree PROTO(( SeqEntryListPtr ufp));
extern SeqEntryListPtr CDECL SeqEntryListAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean CDECL SeqEntryListAsnWrite PROTO((SeqEntryListPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

#endif
