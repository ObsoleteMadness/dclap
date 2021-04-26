// USeqPrint.remap.p 
// d.g.gilbert, 1991 

/*
		restrict map print
*/
	
#pragma segment Remap


CONST
	kREMapWindowRSRCID == 1022;



// TREMapDocument -----------------------------------------------


pascal void TREMapDocument::IREMapDocument(OSType fileType, TDocument parentDoc, 
													TSequence		aSeq; longint startbase, nbases)
VAR  aFile: TFile;
{
	fREMapView= NULL;
	fSeq= NULL;
	fParentDoc= parentDoc;
	FailNIL( aSeq);
	fSeq= TSequence(aSeq->Clone());
	if (nbases<1) fSeq->GetSelection( startbase,nbases);
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
	IFileBasedDocument( aFile, fileType);
	fSavePrintInfo = FALSE; //was TRUE;
}

pascal TFile TREMapDocument::DoMakeFile(itsCommandNumber:CommandNumber); // override 
{
	DoMakeFile= NewFile(fScrapType, gApplication->fCreator, kUsesDataFork, kUsesRsrcFork, 
												!kDataOpen, !kRsrcOpen);
}

pascal void TREMapDocument::FreeData(void)
{
	if (fSeq!=NULL)	fSeq->Free(); fSeq= NULL;
	//-- !?!?! fREMapView.free should be done automatically by superview.free
	//- if ((fREMapView!=NULL)) fREMapView->Free();  fREMapView= NULL; 
}


pascal void TREMapDocument::Free(void) // override 
{
	FreeData();
	inherited::Free();
}


pascal void TREMapDocument::Close(void) /* override */ 
{
		/* patches for prefwindow */		
	if ((fREMapView->fUseColor) && (fColorButton!=NULL)) fColorButton->fHilite= TRUE;
	this->SetChangeCount(0); //so we don't always get Save? yes/no/canc dlog 
  inherited::Close();
}



pascal void TREMapDocument::DoMakeViews(Boolean forPrinting) // override 
VAR
		aWindow	: TWindow;
		minSize	: Point;
		maxSize	: Point;
		vSize 	: Vpoint;
{
	aWindow = gViewServer->NewTemplateWindow(/*!*/kREMapWindowRSRCID, this);
	FailNil(aWindow);
	
	fREMapWind = TREMapWind(aWindow);
	fREMapWind->IREMapWind();
	fREMapWind->fRedrawBut= TButton(aWindow->FindSubView('bRDR'));
	
	fREMapView = TREMapView(aWindow->FindSubView('PrVw'));
	FailNil(fREMapView);
	fREMapView->InstallControls();

	fColorButton= TIcon(aWindow->FindSubView('iClr'));
	fColorButton->fEventNumber= mColorButHit;
	fMonoButton = TIcon(aWindow->FindSubView('iB&W'));
	fMonoButton->fEventNumber= mMonoButHit;
	fREMapView->fUseColor= fColorButton->fHilite;  
	fColorButton->fHilite= FALSE;
	fMonoButton->fHilite= FALSE;
	if (!gConfiguration.hasColorQD) {
		fColorButton->ViewEnable(FALSE, kDontRedraw);
		fColorButton->Show(FALSE, kDontRedraw);
		fMonoButton->ViewEnable(FALSE, kDontRedraw);
		fMonoButton->Show(FALSE, kDontRedraw);
		fREMapView->fUseColor	= FALSE;
		}
	
		// get our damn data into view 
	/*!*/ 
	fREMapView->AddSeqs();
	fREMapView->SetDrawOptions();
	
	// set window's resize limits so it can't become wider than the REMapview's edge 
	WITH aWindow->fResizeLimits){
		minSize = topLeft;
		maxSize = botRight;
		}
	WITH maxSize)h = Min( 2 + fREMapView->fSize.h + kSBarSizeMinus1, h);
	//-- aWindow->SetResizeLimits(minSize, maxSize); << bad !!

	//?! need to show TEView in PrintView... ?
	this->ShowReverted();   
}


pascal void TREMapDocument::UntitledName(Str255 VAR noName) // override 
//!called AFTER .Idoc and .MakeViews 
{
	noName= Concat(fSeq->fName,' R.E.Map'); 
	if ((fWindowList != NULL) && (fWindowList.GetSize > 0)) 		
		TWindow(fWindowList->First())->SetTitle(noName);
}



pascal void TREMapDocument::DoNeedDiskSpace(TFile itsFile,
															long VAR dataForkBytes, rsrcForkBytes)
{
	//)!get Print record requirements 
	//- inherited::DoNeedDiskSpace(dataForkBytes, rsrcForkBytes); 
	
	dataForkBytes = dataForkBytes + kMacdrawHeaderSize  /*+ sizeof window pict */;
	
	/*-- if you really want to know pict size:
	fREMapView->WriteToDeskScrap(); 
	len= fREMapView->GivePasteData( NULL, 'PICT');
	rsrcForkBytes = rsrcForkBytes + kRsrcTypeOverhead + kRsrcOverhead + sizeof(DocState);
	---*/
}


pascal void TREMapDocument::DoRead(aFile:TFile; Boolean forPrinting)
{
	//-- inherited::DoRead(aRefNum, rsrcExists, forPrinting);)!read print info stuff
	// This is a Write-Only document == PICT of output drawing, no reading... ?
}


pascal void TREMapDocument::DoWrite(TFile aFile, Boolean makingCopy)
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

	fREMapView->WriteToDeskScrap();
	
	hPict= NewHandle(0);
	CatchFailures(fi, HdlDoWrite);
	len= fREMapView->GivePasteData( hPict, 'PICT');
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




// TREMapWind ------------------------- 

pascal void TREMapWind::IREMapWind(void)
{
	IPrefWindow();
	fWantSave= TRUE;
}

pascal void TREMapWind::SetPrefID(void) /* override */ 
{
	gPrefWindID= kREMapWindowRSRCID; gPrefWindName= 'TREMapWind';
}


pascal void TREMapWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
VAR
		TREMapView		aREMapView;

{
	aREMapView= TREMapDocument(fDocument).fREMapView;
	//- aREMapView->SetDrawOptions();

	switch (eventNumber) {
		mColorButHit: {
				aREMapView->fUseColor= TRUE;
				aREMapView->ForceRedraw();
				}
		mMonoButHit: {
				aREMapView->fUseColor= FALSE;
				aREMapView->ForceRedraw();
				}
		mButtonHit: if ((source == fRedrawBut)) {
				aREMapView->SetDrawOptions();
				aREMapView->ForceRedraw();
				}			else 
				inherited::DoEvent(eventNumber,source, event); 

		otherwise
				inherited::DoEvent(eventNumber,source, event); 
		}
}






// TREMapView -------------------------------------


pascal void TREMapView::Free(void) // override 
{
	TObject(fREMap)= FreeIfObject(fREMap);
	Handle(fProt)= DisposeIfHandle(fProt);
	Handle(fCoProt)= DisposeIfHandle(fCoProt);
	inherited::Free();
}


pascal void TREMapView::Initialize(void) // override 
//called by IObject/via IView...
CONST
	//- kBasesPerLine == 40;
	kTenSpacer 	== 10;
{
	inherited::Initialize();

	fREMapDocument= NULL;
	fSeq= NULL;
	fFirstBase= 0;
	fNbases= 0;
	
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
	
	//! new for REMap.............
	fSeqRowHeight= 20;
	fLastZymeCut= 1;
	fREMap= NULL;
	fProt= NULL;
	fCoProt= NULL;
	
	fNoncutters= NULL;
	fCutpoints= NULL;
	fAllzymes= NULL;
	fShowCoseq= NULL;
	fShowindex= NULL;
	fMincuts= NULL;
	fMaxcuts= NULL;
			
	fAllZymesStart= 0;
	fAllZymesEnd= 0;
	fNoCutsStart= 0;
	fNoCutsEnd= 0;
	fCutPointStart= 0;
	fCutPointEnd= 0;

}


pascal void TREMapView::IREMapView( TREMapDocument itsDocument, Boolean forClipboard)
VAR
		itsSize		: VPoint;
		aHandler	: TStdPrintHandler;
		sd				: SizeDeterminer;
{
	fREMapDocument = TREMapDocument(itsDocument);
	fFirstBase= fREMapDocument->fFirstBase;  
	fNbases= fREMapDocument->fNbases;  
	fSeq= fREMapDocument->fSeq;  
	
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

 

pascal void TREMapView::DoPostCreate(TDocument itsDocument) // override 
VAR  aHandler: TStdPrintHandler;
{
	inherited::DoPostCreate( itsDocument);
	fREMapDocument= TREMapDocument(itsDocument);
	fFirstBase= fREMapDocument->fFirstBase;  
	fNbases= fREMapDocument->fNbases;  
	fSeq= fREMapDocument->fSeq;  
	New(aHandler);
	FailNil(aHandler);
	aHandler->IStdPrintHandler(itsDocument, this, !kSquareDots, kFixedSize, kFixedSize);	 
}
 
pascal void TREMapView::AddSeqs(void)
VAR
		atRow, i	 : integer;
		GridCell		c1, c2 ;
		h			: Handle;
		nums	: Str255;
		nb		: longint;
		pc		: CharsPtr;
{	
	fREMap= TREMap(gREMap->Clone()); //duplicate master w/ re lists
	fREMap->MapSeq( fSeq);
	fCutList= fREMap->fSeqCuts; //copy for easy use
	
	if (fNbases<1) then nb= GetHandleSize(fSeq->fBases)
	else nb= fNbases;
	pc= CharsPtr(fSeq.(*fBases));
	pc= &(*pc)[fFirstBase];
	fProt= Nucleic23Protein( pc, nb);
	pc= CharsPtr(fREMap->fCoSeq.(*fBases));
	pc= &(*pc)[fFirstBase];
	fCoProt= Nucleic23Protein( pc, nb);
}

pascal void TREMapView::InstallControls(void)
VAR  aWindow	: TWindow;
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
	
		//!--- added for REMap ---
	fNoncutters= TCheckBox(aWindow->FindSubView('cNOC'));
	fCutpoints= TCheckBox(aWindow->FindSubView('cCUT'));
	fAllzymes= TCheckBox(aWindow->FindSubView('cALL'));
	fShowCoseq= TCheckBox(aWindow->FindSubView('cCOS'));
	fMincuts = TNumberText(aWindow->FindSubView('nMIN'));
	fMaxcuts = TNumberText(aWindow->FindSubView('nMAX'));

	fSeqFrame1= TCheckBox(aWindow->FindSubView('cSF1'));
	fSeqFrame2= TCheckBox(aWindow->FindSubView('cSF2'));
	fSeqFrame3= TCheckBox(aWindow->FindSubView('cSF3'));
	fCoFrame1 = TCheckBox(aWindow->FindSubView('cCF1'));
	fCoFrame2 = TCheckBox(aWindow->FindSubView('cCF2'));
	fCoFrame3 = TCheckBox(aWindow->FindSubView('cCF3'));
	fThreeBase= TRadio(aWindow->FindSubView('r3aa'));
}


pascal void TREMapView::FindNameWidth(void)
VAR    
		linesPerParag, numLinesPerSeq,
		allzymeRows, allzymeHeight,  
		cutRows, cutHeight,
		nocutsRows, nocutsHeight,
		integer		maxNameWid, fontheight, maxdeep;
		GrafPtr		savePort;
		nums	: Str255;
		longint		nlines, need, lastBase;
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
	need= (fBasesPerLine+4) - fNumOfCols;
	if ((need>0))  InsColLast( need, fBaseWidth)
	else if ((need<0)) DelColLast( -need);
	
	//! num of rows == numLinesPerSeq, BUT each row may have different width !!
	//! add one header line at top?
	numLinesPerSeq= (fBasesPerLine - 1 + fNBases) / fBasesPerLine;
	linesPerParag= 11; // spacer + 3 pro + seq + index + coseq + 3copro + renzymes 
	
	fSeqRowHeight= maxdeep;
	nlines= 1 + numLinesPerSeq * linesPerParag; //header + sequence

/*****
	this->AllZymeTabRows( allzymeRows, allzymeHeight);
	fAllZymesStart= nlines+1;
	fAllZymesEnd= fAllZymesStart + allzymeRows-1;
	nlines= fAllZymesEnd;  //allzyme table

	this->NocuttersRows( nocutsRows, nocutsHeight); //!! BAD if nocutsRows != 0 !!!!!!!!!
	fNoCutsStart= nlines+1;
	fNoCutsEnd= fNoCutsStart + nocutsRows-1;
	nlines= fNoCutsEnd;  //nocutters table

	this->CutpointsRows( cutRows, cutHeight);
	fCutpointStart= nlines+1;
	fCutpointEnd= fCutpointStart + cutRows-1;
	nlines= fCutpointEnd;  //cutpoints table
******/

	nlines= nlines + 1; //footer
	
	need= nlines - fNumOfRows;
	if ((need>0)) InsRowLast( need, fSeqRowHeight)
	else if ((need<0)) DelRowLast( -need);
	
	SetRowHeight( 1, 1, 30+fSeqRowHeight);		//header
/***
	if ((allzymeRows>0)) SetRowHeight( fAllZymesStart, allzymeRows, allzymeHeight);
	if ((nocutsRows>0)) SetRowHeight( fNoCutsStart, nocutsRows, nocutsHeight);
	if ((cutRows>0)) SetRowHeight( fCutpointStart, cutRows, cutHeight);
***/
	SetRowHeight( nlines, 1, fSeqRowHeight); 	//? footer
	
	SetPort( savePort);
}


pascal void TREMapView::SetDrawOptions(void) 
VAR		
		wid, fixwid, i, atRow,
		first, ncuts, maxcuts,
		integer		startbase, endbase, atbase;
		Integer		numLinesPerSeq, linesPerParag, theRowHeight;
		allSeqFrames, allCoFrames,
		noSeqFrames, noCoFrames,
		doFrame1, doFrame2, doFrame3,
		doCoFrame1, doCoFrame2, doCoFrame3,
		Boolean		doCoSeq;
		aRect	: VRect;
		
	pascal void SetOneRow( integer VAR atRow, Boolean optionIsOn)
	integer		VAR  wid;
	{
		if (optionIsOn) wid= theRowHeight else wid= 0;
		SetRowHeight( atRow, 1, wid); 
		atRow= atRow+1;
	}
		

	pascal void AddTables(void)
	VAR
		allzymeRows, allzymeHeight,  
		cutRows, cutHeight,
		integer		nocutsRows, nocutsHeight ;
	{
		fAllZymesStart= fNumOfRows+1;
		this->AllZymeTabRows( allzymeRows, allzymeHeight);
		InsRowLast( allzymeRows, allzymeHeight);
		fAllZymesEnd= fNumOfRows;

		fNoCutsStart= fNumOfRows+1;
		this->NocuttersRows( nocutsRows, nocutsHeight);  
		InsRowLast( nocutsRows, nocutsHeight);
		fNoCutsEnd= fNumOfRows;
	
		fCutpointStart= fNumOfRows+1;
		this->CutpointsRows( cutRows, cutHeight);
		InsRowLast( cutRows, cutHeight);
		fCutpointEnd= fNumOfRows;
	}
	
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
	
	//! REMap -- SetRowWidth for just about each row...........

		// 11 rows for all options per seq. line 
	numLinesPerSeq= (fBasesPerLine - 1 + fNBases) / fBasesPerLine;
	linesPerParag= 11; // spacer + 3 pro + seq + index + coseq + 3copro + renzymes 
	atRow= 2; //skip header
	allSeqFrames= fSeqFrame1.IsOn && fSeqFrame2.IsOn && fSeqFrame3->IsOn();
	noSeqFrames= !fSeqFrame1.IsOn && !fSeqFrame2.IsOn && !fSeqFrame3->IsOn();
	if (!(allSeqFrames || noSeqFrames)) {
		doFrame1= fSeqFrame1->IsOn();
		doFrame2= fSeqFrame2->IsOn();
		doFrame3= fSeqFrame3->IsOn();
		}
	allCoFrames= fShowCoseq.IsOn && fCoFrame1.IsOn && fCoFrame2.IsOn && fCoFrame3->IsOn();
	noCoFrames= !fShowCoseq.IsOn && !fCoFrame1.IsOn && !fCoFrame2.IsOn && !fCoFrame3->IsOn();
	if (!(allCoFrames || noCoFrames)) {
		doCoSeq= fShowCoseq->IsOn();
		doCoFrame1= fCoFrame1->IsOn();
		doCoFrame2= fCoFrame2->IsOn();
		doCoFrame3= fCoFrame3->IsOn();
		}
		
	SetVRect( aRect, 1, 50, 1000, 100);
	FOR i= 1 TO numLinesPerSeq){

		theRowHeight= fSeqRowHeight;
		if (allSeqFrames) {
			SetRowHeight( atRow, 5, theRowHeight); 
			atRow= atRow+5;
			}		else if (noSeqFrames) {
			SetOneRow( atRow, TRUE);  //Spacer
			SetRowHeight( atRow, 3, 0); 
			atRow= atRow+3;
			SetOneRow( atRow, TRUE);  //fSeq
			}		else {
			SetOneRow( atRow, TRUE);  //Spacer
			SetOneRow( atRow, doFrame1); 
			SetOneRow( atRow, doFrame2); 
			SetOneRow( atRow, doFrame3); 
			SetOneRow( atRow, TRUE);  //fSeq
			}
		
		SetOneRow( atRow, fDoTopIndex);
		
		if (allCoFrames) {
			SetRowHeight( atRow, 4, theRowHeight); 
			atRow= atRow+4;
			}		else if (noCoFrames) {
			SetRowHeight( atRow, 4, 0); 
			atRow= atRow+4;
			}		else {
			SetOneRow( atRow, doCoSeq); 
			SetOneRow( atRow, doCoFrame1); 
			SetOneRow( atRow, doCoFrame2); 
			SetOneRow( atRow, doCoFrame3); 
			}
		
		startbase= fBasesPerLine * (i-1); //?? 0 or 1 origin
		endbase= startbase + fBasesPerLine - 1;
		this->DrawZymeLine( 3, startBase, endBase, aRect, FALSE, theRowHeight);
		SetOneRow( atRow, TRUE);  //Enzymes...
		}
	
	AddTables();
	
}


CONST 	kMaxZymeWidth ==  9;  

pascal void TREMapView::AllZymeTabRows( Integer VAR nRows, rowHeight)
VAR	nzymes, ncols: integer;
		fontinfo		fi ;
{
	if ((fAllzymes->IsOn())) {
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
		GetFontInfo(fi);
		rowHeight= 1 + fi.ascent + fi.descent + fi.leading;
		}
	else rowHeight= 0;
	nzymes= fREMap->fREnzymes->GetSize();
	ncols= fBasesPerLine / kMaxZymeWidth; 
	nRows= ((nzymes+ncols-1) / ncols) + 3; /*+ header + footer*2 */	
}

pascal void TREMapView::AllZymeTable( integer atRow, VRect aRect)
CONST
		kFontDescent == 2;
VAR		
		integer		h, v, zymesPerCol, rowHeight, nzymes, ncols, nrows;
		fi	: fontinfo;
		
	pascal void DrawHeader( integer h, v)
	{	
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
		//- TextSize(12); 
		TextFace([bold]);  
		moveto(h, v);
		DrawString('Site usage for all enzymes');
		v= v + kFontDescent; //fi.descent;
		moveto(h, v);
		Line( fBaseWidth * fBasesPerLine, 0);
	}
	
	pascal void DrawFooter( integer h, v)
	str255		VAR  aLine;
	{	
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
		moveto(h, v);
		DrawString('Total number of cuts: ');
		NumToString( fREMap->fCutCount, aLine);
		DrawString( aLine);
	}

	pascal void DrawInfoLine( integer h, v)
	VAR  
			str255		aLine;
			integer		i, tab, atZyme, numTab, chleft ;
			aZyme	: TREnzyme;
			
		pascal void drawInfo( TREnzyme zyme)
		{
			NumToString( zyme->fCutCount, aLine);
			moveto( chleft+numTab-StringWidth(aLine), v);
			DrawString(aLine);
			aLine= zyme->fName;
			moveto( chleft+numTab+5, v);
			DrawString(aLine);
		}
	
	{
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
		numTab= 2*fBaseWidth;
		chleft= h;
		tab= kMaxZymeWidth * fBaseWidth;
		atZyme= atRow;
		FOR i= 1 to ncols){
			//- atZyme= atRow + i*zymesPerCol;
			if (atZyme<= nzymes) {
				aZyme= TREnzyme( fREMap->fREnzymes->At( atZyme)); 
				if ((aZyme!=NULL)) drawInfo( aZyme);
				}
			atZyme= atZyme + zymesPerCol;
			chleft= chleft + tab;
			}
	}
	
	
{
	/*- this->AllZymeTabRows( nrows, rowHeight);*/ 
	nzymes= fREMap->fREnzymes->GetSize();
	ncols	= fBasesPerLine / kMaxZymeWidth; 
	zymesPerCol= (nzymes+ncols-1) / ncols;	
	GetFontInfo(fi);
	rowHeight= fi.ascent + fi.descent + fi.leading;
	h= aRect.left;
	//- v= aRect.top + fi.ascent; 
	v= aRect.bottom - kFontDescent; //??
	
	if ((atRow<1))
		DrawHeader( h, v)
	else if ((atRow=zymesPerCol+1))
		DrawFooter( h, v)
	else if ((atRow <= zymesPerCol))
		DrawInfoLine( h, v);
}


pascal void TREMapView::NocuttersRows( Integer VAR nRows, rowHeight)
VAR
		fontinfo		fi;
		longint		nzymes, ncols;
{
	if ((fNoncutters->IsOn())) {
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
		GetFontInfo(fi);
		rowHeight= 1 + fi.ascent + fi.descent + fi.leading;
		}
	else rowHeight= 0;
	nzymes= Max( 0, fREMap->fREnzymes.GetSize - fREMap->fCuttersCount);
	ncols	= fBasesPerLine / kMaxZymeWidth; 
	nRows= ((nzymes+ncols-1) / ncols) + 3;	
}


pascal void TREMapView::NocuttersTab( integer atRow, VRect aRect)
CONST
		kFontDescent == 2;
VAR		
			integer		h, v, zymesPerCol, totalzymes, rowHeight, nzymes, ncols, nrows;
			fi	: fontinfo;
		
	pascal void DrawHeader( integer h, v)
	{	
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
		//- TextSize(12); 
		TextFace([bold]);  
		moveto(h, v);
		DrawString('Enzymes that did not cut');
		v= v + kFontDescent; //fi.descent;
		moveto(h, v);
		Line( fBaseWidth * fBasesPerLine, 0);
	}
	
	pascal void DrawFooter( integer h, v)
	{	

	}

	pascal void DrawInfoLine( integer h, v)
	VAR  
			str255		aLine;
			integer		izyme,  kzyme, i, tab, atZyme, numTab, chleft ;
			aZyme	: TREnzyme;
			okay	: boolean;
			
		pascal void drawInfo( TREnzyme zyme)
		{
			if ((zyme->fCutCount=0)) {
				moveto( chleft, v);
				aLine= zyme->fName;
				DrawString(aLine);
				}
		}
	
	{
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
		numTab= 2*fBaseWidth;
		chleft= h;
		tab= kMaxZymeWidth * fBaseWidth;
		atZyme= atRow;
		iZyme = atZyme-1;
		kZyme = atRow;
		FOR i= 0 to ncols-1){
			atZyme= atRow + i*zymesPerCol; 
			do {
				aZyme= TREnzyme( fREMap->fREnzymes->At( kZyme)); 
				if ((aZyme!=NULL) && (aZyme->fCutCount=0)) {
					iZyme= iZyme+1;
					okay= (iZyme == atZyme);
					}				else 
					okay= FALSE;
				kZyme= kZyme+1;
			} while (!(okay || (kZyme > totalzymes)));
			if (okay) {
				drawInfo( aZyme);
				chleft= chleft + tab;
				}
			}
	}
	
{
	ncols	= fBasesPerLine / kMaxZymeWidth; 
	this->NocuttersRows( nrows, rowHeight);  
	//- zymesPerCol= nrows - 3;
	totalzymes= fREMap->fREnzymes->GetSize();
	nzymes= Max( 0, totalzymes - fREMap->fCuttersCount); 
	zymesPerCol= (nzymes+ncols-1) / ncols;	 
	GetFontInfo(fi);
	h= aRect.left;
	//- v= aRect.top + fi.ascent; 
	v= aRect.bottom - kFontDescent;
	
	if ((atRow<1))
		DrawHeader( h, v)
	else if ((atRow=zymesPerCol+1))
		DrawFooter( h, v)
	else if ((atRow <= zymesPerCol))
		DrawInfoLine( h, v);
}




pascal void TREMapView::CutpointsRows( Integer VAR nRows, rowHeight)
{
	//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
	if ((fCutpoints->IsOn())) rowHeight= cHeight
	else rowHeight= 0;
	nRows= 4;
}

pascal void TREMapView::CutpointsTab( integer atRow, VRect aRect)
CONST
		kFontDescent == 2;
VAR
		integer		zymesPerCol, nrows, rowHeight, h, v, ncols ;
		fontInfo		fi ;
		
	pascal void DrawHeader( integer h, v)
	{	
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
		TextFace([bold]);  
		moveto(h, v);
		DrawString('Cut points by enzyme');
		v= v + kFontDescent; //fi.descent;
		moveto(h, v);
		Line( fBaseWidth * fBasesPerLine, 0);
	}
	
	pascal void DrawFooter( integer h, v)
	{	
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
		moveto(h, v);
		DrawString('--- not ready yet ---');
	}
	
	pascal void DrawInfoLine( integer h, v)
	{
	}

{
	ncols	= fBasesPerLine / kMaxZymeWidth; 
	this->CutpointsRows( nrows, rowHeight);  
	zymesPerCol= nrows - 4; 
	GetFontInfo(fi);
	//- rowHeight= fi.ascent + fi.descent + fi.leading; 
	h= aRect.left;
	//- v= aRect.top + fi.ascent; 
	v= aRect.bottom - kFontDescent; //??
	
	if ((atRow<1))
		DrawHeader( h, v)
	else if ((atRow=zymesPerCol+1))
		DrawFooter( h, v)
	else if ((atRow <= zymesPerCol))
		DrawInfoLine( h, v);

}



pascal void TREMapView::DrawZymeLine( integer startCellh, startBase, endbase, VRect aRect,
																	 Boolean		doDraw; Integer VAR lineHeight)
/*==== 
		gctcggctgctgctcggctg
				||        | ||
			 abcI    cbaII
									hcgX
====*/
VAR
		RgnHandle		filledArea, aRgn;
		integer		atbot, cuts, wd, ws, rowbot, lasti, i, j, k;
		Point		at, lat;
		first,overlap	: boolean;
		lastzyme, zymes	: str63;	
		Rect		fillRect, bRect, cRect;
		integer		nameHeight, rowLeft, rowTop, rotBot;
		integer		atcellh, chleft, minCuts,maxCuts, firstCut, nCuts, cutIndx;
		
	pascal void spaceright(void)
	integer		VAR  chright;
	{
		//- chleft= chleft + fBaseWidth; *//* ? bad for wide columns -- use grid cell positions ?
		chright= chleft + GetColWidth(atCellh) - fColInset;
		chleft = chright + fColInset;
		atcellh= atcellh + 1;
	}

{
	firstCut= fLastZymeCut;
	fREMap->CutsAtBase( startBase, firstCut, ncuts);	//!? don't care if startbase has no cuts
	cutindx= firstCut;
	
	//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle/*fZymeStyle*/);  /*$POP

	atcellh= startcellh;
	nameHeight= cHeight;
	rowTop= aRect.top;
	rowleft= aRect.left;
	//rowTop= rowTop-4;
	rowBot= rowTop;
	filledArea= newRgn;
	aRgn= newRgn;
	if ((fMincuts!=NULL)) minCuts= fMinCuts.GetValue else minCuts= 1; //-1?
	if ((fMaxcuts!=NULL)) maxCuts= fMaxCuts.GetValue else maxCuts= maxint; //-1?
	
	chleft= rowLeft + kNucSpace;
	SetRect( fillRect, chleft, rowTop, chleft+1, rowTop+1);
	RectRgn( filledArea, fillRect); //must start w/ non-empty rgn
	MoveTo( chleft, rowTop);
	FOR i= startBase TO endbase){
		
		while ( (*fCutList)^[cutindx].fSeqindex < i do cutindx= cutindx+1;  
		first= TRUE;
		lasti= -1; lastzyme= '';
		while ( (*fCutList)^[cutindx].fSeqindex == i do {
			zymes= (*fCutList)^[cutindx].fREnzyme->fName;
			cuts= (*fCutList)^[cutindx].fREnzyme->fCutcount;
			cutindx= cutindx+1;
			if ((cuts < minCuts) || (cuts > maxCuts)) LEAVE;
			//! Kludge til we fix REMap to stop dups
			if ((i=lasti) && (zymes == lastzyme)) LEAVE /*While*/; 
			lasti= i; lastzyme= zymes;
			
			if (first) {
				MoveTo( chleft, rowTop);
				if (doDraw) Line( 0, 2/*kTicSize*/) else Move( 0, 2);
				}
			ws= StringWidth( zymes);
			wd= 2 + ws div 2;
			do {
				GetPen( at);
				SetRect( cRect, at.h-wd, at.v, at.h+wd, at.v+nameHeight);
				overlap= RectInRgn( cRect, filledArea); 
			
				if (doDraw && overlap && !first) {
					//!? replace this w/ draw top to bottom of line, then overwrite zyme names 
					SetRect( bRect, at.h, at.v-nameHeight, at.h+1, at.v);
					if (!RectInRgn( bRect, filledArea)) {
						MoveTo( chleft, at.v-nameHeight+1);
						Line( 0, nameHeight-1);
						}
					}
				first= FALSE;
				MoveTo( chleft, at.v+nameHeight);
			} while (!(!overlap));
	
			RectRgn( aRgn, cRect);
			UnionRgn(filledArea, aRgn, filledArea);
			if (doDraw) {
				Move( -(ws div 2), 0);
				TextMode(srcCopy);  //erase line overlaps...
				DrawString( zymes); 
				TextMode(srcOr);
				}
			GetPen( at);
			rowBot = max( rowBot, at.v);   
			}
			
		spaceright();
		}
	lineHeight= rowbot - rowtop + 10; /* + fudge factor*/	
	DisposeRgn( filledArea);
	DisposeRgn( aRgn);
}



pascal void TREMapView::DrawRangeOfCells(GridCell startCell, stopCell, VRect aRect) 
										// override 
VAR
		short		colWidth;
		short		rowHeight;
		short		i, j;
		GridCell		aCell;
		short		left;
	
		lineKind, startBase, endBase,
		Integer		linesPerParag, dummy, seqLine, atBase;


	pascal void DrawHeader(void)
	VAR
		h, v	: integer;
		s, s1	: str255;
		longint		daytime;
		Rect		pagerect;
		
		pascal void nextLine( str255 s)
		{
			WITH pagerect)h= (right + left - stringwidth(s)) / 2;
			v= v + cHeight + 4; //! CHeight is a UPlot funct !
			MoveTo(h,v);
			DrawString(s);
		}
			
	{
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
		GetQDExtent( pagerect);
		v= pagerect.top;
		TextSize(14);
		nextline( concat('Restriction Map of ', fSeq->fName));

		//Stat line: fSeq->fLength, fMincuts->GetValue(), fMaxcuts.GetValue 
		TextSize(10);
		getDateTime( daytime);
		iuDateString( daytime, abbrevDate, s);
		iuTimeString( daytime, FALSE,  s1);
		nextline( concat(s,'  ',s1));				
	}

{
	laserLine( 2, 4); // set laserline smaller ...

	linesPerParag= 11; // spacer + 3 pro + seq + index + coseq + 3copro + renzymes 
	seqLine= (startCell.v-1) / linesPerParag;
	atBase = max( 0, startCell.h-3) + fFirstBase + min( fNbases, seqline * fBasesPerLine);
	fLastZymeCut= atBase;
	
	if (startCell.v == 1) DrawHeader;
	
	//- inherited::DrawRangeOfCells( startCell, stopCell, aRect); 
	//......... copy of UGricView.inc1.p:DrawRangeOfCells() .........
	aRect.left = aRect.left + ((fColInset) >> 1);	// fColInset / 2 
	aRect.top = aRect.top + ((fRowInset) >> 1); 	// fRowInset / 2 
	left = aRect.left;

	if (fColWidths->fNoOfChunks == 1) colWidth = GetColWidth(1);
	if (fRowHeights->fNoOfChunks == 1) rowHeight = GetRowHeight(1);

	FOR j = startCell.v TO stopCell.v){
		if (fRowHeights->fNoOfChunks == 1) 			// only one height 
			aRect.bottom = aRect.top + rowHeight - fRowInset
		else
			aRect.bottom = aRect.top + GetRowHeight(j) - fRowInset;

		aRect.left = left;							// start back at the left for the next row 
		lineKind= (j-1) % linesPerParag;
		
		if ((j>= fAllZymesStart) && (j <= fAllZymesEnd)) {
			if ((fAllzymes->IsOn())) this->AllZymeTable(j-fAllZymesStart, aRect);
			}		else if ((j>= fNoCutsStart) && (j <= fNoCutsEnd)) {
			if ((fNoncutters->IsOn())) this->NoCuttersTab(j-fNoCutsStart, aRect);
			}		else if ((j>= fCutPointStart) && (j <= fCutPointEnd)) {
			if ((fCutpoints->IsOn())) this->CutpointsTab(j-fCutPointStart, aRect);
			}			
		else if (((j>1) && (lineKind=0))) { //zyme line
			 i= startCell.h;
			 while ( i < 3){ //scoot over to zyme start...
				aRect.right= aRect.left + GetColWidth(i) - fColInset;
				aRect.left = aRect.right + fColInset;
				i= i+1;
				}
			seqLine = (j-2) / linesPerParag; //! j-2 to get zymes on right line
				/*! could get in trouble here -- need to draw full line for positioning, but
				 aRect &/or startCell.h may be inset from start of line...*/
			startBase= max(0, i - 3) + fFirstBase + min( fNbases, seqline * fBasesPerLine);
			endBase= startBase + min(fBasesPerLine-1, stopCell.h - i);
			this->DrawZymeLine( i, startBase, endBase, aRect, TRUE, dummy);
			}			
		else { // ... original code
			FOR i = startCell.h TO stopCell.h){
				if (fColWidths->fNoOfChunks == 1)			// only one height 
					aRect.right = aRect.left + colWidth - fColInset
				else
					aRect.right = aRect.left + GetColWidth(i) - fColInset;
	
				aCell.h = i;
				aCell.v = j;
				DrawCell(aCell, aRect);
	
				aRect.left = aRect.right + fColInset;
				}
			}
			
		aRect.top = aRect.bottom + fRowInset;
		}
}



pascal void TREMapView::DrawCell(GridCell aCell, VRect aRect) // override 
CONST
	kFontDescent == 2;
	kIndexRise == 1;
VAR
	integer		lineKind, linesPerParag ;
	longint		atBase, atBase1, atSeq, seqLine ;	
	Boolean		doTopIndex, doSequence;
	VRect		subRect;
	Boolean		threeBase;
	
/*!! aRect is band w/ all lines associated w/ sequence - base, index,
		prot[1-3], coprot[1-3], and zyme(s)
		? make aRect == fSeqRowHeight high, and shift aRect down for each sub-line
--*/

	pascal void drawTopIndex( longint atBase, boolean toptic, bottic)  
	VAR  nums	: str255;
			 integer		chRight, chLeft, rowtop, ws;
	{
	//$PUSH*/ /*$H-*/ SetPortTextStyle(fNumStyle);  /*$POP
		chleft	= subRect.left;
		chRight	= chleft + fBaseWidth; //fix for extra spacers
		rowtop	= subRect.bottom-kIndexRise;  
		MoveTo( chleft+kNucSpace, rowtop);
		if ((atBase % 10 == 4)) {
			if ((bottic)) Line(0,-1) else Move(0,-1);
			NumToString( atBase+1, nums);
			ws= StringWidth(nums);
			Move( -ws / 2, -1);
			DrawString(nums);
			if ((toptic)) {
				MoveTo( chleft+kNucSpace, rowtop);
				Line(0,2);
				}
			}
		/*---
		else if ((atBase % 10 == 9)) {
			if ((bottic)) Line(0,-2);
			if ((toptic)) {
				MoveTo( chleft+kNucSpace, rowTop);
				Line(0,2);
				}
			}
		---*/
		MoveTo( chleft, rowtop);
		LineTo( chright, rowtop);
	}

	pascal void drawSideIndex(longint atBase, leftBorder)
	str255		VAR  nums ;
	{
	//$PUSH*/ /*$H-*/ SetPortTextStyle(fNumStyle);  /*$POP
		MoveTo( subRect.right-leftBorder, subRect.bottom-kFontDescent);
		NumToString( atBase+1, nums);
		Move( -StringWidth(nums), 0);
		DrawString(nums);
	}

	pascal void DrawName( integer rightBorder, longint atBase)
	VAR 	aName	: Str63;
				aSeq	: TSequence;
	{
		aSeq= fSeq;
		if (aSeq!=NULL) {
	//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
			MoveTo( subRect.left+rightBorder, subRect.bottom-kFontDescent);
			aName= aSeq->fName;
			DrawString(aName);
			} 
	}
	
	pascal void DrawWord( str63 word, integer rightBorder, longint atBase)
	{
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle);  /*$POP
		MoveTo( subRect.left+rightBorder, subRect.bottom-kFontDescent);
		DrawString(word);
	}

	pascal void DrawNuc( longint atBase, TSequence aSeq)
	VAR
 		integer		rowbot, cw, rowleft;
		hSeq		: CharsHandle;
		ch			: char;
		myRect	: VRect;
	{
		if (aSeq!=NULL) {
		//$PUSH*/ /*$H-*/ SetPortTextStyle(fBaseStyle);  /*$POP
			rowleft= subRect.left;
			rowbot = subRect.bottom - kFontDescent;  
			myRect= subRect;
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
												
			cw= (fBaseWidth - charwidth(ch)) / 2;
			MoveTo( rowleft+cw, rowbot);
			DrawChar(ch);
			TextMode(srcOr); //? srcOr/srcCopy
			}
	}  
	
	
	pascal void DrawProt( integer atBase,  boolean use3aa, frame: integer; charsHandle hSeq)
	//for frame in [1..3] use seq[0+frame-1..end] 
	VAR
		integer		cw, atFrame;
		char		ch ;
	{
		frame= frame-1; //make 0-relative
		atFrame= atBase % 3;
		if ((atFrame == frame) && (hSeq!=NULL)) {
			//! atBase= (atBase / 3) * 3 + frame; *//*<< don't need: locate proper aa index
	
			/*$PUSH*/ /*$H-*/ SetPortTextStyle(fBaseStyle);  /*$POP*/	
			ch= (*hSeq)^[atBase];
			cw= (fBaseWidth - charwidth(ch)) / 2;
			MoveTo( subRect.left+cw,  subRect.bottom - kFontDescent);
			if (use3aa) DrawString(Amino123(ch)) else DrawChar(ch);
			}
	}  
	
	
	pascal void DrawZymes( integer atBase)
			/*==== 
					gctcggctgctgctcggctg
							||        | ||
						 abcI    cbaII
												hcgX
			====*/
	VAR
			RgnHandle		filledArea,aRgn;
			rowTop, chLeft, cutindx, firstCut, nCuts,
			integer		cuts, wd, ws, lasti, i, j, k;
			Point		at, lat;
			first, overlap	: boolean;
			lastzyme, zymes	: str63;	
			Rect		fillRect,bRect,aRect;
			
			nameHeight,
			integer		minCuts, maxCuts; //make these object fields...

	{
		firstCut= fLastZymeCut;
		fREMap->CutsAtBase( atBase, firstCut, ncuts);	
		fLastZymeCut= firstCut; //?
		if (nCuts > 0) {
			/*$PUSH*/ /*$H-*/ SetPortTextStyle(fNameStyle/*fZymeStyle*/);  /*$POP*/	
			rowTop= subRect.top-4;
			chleft= subRect.left+kNucSpace;
			if ((fMincuts!=NULL)) minCuts= fMinCuts.GetValue else minCuts= 1; //-1?
			if ((fMaxcuts!=NULL)) maxCuts= fMaxCuts.GetValue else maxCuts= maxint; //-1?
			nameHeight= cHeight;
			first= TRUE;
			lasti= -1; 
			lastzyme= '';
			
			filledArea= newRgn;
			aRgn= newRgn;
			SetRect( fillRect, chleft, rowTop, chleft+1, rowTop+1);
			RectRgn( filledArea, fillRect); //must start w/ non-empty rgn
	
			cutindx= firstCut;
			while ( (*fCutList)^[cutindx].fSeqindex == atBase do {
				zymes= (*fCutList)^[cutindx].fREnzyme->fName;
				cuts= (*fCutList)^[cutindx].fREnzyme->fCutcount;
				cutindx= cutindx+1;
				if ((cuts < minCuts) || (cuts > maxCuts)) LEAVE;
				//! Kludge til we fix REMap to stop dups
				if ((atBase=lasti) && (zymes == lastzyme)) LEAVE /*While*/; 
				lasti= atBase; 
				lastzyme= zymes;
				
				if (first) {
					MoveTo( chleft, rowTop);
					Line( 0, 2 /*kTicSize*/); 
					}
				ws= StringWidth( zymes);
				wd= 2 + ws div 2;
				do {
					GetPen( at);
					SetRect( aRect, at.h-wd, at.v, at.h+wd, at.v+nameHeight);
					overlap= RectInRgn( aRect, filledArea); 
				
					if (overlap && !first) {
						SetRect( bRect, at.h, at.v-nameHeight, at.h+1, at.v);
						if (!RectInRgn( bRect, filledArea)) {
							MoveTo( chleft, at.v-nameHeight+1);
							Line( 0, nameHeight-1);
							}
						}
					first= FALSE;
					MoveTo( chleft, at.v+nameHeight);
				} while (!(!overlap));
		
				RectRgn( aRgn, aRect);
				UnionRgn(filledArea, aRgn, filledArea);
				Move( -(ws div 2), 0);
				TextMode(srcCopy);  //erase line overlaps...
				DrawString( zymes); 
				TextMode(srcOr);
				GetPen( at);
				}
							
			DisposeRgn( filledArea);
			DisposeRgn( aRgn);
			}
	}

	
{
	if ((aRect.bottom < aRect.top+2)) exit(DrawCell);

	if ((aCell.v == 1)) { //header line
		//nada -- don't want to draw header for each of 44 CELLs in the header line
		}	else if ((aCell.v >= fAllZymesStart)) {
		//nada -- tables
		}	else {
		subRect= aRect;
		linesPerParag= 11; // spacer + 3 pro + seq + index + coseq + 3copro + renzymes 
		lineKind= (aCell.v-1) % linesPerParag;
		seqLine = (aCell.v-1) / linesPerParag; //!? (aCell.v-2) for enz line !?
		/* atBase equation depends on if in seq or on sides */ 
		threeBase= (fThreeBase!=NULL) && (fThreeBase->IsOn());

		if ((aCell.h == 1)) { //nameleft
			atBase= fFirstBase + min( fNbases, seqLine * fBasesPerLine);
			if (fDoLeftName) CASE lineKind OF
				1 : ; //spacer
				2 : DrawWord( 's1', 0, atBase);
				3 : DrawWord( 's2', 0, atBase);
				4 : DrawWord( 's3', 0, atBase);
				5 : DrawName( 0, atBase);
				7 : DrawWord( 'cmp.', 0, atBase);
				8 : DrawWord( 'c1', 0, atBase);
				9 : DrawWord( 'c2', 0, atBase);
				10: DrawWord( 'c3', 0, atBase);
				0 : DrawWord( 'enz.', 0, atBase);
				}
			}			
		else if ((aCell.h == 2)) { //indexLeft
			atBase= fFirstBase + min( fNbases, seqLine * fBasesPerLine);
			if (fDoLeftIndex) CASE lineKind OF
				5 : DrawSideIndex( atBase, kNucBorder);
				}
			}			
		else if ((aCell.h < fBasesPerLine+3)) { //bases
			atBase1= (seqLine * fBasesPerLine) + aCell.h - 2;
			if ((atBase1 <= fNBases)) {
				atBase= fFirstBase - 1 + atBase1; 	//atbase == 0  for first 
				switch (lineKind) {
					1 : ; //spacer
					2, 3, 4 : if ((atBase1 <= fNBases-2)) 
							DrawProt( atBase, ThreeBase, lineKind-1, fProt);
					5 : DrawNuc( atBase, fSeq);
					6 : DrawTopIndex( atBase, TRUE, TRUE); //?? - orig was above fSeq, below fCoseq or here depending on others
					7 : DrawNuc( atBase, fREMap->fCoSeq);
					8, 9, 10: if ((atBase1 <= fNBases-2)) 
							DrawProt( atBase, ThreeBase, lineKind-7, fCoProt);
					0 : ; //- DrawZymes( atBase);
					}
				}
			}			
		else if ((aCell.h == fBasesPerLine+3)) { //indexRight
			atBase= fFirstBase - 1 + min( fNBases, (seqLine+1) * fBasesPerLine);
			if (fDoRightIndex) CASE lineKind OF
				5 : DrawSideIndex( atBase, 0);
				}
			}		else if ((aCell.h == fBasesPerLine+4)) { //nameRight
			atBase= fFirstBase - 1 + min( fNBases, (seqLine+1) * fBasesPerLine);
			if (fDoRightName) CASE lineKind OF
				1 : ; //spacer
				2 : DrawWord( 's1', kNucBorder, atBase);
				3 : DrawWord( 's2', kNucBorder, atBase);
				4 : DrawWord( 's3', kNucBorder, atBase);
				5 : DrawName( kNucBorder, atBase);
				7 : DrawWord( 'cmp.', kNucBorder, atBase);
				8 : DrawWord( 'c1', kNucBorder, atBase);
				9 : DrawWord( 'c2', kNucBorder, atBase);
				10: DrawWord( 'c3', kNucBorder, atBase);
				0 : DrawWord( 'enz.', kNucBorder, atBase);
				}
			}
	}
}



pascal void TREMapView::CalcMinFrame(VRect VAR minFrame)
{
	inherited::CalcMinFrame( minFrame);
}




pascal void TREMapView::DoMenuCommand(CommandNumber aCommandNumber) // override 
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
pascal TCommand TREMapView::DoMouseCommand(Point VAR theMouse, EventInfo VAR info,
								   Point VAR hysteresis)
VAR
		FailInfo		fi;

	pascal void HdlInitCmdFailed(OSErr error, long message)
		{
		FreeIfObject(protoREMap);
		protoREMap = NULL;
		}

	{					
	DoMouseCommand = NULL;

	fClickPt = theMouse;
	if (palette->fCurrREMap > 0) { // draw mode
		FailSpaceIsLow(); 								// Make sure we aren't low on memory 

		Deselect();

		//Clone appropriate REMap

		protoREMap = TREMap(gREMapsArray[palette->fCurrREMap].Clone);
		FailNil(protoREMap);

		CatchFailures(fi, HdlInitCmdFailed);
		// Make sure cloning the REMap left us with enough memory to continue.
		FailSpaceIsLow();

		New(REMapSketcher);
		FailNil(REMapSketcher);
		REMapSketcher->IREMapSketcher(this, protoREMap, info.theOptionKey);
		Success(fi);
		DoMouseCommand = REMapSketcher;
		}			
		
	else {	 	//select mode
		REMapUnderMouse = NULL;
		fREMapDocument->EachVirtualREMapDo(CheckREMap);

		if (REMapUnderMouse == NULL)	{		//area select
			if (!info.theShiftKey)
				Deselect();
			New(REMapSelector);
			FailNil(REMapSelector);
			REMapSelector->IREMapSelector(cMouseCommand, this);
			DoMouseCommand = REMapSelector;
			} 

		else {										//REMap select/move/...
			if (!(REMapUnderMouse->fIsSelected || info.theShiftKey))
				Deselect();

			if (info.theShiftKey) {
				REMapUnderMouse->fIsSelected = !REMapUnderMouse->fIsSelected;
				if (REMapUnderMouse->fIsSelected)
					REMapUnderMouse->Highlight(hlOff, hlOn)
				else
					REMapUnderMouse->Highlight(hlOn, hlOff);
				}			else if (!REMapUnderMouse->fIsSelected)
				{
				REMapUnderMouse->fIsSelected = TRUE;
				DoHighlightSelection(hlOff, hlOn);
				}

			if (REMapUnderMouse->fIsSelected) {
				New(REMapDragger);
				FailNil(REMapDragger);
				REMapDragger->IREMapDragger(this);
				DoMouseCommand = REMapDragger;
				}
			//else, fall-through, we return NULL
			}	 
		}					 
}										
**********/



pascal void TREMapView::DoSetupMenus(void)
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

