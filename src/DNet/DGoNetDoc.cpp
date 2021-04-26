// DGopherDoc.cp
// d.g.gilbert



#include "DGoNetDoc.h"

#include <DGoDoc.h>

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
#include <DIconLib.h>  
#include <DIcons.h> 

#include <DChildApp.h>

#include <DRichViewNu.h>
#include <DRTFHandler.h>
#include <DHTMLHandler.h>
#include <DPICTHandler.h>
#include <DGIFHandler.h>

// DAMN SUN CC/LD can't find DNetHTML objects !
#include <DNetHTMLHandler.h>
 
 
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
 
 
//Global 	DList*	gTextStoreList = NULL;
//Local		short		gListAt = kEmptyIndex;
Global  DGopherTextDoc* gGopherTextDoc = NULL;

Local unsigned long gLinkcolor = (0<<16) + (170<<8) + 0;
															  // red      green    blue


enum { kGolistViewWidth = 225 };

enum DGoDocCmds {
	cProcessGopher = 30346,
	cStuf,
	cStufInline,
	cSave,
	cAdd,
	cProcessInlines
	};
 
Local Nlm_FonT	gSmallFont= NULL;


inline void MakeSmallFont()
{
	if (!gSmallFont) 
		gSmallFont = Nlm_GetFont( "Times", 9, false, false, false, "Roman");
}




class DLinkedRichView : public DRichView
{
public:
	enum MarkState {
		kMarkNone, kMarkMoving, kMarkOff, kMarkOn
		};
	Nlm_PoinT	fAnchorPt, fEndPt;
	MarkState fMarqueeState;
	DGoLinkedTextDoc * fDoc;
	
	DLinkedRichView(long id, DGoLinkedTextDoc* itsSuperior, short pixwidth, short pixheight);

	virtual void DoClickFetch(Nlm_PoinT mouse);
	virtual void Click(Nlm_PoinT mouse);
	virtual void Drag(Nlm_PoinT mouse);
	virtual void Hold(Nlm_PoinT mouse);
	virtual void Release(Nlm_PoinT mouse);
	virtual void DrawMarquee( MarkState state = kMarkMoving);
};


DLinkedRichView::DLinkedRichView(long id, DGoLinkedTextDoc* itsSuperior, short pixwidth, short pixheight):
	DRichView( id, itsSuperior, pixwidth, pixheight),
	fDoc( itsSuperior),
	fMarqueeState(kMarkNone)
{
	Nlm_LoadPt( &fAnchorPt, 0, 0);
	fEndPt= fAnchorPt;
}




void DLinkedRichView::Click(Nlm_PoinT mouse)
{
	if (fMarqueeState == kMarkOn) DrawMarquee( kMarkOff);
	if (Nlm_dblClick) {
		if (gDoubleClicker) DoClickFetch( mouse);
		//else DoubleClick(mouse);
		}
	else {
		fAnchorPt= mouse;
		fEndPt= mouse;
		if (gSingleClicker) DoClickFetch( mouse);
		else SingleClick(mouse);		
		}
}


void DLinkedRichView::DrawMarquee( MarkState state)
{
	if (state != kMarkNone && ! Nlm_EqualPt(fAnchorPt, fEndPt)) {
		Nlm_RecT r;
		r.left= MIN(fAnchorPt.x, fEndPt.x);
		r.right= MAX(fAnchorPt.x, fEndPt.x);
		r.top= MIN(fAnchorPt.y, fEndPt.y);
		r.bottom= MAX(fAnchorPt.y, fEndPt.y);
		switch (state) {
			case kMarkMoving:
				Nlm_InvertMode();
				Nlm_Dotted();
				break;
			case kMarkOff:
				Nlm_InvertMode();
				break;
			case kMarkOn:
				Nlm_InvertMode();
				break;
			}
		Nlm_FrameRect( &r);
		switch (state) {
			case kMarkMoving:
				Nlm_Solid();
				Nlm_CopyMode();
				fMarqueeState= kMarkMoving;
				break;
			case kMarkOff:
				Nlm_CopyMode();
				fMarqueeState= kMarkNone;
				break;
			case kMarkOn:
				Nlm_CopyMode();
				fMarqueeState= kMarkOn;
	 			break;
			}
		}
}


void DLinkedRichView::Hold(Nlm_PoinT mouse)
{
		// for marquee selection ??
	//DrawMarquee(kMarkMoving);
	//fEndPt= mouse;
	//DrawMarquee(kMarkMoving);
}

void DLinkedRichView::Drag(Nlm_PoinT mouse)
{
		// for marquee selection
	DrawMarquee(kMarkMoving);
	fEndPt= mouse;
		// !! need to scroll doc if mouse is outside of doc view, but in doc area
	DrawMarquee(kMarkMoving);
}


void DLinkedRichView::Release(Nlm_PoinT mouse)
{
	if (fMarqueeState != kMarkNone) {
			// don't mark things if this was just a simple, non-dragged click
			// for marquee selection
		DrawMarquee(kMarkMoving);
		fEndPt= mouse;
#if 1
		Nlm_RecT r;
		r.left= MIN(fAnchorPt.x, fEndPt.x);
		r.right= MAX(fAnchorPt.x, fEndPt.x);
		r.top= MIN(fAnchorPt.y, fEndPt.y);
		r.bottom= MAX(fAnchorPt.y, fEndPt.y);
	
		// ClearSelection();
		if (r.right > r.left + 5 || r.bottom > r.top + 5)
			MarkSelection( r);
#else
		DrawMarquee(kMarkOn);
#endif
	}
}

 

void DLinkedRichView::DoClickFetch(Nlm_PoinT mouse)
{
	short item, row;
	Nlm_RecT		rct;
	char				wordAt[512];
	DRichStyle	styleAt;

	wordAt[0]= 0;
	MapPoint( mouse, item, row, &rct, wordAt, 512, &styleAt);
	if (styleAt.ispict && styleAt.fPict) {
		Nlm_PoinT atp;
		short	linkid, kind = 0;
		atp= mouse;
		MapView2Doc( atp);
    	// if atp is inside a pict, adjust it by pict top,left
		atp.x -= fDrawr.left;
		atp.y	-= fDrawr.top;

		linkid= styleAt.fPict->GetLink( atp, kind); 
				
		if (kind == DPictStyle::kPictNetPic)
			fDoc->SetupInlinePart( fMapStyle); 
		else if (linkid) {
			// if styleAt.ismap, do things w/ gopher link
			// need to stuff atp.x,y into gopher.fQuery....
			if (styleAt.ismap) fDoc->OpenGopherLink(linkid-1, &atp);	
			else fDoc->OpenGopherLink(linkid-1, NULL);	// send atp anyway ???
			}
		else if (fDoc->fStore->fMapLink) {
			fDoc->OpenGopherLink( fDoc->fStore->fMapLink-1, &atp);	
			}
		else {
			fDoc->SetupInlinePart( fMapStyle); // ?? check in case it needs pic fetched
			}
		}
	else {
		if (styleAt.linkid) fDoc->OpenGopherLink(styleAt.linkid-1);	
		}
	styleAt.ownpict= false; // !!!! not this copy, so we don't delete
}
















// class	DGopherTextStore : public DTaskMaster



DGopherTextStore::DGopherTextStore( DGopher* itsGopher, DWindow* itsWindow, Boolean havelinks) :
		DTaskMaster( 0, gApplication),
		fWindow(itsWindow), fIsLinkDoc(havelinks),
		fParentGo(itsGopher), fStufGo(NULL),
		fRichView(NULL),  
		fDocFormat(DRichHandler::kTextformat),
		fRichHandler(NULL), 
		fInlineStyle(NULL),
		fDocFile(NULL), fFetchFile(NULL),
		fGolist(NULL), fIsFromFile(false), fMapLink(0),
		fFirststuff(1)
{ 
	short pixwidth = gPrintManager->PageWidth(), pixheight = 0;
	
	if (fParentGo) {
		fParentGo->newOwner();
		if (fParentGo->fType == kTypeHtml) fDocFormat= DHTMLHandler::kHTMLformat;
		}
	fFetchFile= new DTempFile();
	//fFetchFile->Open("wb"); //! << WITHOUT THIS, save IS BAD for Binary data
 
#if 1	
	if (((DGopherTextDoc*)fWindow)->fTextStoreList->GetSize()) {
			// already have a view in window, use same size
		Nlm_RecT r;
		((DGopherTextDoc*)fWindow)->fRichView->GetPosition( r);
		pixwidth=  r.right - r.left;
		pixheight= r.bottom- r.top;
				// need this for all systems
		pixwidth -= 16; // Damn, this is scrollbar width, why do we need this adjustment !?
		}
	else if (!Nlm_EmptyRect( &DRichTextDoc::gRichDocRect)) {
		Nlm_PoinT  npt;
		//fWindow->GetNextPosition( &npt);
		pixwidth= DRichTextDoc::gRichDocRect.right - DRichTextDoc::gRichDocRect.left
				+ 20; //- npt.x;
		pixheight= DRichTextDoc::gRichDocRect.bottom - DRichTextDoc::gRichDocRect.top
				+ 8; //- npt.y;
		}
#endif		
	if (fIsLinkDoc)
		fRichView= new DLinkedRichView(0, (DGoLinkedTextDoc*)fWindow, pixwidth, pixheight);
	else
	  fRichView= new DRichView(0, fWindow, pixwidth, pixheight);
	fRichView->SetSlateBorder( true);
	fRichView->SetResize( DView::matchsuper, DView::relsuper);
	fRichView->SetTabs( gTextTabStops);
	//fRichView->SizeToSuperview( fWindow, true, false); // this needs to adjust for if view has scrollbar !
}
		

DGopherTextStore::~DGopherTextStore()
{
	this->Close();
	if (fGolist) fGolist->suicide(); //!
	if (fParentGo) fParentGo->suicide();  
  if (fRichView) fRichView->ResetDoc(); // drop all data, but leave view for deleteSubviews
	// ?? ^^ ResetDoc is a memory sieve !!???!  doesn't free any of the view mem ?? or is bug elsewhere
	if (fRichHandler) delete fRichHandler; fRichHandler= NULL;
	if (fDocFile) delete fDocFile; fDocFile= NULL;
	if (fFetchFile) delete fFetchFile; fFetchFile= NULL;
}

Boolean DGopherTextStore::suicide(void) 
{ 
	if (GetOwnerCount() <= 1) { 
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide();
}

Boolean DGopherTextStore::suicide(short ownercount)
{
	if (ownercount < 1) {
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide(ownercount);
}

#if 0
void DGopherTextStore::AddToList()
{
	if (!gTextStoreList) {
		gTextStoreList= new DList(NULL, DList::kDeleteObjects);
		gListAt= kEmptyIndex;
		}
	if (gTextStoreList) {
			// clean out storage below current displayed item...
			// ???? is this going to be a mess w/ different windows ??
	  for (short idoc= gTextStoreList->GetSize()-1; idoc>gListAt; idoc--)
	  	gTextStoreList->AtDelete(idoc); // deletes all of storage
		gTextStoreList->Push( this);
		gListAt= gTextStoreList->GetSize()-1;
		}
}
#endif

void DGopherTextStore::SetGolist( DGopherList *theList)
{
	fGolist= theList;
	if (fParentGo && fParentGo->fGoMenuBlock) {
		fGolist= fParentGo->fGoMenuBlock->fGolist;
		if (fGolist) fGolist->newOwner();
		}
		
	if (fParentGo && fParentGo->fIsLocal) {
			// look for a separate "path.go4" link file
		DFile* goFile= NULL;
		char	name[512];
		StrNCpy( name, fParentGo->GetPath(), 512);
		gFileManager->ReplaceSuffix( name, 512, DGopherListDoc::kGopherDocSuffix);
		if (gFileManager->FileExists( name))  
			goFile= new DFile(name, "r");
		if (goFile) {
			ulong filelen;
			goFile->OpenFile();
			goFile->GetDataLength(filelen);
			char* buf= (char*) MemNew( filelen+1);
			goFile->ReadData( buf, filelen);
			fGolist->ReadLinks( buf, filelen);
			MemFree( buf);
			goFile->CloseFile();
			delete goFile;
			}
		}
		
	if (fGolist) fGolist->fParentMenu= fParentGo;
}

void DGopherTextStore::WriteDoc( DFile* toFile, char* readkind, char* writekind)
{
	enum { kLineMax = 2048 };
	char	 line[kLineMax];
	ulong count, bytesleft;
	
	if (!fDocFile) {
		if (!fFetchFile) return;
		fDocFile= fFetchFile;
		fFetchFile= new DTempFile();
		}
	fDocFile->Open(readkind);
	fDocFile->GetDataLength(bytesleft);
	toFile->Open(writekind);
	while (bytesleft>0) {
		count= MIN(kLineMax, bytesleft);
		fDocFile->ReadData(line, count);
		bytesleft -= count;
		toFile->WriteData(line, count);
		}
	toFile->Close();
	fDocFile->Close();
}


const char* DGopherTextStore::GetTitle()
{
	static char buf[256];
	if (fParentGo) return fParentGo->ShortName();
	else if (fStufGo) return fStufGo->ShortName();
	else if (fRichView) return fRichView->GetTitle( buf, 256); 
	else return "";
}

void DGopherTextStore::StartInlineFetch( DGopher* stufGo, DRichStyle* stufStyle)
{
	fStufGo= stufGo;
	fInlineStyle= stufStyle;
	fFirststuff= 1;
	if (!fDocFile && fFetchFile) {
		fDocFile= fFetchFile;
		fFetchFile= new DTempFile();
		}
}

Boolean DGopherTextStore::IsMyTask(DTask* theTask) 
{
	if (theTask->fKind == DGopherTextDoc::kGoTextdoc) {
		ProcessTask( theTask);
		return true;
		}
	else 
		return DTaskMaster::IsMyTask(theTask);
}


ulong CRLF2Newline( char* databuf, ulong datasize)
{
	ulong  oldsize = 0;
	char lastc = 0, * newp = databuf, * oldp = databuf;
	
	while (oldsize < datasize) {
		if (*oldp == kLF && lastc == kCR) {
			newp[-1]= '\n'; // system newline
			lastc= kLF;
			oldp++;
			oldsize++;
			}
		else {
			*newp++= lastc= *oldp++;
			oldsize++;
			}
		}
	*newp= 0;
	return newp - databuf;
} 
 
char* DGopherTextStore::ProcessTask1( char* databuf, ulong& datasize) 
{
	char *dataAt;

	fFirststuff++;	
	
	if (!fFetchFile) fFetchFile= new DTempFile();
	fFetchFile->Open("wb");  
	fRichHandler = new DRTFHandler(fRichView, fFetchFile);
	dataAt= fRichHandler->IsRich( databuf, datasize);
	if (dataAt) {
		fDocFormat= fRichHandler->Format();
		return dataAt;
		}
	delete fRichHandler;

	fRichHandler = new DPICTHandler(fRichView, fFetchFile);
	dataAt= fRichHandler->IsRich( databuf, datasize);
	if (dataAt) {
		fDocFormat= fRichHandler->Format();
		fStufGo->fTransferType= kTransferBinary;  // ! this mayn't be safe ?!
		return dataAt;
		}
	delete fRichHandler;

	fRichHandler = new DGIFHandler(fRichView, fFetchFile);
	dataAt= fRichHandler->IsRich( databuf, datasize);
	if (dataAt) {
		fDocFormat= fRichHandler->Format();
		fStufGo->fTransferType= kTransferBinary;  // ! this mayn't be safe ?!
		return dataAt;
		}
	delete fRichHandler;
				
	fRichHandler = new DNetHTMLHandler(fRichView, fFetchFile, fGolist);
#if 0
	// ?? not always ??
	if (fStufGo && fStufGo->fProtocol == DGopher::kHTTPprot)
		dataAt= databuf;
	else
#endif
		dataAt= fRichHandler->IsRich( databuf, datasize);
	if (dataAt) {
		fDocFormat= fRichHandler->Format();
		return dataAt;
		}
	delete fRichHandler;

	fRichHandler = new DRichHandler(fRichView, fFetchFile);
	fDocFormat= fRichHandler->Format();
	fStufGo->fTransferType= kTransferText; // fix problems caused by binary test
	datasize= CRLF2Newline( databuf, datasize); 
	fStufGo->fInfoSize= datasize;	// ??	
	return databuf; 
}


void DGopherTextStore::ProcessTask(DTask* theTask) 
{
	if (theTask->fNumber == cStuf || theTask->fNumber == cStufInline) {
 
 		Boolean gotInline= (theTask->fNumber == cStufInline);

		if ( fStufGo 
#if 0
			&& (fStufGo->fInfoSize || fStufGo->ThreadProgress() != DGopher::kThreadDoneReading) 
#endif
			) {
			char * cbeg, * cend, clineend;
			Boolean update= true, endOfData;
			short lastItemCount=0, lastLineCount;
			ulong dataRemaining, oldsize;
			
			endOfData= fStufGo->ThreadProgress() == DGopher::kThreadDoneReading;
			fStufGo->ShowProgress();			
			cbeg = fStufGo->fInfo; 
			ulong csize= fStufGo->fInfoSize;
			if (cbeg==NULL || csize==0) {
				if (! (endOfData && fRichHandler) ) return; 
				}
			cend = cbeg + csize; // - 1; 
			clineend = *LineEnd; 
			
			if (!fRichHandler || fFirststuff == 1) {
				oldsize= csize;
				char* cp= ProcessTask1(cbeg, csize);
				fRichHandler->fTasknum= theTask->fNumber - cStuf;
				if (!cp) return; 
				else if (cp > cbeg) {
					if (cp > cend) csize= 0; 
					else csize= cend - cp; // + 1; 
					Nlm_MemMove( cbeg, cp, csize);
					cend= cbeg + csize; // - 1;
					fStufGo->fInfoSize= csize;		
					}
				else if (oldsize != csize) {
					fStufGo->fInfoSize= csize;	
					cend = cbeg + csize; // - 1; 
					}
				}
				
			if (fFirststuff>0)  
				fRichView->GetDocStats( lastItemCount, lastLineCount);
			 
			dataRemaining = fStufGo->fInfoSize;
			update= fRichHandler->ProcessData( cbeg, cend, endOfData, dataRemaining);
			if (update) fStufGo->fInfoSize= dataRemaining;		
			
			if (endOfData && gotInline) {
				fRichHandler->InstallInStyle( fRichView, fInlineStyle);
				fInlineStyle= NULL;
				}

			if (update && fFirststuff>0 && update != 3 && !gotInline) { 
				Nlm_RecT  invalr;
				short i, winheight, pixh, pixold, 
							itemCount, lineCount, lineHeight, startsAt, rowCount;

				invalr= fRichView->fViewrect;
				// check if we have updated all of display...
				winheight= invalr.bottom  -  invalr.top;
				fRichView->GetDocStats( itemCount, lineCount);
				pixold= 0;
				for (i= 1, pixh= 0; i<=itemCount; i++) {
					fRichView->GetItemStats( i, startsAt, rowCount, lineHeight);
					pixh += rowCount * lineHeight;
					if (i<=lastItemCount) pixold= pixh;
					}
				// remember pixh from last time & only inval new part !?
				invalr.top += pixold;
				fRichView->InvalRect(invalr);
				if (pixh >= winheight) fFirststuff= 0; 
				}

			}
		
		if (!fStufGo || fStufGo->ThreadProgress() == DGopher::kThreadDoneReading) {
			theTask->SetRepeat(false); // done w/ gopher fetch, dequeue this task
			//this->ShowMessage("done reading");
			fFetchFile->Close();
			delete fRichHandler; 
			fRichHandler= NULL;
			}
		}
}


void DGopherTextStore::Close()
{
	if (fStufGo && fStufGo->ThreadProgress() != DGopher::kThreadDoneReading) {
			// kill tcp connection !!!!!!!!
		fStufGo->CloseQuery();
		fStufGo->fThreadState= DGopher::kThreadNotStarted;  
		fStufGo= NULL; // don't delete !?
		}
	if (fFetchFile) fFetchFile->Close();
	if (fDocFile) fDocFile->Close();
}



Boolean DGopherTextStore::OpenStore()
{
	// ?! need to link this proc w/ Window.Open() procs

#if 0
			// do this in constructor !?
	if (!fRichView) {
		if (fIsLinkDoc)
			fRichView= new DLinkedRichView(0, (DGoLinkedTextDoc*)fWindow, 
														gPrintManager->PageWidth(), 0);
		else
		  fRichView= new DRichView(0, fWindow, gPrintManager->PageWidth(), 0);
		}
	fRichView->SetSlateBorder( true);
	fRichView->SetResize( DView::matchsuper, DView::relsuper);
	fRichView->SetTabs( gTextTabStops);
	//fRichView->SizeToSuperview( fWindow, true, false); // this needs to adjust for if view has scrollbar !
#endif
		
  if (!fGolist) fGolist= new DGopherList(NULL);
	fGolist->fParentMenu= fParentGo;  
	if (!fIsFromFile && fParentGo) {
		fGolist->InsertFirst( fParentGo); // !?!? put papa into list, 
				// mainly for use  with extract !?
		if (fParentGo->fThreadState == DGopher::kThreadNotStarted) 
			fParentGo->ReadTask();  // tell gopher to read data thru a repeat task
		if (fParentGo->fThreadState == DGopher::kThreadDoneReading 
		  && fParentGo->fInfoSize < 1) { 
			fParentGo->fThreadState= DGopher::kThreadNotStarted; // ! need to reset this somewhere for used gophers
			this->suicide(); 
			return false;  // ?? return false; ?? 
			}
		fStufGo= fParentGo;
		//DTask* aStuffTask= newTask(cStuf, DGopherTextDoc::kGoTextdoc);
		DTask* aStuffTask= new DTask(cStuf, DGopherTextDoc::kGoTextdoc, fWindow);
		aStuffTask->SetRepeat(true);
		this->PostTask( aStuffTask);
		}
		
	return true; 
}
	


















// class	DGopherTextDoc : public DRichTextDoc


DGopherTextDoc::DGopherTextDoc(long id, DGopher* itsGopher, Boolean havelinks) :
		DRichTextDoc( id, false),
		fInUse(false), fPinned(false), fStickpin(NULL),
		fTextStoreList(NULL), fListAt(kEmptyIndex),
		fStore(NULL), fStatus(NULL)
{ 
	DIconButton* ib;
	
	if (fRichView) { this->RemoveSubview( fRichView, true); fRichView= NULL; }
	
	//fTextStoreList= new DList(NULL, DList::kDeleteObjects);
	fTextStoreList= new DList(NULL);
	fStickpin= new DIconButton( cPushpin, this, &gPushpinOut);
	this->NextSubviewToRight();
#if 0
	ib = new DIconButton( cParentDrop, this, &gDownTriangle);
	this->NextSubviewBelowLeft();
#endif
	ib = new DIconButton( cShiftBack, this, &gLeftTriangle);
	this->NextSubviewToRight();
	fShiftBack= ib; fShiftBack->Disable();
	ib = new DIconButton( cShiftFore, this, &gRightTriangle);
	this->NextSubviewBelowLeft();
	fShiftFore= ib; fShiftFore->Disable();
	
	this->GetNextPosition( &fTextViewLoc);
	fTextViewLoc.x= 0; // fix for odd offset
	this->SetNextPosition( fTextViewLoc);
	fStore= new DGopherTextStore( itsGopher, this, havelinks);
	fRichView= fStore->fRichView;
	
	AddToList(fStore);
	fSaveHandler= this;  
	fPrintHandler= fRichView; // make sure current view is printed
	this->NextSubviewBelowLeft();
}
		
DGopherTextDoc::~DGopherTextDoc()
{
	if (fTextStoreList) {
			// !! DAMMIT, delete fTextStoreList IS NOT freeing fStore objects !!
		short i, n= fTextStoreList->GetSize();
		for (i=0; i<n; i++) {
			DGopherTextStore* st= (DGopherTextStore*) fTextStoreList->At(i);
			delete st; // !!!!! DIE DAMMIT
			}
		delete fTextStoreList; // deletes all objects in list
		}
	//if (fStore) delete fStore; //<< fTextStoreList kills this
}


void DGopherTextDoc::AddToList( DGopherTextStore* theStore)
{
		// clean out storage below current displayed item...
  for (short idoc= fTextStoreList->GetSize()-1; idoc>fListAt; idoc--) {
		DGopherTextStore* st= (DGopherTextStore*) fTextStoreList->At(idoc);
		delete st; // !!!!! DIE DAMMIT
  	fTextStoreList->AtDelete(idoc); // DOESN"T delete all of storage
  	}
	fTextStoreList->Push( theStore);
	fListAt= fTextStoreList->GetSize()-1;
}


Boolean DGopherTextDoc::PushNewDoc( DGopher* itsGopher, DGopherList* itsList, Boolean havelinks)
{
	// !! need to test for free mem here, free top of fTextStoreList if full...
	if (!fInUse && !fPinned) {
		this->SetNextPosition( fTextViewLoc);
		DGopherTextStore* newStore= new DGopherTextStore( itsGopher, this, havelinks);
		if (!newStore->OpenStore()) { 
			delete newStore;
			}
		else {
		  fRichView->Hide();
			fStore= newStore;
			fRichView= fStore->fRichView;
			this->AddToList(fStore);
			if (fStatus) {
				fStore->fGolist->SetStatus( fStatus);
				if (fStore->fParentGo) fStore->fParentGo->fStatusLine= fStatus;
				}
			this->SetTitle((char*)fStore->GetTitle());
			fSaveHandler= this; // replace fRichView's handler w/ us		
			fPrintHandler= fRichView; // make sure current view is printed
			this->ShowMessage("Reading...");
			gGopherTextDoc= this; // track front gopher doc/window
			fRichView->SizeToSuperview( this, true, false); // this needs to adjust for if view has scrollbar !
			this->Invalidate();
			fShiftBack->Enable();
			fShiftFore->Disable();
			//this->Select();	//?? DWindow::Open()
			//this->Show(); 	//?? DWindow::Open()
			}
		return true;
		}
	else
		return false;
}


Boolean DGopherTextDoc::PopDoc( short numitems)
{
	DGopherTextStore* astore = NULL;
	if (fTextStoreList->GetSize() != kEmptySize) {
		short newat = fListAt - numitems;
		if (newat > kEmptyIndex && newat < fTextStoreList->GetSize()) {
			fListAt= newat;
			astore= (DGopherTextStore*) fTextStoreList->At( fListAt);
			}
		}
	if (astore) {
	  fRichView->Hide();
		fStore= astore;
		fRichView= fStore->fRichView;
			// ?? need to make sure fRichView is in this window  !! YES
		//fRichView->SetWindow(this); //<< doesn't set scrollbars, other stuff !?
		this->SetTitle( (char*)fStore->GetTitle());
	  fRichView->Select(); //?? for motif
	  fRichView->Show();
	  
		if (fListAt+1 < fTextStoreList->GetSize()) fShiftFore->Enable();
		else fShiftFore->Disable();
		if (fListAt > 0) fShiftBack->Enable();
		else fShiftBack->Disable();
		return true;
		}
	else
		return false;
}


void DGopherTextDoc::Close()
{
	fStore->Close();
	if (gGopherTextDoc == this) gGopherTextDoc= NULL; // track front gopher doc/window
	fInUse= false;
	fPinned= false;
	DRichTextDoc::Close();
}


void DGopherTextDoc::Activate()
{	
	if (DGopherListDoc::gLockWinMenuItem) 
		DGopherListDoc::gLockWinMenuItem->SetStatus( fInUse || fPinned);		
	gGopherTextDoc= this; // track front gopher doc/window
	DRichTextDoc::Activate();
}

void DGopherTextDoc::Deactivate()
{	
#if 0
	if (DGopherListDoc::gLockWinMenuItem)  
		DGopherListDoc::gLockWinMenuItem->SetStatus(false);
	if (gGopherTextDoc == this) gGopherTextDoc= NULL;  
#endif
	DRichTextDoc::Deactivate();
}


Boolean DGopherTextDoc::IsMyTask(DTask* theTask) 
{
	if (theTask->fKind == kGoTextdoc) {
		ProcessTask( theTask);
		return true;
		}
	else 
		return DWindow::IsMyTask(theTask);
}

void DGopherTextDoc::ProcessTask( DTask* theTask)
{
	if (theTask->fNumber == cStuf || theTask->fNumber == cStufInline) { 
		fStore->ProcessTask( theTask);
		if (!fStore->fRichHandler) this->ShowMessage("done reading");
		}
	else
		DRichTextDoc::ProcessTask( theTask);
}


void DGopherTextDoc::PinWindow(Boolean turnon)
{ 
	fPinned= turnon;
	if (fStickpin) {
		if (fPinned) fStickpin->SetIcon( &gPushpinIn, true);
		else fStickpin->SetIcon( &gPushpinOut, true);
		}
}

Boolean DGopherTextDoc::IsMyAction(DTaskMaster* action) 
{
	switch(action->Id()) {
	
		case cParentDrop:
			return true;

		case cPushpin:
			PinWindow( !fPinned);
			return true;

		default: 
			return DRichTextDoc::IsMyAction(action);
		}
}


const char* DGopherTextDoc::GetFileToSave(DFile*& aFile)
{
	static char *name;
	enum   { kLineMax = 512 };
	char	 filename[kLineMax];
	char   *suffix = NULL, *macsire = NULL, *mactype = NULL;
	Boolean	useTemp;
	char	 *writeflag= "w"; 
	
	useTemp= (fStore->fDocFormat != DRichHandler::kTextformat 
				&& (fStore->fDocFile || fStore->fFetchFile));
	if (!aFile) {
		name= this->GetTitle();
		if (useTemp) switch (fStore->fDocFormat) {
			case DRTFHandler::kRTFformat  : 
				DRTFHandler::RTFFileSigs( suffix, mactype, macsire);
				writeflag= "w"; 
				break;
			case DPICTHandler::kPICTformat:  
				DPICTHandler::PICTFileSigs( suffix, mactype, macsire); 
				writeflag= "wb"; 
				break;
			case DGIFHandler::kGIFformat	: 
				DGIFHandler::GIFFileSigs( suffix, mactype, macsire); 
				writeflag= "wb"; 
				break;
			case DHTMLHandler::kHTMLformat: 
				DHTMLHandler::HTMLFileSigs( suffix, mactype, macsire); 
				writeflag= "w"; 
				break;
			default:
				DRichHandler::RichFileSigs( suffix, mactype, macsire); 
				writeflag= "w"; 
				break;
			}
		else  
			DRichHandler::RichFileSigs( suffix, mactype, macsire); 
		sprintf(filename, "%s%s", name, suffix);
		MemFree(name);
		name= (char*) gFileManager->GetOutputFileName(filename);  
		if (name) aFile= new DFile( name, writeflag, mactype, macsire);
		}
	else  {
		name= (char*) aFile->GetName();
		}
	return name;
}


void DGopherTextDoc::Save(DFile* aFile)
{
	char	*name = NULL, *data, *readkind, *writekind;
	char 	*mackind= "TEXT", *macsire= "ttxt";
	Boolean callerOwnsFile= (aFile != NULL);
	Boolean	useTemp;
	
	useTemp= (fStore->fDocFormat != DRichHandler::kTextformat 
				&& (fStore->fDocFile || fStore->fFetchFile));
	gCursor->watch();
	if (callerOwnsFile) 
		name= (char*) aFile->GetName();
	else		
		name= (char*) GetFileToSave( aFile);
		
	if (aFile) {
		if (useTemp) { 
			mackind= aFile->fType;
			macsire= aFile->fSire;
			aFile->Delete();
			}
		else {
			fRichView->Save(aFile); 
			this->NotDirty(); // !? is this correct, but only for current fRichView !?
			}
		if (!callerOwnsFile) delete aFile; //aFile->suicide();
		}

	switch( fStore->fDocFormat ) {
		case DPICTHandler::kPICTformat:  
		case DGIFHandler::kGIFformat:  
			writekind= "wb";
			readkind= "rb";
			break;
				
		case DHTMLHandler::kHTMLformat:  
		case DRTFHandler::kRTFformat:
		case DRichHandler::kTextformat:
		default: 
			writekind= "w";
			readkind= "r";
			break;
		}
			
	if (useTemp) {
		DFile tFile( name, writekind, mackind, macsire);
		fStore->WriteDoc( &tFile, readkind, writekind);
		this->NotDirty(); // !? is this correct, but only for current fRichView !?
		}
			
	gCursor->arrow();
}


void DGopherTextDoc::ShowMessage(const char* msg)
{
	if (fStatus) fStatus->SetTitle( (char*)msg);
}



void DGopherTextDoc::Open()
{
	if (!fStore->OpenStore()) 
		this->suicide();
	else {
				// add status line after DRichView...
				// this is for Window part of doc
		Nlm_ResizeWindow( (Nlm_GraphiC)fWindow, kWinDragHeight, kWinScrollWidth,
							 kWinMinWidth, kWinExtraHeight+16);
		MakeSmallFont();
		Nlm_PoinT nps;
		this->GetNextPosition( &nps);
		DPrompt* pr= new DPrompt( 0, this, "Status:", 0, 0, gSmallFont); // Nlm_programFont
		pr->SetResize( DView::fixed, DView::moveinsuper);
		// positioning is messed up, maybe because this is at bottom of window??
		//this->NextSubviewToRight();
		nps.x += Nlm_StringWidth( "Status:");
		this->SetNextPosition(nps);
		
		fStatus= new DPrompt(  0, this, NULL, 350, 0, gSmallFont); // Nlm_programFont
		fStatus->SetResize( DView::fixed, DView::moveinsuper);
		
		if (fStatus) {
			fStore->fGolist->SetStatus( fStatus);
			if (fStore->fParentGo) fStore->fParentGo->fStatusLine= fStatus;
			}
		
		this->SetTitle( (char*)fStore->GetTitle());
		fSaveHandler= this; // replace fRichView's handler w/ us	
		fPrintHandler= fRichView; // make sure current view is printed
		fRichView->SizeToSuperview( this, true, false); // this needs to adjust for if view has scrollbar !
		this->ShowMessage("Reading...");
		gGopherTextDoc= this; // track front gopher doc/window
		
		// DRichTextDoc::Open() does ResizeWindow, SizeToSuperview && DWIndow::Open 
		// ?? use it instead ??
		DWindow::Open();
		}
}




















//class DGoLinkedTextDoc : public DGopherTextDoc

Boolean DGoLinkedTextDoc::gAutoloadInlines = TRUE;

DGoLinkedTextDoc::DGoLinkedTextDoc(long id, DGopher* itsGopher, DGopherList* itsList) :
		DGopherTextDoc( id, itsGopher),
		fGopherDoc(NULL),
		fInlineParts(NULL)
{ 
#if 1
	if (itsList) itsList->newOwner();
	else itsList= new DGopherList(NULL);
	fStore->fIsLinkDoc= true;
	fStore->SetGolist(itsList);
#else
	fGopherDoc= itsGoDoc;  
	if (!fGopherDoc) {
		fGopherDoc= new DGopherListDoc();
		fGopherDoc->fGolist= new DGopherList(NULL);
		fGopherDoc->fGoview= new DGolistView(0, fGopherDoc, fGopherDoc->fGolist, kGolistViewWidth);
		}
	else
		fGopherDoc->newOwner();
	fStore->fIsLinkDoc= true;
	fStore->SetGolist(fGopherDoc->fGolist);
#endif
}
		
DGoLinkedTextDoc::~DGoLinkedTextDoc()
{
	if (fGopherDoc) fGopherDoc->suicide(); //???
	if (fInlineParts) delete fInlineParts;
}


Boolean DGoLinkedTextDoc::PushNewDoc( DGopher* itsGopher, DGopherList* itsList, Boolean havelinks) 
{
	if (itsList) itsList->newOwner();
	else itsList= new DGopherList(NULL);

	if (!DGopherTextDoc::PushNewDoc( itsGopher, itsList, true)) 
		return false;

	fStore->fIsLinkDoc= true;
	fStore->SetGolist(itsList);
	
	return true;
}

Boolean DGoLinkedTextDoc::PushNewLocalDoc(DFile* docFile, DFile* goFile)
{
	if (!this->PushNewDoc( (DGopher*)NULL, (DGopherList*)NULL, true)) 
		return false;
	if ( ReadFrom( docFile, goFile) ) {
		fStore->OpenStore();
		if (fStore->fParentGo) this->SetTitle((char*)fStore->GetTitle());
		else if (docFile) this->SetTitle((char*)docFile->GetName());
		}
	else 
		this->PopDoc(1);
	return true;
}


// static
short DGoLinkedTextDoc::TestFormat(char* databuf, ulong datasize, char*& datastart) 
{
	short format = 0;
	datastart= NULL;
	DRichHandler* doctest;
	long		ioffs;
	
	if ( Nlm_StrNICmp( databuf, "+MENU:", 6 ) == 0) {
		// this is a shaky test, need better magic key for this data ?!!?
		datastart= databuf;
		return DGopher::cGopherNetDoc;
		}
	else {
		char* cp= (char*) MemChr( databuf, '\n', MIN(datasize,255));
	  if (cp && Nlm_StrNICmp( cp+1, "+MENU:", 6 ) == 0)  {
			datastart= cp+1;
			return DGopher::cGopherNetDoc;
			}
		}
		
	if ( DGopherList::IsGopherLine( databuf) ) {
		datastart= databuf;
		return DGopher::cNewGopherFolder; // == 2211
		}

	doctest = new DRTFHandler( NULL, NULL);
	datastart= doctest->IsRich( databuf, datasize);
	if (datastart) format= doctest->Format();
	delete doctest;
	if (format) return format;
		
	doctest = new DPICTHandler( NULL, NULL);
	datastart= doctest->IsRich( databuf, datasize);
	if (datastart)  format= doctest->Format();
	delete doctest;
	if (format) return format;

	doctest = new DGIFHandler( NULL, NULL);
	datastart= doctest->IsRich( databuf, datasize);
	if (datastart)  format= doctest->Format();
	delete doctest;
	if (format) return format;

	doctest = new DHTMLHandler( NULL, NULL);
	datastart= doctest->IsRich( databuf, datasize);
	if (datastart)  format= doctest->Format();
	delete doctest;
	if (format) return format;

	doctest = new DRichHandler( NULL, NULL);
	datastart= databuf;
	format= doctest->Format();
	delete doctest;
	return format;
}


// ?????
void DGoLinkedTextDoc::ProcessTask( DTask* theTask)
{
	if (theTask->fNumber == cProcessInlines) {
		if (!fStore->fStufGo) { // !! we can't do more than one at a time !! (not yet)
			if (!ReadInlinePart() )
				theTask->SetRepeat(false);  // dequeue this task
			}
		}
	else 
		DGopherTextDoc::ProcessTask( theTask); 

	if (theTask->fNumber == cStuf) {
		if (!fStore->fStufGo 
		 || fStore->fStufGo->ThreadProgress() == DGopher::kThreadDoneReading) {
			// add fGolist links into fRichView text.... (do also for net-read doc)
			this->ShowMessage("marking links...");
			this->AddGolinksToDoc();
			fRichView->Invalidate();
			this->ShowMessage("ready");
			fStore->fStufGo= NULL;
			}
		}
	else if (theTask->fNumber == cStufInline) {
		if (!fStore->fStufGo 
		 || fStore->fStufGo->ThreadProgress() == DGopher::kThreadDoneReading) {
			fRichView->Invalidate();
			this->ShowMessage("ready");
			fStore->fStufGo= NULL;
			}
		}
}


class DGoStyle : public DObject
{
public:
	DGopher* 		fGo;
	DRichStyle* fStyle;
	short	fStyleindex;
	
	DGoStyle( short indx, DRichStyle* st, DGopher* go):
		fStyleindex(indx), fGo(go), fStyle(st)
	{
	}
	~DGoStyle() {}
};


Boolean DGoLinkedTextDoc::ReadInlinePart()
{
	DGoStyle* gs = (DGoStyle*) fInlineParts->Dequeue();
	fStore->fStufGo= NULL;
	if (gs) {
		//fStore->fStufGo= gs->fGo;
		//fStore->fStufGo->ReadTask();  // tell gopher to read data thru a repeat task
		gs->fGo->ReadTask();
		if (gs->fGo->fThreadState != DGopher::kThreadDoneReading) { 
#if 1
			fStore->StartInlineFetch( gs->fGo, gs->fStyle);
#else
			if (!fStore->fDocFile && fStore->fFetchFile) {
				fStore->fDocFile= fStore->fFetchFile;
				fStore->fFetchFile= new DTempFile();
				}
			fStore->fFirststuff= 1;
			fStore->fInlineStyle= gs->fStyle;
#endif
			DTask* aStuffTask= newTask( cStufInline, kGoTextdoc);
			aStuffTask->SetRepeat(true);
			this->PostTask( aStuffTask); 
			}
		else 
			fStore->fStufGo= NULL;
		delete gs;
		return true;
		}
	else 
		return false;
}


void DGoLinkedTextDoc::SetupInlinePart( DRichStyle* st)
{
	Boolean added= false;
	Boolean needtask= (!fInlineParts || fInlineParts->GetSize() == 0);
	if (st->ispict && st->fPict && st->fPict->HasNetPict()) {
		short linkid= st->fPict->fLinks[0].fLinkid;
		if (linkid) {
			DGopher* ag= fStore->fGolist->GopherAt(linkid-1);
			if (ag) {
				DGoStyle* gs= new DGoStyle( 0, st, ag);
				if (!fInlineParts) fInlineParts= new DList();
				fInlineParts->Queue( gs);
				added= true;
				}
			}
		}
	if (needtask && added && fInlineParts->GetSize()) {
		DTask* aStuffTask= newTask( cProcessInlines, kGoTextdoc);
		aStuffTask->SetRepeat(true);
		this->PostTask( aStuffTask); 
		}
}

 
void DGoLinkedTextDoc::SetupInlineParts()
{
	Boolean added= false;
	Boolean needtask= (!fInlineParts || fInlineParts->GetSize() == 0);
	short i, n= fRichView->fStyles->GetSize();
	for (i= 0; i<n; i++) {
		DRichStyle* st= (DRichStyle*) fRichView->fStyles->At(i);
		if (st->ispict && st->fPict && st->fPict->HasNetPict()) {
			short linkid= st->fPict->fLinks[0].fLinkid;
			if (linkid) {
				DGopher* ag= fStore->fGolist->GopherAt(linkid-1);
				if (ag) {
					DGoStyle* gs= new DGoStyle( i, st, ag);
					if (!fInlineParts) fInlineParts= new DList();
					fInlineParts->Queue( gs);
					added= true;
					}
				}
			}
		}
	if (needtask && added && fInlineParts->GetSize()) {
		DTask* aStuffTask= newTask( cProcessInlines, kGoTextdoc);
		aStuffTask->SetRepeat(true);
		this->PostTask( aStuffTask); 
		}
}




void DGoLinkedTextDoc::ExtractGopherDoc()
{
	fStore->fGolist->newOwner();
	if (fStore->fParentGo) fStore->fParentGo->newOwner();
	fStore->fGolist->fParentMenu= fStore->fParentGo;
#if 0
		// Replace is buggy still 
	if (gGopherDoc && gGopherDoc->ReplaceData( fStore->fGolist)) 
		;  // never here because this doc is active & gGopherDoc is NULL if not frontwin
	else
#endif
  {
		DGopherListDoc* win= 
		  new DGopherListDoc( DGopherListDoc::kGoListdoc, fStore->fGolist, 
											    this->GetTitle());
		//win->Open(true); //<< new does this !?
		}
}


void DGoLinkedTextDoc::OpenGopherLink( short gopherItem, Nlm_PoinT* mappt)
{ 
	if (fStore->fGolist) {
		DGopher* ag= fStore->fGolist->GopherAt(gopherItem);
		OpenGopherLink( ag, mappt);
		}
}

void DGoLinkedTextDoc::OpenGopherLink( DGopher* ag, Nlm_PoinT* mappt)
{ 
	if (ag) {   
		char *maps = NULL;
		gCursor->watch();
		this->ShowMessage("opening link...");
		Boolean nooption= false;
		Boolean optionIsOn= gKeys->option();
		if (ag) {
			nooption= (ag->fType == kTypeFolder 
				|| ag->fType == kTypeQuery  // ??
				|| ag->fIsMap
				|| ag->fIsPlus != kGopherPlusYes); 
			if (!nooption && DGopherListDoc::gOptionIsOn) 
				optionIsOn= !optionIsOn;
			if (mappt) {
				char  buf[128];
				sprintf( buf, "\t%d,%d", mappt->x, mappt->y);
				maps= StrDup( buf);
				ag->fQuery= maps;
				}
			if (optionIsOn)  
				(void) DGopherListDoc::GopherByViewDialog(ag, ag->fOwnerList);
			else {
				short itsViewchoice= DGopherListDoc::DefaultGopherView(ag);
				DGopherListDoc::ProcessGopher( ag, itsViewchoice);
				}
			if (mappt && maps) {
					// !? maker of fQuery must free it when gopher processing is done!?
					// can we fix DGopher::CloseQuery() to handle this?
					// e.g., all fQuery must be owned by the gopher item...
				maps= (char*) MemFree(maps);
				}
	 		}
		gCursor->arrow();
 		}
}


Boolean DGoLinkedTextDoc::IsMyAction(DTaskMaster* action) 
{
	switch(action->Id()) {
	
		case cParentDrop:
			return true;
		
		case cShiftFore:
			this->PopDoc(-1);
			return true;
			
		case cShiftBack:
			this->PopDoc(1);
			return true;
			
		case cMenuPopup: {
			DPopupList* pl= (DPopupList*) action;  
			if (pl) OpenGopherLink( pl->GetValue() - 1);
			}
			return true;

		default: 
			return DGopherTextDoc::IsMyAction(action);
		}
}



Boolean DGoLinkedTextDoc::ReadFrom(DFile* docFile, DFile* goFile)
{
	if (docFile) {
#if 1

#else
		if (!fGopherDoc) {
			fGopherDoc= new DGopherListDoc();
			fGopherDoc->fGolist= new DGopherList(NULL);
			fGopherDoc->fGoview= new DGolistView(0, fGopherDoc, fGopherDoc->fGolist, kGolistViewWidth);
			}
		fStore->SetGolist(fGopherDoc->fGolist);
#endif

		if (fStatus) fStore->fGolist->SetStatus( fStatus);
		if (!fStore->fGolist->fParentMenu) {
			char docurl[1024];
			char * name = (char*) docFile->GetName();
			char quote= '\'';
			if (StrChr( name, quote)) quote= '"';
			StrCpy(docurl, " file:///");
			StrNCat(docurl, name, 1000);
			StrNCat(docurl, " ", 1000); 
			docurl[1023]= 0;
			long doclen= StrLen(docurl);
			docurl[0]= quote; // so parseURL doesn't eat spaces
			docurl[doclen-1]= quote;
			fStore->fGolist->fParentMenu= DGopherList::GopherFromURL(docurl, doclen, fStore->fGolist);
			//this->SetTitle((char*)docFile->GetName());
			// ! GopherFromURL() sticks new gopher into list also !
			}
		fStore->fParentGo= fStore->fGolist->fParentMenu;
	
	  if (goFile) {
			ulong filelen;
			goFile->OpenFile();
			goFile->GetDataLength(filelen);
			char* buf= (char*) MemNew( filelen+1);
			goFile->ReadData( buf, filelen);
			//fGopherDoc->AddData( buf, filelen);
			fStore->fGolist->ReadLinks( buf, filelen);
			MemFree( buf);
			goFile->CloseFile();
			delete goFile;
			//fGopherDoc->UpdateListSize();
			//fGopherDoc->fGoview->SetColWidths(); //?
			}
			
		// delete docFile; //<< !?!?!?
		fStore->fIsFromFile= false;
		return true;
		}
	else
		return false;
}


void DGoLinkedTextDoc::Save(DFile* aFile)
{
	// we have to dup this part from DGopherTextDoc::Save() or rewrite all of save usage...
	char	filename[512];
	char	*name = NULL, *data;
	ulong	dlen;
	Boolean callerOwnsFile= (aFile != NULL);
	
	if (callerOwnsFile)  
		name= (char*) aFile->GetName();
	else		
		name= (char*) GetFileToSave( aFile);
		
	if (aFile) {
		DGopherTextDoc::Save(aFile);
		// and save data to filename.go4 == filename.DGopherListDoc::kGopherDocSuffix
 
		switch ( fStore->fDocFormat && fStore->fGolist->GetSize()>0 ) {
			case DHTMLHandler::kHTMLformat:  
				break;
				
			case DGIFHandler::kGIFformat:  
			case DPICTHandler::kPICTformat:  
			case DRTFHandler::kRTFformat:
			default: 
				gCursor->watch();
				aFile->Close();
				StrNCpy( filename, (char*) aFile->GetName(), 512);
				gFileManager->ReplaceSuffix(filename, 512, DGopherListDoc::kGopherDocSuffix);
			
				data= fStore->fGolist->WriteLinks();
				dlen= StrLen(data);
 				{
 					// sunCC is getting confused w/ DFile bFile() unless wrapped in braces !
 					DFile bFile( (const char*)filename, (const char*)"w", 
 					 					(const char*)"TEXT",  (const char*)"IGo4");
  				//aFile->Initialize( filename, "w", "TEXT", "IGo4");
  				bFile.OpenFile();  
  				bFile.WriteData(data, dlen); 
  				bFile.CloseFile(); 
 				}
				MemFree(data);
				gCursor->arrow();
				break;
			}
			
		if (!callerOwnsFile) delete aFile; // aFile->suicide();  
		}
	
}





void DGoLinkedTextDoc::InsertLink( short golistItem, DGopher* theGo, short atparag, short atchar, short atlen)
{
	DRichStyle linkStyle;
	
	DRichStyle* aStyle= fRichView->GetStyleAtChar( atparag, atchar);
	if (aStyle) linkStyle= *aStyle; 
	linkStyle.color= gLinkcolor;
	linkStyle.last = false;
	linkStyle.linkid= golistItem + 1; 
	if (theGo) {
		if (theGo->fIsMap) {
			linkStyle.ismap= true;
			fStore->fMapLink= linkStyle.linkid;
			}
		}
	fRichView->InsertStyle( atparag, atchar, atlen, &linkStyle);
	linkStyle.ownpict= false;
}

void DGoLinkedTextDoc::InsertLink( short golistItem, DGopher* theGo, 
					Nlm_RecT arect, short atparag, short atchar)
{	
	DRichStyle* aStyle= fRichView->GetStyleAtChar( atparag, atchar);
	if (aStyle && aStyle->ispict && aStyle->fPict) {
			 //!! arect must be relative to pict top,left !!
		if (theGo) {
			if (theGo->fIsMap) {
				aStyle->ismap= true;
				// fStore->fMapLink= golistItem+1;
				}
			}
		aStyle->fPict->AddLink( DPictStyle::kPictNetLink, golistItem+1, arect);
		}
}

void DGoLinkedTextDoc::InsertLink( DGopher* theGo, short atparag, short atchar, short atlen)
{
	if (theGo) {
		short igo= fStore->fGolist->GetIdentityItemNo( (DObject*) theGo);
		if (igo == kEmptyIndex) {
			fStore->fGolist->InsertLast( theGo);
			igo= fStore->fGolist->GetSize()-1;
			}
	  InsertLink( igo, theGo, atparag, atchar, atlen);
	  }
}

void DGoLinkedTextDoc::InsertLink( DGopher* theGo, Nlm_RecT arect, short atparag, short atchar)
{
	if (theGo) {
		short igo= fStore->fGolist->GetIdentityItemNo( (DObject*) theGo);
		if (igo == kEmptyIndex) {
			fStore->fGolist->InsertLast( theGo);
			igo= fStore->fGolist->GetSize()-1;
			}
	  InsertLink( igo, theGo, arect, atparag, atchar);
	  }
}


void DGoLinkedTextDoc::LinkToSelection( DGopher* aGopher)
{
	short  			selparag, selchar, selendparag, selendchar;
	Boolean 		inpict, isSelected;
	Nlm_RecT		selrect;
	DRichStyle	selstyle;
	
	isSelected= fRichView->GetSelection( inpict, selparag, selchar, selendparag, selendchar, 
													selrect, &selstyle);
	if (isSelected && aGopher) {
		aGopher= (DGopher*) aGopher->Clone(); 
		// must add +MENULINE or +MENURECT info to aGopher !!
		if (!aGopher->fDocLink) aGopher->fDocLink= new DocLink();
		if (inpict) {
			aGopher->fDocLink->SetLineRect( selrect, selparag, selchar);
			this->InsertLink( aGopher, selrect, selparag, selchar);
			}
		else {
			aGopher->fDocLink->SetLine( selparag, selchar, selendparag, selendchar);
			this->InsertLink( aGopher, selparag, selchar, selendchar-selchar);
			}
		fRichView->Invalidate(); // !? show new link
		Dirty();
		}
	selstyle.ownpict= false;
}


void DGoLinkedTextDoc::MarkURLs()
{
	if (fRichView && fStore->fGolist) {
		Boolean found;
		short		atparag, atchar, atlen, startparag = 0, startchar = 0;
		do {
			found= fRichView->FindURL( atparag, atchar, atlen, startparag, startchar);
			if (found) {
				DGopher* go;
				
				char* itemtxt= fRichView->GetText(atparag, -1);
				if (itemtxt) {
				  go= DGopherList::GopherFromURL( itemtxt+atchar, atlen, fStore->fGolist); 
					InsertLink( go, atparag, atchar, atlen);
					}

				startchar= atchar+atlen+1; 
				startparag= atparag;
				}
		} while (found);
		}	
}


void DGoLinkedTextDoc::AddGolinksToDoc()
{
	if (gAutoloadInlines) this->SetupInlineParts();
	if (fStore->fDocFormat == DHTMLHandler::kHTMLformat) return;

	this->MarkURLs();
	
	if (fRichView && fStore->fGolist) {
		short igo, ngo= fStore->fGolist->GetSize();
		for (igo= 0; igo<ngo; igo++) {
			Boolean found;
			short		atparag, atchar, atlen, startparag = 0, startchar = 0;
			DGopher* go= fStore->fGolist->GopherAt(igo);

			if (go->fIsMap) {
				fStore->fMapLink= igo+1;
				}
				
			if (go->HasDocLink()) 
				switch (go->fDocLink->Kind()) {
								
				case DocLink::kNoLink:
					break;
					
				case DocLink::kLine: {
					short endparag, endchar;
					go->fDocLink->GetLine( atparag, atchar, endparag, endchar);
					//InsertLink( igo, atparag, atchar, endparag, endchar);
					InsertLink( igo, go, atparag, atchar, endchar-atchar); // need new Ins type
					}
					break;

				case DocLink::kLineRect: {
					Nlm_RecT rect;
					go->fDocLink->GetLineRect( rect, atparag, atchar);
					InsertLink( igo, go, rect, atparag, atchar);
					}
					break;

				case DocLink::kRect: {
					Nlm_RecT rect;
					go->fDocLink->GetRect( rect);
					InsertLink( igo, go, rect, 0, 0); // assume this is PICT w/ only 1 char !?
					}
					break;
					
				case DocLink::kString: {
					short skipval = 0;
					char* finds= go->fDocLink->GetString( skipval);
					Boolean haveskipval= skipval>0;
					atlen= MIN( 10240, StrLen(finds));
					do {
						found= fRichView->Find( finds, atparag, atchar, startparag, startchar);
						if (skipval) skipval--;
						if (found && !skipval) {
							InsertLink( igo, go, atparag, atchar, atlen);
							startchar= atchar+atlen+1; 
							startparag= atparag;
							if (haveskipval) found= false; // do no more
							}
						} while (found);
					}
				break;
				
				}
			}
		}
}



void DGoLinkedTextDoc::Open(DFile* docFile, DFile* goFile)
{
	if ( ReadFrom( docFile, goFile) ) 
		this->Open();
	else 
		this->Close(); //this->suicide();
}

 
void DGoLinkedTextDoc::Open()
{
	DGopherTextDoc::Open();

#if 0 
	this->NextSubviewBelowLeft();
	DPrompt* pr= new DPrompt( 0, this, "Related topics:", 0, 0, Nlm_programFont);	 		
	pr->SetResize( DView::fixed, DView::moveinsuper);
	this->NextSubviewToRight();
	fMenuList= new DPopupList( cMenuPopup, this);
	fMenuList->SetResize( DView::fixed, DView::moveinsuper);

	if (fGolist) {
		short i, n= fGolist->GetSize();
		for (i= 0; i<n; i++) {
			DGopher* go= fGolist->GopherAt(i);
			fMenuList->AddItem((char*)go->GetName()); //<< need to MenuCleanName !! ShortName());  
			}
		}
#endif
}






//class DGopherInfoDoc : public DGopherTextDoc

void DGopherInfoDoc::ProcessTask(DTask* theTask) 
{
	if (theTask->fNumber == cInfo) {

		if (fStore->fStufGo && fStore->fStufGo->ThreadProgress() != DGopher::kThreadDoneReading)  
				fStore->fStufGo->ShowProgress();
				
		if (!fStore->fStufGo || fStore->fStufGo->ThreadProgress() == DGopher::kThreadDoneReading) {
			theTask->SetRepeat(false); // done w/ gopher fetch, dequeue this task
			this->ShowMessage("done reading");
			if (fStore->fStufGo && fStore->fStufGo->fInfoSize) {
				fRichView->Append( fStore->fStufGo->fInfo, NULL, NULL, 0, gTextFont);
				fRichView->Invalidate();
				fStore->fStufGo->DeleteInfo(); 
				fStore->fStufGo->fTransferType= fSavedType;
				}
			}
		}
	else {
		DWindow::ProcessTask(theTask);
		}
}


void DGopherInfoDoc::Open()
{
	fSavedType= fStore->fParentGo->fTransferType;
	fStore->fParentGo->fTransferType= kTransferText;
	fStore->fParentGo->InfoTask();   
	if (fStore->fParentGo->fThreadState == DGopher::kThreadDoneReading) { 
		this->suicide(); //delete this; 
		return; 
		}
	fStore->fStufGo= fStore->fParentGo;
	DTask* aTask= newTask(cInfo, kGoTextdoc);
	aTask->SetRepeat(true);
	this->PostTask( aTask);  

	//// view
	
	char title[512];
	sprintf(title,"Info for %s",(char*)fStore->GetTitle());
	this->SetTitle(title);
			// this is for Window part of doc
	Nlm_ResizeWindow( (Nlm_GraphiC)fWindow, kWinDragHeight, kWinScrollWidth,
						 kWinMinWidth, kWinExtraHeight+16);

	fSaveHandler= this; // replace fRichView's handler w/ us
	this->ShowMessage("Reading...");
	gGopherTextDoc= this; // track front gopher doc/window
	DWindow::Open();
}



//class	DGopherFileDoc : public DWindow


DGopherFileDoc::DGopherFileDoc(long id, DGopher* itsGopher) :
		DWindow( id, gApplication, fixed),
		fParentGo(itsGopher), 
		fFile(NULL), fLaunch(NULL), fMapper(NULL),
		fBytesAvail(NULL), fBytesGot(NULL),
		fTimeSpent(NULL), fBytesPerSec(NULL),
		fFirststuff(1)
{ 
	// FailNIL(fParentGo);
	fParentGo->newOwner();
}
		
DGopherFileDoc::~DGopherFileDoc()
{
#if 0
		/// nnoooooooo ... dview::deletesubviews does this
	if (fBytesAvail) delete fBytesAvail;
	if (fBytesGot) delete fBytesGot;
	if (fTimeSpent) delete fTimeSpent;
	if (fBytesPerSec) delete fBytesPerSec;
#endif
	if (fFile) delete fFile;
	delete fParentGo; //fParentGo->suicide();
}
	
Boolean DGopherFileDoc::suicide(void) 
{ 
	if (GetOwnerCount() <= 1) { 
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide();
}

Boolean DGopherFileDoc::suicide(short ownercount)
{
	if (ownercount < 1) {
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide(ownercount);
}


Boolean DGopherFileDoc::IsMyTask(DTask* theTask) 
{
	if (theTask->fKind == kGoFiledoc) {
		ProcessTask( theTask);
		return true;
		}
	else 
		return DWindow::IsMyTask(theTask);
}

Boolean DGopherFileDoc::IsMyAction(DTaskMaster* action) 
{
	switch (action->Id()) {
	
		case kLaunchBut:
			{
			// launch file
			char* name= (char*) fFile->GetName();
#ifdef WIN_MAC
#if 1
			DExternalHandler* child= new DExternalHandler( name);
			if (!child->Launch()) 
#else
			if (!Dgg_LaunchFile( name))
#endif
      	Nlm_Message(MSG_OK, "Application launch failed");
#else
			char  buf[1024];
			char* commandline= (char*)fMapper->GetHandlerName(); // may well include $file variable
			char* cp= StrStr(commandline, "$file"); // may look for other vars later, like $url, ...
			if (cp) {
				long len= cp - commandline;
				StrNCpy(buf, commandline, len);
				buf[len]= 0;
				StrNCat(buf, name, 512);
				StrNCat(buf, cp+5, 512);
				}
			else {
				StrNCpy(buf, commandline, 512);
				StrNCat(buf, " ",512);
				StrNCat(buf,name,512);
				}				
#if 1
			DExternalHandler* child= new DExternalHandler( buf);
			if (!child->Launch()) 
#else
			if (!Dgg_LaunchFile( buf))
#endif
       	Nlm_Message(MSG_OK, "Application launch failed");
#endif
			}
			return true;
			
		default: 
			return DWindow::IsMyAction(action);		
		}
}
			

void DGopherFileDoc::ProcessTask(DTask* theTask) 
{
	if (theTask->fNumber == cSave) {
		char snum[128];
		Boolean alldone =  (fParentGo->ThreadProgress() == DGopher::kThreadDoneReading);

		if (fParentGo->fInfoSize) {
			char * cbeg = fParentGo->fInfo;
			ulong clen = fParentGo->fInfoSize;
			short err= fFile->WriteData( cbeg, clen);
			fParentGo->DeleteInfo(); 
			//fParentGo->fInfo= (char*)MemGet(1, true); // NOW part of DGopher::DeleteInfo()
			
			// also, update progress stats on display
			Dgg_LongToStr(fParentGo->fBytesReceived, snum, 0, 128);
			fBytesGot->SetTitle( snum);
			Dgg_LongToStr(fParentGo->fConnectTime, snum, 0, 128);
			fTimeSpent->SetTitle( snum);

			long csec = fParentGo->fConnectTime; // time / 60;
			if (csec > 0) {
				csec= fParentGo->fBytesReceived / csec;
				Dgg_LongToStr( csec, snum, 0, 128);
				fBytesPerSec->SetTitle( snum);
				}

			if (fFirststuff>0) {
				long bytesavail;
				if (fParentGo->fBytesExpected > 0) bytesavail= fParentGo->fBytesExpected; 
				else bytesavail= fParentGo->fSizeLong;
				Dgg_LongToStr(bytesavail, snum, 0, 128);
				fBytesAvail->SetTitle( snum);
				fFirststuff= 0;
				}
			}
		
		if (alldone) {
			char sdone[80];
			StrCpy(sdone, "Done ");
			Dgg_LongToStr(fParentGo->fConnectTime, snum, 0, 20);
			StrCat(sdone, snum);
			fTimeSpent->SetTitle( sdone);
			if (fLaunch) {
				fLaunch->Enable();
				//fLaunch->Invalidate(); //?? do we need this to get it redrawn?
				}
				
			theTask->SetRepeat(false); // done w/ gopher fetch, dequeue this task
			fFile->CloseFile();
			}
		}
		
	else {
		DWindow::ProcessTask(theTask);
		}
}


void DGopherFileDoc::Open()
{
	char namebuf[256], *name;
	long  macType, macSire;
	char *suffix, macTypeStr[6], macSireStr[6];
	
	StrCpy(namebuf, (char*) fParentGo->ShortName());
	name= namebuf;
	this->SetTitle(name);

	// TESTING....
	//Nlm_ResizeWindow( (Nlm_GraphiC)fWindow, kWinDragHeight, kWinScrollWidth,
	//					 kWinMinWidth, kWinExtraHeight);
	
	// ?? is this info already set in fParentGo ?? ->fLaunch, ->fMacType, ->fMacSire
	// YES -- call to ItemForm before this doc is created sets it in fParentGo !
	fMapper= gGopherMap->GetPreferedFiletype(fParentGo, macType, macSire, suffix);
	if (fMapper) {
#ifdef WIN_MAC
		StrCpy(macTypeStr, Idtype2Str(macType));
		StrCpy(macSireStr, Idtype2Str(macSire));
#else
		gFileManager->ReplaceSuffix( namebuf, 255, suffix);
#endif
		MemFree(suffix);
		}
	
	name= (char*)gFileManager->GetOutputFileName( namebuf);
	if (!name) {
		this->suicide(); //delete this;
		return;
		}
	
	DFile* itsFile= new DFile( name, "wb", macTypeStr, macSireStr); //!! NEED "b" or stupid ansi/NLM converts newlines !
	if (!itsFile) { this->suicide(); return; } //delete this // ?? is this a reasonable abort?...
	fFile= itsFile;
	fFile->OpenFile();
	
	// add (open)/cancel button
	// static text w/ file name, total size, & size transferred
	// progress indicator ??

	DView *super = this;
	DPrompt *pr, *prDate = NULL;
	DCluster* clus;
	
	pr= new DPrompt( 0, super, "File:", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
	pr= new DPrompt( 0, super, (char*)gFileManager->FilenameFromPath( name), 0, 0, Nlm_programFont);	 		
	super->NextSubviewBelowLeft();

			// left side
	clus= new DCluster(0, this, 0, 0, true);
	super = clus;
	if (fParentGo->fDate) {
		pr= new DPrompt( 0, super, "File Date:", 0, 0, Nlm_programFont);	 		
		super->NextSubviewBelowLeft();
		}
	pr= new DPrompt( 0, super, "Total bytes:", 0, 0, Nlm_programFont);	 		
	super->NextSubviewBelowLeft();
	pr= new DPrompt( 0, super, "Received:", 0, 0, Nlm_programFont);	 		
	super->NextSubviewBelowLeft();
	pr= new DPrompt( 0, super, "Time:", 0, 0, Nlm_programFont);	 		
	super->NextSubviewBelowLeft();
	pr= new DPrompt( 0, super, "Bytes/sec:", 0, 0, Nlm_programFont);	 		
	super->NextSubviewBelowLeft();

			// right side
	char *kFillnum = "     000000";
	this->NextSubviewToRight();
	clus= new DCluster(0, this, 0, 0, true);
	super = clus;
	if (fParentGo->fDate) {
		prDate= new DPrompt( 0, super, kFillnum, 0, 0, Nlm_programFont, justright);	 		
		prDate->SetTitle( fParentGo->fDate);
		super->NextSubviewBelowLeft();
		}	
	fBytesAvail= 	new DPrompt( 0, super, kFillnum, 0, 0, Nlm_programFont, justright);	 		
	super->NextSubviewBelowLeft();
	fBytesGot= 		new DPrompt( 0, super, kFillnum, 0, 0, Nlm_programFont, justright);	 		
	super->NextSubviewBelowLeft();	
	fTimeSpent= 	new DPrompt( 0, super, kFillnum, 0, 0, Nlm_programFont, justright);	 		
	super->NextSubviewBelowLeft();	
	fBytesPerSec= new DPrompt( 0, super, kFillnum, 0, 0, Nlm_programFont, justright);	 		
	super->NextSubviewBelowLeft();
	
	if (fMapper && (fMapper->fHandlerType == 2 || fMapper->fHandlerName)) {   // 2 == kExternalHandler !! fix this??
		fLaunch= new DButton( kLaunchBut, super, "Launch");
		fLaunch->Disable();
		}
	
	//fParentGo->fTransferType= kTransferBinary; // ?? is this messed up ??
	if (fParentGo->fThreadState == DGopher::kThreadNotStarted) 
		fParentGo->ReadTask();   
	if (fParentGo->fThreadState == DGopher::kThreadDoneReading) { 
		fParentGo->fThreadState= DGopher::kThreadNotStarted; // ! need to reset this somewhere for used gophers
		this->suicide(); //delete this; 
		return; 
		}
	//fParentGo= fParentGo;
	DTask* aSaveTask= newTask(cSave, kGoFiledoc);
	aSaveTask->SetRepeat(true);
	this->PostTask( aSaveTask); // ?? bug in gTaskCentral, getting only ReadTask in queue...

	DWindow::Open();
}










