// DTableView.cp
// d.g.gilbert

// this class is used a lot for various display/user interaction arrays of items
// still needs work for multiple selections, run arrays for height/width?
// 

#include "DTableView.h"
#include "DTabSelect.h"
#include <DPanel.h>
#include <DUtil.h>
#include <DApplication.h>
#include <Dvibrant.h>
#include <DTracker.h>
#include <ncbi.h>
#include <dgg.h>






//class DTableView : public DPanel


DTableView::DTableView(long id, DView* itsSuperior, short pixwidth, short pixheight, 
												long nrows, long ncols, short itemwidth, short itemheight,
												Boolean hasVscroll, Boolean hasHscroll):
		DAutoPanel( id, itsSuperior, pixwidth, pixheight, hasVscroll, hasHscroll),
		fItemWidth(itemwidth), fItemHeight(itemheight),
		fSelection(NULL), fHScrollScale(1), fVScrollScale(1),
		fTop(0), fLeft(0), fColsDrawn(0), fMaxRows(nrows), fMaxCols(ncols), 
		fIsPrinting(false), fWidths(NULL), fHeights(NULL),
		fFont(Nlm_programFont)
{
	Nlm_BaR sb;
	sb = Nlm_GetSlateVScrollBar((Nlm_SlatE) GetNlmObject());
  fHasVbar= (sb != NULL);
  sb = Nlm_GetSlateHScrollBar((Nlm_SlatE) GetNlmObject());
  fHasHbar= (sb != NULL);
	fSelection= new DTabSelection( this, fMaxRows, fMaxCols);
	
  FindLocation();
  SetTableSize( fMaxRows, fMaxCols);
	fTabSelector= new DTabSelector( this);
	//fTabSelector->ITabSelector( this);
	fCurrentTracker= fTabSelector;
}
		
DTableView::~DTableView()
{
	MemFree(fWidths);
	MemFree(fHeights);
	//if (fTabSelector) delete fTabSelector; //<< causing Motif crashes !?
}






Nlm_Boolean DTableView::ScrollIntoView(Nlm_RecT itemr)
{
	enum { horizontal= false, vertical= true };
	short	lastitem;
	long	atpix;
	Nlm_BaR sb;
	Boolean didscroll= false;
	
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
			long barval= itemr.left;
#if 1
			if (fHScrollScale > 1)  barval /= fHScrollScale;
#endif
			Nlm_SetValue(sb, Max(0, barval - 1));  // calls our scroll funct
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
		if (sb) {
			long barval= itemr.top;
#if 1
			if (fVScrollScale > 1)  barval /= fVScrollScale;
#endif
			Nlm_SetValue(sb,  Max(0, barval - 1));  
			}
		}
		
	return didscroll;
}




void DTableView::SetTableFont( Nlm_FonT itsFont)
{
	fFont= itsFont;
}

void DTableView::SetTableSize( long rows, long cols)
{
	long i;
	
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
	fSelection->SetTableSize( rows, cols);

	SetScrollPage();
}

void DTableView::ChangeRowSize( long atrow, long deltarows)
{
	Boolean needupdate;
	long newrows= Max(0, fMaxRows + deltarows);
	Nlm_RecT  r;
	ViewRect(fRect); 
	Nlm_InsetRect( &fRect, 1, 1);
	GetRowRect( atrow, r, (fMaxRows - atrow));
	needupdate= Nlm_SectRect( &r, &fRect, &r);
	//fSelection->ChangeRowSize( atrow, deltarows); // done thru SetTableSize
	SetTableSize( newrows, fMaxCols);
	if (needupdate) this->InvalRect( r);
}

void DTableView::ChangeColSize( long atcol, long deltacols)
{
	long newcols= Max(0, fMaxCols + deltacols);
	Boolean needupdate= (atcol >= fLeft && atcol <= (fRect.right - fRect.left) / fItemWidth);
		// ^^ needs work
	//fSelection->ChangeColSize( atcol, deltacols);// done thru SetTableSize
	SetTableSize( fMaxRows, newcols);
	if (needupdate) {
		//this->Invalidate(); // locate just update rect??	
		Nlm_RecT r;
		long ncols= ((fRect.right - fRect.left) / fItemWidth) - atcol;
		GetColRect( atcol, r, ncols);
		this->InvalRect( r);
		}
}

		
void DTableView::SetItemWidth(long atcol, long ncols, short itemwidth)
{
	long i;
	if (ncols >= fMaxCols && !fWidths) {
		fItemWidth= itemwidth;
		}
	else {
		if (!fWidths) {
			// make fWidths
			fWidths= (short*)MemNew( (fMaxCols+1)*sizeof(short));
			for (i= 0; i<=fMaxCols; i++) fWidths[i]= fItemWidth;
			}
		long ncol= Min(fMaxCols, atcol+ncols);
		for (i= atcol; i<ncol; i++) fWidths[i]= itemwidth;
    fItemWidth= Max( fItemWidth, itemwidth);
		}
  SetScrollPage();
}

void DTableView::SetItemHeight(long atrow, long nrows, short itemheight)
{
	long i;
	if (nrows >= fMaxRows && !fHeights) {
		fItemHeight= itemheight;
		}
	else {
		if (!fHeights) {
			fHeights= (short*)MemNew( (fMaxRows+1)*sizeof(short));
			for (i= 0; i<=fMaxRows; i++) fHeights[i]= fItemHeight;
			}
		long nrow= Min(fMaxRows, atrow+nrows);
		for (i= atrow; i<nrow; i++) fHeights[i]= itemheight;
#if 0
    if (itemheight) fItemHeight= MIN( fItemHeight, itemheight);
#else
    fItemHeight= MAX( fItemHeight, itemheight);
#endif
		}
  SetScrollPage();
}
		

long DTableView::GetItemWidth(long atcol)
{
	if (fWidths && atcol >= 0 && atcol < fMaxCols) 
		return fWidths[atcol];
	else 
		return fItemWidth;
}

long DTableView::GetItemHeight(long atrow)
{
	if (fHeights && atrow >= 0 && atrow < fMaxRows) 
		return fHeights[atrow];
	else 
		return fItemHeight;
}

		
void DTableView::SetScrollPage()
{
	// set paging size of hor/vert sbars.. need to do on open & on resize
	Nlm_BaR sb;
	long pgDn, pg, maxn;
	if (fHasHbar) {
		sb = Nlm_GetSlateHScrollBar((Nlm_SlatE) GetNlmObject());
		if (sb) {
			pg= (fRect.right - fRect.left) / fItemWidth;
			pgDn= Max(1, pg - 1);
			maxn= Max( fMaxCols - pgDn, 1);

#if 1
		// test fix for long int range !			
			if (fMaxCols > SHRT_MAX) {
				fHScrollScale= (fMaxCols + SHRT_MAX-1) / SHRT_MAX;
				pg /= fHScrollScale;
				pgDn /= fHScrollScale;
				maxn /= fHScrollScale;
				}
			else fHScrollScale= 1;
#endif
			Nlm_SetRange(sb, pgDn, pgDn, maxn);
			}
		}
		
	if (fHasVbar) {
		sb = Nlm_GetSlateVScrollBar((Nlm_SlatE) GetNlmObject());
		if (sb) {
			pg= (fRect.bottom - fRect.top) / fItemHeight;
			pgDn= Max(1, pg - 1);
			maxn= Max( fMaxRows - pgDn, 1);
#if 1
		// test fix for long int range !			
			if (fMaxRows > SHRT_MAX) {
				fVScrollScale= (fMaxRows + SHRT_MAX-1) / SHRT_MAX;
				pg /= fVScrollScale;
				pgDn /= fVScrollScale;
				maxn /= fVScrollScale;
				}
			else fVScrollScale= 1;
#endif
			Nlm_SetRange(sb, pgDn, pgDn, maxn);
			}
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

void DTableView::Scroll(Boolean vertical, DView* scrollee, long newval, long oldval)
{
	Nlm_RecT	r = fRect;
	long delta, i, diff;
	this->Select(); // need for motif !
	if (vertical) {
		if (fVScrollScale>1) { newval *= fVScrollScale; oldval *= fVScrollScale; }
		diff= newval-oldval;

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
		if (diff<0) r.bottom= r.top - delta; // + Min(10,-delta/2);
		else r.top= r.bottom - delta; // - Min(10,delta/2);
		}
		
	else {
		if (fHScrollScale>1) { newval *= fHScrollScale; oldval *= fHScrollScale; }
		diff= newval-oldval;

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
		if (diff<0) r.right= Min(r.right, r.left - delta); // + Min(fItemWidth,-delta/2));
		else r.left= Max(r.left, r.right - delta); // - Min(fItemWidth,delta/2));
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
			

void DTableView::GetRowRect( long row, Nlm_RecT& r, long nrows)
{
	long i;
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

void DTableView::GetColRect( long col, Nlm_RecT& r, long ncols)
{
	long i;
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

void DTableView::GetCellRect( long row, long col, Nlm_RecT& r)
{
	long i;
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
	long i;
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
extern Nlm_IntD     Nlm_XOffset;
extern Nlm_IntD     Nlm_YOffset;
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



void DTableView::GetPixRgn(Nlm_RecT cellrect, Nlm_RegioN cellrgn, Nlm_RegioN pixrgn)
{
	long i;
	Nlm_PoinT pt;
	Nlm_RecT  r;
	
	r= fRect;
	Nlm_RegioN ptrgn= Nlm_CreateRgn();
	if (fHeights) for (i= fTop; i<cellrect.top; i++) r.top += fHeights[i];
	else r.top += (cellrect.top-fTop) * fItemHeight;
	
	for (long irow= cellrect.top; irow<=cellrect.bottom; irow++) {
		r.left= fRect.left;
		if (fWidths) for (i= fLeft; i<cellrect.left; i++) r.left += fWidths[i];
		else r.left += (cellrect.left-fLeft) * fItemWidth;
		if (fHeights) r.bottom= r.top + fHeights[irow];
		else r.bottom = r.top + fItemHeight;
		
		for (long jcol= cellrect.left; jcol<=cellrect.right; jcol++) {
			pt.y= irow; pt.x= jcol;
			if (fWidths) r.right= r.left + fWidths[jcol];
			else r.right= r.left + fItemWidth;
			if (Nlm_PtInRgn( pt, cellrgn)) {
				Nlm_LoadRectRgn( ptrgn, r.left, r.top, r.right, r.bottom);
				Nlm_UnionRgn( pixrgn, ptrgn, pixrgn);
				}
			r.left= r.right;
			}
		r.top= r.bottom;
		}
	ptrgn= Nlm_DestroyRgn(ptrgn);
}


void DTableView::InvertRgn( Nlm_RegioN pixrgn)
{	
	Nlm_WindoW w= Nlm_SavePort( fNlmObject);
#ifdef WIN_MOTIF
	this->Select();
#endif
  Nlm_InvertRgn(pixrgn);
	if (w) Nlm_UseWindow (w); 
}





void DTableView::DoubleClickAt(long row, long col)
{
}


void DTableView::SingleClickAt(long row, long col)
{
	if ( fSelection->IsSelected(row, col) && !gKeys->shift()) 
		fSelection->SetEmptySelection( true);
	else {
		long ext;
		if (gKeys->shift()) ext= DTabSelection::kExtendSingle;
		else if (gKeys->command()) ext= DTabSelection::kExtendMulti;
		else ext= DTabSelection::kDontExtend;
		fSelection->SelectCells(  row, col, ext, 
					DTabSelection::kHighlight, DTabSelection::kSelect);
		}
}
			

void DTableView::PointToCell(Nlm_PoinT mouse, long& row, long& col)
{
	if (fHeights) {
		row= fTop - 1;
		long yrow= fRect.top;
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
		long xcol= fRect.left;
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
	long row, col;
	
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
				long	diff1, diff2;
				
				GetCellRect( fSelection->GetSelRect(), pixr);
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
			long row, col;
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




void DTableView::DrawCell(Nlm_RecT r, long row, long col)
{
	char	snum[80];
	sprintf(snum,"r%d,c%d", row, col);
	Nlm_DrawString( &r, snum, 'c', false);
}
	
Boolean gHasSel = false;

void DTableView::DrawRow(Nlm_RecT r, long row)
{
	long col = fLeft; 
	if (fWidths && col <= fMaxCols) r.right= r.left + fWidths[col];
	else r.right= r.left + fItemWidth;
	while (r.left < fRect.right && col < fMaxCols) {
		if (Nlm_RectInRgn (&r, Nlm_updateRgn))  {
			DrawCell( r, row, col);
#if 1
 			if (gHasSel && fSelection->IsSelected(row, col) ) 
 				InvertRect( r);
#endif
			}
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


void DTableView::GetPageCount(Nlm_RecT pagerect, long& rowpages, long& colpages)
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
	long 			row = fTop; 
	Boolean			hasSel;
	Nlm_RegioN saveUpdrgn = NULL;
	
	if (fHeights) r.bottom= r.top + fHeights[row];
	else r.bottom= r.top + fItemHeight;
	Nlm_SelectFont(fFont); //?? here
	hasSel= fSelection->IsSelected();
	gHasSel= hasSel;
	if (hasSel) {
#if 0
		saveUpdrgn= Nlm_CreateRgn();
		Nlm_UnionRgn(saveUpdrgn,Nlm_updateRgn, saveUpdrgn);
#endif
#if 0
		pt.x= fSelection->GetSelectedCol();
		r2= fSelection->GetSelRect();
		GetCellRect( r2, r2);
#endif		
		}
	while (r.top < fRect.bottom && row < fMaxRows) {
		if (Nlm_RectInRgn (&r, Nlm_updateRgn)) { 
			DrawRow( r, row);
 			pt.y= row;
#if 0
 			if (hasSel && fSelection->IsSelected(pt.y, pt.x) ) {
 				Nlm_RecT ir= r;
 				ir.left= r2.left;
 				ir.right= r2.right;
				InvertRect( ir);
 				}
#endif
  		}
		if (fHeights) {
			Nlm_OffsetRect( &r, 0, fHeights[row]);
			r.bottom= r.top + fHeights[row+1];
			}
		else
			Nlm_OffsetRect( &r, 0, fItemHeight);
		row++;
		}
#if 0
	if (hasSel) {
		fSelection->InvertSelection(saveUpdrgn); // ! only do redrawn section !!
		saveUpdrgn= Nlm_DestroyRgn(saveUpdrgn);
		}
#endif
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
	long  				row = 0, rowpages, colpages; 
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

