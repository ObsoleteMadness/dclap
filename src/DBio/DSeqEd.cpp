// DSeqEd.cpp -- one sequence edit view/window 
// d.g.gilbert, 1991-94 


#include "DSeqEd.h"
#include <ncbi.h>
#include <dgg.h>
#include <Dvibrant.h>
#include <DControl.h>
#include <DWindow.h>
#include <DTableView.h>
#include <DApplication.h>
#include <DTask.h>
#include <DUtil.h>
#include <DDialogText.h>
#include <DMenu.h>
#include <DFindDlog.h>

#include "DSeqFile.h"
#include "DSeqDoc.h"
#include "DSeqCmds.h"





Local Nlm_FonT	gPromptFont = Nlm_programFont;
//Local Nlm_FonT	gSeqFont = Nlm_programFont;

enum SeqedIDs {
	mReformatHit = 2101,
	ceNAM, cTEVW, ctBEG, ctEND, ctSEL, ctLEN,  ctCHK, ctTYP, cmCOD	
};







// class DSeqFindDialog

class DSeqFindDialog : public DFindDialog {
public:
	DSequence* fSequence;
	DSeqedView* fSeqedView;
	char	* fLastTarget;
	long		fLastMatch;
	DSeqFindDialog( DSequence* itsSeq, DSeqedView* itsView);
	virtual void DoFind();
	virtual void BuildDlog();
};

DSeqFindDialog::DSeqFindDialog( DSequence* itsSeq, DSeqedView* itsView) :
		fSequence(itsSeq), fSeqedView( itsView), 
		fLastTarget(NULL), fLastMatch(-1)
{
}

void DSeqFindDialog::BuildDlog()
{
	DFindDialog::BuildDlog();

	DView* av;
	av= FindSubview(replaceId); if (av) { av->Disable(); } //av->Hide();
	av= FindSubview(replaceFindId); if (av) { av->Disable();  }
	av= FindSubview(replaceAllId); if (av) { av->Disable(); }
	av= FindSubview(cBackwards); 	if (av) { av->Disable(); }
	av= FindSubview(cFullWord); if (av) { av->Disable(); }
	av= FindSubview(cCaseSense); if (av) { av->Disable(); }
	if (fReplaceText) fReplaceText->Disable();
}

void DSeqFindDialog::DoFind() 
{
	long	firstBase, nBases, match;
	short n1, n2;
	const char	*target;
	Boolean back;
	
	fSequence->UpdateFlds();  
	fSeqedView->GetSelection( n1, n2);
	firstBase= n1;
	nBases= 0; //n2 - n1;
	fSequence->SetSelection( firstBase, nBases);
	target= this->GetFind();
	back	= this->Backwards();
	if (target && *target) {
		if (fLastTarget && StringCmp(target, fLastTarget)==0 && firstBase == fLastMatch)
			match= fSequence->SearchAgain();
		else {
			if (fLastTarget) MemFree(fLastTarget);
			fLastTarget= StrDup(target);
			match= fSequence->Search( (char*)target, back);
			}
		fLastMatch= match;
		if (match>=0) {
			nBases= StrLen(target);
			fSequence->SetSelection( match, nBases);
			fSeqedView->SetSelection( match, match+nBases);
			}
		}
}				





// DSeqedView -----------------------------


DSeqedView::DSeqedView(long id, DView* itsSuperior, DSeqedWindow* itsDoc, DSequence* aSeq,
								 short width, short height) :
	DDialogScrollText( id, itsSuperior, width, height, gSeqFont, true) // true==wrap

{
  fOldStart= fOldEnd= fOldLen= -1;
	fOldKind= -1; 
	fOldCheck= -1;

	//fControlChars = fControlChars - [chTab];
	
	Nlm_RecT	 	arect; 
	Nlm_LoadRect(&arect,0,0,0,0);
	fOldDest= arect;
	fOldView= arect;
	fChanged= FALSE;

	// FailNIL( aSeq); 
	if (!aSeq) { delete this; return; }
	fSequence= aSeq;
	fDoc= itsDoc;

  fSeqStart = fDoc->fSeqStart;
  fSeqEnd = fDoc->fSeqEnd;
  fSeqSel = fDoc->fSeqSel;
  fSeqLen = fDoc->fSeqLen;
  fSeqCheck = fDoc->fSeqCheck;
  //fSeqTyp = fDoc->fSeqTyp;
  fCodePop = fDoc->fCodePop;
  fCodePop->SetValue( fSequence->Kind());

	//SetKeyHandler(this); //?? also need this call in a window activate/deactivate method??

	this->SetResize( DView::relsuper, DView::relsuper);
	this->SetText( fSequence->Bases());

	UpdateCtls(TRUE); //??
}


DSeqedView::~DSeqedView()
{
	if (fChanged && fDoc && fDoc->fMainDoc)	
		fDoc->fMainDoc->Dirty(); //((DSeqDoc*)fSuperior)->Dirty();
}


void DSeqedView::SetSeq(DSequence* aSeq) 
{
	fSequence= aSeq;
	this->SetText( fSequence->Bases());
	UpdateCtls(true);
	//this->Invalidate(); //??
}

void DSeqedView::UpdateCtls(Boolean forceIndex)  //call this when seq indices change
{	
 	char	newNum[256];
 	short	n1, n2;
	Boolean		newrange, didChange;
	Nlm_RecT		adest, aview, bview;
 	
 	
	newrange= FALSE;	
	didChange= FALSE;
	adest= fOldDest;
	aview= fOldView;
	this->ViewRect(bview);
	
#if 0
	if (!Nlm_EqualRect( &aview, &bview)) fSeqHindex->Invalidate();  
		
	if (forceIndex 
	 || !Nlm_EqualRect(&adest, (*fHTE)->destRect) 
	 || !Nlm_EqualRect(&aview, &bview)) {
		fSeqIndex->Invalidate();  
		fOldDest= (*fHTE)->destRect;
		fOldView= bview;
		}
#endif		 
		
	this->GetSelection( n1, n2);
	if (n1 != fOldStart) {
		sprintf(newNum, "%d", n1+1);
		fSeqStart->SetTitle(newNum);
		fOldStart= n1;
		newrange= TRUE;
		}
	
	if (n2 != fOldEnd) {
		short val;
		if (n2 > n1) val= n2; else val= n2+1;
		sprintf(newNum, "%d", val);
		fSeqEnd->SetTitle(newNum);
		fOldEnd= n2;
		newrange= TRUE;
		}

	if (newrange) {
		sprintf(newNum, "%d", n2 - n1);
		fSeqSel->SetTitle(newNum);
		}

	if (fSequence->Checksum() != fOldCheck) {
		sprintf(newNum, "%-8lx", fSequence->Checksum());
		fSeqCheck->SetTitle(newNum);
		fOldCheck= fSequence->Checksum();
		didChange= TRUE;
		}
		
	if (fSequence->Kind() != fOldKind) {
	  fCodePop->SetValue( fSequence->Kind()+1); 		
		//fSequence->SetKind(fCodePop->GetValue()-1); 
		fOldKind= fSequence->Kind();
		didChange= TRUE;
		}
		
	long len= this->TextLength();
	if (len != fOldLen) {
		sprintf(newNum, "%d", len);
		fSeqLen->SetTitle(newNum);
		//fSeqIndex->AdjustFrame(); //! change index size, ?do this only when nlines changes?
		fOldLen= len;
		didChange= TRUE;
		}
		
	if (didChange) this->fChanged= TRUE;
}



//-- Override some TTEView routines that can change selection && scroll
// SynchView is called only/just after some, but not all, RecalcTexts 
// main usage will be from Resize 

void DSeqedView::selectAction()
{
	DDialogScrollText::selectAction();
	gCursor->ibeam();
	SetKeyHandler(this); //?? also need this call in a window activate/deactivate method??
	this->UpdateCtls(TRUE);
}

void DSeqedView::deselectAction()
{
	SetKeyHandler(NULL); //?? also need this call in a window activate/deactivate method??
	fDoc->SetEditText(NULL);
	gCursor->arrow();
	DDialogScrollText::deselectAction();
}


/****
class DKeyCallback
{
public:
	DKeyCallback();
	virtual ~DKeyCallback();
	virtual void SetKeyHandler(DKeyCallback* handler);
  virtual void ProcessKey( char c) = 0;
};
*****/

void DSeqedView::ProcessKey( char c)
{
	//fSequence->SetKind(fCodePop->GetValue()-1); //?? why this on each char?? 
	if ( IsEnabled()) { 
			if (!fSequence->GoodChar(c)) {
				Nlm_Beep(); //gApplication->Beep(1);  
				// Flush further keystrokes ?
				}
		UpdateCtls(FALSE);
		}
}





Boolean DSeqedView::DoMenuTask(long tasknum, DTask* theTask)
{
	DWindow* win = NULL;
	
	switch (tasknum) {

		case DSeqDoc::cFindORF:
			{
				long	start, stop;
				short n1, n2;
				
				fSequence->UpdateFlds();  
				this->GetSelection( n1, n2);
				if (n2 > n1+1) n1++; // !? offset sel start so we can repeat call
				fSequence->SetSelection( n1, 0);
				fSequence->SearchORF( start, stop);
				if (start>=0) {
					if (stop<start) stop= fSequence->LengthF(); //??
					fSequence->SetSelection( start, stop - start);
					this->SetSelection( start, stop);
					}
			}				
			return true;

		case DSeqDoc::cRevSeq:
		case DSeqDoc::cCompSeq:
		case DSeqDoc::cRevCompSeq:
		case DSeqDoc::cDna2Rna: 
		case DSeqDoc::cRna2Dna:
		case DSeqDoc::cToUpper: 
		case DSeqDoc::cToLower:
		case DSeqDoc::cDegap:
		case DSeqDoc::cLockIndels:
		case DSeqDoc::cUnlockIndels:
		//case DSeqDoc::cConsensus:
		case DSeqDoc::cTranslate:
			{
			long	firstBase, nBases;
			short n1, n2;
			DSeqList * aSeqList= NULL;
			DSeqChangeCmd *	cmd = NULL;  
			DSeqDoc* maindoc= this->fDoc->fMainDoc;
			
			//GetSelection( TRUE, TRUE, aSeqList, firstBase, nBases);
			fSequence->UpdateFlds();  
			this->GetSelection( n1, n2);
			firstBase= n1;
			nBases= n2 - n1;
			fSequence->SetSelection( firstBase, nBases);
			aSeqList= new DSeqList();
			aSeqList->InsertLast(fSequence);
			
			switch (tasknum) {
		  	case DSeqDoc::cRevSeq			: cmd= new DSeqReverseCmd( maindoc, this, aSeqList);  break;
				case DSeqDoc::cCompSeq		: cmd= new DSeqComplementCmd(  maindoc, this, aSeqList); break;
				case DSeqDoc::cRevCompSeq : cmd= new DSeqRevComplCmd(  maindoc, this, aSeqList); break;
				case DSeqDoc::cDna2Rna		: cmd= new DSeqDna2RnaCmd(  maindoc, this, aSeqList); break;
				case DSeqDoc::cRna2Dna		:	cmd= new DSeqRna2DnaCmd(  maindoc, this, aSeqList); break;
				case DSeqDoc::cToUpper		:	cmd= new DSeqUppercaseCmd(  maindoc, this, aSeqList); break;
				case DSeqDoc::cToLower		:	cmd= new DSeqLowercaseCmd(  maindoc, this, aSeqList); break;
				case DSeqDoc::cDegap			:	cmd= new DSeqCompressCmd(  maindoc, this, aSeqList); break;
				case DSeqDoc::cLockIndels	:	cmd= new DSeqLockIndelsCmd( maindoc, this, aSeqList); break;
				case DSeqDoc::cUnlockIndels: cmd= new DSeqUnlockIndelsCmd( maindoc, this, aSeqList); break;
				case DSeqDoc::cTranslate	:	cmd= new DSeqTranslateCmd( maindoc, this, aSeqList); break;
				
				default:
					delete aSeqList;
					Message(MSG_OK,"DSeqedView::method not ready.");
					return true;
				
		  	}
		  if (cmd) {
		  	if (cmd->Initialize()) PostTask( cmd);
		  	else {
		  		delete cmd;
		  		Message(MSG_OK,"DSeqedView::method failed.");
					}
		  	}
			return true;	
			}
		
		case DSeqDoc::cPrettyPrint:  //MakeAlnPrint();
		case DSeqDoc::cREMap:				//MakeSeqPrint( TRUE); 
		case DSeqDoc::cDotPlot:			//MakeDottyPlot();
			Message(MSG_OK,"DSeqedView::method not ready.");
			return true;

		default: 
			return DTaskMaster::DoMenuTask(tasknum, theTask);
		}

}


Boolean DSeqedView::IsMyAction(DTaskMaster* action) 
{
	long  menuid= 0;
	if (action->fSuperior)  menuid = action->fSuperior->Id();
	switch(action->Id()) {
		case DApplication::kCut:
		case DApplication::kCopy:
		case DApplication::kPaste:
		case DApplication::kClear:
		case DApplication::kSelectAll:
			return DDialogText::IsMyAction(action);
		}
		
#if 0			
 if (action->Id() == kColorButHit) {
		fUseColor= ((DView*)action)->GetStatus();
		fAlnView->Invalidate();
  	return true;
  	}
	else 
#endif
		return DoMenuTask(action->Id(), NULL);
}
			



#if FIX_LATER
pascal void TSeqedView::DoMenuCommand(aCommandNumber:CommandNumber); // override 
VAR
		TSeqTranslateCmd		aSeqTranslateCmd;
		TSeqReverseCmd		aSeqReverseCmd;
		TSeqRevComplCmd		aSeqRevComplCmd;
		TSeqComplementCmd		aSeqComplementCmd;
		TSeqCompressCmd		aSeqCompressCmd;
		TSeqDna2RnaCmd		aSeqDna2RnaCmd;
		TSeqRna2DnaCmd		aSeqRna2DnaCmd;
		TSeqLockIndelsCmd		aSeqLockIndelsCmd;
		TSeqUnlockIndelsCmd		aSeqUnlockIndelsCmd;
	
	pascal void MakeSeqPrint(Boolean doREMap)
	VAR
			TREMapDocument		aREMapDoc;
			aSeqPrDoc	: TSeqPrintDocument;
			longint		firstBase, nBases;
	{
		fSequence->UpdateFlds(); //!? need for New seq...
		firstBase= this.(*fHTE)->selStart; 
		nBases= this.(*fHTE)->selEnd - firstBase;  
		if (doREMap) {
			New(aREMapDoc);
			FailNIL(aREMapDoc);
			aREMapDoc->IREMapDocument(kPrintClipType, fDocument, fSequence, firstBase, nbases);
			gSeqAppApplication->OpenNewDocument(aREMapDoc);  
			}		else {
			New(aSeqPrDoc);
			FailNIL(aSeqPrDoc);
			aSeqPrDoc->ISeqPrintDocument(kPrintClipType, fDocument, fSequence, firstBase, nbases);
			gSeqAppApplication->OpenNewDocument(aSeqPrDoc);  
			}
	}
	
	pascal void GetSeqList( TSeqList VAR aSeqList)
	longint		VAR firstBase, nBases;
	{
		fSequence->UpdateFlds(); //!? need for New seq...
		firstBase= (*fHTE)->selStart;
		nBases= (*fHTE)->selEnd - firstBase;
		fSequence->SetSelection( firstBase, nBases);
		aSeqList= TSeqList(NewList); 
		FailNIL( aSeqList);
		aSeqList->InsertLast(fSequence);
	}

	pascal void	DoSeqChangeCommand( TSeqChangeCmd aCommand)
	VAR		aSeqList: TSeqList; 
	{
		FailNIL(aCommand);
		GetSeqList( aSeqList);
		aCommand->ISeqChangeCmd( TAlnDoc(fDocument), this, aSeqList); 
		PostCommand( aCommand);
	}

{
	switch (aCommandNumber) {
		cTranslateSeq: {
				New(aSeqTranslateCmd);
				DoSeqChangeCommand(aSeqTranslateCmd);
				}
		cReverseSeq: {
				New(aSeqReverseCmd);
				DoSeqChangeCommand(aSeqReverseCmd);
				}
		cComplementSeq: {
				New(aSeqComplementCmd);
				DoSeqChangeCommand(aSeqComplementCmd);
				}
		cRevComplement: {
				New(aSeqRevComplCmd);
				DoSeqChangeCommand(aSeqRevComplCmd);
				}
		cCompress: {
				New(aSeqCompressCmd);
				DoSeqChangeCommand(aSeqCompressCmd);
				}
		cLockIndels: {
				New(aSeqLockIndelsCmd);
				DoSeqChangeCommand(aSeqLockIndelsCmd);
				}
		cUnlockIndels: {
				New(aSeqUnlockIndelsCmd);
				DoSeqChangeCommand(aSeqUnlockIndelsCmd);
				}
		cDna2Rna: {
				New(aSeqDna2RnaCmd);
				DoSeqChangeCommand(aSeqDna2RnaCmd);
				}
		cRna2Dna: {
				New(aSeqRna2DnaCmd);
				DoSeqChangeCommand(aSeqRna2DnaCmd);
				}
					
		cPrettyPrint: MakeSeqPrint( FALSE);
		cRestrictMap: MakeSeqPrint( TRUE);
		  
		default:
			inherited::DoMenuCommand(aCommandNumber);
		}

	UpdateCtls(FALSE);
}

pascal void TSeqedView::DoSetupMenus(void) // override 
VAR  haveSel: boolean;
{
  inherited::DoSetupMenus();	
	
	//haveSel= ((*fHTE)->selEnd - (*fHTE)->selStart) > 0; 
	haveSel= TRUE; 

	Enable( cReverseSeq, haveSel); 
	Enable( cComplementSeq, haveSel); 
	Enable( cRevComplement, haveSel); 
	Enable( cCompress, haveSel); 
	Enable( cLockIndels, haveSel); 
	Enable( cUnlockIndels, haveSel); 
	Enable( cDna2Rna, haveSel); 
	Enable( cRna2Dna, haveSel); 
	Enable( cTranslateSeq, haveSel); 
	
	Enable( cPrettyPrint, TRUE); 
	Enable( cRestrictMap, TRUE); 
}


pascal void TSeqedView::DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event,
											   Point hysteresis) // override 
{
	inherited::DoMouseCommand(theMouse,event,hysteresis);
	UpdateCtls(FALSE);
}


 //keep cursor at arrow when teview is not selected (inactive "edittext")
pascal void TSeqedView::DoSetCursor(VPoint localPoint, RgnHandle cursorRegion)	// override 
{
	if GetWindow->fTarget == this then 
		inherited::DoSetCursor(localPoint, cursorRegion);
}


pascal TSeqedView::HandleMouseDown( VPoint theMouse, TToolboxEvent event, 
							 hysteresis: Point):Boolean; // override 
//TView method -- note this is Generic for any TEView...
{		
	if (GetWindow->fTarget != this) { 
		/*---
		TDialogView		VAR aDlog; 
		aDlog = TDialogView(GetDialogView);   
		if ((aDlog!=NULL)) if (!aDlog->DeselectCurrentEditText()) ;
		---*/
		/*---
		 Else if fTarget == otherTEView)
			otherTEView->InstallSelection(TRUE,FALSE);
		-----*/
		// gApplication->SetTarget(this);
		HandleMouseDown= BecomeTarget;   //! turn on our TEview event handler
		}	else
  	HandleMouseDown= inherited::HandleMouseDown(theMouse,event,hysteresis);
}


#endif // FIX_LATER




// TSeqIndex ----------------------------------


#if FIX_LATER

pascal void TSeqIndex::Initialize(void) // override 
{
	inherited::Initialize();
	fTEView	= NULL;
}

pascal void TSeqIndex::CalcMinFrame(VRect VAR minFrame) // override 
// THIS WAS bombing us on initial call to CreateViews.. until we IRes'd
VAR
	VRect		superExtent;
{
  inherited::CalcMinFrame(minFrame);
	if (fSuperView!=NULL) {
		fSuperView->GetExtent(superExtent);
		minFrame.bottom= max(minFrame.bottom, superExtent.bottom);
		}
	if ((fTEView!=NULL) && (fTEView->fHTE!=NULL))
		minFrame.bottom= max(minFrame.bottom, 10+TEGetHeight(0,fTEView.(*fHTE)->nlines,fTEView->fHTE));
}

pascal void TSeqIndex::Draw(VRect area) // override 
VAR
	rect		vrect ;
	integer		cat, il, h, v, v1, nl, vend, ih, ws ;
	nums	: Str255;
	ptop	: point;
{
	if ((fTEView!=NULL)) {
		vrect= fTEView.(*fHTE)->viewrect;
		h		= area.right-3;
		v		= vrect.top;
		vend= area.bottom;
		
		cat = TEGetOffset(vrect.topleft,fTEView->fHTE);
		il  = 0;
		nl= fTEView.(*fHTE)->nlines;
		while ( (il < nl) && (cat > fTEView.(*fHTE)->lineStarts[il]) do il= il+1;
		while ( (v < vend) && (il < nl)){
			v1= v + TEGetHeight( il, il, fTEView->fHTE);
			if ((v1 >= area.top) && (v <= vend)) {
				cat= fTEView.(*fHTE)->lineStarts[il];
				NumToString( cat, nums);
				moveto( h-StringWidth(nums), v1);
				drawString(nums); 
				}
			v= v1;  
			il= il + 1;
			}
		}
	inherited::Draw(area); //!?
}
 
#endif //FIX_LATER




// TSeqHIndex ----------------------------

#if FIX_LATER

pascal void TSeqHIndex::Draw(VRect area) // override 
var
	rect		vrect ;
	integer		cat, ecat, il, h, v, vend, ws ;
	nums	: Str255;
	pt		: point;
{
	if ((fTEView!=NULL)) {
		vrect= fTEView.(*fHTE)->viewrect;
		v		= area.bottom;
		vend= area.right;
		cat = TEGetOffset(vrect.topleft,fTEView->fHTE); //!? is pt rel to view or dest ?
	
		WITH fTEView.(*fHTE)^){
			il  = 0;
			while ( (il < nlines) && (cat >= lineStarts[il]))il= il+1;
			ecat= lineStarts[il];
			if (il>0) cat= lineStarts[il-1];
			}
		
		il= 5; 
		cat= cat + 4; //? skip 1
		do {
			pt= TEGetPoint(cat, fTEView->fHTE);
			moveto(pt.h, v);
	
			if ((il mod 10 == 0)) {
				line(0,-3);
				NumToString( il, nums);
				ws= StringWidth(nums);
				move(-ws div 2,-1);
				drawstring(nums);
				}			else 
				line(0,-8);
			cat= cat + 5;
			il= il+5;
		} while (!((pt)).h >= area.right) || (cat >= ecat);
		}
	inherited::Draw(area);
}


#endif //FIX_LATER







// DSeqedWindow ------------------------



	
DSeqedWindow::DSeqedWindow(long id, DTaskMaster* itsSuperior, DSeqDoc* itsSeqDoc, DSequence* aSeq,
									short winwidth, short winheight, short winleft, short wintop, char* title) :
	DWindow(id, itsSuperior, DWindow::document, winwidth, winheight, winleft, wintop, title)
{	
	DView* super = this;
	DPrompt* pr;
	DCluster* clu;
	
		// fix this later...
	gPromptFont= Nlm_programFont;
	//gSeqFont 	= Nlm_programFont;

	fSequence= aSeq; // ?? Clone it for safety ??
	fMainDoc= itsSeqDoc;
	
  clu= new DCluster(0,super,0,0,true,NULL);
  super= clu;
  pr = new DPrompt(0, super, "Name:", 0, 0, gPromptFont, justleft);
  super->NextSubviewToRight();
  fSeqName = new DEditText( ceNAM, super, aSeq->Name(), 20);  
  

	super= this;
	super->NextSubviewBelowLeft();
	
  clu= new DCluster(0,super,0,0,false,"Sequence");
	super= clu;
		
  pr = new DPrompt(0, super, " length:", 0, 0, gPromptFont, justleft);
  super->NextSubviewToRight();
 	fSeqLen = new DPrompt(ctLEN, super, "00000", 0, 0, gPromptFont, justright);
  super->NextSubviewToRight();
  
  pr = new DPrompt(0, super, " checksum:", 0, 0, gPromptFont, justleft);
  super->NextSubviewToRight();
  fSeqCheck = new DPrompt(ctCHK, super, "0000000", 0, 0, gPromptFont, justright);
  super->NextSubviewToRight();
  
  pr = new DPrompt(0, super, " type:", 0, 0, gPromptFont, justleft);
  super->NextSubviewToRight();
  //fSeqTyp   = new DPrompt(ctTYP, super, "Unknown type", 0, 0, gPromptFont, justleft);
  //super->NextSubviewToRight();
  fCodePop  = new DPopupList( cmCOD, super, true);
  fCodePop->AddItem("Unknown type");
  fCodePop->AddItem("DNA");
  fCodePop->AddItem("RNA");
  fCodePop->AddItem("Nucleic");
  fCodePop->AddItem("Protein");
  fCodePop->SetValue( fSequence->Kind()+1); // ?? damn this doesn't work 
	//fSequence->SetKind(fCodePop->GetValue()-1); 


	super= this;
	super->NextSubviewBelowLeft();
	
  clu= new DCluster(0,super,0,0,false,"Selection");
  super= clu;
  pr = new DPrompt(0, super, "start:", 0, 0, gPromptFont, justleft);
  super->NextSubviewToRight();
  fSeqStart = new DPrompt(ctBEG, super, "00000", 0, 0, gPromptFont, justright);
  super->NextSubviewToRight();

  pr = new DPrompt(0, super, " end:", 0, 0, gPromptFont, justleft);
  super->NextSubviewToRight();
  fSeqEnd   = new DPrompt(ctEND, super, "00000", 0, 0, gPromptFont, justright);
  super->NextSubviewToRight();

  pr = new DPrompt(0, super, " size:", 0, 0, gPromptFont, justleft);
  super->NextSubviewToRight();
  fSeqSel   = new DPrompt(ctSEL, super, "00000", 0, 0, gPromptFont, justright);
  	
 
	super= this;
	super->NextSubviewBelowLeft();
 	
	fSeqedView= new DSeqedView(cTEVW, super, this, fSequence, 30, 8); // sizes are in SysFontSize...
	fSeqedView->Enable(); //??
	this->SetEditText(fSeqedView);

	fFindDlog= new DSeqFindDialog( fSequence, fSeqedView);
}

DSeqedWindow::~DSeqedWindow()
{
	// ?? do we need to kill the view components, or does window free do that??
}


// static
void DSeqedWindow::UpdateEdWinds( DSequence* oldSeq, DSequence* newSeq)
{
	DList* wins= gWindowManager->GetWindowList();
	long i, n= wins->GetSize();
	for (i=0; i<n; i++) {
		DWindow* win= (DWindow*) wins->At(i);
		if (win->Id() == DSeqedWindow::kId) {
			if (((DSeqedWindow*)win)->fSequence == oldSeq) {
				((DSeqedWindow*)win)->ReplaceSeq(newSeq);
				win->Invalidate();
				}
			}
		}
}

				
#if MAYBE_FIX_LATER
pascal void TSeqedWindow::DoEvent(EventNumber eventNumber, 
											TEventHandler		source; TEvent event) // override 
{
	if (eventNumber == mReformatHit) {
		fSequence->Reformat(1);
		fSeqedView->ShowReverted(); //? calls recalc + display?
		}	else
		inherited::DoEvent( eventNumber, source, event);
}
#endif		
 

Nlm_Boolean DSeqedWindow::IsMyAction(DTaskMaster* action) 
{	
	long  menuid= 0;
	if (action->fSuperior) menuid = action->fSuperior->Id();
	if (fSeqedView->IsMyAction(action)) 
		return true;
	else if (menuid == DApplication::cEditMenu && HasEditText()) {
		if (fEditText->IsMyAction(action)) ;
		return true;
		}		
	else 
		return DWindow::IsMyAction(action);
}


void DSeqedWindow::UpdateEdits() 
{
	char	name[128];
	fSeqName->GetTitle( name, 128);
	fSequence->SetName( name);
	
	char* bases= fSeqedView->GetText();
	if (bases) fSequence->SetBases(bases);
	MemFree(bases);
	
	fSequence->UpdateFlds();
	
		//! Need better way than to ForceRedraw of single seq...
	if (fMainDoc) {
		fMainDoc->fAlnView->UpdateWidth(fSequence);
		fMainDoc->fAlnView->Invalidate(); 
		fMainDoc->fAlnIndex->Invalidate();
		}
}


void DSeqedWindow::Close()  
{
	UpdateEdits();
  DWindow::Close();
}


void DSeqedWindow::ReplaceSeq(DSequence* aSeq)
{	
	fSequence= aSeq;
	fSeqedView->SetSeq( aSeq);
}
















//class DSeqDocPrefs : public DWindow 

class DSeqDocPrefs : public DWindow {
public:
	enum { 
			kSeqDocPrefID = 1492,
			kStylePlain,kStyleItalic,kStyleBold,kStyleUnderline,
			cWriteMasks, cStartDoc, cColored,
			cDistCor, cMatKind
			};
	static char *gSeqFontName;
	static short gSeqFontSize;
	static short gSeqStyle;
	static Boolean gColored;
	static void InitGlobals();
	static void SaveGlobals();

	DPopupMenu  * fSeqFontMenu, 
							* fSeqStyleMenu;
	DSwitchBox	* fSeqSizeSw, * fCommonPctSw;
	Boolean	 		fNeedSave;
	short				fOldStyle;
	DCluster	* fDistCor, * fMatKind;
	
	DSeqDocPrefs();
	virtual ~DSeqDocPrefs();
	virtual void Initialize();
	virtual void Open();
	virtual void Close();
	virtual void OkayAction();
	virtual Boolean IsMyAction(DTaskMaster* action); 
	virtual void NewFontCluster(char* title, DView* mainview,
					DPopupMenu*& mfont, DPopupMenu*& mstyle, DSwitchBox*& swsize,
					char* fontname, short fontstyle, short fontsize );
};

Global DSeqDocPrefs* gSeqDocPrefs = NULL;
Global Nlm_FonT gSeqFont = NULL;
 
char* DSeqDocPrefs::gSeqFontName = (char*) "times";
short 
			DSeqDocPrefs::gSeqFontSize = 10, 
			DSeqDocPrefs::gSeqStyle = 0; 
Boolean 	
			DSeqDocPrefs::gColored = true;

enum FonStyles { 
	kPlain = 0, kItalic = 1, kBold = 2, kUnderline = 4 
	};

 
DSeqDocPrefs::DSeqDocPrefs() :
	DWindow( 0, NULL, DWindow::fixed, -10, -10, -50, -20, "SeqDoc prefs", kDontFreeOnClose),
	fSeqFontMenu(NULL), fSeqStyleMenu(NULL), fSeqSizeSw(NULL), fCommonPctSw(NULL),
	fDistCor(NULL), fMatKind(NULL),
	fOldStyle(-1), fNeedSave(false)
{	
}

DSeqDocPrefs::~DSeqDocPrefs()
{
}


inline Nlm_FonT GetAFont( char* fname, short fsize, short fstyle)
{
	if (StrICmp(fname,"System")==0) return Nlm_systemFont;
	else if (StrICmp(fname,"Program")==0) return Nlm_programFont;
	else return Nlm_GetFont( fname, fsize, 
	         fstyle & kBold, fstyle & kItalic, fstyle & kUnderline, NULL);
}

// static
void DSeqDocPrefs::InitGlobals() 
{ 
	gSeqFontName= gApplication->GetPref( "gSeqFontName", "fonts", gSeqFontName);
	gSeqFontSize= gApplication->GetPrefVal( "gSeqFontSize", "fonts", "10");
	gSeqStyle= gApplication->GetPrefVal( "gSeqStyle", "fonts", "0");
	gSeqFont= ::GetAFont(gSeqFontName,gSeqFontSize, gSeqStyle);

	DSeqFile::gWriteMasks= gApplication->GetPrefVal( "gWriteMasks", "seqdoc", "1");
	DSeqDoc::fgStartDoc	= gApplication->GetPrefVal( "fgStartDoc", "seqdoc", "1");
	DSeqList::gMinCommonPercent= gApplication->GetPrefVal( "gMinCommonPercent", "seqdoc", "70");

	DSeqDoc::fgDistCor	= gApplication->GetPrefVal( "fgDistCor", "seqdoc", "0");
	DSeqDoc::fgMatKind	= gApplication->GetPrefVal( "fgMatKind", "seqdoc", "0");

	//gColored= gApplication->GetPrefVal( "gColored", "seqdoc","1");
#if 0
	{
	char* srect = gApplication->GetPref( "gSeqPrintDocRect", "windows", "20 20 450 220");
		// sscanf is failing on Mac/codewar !? used to work
	if (srect) {
		char* cp= srect;
		while (*cp && isspace(*cp)) cp++;
		gSeqPrintDocRect.left= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		gSeqPrintDocRect.top= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		gSeqPrintDocRect.right= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		gSeqPrintDocRect.bottom= atoi( cp);
		}
	MemFree(srect);
	}	
#endif

}


//static
void DSeqDocPrefs::SaveGlobals() 
{
	gApplication->SetPref( gSeqFontName, "gSeqFontName", "fonts");
	gApplication->SetPref( gSeqFontSize, "gSeqFontSize", "fonts");
	gApplication->SetPref( gSeqStyle, "gSeqStyle", "fonts");
	
	//gApplication->SetPref( gColored, "gColored", "seqdoc");
	gApplication->SetPref(  DSeqFile::gWriteMasks, "gWriteMasks", "seqdoc");
	gApplication->SetPref(  DSeqDoc::fgStartDoc, "fgStartDoc", "seqdoc");
	gApplication->SetPref(  DSeqDoc::fgDistCor, "fgDistCor", "seqdoc");
	gApplication->SetPref(  DSeqDoc::fgMatKind, "fgMatKind", "seqdoc");
	gApplication->SetPref(  DSeqList::gMinCommonPercent, "gMinCommonPercent", "seqdoc");

}




void DSeqDocPrefs::NewFontCluster(char* title, DView* mainview,
					DPopupMenu*& mfont, DPopupMenu*& mstyle, DSwitchBox*& swsize,
					char* fontname, short fontstyle, short fontsize )
{	
		// Name style -- font, font size, font style
	DPopupMenu* popm;
	DSwitchBox* sw;
	DPrompt* pr;
	DView* super;
	
	super= mainview;
	super->NextSubviewBelowLeft();

	DCluster* maincluster= new DCluster( 0, super, 50, 10, true, NULL); //false, title);
	super= maincluster;

	popm= new DPopupMenu( 0, (Nlm_GrouP)super->GetNlmObject(), "Font  ");
	mfont= popm;
	popm->AddFonts();
	popm->SetFontChoice( fontname); 
	//super->NextSubviewToRight();

#if 0
	DPopupList* popl;
	popl= new DPopupList( 0,super,true);
	mstyle= (DPopupMenu*) popl;
	popl->AddItem("Plain");
	popl->AddItem("Italic");
	popl->AddItem("Bold");
	popl->AddItem( "Underline");
#else
	popm= new DPopupMenu( 0, (Nlm_GrouP)super->GetNlmObject(), "Style  ");
	mstyle= popm;
	popm->AddItem( kStylePlain, "Plain", true);
	popm->AddItem( kStyleItalic, "Italic", true);
	popm->AddItem( kStyleBold, "Bold", true);
	popm->AddItem( kStyleUnderline, "Underline", true);
	popm->SetItemStatus( kStylePlain, fontstyle==0);
	popm->SetItemStatus( kStyleItalic, fontstyle & kItalic);
	popm->SetItemStatus( kStyleBold, fontstyle & kBold);
	popm->SetItemStatus( kStyleUnderline, fontstyle & kUnderline);
#endif
	//super->NextSubviewToRight();
	
	DCluster* cluster= new DCluster(0, super, 30, 10, true, NULL);
	super= cluster;
	
	pr= new DPrompt( 0, super, "font size", 0, 0, Nlm_programFont);	 		
	//super->NextSubviewToRight();
	super->NextSubviewBelowLeft();
	sw = new DSwitchBox(0, super, true, true);
	swsize= sw;
	sw->SetValues(fontsize,99);
	//super->NextSubviewBelowLeft();

	super= mainview;
	super->NextSubviewBelowLeft();
}


void DSeqDocPrefs::Initialize() 
{ 
	DView* super;
	DCluster* clu;
	DCheckBox* ck;
	DRadioButton* rad;

	super= this;
	clu= new DCluster( 0, super, 80, 10, false, "Sequence");
	super= clu;

	NewFontCluster( "Seq font style", super, fSeqFontMenu, fSeqStyleMenu, fSeqSizeSw,
							gSeqFontName, gSeqStyle, gSeqFontSize);
  fOldStyle= gSeqStyle;

	super= this;
	ck= new DCheckBox(cWriteMasks, super, "Save masks in file");
	ck->SetStatus(DSeqFile::gWriteMasks);
	super->NextSubviewBelowLeft();

	// check box for auto new document on startup ??
	ck= new DCheckBox(cStartDoc, super, "New align document on startup");
	ck->SetStatus(DSeqDoc::fgStartDoc);
	super->NextSubviewBelowLeft();

	new DPrompt( 0, super, "% common bases for consensus", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
	fCommonPctSw = new DSwitchBox(0, super, true, true);
	fCommonPctSw->SetValues(DSeqList::gMinCommonPercent,100);
	super->NextSubviewBelowLeft();
	
	//ck= new DCheckBox(cColored, super, "Colored bases");
	//ck->SetStatus(gColored);

	super = this;
	fMatKind= new DCluster(cMatKind,super,0,0,false,"Comparison matrix");
	super= fMatKind;
	(void) new DRadioButton(0,super,"Distance");
	super->NextSubviewToRight();
	(void) new DRadioButton(0,super,"Similarity");
	fMatKind->SetValue(DSeqDoc::fgMatKind+1); 
	super = this;
	
	fDistCor= new DCluster(cDistCor,super,0,0,false,"Comparison correction");
	super= fDistCor;
	(void) new DRadioButton(0,super,"none");
	super->NextSubviewToRight();
	(void) new DRadioButton(0,super,"Jukes");
	super->NextSubviewToRight();
	(void) new DRadioButton(0,super,"Olsen");
	fDistCor->SetValue(DSeqDoc::fgDistCor+1); 
	
	super = this;
	
	//super->NextSubviewToRight();
	//super->NextSubviewBelowLeft();

	this->AddOkayCancelButtons();
}


void DSeqDocPrefs::OkayAction() 
{ 
	short	 		aSize, aStyle, aCommon;
	char			name[256];
	DMenu 	*	aFontMenu, * aStyleMenu;
	Nlm_FonT	aFont;
	Boolean   newstyle;
	
	for (short imenu= 0; imenu<1; imenu++) {
		
		switch (imenu) {
		  case 0:
				aFontMenu= fSeqFontMenu; 
				aStyleMenu= fSeqStyleMenu; 
				aSize= fSeqSizeSw->GetValue();// aSize= gNameFontSize;
				 
				aCommon= fCommonPctSw->GetValue();
				if (aCommon != DSeqList::gMinCommonPercent) {				
					DSeqList::gMinCommonPercent= aCommon;
					fNeedSave= true;
					}
		  	break;
		  }
			
		if (aFontMenu && aFontMenu->GetFontChoice(name, sizeof(name))) {
		
			aStyle= 0;
#ifdef WIN_MAC
			if (!aStyleMenu->GetItemStatus(kStylePlain)) {
				if (aStyleMenu->GetItemStatus(kStyleItalic   )) aStyle |= kItalic;
				if (aStyleMenu->GetItemStatus(kStyleBold     )) aStyle |= kBold;
				if (aStyleMenu->GetItemStatus(kStyleUnderline)) aStyle |= kUnderline;
				}
#else
			short item= Nlm_GetValue(((DPopupMenu*)aStyleMenu)->fPopup);
			switch (item) {
				case 1: aStyle= 0; break;
				case 2: aStyle |= kItalic; break;
				case 3: aStyle |= kBold; break;
				case 4: aStyle |= kUnderline; break;
        }
#endif
 			newstyle= aStyle != gSeqStyle;
 			
			if (StringCmp(name,"System")==0) aFont= Nlm_systemFont;
			else if (StringCmp(name,"Program")==0) aFont= Nlm_programFont;
			else aFont= Nlm_GetFont( name, aSize, aStyle & kBold, aStyle & kItalic, 
												 			aStyle & kUnderline, NULL);
			switch (imenu) {
			  case 0: 
			  if (newstyle || aSize != gSeqFontSize || StringCmp(name,gSeqFontName)!=0) {
					if (gSeqFontName) MemFree(gSeqFontName);
					gSeqFontName= StrDup(name);
					gSeqFont= aFont;
					gSeqFontSize= aSize;
					gSeqStyle= aStyle;
					fNeedSave= true;
					}
			  	break;
			  }
			  
			}
		}
}



Boolean DSeqDocPrefs::IsMyAction(DTaskMaster* action) 
{	
	DView* aview= (DView*) action;
	short result;
	
	switch (action->Id()) {
			// ?? handle stylemenu popup -- if plain selected, deselect others...
		//case kSeqStyleMenu: 	break;
		//case cColored	: gColored= aview->GetStatus(); break;
		case cWriteMasks: DSeqFile::gWriteMasks= aview->GetStatus(); break;
		case cStartDoc  : DSeqDoc::fgStartDoc= aview->GetStatus(); break;
		case cMatKind		: DSeqDoc::fgMatKind= fMatKind->GetValue()-1; break;
		case cDistCor		: DSeqDoc::fgDistCor= fDistCor->GetValue()-1; break;
		default : return DWindow::IsMyAction(action);	
		}
		
	fNeedSave= true;
	return true;
}


void DSeqDocPrefs::Open()
{
	DWindow::Open();
}

void DSeqDocPrefs::Close()
{
	if (fNeedSave) {
		DSeqDocPrefs::SaveGlobals();
		fNeedSave= false;
		}
	DWindow::Close();
}



	// global calling function
void SeqDocPrefs(short id)
{
	switch (id) {
		case kSeqDocPrefInit: 
			DSeqDocPrefs::InitGlobals(); 
			break;
	
		case kSeqDocPrefDialog:
			if (!gSeqDocPrefs) {
				gSeqDocPrefs = new DSeqDocPrefs();
				gSeqDocPrefs->Initialize();
				}
			if (gSeqDocPrefs && gSeqDocPrefs->PoseModally()) ;
			break;
	}
	
}



#if NETBLAST

///class DBlastDialog : public DWindow 

class DBlastDialog : public DWindow {
public:
	enum { 
			kBlastDialogID = 29852,
			kStylePlain,kStyleItalic,kStyleBold,kStyleUnderline,
			cWriteMasks, cStartDoc, cColored
			};
	static char *gSeqFontName;
	static short gSeqFontSize;
	static short gSeqStyle;
	static Boolean gColored;
	static void InitGlobals();
	static void SaveGlobals();

	DPopupMenu  * fBlastDataMenu, 
							* fBlastProgMenu;
	DSwitchBox	* fSeqSizeSw, * fCommonPctSw;
	Boolean	 		fNeedSave;
	short				fOldStyle;
	
	DBlastDialog();
	virtual ~DBlastDialog();
	virtual void Initialize();
	virtual void Open();
	virtual void Close();
	virtual void OkayAction();
	virtual Boolean IsMyAction(DTaskMaster* action); 
	virtual void NewFontCluster(char* title, DView* mainview,
					DPopupMenu*& mfont, DPopupMenu*& mstyle, DSwitchBox*& swsize,
					char* fontname, short fontstyle, short fontsize );
};

Global DBlastDialog* gSeqDocPrefs = NULL;
Global Nlm_FonT gSeqFont = NULL;
 
char* DBlastDialog::gSeqFontName = (char*) "times";
short 
			DBlastDialog::gSeqFontSize = 10, 
			DBlastDialog::gSeqStyle = 0; 
Boolean 	
			DBlastDialog::gColored = true;

enum FonStyles { 
	kPlain = 0, kItalic = 1, kBold = 2, kUnderline = 4 
	};

 
DBlastDialog::DBlastDialog() :
	DWindow( 0, NULL, DWindow::fixed, -10, -10, -50, -20, "SeqDoc prefs", kDontFreeOnClose),
	fSeqFontMenu(NULL), fSeqStyleMenu(NULL), fSeqSizeSw(NULL), fCommonPctSw(NULL),
	fOldStyle(-1), fNeedSave(false)
{	
}

DBlastDialog::~DBlastDialog()
{
}


// static
void DBlastDialog::InitGlobals() 
{ 
	gSeqFontName= gApplication->GetPref( "gSeqFontName", "fonts", gSeqFontName);
	gSeqFontSize= gApplication->GetPrefVal( "gSeqFontSize", "fonts", "10");
	gSeqStyle= gApplication->GetPrefVal( "gSeqStyle", "fonts", "0");
	gSeqFont= ::GetAFont(gSeqFontName,gSeqFontSize, gSeqStyle);

	DSeqFile::gWriteMasks= gApplication->GetPrefVal( "gWriteMasks", "seqdoc", "1");
	DSeqDoc::fgStartDoc	= gApplication->GetPrefVal( "fgStartDoc", "seqdoc", "1");
	DSeqList::gMinCommonPercent= gApplication->GetPrefVal( "gMinCommonPercent", "seqdoc", "70");
	//gColored= gApplication->GetPrefVal( "gColored", "seqdoc","1");

}


//static
void DBlastDialog::SaveGlobals() 
{
	gApplication->SetPref( gSeqFontName, "gSeqFontName", "fonts");
	gApplication->SetPref( gSeqFontSize, "gSeqFontSize", "fonts");
	gApplication->SetPref( gSeqStyle, "gSeqStyle", "fonts");
	
	//gApplication->SetPref( gColored, "gColored", "seqdoc");
	gApplication->SetPref(  DSeqFile::gWriteMasks, "gWriteMasks", "seqdoc");
	gApplication->SetPref(  DSeqDoc::fgStartDoc, "fgStartDoc", "seqdoc");
	gApplication->SetPref(  DSeqList::gMinCommonPercent, "gMinCommonPercent", "seqdoc");
}





void DBlastDialog::Initialize() 
{ 
	DView* super;
	DCluster* clu;
	DCheckBox* ck;

	char* blastProgram = "blastn";
	char* blastDatabase = "nr";
	char* blastOptions = NULL;
	char* fastaInputFile = "spup-blast-input.fasta";
	char* outputFile = "spup-blast-result.text";

	super= this;
	
	new DPrompt( 0, super, "BLAST program", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
		// fBlastProgMenu needs to be configurable from table file/prefs file
	fBlastProgMenu= new DPopupList( cBlastProg, super, true);
	super->NextSubviewBelowLeft();
	if (fSeq->IsAmino() {
	  	// protein query seq
	  fBlastProgMenu->AddItem("blastp");
	  fBlastProgMenu->AddItem("tblastn");
		}
	else {
			// nucleic query seq
	  fBlastProgMenu->AddItem("blastn");
	  fBlastProgMenu->AddItem("blastx");
	  fBlastProgMenu->AddItem("tblastx");
	  }
  fBlastProgMenu->SetValue(1); 


	new DPrompt( 0, super, "Nucleic databanks", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
		// fDataMenu needs to be configurable from table file/prefs file
	fNucDataMenu= new DPopupList( cNucData, super, true);
	super->NextSubviewBelowLeft();

  fNucDataMenu->AddItem("nr");
  fNucDataMenu->AddItem("genbank");   
  fNucDataMenu->AddItem("gbupdate");  
  fNucDataMenu->AddItem("embl");  
  fNucDataMenu->AddItem("emblu");   
  fNucDataMenu->AddItem("pdb");  
  fNucDataMenu->AddItem("vector");  
  fNucDataMenu->AddItem("vector2");  
  fNucDataMenu->AddItem("dbest");  
  fNucDataMenu->AddItem("dbsts");  
  fNucDataMenu->AddItem("epd");  
  fNucDataMenu->AddItem("kabat");  
  fNucDataMenu->AddItem("hspest");  
  fNucDataMenu->AddSeparator();  
  fNucDataMenu->SetValue(1); 

	new DPrompt( 0, super, "Amino-acid databanks", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
		// fDataMenu needs to be configurable from table file/prefs file
	fAminoDataMenu= new DPopupList( cAminoData, super, true);
	super->NextSubviewBelowLeft();

  fAminoDataMenu->AddItem("nr");
  fAminoDataMenu->AddItem("pdb");  
  fAminoDataMenu->AddItem("swissprot");  
  fAminoDataMenu->AddItem("spupdate");  
  fAminoDataMenu->AddItem("pir");  
  fAminoDataMenu->AddItem("genbank");  
  fAminoDataMenu->AddItem("gpupdate");  
  fAminoDataMenu->AddItem("tfd");  
  fAminoDataMenu->AddItem("kabat");  
  fAminoDataMenu->AddItem("hspnr");  
  fAminoDataMenu->AddItem("hspspdb");  
	 
  fAminoDataMenu->SetValue(1); 

//////////////////////////////
	super= this;
	ck= new DCheckBox(cWriteMasks, super, "Save masks in file");
	ck->SetStatus(DSeqFile::gWriteMasks);
	super->NextSubviewBelowLeft();

	// check box for auto new document on startup ??
	ck= new DCheckBox(cStartDoc, super, "New align document on startup");
	ck->SetStatus(DSeqDoc::fgStartDoc);
	super->NextSubviewBelowLeft();

	new DPrompt( 0, super, "% common bases for consensus", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
	fCommonPctSw = new DSwitchBox(0, super, true, true);
	fCommonPctSw->SetValues(DSeqList::gMinCommonPercent,100);
	super->NextSubviewBelowLeft();
	
	//ck= new DCheckBox(cColored, super, "Colored bases");
	//ck->SetStatus(gColored);

	super = this;
	//super->NextSubviewToRight();
	//super->NextSubviewBelowLeft();

	this->AddOkayCancelButtons();
}


void DBlastDialog::OkayAction() 
{ 
	short	 		aSize, aStyle, aCommon;
	char			name[256];
	DMenu 	*	aFontMenu, * aStyleMenu;
	Nlm_FonT	aFont;
	Boolean   newstyle;
	
	for (short imenu= 0; imenu<1; imenu++) {
		
		switch (imenu) {
		  case 0:
				aFontMenu= fSeqFontMenu; 
				aStyleMenu= fSeqStyleMenu; 
				aSize= fSeqSizeSw->GetValue();// aSize= gNameFontSize;
				 
				aCommon= fCommonPctSw->GetValue();
				if (aCommon != DSeqList::gMinCommonPercent) {				
					DSeqList::gMinCommonPercent= aCommon;
					fNeedSave= true;
					}
		  	break;
		  }
			
		if (aFontMenu && aFontMenu->GetFontChoice(name, sizeof(name))) {
		
			aStyle= 0;

			short item= Nlm_GetValue(((DPopupMenu*)aStyleMenu)->fPopup);
			switch (item) {
				case 1: aStyle= 0; break;
				case 2: aStyle |= kItalic; break;
				case 3: aStyle |= kBold; break;
				case 4: aStyle |= kUnderline; break;
        }
 			newstyle= aStyle != gSeqStyle;
 			
			if (StringCmp(name,"System")==0) aFont= Nlm_systemFont;
			else if (StringCmp(name,"Program")==0) aFont= Nlm_programFont;
			else aFont= Nlm_GetFont( name, aSize, aStyle & kBold, aStyle & kItalic, 
												 			aStyle & kUnderline, NULL);
			switch (imenu) {
			  case 0: 
			  if (newstyle || aSize != gSeqFontSize || StringCmp(name,gSeqFontName)!=0) {
					if (gSeqFontName) MemFree(gSeqFontName);
					gSeqFontName= StrDup(name);
					gSeqFont= aFont;
					gSeqFontSize= aSize;
					gSeqStyle= aStyle;
					fNeedSave= true;
					}
			  	break;
			  }
			  
			}
		}
}



Boolean DBlastDialog::IsMyAction(DTaskMaster* action) 
{	
	DView* aview= (DView*) action;
	
	switch (action->Id()) {
		//case cWriteMasks: DSeqFile::gWriteMasks= aview->GetStatus(); break;
		//case cStartDoc  : DSeqDoc::fgStartDoc= aview->GetStatus(); break;
		default : return DWindow::IsMyAction(action);	
		}
		
	fNeedSave= true;
	return true;
}


void DBlastDialog::Open()
{
	DWindow::Open();
}

void DBlastDialog::Close()
{
	if (fNeedSave) {
		DBlastDialog::SaveGlobals();
		fNeedSave= false;
		}
	DWindow::Close();
}


		
#endif
