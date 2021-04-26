/*  objblock.h
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
* File Name:  objblock.h
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.2 $
*
* File Description:  Object manager for module GenBank-General,
*   					EMBL-General, PIR-General, SWISSPROT-General
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 05-17-93 Schuler     pdbblock.class renamed pdpblock.pdbclass because
*                      class is a C++ keyword.
*
*
* ==========================================================================
*/

#ifndef _NCBI_SeqBlock_
#define _NCBI_SeqBlock_

#ifndef _ASNTOOL_
#include <asn.h>
#endif
#ifndef _NCBI_General_
#include <objgen.h>
#endif
#ifndef _NCBI_Seqloc_
#include <objloc.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
Boolean LIBCALL SeqBlockAsnLoad PROTO((void));

/*****************************************************************************
*
*   PirBlock - PIR specific data elements
*
*****************************************************************************/
typedef struct pirblock {
    Boolean had_punct;
    CharPtr host,
        source,
        summary,
        genetic,
        includes,
        placement,
        superfamily,
        cross_reference,
        date,
        seq_raw;
    ValNodePtr keywords;
    SeqIdPtr seqref;
} PirBlock, PNTR PirBlockPtr;

PirBlockPtr LIBCALL PirBlockNew PROTO((void));
Boolean     LIBCALL PirBlockAsnWrite PROTO((PirBlockPtr pbp, AsnIoPtr aip, AsnTypePtr atp));
PirBlockPtr LIBCALL PirBlockAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
PirBlockPtr LIBCALL PirBlockFree PROTO((PirBlockPtr pbp));

/*****************************************************************************
*
*   GBBlock - GenBank specific data elements
*
*****************************************************************************/
typedef struct gbblock {
    ValNodePtr extra_accessions,
        keywords;
    CharPtr source,
        origin,
        date,
        div,
        taxonomy;
    DatePtr entry_date;
} GBBlock, PNTR GBBlockPtr;

GBBlockPtr LIBCALL GBBlockNew PROTO((void));
Boolean    LIBCALL GBBlockAsnWrite PROTO((GBBlockPtr gbp, AsnIoPtr aip, AsnTypePtr atp));
GBBlockPtr LIBCALL GBBlockAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
GBBlockPtr LIBCALL GBBlockFree PROTO((GBBlockPtr gbp));

/*****************************************************************************
*
*   SPBlock - SWISSPROT specific data elements
*
*****************************************************************************/
typedef struct spblock {
    Uint1 _class;
    ValNodePtr extra_acc;
    Boolean imeth;
    ValNodePtr plasnm;
    SeqIdPtr seqref;
    ValNodePtr dbref;
    ValNodePtr keywords;
	NCBI_DatePtr created,
		sequpd,
		annotupd;
} SPBlock, PNTR SPBlockPtr;

SPBlockPtr LIBCALL SPBlockNew PROTO((void));
Boolean    LIBCALL SPBlockAsnWrite PROTO((SPBlockPtr sbp, AsnIoPtr aip, AsnTypePtr atp));
SPBlockPtr LIBCALL SPBlockAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
SPBlockPtr LIBCALL SPBlockFree PROTO((SPBlockPtr sbp));

/*****************************************************************************
*
*   EMBLBlock - EMBL specific data elements
*
*****************************************************************************/
typedef struct emblxref {
	Uint1 _class;		
	CharPtr name;			/* NULL if class used */
	ValNodePtr id;          /* ValNode->data.ptrvalue is an ObjectIdPtr */
	struct emblxref PNTR next;
} EMBLXref, PNTR EMBLXrefPtr;

typedef struct emblblock {
    Uint1 _class,
		div;               /* 255 = not set */
	NCBI_DatePtr creation_date ,
		update_date;
    ValNodePtr extra_acc;
    ValNodePtr keywords;
	EMBLXrefPtr xref;
} EMBLBlock, PNTR EMBLBlockPtr;

EMBLBlockPtr LIBCALL EMBLBlockNew PROTO((void));
Boolean      LIBCALL EMBLBlockAsnWrite PROTO((EMBLBlockPtr ebp, AsnIoPtr aip, AsnTypePtr atp));
EMBLBlockPtr LIBCALL EMBLBlockAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
EMBLBlockPtr LIBCALL EMBLBlockFree PROTO((EMBLBlockPtr ebp));

/*****************************************************************************
*
*	PRF-Block	- PRF specific data emements
*				by A.Ogiwara
*****************************************************************************/
typedef struct prfextsrc {
	CharPtr		host;
	CharPtr		part;
	CharPtr		state;
	CharPtr		strain;
	CharPtr		taxon;
} PrfExtSrc, PNTR PrfExtSrcPtr;

typedef struct prfblock {
	PrfExtSrcPtr	extra_src;
	ValNodePtr	keywords;
} PrfBlock, PNTR PrfBlockPtr;

PrfBlockPtr	LIBCALL PrfBlockNew	PROTO((void));
Boolean		LIBCALL PrfBlockAsnWrite	PROTO((PrfBlockPtr pbp, AsnIoPtr aip,
						AsnTypePtr atp));
PrfBlockPtr	LIBCALL PrfBlockAsnRead	PROTO((AsnIoPtr aip, AsnTypePtr atp));
PrfBlockPtr	LIBCALL PrfBlockFree	PROTO((PrfBlockPtr pbp));


/*****************************************************************************
*
*	PDB-Block	- PDB specific data emements
*
*****************************************************************************/

typedef struct pdbreplace {
	DatePtr date;
	ValNodePtr ids;
}
PdbRep, PNTR PdbRepPtr;

typedef struct pdbblock {
	DatePtr deposition ;
	CharPtr pdbclass ;
	ValNodePtr compound ;
	ValNodePtr source ;
	CharPtr exp_method ;
	PdbRepPtr replace;
}
PdbBlock, PNTR PdbBlockPtr;

PdbBlockPtr	LIBCALL PdbBlockNew	PROTO((void));
Boolean		LIBCALL PdbBlockAsnWrite	PROTO((PdbBlockPtr pdbp, AsnIoPtr aip,
						AsnTypePtr atp));
PdbBlockPtr	LIBCALL PdbBlockAsnRead	PROTO((AsnIoPtr aip, AsnTypePtr atp));
PdbBlockPtr	LIBCALL PdbBlockFree	PROTO((PdbBlockPtr pdbp));

#ifdef __cplusplus
}
#endif

#endif

