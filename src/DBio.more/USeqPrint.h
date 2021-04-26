// USeqPrint.p 
// by d.g.gilbert, 1992 for SeqApp 


UNIT USeqPrint;

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

#ifndef __Fonts__
#include <Fonts.h>
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
	
	USequence,
	USeqReader,
	USeqDoc();

TYPE

	TSeqPrintDocument		== OBJECT; FORWARD;
	
	TSeqPrintView			== OBJECT (TGridView)  
		fSeq		: TSequence;  
		integer		fFirstBase, fNBases; //index each TSequence

		fLeftName, fRightName, 
		TCheckBox		fTopIndex, fLeftIndex, fRightIndex ;
		TDlogTextView		fStyleName, fStyleBase, fStyleNums; 
		TextStyle		fNameStyle, fNumStyle, fBaseStyle;

		fDoTopIndex, fDoLeftIndex, fDoRightIndex, 
		boolean		fUseColor, fDoLeftName, fDoRightName;
		Integer		fNameWidth, fIndexWidth, fBaseWidth;
		Integer		fBasesPerLine, fTenSpacer;
		
		TSeqPrintDocument		fSeqPrintDocument; 	

		pascal void TSeqPrintView::Initialize(void) // override 
		pascal void TSeqPrintView::Free(void) // override 
		pascal void TSeqPrintView::ISeqPrintView(TSeqPrintDocument itsDocument,
									Boolean forClipboard)
		pascal void TSeqPrintView::DoPostCreate(TDocument itsDocument) // override 
		
		pascal void TSeqPrintView::InstallControls(void)
		pascal void TSeqPrintView::FindNameWidth(void)
		pascal void TSeqPrintView::SetDrawOptions(void) 
		
		pascal void TSeqPrintView::DrawCell(GridCell aCell, VRect aRect) // override 
		pascal void TSeqPrintView::DrawRangeOfCells(GridCell startCell, stopCell, VRect aRect) 
														// override 
		pascal void TSeqPrintView::CalcMinFrame(VRect VAR minFrame) // override 
		
		pascal void TSeqPrintView::DoMenuCommand(CommandNumber aCommandNumber) // override 
		pascal void TSeqPrintView::DoSetupMenus(void) // override 
		}

	TSeqPrintWind			== OBJECT (TPrefWindow)
		TButton		fRedrawBut;
	
		pascal void TSeqPrintWind::ISeqPrintWind(void)
		pascal void TSeqPrintWind::SetPrefID(void) /* override */ 
		pascal void TSeqPrintWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
		}
							
	TSeqPrintDocument		== OBJECT (TFileBasedDocument)
		fSeqPrintView	: TSeqPrintView;
		fSeqPrintWind	: TSeqPrintWind;
		fParentDoc		: TDocument;
		fSeq					: TSequence;  
		longint		fFirstBase, fNBases; //index each TSequence
		fColorButton, fMonoButton	: TIcon;
		
		pascal void TSeqPrintDocument::ISeqPrintDocument(OSType fileType, TDocument parentDoc, 
										TSequence		aSeq; longint startbase, nbases)
		pascal void TSeqPrintDocument::Free(void) // override 
		pascal void TSeqPrintDocument::FreeData(void) // override 
		pascal void TSeqPrintDocument::Close(void) /* override */ 

		pascal void TSeqPrintDocument::DoMakeViews(Boolean forPrinting) // override 
		pascal void TSeqPrintDocument::UntitledName(Str255 VAR noName) // override 
		pascal TFile TSeqPrintDocument::DoMakeFile(CommandNumber itsCommandNumber) // override 
		pascal void TSeqPrintDocument::DoNeedDiskSpace(TFile itsFile,
															long VAR dataForkBytes, rsrcForkBytes) // override 
		pascal void TSeqPrintDocument::DoRead(aFile:TFile; Boolean forPrinting) // override 
		pascal void TSeqPrintDocument::DoWrite(TFile aFile, Boolean makingCopy) // override 
		}
		
		
	TAlnPrintDocument		== OBJECT; FORWARD;
							
	TAlnPrintWind			== OBJECT (TPrefWindow)
		TButton		fRedrawBut;
		TButton		fUserSelect, fUserNotSelect, fUserClear;
		TNumberText		fCommonNumbers;
	
		pascal void TAlnPrintWind::IAlnPrintWind(void)
		pascal void TAlnPrintWind::SetPrefID(void) /* override */ 
		pascal void TAlnPrintWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
		}
		
	TAlnPrintView			== OBJECT (TGridView) //was TView
		fAlnList		: TSeqList; //list of TSequence in print view
		integer		fNumSeqs, fFirstBase, fNBases; //index each TSequence
		charsHandle		fCommonBases, fFirstCommon;
		integer		fCommonPercent;		
		RgnHandle		fUserSelection;

		TNumberText		fCommonNumbers;
		fShadePop		: TPopup;
		fLeftName, fRightName, 
		TCheckBox		fTopIndex, fLeftIndex, fRightIndex ;
		TCheckBox		fUseShade, fUseMatch;
		fMatchSym	: TEditText;
		TDlogTextView		fStyleName, fStyleBase, fStyleNums; 
		TextStyle		fNameStyle, fNumStyle, fBaseStyle;

		fUseColor, fDoUseShading, fDoUseMatch,
		fDoTopIndex, fDoLeftIndex, fDoRightIndex, 
		boolean		fDoLeftName, fDoRightName;
		fMatchChar	: Char;
		Integer		fShadeStyle;
		Integer		fNameWidth, fIndexWidth, fBaseWidth;
		Integer		fBasesPerLine, fTenSpacer;
		
		TAlnPrintDocument		fAlnPrintDocument; 	

		pascal void TAlnPrintView::Initialize(void) // override 
		pascal void TAlnPrintView::Free(void) // override 
		pascal void TAlnPrintView::IAlnPrintView(TAlnPrintDocument itsDocument,
									Boolean forClipboard)
		pascal void TAlnPrintView::DoPostCreate(TDocument itsDocument) // override 
		
		pascal void TAlnPrintView::InstallControls(void)
		pascal void TAlnPrintView::AddSeqs(void)
		pascal void TAlnPrintView::FindNameWidth(void)
		pascal void TAlnPrintView::SetDrawOptions(void) 
		
		pascal void TAlnPrintView::DrawCell(GridCell aCell, VRect aRect) // override 
		pascal void TAlnPrintView::DrawRangeOfCells(GridCell startCell, stopCell, VRect aRect) 
														// override 
		pascal void TAlnPrintView::CalcMinFrame(VRect VAR minFrame) // override 
		
		pascal void TAlnPrintView::DoMenuCommand(CommandNumber aCommandNumber) // override 
		pascal void TAlnPrintView::DoSetupMenus(void) // override 
		}

	TAlnPrintDocument		== OBJECT (TFileBasedDocument) //? TSeqPrintDocument ?
		fAlnPrintView	: TAlnPrintView;
		fAlnPrintWind	: TAlnPrintWind;
		fParentDoc		: TDocument;
		fAlnList			: TSeqList; //list of TSequence in print view
		longint		fFirstBase, fNBases; //index each TSequence
		fColorButton, fMonoButton	: TIcon;
		
		pascal void TAlnPrintDocument::IAlnPrintDocument(OSType fileType, TDocument parentDoc, 
										TSeqList		aAlnList; longint startbase, nbases)
		pascal void TAlnPrintDocument::Free(void) // override 
		pascal void TAlnPrintDocument::FreeData(void) // override 
		pascal void TAlnPrintDocument::Close(void) /* override */ 

		pascal void TAlnPrintDocument::DoMakeViews(Boolean forPrinting) // override 
		pascal void TAlnPrintDocument::UntitledName(Str255 VAR noName) // override 
		pascal TFile TAlnPrintDocument::DoMakeFile(CommandNumber itsCommandNumber) // override 
		pascal void TAlnPrintDocument::DoNeedDiskSpace(TFile itsFile,
															long VAR dataForkBytes, rsrcForkBytes) // override 
		pascal void TAlnPrintDocument::DoRead(aFile:TFile; Boolean forPrinting) // override 
		pascal void TAlnPrintDocument::DoWrite(TFile aFile, Boolean makingCopy) // override 
		}


	TDottyPlotDocument		== OBJECT; FORWARD;

	TDottyPlotWind			== OBJECT (TPrefWindow)
		TNumberText		fNucWindow, fNucMatches;
		fAllDots	: TCheckBox;
		TButton		fRedrawBut;
		
		pascal void TDottyPlotWind::IDottyPlotWind(void)
		pascal void TDottyPlotWind::SetPrefID(void) /* override */ 
		pascal void TDottyPlotWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
		}
		
	TDottyPlotView			== OBJECT (TView)
		fAlnList		: TSeqList; //list of TSequence in print view
		integer		fFirstBase, fNBases; //index each TSequence
		fMinSize		: VPoint;
		TNumberText		fNucWindow, fNucMatches;
		fAllDots		: TCheckBox;
		fDotsH			: Handle;
		TSequence		fSeqH, fSeqV;
		fDotRect		: Rect;
		integer		fNucWindowVal, fNucMatchesVal;
		
		TDottyPlotDocument		fDottyPlotDocument; 	
		TScroller		fScroller;			 

		pascal void TDottyPlotView::IDottyPlotView(TDottyPlotDocument itsDocument,
										Boolean forClipboard)
		pascal void TDottyPlotView::Free(void) // override 
		pascal void TDottyPlotView::FreeData(void)

		pascal void TDottyPlotView::Initialize(void) // override 
		pascal void TDottyPlotView::DoPostCreate(TDocument itsDocument) // override 
		
		pascal void TDottyPlotView::SetDotRect(void)
		pascal void TDottyPlotView::FindDots(void)
		
		pascal void TDottyPlotView::CalcMinFrame(VRect VAR minFrame) // override 

		pascal void TDottyPlotView::DoMenuCommand(CommandNumber aCommandNumber) // override 
		pascal void TDottyPlotView::DoSetupMenus(void) // override 

		pascal void TDottyPlotView::Draw(VRect area) // override 
		}
		
	TDottyPlotDocument		== OBJECT (TFileBasedDocument)
		fDottyPlotView	: TDottyPlotView;
		fDottyPlotWind	: TDottyPlotWind;
		fParentDoc			: TDocument;
		fAlnList				: TSeqList; //list of TSequence in print view
		longint		fFirstBase, fNBases; //index each TSequence
		
		pascal void TDottyPlotDocument::IDottyPlotDocument(OSType fileType, TDocument parentDoc, 
										TSeqList		aAlnList; longint startbase, nbases)
		pascal void TDottyPlotDocument::Free(void) // override 
		pascal void TDottyPlotDocument::FreeData(void) // override 

		pascal void TDottyPlotDocument::DoMakeViews(Boolean forPrinting) // override 
		pascal void TDottyPlotDocument::UntitledName(Str255 VAR noName) // override 
		pascal TFile TDottyPlotDocument::DoMakeFile(CommandNumber itsCommandNumber) // override 
		pascal void TDottyPlotDocument::DoNeedDiskSpace(TFile itsFile,
															long VAR dataForkBytes, rsrcForkBytes) // override 
		pascal void TDottyPlotDocument::DoRead(aFile:TFile; Boolean forPrinting) // override 
		pascal void TDottyPlotDocument::DoWrite(TFile aFile, Boolean makingCopy) // override 
		}

	TDottyPlotHandler			== OBJECT (TStdPrintHandler)
		fAlnList		: TSeqList;  
		pascal void TDottyPlotHandler::IDottyPlotHandler(TDocument itsDocument, TView itsView,
											Boolean itsSquareDots, itsHFixedSize, itsVFixedSize)
		pascal VCoordinate TDottyPlotHandler::BreakFollowing(VHSelect vhs,  VCoordinate previousBreak,
								Boolean VAR automatic) // override 
		}
	
	
	TTreePrintDocument		== OBJECT; FORWARD;
								
	TTreePrintWind			== OBJECT (TPrefWindow)
		fStylePop		: TPopup;
		fRedrawBut	: TButton;
		TCluster		fGrowCluster, fNodeCluster;
		
		pascal void TTreePrintWind::ITreePrintWind(void)
		pascal void TTreePrintWind::SetPrefID(void) /* override */ 
		pascal void TTreePrintWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
		}
			
	TTreePrintView			== OBJECT (TView)
		fTreeData		: Handle;
		fParams			: Handle;
		fRoot				: Handle; //to tree node recs 
		
		Real		fRooty, fExpand, fLabelheight, fLabelrotation;
		fPlotNodeNum	: Boolean;
		
		fMinSize		: VPoint;
		fStyle			: integer;
		fStylePop		: TPopup;
		fHoriz			: TRadio;
		fFixed			: TCheckBox;
		fNodeLen		: TCheckBox;
		fNodeCluster	: TCluster;
		fRedrawBut	: TButton;
		
		TTreePrintDocument		fTreePrintDocument; 	
		TScroller		fScroller;			 

		pascal void TTreePrintView::ITreePrintView(TTreePrintDocument itsDocument,
										Boolean forClipboard)
		pascal void TTreePrintView::Free(void) // override 
		pascal void TTreePrintView::FreeData(void)

		pascal void TTreePrintView::Initialize(void) // override 
		pascal void TTreePrintView::DoPostCreate(TDocument itsDocument) // override 
				
		pascal void TTreePrintView::ForEachNodeDo( pascal void DoThis( aNode: UNIV Handle));
		pascal void TTreePrintView::CalcTree(void)

		pascal void TTreePrintView::CalcMinFrame(VRect VAR minFrame) // override 

		pascal void TTreePrintView::DoMenuCommand(CommandNumber aCommandNumber) // override 
		pascal void TTreePrintView::DoSetupMenus(void) // override 

		pascal void TTreePrintView::Draw(VRect area) // override 
		}
		
	TTreePrintDocument		== OBJECT (TFileBasedDocument)
		fTreeData		: Handle;
		fParams			: Handle;
		fTreePrintView	: TTreePrintView;
		fTreePrintWind	: TTreePrintWind;
		fParentDoc		: TDocument;
		
		pascal void TTreePrintDocument::ITreePrintDocument(OSType fileType, 
																		Handle		treeData, params; TFile itsFile)
		pascal void TTreePrintDocument::Free(void) // override 
		pascal void TTreePrintDocument::FreeData(void) // override 

		pascal void TTreePrintDocument::DoMakeViews(Boolean forPrinting) // override 
		pascal void TTreePrintDocument::UntitledName(Str255 VAR noName) // override 
		pascal TFile TTreePrintDocument::DoMakeFile(CommandNumber itsCommandNumber) // override 
		pascal void TTreePrintDocument::DoNeedDiskSpace(TFile itsFile,
															long VAR dataForkBytes, rsrcForkBytes) // override 
		pascal void TTreePrintDocument::DoRead(aFile:TFile; Boolean forPrinting) // override 
		pascal void TTreePrintDocument::DoWrite(TFile aFile, Boolean makingCopy) // override 
		}

	TTreePrintHandler			== OBJECT (TStdPrintHandler)
		pascal void TTreePrintHandler::ITreePrintHandler(TDocument itsDocument, TView itsView,
											Boolean itsSquareDots, itsHFixedSize, itsVFixedSize)
		pascal VCoordinate TTreePrintHandler::BreakFollowing(VHSelect vhs,  VCoordinate previousBreak,
								Boolean VAR automatic) // override 
		}
		


	TREMapDocument		== OBJECT; FORWARD; 

	TREMapWind			== OBJECT (TPrefWindow)/*! subclass TSeqPrintWind...*/ 
		TButton		fRedrawBut;
	
		pascal void TREMapWind::IREMapWind(void)
		pascal void TREMapWind::SetPrefID(void) /* override */ 
		pascal void TREMapWind::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
		}
		
	TREMapView			== OBJECT (TGridView)  /*! subclass TSeqPrintView...*/ 
		fSeq		: TSequence;  
		integer		fFirstBase, fNBases; //index each TSequence

		fLeftName, fRightName, 
		TCheckBox		fTopIndex, fLeftIndex, fRightIndex ;
		TDlogTextView		fStyleName, fStyleBase, fStyleNums; 
		TextStyle		fNameStyle, fNumStyle, fBaseStyle;

		fDoTopIndex, fDoLeftIndex, fDoRightIndex, 
		boolean		fUseColor, fDoLeftName, fDoRightName;
		Integer		fNameWidth, fIndexWidth, fBaseWidth;
		Integer		fBasesPerLine, fTenSpacer;
		
		TREMapDocument		fREMapDocument; 
		
			//! added for REMap...............
		fREMap		: TREMap;
		fCutList	:	RECutsHandle;
		TCheckBox		fNoncutters, fCutpoints, fAllzymes, fShowCoseq, fShowindex;
		fMincuts, fMaxcuts	: TNumberText;
		CharsHandle		fProt, fCoProt;
		fSeqFrame1, fSeqFrame2, fSeqFrame3,
		fCoFrame1, fCoFrame2, fCoFrame3	: TCheckBox;
		TRadio		fThreeBase;
		Integer		fSeqRowHeight;
		Integer		fLastZymeCut;

		Integer		fAllZymesStart, fAllZymesEnd;
		Integer		fNoCutsStart, fNoCutsEnd;
		Integer		fCutpointStart, fCutpointEnd;
		
		pascal void TREMapView::Free(void) // override */ /*!
		pascal void TREMapView::IREMapView(TREMapDocument itsDocument,
									Boolean forClipboard)
		pascal void TREMapView::Initialize(void) // override */ /*!
		pascal void TREMapView::DoPostCreate(TDocument itsDocument) // override 
		
		pascal void TREMapView::InstallControls(void)//!
		pascal void TREMapView::FindNameWidth(void)//!
		pascal void TREMapView::SetDrawOptions(void) //!
		pascal void TREMapView::AddSeqs(void) //! added for REMap...
		pascal void TREMapView::DrawZymeLine(integer startCellh, startBase, endBase, 
											VRect		aRect; Boolean doDraw, Integer VAR lineHeight)

		pascal void TREMapView::AllZymeTabRows( Integer VAR nRows, rowHeight)
		pascal void TREMapView::AllZymeTable( integer atRow, VRect aRect)
		pascal void TREMapView::NocuttersRows( Integer VAR nRows, rowHeight)
		pascal void TREMapView::NocuttersTab( integer atRow, VRect aRect)
		pascal void TREMapView::CutpointsRows( Integer VAR nRows, rowHeight)
		pascal void TREMapView::CutpointsTab( integer atRow, VRect aRect)
		
		pascal void TREMapView::DrawCell(GridCell aCell, VRect aRect) // override */ /*!
		pascal void TREMapView::DrawRangeOfCells(GridCell startCell, stopCell, VRect aRect) 
														// override 
		pascal void TREMapView::CalcMinFrame(VRect VAR minFrame) // override 
		
		pascal void TREMapView::DoMenuCommand(CommandNumber aCommandNumber) // override 
		pascal void TREMapView::DoSetupMenus(void) // override 
		}

	TREMapDocument		== OBJECT (TFileBasedDocument)/*! subclass TSeqPrintDocument...*/ 
		fREMapView	: TREMapView;
		fREMapWind	: TREMapWind;
		fParentDoc		: TDocument;
		fSeq					: TSequence;  
		longint		fFirstBase, fNBases; //index each TSequence
		fColorButton, fMonoButton	: TIcon;
		
		pascal void TREMapDocument::IREMapDocument(OSType fileType, TDocument parentDoc, 
										TSequence		aSeq; longint startbase, nbases)
		pascal void TREMapDocument::Free(void) // override 
		pascal void TREMapDocument::FreeData(void) // override 
		pascal void TREMapDocument::Close(void) /* override */ 

		pascal void TREMapDocument::DoMakeViews(Boolean forPrinting) // override 
		pascal void TREMapDocument::UntitledName(Str255 VAR noName) // override 
		pascal TFile TREMapDocument::DoMakeFile(CommandNumber itsCommandNumber) // override 
		pascal void TREMapDocument::DoNeedDiskSpace(TFile itsFile,
															long VAR dataForkBytes, rsrcForkBytes) // override 
		pascal void TREMapDocument::DoRead(aFile:TFile; Boolean forPrinting) // override 
		pascal void TREMapDocument::DoWrite(TFile aFile, Boolean makingCopy) // override 
		}



VAR
	RGBColor		gGray50, gGray75, gGray25; 
	RGBColor		gAcolor, gCcolor, gGcolor, gTcolor, gNcolor; 
	gPrintZymeStyle, gPrintNucStyle,gPrintNameStyle	: TextStyle;

		
IMPLEMENTATION

		//$I USeqPrint.seq.p
		//$I USeqPrint.aln.p
		//$I USeqPrint.dotty.p
		//$I USeqPrint.tree.p
		//$I USeqPrint.remap.p

}
