/*   ncbimem.h
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
* File Name:  ncbimem.h
*
* Author:  Gish, Kans, Ostell, Schuler
*
* Version Creation Date:   1/1/91
*
* $Revision: 2.14 $
*
* File Description: 
*   	prototypes for ncbi memory functions
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 9-19-91  Schuler     Modified existing prototypes for ANSI-resemblance
* 9-19-91  Schuler     Added new prototypes for Windows ANSI-like functions
* 9-19-91  Schuler     Changed all functions to _cdecl calling convention
* 04-15-93 Schuler     Changed _cdecl to LIBCALL
* 05-21-93 Schuler     Nlm_MemFreeTrace added for debugging MemFree
* 06-14-93 Schuler     Added dll_Malloc and dll_Free
*
* ==========================================================================
*/
#ifndef _NCBIMEM_
#define _NCBIMEM_

#ifdef __cplusplus
extern "C" {
#endif

/* ======== PROTOTYPES ======== */

void * LIBCALL Nlm_MemNew PROTO((Nlm_sizeT size));
void * LIBCALL Nlm_MemGet PROTO((Nlm_sizeT size, unsigned int flags));
void * LIBCALL Nlm_MemMore PROTO((void *ptr, Nlm_sizeT size));
void * LIBCALL Nlm_MemExtend PROTO((void *ptr, Nlm_sizeT size, Nlm_sizeT oldsize));
void * LIBCALL Nlm_MemFree PROTO((void *ptr));
void * LIBCALL Nlm_MemCopy PROTO((void *dst, const void *src, Nlm_sizeT bytes));
void * LIBCALL Nlm_MemMove PROTO((void *dst, const void *src, Nlm_sizeT bytes));
void * LIBCALL Nlm_MemFill PROTO((void *ptr, int value, Nlm_sizeT bytes));
void * LIBCALL Nlm_MemDup PROTO((const void *orig, Nlm_sizeT size));

#if (defined(OS_MAC) || defined(WIN_MSWIN) || defined(MSC_VIRT))
Nlm_Handle  LIBCALL Nlm_HandNew PROTO((Nlm_sizeT size));
Nlm_Handle  LIBCALL Nlm_HandGet PROTO((Nlm_sizeT size, Nlm_Boolean clear_out));
Nlm_Handle  LIBCALL Nlm_HandMore PROTO((Nlm_Handle hnd, Nlm_sizeT size));
Nlm_Handle  LIBCALL Nlm_HandFree PROTO((Nlm_Handle hnd));
void *LIBCALL Nlm_HandLock PROTO((Nlm_Handle hnd));
void *LIBCALL Nlm_HandUnlock PROTO((Nlm_Handle hnd));
#endif

#ifdef WIN16
void *  LIBCALL win16_Malloc (Nlm_sizeT bytes);
void *  LIBCALL win16_Calloc (Nlm_sizeT items, Nlm_sizeT size);
void *  LIBCALL win16_Realloc (void *ptr, Nlm_sizeT size);
void    LIBCALL win16_Free (void *ptr);
#endif

#ifdef WIN_MAC
#ifdef USE_MAC_MEMORY
void *mac_Malloc  PROTO((Nlm_sizeT size));
void *mac_Calloc  PROTO((Nlm_sizeT nmemb, Nlm_sizeT size));
void *mac_Realloc PROTO((void *ptr, Nlm_sizeT size));
void  mac_Free    PROTO((void *ptr));
#endif
#endif

/* ========= MACROS ======== */

/* low-level ANSI-style functions */
#ifdef WIN16
#define Nlm_Malloc  win16_Malloc
#define Nlm_Calloc  win16_Calloc
#define Nlm_Realloc win16_Realloc
#define Nlm_Free    win16_Free
#define Nlm_MemSet  _fmemset
#define Nlm_MemCpy  _fmemcpy
#define Nlm_MemChr  _fmemchr
#define Nlm_MemCmp  _fmemcmp
#else
#ifdef USE_MAC_MEMORY
#define Nlm_Malloc  mac_Malloc
#define Nlm_Calloc  mac_Calloc
#define Nlm_Realloc mac_Realloc
#define Nlm_Free    mac_Free
#define Nlm_MemSet  memset
#define Nlm_MemCpy  memcpy
#define Nlm_MemChr  memchr
#define Nlm_MemCmp  memcmp
#else
#define Nlm_Malloc  malloc
#define Nlm_Calloc  calloc										 
#define Nlm_Realloc realloc
#define Nlm_Free    free
#define Nlm_MemSet  memset
#define Nlm_MemCpy  memcpy
#define Nlm_MemChr  memchr
#define Nlm_MemCmp  memcmp
#endif
#endif

#define Malloc  Nlm_Malloc
#define Calloc  Nlm_Calloc
#define Realloc Nlm_Realloc
#define Free    Nlm_Free
#define MemSet  Nlm_MemSet
#define MemCpy  Nlm_MemCpy
#define MemChr  Nlm_MemChr
#define MemCmp  Nlm_MemCmp

/*** High-level NCBI functions ***/

/* Fake handle functions with pointer functions */

#if !( defined(OS_MAC) || defined(WIN_MSWIN) || defined(MSC_VIRT))
#define Nlm_HandNew(a)    Nlm_MemNew(a)    
#define Nlm_HandGet(a,b)  Nlm_MemGet(a,b)
#define Nlm_HandMore(a,b) Nlm_MemMore(a,b)
#define Nlm_HandFree(a)   Nlm_MemFree(a)
#define Nlm_HandLock(a)   (a)
#define Nlm_HandUnlock(a) NULL
#endif

/* Pointer functions */
#define MemNew(x)     Nlm_MemGet(x,MGET_CLEAR|MGET_ERRPOST)
#define MemGet(x,y)   Nlm_MemGet(x,(unsigned int)(y))
#define MemFree       Nlm_MemFree
#define MemMore       Nlm_MemMore
#define MemExtend     Nlm_MemExtend
#define MemCopy       Nlm_MemCopy
#define MemMove       Nlm_MemMove
#define MemFill       Nlm_MemFill
#define MemDup        Nlm_MemDup

#define HandNew     Nlm_HandNew
#define HandGet     Nlm_HandGet
#define HandMore    Nlm_HandMore
#define HandFree    Nlm_HandFree
#define HandLock    Nlm_HandLock
#define HandUnlock  Nlm_HandUnlock


#if defined(WIN_MSWIN) && defined(_DEBUG)
void * LIBCALL Nlm_MemFreeTrace (void *, const char*, const char*, int);
#undef MemFree
#define MemFree(_ptr_)  Nlm_MemFreeTrace(_ptr_,THIS_MODULE,THIS_FILE,__LINE__)
#endif


#ifdef _WINDLL
void * dll_Malloc (Nlm_sizeT bytes);
void   dll_Free (void *pMem);
#else
#define dll_Malloc(x)	(void*)Nlm_Malloc(x)
#define dll_Free(x)	Nlm_Free((void*)(x))
#endif



/* flags for MemGet */
#define MGET_CLEAR    0x0001
#define MGET_ERRPOST  0x0004


/* obsolete */
#define MG_CLEAR    MGET_CLEAR
#define MG_MAXALLOC 0x0002
#define MG_ERRPOST  MGET_ERRPOST


#ifdef __cplusplus
}
#endif

#endif
