// USeqDoc.inc.p 

	
#pragma segment USeqDoc

CONST
	kCodonTableFile == ':Tables:Codon.Table';
	kRenzymeFile == ':Tables:REnzyme.Table';
	kInterleaveTemp == 'seqapp-interleave.temp';
	msgMyError == 0x80001234;


	
pascal void InitUSequence(void)
{
	gCodonTable[1].amino='?';
	gCodonTable[1].codon='xxx';
	gCodonTable[1].numPerK= 0;

	indelHard= '-';
	indelSoft= '~'; //? or '.'
	indelEdge= '.';

	/*!!! NEED Internal Codon Table in case default is missing 
		?? use resource -- stuff one of these into Code/Prefs file ?
		*/
	ReadCodonTable(kCodonTableFile); //read default tables...
	
	NEW(gREMap);
	FailNIL(gREMap);
	gREMap->IREMap();
	gREMap->ReadREnzymeTable(kRenzymeFile); 
}










// TSeqList  -------------------------------------

pascal Boolean TSeqList::ListIsEmpty(void)
VAR  aSeq: TSequence;
{
	if ((this.GetSize > 1))
		ListIsEmpty= FALSE
	else if ((this.GetSize == 1)) {
		aSeq= TSequence(this->First());
		if ((aSeq!=NULL) && (aSeq->fLength > 0))
			ListIsEmpty= FALSE
		else
			ListIsEmpty= TRUE;
		}	else
	  ListIsEmpty= TRUE;
}

pascal TSequence TSeqList::SeqAt( integer aRow)  
{
	SeqAt= TSequence(this->At( aRow));  
}

 
TYPE 
	longarray 	== array [0..1] of longint;
	pLongarray	== ^longarray;
	hLongarray	 == ^pLongarray;		

VAR
	longint		gNoutindex, gOutindexmax;
	hLongarray		gOutindex;

pascal void IndexSetup(void)
{
	gNoutindex= 0;
	gOutindexmax= 0;
	gOutindex= hLongarray( NewHandle(0));
	FailNIL( gOutindex);
}

pascal void IndexCleanup(void)
{
	gOutindex= hLongarray( DisposeIfHandle( Handle( gOutindex)));
}

pascal void IndexOut( longint index)
{
	if (gNoutindex >= gOutindexmax) then begin
		gOutindexmax= gNoutindex + 20; 
		SetHandleSize( Handle(gOutindex), sizeof(longint)*gOutindexmax); 
		FailMemError();
		}
	(*gOutindex)^[gNoutindex]= index;
	gNoutindex= gNoutindex+1;
}
 
pascal void indexEOF( integer fileRef)
VAR index: longint;
{
	// indexout();  noutindex--; /* mark eof */ 
	if (0!=GetFPos( fileRef, index)) then ;
	IndexOut(index);
	gNoutindex= gNoutindex-1;
}



pascal void TSeqList::doWriteRef( integer aFileRef, integer format)  
VAR
		Boolean		needSameSize, sizesDiffer, isInterleaved;
		integer		aRow, nseqs, seqtype, err, vref, fileSave;
		longint		minbases, dirID;
		
	pascal void findMinSize( TSequence aSeq)
	longint		var  start, nbases;
	{
		aSeq->GetSelection( start, nbases);
		seqtype= aSeq->fKind;
		if (minbases<0) then minbases= nbases
		else begin
			if (nbases!=minbases) then sizesDiffer= true;
			minbases= min( nbases, minbases);
			end();
	}
	
	pascal void SetSameSize( TSequence aSeq)
	longint		var  start, nbases;
	{
		aSeq->GetSelection( start, nbases);
		aSeq->SetSelection( start, minbases); 
	}
	
	pascal void writeRangeToFile( TSequence aSeq)
	longint		VAR  index;
	{
		if (isInterleaved) then begin
			if (0!=GetFPos( aFileRef, index)) then;
			IndexOut(index);
			end();
		aSeq->doWriteSelection( aFileRef, format);
	}
	
	FailInfo		VAR  fi;
	pascal void HndlFailure(OSErr error, long message)		
	{
		aFileRef= fileSave;
		IndexCleanup(); 
		if 0!=HDelete(vRef, dirID, kInterleaveTemp) then ;
	}
	
	
{	
	fileSave= aFileRef;
	gOutindex= NULL; //make sure for failure
	vref= gAppVolRef; dirID= gAppDirID; 
	needSameSize= false;
	isInterleaved= false;
	sizesDiffer= false;
	seqtype= kOtherSeq;
	minbases= -1; 
	nseqs= this->GetSize();
	
	if (nseqs>1)) // deal w/ one-per-file formats 
		switch (format) {
			kGCG		: format= kMSF;
			kStrider: format= kIG;
			kNoformat, kPlain, kUnknown: format= kGenbank;
			}
		
	this->Each( findMinSize);	
	
	WriteSeqHeader(aFileRef, FALSE, format, nseqs, minbases, gOutputName, 
				needSameSize, isInterleaved);

	CatchFailures(fi, HndlFailure);
	if (isInterleaved) {
		IndexSetup();
		err= FindFolder(kOnSystemDisk,kTemporaryFolderType,kCreateFolder,vRef,dirID);
		if (err!=0)) { vref= gAppVolRef; dirID= gAppDirID; }
		if 0!=HDelete(vRef, dirID, kInterleaveTemp) then ;
		FailOSErr( HCreate( vRef, dirID, kInterleaveTemp, kSappSig, 'TEXT'));
		FailOSErr( HOpen( vRef, dirID, kInterleaveTemp, fsRdWrPerm, aFileRef));
		}
	
	if (((needSameSize || isInterleaved/*?*/) && sizesDiffer)) {
		/*---
		ParamText('all sequences must have same # bases for this format','', 'WriteSeq','');	
		Failure( -1, msgMyError); 
		---*/
		this->Each( SetSameSize);
		}

	this->Each( writeRangeToFile);		
	
		/* ^^ if format in [interleaved kinds] then 
			(a) write header info to aFileRef
			(b) open temp file, write sequential seqs to that
			(c) rewind temp, interleave and write to aFileRef
			(d) write trailer info
			-- make writeHeader, writeTrailer methods of TSeqList...
			** need alert dlogs for (1) oneperfile, (2) needSameSize problems
		*/

	if (isInterleaved) {
		indexEOF( aFileRef);
		DoInterleave( fileSave, FALSE/*isHandle*/, format, seqtype, nseqs,
									minbases, gLinesPerSeqWritten, 
									gNoutindex, Handle(gOutindex), aFileRef);

		FailOSErr( FSClose(aFileRef));
		if 0!=HDelete(vRef, dirID, kInterleaveTemp) then ;
		IndexCleanup();
		}
	aFileRef= fileSave;
	Success(fi);
		
	WriteSeqTrailer( aFileRef, FALSE, format, this->GetSize(), gLinesPerSeqWritten, minbases);

}


pascal void TSeqList::doWrite( Str255 aFileName, integer format)  
VAR  err, aFileRef: integer;
{
	gOutputName= aFileName;
	err= FSDelete( aFileName, 0);  //!?? must check before call if overwrite is okay ????
	FailOSErr( Create( aFileName, 0, kSAppSig, 'TEXT'));
	FailOSErr( FSOpen( aFileName, 0, aFileRef));
	
	doWriteRef( aFileRef, format);
	
	FailOSErr( FSClose( aFileRef));
}


pascal Handle TSeqList::doWriteHandle( integer format)  
VAR
		Handle		aHand, handSave;
		Boolean		needSameSize, sizesDiffer, isInterleaved;
		integer		aRow, nseqs, seqtype, err, vref, tempFileRef;
		longint		minbases, dirID;
		
	pascal void findMinSize( TSequence aSeq)
	longint		var  start, nbases;
	{
		aSeq->GetSelection( start, nbases);
		seqtype= aSeq->fKind;
		if (minbases<0) then minbases= nbases
		else {
			if (nbases!=minbases) then sizesDiffer= true;
			minbases= min( nbases, minbases);
			}
	}
	
	pascal void SetSameSize( TSequence aSeq)
	longint		var  start, nbases;
	{
		aSeq->GetSelection( start, nbases);
		aSeq->SetSelection( start, minbases);
	}
	
	pascal void writeRangeToHand( TSequence aSeq)
	longint		VAR index;
	{
		if (isInterleaved) then begin
			if (0!=GetFPos( tempFileRef, index)) then ;
			IndexOut(index);
			aSeq->doWriteSelection( tempFileRef, format);
			end
		else
			aSeq->doWriteSelectionHandle( aHand, format);
	}
		
	FailInfo		VAR  fi;
	pascal void HndlFailure(OSErr error, long message)		
	{
		IndexCleanup(); 
		aHand= DisposeIfHandle( aHand);
	}
	
	
{
	gOutindex= NULL; //make sure for failure
	aHand= NULL;
	minbases= -1; 
	needSameSize= false;
	isInterleaved= false;
	sizesDiffer= false;
	seqtype= kOtherSeq;
	nseqs= this->GetSize();
	this->Each( findMinSize);		

	if (nseqs>1)) // deal w/ one-per-file formats 
		switch (format) {
			kGCG		: format= kMSF;
			kStrider: format= kIG;
			kNoformat, kPlain, kUnknown: format= kGenbank;
			}

	CatchFailures(fi, HndlFailure);
	aHand= NewPermHandle(0);
	FailMemError();
	
	WriteSeqHeader(longint(aHand), TRUE, format, nseqs, minbases, gOutputName,
			needSameSize, isInterleaved);
			
	if (isInterleaved) {
		IndexSetup();
		err= FindFolder(kOnSystemDisk,kTemporaryFolderType,kCreateFolder,vRef,dirID);
		if (err!=0)) { vref= gAppVolRef; dirID= gAppDirID; }
		if 0!=HDelete(vRef, dirID, kInterleaveTemp) then ;
		FailOSErr( HCreate( vRef, dirID, kInterleaveTemp, kSappSig, 'TEXT'));
		FailOSErr( HOpen( vRef, dirID, kInterleaveTemp, fsRdWrPerm, tempFileRef));
		}
	
	if (((needSameSize || isInterleaved/*?*/) && sizesDiffer)) {
		/*---
		ParamText('all sequences must have same # bases for this format', '', 'WriteSeq','');	
		Failure( -1, msgMyError); 
		---*/
		this->Each( SetSameSize);
		}

	this->Each( writeRangeToHand);		

	if (isInterleaved) {
		indexEOF( tempFileRef);
		DoInterleave( longint(aHand), TRUE/*isHandle*/, format, seqtype, nseqs,
									minbases, gLinesPerSeqWritten, 
									gNoutindex, Handle(gOutindex), tempFileRef);

		FailOSErr( FSClose(tempFileRef));
		if 0!=HDelete(vRef, dirID, kInterleaveTemp) then ;
		IndexCleanup();
		}

	WriteSeqTrailer(longint(aHand), TRUE, format, this->GetSize(), gLinesPerSeqWritten, minbases);

	Success(fi);	
	doWriteHandle= aHand;
}


pascal void TSeqList::AddNewSeq(void)
VAR  aSeq: TSequence;
{
	NEW(aSeq); 
	FailNIL(aSeq);
	aSeq->ISequence(); 
	InsertLast( aSeq);
	aSeq->fIndex= this->GetSize();
}

pascal void TSeqList::ClearSelections(void)
	pascal void clearOne( TSequence aSeq)
	{
		aSeq->ClearSelection();
	}
{
	this->Each(ClearOne);
}

pascal integer TSeqList::ZeroOrigin(void)
VAR	shiftall: integer;

	pascal void findshift( TSequence aSeq)
	{
		if ((aSeq->fOrigin<0))
			shiftall= min(shiftall, aSeq->fOrigin);
	}
	
	pascal void doshift( TSequence aSeq)
	{
		aSeq->fOrigin= aSeq->fOrigin - shiftall;
		if (aSeq->fOrigin > 0) {
			aSeq->InsertSpacers( 0, aSeq->fOrigin, indelEdge);
			aSeq->fOrigin= 0;
			}
	}
{
	shiftall= 0;
	this->Each(findshift);
	this->Each(doshift);
	ZeroOrigin= shiftall;
}


pascal TSequence TSeqList::Consensus(void)
VAR 	cons: TSequence;
	pascal boolean isConsensus( TSequence aSeq)
	{
		isConsensus= aSeq.isConsensus;
	}
{
	Consensus= TSequence(this->FirstThat(isConsensus)); 		
}


pascal Integer TSeqList::ConsensusRow(void)
VAR 	cons: TSequence;
{
	cons= this->Consensus();		
	if ((cons != NULL)) 
		ConsensusRow= this->GetIdentityItemNo( cons) 
	else
		ConsensusRow= 0;
}

pascal void TSeqList::MakeConsensus(void)
VAR  
	TSequence		cons;
	CharsHandle		hCon, hSeq;
	integer		count ;
	len, conlen		: longint;
	arow	: integer;
	arect	: VRect;
	
	pascal void addToCons(TSequence aSeq)
	VAR	 	i : integer;
	{
		if (!(aSeq.isConsensus || (aSeq->fKind=kOtherSeq))) {
			count= count+1;
			handle(hSeq)= aSeq->fBases;
			len= GetHandleSize(Handle(hSeq));
			if (len > conlen then begin
				conlen= len;
				SetHandleSize( Handle(hCon), conlen);
				FailMemError();
				}
			if (count=1) {
				for i= 0 to len-1 do (*hCon)^[i]= (*hSeq)^[i];
				}			else {
				for i= 0 to len-1 do  
					(*hCon)^[i]= NucleicConsensus(NucleicBits((*hCon)^[i]), NucleicBits((*hSeq)^[i]));  
				}
			}
	}
		
{
	cons= this->Consensus();		
	if ((cons=NULL)) {
		NEW(cons); 
		FailNIL(cons);
		cons->ISequence();
		cons->fName= kConsensus;
		InsertLast( cons); //?? first or last 
		cons->fIndex= this->GetSize();
		}
		
	if ((cons != NULL)) {
		arow= this->GetIdentityItemNo( cons);
		handle(hCon)= cons->fBases;   
		conlen= GetHandleSize(handle(hCon));
		count= 0;
		this->Each( addToCons);  
		}
}

 
pascal charsHandle TSeqList::FindCommonBases( integer minCommonPerCent, 
											CharsHandle VAR firstCommon)
VAR  
	TSequence		cons;
	CharsHandle		hCon, hSeq;
	longint		len, maxlen, conlen, ibase;
	arow	: integer;
	letcount : array ['A'..'Z'] of integer;
	letfirst : array ['A'..'Z'] of byte;
	integer		spccount, nilcount ;

	integer		nseq ;
	maxc	: real;
	char		maxch, ch;
	CharsHandle		hMaxbase;
	hAtbase	: CharsHandle;
	
	pascal void findCommon(TSequence aSeq)
	char		VAR ch;
	{
		if !(aSeq.isConsensus || (aSeq->fKind=kOtherSeq)) then {
			nseq= nseq+1;
			handle(hSeq)= aSeq->fBases;
			len= GetHandleSize(Handle(hSeq));
			/*------
			maxlen= max(maxlen, len);
			-------*/
			ch= toupper( (*hSeq)^[ibase]);
			if (ch >= 'A') && (ch <= 'Z') then begin
				letcount[ch]= letcount[ch]+1;
				if (letfirst[ch] < 1) then letfirst[ch]= Min(255, nseq);
				end
			else if (ch == indelSoft) || (ch == indelHard) || (ch == indelEdge)
				|| (ch == ' ') || (ch == '_') then
				spccount= spccount+1
			else 
				nilcount= nilcount+1;
			}
	}
		
{
	cons= this->Consensus();		
	if ((cons=NULL)) cons= TSequence(this->First()); //any seq will do...
	
	hMaxBase= NULL;
	firstCommon= NULL;
	if ((cons!=NULL)) {		
		arow= this->GetIdentityItemNo( cons);
		handle(hCon)= cons->fBases;   
		maxlen= GetHandleSize(handle(hCon)); //???
		
		handle(hMaxBase)= NewHandle(maxlen); 
		FailMemError();
		handle(firstCommon)= NewHandle(maxlen); 
		FailMemError();
		
		FOR ibase= 0 to maxlen-1/*?*/ do {
			for ch= 'A' to 'Z' do begin letcount[ch]= 0; letfirst[ch]= 0; end;
			nilcount= 0; 
			spccount= 0;
			nseq= 0;
			this->Each( findCommon);
			maxc= 0;
			FOR ch= 'A' to 'Z' do 
				if letcount[ch] > maxc then { 
					maxc= letcount[ch]; 
					maxch= ch; 
					}
			if ((100 * maxc / nseq) >= minCommonPerCent) then  //mark maxch's
				(*hMaxbase)^[ibase]= maxch
			else
				(*hMaxbase)^[ibase]= '!';
			(*firstCommon)^[ibase]= ((char)(letfirst[maxch]));
			}
		}
	
	FindCommonBases= hMaxBase;
}
 






// TSeqListView			== OBJECT (TTextListView) ------------------


pascal void TSeqListView::Infoheadline(Str255 var aStr)
var  
	str255		s ;
{
	s= 'Sequence';
	while length(s) < kMaxSname do s= concat(s,' ');
	aStr= s;
	s= 'Kind   ';  
	aStr= concat(aStr,' ',s);
	s='  Size';
	aStr= concat(aStr,' ',s);
	s=' Check';
	astr= concat(astr,'  ',s);
	s= 'Bases';
	while length(s) < kMaxSeqShow do s= concat(s,' ');
	astr= concat(astr,' ',s);
	s='Last Modified';
	astr= concat(astr,' ',s);
}

pascal void TSeqListView::GetItemText(integer anItem, Str255 VAR aString)
			// override 
VAR
		TSequence		aSeq;
{
	aString= '-';
  if fSeqList != NULL then 
	 if (anItem <= fSeqList->fSize) then begin
		aSeq= TSequence(fSeqList->At(anItem));
		if aSeq != NULL then aSeq->Infoline(aString);
		end();
}


pascal void TSeqListView::Initialize(void) // override 
{
	inherited::Initialize();
	fSeqList= NULL; 
}


pascal void TSeqSelectCommand::ComputeNewSelection(GridCell VAR clickedCell) // override 
{
	//keep selection fixed at max size ?
  if fShiftKey 
		&& !fGridView->fSingleSelection 
		&& fGridView->IsCellSelected(clickedCell))  
	  	clickedCell= fGridView->LastSelectedCell(); //last line

	inherited::ComputeNewSelection(clickedCell);
}


pascal void TSeqSelectCommand::DoIt(void) // override 
//example/dummy DoIt
var
	aCell	: GridCell;
{
	inherited::DoIt();
	aCell.h= 1;
	aCell.v= fAtRow;
	if !fGridView->IsCellSelected( aCell) then fDoubleClick= FALSE;
	
	if fDoubleClick then TSeqListDoc(fView->fDocument).DoSeqSelectCommand;
}


/*--
pascal TCommand TSeqListView::DoMouseCommand(VAR theMouse:Point; EventInfo VAR Info,
			point VAR hysteresis) // override 
--*/
pascal void TSeqListView::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 
VAR
	aCell	: GridCell;
	aCommand		: TSeqSelectCommand;
	boolean		doToggle, doShift;
{
	if ((IdentifyPoint(theMouse, aCell) != badChoice)) {
		New(aCommand);
		FailNIL(aCommand);
		aCommand->fDoubleClick= (event->fClickCount > 1);
		aCommand->fAtRow= aCell.h;
		if (firstSelectedItem <= aCell.h) && (lastSelectedItem >= aCell.h) then
			doShift= TRUE
		else
			doShift= event->IsShiftKeyPressed(); //- info.theShiftKey;
		if aCommand->fDoubleClick then
			doToggle= FALSE
		else if (firstSelectedItem == aCell.h) && (lastSelectedItem == aCell.h) then
			doToggle= TRUE
		else 
			doToggle= event->IsCommandKeyPressed(); //-info.theCommandKey;
		aCommand->ICellSelectCommand(this, theMouse, doShift, doToggle);
		PostCommand( aCommand);
		}
}



/*-- pascal TCommand TSeqListDlog::DoMouseCommand(VAR theMouse:Point; EventInfo VAR Info,
							point VAR hysteresis) // override */ ---
pascal void TSeqListDlog::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 
{
	TSeqListDoc(fDocument).fSeqListView->SetEmptySelection(kHighlight);
	inherited::DoMouseCommand( theMouse, event, hysteresis);
}

pascal void TSeqListDlog::CalcMinFrame(VRect VAR minFrame) // override 
//! need to check CalcMinFrame of window 
var  aDoc: TSeqListDoc;
		 longint		dsize;
		 VRect		listVRect;
{
  inherited::CalcMinFrame(minFrame); //do horiz ?
	
	aDoc= TSeqListDoc(fDocument);
	dsize= 0;
	if ((aDoc->fSeqList != NULL)) 
		dsize= aDoc->fSeqList.fSize + 2;
	/*-- 
		fSeqListView is NULL now when called from NewTemplateWindow 
		at top of TSeqListDoc.DoMakeViews
		... adding TSeqListDlog.IRes w/ inits may overcome that
	---*/
	
	if ((aDoc->fSeqListView != NULL)) {
		aDoc->fSeqListView->GetFrame( listVRect);
		dsize= listVRect.top + (aDoc->fSeqListView.fLineHeight * dsize);
		}
	minFrame.bottom= max(minFrame.bottom, dsize);
}

 
pascal void TSeqListDlog::Initialize(void) // override 
{
	inherited::Initialize();
}
 


// TSeqListDoc	 ----------

CONST
		kProjType == 'TEXT';
		kSeqListDocID == 1007;
		
pascal void unDeadStripObjects(void)
{
	if (gDeadStripSuppression) {
	/*---
		if (Member(TObject(NULL), TReadGCG));
		if (Member(TObject(NULL), TReadFitch));
		if (Member(TObject(NULL), TReadZuker));
		if (Member(TObject(NULL), TReadEMBL));
		if (Member(TObject(NULL), TReadPearson));
		if (Member(TObject(NULL), TReadNBRF));
		if (Member(TObject(NULL), TReadGB));
		if (Member(TObject(NULL), TReadStrider));
		if (Member(TObject(NULL), TReadIG));
		if (Member(TObject(NULL), TReadSeq));
		if (Member(TObject(NULL), TReadOlsen));
	----*/		
		if (Member(TObject(NULL), TSequence));
		if (Member(TObject(NULL), TSeqList));
		if (Member(TObject(NULL), TSeqListView));
		if (Member(TObject(NULL), TSeqListDlog));
		if (Member(TObject(NULL), TSeqListDoc));
		}
}



pascal void TSeqListDoc::ISeqListDoc(TFile itsFile)
VAR
	aSeq		: TSequence;
{
	UnDeadStripObjects(); //!? keep'em in code
	
	this->IFileBasedDocument(itsFile, kProjType);
	
	fInFormat= kNoformat;
	fHeadline	= NULL;
	fUpdateTime= 0;
	fSeqListView= NULL;
	gSeqListDoc= this; //!*/	/*? store other gSeqListDocs in gApp.TList ?
	GetTitle(gDefSeqName);
	fSaveSelection= false;	

	//! do this in .ISeqListDoc, as DoRead comes before DoMakeViews 
	fSeqList= TSeqList(NewList);  
	FailNIL(fSeqList);
	fSeqList->AddNewSeq();	// install 1 blank seq for new doc
}



pascal Boolean TSeqListDoc::ListIsEmpty(void)
{
	if ((fSeqList!=NULL)) ListIsEmpty= fSeqList.ListIsEmpty
	else ListIsEmpty= TRUE;
}


pascal void TSeqListDoc::AddSeqToList(TSequence item)
var last: integer;
{
	fSeqList->InsertLast(item);
	item->fIndex= fSeqList->GetSize();
	if (fSeqListView!=NULL) {
		fSeqListView->InsItemLast(1);
		last= fSeqListView->fNumOfRows;
		fSeqListView->SelectItem( last, FALSE, TRUE, TRUE);
		}
	//- fChangeCount= fChangeCount+1;
	this->Changed( 1, this);
}	

pascal void TSeqListDoc::AddNewSeqToList(void)
VAR  aSeq: TSequence;
{
	NEW(aSeq); 
	FailNIL(aSeq);
	aSeq->ISequence(); 
	AddSeqToList( aSeq);
}
 

pascal void TSeqListDoc::DoSeqSelectCommand(void)
{
	//OVERRIDE this to do something w/ selected seqs on dblclk
}


pascal void TSeqListDoc::DoMakeViews(Boolean forPrinting) // override 
VAR
		s		: str255;
		TView		aView;
		TSeqListDlog		aDialogView;
		TStdPrintHandler		aHandler;
{
	fHeadline= NULL;
	fSeqListView= NULL;
	aView = gViewServer->NewTemplateWindow(kSeqListDocID, this);
	FailNIL(aView); 				 
	 
  fHeadline = TStaticText(aView->FindSubView('HEAD'));
	FailNIL(fHeadline);
	fSeqListView= TSeqListView(aView->FindSubView('LIST'));
	FailNIL(fSeqListView);
  fFormatPop= TPopup(aView->FindSubView('pFMT'));

	fSeqListView->fSeqList= fSeqList;  
	fSeqListView->Infoheadline(s);	
	fHeadline->SetText( s, FALSE);
	
	//get superview of window, minus scrolls, for printing
	aDialogView = TSeqListDlog(aView->FindSubView('DLOG'));   	
	New(aHandler);
	FailNIL(aHandler);
	aHandler->IStdPrintHandler(this, 					// its document 
							  aDialogView,					// its view 
							  !kSquareDots,			// does not have square dots 
							  kFixedSize, 				// horzontal page size is fixed 
							  !kFixedSize);			/* vertical page size is variable (could be
														  set to TRUE on non-style TE systems) */
 
	ShowReverted();
}



pascal void TSeqListDoc::DoSetupMenus(void) // override 
{
  inherited::DoSetupMenus();	
}

pascal void TSeqListDoc::DoMenuCommand(CommandNumber aCommandNumber) // override 
{
	inherited::DoMenuCommand(aCommandNumber);
}



pascal void TSeqListDoc::ShowReverted(void) // override 
var  nnew, nold: integer;
		 longint		t;
		 
		procedure update(TSequence aSeq)
		begin
		 //?? can't do "<" comparison of times, negative longint
		 //- if fUpdateTime < aSeq->fModTime then  
		 if aSeq->fChanged then aSeq->UpdateFlds();
		end();
		
{
	inherited::ShowReverted();
	
	fSeqList->each(update);
	if ((fSeqListView!=NULL)) {
		fSeqListView->fSeqList= fSeqList;  
		nold= fSeqListView->fNumOfRows;
		nnew= fSeqList->fSize;
		fSeqListView->DelItemLast(nold);
		fSeqListView->InsItemLast(nnew);
		}
	GetDateTime(t);
	fUpdateTime	= t;
}



/***
function TSeqListDoc.filedate: longint;
begin
	HLock(Handle(fTitle));
	//$Push*/ /*$H-
	filedate = FileModDate((*fTitle)^, fVolRefNum);
	//$Pop
	HUnLock(Handle(fTitle));
end;
****/




pascal void TSeqListDoc::AppendRead(short aRefNum)
{	
	GetTitle(gDefSeqName);
	fInFormat= ReadSeqFile(aRefNum, FALSE, fSeqList);  
}

pascal void TSeqListDoc::AppendHandle( Handle aHand)
{	
	GetTitle(gDefSeqName);
	fInFormat= ReadSeqFile(Longint(aHand), TRUE, fSeqList);  
}



pascal void TSeqListDoc::AboutToSaveFile(TFile theSaveFile, CommandNumber itsCmd, 
															Boolean VAR makingCopy) // override 
VAR
		Boolean		needSameSize, sizesDiffer, isInterleaved;
		integer		nseqs, err, format ;
		longint		minbases;
		Handle		aHand;
		
	pascal void checkEqualSize( TSequence aSeq)
	longint		var  start, nbases;
	{
		//aSeq->GetSelection( start, nbases); << can still be set from fSelection or other old selecting command 
		nBases= aSeq->fLength; 
		if (minbases<0) then minbases= nbases
		else begin
			if (nbases!=minbases) then sizesDiffer= true;
			//- minbases= min( nbases, minbases); 
			end();
	}

{
	inherited::AboutToSaveFile(theSaveFile, itsCmd, makingCopy);
	
	if ((!fSaveSelection)) { 
					//^^^^ !???? TAlnDoc will set this, always equal #bases????
		if (fFormatPop == NULL) format= kGenBank
		else format= fFormatPop->GetCurrentItem(); 
		needSameSize= false;
		isInterleaved= false;
		sizesDiffer= false;
		minbases= -1; 
		nseqs= fSeqList->GetSize();
		
		if (nseqs>1)) // deal w/ one-per-file formats 
			switch (format) {
				kGCG		: format= kMSF;
				kStrider: format= kIG;
				kNoformat, kPlain, kUnknown: format= kGenbank;
				}
			
		fSeqList->ClearSelections(); //! make sure we write all of seq from this call !?
		fSeqList->Each( checkEqualSize);	
	
		aHand= NewHandle(0);
		WriteSeqHeader( longint(aHand), TRUE, format, nseqs, minbases, gOutputName, 
					needSameSize, isInterleaved); //<< need write for last two results 
		aHand= DisposeIfHandle(aHand);
		
		if (((needSameSize || isInterleaved/*?*/) && sizesDiffer)) {
			ParamText('all sequences must have same # bases for this format',
					' Pick another format like Genbank or change selection.',
					'WriteSeq','');	
			Failure( -1, msgMyError);  
			}
		}
}


pascal void TSeqListDoc::DoRead(TFile aFile, Boolean forPrinting) // override 
{
	//? if forPrinting then ???
	FreeData();   //free initial stuff
  AppendRead( aFile->fDataRefNum); //<< change AppendRead to use aFile.WriteData ?
}

pascal void TSeqListDoc::DoReadHandle(Handle aHand)
{
	FreeData();   //free initial stuff
  AppendHandle( aHand);
}


	
pascal void TSeqListDoc::DoWrite(TFile aFile, Boolean makingCopy) // override 
VAR   outformat: integer;  Str63 aStr63,
{
	aFile->GetName( aStr63); gOutputName= aStr63;
	if (fFormatPop == NULL) outformat= kGenBank
	else outformat= fFormatPop->GetCurrentItem(); 
	fSeqList->ClearSelections(); //! make sure we write all of seq from this call !?
	fSeqList->doWriteRef( aFile->fDataRefNum, outformat);
}


pascal void TSeqListDoc::Close(void) // override 
//must make sure all windows owned by ListDoc are closed 1st

	procedure closeSubwindows(TWindow aWind)
	begin
	  // this is a subwindow !may not be owned by this  doc
		if (!aWind->fClosesDocument) aWind->CloseAndFree();
	end();
	
{
	ForAllWindowsDo(closeSubwindows);
	inherited::Close();
}

pascal void TSeqListDoc::Free(void) // override 
{
	FreeData();
	TSortedList(fSeqList)= FreeListIfObject( fSeqList);
	if (fSeqListView!=NULL) fSeqListView->fSeqList= NULL;
	gSeqListDoc= NULL;
	inherited::Free();
}

pascal void TSeqListDoc::FreeData(void) // override 
{
	if (fSeqList != NULL) fSeqList->FreeAll(); //frees each item, sets fsize=0
}


