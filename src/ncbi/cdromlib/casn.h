/* casn.h
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
* File Name: casn.h
*
* Author:  Greg Schuler
*
* Version Creation Date: 9/23/92
*
* $Revision: 2.5 $
*
* File Description:
	functions to decompress a compressed ASN,1 (CASN) file.
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 04-21-93 Schuler     CASN_ReadBuff declared as LIBCALLBACK
* 06-28-93 Schuler     New function:  CASN_Seek()
*
* ==========================================================================
*/
#ifndef __CompressedASN1__
#define __CompressedASN1__

#include <objsset.h>

#ifdef _cplusplus
extern "C" {
#endif

enum CASN_Error	{
	CASN_ErrNone,			/* no error */
	CASN_ErrGeneric,		/* general error, not one listed below */
	CASN_ErrMemory,			/* memory allocation failed */
	CASN_ErrBadHandle,		/* CASN_Handle is invalid or corrupt */
	CASN_ErrFileOpen,		/* unable to open file for reading */
	CASN_ErrFileCreate,		/* unable to open file for writing */
	CASN_ErrFileRead,		/* unable to read from open file */
	CASN_ErrFileWrite,		/* unable to write to open file */
	CASN_ErrFileSeek,		/* either seek or tell failed */
	CASN_ErrFileFormat };	/* file format not recognized, or file corrupt */

enum CASN_DocType	{
	CASN_TypeMed	= 1,	/* MEDLINE record (ASN.1 type Medline-entry)*/
	CASN_TypeSeq	= 2 };	/* Sequence record (ASN.1 type Seq-entry)*/

struct casn_ioblock;
typedef struct casn_ioblock *CASN_Handle;


/* ----- high-level ----- */
CASN_Handle     LIBCALL  CASN_Open PROTO((CharPtr fname));
void            LIBCALL  CASN_Close PROTO((CASN_Handle handle)); 
AsnIoPtr        LIBCALL  CASN_GetAsnIoPtr PROTO((CASN_Handle handle));
Int2            LIBCALL  CASN_DocType PROTO((CASN_Handle handle));
Int4            LIBCALL  CASN_DocCount PROTO((CASN_Handle handle));
MedlineEntryPtr LIBCALL  CASN_NextMedlineEntry PROTO((CASN_Handle handle));
SeqEntryPtr     LIBCALL  CASN_NextSeqEntry PROTO((CASN_Handle handle));
int             LIBCALL  CASN_Seek PROTO((CASN_Handle,long offset,int origin));

/* ----- low-level ----- */
CASN_Handle LIBCALL  CASN_New PROTO((Int2 doc_type, Int2 huff_count));
void        LIBCALL  CASN_Free PROTO((CASN_Handle));

#ifdef _cplusplus
}
#endif

#endif
