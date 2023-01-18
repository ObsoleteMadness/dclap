// DWindow.cp
// d.g.gilbert

#include "Dvibrant.h"
#include "DWindow.h"
#include "DControl.h"
#include "DFindDlog.h"
#include "DList.h"
#include "DApplication.h"
#include "DTask.h"
#include "DMethods.h"


//
//		Mswin/Borland C++ has some weird problem with the "Boolean"
//		def of "Nlm_Boolean" when it is in public function parameter lists
//


// need hidden vibrant methods & object for window-system-specific 

#include <vibincld.h>

#ifdef WIN_MOTIF
extern "C" Nlm_ShellTool Nlm_GetWindowShell PROTO((Nlm_WindoW w));
#endif

extern "C" void Nlm_SetAppWindow( Nlm_WindoW w); /* vibwndws.c */


enum ResizeWindSizes {
	kWinDragHeight = 20,
	kWinScrollWidth = 0,   // added to both vert & hor.
	kWinMinWidth = 0,
	kWinExtraHeight
#ifdef WIN_MSWIN
	= 20
#else
	= 0
#endif
	};

		/* from vibrant/vibwndws.c */
extern "C" void Nlm_ResizeWindow(Nlm_GraphiC w, Nlm_Int2 dragHeight,
                              Nlm_Int2 scrollWidth, Nlm_Int2 minWidth,
                              Nlm_Int2 extraHeight);


DWindowManager*	gWindowManager = NULL;


class DWindowCarcass : public DView
{
	// this object holds the parts of DWindow that cannot be deleted by ~DWindow()
	// because Nlm subview objects are likely in the calling chain.
	// So create this object from a dying window, post to task chain for later
	// trashing by gApplication handler.
//
//   23Dec94 -- this looks obsolete -- needs further test, but
//   simple tests say ~DWindow() is working well w/o it
// !!! NO, MOTIF bombs after ~DWindow() due to calls to windowDeactivate after
//			deleteing window object !
public:
	DWindowCarcass( DView* theWindow) : 
		DView( 0, NULL, 0, NULL, theWindow->fSubordinates)
	{
#ifndef WIN_MOTIF
			// motif bombs in this.delete due to something 
    fNlmObject= theWindow->fNlmObject; // bypass the DView(object) setting
#endif
		theWindow->fNlmObject= NULL;
		theWindow->fSubordinates= NULL;
	}
};




// class DWindow

extern "C" void windowCloseProc (Nlm_WindoW item)
{
	DWindow *obj= (DWindow*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Close();
}

extern "C" void windowResizeProc (Nlm_WindoW item)
{
	DWindow *obj= (DWindow*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->ResizeWin();
}


extern "C" void windowActivateProc (Nlm_WindoW item)
{
	DWindow *obj= (DWindow*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Activate();
}

extern "C" void windowDeactivateProc (Nlm_WindoW item)
{
	DWindow *obj= (DWindow*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Deactivate();
}



DWindow::DWindow(long id, DTaskMaster* itsSuperior, WindowStyles style,
					short width, short height, short left, short top, char* title, Nlm_Boolean freeOnClose) :
			DView( id, NULL, kWindow, itsSuperior),
			fWindow(NULL), fEditText(NULL), fFilename(NULL),
			fModal(false), fOkay(false), fFreeOnClose(false),
			fPrintHandler(NULL), fSaveHandler(NULL), fFindDlog(NULL)
{
	InitWindow(style, width, height, left, top, title, freeOnClose);
}


DWindow::DWindow(long id, DTaskMaster* itsSuperior) :
			DView( id, NULL, kWindow, itsSuperior),
			fWindow(NULL), fEditText(NULL), fFilename(NULL),
			fModal(false), fOkay(false), fFreeOnClose(false),
			fPrintHandler(NULL), fSaveHandler(NULL), fFindDlog(NULL)
{
	// caller MUST call: Initialize(style, width, height, left, top, title);
}


DWindow::~DWindow()
{		
#if 0
// this gets us into trouble when child ~ has been called...
// do in ::Close() instead - but QUIT() bypasses Close() !
	if (fSaveHandler) {
		char mytitle[50];
		GetTitle(mytitle, sizeof(mytitle));
		if (fSaveHandler->DirtySaveCancelled(mytitle)) 
			;// can't abort here if they cancel the save 
     }
#endif

#ifdef WIN_MOTIF
  if (gApplication->fDone) {
#endif
		// this is working now !! we can throw out that WindowCarcass jazz !!
	Nlm_SetActivate(fWindow, NULL); 
	Nlm_SetDeactivate(fWindow, NULL); // need for motif

#ifdef WIN_MOTIF
		// also need for motif, to avoid damn call this window's dead draw procs !! 
		// (via nlm_remove->nlm_hidewindow->nlm_update->Xtcallbacks->subview draw methods )
  Nlm_GphPrcsPtr classPtr = Nlm_GetClassPtr((Nlm_GraphiC)fNlmObject);
 	classPtr->hide= NULL;
    }
  else {
 	  DWindowCarcass* deadbody= new DWindowCarcass(this);
 	  gApplication->PostTask( 
	  	gApplication->newTask( DApplication::kDeleteDeadWindow, 
            DTask::kMenu,   (long)deadbody));
    }
#endif

	gWindowManager->DeleteWindow(this, false);
}
					
					
void DWindow::InitWindow(WindowStyles style, 
						short width, short height, short left, short top, char* title, Nlm_Boolean freeOnClose) 
{
	//this->Initialize(); // << this is done in call to DView::DView() 
	fFreeOnClose= freeOnClose;
	switch (style) {
		case document:
			fWindow= Nlm_DocumentWindow(left, top, width, height, title, windowCloseProc, windowResizeProc);
			break;
		case fixed:
			fWindow= Nlm_FixedWindow(left, top, width, height, title, windowCloseProc);
			break;
		case frozen:
			fWindow= Nlm_FrozenWindow(left, top, width, height, title, windowCloseProc);
			break;
		case round:
			fWindow= Nlm_RoundWindow(left, top, width, height, title, windowCloseProc);
			break;
		case alert:
			fWindow= Nlm_AlertWindow(left, top, width, height, windowCloseProc);
			break;
		case modal:
			fWindow= Nlm_ModalWindow(left, top, width, height, windowCloseProc);
			break;
		case floating:
			fWindow= Nlm_FloatingWindow(left, top, width, height, windowCloseProc);
			break;
		case shadow:
			fWindow= Nlm_ShadowWindow(left, top, width, height, windowCloseProc);
			break;
		case plain:
			fWindow= Nlm_PlainWindow(left, top, width, height, windowCloseProc);
			break;
		}
		
	this->SetNlmObject( fWindow);
	Nlm_SetActivate(fWindow, windowActivateProc);
	Nlm_SetDeactivate(fWindow, windowDeactivateProc);
	gWindowManager->AddWindow(this);
}					


void DWindow::SetFilename( char* name) 
{
	if (name) {
		MemFree(fFilename);
		fFilename= StrDup(name);
		}
}

char* DWindow::GetFilename( char* name, ulong maxsize)
{
	if (name==NULL) name= (char*) MemNew(maxsize);  
	Nlm_StrNCpy( name, fFilename, maxsize);
	return name;
}


void DWindow::AddOkayCancelButtons( long okayId, char* okayName, long cancelId, char* cancelName)
{
 	Nlm_PoinT npt;
 	this->NextSubviewBelowLeft(); // this doesn't account for extra size of DDefaultButton !
 	this->GetNextPosition( &npt);
 	npt.y += 10;  // is +3 for Mac, +8 for MSWin, +10 for Motif
 	this->SetNextPosition( npt);
	DButton* bb= new DButton( cancelId, this, cancelName);
	bb->SetResize( DView::fixed, DView::moveinsuper); // testing, want this??
	bb->suicide(1);
	
	this->NextSubviewToRight();
 	this->GetNextPosition( &npt);
 	npt.x += 10;  // is +3 for Mac, +8 for MSWin, +10 for Motif
 	this->SetNextPosition( npt);
	DDefaultButton* db= new DDefaultButton( okayId, this, okayName);
	db->SetResize( DView::fixed, DView::moveinsuper);
	db->suicide(1);
}


Nlm_Boolean DWindow::IsMyAction(DTaskMaster* action) 
{	
	// ?? add fEditText cut/copy/paste handling here??
	switch(action->Id()) {
		case cOKAY:
			this->OkayAction();
			fOkay= true;
			// fall thru to next case
		case cCANC: 
			fModal= false;
			this->Close(); // !! Cant Free yet, caller may need object's data !! CloseAndFree();
			return true;
		default:
			return DView::IsMyAction(action);	
		}
}

void DWindow::OkayAction() 
{ 
}

Nlm_Boolean DWindow::PoseModally() 
{ 
	fModal = true;
	this->Open();
	while (fModal)  
		gApplication->ProcessTasks();  
	return fOkay; // ?? ain't what we want - want fSubordinate's fId
}
 

void DWindow::CalcWindowSize()
{
#ifdef WIN_MSWIN
  short WinExtraHeight = kWinExtraHeight;

  // note: SM_CYHSCROLL == Nlm_hScrollBarHeight
  WinExtraHeight = 10 + GetSystemMetrics(SM_CYHSCROLL);

	Nlm_ResizeWindow( (Nlm_GraphiC)fWindow, kWinDragHeight, kWinScrollWidth,
						 kWinMinWidth, WinExtraHeight);

#else
	Nlm_ResizeWindow( (Nlm_GraphiC)fWindow, kWinDragHeight, kWinScrollWidth,
						 kWinMinWidth, kWinExtraHeight);
#endif
}

void DWindow::ResizeWin() 
{
	Nlm_PoinT delta;
	Nlm_RecT 	current;
	this->ViewRect(current);
			// need to check that fViewrect was set to valid values
	if (fViewrect.bottom>0 && fViewrect.right>0 && fViewrect.top>=0 && fViewrect.left>=0) {
		delta.x = (current.right - current.left) - (fViewrect.right - fViewrect.left);
		delta.y = (current.bottom - current.top) - (fViewrect.bottom - fViewrect.top);
		if (delta.x || delta.y) {
			this->Select(); // for motif ??
			this->ResizeSubviews( this, delta);
			}
		}
	fViewrect= current;
	gWindowManager->SetCurrent(this);
}


void  DWindow::Select() 
{
	DView::Select();
	Nlm_UseWindow(fWindow); // 23feb -- does Motif need this also ??
	gWindowManager->SetCurrent(this);
}

void DWindow::Open()  
{ 
#if 1
	// test order of this, 15may94
	this->Select(); 
	this->Show(); 
#else
	this->Show(); 
	this->Select(); 
#endif
	this->ViewRect(fViewrect); 
	//gWindowManager->SetCurrent(this); // select does
}

void DWindow::Hide() 
{ 
	DView::Hide(); 
	gWindowManager->UnsetCurrent(this);
}

void DWindow::Close() 
{ 
	fModal= false;
	if (this != gWindowManager->GetAppWindow()) {
		if (fSaveHandler) {
			char mytitle[50];
			GetTitle(mytitle, sizeof(mytitle));
			if (fSaveHandler->DirtySaveCancelled(mytitle)) 
				return; 
			}

		this->Hide(); 
		if (fFreeOnClose) {
			
					// We may have a LARGE BUG here... 
					// looks like DObject::suicide isn't calling destructors of DObject subclasses !!!!!!!!!
					
			//this->suicide(); 
			//delete this;  << THIS IS BAD programming
			gWindowManager->DeleteWindow(this, true);
			}
		gWindowManager->UnsetCurrent(this);
		}
}

void DWindow::CloseAndFree() 
{ 
	fModal= false;
	if (this != gWindowManager->GetAppWindow()) {
		gWindowManager->DeleteWindow(this, false); // do so close/hide won't mess w/ win list
		fFreeOnClose= true;
		this->Close(); 
		}
}

void DWindow::PrintDoc()  
{
	if (fPrintHandler) fPrintHandler->Print(); 
	else Message(MSG_OK,"DWindow::Print not ready.");
}

void DWindow::SaveDoc(DFile* f)  
{ 
	if (fSaveHandler) fSaveHandler->Save(f);
	else Message(MSG_OK,"DWindow::Save not ready.");
}


void DWindow::SendToBack()
{
	gWindowManager->SendToBack(this);
}  

void DWindow::BringToFront()
{
	gWindowManager->BringToFront(this);
}  

void  DWindow::Activate() 
{
	gWindowManager->SetCurrent(this);
}

void  DWindow::Deactivate() 
{
	//gWindowManager->UnsetCurrent(this);
}

void DWindow::Erase() 		
{
	Nlm_EraseWindow(fWindow); 
}

void DWindow::Use() 			
{ 
	Nlm_UseWindow(fWindow); 
	gWindowManager->SetCurrent(this);
}

Nlm_Boolean DWindow::IsUsing() 
{ 
	return Nlm_UsingWindow(fWindow); 
}

void DWindow::Realize() 	
{ 
	Nlm_RealizeWindow(fWindow); 
}

#if MAC_ONLY_OPTION
Nlm_Boolean DWindow::InFront() 
{ 
	return Nlm_InFront(fWindow); 
}
#endif






// DSaveHandler

class DSaveDocDialog : public DWindow 
{
public:	
	DSaveDocDialog(char* title);
};

DSaveDocDialog::DSaveDocDialog(char* title) :
	DWindow( 0, NULL, DWindow::modal, -10, -10, -50, -20, "", kDontFreeOnClose) // DWindow::fixed
{
	char str[80];
	if (title && *title) {
		StrCpy( str, "Save document '");
    StrNCat( str, title, 20);
    if (StrLen(title)>20) StrCat( str, "...");
    StrCat( str, "'?");
		}
  else
	  StrCpy( str, "Save this document?");
	new DPrompt( 0, this, str, 0, 0, Nlm_systemFont);	 		
	//DButton* but= new DButton( cMapBut, this, "Cancel");
	this->AddOkayCancelButtons(cOKAY,"Yes",cCANC,"No");	
	DWindow::Open();
}



short DSaveHandler::DirtySaveCancelled(char *doctitle) 
{ 
	short cancelled= 0;
	if (fDirty) {
		// save/dontsave/cancel;
		DSaveDocDialog* dlog= new DSaveDocDialog(doctitle);
		if (dlog->PoseModally()) { 
			gApplication->DoMenuTask( DApplication::kSaveAs, NULL);
			}
    NotDirty();  // ?? regardless of user choice?
		delete dlog; // <!! not if kFreeOnClose is DWindow option !!
		}
	return cancelled; 		// return cancelSave
}






// class DWindowManager


DWindowManager::DWindowManager()
{
	fWindows= new DList();
	fAppWindow = NULL;
	fLastActive= NULL;
}

DWindowManager::~DWindowManager()
{
	long i, n= fWindows->GetSize();
	for (i= n-1; i>0; i--) { 
		DWindow* theWin= (DWindow*) fWindows->At(i); 
		if (theWin) {
			theWin->fFreeOnClose= false;
			theWin->Close(); // to save dirty?
			delete theWin; 
			}
		}
	delete fWindows;
}

void DWindowManager::AddWindow(DWindow* theWin)
{
	if (theWin != fAppWindow) {
		fWindows->InsertLast(theWin);
		fLastActive= theWin;
		}
}

void DWindowManager::DeleteWindow(DWindow* theWin, Nlm_Boolean postDeleteTask)
{
	if (theWin == fAppWindow) fAppWindow= NULL;
	else if (theWin) {
	  fWindows->Delete(theWin); // do before Unset so it isn't stuck back in win list
    UnsetCurrent(theWin);
		// 4sep94: fix to eliminate use of "delete this" in DWindow::Close()
		if (postDeleteTask) delete theWin;  // ~DWindow calls back here, with !postDeleteTask
	  }
}

void DWindowManager::SetCurrent(DWindow* theWin)
{
	if (theWin && theWin != fAppWindow) {
		if (theWin != (DWindow*)fWindows->Last()) {
			fWindows->Delete(theWin);
			fWindows->InsertLast(theWin);
			}
		fLastActive= theWin;
		}
}

void DWindowManager::UnsetCurrent(DWindow* theWin)
{
	if (theWin && theWin == fLastActive) {
		if (theWin == (DWindow*)fWindows->Last()) {
			fWindows->Delete(theWin);
				//??? where do we put it in list so it cycles to top as real window does
			fWindows->InsertFirst(theWin); 
			}
		fLastActive= (DWindow*)fWindows->Last();
		}
}


DWindow*	DWindowManager::CurrentWindow() 
{
#if 0
//#ifdef WIN_MAC
	return (DWindow*) Nlm_GetObject( (Nlm_GraphiC)Nlm_FrontWindow());
#else
			// try to get motif to recognize front/active window
	return fLastActive;
#endif
}



void DWindowManager::SendToBack(DWindow* theWin)
{
	if (!theWin) theWin= (DWindow*) fWindows->Last();
	if (theWin) {
		this->UnsetCurrent(theWin);
		
#ifdef WIN_MAC
		Nlm_WindowTool wptr= Nlm_ParentWindowPtr((Nlm_GraphiC)theWin->fWindow);
		if (wptr) ::SendBehind( wptr, NULL);
#endif

#ifdef WIN_MSWIN
			// apparently no SendBehind for MSWindows !! 
			// do it the hard way ... bring all others to front
			// this probably looks like shit
			// UnsetCurrent puts theWin == fWindows.First(), so start w/ iwin=1, after 0th
		short nwin= fWindows->GetSize();
		for (short iwin=1; iwin<nwin; iwin++) {
			DWindow* aWin= (DWindow*)fWindows->At(iwin);
    	Nlm_WindowTool wptr = Nlm_ParentWindowPtr((Nlm_GraphiC)aWin->fWindow);
   		if (wptr) ::BringWindowToTop(wptr);
			}
#endif

#ifdef WIN_MOTIF
    if (Nlm_WindowHasBeenShown(theWin->fWindow)) {
      if (Nlm_currentXDisplay != NULL) {
        //Nlm_ShellTool shl;
				Widget shl; // for IRIX

        shl = Nlm_GetWindowShell(theWin->fWindow);
        //XMapRaised (Nlm_currentXDisplay, XtWindow (shl));
#ifndef OS_UNIX_IRIX
// can't get dang sgi to compile w/ XtWindow(Widget) !!
				XLowerWindow(Nlm_currentXDisplay, XtWindow (shl));
#endif
        }
      }
#endif

		theWin= (DWindow*) fWindows->Last();
		theWin->Select();
		}
}



void DWindowManager::BringToFront(DWindow* theWin)
{
 	// if null, lastwin
	if (!theWin) {
		theWin= (DWindow*) fWindows->First();
		if (theWin == fAppWindow) theWin= (DWindow*) fWindows->At(1); // skip appwin
		}
	if (theWin) theWin->Select();
}


void DWindowManager::SetAppWindow(DWindow* theWin)
{
	fAppWindow= theWin;
	if (theWin) Nlm_SetAppWindow( theWin->fWindow);
}

DWindow* DWindowManager::GetAppWindow()
{
	return fAppWindow;
}


Nlm_Boolean	DWindowManager::InWindow(Nlm_PoinT mouse) 
{ 
	return Nlm_InWindow(mouse); 
}

DWindow*	DWindowManager::WhichWindow(Nlm_PoinT mouse) 
{
	return (DWindow*) Nlm_GetObject( (Nlm_GraphiC)Nlm_WhichWindow(mouse));
}


	
#ifdef THESE_ARE_REDUNDANT
DWindow*	DWindowManager::FrontWindow() 
{
	return (DWindow*) Nlm_GetObject( (Nlm_GraphiC)Nlm_FrontWindow());
}

DWindow*	DWindowManager::ActiveWindow() 
{
	return (DWindow*) Nlm_GetObject( (Nlm_GraphiC)Nlm_ActiveWindow());
}
#endif


DWindow*	DWindowManager::SavePort(DView* newport) 
{
	Nlm_Handle nlmobject= newport->GetNlmObject();
	return (DWindow*) Nlm_GetObject( (Nlm_GraphiC)Nlm_SavePort(nlmobject));
}

void	DWindowManager::RestorePort(DWindow* savedwindow) 
{
	Nlm_RestorePort((Nlm_WindoW) savedwindow->GetNlmObject());
}

DDialogText*	DWindowManager::CurrentDialogText() 
{
	return (DDialogText*) Nlm_GetObject( (Nlm_GraphiC)Nlm_CurrentText());
}

void	DWindowManager::UpdateDisplays()
{
  Nlm_Update();
}
