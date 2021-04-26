/*   ncbiprop.h
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
* File Name:  ncbiprop.h
*
* Author:  Schuler
*
* Version Creation Date:   06-04-93
*
* $Revision: 2.1 $
*
* File Description: 
*   	Application Property functions.
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/
#ifndef _NCBIPROP_
#define _NCBIPROP_

#ifdef __cplusplus
extern "C" {
#endif


typedef int (LIBCALLBACK *Nlm_AppPropEnumProc) PROTO((const char *key, void *data));

void  LIBCALL Nlm_InitAppContext PROTO((void));
void  LIBCALL Nlm_ReleaseAppContext PROTO((void));
void* LIBCALL Nlm_SetAppProperty PROTO((const char *key, void *data));
void* LIBCALL Nlm_GetAppProperty PROTO((const char *key));
void* LIBCALL Nlm_RemoveAppProperty PROTO((const char *key));
int   LIBCALL Nlm_EnumAppProperties PROTO((Nlm_AppPropEnumProc));
long  LIBCALL Nlm_GetAppProcessID PROTO((void));

#define AppPropEnumProc Nlm_AppPropEnumProc
#define InitAppContext Nlm_InitAppContext
#define ReleaseAppContext Nlm_ReleaseAppContext
#define SetAppProperty Nlm_SetAppProperty
#define GetAppProperty Nlm_GetAppProperty
#define RemoveAppProperty Nlm_RemoveAppProperty
#define EnumAppProperties Nlm_EnumAppProperties
#define GetAppProcessID Nlm_GetAppProcessID

#ifdef __cplusplus
}
#endif

#endif
