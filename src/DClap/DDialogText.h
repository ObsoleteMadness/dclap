// DDialogText.h
// d.g.gilbert

#ifndef _DDIALOGTEXT_
#define _DDIALOGTEXT_

#include "DView.h"
#include "Dvibrant.h"


class DDialogText : public DView
{
public:
	Nlm_TexT	fText;
	Nlm_FonT	fFont;
	
	DDialogText(long id, DView* itsSuperior, Nlm_FonT itsFont = NULL);
	virtual ~DDialogText();
	
	virtual Boolean IsMyAction(DTaskMaster* action);  
	
	virtual char* GetText();
	virtual void SetText(char* theText) { DView::SetTitle(theText); }
	virtual void GetSelection( short& begin, short& end) { Nlm_TextSelectionRange( fText, &begin, &end); }
	virtual void SetSelection( short begin, short end) 	 { Nlm_SelectText( fText, begin, end); }
	virtual void CutText() 		{ Nlm_CutText( fText); }
	virtual void CopyText() 	{ Nlm_CopyText( fText); }
	virtual void PasteText() 	{ Nlm_PasteText( fText); }
	virtual void ClearText() 	{ Nlm_ClearText( fText); }
	virtual ulong TextLength() { return Nlm_TextLength( fText); }
	virtual void SetMultilineText( Nlm_Boolean turnon);
	
	virtual void selectAction();
	virtual void deselectAction();
};

DDialogText* CurrentDialogText();


class DEditText : public DDialogText
{	
public:
	DEditText(long id, DView* itsSuperior, char* defaulttext, short charwidth = 0, Nlm_FonT font = Nlm_systemFont);
};

class DHiddenText : public DDialogText
{	
public:
	DHiddenText(long id, DView* itsSuperior, char* defaulttext, short charwidth = 0, Nlm_FonT font = Nlm_systemFont);
	virtual void tabaction() {}
};

class DTextLine : public DDialogText
{	
public:
	DTextLine(long id, DView* itsSuperior, char* defaulttext, short charwidth = 0, Nlm_FonT font = Nlm_systemFont);
	virtual void tabaction() {}
};

class DPasswordText : public DDialogText
{	
public:
	DPasswordText(long id, DView* itsSuperior, char* defaulttext, short charwidth = 0, Nlm_FonT font = Nlm_systemFont);
};

class DDialogScrollText : public DDialogText
{	
public:
	DDialogScrollText(long id, DView* itsSuperior, short width, short height, 
		Nlm_FonT font, Boolean wrap);
};



inline DDialogText* CurrentDialogText() 
{
	return (DDialogText*) Nlm_GetObject( (Nlm_GraphiC)Nlm_CurrentText());
}
		

#endif
