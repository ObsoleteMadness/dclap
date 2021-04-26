// USeqApp.aln.p -- alignment window/views 
// d.g.gilbert, 1991 

/*
		fix AlnPasteCommand so we don't cause bombs w/ paste + use of
		uncommitted seqs (dispose?)
*/


CONST
	kAlnDocID == kAlnWindID;
	kMinCommonPercent == 0.80;  //need user opt 

	kModifiedFlag == False; // temp until we figure out proper conditions for fSeq.modified 

VAR
	integer		gMinCommonPercent; //??
	TextStyle		gAlnTextStyle ;
	longint		gMaxViewCellsBugFix;
	Boolean		gMaxViewCellsBugSeen;
	
// TAlnSequence ------------------------

/*DEBUG AlnSeq edit display... 

	!problem of jumping up to vert=0 line seems to be color-related,
	!problem of cursor/display frozen at horizontal=1 is related to long seqs > 1000...3000
*/

#pragma segment AOpen
pascal void TAlnSequence::IAlnSequence(TSequence aSeq)
VAR	aTextData: TTextData;
{
	fSeq= aSeq;
	fFreeText= FALSE;   
  fInset= gZeroVRect;   

	New(aTextData); 
	FailNIL( aTextData);
	aTextData->ITextData();
	if ((aSeq!=NULL)) {
		Handle(aTextData->fChars)= DisposeIfHandle(aTextData->fChars);
		aTextData->fChars= aSeq->fBases;
		}
	fTextData= aTextData;
	fSearchData.hTarget= NULL;
}

pascal void TAlnSequence::ReplaceSeq(TSequence aSeq)
{
	fSeq= aSeq;
	if ((fSeq!=NULL)) {
		fTextData->fChars= fSeq->fBases;
		StuffText( fSeq->fBases);
		}
}

#pragma segment AClose
pascal void TAlnSequence::Free(void) // override 
{
	fTextData->fChars= NULL;
	fTextData->Free();
	fTextData= NULL;
	fFreeText= FALSE;  
	if ((fSearchData.hTarget!=NULL)) {
		DisposHandle(Handle(fSearchData.hTarget));
		fSearchData.hTarget= NULL;
		}
		
	inherited::Free();   	
}


pascal void TAlnSequence::DeInstall(void) 
//unlink fSeq: TSequence from TTextView stuff
//! must unlink seq from EditSeq or will dispose seq in next*2 install
{
	//- InstallSelection(TRUE, FALSE); -- not in Mapp3 ?! Need alternate?
	fSeq= NULL;
	fTextData->fChars= NULL;
	StuffText(fSavedTEHandle);  
	// ^^^^ this prevents sequences from Being Trashed !!!
}




#pragma segment ARes
pascal void alnTEDrawHook(void)
VAR
	integer		indx, len; //must be, for regs?
	integer		endx, i ;
	char		ch, lastch;
	CharsPtr		pText;

	longint		cw, pend, skip;
	grafPtr		aPort;
	pt	: point;
	
	procedure pushregs; inline 0x48E7,$FFF8;//movem.l d0-d7/a0-a4,-(sp)
	procedure popregs;	inline 0x4CDF,$1FFF;//movem.l (sp)+,d0-d7/a0-a4
	procedure getD0D1A0( integer var d0, d1, CharsPtr var a0) inline
		0x225F,	//move.l  (a7)+, a1
		0x2288,	/*move.l	a0, (a1)*/		 
		0x225F,	//move.l  (a7)+, a1
		0x3281,	/*move.w	d1, (a1)*/	   
		0x225F,	//move.l  (a7)+, a1
		0x3280();	/*move.w	d0, (a1)*/		 

	//-- test if rgbcolor is time dragger.. only Partly...
	/*! looks like Indx == LineStart and Len == lineLen even
		 when only a small portion of line is drawn !
		 -- try calc TRUE indx for visible text
		 from destrect-viewrect ???
		---*/
{
	pushregs(); //-- ? not needed
	getD0D1A0( indx, len, pText);
	
	//this helps!
	cw= gAlnCharWidth;
	getpen( pt);
	if pt.h < 0 then {
		skip= (-pt.h) / cw;
  	indx= indx + skip;
		len= len - skip;
		move( skip*cw, 0);
		}
		
	pend= len * cw; //<<! integer overflow w/ len ~> 3000 ??
	GetPort(aPort);
	if pend > aPort->portRect.right then {
		skip= (pend - aPort->portRect.right) / cw;
		len= len - skip;
		}
			
	endx= indx + len - 1;
	lastch= ((char)(0));
	for indx= indx to endx do {
		ch= (*pText)[indx];
		if (ch!=lastch) then
		 case ch of
			'A','a': RGBForeColor( gAcolor);
			'C','c': RGBForeColor( gCcolor);
			'G','g': RGBForeColor( gGcolor);
			'T','t',
			'U','u': RGBForeColor( gTcolor);
			otherwise
							 RGBForeColor( gNcolor);
			}
	 DrawChar(ch);
	 lastch= ch;
	//_ if (indx mod 10 == 9) then DrawChar(' '); *//*spacer ??
	 }
	 
	popregs();
} //alnTEDrawHook



pascal void TAlnSequence::MakeTERecord(void) // override 
VAR  
	procPtr		mydraw, mydrawstripped, addr;
{
	inherited::MakeTERecord();
	
	mydraw= &alnTEDrawHook;  // debug this, are we getting right address here !?
	mydrawstripped= procPtr(StripLong(mydraw));
	addr= mydrawstripped;

	//- TECustomHook(intDrawHook, addr, fHTE);
	gDefTEDrawHook= addr; 	//save default

	//!? is alnTEDrawHook bad in mapp3 YES YEYEYEYEYEYEGGEYEGEGEGEG?!?!>?!?!?!?!?>?!
 /* 	if (True) { */
  if (!gConfiguration.hasColorQD) {   
		addr= gDefTEDrawHook;
		//- TECustomHook( intDrawHook, addr, fHTE); 
		}
}


#pragma segment AOpen
pascal void TAlnSequence::ShowReverted(void) /* override */ 
{
	inherited::ShowReverted();
}
		
		
pascal TAlnSequence::HandleMouseDown( VPoint theMouse, TToolboxEvent event, 
							 hysteresis: Point):Boolean; // override 
VAR  theTarget: TView; //?
{		
	theTarget= TView(GetWindow->fTarget); 
	if (theTarget != this) if (BecomeTarget); //GetWindow->SetTarget(this); 
  HandleMouseDown= inherited::HandleMouseDown(theMouse,event,hysteresis);
}
		
	
pascal void TAlnSequence::DoKeyEvent( TToolboxEvent event) // override 
/*! Intercept upArrow, downArrow, return, enter, tab keys:
	up/downarrow -> move this up/down in alnView
	return -> move this down (& to left margin?) in alnView
	enter  -> ?
	tab    -> ?
	chClear chDown chEnd	chHome chEnter chEscape
	chHelp chLeft chPageDown chPageUp
	chReturn chRight chTab chUp
	--------*/
CONST
	down1 	==  1;
	up1			== -1;
	downPage 	==  10;  // this is arbitrary...
	upPage		== -10;
	toBottom=  25000;
	toTop		== -25000;
	
	procedure vertMove(integer nrows) // +/- nrows
	integer		var  aRow, aCol;
	{
		aRow= MinMax( 1, TAlnView(fSuperView).fEditRow + nrows, 
											TAlnView(fSuperView).fNumOfRows);
		aCol= (*fHTE)->selStart;
		TAlnView(fSuperView)->InstallEditSeq(aRow, aCol, aCol, kHighlight);
	}
		
{
	if (Focus) ; //? fix for misplaced drawing when typing ???
	
	CASE event->fCharacter OF
		/*----
		case chEscape:
			if (event->fKeycode == kClearVirtualCode) ...
			else ...;
		-----*/
		
		chTab: ;
		  //? do horizontal shift here ?
			/*---
			if event.IsShiftKeyPressed then vertMove(up1)
			else vertMove(down1);
			-----*/
			
		case chRight:
			if event.IsShiftKeyPressed then 
			else inherited::DoKeyEvent(event);
		case chLeft:
			if event.IsShiftKeyPressed then 
			else inherited::DoKeyEvent(event);
			
		case chUp:
			if event.IsShiftKeyPressed then vertMove(toTop)
			else vertMove(up1);
			
		chDown,
		chEnter, chReturn:
			if event.IsShiftKeyPressed then vertMove(toBottom)
			else vertMove(down1);
			
		chPageDown	:	vertMove( downPage); 
		chPageUp		:	vertMove( upPage);
		chHome			: vertMove( toTop);
		chEnd	 			: vertMove( toBottom);
			
		default: 
			inherited::DoKeyEvent(event);
			
		}
	
	if (( kModifiedFlag /*???? aCommand != gNoChanges*/) && (fSeq!=NULL) then begin
		fSeq.modified; 
		}
}

pascal void TAlnSequence::DoMenuCommand(aCommandNumber:CommandNumber); // override 
{
	inherited::DoMenuCommand(aCommandNumber);
	if ( kModifiedFlag /*????aCommand != gNoChanges*/) && (fSeq!=NULL) then begin
		fSeq.modified; 
		end();
}

pascal void TAlnSequence::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 
{
	inherited::DoMouseCommand(theMouse,event,hysteresis);
	if ( kModifiedFlag /*????aCommand != gNoChanges*/) && (fSeq!=NULL) then begin
		fSeq.modified; 
		end();
}

pascal TTETypingCommand TAlnSequence::DoMakeTypingCommand(Char ch) // override 
VAR  aAlnseqTypingCommand: 	TAlnseqTypingCommand;
{
	if (Focus) ; //? fix for misplaced drawing when typing ???
	New(aAlnseqTypingCommand);
	FailNIL(aAlnseqTypingCommand);
	aAlnseqTypingCommand->ITETypingCommand(this, ch);
	DoMakeTypingCommand = aAlnseqTypingCommand;
}

pascal void TAlnseqTypingCommand::DoNormalChar( Char aChar) // override 
VAR  aSeq: TSequence;
		TAlnView		aAlnView;
{
	inherited::DoNormalChar( aChar);
	aAlnView= TAlnView(TAlnSequence(fTEView).fSuperView);
	aSeq= TAlnSequence(fTEView).fSeq;
	aAlnView->UpdateWidth(aSeq);
	if (TAlnSequence(fTEView).Focus) ; //? fix for misplaced drawing when typing ???

		//? fix for integer overflow.... 
	/*---  no good...
	WITH TAlnSequence(fTEView).(*fHTE)^){
		if ((destRect.right < 0)) destRect.right= 20000;
		if ((viewRect.right < 0)) viewRect.right= 20000;
		}
	----*/
}





// TAlnView ------------------------

				
#pragma segment TAlnView
	
	
pascal void TAlnView::IAlnView( TAlnDoc itsDocument, TSeqList aSeqList)
VAR  
		VRect		vbounds;
{
	fAlnDoc= itsDocument;
	fAlnList= aSeqList;
	fLocked= FALSE;
	
	DelColLast( fNumOfCols);   //fix for prefsave...
	fRowheight= GetRowHeight(1);
	DelRowLast( fNumOfRows);  
	RowToVRect( 1, 1, vbounds); 
	gAlnTextStyle= gAlnStyle; //temp
	fEditSeq= NULL;
	
	fEditSeq = TAlnSequence(
					gViewServer->DoCreateViews(fDocument, this, kAlnTEViewID, vbounds.topleft));
	FailNIL(fEditSeq);
	fEditSeq->IAlnSequence(NULL);  //must be NULL or 1st seq gets trashed...
	fEditRow= 0; 
	RemoveSubView(fEditSeq);
	gAlnTextStyle= fEditSeq->fTextStyle;

	/*-- SetPortTextStyle(gAlnTextStyle);  << THIS BOMBS PROG, no window here... */ 
	gAlnCharWidth= CharWidth('G');   
	gMaxViewCellsBugFix= kMaxCoord / (gAlnCharWidth+4);
	gMaxViewCellsBugSeen= false;
}


pascal void TAlnView::UpdateSize(void)
VAR	  diff: longint;
{
	if ((fAlnList!=NULL)) {
		diff= fAlnList.GetSize - fNumOfRows;
		if ((diff>0)) InsRowLast( 1, fRowheight) 	
		else if ((diff<0)) DelRowLast( -diff);
		}	else
		DelRowLast( fNumOfRows);
}


pascal void TAlnView::UpdateWidth(TSequence aSeq)
VAR			alnlen : longint;
			  TScroller		aScroll;
{
	alnlen= GetHandleSize(aSeq->fBases) + 30;
//+?!?? trying to find bug which mangles view when > ~2000 bases 
	if (alnlen>gMaxViewCellsBugFix) then gMaxViewCellsBugSeen= true;
	alnlen= Min( gMaxViewCellsBugFix, alnlen); 
	if alnlen > fNumOfCols then InsColLast( alnlen-fNumOfCols, gAlnCharWidth);  
	SetColWidth(1, fNumOfCols, gAlnCharWidth);  
	if (fAlnDoc!=NULL) && (fAlnDoc->fAlnHIndex!=NULL))
		fAlnDoc->fAlnHIndex->UpdateWidth(); //keep ruler in sync
}

pascal void TAlnView::UpdateAllWidths(void)
VAR			alnlen : longint;
			  TScroller		aScroll;
		pascal void findMaxLength( TSequence aSeq)
		{
			alnlen= Max( alnlen, GetHandleSize(aSeq->fBases));
//+?!?? trying to find bug which mangles view when > ~2000 bases 
			if (alnlen>gMaxViewCellsBugFix) then gMaxViewCellsBugSeen= true;
			alnlen= Min( gMaxViewCellsBugFix, alnlen); 
		}
{
	alnlen= 0;
	fAlnList->Each( findMaxLength);
	alnlen= alnlen + 30; 
	if alnlen > fNumOfCols then InsColLast( alnlen-fNumOfCols, gAlnCharWidth);  
	SetColWidth(1, fNumOfCols, gAlnCharWidth);  
	if (fAlnDoc!=NULL) && (fAlnDoc->fAlnHIndex!=NULL))
		fAlnDoc->fAlnHIndex->UpdateWidth(); //keep ruler in sync
}


pascal void TAlnView::DeInstallEditSeq(void)
VAR  	lastCmd	: TCommand;
			VRect		vbounds;
{
	if ((fEditRow!=0)) { 
	  // uninstall old editrow 
		// Commit the last command to prevent undo from applying to the wrong edit text 
		lastCmd= gApplication->GetLastCommand();
		if ((lastCmd != NULL) && (lastCmd->fContext == fEditSeq))
			gApplication->CommitLastCommand();
		UpdateWidth(fEditSeq->fSeq);
		fEditSeq->DeInstall();			
		RemoveSubView(fEditSeq);

		if (TAlnDoc(GetWindow->fDocument).fUseColor) {
			//Partial fix for lack of TEDrawHook...
			RowToVRect( fEditRow, 1, vbounds);
			InvalidateVRect( vbounds);
			}
			
		fEditRow= 0;
		}
}


pascal void TAlnView::InstallEditSeq(integer row, selStart,selEnd, boolean doLight)
VAR
	VRect		vbounds ;
	Rect		qdbounds;
	aSeq		: TSequence;
	aCell		: GridCell;
	vp			: Vpoint;
{
	DeInstallEditSeq();

	fEditRow= row;
	if ((row != 0)) {
		aSeq= TSequence(fAlnList->At( row));
		RowToVRect( row, 1, vbounds);
		//- ViewToQDRect( vbounds, qdbounds); *//*???
		
		AddSubView( fEditSeq);  //?! must do before locate/resize so suview is set
 	  if (fEditSeq->Focus()) ; //??

		fEditSeq->ReplaceSeq( aSeq);

		WITH vbounds){ //was qdbounds
			SetVpt( vp, left, top);
			fEditSeq->Locate( vp, kDontRedraw);
			SetVpt( vp, right-left,bottom-top);
			fEditSeq->Resize( vp, kDontRedraw);
			}
			
		SetSelect( selStart, selEnd, fEditSeq->fHTE);  
		if (doLight) {
			aCell.h= selStart+1; 
			aCell.v= row;
		  SelectCell( aCell, FALSE, TRUE, TRUE);
			ScrollSelectionIntoView( TRUE); //< redraw does highlight !
		  SelectCell( aCell, FALSE, TRUE, FALSE); //turn off highlight

			//- fEditSeq->InstallSelection(FALSE, TRUE);
			//- TESetSelect( selStart, selEnd, fEditSeq->fHTE);
			}
	
		if (fEditSeq->BecomeTarget()) ; // GetWindow->SetTarget(fEditSeq); 
		}
}

pascal void TAlnView::registerInsertLast( TSequence aSeq)
{
	InsRowLast( 1, fRowheight);	
	UpdateWidth(aSeq);	
}

pascal void TAlnView::addToAlnList( TSequence aSeq)
VAR  aRect: VRect;
{
	fAlnList->InsertLast( aSeq);
	//-- registerInsertLast( aSeq); 
	InsRowLast( 1, fRowheight);	
	UpdateWidth( aSeq);	
	/*-- // insrowlast does this inval....
	RowToVRect( fNumOfRows, 1, aRect);
	InvalidateVRect( aRect); 
	----*/
}

pascal TSequence TAlnView::SeqAt( integer aRow)  
{
	SeqAt= TSequence(fAlnList->At( aRow));  
}


pascal void TAlnView::MakeConsensus(void)
VAR
	arow	: integer;
	arect	: VRect;
{
	fAlnList->MakeConsensus();
	arow= fAlnList->ConsensusRow();
	if ((arow>0)) {
		RowToVRect( arow, 1, aRect);
		InvalidateVRect( aRect);
		}
}

 
pascal charsHandle TAlnView::FindCommonBases( integer minCommonPerCent)
VAR  hFirst: CharsHandle;
{
	gMinCommonPercent= minCommonPerCent;
	FindCommonBases= fAlnList->FindCommonBases(minCommonPercent, hFirst);
	Handle(hFirst)= DisposeIfHandle( hFirst);
}


pascal void TAlnView::Close(void) /* override */ 
{
		DeInstallEditSeq();
		inherited::Close();
}

pascal void TAlnView::HiliteCommonBases(void)
var  
	TSequence		cons;
	CharsHandle		hCon, hSeq, hFirst;
	longint		len, maxlen, conlen, ibase;
	arow	: integer;
	integer		nseq ;
	CharsHandle		hMaxbase;
	
		procedure markCommon(TSequence aSeq)
		integer		var ibase;
				char		ch;
				gridCell		aCell;
		{
			nseq= nseq+1;
			if !(aSeq.isConsensus || (aSeq->fKind=kOtherSeq)) then begin
				handle(hSeq)= aSeq->fBases;
				//- len= GetHandleSize(Handle(hSeq));
				for ibase= 0 to maxlen-1 do  
				 if (UprChar((*hSeq)^[ibase]) == (*hMaxbase)^[ibase]) then begin
				  aCell.h= ibase+1;
				  aCell.v= nseq;
				  this->SelectCell( aCell, TRUE, !kHighlight, TRUE);
					}
				}
		}
		
{
	cons= fAlnList->Consensus();
	if ((cons=NULL)) cons= TSequence(fAlnList->First());

	if ((cons!=NULL)) {
		arow= fAlnList->GetIdentityItemNo( cons);
		handle(hCon)= cons->fBases;   
		
		hMaxBase= fAlnList->FindCommonBases(gMinCommonPercent, hFirst); 
		
		maxlen= GetHandleSize(handle(hMaxBase));
		this->InvalidateSelection();
		this->SetEmptySelection( !kHighlight);		
		nseq= 0;
		fAlnList->Each( markCommon);
		this->InvalidateSelection();
		DisposHandle(	handle(hMaxBase)); 
		DisposHandle(	handle(hFirst)); 
		}
}
 


#pragma segment ARes

pascal void TAlnView::Free(void) // override 
VAR	 itsEditSeq: TAlnSequence;
{
	FreeData();
	fAlnList= NULL; //! fAlnList == fSeqList is owned by Doc !
	
	itsEditSeq= fEditSeq;	 
	if ((itsEditSeq != NULL) && (itsEditSeq->fSuperView != NULL))
		itsEditSeq->fSuperView->RemoveSubView(itsEditSeq);

	inherited::Free();
	TObject(itsEditSeq)= FreeIfObject(itsEditSeq);					 
}

pascal void TAlnView::FreeData(void)
{
	//!! TAlnView doesn't own fAlnList data ! is just ptr to fDoc->fSeqList 
}


/************
procedure myDrawAln( charsPtr pText, integer indx, len)
var
	integer		endx, i ;
	ch	: char;

	integer		cw, pend, skip;
	grafPtr		aPort;
	pt	: point;
{
	cw= gAlnCharWidth;
	getpen( pt);
	if pt.h < 0 then begin
		skip= (-pt.h) / cw;
  	indx= indx + skip;
		len= len - skip;
		move( skip*cw, 0);
		end();
	pend= len * cw;
	GetPort(aPort);
	if pend > aPort->portRect.right then begin
		skip= (pend - aPort->portRect.right) / cw;
		len= len - skip;
		end();
			
	endx= indx + len - 1;
	for i= indx to endx do begin
		ch= (*pText)[i];
		case ch of
			'A','a': RGBForeColor( gAcolor);
			'C','c': RGBForeColor( gCcolor);
			'G','g': RGBForeColor( gGcolor);
			'T','t',
			'U','u': RGBForeColor( gTcolor);
			otherwise
							 RGBForeColor( gNcolor);
			end();
	 DrawChar(ch);
	//_ if (indx mod 10 == 9) then DrawChar(' '); *//*spacer ??
	 end();
end; //myDrawAln
*******/


pascal void DrawAlnColors(colorsArray VAR colors, Boolean swapBackColor,
								charsPtr		pText; integer indx, len)
VAR
	longint		endx, i ;
	ch, lastch	: char;
	longint		cw, pend, skip;
	grafPtr		aPort;
	pt	: point;
{
	cw= gAlnCharWidth;
	getpen( pt);
	if (pt.h < 0) {
		skip= (-pt.h) / cw;
  	indx= indx + skip;
		len= len - skip;
		move( skip*cw, 0);
		}
	pend= len * cw;
	GetPort(aPort);
	if (pend > aPort->portRect.right) {
		skip= (pend - aPort->portRect.right) / cw;
		len= len - skip;
		}
			
	endx= indx + len - 1;
	/*-- backcolor not showing -- need erase or something...
	if ((swapBackcolor)) RGBForeColor( colors[' '])
	else RGBBackColor( colors[' ']);
	---*/
	
	lastch= ((char)(0));
	FOR i= indx to endx do {
		ch= (*pText)[i];
		/*- if ((swapBackcolor)) RGBBackColor( colors[ch])
		else */
		if ((ch!=lastch)) RGBForeColor( colors[ch]);
		DrawChar(ch);
		lastch= ch;
		}
} 



pascal void TAlnView::DrawCell(aCell:GridCell; VRect aRect)
			// override 
VAR
		TAlnSequence		aSeq;
		Handle		hText;
		longint		len ;
		TextStyle		aTextStyle;
{
/*----
  if (fAlnList!=NULL) && (aCell.v <= fAlnList->fSize) then begin
		aSeq= TAlnSequence(fAlnList->at(aCell.v));
		if aSeq!=NULL then begin
			aTextStyle = aSeq->fTextStyle;
			SetPortTextStyle(aTextStyle);
		
			moveto( aRect.left, aRect.bottom-4);  
			hText= aSeq->fSeq.fBases;
			len= GetHandleSize(hText);
			HLock(hText);
			myDrawAln( charsPtr((*hText)), 0, len);
			HUnlock(hText);
			end();
		end();
-------*/
}



//+?!?? trying to find bug which mangles view when > ~2000 bases 
pascal void TAlnView::CellToVRect( GridCell aCell, VRect VAR aRect) // override 
VAR
	 	longint		width; //! these where shorts 
		longint		height;
{
	inherited::CellToVRect( aCell, aRect);
/***
	if ((aCell.h < 1) || (aCell.v < 1) 
	|| (aCell.h > fNumOfCols) || (aCell.v > fNumOfRows))) 
		aRect= gZeroVRect 
	else {							 
		width= fColWidths->GetValue(aCell.h);
		aRect.left= width * (aCell.h - 1);
		/*+ if (fColWidths->fNoOfChunks == 1) then  
			aRect.left= width * (aCell.h - 1) 
		else
		 aRect.left= fColWidths->SumValues(1, aCell.h - 1);
		*/

		aRect.right= aRect.left + width;

		height= fRowHeights->GetValue(aCell.v);
		if (fRowHeights->fNoOfChunks == 1) then
			aRect.top= height * (aCell.v - 1) 
		else
			aRect.top= fRowHeights->SumValues(1, aCell.v - 1);
		aRect.bottom= aRect.top + height;
		}
***/
}

//+?!?? trying to find bug which mangles view when > ~2000 bases 
pascal TAlnView::VPointToLastCell( aPoint: VPoint):GridCell; // override 
VAR  aCell: GridCell;  longint lwidth,
{
	VPointToLastCell= inherited::VPointToLastCell( aPoint);
/***
	//+ aCell.h= fColWidths->FindItem(aPoint.h);
	aCell.h= 0;
	if ((aPoint.h >= 0) && (aPoint.h <= fColWidths->fTotal))) {
			if (fColWidths.(*fChunks)^[0].value > 0)) {
				lwidth= 1 +  (aPoint.h - 1) / fColWidths.(*fChunks)^[0].value;
				lwidth= Min( lwidth, fColWidths->fNoOfItems); 
				if (lwidth>Maxint) then aCell.h= Maxint
				else aCell.h= lwidth;
				}			}		else if (aPoint.h == 0) then
			aCell.h= 1;
	
	if (aCell.h == 0) then aCell.h= fNumOfCols;

	aCell.v= fRowHeights->FindItem(aPoint.v);
	if (aCell.v == 0) then aCell.v= fNumOfRows;

	VPointToLastCell= aCell;
***/
}

//+?!?? trying to find bug which mangles view when > ~2000 bases 
pascal Boolean TAlnView::Focus(void) // override 
VAR
		 Point		aPt, bPt, deltaOrigin;
		 Rect		visQDRect;
		 
{
	Focus= inherited::Focus;
/****
	if (this->IsFocused()))	
		Focus= TRUE 

	else if (this->FocusOnSuperView())) {
 		//? don't do it -- use superview focus here ?!?!?
 
		SetOrigin(fQDOrigin.h, fQDOrigin.v);	 
		if ((gCurrPrintHandler != NULL) && (gCurrPrintHandler->fView == this))) {
			//- deltaOrigin= fQDOrigin - gCurrPrintHandler.GetQDOrigin
			aPt= fQDOrigin;
			bPt= gCurrPrintHandler->GetQDOrigin();
			SubPt( bPt, aPt);
			deltaOrigin= bPt;
			}		else if (fSuperView!=NULL)) {
			//- deltaOrigin= fQDOrigin - fSuperView->fQDOrigin
			aPt= fQDOrigin;
			bPt= fSuperView->fQDOrigin;
			SubPt( bPt, aPt);
			deltaOrigin= bPt;
			}		else
			deltaOrigin= gZeroPt;

		gFocusedView= this;		// Now we have the basic focus 

		//- // Add the clipping 
		/*--
		this->GetVisibleQDRect(visQDRect);
		this->OffsetAndClipFurtherTo(visQDRect, deltaOrigin);
 		---*/
	 
		Focus= TRUE;
		}	else	 
		Focus= FALSE;
***/
}

//+?!?? trying to find bug which mangles view when > ~2000 bases 
pascal void TAlnView::UpdateCoordinates(void) // override 
CONST  kMaxOriginFixup == 16000; //was 1024; // The maximum amount by which to _fixup_
VAR
		 VPoint		localOriginInSuper, aVpt;
		 VPoint		actualViewToQDOffset;
		 VRect		itsExtent, aVRect;
		 Point		deltaOrigin, aPt; 
		 VRect		itsFrame, visibleExtent;
{
	inherited::UpdateCoordinates();
/***
	this->GetLocalOffsetInSuper(localOriginInSuper);

	if (gCurrPrintHandler!=NULL) && (gCurrPrintHandler->fView == this)
) {
		gCurrPrintHandler->GetViewToQDOffset(actualViewToQDOffset);
		/*- actualViewToQDOffset = actualViewToQDOffset + 
			VPoint(gCurrPrintHandler->GetQDOrigin()); */
		PtToVPt(gCurrPrintHandler->GetQDOrigin(), aVpt);
		AddVPt(aVpt, actualViewToQDOffset);
		}	else {
		/*- actualViewToQDOffset = fSuperView->fViewToQDOffset + VPoint(fSuperView->fQDOrigin) 
						- localOriginInSuper;*/
		PtToVPt(fSuperView->fQDOrigin, actualViewToQDOffset);
		aVPt= fSuperView->fViewToQDOffset;
		AddVPt(aVPt, actualViewToQDOffset);
		SubVPt(localOriginInSuper, actualViewToQDOffset);
		}			
		
	fViewToQDOffset= actualViewToQDOffset;	 
	this->GetExtent(itsExtent);
	//- if (VRect(-kMaxCoord,-kMaxCoord, kMaxCoord, kMaxCoord)->Contains(itsExtent))
	if (itsExtent.left >= -kMaxCoord) && (itsExtent.right <= kMaxCoord)
		&& (itsExtent.top <= -kMaxCoord) && (itsExtent.bottom <= kMaxCoord)
) {
		//-fQDOrigin= actualViewToQDOffset->ToPoint();
		aPt= fQDOrigin;
		SetPt( aPt, actualViewToQDOffset.h, actualViewToQDOffset.v);
		fViewToQDOffset= gZeroVPt;
		}	else {
		SetPt( deltaOrigin, 
					(actualViewToQDOffset.h % kMaxOriginFixup), 
					(actualViewToQDOffset.v % kMaxOriginFixup) );
		//- fViewToQDOffset= actualViewToQDOffset - VPoint(deltaOrigin);
		PtToVPt( deltaOrigin, aVpt);
		fViewToQDOffset= VPtSubVPt( actualViewToQDOffset, aVPt);
		fQDOrigin= deltaOrigin;
		}

	if ((gDrawingPictScrapView == this) || ((gCurrPrintHandler != NULL) && (gCurrPrintHandler->fView == this)))
) {
		this->GetExtent(itsExtent);
		fVisibleExtent= itsExtent;
		}	else {
		this->GetFrame(itsFrame);
		//- visibleExtent= itsFrame && fSuperView->fVisibleExtent;
		aVrect= fSuperView->fVisibleExtent;
		if (SectVRect(itsFrame, aVrect, visibleExtent)) ;
		this->SuperToLocalVRect(visibleExtent);
		fVisibleExtent= visibleExtent;
		}
*****/		
}


//+?!?? trying to find bug which mangles view when > ~2000 bases 
pascal void TAlnView::Draw(VRect area) /* override */ 
VAR
	VRect		aRect, bRect;
	GridCell		startCell, stopCell, startCellToDraw;
	VRect		cellsArea, localArea;
{
	inherited::Draw( area);
/*****
	if ((fNumOfRows > 0) && (fNumOfCols > 0))) {

		startCell= this->VPointToLastCell(area.topLeft);
		stopCell = this->VPointToLastCell(area.botRight);

		this->CellToVRect(startCell, aRect);
		this->CellToVRect(stopCell, bRect);
		bRect.top= aRect.top; 
		bRect.left= aRect.left; //[topLeft]= aRect[topLeft];
		localArea= bRect;

		startCellToDraw= startCell;
		cellsArea= localArea;
		if (area.top >= aRect.bottom - (fRowInset / 2))) {
			startCellToDraw.v= startCellToDraw.v + 1;
			cellsArea.top=  cellsArea.top + (aRect.bottom - aRect.top); //GetLength(vSel);
			}

		this->DrawRangeOfCells(startCellToDraw, stopCell, cellsArea);
		}
****/
}




pascal void TAlnView::DrawRangeOfCells(GridCell startCell, stopCell, VRect aRect) 
										// override */ /*from TGridView method
VAR
	colWidth, rowHeight, qdleft	: longint;
	vleft			: longint;
  TextStyle		aTextStyle;
	Rect		aQDRect;

	function showA( c: char): char;
	begin 	if (c='A') || (c='a') then showA= c else showA= ' ';
	end();
	function showC( c: char): char;
	begin 	if (c='C') || (c='c') then showC= c else showC= ' ';
	end();
	function showG( c: char): char;
	begin 	if (c='G') || (c='g') then showG= c else showG= ' ';
	end();
	function showT( c: char): char;
	begin 	
		if (c in ['T','t','U','u']) then showT= c else showT= ' ';
	end();
	function showN( c: char): char;
	begin 	
		if (c in ['A','a','C','c','G','g','T','t','U','u'])
					then showN= ' ' else showN= c;
	end();
	function showEach( c: char): char;
	begin 	showEach= c;
	end();

 procedure DrawOneColor(function showChar(c:char):char);
	const
		maxbuf	== 500; //are any displays wider than maxbuf*charwidth?
 var
 		longint		i, j, ibeg, iend, rowtop, rowbot;
		len			: longint;
		aSeq		: TSequence;
		hSeq		: CharsHandle;
		cbuf		: packed array [0..maxbuf] of char;
 begin
	ibeg= startCell.h - 1;
	iend= stopCell.h;

	rowtop= aRect.top;
	FOR j = startCell.v TO stopCell.v){
		rowbot = rowtop + rowHeight - fRowInset;  
		aSeq= TSequence(fAlnList->at(j));
		if (aSeq!=NULL) && (aSeq->fBases!=NULL) then begin
			handle(hSeq)= aSeq->fBases;
			len= min(maxbuf, min( GetHandleSize(handle(hSeq)),iend) - ibeg);
			for i= 0 to len-1 do cbuf[i]= showChar((*hSeq)^[i+ibeg]);
			
			moveto( qdleft+1, rowbot-5); //<< fix fudged v !
			
			DrawText( &cbuf, 0, len);
			end();
		rowtop = rowbot + fRowInset;
		}
	end(); //DrawOneColor

 pascal void DrawAllColors(void)
 VAR
 		longint		i, j, ibeg, iend, rowtop, rowbot;
		len			: longint;
		aSeq		: TSequence;
		hSeq		: CharsHandle;
 {
	ibeg= startCell.h - 1;
	iend= stopCell.h;
	rowtop= aQDRect.top;
	FOR j = startCell.v TO stopCell.v){
		rowbot = rowtop + rowHeight - fRowInset;  
		aSeq= TSequence(fAlnList->at(j));
		if ((aSeq!=NULL) && (aSeq->fBases!=NULL)) {
			handle(hSeq)= aSeq->fBases;
			len= min( GetHandleSize(handle(hSeq)),iend) - ibeg;
			moveto( qdleft+1, rowbot-5); //<< fix fudged v !
			if ((aSeq->fKind == kAmino))
				DrawAlnColors(gAAcolors, gSwapBackground, (*hSeq), ibeg, len)
			else
				DrawAlnColors(gNAcolors, gSwapBackground, (*hSeq), ibeg, len);
			}
		rowtop = rowbot + fRowInset;
		}
	} //DrawAllColors
		
{
	aRect.left = aRect.left + ((fColInset) >> 1);	// fColInset / 2 
	aRect.top = aRect.top + ((fRowInset) >> 1); 	// fRowInset / 2 
	/***
	LocalToWindow( aRect.topleft); //+ ?? mapp3 fix for bad aRect ???
	LocalToWindow( aRect.botright); /*+ sort of -- window coords are not correct
			as drawing is done rel. to localview(0,0), but we draw parts of
			all of long sequence now -- BUT we still get crash when view scrolled
			to the long sequence -- TScroller problem ???*/
			//+ !! somewhere someone is probably doing short/int overflow by mistake !
	***/  
	
	VRectToRect( aRect, aQDRect); //??? do we want this -- truncs at maxint 
	qdleft 		= aQDRect.left;
	colWidth  = GetColWidth(1);
	rowHeight = GetRowHeight(1); //fails w/ fNumOfRows=0 !

	//! need to let TE do its hightlighting... don't need TEDeact??
  if (fEditRow != 0) {
 	  if (fEditSeq->Focus()) ; //??
		fEditSeq->Show(TRUE,FALSE);  
		//- if fEditSeq.Focus then TEDeactivate(fEditSeq->fHTE); ---
		if (Focus) ;
		}
		
	SetPortTextStyle(gAlnTextStyle);

	if TAlnDoc(GetWindow->fDocument).fUseColor then begin
		/* DrawAllColors is slower than DrawOneColor x 5 
			-- do DrawOne for nucs ??*/
		DrawAllColors(); 
		/*--
		RGBForeColor( gAcolor); DrawOneColor(showA);
		RGBForeColor( gCcolor); DrawOneColor(showC);
		RGBForeColor( gGcolor); DrawOneColor(showG);
		RGBForeColor( gTcolor); DrawOneColor(showT);
	  RGBForeColor( gNcolor); DrawOneColor(showN);
		---*/
		end
	else
		DrawOneColor(showEach);
	
	if (fEditRow != 0) begin
	  if fEditSeq.Focus then TEActivate(fEditSeq->fHTE);  
		if Focus then ;
		}
}



pascal void TAlnView::DrawContents(void) // override 
{
	//?? if ((fEditSeq!=NULL)) fEditSeq->show(FALSE,FALSE);  
	inherited::DrawContents();
  /*?? if ((fEditSeq!=NULL)) fEditSeq->show(TRUE,FALSE);  */ 
}


pascal void TAlnView::CalcMinFrame(VRect VAR minFrame) // override 
{
	inherited::CalcMinFrame( minFrame);
	/*----
	minFrame.right=  max(minFrame.right, 10 + fNumOfCols * gAlnCharWidth);
	minFrame.bottom=  max(minFrame.bottom, 10 + fNumOfRows * GetRowHeight(1));
	-----*/
}



pascal void TAlnView::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 
VAR
	aRow, aCol	: short;
	aCell	: GridCell;
	aAlnSlider		: TAlnSlider;
	boolean		aDoubleClick;
	vpt		: VPoint;
	GridViewPart		gridPart;	
{
	aDoubleClick= event->fClickCount > 1;
	gridPart= IdentifyPoint(theMouse, aCell);
	aCol= aCell.h;
	
	if (aDoubleClick) {
		inherited::DoMouseCommand(theMouse,event,hysteresis); 
		}		
	else if ((event.IsShiftKeyPressed |event.IsCommandKeyPressed 
				|| event->IsOptionKeyPressed())) { 
		inherited::DoMouseCommand(theMouse,event,hysteresis);
		}		
	else if (gridPart != badChoice) {
		aCell	= VPointToLastCell(theMouse);
		//if TRUE then begin
		if (isCellSelected(aCell)) {
			New(aAlnSlider);
			FailNIL(aAlnSlider);
			aAlnSlider->IAlnSlider(this, aCol);
			PostCommand( aAlnSlider);
			}		else {
			SetEmptySelection(kHighlight);
			event->fEventRecord.modifiers = bOr(event->fEventRecord.modifiers,shiftKey);
			 	//!? mimic shiftIsDown when no selection ... ?
			inherited::DoMouseCommand(theMouse,event,hysteresis); 
			}
		}
}



pascal TAlnView::HandleMouseDown( VPoint theMouse, TToolboxEvent event, 
							 hysteresis: Point):Boolean; // override 
VAR
		aCell	: GridCell;
{		
	if (!(fLocked || event.IsShiftKeyPressed || event.IsCommandKeyPressed 
		|| event->IsOptionKeyPressed())) {  
		SetEmptySelection(kHighlight); 
		if (fAlnDoc->fAlnIndex!=NULL) fAlnDoc->fAlnIndex->SetEmptySelection(kHighlight); 
		aCell= VPointToLastCell(TheMouse);
		InstallEditSeq(aCell.v,0,0,FALSE);
		}
	
  HandleMouseDown= inherited::HandleMouseDown(theMouse,event,hysteresis);
}
		

pascal Boolean TAlnView::ContainsClipType(ResType aType) // override 
{
	ContainsClipType = aType == kAlnScrapType;
	//!? also (aType == 'TEXT') can be pasted as seq... w/ SeqReadScrap...
}

pascal void TAlnView::WriteToDeskScrap(void) // override 
VAR
		textScrap	: Handle;
		err				: OSErr;
		format		: integer;
{
	format= kGenBank;
	/*--- ??? causing bombs here !!!
	if (fAlnDoc->fFormatPop == NULL) format= kGenBank
	else format= fAlnDoc->fFormatPop.fCurrentItem; 
	---*/
	fAlnList->ClearSelections();
	textScrap= fAlnList->doWriteHandle( format);  
	err = gClipboardMgr->PutDeskScrapData('TEXT', textScrap);
	textScrap = DisposeIfHandle(textScrap);
	FailOSErr(err);
}



		



// TAlnSlider --------------------------------------

pascal void TAlnSlider::IAlnSlider( TAlnView itsAlnView, integer atRC)
{
	fAlnView= itsAlnView;	
	fOldSeqs= NULL;
	fNewSeqs= NULL;
	fOldSelection= NULL;
	fNewSelection= NULL;
	
	IRCShifter(itsAlnView->fDocument, itsAlnView, NULL, h, atRC);
	fCausesChange= TRUE; //!? fix since RCShifter thinks we make no changes
	
	fOldSeqs= TSeqList(NewList); FailNIL( fOldSeqs);
	fNewSeqs= TSeqList(NewList); FailNIL( fNewSeqs);
	fOldSelection= NewRgn; FailNil( fOldSelection);
	fNewSelection= NewRgn; FailNIL( fNewSelection);
	CopyRgn( itsAlnView->fSelections, fOldSelection);
	CopyRgn( fOldSelection, fNewSelection);
}

pascal void TAlnSlider::Free(void) // override 
{
	if ((fNewSeqs!=NULL)) {
		fNewSeqs->DeleteAll(); //forget objects -- doesn't affect objects 
		fNewSeqs->Free(); //drop list, not objects
		}
	if ((fOldSeqs!=NULL)) fOldSeqs->FreeList(); //drop all objects && list
	if ((fOldSelection!=NULL)) DisposeRgn( fOldSelection);
	fOldSelection= NULL;
	if ((fNewSelection!=NULL)) DisposeRgn( fNewSelection);
	fNewSelection= NULL;
	
	inherited::Free();
}


pascal TTracker TAlnSlider::TrackMouse(TrackPhase aTrackPhase, VAR anchorPoint, previousPoint,
								 VPoint		nextPoint; Boolean mouseDidMove) // override 
VAR
	GridCell		aCell;
	
	pascal void DoSlide(void)
	VAR
		aCell	: GridCell;
		longint		start, nbases;
		integer		left, right, dist, aRow;
		oldSeq, newSeq	: TSequence;
		vLoc	: VRect;
	{
		dist= fNewRC - fOldRC;
		OffsetRgn( fNewSelection, dist, 0);
		
		left= fGrid.FirstSelectedCell.h;
		right= fGrid.LastSelectedCell.h;
		FOR aRow= fGrid.FirstSelectedCell.v TO fGrid.LastSelectedCell.v){
			aCell.h= left;
			aCell.v= aRow;
			while (aCell.h<right) && !fGrid->isCellSelected(aCell))aCell.h= aCell.h+1;
			start= aCell.h - 1;
			while (aCell.h<right) && fGrid->isCellSelected(aCell))aCell.h= aCell.h+1;
			nbases= aCell.h - start;
			if ((nBases>0)) {
				oldSeq= fAlnView->fAlnList->SeqAt(aRow);
				oldSeq->fIndex= aRow;
				oldSeq->SetSelection( start, nbases);
				
				newSeq= oldSeq->Slide( dist); //< ?? TSeqChangeCmd !?
				
				oldSeq->ClearSelection();		// 12nov92 - fix for write trunc bug? 
				if ((newSeq!=NULL)) {
					newSeq->ClearSelection();
					fOldSeqs->InsertLast(oldSeq);
					fNewSeqs->InsertLast(newSeq);
	  			}
				}
			}
		}
		
{
	if (aTrackPhase == trackRelease then {
		aCell	= fGrid->VPointToLastCell(nextPoint);
		fNewRC= aCell.vh[fRowCol];
		if fNewRC != fOldRC) DoSlide;
		}
	TrackMouse = this;
}


pascal void TAlnSlider::DoIt(void) // override 
/* insert "-" at left/5'/bottom of sequence if dist>0
	squeeze out "-" at top/3' of sequence.
	If dist<0 then slide opposite direction.
*/
VAR	
	integer		zeroshift, left, i;
	
	pascal void UpdateEdWinds( TSequence oldSeq, newSeq)
		pascal void checkOpenEdWinds( TWindow aWind)
		{
			if (Member(TObject(aWind), TSeqedWindow)) 
				if (TSeqedWindow(aWind).fSequence == oldSeq) { 
					TSeqedWindow(aWind)->ReplaceSeq(newSeq);
					aWind->ForceRedraw();
					}
		}
	{
		TDocument(fContext)->ForAllWindowsDo(checkOpenEdWinds);
	}

	pascal void ReplaceViewSeq( TSequence aSeq)
	VAR  vLoc	: VRect;
	{
		fAlnView->fAlnList->AtPut( aSeq->fIndex, aSeq);
		fAlnView->RowToVRect( aSeq->fIndex, 1, vLoc);
		fAlnView->InvalidateVRect( vLoc);
	}
	
{
	if fNewRC != fOldRC then {
		fNewSeqs->Each( replaceViewSeq);
		zeroshift= fAlnView->fAlnList->ZeroOrigin(); //!?
		if ((zeroshift!=0)) {
			// THIS is getting messy w/ undo/redo because zeroOrigin affects All seqs!
			fAlnView->UpdateAllWidths();
			left= (*fNewSelection)->rgnBBox.left;
			if (left + zeroshift <= 0) then zeroshift= -left + 1; //??
			offsetRgn( fNewSelection, zeroshift, 0); 
			fAlnView->ForceRedraw(); 
			}
		fGrid->SetSelection( fNewSelection, !kExtend, kHighlight, kSelect);
		FOR i= 1 to fNewSeqs->GetSize())UpdateEdWinds( TSequence(fOldSeqs->At(i)), TSequence(fNewSeqs->At(i)));
		fCommandDone= True;
		}
}


#pragma segment ADoCommand

pascal void TAlnSlider::UndoIt(void) // override 
VAR  tmp: TSeqList; RgnHandle tmpRgn,
{
	if fNewRC != fOldRC then {
		tmp= fOldSeqs; 
		fOldSeqs= fNewSeqs;
		fNewSeqs= tmp;
		tmpRgn= fOldSelection;
		fNewSelection= fOldSelection;
		fOldSelection= tmpRgn;
		DoIt();
		fCommandDone= False; 
		}
}


#pragma segment ADoCommand

pascal void TAlnSlider::RedoIt(void) // override 
{
	UndoIt();
	fCommandDone= True; 
}

pascal void TAlnSlider::Commit(void) // override 
{
	this->DoNotification();
}

pascal void TAlnSlider::DrawFeedback( VPoint anchorPoint, nextPoint) // override 
VAR
	RgnHandle		aSelectRgn;
	deltah 	: longint;
	abounds	: VRect;
	aCell		: GridCell;
{
	// horizontal slider...
	aCell	= fGrid->VPointToLastCell(nextPoint);
	aSelectRgn= NewRgn;
	fGrid->CellsToPixels( fGrid->fSelections, aSelectRgn);
	fGrid->CellToVRect( aCell, abounds);
	deltah= abounds.left - anchorPoint.h;
	OffsetRgn( aSelectRgn, deltah, 0);
	FrameRgn( aSelectRgn);
	DisposeRgn( aSelectRgn);
}


		
		
		
// TAlnIndex	 ------------------

pascal void TAlnIndex::GetItemText(integer anItem, str255 var aString) // override 
VAR  aSeq: TSequence;
{
 	aString= '';
  if ((fAlnList!=NULL) && (anItem <= fAlnList->fSize)) {
		aSeq= TSequence(fAlnList->at(anItem));
		if aSeq!=NULL then aString= aSeq->fName;
		}
}

pascal void TAlnIndex::UpdateSize(void)
VAR	diff: integer;
{
	if ((fAlnView!=NULL)) {
		diff= fAlnView->fNumOfRows - fNumOfRows;
		if ((diff>0)) InsRowLast( 1, fAlnView->fRowheight) 	
		else if ((diff<0)) DelItemLast( -diff);
		}	else
		DelItemLast(fNumOfRows);
}

pascal void TAlnIndex::ReSelect(RgnHandle indexRegion)
VAR  aCell: GridCell;
{
	/*-----
	aCell = (*indexRegion)->rgnBBox.topLeft;
	if (!IsCellVisible(aCell)) PositionAtCell(aCell);				 
	----*/
	
	if (!EqualRect((*indexRegion)->rgnBBox, (*fSelections)->rgnBBox)) {
		/*---- ??
		WITH fAlnDoc){
			fAlnView->SetEmptySelection(kHighlight);  
			}
		-----*/
		SetSelection(indexRegion, kDontExtend, kHighlight, kSelect);
		}
}



pascal TAlnIndex::HandleMouseDown( VPoint theMouse, TToolboxEvent event, 
							 hysteresis: Point):Boolean; // override 
{		
	fAlnView->DeInstallEditSeq();
	fAlnView->SetEmptySelection(kHighlight); 
	
  HandleMouseDown= inherited::HandleMouseDown(theMouse,event,hysteresis);
}

pascal void TAlnIndex::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 
VAR
	aRow, aCol	: short;
	aCell	: GridCell;
	aAlnShifter		: TAlnShifter;
	boolean		aDoubleClick;
	aName	: Str255;
	vpt		: VPoint;
	GridViewPart		gridPart;
	aSeq	: TSequence;
	
{
	aDoubleClick= event->fClickCount > 1;
	gridPart= IdentifyPoint(theMouse, aCell);
	aRow= aCell.v;
	
	if (aDoubleClick) {
		aSeq= TSequence(fAlnList->At(aRow));
		if (aSeq!=NULL)) TAlnDoc(fDocument)->OpenSeqedWindow(aSeq);
		}		
	else if ((event.IsShiftKeyPressed |event->IsCommandKeyPressed())) { 
		inherited::DoMouseCommand(theMouse,event,hysteresis) 
		}		
	else if (gridPart != badChoice) {
		aCell	= VPointToLastCell(theMouse);
		//if TRUE then begin
		if (isCellSelected(aCell)) {
			GetItemText(aRow, aName);
			New(aAlnShifter);
			FailNIL(aAlnShifter);
			aAlnShifter->IAlnShifter(this, fAlnView, aRow, aName);
			PostCommand( aAlnShifter);
			}		else
			inherited::DoMouseCommand(theMouse,event,hysteresis); 
		}
}





// TAlnShifter --------------------------------


pascal void TAlnShifter::IAlnShifter(
		TGridView		itsView; TAlnView itsAlnView, atRC: integer; seqName aName)
{
	fAlnView= itsAlnView;
	fName		= aName;
	IRCShifter(itsView->fDocument, itsView, itsAlnView, v, atRC);
	fCausesChange= TRUE; //!? fix since RCShifter thinks we make no changes

	fAlnView->SetEmptySelection(kHighlight); //? or move select rgn rows ?
}

pascal void TAlnShifter::DoShift( integer fromRC, toRC) // override 
	/*!? This won't do, 
			need to move fAlnList.contents ?? or revise TAlnIndex.GetItemText
	--  don't Swap from/to,
		  need to remove seq at from, then insert seq at to.
			Need to shift a multiple selection...
	--*/
VAR
	vRect		vFrom, vTo;
	TSequence		fromSeq, toSeq;
	integer		aFromRow, aToRow, indx, saveEdit; 
	integer		saveStart, saveEnd;
	GridCell		aCell;
	
	pascal void relocate( TSequence aSeq)
	vRect		VAR vLoc; vPoint vSize,
	{
		indx= indx+1;
		if (indx >= aFromRow) && (indx <= aToRow) then {
			//! also need to shift any selection or deselect all...
		  fAlnView->RowToVRect( indx, 1, vLoc);
			fAlnView->InvalidateVRect( vLoc);
			if indx == saveEdit then {
				fAlnView->fEditRow= saveEdit;
				fAlnView->fEditSeq->Locate( vLoc.topleft, TRUE);
				vSize= fAlnView->fEditSeq.fSize;
				fAlnView->fEditSeq->Resize( vSize, FALSE);  
				}
			aCell.v= indx;
			fGrid->InvalidateCell(aCell);
			}
	}
	
{
	//- for firstSelected to lastSelected do... 

	fromSeq	= TSequence(fAlnView->fAlnList->At(fromRC));
	toSeq		= TSequence(fAlnView->fAlnList->At(toRC));
	if (fromSeq!=NULL) && (toSeq!=NULL) then {
		fAlnView->fAlnList->Delete(fromSeq);
		fAlnView->fAlnList->InsertBefore(toRC, fromSeq);
		if fromRC > toRC then begin 
			aFromRow= toRC; aToRow= fromRC; 
			}		else begin
			aFromRow= fromRC; aToRow= toRC; 
			}
		saveEdit= fAlnView->fEditRow;
		if saveEdit == fromRC then saveEdit= toRC
		else if fromRC > toRC then saveEdit= saveEdit + 1 
		else if fromRC < toRC then saveEdit= saveEdit - 1; 
		indx= 0;
		aCell.h= 1;
		fAlnView->fAlnList->Each(relocate);	
		fGrid->SetEmptySelection(kHighlight); //?
		}
}







// TAlnHIndex	-----------------

pascal TAlnHIndex::HandleMouseDown( VPoint theMouse, TToolboxEvent event, 
							 hysteresis: Point):Boolean; // override 
{		
	fAlnView->DeInstallEditSeq();
	//- fAlnView->SetEmptySelection(kHighlight); 
  HandleMouseDown= inherited::HandleMouseDown(theMouse,event,hysteresis);
}

pascal void TAlnHIndex::GetText(aCell:GridCell; Str255 VAR aString) // override 
//! don't need w/ .Draw method...
{
 	aString= '';
	if aCell.h mod 10 == 0 then NumToString(aCell.h, aString);
}

pascal void TAlnHIndex::UpdateWidth(void)
{
	if fAlnView->fNumOfCols > fNumOfCols then 
		InsColLast( fAlnView->fNumOfCols-fNumOfCols, gAlnCharWidth)
	else if fAlnView->fNumOfCols < fNumOfCols then
		DelColLast( fNumOfCols-fAlnView->fNumOfCols);
	SetColWidth(1, fNumOfCols, gAlnCharWidth);  
}


pascal void TAlnHIndex::DrawCell(GridCell aCell, VRect aRect) // override 
//! left side of nums are badly clipped on scroll left
var  nums: str255;
		 ws	: integer;
		 Rect		aQDRect;
{
	VRectToRect( aRect, aQDRect);
	if ((aCell.h % 10 == 0)) {
		MoveTo(aQDRect.left, aQDRect.bottom);
		Line(0,-3);
		NumToString( aCell.h, nums);
		ws= StringWidth(nums);
		Move(-ws div 2, -1);
		DrawString(nums);
		}	else if ((aCell.h % 5 == 0)) {
		MoveTo(aQDRect.left, aQDRect.bottom);
		Line(0,-6);
		}
}

pascal void TAlnHIndex::Draw(VRect area) // override 
{
	inherited::Draw(area); 
}


/***********
pascal void TAlnHIndex::Draw(VRect area) // override 
var
	rect		ter ;
	integer		cat, ecat, v, ws ;
	nums	: Str255;
	pt		: point;
{
	v		= area.bottom;
	
	cat= 1;
	pt= TEGetPoint(cat, fSeq->fHTE);
	if pt.h < area.left then begin
	 															 //! this is off by 30-40+ for 2000+ nucs
		ws= gAlnCharWidth;  
	  cat= cat + (area.left - pt.h) / ws;
		pt= TEGetPoint(cat, fSeq->fHTE);
		if pt.h > area.left then
			cat= cat - (pt.h - area.left) / ws;
		end();
		
	ecat= fSeq.(*fHTE)->teLength;

	repeat
		pt= TEGetPoint(cat, fSeq->fHTE);
		if pt.h >= area.left then begin
			moveto(pt.h, v);
			if (cat mod 10 == 0) then begin
				line(0,-3);
				NumToString( cat, nums);
				ws= StringWidth(nums);
				move(-ws div 2,-1);
				drawstring(nums);
				end
			else if (cat mod 5 == 0) then
				line(0,-8);
			end();
		cat= cat + 1;
  until (pt.h >= area.right) || (cat >= ecat);
	
	//- inherited::Draw(area); 
}
********/		
	
		
		
		
		
		
		
// TAlnScroller ------------------------

pascal void TAlnScroller::Free(void) // override 
{
	fRowScroller->fMainScroller= NULL;
	fColumnScroller->fMainScroller= NULL;
	fRowScroller= NULL;
	fColumnScroller= NULL;
	inherited::Free();
}

pascal void TAlnScroller::SingleScroll(VPoint delta, Boolean redraw)
{
	inherited::DoScroll(delta, redraw); // scroll the main view 
	//+? fix TAlnView origin to scroller origin ???
	//+ GetAlnView->fTranslation= fTranslation;
}

pascal void TAlnScroller::DoScroll(VPoint delta, Boolean redraw) // override 
{
	if ((delta.v != 0) && (fRowScroller!=NULL)) fRowScroller->SingleScroll(delta, redraw);
	if ((delta.h != 0) && (fColumnScroller!=NULL)) fColumnScroller->SingleScroll(delta, redraw);
	inherited::DoScroll(delta, redraw); // scroll the main view 
	//+? fix TAlnView origin to scroller origin ???
	//+ GetAlnView->fTranslation= fTranslation;
}



pascal void TSideScroller::Free(void) // override 
{
	fMainScroller->fRowScroller= NULL;
	fMainScroller->fColumnScroller= NULL;
	fMainScroller= NULL;
	inherited::Free();
}

pascal void TSideScroller::SingleScroll(VPoint delta, Boolean redraw)
{
	inherited::DoScroll(delta, redraw); // scroll the main view 
}

pascal void TSideScroller::DoScroll(VPoint delta, Boolean redraw) // override 
{
	if ((delta.h != 0) && (fMainScroller!=NULL)) fMainScroller->SingleScroll(delta, redraw);
	if ((delta.v != 0) && (fMainScroller!=NULL)) fMainScroller->SingleScroll(delta, redraw);
	inherited::DoScroll(delta, redraw); // scroll the main view 
}



/* TAlnDlog ------------------------*/		

pascal void TAlnDlog::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 
{
	if (TAlnDoc(fDocument).fAlnView != NULL)
		TAlnDoc(fDocument).fAlnView->SetEmptySelection(kHighlight);
	inherited::DoMouseCommand( theMouse, event, hysteresis);
}

pascal void TAlnDlog::CalcMinFrame(VRect VAR minFrame) // override 
//! need to check CalcMinFrame of window 
var  aDoc: TAlnDoc;
		 longint		dsize;
		 VRect		listVRect;
		 TScroller		aScroll;
		 Vpoint		scrollSize ;
{
  inherited::CalcMinFrame(minFrame); //do horiz ?
/**********
	aScroll= NULL;
	aDoc= TAlnDoc(fDocument);
	dsize= 0;
	if ((aDoc->fSeqList != NULL)) 
		dsize= aDoc->fSeqList.fSize + 2;
	/*-- 
		fSeqListView is NULL now when called from NewTemplateWindow 
		at top of TSeqListDoc.DoMakeViews
	---*/
	if ((aDoc->fAlnView != NULL)) {
		aDoc->fAlnView->GetFrame( listVRect);
		dsize= listVRect.top + (aDoc->fAlnView.fRowHeight * dsize);
		aScroll = TAlnScroller(aDoc->fAlnView->GetScroller(TRUE));
		scrollSize.h= 10 + aDoc->fAlnView.fNumOfCols * gAlnCharWidth;
		scrollSize.v= 10 + aDoc->fAlnView.fNumOfRows * aDoc->fAlnView->GetRowHeight(1);
		}	else {
		scrollSize.v= minFrame.bottom;
		scrollSize.h= minFrame.right;
		}
	minFrame.bottom= max(minFrame.bottom, dsize);

/*?! didn't need til we made view a prefwindow!!!!:	
	if ((aScroll!=NULL)) aScroll->SetScrollLimits(scrollSize, kDontRedraw); 
*/
******/
}

 



// TAlnWindow --------------------------

pascal void TAlnWindow::IAlnWindow(void)
{
	IPrefWindow();
	fWantSave= TRUE;
}

pascal void TAlnWindow::SetPrefID(void) /* override */ 
{
	gPrefWindID= kAlnDocID; gPrefWindName= 'TAlnWindow';
}







/* TSeqChangeCmd ------------------------*/		

pascal TSequence TSeqChangeCmd::ChangeToNew( oldSeq:TSequence);
//! Override this for each type of sequence changer 
{ 	ChangeToNew= TSequence(oldSeq->Clone());
}

pascal TSequence TSeqReverseCmd::ChangeToNew( oldSeq:TSequence); // override 
{  	ChangeToNew= oldSeq->Reverse();
}
pascal TSequence TSeqComplementCmd::ChangeToNew( oldSeq:TSequence); // override 
{  	ChangeToNew= oldSeq->Complement();
}
pascal TSequence TSeqRevComplCmd::ChangeToNew( oldSeq:TSequence); // override 
VAR  tempSeq: TSequence;
{  	
		tempSeq= oldSeq->Reverse();  
		ChangeToNew= tempSeq->Complement();
		tempSeq->Free();  
}

pascal TSequence TSeqCompressCmd::ChangeToNew( oldSeq:TSequence); // override 
{  	ChangeToNew= oldSeq->Compress();
}
pascal TSequence TSeqDna2RnaCmd::ChangeToNew( oldSeq:TSequence); // override 
{  	ChangeToNew= oldSeq->Dna2Rna( TRUE);
}
pascal TSequence TSeqRna2DnaCmd::ChangeToNew( oldSeq:TSequence); // override 
{  	ChangeToNew= oldSeq->Dna2Rna( FALSE);
}
pascal TSequence TSeqTranslateCmd::ChangeToNew( oldSeq:TSequence); // override 
{  	ChangeToNew= oldSeq->Translate();
}
pascal TSequence TSeqLockIndelsCmd::ChangeToNew( oldSeq:TSequence); // override 
{  	ChangeToNew= oldSeq->LockIndels(True);
}
pascal TSequence TSeqUnlockIndelsCmd::ChangeToNew( oldSeq:TSequence); // override 
{  	ChangeToNew= oldSeq->LockIndels(False);
}

		
pascal void TSeqChangeCmd::ISeqChangeCmd( TAlnDoc itsAlnDoc , 
										TView		itsView ; TSeqList itsSeqs )
VAR start, nbases: longint;
	TCommandHandler		theContext; 

		pascal void CopyToNew( TSequence oldSeq)
		{
			fNewSeqs->InsertLast(this->ChangeToNew(oldSeq));
		}

{
	//- gApplication->CommitLastCommand(); -- Paste seqs is giving probs...

	fAlnView= itsAlnDoc->fAlnView;
	fAlnView->DeinstallEditSeq(); //! prevent mangle do to fBases newhandle 
	//- if (fAlnView->Focus()) ; --- what do/did we need this for ??
	fOldSeqs= NULL;
	fNewSeqs= NULL;
	// theContext:	TCommandHandler;
	theContext= itsView->GetContext(cSeqChange);
	ICommand(cSeqChange, theContext, kCanUndo, kCausesChange, theContext);

	if ((itsSeqs == NULL)) itsAlnDoc->GetSelection( TRUE, TRUE, itsSeqs, start, nbases);
	FailNIL( itsSeqs);
	fOldSeqs= itsSeqs;

	fNewSeqs= TSeqList(NewList); 
	FailNIL(fNewSeqs);
	fOldSeqs->Each( CopyToNew);
}	


pascal void TSeqChangeCmd::Free(void) // override 
{
	if ((fNewSeqs!=NULL)) {
		fNewSeqs->DeleteAll(); //forget objects -- doesn't affect objects 
		fNewSeqs->Free(); 			//drop list, not objects
		}
	if ((fOldSeqs!=NULL)) fOldSeqs->FreeList(); 	//drop all objects && list
	inherited::Free();
}

pascal void TSeqChangeCmd::UndoIt(void) // override 
VAR	tempList: TSeqList;
{
	tempList= fNewSeqs;
	fNewSeqs= fOldSeqs;
	fOldSeqs= tempList;
	Doit();
	fCommandDone= False; 
}

pascal void TSeqChangeCmd::RedoIt(void) // override 
{
	UndoIt();
	fCommandDone= True; 
}

pascal void TSeqChangeCmd::DoIt(void) // override 
VAR
	aFromRow, aToRow, indx, saveEdit,
	integer		i, j, nseqs, saveStart, saveEnd;
	GridCell		aCell;
	TSequence		oldSeq, newSeq;
	
	pascal void UpdateEdWinds( TSequence oldSeq, newSeq)
		pascal void checkOpenEdWinds( TWindow aWind)
		{
			if (Member(TObject(aWind), TSeqedWindow)) 
				if (TSeqedWindow(aWind).fSequence == oldSeq) { 
					TSeqedWindow(aWind)->ReplaceSeq(newSeq);
					aWind->ForceRedraw();
					}
		}
	{
		TDocument(fContext)->ForAllWindowsDo(checkOpenEdWinds);
	}

	pascal void relocate( TSequence aSeq)
	var 	
			vRect		vLoc; vPoint vSize,
	{
		indx= indx+1;
		if (indx >= aFromRow) && (indx <= aToRow) then {
			//! also need to shift any selection or deselect all...
		  fAlnView->RowToVRect( indx, 1, vLoc);
			fAlnView->InvalidateVRect( vLoc);
			if indx == saveEdit then {
				fAlnView->fEditRow= saveEdit;
				fAlnView->fEditSeq->Locate( vLoc.topLeft, TRUE);
				vSize= fAlnView->fEditSeq.fSize;
				fAlnView->fEditSeq->Resize( vSize, FALSE);  
				}
			aCell.v= indx;
			fAlnView->InvalidateCell(aCell);
			}
	}
	
{
	if (fOldSeqs!=NULL) && (fNewSeqs!=NULL) then begin
		saveEdit= fAlnView->fEditRow;		
		aFromRow= 0;
		aToRow= 0;
		nseqs= fNewSeqs->GetSize();
		for i= 1 to nseqs do {
			newSeq= TSequence( fNewSeqs->At(i));
			oldSeq= TSequence( fOldSeqs->At(i));
			j= fAlnView->fAlnList->GetEqualItemNo( oldSeq);
			if (j>0) then {
				fAlnView->fAlnList->InsertBefore( j, newSeq);
				fAlnView->fAlnList->Delete( oldSeq);
				if (aFromRow == 0) then aFromRow= j
				else aToRow= j;
				}	
			UpdateEdWinds( oldSeq, newSeq);
			}
		
		if ((aToRow=0)) aToRow= aFromRow;
		indx= 0;
		aCell.h= 1;
		fAlnView->fAlnList->Each(relocate);	
		//- fAlnView->SetEmptySelection(kHighlight);*/ /*?
		fCommandDone= True; 
		}
}

pascal void TSeqChangeCmd::Commit(void) // override 
{
	this->DoNotification();
}






/* TAlnDoc ------------------------*/		
		
CONST
	mColorButHit == 1001;
	mMonoButHit	== 1002;
	mLockButHit == kLockIconID;	

#pragma segment AClose

pascal void TAlnDoc::Close(void) /* override */ 
{

	/*! if !gConfiguration.hasColorQD then app bombs when this doc is closed 
			-- DUE TO RemoveSubview(fColorButton) !!!!!! */
			
		/* patches for prefwindow */		
	if ((fUseColor) && (fColorButton!=NULL)) fColorButton->fHilite= TRUE;
	//- if (fAlnWindow!=NULL) fAlnWindow->SetTitle('<<!=>> Align');  
  inherited::Close();
}


#pragma segment ARes
		
pascal void TAlnDoc::IAlnDoc( TFile itsFile)
{	
	fAlnView= NULL;
	fAlnIndex= NULL;
	fAlnHIndex= NULL;
	fUseColor= FALSE;
	fColorButton= NULL;
	fMonoButton= NULL;
	fAlnWindow= NULL;
	fSaveSelection= False;
	
	ISeqListDoc(itsFile);
}



pascal void TAlnDoc::DoMakeViews(Boolean forPrinting) // override 
VAR
		s			: str255;
		aView	: TAlnWindow;
		TAlnDlog		aDialogView;
		TStdPrintHandler		aHandler;
		TAlnScroller		aAlnScroller;
		TSideScroller		aSideScroller;
		vsize		: VPoint;
		hIcon		: Handle;
{
/*------- TSeqListDoc.MakeViews  -----------*/	
	fHeadline= NULL;
	fSeqListView= NULL;
	fFormatPop= NULL;
	
	fAlnWindow = TAlnWindow( gViewServer->NewTemplateWindow(kAlnDocID, this));
	FailNIL(fAlnWindow); 				 
	fAlnWindow->IAlnWindow();
	aView= fAlnWindow;
	
  fFormatPop= TPopup(aView->FindSubView('pFMT'));
	if ((fFormatPop!=NULL) && (fInFormat > kUnknown)) 
		fFormatPop->SetCurrentItem(fInFormat, kDontRedraw);
	
	//get superview of window, minus scrolls, for printing
	aDialogView = TAlnDlog(aView->FindSubView('DLOG')); 
	
//!! Substitute TAlnPrintView for this handler.... !!!
	New(aHandler);
	FailNIL(aHandler);
	aHandler->IStdPrintHandler(this, 					// its document 
							  aDialogView,					// its view 
							  !kSquareDots,			// does not have square dots 
							  kFixedSize, 				// horzontal page size is fixed 
							  !kFixedSize);			/* vertical page size is variable (could be
														  set to TRUE on non-style TE systems) */
 
/*------- TAlnWindow inits -----------*/	
	
	fAlnView 	= TAlnView(aView->FindSubView(kAlnViewType));
	FailNIL( fAlnView);
	fAlnView->IAlnView( this, fSeqList);  

  fAlnIndex = TAlnIndex(aView->FindSubView('VIND'));
	FailNIL(fAlnIndex);
	fAlnIndex->fAlnList= fAlnView->fAlnList;
	fAlnIndex->fAlnView= fAlnView;
	
  fAlnHIndex = TAlnHIndex(aView->FindSubView('HIND'));
	FailNIL(fAlnHIndex);
	fAlnHIndex->fAlnView= fAlnView;
	
	//! need to have num of seqs here...
	//- fAlnIndex->DelRowLast(fAlnIndex->fNumOfRows);
	//- fAlnIndex->InsRowLast(fAlnView->fNumOfRows,fAlnView->GetRowHeight(1));

	// set up the aln view scroller to scroll the rows and columns too 
	aAlnScroller = TAlnScroller(fAlnView->GetScroller(TRUE));
	aSideScroller= TSideScroller(fAlnHIndex->GetScroller(TRUE));
	aAlnScroller->fColumnScroller= aSideScroller;
	aSideScroller->fMainScroller= aAlnScroller;

	//fix for pref saved view w/o active scrolls
	/*----
	if ((aAlnScroller!=NULL)) aAlnScroller->fScrollLimit.v= 256;  
	if ((aAlnScroller!=NULL)) aAlnScroller->fScrollLimit.h= 256;  
	----*/
	
/*+ ?? will this help/hurt alnview overflow bug ??*/ 
/*+
	vsize.h= MaxInt; vsize.v= MaxInt;
	if ((aAlnScroller!=NULL)) aAlnScroller->SetScrollLimits(vsize, kDontRedraw);
+*/

	aSideScroller= TSideScroller(fAlnIndex->GetScroller(TRUE));
	aAlnScroller->fRowScroller = aSideScroller;
	aSideScroller->fMainScroller= aAlnScroller;
	
	fLockButton= TIcon(aView->FindSubView('LOCK'));
	fLockButton->fEventNumber= mLockButHit;
	
		//fix for prefwindow
	fAlnView->fLocked= TRUE; // (fLockButton->fRsrcID == kLockIconID);  
	if (gConfiguration.hasColorQD) 
		hIcon= Handle(GetCIcon( kLockIconID))
	else 
		hIcon= Handle(GetIcon( kLockIconID));
	fLockButton->SetIcon( hIcon, kDontRedraw);

	fColorButton= TIcon(aView->FindSubView('iCLR'));
	fColorButton->fEventNumber= mColorButHit;
	fMonoButton= TIcon(aView->FindSubView('iMNO'));
	fMonoButton->fEventNumber= mMonoButHit;
	fUseColor	= fColorButton->fHilite; //? user pref?
	fColorButton->fHilite= FALSE;
	fMonoButton->fHilite= FALSE;
	if (!gConfiguration.hasColorQD) {
			/*===	!!! this .RevmoveSubView is causing BOMBS on Close of view 
			aView->RemoveSubView(fColorButton);
			aView->RemoveSubView(fMonoButton);
			===*/
		fColorButton->ViewEnable(FALSE, kDontRedraw);
		fColorButton->Show(FALSE, kDontRedraw);
		fMonoButton->ViewEnable(FALSE, kDontRedraw);
		fMonoButton->Show(FALSE, kDontRedraw);
		fUseColor	= FALSE;
		}

	ShowReverted();
}




pascal void TAlnDoc::ShowReverted(void) // override 
VAR  nc : integer;
		 integer		nold, nnew;
{
	inherited::ShowReverted(); // ->> TSeqListDoc.ShowReverted !
	
	SetPortTextStyle(gAlnTextStyle);  
	gAlnCharWidth= CharWidth('G');  // must do before widths; get it here when we have a window...
	gMaxViewCellsBugFix= kMaxCoord / (gAlnCharWidth+4);
	gMaxViewCellsBugSeen= false;
	
	fAlnView->fAlnList= fSeqList;  
	nold= fAlnView->fNumOfRows;
	nnew= fSeqList->GetSize();
	fAlnView->DelRowLast( nold);
	fAlnView->InsRowLast( nnew, fAlnView->fRowHeight);
	fAlnView->UpdateAllWidths();
	
	fAlnIndex->DelRowLast(fAlnIndex->fNumOfRows);
	fAlnIndex->InsRowLast(fAlnView->fNumOfRows,fAlnView->GetRowHeight(1));
	
	/*--
	fAlnHIndex->DelColLast( fAlnHIndex->fNumOfRows);
	fAlnHIndex->InsColLast( fAlnView->fNumOfCols, gAlnCharWidth);
				^^^ done thru UpdateAllWidths...
	---*/
		
	if (gMaxViewCellsBugSeen) then begin
		ParamText('a known bug prevents display of more than ~2700 bases',
				' All bases are there, and will be displayed in the edit window.',
				'DisplayAlign','');	
		gApplication->ShowError( -1, msgMyError); 
		end();
		
}




 

pascal void PascalClose( text VAR aFile)
{
	close( aFile);
}


pascal void TAlnDoc::FirstSelection( TSequence VAR aSeq, longint VAR start, nbases)
VAR		
		atRow, right	: integer;
		GridCell		c1, c2, aCell ;
{
	c1= fAlnView->FirstSelectedCell();
	c2= fAlnView->LastSelectedCell();
	if (!EqualPt( c1, c2)) {
		right= c2.h;
		atRow= c1.v;
		aCell.h= c1.h;
		aCell.v= atRow;
		while (aCell.h<right) && !fAlnView->isCellSelected(aCell))aCell.h= aCell.h+1;
		start= aCell.h - 1;
		while (aCell.h<right) && fAlnView->isCellSelected(aCell))aCell.h= aCell.h+1;
		nbases= aCell.h - start;
		}	else {
		atRow= fAlnIndex->FirstSelectedItem();
		start= 0; 
		nbases= 0; //key for all of seq
		}
	if ((atRow < 1)) atRow= 1; //fix No selection to 1st 
	aSeq= fAlnView->SeqAt( atRow);
	if ((aSeq!=NULL)) {
		aSeq->fIndex= atRow;
		aSeq->SetSelection( start, nbases);
		}
}


pascal void TAlnDoc::GetSelection( boolean equalCount, allAtNoSelection,
											TSeqList		VAR aSeqList; longint VAR start, nbases)
VAR
	integer		left, right, top, bottom;
	aCell	: GridCell;
	aRow	: longint;
	aSeq	: TSequence;
	longint		start1, nbases1;
	Boolean		wantAll;
	
	pascal void AddSeqAtToList( integer aRow)
	{
		aSeq= fAlnView->SeqAt( aRow); 
		if ((aSeq!=NULL)) {
			/*--- ?? default is use all when nbases==0 !?
			if (wantAll) {
				start1= 0;
				nbases1= GethandleSize(aSeq->fBases);
				}
			---*/
			aSeq->fIndex= aRow;
			aSeq->SetSelection( start1, nbases1);
			aSeqList->InsertLast( aSeq);
			start= max(start,start1);
			if ((nBases=0)) nBases= nbases1
			else nBases= min(nBases,nbases1);
			}
	}
	
{
	aSeqList= TSeqList(NewList);
	FailNIL( aSeqList);
	
	left = fAlnView.FirstSelectedCell.h;
	right= fAlnView.LastSelectedCell.h;
	top  = fAlnView.FirstSelectedCell.v;
	bottom= fAlnView.LastSelectedCell.v;
	start= 0; start1= 0;
	nBases= 0; nBases1= 0;
	if ((left!=right) || (top!=bottom)) {
		wantAll= FALSE;
		FOR aRow= top TO bottom){
			if (equalcount) aCell.h= left else aCell.h= 1; 
			aCell.v= aRow;
			while (aCell.h<right) && !fAlnView->isCellSelected(aCell))aCell.h= aCell.h+1;
			start1= aCell.h - 1;
			if (equalCount)
				while (aCell.h<right) && fAlnView->isCellSelected(aCell))aCell.h= aCell.h+1
			else 
				while fAlnView->isCellSelected(aCell))aCell.h= aCell.h+1;
			nbases1= aCell.h - start1;
			AddSeqAtToList( aRow);
			}
		start = left - 1;
		nbases= right - start;  
		}	else if ((fAlnIndex.LastSelectedItem=0)) {
		wantAll= TRUE;
		if (allAtNoSelection) fAlnIndex->AllItemsDo( AddSeqAtToList);
		}	else {
		wantAll= TRUE;
		fAlnIndex->EachSelectedItemDo( AddSeqAtToList);
		}
}
			

pascal Integer TAlnDoc::SelectionToFile( Boolean AllatNoSelection,
			Str255		aFileName; integer seqFormat)  
			//return # written 
VAR
	longint		start, nbases;
	integer		nseqs;
	TSeqList		aSeqList;
{
	GetSelection( FALSE, allAtNoSelection, aSeqList, start, nbases);
	nseqs= aSeqList->GetSize();
	aSeqList->doWrite( aFileName, seqFormat); 
	aSeqList->Free();
	SelectionToFile= nseqs;
}



pascal void TAlnDoc::DoWrite(TFile aFile, Boolean makingCopy) // override 
VAR   outformat: integer;
	longint		start, nbases;
	integer		nseqs;
	TSeqList		aSeqList;
	Str63		aStr63;
	fi	: failInfo;
	
	pascal void HdlFailure(OSErr error, long message)
	{
		aSeqList= TSeqList(FreeIfObject(aSeqList));
		fSaveSelection= False;
	}
	
{
	aSeqList= NULL;
	aFile->GetName( aStr63); gOutputName= aStr63;
	if (fFormatPop == NULL) outformat= kGenBank
	else outformat= fFormatPop->GetCurrentItem(); 
	if ((fSaveSelection)) {
		fSaveSelection= False;
		CatchFailures( fi, HdlFailure);
		GetSelection( FALSE, True, aSeqList, start, nbases);
		aSeqList->doWriteRef( aFile->fDataRefNum, outformat);
		aSeqList= TSeqList(FreeIfObject( aSeqList));
		Success(fi);
		}	else {
		fSeqList->ClearSelections(); //! make sure we write all of seq from this call !?
		fSeqList->doWriteRef( aFile->fDataRefNum, outformat);
		}
}



pascal void TAlnDoc::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
VAR
	procPtr		drawHook;
	hIcon	: Handle;
{
	switch (eventNumber) {
		case mLockButHit:
		  if (fAlnView->fLocked) { 
				fAlnView->fLocked= FALSE;
				if (gConfiguration.hasColorQD) 
					hIcon= Handle(GetCIcon( kUnLockIconID))
				else 
					hIcon= Handle(GetIcon( kUnLockIconID));
				fLockButton->SetIcon( hIcon, kRedraw);
				}			else {
				fAlnView->fLocked= TRUE;
				fAlnView->DeInstallEditSeq();
				if (gConfiguration.hasColorQD) 
					hIcon= Handle(GetCIcon( kLockIconID))
				else 
					hIcon= Handle(GetIcon( kLockIconID));
				fLockButton->SetIcon( hIcon, kRedraw);	
				}
			
		mColorButHit: {
		 	// fColorButton->HiliteState( TRUE, not fColorButton->fHilite);
		  if (!fUseColor) { 
				fUseColor= TRUE;
				/*---
				drawHook= &alnTEDrawHook;
				TECustomHook(intDrawHook, drawHook, fAlnView->fEditSeq.fHTE);
				---*/
				fAlnView->ForceRedraw();
				}
			}
			
		mMonoButHit: {
		 	// fMonoButton->HiliteState( TRUE, not fMonoButton->fHilite);
		  if (fUseColor) { 
				fUseColor= FALSE;
				/*---
				drawHook= gDefTEDrawHook;
				TECustomHook(intDrawHook, drawHook, fAlnView->fEditSeq.fHTE);
				---*/
				fAlnView->ForceRedraw();
				}
			}
					
		otherwise
				inherited::DoEvent( eventNumber, source, event);
		}
}
		


CONST
		kMaxOpenSeqed == 10;


pascal void TAlnDoc::AddSeqToList(TSequence item)  // override 
VAR last: integer;
{
 /*----
 	inherited::AddSeqToList( item);  
	fAlnView->registerInsertLast( item);  
 ----*/
	fAlnView->addToAlnList( item);
	fAlnIndex->InsItemLast( 1);
	fAlnIndex->SelectItem( fAlnIndex->fNumOfRows, FALSE, TRUE, TRUE);
	//- fChangeCount= fChangeCount+1;
	this->Changed( 1, this);
}	


  
#pragma segment AOpen

pascal void TAlnDoc::OpenSeqedWindow(TSequence aSeq)
VAR
		TStdPrintHandler		aHandler;
		aSeqed	: TSeqedWindow;
		boolean		foundDoc;
		sname		: SeqName;
		
		pascal void checkOpenEdWinds( TWindow aWind)
		{
			if (Member(TObject(aWind), TSeqedWindow)) 
				if (TSeqedWindow(aWind).fSequence == aSeq) {
					aWind->Select(); foundDoc= TRUE;
					}
		}
{
	foundDoc= FALSE;
	ForAllWindowsDo(checkOpenEdWinds);
	if (!foundDoc then begin
		aSeqed = TSeqedWindow(gViewServer->NewTemplateWindow(kSeqEdWindID, this));
		FailNIL(aSeqed); 		 
		aSeqed->ISeqedWindow(aSeq);
		sname= aSeq->fName;
		//- aSeqed->SetTitleForDoc( sname);
		aSeqed->SetTitle( concat(sname, ' Edit'));
		
		New(aHandler);  
		FailNIL(aHandler);
		aHandler->IStdPrintHandler(this, 					// its document 
									aSeqed->fSeqedView,					// its view 
									!kSquareDots,			// does not have square dots 
									kFixedSize, 				// horzontal page size is fixed 
									!kFixedSize);			/* vertical page size is variable (could be
																set to TRUE on non-style TE systems) */
	
		aSeqed->ShowReverted(); 
		aSeqed->Open(); 
		aSeqed->Select();	 
		}
}

 

pascal void TAlnDoc::OpenSeqs( boolean editOnly) 
VAR
	aCell	: GridCell;
	count	: integer;
	
	pascal void newEditWindow( integer anItem)
	TSequence		VAR aSeq;
	{
		aSeq= TSequence(this->fSeqList->At(anItem));
		if ((aSeq!=NULL) && (count<kMaxOpenSeqed)) {
			this->OpenSeqedWindow(aSeq);
			count= count+1;
			}
  }
	
{
	if ((fAlnIndex=NULL) || (fAlnIndex.firstSelectedItem == 0)) 
			//nada
			
	else if (editOnly 
		|| (fAlnIndex.FirstSelectedItem == fAlnIndex->LastSelectedItem())) {
		count= 0;
		fAlnIndex->EachSelectedItemDo( newEditWindow);
		}
		
}





#pragma segment ARes //??

pascal void  TAlnDoc::DoMenuCommand(CommandNumber aCommandNumber) // override 
VAR
		aSeq		: TSequence;
		integer		menu, item, nSel;
		aName 	: Str255;
		TAlnEditCommand		aAlnEditCommand ;
		TAlnPasteCommand		aAlnPasteCommand;

		longint		firstBase, nBases;
		TSeqList		aSeqList;
		TSeqTranslateCmd		aSeqTranslateCmd;
		TSeqReverseCmd		aSeqReverseCmd;
		TSeqRevComplCmd		aSeqRevComplCmd;
		TSeqComplementCmd		aSeqComplementCmd;
		TSeqCompressCmd		aSeqCompressCmd;
		TSeqDna2RnaCmd		aSeqDna2RnaCmd;
		TSeqRna2DnaCmd		aSeqRna2DnaCmd;
		TSeqLockIndelsCmd		aSeqLockIndelsCmd;
		TSeqUnlockIndelsCmd		aSeqUnlockIndelsCmd;
		
		
	pascal void doMakeConsensus(void)
	VAR 
			cons	: TSequence;
			TSeqList		aSeqList;
			longint		firstBase, nBases;
			arow, totalRows	: integer;
			VRect		vbounds;
	{		
		cons= fSeqList->Consensus();
		if ((cons!=NULL)) {
			aRow= fSeqList->GetIdentityItemNo( cons);
			totalRows= fSeqList->GetSize();
			fSeqList->Delete(cons);
			cons->Free();
			this->fAlnView->RowToVRect( aRow, totalRows-aRow+1, vbounds); 
			this->fAlnView->InvalidateVRect( vbounds);
			this->fAlnIndex->RowToVRect( aRow, totalRows-aRow+1, vbounds); 
			this->fAlnIndex->InvalidateVRect( vbounds);
			this->fAlnView->UpdateSize();   
			this->fAlnIndex->UpdateSize();
			}
			
		GetSelection( TRUE, TRUE, aSeqList, firstBase, nBases);
		aSeqList->MakeConsensus();
		cons= aSeqList->Consensus();
		aSeqList->Free();
		if ((cons!=NULL)) {
			AddSeqToList( cons);
			fAlnIndex->SelectItem( fSeqList.consensusRow, FALSE, TRUE, TRUE);
			fAlnIndex->ScrollSelectionIntoView(TRUE);
			}
	}

	
	pascal void MakeSeqPrint(Boolean doREMap)
	VAR
			TREMapDocument		aREMapDoc;
			aSeqPrDoc	: TSeqPrintDocument;
			longint		firstBase, nBases;
			aSeq		: TSequence;
	{
		FirstSelection( aSeq, firstBase, nBases);
		
		if (doREMap) {
			New(aREMapDoc);
			FailNIL(aREMapDoc);
			aREMapDoc->IREMapDocument(kPrintClipType, this, aSeq, firstBase, nbases);
			gSeqAppApplication->OpenNewDocument(aREMapDoc);  
			}		else {
			New(aSeqPrDoc);
			FailNIL(aSeqPrDoc);
			aSeqPrDoc->ISeqPrintDocument(kPrintClipType, this, aSeq, firstBase, nbases);
			gSeqAppApplication->OpenNewDocument(aSeqPrDoc);  
			}
	}

		
	pascal void MakeSeqSearch(integer command)
	VAR
			longint		firstBase, nBases, nList;
			aSeq		: TSequence;
			TSeqList		aSeqList;
	{		
		switch (command) {
			cGBBlast, 
			cGBFasta,
			cGeneidSearch,
			cBlocksSearch, 
			cGenmarkSearch: {
				FirstSelection( aSeq, firstBase, nBases);
				MailWithSeq( command, aSeq); 
				}
				
			cPythiaSearch,
			cGrailSearch: {
				GetSelection( FALSE, TRUE, aSeqList, firstBase, nBases);
				MailWithSeqList( command, aSeqList);  
				aSeqList->Free();
				}
			}
	}

	pascal void MakeAlnPrint(void)
	VAR  
			longint		start, nBases;
			aAlnPrDoc	: TAlnPrintDocument;
			aSeqList	: TSeqList;
	{
		GetSelection( TRUE, TRUE, aSeqList, start, nBases);
		if ((aSeqList.GetSize>1)) {
			New(aAlnPrDoc);
			FailNIL(aAlnPrDoc);
			aAlnPrDoc->IAlnPrintDocument(kPrintClipType, this, aSeqList, start, nBases);
			gSeqAppApplication->OpenNewDocument(aAlnPrDoc); //finish up
			//?? aSeqList->Free(); << NO, aAlnPrDoc owns this list !
			}		else {
		  aSeqList->Free(); //drop only list
			MakeSeqPrint( FALSE);
			}
	}
	
	pascal void MakeDottyPlot(void)
	VAR  
			longint		start, nBases;
			aSeqList	: TSeqList;
			aDottyPlotDoc	: TDottyPlotDocument;
	{
		GetSelection( FALSE, TRUE, aSeqList, start, nBases);
		if ((aSeqList.GetSize>0)) {
			New(aDottyPlotDoc);
			FailNIL(aDottyPlotDoc);
			aDottyPlotDoc->IDottyPlotDocument(kPrintClipType, this, aSeqList, start, nBases);
			gSeqAppApplication->OpenNewDocument(aDottyPlotDoc); //finish up
			//! Don't FREE aSeqList -- DottyPlotDoc took over ownership 
			}
	}
	
	pascal void CallChildApp(theMenuName:Str255);
	VAR
		TSeqList		aSeqList;
		longint		start, nbases;
		
		pascal Boolean isTheCommand( TChildCommand aChildCommand)
		{
			if ((aChildCommand->fMenuName == theMenuName)) {
				aChildCommand->LaunchDialog( aSeqList);
				isTheCommand= TRUE;
				}			else
				isTheCommand= FALSE;
		}
		
	{
		GetSelection( TRUE, TRUE, aSeqList, start, nBases);
		if (gChildCommandList->FirstThat( isTheCommand) != NULL) ;
		aSeqList->Free();
	}

	pascal void	SelectSequence( integer anItem)
	{
		fAlnIndex->SelectItem( anItem, kExtend, kHighlight, kSelect);
	}
				
	pascal void	DoSeqChangeCommand( TSeqChangeCmd aCommand)
	VAR		aSeqList: TSeqList; 
				Longint		firstBase, nBases;
	{
		FailNIL(aCommand);
		GetSelection( TRUE, TRUE, aSeqList, firstBase, nBases);
		aCommand->ISeqChangeCmd( this, fAlnView, aSeqList); 
		PostCommand( aCommand);
	}
	
	
{
	CommandToMenuItem(aCommandNumber, menu, item);
	if (menu == mChildren) {
		switch (aCommandNumber) {
			cAddChildCmd,
			case cRemoveChildCmd:  
				inherited::DoMenuCommand(aCommandNumber);
			Otherwise {
						GetItem(GetMHandle(menu), item, aName);
						CallChildApp( aName);
						}
			}
		}		
	else CASE aCommandNumber OF

		cSaveSelection: {
			fSaveSelection= True;
			inherited::DoMenuCommand(cSaveCopy);
			}
			
		cSelectAll:  fAlnIndex->AllItemsDo( SelectSequence);

		cCut, cCopy, cClear: {
				NEW(aAlnEditCommand);
				FailNIL(aAlnEditCommand);
				aAlnEditCommand->IAlnEditCommand(this, aCommandNumber);
				PostCommand( aAlnEditCommand);
				}

		case cPaste:
				if (gClipboardMgr->fClipView->ContainsClipType(kAlnScrapType)) {
					NEW(aAlnPasteCommand);
					FailNIL(aAlnPasteCommand);
					aAlnPasteCommand->IAlnPasteCommand(this);
					PostCommand(  aAlnPasteCommand);
					}				/* else if (gClipView->ContainsClipType('TEXT')) {
					// convert from TEXT to sequence data...
					}				*/
				else  
					inherited::DoMenuCommand(aCommandNumber);
	
		
		cNewSeq		: {
				AddNewSeqToList();
				OpenSeqs(kEditOnly); 
				}
						
		cEditSeq		: OpenSeqs(kEditOnly);

		cConsensus	: doMakeConsensus;

		cTranslateSeq: {
				New(aSeqTranslateCmd);
				DoSeqChangeCommand( aSeqTranslateCmd);
				}
		cReverseSeq: {
				New(aSeqReverseCmd);
				DoSeqChangeCommand( aSeqReverseCmd);
				}
		cComplementSeq: {
				New(aSeqComplementCmd);
				DoSeqChangeCommand( aSeqComplementCmd);
				}
		cRevComplement: {
				New(aSeqRevComplCmd);
				DoSeqChangeCommand( aSeqRevComplCmd);
				}
		cCompress: {
				New(aSeqCompressCmd);
				DoSeqChangeCommand( aSeqCompressCmd);
				}
		cLockIndels: {
				New(aSeqLockIndelsCmd);
				DoSeqChangeCommand( aSeqLockIndelsCmd);
				}
		cUnlockIndels: {
				New(aSeqUnlockIndelsCmd);
				DoSeqChangeCommand( aSeqUnlockIndelsCmd);
				}
		cDna2Rna: {
				New(aSeqDna2RnaCmd);
				DoSeqChangeCommand( aSeqDna2RnaCmd);
				}
		cRna2Dna: {
				New(aSeqRna2DnaCmd);
				DoSeqChangeCommand( aSeqRna2DnaCmd);
				}
		
		MakeAlnPrint		cPrettyPrint;
		cRestrictMap: MakeSeqPrint( TRUE); 
		cDottyPlot	: MakeDottyPlot;
	  
		cGeneidSearch,
		cBlocksSearch,
		cGenmarkSearch,
		cGrailSearch,
		cPythiaSearch,
		cGBBlast,
		cGBSearch	:	MakeSeqSearch(aCommandNumber);
		
		default:
			inherited::DoMenuCommand(aCommandNumber);
		}
}


#pragma segment ARes  


pascal void TAlnDoc::DoSetupMenus(void) // override 
VAR 
	nsel	: integer;
	MenuHandle		childMenuHandle;
	indexSelection, haveSel	: Boolean;
	
	pascal void EnableChildMenus( TChildCommand aChildCommand)
	{
		aChildCommand->EnableMenu( childMenuHandle, nsel) ;
	}
	
{
  inherited::DoSetupMenus();	
	
	nsel= 0;
	if ((fAlnView!=NULL) && (fAlnView.FirstSelectedCell.v > 0)) 
		nsel= 1 + fAlnView.LastSelectedCell.v - fAlnView.FirstSelectedCell.v;
	indexSelection= ((fAlnIndex!=NULL) && (fAlnIndex.FirstSelectedItem>0));
	if (indexSelection && (nSel=0)) 
		nsel= 1 + fAlnIndex.LastSelectedItem - fAlnIndex->FirstSelectedItem();
	haveSel= nsel>=1;
	
	/*----
	if (indexSelection) {
		SetEditCmdName(cCut, cCutSeq);
		SetEditCmdName(cCopy, cCopySeq);
		SetEditCmdName(cClear, cClearSeq);
		}
	else if ((fAlnView->fEditRow!=0)) {
		SetEditCmdName(cCut, cCutText);
		SetEditCmdName(cCopy, cCopyText);
		SetEditCmdName(cClear, cClearText);
		}
	----*/
	//if (indexSelection && clipHasSeqText...) CanPaste('TEXT'); 
	//if (indexSelection) */ /*!? Paste at end if no selection 
	gClipboardMgr->CanPaste(kAlnScrapType);
	
	Enable(cSaveSelection, haveSel); 

	Enable(cCut, indexSelection);
	Enable(cCopy, indexSelection);
	Enable(cClear, indexSelection);
	Enable(cSelectAll, TRUE);

	Enable( cNewSeq, TRUE);
	Enable( cEditSeq, haveSel); 
	Enable( cPrettyPrint, haveSel); 
	Enable( cDottyPlot, haveSel); 
	Enable( cConsensus, nsel>=2); 
	
	Enable( cGBSearch, gGoodMac && gTCPIsInstalled && (haveSel)); 
	Enable( cGBBlast, gGoodMac && gTCPIsInstalled && (haveSel)); 
	Enable( cGeneidSearch, gGoodMac && gTCPIsInstalled && (haveSel)); 
	Enable( cBlocksSearch, gGoodMac && gTCPIsInstalled && (haveSel)); 
	Enable( cGenmarkSearch, gGoodMac && gTCPIsInstalled && (haveSel)); 
	Enable( cGrailSearch, gGoodMac && gTCPIsInstalled && (haveSel)); 
	Enable( cPythiaSearch, gGoodMac && gTCPIsInstalled && (haveSel)); 

	Enable( cReverseSeq, haveSel); 
	Enable( cComplementSeq, haveSel); 
	Enable( cRevComplement, haveSel); 
	Enable( cCompress, haveSel); 
	Enable( cLockIndels, haveSel); 
	Enable( cUnlockIndels, haveSel); 
	Enable( cDna2Rna, haveSel/*&fKind=Dna or Nuc*/); 
	Enable( cRna2Dna, haveSel/*&fKind=Rna or Nuc*/); 
	Enable( cTranslateSeq, haveSel); 
	
	Enable( cRestrictMap, haveSel); 
	
	if ((gChildCommandList!=NULL)) {
		childMenuHandle = GetMHandle(mChildren);
		gChildCommandList->Each( enableChildMenus);
		}
}







//	TAlnEditCommand -----------------------	
#pragma segment TAlnEditCommand

pascal void TAlnEditCommand::IAlnEditCommand(TAlnDoc itsDocument, short itsCommand)
	FailInfo		VAR fi;
	TCommandHandler		theContext; 

	pascal void HdlAllocationFailure(OSErr error, long message)
	{
		Free();
	}

{
	fSelection= NULL;
	fOldList= NULL;
	FailNIL( itsDocument);
	// theContext:	TCommandHandler;
	theContext= itsDocument->fAlnView->GetContext(itsCommand);
	ICommand(itsCommand, theContext, kCanUndo, kCausesChange, theContext);

	fAlnDoc = itsDocument;
	CatchFailures(fi, HdlAllocationFailure);
	fSelection = MakeNewRgn;					 
	fOldList= TSeqList(fAlnDoc->fSeqList->Clone());
	FailNIL( fOldList);
	Success(fi);
	CopyRgn(fAlnDoc->fAlnIndex.fSelections, fSelection);
	
	fChangesClipboard = itsCommand != cClear;
	fCausesChange = itsCommand != cCopy;
	fCanUndo = itsCommand != cCopy;  //?
}


pascal void TAlnEditCommand::Free(void) // override 
{
	if ((fSelection!=NULL)) DisposeRgn(fSelection);
	fSelection = NULL;
	if ((fOldList!=NULL)) fOldList->Free();
	fOldList= NULL;
	inherited::Free();
}


pascal void TAlnEditCommand::CopySelection(void)
VAR
		TAlnDoc		clipAlnDoc;
		clipView	: TAlnView;
		
	pascal void CopyRowToClipboard(integer aRow)
	TSequence		VAR  aSeq;
	{
		aSeq = TSequence(fAlnDoc->fSeqList->SeqAt(aRow).Clone);
		clipAlnDoc->fSeqList->InsertLast( aSeq);
	}

{
	New(clipAlnDoc);
	FailNIL(clipAlnDoc);
	clipAlnDoc->IAlnDoc(NULL);   //< IAlnDoc creates AlnDoc->fSeqList && adds 1 blank new seq...
	clipAlnDoc->FreeData();	/* drop blank seq*/	
	clipAlnDoc->DoInitialState();  //?
	
	fAlnDoc->fAlnIndex->EachSelectedItemDo(CopyRowToClipboard); 

	clipView= TAlnView( gViewServer->DoCreateViews(clipAlnDoc, NULL, kAlnViewID, gZeroVPt));
	FailNIL( clipView);

			// from TAlnView.IAlnView....
	clipView->fAlnDoc= clipAlnDoc;
	clipView->fAlnList= clipAlnDoc->fSeqList;
	clipView->fEditSeq= NULL;
	clipView->fEditRow= 0;
	clipView->fRowheight= clipView->GetRowHeight(1); 
	clipAlnDoc->fAlnView= clipView;

			// from TAlnDoc.ShowReverted...
	clipView->DelRowLast( clipView->fNumOfRows);
	clipView->InsRowLast( clipAlnDoc->fSeqList->GetSize(), clipView->fRowHeight);  
	clipView->UpdateAllWidths(); 

	this->ClaimClipboard(clipView);  
	//- clipView->AdjustFrame(); ??
}



pascal void TAlnEditCommand::DeleteSelection(void)
VAR
		integer		totalRows;
		
	pascal void MarkDelete(integer aRow)
	VAR	 aSeq	: TSequence;
			 VRect		vbounds;
	{
		aSeq= fAlnDoc->fSeqList->SeqAt(aRow);
		if ((aSeq!=NULL)) {
			aSeq->fDeleted= TRUE;
			//- fAlnDoc->fSeqList->AtDelete(aRow);*/ /* this should be safe !!! NO !in Mapp3 (or 2)!!!...
			
			fAlnDoc->fAlnView->RowToVRect( aRow, totalRows-aRow+1, vbounds); 
			fAlnDoc->fAlnView->InvalidateVRect( vbounds);
			
			fAlnDoc->fAlnIndex->RowToVRect( aRow, totalRows-aRow+1, vbounds); 
			fAlnDoc->fAlnIndex->InvalidateVRect( vbounds);
			fAlnDoc->fAlnIndex->SelectItem( aRow, kDontExtend, kDontHighlight, !kSelect);
			}
	}

	pascal void ReallyDeleteMarked( TSequence aSeq)
	{
		if (aSeq->fDeleted)) fAlnDoc->fSeqList->Delete( aSeq);
	}

{
	totalRows= fOldList->GetSize();
	fAlnDoc->fAlnIndex->EachSelectedItemDo(MarkDelete);
	fAlnDoc->fSeqList->Each(ReallyDeleteMarked);
	fAlnDoc->fAlnView->UpdateSize();  //must do before Index.update... combine these
	fAlnDoc->fAlnIndex->UpdateSize();
}


pascal void TAlnEditCommand::RestoreSelection(void)
VAR  aRow, totalRows: integer;
	
	pascal void restoreDeletes(TSequence aSeq)
	VRect		VAR  vbounds;
	{
		aRow= aRow+1;
		if ((aSeq->fDeleted)) {  
			aSeq->fDeleted= FALSE;
			fAlnDoc->fSeqList->InsertBefore(aRow, aSeq); 	
			
			fAlnDoc->fAlnView->InsRowBefore( aRow, 1, fAlnDoc->fAlnView.fRowHeight);
			/*- // insrow does this
			fAlnDoc->fAlnView->RowToVRect( aRow, totalRows-aRow+1, vbounds); 
			fAlnDoc->fAlnView->InvalidateVRect( vbounds);
			---*/

			fAlnDoc->fAlnIndex->InsRowBefore( aRow, 1, fAlnDoc->fAlnView.fRowHeight);
			//- fAlnDoc->fAlnIndex->SelectItem( aRow, kExtend, kHighlight, kSelect);
			/*- // insrow does this
			fAlnDoc->fAlnIndex->RowToVRect( aRow, totalRows-aRow+1, vbounds); 
			fAlnDoc->fAlnIndex->InvalidateVRect( vbounds);
			---*/
			}
	}

{
	aRow= 0;
	totalRows= fOldList->GetSize();
	fOldList->Each( restoreDeletes);
	fAlnDoc->fAlnView->UpdateSize();  //must do before Index.update... combine these
	fAlnDoc->fAlnIndex->UpdateSize();  
	ReSelect(); // == fAlnDoc->fAlnIndex->ReSelect(fSelection);
}


pascal void TAlnEditCommand::Commit(void)
	pascal void commitDeletes(TSequence aSeq)
	{
		if ((aSeq!=NULL) && (aSeq->fDeleted)) aSeq->Free();  
	}
{
	fOldList->Each( commitDeletes);
	this->DoNotification();
}


pascal void TAlnEditCommand::ReSelect(void)
{
	fAlnDoc->fAlnIndex->ReSelect(fSelection);
}


pascal void TAlnEditCommand::DoIt(void)
{
	if (fIdentifier != cClear) CopySelection;
	if (fIdentifier != cCopy) DeleteSelection;
	fCommandDone= True; 
}


pascal void TAlnEditCommand::UndoIt(void)
/* If the user has changed the selection since this command was created,
  restore it before Undoing so that the correct cells are affected. */
{
	if (fIdentifier != cCopy) {
		RestoreSelection();
		//- ReSelect;	<< see RestoreSelection....
		fAlnDoc->fAlnView->ScrollSelectionIntoView(TRUE);
		fCommandDone= false; 
		}
}


pascal void TAlnEditCommand::RedoIt(void)
/* If the user has changed the selection since this command was created,
  restore it before Redoing so that the correct cells are affected. */
{
	if (fIdentifier != cCopy) {
		DeleteSelection();
		/*- ReSelect;	 ! !After DELETION....*/	 
		fAlnDoc->fAlnView->ScrollSelectionIntoView(TRUE);
		fCommandDone= true; 
		}
}










// TAlnPasteCommand -----------------  
#pragma segment TAlnPasteCommand


pascal void TAlnPasteCommand::IAlnPasteCommand(TAlnDoc itsDocument)
VAR 	fi:			FailInfo;
		TCommandHandler		theContext; 

	pascal void HdlAllocationFailure(OSErr error, long message)		
	{
		Free();
	}

	pascal void cloneit( TSequence aSeq)
	integer		VAR  aRow;
	{
		aRow= fClipList->GetEqualItemNo(aSeq);
		aSeq= TSequence(aSeq->Clone());
		fClipList->AtPut( aRow, aSeq);
	}

{
	// theContext:	TCommandHandler;
	theContext= itsDocument->fAlnView->GetContext(cPaste);
	ICommand(cPaste, theContext, kCanUndo, kCausesChange, theContext);

	fAlnDoc = itsDocument;
	CatchFailures(fi, HdlAllocationFailure);
	fSelection = MakeNewRgn;							// copy the current selection region 
	Success(fi);
	CopyRgn(fAlnDoc->fAlnIndex.fSelections, fSelection);
	//- fClipDocument= TAlnView(gClipView).fAlnDoc;
		/*^^ used cloned clip list of seqs only, private store so other commands don't
			mysteriously mangle our command before commit...
			--- still mangling before commit -- do away w/ clone cliplist,
			&& clone on doit, free on undo...
		*/
	fClipList= TSeqList(TAlnView(gClipboardMgr->fClipView).fAlnList->Clone());
	fClipList->Each( cloneit);

	fInsRow= fAlnDoc->fAlnIndex->LastSelectedItem();
	if (fInsRow=0) fInsRow= fAlnDoc->fSeqList->GetSize();
	
	fReplacedSeqs= TSeqList(NewList);
	FailNIL(fReplacedSeqs);
}


pascal void TAlnPasteCommand::Free(void) // override 
{
	if (fSelection != NULL) DisposeRgn(fSelection);
	fSelection = NULL;
	TObject(fClipList) = FreeListIfObject(fClipList); //New since cloned clip list
	TObject(fReplacedSeqs) = FreeListIfObject(fReplacedSeqs);
	inherited::Free();
}


pascal void TAlnPasteCommand::DoIt(void)
VAR  aRow, totalRows: integer;
		
	pascal void InsertMarker( TSequence aSeq)
	VRect		VAR   vbounds;
	{
		aRow= aRow+1;
		// mark here, fDeleted, then clone in .Commit method
		aSeq= TSequence(aSeq->Clone()); //!NEW: prevent mangle by other cmds
		aSeq->fDeleted= TRUE;  //< this is mark that we Inserted it (not deleted)

		fAlnDoc->fSeqList->InsertBefore( aRow, aSeq);
		
		fAlnDoc->fAlnView->InsRowBefore( aRow, 1, fAlnDoc->fAlnView.fRowHeight);
		/*-- // insrow does this
		fAlnDoc->fAlnView->RowToVRect( aRow, totalRows-aRow+1, vbounds); 
		fAlnDoc->fAlnView->InvalidateVRect( vbounds);
		---*/

		fAlnDoc->fAlnIndex->InsRowBefore( aRow, 1, fAlnDoc->fAlnView.fRowHeight);
		/*-- // insrow does this
		fAlnDoc->fAlnIndex->RowToVRect( aRow, totalRows-aRow+1, vbounds); 
		fAlnDoc->fAlnIndex->InvalidateVRect( vbounds);
		---*/
	}

{		
	aRow= fInsRow;
	/*---
	totalRows= fAlnDoc->fSeqList.GetSize + fClipDocument->fSeqList->GetSize();
	fClipDocument->fSeqList->Each(InsertMarker);
	---*/
	totalRows= fAlnDoc->fSeqList.GetSize + fClipList->GetSize();
	fClipList->Each(InsertMarker);
	fAlnDoc->fAlnView->UpdateAllWidths();
	fAlnDoc->fAlnView->UpdateSize();  //must do before Index.update... combine these
	fAlnDoc->fAlnIndex->UpdateSize();  
	fCommandDone= True; 
} 


pascal void TAlnPasteCommand::UndoIt(void)
VAR
		integer		totalRows;
		
	pascal void DeleteIfMarked(TSequence aSeq)
	VAR	 vbounds: VRect;
			 integer		aRow;
	{
		if ((aSeq!=NULL) && (aSeq->fDeleted)) {
			aSeq->fDeleted= FALSE;
			aRow= fAlnDoc->fSeqList->GetEqualItemNo( aSeq);
			fAlnDoc->fSeqList->AtDelete(aRow); // is this safe? -- we are in SeqList.Each...
			aSeq->Free(); //!NEW: prevent mangled command 
			
			fAlnDoc->fAlnView->RowToVRect( aRow, totalRows-aRow+1, vbounds); 
			fAlnDoc->fAlnView->InvalidateVRect( vbounds);
			
			fAlnDoc->fAlnIndex->RowToVRect( aRow, totalRows-aRow+1, vbounds); 
			fAlnDoc->fAlnIndex->InvalidateVRect( vbounds);
			fAlnDoc->fAlnIndex->SelectItem( aRow, kDontExtend, kDontHighlight, !kSelect);
			}
	}

{
	totalRows= fAlnDoc->fSeqList->GetSize();
	fAlnDoc->fSeqList->Each( DeleteIfMarked);
	fAlnDoc->fAlnView->UpdateAllWidths();
	fAlnDoc->fAlnView->UpdateSize();  //must do before Index.update... combine these
	fAlnDoc->fAlnIndex->UpdateSize();
	fCommandDone= false; 
}


pascal void TAlnPasteCommand::RedoIt(void)
{
	DoIt();
	fCommandDone= True; 
}

pascal void TAlnPasteCommand::Commit(void) // override 

	pascal void commitInserts(TSequence aSeq)
	integer		VAR  aRow;
	{
		if ((aSeq->fDeleted)) {
			aSeq->fDeleted= FALSE;
			/*---   !NEW: no more: too much hassle
			aRow= fAlnDoc->fSeqList->GetEqualItemNo( aSeq);
			aSeq= TSequence(aSeq->Clone());
			fAlnDoc->fSeqList->AtPut( aRow, aSeq);
			---*/
			}
	}
{
	fAlnDoc->fSeqList->Each( commitInserts);
	this->DoNotification();
}

pascal void TAlnPasteCommand::UpdateViews(void)
{

}

