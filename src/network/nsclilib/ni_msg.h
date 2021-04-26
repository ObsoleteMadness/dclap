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
* File Name:    ni_msg.h
*
* Author:       Beatty, Gish
*
* Version Creation Date:        1/1/92
*
* $Revision: 4.0 $
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
* 04-21-93 Schuler     Changed declaration of NI_AsnNew(), NI_AsnWrite(), 
*                      and NI_ASNIOError() to use LIBCALLBACK calling convention.
*
*
* ==========================================================================
*
*
* RCS Modification History:
* $Log: ni_msg.h,v $
 * Revision 4.0  1995/07/26  13:56:32  ostell
 * force revision to 4.0
 *
 * Revision 1.17  1995/05/24  12:08:49  epstein
 * add support for tracking of how many times a client IP has used a service within a time interval
 *
 * Revision 1.16  95/05/17  17:52:44  epstein
 * add RCS log revision history
 * 
*/

#ifndef _NI_MSG_
#define _NI_MSG_

#include <setjmp.h>

#include "ncbinet.h"
#include "ni_net.h"

#include "ni_list.h"


#ifdef WIN16
#define Jumpbuf   CATCHBUF
#define SetJump   Catch
#define LongJump  Throw
#else
#define Jumpbuf   jmp_buf
#define SetJump   setjmp
#define LongJump  longjmp
#endif

#define NI_BLOCK_TIMEOUT        60
#define NI_BLOCKSIZE            4096 /* matches TCP_MAXSEG in many systems */

/* VARS */

extern Uint4    conid;          /* globally unique connection ID counter */


/* TYPES */

#define MsgType enum msgtype
MsgType {
        NI_UNKNOWN = 0,
        NI_ACK,
        NI_NACK,
        NI_LOGIN,
        NI_SVC_LIST,
        NI_SVC_REQUEST,
        NI_SVC_RESPONSE,
        NI_COMMAND,
        NI_ACCT,
        NI_CATALOG,
        NI_SVC_PRE_RESPONSE
};

#define MsgCommand enum msgcommand
MsgCommand {
        NI_SEND_SVCLIST = 1,    /* send me a list of your services */
        NI_SEND_CATALOG,        /* send me the catalog (dispatcher) */
        NI_KILL,                /* kill yourself */
        NI_POLL                 /* health check poll */
};

typedef struct NIPubKey { /* internal format for public key */
        Int2            bits;           /* bits used in modulus */
        ByteStorePtr    modulus;
        ByteStorePtr    exponent;
} NIPubKey, PNTR NIPubKeyPtr;

typedef struct NIDispInfo {
        Int4            serialno;       /* unique dispatcher serial # */
        Boolean         isalternatelist;/* is this an alternate disp-list ? */
        int             numdispatchers; /* number of dispatchers in disp-list */
        CharPtr PNTR    displist;       /* list of dispatchers */
        NIPubKeyPtr     pubKey;         /* public RSA key */
} NIDispInfo, PNTR NIDispInfoPtr;

typedef struct NIAck {
        Uint4           seqno;
        NIDispInfoPtr   dispinfo;
        CharPtr         adminInfo;      /* info. regarding guest's admin */
        CharPtr         motd;           /* message of the day */
} NIAck, PNTR NIAckPtr;

typedef struct NINack {
        Uint4           seqno;
        NI_Error        code;
        CharPtr         reason;
        NIDispInfoPtr   dispinfo;
} NINack, PNTR NINackPtr;

typedef struct NILogin {
        Uint4           seqno;
        NI_UidPtr       uid;            /* ID structure */
        CharPtr         password;       /* must be encrypted */
        Int4            dispserialno;   /* unique dispatcher serial # */
        Boolean         encryptionDesired; /* encryption desired by client */
        NIPubKeyPtr     pubKey;         /* public RSA key */
        ByteStorePtr    desKey;         /* DES key for ncbid<->dispd comm */
        Int2            connectDelay;   /* connection delay (seconds) */
} NILogin, PNTR NILoginPtr;

typedef struct NISvcList {
        Uint4           seqno;
        NIToolsetPtr    toolset;
        Boolean         knowsTracking;  /* does this ncbid know about tracking? */
} NISvcList, PNTR NISvcListPtr;

typedef struct NISvcReq {
        Uint4           seqno;
        Uint4           conid;          /* for client accounting */
        NI_UidPtr       uid;
        ReqPtr          request;
        Uint4           platform;       /* client's platform */
        CharPtr         applId;         /* application identifier */
        ByteStorePtr    desKey;
        Boolean         wantPreResponse; /* client wants to know server's IP */
        Uint4           server_ip;
} NISvcReq, PNTR NISvcReqPtr;

typedef struct NISvcResp {
        Uint4           seqno;
        ReqPtr          request;
} NISvcResp, PNTR NISvcRespPtr;

typedef struct NICmd {
        Uint4           seqno;
        MsgCommand      code;
} NICmd, PNTR NICmdPtr;

typedef struct NIPreResp {
        Uint4           seqno;
        Uint4           server_ip;
} NIPreResp, PNTR NIPreRespPtr;

typedef struct NIAcct {
        Uint4           seqno;
        Uint4           conid;
        CharPtr         jobname;
        Uint4           usertime;
        Uint4           systemtime;
} NIAcct, PNTR NIAcctPtr;

typedef union NIMSun {
        NIAckPtr        ack;
        NINackPtr       nack;
        NILoginPtr      login;
        NISvcListPtr    svclist;
        NISvcReqPtr     svcreq;
        NISvcRespPtr    svcresp;
        NICmdPtr        command;
        NIAcctPtr       acct;
        NICatalogPtr    catalog;
        NIPreRespPtr    preresp;
} NIMSun, PNTR NIMSunPtr;

typedef struct NIMessage {
        MsgType         type;
        Uint4           conid;          /* for ACKing */
        NIMSun          msun;           /* message structure union */
} NIMessage, PNTR NIMsgPtr;


/* FUNCTION PROTOTYPES */

extern NIMsgPtr         MsgRead PROTO((MHandPtr handle, Boolean unblocked));

extern Int2             MsgWrite PROTO((MHandPtr handle, NIMsgPtr mp, Boolean noDestroy));

extern NIMsgPtr         MsgBuild PROTO((MsgType type, Uint4 conn, VoidPtr stp));

extern Int2             MsgDestroy PROTO((NIMsgPtr mp));

extern MHandPtr         MsgMakeHandle PROTO((Boolean createSocket));

extern Int2             MsgDestroyHandle PROTO((MHandPtr hp));

extern void             MsgSetLJError PROTO((MHandPtr mh));

extern void             MsgSetReadTimeout PROTO((MHandPtr mh, int t));

extern void             MsgSetWriteTimeout PROTO((MHandPtr mh, int t));

extern void             MsgSetFilters PROTO((MHandPtr mh, VoidPtr ex_proc, NI_WriteFilt wfilt, NI_ReadFilt rfilt, Boolean wfilt_pass, Boolean rfilt_pass));

extern void             MsgSetTimeoutHooks PROTO((MHandPtr mh, NI_TimeoutHook rhook, NI_TimeoutHook whook));

extern Int2 LIBCALLBACK NI_AsnRead PROTO((Pointer p, CharPtr buf, Uint2 len));

extern Int2 LIBCALLBACK NI_AsnWrite PROTO((Pointer p, CharPtr buf, Uint2 len));

extern void LIBCALLBACK NI_ASNIOError PROTO((Int2 level, CharPtr str));

extern int              NI_poll_select PROTO((int width, fd_set PNTR rfds, fd_set PNTR wfds, fd_set PNTR xfds, struct timeval PNTR timeout));
extern int              NI_SetBlocking PROTO((int sock));
extern int              NI_SetNonBlocking PROTO((int sock));


/* Message Type Functions */

extern NIAckPtr         NI_MakeMsgAck PROTO((void));

extern Int2             NI_DestroyMsgAck PROTO((NIAckPtr));

extern NINackPtr        NI_MakeMsgNack PROTO((void));

extern Int2             NI_DestroyMsgNack PROTO((NINackPtr));

extern NILoginPtr       NI_MakeMsgLogin PROTO((void));

extern Int2             NI_DestroyMsgLogin PROTO((NILoginPtr));

extern NISvcListPtr     NI_MakeMsgSvclist PROTO((void));

extern Int2             NI_DestroyMsgSvclist PROTO((NISvcListPtr));

extern NISvcReqPtr      NI_MakeMsgSvcreq PROTO((void));

extern Int2             NI_DestroyMsgSvcreq PROTO((NISvcReqPtr));

extern NISvcRespPtr     NI_MakeMsgSvcresp PROTO((void));

extern Int2             NI_DestroyMsgSvcresp PROTO((NISvcRespPtr));

extern NICmdPtr         NI_MakeMsgCmd PROTO((void));

extern Int2             NI_DestroyMsgCmd PROTO((NICmdPtr));

extern NIPreRespPtr     NI_MakeMsgPreResp PROTO((void));

extern Int2             NI_DestroyMsgPreResp PROTO((NIPreRespPtr));

extern NIAcctPtr        NI_MakeMsgAcct PROTO((void));

extern Int2             NI_DestroyMsgAcct PROTO((NIAcctPtr));

extern NISvcPtr         NI_MakeService PROTO((void));

extern Int2             NI_DestroyService PROTO((NISvcPtr));

extern NIResPtr         NI_MakeResource PROTO((void));

extern Int2             NI_DestroyResource PROTO((NIResPtr));

extern NIRegionPtr      NI_MakeRegion PROTO((void));

extern Int2             NI_DestroyRegion PROTO((NIRegionPtr));

extern ReqPtr           NI_MakeRequest PROTO((void));

extern Int2             NI_DestroyRequest PROTO((ReqPtr));

extern NICatalogPtr     NI_MakeMsgCatalog PROTO((void));

extern Int2             NI_DestroyMsgCatalog PROTO((NICatalogPtr));

extern NI_UidPtr        NI_MakeUid PROTO((void));

extern Int2             NI_DestroyUid PROTO((NI_UidPtr));

extern NIPubKeyPtr      NI_MakePubKey PROTO((void));

extern Int2             NI_DestroyPubKey PROTO((NIPubKeyPtr));

extern int              NI_ReadPubKey PROTO((AsnIoPtr extaip, AsnTypePtr extatp, NIPubKeyPtr dip));
extern void             NI_WritePubKey PROTO((AsnIoPtr extaip, AsnTypePtr extatp, NIPubKeyPtr dip));

extern NIDispInfoPtr    NI_MakeDispInfo PROTO((void));

extern Int2             NI_DestroyDispInfo PROTO((NIDispInfoPtr));

extern int              NI_ReadDispInfo PROTO((AsnIoPtr extaip, AsnTypePtr extatp, NIDispInfoPtr dip));
extern void             NI_WriteDispInfo PROTO((AsnIoPtr extaip, AsnTypePtr extatp, NIDispInfoPtr dip));

extern NIToolsetPtr     NI_MakeToolset PROTO((void));

extern Int2             NI_DestroyToolset PROTO((NIToolsetPtr tsp));

extern NIToolsetPtr     NI_GetCatToolset PROTO((NIToolsetPtr tsp));


#endif
