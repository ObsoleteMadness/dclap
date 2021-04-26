// SeqPup.cp
// d.g.gilbert -- biosequence editor & analysis platform
// Jan 1994

#define UseGopher  0

#include <DClap.h>
#if UseGopher
#  include <DNet.h>
#else
#  include <DTCP.h>
#  include <DSMTPclient.h>
#  include <DSendMailDialog.h>
#endif
#include <DBio.h>
#include <DRich.h>
#include <DDrawPICT.h>
#include <DSeqPict.h>
#include <Dversion.h>

void AboutSeqPupPicture(); // DAboutSeqPup.cpp



class DSeqPup : public DApplication
{
public:
	static char* kBugmailAddress;
	static char* kGopherRTFDocSuffix;
	
	enum appMenus { kInternetMenu = 100, kPrefsMenu, kOpenMenu };  
	enum menuCmds { 
		kSeqPrefs=1001,  kOtherPrefs, kEditGopherMap,
		kViewByDefault,kViewByDate,kViewBySize,kViewByName,kViewByKind,kViewByHost,
		kViewAsText, kViewAsLink,
		kOpenDoc, kFetchLink, kFetchDefault, // kFetchView1...kFetchViewN
		kFetchOptionFlag, kAutoloadFlag,
		kViewPrefs, kCodonPref,kREnzymePref,kColorPref,kStylePref,
		kSaveWinPrefs,kSeqPrintPrefs, kSeqDocPrefs,
		kGetInfo, kMailAdmin, kOpenLocalGo,
		kNewServer,kFindServer,kNewLink, kEditLink,kExtractGodoc,kLockWindow,
		kOpenText, kOpenSeq, kOpenAppendSeq, kOpenTree,  
		kMailSetup, kSendMail, kBugMail
		};

	DView *	fFetchOptionMenuItem;
	DView *	fLockWinMenuItem;
	DView * fAutoloadItem;
	DMenu * fViewsMenu;
	Boolean fWinPrefsNotSaved, fAppendSeq;
	
	void ISeqPup();	 
	virtual ~DSeqPup();
	void DoAboutBox();

	virtual void SetUpMenus();	
	virtual Boolean DoMenuTask(long tasknum, DTask* theTask);
	virtual	Boolean IsMyAction(DTaskMaster* action);
	virtual Boolean IsInternetMenu(DTaskMaster* action); 
	virtual	void UpdateMenus(); 
	virtual void OpenDocument(DFile* aFile);
	virtual void OpenLocalGopher();
	DWindow* DoMakeDocument(short itsCommandNumber, DFile* aFile);
};


char* DSeqPup::kBugmailAddress= "seqpup@bio.indiana.edu";
char* DSeqPup::kGopherRTFDocSuffix = ".start";


	// here we go -- the Nlm_Main() program (called from main() in ncbimain.c)
extern "C" short Main(void)
{
  DSeqPup* app = new DSeqPup();
  app->ISeqPup();
  app->Run();
  delete app; 
  return 0;
}



void DSeqPup::ISeqPup()
{
	DApplication::kName= "SeqPup";
	DApplication::kVersion= (char*) kDCLAPVersionString; //" v 0.4";
	
	fFetchOptionMenuItem= NULL;
	fLockWinMenuItem= NULL;
	fWinPrefsNotSaved= true;
	fAppendSeq= false;
	fAutoloadItem = NULL;
#if UseGopher
	gGopherDoc = NULL; // ??? this should be set, but isn't in 68K ??
#endif
	fViewsMenu= NULL;
	fFileSuffix = NULL; // unix, msdos file suffix
	fAcceptableFileTypes = NULL; 	// really want "IGo4 BKMK TEXT" and perhaps others
																// but NCBI tools only handle 1 type now
	IApplication();
	
#if UseGopher
		// network doc initializations
	InitializeUGopher();
	DGoviewPrefs::InitGlobals();
	DGoOtherPrefs::InitGlobals();
	if (fFetchOptionMenuItem) fFetchOptionMenuItem->SetStatus(DGopherListDoc::gOptionIsOn);
	if (fAutoloadItem) fAutoloadItem->SetStatus(DGoLinkedTextDoc::gAutoloadInlines);
#endif

	DRichView::InitRichView();
	
		// sequence initializations
	DSeqDoc::GetGlobals(); //SeqDocPrefs(kSeqDocPrefInit);
	DCodons::Initialize("codon","data","tables:codon.table");
	DREMap::Initialize("renzyme","data","tables:renzyme.table");
	DBaseColors::Initialize("color","data","tables:color.table");
	if (DBaseColors::NotAvailable()) ; // read in DBaseColors data now 
	DStyleTable::Initialize("seqmasks","data","tables:seqmasks.table");
	if (DStyleTable::NotAvailable()) ; // read in DStyleTable data now 
	
		// default documents ??
	if (DSeqDoc::fgStartDoc) DSeqDoc::NewSeqDoc();
	{
		// open default gopher document if it exists.
		// TRY RTF Form first (.start ?? )
		// open both default.go4 and default.start files, if they exist !
	char godefault[512];
#if UseGopher
	StrNCpy(godefault, (char*)Shortname(), 512);	
	StrNCat(godefault, DGopherListDoc::kGopherDocSuffix, 512);
	DApplication::OpenDocument( (char*)godefault);
#endif
	StrNCpy(godefault, (char*)Shortname(), 512);	
	StrNCat(godefault, kGopherRTFDocSuffix, 512);
	DApplication::OpenDocument( (char*)godefault);
	}

}	 


DSeqPup::~DSeqPup()
{
#if UseGopher
	DGoviewPrefs::SaveGlobals();
	DGoOtherPrefs::SaveGlobals();
#endif
	if (fWinPrefsNotSaved) DSeqDoc::SaveGlobals(); 
	DDrawPict::Cleanup();
}


void DSeqPup::DoAboutBox()
{
#if !defined(WIN_MSWIN) && !defined(WIN_MOTIF)
		// problems yet w/ this large codepic in mswin(WIN16) and unix/motif
	AboutSeqPupPicture();
#else
	char aboutMe[512];
	sprintf( aboutMe, "SeqPup, version %s"LINEEND
				" a biosequence editor & analysis application "LINEEND
				" copyright by D.G. Gilbert, 1990-1995"
				, DApplication::kVersion);
	DAboutBoxWindow* about = new DAboutBoxWindow(aboutMe);
#endif
}


void DSeqPup::SetUpMenus(void)
{
	//DApplication::SetUpMenus();

	DMenu* aMenu = NULL;
#ifdef WIN_MAC
	DAppleMenu* appleMenu = new DAppleMenu(this,fAboutLine);
#endif
	if (!aMenu) aMenu = this->NewMenu( cFileMenu, "File");
#ifndef WIN_MAC
	aMenu->AddItem(kAbout, fAboutLine);
	aMenu->AddSeparator();
#endif
	aMenu->AddItem(kNew,"New/N", false, true);

	aMenu->AddItem( kOpenSeq, "Open sequence.../O",false, true);
	//aMenu->AddItem(kOpen,"Open.../O",false, true);
	DMenu* oMenu= new DSubMenu( kOpenMenu, aMenu, "Open other");
	//oMenu->AddItem( kOpenSeq, "Sequence file.../O",false, true);
	oMenu->AddItem( kOpenText, "Text,RTF,Image...",false, true);
	oMenu->AddItem( kOpenAppendSeq, "Append sequence...",false, true);
	//oMenu->AddItem( kOpenTree, "Phylogenetic tree...",false, true);
	//oMenu->AddItem( kOpenLocalGo,"Open Local gopher...",false, true);
	//oMenu->AddItem( kNewServer,"Open Remote.../R",false, true);

	aMenu->AddSeparator();
	aMenu->AddItem(kClose,"Close/W",false, true);
	aMenu->AddItem(kSave,"Save/S",false, true);
	aMenu->AddItem(kSaveAs,"Save As...",false, true);
	aMenu->AddItem(DSeqDoc::cSaveSel,"Save selection...",false, true);
	aMenu->AddSeparator();
	aMenu->AddItem(kPrint,"Print",false, true);
	aMenu->AddItem(kHelp,"Help/H",false, true);
	aMenu->AddSeparator();
	aMenu->AddItem(kQuit,"Quit/Q",false, true);

	aMenu= NULL;
	this->SetUpMenu(DApplication::cEditMenu, aMenu);
	DSeqDoc::SetUpMenu(DApplication::cEditMenu, aMenu);

	DMenu* sMenu = NULL;
	DSeqDoc::SetUpMenu(DSeqDoc::kSeqMenu, sMenu);

	DMenu* iMenu = NewMenu( kInternetMenu, "Internet");
	//iMenu->AddItem( kMailSetup, "Mail setup...");
 	//iMenu->AddSeparator();		

	DSeqDoc::SetUpMenu(DSeqDoc::kInternetMenu, iMenu);
 	
	iMenu->AddItem( kSendMail, "Send Mail...");
	iMenu->AddItem( kBugMail, "Comment on this app...");

	DMenu* cMenu = NULL;
	DSeqApps::SetUpMenu(DSeqApps::kChildMenu, cMenu);
	
	iMenu = NewMenu( kPrefsMenu, "Options");
	iMenu->AddItem( kSeqDocPrefs, "Seq Prefs...",false, true);
	iMenu->AddItem( kSeqPrintPrefs, "SeqPrint Prefs...", false, true); 
	iMenu->AddItem( kColorPref, "Base color table...", false, true);
	iMenu->AddItem( kStylePref, "Base style table...", false, true);
	iMenu->AddItem( kCodonPref, "Codon table...", false, true);
	iMenu->AddItem( kREnzymePref, "R.Enzyme table...", false, true);
	iMenu->AddItem( kSaveWinPrefs, "Save window settings", false, true);
	// add opt to save current aln window size,text-lock,color-state to prefs ?
	// or do automatically on close?
	iMenu->AddSeparator();
	
#if UseGopher
	iMenu->AddItem( kViewPrefs, "View Prefs...", false, true); 
#if NETSTUF_LATER
	iMenu->AddItem( kFetchOptionFlag, "Fetch with options", true, true);
		// we can only access menu item thru gViewCentral now the way DMenu::AddItem() is written...
	fFetchOptionMenuItem= (DView*) gViewCentral->GetView(kFetchOptionFlag);

	iMenu->AddItem( kAutoloadFlag, "Autoload images", true, true);
	fAutoloadItem= (DView*) gViewCentral->GetView(kAutoloadFlag);
	iMenu->AddItem( kEditGopherMap, "Edit view handlers...",false, true);
	iMenu->AddItem( kOtherPrefs, 	"Other prefs...",false, true);
#endif
#endif
	iMenu->AddItem( kMailSetup, 	"Mail setup...",false, true);

	aMenu= NULL;
	this->SetUpMenu(DApplication::cWindowMenu, aMenu);
	
	this->SetUpMenu( 0, aMenu);
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
		return DSendMailDialog::InstallTo(super, DSeqPup::kBugmailAddress);
	}
};



#if UseGopher
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
#endif


void DSeqPup::OpenLocalGopher()
{
#if UseGopher
	DLocalDialog* win= new DLocalDialog();
	if ( win->PoseModally() ) {
		char* localpath= win->Result();
	  DGopher* localgo= DGopherList::LocalGopher( localpath);
		MemFree( localpath);
	  DGopherListDoc::ProcessGopher( localgo, 0);
	  }
	delete win;  
#endif
}
		
 
// DEBUG TEST of DChildDlogDoc
#include "DChildDlogDoc.h"

void DSeqPup::OpenDocument(DFile* aFile)
{
	if (!aFile || !aFile->Exists()) return;
	gCursor->watch();
	if (DSeqDoc::IsSeqFile(aFile)) {
		if (fAppendSeq) {
			DSeqDoc* sdoc= (DSeqDoc*) gWindowManager->CurrentWindow();
			if (sdoc && sdoc->Id() == DSeqDoc::kSeqdoc) 
				sdoc->Open(aFile);   
			else
				fAppendSeq= false;
			}
		if (!fAppendSeq) {
			DSeqDoc* sdoc= new DSeqDoc(DSeqDoc::kSeqdoc, NULL, NULL);
			sdoc->Open(aFile);   
			}
		}
		
	else {
		aFile->Close();
#if UseGopher
	  DGopher* localgo= DGopherList::LocalGopher( aFile);
	  DGopherListDoc::ProcessGopher( localgo, 0);
#else
				// DEBUG TEST of DChildDlogDoc ...
		//DChildDlogDoc* doc= new DChildDlogDoc( 0, gTextFont);
		DRichTextDoc* doc= new DRichTextDoc( 0, true, gTextFont);
		doc->Open( aFile);
#endif
		}
	gCursor->arrow();
}



DWindow* DSeqPup::DoMakeDocument(short itsCommandNumber, DFile* aFile)
{
	DWindow* theDoc= NULL;
	
 	switch (itsCommandNumber) {
#if 0
		case cOpenNAColors:
		case cOpenAAColors: {
			DColorPrefDocument* aDoc= new DColorPrefDocument();
			if (itsCommandNumber == cOpenAAColors)
				aDoc->IColorPrefDocument( itsFile, kAmino);
			else 
				aDoc->IColorPrefDocument( itsFile, kDNA);
			return aDoc;
			}

		case cOpenTree: {
			DTreeDoc* aDoc= new DTreeDoc();
			aDoc->ITreePrintDocument(kPrintClipType, NULL, NIL, itsFile);
			return aDoc;
			}
		
		// also handle gopher documents ... some gopher method should do this
		
		default:  { //== cNewProj/cOpenProj, cNew/cOpen
			DAlnDoc* aDoc= new DAlnDoc();
			aDoc->IAlnDoc( itsFile);
			return aDoc;
			}
#endif
		}
		
	return theDoc;
}





		
Boolean DSeqPup::IsInternetMenu(DTaskMaster* action) 
{
	DWindow* win;
	DSeqDoc* sdoc = (DSeqDoc*) gWindowManager->CurrentWindow();

	switch(action->Id()) {

		case kCodonPref: 
			DCodons::TableChoice();
			return true;

		case kColorPref: 
			DBaseColors::TableChoice();
			return true;

		case kStylePref: 
			DStyleTable::TableChoice();
			return true;
			
		case kREnzymePref: 
			DREMap::TableChoice();
			return true;
			
		case kSaveWinPrefs: 
			if (sdoc && sdoc->Id() == DSeqDoc::kSeqdoc) {
				sdoc->MakeGlobalsCurrent();
				DSeqDoc::SaveGlobals();
				fWinPrefsNotSaved= false;
				}
			return true;

		case kSeqDocPrefs: 
			SeqDocPrefs(kSeqDocPrefDialog);
			return true;
	
		case kSeqPrintPrefs: 
			SeqPrintPrefs(kSeqPrintPrefDialog);
			return true;

#if UseGopher
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
#endif

#if UseGopher
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
				short iview;
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
#if 0					
					case kMailAdmin	:
						go= gdoc->fGoview->SelectedGopher();
						if (go && go->fAdminEmail) {
							win= new DAdminMailDialog(0, this, go);
							win->Open();
							}
						break;
#endif
					}
				}
			return true;
			}
#endif
 
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
			
		default:	
			if (sdoc && sdoc->Id() == DSeqDoc::kSeqdoc) 
				return sdoc->IsMyAction(action);
			else return false;
		}
}


Boolean DSeqPup::DoMenuTask(long tasknum, DTask* theTask)
{
	switch (tasknum) {
	
		case kPrintAFile:	
		case kOpenAFile:
			DSeqDoc::fgTestSeqFile= true;  
			(void) DApplication::DoMenuTask(tasknum,theTask);
			DSeqDoc::fgTestSeqFile= false;
			return true;
			
		default: 
			return DApplication::DoMenuTask(tasknum, theTask);
		}
}


Boolean DSeqPup::IsMyAction(DTaskMaster* action) 
{
	enum doctype { nodoc, anydoc, godoc, seq1doc, alndoc };
	doctype	thedoc = nodoc;
	Boolean	done= false;
	
	DWindow* win 	= gWindowManager->CurrentWindow();
	DSeqDoc* sdoc = (DSeqDoc*) win;
	DSeqedWindow* s1doc= (DSeqedWindow*) win;
#if UseGopher
	DGopherListDoc* gdoc = (DGopherListDoc*) win;
#endif
	if (win) {
		if (win->Id() == DSeqDoc::kSeqdoc) thedoc = alndoc;
		else if (win->Id() == DSeqedWindow::kId) thedoc = seq1doc;
#if UseGopher
		else if (win->Id() == DGopherListDoc::kGoListdoc) thedoc = godoc;
#endif
		else thedoc= anydoc;
		}
		
	short	menuitem = action->Id();
		  
	if (action->fSuperior)  
		switch (action->fSuperior->Id()) {

			case kOpenMenu : 
			case cFileMenu : 
			  switch (menuitem) {
			  
				case kNewServer:
					return IsInternetMenu(action);
					
				case DSeqDoc::cSaveSel:
					goto caseSeqDoc;
					
				case DApplication::kNew:
					DSeqDoc::NewSeqDoc();
					return true;
					
				case kOpenText:
					DApplication::DoMenuTask(DApplication::kOpen, NULL);
					return true;
					
				case kOpenTree:
					return true;

				case kOpenAppendSeq:
					fAppendSeq= true;
				case kOpenSeq:
				case DApplication::kOpen:
					DSeqDoc::fgTestSeqFile= true; // ???
					DApplication::DoMenuTask(DApplication::kOpen,NULL);
					DSeqDoc::fgTestSeqFile= false;
					fAppendSeq= false;
					return true;
					
#if UseGopher
				case kOpenLocalGo:
					OpenLocalGopher();
					return true;
#endif
 				default: 
					return DApplication::IsMyAction(action);
				}
				break;
		
		
			case DApplication::cEditMenu:
#if 1
				if (thedoc == alndoc) done= sdoc->IsMyAction(action);
				else if (thedoc == seq1doc) done= s1doc->IsMyAction(action);
				else done= false;
				if (done) return true;
#else
				if (menuitem == DSeqDoc::cFindORF) {
					if (thedoc == alndoc) return sdoc->IsMyAction(action);
					else if (thedoc == seq1doc) return s1doc->IsMyAction(action);
					}
#endif
				break;
				
			case kInternetMenu	:
			case kPrefsMenu : 
#if UseGopher
				if (menuitem == DGopherListDoc::kViewChoiceMenu) 
					goto caseViewChoiceMenu; 
				else
#endif
				  return IsInternetMenu(action);
				
#if UseGopher
			case DGopherListDoc::kViewKindMenu	: 
				if (thedoc == godoc) return gdoc->IsMyAction(action);
				else return false;
				
			case DGopherListDoc::kViewChoiceMenu	: 
			caseViewChoiceMenu:
				if (thedoc == godoc) return gdoc->IsMyAction(action);
				else return false;
#endif
			
			case DSeqApps::kChildMenu :
				if (thedoc == alndoc) return sdoc->IsMyAction(action);
				else if (thedoc == seq1doc) return s1doc->IsMyAction(action);
				else {
					DSeqApps::CallChildApp( menuitem, NULL);
					return true;
					}

			case DSeqDoc::kSeqMenu	: 
			case DSeqDoc::kViewKindMenu	: 
			case DSeqDoc::kSeqMaskMenu	: 
			caseSeqDoc:
				switch(menuitem) {
					case DSeqDoc::cNAcodes:	NucCodesPicture(); return true;
					case DSeqDoc::cAAcodes: AminoCodesPicture(); return true;
					}
				if (thedoc == alndoc) return sdoc->IsMyAction(action);
				else if (thedoc == seq1doc) return s1doc->IsMyAction(action);
				else return false;
			}
	 
	 
	switch(menuitem) {

		case DApplication::kNew:
			DSeqDoc::NewSeqDoc();
			return true;
		default: 
			return DApplication::IsMyAction(action);
		}
}



void DSeqPup::UpdateMenus(void)
{
	DApplication::UpdateMenus();
	
	//gViewCentral->EnableView(kSpecial1);
	//gViewCentral->EnableView(kSpecial2);
}


