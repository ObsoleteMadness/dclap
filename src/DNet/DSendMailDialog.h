// DSendMailDialog.h

#ifndef _DSENDMAILDIALOG_
#define _DSENDMAILDIALOG_

#include <DWindow.h>
#include <DDialogText.h>

class	DSendMailDialog : public DWindow
{
public:
	enum fieldIDs { cTo, cFrom, cSubj, cCCopy, cCopySelf, cMsg };
	DView *fTo, *fFrom, *fSubj, *fCCopy, *fMsg;
	char*	fMailhost;
	
	DSendMailDialog(long id, DTaskMaster* itsSuperior,
		short width = -5, short height = -5, short left = -50, short top = -20, char* title = NULL);
	virtual ~DSendMailDialog();
	
	virtual DView* InstallTo(DView* super, char* toStr = "");
	virtual DView* InstallFrom(DView* super, char* fromStr = "your@email.address.here");
	virtual DView* InstallSubject(DView* super, char* subjStr = "");
	virtual DView* InstallCCopy(DView* super, char* ccopyStr = NULL);
	virtual DView* InstallMessage(DView* super, char* msgStr = NULL);
	virtual char*  BuildMessage();
	virtual void   SendMessage();
	virtual void   SetMailhost(char* hostname);
	
	virtual void Open(); //override
	virtual Boolean IsMyAction(DTaskMaster* action); // override
	virtual void SetDlogItems( DView* to, DView* from, DView* ccopy, DView* subj, DView* msg);
};


class	DMailSetupDialog : public DWindow
{
public:
	enum fieldIDs { cTo, cFrom };
	DView *fMailhost, *fUseraddress;
	static void GetDefaults();

	DMailSetupDialog(long id, DTaskMaster* itsSuperior,
		short width = -5, short height = -5, short left = -50, short top = -20, char* title = NULL);
	virtual ~DMailSetupDialog();
	
	virtual DView* InstallHost(DView* super);
	virtual DView* InstallAddress(DView* super);
	
	virtual void Open(); //override
	virtual Boolean IsMyAction(DTaskMaster* action); // override
	virtual void SetDlogItems( DView* host, DView* address);
};


extern char* gUseraddress;
extern char* gMailhost;

#endif
