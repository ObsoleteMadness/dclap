// USeqPrint.tree.p 
// d.g.gilbert, 1991 

/*

phylogenetic tree print

*/

#pragma segment TreeDraw

CONST

	kTreePrintWindowRSRCID == 1030;

	kUnrooted == 1;
	kCladogram == 2;
	kPhenogram == 3;
	kEurogram == 4;
	kSwoopogram == 5;
	kCurvogram == 6;
	kDefaultTreeStyle == kPhenogram;

// TTreePrintDocument -----------------------------------------------


pascal void TTreePrintDocument::ITreePrintDocument(OSType fileType, treeData,
						Handle		params; TFile itsFile)
VAR  
	aStr: string[80];
	i	: integer;
	TFile		aFile;
{
	fTreeData= NULL;
	fParams= NULL;
	fTreePrintView = NULL;
	
	if ((treeData!=NULL)) FailOsErr( HandToHand( treeData)); //Dup it so we own it
	fTreeData= treeData; //may be nil if reading from file...
	if ((params!=NULL))
		FailOsErr( HandToHand( params))
	else {
		/*temp fix dummy param file...
			Style Clado
			GRows Horiz 
		*/
		//- aStr= 'Style (*Clado)GRows (*Horiz)0'; 
		aStr= '^^0'; 
		i= length(aStr);  aStr[i]= ((char)(0));
		i= pos('^',aStr); aStr[i]= ((char)(13));
		i= pos('^',aStr); aStr[i]= ((char)(13));
		params= Handle(NewString(aStr));
		FailNIL( params);
		CharsPtr((*params))^[0]= ' ';
		}
	fParams= params;
	
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

pascal TFile TTreePrintDocument::DoMakeFile(itsCommandNumber:CommandNumber); // override 
{
	DoMakeFile= NewFile(fScrapType, gApplication->fCreator, kUsesDataFork, kUsesRsrcFork, 
												!kDataOpen, !kRsrcOpen);
}

pascal void TTreePrintDocument::FreeData(void)
{
	//- if ((fTreePrintView!=NULL)) fTreePrintView->Free(); */ /*??
	fTreeData= DisposeIfHandle( fTreeData);
	fParams= DisposeIfHandle( fParams);
}


pascal void TTreePrintDocument::Free(void) // override 
{
	FreeData();
	inherited::Free();
}



pascal void TTreePrintDocument::DoMakeViews(Boolean forPrinting) // override 
VAR
		TWindow		aWindow;
		Point		minSize;
		Point		maxSize;
		vSize 	: Vpoint;
		TStdPrintHandler		aHandler;
{

	aWindow = gViewServer->NewTemplateWindow(kTreePrintWindowRSRCID, this);
	FailNil(aWindow);
	
	fTreePrintWind= TTreePrintWind(aWindow);
	fTreePrintWind->ITreePrintWind();

	fTreePrintView = TTreePrintView(aWindow->FindSubView('PrVw'));
	FailNil(fTreePrintView);

	fTreePrintView->fStylePop= TPopup(aWindow->FindSubView('pSTL'));
	fTreePrintView->fHoriz= TRadio(aWindow->FindSubView('rHOR'));
	//- fTreePrintView->fVert= TRadio(aWindow->FindSubView('rVER')); 
	fTreePrintView->fFixed= TCheckBox(aWindow->FindSubView('cFIX'));
	fTreePrintView->fNodeLen= TCheckBox(aWindow->FindSubView('cNDL'));
	fTreePrintView->fNodeCluster= TCluster(aWindow->FindSubView('node'));
	fTreePrintView->fRedrawBut= TButton(aWindow->FindSubView('bRDR'));
	
	if (fTreePrintView->fStylePop == NULL) 
		fTreePrintView->fStyle= kDefaultTreeStyle
	else
		fTreePrintView->fStyle= fTreePrintView->fStylePop->GetCurrentItem();  

	fTreePrintWind->fStylePop= fTreePrintView->fStylePop;
	fTreePrintWind->fRedrawBut= fTreePrintView->fRedrawBut;
	fTreePrintWind->fGrowCluster= TCluster(aWindow->FindSubView('grow'));
	fTreePrintWind->fNodeCluster= TCluster(aWindow->FindSubView('node'));
	
	fTreePrintView->fScroller = fTreePrintView->GetScroller(TRUE);
			//fix so prefview scroll bar shows
/*-	if ((fTreePrintView->fScroller!=NULL)) 
			fTreePrintView->fScroller.fScrollLimit.v= 500;*/ 

		//! get initial calc of data 
	fTreePrintView->CalcTree();
			
	// set window's resize limits so it can't become wider than the TreePrintview's edge 
	/*---
	WITH aWindow->fResizeLimits){
		minSize = topLeft;
		maxSize = botRight;
		}
	WITH maxSize)h = Min( 2 + fTreePrintView->fSize.h + kSBarSizeMinus1, h);
	aWindow->SetResizeLimits(minSize, maxSize);
	-----*/
	
	vsize.h= maxSize.h;
	vsize.v= maxSize.v;
	if ((fTreePrintView->fScroller!=NULL)) 
		fTreePrintView->fScroller->SetScrollLimits(vsize, kDontRedraw);
		 
	ShowReverted();  //!!!

}


pascal void TTreePrintDocument::UntitledName(Str255 VAR noName) // override 
{
	noName= 'Untitled Tree';
	//- inherited::UntitledName( noName); 
	/*------
	noName= Concat(fParentDoc.(*fTitle)^,' Tree'); 
	if ((fWindowList != NULL) && (fWindowList.GetSize > 0)) 		
		TWindow(fWindowList->First())->SetTitle(noName);
	----*/
}


/*
PICT file has the following format:
  Data Fork:
	(a)  kMacdrawHeaderSize (512 bytes) == nulls
	(b)  The rest => PICTure of window

 Resource Fork:
	nothing, yet
*/



pascal void TTreePrintDocument::DoNeedDiskSpace(TFile itsFile,
															long VAR dataForkBytes, rsrcForkBytes)
{
	//)!get Print record requirements 
	//- inherited::DoNeedDiskSpace(dataForkBytes, rsrcForkBytes); 
	
	dataForkBytes = dataForkBytes + kMacdrawHeaderSize  /*+ sizeof window pict */;
	
	/*-- if you really want to know pict size:
	fTreePrintView->WriteToDeskScrap(); 
	len= fTreePrintView->GivePasteData( NULL, 'PICT');
	----*/
	/*---
	rsrcForkBytes = rsrcForkBytes + kRsrcTypeOverhead + kRsrcOverhead + sizeof(DocState);
	---*/
}



	
pascal void TTreePrintDocument::DoRead(aFile:TFile; Boolean forPrinting) // override 
VAR
		longint		numChars;
		hData	: Handle;
		err		: OSErr;		
		p			: CharsPtr;
{	
	//-- inherited::DoRead(aRefNum, rsrcExists, forPrinting);)!read print info stuff
	// This is a Write-Only document == PICT of output drawing, no reading... ?
}



pascal void TTreePrintDocument::DoWrite(TFile aFile, Boolean makingCopy)
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

	fTreePrintView->WriteToDeskScrap();
	
	hPict= NewHandle(0);
	CatchFailures(fi, HdlDoWrite);
	len= fTreePrintView->GivePasteData( hPict, 'PICT');
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


// TTreePrintWind ------------------------- 

pascal void TTreePrintWind::ITreePrintWind(void)
{
	IPrefWindow();
	fWantSave= TRUE; 
}

pascal void TTreePrintWind::SetPrefID(void) /* override */ 
{
	gPrefWindID= kTreePrintWindowRSRCID; gPrefWindName= 'TTreePrintWind';
}

pascal void TTreePrintWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
VAR
		TTreePrintView		aTreePrintView;	
		
	 pascal void RedoTree(void)
	 {
		 aTreePrintView->CalcTree();
		 aTreePrintView->ForceRedraw();
	 }
	 
{
	aTreePrintView= TTreePrintDocument(fDocument).fTreePrintView;
	switch (eventNumber) {
		 
		mPopupHit	: if ((source == fStylePop)) RedoTree
			else inherited::DoEvent(eventNumber,source, event); 
			
		mButtonHit: if ((source == fRedrawBut)) RedoTree
			else inherited::DoEvent(eventNumber,source, event); 
			
		mClusterHit: if ((source == fNodeCluster) || (source == fGrowCluster) 
) RedoTree
			else inherited::DoEvent(eventNumber,source, event); 
			
		//dialogView eats all mEditTextHits -- 

		otherwise
			inherited::DoEvent(eventNumber,source, event); 
		}
}




// TTreePrintView -------------------------------------



pascal void TTreePrintView::ITreePrintView( TTreePrintDocument itsDocument, Boolean forClipboard)
VAR
		itsSize		: VPoint;
		aHandler	: TTreePrintHandler;
		sd				: SizeDeterminer;
{
	SetVPt(itsSize, kMaxCoord, kMaxCoord);
	if (forClipboard)
		sd = sizeVariable
	else
		sd = sizeFillPages;
	IView(itsDocument, NULL, gZeroVPt, itsSize, sd, sd);
	fScroller = NULL;
	fStyle= kDefaultTreeStyle; 
	fTreePrintDocument = itsDocument;

	fTreeData= itsDocument->fTreeData;
	fParams	= itsDocument->fParams;
	fRoot= NULL;
	
#if FALSE										//!!! Need to handle this
	if (forClipboard) fWouldMakePICTScrap = TRUE;
#endif

	if (!forClipboard) {
		New(aHandler);
		FailNil(aHandler);
		aHandler->ITreePrintHandler(itsDocument, this, 
									!kSquareDots,  
								  kFixedSize,			// horizontal page size is fixed 
								  kFixedSize);			// vertical page size is fixed 
		}
}

 

pascal void TTreePrintView::Initialize(void) // override 
{
	fScroller = NULL;
	fRoot= NULL;
	fStyle= kDefaultTreeStyle; 
	fTreePrintDocument= NULL;
	fTreeData= NULL;
	fParams= NULL;
	fMinSize= gZeroVPt;
	inherited::Initialize();
}

pascal void TTreePrintView::DoPostCreate(TDocument itsDocument) // override 
VAR
		TTreePrintHandler		aHandler;
{	
	inherited::DoPostCreate( itsDocument);
	fTreePrintDocument = TTreePrintDocument(itsDocument);
	fTreeData= TTreePrintDocument(itsDocument).fTreeData;
	fParams	 = TTreePrintDocument(itsDocument).fParams;
	New(aHandler); FailNil(aHandler);
	aHandler->ITreePrintHandler(itsDocument, this, !kSquareDots, // does not have square dots 
							  kFixedSize,  kFixedSize);		 
}
 

pascal void TTreePrintView::Free(void) // override 
{
	FreeData();
	inherited::Free();
}



pascal void TTreePrintHandler::ITreePrintHandler(TDocument itsDocument, TView itsView,
											Boolean itsSquareDots, itsHFixedSize, itsVFixedSize)
{
	IStdPrintHandler( itsDocument, itsView,itsSquareDots,itsHFixedSize,itsVFixedSize);

}

pascal VCoordinate TTreePrintHandler::BreakFollowing(VHSelect vhs,
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

	BreakFollowing= inherited::BreakFollowing(vhs, previousBreak, automatic);
	
/*********
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
*****/
}



CONST
	maxnch == 30;
	
TYPE
	plotstring == STRING[maxnch];   
				
	nodeptr == ^node;
	nodeHandle == ^nodeptr;
	node == RECORD
		/*b*/ next, back : nodeHandle;
		/*b*/ tip 			: Boolean;
		/*b*/ nayme 		: plotstring;
		/*r*/ tipsabove 	: short;  
		/*b*/ xcoord, ycoord, userlen : REAL;
		}
			
VAR //globals for DrawGram.... fold into locals as time permits
		PicRect, PicFrame	: Rect;
		treeXmin, treeYmin, treeXmax, treeYmax, 
		Real		picXscale, picYscale; //plot2pict scale factors
		integer		QDstemunit;
		PicFirst	: boolean;



pascal void TTreePrintView::ForEachNodeDo( pascal void DoThis( aNode: UNIV Handle));
	pascal void ToEachNode( nodeHandle p)
	nodeHandle		VAR  pp ;
	{ 
		 if ((p!=NULL)) {
			 if (!(*p)->tip) {
				pp = (*p)->next;
				while ( pp != p){
					ToEachNode ((*pp)->back);
					pp = (*pp)->next;
					}
				}
			DoThis( Handle(p));
			}
	}
{
	ToEachNode( NodeHandle(fRoot));
}
	

pascal void TTreePrintView::FreeData(void)
	pascal void disposeTree(void)
		pascal void DisposeNode( nodeHandle aNode)
		{ 
			DisposHandle( Handle(aNode));
		}
	{
		ForEachNodeDo( DisposeNode);
	}
{
	DisposeTree();
	fRoot= NULL;
}


procedure map2pic(real x,y, integer VAR h,v)
begin
	h= round( PicRect.left + (x-treeXmin) * picXscale);   
	v= round( PicRect.top  + (treeYmax-y) * picYscale); 

	/*-----
	WITH PicFrame){
		if (picFirst then begin
			left= h; right= h;
			top= v; bottom= v;
			picFirst= FALSE;
			end
		else {
			left	= min(left, h);
			right	= max(right,h);
			top		= min(top,v);
			bottom= max(bottom,v);
			}
		}
	----*/
end;

function QDfontHeight: integer;
var  fi: fontInfo;
begin
	getFontInfo(fi);
	QDfontHeight= fi.ascent + fi.descent + fi.leading;
end;

CONST
	k90deg		== 1;
	k180deg 	== 2;
	k270deg 	== 3;
	k0deg			== 0;

pascal void locateStr( integer h, v, rot,  str255 s, Rect VAR theRect)
var  fi: fontInfo;  integer w, d, rotnum,
{
	rotnum= ((rot+44) % 360) / 90;
	getFontInfo(fi);
	d =  fi.ascent + fi.descent + fi.leading ;
	w =  stringwidth(s) ;
	CASE (rotnum) OF
		k0deg, k180deg	:	SetRect( theRect, h, v, h + w, v + d);
		k90deg, k270deg	: SetRect( theRect, h, v, h + d, v + w);
		}
}

CONST
	kVertical == 0;
	kHorizontal == 1;
  
	kWeighted == 0; 
	kIntermediate == 1; 
	kCentered == 2; 
	kInner == 3;
	kVshaped == 4;
	

pascal void TTreePrintView::CalcTree(void)

pascal void CalcGram( Rect frame, 
					integer		theStyle, grows, nodeposition; 
					boolean		fixedsize, uselengths;
					hcFile VAR treefile, parmfile)
		
/* Version 3.3.  Copyright (c) 1986, 1990 by Joseph Felsenstein and
	Christopher A. Meacham.
	Permission is granted to copy, distribute, and modify this
	program provided that (1) this copyright message is not removed
	and (2) no fee is charged for this program. 
	
	hacked w/ a machetŽ by d.gilbert, June 1990:
	-- dropped a plotters but hp
	-- replaced NEW with NewPtr and DisposeTree
	-- dropped all font stuff
	-- replaced all file i/o with pointer i/o
	-- dropped all interactive stuff
		 and replaced w/ parmfile reader
	-- parmfile format:
		-- 1 space b/n param name_value, uppercase is minimum abbrev.
		Grow Horizontal/Vertical
		Style Cladogram/Phenogram/Eurogram/Swoopogram/curVogram
		Uselengths Yes/No
		Rotate 95.0
		Depth	1.23
		Length 1.23
		Nodelength 1.23
		Position kIntermediate/kWeighted/kCentered/kInner/kVshaped

	*/

CONST 
   //- maxnch == 30; 
    pointCh == '.';
    pi == 3.141592653;
    epsilon == 0.00001;
		pendown == TRUE;
		penup		== FALSE;
	
 		//paramfile keys 
		
		//Used letters: ABCDEFGhijkLmNoPqRSTUvwxyz 
		
		kStyle				== 'S';	//Style == Unrooted/Clado/Swoopo/Euro/Pheno/Curvo
	
		kPlotNodeNum	== 'B'; 
		kUnitPicScale == 'E';
		
			//Rooted tree params 
		kUseLengths		== 'U';	//Use lengths= Yes/No
		kFixedSize		== 'F';	//Fixed size
		kGrows 				== 'G';	/*GRows == Horiz/Vert*/		 
		kRotateLabel	== 'R';	//Rotate label
		kTreeDepth 		== 'D';	//Depth of tree
		kStemLength		== 'L';	//Length of stem
		kNodeSpace 		== 'N';	//Node space
		kNodePosition == 'P';	//Position of node
			
  //- TYPE 
    //- growth == integer; -- (vertical, horizontal);
    //- treestyle == integer; -- (unrooted, cladogram, phenogram, curvogram, eurogram, swoopogram);
		//- labelorient == (fixed, radial, along); 

/******
    plotstring == STRING[maxnch];   
					
    nodeptr == ^node;
		nodeHandle == ^nodeptr;
    node == RECORD
      /*b*/ next, back : nodeHandle;
      /*b*/ tip 			: Boolean;
      /*b*/ nayme 		: plotstring;
     	/*r*/ tipsabove 	: short;  
      /*b*/ xcoord, ycoord, userlen : REAL;
      }
*******/

VAR
    ntips, nextnode, nextnext,
		short		numtochange, oldx, oldy, nmoves, payge ;
		/*plotNodeNum,*/ haslengths : Boolean;
		// fixedSize , uselengths: boolean;
		unitPicScale,
		xmargin, ymargin, topoflabels, rightoflabels, leftoflabels, tipspacing,
      scale, nodespace, stemlength, treedepth, xsize, ysize 
      /* labelheight, */ /* labelrotation,*/  /*expand,  rooty*/ : REAL;
    //- grows : growth; 
    //- theStyle : treestyle; 
    //- root, where : nodeHandle;
    //- nodeposition : integer;*/ /*(kWeighted, kIntermediate, kCentered, kInner, kVshaped);
		


  pascal void uppercase (char VAR ch )
  { 
    if (((ch >= 'a') && (ch <= 'z'))) ch = ((char)(ORD(ch) - 32)); 
  }  

	pascal void getch (char VAR c ) 	// get next nonblank character 
	{  
		do {
			if (hcEOLn(treefile)) c= hcRead(treefile); //if (hcReadLN(treefile)!='');
			c= hcRead(treefile);
		} while (!(c != ))' ';
		if ((c == ((char)(0)))) c= ';'; //our terminator...
	}  


  pascal void treeread(void)
    // read a tree from the treefile and set up nodes and pointers 
    char		VAR ch ;
				char		quote;
				NodeHandle		aRoot;
				
    pascal void addelement (nodeHandle VAR p , nodeHandle q )
      /* read in and add next part of tree, it will be node p
        and will be hooked to pointer q */
      nodeHandle		VAR pfirst ;
        short		n ;
        Boolean		notlast ;
				done	: boolean;
				
      pascal void processlength(nodeHandle p )
        short		VAR digit, ordzero ;
          REAL		valyew, divisor ;
          Boolean		pointread ;
      { 
        ordzero = ORD('0');
        pointread = FALSE;
        valyew = 0.0;
        divisor = 1.0;
        getch(ch);
        digit = ORD(ch)-ordzero;
        while ( ((digit >= 0) && (digit <= 9)) || (ch=pointCh)){
          if (ch == pointCh) pointread = TRUE
          else {
            valyew = valyew*10.0 + digit;
            if (pointread) divisor = divisor*10.0;
            }
          getch(ch);
          digit = ORD(ch)-ordzero;
          }
       	(*p)->userlen = valyew/divisor;
      } // processlength 

    {  
      nextnode = nextnode + 1;
			Handle(p) = NewHandle(sizeof(node)); 
      if (ch == '(') {
        (*p)->tip = FALSE;
        (*p)->tipsabove = 0;
        pfirst = p;
        notlast = TRUE;
        while ( notlast){
					Handle((*p)->next) = NewHandle(sizeof(node));
          p = (*p)->next;
					nextnext= nextnext + 1;
          (*p)->tip = FALSE;
          getch (ch);
          addelement ((*p)->back, p);
          (*pfirst)->tipsabove = (*pfirst)->tipsabove + (*p)->(*back)->tipsabove;
          if ((ch == ')') || (ch == ';')) {
            notlast = FALSE;
            do {  getch (ch);
            } while (!((ch == ))':') || (ch == ',') || (ch == ')') || (ch == ';');
            }
          }
        (*p)->next = pfirst;
        p = pfirst;
        }      else {
        (*p)->tip = TRUE;
        (*p)->tipsabove = 1;
        ntips = ntips + 1;
        /*---
				if (((ch == '''') || (ch='"'))) {
					quote= ch;
          if (hcEOLn(treefile)) ch= hcRead(treefile);  
          ch= hcRead(treefile);
					}				
				else --*/
				quote= ((char)(0));
        n = 0;
        do {
					if ((ch == '_') && (quote <= ' ')) ch = ' ';
					if (n < maxnch) { n= n+1; (*p)->nayme[n] = ch; }
          if (hcEOLn(treefile)) ch= hcRead(treefile);
					ch= hcread(treefile);
				//- if ((quote > ' ')) done= (ch == quote) else 
					done= (ch == ':') || (ch == ',') || (ch == ')') || (ch == ';');
        } while (!(done));
				(*p)->nayme[0]= ((char)(n));  
        }
      if (ch == ':') processlength (p)
      						else haslengths = haslengths && (q == NULL);
      (*p)->back = q;
    } // addelement 

  {  
    haslengths = TRUE;
    ntips = 0;
    nextnode = 0; nextnext= 0;
    getch (ch);
		aRoot= NULL;
    addelement (aRoot, NULL);
		NodeHandle(fRoot)= aRoot;
		if (hcReadln(treefile) != '') ;
    //- uselengths = haslengths;
		if (!haslengths) uselengths= FALSE;
  } // treeread 


  pascal void initialparms(void)
	
		pascal void plotrparms(void)//for Macintosh
		{ 
			PicRect= PicFrame;
			InsetRect(PicRect, 10,10);
			/*- ??? invert to collect TRUE frame.. 
			with PicRect do 
				SetRect( PicFrame, right,bottom, left, top);
			---*/
			WITH PicRect){
				xsize = (right-left);  
				ysize = (bottom-top);  
				}
			picXscale= 1.0;  
			picYscale= 1.0; 
			QDstemunit= 8;
			treeXmin= 0.0;
			treeYmin= 0.0;
			treeXmax= 1.0;
			treeYmax= 1.0;
		}  
			
 {  
    plotrparms();
		
		fixedSize= TRUE;
 		fPlotNodeNum= FALSE;
		unitPicScale= 50;  //< useroption
		
	  xmargin = 0.08 * xsize;
    ymargin = 0.08 * ysize;
		nextnode= 0;
		nextnext= 0;

   //-  grows = kVertical;
    fLabelrotation = 45.0;
    nodespace = 3.0;
    stemlength = 0.05;
    treedepth = 0.5/0.95;
  } 



  pascal void FindTreeStyle(void)
  char		VAR key, ch ;   boolean done,
  { 
		//- theStyle= kCladogram; */ /*Default
		done= hcEOF(parmfile);
		
		while ( !done){
			//read parameter key/name
			do { key= hcRead(parmfile); until (key > ' ') or hcEOLn(parmfile);
			//skip rest of parameter name...
			do { ch= hcread(parmfile); until (ch <= ' ') or hcEOLn(parmfile);
	
			uppercase(key);
			if ((key == kStyle)) {
				done= TRUE;
				ch= hcread(parmfile);
				uppercase (ch);
				switch (ch) {
					'C' : theStyle = kCladogram;
					'P' : theStyle = kPhenogram;
					'E' : theStyle = kEurogram;
					'S' : theStyle = kSwoopogram;
					'V' : theStyle = kCurvogram;
					'U' : theStyle = kUnrooted;
					}
				}
			if (hcReadln(parmfile) != '') ;
			done= done || hcEOF(parmfile);
			}
		hcRewind( parmfile);
  } 
	
	
  pascal void getparms(void)
    // get from user the relevant parameters for the plotter and diagram 
  char		VAR key, ch ; 
			Boolean		ok ;  
			x	: real;
  { 
	//rely on treeread to set uselengths
	if (uselengths) nodeposition = kIntermediate
	else nodeposition = kVshaped;
	
	while not hcEOF(parmfile) do begin
	  //read parameter key/name
		repeat key= hcRead(parmfile); until (key > ' ') or hcEOLn(parmfile);
		//skip rest of parameter name...
		repeat 	 ch= hcread(parmfile);
	  until (ch <= ' ') or hcEOLn(parmfile);

		uppercase(key);
    switch (key) {
		
      kGrows: {  //! note change in documentation 
				ch= hcread(parmfile);
				uppercase(ch);
				case ch of
					'H' : grows = kHorizontal;
					'V' : grows = kVertical;
					end();
				}
				
      kUseLengths: {
				ch= hcread(parmfile);
				uppercase (ch);
				if haslengths then CASE ch OF
					'Y': begin uselengths= TRUE;
							 nodeposition= kIntermediate;
							 end();
					'N': begin uselengths= FALSE;
							 nodeposition= kVshaped;
						   end();
				  	end();
        }
				
      kFixedSize: {
				ch= hcread(parmfile);
				uppercase (ch);
				switch (ch) {
					'Y': fixedSize= TRUE;
					'N': fixedSize= FALSE;
					end();
        }
				
			kUnitPicScale	:	 
         begin
				 hcReadReal(parmfile, unitPicScale);
				 end();

      kPlotNodeNum: {
				ch= hcread(parmfile);
				uppercase (ch);
				switch (ch) {
					'Y': fPlotNodeNum= TRUE;
					'N': fPlotNodeNum= FALSE;
					end();
        }

      kRotateLabel: {
				hcREADReal(parmfile, x);
				fLabelrotation= x;
        }
 
		  kTreeDepth: {
        hcReadReal(parmfile, treedepth);
        }
				
      kStemLength: {
				hcReadReal(parmfile, x);
				if (x >= 0.0) && (x < 0.9) then stemlength = x;
				}
				
      kNodeSpace: {
        hcReadReal(parmfile, x);
        if (x != 0.0) then nodespace = 1.0/x;
        }
				
      kNodePosition: {
				ch= hcread(parmfile);
				uppercase(ch);
        switch (ch) {
          'W' : nodeposition = kWeighted;
          'I' : nodeposition = kIntermediate;
          'C' : nodeposition = kCentered;
          'N' : nodeposition = kInner;
          'V' : nodeposition = kVshaped
          }
        }
				
      }
		if (hcReadln(parmfile) != '') ;
		end();
  } // getparms 




  pascal void calculate(void)
    // compute coordinates for tree 
    VAR 
			sum, tipx, maxtextlength, textlength,
			stemdep, stem1dep,
      REAL		firstlet, maxheight, fontheight, angle ;
      short		i ;

    pascal void calctraverse (nodeHandle p , REAL lengthsum )
      // traverse to establish initial node coordinates 
      REAL		VAR x1, y1, x2, y2, x3, w1, w2, sumwx, sumw, nodeheight ;
        nodeHandle		pp, plast ;
    { 
      if (p == NodeHandle(fRoot)) nodeheight = 0.0
      else if (uselengths) nodeheight = lengthsum + (*p)->userlen
 			else nodeheight = 1.0;
      if (nodeheight > maxheight) maxheight = nodeheight;
      if ((*p)->tip) {
        (*p)->xcoord = tipx;
        if (uselengths) (*p)->ycoord = nodeheight
        else (*p)->ycoord = 1.0;
        tipx = tipx + tipspacing;
        }      else {
        sumwx = 0.0;
        sumw = 0.0;
        pp = (*p)->next;
        x3 = 0.0;
        do {
          calctraverse ((*pp)->back, nodeheight);
          sumw = sumw + (*pp)->(*back)->tipsabove;
          sumwx = sumwx + (*pp)->(*back)->tipsabove*(*pp)->(*back)->xcoord;
          if (ABS((*pp)->(*back)->xcoord-0.5) < ABS(x3-0.5)
) x3 = (*pp)->(*back)->xcoord;
          plast = pp;
          pp = (*pp)->next;
        } while (!(pp == p));
        x1 = (*p)->(*next)->(*back)->xcoord;
        x2 = (*plast)->(*back)->xcoord;
        y1 = (*p)->(*next)->(*back)->ycoord;
        y2 = (*plast)->(*back)->ycoord;
        switch (nodeposition) {
          kWeighted 	: {
            w1 = y1 - nodeheight;
            w2 = y2 - nodeheight;
            if ((w1 + w2) <= 0.0
) (*p)->xcoord = (x1 + x2)/2.0
            else (*p)->xcoord = (w2*x1 + w1*x2)/(w1+w2);
            }
          kIntermediate 	: (*p)->xcoord = (x1 + x2)/2.0;
          kCentered 			: (*p)->xcoord = sumwx/sumw;
          kInner 				: (*p)->xcoord = x3;
          kVshaped 			: (*p)->xcoord = (x1 + x2 + (y1 - y2)/maxheight)/2.0
          }
        if (uselengths) (*p)->ycoord = nodeheight
        else {
          (*p)->ycoord = (x1 - x2 + y1 + y2)/2.0;
          if (nodeposition == kInner) {
            if (ABS(x1-0.5) > ABS(x2 - 0.5)) {
              (*p)->ycoord = y1 + x1 - x2;
              w1 = y2 - (*p)->ycoord;
              }            else {
              (*p)->ycoord = y2 + x1 - x2;
              w1 = y1 - (*p)->ycoord;
              }
            if (w1 < epsilon) (*p)->ycoord = (*p)->ycoord - ABS(x1-x2);
            }
          }
        }
    } // traverse 

		pascal lengthtext(plotstring pstring , nchars : short) : REAL;
			 short		VAR i, j, code ;
					REAL		cfix, sumlength, heightfont, widthfont ;
		{  
			 sumlength = 0.0;
			 FOR i = 1 TO nchars){
					widthfont=  charwidth(pstring[i]);
					sumlength = sumlength + widthfont;
					}
			 lengthtext = sumlength;
		} // lengthtext 


	pascal void getTextLength( nodeHandle aNode)
	{
		if ((*aNode)->tip) {
			firstlet   = lengthtext ((*aNode)->nayme, 1);
			textlength = lengthtext ((*aNode)->nayme, length((*aNode)->nayme));
			if (textlength > maxtextlength) maxtextlength = textlength;
			}
	}

	pascal void setYcoord( nodeHandle aNode)
	{
		(*aNode)->ycoord = stemdep + stem1dep * ((*aNode)->ycoord - fRooty);
	}

  { // calculate 
    maxheight = 0.0;
    maxtextlength = 0.0;
    sum = 0.0;
    tipx = 0.0;
		ForEachNodeDo( getTextLength);
		
    fontheight = QDfontHeight;
    angle = pi*fLabelrotation/180.0;
    maxtextlength = maxtextlength/fontheight;
    textlength = textlength/fontheight;
    firstlet = firstlet/fontheight;
    if (ntips > 1) fLabelheight = 1.0/(nodespace*(ntips-1))
    						 else fLabelheight = 1.0/nodespace;
    if (angle < (pi/6.0)) tipspacing = (nodespace
                  + COS(angle)*(maxtextlength-0.5))*fLabelheight
    else if (ntips > 1) tipspacing = 1.0/(ntips-1.0)
  	else tipspacing = 1.0;
    topoflabels = fLabelheight*(1.0 + SIN(angle)*(maxtextlength-0.5)
                          + COS(angle)*0.5);
    rightoflabels = fLabelheight*(COS(angle)*(textlength-0.5) +SIN(angle)*0.5);
    leftoflabels = fLabelheight*(COS(angle)*firstlet*0.5+SIN(angle)*0.5);
    calctraverse (NodeHandle(fRoot), sum);
		
    fRooty = (*NodeHandle(fRoot))->ycoord;
		stemdep	= stemlength*treedepth;
		stem1dep= ((1.0-stemlength)*treedepth) / (maxheight-fRooty);
		ForEachNodeDo( setYcoord);
    fRooty = 0.0;
  } // calculate 


	procedure findPicScale;
	var
			integer		fhigh, wmax, dmax, h, v, i, j;
			firstNode,firstTip 	: boolean;
			theRect, textFrame	: rect;

		pascal void findRange( nodeHandle aNode)
		{
			if (firstNode) {
				treeXmax= (*aNode)->xcoord; treeXmin= treeXmax;
				treeYmax= (*aNode)->ycoord; treeYmin= treeYmax;
				firstNode= FALSE;
				}			else with (*aNode)^ do begin
				if xcoord > treeXmax then treeXmax= xcoord
				  else if xcoord < treeXmin then treeXmin= xcoord;
				if ycoord > treeYmax then treeYmax= ycoord
				  else if ycoord < treeYmin then treeYmin= ycoord;
				}
				
			if ((*aNode)->tip) {
				locateStr( h, v, round(fLabelrotation), (*aNode)->nayme, theRect);
				if (firstTip) { textFrame= theRect; firstTip= FALSE; }				else UnionRect( textFrame, theRect, textFrame);
				if (grows == kVertical) h= textFrame.right+2 
				else v= textFrame.bottom+2;
				WITH theRect){
					wmax= max(wmax, right-left);
					dmax= max(dmax, bottom-top);
					}
				}
		}
			
	{
		firstTip = TRUE; 
		firstNode= TRUE;
		wmax= 0; dmax= 0; 
		h= 1; v= 1;
		ForEachNodeDo( findRange);
			
		if (FixedSize) { //set always TRUE ?!
			/*---
			picXScale= unitPicScale;
			picYscale= picXScale;
			---*/
			fhigh= QDfontHeight + 5;
			if (grows == kVertical) {
				WITH textFrame)OffsetRect( PicRect, 0, top);  
				picXScale= (wmax+5)/*fhigh*/ / tipspacing;
				picYscale= QDstemunit / stemlength;
				}			else {
				picXScale= QDstemunit / stemlength;
				picYscale= (dmax+5)/*fhigh*/ / tipspacing;
				}
			}			
		else {
			if (grows == kVertical) {
				WITH textFrame)OffsetRect( PicRect, 0, top); 
				}
			picXscale= 1.0;
			picYscale= picXscale;
			}
	}
	

  pascal void rescale(void)
    // compute coordinates of tree for plot or preview device 
    short		VAR i ;
      REAL		treeheight, treewidth, extrax, extray, temp ;
			
		pascal void scaleXY( nodeHandle aNode)
		{
			(*aNode)->xcoord = fExpand*((*aNode)->xcoord + leftoflabels);
			(*aNode)->ycoord = fExpand*((*aNode)->ycoord - fRooty);
      if (grows == kHorizontal) {
        temp = (*aNode)->ycoord;
        (*aNode)->ycoord = fExpand*treewidth-(*aNode)->xcoord;
        (*aNode)->xcoord = temp;
        }
      (*aNode)->xcoord = (*aNode)->xcoord + xmargin + extrax;
      (*aNode)->ycoord = (*aNode)->ycoord + ymargin + extray;
		}
		
		pascal void findTreeMax( nodeHandle aNode)
		{
			if ((*aNode)->ycoord > treeheight) treeheight = (*aNode)->ycoord;
		}

  { 
		treeheight = 0.0;
		ForEachNodeDo(findTreeMax);
		treeheight = treeheight + topoflabels;
		treewidth  = (ntips-1)*tipspacing + rightoflabels + leftoflabels;
		
    if (grows == kVertical) {  //! drop all of this !?
      fExpand = (xsize - 2*xmargin)/treewidth;
      if ((ysize - 2*ymargin)/treeheight < fExpand) 
				fExpand = (ysize - 2*ymargin)/treeheight;
      extrax = (xsize - 2*xmargin - treewidth*fExpand)/2.0;
      extray = (ysize - 2*ymargin - treeheight*fExpand)/2.0;
      }    else {
      fExpand = (ysize - 2*ymargin)/treewidth;
      if ((xsize - 2*xmargin)/treeheight < fExpand) 
				fExpand = (xsize - 2*xmargin)/treeheight;
      extrax = (xsize - 2*xmargin - treeheight*fExpand)/2.0;
      extray = (ysize - 2*ymargin - treewidth*fExpand)/2.0;
      }
			
		if (fixedSize) { //! always !?
			fExpand= 1.0;
			extrax= 0.0;
			extray= 0.0;
			}
			
		ForEachNodeDo(scaleXY);
			
    if (grows == kVertical) fRooty = ymargin + extray
    								     else fRooty = xmargin + extrax;
	  findPicScale();   //!!!!
  } // rescale 
 
{ 
	PicFrame= frame;
	FindTreeStyle();
	treeread();
	initialparms();
	getparms();
	calculate();
	rescale();
} //calcGram

VAR
	fi	: fontInfo;
	hcFile		hcTree, hcParams;
	IDType		nodeID;
	Rect		frame;
	integer		grow, nodeposition;
	boolean		fixedsize, uselengths;
{
//-	SetPortTextStyle(gPrintNameStyle); < times-italic 
	SetPortTextStyle(gPrintNucStyle); //< helvetica ?

	if ((fRoot!=NULL)) FreeData;
	
	if (fStylePop == NULL) fStyle= kDefaultTreeStyle
	else fStyle= fStylePop->GetCurrentItem();  
	
	if (fHoriz == NULL) grow= kHorizontal
	else if (fHoriz.IsOn then grow= kHorizontal
	else grow= kVertical;

	nodeId= fNodeCluster->GetCurrentChoice();
	if nodeID == 'nInt') nodeposition= kIntermediate
	else if nodeID == 'nWgt' then nodeposition= kWeighted
	else if nodeID == 'nCen' then nodeposition= kCentered
	else if nodeID == 'nInn' then nodeposition= kInner
	else if nodeID == 'nVsh' then nodeposition= kVshaped
	else nodeposition= kIntermediate;

	fixedSize= fFixed->IsOn();
	uselengths= fNodeLen->IsOn();

	hcOpen(hcTree, fTreeData);
	hcOpen(hcParams, fParams);
	this->GetQDExtent( frame);
	InsetRect( frame, 25, 50);

	CalcGram( frame, fStyle, grow, nodeposition, fixedsize, uselengths,
						hcTree, hcParams);
}



pascal void TTreePrintView::Draw(VRect area)	
VAR
		integer		grows;
		
	pascal void DrawGram(void)
	CONST 
	 //- maxnch == 30; 
		//- pi == 3.141592653; 
		epsilon == 0.00001;
		pendown == TRUE;
		penup		== FALSE;

  pascal void plottree (nodeHandle p, q )
	// plot part or all of tree on the plotting device 
	CONST 
			segments == 40;
	VAR 
			REAL		x1, y1, x2, y2, x3, y3, f, g, h, fract, minny, miny ;
			nodeHandle		pp ;

		pascal void plot(boolean penIsDown, REAL xabs, yabs ) 
		integer		VAR  h, v;
		{  
			map2pic( xabs, yabs, h, v);
			if (penIsDown) LineTo( h,v)
			else MoveTo( h,v);
		}  

		pascal void SwoopoMe(void)
		integer		VAR  i;
		{
			if ((grows == kVertical) && (!(ABS(y1-y2) < epsilon))
			|| ((grows == kHorizontal) && (!(ABS(x1-x2) < epsilon))) 
) {
				if (grows == kVertical) miny = (*p)->ycoord
														else miny = (*p)->xcoord;
				pp = (*q)->next;
				while ( pp != q){
					if (grows == kVertical) minny = (*pp)->(*back)->ycoord
															else minny = (*pp)->(*back)->xcoord;
					if (minny < miny) miny = minny;
					pp = (*pp)->next;
					}
				if (grows == kVertical) fract = 0.3333*(miny-y1)/(y2-y1)
					else fract = 0.3333*(miny-x1)/(x2-x1);
				FOR i = 1 TO segments){
					f = i/segments;
					if (f < fract) g = f/fract
						else g = (f-fract)/(1.0-fract);
					if (f < fract) h = fract*SQRT(1.0-(1.0-g)*(1.0-g))
						else h = fract + (1.0-fract)*(1.000001 - SQRT(1.000001-g*g));
					if (grows == kVertical) {
						x3 = x1*(1.0-f)+x2*f;
						y3 = y1 + (y2 - y1)*h;
						}					else {
						x3 = x1 + (x2 - x1)*h;
						y3 = y1*(1.0-f)+y2*f;
						}
					plot (pendown, x3, y3);
					}
				}
		} //swoopoMe
			
		pascal void curvoMe(void)
		integer		VAR i;
		{
			FOR i = 1 TO segments){
				f = i/segments;
				g = i/segments;
				h = 1.0 - SQRT(1.0-g*g);
				if (grows == kVertical
) {
					x3 = x1*(1.0-f)+x2*f;
					y3 = y1 + (y2 - y1)*h;
					}				else {
					x3 = x1 + (x2 - x1)*h;
					y3 = y1*(1.0-f)+y2*f;
					}
				plot (pendown, x3, y3);
				}
		}

  {  
    x2 = ( (*p)->xcoord);
    y2 = ( (*p)->ycoord);
    if (p != NodeHandle(fRoot)) {
      x1 = ( (*q)->xcoord);
      y1 = ( (*q)->ycoord);
      plot (penup, x1, y1);
			
      switch (fStyle) {
			
				kUnrooted,
        kCladogram 	: plot (pendown, x2, y2);
				
        kPhenogram 	: {
          if (grows == kVertical) plot (pendown, x2, y1)
     			else plot (pendown, x1, y2);
          plot (pendown, x2, y2);
          }
					
        kEurogram 		: {
          if (grows == kVertical) plot (pendown, x2, (2*y1+y2)/3)
          else plot (pendown, (2*x1+x2)/3, y2);
          plot (pendown, x2,y2);
          }
					
        kCurvogram 	: curvoMe;
        kSwoopogram 	: SwoopoMe;
        }
      }			
    else if ((fStyle != kUnrooted)) {
      if (grows == kVertical) {
        x1 = ( (*p)->xcoord);
        y1 = ( fRooty);
        }      else {
        x1 = ( fRooty);
        y1 = ( (*p)->ycoord);
        }
      plot (penup, x1, y1);
      plot (pendown, x2, y2);
      }
			
    if (!(*p)->tip) {
      pp = (*p)->next;
      while ( pp != p){
        plottree ((*pp)->back, p);
        pp = (*pp)->next;
        }
      }
  } // plottree 

		
  pascal void plotlabels(void)
	short		VAR i ;
     	REAL		dx, dy, angle ;
      integer		nodenum;
			
		pascal void plottext(plotstring pstring ,  REAL x, y, rotate )
		integer		VAR   h, v;
		{  
			map2pic( x, y, h, v);
			textPicDraw( h, v, round(rotate), pstring);  
			//- UnionRect( PicFrame, gTextRect, PicFrame); << gTextRect supposed to come from textPicDraw
		}  
	
		pascal void PlotNodeLabel( nodeHandle aNode)
		VAR	
				str255		nums;  
				integer		h, v;
		{
			nodenum= nodenum+1;
      if ((*aNode)->tip) {
				dx= 0; dy= 0;  //?
        dx = -fLabelheight*fExpand*0.70710*COS(angle+pi/4.0);
        dy =  fLabelheight*fExpand*(1.0-0.70710*SIN(angle+pi/4.0));
        if (grows == kVertical) plottext((*aNode)->nayme,
         		((*aNode)->xcoord+dx),  ((*aNode)->ycoord+dy), fLabelrotation)
        else plottext((*aNode)->nayme, 
						((*aNode)->xcoord+dy), ((*aNode)->ycoord-dx), fLabelrotation);
				}				
			else if (fPlotNodeNum) {   
				numtostring( nodenum, nums);
				map2pic( (*aNode)->xcoord, (*aNode)->ycoord, h, v);
				moveto(h,v); 
				drawstring( nums);			
				}
		}
	
  {  
		nodenum= 0;
    angle = fLabelrotation*pi/180.0;
		ForEachNodeDo( PlotNodeLabel);
  } // plotlabels 

	{  
		PicFirst= TRUE;
		PlotTree (NodeHandle(fRoot), NodeHandle(fRoot));
		PlotLabels();		
	}

{
	inherited::Draw(area);
//-	SetPortTextStyle(gPrintNameStyle); < times-italic 
	SetPortTextStyle(gPrintNucStyle); //< helvetica ?

	if (fHoriz == NULL) grows= kHorizontal
	else if (fHoriz.IsOn then grows= kHorizontal
	else grows= kVertical;

	beginPicGroup(); 
	DrawGram();
	endPicGroup();
}



pascal void TTreePrintView::CalcMinFrame(VRect VAR minFrame)
VAR
	integer		nb;
	TScroller		aScroller;
{
	inherited::CalcMinFrame( minFrame);
	
	/******
	if ((fAlnList!=NULL) && (fAlnList.GetSize>0)) {
		if (fNbases == 0) then nb= TSequence(fAlnList->First()).fLength 
		else nb= fNbases;
		//this is a kludge...
		minsize.h= max(minsize.h, (15+min(40,nb))*12);
	 	minSize.v= max(minsize.v, (2 + (nb / 40)) * (fAlnList.GetSize * 14 + kParagraph));
		end();
	minSize.h= max(minSize.h, fMinSize.h);
	minSize.v= max(minSize.v, fMinSize.v);
	aScroller= GetScroller(TRUE);
	if ((ascroller!=NULL)) aScroller->SetScrollLimits(minSize, kDontRedraw);
	*******/
}




pascal void TTreePrintView::DoMenuCommand(CommandNumber aCommandNumber) // override 
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
		
/*---------	
pascal void TTreePrintView::DoMenuCommand(CommandNumber aCommandNumber) // override 
{
VAR
		short		menu, item;
		MCEntryPtr		pMCEntry;
		RGBColor		theColor;
		StringHandle		pickerPrompt;
{
	switch (aCommandNumber) {
		cCut, cCopy:
			{
			New(TreePrintCutCopyCommand);
			FailNil(TreePrintCutCopyCommand);
			TreePrintCutCopyCommand->ITreePrintCutCopyCommand(aCommandNumber, this);
			DoMenuCommand = TreePrintCutCopyCommand;
			}

		case cPaste:
			{
			New(TreePrintPasteCommand);
			FailNil(TreePrintPasteCommand);
			TreePrintPasteCommand->ITreePrintPasteCommand(this);
			DoMenuCommand = TreePrintPasteCommand;
			}

		case cClear:
			{
			New(TreePrintClearCommand);
			FailNil(TreePrintClearCommand);
			TreePrintClearCommand->ITreePrintClearCommand(this);
			DoMenuCommand = TreePrintClearCommand;
			}

		default:
			{
			CommandToMenuItem(aCommandNumber, menu, item);
			if (menu == mColor) {
				New(recolorCmd);
				FailNil(recolorCmd);
				pMCEntry = GetMCEntry(menu, item);
				theColor = pMCEntry->mctRGB2;		 
				recolorCmd->IRecolorCmd(theColor, this);
				DoMenuCommand = recolorCmd;
				}			else
				inherited::DoMenuCommand(aCommandNumber);
			}
	}								 
}
------*/


/*******
pascal TCommand TTreePrintView::DoMouseCommand(Point VAR theMouse, EventInfo VAR info,
								   Point VAR hysteresis)
VAR
		FailInfo		fi;

	pascal void HdlInitCmdFailed(OSErr error, long message)
		{
		FreeIfObject(protoTreePrint);
		protoTreePrint = NULL;
		}

	{					

	fClickPt = theMouse;
	if (palette->fCurrTreePrint > 0) { // draw mode
		FailSpaceIsLow(); 								// Make sure we aren't low on memory 

		Deselect();

		//Clone appropriate TreePrint

		protoTreePrint = TTreePrint(gTreePrintsArray[palette->fCurrTreePrint].Clone);
		FailNil(protoTreePrint);

		CatchFailures(fi, HdlInitCmdFailed);
		// Make sure cloning the TreePrint left us with enough memory to continue.
		FailSpaceIsLow();

		New(TreePrintSketcher);
		FailNil(TreePrintSketcher);
		TreePrintSketcher->ITreePrintSketcher(this, protoTreePrint, info.theOptionKey);
		Success(fi);
		DoMouseCommand = TreePrintSketcher;
		}			
		
	else {	 	//select mode
		TreePrintUnderMouse = NULL;
		fTreePrintDocument->EachVirtualTreePrintDo(CheckTreePrint);

		if (TreePrintUnderMouse == NULL)	{		//area select
			if (!info.theShiftKey)
				Deselect();
			New(TreePrintSelector);
			FailNil(TreePrintSelector);
			TreePrintSelector->ITreePrintSelector(cMouseCommand, this);
			DoMouseCommand = TreePrintSelector;
			} 

		else {										//TreePrint select/move/...
			if (!(TreePrintUnderMouse->fIsSelected || info.theShiftKey))
				Deselect();

			if (info.theShiftKey) {
				TreePrintUnderMouse->fIsSelected = !TreePrintUnderMouse->fIsSelected;
				if (TreePrintUnderMouse->fIsSelected)
					TreePrintUnderMouse->Highlight(hlOff, hlOn)
				else
					TreePrintUnderMouse->Highlight(hlOn, hlOff);
				}			else if (!TreePrintUnderMouse->fIsSelected)
				{
				TreePrintUnderMouse->fIsSelected = TRUE;
				DoHighlightSelection(hlOff, hlOn);
				}

			if (TreePrintUnderMouse->fIsSelected) {
				New(TreePrintDragger);
				FailNil(TreePrintDragger);
				TreePrintDragger->ITreePrintDragger(this);
				DoMouseCommand = TreePrintDragger;
				}
			//else, fall-through, we return NULL
			}	 
		}					 
}										
**********/



pascal void TTreePrintView::DoSetupMenus(void)
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

