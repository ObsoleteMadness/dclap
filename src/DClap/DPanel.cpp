// DPanel.cp
// d.g.gilbert


#include "Dvibrant.h"
#include "DWindow.h"
#include "DPanel.h"
#include "DControl.h"
#include "DIconLib.h"
#include "DCommand.h"
#include "DTracker.h"


extern "C" void panelDrawProc(Nlm_PaneL	item)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj)  obj->Draw();
}

extern "C" void panelVscrollProc(Nlm_BaR sb, Nlm_SlatE s, short newval, short oldval)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)s);
	if (obj) obj->Scroll(true, obj, newval, oldval);
}

extern "C" void panelHscrollProc(Nlm_BaR sb, Nlm_SlatE s, short newval, short oldval)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)s);
	if (obj) obj->Scroll(false, obj, newval, oldval);
}

extern "C" void panelClickProc(Nlm_PaneL item, Nlm_PoinT mouse)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Click(mouse);
}

extern "C" void panelDragProc(Nlm_PaneL item, Nlm_PoinT mouse)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Drag(mouse);
}

extern "C" void panelHoldProc(Nlm_PaneL item, Nlm_PoinT mouse)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Hold(mouse);
}

extern "C" void panelReleaseProc(Nlm_PaneL item, Nlm_PoinT mouse)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Release(mouse);
}

extern "C" void panelCharProc(Nlm_SlatE s, Nlm_Char c)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)s);
	if (obj) obj->CharHandler(c);
}


//class DPanel : public DView

DPanel::DPanel(long id, DView* itsSuperior, short pixwidth, short pixheight, PanelKind kind) :
		DView( id, NULL, kPanel, itsSuperior),
		fMouseStillDown(false), fTracker(NULL),
		fPanelKind(kind) 
{
	switch (kind) {
	
		case simple:
			fPanel= Nlm_SimplePanel((Nlm_WindoW)itsSuperior->GetNlmObject(), 
			  pixwidth, pixheight, panelDrawProc);
			break;
			
		case autonomous:
			fPanel= Nlm_AutonomousPanel((Nlm_WindoW)itsSuperior->GetNlmObject(),
			  pixwidth, pixheight, 
				panelDrawProc, panelVscrollProc, panelHscrollProc, 0/*extra*/, NULL/*panelResetProc*/, 
				NULL/*panelGraphProcPtr*/);
			Nlm_SetSlateChar( (Nlm_SlatE) fPanel, panelCharProc);
			// ?? make autonomous panel a new class ??
			break;
			
		case slatebound:
			fPanel= Nlm_CustomPanel((Nlm_SlatE)itsSuperior->GetNlmObject(), panelDrawProc, 
				0/*extra*/, NULL/*panelResetProc*/);
			break;
			
		case subclasscreates:
			return; // this null case used by DDocPanel
		}
	Nlm_SetPanelClick(fPanel, panelClickProc, panelDragProc, panelHoldProc, panelReleaseProc);
	this->SetNlmObject(fPanel);	
}

extern "C" void Nlm_ClearPanelData(Nlm_PaneL p); /* in vibslate.c */

DPanel::~DPanel()
{
		// this is to stop XWin callbacks -- don't think it is working ...
	//Nlm_ClearPanelData( fPanel);
}

void DPanel::SetSlateBorder(Boolean turnon) 
{
 	if (fPanelKind == autonomous) 
 		Nlm_SetSlateBorder((Nlm_SlatE)fPanel, turnon);  
}


//void DPanel::Draw() { } // abstract class, user must subclass

void DPanel::Scroll(Boolean vertical, DView* scrollee, long newval, long oldval)
{
}



void DPanel::SetTracker( DTracker* mouseTracker)
{
	//if (fTracker) fTracker->SetTracker(mouseTracker);
	fTracker= mouseTracker;
}

Nlm_Boolean DPanel::IsDoneTracking()
{
	if (fTracker) return !fMouseStillDown;
	else return true;
}

void DPanel::Click(Nlm_PoinT mouse)
{
	// this is always called before Drag/Hold/Release
	// Release is only called if Drag/Hold are called (only if mouse is down for a while)
	fMouseStillDown= false;
	if (fTracker) fTracker= fTracker->Start( mouse);
}

void DPanel::Drag(Nlm_PoinT mouse)
{
	fMouseStillDown= true;
	if (fTracker) fTracker= fTracker->Continue( mouse, true);
}

void DPanel::Hold(Nlm_PoinT mouse)
{
	fMouseStillDown= true;
	if (fTracker) fTracker= fTracker->Continue( mouse, false);
}

void DPanel::Release(Nlm_PoinT mouse)
{
	fMouseStillDown= false;
	if (fTracker) fTracker= fTracker->Finish( mouse);
	if (fTracker) fTracker->Process(); // DoIt, if the command is still here...
}

void DPanel::TrackMouse( short aTrackPhase,
					Nlm_PoinT& anchorPoint, Nlm_PoinT& previousPoint,
					Nlm_PoinT& nextPoint,	Nlm_Boolean mouseDidMove)
{
}

void DPanel::TrackFeedback( short aTrackPhase,
					const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
					const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn)
{
}



void DPanel::CharHandler(char c) 
{
}


//class DAutoPanel : public DPanel

DAutoPanel::DAutoPanel(long id, DView* itsSuperior, short pixwidth, short pixheight,
			Boolean hasVscroll, Boolean hasHscroll) :
		DPanel(id, itsSuperior, pixwidth, pixheight, DPanel::subclasscreates)
{
	fPanelKind = DPanel::autonomous;
	Nlm_SltScrlProc vproc= NULL;
	Nlm_SltScrlProc hproc= NULL;
	if (hasVscroll) vproc= panelVscrollProc;
	if (hasHscroll) hproc= panelHscrollProc;
	fPanel= Nlm_AutonomousPanel((Nlm_WindoW)itsSuperior->GetNlmObject(),
	  pixwidth, pixheight, panelDrawProc, vproc, hproc, 0/*extra*/, NULL/*panelResetProc*/, 
		NULL/*panelGraphProcPtr*/);
	Nlm_SetSlateChar( (Nlm_SlatE) fPanel, panelCharProc);
	Nlm_SetPanelClick(fPanel, panelClickProc, panelDragProc, panelHoldProc, panelReleaseProc);
	this->SetNlmObject(fPanel);	
}




//class DNotePanel : public DPanel


extern "C" void notepDrawProc(Nlm_IcoN item)
{
	DNotePanel *obj= (DNotePanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Draw();
}

extern "C" void notepInvalProc(Nlm_IcoN item, Nlm_Int2 newval, Nlm_Int2 oldval)
{
	DNotePanel *obj= (DNotePanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Invalid(newval, oldval);
}

extern "C" void notepClickProc(Nlm_IcoN item, Nlm_PoinT mouse)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Click(mouse);
}

extern "C" void notepDragProc(Nlm_IcoN item, Nlm_PoinT mouse)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Drag(mouse);
}

extern "C" void notepHoldProc(Nlm_IcoN item, Nlm_PoinT mouse)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Hold(mouse);
}

extern "C" void notepReleaseProc(Nlm_IcoN item, Nlm_PoinT mouse)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Release(mouse);
}



DNotePanel::DNotePanel(long id, DView* itsSuperior, const char* title, 
			short pixwidth, short pixheight, Nlm_FonT font, Justify just) :
	DPanel( id, itsSuperior, pixwidth, pixheight, subclasscreates),
	fText(NULL),
	fTextLen(0),
	fFont(font),
	fJust(just),
	fDimmed(false),
	fEnabled(false)
{
	fText= StrDup(title);
	if (fText) fTextLen= StrLen(fText);
	if (pixwidth==0 || pixheight == 0) {
		short width = pixwidth, height = pixheight;
		this->GetSize(width,height);
		if (pixwidth==0) pixwidth= width;
		if (pixheight==0) pixheight= height;
		}
	fIconBut= Nlm_IconButton((Nlm_WindoW)itsSuperior->GetNlmObject(), pixwidth, pixheight, 
		notepDrawProc, notepInvalProc, notepClickProc, notepDragProc, notepHoldProc, notepReleaseProc);
	fPanel= (Nlm_PaneL) fIconBut;
	this->SetNlmObject(fIconBut);	
}


void DNotePanel::SetTitle( char* title) 
{ 
	Nlm_RecT r;
	short width,height,curwidth,curheight;
	
	if (fText) MemFree(fText);
	fText= StrDup(title);
	if (fText) fTextLen= StrLen(fText);
	else fTextLen= 0;
	this->GetPosition(r); // ViewRect
	width= curwidth= r.right - r.left;
	height= curheight= r.bottom - r.top;
	this->GetSize(width,height);
	if (width>curwidth || height>curheight) {
		r.right= r.left + width;
		r.bottom= r.top + height; 
		this->SetPosition(r);
		}
	
}

char* DNotePanel::GetTitle(char* title, ulong maxsize) 
{ 
	if (title==NULL) title= (char*)MemNew(maxsize);
	StrNCpy( title, fText, maxsize);
	return title;
}

void DNotePanel::GetSize(short& width, short& height)
{
	long	nchars, pixwid;
	char	*cp, *cp0;
	short defwidth= width, defheight= height, nlines= 0;
	Nlm_SelectFont(fFont);
	cp0= fText;
	width= 0;
	if (fText) do {
		nlines++;
		cp= Nlm_StrChr(cp0, NEWLINE);  // '\n'  
		if (cp) nchars= cp - cp0;
		else nchars= fTextLen - (cp0 - fText);
		pixwid= Nlm_TextWidth(cp0, nchars);
		
		if (defwidth && pixwid > defwidth) {
			nchars= Min(nchars, 1 + defwidth / Nlm_MaxCharWidth());
			pixwid= Nlm_TextWidth(cp0, nchars);
			while (nchars > 0 && pixwid > defwidth) {
        	nchars--;
        	pixwid = Nlm_TextWidth (cp0, nchars);
      		}
			}
			
		width= Max(width, pixwid);
		if (cp) {
			if (nchars < cp - cp0) cp0 += nchars; 
			else cp0 = 1 + cp;
			}
		else if (cp0 + nchars < fText + fTextLen) {
			cp0= cp0 + nchars;
			cp= cp0;
			}
	} while (cp);
	height= nlines * Nlm_LineHeight();
}


void DNotePanel::Draw()
{
	if (fText) {
		Nlm_RecT area;
		this->ViewRect( area);
		Nlm_SelectFont(fFont);
		Dgg_DrawTextbox( &area, fText, fTextLen, fJust, fDimmed); 
		}
}


void DNotePanel::SetDim(Boolean isDim, Boolean redraw)  
{
	fDimmed= isDim;
	if (redraw) this->Draw();
}		 

void DNotePanel::Enable()  
{
	fEnabled= true;
	DPanel::Enable();
}		

void DNotePanel::Disable()  
{
	fEnabled= false;
	DPanel::Disable();
}		


void DNotePanel::Click(Nlm_PoinT mouse)
{
	if (fEnabled) {
		Nlm_RecT area;
		this->ViewRect( area);
		Nlm_InvertRect( &area);
	}
}

void DNotePanel::Release(Nlm_PoinT mouse)
{
	if (fEnabled) {
		Nlm_RecT area;
		this->ViewRect( area);
		Nlm_InvertRect( &area);
		if (Nlm_PtInRect( mouse, &area)) {
			// then button is "selected", otherwise not.
			this->IsMyViewAction(this);
			}
		}
}

void DNotePanel::Invalid(short newval, short oldval)
{
}






//class DIconButton : public DPanel


extern "C" void iconDrawProc(Nlm_IcoN item)
{
	DIconButton *obj= (DIconButton*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Draw();
}

extern "C" void iconInvalProc(Nlm_IcoN item, Nlm_Int2 newval, Nlm_Int2 oldval)
{
	DIconButton *obj= (DIconButton*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Invalid(newval, oldval);
}

extern "C" void iconClickProc(Nlm_IcoN item, Nlm_PoinT mouse)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Click(mouse);
}

extern "C" void iconDragProc(Nlm_IcoN item, Nlm_PoinT mouse)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Drag(mouse);
}

extern "C" void iconHoldProc(Nlm_IcoN item, Nlm_PoinT mouse)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Hold(mouse);
}

extern "C" void iconReleaseProc(Nlm_IcoN item, Nlm_PoinT mouse)
{
	DPanel *obj= (DPanel*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Release(mouse);
}



// test this iconbutton frame !?
enum {
 kFInset= 2,
 kFInset2= 4,
 fFrame = 1
 };

DIconButton::DIconButton(long id, DView* itsSuperior, DIcon* itsIcon):
	DPanel( id, itsSuperior, 0, 0, subclasscreates),
	fEnabled(true),
	fIcon(itsIcon)
{
	short pixwidth, pixheight;
	GetIconSize( pixwidth, pixheight);
	fIconBut= Nlm_IconButton((Nlm_WindoW)itsSuperior->GetNlmObject(), pixwidth, pixheight, 
			iconDrawProc, iconInvalProc, iconClickProc, iconDragProc, iconHoldProc, iconReleaseProc);
	fPanel= (Nlm_PaneL) fIconBut;
	this->SetNlmObject(fIconBut);	
}

void DIconButton::GetIconSize(short& pixwidth, short& pixheight) 
{
	if (fIcon) {
		pixwidth= fIcon->fWidth;
		pixheight= fIcon->fHeight;
		}
	else {
		pixwidth= 16;
		pixheight= 16;
		}
	if (fFrame) {  
		pixwidth  += kFInset2;
		pixheight += kFInset2;
		}
}

void DIconButton::SetIcon(DIcon* itsIcon, Boolean redraw) 
{ 
	Nlm_RecT area;
	fIcon= itsIcon; 
	short pixwidth, pixheight;
	GetIconSize( pixwidth, pixheight);
	this->ViewRect(area);
	area.right = area.left + pixwidth;
	area.bottom= area.top + pixheight;
	this->RegisterRect( area);
	if (redraw) InvalRect( area);
}

void DIconButton::Enable()  
{
	if (!fEnabled) { 
		fEnabled= true;
		Invalidate();
		DPanel::Enable();
		}
}		

void DIconButton::Disable()  
{
	if (fEnabled) {
		fEnabled= false;
		Invalidate();
		DPanel::Disable();
		}
}		

void DIconButton::Draw()
{
	if (fIcon) {
		Nlm_RecT area;
		this->ViewRect( area);
			// gray is usable only on color/grayscale monitors
		if (!fEnabled) Nlm_LtGray(); 
		if (fFrame) {  
			//Nlm_WidePen(2);
		 	Nlm_FrameRoundRect( &area, 8, 8);
			Nlm_InsetRect( &area, kFInset, kFInset);
			}
		fIcon->Draw( area);
		if (!fEnabled) Nlm_Black();
		}
}


		// ?? should Invert() be a general DView method ??
void DIconButton::Click(Nlm_PoinT mouse)
{
	if (fEnabled) {
		Nlm_RecT area;
		this->ViewRect( area);
		if (fFrame) Nlm_InsetRect( &area, kFInset, kFInset);
		Nlm_InvertRect( &area);
		}
}

void DIconButton::Release(Nlm_PoinT mouse)
{
	if (fEnabled) {
		Nlm_RecT area;
		this->ViewRect( area);
		if (fFrame) Nlm_InsetRect( &area, kFInset, kFInset);
		Nlm_InvertRect( &area);
		if (Nlm_PtInRect( mouse, &area)) {
			// then button is "selected", otherwise not.
			this->IsMyViewAction(this);
			}
		}
}


void DIconButton::Invalid(short newval, short oldval)
{
}





//class DSlatePanel : public DPanel




//class DSlate : public DView

extern "C" void slateScrollProc(Nlm_BaR sb, Nlm_SlatE s, short newval, short oldval)
{
	DScrollBar *obj= (DScrollBar*) Nlm_GetObject( (Nlm_GraphiC)sb);
	DSlate *scrollee= (DSlate*) Nlm_GetObject( (Nlm_GraphiC)s);
	if (obj) obj->Scroll(scrollee, newval, oldval);
}

extern "C" void slateCharProc(Nlm_SlatE s, Nlm_Char c)
{
	DSlate *obj= (DSlate*) Nlm_GetObject( (Nlm_GraphiC)s);
	if (obj) obj->CharHandler(c);
}

	
DSlate::DSlate(long id, DView* itsSuperior, short width, short height, SlateKind kind) :
		DView( id, NULL, kSlate, itsSuperior) 
{
	switch (kind) {
		case scrolling:
			fSlate= Nlm_ScrollSlate((Nlm_WindoW)itsSuperior->GetNlmObject(), width, height); 
			break;
		case normal:
			fSlate= Nlm_NormalSlate((Nlm_WindoW)itsSuperior->GetNlmObject(), width, height); 
			break;
		case hidden:
			fSlate= Nlm_HiddenSlate((Nlm_WindoW)itsSuperior->GetNlmObject(), width, height); 
			break;
		case general:
			fSlate= Nlm_GeneralSlate((Nlm_WindoW)itsSuperior->GetNlmObject(), width, height, 
								slateScrollProc, 0/*extra*/, NULL/*classPtr*/); 
			break;
	
		}
	this->SetNlmObject(fSlate);	
}

void DSlate::VirtualSlate( short before, short after) 
{
	Nlm_VirtualSlate( fSlate, before, after, slateScrollProc);
}

void DSlate::SetCharHandler() 
{
	Nlm_SetSlateChar( fSlate, slateCharProc);
}

void DSlate::CharHandler(char c) 
{
}








//class DDisplay : public DView	

DDisplay::DDisplay(long id, DView* itsSuperior, short width, short height, DisplayKind kind):
	DView( id, NULL, kDisplay, itsSuperior) 
{
	switch (kind) {
		case normal:
			fDisplay= Nlm_NormalDisplay((Nlm_WindoW)itsSuperior->GetNlmObject(), width, height);
			break;
		case scrolling:
			fDisplay= Nlm_ScrollDisplay((Nlm_WindoW)itsSuperior->GetNlmObject(), width, height);
			break;
		}
	this->SetNlmObject(fDisplay);	
}
	
 



//class DDocPanel : public DPanel

#if 0
typedef CharPtr (*DocPrntProc)  PROTO((Pointer));
typedef void    (*DocClckProc)  PROTO((DoC, PoinT));
typedef void    (*DocDrawProc)  PROTO((DoC, RectPtr, Int2, Int2));
typedef Boolean (*DocShadeProc) PROTO((DoC, Int2, Int2, Int2));
typedef void    (*DocDataProc)  PROTO((DoC, Pointer));
typedef void    (*DocPanProc)   PROTO((DoC));
typedef void    (*DocFreeProc)  PROTO((DoC, VoidPtr));
#endif

 
DDocPanel::DDocPanel(long id, DView* itsSuperior, short pixwidth, short pixheight):
		DPanel(id, itsSuperior, pixwidth, pixheight, DPanel::subclasscreates) 
{
	fDocpanel= Nlm_DocumentPanel( (Nlm_WindoW)itsSuperior->GetNlmObject(), pixwidth, pixheight);
	fPanel= (Nlm_PaneL) fDocpanel;
	Nlm_SetPanelClick(fPanel, panelClickProc, panelDragProc, panelHoldProc, panelReleaseProc);
	this->SetNlmObject(fDocpanel);
	this->SetAutoAdjust(true); // used after append/insert/replace/delete

	DWindow* win= this->GetWindow();
	if (win) {
		win->fPrintHandler= this;
		win->fSaveHandler= this;
		}
	
	//Nlm_SetDocProcs(fDocpanel DocClckProc click, DocClckProc drag, DocClckProc release, DocPanProc pan);
	//Nlm_SetDocShade(fDocpanel, DocDrawProc draw, DocShadeProc gray, DocShadeProc invert, DocShadeProc color));
}
	
DDocPanel::~DDocPanel()
{
	//?? need for SetDocData's DocFreeProc call...
}
	
void 	DDocPanel::SetData(void* data)
{
	//Nlm_SetDocData(fDocpanel, data, DocFreeProc cleanup); 
}

char* DDocPanel::DataToChar(Pointer ptr)  // this is vibrant-document.h CharPrtProc()
{
  if (ptr != NULL) {
    return Nlm_StringSave ((char*)ptr);
  } else {
    return NULL;
  }
}

void 	DDocPanel::Append( Pointer data, short lines, Nlm_ParPtr parFormat, Nlm_ColPtr colFormat,
				Nlm_FonT font, Boolean docOwnsData)
{
	Nlm_AppendItem(fDocpanel, NULL/*data2char*/, data, docOwnsData, lines, parFormat, colFormat, font);
}


void 	DDocPanel::Replace( short item, Pointer data, short lines, Nlm_ParPtr parFormat, Nlm_ColPtr colFormat,
				Nlm_FonT font, Boolean docOwnsData)
{
	// need &DDocPanel::DataToChar in C form (but no object ref to call back !)
	//from: char *(DDocPanel::*Cpp func)(void *) -to-> char *(*C func)(void *)
	Nlm_ReplaceItem(fDocpanel, item, NULL/*data2char*/, data, docOwnsData, lines, parFormat, colFormat, font);
}


void 	DDocPanel::Insert( short item, Pointer data, short lines, Nlm_ParPtr parFormat, Nlm_ColPtr colFormat,
				Nlm_FonT font, Boolean docOwnsData)
{
	Nlm_InsertItem(fDocpanel, item, NULL/*data2char*/, data, docOwnsData, lines, parFormat, colFormat, font);
}






