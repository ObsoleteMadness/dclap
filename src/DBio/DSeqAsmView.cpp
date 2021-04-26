// DSeqAsmView.cpp


#define TESTVARHEIGHT
#define TESTZOOM
#undef  USEDRAWBASE
#define POPBASECALL


#include "DSeqViews.h"
#include "DSeqAsmView.h"
#include "DAsmSequence.h"

#include <ncbi.h>
#include <dgg.h>
#include <Dvibrant.h>
#include <DApplication.h>
#include <DClipboard.h>
#include <DControl.h>
#include <DWindow.h>
#include <DRichViewNu.h>
#include <DTableView.h>
#include <DViewCentral.h>
#include <DTask.h>
#include <DTracker.h>
#include <DMenu.h>
#include <DUtil.h>
#include <DFindDlog.h>
#include "DSeqFile.h"
#include "DSeqEd.h"
#include "DSeqMail.h"
#include "DSeqCmds.h"
#include "DSeqChildApp.h"
#include "DSeqPrint.h"
#include "DSeqPict.h"
#include "DSeqDoc.h"




enum {
	kORFxtraHeight = 4,
	kTracextraHeight = 64,
	kTracePointWidth = 3,  // ?? 1, 2 or 3 ? 
	knadaxxx
	};
	
typedef unsigned long  colorVal;

Local short gAlnCharWidth  = 12;
Local short gAlnCharHeight = 15;
Local short gBaseCharWidth = 12;
Local short gRealCharWidth = 12;
Local Boolean gSwapBackground = false;

extern "C" void Nlm_SelectBackColor(Nlm_Uint1 red, Nlm_Uint1 green, Nlm_Uint1 blue);
extern "C" void Nlm_SetBackColor (Nlm_Uint4 color);
extern "C" Nlm_Boolean Nlm_MacPopListClick(Nlm_GraphiC m, Nlm_PoinT pt);
extern "C" Nlm_Boolean Nlm_DoClick (Nlm_GraphiC a, Nlm_PoinT pt);
extern "C" Nlm_GraphiC Nlm_GetParent (Nlm_GraphiC a);

//inline 
static short BaseCharWidth()
{
#ifdef TESTZOOM
	if (gBaseCharWidth>5) gBaseCharWidth= Nlm_CharWidth('G');
	gRealCharWidth= Nlm_CharWidth('G');
#else
	gBaseCharWidth= gRealCharWidth= Nlm_CharWidth('G');
#endif
	return gBaseCharWidth;
}


#ifdef POPBASECALL

#include "DBaseCallPop.h"


//DBaseCallPop* gBaseCallPop = NULL;


DBaseCallPop::DBaseCallPop(long id, DAsmView* itsSuperior):
		DPopupList(id, itsSuperior->fDoc), 
		fAsmView(itsSuperior), 
		fVisible(false), fRow(0), fCol(0) 
{
	Nlm_LoadRect( &fPoprect, 0,0,0,0);
	AddItem("no change"); // default
	AddItem("clear");
	AddItem("set A");
	AddItem("set C");
	AddItem("set G");
	AddItem("set T");
	// ?? stats display at position, other functions at trace position?
}



void DBaseCallPop::LimitTrace(short item)
{
#if 0
			// this isn't working
		// disable choices when trace view is limited to one base !/
	short i;
	item += 3; // convert from base 0..3 to popup item 3..6
	
	if (item<3 || item>6)
		for ( i=3; i<7; i++) SetItemStatus(i,true);
	else {
		for ( i=3; i<7; i++) SetItemStatus(i,false);
		SetItemStatus(item,false);
		}
#endif		
}

void DBaseCallPop::Hold(Nlm_PoinT mouse)
{
  Nlm_GraphiC m = Nlm_GetParent ((Nlm_GraphiC)fPopup);
#ifdef WIN_MAC
	if ( Nlm_DoClick( (Nlm_GraphiC) m, mouse) ) ;
		// !! WORKS, gets us to DSeqAsmDoc::IsMyAction(kBaseCallPop) 
#endif

}
		 

void DBaseCallPop::Release(Nlm_PoinT mouse)
{
	//Message(MSG_OK,"DBaseCallPop::Release"); // this is seen
}


Boolean DBaseCallPop::IsMyAction(DTaskMaster* action) 
{
	switch(action->Id()) {
			
	 case kBaseCallPopup: {
	 			// call here from DSeqAsmDoc::IsMyAction()
		DAsmSequence* aSeq = NULL;
		TraceBase tr, * tra = NULL;
		Boolean doupdate= false;
		short	mode= this->GetValue();
		long	i, maxcol= 0, atcol = fCol;
		
		if (mode>1) {  
			aSeq= (DAsmSequence*)fAsmView->fSeqList->SeqAt(fRow);
			maxcol= aSeq->NumPoints();
			if (mode>2) // make sure we don't overwrite other base!
				for (i=0; 
					aSeq->TraceAt(atcol).base>' ' && atcol<maxcol && i<20; 
					i++, atcol++) ;
			tr = aSeq->TraceAt(atcol);
			}
			
		switch (mode) {
			case 1:  break;  // nothing...
			case 2:  tr.base= 0; doupdate= true; break;	 
			case 3:  tr.base= 'A'; doupdate= true; break;  
			case 4:  tr.base= 'C'; doupdate= true; break; 	
			case 5:	 tr.base= 'G'; doupdate= true; break; 
			case 6:  tr.base= 'T'; doupdate= true; break;
			}
			
		if (doupdate) {
			Nlm_RecT  r;
			aSeq->SetTraceAt(atcol, tr);
			aSeq->SetChanged(true);
			// !! need to update aSeq->fBases sometime w/ new set !!
			// need undo/redo of change !
			
			fAsmView->GetCellRect(fRow, fCol, r);  
			r.left  = Min( r.left, fPoprect.left-20);
			r.right = Max( r.right, fPoprect.right+20);
			fAsmView->InvalRect( r);  // fPoprect

			if (fAsmView->fDoc->fAlnIndex) {
				fAsmView->fDoc->fAlnIndex->GetRowRect( fRow, r);
				fAsmView->fDoc->fAlnIndex->InvalRect( r);
				}
			}
  	return true;
		}
		
		default:
			return DPopupList::IsMyAction(action);
		}
}



void DBaseCallPop::Show(Nlm_PoinT mouse)
{
	long row, col;

	fAsmView->PointToCell( mouse, row, col);
	if (col) col--; // fix off-by-one error
	if ( row >=0 && row < fAsmView->GetMaxRows()
	 && col >= 0 && col < fAsmView->GetMaxCols()) {
		Nlm_RecT  r;
		short   w, h;
		fMouse= mouse;
		fRow= row;
		fCol= col;

		this->GetPosition(r);
		w= r.right - r.left;
		h= r.bottom- r.top;
		r.left= mouse.x - 10;  // make sure mouse is in rect..
		r.top = mouse.y - 10;
		r.right= r.left + w;
		r.bottom= r.top + h;
		fPoprect= r;
		this->SetPosition(r);		
		
		Show();
		}
}

void DBaseCallPop::Show()
{
	if (!fVisible) {
		fVisible= true;
		DPopupList::Show(); 
		this->SetValue(1); // ?? force label to show?
		Enable();
		}
}

void DBaseCallPop::Hide()
{
	if (fVisible) {
		fVisible= false;
		Disable();
		DPopupList::Hide();
		fAsmView->InvalRect(fPoprect);
		}
}

#endif





//class DAsmView : public DAlnView  


DAsmView::DAsmView( long id, DView* itsSuper, DSeqDoc* itsDocument, DSeqList* itsSeqList, 
								long pixwidth, long pixheight) :
	DAlnView( id, itsSuper, itsDocument, itsSeqList,  pixwidth,  pixheight),
	fHaveAutoseq(false), fBaseOnTrace(true)  				
{ 
	fKind= kindAlnView;
	for (short t=0; t<4; t++) fShowTrace[t]= true;
	
#ifdef TESTZOOM
	//DSequence* aSeq= fSeqList->SeqAt(0);
	if (1) { // (aSeq && aSeq->ShowTrace())
		fHaveAutoseq= true;
		gBaseCharWidth= kTracePointWidth; 
		}
	else gBaseCharWidth= Nlm_stdCharWidth;
#endif

	gAlnCharWidth= gBaseCharWidth; //Nlm_stdCharWidth; //BaseCharWidth();   
	gAlnCharHeight= Nlm_stdLineHeight;
	
#ifdef POPBASECALL
  Nlm_PoinT  nps, saveps;
#if 1
	fDoc->GetNextPosition( &saveps);
	nps.x= 1; nps.y= 1; // locate dialog item 
	fDoc->SetNextPosition( nps);
#endif
	
	DBaseCallPop* bcpop= new DBaseCallPop( kBaseCallPopup, this);  
	fBaseCallPop= bcpop;  
	bcpop->fVisible= true;
	bcpop->Hide();  
#if 1
	fDoc->SetNextPosition( saveps);
#endif
#endif

}


DAsmView::~DAsmView()
{
}

				

void DAsmView::CheckViewCharWidth()
{
	if (fEditSeq) {
		Nlm_RecT vr;
		ViewRect( vr);
		long cwidth= (vr.right - vr.left) / GetItemWidth(0);
		fEditSeq->SetColumns( cwidth);
		}
}


void DAsmView::GetReadyToShow()
{
	// assume port is set??
	SelectFont();  
	gAlnCharWidth = BaseCharWidth();   
		// use Nlm_FontHeight for background color to adjoin among rows
	gAlnCharHeight= Nlm_FontHeight(); //Nlm_LineHeight(); // + 2; 
	
	//SetItemWidth(0, GetMaxCols(), gAlnCharWidth); // done in UpdateAllWidths
	this->UpdateAllWidths(); 
	this->UpdateSize();  // need before GetMaxRows()!

#ifdef TESTVARHEIGHT
	long i, nrow= GetMaxRows();
	for (i= 0; i<nrow; i++) {
		short ht= gAlnCharHeight;
		DSequence* aSeq= fSeqList->SeqAt(i);
		if (aSeq && aSeq->ShowORF()) ht += kORFxtraHeight;
		if (aSeq && aSeq->ShowTrace()) ht += kTracextraHeight;
		SetItemHeight(i, 1, ht);
		}	
#else
	SetItemHeight(0, GetMaxRows(), gAlnCharHeight);
#endif
}





void DAsmView::Drag(Nlm_PoinT mouse)
{
	DAlnView::Drag(mouse); // sets fMouseStillDown= true
}

void DAsmView::Hold(Nlm_PoinT mouse)
{
#ifdef POPBASECALL
	if (fBaseCallPop) {
		if (!fBaseCallPop->fVisible) fBaseCallPop->Show(mouse);
		if ( fBaseCallPop->fVisible) fBaseCallPop->Hold(mouse);
		}
#endif
	DAlnView::Hold(mouse);
}

void DAsmView::Release(Nlm_PoinT mouse)
{
#ifdef POPBASECALL
	if (fBaseCallPop) {
	  if ( fBaseCallPop->fVisible) fBaseCallPop->Hide();
		if (!fBaseCallPop->fVisible) fBaseCallPop->Release(mouse);
		}
#endif
	DAlnView::Release(mouse);
}

void DAsmView::DoubleClickAt(long row, long col)
{
}

void DAsmView::Click(Nlm_PoinT mouse)
{ 
#if 0
	DAlnView::Click( mouse);
#else
	long 	row, col;
	Nlm_CaptureSlateFocus((Nlm_SlatE) fPanel); // for CharHandler !
	PointToCell( mouse, row, col);
	SeqMeter(row,col);
	if (fMaskLevel<1 && IsSelected(row, col)) {

		}
	else {
		fTabSelector= new DTabSelector( this);
		fCurrentTracker= fTabSelector;
		}
	DTableView::Click( mouse);
#endif
}
 
 
void DAsmView::TrackMouse( short aTrackPhase,
					Nlm_PoinT& anchorPoint, Nlm_PoinT& previousPoint,
					Nlm_PoinT& nextPoint,	Nlm_Boolean mouseDidMove)
{
	DAlnView::TrackMouse( aTrackPhase, anchorPoint, previousPoint, nextPoint,mouseDidMove); 
}
 

void DAsmView::TrackFeedback( short aTrackPhase,
					const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
					const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn)
{
#if 1
	DAlnView::TrackFeedback( aTrackPhase, anchorPoint, previousPoint, nextPoint,
					mouseDidMove, turnItOn);
#else
	long 	row, col;
	if (mouseDidMove && aTrackPhase == DTracker::trackContinue ) { // 2 == trackContinue
		PointToCell( nextPoint, row, col);
		SeqMeter(row,col);
		}
	DTableView::TrackFeedback( aTrackPhase, anchorPoint, previousPoint, nextPoint,
					mouseDidMove, turnItOn);
#endif
}


void DAsmView::SingleClickAt(long row, long col)
{
#if 1
 		DAlnView::SingleClickAt(row,  col);
#else
	if (!(fLocked || gKeys->shift() || gKeys->command() || gKeys->option() )) {  
		SetEmptySelection( true); 
		if (fDoc->fAlnIndex) fDoc->fAlnIndex->SetEmptySelection( true); 
		InstallEditSeq(row,col,col,false);
		}
	else 	if (fMaskLevel>0) {
		Nlm_RecT r;
		DSequence* ag= fSeqList->SeqAt(row); 
		if (ag) {
			if (gKeys->shift()) ag->SetMaskAt(col,fMaskLevel);
			else ag->FlipMaskAt(col,fMaskLevel);
			}
		GetCellRect( row, col, r);
		InvalRect( r);
		}
	else
 		DTableView::SingleClickAt(row,  col);
#endif
}

	


void DAsmView::Scroll(Boolean vertical, DView* scrollee, long newval, long oldval)
{
	DAlnView::Scroll(vertical, scrollee, newval, oldval);
}





void DAsmView::UpdateWidth(DSequence* aSeq)
{ 	
	long alnlen;
	if (fHaveAutoseq)  
		alnlen= ((DAsmSequence*)aSeq)->NumPoints() + 30;
	else
		alnlen= aSeq->LengthF() + 30;
	if (alnlen > GetMaxCols()) ChangeColSize( -1, alnlen-GetMaxCols());  
	SetItemWidth( 0, GetMaxCols(), gAlnCharWidth); 
	if (fMaskLevel>0 || aSeq->Masks()) aSeq->FixMasks();
}

void DAsmView::UpdateAllWidths()
{
	long alnlen= 0;
	long i, nseq= fSeqList->GetSize();
	for (i=0; i<nseq; i++) {
		DSequence* aSeq= fSeqList->SeqAt(i);
		if (fHaveAutoseq)  
			alnlen= Max( alnlen, ((DAsmSequence*)aSeq)->NumPoints());
		else
			alnlen= Max(alnlen, aSeq->LengthF()); 
		if (fMaskLevel>0 || aSeq->Masks()) aSeq->FixMasks();
		}
	alnlen += 30; 
	if (alnlen > GetMaxCols()) ChangeColSize( -1, alnlen-GetMaxCols());
	SetItemWidth( 0, GetMaxCols(), gAlnCharWidth);  
	//if (fDoc && fDoc->fAlnHIndex) fDoc->fAlnHIndex->UpdateWidth(); //keep ruler in sync
}



void DAsmView::SetViewMode(short viewmode) 
{
	short  t, i;
	switch (viewmode) {
	
		case kShowTraces: 
			for (t=0; t<4; t++) fShowTrace[t]= true;
			this->fMaskLevel= 1;
			fBaseCallPop->LimitTrace(-1);
			goto caseMainSet;
			
		case kShowTraceA: 
			i= 0; goto caseTraceI;
		case kShowTraceC: 
			i= 1; goto caseTraceI;
		case kShowTraceG: 
			i= 2; goto caseTraceI;
		case kShowTraceT: 
			i= 3; //goto caseTraceI;
		caseTraceI:
			for (t=0; t<4; t++) fShowTrace[t]= false;
			fShowTrace[i]= true;
			fBaseCallPop->LimitTrace(i);
			this->fMaskLevel= 1;
			// goto caseMainSet;
		case kModeSlide: 
		caseMainSet:
			this->SetTextLock( true);
			if (this->fMaskLevel>0) this->Invalidate();
			this->fMaskLevel= 0; 
			break;
			
		case kModeEdit:  
			this->SetTextLock( false);
			if (this->fMaskLevel>0) this->Invalidate();
			this->fMaskLevel= 0; 
			break;
			
		default :
			break;
		}
}









 
static void DrawTrace( long atx, long y0, short t, TraceBase* trace, 
					long startcol, long stopcol, short cwidth, float scale)
{
#define kTYScale 0.25     // need Scale of ht !
	//short	cwidth= GetItemWidth(0); 
	long aty;
	Nlm_MoveTo(atx, y0);
	for (long i= startcol+1; i<stopcol; i++) {
		//if (fWidths) atx += GetItemWidth(i); else
		atx += cwidth;
		aty = y0 - (long)(trace[i].tr[t] * scale);
		Nlm_LineTo( atx, aty);
		}
}


#ifdef USEDRAWBASE

class DrawBasesA
{
public:
	Nlm_PoinT pt;
	long			endx, pend, indx, len, cw, cht;
	long			row, ytop, ybot;
	char			ch, lastch, *pText;
	DAsmView * tview;
	Boolean  	swapBackcolor, fDoAll;
	float 		fScale;
	char			fAtch;
	Boolean 	fIsGrey;
	
	DrawBasesA() : fIsGrey(false) {}
	virtual void doDraw(DAsmView* theView, baseColors theColors, Boolean backcolor,
		       char*	theText, long theIndx, long theLen, long theRow);
	virtual void Calc();
	
	virtual void DrawSub(baseColors colors);
	virtual void DrawORFMasks(baseColors colors, short masklevel);
	virtual void DrawTraces(baseColors colors);
};

class DrawGreyBasesA : public DrawBasesA
{
public:	
	DrawGreyBasesA() { fIsGrey = true; }
};



void DrawBasesA::Calc() 
{
	long skip;
	cht= tview->GetItemHeight(row);
	cw = tview->GetItemWidth(0); 

	DAsmSequence* aSeq= (DAsmSequence*) tview->fCurSeq;
	fScale= kTracextraHeight;
	short t, att, nt;
	for (t= 0, att= 0, nt= 0; t<4; t++) if (tview->fShowTrace[t]) { att= t; nt++; } 
	if (nt==1) {
		switch (att) {
			case 0: fAtch= 'A'; break;
			case 1: fAtch= 'C'; break;
			case 2: fAtch= 'G'; break;
			case 3: fAtch= 'T'; break;
			}
		fScale /= aSeq->MaxHeight(att);
		fDoAll= false;
		}
	else {
		fDoAll= true;
		fScale /= aSeq->MaxHeight(4);
		}
		
	Nlm_GetPen( &pt);
	ytop= pt.y-cht+2;
	ybot= pt.y+2;  
	if (pt.x < 0) {
		skip= (-pt.x) / cw;
  	indx+= skip;
		len -= skip;
		Nlm_MoveTo( pt.x + skip*cw, pt.y);
		}
	pend= len * cw;
	
	if (pend > tview->GetRect().right) {   
		skip= (pend - tview->GetRect().right) / cw;  
		len -= skip;
		}
	endx= indx + len - 1;
}




void DrawBasesA::doDraw(DAsmView* theView, baseColors theColors, Boolean backcolor,
	       char*	theText, long theIndx, long theLen, long theRow)
{
	tview= theView;
	//colors= &theColors;
	pText= theText;
	indx= theIndx;
	len= theLen;
	row= theRow;
	lastch= 0;
	swapBackcolor= backcolor;

	Calc();

	if (swapBackcolor) Nlm_CopyMode();
	Nlm_PoinT savept= pt;
	
#if 1
	if (tview->fCurSeq->ShowTrace()) {
		pt= savept;
		pt.y -= 2;  
		DrawTraces(theColors);		
		savept.y -= kTracextraHeight;
		pt= savept;
		Nlm_MoveTo( pt.x, pt.y);
		}
#endif		

#if 1
	if (tview->fCurSeq->ShowORF()) {
		pt= savept;
		pt.y -= (kORFxtraHeight - 3);
		DrawORFMasks(theColors, 5);			
		//pt= savept; pt.y -= 7;
		//DrawORFMasks(theColors, 6);			
		//pt= savept; /pt.y - 2;
		//DrawORFMasks(theColors, 7);			
		
		savept.y -= kORFxtraHeight;
		pt= savept;
		Nlm_MoveTo( pt.x, pt.y);
		}
#endif

	DrawSub(theColors);
	
#if 0
	if (tview->fCurSeq->ShowRE()) {
		DrawREMasks();			
		}
#endif

	if (swapBackcolor) Nlm_MergeMode();
}



void DrawBasesA::DrawSub(baseColors colors)
{
	Nlm_RecT	crec;  
	DAsmSequence* aSeq= (DAsmSequence*) tview->fCurSeq;
	TraceBase* trace= aSeq->Trace();
	
	for (long i= indx; i<=endx; i++) {

		Boolean notInverted= true;
		ch= trace[i].base;

		if ( (fDoAll && ch > ' ') || ch == fAtch) {
			//if (fIsGrey) {
				//if (swapBackcolor) Nlm_SetBackColor( tview->fColcolors[i-indx]);  else 
				//Nlm_SetColor( tview->fColcolors[i-indx]);
				//}
			//else 
			if (ch!=lastch) {
				//if (swapBackcolor) Nlm_SetBackColor( colors[ch-' ']);  else 
				Nlm_SetColor( colors[ch-' ']);
				}
				
			Nlm_PaintChar(ch);
			lastch= ch;
			}
				
		if (notInverted) {
		  if (tview->fSelection->IsSelected(row, i)) {
				Nlm_LoadRect( &crec, pt.x, ytop, pt.x+cw, ybot); //??
				tview->DTableView::InvertRect( crec);
				}
			}
			
		pt.x += cw;
		}
	Nlm_Black();
}



void DrawBasesA::DrawTraces(baseColors colors)
{
	DAsmSequence* aSeq= (DAsmSequence*) tview->fCurSeq;
	for (short t= 0; t<4; t++) if (tview->fShowTrace[t]) {
		char ch;
		switch (t) {
			case 0: ch= 'A'; break;
			case 1: ch= 'C'; break;
			case 2: ch= 'G'; break;
			case 3: ch= 'T'; break;
			}
		Nlm_SetColor( colors[ch-' ']);
		::DrawTrace( pt.x, pt.y, t, aSeq->Trace(), indx, endx, cw, fScale);
		}
	Nlm_Black();
}


// this should move into DSeqView's DrawBases class !! not useful w/ trace view 
void DrawBasesA::DrawORFMasks(baseColors colors, short masklevel)
{
	enum { otic = 3 };
	long		lastx, startx = pt.x;
	Boolean inorf= false;
	
	for (long i= indx; i<=endx; i++) {
		ch= pText[i];
		if (ch >= ' ') {
			short maskval= tview->fCurSeq->MaskAt(i, masklevel);
			if (maskval && !inorf) { 
				startx= lastx= pt.x; 
				inorf= true; 
				}
			else if ( !maskval 
				&& inorf 
				&& (DSequence::NucleicBits(ch) != DSequence::kMaskIndel)
				) {
				Nlm_MoveTo( startx, pt.y-otic);
				Nlm_LineTo( startx, pt.y);
				Nlm_LineTo( lastx, pt.y);
				Nlm_LineTo( lastx, pt.y-otic);
				inorf= false;
				}
			lastx= pt.x;
			pt.x += cw;
			}
		}
	if (inorf) {
		Nlm_MoveTo( startx, pt.y-otic);
		Nlm_LineTo( startx, pt.y);
		Nlm_LineTo( pt.x, pt.y);
		inorf= false;
		}
		
	//Nlm_Black();
}

	
DrawBasesA         gDrawBasesA;
DrawGreyBasesA     gDrawGreyBasesA;
#endif  // USEDRAWBASE


Local Boolean 		 gDoDrawColors = false;

void DAsmView::DrawAllColors(Nlm_RecT r, long row)
{ 
	DAsmSequence* aSeq= (DAsmSequence*)fSeqList->SeqAt(row);
	if (!aSeq || !aSeq->Bases()) return;
	
	Boolean dotrace= aSeq->ShowTrace() && (aSeq->Kind() == DAsmSequence::kAutoseq);
	if (!dotrace) { DAlnView::DrawAllColors( r, row); return; }

#ifndef USEDRAWBASE
	gDoDrawColors= true;
	DrawNoColors( r, row);
	gDoDrawColors= false;
	
#else
	long  len, newright, stopcol, startcol = GetLeft(); 
	char* hSeq= aSeq->Bases();
	fCurSeq= aSeq;

	for (stopcol= startcol, newright= r.left; 
		stopcol<GetMaxCols() && newright<r.right;
		stopcol++) {
		if (fWidths) newright += GetItemWidth(stopcol); else 
		newright += GetItemWidth(0);
		}
			
	len= Min( stopcol, ((DAsmSequence*)aSeq)->NumPoints());
	//len= Min( aSeq->LengthF(), stopcol);
	len -= startcol;
	if (len>0) {
		Nlm_MoveTo( r.left, r.bottom-2); //bottom-5
#if 0
		if (aSeq->Kind() == DSequence::kAmino)
			gDrawBasesA.doDraw(this, DBaseColors::gAAcolors, gSwapBackground,
					 hSeq, startcol, len, row);
		else
#endif
			gDrawBasesA.doDraw(this, DBaseColors::gNAcolors, gSwapBackground,
					 hSeq, startcol, len, row);
		}
 
	fColsDrawn= stopcol - startcol;
#endif
}  



void DAsmView::DrawNoColors(Nlm_RecT r, long row)
{ 
	DAsmSequence* aSeq= (DAsmSequence*)fSeqList->SeqAt(row);
	if (!aSeq || !aSeq->Bases()) return;
	
	Boolean dotrace= aSeq->ShowTrace() && (aSeq->Kind() == DAsmSequence::kAutoseq);
	if (!dotrace) { DAlnView::DrawNoColors( r, row); return; }
	
	long  y0, len, atx, aty, stopcol, startcol = GetLeft(), newright;
	short cwidth= GetItemWidth(0); 
	TraceBase* trace = aSeq->Trace();
	if (!trace) return; // early calls here before traces read !
	
	for (stopcol= startcol, newright= r.left; 
		stopcol<GetMaxCols() && newright<r.right; 
		stopcol++) {
			//if (fWidths) newright += GetItemWidth(stopcol); else 
			newright += cwidth;
			}
	
	//len= Min( aSeq->LengthF(), stopcol);
	len = Min( stopcol, ((DAsmSequence*)aSeq)->NumPoints());  
	len -= startcol;
	if (len>0) {
		atx= r.left;
		aty= y0 = r.bottom-2;
		stopcol= startcol + len;

		float scale = kTracextraHeight;
		short t, att, nt;
		char ch, atch, lastch= 0;
		short xoffset= gRealCharWidth / 2;
		
		for (t= 0, att= 0, nt= 0; t<4; t++) if (this->fShowTrace[t]) { att= t; nt++; } 
		if (nt==1) {
			switch (att) {
				case 0: atch= 'A'; break;
				case 1: atch= 'C'; break;
				case 2: atch= 'G'; break;
				case 3: atch= 'T'; break;
				}
			scale /= aSeq->MaxHeight(att);
			}
		else scale /= aSeq->MaxHeight(4);
		
				// draw traces
		if (aSeq->ShowTrace()) {
		  for ( t= 0; t<4; t++)
		  if (fShowTrace[t]) {
				switch (t) {
					case 0: ch= 'A'; break;
					case 1: ch= 'C'; break;
					case 2: ch= 'G'; break;
					case 3: ch= 'T'; break;
					}
				if (gDoDrawColors) Nlm_SetColor( DBaseColors::gNAcolors[ch-' ']);
				::DrawTrace( atx, aty, t, aSeq->Trace(), startcol, stopcol, cwidth, scale);
				}
			Nlm_Black();
			aty -= ( kTracextraHeight - 2); //??
			}
		
			// draw ORFs
		if (aSeq->ShowORF()) {
			aty -= ( kORFxtraHeight ); //??
			}
		
			// draw bases
		aty= r.top + gAlnCharHeight - 2; //??
		for (long i= startcol; i<stopcol; i++) {
			long y1, x1;
			ch= trace[i].base;
			if ( (nt  > 1 && ch > ' ')
			  || (nt == 1 && ch == atch)) {

				if (lastch != ch && gDoDrawColors) 
					Nlm_SetColor( DBaseColors::gNAcolors[ch-' ']);

				switch (ch) {
					case 'A': t= 0; break;
					case 'C': t= 1; break;
					case 'G': t= 2; break;
					case 'T': t= 3; break;
					}
					
				if (fBaseOnTrace) { 
					y1 = Max( aty, y0 - (long)(trace[i].tr[t] * scale) - 2);
					}
				else 
					y1= aty;
				
				Nlm_MoveTo(atx - xoffset, y1);
				Nlm_PaintChar(ch);
				lastch= ch;
				}
		  if (this->fSelection->IsSelected(row, i)) {
				Nlm_RecT crec;
				Nlm_LoadRect( &crec, atx, r.top, atx+cwidth, r.bottom); 
				DTableView::InvertRect( crec); 
				}
			//if (fWidths) atx += GetItemWidth(i); else
			atx += cwidth;
			}
  	}
	Nlm_Black();
	fColsDrawn= stopcol - startcol;
}  

		
void DAsmView::Draw()
{
	DAlnView::Draw();
}


void DAsmView::DrawRow(Nlm_RecT r, long row)
{
	DAlnView::DrawRow(r, row);
}

		




