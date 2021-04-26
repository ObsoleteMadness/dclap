// DViewCentral.h
// d.g.gilbert

#ifndef _DVIEWCENTRAL_
#define _DVIEWCENTRAL_

#include "DView.h"
#include "Dvibrant.h"

class DList;

// original use for DViewCentral class was to link nlmObjects to DViews.
// That need may be gone.  Other use:
// find any view given a view id -- is that useful to handle view tasks?
// still likely need this to use MenuItem views...
//

class DViewCentral : public DObject
{	
	DList*		fViewRegistry;  
public:
	DViewCentral();
	virtual ~DViewCentral();
	void RegisterView(DView* theView);
	void RegisterView(long id, Nlm_Handle nlmObject, short kind, DTaskMaster* itsSource);
	void RemoveView(long id);
	
	void DoViewMethod(long id, ViewMethod vmethod); 
	void EnableView(long id) 	{ this->DoViewMethod(id, &DView::Enable); }
	void DisableView(long id) { this->DoViewMethod(id, &DView::Disable); }
	void ShowView(long id) 		{ this->DoViewMethod(id, &DView::Show); }
	void HideView(long id) 		{ this->DoViewMethod(id, &DView::Hide); }
	void InvalidateView(long id) { this->DoViewMethod(id, &DView::Invalidate); }
	void SelectView(long id) 	{ this->DoViewMethod(id, &DView::Select); }
	void ResetView(long id) 	{ this->DoViewMethod(id, &DView::Reset); }
	
	DView* GetView(long id);
	DView* NlmItem2View(Nlm_Handle nlmObject);
	Nlm_Handle View2NlmItem(long id);
};


extern DViewCentral *gViewCentral;

#endif
