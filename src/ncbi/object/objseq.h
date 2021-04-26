/*  objseq.h
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
* File Name:  objseq.h
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.3 $
*
* File Description:  Object manager interface for module NCBI-Seq
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_Seq_
#define _NCBI_Seq_

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
#ifndef _NCBI_Seqalign_
#include <objalign.h>
#endif
#ifndef _NCBI_Pubdesc_
#include <objpubd.h>     /* separated out to avoid typedef order problems */
#endif
#ifndef _NCBI_Seqfeat_
#include <objfeat.h>       /* include organism for now */
#endif
#ifndef _NCBI_Seqres_
#include <objres.h>
#endif
#ifndef _NCBI_Access_
#include <objacces.h>
#endif
#ifndef _NCBI_SeqBlock_
#include <objblock.h>
#endif
#ifndef _NCBI_SeqCode_
#include <objcode.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
Boolean LIBCALL SeqAsnLoad PROTO((void));

/*****************************************************************************
*
*   internal structures for NCBI-Seq objects
*
*****************************************************************************/

/*****************************************************************************
*
*   SeqAnnot - Sequence annotations
*
*****************************************************************************/
typedef struct seqannot {
    ObjectIdPtr id;
    DbtagPtr db;
    CharPtr name,
        desc;
    Uint1 type;             /* 1=ftable, 2=align, 3=graph */
    Pointer data;
    struct seqannot PNTR next;
} SeqAnnot, PNTR SeqAnnotPtr;

SeqAnnotPtr LIBCALL SeqAnnotNew PROTO((void));
Boolean     LIBCALL SeqAnnotAsnWrite PROTO((SeqAnnotPtr sap, AsnIoPtr aip, AsnTypePtr atp));
SeqAnnotPtr LIBCALL SeqAnnotAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
SeqAnnotPtr LIBCALL SeqAnnotFree PROTO((SeqAnnotPtr sap));

/*****************************************************************************
*
*   Sets of SeqAnnots
*
*****************************************************************************/
Boolean     LIBCALL SeqAnnotSetAsnWrite PROTO((SeqAnnotPtr sap, AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element));
SeqAnnotPtr LIBCALL SeqAnnotSetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element));


/*****************************************************************************
*
*   SeqHist
*
*****************************************************************************/
typedef struct seqhist {
	SeqAlignPtr assembly;
	DatePtr replace_date;
	SeqIdPtr replace_ids;
	DatePtr replaced_by_date;
	SeqIdPtr replaced_by_ids;
	Boolean deleted;
	DatePtr deleted_date;
} SeqHist, PNTR SeqHistPtr;

SeqHistPtr LIBCALL SeqHistNew PROTO((void));
Boolean    LIBCALL SeqHistAsnWrite PROTO((SeqHistPtr shp, AsnIoPtr aip, AsnTypePtr atp));
SeqHistPtr LIBCALL SeqHistAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
SeqHistPtr LIBCALL SeqHistFree PROTO((SeqHistPtr shp));

/*****************************************************************************
*
*   Bioseq.
*     Inst is incorporated within Bioseq for efficiency
*     seq_data_type
*       0 = not set
*       1 = IUPACna
*       2 = IUPACaa
*       3 = NCBI2na
*       4 = NCBI4na
*       5 = NCBI8na
*       6 = NCBIpna
*       7 = NCBI8aa
*       8 = NCBIeaa
*       9 = NCBIpaa
*      11 = NCBIstdaa
*   seq_ext_type
*       0 = none
*       1 = seg-ext
*       2 = ref-ext
*       3 = map-ext
*
*****************************************************************************/
#define Seq_code_iupacna 1
#define Seq_code_iupacaa 2
#define Seq_code_ncbi2na 3
#define Seq_code_ncbi4na 4
#define Seq_code_ncbi8na 5
#define Seq_code_ncbipna 6
#define Seq_code_ncbi8aa 7
#define Seq_code_ncbieaa 8
#define Seq_code_ncbipaa 9
#define Seq_code_iupacaa3 10
#define Seq_code_ncbistdaa 11

#define Seq_repr_virtual 1
#define Seq_repr_raw    2
#define Seq_repr_seg    3
#define Seq_repr_const  4
#define Seq_repr_ref    5
#define Seq_repr_consen 6
#define Seq_repr_map    7
#define Seq_repr_other  255

#define Seq_mol_dna     1
#define Seq_mol_rna     2
#define Seq_mol_aa      3
#define Seq_mol_na      4
#define Seq_mol_other   255
#define ISA_na(x)  ((x==1)||(x==2)||(x==4))
#define ISA_aa(x)  (x == 3)

typedef struct bioseq {
    SeqIdPtr id;          /* Seq-ids */
    ValNodePtr descr;              /* Seq-descr */
    Uint1 repr,
        mol;
    Int4 length;            /* -1 if not set */
    IntFuzzPtr fuzz;
    Uint1 topology,
        strand,
        seq_data_type,      /* as in Seq_code_type above */
        seq_ext_type;
    ByteStorePtr seq_data;
    Pointer seq_ext;
    SeqAnnotPtr annot;
	SeqHistPtr hist;
} Bioseq, PNTR BioseqPtr;

BioseqPtr LIBCALL BioseqNew PROTO((void));
Boolean   LIBCALL BioseqAsnWrite PROTO((BioseqPtr bsp, AsnIoPtr aip, AsnTypePtr atp));
BioseqPtr LIBCALL BioseqAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
BioseqPtr LIBCALL BioseqFree PROTO((BioseqPtr bsp));

Boolean LIBCALL BioseqInstAsnWrite PROTO((BioseqPtr bsp, AsnIoPtr aip, AsnTypePtr orig));
Boolean LIBCALL BioseqInstAsnRead PROTO((BioseqPtr bsp, AsnIoPtr aip, AsnTypePtr orig));

BioseqPtr FAR* LIBCALL BioseqInMem PROTO((Int2Ptr numptr));

/*****************************************************************************
*
*   Initialize bioseq and seqcode tables and default numbering
*
*****************************************************************************/
Boolean LIBCALL BioseqLoad PROTO((void));

/*****************************************************************************
*
*   BioseqAsnRead Options
*
*****************************************************************************/

typedef struct op_objseq {
	SeqIdPtr sip;          /* seq id to find */
	Boolean found_it;      /* set to TRUE when BioseqAsnRead matches sip */
	Boolean load_by_id;    /* if TRUE, load only if sip matches */
} Op_objseq, PNTR Op_objseqPtr;

/* types for AsnIoOption OP_NCBIOBJSEQ */
#define BIOSEQ_CHECK_ID 1    /* match Op_objseq.sip */

/*****************************************************************************
*
*   SeqDescr uses an ValNode with choice = 
    1 = * mol-type GIBB-mol ,          -- type of molecule
    2 = ** modif SET OF GIBB-mod ,      -- modifiers
    3 = * method GIBB-method ,         -- sequencing method
    4 = name VisibleString ,         -- a name for this sequence
    5 = title VisibleString ,        -- a title for this sequence
    6 = org Org-ref ,                -- if all from one organism
    7 = comment VisibleString ,      -- a more extensive comment
    8 = num Numbering ,              -- a numbering system
    9 = maploc Dbtag ,               -- map location of this sequence
    10 = pir PIR-block ,              -- PIR specific info
    11 = genbank GB-block ,           -- GenBank specific info
    12 = pub Pubdesc                 -- a reference to the publication
    13 = region VisibleString        -- name for this region of sequence
    14 = user UserObject             -- user structured data object
    15 = sp SP-block                 -- SWISSPROT specific info
    16 = neighbors Entrez-link       -- links to sequence neighbors
	17 = embl EMBL-block             -- EMBL specific info
	18 = create-date Date            -- date entry created
	19 = update-date Date            -- date of last update
	20 = prf PRF-block 				 -- PRF specific information
	21 = pdb PDB-block              -- PDB specific information
	22 = het Heterogen              -- cofactor, etc associated but not bound

    types with * use data.intvalue.  Other use data.ptrvalue
    ** uses a chain of ValNodes which use data.intvalue for enumerated type
*
*****************************************************************************/
#define Seq_descr_mol_type 1
#define Seq_descr_modif 2
#define Seq_descr_method 3
#define Seq_descr_name 4
#define Seq_descr_title 5
#define Seq_descr_org 6
#define Seq_descr_comment 7
#define Seq_descr_num 8
#define Seq_descr_maploc 9
#define Seq_descr_pir 10
#define Seq_descr_genbank 11
#define Seq_descr_pub 12
#define Seq_descr_region 13
#define Seq_descr_user 14
#define Seq_descr_sp 15
#define Seq_descr_neighbors 16
#define Seq_descr_embl 17
#define Seq_descr_create_date 18
#define Seq_descr_update_date 19
#define Seq_descr_prf 20
#define Seq_descr_pdb 21
#define Seq_descr_het 22

typedef ValNode SeqDescr, FAR *SeqDescrPtr;

Boolean    LIBCALL SeqDescrAsnWrite PROTO((SeqDescrPtr anp, AsnIoPtr aip, AsnTypePtr atp));
SeqDescrPtr LIBCALL SeqDescrAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
SeqDescrPtr LIBCALL SeqDescrFree PROTO((SeqDescrPtr anp));

     /* this writes one SeqDescr element (Seq-descr.E) -- useful for */
     /* print template functions */

Boolean LIBCALL SeqDescrEAsnWrite PROTO((ValNodePtr anp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Pubdesc and Numbering types defined in objpubd.h
*
*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
