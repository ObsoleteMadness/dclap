/*   accentr.h
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
* File Name:  accentr.h
*
* Author:  Ostell
*
* Version Creation Date:   4/23/92
*
* $Revision: 2.11 $
*
* File Description: 
*       entrez index access library for Entrez
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/

#ifndef _ACCENTR_
#define _ACCENTR_

#ifndef _NCBI_Seqset_
#include <objsset.h>
#endif

#ifndef _NCBI_Medline_
#include <objmedli.h>
#endif

#ifndef _NCBI_Access_
#include <objacces.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* --- Type Definitions --- */

typedef Int4  DocUid;
typedef Int4Ptr DocUidPtr;
typedef Int2   DocType;
typedef Int2   DocField;


typedef struct fielddata {
    Int4    num_terms;                /* number of terms in this field */
    Int2    num_bucket;           /* number of buckets of terms */
} EntrezFieldData, PNTR EntrezFieldDataPtr;
    
typedef struct typedata {
    Int4    num;                   /* number of entries */
    Int4    num_uids;                  /* number of uids */
    DocUid  minuid;                /* minimum uid for this type */
    DocUid  maxuid;                /* maxuid for this type */
    Int2    num_bucket;            /* number of uid buckets */
    EntrezFieldDataPtr fields;
} EntrezTypeData, PNTR EntrezTypeDataPtr;
    
typedef struct _EntrezInfo {
    CharPtr volume_label;
    Int2    version;
    Int2    issue;
    Int2    format;
    CharPtr descr;
    Boolean no_compression;
    Int2    huff_count;
    Int2Ptr huff_left;
    Int2Ptr huff_right;
    Int2    type_count;
    CharPtr PNTR type_names;
    Int2    type_bucket_size;
    Int2    field_count;
    CharPtr PNTR field_names;
    Int2    field_bucket_size;
    EntrezTypeDataPtr types;
} EntrezInfo, PNTR EntrezInfoPtr;

/*****************************************************************************
*
*   PreDefined Entrez types and fields
*
*****************************************************************************/

/* doc type codes */
#define NTYPE       3   /* number of types == 3 */
#define TYP_ML      0   /*  Medline-entry */
#define TYP_AA      1   /*  amino acid data   */
#define TYP_NT      2   /*  nucleic acid data */
#define TYP_SEQ     4   /*  either aa or na used only for uid lookups */

/* field codes */
#define NFLD        10  /* number of fields == 10 */
#define FLD_WORD    0   /*  Words           */
#define FLD_MESH    1   /*  MeSH terms      */
#define FLD_KYWD    2   /*  Keyword         */
#define FLD_AUTH    3   /*  Authors         */
#define FLD_JOUR    4   /*  Journal title   */
#define FLD_ORGN    5   /*  Organism        */
#define FLD_ACCN    6   /*  Accession number */
#define FLD_GENE    7   /*  Gene Symbol     */
#define FLD_PROT    8   /*  Protein name    */
#define FLD_ECNO    9   /*  E.C. number     */

typedef struct docsum {
    DocUid uid;
    Boolean no_abstract,
        translated_title,
        no_authors;
    CharPtr caption,
        title;
} DocSum, PNTR DocSumPtr;

typedef Boolean (*DocSumListCallBack) PROTO((DocSumPtr dsp, DocUid uid));

#define NULLSYM     0     /* for building booleans */
#define LPAREN      1
#define RPAREN      2
#define ANDSYMBL    3
#define ORSYMBL     4
#define BUTNOTSYMBL 5
#define SPECIALTERM 6
#define TOTALTERM   7


/**** Initialize and close session *********************/

/* Note:                                                                      */
/*   The EntrezInitWithExtras() capability is provided for backwards          */
/*   compatability until May, 1994.  This name was introduced as a temporary  */
/*   interface to EntrezInit(), to smooth the transition for EntrezInit()'s   */
/*   use of three parameters; previously it took no parameters.               */
/*                                                                            */
/*                                  - J. Epstein, 17 Feb 1994                 */

#define EntrezInitWithExtras(a,b,c) EntrezInit(a,b,c)
Boolean LIBCALL EntrezInit PROTO((CharPtr appl_id, Boolean no_warnings, BoolPtr is_network));
void LIBCALL EntrezFini PROTO((void));

/**** Get names and numbers of fields and types ********/

EntrezInfoPtr LIBCALL EntrezGetInfo PROTO((void));

EntrezInfoPtr LIBCALL EntrezInfoAsnRead PROTO((AsnIoPtr aip, AsnTypePtr orig));
EntrezInfoPtr LIBCALL EntrezInfoFree PROTO((EntrezInfoPtr cip));

/**** Creates a term node from the uid parameter ********/
void LIBCALL EntrezCreateNamedUidList PROTO((CharPtr term, DocType type, DocField field, Int4 num, DocUidPtr uids));

/**** Used only by Entrez network server ***************/
Boolean LIBCALL EntrezInfoAsnWrite PROTO((EntrezInfoPtr p, AsnIoPtr aip, AsnTypePtr orig));

/**** Get detailed text information about the current status *****/
CharPtr LIBCALL EntrezDetailedInfo PROTO((void));

/**** Get Links and Neighbors **************************/

Int4 LIBCALL EntrezGetMaxLinks PROTO((void));
Int4 LIBCALL EntrezSetUserMaxLinks PROTO((Int4 usermax));
Int4 LIBCALL EntrezGetUserMaxLinks PROTO((void));

LinkSetPtr LIBCALL EntrezUidLinks PROTO((DocType type, DocUid uid, DocType link_to_type));
Int2 LIBCALL EntrezLinkUidList PROTO((LinkSetPtr PNTR result, DocType type,
	DocType link_to_type, Int2 numuid, Int4Ptr uids, Boolean mark_missing));

/**** Get Summaries ************************************/

DocSumPtr LIBCALL EntrezDocSum PROTO((DocType type, DocUid uid));
DocSumPtr LIBCALL DocSumFree PROTO((DocSumPtr dsp));
Int2 LIBCALL EntrezDocSumListGet PROTO((Int2 numuid, DocType type, DocUidPtr uids, DocSumListCallBack callback));
Int2 LIBCALL EntrezMlSumListGet PROTO((DocSumPtr PNTR result, Int2 numuid, Int4Ptr uids));
Int2 LIBCALL EntrezSeqSumListGet PROTO((DocSumPtr PNTR result, Int2 numuid, DocType type, Int4Ptr uids));

/**** Get Term Lists ***********************************/

typedef Boolean (*TermListProc) PROTO((CharPtr term, Int4 special, Int4 total));

Int2 LIBCALL EntrezTermListByPage PROTO((DocType type, DocField field, Int2 page, Int2 numpage, TermListProc proc));
Int2 LIBCALL EntrezTermListByTerm PROTO((DocType type, DocField field, CharPtr term, Int2 numterms, TermListProc proc, Int2Ptr first_page));

Boolean LIBCALL EntrezFindTerm PROTO((DocType type, DocField field, CharPtr term, Int4Ptr spcl, Int4Ptr totl));

/**** Look up terms with Boolean operations ************/

ValNodePtr LIBCALL EntrezTLNew PROTO((DocType type));
ValNodePtr LIBCALL EntrezTLAddTerm PROTO((ValNodePtr elst, CharPtr term, DocType type, DocField field, Boolean special));
ValNodePtr LIBCALL EntrezTLAddLParen PROTO((ValNodePtr elst));
ValNodePtr LIBCALL EntrezTLAddRParen PROTO((ValNodePtr elst));
ValNodePtr LIBCALL EntrezTLAddAND PROTO((ValNodePtr elst));
ValNodePtr LIBCALL EntrezTLAddOR PROTO((ValNodePtr elst));
ValNodePtr LIBCALL EntrezTLAddBUTNOT PROTO((ValNodePtr elst));
ValNodePtr LIBCALL EntrezTLFree PROTO((ValNodePtr elst));
LinkSetPtr LIBCALL EntrezTLEval PROTO((ValNodePtr elst));

/**** Look Up a Uid from a SeqId using the Terms list ****/

Int4 LIBCALL EntrezFindSeqId PROTO((SeqIdPtr sip));

/**** Get Sequence or MEDLINE data **********************/

Int2 LIBCALL EntrezSeqEntryListGet PROTO((SeqEntryPtr PNTR result, Int2 numuid, Int4Ptr uids, Int2 retcode, Boolean mark_missing));
SeqEntryPtr LIBCALL EntrezSeqEntryGet PROTO((Int4 uid, Int2 retcode));

Int2 LIBCALL EntrezMedlineEntryListGet PROTO((MedlineEntryPtr PNTR result, Int2 numuid, Int4Ptr uids, Boolean mark_missing));
MedlineEntryPtr LIBCALL EntrezMedlineEntryGet PROTO((Int4 uid));

#ifdef __cplusplus
}
#endif

#endif


