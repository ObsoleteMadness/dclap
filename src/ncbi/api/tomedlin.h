/*  tomedlin.h
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
* File Name:  tomedlin.h
*
* Author:  Jonathan Kans
*   
* Version Creation Date: 10/15/91
*
* $Revision: 2.1 $
*
* File Description:  conversion to medlars format
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_Tomedlin_
#define _NCBI_Tomedlin_

#include <objmedli.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct medlinedata {
  CharPtr  journal;
  CharPtr  volume;
  CharPtr  pages;
  CharPtr  year;
  CharPtr  title;
  CharPtr  transl;
  CharPtr  authors;
  CharPtr  affil;
  CharPtr  abstract;
  CharPtr  mesh;
  CharPtr  gene;
  CharPtr  substance;
  CharPtr  uid;
} MedlineData, PNTR MedlinePtr;

extern Boolean MedlineEntryToDataFile PROTO((MedlineEntryPtr mep, FILE *fp));
extern Boolean MedlineEntryToDocFile PROTO((MedlineEntryPtr mep, FILE *fp));

extern MedlinePtr ParseMedline PROTO((MedlineEntryPtr mep));
extern MedlinePtr FreeMedline PROTO((MedlinePtr mPtr));

#ifdef __cplusplus
}
#endif

#endif
