// UClipboard.h


#ifndef __UCLIPBOARD__
#define __UCLIPBOARD__

#include <DTaskMaster.h>

class DView;
class DWindow;
class DTask;

class DClipboardMgr : public DTaskMaster
{
public:
	enum { kIDClipView = 23847 };
	long	 		fPrefClipType;
	DView   * fClipView, * fClipOrphan;					 
	DWindow * fClipWindow;	 
	Boolean  	fGotClipType, fClipWrittenToSystem;	 

	DClipboardMgr();
	void IClipboardMgr();
	
	virtual Boolean DoMenuTask(long tasknum, DTask* theTask);
	virtual Boolean IsMyAction(DTaskMaster* action);
	
	virtual short PutToSystem(char* data, ulong datasize, long clipKind); // PutDeskScrapData();
	virtual void ReadFromSystem(); //ReadFromDeskScrap();
	virtual void CheckSystemClip(); // CheckDeskScrap();
	virtual long GetDataToPaste(char* data, ulong datalen, long& clipKind);
	virtual void LoadSystemClip();

	virtual void Launch();
	virtual void Close();
	virtual DWindow* MakeClipboardWindow();
	virtual DView* MakeViewForAlienClipboard();
	virtual void AboutToLoseControl(Boolean convertClipboard);
	virtual void CanPaste(long clipKind);
	virtual void DoSetupMenus();
	virtual void RegainControl(Boolean checkClipboard);
	virtual void SetClipView(DView* clipView);

private:
	void* fOldScrapStuff; 
	void* fNewScrapStuff;	 //ScrapStuff in macOs
};


extern DClipboardMgr* gClipboardMgr;
//extern void InitUClipboardMgr();

#endif


