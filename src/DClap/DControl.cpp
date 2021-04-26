// DControl.cp
// d.g.gilbert


#include "Dvibrant.h"
#include "DControl.h"
#include "DList.h"
#include "DUtil.h"




// class DControl  




// class DCluster : DControl

extern "C" void groupActionProc(Nlm_GrouP	item)
{
	DCluster *obj= (DCluster*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj)  obj->IsMyViewAction(obj);
}


// fFont and SetFont are static
Nlm_FonT DCluster::fFont = NULL;

void DCluster::SetFont( Nlm_FonT theFont)
{
	fFont= theFont;
}


DCluster::DCluster(long id, DView* itsSuperior, 
	short width, short height, Boolean hidden, char* title) :
	DControl( id, itsSuperior) 
{ 
	if (hidden)
		fGroup= Nlm_HiddenGroup( (Nlm_WindoW)itsSuperior->GetNlmObject(), width, height, 
			groupActionProc);
	else
		fGroup= Nlm_NormalGroup( (Nlm_WindoW)itsSuperior->GetNlmObject(), width, height, title, 
			fFont, groupActionProc);
	this->SetNlmObject(fGroup);	
}

DCluster::~DCluster()
{

}


//class DPrompt : public DControl

DPrompt::DPrompt(long id, DView* itsSuperior, char* title, short pixwidth, short pixheight, 
	Nlm_FonT font, Justify just) :
	DControl( id, itsSuperior) 
{
	fPrompt= Nlm_StaticPrompt((Nlm_WindoW)itsSuperior->GetNlmObject(), title, pixwidth, 
								pixheight, font, just);  
	this->SetNlmObject(fPrompt);	
}




	




//class DPopupList : public DView
	
extern "C" void PopupListHandler(Nlm_PopuP p)
{
	DPopupList *obj= (DPopupList*) Nlm_GetObject((Nlm_GraphiC)p);
	if (obj) obj->IsMyViewAction(obj);
}

DPopupList::DPopupList(long id, DView* itsSuperior, Boolean macLike) :
	DControl( id, itsSuperior) 
{
	fPopup= Nlm_PopupList((Nlm_GrouP)itsSuperior->GetNlmObject(), macLike, PopupListHandler);
	this->SetNlmObject(fPopup);
	//gViewCentral->RegisterView(id, fPopup, kMenu, this); << done by ::DView() 
	//fItemList= new DList(NULL,DList::kOwnObjects + DList::kDeleteObjects);
	fItemList= new DList(NULL);
}
 

DPopupList::~DPopupList()
{
	// dammit, DList( , kDeleteObjects) is NOT WORKING !! (not called obj destructors)
	short i, n= fItemList->GetSize();
	for (i=0; i<n; i++) {
		DString* st= (DString*) fItemList->At(i);
		delete st;
		}
	delete fItemList;
}

#if FIX_THIS_LATER
void  DPopupList::SetItemState(short item, Boolean enable) 
{
	DView* aview= (DView*)fPopupItemList->At(item);
	if (aview) vobj= aview->fNlmObject;
	if (enable) 
		Nlm_EnableChoiceItem ( vobj, false, false);
	else
		Nlm_DisableChoiceItem ( vobj, false, false);
}
#endif

void  DPopupList::AddItem(char* title) 
{
	DString* s= new DString(title);
	fItemList->InsertLast(s);
	Nlm_PopupItem(fPopup, title);
}
 
char* DPopupList::GetItemTitle( short item, char* title, ulong maxsize) 
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

char* DPopupList::GetSelectedItem( short& item, char* name, ulong namesize) 
{ 
	item= GetValue(); 
#if 1
	return this->GetItemTitle( item, name, namesize);
#else
  if (name && namesize) {
  	*name= 0;
  	if (item) {
  		DString* s= (DString*) fItemList->At(item-1);
  		StrNCpy(name, (char*)s->Get(), namesize);
  		}
  	}
#endif
  return name;
	// this fails for MOTIF ... urgh
	//return GetItemTitle(item, name, namesize); 
}




 

extern "C" void buttonActionProc(Nlm_ButtoN	item)
{
	DButton *obj= (DButton*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj)  obj->IsMyViewAction(obj);
}

//class DButton : public DControl

DButton::DButton(long id, DView* itsSuperior, char* title):
	DControl( id, itsSuperior) 
{
	fButton	= Nlm_PushButton( (Nlm_WindoW)itsSuperior->GetNlmObject(), title, buttonActionProc);
	this->SetNlmObject(fButton);	
}

 
//class DDefaultButton : public DButton

DDefaultButton::DDefaultButton(long id, DView* itsSuperior, char* title):
	DControl( id, itsSuperior) 
{
	fButton	= Nlm_DefaultButton((Nlm_WindoW)itsSuperior->GetNlmObject(), title, buttonActionProc);
	this->SetNlmObject(fButton);	
}


//class DCheckBox : public DButton

DCheckBox::DCheckBox(long id, DView* itsSuperior, char* title):
	DControl( id, itsSuperior) 
{
	fButton	= Nlm_CheckBox((Nlm_WindoW)itsSuperior->GetNlmObject(), title, buttonActionProc);
	this->SetNlmObject(fButton);	
}

//class DRadioButton : public DButton

DRadioButton::DRadioButton(long id, DView* itsSuperior, char* title):
	DControl( id, itsSuperior) 
{
	fButton	= Nlm_RadioButton((Nlm_WindoW)itsSuperior->GetNlmObject(), title);
	this->SetNlmObject(fButton);	
}





//class DListBox : public DControl


extern "C" void listboxActionProc(Nlm_LisT	item)
{
	DListBox *obj= (DListBox*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj)  obj->IsMyViewAction(obj);
}

DListBox::DListBox(long id, DView* itsSuperior, 
	short width, short height, Boolean multiselection):
	DControl( id, itsSuperior) 
{
	if (multiselection)
		fListBox= Nlm_MultiList((Nlm_WindoW)itsSuperior->GetNlmObject(), width, height, listboxActionProc);
	else
		fListBox= Nlm_SingleList((Nlm_WindoW)itsSuperior->GetNlmObject(), width, height, listboxActionProc);
	this->SetNlmObject(fListBox);	
}



//class DRepeatButton : public DControl

extern "C" void repeaterProc(Nlm_RepeaT item, Nlm_PoinT mouse)
{
	DRepeatButton *obj= (DRepeatButton*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->ClickAt(mouse);
}

DRepeatButton::DRepeatButton(long id, DView* itsSuperior, char* title):
	DControl( id, itsSuperior) 
{
	fRepeater= Nlm_RepeatButton((Nlm_WindoW)itsSuperior->GetNlmObject(), title, repeaterProc);
	this->SetNlmObject(fRepeater);	
}

void DRepeatButton::ClickAt( Nlm_PoinT mouse)
{

}




//class DScrollBar : public DControl

extern "C" void scrollbarProc(Nlm_BaR item, Nlm_GraphiC itsSlave, Nlm_Int2 newval, Nlm_Int2 oldval)
{
	DScrollBar *obj= (DScrollBar*) Nlm_GetObject( (Nlm_GraphiC)item);
	DView *scrollee= (DView*) Nlm_GetObject( itsSlave);
	// scrollee should == itsSuperior 
	if (obj)  obj->Scroll(scrollee,newval,oldval);
}

DScrollBar::DScrollBar(long id, DView* itsSuperior, short width, short height):
	DControl( id, itsSuperior)
{
	fScrollbar=  Nlm_ScrollBar((Nlm_WindoW)itsSuperior->GetNlmObject(), width, height, scrollbarProc);
	this->SetNlmObject(fScrollbar);	
}

void DScrollBar::Scroll(DView* scrollee, short newval, short oldval)
{
	
}





//class DSwitchBox : public DControl

extern "C" void switchProc(Nlm_SwitcH	item, short newval, short oldval)
{
	DSwitchBox *obj= (DSwitchBox*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj)  obj->Switch(newval, oldval);
}

DSwitchBox::DSwitchBox(long id, DView* itsSuperior, Boolean displayTextOfValue, Boolean vertical):
	DControl( id, itsSuperior)
{
	if (vertical)
		fSwitch= Nlm_UpDownSwitch((Nlm_WindoW)itsSuperior->GetNlmObject(), displayTextOfValue, switchProc);
	else
		fSwitch= Nlm_LeftRightSwitch((Nlm_WindoW)itsSuperior->GetNlmObject(), displayTextOfValue, switchProc);
	this->SetNlmObject(fSwitch);	
}

void DSwitchBox::Switch(short newval, short oldval)
{
	
}


