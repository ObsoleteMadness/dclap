// DSeqCmds.cp


#include <ncbi.h>
#include <DTask.h>
#include <DApplication.h>
#include <DClipboard.h>
#include "DSeqDoc.h"
#include "DSeqEd.h"
#include "DSeqCmds.h"



static Nlm_PoinT gZeroPt = { 0, 0 };


// DSeqChangeCmd : public DCommand ----------------------------

DSeqChangeCmd::DSeqChangeCmd(char* title, DSeqDoc* itsAlnDoc, DView* itsView, DSeqList* itsSeqs) :
	DCommand( cSeqChange, itsView, title, DCommand::kCanUndo, DCommand::kCausesChange)
{
	fOldSeqs= NULL;
	fNewSeqs= NULL;
	fAlnView= itsAlnDoc->fAlnView;
	fSeqDoc= itsAlnDoc;
	//fAlnView->DeinstallEditSeq(); //! prevent mangle do to fBases newhandle 
	
	//- gApplication->CommitLastCommand(); -- Paste seqs is giving probs...
	// theContext:	TCommandHandler;
	//theContext= itsView->GetContext(cSeqChange);
	//ICommand(cSeqChange, theContext, kCanUndo, kCausesChange, theContext);

#if 0
		// let user fix fOldSeqs !?
	if (itsSeqs == NULL)  {
		long start, nbases;
		itsAlnDoc->GetSelection( TRUE, TRUE, itsSeqs, start, nbases);
		}
#endif
	fOldSeqs= itsSeqs;
	fNewSeqs= new DSeqList(); 
	
	// caller must call Initialize() before other use of this object
}	


DSeqChangeCmd::~DSeqChangeCmd() 
{
	if (fNewSeqs) {
		fNewSeqs->DeleteAll(); 		//forget objects -- doesn't affect objects 
		delete fNewSeqs; 			 
		}
	if (fOldSeqs) {
		fOldSeqs->FreeAllObjects(); 	//drop all objects && list
		delete fOldSeqs;
		}
}

Boolean DSeqChangeCmd::Initialize()  
{
	if (fOldSeqs) {
		short i, n= fOldSeqs->GetSize();
		for (i=0; i<n; i++) {
			DSequence* oldSeq= fOldSeqs->SeqAt(i);
			DSequence* newSeq = ChangeToNew(oldSeq);
			if (newSeq) 
				fNewSeqs->InsertLast( newSeq);
			else  
				return false; // fail 
			}
		}
	return true;
}

void DSeqChangeCmd::DoIt()  
{
	DCommand::DoIt();
	fSeqDoc->Dirty();
	DoItWork();
}

void DSeqChangeCmd::Undo()  
{
	DCommand::Undo();
	fSeqDoc->UnDirty();
	UndoWork();
}

void DSeqChangeCmd::Redo()  
{
	DCommand::Redo();
	fSeqDoc->Dirty();
	UndoWork();
}

void DSeqChangeCmd::UndoWork()  
{
	DSeqList* tempList= fNewSeqs;
	fNewSeqs= fOldSeqs;
	fOldSeqs= tempList;
	DoItWork();
}



void DSeqChangeCmd::DoItWork()  
{
	short				i, j, nseqs, aFromRow, aToRow, indx, saveEdit;
	DSequence		* oldSeq, * newSeq;
	DSeqList 		*	fullList;
	
	if (fOldSeqs && fNewSeqs) {
		saveEdit= fAlnView->fEditRow;		
		aFromRow= kEmptyIndex;
		aToRow= kEmptyIndex;
		nseqs= fNewSeqs->GetSize();
		fullList= fAlnView->fSeqList;
		for (i= 0; i<nseqs; i++) {
			newSeq= fNewSeqs->SeqAt(i);
			oldSeq= fOldSeqs->SeqAt(i);
#if 1
			long k, nfull= fullList->GetSize();
			if (oldSeq) for (k=0, j= kEmptyIndex; k<nfull; k++) {
				DSequence* aseq= fullList->SeqAt(k);
				if (aseq && aseq->Checksum() == oldSeq->Checksum() 
					&& aseq->LengthF() == oldSeq->LengthF()
					&& aseq->ModTime() == oldSeq->ModTime()
					) {
						j= k;
						break;
						}
				}
#else
			j= fullList->GetEqualItemNo( oldSeq); // this doesn't work, default equal item tests ptr val
#endif
			if (j>kEmptyIndex) {
				fullList->AtDelete(j); //fullList->Delete( oldSeq);
				fullList->InsertBefore( j, newSeq);
				if (aFromRow == kEmptyIndex) aFromRow= j;
				else aToRow= j;
				}	
			DSeqedWindow::UpdateEdWinds( oldSeq, newSeq);
			}
		
		if (aToRow==kEmptyIndex) aToRow= aFromRow;
		
		nseqs= fullList->GetSize();
		for (i=0, indx= 0; i<nseqs; i++) {
			oldSeq= fullList->SeqAt(i);
			if (indx >= aFromRow && indx <= aToRow) {
				//! also need to shift any selection or deselect all...
				Nlm_RecT vLoc;
				fAlnView->GetRowRect( indx, vLoc, 1);
				fAlnView->InvalRect( vLoc);
				if (indx == saveEdit) {
					fAlnView->fEditRow= saveEdit;
#if 0
					fAlnView->fEditSeq->Locate( vLoc.topLeft, true);
					vSize= fAlnView->fEditSeq.fSize;
					fAlnView->fEditSeq->Resize( vSize, false);  
#endif
					}
					
				// ??? isn't this already done above
				//fAlnView->GetCellRect( indx, 1, vLoc);
				//fAlnView->InvalRect( vLoc);
				}
			indx++;
			}
			
		//- fAlnView->SetEmptySelection(kHighlight); 
		//fCommandDone= true; 
		}
}

void DSeqChangeCmd::Commit()  
{
	DCommand::Commit(); 
	//this->DoNotification();
}


DSequence* DSeqChangeCmd::ChangeToNew( DSequence* oldSeq) 
{ 	
	return (DSequence*) oldSeq->Clone();
}

DSequence* DSeqReverseCmd::ChangeToNew( DSequence* oldSeq) 
{ 	
	return oldSeq->Reverse();
}

DSequence* DSeqComplementCmd::ChangeToNew( DSequence* oldSeq) 
{ 	
	return oldSeq->Complement();
}

DSequence* DSeqRevComplCmd::ChangeToNew( DSequence* oldSeq) 
{ 	
	DSequence* tempSeq= oldSeq->Reverse();  
	DSequence* result= tempSeq->Complement();
	delete tempSeq;  
	return result;
}


DSequence* DSeqCompressCmd::ChangeToNew( DSequence* oldSeq) 
{ 	
	return oldSeq->Compress();
}

DSequence* DSeqDna2RnaCmd::ChangeToNew( DSequence* oldSeq) 
{ 	
	return oldSeq->Dna2Rna(TRUE);
}

DSequence* DSeqRna2DnaCmd::ChangeToNew( DSequence* oldSeq) 
{ 	
	return oldSeq->Dna2Rna(FALSE);
}

DSequence* DSeqTranslateCmd::ChangeToNew( DSequence* oldSeq) 
{ 	
	return oldSeq->Translate();
}

DSequence* DSeqLockIndelsCmd::ChangeToNew( DSequence* oldSeq) 
{ 	
	return oldSeq->LockIndels(TRUE);
}

DSequence* DSeqUnlockIndelsCmd::ChangeToNew( DSequence* oldSeq) 
{ 	
	return oldSeq->LockIndels(FALSE);
}

DSequence* DSeqUppercaseCmd::ChangeToNew( DSequence* oldSeq) 
{ 	
	return oldSeq->ChangeCase(TRUE);
}

DSequence* DSeqLowercaseCmd::ChangeToNew( DSequence* oldSeq) 
{ 	
	return oldSeq->ChangeCase(FALSE);
}






		


// DAlnSlider --------------------------------------


DAlnSlider::DAlnSlider()
{
	fSlideAll= true;
	fAlnView= NULL;	
	fSeqDoc= NULL;
	fOldSeqs= NULL;
	fNewSeqs= NULL;
	//IAlnSlider( NULL, NULL, 0);	
}

DAlnSlider::DAlnSlider( DSeqDoc* itsDoc, DTableView* itsView, DAlnView* itsAlnView,  short oldRC)
{
	IAlnSlider( itsDoc, itsView, itsAlnView, oldRC);	
}

DAlnSlider::~DAlnSlider() 
{
	fNewSeqs->fDeleteObjects= false;
	FreeListIfObject( fNewSeqs);  
	fOldSeqs->fDeleteObjects= true;
	FreeListIfObject( fOldSeqs); //drop all objects && list
#if 0
	fOldSelection= Nlm_DestroyRgn( fOldSelection);
	fNewSelection= Nlm_DestroyRgn( fNewSelection);
#endif
}

void DAlnSlider::IAlnSlider(DSeqDoc* itsDoc, DTableView* itsView, DAlnView* itsAlnView, short oldRC)
{
	fAlnView= itsAlnView;	
	fSeqDoc= itsDoc;
	fOldSeqs= NULL;
	fNewSeqs= NULL;
	
	fSlideAll= ! (gKeys->shift());
	//fSlideAll=  (gKeys->shift()); // !! which is best !?!?!?
	

	IRCshifter( cAlnShiftCmd, itsDoc, itsView, kDoCol, oldRC); 
		// IRCshifter->ITracker->Reset()  calls our Reset() !
#if 0
	fOldSeqs= new DSeqList();  
	fNewSeqs= new DSeqList();  
#if 1
	fOldSelection= fTable->GetSelRect();
	fNewSelection= fOldSelection;
#else
	fOldSelection= Nlm_NewRgn(); 
	fNewSelection= Nlm_NewRgn();  
	CopyRgn( itsAlnView->fSelections, fOldSelection);
	CopyRgn( fOldSelection, fNewSelection);
#endif
#endif
}

void DAlnSlider::Reset()
{
	if (fNewSeqs) {
	fNewSeqs->fDeleteObjects= false;
	FreeListIfObject( fNewSeqs);  
	}
	if (fOldSeqs) {
	fOldSeqs->fDeleteObjects= true;
	FreeListIfObject( fOldSeqs); //drop all objects && list
	}
	fOldSeqs= new DSeqList();  
	fNewSeqs= new DSeqList();  
	if (fTable) fOldSelection= fTable->GetSelRect();
	fNewSelection= fOldSelection;
}



void DAlnSlider::TrackFeedback( TrackPhase aTrackPhase,
					const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
					const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn)
{
	short row, col, oldrow, oldcol;
	Nlm_RecT 	selr, pixr;
	
	switch (aTrackPhase) {
			
		case trackContinue:
			//mouseDidMove= ((previousPoint.x != nextPoint.x)); //(col != pcol);
		case trackBegin:
		case trackEnd:
			if (mouseDidMove) {
				fTable->PointToCell( nextPoint, row, col);
				fTable->PointToCell( anchorPoint, oldrow, oldcol);
				selr= fOldSelection;
				if (fRowCol == kDoCol)
					Nlm_OffsetRect( &selr, col - oldcol, 0);
				else
					Nlm_OffsetRect( &selr, 0, row - oldrow);
#if 1
				fTable->GetCellRect( selr, pixr);
				Nlm_InvertMode();
				Nlm_FrameRect( &pixr);
				Nlm_CopyMode();
#else
				fTable->SelectCells( selr, 
						!DTableView::kExtend, DTableView::kHighlight, DTableView::kSelect);
#endif
				}
			break;
		}
}


void DAlnSlider::DoSlide()
{
	short 	row, col, left, right, top, bottom;
	long		start, nbases, dist;
	DSequence * oldSeq, * newSeq;
 	
	dist = fNewRC - fOldRC;
	
	// if (fRowCol == kDoCol)
	Nlm_OffsetRect( &fNewSelection, dist, 0);
	
	//fTable->GetFirstSelectedCell( top, left);
	//fTable->GetLastSelectedCell( bottom, right);
	top= fOldSelection.top;
	bottom= fOldSelection.bottom;
	left= fOldSelection.left;
	right= fOldSelection.right;
	for (row= Max(0,top); row<bottom; row++) {		
		col= left;
		//while (col<right && !fTable->IsSelected(row,col)) col++;
		start= col;
		col= right; 
		//while (col<=right && fTable->IsSelected(row,col)) col++;
			// ! nbases == 0 is key to slide all of sequence from sel to top...
		if (dist<0 && fSlideAll) nbases= 0;
		else nbases= col - start;
		if (nbases>=0) {
			oldSeq= fAlnView->fSeqList->SeqAt(row);
			if (oldSeq) {
				oldSeq->SetIndex( row);
				oldSeq->SetSelection( start, nbases);
				newSeq= oldSeq->Slide( dist); //< ?? DSeqChangeCmd !?
				oldSeq->ClearSelection();		// 12nov92 - fix for write trunc bug? 
				if (newSeq) {
					newSeq->ClearSelection();
					fOldSeqs->InsertLast(oldSeq);
					fNewSeqs->InsertLast(newSeq);
	  			}
				}
			}
		}
}


void DAlnSlider::DoItWork()  
{
/* insert "-" at left/5'/bottom of sequence if dist>0
	squeeze out "-" at top/3' of sequence.
	If dist<0 then slide opposite direction.
*/
	Nlm_RecT 	r;
	short			zeroshift, left;
	
	if (fMovedOnce) {
		short i, n= fNewSeqs->GetSize();
		for (i=0; i<n; i++) {		
				// replaceViewSeq
			DSequence* aSeq= fNewSeqs->SeqAt(i);
			fAlnView->fSeqList->AtPut( aSeq->Index(), aSeq);
			fAlnView->GetRowRect( aSeq->Index(), r);
			fAlnView->InvalRect( r);
			}

		zeroshift= fAlnView->fSeqList->ZeroOrigin(); //!?
		if (zeroshift) {
				// THIS is getting messy w/ undo/redo because zeroOrigin affects All seqs!
			fAlnView->UpdateAllWidths();
			left= fNewSelection.left; //(*fNewSelection)->rgnBBox.left;
			if (left + zeroshift <= 0)  zeroshift= -left + 1; //??
			Nlm_OffsetRect( &fNewSelection, zeroshift, 0); 
			fAlnView->Invalidate(); 
			}

				// the inval/redraw will handle selection highlight
		fTable->SelectCells( fNewSelection, 
				!DTableView::kExtend, !DTableView::kHighlight, DTableView::kSelect);

#if 1
		n= fNewSeqs->GetSize();
		for (i= 0; i<n; i++)
			DSeqedWindow::UpdateEdWinds( fOldSeqs->SeqAt(i), fNewSeqs->SeqAt(i) );
#endif
		}
}


void DAlnSlider::UndoWork()  
{
	DSeqList* tmp= fOldSeqs; 
	fOldSeqs= fNewSeqs;
	fNewSeqs= tmp;
#if 1
	Nlm_RecT tmpRect= fOldSelection;
	fOldSelection= fNewSelection;
	fNewSelection= tmpRect;
#else
	Nlm_RgN tmpRgn= fOldSelection;
	fNewSelection= fOldSelection;
	fOldSelection= tmpRgn;
#endif
	DoItWork();
}

void DAlnSlider::DoIt()  
{
	short row, col, oldrow, oldcol;
	
	fTable->PointToCell( fNextPoint, row, col);
	fTable->PointToCell( fAnchorPoint, oldrow, oldcol);
	if (fRowCol == kDoRow) { fOldRC= oldrow; fNewRC= row; }
	else if (fRowCol == kDoCol) { fOldRC= oldcol; fNewRC= col; }
	
	if (fNewRC != fOldRC) DoSlide();
	 
	DCommand::DoIt();
	fSeqDoc->Dirty();
	DoItWork();
}

void DAlnSlider::Undo()  
{
	DCommand::Undo();
	fSeqDoc->UnDirty();
	UndoWork();
}

void DAlnSlider::Redo()  
{
	DCommand::Redo();
	fSeqDoc->Dirty();
	UndoWork();
}






// DAlnShifter --------------------------------



DAlnShifter::DAlnShifter()
{
}

DAlnShifter::DAlnShifter( DSeqDoc* itsDoc, DTableView* itsView, DAlnView* itsAlnView, short oldRC) :
  DAlnSlider( itsDoc, itsView, itsAlnView, oldRC)
{
	DAlnShifter::IAlnSlider( itsDoc, itsView, itsAlnView, oldRC);
}

DAlnShifter::~DAlnShifter() 
{
}

void DAlnShifter::IAlnSlider( DSeqDoc* itsDoc, DTableView* itsView, DAlnView* itsAlnView, short oldRC)
{
	fAlnView= itsAlnView;	
	fSeqDoc= itsDoc;
	fOldSeqs= NULL;
	fNewSeqs= NULL;

	IRCshifter( cAlnShiftCmd, itsDoc, itsView, kDoRow, oldRC);
	fOldSeqs= new DSeqList();  
	fNewSeqs= new DSeqList();  
	fOldSelection= fTable->GetSelRect();
	fNewSelection= fOldSelection;
}


void DAlnShifter::TrackFeedback( TrackPhase aTrackPhase,
					const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
					const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn)
{
	DAlnSlider::TrackFeedback( aTrackPhase, anchorPoint, previousPoint, nextPoint,
														mouseDidMove, turnItOn);
}


void DAlnShifter::DoSlide()
{
}


void DAlnShifter::UndoWork()  
{
	DSeqList* tmp= fOldSeqs; 
	fOldSeqs= fNewSeqs;
	fNewSeqs= tmp;

	Nlm_RecT tmpRect= fOldSelection;
	fOldSelection= fNewSelection;
	fNewSelection= tmpRect;

	short saverc= fNewRC;
	fNewRC= fOldRC;
	fOldRC= saverc;
	
	DoItWork();
}

void DAlnShifter::DoItWork()  
{
	short diff = fNewRC - fOldRC;
	if (fMovedOnce && diff) {
		short i, nrows, newtop, maxrow, top, bottom, minr, maxr;
		DSeqList* tmpseq;
		DSequence* aseq;
		Nlm_RecT	r;
		
		tmpseq= new DSeqList();
		top= fOldSelection.top;
		bottom= fOldSelection.bottom;
		nrows= bottom - top;
		if (nrows<1) nrows= 1;
		newtop= top + diff;
		maxrow= fAlnView->fSeqList->GetSize();
		if (newtop<0) {
			newtop=0;
			diff= newtop - top;
			}
		else if (newtop > maxrow-nrows) {
			newtop= maxrow-nrows;
			diff= newtop - top;
			}
			
#if 1
		fNewSelection= fOldSelection;
		fNewSelection.top= newtop;
		fNewSelection.bottom= newtop + nrows;
#else
		if (fRowCol == kDoCol)
			Nlm_OffsetRect( &fNewSelection, diff, 0);
		else
			Nlm_OffsetRect( &fNewSelection, 0, diff);
#endif			
		
		for ( i= 0; i<nrows; i++) {
		  aseq= fAlnView->fSeqList->SeqAt( i+top);
			tmpseq->InsertLast(aseq);
			}
		for (i= nrows-1; i>=0; i--) 
		  fAlnView->fSeqList->AtDelete( i+top);
		for (i= 0; i<nrows; i++) {
		  aseq= tmpseq->SeqAt(i);
		  fAlnView->fSeqList->InsertBefore( i+newtop, aseq);
			}

		tmpseq->DeleteAll();  
		delete tmpseq; 			//drop list, not objects
		 
		minr= Min( top, newtop);
		maxr= Max( top, newtop);
		fTable->GetRowRect( minr, r, (maxr-minr) + nrows);
		fTable->InvalRect( r);
		if (fTable != fAlnView) {
			fAlnView->GetRowRect( minr, r, (maxr-minr) + nrows);
			fAlnView->InvalRect( r);
			}
		
		fTable->SelectCells( fNewSelection, 
				!DTableView::kExtend, !DTableView::kHighlight, DTableView::kSelect);
		}
}






		

//	DAlnEditCommand 

DAlnEditCommand::DAlnEditCommand(DSeqDoc* itsDoc, short itsCommand) :
	DCommand( itsCommand, itsDoc->fAlnView, "edit", DCommand::kCanUndo, DCommand::kCausesChange)
{
	fAlnDoc = itsDoc;
	fOldList= NULL;
	//fChangesClipboard = (itsCommand != DApplication::kClear);
	fCausesChange = (itsCommand != DApplication::kCopy);
	fCanUndo = (itsCommand != DApplication::kCopy);  //?

	fOldList= new DSeqList();
	long  i, n= fAlnDoc->fSeqList->GetSize();
	for (i=0; i<n; i++) fOldList->InsertLast( fAlnDoc->fSeqList->At(i));

	fOldList->fDeleteObjects= false;
	Nlm_LoadRect( &fSelection, 0,0,0,0);
	if (fAlnDoc->fAlnIndex) fSelection= fAlnDoc->fAlnIndex->GetSelRect();
}



DAlnEditCommand::~DAlnEditCommand()  
{
	//fSelection= Nlm_DestroyRgn( fSelection);
	FreeListIfObject( fOldList); 
	fOldList= NULL;
}


void DAlnEditCommand::CopySelection()
{
	long	irow, minrow, maxrow;
	DSeqList 	*	aSeqList;
	DWindow		* clipWindow;
	DAlnView	* clipView = NULL;
	DSeqDoc		*	clipAlnDoc = NULL;
	
#if 1
		// don't want/need DSeqDoc window for clipview !!?
	aSeqList= new DSeqList();
	//clipWindow= new DWindow(0, NULL, DWindow::document, -5, -5, -50, -20, "Clip"); 
	clipWindow= gClipboardMgr->fClipWindow; //!! contents of fClipWindow are getting trashed
#else	
	clipAlnDoc= new DSeqDoc(DSeqDoc::kSeqdoc,NULL,"clip");   
	aSeqList= clipAlnDoc->fSeqList;
	clipWindow= clipAlnDoc;
#endif

	minrow= fSelection.top;
	maxrow= fSelection.bottom;
	for (irow=minrow; irow<maxrow; irow++) {
	 	DSequence* aseq= fAlnDoc->fSeqList->SeqAt(irow);
	 	if (aseq) aSeqList->InsertLast( aseq->Clone());
		}

	clipView= new DAlnView(0,clipWindow,clipAlnDoc,aSeqList, 120, 100);
	if (clipAlnDoc) clipAlnDoc->fAlnView= clipView;
	else clipView->fOwnSeqlist= true;

#if 1
	clipView->GetReadyToShow();
#else
	clipView->UpdateSize();
	clipView->UpdateAllWidths(); 
#endif

	//this->ClaimClipboard(clipView);  
	gClipboardMgr->SetClipView(clipView);	
}



void DAlnEditCommand::DeleteSelection()
{ 
  long irow, minrow, maxrow;
	long totalRows= fAlnDoc->fAlnView->GetMaxRows(); //fOldList->GetSize();
	DAlnIndex* tab= fAlnDoc->fAlnIndex;
  minrow= fSelection.top;
  maxrow= fSelection.bottom;
	Boolean savedel= fAlnDoc->fSeqList->fDeleteObjects;
	fAlnDoc->fSeqList->fDeleteObjects= false;
	for (irow= maxrow-1; irow>=minrow; irow--)  {
		DSequence* aSeq= fAlnDoc->fSeqList->SeqAt(irow);
		if (aSeq) {
			Nlm_RecT 	r;
			aSeq->SetDeleted( true);
			fAlnDoc->fSeqList->AtDelete(irow); 
			fAlnDoc->fAlnView->GetRowRect( irow, r, totalRows-irow+1);
			fAlnDoc->fAlnView->InvalRect( r);
			if (tab) {
				tab->GetRowRect( irow, r, totalRows-irow+1);
				tab->InvalRect( r);
				tab->SelectCells( irow, 0, 
						!DTableView::kExtend, !DTableView::kHighlight, !DTableView::kSelect);
				}
			}
		}
	fAlnDoc->fSeqList->fDeleteObjects= savedel;
	fAlnDoc->fAlnView->UpdateSize();
}


void DAlnEditCommand::RestoreSelection()
{
	long irow, totalRows= fOldList->GetSize();
	long minrow= fSelection.top;
	for (irow=totalRows-1; irow>=0; irow--) {
		DSequence* aSeq= fOldList->SeqAt(irow);
		if (aSeq && aSeq->IsDeleted()) {
			aSeq->SetDeleted( false);
			fAlnDoc->fSeqList->InsertBefore( minrow, aSeq); 	

			//fAlnDoc->fAlnView->InsRowBefore( minrow, 1, fAlnDoc->fAlnView->fRowHeight);
			//fAlnDoc->fAlnIndex->InsRowBefore( minrow, 1, fAlnDoc->fAlnView-fRowHeight);
			fAlnDoc->fAlnView->ChangeRowSize( fAlnDoc->fAlnView->GetMaxRows(), 1);
			if (fAlnDoc->fAlnIndex) 
				fAlnDoc->fAlnIndex->ChangeRowSize( fAlnDoc->fAlnIndex->GetMaxRows(), 1);
			}
		}
		
	fAlnDoc->fAlnView->UpdateSize();  
	ReSelect(); 
}


void DAlnEditCommand::Commit()
{
	long irow, totalRows= fOldList->GetSize();
	Boolean savedel= fOldList->fDeleteObjects;
	fOldList->fDeleteObjects= true;
	for (irow= totalRows-1; irow>=0; irow--) {
		DSequence* aSeq= fOldList->SeqAt(irow);
		if (aSeq && aSeq->IsDeleted())  
			fOldList->AtDelete(irow);
		}
	fOldList->fDeleteObjects= savedel;
	DCommand::Commit(); 
}


void DAlnEditCommand::ReSelect()
{
	if (fAlnDoc->fAlnIndex)
	  fAlnDoc->fAlnIndex->SelectCells( fSelection, 
				!DTableView::kExtend, DTableView::kHighlight, DTableView::kSelect);
}


void DAlnEditCommand::DoItWork()
{
	if (fNumber != DApplication::kClear) CopySelection();
	if (fNumber != DApplication::kCopy) DeleteSelection();
}


void DAlnEditCommand::UndoWork()
{
	if (fNumber != DApplication::kCopy) {
		RestoreSelection();
		//fAlnDoc->fAlnView->ScrollSelectionIntoView(TRUE);
		}
}


void DAlnEditCommand::RedoWork()
{
	if (fNumber != DApplication::kCopy) {
		DeleteSelection();
		//fAlnDoc->fAlnView->ScrollSelectionIntoView(TRUE);
		}
}

void DAlnEditCommand::DoIt()  
{
	DCommand::DoIt();
	fAlnDoc->Dirty();
	DoItWork();
}

void DAlnEditCommand::Undo()  
{
	DCommand::Undo();
	fAlnDoc->UnDirty();
	UndoWork();
}

void DAlnEditCommand::Redo()  
{
	DCommand::Redo();
	fAlnDoc->Dirty();
	RedoWork();
}












// DAlnPasteCommand 


DAlnPasteCommand::DAlnPasteCommand(DSeqDoc* itsDoc) :
	DCommand( DApplication::kPaste, itsDoc->fAlnView, "paste", DCommand::kCanUndo, DCommand::kCausesChange)
{
	fAlnDoc = itsDoc;
	fClipList= NULL;
	//fChangesClipboard = false;

	short col;
	fInsRow= 0;
	if (fAlnDoc->fAlnIndex) 
		fAlnDoc->fAlnIndex->GetLastSelectedCell( fInsRow, col);
	if (fInsRow == DTableView::kNoSelection) 
		fInsRow= fAlnDoc->fSeqList->GetSize();

	DAlnView* clipview= NULL;
  clipview= (DAlnView*) gClipboardMgr->fClipView; // GetClipView();
	if (clipview && clipview->fKind == DAlnView::kindAlnView) {
		long  i, n;
		fClipList= new DSeqList();
		n= clipview->fSeqList->GetSize();
		for (i=0; i<n; i++) {
			DSequence* aSeq= clipview->fSeqList->SeqAt(i);
			fClipList->InsertLast( aSeq->Clone());
			}
		fClipList->fDeleteObjects= true;
	}
}


DAlnPasteCommand::~DAlnPasteCommand()  
{
	FreeListIfObject( fClipList); //drop all objects && list 
	fClipList= NULL;
}


void DAlnPasteCommand::DoItWork()
{
	if (fClipList) {
		long irow, nrows= fClipList->GetSize();
		for (irow= nrows-1; irow>=0; irow--) {
			DSequence* aSeq= fClipList->SeqAt(irow);
			aSeq= (DSequence*) aSeq->Clone();
			aSeq->SetDeleted(true);
			
			fAlnDoc->fSeqList->InsertBefore( fInsRow, aSeq);
			//fAlnDoc->fAlnView->InsRowBefore( fInsRow, 1, fAlnDoc->fAlnView->fRowHeight);
			//fAlnDoc->fAlnIndex->InsRowBefore( fInsRow, 1, fAlnDoc->fAlnView.fRowHeight);
			fAlnDoc->fAlnView->ChangeRowSize( fAlnDoc->fAlnView->GetMaxRows(), 1);
			if (fAlnDoc->fAlnIndex)
				fAlnDoc->fAlnIndex->ChangeRowSize( fAlnDoc->fAlnIndex->GetMaxRows(), 1);
			}
		fAlnDoc->fAlnView->UpdateAllWidths();
		fAlnDoc->fAlnView->UpdateSize(); 
		} 
} 


void DAlnPasteCommand::UndoWork()
{ 
	if (fClipList) {
		long irow, totalRows= fAlnDoc->fSeqList->GetSize();
		Boolean savedel= fAlnDoc->fSeqList->fDeleteObjects;
		fAlnDoc->fSeqList->fDeleteObjects= true;
		for (irow= totalRows-1; irow>=0; irow--) {
			DSequence* aSeq= fAlnDoc->fSeqList->SeqAt(irow);
			if (aSeq && aSeq->IsDeleted()) {
				Nlm_RecT 	r;
				aSeq->SetDeleted(false);
				fAlnDoc->fSeqList->AtDelete(irow);
				fAlnDoc->fAlnView->GetRowRect( irow, r, totalRows-irow+1);
				fAlnDoc->fAlnView->InvalRect( r);
				if (fAlnDoc->fAlnIndex) {
					fAlnDoc->fAlnIndex->GetRowRect( irow, r, totalRows-irow+1);
					fAlnDoc->fAlnIndex->InvalRect( r);
					fAlnDoc->fAlnIndex->SelectCells( irow, 0, 
							!DTableView::kExtend, !DTableView::kHighlight, !DTableView::kSelect);
					}
				}
			}
		fAlnDoc->fSeqList->fDeleteObjects= savedel;
		fAlnDoc->fAlnView->UpdateAllWidths();
		fAlnDoc->fAlnView->UpdateSize();  
		}
}



void DAlnPasteCommand::Commit()  
{
	if (fClipList) {
		long i, n= fAlnDoc->fSeqList->GetSize();
		for (i=0; i<n; i++) {
			DSequence* aseq= fAlnDoc->fSeqList->SeqAt(i);
			aseq->SetDeleted(false);
			}
		}
	DCommand::Commit(); 
}


void DAlnPasteCommand::DoIt()  
{
	if (fClipList) {
	DCommand::DoIt();
	fAlnDoc->Dirty();
	DoItWork();
	}
}

void DAlnPasteCommand::Undo()  
{
	if (fClipList) {
	DCommand::Undo();
	fAlnDoc->UnDirty();
	UndoWork();
	}
}

void DAlnPasteCommand::Redo()  
{
	if (fClipList) {
	DCommand::Redo();
	fAlnDoc->Dirty();
	DoItWork();
	}
}



 
