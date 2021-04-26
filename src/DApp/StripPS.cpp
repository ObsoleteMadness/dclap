// StripPS.cp
// d.g.gilbert -- simple file filter, strip codes from Postscript file leaving readable text
// demo app for DCLAP app library


#include "DClap.h"

class DMyApp : public DApplication
{
public:
	enum myapptasks {  mStripPS= 1000, kStripPS };
	void IMyApp(void);
	void DoAboutBox();
	void TextDocDemo(void);
	void SetUpMenus(void);	// override
	Boolean IsMyAction(DTaskMaster* action); //override
	void UpdateMenus(void); // override
};



extern "C" short Main(void)
{
  DMyApp* myapp = new DMyApp();
  myapp->IMyApp();
  myapp->Run();
  delete myapp; 
  return 0;
}


void DMyApp::IMyApp()
{
	IApplication();
}	 

void DMyApp::DoAboutBox()
{
	char* aboutMe =
"A simple file filter to convert Postscript code"LINEEND
"into readable text."LINEEND
"Strictly a hack, to work on one form of ps code only.";
	DAboutBoxWindow* about = new DAboutBoxWindow(aboutMe); // make instance & it handles rest
}

void DMyApp::SetUpMenus()
{
	DApplication::SetUpMenus();
 
	DMenu* sMenu = NewMenu(mStripPS, "Strip PS");
	sMenu->AddItem( kStripPS, "Strip PS file");
}


void DMyApp::TextDocDemo()
{
	char* filename= (char*) gFileManager->GetInputFileName(".ps","TEXT");
	if (filename) {
		char *cp;
		char	line[256], *endp;
		Boolean inword= false;

		gCursor->watch();
		DFile* psf= new DFile(filename,"r");
		if ((cp= StrStr(filename,".ps")) != NULL) *cp= 0;
		filename= StrAppend(filename,".pstripped");
		DFile* stripf= new DFile( filename, "w");
		
		psf->OpenFile();
		stripf->OpenFile();
		while (psf->ReadLine(line, 255) == 0) {

			if (inword)
				cp= line;
			else {
				cp= strchr(line,'(');
				if (cp) { cp++; inword= true; }
				else cp= line;
				}
				
			if (inword) {
				endp= strchr(cp,')');
				if (endp) { *endp++= '\t'; *endp= '\0'; inword= false; }
				else endp= strchr(cp,'\0');
				stripf->WriteLine(cp);
				}
			else if (StrStr(line,"%ADO") != 0 || StrStr(line," sf") != 0) {
				stripf->WriteLine("\n");
				}
			else if (StrStr(line,"%%Page") != 0) {
				stripf->WriteLine("\n\n\n");
				}
				
			}
		psf->CloseFile();
		stripf->CloseFile();
		
		
		Nlm_ParData paratabs = {false,false,false,false,true,0,0}; // tabs==tabs
		short tabstep = 16;
		
		char* shortname= (char*) gFileManager->FilenameFromPath(filename);
		DWindow* win= new DWindow( 0, this, DWindow::document, -1, -1, -10, -10, shortname);
		DTextDocPanel* td= new DTextDocPanel( 0, win, 490, 300);
		td->SetResize( DView::matchsuper, DView::relsuper);
		td->ShowFileFancy(filename, &paratabs, NULL, Nlm_programFont, tabstep);
		td->suicide(1);
		
		//win->AddOkayCancelButtons();
		win->Open();
		win->suicide(1);
		free(filename);
		gCursor->arrow();
		}
}





Boolean DMyApp::IsMyAction(DTaskMaster* action) 
{
	DWindow* win;
	
	switch(action->Id()) {
		
				// some menu item messages
		case DMyApp::kStripPS:
			this->TextDocDemo();
			return true;

		case cOKAY:
		case cCANC:
			{
			DButton* but= (DButton*) action; 
			if ( but && ((win= but->GetWindow()) != NULL) ) {
				if (action->Id() == cOKAY) {
					// do Okay close handling...
					}
				win->CloseAndFree();
				}
			}
			return true;
	   
		default: 
			return DApplication::IsMyAction(action);
		}
}



void DMyApp::UpdateMenus(void)
{
	DApplication::UpdateMenus();
	
	//gViewCentral->EnableView(DMyApp::kSpecial1);
	//gViewCentral->EnableView(DMyApp::kSpecial2);
}
