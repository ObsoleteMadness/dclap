// DTableView.h
// d.g.gilbert

#ifndef _DTABLEVIEW_
#define _DTABLEVIEW_

#include "DPanel.h"
#include "DCommand.h"
#include "DTracker.h"
#include "DMethods.h"
#include "DTabSelect.h"


class DTableView : public DAutoPanel, public DPrintHandler
{
public:
		DTabSelector * fTabSelector; 
		DTracker * fCurrentTracker;
		DTabSelection * fSelection;
		
		//enum { kNoSelection = -1, kExtend = true, kHighlight = true, kSelect = true };

		DTableView(long id, DView* itsSuperior,  
					short pixwidth, short pixheight, 
					long nrows, long ncols,
					short itemwidth, short itemheight,
					Boolean hasVscroll = true, Boolean hasHscroll = true);
		virtual ~DTableView();
		
		virtual void SetTableFont( Nlm_FonT itsFont);
		virtual void SetTableSize( long rows, long cols);
		virtual void ChangeRowSize( long atrow, long deltarows);
		virtual void ChangeColSize( long atcol, long deltacols);
		
		virtual void SetItemWidth(long atcol, long ncols, short itemwidth);
		virtual void SetItemHeight(long atrow, long nrows, short itemheight);
		virtual void SetScrollPage();
		
		virtual void FindLocation();
		virtual void Resize(DView* superview, Nlm_PoinT sizechange);
		virtual void SizeToSuperview( DView* super, Boolean horiz, Boolean vert);
		virtual void ViewRect(Nlm_RecT& r); 

		virtual void Scroll(Boolean vertical, DView* scrollee, long newval, long oldval);
		virtual Nlm_Boolean ScrollIntoView( Nlm_RecT itemr);
			
		virtual void GetRowRect( long row, Nlm_RecT& r, long nrows = 1);
		virtual void GetColRect( long col, Nlm_RecT& r, long ncols = 1);
		virtual void GetCellRect( long row, long col, Nlm_RecT& r);
		virtual void GetCellRect( Nlm_RecT cellr, Nlm_RecT& r);
		virtual void PointToCell(Nlm_PoinT mouse, long& row, long& col);
			
		virtual void TrackFeedback( short aTrackPhase,
					const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
					const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn);
		virtual void TrackMouse( short aTrackPhase,
					Nlm_PoinT& anchorPoint, Nlm_PoinT& previousPoint,
					Nlm_PoinT& nextPoint,	Nlm_Boolean mouseDidMove);
		
		virtual void SingleClickAt(long row, long col);
		virtual void DoubleClickAt(long row, long col);
		virtual void Click(Nlm_PoinT mouse);
		virtual void Drag(Nlm_PoinT mouse);
		virtual void Release(Nlm_PoinT mouse);

		virtual void DrawCell(Nlm_RecT r, long row, long col);
		virtual void DrawRow(Nlm_RecT r, long row);
		virtual void Draw();
		virtual void InvertRect( Nlm_RecT& r);
		virtual void Print();
		virtual void WriteToPICT(DFile* afile); 
		virtual void GetPageCount(Nlm_RecT pagerect, long& rowpages, long& colpages);
		virtual long GetItemWidth(long atcol = 0);
		virtual long GetItemHeight(long atrow = 0);

		virtual void GetPixRgn( Nlm_RecT cellrect, Nlm_RegioN cellrgn, Nlm_RegioN pixrgn);
		virtual void InvertRgn( Nlm_RegioN pixrgn);
		
		Nlm_FonT GetFont() { return fFont; }
		void SelectFont() { Nlm_SelectFont(fFont); }
		long GetMaxRows() { return fMaxRows; }
		long GetMaxCols() { return fMaxCols; }
		Nlm_RecT GetRect() { return fRect; }
		long	GetLeft() { return fLeft; }
		long GetTop()	{ return fTop; }
		void SetLeft( long val) { fLeft = val; }
		void SetTop( long val)	{ fTop = val; }

		void SetCanSelect( Boolean canrow, Boolean cancol)
			{ fSelection->SetCanSelect(canrow, cancol); }
		long GetSelectedRow() 
			{ return fSelection->GetSelectedRow(); }
		long GetSelectedCol() 
			{ return fSelection->GetSelectedCol(); }
		Nlm_RecT GetSelRect() 
			{ return fSelection->GetSelRect(); }
		void GetFirstSelectedCell( long& row, long& col)
			{ fSelection->GetFirstSelectedCell(row, col); }
		void GetLastSelectedCell( long& row, long& col)
			{ fSelection->GetLastSelectedCell(row, col); }	
		void SelectCells( long row, long col, 
				short extend = DTabSelection::kDontExtend, Nlm_Boolean highlight = true, Nlm_Boolean select = true)
			{ fSelection->SelectCells(row, col, extend, highlight, select); }
		void SelectCells( Nlm_RecT selrect,  
				short extend = DTabSelection::kDontExtend, Nlm_Boolean highlight = true, Nlm_Boolean select = true)
			{ fSelection->SelectCells(selrect, extend, highlight, select); }
		void SetEmptySelection(Boolean redraw = true)
			{ fSelection->SetEmptySelection(redraw); }
		void InvalidateSelection()
			{ fSelection->InvalidateSelection(); }
		Boolean IsSelected()
			{ return fSelection->IsSelected(); }
		Boolean IsSelected(long row, long col)
			{ return fSelection->IsSelected(row, col); }
		void InvertSelection()
			{ fSelection->InvertSelection(); }
		
protected:
		short		*fWidths, *fHeights;
		long		fMaxRows, fMaxCols;
		short		fItemHeight, fItemWidth;
		long		fTop, fLeft, fColsDrawn;
		long		fHScrollScale, fVScrollScale;
		Boolean	fHasVbar, fHasHbar, fIsPrinting;
		Nlm_RecT	fRect;
		Nlm_FonT	fFont;
};


class DRCshifter : public DTracker 
{
public:
	enum { kDoCol= 0, kDoRow= 1 };
	Nlm_Boolean fRowCol;
	long 			fNewRC, fOldRC;
	DTableView	* fTable;
	
	DRCshifter() 
	{
		IRCshifter( 0, NULL, NULL, kDoRow, 0);
	}
		
	DRCshifter( long command, DTaskMaster* itsSource, DTableView* itsTable, 
							Nlm_Boolean RowOrCol, long oldRC) 
	{
	 	IRCshifter( command, itsSource, itsTable, RowOrCol, oldRC);
	}
	 	
	void IRCshifter( long command, DTaskMaster* itsSource, DTableView* itsTable, 
							Nlm_Boolean RowOrCol, long oldRC)
	{
	 	fTable= itsTable; fRowCol= RowOrCol; fNewRC= -1; fOldRC= oldRC;
	 	ITracker(command,  itsSource, "shift", kCanUndo, kCausesChange,
	 							itsTable);
	}
	 
};
#endif
