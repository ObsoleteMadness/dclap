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
* File Name:    ni_error.h
*
* Author:       Beatty, Gish
*
* Version Creation Date:        1/1/92
*
* $Revision: 1.10 $
*
* File Description: 
*
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 5/12/92  Epstein     Converted tabs to spaces
*
*
* ==========================================================================
*/

#ifndef _NI_ERROR_
#define _NI_ERROR_

#include "ncbinet.h"

#define NI_PLATFORM_UNKNOWN             0
#define NI_PLATFORM_MAC                 1
#define NI_PLATFORM_VMS_TGV             2
#define NI_PLATFORM_AXP_OPENVMS         3
#define NI_PLATFORM_GENERIC_UNIX        4
#define NI_PLATFORM_IBM370AIX           5
#define NI_PLATFORM_SUN                 6
#define NI_PLATFORM_ALPHA_OSF1          7
#define NI_PLATFORM_AUX                 8
#define NI_PLATFORM_CRAY                9
#define NI_PLATFORM_CONVEX              10
#define NI_PLATFORM_HPUX                11
#define NI_PLATFORM_NEXT                12
#define NI_PLATFORM_SGI                 13
#define NI_PLATFORM_ULTRIX              14
#define NI_PLATFORM_DOS                 15
#define NI_PLATFORM_WIN16               16
#define NI_PLATFORM_WIN_NEWT            17
#define NI_PLATFORM_WIN_PCNFS           18
#define NI_PLATFORM_WIN_WINSOCK         19
#define NI_PLATFORM_WINNT               20
#define NI_PLATFORM_SYSV_ON_SPARC       21
#define NI_PLATFORM_VMS_UCX             22
#define NI_PLATFORM_VMS_TWG             23
#define NI_PLATFORM_VMS_WPW             24

#define NI_MAX_PLATFORMS                24

#define NI_ErrLevel enum ni_errlevel
NI_ErrLevel {
    NIL_INFORMATION = 1,        /* informational */
    NIL_WARNING,                /* small problem */
    NIL_SEVERE,         /* recoverable */
    NIL_FATAL           /* not recoverable */
};

#define NI_Error enum ni_error
NI_Error {
    NIE_NO_ERROR = 0,   /* no error */
    NIE_MISC,           /* error */
    NIE_NOUSERENT,      /* no entry for user */
    NIE_NOHOSTENT,      /* no entry for host */
    NIE_NOSERVENT,      /* no entry for service */
    NIE_BADHOST,        /* host address invalid */
    NIE_BADPORT,        /* port number invalid */
    NIE_SOCKOPEN,       /* can't open socket */
    NIE_NOBIND,         /* can't bind socket */
    NIE_NOACCEPT,       /* accept socket failed */
    NIE_NOLISTEN,       /* listen on socket failed */
    NIE_DISPCONN,       /* can't connect to dispatcher */
    NIE_CLICONN,        /* can't connect to client */
    NIE_APPCONN,        /* can't connect to application */
    NIE_MAXCONNS,       /* maximum number of connections in use */
    NIE_SELECT,         /* select statement failed */
    NIE_MSGWRITE,       /* message write failed */
    NIE_MSGREAD,        /* message read failed */
    NIE_MSGUNK,         /* message type unknown */
    NIE_FORK,           /* fork failed */
    NIE_SETUID,         /* unable to set user ID */
    NIE_EXEC,           /* execute failed */
    NIE_SERVUNK,        /* unknown service */
    NIE_FILOPEN,        /* can't open file */
    NIE_CONFIG,         /* error in configuration file */
    NIE_SVCENT,         /* error parsing service entry */
    NIE_RESENT,         /* error parsing resource entry */
    NIE_SVCUNAVL,       /* service is unavailable */
    NIE_RESUNAVL,       /* resource is unavailable */
    NIE_DSPDCONN,       /* dispatcher disconnected */
    NIE_SVCDCONN,       /* service disconnected */
    NIE_BADLOGIN,       /* login failed */
    NIE_LOGTIMEOUT,     /* login timed out */
    NIE_DSPTIMEOUT,     /* dispatcher timed out */
    NIE_SVCTIMEOUT,     /* service timed out */
    NIE_CMDTIMEOUT,     /* command timed out */
    NIE_TIMEOUT,        /* timed out */
    NIE_MAKEHAND,       /* unable to make message handle */
    NIE_NOTINGRP,       /* user not in group required for access */
    NIE_BADVERS,        /* version requested not in valid range */
    NIE_BADNAME,        /* name requested was not found */
    NIE_BADREQID,       /* requested ID was not found */
    NIE_BADSELFD,       /* select returned unrequested fd */
    NIE_NISERROR,       /* error using NIS service */
    NIE_INVAL,          /* invalid argument */
    NIE_BADEXIT,        /* child process exited abnormally */
    NIE_SEMBUSY,        /* already running on this machine */
    NIE_PIPE,           /* attempt to set up pipe failed */
    NIE_PIPEIO,         /* i/o error on pipe */
    NIE_SERVACK,        /* spawned server OK */
    NIE_SERVNACK,       /* spawned server failed */
    NIE_PARSE,          /* unable to parse string */
    NIE_TYPEMISM,       /* service/resource type mismatch */
    NIE_BACKUPDISP,     /* dispatcher is acting as backup */
    NIE_NOTGUEST,       /* client is not an authorized guest user */
    NIE_TCPINITFAIL,    /* TCP stack initialization failure */
    NIE_NOLOCALHOSTENT, /* no entry for local host */
    NIE_ASN1SPECFAIL,   /* ASN.1 specification load failure */
    NIE_SVCNOTENCR,     /* service does not support encryption */
    NIE_BROKSVCCONN,    /* can't connect to brokered service */
    NIE_PUBKEYENCRFAIL, /* failure trying to perform public-key encryption */
    NIE_PUBKEYDECRFAIL, /* failure trying to perform public-key decryption */
    NIE_NEWKEYNOTACCPT, /* new encryption key was not accepted by user */
    NIE_NEWKEYMISMATCH, /* mismatched new encryption key not accepted by user */
    NIE_UNKNOWN         /* unknown error */
};

extern Uint2            ni_nerr;                /* number of errors */
extern enum ni_errlevel ni_errlev;              /* error level - NOT SET on error */
extern enum ni_error    ni_errno;               /* error number set by failing function */
extern Char * NEAR      ni_errlist[];           /* list of error messages */
extern Char NEAR        ni_errtext[];           /* additional error text buffer */
extern Char * NEAR      ni_platform[];          /* list of platform names messages */

#endif

