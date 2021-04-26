// DMenu.cp
// d.g.gilbert


#include "Dvibrant.h"
#include "DMenu.h"
#include "DViewCentral.h"
#include "DApplication.h"
#include "DUtil.h"



extern "C" char * kMenuCommandFlag; // from vibrant's vibmenus.c == "%$%"; 


//class DMenuChoiceGroup

extern "C" void ChoiceGroupHandler(Nlm_ChoicE c)
{
	DMenuChoiceGroup *obj= (DMenuChoiceGroup*) Nlm_GetObject( (Nlm_GraphiC)c);
	if (obj) obj->IsMyViewAction(obj);
}

DMenuChoiceGroup::DMenuChoiceGroup(long id, DMenu* supermenu) :
	DView( id, NULL, kMenu, supermenu),
	fLastItem(NULL)
{
	fChoice = Nlm_ChoiceGroup(supermenu->fMenu, ChoiceGroupHandler);
	fSupermenu= supermenu;
	this->SetNlmObject(fChoice);
}
    
DMenuChoiceGroup::DMenuChoiceGroup(long id, DMenu* supermenu, Nlm_ChoicE itsChoice) :
	DView( id, NULL, kMenu, supermenu),
	fChoice(itsChoice)
{
	fSupermenu= supermenu;
	this->SetNlmObject(fChoice);
}

void  DMenuChoiceGroup::AddItem( long id, char* title, Boolean hasCmdKeys) 
{
	if (hasCmdKeys) { 
		char stemp[256];
		StrCpy( stemp, kMenuCommandFlag);
		StrNCat( stemp, title, sizeof(stemp));
		fLastItem= Nlm_ChoiceItem(fChoice, stemp);
		}
	else
		fLastItem= Nlm_ChoiceItem(fChoice, title);
	//Nlm_Disable(item); // not til debug
	gViewCentral->RegisterView(id, fLastItem, kMenuItem, this); 
}



// class DMenu  



// vibrant callback handler

extern "C" void MenuItemHandler(Nlm_IteM	item)
{
	DMenu *obj= (DMenu*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj)  obj->IsMyViewAction(obj);
}


DMenu::DMenu(long id, Nlm_MenU itsMenu, DTaskMaster* itsSuperior) :
		DView(id, itsMenu, kMenu, itsSuperior),
		fFonts(NULL),
		fMenu(itsMenu) 
{
	//fItemList= new DList(NULL, DList::kDeleteObjects);
	fItemList= new DList(NULL);
}

DMenu::~DMenu()
{
	// !? ~DList is not calling obj destructors for kDeleteObjects !?!?
	short i, n= fItemList->GetSize();
	for (i=0; i<n; i++) {
		DString* st= (DString*) fItemList->At(i);
		delete st;
		}
	delete fItemList;
}


void  DMenu::AddItem(long id, char* title, Boolean isCheckItem, Boolean hasCmdKeys) 
{
	Nlm_IteM item;
	if (hasCmdKeys) { 
		char stemp[256];
		StrCpy( stemp, kMenuCommandFlag);
		StrNCat( stemp, title, sizeof(stemp));
		if (isCheckItem) 
			item= Nlm_StatusItem(fMenu, stemp, MenuItemHandler);
		else
			item= Nlm_CommandItem(fMenu, stemp, MenuItemHandler);
		}
	else {
		if (isCheckItem) 
			item= Nlm_StatusItem(fMenu, title, MenuItemHandler);
		else
			item= Nlm_CommandItem(fMenu, title, MenuItemHandler);
		}
		
	// need our own copy of titles as XMotif/Vibrant has no callback to get them !
	DString* s= new DString(title);
	fItemList->InsertLast(s);
			
			// SOMETIME, REVISE this method to return itemview as result !!!
			
	DView* itemview= new DView(id, item, kMenuItem, this);
	itemview->SetNlmObject(item);
	//Nlm_Disable(item); // not till we debug...
	if (itemview->GetOwnerCount()>1) itemview->suicide(); // leave gViewCentral as owner...
}


void DMenu::SetItemStatus(short itemid, Boolean status) 
{  
	DView* anItem;
	anItem= this->FindSubview(itemid);
	if (anItem) anItem->SetStatus(status);
}			

Boolean DMenu::GetItemStatus(short itemid) 
{  
	DView* anItem;
	anItem= this->FindSubview(itemid);
	if (anItem && anItem->GetStatus()) return true;
	else return false;
}			

#if 0			
		// ?? redo thise as above ??
	virtual void SetItemTitle(short item, char* title);	
	virtual char* GetItemTitle(short item, char* title = NULL, size_t maxsize = 256);
#endif



char* DMenu::GetItemTitle(short item, char* title, ulong maxsize)
{
	if (item>0) {
	  DString* s= (DString*) fItemList->At(item-1);
	  if (s) {
		char *cp= (char*) s->Get();
		if (title==NULL) {
		  maxsize= StrLen(cp) + 1;
		  title= (char*) MemNew(maxsize);
		  }
		StrNCpy(title, cp, maxsize);
		}
	}
	return title;
}

void  DMenu::AddSeparator(void) 
{
	DString* s= new DString("-");
	fItemList->InsertLast(s);
	Nlm_SeparatorItem(fMenu);
}


static Nlm_VGphIntChrSiz	gGetFontTitle = NULL;  // need special vibrant callback

void  DMenu::AddFonts(void) 
{
#ifdef WIN_MOTIF
	fFonts= new DMenuChoiceGroup(cFontGroup, this);
	fFonts->AddItem(cFontGroup+1,"Courier");
	fFonts->AddItem(cFontGroup+2,"Helvetica");
	fFonts->AddItem(cFontGroup+3,"Times");
#endif
#ifdef WIN_MSWIN
	fFonts= new DMenuChoiceGroup(cFontGroup, this);
	fFonts->AddItem(cFontGroup+1,"Arial");
	fFonts->AddItem(cFontGroup+2,"Courier");
	fFonts->AddItem(cFontGroup+3,"Times");
	fFonts->AddItem(cFontGroup+4,"Modern");
	fFonts->AddItem(cFontGroup+5,"Roman");
	fFonts->AddItem(cFontGroup+6,"Script");
	fFonts->AddItem(cFontGroup+7,"Serife");
#endif
#ifdef WIN_MAC
	Nlm_ChoicE theFonts= Nlm_FontGroup(fMenu);
	fFonts= new DMenuChoiceGroup(cFontGroup, this, theFonts);
#endif
	fFonts->AddItem(cFontGroup+20,"System");
	fFonts->AddItem(cFontGroup+21,"Program");
}

 
void  DMenu::SetFontChoice(char* name) 
{
	char  namebuf[256];
	short item, nitems;
	
	*namebuf= 0;
	if (name && fFonts) { //&& gGetFontTitle) {
		nitems= this->CountItems();
		for (item= 1; item<= nitems; item++) {
			Nlm_GetChoiceTitle((Nlm_GraphiC)fFonts->fNlmObject, item, namebuf, 256);
			if (StringCmp(name, namebuf)==0)  {
				fFonts->SetValue(item); // is this it?
				return;
				}
			}
		}
}
 

char*  DMenu::GetFontChoice(char* namebuf, ulong bufsize)
{
	if (namebuf && bufsize) *namebuf= 0;
	if (fFonts) { // && gGetFontTitle) {
		short item= fFonts->GetValue();  // this is correct
		if (!namebuf) {
 			bufsize= 256;
			namebuf= (char*)MemNew(bufsize);
			}
		Nlm_GetChoiceTitle((Nlm_GraphiC)fFonts->fNlmObject, item, namebuf, bufsize);
		return namebuf;  
		}
	else
		return NULL;
}


 



 
//class DPulldownMenu : public DMenu
 
DPulldownMenu::DPulldownMenu(long id, DTaskMaster* itsSuperior, Nlm_WindoW w, char* title):
	DMenu(id, Nlm_PulldownMenu(w, title), itsSuperior)
{ 
	//fMenu= Nlm_PulldownMenu(w, title);
	//this->SetNlmObject(fMenu);
} 

 
 
//class DAppleMenu : public DMenu

DAppleMenu::DAppleMenu(DTaskMaster* itsSuperior, char* aboutName):
	DMenu(kAboutMenu, Nlm_AppleMenu(NULL), itsSuperior)
{
	//fMenu= Nlm_AppleMenu(NULL);
	//this->SetNlmObject(fMenu);
	this->AddItem( kAboutMenuItem, aboutName);
	this->AddSeparator();
	Nlm_DeskAccGroup(fMenu);
}



Boolean DAppleMenu::IsMyAction(DTaskMaster* theView) // override
{
	if (theView->Id() == kAboutMenuItem) {
		gApplication->DoAboutBox();
		return true;
		}
	else
		return DView::IsMyAction(theView);
}


//class DSubMenu : public DMenu

DSubMenu::DSubMenu(long id, DMenu* supermenu, char* title) :
	DMenu(id, Nlm_SubMenu(supermenu->fMenu, title), supermenu) 
{
	//fMenu= Nlm_SubMenu(supermenu->fMenu, title);
	//this->SetNlmObject(fMenu);
}


//class DPopupMenu : public DMenu

//#ifdef WIN_MOTIF
#ifndef WIN_MAC


// this works for motif (and other form DOESN'T work for Motif...)
// but this fails for Mac (at least names of items are not displayed... fiddle w/ it)
// works now for mswin 

extern "C" void PopupMenuHandler(Nlm_PopuP p)
{
	DPopupMenu *obj= (DPopupMenu*) Nlm_GetObject((Nlm_GraphiC)p);
	if (obj) obj->IsMyViewAction(obj);
}


DPopupMenu::DPopupMenu(long id, Nlm_GrouP prnt, char* title) :
	DMenu(id, NULL, NULL)
{
		//fMenu= Nlm_PopupMenu(prnt, title);
		// try it w/ crazee popuplist...
		// really want to have a DPopupList class that includes all the DMenu methods...
	
	Nlm_RecT r;
	fPopup= Nlm_PopupList( prnt, true, PopupMenuHandler); // true==maclike
	fMenu= (Nlm_MenU) Nlm_Parent(fPopup);
	this->SetNlmObject(fMenu);
}

void  DPopupMenu::AddFonts()
{
	// strictly a hack -- we need to clean up this/DMenu font stuff
#ifdef WIN_MOTIF
	Nlm_PopupItem(fPopup, "Courier");
	Nlm_PopupItem(fPopup, "Helvetica");
	Nlm_PopupItem(fPopup, "Times");
	Nlm_PopupItem(fPopup, "Program");
	Nlm_PopupItem(fPopup, "System");
#endif
#ifdef WIN_MSWIN
	Nlm_PopupItem(fPopup, "Arial");
	Nlm_PopupItem(fPopup, "Courier");
	Nlm_PopupItem(fPopup, "Modern");
	Nlm_PopupItem(fPopup, "Roman");
	Nlm_PopupItem(fPopup, "Times");
	Nlm_PopupItem(fPopup, "Script");
	Nlm_PopupItem(fPopup, "Serife");
	Nlm_PopupItem(fPopup, "Program");
	Nlm_PopupItem(fPopup, "System");
#endif
#ifdef WIN_MAC
	Nlm_IteM item;
	Nlm_ChoicE theFonts= Nlm_FontGroup(fMenu);
	item= Nlm_ChoiceItem(theFonts, "Program");
	item= Nlm_ChoiceItem(theFonts, "System");
	fFonts= new DMenuChoiceGroup(cFontGroup, this, theFonts);
#endif
}

void  DPopupMenu::AddItem(long id, char* title, Boolean isCheckItem, Boolean hasCmdKeys)
{
	Nlm_IteM item;
	Nlm_PopupItem(fPopup, title);

	// need our own copy of titles as XMotif/Vibrant has no callback to get them !
	DString* s= new DString(title);
	fItemList->InsertLast(s);

			// SOMETIME, REVISE this method to return itemview as result !!!
	//DView* itemview= new DView(id, item, kMenuItem, this);
	//itemview->SetNlmObject(item);
	//if (itemview->GetOwnerCount()>1) itemview->suicide(); // leave gViewCentral as owner...
}


void  DPopupMenu::SetFontChoice(char* name)
{
	char  namebuf[256];
	short item, nitems;

	*namebuf= 0;
	if (name) {
		nitems= this->CountItems();
		for (item= 1; item<= nitems; item++) {
			Nlm_GetItemTitle( fPopup, item, namebuf, 256);
			if (StringCmp(name, namebuf)==0)  {
				Nlm_SetValue(fPopup, item);
				return;
				}
			}
		}
}

char*  DPopupMenu::GetFontChoice(char* namebuf, ulong bufsize)
{
	if (namebuf && bufsize) *namebuf= 0;
	if (fPopup) {
		short item= Nlm_GetValue(fPopup);
		if (!namebuf) {
			bufsize= 256;
			namebuf= (char*)MemNew(bufsize);
			}
		Nlm_GetItemTitle( fPopup, item, namebuf, bufsize);
		return namebuf;
		}
	else
		return NULL;
}

#else

	// this stupid old Nlm_PopupMenu() has no callback to GetItemTitle() for items !!
DPopupMenu::DPopupMenu(long id, Nlm_GrouP prnt, char* title) :
	DMenu(id, Nlm_PopupMenu(prnt, title), NULL)
{
	//fMenu= Nlm_PopupMenu(prnt, title);
	//this->SetNlmObject(fMenu);
}

void  DPopupMenu::AddFonts()
{
	DMenu::AddFonts();
}

void  DPopupMenu::AddItem(long id, char* title, Boolean isCheckItem, Boolean hasCmdKeys)
{
	DMenu::AddItem(id,title,isCheckItem,hasCmdKeys);
}

void  DPopupMenu::SetFontChoice(char* name)
{
	DMenu::SetFontChoice(name);
}

char*  DPopupMenu::GetFontChoice(char* namebuf, ulong bufsize)
{
	return DMenu::GetFontChoice(namebuf, bufsize);
}

#endif

