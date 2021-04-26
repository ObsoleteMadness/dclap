// USeqDoc.p 
// by d.g.gilbert, Oct 1990 


UNIT USeqDoc;
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

#ifndef __Folders__
#include <Folders.h>
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
USES
	
	/*$U $$Shell(UStd)UStandard.p */ UStandard, 
	/*$U $$Shell(UStd)UApp.p */ UApp, 
	/*$U $$Shell(UText)UTextDoc.p */ UTextDoc,

	USequence,
	USeqReader();


CONST

	mSeqListMouseClick == 1111; // seqList "open" selected seqs msg for DoChoice
		
	
TYPE
	
	TSeqList				== OBJECT (TList)
	
		pascal Boolean TSeqList::ListIsEmpty(void)
		pascal void TSeqList::AddNewSeq(void)
		pascal TSequence TSeqList::SeqAt( integer aRow)  
		pascal void TSeqList::ClearSelections(void)
		pascal void TSeqList::doWrite( Str255 aFileName, integer format)  
		pascal void TSeqList::doWriteRef( integer aFileRef, integer format)  
		pascal Handle TSeqList::doWriteHandle( integer format)  
		
		pascal integer TSeqList::ZeroOrigin(void)
		pascal void TSeqList::MakeConsensus(void)
		pascal TSequence TSeqList::Consensus(void)  //may be NULL
		pascal Integer TSeqList::ConsensusRow(void) //0 if NULL
		
		pascal charsHandle TSeqList::FindCommonBases( integer minCommonPerCent, 
													CharsHandle VAR firstCommon)
		}

	
	TSeqListView			== OBJECT (TTextListView)
		fSeqList	: TSeqList; //list of TSequence. NOTE: TSeqListDoc "owns" this list
		
		pascal void TSeqListView::Initialize(void) // override 
		pascal void TSeqListView::Infoheadline(Str255 var aStr)
		pascal void TSeqListView::GetItemText(integer anItem, Str255 VAR aString)
									// override 
		pascal void TSeqListView::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 
		}
		
	
	TSeqSelectCommand == OBJECT(TCellSelectCommand)
		//Override to process command w/ DoIt?
		boolean		fDoubleClick;
		fAtRow			: integer;
		pascal void TSeqSelectCommand::ComputeNewSelection(GridCell VAR clickedCell) 
											// override 
		pascal void TSeqSelectCommand::DoIt(void) // override 
		}
		
	TSeqListDlog			== OBJECT (TDialogView)
		pascal void TSeqListDlog::Initialize(void) // override 
		pascal void TSeqListDlog::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 
		pascal void TSeqListDlog::CalcMinFrame(VRect VAR minFrame) // override 
		}
		
	TSeqListDoc			== OBJECT (TFileBasedDocument)
		/*this is a "project" document consisting of SeqListView of 1 or more
			sequences from 1 or more files */
			
		fHeadline			:	TStaticText; 	//seqlist header
		fSeqList			: TSeqList; 				//the list of TSequences owned by this doc
		fSeqListView	: TSeqListView; //view showing fSeqList
		fFormatPop		:	TPopup; 			//seq output format
		fUpdateTime		: longint;			//time of last .showreverted/.updateFlds 
		fInFormat			: integer;			//file input format 
		Boolean		fSaveSelection;		
		
		pascal void TSeqListDoc::ISeqListDoc(TFile itsFile)
		pascal void TSeqListDoc::Free(void) // override 
		pascal void TSeqListDoc::FreeData(void) // override 
		pascal void TSeqListDoc::Close(void) // override 
		
		pascal void TSeqListDoc::AppendRead(short aRefNum)
		pascal void TSeqListDoc::DoRead(TFile aFile, Boolean forPrinting) // override 
		pascal void TSeqListDoc::AppendHandle( Handle aHand)
		pascal void TSeqListDoc::DoReadHandle( Handle aHand)
		pascal void TSeqListDoc::DoWrite(TFile aFile, Boolean makingCopy) // override 
		pascal void TSeqListDoc::AboutToSaveFile(TFile theSaveFile, CommandNumber itsCmd, 
															Boolean VAR makingCopy) // override 

		pascal void TSeqListDoc::AddSeqToList(TSequence item)
		pascal void TSeqListDoc::AddNewSeqToList(void)
		pascal Boolean TSeqListDoc::ListIsEmpty(void)
		 
		pascal void TSeqListDoc::DoSeqSelectCommand(void)
		 //override this to open sequences selected w/ dblclk
		 
		pascal void TSeqListDoc::ShowReverted(void) // override 
		pascal void TSeqListDoc::DoMakeViews(Boolean forPrinting) // override 
						
		// Command Handlers 
		pascal void TSeqListDoc::DoMenuCommand(CommandNumber aCommandNumber) // override 
		pascal void TSeqListDoc::DoSetupMenus(void) // override 
		}



	pascal void InitUSequence(void)  //read in std codon table, ...
	

VAR
	gSeqListDoc	: TSeqListDoc; //== gApp->fSeqListDoc
	gREMap			: TREMap;		
	gOutputName	: Str255;		
	
IMPLEMENTATION

		//$I USeqDoc.inc.p

}
