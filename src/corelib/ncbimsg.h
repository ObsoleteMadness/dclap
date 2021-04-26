/*   ncbimsg.h
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
* File Name:  ncbimsg.h
*
* Author:  Gish, Kans, Ostell, Schuler
*
* Version Creation Date:   1/1/91
*
* $Revision: 2.7 $
*
* File Description:
*   	prototypes for message and alert handling
*
* Modifications:
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 04-15-93 Schuler     Changed _cdecl to LIBCALL
* 05-28-93 Schuler     New function: SetMonitorHook()
* 06-09-93 Schuler     Added magic_value field to Monitor
* 01-13-94 Schuler     Added SetMsgHook and SetBeepHook
*
* ==========================================================================
*/

#ifndef _NCBIMSG_
#define _NCBIMSG_

#ifdef __cplusplus
extern "C" {
#endif

#include <ncbierr.h>

/*************************************************************************\
|                                ALERTS                                   |
\*************************************************************************/


typedef enum MsgKey
{
	KEY_NONE =0, 
	KEY_OK,
	KEY_OKC,
	KEY_ARI,
	KEY_YNC,
	KEY_YN,
	KEY_RC,
	KEY_other
}
MsgKey;

#define MSG_OK   KEY_OK
#define MSG_OKC  KEY_OKC
#define MSG_ARI  KEY_ARI
#define MSG_YNC  KEY_YNC
#define MSG_YN   KEY_YN
#define MSG_RC   KEY_RC
#define MSG_ERROR   (KEY_other +1)
#define MSG_FATAL   (KEY_other +2)
#define MSG_POST    (KEY_other +3)
#define MSG_POSTERR (KEY_other +4)


/* Message response codes */
typedef enum MsgAnswser
{
	ANS_NONE,
	ANS_OK,
	ANS_CANCEL,
	ANS_ABORT,
	ANS_RETRY,
	ANS_IGNORE,
	ANS_YES,
	ANS_NO
}
MsgAnswer;

/******
#define ANS_NO 0
#define ANS_YES 1
#define ANS_OK 1
#define ANS_RETRY 1
#define ANS_ABORT 2
#define ANS_CANCEL 2
#define ANS_IGNORE 3
******/


MsgAnswer CDECL Nlm_Message VPROTO((Nlm_Int2 keysev, const char *fmt, ...));

MsgAnswer CDECL   Nlm_MsgAlert VPROTO((MsgKey key, ErrSev sev, 
		const char *caption, const char *fmt, ...));
MsgAnswer LIBCALL Nlm_MsgAlertStr PROTO((MsgKey key, ErrSev sev, 
		const char *caption, const char *message));

typedef MsgAnswer (LIBCALLBACK *MessageHook) PROTO((MsgKey key, ErrSev sev, 
		const char *caption, const char *message));
			
MessageHook LIBCALL Nlm_SetMessageHook PROTO((MessageHook hook));

#define Message Nlm_Message
#define MsgAlert Nlm_MsgAlert
#define MsgAlertStr Nlm_MsgAlertStr
#define SetMessageHook Nlm_SetMessageHook

/*************************************************************************\
|                                 BEEPS                                   |
\*************************************************************************/

typedef void (LIBCALLBACK *BeepHook) PROTO((void));

void LIBCALL Nlm_Beep PROTO((void));
BeepHook LIBCALL Nlm_SetBeepHook PROTO((BeepHook));

#ifndef DCLAP
/* dang conflict w/ Windows Beep beep */
#define Beep Nlm_Beep
#endif
#define SetBeepHook Nlm_SetBeepHook

/*************************************************************************\
|                            PROGRESS MONITORS                            |
\*************************************************************************/
/* Monitor type codes */
typedef enum _MonType_
{
	MonType_Int =1, MonType_Str
}
MonType;

/* Monitor notification codes */
typedef enum _MonCode_
{
	MonCode_Create=1,
	MonCode_Destroy,
	MonCode_IntValue,
	MonCode_StrValue
}
MonCode;

typedef struct _Monitor_
{
	MonType     type;
	const char *strTitle;
	long        intValue;
	const char *strValue;
	long        num1;
	long        num2;
	void       *extra;	/* hook function may put things here */
}
Nlm_Monitor, *Nlm_MonitorPtr;

#ifndef DCLAP
#define Monitor Nlm_Monitor
#define MonitorPtr Nlm_MonitorPtr
#endif

typedef int (LIBCALLBACK *Nlm_MonitorHook) PROTO((Nlm_MonitorPtr pMon, MonCode code));
#define MonitorHook Nlm_MonitorHook

Nlm_MonitorPtr LIBCALL Nlm_MonitorIntNew PROTO((Nlm_CharPtr title, Nlm_Int4 n1, Nlm_Int4 n2));
Nlm_MonitorPtr LIBCALL Nlm_MonitorStrNew PROTO((Nlm_CharPtr title, Nlm_Int2 len));
Nlm_Boolean LIBCALL Nlm_MonitorStrValue PROTO((Nlm_MonitorPtr mon, Nlm_CharPtr sval));
Nlm_Boolean LIBCALL Nlm_MonitorIntValue PROTO((Nlm_MonitorPtr mon, Nlm_Int4 ival));
Nlm_MonitorPtr LIBCALL Nlm_MonitorFree PROTO((Nlm_MonitorPtr mon));
MonitorHook LIBCALL Nlm_SetMonitorHook PROTO((MonitorHook hook));

#define MonitorIntNew Nlm_MonitorIntNew
#define MonitorStrNew Nlm_MonitorStrNew
#define MonitorStrValue Nlm_MonitorStrValue
#define MonitorIntValue Nlm_MonitorIntValue
#define MonitorFree Nlm_MonitorFree
#define SetMonitorHook Nlm_SetMonitorHook

/*****************************************************************************
*
*   Default Monitor
*
*****************************************************************************/
typedef Nlm_Boolean (LIBCALLBACK * Nlm_ProgMonFunc) PROTO((Nlm_VoidPtr data, Nlm_CharPtr str));

Nlm_Boolean LIBCALL Nlm_ProgMon PROTO((Nlm_CharPtr str));
Nlm_Boolean LIBCALL Nlm_SetProgMon PROTO((Nlm_ProgMonFunc func, Nlm_VoidPtr data));
Nlm_Boolean LIBCALLBACK Nlm_StdProgMon PROTO((Nlm_VoidPtr data, Nlm_CharPtr str));

#define ProgMonFunc Nlm_ProgMonFunc
#define ProgMon Nlm_ProgMon
#define SetProgMon Nlm_SetProgMon
#define StdProgMon Nlm_StdProgMon

#ifdef __cplusplus
}
#endif

#endif
