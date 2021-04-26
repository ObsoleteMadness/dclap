// USeqChild.inc.p -- call/manage child applications 
// d.g.gilbert, 1991-2 

#pragma segment USeqChild

CONST
			//! copied from SeqApp.app.p 
	cOpenText	== 21;	 //Open plain text
	cOpenProj	== 22;	 //Open project list
	cOpenHelp	== 24;
	cOpenTree == 26;

	kIsInput == FALSE;
	kIsOutput == TRUE;
	kDoDelete == TRUE;
	kDoSave == FALSE;

	mChildren == 30;  //this is also USeqApp private global... make public here?

	kPrefViewType == 'VPrf'; // from UApp. private -- should be public 
	msgMyError == 0x80001234;


VAR
	integer		gChildDialogID; //for  TChildDialog.SetPrefID 

	
// TChildFile -------------------

pascal void TChildFile::IChildFile(str255 theName, OSType itsType, 
						integer		itsVRefNum, action; boolean isOutput, doDelete)
VAR  aFile: TFile;
{
	NEW(aFile); fFile= aFile;
	fFile->IFile( itsType, kSAppSig, //?! or Child sig
						true, noResourceFork, false, false);
	FailOSErr( fFile->SpecifyWithTrio( itsVRefNum, 0, theName));
	fAction= action;
	fOutput= isOutput;
	fDelete= doDelete;
}

pascal void TChildFile::Free(void) // override 
VAR  err: integer;
{
	// Test this after rest is working...
	if ((fDelete)) err= fFile->DeleteFile();
	TObject(fFile)= FreeIfObject( fFile);
	
	inherited::Free();
}




// TChildApp -------------------

pascal void TChildApp::IChildApp( theSN:ProcessSerialNumberPtr; TList theFiles)
VAR
	integer		err, vref ;
{
	fSN= theSN;
	fFiles= theFiles;
}

pascal void TChildApp::Free(void) // override 
{
	Ptr(fSN)= DisposeIfPtr( Ptr(fSN));
	TObject(fFiles)= FreeListIfObject( fFiles);
	inherited::Free();
}

pascal void TChildApp::Finished(void) 
// post processing by parent 

	pascal void processChildFile( TChildFile aChildF)
	TList		VAR  aList;
			 cmd	: Integer;
	{
		cmd= 0;
		CASE aChildF->fAction OF
			kChildReturnOpenFile 		:	cmd= cFinderOpen;
			kChildReturnOpenSeqFile	:	cmd= cOpenProj; 
			kChildReturnOpenTextFile: cmd= cOpenText; 
			kChildReturnOpenTreeFile: cmd= cOpenTree;
			kChildReturnOpenPictFile: gApplication->Beep(1);
			kChildReturnNoAction : ;
			}
		if (cmd!=0)) {
			//! can't use aChildF->fFile then .Free it and still have Doc free it -- Clone it
			aList= NewList;
			aList->InsertLast( aChildF->fFile->Clone());
			gApplication->OpenOld(cmd, aList); 
			aList->Free();
			}
	}
	
{
	if ((fFiles!=NULL)) fFiles->Each( processChildFile);
}







pascal OSErr HandleChildDiedEvent( AppleEvent theAppleEvent, reply, Longint handlerRefCon)
VAR
	myErr		: OSerr;
	appErr	: longint;
	appSN		: ProcessSerialNumber;
	DescType		returnedType ;
	size		actualSize;
	TChildApp		aChild ;
	
		pascal Boolean MatchesSN( aChild:TChildApp);
		boolean		var  match ;
		{		
			myErr= SameProcess( aChild.(*fSN), appSN, match);
			MatchesSN= match;
		}
	
{
	myErr= AEGetParamPtr( theAppleEvent, keyErrorNumber, typeLongInteger, 
						returnedType, &appErr, sizeof(appErr), actualSize);
	FailOSErr(myErr);
	if ((appErr == noErr)) {  
		myErr= AEGetParamPtr( theAppleEvent, keyProcessSerialNumber, typeProcessSerialNumber, 
							returnedType, &appSN, sizeof(appSN), actualSize);
		FailOSErr(myErr);
		
		TObject(aChild)= gChildList->FirstThat( MatchesSN);
		if ((aChild!=NULL)) {
			aChild->Finished();
			gChildList->Delete(aChild);
			aChild->Free();
			}
		}
	HandleChildDiedEvent= noErr;
}



/*usage-----------------------
VAR docList: AEDescList;
		boolean		firstChildFile;
		
	firstChildFile= TRUE;
	do {
		ChildOpenAddFile( firstChildFile, fileName, vRefNum, docList);
		firstChildFile= FALSE;
	} while (!(NoMoreFiles));
	childSN= LaunchChildApp( appName, ChildOpenLaunchParams( docList));
---------------------*/




//	TChildListView	-----------------

pascal void TChildListView::GetItemText( integer anItem, Str255 VAR aString) // override 
VAR 	aChildCommand: TChildCommand;
{
   aChildCommand= TChildCommand(gChildCommandList->At( anItem));
	 if ((aChildCommand!=NULL)) aString= aChildCommand->fMenuName
	 else aString= '';
}





//	TEditChildWind	-----------------


pascal void TEditChildWind::SetPrefID(void) /* override */ 
{
	gPrefWindID= kEditChildWindID;  
	gPrefWindName= 'TEditChildWind';
}



pascal void TEditChildWind::IEditChildWind(void)
{
	IPrefWindow();
	fWantSave= FALSE;  
		//build TTextListView data from gChildCommandList 
	fListView = TTextListView( this->FindSubView('LIST'));
	FailNIL( fListView);
	fListView->DelItemLast( fListView->fNumOfRows);
	fListView->InsItemLast( gChildCommandList->GetSize());
	fEdit= TButton( FindSubView('OKAY'));
	fDelete= TButton( FindSubView('DELE'));
}


pascal void TEditChildWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
CONST
		kDeleteMe == -12345;
VAR
	TEditText		aEdit;
	THelpDocument		aHelpDocument;
	Str255		helpName;
	
	pascal void EditChild( integer anItem)
	VAR	 aChild: TChildCommand;
	{
		aChild= TChildCommand(gChildCommandList->At( anItem));
		if ((aChild!=NULL)) aChild->Edit();
	}

	pascal void MarkChildForRemoval( integer anItem)
	VAR	 aChild: TChildCommand;
	{
		aChild= TChildCommand(gChildCommandList->At( anItem));
		if ((aChild!=NULL)) aChild->fMinSeqs= kDeleteMe;
	}
	
	pascal void RemoveMarkedChildren( TChildCommand aChild)
	VAR	
		integer		viewID;
		MenuHandle		aMenuHandle;
		integer		i;
		Str255		aStr;
	{
		if ((aChild->fMinSeqs == kDeleteMe)) {
			viewID= aChild->fViewID;
			TPrefApplication(gApplication)->DeletePreference( kPrefViewType, viewID);
			TPrefApplication(gApplication)->DeletePreference( 'STR#', viewID);
			TPrefApplication(gApplication)->DeletePreference( 'TEXT', viewID);
			TPrefApplication(gApplication)->DeletePreference( 'TEXT', 10000+viewID);
				//! must also remove from Menu...
			aMenuHandle= GetMenu(mChildren);
			if ((aMenuHandle!=NULL))
			 FOR i= 3 to CountMITems( aMenuHandle)){
				GetItem( aMenuHandle, i, aStr);
				if ((aStr == aChild->fMenuName)) {
					DelMenuItem( aMenuHandle, i);
					LEAVE();
					}
				}
			gChildCommandList->Delete( aChild);
			aChild->Free();
			}
	}

{
	switch (eventNumber) {

		mButtonHit : 
		
			if ((source == fEdit)) {
				fListView->EachSelectedItemDo( EditChild);
				}				
			else if ((source == fDelete)) {
				fListView->EachSelectedItemDo( MarkChildForRemoval);
				gChildCommandList->Each( RemoveMarkedChildren);
				}				
			/*-----
			else if ((source == fHelp)) {
				aEdit= TEditText( this->FindSubView('tHLP'));
				FailNIL(aEdit);
				aEdit->GetText( helpName); 
				New(aHelpDocument);
				FailNIL(aHelpDocument);
				aHelpDocument->IHelpDocument( helpName, gAppWDRef);
				TPrefApplication(gApplication)->OpenNewDocument(aHelpDocument);
				}			----------*/
			
			else
				inherited::DoEvent( eventNumber, source, event);
			
		Otherwise {
			inherited::DoEvent( eventNumber, source, event);
			}
		}
	
}



//	TChildDialog	-----------------

pascal void TChildDialog::IChildDialog( TChildCommand theCommand, integer theViewID, Handle cmdText, descripText)
VAR
	aScroller	: TScroller;
	aDlog			:	TDialogView;
	dlogRect	: VRect;
	delta			: VPoint;
	Point		minSize, maxSize;
{	
	IPrefWindow();
	fViewID= theViewID;
	fChildCommand= theCommand;
	
	fCommands= TDlogTextView(this->FindSubView('Cmds'));
	FailNIL(fCommands);
	fCommands->IDlogTextView(cmdText);
	
	fDescription= TDlogTextView(this->FindSubView('Desc'));
	FailNIL(fDescription);
	fDescription->IDlogTextView(descripText);
	fHelp= TButton( this->FindSubView('HELP'));
	
	aDlog= TDialogView(this->FindSubView('DLOG'));
	if (aDlog!=NULL) {
		WITH this->fResizeLimits){
			minSize = topLeft;
			maxSize = botRight;
			}
		WITH maxSize)h = Min( 2 + aDlog->fSize.h + kSBarSizeMinus1, h);
		this->SetResizeLimits(minSize, maxSize);
		
			//fix so prefview scroll bar shows
		aDlog->GetExtent(dlogRect);
		dlogRect.bottom= dlogRect.bottom+1;
		ascroller= aDlog->GetScroller(FALSE);
		if ((ascroller!=NULL)) 
		  aScroller->SetScrollLimits(dlogRect.botRight, kRedraw);
		}
}

pascal void TChildDialog::SetPrefID(void) /* override */ 
VAR  sid: str255;
{
	gPrefWindID= gChildDialogID;  
	NumToString( gPrefWindID, sid);
	gPrefWindName= concat('Child-',sid); //!& Need unique gPrefWindName for damn TPrefWind.SaveVPref
}




pascal void TChildDialog::SaveEdits(void)  
VAR
	aEdit		: TEditText;
	aNumber	: TNumberText;
	TChildCommand		aChildCommand;
	TControl		aControl;
	aButton	: TButton;
	Str255		menuName;
	integer		minSeqs;
	Handle		hCmds, hDesc;
{
	aEdit= TEditText( this->FindSubView('MENU'));
	FailNIL(aEdit);
	aEdit->GetText( menuName); 
	
	aNumber= TNumberText( this->FindSubView('iMin'));
	FailNIL(aNumber);
	minSeqs= aNumber->GetValue();
	
		//! Need to save/restore these texts w/ view, but view saver won't touch this:
	hCmds= this->fCommands.(*fHTE)->hText;  
	TPrefApplication(gApplication)->SetPreference('TEXT', fViewID,  hCmds);
	hCmds= TPrefApplication(gApplication)->GetPreference( 'TEXT', fViewID);

	hDesc= this->fDescription.(*fHTE)->hText;  
	TPrefApplication(gApplication)->SetPreference('TEXT', 10000+fViewID,  hDesc);
	hDesc= TPrefApplication(gApplication)->GetPreference( 'TEXT', 10000+fViewID);
	
	fChildCommand->IChildCommand( menuName, fViewID, minSeqs, hCmds, hDesc);
}


pascal void TChildDialog::DisplayForLaunch(void)
VAR
	aView		: TView;
	aEdit		: TEditText;
	aNumber	: TNumberText;
	TChildCommand		aChildCommand;
	TControl		aControl;
	aButton	: TButton;
	Str255		menuName;
	integer		minSeqs;
	Handle		hCmds, hDesc;
	aScroller	: TScroller;
	aDlog			:	TDialogView;
	limitPt		: VPoint;
{

			//revise this Template dialog to a Launch dialog
			//?? or do we just want a second Launch Dialog for display ?
	fWantSave= FALSE;
	this->fProcID= 5; //moveable-modal dlog; was 1?
	this->SetModality(True);  
	this->fIsResizable= FALSE;
	this->fTargetID= 'DLOG';
	this->SetTitle(''); //- SetWTitle(this->fWMgrWindow, '');  

		// make size just large enough for appname, descript, launch && cancel buttons
	aView=  this->FindSubView('SubD');
	//- this->fSize= aView->fSize; 
	SetVpt( limitPt, aView->fSize.h  + kSBarSizeMinus1, aView->fSize.v  + kSBarSizeMinus1);
	if ((aView!=NULL)) this->Resize( limitPt, TRUE);
	 
	aDlog= TDialogView(this->FindSubView('DLOG'));
	if (aDlog!=NULL) {
		aScroller= aDlog->GetScroller(FALSE);
		limitPt= this->fSize;
		limitPt.h= 0; limitPt.v= 0;
		if ((aScroller!=NULL)) 
		  aScroller->SetScrollLimits( limitPt, kDontRedraw);
		aDlog->BeInScroller(NULL); //< no help...
		//- this->RemoveSubView( aScroller);  < causes Fail 
		}

	this->fCommands.fAcceptsChanges= FALSE;
	this->fDescription.fAcceptsChanges= FALSE;
	this->fDescription->ViewEnable( FALSE, kDontRedraw);
		/*^ need to fix fDescription so it displays properly on Launch call
			after NewApp call -- bad save to prefs file ??
		*/
	
	aEdit= TEditText( this->FindSubView('APPL'));
	FailNIL(aEdit);
	aEdit->ViewEnable( FALSE, kDontRedraw);
	//- aEdit->fAdornment= [];
	aEdit->DeleteAdornerByID( kFrameAdorner, kDontRedraw);
	
	aButton= TButton( this->FindSubView('OKAY'));
	FailNIL(aButton);
	aButton->SetText('Launch App', kDontRedraw);
}


pascal void TChildDialog::Edit(void)
VAR		
		IDType		dismisser ;
{
	this->ShowReverted(); /*need for TDlogText... */   
	this->SetModality( True); //!!! or PoseModally will fail !
	dismisser= this->PoseModally(); 
	if (dismisser == 'OKAY') {
		this->fWantSave= TRUE;
		gChildDialogID= fViewID; //! need this in mapp3 version when fWantSave == True !
		this->SaveEdits();  
		}	else {
		this->fWantSave= FALSE;
		}
	this->CloseAndFree();  
}


pascal void TChildDialog::NewChildDialog(Integer itsViewID)
//!??? 2nd NewChild got same ViewID as 1st/existing Child...
VAR		
		menuName	: Str255;
		IDType		dismisser ;
		minSeqs		: integer;
		Handle		hCmds, hDesc;
		TChildCommand		aChildCommand;
		aEdit		: TEditText;
		aNumber	: TNumberText;
		aControl	: TControl;
		aButton		: TButton;
		
			//!? is gList sorted in ascending fViewID order -- then just need gList.Each... ??
		pascal Boolean isIDinUse( aChildCommand:TChildCommand);
		{
			isIDinUse= (aChildCommand->fViewID == itsViewID);
		}
		
{
	if ((itsViewID < kNewChildTemplateID)) {
		itsViewID= kNewChildTemplateID;
		do { itsViewID= itsViewID+1; 
		} while (!((gChildCommandList))->FirstThat( isIDinUse) == NULL);
		}
		
	New(aChildCommand);
	FailNIL( aChildCommand);
	aChildCommand->IChildCommand( 'item', itsViewID, 0, NULL, NIL);

	this->IChildDialog( aChildCommand, itsViewID, NULL, NIL); 
	this->ShowReverted();  //! need for TDlogText...
	
	this->SetModality( True); 
	dismisser= this->PoseModally();
	if (dismisser == 'OKAY') {
		this->fWantSave= TRUE;
		gChildDialogID= fViewID; //! need this in mapp3 version when fWantSave == True !
		this->SaveEdits();  
		gChildCommandList->InsertLast( aChildCommand);
		}		
	else {
		this->fWantSave= FALSE;
		aChildCommand->Free();
		}
		
	this->CloseAndFree();  
}


pascal void TChildDialog::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
VAR
	TEditText		aEdit;
	THelpDocument		aHelpDocument;
	Str255		helpName;
	fi			: FailInfo;

	pascal void HdlFail(integer error,  long message)
	{
		TObject(aHelpDocument)= FreeIfObject(aHelpDocument); 
		//- FailNewMessage(error, message, messageInitializationFailed);  
	}
	
{
	switch (eventNumber) {

		mButtonHit : 
			if ((source == fHelp)) {
				aHelpDocument= NULL;
				aEdit= TEditText( this->FindSubView('tHLP'));
				FailNIL(aEdit);
				aEdit->GetText( helpName); 
				New(aHelpDocument);
				FailNIL(aHelpDocument);
				CatchFailures(fi, HdlFail);
				aHelpDocument->IHelpDocument( helpName, gAppWDRef);
				TPrefApplication(gApplication)->OpenNewDocument(aHelpDocument);
				Success(fi);
				}			else
				inherited::DoEvent( eventNumber, source, event);
			
		Otherwise {
			inherited::DoEvent( eventNumber, source, event);
			}
		}
}


pascal void TChildDialog::Launch( TSeqList selectedSeqs)
VAR
	aChildApp	: TChildApp;
	IDType		dismisser, aNam ;
	outnum		: integer;
	boolean		NoMoreOutputs;
	aStr			: Str255;
	integer		curVolRef, aFileRef, err, nseqs;
	childSN		: ProcessSerialNumberPtr;
	count			: longint;
	hbuf			: Handle;
	
	Str63		aCommandFile ;
	aInFormat, aReturnAction	: integer;
	aChildFile	: TChildFile;
	theFiles		: TList;
	okind				: OSType;
	
	aPopup	: TPopup;
	aEdit		: TEditText;
	aRadio	: TRadio;
	
	integer		nDocList;
	docList	: AEDescList;
	fi			: FailInfo;
	
	pascal void HdlLaunchFail(integer error,  long message)
	{
		if ((theFiles!=NULL)) theFiles->FreeList();  //frees childfile objects << erase files?! && list
		this->CloseAndFree(); // < may be causing bombs ??
		//- FailNewMessage(error, message, messageInitializationFailed); 
			// this is MacApp err STR# 130 ^^^^, need own STR# errlist?
	}

{
	theFiles= NULL;
	this->DisplayForLaunch();
  this->ShowReverted();   //need for TDlogText...
	this->fWantSave= FALSE;  
	this->SetModality( True); 
	dismisser = this->PoseModally();
	
	CatchFailures(fi, HdlLaunchFail);
	
	if (dismisser == 'OKAY') {		
		nDocList= 0;
		theFiles= NewList;
		FailNIL( theFiles);
				
		err= GetVol(NULL, curVolRef);  //!!?? need working dir here ??
				//! put us into App file location so we find subfolders...
		err= SetVol( NULL, gAppWDRef);
																		// Look into aliases !!!
		aEdit= TEditText(this->FindSubView('iNam'));
		if (!((aEdit=NULL) || (selectedSeqs=NULL) || (selectedSeqs.GetSize<1))) {
			aEdit->GetText(aStr); 
			if (Length(aStr) > 0) {
				New( aChildFile); 
				FailNIL( aChildFile);
				aChildFile->IChildFile( aStr, 'TEXT', gAppWDRef, kChildReturnNoAction, 
																			kIsInput, kDoDelete);
				theFiles->InsertLast( aChildFile);
				
				aInFormat = kPearson;  
				aPopup= TPopup(this->FindSubView('iFmt'));
				if ((aPopup!=NULL)) aInFormat= aPopup->GetCurrentItem();
			
				selectedSeqs->doWrite( aStr, aInFormat);
				
				ChildOpenAddFile( (nDocList=0), aStr, gAppWDRef, docList);
				nDocList= nDocList+1;
				}
			}

					//!!! NOTE: We now have many subviews w/ name 'oNam'/'oSeq'...
					// Must cycle thru them / clusterize them? / && get all return files 
	outnum= 0;
	do {
		outnum= outnum+1;
		aNam= '1Nam';
		aNam[1]= ((char)(ord('0') + outnum));
		aEdit= TEditText(this->FindSubView(aNam));
		noMoreOutputs= aEdit == NULL;
		if ((!noMoreOutputs)) {
			aEdit->GetText(aStr);   
			if (Length(aStr) > 0) {				
				aNam= '1Typ';
				aNam[1]= ((char)(ord('0') + outnum));
				aPopup= TPopup(this->FindSubView(aNam));
				if ((aPopup=NULL)) aReturnAction= kChildReturnOpenSeqFile
				else aReturnAction= aPopup.GetCurrentItem - 1;
				switch (aReturnAction) {
					kChildReturnOpenPictFile: okind= 'PICT';
					otherwise okind= 'TEXT';
					}
				
				if (aReturnAction != kChildReturnNoAction) {
					New( aChildFile); 
					FailNIL( aChildFile);
					aChildFile->IChildFile( aStr, okind, gAppWDRef, aReturnAction, kIsOutput, 
													!kDoDelete);
					theFiles->InsertLast( aChildFile);
					}
				}
			}
		} while (!(NoMoreOutputs)); 
		
		aEdit= TEditText(this->FindSubView('CmdF'));
		if ((aEdit!=NULL)) {
			aEdit->GetText(aStr);   
			if (Length(aStr) > 0) {
				aCommandFile= aStr;
				New( aChildFile); 
				FailNIL( aChildFile);
				aChildFile->IChildFile( aCommandFile, 'TEXT', gAppWDRef, 
															kChildReturnNoAction, kIsInput, kDoDelete);
				theFiles->InsertLast( aChildFile);
				
				hbuf= this->fCommands.(*fHTE)->hText;
				HLock( hbuf);
				count= GetHandleSize( hbuf);
				err= FSDelete( aCommandFile, 0);   
				FailOSErr( Create( aCommandFile, 0, kSAppSig/*?*/, 'TEXT'));
				FailOSErr( FSOpen( aCommandFile, 0, aFileRef));
				FailOSErr( FSWrite( aFileRef, count, (*hbuf)));
					//make sure there is a CR at end of line/file or cmdfile may fail
				aStr[0]= ((char)(13)); count= 1;
				FailOSErr( FSWrite( aFileRef, count, &aStr[0]));
				FailOSErr( FSClose( aFileRef));
				HUnlock( hBuf);		

				ChildOpenAddFile( (nDocList=0), aCommandFile, gAppWDRef, docList);
				nDocList= nDocList+1;
				}
			}

		
		childSN= NULL;
		aEdit= TEditText(this->FindSubView('APPL'));
		if ((aEdit!=NULL)) {
			aEdit->GetText(aStr);   
			if (Length(aStr) > 0) {
				if ((nDocList<1))  
					childSN= LaunchChildApp( gAppWDRef, 0, aStr, NULL)
				else
					childSN= LaunchChildApp( gAppWDRef, 0, aStr, ChildOpenLaunchParams( docList));
				}
			}
			
		if ((childSN == NULL)) {
			gApplication->Beep(1);
			//LaunchChildApp should Fail w/ messages for type of failure (app not found,...) 
			theFiles->FreeList();  //frees childfile objects << erase files?! && list
			}		else {
			New(aChildApp);
			FailNIL(aChildApp);
			aChildApp->IChildApp( childSN, theFiles);
			gChildList->InsertLast( aChildApp);
			//message: launched okay...
			}
			
		err= SetVol( NULL, curVolRef);
		}
		
	Success(fi);
	this->CloseAndFree();
}







//	TChildCommand	=================================


pascal void TChildCommand::IChildCommand( str63 name, integer viewID, minSeqs,
									Handle aCommandsTxt, aDescriptionTxt)
{
	fMenuName= name;
	fViewID= viewID;
	fMinSeqs= MinSeqs;
	fCommandsTxt= aCommandsTxt;
	fDescriptionTxt= aDescriptionTxt;
}


pascal void TChildCommand::EnableMenu( MenuHandle aMenuHandle, integer nSelectedSeqs)
VAR  i, atMenuItem: integer;
		 boolean		found;
		 Str255		aStr;
{
	found= FALSE;
	FOR i= 1 to CountMITems( aMenuHandle)){
		GetItem( aMenuHandle, i, aStr);
		if ((aStr == fMenuName)) {
			found= TRUE;
			atMenuItem= i;
			LEAVE();
			}
		}
	if (!found) {
		aStr= fMenuName;
		AppendMenu(aMenuHandle, aStr);
		atMenuItem= CountMITems( aMenuHandle); 
		GetItem( aMenuHandle, atMenuItem, aStr);
		fMenuName= aStr; //make sure -- MenuMgr drops "(" and others
		}
		
	if ((nSelectedSeqs >= fMinSeqs) && gHasAppleEvents) {
		if ((atMenuItem <= 31)) EnableItem(aMenuHandle, atMenuItem); 
		}
}


pascal void TChildCommand::LaunchDialog( TSeqList selectedSeqs)
VAR
	TChildDialog		aChildDlog;
{
	gChildDialogID= fViewID;
	aChildDlog = TChildDialog(
		gViewServer->NewTemplateWindow( kNewChildTemplateID/*fViewID*/, NULL));
		// 18may93:!! TRICK: ^^ don't use fViewID here which is just for VPref, not View now !!!
		// BUT do use fViewID for DoPostCreate w/ View, to read in specific values...
	FailNIL(aChildDlog); 		 
	aChildDlog->IChildDialog( this, fViewID, fCommandsTxt, fDescriptionTxt);
	aChildDlog->Launch( selectedSeqs);
}

pascal void TChildCommand::Edit(void)
VAR
	TChildDialog		aChildDlog;
{
	gChildDialogID= fViewID;
	aChildDlog = TChildDialog( 
		gViewServer->NewTemplateWindow( kNewChildTemplateID/*fViewID*/, NULL));
		// 18may93:!! TRICK: ^^ don't use fViewID here which is just for VPref, not View now !!!
		// BUT do use fViewID for DoPostCreate w/ View, to read in specific values...
	FailNIL(aChildDlog); 
	aChildDlog->IChildDialog( this, fViewID, fCommandsTxt, fDescriptionTxt);
	aChildDlog->Edit();
}




pascal void OpenNewChild(void)
VAR  aChildDlog : TChildDialog;  
{
	if (!gHasAppleEvents) {
		ParamText('System-7 && AppleEvents are not installed',
							'', 'Children','');	
		Failure( -1, msgMyError);  //takes us out of proc...
		Exit(OpenNewChild);
		}
		
	aChildDlog = TChildDialog(
		gViewServer->NewTemplateWindow(kNewChildTemplateID, NULL));
	FailNIL(aChildDlog); 		 
	aChildDlog->NewChildDialog( 0);  
	// gChildDialogID= aChildDlog->fViewID;??
}



pascal void InitChildApps( boolean hasAppleEvents)  //call from gApplication.IApplication
CONST
	MaxViewID ==  kNewChildTemplateID+200;
VAR
	integer		nextViewID;
	TChildDialog		aChildDlog;
	Str255		menuName;
	minSeqs	: integer;
	TChildCommand		aChildCommand;
	h			: Handle;
	aCommandsTxt, 
	aDescriptionTxt	: Handle;
	TEditText		aEdit;
	TNumberText		aNumber;
	
{
	gHasAppleEvents= hasAppleEvents;
	if (!hasAppleEvents) {
		gChildList= NULL;
		gChildCommandList= NULL;
		exit(InitChildApps);
		}
		
	gChildList= NewList;
	FailNIL(gChildList);
	gChildCommandList= NewList;
	FailNIL(gChildCommandList);
	
	//! Read ChildCommandList from Prefs resources....
	// views start at kNewChildTemplateID+1 

	nextViewID= kNewChildTemplateID; /*getsize=0*/ 
	do {
		nextViewID= nextViewID + 1;
		h= GetResource(kPrefViewType, nextViewID);
		if (h!=NULL) {
			//?? ReleaseResource( h); ??
			gChildDialogID= nextViewID; 
				//!! ^^ 18may93: so TChildDialog/TPrefs->DoPostCreate() reads right vals
			aChildDlog = TChildDialog(
				gViewServer->NewTemplateWindow(kNewChildTemplateID/*nextViewID*/, NULL));
				//!! 18may93: TRICK: ^^ don't use nextViewID here which is just for VPref, not View now !!!
				// BUT do use nextViewID=gChildDialogID for DoPostCreate w/ View, to read in specific values...
			FailNIL(aChildDlog);
			aChildDlog->IChildDialog( NULL, nextViewID, NULL, NIL); /*<< use aCommandsTxt,aDescText for NILs?*/ 
			
			NumToString( nextViewID, menuName);
			menuName= concat('Task ',menuName);
			aEdit= TEditText( aChildDlog->FindSubView('MENU'));
			if ((aEdit!=NULL)) aEdit->GetText( menuName); 
	
			minSeqs= 0;
			aNumber= TNumberText( aChildDlog->FindSubView('iMin'));
			if ((aNumber!=NULL)) minSeqs= aNumber->GetValue();
			
			aCommandsTxt	 = 
				TPrefApplication(gApplication)->GetPreference( 'TEXT', nextViewID);
			aDescriptionTxt= 
				TPrefApplication(gApplication)->GetPreference( 'TEXT', 10000+nextViewID);

			New(aChildCommand);
			FailNIL( aChildCommand);
			aChildCommand->IChildCommand( menuName, nextViewID, minSeqs, 
																			aCommandsTxt, aDescriptionTxt);
			gChildCommandList->InsertLast( aChildCommand);
		
			aChildDlog->fWantSave= FALSE;	
			aChildDlog->Free(); //? or close? but never opened...
			}
	} while (!((nextViewID >= MaxViewID))); 
					//^^ look thru 100 id's, allow for removals
					
	if (gDeadStripSuppression) {
		if (Member(TObject(NULL), TChildListView));
		if (Member(TObject(NULL), TEditChildWind));
		if (Member(TObject(NULL), TChildCommand));
		if (Member(TObject(NULL), TChildDialog));
		if (Member(TObject(NULL), TChildApp));
		}
		
}


pascal void OpenEditChild(void)
VAR aWind	: TEditChildWind; boolean isNowOpen,

	pascal void checkOpenWind( TWindow aWind)
	{
		if (Member(TObject(aWind), TEditChildWind)) { 
			aWind->Select(); 
			isNowOpen= TRUE;
			}
	}
{
	isNowOpen= FALSE;
	gApplication->ForAllWindowsDo(checkOpenWind);
	if (!isNowOpen) {
		aWind = TEditChildWind(gViewServer->NewTemplateWindow(kEditChildWindID, NULL));
		FailNIL(aWind); 		 
		aWind->IEditChildWind();  
		aWind->ShowReverted();   
		aWind->Open();  
		}
}
