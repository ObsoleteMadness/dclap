// DView.cp
// d.g.gilbert


#include "DView.h"
#include "DViewCentral.h"

#include "DApplication.h"

//class DView : public DTaskMaster


DView::DView(long id, Nlm_Handle nlmObject, long kind, DTaskMaster* superior, 
	DList* subordinates) :
		DTaskMaster(id, superior, subordinates),
		fHresize(fixed),
		fVresize(fixed),
		fNlmObject(nlmObject),
		fKind(kind)
{
	this->Initialize();
}

DView::DView( long id, Nlm_Handle nlmObject, ResizeMethod Hresize, ResizeMethod Vresize,
				 long kind, DTaskMaster* superior, DList* subordinates) :
		DTaskMaster(id, superior, subordinates),
		fHresize(Hresize),
		fVresize(Vresize),
		fNlmObject(nlmObject),
		fKind(kind)
{
	this->Initialize();
}

void DView::Initialize()
{
	Nlm_LoadRect(&fViewrect, 0, 0, 0, 0);
	Nlm_LoadRect(&fSizerange, 0, 0, 32500, 32500);
	if (fId) gViewCentral->RegisterView(this);  // do this for all new views ?!
	this->SetNlmObject(fNlmObject);  
}

	
DView::~DView() 
{
		// !!!!!!!!!!!!!     FIXED 20 mar 94 w/ DeleteSubviews  !!!!!!!!!!!!!
			
		// we HAD a BIG memory leak here, as most of view methods
		// are written assuming that deleting a superview will auto-delete all subviews
		// BUT we never got around to fixing this, as we need to negotiate it w/ Vibrant
		// who also removes subviews at each call to Nlm_Remove(), but it doesn't remove
		// our Objects... just its own.  So whatever recursive call to RemoveAllSubviews
		// we need to fiddle w/ fNlmObject so Nlm_Remove() doesn't try to removed gone objects
		
		// *****************
 
 
	DeleteSubviews();
 
	 
	if (fNlmObject) {
		Nlm_SetObject((Nlm_GraphiC)fNlmObject, NULL); //????

		 // THIS IS A BIG HACK, but it may solve several of the messy crashes
		 // on Quit due to this messy vibrant-dclap interface
		 // (vibrant is insisting on drawing to non-existant views thru
		 //  its remove callbacks...)
		if (!gApplication->fDone) Nlm_Remove(fNlmObject); 
		}
	fNlmObject= NULL;
	gViewCentral->RemoveView(fId); // ?? this calls back here??
}



void DView::DeleteSubviews()
{
	/// tricky part: tell superior Nlm object to forget about its subordinates, so it
	/// doesn't try to delete them also !

	if (fSubordinates) {
		Nlm_Handle aNlmHand = NULL;
		Nlm_GraphiC aNlmChild= NULL;
		//if (fNlmObject) aNlmChild= Nlm_GetChild(fNlmObject);
		
		long i, n= fSubordinates->GetSize();
		for (i= 0; i<n; i++) {
			DView* subord= (DView*)fSubordinates->At(i);
			if (subord) {
				subord->DeleteSubviews(); 
				aNlmHand= Nlm_Parent(subord->fNlmObject);
				if (aNlmHand && aNlmHand == fNlmObject) 
					Nlm_SetChild((Nlm_GraphiC)fNlmObject, NULL);				
				delete subord;
				}
			}		
		fSubordinates->suicide(); //delete  ~DTaskMaster does this, but we need to do it now... ?
		fSubordinates= NULL;
		}
}


// from Vibrant vibincld.h // This should be public info !?
typedef  struct  Nlm_graphicrec {
  Nlm_GraphiC   next;
  Nlm_GraphiC   parent;
  Nlm_GraphiC   children;
  Nlm_GraphiC   lastChild;
  Nlm_GphPrcs   PNTR classptr;
  Nlm_ActnProc  action;
  Nlm_RecT      rect;
  Nlm_Boolean   enabled;
  Nlm_Boolean   visible;
  Nlm_VoidPtr   thisobject; /* <<<add to hold DObjectPtr dgg++ */
} Nlm_GraphicRec, Nlm_GraphicData, PNTR Nlm_GphPtr;

extern "C" void  Nlm_GetGraphicData PROTO((Nlm_GraphiC a, Nlm_GraphicData PNTR gdata));
extern "C" void  Nlm_SetGraphicData PROTO((Nlm_GraphiC a, Nlm_GraphicData PNTR gdata));
extern "C" void Nlm_SetNext PROTO((Nlm_GraphiC a, Nlm_GraphiC nxt));
extern "C" Nlm_GraphiC Nlm_GetNext PROTO((Nlm_GraphiC a));

void DView::RemoveSubview( DView* aSubview, Nlm_Boolean doDeleteIt)
{
	if (aSubview) {
		if (fSubordinates) {
  		Nlm_GraphicData  gdata;
  		Nlm_GraphiC theNlmview= (Nlm_GraphiC)aSubview->fNlmObject;
			Nlm_Handle aNlmHand= Nlm_Parent( aSubview->fNlmObject);
			if (aNlmHand && aNlmHand == fNlmObject) {
#if 1
				Nlm_GetGraphicData((Nlm_GraphiC)fNlmObject, &gdata);
				if (gdata.children) {
					if (gdata.lastChild && gdata.lastChild != gdata.children) {
			      Nlm_GraphiC last= NULL, next= NULL, p;
			      p = gdata.children;
			      while (p) {
			        next = Nlm_GetNext(p);
			      	if (p == theNlmview) { // remove p from chain
								if (last) Nlm_SetNext( last, next);
								else gdata.children= next;
								if (p == gdata.lastChild) gdata.lastChild= last;
			          }
			        else
			        	last = p;
			        p= next;
			      	}
						}
					else if (gdata.children == theNlmview)
						gdata.children= NULL; // only child == theNlmview !?
   			 	Nlm_SetGraphicData ((Nlm_GraphiC)fNlmObject, &gdata);
					}
#else
				//Nlm_GraphiC chld= Nlm_GetChild( (Nlm_GraphiC)fNlmObject);
					// ! this alone is bad if there is more than one subview for this !
				Nlm_SetChild( (Nlm_GraphiC)fNlmObject, NULL);	
#endif
				}			
	    fSubordinates->Delete( aSubview);
			}
		if (doDeleteIt) {
			aSubview->DeleteSubviews(); 
			delete aSubview;
			}
		}
}

Nlm_CharPtr DView::GetTitle(char* title, ulong maxsize) 
{ 
	if (title==NULL) title= (char*) MemNew(maxsize); //new char[maxsize];
	Nlm_GetTitle( fNlmObject, title, maxsize);
	return title;
}
	
void DView::SetNlmObject(Nlm_Handle nlmObject) 
{ 
	fNlmObject= nlmObject; 
	if (fNlmObject) {
		Nlm_SetObject((Nlm_GraphiC)fNlmObject, this);  
		Nlm_ObjectRect( fNlmObject, &fViewrect);  
		}
}		

void DView::SetSuperior(Nlm_Handle newSuper) 
{ 
	Nlm_SetParent( (Nlm_GraphiC)fNlmObject, (Nlm_GraphiC)newSuper); 
}

void DView::SetWindow( DWindow* newWindow)
{
	if (!newWindow) Dgg_SetParentWindow( (Nlm_GraphiC)GetNlmObject(), NULL);
  else Dgg_SetParentWindow( (Nlm_GraphiC)GetNlmObject(), 
											(Nlm_WindoW) ((DView*)newWindow)->GetNlmObject() );
}


void DView::InvalRect(Nlm_RecT& r) 
{
	Nlm_WindoW w= Nlm_SavePort( fNlmObject);
#ifdef WIN_MOTIF
	this->Select();
#endif
	Nlm_InvalRect( &r);
	// Nlm_RestorePort( w); //<<!! THIS CALLS UPDATER !!!!!!
	if (w) Nlm_UseWindow (w); 
}

void DView::Invalidate(void) 
{ 
	Nlm_WindoW w= Nlm_SavePort( fNlmObject);
#ifdef WIN_MOTIF
	this->Select();
#endif
	Nlm_InvalObject(fNlmObject);  // naughty vib -- Nlm_Inval doesn't check current port...
	// Nlm_RestorePort( w); //<<!! THIS CALLS UPDATER !!!!!!
	if (w) Nlm_UseWindow (w); 
}

void DView::SetTitle( char* title) 
{ 
	Nlm_SetTitle( fNlmObject, title); 
	Invalidate();
}

void DView::Show(void) 
{  
	Invalidate(); //?? need now that we pulled out vibwndws/Nlm_RestorePort/Update call
	Nlm_Update();
	Nlm_Show(fNlmObject); 
}

void DView::Hide(void) 
{  
	//Nlm_RecT  r;
	//ViewRect( r);
	//Nlm_EraseRect( &r);
	Nlm_Hide(fNlmObject); 
	//Nlm_Update();
}

void DView::SetSizerange(Nlm_RecT r) 
{
	fSizerange= r;
  ViewRect(fViewrect); // GetPosition(fViewrect);
	fViewrect.left  = Max( fSizerange.left, fViewrect.left);
	fViewrect.top   = Max( fSizerange.top, fViewrect.top);
	fViewrect.right = Min( fViewrect.right, fSizerange.right);
	fViewrect.bottom= Min( fViewrect.bottom, fSizerange.bottom);
  this->SetPosition(fViewrect);
}



	// !!?? Vibrant windows (& others) have no GetPosition callback & return 0 for position !!!????
	// FIX THIS thru DView -- have one sane function (getposition/viewrect), not two at
	// odds and incomplete -- use GetPosition when want to include scrollbar size
	// or revise this somehow
	
void DView::ViewRect(Nlm_RecT& r) 
{ 
	Nlm_ObjectRect(fNlmObject, &r);  
}

void DView::GetPosition(Nlm_RecT& r) 
{ 
	Nlm_GetPosition(fNlmObject, &r); 
	if (Nlm_EmptyRect( &r)) Nlm_ObjectRect(fNlmObject, &r);
	fViewrect= r; 
}

void DView::SetPosition(Nlm_RecT& r) 
{ 
	Nlm_RecT testr;
	Nlm_GetPosition(fNlmObject, &testr); 
	Boolean hasSetposition= (!Nlm_EmptyRect( &testr));
	
	if (hasSetposition) Nlm_SetPosition(fNlmObject, &r);
	else Nlm_SetRect((Nlm_GraphiC)fNlmObject, &r);
	fViewrect= r;
}


void DView::SizeToSuperview( DView* super, Boolean horiz, Boolean vert)
{
//#ifndef WIN_MSWIN
#ifdef WIN_MAC
	// the other win systems seem bothered by this...
	// if this is enabled in motif, motif fails to move scrollbars !
	
	Nlm_RecT r, myr; 
	// GetPosition includes size of scrollbars for slates, etc
	super->ViewRect(r); //super->GetPosition(r);  
	this->GetPosition(myr); //ViewRect(myr);   
	if (horiz) {
		myr.left = 0; 
		myr.right= myr.left + (r.right - r.left) +1; 
		}
	if (vert) {
		myr.top = 0;
		myr.bottom = myr.top + (r.bottom - r.top) +1;
		}
	SetPosition( myr);
#endif
}


void DView::ResizeSubviews(DView* superview, Nlm_PoinT sizechange)
{
	this->Resize(superview, sizechange);
	if (fSubordinates) {
		long i, n= fSubordinates->GetSize();
		for (i= 0; i<n; i++) {
			DView* subord= (DView*)fSubordinates->At(i);
			if (subord) subord->ResizeSubviews(this, sizechange); 
			}		
		}
}

void DView::Resize(DView* superview, Nlm_PoinT sizechange)
{
	Nlm_RecT	super;
	short	wid, hgt, oldw, oldh, top, oldt, left, oldl;
	
	GetPosition(fViewrect); // mac: this one is *required*, in DTableView w/ scrollbars
	//ViewRect(fViewrect); // mac: this one makes too small a resize for DTableView
	left= oldl= fViewrect.left;
	top= oldt= fViewrect.top;
	wid= oldw= fViewrect.right  - left;
	hgt= oldh= fViewrect.bottom - top;
	
	// want point & rect classes like MacApp that can iterate over to save us this dup?
	switch (fHresize) {
		case matchsuper: 
#ifdef WIN_MAC
	// problems w/ this w/ mswin, xwin -- need to fiddle to get it right
			if (superview) { 
				superview->ViewRect(super);	
				left = 0; 
				wid  = super.right - super.left + 1; 
				}
			else
				wid += sizechange.x;
			break;
#endif
		case relsuper	: wid += sizechange.x; break;
		case moveinsuper: left += sizechange.x; break;
		case calculate: wid = this->CalculateNewSize(false, wid, sizechange.x); break;
		case fixed	:
		default			: break;
		}
		
	switch (fVresize) {
		case matchsuper: 
#ifdef WIN_MAC
			if (superview) { 
				superview->ViewRect(super);	
				top = 0;
				hgt = super.bottom - super.top + 1;
				}
			else
				hgt += sizechange.y;
			break;
#endif
		case relsuper	: hgt += sizechange.y; break;
		case moveinsuper: top += sizechange.y; break;
		case calculate: hgt = this->CalculateNewSize(true, hgt, sizechange.y); break;
		case fixed		:
		default				: break;
		}
	
	if (oldw != wid || oldh != hgt || oldl != left || oldt != top) {
		fViewrect.left  = Max( fSizerange.left, left);
		fViewrect.top   = Max( fSizerange.top, top);
		fViewrect.right = Min( Max( fViewrect.left+1, fViewrect.left + wid), fSizerange.right);
		fViewrect.bottom= Min( Max( fViewrect.top+1, fViewrect.top + hgt), fSizerange.bottom);
		this->Select(); // motif requires !!
	  this->SetPosition(fViewrect);
	  
#if 0
//#ifdef WIN_MOTIF
	  // !! in Motif, nothing is drawn in resized area, and scrollbars, etc, disappear 
	  this->Select(); 
	  this->Hide();  
	  this->Show();  
#endif
	  }
}

