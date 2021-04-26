// DGopherAskDoc.h
// d.g.gilbert


#ifndef _DGOPHERASKDOC_
#define _DGOPHERASKDOC_


#include "DGoDoc.h"
#include <DWindow.h>
#include <DPanel.h>
#include <DTableView.h>
#include <Dvibrant.h>
#include <DGoList.h>
#include <DUtil.h>
#include <DControl.h>

class DGopher;
class DFile;
class DPrompt;
class	DMenu;

class DGopherAskDoc;

class DGopherAskShort : public DCluster
{
public:
	DList* fControls;
	DList* fAskers;
	DGopherAskDoc*	fDoc;
	
	DGopherAskShort(long id, DGopherAskDoc* itsSuperior, DList* itsAskers, DList* itsControls);
	virtual ~DGopherAskShort();
	virtual Boolean IsMyAction(DTaskMaster* action);
	virtual void InstallControls(short startControl, short& endControl);
};


class	DGopherAskDoc : public DWindow
{
public:
	enum gaTasks { kGoAskdoc = 250 };
	short							fViewchoice, fNumWindows, fIwindow, fListStart, fListEnd;
	DList						* fControls;
	DList						* fAskers;
	DGopher					* fGopher;
	DGopherListDoc	* fMainDoc;
	DGopherAskShort	* fAskShort;
	DFile						* fChosenFile;
	char						* fAskFile;
	DGopherAskDoc		* fNextW, * fPrevW;
	DButton					* fSendBut, * fNextBut, * fPrevBut;
	
	DGopherAskDoc(long id, DGopher* itsGopher, DGopherListDoc* itsMainDoc, short itsViewChoice);
	DGopherAskDoc(long id, DGopherAskDoc* prevWindow, short listStart);
	virtual ~DGopherAskDoc();
	
	virtual	void BuildWindows();
	virtual short	NumOfWindows();
	virtual char* Answers(DFile*& replyFile);
	virtual	void AddOkayCancelButtons();
	virtual void DoReply();
	virtual void OkayAction();
	virtual void Open();
	virtual void Close();
	virtual Boolean IsMyAction(DTaskMaster* action);
};

#endif
