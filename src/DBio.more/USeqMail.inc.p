{ USeqMail.inc.p -- SMTP SendMail and POP CheckMail }
{ Copyright 1992 by d.g.gilbert, for SeqApp }

{$S USeqMail }

CONST
	
	kSendmailWindID = 2103;			 
	kBugmailWindID 	= 2106;	 
	kGBFetchWindID	= 2105;
	kEMBLFetchWindID	= 2107;
	kMailPrefWindID = 2108;
	kPopMailWindID = 2102;
	kCheckMailWindID = kPopMailWindID;
	
	kGBFastaWindID = 2110;
	kGBBlastWindID = 2111;

	kGeneIDWindID = 2112;
	kGrailWindID = 2113;
	kUHServerWindID = 2114;

	kNCBIBlastWindID = 2115;
	kNCBIFetchWindID	= 2116;
	kGenmarkWindID = 2118;
	kBlocksWindID = 2119;
	kPythiaWindID = 2120;
	

TYPE

	{ These guys are known only here ... need not be public}
	
	TBugmailWindow	= OBJECT (TSendmailWindow)
		fSendUpdate	: TCheckBox;
		PROCEDURE TBugmailWindow.ISendmailWindow; OVERRIDE;
		PROCEDURE TBugmailWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
		PROCEDURE TBugmailWindow.Sendit; OVERRIDE;
		END;
		
	TGBFetchWindow	= OBJECT (TSendmailWindow)
		PROCEDURE TGBFetchWindow.ISendmailWindow; OVERRIDE;
		END;
					
	TEMBLFetchWindow	= OBJECT (TSendmailWindow)
		PROCEDURE TEMBLFetchWindow.ISendmailWindow; OVERRIDE;
		END;

	TUHServerWindow	= OBJECT (TSendmailWindow)
		PROCEDURE TUHServerWindow.ISendmailWindow; OVERRIDE;
		END;
 
	TGeneidWindow	= OBJECT (TSendmailWindow)
		fUseNetgene: TCheckBox;
		PROCEDURE TGeneidWindow.IGeneidWindow( aSeq: TSequence);
		PROCEDURE TGeneidWindow.Sendit; OVERRIDE;
		END;

	TGenmarkWindow	= OBJECT (TSendmailWindow)
		fPSGraph: TCheckBox;
		fMarkovOrder: TNumberText;
		fWinStep	: TNumberText;
		fThreshold: TNumberText;
		fAnalWin	: TNumberText;
		fPSTitle: TEditText;	
		
		PROCEDURE TGenmarkWindow.IGenmarkWindow( aSeq: TSequence);
		PROCEDURE TGenmarkWindow.Sendit; OVERRIDE;
		PROCEDURE TGenmarkWindow.SetPrefID;  OVERRIDE; 
		END;

	TBlocksWindow	= OBJECT (TSendmailWindow)
		PROCEDURE TBlocksWindow.IBlocksWindow( aSeq: TSequence);
		PROCEDURE TBlocksWindow.Sendit; OVERRIDE;
		END;
		
	TGrailWindow	= OBJECT (TSendmailWindow)
		fMailID	: TEditText;
		fNumSeqs: integer;
		
		PROCEDURE TGrailWindow.IGrailWindow( aSeqList: TSeqList);
		PROCEDURE TGrailWindow.Sendit; OVERRIDE;
		PROCEDURE TGrailWindow.SetPrefID;  OVERRIDE; 
		END;

	TPythiaWindow	= OBJECT (TSendmailWindow)
		fDoAlu	: TRadio;
		fDoRpts	: TRadio;
		
		PROCEDURE TPythiaWindow.IPythiaWindow( aSeqList: TSeqList);
		PROCEDURE TPythiaWindow.Sendit; OVERRIDE;
		PROCEDURE TPythiaWindow.SetPrefID;  OVERRIDE; 
		END;

	TGBFastaWindow	= OBJECT (TSendmailWindow)
		fScore	: TNumberText;
		fAligns	: TNumberText;
		fDatabank	: TPopUp;
		fAmino, fNucleic : TRadio;		
		
		PROCEDURE TGBFastAWindow.IGBFastAWindow( aSeq: TSequence);
		PROCEDURE TGBFastAWindow.Sendit; OVERRIDE;
		PROCEDURE TGBFastAWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
		PROCEDURE TGBFastAWindow.SetPrefID;  OVERRIDE; 
		END;

	TGBBlastWindow	= OBJECT (TSendmailWindow)
		fDatabank	: TPopUp;
		fAmino, fNucleic : TRadio;		
		
		PROCEDURE TGBBlastWindow.IGBBlastWindow( aSeq: TSequence);
		PROCEDURE TGBBlastWindow.Sendit; OVERRIDE;
		PROCEDURE TGBBlastWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
		PROCEDURE TGBBlastWindow.SetPrefID;  OVERRIDE; 
		END;

	TNCBIBlastWindow	= OBJECT (TSendmailWindow)
		fDatabank	: TPopUp;
		fMethod		: TPopUp;
		fAmino, fNucleic : TRadio;		
		fDesc, fAlign	: TNumberText;
		fExcut, fICut	: TEditText;
		fHistogram: TCheckBox;
		
		PROCEDURE TNCBIBlastWindow.INCBIBlastWindow( aSeq: TSequence);
		PROCEDURE TNCBIBlastWindow.SetMenus( redraw: boolean);
		PROCEDURE TNCBIBlastWindow.Sendit; OVERRIDE;
		PROCEDURE TNCBIBlastWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
		PROCEDURE TNCBIBlastWindow.SetPrefID;  OVERRIDE; 
		END;

	TNCBIFetchWindow	= OBJECT (TSendmailWindow)
		fDatabank	: TPopUp;
		fNDocs, fNLines, fStartDoc	: TNumberText;
		
		PROCEDURE TNCBIFetchWindow.INCBIFetchWindow;
		PROCEDURE TNCBIFetchWindow.Sendit; OVERRIDE;
		PROCEDURE TNCBIFetchWindow.SetPrefID;  OVERRIDE; 
		END;




PROCEDURE CryptPassword(VAR password: Str255);
VAR  	s		: Str255;  
			len, i, j : integer;
BEGIN {// from Nuntius, thanks P. Speck}
	s:= '01';  { version number }
	len:= Length(password);
	j:= Length(s);
	for i:= 1 to len do begin
		j:= j+1; s[j]:= chr(97 + bAnd(Random, 15)); 			{s += char(97 + (Random() & 15));}
		j:= j+1; s[j]:= chr(bXor( ord(password[i]), 9)); 	{s += password[i] ^ 9;}
		j:= j+1; s[j]:= chr(107 + bAnd(Random, 15)); 			{s += char(107 + (Random() & 15));}
		END;
	s[0]:= chr(j);
	password:= s;
END;

FUNCTION DeCryptPassword(VAR password: Str255): Boolean;
VAR  	s		: Str255;  
			len, i, j : integer;
BEGIN {// from Nuntius, thanks P. Speck}
	s:= password;
	len:= Length(s);
	password:= '';
	DeCryptPassword:= false; 
	if (len < 2) THEN EXIT(DeCryptPassword); {missing version number}
	if (s[1] <> '0') | (s[2] <> '1') THEN EXIT(DeCryptPassword); { unknown version }
	i:= 4; j:= 0;
	while (i < len) do begin
		j:= j+1;
		password[j]:= chr( bXor(ord(s[i]), 9) );  { pass += s[i] ^ 9; }
		i:= i+3; 
		END;
	password[0]:= chr(j);
	DeCryptPassword:= true;
END;



{	TMailPrefWindow	-----------------}

PROCEDURE TMailPrefWindow.IMailPrefWindow;
VAR   aStr: Str255;
BEGIN	
	IPrefWindow;
  fFrom   	:= TPrefEditText(FindSubView('MAIL'));  
  fPOPAddress	:= TPrefEditText(FindSubView('POPP'));  
  fSMTPHost	:= TPrefEditText(FindSubView('SMTP'));  
  fPassword := THiddenText(FindSubView('PASS'));  
  fKeepPass := TCheckBox(FindSubView('cPAS'));  

	fPassword.GetText( aStr);  
	if (DeCryptPassword(aStr)) THEN ;  
	fPassword.SetText( aStr, kDontRedraw);  
END;


PROCEDURE TMailPrefWindow.SetGlobals;  
VAR		aStr: Str255;
BEGIN
	fFrom.GetText( aStr); gMyMailAddress:= aStr;
	fSMTPHost.GetText( aStr); gSMTPHost:= aStr;
	fPOPAddress.GetText( aStr); gMyPOPAddress:= aStr;
	fPassword.GetText( aStr); gPassword:= aStr;
END;

PROCEDURE TMailPrefWindow.ModalDialog;  
VAR		
		aStr: Str255;
		dismisser : IDType;
BEGIN
	SELF.SetModality( True); {!!! or PoseModally will fail !}
	dismisser := SELF.PoseModally;
	IF dismisser = 'OKAY' THEN BEGIN
		fWantSave:= TRUE;
		SetGlobals;
		fPassword.GetText( aStr);  
		CryptPassword(aStr); {do this so no visible password in Prefs file}
		fPassword.SetText( aStr, kDontRedraw);  
		END
	ELSE BEGIN
		fWantSave:= FALSE;	
		END;
	IF NOT fKeepPass.isOn THEN fPassword.SetText('',FALSE);		
END;

PROCEDURE TMailPrefWindow.SetPrefID;  
BEGIN
	gPrefWindID:= kMailPrefWindID; gPrefWindName:= 'TMailPrefWindow';
END;


 






FUNCTION BadSMTPAddresses: Boolean;
BEGIN
	IF (length(gSMTPHost) = 0) | (gSMTPHost[1] = '»')
	 | (length(gMyMailAddress)=0) | (gMyMailAddress[1] = '»') THEN
		OpenMailPref;

	BadSMTPAddresses:= (length(gSMTPHost) = 0) | (gSMTPHost[1] = '»')
		 | (length(gMyMailAddress)=0) | (gMyMailAddress[1] = '»');
END;


{ TSendmailWindow ------------------------}

CONST
	mSendit = 2103;
	mHelp		= 2104;
	
PROCEDURE TSendmailWindow.MakeSMTP( theTo, theSubject, theCCopy: Str255; 
													theMessage: Handle; isTEHandle: Boolean); 
VAR
	aSMTP	: TSMTP;
BEGIN
	New(aSMTP);
	FailNIL(aSMTP);
	aSMTP.ISMTP( gSMTPHost);
	aSMTP.fMailerID:= gMailerID;
	aSMTP.InstallMessageLine( fStatus); 
	IF isTEHandle THEN
		aSMTP.SendMailTE( theTo, gMyMailAddress, theSubject, theCCopy, TEHandle(theMessage))
	ELSE
		aSMTP.SendMail( theTo, gMyMailAddress, theSubject, theCCopy, theMessage);
	aSMTP.Free;
END;

PROCEDURE TSendmailWindow.Sendit; 
VAR
	theTo, theSubject, theCCopy: str255;
BEGIN
	IF BadSMTPAddresses THEN EXIT(Sendit);
		
	IF (fTo<>NIL) THEN fTo.GetText( theTo) ELSE EXIT(SendIt);
	IF (fSubject<>NIL) THEN fSubject.GetText( theSubject) ELSE theSubject:= '';
	IF (fCopySelf<>NIL) & (fCopySelf.IsOn) THEN theCCopy:= gMyMailAddress 
	ELSE theCCopy:= '';
	MakeSMTP( theTo, theSubject, theCCopy, Handle(fSendmailView.fHTE), TRUE);
END;

PROCEDURE TSendmailWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
VAR
		aHelpFile: TStaticText;
		aHelpDocument: THelpDocument;
		aStr		: Str255;
		fi			: FailInfo;

	PROCEDURE HdlFail(error: integer;  message: LONGINT);
	BEGIN
		TObject(aHelpDocument):= FreeIfObject(aHelpDocument); 
	END;
	
BEGIN
	CASE eventNumber OF
		mSendit	: BEGIN 
				Sendit; 
				CloseAndFree; 
				END;
		
		mHelp	: BEGIN
				aHelpFile:= TStaticText(FindSubView('tHlp'));
				IF aHelpFile<>NIL THEN BEGIN
					aHelpDocument:= NIL;
					aHelpFile.GetText( aStr);
					New(aHelpDocument);
					FailNIL(aHelpDocument);
					CatchFailures(fi, HdlFail);
					aHelpDocument.IHelpDocument( aStr, gAppWDRef);
					TPrefApplication(gApplication).OpenNewDocument(aHelpDocument);
					Success(fi);
					END;
				END;
						
		mCheckboxHit: 
		  IF (fUnlockTexts<>NIL) & (source=fUnlockTexts) THEN BEGIN
				IF fUnlockTexts.IsOn THEN UnlockTexts
				ELSE RelockTexts;
				END; 

		Otherwise
			INHERITED DoEvent(eventNumber, source, event);
		END;
END;

		
PROCEDURE TSendmailWindow.ISendmailWindow;
VAR
	aSendBut: TButton;
	aFrom		: TEditText;
BEGIN	
	IPrefWindow;
	fSendmailView:= TDlogTextView(FindSubView('MAIL'));
	IF (fSendmailView<>NIL) THEN fSendmailView.IDlogTextView(NIL);
  fTo 			:= TEditText(FindSubView('eTo:'));  
  fSubject	:= TEditText(FindSubView('eSb:'));  
	fStatus		:= TStaticText(FindSubView('tSTS'));
	IF (fStatus<>NIL) THEN fStatus.SetText( '', FALSE);
  fCopySelf	:= TCheckBox(FindSubView('cCPY'));  
  aFrom			:= TEditText(FindSubView('eFm:')); { used as Static Text }  
	IF (aFrom<>NIL) THEN aFrom.SetText( gMyMailAddress, TRUE);
	aSendBut:= TButton(FindSubView('OKAY'));
	IF (aSendBut<>NIL) then aSendBut.fEventNumber:= mSendit;
	aSendBut:= TButton(FindSubView('Help'));
	IF (aSendBut<>NIL) THEN aSendBut.fEventNumber:= mHelp;
	fUnlockTexts:= TCheckBox(FindSubView('Unlk'));
END;

PROCEDURE TSendmailWindow.OpenWindow;
BEGIN
	SELF.ISendmailWindow;
	SELF.ShowReverted; 
	SELF.Open; 
	SELF.Select;	 
END;
		

PROCEDURE UnlockEditText( aTxt: TEditText);
BEGIN
	IF (aTxt<>NIL) & (NOT aTxt.WantsToBeTarget) THEN BEGIN
		{re-adorn w/ shadow box ?}
		aTxt.ViewEnable(TRUE, TRUE);
		aTxt.fMaxChars:= 251; {tricky key that we were locked}
		END;
END;

PROCEDURE LockEditText( aTxt: TEditText; force: boolean);
BEGIN
	IF (aTxt<>NIL) & ((aTxt.fMaxChars = 251) | force) THEN BEGIN
		{re-adorn w/ shadow box ?}
		aTxt.StopEdit;
		aTxt.ViewEnable(FALSE, TRUE);
		aTxt.fMaxChars:= 255;  
		END;
END;

PROCEDURE TSendmailWindow.UnlockTexts;
BEGIN
	UnlockEditText( fTo);
  UnlockEditText( fSubject);
END;

PROCEDURE TSendmailWindow.RelockTexts;
BEGIN
	LockEditText( fTo, FALSE);
  LockEditText( fSubject, FALSE);
END;



PROCEDURE TSendmailWindow.SetPrefID;  
BEGIN
	gPrefWindID:= kSendmailWindID; gPrefWindName:= 'TSendmailWindow';
END;




{ TBugmailWindow ------------}

	
PROCEDURE TBugmailWindow.ISendmailWindow;
BEGIN
	INHERITED ISendmailWindow;
	fWantSave:= FALSE;
END;

PROCEDURE TBugmailWindow.Sendit; OVERRIDE;
VAR
	theTo, theSubject, theCCopy: str255;
	theMessage: TEHandle;
	strHand: StringHandle;
	len	: integer;
BEGIN
	IF BadSMTPAddresses THEN EXIT(Sendit);
		
	IF (fTo<>NIL) THEN fTo.GetText( theTo) ELSE EXIT(SendIt);
	IF (fSubject<>NIL) THEN fSubject.GetText( theSubject) ELSE theSubject:= '';
	IF (fCopySelf<>NIL) & (fCopySelf.IsOn) THEN theCCopy:= gMyMailAddress 
	ELSE theCCopy:= '';

	MakeSMTP( theTo, theSubject, theCCopy, Handle(fSendmailView.fHTE), TRUE);
END;

PROCEDURE TBugmailWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
BEGIN
	CASE eventNumber OF
		mSendit			: BEGIN 
			Sendit; 
			CloseAndFree; 
			END;
		
		Otherwise
			INHERITED DoEvent(eventNumber, source, event);
		END;
END;



PROCEDURE TGBFetchWindow.ISendmailWindow;
BEGIN
	INHERITED ISendmailWindow;
	fWantSave:= FALSE;
END;
			
PROCEDURE TEMBLFetchWindow.ISendmailWindow;
BEGIN
	INHERITED ISendmailWindow;
	fWantSave:= FALSE;
END;


PROCEDURE TUHServerWindow.ISendmailWindow;
BEGIN
	INHERITED ISendmailWindow;
	fWantSave:= FALSE;
END;

{----
PROCEDURE THiddenText.DoSubstitution(VAR theText: Str255); OVERRIDE;
VAR  i	: INTEGER;
BEGIN
	FOR i:= 1 to length(theText) DO theText[i]:= '•';
END;

PROCEDURE THiddenText.StopEdit; OVERRIDE;
BEGIN
	INHERITED StopEdit;
	ForceRedraw;  
END;
---}



						{!! This should become enclosed in a Document .. }

{ TPopmailWindow ------------------------}

CONST
	mPopNext 	= 2102;
	mPopLast 	= mPopNext+1;
	mPopDelete = mPopNext+2;
	mPopSave	= mPopNext+3;
	
PROCEDURE TPopmailWindow.IPopmailWindow;
BEGIN	
	IPrefWindow; 
	fWantSave:= TRUE;
	fMailView:= TDlogTextView(FindSubView('MAIL'));
	IF (fMailView<>NIL) THEN fMailView.IDlogTextView(NIL);
	fPOP	:= NIL;
	
	fMsgNum:= 0;
	
  fFrom			:= TEditText(FindSubView('eFm:'));    
  fSubject	:= TEditText(FindSubView('eSb:'));  
	fStatus		:= TStaticText(FindSubView('tSTS'));
	IF (fFrom<>NIL) THEN fFrom.SetText( '', FALSE);
	IF (fSubject<>NIL) THEN fSubject.SetText( '', FALSE);
	IF (fStatus<>NIL) THEN fStatus.SetText( '', FALSE);

	fNextBut	:= TButton(FindSubView('NEXT'));
	FailNIL( fNextBut);
	fNextBut.fEventNumber:= mPopNext;
	fLastBut	:= TButton(FindSubView('LAST'));
	FailNIL( fLastBut);
	fLastBut.fEventNumber:= mPopLast;
	fDelBut	:= TButton(FindSubView('bDEL'));
	fSaveBut	:= TButton(FindSubView('bSAV'));
	IF (fDelBut<>NIL) then fDelBut.fEventNumber:= mPopDelete;
	IF (fSaveBut<>NIL) then fSaveBut.fEventNumber:= mPopSave;

	fNextBut.ViewEnable( TRUE,  kDontRedraw);
	fNextBut.Dimstate( FALSE,  kDontRedraw);
	
	fDelCheck	:= TCheckBox(FindSubView('cDEL'));
  fCountTxt	:= TStaticText(FindSubView('tCNT'));  
END;


PROCEDURE TPopmailWindow.SetPrefID; OVERRIDE;  
BEGIN
	gPrefWindID:= kPopMailWindID; gPrefWindName:= 'TPopmailWindow';
END;


	
PROCEDURE TPopmailWindow.SaveMail; 
VAR
		where	: point;
		reply	: SFReply;
		h			: Handle;
		fileref, err: integer;
		count	: longint;
		aStr	: Str255;
BEGIN
	fSubject.GetText(aStr);
	reply.fName:= aStr;
	CenterSFPut( where);
	SFPutFile( where, 'Save message as:', reply.fName, NIL, reply);
	gApplication.UpdateAllWindows;
	IF reply.good THEN BEGIN		
		IF 0 <> FSDelete( reply.fName, reply.vRefnum) THEN ;
		FailOSErr( Create( reply.fName, reply.vRefnum, 'EDIT', 'TEXT'));
		FailOSErr( FSOpen( reply.fName, reply.vRefnum, fileRef));
		h:= fMailView.fTextData.fChars;
		IF (h<>NIL) THEN BEGIN
			count:= max( 0, GetHandleSize(h) ); {-1? drop 0 at end }
			HLock( h);
			FailOSErr( FSWrite( fileRef, count, h^));
			HUnLock( h);
			END;
		err:= FSClose( fileRef);
		END;
END;



PROCEDURE TPopmailWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
VAR	aPOP: TPOP;
BEGIN
	CASE eventNumber OF
		mPopNext	: IF (fPOP=NIL) THEN OpenPop
								ELSE IF FetchOne(fMsgNum+1) THEN ;
								
		mPopLast	: IF (fPOP=NIL) THEN OpenPop
								ELSE IF FetchOne(fMsgNum-1) THEN ;
		
		mPopDelete: IF (fPOP<>NIL) THEN BEGIN 
					TPOP(fPOP).DeleteMail( fMsgNum); 
					END;
					
		mPopSave: SaveMail; 
				
		mCheckboxHit: IF (source = fDelCheck) THEN BEGIN 
			IF fDelCheck.IsOn & NOT fDelBut.IsDimmed THEN BEGIN
				fDelBut.ViewEnable( FALSE, NOT kRedraw);
				fDelBut.DimState( TRUE, kRedraw);
				END
			ELSE IF NOT fDelCheck.IsOn & fDelBut.IsDimmed THEN BEGIN
				fDelBut.ViewEnable( TRUE, NOT kRedraw);
				fDelBut.DimState( FALSE, kRedraw);
				END;
			END;
			
		Otherwise
			INHERITED DoEvent(eventNumber, source, event);
		END;
END;


PROCEDURE TPopmailWindow.OpenPop; 
VAR	aPOP	: TPOP;
BEGIN
	IF (length(gPassword)=0) | (gPassword[1] = '»') 
	 | (length(gMyPOPAddress)=0) | (gMyPOPAddress[1] = '»')THEN
		OpenMailPref;
		
	IF (length(gPassword)=0) | (gPassword[1] = '»') 
	 | (length(gMyPOPAddress)=0) | (gMyPOPAddress[1] = '»')THEN
		EXIT(OpenPop);

{- gDebugBell:= TRUE;}
{- DebugMsg('TPopmailWindow New IPOP');}
	
	New(aPOP); 
	FailNIL(aPOP);
	aPOP.IPOP( gMyPOPAddress, gPassword);
	aPOP.InstallMessageLine( fStatus); 
	aPOP.ResetHost( gMyPOPAddress, gPassword);  {or aPop.ReOpen; } 
	TPOP(fPOP):= aPOP;

{- DebugMsg('TPopmailWindow done IPOP');}

	IF FetchOne(1) THEN ;
END;


PROCEDURE TPopmailWindow.Close; OVERRIDE;
VAR 	fi: failInfo;
		PROCEDURE HdlFail(error: OSErr; message: LONGINT);
		BEGIN
			INHERITED Close;
		END;
BEGIN
	CatchFailures(fi, HdlFail);
	TObject(fPOP):= FreeIfObject( fPOP);
	Success(fi);
	INHERITED Close;
END;


FUNCTION TPopmailWindow.FetchOne(theMsgNum: integer): Boolean; 
VAR
	hMsg	: handle;
	andDelete, gotone, gotsub, gotfrom: boolean;
	aPOP	: TPOP;
	nMsg	: integer;
	aStr, bStr: Str255;
	hc		: hcFile;
BEGIN
	gotone:= FALSE;
	IF (fDelCheck=NIL) THEN andDelete:= FALSE
	ELSE andDelete:= fDelCheck.IsOn;  
	aPOP:= TPOP(fPOP); { fPOP is stored as TObject... }
	nMsg:= 0;
	
	IF (aPOP<>NIL) & (aPOP.MailWaiting) THEN BEGIN
		nMsg:= aPOP.NumberOfMessages;
		IF (theMsgNum <= nMsg) THEN BEGIN
			fMsgNum:= theMsgNum;
			hMsg:= aPOP.ReadMail( fMsgNum, andDelete); 
			IF hMsg <> NIL THEN BEGIN

				hcOpen( hc, hMsg);
				gotsub:= FALSE; gotfrom:= FALSE;
				REPEAT
					aStr:= hcReadln( hc);
					IF pos('From:', aStr) = 1 THEN BEGIN
						Delete( aStr, 1, 5);
						IF (fFrom<>NIL) THEN fFrom.SetText( aStr, TRUE);
						gotfrom:= TRUE;
						END;
					IF pos('Subject:',aStr) = 1 THEN BEGIN
						Delete( aStr, 1, 8);
						IF (fSubject<>NIL) THEN fSubject.SetText( aStr, TRUE);
						gotsub:= TRUE;
						END;
				UNTIL (gotsub & gotfrom) | hcEof( hc);
				
{!!! Must keep hMsg & optionally save to disk,
	and check if > 32,000 chars, since TEView will truncate/bomb long msgs
!!!}
				if (GetHandleSize(hMsg)>31000) THEN 
					aPOP.ShowMessage( 'More text than window can hold…');

				{?? does someone else dispose these -- yes, sendmailview.free}
				IF (NIL = DisposeIfHandle(Handle(fMailView.fTextData.fChars))) THEN;
				fMailView.fTextData.fChars:= hMsg;
				
				IF (NIL = DisposeIfHandle(Handle(fMailView.fTextData.fStyles))) THEN;
				fMailView.fTextData.fStyles:= NIL;
				IF (NIL = DisposeIfHandle(Handle(fMailView.fTextData.fElements))) THEN;
				fMailView.fTextData.fElements:= NIL;
				
				fMailView.ShowReverted;
				gotone:= TRUE;
				END;
			END;
		{ aPOP.Release; ??? } 
		{ ^^ this cuts connection 
				-- .MailWaiting will reestablish as needed, but at cost of time delay }
		END;
		
	FetchOne:= gotone;
	
	IF (fCountTxt<>NIL) THEN BEGIN
		{!? nMsg is weird ... }
		NumToString( fMsgNum, aStr);
		NumToString( nMsg, bStr);
		fCountTxt.SetText( concat( aStr, ' of ', bStr), kRedraw);
		END;
	IF (fNextBut<>NIL) THEN BEGIN
		fNextBut.ViewEnable( (fMsgNum < nMsg), NOT kRedraw);
		fNextBut.Dimstate( NOT (fMsgNum < nMsg), kRedraw);
		END;
	IF (fLastBut<>NIL) THEN BEGIN
		fLastBut.ViewEnable( (fMsgNum > 1), NOT kRedraw);
		fLastBut.Dimstate( NOT (fMsgNum > 1), kRedraw);
		END;
END;





{ TGBFastAWindow .......................................}

CONST
	kFastaNucMenu = 2100;
	kFastaProtMenu = 2101;
	kBlastNucMenu = 2102;
	kBlastProtMenu = 2103;
	
	
PROCEDURE TGBFastAWindow.IGBFastAWindow( aSeq: TSequence);
VAR
	aSendBut: TButton;
	aFrom		: TEditText;
	hData	: Handle;
	isAmino: boolean;
BEGIN	
	{---
	IPrefWindow;
	fSendmailView:= TDlogTextView(FindSubView('MAIL'));
	IF (fSendmailView<>NIL) THEN fSendmailView.IDlogTextView(NIL);
  fTo 			:= TEditText(FindSubView('eTo:'));
	fStatus		:= TStaticText(FindSubView('tSTS'));
	IF (fStatus<>NIL) THEN fStatus.SetText( '', FALSE);
  aFrom			:= TEditText(FindSubView('eFm:'));  
	IF (aFrom<>NIL) THEN aFrom.SetText( gMyMailAddress, TRUE);
	aSendBut:= TButton(FindSubView('OKAY'));
	IF (aSendBut<>NIL) then aSendBut.fEventNumber:= mSendit;
	aSendBut:= TButton(FindSubView('Help'));
	IF (aSendBut<>NIL) THEN aSendBut.fEventNumber:= mHelp;
	fUnlockTexts:= TCheckBox(FindSubView('Unlk'));
	----}
	ISendmailWindow;
	fWantSave:= TRUE;

	IF (aSeq<>NIL) THEN BEGIN
		hData:= fSendmailView.fTextData.fChars;
	  aSeq.doWriteSelectionHandle( hData, kPearson);
																		{^^^ view field??}
		isAmino:= (aSeq.fKind = kAmino);
		END
	ELSE
		isAmino:= FALSE;

	fScore		:= TNumberText(FindSubView('SCOR'));
	fAligns		:= TNumberText(FindSubView('ALNS'));
	fDatabank	:= TPopUp(FindSubView('DATA'));
	fAmino		:= TRadio(FindSubView('rAMI'));
	fNucleic  := TRadio(FindSubView('rNUC'));
	fAmino.SetState( isAmino, NOT kRedraw);
	fNucleic.SetState( NOT isAmino, NOT kRedraw);
	IF isAmino THEN 
		fDataBank.SetPopup( GetMenu( kFastaProtMenu), 1, NOT kRedraw) 
	ELSE
		fDataBank.SetPopup( GetMenu( kFastaNucMenu), 1, NOT kRedraw);
END;
		
		
PROCEDURE TGBFastAWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
BEGIN
	CASE eventNumber OF
		{- mSendit			: BEGIN Sendit; CloseAndFree; END; }
		
		mRadioHit:  
		  IF (source=fNucleic) | (source=fAmino) THEN BEGIN
				IF fAmino.IsOn THEN 
					fDataBank.SetPopup( GetMenu( kFastaProtMenu), 1, kRedraw) 
				ELSE
					fDataBank.SetPopup( GetMenu( kFastaNucMenu), 1, kRedraw);
				END;

		Otherwise
			INHERITED DoEvent(eventNumber, source, event);
		END;
END;

{-------
fasta search mail:
	From:  drbob@someaddress.somewhere.edu Tue Jun 14 21:36:38 1988
	Date:  14 Jun 1988 2129:02-PDT
	To:    SEARCH@GENBANK.BIO.NET  
	Subject:  
	DATALIB GenBank/other_mammalian
	KTUP 4
	SCORES 100
	ALIGNMENTS 20
	BEGIN
	>BOVPRL GenBank entry BOVPRL from gbmam file.907 nucleotides. 
	tgcttggctgaggagccataggacgagagcttcctggtgaagtgtgtttcttgaaatcat
	caccaccatggacagcaaa
---------}

PROCEDURE TGBFastAWindow.Sendit; OVERRIDE;
VAR
	theTo, aStr: str255;
	theSubject, theCCopy: string[1];
	theMessage: Handle;
	hc	: hcFile;
	n		: longint;
BEGIN
	IF BadSMTPAddresses THEN EXIT(Sendit);
		
	fTo.GetText( theTo); 
	theSubject:= '';
	theCCopy:= '';

	theMessage:= NewPermHandle(1000);
	hc.h:= theMessage;
	hc.len:= 0;
	
	n:= fDatabank.GetCurrentItem;
	fDatabank.GetItemText( n, aStr);
	hcWriteln( hc, concat('DATALIB ',aStr));
	
	n:= fScore.GetValue;
	hcWrite( hc, 'SCORES '); hcWriteInt( hc, n); hcWriteln( hc, '');
	
	n:= fAligns.GetValue;
	hcWrite( hc, 'ALIGNMENTS '); hcWriteInt( hc, n); hcWriteln( hc, '');

	hcWriteln( hc, 'BEGIN');
	hcWriteHand( hc, fSendmailView.fHTE^^.hText);
	hcWriteln( hc, '');
	SetHandleSize( hc.h, hc.len);
	FailMemError;
		
	MakeSMTP( theTo, theSubject, theCCopy, theMessage, FALSE);
	DisposHandle( theMessage);
END;


PROCEDURE TGBFastAWindow.SetPrefID;  OVERRIDE;
BEGIN
	gPrefWindID:= kGBFastaWindID; gPrefWindName:= 'TGBFastAWindow';
END;
	
	
	
	
{ TGBBlastWindow .......................................}

PROCEDURE TGBBlastWindow.IGBBlastWindow( aSeq: TSequence);
VAR
	aSendBut: TButton;
	aFrom		: TEditText;
	hData		: Handle;
	isAmino	: boolean;
BEGIN	
	{-----
	IPrefWindow;
	fSendmailView:= TDlogTextView(FindSubView('MAIL'));
	IF (fSendmailView<>NIL) THEN fSendmailView.IDlogTextView(NIL);
  fTo 			:= TEditText(FindSubView('eTo:'));
	fStatus		:= TStaticText(FindSubView('tSTS'));
	IF (fStatus<>NIL) THEN fStatus.SetText( '', FALSE);
  aFrom			:= TEditText(FindSubView('eFm:'));  
	IF (aFrom<>NIL) THEN aFrom.SetText( gMyMailAddress, TRUE);
	aSendBut:= TButton(FindSubView('OKAY'));
	IF (aSendBut<>NIL) then aSendBut.fEventNumber:= mSendit;
	aSendBut:= TButton(FindSubView('Help'));
	IF (aSendBut<>NIL) THEN aSendBut.fEventNumber:= mHelp;
	fUnlockTexts:= TCheckBox(FindSubView('Unlk'));
	---}
	ISendmailWindow;
	fWantSave:= TRUE;
	
	IF (aSeq<>NIL) THEN BEGIN
		hData:= fSendmailView.fTextData.fChars;
	  aSeq.doWriteSelectionHandle( hData, kPearson);
		isAmino:= (aSeq.fKind = kAmino);
		END
	ELSE
		isAmino:= FALSE;

	fDatabank	:= TPopUp(FindSubView('DATA'));
	fAmino		:= TRadio(FindSubView('rAMI'));
	fNucleic  := TRadio(FindSubView('rNUC'));
	fAmino.SetState( isAmino, NOT kRedraw);
	fNucleic.SetState( NOT isAmino, NOT kRedraw);
	IF isAmino THEN 
		fDataBank.SetPopup( GetMenu( kBlastProtMenu), 1,  NOT kRedraw) 
	ELSE
		fDataBank.SetPopup( GetMenu( kBlastNucMenu), 1, NOT kRedraw);
END;
		
		
PROCEDURE TGBBlastWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
BEGIN
	CASE eventNumber OF
		{- mSendit			: BEGIN Sendit; CloseAndFree; END; }
		
		mRadioHit:  
		  IF (source=fNucleic) | (source=fAmino) THEN BEGIN
				IF fAmino.IsOn THEN 
					fDataBank.SetPopup( GetMenu( kBlastProtMenu), 1, kRedraw) 
				ELSE
					fDataBank.SetPopup( GetMenu( kBlastNucMenu), 1, kRedraw);
				END;

		Otherwise
			INHERITED DoEvent( eventNumber, source, event);
		END;
END;

{-------
blast search mail:
	From:  drgene@someaddress.somewhere.edu Tue Jun 14 21:36:38 1988
	Date:  14 Jun 1988 2129:02-PDT
	To:    BLAST@GENBANK.BIO.NET  
	Subject:  
	BLASTPROGRAM blastn
	DATALIB embl
	BEGIN
	>BOVPRL GenBank entry BOVPRL from gbmam file.907 nucleotides. 
	tgcttggctgaggagccataggacgagagcttcctggtgaagtgtgtttcttgaaatcat
	caccaccatggacagcaaa
---------}

PROCEDURE TGBBlastWindow.Sendit; OVERRIDE;
VAR
	theTo, aStr: str255;
	theSubject, theCCopy: string[1];
	theMessage: Handle;
	hc	: hcFile;
	n		: longint;
BEGIN
	IF BadSMTPAddresses THEN EXIT(Sendit);
		
	fTo.GetText( theTo); 
	theSubject:= '';
	theCCopy:= '';

	theMessage:= NewPermHandle(1000);
	hc.h:= theMessage;
	hc.len:= 0;

	IF fAmino.IsOn THEN
		hcWriteln( hc, 'BLASTPROGRAM BLASTP')
	ELSE
		hcWriteln( hc, 'BLASTPROGRAM BLASTN'); 

	n:= fDatabank.GetCurrentItem;
	fDatabank.GetItemText( n, aStr);
	hcWriteln( hc, concat('DATALIB ',aStr));
	
	hcWriteln( hc, 'BEGIN');
	hcWriteHand( hc, fSendmailView.fHTE^^.hText);
	hcWriteln( hc, '');
	SetHandleSize( hc.h, hc.len);
	FailMemError;
		
	MakeSMTP( theTo, theSubject, theCCopy, theMessage, FALSE);
	DisposHandle( theMessage);
END;




PROCEDURE TGBBlastWindow.SetPrefID;  OVERRIDE;
BEGIN
	gPrefWindID:= kGBBlastWindID; gPrefWindName:= 'TGBBlastWindow';
END;







{ TNCBIFetchWindow .......................................}

CONST
	kNCBINuclibs = 2115;
	kNCBINucMethods = 2116;
	kNCBIFetchlibs = 2117;
	kNCBIProtlibs = 2118;
	kNCBIProtMethods = 2119;

	kBlastP = 'blastp';
	kTBlastN = 'tblastn';
	kBlastX = 'blastx';
	kBlastN = 'blastn';


PROCEDURE TNCBIFetchWindow.INCBIFetchWindow;
BEGIN	
	ISendmailWindow;
	fWantSave:= TRUE;
	fDatabank	:= TPopUp(FindSubView('DATA'));
	fNDocs		:= TNumberText(FindSubView('ndoc'));
	fNLines		:= TNumberText(FindSubView('nlin'));
	fStartDoc	:= TNumberText(FindSubView('idoc'));
END;
		
PROCEDURE TNCBIFetchWindow.Sendit; OVERRIDE;
VAR
	theTo, aStr: str255;
	theSubject, theCCopy: string[1];
	theMessage: Handle;
	hc	: hcFile;
	n		: longint;
	k		: integer;
BEGIN
	IF BadSMTPAddresses THEN EXIT(Sendit);
		
	fTo.GetText( theTo); 
	theSubject:= '';
	theCCopy:= '';
	theMessage:= NewPermHandle(1000);
	FailNIL( theMessage);
	hc.h:= theMessage;
	hc.len:= 0;

	k:= fDatabank.GetCurrentItem;
	fDatabank.GetItemText( k, aStr);
	k:= pos(' ', aStr); IF (k>0) THEN aStr[0]:= chr(k-1);
	hcWriteln( hc, concat('DATALIB ',aStr));

	fNDocs.GetText( aStr);
	IF (length(aStr)>0) THEN hcWriteln( hc, concat('MAXDOCS ',aStr));
	fNLines.GetText( aStr);
	IF (length(aStr)>0) THEN hcWriteln( hc, concat('MAXLINES ',aStr));
	fStartDoc.GetText( aStr);
	IF (length(aStr)>0) THEN hcWriteln( hc, concat('STARTDOC ',aStr));
	
	hcWriteln( hc, 'BEGIN');
	hcWriteHand( hc, fSendmailView.fHTE^^.hText);
	hcWriteln( hc, '');
	SetHandleSize( hc.h, hc.len);
	FailMemError;
	
	MakeSMTP( theTo, theSubject, theCCopy, theMessage, FALSE);
	DisposHandle( theMessage);
END;



PROCEDURE TNCBIFetchWindow.SetPrefID;  OVERRIDE;
BEGIN
	gPrefWindID:= kNCBIFetchWindID; gPrefWindName:= 'TNCBIFetchWindow';
END;




	
{ TNCBIBlastWindow .......................................}


PROCEDURE TNCBIBlastWindow.SetMenus( redraw: boolean);
VAR  method: Str255; k: integer;
BEGIN

	{!?? or use one methods menu & one datalib menu, & disable nonusable items }
	IF (fMethod<>NIL) & (fDataBank <> NIL) THEN BEGIN
		IF fAmino.IsOn THEN BEGIN
			IF fMethod.fMenuID <> kNCBIProtMethods THEN
				fMethod.SetPopup( GetMenu( kNCBIProtMethods), 1, redraw);
			END
		ELSE BEGIN
			IF fMethod.fMenuID <> kNCBINucMethods THEN
				fMethod.SetPopup( GetMenu( kNCBINucMethods), 1, redraw);
			END;
	
		k:= fMethod.GetCurrentItem;
		fMethod.GetItemText( k, method);
		k:= pos(' ', method); IF (k>0) THEN method[0]:= chr(k-1);
		
		IF (method = kBlastP) | (method = kBlastX) THEN BEGIN
			IF fDataBank.fMenuID <> kNCBIProtLibs THEN
				fDataBank.SetPopup( GetMenu( kNCBIProtlibs), 1,  redraw);
			END
		ELSE BEGIN
			IF fDataBank.fMenuID <> kNCBINuclibs THEN
				fDataBank.SetPopup( GetMenu( kNCBINuclibs), 1,  redraw);
			END;
		END;
END;


PROCEDURE TNCBIBlastWindow.INCBIBlastWindow( aSeq: TSequence);
VAR
	aSendBut: TButton;
	aFrom		: TEditText;
	hData		: Handle;
	isAmino	: boolean;
BEGIN	
	{-----
	IPrefWindow;
	fSendmailView:= TDlogTextView(FindSubView('MAIL'));
	IF (fSendmailView<>NIL) THEN fSendmailView.IDlogTextView(NIL);
  fTo 			:= TEditText(FindSubView('eTo:'));
	fStatus		:= TStaticText(FindSubView('tSTS'));
	IF (fStatus<>NIL) THEN fStatus.SetText( '', FALSE);
  aFrom			:= TEditText(FindSubView('eFm:'));  
	IF (aFrom<>NIL) THEN aFrom.SetText( gMyMailAddress, TRUE);
	aSendBut:= TButton(FindSubView('OKAY'));
	IF (aSendBut<>NIL) then aSendBut.fEventNumber:= mSendit;
	aSendBut:= TButton(FindSubView('Help'));
	IF (aSendBut<>NIL) THEN aSendBut.fEventNumber:= mHelp;
	fUnlockTexts:= TCheckBox(FindSubView('Unlk'));
	------}
	ISendmailWindow;
	fWantSave:= TRUE;

	IF (aSeq<>NIL) THEN BEGIN
		hData:= fSendmailView.fTextData.fChars;
	  aSeq.doWriteSelectionHandle( hData, kPearson);
		isAmino:= (aSeq.fKind = kAmino);
		END
	ELSE
		isAmino:= FALSE;

	fDatabank	:= TPopUp(FindSubView('DATA'));
	fMethod		:= TPopUp(FindSubView('METH')); 
	fDesc			:= TNumberText(FindSubView('ndes'));  
	fAlign		:= TNumberText(FindSubView('nali'));
	fExcut		:= TEditText(FindSubView('ecut'));
	fICut			:= TEditText(FindSubView('icut'));
	fHistogram:= TCheckBox(FindSubView('hist'));
	
	fAmino		:= TRadio(FindSubView('rAMI'));
	fNucleic  := TRadio(FindSubView('rNUC'));
	IF (fAmino<>NIL) THEN fAmino.SetState( isAmino, NOT kRedraw);
	IF (fNucleic<>NIL) THEN fNucleic.SetState( NOT isAmino, NOT kRedraw);
	
	SELF.SetMenus( kDontRedraw); 
	
END;
		
		
PROCEDURE TNCBIBlastWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
BEGIN
	CASE eventNumber OF
		{- mSendit : BEGIN Sendit; CloseAndFree; END; }
		
		mRadioHit:  
		  IF (source=fNucleic) | (source=fAmino) THEN BEGIN
				SELF.SetMenus( kRedraw);
				END;
				
		mPopupHit:
		  IF (source=fMethod) THEN BEGIN
				SELF.SetMenus( kRedraw);
				END;
		
		Otherwise
			INHERITED DoEvent( eventNumber, source, event);
		END;
END;

{-------
blast search mail:
	From:  drgene@someaddress.somewhere.edu Tue Jun 14 21:36:38 1988
	Date:  14 Jun 1988 2129:02-PDT
	To:    BLAST@ncbi.nlm.nih.gov
	Subject:  
	PROGRAM blastn
	DATALIB embl
	BEGIN
	>BOVPRL GenBank entry BOVPRL from gbmam file.907 nucleotides. 
	tgcttggctgaggagccataggacgagagcttcctggtgaagtgtgtttcttgaaatcat
	caccaccatggacagcaaa

PROGRAM          Yes            BLAST program to execute
                                (blastp, blastn, blastx, tblastn).
DATALIB          Yes            Database to be searched (see list in 
                                section 6); only one database can be
                                be searched per mail message.
DESCRIPTION      No             Maximum number of short descriptions of
                                matching sequences to be reported; default
                                is 100.  (See Parameter V, manual page below)
ALIGNMENTS       No             Maximum number of High Scoring Segment
                                Pairs to be reported; default is 50.  (See
                                Parameter B, manual page)
HISTOGRAM        No             Display of histogram of scores with each search;
                                default is yes.  Not applicable to BLASTX.  
                                (See Parameter H, manual page)
EXPECT           No             Expectation cutoff; default is 10.0.  (See 
                                Parameter E, manual page)
CUTOFF           No             Cutoff score.  (See Parameter S, manual page)
BEGIN            Yes            This mandatory parameter is not paired with a
                                value and must be the last parameter before
                                the actual query.
---------}

PROCEDURE TNCBIBlastWindow.Sendit; OVERRIDE;
VAR
	theTo, aStr: str255;
	theSubject, theCCopy: string[1];
	theMessage: Handle;
	hc	: hcFile;
	n		: longint;
	k		: integer;
BEGIN
	IF BadSMTPAddresses THEN EXIT(Sendit);
		
	fTo.GetText( theTo); 
	theSubject:= '';
	theCCopy:= '';

	theMessage:= NewPermHandle(1000);
	hc.h:= theMessage;
	hc.len:= 0;

	fMethod.GetItemText( fMethod.GetCurrentItem, aStr);
	k:= pos(' ', aStr); IF (k>0) THEN aStr[0]:= chr(k-1);
	hcWriteln( hc, concat('PROGRAM ', aStr));

	fDesc.GetText( aStr);
	IF (length(aStr)>0) THEN hcWriteln( hc, concat('DESCRIPTION ',aStr));
	fAlign.GetText( aStr);
	IF (length(aStr)>0) THEN hcWriteln( hc, concat('ALIGNMENTS ',aStr));
	IF fHistogram.isOn THEN hcWriteln( hc, 'HISTOGRAM');
	fExCut.GetText( aStr);
	IF (length(aStr)>0) THEN hcWriteln( hc, concat('EXPECT ',aStr));
	fICut.GetText( aStr);
	IF (length(aStr)>0) THEN hcWriteln( hc, concat('CUTOFF ',aStr));
	
	fDatabank.GetItemText( fDatabank.GetCurrentItem, aStr);
	k:= pos(' ', aStr); IF (k>0) THEN aStr[0]:= chr(k-1);
	hcWriteln( hc, concat('DATALIB ',aStr));
	
	hcWriteln( hc, 'BEGIN');
	hcWriteHand( hc, fSendmailView.fHTE^^.hText);
	hcWriteln( hc, '');
	SetHandleSize( hc.h, hc.len);
	FailMemError;
		
	MakeSMTP( theTo, theSubject, theCCopy, theMessage, FALSE);
	DisposHandle( theMessage);
END;



PROCEDURE TNCBIBlastWindow.SetPrefID;  OVERRIDE;
BEGIN
	gPrefWindID:= kNCBIBlastWindID; gPrefWindName:= 'TNCBIBlastWindow';
END;


{	TGeneidWindow	 -------------}

PROCEDURE TGeneidWindow.IGeneidWindow( aSeq: TSequence);
VAR
	aSendBut: TButton;
	aFrom		: TEditText;
	hData		: Handle;
BEGIN	
	{----
	IPrefWindow;
	fSendmailView:= TDlogTextView(FindSubView('MAIL'));
	IF (fSendmailView<>NIL) THEN fSendmailView.IDlogTextView(NIL);
  fTo 			:= TEditText(FindSubView('eTo:'));
	fStatus		:= TStaticText(FindSubView('tSTS'));
	IF (fStatus<>NIL) THEN fStatus.SetText( '', FALSE);
  aFrom			:= TEditText(FindSubView('eFm:'));  
	IF (aFrom<>NIL) THEN aFrom.SetText( gMyMailAddress, TRUE);
	aSendBut:= TButton(FindSubView('OKAY'));
	IF (aSendBut<>NIL) then aSendBut.fEventNumber:= mSendit;
	aSendBut:= TButton(FindSubView('Help'));
	IF (aSendBut<>NIL) THEN aSendBut.fEventNumber:= mHelp;
	fUnlockTexts:= TCheckBox(FindSubView('Unlk'));
	---}
	
	ISendmailWindow;
	fWantSave:= FALSE;
	IF (aSeq<>NIL) THEN BEGIN
		hData:= fSendmailView.fTextData.fChars;
	  aSeq.doWriteSelectionHandle( hData, kPearson);
		IF NOT (aSeq.fKind = kDna ) THEN ; {Fail....}
		END;
		
	fUseNetgene:= TCheckBox(FindSubView('NetG'));
END;
		

{-------
Geneid  mail:
	From:  me@someaddress.somewhere.edu 
	To:    geneid@darwin.bu.edu
	Subject:  
	Genomic Sequence
	>seqname
	TTGGCCACTCCCTCTCTGCGCGCTCGCTCGCTCACTGAGGCCGGGCGACCAAAGGTCGCC
	CGACGCCCGGGCTTTGCCCGGGCGGCCTCAGTGAGCGAGCGAGCGCGCAGAGAGGGAGTG
	GCCAACTCCATCACTA...................
---------}

PROCEDURE TGeneidWindow.Sendit; OVERRIDE;
VAR
	theTo, aStr: str255;
	theSubject, theCCopy: string[1];
	theMessage: Handle;
	hc	: hcFile;
	n		: longint;
BEGIN
	IF BadSMTPAddresses THEN EXIT(SendIt);
		
	fTo.GetText( theTo); 
	theSubject:= '';
	theCCopy:= '';

	theMessage:= NewPermHandle(100);
	hc.h:= theMessage;
	hc.len:= 0;

	hcWriteln( hc, 'Genomic Sequence'); 
	IF (fUseNetgene<>NIL) & (fUseNetgene.IsOn) THEN hcWriteln( hc, 'NetGene'); 
	hcWriteHand( hc, fSendmailView.fHTE^^.hText);
	hcWriteln( hc, '');
	SetHandleSize( hc.h, hc.len);
	FailMemError;
		
	MakeSMTP( theTo, theSubject, theCCopy, theMessage, FALSE);
	DisposHandle( theMessage);
END;




{	TBlocksWindow	 -------------}

PROCEDURE TBlocksWindow.IBlocksWindow( aSeq: TSequence);
VAR
	aSendBut: TButton;
	aFrom		: TEditText;
	hData		: Handle;
BEGIN	
	{----
	IPrefWindow;
	fSendmailView:= TDlogTextView(FindSubView('MAIL'));
	IF (fSendmailView<>NIL) THEN fSendmailView.IDlogTextView(NIL);
  fTo 			:= TEditText(FindSubView('eTo:'));
	fStatus		:= TStaticText(FindSubView('tSTS'));
	IF (fStatus<>NIL) THEN fStatus.SetText( '', FALSE);
  aFrom			:= TEditText(FindSubView('eFm:'));  
	IF (aFrom<>NIL) THEN aFrom.SetText( gMyMailAddress, TRUE);
	aSendBut:= TButton(FindSubView('OKAY'));
	IF (aSendBut<>NIL) then aSendBut.fEventNumber:= mSendit;
	aSendBut:= TButton(FindSubView('Help'));
	IF (aSendBut<>NIL) THEN aSendBut.fEventNumber:= mHelp;
	fUnlockTexts:= TCheckBox(FindSubView('Unlk'));
	----}
	
	ISendmailWindow;
	fWantSave:= FALSE;
	IF (aSeq<>NIL) THEN BEGIN
		hData:= fSendmailView.fTextData.fChars;
	  aSeq.doWriteSelectionHandle( hData, kPearson);
		IF NOT (aSeq.fKind = kDna ) THEN ; {Fail....}
		END;
		
END;
		

{-------
Blocks  mail:
To: blocks@howard.fhcrc.org
Subject:
>YCZ2_YEAST   Hypothetical 40.1 KD protein in HMR 3' region
MKAVVIEDGKAVVKEGVPIPELEEGFVLIKTLAVAGNPTDWAHIDYKVGPQGSILGCDAA
GQIVKLGPAVDPKDFSIGDYIYGFIHGSSVRFPSNGAFAEYSAISTVVAYKSPNELKFLG
---------}

PROCEDURE TBlocksWindow.Sendit; OVERRIDE;
VAR
	theTo, aStr: str255;
	theSubject, theCCopy: string[1];
	theMessage: Handle;
	hc	: hcFile;
	n		: longint;
BEGIN
	IF BadSMTPAddresses THEN EXIT(SendIt);
		
	fTo.GetText( theTo); 
	theSubject:= '';
	theCCopy:= '';

	theMessage:= NewPermHandle(100);
	hc.h:= theMessage;
	hc.len:= 0;

	hcWriteHand( hc, fSendmailView.fHTE^^.hText);
	hcWriteln( hc, '');
	SetHandleSize( hc.h, hc.len);
	FailMemError;
		
	MakeSMTP( theTo, theSubject, theCCopy, theMessage, FALSE);
	DisposHandle( theMessage);
END;


	

{	TGenmarkWindow	 -------------}


PROCEDURE TGenmarkWindow.IGenmarkWindow( aSeq: TSequence);
VAR
	aSendBut: TButton;
	aFrom		: TEditText;
	hData		: Handle;
BEGIN	
	{----
	IPrefWindow;
	fSendmailView:= TDlogTextView(FindSubView('MAIL'));
	IF (fSendmailView<>NIL) THEN fSendmailView.IDlogTextView(NIL);
  fTo 			:= TEditText(FindSubView('eTo:'));
	fStatus		:= TStaticText(FindSubView('tSTS'));
	IF (fStatus<>NIL) THEN fStatus.SetText( '', FALSE);
  aFrom			:= TEditText(FindSubView('eFm:'));  
	IF (aFrom<>NIL) THEN aFrom.SetText( gMyMailAddress, TRUE);
	aSendBut:= TButton(FindSubView('OKAY'));
	IF (aSendBut<>NIL) then aSendBut.fEventNumber:= mSendit;
	aSendBut:= TButton(FindSubView('Help'));
	IF (aSendBut<>NIL) THEN aSendBut.fEventNumber:= mHelp;
	fUnlockTexts:= TCheckBox(FindSubView('Unlk'));
	---}
	
	ISendmailWIndow;
	fWantSave:= FALSE;
	IF (aSeq<>NIL) THEN BEGIN
		hData:= fSendmailView.fTextData.fChars;
	  aSeq.doWriteSelectionHandle( hData, kPlain);
		IF NOT (aSeq.fKind = kDna ) THEN ; {Fail....}
		END;
			
	fPSTitle:= TEditText( FindSubView('titl'));
	fPSGraph:= TCheckBox( FindSubView('graf'));
	fMarkovOrder:= TNumberText( FindSubView('ordr'));
	fWinStep	:= TNumberText( FindSubView('step'));
	fThreshold:= TNumberText( FindSubView('thrs'));
	fAnalWin	:= TNumberText( FindSubView('awnd'));
END;
		

{-------
Genmark  mail:
> mail genmark@ford.gatech.edu
Subject: genmark
# This is an example of using all of the different options.
address biologist@college.edu
name John Doe
order 5
psgraph
step 6
threshold 0.50
title John Doe's New Protein Coding Region
window 144
data
TCAGTTCCAAGGTTTCCCAAAGGGTTTTCCCCAAAAGGGG...

---------}

PROCEDURE TGenmarkWindow.Sendit; OVERRIDE;
VAR
	theTo, aStr: str255;
	theSubject: string[29];
	theCCopy: string[1];
	theMessage: Handle;
	hc	: hcFile;
	n		: longint;
BEGIN
	IF BadSMTPAddresses THEN EXIT(SendIt);
		
	fTo.GetText( theTo); 
	theCCopy:= '';

	theMessage:= NewPermHandle(1000);
	hc.h:= theMessage;
	hc.len:= 0;

	theSubject:= 'genmark';
	hcWrite( hc, 'order '); hcWriteInt( hc, fMarkovOrder.GetValue); hcWriteln( hc, '');
	hcWrite( hc, 'step '); hcWriteInt( hc, fWinStep.GetValue); hcWriteln( hc, '');
	hcWrite( hc, 'threshold '); hcWriteInt( hc, fThreshold.GetValue); hcWriteln( hc, '');
	hcWrite( hc, 'window '); hcWriteInt( hc, fAnalWin.GetValue); hcWriteln( hc, '');	
	IF (fPSGraph<>NIL) & (fPSGraph.IsOn) THEN BEGIN
		hcWriteln( hc, 'psgraph'); 
		fPSTitle.GetText( aStr);
		hcWriteln( hc, concat('title ', aStr));
		END;
		
	hcWriteln( hc, 'Data'); 
	hcWriteHand( hc, fSendmailView.fHTE^^.hText);
	hcWriteln( hc, '');
	SetHandleSize( hc.h, hc.len);
	FailMemError;
		
	MakeSMTP( theTo, theSubject, theCCopy, theMessage, FALSE);
	DisposHandle( theMessage);
END;


PROCEDURE TGenmarkWindow.SetPrefID;  OVERRIDE;
BEGIN
	gPrefWindID:= kGenmarkWindID; gPrefWindName:= 'TGenmarkWindow';
END;
	
	


{ TGrailWindow ------------------}

		
PROCEDURE TGrailWindow.IGrailWindow( aSeqList: TSeqList);
VAR
	aSendBut: TButton;
	aFrom		: TEditText;
	hData		: Handle;
BEGIN	
	{---
	IPrefWindow;
	fSendmailView:= TDlogTextView(FindSubView('MAIL'));
	IF (fSendmailView<>NIL) THEN fSendmailView.IDlogTextView(NIL);
  fTo 			:= TEditText(FindSubView('eTo:'));
	FailNIL(fTo);
	fStatus		:= TStaticText(FindSubView('tSTS'));	
	IF (fStatus<>NIL) THEN fStatus.SetText( '', FALSE);
  aFrom			:= TEditText(FindSubView('eFm:'));  
	IF (aFrom<>NIL) THEN aFrom.SetText( gMyMailAddress, TRUE);
	aSendBut:= TButton(FindSubView('OKAY'));
	IF (aSendBut<>NIL) then aSendBut.fEventNumber:= mSendit;
	aSendBut:= TButton(FindSubView('Help'));
	IF (aSendBut<>NIL) THEN aSendBut.fEventNumber:= mHelp;
	fUnlockTexts:= TCheckBox(FindSubView('Unlk'));
	---}
	ISendmailWindow;
	fWantSave:= TRUE;

	IF (aSeqList<>NIL) THEN BEGIN
		fNumSeqs:= aSeqList.GetSize;
	  hData:= aSeqList.doWriteHandle( kPearson);
		IF (NIL = DisposeIfHandle(fSendmailView.fTextData.fChars) ) THEN;
		fSendmailView.fTextData.fChars:= hData;
		{- IF NOT (TSequence(aSeqList.First).fKind = kDna ) THEN ; } {Fail....}
		END
	ELSE
		fNumSeqs:= 0;

  fMailID 			:= TEditText(FindSubView('eGID'));
	FailNIL(fMailID);
END;

{-------
Grail mail:
	From:  me@someaddress.somewhere.edu 
	To:    grail@ornl.gov 
	Subject:  
	Sequences number_of_sequences  your_user_ID
	>seq1name
	AAAAAAAA........
	
	>seq2name
	TTTTTTTT..........
---------}

PROCEDURE TGrailWindow.Sendit; OVERRIDE;
VAR
	theTo, aStr: str255;
	theSubject, theCCopy: string[1];
	theMessage: Handle;
	hc	: hcFile;
	n		: longint;
BEGIN
	IF BadSMTPAddresses THEN EXIT(Sendit);
		
	fTo.GetText( theTo); 
	theSubject:= '';
	theCCopy:= '';

	theMessage:= NewPermHandle(1000);
	hc.h:= theMessage;
	hc.len:= 0;

	hcWrite( hc, 'Sequences ');
	hcWriteInt( hc, fNumSeqs);
	fMailID.GetText( aStr);
	hcWriteln( hc, concat(' ',aStr));
	hcWriteHand( hc, fSendmailView.fHTE^^.hText);
	hcWriteln( hc, '');
	SetHandleSize( hc.h, hc.len);
	FailMemError;
		
	MakeSMTP( theTo, theSubject, theCCopy, theMessage, FALSE);
	DisposHandle( theMessage);
END;


PROCEDURE TGrailWindow.SetPrefID;  OVERRIDE;
BEGIN
	gPrefWindID:= kGrailWindID; gPrefWindName:= 'TGrailWindow';
END;
	
	
	
	

{ TPythiaWindow ------------------}

		
PROCEDURE TPythiaWindow.IPythiaWindow( aSeqList: TSeqList);
VAR
	aSendBut: TButton;
	aFrom		: TEditText;
	hData		: Handle;
BEGIN	
	{---
	IPrefWindow;
	fSendmailView:= TDlogTextView(FindSubView('MAIL'));
	IF (fSendmailView<>NIL) THEN fSendmailView.IDlogTextView(NIL);
  fTo 			:= TEditText(FindSubView('eTo:'));
	FailNIL(fTo);
	fStatus		:= TStaticText(FindSubView('tSTS'));	
	IF (fStatus<>NIL) THEN fStatus.SetText( '', FALSE);
  aFrom			:= TEditText(FindSubView('eFm:'));  
	IF (aFrom<>NIL) THEN aFrom.SetText( gMyMailAddress, TRUE);
	aSendBut:= TButton(FindSubView('OKAY'));
	IF (aSendBut<>NIL) then aSendBut.fEventNumber:= mSendit;
	aSendBut:= TButton(FindSubView('Help'));
	IF (aSendBut<>NIL) THEN aSendBut.fEventNumber:= mHelp;
	fUnlockTexts:= TCheckBox(FindSubView('Unlk'));
	---}
	
	ISendmailWindow;
	fWantSave:= TRUE;

	IF (aSeqList<>NIL) THEN BEGIN
	  hData:= aSeqList.doWriteHandle( kIG);
		IF (NIL = DisposeIfHandle(fSendmailView.fTextData.fChars)) THEN;
		fSendmailView.fTextData.fChars:= hData;
		{- IF NOT (TSequence(aSeqList.First).fKind = kDna ) THEN ; } {Fail....}
		END;

	fDoAlu:= TRadio(FindSubView('alus'));
	fDoRpts:= TRadio(FindSubView('rpts'));
	
END;

{-------
Pythia mail:
To:    pythia@anl.gov 
Subject: Rpts  [or Alu]
;
HUMGENE1
CTTCTTTGTGGCATTCTGCTGTCGTATACCATGTGGAACACATTAAGAACGTTATGGCCAGGCGTGTTGG
CTCACGCCTGTAATCCTAGCACTTTGGGAGGCCAAGGTGGACAGATCACCTGAGGTTGGGAGTTCGAGAC
ACT1
;
HUMGENE2
CAATAAAATCCCAATGCTTCCGCTGCAGAAGTCCAAGAGGACATGACTGCGGCTCCATCTAGTCAAGCCC
AGGGCAGGAATTCCCTTCCAGGAAACCAAGCCAGAGCGCTGTGGTCTCTGGGCTGCCAAGATGTCTCAGA
A1
---------}

PROCEDURE TPythiaWindow.Sendit; OVERRIDE;
VAR
	theTo, aStr: str255;
	theSubject: string[29];
	theCCopy: string[1];
	theMessage: Handle;
	hc	: hcFile;
	n		: longint;
BEGIN
	IF BadSMTPAddresses THEN EXIT(Sendit);
		
	fTo.GetText( theTo); 
	theCCopy:= '';

	theMessage:= NewPermHandle(1000);
	hc.h:= theMessage;
	hc.len:= 0;

	if fDoAlu.IsOn THEN	theSubject:= 'Alu'
	ELSE theSubject:= 'Rpts';

	hcWriteHand( hc, fSendmailView.fHTE^^.hText);
	hcWriteln( hc, '');
	SetHandleSize( hc.h, hc.len);
	FailMemError;
		
	MakeSMTP( theTo, theSubject, theCCopy, theMessage, FALSE);
	DisposHandle( theMessage);
END;


PROCEDURE TPythiaWindow.SetPrefID;  OVERRIDE;
BEGIN
	gPrefWindID:= kPythiaWindID; gPrefWindName:= 'TPythiaWindow';
END;
	
	
	

{ OpenDialogs ....................................}

CONST
			{! USeqApp.app.p is owner of these command numbers; they may change !}
	cSendMail	=	2103;
	cGBFetch	= 2105;
	cBugMail	= 2106;
	cEMBLFetch	= 2107;
	cGBFasta = 2110;
	cGBBlast	= 2111;
	cGeneidSearch = 2112;
	cGrailSearch = 2113;
	cUHServer = 2114;
	cGenmarkSearch = 2118;
	cBlocksSearch = 2119;
	cPythiaSearch = 2120;

PROCEDURE InitializeMailPrefs;
VAR 	aWind	: TMailPrefWindow;
			aTCP	: TTCP;
BEGIN
	New(aTCP); 
	FailNIL(aTCP);
	aTCP.ITCP;
	gTCPIsInstalled:= aTCP.IsTCPInstalled;
	aTCP.Free;

	gMailerID:= concat( 'SeqApp ',compdate);

	gMyMailAddress:= '';
	gSMTPHost	:= '';
	gMyPOPAddress	:= '';
	gPassword	:= '';
	aWind := TMailPrefWindow(gViewServer.NewTemplateWindow(kMailPrefWindID, NIL));
	IF (aWind<>NIL) THEN BEGIN		 
		aWind.IMailPrefWindow;
		aWind.SetGlobals;
		aWind.Free;
		END;
		
	IF gDeadStripSuppression THEN BEGIN
	{-	IF Member(TObject(NIL), THiddenText) THEN; -}
		IF Member(TObject(NIL), TMailPrefWindow) THEN;
		IF Member(TObject(NIL), TNCBIfetchWindow) THEN;
		IF Member(TObject(NIL), TNCBIBlastWindow) THEN;
		IF Member(TObject(NIL), TGBBlastWindow) THEN;
		IF Member(TObject(NIL), TGrailWindow) THEN;
		IF Member(TObject(NIL), TGeneidWindow) THEN;
		IF Member(TObject(NIL), TGenmarkWindow) THEN;
		IF Member(TObject(NIL), TBlocksWindow) THEN;
		IF Member(TObject(NIL), TPythiaWindow) THEN;
		IF Member(TObject(NIL), TEMBLFetchWindow) THEN;
		IF Member(TObject(NIL), TUHServerWindow) THEN;
		IF Member(TObject(NIL), TBugmailWindow) THEN;
		IF Member(TObject(NIL), TPopmailWindow) THEN;
		IF Member(TObject(NIL), TSendmailWindow) THEN;
		END;
END;

PROCEDURE OpenMailPref;
CONST  msgMyError = $80001234;
VAR   aWind	: TMailPrefWindow;
BEGIN
	aWind := TMailPrefWindow(gViewServer.NewTemplateWindow(kMailPrefWindID, NIL));
	FailNIL(aWind); 		 
	aWind.IMailPrefWindow;
	aWind.ModalDialog;
	aWind.CloseAndFree;
	{- aWind.Free; }  
	IF NOT gTCPIsInstalled THEN BEGIN
		ParamText('MacTCP is not installed; Internet methods are not available',
									'','Internet','');	
		Failure( -1, msgMyError);  
		END;
END;

PROCEDURE OpenCheckmail;
		{! THIS MUST BE Part of a Checkmail/Popmail document so we can save msgs...}
VAR
		aWind	: TPopmailWindow;
		isNowOpen: boolean;
		
		PROCEDURE checkWindows( aWind: TWindow);
		BEGIN
			IF Member(TObject(aWind), TPopmailWindow) THEN BEGIN
				aWind.Select; isNowOpen:= TRUE;
				END;
		END;
BEGIN
	isNowOpen:= FALSE;
	gApplication.ForAllWindowsDo(checkWindows);
	if NOT isNowOpen then begin
		aWind := TPopmailWindow(gViewServer.NewTemplateWindow(kPopMailWindID, NIL));
		FailNIL(aWind); 		 
		aWind.IPopmailWindow;
		aWind.ShowReverted; 
		aWind.Open; aWind.Select;	  

		{- aWind.OpenPop; }{<< call from PopmailWindow dialog msgs...} 
		END;
END;



PROCEDURE MailTo( aCommand: Integer);
VAR
		aSendmail	: TSendmailWindow;
		aBugmail	: TBugmailWindow;
		aNCBIfetch	: TNCBIfetchWindow;
		aUHServer	: TUHServerWindow;
		aEMBLFetch	: TEMBLFetchWindow;
		isNowOpen: boolean;
		
		PROCEDURE checkSendmail( aWind: TWindow);
		BEGIN
			IF Member(TObject(aWind), TSendmailWindow) THEN BEGIN
				aWind.Select; isNowOpen:= TRUE;
				END;
		END;
		PROCEDURE checkBugmail( aWind: TWindow);
		BEGIN
			IF Member(TObject(aWind), TBugmailWindow) THEN BEGIN
				aWind.Select; isNowOpen:= TRUE;
				END;
		END;
		PROCEDURE checkNCBIfetch( aWind: TWindow);
		BEGIN
			IF Member(TObject(aWind), TNCBIfetchWindow) THEN BEGIN
				aWind.Select; isNowOpen:= TRUE;
				END;
		END;
		PROCEDURE checkUHServer( aWind: TWindow);
		BEGIN
			IF Member(TObject(aWind), TUHServerWindow) THEN BEGIN
				aWind.Select; isNowOpen:= TRUE;
				END;
		END;
		PROCEDURE checkEMBL( aWind: TWindow);
		BEGIN
			IF Member(TObject(aWind), TEMBLFetchWindow) THEN BEGIN
				aWind.Select; isNowOpen:= TRUE;
				END;
		END;
		
BEGIN
	isNowOpen:= FALSE;
	CASE aCommand OF
	
		cSendMail		: 
			BEGIN
			gApplication.ForAllWindowsDo(checkSendmail);
			IF NOT isNowOpen THEN BEGIN
				aSendmail := TSendmailWindow(gViewServer.NewTemplateWindow(kSendmailWindID, NIL));
				FailNIL( aSendmail);
				aSendmail.OpenWindow;
				END;
			END;
			
		cBugMail		:  
			BEGIN
			gApplication.ForAllWindowsDo(checkBugmail);
			IF NOT isNowOpen THEN BEGIN
				aBugmail := TBugmailWindow(gViewServer.NewTemplateWindow(kBugmailWindID, NIL));
				FailNIL( aBugmail);
				aBugmail.OpenWindow;
				END;
			END;
			
		cGBFetch		:  
			BEGIN
			gApplication.ForAllWindowsDo(checkNCBIfetch);
			IF NOT isNowOpen THEN BEGIN
				aNCBIfetch := TNCBIfetchWindow(gViewServer.NewTemplateWindow(kNCBIfetchWindID, NIL));
				FailNIL( aNCBIfetch);
				aNCBIfetch.INCBIfetchWindow;
				aNCBIfetch.ShowReverted; 
				aNCBIfetch.Open; aNCBIfetch.Select;	  
				END;
			END;
		
		cUHServer 	:   
			BEGIN
			gApplication.ForAllWindowsDo(checkUHServer);
			IF NOT isNowOpen THEN BEGIN
				aUHServer := TUHServerWindow(gViewServer.NewTemplateWindow(kUHServerWindID, NIL));
				FailNIL( aUHServer);
				aUHServer.OpenWindow;
				END;
			END;
			
		cEMBLFetch 	: 
			BEGIN
			gApplication.ForAllWindowsDo(checkEMBL);
			IF NOT isNowOpen THEN BEGIN
				aEMBLFetch := TEMBLFetchWindow(gViewServer.NewTemplateWindow(kEMBLFetchWindID, NIL));
				FailNIL( aEMBLFetch);
				aEMBLFetch.OpenWindow;
				END;
			END;
			
		END;
END;


PROCEDURE MailWithSeq( aCommand: Integer; aSeq: TSequence);
VAR
		aBlast	: {TGBBlastWindow;} TNCBIBlastWindow;
		aFasta	: TGBFastAWindow;
		aGeneid	: TGeneidWindow;
		aGenmark: TGenmarkWindow;
		aBlocks	: TBlocksWindow;
		isNowOpen: boolean;
		
		PROCEDURE checkBlast( aWind: TWindow);
		BEGIN
			IF Member(TObject(aWind), TNCBIBlastWindow) THEN BEGIN
			{- IF Member(TObject(aWind), TGBBlastWindow) THEN BEGIN}
				aWind.Select; isNowOpen:= TRUE;
				END;
		END;
		
		PROCEDURE checkFasta( aWind: TWindow);
		BEGIN
			IF Member(TObject(aWind), TGBFastAWindow) THEN BEGIN
				aWind.Select; isNowOpen:= TRUE;
				END;
		END;
		
		PROCEDURE checkGeneid( aWind: TWindow);
		BEGIN
			IF Member(TObject(aWind), TGeneidWindow) THEN BEGIN
				aWind.Select; isNowOpen:= TRUE;
				END;
		END;
		
		PROCEDURE checkBlocks( aWind: TWindow);
		BEGIN
			IF Member(TObject(aWind), TBlocksWindow) THEN BEGIN
				aWind.Select; isNowOpen:= TRUE;
				END;
		END;
		
		PROCEDURE checkGenmark( aWind: TWindow);
		BEGIN
			IF Member(TObject(aWind), TGenmarkWindow) THEN BEGIN
				aWind.Select; isNowOpen:= TRUE;
				END;
		END;
		
BEGIN
	isNowOpen:= FALSE;
	CASE aCommand OF
	
		cGBBlast: BEGIN
				gApplication.ForAllWindowsDo(checkBlast);
				IF NOT isNowOpen THEN BEGIN
					aBlast := TNCBIBlastWindow(gViewServer.NewTemplateWindow(kNCBIBlastWindID, NIL));
					FailNIL(aBlast); 		 
					aBlast.INCBIBlastWindow(aSeq);
					aBlast.ShowReverted; 
					aBlast.Open; aBlast.Select;	  
					END;
				END;
				
		cGBFasta: BEGIN
				gApplication.ForAllWindowsDo(checkFasta);
				if NOT isNowOpen then begin
					aFasta := TGBFastAWindow(gViewServer.NewTemplateWindow(kGBFastAWindID, NIL));
					FailNIL(aFasta); 		 
					aFasta.IGBFastAWindow(aSeq);
					aFasta.ShowReverted; 
					aFasta.Open; aFasta.Select;	 
					END;
			END;
			
		cGeneidSearch: BEGIN
				gApplication.ForAllWindowsDo(checkGeneid);
				if NOT isNowOpen then begin
					aGeneid := TGeneidWindow(gViewServer.NewTemplateWindow(kGeneidWindID, NIL));
					FailNIL(aGeneid); 		 
					aGeneid.IGeneidWindow(aSeq);
					aGeneid.ShowReverted; 
					aGeneid.Open; aGeneid.Select;	  
					END;
				END;
				
		cBlocksSearch: BEGIN
				gApplication.ForAllWindowsDo(checkBlocks);
				if NOT isNowOpen then begin
					aBlocks := TBlocksWindow(gViewServer.NewTemplateWindow(kBlocksWindID, NIL));
					FailNIL(aBlocks); 		 
					aBlocks.IBlocksWindow(aSeq);
					aBlocks.ShowReverted; 
					aBlocks.Open; aBlocks.Select;	 
					END;
				END;
				
		cGenmarkSearch: BEGIN
				gApplication.ForAllWindowsDo(checkGenmark);
				if NOT isNowOpen then begin
					aGenmark := TGenmarkWindow(gViewServer.NewTemplateWindow(kGenmarkWindID, NIL));
					FailNIL(aGenmark); 		 
					aGenmark.IGenmarkWindow(aSeq);
					aGenmark.ShowReverted;  
					aGenmark.Open; aGenmark.Select;	 
					END;
				END;
				
				
		END;
END;


PROCEDURE MailWithSeqList( aCommand: Integer;  aSeqList: TSeqList);
VAR
		aGrail	: TGrailWindow;
		aPythia	: TPythiaWindow;
		isNowOpen: boolean;
		
		PROCEDURE checkGrail( aWind: TWindow);
		BEGIN
			IF Member(TObject(aWind), TGrailWindow) THEN BEGIN
				aWind.Select; isNowOpen:= TRUE;
				END;
		END;
		PROCEDURE checkPythia( aWind: TWindow);
		BEGIN
			IF Member(TObject(aWind), TPythiaWindow) THEN BEGIN
				aWind.Select; isNowOpen:= TRUE;
				END;
		END;
		
BEGIN
	isNowOpen:= FALSE;
	CASE aCommand OF
	
		cGrailSearch: BEGIN
			gApplication.ForAllWindowsDo(checkGrail);
			IF NOT isNowOpen THEN BEGIN
				aGrail := TGrailWindow(gViewServer.NewTemplateWindow(kGrailWindID, NIL));
				FailNIL(aGrail); 		 
				aGrail.IGrailWindow(aSeqList);
				aGrail.ShowReverted; 
				aGrail.Open; aGrail.Select;	 
				END;
			END;
			
		cPythiaSearch: BEGIN
			gApplication.ForAllWindowsDo(checkPythia);
			IF NOT isNowOpen THEN BEGIN
				aPythia := TPythiaWindow(gViewServer.NewTemplateWindow(kPythiaWindID, NIL));
				FailNIL(aPythia); 		 
				aPythia.IPythiaWindow(aSeqList);
				aPythia.ShowReverted;  
				aPythia.Open; aPythia.Select;	 
				END;
			END;
			
		END;
END;


(*****

PROCEDURE OpenFiletransfer;
CONST
	msgMyError = $80001234;
BEGIN
	ParamText('File transfer is not ready','','File Transfer','');	
	Failure( -1, msgMyError);  
END;

********)