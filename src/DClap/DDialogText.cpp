// DDialogText.cp
// d.g.gilbert


#include "Dvibrant.h"
#include "DDialogText.h"
#include "DWindow.h"
#include "DApplication.h"


//class DDialogText : public DView

extern "C" void Nlm_SetDialogTextFont (Nlm_FonT theFont);
extern Nlm_FonT		   gDialogTextFont;
//extern Nlm_Boolean   gDialogTextMultiline;


DDialogText::DDialogText(long id, DView* itsSuperior, Nlm_FonT itsFont) :
		DView( id, NULL, kDialogText, itsSuperior),
		fText(NULL), fFont(itsFont)
{
	if (fFont) Nlm_SetDialogTextFont(fFont);   
}

DDialogText::~DDialogText()
{
	if (fText) Nlm_SetTextSelect(fText, NULL, NULL); // need for motif
}

char* DDialogText::GetText()
{
	long len = this->TextLength();
	if (!len) return NULL;
	char *txt = (char*) MemNew(++len);
	this->GetTitle( txt, len);
	txt[len-1]= 0;
	return txt;
}

void DDialogText::SetMultilineText( Nlm_Boolean turnon)
{
	//gDialogTextMultiline= turnon;
}

void DDialogText::selectAction()
{
	if (fFont) Nlm_SetDialogTextFont(fFont);   
	GetWindow()->SetEditText(this);
}

void DDialogText::deselectAction()
{
	GetWindow()->SetEditText(NULL);
}
	
	
Boolean DDialogText::IsMyAction(DTaskMaster* action) 
{
	switch(action->Id()) {
		case DApplication::kUndo:
			Message(MSG_OK,"DDialogText::Undo not ready.");
			return true;
		case DApplication::kCut:
			this->CutText(); 
			return true;
		case DApplication::kCopy:
			this->CopyText(); 
			return true;
		case DApplication::kPaste:
			this->PasteText(); 
			return true;
		case DApplication::kClear:
			this->ClearText(); 
			return true;
		case DApplication::kSelectAll:
			this->SetSelection(0,32000); 
			return true;
		default:
			return DView::IsMyAction(action);
		}
}


extern "C" void textActionProc(Nlm_TexT item)
{
	DDialogText *obj= (DDialogText*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->IsMyViewAction(obj);
}

extern "C" void textSelectProc(Nlm_TexT item)
{
	DDialogText *obj= (DDialogText*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->selectAction();
}

extern "C" void textDeselectProc(Nlm_TexT item)
{
	DDialogText *obj= (DDialogText*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->deselectAction();
}


		
//class DEditText : public DDialogText


DEditText::DEditText(long id, DView* itsSuperior, char* defaulttext, short charwidth, Nlm_FonT font) :
		DDialogText( id, itsSuperior, font) 
{
#ifdef WIN_MSWIN
	font= Nlm_systemFont; // some odd bug in mswin requires this font for edittext
	fFont= font;
#endif
	Nlm_SetDialogTextFont(font);  // must be prior to Nlm_DialogText() call -- should be a call param
	fText= Nlm_DialogText((Nlm_WindoW)itsSuperior->GetNlmObject(), defaulttext, charwidth, textActionProc);
	this->SetNlmObject(fText);	
	Nlm_SetTextSelect(fText, textSelectProc, textDeselectProc);
}



//class DHiddenText : public DDialogText

extern "C" void textTabProc(Nlm_TexT item)
{
	DHiddenText *obj= (DHiddenText*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->tabaction();
}

DHiddenText::DHiddenText(long id, DView* itsSuperior, char* defaulttext, short charwidth, Nlm_FonT font) :
		DDialogText( id, itsSuperior, font) 
{
	fText= Nlm_HiddenText((Nlm_WindoW)itsSuperior->GetNlmObject(), defaulttext, charwidth, textActionProc, textTabProc);
	this->SetNlmObject(fText);	
	Nlm_SetTextSelect(fText, textSelectProc, textDeselectProc);
}
		


//class DTextLine : public DDialogText

extern "C" Nlm_TexT Nlm_TextLine(Nlm_GrouP prnt, Nlm_CharPtr dfault,
                                Nlm_Int2 charWidth, Nlm_TxtActnProc actn,
                                Nlm_TxtActnProc tabProc);
                                
extern "C" void textlineTabProc(Nlm_TexT item)
{
	DTextLine *obj= (DTextLine*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->tabaction();
}

DTextLine::DTextLine(long id, DView* itsSuperior, char* defaulttext, short charwidth, Nlm_FonT font) :
		DDialogText( id, itsSuperior, font) 
{
	fText= Nlm_TextLine((Nlm_GrouP)itsSuperior->GetNlmObject(), defaulttext, charwidth, 
											textActionProc, textlineTabProc);
	this->SetNlmObject(fText);	
	Nlm_SetTextSelect(fText, textSelectProc, textDeselectProc);
}
		


//class DPasswordText : public DDialogText
 
DPasswordText::DPasswordText(long id, DView* itsSuperior, char* defaulttext, short charwidth, Nlm_FonT font) :
		DDialogText( id, itsSuperior, font) 
{
	fText= Nlm_PasswordText((Nlm_WindoW)itsSuperior->GetNlmObject(), defaulttext, charwidth, textActionProc);
	this->SetNlmObject(fText);	
	Nlm_SetTextSelect(fText, textSelectProc, textDeselectProc);
}



//class DDialogScrollText : public DDialogText

DDialogScrollText::DDialogScrollText(long id, DView* itsSuperior, short width, short height, Nlm_FonT font, Boolean wrap) :
		DDialogText( id, itsSuperior, font) 
{
	fText= Nlm_ScrollText((Nlm_WindoW)itsSuperior->GetNlmObject(), width, height, font, wrap, textActionProc);
	this->SetNlmObject(fText);	
	Nlm_SetTextSelect(fText, textSelectProc, textDeselectProc);
}



#if 0                             
//class DTextInScroller : public DDialogText
class DTextInScroller : public DDialogText
{	
public:
	DTextInScroller(long id, DView* itsSlateView, short width, short height, 
				Nlm_FonT font, Boolean wrap);  
	DTextInScroller(long id, DView* itsSuperior, short width, short height, 
				Nlm_FonT font, Boolean wrap, Nlm_BaR verticalsb, Nlm_BaR horizontalsb);
};

extern "C" Nlm_TexT Nlm_TextInScroller (Nlm_GrouP prnt, Nlm_Int2 width,
                                Nlm_Int2 height, Nlm_FonT font,
                                Nlm_Boolean wrap, Nlm_TxtActnProc actn,
                                Nlm_BaR verticalsb, Nlm_BaR horizontalsb);
                                
DTextInScroller::DTextInScroller(long id, DView* itsSuperior, short width, short height,
	 Nlm_FonT font, Boolean wrap, Nlm_BaR verticalsb, Nlm_BaR horizontalsb) :
		DDialogText( id, itsSuperior, font) 
{
	fText= Nlm_TextInScroller((Nlm_WindoW)itsSuperior->GetNlmObject(), width, height, font, wrap, textActionProc,
															verticalsb, horizontalsb);
	this->SetNlmObject(fText);	
	Nlm_SetTextSelect(fText, textSelectProc, textDeselectProc);
}

DTextInScroller::DTextInScroller(long id, DView* itsSlateView, short width, short height,
	 											Nlm_FonT font, Boolean wrap) :
	DDialogText( id, itsSlateView, font)
{
	Nlm_BaR verticalsb, horizontalsb;
	Nlm_SlatE theSlate= (Nlm_SlatE)itsSlateView->GetNlmObject();
	verticalsb  = Nlm_GetSlateVScrollBar(theSlate);
	horizontalsb= Nlm_GetSlateHScrollBar(theSlate);
	fText= Nlm_TextInScroller((Nlm_GrouP)theSlate, width, height, font, wrap, textActionProc,
													verticalsb, horizontalsb);
	this->SetNlmObject(fText);	
	Nlm_SetTextSelect(fText, textSelectProc, textDeselectProc);
}

#endif


