// DMenu.h
// d.g.gilbert

#ifndef _DMENU_
#define _DMENU_

#include "Dvibrant.h"
#include "DView.h"


class DList;
class DMenuChoiceGroup;

class DMenu : public DView
{	
public:
	Nlm_MenU		fMenu;
	DList		* fItemList;
	DMenuChoiceGroup  * fFonts;
	enum menuIDs { cFontGroup = 100 };
	
	DMenu(long id, Nlm_MenU itsMenu, DTaskMaster* itsSuperior);
	virtual ~DMenu();

	virtual void  AddItem(long id, char* title, Boolean isCheckItem = false,
											Boolean hasCmdKeys = false);
	virtual void  AddSeparator(void);
	virtual void  AddFonts(void);			
	virtual char* GetFontChoice(char* namebuf = NULL, ulong bufsize = 256);
	virtual	void  SetFontChoice(char* name);

	virtual char* GetItemTitle(short item, char* title = NULL, ulong maxsize = 256);
		// note: "itemid" here is view ID for menuitem, not index into list of menu items
	virtual void SetItemStatus(short itemid, Boolean status);
	virtual Boolean GetItemStatus(short itemid);  
};


class DPulldownMenu : public DMenu
{	
public:
	DPulldownMenu(long id, DTaskMaster* itsSuperior, Nlm_WindoW w, char* title);
};

class DAppleMenu : public DMenu
{	
public:
	enum aboutMenuIDs { kAboutMenu = 110, kAboutMenuItem };
	DAppleMenu(DTaskMaster* itsSuperior, char* aboutName);
	virtual Boolean	IsMyAction(DTaskMaster* theView); // override
};

class DSubMenu : public DMenu
{	
public:
	DSubMenu(long id, DMenu* supermenu, char* title);
};

class DPopupMenu : public DMenu
{	
public:
	Nlm_PopuP fPopup;
	DPopupMenu(long id, Nlm_GrouP prnt, char* title);
	virtual void AddFonts();
	virtual void SetFontChoice(char* name);
	virtual char* GetFontChoice(char* namebuf, ulong bufsize);
	virtual void  AddItem(long id, char* title, Boolean isCheckItem = false,
											Boolean hasCmdKeys = false);
};



class DMenuChoiceGroup : public DView
{	
public:
	DMenu* fSupermenu;
	Nlm_ChoicE fChoice;
	Nlm_IteM fLastItem;
	DMenuChoiceGroup(long id, DMenu* supermenu);
	DMenuChoiceGroup(long id, DMenu* supermenu, Nlm_ChoicE itsChoice);
	virtual void AddItem(long id, char* title, Boolean hasCmdKeys = false);
};







#endif
