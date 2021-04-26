// USeqPrint.aln.p 
// d.g.gilbert, 1991 

/*
	alignment print
	
*/

#pragma segment AlnPrint

CONST

	kAlnPrintWindowRSRCID == 1021;

/*-----
  kMaxNameWidth == 120;
	kNucSpace 	== 2;
	kNucBorder	== 6;
____*/	


// TAlnPrintDocument -----------------------------------------------


pascal void TAlnPrintDocument::IAlnPrintDocument(OSType fileType, TDocument parentDoc, 
					TSeqList		aAlnList; longint startbase, nbases)
VAR
		Boolean		needNums;
		TFile		aFile;
		
	pascal void CloneIt( TSequence aSeq)  
		//! CloneSeqList: used also in TAlnPasteCommand; make TSeqList method !?
	VAR 	aRow: integer;
				longint		start1, nbase1;
	{
		aRow= aAlnList->GetEqualItemNo(aSeq);
		aSeq= TSequence(aSeq->Clone());
		FailNIL( aSeq); 
				/*!? if fail, need to back out gracefully by 
					freeing already cloned seqs, BUT !NON-Cloned ones*/
		fAlnList->AtPut( aRow, aSeq);
		if (needNums) {
			aSeq->GetSelection( start1, nbase1);
			startbase= max(startbase, start1);
			nbases= min(nbases, nbase1);
			}
	}

{
	fAlnPrintView = NULL;
	fParentDoc= parentDoc;
	fAlnList= aAlnList;
	needNums= (nbases < 1);
	if (needNums) nbases= 9999999;
	aAlnList->Each(CloneIt);
	fFirstBase= startBase;
	fNBases= nbases;
	
	/*---
	New(aFile);
	aFile->IFile(...);
	aname= parentDoc.name + '.dotplot';
	vol= parentdoc.vol;
	dirid= parentdoc.dirid;
	err= aFile->SpecifyWithTrio( name/vol/dirid);
	----*/
	aFile= NewFile(fileType, kSAppSig, kUsesDataFork, kUsesRsrcFork, !kDataOpen, !kRsrcOpen);
	IFileBasedDocument(aFile, fileType);
	fSavePrintInfo = FALSE; //was TRUE;
}

pascal TFile TAlnPrintDocument::DoMakeFile(itsCommandNumber:CommandNumber); // override 
{
	DoMakeFile= NewFile(fScrapType, gApplication->fCreator, kUsesDataFork, kUsesRsrcFork, 
												!kDataOpen, !kRsrcOpen);
}

pascal void TAlnPrintDocument::FreeData(void)
{
	TObject(fAlnList) = FreeListIfObject(fAlnList); //We own this list
}


pascal void TAlnPrintDocument::Free(void) // override 
{
	FreeData();
	inherited::Free();
}


pascal void TAlnPrintDocument::Close(void) /* override */ 
{
		/* patches for prefwindow */		
	if ((fAlnPrintView->fUseColor) && (fColorButton!=NULL)) fColorButton->fHilite= TRUE;
	this->SetChangeCount(0); //so we don't always get Save? yes/no/canc dlog 
	inherited::Close();
}



pascal void TAlnPrintDocument::DoMakeViews(Boolean forPrinting) // override 
VAR
		aWindow	: TWindow;
		minSize	: Point;
		maxSize	: Point;
		vSize 	: Vpoint;
{
	aWindow = gViewServer->NewTemplateWindow(kAlnPrintWindowRSRCID, this);
	FailNil(aWindow);
	
	fAlnPrintWind = TAlnPrintWind(aWindow);
	fAlnPrintWind->IAlnPrintWind();
	fAlnPrintWind->fCommonNumbers= TNumberText(aWindow->FindSubView('nPCT'));
	fAlnPrintWind->fRedrawBut= TButton(aWindow->FindSubView('bRDR'));
	fAlnPrintWind->fUserSelect= TButton(aWindow->FindSubView('bUsr'));
	fAlnPrintWind->fUserNotSelect= TButton(aWindow->FindSubView('bNot'));
	fAlnPrintWind->fUserClear= TButton(aWindow->FindSubView('bClr'));
	
	fAlnPrintView = TAlnPrintView(aWindow->FindSubView('PrVw'));
	FailNil(fAlnPrintView);

	fAlnPrintView->fCommonNumbers= fAlnPrintWind->fCommonNumbers;
	fAlnPrintView->InstallControls();
	
	fColorButton= TIcon(aWindow->FindSubView('iClr'));
	fColorButton->fEventNumber= mColorButHit;
	fMonoButton= TIcon(aWindow->FindSubView('iB&W'));
	fMonoButton->fEventNumber= mMonoButHit;
	fAlnPrintView->fUseColor= fColorButton->fHilite;  
	fColorButton->fHilite= FALSE;
	fMonoButton->fHilite= FALSE;
	if (!gConfiguration.hasColorQD) {
		fColorButton->ViewEnable(FALSE, kDontRedraw);
		fColorButton->Show(FALSE, kDontRedraw);
		fMonoButton->ViewEnable(FALSE, kDontRedraw);
		fMonoButton->Show(FALSE, kDontRedraw);
		fAlnPrintView->fUseColor	= FALSE;
		}
	
	// get our damn data into view 
	fAlnPrintView->AddSeqs();
	fAlnPrintView->SetDrawOptions();
	
	// set window's resize limits so it can't become wider than the AlnPrintview's edge 
	WITH aWindow->fResizeLimits){
		minSize = topLeft;
		maxSize = botRight;
		}
	WITH maxSize)h = Min( 2 + fAlnPrintView->fSize.h + kSBarSizeMinus1, h);
	//-- aWindow->SetResizeLimits(minSize, maxSize); << bad ??

	//?! need to show TEView in PrintView... ?
	this->ShowReverted();   
}


pascal void TAlnPrintDocument::UntitledName(Str255 VAR noName) // override 
//called AFTER .Idoc and .MakeViews 
{
	fParentDoc->GetTitle( noName);
	noName= Concat(noName,' PrettyPrint'); 

	if ((fWindowList != NULL) && (fWindowList.GetSize > 0)) 		
		TWindow(fWindowList->First())->SetTitle(noName);
}



pascal void TAlnPrintDocument::DoNeedDiskSpace(TFile itsFile,
															long VAR dataForkBytes, rsrcForkBytes)
{
	//)!get Print record requirements 
	//- inherited::DoNeedDiskSpace(dataForkBytes, rsrcForkBytes); 
	
	dataForkBytes = dataForkBytes + kMacdrawHeaderSize  /*+ sizeof window pict */;
	
	/*-- if you really want to know pict size:
	fAlnPrintView->WriteToDeskScrap(); 
	len= fAlnPrintView->GivePasteData( NULL, 'PICT');
	rsrcForkBytes = rsrcForkBytes + kRsrcTypeOverhead + kRsrcOverhead + sizeof(DocState);
	---*/
}


pascal void TAlnPrintDocument::DoRead(aFile:TFile; Boolean forPrinting)
{
	//-- inherited::DoRead(aRefNum, rsrcExists, forPrinting);)!read print info stuff
	// This is a Write-Only document == PICT of output drawing, no reading... ?
}


pascal void TAlnPrintDocument::DoWrite(TFile aFile, Boolean makingCopy)
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

	fAlnPrintView->WriteToDeskScrap();
	
	hPict= NewHandle(0);
	CatchFailures(fi, HdlDoWrite);
	len= fAlnPrintView->GivePasteData( hPict, 'PICT');
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


// TAlnPrintWind ------------------------- 

pascal void TAlnPrintWind::IAlnPrintWind(void)
{
	IPrefWindow();
	fWantSave= TRUE;
}

pascal void TAlnPrintWind::SetPrefID(void) /* override */ 
{
	gPrefWindID= kAlnPrintWindowRSRCID; gPrefWindName= 'TAlnPrintWind';
}

pascal void TAlnPrintWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
VAR
		TAlnPrintView		aAlnPrintView;

	pascal void CheckPercent(void)
	{
		if ((fCommonNumbers.GetValue!=aAlnPrintView->fCommonPercent)) {
			aAlnPrintView->AddSeqs();
			//- aAlnPrintView->ForceRedraw(); 
			}
	}
	
{
	aAlnPrintView= TAlnPrintDocument(fDocument).fAlnPrintView;
	//- aAlnPrintView->SetDrawOptions();

	switch (eventNumber) {
		mColorButHit: {
				aAlnPrintView->fUseColor= TRUE;
				aAlnPrintView->ForceRedraw();
				}
		mMonoButHit: {
				aAlnPrintView->fUseColor= FALSE;
				aAlnPrintView->ForceRedraw();
				}
								
		mButtonHit: if ((source == fRedrawBut)) {
				CheckPercent();
				aAlnPrintView->SetDrawOptions();
				aAlnPrintView->ForceRedraw();
				}				
			else if ((source == fUserSelect)) {
				if (aAlnPrintView->fUserSelection=NULL) 
					aAlnPrintView->fUserSelection= NewRgn;
				FailNIL( aAlnPrintView->fUserSelection);
				UnionRgn( aAlnPrintView->fSelections, aAlnPrintView->fUserSelection, 
						aAlnPrintView->fUserSelection);
				aAlnPrintView->ForceRedraw();				
				}				
			else if ((source == fUserNotSelect)) {
				if (aAlnPrintView->fUserSelection!=NULL) {
					DiffRgn( aAlnPrintView->fUserSelection, aAlnPrintView->fSelections, 
							aAlnPrintView->fUserSelection);
					aAlnPrintView->ForceRedraw();	
					}
				}
			else if ((source == fUserClear)) {
				if (aAlnPrintView->fUserSelection!=NULL) {
					DisposeRgn( aAlnPrintView->fUserSelection);
					aAlnPrintView->fUserSelection= NULL;
					aAlnPrintView->ForceRedraw();				
					}
				}				
			else 
				inherited::DoEvent(eventNumber,source, event); 
	
		otherwise
			inherited::DoEvent(eventNumber,source, event); 
		}
}




// TAlnPrintView -------------------------------------

pascal void TAlnPrintView::Free(void) // override 
{
	Handle(fCommonBases)= DisposeIfHandle(fCommonBases);
	Handle(fFirstCommon)= DisposeIfHandle(fFirstCommon);
	inherited::Free();
}

pascal void TAlnPrintView::AddSeqs(void)
VAR  hFirst: CharsHandle;
{
	Handle(fCommonBases)= DisposeIfHandle(fCommonBases);
	Handle(fFirstCommon)= DisposeIfHandle(fFirstCommon);
	if (fCommonNumbers == NULL) fCommonPercent= 80
	else fCommonPercent= fCommonNumbers->GetValue();  
	fCommonBases= fAlnList->FindCommonBases(fCommonPercent, hFirst); 
	fFirstCommon= hFirst;
}

pascal void TAlnPrintView::Initialize(void) // override 
//called by IObject/via IView...
CONST
	//- kBasesPerLine == 40;
	kTenSpacer 	== 10;

{
	inherited::Initialize();
	fAlnPrintDocument = NULL;
	fFirstBase= 0;  
	fNbases = 0;  
	fAlnList= NULL;  
	fNumSeqs= 0;
	
	fUserSelection= NULL;
	fCommonBases= NULL;
	fFirstCommon= NULL;
	fStyleName= NULL;
	fStyleBase= NULL;
	fStyleNums= NULL;
	fUseColor= TRUE;
	fDoUseShading= TRUE;
	fDoUseMatch= FALSE;
	fDoTopIndex= TRUE;
	fDoLeftIndex= FALSE;
	fDoRightIndex= FALSE;
	fDoLeftName= TRUE;
	fDoRightName= FALSE;
	fBasesPerLine= kBasesPerLine;
	fTenSpacer= kTenSpacer;
	fNameWidth= kMaxNameWidth;
	fIndexWidth= kMaxNameWidth;
	fShadeStyle= kShadeInvert;
	fMatchChar= '.';
}

pascal void TAlnPrintView::InstallControls(void)
VAR  aWindow	: TWindow;
{
	aWindow= GetWindow;

	fShadePop= TPopup(aWindow->FindSubView('pSHD'));
	fTopIndex= TCheckBox(aWindow->FindSubView('cTop'));
	fLeftIndex= TCheckBox(aWindow->FindSubView('cLft'));
	fRightIndex= TCheckBox(aWindow->FindSubView('cRgt'));
	fLeftName= TCheckBox(aWindow->FindSubView('nLft'));
	fRightName= TCheckBox(aWindow->FindSubView('nRgt'));
	fUseShade= TCheckBox(aWindow->FindSubView('shad'));
	fUseMatch= TCheckBox(aWindow->FindSubView('symb'));
	fMatchSym= TEditText(aWindow->FindSubView('eSym'));
	
	fStyleName= TDlogTextView(aWindow->FindSubView('tNam'));
	fStyleBase= TDlogTextView(aWindow->FindSubView('tBas'));
	fStyleNums= TDlogTextView(aWindow->FindSubView('tNum'));

	if (fStyleName!=NULL) fStyleName->SetText('Names');
	if (fStyleBase!=NULL) fStyleBase->SetText('Bases');
	if (fStyleNums!=NULL) fStyleNums->SetText('Index');
}


pascal void TAlnPrintView::IAlnPrintView( TAlnPrintDocument itsDocument, Boolean forClipboard)
VAR
		itsSize		: VPoint;
		aHandler	: TStdPrintHandler;
		sd				: SizeDeterminer;
{
	fAlnPrintDocument = TAlnPrintDocument(itsDocument);
	fFirstBase= fAlnPrintDocument->fFirstBase;  
	fNbases = fAlnPrintDocument->fNbases;  
	fAlnList= fAlnPrintDocument->fAlnList;  
	fNumSeqs= fAlnList->GetSize();
	
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
		aHandler->IStdPrintHandler(itsDocument, this, !kSquareDots, kFixedSize, !kFixedSize);		 
		}
}

 

pascal void TAlnPrintView::DoPostCreate(TDocument itsDocument) // override 
VAR  aHandler: TStdPrintHandler;
{
	inherited::DoPostCreate( itsDocument);
	fAlnPrintDocument = TAlnPrintDocument(itsDocument);
	fFirstBase= fAlnPrintDocument->fFirstBase;  
	fNbases = fAlnPrintDocument->fNbases;  
	fAlnList= fAlnPrintDocument->fAlnList;  
	fNumSeqs= fAlnList->GetSize();
			
	New(aHandler);
	FailNil(aHandler);
	aHandler->IStdPrintHandler(itsDocument, this, !kSquareDots, kFixedSize, !kFixedSize);	 
}
 


pascal void TAlnPrintView::FindNameWidth(void)
VAR    
		linesPerParagraph, numLinesPerSeq,
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
	fAlnList->Each(GetNameWidth);
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
	
	linesPerParagraph= fNumSeqs + 2; //+ top index + bottom spacer
	numLinesPerSeq= (fBasesPerLine - 1 + fNBases) / fBasesPerLine;
	
	DelRowLast( fNumOfRows);
	InsRowLast( linesPerParagraph * numLinesPerSeq, maxDeep);
	
	SetPort( savePort);
}


pascal void TAlnPrintView::SetDrawOptions(void) 
VAR		
		integer		wid, i ;
		Str255		aStr;
{
	FindNameWidth();

	if (fShadePop == NULL) fShadeStyle= kShadeInvert 
	else fShadeStyle= fShadePop->GetCurrentItem();  

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

	if (fUseShade == NULL) fDoUseShading= FALSE else fDoUseShading= fUseShade->IsOn();
	if (fUseMatch == NULL) fDoUseMatch= FALSE else fDoUseMatch= fUseMatch->IsOn();
	if (fDoUseMatch) {
		fMatchSym->GetText( aStr);
		fMatchChar= aStr[1];
		}
}


pascal void TAlnPrintView::DrawRangeOfCells(GridCell startCell, stopCell, VRect aRect) 
					// override 
{
	laserLine( 2, 4); // set laserline smaller ...
	//- SetDrawOptions; 

	inherited::DrawRangeOfCells( startCell, stopCell, aRect);
}



pascal void TAlnPrintView::DrawCell(GridCell aCell, VRect aRect) // override 
CONST
	kFontDescent == 2;
	kIndexRise == 1;
VAR
	longint		atBase ;
	integer		linesPerParagraph ;
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
		aSeq= fAlnList->SeqAt( atSeq);
		if (aSeq!=NULL) {
	//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
			MoveTo( aRect.left+rightBorder, aRect.bottom-kFontDescent);
			aName= aSeq->fName;
			DrawString(aName);
			} 
	}

	pascal void DrawNuc(integer atSeq, longint atBase, GridCell atCell)
	VAR
 		integer		rowbot, nucwidth, cw, rowleft;
		Boolean		firstSeq, useSym, isUserShaded, isCommonBase;
		hSeq		: CharsHandle;
		ch			: char;
		aSeq		: TSequence;
		myRect	: Rect;
	{
		aSeq= fAlnList->SeqAt( atSeq);
		if (aSeq!=NULL) {
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fBaseStyle);  /*$POP
			nucwidth= fBaseWidth; //aRect.right - aRect.left; 
			rowleft= aRect.left;
			rowbot = aRect.bottom - kFontDescent;  
			VRectToRect( aRect, myRect);
			myRect.right= myRect.left + fBaseWidth; //fix for extra spacers
	 		firstSeq= (atSeq == 1);
			handle(hSeq)= aSeq->fBases;
			ch= (*hSeq)^[atBase];
			if (ch == indelSoft) then ch= indelHard; //look better for output...
			useSym= FALSE;
			isCommonBase= (fCommonBases!=NULL) && (UprChar(ch) == (*fCommonBases)^[atbase]);
			isUserShaded= (fUserSelection!=NULL) && (PtInRgn( atCell, fUserSelection));
			
			if ((fDoUseMatch) && (isCommonBase)) {
				//- if (!firstSeq) useSym= TRUE; *//*< THIS IS Bad, need fix for FirstDrawn...
				if ((fFirstCommon!=NULL) && (ORD((*fFirstCommon)^[atbase]) < atSeq)) useSym= TRUE;
				}
								
			if ((isUserShaded) || ((fDoUseShading) && (isCommonBase))) 
				switch (fShadeStyle) {
					kShadeGray25	 : { RGBForeColor( gGray25); PaintRect(myRect); }
					kShadeGray75	 : { RGBForeColor( gGray75); PaintRect(myRect); }
					kShadeGray50	 : { RGBForeColor( gGray50); PaintRect(myRect); }
					kShadeStipple50: { RGBForeColor( gNcolor); FillRect(myRect, gray); }
					kShadeStipple75: { RGBForeColor( gNcolor); FillRect(myRect, dkGray); }
					kShadeStipple25: { RGBForeColor( gNcolor); FillRect(myRect, ltGray); }
					}

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
			
			if ((isUserShaded) || ((fDoUseShading) && (isCommonBase))) 
				switch (fShadeStyle) {
					kShadeInvert: {
						PaintRect(myRect); 
						TextMode(srcBIC);
						}
					}
									
			cw= (nucwidth - charwidth(ch)) / 2;
			MoveTo( rowleft+cw, rowbot);
			if (useSym) DrawChar(fMatchChar) else DrawChar(ch);
			TextMode(srcOr); //? srcOr/srcCopy
			}
	}  
 
{
	linesPerParagraph= fNumSeqs + 2; //+ top index + top spacer
	seqLine=  (aCell.v-1) / linesPerParagraph;
	atSeq	 = ((aCell.v-1) % linesPerParagraph) - 1;
	
	if ((atSeq < 0) || (atSeq > fNumSeqs)) {
		//spacer line
		doTopIndex= FALSE;
		doSequence= FALSE;
		}	else if ((atSeq == 0)) {
		doTopIndex= TRUE;
		doSequence= FALSE;
		}	else {
		//sequence line
		doTopIndex= FALSE;
		doSequence= TRUE;
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
			if (doSequence) DrawNuc( atSeq, atBase, aCell)
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



pascal void TAlnPrintView::CalcMinFrame(VRect VAR minFrame)
{
	inherited::CalcMinFrame( minFrame);
}




pascal void TAlnPrintView::DoMenuCommand(CommandNumber aCommandNumber) // override 
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
pascal TCommand TAlnPrintView::DoMouseCommand(Point VAR theMouse, EventInfo VAR info,
								   Point VAR hysteresis)
VAR
		FailInfo		fi;

	pascal void HdlInitCmdFailed(OSErr error, long message)
		{
		FreeIfObject(protoAlnPrint);
		protoAlnPrint = NULL;
		}

	{					
	DoMouseCommand = NULL;

	fClickPt = theMouse;
	if (palette->fCurrAlnPrint > 0) { // draw mode
		FailSpaceIsLow(); 								// Make sure we aren't low on memory 

		Deselect();

		//Clone appropriate AlnPrint

		protoAlnPrint = TAlnPrint(gAlnPrintsArray[palette->fCurrAlnPrint].Clone);
		FailNil(protoAlnPrint);

		CatchFailures(fi, HdlInitCmdFailed);
		// Make sure cloning the AlnPrint left us with enough memory to continue.
		FailSpaceIsLow();

		New(AlnPrintSketcher);
		FailNil(AlnPrintSketcher);
		AlnPrintSketcher->IAlnPrintSketcher(this, protoAlnPrint, info.theOptionKey);
		Success(fi);
		DoMouseCommand = AlnPrintSketcher;
		}			
		
	else {	 	//select mode
		AlnPrintUnderMouse = NULL;
		fAlnPrintDocument->EachVirtualAlnPrintDo(CheckAlnPrint);

		if (AlnPrintUnderMouse == NULL)	{		//area select
			if (!info.theShiftKey)
				Deselect();
			New(AlnPrintSelector);
			FailNil(AlnPrintSelector);
			AlnPrintSelector->IAlnPrintSelector(cMouseCommand, this);
			DoMouseCommand = AlnPrintSelector;
			} 

		else {										//AlnPrint select/move/...
			if (!(AlnPrintUnderMouse->fIsSelected || info.theShiftKey))
				Deselect();

			if (info.theShiftKey) {
				AlnPrintUnderMouse->fIsSelected = !AlnPrintUnderMouse->fIsSelected;
				if (AlnPrintUnderMouse->fIsSelected)
					AlnPrintUnderMouse->Highlight(hlOff, hlOn)
				else
					AlnPrintUnderMouse->Highlight(hlOn, hlOff);
				}			else if (!AlnPrintUnderMouse->fIsSelected)
				{
				AlnPrintUnderMouse->fIsSelected = TRUE;
				DoHighlightSelection(hlOff, hlOn);
				}

			if (AlnPrintUnderMouse->fIsSelected) {
				New(AlnPrintDragger);
				FailNil(AlnPrintDragger);
				AlnPrintDragger->IAlnPrintDragger(this);
				DoMouseCommand = AlnPrintDragger;
				}
			//else, fall-through, we return NULL
			}	 
		}					 
}										
**********/



pascal void TAlnPrintView::DoSetupMenus(void)
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

