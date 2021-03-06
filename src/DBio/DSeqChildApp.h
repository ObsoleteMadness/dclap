// DSeqChildApp.h
// d.g.gilbert

#ifndef _DSEQCHILDAPP_
#define _DSEQCHILDAPP_

#include <DObject.h>

class DList;
class DMenu;
class DSeqList;



class DSeqApps : public DObject
{	
public:
	enum { 
		kSeqApps = 360, 
		kChildMenu, kChildMenuBaseID
		};
	static char * kAppSection;
	
	//static DList* fgChildList;
	static void SetUpMenu( short menuId, DMenu*& aMenu, char* menuPrefs = kAppSection); 
	static void CallChildApp( short menucmd, DSeqList* aSeqList);

	DSeqApps();
	
};



#endif

