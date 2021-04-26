// DControl.h
// d.g.gilbert

#ifndef _DCONTROL_
#define _DCONTROL_

#include "Dvibrant.h"
#include "DView.h"

class DList;

class DControl : public DView
{	
public:
	DControl(long id, DView* itsSuperior) :
		DView( id, NULL, kControl, itsSuperior) 
		{}
};


class DPrompt : public DControl
{	
public:
	Nlm_PrompT	fPrompt;
	DPrompt(long id, DView* itsSuperior, char* title, short pixwidth = 0, short pixheight = 0, 
		Nlm_FonT font = Nlm_programFont, Justify just = justleft);
};


class DCluster : public DControl
{	
public:
	static Nlm_FonT	fFont;
	Nlm_GrouP fGroup;
	
	DCluster(long id, DView* itsSuperior, 
			short width, short height, Boolean hidden = false, char* title = NULL);
	virtual ~DCluster();
	static void SetFont(Nlm_FonT theFont);
	void SetMargins(short xMargin, short yMargin) {
		Nlm_SetGroupMargins( fGroup, xMargin, yMargin);
		}
	void SetSpacing(short xSpacing, short ySpacing) {
		Nlm_SetGroupSpacing( fGroup, xSpacing, ySpacing);
		}
};


class DButton : public DControl
{	
public:
	Nlm_ButtoN fButton;
	DButton(long id, DView* itsSuperior, char* title = NULL);
};
 
class DDefaultButton : public DControl
{	
public:
	Nlm_ButtoN fButton;
	DDefaultButton(long id, DView* itsSuperior, char* title = NULL);
};

class DCheckBox : public DControl
{	
public:
	Nlm_ButtoN fButton;
	DCheckBox(long id, DView* itsSuperior, char* title = NULL);
};

class DRadioButton : public DControl
{	
public:
	Nlm_ButtoN fButton;
	DRadioButton(long id, DView* itsSuperior, char* title = NULL);
};


class DPopupList : public DControl
{	
public:
	Nlm_PopuP	fPopup;
	DList*	fItemList;
	DPopupList(long id, DView* itsSuperior, Boolean macLike = true);
	~DPopupList();
	virtual void AddItem(char* title);
	virtual char* GetSelectedItem( short& item, char* name, ulong namesize);
	virtual char* GetItemTitle( short item, char* title = NULL, ulong maxsize = 256);

};



class DListBox : public DControl
{	
public:
	Nlm_LisT fListBox;
	DListBox(long id, DView* itsSuperior, short width, short height, 
			Boolean multiselection = false);

	void Append( char* title) { Nlm_ListItem( fListBox, title); }
	void StartAppending() { this->Disable(); }
	void DoneAppending()  { this->Enable(); }
};


class DRepeatButton : public DControl
{	
public:
	Nlm_RepeaT fRepeater;
	DRepeatButton(long id, DView* itsSuperior, char* title);
	virtual void ClickAt( Nlm_PoinT mouse);
};




class DScrollBar : public DControl
{	
public:
	Nlm_BaR fScrollbar;
	DScrollBar(long id, DView* itsSuperior, short width, short height);

	virtual void Scroll(DView* scrollee, short newval, short oldval);
	
	void SetValue(short val) { Nlm_CorrectBarValue( fScrollbar, val); }
	void SetMax(short max)   { Nlm_CorrectBarMax( fScrollbar, max); }
	void SetPage(short pgUp, short pgDn) { Nlm_CorrectBarPage(fScrollbar, pgUp, pgDn); }
};


class DSwitchBox : public DControl
{	
public:
	Nlm_SwitcH fSwitch;
	DSwitchBox(long id, DView* itsSuperior, Boolean displayTextOfValue = false, 
						Boolean vertical = true);
	virtual void Switch(short newval, short oldval);
	void SetMax(short max) { Nlm_SetSwitchMax( fSwitch, max); }
	short GetMax() { return Nlm_GetSwitchMax(fSwitch); }
	void SetValues(short value, short max) { Nlm_SetSwitchParams(fSwitch, value, max); }
};

#endif
