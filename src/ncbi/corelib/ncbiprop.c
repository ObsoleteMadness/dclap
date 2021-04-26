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
* $Revision: 2.12 $
*
* File Description:		Application Property Functions.
*
*		Application properties were	introduced to allow the NCBI Toolbox 
*		to be implemented as a dynamic link library (DLL).  Under Windows, 
*		global variables located in the DLL are not instantiated for each 
*		application	instance, but instead shared by all applications having 
*		a run-time linkage.  Values that might ordinarily be stored in 
*		global variables can instead be saved as application properties
*		(unless sharing is desired, of course).  An application property
*		is simply a pointer (or other value cast to a pointer) that is
*		identified by a string key.  Properties are kept in a linked list
*		(sorted by key) whose head is stored in an application context block.
*		The application context block is created on-demand and marked 
*		with the process ID of the calling application (or it's creation
*		can be forced by calling InitAppContext() at the beginning of your
*		program.  The linked list of (smallish) application context 
*		structures is the only thing allocated from the DLL's data space, 
*		all other memory is owned by the application that called the DLL 
*		is automatically freed by the system when the application terminates.
*		
*		If this code is not actually compiled as a DLL, but bound at link
*		time to a single application in the normal way, the behavior of all
*		functions will be the same.  The only difference being that the
*		list of application context structures will contain exactly one item.
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/
#include <ncbi.h>
#include <ncbiwin.h>




/*	----------------------------------------------------------------------
 *	new_AppProperty			allocates/constructs an AppProperty struct
 *	delete_AppProperty		deallocates/destructs an AppProperty struct
 *
 *	Created: 
 *		06-04-93 Schuler
 *	Modified:
 *		00-00-00 YourName	What changes did you make?
 *	----------------------------------------------------------------------	*/

typedef struct _AppProperty_
{
	struct _AppProperty_	*next;
	char *key;
	void *data;
}
AppProperty;

static AppProperty * new_AppProperty PROTO((const char *key, void *data));
static void delete_AppProperty PROTO((AppProperty *prop));

static AppProperty * new_AppProperty (const char *key, void *data)
{
	AppProperty *prop = MemNew(sizeof(AppProperty));

	ASSERT(key != NULL);
	ASSERT(*key != '\0');
	
	if (prop != NULL)
	{
		if ( ! (prop->key = StrSave(key)) )
			prop = MemFree(prop);
		else
			prop->data = data;
	}
	return prop;
}

static void delete_AppProperty (AppProperty *prop)
{
	ASSERT(prop != NULL);
	MemFree(prop->key);
	MemFree(prop);
}



/*	----------------------------------------------------------------------
 *	new_AppContext			allocates/constructs an AppContext struct
 *	delete_AppContext		deallocates/destructs an AppContext struct
 *
 *  Notes:  (1) If a valid context cannot be created, the application halts.
 *			(2) The memory allocated for the AppContext struct is owned by
 *				the DLL and the scratch buffer is owned by the application.
 *			(3) It is important that this function NOT call TRACE, Message,
 *				or ErrPost (or anything else that could result in any of these
 *				being called) as they use the applicaion context's scratch buffer.
 *
 *	Created: 
 *		06-04-93 Schuler
 *	Modified:
 *		00-00-00 YourName	What changes did you make?
 *	----------------------------------------------------------------------	*/

#define SCRATCH_SIZE_DEFAULT	(2*KBYTE)

typedef struct _AppContext_
{
	struct _AppContext_ 	*next;
	struct _AppProperty_	*proplist;
	long 	pid;
	unsigned	enums	:15;	/* number of nested enumerations in-progress */
	unsigned	lock	:1;		/* if TRUE, property list is locked */
	size_t	scratch_size;
	void	*scratch;
}
AppContext;

		/* this is the only global variable in this file: */
static AppContext * g_appList;	/* Application Context List */

static AppContext * new_AppContext PROTO((long pid));
static void delete_AppContext PROTO((AppContext *prop));
INLINE static void  AppContext_Lock PROTO((AppContext *context));
INLINE static void  AppContext_Unlock PROTO((AppContext *context));
INLINE static unsigned  AppContext_IsLocked PROTO((AppContext *context));


static AppContext * new_AppContext (long pid)
{
	AppContext *context = dll_Malloc(sizeof(AppContext));
	
	if (context == NULL)
		AbnormalExit(1);
		
	memset((void*)context,0,sizeof(struct _AppContext_));
	context->pid = pid;
	context->scratch_size = SCRATCH_SIZE_DEFAULT;
	context->scratch = Malloc(SCRATCH_SIZE_DEFAULT);
	if (context->scratch == NULL)
		AbnormalExit(1);    
	return context;
}

static void delete_AppContext (AppContext *context)
{
	AppProperty *p1, *p2;
	
	ASSERT(context != NULL);
	
	for (p1=context->proplist; p1; p1=p2)
	{
		p2 = p1->next;
		delete_AppProperty(p1);
	}
	Free(context->scratch);
	dll_Free(context);
}

static INLINE void AppContext_Lock (AppContext *context)
{
	ASSERT(context->lock==0);
	context->lock = 1;
}

static INLINE void AppContext_Unlock (AppContext *context)
{
	ASSERT(context->lock==1);
	context->lock = 0;
}

static INLINE unsigned AppContext_IsLocked (AppContext *context)
{
	return context->lock;
}



/*	----------------------------------------------------------------------
 *	InitAppContext -- Initializes a context struct for current application.
 *
 *  Notes:  
 *		If a valid context cannot be created, the application halts. 
 *		Although it is not strictly necessary to call InitAppContext() as
 *		contexts are created on-demand, calling it once at the beginning
 *		of the program may reduce heap fragmentation.
 *
 *	Created: 
 *		06-04-93 Schuler
 *	Modified:
 *		00-00-00 YourName	What changes did you make?
 *
 *	----------------------------------------------------------------------	*/

/* helper functions for internal use only */
AppContext *GetAppContext PROTO((void));
char * GetScratchBuffer PROTO((void));
size_t GetScratchBufferSize PROTO((void));


void LIBCALL Nlm_InitAppContext ()
{
	GetAppContext ();
}


AppContext *GetAppContext (void)
{
	long pid = Nlm_GetAppProcessID();
	AppContext *p1, *p2;
	AppContext *app;
	
	/*
	 *	First we scan the list of contexts for one with the current
	 *	application's process ID.
	 */
	for (p1=g_appList,p2=NULL; p1; p1=p1->next)
	{
		if (p1->pid == pid) return p1;
		if (p1->pid > pid) break;
		p2 = p1;
	}
	
	/* 
	 *	If we reach this point, the context for current does not 
	 *	exist yet, so we need to create one and link it into the list.
	 */
	app = new_AppContext(pid);

	if (p2 == NULL)	 
		g_appList = app;
	else  
		p2->next = app;	
	app->next = p1;
	return app;
}

char * GetScratchBuffer ()
{
	return GetAppContext()->scratch;
}

size_t GetScratchBufferSize ()
{
	return GetAppContext()->scratch_size;
}



/*	----------------------------------------------------------------------
 *	ReleaseAppContext -- frees application context struct for current app.
 *
 *	Notes:
 *		For most platforms, memory will be recovered automatically by the
 *		operating system.  However, since we cannot guarantee this for
 *		all systems, it might be wise to call ReleaseAppContext() once
 *		just before the application exits.
 *
 *	Created: 
 *		06-04-93 Schuler
 *	Modified:
 *
 *	----------------------------------------------------------------------	*/

void LIBCALL Nlm_ReleaseAppContext (void)
{
	long pid = Nlm_GetAppProcessID();
	AppContext *p1, *p2;
	
	/*
	 *	Scan the list for the context of the current app
	 */
	for (p1=g_appList,p2=NULL; p1; p1=p1->next)
	{
		if (p1->pid == pid) break;
		p2 = p1;
	}
	/*
	 *	Adjust links and release memory 
	 */
	if (p1 != NULL)
	{
		if (p2 == NULL)
			g_appList = p1->next;
		else
			p2->next = p1->next;
			
		delete_AppContext(p1);
	}
}



/*	----------------------------------------------------------------------
 *	SetAppProperty -- Sets a data item int the application context's 
 *		property list, replacing the existing one or creating a new
 *		one if no property with that key exists.
 *
 *	Parameters:
 *		key:		key identifying the property
 *		value:		pointer to arbitrary data
 *
 *	Return value:	
 *		Previous value of the property, if any, or NULL otherwise.
 *
 *	Created: 
 *		06-08-93	Schuler
 *	Modified:
 *		00-00-00	YourName	What changes did you make?
 *	----------------------------------------------------------------------	*/

void * LIBCALL Nlm_SetAppProperty (const char *key, void *data)
{
	if (key && *key)
	{
		AppContext *context = GetAppContext();
		AppProperty *p1, *p2, *prop;
		void *prev;
		int d;
		
		for (p1=context->proplist, p2=NULL; p1; p1=p1->next)
		{
			d = strcmp(key,p1->key);
			if (d < 0)  break;
			if (d==0)
			{
				prev = p1->data;
				p1->data = data;
				return prev;	/* previous value */
			}
			p2 = p1;
		}
		
		/*
		 *	If we reach here, a property with the given key does not exist, so 
		 *	let's create a new one and link it into the list.
		 */
		
		if (AppContext_IsLocked(context))
		{
			TRACE("SetAppProperty:  ** property list is locked **\n");
		}
		else
		{		
			AppContext_Lock(context);
			if ((prop = new_AppProperty(key,data)) != NULL)
			{
				if (p2 == NULL)	 
					context->proplist = prop;
				else  
					p2->next = prop;	
				prop->next = p1;
			}
			AppContext_Unlock(context);
		}
	}	
	return NULL;	/* no previous value */
}



/*	----------------------------------------------------------------------
 *	GetAppProperty -- Retrieves data value that was set with SetAppProperty.
 *
 *	Parameters:
 *		key:	key identifying the property
 *
 *	Return value:	
 *		Value that was set with SetAppProperty or NULL if no property with 
 *		that key exists.
 *
 *	Created: 
 *		06-08-93	Schuler
 *	Modified:
 *		00-00-00	YourName	What changes did you make?
 *
 *	----------------------------------------------------------------------	*/

void * LIBCALL Nlm_GetAppProperty (const char *key)
{
	if (key && *key)
	{
		AppContext *context = GetAppContext();
		AppProperty *prop;
		
		for (prop=context->proplist; prop; prop=prop->next)
		{
			if (strcmp(prop->key,key) ==0)
				return prop->data;
		}
	}
	return NULL;
}



/*	----------------------------------------------------------------------
 *	RemoveAppProperty -- Removes a propery from the application context's
 *		property list (if it exists) and returns the data value that was
 *		set with SetAppParam().
 *
 *	Parameters:
 *		key:	key identifying the property
 *
 *	Return value:	
 *		Value that was set with SetAppProperty or NULL if no property with 
 *		that key exists.
 *
 *	Notes:
 *		It is the responsibiliy of the caller to free whatever resources 
 *		the property's data (return value) may happen to point to.
 *
 *	Created: 
 *		06-08-93	Schuler
 *	Modified:
 *		00-00-00	YourName	What changes did you make?
 *
 *	----------------------------------------------------------------------	*/

void * LIBCALL Nlm_RemoveAppProperty (const char *key)
{
	if (key && *key)
	{
		AppContext *context = GetAppContext();
		
		if (AppContext_IsLocked(context))
		{
			TRACE("RemoveAppProperty:  ** property list is locked **\n");
		}
		else
		{
			AppProperty *p1, *p2;
			int d;
			
			AppContext_Lock(context);
			for (p1=context->proplist, p2=NULL; p1; p1=p1->next)
			{
				d = strcmp(key,p1->key);
				if (d < 0)  break;
				if (d==0)
				{
					void *data = p1->data;
					if (p2 == NULL)
						context->proplist = p1->next;
					else
						p2->next = p1->next;
					delete_AppProperty(p1);
					AppContext_Unlock(context);
					return data;	/* success */
				}
				p2 = p1;
			}
			AppContext_Unlock(context);
		}
	}
	return NULL;	/* failure */		
}



/*	----------------------------------------------------------------------
 *	EnumAppProperties -- Enumerates all application properties, calling
 *		a caller-supplied callback function with the key and data for
 *		each one.
 *
 *	Parameters:
 *		Pointer to the enumeration callback procedure.
 *
 *	Return value:
 *		Number of properties enumerated.
 *
 *	Callback function:
 *
 *		int LIBCALLBACK MyEmumProc (const char *key, void *value)
 *		{
 *			//--- insert your code here ---
 *
 *			// for example:
 *			if (strcmp(key,"MyBigBuffer") ==0)
 *			{
 *				SetAppProperty(key,NULL);
 *				MemFree(data)
 *				return FALSE;	// FALSE to stop enumeration at this point
 *			}
 *			return TRUE;		// TRUE to continue the enumeration.
 *		}
 *
 *	Notes:
 *		It is OK to call SetAppProperty() from within the callback function,
 *		but _only_ to change the value of an existing property.  Any attempt 
 *		to alter the property list, either by calling SetAppProperty() with 
 *		a new key or calling RemoveAppProperty() will fail while an enumera-
 *		tion is in progress.
 *
 *	Created: 
 *		06-09-93	Schuler
 *	Modified:
 *		00-00-00	YourName	What changes did you make?
 *
 *	----------------------------------------------------------------------	*/

int LIBCALL Nlm_EnumAppProperties (Nlm_AppPropEnumProc proc)
{
	int count = 0;
	if (proc != NULL)
	{
		AppContext *context = GetAppContext();
		AppProperty *prop;
		
		if (context->enums==0)
			AppContext_Lock(context);
		context->enums++;
		for (prop=context->proplist; prop; prop=prop->next)
		{
			count ++;
			if ( ! (*proc)(prop->key,prop->data) )
				break;
		}
		context->enums--;
		if (context->enums==0)
			AppContext_Unlock(context);
	}
	return count;
}



/*	----------------------------------------------------------------------
 *	GetAppProcessID  [Schuler, 06-04-93]
 *
 *	Returns an identifier for the current application instance.
 *
 *	Notes:
 *	On the Macintosh, the process ID is a 64-bit value, which is being
 *	condensed down to 32-bits here by XORing the high and low halves
 *	of the value.  I can't guarantee it will be unique (although it
 *	seems to be in practice), but this code is not being dynamically
 *	linked on the Mac, so it doesn't matter.
 *
 *	MODIFICATIONS
 *	04-10-93  Schuler  Added Macintosh version.
 *	12-16-93  Schuler  Added Borland version contributed by M.Copperwhite
 *	----------------------------------------------------------------------	*/

#define USE_GETPID

/* (insert other platform-specific versions here as necessary and #undef USE_GETPID) */


/* ----- Macintosh Version ----- */
#ifdef OS_MAC
#include <Processes.h>
#include <GestaltEqu.h>

long LIBCALL Nlm_GetAppProcessID (void)
{
	long gval;
	ProcessSerialNumber psn;	/* a 64-bit value*/
	
    if (Gestalt (gestaltSystemVersion, &gval) == noErr && (short) gval >= 7 * 256) {
		GetCurrentProcess(&psn);	
		return (psn.highLongOfPSN ^ psn.lowLongOfPSN);	/* merge to 32-bits */
	} else {
		return 1;
	}
}

#undef USE_GETPID
#endif


/* ----- NCBIDLL mod Borland DLL version - call Windows API to get PSP ----- */
#ifdef _WINDLL
#ifdef COMP_BOR

long LIBCALL Nlm_GetAppProcessID (void)
{
  return GetCurrentPDB();
}

#undef USE_GETPID
#endif
#endif


/* ----- Generic Version ----- */
#ifdef USE_GETPID
#if defined(COMP_MSC) || defined(COMP_BOR)
#include <process.h>
#endif

long LIBCALL Nlm_GetAppProcessID (void)
{
	return getpid();
}

#endif


