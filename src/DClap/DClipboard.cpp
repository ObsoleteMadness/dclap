// UClipboard.cp 


#include "DClipboard.h"
#include <dgg.h>
#include <DWindow.h>
#include <DPanel.h>
#include <DTaskMaster.h>
#include <DApplication.h>
#include <DViewCentral.h>

#ifdef WIN_MAC
	// undef some conflicts b/n Types.h && NlmTypes
#undef Handle
#include <Scrap.h>
#endif


DClipboardMgr* gClipboardMgr = NULL;			 


class DInitClipboard {
public:
	DInitClipboard() 	{ gClipboardMgr = new DClipboardMgr(); }  
};

DInitClipboard globalInit; // do init now




// class DClipboardMgr


DClipboardMgr::DClipboardMgr() : DTaskMaster(0)
{
	fClipView = NULL;
	fClipOrphan= NULL;
	fClipWindow = NULL;
	fGotClipType = FALSE;
	fOldScrapStuff= NULL;
	fNewScrapStuff= NULL;
	gClipboardMgr = this;
	fPrefClipType = Str2Idtype("TEXT"); // kPICT, ...
	
#ifdef WIN_MAC
	fOldScrapStuff= MemNew(sizeof(ScrapStuff));
	fNewScrapStuff= MemNew(sizeof(ScrapStuff));
#endif
} 


void DClipboardMgr::AboutToLoseControl(Boolean convertClipboard)
{
	if (convertClipboard) {
		if (fClipView && !fClipWrittenToSystem) {
#ifdef WIN_MAC
			long err = ZeroScrap();
#endif
#if FIX
 			fClipView->WriteToSystemClipboard();  //WriteToDeskScrap
#endif
 			fClipWrittenToSystem = TRUE;
			this->LoadSystemClip();
		}
	}
}  


void DClipboardMgr::LoadSystemClip()
{
#ifdef WIN_MAC
	ScrapStuff* newscrap = InfoScrap();
	MemCpy( fOldScrapStuff, fNewScrapStuff, sizeof(ScrapStuff));
	MemCpy( fNewScrapStuff, newscrap, sizeof(ScrapStuff));
#endif 
}  



void DClipboardMgr::CheckSystemClip()
{
	this->LoadSystemClip();
#ifdef WIN_MAC
	if (((ScrapStuff*)fOldScrapStuff)->scrapCount 
	 != ((ScrapStuff*)fNewScrapStuff)->scrapCount) {
#if FIX
		fClipView->FreeFromClipboard();		 
#endif
		fClipView = NULL;	 
		this->ReadFromSystem();
	}
#endif
}  



long DClipboardMgr::GetDataToPaste(char* data, ulong datalen, long& clipKind)
{
	long length= 0;
	if (fGotClipType) {
		clipKind = fPrefClipType;
#if FIX
		length = fClipView->GivePasteData(data, datalen, clipKind);
#endif
		if (length < 0) ; //Failure((OSErr)length, 0);
		}
	else 
		length = -1;
	return length;
}  




void DClipboardMgr::Launch()
{
	// Create the clip window and default view 
	fClipWindow = this->MakeClipboardWindow();
	DView* aview= fClipWindow->FindSubview(kIDClipView);
	if (aview) fClipOrphan = aview;

	// Add the clipboard manager as an application behavior
	//gApplication->AddBehavior(this);

	this->LoadSystemClip();
	this->ReadFromSystem();
} 



class DClipView : public DNotePanel {
public:
	short fTestFlag;
	DClipView( DWindow* aWindow) :
		DNotePanel( DClipboardMgr::kIDClipView, aWindow, "Clip view"),
		fTestFlag(0)
		{}
	virtual ~DClipView();
};

class DClipWindow : public DWindow {
public:
  short  fTestShort;
	DClipWindow() :
	 DWindow( DClipboardMgr::kIDClipView+1, NULL, DWindow::document, -5, -5, -50, -20, "Clipboard"),
	 fTestShort(0)
	 {}
	virtual ~DClipWindow();
};

DClipWindow::~DClipWindow()
{
	if (fTestShort) ; // debugger stop point... do we get here !?
}

DClipView::~DClipView()
{
	if (fTestFlag) ; // debugger stop point... do we get here !?
}

DWindow* DClipboardMgr::MakeClipboardWindow()
{
	DClipWindow* aWindow= new DClipWindow(); 
	if (!fClipOrphan) fClipOrphan= new DClipView( aWindow);
	fClipView = fClipOrphan;
	return aWindow;
} 


void DClipboardMgr::ReadFromSystem()
{
	DView* aViewForClipboard = this->MakeViewForAlienClipboard();
	this->SetClipView(aViewForClipboard);
	fClipWrittenToSystem = TRUE;	 
}  


DView* DClipboardMgr::MakeViewForAlienClipboard()
{
	DView * aView = NULL;
	//aView= gApplication->MakeViewForAlienClipboard();
	if (!aView) aView = fClipOrphan;
	return aView;
}  

void DClipboardMgr::RegainControl(Boolean checkClipboard)
{
	if (checkClipboard) this->CheckSystemClip();
}  


void DClipboardMgr::SetClipView(DView* clipView)
{
	if (fClipWindow && clipView != fClipView) {
		// get the old clip window scroller if possible
		DView * theSuperView;
		Boolean haveview;
		theSuperView = fClipWindow;
			
			// throw out any views contained in the scroller (or window)
		haveview= false;
		if (theSuperView->fSubordinates) {
			short i, n= theSuperView->fSubordinates->GetSize();
			//for (i=0; i<n; i++) 
			for (i=n-1; i>=0; i--) {
				DView* subview= (DView*) theSuperView->fSubordinates->At(i);
				//?? if (fClipWindow->IsVisible()) subview->Close();
				haveview= (subview == clipView); 
				Boolean dodelete= (!haveview && subview != fClipOrphan);
#if 1
				if (subview) {
					theSuperView->RemoveSubview(subview, false);
					if (dodelete) {
                 //mswin is failing here at times
						if (subview->fSubordinates) subview->DeleteSubviews();
						delete subview; subview= NULL;
						// RemoveSubview == theSuperView->fSubordinates->AtDelete(i);
						}
					}
#else
				theSuperView->RemoveSubview(subview, dodelete);
#endif
				}
			}
		
			// pop in the new clipview
		if (clipView) {
			if (!haveview) theSuperView->AddSubordinate(clipView);
			clipView->SetSuperior(theSuperView->GetNlmObject()); 
			clipView->Invalidate();
			clipView->Disable();	 
			fClipWrittenToSystem = (clipView == fClipOrphan);
			}
		}
	fClipView = clipView;
} 


void DClipboardMgr::DoSetupMenus()
{
	gViewCentral->EnableView(DApplication::kShowClipboard);
}  


void DClipboardMgr::CanPaste(long clipKind)
{
#if FIX
	if (fClipView && !fGotClipType)
		if (fClipView->ContainsClipType(clipKind)) {
			fGotClipType = TRUE;
			fPrefClipType = clipKind;
			}
#endif
	if (fGotClipType)
		gViewCentral->EnableView(DApplication::kPaste);
	else
		gViewCentral->DisableView(DApplication::kPaste);
}  


Boolean DClipboardMgr::DoMenuTask(long tasknum, DTask* theTask)
{
	switch (tasknum) {
	
		case DApplication::kShowClipboard:
#if 0
			if (fClipWindow == gWindowManager->CurrentWindow())
				fClipWindow->Close(); //CloseAndFree();	 
			else {
				fClipWindow->Open();
				fClipWindow->Select();
				}
#endif
			return true;
			
		default:
			return DTaskMaster::DoMenuTask(tasknum, theTask);
			break;
	}
} 


Boolean DClipboardMgr::IsMyAction(DTaskMaster* action)
{
	switch (action->Id()) {
		case DApplication::kShowClipboard:
			return DoMenuTask(action->Id(), NULL);
		default:
			return DTaskMaster::IsMyAction(action);
		}
}


void DClipboardMgr::Close()
{
#ifdef WIN_MAC	
	LoadScrap();
#endif
} 


short DClipboardMgr::PutToSystem(char* data, ulong datasize, long clipKind)
{
	long err= -1;

#ifdef WIN_MAC	
	err = PutScrap( datasize, clipKind, data);
#endif
	return err;
} 



