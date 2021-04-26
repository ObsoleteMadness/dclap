// DWindow.h
// d.g.gilbert

#ifndef _DWINDOW_
#define _DWINDOW_

#include "Dvibrant.h"
#include "DView.h"
#include "DMethods.h"

class DDialogText;
class DList;
class DFindDialog;
class DFile;


class DWindow : public DView
{	
public:
	enum WindowStyles { document, fixed, frozen, round, alert, modal, floating, shadow, plain };
	enum WindowConst { kDontFreeOnClose = false, kFreeOnClose = true };
	Nlm_WindoW			fWindow;
	DDialogText		* fEditText; // for use w/ cut/copy/paste/etc
	DPrintHandler	*	fPrintHandler;
	DSaveHandler	*	fSaveHandler;
	Boolean					fModal, fOkay, fFreeOnClose;
	DFindDialog		* fFindDlog;
	char					*	fFilename;
	
	DWindow(long id, DTaskMaster* itsSuperior, WindowStyles style,
					short width = -5, short height = -5, short left = -50, short top = -20, 
					char* title = NULL, Nlm_Boolean freeOnClose= kFreeOnClose);
	DWindow(long id, DTaskMaster* itsSuperior); // user of this MUST call Initialize()
	virtual ~DWindow();

	virtual void InitWindow(WindowStyles style = fixed, 
					short width = -5, short height = -5, short left = -50, short top = -20, 
					char* title = NULL, Nlm_Boolean freeOnClose= kFreeOnClose);
					
	virtual void Open();
	virtual void Hide(); 
	virtual void Close();
	virtual void CloseAndFree();
	virtual	void AddOkayCancelButtons( 
			long okayId = cOKAY, char* okayName = "Okay",
			long cancelId = cCANC, char* cancelName = "Cancel");

	virtual Boolean IsMyAction(DTaskMaster* action); // traps OKAY, CANC messages
	virtual Boolean PoseModally(); 	// not really modal, just open/eventloop til close/return okayHit 
	virtual void 		OkayAction(); 	// override to handle 'OKAY' message

	virtual void PrintDoc();
	virtual void SaveDoc(DFile* f);
		
	virtual void  Select(); 		// override
		// activate()/deactivate() are not useful in x-win
		// need an equivalent for all win systems, ???? what 
	virtual void  Activate();
	virtual void  Deactivate();
	
	virtual void  ResizeWin();
	virtual void  CalcWindowSize();
	
	Boolean  HasEditText() { return (fEditText != NULL); }
	void  SetEditText(DDialogText* theText) { fEditText= theText; }

	virtual void Erase();
	virtual void Use();					// useful way to activate a window ! 
	virtual Boolean IsUsing();	// ??
	virtual void Realize();			// == Nlm_DoShow(), which is good for what?? 
	//virtual Boolean InFront();	// Mac only
	virtual void SendToBack();  
	virtual void BringToFront();  
	
	virtual char* GetFilename( char* name = NULL, ulong maxsize = 256);
	virtual void  SetFilename( char* name);
};




class	DWindowManager	: public	DObject
{
	DList*	fWindows;
	DWindow* fAppWindow;	// main-menu window for app, non-mac systems
	DWindow* fLastActive;	// last current, non-main window (? non-modal...)
public:
	DWindowManager();
	virtual ~DWindowManager();
	
	virtual void AddWindow(DWindow* theWin);
	virtual void DeleteWindow(DWindow* theWin, Nlm_Boolean postDeleteTask = true);
	virtual void SetAppWindow(DWindow* theWin);
	virtual DWindow* GetAppWindow();
	virtual void SetCurrent(DWindow* theWin);
	virtual void UnsetCurrent(DWindow* theWin);
	virtual DWindow*	CurrentWindow();	// all, but may not be accurate?
	virtual DList* GetWindowList() { return fWindows; }

	virtual void SendToBack(DWindow* theWin = NULL); // if null, frontwin
	virtual void BringToFront(DWindow* theWin = NULL); // if null, lastwin

			// these are not all useful in all win systems
			// consolidate as one function CurrentWindow() that
			// always returns the app's front/current/active window
	//virtual DWindow*	FrontWindow();		// Mac, MSWin, NOT xwin
	//virtual DWindow*	ActiveWindow();		// Mac, MSWin, NOT xwin

	virtual DWindow* WhichWindow(Nlm_PoinT mouse); 	// Mac, MSWin, NOT xwin
	virtual Boolean	InWindow(Nlm_PoinT mouse);			 // Mac, MSWin, NOT xwin

	virtual DWindow* SavePort(DView* newport);
	virtual void RestorePort(DWindow* savedwindow);
	
	virtual DDialogText* CurrentDialogText();

	virtual void	UpdateDisplays();
};


extern	DWindowManager*	gWindowManager;


#endif
