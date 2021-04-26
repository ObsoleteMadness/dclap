// DTabSelect.cp
// d.g.gilbert

// this class is used a lot for various display/user interaction arrays of items
// still needs work for multiple selections, run arrays for height/width?
// 

#include "DTableView.h"
#include <DPanel.h>
#include <DUtil.h>
#include <DApplication.h>
#include <Dvibrant.h>
#include <DTracker.h>
#include <ncbi.h>
#include <dgg.h>





DTabSelector::DTabSelector( DTableView* itsTable)
{
	ITabSelector( itsTable); // in constructor !?
}

DTabSelector::~DTabSelector() 
{
#if 0
	fOldSelection= Nlm_DestroyRgn( fOldSelection);
	fNewSelection= Nlm_DestroyRgn( fNewSelection);
#endif
}

void DTabSelector::ITabSelector( DTableView* itsTable)
{
	ITracker( cTabSelCmd, itsTable, "selection", true, false, itsTable); 
	// ITracker() calls Reset()
}


void DTabSelector::Reset()
{
	DTracker::Reset();
	if (gKeys->shift()) fDoExtend=  DTabSelection::kExtendSingle;
	else if (gKeys->command()) fDoExtend=  DTabSelection::kExtendMulti;
	else fDoExtend=  DTabSelection::kDontExtend;
#if 1
	fOldSelection= ((DTableView*)fView)->GetSelRect();
	fNewSelection= fOldSelection;
#else
	fOldSelection= Nlm_NewRgn(); 
	fNewSelection= Nlm_NewRgn();  
	CopyRgn( ((DTableView*)fView)->fSelections, fOldSelection);
	CopyRgn( fOldSelection, fNewSelection);
#endif
}


void DTabSelector::DoItWork()  
{
	if (fMovedOnce || fDoExtend) {
		((DTableView*)fView)->SelectCells( fNewSelection, 
				fDoExtend,  DTabSelection::kHighlight,  DTabSelection::kSelect);
		}
}

void DTabSelector::UndoWork()  
{
#if 1
	Nlm_RecT tmpRect= fNewSelection;
	fNewSelection= fOldSelection;
	fOldSelection= tmpRect;
#else
	Nlm_RgN tmpRgn= fNewSelection;
	fNewSelection= fOldSelection;
	fOldSelection= tmpRgn;
#endif
	DoItWork();
}


void DTabSelector::DoIt()  
{
	DCommand::DoIt();
	
	Nlm_PoinT pt1, pt2;
	Nlm_RecT	selr;
	long row, col;
		// +1 is messy fix to stop left/top -1 creap !
	pt1.x= MIN( fAnchorPoint.x+1, fNextPoint.x);
	pt2.x= MAX( fAnchorPoint.x-1, fNextPoint.x);
	pt1.y= MIN( fAnchorPoint.y+1, fNextPoint.y);
	pt2.y= MAX( fAnchorPoint.y-1, fNextPoint.y);

	((DTableView*)fView)->PointToCell( pt1, row, col);
	selr.top=row; selr.left= col;
	((DTableView*)fView)->PointToCell( pt2, row, col);
	selr.bottom= row; selr.right= col;
	selr.bottom++;
	selr.right++;
	fNewSelection= selr;
	if (!fDoExtend) {
		if (gKeys->shift()) fDoExtend=  DTabSelection::kExtendSingle;
		else if (gKeys->command()) fDoExtend=  DTabSelection::kExtendMulti;
		}
	
	DoItWork();
}

void DTabSelector::Undo()  
{
	DCommand::Undo();
	short saveext= fDoExtend;
	fDoExtend= DTabSelection::kDontExtend;
	UndoWork();
	fDoExtend= saveext;
}

void DTabSelector::Redo()  
{
	DCommand::Redo();
	UndoWork();
}




//----------------------------

DTabSelection::DTabSelection(DTableView* itsTable, long nrows, long ncols):
		fTable(itsTable),  fMaxRows(nrows), fMaxCols(ncols), 
		fSelrgn(NULL), fSelectedRow(kNoSelection), fSelectedCol(kNoSelection),
		fCanSelectRow(true), fCanSelectCol(false)
{
	fSelrgn= Nlm_CreateRgn();
	fWorkrgn= Nlm_CreateRgn();
	SetEmptySelection(false); 
}
		
DTabSelection::~DTabSelection()
{
	fSelrgn= Nlm_DestroyRgn( fSelrgn);
	fWorkrgn= Nlm_DestroyRgn( fWorkrgn);
}

void DTabSelection::SetTableSize( long rows, long cols)
{
	fMaxRows= rows; 
	fMaxCols= cols;
}

void DTabSelection::ChangeRowSize( long atrow, long deltarows)
{
	long newrows= Max(0, fMaxRows + deltarows);
	SetTableSize( newrows, fMaxCols);
}

void DTabSelection::ChangeColSize( long atcol, long deltacols)
{
	long newcols= Max(0, fMaxCols + deltacols);
	SetTableSize( fMaxRows, newcols);
}

		
void DTabSelection::SetCanSelect(Boolean canrow, Boolean cancol)
{
	fCanSelectRow= canrow;
	fCanSelectCol= cancol;
}


void DTabSelection::SelectCells( long row, long col, 
						short extend, Nlm_Boolean highlight, Nlm_Boolean select)
{
	if (row>=fMaxRows || row<0 || col>=fMaxCols || col<0) return;

	if (highlight) InvertSelection(); // hide old
	if (fCanSelectRow) fSelectedRow= row; 
	if (fCanSelectCol) fSelectedCol= col;
		 	
	if (extend == kExtendMulti) {
		if (fSelrect.top == kNoSelection) fSelrect.top= row;
		else if (fSelrect.top > row) fSelrect.top= row;
		if (fSelrect.bottom <= row) fSelrect.bottom= row+1; 
	
		if (fSelrect.left == kNoSelection) fSelrect.left= col;
		else if (fSelrect.left > col) fSelrect.left= col;
		if (fSelrect.right <= col) fSelrect.right= col+1; 

		Nlm_LoadRectRgn(fWorkrgn, col, row, col+1, row+1); //?? +1
		if (select) Nlm_UnionRgn( fSelrgn, fWorkrgn, fSelrgn);
		else Nlm_DiffRgn( fSelrgn, fWorkrgn, fSelrgn); //?? Sect or Diff or ??
		}
		
	else if (extend == kExtendSingle) {
		if (fSelrect.top == kNoSelection) fSelrect.top= row;
		else if (fSelrect.top > row) fSelrect.top= row;
		if (fSelrect.bottom <= row) fSelrect.bottom= row+1; 
	
		if (fSelrect.left == kNoSelection) fSelrect.left= col;
		else if (fSelrect.left > col) fSelrect.left= col;
		if (fSelrect.right <= col) fSelrect.right= col+1; 

		//Nlm_ClearRgn(fSelrgn);
		//Nlm_LoadRectRgn(fSelrgn, fSelrect.left, fSelrect.top, fSelrect.right, fSelrect.bottom);
		}
		
	else if (select) {
		fSelrect.left= col;
		fSelrect.right= col+1; 
		fSelrect.top= row;
		fSelrect.bottom= row+1; 
		
		Nlm_ClearRgn(fSelrgn);
		//Nlm_LoadRectRgn(fSelrgn, fSelrect.left, fSelrect.top, fSelrect.right, fSelrect.bottom);
		}
	else
		SetEmptySelection(false);
			
	Boolean didscroll= fTable->ScrollIntoView( fSelrect);
	if (!didscroll && highlight) InvertSelection(); // show new
}

void DTabSelection::SelectCells( Nlm_RecT selrect,  
						short extend, Nlm_Boolean highlight, Nlm_Boolean select)
{
	if (selrect.top>= fMaxRows || selrect.bottom<0 
	 || selrect.left>=fMaxCols || selrect.right<0) return;
	selrect.left= Max(0,selrect.left);
	selrect.right= Min(fMaxCols, selrect.right);
	selrect.top= Max(0,selrect.top);
	selrect.bottom= Min(fMaxRows, selrect.bottom);
		
	if (highlight) InvertSelection();  
	if (fCanSelectRow) fSelectedRow= selrect.top; 
	if (fCanSelectCol) fSelectedCol= selrect.left;
	
	if (extend == kExtendMulti) {
		if (fSelrect.top == kNoSelection) fSelrect.top= selrect.top;
		if (fSelrect.bottom == kNoSelection) fSelrect.bottom= selrect.bottom;
		if (fSelrect.left == kNoSelection) fSelrect.left= selrect.left;
		if (fSelrect.right == kNoSelection) fSelrect.right= selrect.right;
		if (select) Nlm_UnionRect( &fSelrect, &selrect, &fSelrect);
		else Nlm_SectRect( &fSelrect, &selrect, &fSelrect);

		Nlm_LoadRectRgn(fWorkrgn, selrect.left, selrect.top, selrect.right, selrect.bottom);
		if (select) Nlm_UnionRgn( fSelrgn, fWorkrgn, fSelrgn);
		else Nlm_DiffRgn( fSelrgn, fWorkrgn, fSelrgn); // ?? Nlm_SectRgn or Nlm_DiffRgn
		}
		
	else if (extend == kExtendSingle) {
		if (fSelrect.top == kNoSelection) fSelrect.top= selrect.top;
		if (fSelrect.bottom == kNoSelection) fSelrect.bottom= selrect.bottom;
		if (fSelrect.left == kNoSelection) fSelrect.left= selrect.left;
		if (fSelrect.right == kNoSelection) fSelrect.right= selrect.right;
		if (select) Nlm_UnionRect( &fSelrect, &selrect, &fSelrect);
		else Nlm_SectRect( &fSelrect, &selrect, &fSelrect);

		//Nlm_LoadRectRgn(fSelrgn, fSelrect.left, fSelrect.top, fSelrect.right, fSelrect.bottom);
	  }
	else if (select) {
		fSelrect= selrect; // ?? add +1 to .right & .bottom ??
		Nlm_ClearRgn(fSelrgn);
		//Nlm_LoadRectRgn(fSelrgn, fSelrect.left, fSelrect.top, fSelrect.right, fSelrect.bottom);
		}
	else
		SetEmptySelection(false);
		
	Boolean	didscroll= fTable->ScrollIntoView( fSelrect);
	if (!didscroll && highlight) InvertSelection();  
}


void DTabSelection::SetEmptySelection(Boolean redraw)
{
	if (redraw) {
		//InvertSelection();
		InvalidateSelection();
		}
	fSelectedRow = kNoSelection;
	fSelectedCol = kNoSelection;
	//Nlm_SetRect
	Nlm_LoadRect( &fSelrect, kNoSelection, kNoSelection, kNoSelection, kNoSelection);
	Nlm_ClearRgn( fSelrgn);
}

void DTabSelection::InvalidateSelection()
{
	Nlm_RecT r, r2;
	if (!Nlm_EmptyRgn( fSelrgn)) {
		r2= fSelrect;
		fTable->GetCellRect( r2, r);
		fTable->InvalRect( r);
		}
	else if (!Nlm_EmptyRect(&fSelrect)) {
		r2= fSelrect;
		fTable->GetCellRect( r2, r);
		fTable->InvalRect( r);
		}
	else if (fSelectedRow>=0 && fSelectedCol>=0) {
		fTable->GetCellRect( fSelectedRow, fSelectedCol, r);
		fTable->InvalRect( r);
		}
	else if (fSelectedRow>=0) {
		fTable->GetRowRect( fSelectedRow, r);
		fTable->InvalRect( r);
		}
	else if (fSelectedCol>=0) {
		fTable->GetRowRect( fSelectedCol, r);
		fTable->InvalRect( r);
		}
}

Boolean DTabSelection::IsSelected()
{
	return (!Nlm_EmptyRect( &fSelrect));
	//return (fSelectedRow == kNoSelection && fSelectedCol == kNoSelection);
} 


Boolean DTabSelection::IsSelected(long row, long col)
{
	Nlm_PoinT pt;
	Boolean issel;
	pt.y= row; pt.x= col;
	if (!Nlm_EmptyRgn(fSelrgn)) issel= Nlm_PtInRgn( pt, fSelrgn);
	else issel= Nlm_PtInRect( pt, &fSelrect);
	return issel;
}  

Nlm_RecT DTabSelection::GetSelRect() const
{ 
	return fSelrect; 
}

long DTabSelection::GetSelectedRow()
{ 
	return fSelrect.top; //fSelectedRow; 
}

long DTabSelection::GetSelectedCol()  
{ 
	return fSelrect.left; //fSelectedCol; 
}

void DTabSelection::GetFirstSelectedCell( long& row, long& col)
{ 
#if 0
	row= fSelectedRow; 
	col= fSelectedCol; 
#else
	row= fSelrect.top;
	col= fSelrect.left;
#endif
}

void DTabSelection::GetLastSelectedCell( long& row, long& col)
{ 
#if 0
	row= fSelectedRow; 
	col= fSelectedCol; 
#else
	row= fSelrect.bottom;
	col= fSelrect.right;
#endif
}
	
void DTabSelection::InvertSelection()
{	
	InvertSelection(NULL);
}

void DTabSelection::InvertSelection(Nlm_RegioN updateRgn)
{	
	Nlm_RecT r, r2;

	if (!Nlm_EmptyRgn( fSelrgn)) {
#if 0	
				// this takes much much tooooooo lonnnnggggg to draw
		Nlm_ClearRgn( fWorkrgn);
		Nlm_UnionRgn( fWorkrgn, fSelrgn, fWorkrgn);
		Nlm_RegioN pixrgn= Nlm_CreateRgn();
		fTable->GetPixRgn( fSelrect, fWorkrgn, pixrgn);
		if (updateRgn) Nlm_SectRgn( pixrgn, updateRgn, pixrgn);
		fTable->InvertRgn( pixrgn);
  	pixrgn= Nlm_DestroyRgn( pixrgn);
#endif
	  return;
		}
	else if (!Nlm_EmptyRect(&fSelrect)) {
		r2= fSelrect;
		//r2.bottom--;
		//r2.right--;
		fTable->GetCellRect( r2, r);
		}
	else if (fSelectedRow>=0 && fSelectedCol>=0) 
		fTable->GetCellRect( fSelectedRow, fSelectedCol, r);
	else if (fSelectedRow>=0) 
		fTable->GetRowRect( fSelectedRow, r);
	else if (fSelectedCol>=0) 
		fTable->GetRowRect( fSelectedCol, r);
  else
	  return;
	fTable->InvertRect( r);
}



