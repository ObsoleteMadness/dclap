// DGopherPrefs.cp
// d.g.gilbert




#include "DGoPrefs.h"

#include <ncbi.h>
#include <dgg.h>
#include <Dvibrant.h>
#include <DApplication.h>
#include <DGoDoc.h>
#include <DGoNetDoc.h>
#include <DGopher.h>
//#include <DGoList.h>
#include <DGoInit.h>
//#include <DGoPlus.h>
#include <DGoClasses.h>
//#include <DPanel.h>
#include <DControl.h>
#include <DDialogText.h>
#include <DWindow.h>
#include <DMenu.h>
#include <DFile.h>
#include <DUtil.h>
#include <DIconLib.h>  
#include <stdio.h>

 	// some public globals -- do these need to be public? -- yes to share w/ DGopherDoc
 	
Global DGoviewPrefs* gGoviewPrefs = NULL;
Global DGoOtherPrefs* gGoOtherPrefs = NULL;

Global	Boolean gShowKind = true, gShowPath = true, gShowHost = true, gShowPort = true, 
				gShowDate = true, gShowSize = true, gShowPlus = true, gShowAdmin = false, 
				gShowURL= false, gShowAbstract = true, gShowExtras = true, 
				gAllowLinkEdits = true, gWantFonts = false, gSameWindow = false;
 
Global Nlm_FonT gGoviewFont = NULL; //Nlm_programFont;
Global short  gGoviewFontSize = 12;
Global char	*gGoviewFontName = NULL;
Global Nlm_Boolean gDoubleClicker = true;
Global Nlm_Boolean gSingleClicker = true;


#if 0
Nlm_FonT     Nlm_GetFont PROTO((Nlm_CharPtr name, Nlm_Int2 size, Nlm_Boolean bld, Nlm_Boolean itlc, Nlm_Boolean undrln, Nlm_CharPtr fmly));
Nlm_FonT     Nlm_ParseFont PROTO((Nlm_CharPtr spec));
void         Nlm_SelectFont PROTO((Nlm_FonT f));
Font Families that vibrant knows:
        if (Nlm_StringICmp (fmly, "Roman") == 0) {
        } else if (Nlm_StringICmp (fmly, "Swiss") == 0) {
        } else if (Nlm_StringICmp (fmly, "Modern") == 0) {
        } else if (Nlm_StringICmp (fmly, "Script") == 0) {
        } else if (Nlm_StringICmp (fmly, "Decorative") == 0) {
#endif




//class DGoviewPrefs : public DWindow 

 
enum DGoviewPrefCmds {
cIfsz,cTfsz,cTtab,citem,cicoc,cicob,cIsi0,cIsi1,cIsi2,cicos,
cIsz1,cIsz2,cIsz3,cattr,
cDate,cSize,cKind,cPath,cHost,cPort,cURL,cAbstract,cAdmin,
ccfnt,cIfnt,ctext,cTfnt
};

 
class DFontSizeControl : public DSwitchBox
{
public:
	DGoviewPrefs*	fDoc;
	
	DFontSizeControl(long id, DView* super, DGoviewPrefs* itsDoc) :
		DSwitchBox( id, super, true, true),
		fDoc(itsDoc)
		{
		}
		
	virtual void Switch(short newval, short oldval)
	{
		switch (Id()) {
			case cIfsz:
					gGoviewFontSize= newval;
					fDoc->fNeedSave= true;
					break;
			case cTfsz:
					gTextFontSize= newval;
					fDoc->fNeedSave= true;
					break;
			case cTtab:
					gTextTabStops= newval;
					fDoc->fNeedSave= true;
					break;
			}
	}
};

 
 
DGoviewPrefs::DGoviewPrefs() :
	DWindow( 0, NULL, DWindow::fixed, -10, -10, -50, -20, "View prefs", kDontFreeOnClose),
	fGoFontMenu(NULL),
	fTextFontMenu(NULL),
	fNeedSave(false),
	fItemGroup(NULL),
	fTextGroup(NULL)
{	
}

DGoviewPrefs::~DGoviewPrefs()
{
}


// static
void DGoviewPrefs::InitGlobals() 
{ 
	gGoviewFontName= gApplication->GetPref( "gGoviewFontName", "fonts", gGoviewFontName);
	gGoviewFontSize= gApplication->GetPrefVal( "gGoviewFontSize", "fonts", "12");
	gGoviewFont= Nlm_GetFont( gGoviewFontName, gGoviewFontSize, false, false, false, NULL);

	if (!gTextFont || !gTextFontName) {
				// DApplication::IApplication() now reads these ! 
		gTextFontName= gApplication->GetPref( "gTextFontName", "fonts", gTextFontName);
		gTextFontSize= gApplication->GetPrefVal( "gTextFontSize", "fonts", "12");
		gTextTabStops= gApplication->GetPrefVal( "gTextTabStops", "fonts", "4");
		gTextFont  = Nlm_GetFont( gTextFontName, gTextFontSize, false, false, false, NULL);
		}
	
	gIconSize= gApplication->GetPrefVal( "gIconSize", "goview","2");
	gShowDate= gApplication->GetPrefVal( "gShowDate", "goview","1");
	gShowSize= gApplication->GetPrefVal( "gShowSize", "goview","1");
	gShowKind= gApplication->GetPrefVal( "gShowKind", "goview");
	gShowSize= gApplication->GetPrefVal( "gShowSize", "goview");
	gShowPath= gApplication->GetPrefVal( "gShowPath", "goview");
	gShowHost= gApplication->GetPrefVal( "gShowHost", "goview");
	gShowPort= gApplication->GetPrefVal( "gShowPort", "goview");
	gShowURL= gApplication->GetPrefVal( "gShowURL", "goview","0");
	gShowAbstract= gApplication->GetPrefVal( "gShowAbstract", "goview","1");
	gShowAdmin= gApplication->GetPrefVal( "gShowAdmin", "goview");
	gShowExtras= (gShowURL||gShowDate||gShowSize||gShowHost||gShowPort
							||gShowPath||gShowAdmin);

#ifndef NOTNOW_WIN_MOTIF
	// gcc/motif bombs here w/ bus error
	// deal with windowrect data also? -- gGopherListDocRect
	{
	char* srect = gApplication->GetPref( "gGopherListDocRect", "windows", "20 20 450 220");
		// sscanf is failing on Mac/codewar !? used to work
#if 1
	if (srect) {
		char* cp= srect;
		while (*cp && isspace(*cp)) cp++;
		gGopherListDocRect.left= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		gGopherListDocRect.top= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		gGopherListDocRect.right= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		gGopherListDocRect.bottom= atoi( cp);
		}
#else
	if (srect) sscanf( srect, "%d%d%d%d", 
				&gGopherListDocRect.left, &gGopherListDocRect.top, 
				&gGopherListDocRect.right, &gGopherListDocRect.bottom);
#endif
	MemFree(srect);
	}	
#endif
}

void DGoviewPrefs::SaveGlobals() 
{

}

void DGoviewPrefs::Initialize() 
{ 
	// InitGlobals(); // ??
		
				// THIS dialog takes a while to build -- pre-make this dialog at startup??
				// build the dialog items now, before open, so we can keep a hidden window 
				// around all of applications life.
				
	DView* super;
	DEditText* et;
	DPrompt* pr;
	DCheckBox* ck;
	DPopupList* popl;
	DPopupMenu* popm;
	DCluster * cluster, * subcluster;
	DSwitchBox* sw;
	
		// dialog contents:
		// top left -- cluster of 3 iconbuttons over 3 radio buttons, for icon size choice
		// top right -- cluster of 3 rows x 2 cols of check boxes for show attributes
		// bottom -- font popup, fontsize popup + edittext for item font

	fItemGroup= new DCluster(citem, this, 30, 10, false, "Item view");
	super= fItemGroup;
	
		// top left -- cluster of 3 iconbuttons over 3 radio buttons, for icon size choice
	
	cluster= new DCluster(cicoc, super, 15, 5, true, NULL);
	super= cluster;
	if (gGopherIcons) {
		DIcon* ico= NULL;
		DIconButton* isize[3];
		
		subcluster= new DCluster(cicob, super, 15, 5, true, NULL);
		super= subcluster;
		
		ico= gGopherIcons->IconById( kTextIcon+20000);  //tiny icon
		isize[0]= new DIconButton(cIsi0, super, ico);
		super->NextSubviewToRight();
		ico= gGopherIcons->IconById( kTextIcon+10000);  //small icon
		isize[1]= new DIconButton(cIsi1, super, ico);
		super->NextSubviewToRight();
		ico= gGopherIcons->IconById( kTextIcon);  //big icon
		isize[2]= new DIconButton(cIsi2, super, ico);
		
		//isize[gIconSize]->SetStatus(true);
		super= cluster;
		super->NextSubviewBelowLeft();
		}
	
	subcluster= new DCluster(cicos, super, 15, 5, true, NULL);
	super= subcluster;
	DRadioButton* rsize[3];
	rsize[0]= new DRadioButton(cIsz1, super, NULL);
	super->NextSubviewToRight();
	rsize[1]= new DRadioButton(cIsz2, super, NULL);
	super->NextSubviewToRight();
	rsize[2]= new DRadioButton(cIsz3, super, NULL);
	rsize[gIconSize]->SetStatus(true);
	
	
		// top right -- cluster of 3 rows x 2 cols of check boxes for show attributes
	super= fItemGroup;
	super->NextSubviewToRight();
	cluster= new DCluster(cattr, super, 15, 5, false, "Show attributes");
	super= cluster;

	ck= new DCheckBox(cDate, super, "Date");
	ck->SetStatus(gShowDate);
	super->NextSubviewToRight();
	ck= new DCheckBox(cSize, super, "Size");
	ck->SetStatus(gShowSize);   
	super->NextSubviewBelowLeft();

	ck= new DCheckBox(cKind, super, "Kind");
	ck->SetStatus(gShowKind); 
	super->NextSubviewToRight();
	ck= new DCheckBox(cPath, super, "Path");
	ck->SetStatus(gShowPath);   
	super->NextSubviewBelowLeft();

	ck= new DCheckBox(cHost, super, "Host");
	ck->SetStatus(gShowHost);
	super->NextSubviewToRight();
	ck= new DCheckBox(cPort, super, "Port");
	ck->SetStatus(gShowPort);   
	super->NextSubviewBelowLeft();

	ck= new DCheckBox(cURL, super, "URL");
	ck->SetStatus(gShowURL);
	super->NextSubviewToRight();
	ck= new DCheckBox(cAbstract, super, "Abstract");
	ck->SetStatus(gShowAbstract);   
	super->NextSubviewBelowLeft();

	ck= new DCheckBox(cSize, super, "Admin");
	ck->SetStatus(gShowAdmin);   
	super->NextSubviewToRight();
  // another check?
	super->NextSubviewBelowLeft();
	
		// bottom -- font popup, fontsize popup + edittext for item font
	super= fItemGroup;
	super->NextSubviewBelowLeft();
	cluster= new DCluster(ccfnt, super, 15, 5, true, NULL);
	super= cluster;

	popm= new DPopupMenu(cIfnt, (Nlm_GrouP)super->GetNlmObject(), "Font  ");
	fGoFontMenu= popm;
	popm->AddFonts();
	popm->SetFontChoice( gGoviewFontName); 
	super->NextSubviewToRight();
	
	pr= new DPrompt(0, super, " font size", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
	sw = new DFontSizeControl(cIfsz, super, this);
	sw->SetValues(gGoviewFontSize,99);
	
	
		// Text Group -- font, font size
	super= this;
	super->NextSubviewBelowLeft();

	fTextGroup= new DCluster(ctext, this, 30, 10, false, "Text view");
	super= fTextGroup;

	popm= new DPopupMenu(cTfnt, (Nlm_GrouP)super->GetNlmObject(), "Font  ");
	fTextFontMenu= popm;
	popm->AddFonts();
	popm->SetFontChoice( gTextFontName); 
	super->NextSubviewToRight();
	
	cluster= new DCluster(0, super, 30, 10, true, NULL);
	super= cluster;
	
	pr= new DPrompt(0, super, "font size", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
	sw = new DFontSizeControl(cTfsz, super, this);
	sw->SetValues(gTextFontSize,99);
	super->NextSubviewBelowLeft();

	pr= new DPrompt(0, super, "tab stops", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
	sw = new DFontSizeControl(cTtab, super, this);
	sw->SetValues(gTextTabStops,99);

	super= fTextGroup;

	this->AddOkayCancelButtons();
	
}


void DGoviewPrefs::OkayAction() 
{ 
	// convert dialog control values to global values & app static/file prefs
		
	// font check not seen below...
	
	for (short imenu= 0; imenu<2; imenu++) {
		short	 		aSize, nitem, i;
		char			name[256];
		DMenu 	*	aMenu;
		Nlm_FonT	aFont;
		if (imenu) {
			aMenu= fTextFontMenu; 
			aSize= gTextFontSize;
			}
		else {
			aMenu= fGoFontMenu; 
			aSize= gGoviewFontSize;
			}
		if (aMenu && aMenu->GetFontChoice(name, 255)) {
			if (StrCmp(name,"System")==0) aFont= Nlm_systemFont;
			else if (StrCmp(name,"Program")==0) aFont= Nlm_programFont;
			else aFont= Nlm_GetFont( name, aSize, false, false, false, NULL);
			
			if (aMenu == fGoFontMenu && StrCmp(name,gGoviewFontName)!=0 ) {
				if (gGoviewFontName) MemFree(gGoviewFontName);
				gGoviewFontName= StrDup(name);
				gGoviewFont= aFont;
				fNeedSave= true;
				}
			else if (aMenu == fTextFontMenu &&  StrCmp(name,gTextFontName)!=0 ){
				if (gTextFontName) MemFree(gTextFontName);
				gTextFontName= StrDup(name);
				gTextFont= aFont;
				fNeedSave= true;
				}
			}
		}
	
		
	DList* wins= gWindowManager->GetWindowList();
	if (wins) {
		long i, nwin= wins->GetSize();
		for (i= 0; i<nwin; i++) {
			DGopherListDoc* awin= (DGopherListDoc*) wins->At(i);
			if (awin && awin->Id() == DGopherListDoc::kGoListdoc) {
				awin->fGoview->SetTableFont(gGoviewFont);
				awin->fGoview->SetColWidths();
				awin->fGoview->Invalidate();
				//awin->Erase(); awin->Show(); // why doesn't Invalidate() work ??
				}
			}
		}
		

	if (fNeedSave) {
		gApplication->SetPref( gGoviewFontName, "gGoviewFontName", "fonts");
		gApplication->SetPref( gGoviewFontSize, "gGoviewFontSize", "fonts");
		gApplication->SetPref( gTextFontName, "gTextFontName", "fonts");
		gApplication->SetPref( gTextFontSize, "gTextFontSize", "fonts");
		gApplication->SetPref( gTextTabStops, "gTextTabStops", "fonts");
		
		gApplication->SetPref( gIconSize, "gIconSize", "goview");
		gApplication->SetPref( gShowDate, "gShowDate", "goview");
		gApplication->SetPref( gShowSize, "gShowSize", "goview");
		gApplication->SetPref( gShowKind, "gShowKind", "goview");
		gApplication->SetPref( gShowPath, "gShowPath", "goview");
		gApplication->SetPref( gShowHost, "gShowHost", "goview");
		gApplication->SetPref( gShowPort, "gShowPort", "goview");
		gApplication->SetPref( gShowURL, "gShowURL", "goview");
		gApplication->SetPref( gShowAbstract, "gShowAbstract", "goview");
		gApplication->SetPref( gShowAdmin, "gShowAdmin", "goview");

		fNeedSave= false;
		}
}



Boolean DGoviewPrefs::IsMyAction(DTaskMaster* action) 
{	
	DView*		aview= (DView*) action;
	
	switch(action->Id()) {
			// these really should be done at OkayAction !??
		case cicos: gIconSize= aview->GetValue() - 1; break;

		case cIsi0:
		case cIsi1:
		case cIsi2:
			{
			switch(action->Id()) {
				case cIsi0: gIconSize= 0; break;
				case cIsi1: gIconSize= 1; break;
				case cIsi2: gIconSize= 2; break;
				}
			aview= fItemGroup->FindSubview(cicos);
			if (aview) {
				aview->SetValue(gIconSize+1);
				aview->Invalidate();
				}
			break; 
			}
			
		case cDate: gShowDate= aview->GetStatus(); break;
		case cSize: gShowSize= aview->GetStatus(); break;
		case cKind: gShowKind= aview->GetStatus(); break;
		case cPath: gShowPath= aview->GetStatus(); break;
		case cHost: gShowHost= aview->GetStatus(); break;
		case cPort: gShowPort= aview->GetStatus(); break;
		case cURL: gShowURL= aview->GetStatus(); break;
		case cAbstract: gShowAbstract= aview->GetStatus(); break;
		case cAdmin: gShowAdmin= aview->GetStatus(); break;
								
		default : return DWindow::IsMyAction(action);	
		}
		
	gShowExtras= (gShowURL||gShowDate||gShowSize||gShowHost||
								gShowPort||gShowPath||gShowAdmin);
	fNeedSave= true;
	return true;
}


void DGoviewPrefs::Open()
{
	DWindow::Open();
}





// class DGoOtherPrefs : public DWindow 


enum GoOtherPrefsCmds {
	kEditGopherMapHit = 4580,
	kLocalRootHit, kLocalAskWaisHit,
	kFetchSingles,
	kClickerCluster,
	kShortFolderCheck
	};

DGoOtherPrefs::DGoOtherPrefs() :
	DWindow( 0, NULL, DWindow::fixed, -10, -10, -50, -20, "Other prefs", kDontFreeOnClose),
	fNeedSave(false),
	fPlusGroup(NULL),
	fMiscGroup(NULL),
	fMapGroup(NULL),
	fStartGroup(NULL),
	fClickerGroup(NULL),
	fNetGroup(NULL)
{	
}

DGoOtherPrefs::~DGoOtherPrefs()
{
}

void DGoOtherPrefs::OkayAction() 
{ 
	// convert dialog control values to global values & app static/file prefs

	DCheckBox* ck;
	ck= (DCheckBox*) this->FindSubview(kShortFolderCheck);
	if (ck) gShortFolder= ck->GetStatus();

	//DGopherListDoc::gFetchSingles	 
	ck= (DCheckBox*) this->FindSubview(kFetchSingles);
	if (ck) DGopherListDoc::gFetchSingles= ck->GetStatus();

	if (fClickerGroup) {
		switch ( fClickerGroup->GetValue()) {
			default:
			case 1: gDoubleClicker= true; break;
			case 2: gDoubleClicker= false; break;
			}
		gSingleClicker= ! gDoubleClicker;
		}
}

Boolean DGoOtherPrefs::IsMyAction(DTaskMaster* action) 
{	
	char* path;
	switch(action->Id()) {
		case kEditGopherMapHit: 
			EditGopherMap(); //DGoInit.h
			return true;
	
		case kLocalRootHit: 
			// Really need a DFileManager:GetFolderName();
			path= (char*) DFileManager::GetFolderName();
			if (path) gLocalGopherRoot= StrDup( path);
			return true;

		case kLocalAskWaisHit: 
			path= (char*) DFileManager::GetInputFileName(NULL,"APPL");
			if (path) {
				long len= StrLen(gLocalGopherRoot);
				if (StrNCmp( gLocalGopherRoot,path,len)==0) path += len;
				gAskWaisPath= StrDup( path);
				}
			return true;
			 
		}
		
	return DWindow::IsMyAction(action);	
}



//static
void DGoOtherPrefs::InitGlobals()
{
		// need to load in the user prefs & build prefs dialogs
	char* onoffs;

	//gDoSuffix2MacMap= false; // !?? started bombing 26Dec94 w/ this != false

	onoffs= (DGopherListDoc::gFetchSingles) ? "1" : "0";
	DGopherListDoc::gFetchSingles= gApplication->GetPrefVal("gFetchSingles","general", onoffs);
	onoffs= (gShortFolder) ? "1" : "0";
	gShortFolder= gApplication->GetPrefVal("gShortFolder","general", onoffs);
	onoffs= (DGopherListDoc::gOptionIsOn) ? "1" : "0";
	DGopherListDoc::gOptionIsOn= gApplication->GetPrefVal("gOptionIsOn","general", onoffs);
	onoffs= (DGoLinkedTextDoc::gAutoloadInlines) ? "1" : "0";
	DGoLinkedTextDoc::gAutoloadInlines= gApplication->GetPrefVal("gAutoloadInlines","general", onoffs);

	gLocalGopherRoot= gApplication->GetPref( "gLocalGopherRoot", "general", gLocalGopherRoot);
	gAskWaisPath= gApplication->GetPref( "gAskWaisPath", "general", gAskWaisPath);

	onoffs= (gDoubleClicker) ? "1" : "0";
	gDoubleClicker= gApplication->GetPrefVal("gDoubleClicker","general", onoffs);
	gSingleClicker= ! gDoubleClicker;


#ifndef NOTNOW_WIN_MOTIF
	// gcc/motif bombs here w/ bus error
	// deal with windowrect data also? -- gGopherListDocRect
	{
	char* srect = gApplication->GetPref( "gGopherListDocRect", "windows", "20 20 450 220");
		// sscanf is failing on Mac/codewar !? used to work
#if 1
	if (srect) {
		char* cp= srect;
		while (*cp && isspace(*cp)) cp++;
		gGopherListDocRect.left= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		gGopherListDocRect.top= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		gGopherListDocRect.right= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		gGopherListDocRect.bottom= atoi( cp);
		}
#else
	if (srect) sscanf( srect, "%d%d%d%d", 
				&gGopherListDocRect.left, &gGopherListDocRect.top, 
				&gGopherListDocRect.right, &gGopherListDocRect.bottom);
#endif
	MemFree(srect);
	}	

	{
	char* srect = gApplication->GetPref( "gRichDocRect", "windows", "0 0 0 0");
		// sscanf is failing on Mac/codewar !? used to work
	if (srect) {
		char* cp= srect;
		while (*cp && isspace(*cp)) cp++;
		DRichTextDoc::gRichDocRect.left= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		DRichTextDoc::gRichDocRect.top= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		DRichTextDoc::gRichDocRect.right= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		DRichTextDoc::gRichDocRect.bottom= atoi( cp);
		}
	MemFree(srect);
	}	
#endif
}


void DGoOtherPrefs::SaveGlobals()
{
	gApplication->SetPref( (int) gShortFolder,"gShortFolder","general");
	gApplication->SetPref( (int) DGopherListDoc::gFetchSingles, "gFetchSingles","general");
	gApplication->SetPref( (int) DGopherListDoc::gOptionIsOn,"gOptionIsOn","general");
	gApplication->SetPref( (int) DGoLinkedTextDoc::gAutoloadInlines,"gAutoloadInlines","general");
	gApplication->SetPref( gLocalGopherRoot,"gLocalGopherRoot","general");
	gApplication->SetPref( gAskWaisPath,"gAskWaisPath","general");
	gApplication->SetPref( (int) gDoubleClicker,"gDoubleClicker","general");

	if (!Nlm_EmptyRect(&gGopherListDocRect)) {
		char  srect[128];
		sprintf( srect, "%d %d %d %d", 
					gGopherListDocRect.left, gGopherListDocRect.top, 
					gGopherListDocRect.right, gGopherListDocRect.bottom);
		gApplication->SetPref( srect, "gGopherListDocRect", "windows");
		}
		
	if (!Nlm_EmptyRect(&DRichTextDoc::gRichDocRect)) {
		char  srect[128];
		sprintf( srect, "%d %d %d %d", 
					DRichTextDoc::gRichDocRect.left, DRichTextDoc::gRichDocRect.top, 
					DRichTextDoc::gRichDocRect.right, DRichTextDoc::gRichDocRect.bottom);
		gApplication->SetPref( srect, "gRichDocRect", "windows");
		}
}

void DGoOtherPrefs::Initialize()
{
	DEditText* et;
	DPrompt* pr;
	DCheckBox* ck;
	DPopupList* popl;
	DPopupMenu* popm;
	DButton* but;
	DCluster* clu;
	DRadioButton* rb;
	Nlm_FonT	promptFont = Nlm_programFont;
	DView* super = this;
	char   buf[512];
	
		// dialog contents:

	ck= new DCheckBox(kShortFolderCheck,this,"Fetch brief gopher+ folders");
	ck->SetStatus(gShortFolder);
	//this->NextSubviewBelowLeft();
	
	ck= new DCheckBox(kFetchSingles,this,"auto-Fetch from folder with just one item");
	ck->SetStatus(DGopherListDoc::gFetchSingles);

	this->NextSubviewBelowLeft();
	
	but= new DButton(kEditGopherMapHit,this,"Edit Gopher type handlers");
	//this->NextSubviewBelowLeft();
	
	sprintf( buf, "Local services folder is '%s'", gLocalGopherRoot);
	pr= new DPrompt(0, this, buf, 0, 0, promptFont);	 		
	but= new DButton(kLocalRootHit,this,"Select local services folder");
	//this->NextSubviewBelowLeft();

	sprintf(buf, "Local search app is '%s'", gAskWaisPath);
	pr= new DPrompt(0, this, buf, 0, 0, promptFont);	 		
	but= new DButton(kLocalAskWaisHit,this,"Select local search app");
	
	clu= new DCluster(kClickerCluster, super, 15, 5, false, "Mouse-clicks to fetch links");
	fClickerGroup= clu;
	super= clu;
	rb= new DRadioButton( 0, super, "Double click");
	rb->SetStatus(gDoubleClicker); 

	super->NextSubviewToRight();
	rb= new DRadioButton( 0, super, "Single click");
	rb->SetStatus(gSingleClicker);
	super= this;

		//?? include a "Save prefs only on quit..." to save disk thrashing by saveprefs()
	this->NextSubviewBelowLeft(); 
	pr= new DPrompt(0, this, "Watch this space...", 0, 0, promptFont);	 		
		
	this->AddOkayCancelButtons();	
}

void DGoOtherPrefs::Open()
{
	DWindow::Open();
}

