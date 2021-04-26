// USeqPrint.seq.p 
// d.g.gilbert, 1991 

/*
	one sequence print
	
*/

#pragma segment SeqPrint

CONST
	kMacdrawHeaderSize == 512;

	kShadeInvert == 1;
	kShadeGray50 == 2;
	kShadeGray25 == 3;
	kShadeGray75 == 4;
	kShadeStipple50	== 5;
	kShadeStipple25	== 6;
	kShadeStipple75	== 7;
	
	mColorButHit == 1001;
	mMonoButHit	== 1002;

  kMaxNameWidth == 120;
	kNucSpace 	== 0; //was 2
	kBasesPerLine == 50; //was 40
	kNucBorder	== 6;
	kSeqLinesPerParagraph == 5;
	
	kSeqPrintWindowRSRCID == 1032;



// TSeqPrintDocument -----------------------------------------------


pascal void TSeqPrintDocument::ISeqPrintDocument(OSType fileType, TDocument parentDoc, 
													TSequence		aSeq; longint startbase, nbases)

VAR  aFile: TFile;
{
	fSeqPrintView = NULL;
	fSeq= NULL;
	fParentDoc= parentDoc;
	FailNIL( aSeq);
	fSeq= TSequence(aSeq->Clone());
	if (nbases<1) fSeq->GetSelection( startbase,nbases);
	fFirstBase= startBase;
	fNBases= nbases;
	
	/*---
	New(aFile); aFile->IFile(...);
	aname= parentDoc.name + '.dotplot';
	vol= parentdoc.vol;
	dirid= parentdoc.dirid;
	err= aFile->SpecifyWithTrio( name/vol/dirid);
	----*/
	aFile= NewFile(fileType, kSAppSig, kUsesDataFork, kUsesRsrcFork, !kDataOpen, !kRsrcOpen);
	IFileBasedDocument(aFile, fileType);
	fSavePrintInfo = FALSE; //was TRUE;
}

pascal void TSeqPrintDocument::FreeData(void)
{
	if (fSeq!=NULL)	fSeq->Free();
	fSeq= NULL;
}


pascal void TSeqPrintDocument::Free(void) // override 
{
	FreeData();
	inherited::Free();
}


pascal void TSeqPrintDocument::Close(void) /* override */ 
{
		/* patches for prefwindow */		
	if ((fSeqPrintView->fUseColor) && (fColorButton!=NULL)) fColorButton->fHilite= TRUE;
	this->SetChangeCount(0); //so we don't always get Save? yes/no/canc dlog 
  inherited::Close();
}



pascal void TSeqPrintDocument::DoMakeViews(Boolean forPrinting) // override 
VAR
		aWindow	: TWindow;
		minSize	: Point;
		maxSize	: Point;
		vSize 	: Vpoint;
		aTEDlog	: TDialogViewTE;
{
	aWindow = gViewServer->NewTemplateWindow(kSeqPrintWindowRSRCID, this);
	FailNil(aWindow);
	
	fSeqPrintWind = TSeqPrintWind(aWindow);
	fSeqPrintWind->ISeqPrintWind();
	fSeqPrintWind->fRedrawBut= TButton(aWindow->FindSubView('bRDR'));
	
	fSeqPrintView = TSeqPrintView(aWindow->FindSubView('PrVw'));
	FailNil(fSeqPrintView);
	fSeqPrintView->InstallControls();

	fColorButton= TIcon(aWindow->FindSubView('iClr'));
	fColorButton->fEventNumber= mColorButHit;
	fMonoButton = TIcon(aWindow->FindSubView('iB&W'));
	fMonoButton->fEventNumber= mMonoButHit;
	fSeqPrintView->fUseColor= fColorButton->fHilite;  
	fColorButton->fHilite= FALSE;
	fMonoButton->fHilite= FALSE;
	if (!gConfiguration.hasColorQD) {
		fColorButton->ViewEnable(FALSE, kDontRedraw);
		fColorButton->Show(FALSE, kDontRedraw);
		fMonoButton->ViewEnable(FALSE, kDontRedraw);
		fMonoButton->Show(FALSE, kDontRedraw);
		fSeqPrintView->fUseColor	= FALSE;
		}
	
		// get our damn data into view 
	fSeqPrintView->SetDrawOptions();
	
		//?? need something to get text in style TE's displayed !

	// set window's resize limits so it can't become wider than the SeqPrintview's edge 
	WITH aWindow->fResizeLimits){
		minSize = topLeft;
		maxSize = botRight;
		}
	WITH maxSize)h = Min( 2 + fSeqPrintView->fSize.h + kSBarSizeMinus1, h);
	//-- aWindow->SetResizeLimits(minSize, maxSize); << bad !!
	
	//? may need to show TEView in PrintView... ?
	this->ShowReverted();   

}


pascal void TSeqPrintDocument::UntitledName(Str255 VAR noName) // override 
//called AFTER .Idoc and .MakeViews 
{
	noName= Concat(fSeq->fName,' PrettyPrint'); 
	if ((fWindowList != NULL) && (fWindowList.GetSize > 0)) 		
		TWindow(fWindowList->First())->SetTitle(noName);
}


pascal TFile TSeqPrintDocument::DoMakeFile(itsCommandNumber:CommandNumber); // override 
{
	DoMakeFile= NewFile(fScrapType, gApplication->fCreator, kUsesDataFork, kUsesRsrcFork, 
												!kDataOpen, !kRsrcOpen);
}



pascal void TSeqPrintDocument::DoNeedDiskSpace(TFile itsFile,
															long VAR dataForkBytes, rsrcForkBytes)
{
	//)!get Print record requirements 
	//- inherited::DoNeedDiskSpace(dataForkBytes, rsrcForkBytes); 
	
	dataForkBytes = dataForkBytes + kMacdrawHeaderSize  /*+ sizeof window pict */;
	
	/*-- if you really want to know pict size:
	fSeqPrintView->WriteToDeskScrap(); 
	len= fSeqPrintView->GivePasteData( NULL, 'PICT');
	rsrcForkBytes = rsrcForkBytes + kRsrcTypeOverhead + kRsrcOverhead + sizeof(DocState);
	---*/
}

pascal void TSeqPrintDocument::DoRead(aFile:TFile; Boolean forPrinting)
{
	//-- inherited::DoRead(aRefNum, rsrcExists, forPrinting);)!read print info stuff
	// This is a Write-Only document == PICT of output drawing, no reading... ?
}


pascal void TSeqPrintDocument::DoWrite(TFile aFile, Boolean makingCopy)
VAR
		longint		len, count;
		hPict			: handle;
		header		: packed array [1..kMacdrawHeaderSize] of char;
		fi				: FailInfo;

	pascal void HdlDoWrite(OSErr error, long message)
	{
		if (hPict != NULL) DisposHandle( hPict);
	}

{
	//- inherited::DoWrite(aRefNum, makingCopy); 	--)NO write print info stuff

	fSeqPrintView->WriteToDeskScrap();
	
	hPict= NewHandle(0);
	CatchFailures(fi, HdlDoWrite);
	len= fSeqPrintView->GivePasteData( hPict, 'PICT');
	if ((len > 0)) {
		fillchar(header, kMacdrawHeaderSize, ((char)(0)));  
		count = kMacdrawHeaderSize;
		FailOSErr( aFile->WriteData( &header, count));
		count= len;
		HLock(hPict);
		FailOSErr( aFile->WriteData( ptr((*hPict)), count));
		HUnlock(hPict);	
		}
	Success(fi);
	DisposHandle( hPict);
}




// TSeqPrintWind ------------------------- 

pascal void TSeqPrintWind::ISeqPrintWind(void)
{
	IPrefWindow();
	fWantSave= TRUE;
}

pascal void TSeqPrintWind::SetPrefID(void) /* override */ 
{
	gPrefWindID= kSeqPrintWindowRSRCID; gPrefWindName= 'TSeqPrintWind';
}

pascal void TSeqPrintWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
VAR
		TSeqPrintView		aSeqPrintView;

{
	aSeqPrintView= TSeqPrintDocument(fDocument).fSeqPrintView;
	//- aSeqPrintView->SetDrawOptions();

	switch (eventNumber) {
		mColorButHit: {
				aSeqPrintView->fUseColor= TRUE;
				aSeqPrintView->ForceRedraw();
				}
		mMonoButHit: {
				aSeqPrintView->fUseColor= FALSE;
				aSeqPrintView->ForceRedraw();
				}
		mButtonHit: if ((source == fRedrawBut)) {
				aSeqPrintView->SetDrawOptions();
				aSeqPrintView->ForceRedraw();
				}			else 
				inherited::DoEvent(eventNumber,source, event); 

		otherwise
			inherited::DoEvent(eventNumber,source, event); 
		}
}






// TSeqPrintView -------------------------------------

pascal void TSeqPrintView::Free(void) // override 
{
	inherited::Free();
}


pascal void TSeqPrintView::Initialize(void) // override 
//called by IObject/via IView...
CONST
	//- kBasesPerLine == 50;
	kTenSpacer 	== 10;
{
	inherited::Initialize();
	
	fSeqPrintDocument= NULL;
	fFirstBase= 0;  
	fNbases= 0;  
	fSeq= NULL;  

	fStyleName= NULL;
	fStyleBase= NULL;
	fStyleNums= NULL;
	fUseColor= TRUE;
	fDoTopIndex= TRUE;
	fDoLeftIndex= FALSE;
	fDoRightIndex= FALSE;
	fDoLeftName= TRUE;
	fDoRightName= FALSE;
	fBasesPerLine= kBasesPerLine;
	fTenSpacer= kTenSpacer;
	fNameWidth= kMaxNameWidth;
	fIndexWidth= kMaxNameWidth;
}


pascal void TSeqPrintView::ISeqPrintView( TSeqPrintDocument itsDocument, Boolean forClipboard)
VAR
		itsSize		: VPoint;
		aHandler	: TStdPrintHandler;
		sd				: SizeDeterminer;
{
	fSeqPrintDocument = TSeqPrintDocument(itsDocument);
	fFirstBase= fSeqPrintDocument->fFirstBase;  
	fNbases= fSeqPrintDocument->fNbases;  
	fSeq= fSeqPrintDocument->fSeq;  
	
	SetVPt(itsSize, kMaxCoord, kMaxCoord);
	if (forClipboard)
		sd = sizeVariable
	else
		sd = sizeFillPages;
	//- IView(itsDocument, NULL, gZeroVPt, itsSize, sd, sd); 
	IGridView( itsDocument, NULL, gZeroVPt, itsSize, sd, sd, 10, 50, 20, 20, 
													FALSE, FALSE, 0, 0, FALSE);

#if FALSE										//!!! Need to handle this
	if (forClipboard) fWouldMakePICTScrap = TRUE;
#endif
	if (!forClipboard) {
		New(aHandler);
		FailNil(aHandler);
		aHandler->IStdPrintHandler(itsDocument, this, !kSquareDots, kFixedSize, kFixedSize);		 
		}
}

 

pascal void TSeqPrintView::DoPostCreate(TDocument itsDocument) // override 
VAR  aHandler: TStdPrintHandler;
{
	inherited::DoPostCreate(itsDocument);
	fSeqPrintDocument= TSeqPrintDocument(itsDocument);
	fFirstBase= fSeqPrintDocument->fFirstBase;  
	fNbases= fSeqPrintDocument->fNbases;  
	fSeq= fSeqPrintDocument->fSeq;  
	New(aHandler);
	FailNil(aHandler);
	aHandler->IStdPrintHandler(itsDocument, this, !kSquareDots, kFixedSize, kFixedSize);	 
}
 

pascal void TSeqPrintView::InstallControls(void)
VAR  aWindow	: TWindow;
		 sh	: StringHandle;
{
	aWindow= GetWindow;
	
	fTopIndex	= TCheckBox(aWindow->FindSubView('cTop'));
	fLeftIndex= TCheckBox(aWindow->FindSubView('cLft'));
	fRightIndex= TCheckBox(aWindow->FindSubView('cRgt'));
	fLeftName	= TCheckBox(aWindow->FindSubView('nLft'));
	fRightName= TCheckBox(aWindow->FindSubView('nRgt'));
	
	fStyleName= TDlogTextView(aWindow->FindSubView('tNam'));
	fStyleBase= TDlogTextView(aWindow->FindSubView('tBas'));
	fStyleNums= TDlogTextView(aWindow->FindSubView('tNum'));

	if (fStyleName!=NULL) fStyleName->SetText('Names');
	if (fStyleBase!=NULL) fStyleBase->SetText('Bases');
	if (fStyleNums!=NULL) fStyleNums->SetText('Index');
	/*- this->ShowReverted();  -- need this in MakeViews to get displayed !?!?! --*/ 
}


pascal void TSeqPrintView::FindNameWidth(void)
VAR    
		linesPerParag, numLinesPerSeq,
		integer		maxNameWid,fontheight, maxdeep;
		GrafPtr		savePort;
		nums	: Str255;
		longint		lastBase;
		fi		: fontInfo;
		
	pascal void GetNameWidth(TSequence aSeq)
	VAR  aName		: Str63;
	{
		if (aSeq!=NULL) {
			aName= aSeq->fName;
			maxNameWid= max(maxNameWid, StringWidth(aName));
			} 
	}
{
	if (fStyleName == NULL) fNameStyle= gPrintNameStyle 
	else fNameStyle= fStyleName->fTextStyle;
	if (fStyleBase == NULL) fBaseStyle= gPrintNucStyle 
	else fBaseStyle= fStyleBase->fTextStyle;
	if (fStyleNums == NULL) fNumStyle= gPrintNameStyle 
	else fNumStyle= fStyleNums->fTextStyle;
	maxdeep= 0;
	
	GetPort( savePort);
	SetPort( gWorkPort);
	//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
	maxNameWid= 0;
	//- fAlnList->Each(GetNameWidth);
	GetNameWidth( fSeq);
	fNameWidth= Min( maxNameWid + kNucBorder, kMaxNameWidth); 
	GetFontInfo(fi);
	fontheight= fi.ascent+fi.descent+fi.leading;
	maxdeep= Max(maxdeep, fontheight);
	
	//$PUSH*/ /*$H-*/ SetPortTextStyle(fNumStyle);  /*$POP
	lastBase= fFirstBase + fNBases;
	NumToString( lastBase, nums);
	fIndexWidth= StringWidth(nums) + kNucBorder;
	GetFontInfo(fi);
	fontheight= fi.ascent+fi.descent+fi.leading;
	maxdeep= Max(maxdeep, fontheight);
	
	//$PUSH*/ /*$H-*/ SetPortTextStyle(fBaseStyle);  /*$POP
	fBaseWidth= CharWidth('G') + kNucSpace;
	GetFontInfo(fi);
	fontheight= fi.ascent+fi.descent+fi.leading;
	maxdeep= Max(maxdeep, fontheight);
	
		//fix grid size: 
	DelColLast( fNumOfCols);
	InsColLast( fBasesPerLine+4, fBaseWidth);
	
	numLinesPerSeq= (fBasesPerLine - 1 + fNBases) / fBasesPerLine;
	//- linesPerParag= fNumSeqs + 2; */ /*+ top index + bottom spacer
	linesPerParag= kSeqlinesPerParagraph;
	
	DelRowLast( fNumOfRows);
	//- InsRowLast( linesPerParag * numLinesPerSeq, maxDeep);
	InsRowLast( numLinesPerSeq 
		+ 2*((numLinesPerSeq+linesPerParag) / linesPerParag), maxDeep); 
	
	SetPort( savePort);
}


pascal void TSeqPrintView::SetDrawOptions(void) 
VAR		
		integer		wid, i ;
		Str255		aStr;
{
	FindNameWidth();

	if (fTopIndex == NULL) fDoTopIndex= TRUE else fDoTopIndex= fTopIndex->IsOn();  
	if (fLeftIndex == NULL) fDoLeftIndex= FALSE else fDoLeftIndex= fLeftIndex->IsOn();  
	if (fRightIndex == NULL) fDoRightIndex= FALSE else fDoRightIndex= fRightIndex->IsOn();  
	if (fLeftName == NULL) fDoLeftName= FALSE else fDoLeftName= fLeftName->IsOn();  
	if (fRightName == NULL) fDoRightName= FALSE else fDoRightName= fRightName->IsOn();  
	
	if (fDoLeftName) wid= fNameWidth else wid= 0;
	this->SetColWidth( 1, 1, wid);
	if (fDoLeftIndex) wid= fIndexWidth else wid= 0;
	this->SetColWidth( 2, 1, wid);
	
	FOR i= 1 TO fBasesPerLine){
		if ((i<fBasesPerLine) && (i % 10 == 0)) SetColWidth( i+2, 1, fBaseWidth+fTenSpacer)
		else SetColWidth( i+2, 1, fBaseWidth);
		}
		
	if (fDoRightIndex) wid= fIndexWidth else wid= 0;
	this->SetColWidth( 2+fBasesPerLine+1, 1, wid);
	if (fDoRightName) wid= fNameWidth else wid= 0;
	this->SetColWidth( 2+fBasesPerLine+2, 1, wid);
}


pascal void TSeqPrintView::DrawRangeOfCells(GridCell startCell, stopCell, VRect aRect) 
					// override 
{
	laserLine( 2, 4); // set laserline smaller ...
	
	inherited::DrawRangeOfCells( startCell, stopCell, aRect);
}



pascal void TSeqPrintView::DrawCell(GridCell aCell, VRect aRect) // override 
CONST
	kFontDescent == 2;
	kIndexRise == 1;
VAR
	longint		atBase ;
	integer		linesPerParag ;
	longint		atSeq, seqLine ;	
	Boolean		doTopIndex, doSequence;
	
	
	pascal void drawTopIndex( longint atBase)  
	VAR  nums	: str255;
			 integer		chRight, chLeft, rowtop, ws;
	{
	//$PUSH*/ /*$H-*/ SetPortTextStyle(fNumStyle);  /*$POP
		chleft	= aRect.left;
		chRight	= chleft + fBaseWidth; //fix for extra spacers
		rowtop	= aRect.bottom-kIndexRise;  
		MoveTo( chleft+kNucSpace, rowtop);
		if (atBase % 10 == 4) then begin
			Line(0,-1);
			NumToString( atBase+1, nums);
			ws= StringWidth(nums);
			Move(-ws / 2, -1);
			DrawString(nums);
			}		else
			Line(0,-2);
		MoveTo( chleft, rowtop);
		LineTo( chright, rowtop);
	}

	pascal void drawSideIndex(longint atBase, leftBorder)
	str255		VAR  nums ;
	{
	//$PUSH*/ /*$H-*/ SetPortTextStyle(fNumStyle);  /*$POP
		MoveTo( aRect.right-leftBorder, aRect.bottom-kFontDescent);
		NumToString( atBase+1, nums);
		Move( -StringWidth(nums), 0);
		DrawString(nums);
	}

	pascal void DrawName(integer atSeq, rightBorder, longint atBase)
	VAR 	aName	: Str63;
				aSeq	: TSequence;
	{
		//- aSeq= fAlnList->SeqAt( atSeq); 
		aSeq= fSeq;
		if (aSeq!=NULL) {
	//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
			MoveTo( aRect.left+rightBorder, aRect.bottom-kFontDescent);
			aName= aSeq->fName;
			DrawString(aName);
			} 
	}

	pascal void DrawNuc(integer atSeq, longint atBase)
	VAR
 		integer		rowbot, nucwidth, cw, rowleft;
		hSeq		: CharsHandle;
		ch			: char;
		aSeq		: TSequence;
		myRect	: VRect;
	{
		//- aSeq= fAlnList->SeqAt( atSeq); 
		aSeq= fSeq;
		if (aSeq!=NULL) {
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fBaseStyle);  /*$POP
			nucwidth= fBaseWidth; //aRect.right - aRect.left; 
			rowleft= aRect.left;
			rowbot = aRect.bottom - kFontDescent;  
			myRect= aRect;
			myRect.right= myRect.left + fBaseWidth; //fix for extra spacers
			handle(hSeq)= aSeq->fBases;
			ch= (*hSeq)^[atBase];
			if (ch == indelSoft) then ch= indelHard; //look better for output...

			if (fUseColor) CASE ch OF
				'A','a': RGBForeColor( gAcolor);
				'C','c': RGBForeColor( gCcolor);
				'G','g': RGBForeColor( gGcolor);
				'T','t',
				'U','u': RGBForeColor( gTcolor);
				otherwise
								 RGBForeColor( gNcolor);
				}			else
				RGBForeColor( gNcolor);
												
			cw= (nucwidth - charwidth(ch)) / 2;
			MoveTo( rowleft+cw, rowbot);
			DrawChar(ch);
			TextMode(srcOr); //? srcOr/srcCopy
			}
	}  

{
	linesPerParag= kSeqLinesPerParagraph + 2; // 5 seq + top index + top spacer
	atSeq	 = ((aCell.v-1) % linesPerParag) - 1;
	
	if ((atSeq < 0) || (atSeq > kSeqLinesPerParagraph)) {
		//spacer line
		doTopIndex= FALSE;
		doSequence= FALSE;
		seqLine= ((aCell.v-1) / linesPerParag) * kSeqLinesPerParagraph + atSeq+1; //???
		}	else if ((atSeq == 0)) {
		doTopIndex= TRUE;
		doSequence= FALSE;
		seqLine= ((aCell.v-1) / linesPerParag) * kSeqLinesPerParagraph + atSeq; //???
		}	else {
		//sequence line
		doTopIndex= FALSE;
		doSequence= TRUE;
		seqLine= ((aCell.v-1) / linesPerParag) * kSeqLinesPerParagraph + atSeq-1; //???
		}

	if ((aCell.h == 1)) {
		//nameleft
		atBase= fFirstBase + min( fNbases, seqLine * fBasesPerLine);
		if (doSequence && fDoLeftName) DrawName( atSeq, 0, atBase);
		}	else if ((aCell.h == 2)) {
		//indexLeft
		atBase= fFirstBase + min( fNbases, seqLine * fBasesPerLine);
		if (doSequence && fDoLeftIndex) DrawSideIndex( atBase, kNucBorder);
		}		
	else if ((aCell.h < fBasesPerLine+3)) {
		//bases
		atBase= (seqLine * fBasesPerLine) + aCell.h - 2;
		if ((atBase <= fNBases)) {
			atBase= fFirstBase - 1 + atBase; 	//atbase == 0  for first 
			if (doSequence) DrawNuc( atSeq, atBase)
			else if (doTopIndex && fDoTopIndex) DrawTopIndex( atBase);
			}
		}		
	else if ((aCell.h == fBasesPerLine+3)) {
		//indexRight
		atBase= fFirstBase - 1 + min( fNBases, (seqLine+1) * fBasesPerLine);
		if (doSequence && fDoRightIndex) DrawSideIndex( atBase, 0);
		}	else if ((aCell.h == fBasesPerLine+4)) {
		//nameRight
		atBase= fFirstBase - 1 + min( fNBases, (seqLine+1) * fBasesPerLine);
		if (doSequence && fDoRightName) DrawName( atSeq, kNucBorder, atBase);
		}
}



pascal void TSeqPrintView::CalcMinFrame(VRect VAR minFrame)
{
	inherited::CalcMinFrame( minFrame);
}




pascal void TSeqPrintView::DoMenuCommand(CommandNumber aCommandNumber) // override 
{
	switch (aCommandNumber) {
		cCopy: {
				this->WriteToDeskScrap(); 
				gClipboardMgr->CheckDeskScrap(); //! this notifies app of changed scrap 
				}
		otherwise
			inherited::DoMenuCommand(aCommandNumber);
		}
}


/*******
pascal TCommand TSeqPrintView::DoMouseCommand(Point VAR theMouse, EventInfo VAR info,
								   Point VAR hysteresis)
VAR
		FailInfo		fi;

	pascal void HdlInitCmdFailed(OSErr error, long message)
		{
		FreeIfObject(protoSeqPrint);
		protoSeqPrint = NULL;
		}

	{					
	DoMouseCommand = NULL;

	fClickPt = theMouse;
	if (palette->fCurrSeqPrint > 0) { // draw mode
		FailSpaceIsLow(); 								// Make sure we aren't low on memory 

		Deselect();

		//Clone appropriate SeqPrint

		protoSeqPrint = TSeqPrint(gSeqPrintsArray[palette->fCurrSeqPrint].Clone);
		FailNil(protoSeqPrint);

		CatchFailures(fi, HdlInitCmdFailed);
		// Make sure cloning the SeqPrint left us with enough memory to continue.
		FailSpaceIsLow();

		New(SeqPrintSketcher);
		FailNil(SeqPrintSketcher);
		SeqPrintSketcher->ISeqPrintSketcher(this, protoSeqPrint, info.theOptionKey);
		Success(fi);
		DoMouseCommand = SeqPrintSketcher;
		}			
		
	else {	 	//select mode
		SeqPrintUnderMouse = NULL;
		fSeqPrintDocument->EachVirtualSeqPrintDo(CheckSeqPrint);

		if (SeqPrintUnderMouse == NULL)	{		//area select
			if (!info.theShiftKey)
				Deselect();
			New(SeqPrintSelector);
			FailNil(SeqPrintSelector);
			SeqPrintSelector->ISeqPrintSelector(cMouseCommand, this);
			DoMouseCommand = SeqPrintSelector;
			} 

		else {										//SeqPrint select/move/...
			if (!(SeqPrintUnderMouse->fIsSelected || info.theShiftKey))
				Deselect();

			if (info.theShiftKey) {
				SeqPrintUnderMouse->fIsSelected = !SeqPrintUnderMouse->fIsSelected;
				if (SeqPrintUnderMouse->fIsSelected)
					SeqPrintUnderMouse->Highlight(hlOff, hlOn)
				else
					SeqPrintUnderMouse->Highlight(hlOn, hlOff);
				}			else if (!SeqPrintUnderMouse->fIsSelected)
				{
				SeqPrintUnderMouse->fIsSelected = TRUE;
				DoHighlightSelection(hlOff, hlOn);
				}

			if (SeqPrintUnderMouse->fIsSelected) {
				New(SeqPrintDragger);
				FailNil(SeqPrintDragger);
				SeqPrintDragger->ISeqPrintDragger(this);
				DoMouseCommand = SeqPrintDragger;
				}
			//else, fall-through, we return NULL
			}	 
		}					 
}										
**********/



pascal void TSeqPrintView::DoSetupMenus(void)
	{
		short		i;
		Boolean		anySelection;
		Boolean		haveMemory;
		MenuHandle		aMenuHandle;
		short		item;
		Str255		itemName;


	inherited::DoSetupMenus();

	anySelection = FALSE;
	haveMemory = !MemSpaceIsLow;

	Enable(cCopy, TRUE && haveMemory);
/*----	
	Enable(cCut, anySelection && haveMemory);
	if (haveMemory) CanPaste(kPrintClipType);
	Enable(cClear, anySelection);
-------*/
}			 

