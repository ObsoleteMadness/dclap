// DTableView.h
// d.g.gilbert

#ifndef _DTABLEVIEW_
#define _DTABLEVIEW_

#include "DPanel.h"
#include "DCommand.h"
#include "DTracker.h"
#include "DMethods.h"


class DTabSelector;

class DTableView : public DAutoPanel, public DPrintHandler
{
public:
		enum { kNoSelection = -1, kExtend = true, kHighlight= true, kSelect= true };
		DTabSelector * fTabSelector; 
		DTracker * fCurrentTracker;
		
		DTableView(long id, DView* itsSuperior,  
					short pixwidth, short pixheight, 
					short nrows, short ncols,
					short itemwidth, short itemheight,
					Boolean hasVscroll = true, Boolean hasHscroll = true);
		virtual ~DTableView();
		
		virtual void SetCanSelect( Boolean canrow, Boolean cancol);
		virtual void SetTableFont( Nlm_FonT itsFont);
		virtual void SetTableSize( short rows, short cols);
		virtual void ChangeRowSize( short atrow, short deltarows);
		virtual void ChangeColSize( short atcol, short deltacols);
		
		virtual void SetItemWidth(short atcol, short ncols, short itemwidth);
		virtual void SetItemHeight(short atrow, short nrows, short itemheight);
		virtual void SetScrollPage();
		
		virtual void FindLocation();
		virtual void Resize(DView* superview, Nlm_PoinT sizechange);
		virtual void SizeToSuperview( DView* super, Boolean horiz, Boolean vert);
		virtual void ViewRect(Nlm_RecT& r); 

		virtual void Scroll(Boolean vertical, DView* scrollee, short newval, short oldval);
		virtual void ScrollIntoView( Nlm_RecT itemr);
			
		virtual void GetRowRect( short row, Nlm_RecT& r, short nrows = 1);
		virtual void GetColRect( short col, Nlm_RecT& r, short ncols = 1);
		virtual void GetCellRect( short row, short col, Nlm_RecT& r);
		virtual void GetCellRect( Nlm_RecT cellr, Nlm_RecT& r);
		virtual short GetSelectedRow();
		virtual short GetSelectedCol();
		virtual void GetFirstSelectedCell( short& row, short& col);
		virtual void GetLastSelectedCell( short& row, short& col);
		virtual void PointToCell(Nlm_PoinT mouse, short& row, short& col);
			
		virtual void SelectCells( short row, short col, 
				Nlm_Boolean extend = false, Nlm_Boolean highlight = true, Nlm_Boolean select = true);
		virtual void SelectCells( Nlm_RecT selrect,  
				Nlm_Boolean extend = false, Nlm_Boolean highlight = true, Nlm_Boolean select = true);
		virtual void SetEmptySelection(Boolean redraw = true);
		virtual void InvalidateSelection();
		virtual Boolean IsSelected();
		virtual Boolean IsSelected(short row, short col);
		virtual void InvertSelection();

		virtual void TrackFeedback( short aTrackPhase,
					const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
					const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn);
		virtual void TrackMouse( short aTrackPhase,
					Nlm_PoinT& anchorPoint, Nlm_PoinT& previousPoint,
					Nlm_PoinT& nextPoint,	Nlm_Boolean mouseDidMove);
		
		virtual void SingleClickAt(short row, short col);
		virtual void DoubleClickAt(short row, short col);
		virtual void Click(Nlm_PoinT mouse);
		virtual void Drag(Nlm_PoinT mouse);
		virtual void Release(Nlm_PoinT mouse);

		virtual void DrawCell(Nlm_RecT r, short row, short col);
		virtual void DrawRow(Nlm_RecT r, short row);
		virtual void Draw();
		virtual void InvertRect( Nlm_RecT& r);
		virtual void Print();
		virtual void WriteToPICT(DFile* afile); 
		virtual void GetPageCount(Nlm_RecT pagerect, short& rowpages, short& colpages);
		
		Nlm_FonT GetFont() { return fFont; }
		void SelectFont() { Nlm_SelectFont(fFont); }
		short GetMaxRows() { return fMaxRows; }
		short GetMaxCols() { return fMaxCols; }
		short GetItemWidth() { return fItemWidth; }
		short GetItemHeight() { return fItemHeight; }
		Nlm_RecT GetRect() { return fRect; }
		Nlm_RecT GetSelRect() { return fSelrect; }
		short	GetLeft() { return fLeft; }
		short GetTop()	{ return fTop; }
		void SetLeft( short val) { fLeft = val; }
		void SetTop( short val)	{ fTop = val; }
		
protected:
		short		*fWidths, *fHeights;
		short		fMaxRows, fMaxCols;
		short		fItemHeight, fItemWidth;
		short		fTop, fLeft, fSelectedRow, fSelectedCol, fColsDrawn;
		Boolean fCanSelectRow, fCanSelectCol;
		Boolean	fHasVbar, fHasHbar, fIsPrinting;
		Nlm_RecT	fRect;
		Nlm_RecT	fSelrect;
		Nlm_FonT	fFont;
};



class DTabSelector : public DTracker 
{
public:
	enum { cTabSelCmd = 2421 };
	Nlm_RecT	fOldSelection, fNewSelection; //Nlm_RgN later ...
	Boolean 	fDoExtend;
	
	DTabSelector( DTableView* itsTable);
	virtual ~DTabSelector();
	virtual void ITabSelector( DTableView* itsTable);
	virtual void Reset(); 
	virtual void DoIt(); 
	virtual void Undo();
	virtual void Redo(); 
	virtual void DoItWork(); 
	virtual void UndoWork(); 
};

class DRCshifter : public DTracker 
{
public:
	enum { kDoCol= 0, kDoRow= 1 };
	Nlm_Boolean fRowCol;
	short 			fNewRC, fOldRC;
	DTableView	* fTable;
	
	DRCshifter() 
	{
		IRCshifter( 0, NULL, NULL, kDoRow, 0);
	}
		
	DRCshifter( long command, DTaskMaster* itsSource, DTableView* itsTable, 
							Nlm_Boolean RowOrCol, short oldRC) 
	{
	 	IRCshifter( command, itsSource, itsTable, RowOrCol, oldRC);
	}
	 	
	void IRCshifter( long command, DTaskMaster* itsSource, DTableView* itsTable, 
							Nlm_Boolean RowOrCol, short oldRC)
	{
	 	fTable= itsTable; fRowCol= RowOrCol; fNewRC= -1; fOldRC= oldRC;
	 	ITracker(command,  itsSource, "shift", kCanUndo, kCausesChange,
	 							itsTable);
	}
	 
};


#endif
