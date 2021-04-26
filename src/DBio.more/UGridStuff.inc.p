{ UGridStuff.inc.p  }
{ Copyright 1992 by d.g.gilbert, for SeqApp }

{$S UGridStuff }




{	TRCshifter -------------------------- }

CONST
		cRCShift	= 2020;
		
PROCEDURE TRCshifter.IRCshifter(itsDocument: TDocument; 
								itsView: TGridView; itsSibling: TGridView;
								RowOrCol: VHSelect; atRC: integer);
VAR
	vwr:			VRect;
	itsMouse: Point;
	itsVMouse: VPoint;
BEGIN
	GetMouse( itsMouse);  
	itsView.QDToViewPt( itsMouse, itsVMouse);
	ITracker(cRCShift, itsDocument, kCanUndo, kCausesChange{kDoesNotCauseChange} ,
					 itsDocument, itsView, itsView.GetScroller(FALSE), itsVMouse); 	 

	fConstrainsMouse := true;
	fGrid 	:= itsView;
	fSibling:= itsSibling;
	fNewRC := atRC;
	fOldRC := fNewRC;
	fRowCol:= RowOrCol;
	fGrid.RowToVRect( 1, fGrid.fNumOfRows, vwr);
	fBounds:= vwr;
END;



FUNCTION TRCshifter.TrackMouse(aTrackPhase: TrackPhase; VAR anchorPoint, previousPoint,
								 nextPoint: VPoint; mouseDidMove: BOOLEAN): TTracker; OVERRIDE;
VAR
	aCell: GridCell;
BEGIN
	IF aTrackPhase = trackRelease then BEGIN
		aCell	:= fGrid.VPointToLastCell(nextPoint);
		fNewRC:= aCell.vh[fRowCol];
		END;
	TrackMouse := SELF;
END;



PROCEDURE TRCshifter.DrawFeedback( anchorPoint, nextPoint: VPoint);
VAR
	qdr: Rect;
	vwr: VRect;
	aCell: GridCell;
BEGIN
	aCell	:= fGrid.VPointToLastCell(nextPoint);
	IF fRowCol = v then BEGIN
		fGrid.RowToVRect(aCell.v, 1, vwr);
		fGrid.ViewToQDRect( vwr, qdr);
		FrameRect( qdr);
		if fSibling<>NIL then begin
			if fSibling.Focus then ;
			fSibling.RowToVRect(aCell.v, 1, vwr);
			fSibling.ViewToQDRect( vwr, qdr);
			FrameRect( qdr);
			if fGrid.Focus then ;
			END;
		END

	ELSE BEGIN
		fGrid.ColToVRect(aCell.h, 1, vwr);
		fGrid.ViewToQDRect( vwr, qdr);
		FrameRect( qdr);
		if fSibling<>NIL then begin
			if fSibling.Focus then ;
			fSibling.ColToVRect(aCell.h, 1, vwr);
			fSibling.ViewToQDRect( vwr, qdr);
			FrameRect( qdr);
			if fGrid.Focus then ;
			END;
		END;
END;
	
PROCEDURE TRCshifter.TrackFeedback(aTrackPhase: TrackPhase; 
							anchorPoint, previousPoint, nextPoint: VPoint; 
							mouseDidMove, turnItOn: BOOLEAN); OVERRIDE;
VAR  pState: PenState;
BEGIN
	IF mouseDidMove THEN BEGIN
		GetPenState(pState);
		PenMode(patXOR);  
		DrawFeedback( anchorPoint, nextPoint); 
		SetPenState(pState);
		END;
END;




PROCEDURE TRCshifter.TrackConstrain(aTrackPhase: TrackPhase; 
						anchorPoint, previousPoint: VPoint; VAR nextPoint: VPoint;
						mouseDidMove: BOOLEAN); OVERRIDE;
var vhs: vhSelect;
	  d	: integer;
BEGIN
	if nextpoint.h < fBounds.left then nextPoint.h:= fBounds.left
	else if nextpoint.h > fBounds.right then nextPoint.h:= fBounds.right;
	if nextpoint.v < fBounds.top then nextPoint.v:= fBounds.top
	else if nextpoint.v > fBounds.bottom then nextPoint.v:= fBounds.bottom;
END;




PROCEDURE TRCshifter.DoShift( fromRC, toRC: integer);
{! OVERRIDE THIS.
	need to know details of storage here. 
 	we aren't changing num of rows or cols, 
	just shifting (private) contents of row or col
}
BEGIN

END;

PROCEDURE TRCshifter.DoIt; OVERRIDE;
BEGIN
	if fNewRC <> fOldRC then BEGIN
		DoShift(fOldRC, fNewRC);
		END;
END;



PROCEDURE TRCshifter.UndoIt; OVERRIDE;
BEGIN
	if fNewRC <> fOldRC then BEGIN
		DoShift(fNewRC, fOldRC);
		END;
END;



PROCEDURE TRCshifter.RedoIt; OVERRIDE;
BEGIN
	DoIt;
END;









{ TListSlider ------------------------}


PROCEDURE TListSlider.IListSlider( itsGridView: TGridView; itsList: TList;
																		RowOrCol: VHSelect; atRC: integer);
BEGIN
	IRCShifter(itsGridView.fDocument, itsGridView, NIL, RowOrCol, atRC);
	fList:= itsList;
	fOldTop:= fGrid.FirstSelectedCell.vh[fRowCol];
	fOldBottom:= fGrid.LastSelectedCell.vh[fRowCol];
	fOldSelection:= NewRgn; FailNil( fOldSelection);
	fNewSelection:= NewRgn; FailNIL( fNewSelection);
	CopyRgn( itsGridView.fSelections, fOldSelection);
	CopyRgn( fOldSelection, fNewSelection);
	IF fGrid.Focus THEN ;
	IF (fRowCol = v) THEN BEGIN
		{- fTrackBottom:= fBounds.bottom - (fOldBottom-fOldTop);}
		fTrackBottom:= fBounds.bottom;
		fTrackRight:= fBounds.right;
		END
	ELSE BEGIN
		fTrackBottom:= fBounds.bottom;
		{-fTrackRight:= fBounds.right - (fOldBottom-fOldTop);}
		fTrackRight:= fBounds.right;
		END;
END;


PROCEDURE TListSlider.Free; OVERRIDE;
BEGIN
	IF (fOldSelection<>NIL) THEN DisposeRgn( fOldSelection);
	fOldSelection:= NIL;
	IF (fNewSelection<>NIL) THEN DisposeRgn( fNewSelection);
	fNewSelection:= NIL;
	INHERITED Free;
END;


PROCEDURE TListSlider.DoShift( fromRC, toRC: integer); OVERRIDE;
VAR	
	tmpList: TList; 
	i, newtop, newAt, newbottom,nShift, dist: integer;
	vLoc	: VRect;
	tmpRgn: RgnHandle;
BEGIN
	dist:= toRC - fromRC;
	newTop:= fOldTop + dist;
	newBottom:= fOldBottom + dist;
	nShift:= fOldBottom - fOldTop + 1;
	tmpList:= NewList;
	FailNIL( tmpList);
	for i:= fOldBottom DOWNTO fOldTop DO BEGIN
		tmpList.InsertFirst( fList.At( i));
		fList.AtDelete( i);
		END;
	if (dist<0) THEN BEGIN
		newAt:= newTop;
		IF (fRowCol = v) THEN fGrid.RowToVRect( newtop, fOldBottom - newtop+1, vLoc)
		ELSE fGrid.ColToVRect( newtop, fOldBottom - newtop + 1, vLoc);
		END
	ELSE BEGIN
		newAt:= newTop - (newBottom - newTop){ + 1};
		IF (fRowCol = v) THEN fGrid.RowToVRect( fOldTop, newbottom -  fOldTop+1, vLoc)
		ELSE fGrid.ColToVRect( fOldTop, newbottom - fOldTop+1, vLoc);
		END;
	for i:= nShift DOWNTO 1 DO 
		fList.InsertBefore( newAt, tmpList.At(i));
	tmpList.Free;
	fGrid.InvalidateVrect( vLoc);

	CopyRgn( fOldSelection, fNewSelection);
	IF (fRowCol = v) THEN OffsetRgn( fNewSelection, 0, dist)
	ELSE OffsetRgn( fNewSelection, dist, 0);
	fGrid.SetSelection( fNewSelection, NOT kExtend, kHighlight, kSelect);

	{! tell where our selection is now, for undo/redo }
	fOldTop:= newTop; {?? or is it fOldTop:= fOldTop + dist - (size-1) }
	fOldBottom:= newBottom; {"" }
	tmpRgn:= fOldSelection;
	fNewSelection:= fOldSelection;
	fOldSelection:= tmpRgn;
END;


PROCEDURE TListSlider.DrawFeedback( anchorPoint, nextPoint: VPoint); OVERRIDE;
VAR
	aSelectRgn: RgnHandle;
	delta		: longint;
BEGIN
	aSelectRgn:= NewRgn;
	fGrid.CellsToPixels( fGrid.fSelections, aSelectRgn); 
	IF (fRowCol = v) THEN BEGIN
		delta:= nextPoint.v - anchorPoint.v;
		OffsetRgn( aSelectRgn, 0, delta);
		END
	ELSE BEGIN
		delta:= nextPoint.h - anchorPoint.h;
		OffsetRgn( aSelectRgn, delta, 0);
		END;
	FrameRgn( aSelectRgn);
	DisposeRgn( aSelectRgn);
END;


PROCEDURE TListSlider.TrackConstrain(aTrackPhase: TrackPhase; 
						anchorPoint, previousPoint: VPoint; VAR nextPoint: VPoint;
						mouseDidMove: BOOLEAN); OVERRIDE;
BEGIN
	if nextpoint.h < fBounds.left then
		nextPoint.h:= fBounds.left
	else if nextpoint.h > fTrackright then
		nextPoint.h:= fTrackright;
		
	if nextpoint.v < fBounds.top then
		nextPoint.v:= fBounds.top
	else if nextpoint.v > fTrackBottom then
		nextPoint.v:= fTrackBottom;
END;






