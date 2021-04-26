// USeqChild.p 
// by d.g.gilbert, 1992 for SeqApp 


UNIT USeqChild;

INTERFACE


#ifndef __Types__
#include <Types.h>
#endif

#ifndef __Memory__
#include <Memory.h>
#endif

#ifndef __QuickDraw__
#include <QuickDraw.h>
#endif

#ifndef __ToolUtils__
#include <ToolUtils.h>
#endif

#ifndef __OSUtils__
#include <OSUtils.h>
#endif

#ifndef __Resources__
#include <Resources.h>
#endif

#ifndef __Packages__
#include <Packages.h>
#endif

#ifndef __Files__
#include <Files.h>
#endif

#ifndef __Printing__
#include <Printing.h>
#endif

#ifndef __TextEdit__
#include <TextEdit.h>
#endif

#ifndef __Controls__
#include <Controls.h>
#endif

#ifndef __Aliases__
#include <Aliases.h>
#endif

#ifndef __Editions__
#include <Editions.h>
#endif

#ifndef __Events__
#include <Events.h>
#endif

#ifndef __Notification__
#include <Notification.h>
#endif

#ifndef __AppleEvents__
#include <AppleEvents.h>
#endif

#ifndef __Processes__
#include <Processes.h>
#endif

#ifndef __Balloons__
#include <Balloons.h>
#endif

#ifndef __GestaltEqu__
#include <GestaltEqu.h>
#endif

#ifndef __Fonts__
#include <Fonts.h>
#endif

#ifndef __Scrap__
#include <Scrap.h>
#endif

#ifndef __MacAppTypes__
#include <MacAppTypes.h>
#endif

#ifndef __UPascalObject__
#include <UPascalObject.h>
#endif

#ifndef __UObject__
#include <UObject.h>
#endif

#ifndef __UEventHandler__
#include <UEventHandler.h>
#endif

#ifndef __UList__
#include <UList.h>
#endif

#ifndef __UAdorners__
#include <UAdorners.h>
#endif

#ifndef __UStream__
#include <UStream.h>
#endif

#ifndef __UGeometry__
#include <UGeometry.h>
#endif

#ifndef __UEvent__
#include <UEvent.h>
#endif

#ifndef __UCommand__
#include <UCommand.h>
#endif

#ifndef __UCommandHandler__
#include <UCommandHandler.h>
#endif

#ifndef __UView__
#include <UView.h>
#endif

#ifndef __UBehavior__
#include <UBehavior.h>
#endif

#ifndef __UTEView__
#include <UTEView.h>
#endif

#ifndef __UTECommands__
#include <UTECommands.h>
#endif

#ifndef __UMacAppUtilities__
#include <UMacAppUtilities.h>
#endif

#ifndef __UPatch__
#include <UPatch.h>
#endif

#ifndef __UFailure__
#include <UFailure.h>
#endif

#ifndef __UMacAppGlobals__
#include <UMacAppGlobals.h>
#endif

#ifndef __UFile__
#include <UFile.h>
#endif

#ifndef __UApplication__
#include <UApplication.h>
#endif

#ifndef __UPrintHandler__
#include <UPrintHandler.h>
#endif

#ifndef __UPrinting__
#include <UPrinting.h>
#endif

#ifndef __UWindow__
#include <UWindow.h>
#endif

#ifndef __UMenuMgr__
#include <UMenuMgr.h>
#endif

#ifndef __UMemory__
#include <UMemory.h>
#endif

#ifndef __UErrorMgr__
#include <UErrorMgr.h>
#endif

#ifndef __UControl__
#include <UControl.h>
#endif

#ifndef __UDialog__
#include <UDialog.h>
#endif

#ifndef __UPopup__
#include <UPopup.h>
#endif

#ifndef __UScroller__
#include <UScroller.h>
#endif

#ifndef __UDocument__
#include <UDocument.h>
#endif

#ifndef __UFileHandler__
#include <UFileHandler.h>
#endif

#ifndef __UFileBasedDocument__
#include <UFileBasedDocument.h>
#endif

#ifndef __UViewServer__
#include <UViewServer.h>
#endif

#ifndef __UGridView__
#include <UGridView.h>
#endif

#ifndef __UClipboardMgr__
#include <UClipboardMgr.h>
#endif
USES
	
	/*$U $$Shell(UStd)UStandard.p */ UStandard, 
	/*$U $$Shell(UStd)UPlot.p */ UPlot, 
	/*$U $$Shell(UStd)UApp.p */ UApp, 
	/*$U $$Shell(UText)UTextDoc.p */ UTextDoc,
	
	USequence,
	USeqReader,
	USeqDoc();

CONST
	kChildReturnNone	== 0;
	kChildReturnOpenTextFile	== 1;
	kChildReturnOpenSeqFile		== 2;
	kChildReturnOpenTreeFile	== 3;
	kChildReturnOpenPictFile	== 4;
	kChildReturnOpenFile 			== 9;
	kChildReturnNoAction			== 10;
	
	kNewChildTemplateID == 3001;
	kEditChildWindID == 3000;
	
TYPE
	
	TChildCommand	== OBJECT(TObject)
		fMenuName	: Str63;
		fViewID		: integer;
		fMinSeqs	: integer;
		fCommandsTxt, fDescriptionTxt	: Handle;
		
		pascal void TChildCommand::IChildCommand( str63 name, integer viewID, minSeqs,
																		Handle aCommandsTxt, aDescriptionTxt)
		pascal void TChildCommand::LaunchDialog( TSeqList selectedSeqs)
		pascal void TChildCommand::Edit(void)
		pascal void TChildCommand::EnableMenu( MenuHandle aMenuHandle, integer nSelectedSeqs)
		}

	TChildFile == OBJECT(TObject)
		fFile	 : TFile;  //.vRefNum, .fType, .versNum, .fName
		Integer		fAction; //kChildReturnOpenSeqFile,kChildReturnOpenTextFile,...
		Boolean		fOutput; //FALSE=input,TRUE=child outputs
		Boolean		fDelete; //erase when done?
		
		pascal void TChildFile::IChildFile(str255 theName, OSType itsType, 
								integer		itsVRefNum, action; boolean isOutput, doDelete)
		pascal void TChildFile::Free(void) // override 
		}
	
	TChildApp	== OBJECT(TObject)
		fSN			: ProcessSerialNumberPtr;
		fFiles	: TList; //of TChildFile
		
		pascal void TChildApp::IChildApp( theSN:ProcessSerialNumberPtr; TList theFiles)
		pascal void TChildApp::Free(void) // override */ /*delete .ptr, in/out files if needed 
		pascal void TChildApp::Finished(void) // post processing by parent 
		}


	TChildDialog == OBJECT(TPrefWindow)
		fViewID	: integer;
		fCommands, fDescription	: TDlogTextView;
		fHelp		: TButton;
		TChildCommand		fChildCommand;
		
		pascal void TChildDialog::IChildDialog( TChildCommand theCommand, integer theViewID, Handle cmdText, descripText)
		pascal void TChildDialog::SetPrefID(void) /* override */ 

		pascal void TChildDialog::NewChildDialog(Integer itsViewID)  
		pascal void TChildDialog::Edit(void)
		pascal void TChildDialog::SaveEdits(void)
		pascal void TChildDialog::DisplayForLaunch(void)
		pascal void TChildDialog::Launch( TSeqList selectedSeqs)
		pascal void TChildDialog::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
		}

	TChildListView == OBJECT(TTextListView)
		pascal void TChildListView::GetItemText( integer anItem, Str255 VAR aString) // override 
		}
		
	TEditChildWind == OBJECT(TPrefWindow)
		TTextListView		fListView;
		TButton		fDelete, fEdit ;
		
		pascal void TEditChildWind::IEditChildWind(void)
		pascal void TEditChildWind::SetPrefID(void) /* override */ 
		pascal void TEditChildWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
		//- pascal void TEditChildWind::ModalDialog(void)
		}


VAR
	gChildList 		: TList;		 //added on by TChildCommand.LaunchDialog
	TList		gChildCommandList;  //built/maintained by gApplication
						
	pascal void InitChildApps( boolean hasAppleEvents)   
	pascal void OpenEditChild(void)
	pascal void OpenNewChild(void)
	
		// AppleEvent handler for kAEApplicationDied 
	pascal OSErr HandleChildDiedEvent( AppleEvent theAppleEvent, reply, 
																	Longint handlerRefCon)
		
IMPLEMENTATION

		//$I USeqChild.inc.p

}
