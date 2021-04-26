{ UGridStuff.p }
{ Copyright 1992 by d.g.gilbert, for SeqApp }


UNIT UGridStuff;

INTERFACE

USES
Types, Memory, QuickDraw, ToolUtils, OSUtils, Resources, Packages, 
Files, Printing, TextEdit,
	TextUtils, StandardFile, Scrap, 
Controls, Aliases, Editions, Events, Notification, 
AppleEvents, Processes, Balloons,

MacAppTypes, UPascalObject, UObject, 
UEventHandler, UList, UAdorners, UStream, UGeometry,
UEvent, UCommand, UCommandHandler,
	UDependencies,
UView,  UBehavior, UTEView, UTECommands,
UMacAppUtilities, UPatch, UFailure, UMacAppGlobals,

UFile, UApplication,
UPrintHandler, UPrinting,
UWindow, UMenuMgr, UMemory, UErrorMgr,

UControl, UDialog, UPopup,
UDocument, UFileHandler, UFileBasedDocument,

UViewServer, UGridView,

{$U $$Shell(UStd)UStandard.p } UStandard, 
{$U $$Shell(UStd)UApp.p } UApp, 
{$U $$Shell(UText)UTextDoc.p } UTextDoc;

TYPE

	TRCshifter		= OBJECT (TTracker)
		{If possible, get this to work on any TGridView descendant }
		{ TRCshifter is a command object to handle mouse movement when
			you select a row (?& col) in given TGridView object }

		fGrid		: TGridView;
		fSibling:	TGridView; {associated gridView to drag along}
		
		fRowCol	:	VHSelect;
		fNewRC	:	INTEGER;
		fOldRC	:	INTEGER; { remember previous width for Undo }
		fBounds	: VRect;
		
		PROCEDURE TRCshifter.IRCshifter(itsDocument: TDocument; 
						itsView: TGridView; itsSibling: TGridView;
						RowOrCol: VHSelect; atRC: integer);
 

 		FUNCTION  TRCshifter.TrackMouse(aTrackPhase: TrackPhase; VAR anchorPoint,
										 previousPoint, nextPoint: VPoint;
										 mouseDidMove: BOOLEAN): TTracker; OVERRIDE;
										
 		PROCEDURE TRCshifter.TrackFeedback(aTrackPhase: TrackPhase; 
							anchorPoint, previousPoint, nextPoint: VPoint; 
							mouseDidMove, turnItOn: BOOLEAN); OVERRIDE;
										
		PROCEDURE TRCshifter.DrawFeedback( anchorPoint, nextPoint: VPoint);

 		PROCEDURE TRCshifter.TrackConstrain(aTrackPhase: TrackPhase; 
						anchorPoint, previousPoint: VPoint; VAR nextPoint: VPoint;
						mouseDidMove: BOOLEAN); OVERRIDE;							
							
		PROCEDURE TRCshifter.DoShift( fromRC, toRC: integer);
			{This one does shift, MUST OVERRIDE with particulars of grid storage}
			
		PROCEDURE TRCshifter.DoIt; OVERRIDE;
		PROCEDURE TRCshifter.UndoIt; OVERRIDE;
		PROCEDURE TRCshifter.RedoIt; OVERRIDE;
		END;	 


	TListSlider		= OBJECT (TRCshifter)
			{ this variant will shift a list of objects according to Row or Col shift }
		fList	: TList;  {row (or column) list of items }
		fOldTop, fOldBottom: integer;
		fTrackRight,fTrackBottom: integer;
		fOldSelection, fNewSelection: RgnHandle;

		PROCEDURE TListSlider.IListSlider( itsGridView: TGridView; itsList: TList;
																						RowOrCol: VHSelect; atRC: integer);
		PROCEDURE TListSlider.Free; OVERRIDE;
		PROCEDURE TListSlider.DrawFeedback( anchorPoint, nextPoint: VPoint); OVERRIDE;
		PROCEDURE TListSlider.DoShift( fromRC, toRC: integer); OVERRIDE;
 		PROCEDURE TListSlider.TrackConstrain(aTrackPhase: TrackPhase; 
						anchorPoint, previousPoint: VPoint; VAR nextPoint: VPoint;
						mouseDidMove: BOOLEAN); OVERRIDE;
		END;

IMPLEMENTATION

		{$I UGridStuff.inc.p}

END.
