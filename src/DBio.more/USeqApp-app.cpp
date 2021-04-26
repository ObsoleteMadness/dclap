// USeqApp.app.p 
// d.g.gilbert, 1991 


#ifndef __Picker__
#include <Picker.h>
#endif

#ifndef __UDrawingEnvironment__
#include <UDrawingEnvironment.h>
#endif
USES
	UFloatWindow();

CONST
		
			// when do I died?
	expireYear 	== 1994;
	expireMonth == 7; //jan=1
	expireDay		== 1;	

	kAppPrefsFile == 'SeqApp Prefs';
	kHelpFile == 'SeqApp.Help';
	kHelpFileID	== 2402;
	
	cNewText  == 11;	 //new plain text
	cNewProj	== 12;	 //new project list
	cNewDebug == 14;
	cNewGopher == 15;

	cOpenText	== 21;	 //Open plain text
	cOpenProj	== 22;	 //Open project list
	cOpenHelp	== 24;
	cOpenGopher == 25;
	cOpenTree == 26;
	cOpenAAColors == 27;
	cOpenNAColors == 28;

	mSequence	== 20;   //sequence menu
	cNewSeq 	== 2001;
	cEditSeq	== 2002;
	cAlnSeqs	== 2003;
	cAutoAlign= 2004;
	cPrettyPrint= 2005;
	cDottyPlot= 2015;
	cHelpCommand=4321;
	
	//UText has reserved c1000..c1999, hasn't yet used c1900...
	//use these nums for menu buzz words
	cCutSeq				== 1903;
	cCopySeq			== 1904;
	cPasteSeq 		== 1905;
	cClearSeq 		== 1906;
	cCutText			== 1913;
	cCopyText			== 1914;
	cClearText		== 1916;
	cStandardCut	== 1923;
	cStandardCopy	== 1924;
	cStandardClear= 1926;
	
	cReverseSeq		== 2006;
	cComplementSeq == 2007;
	cTranslateSeq == 2008;
	cDna2Rna == 2009;
	cRna2Dna == 2010;
	cSeqChange == 2000;
	cRestrictMap	== 2011;
	cConsensus == 2012;
	cCompress == 2013;
	cRevComplement == 2014;
	cLockIndels		==	2016;
	cUnlockIndels	== 2017;
	cSaveSelection= 2018;
	
	cIUBcodes	== 2402;
	cAAcodes	== 2403;
	cPrefsDialog == 2404;
	
	mChildren == 30;
	cAddChildCmd == 3001;
	cRemoveChildCmd == 3002;
	cChildDied == 2999;
	//-	kNewChildTemplateID == 3001;
	
	cCheckMail=	2102;
	cSendMail	==	2103;
	cFileTransfer	== 2104;
	cGBFetch	== 2105;
	cBugMail	== 2106;
	cEMBLFetch	== 2107;
	cMailPref == 2108;

	cGBFasta == 2110;
	cGBSearch	==	cGBFasta;
	cGBBlast	== 2111;
	cGeneidSearch == 2112;
	cGrailSearch == 2113;
	cUHServer == 2114;
	cGenmarkSearch == 2118;
	cBlocksSearch == 2119;
	cPythiaSearch == 2120;

	cGopher == 2201;
	cGopherPrefs == 2203;
	
	kIUBcodeWindID == 2402;
	kAAcodeWindID == 2403;
	kPrefsDialogID == 2404;

	kEditOnly == TRUE;
	prefMyMailAddress == 100;
	kNoAddress == 'put.your&internet.address.here';

	kMacdrawHeaderSize == 512;
	
	msgMyError == 0x80001234;

	kSeqEdWindID		== 1006; 				// seq1ed 'view' template for a window 
	// kSeqEdDocID			== 1009;  unused, seq1ed 'view' template for a window 
	kAlnViewID 			== 1009;
	kAlnWindID			== 1010; 				// aln 'view' template for a window 
	kAlnTEViewID 		== 1011;					// aln 'teview' template
	kAlnAColorID		== 1012;					// A color 
	kAlnCcolorID		== 1013;
	kAlnGcolorID		== 1014;
	kAlnTcolorID		== 1015;
	kAlnNcolorID		== 1016;

	kGray50ID		== 1026;			 
	kGray25ID		== 1027;			 
	kGray75ID		== 1028;			 
	
	
	kPrintNucStyle	== 1023;
	kPrintNameStyle	== 1024;
	kPrintZymeStyle == 1025;
	kPrintClipType == 'PICT';

	kColorIconID		== 1001;
	kMonoIconID			== 1002;
	kLockIconID 		== 1010;
	kUnlockIconID		== 1011;
	
	kSeqFileType		== 'TEXT';				// uses standard text files 
	kSeqedViewType 	== 'TEVW';
	kAlnViewType	 	== 'ALNV';
	kAlnScrapType 	==  kAlnViewType;			// Scrap == TAlnView type 
	
	kMovableModalWindProc == 5;  //wind manager proc id for movable modal window 
	
	kHierDisplayedMBar	== 131;							// Displayed menus on hier. menu system 
	kNonHierDisplayedMBar == 128;						// Displayed menus on non-hier. system 

	kHierMenuOffset 	== 1000; 						/* Offset added to non-hier menu cmds to get
														  the right number */
	
	// The 'File is too large' alert 
	kFileTooBig 		== 1000;

	// Constants for the text specs resource 
	kTextSpecsRsrcType	== 'SPEC';
	kTextSpecsRsrcID	== 1;

	// Constants for the text style resource 
	kTextStyleRsrcType	== 'STYL';     //!not same as 'styl' == STScrapHandle!
	kStylesRsrcID		== 1;
	kElementsRsrcID 	== 2;

	kNoAction	== 0;
	kOpenSeqFile	== 1;
	kOpenTextFile	== 2;
	kOpenFile	== 3;
 
	
TYPE

		TIUBWindow == OBJECT(TPrefWindow)
			pascal void TIUBWindow::SetPrefID(void) // override 
			}
			
		TAAWindow == OBJECT(TPrefWindow)
			pascal void TAAWindow::SetPrefID(void) // override 
			}


CONST
	kAAColorView == 1051;
	kNAColorView == 1050;
	kAAColorType == 'AClr';
	kNAColorType == 'NClr';
	
TYPE

	colorsArray == array [' '..'~'] of RGBColor;
	IDsArray == array [' '..'~'] of OSType;

	TColorPrefWind			== OBJECT (TWindow) //? TFloatWindow or plain  ?
		pascal void TColorPrefWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
		}
							
	TColorText	== OBJECT(TStaticText)
		pascal void TColorText::InstallColor(RGBColor theColor, Boolean redraw) // override 
		}

	TColorPrefDocument		== OBJECT (TFileBasedDocument)
		fBaseKind	: integer;
		fWindID		: integer;
		fColorPrefWind	: TColorPrefWind;
		fSwapBackground	: TCheckBox;
		fColors		: colorsArray;
		fViewIDs	: IDsArray;
		Boolean		fDoSetGlobals;
		fSwapBack	: Boolean;
		Str255		fDescription;
		
		pascal void TColorPrefDocument::IColorPrefDocument( TFile itsFile, Integer baseKind)
		pascal void TColorPrefDocument::Free(void) // override 
		pascal void TColorPrefDocument::FreeData(void) // override 
		pascal void TColorPrefDocument::Close(void) /* override */ 

		pascal void TColorPrefDocument::SetGlobals(void) 
		pascal void TColorPrefDocument::DoHitLetter(TStaticText theLet)
		pascal void TColorPrefDocument::DoMakeViews(Boolean forPrinting) // override 
		pascal void TColorPrefDocument::UntitledName(Str255 VAR noName) // override 
		pascal TFile TColorPrefDocument::DoMakeFile(CommandNumber itsCommandNumber) // override 
		pascal void TColorPrefDocument::DoNeedDiskSpace(TFile itsFile,
															long VAR dataForkBytes, rsrcForkBytes) // override 
		pascal void TColorPrefDocument::DoRead(aFile:TFile; Boolean forPrinting) // override 
		pascal void TColorPrefDocument::DoWrite(TFile aFile, Boolean makingCopy) // override 
		}
	
VAR
	gNoColor	: RGBColor;
	gNoViewID	: OSType;
	colorsArray		gAAcolors, gNAcolors;
	Boolean		gSwapBackground;
	
		

VAR 
	gDefWordBreak			: procPtr;	
	gDefTEDrawHook		: procPtr;
	gDefTEHitTestHook	: procPtr;
	
	gAlnStyle			: TextStyle;
	integer		gAlnCharWidth ;
	gMachineType	: Integer; 
	gGoodMac				: Boolean; //temp test Mac SE bombs 
	
//-	gChildList 		: TList; 

	gActiveAlnDoc	: TAlnDoc;
	Handle		gLockIcon, gUnLockIcon;  
	gHelpFile		: StringHandle;


 


#pragma segment AInit


pascal void TSeqAppApplication::DoAppleCommand( CommandNumber aCommandNumber,
												 AppleEvent message, reply) // override 
{	
			inherited::DoAppleCommand(aCommandNumber, message, reply);
/*------
	switch (aCommandNumber) {
		case cChildDied:  
			if ((noErr != HandleChildDiedEvent( message, reply, aCommandNumber))) ;
			
		Otherwise
			inherited::DoAppleCommand(aCommandNumber, message, reply);
			 
		}
-----*/
}

pascal void TSeqAppApplication::DoAboutBox(void) // override 
/* Method to display the "About" box for your application.  Override to do interesting things.
Since it is normally called from a command; the app usually has the maximum free space available. */
CONST
		kAboutID == 1000;

VAR
	apRefnum, indx, len, saverez	: integer;
	apParam	: Handle;
	aWind	: TWindow;
	aDies, aVers	: TStaticText;
	IDType		dismisser ;
	aStr	: Str255;
	aDay	: DateTimeRec;
	secs	: longint;
	VersRecHndl		appVers;
{
	FailSpaceIsLow();
	GetAppParms(aStr, apRefnum, apParam);
	ParamText(aStr, '', '', '');

	aWind = TWindow(gViewServer->NewTemplateWindow(kAboutID, NULL));
	FailNIL(aWind); 		 
	aDies = TStaticText( aWind->FindSubView('DIES'));
	if (aDies!=NULL) {
		aDay.year= expireYear;
		aDay.month= expireMonth;
		aDay.day= expireDay;
		aDay.hour= 1;
		aDay.minute= 1;
		aDay.second= 1;
		Date2Secs( aDay, secs);
		IUDateString( secs, abbrevDate, aStr);
		aDies->SetText( aStr, kDontRedraw);
		}

	//- pascal void WithApplicationResFileDo(pascal void DoWithResFile); << global proc in TApplication 
	
	aVers = TStaticText( aWind->FindSubView('vers'));
	if (aVers!=NULL) {
		saverez= CurResFile;
		UseResFile(gApplicationRefNum);  
		Handle(appVers)= GetResource('vers', 1);  //!this pulls from Prefs file if open
		UseResFile(saverez);  
		if (appVers!=NULL)) aVers->SetText((*appVers)->shortVersion, kDontRedraw);
		/*--- ?? pull long vers string from record 
		indx= ord((*appVers)->shortVersion[0]) + 1; 
		len= (*appVers)->shortVersion[indx];
		BlockMove( &(*appVers)->shortVersion[indx], &aStr[0], len);
		if (appVers!=NULL)) aVers->SetText(aStr, kDontRedraw);
		---*/
		}	
	
	/*---
	aWind->ShowReverted();   
	aWind->Open();  
	AddWindow( aWind);
	---*/
	aWind->SetModality( True); //!!! or PoseModally will fail !
	dismisser= aWind->PoseModally();
	aWind->CloseAndFree(); 
}

pascal void TSeqAppApplication::InstallHelpMenuItems(void) // override 
{
	//  // gMenuBarManager->AddHelpMenuItem( "^0 Help", kYourHelpCommandNumber ); 
	gMenuBarManager->AddHelpMenuItem( '^0 Help', cHelpCommand);
}


pascal void TSeqAppApplication::ISeqAppApplication(void)
VAR
	line		:	Str255;
	aTEView	:	TTEView;
	TextStyle		aTextStyle;
	aColor	: RGBColor;
	hTest		: Handle;
	i, dirId, lnum	: longint;
	err	: OSErr;
	ch	: char;
	
	procedure getAlnStyle( RGBColor var aColor, TextStyle var aTextStyle, integer rsrcID)
	TTEView		var  aTEView;
			TextStyle		SaveStyle;
	begin
		MAGetTextStyle(rsrcID, aTextStyle); //Mapp3
		aColor= aTextStyle.tsColor;
		/*----
		aTEView = TTEView(gViewServer->DoCreateViews(NULL, NIL, rsrcID, gZeroVPt));
		FailNIL(aTEView);
		aColor= aTEView->fTextStyle.tsColor;
	  aTextStyle = aTEView->fTextStyle;
		aTEView->Free();
		----*/
	end();
	
	pascal void ListZyme( TREnzyme aZyme)
	str255		VAR  cuts, cut3s;
	{
		NumToString( aZyme->fCutpoint, cuts);
		NumToString( aZyme->fCut3from5, cut3s);
		DebugMsg(concat( aZyme->fName, '  ',cuts,' ',aZyme->fSite, ' ', cut3s, 
								' ! >',aZyme->fVendors));
	}
	

	pascal void Expire(void)
		/* If date > expireDate then 
				a) change self.name to "Expired"+self.name (may fail if locked)
				b) change file type from 'APPL' to '????'  (ditto)
				c) exitToShell
			--------*/
		/* MacApp alert 129:
			Could not complete the "^2" command because ^0. ^1
			MacApp alert 130:  <<<<<<
			Could not complete your request because ^0. ^1
		*/
	VAR
		apRefnum, err	: integer;
		apParam		: Handle;
		apName		: Str255;
		today			: DateTimeRec;
		fndrInfo	: fInfo;
		dead, realdead			: Boolean;
	{
		GetTime( today);
		dead= (today.year > expireYear);
		dead= dead || ((today.year == expireYear) && (today.month > expireMonth));
		realdead= dead;
		dead= dead || ((today.year == expireYear) && (today.month == expireMonth) 
								&& (today.day >= expireDay));
		
		if (dead) {
			GetAppParms(apName, apRefnum, apParam);
			err= GetFInfo( apName, 0, fndrInfo);
			if (realdead)) {
				fndrInfo.fdType= '????';
				err= SetFInfo( apName, 0, fndrInfo);
				err= Rename( apName, 0, concat('Expired ',apName));
				}
			ParamText(
				concat('this release of ',apName,' has expired'),
				'Read docs or e-mail to SeqApp&Bio.Indiana.Edu.',
										'','');	
			err= Alert( 130, NULL); //<< this pulls us out of this proc??.
			if (realdead)) 
				ExitToShell();
			}
	}
	
{

	Expire();
	
	if (gHasGestalt && (Gestalt( gestaltMachineType, lnum) == noErr))
		gMachineType= lnum
	else 
		gMachineType= 0;
	gGoodMac= !(gMachineType IN 
		[gestaltClassic,gestaltMacXL,gestaltMac512KE,gestaltMacPlus,gestaltMacSE]);
	
	//- gConfiguration.hasColorQD= FALSE; 
		
	gDebugDoc= NULL;
	gActiveAlnDoc= NULL;	// this one is obsolete?
	gTextDocSig = kSAppSig; //??

	/*---
	if (!gGoodMac) IPrefApplication(kSeqFileType,'¥¥BadMac¥¥') else 
	---*/
	IPrefApplication(kSeqFileType,kSAppSig,kAppPrefsFile);
	
	gHelpFile= GetString(kHelpFileID);
  if (gHelpFile=NULL) gHelpFile= NewString(kHelpFile);
		
	InitUFloatWindow(); //??
	
	InitTextGlobals();
	InitializeTCP();
	InitializeMailPrefs();	
	InitializeUGopher();   	// reads in icons, ...
	InitializeGopher();
	gGopherDocSig= kSAppSig;	// we want to own our gopher folders....
	InitUSequence(); 				//<< This reads a codon data file !?

/**** not for Mapp3 
  THandleHighlevelEvents		aHandleHighlevelEvents;  
	if (gHasAppleEvents) {
		fLaunchWithNewDocument = FALSE;  //< turn off this damn nuisance 
		NEW(aHandleHighlevelEvents);
		aHandleHighlevelEvents->IEventHandler(NULL);
		this->InstallCohandler( aHandleHighlevelEvents, TRUE);
		}
******/		
	FailOSErr(AEInstallEventHandler( kCoreEventClass, kAEApplicationDied, 
								EventHandlerProcPtr(StripLong(&HandleChildDiedEvent)), 0, FALSE));
	//^^ see DoAppleCommand instead 

		
	InitChildApps( gHasAppleEvents);		/* Must call After IPrefApplication*/	

	//! DON'T USE -- Cause BOMB, MUST fetch icon hand as used?
	if (gConfiguration.hasColorQD then begin
		gLockIcon= Handle( GetCIcon( kLockIconID));
		gUnlockIcon= Handle( GetCIcon( kUnLockIconID));
		}	else {
		gLockIcon= Handle( GetIcon( kLockIconID));
		gUnlockIcon= Handle( GetIcon( kUnLockIconID));
		}
	
	/*set alnseq colors from a view !*/	
	
	getAlnStyle( gAcolor, gAlnStyle, kAlnAColorID);  //globals are in USeqPrint
	getAlnStyle( gCcolor, gAlnStyle, kAlnCColorID);
	getAlnStyle( gGcolor, gAlnStyle, kAlnGColorID);
	getAlnStyle( gTcolor, aTextStyle, kAlnTColorID);
	getAlnStyle( gNcolor, aTextStyle, kAlnNColorID);

	gNAcolors[' ']= gRGBWhite;
	for ch= '!' to '~' do gNAcolors[ch]= gNcolor;
	gNAcolors['A']= gAcolor;
	gNAcolors['C']= gCcolor;
	gNAcolors['G']= gGcolor;
	gNAcolors['T']= gTcolor;
	gNAcolors['U']= gTcolor;
	gNAcolors['a']= gAcolor;
	gNAcolors['c']= gCcolor;
	gNAcolors['g']= gGcolor;
	gNAcolors['t']= gTcolor;
	gNAcolors['u']= gTcolor;
	BlockMove( &gNAcolors, &gAAcolors, sizeof(colorsArray));
	gSwapBackground= False;
	
	getAlnStyle( gGray50, aTextStyle, kGray50ID);		//globals are in USeqPrint
	getAlnStyle( gGray25, aTextStyle, kGray25ID);
	getAlnStyle( gGray75, aTextStyle, kGray75ID);
	
	getAlnStyle( aColor, gPrintNucStyle, kPrintNucStyle);//globals are in USeqPrint
	getAlnStyle( aColor, gPrintNameStyle, kPrintNameStyle);
	getAlnStyle( aColor, gPrintZymeStyle, kPrintZymeStyle);

	gSeqListDoc= NULL;

	// Suppress dead-stripping of the following classes 
	if (gDeadStripSuppression) {

 		if (Member(TObject(NULL), TREMapDocument));
		if (Member(TObject(NULL), TREMapWind));
		if (Member(TObject(NULL), TREMapView));

		if (Member(TObject(NULL), TSeqPrintDocument));
		if (Member(TObject(NULL), TSeqPrintWind));
		if (Member(TObject(NULL), TSeqPrintView));

		if (Member(TObject(NULL), TAAWindow));
		if (Member(TObject(NULL), TIUBWindow));
		if (Member(TObject(NULL), TPrefWindow));

		if (Member(TObject(NULL), TAlnDlog));
		if (Member(TObject(NULL), TAlnDoc));
		
		if (Member(TObject(NULL), TTreePrintView));
		if (Member(TObject(NULL), TTreePrintWind));
		if (Member(TObject(NULL), TTreePrintHandler));
		if (Member(TObject(NULL), TTreePrintDocument));

		if (Member(TObject(NULL), TDottyPlotView));
		if (Member(TObject(NULL), TDottyPlotWind));
		if (Member(TObject(NULL), TDottyPlotHandler));
		if (Member(TObject(NULL), TDottyPlotDocument));

		if (Member(TObject(NULL), TAlnPrintView));
		if (Member(TObject(NULL), TAlnPrintWind));
		if (Member(TObject(NULL), TAlnPrintDocument));

		if (Member(TObject(NULL), TColorText));
		if (Member(TObject(NULL), TColorPrefWind));
		if (Member(TObject(NULL), TColorPrefDocument));

		if (Member(TObject(NULL), TRCshifter));
		if (Member(TObject(NULL), TAlnShifter));

		if (Member(TObject(NULL), TREnzyme));
		if (Member(TObject(NULL), TREnzymeVendor));
		if (Member(TObject(NULL), TREMap));
		
		if (Member(TObject(NULL), TSeqIndex));
		if (Member(TObject(NULL), TSeqHIndex));
		if (Member(TObject(NULL), TSeqedView));
		if (Member(TObject(NULL), TSequence));

		if (Member(TObject(NULL), TSeqListView)); //? are we dropping this?
		if (Member(TObject(NULL), TSeqListDoc));

		if (Member(TObject(NULL), TSeqedWindow));

		if (Member(TObject(NULL), TAlnSequence));
		if (Member(TObject(NULL), TAlnView));
		if (Member(TObject(NULL), TAlnIndex));
		if (Member(TObject(NULL), TAlnHIndex));
		if (Member(TObject(NULL), TAlnScroller));
		if (Member(TObject(NULL), TSideScroller));
		if (Member(TObject(NULL), TAlnWindow));

		if (Member(TObject(NULL), TFindDialog));
		if (Member(TObject(NULL), TTextData));
		if (Member(TObject(NULL), TTextView));
		if (Member(TObject(NULL), TTextDocument));
		}
		
//! Debug Text window
/*========
	gApplication->OpenNew(cNewDebug);

	DebugMsg('R.Enzyme Table Dump........');	
	DebugMsg('Name / Site / Vendors');	
	gREMap->fREnzymes->Each(listZyme);
=======*/
/*=========
	DebugMsg('Codon Table Dump........');
	DebugMsg(' AA  Codon  num/K');
	for i= 1 to 64 do begin
		lnum= trunc( gCodonTable[i].numPerK);
		NumToString( lnum, line);
		line= concat('  a  ccc   ',line);
		line[3]= gCodonTable[i].amino;
		line[6]= gCodonTable[i].codon[1];
		line[7]= gCodonTable[i].codon[2];
		line[8]= gCodonTable[i].codon[3];
		DebugMsg( line);
		}
===========*/

}





CONST  
		kSeqSFGetDlogID == 4000;
		kSeqSFGetPopupMenuID == 4000;
    kSFGetText == 12;  //dlog item #
		kSFGetBases == 13;
		kSFGetTree == 14;
		kSFGetBasesAppend == 15;
		
VAR
		gSFGetFirst	: boolean;
    gSFGetKind	: integer;
		MenuHandle		gSFGetPopupMenu;
		

//- pascal integer sfGetDlogHook( integer item, dialogPtr dlog) << Mapp2
//! Mapp3 uses CustomGetFile which has 3rd param on sfGetDlogHook !
pascal integer sfGetDlogHook( integer item, dialogPtr dlog, Ptr myDataPtr) //<< Mapp3
VAR  itype: integer; controlHandle ch, box: rect; longint menuitem,
{
  if (gSFGetFirst || (item IN [kSFGetText..kSFGetBasesAppend])) {
    if ((item IN [kSFGetText..kSFGetBasesAppend])) gSFGetKind= item;
		
    getDItem(dlog, kSFGetBases, itype, handle(ch), box);
    setCtlValue(ch, ord(gSFGetKind=kSFGetBases));

    getDItem(dlog, kSFGetBasesAppend, itype, handle(ch), box);
    setCtlValue(ch, ord(gSFGetKind=kSFGetBasesAppend));

    getDItem(dlog, kSFGetText, itype, handle(ch), box);
    setCtlValue(ch, ord(gSFGetKind=kSFGetText));

    getDItem(dlog, kSFGetTree, itype, handle(ch), box);
    setCtlValue(ch, ord(gSFGetKind=kSFGetTree));

		/*--
    getDItem(dlog, kSFGetPopup, itype, handle(ch), box);
		InsertMenu( gSFGetPopupMenu, -1);
		menuitem= PopUpMenuSelect( gSFGetPopupMenu, box.top, box.left, gSFGetKind);
		DeleteMenu( kSeqSFGetPopupMenuID);
		gSFGetKind= LoWord( menuItem);
		--*/
    gSFGetFirst= FALSE;
    }
  SFGetDlogHook= item;
} //SFGetDlogHook


#pragma segment AOpen
pascal void TSeqAppApplication::GetStandardFileParameters(CommandNumber itsCommandNumber,
								 ProcPtr		VAR fileFilter;
								 TypeListHandle		VAR typeList;
								 short		VAR dlgID;
								 Point		VAR where; VAR dlgHook,
								 ProcPtr		modalFilter;
								 Ptr		VAR activeList;
								 ProcPtr		VAR activateProc;
								 VAR yourDataPtr: UNIV Ptr); // override 
VAR
	integer		numTypes;
{
	inherited::GetStandardFileParameters( itsCommandNumber, fileFilter, typeList, 
			dlgID, where, dlgHook, modalFilter, activeList, activateProc, yourDataPtr);

	dlgID= kSeqSFGetDlogID;
	dlgHook= &sfGetDlogHook;
	gSFGetFirst= TRUE;
	gSFGetKind= kSFGetBases;
	//- gSFGetPopupMenu= GetMenu( kSeqSFGetPopupMenuID);
	
	numTypes = GetHandleSize(Handle(typeList)) / sizeof(ResType);
			//??
	numTypes = numTypes + 2;
	SetHandleSize(Handle(typeList), numTypes * sizeof(ResType));
	(*typeList)^[numTypes-1] = kAAColorType;
	(*typeList)^[numTypes] = kNAColorType;
	
  if (((itsCommandNumber != cOpenGopher) || optionKeyIsDown)) {
		numTypes = numTypes + 1;
		SetHandleSize(Handle(typeList), numTypes * sizeof(ResType));
		FailMemError();
		}
	(*typeList)^[numTypes] = kGopherFileType;
}


pascal CommandNumber TSeqAppApplication::KindOfDocument(CommandNumber itsCommandNumber,
											 TFile itsFile) // override 
VAR  aType: OSType;
{
	itsCommandNumber= inherited::KindOfDocument(itsCommandNumber, itsFile); 
	switch (itsCommandNumber) {
		cFinderNew  : itsCommandNumber= cNewProj;
		cFinderPrint: itsCommandNumber= cOpenText;
		cOpen, 
		cFinderOpen : if (itsFile!=NULL) {
			if ((itsFile->GetFileType( aType) == noErr)) {
				if ((aType == kSeqFileType)) 
					itsCommandNumber= cOpenProj
				else if ((aType == kNAColorType)) 
					itsCommandNumber= cOpenNAColors
				else if ((aType == kAAColorType)) 
					itsCommandNumber= cOpenAAColors
				else if ((aType == kGopherFileType)) 
					itsCommandNumber= cOpenGopher;
				}
			}			
		}
	KindOfDocument= itsCommandNumber;
}



pascal Boolean TSeqAppApplication::TestForSeqFile(TFile aFile)
VAR
		integer		fref, rezref, format;
		OSType		maker;
{
	if ((aFile->GetFileCreator( maker) == noErr) && (maker == kSAppSig)) 
		TestForSeqFile= TRUE 
	else {
		FailOSErr(aFile->OpenFile());
		format= SeqFileFormatWrapper( aFile->fDataRefNum, FALSE);
		FailOSErr(aFile->CloseFile());
		TestForSeqFile= (format != kUnknown);
		}
}





CONST
	kLongIsHandle == 1;
	kLongIsAppFilePtr == 2;
	kLongIsMacFileRef == 3;
	kLongIsTFile == 4;

pascal void OpenTreeDoc( longint data, integer datakind)
VAR  
		aTreeDoc	: TTreePrintDocument;
		aFile	: TFile;
		fref	: integer;
		Handle		hData, hParams;
		p			: CharsPtr;
		
		pascal void ReadDataFile( integer fref)
		VAR	numChars	: longint;
				err	: integer;
		{
			FailOSErr(GetEOF(fref, numChars));	
			hData= NewHandle( numChars+1);	
			FailNIL( hData);
			HLock( hData);
			err= FSRead(fref, numChars, (*hData));
			longint(p)= longint((*hData)) + numChars;  
			(*p)[0]= ((char)(0)); // ?? NULL terminator...!! yes, for hcRead routines 
			HUnlock(hData);
			FailOSErr( err);
			err= FSClose(fref);
		}
		
{
		switch (datakind) {
			case kLongIsHandle:
				hData= Handle(data);
			
			kLongIsTFile: {
				longint(aFile)= data;
				FailOSErr(aFile->OpenFile());
 				ReadDataFile( aFile->fDataRefNum);
				FailOSErr(aFile->CloseFile());
				}
				
			kLongIsAppFilePtr: {
				/*----
				AppFilePtr		anAppFilePtr;
				longint(anAppFilePtr)= data;
				FailOSErr( FSOpen(anAppFilePtr->fName, anAppFilePtr->vRefNum, fref)); 
 				ReadDataFile( fref);
				----*/
				}
				
			kLongIsMacFileRef: {
				ReadDataFile( data);
				}
			
			Otherwise
				FailOSErr( -1); //?
			}

		New(aTreeDoc);
		FailNIL(aTreeDoc);
		aTreeDoc->ITreePrintDocument(kPrintClipType, hData, NULL, NIL/*itsFile*/);
		
		switch (datakind) {
			kLongIsMacFileRef,
			kLongIsAppFilePtr: hData= DisposeIfHandle( hData);
			}
			
		gSeqAppApplication->OpenNewDocument(aTreeDoc); //finish up
}




pascal void TSeqAppApplication::OpenOld(CommandNumber itsOpenCommand, TList aFileList)
		 // override 
//override to import alien sequences into current project doc
//handle std open command only, to import alien TEXT seq files
var
		integer		i, nfiles, fref, rezref, kind, format, err;
		fndrInfo	: FInfo;
		aWind			: TWindow;
		aAlnDoc		: TAlnDoc;
		Boolean		wantAppend;
		aFile			: TFile;
		clName		: MAName;
{
	nfiles= aFileList->GetSize();
	aAlnDoc= NULL;
	
	FOR i= 1 to nfiles){
	aFile= TFile(aFileList->At(i));   

	if ((aFile->fFileType == 'PICT')) {  
		//- inherited::OpenOld( cOpenPict, aFileList); 
		FailOSErr(-1); //not ready yet...
		}		
	else if ((itsOpenCommand == cOpenTree) && (aFile->fFileType == 'TEXT'))   
		OpenTreeDoc( longint(aFile), kLongIsTFile) 
		
	else if ((itsOpenCommand in [cOpen,cOpenProj,cFinderOpen]) 
	 && (aFile->fFileType == 'TEXT')) {  
		wantAppend= shiftKeyIsDown;
		
	 	if ((itsOpenCommand!=cFinderOpen)) CASE gSFGetKind OF
			kSFGetText: {
				 inherited::OpenOld(cOpenText, aFileList); exit(OpenOld);
				 }
			kSFGetTree: {
				OpenTreeDoc( longint( aFile), kLongIsTFile); 
				exit(OpenOld); //!? should be goto next file !
				}
			kSFGetBasesAppend: wantAppend= TRUE;
			kSFGetBases: ;
			}
		 
		if (optionKeyIsDown) {
			inherited::OpenOld(cOpenText, aFileList);	exit(OpenOld);
			}			
		else if (TestForSeqFile( aFile)) {
			//? Do we want to append to current project ?
			aWind= this->GetFrontWindow(); //GetActiveWindow;  
			if ((aWind!=NULL) && (aWind->fDocument != NULL)) {
				aWind->fDocument->GetClassName( clName);
				UprMAName(clName);
				if (clName == 'TALNDOC')) {
					aAlnDoc= TAlnDoc(aWind->fDocument);
					if (aAlnDoc->ListIsEmpty()) { 
						aAlnDoc->CloseAndFree();
						aAlnDoc= NULL;
						}
					}
				}
				
			if ((aAlnDoc!=NULL) && wantAppend) {
				// import into current proj 
				gSeqListDoc= aAlnDoc; //!? do we need this holdover 
				FailOSErr(aFile->OpenFile());
		  	aAlnDoc->AppendRead(aFile->fDataRefNum);
				FailOSErr(aFile->CloseFile());
				aAlnDoc->ShowReverted();
				}			else {
				inherited::OpenOld(cOpenProj, aFileList); exit(OpenOld);
				}
			}			
		else {
			inherited::OpenOld(cOpenText, aFileList); exit(OpenOld);
			}			
		}	else {
		inherited::OpenOld(itsOpenCommand, aFileList); exit(OpenOld);
		}
	} //for each file
	
}

/*******
pascal void TSeqAppApplication::OpenNewDocument(TDocument VAR aNewDocument)
VAR
		FailInfo		fi;
		Str255		newTitle;
		TWindow		aWindow;

	pascal void HdlOpenNew(integer error,  long message)
	{
		FreeIfObject(aNewDocument);
		aNewDocument = NULL;
		FailNewMessage(error, message, messageNewFailed);
	}

{
/*-------
	THIS IS TApplication->OpenNew(itsCommandNumber) WITHOUT THIS
	Prelim DoMakeDocument SO I CAN Create && Initialize Subdocs properly
	from other Documents
	.....
	aNewDocument = NULL;
	aNewDocument = DoMakeDocument(KindOfDocument(itsCommandNumber, NULL));
-------*/
	CatchFailures(fi, HdlOpenNew);

	aNewDocument->DoInitialState();
	aNewDocument->DoMakeViews(kForDisplay);
	aNewDocument->DoMakeWindows();

	aNewDocument->UntitledName(newTitle);
	// For MacApp 1.1, newTitle should be always != '' 
	if (newTitle != '')
		aNewDocument->SetTitle(newTitle)
	else if ((aNewDocument->fWindowList != NULL) && (aNewDocument->fWindowList.GetSize > 0))
	// Grope, grope, grope 
		{											// must set fTitle field anyways 
		aWindow = TWindow(aNewDocument->fWindowList->First());

		aWindow->GetTitle(newTitle);
		Handle(aNewDocument->fTitle) = DisposeIfHandle(aNewDocument->fTitle);
		aNewDocument->fTitle = NewString(Copy(newTitle, aWindow->fPreDocname, length(newTitle) -
										   aWindow->fConstTitle));
		FailNil(aNewDocument->fTitle);
		}

	AddDocument(aNewDocument);
	FailSpaceIsLow(); 									// Fail if document leaves us with no room 
	// Don't attempt to show the windows until we're sure we won't fail 
	aNewDocument->ShowWindows();
	Success(fi);
}
******/

 

pascal TDocument TSeqAppApplication::DoMakeDocument(CommandNumber itsCommandNumber, 
													TFile itsFile) // override 
VAR
		aAlnDoc				: TAlnDoc;
		aTextDocument	: TTextDocument;
		aHelpDocument	: THelpDocument;
		aGopherDocument	: TGopherDocument;
		aTreeDoc	: TTreePrintDocument;
		numChars	:  longint;
		TColorPrefDocument		aColorPrefDocument;
{
 	switch (itsCommandNumber) {
			
		cHelpCommand,
		cOpenHelp: {
			New(aHelpDocument);
			FailNIL(aHelpDocument);
			aHelpDocument->IHelpDocument( (*gHelpFile)^, gAppWDRef);
			DoMakeDocument = aHelpDocument;
			}
 
		cOpenNAColors,
		cOpenAAColors: {
			New(aColorPrefDocument);
			FailNIL(aColorPrefDocument);
			if itsCommandNumber == cOpenAAColors
) aColorPrefDocument->IColorPrefDocument( itsFile, kAmino)
				else aColorPrefDocument->IColorPrefDocument( itsFile, kDNA);
			DoMakeDocument = aColorPrefDocument;
			}

		cOpenTree: {
			New(aTreeDoc);
			FailNIL(aTreeDoc);
			aTreeDoc->ITreePrintDocument(kPrintClipType, NULL, NIL, itsFile);
			DoMakeDocument = aTreeDoc;
			}

		cNewGopherFolder,  	// data is in gGopherData 
		cOpenGopherFolder, 	// data is in gGopherFile 
		cNewGopher, cOpenGopher: {
		
			New(aGopherDocument);
			FailNIL(aGopherDocument);
			if ((itsCommandNumber == cNewGopherFolder)) {
				//! set aGopherDocument->Untitled() to name it gGopherTitle 
				aGopherDocument->IGopherDocument( gGopherData, gGopherTitle, gCurrentGopher, itsFile);
				}			else if ((itsCommandNumber == cOpenGopherFolder)) {
				aGopherDocument->IGopherDocument( NULL, gGopherTitle, NULL, itsFile);
				}			else {
				aGopherDocument->IGopherDocument( NULL, NIL, NULL, itsFile);
				}
			DoMakeDocument = aGopherDocument;
			}
			
			
		cNewGopherText,		// data is in gGopherData 
		cOpenGopherText,	// data is in gGopherFile 
		cNewText, cOpenText: {
			New(aTextDocument);
			FailNIL(aTextDocument);
			aTextDocument->ITextDocument(itsFile, kTextScrapType, 0, 0);
			
			if ((itsCommandNumber == cNewGopherText)) {
					// this is a mess ... need new textdoc type for gopher? -- display > 32K !?
				numChars= GetHandleSize( gGopherData);
				if (numChars > kUnlimited) {	 
					gApplication->ShowError(0, messageAlert + kFileTooBig);
					numChars = kUnlimited;
					/*--
					if ((aTextDocument->fTitle!=NULL)) 
							SetString( aTextDocument->fTitle, concat(aTextDocument.(*fTitle)^,'-Short'));
					--*/
					}
				// set aTextDocument->Untitled() to gGopherTitle 
				SetHandleSize(aTextDocument->fTextData.fChars, numChars);
				FailMemError();
				HLock( gGopherData);
				HLock( aTextDocument->fTextData.fChars);
				BLockMove( (*gGopherData), aTextDocument->fTextData.(*fChars), numChars);
				HUnlock( gGopherData);
				HUnlock( aTextDocument->fTextData.fChars);
				}
				
			DoMakeDocument = aTextDocument;
			}
			
			
		cNewDebug: if (gDebugDoc=NULL) {
			New(gDebugDoc);
			FailNIL(gDebugDoc);
			gDebugDoc->ITextDocument(itsFile, kTextScrapType, 0, 0);
			DoMakeDocument = gDebugDoc;
			}
								
		/*cNew: begin
			-- ? variable action
			end(); */
		/*cOpen: begin
			-- variable action
			++ caught by OpenOld if ftype == TEXT
			end();*/
		
		otherwise begin //== cNewProj/cOpenProj, cNew/cOpen
			New(aAlnDoc);
			FailNIL(aAlnDoc);
			aAlnDoc->IAlnDoc( itsFile);
			DoMakeDocument = aAlnDoc;
			}
			
		}
}


pascal void TSeqAppApplication::AboutToLoseControl(Boolean convertClipboard) // override 
{
	inherited::AboutToLoseControl(convertClipboard);
}


	
pascal void TIUBWindow::SetPrefID(void)    
{
	gPrefWindID= kIUBcodeWindID; gPrefWindName= 'TIUBWindow';
}

pascal void TAAWindow::SetPrefID(void)    
{
	gPrefWindID= kAAcodeWindID; gPrefWindName= 'TAAWindow';
}


pascal void TSeqAppApplication::DoMenuCommand(aCommandNumber:CommandNumber); // override 

	pascal void PrefsDialog(void)
	VAR aWind	: TPrefsDialog; boolean isNowOpen,
		pascal void checkOpenWind( TWindow aWind)
		{
			if (Member(TObject(aWind), TPrefsDialog)) { 
				aWind->Select(); isNowOpen= TRUE;
				}
		}
	{
		isNowOpen= FALSE;
		ForAllWindowsDo(checkOpenWind);
		if (!isNowOpen) {
			aWind = TPrefsDialog(gViewServer->NewTemplateWindow(kPrefsDialogID, NULL));
			FailNIL(aWind); 		 
			aWind->IPrefsDialog();
			aWind->ShowReverted(); 
			aWind->Open(); 
			aWind->Select();	 
			}
	}
	
	pascal void HelpDialog(void)
	VAR 
			isNowOpen	: boolean;
			THelpDocument		aHelpDocument ;
			
		pascal void checkOpenDocs( TDocument aDoc)
		{
			if (Member(TObject(aDoc), THelpDocument)) { 
				TWindow(aDoc->fWindowList->First()).Select; 
				isNowOpen= TRUE;
				}
		}
	{
		isNowOpen= FALSE;
		ForAllDocumentsDo(checkOpenDocs);
		if (!isNowOpen) {
			New(aHelpDocument);
			FailNIL(aHelpDocument);
			aHelpDocument->IHelpDocument( (*gHelpFile)^, gAppWDRef);
			this->OpenNewDocument( aHelpDocument);
			}
	}

/******
	pascal void ShowIUBDlog(void)
	VAR aWind	: TIUBWindow; boolean isNowOpen,
	
		pascal void checkOpenWind( TWindow aWind)
		{
			if (Member(TObject(aWind), TIUBWindow)) { 
				aWind->Select(); isNowOpen= TRUE;
				}
		}
	{
		isNowOpen= FALSE;
		ForAllWindowsDo(checkOpenWind);
		if (!isNowOpen) {
			aWind = TIUBWindow(gViewServer->NewTemplateWindow(kIUBcodeWindID, NULL));
			FailNIL(aWind); 		 
			aWind->IPrefWindow(); 
			aWind->ShowReverted();   
			aWind->Open();  
			}
	}
	*****/
	
	pascal void ShowIUBDlog(void)
	VAR 
			isNowOpen	: boolean;
			TColorPrefDocument		aColorPrefDocument ;
			
		pascal void checkOpenDocs( TDocument aDoc)
		{
			if (Member(TObject(aDoc), TColorPrefDocument)) 
			 if (TColorPrefDocument(aDoc).fBaseKind != kAmino)
			  { 
				TWindow(aDoc->fWindowList->First()).Select; 
				isNowOpen= TRUE;
				}
		}
	{
		isNowOpen= FALSE;
		ForAllDocumentsDo(checkOpenDocs);
		if (!isNowOpen) {
			New(aColorPrefDocument);
			FailNIL(aColorPrefDocument); 
			aColorPrefDocument->IColorPrefDocument( NULL/*itsFile*/, kDNA);
			this->OpenNewDocument( aColorPrefDocument);
			}
	}

/******
	pascal void ShowAADlog(void)
	VAR aWind	: TAAWindow; boolean isNowOpen,
	
		pascal void checkOpenWind( TWindow aWind)
		{
			if (Member(TObject(aWind), TAAWindow)) {  
				aWind->Select(); isNowOpen= TRUE;
				}
		}
	{
		isNowOpen= FALSE;
		ForAllWindowsDo(checkOpenWind);
		if (!isNowOpen) {
			aWind = TAAWindow(gViewServer->NewTemplateWindow(kAAcodeWindID, NULL));
			FailNIL(aWind); 		 
			aWind->IPrefWindow(); 
			aWind->ShowReverted();   
			aWind->Open();  
			}
	}
******/

	pascal void ShowAADlog(void)
	VAR 
			isNowOpen	: boolean;
			TColorPrefDocument		aColorPrefDocument ;
			
		pascal void checkOpenDocs( TDocument aDoc)
		{
			if (Member(TObject(aDoc), TColorPrefDocument)) 
			 if (TColorPrefDocument(aDoc).fBaseKind == kAmino)
				{ 
				TWindow(aDoc->fWindowList->First()).Select; 
				isNowOpen= TRUE;
				}
		}
	{
		isNowOpen= FALSE;
		ForAllDocumentsDo(checkOpenDocs);
		if (!isNowOpen) {
			New(aColorPrefDocument);
			FailNIL(aColorPrefDocument); 
			aColorPrefDocument->IColorPrefDocument( NULL/*itsFile*/, kAmino);
			this->OpenNewDocument( aColorPrefDocument);
			}
	}
	


{
	switch (aCommandNumber) {
	
		PrefsDialog		cPrefsDialog;

		cHelpCommand,
		cOpenHelp	: HelpDialog;  
		
		cIUBcodes	: ShowIUBDlog;
		cAAcodes	: ShowAADlog;
		
		OpenNewChild		cAddChildCmd;
		OpenEditChild		cRemoveChildCmd; //RemoveChildren;

		cGopher		: OpenGopher;  
		OpenGopherPrefs		cGopherPrefs;

		cMailPref	: OpenMailPref;
		OpenCheckmail		cCheckMail;
		
		cSendMail, cBugMail, cGBFetch, cEMBLFetch, 
		cUHServer : MailTo(aCommandNumber); 		
		
		default:
			inherited::DoMenuCommand(aCommandNumber);
		}
}


pascal void TSeqAppApplication::DoSetupMenus(void) // override 
VAR
	Boolean		lowSpace;
{
	inherited::DoSetupMenus();
	
	lowSpace = MemSpaceIsLow;  
	Enable(cNewText, !lowSpace);
	Enable(cOpenText, !lowSpace);
	Enable(cOpenHelp, !lowSpace);
	
	//some help dialogs/picts...
	Enable( cIUBcodes, TRUE);
	Enable( cAAcodes, TRUE);
	Enable( cPrefsDialog, TRUE);

	Enable( cAddChildCmd, TRUE);
	if ((gChildCommandList!=NULL))
		Enable( cRemoveChildCmd, gChildCommandList.GetSize>0);

	Enable( cMailPref, TRUE); 
	//! all of these depend on MacTCP 
	Enable( cCheckMail, gTCPIsInstalled); 
	Enable( cSendMail, gGoodMac && gTCPIsInstalled); 
	Enable( cGopher, gTCPIsInstalled); 
	Enable( cNewGopher, gTCPIsInstalled && !lowSpace);
	Enable( cGopherPrefs, TRUE);
//-	Enable( cFileTransfer, gTCPIsInstalled);  
	Enable( cBugMail, gGoodMac && gTCPIsInstalled);
	Enable( cGBFetch, gGoodMac && gTCPIsInstalled); 
	Enable( cEMBLFetch, gGoodMac && gTCPIsInstalled); 
	Enable( cUHServer, gGoodMac && gTCPIsInstalled); 
}






pascal void TSeqAppApplication::ShowError(OSErr error,  LongInt message) // override 
CONST
		kMsgCmdErr			== messageCommandError / 0x10000;
		kMsgAlert				== messageAlert / 0x10000;
		kMsgLookup			== messageLookup / 0x10000;
		kMsgAltRecov		== messageAlternateRecovery / 0x10000;
TYPE
		Converter			== RECORD CASE Boolean OF
				TRUE: (LongInt message)
				FALSE: (short hiWd, loWd)
				}
VAR
		Converter		c;
		short		alertID;
		Boolean		genericAlert;
		Str255		opString;
{
	if ((message == msgMyError) /*? or err == errMyError*/) {
		/*------
		c.message = message;
		if ((c.hiWd == kMsgCmdErr))	{
			alertID = phCommandError;
			CommandToName(c.loWd, opString);
			}
		----*/
		//- alertID = phGenError; 
		//- alertID = phUknownErr; 
		alertID = phCommandError;
		genericAlert = TRUE;
		opString = '';
		StdAlert(alertID);
		gInhibitNestedHandling = FALSE;					 
		if (genericAlert) ResetAlrtStage;
		}	else
		inherited::ShowError(error,  message);
}







 

/*****
// THandle1HighlevelEvents -----------------------------

	THandle1HighlevelEvents	== OBJECT (TEventHandler)
		pascal void THandle1HighlevelEvents::IEventHandler(TEventHandler itsNextHandler) 
			// override 
		pascal Boolean THandle1HighlevelEvents::DoHandleEvent( EventRecordPtr nextEvent, 
				TCommand VAR commandToPerform) // override 
		}

pascal void THandle1HighlevelEvents::IEventHandler(TEventHandler itsNextHandler) 
		// override 
{
	inherited::IEventHandler(itsNextHandler);
}

pascal Boolean THandle1HighlevelEvents::DoHandleEvent( EventRecordPtr nextEvent, 
		TCommand VAR commandToPerform) // override 
VAR
	theErr:OSErr;
{

	if ((nextEvent->what == kHighLevelEvent)) {
		DoHandleEvent= TRUE;
		//-- this->SetupTheMenus(); 
		theErr= AEProcessAppleEvent((*nextEvent));
		if ((theErr!=noErr) && (theErr != errAEEventNotHandled))
			then FailOSErr(theErr);
		}	else
		DoHandleEvent= FALSE;
		
}
******/
 



//	TPrefsDialog -------------------------- 


pascal void TPrefsDialog::IPrefsDialog(void)
{	
	IPrefWindow();
	fWantSave= TRUE;
/*----------
VAR
	TButton		aSendBut;
	aFrom		: TEditText;
{	

	fPrefsDialogView= TPrefsDialogView(FindSubView('View'));
	if ((fPrefsDialogView!=NULL)) fPrefsDialogView->IPrefsDialogView();
	fTo 			= TEditText(FindSubView('eTo:'));  
  fCc   		= TEditText(FindSubView('eCc:'));  
  fSubject	= TEditText(FindSubView('eSb:'));  
  aFrom			= TEditText(FindSubView('eFm:'));  
	if ((aFrom!=NULL)) aFrom->SetText( gMyMailAddress, TRUE);
	aSendBut= TButton(FindSubView('OKAY'));
	if ((aSendBut!=NULL) then aSendBut->fDefaultChoice= mSendit;
--------*/
}


pascal void TPrefsDialog::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) /* override */ 
{
	inherited::DoEvent(eventNumber, source, event);
	/*-----
	switch (eventNumber) {
		Sendit		mSendit;
		case mCheckboxHit: 
		  if ((fUnlockTexts!=NULL) && (source=fUnlockTexts)) {
				if (fUnlockTexts->IsOn()) UnlockTexts
				else RelockTexts;
				}

		Otherwise
			inherited::DoEvent(eventNumber, source, event);
		}
	------*/
}


pascal void TPrefsDialog::SetPrefID(void) /* override */ 
{
	gPrefWindID= kPrefsDialogID; gPrefWindName= 'TPrefsDialog';
}





// TColorPrefDocument ---------------------



pascal void TColorText::InstallColor(RGBColor theColor, Boolean redraw) // override 
/*
  ((CRGBColor &) fTextStyle.tsColor) == theColor;
	if (redraw) this->DrawContents();
*/  
{
	if ((fDrawingEnvironment!=NULL)) {
		if (TColorPrefDocument(fDocument).fSwapBackground->IsOn())) 
			fDrawingEnvironment->fBackgroundColor= theColor
		else  
			fDrawingEnvironment->fForegroundColor= theColor;
		}
	
	inherited::InstallColor( theColor, redraw);
}

		
pascal void TColorPrefDocument::IColorPrefDocument( TFile itsFile, Integer baseKind)
VAR
	OSType		fileType; 
	ch 	: char;
{
	fBaseKind= baseKind;
	fDoSetGlobals= False;
	fSwapBack= gSwapBackground;
	fSwapBackground= NULL;
	fDescription= '';
	if (baseKind == kAmino) then {
		fWindID = kAAColorView;
		fileType= kAAColorType;
		}	else {
		fWindID = kNAColorView;
		fileType= kNAColorType;
		}
	gNoColor.red= -123;
	gNoColor.blue= 456;
	gNoColor.green= -789;
	gNoViewID= '????';
	FOR ch= ' ' TO '~'){
		fColors[ch]	= gNoColor;
		fViewIDS[ch]= gNoViewID;
		}
		
	if (itsFile == NULL)) //? don't need ?
		itsFile= NewFile(fileType, kSAppSig, kUsesDataFork, kUsesRsrcFork, !kDataOpen, !kRsrcOpen);
	IFileBasedDocument(itsFile, fileType);
	fSavePrintInfo= FALSE; 
}

pascal void TColorPrefDocument::FreeData(void)
{

}


pascal void TColorPrefDocument::Free(void) // override 
{
	FreeData();
	inherited::Free();
}


pascal void TColorPrefDocument::SetGlobals(void) 
VAR  ch: char;
{
		//install colors only at close ?? -- need also to initialize gAAcolors && gNAcolors !
		//need Okay/Cancel buttons on this window/doc ??
	FOR ch= 'a' TO 'z')fColors[ch]= fColors[UprChar(ch)]; //!?!
	if ((fBaseKind == kAmino))
		BlockMove( &fColors, &gAAcolors, sizeof(colorsArray)) 
	else
		BlockMove( &fColors, &gNAcolors, sizeof(colorsArray)); 
	//-- not ready -- background not displaying...
	if ((fSwapBackground!=NULL)) gSwapBackground= fSwapBackground->IsOn();
}


pascal void TColorPrefDocument::Close(void) /* override */ 
{
	if ((fDoSetGlobals)) SetGlobals;		
  inherited::Close();
}



pascal void TColorPrefDocument::DoHitLetter(TStaticText theLet)
VAR
		ch	: char;
		OSType		id ;
		Point		where;
		Str63		prompt;
		RGBColor		outColor;
{				
	id= theLet->fIdentifier;
	for ch= ' ' to '~' do if (fViewIDs[ch] == id) then begin
		SetPt( where, 0, 0);
		prompt= '1'; 
		prompt[1]= ch;
		prompt= concat('Choose color for "',prompt,'"');
		outColor= fColors[ch];
		if (GetColor( where, prompt, outColor, outColor)) {
			fColors[ch]= outColor; //-- save color
			this->Changed( 1, this); //-- mark fDoc as dirty/need save
			fDoSetGlobals= True;   //-- install color in any global color set if needed 
			//- theLet->fTextStyle.tsColor= outColor;  theLet->ForceRedraw();
			TColorText(theLet)->InstallColor(outColor, kRedraw); 
			}
		LEAVE(); 
		}
}


pascal void TColorPrefDocument::DoMakeViews(Boolean forPrinting) // override 
VAR
		aWindow	: TWindow;
		ch 	: char;
		RGBColor		aColor;
		TCluster		aColorGroup;
		TEditText		aDescText;
		aStr	: Str255;
		
	pascal Boolean equalColors( RGBColor a, b)
	{
		equalColors= (a.red == b.red) && (a.green == b.green) && (a.blue == b.blue);
	}
	
	pascal void collectLetters(TColorText letter)
	VAR   
			ch 	: char;
			Str255		aStr;
			RGBColor		aColor;
	{
		letter->GetText(aStr); 
		ch= aStr[1];
		fViewIDs[ch]= letter->fIdentifier;
		//- aColor= letter->fTextStyle.tsColor;
		aColor= fColors[ch];
		letter->InstallColor( aColor, kDontRedraw); 
	}
		
{
	aWindow = gViewServer->NewTemplateWindow(fWindID, this);
	FailNil(aWindow);
	
	fColorPrefWind= TColorPrefWind(aWindow);
	fColorPrefWind->fDocument= this;

	aStr= fDescription;
	aDescText= TEditText(aWindow->FindSubView('desc')); 
	if (aDescText!=NULL)) aDescText->SetText( aStr, kDontRedraw);
	fSwapBackground= TCheckBox(aWindow->FindSubView('back')); 
	FailNIL(fSwapBackground);
	fSwapBackground->SetState( fSwapBack, kDontRedraw);
	
	aColorGroup= TCluster(aWindow->FindSubView('cclu')); 
	if (gConfiguration.hasColorQD) {
		FailNil(aColorGroup);
			 //set unknowns to usable color
		FOR ch= ' ' to '~'){
			aColor= fColors[ch];
			if (equalColors( aColor, gNoColor)) {
				if (fBaseKind == kAmino)) fColors[ch]= gAAcolors[ch]
				else fColors[ch]= gNAcolors[ch];
				}
			}
		aColorGroup->EachSubView(collectLetters);
		}
	
	//!? call SetGlobals here if data was read from disk ?
	if (fDoSetGlobals) {
		SetGlobals(); 
		fDoSetGlobals= False; 
		}

	//? may need to show TEView in PrintView... ?
	this->ShowReverted();   
}


pascal void TColorPrefDocument::UntitledName(Str255 VAR noName) // override 
//called AFTER .Idoc and .MakeViews 
{
	inherited::UntitledName( noName);
	/*---
	noName= Concat(fSeq->fName,' PrettyPrint'); 
	if ((fWindowList != NULL) && (fWindowList.GetSize > 0)) 		
		TWindow(fWindowList->First())->SetTitle(noName);
	---*/
}


pascal TFile TColorPrefDocument::DoMakeFile(itsCommandNumber:CommandNumber); // override 
{
	DoMakeFile= NewFile(fScrapType, gApplication->fCreator, kUsesDataFork, kUsesRsrcFork, 
												!kDataOpen, !kRsrcOpen);
}



pascal void TColorPrefDocument::DoNeedDiskSpace(TFile itsFile,
															long VAR dataForkBytes, rsrcForkBytes)
VAR numLets: integer;															
{
	//)!get Print record requirements 
	//- inherited::DoNeedDiskSpace(dataForkBytes, rsrcForkBytes); 
	
	numLets= ord('~') - ord(' ') + 1;
	rsrcForkBytes = rsrcForkBytes + numLets * (sizeof(TextStyle) /*+ Length(fontname)*/);
}

pascal void TColorPrefDocument::DoRead(aFile:TFile; Boolean forPrinting)
VAR
		saveMap 	: integer;
		TextStyle		aTextStyle;
		integer		i, nColors;
		ch	: char;
		Handle		oldValue ;
{
	//-- inherited::DoRead(aRefNum, rsrcExists, forPrinting);)!read print info stuff
	if (aFile->HasRsrcFork()) {
		if (!aFile->IsRsrcForkOpen()) FailOSErr( aFile->OpenRsrcFork(fsRdPerm));
		saveMap= aFile->UseResource();
		
		/*! need also to read/write
			-- description string
			-- fSwapBackground.IsOn flag
		*/
		oldValue= Get1Resource('STR ', 1);
		if (oldValue!=NULL) fDescription= (*StringHandle( oldValue))^;
		
		oldValue= Get1Resource('Flag', 1);
		if (oldValue!=NULL))	fSwapBack= Boolean((*oldValue)^);

		//- fHaveColors= GetTextStyleRsrc(aFile->fRsrcRefNum, theTextStyle, 1);
		nColors= Count1Resources('TxSt');
		for i= 1 to nColors do begin
			ch= ((char)( i-1+ord(' ')));
			MAGetTextStyle(i, aTextStyle);  
			fColors[ch]= aTextStyle.tsColor;
			}
		if (nColors > 0) fDoSetGlobals= True;   //-- install color in any global color set if needed 
		
		UseResFile(saveMap);
		}
}


pascal void TColorPrefDocument::DoWrite(TFile aFile, Boolean makingCopy)
VAR
		TextStyle		aTextStyle;
		integer		i, saveMap ;
		ch	: char;
		TEditText		aDescText;
		Str255		aStr;
		StringHandle		aStrHand;
		Handle		oldValue;
{
	/*- inherited::DoWrite(aRefNum, makingCopy); 	--)NO write print info stuff*/			
	aTextStyle= gAlnStyle; //?
	if (aFile->HasRsrcFork()) { //? if not, make one?
		if (!aFile->IsRsrcForkOpen()) FailOSErr( aFile->OpenRsrcFork(fsRdWrPerm));
		saveMap= aFile->UseResource();
		
		oldValue= Get1Resource('STR ', 1);
		if (oldValue!=NULL) {
			RmveResource(oldValue);
			DisposHandle(oldValue);
			}
		aDescText= TEditText(fColorPrefWind->FindSubView('desc')); 
		aDescText->GetText(aStr);
		aStrHand= NewString(aStr);
		AddResource( Handle(aStrHand), 'STR ', 1, '');
		SetResAttrs( Handle(aStrHand), resPurgeable);
		ChangedResource( Handle(aStrHand)); 
		
		oldValue= Get1Resource('Flag', 1);
		if (oldValue!=NULL) {
			RmveResource(oldValue);
			DisposHandle(oldValue);
			}
		oldValue= NewHandle(sizeof(SignedByte));
		(*oldValue)^= SignedByte(fSwapBackground->IsOn());
		AddResource( Handle(oldValue), 'Flag', 1, '');
		SetResAttrs( Handle(oldValue), resPurgeable);
		ChangedResource( Handle(oldValue)); 
		
		FOR ch= ' ' TO '~'){
			i= ord(ch) - ord(' ') + 1;
			aTextStyle.tsColor= fColors[ch];
			SetTextStyleRsrc(aFile->fRsrcRefNum, aTextStyle, i);
			}
			
		FailOSErr( aFile->UpdateResource());
		UseResFile(saveMap);
		}
}




// TColorPrefWind ------------------------- 

/*----
pascal void TColorPrefWind::IColorPrefWind(void)
{
	IPrefWindow();
	fWantSave= TRUE;
}
pascal void TColorPrefWind::SetPrefID(void) /* override */ 
{
	gPrefWindID= kColorPrefWindowRSRCID; gPrefWindName= 'TColorPrefWind';
}
---*/

pascal void TColorPrefWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
{
	switch (eventNumber) {

		mOKhit: {
			TColorPrefDocument(fDocument).fDoSetGlobals= True;
			CloseByUser();
			//- inherited::DoEvent(mDismiss,source, event); 
			}
		mCancelHit: {
			TColorPrefDocument(fDocument).fDoSetGlobals= False;
			CloseByUser();
			//- inherited::DoEvent(mDismiss,source, event); 
			}
		
		case mStaticTextHit:  
			TColorPrefDocument(fDocument)->DoHitLetter( TStaticText(source));
				
		case mCheckboxHit: 
			if ((source == TColorPrefDocument(fDocument).fSwapBackground)) {

				}			else 
				inherited::DoEvent(eventNumber,source, event); 

		otherwise
			inherited::DoEvent(eventNumber,source, event); 
		}
}

