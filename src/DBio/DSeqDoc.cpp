// DSeqDoc.cp

#define ETEXT 1


#include "DSeqDoc.h"
#include <ncbi.h>
#include <dgg.h>
#include <Dvibrant.h>
#include <DApplication.h>
#include <DClipboard.h>
#include <DControl.h>
#include <DWindow.h>
#include <DRichViewNu.h>
#include <DTableView.h>
#include <DViewCentral.h>
#include <DTask.h>
#include <DTracker.h>
#include <DMenu.h>
#include <DUtil.h>
#include <DFindDlog.h>
#include "DRichHandler.h"
#include "DSeqFile.h"
#include "DSeqEd.h"
#include "DSeqMail.h"
#include "DSeqCmds.h"
#include "DSeqChildApp.h"
#include "DSeqPrint.h"
#include "DSeqPict.h"
#include "DSeqViews.h"

//#include "DSeqAsmView.h"



Global char* gDefSeqName = NULL;
static char gDefSeqNameStore[128];
Boolean  DSeqDoc::fgTestSeqFile = false; // turn on/off seq-test for OpenDocument()
Boolean  DSeqDoc::fgStartDoc = true;
short		 DSeqDoc::fgMatKind= 0;
short		 DSeqDoc::fgDistCor= 0;
Boolean  DSeqDoc::fgLockText = false;  
short		 DSeqDoc::fgViewMode = DAlnView::kModeSlide;
//Boolean  DSeqDoc::fgUseColor = false;  
short		 DSeqDoc::fgUseColor = DAlnView::kBaseColor;
Nlm_RecT DSeqDoc::fgWinRect = { 0, 0, 0, 0 };
char *	 DSeqDoc::fgSire = "SPup";




class DSeqFormatPopup : public DPopupList
{
public:
	DSeqFormatPopup(long id, DView* superior, short defaultFormat);
	virtual short GetValue() { return DSeqFile::kMinFormat + DPopupList::GetValue() - 1; }
};

DSeqFormatPopup::DSeqFormatPopup(long id, DView* superior, short defaultFormat) :
			DPopupList(id,superior,true)
{
	for (short i= DSeqFile::kMinFormat; i<=DSeqFile::kMaxFormat; i++) 
		this->AddItem( (char*)DSeqFile::FormatName(i));
	this->SetValue(defaultFormat);
}



class DAlnModePopup : public DPopupList
{
public:
	DAlnModePopup(long id, DView* superior, short defaultvalue);
	virtual short GetValue() { return DAlnView::kModeSlide + DPopupList::GetValue() - 1; }
};

DAlnModePopup::DAlnModePopup(long id, DView* superior, short defaultvalue) :
			DPopupList(id,superior,true)
{
	AddItem( "Slide bases");
	AddItem( "Edit bases");
	AddItem( "Select mask 1");
	AddItem( "Select mask 2");
	AddItem( "Select mask 3");
	AddItem( "Select mask 4");
	this->SetValue(defaultvalue+1);
}


class DAlnColorPopup : public DPopupList
{
public:
	DAlnColorPopup(long id, DView* superior, short defaultvalue);
	virtual short GetValue() { return DAlnView::kBaseBlack + DPopupList::GetValue() - 1; }
};

DAlnColorPopup::DAlnColorPopup(long id, DView* superior, short defaultvalue) :
			DPopupList(id,superior,true)
{
	AddItem( "Black");
	AddItem( "Color");
	AddItem( "Align");
	AddItem( "Back color");
	AddItem( "Back align");
	this->SetValue(defaultvalue +1 - DAlnView::kBaseBlack);
}



// class DAlnFindDialog

class DAlnFindDialog : public DFindDialog {
public:
	DSequence* fSequence;
	DAlnView* fAlnView;
	short 	fRow;
	char	* fLastTarget;
	long		fLastMatch;
	DAlnFindDialog( DSequence* itsSeq, DAlnView* itsView);
	virtual void DoFind();
	virtual void BuildDlog();
};

DAlnFindDialog::DAlnFindDialog( DSequence* itsSeq, DAlnView* itsView) :
		fSequence(itsSeq), fAlnView( itsView), 
		fLastTarget(NULL), fRow(-1), fLastMatch(-1)
{
}

void DAlnFindDialog::BuildDlog()
{
	DFindDialog::BuildDlog();

	DView* av;
	av= FindSubview(replaceId); if (av) { av->Disable(); } //av->Hide();
	av= FindSubview(replaceFindId); if (av) { av->Disable();  }
	av= FindSubview(replaceAllId); if (av) { av->Disable(); }
	av= FindSubview(cBackwards); 	if (av) { av->Disable(); }
	av= FindSubview(cFullWord); if (av) { av->Disable(); }
	av= FindSubview(cCaseSense); if (av) { av->Disable(); }
	if (fReplaceText) fReplaceText->Disable();
}

void DAlnFindDialog::DoFind() 
{
	long	firstBase, nBases, match;
	const char	*target;
	Boolean back;
	Nlm_RecT selr;
	long		selrow;
	
	//fAlnView->DeInstallEditSeq(); //?? reinstall after ?
	fSequence= fAlnView->SelectedSequence(selrow);
	fRow= selrow;
	if (fSequence) {
		fSequence->UpdateFlds();  
		selr= fAlnView->GetSelRect();
		selr.top= fRow;
		selr.bottom= fRow+1; //??
		firstBase= selr.left;
		nBases= 0; //selr.right - firstBase;
		fSequence->SetSelection( firstBase, nBases);
		target= this->GetFind();
		back	= this->Backwards();
		if (target && *target) {
			if (fLastTarget && (StringCmp(target, fLastTarget)==0) && firstBase == fLastMatch)
				match= fSequence->SearchAgain();
			else {
				if (fLastTarget) MemFree(fLastTarget);
				fLastTarget= StrDup(target);
				match= fSequence->Search( (char*)target, back);
				}
			fLastMatch= match;
			if (match>=0) {
				nBases= StrLen(target);
				fSequence->SetSelection( match, nBases);
				selr.left= match;
				selr.right= match + nBases;
				fAlnView->SelectCells( selr);  
				if (fAlnView->fEditSeq && fAlnView->fEditRow == selrow)
					fAlnView->fEditSeq->Select(match, nBases);
				}
			}
		}
}				



//class DSeqDoc


DSeqDoc::DSeqDoc( long id, DSeqList* itsSeqList, char* name) :
		DWindow( id, gApplication),
		fAlnView(NULL), fAlnIndex(NULL), fAlnITitle(NULL),
		fSeqList(itsSeqList), fPrintDoc(NULL),
		fHeadline(NULL), fSeqMeter(NULL), fFormatPop(NULL),
		fUpdateTime(0), fDocTitle(NULL),
		fInFormat(DSeqFile::kGenBank), // kNoFormat
		fSaveSelection(false),
		//fLockButton(NULL), fColorButton(NULL), fMonoButton(NULL),
		fColorCheck(NULL), fLockCheck(NULL),
		fUseColor(false)
{ 
	short width= -1, height= -1, left= -10, top= -20; // default window loc
	if (gTextFont == NULL) gTextFont= Nlm_programFont;
	if (gSeqFont == NULL) gSeqFont= Nlm_programFont;

	if (!Nlm_EmptyRect(&fgWinRect))  {
			// these two are causing resize to fail >> width & height setting !!
		//width= MAX( 40, fgWinRect.right - fgWinRect.left);
		//height= MAX( 50, fgWinRect.bottom - fgWinRect.top);
		left= MAX(20,fgWinRect.left);
		top = MAX(40,fgWinRect.top);
		}

	this->InitWindow( document, width, height, left, top, name); 
	gDefSeqName= name; 
	
	if (!fSeqList) fSeqList= new DSeqList();  
	//fSeqList->AddNewSeq();	// install 1 blank seq for new doc
}


DSeqDoc::~DSeqDoc()
{
	fAlnView->DeInstallEditSeq(); 
	if (fSeqList) { 
		//fSeqList->FreeAllObjects(); 
		//fSeqList->suicide(); // need this for other users of list ??
		fSeqList->fDeleteObjects= true; 
		delete fSeqList; // knows how to delete
		}
}

void DSeqDoc::FreeData()
{
	fAlnView->DeInstallEditSeq(); 
	if (fSeqList) { 
		//fSeqList->FreeAllObjects(); // DaMN !! THIS IS NOT CALLING Seq Destructors !!
		long i, n= fSeqList->GetSize();
		for (i=0; i<n; i++) {
			DSequence* aseq= fSeqList->SeqAt(i);
			delete aseq;
			}
		fSeqList->DeleteAll();
		}
}


Boolean DSeqDoc::IsSeqFile(DFile* aFile)
{
	// this is a static function, usable w/o a DGopherListDoc object
	if (fgTestSeqFile && aFile && aFile->Exists()) {
		aFile->OpenFile();
		short format= DSeqFile::SeqFileFormatWrapper( aFile);
		return (format != DSeqFile::kUnknown);
		}
	else
		return false;
}

void DSeqDoc::MakeGlobalsCurrent()
{
	ViewRect( fViewrect); // get current rect...
	if (!Nlm_EmptyRect(&fViewrect)) fgWinRect= fViewrect;  
	if (fLockCheck) fgLockText= fLockCheck->GetStatus();
	//if (fColorCheck) fgUseColor= fColorCheck->GetStatus();
	if (fColorPop) fgUseColor= fColorPop->GetValue();
	if (fModePop) fgViewMode= fModePop->GetValue();
}

void DSeqDoc::Close()
{
#if 1
	DList* winlist= gWindowManager->GetWindowList();
	short  i, nwin= winlist->GetSize();
	for (i=0; i<nwin; i++) {
		DWindow* awin= (DWindow*) winlist->At(i);
		if (awin->Id() == DSeqedWindow::kId 
			&& ((DSeqedWindow*) awin)->fMainDoc == this) { 
				delete awin; // ?? or awin->Close();
			 	}
		}
#endif
	
	MakeGlobalsCurrent();
	DWindow::Close();
}

void DSeqDoc::ResizeWin()
{
	DWindow::ResizeWin();
	MakeGlobalsCurrent();
}

void DSeqDoc::Open(DFile* aFile)
{
	Boolean isempty= (fSeqList->GetSize() < 1);
	if ( ReadFrom( aFile, true) ) {
		if (isempty) {
			this->SetTitle((char*)aFile->GetShortname());
			this->SetFilename((char*)aFile->GetName());
			}
		this->Open();
		}
	else 
		if (isempty) this->suicide();
}


void DSeqDoc::Revert()
{
	char buf[512];
	char *name= GetFilename(buf, sizeof(buf));
	DFile aFile(name);
	fAlnView->DeInstallEditSeq(); 
	if ( ReadFrom( &aFile, false) ) {
		//this->Open();
		DSaveHandler::NotDirty();
		fAlnView->GetReadyToShow();  
		fAlnView->Invalidate();
		fAlnITitle->GetReadyToShow();
		if (fAlnIndex){
			fAlnIndex->GetReadyToShow();
			fAlnIndex->Invalidate();
			}
		}
}


// static
void DSeqDoc::NewSeqDoc()
{
	DSeqDoc* newdoc= new DSeqDoc( DSeqDoc::kSeqdoc, NULL, (char*)DFileManager::kUntitled);
	newdoc->Open();
	newdoc->EditSeqs(); //?? make it real obvious to beginners?
}

// static
void DSeqDoc::GetGlobals()
{
	char* onoffs;

	SeqDocPrefs(kSeqDocPrefInit);
	DSeqPrintDoc::GetGlobals();
	SeqPrintPrefs(kSeqPrintPrefInit);

	onoffs= (fgLockText) ? "1" : "0";
	fgLockText= gApplication->GetPrefVal( "fgLockText", "windows", onoffs);
	//onoffs= (fgUseColor) ? "1" : "0";
	//fgUseColor= gApplication->GetPrefVal( "fgUseColor", "windows", onoffs);
	fgUseColor= gApplication->GetPrefVal( "fgUseColor", "windows", "0");
	fgViewMode= gApplication->GetPrefVal( "fgViewMode", "seqdoc", "0");
	fgSire= gApplication->GetPref( "fgSire", "seqdoc", "SApp");
	//DSeqFile::fgSire= fgSire; // messy !
	//DSeqList::fgSire= fgSire; // messy !
	DFile::fgSire= fgSire; // messy !
	
	{
	//350, 200
	char* srect = gApplication->GetPref( "fgWinRect", "windows", "30 40 450 220");
#if 1
		// sscanf is failing on Mac/codewar !! used to work
	if (srect) {
		char* cp= srect;
		while (*cp && isspace(*cp)) cp++;
		fgWinRect.left= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		fgWinRect.top= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		fgWinRect.right= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		fgWinRect.bottom= atoi( cp);
		}
#else
		if (srect) sscanf( srect, "%d%d%d%d", &fgWinRect.left, &fgWinRect.top, 
												&fgWinRect.right, &fgWinRect.bottom);
#endif
		MemFree(srect);
	}
}

// static
void DSeqDoc::SaveGlobals()
{
	gApplication->SetPref( (int) fgLockText,"fgLockText","windows");
	gApplication->SetPref( (int) fgUseColor, "fgUseColor","windows");
	gApplication->SetPref( (int) fgViewMode, "fgViewMode","seqdoc");
	gApplication->SetPref( fgSire, "fgSire","seqdoc");

	if (!Nlm_EmptyRect(&fgWinRect)) {
		char  srect[128];
		sprintf( srect, "%d %d %d %d", fgWinRect.left, fgWinRect.top, 
														fgWinRect.right, fgWinRect.bottom);
		gApplication->SetPref( srect, "fgWinRect", "windows");
		}
}


void DSeqDoc::AddAlnIndex(DView* super, short width, short height)
{
	fAlnIndex= new DAlnIndex(0, super, this, fSeqList, width, height); 
}

void DSeqDoc::AddAlnView(DView* super, short width, short height)
{
	fAlnView= new DAlnView(0, super, this, fSeqList, width, height); 
}

void DSeqDoc::AddModePopup(DView* super)
{
	fModePop= new DAlnModePopup(kModePopup, super, fgViewMode);
}

void DSeqDoc::AddTopViews(DView* super)
{
	DPrompt* pr;

	pr= new DPrompt(0,super,"Color:",0, 0, Nlm_programFont);
	super->NextSubviewToRight();
	fColorPop= new DAlnColorPopup(kColorPopup,super,fgUseColor);
	fUseColor= fgUseColor;
	super->NextSubviewToRight();

	pr= new DPrompt(0,super," View:",0, 0, Nlm_programFont);
					
	super->NextSubviewToRight();
	this->AddModePopup(super); 
	fLockCheck= NULL;

	super->NextSubviewToRight();
	pr= new DPrompt(0,super,"  File format:",0, 0, Nlm_programFont);

	DCluster* metergrp= new DCluster( 0, this, 0, 0, true);   // was 0,0 for w,h
	pr= new DPrompt(0,metergrp,"Base#",0, 0, Nlm_programFont);
	metergrp->NextSubviewToRight();
	fSeqMeter= new DPrompt(0,metergrp,"0000000",0,0,Nlm_programFont,justright); //60,14

	super->NextSubviewToRight();
	fFormatPop= new DSeqFormatPopup(0,super,fInFormat);
	super->NextSubviewBelowLeft();
}


void DSeqDoc::AddViews()
{
	DView* super;
	short width, height;
	Nlm_PoinT		nps;
	DPrompt* pr;
	
	if (!fAlnView || !fAlnIndex || !fAlnHIndex || !fAlnITitle) {
		super= this;
		
		AddTopViews( super);

			// left group : titleline over name table
		DCluster* grouper= new DCluster( 0, this, 0, 0, true);   // was 0,0 for w,h
		grouper->SetResize( DView::fixed, DView::relsuper);
		super= grouper;

		fAlnITitle= new DAlnITitle(0, super, 80, Nlm_stdLineHeight);
		super->NextSubviewBelowLeft();

		//width= 350; height= 200;
		super->GetNextPosition( &nps);
		width = MAX( 40, fgWinRect.right - fgWinRect.left) - Nlm_vScrollBarWidth  - nps.x; 
		height= MAX( 60, fgWinRect.bottom - fgWinRect.top) - Nlm_hScrollBarHeight - nps.y;   
		
		this->AddAlnIndex( super, 80, height);

		super= this;
		super->NextSubviewToRight();	
 	
		super->GetNextPosition( &nps);
		nps.x += 3;
		super->SetNextPosition( nps);
 
			// right group : index topline over bases table
		grouper= new DCluster( 0, this, 0, 0, true); // was 0,0 wid,hgt
		grouper->SetResize( DView::relsuper, DView::relsuper);
		super= grouper;
		super->GetNextPosition( &nps);
		width = MAX( 40, fgWinRect.right - fgWinRect.left) - Nlm_vScrollBarWidth  - nps.x; 
		height= MAX( 60, fgWinRect.bottom - fgWinRect.top) - Nlm_hScrollBarHeight - nps.y; 
		fAlnHIndex= new DAlnHIndex(0, super, this, fSeqList, width, Nlm_stdLineHeight);
		super->NextSubviewBelowLeft();
		
		//width= 350; height= 200;
		super->GetNextPosition( &nps);
		width = MAX( 40, fgWinRect.right - fgWinRect.left) - Nlm_vScrollBarWidth  - nps.x; 
		height= MAX( 60, fgWinRect.bottom - fgWinRect.top) - Nlm_hScrollBarHeight - nps.y; 
		this->AddAlnView( super, width, height);
		fAlnView->SetTextLock( fgLockText);
		super= this;

#if ETEXT
		//if (gLastEditView) gLastEditView->DeInstallEditSeq();
		{
		short charwidth= 0;
    Nlm_PoinT  nps, saveps;
		GetNextPosition( &saveps);
		nps.x= 1; nps.y= 1; // locate dialog item 
		SetNextPosition( nps);
		//gDialogTextMultiline= false;

		charwidth= width / fAlnView->GetItemWidth();

		DAlnSequence* eseq= new DAlnSequence( 0, fAlnView, charwidth);  
		fAlnView->fEditSeq= eseq;
		//this->SetEditText(eseq);
		eseq->fVisible= true;
		eseq->HideEdit(); // eseq->Disable(); eseq->Hide();
		this->SetEditText(NULL);  
		SetNextPosition( saveps);
		}
#endif
		}
		
	if (!fFindDlog) fFindDlog= new DAlnFindDialog( NULL, (DAlnView*)fAlnView);
}



void DSeqDoc::Open()
{
	Boolean selectone= false;
	
	if (!fSeqList) fSeqList= new DSeqList();
	if (fSeqList->GetSize() == 0) {
		fSeqList->AddNewSeq();	// install 1 blank seq for new doc
		selectone= true;
		}
	
	AddViews();
	fSaveHandler= this;  
	fPrintHandler= this; 

	this->Select(); // for motif
#if 0
	fAlnView->GetReadyToShow(); // do before others ...
	if (fAlnIndex) fAlnIndex->GetReadyToShow();
	fAlnITitle->GetReadyToShow();

	if (0) {
			// try auto-sizing aln view to data
			//long ns= fSeqList->GetSize();
	long ht = 0, nrow= fAlnView->GetMaxRows();
	if (nrow > 2) {
		ht = 80 + fAlnITitle->GetItemHeight(0);  
		if (nrow > 25) nrow= 25; // max size?
		for (long i= 0; i<nrow; i++) ht += fAlnView->GetItemHeight(i);
		Nlm_RecT r;
		GetPosition(r);
		r.bottom = r.top + ht;
		SetPosition(r);
		}
	}
	this->CalcWindowSize();

#else
	this->CalcWindowSize();
	fAlnView->GetReadyToShow(); // do before others ...
	if (fAlnIndex) fAlnIndex->GetReadyToShow();
	fAlnITitle->GetReadyToShow();
	//fAlnHIndex->GetReadyToShow();

#endif

	fAlnView->SetViewColor(fgUseColor);
	fAlnView->SetViewMode(fgViewMode);
	if (selectone && fAlnIndex) fAlnIndex->SelectCells(0,0);
	fSeqMeter->Show();  
	
	DWindow::Open();
}


void DSeqDoc::Activate()
{
	if (fAlnView && fAlnView->fEditSeq && fAlnView->fEditSeq->fVisible) 
		fAlnView->fEditSeq->selectAction();
	gCursor->arrow();
	DWindow::Activate();
}

void DSeqDoc::Deactivate()
{
	if (fAlnView && fAlnView->fEditSeq) fAlnView->fEditSeq->deselectAction();
	DWindow::Deactivate();
}

void DSeqDoc::Print()
{
	// fix this to print more of window !
	//fAlnView->Print();
	
	// or do prettyprint...
	MakeSeqPrint( false);
	fPrintDoc->PrintDoc();
}

Boolean DSeqDoc::IsMyTask(DTask* theTask) 
{
#if 0
	if (theTask->fKind == kSeqDoc) {
		ProcessTask( theTask);
		return true;
		}
	else 
#endif
		return DWindow::IsMyTask(theTask);
}

void DSeqDoc::ProcessTask(DTask* theTask) 
{
	if (theTask->fNumber == -987654) {

		}
	else {
		DWindow::ProcessTask(theTask);
		}
}


void DSeqDoc::SortView(DSeqList::Sorts sortorder)
{
	fSeqList->SortList(sortorder);
	fAlnView->Invalidate();
	if (fAlnIndex) fAlnIndex->Invalidate();
}


void DSeqDoc::AddSeqToList(DSequence* item)
{
	fAlnView->addToAlnList( item);
	//fAlnView->SelectCells( fAlnView->GetMaxRows()-1, fAlnView->GetMaxCols());
	if (fAlnIndex) fAlnIndex->SelectCells( fAlnView->GetMaxRows()-1, 0); 
#if 0
	this->Changed( 1, this);
#endif
}	



void DSeqDoc::AddNewSeqToList()
{
	DSequence* aSeq = new DSequence(); 
	AddSeqToList( aSeq);
}
 
 
void DSeqDoc::FirstSelection( DSequence*& aSeq, long& start, long& nbases)
{	
	long			left, right, top, bottom, atRow;

	start= nbases= atRow= 0;
	fAlnView->DeInstallEditSeq(); //?? reinstall after ?
	fAlnView->GetFirstSelectedCell(top,left);
	fAlnView->GetLastSelectedCell(bottom,right);
	
	if (left !=  DTabSelection::kNoSelection && top !=  DTabSelection::kNoSelection 
	 && (left != right || top != bottom)) {
	  atRow = top;
		start = left; // - 1;
		nbases= right - start;  
		}	
	else if (fAlnIndex && fAlnIndex->IsSelected()) {
		fAlnIndex->GetFirstSelectedCell(top,left);
		fAlnIndex->GetLastSelectedCell(bottom,right);
		atRow= top;
		start= 0; 
		nbases= 0; //key for all of seq
		}	

	aSeq= fAlnView->SeqAt( atRow);
	if (aSeq) {
		aSeq->SetIndex(atRow);
		aSeq->SetSelection( start, nbases);
		}
}

void DSeqDoc::AddSeqAtToList( long aRow, long start1, long nbases1, 
														DSeqList*& aSeqList, long& start, long& nbases)
{
	DSequence* aSeq= fAlnView->SeqAt( aRow); 
	if (aSeq) {
		aSeq->SetIndex( aRow);
		aSeq->SetSelection( start1, nbases1);
		aSeqList->InsertLast( aSeq);
		start= Max(start,start1);
		if (nbases==0) nbases= nbases1;
		else nbases= Min(nbases,nbases1);
		}
}

void DSeqDoc::AddMaskedSeqToList( long aRow, short masklevel,
									DSeqList*& aSeqList, long& start, long& nbases)
{
	DSequence* aSeq= fAlnView->SeqAt( aRow); 
	if (aSeq && aSeq->MasksOk()) {
		aSeq->ClearSelection();
		aSeq= aSeq->CompressFromMask(masklevel);
			// !! NOTE these Seqs are NEW -- delete when aSeqList is deleted
		if (!aSeq) return;
		if (aSeq->LengthF() == 0) { delete aSeq; return; }
		aSeq->SetIndex( aRow);
		aSeqList->InsertLast( aSeq);
		aSeqList->fDeleteObjects= true;  // so NEW seqs are deleted
		start= 0;
		if (nbases==0) nbases= aSeq->LengthF();
		else nbases= Min(nbases,aSeq->LengthF());
		}
}

#if 0
enum  selectforms { 
	kSeqSel = 1, kNoSeqSel = 0,
	kMaskSel = 2, kNoMaskSel= 0,
	kIndexSel = 4, kNoIndexSel = 0,
	kAllIfNone = 8, kNoneIfNone = 0,
	kEqualCount = 16, kUnequalCount = 0,
	};
  // kSeqSel+kMaskSel+kIndexSel+kAllIfNone+kEqualCount
#endif
  
void DSeqDoc::GetSelection( long selectFlags, DSeqList*& aSeqList, long& start, long& nbases)
// GetSelection(Boolean equalCount, Boolean allAtNoSelection,
{
	long			left, right, top, bottom, arow, acol;
	long			start1, nbases1;
	Boolean		equalCount, allAtNoSelection, useSeqSelection, useMasks;

	equalCount = (selectFlags & kEqualCount);
	allAtNoSelection = (selectFlags & kAllIfNone);
	useMasks = (selectFlags & kMaskSel);
	useSeqSelection = (selectFlags & kSeqSel);
	
	fAlnView->DeInstallEditSeq(); //?? reinstall after ?
	start= 0; start1= 0;
	nbases= 0; nbases1= 0;
	aSeqList= new DSeqList();
	useMasks= useMasks && (fAlnView->fMaskLevel > 0);
	
	fAlnView->GetFirstSelectedCell(top,left);
	fAlnView->GetLastSelectedCell(bottom,right);
	useSeqSelection = useSeqSelection && (!useMasks 
			&& left != DTabSelection::kNoSelection 
			&& top != DTabSelection::kNoSelection 
	 		&& (left != right || top != bottom));
	
#if 0
		// THIS IS STILL BUGGY -- need to TEST & FIX
	if (useMasks && fAlnView->IsMasked()) {
		top= 0;
		bottom= fSeqList->GetSize();
		if (fAlnIndex && fAlnIndex->IsSelected()) {
			fAlnIndex->GetFirstSelectedCell(top,left);
			fAlnIndex->GetLastSelectedCell(bottom,right);
			}
		for (arow= top; arow<bottom; arow++)  
			AddMaskedSeqToList( arow, fAlnView->fMaskLevel, 
														aSeqList, start, nbases);
		}		
	else 
#endif

	if (useSeqSelection) {
		for (arow= top; arow<bottom; arow++) {
			if (equalCount) acol= left; else acol= 0; 
			
			while (acol<right && !fAlnView->IsSelected(arow,acol)) acol++;
			start1= acol; // - 1;
			if (equalCount)
				while (acol<right && fAlnView->IsSelected(arow,acol)) acol++;
			else 
				while (fAlnView->IsSelected(arow,acol)) acol++;
			nbases1= acol - start1;
			
			AddSeqAtToList( arow, start1, nbases1, aSeqList, start, nbases);
			}
		start = left; // - 1;
		nbases= right - start;  
		}	
		
	else if (fAlnIndex && fAlnIndex->IsSelected()) {
		fAlnIndex->GetFirstSelectedCell(top,left);
		fAlnIndex->GetLastSelectedCell(bottom,right);
		for (arow= top; arow<bottom; arow++) 
			AddSeqAtToList( arow, start1, nbases1, aSeqList, start, nbases);
		}	
		
	else if (allAtNoSelection) {
		 for (arow= 0; arow<fSeqList->GetSize(); arow++) 
			AddSeqAtToList( arow, start1, nbases1, aSeqList, start, nbases);
		}
}
			

short DSeqDoc::SelectionToFile(Boolean AllatNoSelection, char* aFileName, short seqFormat)  
{
	long		start, nbases;
	long		nseqs;
	DSeqList *aSeqList;
 
	GetSelection( kSeqSel+kMaskSel+kIndexSel+kAllIfNone, aSeqList, start, nbases);
	nseqs= aSeqList->GetSize();
	aSeqList->DoWrite( aFileName, seqFormat); 
	delete aSeqList;
	return nseqs;
}


void DSeqDoc::NotDirty()
{
	DSaveHandler::NotDirty();
	long i, nseqs= fSeqList->GetSize();
	for (i=0; i<nseqs; i++) {
		fSeqList->SeqAt(i)->SetChanged(false);
		}
	if (fAlnIndex) fAlnIndex->Invalidate();
}  


void DSeqDoc::WriteTo(DFile* aFile)  
{
	short outformat= DSeqFile::kGenBank;
	if (fFormatPop) outformat= fFormatPop->GetValue(); 
	fAlnView->DeInstallEditSeq(); //?? reinstall after ?
	if (fSaveSelection) {
		long		start, nbases;
		DSeqList* aSeqList= NULL;
		fSaveSelection= false;
		GetSelection( kSeqSel+kMaskSel+kIndexSel+kAllIfNone, aSeqList, start, nbases);
		aSeqList->DoWrite( aFile, outformat);
		delete aSeqList;
		}	
	else {
		fSeqList->ClearSelections(); //! make sure we write all of seq from this call !?
		fSeqList->DoWrite( aFile, outformat);
		this->NotDirty();  
		}
}





Boolean DSeqDoc::ReadFrom(DFile* aFile, Boolean append)	// revise for iostreams
{
	if (!append) FreeData();   //?? or leave here for append...
	gDefSeqName= this->GetTitle(gDefSeqNameStore,sizeof(gDefSeqNameStore));
	fInFormat= DSeqFile::ReadSeqFile(aFile, fSeqList);  
	return true; //?? (fInFormat != DSeqFile::kUnknown)
}




void DSeqDoc::OpenSeqedWindow(DSequence* aSeq)
{
	char* title= StrDup( aSeq->Name());
	StrExtendCat( &title, " Edit");
	DSeqedWindow* edwin = new DSeqedWindow( DSeqedWindow::kId, gApplication, 
												this, aSeq, -5, -5, -50, -20, title);
	MemFree( title);
	edwin->Open(); 
	edwin->Select();	 
}

 

void DSeqDoc::EditSeqs() 
{
	DSequence* aSeq= fAlnView->SelectedSequence();  
	if (aSeq) this->OpenSeqedWindow(aSeq);
}




void DSeqDoc::ToTextDoc()
{
	DTempFile* tmpFile= new DTempFile();
	this->WriteTo(tmpFile);
	DRichTextDoc* doc= new DRichTextDoc(0,  true, gTextFont);
	doc->Open(tmpFile);
	delete tmpFile;
}


void DSeqDoc::MakeSeqPrint(Boolean doREMap)
{
	long		firstbase, nbases;
	DSeqList* aSeqList= NULL;
	fSaveSelection= false;
	DWindow	* aDoc;
	
	GetSelection( kSeqSel+kMaskSel+kIndexSel+kAllIfNone, //+kEqualCount??
								aSeqList, firstbase, nbases);
	if (!aSeqList)
		return;
	else if (doREMap) {
		aDoc = new DREMapPrintDoc( DSeqPrintDoc::kSeqPrintDoc, this, aSeqList, firstbase, nbases);
		aDoc->Open();  
		}
	else if (aSeqList->GetSize() > 1) {
		aDoc = new DAlnPrintDoc( DSeqPrintDoc::kSeqPrintDoc, this, aSeqList, firstbase, nbases);
		aDoc->Open();  
		}		
	else {
		aDoc = new DSeqPrintDoc( DSeqPrintDoc::kSeqPrintDoc, this, aSeqList, firstbase, nbases);
		aDoc->Open();  
		}
	fPrintDoc= aDoc;
}

void DSeqDoc::MakeAlnPrint()
{
	MakeSeqPrint(false);
}



void DSeqDoc::SetUpMenu(short menuId, DMenu*& aMenu) 
{
	//DWindow::SetUpMenu(menuId, aMenu);

	if (menuId == kSeqMenu) {
		if (!aMenu) aMenu = gApplication->NewMenu( menuId, "Sequence");
		//aMenu->AddItem( cSeqPrefs, "Seq Prefs...");	
	
		aMenu->AddItem( cNewSeq, "New sequence");
		aMenu->AddItem( cEditSeq, "Edit seq...");
	 
		DMenu* vMenu= new DSubMenu( kViewKindMenu, aMenu, "View seqs");
		vMenu->AddItem( kViewByDefault, "default");
		vMenu->AddItem( kViewByDate, "by Date");
		vMenu->AddItem( kViewBySize, "by Size");
		vMenu->AddItem( kViewByName, "by Name");
		vMenu->AddItem( kViewByKind, "by Kind");
		vMenu->AddItem( kViewAsText, "as Text");

		DMenu* mMenu= new DSubMenu( kSeqMaskMenu, aMenu, "Seq mask");
		mMenu->AddItem(cMaskSelAll,"Mask all",false, true);
		mMenu->AddItem(cMaskInvert,"Invert mask",false, true);
		mMenu->AddItem(cMaskClear,"Clear mask",false, true);
		mMenu->AddSeparator();

		mMenu->AddItem(cSel2Mask,"Selection to Mask",false, true);
		mMenu->AddItem(cMask2Sel,"Mask to Selection",false, true);
		mMenu->AddItem(cMaskOrSel,"Mask OR Selection",false, true);
		mMenu->AddItem(cMaskAndSel,"Mask AND Selection",false, true);
		//gViewCentral->DisableView(cMask2Sel);
		//gViewCentral->DisableView(cMaskOrSel);
		//gViewCentral->DisableView(cMaskAndSel);
		mMenu->AddSeparator();
		mMenu->AddItem(cMaskCompress, "Compress by mask");
		mMenu->AddItem(cMaskSelCommon,"Mask consensus",false, true);
		mMenu->AddItem(cMaskSelORF,"Mask ORFs",false, true);
		mMenu->AddItem(cMaskReplicate,"Replicate mask to all seqs",false, true);

		aMenu->AddSeparator();
		aMenu->AddItem( cRevSeq, "Reverse");
		aMenu->AddItem( cCompSeq, "Complement");
		aMenu->AddItem( cRevCompSeq, "Rev-Compl");
		aMenu->AddItem( cDna2Rna, "Dna->Rna");
		aMenu->AddItem( cRna2Dna, "Rna->Dna");
		aMenu->AddItem( cToUpper, "UPPER Case");
		aMenu->AddItem( cToLower, "lower Case");

		aMenu->AddItem( cDegap, "Degap");
		aMenu->AddItem( cLockIndels, "Lock indels");
		aMenu->AddItem( cUnlockIndels, "Unlock indels");
		aMenu->AddItem( cConsensus, "Consensus");
		aMenu->AddItem( cTranslate, "Translate");
		aMenu->AddItem( cDistance, "Distance");
		//aMenu->AddItem( cSimilarity, "Similarity");

		aMenu->AddSeparator();
		aMenu->AddItem( cPrettyPrint, "Pretty Print...");
		aMenu->AddItem( cREMap, "Restriction map...");
		aMenu->AddItem( cDotPlot, "Dot plot...");
		gViewCentral->DisableView(cDotPlot);

		aMenu->AddSeparator();		
		aMenu->AddItem( cNAcodes, "Nucleic codes...");
		aMenu->AddItem( cAAcodes, "Amino codes...");
	 	}
 	
	else if (menuId == DApplication::cEditMenu) {
		aMenu->AddSeparator();
		aMenu->AddItem(DApplication::kFind,"Find.../F",false, true);
		aMenu->AddItem(DApplication::kFindAgain,"Find again/G",false, true);
		aMenu->AddItem(cFindORF,"Find ORF/R",false, true);

		}
		
 	else if (menuId == kInternetMenu) {
		if (!aMenu) aMenu = gApplication->NewMenu( menuId, "Internet");
		aMenu->AddItem( 0, "[Mail servers]");
		aMenu->AddItem( cNCBIfetch, "NCBI fetch...");
		aMenu->AddItem( cNCBIblast, "NCBI BLAST search...");
		aMenu->AddItem( cEMBLfetch, "EMBL fetch...");
		aMenu->AddItem( cEMBLquicks, "EMBL Quicksearch...");
		aMenu->AddItem( cEMBLfasta, "EMBL FastA search...");
		aMenu->AddItem( cEMBLblitz, "EMBL Blitz search...");
		aMenu->AddItem( cFHCRCfetch, "FHCRC fetch...");
		aMenu->AddItem( cFHCRCblocks, "FHCRC Blocks search...");
		aMenu->AddItem( cGeneidSearch, "Geneid search...");
		aMenu->AddItem( cGrailSearch, "Grail search...");
		//aMenu->AddItem( cGenmarkSearch, "Genmark search...");
		//aMenu->AddItem( cPythiaSearch, "Pythia search...");
	 	//aMenu->AddSeparator();	
	 	}							

}




char* DSeqDoc::GetTitle(char* title, ulong maxsize) 
{ 
	if (fSaveSelection) {
		if (title==NULL) title= (char*) MemNew(maxsize); 
		DWindow::GetTitle( title, maxsize); 
#ifdef OS_DOS
		DFileManager::ReplaceSuffix( title, maxsize, ".sel");
#else
		StrNCat( title,".part", maxsize); 
#endif
		return title;
		}
	else if (fDocTitle && *fDocTitle) {
		if (title==NULL) {
			maxsize= Min(maxsize,StrLen(fDocTitle)+1);
			title= (char*) MemNew(maxsize); 
			}
		StrNCpy(title,fDocTitle,maxsize);
		return title;
		}
	else {
		DWindow::GetTitle( title, maxsize);
		//if (StringCmp(title,kUntitled)==0) *title= 0; // set to null so saveas is called
		return title;
		}
}

void DSeqDoc::FindORF()
{
	long	start, stop;
	DSequence* aSeq;
	long			selrow= 0;
	Nlm_RecT	selr;
	
	aSeq= fAlnView->SelectedSequence(selrow);
	if (aSeq) {
		selr= fAlnView->GetSelRect();
		if (selr.right > selr.left+1) selr.left++; // !? offset sel start so we can repeat call
		selr.top= selrow;
		selr.bottom= selrow+1; 
		aSeq->SetSelection( selr.left, 0);
		aSeq->SearchORF( start, stop);
		if (start>=0) {
			if (stop<start) stop= aSeq->LengthF(); //??
			aSeq->SetSelection( start, stop - start);
			selr.left= start;
			selr.right= stop;
			fAlnView->SelectCells( selr);
			if (fAlnView->fEditSeq && fAlnView->fEditRow == selrow)
				fAlnView->fEditSeq->Select(start, stop - start);
			}
		} 
}


void DSeqDoc::MakeConsensus()
{
	long	firstBase = 0, nBases = 0;
	DSeqList * aSeqList= NULL;
	DSequence* cons;
	
	GetSelection( kSeqSel+kMaskSel+kIndexSel+kAllIfNone+kEqualCount, 
								aSeqList, firstBase, nBases);
	if (aSeqList) {
		cons= fSeqList->Consensus();
		if (cons) {
			long aRow, totalRows;
			Nlm_RecT	r;
			aRow= fSeqList->GetIdentityItemNo( cons);
			totalRows= fSeqList->GetSize();
			fSeqList->Delete(cons);
			delete cons;
			fAlnView->GetRowRect( aRow, r, totalRows-aRow+1);
			fAlnView->InvalRect( r);
			if (fAlnIndex) {
				fAlnIndex->GetRowRect( aRow, r, totalRows-aRow+1);
				fAlnIndex->InvalRect( r);
				}
			fAlnView->UpdateSize();   
			}
			
		aSeqList->MakeConsensus();
		cons= aSeqList->Consensus();
		if (cons) {
			AddSeqToList( cons);
			//if (fAlnIndex) fAlnIndex->SelectCells( fSeqList->ConsensusRow(), 0); 
			//fAlnIndex->ScrollSelectionIntoView(TRUE);
			}
		delete aSeqList;
		}
}

void DSeqDoc::DistanceMatrix(short form)
{
	long	firstBase = 0, nBases = 0;
	DSeqList * aSeqList= NULL;
	short correction= fgDistCor; //DSeqList::DCnone; //DCnone, DCjukes, DColsen 
	char  title[128];
	
	this->GetTitle(title, sizeof(title));
	
	GetSelection( kSeqSel+kMaskSel+kIndexSel+kAllIfNone+kEqualCount, 
								aSeqList, firstBase, nBases);
	if (aSeqList) {			
		char* dmat = NULL;
		switch (form) {
			case 1: 
				DFileManager::ReplaceSuffix(title, sizeof(title), ".Similarities");
				dmat= aSeqList->Similarities(correction);  
				break;
			case 0: 
			default:
				DFileManager::ReplaceSuffix(title, sizeof(title), ".Distances");
				dmat= aSeqList->Distances(correction); 
				break;
			}
			
		if (dmat) {
			DRichTextDoc* doc= new DRichTextDoc();
			doc->fRichView->fDocFormat= DRichHandler::kTextformat;
			doc->SetTitle(title);
			doc->OpenText( dmat);
			}
		delete aSeqList;
		}
}


#if NETBLAST
extern "C" short CallNCBIBlastService( char* blastProgram, char* blastDatabase, char* blastOptions, 
													 char* fastaInputFile, char* outputFile );

void DSeqDoc::BLASTdialog()
{
	char* blastProgram = "blastn";
	char* blastDatabase = "nr";
	char* blastOptions = NULL;
	char* fastaInputFile = "spup-blast-input.fasta";
	char* outputFile = "spup-blast-result.text";
	char	instore[128];
	char  outstore[128];
	DFile aFile;
			
	fastaInputFile= DFileManager::TempFilename( instore);
	outputFile= DFileManager::TempFilename( outstore);

	short nseq= SelectionToFile( false, fastaInputFile, DSeqFile::kPearson);
	if (nseq) {
		short err= CallNCBIBlastService(blastProgram, blastDatabase, blastOptions, 
												fastaInputFile, outputFile);

		if (err) {
			Message(MSG_OK,"DSeqDoc::CallNCBIBlastService error %d", err);
			}
		else {
			DRichTextDoc* doc= new DRichTextDoc(0,  true, gTextFont);
			doc->Open(outputFile);
			aFile.Initialize(outputFile);
			aFile.Delete();
			}
	 	}
	aFile.Initialize(fastaInputFile);
	aFile.Delete();
}
#endif


Boolean DSeqDoc::DoMenuTask(long tasknum, DTask* theTask)
{
	DWindow* win = NULL;
	
	switch (tasknum) {
	
		case cNewSeq		: 
			AddNewSeqToList(); 
			//fall into EditSeqs
		case cEditSeq: 
			EditSeqs(); 
			return true;

		case cSaveSel:
			fSaveSelection= true;
			gApplication->DoMenuTask( DApplication::kSaveACopy, NULL);
			fSaveSelection= false;
			return true;

		case cRevert:
			Revert();
			return true;
			
		case cFindORF: 
			FindORF();
			return true;
			
		case cConsensus:
			MakeConsensus();
			return true;

		case cDistance:
			DistanceMatrix(fgMatKind);
			return true;

		case cSimilarity:
			DistanceMatrix(1);
			return true;
					
	
		case cRevSeq:
		case cCompSeq:
		case cRevCompSeq:
		case cDna2Rna: 
		case cRna2Dna:
		case cToUpper: 
		case cToLower:
		case cDegap:
		case cMaskCompress:
		case cLockIndels:
		case cUnlockIndels:
		case cTranslate:
			{
			long	firstBase = 0, nBases = 0;
			DSeqList * aSeqList= NULL;
			DSeqChangeCmd *	cmd = NULL;  
			
			//fAlnView->DeInstallEditSeq(); //?? reinstall after ?
			long flags;
			flags= kIndexSel+kAllIfNone; //+kEqualCount??
			if (tasknum != cMaskCompress) flags |= kMaskSel|kSeqSel;
			GetSelection( flags, aSeqList, firstBase, nBases);
			switch (tasknum) {
		  	case cRevSeq		: cmd= new DSeqReverseCmd( this, fAlnView, aSeqList);  break;
				case cCompSeq		: cmd= new DSeqComplementCmd( this, fAlnView,aSeqList); break;
				case cRevCompSeq: cmd= new DSeqRevComplCmd( this, fAlnView,aSeqList); break;
				case cDna2Rna		: cmd= new DSeqDna2RnaCmd( this, fAlnView,aSeqList); break;
				case cRna2Dna		:	cmd= new DSeqRna2DnaCmd( this, fAlnView,aSeqList); break;
				case cToUpper		:	cmd= new DSeqUppercaseCmd( this, fAlnView,aSeqList); break;
				case cToLower		:	cmd= new DSeqLowercaseCmd( this,fAlnView, aSeqList); break;
				case cDegap			:	cmd= new DSeqCompressCmd( this, fAlnView,aSeqList); break;
				case cLockIndels:	cmd= new DSeqLockIndelsCmd( this, fAlnView,aSeqList); break;
				case cUnlockIndels: cmd= new DSeqUnlockIndelsCmd( this, fAlnView,aSeqList); break;
				case cTranslate	:	cmd= new DSeqTranslateCmd( this, fAlnView,aSeqList); break;

				case cMaskCompress:	cmd= new DSeqCompressMaskCmd( this, fAlnView,aSeqList); break;

				default:
					delete aSeqList;
					Message(MSG_OK,"DSeqDoc::method not ready.");
					return true;
				
		  	}
		  if (cmd) {
		  	if (cmd->Initialize()) PostTask( cmd);
		  	else {
		  		// use cmd.fTitle==GetTitle for ::method name !?
		  		Message(MSG_OK,"DSeqDoc::method failed.");
		  		delete cmd; 
					}
		  	}
			return true;	
			}

		case cPrettyPrint:  MakeSeqPrint( false); return true; //MakeAlnPrint();
		case cREMap:				MakeSeqPrint( true); return true;
		case cNAcodes:			NucCodesPicture(); return true;
		case cAAcodes:			AminoCodesPicture(); return true;
		case cDotPlot:			//MakeDottyPlot(); return true;
			Message(MSG_OK,"DSeqDoc::method not ready.");
			return true;

#if NETBLAST
		case cNCBIblast:
			{
			char* blastProgram = "blastn";
			char* blastDatabase = "nr";
			char* blastOptions = NULL;
			char* fastaInputFile = "spup-blast-input.fasta";
			char* outputFile = "spup-blast-result.text";
			char	instore[128];
			char  outstore[128];
			DFile aFile;
			
			fastaInputFile= DFileManager::TempFilename( instore);
			outputFile= DFileManager::TempFilename( outstore);

			short nseq= SelectionToFile( false, fastaInputFile, DSeqFile::kPearson);
			if (nseq) {
				short err= CallNCBIBlastService(blastProgram, blastDatabase, blastOptions, 
														fastaInputFile, outputFile);

				if (err) {
					Message(MSG_OK,"DSeqDoc::CallNCBIBlastService error %d", err);
					}
				else {
					DRichTextDoc* doc= new DRichTextDoc(0,  true, gTextFont);
					doc->Open(outputFile);
					aFile.Initialize(outputFile);
					aFile.Delete();
					}
			 	}
			aFile.Initialize(fastaInputFile);
			aFile.Delete();
			}
			return true;
			
#else		
		case cNCBIblast:
#endif
	
		case cGrailSearch:
		case cGeneidSearch:
	  case cFHCRCblocks:
	  case cEMBLfasta:
	  case cEMBLblitz:
	  case cEMBLquicks:
			{
			DSequence* aSeq= fAlnView->SelectedSequence();
			if (aSeq) {
				switch (tasknum) {
					case cNCBIblast: win= new DNCBIBlast(0, this, aSeq); break;
			 		case cEMBLblitz: win= new DEMBLBlitz(0, this, aSeq); break;
			 		case cEMBLfasta: win= new DEMBLFasta(0, this, aSeq); break;
			 		case cEMBLquicks: win= new DEMBLQuicks(0, this, aSeq); break;
			 		case cFHCRCblocks: win= new DFHCRCblocks(0, this, aSeq); break;
			 		case cGeneidSearch: win= new DUWFGeneID(0, this, aSeq); break;
		 			case cGrailSearch: win= new DORNLGrail(0, this, aSeq); break;
					}
				if (win) win->Open();
				}
			return true;
			}

	  case cFHCRCfetch:
		case cEMBLfetch:
		case cNCBIfetch:
			switch (tasknum) {
				case cNCBIfetch: win= new DNCBIFetch(0, this); break;
		 		case cEMBLfetch: win= new DEMBLFetch(0, this); break;
		 		case cFHCRCfetch: win= new DFHCRCfetch(0, this); break;
				}
			if (win) win->Open();
			return true;
						
		case cGenmarkSearch:
		case cPythiaSearch:
			Message(MSG_OK,"DSeqDoc::method not ready.");
			return true;


		default: 
			return DTaskMaster::DoMenuTask(tasknum, theTask);
		}

}


Boolean DSeqDoc::IsMyAction(DTaskMaster* action) 
{
	enum appMenus { kInternetMenu = 100, kPrefsMenu, kOpenMenu };  // HACK ! from SeqPup.cpp
	long  menuid= 0, menuitem = action->Id();
	if (action->fSuperior) menuid = action->fSuperior->Id();
	
	if ( menuid == DApplication::cEditMenu
		&& HasEditText() && fEditText == fAlnView->fEditSeq ) {
		if (fEditText->IsMyAction( action)) return true;
		}
		
	//if (menuid == DApplication::cEditMenu)  
	
	if ((menuid == kInternetMenu
	    ||menuid == DSeqApps::kChildMenu)
	 && (menuitem >= DSeqApps::kChildMenuBaseID 
	     && menuitem <= DSeqApps::kChildMenuBaseID + 50) // +50 = HACK !! need better way to distinguish menu items !
	) goto caseChildMenu;
		
	switch (menuitem) {
	
		case DApplication::kCut:
		case DApplication::kCopy:
		case DApplication::kClear:
		  {
			DAlnEditCommand* cmd= new DAlnEditCommand(this, menuitem);
			PostTask( cmd);
			return true;			
			}
		
		case DApplication::kPaste:
			{
		  DView* clipview= (DView*) gClipboardMgr->fClipView; // GetClipView();
			if (clipview && clipview->fKind == DAlnView::kindAlnView) {
				DAlnPasteCommand* cmd= new DAlnPasteCommand(this);
				PostTask( cmd);
				return true;			
				}
			else if (HasEditText()) {  /* && clipview->fKind == OStype("TEXT") */
				if (fEditText->IsMyAction(action)) return true;
				}
			return true; //?? always handle this message?
			}
			
		case DApplication::kSelectAll:
			if (fAlnIndex) {
			Nlm_RecT r;
			Nlm_LoadRect(& r, 0, 0, 1/*fAlnIndex->GetMaxCols()*/, fAlnIndex->GetMaxRows());
			fAlnIndex->SelectCells( r);
			return true;			
			}

		}

	if (menuid == kViewKindMenu)  
	 switch (menuitem) {
			case kViewByDefault:
					this->SortView(DSeqList::kSortByItem); return true;
			case kViewByDate:
					this->SortView(DSeqList::kSortByDate); return true;
			case kViewBySize:
					this->SortView(DSeqList::kSortBySize); return true;
			case kViewByName:
					this->SortView(DSeqList::kSortByName); return true;
			case kViewByKind:
					this->SortView(DSeqList::kSortByKind); return true;
			case kViewAsText:
					this->ToTextDoc(); return true;
			default: return true;
			}

	else if (menuid == kSeqMaskMenu && fAlnView->fMaskLevel>0) {
		if (menuitem == cMaskCompress) 
			return DoMenuTask(menuitem, NULL);
		else
			return fAlnView->MaskCommand( menuitem);
		}

	else if (menuid == DSeqApps::kChildMenu) {
	caseChildMenu:
		switch (menuitem) {
#if 0
			case cAddChildCmd:
			case cRemoveChildCmd:  
				DoMenuTask(menuitem);
				break;
#endif	
			default:
				{
				long	firstBase = 0, nBases = 0;
				DSeqList * aSeqList= NULL;
				//fAlnView->DeInstallEditSeq(); //?? reinstall after ?
				GetSelection( kSeqSel+kMaskSel+kIndexSel+kAllIfNone, //+kEqualCount ??
								aSeqList, firstBase, nBases);
				DSeqApps::CallChildApp( menuitem, aSeqList);
				if (aSeqList) delete aSeqList;
				}
				break;
			}
		}
		
	else if (menuitem == kColorPopup) {
		fgUseColor= fColorPop->GetValue();
		fAlnView->SetViewColor( fgUseColor);
		//fAlnView->Invalidate();
  	return true;
		}
 
	else if (menuitem == kLockButHit) {
		fAlnView->SetTextLock( ((DView*)action)->GetStatus());
  	return true;
  	}

	else if (menuitem == kModePopup) {
		short mode= fModePop->GetValue();  
		fAlnView->SetViewMode( mode);
  	return true;
  	}
	
	else 
		return DoMenuTask(menuitem, NULL);
		
	return false;
}
			



