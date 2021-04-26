// DView.h
// d.g.gilbert

#ifndef _DVIEW_
#define _DVIEW_

#include "DTaskMaster.h"
#include "DViewTypes.h"
#include "DList.h"
#include "Dvibrant.h"


//generally used global constants, only ones used by Vibrant
enum Justify { justleft = 'l', justcenter = 'c', justright = 'r' }; 

// All views are some kind of Nlm Vibrant object, since we rely
// on the Vibrant library for all cross-platform user-interface code.
// DView class, and its subclasses, link Vibrant objects to this DClAp 
// object library.  A principle of this library is to hide all the 
// Vibrant C objects inside C++ objects.  Therefore all Nlm_ calls 
// dealing w/ any kind of graphics/user-event object are enclosed
// in a DView(subclass) method, and all Nlm_ handles (objects) are
// likewise enclosed in a DView object.


class DWindow;

class DView : public DTaskMaster
{	
public:
	enum	ResizeMethod	{ fixed, matchsuper, relsuper, moveinsuper, calculate };
	
	Nlm_Handle		fNlmObject;		 
	long					fKind;			
	Nlm_RecT			fViewrect;
	Nlm_RecT			fSizerange;
	ResizeMethod	fHresize;
	ResizeMethod	fVresize;
	
	DView( long id, Nlm_Handle nlmObject, long kind = 0, 
				 DTaskMaster* superior = NULL, DList* subordinates = NULL);
	DView( long id, Nlm_Handle nlmObject, ResizeMethod Hresize = fixed, ResizeMethod Vresize = fixed,
				 long kind = 0, DTaskMaster* superior = NULL, DList* subordinates = NULL);
	virtual ~DView(void);
	virtual void Initialize();
	
	virtual Boolean IsMyViewAction(DView* action) {
		return DTaskMaster::IsMyAction(action); // name-class change, remove IsMyViewAction?
		}
		
		// do we want these methods which are just name changes ?? 
	virtual DView* FindSubview(long id)  { return (DView*)FindSubordinate(id); } 
	virtual DView* FindSuperview(long id) { return (DView*)FindSuperior(id); }  
	
	Nlm_Handle GetNlmObject(void) { return fNlmObject; }		
	void SetNlmObject(Nlm_Handle nlmObject);
	virtual void DeleteSubviews();
	virtual void RemoveSubview( DView* aSubview, Nlm_Boolean doDeleteIt);

	virtual void Enable(void)  { Nlm_Enable(fNlmObject); }		
	virtual void Disable(void) { Nlm_Disable(fNlmObject); }
	virtual Boolean IsEnabled(void) { return Nlm_Enabled(fNlmObject); }
	virtual Boolean IsVisible(void) { return Nlm_Visible(fNlmObject); }
	virtual void Show(void);
	virtual void Hide(void);
	virtual void Invalidate(void);
	virtual void InvalRect(Nlm_RecT& r);

			// Select is used to bring windows to the front, to select a dialog text item, 
			// or to select a particular display object for use as a terminal display
	virtual void Select(void) { Nlm_Select(fNlmObject); }

	virtual void SetTitle( char* title);
	virtual Nlm_CharPtr GetTitle(char* title = NULL, ulong maxsize = 256);
	virtual	Nlm_CharPtr	GetText() { return GetTitle(); }

	virtual void  SetValue(short value) { Nlm_SetValue(fNlmObject, value); }
	virtual short GetValue(void) { return Nlm_GetValue(fNlmObject); }

	virtual void SetStatus( Boolean status) { Nlm_SetStatus(fNlmObject, status); }
	virtual Boolean GetStatus(void) { return Nlm_GetStatus(fNlmObject); }

		//SetOffset and GetOffset manipulate the scroll bar offsets for any object
	virtual void SetOffset(short horiz, short vert) { Nlm_SetOffset(fNlmObject, horiz, vert); }
	virtual void GetOffset(short& horiz, short& vert) { Nlm_GetOffset(fNlmObject, &horiz, &vert); }

	virtual void SetPosition(Nlm_RecT& r);
	virtual void GetPosition(Nlm_RecT& r);
			// this generally is same as GetPosition, but fewer proc calls - but may differ ?
	virtual void ViewRect(Nlm_RecT& r);

	virtual void ResizeSubviews(DView* superview, Nlm_PoinT sizechange);
	virtual void Resize(DView* superview, Nlm_PoinT sizechange);
			// ResizeMethod == calculate
	virtual short CalculateNewSize(Boolean vertaxis, short old, short delta) { return old+delta; } 
	void SetResize( ResizeMethod Hresize, ResizeMethod Vresize) { fHresize= Hresize; fVresize = Vresize; }
	void SetSizerange(Nlm_RecT r);
	virtual void SizeToSuperview( DView* super, Boolean horiz, Boolean vert);
			
		
		// clear the value or remove all children from a group.	
	virtual void Reset() { Nlm_Reset(fNlmObject); }
		
	virtual DTaskMaster* GetSuperior(void)  { return fSuperior; }
	virtual Nlm_Handle GetNlmSuperior(void) { return Nlm_Parent(fNlmObject); }
	virtual void SetSuperior(Nlm_Handle newSuper);

	virtual DWindow* GetWindow(void) { 
		return (DWindow*) Nlm_GetObject( (Nlm_GraphiC)Nlm_ParentWindow(fNlmObject));
		}
	virtual Nlm_WindoW GetNlmWindow(void) { return Nlm_ParentWindow(fNlmObject); }
	virtual void SetWindow( DWindow* newWindow);

	virtual Boolean AreAllSuperiorsEnabled() { return Nlm_AllParentsEnabled(fNlmObject); }
	virtual Boolean AreAllSuperiorsVisible() { return Nlm_AllParentsVisible(fNlmObject); }
		
		
		// Subview Positioning
	virtual void NextSubviewToRight(void)   { Nlm_Advance(fNlmObject); }
	virtual void NextSubviewBelowLeft(void) { Nlm_Break(fNlmObject); }

	virtual void SetNextPosition(Nlm_PoinT nps) { Nlm_SetNextPosition(fNlmObject, nps); }
	virtual void GetNextPosition(Nlm_PointPtr nps) {  Nlm_GetNextPosition(fNlmObject, nps); }


		// following used by only some subclasses of DView, but are put here for simplicity.
		// at this time, i don't know what kind of error results from misuse.
		
		//SetRange is used to set the page increment and maximum values for a scroll bar. 	
	virtual void SetRange(short pgUp, short pgDn, short max) {  
		Nlm_SetRange(fNlmObject, pgUp, pgDn, max);
		}

		// for groups, menus & such which are lists of items
	virtual short CountItems(void) { return Nlm_CountItems(fNlmObject); }

	virtual short GetNextItem(short prev) {  
		return Nlm_GetNextItem(fNlmObject, prev);
		}
		
	virtual void SetItemTitle(short item, char* title) {  
		Nlm_SetItemTitle(fNlmObject, item, title);
		}
	virtual char* GetItemTitle(short item, char* title = NULL, size_t maxsize = 256) {  
		if (title==NULL) title= new char[maxsize];
		Nlm_GetItemTitle(fNlmObject, item, title, maxsize);
		return title;
		}

	virtual void SetItemStatus(short item, Boolean status) {  
		Nlm_SetItemStatus(fNlmObject, item, status);
		}
	virtual Boolean GetItemStatus(short item) {  
		return Nlm_GetItemStatus(fNlmObject, item);
		}

};

typedef void (DView::*ViewMethod)(void); 	// used elsewhere


#endif
