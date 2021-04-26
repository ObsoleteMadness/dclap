// DSeqViews.cpp

#define MASKS 1

#include "DSeqViews.h"

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
#include "DSeqFile.h"
#include "DSeqEd.h"
#include "DSeqMail.h"
#include "DSeqCmds.h"
#include "DSeqChildApp.h"
#include "DSeqPrint.h"
#include "DSeqPict.h"
#include "DSeqDoc.h"





Local short gAlnCharWidth = 12;
Local short gAlnCharHeight = 15;
//Local short gMinCommonPercent = 0;
Local Boolean gSwapBackground = false;
typedef unsigned long  colorVal;


#define ETEXT 1


extern "C" void Nlm_HScrollText (Nlm_BaR sb, Nlm_GraphiC t,
                               Nlm_Int2 newval, Nlm_Int2 oldval);
extern "C" void Nlm_DoActivate(void* g, Nlm_Boolean saveport);
//extern Nlm_Boolean   gDialogTextMultiline;



// class DAlnSequence

DAlnSequence::DAlnSequence(long id, DSeqDoc* itsSuperior):
		DTextLine(id, itsSuperior, NULL, 1, gSeqFont),  
		fSeq(NULL),fVisible(false)
{
	fDoc= itsSuperior;
	SetMultilineText(false);
}

void DAlnSequence::Scroll(Boolean vertical, DView* scrollee, short newval, short oldval) 
{
	if (fVisible && !vertical) { 
		Nlm_HScrollText( NULL, (Nlm_GraphiC)GetNlmObject(), newval, oldval);
		}
}

void DAlnSequence::ShowEdit()
{
	if (fSeq) SetText( fSeq->Bases());
	if (!fVisible) {
		fVisible= true;
		Show(); 
		Enable();
		}
		// may need this for MSWIN
	//Nlm_DoActivate(GetNlmObject(),false); 
}

void DAlnSequence::HideEdit()
{
	if (fVisible) {
		fVisible= false;
		Disable();
		Hide();
		}
}

void DAlnSequence::selectAction()
{
	DTextLine::selectAction();
	//gCursor->ibeam();
	SetKeyHandler(this); //?? also need this call in a window activate/deactivate method??
}

void DAlnSequence::deselectAction()
{
	SetKeyHandler(NULL); //?? also need this call in a window activate/deactivate method??
	//fDoc->SetEditText(NULL);
	//gCursor->arrow();
	DTextLine::deselectAction();
}

void DAlnSequence::ProcessKey( char c)
{
	enum moves {
		down1 =  1,
		up1		= -1,
		downPage= 10,  // this is arbitrary...
		upPage  = -10,
		toBottom=  25000,
		toTop		= -25000
		};
		
	if ( IsEnabled() && !gKeys->command()) { 
		// dang vibrant doens't have means to change this key, and default handlers still get
		// a crack at it...

		switch (c) {
		
#if 0
			case chTab: 
		  	//? do horizontal shift here ?
				break;

			case chRight:
				if (gKeys->shift()) ;
				else ;
			case chLeft:
				if (gKeys->shift()) ;
				else ;
				
			case chUp:
				if (gKeys->shift()) vertMove(toTop);
				else vertMove(up1);
				break;
				
			case chDown:
			case chEnter:
			case chReturn:
				if (gKeys->shift())  vertMove(toBottom);
				else vertMove(down1);
				break;
				
			case chPageDown	:	vertMove( downPage); break;
			case chPageUp		:	vertMove( upPage); break;
			case chHome			: vertMove( toTop); break;
			case chEnd	 		: vertMove( toBottom); break;	
#endif
		
			default: 
				if (!fSeq->GoodChar(c)) Nlm_Beep(); 
				break; 
			}
		}
}



class DMaskSelector : public DTabSelector 
{
public:
	enum { cMaskSelector = 20732 };
	short fMaskLevel;
	DSeqList* fSeqList;
	
	DMaskSelector( DTableView* itsTable, short masklevel, DSeqList* seqlist) : 
		DTabSelector( itsTable),
		fSeqList( seqlist),  fMaskLevel(masklevel) 
		{
		fNumber= cMaskSelector;
		}
	virtual void DoItWork(); 
};


void DMaskSelector::DoItWork()  
{
	if (fMovedOnce || fDoExtend) {
#if MASKS
			// select masks... instead of, or as well as, table sel rect??
		if (fMaskLevel>0) {
			short row, col, top, bot, left, right;
			Nlm_RecT viewr;
			top= fNewSelection.top;
			bot= fNewSelection.bottom;
			left= fNewSelection.left;
			right= fNewSelection.right;
			for (row= top; row<bot; row++) {
				DSequence* sq= fSeqList->SeqAt(row);
				if (sq) {
					for (col= left; col<right; col++) 
					  if (fDoExtend) sq->SetMaskAt(col,fMaskLevel);
						else sq->FlipMaskAt(col, fMaskLevel);
					}				
				}
			((DTableView*)fView)->GetCellRect(fNewSelection,viewr);
			((DTableView*)fView)->InvalRect( viewr);
			}
		else
#endif
		((DTableView*)fView)->SelectCells( fNewSelection, 
				fDoExtend, DTableView::kHighlight, DTableView::kSelect);
		}
}




//class DAlnView : public DTableView  

	// test var for fEditSeq bomb...
DAlnView* gLastEditView = NULL;


DAlnView::DAlnView( long id, DView* itsSuper, DSeqDoc* itsDocument, DSeqList* itsSeqList, 
								long pixwidth, long pixheight) :
	DTableView( id, itsSuper, pixwidth, pixheight, 0, 0, Nlm_stdCharWidth, 2+Nlm_stdFontHeight,
						true, true),
	fDoc(itsDocument), fSeqList(itsSeqList), fSlider(NULL),
	fEditRow(-1), fMaskLevel(0), fCurSeq(NULL), fEditSeq(NULL),
	fLocked(false), fOwnSeqlist(false), 
	fColorBases(kBaseBlack), fColcolors(NULL), fNcolcolors(0)
{ 
	fKind= kindAlnView;
	gAlnCharWidth= Nlm_stdCharWidth; //Nlm_CharWidth('G');   
	gAlnCharHeight= Nlm_stdLineHeight;
	this->SetResize( DView::relsuper, DView::relsuper);
	this->SetTableFont(gSeqFont);
	//fSlider= new DAlnSlider( fDoc, this, 0);
}


DAlnIndex::DAlnIndex( long id, DView* itsSuper, DSeqDoc* itsDocument, DSeqList* itsSeqList,
				 long pixwidth, long pixheight) :
	DTableView( id, itsSuper, pixwidth, pixheight, 0, 4,
			5*Nlm_stdCharWidth, 2+Nlm_stdFontHeight, false, true),
	fNameWidth(15),
	fSizeWidth(8),
	fKindWidth(8),
	fDoc(itsDocument),
	fSeqList(itsSeqList)
{ 
	//this->SetSlateBorder(false);
	this->SetResize( DView::fixed, DView::relsuper);
	this->SetTableFont(gTextFont);
}

DAlnITitle::DAlnITitle( long id, DView* itsSuper, long pixwidth, long pixheight) :
	DTableView( id, itsSuper, pixwidth, pixheight, 1, 4,
			5*Nlm_stdCharWidth, 2+Nlm_stdFontHeight, false, false),
	fNameWidth(15),
	fSizeWidth(8),
	fKindWidth(8)
{ 
	//this->SetSlateBorder(false);
	this->SetResize( DView::fixed, DView::fixed);
	this->SetTableFont(gTextFont);
}

DAlnHIndex::DAlnHIndex( long id, DView* itsSuper, DSeqDoc* itsDocument, DSeqList* itsSeqList, long pixwidth, long pixheight) :
	DPanel( id, itsSuper, pixwidth, pixheight, DPanel::simple),
	fDoc(itsDocument), fLastCol(-1),
	fSeqList(itsSeqList)
{ 
	//this->SetResize( DView::matchsuper, DView::fixed);
	this->SetResize( DView::relsuper, DView::fixed);
}

DAlnView::~DAlnView()
{
	//fSeqList= NULL; // fSeqList is owned by Doc !
	DeInstallEditSeq();
	//if (fSlider) delete fSlider;
#if FIX_LATER
	itsEditSeq= fEditSeq;	 
	if (itsEditSeq && itsEditSeq->fSuperior)
		itsEditSeq->fSuperior->RemoveSubView(itsEditSeq);
	delete itsEditSeq;
#endif
}

				

void DAlnHIndex::Resize(DView* superview, Nlm_PoinT sizechange)
{
	DPanel::Resize(superview, sizechange);
}

void DAlnView::Resize(DView* superview, Nlm_PoinT sizechange)
{
	DTableView::Resize(superview, sizechange);
}

void DAlnView::GetReadyToShow()
{
	// assume port is set??
	SelectFont();  
	gAlnCharWidth = Nlm_CharWidth('G');   
	gAlnCharHeight= Nlm_LineHeight() + 2; // Nlm_FontHeight()
	SetItemWidth(0, GetMaxCols(), gAlnCharWidth);
	SetItemHeight(0, GetMaxRows(), gAlnCharHeight);
	this->UpdateAllWidths(); 
	this->UpdateSize();  
}

void DAlnIndex::GetReadyToShow()
{
	//SelectFont(); //Nlm_SelectFont(fFont);  
	SetItemWidth(0, GetMaxCols(), fViewrect.right-fViewrect.left-1); //<< width of view
	SetItemHeight(0, GetMaxRows(), gAlnCharHeight);
}

void DAlnITitle::GetReadyToShow()
{
	// assume port is set??
	SelectFont();
	short charheight=  Nlm_LineHeight(); // Nlm_FontHeight();
	SetItemWidth(0, GetMaxCols(), fViewrect.right-fViewrect.left-1); //<< width of view
	SetItemHeight(0, GetMaxRows(), charheight);
}

void DAlnView::Show()
{
	GetReadyToShow();
	DTableView::Show();
}

void DAlnIndex::Show()
{
	GetReadyToShow();
	DTableView::Show();
}

void DAlnView::Drag(Nlm_PoinT mouse)
{
#if 0
	short 	row, col;
	PointToCell( mouse, row, col);
	SeqMeter(row,col);
#endif
	DTableView::Drag(mouse); // sets fMouseStillDown= true
}

void DAlnIndex::Drag(Nlm_PoinT mouse)
{
	DTableView::Drag(mouse); // sets fMouseStillDown= true
}

void DAlnHIndex::Drag(Nlm_PoinT mouse)
{
	DPanel::Drag(mouse);
	ClickColumn(mouse);
}

void DAlnView::Hold(Nlm_PoinT mouse)
{
	DTableView::Hold(mouse);
}

void DAlnIndex::Hold(Nlm_PoinT mouse)
{
	DTableView::Hold(mouse);
}

void DAlnHIndex::Hold(Nlm_PoinT mouse)
{
	DPanel::Hold(mouse);
}

void DAlnView::Release(Nlm_PoinT mouse)
{
	DTableView::Release(mouse);
}

void DAlnIndex::Release(Nlm_PoinT mouse)
{
	DTableView::Release(mouse);
}

void DAlnHIndex::Release(Nlm_PoinT mouse)
{
	DPanel::Release(mouse);
}


void DAlnView::DoubleClickAt(short row, short col)
{
#if 0
	//?? do this only on DAlnIndex dblclik??
	DSequence* ag= fSeqList->SeqAt(row); 
	if (ag) fDoc->OpenSeqedWindow(ag);
#endif
}


void DAlnIndex::Click(Nlm_PoinT mouse)
{
	short  	row, col;
	//if (gLastCommand) { delete gLastCommand; gLastCommand= NULL; } //gLastCommand->Commit();	//??
	
	fDoc->fAlnView->DeInstallEditSeq();
	fDoc->fAlnView->SetEmptySelection(true); 

	PointToCell( mouse, row, col);
	if (Nlm_dblClick)
		;
	else if (IsSelected(row, col)) {   //if (gKeys->shift()) 
		// slide/shift selected lines...
		DAlnShifter* shifter= new DAlnShifter();  
		shifter->IAlnSlider( fDoc, this, fDoc->fAlnView, row);
		fCurrentTracker= shifter;
		}
	else {
		//if (fTabSelector) fTabSelector->suicide();  // is this delete causing bombs? YES, in motif
		fTabSelector= new DTabSelector( this);
		fCurrentTracker= fTabSelector;
	 }
	DTableView::Click( mouse);
}

 
void DAlnITitle::Click(Nlm_PoinT mouse)
{
		// damn, need this or the like for each user of DTableView::Click ...
	//if (gLastCommand) { delete gLastCommand; gLastCommand= NULL; } //gLastCommand->Commit();	//??

	//if (fTabSelector) fTabSelector->suicide();  // is this delete causing bombs? YES, in motif
	fTabSelector= new DTabSelector( this);
	fCurrentTracker= fTabSelector;
 
	DTableView::Click( mouse);
}




void DAlnView::Click(Nlm_PoinT mouse)
{
	short 	row, col;

	//if (gLastCommand) { delete gLastCommand; gLastCommand= NULL; } //gLastCommand->Commit();	//??

	Nlm_CaptureSlateFocus((Nlm_SlatE) fPanel); //! need for CharHandler !
 
	PointToCell( mouse, row, col);
	
	SeqMeter(row,col);

	if (IsSelected(row, col)) {
		//if (fSlider) delete fSlider;  // is this delete causing bombs?
		fSlider= new DAlnSlider();
		fSlider->IAlnSlider( fDoc, this, this, 0);
		fCurrentTracker= fSlider;
		}
	else {
		//if (fTabSelector) fTabSelector->suicide();  // is this delete causing bombs? YES, in motif
#if MASKS
		if (fMaskLevel>0) 
			fTabSelector= new DMaskSelector( this, fMaskLevel, fSeqList);
		else
#endif
			fTabSelector= new DTabSelector( this);
		fCurrentTracker= fTabSelector;
		}
 
	DTableView::Click( mouse);
}
 
 
void DAlnView::TrackMouse( short aTrackPhase,
					Nlm_PoinT& anchorPoint, Nlm_PoinT& previousPoint,
					Nlm_PoinT& nextPoint,	Nlm_Boolean mouseDidMove)
{
	DTableView::TrackMouse( aTrackPhase, anchorPoint, previousPoint, nextPoint,mouseDidMove); 
}
 

void DAlnView::TrackFeedback( short aTrackPhase,
					const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
					const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn)
{
#if 1
	short 	row, col;
	if (mouseDidMove && aTrackPhase == DTracker::trackContinue ) { // 2 == trackContinue
		PointToCell( nextPoint, row, col);
		SeqMeter(row,col);
		}
#endif
	DTableView::TrackFeedback( aTrackPhase, anchorPoint, previousPoint, nextPoint,
					mouseDidMove, turnItOn);
}


void DAlnIndex::DoubleClickAt(short row, short col)
{
	DSequence* ag= fSeqList->SeqAt(row); 
	if (ag) fDoc->OpenSeqedWindow(ag);
}

void DAlnITitle::DoubleClickAt(short row, short col)
{
}



void DAlnView::SeqMeter(short row, short col)
{
	// display base# location of mouse
	if (fDoc->fSeqMeter) {
		char basen[80];
		sprintf(basen,"%d",col+1);
		fDoc->fSeqMeter->SetTitle( basen);
		}
}

void DAlnView::SingleClickAt(short row, short col)
{
#if ETEXT
	if (!(fLocked || gKeys->shift() || gKeys->command() || gKeys->option() )) {  
		SetEmptySelection( true); 
		if (fDoc->fAlnIndex) fDoc->fAlnIndex->SetEmptySelection( true); 
		InstallEditSeq(row,col,col,false);
		}
	else
#endif
#if MASKS
	if (fMaskLevel>0) {
		Nlm_RecT r;
		DSequence* ag= fSeqList->SeqAt(row); 
		if (ag) {
			if (gKeys->shift()) ag->SetMaskAt(col,fMaskLevel);
			else ag->FlipMaskAt(col,fMaskLevel);
			}
		GetCellRect( row, col, r);
		InvalRect( r);
		}
	else
#endif
 		DTableView::SingleClickAt(row,  col);
	 //fDoc->SetViewMenu();
}

void DAlnIndex::SingleClickAt(short row, short col)
{
	 DTableView::SingleClickAt(row,  col);
	 //fDoc->SetViewMenu();
}

void DAlnITitle::SingleClickAt(short row, short col)
{
}

void DAlnHIndex::Click(Nlm_PoinT mouse)
{
	DPanel::Click(mouse);
	fLastCol= -1;
	ClickColumn(mouse);
}
	
void DAlnHIndex::ClickColumn(Nlm_PoinT mouse)
{
#if MASKS
	short row, col;
	// if (fMouseStillDown && ...)
	short masklevel= fDoc->fAlnView->fMaskLevel;
	fDoc->fAlnView->PointToCell( mouse, row, col);
	
	if (col != fLastCol && col >= 0 && col < fDoc->fAlnView->GetMaxCols()) {
		Nlm_RecT r;
		short irow, nrow= fSeqList->GetSize();
		if (masklevel == 0) {
			r.left= col;
			r.right= col+1;
			r.top= 0;
			r.bottom= fDoc->fAlnView->GetMaxRows(); 
			fDoc->fAlnView->InvalidateSelection();
			fDoc->fAlnView->SelectCells( r, gKeys->shift(), 
							!DTableView::kHighlight, DTableView::kSelect);
			fDoc->fAlnView->InvalidateSelection();
			}
		else for (irow=0; irow<nrow; irow++) {
			DSequence* ag= fSeqList->SeqAt(irow); 
			if (ag) {
				if (gKeys->shift()) ag->SetMaskAt(col,masklevel);
				else ag->FlipMaskAt(col,masklevel);
				}
			fDoc->fAlnView->GetCellRect( irow, col, r);
			InvalRect( r);
			}
		fLastCol= col;
		}
		
#endif
}
	


void DAlnView::Scroll(Boolean vertical, DView* scrollee, short newval, short oldval)
{
	short diff= newval - oldval;
	DTableView::Scroll(vertical, scrollee, newval, oldval);
	
	if (diff && fEditSeq && fEditSeq->fVisible) {
		Nlm_RecT	r;
		short delta;
		if (vertical) {
			delta= diff * fItemHeight; // bad for variable line height
			fEditSeq->GetPosition( r);
			Nlm_OffsetRect( &r, 0, -delta);
			if (r.top < fRect.top || r.top >= fRect.bottom) DeInstallEditSeq();
			else fEditSeq->SetPosition( r);
			}
		else {
				// messy, undo bitmap scroll by TableView for just editseq rect...
			delta= diff * fItemWidth;
			fEditSeq->GetPosition(r);
			Nlm_ScrollRect(&r, delta, 0);
					// then redo scroll using edit method !
			fEditSeq->Scroll(vertical, scrollee, newval, oldval);
			}
		}
		
	if (scrollee == this) {
		if (vertical) {
			fDoc->fAlnIndex->Scroll(vertical, scrollee, newval, oldval);
			}
		else {
			fDoc->fAlnHIndex->Scroll(vertical, scrollee, newval, oldval);
			}
		}
}

void DAlnIndex::Scroll(Boolean vertical, DView* scrollee, short newval, short oldval)
{
	DTableView::Scroll(vertical, scrollee, newval, oldval);
	if (scrollee == this) {
		if (vertical) {
			fDoc->fAlnView->Scroll(vertical, scrollee, newval, oldval);
			}
		else   // horizontal
			fDoc->fAlnITitle->Scroll(vertical, scrollee, newval, oldval);
			 
		}
}


void DAlnHIndex::Scroll(Boolean vertical, DView* scrollee, short newval, short oldval)
{
	Nlm_RecT	r;
	short delta;
	short diff= newval-oldval;
	ViewRect(r);
	if (vertical) {
#if 0
		delta= diff * cHeight;
		Nlm_ScrollRect (&r, 0, -delta);
		if (diff<0) r.bottom= r.top - delta + Min(5,-delta/2);
		else r.top= r.bottom - delta - Min(5,delta/2);
#endif
		}
	else {
#if 1
		short cWidth= gAlnCharWidth;
		delta= diff * cWidth;
		Nlm_ScrollRect (&r, -delta, 0);
			// test fix for off-by-a-few-pixels in scroll...
		if (diff<0) r.right= r.left - delta + Min(5,-delta/2);
		else r.left= r.right - delta - Min(5,delta/2);
#endif
		}
	this->Select(); // need for motif !
	this->InvalRect( r);
 
	if (scrollee == this) {
		if (!vertical) fDoc->fAlnView->Scroll(vertical, scrollee, newval, oldval);
		}
}



DSequence* DAlnView::SelectedSequence(short& selectedRow)
{
	if (GetSelectedRow() != kNoSelection) {
		selectedRow= GetSelectedRow();
		return fSeqList->SeqAt(selectedRow);
		}
	else if (fDoc && fDoc->fAlnIndex->GetSelectedRow() != kNoSelection) {
		selectedRow= fDoc->fAlnIndex->GetSelectedRow();
		return fSeqList->SeqAt(selectedRow);	
		}
	else {
		selectedRow= kNoSelection;
		return NULL;
		}
}


void DAlnView::UpdateSize()
{	
	long diff;   
	if (fSeqList) diff= fSeqList->GetSize() - GetMaxRows();
	else diff= -GetMaxRows();
	ChangeRowSize( -1, diff);  // -1 prevents redraw..else use fMaxRows	
	if (fDoc && fDoc->fAlnIndex) fDoc->fAlnIndex->ChangeRowSize(-1, diff);

#if MASKS
	if (diff>0) { //fMaskLevel>0 && 
		short i, nseq= fSeqList->GetSize();
		for (i=nseq-diff; i<nseq; i++) {
			DSequence* aseq= fSeqList->SeqAt(i);
			if (fMaskLevel>0 || aseq->Masks()) aseq->FixMasks();
			}
		}
#endif
}


void DAlnView::UpdateWidth(DSequence* aSeq)
{ 
	long alnlen= aSeq->LengthF() + 30;
	if (alnlen > GetMaxCols()) ChangeColSize( -1, alnlen-GetMaxCols());  
	SetItemWidth( 0, GetMaxCols(), gAlnCharWidth);  
#if MASKS
	if (fMaskLevel>0 || aSeq->Masks()) aSeq->FixMasks();
#endif
	//if (fDoc && fDoc->fAlnHIndex) fDoc->fAlnHIndex->UpdateWidth(); //keep ruler in sync
}

void DAlnView::UpdateAllWidths()
{
	long alnlen= 0;
	short i, nseq= fSeqList->GetSize();
	for (i=0; i<nseq; i++) {
		DSequence* aSeq= fSeqList->SeqAt(i);
		alnlen= Max(alnlen, aSeq->LengthF()); 
#if MASKS
		if (fMaskLevel>0 || aSeq->Masks()) aSeq->FixMasks();
#endif
		}
	alnlen += 30; 
	if (alnlen > GetMaxCols()) ChangeColSize( -1, alnlen-GetMaxCols());
	SetItemWidth( 0, GetMaxCols(), gAlnCharWidth);  
	//if (fDoc && fDoc->fAlnHIndex) fDoc->fAlnHIndex->UpdateWidth(); //keep ruler in sync
}

void DAlnView::SetTextLock( Boolean turnon)
{
	fLocked= turnon;
	if (fLocked) DeInstallEditSeq();
}


void DAlnView::SetViewMode(short viewmode) 
{
	switch (viewmode) {
		case kModeSlide: 
			this->SetTextLock( true);
			if (this->fMaskLevel>0) this->Invalidate();
			this->fMaskLevel= 0; 
			break;
			
		case kModeEdit:  
			this->SetTextLock( false);
			if (this->fMaskLevel>0) this->Invalidate();
			this->fMaskLevel= 0; 
			break;
			
		case kModeMask1: 
		case kModeMask2: 
		case kModeMask3: 
		case kModeMask4: {
			this->SetTextLock( true);
			this->fMaskLevel= viewmode - kModeMask1 + 1; 
			this->Invalidate();
			short i, nseq= fSeqList->GetSize();
			for (i=0; i<nseq; i++) fSeqList->SeqAt(i)->FixMasks();
			}
			break;
			
		default :
			break;
		}
}

void DAlnView::SetViewColor(short colorkind) 
{
	switch (colorkind) {
		case kBaseBlack: 
		case kBaseColor:  
		case kBaseVarLite: 
		default :
			break;
		}
	//if (fColorBases != colorkind) 
	Invalidate();
	fColorBases= colorkind;
}


void DAlnView::CharHandler(char c) 
{
	switch (c) {
	
		case 0x08:	// backspace
		case 0x7f: 	// Delete (but mapped to backspace here...)
			// delete selected softindels... == degap
			(void) fDoc->DoMenuTask( DSeqDoc::cDegap, NULL);
			break;
			
		case 0x0D: // return // 0x03 == enter
		case 0x09: // tab
	  case 0x1C: // <- arrow
		case 0x1D: // -> arrow
		case 0x1E: // ^ arrow
		case 0x1F: // v arrow
		default: 
			break; 
		}
}




// move this to DSeqCmds.h ...
class DSetEditCmd : public DSeqChangeCmd {
public: 
	DSetEditCmd( DSeqDoc* itsAlnDoc, DView* itsView, DSequence* oldSeq, char* newbases):
		 DSeqChangeCmd("edit seq", itsAlnDoc, itsView, NULL) 
		 {
			fOldSeqs= new DSeqList(); 
		 	fOldSeqs->InsertLast( oldSeq);
			DSequence* newSeq= MakeSequence( oldSeq->Name(), newbases, oldSeq->Info(), 0);
			fNewSeqs->InsertLast( newSeq);
		 }
};


void DAlnView::DeInstallEditSeq()
{
	if (fEditRow >= 0) { 
		if (fEditSeq) {
			Nlm_RecT r;
			char* newbases = fEditSeq->GetText();
			if (StringCmp(newbases, fEditSeq->fSeq->Bases()) != 0) {
#if 0
				// test if DSetEditCmd is cause of bombs when fEditSeq is used...
				// no -- this isn't it...
				fEditSeq->fSeq->SetBases(newbases);					
#else
				DSetEditCmd* cmd= new DSetEditCmd( fDoc, this, fEditSeq->fSeq, newbases);
				if (cmd) PostTask(cmd); 
#endif
				}
			MemFree( newbases);
		  
			fEditSeq->GetPosition(r);
			fEditSeq->HideEdit();
			fDoc->SetEditText(NULL); //??
			InvalRect(r);
			}
		fEditRow= -1;
		}

}



void DAlnView::InstallEditSeq(short row, short selStart, short selEnd, Boolean doLight)
{
		 
	DeInstallEditSeq();
	//if (gLastEditView && gLastEditView != this) gLastEditView->DeInstallEditSeq();

	fEditRow= row;
	if (row >= 0) {
		Nlm_RecT r;
		DSequence* aSeq= fSeqList->SeqAt( row);
		this->GetRowRect( row, r);
		fEditSeq->fSeq= aSeq;

		r.left--; // fix off-by-one
		fEditSeq->SetPosition( r);
		fEditSeq->ShowEdit();
		fEditSeq->SetSelection( selStart, selEnd);
		if (fLeft>0) fEditSeq->Scroll(false, NULL, fLeft, 0);
		fDoc->SetEditText(fEditSeq); //??

		if (gLastCommand) { delete gLastCommand; gLastCommand= NULL; } //gLastCommand->Commit();	//??
		//gLastEditView= this;
		}

}


void DAlnView::registerInsertLast( DSequence* aSeq)
{
	ChangeRowSize( GetMaxRows(), 1);
	if (fDoc && fDoc->fAlnIndex) 
		fDoc->fAlnIndex->ChangeRowSize( GetMaxRows(), 1);
	UpdateWidth(aSeq);	
}

void DAlnView::addToAlnList( DSequence* aSeq)
{
	fSeqList->InsertLast( aSeq);
	//aSeq->fIndex= fSeqList->GetSize(); //??
	registerInsertLast( aSeq); 

#if NOT_NOW_MASKS
		// done via registerInsertLast -> UpdateWidth
		if (fMaskLevel>0 || aSeq->fMasks) aSeq->FixMasks();
#endif
}


void DAlnView::MakeConsensus()
{ 
	fSeqList->MakeConsensus();
	short arow= fSeqList->ConsensusRow();
	if (arow>0) {
		Nlm_RecT r;
		GetRowRect( arow, r);
		this->InvalRect( r);
		}
}

 
char* DAlnView::FindCommonBases( short minCommonPerCent)
{
// THIS_IS_OBSOLETE
	char *hCommon, *hFirst = NULL;
	//gMinCommonPercent= minCommonPerCent;
	hCommon= fSeqList->FindCommonBases(DSeqList::gMinCommonPercent, hFirst);
	if (hFirst) MemFree( hFirst);
	return hCommon;
}


void DAlnView::HiliteORFs()
{
	long	start, stop;
	long nseq= fSeqList->GetSize();
	if (fMaskLevel>0) 
	for (long iseq= 0; iseq<nseq; iseq++) {
		DSequence* aSeq= fSeqList->SeqAt(iseq);
		long alen= aSeq->LengthF();
		aSeq->ClearMask(fMaskLevel);
		aSeq->ClearSelection();

		aSeq->SearchORF( start, stop);
		while (start>=0) {
			if (stop<start) stop= alen;
			for (long ibase=start; ibase<=stop; ibase++) 
				aSeq->SetMaskAt( ibase, fMaskLevel);
			if (stop >= alen) start= -1;
			else {
			  aSeq->SetSelection( stop+1, alen);
				aSeq->SearchORF( start, stop);
				}
			}
		} 
	this->Invalidate();
}


void DAlnView::HiliteCommonBases()
{  
#if 1
	char * hFirst = NULL;
	char * hCommon= fSeqList->FindCommonBases(DSeqList::gMinCommonPercent, hFirst);
	
	long nseq= fSeqList->GetSize();
	if (fMaskLevel>0) 
	for (long iseq= 0; iseq<nseq; iseq++) {
		DSequence* aSeq= fSeqList->SeqAt(iseq);
		long alen= aSeq->LengthF();
		char * hf = hFirst;
		char * hc = hCommon;
		char * bc = aSeq->Bases();
		aSeq->ClearMask(fMaskLevel);
		for (long ibase=0; ibase<alen; ibase++, hf++, hc++, bc++) 
			if (iseq >= (unsigned char) *hf && toupper(*bc) == *hc)
				aSeq->SetMaskAt( ibase, fMaskLevel);
		}
	this->Invalidate();
	MemFree(hCommon);
	MemFree(hFirst);
#endif

#if THIS_IS_OBSOLETE
	char		*hCon, *hSeq, *hFirst, *hMaxbase;
	long		maxlen;
	short 	arow;
	 
	DSequence* cons= fSeqList->Consensus();
	if (!cons) cons= (DSequence*) fSeqList->First();

	if (cons) {
		arow= fSeqList->GetIdentityItemNo( cons);
		hCon= cons->Bases();   
		hMaxbase= fSeqList->FindCommonBases(DSeqList::gMinCommonPercent, hFirst); 
		
		maxlen= StrLen(hMaxbase);
		this->SetEmptySelection( true);		
		long iseq, nseq= fSeqList->GetSize();
		for (iseq= 0; iseq<nseq; iseq++) {
			DSequence* aSeq= fSeqList->SeqAt(iseq);
			if (!aSeq->IsConsensus() && aSeq->Kind() != DSequence::kOtherSeq) {
				hSeq= aSeq->Bases();
				for (short ibase=0; ibase<maxlen; ibase++) {
					if ( toupper(hSeq[ibase]) == hMaxbase[ibase] )  
				  	this->SelectCells( iseq, ibase, false, false);  
					}
				}
			}
		this->InvalidateSelection();
		MemFree(hMaxbase); 
		MemFree(hFirst); 
		}
#endif
}
 
 
#define DRAWCLASS 1
#ifndef DRAWCLASS

void DAlnView::DrawAlnInStyle( baseColors colors, Boolean swapBackColor,
															char*	pText, long indx, long len, short row)
{
#if MASKS
		// ?? do we want this? main use of styles is in PrettyPrint...
	Nlm_PoinT pt;
	long			endx, pend, skip;
	char			ch, lastch;
	Nlm_RecT	crec; // set this to rect about draw char...
	short 		maskval;
		
	Boolean dostyles= (fCurSeq && fCurSeq->MasksOk());
	if (!dostyles) {
		DrawAlnColors( colors, swapBackColor, pText, indx, len,row);
		return;
		}
		
	long cw= GetItemWidth();
	Nlm_GetPen( &pt);
	if (pt.x < 0) {
		skip= (-pt.x) / cw;
  	indx += skip;
		len -= skip;
		pt.x += skip*cw;
		Nlm_MoveTo( pt.x, pt.y);
		}
	pend= len * cw;
	
	if (pend > GetRect().right) {   
		skip= (pend - GetRect().right) / cw;  
		len -= skip;
		}
	endx= indx + len - 1;
	
	lastch= 0;
	long cht= GetItemHeight();
	for (long i= indx; i<=endx; i++) {
		ch= pText[i];
		if (ch >= ' ') {
			if (ch!=lastch) Nlm_SetColor( colors[ch-' ']);
			Nlm_PaintChar(ch);
			lastch= ch;
			if (fMaskLevel>0) {
				maskval= fCurSeq->MaskAt(i, fMaskLevel);
				if (maskval>0) {
					Nlm_LoadRect( &crec, pt.x, pt.y-cht+2, pt.x+cw, pt.y+2); //??
					//Nlm_FrameRect( &crec); //?? doesn't show on XMotif?
					DTableView::InvertRect( crec); 
					}
				}
			pt.x += cw;
			// !? must reset some of drawing environ !? > Font !
			}
		}
	Nlm_Black();
	
#else
	DrawAlnColors( colors, swapBackColor, pText, indx, len, row);
#endif
}


void DAlnView::DrawAlnColors(baseColors colors, Boolean swapBackColor,
															char*	pText, long indx, long len, short row)
{
	Nlm_PoinT pt;
	long			endx, pend, skip;
	char			ch, lastch;
	
	long cw= GetItemWidth(); 
	Nlm_GetPen( &pt);
	if (pt.x < 0) {
		skip= (-pt.x) / cw;
  	indx += skip;
		len -= skip;
		Nlm_MoveTo( pt.x + skip*cw, pt.y);
		}
	pend= len * cw;
	
	if (pend > GetRect().right) {   
		skip= (pend - GetRect().right) / cw;  
		len -= skip;
		}
	endx= indx + len - 1;
	
	//-- backcolor not showing -- need erase or something...
	//if (swapBackcolor) RGBForeColor( colors[' '-' ']); else RGBBackColor( colors[' '-' ']);
	 
	//Nlm_CopyMode(); //?? assume?
	lastch= 0;
	for (long i= indx; i<=endx; i++) {
		ch= pText[i];
		if (ch >= ' ') {
			// if (swapBackcolor) RGBBackColor( colors[ch-' ']); else 
			if (ch!=lastch) Nlm_SetColor( colors[ch-' ']);
			Nlm_PaintChar(ch);
			lastch= ch;
			}
		}
	Nlm_Black();
} 

#else
///////////////////////////

void DAlnView::DrawAlnColors(baseColors colors, Boolean swapBackColor,
															char*	pText, long indx, long len, short row)
{
}

void DAlnView::DrawAlnInStyle( baseColors colors, Boolean swapBackColor,
															char*	pText, long indx, long len, short row)
{
}

class DrawBases
{
public:
	Nlm_PoinT pt;
	long		endx, pend, indx, len, cw;
	short		row;
	char		ch, lastch, *pText;
	DAlnView* tview;
	//baseColors&	colors;
	
	DrawBases() {}
	virtual void doDraw(DAlnView* theView, baseColors theColors, Boolean backcolor,
		       char*	theText, long theIndx, long theLen, short theRow);
	virtual void Calc();
	virtual void DrawSub(baseColors colors);
};

class DrawGreyBases : public DrawBases
{
public:	
	DrawGreyBases() {}
	virtual void DrawSub(baseColors colors);
};

class DrawBasesWithMask : public DrawBases
{
public:
	DrawBasesWithMask() {}
	virtual void doDraw(DAlnView* theView, baseColors theColors, Boolean backcolor,
		       char*	theText, long theIndx, long theLen, short theRow);
	virtual void DrawWithMasks(baseColors colors);
};


void DrawBases::doDraw(DAlnView* theView, baseColors theColors, Boolean backcolor,
	       char*	theText, long theIndx, long theLen, short theRow)
{
	tview= theView;
	//colors= theColors;
	pText= theText;
	indx= theIndx;
	len= theLen;
	row= theRow;
	lastch= 0;
	
	Calc();
	DrawSub(theColors);
}

void DrawBases::Calc() 
{
	long skip;
	cw= tview->GetItemWidth(); 
	Nlm_GetPen( &pt);
	if (pt.x < 0) {
		skip= (-pt.x) / cw;
  	indx+= skip;
		len -= skip;
		Nlm_MoveTo( pt.x + skip*cw, pt.y);
		}
	pend= len * cw;
	
	if (pend > tview->GetRect().right) {   
		skip= (pend - tview->GetRect().right) / cw;  
		len -= skip;
		}
	endx= indx + len - 1;
}

void DrawBases::DrawSub(baseColors colors)
{
	//-- backcolor not showing -- need erase or something...
	//if (swapBackcolor) RGBForeColor( colors[' '-' ']); else RGBBackColor( colors[' '-' ']);
 	//Nlm_CopyMode(); //?? assume?
 	
	for (long i= indx; i<=endx; i++) {
		ch= pText[i];
		if (ch >= ' ') {
			// if (swapBackcolor) RGBBackColor( colors[ch-' ']); else 
			if (ch!=lastch) Nlm_SetColor( colors[ch-' ']);
			Nlm_PaintChar(ch);
			lastch= ch;
			}
		}
	Nlm_Black();
}


void DrawGreyBases::DrawSub(baseColors colors)
{	 	
	for (long i= indx; i<=endx; i++) {
		ch= pText[i];
		if (ch >= ' ') {
				// could draw by columns & setcolor once/col for speed !
			Nlm_SetColor( tview->fColcolors[i-indx]);
			Nlm_PaintChar(ch);
			}
		}
	Nlm_Black();
}




void DrawBasesWithMask::doDraw(DAlnView* theView, baseColors theColors, Boolean backcolor,
	       char*	theText, long theIndx, long theLen, short theRow)
{
	tview= theView;
	//colors= &theColors;
	pText= theText;
	indx= theIndx;
	len= theLen;
	row= theRow;
	lastch= 0;
	Calc();
	Boolean dostyles= (tview->fCurSeq && tview->fCurSeq->MasksOk());
	if (!dostyles) DrawSub(theColors);
	else DrawWithMasks(theColors);
}

void DrawBasesWithMask::DrawWithMasks(baseColors colors)
{
	long cht= tview->GetItemHeight();
	Nlm_RecT	crec;  
	
	for (long i= indx; i<=endx; i++) {
		ch= pText[i];
		if (ch >= ' ') {
			if (ch!=lastch) Nlm_SetColor( colors[ch-' ']);
			Nlm_PaintChar(ch);
			lastch= ch;
			if (tview->fMaskLevel>0) {
				short maskval= tview->fCurSeq->MaskAt(i, tview->fMaskLevel);
				if (maskval>0) {
					Nlm_LoadRect( &crec, pt.x, pt.y-cht+2, pt.x+cw, pt.y+2); //??
					//Nlm_FrameRect( &crec); //?? doesn't show on XMotif?
					tview->DTableView::InvertRect( crec); 
					}
				}
			pt.x += cw;
			// !? must reset some of drawing environ !? > Font !
			}
		}
	Nlm_Black();
}

	
	
DrawBases         gDrawBases;
DrawBasesWithMask gDrawBasesWithMask;
DrawGreyBases			gDrawGreyBases;

#endif


void DAlnView::DrawAllColors(Nlm_RecT r, short row)
{ 
#ifdef DRAWCLASS
#define DRAWALN(a,b,c,d,e,f)	gDrawBasesWithMask.doDraw(this,a,b,c,d,e,f)
#else
#if MASKS
#define DRAWALN	DrawAlnInStyle
#else
#define DRAWALN	DrawAlnColors
#endif
#endif

	long stopcol, startcol = GetLeft(); 
	long newright= r.left;

	for (stopcol= startcol; stopcol<GetMaxCols() && newright<r.right; stopcol++) {
		//if (fWidths) newright += fWidths[startcol]; else 
		newright += GetItemWidth();
		// optimize later -- check update region by each char rect ...
		//if (Nlm_RectInRgn (&item_rect, Nlm_updateRgn)) done= true;
		}

	DSequence* aSeq= fSeqList->SeqAt(row);
	if (aSeq && aSeq->Bases()) {
		char* hSeq= aSeq->Bases();
		fCurSeq= aSeq;
		//Boolean dostyles= (fCurSeq && fCurSeq->MasksOk());
		
		long len= Min( aSeq->LengthF(), stopcol) - startcol;
		if (len>0 && hSeq) {
			Nlm_MoveTo( r.left, r.bottom-2); //bottom-5
			if (fColorBases == kBaseVarLite && fColcolors) 
				gDrawGreyBases.doDraw(this,DBaseColors::gNAcolors, gSwapBackground, hSeq, startcol, len, row);
			else if (aSeq->Kind() == DSequence::kAmino)
				DRAWALN(DBaseColors::gAAcolors, gSwapBackground, hSeq, startcol, len, row);
			else
				DRAWALN(DBaseColors::gNAcolors, gSwapBackground, hSeq, startcol, len, row);
			}
		}
	fColsDrawn= stopcol - startcol;
}  


void DAlnView::DrawNoColors(Nlm_RecT r, short row)
{ 
	long stopcol, startcol = GetLeft(); 
	long newright= r.left;
	short	cwidth= GetItemWidth(); // Nlm_CharWidth('G'); //fItemWidth
	
	for (stopcol= startcol; stopcol<GetMaxCols() && newright<r.right; stopcol++) {
		//if (fWidths) newright += fWidths[startcol]; else 
		newright += cwidth;
		// optimize later -- check update region by each char rect ...
		//if (Nlm_RectInRgn (&item_rect, Nlm_updateRgn)) done= true;
		}

	DSequence* aSeq= fSeqList->SeqAt(row);
	if (aSeq && aSeq->Bases()) {
		char *s, se, *hSeq= aSeq->Bases();
		long len= Min( aSeq->LengthF(), stopcol) - startcol;
		if (len>0 && hSeq) {
#if MASKS
			if (fMaskLevel>0) {
				short atx= r.left;
				short aty= r.bottom-2;
				stopcol= startcol + len;
				for (long i= startcol; i<stopcol; i++) {
					Nlm_MoveTo(atx, aty);
					Nlm_PaintChar(hSeq[i]);
					short maskval= aSeq->MaskAt(i, fMaskLevel);
					if (maskval>0) {
						Nlm_RecT crec;
						Nlm_LoadRect( &crec, atx, r.top, atx+cwidth, r.bottom); 
						//Nlm_FrameRect( &crec); //<< ? bad for Motif, also messy looking
						DTableView::InvertRect( crec); 
						}
					atx += cwidth;
					}
				}
			else 
#endif
				{
				Nlm_MoveTo( r.left, r.bottom-2); //bottom-5
				s= hSeq+startcol;
				se= s[len];
				s[len]= 0;
				Nlm_PaintString( s); 
				s[len]= se;
				//Nlm_DrawText( &r, hSeq+startcol, len, 'l', false); // gray== false
				}
    	}
		}
	fColsDrawn= stopcol - startcol;
}  

		
void DAlnView::Draw()
{

	if (fColorBases == kBaseVarLite) {
		// find grey color mask for base columns in view 
		Nlm_RecT r = fRect;
		short	row = 0; 
		long 	ncols, icol, stopcol, startcol = GetLeft(); 
		long 	newright= r.left;
		long	basecount['~'];
		char	maxc, ac;
		
		for (stopcol= startcol; stopcol < GetMaxCols() && newright<r.right; stopcol++) {
			newright += GetItemWidth();
			}
		ncols= stopcol - startcol + 1;
		
		if (ncols != fNcolcolors || !fColcolors) {
			MemFree( fColcolors);
			fColcolors= (unsigned long*) MemNew(ncols * sizeof(unsigned long));
			}
		fNcolcolors= ncols;
		
		for (icol= startcol; icol<stopcol; icol++) {
			long maxcount, nbases= 0;
			for (ac=0; ac<'~'; ac++) basecount[ac]= 0;
			for (row= 0; row < GetMaxRows(); row++) {
				DSequence* aSeq= fSeqList->SeqAt(row);
				if (aSeq && aSeq->Bases() && aSeq->LengthF()>icol) {
				  char ch= aSeq->Bases()[icol];
				  if (ch == DSequence::indelHard
				   || ch == DSequence::indelSoft
				   || ch == DSequence::indelEdge)
				   	;
					else { 
						ch |= 32;	// use case-insensitive ?
						// ?? or use DSequence::NucleicBits(ch) ??
						basecount[ch]++;
			    	nbases++;
				    }
					}
				}
			for (maxcount=0, ac=0; ac<'~'; ac++) 
			 if (basecount[ac] > maxcount) {
				maxcount= basecount[ac];
				maxc= ac;
				}
					 
			enum { kMaxGrey = 127 }; // 255 is max color level
			unsigned long greycolor = 0;
			unsigned short level;
			float flevel= float(maxcount) / float(nbases);
			level= kMaxGrey - (kMaxGrey * flevel);
			level= 0xff & (level << 1);
 
#ifdef WIN_MAC
			greycolor= (((ulong)level)<<16) | (level<<8) | level;
#endif
#ifdef WIN_MSWIN
  		greycolor = (level | level << 8 | ((ulong)level) << 16); //== RGB (cr, cg, cb);
#endif
#ifdef WIN_MOTIF
			// X uses mapped values thru setcolor -- need to do the select/get dance
			Nlm_SelectColor( level, level, level);
			greycolor= Nlm_GetColor();
#endif
			fColcolors[icol-startcol]= greycolor;
		 
			}
		
		}

	DTableView::Draw();
}


void DAlnView::DrawRow(Nlm_RecT r, short row)
{
 	// DTableView::Draw() does SelectFont(fFont)
  if (fEditRow == row) {
 		// do nothing? , fEditSeq handle's redraw ??
		fColsDrawn= 0;
 		}
 	else switch (fColorBases) {
 		case kBaseVarLite:
 		case kBaseColor: 
 			DrawAllColors(r, row); 
 			break;
 		case kBaseBlack: 
 		default: 
 			DrawNoColors(r, row); 
 			break;
 		}
}

		
void DAlnIndex::DrawCell(Nlm_RecT r, short row, short col)
{
#if 0
	long stopcol, startcol = fLeft; 
	long newright= r.left;
	short	cwidth= Nlm_CharWidth('G'); //fItemWidth
	
	for (stopcol= startcol; stopcol<fMaxCols && newright<r.right; stopcol++) {
		//if (fWidths) newright += fWidths[startcol]; else 
		newright += cwidth;
		// optimize later -- check update region by each char rect ...
		//if (Nlm_RectInRgn (&item_rect, Nlm_updateRgn)) done= true;
		}
#endif

	// add cols for Kind(), Length(), Origin(), UpdateTime(), Info()(wide col..better in box)
		
	DSequence* aSeq= fSeqList->SeqAt(row);
	if (aSeq) switch (col) {
		case 0: 
			{
			char* hSeq= aSeq->Name();
			Nlm_DrawString( &r, hSeq, 'l', false);
			return;
			}
		case 1:
			{
			char buf[128];
			sprintf(buf,"%d ", aSeq->LengthF()); // switch to ostrstream !?
			Nlm_DrawString( &r, buf, 'r', false);
			return;
			}
		case 2:
			{
			char* hSeq= aSeq->KindStr();
			Nlm_DrawString( &r, hSeq, 'l', false);
			return;
			}
		case 3:
			{
			char buf[128];
			sprintf(buf,"%8lx ", aSeq->Checksum()); // switch to ostrstream !?
			Nlm_DrawString( &r, buf, 'l', false);
			return;
			}
		}
}


void DAlnITitle::DrawCell(Nlm_RecT r, short row, short col)
{
	switch (col) {
		case 0: 
			{
			Nlm_DrawString( &r, "Name", 'l', false);
			return;
			}
		case 1:
			{
			Nlm_DrawString( &r, "Bases", 'l', false);
			return;
			}
		case 2:
			{
			Nlm_DrawString( &r, "Kind", 'l', false);
			return;
			}
		case 3:
			{
			Nlm_DrawString( &r, "Checksum", 'l', false);
			return;
			}
		}
}

void DAlnHIndex::Draw()
{
	Nlm_RecT r;
	short atx, aty;
	char nums[128];

	fDoc->fAlnView->SelectFont();  
	ViewRect( r);
	long stopcol, startcol = fDoc->fAlnView->GetLeft(); 
	long newright= r.left;
	short	cwidth= Nlm_CharWidth('G');  

#if 0	
	Nlm_RecT vr;
	// is ViewRect bad !? (too short)  YES - isn't grown !?
  fDoc->fAlnView->ViewRect( vr);
  r.right= vr.right; //??
	Nlm_ClipRect( &r); // ?? why is hindex clipped to small !?
#endif
	
	for (stopcol= startcol; 
		stopcol < fDoc->fAlnView->GetMaxCols() && newright<r.right;
	  stopcol++) {
		newright += cwidth;
		}

	long len= stopcol - startcol;
	atx= r.left-(cwidth/2);
	aty= r.bottom;
	for (short i= startcol; i<=stopcol; i++) {
		if (i == 0) ; //skip
		else if (i % 10 == 0) {
			Nlm_MoveTo(atx, aty);
			Nlm_LineTo(atx, aty-2);
			Nlm_MoveTo(atx+1, aty);
			Nlm_LineTo(atx+1, aty-2);
			sprintf( nums, "%d", i);
			short ws= Nlm_StringWidth(nums);
			Nlm_MoveTo( atx-(ws/ 2), aty-3);
			Nlm_PaintString(nums);
			//Nlm_DrawString( &r1, nums, 'c', false);
			}	
		else if (i % 5 == 0) {
			Nlm_MoveTo(atx, aty);
			Nlm_LineTo(atx, aty-6);
			}
		else {
			Nlm_MoveTo(atx, aty);
			Nlm_LineTo(atx, aty-2);
			}
		atx += cwidth;
		}
}




		

#if FIX_LATER
Boolean DAlnView::ContainsClipType(long aType) // override 
{
	return (aType == kAlnScrapType);
	//!? also (aType == 'TEXT') can be pasted as seq... w/ SeqReadScrap...
}
#endif

#if FIX_LATER
void DAlnView::WriteToDeskScrap() // override 
{
	short format = kGenBank;
	//if (fDoc->fFormatPop) format= fDoc->fFormatPop->GetValue(); 
	fSeqList->ClearSelections();
	char* textScrap= fSeqList->doWriteHandle( format);  
	short err = gClipboardMgr->PutDeskScrapData('TEXT', textScrap);
	MemFree( textScrap);
}
#endif




		
		
		
// DAlnIndex	 ------------------


char* DAlnIndex::GetItemTitle(short item, char* title, size_t maxsize)   
{
  if (fSeqList && item <= fSeqList->GetSize()) {
		DSequence* aSeq= fSeqList->SeqAt(item);
		if (aSeq) {
			if (!title) title= (char*) MemNew( maxsize);
			StrNCpy( title, aSeq->Name(), maxsize);
			return title;
			}
		}
	return NULL;
}


#if FIX_LATER

void TAlnIndex::UpdateSize(void)
VAR	diff: integer;
{
	if ((fAlnView!=NULL)) {
		diff= fAlnView->fNumOfRows - fNumOfRows;
		if ((diff>0)) InsRowLast( 1, fAlnView->fRowheight) 	
		else if ((diff<0)) DelItemLast( -diff);
		}	else
		DelItemLast(fNumOfRows);
}

void TAlnIndex::ReSelect(RgnHandle indexRegion)
VAR  aCell: GridCell;
{
	/*-----
	aCell = (*indexRegion)->rgnBBox.topLeft;
	if (!IsCellVisible(aCell)) PositionAtCell(aCell);				 
	----*/
	
	if (!EqualRect((*indexRegion)->rgnBBox, (*fSelections)->rgnBBox)) {
		/*---- ??
		WITH fDoc){
			fAlnView->SetEmptySelection(kHighlight);  
			}
		-----*/
		SetSelection(indexRegion, kDontExtend, kHighlight, kSelect);
		}
}



TAlnIndex::HandleMouseDown( VPoint theMouse, TToolboxEvent event, 
							 hysteresis: Point):Boolean; // override 
{		
	fAlnView->DeInstallEditSeq();
	fAlnView->SetEmptySelection(kHighlight); 
	
  HandleMouseDown= inherited::HandleMouseDown(theMouse,event,hysteresis);
}

void TAlnIndex::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 
VAR
	aRow, aCol	: short;
	aCell	: GridCell;
	aAlnShifter		: TAlnShifter;
	boolean		aDoubleClick;
	aName	: Str255;
	vpt		: VPoint;
	GridViewPart		gridPart;
	aSeq	: DSequence;
	
{
	aDoubleClick= event->fClickCount > 1;
	gridPart= IdentifyPoint(theMouse, aCell);
	aRow= aCell.y;
	
	if (aDoubleClick) {
		aSeq= DSequence(fSeqList->At(aRow));
		if (aSeq!=NULL)) TAlnDoc(fDocument)->OpenSeqedWindow(aSeq);
		}		
	else if ((event.IsShiftKeyPressed |event->IsCommandKeyPressed())) { 
		inherited::DoMouseCommand(theMouse,event,hysteresis) 
		}		
	else if (gridPart != badChoice) {
		aCell	= VPointToLastCell(theMouse);
		//if TRUE then begin
		if (isCellSelected(aCell)) {
			GetItemText(aRow, aName);
			New(aAlnShifter);
			FailNIL(aAlnShifter);
			aAlnShifter->IAlnShifter(this, fAlnView, aRow, aName);
			PostCommand( aAlnShifter);
			}		else
			inherited::DoMouseCommand(theMouse,event,hysteresis); 
		}
}

#endif // FIX_LATER


