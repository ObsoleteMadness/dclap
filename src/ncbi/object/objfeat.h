/*  objfeat.h
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
* File Name:  objfeat.h
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.3 $
*
* File Description:  Object manager interface for module NCBI-SeqFeat
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_Seqfeat_
#define _NCBI_Seqfeat_

#ifndef _ASNTOOL_
#include <asn.h>
#endif
#ifndef _NCBI_General_
#include <objgen.h>
#endif
#ifndef _NCBI_Seqloc_
#include <objloc.h>
#endif
#ifndef _NCBI_Pub_
#include <objpub.h>
#endif
#ifndef _NCBI_Pubdesc_
#include <objpubd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
Boolean LIBCALL SeqFeatAsnLoad PROTO((void));

/*****************************************************************************
*
*   GBQual
*
*****************************************************************************/
typedef struct gbqual {
    CharPtr qual,
        val;
    struct gbqual PNTR next;
} GBQual, PNTR GBQualPtr;

GBQualPtr LIBCALL GBQualNew PROTO((void));
Boolean   LIBCALL GBQualAsnWrite PROTO((GBQualPtr gbp, AsnIoPtr aip, AsnTypePtr atp));
GBQualPtr LIBCALL GBQualAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
GBQualPtr LIBCALL GBQualFree PROTO((GBQualPtr gbp));

/*****************************************************************************
*
*   SeqFeatXref
*   	cross references between features
*
*****************************************************************************/
typedef struct seqfeatxref {
    Choice id;      
    Choice data;
    struct seqfeatxref PNTR next;
} SeqFeatXref, PNTR SeqFeatXrefPtr;

SeqFeatXrefPtr LIBCALL SeqFeatXrefNew PROTO((void));
Boolean        LIBCALL SeqFeatXrefAsnWrite PROTO((SeqFeatXrefPtr sfxp, AsnIoPtr aip, AsnTypePtr atp));
SeqFeatXrefPtr LIBCALL SeqFeatXrefAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
SeqFeatXrefPtr LIBCALL SeqFeatXrefFree PROTO((SeqFeatXrefPtr sfxp));
					  /* free frees whole chain of SeqFeatXref */
/*****************************************************************************
*
*   SeqFeat
*     Feat-id is built into idtype/id
*       1=gibb (in id.intvalue)
*       2=gimm (id.ptrvalue)
*       3=local (id.ptrvalue to Object-id)
*       4=general (id.ptrvalue to Dbtag)
*     SeqFeatData is built into datatype/data
*       datatype gives type of SeqFeatData:
*   0 = not set
    1 = gene, data.value.ptrvalue = Gene-ref ,
    2 = org , data.value.ptrvalue = Org-ref ,
    3 = cdregion, data.value.ptrvalue = Cdregion ,
    4 = prot , data.value.ptrvalue = Prot-ref ,
    5 = rna, data.value.ptrvalue = RNA-ref ,
    6 = pub, data.value.ptrvalue = Pubdesc ,  -- publication applies to this seq 
    7 = seq, data.value.ptrvalue = Seq-loc ,  -- for tracking source of a seq.
    8 = imp, data.value.ptrvalue = Imp-feat ,
    9 = region, data.value.ptrvalue= VisibleString,      -- for a name
    10 = comment, data.value.ptrvalue= NULL ,             -- just a comment
    11 = bond, data.value.intvalue = ENUMERATED {
        disulfide (1) ,
        thiolester (2) ,
        xlink (3) ,
        other (255) } ,
    12 = site, data.value.intvalue = ENUMERATED {
        active (1) ,
        binding (2) ,
        cleavage (3) ,
        inhibit (4) ,
        modified (5),
        other (255) } ,
    13 = rsite, data.value.ptrvalue = Rsite-ref
    14 = user, data.value.ptrvalue = UserObjectPtr
    15 = txinit, data.value.ptrvalue = TxinitPtr
	16 = num, data.value.ptrvalue = NumberingPtr   -- a numbering system
	17 = psec-str data.value.intvalue = ENUMERATED {   -- protein secondary structure
		helix (1) ,         -- any helix
		sheet (2) ,         -- beta sheet
		turn  (3) } ,       -- beta or gamma turn
	18 = non-std-residue data.value.ptrvalue = VisibleString ,  -- non-standard residue here in seq
	19 = het data.value.ptrvalue=CharPtr Heterogen   -- cofactor, prosthetic grp, etc, bound to seq
*   
*
*****************************************************************************/
typedef struct seqfeat {
    Choice id;      
    Choice data;
    Boolean partial ,
        excpt;         /* except changed to excpt (Win32 reserved word) */
    CharPtr comment;
    ValNodePtr product ,
        location;
    GBQualPtr qual;
    CharPtr title;
    UserObjectPtr ext;
    ValNodePtr cit;       /* citations (Pub-set)  */
	Uint1 exp_ev;
	SeqFeatXrefPtr xref;
    struct seqfeat PNTR next;
} SeqFeat, PNTR SeqFeatPtr;

SeqFeatPtr LIBCALL SeqFeatNew PROTO((void));
Boolean    LIBCALL SeqFeatAsnWrite PROTO((SeqFeatPtr anp, AsnIoPtr aip, AsnTypePtr atp));
SeqFeatPtr LIBCALL SeqFeatAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
SeqFeatPtr LIBCALL SeqFeatFree PROTO((SeqFeatPtr anp));

     /* get a SeqFeatXref from a feature.  Currently only Prot-ref and */
     /* Gene-ref are supported */

SeqFeatXrefPtr LIBCALL SeqFeatToXref PROTO((SeqFeatPtr sfp));

/*****************************************************************************
*
*   SeqFeatId - used as parts of other things, so is not allocated itself
*
*****************************************************************************/
void    LIBCALL SeqFeatIdFree PROTO((ChoicePtr cp));  /* does NOT free cp itself */
Boolean LIBCALL SeqFeatIdAsnWrite PROTO((ChoicePtr cp, AsnIoPtr aip, AsnTypePtr orig));
Boolean LIBCALL SeqFeatIdAsnRead PROTO((AsnIoPtr aip, AsnTypePtr orig, ChoicePtr cp));
       /** NOTE: SeqFeatIdAsnRead() does NOT allocate cp ***/
Boolean LIBCALL SeqFeatIdDup PROTO((ChoicePtr dest, ChoicePtr src));

/*****************************************************************************
*
*   SeqFeatData - used as parts of other things, so is not allocated itself
*
*****************************************************************************/
void    LIBCALL SeqFeatDataFree PROTO((ChoicePtr cp));  /* does NOT free cp itself */
Boolean LIBCALL SeqFeatDataAsnWrite PROTO((ChoicePtr cp, AsnIoPtr aip, AsnTypePtr orig));
Boolean LIBCALL SeqFeatDataAsnRead PROTO((AsnIoPtr aip, AsnTypePtr orig, ChoicePtr cp));
       /** NOTE: SeqFeatDataAsnRead() does NOT allocate cp ***/

/*****************************************************************************
*
*   SeqFeatSet - sets of seqfeats
*
*****************************************************************************/
Boolean    LIBCALL SeqFeatSetAsnWrite PROTO((SeqFeatPtr anp, AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element));
SeqFeatPtr LIBCALL SeqFeatSetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element));

/*****************************************************************************
*
*   CodeBreak
*
*****************************************************************************/
typedef struct cb {
    SeqLocPtr loc;          /* the Seq-loc */
    Choice aa;              /* 1=ncbieaa, 2=ncbi8aa, 3=ncbistdaa */
    struct cb PNTR next;
} CodeBreak, PNTR CodeBreakPtr;

CodeBreakPtr LIBCALL CodeBreakNew PROTO((void));
Boolean      LIBCALL CodeBreakAsnWrite PROTO((CodeBreakPtr cbp, AsnIoPtr aip, AsnTypePtr atp));
CodeBreakPtr LIBCALL CodeBreakAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
CodeBreakPtr LIBCALL CodeBreakFree PROTO((CodeBreakPtr cbp));

/*****************************************************************************
*
*   CdRegion
*
*****************************************************************************/
typedef struct cdregion {
    Boolean orf;
    Uint1 frame;
    Boolean conflict;
    Uint1 gaps,                         /* 255 = any number > 254 */
        mismatch,
        stops;
    ValNodePtr genetic_code;                 /* NULL = not set */
    CodeBreakPtr code_break;
} CdRegion, PNTR CdRegionPtr;

CdRegionPtr LIBCALL CdRegionNew PROTO((void));
Boolean     LIBCALL CdRegionAsnWrite PROTO((CdRegionPtr cdp, AsnIoPtr aip, AsnTypePtr atp));
CdRegionPtr LIBCALL CdRegionAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
CdRegionPtr LIBCALL CdRegionFree PROTO((CdRegionPtr cdp));

/*****************************************************************************
*
*   GeneticCode
*
*      ncbieaa, ncbi8aa, ncbistdaa
*      are arrays 64 cells long, where each cell gives the aa produced
*      by triplets coded by T=0, C=1, A=2, G=3
*      TTT = cell[0]
*      TTC = cell[1]
*      TTA = cell[2]
*      TTG = cell[3]
*      TCT = cell[4]
*      ((base1 * 16) + (base2 * 4) + (base3)) = cell in table
*
*      sncbieaa, sncbi8aa, sncbistdaa
*   	are arrays same as above, except the AA's they code for are only for
*   	the first AA of a peptide.  This accomdates alternate start codes.
*       If a codon is not a valid start, the cell contains the "gap" symbol
*       instead of an AA.
*
*      in both cases, IUPAC cannot be used because it has no symbol for
*       stop.
*   	
*
*   GeneticCode is a ValNodePtr so variable numbers of elements are
*   	easily accomodated.  A ValNodePtr with choice = 254 is the head
*       of the list.  It's elements are a chain of ValNodes beginning with
*       the data.ptrvalue of the GeneticCode (head).  GeneticCodeNew()
*       returns the head.
*   
*   Types in ValNodePtr->choice are:
*   	0 = not set
*   	1 = name (CharPtr in ptrvalue)
*   	2 = id	(in intvalue)
*   	3 = ncbieaa (CharPtr in ptrvalue)
*   	4 = ncbi8aa (ByteStorePtr in ptrvalue)
*   	5 = ncbistdaa (ByteStorePtr in ptrvalue)
*   	6 = sncbieaa (CharPtr in ptrvalue)
*   	7 = sncbi8aa (ByteStorePtr in ptrvalue)
*   	8 = sncbistdaa (ByteStorePtr in ptrvalue)
*   	255 = read unrecognized type, but passed ASN.1
*   
*****************************************************************************/
typedef ValNode GeneticCode, FAR *GeneticCodePtr;

GeneticCodePtr LIBCALL GeneticCodeNew PROTO((void));
Boolean        LIBCALL GeneticCodeAsnWrite PROTO((GeneticCodePtr gcp, AsnIoPtr aip, AsnTypePtr atp));
GeneticCodePtr LIBCALL GeneticCodeAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
GeneticCodePtr LIBCALL GeneticCodeFree PROTO((GeneticCodePtr gcp));

Boolean        LIBCALL GeneticCodeTableAsnWrite PROTO((GeneticCodePtr gcp, AsnIoPtr aip, AsnTypePtr atp));
GeneticCodePtr LIBCALL GeneticCodeTableAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));

GeneticCodePtr LIBCALL GeneticCodeFind PROTO((Int4 id, CharPtr name));
GeneticCodePtr LIBCALL GeneticCodeTableLoad PROTO((void));

/*****************************************************************************
*
*   ImpFeat
*
*****************************************************************************/
typedef struct impfeat {
    CharPtr key,
        loc,
        descr;
} ImpFeat, PNTR ImpFeatPtr;

ImpFeatPtr LIBCALL ImpFeatNew PROTO((void));
Boolean    LIBCALL ImpFeatAsnWrite PROTO((ImpFeatPtr ifp, AsnIoPtr aip, AsnTypePtr atp));
ImpFeatPtr LIBCALL ImpFeatAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
ImpFeatPtr LIBCALL ImpFeatFree PROTO((ImpFeatPtr ifp));

/*****************************************************************************
*
*   RnaRef
*    Choice used for extensions
*      0 = no extension
*      1 = name, ext.value.ptrvalue = CharPtr
*      2 = trna, ext.value.ptrvalue = tRNA
*
*****************************************************************************/
typedef struct rnaref {
    Uint1 type;
    Boolean pseudo;
    Choice ext;
} RnaRef, PNTR RnaRefPtr;

RnaRefPtr LIBCALL RnaRefNew PROTO((void));
Boolean   LIBCALL RnaRefAsnWrite PROTO((RnaRefPtr rrp, AsnIoPtr aip, AsnTypePtr atp));
RnaRefPtr LIBCALL RnaRefAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
RnaRefPtr LIBCALL RnaRefFree PROTO((RnaRefPtr rrp));

/*****************************************************************************
*
*   tRNA
*
*****************************************************************************/
typedef struct trna {
    Uint1 aatype,  /* 0=not set, 1=iupacaa, 2=ncbieaa, 3=ncbi8aa 4=ncbistdaa */
        aa;        /* the aa transferred in above code */
    Uint1 codon[6];    /* codons recognized, coded as for Genetic-code */
} tRNA, PNTR tRNAPtr;   /*  0-63 = codon,  255=no data in cell */

/*****************************************************************************
*
*   GeneRef
*
*****************************************************************************/
typedef struct generef {
    CharPtr locus,
        allele,
        desc,
        maploc;
    Boolean pseudo;
    ValNodePtr db;          /* ids in other databases */
    ValNodePtr syn;         /* synonyms for locus */
} GeneRef, PNTR GeneRefPtr;

GeneRefPtr LIBCALL GeneRefNew PROTO((void));
Boolean    LIBCALL GeneRefAsnWrite PROTO((GeneRefPtr grp, AsnIoPtr aip, AsnTypePtr atp));
GeneRefPtr LIBCALL GeneRefAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
GeneRefPtr LIBCALL GeneRefFree PROTO((GeneRefPtr grp));
GeneRefPtr LIBCALL GeneRefDup PROTO((GeneRefPtr grp));

/*****************************************************************************
*
*   OrgRef
*
*****************************************************************************/
typedef struct orgref {
    CharPtr taxname,
        common;
    ValNodePtr mod;
    ValNodePtr db;          /* ids in other databases */
    ValNodePtr syn;         /* synonyms for taxname and/or common */
} OrgRef, PNTR OrgRefPtr;

OrgRefPtr LIBCALL OrgRefNew PROTO((void));
Boolean   LIBCALL OrgRefAsnWrite PROTO((OrgRefPtr orp, AsnIoPtr aip, AsnTypePtr atp));
OrgRefPtr LIBCALL OrgRefAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
OrgRefPtr LIBCALL OrgRefFree PROTO((OrgRefPtr orp));

/*****************************************************************************
*
*   ProtRef
*
*****************************************************************************/
typedef struct protref {
    ValNodePtr name;
    CharPtr desc;
    ValNodePtr ec,
        activity;
    ValNodePtr db;          /* ids in other databases */
} ProtRef, PNTR ProtRefPtr;

ProtRefPtr LIBCALL ProtRefNew PROTO((void));
Boolean    LIBCALL ProtRefAsnWrite PROTO((ProtRefPtr orp, AsnIoPtr aip, AsnTypePtr atp));
ProtRefPtr LIBCALL ProtRefAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
ProtRefPtr LIBCALL ProtRefFree PROTO((ProtRefPtr orp));
ProtRefPtr LIBCALL ProtRefDup PROTO((ProtRefPtr orp));

/*****************************************************************************
*
*   RsiteRef
*       uses an ValNode
*       choice = 1 = str
*                2 = db
*
*****************************************************************************/
typedef ValNode RsiteRef, FAR *RsiteRefPtr;

Boolean     LIBCALL RsiteRefAsnWrite PROTO((RsiteRefPtr orp, AsnIoPtr aip, AsnTypePtr atp));
RsiteRefPtr LIBCALL RsiteRefAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
RsiteRefPtr LIBCALL RsiteRefFree PROTO((RsiteRefPtr orp));

/*****************************************************************************
*
*   Txinit
*       Transcription initiation site
*
*****************************************************************************/
typedef struct txevidence {
    Uint1 exp_code ,
        exp_sys ;
    Boolean low_prec_data ,
        from_homolog;
    struct txevidence PNTR next;
} TxEvidence, PNTR TxEvidencePtr;

typedef struct txinit {
    CharPtr name;
    ValNodePtr syn ,
        gene ,
        protein ,
        rna ;
    CharPtr expression;
    Uint1 txsystem;
    CharPtr txdescr;
    OrgRefPtr txorg;
    Boolean mapping_precise,
        location_accurate;
    Uint1 inittype;              /* 255 if not set */
    TxEvidencePtr evidence;
} Txinit, PNTR TxinitPtr;

TxinitPtr LIBCALL TxinitNew PROTO((void));
Boolean   LIBCALL TxinitAsnWrite PROTO((TxinitPtr txp, AsnIoPtr aip, AsnTypePtr atp));
TxinitPtr LIBCALL TxinitAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
TxinitPtr LIBCALL TxinitFree PROTO((TxinitPtr txp));



#ifdef __cplusplus
}
#endif

#endif
