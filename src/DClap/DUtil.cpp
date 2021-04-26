// DUtil.cp
// d.g.gilbert

// Add these general graphics objects ???

// DFont == enclose an Nlm_FonT item.  
// Include 	ParseFont method: Nlm_FonT itsfont = Nlm_ParseFont ("Times,12");
// set a few global defaults, and let user make other DFont objects as needed.

// DDraw == some drawing primitives...
// InsetRect/OffsetRect/...


#include <ncbi.h>
#include <dgg.h>

#include "DUtil.h"
#include "DFile.h"
#include "DWindow.h"
#include "DTaskCentral.h"
#include "DViewCentral.h"

  
DCursor*	gCursor = NULL;
DMouse*		gMouse = NULL;
DKeys*		gKeys = NULL;
DUtil*		gUtil = NULL;
DPrintManager*	gPrintManager = NULL;
DErrorManager*	gErrorManager = NULL;
DPrefManager*		gPrefManager = NULL;

Global Nlm_FonT gTextFont   	= NULL; //Nlm_programFont; << not initialized at this point.
Global Nlm_FonT gItalicTextFont= NULL;  
Global Nlm_FonT gBoldTextFont  = NULL; 
Global Nlm_FonT gUlineTextFont = NULL; 
Global char	 	* gTextFontName = NULL;
Global short  	gTextFontSize = 12;
Global short		gTextTabStops = 4;


// change NLM error management, esp for MemNew/MemMore failure on out-of-mem
#if 0
	// void LIBCALL  Nlm_ErrSetOpts (short erract, short errlog)
	// void LIBCALL  Nlm_ErrGetOpts (short * erract, short * errlog)
/* actopt codes */
#define ERR_CONTINUE  1
#define ERR_IGNORE    2
#define ERR_ADVISE    3
#define ERR_ABORT     4
#define ERR_PROMPT    5
#define ERR_TEE       6
/* logopt codes */
#define ERR_LOG_ON    1
#define ERR_LOG_OFF   2

typedef struct _ErrDesc {
    int   context;
    int   errcode;
	char  errtext [ERRTEXT_MAX+1];
	char  srcfile [PATH_MAX+1];
    int   srcline;
} ErrDesc, PNTR ErrDescPtr;

typedef void (LIBCALLBACK *ErrHookProc) PROTO((const ErrDesc *err));
#endif


//class DErrorHandler	: public DObject

void DErrorManager::TurnOff() 	
{ 
	Nlm_ErrGetOpts( &fErract, &fErrlog); 
	Nlm_ErrSetOpts(ERR_CONTINUE,ERR_LOG_OFF); 
}

void DErrorManager::TurnOn() 	
{ 
	Nlm_ErrSetOpts( fErract, fErrlog); 
}
 

//class DKeys

char	DKeys::current()		{ return Nlm_currentKey; }
Boolean	DKeys::shift()		{ return Nlm_shftKey; }
Boolean	DKeys::control()	{ return Nlm_ctrlKey; }
Boolean	DKeys::option()		{ return Nlm_optKey; }
Boolean	DKeys::command()	
{ 
#ifdef WIN_MAC
	return Nlm_cmmdKey; 
#endif
#ifdef WIN_MSWIN
	return Nlm_ctrlKey; // or alt !!??
#endif
#ifdef WIN_MOTIF
	return Nlm_cmmdKey; 
#endif
	return Nlm_ctrlKey; 
}


//class DKeyCallback

Global DKeyCallback* gKeyHandlerObject = NULL;

extern "C" void keyHandlerProc( char c)
{
	if (gKeyHandlerObject) gKeyHandlerObject->ProcessKey(c);
}

DKeyCallback::DKeyCallback()
{
	gKeyHandlerObject= this;
 	Nlm_KeyboardView(keyHandlerProc); //Nlm_KeyProc key
}

DKeyCallback::~DKeyCallback()
{
	if (gKeyHandlerObject == this) gKeyHandlerObject= NULL;
}
  
void DKeyCallback::SetKeyHandler(DKeyCallback* handler)
{
	gKeyHandlerObject= handler;
}


//class	DPrefManager	: public DObject

Local char* kDefaultPrefApp = "DClap";

char* DPrefManager::GetAppPref(char* type, char* section, char* appname, char* defaultvalue,
												char* prefvalue, ulong maxsize) 
{ 
	if (prefvalue==NULL) prefvalue= (char*)MemNew(maxsize);
	long buflen= Nlm_GetAppParam(appname, section, type, defaultvalue, prefvalue, maxsize);
	return prefvalue;
}
			
char* DPrefManager::GetPref(char* type, char* section, char* defaultvalue,
										char* prefvalue, ulong maxsize) 
{ 
	return GetAppPref(type, section, kDefaultPrefApp, defaultvalue, prefvalue, maxsize);
}

Boolean DPrefManager::SetAppPref(char* prefvalue, char* type, char* section, char* appname) 
{ 
	return Nlm_SetAppParam(appname, section, type, prefvalue);
}

Boolean DPrefManager::SetPref(char* prefvalue, char* type, char* section) 
{ 
	return SetAppPref( prefvalue, type, section, kDefaultPrefApp);
}

Boolean DPrefManager::RemovePrefSection(char* section, char* appname) 
{ 
	return Nlm_SetAppParam(appname, section, NULL, NULL);
}

char* DPrefManager::GetAppPrefSection(char* section, ulong& sectlen, char* appname, char* defaultvalue,
											char* prefvalue, ulong maxsize) 
{ 
	// NOTE: this reads SECTION TYPE NAMES only, e.g.  
	// [fonts]
	// myfont=times
	// hisfont=courier
	// misfont=symbol
	// --- result is list of "myfont.hisfont.misfont." (null term at end of each name)
	
	if (prefvalue==NULL) prefvalue= (char*)MemNew(maxsize);
	sectlen= Nlm_GetAppParam(appname, section, NULL, defaultvalue, prefvalue, maxsize);
	return prefvalue;
}
			
char* DPrefManager::GetPrefSection(char* section, ulong& sectlen, char* defaultvalue,
												char* prefvalue, ulong maxsize) 
{ 
	return GetAppPrefSection(section, sectlen, kDefaultPrefApp, defaultvalue, prefvalue, maxsize);
}

char** DPrefManager::GetPrefFileLines(const char* prefFile, short& nlines) 
{
		char** linelist = NULL;
		DFile aFile(prefFile,"r");
		aFile.Open();
 		linelist= Dgg_ReadDefaultPrefs( aFile.fFile, &nlines);
		aFile.Close();
 		return linelist;
}





//class	DPrintManager	: public DObject

DPrintManager::DPrintManager()
{
	Nlm_LoadRect( &fPageRect, 0,-1,0,-1);
}

DWindow* DPrintManager::StartPrinting()
{
	return (DWindow*) Nlm_GetObject((Nlm_GraphiC)Nlm_StartPrinting());
}

void DPrintManager::DonePrinting(DWindow* savedWindow)
{
	Nlm_EndPrinting( (Nlm_WindoW)savedWindow->GetNlmObject());
}

Boolean DPrintManager::StartPage()
{
	return Nlm_StartPage();
}

Boolean DPrintManager::DonePage()
{
	return Nlm_EndPage();
}

Boolean DPrintManager::PrintRect(Nlm_RecT& rpt)
{
	return Nlm_PrintingRect(&rpt);
}
 

short	DPrintManager::PageWidth(Boolean onScreen)
{
	short		val;
	Boolean okay= !Nlm_EmptyRect( &fPageRect);
	if (!okay) okay= Nlm_PrintingRect( &fPageRect);
	if (okay) {
		val= fPageRect.right - fPageRect.left;
		if (onScreen) val= MIN(val - 140, Nlm_screenRect.right - Nlm_screenRect.left - 40);
		}
	else 
		val= MIN(500, Nlm_screenRect.right - Nlm_screenRect.left - 50);
	return val;
}

short	DPrintManager::PageHeight(Boolean onScreen)
{
	short		val;
	Boolean okay= !Nlm_EmptyRect( &fPageRect);
	if (!okay) okay= Nlm_PrintingRect( &fPageRect);
	if (okay) {
		val= fPageRect.bottom - fPageRect.top;
		if (onScreen) val= MIN(val - 100, Nlm_screenRect.bottom - Nlm_screenRect.top - 80);
		}
	else 
		val= MIN(700, Nlm_screenRect.bottom - Nlm_screenRect.top - 80);
	return val;
}



//DClapGlobals	globals; // initializes globals here

 

DClapGlobals::DClapGlobals()
{
	gCursor = new DCursor();
	gMouse = new DMouse();
	gKeys = new DKeys();
	gUtil = new DUtil();
	gFileManager = new DFileManager();
	gPrintManager = new DPrintManager();
	gErrorManager= new DErrorManager();
	gPrefManager= new DPrefManager();

	gWindowManager = new DWindowManager(); 	// "DWindow.h"
	gTaskCentral= new DTaskCentral(); 	// DTaskCentral.h
	gViewCentral= new DViewCentral(); 	// DViewCentral.h

	gTextFont   	= Nlm_programFont;  
	gItalicTextFont = Nlm_programFont;  
	gBoldTextFont  	= Nlm_programFont;  
	gUlineTextFont 	= Nlm_programFont;  
}


DClapGlobals::~DClapGlobals()
{
	delete gViewCentral; //->suicide();  
	delete gTaskCentral; //->suicide();  
	delete gWindowManager; //->suicide();
	delete gErrorManager; //->suicide();
	delete gPrefManager; //->suicide();
	delete gPrintManager; //->suicide();
	delete gFileManager; //->suicide();
	delete gUtil; //->suicide();
	delete gKeys; //->suicide();
	delete gMouse; //->suicide();
	delete gCursor; //->suicide();  
}

 