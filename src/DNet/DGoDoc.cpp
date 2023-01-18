// DGopherDoc.cp
// d.g.gilbert




#include "DGoDoc.h"
#include "DGoNetDoc.h"

#include "DGoAskDoc.h"
#include "DGoPrefs.h"

#include <ncbi.h>
#include <dgg.h>
#include <Dvibrant.h>
#include <DApplication.h>
#include <DGopher.h>
#include <DGoList.h>
#include <DGoInit.h>
#include <DGoPlus.h>
#include <DGoClasses.h>
#include <DPanel.h>
#include <DControl.h>
#include <DDialogText.h>
#include <DWindow.h>
#include <DMenu.h>
#include <DFile.h>
#include <DUtil.h>
#include <DIconLib.h> //??
#include <DIcons.h>
#include <DTabSelect.h>
 
 
enum ResizeWindSizes {
	kWinDragHeight = 20,
	kWinScrollWidth = 0,   // added to both vert & hor.
	kWinMinWidth = 0,
	kWinExtraHeight= 0
	};

/* from vibrant/vibwndws.c */
extern "C" void Nlm_ResizeWindow(Nlm_GraphiC w, Nlm_Int2 dragHeight,
                              Nlm_Int2 scrollWidth, Nlm_Int2 minWidth,
                              Nlm_Int2 extraHeight);
/* for doc window: Nlm_ResizeWindow (w, 20, 0, Nlm_StringWidth (title) + 70, 0); */
 
 
Global  Nlm_RecT  gGopherListDocRect = { 0, 0, 0, 0 };
Global  DGopherListDoc* gGopherDoc = NULL;

enum { kGolistViewWidth = 225 };

enum DGoDocCmds {
	cProcessGopher = 30346,
	cStuf,
	cStufInline,
	cSave,
	cAdd,
	cProcessInlines
	};

char* DGopherListDoc::kGopherDocSuffix = ".go4";
 
Local Nlm_FonT	gSmallFont= NULL;

inline void MakeSmallFont()
{
	if (!gSmallFont) 
		gSmallFont = Nlm_GetFont( "Times", 9, false, false, false, "Roman");
}









class DGoDropMenuButton : public DIconButton
{
public:
	DPopupList* fPop;
	
 	DGoDropMenuButton(long id, DView* itsSuperior, DIcon* itsIcon, DPopupList* itsPopup);
	virtual void Click(Nlm_PoinT mouse);
	virtual void Release(Nlm_PoinT mouse);

};

DGoDropMenuButton::DGoDropMenuButton(long id, DView* itsSuperior, 
														DIcon* itsIcon, DPopupList* itsPopup):
		DIconButton( id, itsSuperior, itsIcon),
		fPop(itsPopup)
{
}

void DGoDropMenuButton::Click(Nlm_PoinT mouse)
{
	DIconButton::Click(mouse);
	fPop->Show();
	fPop->Invalidate();
	//Nlm_Beep();
}

void DGoDropMenuButton::Release(Nlm_PoinT mouse)
{
	fPop->Hide();
	fPop->Invalidate();
	DIconButton::Release(mouse);
#if 0
	Nlm_RecT area;
	this->ViewRect( area);
	if (fFrame) Nlm_InsetRect( &area, kFInset, kFInset);
	Nlm_InvertRect( &area);
	if (Nlm_PtInRect( mouse, &area)) {
		// then button is "selected", otherwise not.
		this->IsMyViewAction(this);
		}
#endif
}




//class	DGopherListDoc : public DWindow

Boolean DGopherListDoc::gOptionIsOn = false;
Boolean DGopherListDoc::gFetchSingles = true;
DMenu * DGopherListDoc::gViewChoiceMenu = NULL;
DView * DGopherListDoc::gLockWinMenuItem = NULL;

Local DGopherList *	 gClipGophers = NULL;
Local DGopher 	  *	 gClipGopher = NULL;


DGopherListDoc::DGopherListDoc( long id) :
		DWindow( id, gApplication) 
{
	Initialize(NULL);
	//fParentlist= new DList(NULL, DList::kDeleteObjects);
	fParentlist= new DList(NULL);
	SetInfoLine();
}


DGopherListDoc::DGopherListDoc( long id, DGopher* parentGopher) :
		DWindow( id, gApplication)
{ 
	Initialize(NULL);

	fParent= parentGopher;
	if (fParent) fParent->newOwner();

	//fParentlist= new DList(NULL, DList::kDeleteObjects);
	fParentlist= new DList(NULL);
	SetInfoLine();
}

DGopherListDoc::DGopherListDoc( long id, DGopherList* activeList, char* name) :
		DWindow( id, gApplication) 
{ 
	Initialize(name);

	//fGolist= activeList;
	// !! clone fGolist !?
	//if (fGolist) fParent= fGolist->fParentMenu;
	//if (fParent) fParent->newOwner();
	//fParentlist= new DList(NULL, DList::kDeleteObjects);
	fParentlist= new DList(NULL);

	SetInfoLine();
	this->ReplaceData( activeList); //??
}


DGopherListDoc::~DGopherListDoc()
{
	if (fGolist) {
		//if (fGolist->GetOwnerCount() <= 1) delete fGolist; else 
		fGolist->suicide();  
		}
	if (fOldlist) {
		//if (fOldlist->GetOwnerCount() <= 1) delete fOldlist; else 
		fOldlist->suicide();  
		}
	if (fParentlist) {
		short i, n= fParentlist->GetSize();
		for (i=0; i<n; i++) {
			short j, m;
			DGopherList* st= (DGopherList*) fParentlist->At(i);
			delete st;
			}
		delete fParentlist;
		}
#if 0
	//if (fParentPop) delete fParentPop;  //noooooooooo ... dview::deletesubviews...
	//if (fGoview) fGoview->suicide();  //noooooooooo ... dview::deletesubviews...
	//if (fParent) fParent->suicide();  // !BOMB BUG -- fParent sometimes (?) in fGolist
#endif
}


void DGopherListDoc::Initialize( char* name)
{
	fGoview= NULL;
	fGolist= NULL; 
	fOldlist= NULL;
	fStatus= NULL;
	fParentlist= NULL;
	fParent= NULL;
	fInUse= false;
	fPinned= false;
	fFirststuff= 1;
	fParentPop= NULL;
	fStickpin= NULL;
	
	short width= -1, height= -1, left= -10, top= -20; // default window loc
	if (!Nlm_EmptyRect(&gGopherListDocRect))  {
#ifndef TRY_WIN_MOTIF
			// leave out of motif til find bug preventing its' DView::SizeTo...
		width= MAX( 40, gGopherListDocRect.right - gGopherListDocRect.left);
		height= MAX( 50, gGopherListDocRect.bottom - gGopherListDocRect.top);
#endif
		}
	this->InitWindow(document, width, height, left, top, name); 
}



void DGopherListDoc::SetInfoLine()
{
	DPrompt* pr;
	Nlm_PoinT nps, droppt;
	Nlm_FonT smallfnt = Nlm_GetFont( "Times", 9, false, false, false, "Roman");
	//smallfnt= Nlm_programFont;

	nps.x= 1;
	nps.y= 1; // clear some weird offset on new docs !? 
	this->SetNextPosition( nps);
	fStickpin= new DIconButton( cPushpin, this, &gPushpinOut);
	this->NextSubviewToRight();

#if 0
	this->GetNextPosition( &nps);
	fParentPop= new DPopupList( cParentPopup, this);
	fParentPop->Hide();
	nps.x += 5;  
	this->SetNextPosition( nps);
	DGoDropMenuButton* ib = 
		new DGoDropMenuButton( cParentDrop, this, &gDownTriangle, fParentPop);
	this->NextSubviewToRight();
#endif

#if 0
	DIconButton* ib = new DIconButton( cParentDrop, this, &gDownTriangle);
	this->NextSubviewToRight();
	pr= new DPrompt( 0, this, "Recent:", 0, 0, smallfnt);	 		
	//pr->SetResize( DView::fixed, DView::moveinsuper);
	this->NextSubviewToRight();

	fParentPop= new DPopupList( cParentPopup, this);
	//fParentPop->SetResize( DView::fixed, DView::moveinsuper);  
	this->NextSubviewToRight();
	this->GetNextPosition( &nps);
	nps.x += 50; // 150 
	this->SetNextPosition(nps);
#endif

	pr= new DPrompt( 0, this, "Status:", 0, 0, smallfnt); 
	//pr->SetResize( DView::fixed, DView::fixed);
	this->NextSubviewToRight();

	fStatus= new DPrompt(  0, this, NULL, 150, 0, smallfnt);   // 350
	//fStatus->SetResize( DView::fixed, DView::fixed);
	this->NextSubviewBelowLeft();
	if (fParent) fParent->fStatusLine= fStatus;
}


void DGopherListDoc::EmptyData(Boolean saveit)
{
	if (saveit)  
		fParentlist->Push( fGolist);
	else {
		if (fOldlist) fOldlist->suicide(); //delete fOldlist;  
		fOldlist= fGolist;
		if (fParent) fParent->suicide();  
		}
	fGolist= NULL;
	fParent= NULL;
	//if (fGoview) fGoview->suicide();  //noooooooooo ... dview::deletesubviews...
	if (fParentPop) {
			//  doesn't vibrant know how to clean out a (popup)menu list !?
		fParentPop->Reset(); // does this drop menu items ?? YES
		//delete fParentPop;
		//fParentPop= new DPopupList( cParentPopup, this);
		}
	if (fGoview) fGoview->SetEmptySelection(false);
	fFirststuff= 1;
	fInUse= false;
}


Boolean DGopherListDoc::ReplaceData( DGopher* parentGopher, char* name, DFile* aFile)
{
	if (!fInUse && !fPinned) {
		Boolean ok = false;
		if (parentGopher) ok= this->CanReadFrom( parentGopher);
		else ok= this->CanReadFrom( aFile);
		if (ok) {
	 		EmptyData(true);    
			if (name) this->SetTitle( name); 
			if (parentGopher) {
				parentGopher->newOwner(); // DGopherListDoc constructor normally does this...
				this->Open( parentGopher);
				}
			else this->Open( aFile);
			return true;
			}
		return true; // !!!!!??? this is a funny result when we didn't replace !! but
				// it means don't try a new window !!
		}
	return false;
}


Boolean DGopherListDoc::ReplaceData( DGopherList* parentlist)
{
		// ! this is buggy still for real replace, but is also used for new DGoListDoc !
	if (!fInUse && !fPinned && parentlist) {
		EmptyData(false);
		fGolist= parentlist;
		fParent= fGolist->fParentMenu;
		if (!fGoview) {
			MakeSubviews(); //fGoview= new DGolistView(0, this, fGolist, kGolistViewWidth);
			fGoview->ChangeRowSize( -1, fGolist->GetSize());  
			}
 		else {
 			fGoview->fItems= fGolist;
			short nold= fGoview->GetMaxRows();
			short nnew= fGolist->GetSize();
			fGoview->ChangeRowSize( -1, nnew-nold);  
			}
		fGoview->SetColWidths(); 	//!
		this->Open(true);  
		return true;
		}
	else
		return false;
}

Boolean DGopherListDoc::PopData( short numitems)
{
		// this freezes after first !!
	short npop= MIN( numitems, fParentlist->GetSize());
	if (!fInUse && !fPinned && npop) {
	
		EmptyData(false);
		
		for ( ; npop>0; npop--) {
			if (fGolist) fGolist->suicide(); //delete fGolist;  
			fGolist= (DGopherList*) fParentlist->Pop();
			}

		if (!fGoview) {
			MakeSubviews(); //fGoview= new DGolistView(0, this, fGolist, kGolistViewWidth);
			fGoview->ChangeRowSize( -1, fGolist->GetSize());  
			}
 		else {
 			fGoview->fItems= fGolist;
			short nold= fGoview->GetMaxRows();
			short nnew= fGolist->GetSize();
			fGoview->ChangeRowSize( -1, nnew-nold);  
			}
		fGoview->SetColWidths(); 	//!
						
		fParent= fGolist->fParentMenu;
		this->Open(true);
		return true;
		}
	else
		return false;
}


Boolean DGopherListDoc::IsGopherDoc(DFile* aFile)
{
	// test to see if aFile is a gopher doc.
	// this is a static function, usable w/o a DGopherListDoc object
	
	if (aFile && aFile->Exists()) {
		char	line[256];
		aFile->OpenFile();
		aFile->ReadLine( line, 256);
		return DGopherList::IsGopherLine( line);
		}
	else
		return false;
}


void DGopherListDoc::SortView(DGopherList::Sorts sortorder)
{
	fGolist->SortList(sortorder);
	fGoview->Invalidate();
}

void DGopherListDoc::ToTextDoc( Boolean ViewAsLink)
{
	Nlm_ParData paratabs = {false, false, false, false, true, 0, 0}; // tabs==tabs
  Nlm_ParData parafmt  = {false, false, false, false, false, 0, 0}; // tabs==colFmt
	char	*txt 	= NULL;
	Nlm_ColData*	cols= NULL;
	Nlm_ParData*	para= &paratabs;

  		// this colFmt needs work -- ?? use char sizes rather than pixel sizes ??
#if 0  
// DAMN SUN CC/CFRONT FAILS at THIS !? the bozo 
	Nlm_ColData colFmt [8] = {
	  { 0, 0, 40, 1, gTextFont, 'l', FALSE, FALSE, FALSE, FALSE}, // name
	  { 0, 0,  7, 1, gTextFont, 'l', FALSE, FALSE, FALSE, FALSE},	// date
	  { 0, 0,  6, 1, gTextFont, 'l', FALSE, FALSE, FALSE, FALSE},	// size
	  { 0, 0, 10, 1, gTextFont, 'l', FALSE, FALSE, FALSE, FALSE},	// kind
	  { 0, 0, 25, 1, gTextFont, 'l', FALSE, FALSE, FALSE, FALSE},	// path
	  { 0, 0, 25, 1, gTextFont, 'l', FALSE, FALSE, FALSE, FALSE},	// host
	  { 0, 0,  5, 1, gTextFont, 'r', FALSE, FALSE, FALSE, FALSE},	// port
	  { 0, 0, 30, 1, gTextFont, 'l', FALSE, FALSE, FALSE, TRUE}		// admin
		};
#else
// for damn sun CC cfront
		Nlm_ColData colFmt [8];
	   Nlm_ColData colFmt0 ={ 0, 0, 40, 1, NULL, 'l', FALSE, FALSE, FALSE, FALSE, FALSE}; // name
	   Nlm_ColData colFmt1 ={ 0, 0,  7, 1, NULL, 'l', FALSE, FALSE, FALSE, FALSE, FALSE};	// date
	   Nlm_ColData colFmt2 ={ 0, 0,  6, 1, NULL, 'l', FALSE, FALSE, FALSE, FALSE, FALSE};	// size
	   Nlm_ColData colFmt3 ={ 0, 0, 10, 1, NULL, 'l', FALSE, FALSE, FALSE, FALSE, FALSE};	// kind
	   Nlm_ColData colFmt4 ={ 0, 0, 25, 1, NULL, 'l', FALSE, FALSE, FALSE, FALSE, FALSE};	// path
	   Nlm_ColData colFmt5 ={ 0, 0, 25, 1, NULL, 'l', FALSE, FALSE, FALSE, FALSE, FALSE};	// host
	   Nlm_ColData colFmt6 ={ 0, 0,  5, 1, NULL, 'r', FALSE, FALSE, FALSE, FALSE, FALSE};	// port
	   Nlm_ColData colFmt7 ={ 0, 0, 30, 1, NULL, 'l', FALSE, FALSE, FALSE, FALSE, TRUE};		// admin
    colFmt[0]= colFmt0;
		colFmt[1]= colFmt1;
		colFmt[2]= colFmt2;
		colFmt[3]= colFmt3;
		colFmt[4]= colFmt3;
		colFmt[5]= colFmt5;
		colFmt[6]= colFmt6;
		colFmt[7]= colFmt7;
#endif
	
	if (ViewAsLink)
		txt= fGolist->WriteLinksForServer();
	else {
			//! later -- set these from user display prefs ....
		Boolean showdate= true, showsize= true, showkind= true, showpath= false,
				showhost=true, showport=true, showadmin= true;
		txt= fGolist->WriteLinksForDisplay( showdate, showsize, showkind, showpath,
						showhost, showport, showadmin);
						
			// this col formatting isn't working right -- name col is too short
		//cols= colFmt;
		//para= &parafmt;
		}
		
	if (txt) {
		// ?? add fancy col tabs ForDisplay ??
		char name[256];
		strcpy(name,"Text of ");
		long len= strlen(name);
		char *namep= name+ len;
		len = 256 - len;
		(void) this->GetTitle(namep, len);
		DWindow* aWin= new DWindow( 0, gApplication, document, -1, -1, -10, -10, name);
		DTextDocPanel* aDoc= new DTextDocPanel( 0, aWin, 500, 300);
		aDoc->SetResize( DView::matchsuper, DView::relsuper);
		//aDoc->SetSlateBorder( false);
		aDoc->SetTabs( gTextTabStops);
		aDoc->Append( txt, para, cols, gTextFont);
		aWin->Open();  
		aDoc->SizeToSuperview( aWin, true, false);
		}
}


void DGopherListDoc::ShowMessage(const char* msg)
{
	if (fStatus) fStatus->SetTitle( (char*)msg);
}


// static
void DGopherListDoc::ProcessFile( DFile* aFile)
{
	Boolean replaced= false;
	//if (gGopherDoc) // !??? this is bad, set to non-null when must be null, on 68Kmac
	//	replaced= gGopherDoc->ReplaceData( NULL, NULL, aFile);
	if (! replaced ) {
		DGopherListDoc* adoc= new DGopherListDoc( kGoListdoc);
		adoc->Open(aFile);   
		}
}



// static
Boolean DGopherListDoc::ProcessGopherWithTest( DGopher* theGo)
{
	DWindow* win;
	
	// ?? always reset thread state here ??
	theGo->fThreadState= DGopher::kThreadNotStarted;  
	if (theGo->fThreadState == DGopher::kThreadNotStarted) {
		if (theGo->fHasAsk && theGo->fHaveReply) theGo->fQueryPlus= theGo->fReplyData;  
		theGo->OpenQuery();
		}
	//fInUse= true; //??
	
	if (theGo->fThreadState == DGopher::kThreadDoneReading) { 
		theGo->fThreadState= DGopher::kThreadNotStarted; // ! need to reset this somewhere for used gophers
		return false; 
		}
	else {
		DTask* readTask= theGo->newTask(cRead, DGopher::kGopherTask);
		readTask->SetRepeat(true);
		theGo->fTransferType= kTransferBinary;  // ! NOT True always, but need in cases of binary data? ?!
		theGo->ProcessTask(readTask); // read 1st part of data
		theGo->PostTask( readTask);		// post to event handler to read rest of data
		
		char * cbeg = theGo->fInfo;
		ulong clen = theGo->fInfoSize;
		char * datastart;
		short format= DGoLinkedTextDoc::TestFormat(cbeg, clen, datastart);
		switch (format) {
		
			case DGopher::cGopherNetDoc:
				DGopherListDoc::NewNetdoc( theGo, readTask);
				break;

			case DGopher::cNewGopherFolder:
			case DGopher::cOpenGopherFolder:
				if (true) { // (! this->ReplaceData( theGo, NULL, NULL)) {
					DGopherListDoc* win= new DGopherListDoc( DGopherListDoc::kGoListdoc, theGo);
					win->Open( theGo);  
					}
				break;
		
			case DGopher::cNewGopherFile:
			case DGopher::cOpenGopherFile:
			caseFile:
				win= new DGopherFileDoc( DGopherFileDoc::kGoFiledoc, theGo);  
				win->Open();
				break;
	
#if 0
			case DHTMLHandler::kHTMLformat: 
			case DGIFHandler::kGIFformat	: 
			case DPICTHandler::kPICTformat:  
			case DRTFHandler::kRTFformat  : 
#endif
			default:
				if (theGo->fSaveToDisk) goto caseFile;
				if (gGopherTextDoc && gGopherTextDoc->PushNewDoc(theGo, NULL)) 
					;
				else {
					win= new DGoLinkedTextDoc( DGoLinkedTextDoc::kGoLitextdoc, theGo, NULL); //?? this);  
					win->Open();
					}
				break;

				}
			return true;
			}
}



// static
void DGopherListDoc::NewNetdoc( DGopher* theGo, DTask* readTask)
{
	DWindow* win;
	Boolean  doPostReader = false;
	
	// document is gopher link data followed by any other document data kind
#if 1
	DGopherList* newGolist= new DGopherList(NULL);
#else
	DGopherListDoc* newGopherDoc= new DGopherListDoc();
	newGopherDoc->fGolist= new DGopherList(NULL);
	newGopherDoc->fGoview= new DGolistView(0, newGopherDoc, newGopherDoc->fGolist, kGolistViewWidth);
#endif

	// read data from net & process as gopherdoc data as long as it
	// is right kind: +INFO: ... +etc:
	// ?? look for transition flag, +DATA: ??
	if (theGo->fThreadState == DGopher::kThreadNotStarted) {
		if (theGo->fHasAsk && theGo->fHaveReply) theGo->fQueryPlus= theGo->fReplyData;  
		theGo->OpenQuery();
		}
	theGo->fTransferType= kTransferBinary; // !?! this is a problem !! need to resolve

	if (theGo->fInfoSize<10 && theGo->fThreadState == DGopher::kThreadDoneReading) { 
		theGo->fThreadState= DGopher::kThreadNotStarted; // ! need to reset this somewhere for used gophers
		return; 
		}
	else {
		char * cbeg, * cdata, * cp, * ep;
		ulong clen;
		Nlm_Uint4 ioffs;
		Boolean found= false;
		ioffs= 0;
		cdata= NULL;
		if (readTask) {
			doPostReader= false;
			}
		else {
			readTask= theGo->newTask(cRead, DGopher::kGopherTask);
			readTask->SetRepeat(true);
			doPostReader= true;
			}
		do {
			theGo->ProcessTask(readTask); // read 1st part of data
			cbeg = theGo->fInfo;
			clen = theGo->fInfoSize;
			cp= cbeg + ioffs;
			found= false;
			do {
			  ep= (char*) MemChr( cp, kLF, clen - ioffs);
			  if (!ep) ep= (char*) MemChr( cp, kCR, clen - ioffs);
			  if (ep) {
			  	cp = ep+1;
			  	if (*cp == '+') found = (StrNICmp( cp, "+DATA:", 6) == 0);
			  	}
			  ioffs= cp - cbeg;
			} while (!found && ep);
			if (found) cdata= cp;
		} while (!found && theGo->fThreadState != DGopher::kThreadDoneReading);
			
	 	if (found) {
	 		
	 		{  // drop the +MENU: indent !
	 		char *cp, *newp, thisc, lastc;
	 		ioffs= 0;
			found= Nlm_StrngPos(cbeg, "+MENU:"/*CRLF*/,0, false/*case*/, &ioffs);
			if (found) {
				cbeg += ioffs + 6; 
				for (cp= newp= cbeg, lastc= 0; cp < cdata && *cp; cp++) {
					thisc= *cp;
					if (!(thisc == ' ' && (lastc == kCR || lastc == kLF))) 
						*newp++= thisc;
					lastc= thisc;
					}
				*newp= 0;
				}
			}
			
		  //newGopherDoc->AddData( cbeg, cdata-cbeg); //ReadLinks
		  newGolist->ReadLinks(  cbeg, cdata-cbeg);
		  cdata += 6; // ?? + sizeof("+DATA:"/*CRLF*/);
		  while (*cdata == '+' || isspace(*cdata)) cdata++;
			// look for gopher size info
			if (*cdata == '-' || isdigit(*cdata)) {
				theGo->fBytesExpected = atol( cdata);
		  	while (*cdata == '-' || isdigit(*cdata)) cdata++;
		    if (*cdata == kCR || *cdata == kLF ) cdata++;
		    if (*cdata == kCR || *cdata == kLF ) cdata++;
				}
			theGo->fInfoSize -= (cdata - theGo->fInfo);
			Nlm_MemMove( theGo->fInfo, cdata, theGo->fInfoSize+1); 
			}
			
		//if (theGo->fInfoSize < 1024) 
		if (theGo->fThreadState != DGopher::kThreadDoneReading)
			theGo->ProcessTask(readTask); // read more of data so testor can find format
		if (doPostReader) theGo->PostTask(readTask);		// post to event handler to read rest of data
		if (gGopherTextDoc && gGopherTextDoc->PushNewDoc(theGo, newGolist)) 
			;
		else {
			win= new DGoLinkedTextDoc( DGoLinkedTextDoc::kGoLitextdoc, theGo, newGolist); //?? this);  
			win->Open();
			}
		//newGopherDoc->suicide(); //!? DGoLinkedTextDoc is now owner
		newGolist->suicide(); 
		}
}


//static
void DGopherListDoc::ProcessGopher( DGopher* theGo, short itsViewChoice)
{
	DWindow* win;
	
	if (theGo) {
		gCursor->watch();

		switch (theGo->ItemForm(itsViewChoice)) {   

#if 1
			case DGopher::cOpenGopherText:
			case DGopher::cNewGopherText:
			case DGopher::cNewGopherFolder:
			case DGopher::cOpenGopherFolder:
			caseText:
				(void) DGopherListDoc::ProcessGopherWithTest( theGo);				
				break;
#else
			case DGopher::cOpenGopherText:
			case DGopher::cNewGopherText:
			caseText:
				if (theGo->fSaveToDisk) goto caseFile;
				if (gGopherTextDoc && gGopherTextDoc->PushNewDoc(theGo, NULL)) 
					;
				else
					win= new DGoLinkedTextDoc( DGoLinkedTextDoc::kGoLitextdoc, theGo, NULL); //?? this);  
					win->Open();
					}
				break;

			case DGopher::cNewGopherFolder:
			case DGopher::cOpenGopherFolder:
				if (! this->ReplaceData( theGo, NULL, NULL)) {
					DGopherListDoc* win=
						new DGopherListDoc( DGopherListDoc::kGoListdoc, theGo);
					win->Open( theGo);  
					}
				break;
#endif	

			case DGopher::cGopherNetDoc:
				DGopherListDoc::NewNetdoc( theGo, NULL);
				break;

			case DGopher::cNewGopherImage:
			case DGopher::cOpenGopherImage:
				{
				DGopherMap* aMapper= NULL;
				if (gGopherMap) aMapper = gGopherMap->MatchHandlerKind( theGo);
				if (aMapper && aMapper->GetDisplay() ) goto caseText;
				else goto caseFile;
				}
				break;
								
			case DGopher::cNewGopherFile:
			case DGopher::cOpenGopherFile:
			caseFile:
				win= new DGopherFileDoc( DGopherFileDoc::kGoFiledoc, theGo); //'GFil'
				win->Open();
				break;
		
			case DGopher::kGopherTask:
				if (theGo->fHasAsk && theGo->fHaveReply) theGo->fQueryPlus= theGo->fReplyData;  
				theGo->OpenQuery();
				break;

			case DGopher::cGopherGetInfo:
				win= new DGopherInfoDoc( DGopherInfoDoc::kGoInfodoc, theGo); // 'GIfo'
				win->Open();
				break;
			
			case DGopher::cNewGopherAsk:
			case DGopher::cOpenGopherAsk:
				win= 
					new DGopherAskDoc( DGopherAskDoc::kGoAskdoc, theGo, gGopherDoc/*this*/, itsViewChoice);
				if (win) win->Open();
				break;
	
			default:
				break;
			}
		
		gCursor->arrow();
		}
}


// static
void DGopherListDoc::ReadItemInfo(DGopher* theGo)
{
	if (theGo) {
		gCursor->watch();
		short aSavedType= theGo->fTransferType;
		theGo->fTransferType= kTransferText;
		theGo->InfoTask();   
		theGo->ShowMessage("read item info...");
		while (theGo->ThreadProgress() != DGopher::kThreadDoneReading) {
			gApplication->ProcessTasks();  // wait for info to arrive
			}
		theGo->CloseQuery(); // patch for mswin net bug ?? 
		theGo->DeleteInfo(); 
		theGo->ShowMessage(" ");
		theGo->fTransferType= aSavedType;
		gCursor->arrow();
		}
}

void DGopherListDoc::TopOfGopherHole()
{	
	DGopher* aGopher= fGoview->SelectedGopher();
	short aRow = fGoview->GetSelectedRow();
	if (aGopher) {
		DGopher* bGopher= DGopherList::CopyToNewKind( kTypeFolder, aGopher, fGolist);
		if (bGopher) {
			bGopher->StorePath(""); 		// this is key to top of gopher server
			char buf[256];
			StrCpy(buf, "Gopher at ");
			StrNCat(buf, bGopher->GetHost(), 255);
			bGopher->StoreName(buf); 		 
			fGolist->InsertBefore( aRow, (DObject*) bGopher);
			fGoview->ChangeRowSize( aRow, 1); 
			Dirty(); 
			}
		}
}

void DGopherListDoc::EditOldGopher()
{	
	short aRow = fGoview->GetSelectedRow();
	DGopher *aGopher= fGoview->SelectedGopher();
	if (aGopher) {
		char oldKind= aGopher->fType;
		if (aGopher->Edit(true) ) {
			if (aGopher->fType != oldKind) {
			  DGopher* bGopher=
			  	 DGopherList::CopyToNewKind( aGopher->fType, aGopher, fGolist, true);
			  if (bGopher) {
					delete aGopher; //aGopher->suicide();  
					fGolist->AtPut( aRow, (DObject*) bGopher); //! re-insert bgopher into list 
					}
				}
			Nlm_RecT vbounds;
			fGoview->GetRowRect( aRow, vbounds);
			fGoview->InvalRect( vbounds);
			Dirty(); 
			}
		}
}
 
 
void DGopherListDoc::InsertNewGopher(Boolean editAll)
{			
	short aRow = fGoview->GetSelectedRow();
	if (aRow==0) aRow= fGolist->GetSize();
	aRow++;	// insert after...
	
	DGopher* aGopher= new DGopher(); 
	char oldKind= aGopher->fType;	
	aGopher->SetOwner(fGolist); 
	if (!editAll) aGopher->fType= kTypeFolder; // !editAll is used for NewServer() call !
	 
	if (aGopher->Edit( editAll)) {
		//if (aGopher->fType != oldKind || !editAll) // bug here -- missing CopyToNewKind
		{
		 DGopher* bGopher=
		 	 DGopherList::CopyToNewKind( aGopher->fType, aGopher, fGolist, true); 
		 if (bGopher) {
				delete aGopher; //aGopher->suicide();  
				aGopher= bGopher;
				}
			}
			
		fGolist->InsertBefore( aRow, (DObject*) aGopher);
		fGoview->ChangeRowSize( aRow, 1);  
		Dirty(); // !? this needs to track the fGolist, which can be popped/pushed
		}		
	else
		delete aGopher; //aGopher->suicide();   
} 
	

// static
short DGopherListDoc::DefaultGopherView(DGopher* aGopher)
{
	Boolean canDisplay = false;
	short bestItem = 0;
	DGopherMap* aMapper;
	
	if (!aGopher) return 0;
	
			// ???
	if (aGopher->fType == kTypeFolder 
		|| aGopher->fType == kTypeQuery
		|| aGopher->fIsMap ) 
			return 0;
			
	gCursor->watch();
	if (aGopher->fIsPlus == kGopherPlusYes && !aGopher->fViews) 
		DGopherListDoc::ReadItemInfo(aGopher);
	if (aGopher->fViews) {

#if 0
		aGopher->CheckViewMappers();
		// ?? now how do we get bestItem value ?? set aGopher->fViewChoice ??
		
#else
		char *cp, *viewkind;
		short i, nviews;
		unsigned short lastrank, bestPref = 0;
		
		nviews= aGopher->fViews->GetSize();
		for (i= 0; i<nviews; i++) {
			lastrank = 0; 
			DGopherItemView* giv= (DGopherItemView*) aGopher->fViews->At(i);
			viewkind= (char*)giv->Kind();
			aMapper= gGopherMap->MatchHandlerKind( lastrank, viewkind); 
			if (aMapper) {
				if ( aMapper->GetDisplay() ) canDisplay= true;
				if ( aMapper->fPreference > bestPref) {
					bestItem= i + 1;
					bestPref= aMapper->fPreference;
					}
				}
			}
#endif

		}
	else {
		aMapper= gGopherMap->MatchGopherType( aGopher->fType, true); 
		if (aMapper) {
			if ( aMapper->GetDisplay() ) canDisplay= true;
			//if (aMapper->fHandlerType == 1) canDisplay= true;
			//else if (aMapper->fHandlerType == 2) canFile= true;
			}
		}
		
	//gCursor->arrow();
	return bestItem;
}










class DGoviewListView;

class DViewChoiceDialog : public DWindow 
{
public:
	enum ViewChoiceCmds { cFetchToDisplay = 111, cFetchToFile, cFetchInfo, 
										cFetchGroup, cFetchEdit, cMapBut };
	DGoviewListView*	fListBox;  //DListBox * fListBox;
	short			 fViewChoice, fFetchMethod;
	DGopher  * fParentGo;
	DCluster * fFetchCluster;
	
	DViewChoiceDialog(DGopher* itsGopher);
	virtual void Open();
	virtual void OkayAction();
	virtual short Result() { return fViewChoice; }
	virtual Boolean IsMyAction(DTaskMaster* action);
};


class DGoviewListView : public DTableView 
{
public:
	DViewChoiceDialog *	fDialog;
	DList * fItems; // of DGopherItemView*
	DGopher * fGopher;
	Nlm_FonT	fItalFont;
	
	DGoviewListView(long id, DGopher* itsGopher, DViewChoiceDialog* itsDialog, short pixwidth, short pixheight);
	virtual void DoClickAt(long row, long col);
	virtual void SingleClickAt(long row, long col);
	virtual void DoubleClickAt(long row, long col);
	virtual void GetReadyToShow();
	virtual void DrawCell(Nlm_RecT r, long row, long col);
	virtual DGopherItemView* SelectedView();
	virtual void  SetValue( short value) { SelectCells( value, 0, false); }
	virtual short GetValue() { 
		if (GetSelectedRow() != DTabSelection::kNoSelection) return (1+GetSelectedRow());
		else return 0;
	  }
};


DGoviewListView::DGoviewListView(long id, DGopher* itsGopher, DViewChoiceDialog* itsDialog, 
												short pixwidth, short pixheight):
	DTableView( id, itsDialog, pixwidth, pixheight, 5, 2, 50, 15, true, false),
	fGopher( itsGopher), fItalFont(NULL),
	fDialog(itsDialog)
{	
	fItems= fGopher->fViews;
	this->SetSlateBorder(true); 
	this->SetResize( DView::relsuper, DView::relsuper);
	this->SetTableFont(gGoviewFont);
	if (gGoviewFontName && *gGoviewFontName)
		fItalFont= Nlm_GetFont( gGoviewFontName, gGoviewFontSize, false, true, false, NULL);
	else
		fItalFont= Nlm_GetFont( gTextFontName, gTextFontSize, false, true, false, NULL);
	this->GetReadyToShow(); // ??
}

void DGoviewListView::DoClickAt( long row, long col)
{
	DGopherItemView* giv= (DGopherItemView*) fItems->At(row);
	if (giv) {
		if (giv->fViewStatus  == DGopherItemView::kUnknownStatus) {
			// dialog to append view handler
			if (DGopherMapDoc::EditHandler( (char*) giv->Kind())) {
				fGopher->CheckViewMappers();
				this->Invalidate();
				}
			}
		else {
			DTaskMaster okay(cOKAY);
			fDialog->IsMyAction( &okay);
			//fDialog->OkayAction();
			//fDialog->Close();
			}
		}
}

void DGoviewListView::SingleClickAt(long row, long col)
{
	DTableView::SingleClickAt( row, col); // must select item !
	if (gSingleClicker) DoClickAt( row, col);
}

void DGoviewListView::DoubleClickAt(long row, long col)
{
	DTableView::DoubleClickAt( row, col); 
	if (gDoubleClicker) DoClickAt( row, col);
}


DGopherItemView* DGoviewListView::SelectedView()
{
	if (GetSelectedRow() != DTabSelection::kNoSelection)
		return (DGopherItemView*)fItems->At( GetSelectedRow());
	else 
		return NULL;
}

void DGoviewListView::GetReadyToShow()
{
	short  wid;
	SelectFont();
	short charheight= Nlm_FontHeight();
	if (fItems && fItems->GetSize() > 0) SetTableSize( fItems->GetSize(), GetMaxCols());
	SetItemHeight(0, GetMaxRows(), charheight);
	wid= Nlm_StringWidth("* ");
	this->SetItemWidth( 0, 1, wid);
	wid= Nlm_StringWidth("application/gopher+-menu-long");
	this->SetItemWidth( 1, 1, wid);  
}

void DGoviewListView::DrawCell(Nlm_RecT r, long row, long col)
{
	if (col) r.left += 2; // quick fix for cells against each
	DGopherItemView* giv= (DGopherItemView*) fItems->At(row);
	if (giv) {
		Boolean changefont= false;
		//if (giv->fViewStatus == DGopherItemView::kMaxStatus) 
		//	this->SetValue( row); 		 
		switch (col) {
			case 0:  
				Nlm_DrawString(&r, (char*) giv->ViewStatus(), 'l', false);
				break;   
			case 1: 
				if (giv->fViewStatus==DGopherItemView::kUnknownStatus && fItalFont) {
					changefont= true;
		 			Nlm_SelectFont(fItalFont);
		 			}
				Nlm_DrawString(&r, (char*) giv->ViewVal(), 'l', false);
				break;   
			}
		if (changefont) SelectFont();
		}
}








DViewChoiceDialog::DViewChoiceDialog(DGopher* itsGopher) :
		DWindow( 0, NULL, DWindow::fixed, -10, -10, -50, -20, "View choices", kDontFreeOnClose),
		fViewChoice(0),
		fFetchMethod(0),
		fParentGo(itsGopher),
		fListBox(NULL)
{	
}

Boolean DViewChoiceDialog::IsMyAction(DTaskMaster* action) 
{
	  switch (action->Id()) {

		default:
			return DWindow::IsMyAction(action);	
			
#if 0			
			case cMapBut: {
				short nviews= 0;
				if (fParentGo && fParentGo->fViews) 
					nviews= fParentGo->fViews->GetSize();
				if (nviews) {		
					long i;
					char* cp;
					for (i= 0; i<nviews; i++) {
						char * viewkind;
						unsigned short lastrank = 0; 
						DGopherItemView* giv= (DGopherItemView*) fParentGo->fViews->At(i);
						viewkind= (char*)giv->Kind();
						DGopherMap* aMapper= gGopherMap->MatchHandlerKind( lastrank, viewkind); 
						if (!aMapper) {
							
							}
						}
					}
				}
				return true;
#endif

		}
}


void DViewChoiceDialog::OkayAction() 
{ 
	if (fListBox) fViewChoice= fListBox->GetValue(); /* - 1;  */
	else fViewChoice= 0;
	if (fFetchCluster) switch ( fFetchCluster->GetValue()) {
		default:
		case 1: fFetchMethod= cFetchToDisplay; break;
		case 2: fFetchMethod= cFetchToFile; break;
		case 3: fFetchMethod= cFetchInfo; break;
		case 4: fFetchMethod= cFetchEdit; break;
		}
}

void DViewChoiceDialog::Open()
{
	DCluster* clu;
	DRadioButton* rb;
	DView* super = this;
	DPrompt* pr;
	unsigned short lastrank, bestPref = 0;
	long  nviews = 0, bestItem = 0;
	long	macType, macSire;
	char * viewkind = NULL;
	DGopherMap* aMapper;
	Boolean canDisplay= false, canFile= true; // always?? even for folder?
	Boolean unmapped= false;
	char namebuf[256];
	
	sprintf( namebuf, "Fetching '%s'", (char*)fParentGo->ShortName());
	pr= new DPrompt(0, this, namebuf, 0, 0, Nlm_programFont);	 	
	this->NextSubviewBelowLeft();
	sprintf( namebuf,  "Mod-date: %s   Size: %s", (char*)fParentGo->GetDate(), (char*)fParentGo->GetDataSize());
	pr= new DPrompt(0, this, namebuf, 0, 0, Nlm_programFont);	 	
	this->NextSubviewBelowLeft();
	
	if (fParentGo && fParentGo->fViews) {
		fParentGo->CheckViewMappers();
		short height= 12 * Max( 3, Min( 7, fParentGo->fViews->GetSize()));
		fListBox= new DGoviewListView( 0, fParentGo, this, 250, height);
		fListBox->SetResize( DView::relsuper, DView::relsuper);
		fListBox->SetValue( fParentGo->fViewchoice - 1); 
		lastrank = 0; 
		aMapper= gGopherMap->MatchHandlerKind( lastrank, 
									(char*)fParentGo->GetViewChoiceKind()); 
		if (aMapper && aMapper->GetDisplay() ) canDisplay= true;
		
		this->NextSubviewBelowLeft();
		sprintf( namebuf, " ? is an unknown view, double-click to link to a handler" LINEEND " * indicates the best view");
		//new DPrompt(0, this, namebuf, 0, 0, gSmallFont);	 	
		new DNotePanel(0, this, namebuf, 250, 0, gSmallFont);
		//this->NextSubviewBelowLeft();
		}
 	
	else {
		// no views
		fListBox= NULL;
		pr= new DPrompt(0, this, "No view data. Default view is selected.", 0, 0, Nlm_programFont);	 	
		aMapper= gGopherMap->MatchGopherType( fParentGo->fType, true); 
		if (aMapper && aMapper->GetDisplay() ) canDisplay= true;
		}

		
	clu= new DCluster(cFetchGroup, super, 15, 5, false, "Fetch options");
	fFetchCluster= clu;
	super= clu;
	rb= new DRadioButton( cFetchToDisplay, super, "to Display");
	if (!canDisplay) rb->Disable();
	else rb->SetStatus(true); // ?? or clu->SetValue(1); 

	super->NextSubviewToRight();
	rb= new DRadioButton( cFetchToFile, super, "to File");
	if (!canFile) rb->Disable();
	else if (!canDisplay) rb->SetStatus(true);
	
	super->NextSubviewToRight();
	rb= new DRadioButton( cFetchInfo, super, "Get Info");

#ifdef BYVIEW_EDIT
	super->NextSubviewToRight();
	rb= new DRadioButton( cFetchEdit, super, "Edit");
#endif
	super= this;
	
	//if (unmapped) DButton* but= new DButton( cMapBut, super, "Map unknown(?) types");
	 
	this->AddOkayCancelButtons();	
	gCursor->arrow();
	DWindow::Open();
}




	
// static
Boolean DGopherListDoc::GopherByViewDialog(DGopher* go, DGopherList* itsList) 
{
	short iview= 0;
	Boolean result= false;
	if (go) { 
		if (go->fIsPlus == kGopherPlusYes && !go->fViews) 
			DGopherListDoc::ReadItemInfo(go);
		DViewChoiceDialog* dlg= new DViewChoiceDialog(go);
		if ( dlg->PoseModally() ) {
			iview = dlg->Result();
			Boolean oldsave= go->fSaveToDisk;
			switch (dlg->fFetchMethod) {
				default:
				case DViewChoiceDialog::cFetchToDisplay: go->fSaveToDisk= false; break;
				case DViewChoiceDialog::cFetchToFile: go->fSaveToDisk= true; break;
				case DViewChoiceDialog::cFetchInfo  : iview= DGopher::kGetItemInfo; break;

#ifdef BYVIEW_EDIT
						// this is mainly for debugging golinkdocs ...
				case DViewChoiceDialog::cFetchEdit  : 
				{	
				char oldKind= go->fType;
				if (go->Edit(true) ) {
					if (go->fType != oldKind) {
							// ?? use go->fOwnerList instead of passing itsList ??
					  DGopher* bGopher= 
					  	DGopherList::CopyToNewKind( go->fType, go, itsList, true);
					  if (bGopher) {
							long aRow= itsList->GetIdentityItemNo((DObject*) go);		// uses long(item) == long(indexobj)
							itsList->AtPut( aRow, (DObject*) bGopher); //! re-insert bgopher into list 
							delete go; //go->suicide();  
							}
						}
					}
				delete dlg;
				return true;
				}
				break;
#endif

				}
			delete dlg;
			result= true;
			DGopherListDoc::ProcessGopher(go, iview);
			go->fSaveToDisk= oldsave;
			}
		else  
			delete dlg;
		}
	return result;
}


void DGopherListDoc::PinWindow(Boolean turnon)
{ 
	fPinned= turnon;
	if (fStickpin) {
		if (fPinned) fStickpin->SetIcon( &gPushpinIn, true);
		else fStickpin->SetIcon( &gPushpinOut, true);
		}
}



Boolean DGopherListDoc::IsMyAction(DTaskMaster* action) 
{
	long  menuid;
	
	if (!action) return false;
	if (action->fSuperior && action->fSuperior->Id() == kViewKindMenu)
		menuid= action->fSuperior->Id();
	else 
		menuid= action->Id();
		
	switch (menuid) {

			case DApplication::kNew:
				NewGopherDoc();
				return true;

			case DApplication::kUndo:
				Message(MSG_OK,"DGopherListDoc::Undo not ready.");
				return true;
				
			case DApplication::kCopy:
				if (gClipGopher) gClipGopher->suicide();
				gClipGopher= fGoview->SelectedGopher();
				if (gClipGopher) {
					gClipGopher= (DGopher*) gClipGopher->Clone();
					gClipGopher->SetOwner(gClipGophers);
					}
				return true;
			
			case DApplication::kPaste:
				AddGopherToView( gClipGopher);
				return true;
 
			case DApplication::kCut:
				CutClearSelection( true);
				return true;
			case DApplication::kClear:
				CutClearSelection( false);
				return true;
			case DApplication::kSelectAll:
				Message(MSG_OK,"DGopherListDoc::SelectAll not ready.");
				return true;
			
			case cParentPopup:
				{
				DPopupList* pl= (DPopupList*) action;  
				if (pl) PopData( pl->GetValue() - 1);					
				}
				return true;
				
			case cParentDrop:
				//if (fParentPop) PopData( fParentPop->GetValue() - 1);		
				Nlm_Beep();			
				return true;
			
			case cPushpin:
				PinWindow( !fPinned);
				return true;

	  	case kViewChoiceMenu:

#ifndef BOBS_WIN_MAC						
				// we need a small dialog w/ our newest popup menu
				(void) DGopherListDoc::GopherByViewDialog( this->fGoview->SelectedGopher(), fGolist);
#else
	  		{
				short iview= action->Id() - kViewDefault;  
				DGopher* go= this->fGoview->SelectedGopher();
				if (go) DGopherListDoc::ProcessGopher(go, iview);
				}
#endif
				return true;
				
			case kViewKindMenu:
				switch(action->Id()) {
					case kViewByDefault:
							this->SortView(DGopherList::kSortByItem); return true;
					case kViewByDate:
							this->SortView(DGopherList::kSortByDate); return true;
					case kViewBySize:
							this->SortView(DGopherList::kSortBySize); return true;
					case kViewByName:
							this->SortView(DGopherList::kSortByName); return true;
					case kViewByKind:
							this->SortView(DGopherList::kSortByKind); return true;
					case kViewByHost:
							this->SortView(DGopherList::kSortByHost); return true;
					case kViewAsText:
					case kViewAsLink:
							this->ToTextDoc( action->Id() == kViewAsLink); return true;
					default: return false;
					}
			
		default: 
			return false;
		}
}
			


Boolean DGopherListDoc::IsMyTask(DTask* theTask) 
{
	if (theTask->fKind == kGoListdoc) {
		ProcessTask( theTask);
		return true;
		}
	else 
		return DWindow::IsMyTask(theTask);
}

void DGopherListDoc::ProcessTask(DTask* theTask) 
{
	if (theTask->fNumber == cProcessGopher) {
		// ?? fix for obscure Motif bug from DGoAskDoc calling ProcessGopher...
		DGopher* aGoph= (DGopher*) theTask->fExtra;
		Boolean saveuse= this->fInUse;
		this->fInUse= true;
		DGopherListDoc::ProcessGopher( aGoph, aGoph->fViewchoice);
		this->fInUse= saveuse;
		//aGoph->suicide(); //??
		}
		
	else if (theTask->fNumber == cAdd && fParent) {
		fInUse= true;  
		fParent->ShowProgress();
		if (fParent->fInfoSize) {
		
#ifdef TRY_WITHOUT_THIS
			// this code, if it worked, would permit async display of gopher items
			// as each line/item is received.  but it is buggy, causing crashes
			// after a few fetches.  Rewrite at some point -- try Reverse StrStr(bufend,CRLF"+INFO")
			
			char * cbeg = fParent->fInfo;
			char * cend = cbeg + fParent->fInfoSize;
			char * cp = cend;
			char	clineend = '\012';  
				
			if (StrStr( cbeg, CRLF"+INFO")) {
				Boolean done = false;
				do {
					while (cp > cbeg && *cp != clineend) cp--; 
					if (cp <= cbeg) done= true;
					else if (StrNCmp( cp+1, "+INFO", 5) == 0) done= true;
					else cp--;
				} while (!done);
				}
			else	// assume gopher- !? poor assumption?
				while (cp > cbeg && *cp != clineend) cp--; 
			
			if (cp <= cbeg) cp= cend; 
			//char csave= *cp; *cp= 0;  // no more need for null term
			this->AddData(cbeg, cp - cbeg);  
			//*cp= csave;
			if (cp < cend) cp++; // skip the linefeed, which puts extra blank line in fDoc...
			
			long clen= cend - cp;
			cbeg= (char*) MemDup( cp, clen+1);
			//fParent->DeleteInfo();// this creates new fInfo
			if (fParent->fInfo) MemFree(fParent->fInfo); 
			fParent->fInfo= cbeg;   
			fParent->fInfoSize= clen; 

			if (fFirststuff>0) {
				fGoview->SetColWidths(); //?!! required, some time after having real data
				fFirststuff= 0;
				}
#endif

			}
		
		if (fParent->ThreadProgress() == DGopher::kThreadDoneReading) {
			theTask->SetRepeat(false); // done w/ gopher fetch, dequeue this task
			this->ShowMessage("done reading");
			fInUse= false;  
			
				// if TRY_WITHOUT_THIS is fixed, ?? change this
			char * cbeg = fParent->fInfo;
			char * cend = cbeg + fParent->fInfoSize;
			if (cend > cbeg) {
				//*cend= 0; // no longer need null term
				// ?? is bomb bug due to lack of end of data ??
#if 0
				// still don't know where this bomb is coming from
				// occurs ?? only when returning 1 gopher item in a gopherlist
				// but beyond that ??? data looks okay, happens thru query item & thru go+ ask forms
				
				//char* kBombBugPatch = "i(Quick bug patch for obscure bomb)\t\tno.where.ever\t0"CRLF;	
				
				char *cp = cbeg;
				for (short i= 0; i<2; i++) {
					cp = StrChr(cp, kLF);
					if (cp) cp++;
					}
				if (!cp) {
					DGopher* ago= new DGopher();
					ago->SetLink( *fParent->fInfo, fParent->fInfo);
					char link1[512];
					sprintf( link1, "%c%s\t%s\t%s\t%d\t%c",
						ago->fType, (char*) ago->GetName(), (char*) ago->GetPath(), 
						(char*) ago->GetHost(), ago->fPort, ' ');
					delete ago;
					
					char *next= fParent->fInfo;
					next= StrChr(next, kLF);
					if (next) next++;
					
					char *tmp= StrDup(kBombBugPatch);
				
					StrExtendCat( &tmp, link1);
					StrExtendCat( &tmp, next);
					//StrExtendCat( &tmp, fInfo);
					MemFree(fParent->fInfo);
					fParent->fInfo= tmp;
					fParent->fInfoSize= StrLen(tmp);
					}
#endif
					
				this->AddData(cbeg, cend-cbeg);  
					 
				if (fGolist->GetSize() < 1) { 
					this->Close(); //this->suicide(); //delete this; 
					return; 
					}
				else if (gFetchSingles && fGolist->GetSize() == 1) {
					fGoview->OpenGopherLink(0);
					this->Close(); // this works, but leaves un-updated window region on front window
					return;
					}
					
				fGoview->SetColWidths(); // !! required, some time after having real data
				fGoview->Invalidate(); 
				}
			else 
				this->Close();  //this->suicide(); //delete this;  // dies here on mswin?
			}
		}
		
	else {
		DWindow::ProcessTask(theTask);
		}
}


void DGopherListDoc::Close()
{
	if (fParent &&  fParent->ThreadProgress() != DGopher::kThreadDoneReading) {
		// kill tcp connection !!!!!!!!
		fParent->CloseQuery();
		//fParent= NULL; // don't delete !?
		}

	if (!Nlm_EmptyRect(&fViewrect)) gGopherListDocRect= fViewrect;  
	if (gGopherDoc == this) gGopherDoc= NULL; // track front gopher doc/window
	fInUse= false;
	fPinned= false;
	DWindow::Close();
}

void DGopherListDoc::ResizeWin()
{
	DWindow::ResizeWin();
	if (fGoview && !Nlm_EmptyRect(&fViewrect)) gGopherListDocRect= fViewrect;  
}





void DGopherListDoc::MakeSubviews()
{
	if (!fGolist) fGolist= new DGopherList(fParent); //NULL
	fGolist->fDeleteObjects= DList::kDeleteObjects;
	if (!fGoview) {
		short width, height;
		Nlm_RecT		r;
		Nlm_PoinT		nps;
		this->GetNextPosition( &nps);
		nps.x= 0;  // where is that odd x-offset from ?
		this->SetNextPosition( nps);
		width = MAX( 40, gGopherListDocRect.right - gGopherListDocRect.left) 
	   			- Nlm_vScrollBarWidth  - nps.x; 
		height= MAX( 60, gGopherListDocRect.bottom - gGopherListDocRect.top) 
	 				- Nlm_hScrollBarHeight - nps.y;   
#ifdef WIN_MSWIN
		width  -= 16;
		height -= 24;
#endif
	
		fGoview= new DGolistView(0, this, fGolist, width, height);
		}
	else fGoview->fItems= fGolist;  
} 


Boolean DGopherListDoc::CanReadFrom(DFile*& aFile)
{
		// callers have already checked that aFile exists and IsGopherDoc !!
		// ?? don't need this part now ??
	if (!aFile) {
		char *data = (char*) gFileManager->GetInputFileName(".go4",NULL); 
		if (data) aFile= new DFile( data, "r");
		}
	if (aFile) return true;
	else return false;
}


Boolean DGopherListDoc::ReadFrom(DFile* aFile)
{
	if (aFile) {
		gCursor->watch();
		this->SetTitle( (char*)aFile->GetShortname() );

		MakeSubviews();
		if (!fParent) {
			char docurl[1024];
			StrCpy(docurl, "file:///");
			StrNCat(docurl, (char*)aFile->GetName(), 1000);
			docurl[1023]= 0;
			fParent= DGopherList::GopherFromURL(docurl, strlen(docurl), fGolist);
			// ! GopherFromURL() sticks new gopher into list also !
			fGolist->fParentMenu= fParent;
			}
		
		aFile->OpenFile();
		ulong filelen;
		aFile->GetDataLength(filelen);
		char* buf= (char*) MemNew( filelen+1);
		aFile->ReadData( buf, filelen);
		this->AddData( buf, filelen);
		MemFree( buf);
		aFile->CloseFile();
		delete aFile;
		
		if (fGolist->GetSize() < 1) { return false; }
		fGoview->SetColWidths(); 	//?
		//fGoview->Invalidate();  	//?? for ReplaceData only ??
		gCursor->arrow();
		fInUse= false;  
		return true;
		}
	else
		return false;
}



Boolean DGopherListDoc::CanReadFrom( DGopher* itsParent)
{
	if (itsParent) {
		fParent= itsParent;
		//if (fParent->fThreadState == DGopher::kThreadNotStarted) 
			fParent->ReadTask();   
		if (fParent->fInfoSize<10 && fParent->fThreadState == DGopher::kThreadDoneReading) {
			fParent->fThreadState= DGopher::kThreadNotStarted; // ! need to reset this somewhere for used gophers
			return false; 
			}
		else
			return true; 
		}
	else
		return false;
}


Boolean DGopherListDoc::ReadFrom( DGopher* itsParent)
{
	if (itsParent) {
		fParent= itsParent;
		this->SetTitle( (char*)fParent->ShortName());
		MakeSubviews();
		if (fParent->fThreadState == DGopher::kThreadNotStarted) 
			fParent->ReadTask();   
		if (fParent->fInfoSize<10 && fParent->fThreadState == DGopher::kThreadDoneReading) { 
			//this->Close(); //suicide();
			fParent->fThreadState= DGopher::kThreadNotStarted; // ! need to reset this somewhere for used gophers
			return false; 
			}
	 
		DTask* aTask= newTask(cAdd, kGoListdoc); 
		aTask->SetRepeat(true);
		this->PostTask( aTask); 
		this->ShowMessage("opening link...");
		fInUse= true;
		return true; 
		}
	else
		return false;
}


// static
void DGopherListDoc::NewGopherDoc()
{
	gCursor->watch();
	DGopherListDoc* win= new DGopherListDoc( DGopherListDoc::kGoListdoc);
	//win->Open( theGo); 
	if (win) { 
		char* buf="+INFO: 1IUBio-Archive\t\tiubio.bio.indiana.edu\t71\t+\n"
				"+INFO: 1GopherPup home\t1/IUBio-Software+Data/util/gopher/gopherpup\tiubio.bio.indiana.edu\t70\t+\n"
				"+INFO: 1University of Minnesota Mother Gopher\t\tgopher.micro.umn.edu\t70\t+\n";
		ulong buflen = StrLen(buf);

		win->SetTitle( "Untitled" );
		win->MakeSubviews();
		win->AddData( buf, buflen);
		win->fGoview->SetColWidths(); //?
		win->Open( true);
		gCursor->arrow();
		}
}



void DGopherListDoc::Open( DFile* aFile)
{
	if ( ReadFrom( aFile) )
		Open( true);
	else
		Close(); //suicide();
}

void DGopherListDoc::Open( DGopher* itsParent)
{
	if ( ReadFrom( itsParent) )
		Open( true);
	else
		Close(); 
}


void DGopherListDoc::Open( Boolean haveData)
{
	if (!haveData) {
		Close();
		return;
		}

#if 1				
	// short minwidth= Nlm_StringWidth( (char*)fParentGo->ShortName());
	this->Select(); // ?? for motif
	Nlm_ResizeWindow( (Nlm_GraphiC)fWindow, kWinDragHeight, kWinScrollWidth,
						 kWinMinWidth, kWinExtraHeight);
#endif

	fSaveHandler= this; // replace fGoview's handler w/ us
	if (fStatus && fGolist) fGolist->SetStatus( fStatus);
	if (fParentlist && fParentPop) {
		char * name;
		short i, n= fParentlist->GetSize();
		if (fParent) name= (char*)fParent->ShortName();
		else name= "Current link";
		fParentPop->AddItem(name);
		for (i= n-1; i>=0; i--) {
			DGopher* go= NULL;
			DGopherList* golist= (DGopherList*) fParentlist->At(i);
			if (golist) go= golist->fParentMenu;
			if (go) fParentPop->AddItem((char*)go->ShortName());   
			}
		}

	fGoview->Invalidate(); 		// force redraw !?
	fGoview->SetOffset(0, 0); //  zero the scrollbars
	fGoview->SetTop(0); 				//  reset data scroll offset 

	short i, n= fGolist->GetSize();
	for (i=0; i<n; i++) 
		fGoview->SetLineHeight( i, fGolist->GopherAt(i));

	gGopherDoc= this; // track front gopher doc/window
	fGoview->SizeToSuperview( this, true, true); //??
	DWindow::Open();
}


void DGopherListDoc::UpdateListSize()
{		
	short nnew, nold, i;
	
	nold= fGoview->GetMaxRows();  
	nnew= fGolist->GetSize();
	//fGoview->SetTableSize(nnew, 9);
	fGoview->ChangeRowSize( -1, nnew-nold); // was (nold, .. -1 prevents unneeded redraw ?
}

void DGopherListDoc::AddData(char* gopherData, long datalen)
{		
	short nnew, nold, i;
	
	nold= fGolist->GetSize();
	fGolist->ReadLinks(gopherData, datalen);
	nnew= fGolist->GetSize();
	//fGoview->SetTableSize(nnew, 9);
	fGoview->ChangeRowSize( -1, nnew-nold); // was (nold, .. -1 prevents unneeded redraw ?
}

void DGopherListDoc::Activate()
{	
	SetViewMenu(true); 
	if (gLockWinMenuItem)  gLockWinMenuItem->SetStatus( fInUse || fPinned);		
	gGopherDoc= this; // track front gopher doc/window
	DWindow::Activate();
}

void DGopherListDoc::Deactivate()
{	
	SetViewMenu(false); 
	if (gLockWinMenuItem)  gLockWinMenuItem->SetStatus(false);
	//if (gGopherDoc == this) gGopherDoc= NULL; 
	// ^^ can't do now, Ask window needs this, and when it comes out, this goes... 
	DWindow::Deactivate();
}

void DGopherListDoc::SetViewMenu(Boolean activate)
{	 	
#ifndef BOBS_WIN_MAC						
  //gWindowManager->SetLastActive(this); // motif don't track active windows too good, needs help
#else						
						// THIS IS NO GOOD ON XWIN, and probably NO GOOD on MSWIN
						// Vibrant currently has no way of removing single menu items
						// except for Mac.  Only option for others seems to be removal
						// of entire (sub) menu, with no way to replace w/ a new menu
						// in *same* position !!!
   if (gViewChoiceMenu) {
		 short item= kViewDefault;
		 gViewChoiceMenu->Reset();  
		 gViewChoiceMenu->AddItem( item, "default");
		 DGopher* go= fGoview->SelectedGopher();
		 if (activate && go && go->fViews) {
			long i, n= go->fViews->GetSize();
			for (i= 0; i<n; i++) {
				char *cp, smenu[256];
				item++;
				DGopherItemView* giv= (DGopherItemView*) go->fViews->At(i);
				StrNCpy(smenu, (char*) giv->ViewVal(), 255);
				//cp= smenu; 
				//while (cp= strchr(cp, '/')) *cp= '_';
				gViewChoiceMenu->AddItem( item, smenu);
				//gViewChoiceMenu->EnableItem( item); 
				}
		 	}
	 }
#endif
}


void DGopherListDoc::AddGopherToView( DGopher* aGopher)
{	
	if (aGopher) {
		short aRow = fGoview->GetSelectedRow();
		if (aRow==0) aRow= fGolist->GetSize();
		aRow++;	// insert after...
		aGopher= (DGopher*) aGopher->Clone(); 
		aGopher->SetOwner(fGolist); 
		fGolist->InsertBefore( aRow, (DObject*) aGopher);
		fGoview->ChangeRowSize( aRow, 1);  
		Dirty(); // !? this needs to track the fGolist, which can be popped/pushed
		}
}


void DGopherListDoc::CutClearSelection( Nlm_Boolean saveCut)
{	
	short aRow = fGoview->GetSelectedRow();
	if (aRow == DTabSelection::kNoSelection) return;
	DGopher* aGopher= fGolist->GopherAt(aRow);
	if (saveCut && aGopher) {
		if (gClipGopher) gClipGopher->suicide();
		gClipGopher= aGopher;
		gClipGopher= (DGopher*) gClipGopher->Clone();		
		gClipGopher->SetOwner(gClipGophers); 
		}
	fGolist->AtDelete( aRow); // this kills aRow gopher
	fGoview->SetEmptySelection(false);
	fGoview->ChangeRowSize( aRow, -1);  
	Dirty();  
}


void DGopherListDoc::WriteTo(DFile* aFile)
{
	char	 line[512];
	char	*data;
	Boolean	callerOwnsFile= (aFile != NULL);
	
	if (!callerOwnsFile) {
		data= this->GetTitle();
		sprintf(line, "%s.go4", data);
		MemFree(data);
		data= (char*) gFileManager->GetOutputFileName(line);  
		if (data) aFile= new DFile( data, "w", "TEXT", "IGo4");
		}
		
	if (aFile) {
		gCursor->watch();
		data= fGolist->WriteLinks();
		ulong	dlen= StrLen(data);
		
		aFile->OpenFile();
		aFile->WriteData(data, dlen);
		aFile->CloseFile();
		MemFree(data);
		if (!callerOwnsFile) delete aFile; 
		this->NotDirty();
		gCursor->arrow();
		}	
}












//class DGolistView : public DTableView

	
DGolistView::DGolistView(long id, DView* itsSuperior, DGopherList* itsItems, 
	short pixwidth, short pixheight):
	DTableView( id, itsSuperior, pixwidth, pixheight, 0, 9, 50, 36),
	fItems(itsItems), fDragGopher(NULL), fDragging(false)
{	
	this->SetSlateBorder(false);
  this->SetResize( DView::relsuper, DView::relsuper);

	////>>nevermore>>////this->SetColWidths(); // BAD call HERE ON PPC !!!
	if (gGoviewFont == NULL) gGoviewFont= Nlm_programFont;
	if (gTextFont == NULL) gTextFont= Nlm_programFont;
	this->SetTableFont(gGoviewFont);
	MakeSmallFont();
}


void DGolistView::SizeToSuperview( DView* super, Boolean horiz, Boolean vert)
{
		// size only lower and right sides == leave top area...
#if 0
#ifdef WIN_MOTIF
	//DTableView::SizeToSuperview( super, horiz, vert);
#if 0
	Nlm_RecT r, myr;
	super->ViewRect(r);
	GetPosition(myr);
	if (horiz) myr.right  = r.right - myr.left; // - r.left
	if (vert)  myr.bottom = r.bottom - myr.top; //- r.top 
	//this->Select(); // motif requires ?
	SetPosition( myr);
#endif
#else	
	Nlm_RecT r, myr;
	super->ViewRect(r);
	GetPosition(myr); // ViewRect(myr); <<!NO, diff from GetPos/SetPos
	if (horiz) myr.right= (r.right - r.left) +1;
	// myr.right -= (Nlm_vScrollBarWidth - 8); // mswin fix ?? 
	if (vert) myr.bottom =  (r.bottom - r.top)  +1; 
	// myr.bottom -= (Nlm_hScrollBarHeight + 10); // mswin fix ??
	SetPosition( myr);
#endif
#endif
	this->FindLocation(); //?? added 14Mar94, gopher tables were doing this
}

void DGolistView::Resize(DView* superview, Nlm_PoinT sizechange)
{
	DTableView::Resize( superview, sizechange);
}



void DGolistView::Hold(Nlm_PoinT mouse)
{
	DTableView::Hold(mouse);
}

//Local DGopher* gDragGopher = NULL;

void DGolistView::Drag(Nlm_PoinT mouse)
{
	long row, col;
	DTableView::Drag(mouse);

	// this is called on drag, if click/release are ignored
	//PointToCell(mouse, row, col);
	//fDragGopher= fItems->GopherAt(row);
	fDragging= true;
	gCursor->plus();
}

void DGolistView::Release(Nlm_PoinT mouse)
{
	DTableView::Release(mouse);
	gCursor->arrow();
	
	//short row, col;
	//PointToCell(mouse, row, col);
	//DGopher* ag= fItems->GopherAt(row);
	
	DWindow* win= gWindowManager->WhichWindow( mouse);
	if (fDragGopher && fDragging && win && win != this->GetWindow()) {
		if (win->Id() == DGoLinkedTextDoc::kGoLitextdoc) {
			((DGoLinkedTextDoc*) win)->LinkToSelection( fDragGopher);
			Nlm_Beep();
			}
		else if (win->Id() == DGopherListDoc::kGoListdoc) {
			((DGopherListDoc*) win)->AddGopherToView( fDragGopher);
			Nlm_Beep();
			}
		}
	fDragging= false;
	fDragGopher= NULL;
}


void DGolistView::OpenGopherLink(long gopherItem)
{
	DGopher* ag= fItems->GopherAt(gopherItem); 
	if (ag) {
		Boolean nooption= false;
		Boolean optionIsOn= gKeys->option();
		Boolean oneKnownView = (ag->fViews && ag->fViews->GetSize() < 2);

		if (oneKnownView) {
			unsigned short lastrank = 0;
			DGopherItemView* giv= (DGopherItemView*) ag->fViews->First();
			char * viewkind= (char*)giv->Kind();
			DGopherMap* aMapper= gGopherMap->MatchHandlerKind( lastrank, viewkind); 
			if (!aMapper) oneKnownView= false;
			}
			
		nooption= (ag->fType == kTypeFolder
			|| ag->fType == kTypeQuery  // ???
	  	|| ag->fIsPlus != kGopherPlusYes
	  	|| oneKnownView 
	  	); 
	  	
		if (!nooption && DGopherListDoc::gOptionIsOn) 
			optionIsOn= !optionIsOn;
					
		((DGopherListDoc*)fSuperior)->ShowMessage("opening link...");
		if (optionIsOn) 
			(void) DGopherListDoc::GopherByViewDialog(ag, ag->fOwnerList);
		else  {
			short itsViewchoice= DGopherListDoc::DefaultGopherView(ag);
			DGopherListDoc::ProcessGopher( ag, itsViewchoice);
			}
		}
}




void DGolistView::DoubleClickAt(long row, long col)
{
	if (gDoubleClicker) OpenGopherLink(row);
}

void DGolistView::SingleClickAt(long row, long col)
{
	DTableView::SingleClickAt(row,  col);
	((DGopherListDoc*)fSuperior)->SetViewMenu();
	fDragGopher= fItems->GopherAt(row);
	gWindowManager->SetCurrent((DWindow*)fSuperior); // motif needs help
	if (gSingleClicker) OpenGopherLink(row);
}


DGopher* DGolistView::SelectedGopher()
{
	if (GetSelectedRow() != DTabSelection::kNoSelection)
		return fItems->GopherAt( GetSelectedRow());
	else 
		return NULL;
}


#define USE_MULTI_LINES 1

void DGolistView::SetLineHeight( long item, DGopher* theGo)
{
#if NOTNOW_USE_MULTI_LINES
  short icowidth;
  Nlm_SelectFont(fFont);
	switch (gIconSize) {
		case 0: icowidth= 14; break;
		case 1: icowidth= 20; break;
		default:
		case 2: icowidth= 36; break;
		}
	short nlines = 1;
	//if (gShowURL||gShowDate||gShowSize||gShowHost||gShowPort||gShowPath||gShowAdmin)
	if (gShowExtras)
		nlines++;
	short height= nlines * Nlm_LineHeight();
	if (theGo) height += theGo->HeightAbstract();
	height= MAX( icowidth, height);  // Nlm_FontHeight
	this->SetItemHeight( item, 1, height);
#endif
}



void DGolistView::SetColWidths()
{
	enum { kPlusWidth = 0, kTitleWidth = 1200 }; // == 10
	
	short icowidth, height, nlines, wid;

	nlines = 1;
	//if (gShowURL||gShowDate||gShowSize||gShowHost||gShowPort||gShowPath||gShowAdmin)
	if (gShowExtras)
		nlines++;
		
	//Nlm_WindoW w= Nlm_SavePort( GetNlmObject());
	SelectFont();

	if (fItems && fItems->GetSize() > 0) 
		this->SetTableSize( fItems->GetSize(), 3);  // was 9 cols
	
	switch (gIconSize) {
		case 0: icowidth= 14; break;
		case 1: icowidth= 20; break;
		default:
		case 2: icowidth= 36; break;
		}
		
	height= nlines * Nlm_LineHeight();  // Nlm_FontHeight
#if USE_MULTI_LINES
	if (fItems) {
		short i, n = fItems->GetSize();
		for (i= 0; i<n; i++) {
			DGopher* go= fItems->GopherAt(i);
			this->SetItemHeight( i, 1, MAX( icowidth, height + go->HeightAbstract()));
			}
		}
	else
#endif
		this->SetItemHeight( 0, GetMaxRows(), MAX( icowidth, height));
		
	this->SetItemWidth( 0, 1, icowidth+2); 		
	this->SetItemWidth( 1, 1, kPlusWidth);  
	//wid= Nlm_StringWidth("A long title to measure for max width");
	this->SetItemWidth( 2, 1, kTitleWidth); 
}


void DGolistView::DrawCell(Nlm_RecT r, long row, long col)
{
#define gShowPlus  1
	// need a DTableView field to handle cell rect insets !
	//if (col) 
	r.left += 2; // quick fix for cells against each
	DGopher* ag= fItems->GopherAt(row); 
	if (ag) switch (col) {
	
		case 0: 
			ag->DrawIcon(r, gIconSize); 
			break;   

#if 0		
		case 1: { 
			Nlm_RecT 	lr;
			lr= r;
			lr.bottom= lr.top + Nlm_LineHeight();
			ag->DrawPlus( lr); 
			} 
			break;
#endif
		
		case 2:  {
			Nlm_RecT 	lr;
			Nlm_PoinT pt;
			short lineheight, absHeight = 0, nlines = 1;
			
#if 1
			lineheight= Nlm_LineHeight(); // store this const somewhere !?
#else
			//if (gShowURL||gShowDate||gShowSize||gShowHost||gShowPort||gShowPath||gShowAdmin)
			if (gShowExtras)	nlines++;
			if (ag->fAbstract) absHeight= ag->HeightAbstract(); // this can drag on draw time !!
			lineheight=  (r.bottom - r.top - absHeight) / nlines;
#endif

			lr= r;
			lr.bottom= lr.top + lineheight;
			ag->DrawTitle(lr); 
			
			if (ag->fType != kTypeNote && gShowExtras) {
			  //(gShowURL||gShowDate||gShowSize||gShowHost||gShowPort||gShowPath||gShowAdmin)) 
			  
				lr.top = lr.bottom; 
				lr.bottom= lr.top + lineheight;
				Nlm_MoveTo(lr.left, lr.top);
			  Nlm_SelectFont(gSmallFont); // this is a HACK !!
				if (gShowDate) { ag->DrawDate(lr); Nlm_GetPen(&pt); lr.left = pt.x + 5;  } 
				if (gShowSize) { ag->DrawSize(lr); Nlm_GetPen(&pt); lr.left = pt.x + 5;  } 
				if (gShowHost) { ag->DrawHost(lr); Nlm_GetPen(&pt); lr.left = pt.x + 5;  } 
				if (gShowPort) { ag->DrawPort(lr); Nlm_GetPen(&pt); lr.left = pt.x + 5;  } 
				if (gShowPath) { ag->DrawPath(lr); Nlm_GetPen(&pt); lr.left = pt.x + 5;  } 
				if (gShowURL)  { ag->DrawURL(lr); Nlm_GetPen(&pt); lr.left = pt.x + 5;  } 
				if (gShowAdmin) { ag->DrawAdmin(lr); Nlm_GetPen(&pt); lr.left = pt.x + 5;  } 
				if (gShowPlus) { ag->DrawPlus(lr); Nlm_GetPen(&pt); lr.left = pt.x + 5;  } 
				}
			
#if USE_MULTI_LINES
			if (gShowAbstract && ag->fAbstract) {
				lr.left= r.left;
				lr.top = lr.bottom; 
				lr.bottom= r.bottom; //lr.top + absHeight; 
				Nlm_MoveTo(lr.left, lr.top);
				ag->DrawAbstract( lr);
				}
#endif

			SelectFont(); // double HACK !!
			}
			break;

		}
}




#if 0
				////////// old view

void DGolistView::SetColWidths()
{
	short icowidth, height, wid;

	//Nlm_WindoW w= Nlm_SavePort( GetNlmObject());
  Nlm_SelectFont(fFont);

	if (fItems && fItems->GetSize() > 0) 
		this->SetTableSize( fItems->GetSize(), 9);
	
	switch (gIconSize) {
		case 0: icowidth= 14; break;
		case 1: icowidth= 20; break;
		default:
		case 2: icowidth= 36; break;
		}
	height= icowidth; //Max( icowidth, Nlm_LineHeight());  // Nlm_FontHeight
	this->SetItemHeight( 0, fMaxRows, height);

	this->SetItemWidth( 0, 1, icowidth); 	// go->WidthIcon(size)
	this->SetItemWidth( 1, 1, 10); 	// go->WidthPlus()
	wid= Nlm_StringWidth("A long title to measure for max width");
	this->SetItemWidth( 2, 1, wid); // go->WidthTitle()
	
	wid= (gShowDate) ? Nlm_StringWidth(" 30Jan94 ") : 0;
	this->SetItemWidth( 3, 1, wid); 	 
	wid= (gShowSize) ? Nlm_StringWidth(" 1234kb ") : 0;
	this->SetItemWidth( 4, 1, wid); 	 
	wid= (gShowHost) ? Nlm_StringWidth(" gopher.some.where.edu ") : 0;
	this->SetItemWidth( 5, 1, wid);  
	wid= (gShowPort) ? 30 : 0;
	this->SetItemWidth( 6, 1, 30); 		 
	wid= (gShowPath) ? Nlm_StringWidth(" /some/path/to/timbuktu ") : 0;;
	this->SetItemWidth( 7, 1, wid); 	 
	wid= (gShowAdmin) ? Nlm_StringWidth(" gtirebiter@some.fancy.mailhost ") : 0;
	this->SetItemWidth( 8, 1, wid);  
	//Nlm_RestorePort( w); // bombing here w/ GetSize()==1 ???
}


void DGolistView::DrawCell(Nlm_RecT r, long row, long col)
{
	// need a DTableView field to handle cell rect insets !
			
	if (col) r.left += 2; // quick fix for cells against each
	DGopher* ag= fItems->GopherAt(row); 
	if (ag) switch (col) {
		case 0: ag->DrawIcon(r, gIconSize); break;   
		case 1: ag->DrawPlus(r); break;
		
		case 2: ag->DrawTitle(r); break;
		
		case 3: ag->DrawDate(r); break;
		case 4: ag->DrawSize(r); break;
		case 5: ag->DrawHost(r); break;
		case 6: ag->DrawPort(r); break;
		case 7: ag->DrawPath(r); break;
		case 8: ag->DrawAdmin(r); break;
		//case 9: ag->DrawKind(r); break;
		}
}
	//////////////// old view
#endif 

