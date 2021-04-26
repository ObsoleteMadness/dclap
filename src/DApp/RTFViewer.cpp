// RTFViewer.cpp
// d.g.gilbert --Rich Text Format (RTF) document display, using DClAp library
// may 1994


#include "DClap.h"
#include "DRTFView.h"

#if 0
#include "rtf.h"
#include "drtfwriter.h"
#include "drtfdoc.h"
#endif


class DRTFViewApp : public DApplication
{
public:
	enum myapptasks { kRTFdemo=1301 };
	DRTFsetup* fRTFsetup;	

	void IMyApp(void);	 
	virtual ~DRTFViewApp();
	
	virtual void RTFdemo(void);	

	virtual void OpenDocument(DFile* aFile);	// override
	virtual void OpenHelp(DFile* aFile); // override
	virtual void SetUpMenus(void);	// override
	virtual	Boolean IsMyAction(DTaskMaster* action); //override
};




extern "C" short Main(void)
{
  DRTFViewApp* myapp = new DRTFViewApp();
  myapp->IMyApp();
  myapp->Run();
  delete myapp; 
  return 0;
}


void DRTFViewApp::IMyApp(void)
{
	IApplication();
	fRTFsetup= new DRTFsetup(); // depends on gFileManager created in IApplication
}	 

DRTFViewApp::~DRTFViewApp()
{
	delete fRTFsetup;
}

void DRTFViewApp::SetUpMenus(void)
{
	DApplication::SetUpMenus();
#if 0
	DMenu* sMenu = NewMenu(kRTFdemo, "RTF");
	sMenu->AddItem( DRTFViewApp::kRTFdemo, "Open RTF file...");
#endif
}



 
 
void DRTFViewApp::RTFdemo()
{
	char* filename= (char*) gFileManager->GetInputFileName(NULL,NULL);
	if (filename) {
		filename= StrDup(filename);
		char* shortname= (char*)gFileManager->FilenameFromPath(filename);
		gCursor->watch();
		DWindow* win= new DWindow( 0, this, DWindow::document, -1, -1, -10, -10, shortname);

		short docwidth= gPrintManager->PageWidth();	
		DRTFView* rtfdoc= new DRTFView( 0, win, docwidth, 0);
		rtfdoc->SetResize( DView::matchsuper, DView::relsuper);
		//rtfdoc->SetSlateBorder( false);
		rtfdoc->ShowFile( filename);	
			
		win->Open();
		rtfdoc->SizeToSuperview( win, true, false); // this needs to adjust for if view has scrollbar !
		MemFree(filename);
		gCursor->arrow();
		}
}


void DRTFViewApp::OpenHelp(DFile* aFile)
{
	if (!aFile->Exists()) {
		Message(MSG_OK,"Can't find help file '%s'",aFile->GetName());
		return;
		}
#if 1
	this->OpenDocument(aFile); 
#else
	DApplication::OpenHelp(aFile);
#endif 
}

void	DRTFViewApp::OpenDocument(DFile* aFile)
{
	// default OpenDoc is to use Vibrant's text display
	if (aFile && aFile->Exists()) {

		gCursor->watch();
		aFile->CloseFile(); // ShowFile manages this..
		char* shortname= (char*)aFile->GetShortname();
		const char* fullname= aFile->GetName();
		DWindow* win= new DWindow(0, this, DWindow::document, -1, -1, -10, -10, shortname);

		short docwidth= gPrintManager->PageWidth();	
		DRTFView* rtfdoc= new DRTFView( 0, win, docwidth, 0);
		rtfdoc->SetResize( DView::matchsuper, DView::relsuper);
		//rtfdoc->SetSlateBorder( false);
		rtfdoc->ShowFile( (char*)fullname);

		win->Open();
		rtfdoc->SizeToSuperview( win, true, false); // this needs to adjust for if view has scrollbar !
		gCursor->arrow();
		}
}


Boolean DRTFViewApp::IsMyAction(DTaskMaster* action) 
{
	DWindow* win;
	
	switch(action->Id()) {

		case kRTFdemo: this->RTFdemo(); return true;
		
		default: 
			return DApplication::IsMyAction(action);
		}
}


