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
* File Name:    ni_types.h
*
* Author:       Beatty, Gish
*
* Version Creation Date:        1/1/92
*
* $Revision: 1.12 $
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

#ifndef _NI_TYPES_
#define _NI_TYPES_

#include <ncbi.h>
#include <asn.h>

#define NI_Handle       MHandle                 /* for API use */
#define NI_HandPtr      MHandPtr                /* for API use */
#define NI_Request      Request                 /* for API use */
#define NI_ReqPtr       ReqPtr                  /* for API use */

#define INETADDR_SIZ    16                      /* of the form 255.255.255.255 */

#define NI_ConnState enum NIConState
NI_ConnState {
        NI_ERROR_COND = 0,
        NI_CREATED,
        NI_BOUND,
        NI_CONNECTED,
        NI_DISCARD,             /* disconnect pending (requested) */
        NI_DISCONNECTED
};

typedef struct Node PNTR NodePtr;
typedef struct Node {
        VoidPtr         elem;           /* pointer to the element */
        NodePtr         last;           /* previous element */
        NodePtr         next;           /* next element */
} Node;

typedef Int4 (LIBCALLBACK *NI_ReadFilt) PROTO((VoidPtr mh, CharPtr buf, int bytesread, int len, CharPtr PNTR extra_buf, Int4Ptr extra_buf_len));
typedef Int4 (LIBCALLBACK *NI_WriteFilt) PROTO((VoidPtr mh, CharPtr buf, int len, CharPtr tmpbuf));
typedef void (LIBCALLBACK *NI_TimeoutHook) PROTO((Pointer p));

typedef struct MHandle {
        CharPtr         hostname;       /* name of peer machine */
        Uint4           conid;          /* unique connection id number */
        Uint4           seqno;          /* unique message number */
        NI_ConnState    state;          /* of connection - bound, connected, etc. */
        int             sok;            /* the socket or file descriptor */
        int             r_timeout;      /* read timeout value in seconds */
        int             w_timeout;      /* write timeout value in seconds */
        Uint4           peer;           /* Internet address of peer */
        AsnIoPtr        raip;           /* ASNtool IO read pointer */
        AsnIoPtr        waip;           /* ASNtool IO write pointer */
        Boolean         longjump;       /* TRUE if OK to use longjump (servers) */
        time_t          access_time;    /* time stamp of last access */
        Boolean         unblocked_mode; /* is this interface "unblocked" ? */
        Boolean         have_blocked;   /* have we blocked reading this msg? */
        int             num_queued_bytes; /* amt of queued data for this msg */
        int             cur_index;      /* current index for reading queue */
        NodePtr         queued_data_list; /* queued data for this msg */
        VoidPtr         extra_proc_info; /* extra processing info, used externally */
        NI_WriteFilt    write_filter;    /* output filter routine */
        NI_ReadFilt     read_filter;     /* input filter routine */
        Boolean         write_filt_pass_thru; /* write filter is pass-thru */
        Boolean         read_filt_pass_thru; /* read filter is pass-thru */
        NI_TimeoutHook  readTimeoutHook; /* hook for read timeout expiration */
        NI_TimeoutHook  writeTimeoutHook; /* hook for write tmout expiration */
        NodePtr         readTimer;
        NodePtr         writeTimer;
} MHandle, PNTR MHandPtr;

typedef enum _NetServHookCode_ {
        NetServHook_dispconn = 1,
        NetServHook_svcreq,
        NetServHook_svcdisconn,
        NetServHook_dispdisconn,
        NetServHook_read,
        NetServHook_write
} NI_NetServHookCode;

typedef void (LIBCALLBACK *NI_NetServHook) PROTO((NI_HandPtr, NI_NetServHookCode, int));


typedef struct NICatalog {
        Uint4           seqno;
        CharPtr         motd;
        NodePtr         toolsetL;
} NICatalog, PNTR NICatalogPtr;

typedef struct NIRegion {
        CharPtr         regionName;
        Uint2           priorityDelta; /* incentive to clients in that region */
} NIRegion, PNTR NIRegionPtr;

typedef struct NIToolset {
        CharPtr         host;
        CharPtr         motd;
        NodePtr         services;
        NodePtr         resources;
        NodePtr         regions;
} NIToolset, PNTR NIToolsetPtr;

typedef struct NIService {
        CharPtr         name;           /* name of service */
        Uint2           minVersion;
        Uint2           maxVersion;     /* NULL if infinite */
        Uint2           id;             /* ID that is unique on host */
        Uint2           priority;       /* priority of service */
        CharPtr         group;          /* group with access to resource */
        CharPtr         descrip;        /* description of service */
        NodePtr         typeL;          /* list of res types supported by svc */
        NodePtr         subSetList;     /* svcs of which this supports a subset */
        NodePtr         superSetList;   /* svcs of which this supports a superset */
        Uint2           priorityTimeout; /* timeout by which dispd must hear  */
        Uint2           priorityPenalty; /* resp from ncbid or impose penalty */
} NIService, PNTR NISvcPtr;

typedef struct NIResource {
        CharPtr         name;
        CharPtr         type;           /* same as a service name ? */
        Uint2           minVersion;
        Uint2           maxVersion;
        Uint2           id;             /* ID that is unique on host */
        CharPtr         group;          /* group with access to resource */
        CharPtr         descrip;        /* description of resource */
} NIResource, PNTR NIResPtr;

typedef struct NI_Uid {
        CharPtr         username;       /* kerberos principle */
        CharPtr         group;          /* kerberos instance */
        CharPtr         domain;         /* kerberos realm */
} NI_Uid, PNTR NI_UidPtr;

typedef struct NI_Dispatcher {
        struct Request PNTR reqResponse;     /* ptr to request struct with actual services/resources used */
        CharPtr          dispHostName;
        CharPtr          dispServiceName;
        Int4             dispSerialNo;
        Char             localHostAddr[INETADDR_SIZ];  /* Internet address of client */
        NI_HandPtr       dispHP;             /* handle for dispatcher */
        NI_HandPtr       svcsHP;             /* handle for incoming services */
        Uint2            clientPort;
        NI_UidPtr        identity;           /* client identity struct */
        int              dispTimeout;        /* timeout when communicating w/dispatcher */
        Int2             loport;             /* range of client ports to bind */
        Int2             hiport;             /* range of client ports to bind */
        Int2             referenceCount;
} NI_Dispatcher, PNTR NI_DispatcherPtr;

typedef struct Request {
        CharPtr         clientAddr;
        Uint2           clientPort;
        NISvcPtr        service;        /* ptr to a service (NOT a node) */
        NodePtr         resourceL;      /* ptr to list of resources */
        NI_DispatcherPtr dispatcher;    /* dispatcher on which req is made */
} Request, PNTR ReqPtr;

typedef struct NI_DispInfo {
        Int4            serialno;       /* unique dispatcher serial # */
        Boolean         isalternatelist;/* is this an alternate disp-list ? */
        int             numdispatchers; /* number of dispachers in disp-list */
        CharPtr PNTR    displist;       /* list of dispatchers */
} NI_DispInfo, PNTR NI_DispInfoPtr;


#endif
