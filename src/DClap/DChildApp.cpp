// DChildApp.cpp


#include "DClap.h"
#include "DChildApp.h"


// for DChildDlogDoc
#include "Dvibrant.h"
#include "DWindow.h"
#include "DNetObject.h"
#include "DURL.h"
#include "DRichMoreStyle.h"
#include "DBOPclient.h"


#undef DEBUG



#ifdef WIN_MAC
	// undef some conflicts b/n Types.h && NlmTypes
#undef Handle
//#undef true
//#undef false
//#undef Boolean
#include <AppleEvents.h>
#include <Processes.h>
//#define Boolean  Nlm_Boolean
#endif

#ifdef WIN_MSWIN
#include <windows.h>
#include <windowsx.h>
#endif

//class DChildFile : public DFile


DChildFile::DChildFile( const char* filename, short kind,
	Boolean deleteWhenDone,
	short doneAction, const char* openmode, 
	const char* ftype, const char* fcreator) :
	DFile( filename, openmode, ftype, fcreator),
	fAction(doneAction), fKind(kind), fDelete( deleteWhenDone)
{
	//fName2= StrDup(filename); // fix probs w/ fName getting corrupted !!!!???
	if (!openmode) switch (fKind) {
		case kInput: 
		case kStdin:  fMode= StrDup("r"); break;	//?? we really open "w" not "r" ??
		default:
		case kOutput: fMode= StrDup("r"); break;  //?? we really open "r" not "w" !? 
		case kStdout: fMode= StrDup("r"); break;  // was "a", ??? "w" or "a"
		case kStderr: fMode= StrDup("r"); break;  // was "a", ??? "w" or "a"
		}
}

DChildFile::~DChildFile()
{
	MemFree(fName2);
#ifndef DEBUG
	if (fDelete) this->Delete();
#endif
}

void DChildFile::ClearStorage()
{
	this->Close();
#ifndef DEBUG
	if (fDelete) this->Delete();
#endif
}





//class DChildApp : public DTaskMaster
	
DChildApp::DChildApp() : 	
	DTaskMaster(0),
	fName(NULL), fCmdline(NULL), 
	fStdin(NULL), fStdout(NULL), fStderr(NULL),
	fLaunched(0), fResult(0), fReusable(false), fCallMethod(kNone),
	fProcessNum(0), fFiles(NULL), fNob(NULL), fBopper(NULL)
{
  if (!DChildAppManager::gChildList) DChildAppManager::gChildList= new DList();
	DChildAppManager::gChildList->InsertLast( this); // do this here so caller doesn't forget
}


DChildApp::DChildApp( char* appname, char* cmdline, 
	Boolean showStdout, char* Stdinfile, Boolean showStderr)  :  
	DTaskMaster(0),
	fName(NULL), fCmdline(NULL), 
	fStdin(NULL), fStdout(NULL), fStderr(NULL),
	fLaunched(0), fResult(0), fReusable(false), fCallMethod(kNone),
	fProcessNum(0), fFiles(NULL), fNob(NULL), fBopper(NULL)
{
	fName= StrDup( appname);
	fCmdline= StrDup( cmdline);
	
	//fFiles= new DList();

	// this is called for launching docs as well as apps,
	// don't do the stdout/stderr stuff if this is doc
	if (showStdout) AddFile( DChildFile::kStdout, NULL);
	if (showStderr) AddFile( DChildFile::kStderr, NULL);
	if (Stdinfile) AddFile( DChildFile::kStdin, Stdinfile);
	
  if (!DChildAppManager::gChildList) DChildAppManager::gChildList= new DList();
	DChildAppManager::gChildList->InsertLast( this); // do this here so caller doesn't forget
}


DChildApp::~DChildApp()
{
	ClearFiles();
	if (fFiles) delete fFiles; fFiles= NULL;
	MemFree( fName);
	MemFree( fCmdline);
	if (fNob) delete fNob;
	if (fBopper) delete fBopper;
	DChildAppManager::gChildList->Delete( this); //need here to balance constructor Insert()
}

void DChildApp::ClearFiles()
{
	if (fFiles) {
		short i, n= fFiles->GetSize();
#if 0
			// ?? is this causing trashed DChildFiles ??
		for (i=n-1; i>=0; i--) {
			DChildFile* afile= (DChildFile*) fFiles->At(i);
			delete afile;
			}
#endif
#if 1
		fFiles->DeleteItemsAt(0,n);
#else
		delete fFiles; fFiles= NULL;
#endif
		}
}


void DChildApp::AddInputBuffer( short filekind, char* buffer, ulong buflen)
{
	char	namestore[512], *name;
	DChildFile* aFile;
	name= gFileManager->TempFilename(namestore);
	switch (filekind) {
		case DChildFile::kInput: 
		case DChildFile::kStdin:  
			aFile= new DChildFile( name, filekind, true, DChildFile::kNoAction);
			break;
		default:
			return;
		}
	if (aFile) {
		aFile->Open("w");
		aFile->WriteData( buffer, buflen);
		aFile->Close();
		aFile->SetMode("r");
		this->AddFile( aFile);
		}
}

void DChildApp::AddFile( DChildFile* aFile)
{
		// ?? need to check if fFiles already has one of stdio types ??
	if (!fFiles) 	fFiles= new DList();
	fFiles->InsertLast( aFile);
	switch (aFile->fKind) {
		case DChildFile::kStdin:  
			fStdin= aFile->GetName();
			break;
		case DChildFile::kStdout:  
			fStdout= aFile->GetName();
			break;
		case DChildFile::kStderr:  
			fStderr= aFile->GetName();
			break;
		}
}

void DChildApp::AddFile( short filekind, char* name)
{
	char	namestore[512];
	DChildFile* aFile;
	if (!name) {
		if (filekind == DChildFile::kStdin || filekind == DChildFile::kInput) return;
		name= gFileManager->TempFilename(namestore);
	  }
	  
	switch (filekind) {
		case DChildFile::kInput: 
		case DChildFile::kStdin:  
			aFile= new DChildFile( name, filekind, false, DChildFile::kNoAction);
			break;
		
		default:
		case DChildFile::kOutput:   
		case DChildFile::kStdout:  
			aFile= new DChildFile( name, filekind, false);
			break;
		case DChildFile::kStderr:  
			aFile= new DChildFile( name, filekind, true);
			break;
		}
	this->AddFile( aFile);
} 



#ifdef WIN_MAC
extern "C" ProcessSerialNumber  gChildProcessSN;
#else
extern "C" long gChildProcessSN;
#endif

enum { kChildTask = 3245, kStatusTask = 34382 };

class DBurialTask : public DTask {
public:
	DBurialTask( long processnum, DTaskMaster* itsSource) :
		DTask( processnum, kChildTask, itsSource) {}
	virtual void DoIt() { DChildAppManager::BuryDeadChildApp(fNumber); }
};

class DStatusTask : public DTask {
public:
	DStatusTask() :
		DTask( DChildAppManager::kStatusTask, DChildAppManager::kChildManagerTask, 
					gChildAppManager) 
		{
		if (!gTaskCentral->FindTask(this)) fRepeat= true;
		}
	virtual void DoIt() { 
		//if (!DChildAppManager::CheckStatus()) fRepeat= false;
		}
};


Boolean DChildApp::Launch()
{
	fResult= 0;
	gCursor->watch();
	switch (fCallMethod) {
		case kLocalexec:
			(void) LaunchLocal();
			break;
		case kBOPexec:
			(void) LaunchBop();
			break;
		case kHTTPget:
		case kHTTPpost:
		default:
			break;
		}	
	gCursor->arrow();
	return  fResult;
}


Boolean DChildApp::LaunchBop()
{
	char * aHost, * aUser, * aPass, * aPath;
	long	aPort;
	fResult= 0;
	
	if (!fNob) fNob = new DNetOb();
	if (!DURL::ParseURL( fNob, fName)) ; // ?? error
	aHost= (char *)fNob->GetHost(); if (!aHost||!*aHost) aHost= DBOP::gHost;
	aPort= fNob->fPort;  if (!aPort) aPort= DBOP::gPort;
	aPath= (char *)fNob->GetPath();

	//if (!aHost || (!aPath && !fCmdline)) return fResult;
	if ((!aPath && !fCmdline)) return fResult;

	aUser= (char *)fNob->GetUser(); if (!aUser||!*aUser) aUser= DBOP::gUsername;
	aPass= (char *)fNob->GetPass(); if (!aPass||!*aPass) aPass= DBOP::gPassword;
	
	if (!fBopper) fBopper= new DBOP( aUser, aPass, aHost, aPort);
	else fBopper->ResetHost( aUser, aPass, aHost, aPort);

	if (*aPath == '/') aPath++; // default / is leftover from URL syntax
	long len= 2 + StrLen(aPath) + StrLen(fCmdline);
	char* cmdline = (char*) MemNew( len);
	*cmdline= 0;
	if (aPath) { 
		StrCat( cmdline, aPath);
		StrCat( cmdline, " ");
		}
	if (fCmdline) StrCat( cmdline, fCmdline);
	fLaunched++;
	fResult= fBopper->Execute( cmdline, fFiles);
	fProcessNum= fBopper->ProcessNum();
	MemFree( cmdline);
	
	if (fProcessNum) {
		// ?? save user/pass from fBopper (which can ask for new)??
		if (!StrCmp(fBopper->GetUser(),fNob->GetUser()))
			fNob->StoreUser( (char*)fBopper->GetUser());
		if (!StrCmp(fBopper->GetPass(),fNob->GetPass()))
			fNob->StorePass( (char*)fBopper->GetPass());
		if (!StrCmp(fBopper->GetHost(),fNob->GetHost()))
			fNob->StoreHost( (char*)fBopper->GetHost());
		if ( fBopper->gPort != fNob->fPort)
			fNob->fPort= fBopper->gPort;
		}
	
	DStatusTask* statme = new DStatusTask();
	PostTask( statme);
		 
	return fResult;
}
	

Boolean DChildApp::LaunchLocal()
{
		// ?? want special handling for stdin/stdout/stderr files !!
		// !! must assume OtherFiles are already handled, either in cmdline or by child app !!
	fResult= 0;
	
	if (fName || fCmdline) {
		fLaunched++;
		fResult= Dgg_LaunchApp( fName, fCmdline, (char*)fStdin, 
									(char*)fStdout, (char*)fStderr);

		if (fResult) {
#ifdef WIN_MAC
			fProcessNum= (long) MemNew(sizeof(ProcessSerialNumber));
			*((ProcessSerialNumberPtr) fProcessNum)= gChildProcessSN;
#else
			fProcessNum= gChildProcessSN;
#endif

#ifdef WIN_MSWIN
#ifdef WIN16
	    	// assume no multiprocessing?? 
			DBurialTask* buryMe = new DBurialTask( gChildProcessSN, this);
			PostTask( buryMe);
#else
			DStatusTask* statme = new DStatusTask();
			PostTask( statme);
#endif
#endif
			//gChildList->InsertLast( this); // done in constructor;
			// can we assume only one copy of this in list ??
			} 
		}
	return fResult;
}



void DChildApp::FileAction(DChildFile* aFile)
{
	switch (aFile->fAction) {
	  case DChildFile::kOpenPict:
		case DChildFile::kOpenText: 
			gApplication->OpenDocument( aFile);
			if (aFile->fDelete) {
				DWindow* win= gWindowManager->CurrentWindow();
				if (win && win->fSaveHandler) win->fSaveHandler->Dirty();
				}
			break;
			
		default:
			break;
		}
}

void DChildApp::Finished()
{
	fLaunched--; // ?? or use this as counter?
	gCursor->watch();
	if (fCallMethod == kBOPexec && fBopper) {
		fBopper->GetOutput( fProcessNum, fFiles);
		fBopper->Delete( fProcessNum);
		fBopper->Quit(); // ??
		}
	if (fFiles) {
		DChildFile* aFile;
		short i, n= fFiles->GetSize();
		for (i=n-1; i>=0; i--) {
			aFile= (DChildFile*) fFiles->At(i);
			if (aFile->Exists() && aFile->LengthF()>0) 
				FileAction( aFile);
			aFile->ClearStorage();
			}
		ClearFiles();
		}
	gCursor->arrow();
}



// class DExternalHandler : public DChildApp

DExternalHandler::DExternalHandler( char* cmdlineOrDocname) :
	DChildApp( cmdlineOrDocname, NULL, false, NULL, false)
{
}

Boolean DExternalHandler::Launch()
{
	return DChildApp::Launch();
}

void DExternalHandler::Finished()
{
	DChildApp::Finished();
}
			




// class DChildAppManager : public DObject


//static
DList* DChildAppManager::gChildList = NULL; // new DList(); << bad on SGI // of DChildApp
long DChildAppManager::gLastTime = 0;

DChildAppManager* gChildAppManager = new DChildAppManager();

enum { 
#ifdef WIN_MAC
	kStatDelay = 1200 // 60ths of second to minutes
#endif
#ifdef WIN_MSWIN
	kStatDelay = 10000 // milliseconds to minutes 
#endif
#ifdef WIN_MOTIF
	kStatDelay = 1200 //  60ths of second to minutes 
#endif
	};

DChildAppManager::DChildAppManager() :
	DTaskMaster( kChildManagerTask)
{
}


Boolean DChildAppManager::IsMyTask(DTask* theTask)
{
  if (theTask && theTask->fNumber == kStatusTask) { 
    short stat= CheckStatus();
    theTask->fRepeat= (stat != 0);
   }
  return true;
}


//static
short DChildAppManager::BuryDeadChildApp( long theEvent)
{
#ifdef WIN_MAC
	short 		err;
	long 			appErr;
	Size			actualSize;
	ProcessSerialNumber appSN;
	DescType 	returnedType;
 	AppleEvent* theAppleEvent;
 	
 	if (theEvent) {
 	theAppleEvent = (AppleEvent*) theEvent;
 
	err= ::AEGetParamPtr( theAppleEvent, keyErrorNumber, typeLongInteger, 
												&returnedType, &appErr, sizeof(appErr), &actualSize);
	if (appErr == 0) { 
		err = AEGetParamPtr( theAppleEvent, keyProcessSerialNumber, typeProcessSerialNumber, 
													&returnedType, &appSN, sizeof(appSN), &actualSize);
		Boolean match;
		DChildApp* child= NULL;
		short i, n= gChildList->GetSize();
		for (i=0, match= false; !match && i<n; i++) {
			child= (DChildApp*) gChildList->At(i);
			err= ::SameProcess( (ProcessSerialNumberPtr)child->fProcessNum, &appSN, &match);
			}
			
		if (match) {
			child->Finished();
			//gChildList->Delete(child); // done by child destructor
			if (!child->fReusable) delete child; 
			}
	 }
	 }
#endif

#if defined(WIN_MOTIF) || defined(WIN_MSWIN)
	// use unix(?) signals
	Boolean match;
	DChildApp* child= NULL;
	short i, n= gChildList->GetSize();
	for (i=0, match= false; !match && i<n; i++) {
		child= (DChildApp*) gChildList->At(i);
		match= (child->fProcessNum == theEvent);
		}
	if (match) {
		child->Finished();
		//gChildList->Delete(child); // done by child destructor
		if (!child->fReusable) delete child; 
		}
#endif
	return 0;
}


//static
Boolean DChildAppManager::CheckStatus()
{	
	short i, n = gChildList->GetSize();
	long now= gUtil->time();
	if (now - gLastTime > kStatDelay) {
		gLastTime= now;
		for (i=n-1; i>=0; i--) {
			DChildApp* child= (DChildApp*) gChildList->At(i);
			if (child && child->fProcessNum) {
			
				if (child->fCallMethod == DChildApp::kLocalexec) {
#ifdef WIN_MSWIN
#ifndef WIN16
		      long waitresult= ! WAIT_TIMEOUT;
		      HANDLE hobj= OpenProcess( SYNCHRONIZE, false, child->fProcessNum);
		      if (hobj) waitresult= WaitForSingleObject( hobj, 0);
		      if (waitresult != WAIT_TIMEOUT) {
		    	  child->Finished();
				    if (!child->fReusable) delete child; 
		    	  n--;
	    		}
#endif
#endif
					}
				else if (child->fCallMethod == DChildApp::kBOPexec && child->fBopper) {
					if (child->fBopper->CheckStatus(child->fProcessNum)
								>= DBOP::kProcDone) {
		    	  child->Finished();
				    if (!child->fReusable) delete child; 
		    	  n--;
						}
					}
	      }
	    	else n--; //??
			}
		}
	return (n > 0);
}






#include "DChildDlogDoc.h"
#include "DRichViewNu.h"
#include "DRichMoreStyle.h"
#include "DHTMLHandler.h"

class	DChildDlogView : public DRichView {
public:
	Boolean fhasVscroll;
	short fPixwidth, fPixheight;
	
	DChildDlogView(long id, DView* itsSuperior, short pixwidth, short pixheight, 
									Boolean hasVscroll = true);
	virtual void ReadyToOpen();
	virtual void ShowFile( char* filename, Nlm_FonT font);
	virtual void ShowDoc( char* doctext, ulong doclength = 0);
	virtual void Click( Nlm_PoinT mouse);
	virtual DList* GetControlObjects();
};



DChildDlogView::DChildDlogView(long id, DView* itsSuperior, short pixwidth, short pixheight, 
													Boolean hasVscroll) :
		DRichView( id, itsSuperior, pixwidth, pixheight, hasVscroll )
{
	fhasVscroll= hasVscroll;
	fPixwidth= pixwidth;
	fPixheight= pixheight;
}

void DChildDlogView::Click( Nlm_PoinT mouse)
{
	// eat dlog richtext clicks other than to controls !
}


void  DChildDlogView::ShowFile( char* filename, Nlm_FonT font) 
{
	fDocFormat= DRichHandler::kUnknownformat;
  DHTMLHandler htmlhandler( this, NULL);
  if ( htmlhandler.ProcessFile( filename) ) 
  	fDocFormat= htmlhandler.Format(); 
  else {
	  DRichHandler texthandler( this, NULL);
	  if ( texthandler.ProcessFile( filename) ) 
	  	fDocFormat= texthandler.Format(); 
  	}
}


void  DChildDlogView::ShowDoc( char* doctext, ulong doclength) 
{
	if (!doctext) return;
	if (!doclength) doclength= StrLen( doctext);
	char*	docend= doctext + doclength;
	
	fDocFormat= DRichHandler::kUnknownformat;
  DHTMLHandler htmlhandler( this, NULL);
 	if (htmlhandler.ProcessData( doctext, docend, true, doclength))
  	fDocFormat= htmlhandler.Format(); 
  else {
	  DRichHandler texthandler( this, NULL);
 		if ( texthandler.ProcessData( doctext, docend, true, doclength))
	  	fDocFormat= texthandler.Format(); 
  	}
}

void  DChildDlogView::ReadyToOpen()
{
	SetSlateBorder( false);
	SetResize( DView::matchsuper, DView::relsuper);  
	SizeToSuperview( this, true, false);  
	Disable(); //! make so only control items are clickable !! (need for mwin)
}


DList* DChildDlogView::GetControlObjects()
{ 
	DList* controls= new DList();
	short i, n= fStyles->GetSize();
	for (i=0; i<n; i++) {
		DRichStyle* st= (DRichStyle*) fStyles->At(i);
		if (st->fObject 
			&& st->fObject->fKind >= DControlStyle::kControlKindStart
			&& st->fObject->fKind <= DControlStyle::kControlKindEnd) {
				controls->InsertLast(st->fObject);
			}
		}
	return controls;
}




//class	DChildDlogDoc : public DWindow

extern "C" void Nlm_SetRect (Nlm_GraphiC a, Nlm_RectPtr r);
extern "C" void Nlm_GetRect (Nlm_GraphiC a, Nlm_RectPtr r);


DChildDlogDoc::DChildDlogDoc( long id, Boolean freeOnClose, Nlm_FonT itsFont) :
		DWindow( id, gApplication, document, -5, -5, -50, -20, NULL, freeOnClose), 
		fDlog(NULL), fControls(NULL)
{ 	
	Nlm_PoinT nps;
	short width, height;
	Nlm_RecT  r;
	
	fFont= itsFont;
	this->GetNextPosition( &nps);
	nps.x= 0; // fix for odd offset
	this->SetNextPosition( nps);
	gUtil->screensize(width, height);
	width = MAX(250, MIN(width-60,gPrintManager->PageWidth()-100));
	height= MIN(350, height-60);
	fDlog= new DChildDlogView( 0, this, width, height, false);
	
	Nlm_GraphiC nwin= (Nlm_GraphiC)this->fNlmObject;
	Nlm_GetRect( nwin, &r);
	r.bottom= height + 5; // + Nlm_vScrollBarWidth; 
	r.right= width + 5; //+ Nlm_hScrollBarHeight;
	Nlm_SetRect( nwin, &r);
}


DChildDlogDoc::~DChildDlogDoc()
{
	delete fControls;
}

void DChildDlogDoc::AddSubordinate(DTaskMaster* subordinate)
{
	DWindow::AddSubordinate(subordinate);
#if 0
	if (subordinate->fId >= DControlStyle::kControlKindStart 
		&& subordinate->fId <= DControlStyle::kControlKindEnd) {
		if (!fControls) fControls= new DList();
		fControls->InsertLast(subordinate);
		}
#endif
}


DList* DChildDlogDoc::GetControls()
{
	if (!fControls) fControls= fDlog->GetControlObjects();
	return fControls;
}


Nlm_Boolean DChildDlogDoc::PoseModally() 
{ 
	if (!IsVisible()) Open(); // already open !?
	fModal = true;
	while (fModal)  
		gApplication->ProcessTasks();  
	return fOkay; // ?? ain't what we want - want fSubordinate's fId
}

                              

void DChildDlogDoc::OpenText( char* doctext, ulong doclength)
{
	if (doctext) {
		fDlog->ShowDoc( doctext, doclength);
		Open();
		}
}

void DChildDlogDoc::Open( DFile* aFile)
{
	if (aFile) {
		SetTitle( (char*)aFile->GetShortname());
		SetFilename( (char*)aFile->GetName());
		fDlog->ShowFile( (char*)aFile->GetName(), fFont);
		}
	Open();
}

void  DChildDlogDoc::Open( char* filename) 
{
	SetTitle( (char*)DFileManager::FilenameFromPath(filename));
	SetFilename( (char*) filename);
	fDlog->ShowFile( filename, fFont);
  Open();
}

void DChildDlogDoc::Close()
{
	DWindow::Close();
}

void DChildDlogDoc::CloseAndFree() 
{ 
	fModal= false;
	if (this != gWindowManager->GetAppWindow()) {
		gWindowManager->DeleteWindow(this, false);  
		//fFreeOnClose= true; //<< !!! NO NO NO NO - crash in XWin at least !
		this->Close(); 
		}
}

void DChildDlogDoc::ResizeWin()
{
	DWindow::ResizeWin();
	//if (fDlog) fDlog->ViewRect( gRichDocRect);
}

Nlm_Boolean DChildDlogDoc::IsMyAction(DTaskMaster* action) 
{
	char buf[512];

	if (action->Id() >= DControlStyle::kControlKindStart
		&& action->Id() <= DControlStyle::kControlKindEnd)
		{
		switch (action->Id()) {
		
			case DControlStyle::kFileChooser:
				{
				DFileCStyle* cs= (DFileCStyle*) ((DView*)action)->fKind;
				const char* name= DFileManager::GetInputFileName( NULL, NULL);
				if (name) cs->fNamebox->SetTitle( (char*)name);
				}
				return true;
				
			//kButton, kSubmitButton,  kResetButton, 	kRadioButton, kCheckBox,
	 		//kPopup, kPrompt, 	kEditText, 	kDialogScrollText,
	  	//kPasswordText, 
			default:
				//dlog checkboxes, radios, edittext, etc all call here when actioned!
				//Message(MSG_OK,"DControlStyle::message not ready.");
				return true;
			}
		}
		
	switch(action->Id()) {
	
		case cOKAY:
			this->OkayAction();
			fOkay= true;
			// fall thru to next case
		case cCANC: 
			fModal= false;
			this->Close(); 
			return true;

		case DApplication::kUndo:
			Message(MSG_OK,"DChildDlogDoc::Undo not ready.");
			return true;
		case DApplication::kCut:
			Message(MSG_OK,"DChildDlogDoc::Cut not ready.");
			//this->CutText(); 
			return true;
			
		case DApplication::kCopy: {
			Message(MSG_OK,"DChildDlogDoc::Copy not ready.");
	    }
			return true;
			
		case DApplication::kPaste:
			Message(MSG_OK,"DChildDlogDoc::Paste not ready.");
			//this->PasteText(); 
			return true;
		case DApplication::kClear:
			Message(MSG_OK,"DChildDlogDoc::Clear not ready.");
			//this->ClearText(); 
			return true;
		case DApplication::kSelectAll:
			Message(MSG_OK,"DChildDlogDoc::Select all not ready.");
			//if (fDlog) fDlog->SelectAll();
			//this->SetSelection(0,0,32000,32000); 
			return true;
		default:
			return DWindow::IsMyAction(action);
		}
}


#ifdef WIN_MOTIF
#include <ncbiwin.h>
#include <ncbiport.h>
extern Display* Nlm_currentXDisplay;
extern Window   Nlm_currentXWindow;
#endif

void DChildDlogDoc::Open()
{
	this->CalcWindowSize();
	fDlog->ReadyToOpen();
	DWindow::Open();
#ifdef WIN_MOTIF
	//fDlog->SendBehind();
	fDlog->Select();
	XLowerWindow( Nlm_currentXDisplay, Nlm_currentXWindow); // make this a DView method?
#endif
}



