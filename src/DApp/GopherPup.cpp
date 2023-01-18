// GopherPup.cp
// d.g.gilbert -- gopher+ client, from GopherApp
// Jan 1994


#include <DClap.h>
#include <DRich.h>
#include <DNet.h>
#include <DDrawPICT.h>



class DGopherPup : public DApplication
{
public:
	static char* kBugmailAddress;
	static char* kGopherRTFDocSuffix;
	//DRTFsetup		* fRTFsetup;	
	//DHTMLsetup	* fHTMLsetup;	
	char*			fGopherDocSuffix;
	
	enum appMenus { kInternetMenu = 100, kPrefsMenu }; // kViewKindMenu, kViewChoiceMenu 
	enum internetMenu { 
		kViewPrefs=1001,  kOtherPrefs, kEditGopherMap,
		kViewByDefault,kViewByDate,kViewBySize,kViewByName,kViewByKind,kViewByHost,
		kViewAsText, kViewAsLink,
		kOpenDoc, kFetchLink, kFetchDefault, // kFetchView1...kFetchViewN
		kFetchOptionFlag, kAutoloadFlag,
		kGetInfo, kMailAdmin, kOpenLocalGo,
		kNewServer,kFindServer,kNewLink, kEditLink, kExtractGodoc,kLockWindow,
		kMailSetup, kSendMail, kBugMail
		};
		
	DView *	fFetchOptionMenuItem;
	DView *	fLockWinMenuItem;
	DView * fAutoloadItem;

	void IGopherPup(void);	 
	virtual ~DGopherPup();
	void DoAboutBox();

	virtual void SetUpMenus(void);	// override
	virtual	Boolean IsMyAction(DTaskMaster* action); //override
	virtual Boolean IsInternetMenu(DTaskMaster* action); 
	virtual	void UpdateMenus(void); // override
	virtual void OpenDocument(DFile* aFile);
	virtual void OpenHelp(DFile* aFile); // override
	virtual void OpenLocalGopher();

};


char* DGopherPup::kBugmailAddress= "gopherpup@bio.indiana.edu";
char* DGopherPup::kGopherRTFDocSuffix = ".start";


	// here we go -- the Nlm_Main() program (called from main() in ncbimain.c)
extern "C" short Main(void)
{
  DGopherPup* gopherpup = new DGopherPup();
  gopherpup->IGopherPup();
  gopherpup->Run();
  delete gopherpup; 
  return 0;
}



DGopherPup::~DGopherPup()
{
	DGoviewPrefs::SaveGlobals();
	DGoOtherPrefs::SaveGlobals();
	DDrawPict::Cleanup();
}


void DGopherPup::IGopherPup(void)
{
	char *onoffs;
#ifdef OS_DOS
	// damn 8char filename limits !
	DApplication::kName= "GophPup";
#else
	DApplication::kName= "GopherPup";
#endif
	DApplication::kVersion= " v 0.3d";
	
	fFetchOptionMenuItem= NULL;
	fLockWinMenuItem= NULL;
	fAutoloadItem = NULL;
	gGopherDoc = NULL; // ??? this should be set, but isn't in 68K ??

	IApplication();
	InitializeUGopher();
	//gIconList->ReadAppIcons(); //Read("GopherPup.icons");
	gGopherIcons= gIconList; 	// is this what we want??
	gIconSize= 2; // need to read in some app prefs  
	DGoviewPrefs::InitGlobals();
	DGoOtherPrefs::InitGlobals();
	if (fFetchOptionMenuItem) 
		fFetchOptionMenuItem->SetStatus(DGopherListDoc::gOptionIsOn);
	if (fAutoloadItem) 
		fAutoloadItem->SetStatus(DGoLinkedTextDoc::gAutoloadInlines);

	DRichView::InitRichView();
	//fRTFsetup = new DRTFsetup(); // depends on gFileManager created in IApplication
	//fHTMLsetup= new DHTMLsetup();
	
	fAcceptableFileTypes = NULL; 	// really want "IGo4 BKMK TEXT" and perhaps others
																// but NCBI tools only handle 1 type now
// This is a PAIN to have 1 default suffix when several are supported
	fFileSuffix = NULL; // unix, msdos file suffix

	fGopherDocSuffix= DGopherListDoc::kGopherDocSuffix;
	{
		// open default gopher document if it exists.
		// TRY RTF Form first (.start ?? )
		// open both default.go4 and default.start files, if they exist !
	char godefault[512];

	StrNCpy(godefault, (char*)Shortname(), 512);	
	StrNCat(godefault, fGopherDocSuffix, 512);
	DFile *goFile= new DFile( godefault, "r");
	if (goFile->Exists()) OpenDocument( goFile);
	else delete goFile;

	StrNCpy(godefault, (char*)Shortname(), 512);	
	StrNCat(godefault, kGopherRTFDocSuffix, 512);
	DFile * richFile= new DFile( godefault, "r");
	if (richFile->Exists()) OpenDocument( richFile);
	else delete richFile;
	}
}	 


void DGopherPup::DoAboutBox()
{
	char aboutMe[512];
	sprintf( aboutMe, "GopherPup, version %s" LINEEND " an Internet Gopher client ",
				DApplication::kVersion);
	DAboutBoxWindow* about = new DAboutBoxWindow(aboutMe);
}

void DGopherPup::SetUpMenus(void)
{
	DMenu* aMenu = NULL;
	//DApplication::SetUpMenus();

#ifdef WIN_MAC
	DAppleMenu* appleMenu = new DAppleMenu(this,fAboutLine);
#endif
	if (!aMenu) aMenu = this->NewMenu( cFileMenu, "File");
#ifndef WIN_MAC
	aMenu->AddItem(kAbout, fAboutLine);
	aMenu->AddSeparator();
#endif
	aMenu->AddItem(kNew,"New/N", false, true);
	//gViewCentral->DisableView(kNew); //?? didn't disable !?  
	aMenu->AddItem(kOpen,"Open Local.../O",false, true);
	aMenu->AddItem(kOpenLocalGo,"Open Local gopher...",false, true);
	aMenu->AddItem(kNewServer,"Open Remote.../R",false, true);
	aMenu->AddSeparator();
	aMenu->AddItem(kClose,"Close/W",false, true);
	aMenu->AddItem(kSave,"Save/S",false, true);
	aMenu->AddItem(kSaveAs,"Save As...",false, true);
	aMenu->AddSeparator();
	aMenu->AddItem(kPrint,"Print",false, true);
	aMenu->AddItem(kHelp,"Help/H",false, true);
	aMenu->AddSeparator();
	aMenu->AddItem(kQuit,"Quit/Q",false, true);

	aMenu= NULL;
	this->SetUpMenu(DApplication::cEditMenu, aMenu);

	DMenu* iMenu = NewMenu( kInternetMenu, "Internet");

#ifndef BOBS_WIN_MAC						
	// vibrant doesn't have methods to remove/rearrange items in existing menus for other wins
	iMenu->AddItem(  DGopherListDoc::kViewChoiceMenu, "Fetch by view...");
	//fViewsMenu= NULL;
	DGopherListDoc::gViewChoiceMenu = NULL;
#else					
	DMenu* aMenu= new DSubMenu( DGopherListDoc::kViewChoiceMenu, iMenu, "Fetch by view");
	aMenu->AddItem( DGopherListDoc::kViewDefault, "default");
	DGopherListDoc::gViewChoiceMenu = aMenu;
	// add/remove alternate views here, depending on selected gopher item
	aMenu->suicide(1);
#endif // WIN_MAC						

	iMenu->AddItem( kLockWindow,	"Tack window",true, true);						
	fLockWinMenuItem= (DView*) gViewCentral->GetView(kLockWindow);
	DGopherListDoc::gLockWinMenuItem = fLockWinMenuItem;
	
	iMenu->AddItem( kExtractGodoc,"Extract links in netdoc...",false, true);						

	DMenu* vMenu= new DSubMenu( DGopherListDoc::kViewKindMenu, iMenu, "View links");
	vMenu->AddItem( DGopherListDoc::kViewByDefault, "default");
	vMenu->AddItem( DGopherListDoc::kViewByDate, "by Date");
	vMenu->AddItem( DGopherListDoc::kViewBySize, "by Size");
	vMenu->AddItem( DGopherListDoc::kViewByName, "by Name");
	vMenu->AddItem( DGopherListDoc::kViewByKind, "by Kind");
	vMenu->AddItem( DGopherListDoc::kViewByHost, "by Host");
	vMenu->AddItem( DGopherListDoc::kViewAsText, "as Text");
	vMenu->AddItem( DGopherListDoc::kViewAsLink, "as Link text");

	iMenu->AddSeparator();	
	
  //	iMenu->AddItem( kNewServer, 	"Open remote.../R",false, true);
 	iMenu->AddItem( kFindServer, 	"Find server of link",false, true);
	iMenu->AddItem( kNewLink, 		"New link...",false, true);
	iMenu->AddItem( kEditLink, 		"Edit link...",false, true);						
	iMenu->AddSeparator();								

 	//iMenu->AddItem( kGetInfo, 		"Get Info from link/I",false, true);
 	iMenu->AddItem( kMailAdmin,		"Mail to link maintainer...",false, true);
	//iMenu->AddSeparator();								
	iMenu->AddItem( kSendMail, 		"Send Mail...",false, true);
	iMenu->AddItem( kBugMail, 		"Comment on this app...",false, true);
		
	
	iMenu = NewMenu( kPrefsMenu, "Options");
	iMenu->AddItem( kViewPrefs, "View Prefs...",false, true); 

	iMenu->AddItem( kFetchOptionFlag, "Fetch with options", true, true);
		// we can only access menu item thru gViewCentral now the way DMenu::AddItem() is written...
	fFetchOptionMenuItem= (DView*) gViewCentral->GetView(kFetchOptionFlag);

	iMenu->AddItem( kAutoloadFlag, "Autoload images", true, true);
	fAutoloadItem= (DView*) gViewCentral->GetView(kAutoloadFlag);
	iMenu->AddItem( kEditGopherMap, "Edit view handlers...",false, true);
	iMenu->AddItem( kOtherPrefs, 	"Other prefs...",false, true);
	iMenu->AddItem( kMailSetup, 	"Mail setup...",false, true);


	aMenu= NULL;
	this->SetUpMenu(DApplication::cWindowMenu, aMenu);

}





class	DBugMailDialog : public DSendMailDialog
{
public:
	DBugMailDialog(long id, DTaskMaster* itsSuperior,
		short width = -5, short height = -5, short left = -50, short top = -20, char* title = NULL):
		DSendMailDialog(id, itsSuperior, width, height, left, top, title)
		{}

	virtual DView* InstallTo(DView* super, char* toStr)  //override
	{
		return DSendMailDialog::InstallTo(super, DGopherPup::kBugmailAddress);
	}
	
};


class	DAdminMailDialog : public DSendMailDialog
{
public:
	DGopher* fGopher;
	
	DAdminMailDialog(long id, DTaskMaster* itsSuperior, DGopher* theGopherItem):
		DSendMailDialog(id, itsSuperior, -5, -5, -50, -20, "Mail gopher admin"),
		fGopher(theGopherItem)
	{
		// FailNIL(fGopher);
		fGopher->newOwner();
	}
	
	virtual ~DAdminMailDialog()
	{
		fGopher->suicide();
	}
	
	virtual DView* InstallMessage(DView* super, char* msgStr)
	{
		char* txt= (char*) MemGet(1, true);
		StrExtendCat( &txt, "Regarding the gopher item on your server, specified as:");
		StrExtendCat( &txt, LineEnd);
		fGopher->ToServerText( txt, 0);
		DView* msg= DSendMailDialog::InstallMessage(super, txt);
		MemFree(txt);
		return msg;
	}

	virtual DView* InstallSubject(DView* super, char *subjStr)  //override
	{
		char  addrbuf[256];
		StrCpy( addrbuf, "Re: gopher item <");
		StrNCat( addrbuf, fGopher->GetName(), 255);
		StrNCat( addrbuf, ">", 255);
		return DSendMailDialog::InstallSubject(super, addrbuf);
	}
	
	
	virtual DView* InstallTo(DView* super, char* toStr)  //override
	{
		char  *ep, *addr, addrbuf[256];
		StrNCpy( addrbuf, fGopher->fAdminEmail, 255);
		addr= StrChr( addrbuf, '<');
		if (addr) addr++; else addr= addrbuf;
		ep= StrChr( addr, '>');
		if (ep) *ep= 0;
		return DSendMailDialog::InstallTo(super, addr);
	}
	
};



void DGopherPup::OpenHelp( DFile* aFile)
{
	if (!aFile || !aFile->Exists()) {
		Message(MSG_OK,"Can't find help file '%s'",aFile->GetName());
		return;
		}
	fGopherDocSuffix= ".goh";
	this->OpenDocument( aFile); 
	fGopherDocSuffix= DGopherListDoc::kGopherDocSuffix;
	//DApplication::OpenHelp(aFile);
}



class DLocalDialog : public DWindow
{
public:
	enum { kSetDefault = 13345 };
	DEditText* fQuery;
	DLocalDialog();			
	char* Result();
	void OkayAction();
};


DLocalDialog::DLocalDialog() :
	DWindow( 0, gApplication, fixed, -10, -10, -50, -20, "Local Gopher", kDontFreeOnClose)
{
		new DPrompt(0, this, "Open local folder?", 0, 0, Nlm_systemFont);	 		
		this->NextSubviewBelowLeft();
		fQuery= new DEditText(0, this, gLocalGopherRoot, 28); 
		this->SetEditText(fQuery);
		this->NextSubviewBelowLeft();
		(void) new DCheckBox(kSetDefault,this,"Make this default local services folder");
		this->AddOkayCancelButtons();
}
			
void DLocalDialog::OkayAction() 
{ 
	DWindow::OkayAction();
	DCheckBox* ck = (DCheckBox*) this->FindSubview(kSetDefault);
	if (ck && ck->GetStatus()) {
		char* newroot= fQuery->GetText(); // this dups text
		if (newroot) {
			MemFree(gLocalGopherRoot);
			gLocalGopherRoot= newroot;
			}
		}
}

char* DLocalDialog::Result()
{
	if (fOkay) return fQuery->GetText();
	else return NULL;
}


void DGopherPup::OpenLocalGopher()
{
	DLocalDialog* win= new DLocalDialog();
	if ( win->PoseModally() ) {
		char* localpath= win->Result();
	  DGopher* localgo= DGopherList::LocalGopher( localpath);
		MemFree( localpath);
	  DGopherListDoc::ProcessGopher( localgo, 0);
	  }
	delete win;  
}
		
 

void DGopherPup::OpenDocument(DFile* aFile)
{
	if (!aFile || !aFile->Exists()) return;
	gCursor->watch();
#if 1
	aFile->Close();
  DGopher* localgo= DGopherList::LocalGopher( aFile);
  DGopherListDoc::ProcessGopher( localgo, 0);
#else
	if (DGopherListDoc::IsGopherDoc(aFile)) {
		DGopherListDoc::ProcessFile( aFile);
		}
	else {
		DFile* bFile= NULL;
		char	name[512];
		StrNCpy(name, aFile->GetName(), 512);
		gFileManager->ReplaceSuffix(name, 512, fGopherDocSuffix);
		if (gFileManager->FileExists( name))  
			bFile= new DFile(name, "r");
		if ( gGopherTextDoc 
		 && ((DGoLinkedTextDoc*)gGopherTextDoc)->PushNewLocalDoc(aFile, bFile) ) 
			;
		else {
			DGoLinkedTextDoc* tdoc= 
				new DGoLinkedTextDoc( DGoLinkedTextDoc::kGoLitextdoc, NULL, NULL);  
			tdoc->Open(aFile, bFile);   
			}
		}
#endif
	gCursor->arrow();
}


		
Boolean DGopherPup::IsInternetMenu(DTaskMaster* action) 
{
	DWindow* win;
	short 	iview = 0;

	switch(action->Id()) {
				
		case kViewPrefs: 
			if (!gGoviewPrefs) {
				gGoviewPrefs = new DGoviewPrefs();
				gGoviewPrefs->Initialize();
				}
			if (gGoviewPrefs && gGoviewPrefs->PoseModally()) ;
			//delete gGoviewPrefs; // leave the window hidden but alive for later use
			return true;
			
		case kOtherPrefs:
			if (!gGoOtherPrefs) {
				gGoOtherPrefs = new DGoOtherPrefs();
				gGoOtherPrefs->Initialize();
				}
			if (gGoOtherPrefs && gGoOtherPrefs->PoseModally()) ;
			//delete gGoOtherPrefs; // leave the window hidden but alive for later use
			return true;

		case kEditGopherMap: 
			EditGopherMap(); 
			return true;
			
		case 	kOpenDoc:
			//GopherDoc(); return true;
			{
			DTaskMaster* newact= new DTaskMaster(DApplication::kOpen, this);
			Boolean result= DApplication::IsMyAction( newact);
			delete newact;
			return result;
			}
			
		case  kFetchOptionFlag:
			if (fFetchOptionMenuItem) DGopherListDoc::gOptionIsOn= fFetchOptionMenuItem->GetStatus();
			else DGopherListDoc::gOptionIsOn = !DGopherListDoc::gOptionIsOn; // or read menu state
			return true;

		case  kAutoloadFlag:
			if (fAutoloadItem) DGoLinkedTextDoc::gAutoloadInlines= fAutoloadItem->GetStatus();
			else DGoLinkedTextDoc::gAutoloadInlines = !DGoLinkedTextDoc::gAutoloadInlines; // or read menu state
			return true;


		case  kExtractGodoc:
			{
			win= gWindowManager->CurrentWindow();
			if (win && win->Id() == DGoLinkedTextDoc::kGoLitextdoc) {
				DGoLinkedTextDoc* ndoc= (DGoLinkedTextDoc *) win;
				switch (action->Id()) {
					case kExtractGodoc	: ndoc->ExtractGopherDoc(); break;
					}
				}
			return true;
			}


		case  kLockWindow:
			win= gWindowManager->CurrentWindow();
			if (win && fLockWinMenuItem) {
				if (win->Id() == DGopherListDoc::kGoListdoc)
 					((DGopherListDoc *) win)->PinWindow( fLockWinMenuItem->GetStatus());
				else if (win->Id() == DGopherTextDoc::kGoTextdoc   
							|| win->Id() == DGoLinkedTextDoc::kGoLitextdoc)  
					((DGopherTextDoc *) win)->PinWindow( fLockWinMenuItem->GetStatus());
				}		 
			return true;

		case 	kGetInfo:
		case  kFetchLink:
		case  kMailAdmin:
		case	kFindServer:
		case	kNewServer:
		case	kNewLink:
		case  kEditLink:
			{
			win= gWindowManager->CurrentWindow();
			if (win && win->Id() == DGopherListDoc::kGoListdoc) {
				DGopher* go;
				DGopherListDoc* gdoc= (DGopherListDoc *) win;
				switch (action->Id()) {

					case kFindServer:	gdoc->TopOfGopherHole(); break;
					case kNewServer	: gdoc->InsertNewGopher( false); break;
					case kNewLink		: gdoc->InsertNewGopher( true); break;
					case kEditLink	: gdoc->EditOldGopher(); break;
					case kGetInfo:
						iview= DGopher::kGetItemInfo;
							// fall thru to kFetchLink
					case kFetchLink:
						go= gdoc->fGoview->SelectedGopher();
						if (go) gdoc->ProcessGopher(go, iview);
						break;
						
					case kMailAdmin	:
						go= gdoc->fGoview->SelectedGopher();
						if (go && go->fAdminEmail) {
							win= new DAdminMailDialog(0, this, go);
							win->Open();
							}
						break;
					}
				}
			return true;
			}
 
 
		case	kMailSetup:
			{
			win= new DMailSetupDialog(0, this, -10, -10, -20, -20, "Mail Setup");
			win->Open();
			return true;
			}
		case 	kSendMail: 
			{
			win= new DSendMailDialog(0, this, -10, -10, -20, -20, "Send Mail");
			win->Open();
			return true;
			}
		case  kBugMail:
			{
			win= new DBugMailDialog(0, this, -10, -10, -20, -20, "Comment about this App");
			win->Open();
			return true;
			}
		default:	return false;
		}
}


Boolean DGopherPup::IsMyAction(DTaskMaster* action) 
{
	DWindow* win;
	DGopherListDoc* gdoc = (DGopherListDoc*) gWindowManager->CurrentWindow();
	
	if (action->fSuperior)  
		switch (action->fSuperior->Id()) {

			case cFileMenu	: 
			  switch (action->Id()) {
				case kOpenLocalGo:
					OpenLocalGopher();
					return true;
				case kNewServer:
					return IsInternetMenu(action);
				case DApplication::kNew:
					DGopherListDoc::NewGopherDoc();
					return true;
 				default: 
					return DApplication::IsMyAction(action);
				}
				break;

			case kInternetMenu	:
			case kPrefsMenu : 
				// this is here and below due to BOBS_WIN_MAC / win_other problem
				if (action->Id() == DGopherListDoc::kViewChoiceMenu) 
					goto caseViewChoiceMenu; 
				else
					return IsInternetMenu(action);
				
			case DGopherListDoc::kViewKindMenu	: 
				if (gdoc && gdoc->Id() == DGopherListDoc::kGoListdoc)
					return gdoc->IsMyAction(action);
					else return false;
				
			case DGopherListDoc::kViewChoiceMenu	: 
			caseViewChoiceMenu:
				if (gdoc && gdoc->Id() == DGopherListDoc::kGoListdoc)
					return gdoc->IsMyAction(action);
					else return false;
			}
	 
	 
	switch(action->Id()) {

		case DApplication::kNew:
			DGopherListDoc::NewGopherDoc();
			return true;

		case cOKAY:
		case cCANC:
			/// Note:  These Button OKAY/CANC messages are not necessarily from just one window --
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



void DGopherPup::UpdateMenus(void)
{
	DApplication::UpdateMenus();
	
	//gViewCentral->EnableView(kSpecial1);
	//gViewCentral->EnableView(kSpecial2);
}


