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
* File Name:    ni_error.c
*
* Author:       Beatty, Gish
*
* Version Creation Date:        1/1/92
*
* $Revision: 1.7 $
*
* File Description: 
*
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#include "ni_error.h"
    
Char * NEAR ni_errlist[] = {
    "no error",
    "error",
    "no entry for user",
    "no entry for host",
    "no entry for service",
    "host address invalid",
    "port number invalid",
    "can't open socket",
    "can't bind socket",
    "accept socket failed",
    "listen on socket failed",
    "can't connect to dispatcher",
    "can't connect to client",
    "can't connect to application",
    "maximum number of connections in use",
    "select statement failed",
    "message write failed",
    "message read failed",
    "message type unknown",
    "fork failed",
    "unable to set user ID",
    "execute failed",
    "unknown service",
    "can't open file",
    "error in configuration file",
    "error parsing service entry",
    "error parsing resource entry",
    "service is unavailable",
    "resource is unavailable",
    "dispatcher disconnected",
    "service disconnected",
    "login failed",
    "login timed out",
    "dispatcher timed out",
    "service timed out",
    "command timed out",
    "timed out",
    "unable to make message handle",
    "user not in group required for access",
    "version requested not in valid range",
    "name requested was not found",
    "requested ID was not found",
    "select returned unrequested fd",
    "error using NIS service",
    "invalid argument",
    "child process exited abnormally",
    "already running on this machine",
    "attempt to set up pipe failed",
    "i/o error on pipe",
    "spawned server OK",
    "spawned server failed",
    "unable to parse string",
    "service/resource type mismatch",
    "dispatcher is acting as backup",
    "client is not an authorized guest user",
    "TCP stack initialization failure",
    "no entry for local host",
    "ASN.1 specification load failure",
    "unknown error"
};

Char * NEAR ni_platform[] = {
    "unknown",           /* NI_PLATFORM_UNKNOWN */
    "Macintosh",         /* NI_PLATFORM_MAC */
    "VMS (TGV)",         /* NI_PLATFORM_VMS_TGV */
    "AXP/OpenVMS",       /* NI_PLATFORM_AXP_OPENVMS */
    "Generic UNIX",      /* NI_PLATFORM_GENERIC_UNIX */
    "IBM370 AIX",        /* NI_PLATFORM_IBM370AIX */
    "Sun",               /* NI_PLATFORM_SUN */
    "Alpha+OSF/1",       /* NI_PLATFORM_ALPHA_OSF1 */
    "Mac AU/X",          /* NI_PLATFORM_AUX */
    "Cray",              /* NI_PLATFORM_CRAY */
    "Convex",            /* NI_PLATFORM_CONVEX */
    "HP/UX",             /* NI_PLATFORM_HPUX */
    "NEXT",              /* NI_PLATFORM_NEXT */
    "SGI (MIPS)",        /* NI_PLATFORM_SGI */
    "ULTRIX",            /* NI_PLATFORM_ULTRIX */
    "DOS",               /* NI_PLATFORM_DOS */
    "WIN16",             /* NI_PLATFORM_WIN16 */
    "NEWT",              /* NI_PLATFORM_WIN_NEWT */
    "PC-NFS",            /* NI_PLATFORM_WIN_PCNFS */
    "WINSOCK",           /* NI_PLATFORM_WIN_WINSOCK */
    "WIN NT",            /* NI_PLATFORM_WINNT */
    "System V on Sparc", /* NI_PLATRFOM_SYSV_ON_SPARC */
    "VMS (UCX)",         /* NI_PLATFORM_VMS_UCX */
    "VMS (TWG)",         /* NI_PLATFORM_VMS_TWG */
};

Uint2         ni_nerr = (Uint2) NIE_UNKNOWN;  /* number of errors */
NI_ErrLevel   ni_errlev;                      /* error level - NOT SET on error */
NI_Error      ni_errno;                       /* error number set by failing function */
Char NEAR     ni_errtext[ERRTEXT_BUFSIZ];     /* additional error text buffer */
