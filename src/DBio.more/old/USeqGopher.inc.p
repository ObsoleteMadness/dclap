{ USeqGopher.inc.p  }
{ copyright by d.g.gilbert, 1992 for SeqApp. }

{
			This code is Copyright (C) 1992 by D.G. Gilbert.
			All Rights Reserved.

			You may use this code for your personal use, to provide a non-profit
			service to others, or to use as a test platform for a commercial
			implementation. 

			You may NOT use this code in a commercial product, nor to provide a
			commercial service, nor may you sell this code without express
			written permission of the author.  
			
			gilbertd@bio.indiana.edu 
			Biology Dept., Indiana University, Bloomington, IN 47405 
}

{$S USeqGopher }



CONST
		cOpenGopher = 25;  {!? this is used also in SeqApp main -- need public, other way ?}
		cNewGopherLink	= 2240;
		cEditGopher	= 2241;
	
		kGopherWindID 	= 2201;			 
		kGopherStyleID 	= 2202;
		kGopherPrefsID 	= 2203;
		kGopherSearchID	= 2204;
		kGopherViewID 	= 2205;
		
		kServerEditDlog = 2242;
		
		mPrefFont = 1005;	{ for prefs }
		mPrefSize = 1004;
		mParentMenus = 1006;
		
		kGopherScrapType= kGopherFileType;
		kSAppSig				= 'SApp';				{ copied from USequence.p }
		kGopherDocSig 	= kSAppSig;
		kGopherNoname 	= '¥noName';

VAR
		gItemColor 	: RGBColor;
		gItemStyle	: TextStyle;
		gIconSize		: integer;
		gShowKind, gShowPath, gShowHost, gShowPort, gShowDate, gShowSize,
		gListUnknowns, 	gAllowLinkEdits, gUseServerMap	: boolean;
		gWantFonts, gSameWindow	: boolean;
		gNumWindows	: integer;
		gSaveclip		: RgnHandle;

		gUseDig			: boolean;
		gDigFolder	: Str63;
		gDigVol			: Integer;
		gDigDirID		: Longint;
		
		gLastParentList: TGopherList;
		gPoppingMenu: Boolean;
	
FUNCTION AppFileToList( af: AppFile): TList;
VAR  tf: TFile; tl: TList;
BEGIN
  NEW( tf); FailNIL( tf);
	FailOSErr( tf.SpecifyWithTrio( af.vRefNum, 0, af.fName));
	tl:= NewList; FailNIL( tl);
	tl.InsertLast(tf);
	AppFileToList:= tl;
END;



{	TGopherDocument -------------------------- }

		
PROCEDURE TGopherDocument.IGopherDocument( linkData: Handle; title: StringHandle; 
															 parentMenu: TGopher; itsFile: TFile);
var 
		aRefNum, i: Integer;
		aGopherList: TGopherList;
		path: Handle;
		
		PROCEDURE copyParent( aPar: TGopher);
		BEGIN
			aPar:= TGopher(aPar.Clone);
			aPar.fOwnerList:= fParentList;
			fParentList.InsertLast( aPar);
		END;
				
BEGIN
	fStatus:= NIL;
	fDropBut:= NIL;
	fGopherView:= NIL; 
	fLongTitle:= NIL;
	fParentList:= NIL;
	fParentPopup:= NIL;
	NEW( aGopherList);
	FailNIL( aGopherList);
	fParentList:= aGopherList;
{-	gPoppingMenu:= FALSE;}
	
	IF parentMenu<>NIL THEN BEGIN
		parentMenu:= TGopher(parentMenu.Clone);
		parentMenu.fOwnerList:= fParentList;
		END
	ELSE BEGIN
		NEW( parentMenu);
		FailNIL( parentMenu);
		path:= NewHandle(0);
		parentMenu.IGopher( 'local', path, kGopherNoname, 0, '1'); 
		path:= DisposeIfHandle(path);
		END;
	fParentList.IGopherList(gListUnknowns, gUseServerMap, gUseDig,
													gDigVol, gDigDirID, parentMenu);
	
	IF (gLastParentList <> NIL) THEN BEGIN 
		gLastParentList.Each( copyParent);	
		gLastParentList.Free; {we made this a new list, but objects are not dupped}
		gLastParentList:= NIL;
		END;
	fParentList.InsertFirst( parentMenu);
	
	NEW(aGopherList);	
	FailNIL( aGopherList);
	fGopherList:= aGopherList;

	{- PushMenu( linkData, title, parentMenu); }
		
	fGopherList.IGopherList(gListUnknowns, gUseServerMap, gUseDig,
													gDigVol, gDigDirID, parentMenu);
	IF (linkData<>NIL) THEN fGopherList.ReadLinks( linkData);
	IF (title<>NIL) THEN FailOSErr( HandToHand( Handle( title)));
	fLongTitle:= title;
			
	IFileBasedDocument(itsFile, kGopherFileType);
END;


PROCEDURE TGopherDocument.PushMenu( linkData: Handle; title: StringHandle; 
																		 parentMenu: TGopher);
{! this is always failing, as if linkData, title where NIL or mangled...}
BEGIN
	{- FreeData; }  
	if (fLongTitle<>NIL) THEN DisposHandle(Handle(fLongTitle));  fLongTitle:= NIL;
	IF (fGopherList<>NIL) THEN fGopherList.FreeAll;  {empty but valid for new data }

	IF parentMenu<>NIL THEN BEGIN
		parentMenu:= TGopher(parentMenu.Clone);
		parentMenu.fOwnerList:= fParentList;
		fParentList.InsertFirst( parentMenu);	
		IF (fParentPopup<>NIL) THEN
			AppendMenu( fParentPopup.GetMenuHandle, parentMenu.ShortName);
		END;
	fGopherList.fParentMenu:= parentMenu;

	IF (linkData<>NIL) THEN fGopherList.ReadLinks( linkData);
	IF (title<>NIL) THEN FailOSErr( HandToHand( Handle( title)));
	fLongTitle:= title;

	{- ShowReverted; }
	fGopherView.IGopherView( fGopherList); 
  fGopherView.ForceRedraw;
END;


PROCEDURE TGopherDocument.PopMenu( uplevels: integer);
VAR
	fi		: FailInfo;
	parentMenu: TGopher;
	flag, notFound	: boolean;
	i	: integer;
	
	PROCEDURE HdlGopherFailure(error: OSErr; message: LONGINT);
	BEGIN
		gSameWindow:= flag;
	END;

	PROCEDURE checkOpenDocs( aDoc: TDocument);
	VAR 	docList: TGopherList;  docOwner: TGopher;
	BEGIN
		IF Member( TObject(aDoc), TGopherDocument) THEN BEGIN 
			docList:= TGopherDocument(aDoc).fParentList;
			{^^^ need to fix this to find '¥ Home' base document...}
			IF (docList<>NIL) THEN BEGIN
				docOwner:= TGopher(docList.First);
				IF (docOwner<>NIL) & parentMenu.Equals(docOwner) THEN BEGIN
					notFound:= FALSE;
					TWindow(aDoc.fWindowList.First).Select;  
					END;
				END;
			END;
	END;

BEGIN
	parentMenu:= TGopher( fParentList.At( uplevels));
	(******
	--- IF (gSameWindow) THEN ---
	parentMenu:= NIL;
	while (uplevels > 1) & (fParentList.GetSize > 1) DO BEGIN
		fParentList.Delete(parentMenu);		
		IF (fParentPopup<>NIL) THEN DelMenuItem( fParentPopup.GetMenuHandle, 1);
		parentMenu:= TGopher( fParentList.First);
		uplevels:= uplevels-1;
		end;
	****)
	IF (parentMenu<>NIL) THEN BEGIN
		{ !! We should look thru list of open docs for this parent before we DoGopher...}
		notFound:= TRUE;
		gApplication.ForAllDocumentsDo(checkOpenDocs); 
		IF notFound THEN BEGIN
			{- flag:= gSameWindow; gSameWindow:= TRUE;
			CatchFailures(fi, HdlGopherFailure);
			}
			gLastParentList:= TGopherList(NewList);
			FailNIL(gLastParentList);
			FOR i:= uplevels+1 TO fParentList.GetSize DO
				gLastParentList.InsertLast( fParentList.At( i));
			gPoppingMenu:= TRUE; {!@ damn nother global msg, this is getting messy }
			DoGopher( parentMenu);
			gPoppingMenu:= FALSE;
			{- 
			Success(fi);
			gSameWindow:= flag; 
			}
			END;
		END;
END;


PROCEDURE TGopherDocument.ShowParentMenu;
BEGIN
	fParentPopup.ViewEnable( TRUE, kDontRedraw);
	fParentPopup.SetCurrentItem(1, kDontRedraw);
	{- fParentPopup.AdjustBotRight;}
	fParentPopup.Show( TRUE, kRedraw);
END;
	
PROCEDURE TGopherDocument.HideParentMenu;
BEGIN
	IF (fParentPopup.IsShown) THEN BEGIN
		fParentPopup.Show( FALSE, kRedraw);
		fParentPopup.ViewEnable( FALSE, NOT kRedraw);
		END;
END;


PROCEDURE TGopherDocument.DoGopher( aGopher: TGopher);
VAR  
		fi		: FailInfo;
		gopherCommand, i : integer;
		afList: TList;
		
	PROCEDURE HdlGopherFailure(error: OSErr; message: LONGINT);
	BEGIN
		UpdateStatus;
		gopherCommand:= cNoCommand;
	END;
	
BEGIN
	IF (aGopher<>NIL) THEN BEGIN
		If gPoppingMenu THEN HideParentMenu;
		UpdateStatus;
		CatchFailures(fi, HdlGopherFailure);
		gopherCommand:= aGopher.GopherIt;
		Success(fi);
		
		CASE gopherCommand OF
		
			cNewGopherFolder: BEGIN
				IF NOT gPoppingMenu THEN BEGIN
					gLastParentList:= TGopherList(NewList);
					FailNIL(gLastParentList);
					FOR i:= 1 TO fParentList.GetSize DO
						gLastParentList.InsertLast( fParentList.At( i));
					END;
					
				IF (gSameWindow 
					| ((gNumWindows>0) & (fParentList.GetSize >= gNumWindows)) ) THEN BEGIN
					PushMenu( gGopherData, gGopherTitle, gCurrentGopher);
					END
				ELSE BEGIN
					gApplication.OpenNew( cNewGopherFolder);		
					END;
					
				aGopher.fInfo:= DisposeIfHandle( aGopher.fInfo);
				gGopherData:= NIL;
				gLastParentList:= NIL;
				END;
			
			cNewGopherText: BEGIN
				gNewTextName:= aGopher.ShortName;
				gApplication.OpenNew( cNewGopherText);
				aGopher.fInfo:= DisposeIfHandle( aGopher.fInfo);
				gGopherData:= NIL;
				END;

			cOpenGopherText: BEGIN			
				afList:= AppFileToList( gGopherFile);
				gApplication.OpenOld( cOpenGopherText, afList);
				aGopher.fInfo:= DisposeIfHandle( aGopher.fInfo);
				TObject(afList):= FreeListIfObject(afList);
				gGopherData:= NIL;
				END;
				
			END;
		
		UpdateStatus;
		END;
END;

PROCEDURE TGopherDocument.Free; OVERRIDE;
BEGIN
	FreeData; 
	TSortedList(fGopherList):= FreeListIfObject( fGopherList);
	TSortedList(fParentList):= FreeListIfObject( fParentList);
	INHERITED Free;
END;

PROCEDURE TGopherDocument.FreeData; OVERRIDE;
{ this is called from Doc.Revert & other such methods that re-read data }
BEGIN
	if (fLongTitle<>NIL) THEN Handle(fLongTitle):= DisposeIfHandle(Handle(fLongTitle)); 
	IF (fGopherList<>NIL) THEN fGopherList.FreeAll;  {empty but valid for new data }
END;


PROCEDURE TGopherDocument.DoInitialState; OVERRIDE;
{ called for New and Revert... -- load in all 'IGo4' resources }
VAR
		hData, hr	: Handle;
		i, err, rID, nlinks : integer;
		str	: str255;
		rType: ResType;
		usePref: boolean;
BEGIN
	IF (fGopherList.GetSize = 0) THEN BEGIN
				{ may have read in data during IGopherDocument...}
		hData:= NewHandle(0);
		nlinks:= Count1Resources(kGopherScrapType); {?? assume Pref rez is 1st }
		usePref:= (nlinks>0);
		IF NOT usePref THEN nlinks:= CountResources(kGopherScrapType);
		FOR i:= 1 TO nlinks DO BEGIN
			IF usePref THEN hr:= Get1IndResource(kGopherScrapType, i)
			ELSE hr:= GetIndResource(kGopherScrapType, i);
			GetResInfo( hr, rID, rType, str);
			IF (rID > 127) THEN BEGIN {skip sys rez, bundle id rez}
				{??! check that hr[end]<>0, and drop 0 if need be !??}
				HLock( hr);
				err:= HandAndHand( hr, hData);
				HUnlock( hr);
				ReleaseResource( hr);
				END;
			END;
		str[0]:= chr(4);			{append dot-cr-lf-NULL end of data}
		str[1]:= '.'; str[2]:= chr(13); str[3]:= chr(10); str[4]:= chr(0);
		err:= PtrAndHand( @str[1], hData, 4); 
		IF (nLinks>0) THEN fGopherList.ReadLinks( hData);
		DisposHandle( hData);
		END;
	{- SetMenuName('Untitled');}
END;


PROCEDURE TGopherDocument.UntitledName(VAR noName: Str255); OVERRIDE;
VAR
	gotParent: boolean;
	aGopher: TGopher;
BEGIN
	noName:= kGopherNoname; 
	gotParent:= (fParentList<>NIL) & (fParentList.GetSize>0);
	IF gotParent THEN BEGIN
		aGopher:= TGopher(fParentList.First);
		noName:= aGopher.ShortName;
		END;
	IF (noName = kGopherNoname) THEN BEGIN
		INHERITED UntitledName( noName);
		IF gotParent THEN SetMenuName( noName);
		END;
END;

PROCEDURE TGopherDocument.SetMenuName( newName: str255);
VAR
	gotParent: boolean;
	aGopher	: TGopher;
	item		: integer;
BEGIN	
	gotParent:= (fParentList<>NIL) & (fParentList.GetSize>0);
	IF gotParent THEN BEGIN
		aGopher:= TGopher(fParentList.First);
		IF (aGopher.ShortName = kGopherNoname) THEN BEGIN
			SetString( aGopher.fTitle, newName);
			IF (fParentPopup<>NIL) THEN BEGIN
				item:= CountMItems( fParentPopup.GetMenuHandle);
				SetItem( fParentPopup.GetMenuHandle, item, aGopher.ShortName);
				END;
			END;
		END;
END;


PROCEDURE TGopherDocument.DoMakeViews(forPrinting: BOOLEAN); OVERRIDE;
{ this comes after DoRead or DoInitialState...}
VAR
		aGopherWindow	: TGopherWindow;
		aHandler	: TStdPrintHandler;
		
	  Procedure InsertMenuName( aPar: TGopher);
		BEGIN
			AppendMenu( fParentPopup.GetMenuHandle, aPar.ShortName);
		END;
BEGIN
	aGopherWindow := TGopherWindow( gViewServer.NewTemplateWindow(kGopherWindID, SELF));
	FailNIL(aGopherWindow); 					 
	aGopherWindow.IGopherWindow;
	
	fGopherView := TGopherView(aGopherWindow.FindSubView('GoVw')); 
	FailNIL( fGopherView);
	fStatus	:= TStaticText(aGopherWindow.FindSubView('STAT'));
	fGopherList.SetStatus(fStatus);
	fDropBut	:= TIcon(aGopherWindow.FindSubView('DROP'));
	fParentPopup := TPopup(aGopherWindow.FindSubView('pPar')); 
	FailNIL( fParentPopup);
	{ remove it from view til needed... == HideParentMenu ?}
	fParentPopup.Show( FALSE, NOT kRedraw);
	fParentPopup.ViewEnable( FALSE, NOT kRedraw);
	DelMenuItem( fParentPopup.GetMenuHandle, 1); { drop test value }
	IF fParentList<>NIL THEN BEGIN
		fParentList.Each(insertMenuName);
		fParentList.SetStatus(fStatus);
		END;
	
	New(aHandler);
	FailNIL(aHandler);
	aHandler.IStdPrintHandler(SELF, 					{ its document }
							  fGopherView,					{ its view }
							  NOT kSquareDots,			{ does not have square dots }
							  kFixedSize, 				{ horzontal page size is fixed }
							  NOT kFixedSize);			{ vertical page size is variable }
	ShowReverted;
END;


PROCEDURE TGopherDocument.UpdateStatus;
VAR  aStr: Str255;
BEGIN
	IF (fStatus<>NIL) THEN BEGIN
		IF fStatus.Focus THEN ;
		IF (fLongTitle<>NIL) THEN fStatus.SetText( fLongTitle^^, kRedraw)
		ELSE BEGIN
			GetTitle( aStr);
			IF (length(aStr)>0) THEN fStatus.SetText(aStr, kRedraw)
			ELSE fStatus.SetText('', kRedraw);
			END;
		END;
END;


PROCEDURE TGopherDocument.ShowReverted; OVERRIDE;
VAR
	shorty: str255;
BEGIN
	fGopherView.IGopherView( fGopherList); 
	
	IF (fLongTitle<>NIL) THEN BEGIN
		IF (fStatus<>NIL) THEN fStatus.SetText( fLongTitle^^, NOT kRedraw);
		shorty:= fLongTitle^^;
		if length(shorty) > 32 then begin shorty[0]:= chr(32); shorty[32]:= 'É'; end;
		{-----  is this bombing us !!!
		IF (fTitle = NIL) THEN fTitle:= NewString( shorty)
		ELSE BEGIN
			SetString( fTitle, shorty);
			FailMemError;
			END;
		--------}
		END;
		
	INHERITED ShowReverted;
END;

	
PROCEDURE TGopherDocument.DoRead( aFile:TFile; forPrinting: BOOLEAN); OVERRIDE;
VAR
		numChars: longint;
		hData	: Handle;
		err		: OSErr;		
		p			: CharsPtr;
		aStr	: Str255;
BEGIN	
	if (fGopherList<>NIL) THEN fGopherList.FreeAll;  {? do we need this here? == SELF.FreeData, called from Revert method }
	FailOSErr( aFile.GetDataLength( numChars));
	
	hData:= NewHandle( numChars+1);	
	FailNIL( hData);
	HLock( hData);
	err:= aFile.ReadData( hData^, numChars);
	longint(p):= longint(hData^) + numChars; 
	p^[0]:= chr(0); { NULL terminator...}
	HUnlock(hData);
	IF (err=0) THEN fGopherList.ReadLinks( hData);
	DisposHandle( hData);
	FailOSErr( err);
	
	GetTitle( aStr); 
	IF (length(aStr)>0) THEN SetMenuName(aStr);
	
	{!? read window location/size from rez fork...}
END;


PROCEDURE TGopherDocument.DoNeedDiskSpace(itsFile: TFile;
															VAR dataForkBytes, rsrcForkBytes: LONGINT); OVERRIDE;
VAR h : handle;
BEGIN
	{---
	h:= fTextData.fChars;
	dataForkBytes := dataForkBytes + GetHandleSize(h);
	-----}
	INHERITED DoNeedDiskSpace(itsFile, dataForkBytes, rsrcForkBytes);
END;


PROCEDURE TGopherDocument.DoWrite( aFile:TFile; makingCopy: BOOLEAN); OVERRIDE;
VAR
	h : Handle;
	numChars: longint;
BEGIN
	{- INHERITED DoWrite( aRefNum, makingCopy); } {write out print info }

		{ Write out the text }
	IF optionKeyIsDown THEN { damn magic key to write server-style links }
		h:= fGopherList.WriteLinksForServer
	ELSE
		h:= fGopherList.WriteLinks;
	numChars := GetHandleSize(h) - 1;{! drop NULL terminator }
	HLock( h);
	FailOSErr( aFile.WriteData( h^, numChars));
	HUnlock( h);
	DisposHandle( h);
		{!? write window location/size to rez fork...}
END;


PROCEDURE TGopherDocument.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE;
BEGIN
	IF (source=fParentPopup) THEN  
			PopMenu(fParentPopup.GetCurrentItem)
	ELSE
			INHERITED DoEvent( eventNumber, source, event);
END;



PROCEDURE  TGopherDocument.DoMenuCommand(aCommandNumber: CommandNumber); OVERRIDE;
VAR
		menu, item, nSel: integer;
		aGopherEditCommand : TGopherEditCommand;
		aGopherPasteCommand: TGopherPasteCommand;
		aGopher: TGopher;
		vbounds: VRect;
		aRow, totalRows: integer;
		oldKind: Char;
		bGopher: TGopher;
		arect	: Rect;
		
	PROCEDURE EditOldGopher;
	BEGIN
		gApplication.CommitLastCommand; 
		aRow:= fGopherView.FirstSelectedCell.v;
		aGopher:= fGopherView.GopherAt(aRow); 
		IF (aGopher<>NIL) THEN BEGIN
			oldKind:= aGopher.fType;
			IF aGopher.Edit THEN BEGIN
				IF (aGopher.fType<>oldKind) &
				 fGopherList.CopyToNewKind( aGopher.fType, aGopher, bGopher) THEN BEGIN
					aGopher.Free;
					{! must re-insert bgopher into list }
					fGopherList.AtPut( aRow, bGopher);
					END;
				SetChangeCount(GetChangeCount+1);
				fGopherView.RowToVRect( aRow, 1, vbounds); 
				fGopherView.InvalidateVRect( vbounds);
				END;
			END;
	END;
		
	PROCEDURE InsertNewGopher;
	VAR  path: Handle;
	BEGIN			
		gApplication.CommitLastCommand; 
		aRow:= fGopherView.LastSelectedCell.v;
		IF aRow=0 THEN aRow:= fGopherList.GetSize;
		totalRows:= fGopherList.GetSize + 1;
		aRow:= aRow+1;
		NEW(aGopher);
		FailNIL( aGopher);
		oldKind:= aGopher.fType;
		path:= Str2Handle('/put/path/to/document/here');
		aGopher.IGopher('Put.Host.Name.Here', path,
										'Put Title of Gopher Link Here', 70, '0');
		path:= DisposeIfHandle(path);
		aGopher.fOwnerList:= fGopherList;
		IF aGopher.Edit THEN BEGIN
			IF (aGopher.fType<>oldKind) &
			 fGopherList.CopyToNewKind( aGopher.fType, aGopher, bGopher) THEN BEGIN
				aGopher.Free;
				aGopher:= bGopher;
				END;
			fGopherList.InsertBefore( aRow, aGopher);
			fGopherView.InsRowBefore( aRow, 1, fGopherView.fRowHeight);
			fGopherView.RowToVRect( aRow, totalRows-aRow+1, vbounds); 
			fGopherView.InvalidateVRect( vbounds);
			fGopherView.UpdateSize;
			SetChangeCount(GetChangeCount+1);
			END
		ELSE
			aGopher.Free;
		{select the pasted cells & scroll into view ?!!}
	END; 
	

BEGIN
	CommandToMenuItem(aCommandNumber, menu, item);		
	CASE aCommandNumber OF

		cSelectAll:  BEGIN
				SetRect( aRect, 1, 1, fGopherView.fNumOfCols, fGopherView.fNumOfRows);
				fGopherView.SetSelectionRect( aRect, kDontExtend, kHighlight, kSelect);
				END;
				
		cCut, cCopy, cClear: BEGIN
				gApplication.CommitLastCommand; {paste relies on clipView data til commit}
				NEW(aGopherEditCommand);
				FailNIL(aGopherEditCommand);
				aGopherEditCommand.IGopherEditCommand(SELF, aCommandNumber);
				PostCommand( aGopherEditCommand);
				END;

		cPaste:
				IF gClipboardMgr.fClipView.ContainsClipType(kGopherScrapType) THEN BEGIN
					gApplication.CommitLastCommand; {copy/cut/.. relies on clipView data til commit}
					NEW(aGopherPasteCommand);
					FailNIL(aGopherPasteCommand);
					aGopherPasteCommand.IGopherPasteCommand(SELF);
					PostCommand( aGopherPasteCommand);
					END
				{ ELSE IF gClipView.ContainsClipType('TEXT') THEN BEGIN
					// try to convert from TEXT to gopher data...
					END
				}
				ELSE  
					INHERITED DoMenuCommand(aCommandNumber);

		cNewGopherLink	: InsertNewGopher;
		cEditGopher	: EditOldGopher;
		
		OTHERWISE
			INHERITED DoMenuCommand(aCommandNumber);
		END;
END;




PROCEDURE TGopherDocument.DoSetupMenus; OVERRIDE;
VAR 
	nsel	: integer;
	indexSelection	: Boolean;
BEGIN
  INHERITED DoSetupMenus;	
	
	nsel:= 0;
	IF (fGopherView<>NIL) & (fGopherView.FirstSelectedCell.v > 0) THEN 
		nsel:= 1 + fGopherView.LastSelectedCell.v - fGopherView.FirstSelectedCell.v;
	indexselection:= nsel>0;
	
	{IF indexSelection & clipHasSeqText... THEN CanPaste('TEXT'); }
	gClipboardMgr.CanPaste(kGopherScrapType);
	
	Enable(cCut, indexSelection);
	Enable(cCopy, indexSelection);
	Enable(cClear, indexSelection);
	Enable(cSelectAll, TRUE);
	 
	Enable( cNewGopherLink, gAllowLinkEdits);
	Enable( cEditGopher, gAllowLinkEdits & (nsel>=1)); 
 
END;











{ TGopherView -----------------------------}


PROCEDURE TGopherView.IGopherView( aGopherList: TGopherList);
VAR  vbounds: VRect;
BEGIN
	fGopherList:= aGopherList;
	fLocked:= FALSE;
	fRowheight:= GetRowHeight(1);
	UpdateSize; 
	UpdateAllWidths;
END;


PROCEDURE TGopherView.Free; OVERRIDE;
BEGIN
	INHERITED Free;
END;



PROCEDURE TGopherView.UpdateSize;
VAR	  diff: integer;
BEGIN
	IF Focus THEN ;
	
	IF (fGopherList<>NIL) THEN BEGIN
			
		diff:= fGopherList.GetSize - fNumOfRows;
		IF (diff>0) THEN InsRowLast( diff, fRowheight) 	
		ELSE IF (diff<0) THEN DelRowLast( -diff);
		END
	ELSE
		DelRowLast( fNumOfRows);
END;


PROCEDURE TGopherView.UpdateWidth(aGopher: TGopher);
VAR			maxpixels : integer;
BEGIN
	SetPortTextStyle(gItemStyle);  
	maxpixels:= max(GetColWidth(2), StringWidth(aGopher.fTitle^^) + 5);
	SetColWidth(2, 1, maxpixels);  
END;

{
	grid columns (1-2 perm, 3-6 optional)
	1			2			3			4			5			6
	icon	title	kind	path	host	port
}
PROCEDURE TGopherView.UpdateAllWidths;
VAR			maxIcon, max2, max3, max4, max5, max6, maxSize, maxDate : integer;
				saveport : GrafPtr;
				
		PROCEDURE findMaxLength(aGopher: TGopher);
		BEGIN
			max2:= max(max2, StringWidth(aGopher.fTitle^^) + 5);
			IF (gShowDate & (aGopher.fDate<>NIL)) THEN 
				maxDate:= max(maxDate,6+StringWidth(aGopher.fDate^^));
			IF (gShowSize & (aGopher.fDataSize<>NIL)) THEN 
				maxSize:= max(maxSize,6+StringWidth(aGopher.fDataSize^^));
		END;
BEGIN
	GetPort( saveport);
	SetPort( gWorkPort);
	SetPortTextStyle( gItemStyle);  {<< for proper StringWidth }	

	case gIconSize of
		1: maxIcon:= 16;  {12 + 4}
		2: maxIcon:= 20;  {16 + 4}
		3: maxIcon:= 36;
		otherwise maxIcon:= 36;
		END;

	max2:= GetColWidth(2);
	maxDate:= 0;
	maxSize:= 0;
	IF fGopherList<>NIL THEN fGopherList.Each( findMaxLength );
	IF (gShowKind) THEN max3:= StringWidth('Binhex encoded file') ELSE max3:= 0;
	IF (gShowPath) THEN max4:= StringWidth('1/this/is/a/long/path') ELSE max4:= 0;
	IF (gShowHost) THEN max5:= StringWidth('some.where.indiana.edu') ELSE max5:= 0;
	IF (gShowPort) THEN max6:= StringWidth('12345') ELSE max6:= 0;
	SetPort( saveport);

	IF Focus THEN ;
	fRowheight:= maxIcon;
	SetRowHeight( 1, fNumOfRows, fRowheight);  
	SetColWidth( 1, 1, maxIcon);  
	SetColWidth( 2, 1, max2); 
	SetColWidth( 3, 1, maxDate);
	SetColWidth( 4, 1, maxSize);
	SetColWidth( 5, 1, max3);
	SetColWidth( 6, 1, max4);
	SetColWidth( 7, 1, max5);
	SetColWidth( 8, 1, max6);
END;


PROCEDURE TGopherView.addToGopherList(aGopher: TGopher);
VAR  aRect: VRect;
BEGIN
	fGopherList.InsertLast( aGopher);
	InsRowLast( 1, fRowheight);	
	UpdateWidth( aGopher);	
	RowToVRect( fNumOfRows, 1, aRect);
	InvalidateVRect( aRect);  
END;

FUNCTION TGopherView.GopherAt( aRow: integer): TGopher;  
BEGIN
	IF (fGopherList=NIL) | (aRow>fGopherList.GetSize) THEN GopherAt:= NIL
	ELSE GopherAt:= TGopher(fGopherList.At( aRow));  
END;


PROCEDURE TGopherView.EachSelectedRowDo( PROCEDURE DoToRow( aRow: Integer)); 
VAR
		aCell: GridCell;
		irow, jcol, mincol, maxcol: integer;
BEGIN
	mincol:=	FirstSelectedCell.h;
	maxcol:=  LastSelectedCell.h;
	FOR irow:= FirstSelectedCell.v TO LastSelectedCell.v DO BEGIN
		aCell.v:= irow;
		for jcol:= mincol to maxcol DO BEGIN
			aCell.h:= jcol;
			IF isCellSelected( aCell) THEN BEGIN
				DoToRow( irow);
				Leave; { jcol...}
				END;
			END;
		END;
END;


PROCEDURE TGopherView.DrawCell(aCell: GridCell; aRect: VRect); OVERRIDE;
VAR	 aGopher	: TGopher;
			qdRect: Rect;
BEGIN
	aGopher:= GopherAt(aCell.v);
	IF (aGopher<>NIL) then BEGIN
		ViewToQDRect( aRect, qdRect);
		IF Focus THEN ;
		GetClip( gSaveclip);
		ClipFurtherTo( qdRect, gZeroPt);
		MoveTo( aRect.left, aRect.bottom-4); 
		SetPortTextStyle(gItemStyle); { this was failing in DrawContents ??!}
		CASE aCell.h OF
			1	: aGopher.DrawIcon( qdRect, gIconSize);
			2	: aGopher.DrawTitle(qdRect);
			3	: IF (gShowDate) THEN aGopher.DrawDate( qdRect);
			4	: IF (gShowSize) THEN aGopher.DrawSize( qdRect);
			5	: IF (gShowKind) THEN aGopher.DrawKind( qdRect);
			6	: IF (gShowPath) THEN aGopher.DrawPath( qdRect);
			7	: IF (gShowHost) THEN aGopher.DrawHost( qdRect);
			8	: IF (gShowPort) THEN aGopher.DrawPort( qdRect);
			END;
		SetClip( gSaveclip);
		END;
END;

PROCEDURE TGopherView.DrawRangeOfCells(startCell, stopCell: GridCell; aRect: VRect); 
										OVERRIDE; {from TGridView method}
BEGIN
	INHERITED DrawRangeOfCells( startCell, stopCell, aRect);
END;

PROCEDURE TGopherView.DrawContents; OVERRIDE;
BEGIN
	SetPortTextStyle(gItemStyle); {< this one is failing in Mapp3 }
	{- IF gConfiguration.hasColorQD then RGBForeColor( gItemColor);	 }
	INHERITED DrawContents;
END;


PROCEDURE  TGopherView.DoMouseCommand(VAR theMouse: VPoint; event: TToolboxEvent;
											   hysteresis: Point); OVERRIDE;
VAR
	aRow: integer;
	aCell	: GridCell;
	aDoubleClick: boolean;
	vpt		: VPoint;
	gridPart: GridViewPart;	
	aListSlider	: TListSlider;
	
	PROCEDURE DoMouseCommandByRow(VAR theMouse: VPoint; event: TToolboxEvent;
											   hysteresis: Point);
	{ from TGridView.DoMouseCommand, w/ TCellSelectCommand replaced by TRowSelect...}
	VAR
		aCell	: GridCell;
		aRowSelectCommand: TRowSelectCommand;
	BEGIN
		IF IdentifyPoint(theMouse, aCell) <> badChoice THEN BEGIN
			New(aRowSelectCommand);
			FailNIL(aRowSelectCommand);
			aRowSelectCommand.IRowSelectCommand(SELF, theMouse, event.IsShiftKeyPressed, 
								event.IsCommandKeyPressed);
			PostCommand( aRowSelectCommand);
			END;
	END;

BEGIN
	aDoubleClick:= event.fClickCount > 1;
	gridPart:= IdentifyPoint(theMouse, aCell);
	aRow:= aCell.v;
	
	IF aDoubleClick THEN BEGIN
	  TGopherDocument(fDocument).DoGopher( GopherAt(aRow));
		END
		
	ELSE IF (event.IsShiftKeyPressed |event.IsCommandKeyPressed 
				| event.IsOptionKeyPressed) THEN BEGIN 
		DoMouseCommandByRow(theMouse,event,hysteresis);
		END
		
	ELSE IF gridPart <> badChoice THEN BEGIN
		aCell	:= VPointToLastCell(TheMouse);
		IF isCellSelected(aCell) THEN BEGIN
			New(aListSlider);
			FailNIL(aListSlider);
			aListSlider.IListSlider(SELF, fGopherList, v,aRow);
			PostCommand( aListSlider);
			END
		ELSE BEGIN
			SetEmptySelection(kHighlight);
			event.fEventRecord.modifiers := bOr(event.fEventRecord.modifiers,shiftKey);
  		DoMouseCommandByRow(theMouse,event,hysteresis);
			END;
		END;
END;



FUNCTION  TGopherView.HandleMouseDown( theMouse: VPoint; event: TToolboxEvent; 
							 hysteresis: Point):BOOLEAN; OVERRIDE;
BEGIN		
	HandleMouseDown:= INHERITED HandleMouseDown(theMouse,event,hysteresis);
END;


FUNCTION TGopherView.ContainsClipType(aType: ResType): BOOLEAN; OVERRIDE;
BEGIN
	ContainsClipType := aType = kGopherScrapType;
	{!? also (aType = 'TEXT') may be gopher data}
END;

PROCEDURE TGopherView.WriteToDeskScrap; OVERRIDE;
VAR
		textScrap	: Handle;
		err				: OSErr;
		format		: integer;
BEGIN
	textScrap:= fGopherList.WriteLinks;
								{^^ do this just for gopherList items selected in this view ?? }
	SetHandleSize( textScrap, GetHandleSize(textScrap)-1); {! drop NULL terminator }
	err := gClipboardMgr.PutDeskScrapData('TEXT', textScrap); {? instead/also put as kGopherScrapType}
	textScrap := DisposeIfHandle(textScrap);
	FailOSErr(err);
END;










{ TGopherWindow ------------------------}


PROCEDURE TGopherWindow.IGopherWindow;
BEGIN	
	IPrefWindow;
	fWantSave:= FALSE; {TRUE !?}
END;

PROCEDURE TGopherWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
BEGIN
	INHERITED DoEvent( eventNumber, source, event);
	{--------
	CASE eventNumber OF
		mSendit			: BEGIN Sendit; Close; END;
		Otherwise 
				INHERITED DoEvent( eventNumber, source, event);
		END;
	---------}
END;
	

FUNCTION  TGopherWindow.HandleMouseDown( theMouse: VPoint; event: TToolboxEvent; 
							 hysteresis: Point):BOOLEAN; OVERRIDE;
VAR
		aDlog: TDialogView; 
		aCell	: GridCell;
		aWMgrWindow:		WindowPtr;
		whereMouseDown: 	integer;
		aPartCode: integer;
		
		{ a mess, but it works... we could avoid this by subclassing the
			fStatus & fDropBut items to pass on HandleMouseDown to the hidden Popup
			-- cleaner but more work.
			^^^ Move this to Dialog.HandleMouseDown --
				-- Here we get menu popups from behind windows...
		}
	FUNCTION TestStatPopup: BOOLEAN;
	VAR 	subViewPt: VPoint;  dropMenu: Boolean;
	BEGIN
		IF TGopherDocument(fDocument).fStatus = NIL THEN 
			TestStatPopup := FALSE
		ELSE BEGIN
			subViewPt := theMouse;
			TGopherDocument(fDocument).fStatus.SuperToLocal(subViewPt);
			DropMenu:= TGopherDocument(fDocument).fStatus.ContainsMouse(subViewPt);
			IF NOT DropMenu THEN BEGIN
				subViewPt := theMouse;
				TGopherDocument(fDocument).fDropBut.SuperToLocal(subViewPt);
				DropMenu:= TGopherDocument(fDocument).fDropBut.ContainsMouse(subViewPt);
				END;
			IF DropMenu THEN BEGIN
				TGopherDocument(fDocument).ShowParentMenu;
				TestStatPopup:= 
				TGopherDocument(fDocument).fParentPopup.HandleMouseDown(theMouse,event,hysteresis);
				TGopherDocument(fDocument).HideParentMenu;
				END
			ELSE
				TestStatPopup := FALSE;
		END;
	END;
		
BEGIN		
	IF (gApplication.GetActiveWindow = SELF) THEN BEGIN
		CASE SELF.GetPartCode(theMouse) OF
			inContent: 
				IF gApplication.GetActiveWindow <> SELF THEN 
					Select 
				ELSE IF NOT TestStatPopup THEN
					HandleMouseDown:= INHERITED HandleMouseDown(theMouse,event,hysteresis);
			
			{-----------
			inDrag:
				IF theCommandKey THEN BEGIN  
					TGopherDocument(fDocument).ShowParentMenu;
					HandleMouseDown:= 
					TGopherDocument(fDocument).fParentPopup.HandleMouseDown(theMouse,event,hysteresis);
					TGopherDocument(fDocument).HideParentMenu;
					END
				ELSE  
					MoveByUser(where);
			-----------}
		
			Otherwise 
				HandleMouseDown:= INHERITED HandleMouseDown(theMouse,event,hysteresis);
			END
		END
	ELSE
		HandleMouseDown:= INHERITED HandleMouseDown(theMouse,event,hysteresis);
END;



PROCEDURE TGopherWindow.SetPrefID;  
BEGIN
	gPrefWindID:= kGopherWindID; gPrefWindName:= 'TGopherWindow';
END;









{	TGopherPrefs -------------------------- }
			
PROCEDURE TGopherPrefs.Initialize; OVERRIDE;
BEGIN
	inherited Initialize;
	fInitted:= false;
END;

PROCEDURE TGopherPrefs.IGopherPrefs;
VAR		ascroller: TScroller;
BEGIN	
	IPrefWindow;
	fWantSave:= TRUE;

	fFont 		:= TPopup(FindSubView('FONT'));  
  fFSize   	:= TPopup(FindSubView('pSIZ'));  
  fFSizeNum	:= TNumberText(FindSubView('nSIZ'));  
  fSmall		:= TRadio(FindSubView('rSML'));
  fMid			:= TRadio(FindSubView('rMID'));
  fBig			:= TRadio(FindSubView('rBIG'));
  fPath			:= TCheckBox(FindSubView('path'));
  fHost			:= TCheckBox(FindSubView('host'));
  fPort			:= TCheckBox(FindSubView('port'));
  fKind			:= TCheckBox(FindSubView('kind'));	
  fDate			:= TCheckBox(FindSubView('date'));	
  fDataSize	:= TCheckBox(FindSubView('size'));	

  fUnknowns	:= TCheckBox(FindSubView('unkn'));	
  fAllowLinkEdits	:= TCheckBox(FindSubView('edtl'));	
	fUseServerMap   := TCheckBox(FindSubView('umap'));
	fEditServerMap	:= TButton(FindSubView('emap'));
	fEditGopherMap	:= TButton(FindSubView('gmap'));
	
		{diggings}
	fDigFolder	:= TStaticText(FindSubView('tdig'));
	fDigVol			:= TNumberText(FindSubView('vdig'));
	fDigDirID		:= TNumberText(FindSubView('ddig'));
	fFindDig		:= TButton(FindSubView('bdig'));
	fUseDig			:= TCheckBox(FindSubView('digs'));
	IF fDigDirID.GetValue = 0 THEN BEGIN
		fUseDig.DimState( TRUE, kDontRedraw);
		fUseDig.ViewEnable(FALSE, kDontRedraw);
		END;
	
	fTFont 			:= TPopup(FindSubView('TFNT'));  
  fTFSize   	:= TPopup(FindSubView('pTSZ'));  
  fTFSizeNum	:= TNumberText(FindSubView('nTSZ'));  

  fPTimeout   := TPopup(FindSubView('pTIM'));  
  fNTimeout		:= TNumberText(FindSubView('nTIM'));  

  fSaveDefLinks   := TButton(FindSubView('bSDL'));  
  fRestoreDefLinks := TButton(FindSubView('bRDL'));  

	{-----
  fWindRadio:= TRadio(FindSubView('rWND'));
  fWindClust:= TCluster(FindSubView('cWND'));
  fWindNum	:= TNumberText(FindSubView('nWND'));
	----}

	ascroller:= fFont.GetScroller(FALSE);
	IF (ascroller<>NIL) THEN ascroller.fScrollLimit.v:= 256; {fix so prefview scroll bar shows}
	
	IF (false) {gWantFonts} THEN BEGIN {<< need menu for save of font pref }
				{!!!! can't use AddResMenu for View TPopUp (bug in MacApp) }
		{?? fFont.SetPopup( MAGetMenu(mFont), 1 , kDontRedraw);}
		{?? fTFont.SetPopup( MAGetMenu(mFont), 1 , kDontRedraw);}
		{- AddResMenu(	fFont.GetMenuHandle, 'FONT');   }
		{- fFont.AdjustBotRight;}
		{- AddResMenu(	fTFont.GetMenuHandle, 'FONT'); }   
		{- fTFont.AdjustBotRight;}
		END;
	fInitted:= true;
END;


PROCEDURE TGopherPrefs.UpdateGopherDocs;
	PROCEDURE checkOpenDocs( aDoc: TDocument);
	BEGIN
		IF Member(TObject(aDoc), TGopherDocument) 
		 | Member(TObject(aDoc), TTextDocument) THEN  
			aDoc.ShowReverted;
	END;
BEGIN
	gApplication.ForAllDocumentsDo(checkOpenDocs); 
END;


(***	


			DAMN THIS BUG HAS BITTEN ME A FEW TIMES...
			MACAPP THREE REQUIRES THAT ANY MENU CREATED WITH RESEDIT HAVE THE
			MENU ID SET TO EQUAL THE RESOURCE ID, IN THE MENU EDIT 
				"EDIT MENU & MDEF ID..." 
			ITEM.

*****)


PROCEDURE TGopherPrefs.SetGlobals;
VAR  aStr: str255;

	PROCEDURE DoFontChange( VAR aStyle: TextStyle; aFontMenu: TPopup; aFontSize: TNumberText);
	VAR  aName: str255;   
	BEGIN
		aFontMenu.GetItemText( aFontMenu.GetCurrentItem, aName); 
		aStyle.tsFont:= GetFontNum( aName);
		aStyle.tsSize:= aFontSize.GetValue;
	END;
	
	PROCEDURE InitFonts( aStyle: TextStyle; aFontMenu: TPopup; aFontSize: TNumberText);
	VAR  aName: str255;  aFont, i, nitems: integer;   
	BEGIN
		nitems:= CountMitems(aFontMenu.GetMenuhandle);  
	  for i:= 1 to nitems do begin
			aFontMenu.GetItemText( i, aName); 
			GetFNum( aName, aFont);
			if (aFont = aStyle.tsFont) then begin				
				aFontMenu.SetCurrentItem( i, kDontRedraw);
				LEAVE;
				END;
			END;
		aFontSize.SetValue(aStyle.tsSize, kDontRedraw);
	END;

	
BEGIN	
 	if (NOT fInitted) THEN EXIT(SetGlobals); { map3 -- prevent this call before IGopherPrefs}
	
	if fBig.IsOn then gIconSize:= 3
	else if fMid.IsOn then gIconSize:= 2
	else gIconSize:= 1;
	 
	gShowDate:= fDate.IsOn;
	gShowSize:= fDataSize.IsOn;

	gListUnknowns:= fUnknowns.IsOn;
	gAllowLinkEdits:= fAllowLinkEdits.IsOn;
	gUseServerMap:= fUseServerMap.IsOn;

	gUseDig:= fUseDig.IsOn;
	fDigFolder.GetText( aStr); 
	gDigFolder:= aStr;
	gDigVol:= fDigVol.GetValue; 
	gDigDirID:= fDigDirID.GetValue;
	 
	gShowKind:= fKind.IsOn;
	gShowPath:= fPath.IsOn;
	gShowHost:= fHost.IsOn;
	gShowPort:= fPort.IsOn;
	
	gTCPTimeout:= fNTimeout.GetValue;
	{----
	gSameWindow:= NOT fWindRadio.IsOn;
	gNumWindows:= fWindNum.GetValue;
	----}
	
	IF gWantFonts THEN BEGIN
		DoFontChange( gItemStyle, fFont, fFSizeNum);
		DoFontChange( gDefaultSpecs.theTextStyle, fTFont, fTFSizeNum);
		END
	ELSE BEGIN
		InitFonts( gItemStyle, fFont, fFSizeNum);
		InitFonts( gDefaultSpecs.theTextStyle, fTFont, fTFSizeNum);
		END;	

	UpdateGopherDocs;	{ if any gopherDocs/windows open -- update them !?}
END;


PROCEDURE TGopherPrefs.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
VAR
		snum: Str255;
		val	: longint;
		changeit: boolean;
		index, i, len: integer;
		aGopherDoc: TGopherDocument;
		secondfront: WindowPtr;

	PROCEDURE DoEditMap( PROCEDURE ReadMap( h: Handle); mapId: integer; title: Str63);
	VAR		hText: Handle;
				aWind	: TWindow;
				aTextView: TDlogTextView;
				dismisser : IDType;
				aTitle: TStaticText;
	BEGIN
		hText:= TPrefApplication(gApplication).GetPreference('TEXT',mapId);
		IF (hText<>NIL) THEN BEGIN
			aWind := gViewServer.NewTemplateWindow(kServerEditDlog, NIL);
			FailNIL( aWind); 		 
			aTitle:= TStaticText( aWind.FindSubView('titl'));
			IF (aTitle<>NIL) THEN aTitle.SetText( title, kDontRedraw);
			aTextView:= TDlogTextView( aWind.FindSubView('TEXT'));
			FailNIL( aTextView);
			aTextView.IDlogTextView(hText);

			aWind.ShowReverted;  {! need to show hText ?!} 
			{- aWind.Open;  }
			
			aWind.SetModality( True); {!!! or PoseModally will fail !}
			dismisser := aWind.PoseModally;
			IF dismisser = 'OKAY' THEN BEGIN
				hText:= aTextView.fHTE^^.hText;  
				ReadMap( hText);
				TPrefApplication(gApplication).SetPreference('TEXT', mapId, hText);
				END;
				
			aWind.CloseAndFree; {& free}
			gApplication.UpdateAllWindows;
			END;
	END;
	
	PROCEDURE FindDiggingFolder;
	VAR
		 newreply	: StandardFileReply;
		 oldreply	: SFReply;
		 aStr			: Str63;
		 okay			: boolean;
		 pb				: CInfoPBRec;
		 where		: point;
	BEGIN
		aStr:= ''; 
		okay:= FALSE;
		IF GestaltFlag( gestaltStandardFileAttr, gestaltStandardFile58) THEN BEGIN
			StandardPutFile( 'Locate diggings folder', 'Put diggings here', newreply);
			IF newreply.sfGood THEN with newreply do BEGIN
				okay:= TRUE;
				IF sfIsFolder THEN ; {??}
				gDigVol:= sfFile.vRefNum;
				gDigDirID:= sfFile.parID;
				END;
			END
		ELSE BEGIN
			CenterSFPut( where);
			SFPutFile( where, 'Locate diggings folder', 'Put diggings here', NIL, oldreply);
			IF oldReply.good THEN BEGIN
				okay:= TRUE;
				gDigVol:= oldreply.vRefNum;
				gDigDirID:= 0;
				END;
			END;
		IF okay THEN BEGIN			
			WITH pb DO BEGIN
				ioCompletion:= NIL;
				ioNamePtr:= @aStr;
				ioVRefNum:= gDigVol;
				ioFDirIndex:= -1;
				ioDirID:= gDigDirID;
				END;
			IF noErr <> pbGetCatInfo( @pb, FALSE) THEN;
			gDigFolder:= aStr;
			fDigVol.SetValue( gDigVol, kDontRedraw);
			fDigDirID.SetValue( gDigDirID, kDontRedraw);
			fDigFolder.SetText( aStr, kRedraw);
			fUseDig.DimState( FALSE, kDontRedraw);
			fUseDig.ViewEnable(TRUE, kRedraw);
			END;
	END;
	
	PROCEDURE WritePrefLink( item: TGopher);
	VAR  hf: hcFile;
	BEGIN
		hcRewrite( hf);
		item.ToText( hf);
		hcClose( hf); {< puts 0 at end of h, we don't want that !}
		SetHandleSize( hf.h, GetHandleSize(hf.h)-1);
		index:= index+1;
		TPrefApplication(gApplication).SetPreference(kGopherScrapType, index, hf.h);
		hf.h:= DisposeIfHandle(hf.h);
	END;
	
	FUNCTION isFrontGopherDoc( aDoc: TDocument): Boolean;
	BEGIN
		isFrontGopherDoc:= FALSE;
		IF Member(TObject(aDoc), TGopherDocument) THEN BEGIN
			isFrontGopherDoc:= TWindow(aDoc.fWindowList.First).fWMgrWindow = secondfront;
			END;
	END;


BEGIN
	changeit:= false;
	CASE eventNumber OF

		mCheckboxHit,
		mEditTextHit,
		mListItemHit,
		mPictureHit,
		mIconHit,
		mRadioHit: BEGIN {come here with mRadioHit before window is open !!}
			{?? if (event<>NIL) then} changeit:= TRUE;
			END;

		mButtonHit : BEGIN
			if (event<>NIL) then changeit:= TRUE;		
			IF (source = fSaveDefLinks) THEN BEGIN
				changeit:= FALSE;
				secondfront:= WindowPtr( WindowPeek(FrontWindow)^.nextWindow);
				TObject(aGopherDoc):= gApplication.fDocumentList.FirstThat(isFrontGopherDoc); 
				IF (aGopherDoc<>NIL) THEN BEGIN
					TPrefApplication(gApplication).DeleteAllPreference(kGopherScrapType);
					index:= 1000;
					aGopherDoc.fGopherList.Each( WritePrefLink);
					END;
				END			
			ELSE IF (source = fRestoreDefLinks) THEN BEGIN
				changeit:= FALSE;		
				TPrefApplication(gApplication).DeleteAllPreference(kGopherScrapType);
				END 
			ELSE IF (source = fFindDig) THEN BEGIN
				changeit:= FALSE;		
				FindDiggingFolder;
				END 
			ELSE IF (source = fEditGopherMap) THEN BEGIN
				changeit:= FALSE;		
				DoEditMap( ReadGopherMap,kGopherMapID,
					'Gopher to Mac Type map (experimental)');
				END 
			ELSE IF (source = fEditServerMap) THEN BEGIN
				changeit:= FALSE;		
				DoEditMap( ReadServerMap,kServerMapID,
					'Server suffix to Mac Type map (experimental)');
				END;
				
			END;
					
		mPopupHit: BEGIN
			if (event<>NIL) then changeit:= TRUE;
			IF (source = fFSize) THEN BEGIN
				fFSize.GetItemText( fFSize.GetCurrentItem, snum);
				{- MyPopupGetItemText( fFsize, fFsize.GetCurrentItem, snum);}
				StringToNum( snum, val);
				fFSizeNum.SetValue( val, kRedraw);
				END
			ELSE IF (source = fTFSize) THEN BEGIN
				fTFSize.GetItemText( fTFSize.GetCurrentItem, snum); 
				{- MyPopupGetItemText( fTFSize, fTFSize.GetCurrentItem, snum);}
				StringToNum( snum, val);
				fTFSizeNum.SetValue( val, kRedraw);
				END 
			ELSE IF (source = fPTimeout) THEN BEGIN
				fPTimeout.GetItemText( fPTimeout.GetCurrentItem, snum); 
				{- MyPopupGetItemText( fPTimeout, fPTimeout.GetCurrentItem, snum);}
				StringToNum( snum, val);
				fNTimeout.SetValue( val, kRedraw);
				END;
			END;	
				
		Otherwise BEGIN
			changeit:= FALSE;
			INHERITED DoEvent( eventNumber, source, event);
			END;
		END;
		
	IF changeit THEN SetGlobals;
END;


PROCEDURE TGopherPrefs.SetPrefID; OVERRIDE; 
BEGIN
	gPrefWindID:= kGopherPrefsID; gPrefWindName:= 'TGopherPrefs';
END;







{ OpenDialogs ....................................}

PROCEDURE InitializeGopher;
VAR		
		aWind: TGopherPrefs;
		havePrefs: boolean;
		saveMap: integer;
		
	procedure getSomeStyle( var aColor: RGBColor; var aTextStyle: TextStyle; rsrcID: integer);
	var  aTEView: TTEView;
	begin
		MAGetTextStyle(rsrcID, aTextStyle); {Mapp3}
		aColor:= aTextStyle.tsColor;
		{----
		aTEView := TTEView(gViewServer.DoCreateViews(NIL, NIL, rsrcID, gZeroVPt));
		FailNIL(aTEView);
		aColor:= aTEView.fTextStyle.tsColor;
	  aTextStyle := aTEView.fTextStyle;
		aTEView.Free;
		----}
	end;

BEGIN	
	gGopherHost	:= 'gopher.micro.umn.edu'; {these 2 are not used, see 'IGo4' rez}
	gGopherPort := 70;
	gSaveclip:= NewRgn;
	gLastParentList:= NIL;
	gGopherDocSig	:= kGopherFileType; {IGo4}
	gPoppingMenu:= FALSE;

	gIconSize:= 3;
	gShowDate:= FALSE;
	gShowSize:= FALSE;
	gShowKind:= FALSE;
	gShowPath:= FALSE;
	gShowHost:= FALSE;
	gShowPort:= FALSE;
	gSameWindow:= FALSE;
	gNumWindows:= 0;
	gListUnknowns:= FALSE;
	gUseServerMap:= FALSE;
	gAllowLinkEdits:= FALSE;

	gUseDig:= FALSE;
	gDigFolder:= '';
	gDigVol:=  gAppWDRef; {gAppVolRef;  }
	gDigDirID:= gAppDirID;
	
	getSomeStyle( gItemColor, gItemStyle, kGopherStyleID);
		{!! if this is 1st call (template from App rez not from Prefs),
			then need to set sane font/size depending on user installed fonts...
		}
	{- TPrefApplication(gApplication).OpenPreferenceFile;}
	IF TPrefApplication(gApplication).fPrefRefNum > 0 THEN BEGIN
		{- saveMap:= CurResFile; }
		UseResFile(TPrefApplication(gApplication).fPrefRefNum);
		gWantFonts:= Get1Resource( 'view', kGopherPrefsID) <> NIL;
		{- UseResFile(saveMap); }
		END
	ELSE
		gWantFonts:= FALSE;
		
	aWind := TGopherPrefs(gViewServer.NewTemplateWindow(kGopherPrefsID, NIL));
	IF (aWind<>NIL) THEN BEGIN
		aWind.IGopherPrefs;
		aWind.SetGlobals;
		IF NOT gWantFonts THEN aWind.SetPrefID; 
		aWind.Free;
		END;
	gWantFonts:= TRUE;
	
	IF gDeadStripSuppression THEN BEGIN
		IF Member(TObject(NIL), TGopherDocument) THEN;
		IF Member(TObject(NIL), TGopherWindow) THEN;
		IF Member(TObject(NIL), TGopherView) THEN;
		IF Member(TObject(NIL), TGopherPrefs) THEN;
		END;
END;

	
PROCEDURE OpenGopher;
VAR   isNowOpen, okay	: boolean;
			aRefNum	: integer;
			afList:	TList;
			
	PROCEDURE checkOpenDocs( aDoc: TDocument);
	BEGIN
		IF Member(TObject(aDoc), TGopherDocument) THEN BEGIN 
			TWindow(aDoc.fWindowList.First).Select; 
			isNowOpen:= TRUE;
			END;
	END;
BEGIN
	isNowOpen:= FALSE;
	afList:= NIL;
	{- gApplication.ForAllDocumentsDo(checkOpenDocs); }
	
	{! if frontwindow == gopherView/doc & an item is selected, then
		this command should do the equivalent of a doubleclick
		(see TGopherView.DoMouseCommand)
	--}
	
	IF NOT isNowOpen THEN BEGIN
		IF optionKeyIsDown THEN 
			okay:= FALSE
		ELSE WITH gGopherFile DO BEGIN
			vRefNum:= 0; {! should use something better ...}
			versnum:= 0; 
			fType:= kGopherFileType;
			fName:= kGopherFileName;
			okay:= (noErr = FSOpen( fName, vRefNum, aRefNum));
			IF (noErr = FSClose(aRefNum)) THEN ;
			END;
		IF (okay) THEN
			afList:= AppFileToList( gGopherFile) 
	  else
			okay:= gApplication.ChooseDocument( cOpenGopher, afList);
		IF (okay) THEN 
			gApplication.OpenOld( cOpenGopher, afList);
		TObject(afList):= FreeListIfObject( TList(afList));
		END;
END;


PROCEDURE OpenGopherPrefs;
VAR aWind	: TGopherPrefs; isNowOpen: boolean;

	PROCEDURE checkOpenWind( aWind: TWindow);
	BEGIN
		IF Member(TObject(aWind), TGopherPrefs) THEN BEGIN 
			aWind.Select; isNowOpen:= TRUE;
			END;
	END;
BEGIN
	isNowOpen:= FALSE;
	gApplication.ForAllWindowsDo(checkOpenWind);
	IF NOT isNowOpen THEN BEGIN
		aWind := TGopherPrefs(gViewServer.NewTemplateWindow(kGopherPrefsID, NIL));
		FailNIL(aWind); 		 
		aWind.IGopherPrefs;
		aWind.ShowReverted;   
		aWind.Open;  
		END;
END;





{ TGopherEditCommand ------------------------------}


{! DAMN -- this is near duplication of my TSequence GridView handlers
  -- really want GridView general method that we subclass as needed,
		 this is more code (but less brain work...)
}

{$S TGopherEditCommand}

PROCEDURE TGopherEditCommand.IGopherEditCommand(itsDocument: TGopherDocument; itsCommand: INTEGER);
VAR fi:			FailInfo;
	 theContext:	TCommandHandler; 

	PROCEDURE HdlAllocationFailure(error: OSErr; message: LONGINT);
	BEGIN
		Free;
	END;

BEGIN
	fSelection:= NIL;
	fOldList:= NIL;
	FailNIL( itsDocument);
	{ theContext:	TCommandHandler;}
	theContext:= itsDocument.fGopherView.GetContext(itsCommand);
	ICommand(itsCommand, theContext, kCanUndo, kCausesChange, theContext);

	fGopherDoc := itsDocument; 
	CatchFailures(fi, HdlAllocationFailure);
	fSelection := MakeNewRgn;					 
	fOldList:= TGopherList(fGopherDoc.fGopherList.Clone);
	FailNIL( fOldList);
	CopyRgn(fGopherDoc.fGopherView.fSelections, fSelection);
	Success(fi);
	
	fChangesClipboard := itsCommand <> cClear;
	fCausesChange := itsCommand <> cCopy;
	fCanUndo := itsCommand <> cCopy;  {?}
END;


PROCEDURE TGopherEditCommand.Free; OVERRIDE;
BEGIN
	IF (fSelection<>NIL) THEN DisposeRgn(fSelection);
	fSelection := NIL;
	IF (fOldList<>NIL) THEN fOldList.Free;
	fOldList:= NIL;
	INHERITED Free;
END;


PROCEDURE TGopherEditCommand.CopySelection;
VAR
		clipDoc		: TGopherDocument;
		clipView	: TGopherView;
		
	PROCEDURE CopyRowToClipboard(aRow: integer);
	VAR  aGopher: TGopher;
	BEGIN
		aGopher:= TGopher(fGopherDoc.fGopherView.GopherAt(aRow).Clone);
		aGopher.fOwnerList:= clipDoc.fGopherList;
		clipDoc.fGopherList.InsertLast( aGopher);
	END;

BEGIN
	New(clipDoc);
	FailNIL(clipDoc);
	clipDoc.IGopherDocument( NIL, NIL, NIL, NIL);
	fGopherDoc.fGopherView.EachSelectedRowDo(CopyRowToClipboard);  

	clipView:= TGopherView( 
		gViewServer.DoCreateViews(clipDoc, NIL, kGopherViewID, gZeroVPt));
	FailNIL( clipView);
	clipView.IGopherView( clipDoc.fGopherList);

	SELF.ClaimClipboard(clipView);  
END;



PROCEDURE TGopherEditCommand.DeleteSelection;
VAR
		totalRows: integer;
		
	PROCEDURE MarkDelete(aRow: integer);
	VAR	 aGopher : TGopher;
			 vbounds: VRect;
			 jcol, maxcol: integer;
			 aCell: GridCell;
	BEGIN
		aGopher:= fGopherDoc.fGopherView.GopherAt(aRow);
		IF (aGopher<>NIL) THEN BEGIN
			aGopher.fDeleted:= TRUE;
			fGopherDoc.fGopherList.AtDelete(aRow); { this should be safe...}
			
			fGopherDoc.fGopherView.RowToVRect( aRow, totalRows-aRow+1, vbounds); 
			fGopherDoc.fGopherView.InvalidateVRect( vbounds);
			
			aCell.v:= aRow; maxcol:= fGopherDoc.fGopherView.LastSelectedCell.h;
			for jcol:= 1 to maxcol do begin
				aCell.h:= jcol;
				fGopherDoc.fGopherView.SelectCell( aCell, kDontExtend, kDontHighlight, NOT kSelect);
				end;
			END;
	END;

BEGIN
	totalRows:= fOldList.GetSize;
	fGopherDoc.fGopherView.EachSelectedRowDo(MarkDelete);
	fGopherDoc.fGopherView.UpdateSize;  
END;


PROCEDURE TGopherEditCommand.RestoreSelection;
VAR  aRow, totalRows: integer;
	
	PROCEDURE restoreDeletes(aGopher: TGopher);
	VAR  vbounds: VRect;
	BEGIN
		aRow:= aRow+1;
		IF (aGopher.fDeleted) THEN BEGIN  
			aGopher.fDeleted:= FALSE;
			fGopherDoc.fGopherList.InsertBefore(aRow, aGopher); 	
			
			fGopherDoc.fGopherView.InsRowBefore( aRow, 1, fGopherDoc.fGopherView.fRowHeight);
			fGopherDoc.fGopherView.RowToVRect( aRow, totalRows-aRow+1, vbounds); 
			fGopherDoc.fGopherView.InvalidateVRect( vbounds);
			END;
	END;

BEGIN
	aRow:= 0;
	totalRows:= fOldList.GetSize;
	fOldList.Each( restoreDeletes);
	fGopherDoc.fGopherView.UpdateSize;  
	ReSelect; { == fGopherDoc.fGopherView??.ReSelect(fSelection);}
END;


PROCEDURE TGopherEditCommand.Commit;
	PROCEDURE commitDeletes(aGopher: TGopher);
	BEGIN
		IF (aGopher<>NIL) & (aGopher.fDeleted) THEN aGopher.Free;  
	END;
BEGIN
	fOldList.Each( commitDeletes);
END;

PROCEDURE TGopherEditCommand.ReSelect;
BEGIN
	fGopherDoc.fGopherView.SetSelection(fSelection, kDontExtend, kHighlight, kSelect);
END;


PROCEDURE TGopherEditCommand.DoIt;
BEGIN
	IF fIdentifier <> cClear THEN CopySelection;
	IF fIdentifier <> cCopy THEN DeleteSelection;
END;


PROCEDURE TGopherEditCommand.UndoIt;
{ If the user has changed the selection since this command was created,
  restore it before Undoing so that the correct cells are affected. }
BEGIN
	IF fIdentifier <> cCopy THEN BEGIN
		RestoreSelection;
		{- ReSelect;	<< see RestoreSelection....}
		fGopherDoc.fGopherView.ScrollSelectionIntoView(TRUE);
		END;
END;


PROCEDURE TGopherEditCommand.RedoIt;
{ If the user has changed the selection since this command was created,
  restore it before Redoing so that the correct cells are affected. }
BEGIN
	IF fIdentifier <> cCopy THEN BEGIN
		DeleteSelection;
		{- ReSelect;	 ! NOT After DELETION....}	 
		fGopherDoc.fGopherView.ScrollSelectionIntoView(TRUE);
		END;
END;









{ TGopherPasteCommand -----------------  }
{$S TGopherPasteCommand}


PROCEDURE TGopherPasteCommand.IGopherPasteCommand(itsDocument: TGopherDocument);
VAR 	fi:			FailInfo;
	theContext:	TCommandHandler;

	PROCEDURE HdlAllocationFailure(error: OSErr; message: LONGINT);		
	BEGIN
		Free;
	END;
	
	PROCEDURE cloneit( aGopher: TGopher);
	VAR  aRow: integer;
	BEGIN
		aRow:= fClipList.GetEqualItemNo(aGopher);
		aGopher:= TGopher(aGopher.Clone);
		aGopher.fOwnerList:= fClipList;
		fClipList.AtPut( aRow, aGopher);
	END;

BEGIN
	{ theContext	TCommandHandler;}
	theContext:= itsDocument.fGopherView.GetContext(cPaste);
	ICommand(cPaste, theContext, kCanUndo, kCausesChange, theContext);

	fGopherDoc := itsDocument;
	CatchFailures(fi, HdlAllocationFailure);
	fSelection := MakeNewRgn;							{ copy the current selection region }
	Success(fi);
	CopyRgn(fGopherDoc.fGopherView.fSelections, fSelection);
			
	{- fClipList:= TGopherView(gClipView).fGopherList; }
	{^^ clone all of this so we don't have mysterious bombs when something trashes clipbd
		before this command is committed }
	fClipList:= TGopherList(TGopherView(gClipboardMgr.fClipView).fGopherList.Clone);
	fClipList.Each( cloneit);
	
	fReplacedList:= TGopherList(NewList);
	FailNIL(fReplacedList);
END;


PROCEDURE TGopherPasteCommand.Free; OVERRIDE;
BEGIN
	IF fSelection <> NIL THEN DisposeRgn(fSelection);
	fSelection := NIL;
	TObject(fClipList) := FreeListIfObject(fClipList); {New since cloned clip list}
	TObject(fReplacedList) := FreeListIfObject(fReplacedList);
	INHERITED Free;
END;


PROCEDURE TGopherPasteCommand.DoIt;
VAR  aRow, totalRows: integer;
		
	PROCEDURE InsertMarker( aGopher: TGopher);
	VAR   vbounds: VRect;
	BEGIN
		aRow:= aRow+1;
		{- aGopher:= TGopher(aGopher.Clone);} {? do here or in Commit<<<<  ???}
		{ mark here, fDeleted, then clone in .Commit method}
		aGopher.fDeleted:= TRUE;  {< this is mark that we Inserted it (not deleted)}
		
		fGopherDoc.fGopherList.InsertBefore( aRow, aGopher);
		
		fGopherDoc.fGopherView.InsRowBefore( aRow, 1, fGopherDoc.fGopherView.fRowHeight);
		fGopherDoc.fGopherView.RowToVRect( aRow, totalRows-aRow+1, vbounds); 
		fGopherDoc.fGopherView.InvalidateVRect( vbounds);
	END;

BEGIN			
	aRow:= fGopherDoc.fGopherView.LastSelectedCell.v;
	IF aRow=0 THEN aRow:= fGopherDoc.fGopherList.GetSize;
	totalRows:= fGopherDoc.fGopherList.GetSize + fClipList.GetSize;
	fClipList.Each(InsertMarker);
	fGopherDoc.fGopherView.UpdateSize;  
	{select the pasted cells & scroll into view ?!!}
END; 


PROCEDURE TGopherPasteCommand.UndoIt;
VAR
		totalRows: integer;
		
	PROCEDURE DeleteIfMarked( aGopher: TGopher);
	VAR	 vbounds: VRect;
			 aRow, jcol, maxcol: integer;
			 aCell: GridCell;
	BEGIN
		IF (aGopher<>NIL) & (aGopher.fDeleted) THEN BEGIN
			aGopher.fDeleted:= FALSE;
			aRow:= fGopherDoc.fGopherList.GetEqualItemNo(aGopher);
			fGopherDoc.fGopherList.AtDelete(aRow); { is this safe? -- we are in List.Each...}
			
			fGopherDoc.fGopherView.RowToVRect( aRow, totalRows-aRow+1, vbounds); 
			fGopherDoc.fGopherView.InvalidateVRect( vbounds);

			aCell.v:= aRow; maxcol:= fGopherDoc.fGopherView.LastSelectedCell.h;
			for jcol:= 1 to maxcol do begin
				aCell.h:= jcol;
				fGopherDoc.fGopherView.SelectCell( aCell, kDontExtend, kDontHighlight, NOT kSelect);
				END;
			END;
	END;

BEGIN
	totalRows:= fGopherDoc.fGopherList.GetSize;
	fGopherDoc.fGopherList.Each( DeleteIfMarked);
	fGopherDoc.fGopherView.UpdateSize;  
END;


PROCEDURE TGopherPasteCommand.RedoIt;
BEGIN
	DoIt;
END;

PROCEDURE TGopherPasteCommand.Commit; OVERRIDE;

	PROCEDURE commitInserts( aGopher: TGopher);
	VAR  aRow: integer;
	BEGIN
		IF (aGopher.fDeleted) THEN BEGIN
			aGopher.fDeleted:= FALSE;
			aRow:= fGopherDoc.fGopherList.GetEqualItemNo(aGopher);
			aGopher:= TGopher(aGopher.Clone);
			aGopher.fOwnerList:= fGopherDoc.fGopherList;
			fGopherDoc.fGopherList.AtPut( aRow, aGopher);
			END;
	END;
BEGIN
	fGopherDoc.fGopherList.Each( commitInserts);
END;

PROCEDURE TGopherPasteCommand.UpdateViews;
BEGIN

END;
