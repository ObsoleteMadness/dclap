// DTabSelect.h
// d.g.gilbert

#ifndef _DTABSELECT_
#define _DTABSELECT_

#include "DPanel.h"
#include "DCommand.h"
#include "DTracker.h"
#include "DMethods.h"


class DTableView;

class DTabSelection
{
public:
		enum { kNoSelection = -1, 
				kDontExtend = 0, kExtendSingle = 1, kExtendMulti = 2, 
				kExtend = kExtendSingle, 
				kHighlight= true, kSelect= true };
		
		DTabSelection(DTableView* itsTable, long nrows, long ncols);
		virtual ~DTabSelection();
		
		virtual void SetCanSelect( Boolean canrow, Boolean cancol);
		virtual void SetTableSize( long rows, long cols);
		virtual void ChangeRowSize( long atrow, long deltarows);
		virtual void ChangeColSize( long atcol, long deltacols);
			
		virtual long GetSelectedRow();
		virtual long GetSelectedCol();
		virtual void GetFirstSelectedCell( long& row, long& col);
		virtual void GetLastSelectedCell( long& row, long& col);
		virtual Nlm_RecT GetSelRect() const;

		virtual void SelectCells( long row, long col, 
				short extend = kDontExtend, Nlm_Boolean highlight = true, Nlm_Boolean select = true);
		virtual void SelectCells( Nlm_RecT selrect,  
				short extend = kDontExtend, Nlm_Boolean highlight = true, Nlm_Boolean select = true);
		virtual void SetEmptySelection(Boolean redraw = true);
		virtual void InvalidateSelection();
		virtual Nlm_Boolean IsSelected();
		virtual Nlm_Boolean IsSelected(long row, long col);
		virtual void InvertSelection();
		virtual void InvertSelection(Nlm_RegioN updateRgn);
				
protected:
		Boolean fCanSelectRow, fCanSelectCol;
		DTableView * fTable;

		long		fMaxRows, fMaxCols;
		long		fSelectedRow, fSelectedCol;
		Nlm_RecT	fSelrect;
		Nlm_RegioN	fSelrgn, fWorkrgn;
};




class DTabSelector : public DTracker 
{
public:
	enum { cTabSelCmd = 2421 };
	Nlm_RecT	fOldSelection, fNewSelection; //Nlm_RgN later ...
	short 	fDoExtend;
	
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




#endif
