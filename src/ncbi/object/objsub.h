/*  objsub.h
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
* File Name:  objsub.h
*
* Author:  James Ostell
*   
* Version Creation Date: 6/20/92
*
* $Revision: 2.2 $
*
* File Description:  Object manager interface for module NCBI-Sub
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_Submit_
#define _NCBI_Submit_

#ifndef _NCBI_Seqset_
#include <objsset.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
Boolean LIBCALL SubmitAsnLoad PROTO((void));

/*****************************************************************************
*
*   internal structures for NCBI-Submit objects
*
*****************************************************************************/

/*****************************************************************************
*
*   Contact Info
*
*****************************************************************************/
typedef struct contact {
	CharPtr name;
	ValNodePtr address;            /* chain of strings */
	CharPtr phone,
		fax,
		email,
		telex;
	ObjectIdPtr owner_id;
	ByteStorePtr password;
} ContactInfo, PNTR ContactInfoPtr;

ContactInfoPtr LIBCALL ContactInfoNew PROTO((void));
ContactInfoPtr LIBCALL ContactInfoFree PROTO(( ContactInfoPtr cip));
ContactInfoPtr LIBCALL ContactInfoAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean        LIBCALL ContactInfoAsnWrite PROTO((ContactInfoPtr cip, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Submit-block
*
*****************************************************************************/
typedef struct subblk {
	ContactInfoPtr contact;
	CitSubPtr cit;
	Boolean hup;      /* hold until published */
	DatePtr reldate;  /* release date */
} SubmitBlock, PNTR SubmitBlockPtr;

SubmitBlockPtr LIBCALL SubmitBlockNew PROTO((void));
SubmitBlockPtr LIBCALL SubmitBlockFree PROTO(( SubmitBlockPtr sbp));
SubmitBlockPtr LIBCALL SubmitBlockAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean        LIBCALL SubmitBlockAsnWrite PROTO((SubmitBlockPtr sbp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Seq-submit
*   	datatype =
*   		0 = not-set (error)
*   		1 = Seq-entry(s) (data = SeqEntryPtr, possibly a chain)
*   		2 = Seq-annot(s) (data = SeqAnnotPtr, possibly a chain)
*   		3 = Seq-id(s) for delete (data = SeqIdPtr, possibly a chain)
*
*****************************************************************************/
typedef struct seqsub {
	SubmitBlockPtr sub;
	Uint1 datatype;
	Pointer data;
} SeqSubmit, PNTR SeqSubmitPtr;

SeqSubmitPtr LIBCALL SeqSubmitNew PROTO((void));
SeqSubmitPtr LIBCALL SeqSubmitFree PROTO(( SeqSubmitPtr ssp));
SeqSubmitPtr LIBCALL SeqSubmitAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean      LIBCALL SeqSubmitAsnWrite PROTO((SeqSubmitPtr ssp, AsnIoPtr aip, AsnTypePtr atp));

#ifdef __cplusplus
}
#endif

#endif

