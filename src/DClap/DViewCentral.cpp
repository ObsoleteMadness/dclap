// DViewCentral.cp
// d.g.gilbert



#include "DViewCentral.h"
#include "DList.h"
#include "Dvibrant.h"


DViewCentral *gViewCentral = NULL;

// original use for DViewCentral class was to link nlmObjects to DViews
// that need may be gone.  Other use:
// find any view given a view id -- is that useful to handle view tasks?
// still likely need this to use MenuItem views...
//

//class DViewCentral : public DObject

DViewCentral::DViewCentral() 
{
	fViewRegistry= new DList();
}


DViewCentral::~DViewCentral() 
{
	fViewRegistry->FreeAllObjects();  
	fViewRegistry->suicide();   // delete  
}
		
void DViewCentral::RegisterView(DView* theView) 
{
		// 20Jan94 -- looks like we only use gViewCentral for kMenu and kMenuItem kinds !!!
		// make this a bit saner by screening out all others?? 
		// rename viewcentral to menucentral??
	if (theView && theView->Id() 
	 && (theView->fKind == kMenu || theView->fKind == kMenuItem)) {
		theView->newOwner(); // so we can suicide it and caller can suicide it;
		fViewRegistry->InsertLast(theView);
		}
}

void DViewCentral::RegisterView(long id, Nlm_Handle nlmObject, short kind, DTaskMaster* itsSource) 
{
	if (id) {
	  DView* aview= new DView(id, nlmObject, kind, itsSource);
	  if (aview->GetOwnerCount() > 1) aview->suicide(); // new & RegisterView() both bumped counter
	  //fViewRegistry->InsertLast(); << !! new DView() calls this->RegisterView(newview)
	  } 
}
	
void DViewCentral::RemoveView(long id) 
{
	if (id) {
		long i, n= fViewRegistry->GetSize();
		for (i= n-1; i>= 0; i--) {
			DView* aview= (DView*)fViewRegistry->At(i);
			if (aview->Id() == id) {
				fViewRegistry->AtDelete(i);
				aview->suicide();
				break;
				}
			}		
		}
}

DView* DViewCentral::GetView(long id) 
{
	if (id) {
		long i, n= fViewRegistry->GetSize();
		for (i= 0; i<n; i++) {
			DView* aview= (DView*)fViewRegistry->At(i);
			if (aview->Id() == id) return aview;
			}		
		}
	return NULL;
}



void DViewCentral::DoViewMethod(long id, ViewMethod vmethod) 
{
	if (id) {
		long i, n= fViewRegistry->GetSize();
		for (i= 0; i<n; i++) {
			DView* aview= (DView*)fViewRegistry->At(i);
			if (aview && aview->Id() == id) {
				(aview->*vmethod)();
				return;
				}
			}		
		}
}


DView*  DViewCentral::NlmItem2View(Nlm_Handle nlmObject)  
{
	long i, n= fViewRegistry->GetSize();
	for (i= 0; i<n; i++) {
		DView* aview= (DView*)fViewRegistry->At(i);
		if (aview->fNlmObject == nlmObject) return aview;
		}		
	return NULL;
}

Nlm_Handle DViewCentral::View2NlmItem(long id)  
{
	if (id) {
	long i, n= fViewRegistry->GetSize();
	for (i= 0; i<n; i++) {
		DView* aview= (DView*)fViewRegistry->At(i);
		if (aview->Id() == id) return aview->fNlmObject;
		}		
		}
	return NULL;
}
 
 