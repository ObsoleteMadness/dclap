// DApplication.cp


#include "DClap.h"
#include "DFindDlog.h"


#if 0
// Skeleton for application is

extern "C" Int2 Main (void)
{
  DMyApplication* myapp = new DMyApplication();
  myapp->IApplication("App Name");
  myapp->Run();
  return 0;
}

#endif


DApplication* gApplication = NULL;		// the current application object

char* DApplication::kName=  NULL;
char* DApplication::kVersion= NULL;
char* DApplication::kHelpfolder = "Help";
Nlm_RecT DApplication::fgAppWinRect;

extern "C" Nlm_Char		gProgramName[PATH_MAX];// from vibwndws.c

					
class DNoteWindow : public DWindow 
{
public:	
	DNoteWindow(const char* message) :
		DWindow( 0, NULL, DWindow::fixed, -5, -5, -50, -20, NULL)
	{	
	DNotePanel* note= new DNotePanel(0, this, message, 0, 0, Nlm_systemFont, justcenter);
	}

};





class DWindowChoiceDialog : public DWindow 
{
public:
	DListBox * fListBox;
	short			 fChoice;
	DList 	 * fList;
	
	DWindowChoiceDialog();
	void Open();
	void OkayAction();
	DWindow* Result();
};


DWindowChoiceDialog::DWindowChoiceDialog() :
		DWindow( 0, NULL, DWindow::fixed, -10, -10, -50, -20, "Window list", kDontFreeOnClose),
		fChoice(-1),
		fList(NULL),
		fListBox(NULL)
{	
	fList= gWindowManager->GetWindowList();
}

void DWindowChoiceDialog::Open()
{
		// NOTE: this window is in window list !!
	DPrompt* pr= new DPrompt(0, this, "Select a window");	 	
	DListBox* lb= new DListBox(0, this, 20, 5, false);
	fListBox= lb;
	lb->SetResize( DView::relsuper, DView::relsuper);
	lb->StartAppending();
			
	if (fList) {
		char	title[256];
		long i, n= fList->GetSize();
		for (i= n-1; i>=0; i--) {
			DWindow* win= (DWindow*) fList->At(i);
			if (win != this) 
			{
				win->GetTitle(title,256);
				lb->Append( title);
				}
			}
	 	}
	lb->DoneAppending();
	lb->SetValue(1); // doesn't seem active on DListBox display
	
	this->AddOkayCancelButtons();	
	DWindow::Open();
}

void DWindowChoiceDialog::OkayAction() 
{ 
	//fChoice= fListBox->GetValue() - 1;  
		//min GetValue==1, win fChoices == 0..GetSize-2, skip topwin = this
	fChoice= (fList->GetSize() - 1) - (fListBox->GetValue() - 1);   
}
	
DWindow* DWindowChoiceDialog::Result() 
{ 
	if (fChoice<0) return NULL;
	else return (DWindow*) fList->At(fChoice); 
}
	

// CLASS DAppWin
class DAppWin : public DWindow
{
public:
	DAppWin(long id, DTaskMaster* itsSuperior, WindowStyles style,
					short width = -5, short height = -5, short left = -50, short top = -20, 
					char* title = NULL, Nlm_Boolean freeOnClose= kFreeOnClose)
		: DWindow( id, itsSuperior, style, width, height, left, top,
						title, freeOnClose)
	{ }
	virtual void MakeGlobalsCurrent();
	virtual void Close();
	virtual void ResizeWin();
};

void DAppWin::MakeGlobalsCurrent()
{
	ViewRect( fViewrect);  
	if (!Nlm_EmptyRect(&fViewrect)) DApplication::fgAppWinRect= fViewrect;  
}

void DAppWin::Close()
{
	MakeGlobalsCurrent();
	DWindow::Close();
}

void DAppWin::ResizeWin()
{
	DWindow::ResizeWin();
	MakeGlobalsCurrent();
}






// CLASS DApplication

DApplication::DApplication() :
	DTaskMaster(cAppl)
{
	fDone= false;	
	fDidOpenStartDoc= false;	
	fAppID= NULL;
	fAppWindow= NULL;  
	fPathname= NULL;
	fShortname= NULL;
	fAboutLine= NULL;
	fFileSuffix= NULL;
	fAcceptableFileTypes= NULL;
	gApplication= this;
		// default file types
	fFileSuffix = ".txt";
	fAcceptableFileTypes= "TEXT";
}



void DApplication::IApplication(const char* theAppName)
{
	//xdebug("DApplication::IApplication");
	DClapGlobals* initglobals = new DClapGlobals();
	gCursor->watch();  
	
	if (kName) StrNCpy( gProgramName, kName, PATH_MAX); // for vibwndws.c
	fPathname = StrDup( gFileManager->GetProgramPath());
	if (theAppName) 
		fShortname= StrDup( theAppName);
	else if (kName) 
		fShortname= StrDup( kName);
	else {
		fShortname= StrDup( gFileManager->FilenameFromPath(fPathname));   
		char * suf= (char*)gFileManager->FileSuffix(fShortname);
		if (suf) *suf= 0; // we don't want no steenking suffix here
		}
		
	char buf[512];
	sprintf( buf, "About %s...", Shortname()); 
	fAboutLine= StrDup(buf);
	 
	char *tid= DFileManager::TempFilenameonly();
	if (StrNCmp(tid, "tmp", 3)) tid += 2;
	fAppID= StrDup(tid);
	//fAppID[0]= 'D'; //??
	
#ifndef WIN_MAC
	if (!fAppWindow) {
			// Non-Mac apps need an fAppWindow to put any application menus into 
			// eventually want user pref for main wind rect...
    short width= 450, height = 60;
	  Nlm_LoadRect( &fgAppWinRect, 0, 0, width, height);
	  char* srect = gApplication->GetPref( "fgAppWinRect", "windows", "0 0 450 60");
		if (srect) {
			sscanf( srect, "%hd%hd%hd%hd", &fgAppWinRect.left, &fgAppWinRect.top, 
												&fgAppWinRect.right, &fgAppWinRect.bottom);
			width = MAX( 40, fgAppWinRect.right - fgAppWinRect.left); 
			height= MAX( 60, fgAppWinRect.bottom - fgAppWinRect.top);   
			MemFree(srect);
			}
		fAppWindow= new DAppWin(cAppW, this, DWindow::document, 
				width, height, fgAppWinRect.left, fgAppWinRect.top, 
				(char*)Shortname());
		gWindowManager->SetAppWindow(fAppWindow);
		//fAppWindow->Open(); //<<bad here -- main event loop
		}
#endif
	
	if (!gIconList) gIconList = new DIconList(); //<< do in DIconList.cpp as global init.
  gIconList->ReadAppIcons();

	this->InitSpecialEvents();
	this->InstallDefaultPrefs(); // do before calling any GetPref, after ShortName set
	DFindDialog::InitFindDialog();
	
		// read some user prefs
	gTextFontName= this->GetPref( "gTextFontName", "fonts", gTextFontName);
	gTextFontSize= this->GetPrefVal( "gTextFontSize", "fonts", "12");
	gTextTabStops= this->GetPrefVal( "gTextTabStops", "fonts", "4");
	gTextFont		 = Nlm_GetFont( gTextFontName, gTextFontSize, false, false, false, NULL);
	if (!gTextFont) gTextFont= Nlm_programFont;
	gItalicTextFont= Nlm_GetFont( gTextFontName, gTextFontSize, false, true, false, NULL);
	if (!gItalicTextFont) gItalicTextFont= Nlm_programFont;
	gBoldTextFont= Nlm_GetFont( gTextFontName, gTextFontSize, true, false, false, NULL);
	if (!gBoldTextFont) gBoldTextFont= Nlm_programFont;
	gUlineTextFont= Nlm_GetFont( gTextFontName, gTextFontSize, false, false, true, NULL);
	if (!gUlineTextFont) gUlineTextFont= Nlm_programFont;
  
	DCluster::SetFont(Nlm_programFont);  

	this->SetUpMenus();
	this->UpdateMenus(); // !! need to have this called in task loop somewhere !!
	
	//if (gOpenStartDocs) 
	//OpenStartDocs();
} 


DApplication::~DApplication()
{
	if (gIconList) gIconList->suicide();
	if (fAboutLine) MemFree( fAboutLine);
	if (fShortname) MemFree( fShortname);
	if (fPathname) MemFree( fPathname);
	gApplication= NULL;
}  



const char*	DApplication::Pathname(void)
{
	return	fPathname;
}

const char*	DApplication::Shortname(void)
{
	static char buf[256];
	StrNCpy(buf, fShortname, 256); // having problems w/ someone mangling this string !
	return buf;
}




#ifdef WIN_MAC
#ifndef __APPLEEVENTS__
	// undef some conflicts b/n Types.h && NlmTypes
#undef Handle
#include <AppleEvents.h>
#include <Strings.h>
#endif

// ?!? 68k mac calls to here now fail --  
// !! REQUIRES 'pascal' function call settings for Mac68K callbacks !
#define CALLBACKFORM pascal
//#define CALLBACKFORM extern "C"

#else
#define CALLBACKFORM
#endif

#ifdef WIN_MOTIF

#ifdef OS_UNIX_OSF1
#undef _OSF_SOURCE
#undef _POSIX_4SOURCE
#define _ANSI_C_SOURCE
#endif

#include <signal.h>
#if defined(OS_UNIX_SUN) || defined(OS_UNIX_IRIX) || defined(OS_UNIX_LINUX)
#include <sys/wait.h>
#else
#include <wait.h>
#endif

extern int           statargc;
extern char          **statargv;
#endif


// === AppleEvent Numbers ===

enum {
										// Required Suite
ae_OpenApp			= 1001,
ae_OpenDoc			= 1002,
ae_PrintDoc			= 1003,
ae_Quit				= 1004,

										// Core Suite
ae_Clone			= 2001,
ae_Close			= 2002,
ae_CountElements	= 2003,
ae_CreateElement	= 2004,
ae_Delete			= 2005,
ae_DoObjectsExist	= 2006,
ae_GetClassInfo		= 2007,
ae_GetData			= 2008,
ae_GetDataSize		= 2009,
ae_GetEventInfo		= 2010,
ae_Move				= 2011,
ae_Open				= 1002,		// Same as ae_OpenDoc
ae_Print			= 1003,		// Same as ae_PrintDoc
ae_Save				= 2012,
ae_SetData			= 2013,
										// Miscellaneous Standards
ae_ApplicationDied	= 3001,
ae_BeginTransaction	= 3002,
ae_Copy				= 3003,
ae_CreatePublisher	= 3004,
ae_Cut				= 3005,
ae_DoScript			= 3006,
ae_EditGraphic		= 3007,
ae_EndTransaction	= 3008,
ae_ImageGraphic		= 3009,
ae_IsUniform		= 3010,
ae_MakeObjVisible	= 3011,
ae_Paste			= 3012,
ae_Redo				= 3013,
ae_Revert			= 3014,
ae_TransactionTerm	= 3015,
ae_Undo				= 3016,
ae_Select			= 3017
};



#ifdef WIN_MAC

static AEDescList	gAEDocList;
static long gAEndoc, gAEidoc;

static Boolean SetAFileEvent( long inEvent, long outReply)
{    				
	AppleEvent	*aeEvent, *aeReply;
	short				err;
	
	gAEndoc = gAEidoc = 0;
	if (!inEvent) return false;
	aeEvent = (AppleEvent*) inEvent;
	//aeReply = (AppleEvent*) outReply;
	err= AEGetParamDesc( aeEvent, keyDirectObject, typeAEList, &gAEDocList);
	if (err) return false;
	err= AECountItems( &gAEDocList, &gAEndoc);
	return (err == 0);
}

static void ClearAFileEvent()
{    				
	if (gAEndoc) {
		(void) AEDisposeDesc( &gAEDocList);
		gAEndoc= 0;
		}
}

static const char* GetNextAEFile()
{    				
	short				err;
	AEKeyword		keywd;
	DescType		returnedType;
	FSSpec			aFileSpec;
	Size				actualSize;
	
	if (gAEidoc < gAEndoc) {
		gAEidoc++;
		err= AEGetNthPtr( &gAEDocList, gAEidoc, typeFSS, &keywd, &returnedType,
											(void*)&aFileSpec, sizeof(aFileSpec), &actualSize);
		if (err) return NULL; 
#ifdef COMP_THINKC
		PtoCstr ((StringPtr) aFileSpec.name);
#else
		p2cstr((StringPtr) aFileSpec.name);
#endif
		return MacDirIDtoName( aFileSpec.vRefNum, aFileSpec.parID,
														 (char*) aFileSpec.name);
		}
	else
		return NULL;
}
#endif


static Boolean gDidOpenStartDoc = false;

static void HandleFileEvent(long inEvent, long outReply, short tasknum)
{
#ifdef WIN_MAC
	if (::SetAFileEvent( inEvent, outReply)) {
		const char* path;
		do {
			path= ::GetNextAEFile();
			if (path) {
				//gApplication->OpenDocument( (char*) path);
				DTask* filetask= 
				  gApplication->newTask( tasknum, DTask::kMenu, (long)path);
				gApplication->DoMenuTask( tasknum, filetask);
				delete filetask;
				gApplication->fDidOpenStartDoc= true;
				}
		} while (path);
		::ClearAFileEvent();
		}
#endif

#ifdef WIN_MOTIF
	if (statargc > 1 
   && !gDidOpenStartDoc) { 
  //! gApplication->fDidOpenStartDoc) 
		const char* path;
		for (long i=1; i<statargc; i++) {
			path= statargv[i];
			if (path) {
				//gApplication->OpenDocument( (char*) path);
				DTask* filetask= 
				  gApplication->newTask( tasknum, DTask::kMenu, (long)path);
				gApplication->DoMenuTask( tasknum, filetask);
				delete filetask;
				gApplication->fDidOpenStartDoc= true;
				}
			} 
		}
#endif
}



Boolean  DApplication::GotStartDocs()
{
	return fDidOpenStartDoc;
}

Boolean DApplication::OpenStartDocs()
{
	gDidOpenStartDoc= fDidOpenStartDoc;
	HandleSpecialEvent( 0, 0, ae_OpenDoc);
	return fDidOpenStartDoc;
}

#ifdef COMP_CWI
static short int HandleSpecialEvent( long inEvent, long outReply, long inNumber)
#else
CALLBACKFORM short HandleSpecialEvent( long inEvent, long outReply, long inNumber)
#endif
{
	short tasknum;
	
	switch (inNumber) {
	
		case ae_ApplicationDied:
			(void) DChildAppManager::BuryDeadChildApp( inEvent);
			break;
			
		case ae_Quit:
			gApplication->Quit();
			break;

		case ae_OpenApp:
			//StartUp();
			break;
			
		case ae_PrintDoc:
			tasknum= DApplication::kPrintAFile;
			goto caseOpenDoc;

		case ae_OpenDoc:
			tasknum= DApplication::kOpenAFile;
		caseOpenDoc:		
			::HandleFileEvent(inEvent, outReply, tasknum);
			break;
			
		case ae_Copy:
		case ae_Cut:
		case ae_Paste:
		case ae_Redo:
		case ae_Undo:
		default:
			//DSomeone::HandleAppleEvent( inAppleEvent, outAEReply, outResult, inNumber);
			break;
		}

	return 0;
}


#ifdef WIN_MAC
CALLBACKFORM short HandleChildDiedEvent( long inEvent, long outReply, long inNumber)
{
	return ::HandleSpecialEvent( inEvent, outReply, ae_ApplicationDied);
}
#endif

#ifdef WIN_MOTIF
static void HandleChildDiedEvent(int signo)
{
	long 	pid;
	int		status;
	//pid= wait( &status);
	do {
 		pid= waitpid(-1, &status, WNOHANG|WUNTRACED);
 		// note: if child exits w/ error, exit(!0), pid == -1 !
		if (pid>0) ::HandleSpecialEvent( pid, 0, ae_ApplicationDied); 
	} while (pid>0);
}
#endif


short DApplication::HandleSpecialEvent( long inEvent, long outReply, long inNumber)
{
	return ::HandleSpecialEvent(inEvent, outReply, inNumber);
}


void DApplication::InitSpecialEvents()
{
	short err;
#ifdef WIN_MAC

	// problems w/ "aevt-oapp" call bombing on 68k mac but not ppc mac
	
#define aeproccall(x) NewAEEventHandlerProc(x)
//#define aeproccall(x) ((AEEventHandlerUPP)(x))

	err= ::AEInstallEventHandler( kCoreEventClass, kAEOpenApplication,
								aeproccall(::HandleSpecialEvent), 
								//aeproccall(DApplication::HandleSpecialEvent), 
								ae_OpenApp, false);
	err= ::AEInstallEventHandler( kCoreEventClass, kAEQuitApplication, 
								aeproccall(::HandleSpecialEvent), 
								//aeproccall(DApplication::HandleSpecialEvent), 
								ae_Quit, false);

	err= ::AEInstallEventHandler( kCoreEventClass, kAEApplicationDied, 
								aeproccall(::HandleChildDiedEvent), 
								//aeproccall(DApplication::HandleSpecialEvent), 
								ae_ApplicationDied, false);
								
	err = ::AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments,
								aeproccall(::HandleSpecialEvent), 
								//aeproccall(DApplication::HandleSpecialEvent), 
								ae_OpenDoc, false);
	err = ::AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments,
								aeproccall(::HandleSpecialEvent), 
								//aeproccall(DApplication::HandleSpecialEvent), 
								ae_PrintDoc, false);

#endif

#ifdef WIN_MOTIF
	// install signals to trap ?? childDied events at least
	//err= sighold( SIGCHLD);
#ifndef OS_UNIX_IRIX
		// need to figure out IRIX format for this
 	signal( SIGCHLD, HandleChildDiedEvent); 
#endif
#endif
}



Boolean DApplication::InstallDefaultPrefs(const char* defaultSuffix, const char* appName)
{
	Boolean foundprefs, didinstall= false;
	char  *cp, defaultPrefs[512];
	
		// locate any program default preference file
	if (!appName) appName= Shortname();

	StrNCpy(defaultPrefs, (char*)appName, 512);
	cp= (char*)gFileManager->FileSuffix(defaultPrefs); if (cp) *cp= 0;
	StrNCat(defaultPrefs, (char*)defaultSuffix, 512);
	foundprefs= gFileManager->FileExists(defaultPrefs);
	if (!foundprefs) {
		const char* appPath = gFileManager->GetProgramPath();  
		appPath= gFileManager->PathOnlyFromPath( appPath);
		StrNCpy(defaultPrefs, (char*)appPath, 512);
		StrNCat(defaultPrefs, (char*)appName, 512);
		cp= (char*)gFileManager->FileSuffix(defaultPrefs); if (cp) *cp= 0;
		StrNCat(defaultPrefs, (char*)defaultSuffix, 512);
		foundprefs= gFileManager->FileExists(defaultPrefs);
		}
	
	if (foundprefs) {
		short 	iline, nlines = 0;
		char	**linelist;
		char	* cp, * line = NULL, * section = NULL, * varname = NULL, * params = NULL;
		long 		curver= 0, defver= 0;
		 
		curver= this->GetPrefVal("version","general",NULL);
		linelist= gPrefManager->GetPrefFileLines( defaultPrefs, nlines);

			// 1st check quickly if defaultPrefs has anything newer than user prefs
			// if not, exit silently, else put up notice window....
		for (iline= 0; iline<nlines; iline++) {
			line= linelist[iline];
			if (*line == '[') { // sect name 
				cp= StrStr(line,"[version=");
				if (cp) defver= atol(cp+9);
				if (defver > curver) break;
				}
			}
		if (defver <= curver) {
			for (iline= 0; iline<nlines; iline++) {
				line= linelist[iline];
				MemFree(line);
				}
			MemFree(linelist);
			return false;	
			}
			
		Nlm_MonitorPtr progress= Nlm_MonitorIntNew("Installing new preferences...", 0, nlines);
				// ^^^ incorporate Nlm_Monitor into a DCLAP class
		gCursor->watch();
		for (iline= 0; iline<nlines; iline++) {
			if (Nlm_MonitorIntValue(progress, iline)) ;
			line= linelist[iline];
			if (line) {
				if (*line == '[') { // sect name 
					cp= StrStr(line,"[version=");
					if (cp)  
						defver= atol(cp+9);
					else {
						if (section) MemFree(section);
						section= StrDup(line+1);
						}
					}
				else {
					params= StrChr(line, '=');
					if (params) { *params++= 0; if (!*params) params= NULL; }
					varname= line;
					if (defver > curver) {
						this->SetPref( params, varname, section);
						}
					}
				MemFree(line);
				}
			}
		MemFree(linelist);
		this->SetPref(defver, "version","general");
		didinstall= true;
		Nlm_MonitorFree(progress);		
		}
 	return didinstall;
}




char*	DApplication::GetPref(char* type, char* section, char* defaultvalue)
{
	return gPrefManager->GetAppPref( type, section, (char*)Shortname(), defaultvalue);
}

long	DApplication::GetPrefVal(char* type, char* section, char* defaultvalue)
{
	long val= 0;
	char* sval= gPrefManager->GetAppPref( type, section, (char*)Shortname(), defaultvalue);
	if (sval) {
		val= atol(sval);
		MemFree(sval);
		}
	return val;
}

char*	DApplication::GetPrefSection(char* section, ulong& sectlen, char* defaultvalue)
{
	return gPrefManager->GetAppPrefSection(section, sectlen, (char*)Shortname(), defaultvalue);
}


Boolean DApplication::SetPref(char* prefvalue, char* type, char* section)
{
	return gPrefManager->SetAppPref(prefvalue, type, section, (char*)Shortname());
}

Boolean DApplication::SetPref(long prefvalue, char* type, char* section)
{
	char	snum[256];
	sprintf(snum, "%d", prefvalue);
	return gPrefManager->SetAppPref(snum, type, section, (char*)Shortname());
}


char*	DApplication::ConvertStdFilePath(char* pathname)
{
	char	 *path, *atcolon, *name, *newpath;
	
	pathname= StrDup( pathname);
	atcolon= StrChr(pathname, ':');
	if (atcolon) {
		name= atcolon+1;
		*atcolon= 0;
		path= this->GetPref( pathname, "paths", NULL);
			// GetPref always returns nonNULL path
		if (path && *path != '\0') {
defaultpath:
			DFileManager::UnixToLocalPath(path);
			if (DFileManager::IsRelativePath(path)) {
				const char* appPath= DFileManager::PathOnlyFromPath( fPathname);
				newpath= StrDup( DFileManager::BuildPath( appPath, path, name)); 
				}
			else
				newpath= StrDup( DFileManager::BuildPath( path, NULL, name)); 
			MemFree( pathname);
			MemFree( path);
			pathname= newpath;
			}
		else if (StrICmp( pathname, "temp")==0) {
			path= DFileManager::TempFolder();
			newpath= StrDup( DFileManager::BuildPath( path, NULL, name)); 
			MemFree( pathname);
			pathname= newpath;
			}
#ifdef WIN_MAC
		else {
			*atcolon= ':';
			}
#else
		else if (path)
			goto defaultpath;
		else {
			// return full pathname !?
			// MemMove(pathname, name, StrLen(name)+1);
			}
#endif
		}
	return pathname;
}

char*	DApplication::GetFilePref(char* type, char* section, char* defaultvalue)
{
	char * pathname, *newpath;

	pathname = this->GetPref( type, section, defaultvalue);
	newpath = ConvertStdFilePath( pathname);
	MemFree( pathname);
	return newpath;
}






#ifdef WIN_MOTIF
extern void TaskCentralIdleEvent()
{
	gTaskCentral->NextTask();
}
#endif

void	DApplication::Run(void)
{
	if (fAppWindow)  fAppWindow->Open();
	if (gClipboardMgr) gClipboardMgr->Launch();	 

	gCursor->arrow();  
#ifdef WIN_MOTIF
  Nlm_Metronome( TaskCentralIdleEvent);
#endif
	this->MainEventLoop();
	gCursor->arrow();
}

void DApplication::MainEventLoop(void)
{
	while (!fDone) this->ProcessTasks();
}

void DApplication::ProcessTasks(void)
{
	Nlm_ProcessEventOrIdle();
	Nlm_RemoveDyingWindows();
	gTaskCentral->NextTask();
}

Boolean	DApplication::EventsAvail() 
{ 
	return Nlm_EventAvail();
}

void	DApplication::FlushEvents() 
{ 
	Nlm_FlushEvents();
}


void DApplication::Quit()
{
	fDone= true;
	if (gWindowManager) delete gWindowManager; gWindowManager= NULL;
	// ?? ^^ is this okay, we got one bomb on a DWindow call after Quit & WinMgr == NULL

	if (!Nlm_EmptyRect(&fgAppWinRect)) {
		char  srect[128];
		sprintf( srect, "%d %d %d %d", fgAppWinRect.left, fgAppWinRect.top, 
														fgAppWinRect.right, fgAppWinRect.bottom);
		gApplication->SetPref( srect, "fgAppWinRect", "windows");
		}
 
  Nlm_QuitProgram (); // this justs sets a local vibrant global, in ProcessEvents(), which we don't see
}

void	DApplication::SetAppMenuWindow(DWindow* itsWindow)
{
	fAppWindow= itsWindow;
}


void	DApplication::OpenDocument(char* pathname)
{
	DFile *afile= new DFile( pathname, "r");
	OpenDocument( afile);
	delete afile; // ???
}

void	DApplication::OpenDocument(DFile* aFile)
{
	// default OpenDoc is to use Vibrant's text display
	if (aFile && aFile->Exists()) {
		Nlm_ParData  paratabs = {false, false, false, false, true, 0, 0}; // tabs==tabs
		Nlm_ColData* coldata= NULL;

		gCursor->watch();
		aFile->CloseFile(); // ShowFile manages this..
		char* shortname= (char*)aFile->GetShortname();
		DWindow* win= new DWindow(0, this, DWindow::document, -1, -1, -10, -10, shortname);
		DTextDocPanel* td= new DTextDocPanel(0, win, 500, 300);

		td->SetSlateBorder( false);
		td->SetResize( DView::matchsuper, DView::relsuper);
		//td->SetTabs( gTextTabStops);
		
		const char* fullname= aFile->GetName();
#if 0
		td->ShowFile( (char*)fullname, gTextFont);
#else
		td->ShowFileFancy( (char*)fullname, &paratabs, coldata, gTextFont, gTextTabStops);
#endif
		
		win->Open();
		td->SizeToSuperview( win, true, false); // this needs to adjust for if view has scrollbar !
		gCursor->arrow();
		}
}


Boolean	DApplication::ChooseFile(DFile*& openedFile, char* filesuffix, char* acceptableTypeIDs)
{
	DFile* aFile;
	//if (openedFile) delete openedFile; //??
	openedFile= NULL;
	char* name= (char*) gFileManager->GetInputFileName( filesuffix, acceptableTypeIDs); 
	if (name) {
		aFile= new DFile( name, "r");
		if (aFile->Exists()) {
			openedFile= aFile;
			return true;
			}
		else {
			delete aFile;
			return false;
			}
		}
	else
		return false;
}



Boolean DApplication::DoMenuTask(long tasknum, DTask* theTask)
{
	DWindow* win;
	
	switch (tasknum) {

		case DAppleMenu::kAboutMenuItem: 
			// ^^this is likely handled by DAppleMenu object
		case kAbout:
			this->DoAboutBox();
			return true;

		case kHelp:
			this->DoHelp();
			return true;
			
		case kNew:
			Message(MSG_OK,"DApplication::New not ready.");
			//this->CreateDocument();
			return true;
		
		case kPrintAFile:	
		case kOpenAFile:
			if (theTask)  
				this->OpenDocument( (char*)theTask->fExtra);
			return true;
			
		case kOpen:
			{
			DFile* aFile = NULL;
			if (this->ChooseFile( aFile, fFileSuffix, fAcceptableFileTypes)) 
				this->OpenDocument( aFile);
			}
			return true;
			
		case kClose: 
			win= gWindowManager->CurrentWindow();
			if (win) win->CloseAndFree(); // test win for doc needing saving ??
			return true;
							
		case kSave:
		case kSaveAs:
		case kSaveACopy:
			win= gWindowManager->CurrentWindow();
			if (win && win->fSaveHandler) {
				const char* name;
				char  namestore[256], *defname = NULL;
				Boolean changewinname= false;
				
				if (tasknum == kSave)
					defname= win->GetFilename(namestore,sizeof(namestore));
				if (!defname) 
					defname= win->GetTitle(namestore,sizeof(namestore));
				if (!defname || !*defname || StrCmp(defname,DFileManager::kUntitled)==0) 
					tasknum= kSaveAs;  
				if (tasknum == kSave)
					name= defname;
				else {
					name= gFileManager->GetOutputFileName(defname);
					changewinname= (tasknum == kSaveAs);
					}
				if (name && *name) {
					DFile* myFile = new DFile(name, "w");
					if (myFile) {
						myFile->OpenFile();
						win->SaveDoc(myFile); 
						if (changewinname) {
						  win->SetFilename((char*) name);
						  win->SetTitle((char*)DFileManager::FilenameFromPath(name));
						  }
						myFile->CloseFile();
						delete myFile;
						}
					}
				}
			else 
				Message(MSG_OK,"DApplication::Save not ready.");
			return true;

		case kFindAgain:
			win= gWindowManager->CurrentWindow();
			if (win && win->fFindDlog) 
				win->fFindDlog->FindAgain(); 
			else
				Message(MSG_OK,"DApplication::FindAgain not available.");
			return true;
			
		case kFind:
			win= gWindowManager->CurrentWindow();
			if (win && win->fFindDlog) {
				if ( win->fFindDlog->PoseModally() ) ;
				}
			else
				Message(MSG_OK,"DApplication::Find not available.");
			return true;
			
		case kPrint:
			win= gWindowManager->CurrentWindow();
			if (win && win->fPrintHandler) 
				win->PrintDoc();
			else
				Message(MSG_OK,"DApplication::Print not ready.");
			return true;

		case kQuit:
			this->Quit(); // this should be a task added to end of task queue...??
			return true;
			
		case kPrevWindow:
			gWindowManager->BringToFront();
			return true;
		case kNextWindow:
			gWindowManager->SendToBack();
			return true;
			
		case kChooseWindow:
#ifndef BOBS_WIN_MAC						
  		{
			DWindowChoiceDialog* dlg= new DWindowChoiceDialog();
			if ( dlg->PoseModally() ) {
				DWindow* win = dlg->Result();
				if (win && win != dlg) win->Select();
				}
			delete dlg;
			}
#else
#endif
			return true;

		case kUndo:
			if (gLastCommand) gLastCommand->UndoRedo();
			return true;

		case kDeleteDeadWindow:
			{
			DView* windowCarcass= (DView*) theTask->fExtra;
			if (windowCarcass) delete windowCarcass;
			}
			return true;

		default: 
			return DTaskMaster::DoMenuTask(tasknum, theTask);
		}
}



Boolean DApplication::IsMyAction(DTaskMaster* action)
{
	switch(action->Id()) {

		case DAppleMenu::kAboutMenuItem:
		case kAbout:
		case kHelp:
		case kNew:
		case kOpen:
		case kClose:
		case kSaveAs:
		case kSaveACopy:
		case kSave:
		case kPrint:
		case kQuit:
		case kPrevWindow:
		case kNextWindow:
		case kChooseWindow:
		case kFind:
		case kFindAgain:
		case kUndo:
			return DoMenuTask(action->Id(), NULL);

		case kCut:
		case kCopy:
		case kPaste:
		case kClear:
		case kSelectAll:
			{
					// looks like Vibrant doesn't currently support a system clipboard for
					// copy/paste of text (& other objects) between applications...
			static Boolean inrecurse = false;
			DWindow* win = gWindowManager->CurrentWindow();
			DDialogText* dtext= gWindowManager->CurrentDialogText();
			if (inrecurse) return true;
			inrecurse= true;
			if (dtext && win && win->HasEditText()) {
				if (win != dtext->GetWindow()) dtext = win->fEditText;
					// ^^ if this is true, we can't tell which of multiple texts may be
					// the intended target ... CurrentDialogText() is supposed to track user text.
				if (dtext->IsMyAction(action))  goto doneEditCmd;
				}
			if (win && win->IsMyAction(action)) goto doneEditCmd;
			Message(MSG_OK,"Application::Edits not ready.");
	doneEditCmd:
			inrecurse= false;
			return true;
			}
			
		case kShowClipboard:
			gClipboardMgr->DoMenuTask( action->Id(), NULL);
			return true;
			
		case cOKAY:
		case cCANC:
			{
			DButton* but= (DButton*) action;
			DWindow* win;
			if ( but && ((win= but->GetWindow()) != NULL) ) {
				if (action->Id() == cOKAY) {
					// do Okay close handling...
					}
				win->CloseAndFree();
				}
			}
			return true;

		default:
			return DTaskMaster::IsMyAction(action);
		}
}

DMenu* DApplication::NewMenu(long id, char* title)
{
	Nlm_WindoW nwind = (fAppWindow) ? (Nlm_WindoW)fAppWindow->GetNlmObject() : NULL;
	return new DPulldownMenu( id, this, nwind, title);
}


extern "C" void Nlm_SetupMenuBar();

void	DApplication::SetUpMenus(void)
{
	DMenu* aMenu = NULL;
	this->SetUpMenu(cFileMenu, aMenu);
	aMenu= NULL;
	this->SetUpMenu(cEditMenu, aMenu);
	aMenu= NULL;
	this->SetUpMenu(cWindowMenu, aMenu);
	this->SetUpMenu( 0, aMenu);
}

void DApplication::SetUpMenu(short menuId, DMenu*& aMenu) 
{
	// ?? either change menu objects to local/nonpointer, 
	// or suicide() them to remove all but DViewCentral owner
	
	if (menuId == cFileMenu) {
#ifdef WIN_MAC
		DAppleMenu* appleMenu = new DAppleMenu(this,fAboutLine);
#endif
		if (!aMenu) aMenu = this->NewMenu( cFileMenu, "File");
#ifndef WIN_MAC
		aMenu->AddItem(kAbout, fAboutLine);
		aMenu->AddSeparator();
#endif
		aMenu->AddItem(kNew,"New/N",false, true);
		aMenu->AddItem(kOpen,"Open/O",false, true);
		aMenu->AddSeparator();
		aMenu->AddItem(kClose,"Close/W",false, true);
		aMenu->AddItem(kSave,"Save/S",false, true);
		aMenu->AddItem(kSaveAs,"Save As...",false, true);
		aMenu->AddSeparator();
		aMenu->AddItem(kPrint,"Print",false, true);
		aMenu->AddItem(kHelp,"Help/H",false, true);
		aMenu->AddSeparator();
		aMenu->AddItem(kQuit,"Quit/Q",false, true);
		}
	
	else if (menuId == cEditMenu) {
		if (!aMenu) aMenu = this->NewMenu(cEditMenu, "Edit");
		aMenu->AddItem(kUndo,"Undo/Z",false, true);
		gViewCentral->DisableView(kUndo);  // don't know undo yet... :-(ouch)
		aMenu->AddSeparator();
		aMenu->AddItem(kCut,"Cut/X",false, true);
		aMenu->AddItem(kCopy,"Copy/C",false, true);
		aMenu->AddItem(kPaste,"Paste/V",false, true);
		aMenu->AddItem(kClear,"Clear",false, true);
		aMenu->AddItem(kSelectAll,"Select All/A",false, true);	
		aMenu->AddItem(kShowClipboard,"Show clipboard",false, true);
		gViewCentral->DisableView(DApplication::kShowClipboard);
		}
		
	else if (menuId == cWindowMenu) {
		if (!aMenu) aMenu= this->NewMenu(cWindowMenu, "Windows");
		aMenu->AddItem(kPrevWindow,"Prev/-",false, true); // == bring last win to front
		aMenu->AddItem(kNextWindow,"Next/+",false, true); // == send top win to last
		aMenu->AddItem(kChooseWindow,"Select...");  
		//aMenu->AddSeparator();
		//this->AddWindowListToMenu(menuId, aMenu); //gWindowManager->GetWindowList();
		}
		
	else if (menuId == 0) {
		Nlm_SetupMenuBar();
		}
 
}



#if NOT_READY
	
void DApplication::SetWindowMenu(Boolean activate)
{	 	 		
#ifndef BOBS_WIN_MAC						
						// THIS IS NO GOOD ON XWIN, and on MSWIN
						// Vibrant currently has no way of removing single menu items
						// except for Mac.  Only option for others seems to be removal
						// of entire (sub) menu, with no way to replace w/ a new menu
						// in *same* position !!!
#else						
   if (fWindowChoiceMenu) {
		 short item= kViewDefault;
		 fWindowChoiceMenu->Reset();  
		 fWindowChoiceMenu->AddItem( item, "default");
		 DList* wins= gWindowManager->GetWindowList();
		 if (activate && wins) {
			long i, n= wins->GetSize();
			for (i= 0; i<n; i++) {
				char *cp, title[256];
				item++;
				DWindow* win= (DWindow*) fList->At(i);
				win->GetTitle(title,256);
				cp= title; 
				//while (cp= strchr(cp, '/')) *cp= '_';
				fViewChoiceMenu->AddItem( item, smenu);
				}
		 	}
	 }
#endif
}

#endif



	 
void DApplication::DoHelp()
{
	char filename[512];
	DFile* afile = new DFile();
	char* suf;
	
	char* apppath = (char*) gFileManager->GetProgramPath();  
	apppath= (char*) gFileManager->PathOnlyFromPath( apppath);
	apppath= (char*)gFileManager->BuildPath( apppath, kHelpfolder, (char*)Shortname());
	afile->Initialize(apppath,"r");
	if (!afile->Exists()) {
		StrNCpy(filename, (char*)Shortname(),490);
		StrCat(filename,".help");
		afile->Initialize(filename,"r");
		if (!afile->Exists()) {
			StrNCpy(filename, (char*)Pathname(),490);
			suf= (char*)gFileManager->FileSuffix(filename); if (suf) *suf= 0;
			StrCat(filename,".help");
			afile->Initialize(filename,"r");
			}
		}
	OpenHelp( afile);
	delete afile;
}



void DApplication::OpenHelp(const char* helpname)
{
	char* apppath = (char*) gFileManager->GetProgramPath();  
	apppath= (char*) gFileManager->PathOnlyFromPath( apppath);
	const char*	pathname= gFileManager->BuildPath( apppath, kHelpfolder, helpname);
	DFile* afile = new DFile(pathname,"r");
	OpenHelp(afile);
	delete afile;
}


void DApplication::OpenHelp(DFile* aFile)
{
#if 1
	if (!aFile || !aFile->Exists()) {
		const char * name;
		if (aFile) name= aFile->GetName(); else name= "noname";
		Message(MSG_OK,"Can't find help file '%s'",name);
		return;
		}
	this->OpenDocument( aFile); 
#else
	Nlm_ParData  paratabs = {false, false, false, false, true, 0, 0}; // tabs==tabs
	Nlm_ColData* coldata= NULL;
		
	if (!aFile->Exists()) {
		Message(MSG_OK,"Can't find help file '%s'",aFile->GetName());
		return;
		}
		
	gCursor->watch();
	aFile->CloseFile(); // make sure closed so ShowFile can open !?
	DWindow* win= new DWindow(cHelp, this, DWindow::document, -1, -1, -10, -10, 
										(char*) aFile->GetShortname());
	DTextDocPanel* td= new DTextDocPanel(0, win, 490, 300);
	td->SetSlateBorder( false);
	td->SetResize( DView::matchsuper, DView::relsuper);
#if 0
	td->ShowFile( (char*)aFile->GetName(), gTextFont);
#else
	td->ShowFileFancy( (char*)aFile->GetName(), &paratabs, coldata, gTextFont, gTextTabStops);
#endif
	win->Open();
	td->SizeToSuperview( win, true, false);  
	gCursor->arrow();
#endif
}


void DApplication::DoAboutBox(void)
{
	DAboutBoxWindow* about = new DAboutBoxWindow; // make instance & it handles rest
}

	


void DApplication::UpdateMenus( void)
{
		// Is the frontmost window modal?
	
	gViewCentral->EnableView(DApplication::kQuit);

	//if (gInBackground) 
	{
		gViewCentral->EnableView(DApplication::kClose);
		//gViewCentral->EnableView(DApplication::kUndo);
		gViewCentral->EnableView(DApplication::kCut);
		gViewCentral->EnableView(DApplication::kCopy);
		gViewCentral->EnableView(DApplication::kPaste);
		gViewCentral->EnableView(DApplication::kClear);
	} 

	//if (!isModalWindow) 
	{
		gViewCentral->EnableView(DApplication::kNew);
		gViewCentral->EnableView(DApplication::kOpen);
	}

	gViewCentral->EnableView(DApplication::kPrevWindow);
	gViewCentral->EnableView(DApplication::kNextWindow);
	
	//Str63	undoStr;
	//GetIndString(undoStr, STRcommon, strUNDO);
	//gViewCentral->SetMenuText( DApplication::kUndo, undoStr);
}




