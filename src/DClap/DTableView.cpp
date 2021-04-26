// DTableView.cp
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
	fDoExtend= gKeys->shift();
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
				fDoExtend, DTableView::kHighlight, DTableView::kSelect);
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
		// +1 is messy fix to stop left/top -1 creap !
	pt1.x= MIN( fAnchorPoint.x+1, fNextPoint.x);
	pt2.x= MAX( fAnchorPoint.x-1, fNextPoint.x);
	pt1.y= MIN( fAnchorPoint.y+1, fNextPoint.y);
	pt2.y= MAX( fAnchorPoint.y-1, fNextPoint.y);

	((DTableView*)fView)->PointToCell( pt1, selr.top, selr.left);
	((DTableView*)fView)->PointToCell( pt2, selr.bottom, selr.right);
	selr.bottom++;
	selr.right++;
	fNewSelection= selr;
	if (!fDoExtend) fDoExtend= gKeys->shift(); // give'em another chance to shift
	
	DoItWork();
}

void DTabSelector::Undo()  
{
	DCommand::Undo();
	Boolean saveext= fDoExtend;
	fDoExtend= false;
	UndoWork();
	fDoExtend= saveext;
}

void DTabSelector::Redo()  
{
	DCommand::Redo();
	UndoWork();
}







//class DTableView : public DPanel


DTableView::DTableView(long id, DView* itsSuperior, short pixwidth, short pixheight, 
												short nrows, short ncols, short itemwidth, short itemheight,
												Boolean hasVscroll, Boolean hasHscroll):
		DAutoPanel( id, itsSuperior, pixwidth, pixheight, hasVscroll, hasHscroll),
		fItemWidth(itemwidth), fItemHeight(itemheight),
		fTop(0), fLeft(0), fColsDrawn(0), fMaxRows(nrows), fMaxCols(ncols), 
		fSelectedRow(kNoSelection), fSelectedCol(kNoSelection),
		fCanSelectRow(true), fCanSelectCol(false),fIsPrinting(false),
		fWidths(NULL), fHeights(NULL),
		fFont(Nlm_programFont)
{
	Nlm_BaR sb;
	sb = Nlm_GetSlateVScrollBar((Nlm_SlatE) GetNlmObject());
  fHasVbar= (sb != NULL);
  sb = Nlm_GetSlateHScrollBar((Nlm_SlatE) GetNlmObject());
  fHasHbar= (sb != NULL);
  FindLocation();
  SetTableSize( fMaxRows, fMaxCols);
	SetEmptySelection(false); 
	
	fTabSelector= new DTabSelector( this);
	//fTabSelector->ITabSelector( this);
	fCurrentTracker= fTabSelector;
}
		
DTableView::~DTableView()
{
	if (fWidths) MemFree(fWidths);
	if (fHeights) MemFree(fHeights);
	//if (fTabSelector) delete fTabSelector; //<< causing Motif crashes !?
}

void DTableView::SetCanSelect(Boolean canrow, Boolean cancol)
{
	fCanSelectRow= canrow;
	fCanSelectCol= cancol;
}


static Boolean didscroll;

void DTableView::SelectCells( short row, short col, 
						Nlm_Boolean extend, Nlm_Boolean highlight, Nlm_Boolean select)
{
	if (row>=fMaxRows || row<0 || col>=fMaxCols || col<0) return;

	if (highlight) InvertSelection(); // hide old
	if (fCanSelectRow) fSelectedRow= row; 
	if (fCanSelectCol) fSelectedCol= col;
		 
	if (!select) ; // deselect cells !!
	
	if (extend) {
		if (fSelrect.top == kNoSelection) fSelrect.top= row;
		else if (fSelrect.top > row) fSelrect.top= row;
		if (fSelrect.bottom <= row) fSelrect.bottom= row+1; 
	
		if (fSelrect.left == kNoSelection) fSelrect.left= col;
		else if (fSelrect.left > col) fSelrect.left= col;
		if (fSelrect.right <= col) fSelrect.right= col+1; 
		}
	else {
		fSelrect.left= col;
		fSelrect.right= col+1; 
		fSelrect.top= row;
		fSelrect.bottom= row+1; 
		}
		
	didscroll= false;
	ScrollIntoView( fSelrect);
	if (!didscroll && highlight) InvertSelection(); // show new
}

void DTableView::SelectCells( Nlm_RecT selrect,  
						Nlm_Boolean extend, Nlm_Boolean highlight, Nlm_Boolean select)
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
	
	if (extend)  {
		if (fSelrect.top == kNoSelection) fSelrect.top= selrect.top;
		if (fSelrect.bottom == kNoSelection) fSelrect.bottom= selrect.bottom;
		if (fSelrect.left == kNoSelection) fSelrect.left= selrect.left;
		if (fSelrect.right == kNoSelection) fSelrect.right= selrect.right;
	  Nlm_UnionRect( &fSelrect, &selrect, &fSelrect);
	  }
	else   
		fSelrect= selrect; // ?? add +1 to .right & .bottom ??

	didscroll= false;
	ScrollIntoView( fSelrect);
	if (!didscroll && highlight) InvertSelection();  
}


void DTableView::ScrollIntoView(Nlm_RecT itemr)
{
	enum { horizontal= false, vertical= true };
	short	lastitem;
	long	atpix;
	Nlm_BaR sb;

	 // fItemWidth << this isn't accurate for variable width/height.
	if (fWidths) {
		for (lastitem= fLeft, atpix= 0; lastitem< fMaxCols; lastitem++) { 
			atpix += fWidths[lastitem]; 
			if (atpix > fRect.right) break;
			}
		}
	else
		lastitem= fLeft + (fRect.right - fRect.left)/fItemWidth;
	 
	if (itemr.left > lastitem || itemr.left < fLeft) {
	 	didscroll= true;
		sb = Nlm_GetSlateHScrollBar((Nlm_SlatE) GetNlmObject());
		if (sb) { 
			//Nlm_ResetClip(); // ! TEST: are we losing scrollbar update due to cliprect?
			Nlm_SetValue(sb, Max(0, itemr.left - 1));  // calls our scroll funct
			}
		}
	
	if (fHeights) {
		for (lastitem= fTop, atpix= 0; lastitem< fMaxRows; lastitem++) { 
			atpix += fHeights[lastitem]; 
			if (atpix > fRect.bottom) break;
			}
		}
	else
		lastitem= fTop + (fRect.bottom - fRect.top)/fItemHeight;
		
	if (itemr.top > lastitem || itemr.top < fTop) {
	 	didscroll= true;
		sb = Nlm_GetSlateVScrollBar((Nlm_SlatE) GetNlmObject());
		if (sb) Nlm_SetValue(sb,  Max(0, itemr.top - 1));  
		}
}


void DTableView::SetEmptySelection(Boolean redraw)
{
	if (redraw) {
		//InvertSelection();
		InvalidateSelection();
		}
	fSelectedRow = kNoSelection;
	fSelectedCol = kNoSelection;
	//Nlm_SetRect
	Nlm_LoadRect( &fSelrect, kNoSelection, kNoSelection, kNoSelection, kNoSelection);
}

void DTableView::InvalidateSelection()
{
	Nlm_RecT r, r2;
	if (!Nlm_EmptyRect(&fSelrect)) {
		r2= fSelrect;
		GetCellRect( r2, r);
		InvalRect( r);
		}
	else if (fSelectedRow>=0 && fSelectedCol>=0) {
		GetCellRect( fSelectedRow, fSelectedCol, r);
		InvalRect( r);
		}
	else if (fSelectedRow>=0) {
		GetRowRect( fSelectedRow, r);
		InvalRect( r);
		}
	else if (fSelectedCol>=0) {
		GetRowRect( fSelectedCol, r);
		InvalRect( r);
		}
}


Boolean DTableView::IsSelected()
{
	return (!Nlm_EmptyRect( &fSelrect));
	//return (fSelectedRow == kNoSelection && fSelectedCol == kNoSelection);
} 


Boolean DTableView::IsSelected(short row, short col)
{
	Nlm_PoinT pt;
	pt.y= row; pt.x= col;
	Boolean issel= Nlm_PtInRect( pt, &fSelrect);
	return issel;
}  




void DTableView::SetTableFont( Nlm_FonT itsFont)
{
	fFont= itsFont;
}

void DTableView::SetTableSize( short rows, short cols)
{
	short i;
	
	if (fWidths && cols!=fMaxCols) {
		fWidths= (short*)MemMore( fWidths, (cols+1) * sizeof(short));
		for (i= fMaxCols; i<=cols; i++) fWidths[i]= fItemWidth;
		}
		
	if (fHeights && rows!=fMaxRows) {
		fHeights= (short*)MemMore( fHeights, (rows+1) * sizeof(short));
		for (i= fMaxRows; i<=rows; i++) fHeights[i]= fItemHeight;
		}
	
	fMaxRows= rows; 
	fMaxCols= cols;

	SetScrollPage();
}

void DTableView::ChangeRowSize( short atrow, short deltarows)
{
	Boolean needupdate;
	short newrows= Max(0, fMaxRows + deltarows);
#if 1
	Nlm_RecT  r;
	ViewRect(fRect); 
	Nlm_InsetRect( &fRect, 1, 1);
	GetRowRect( atrow, r, (fMaxRows - atrow));
	needupdate= Nlm_SectRect( &r, &fRect, &r);
	SetTableSize( newrows, fMaxCols);
	if (needupdate) this->InvalRect( r);
#else
	short viewrows=  (fRect.bottom - fRect.top) / fItemHeight;
	needupdate= (atrow >= fTop && atrow <= viewrows);
		// ^^ needs work
	SetTableSize( newrows, fMaxCols);
	if (needupdate) {
		//this->Invalidate(); // locate just update rect??	
		Nlm_RecT r;
		short nrows= viewrows - atrow;
		GetRowRect( atrow, r, nrows);
		this->InvalRect( r);
		}
#endif
}

void DTableView::ChangeColSize( short atcol, short deltacols)
{
	short newcols= Max(0, fMaxCols + deltacols);
	Boolean needupdate= (atcol >= fLeft && atcol <= (fRect.right - fRect.left) / fItemWidth);
		// ^^ needs work
	SetTableSize( fMaxRows, newcols);
	if (needupdate) {
		//this->Invalidate(); // locate just update rect??	
		Nlm_RecT r;
		short ncols= ((fRect.right - fRect.left) / fItemWidth) - atcol;
		GetColRect( atcol, r, ncols);
		this->InvalRect( r);
		}
}

		
void DTableView::SetItemWidth(short atcol, short ncols, short itemwidth)
{
	short i;
	if (ncols >= fMaxCols && !fWidths) {
		fItemWidth= itemwidth;
		}
	else {
		if (!fWidths) {
			// make fWidths
			fWidths= (short*)MemNew( (fMaxCols+1)*sizeof(short));
			for (i= 0; i<=fMaxCols; i++) fWidths[i]= fItemWidth;
			}
		short ncol= Min(fMaxCols, atcol+ncols);
		for (i= atcol; i<ncol; i++) fWidths[i]= itemwidth;
    fItemWidth= Max( fItemWidth, itemwidth);
		}
  SetScrollPage();
}

void DTableView::SetItemHeight(short atrow, short nrows, short itemheight)
{
	short i;
	if (nrows >= fMaxRows && !fHeights) {
		fItemHeight= itemheight;
		}
	else {
		if (!fHeights) {
			fHeights= (short*)MemNew( (fMaxRows+1)*sizeof(short));
			for (i= 0; i<=fMaxRows; i++) fHeights[i]= fItemHeight;
			}
		short nrow= Min(fMaxRows, atrow+nrows);
		for (i= atrow; i<nrow; i++) fHeights[i]= itemheight;
#if 0
    if (itemheight) fItemHeight= MIN( fItemHeight, itemheight);
#else
    fItemHeight= MAX( fItemHeight, itemheight);
#endif
		}
  SetScrollPage();
}
		
void DTableView::SetScrollPage()
{
	// set paging size of hor/vert sbars.. need to do on open & on resize
	Nlm_BaR sb;
	short pgDn, pg, maxn;
	if (fHasHbar) {
		pg= (fRect.right - fRect.left) / fItemWidth;
		pgDn= Max(1, pg - 1);
		maxn= Max( fMaxCols - pgDn, 1);
		sb = Nlm_GetSlateHScrollBar((Nlm_SlatE) GetNlmObject());
		if (sb) Nlm_SetRange(sb, pgDn, pgDn, maxn);
		}
	if (fHasVbar) {
		pg= (fRect.bottom - fRect.top) / fItemHeight;
		pgDn= Max(1, pg - 1);
		maxn= Max( fMaxRows - pgDn, 1);
		sb = Nlm_GetSlateVScrollBar((Nlm_SlatE) GetNlmObject());
		if (sb) Nlm_SetRange(sb, pgDn, pgDn, maxn);
		}
}

void DTableView::FindLocation()
{
	ViewRect(fRect); //this->GetPosition(fRect); // ?? this isn't changing except w/ resize ??
	Nlm_InsetRect( &fRect, 1, 1);
	SetScrollPage();
}
		
void DTableView::SizeToSuperview( DView* super, Boolean horiz, Boolean vert)
{
#ifndef WIN_MSWIN
	DView::SizeToSuperview(super, horiz, vert);
#else
	Nlm_RecT r, myr;
	super->ViewRect(r);
	ViewRect(myr);
			// these sizes draw off by -1... thus the +1
	if (horiz) {
		myr.left = 0;
		myr.right= myr.left + (r.right - r.left) +1;
#ifdef WIN_MSWIN
		if (fHasVbar) myr.right -= (Nlm_vScrollBarWidth - 8);
#endif
		}
	if (vert) {
		myr.top = 0;
		myr.bottom = myr.top + (r.bottom - r.top) +1;
#ifdef WIN_MSWIN
		if (fHasHbar) myr.bottom -= (Nlm_hScrollBarHeight + 10);
#endif
		}
	SetPosition( myr);
#endif
	this->FindLocation(); //?? added 14Mar94, gopher tables were doing this
}


void DTableView::Resize(DView* superview, Nlm_PoinT sizechange)
{
		// !! need this fRect change to deal w/ Resize() update event, 
		// which isn't put in a nice event queue, but is done immediately !!
	fRect.right  += sizechange.x;
	fRect.bottom += sizechange.y;
	DPanel::Resize(superview, sizechange);
	FindLocation();
}

void DTableView::Scroll(Boolean vertical, DView* scrollee, short newval, short oldval)
{
	Nlm_RecT	r = fRect;
	short delta, i;
	short diff= newval-oldval;
	this->Select(); // need for motif !
	if (vertical) {

	  if (fHeights) {
	    delta= 0;
			if (oldval>newval) {
				for (i=oldval-1; i>=newval; i--) delta -= fHeights[i];
				}
	    else {
				for (i=oldval; i<newval; i++) delta += fHeights[i];
				}
			}
		else
			delta= diff * fItemHeight;

		fTop += diff;
		Nlm_ScrollRect (&r, 0, -delta);
		if (diff<0) r.bottom= r.top - delta + Min(10,-delta/2);
		else r.top= r.bottom - delta - Min(10,delta/2);
		}
		
	else {
#if 1
	  if (fWidths) {
	    delta= 0;
			if (oldval>newval) {
				for (i=oldval-1; i>=newval; i--) delta -= fWidths[i];
				}
	    else {
				for (i=oldval; i<newval; i++) delta += fWidths[i];
				}
			}
		else
			delta= diff * fItemWidth;
			
		fLeft += diff;
		Nlm_ScrollRect (&r, -delta, 0);
		if (diff<0) r.right= Min(r.right, r.left - delta + Min(20,-delta/2));
		else r.left= Max(r.left, r.right - delta - Min(20,delta/2));
#else
		if (fWidths) {
			// fix later, for now redraw all w/o bitscroll
			fLeft += diff;
			}
		else {
			fLeft += diff;
			delta= diff * fItemWidth;
			Nlm_ScrollRect (&r, -delta, 0);
			if (diff<0) r.right= r.left - delta + Min(10,-delta/2);
			else r.left= r.right - delta - Min(10,delta/2);
			}
#endif
		}
	this->InvalRect( r);
  Nlm_Update(); //!? need since took out vibwnds autoupdate for RestorePort
}
			

void DTableView::GetRowRect( short row, Nlm_RecT& r, short nrows)
{
	short i;
	r= fRect;
	if (fHeights) {
		for (i= fTop; i<row; i++) r.top += fHeights[i];
		r.bottom= r.top;
		for (i= 0; i<nrows; i++) r.bottom += fHeights[row+i];
		}
	else {
		r.top += (row-fTop) * fItemHeight;
		r.bottom = r.top + nrows*fItemHeight;
		}
}

void DTableView::GetColRect( short col, Nlm_RecT& r, short ncols)
{
	short i;
	r= fRect;
	if (fWidths) {
		for (i= fLeft; i<col; i++) r.left += fWidths[i];
		r.right= r.left;
		for (i= 0; i<ncols; i++) r.right += fWidths[col+i];
		}
	else {
		r.left += (col-fLeft) * fItemWidth;
		r.right = r.left + ncols*fItemWidth;
		}
}

void DTableView::GetCellRect( short row, short col, Nlm_RecT& r)
{
	short i;
	r= fRect;
	if (fWidths) {
		for (i= fLeft; i<col; i++) r.left += fWidths[i];
		r.right= r.left + fWidths[col];
		}
	else {
		r.left += (col-fLeft) * fItemWidth;
		r.right = r.left + fItemWidth;
		}
	if (fHeights) {
		for (i= fTop; i<row; i++) r.top += fHeights[i];
		r.bottom= r.top + fHeights[row];
		}
	else {
		r.top += (row-fTop) * fItemHeight;
		r.bottom = r.top + fItemHeight;
		}
}

void DTableView::GetCellRect(Nlm_RecT cellr, Nlm_RecT& r)
{
	short i;
	r= fRect;
	if (fWidths) {
		for (i= fLeft; i<cellr.left; i++) r.left += fWidths[i];
		r.right= r.left;
		for (i=cellr.left; i<cellr.right; i++) r.right += fWidths[i];
		}
	else {
		r.left += (cellr.left-fLeft) * fItemWidth;
		r.right = r.left + (cellr.right - cellr.left) * fItemWidth;
		}
		
	if (fHeights) {
		for (i= fTop; i<cellr.top; i++) r.top += fHeights[i];
		r.bottom= r.top;
		for (i=cellr.top; i<cellr.bottom; i++) r.bottom += fHeights[i];
		}
	else {
		r.top += (cellr.top-fTop) * fItemHeight;
		r.bottom = r.top + (cellr.bottom - cellr.top) * fItemHeight;
		}
}

	
short DTableView::GetSelectedRow()
{ 
	return fSelectedRow; 
}

short DTableView::GetSelectedCol() 
{ 
	return fSelectedCol; 
}

void DTableView::GetFirstSelectedCell( short& row, short& col)
{ 
#if 0
	row= fSelectedRow; 
	col= fSelectedCol; 
#else
	row= fSelrect.top;
	col= fSelrect.left;
#endif
}

void DTableView::GetLastSelectedCell( short& row, short& col)
{ 
#if 0
	row= fSelectedRow; 
	col= fSelectedCol; 
#else
	row= fSelrect.bottom;
	col= fSelrect.right;
#endif
}
	


#ifdef WIN_MOTIF

#include <ncbi.h>
#include <ncbidraw.h>
//typedef unsigned long XID; // this is failing in X.h !?
//#define XID  unsigned long
#include <ncbiwin.h>
#include <ncbiport.h>

extern Display      *Nlm_currentXDisplay;
extern int          Nlm_currentXScreen;
extern Window       Nlm_currentXWindow;
extern GC           Nlm_currentXGC;
extern Nlm_Uint4    Nlm_XbackColor;
extern Nlm_Uint4    Nlm_XforeColor;
extern Nlm_Int2     Nlm_XOffset;
extern Nlm_Int2     Nlm_YOffset;
extern Nlm_RegioN   Nlm_clpRgn;

#endif


inline void DTableInvertRect( Nlm_RecT& r)
{	
#ifdef notWIN_MOTIF
// vals: GXcopy  GXand  GXor GXequiv GXinvert GXorInverted
// !! GXor looks GOOD for colored x -- makes gray rect
// but isn't reverting to regular on deselect, unless click is in selrect !?? 
// GXxor looks okay at start, but repeated calls mess it up (conflict??)
// fill vals: FillStippled  FillOpaqueStippled  FillSolid  FillTiled

#define newfunc  GXxor
#define currentFunction  GXcopy
#define newfill  FillStippled
#define currentFillStyle   FillStippled
  Nlm_RectTool  rtool;

#if 1
  if (Nlm_currentXDisplay && Nlm_currentXWindow && Nlm_currentXGC) {
    rtool.x = MIN (r.left, r.right);
    rtool.y = MIN (r.top, r.bottom);
    rtool.width = ABS (r.right - r.left);
    rtool.height = ABS (r.bottom - r.top);

		// ! recommended by Xlib vol 1, sect. 7.4.3: foreground( forecol ^ backcol)
		// ! and it works !
		ulong xorcolor= Nlm_XforeColor ^ Nlm_XbackColor;
		XSetForeground( Nlm_currentXDisplay,Nlm_currentXGC, xorcolor);
    XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, newfunc);
    //XSetFillStyle (Nlm_currentXDisplay, Nlm_currentXGC, newfill);
    XFillRectangle (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                    rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                    rtool.width, rtool.height);
    XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, currentFunction);
    //XSetFillStyle (Nlm_currentXDisplay, Nlm_currentXGC, currentFillStyle);
		XSetForeground( Nlm_currentXDisplay,Nlm_currentXGC, Nlm_XforeColor);
   }

#else
	Nlm_InvertMode();
	Nlm_PaintRect( &r);
	Nlm_CopyMode();
#endif

#else
	// patched both Nlm_InvertMode & Nlm_InvertRect for Motif
  Nlm_InvertRect( &r);
#endif 
}

void DTableView::InvertRect( Nlm_RecT& r)
{	
	Nlm_WindoW w= Nlm_SavePort( fNlmObject);
#ifdef WIN_MOTIF
	this->Select();
#endif
	::DTableInvertRect( r);
	if (w) Nlm_UseWindow (w); 
}

void DTableView::InvertSelection()
{	
	Nlm_RecT r, r2;
	if (!Nlm_EmptyRect(&fSelrect)) {
		r2= fSelrect;
		//r2.bottom--;
		//r2.right--;
		GetCellRect( r2, r);
		}
	else if (fSelectedRow>=0 && fSelectedCol>=0) 
		GetCellRect( fSelectedRow, fSelectedCol, r);
	else if (fSelectedRow>=0) 
		GetRowRect( fSelectedRow, r);
	else if (fSelectedCol>=0) 
		GetRowRect( fSelectedCol, r);
  else
	  return;
	InvertRect( r);
}



void DTableView::DoubleClickAt(short row, short col)
{
}


void DTableView::SingleClickAt(short row, short col)
{
	Nlm_PoinT pt;
	pt.x= col; pt.y= row;
	if (Nlm_PtInRect( pt, &fSelrect) && !gKeys->shift()) {
		SetEmptySelection( true);
		}
	else {
		SelectCells(  row, col, gKeys->shift(), kHighlight, kSelect);
		}
}
			

void DTableView::PointToCell(Nlm_PoinT mouse, short& row, short& col)
{
	if (fHeights) {
		row= fTop - 1;
		short yrow= fRect.top;
		do {
			row++;
			if (row<=fMaxRows) yrow += fHeights[row];
			else yrow= mouse.y;
			} while (yrow < mouse.y);
		}
	else
		row= fTop  + (mouse.y - fRect.top)  / fItemHeight;

	if (fWidths) {
		col= fLeft - 1;
		short xcol= fRect.left;
		do {
			col++;
			if (col<=fMaxCols) xcol += fWidths[col];
			else xcol= mouse.x;
			} while (xcol < mouse.x);
		}
	else
		col= fLeft + (mouse.x - fRect.left) / fItemWidth;
}



void DTableView::Click(Nlm_PoinT mouse)
{
	short row, col;
	
	//if (gLastCommand) gLastCommand->Commit();	//?? prevent bombs from prev. cmds ?
	//if (fCurrentTracker) fCurrentTracker->Reset();
	SetTracker( fCurrentTracker); // ??  make new one each click?
	DAutoPanel::Click(mouse); 		// sets fMouseStillDown= false

	if (!fCurrentTracker) {
		// TrackMouse::trackEnd does this now, if click is tracked...
		PointToCell( mouse, row, col);
		if (row >=0 && row < fMaxRows && col >= 0 && col < fMaxCols) {
			if (Nlm_dblClick) DoubleClickAt( row, col);
			else SingleClickAt( row, col);
			}
		}
}

void DTableView::Drag(Nlm_PoinT mouse)
{
	DAutoPanel::Drag(mouse); // sets fMouseStillDown= true,... 
}

void DTableView::Release(Nlm_PoinT mouse)
{
	DAutoPanel::Release(mouse); // sets fMouseStillDown= false,... 
}


void DTableView::TrackMouse( short aTrackPhase,
					Nlm_PoinT& anchorPoint, Nlm_PoinT& previousPoint,
					Nlm_PoinT& nextPoint,	Nlm_Boolean mouseDidMove)
{

	switch (aTrackPhase) {
	
		case DTracker::trackBegin:
			if (gKeys->shift()) {  
				Nlm_PoinT pt;
				Nlm_RecT  pixr;
				short	diff1, diff2;
				
				GetCellRect( fSelrect, pixr);
				diff1= anchorPoint.x - pixr.left;
				diff2= anchorPoint.x - pixr.right;
				if (abs(diff1) > abs(diff2)) pt.x= pixr.left; 
				else pt.x= pixr.right;
				diff1= anchorPoint.y - pixr.top;
				diff2= anchorPoint.y - pixr.bottom;
				if (abs(diff1) > abs(diff2)) pt.y= pixr.top; 
				else pt.y= pixr.bottom;
				
				// tracker::Start makes 3 pts all same
				anchorPoint = pt;
				previousPoint = pt;
				nextPoint = pt;
				//fTracker->fMovedOnce= true; // force it for single-click extend?
				}
		 	break;
	
		case DTracker::trackContinue:
			break;
			
		case DTracker::trackEnd:
			{
			short row, col;
			Boolean moved= gKeys->shift() // extend
					|| abs(anchorPoint.x - nextPoint.x) > 2
					|| abs(anchorPoint.y - nextPoint.y) > 2;
			if (!moved) {
				PointToCell( nextPoint, row, col);
				if (row >=0 && row < fMaxRows && col >= 0 && col < fMaxCols) {
					if (Nlm_dblClick) DoubleClickAt( row, col);
					else SingleClickAt( row, col);
					}
				}
			}
			break;
			
		}
}

void DTableView::TrackFeedback( short aTrackPhase,
					const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
					const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn)
{
	if (mouseDidMove) {
		Nlm_RecT	pixr;

		pixr.left= MIN( anchorPoint.x, nextPoint.x);
		pixr.right= MAX( anchorPoint.x, nextPoint.x);
		pixr.top= MIN( anchorPoint.y, nextPoint.y);
		pixr.bottom= MAX( anchorPoint.y, nextPoint.y);
		
		Nlm_InsetRect( &pixr, 1,1); //??
		Nlm_InvertMode();
		Nlm_FrameRect( &pixr);
		Nlm_CopyMode();
		}
}




void DTableView::DrawCell(Nlm_RecT r, short row, short col)
{
	char	snum[80];
	sprintf(snum,"r%d,c%d", row, col);
	Nlm_DrawString( &r, snum, 'c', false);
}
	
void DTableView::DrawRow(Nlm_RecT r, short row)
{
	short col = fLeft; 
	if (fWidths && col <= fMaxCols) r.right= r.left + fWidths[col];
	else r.right= r.left + fItemWidth;
	while (r.left < fRect.right && col < fMaxCols) {
		if (Nlm_RectInRgn (&r, Nlm_updateRgn))  
			DrawCell( r, row, col);
		if (fWidths) {
			Nlm_OffsetRect( &r, fWidths[col], 0);
			r.right= r.left + fWidths[col+1];
			}
		else
			Nlm_OffsetRect( &r, fItemWidth, 0);
		col++;
		}
	fColsDrawn= col - fLeft;
}


void DTableView::GetPageCount(Nlm_RecT pagerect, short& rowpages, short& colpages)
{
	long left, top, row, col; 	
	rowpages= colpages= 1;
	
	for (col=0, left=0; col < fMaxCols; col++) {
		if (fWidths) left += fWidths[col];  
		else left += fItemWidth;
		if (left >= pagerect.right) { 
			colpages++; 
			left= 0; 
			}
		}
	for (row=0, top=0; row < fMaxRows; row++) {
		if (fHeights) top += fHeights[row];  
		else top += fItemHeight;
		if (top >= pagerect.bottom) { 
			rowpages++; 
			top= 0; 
			}
		}
}


void DTableView::Draw()
{
	Nlm_RecT	 	r2, r = fRect;
	Nlm_PoinT  	pt;
	short 			row = fTop; 
	Boolean			hasSel;
	
	if (fHeights) r.bottom= r.top + fHeights[row];
	else r.bottom= r.top + fItemHeight;
	Nlm_SelectFont(fFont); //?? here
	hasSel= !Nlm_EmptyRect(&fSelrect);
	if (hasSel) {
		pt.x= fSelrect.left;
		r2= fSelrect;
		GetCellRect( r2, r2);
		}
		
	while (r.top < fRect.bottom && row < fMaxRows) {
		if (Nlm_RectInRgn (&r, Nlm_updateRgn)) { 
			DrawRow( r, row);
 			pt.y= row;
 			if (hasSel && Nlm_PtInRect( pt, &fSelrect)) {
 				Nlm_RecT ir= r;
 				ir.left= r2.left;
 				ir.right= r2.right;
				InvertRect( ir);
 				}
  		}
		if (fHeights) {
			Nlm_OffsetRect( &r, 0, fHeights[row]);
			r.bottom= r.top + fHeights[row+1];
			}
		else
			Nlm_OffsetRect( &r, 0, fItemHeight);
		row++;
		}
}
		

enum { kWritingAsPICT = 9 };

void DTableView::ViewRect(Nlm_RecT& r) 
{ 
	if (fIsPrinting == kWritingAsPICT) {
		r= fRect; // fRect is set at start of AsPICT
		}
	else if (fIsPrinting) {
		r= fRect; //fRect is set at start of Print 
		// Nlm_PrintingRect(&r);
		}
	else
		DView::ViewRect(r);  
}
		
void DTableView::Print()  
{
#if (defined(WIN_MAC) || defined (WIN_MSWIN))
  Nlm_WindoW    w;
  Boolean   		goOn, newPage, widerThanPage;
  Nlm_RecT    	pager, r, saverect;
	short 				row = 0, rowpages, colpages; 
	long					saveleft;
	
	// problems: assumes each row < page height
	
  w = Nlm_StartPrinting();
  if (w) {
  	fIsPrinting= true;
		gCursor->watch();	
		saverect= fRect;
		Nlm_PrintingRect(&pager);
		fRect= pager;
		Nlm_SelectFont(fFont); 
		saveleft= fLeft; fLeft= 0; 
		//savetop= fTop; fTop= 0; 
		GetPageCount( pager, rowpages, colpages);
		
    newPage = true;
    goOn = row < fMaxRows;
    widerThanPage= colpages > 1; 
    
    // make update region large...
    if (Nlm_updateRgn != NULL)  
			Nlm_LoadRectRgn(Nlm_updateRgn, -32767, -32767, 32767, 32767);
    
    while (goOn) {
    
      if (newPage) {
        goOn = Nlm_StartPage();
        newPage = false;
	      r= pager;
	      
				if (fHeights) r.bottom= r.top + fHeights[row];
				else r.bottom= r.top + fItemHeight;
     	  }
      
      if (goOn) {
				fColsDrawn= fMaxCols; // in case DrawRow forgets to set this
				DrawRow( r, row);
				
				if (widerThanPage) {
				  fLeft += 	fColsDrawn;
					newPage = (fLeft < fMaxCols);
					if (newPage) goOn = Nlm_EndPage();	
					else fLeft= 0;
					}
				
				if (!newPage) {	
					if (fHeights) {
						Nlm_OffsetRect( &r, 0, fHeights[row]);
						r.bottom= r.top + fHeights[row+1];
						}
					else
						Nlm_OffsetRect( &r, 0, fItemHeight);
					newPage = (r.bottom > pager.bottom);
					if (newPage) goOn = Nlm_EndPage();	
					row++;
					if (goOn) goOn= (row < fMaxRows);
					}
					
				}
			}
			
 	 	if (!newPage) (void) Nlm_EndPage();
	  Nlm_EndPrinting(w);
		fIsPrinting= false;
		fRect= saverect;
		fLeft= saveleft;
		gCursor->arrow();
		}  	 
#else
	Nlm_Message(MSG_OK,"Printing is not yet defined for this window system");
#endif
}


#ifdef WIN_MAC
#undef Handle
#undef Rect
#include <QuickDraw.h>
#include <Memory.h>
#endif

#ifdef WIN_MSWIN
#undef FAR 
#undef NEAR 
#include <windows.h>
extern "C" HWND         Nlm_currentHWnd;
extern "C" HDC          Nlm_currentHDC;

struct METAFILEHEADER { // must be 22 bytes !
	DWORD	key;
	WORD	hmf; // HANDLE hmf; for Win16 !
	//RECT	bbox; for Win16 !
	WORD  boxleft;
	WORD  boxtop;
	WORD  boxright;
	WORD  boxbottom;
	WORD	inch;
	DWORD	reserved;
	WORD	checksum;
};

	  
void CalcMFChecksum(METAFILEHEADER* pMFHead)
{
  WORD *p;

  for (p =(WORD *)pMFHead, pMFHead->checksum = 0;
	     p < (WORD *)&pMFHead->checksum; ++p)
         pMFHead->checksum ^= *p;
}

#endif

void DTableView::WriteToPICT(DFile* afile)  
{
	if (!afile) return;
	
#ifdef WIN_MAC
  Nlm_RecT    	pager, saverect, cellr;
	
	gCursor->watch();	
	saverect= fRect;
 	fIsPrinting= kWritingAsPICT;
 	Nlm_LoadRect( &cellr, 0, 0, fMaxCols, fMaxRows);
	this->GetCellRect( cellr, pager);
	fRect= pager;
	
  Rect  picrect;
  picrect.left= pager.left;
  picrect.right= pager.right;
  picrect.top = pager.top;
  picrect.bottom= pager.bottom;
	Handle pic= (Handle) OpenPicture( &picrect); 
	
  if (pic) { 
	  enum { kPicHeadsize = 512 };
	  ulong count;
    	// make update region large...
    if (Nlm_updateRgn != NULL)  
			Nlm_LoadRectRgn(Nlm_updateRgn, -32767, -32767, 32767, 32767);
    Draw();    			
	  ClosePicture();
	  
		  // now write pic to file
	  afile->Delete();
	  afile->Create("PICT","ttxt"); 
		afile->Open("wb"); // Damn "wb" -- must by BINARY 
	  count= kPicHeadsize;
	  char * header = (char *) Nlm_MemGet(kPicHeadsize,MGET_CLEAR);
	  afile->WriteData( header, count);
	  Nlm_MemFree( header);
		HLock(pic);
	  count= GetHandleSize(pic);
	  afile->WriteData( *pic, count);
	  afile->Close();
	  HUnlock(pic);
	  KillPicture ( (PicHandle)pic);
		} 
	fIsPrinting= false;		
	fRect= saverect;
	gCursor->arrow();
	return;
#endif

#ifdef WIN_MSWIN
  Nlm_RecT    	pager, saverect, cellr;
  RECT  picrect;
	
	gCursor->watch();	
	saverect= fRect;
 	fIsPrinting= kWritingAsPICT;
 	Nlm_LoadRect( &cellr, 0, 0, fMaxCols, fMaxRows);
	this->GetCellRect( cellr, pager);
	fRect= pager;
  picrect.left= pager.left;
  picrect.top = pager.top;
  picrect.right = pager.right;
  picrect.bottom= pager.bottom;
	
	char* fname= (char*) afile->GetName();
	fname= (char*)DFileManager::FilenameFromPath(fname); // CreateMF fails for full path !?
	//fname= "test.wmf";
	
#if 1
//#ifdef WIN16
	HDC hdcMeta= CreateMetaFile(fname); // (fname)
	
#define MetaHandle HMETAFILE
#define MetaClose(x)  CloseMetaFile(x)
#define MetaDelete(x) DeleteMetaFile(x)
#define MetaGetData(a,b,c)  GetMetaFileBitsEx(a,b,c)

#else
  char  describes[120];
  char  titlebuf[80];
  char  *dp, *np, *ep;
  
  np= (char*)gApplication->Shortname();
  dp= describes;
  ep= dp + sizeof(describes) - 2;
  if (np) while (*np && dp < ep) { *dp++ = *np++; }
  *dp++ = 0;
  np= this->GetTitle(titlebuf, sizeof(titlebuf));
  if (np) while (*np && dp < ep) { *dp++ = *np++; }
  *dp++ = 0;

  // need pixel to .01 mm conversion for picrect !
  // need some needlessly complex SetMapMode, MapPoint, ResetMapMode...
  
	HDC hdcMeta= CreateEnhMetaFile( Nlm_currentHDC, NULL, &picrect, describes);
#define MetaHandle HENHMETAFILE
#define MetaClose(x)  CloseEnhMetaFile(x)
#define MetaDelete(x) DeleteEnhMetaFile(x)
#define MetaGetData(a,b,c)  GetEnhMetaFileBits(a,b,c)
#endif

  if (hdcMeta) { 
  	POINT point;
  	SIZE  size;

   	HDC savehdc= Nlm_currentHDC;
  	Nlm_currentHDC= hdcMeta;  
  	SetMapMode( hdcMeta, MM_ANISOTROPIC);
		SetWindowOrgEx(hdcMeta, 0,0, &point);
		SetWindowExtEx(hdcMeta, picrect.right-picrect.left, 
		                picrect.bottom-picrect.top, &size);

  	Nlm_ResetDrawingTools();
  	
    	// make update region large...
    if (Nlm_updateRgn != NULL)  
			Nlm_LoadRectRgn(Nlm_updateRgn, -32767, -32767, 32767, 32767);
    Draw();  
      
	  MetaHandle hmf = MetaClose( hdcMeta);

#ifdef WIN16
	  // win16 code
	  //HGLOBAL hgmf = GetMetaFileBits( hmf);
#else
	  // win32 code
	  ulong count, bufsize;
	  METAFILEHEADER placeheader;
	  bufsize= GetEnhMetaFileBits( hmf, 0, NULL);
	  char* buf= (char*) MemNew( bufsize + 1);
	  if ( GetEnhMetaFileBits( hmf, bufsize, (unsigned char*)buf) ) { 
  	  placeheader.key= 0x9AC6CDD7L;
  	  placeheader.hmf= 0;
  	  //c0 f4 90 f7 36 0b 66 08 is bbox rect that powerpnt made
  	  placeheader.boxtop = 0xf4c0; //picrect.top; //?? some neg.num?
  	  placeheader.boxleft= 0xf790; //picrect.left;  //?? some neg.num?
  	  placeheader.boxbottom= 0x0b36; //picrect.bottom;
  	  placeheader.boxright = 0x0866; //picrect.right;
  	  placeheader.inch= 576;
  	  placeheader.reserved= 0;
  	  placeheader.checksum= 0x5551;  
  	  CalcMFChecksum( &placeheader); // == 0x5551 ?
    
  	  afile->Delete();
  	  afile->Create("WMF","DCLAP"); 
  		afile->Open("wb"); 
  	  count= 22; //!! not -> sizeof(placeheader);
  	  afile->WriteData( &placeheader, count);
  	  count= bufsize;
  	  afile->WriteData( buf, count);  
  	  afile->Close();
  	  }
	  MemFree( buf);
#endif

	  MetaDelete( hmf); // just deletes handle, not disk file !
	  Nlm_currentHDC= savehdc;  
		}  	 
	fIsPrinting= false;		
	fRect= saverect;
	gCursor->arrow();
	return;
#endif
 	 
	Nlm_Message(MSG_OK,"DrawToPicture is not yet defined for this window system");
}

