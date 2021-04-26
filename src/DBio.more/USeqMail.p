{ USeqMail.p }
{ Copyright 1992 by d.g.gilbert, for SeqApp }


UNIT USeqMail;

INTERFACE

USES
	Types, Memory, QuickDraw, ToolUtils, OSUtils, Resources, Packages, 
	Files, Printing, TextEdit,
	Controls, Aliases, Editions, Events, Notification, 
	AppleEvents, Processes, Balloons,
	
	MacAppTypes, UPascalObject, UObject, 
	UEventHandler, UList, UAdorners, UStream, UGeometry,
	UEvent, UCommand, UCommandHandler,
	UView,  UBehavior, UTEView, UTECommands,
	UMacAppUtilities, UPatch, UFailure, UMacAppGlobals,
	
	UFile, UApplication,
	UPrintHandler, UPrinting,
	UWindow, UMenuMgr, UMemory, UErrorMgr,
	
	UControl, UDialog, UPopup,
	UDocument, UFileHandler, UFileBasedDocument,
	
	UViewServer, UGridView,
	
	{$U $$Shell(UStd)UStandard.p } UStandard, 
	{$U $$Shell(UStd)UApp.p } UApp, 
	{$U $$Shell(UText)UTextDoc.p } UTextDoc,
	{$U $$Shell(UStd)UPlot.p } UPlot, 
	{$U $$Shell(UTCP)UTCP.p } UTCP, 
	{$U $$Shell(UTCP)USMTP.p } USMTP, 
	{$U $$Shell(UTCP)UPOP.p } UPOP, 

	USequence,
	USeqReader,
	USeqDoc;
	
TYPE
{----							
	THiddenText	= OBJECT (TEditText)
		PROCEDURE THiddenText.DoSubstitution(VAR theText: Str255); OVERRIDE;
		PROCEDURE THiddenText.StopEdit; OVERRIDE;
		END;
----}

	TMailPrefWindow	= OBJECT (TPrefWindow)
		fFrom, fSMTPHost, fPOPAddress: TPrefEditText;
		fPassword: THiddenText;
		fKeepPass: TCheckBox;
		
		PROCEDURE TMailPrefWindow.IMailPrefWindow;
		PROCEDURE TMailPrefWindow.SetGlobals;  
		PROCEDURE TMailPrefWindow.ModalDialog;  
		PROCEDURE TMailPrefWindow.SetPrefID; OVERRIDE; 
		END;


	TSendmailWindow	= OBJECT (TPrefWindow)
		fSendmailView	: TDlogTextView; 
		fTo, fSubject: TEditText;
		fStatus			: TStaticText;
		fUnlockTexts: TCheckBox;		
		fCopySelf		: TCheckBox;
		
		PROCEDURE TSendmailWindow.ISendmailWindow;
		PROCEDURE TSendmailWindow.OpenWindow;
		
		PROCEDURE TSendmailWindow.MakeSMTP( theTo, theSubject, theCCopy: Str255; 
															theMessage: Handle; isTEHandle: Boolean); 
		PROCEDURE TSendmailWindow.Sendit; 
		PROCEDURE TSendmailWindow.UnlockTexts;
		PROCEDURE TSendmailWindow.RelockTexts;
		PROCEDURE TSendmailWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
		PROCEDURE TSendmailWindow.SetPrefID; OVERRIDE; 
		END;
				
				

	TPopmailWindow	= OBJECT (TPrefWindow)  
		fMailView	: TDlogTextView; 
		fFrom, fSubject: TEditText;
		fStatus		: TStaticText;
		fDelBut, fLastBut, fNextBut, fSaveBut: TButton;	
		fDelCheck	: TCheckBox;
		fCountTxt	: TStaticText;
		fPop			: TObject; {TObject == TPOP, kept private from interface}
		fMsgNum		: Integer;
		
		PROCEDURE TPopmailWindow.IPopmailWindow;
		FUNCTION  TPopmailWindow.FetchOne(theMsgNum: integer): Boolean; 
		PROCEDURE TPopmailWindow.OpenPop; 
		PROCEDURE TPopmailWindow.SaveMail; 
		PROCEDURE TPopmailWindow.DoEvent(eventNumber: EventNumber; 
											source: TEventHandler; event: TEvent); OVERRIDE; 
		PROCEDURE TPopmailWindow.Close; OVERRIDE;
		PROCEDURE TPopmailWindow.SetPrefID; OVERRIDE;  
		END;


VAR
		{ mail prefs }
	gTCPIsInstalled: Boolean;
	gMyMailAddress: Str255;
	gSMTPHost			: Str63;
	gMyPOPAddress	: Str255;
	gPassword			: Str63;
	gMailerID			: Str63;

PROCEDURE InitializeMailPrefs;
PROCEDURE OpenMailPref;
PROCEDURE OpenCheckmail;

PROCEDURE MailTo( aCommand: Integer);
PROCEDURE MailWithSeq( aCommand: Integer; aSeq: TSequence);
PROCEDURE MailWithSeqList( aCommand: Integer; aSeqList: TSeqList);

{--------
PROCEDURE OpenSendmail;
PROCEDURE	OpenEMBLFetch;
PROCEDURE	OpenUHServer;
PROCEDURE	OpenGBFetch;
PROCEDURE OpenBugMail;

PROCEDURE OpenGBBlast( aSeq: TSequence);
PROCEDURE OpenGBFasta( aSeq: TSequence);
PROCEDURE OpenGeneID( aSeq: TSequence);
PROCEDURE OpenGrail( aSeqList: TSeqList);
-------}

		
IMPLEMENTATION

		{$I USeqMail.inc.p}

END.
