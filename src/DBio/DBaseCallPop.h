// DBaseCallPop.h

#ifndef _DBASECALLPOP_
#define _DBASECALLPOP_

enum { 	kBaseCallPopup = 4958 };

class DBaseCallPop : public DPopupList {
public:
	Boolean fVisible;
	DAsmView* fAsmView;
	Nlm_RecT	fPoprect;
	Nlm_PoinT	fMouse;
	long			fRow, fCol;
	
	DBaseCallPop(long id, DAsmView* itsSuperior);
	virtual Boolean IsMyAction(DTaskMaster* action);
	virtual void Show();
	virtual void Show(Nlm_PoinT mouse);
	virtual void Hide();
	virtual void Hold(Nlm_PoinT mouse);
	virtual void Release(Nlm_PoinT mouse);
	virtual void LimitTrace(short item);
};

extern DBaseCallPop* gBaseCallPop;

#endif
