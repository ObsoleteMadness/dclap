// DGopherAskDoc.cp
// d.g.gilbert




#include "DGoAskDoc.h"
#include "DGoDoc.h"
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
#include <DTaskCentral.h>
#include <DURL.h>




//class	DGopherAskDoc : public DWindow

 

enum AskConst {
	kAskRowHeight = 25, 
	kAskViewHeight = kAskRowHeight * 6,
	kAskCol0Width = 50,
	kAskColWidth = 250,
	kAskViewWidth = kAskCol0Width + kAskColWidth,
	kAskScrollTextHeight = 80,
	cNextWind = 22456, cPrevWind, 
	cOkayBut, cCancelBut
};



DGopherAskDoc::DGopherAskDoc(long id, DGopher* itsGopher, 
				DGopherListDoc* itsMainDoc, short itsViewChoice):
		DWindow( id, gApplication, document, -1, -1, -5, -5, NULL, kFreeOnClose),
		fGopher(itsGopher), fMainDoc(itsMainDoc), fViewchoice(itsViewChoice),
		fAskShort(NULL), fAskers(NULL), fControls(NULL),
		fSendBut(NULL), fNextBut(NULL), fPrevBut(NULL),
		fNumWindows(1), fAskFile(NULL), fChosenFile(NULL),
		fPrevW(NULL), fNextW(NULL), fListStart(0), fListEnd(0)
{
	fGopher->newOwner();
	if (!fGopher->fAskers) DGopherListDoc::ReadItemInfo(fGopher); // fetch from host now !?
	if (!fGopher->fAskers) { delete this; return; }
	fAskers= fGopher->fAskers;
	fControls= new DList(); //(NULL, DList::kDeleteObjects); //!? no delete -- ~DView deleting subviews !
	fNumWindows= this->NumOfWindows();
	fIwindow= 1;
	this->BuildWindows();
}

DGopherAskDoc::DGopherAskDoc(long id, DGopherAskDoc* prevWindow, short listStart):
		DWindow( id, gApplication, document, -1, -1, -5, -5, NULL, kFreeOnClose),
		fAskShort(NULL), fAskers(NULL), fControls(NULL),
		fSendBut(NULL), fNextBut(NULL), fPrevBut(NULL),
		fNumWindows(1), fAskFile(NULL), fChosenFile(NULL),
		fPrevW(prevWindow), fNextW(NULL), fListStart(listStart), fListEnd(0)
{
	fGopher=		prevWindow->fGopher;
	fMainDoc=		prevWindow->fMainDoc;
	fViewchoice=prevWindow->fViewchoice;
	fNumWindows=prevWindow->fNumWindows;
	fIwindow=		prevWindow->fIwindow+1;
	fControls= 	prevWindow->fControls;
	fAskers=		prevWindow->fAskers;
	this->BuildWindows();
}


DGopherAskDoc::~DGopherAskDoc()
{
	if (fNextW) {
		delete fNextW; // delete chain of windows 
		fNextW= NULL;
		}
	if (fPrevW) { 
		fGopher= NULL;
		fControls= NULL;
		}
	else { // first window
		if (fGopher) fGopher->suicide();  fGopher= NULL;
		if (fControls) delete fControls;  fControls= NULL;
		}
	if (fAskFile) MemFree(fAskFile);  fAskFile= NULL;
	if (fChosenFile) fChosenFile->suicide(); // case we need suicide() -- fGopher MAY also be owner
}

void DGopherAskDoc::BuildWindows()
{	
	char name[256];
	if (fNumWindows>1) {
		sprintf(name, "Ask (%d/%d) for %s", fIwindow, fNumWindows, fGopher->ShortName());
		}
	else {
		StrCpy( name, "Ask for "); StrNCat( name, fGopher->ShortName(), 255);
		}
	this->SetTitle( name);
		// put these buttons at top of window 
	this->AddOkayCancelButtons(); 
		
	fAskShort= new DGopherAskShort( 0, this, fAskers, fControls);
	fAskShort->InstallControls( fListStart, fListEnd);
	if (fListEnd < fAskers->GetSize()) {
		fNextW= new DGopherAskDoc(0,this,fListEnd);
		}
}

void DGopherAskDoc::Open()
{
	if (!fNextW) {  
		if (fNextBut) fNextBut->Disable();
			// user has seen all windows, let her send it
		DGopherAskDoc* lastw= this;
		while (lastw) {
			if (lastw->fSendBut) lastw->fSendBut->Enable();
			lastw= lastw->fPrevW;
			}
		}
	if (!fPrevW && fPrevBut) fPrevBut->Disable();

	DWindow::Open();
}

void DGopherAskDoc::OkayAction() 
{
	gCursor->watch();
	this->DoReply();
	gCursor->arrow();
}


void DGopherAskDoc::AddOkayCancelButtons()
{
		// we can't have a DDefaultButton here, it eats all Return keys !
	DButton* bb= new DButton( cCancelBut, this, "Cancel");
	//bb->SetResize( DView::fixed, DView::moveinsuper);  
	this->NextSubviewToRight();
	fSendBut= new DButton( cOkayBut, this, "Send");  
	if (fNumWindows > 1) {
		fSendBut->Disable(); // keep disabled until all windows visited !?
		this->NextSubviewToRight();
		fPrevBut= new DButton( cPrevWind, this, "Previous");
		this->NextSubviewToRight();
		fNextBut= new DButton( cNextWind, this, "Next");  
		} 
	this->NextSubviewBelowLeft();
}

void DGopherAskDoc::Close() 
{
	// using fFreeOnClose so this deletes each win
#if 1
		// close only 1st window, its destructor closes&frees all others
	DGopherAskDoc * lastw;
	lastw= this->fPrevW;
	if (lastw) {
	  while (lastw->fPrevW) lastw= lastw->fPrevW;
		lastw->Close();
		}
	else
		DWindow::Close();
#else
	DGopherAskDoc * otherw, * next;
	next= this->fPrevW;
	while (next) {
		otherw= next;
		next= next->fPrevW;
		//otherw->Close();
		}
	next= this->fNextW;
	while (next) {
		otherw= next;
		next= next->fNextW;
		otherw->Close();
		}
	DWindow::Close();
#endif
}

Boolean DGopherAskDoc::IsMyAction(DTaskMaster* action) 
{	
	char  buf[512];

	switch(action->Id()) {

		case kGopherAskAskF : {
			char* filename= (char*)gFileManager->GetOutputFileName(this->fAskFile);
			if (filename) {
				this->fAskFile= StrDup(filename);  
				char* shortName= (char*)gFileManager->FilenameFromPath(this->fAskFile);
				sprintf(buf, "Get: %s", shortName);
				((DView*) action)->SetTitle(buf);  
				// !? need also to resize the button to show all of name !?
				}
			}
			return true;
			
		case kGopherAskChooseF :  {
			//char* filename= (char*)gFileManager->GetInputFileName(".txt","TEXT");
			char* filename= (char*)gFileManager->GetInputFileName(NULL,NULL);
			if (filename) {
				if (this->fChosenFile) delete this->fChosenFile;
				this->fChosenFile= new DFile(filename,"r");
				char* shortName= (char*)gFileManager->FilenameFromPath(filename);
				sprintf(buf, "Send: %s", shortName);
				((DView*) action)->SetTitle(buf); 
				// !? need also to resize the button to show all of name !?
				}
			}
			return true;

		case cNextWind:
			if (fNextW) {
				fNextW->Open();
				this->Hide();
				}
			return true;
			
		case cPrevWind:
			if (fPrevW) {
				fPrevW->Open();
				this->Hide();
				}
			return true;
			
		case cOkayBut:
			this->OkayAction();
			fOkay= true;
			// fall thru to next case
		case cCancelBut: 
			fModal= false;
			this->CloseAndFree();  
			return true;
			
		default:
			return DWindow::IsMyAction(action);	
		}
}


short	DGopherAskDoc::NumOfWindows()
{
	long 	irow, nask, len;	
	short	height= 0;
	short lineheight, buttonheight;
	short screenwidth, screenheight;
	gUtil->screensize( screenwidth, screenheight);
	screenheight -= 40; // don't fill entire screen;

		// some items use gGoviewFont, some Nlm_systemFont
	Nlm_SelectFont(gGoviewFont);
	lineheight= Nlm_LineHeight();  
	//lineheight= 2 + Max(Max(Nlm_stdLineHeight, Nlm_LineHeight()), kAskRowHeight);  
	buttonheight= Nlm_stdLineHeight + 10; // this +10 fiddle varies w/ window_os, but 10 is max
	
	// remove fudge for OKAY/CANCEL and window space
	screenheight -= buttonheight * 2;

	nask= fAskers->GetSize();
	for (irow = 0; irow<nask; irow++) {
		DGopherItemAsk* anAsker= (DGopherItemAsk*) fAskers->At(irow);
		switch (anAsker->fKind) {
			case kGopherAskAsk:		//edittext
			case kGopherAskNote: 	//prompt
				height += lineheight;
				break;
			case kGopherAskChoose: 	// popup	
			case kGopherAskSelect: 	// checkbox
			case kGopherAskChooseF: // button
			case kGopherAskAskF:		// button
				height += buttonheight;
				break;
			case kGopherAskAskP:		// passwordtext
			default : 
				height += Nlm_stdLineHeight;
				break;
			case kGopherAskAskL:		// dialog text
				height += kAskScrollTextHeight + Nlm_hScrollBarHeight + 6;
				break;
			}
		}
	return ((height + screenheight-1) / screenheight);
}




void DGopherAskDoc::DoReply()
{
	enum { cProcessGopher = 30346 };
	DFile* replyFile= NULL;
	char* replies= this->Answers(replyFile);

	DGopher* aGopher = (DGopher*) fGopher->Clone();  
	aGopher->SetAskReply( replies, replyFile);
	aGopher->fViewchoice= fViewchoice;
	aGopher->fStatusLine= NULL; //??
	
	//DTask* aTask= fMainDoc->newTask(cProcessGopher, DGopherListDoc::kGoListdoc, (long) aGopher); 
	//fMainDoc->PostTask( aTask); 
	DTask* aTask= new DTask( cProcessGopher, DGopherListDoc::kGoListdoc,
									 gGopherDoc, (long) aGopher);
	gTaskCentral->AddTask(aTask);
}



char* DGopherAskDoc::Answers(DFile*& replyFile)
{
	// ASK data return is 
	// path<tab>+<tab>1<crlf> 
	// +-1<crlf>
	// data...<crlf>
	// data...<crlf>
	// .<crlf>

	char *ans;
	long	iask, nask;
	char  aline[256], *bline;
	DCluster* clus;
	DView* aControl;
	
	replyFile= NULL;
	ans	= (char*)MemNew(1);
	*ans= 0;
	nask= fAskers->GetSize();
	
	Boolean isHTMLForm= (fGopher->fHasAsk == 3 || fGopher->fHasAsk == 5);
	Boolean isHTMLPostForm= (fGopher->fHasAsk == 5);
		
	for (iask = 0; iask<nask; iask++) {
		DGopherItemAsk* anAsker= (DGopherItemAsk*) fAskers->At(iask);
		DGopherItemAsk* origAsker= NULL;
		origAsker= anAsker;   //??

		aControl= (DView*) fControls->At(iask);
		
		if (anAsker!=NULL && aControl!=NULL) {
			if (isHTMLForm) {
				StrExtendCat( &ans, anAsker->fNametag);
				StrExtendCat( &ans, "=");
				}
				
		 switch (anAsker->fKind) {

			case kGopherAskAsk:	
			case kGopherAskAskP:	
					aControl->GetTitle(aline, 250);
					if (origAsker) origAsker->SetAnswer( aline); // preserve answer for next use
					if (isHTMLForm) {
						bline= DURL::EncodeChars( aline);
						if (isHTMLPostForm) StrCat(bline, CRLF);
						StrExtendCat( &ans, bline);
						MemFree( bline);
						}
					else {
						StrCat(aline, CRLF);
						StrExtendCat( &ans, aline);
						}
					break;
				
			case kGopherAskAskF:	
					// send server file *name* chosen by user for local file
					if (fAskFile) {
						StrNCpy(aline, fAskFile, 250);
						if (origAsker) origAsker->SetAnswer( aline); // preserve answer for next use
						}
					else *aline= 0;
					if (isHTMLForm) {
						// no html equivalent...
						}
					else {
						StrCat(aline, CRLF);
						StrExtendCat( &ans, aline);
						}
					break;
					
			case kGopherAskChoose:	
					{
					short choice;
					//choice=  aControl->GetValue();
					//aControl->GetItemTitle( choice, aline, 250);
					DPopupList* pop= (DPopupList*) aControl;
					pop->GetSelectedItem( choice, aline, 250);
					if (origAsker) origAsker->ItemChoice( choice); // preserve item for next use
					if (isHTMLForm) {
					
						// ! HTML uses valuetags for each radio item
						StrCpy(aline, anAsker->fValuetag);
						
						bline= DURL::EncodeChars( aline);
						if (isHTMLPostForm) StrCat(bline, CRLF);
						StrExtendCat( &ans, bline);
						MemFree( bline);
						}
					else {
						StrCat(aline, CRLF);
						StrExtendCat( &ans, aline);
						}
					}
					break;

			case kGopherAskSelect: 
					if ( aControl->GetStatus() )  
						StrCpy(aline,"1");
					else 
						StrCpy(aline,"0");
					if (origAsker) origAsker->SetAnswer( aline); // preserve answer for next use

					if (isHTMLForm ) {
						if ( aControl->GetStatus() ) {
							// html uses value string for each select/checkbox item
							StrCpy(aline, anAsker->fValuetag);
							bline= DURL::EncodeChars( aline);
							if (isHTMLPostForm) StrCat(bline, CRLF);
							StrExtendCat( &ans, bline);
							MemFree( bline);
							}
						}
						
					else {
						StrCat(aline, CRLF);
						StrExtendCat( &ans, aline);
						}
					break;
					
			case kGopherAskChooseF:
					// this goes to replyFile... ??
					replyFile= fChosenFile; // maybe NULL
					if (replyFile) replyFile->newOwner();
					// does protocol require an empty writeline here for server ???
					break;
					
			case kGopherAskAskL:
					{
					char *txt, *cp, *ep;
					long len, nlines= 0;
					txt= aControl->GetText();
					
					if (isHTMLForm) {
						bline= DURL::EncodeChars( txt);
						if (isHTMLPostForm) StrCat(bline, CRLF);
						StrExtendCat( &ans, bline);
						MemFree( bline);
						}
					else {

					// ! first need to count & write # of lines in txt...				
				 	for (short first= 1; first<=2; first++) {
						for (cp= txt; cp && *cp; ) {
							for (ep=cp; *ep != 0 && *ep != kCR && *ep != kLF; ep++) ;
							len= ep - cp;
							if (ep && len>0) {
								if (first==1) nlines++;
								else {
									if (len > 250) len= 250;
									StrNCpy( aline, cp, len);
									aline[len]= 0;
									StrCat( aline, CRLF);
									StrExtendCat( &ans, aline);
									}
								}
							cp= ep;
							if (cp && *cp) {
								if (*cp == kCR && *(cp+1) == kLF) cp += 2;
								else cp++;
								}
							}
						if (first==1) { 
							sprintf(aline, "%d"CRLF, nlines);
							StrExtendCat( &ans, aline);
							}
						}
					}
					MemFree(txt);
					}
					break;
					
			case kGopherAskNote: 
			default : 
					break;
			}
		 }
		}
	
	return ans;
}







// class DGopherAskShort

DGopherAskShort::DGopherAskShort(long id, DGopherAskDoc* itsSuperior, 
	DList* itsAskers, DList* itsControls) :
	DCluster( id, itsSuperior, -1, -1, true, NULL),
	fAskers(itsAskers),
	fDoc(itsSuperior),
	fControls(itsControls)
{
	this->SetResize( DView::matchsuper, DView::relsuper); //??
}


DGopherAskShort::~DGopherAskShort()
{
}


Boolean DGopherAskShort::IsMyAction(DTaskMaster* action) 
{
	switch(action->Id()) {

		default:
			return DCluster::IsMyAction(action);

		}
}



void	DGopherAskShort::InstallControls(short startControl, short& endControl)
{
	short 			irow, nask;
	short 			lineheight, maxlineheight, dialogheight, screenwidth, screenheight;
	DView			* super, *aview;
	DCluster	* cluster1;
	DPrompt		* pr;
	DGopherItemAsk * anAsker;
	Nlm_PoinT		nextpos;
	Nlm_RecT 		aloc;
	
	gUtil->screensize( screenwidth, screenheight);
	screenheight -= 40; // don't fill entire screen;

	Nlm_SelectFont(gGoviewFont);
	lineheight= Nlm_LineHeight();  
	maxlineheight= 10 + Max(Nlm_stdLineHeight, lineheight); //??
	this->GetNextPosition(&nextpos);
	dialogheight= nextpos.y + maxlineheight;
	 
	nask= fAskers->GetSize();
	super= this;
	
	for (irow = startControl; irow<nask && dialogheight<screenheight; irow++) {
		endControl= irow+1;
		
			// need this stupid extra for formatting ?
		cluster1 = new DCluster( 0, super, 0,0, true);
		cluster1->SetResize( DView::relsuper, DView::fixed); // not for all, only some !
		super    = cluster1;

		anAsker= (DGopherItemAsk*) fAskers->At(irow);

		pr= new DPrompt( 0, super, anAsker->fQuestion, 0, 0, gGoviewFont);	 		
		super->NextSubviewToRight();
		aview= NULL;

		switch (anAsker->fKind) {
		
			case kGopherAskAskP:	
				aview = new DPasswordText(anAsker->fKind, super, NULL, 15);
				aview->SetTitle(anAsker->fAnswer);
				aview->SetResize( DView::relsuper, DView::fixed);
				break;
	
			case kGopherAskAsk:	
				aview = new DEditText(anAsker->fKind, super, NULL, 15, gGoviewFont);
				aview->SetTitle(anAsker->fAnswer);
				aview->SetResize( DView::relsuper, DView::fixed);
				break;
					
			case kGopherAskAskL:
				{
				short width = 20;  
				short height= (kAskScrollTextHeight + lineheight-1) / lineheight; 
				aview = new DDialogScrollText(anAsker->fKind, super, width, height, gTextFont, false);
				aview->SetTitle( anAsker->fAnswer);
				aview->SetResize( DView::relsuper, DView::fixed);
				}
				break;

			case kGopherAskChooseF:
			case kGopherAskAskF:	
				{
				char* name = anAsker->fAnswer;
				if (name == NULL || *name == 0) {
					if (anAsker->fKind == kGopherAskAskF) name= "New file from server";
					else name= "Choose file to send";
					}
				aview = new DButton( anAsker->fKind, super, name);
				}
				break;
									

			case kGopherAskSelect:
				{
				Boolean ison= false;
				char* answer = anAsker->fAnswer;
				if (answer) {
					while (*answer && isspace(*answer)) answer++;
					ison= (*answer == '1' || *answer == 'T' || *answer == 't');
					}
				else {
					//  UMinn example select doesn't use TAB to separate quest & answer
					//  strings like other ASK items, uses ':' !!!
					answer= StrChr(anAsker->fQuestion,':');
					if (answer) {
						answer++;
						ison= (*answer != '0');
						}
					}
				aview = new DCheckBox( anAsker->fKind, super, NULL);
				aview->SetStatus( ison);
				}
				break;
							
			
			case kGopherAskChoose:	
				{
				char *cp, *ep;
				Boolean done;
				DPopupList* aPopup = new DPopupList(anAsker->fKind, super, true);
				
				char *words= StrDup( anAsker->fAnswer);
				for (cp= words, done=false; !done && *cp!=0; ) {
					ep= strchr( cp, '\t');
					if (!ep) {
						ep=strchr( cp, '\0');
						done= true;
						}
				  if (ep) {
						*ep= 0;
						aPopup->AddItem( cp);
						if (!done) cp= ep+1;
						}
					else
						done= true;
					}
				MemFree( words);
				aPopup->SetValue(1);
				aview= aPopup;
				}
				break;
				
			case kGopherAskNote: 
			default : 
				aview = new DPrompt( anAsker->fKind, super, anAsker->fAnswer, 0, 0, gGoviewFont);
				//aview->SetResize( DView::relsuper, DView::fixed);
				break;
			}
			
		if (aview) fControls->InsertLast(aview);
		super= this;
		super->NextSubviewBelowLeft();
		super->GetNextPosition(&nextpos);
		dialogheight= nextpos.y + maxlineheight;
		}
		
}









#ifdef THIS_IS_OBSOLETE

///////////////////////////////////////////////////////////////////////
//
// 				THIS DIALOG W/ SCROLLING CONTROLS ISN'T WORKING
//        VIBRANT ISN'T UP TO MANAGING CONTROLS THAT ARE SCROLLED
//        IN&OUT OF A VIEW.    Try multiple windows instead.
//
///////////////////////////////////////////////////////////////////////



// class DGopherAskView
class DGopherAskView : public DTableView, public DKeyCallback
{
public:
	DList* fControls;
	DList* fAskers;
	DDialogText*	fEditText;
	DGopherAskDoc*	fDoc;
	
	DGopherAskView(long id, DGopherAskDoc* itsSuperior, DList* itsAskers, short pixwidth, short pixheight);
	virtual ~DGopherAskView();

	virtual Boolean IsMyAction(DTaskMaster* action);
	virtual void  InstallControls(DList* theControls);
	
	virtual void Click(Nlm_PoinT mouse);	
	virtual void CharHandler(char c);
  virtual void ProcessKey(char c);
	virtual void SetEditText(DDialogText* theText);
	virtual void DrawRow(Nlm_RecT r, short row);
	virtual void SetClusterPositions();
	virtual void Scroll(Boolean vertical, DView* scrollee, short newval, short oldval);
	
	//virtual void Drag(Nlm_PoinT mouse);
	//virtual void Hold(Nlm_PoinT mouse);
	//virtual void Release(Nlm_PoinT mouse);
};



class  DAskEditText : public DEditText
{
public:
	DGopherAskView* fAskView;
	DAskEditText( DGopherAskView* itsView, long id, DView* superior, char* name, short width) :
		DEditText( id, superior, name, width),
		fAskView(itsView) 
		{ }
	virtual void selectAction() { fAskView->SetEditText(this); }
	virtual void deselectAction() { fAskView->SetEditText(NULL); }
};

class  DAskPasswordText : public DPasswordText
{
public:
	DGopherAskView* fAskView;
	DAskPasswordText( DGopherAskView* itsView, long id, DView* superior, char* name, short width) :
		DPasswordText( id, superior, name, width),
		fAskView(itsView) 
		{ }
	virtual void selectAction() { fAskView->SetEditText(this); }
	virtual void deselectAction() { fAskView->SetEditText(NULL); }
};

class  DAskScrollText : public DDialogScrollText
{
public:
	DGopherAskView* fAskView;
	DAskScrollText( DGopherAskView* itsView, long id, DView* superior,
			short width, short height, Nlm_FonT font, Boolean wrap) :
		DDialogScrollText( id, superior, width, height, font, wrap),
		fAskView(itsView) 
		{ }
	virtual void selectAction() { fAskView->SetEditText(this); }
	virtual void deselectAction() { fAskView->SetEditText(NULL); }
};



DGopherAskView::DGopherAskView(long id, DGopherAskDoc* itsSuperior, DList* itsAskers,
				short pixwidth, short pixheight):
		DTableView( id, itsSuperior, pixwidth, pixheight, 2/*nrows*/, 2/*cols*/, 
							kAskColWidth/*itemwidth*/, kAskRowHeight/*itemheight*/),
		fControls(NULL),
		fEditText(NULL),
		fDoc(itsSuperior),
		fAskers(itsAskers)
{
	// FailNIL(fAskers);
	this->SetSlateBorder(false);
	this->SetResize( DView::matchsuper, DView::relsuper);
}

DGopherAskView::~DGopherAskView()
{

}



void DGopherAskView::SetEditText(DDialogText* theText)
{
	if (!theText && fEditText) {
		fEditText->SetSelection(0,0); // no other deselect method?
		}
	fEditText= theText; 
	GetWindow()->SetEditText(fEditText); 
	if (fEditText) {
		fEditText->Enable(); // not needed?
		fEditText->Select(); 
		}
}


void DGopherAskView::CharHandler(char c) 
{
		// this is a vibrant SlatE callback -- not called
	this->ProcessKey(c);
}

void DGopherAskView::ProcessKey(char c) 
{
		// this works -- general key callback from vibrant,
		// but the above vibrant SlatE callback doesn't get called
#ifdef WIN_MAC	
	if (fEditText) { 
		Nlm_GraphiC  g = (Nlm_GraphiC)fEditText->GetNlmObject();
		Nlm_GphPrcsPtr classPtr= Nlm_GetClassPtr( g);
		if (classPtr) {
			Nlm_BGphChr keyproc= classPtr->key;
			if (keyproc && (*keyproc)(g, c)) ;
			}
		}
#endif
}

void DGopherAskView::Click(Nlm_PoinT mouse)
{
	short row, col;
	PointToCell( mouse, row, col);
	DView* ac= (DView*) fControls->At(row); 
	if (!ac) return;
	
	DView* subview = NULL;
	if ( (subview = ac->FindSubview(kGopherAskAsk )) != NULL
	  || (subview = ac->FindSubview(kGopherAskAskP)) != NULL
	  || (subview = ac->FindSubview(kGopherAskAskL)) != NULL )
	  	{
			fEditText= (DDialogText*) subview;
			this->GetWindow()->SetEditText(fEditText); 
			fEditText->Select(); 
			this->SetKeyHandler(this);
			}
			
#ifdef WIN_MAC
	{
	Nlm_GraphiC  		g = (Nlm_GraphiC)ac->GetNlmObject();
	Nlm_GphPrcsPtr 	classPtr  = Nlm_GetClassPtr( g);
	if (classPtr) {
		Nlm_BGphPnt 		groupclick= classPtr->click;
		if (groupclick && (*groupclick)(g, mouse)) ;
		}
	}
#endif
}

void DGopherAskView::DrawRow(Nlm_RecT r, short row)
{
	Nlm_InsetRect(&r, 2, 2);
  Nlm_SelectFont(gGoviewFont);
	
	// draw both text in col0 and control in col1 !!
	DGopherItemAsk* anAsker= (DGopherItemAsk*) fAskers->At(row);
	if (anAsker && anAsker->fQuestion) {
		Nlm_DrawString( &r, anAsker->fQuestion, 'l', false);
		r.left += 5 + Min(500, Max( kAskCol0Width, Nlm_StringWidth(anAsker->fQuestion)));
		}
		
	DView* ac= (DView*) fControls->At(row); 
	if (ac) {
#if 0
				// having problems w/ control getting drawn as big as IT wants, not clipped to this view...
				// this ain't helping
	  Nlm_RecT clipr;
		Nlm_SectRect( &r, &Nlm_updateRect, &clipr);
		Nlm_ClipRect( &clipr);
#endif
		ac->SetPosition( r);
		ac->Show();
		}
}

void DGopherAskView::SetClusterPositions()
{
	Nlm_RecT r;
#if 1
  Nlm_SelectFont(gGoviewFont);
#endif
	for (short arow= fTop; arow< fMaxRows; arow++ ) {
#if 1
		GetRowRect( arow, r);
		DGopherItemAsk* anAsker= (DGopherItemAsk*) fAskers->At(arow);
		if (anAsker && anAsker->fQuestion) {
			r.left += 5 + Min(500, Max( kAskCol0Width, Nlm_StringWidth(anAsker->fQuestion)));
			}
#else
		GetCellRect( arow, 1, r);
#endif
		if (Nlm_RectInRect( &r, &fRect)) {
			DView* ac= (DView*) fControls->At(arow); 
			if (ac) {
				Nlm_InsetRect( &r, 2, 2);
				ac->SetPosition( r);
				Nlm_ValidRect( &r); // ??
				}
			}
		}
}

void DGopherAskView::Scroll(Boolean vertical, DView* scrollee, short newval, short oldval)
{
	DTableView::Scroll(vertical, scrollee, newval, oldval);
	this->SetClusterPositions(); // !! need this if default scroll is used that scrolls bitmap!
}


Boolean DGopherAskView::IsMyAction(DTaskMaster* action) 
{
	char  buf[512];
	
	switch(action->Id()) {

		case kGopherAskAskF : {
			char* filename= (char*)gFileManager->GetOutputFileName(fDoc->fAskFile);
			if (filename) {
				fDoc->fAskFile= StrDup(filename);  
				char* shortName= (char*)gFileManager->FilenameFromPath(fDoc->fAskFile);
				sprintf(buf, "Get: %s", shortName);
				((DView*) action)->SetTitle(buf);  
				// !! need also to resize the button to show all of name !
				}
			}
			return true;
			
		case kGopherAskChooseF :  {
			char* filename= (char*)gFileManager->GetInputFileName(".txt","TEXT");
			if (filename) {
				if (fDoc->fChosenFile) delete fDoc->fChosenFile;
				fDoc->fChosenFile= new DFile(filename,"r");
				char* shortName= (char*)gFileManager->FilenameFromPath(filename);
				sprintf(buf, "Send: %s", shortName);
				((DView*) action)->SetTitle(buf); 
				// !! need also to resize the button to show all of name !
				}
			}
			return true;

		default:
			return DTableView::IsMyAction(action);
			break;
		}
}



void	DGopherAskView::InstallControls(DList* theControls)
{
	long 				irow, nask, len;
	Nlm_RecT 		aloc;
	DCluster	* cluster = NULL;
	DView				*super, *aview;
	char				*defstring, *kPlaceholder = "             "; // 13 spaces
	
	fControls= theControls;
	nask= fAskers->GetSize();
	this->SetTableSize( nask, 2);

	aloc.bottom = 20;
	for (irow = 0; irow<nask; irow++) {
		aview= NULL;
		DGopherItemAsk* anAsker= (DGopherItemAsk*) fAskers->At(irow);
		this->GetCellRect( irow, 1, aloc); //?? how many cols ??

			// !! MUST HAVE DCluster() for popups to work !
			// edittext still won't take keypresses...
		cluster= new DCluster( 0, this, 100, 50, true, NULL);
		cluster->SetResize( DView::relsuper, DView::fixed); // not for all, only some !
		super= cluster;

		switch (anAsker->fKind) {
		
			case kGopherAskAskP:	
				aview = new DAskPasswordText( this, anAsker->fKind, super, kPlaceholder, 15);
				aview->SetTitle(anAsker->fAnswer);
				aview->SetResize( DView::relsuper, DView::fixed);
				break;
	
			case kGopherAskAsk:	
				aview = new DAskEditText( this, anAsker->fKind, super, kPlaceholder, 15);
				aview->SetResize( DView::relsuper, DView::fixed);
				aview->SetTitle(anAsker->fAnswer);
				break;
					
			case kGopherAskAskL:
				{
				this->SetItemHeight( irow, 1, kAskScrollTextHeight + Nlm_hScrollBarHeight + 6);
				aloc.bottom = aloc.top + kAskScrollTextHeight;
						// !! these sizes are in stdCharWidth chars !!! not pixels
				aloc.right -=	Nlm_vScrollBarWidth;
				short width = (aloc.right - aloc.left) / Nlm_stdCharWidth;  
				short height= (aloc.bottom - aloc.top) / Nlm_stdLineHeight; 
				aview = new DAskScrollText( this, anAsker->fKind, super, width, height, gTextFont, false);
				aview->SetTitle( anAsker->fAnswer);
				aview->SetResize( DView::relsuper, DView::fixed);
				}
				break;

			case kGopherAskChooseF:
			case kGopherAskAskF:	
				{
				char* name = anAsker->fAnswer;
				if (name == NULL || *name == 0) {
					if (anAsker->fKind == kGopherAskAskF) name= "New file from server";
					else name= "Choose file to send";
					}
				aview = new DButton( anAsker->fKind, super, name);
				}
				break;
									

			case kGopherAskSelect:
				{
				Boolean ison= false;
				char* answer = anAsker->fAnswer;
				if (answer) {
					while (*answer && isspace(*answer)) answer++;
					ison= (*answer == '1' || *answer == 'T' || *answer == 't');
					}
				else {
					//  UMinn example select doesn't use TAB to separate quest & answer
					//  strings like other ASK items, uses ':' !!!
					answer= StrChr(anAsker->fQuestion,':');
					if (answer) {
						answer++;
						ison= (*answer != '0');
						}
					}
				aview = new DCheckBox( anAsker->fKind, super, NULL);
				aview->SetStatus( ison);
				}
				break;
							
			
			case kGopherAskChoose:	
				{
				char *cp, *ep;
				Boolean done;
				DPopupList* aPopup = new DPopupList(anAsker->fKind, super, true);
				
				char *words= StrDup( anAsker->fAnswer);
				for (cp= words, done=false; !done && *cp!=0; ) {
					ep= strchr( cp, '\t');
					if (!ep) {
						ep=strchr( cp, '\0');
						done= true;
						}
				  if (ep) {
						*ep= 0;
						aPopup->AddItem( cp);
						if (!done) cp= ep+1;
						}
					else
						done= true;
					}
				MemFree( words);
				aPopup->SetValue(1);
				aview= aPopup;
				}
				break;
				
			case kGopherAskNote: 
			default : 
				aview = new DPrompt( anAsker->fKind, super, anAsker->fAnswer, 0, 0, gTextFont);
				aview->SetResize( DView::relsuper, DView::fixed);
				break;

			}
			
		if (aview) {
			if (cluster) fControls->InsertLast(cluster);
			else fControls->InsertLast(aview);
			//this->AddSubordinate( aview); //??? test utility of this
			}
		}
		
	//this->SetClusterPositions(); //??
#if 0		
	Nlm_RecT brect;
	this->GetPosition( brect);
	brect.bottom = Min( brect.bottom, aloc.bottom+2+Nlm_hScrollBarHeight);
	this->SetPosition( brect);
#endif
}

#endif  // OBSOLETE


