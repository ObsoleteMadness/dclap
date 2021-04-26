{ USeqGopher.p }
{ copyright by d.g.gilbert, 1992 for SeqApp }


UNIT USeqGopher;

INTERFACE

USES
	Types, Memory, QuickDraw, ToolUtils, OSUtils, Resources, Packages, 
	Files, Printing, TextEdit,
	Controls, Aliases, Editions, Events, Notification, 
	AppleEvents, Processes, Balloons, 
	GestaltEqu, Fonts, Scrap,
	
	MacAppTypes, UPascalObject, UObject, 
	UEventHandler, UList, UAdorners, UStream, UGeometry,
	UEvent, UCommand, UCommandHandler,
	UView,  UBehavior, UTEView, UTECommands,
	UMacAppUtilities, UPatch, UFailure, UMacAppGlobals,
	
	UFile, UApplication,
	UPrintHandler, UPrinting,
	UWindow, UMenuMgr, UMemory, UErrorMgr,
	
	UControl, UDialog, UPopup, UScroller,
	UDocument, UFileHandler, UFileBasedDocument,
	UViewServer, UGridView, UClipboardMgr,
	
	{$U $$Shell(UStd)UStandard.p } UStandard, 
	{$U $$Shell(UStd)UApp.p } UApp, 
	{$U $$Shell(UText)UTextDoc.p } UTextDoc,
	{$U $$Shell(UTCP)UTCP.p } UTCP, 
	{$U $$Shell(UTCP)UGopher.p } UGopher,
	
	UGridStuff;


CONST	
		kGopherFileType	= 'IGo4';
		kGopherFileName	= 'Seqapp.Gopher'; { default...}
		
TYPE

	TGopherView			= OBJECT (TGridView)
		fGopherList	: TGopherList; 		
		fRowHeight	: integer;
		fLocked			: Boolean;		{edits disallowed ?}
		
		PROCEDURE TGopherView.IGopherView( aGopherList: TGopherList);
		PROCEDURE TGopherView.Free; OVERRIDE;

		PROCEDURE TGopherView.UpdateWidth( aGopher: TGopher);
		PROCEDURE TGopherView.UpdateAllWidths;
		PROCEDURE TGopherView.UpdateSize;
		PROCEDURE TGopherView.AddToGopherList( aGopher: TGopher);
		FUNCTION  TGopherView.GopherAt( aRow: integer): TGopher;  
		PROCEDURE TGopherView.EachSelectedRowDo( PROCEDURE DoToRow( aRow: Integer)); 
		
		PROCEDURE TGopherView.DrawContents; OVERRIDE;
		PROCEDURE TGopherView.DrawRangeOfCells(startCell, stopCell: GridCell; aRect: VRect); 
					OVERRIDE;  
		PROCEDURE TGopherView.DrawCell(aCell: GridCell; aRect: VRect); 	OVERRIDE;
					
		FUNCTION  TGopherView.ContainsClipType(aType: ResType): BOOLEAN; OVERRIDE;
		PROCEDURE TGopherView.WriteToDeskScrap; OVERRIDE;
		
		PROCEDURE  TGopherView.DoMouseCommand(VAR theMouse: VPoint; event: TToolboxEvent;
											   hysteresis: Point); OVERRIDE;
		FUNCTION  TGopherView.HandleMouseDown( theMouse: VPoint; event: TToolboxEvent; 
							 hysteresis: Point):BOOLEAN; OVERRIDE;

		END;
		
		
	TGopherWindow	= OBJECT (TPrefWindow)
		fGopherView	: TGopherView; 
		
		PROCEDURE TGopherWindow.IGopherWindow;
		PROCEDURE TGopherWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
		FUNCTION  TGopherWindow.HandleMouseDown( theMouse: VPoint; event: TToolboxEvent; 
							 hysteresis: Point):BOOLEAN; OVERRIDE;
		PROCEDURE TGopherWindow.SetPrefID; OVERRIDE; 
		END;


	TGopherDocument = OBJECT (TFileBasedDocument)
		fStatus			: TStaticText;
		fDropBut		: TIcon;
		fGopherList	: TGopherList;
		fParentList	: TGopherList;
		fGopherView	: TGopherView;	 
		fLongTitle	: StringHandle;
		fParentPopup: TPopup;	
		
		PROCEDURE TGopherDocument.IGopherDocument( linkData: Handle; title: StringHandle; 
															 parentMenu: TGopher; itsFile: TFile);
		PROCEDURE TGopherDocument.Free; OVERRIDE;
		PROCEDURE TGopherDocument.FreeData; OVERRIDE; 
		PROCEDURE TGopherDocument.PushMenu( linkData: Handle; title: StringHandle; 
																			 parentMenu: TGopher);
		PROCEDURE TGopherDocument.PopMenu( uplevels: integer);
		PROCEDURE TGopherDocument.DoGopher( aGopher: TGopher);
		PROCEDURE TGopherDocument.UpdateStatus;
		PROCEDURE TGopherDocument.ShowParentMenu;
		PROCEDURE TGopherDocument.HideParentMenu;
		PROCEDURE TGopherDocument.SetMenuName( newName: str255);

		PROCEDURE TGopherDocument.DoInitialState; OVERRIDE;
		PROCEDURE TGopherDocument.DoMakeViews(forPrinting: BOOLEAN); OVERRIDE;
		PROCEDURE TGopherDocument.ShowReverted; OVERRIDE;
		PROCEDURE TGopherDocument.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE;
		PROCEDURE TGopherDocument.UntitledName(VAR noName: Str255); OVERRIDE;			

		PROCEDURE TGopherDocument.DoNeedDiskSpace(itsFile: TFile;
															VAR dataForkBytes, rsrcForkBytes: LONGINT); OVERRIDE;
		PROCEDURE TGopherDocument.DoRead( aFile:TFile; forPrinting: BOOLEAN); OVERRIDE;
		PROCEDURE TGopherDocument.DoWrite( aFile:TFile; makingCopy: BOOLEAN); OVERRIDE;

		PROCEDURE TGopherDocument.DoMenuCommand(aCommandNumber: CommandNumber); OVERRIDE;
		PROCEDURE TGopherDocument.DoSetupMenus; OVERRIDE;

		END;


	TGopherEditCommand	= OBJECT (TCommand)
			{ handles the cCopy, cCut, and cClear commands for TGopherView selections. }
		fGopherDoc	: TGopherDocument;
		fOldList		: TGopherList;			{ save orig List (not List) for Undo/Redo...}
		fSelection	: RgnHandle;			{ List selected when command was created }

		PROCEDURE TGopherEditCommand.IGopherEditCommand(itsDocument: TGopherDocument; itsCommand: INTEGER);
		PROCEDURE TGopherEditCommand.Free; OVERRIDE;
		PROCEDURE TGopherEditCommand.DoIt; OVERRIDE;
		PROCEDURE TGopherEditCommand.UndoIt; OVERRIDE;
		PROCEDURE TGopherEditCommand.RedoIt; OVERRIDE;
		PROCEDURE TGopherEditCommand.Commit; OVERRIDE;
		PROCEDURE TGopherEditCommand.CopySelection;
			{ Copy the currently selected List into the clipboard }
		PROCEDURE TGopherEditCommand.DeleteSelection;
			{ Delete the currently selected seq(s) }
		PROCEDURE TGopherEditCommand.RestoreSelection;
			{ Restore the deleted List }
		PROCEDURE TGopherEditCommand.ReSelect;
			{ Change the selection back to what it was when this command was created }
		END;				 

	TGopherPasteCommand	= OBJECT (TCommand)
		fClipList			: TGopherList;
		fGopherDoc		: TGopherDocument;
		fSelection		: RgnHandle; 	{ gophers selected when command was created }
		fReplacedList	: TGopherList;		{ the gophers we pasted over }

		PROCEDURE TGopherPasteCommand.IGopherPasteCommand(itsDocument: TGopherDocument);
		PROCEDURE TGopherPasteCommand.Free; OVERRIDE;
		PROCEDURE TGopherPasteCommand.DoIt; OVERRIDE;
		PROCEDURE TGopherPasteCommand.UndoIt; OVERRIDE;
		PROCEDURE TGopherPasteCommand.RedoIt; OVERRIDE;
		PROCEDURE TGopherPasteCommand.Commit; OVERRIDE;
		PROCEDURE TGopherPasteCommand.UpdateViews;
		END;		 
		
	TGopherPrefs	= OBJECT (TPrefWindow)

			{Item view cluster - cItm }
		fFont			: TPopup; {FONT}
		fFSize		: TPopup; {pSIZ}
		fFSizeNum	: TNumberText; {nSIZ}
		fSmall		: TRadio; { rSML}
		fMid			: TRadio; { rMID}
		fBig			: TRadio; { rBIG}
		fPath			: TCheckBox; { path}
		fHost			: TCheckBox; { host}
		fPort			: TCheckBox; { port}
		fKind			: TCheckBox; { kind}
		fDate			: TCheckBox; { date}
		fDataSize	: TCheckBox; { size}
			
			{mapping}
		fUseServerMap:  TCheckBox; {umap}
		fEditServerMap:  TButton; {emap}
		fEditGopherMap:  TButton; {gmap}
		fUnknowns	: TCheckBox; { unkn}
		fAllowLinkEdits	: TCheckBox; { edtl}

			{diggings folder}
		fDigFolder	: TStaticText; {'tdig'}
		fDigVol			: TNumberText; {'vdig'}
		fDigDirID		: TNumberText; {'ddig'}
		fFindDig		: TButton; {'bdig'}
		fUseDig			: TCheckBox; {'digs'}
		
			{Text view cluster - ctxt }
		fTFont			: TPopup; {TFNT}
		fTFSize			: TPopup; {pTSZ}
		fTFSizeNum	: TNumberText; {nTSZ}

			{Network cluster - cNet }
		fPTimeout		: TPopup; {pTIM}
		fNTimeout		: TNumberText; {nTIM}

			{default links cluster - cDLK }
		fSaveDefLinks: TButton; {bSDL}
		fRestoreDefLinks: TButton; {bRDL}
		
			{Item window cluster - cWND }
		fWindRadio: TRadio; { rWND}
		fWindClust: TCluster; { cWND}
		fWindNum	: TNumberText; { nWND}

		fInitted	: Boolean;
		
		PROCEDURE TGopherPrefs.Initialize; OVERRIDE;
		PROCEDURE TGopherPrefs.IGopherPrefs;
		PROCEDURE TGopherPrefs.SetGlobals;
		PROCEDURE TGopherPrefs.UpdateGopherDocs;
		PROCEDURE TGopherPrefs.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
		PROCEDURE TGopherPrefs.SetPrefID; OVERRIDE; 
		END;
		

VAR
	gGopherHost		: Str255;
	gGopherPort		: integer;
	gGopherDocSig: OSType;
	
PROCEDURE InitializeGopher;
PROCEDURE OpenGopher;
PROCEDURE OpenGopherPrefs;
FUNCTION AppFileToList( af: AppFile): TList;
		
IMPLEMENTATION

		{$I USeqGopher.inc.p}

END.
