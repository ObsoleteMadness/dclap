/*  asnlext.h
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
* File Name: asnlext.h
*
* Author:  James Ostell
*
* Version Creation Date: 1/1/91
*
* $Revision: 2.0 $
*
* File Description:
*   typedefs and prototypes used internally by asnio.c
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _ASNLEXT_
#define _ASNLEXT_

	/*************************************************************************
	*
	*   tokens returned by AsnLexTWord()
	*   	subset returned by AsnLexWord()
	*   	301 - 400  are reserved for asn1 primitive types and map
	*		   	to isa values
	*   		These are defined in asnlex.h
	*   	401 - 500  are used for symbols and other reserved words
	*   	    401-427 defined in asnlex.h
	*          this is a superset which includes ASN.1 additional reserved
	*   		words for parsing syntax specifications
	*       AsnLexTMatchToken()  depends on the order of these defines and
	*   		the strings in asnwords[]
	*
	*************************************************************************/

#define DEF_TOKEN		428     /* DEFINITIONS */
#define BEGIN_TOKEN		429		/* BEGIN */
#define END_TOKEN		430		/* END */
#define AP_TAG_TOKEN	431		/* APPLICATION */
#define PRIV_TAG_TOKEN	432		/* PRIVATE */
#define UNIV_TAG_TOKEN	433		/* UNIVERSAL */
#define COMPS_TOKEN		434		/* COMPONENTS */
#define DEFAULT_TOKEN	435		/* DEFAULT */
#define FALSE_TOKEN		436		/* FALSE */
#define TRUE_TOKEN		437		/* TRUE */
#define IMPLICIT_TOKEN 	438		/* IMPLICIT */
#define OPTIONAL_TOKEN	439		/* OPTIONAL */
#define EXPORTS_TOKEN	440		/* EXPORTS */
#define IMPORTS_TOKEN	441		/* IMPORTS */
#define ABSENT_TOKEN	442		/* ABSENT */
#define BY_TOKEN		443		/* BY */
#define COMP_TOKEN		444		/* COMPONENT */
#define DEFINE_TOKEN	445		/* DEFINED */
#define FROM_TOKEN		446		/* FROM */
#define INCLUDES_TOKEN	447		/* INCLUDES */
#define MIN_TOKEN		448		/* MIN */
#define MINUSI_TOKEN	449		/* MINUS-INFINITY */
#define MAX_TOKEN		450		/* MAX */
#define PRESENT_TOKEN	451		/* PRESENT */
#define PLUSI_TOKEN		452		/* PLUS-INFINITY */
#define SIZE_TOKEN		453		/* SIZE */
#define TAGS_TOKEN		454		/* TAGS */
#define WITH_TOKEN		455		/* WITH */
#define IDENT_TOKEN		456		/* IDENTIFIER (goes with OBJECT) */
#define REVISION_TOKEN  457     /* $Revision   (NCBI only) */

/*****************************************************************************
*
*   prototypes
*
*****************************************************************************/

extern AsnModulePtr AsnLexTReadModule PROTO((AsnIoPtr aip));
extern Int2 AsnLexTReadTypeAssign PROTO((AsnIoPtr aip, AsnModulePtr amp));
extern Int2 AsnLexTReadType PROTO((AsnIoPtr aip, AsnModulePtr amp, AsnTypePtr atp));
extern AsnTypePtr AsnLexTReadElementTypeList PROTO((AsnIoPtr aip, AsnModulePtr amp));
extern AsnTypePtr AsnLexTReadAlternativeTypeList PROTO((AsnIoPtr aip, AsnModulePtr amp));
extern AsnModulePtr AsnLexTStartModule PROTO((AsnIoPtr aip));
extern Int2 AsnLexTMatchToken PROTO((AsnIoPtr aip));
extern Int2 AsnLexTWord PROTO((AsnIoPtr aip));

extern AsnValxNodePtr AsnValxNodeNew PROTO((AsnValxNodePtr anvp, Int2 type));
extern AsnTypePtr AsnGetType PROTO((AsnIoPtr aip, AsnModulePtr amp));
extern AsnTypePtr AsnTypeNew PROTO((AsnIoPtr aip, AsnModulePtr amp));
extern AsnTypePtr AsnElementTypeNew PROTO((AsnIoPtr aip));
extern AsnModulePtr AsnCheckModule PROTO((AsnModulePtr amp, AsnIoPtr aip));
extern void AsnSetTags PROTO((AsnTypePtr atp));

#endif
