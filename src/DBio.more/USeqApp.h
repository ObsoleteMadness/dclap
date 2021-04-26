// USeqApp.p 
// by d.g.gilbert, Oct 1990 


UNIT USeqApp;

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

#ifndef __SANE__
#include <SANE.h>
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
	/*$U $$Shell(UStd)UMath.p */ UMath, 
	/*$U $$Shell(UStd)UPlot.p */ UPlot, 
	/*$U $$Shell(UStd)UApp.p */ UApp, 
	/*$U $$Shell(UText)UTextDoc.p */ UTextDoc,
	/*$U $$Shell(UTCP)UTCP.p */ UTCP, 
	/*$U $$Shell(UTCP)USMTP.p */ USMTP, 
	/*$U $$Shell(UTCP)UGopher.p */ UGopher, 
	/*$U $$Shell(UTCP)UPOP.p */ UPOP,
	
	USequence,
	USeqReader,
	USeqDoc,
	UGridStuff,
	USeqMail,
	USeqGopher,
	USeqPrint,
	USeqChild();


TYPE
			//! remember to unDeadStrip any new objects !
							

	TSeqAppApplication == OBJECT (TPrefApplication)
		
		pascal void TSeqAppApplication::ISeqAppApplication(void)
		pascal void TSeqAppApplication::DoAboutBox(void) // override 
		pascal void TSeqAppApplication::DoAppleCommand( CommandNumber aCommandNumber,
												 AppleEvent message, reply) // override 
		pascal void TSeqAppApplication::InstallHelpMenuItems(void) // override 

		pascal CommandNumber TSeqAppApplication::KindOfDocument(CommandNumber itsCommandNumber,
											 TFile itsFile) // override 
		pascal void TSeqAppApplication::GetStandardFileParameters(CommandNumber itsCommandNumber,
								 ProcPtr		VAR fileFilter; TypeListHandle VAR typeList,
								 short		VAR dlgID;  Point VAR where, VAR dlgHook, modalFilter: ProcPtr;
								 Ptr		VAR activeList; ProcPtr VAR activateProc, VAR yourDataPtr: UNIV Ptr); 
								 	// override 
		pascal void TSeqAppApplication::OpenOld(CommandNumber itsOpenCommand, TList aFileList)
						 		// override 
		pascal Boolean TSeqAppApplication::TestForSeqFile(TFile aFile)
		pascal TDocument TSeqAppApplication::DoMakeDocument(CommandNumber itsCommandNumber,
										TFile itsFile) // override 
		
		pascal void TSeqAppApplication::DoSetupMenus(void) // override 
		pascal void TSeqAppApplication::DoMenuCommand(aCommandNumber:CommandNumber); // override 
		pascal void TSeqAppApplication::AboutToLoseControl(Boolean convertClipboard) // override 
				
		pascal void TSeqAppApplication::ShowError(OSErr error,  LongInt message) // override 
		}

	TSeqedView == OBJECT; FORWARD;
	TSeqedWindow	== OBJECT (TPrefWindow)
		fSequence		: TSequence; // seq we are editing
		fSeqedView	: TSeqedView; 
		fSeqName		: TEditText;
		fReformBut	: TButton; 		//reformat seq
		
		pascal void TSeqedWindow::ISeqedWindow(TSequence aSeq)
		pascal void TSeqedWindow::ReplaceSeq(TSequence aSeq)
		pascal void TSeqedWindow::SetPrefID(void)  // override 
		pascal void TSeqedWindow::ShowReverted(void) // override 
		pascal void TSeqedWindow::UpdateEdits(void) 
		pascal void TSeqedWindow::Close(void) /* override */ 
		pascal void TSeqedWindow::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
		}

		
	TSeqIndex == OBJECT; FORWARD;
	TSeqHIndex == OBJECT; FORWARD;

	TSeqedView			== OBJECT (TTextView)
		integer		fOldStart, fOldEnd, fOldLen;  //save selStart,End for redraw check
		fOldDest, fOldView	: Rect; //save Dest Rect for SeqIndex redraw check
		integer		fOldKind, fOldCheck;
		fSequence		: TSequence;
		
		fSeqIndex		: TSeqIndex;
		fSeqHIndex	: TSeqHIndex;
		fSeqCheck, fSeqStart, fSeqEnd, fSeqLen, 
		TStaticText		fSeqSel, fSeqTyp;
		fCodePop	:	TPopup; //dna/rna/iupac/amino/all
		fChanged	: Boolean; //flag if any edits in view
		
		pascal void TSeqedView::Initialize(void) // override 
		pascal void TSeqedView::ISeqedView( TSequence aSeq)
		pascal void TSeqedView::Free(void) // override 
		pascal void TSeqedView::FreeData(void)  
		
		//call this when seq indices change
		pascal void TSeqedView::UpdateCtls(boolean forceIndex)  
		
		//! Override some TEView methods to call UpdateCtls 
		pascal void TSeqedView::SynchView(boolean redraw) /* override */  
		pascal void TSeqedView::RecalcText(void) /* override */  
		pascal void TSeqedView::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 

		pascal void TSeqedView::DoSetupMenus(void) // override 
		pascal void TSeqedView::DoMenuCommand(aCommandNumber:CommandNumber); // override 
		
		//! install wordbreaker
		pascal void TSeqedView::MakeTERecord(void) // override 
		
		//! override this to filter sequence entry as well as updateCtls
		pascal void TSeqedView::DoKeyEvent( TToolboxEvent event) // override 
		
		//keep cursor at arrow when teview is not selected (inactive "edittext")
		pascal void TSeqedView::DoSetCursor(VPoint localPoint, RgnHandle cursorRegion)	// override 
				
		//use to select this view for editing in DLog environment 
		pascal TSeqedView::HandleMouseDown( VPoint theMouse, TToolboxEvent event, 
							 hysteresis: Point):Boolean; // override 
		}

	TSeqIndex			== OBJECT (TControl)
		fTEView	: TTEView; 
		pascal void TSeqIndex::Initialize(void) // override 
		pascal void TSeqIndex::CalcMinFrame(VRect VAR minFrame) // override 
		pascal void TSeqIndex::Draw(VRect area) // override 
		}
		
	TSeqHIndex			== OBJECT (TControl)
		fTEView	: TTEView;
		pascal void TSeqHIndex::Draw(VRect area) // override 
		}




	TAlnSequence	== OBJECT (TTextView) 
		fSeq				: TSequence;

		pascal void TAlnSequence::IAlnSequence(TSequence aSeq)
		pascal void TAlnSequence::ReplaceSeq(TSequence aSeq)
		pascal void TAlnSequence::Free(void) // override 

		pascal void TAlnSequence::DeInstall(void) 
		
		pascal void TAlnSequence::MakeTERecord(void) // override 
		pascal void TAlnSequence::ShowReverted(void) // override */ /*?
		pascal TAlnSequence::HandleMouseDown( VPoint theMouse, TToolboxEvent event, 
							 hysteresis: Point):Boolean; // override 

		pascal void TAlnSequence::DoMenuCommand(aCommandNumber:CommandNumber); // override 
		pascal void TAlnSequence::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) /* override */				
		pascal void TAlnSequence::DoKeyEvent( TToolboxEvent event) // override 
		pascal TTETypingCommand TAlnSequence::DoMakeTypingCommand(Char ch) // override 
		}
		
	TAlnseqTypingCommand == OBJECT( TTETypingCommand)
		pascal void TAlnseqTypingCommand::DoNormalChar( Char aChar) // override 
		}

	TAlnDoc == OBJECT; FORWARD;
	
	TAlnView			== OBJECT (TGridView)
		fAlnDoc			: TAlnDoc; // our owner
		fAlnList		: TSeqList; 				//list of TSequence
		fRowHeight	: integer;
		fEditSeq		: TAlnSequence; // 1 floating TAlnSequence 
		fEditRow		: integer;
		fLocked			: Boolean;		//edits disallowed ?
		
		pascal void TAlnView::IAlnView( TAlnDoc itsDocument, TSeqList aSeqList)
		pascal void TAlnView::FreeData(void)
		pascal void TAlnView::Free(void) // override 
		pascal void TAlnView::Close(void) /* override */ 

		pascal void TAlnView::UpdateWidth( TSequence aSeq)
		pascal void TAlnView::UpdateAllWidths(void)
		pascal void TAlnView::UpdateSize(void)
		
		pascal void TAlnView::registerInsertLast( TSequence aSeq)
		pascal void TAlnView::addToAlnList( TSequence aSeq)
		pascal TSequence TAlnView::SeqAt( integer aRow)  
		pascal void TAlnView::MakeConsensus(void)
		pascal charsHandle TAlnView::FindCommonBases( integer minCommonPerCent)
		pascal void TAlnView::HiliteCommonBases(void)
		pascal void TAlnView::DeInstallEditSeq(void)
		pascal void TAlnView::InstallEditSeq(integer row, selStart,selEnd, boolean doLight)
		
		pascal void TAlnView::CalcMinFrame(VRect VAR minFrame) // override 
		pascal void TAlnView::DrawContents(void) // override 
		pascal void TAlnView::DrawRangeOfCells(GridCell startCell, stopCell, VRect aRect) 
					/* override */  
		pascal void TAlnView::DrawCell(aCell:GridCell; VRect aRect)
					// override 
					//test - debugging
		pascal void TAlnView::CellToVRect( GridCell aCell, VAR aRect:VRect); // override 
		pascal void TAlnView::Draw(VRect area) /* override */ 
		pascal TAlnView::VPointToLastCell( aPoint: VPoint):GridCell; // override 
		pascal void TAlnView::UpdateCoordinates(void) // override 
		pascal Boolean TAlnView::Focus(void) // override 
					
		pascal Boolean TAlnView::ContainsClipType(ResType aType) // override 
		pascal void TAlnView::WriteToDeskScrap(void) // override 
		
		pascal void TAlnView::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 
		pascal TAlnView::HandleMouseDown( VPoint theMouse, TToolboxEvent event, 
							 hysteresis: Point):Boolean; // override 
		}
		
		
	TAlnIndex			== OBJECT (TTextListView) //? or Scroller type or Grid type?
		fAlnList	: TSeqList;
		fAlnView	: TAlnView;
		
		pascal void TAlnIndex::GetItemText(integer anItem, str255 var aString) // override 
		pascal void TAlnIndex::ReSelect(RgnHandle indexRegion)
		pascal void TAlnIndex::UpdateSize(void)

		pascal TAlnIndex::HandleMouseDown( VPoint theMouse, TToolboxEvent event, 
							 hysteresis: Point):Boolean; // override 
		pascal void TAlnIndex::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 
		}
		
	TAlnHIndex			== OBJECT (TTextGridView)
		fAlnView	: TAlnView;
	//- fSeq		: TAlnSequence; 
		pascal void TAlnHIndex::UpdateWidth(void)
		pascal void TAlnHIndex::Draw(VRect area) // override 
		pascal void TAlnHIndex::DrawCell(GridCell aCell, VRect aRect) // override 
		pascal void TAlnHIndex::GetText(aCell:GridCell; Str255 VAR aString) // override 
		pascal TAlnHIndex::HandleMouseDown( VPoint theMouse, TToolboxEvent event, 
							 hysteresis: Point):Boolean; // override 
		}
					
	TSideScroller == OBJECT; FORWARD;
	TAlnScroller 	== OBJECT (TScroller)
	/* TAlnScroller takes care of scrolling both the aln and the rows or columns view,
		as appropriate */
		fRowScroller		:	TSideScroller;		 
		fColumnScroller	:	TSideScroller;	 

		pascal void TAlnScroller::Free(void) // override 
		pascal void TAlnScroller::DoScroll(VPoint delta, Boolean redraw) // override 
		pascal void TAlnScroller::SingleScroll(VPoint delta, Boolean redraw)
		}	 

	TSideScroller 	== OBJECT (TScroller)
		fMainScroller		:	TAlnScroller;		 
		pascal void TSideScroller::Free(void) // override 
		pascal void TSideScroller::DoScroll(VPoint delta, Boolean redraw) // override 
		pascal void TSideScroller::SingleScroll(VPoint delta, Boolean redraw)
		}


	TAlnWindow	== OBJECT (TPrefWindow)
		pascal void TAlnWindow::IAlnWindow(void)
		pascal void TAlnWindow::SetPrefID(void) /* override */ 
		}

	TAlnDlog			== OBJECT (TDialogView)
		pascal void TAlnDlog::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 
		pascal void TAlnDlog::CalcMinFrame(VRect VAR minFrame) // override 
		}
		
	TAlnDoc	== OBJECT (TSeqListDoc)
		fAlnView		: TAlnView; 
		fAlnIndex		: TAlnIndex;
		fAlnHIndex	: TAlnHIndex;
		fAlnWindow	: TAlnWindow;
		fUseColor		: boolean;
		fLockButton,
		TIcon		fColorButton, fMonoButton;
		
		pascal void TAlnDoc::IAlnDoc( TFile itsFile)
		pascal Integer TAlnDoc::SelectionToFile( Boolean AllatNoSelection,
								Str255		aFileName; integer seqFormat)  //return # written 
		pascal void TAlnDoc::ShowReverted(void) // override 
		pascal void TAlnDoc::Close(void) /* override */ 
		pascal void TAlnDoc::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 

		//overrides of TSeqListDoc
		pascal void TAlnDoc::AddSeqToList(TSequence item)  // override 
		pascal void TAlnDoc::DoMakeViews(Boolean forPrinting) // override 
		pascal void TAlnDoc::DoWrite(TFile aFile, Boolean makingCopy) // override 

		pascal void TAlnDoc::GetSelection( boolean equalCount, allAtNoSelection,
																TSeqList		VAR aSeqList; longint VAR start, nbases)
		pascal void TAlnDoc::FirstSelection( TSequence VAR aSeq, longint VAR start, nbases)		
		pascal void TAlnDoc::OpenSeqedWindow(TSequence aSeq)
		pascal void TAlnDoc::OpenSeqs( boolean editOnly) 
		pascal void TAlnDoc::DoMenuCommand(CommandNumber aCommandNumber) // override 
		pascal void TAlnDoc::DoSetupMenus(void) // override 
		}
	
	TSeqChangeCmd		== OBJECT (TCommand)
		TAlnView		fAlnView;
		TSeqList		fOldSeqs;
		TSeqList		fNewSeqs;
		
		pascal void TSeqChangeCmd::ISeqChangeCmd( itsAlnDoc	: TAlnDoc; 
																	TView		itsView;  TSeqList itsSeqs )
		pascal TSequence TSeqChangeCmd::ChangeToNew( oldSeq:TSequence);
		pascal void TSeqChangeCmd::DoIt(void) // override 
		pascal void TSeqChangeCmd::UndoIt(void) // override 
		pascal void TSeqChangeCmd::RedoIt(void) // override 
		pascal void TSeqChangeCmd::Commit(void) // override 
		pascal void TSeqChangeCmd::Free(void) // override 
		}		 
 
	TSeqReverseCmd		== OBJECT (TSeqChangeCmd)
		pascal TSequence TSeqReverseCmd::ChangeToNew( oldSeq:TSequence); // override 
		}
	TSeqComplementCmd		== OBJECT (TSeqChangeCmd)
		pascal TSequence TSeqComplementCmd::ChangeToNew( oldSeq:TSequence); // override 
		}
	TSeqRevComplCmd		== OBJECT (TSeqChangeCmd)
		pascal TSequence TSeqRevComplCmd::ChangeToNew( oldSeq:TSequence); // override 
		}
	TSeqCompressCmd		== OBJECT (TSeqChangeCmd)
		pascal TSequence TSeqCompressCmd::ChangeToNew( oldSeq:TSequence); // override 
		}
	TSeqDna2RnaCmd		== OBJECT (TSeqChangeCmd)
		pascal TSequence TSeqDna2RnaCmd::ChangeToNew( oldSeq:TSequence); // override 
		}
	TSeqRna2DnaCmd		== OBJECT (TSeqChangeCmd)
		pascal TSequence TSeqRna2DnaCmd::ChangeToNew( oldSeq:TSequence); // override 
		}
	TSeqTranslateCmd		== OBJECT (TSeqChangeCmd)
		pascal TSequence TSeqTranslateCmd::ChangeToNew( oldSeq:TSequence); // override 
		}
	TSeqLockIndelsCmd		== OBJECT (TSeqChangeCmd)
		pascal TSequence TSeqLockIndelsCmd::ChangeToNew( oldSeq:TSequence); // override 
		}
	TSeqUnlockIndelsCmd		== OBJECT (TSeqChangeCmd)
		pascal TSequence TSeqUnlockIndelsCmd::ChangeToNew( oldSeq:TSequence); // override 
		}
	

		
	TPrefsDialog	== OBJECT (TPrefWindow)
		pascal void TPrefsDialog::IPrefsDialog(void)
		pascal void TPrefsDialog::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) /* override */ 
		pascal void TPrefsDialog::SetPrefID(void) /* override */ 
		}
		
		
	TAlnShifter		== OBJECT (TRCshifter)
		TAlnView		fAlnView;
		fName		: SeqName;
		pascal void TAlnShifter::IAlnShifter( TGridView itsView, 
									TAlnView		itsAlnView; integer atRC, seqName aName)
		pascal void TAlnShifter::DoShift( integer fromRC, toRC) // override 
		}
		
	TAlnSlider		== OBJECT (TRCshifter)
		TAlnView		fAlnView;
		TSeqList		fNewSeqs;
		TSeqList		fOldSeqs;
		RgnHandle		fOldSelection;
		RgnHandle		fNewSelection;

		pascal void TAlnSlider::IAlnSlider( TAlnView itsAlnView, integer atRC)
		pascal void TAlnSlider::Free(void) // override 
		pascal void TAlnSlider::DrawFeedback( VPoint anchorPoint, nextPoint) // override 
		pascal TTracker TAlnSlider::TrackMouse(TrackPhase aTrackPhase, VAR anchorPoint, previousPoint,
								 VPoint		nextPoint; Boolean mouseDidMove) // override 
		pascal void TAlnSlider::DoIt(void) // override 
		pascal void TAlnSlider::UndoIt(void) // override 
		pascal void TAlnSlider::RedoIt(void) // override 
		pascal void TAlnSlider::Commit(void) // override 
		}
		

	TAlnEditCommand	== OBJECT (TCommand)
			// handles the cCopy, cCut, and cClear commands for fAlnIndex selections. 
		fAlnDoc		: TAlnDoc;
		fOldList	: TSeqList;			// save orig List (not seqs) for Undo/Redo...
		fSelection	: RgnHandle;			// seqs selected when command was created 

		pascal void TAlnEditCommand::IAlnEditCommand(TAlnDoc itsDocument, short itsCommand)
		pascal void TAlnEditCommand::Free(void) // override 
		pascal void TAlnEditCommand::DoIt(void) // override 
		pascal void TAlnEditCommand::UndoIt(void) // override 
		pascal void TAlnEditCommand::RedoIt(void) // override 
		pascal void TAlnEditCommand::Commit(void) // override 
		pascal void TAlnEditCommand::CopySelection(void)
			// Copy the currently selected seqs into the clipboard 
		pascal void TAlnEditCommand::DeleteSelection(void)
			// Delete the currently selected seq(s) 
		pascal void TAlnEditCommand::RestoreSelection(void)
			// Restore the deleted seqs 
		pascal void TAlnEditCommand::ReSelect(void)
			// Change the selection back to what it was when this command was created 
		}				 

	TAlnPasteCommand	== OBJECT (TCommand)
		fClipDocument	: TAlnDoc;
		fAlnDoc				: TAlnDoc;
		fSelection		: RgnHandle; 	// seqs selected when command was created 
		fReplacedSeqs	: TSeqList;		// the seqs we pasted over 
		fClipList			: TSeqList;
		fInsRow				: Integer;
		
		pascal void TAlnPasteCommand::IAlnPasteCommand(TAlnDoc itsDocument)
		pascal void TAlnPasteCommand::Free(void) // override 
		pascal void TAlnPasteCommand::DoIt(void) // override 
		pascal void TAlnPasteCommand::UndoIt(void) // override 
		pascal void TAlnPasteCommand::RedoIt(void) // override 
		pascal void TAlnPasteCommand::Commit(void) // override 
		pascal void TAlnPasteCommand::UpdateViews(void)
		}		 
		
		
VAR
		gSeqAppApplication	: TSeqAppApplication;
		
IMPLEMENTATION

		//$I USeqApp.app.p
		//$I USeqApp.edit1.p
		//$I USeqApp.aln.p

}
