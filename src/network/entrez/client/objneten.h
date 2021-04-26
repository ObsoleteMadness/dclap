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
* $Revision: 4.0 $
*
* File Description:  Object loader interface for module NCBI-Entrez
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 8-16-94  Brylawski   Added declarations for EntrezNeighborText and
*                      EntrezHierarchy loaders and structures.
*
* 11-20-94 Brylawski   Moved the EntrezNeighborText structure to accentr.h
*                      to permit its use in chosen.c and netentr.c .
* ==========================================================================
*
*
* RCS Modification History:
* $Log: objneten.h,v $
 * Revision 4.0  1995/07/26  13:54:59  ostell
 * force revision to 4.0
 *
 * Revision 1.14  1995/07/11  12:30:30  epstein
 * change CDECLs to LIBCALLs
 *
 * Revision 1.13  1995/07/10  19:39:04  epstein
 * add docsumX
 *
 * Revision 1.12  1995/06/23  13:21:16  kans
 * include <accentr.h> to pull in Biostruc_supported symbol
 *
 * Revision 1.11  1995/05/17  17:53:35  epstein
 * add RCS log revision history
 *
*/

#ifndef _NCBI_NetEntrez_
#define _NCBI_NetEntrez_

#ifndef _ASNTOOL_
#include <asn.h>
#endif

/* accentr.h is included to define Biostruc_supported */
#ifndef _ACCENTR_
#include <accentr.h>
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

extern EntrezIdsPtr LIBCALL EntrezIdsNew PROTO((void));
extern EntrezIdsPtr LIBCALL EntrezIdsFree PROTO(( EntrezIdsPtr ufp));
extern EntrezIdsPtr LIBCALL EntrezIdsAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL EntrezIdsAsnWrite PROTO((EntrezIdsPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


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


extern MarkedLinkSetPtr LIBCALL MarkedLinkSetNew PROTO((void));
extern MarkedLinkSetPtr LIBCALL MarkedLinkSetFree PROTO(( MarkedLinkSetPtr ufp));
extern MarkedLinkSetPtr LIBCALL MarkedLinkSetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL MarkedLinkSetAsnWrite PROTO((MarkedLinkSetPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


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

extern NamedListPtr LIBCALL NamedListNew PROTO((void));
extern NamedListPtr LIBCALL NamedListFree PROTO(( NamedListPtr ufp));
extern NamedListPtr LIBCALL NamedListAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL NamedListAsnWrite PROTO((NamedListPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


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

extern TermByPagePtr LIBCALL TermByPageNew PROTO((void));
extern TermByPagePtr LIBCALL TermByPageFree PROTO(( TermByPagePtr ufp));
extern TermByPagePtr LIBCALL TermByPageAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL TermByPageAsnWrite PROTO((TermByPagePtr ufp, AsnIoPtr aip, AsnTypePtr atp));


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

extern TermByTermPtr LIBCALL TermByTermNew PROTO((void));
extern TermByTermPtr LIBCALL TermByTermFree PROTO(( TermByTermPtr ufp));
extern TermByTermPtr LIBCALL TermByTermAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL TermByTermAsnWrite PROTO((TermByTermPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

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

extern TermLookupPtr LIBCALL TermLookupNew PROTO((void));
extern TermLookupPtr LIBCALL TermLookupFree PROTO(( TermLookupPtr ufp));
extern TermLookupPtr LIBCALL TermLookupAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL TermLookupAsnWrite PROTO((TermLookupPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


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

extern TermPageInfoPtr LIBCALL TermPageInfoNew PROTO((void));
extern TermPageInfoPtr LIBCALL TermPageInfoFree PROTO(( TermPageInfoPtr ufp));
extern TermPageInfoPtr LIBCALL TermPageInfoAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL TermPageInfoAsnWrite PROTO((TermPageInfoPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


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

extern TermCountsPtr LIBCALL TermCountsNew PROTO((void));
extern TermCountsPtr LIBCALL TermCountsFree PROTO(( TermCountsPtr ufp));
extern TermCountsPtr LIBCALL TermCountsAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL TermCountsAsnWrite PROTO((TermCountsPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

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

extern LinkSetGetPtr LIBCALL LinkSetGetNew PROTO((void));
extern LinkSetGetPtr LIBCALL LinkSetGetFree PROTO(( LinkSetGetPtr ufp));
extern LinkSetGetPtr LIBCALL LinkSetGetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL LinkSetGetAsnWrite PROTO((LinkSetGetPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

extern DocSumPtr LIBCALL MlSumNew PROTO((void));
extern DocSumPtr LIBCALL MlSumFree PROTO(( DocSumPtr ufp));
extern DocSumPtr LIBCALL MlSumAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL MlSumAsnWrite PROTO((DocSumPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

extern DocSumPtr LIBCALL SeqSumNew PROTO((void));
extern DocSumPtr LIBCALL SeqSumFree PROTO(( DocSumPtr ufp));
extern DocSumPtr LIBCALL SeqSumAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL SeqSumAsnWrite PROTO((DocSumPtr ufp, AsnIoPtr aip, AsnTypePtr atp));



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

extern EntrezDocGetPtr LIBCALL EntrezDocGetNew PROTO((void));
extern EntrezDocGetPtr LIBCALL EntrezDocGetFree PROTO(( EntrezDocGetPtr ufp));
extern EntrezDocGetPtr LIBCALL EntrezDocGetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL EntrezDocGetAsnWrite PROTO((EntrezDocGetPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


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


extern MedlineEntryListPtr LIBCALL MedlineEntryListNew PROTO((void));
extern MedlineEntryListPtr LIBCALL MedlineEntryListFree PROTO(( MedlineEntryListPtr ufp));
extern MedlineEntryListPtr LIBCALL MedlineEntryListAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL MedlineEntryListAsnWrite PROTO((MedlineEntryListPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Ml-summary-list
*
*****************************************************************************/
typedef struct {
	Int4 num;
	DocSumPtr PNTR data; /* vector */
} MlSummaryList, PNTR MlSummaryListPtr;


extern MlSummaryListPtr LIBCALL MlSummaryListNew PROTO((void));
extern MlSummaryListPtr LIBCALL MlSummaryListFree PROTO(( MlSummaryListPtr ufp));
extern MlSummaryListPtr LIBCALL MlSummaryListAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL MlSummaryListAsnWrite PROTO((MlSummaryListPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

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

extern EntrezSeqGetPtr LIBCALL EntrezSeqGetNew PROTO((void));
extern EntrezSeqGetPtr LIBCALL EntrezSeqGetFree PROTO(( EntrezSeqGetPtr ufp));
extern EntrezSeqGetPtr LIBCALL EntrezSeqGetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL EntrezSeqGetAsnWrite PROTO((EntrezSeqGetPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Seq-summary-list
*
*****************************************************************************/
typedef struct {
	Int4 num;
	DocSumPtr PNTR data; /* vector */
} SeqSummaryList, PNTR SeqSummaryListPtr;


extern SeqSummaryListPtr LIBCALL SeqSummaryListNew PROTO((void));
extern SeqSummaryListPtr LIBCALL SeqSummaryListFree PROTO(( SeqSummaryListPtr ufp));
extern SeqSummaryListPtr LIBCALL SeqSummaryListAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL SeqSummaryListAsnWrite PROTO((SeqSummaryListPtr ufp, AsnIoPtr aip, AsnTypePtr atp));


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


extern SeqEntryListPtr LIBCALL SeqEntryListNew PROTO((void));
extern SeqEntryListPtr LIBCALL SeqEntryListFree PROTO(( SeqEntryListPtr ufp));
extern SeqEntryListPtr LIBCALL SeqEntryListAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL SeqEntryListAsnWrite PROTO((SeqEntryListPtr ufp, AsnIoPtr aip, AsnTypePtr atp));



#ifdef Biostruc_supported

/*****************************************************************************
*
*   Biostruc-list
*
*****************************************************************************/
typedef struct {
	Int4 num;
	BiostrucPtr PNTR data; /* vector */
	EntrezIdsPtr marked_missing; /* marked UIDs, optional */
} BiostrucList, PNTR BiostrucListPtr;


extern BiostrucListPtr LIBCALL BiostrucListNew PROTO((void));
extern BiostrucListPtr LIBCALL BiostrucListFree PROTO(( BiostrucListPtr ufp));
extern BiostrucListPtr LIBCALL BiostrucListAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean LIBCALL BiostrucListAsnWrite PROTO((BiostrucListPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

#endif /* Biostruc_supported */

/**************************************************
*
*    EntrezNeighborText
*
**************************************************/


EntrezNeighborTextPtr LIBCALL EntrezNeighborTextAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL EntrezNeighborTextAsnWrite PROTO (( EntrezNeighborTextPtr , AsnIoPtr, AsnTypePtr));


/**************************************************
*
*    ChildLink
*
*************************************************/
typedef struct struct_Child_Link {
   struct struct_Child_Link PNTR next;
   CharPtr   name;
   Boolean   isLeafNode;
   Int4   special;
   Int4   total;
} ChildLink, PNTR ChildLinkPtr;



EntrezHierarchyChildPtr LIBCALL EntrezHierarchyChildFree PROTO ((EntrezHierarchyChildPtr ));
EntrezHierarchyChildPtr LIBCALL EntrezHierarchyChildNew PROTO (( void ));
EntrezHierarchyChildPtr LIBCALL EntrezHierarchyChildAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL EntrezHierarchyChildAsnWrite PROTO (( EntrezHierarchyChildPtr ,AsnIoPtr, AsnTypePtr));

EntrezHierarchyPtr LIBCALL EntrezHierarchyFree PROTO ((EntrezHierarchyPtr ));
EntrezHierarchyPtr LIBCALL EntrezHierarchyNew PROTO (( void ));
EntrezHierarchyPtr LIBCALL EntrezHierarchyAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL EntrezHierarchyAsnWrite PROTO (( EntrezHierarchyPtr , 
                                                  AsnIoPtr, AsnTypePtr));


/**************************************************
*
*    EntrezBlastreq
*
**************************************************/
typedef struct struct_Entrez_blastreq {
   Uint4 OBbits__;
   BioseqPtr   bsp;
   DocType bsp_database;
   CharPtr   program;
   CharPtr   database;
   CharPtr   options;
   Uint1     showprogress;
} EntrezBlastreq, PNTR EntrezBlastreqPtr;


EntrezBlastreqPtr LIBCALL EntrezBlastreqFree PROTO ((EntrezBlastreqPtr ));
EntrezBlastreqPtr LIBCALL EntrezBlastreqNew PROTO (( void ));
EntrezBlastreqPtr LIBCALL EntrezBlastreqAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL EntrezBlastreqAsnWrite PROTO (( EntrezBlastreqPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    EntrezExtraInfo
*
**************************************************/
typedef struct struct_Entrez_extra_info {
   Uint4 OBbits__;
   Int4   maxlinks;
   Uint1   canneighbortext;
   Uint1   expanded_medline;
   Uint1   canblast;
} EntrezExtraInfo, PNTR EntrezExtraInfoPtr;


EntrezExtraInfoPtr LIBCALL EntrezExtraInfoFree PROTO ((EntrezExtraInfoPtr ));
EntrezExtraInfoPtr LIBCALL EntrezExtraInfoNew PROTO (( void ));
EntrezExtraInfoPtr LIBCALL EntrezExtraInfoAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL EntrezExtraInfoAsnWrite PROTO (( EntrezExtraInfoPtr , AsnIoPtr, AsnTypePtr));

typedef struct struct_New_summary_List {
   Int4   num;
   DocType type;
   DocSumPtr PNTR data;
} NewSummaryList, PNTR NewSummaryListPtr;

NewSummaryListPtr LIBCALL NewSummaryListFree PROTO ((NewSummaryListPtr ));
NewSummaryListPtr LIBCALL NewSummaryListNew PROTO (( void ));
NewSummaryListPtr LIBCALL NewSummaryListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL NewSummaryListAsnWrite PROTO (( NewSummaryListPtr , AsnIoPtr, AsnTypePtr));

 
 

#endif
