// DApplication.h
// d.g.gilbert

#ifndef _DAPPLICATION_
#define _DAPPLICATION_

#include "DTaskMaster.h"

class DWindow;
class DMenu;
class DFile;

class DApplication : public DTaskMaster
{	
public:
	enum filetasks {cFileMenu=40, kAbout,kNew,kOpen,kClose,kSave,kSaveAs,kSaveACopy,
					kPrint,kHelp,kQuit,kOpenAFile, kPrintAFile};
	enum edittasks {cEditMenu=60, kUndo,kCut,kCopy,kPaste,kClear,kSelectAll,
					kShowClipboard, kFind,kFindAgain};
	enum windowtasks {cWindowMenu= 80, kPrevWindow,kNextWindow,kChooseWindow,
					kDeleteDeadWindow};
	
	static char* kName;
	static char* kVersion;
	static char* kHelpfolder;
	static Nlm_RecT	fgAppWinRect;
	static short HandleSpecialEvent( long inEvent, long outReply, long inNumber);
	
	Boolean fDone;								// true when Quit requested...
	Boolean fDidOpenStartDoc;
	char* fPathname;
	char* fShortname;
	char* fAboutLine;
	DWindow* fAppWindow;
	char*	fAcceptableFileTypes;	// mac, ?? other file type ids
	char*	fFileSuffix;					// unix, msdos file suffix
	char*	fAppID;				// == processnum or the like, use as unique id value for process
	
	DApplication();
	~DApplication();	
	void IApplication(const char* theAppName = NULL);
	
	virtual void	Run(void);
	virtual void	MainEventLoop(void);
	virtual void	ProcessTasks(void);
	//virtual void	Idle(EventRecord *macEvent);
	virtual	void	Quit(void);
	virtual const char* AppID() { return fAppID; }
	
	virtual const char*	Pathname(void);
	virtual const char*	Shortname(void);

	virtual Boolean IsMyAction(DTaskMaster* action); // override 
	virtual Boolean	DoMenuTask( long tasknum, DTask* theTask);
	virtual DMenu*  NewMenu(long id, char* title);
	virtual void	UpdateMenus(void);
	virtual void	SetUpMenus(void);
	virtual void  SetUpMenu(short menuId, DMenu*& aMenu); 
	virtual void	DoAboutBox(void);
	virtual void 	DoHelp(void);
	virtual void  OpenHelp(DFile* aFile);
	virtual void  OpenHelp(const char* helpname);
	virtual void	SetAppMenuWindow(DWindow* itsWindow);

	virtual void  InitSpecialEvents();  // == AppleEvents for Mac

	virtual char*	GetPref(char* type, char* section = "general", char* defaultvalue = NULL);
	virtual long	GetPrefVal(char* type, char* section = "general", char* defaultvalue = NULL);
	virtual char*	GetPrefSection(char* section, ulong& sectlen, char* defaultvalue = NULL);
	virtual char* GetFilePref(char* type, char* section = "general", char* defaultvalue = NULL);
	virtual char* ConvertStdFilePath(char* pathname);
	
	virtual Boolean SetPref(char* prefvalue, char* type, char* section = "general");
	virtual Boolean SetPref(long prefvalue, char* type, char* section = "general");
	virtual Boolean InstallDefaultPrefs(const char* defaultSuffix= ".prefs", const char* appName= NULL); // use appName==ShortName()
	
	virtual	Boolean ChooseFile(DFile*& openedFile, char* filesuffix, char* acceptableTypeIDs);
	virtual void OpenDocument(DFile* aFile);
	virtual void OpenDocument(char* pathname);
	virtual Boolean GotStartDocs();
	virtual Boolean OpenStartDocs();
	
		// should these be integrated w/ DTaskMaster/DTaskCentral, as TasksAvail() ????
	Boolean	EventsAvail();
	void	FlushEvents();

};


extern DApplication* gApplication;		// the current application object


#endif
