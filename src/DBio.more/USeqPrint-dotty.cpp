// USeqPrint.dotty.p 
// d.g.gilbert, 1992 

/*
	dotty plot
*/

#pragma segment DottyPlot

CONST

	kDottyPlotWindowRSRCID == 1031;



// TDottyPlotDocument -----------------------------------------------

 /*---
pascal Str255 numStr( longint anum)
VAR  aStr: Str255;
{
  NumToString( anum, aStr);
	numStr= aStr;
}
 ---*/

pascal void windowpt(ptr vSeq, integer nV, hSeq: ptr; integer nH,
									 integer		nucWindow, nucMatches; boolean doself, allDots,
									 handle pich)
	EXTERNAL();   //C version*/ /*fills pich handle with points vector


pascal void TDottyPlotDocument::IDottyPlotDocument(OSType fileType, TDocument parentDoc, 
					TSeqList		aAlnList; longint startbase, nbases)
VAR
		TFile		aFile;
		
	pascal void CloneIt( TSequence aSeq)  
		//! CloneSeqList: used also in TAlnPasteCommand; make TSeqList method !?
	integer		VAR  aRow;
	{
		aRow= fAlnList->GetEqualItemNo(aSeq);
		aSeq= TSequence(aSeq->Clone());
		fAlnList->AtPut( aRow, aSeq);
	}
  
{
	fDottyPlotView = NULL;
	fParentDoc= parentDoc;
	fAlnList= aAlnList;
	fAlnList->Each(CloneIt);
	fFirstBase= startBase;
	fNBases		= nbases;	
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

pascal TFile TDottyPlotDocument::DoMakeFile(itsCommandNumber:CommandNumber); // override 
{
	DoMakeFile= NewFile(fScrapType, gApplication->fCreator, kUsesDataFork, kUsesRsrcFork, 
												!kDataOpen, !kRsrcOpen);
}

pascal void TDottyPlotDocument::FreeData(void)
{
	//- if ((fDottyPlotView!=NULL)) fDottyPlotView->Free(); */ /*??
	TObject(fAlnList) = FreeListIfObject(fAlnList);   
}


pascal void TDottyPlotDocument::Free(void) // override 
{
	FreeData();
	inherited::Free();
}



pascal void TDottyPlotDocument::DoMakeViews(Boolean forPrinting) // override 
VAR
		TWindow		aWindow;
		Point		minSize;
		Point		maxSize;
		vSize 	: Vpoint;
{
	aWindow = gViewServer->NewTemplateWindow(kDottyPlotWindowRSRCID, this);
	FailNil(aWindow);
	fDottyPlotWind = TDottyPlotWind(aWindow);
	fDottyPlotWind->IDottyPlotWind();
	
	fDottyPlotView = TDottyPlotView(aWindow->FindSubView('PrVw'));
	FailNil(fDottyPlotView);

	fDottyPlotView->fNucWindow= TNumberText(aWindow->FindSubView('nwin'));
	fDottyPlotView->fNucMatches= TNumberText(aWindow->FindSubView('nmat'));
	fDottyPlotView->fAllDots= TCheckBox(aWindow->FindSubView('alld'));
	
	fDottyPlotWind->fRedrawBut= TButton(aWindow->FindSubView('bRDR'));
	fDottyPlotWind->fNucWindow= fDottyPlotView->fNucWindow;
	fDottyPlotWind->fNucMatches= fDottyPlotView->fNucMatches;
	fDottyPlotWind->fAllDots= fDottyPlotView->fAllDots;
	
	fDottyPlotView->fScroller = fDottyPlotView->GetScroller(TRUE);
			//fix so prefview scroll bar shows
/*-	if ((fDottyPlotView->fScroller!=NULL)) 
			fDottyPlotView->fScroller.fScrollLimit.v= 500;*/ 

	// get our damn data into view 
	fDottyPlotView->FindDots();
			
	// set window's resize limits so it can't become wider than the DottyPlotview's edge 
	WITH aWindow->fResizeLimits){
		minSize = topLeft;
		maxSize = botRight;
		}
	WITH maxSize)h = Min( 2 + fDottyPlotView->fSize.h + kSBarSizeMinus1, h);
	aWindow->SetResizeLimits(minSize, maxSize);
	vsize.h= maxSize.h;
	vsize.v= maxSize.v;
	if ((fDottyPlotView->fScroller!=NULL)) 
		fDottyPlotView->fScroller->SetScrollLimits(vsize, kDontRedraw);
}

pascal void TDottyPlotDocument::UntitledName(Str255 VAR noName) // override 
//called AFTER .Idoc and .MakeViews 
{
	//- noName= Concat(fParentDoc.(*fTitle)^,' Dotplot'); 
	noName= Concat('Dotplot ',fDottyPlotView->fSeqV.fName, 
								 '-', fDottyPlotView->fSeqH.fName); 

	if ((fWindowList != NULL) && (fWindowList.GetSize > 0)) 		
		TWindow(fWindowList->First())->SetTitle(noName);
}


/*
TDottyPlotDocument file has the following format:
  Data Fork:
	(a)  kMacdrawHeaderSize (512 bytes) == nulls
	(b)  The rest => PICTure of window

 Resource Fork:
	nothing, yet
*/


pascal void TDottyPlotDocument::DoNeedDiskSpace(TFile itsFile,
															long VAR dataForkBytes, rsrcForkBytes)
{
	//)!get Print record requirements 
	//- inherited::DoNeedDiskSpace(dataForkBytes, rsrcForkBytes); 
	
	dataForkBytes = dataForkBytes + kMacdrawHeaderSize  /*+ sizeof window pict */;
	
	/*-- if you really want to know pict size:
	fDottyPlotView->WriteToDeskScrap(); 
	len= fDottyPlotView->GivePasteData( NULL, 'PICT');
	----*/
	/*---
	rsrcForkBytes = rsrcForkBytes + kRsrcTypeOverhead + kRsrcOverhead + sizeof(DocState);
	---*/
}



pascal void TDottyPlotDocument::DoRead(aFile:TFile; Boolean forPrinting)
{
	//-- inherited::DoRead(aRefNum, rsrcExists, forPrinting);)!read print info stuff
	// This is a Write-Only document == PICT of output drawing, no reading... ?
}



pascal void TDottyPlotDocument::DoWrite(TFile aFile, Boolean makingCopy)
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

	fDottyPlotView->WriteToDeskScrap();
	
	hPict= NewHandle(0);
	CatchFailures(fi, HdlDoWrite);
	len= fDottyPlotView->GivePasteData( hPict, 'PICT');
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


// TDottyPlotWind ------------------------- 

pascal void TDottyPlotWind::IDottyPlotWind(void)
{
	IPrefWindow();
	fWantSave= TRUE;
}

pascal void TDottyPlotWind::SetPrefID(void) /* override */ 
{
	gPrefWindID= kDottyPlotWindowRSRCID; gPrefWindName= 'TDottyPlotWind';
}


pascal void TDottyPlotWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
VAR
		TDottyPlotView		aDottyPlotView;
{
	aDottyPlotView= TDottyPlotDocument(fDocument).fDottyPlotView;

	switch (eventNumber) {

		mButtonHit: if ((source == fRedrawBut)) {
				aDottyPlotView->FindDots();
				aDottyPlotView->ForceRedraw();
				}			else inherited::DoEvent(eventNumber,source, event); 

		otherwise
			inherited::DoEvent(eventNumber,source, event); 
		}
}




// TDottyPlotView -------------------------------------


pascal void TDottyPlotView::FreeData(void)
{
	//!!! NO -- fAlnList is owned by DOC
	/*------
	if ((fAlnList!=NULL)) {
		fAlnList->DeleteAll(); 
		fAlnList->Free(); 
		fAlnList= NULL;
		}
	-----------*/
	fDotsH= DisposeIfHandle( fDotsH);
}

pascal void TDottyPlotView::Free(void) // override 
{
	FreeData();
	inherited::Free();
}


pascal void TDottyPlotView::SetDotRect(void)
//call w/ dotr == pagerect; returns square centered in pagerect
VAR i, maxh, maxv: longint;
		Rect		dotr;
		LoV, nV, HiV,
		longint		LoH, nH, HiH;
		VPoint		minSize;
{
	this->GetQDExtent( dotr);
	fMinSize.h= dotr.right;
	fMinSize.v= dotr.bottom;
	minSize= fMinSize;
	fSeqV->GetSelection( LoV, nV); HiV= LoV + nV;
	fSeqH->GetSelection( LoH, nH); HiH= LoH + nH;
	WITH dotr){
		//make label margins
		left = left + 32;
		right= right - 8;
		i = hiV;
		do {
			right = right - 12; 
			i = i / 10;
		} while (!(i < 1));
		top = top + 100; //margin for titles
		bottom = bottom - 32;

		//make plot rect proportional to full dot rect
		maxh = right - left;
		maxv = bottom - top;
		maxh = min(nH, min(maxh, (nH * maxv) / nV));
		maxv = min(nV, min(maxv, (nV * maxh) / nH));

		//center in port -- need 6+ lines at top for titles 
		left = left + (right - left - maxh) / 2;
		right = left + maxh;
	 //?  top = top + (bottom - top - maxv) / 2; 
		bottom = top + maxv;
		}
	fDotRect= dotr;

	if ((fscroller!=NULL)) fScroller->SetScrollLimits(minSize, kDontRedraw);
}	//setDotRect


pascal void TDottyPlotView::FindDots(void)
VAR
		fullDots, doSelf	: boolean;
		dotsH		: Handle;
		CharsHandle		basesV, basesH;
		longint		startV, startH, nV, nH;
{
	//- Curses(watchCursor); 
	FreeData();
	
	fNucWindowVal= fNucWindow->GetValue();
	fNucMatchesVal= fNucMatches->GetValue();
	fullDots= fAllDots->IsOn();

	fDotsH= NewHandle(0);
	FailNIL( fDotsH);

	fSeqV= TSequence(fAlnList->First());
	FailNIL( fSeqV);
	Handle(basesV)= fSeqV->fBases;
	fSeqV->GetSelection( startV, nV);
	
			//!? want to handle size > 2 w/ multiple plots 
	if ((fAlnList.GetSize <= 1)) {
		doSelf= TRUE;
		fSeqH= fSeqV;
		basesH= basesV;
		}	else {
		doSelf= FALSE;
		fSeqH= TSequence(fAlnList->At( 2)); 
		FailNIL( fSeqH);
		Handle(basesH)= fSeqH->fBases;
		fSeqH->GetSelection( startH, nH);
		moveHHi( handle(basesH)); HLock(handle(basesH));
		}
	moveHHi( handle(basesV)); HLock(handle(basesV));

	if (doSelf)
		windowpt(&(*basesV)^[startV], nV, &(*basesV)^[startV], nV, 
						fNucWindowVal, fNucMatchesVal, doSelf,
						fullDots, handle(fDotsH))
	else
		windowpt(&(*basesV)^[startV], nV, &(*basesH)^[startH], nH,
						fNucWindowVal, fNucMatchesVal, doSelf, 
						fullDots, handle(fDotsH));
						
	HUnlock(handle(basesH));
	HUnlock(handle(basesV));
	
	SetDotRect();
}   


	


pascal void TDottyPlotView::IDottyPlotView( TDottyPlotDocument itsDocument, Boolean forClipboard)
VAR
		itsSize		: VPoint;
		aHandler	: TDottyPlotHandler;
		sd				: SizeDeterminer;
{
	fDotsH= NULL;
  fNucWindowVal= 0;
	fNucMatchesVal= 0;
	fMinSize= gZeroVPt;

	SetVPt(itsSize, kMaxCoord, kMaxCoord);
	if (forClipboard)
		sd = sizeVariable
	else
		sd = sizeFillPages;
	IView(itsDocument, NULL, gZeroVPt, itsSize, sd, sd);
	fScroller = NULL;
	fDottyPlotDocument = itsDocument;
	fFirstBase= fDottyPlotDocument->fFirstBase;  
	fNbases= fDottyPlotDocument->fNbases;  
	fAlnList= fDottyPlotDocument->fAlnList;  

#if FALSE										//!!! Need to handle this
	if (forClipboard) fWouldMakePICTScrap = TRUE;
#endif

	if (!forClipboard) {
		New(aHandler);
		FailNil(aHandler);
		aHandler->IDottyPlotHandler(itsDocument, this, 
									!kSquareDots,  
								  kFixedSize,			// horizontal page size is fixed 
								  kFixedSize);			// vertical page size is fixed 
		}
}

 

pascal void TDottyPlotView::Initialize(void) // override 
{
	fDotsH= NULL;
  fNucWindowVal= 0;
	fNucMatchesVal= 0;
	fMinSize= gZeroVPt;

	fScroller = NULL;
	fDottyPlotDocument= NULL;
	fFirstBase= 0;
	fNbases= 0;
	fAlnList= NULL;
	inherited::Initialize();
}

pascal void TDottyPlotView::DoPostCreate(TDocument itsDocument) // override 
VAR
		TDottyPlotHandler		aHandler;
{
	inherited::DoPostCreate( itsDocument);
	
	fDottyPlotDocument = TDottyPlotDocument(itsDocument);
	fFirstBase= fDottyPlotDocument->fFirstBase;  
	fNbases= fDottyPlotDocument->fNbases;  
	fAlnList= fDottyPlotDocument->fAlnList;  
	
	New(aHandler);
	FailNil(aHandler);
	aHandler->IDottyPlotHandler(itsDocument, this, 
								!kSquareDots, // does not have square dots 
							  kFixedSize,				// horizontal page size is fixed 
							  kFixedSize);				// vertical page size is fixed 
}
 


/*---
CONST
	kTop == 40;
	kNameLeft == 6;
	kNucLeft == 12;
	kFontDescent == 5;
	kIndexOutset == 4;
	kRowLeading == 1;
	kMaxName == 12;
	kBasesPerLine == 40;
	kNucSpace == 2;
	kParagraph == 30;
---*/

pascal void TDottyPlotHandler::IDottyPlotHandler(TDocument itsDocument, TView itsView,
											Boolean itsSquareDots, itsHFixedSize, itsVFixedSize)
{
	IStdPrintHandler( itsDocument, itsView,itsSquareDots,itsHFixedSize,itsVFixedSize);
	fAlnList= TDottyPlotView(itsView).fAlnList;
}

pascal VCoordinate TDottyPlotHandler::BreakFollowing(VHSelect vhs,
										 VCoordinate		previousBreak;
										 Boolean VAR automatic) // override 
//from TStdPrintHandler->BreakFollowing(); Called from fView->DoBreakFollowing(), 
VAR
		VHSelect		orthoVHS;
		VCoordinate		rowTop, maxVpage, endPage, newLoc;
		integer		rowHeight, ngroup, i;
		fi		: FontInfo;
		done	: boolean;
{

	BreakFollowing= inherited::BreakFollowing( vhs, previousBreak, automatic);

/*******
	orthoVHS = gOrthogonal[vhs];
	automatic = TRUE;
	endPage =  fViewPerPage.vh[orthoVHS];

	if ((orthoVHS == v)) {
		/*! we maybe need to calc this for each page, 
			since # of kParagraph spaces can differ per page */
		maxvpage = endPage;
	 
		SetPortTextStyle(gPrintNameStyle);
		GetFontInfo(fi);
		rowHeight= fi.ascent+fi.descent+fi.leading;
		rowTop = kTop;
		ngroup = fAlnList->GetSize();
		i= 0;
		do {
			endPage= rowTop;
			i= i+1;
			rowtop = rowtop + rowHeight + kRowLeading;
			if ((i=ngroup)) {
				rowTop= rowTop + kParagraph;
				i= 0;
				}
			done= rowTop >= maxvpage;
		} while (!(done));
		}
		
	newLoc = Min(previousBreak + endPage, fPrintExtent.botRight.vh[orthoVHS]);
	BreakFollowing = newLoc;
*******/
}




pascal void TDottyPlotView::Draw(VRect area)	
VAR
	fi	: fontInfo;
	nameHeight, nucHeight,rowHeight	: integer;
	nucLeft, nameLeft, rowTop, saveRowTop, rowLeft, 
	integer		nBases, startBase, basesRemaining;
	integer		chleft, nucwidth ;
	maxNameWid	: integer;
	hMaxBase		: charsHandle;
	
/*************

	procedure spaceright( integer i)
	begin
		 chleft= chleft + nucwidth; 
		 if (i mod 10 == 9) then chleft= chleft + nucwidth;   
	end();
	
	pascal void DrawIndex(void)  
	str255		var  nums;
			 i, ws, blockleft	: integer;
	{
	
	// set laserline smaller ...
	// use a custom Index TextStyle (9pt times-italic ?) 
		laserLine( 2, 4);
		chleft= rowLeft;
		blockleft= chleft;
		for i= 0 to nBases-1 do begin
			MoveTo( chleft+kNucSpace, rowtop-kIndexOutset);
			if ((i % 10 == 4)) then begin
				Line(0,-4);
				NumToString( i+startBase+1, nums);
				ws= StringWidth(nums);
				Move(-ws div 2, -1);
				DrawString(nums);
				end
			else
				Line(0,-2);
		
			if ((i % 10 == 9)) then begin
				MoveTo( chleft+kNucSpace, rowtop-kIndexOutset);
				LineTo( blockleft, rowtop-kIndexOutset);
				blockleft= chleft;
				end();
			spaceright(i);
			if ((i % 10 == 9)) then 
				blockleft= chleft;
			end();
	}
		

 pascal void GetNameWidth(TSequence aSeq)
 VAR
		myPt		: point;
		aName		: Str63;
 {
	if (aSeq!=NULL) {
		aName= aSeq->fName;
		maxNameWid= max(maxNameWid, StringWidth(aName));
		} 
 }

 pascal void DrawNames(TSequence aSeq)
 VAR
 		integer		i, rowbot;
		myPt		: point;
		myRect, qdArea	: Rect;
		aName		: Str63;
 {
		rowbot= rowtop + rowHeight;  
		if (aSeq!=NULL) {
		  MoveTo( rowleft, rowbot-kFontDescent);
			SetRect( myRect, rowLeft, rowtop, nucLeft, rowBot);
			VRectToRect( area, qdArea);
			if (SectRect( myRect, qdArea, myRect)) {
				aName= aSeq->fName;
				maxNameWid= max(maxNameWid, StringWidth(aName));
				DrawString(aName);
				}
			} 
		rowtop = rowbot + kRowLeading;
 }

**********/

 
	pascal void plotDots(void) 
	CONST
		ticsize      == 2;
	VAR
		longint		np, i       ;
		longint		maxv, maxh  ;
		pp          : ^Point;
		pagerect, rdots	: rect;
		longint		LoH, HiH, LoV, HiV, nH, nV;


	pascal integer maph(longint dotx)
		{
			maph = longint(dotx * maxh) / nH;
		}

	pascal integer mapv(longint doty)
		{
			mapv = longint(doty * maxv) / nV;
		}

	pascal void getStep(integer w, integer VAR step, z)
		{
			if (w >= 4000) {
				step = 100; z = 4000;
				}			else if (w >= 1000) {
				step = 50; z = 1000;
				}			else if (w >= 500) {
				step = 20; z = 500;
				}			else if (w >= 100) {
				step = 10;  z = 100;
				}			else {
				step = 5; z = 10;
				}
		}


	pascal void doTitles(void)
		VAR 
			integer		h, v;
			s, s1	: str255;
			longint		daytime;
			
			pascal void nextLine( str255 s)
			{
				WITH pagerect)h= (right + left - stringwidth(s)) / 2;
				v= v + cheight + 4; //!? what is CHeight -- a UPlot funct ?
				MoveTo(h,v);
				DrawString(s);
			}
			
		{
			v= pagerect.top;

			TextFont(Times); 
			TextSize(14);
			nextline( concat('Dot Plot of ', fSeqV->fName, ' x ', fSeqH->fName));

			TextFont(times); 
			TextSize(12);
			np = getHandleSize(handle(fDotsH)) / sizeof(point);
			nextline( concat('Base Window: ',numStr(fNucWindowVal),
								 '  Stringency: ',numStr(fNucMatchesVal),
								 '  Points: ',numStr(np)) );
			getDateTime( daytime);
			iuDateString( daytime, abbrevDate, s);
			iuTimeString( daytime, FALSE,  s1);
			nextline( concat(s,'  ',s1));				
			
			//horizontal axis 
			s= concat( fSeqH->fName, '  (',numStr(LoH),' to ',numStr(HiH-1),')');
			moveto((rdots.left + rdots.right - stringwidth(s)) / 2, rdots.bottom + 20);
			drawstring(s);

			//vertical axis 
			s= concat( fSeqV->fName, '  (',numStr(LoV),' to ',numStr(HiV-1),')');
			h = rdots.left - 14;
			v = (rdots.bottom + rdots.top) / 2;
			textPicDraw(h, v, 270, s);
		}                         
		
		
		pascal void doTicLabels(void)
		VAR
			integer		h, v, k, k1, i, z, step;
			str255		s           ;
		{
			TextFont(times); 
			TextSize(10);
			//horizontal
			getStep(nH, step, z);
			k = (LoH / z) * z; k1= 0;
			do {
				if (k >= LoH) {
					h = maph(k - LoH);
					moveto(rdots.left + h,  rdots.bottom);
					if k1 mod 10 == 0 then line(0, 5) 
					else line(0, 2);
					moveto(rdots.left + h,  rdots.top);
					if k1 mod 10 == 0 then begin
						numToString(k, s);
						line(0, -5);
						move(-(stringwidth(s) / 2), -3);
						drawstring(s);
						end
					else 
						line(0, -2);
					}
				k = k + step; k1= k1+1;
			} while (!((k > HiH)));

			//verticals 
			getStep(nV, step, z);
			k = (LoV / z) * z; k1= 0;
			do {
				if (k >= LoV) {
					v = mapv(k - LoV);
					moveto(rdots.left, rdots.bottom - v);
					if k1 mod 10 == 0 then line( -5, 0)
					else line(-2, 0);
					moveto(rdots.right, rdots.bottom - v);
					if k1 mod 10 == 0 then begin
						numToString(k, s);
						line( 5, 0);
						move( 4, 3);
						drawstring(s);
						end
					else
						line(2, 0);
					}
				k = k + step; k1= k1+1;
			} while (!((k > HiV)));
		}    //doTicLabels

	{
		//- Handle(basesH)= fSeqH->fBases; 
		fSeqH->GetSelection( LoH, nH);
		HiH= LoH + nH;
		fSeqV->GetSelection( LoV, nV);
		HiV= LoV + nV;
		
		this->GetQDExtent(pagerect); 
		rdots= fDotRect;		
		with rdots do begin
			maxh= right - left;
			maxv= bottom- top;
			end();
			
		beginPicGroup();
		
		beginPicGroup();
		doTitles();
		endPicGroup();

		beginPicGroup();
		framerect(rdots);
		doTicLabels();
		endPicGroup();

		beginPicGroup();
		laserLine(1, 2);  //! note 1,3 or smaller doesn't work on DEC ScriptPrinter 
		moveHHi(handle(fDotsH));
		hlock(handle(fDotsH));
		np = getHandleSize(handle(fDotsH)) / sizeof(point);
		Ptr(pp) = (*fDotsH);
		FOR i = 0 TO np - 1){
			/*--------- (for subrange plots)
			if (pp->h>= fLoH1 - LoH) and (pp->h <= fHiH1-LoH)
			 and (pp->v >= fLoV1 - LoV) and (pp->v <= fHiV1-LoV)
				then
			---------*/
				{
				moveto(rdots.left + maph(pp->h), rdots.top + mapv(pp->v));
				line(0, 0);
				}
			longint(pp) = longint(pp) + sizeof(point);
			}
		hunlock(handle(fDotsH));
		endPicGroup();
		
		endPicGroup();
		
	}  //plotDots


{

	inherited::Draw(area);

	SetPortTextStyle(gPrintNucStyle);
	
	PlotDots();
	
/********
	//! do MacDraw grouping messages 
	nameLeft= kNameLeft;
 
	SetPortTextStyle(gPrintNameStyle);
	GetFontInfo(fi);
	nameHeight= fi.ascent+fi.descent+fi.leading;
	maxNameWid= 0;
	fAlnList->Each(GetNameWidth);
	//- nucLeft = nameLeft + kMaxName*charWidth('W') + kNucLeft;
	nucLeft = nameLeft + maxNameWid + kNucLeft;  //???

	SetPortTextStyle(gPrintNucStyle);
	nucwidth= CharWidth('G')+kNucSpace;
	nucHeight= fi.ascent+fi.descent+fi.leading;
	rowHeight= /*kRowLeading +*/ max( nucHeight, nameHeight);
	
	rowTop	= kTop;
	startBase= fFirstBase;
	basesRemaining= fNBases;
	if (basesRemaining < 1) 
	 if ((fAlnList!=NULL) && (fAlnList.GetSize>0))
		basesRemaining= TSequence(fAlnList->First()).fLength;
	while (basesRemaining > 0)){
	
		nBases= min(kBasesPerLine, basesRemaining);

		beginPicGroup();
		rowLeft= nucLeft;
		SetPortTextStyle(gPrintNameStyle);
		DrawIndex();
		endPicGroup();

		beginPicGroup();
		rowLeft=	nameLeft;  
		saveRowTop= rowTop;
		SetPortTextStyle(gPrintNameStyle);
		fAlnList->Each(DrawNames);
		endPicGroup();
	
		beginPicGroup();
		rowTop= saveRowTop; 
		rowLeft= nucLeft;
		SetPortTextStyle(gPrintNucStyle);
		fAlnList->Each(DrawNucs);
		endPicGroup();
		
		startBase= startBase + nBases;
		basesRemaining= basesRemaining - nBases;
		rowTop= rowTop + kParagraph;
		}
		
	fMinSize.h= nucLeft + chleft + 40;
	fMinSize.v= rowTop + 40;
*******/
}



pascal void TDottyPlotView::CalcMinFrame(VRect VAR minFrame)
VAR
	nb	: integer;
	TScroller		aScroller;
	VPoint		aPt;
{
	inherited::CalcMinFrame( minFrame);
	minFrame.right= max(minFrame.right, fMinSize.h);
	minFrame.bottom= max(minFrame.bottom, fMinSize.v);
	aScroller= GetScroller(TRUE);
	SetVPt( aPt, minFrame.right, minFrame.bottom);
	if ((ascroller!=NULL)) aScroller->SetScrollLimits(aPt, kDontRedraw);
}




pascal void TDottyPlotView::DoMenuCommand(CommandNumber aCommandNumber) // override 
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



pascal void TDottyPlotView::DoSetupMenus(void)
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

