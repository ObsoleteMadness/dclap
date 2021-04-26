/*      
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
* File Name:    ncbinet.h
*
* Author:       Beatty, Gish
*
* Version Creation Date:        1/1/92
*
* $Revision: 1.8 $
*
* File Description: 
*
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 5/12/92  Epstein     Converted tabs to spaces
* 06-28-93 Schuler     Added extern "C" linkage specifier
*
* ==========================================================================
*/

#ifndef _NCBINET_
#define _NCBINET_

#ifdef __cplusplus
extern "C" {
#endif

#include "ni_types.h"   /* include <ncbi.h> */
#include "ni_defin.h"
#include "ni_error.h"

#ifdef OS_MAC
#include "macsockd.h"
#define __TYPES__       /* avoid Mac <Types.h> */
#define __MEMORY__      /* avoid Mac <Memory.h> */
#define ipBadLapErr     /* avoid Mac <MacTCPCommonTypes.h> */
#define APPL_SOCK_DEF
#ifdef __NI_LIB__
#define DONT_DEFINE_INET
#else
#define SOCK_DEFS_ONLY
#endif

#include "sock_ext.h"
extern void bzero PROTO((CharPtr target, long numbytes));

#endif /* OS_MAC */


/* GLOBAL VARIABLES */

#if defined(OS_UNIX) || defined(OS_MAC)
extern CharPtr  sys_errlist[];
extern int      errno;
#endif

#ifdef OS_MAC
extern long     errno_long;
#endif


/* CLIENT FUNCTIONS */

extern NI_DispatcherPtr NI_SetDispatcher PROTO((NI_DispatcherPtr disp, CharPtr host, CharPtr svc, int timeout, Int4 uniqueSeqNo));

extern Int2             NI_InitServices PROTO((NI_DispatcherPtr disp, CharPtr user, CharPtr group, CharPtr password, NI_DispInfoPtr PNTR dip));

NI_DispatcherPtr NI_GenericInit PROTO((CharPtr configFile, CharPtr configSection, Boolean showMonitor, CharPtr lastDispatcher, Int2 lastDispLen));

NI_HandPtr NI_GenericGetService PROTO((NI_DispatcherPtr disp, CharPtr configFile, CharPtr configSection, CharPtr defService, Boolean hasResource));

extern Int4             NI_SetDispConfig PROTO((NI_DispInfoPtr PNTR dip, CharPtr dispatcher, Int2 dispLen));

extern Int2             NI_EndServices PROTO((NI_DispatcherPtr disp));

extern NICatalogPtr     NI_GetCatalog PROTO((NI_DispatcherPtr disp));

extern ReqPtr           NI_SVCRequestBuild PROTO((NI_DispatcherPtr disp));

extern void             NI_SVCRequestDestroy PROTO((ReqPtr reqp));

extern NI_HandPtr       NI_ServiceGet PROTO((NI_DispatcherPtr disp, CharPtr svc, Uint2 svcvermin, Uint2 svcvermax, CharPtr res, CharPtr restype, Uint2 resvermin, Uint2 resvermax));

extern NI_HandPtr       NI_ServiceRequest PROTO((ReqPtr req));

extern Int2             NI_ServiceDisconnect PROTO((NI_HandPtr mhp));

extern int              NI_ServiceGetReadFd PROTO((NI_HandPtr mhp));

extern int              NI_ServiceGetWriteFd PROTO((NI_HandPtr mhp));

extern Int2             NI_RequestSetService PROTO((ReqPtr req, CharPtr name, Uint2 vermin, Uint2 vermax));

extern Int2             NI_RequestAddResource PROTO((ReqPtr req, CharPtr name, CharPtr type, Uint2 vermin, Uint2 vermax));

extern Int2             NI_GetPlatform PROTO((void));


/* SERVER FUNCTIONS */

extern int              NI_ServerACK PROTO((void));

extern int              NI_ServerNACK PROTO((CharPtr err_text));

extern NI_HandPtr       NI_OpenASNIO PROTO((void));

extern Int2             NI_CloseASNIO PROTO((NI_HandPtr hp));

/* TIMER MANAGEMENT */

extern void             NI_ProcessTimers PROTO((void));
extern time_t           NI_GetNextWakeup PROTO((void));
extern NodePtr          NI_SetTimer PROTO((time_t timeout, NI_TimeoutHook hook, Pointer hookParam));
extern void             NI_CancelTimer PROTO((NodePtr timerId));

/* MISC. FUNCTIONS */

extern void             NI_SetActivityHook PROTO((NI_NetServHook hook));

/* for internal network services use only */
extern NI_NetServHook   NI_ActivityHook PROTO((void));
extern void             WriteConFile PROTO((Uint4 conid));

#ifdef __cplusplus
}
#endif

#endif
