/*   ncbimsg.c
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
* File Name:  ncbimsg.c
*
* Author:  Gish, Kans, Ostell, Schuler
*
* Version Creation Date:   2/13/91
*
* $Revision: 2.19 $
*
* File Description:
*   	user alert and error messages
*
* Modifications:
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 2/13/91  Kans        Now only used for non-Vibrant versions
* 09-19-91 Schuler     Added LIBCALL to Nlm_Beep()
* 09-20-91 Schuler     Include <conio.h> for getche() prototype
* 04-15-93 Schuler     Changed _cdecl to LIBCALL
* 05-28-93 Schuler     New function: SetMonitorHook()
* 06-06-93 Schuler     Added code to set/check magic_value for Monitors
* 01-13-94 Schuler     Converted ErrPost to ErrPostEx
* 01-13-94 Schuler     Added SetMsgHook and SetBeepHook
* 01-31-94 Schuler     Modified Message and _DefMessageHook (see below)
* 02-07-94 Schuler     Added element to _sev_code[] array for SEV_NONE
* ==========================================================================
*/

extern char * g_corelib;
static char * this_file = __FILE__;

#undef  THIS_MODULE
#define THIS_MODULE g_corelib
#undef  THIS_FILE
#define THIS_FILE this_file

#include <ncbi.h>
#include <ncbiwin.h>

#ifdef DCLAP
/* add back the undef from header */
#define Monitor Nlm_Monitor
#define MonitorPtr Nlm_MonitorPtr
#endif

#ifdef VAR_ARGS
#include <varargs.h>
#define VSPRINTF(buff,fmt)         \
	{                              \
		va_list args;              \
		va_start(args);            \
		vsprintf(buff,fmt,args);   \
		va_end(args);              \
	}

#else
#include <stdarg.h>
#define VSPRINTF(buff,fmt)         \
	{                              \
		va_list args;              \
		va_start(args,fmt);        \
		vsprintf(buff,fmt,args);   \
		va_end(args);              \
	}
#endif


MsgAnswer PASCAL _DefMessageHook PROTO((MsgKey key, ErrSev sev,
			const char *caption, const char *message));
			
void PASCAL _DefBeepHook PROTO((void));

int PASCAL _DefMonitorHook PROTO((MonitorPtr pMon, MonCode code));


/*************************************************************************\
|                    INSTANCE-SPECIFIC INFO STRUCTURE                     |
\*************************************************************************/

typedef struct AppMsgInfo
{
	MessageHook hookMessage;
	BeepHook hookBeep;
	MonitorHook hookMonitor;
}
AppMsgInfo;

static char * _szPropKey = "_AppMsgInfo";

static AppMsgInfo * GetAppMsgInfo (void)

{
	AppMsgInfo *info = (AppMsgInfo*) GetAppProperty(_szPropKey);
	
	if (info == NULL)
	{
		info = (AppMsgInfo*) MemGet(sizeof(struct AppMsgInfo), TRUE);
		if (info == NULL)  AbnormalExit(1);  
			
		info->hookMessage = _DefMessageHook;
		info->hookBeep = _DefBeepHook;
		info->hookMonitor = _DefMonitorHook;
		
		SetAppProperty(_szPropKey,(void*)info);
	}
	return info;
}


/*************************************************************************\
|                                ALERTS                                   |
\*************************************************************************/
#if defined(WIN_DUMB) || defined(OS_MAC)
static int GetOneChar PROTO((void));
#endif

#ifdef WIN_MSWIN
char sProgramName[16];
#endif

/*-------------------------------------------------------------------------
* Message
*
*   Nlm_Message(key, format, ...)
*   	key = type of message
*   	MSG_ERROR 0 = non-fatal error
*   	MSG_FATAL 1 = fatal error
*   	MSG_OK    2 = OK
*   	MSG_RC    3 = Retry Cancel
*   	MSG_ARI   4 = Abort Retry Ignore
*   	MSG_YN    5 = Yes/No
*   	MSG_YNC   6 = Yes/No/Cancel
*       MSG_OKC   7 = OK/Cancel
*   	MSG_POST  8 = show message, no response required
*   	MSG_POSTERR 9 = beep, show message, no response required
*
*   reply is:
*   	0 = no
*   	1 = yes or ok or retry
*   	abort/cancel
*   	3 = ignore
*
*       #define ANS_NO 0
*       #define ANS_YES 1
*       #define ANS_OK 1
*       #define ANS_RETRY 1
*       #define ANS_ABORT 2
*       #define ANS_CANCEL 2
*       #define ANS_IGNORE 3
*
*
* MODIFICATIONS
* 01-31-94 Schuler   Added calls to Beep() and AbnormalExit() where
*                    appropriate instead of expecting the MessageHook
*                    to do this.
*/

extern char *GetScratchBuffer PROTO((void));

#ifdef VAR_ARGS
MsgAnswer CDECL Nlm_Message (sevkey, fmt, va_alist)
	Nlm_Int2 sevkey;
	const char *fmt;
	va_dcl
#else
MsgAnswer CDECL Nlm_Message (Nlm_Int2 sevkey, const char *fmt, ...)
#endif
{
	char *caption = (char*) GetAppProperty("ProgramName");
	char *message = GetScratchBuffer();
	MsgKey key = KEY_OK;
	ErrSev sev = SEV_INFO;
	MsgAnswer ans;

	if (sevkey < KEY_other)
	{
		key = (MsgKey) sevkey;
		sev = SEV_INFO;
	}
	else 
	{
		key = KEY_OK;
		sev = SEV_INFO;

		switch (sevkey)
		{   
			case MSG_ERROR :
				Nlm_Beep();
				sev = SEV_ERROR;
				break;
			case MSG_FATAL :
				Nlm_Beep();
				sev = SEV_FATAL;
				break;
			case MSG_POSTERR :
				Nlm_Beep();
				sev = SEV_ERROR;
				key = KEY_NONE;
				break;
			case MSG_POST :
				key = KEY_NONE;
				break;
		}
	}
	
	VSPRINTF(message,fmt);
	ans = MsgAlertStr(key,sev,caption,message);

	if (sevkey == MSG_FATAL)
		AbnormalExit(1);

	return ans;
}


/*-------------------------------------------------------------------------
* MsgAlert  [Schuler, 01-13-94]
*/

#ifdef VAR_ARGS
MsgAnswer CDECL Nlm_MsgAlert (key, sev, caption, fmt, va_alist)
	MsgKey key;
	ErrSev sev;
	const char *caption;
	const char *fmt;
	va_dcl
#else
MsgAnswer CDECL Nlm_MsgAlert (MsgKey key, ErrSev sev, const char *caption, const char *fmt, ...)
#endif
{
	char *message = GetScratchBuffer();
	VSPRINTF(message,fmt);
	return MsgAlertStr(key,sev,caption,message);
}


/*-------------------------------------------------------------------------
* MsgAlertStr  [Schuler, 01-13-94]
*/
MsgAnswer LIBCALL Nlm_MsgAlertStr (MsgKey key, ErrSev sev, 
			const char *caption, const char *message)
{
	MessageHook hook = GetAppMsgInfo()->hookMessage;
	return (*hook)(key,sev,caption,message);
}


/*-------------------------------------------------------------------------
* _DefMessageHook  [Schuler, 01-13-94, from the old Message]
*
* MODIFICATIONS:
* 01-24-94 Schuler   Check for NULL message string
* 01-31-94 Schuler   Removed Beep() and AbnormalExit() calls
*/

#ifdef WIN_MSWIN
static UINT _sev_code[] = { 
	/* SEV_NONE */    MB_OK,
	/* SEV_INFO */    MB_ICONINFORMATION,
	/* SEV_WARNING */ MB_ICONASTERISK, /* same as MB_ICONINFORMATION */
	/* SEV_ERROR */   MB_ICONEXCLAMATION,
	/* SEV_FATAL */   MB_ICONHAND };
#endif

#if defined(WIN_DUMB) || defined(OS_MAC)
static char * _key_str [] = { 
	/* KEY_NONE */ "",
	/* KEY_OK */   "Hit Return  ", 
	/* KEY_OKC */  "C = Cancel, Anything else = OK  ",
	/* KEY_ARI */  "A = abort, R = retry, I = ignore  ",
	/* KEY_YNC */  "Y = yes, N = no, C = cancel  ",
	/* KEY_YN */   "Y = yes, N = no  ",
	/* KEY_RC */   "R = retry, C = Cancel  " };
#endif


MsgAnswer PASCAL _DefMessageHook (MsgKey key, ErrSev sev, 
			const char *caption, const char *message)
{
	MsgAnswer answer = ANS_NONE;
	
#ifdef WIN_MSWIN
	UINT flags = MB_TASKMODAL | _sev_code[(int)sev];
	if (key > 0)  flags |= (key-1);                    
	answer = MessageBox(NULL,message,caption,flags);
	if (sev == SEV_FATAL)
		AbnormalExit(1);
#endif

#if defined(WIN_DUMB) || defined(OS_MAC)
	fflush(stdout);
	if (message != NULL)	
		fprintf(stderr,"%s\n",message);
	
	if (key>KEY_NONE && key<KEY_other)
	{
		int ch;
		
		/* show prompt */		
		fprintf(stderr,"%s  ",_key_str[(int)key]);	

		/* set default value */
		switch (key)  
		{
			case KEY_OK :
			case KEY_OKC :
				answer = ANS_OK;
				break;
			case KEY_ARI :
			case KEY_RC :
				answer = ANS_RETRY;
				break;
			case KEY_YNC :
			case KEY_YN :
				answer = ANS_YES;
				break;
		}
	
		/* get response */
		ch = GetOneChar();
		ch = isalpha(key) ? toupper(ch) : ch;
		switch (ch)
		{
			case 'A' :
				answer = ANS_ABORT;
				break;
			case 'C' :
				answer = ANS_CANCEL;
				break;
			case 'I' :
				answer = ANS_IGNORE;
				break;
			case 'N' :
				answer = ANS_NO;
				break;
		}
	}
#endif

	return answer;
}

/*-------------------------------------------------------------------------
* SetMessageHook  [Schuler, 01-13-94]
*/
MessageHook LIBCALL Nlm_SetMessageHook (MessageHook hook)
{
	AppMsgInfo *info = GetAppMsgInfo();
	MessageHook hookPrev = info->hookMessage;
	if (hookPrev ==_DefMessageHook) hookPrev = NULL;
	info->hookMessage = (hook == NULL) ? _DefMessageHook : hook;
	return hookPrev;
}

/*-------------------------------------------------------------------------
* GetOneChar
*
* Gets a single character from the console
*
* MODIFICATIONS
* 01-13-94 Schuler   Simplified (old version preserved below)
*/
#if defined(WIN_DUMB) || defined(OS_MAC)

#if defined(COMP_MSC) || defined(COMP_BOR) || defined(COMP_SYMC)
#include <conio.h>		// for getche prototype
#endif

static int GetOneChar ()
{
	int value =0;

#if defined(COMP_MSC) || defined(COMP_BOR) || defined(COMP_SYMC)
	value = getche();
	putchar('\n');
#else
	int tvalue;
	do
	{
		tvalue = getchar();
		if (! value)
			value = tvalue;
	} while (tvalue != '\n');
#endif
	return value;
}
#endif


/*************************************************************************\
|                                 BEEPS                                   |
\*************************************************************************/


/*-------------------------------------------------------------------------
* Beep
*
* MODIFICATIONS
* 01-13-94 Schuler   Modified to use BeepHook (old version preserved below)
*/

void LIBCALL  Nlm_Beep ()
{
	BeepHook hook = GetAppMsgInfo()->hookBeep;
	(*hook)();
}


/*-------------------------------------------------------------------------
* _DefBeepHook  [Schuler, 01-13-94, from old Beep code]
*
* Default beep function
*/
void LIBCALLBACK _DefBeepHook ()
{
#ifdef OS_MAC
	SysBeep(60);
#endif
#ifdef WIN16
	MessageBeep(0);
#endif
#if defined(WIN32) || defined(WIN32BOR)
	Beep(60,10);
#endif
#ifdef WIN_DUMB
	putc(7,stderr);
#endif
}

/*-------------------------------------------------------------------------
* SetBeepHook  [Schuler, 01-13-94]
*/
BeepHook LIBCALL Nlm_SetBeepHook (BeepHook hook)
{
	AppMsgInfo *info = GetAppMsgInfo();
	BeepHook hookPrev = info->hookBeep;
	if (hookPrev == (BeepHook)_DefBeepHook) hookPrev = NULL;
	info->hookBeep = (hook == NULL) ? _DefBeepHook : hook;
	return hookPrev;
}

/*************************************************************************\
|                            PROGRESS MONITORS                            |
\*************************************************************************/

#define MON_MAGIC_VALUE		1234

/*
#define MON_SET_MAGIC(x)	(x)->magic_value = MON_MAGIC_VALUE
#define MON_IS_VALID(x)		((x)!=NULL  && (x)->magic_value==MON_MAGIC_VALUE)
*/
#define MON_MAGIC(x)	   *((int*)((char*)(x) + sizeof(Monitor)))
#define MON_SET_MAGIC(x)   MON_MAGIC(x) = MON_MAGIC_VALUE
#define MON_IS_VALID(x)    (MON_MAGIC(x) == MON_MAGIC_VALUE)


/*-------------------------------------------------------------------------
* MonitorIntNew
*
* Creates an integer range monitor
*
* MODIFICATIONS
* 05-27-93 Schuler   Support for applications hooking into monitors
* 06-03-93 Schuler   Use of magic number to detect invalid pointers
*/

MonitorPtr LIBCALL Nlm_MonitorIntNew (Nlm_CharPtr title, Nlm_Int4 n1, Nlm_Int4 n2)
{
	AppMsgInfo *info = GetAppMsgInfo();
	Nlm_sizeT bytes = sizeof(Monitor) + sizeof(int);
	Monitor *pMon = (Monitor*) MemNew(bytes);

	if (pMon != NULL)
	{
		MON_SET_MAGIC(pMon);
		pMon->type = MonType_Int;
		pMon->strTitle = title ? StrSave(title) : NULL;
		pMon->num1 = n1;
		pMon->num2 = n2;
		if (!(*info->hookMonitor)(pMon,MonCode_Create))
		{
		    MonitorFree(pMon);
		    /* only post an information message here; it is expected
		    	that the hook function would report the real reason
		    	that the monitor creation failed. */
			ErrPostEx(SEV_INFO,0,0,"Unable to create monitor");
			return NULL;
		}
	}
	return pMon;
}


/** TO DO:  define error codes for these conditions **/
static char * _invalid_mon = "invalid monitor pointer";
static char * _invalid_type = "invalid monitor type";

/*-------------------------------------------------------------------------
* MonitorStrNew
*
* Creates a string monitor.  The len argument is the maximum string length.
*
* MODIFICATIONS
* 05-27-93 Schuler   Support for applications hooking into monitors
* 06-03-93 Schuler   Use of magic number to detect invalid pointers
*/

MonitorPtr LIBCALL Nlm_MonitorStrNew (Nlm_CharPtr title, Nlm_Int2 len)
{
	AppMsgInfo *info = GetAppMsgInfo();
	Nlm_sizeT bytes = sizeof(Monitor) + sizeof(short);
	Monitor *pMon = (Monitor*) MemNew(bytes);

	if (pMon != NULL)
	{
		MON_SET_MAGIC(pMon);
		pMon->type = MonType_Str;
		pMon->strTitle = title ? StrSave(title) : NULL;
		pMon->num1 = MAX(0,MIN(len,72));
		(*info->hookMonitor)(pMon,MonCode_Create);
	}
	return pMon;
}


/*-------------------------------------------------------------------------
* MonitorStrValue
*
* Sets the string value for a string monitor.
*
* MODIFICATIONS
* 05-27-93 Schuler   Support for applications hooking into monitors
* 06-03-93 Schuler   Use of magic number to detect invalid pointers
* 06-03-93 Schuler   Check that monitor is correct type (MonType_Str)
*/

Nlm_Boolean LIBCALL Nlm_MonitorStrValue (MonitorPtr pMon, Nlm_CharPtr sval)
{
	AppMsgInfo *info = GetAppMsgInfo();
	if ( ! MON_IS_VALID(pMon) )
	{
		ErrPostEx(SEV_WARNING,0,0,"MonitorStrValue: %s",_invalid_mon);
		return FALSE;
	}
	if (pMon->type != MonType_Str)
	{
		ErrPostEx(SEV_WARNING,0,0,"MonitorStrValue: %s",_invalid_type);
		return FALSE;
	}

	if (pMon->strValue) MemFree((void*)pMon->strValue);
	pMon->strValue = sval ? StrSave(sval) : NULL;
	(*info->hookMonitor)(pMon,MonCode_StrValue);
	return TRUE;
}


/*-------------------------------------------------------------------------
* MonitorIntValue
*
* Sets the integer value for an integer range monitor
*
* MODIFICATIONS
* 05-27-93 Schuler   Support for applications hooking into monitors
* 06-03-93 Schuler   Use of magic number to detect invalid pointers
* 06-03-93 Schuler   Check that monitor is correct type (MonType_Int)
*/

Nlm_Boolean LIBCALL Nlm_MonitorIntValue (MonitorPtr pMon, Nlm_Int4 ival)
{
	AppMsgInfo *info = GetAppMsgInfo();
	if ( !MON_IS_VALID(pMon) )
	{
		ErrPostEx(SEV_WARNING,0,0,"MonitorIntValue: %s",_invalid_mon);
		return FALSE;
	}
	if (pMon->type != MonType_Int)
	{
		ErrPostEx(SEV_WARNING,0,0,"MonitorStrValue: %s",_invalid_type);
		return FALSE;
	}

	pMon->intValue = ival;
	(*info->hookMonitor)(pMon,MonCode_IntValue);
	return TRUE;
}


/*-------------------------------------------------------------------------
* MonitorFree
*
* MODIFICATIONS
* 05-27-93 Schuler   Support for applications hooking into monitors
* 06-03-93 Schuler   Use of magic number to detect invalid pointers
*/

MonitorPtr LIBCALL Nlm_MonitorFree (MonitorPtr pMon)
{
	AppMsgInfo *info = GetAppMsgInfo();
	if ( ! MON_IS_VALID(pMon) )
	{
		ErrPostEx(SEV_WARNING,0,0,"MonitorFree: %s",_invalid_mon);
	}
	else
	{
		(*info->hookMonitor)(pMon,MonCode_Destroy);
		MemFree((void*)pMon->strTitle);
		MemFree((void*)pMon->strValue);
		MemFree(pMon);
	}
	return NULL;
}


/*-------------------------------------------------------------------------
* SetMonitorHook	[Schuler, 05-27-93]
*
* Allows the application to set a hook procedure that will be called
* when a monitor is created, destroyed, or changes value.
*
* MODIFICATIONS
* 01-13-94 Schuler   No longer allows you to set the hook to NULL.
*/

MonitorHook LIBCALL Nlm_SetMonitorHook (MonitorHook hook)
{
	AppMsgInfo *info = GetAppMsgInfo();
	MonitorHook hookPrev = info->hookMonitor;
	if (hookPrev ==_DefMonitorHook) hookPrev = NULL;
	info->hookMonitor = (hook == NULL) ? _DefMonitorHook : hook;
	return hookPrev;
}



/*-------------------------------------------------------------------------
*	_DefMonitorHook	[Schuler, 05-27-93]
*
*	IMPORTANT:  If your program is NOT a console-style (WIN_DUMB), you
*	should either (1) use Vibrant or (2) write a monitor hook function
*	and install it with SetMonitorHook.  Otherwise, the following
*	"do-nothing" function will be used and you will not see anything.
*/

#ifndef WIN_DUMB

int PASCAL _DefMonitorHook (MonitorPtr pMon, MonCode code)
{
	switch (code)
	{
		case MonCode_Create :
			TRACE("[%s]\n",pMon->strTitle);
			return TRUE;
		case MonCode_StrValue :
			TRACE("%s\n",pMon->strValue);
			break;
	}
	return 0;
}

#else  /* WIN_DUMB */

static int  Dumb_MonCreate PROTO((MonitorPtr pMon));
static void Dumb_MonDestroy PROTO((MonitorPtr pMon));
static void Dumb_MonIntValue PROTO((MonitorPtr pMon));
static void Dumb_MonStrValue PROTO((MonitorPtr pMon));



int PASCAL _DefMonitorHook (MonitorPtr pMon, MonCode code)
{
	switch (code)
	{
		case MonCode_Create :
			return Dumb_MonCreate(pMon);
			break;
		case MonCode_Destroy :
			Dumb_MonDestroy(pMon);
			break;
		case MonCode_IntValue :
			Dumb_MonIntValue(pMon);
			break;
		case MonCode_StrValue :
			Dumb_MonStrValue(pMon);
			break;
	}
	return 0;
}

static int Dumb_MonCreate (MonitorPtr pMon)
{
	char *buf;

	if ((buf = Nlm_Calloc(81,1)) == NULL)
		return FALSE;
	buf[80] = '\0';
	pMon->extra = buf;

	fprintf(stderr, "\n\n<<< %s >>>\n", pMon->strTitle);

	if (pMon->type == MonType_Int)
	{
		fprintf(stderr, "<-%-8ld--------------------%8ld->\n",pMon->num1,pMon->num2);
	}
	else
	{
		Nlm_sizeT len = (Nlm_sizeT) pMon->num1;

		MemSet(buf, '<', 3);
		MemSet(&buf[3], ' ', len+2);
		MemSet(&buf[len+5], '>', 3);
		buf[80] = '\0';
		fprintf(stderr, "%s", buf);
	}

	fflush(stderr);
	return TRUE;
}

static void Dumb_MonDestroy (MonitorPtr pMon)
{
	MemFree(pMon->extra);
	fprintf(stderr, "\n\n");
	fflush(stderr);
}

static void Dumb_MonIntValue (MonitorPtr pMon)
{
	char *buf = pMon->extra;
	long value = pMon->intValue;
	long range = ABS(pMon->num2 - pMon->num1);
	Nlm_sizeT diff = 0;
	int bRefresh = FALSE;

	if (range ==0)  return;  /* watch out for divide-by-zero! */

	if (pMon->num2 >= pMon->num1)
	{
		if (value > pMon->num1)
		{
			if (value >= pMon->num2)
				diff = 40;
			else
				diff = (Nlm_sizeT) (((value - pMon->num1) * 40) / range);

			if (diff==0 || diff==40 || buf[diff-1] != '#' || buf[diff] != ' ')
			{
				MemSet(&buf[0], '#', diff);
				MemSet(&buf[diff], ' ', 40-diff);
				bRefresh = TRUE;
			}
		}
	}
	else
	{
		if (value < pMon->num1)
		{
			if (value <= pMon->num2)
				diff = 40;
			else
				diff = (Nlm_sizeT) (((value - pMon->num2) * 40) / range);

			if (diff==0 || diff==40 || buf[40-diff] != '#' || buf[40-diff-1] != ' ')
			{
				MemSet(&buf[40 - diff],'#',diff);
				MemSet(&buf[0], ' ', 40-diff);
				bRefresh = TRUE;
			}
		}
	}
	if (bRefresh)
	{
		fprintf(stderr, "\r%s", buf);
		fflush(stderr);
	}
}

static void Dumb_MonStrValue (MonitorPtr pMon)
{
	char *buf = (char *) pMon->extra;
	Nlm_sizeT len0 = (int)pMon->num1;
	Nlm_sizeT len1 = MIN(StringLen(pMon->strValue),len0);
	Nlm_sizeT diff = (len0-len1)/2;
	Nlm_MemFill(&buf[3],' ',len0+2);
	Nlm_MemCopy(&buf[4+diff],pMon->strValue,len1);
	fprintf(stderr,"\r%s",buf);
	fflush(stderr);
}

#endif  /* WIN_DUMB */

/*****************************************************************************
*
*   Default Monitor
*      This will be moved to ncbimsg soon !!!!
*
*****************************************************************************/


static Nlm_VoidPtr stdmondata = NULL;
static Nlm_ProgMonFunc stdmonfunc = NULL;

/*****************************************************************************
*
*   ProgMon(str)
*   	Default progress monitor
*
*****************************************************************************/
Nlm_Boolean LIBCALL Nlm_ProgMon(Nlm_CharPtr str)
{
	Nlm_Boolean retval;

	if (stdmonfunc == NULL)
		return TRUE;

	retval = (* stdmonfunc)(stdmondata, str);
	
	return retval;
}


/*****************************************************************************
*
*   SetProgMon(func, data)
*
*****************************************************************************/
Nlm_Boolean LIBCALL Nlm_SetProgMon (ProgMonFunc func, Nlm_VoidPtr data)
{
	stdmonfunc = func;
	stdmondata = data;
	return TRUE;
}


/*****************************************************************************
*
*   StdProgMon(data, str)
*
*****************************************************************************/
Nlm_Boolean LIBCALLBACK Nlm_StdProgMon(Nlm_VoidPtr data, Nlm_CharPtr str)
{
	return Nlm_MonitorStrValue((Nlm_MonitorPtr) data, str);
}

