// MailHelp.cp
// d.g.gilbert -- simple email dialog to fixed help address
// Jan 1994


#include <DClap.h>
#include <DNet.h>


	// customize these strings for your local settings
Local char* kDefaultMailhost= "mailhost.some.university.edu";
Local char* kHelpAddress  = "help@some.university.edu";
Local char* kHelpMenuItem = "Mail to help desk...";
Local	char* aboutMailHelp =
	"Send mail to your department/school/company\n"
	"help desk for answers to your questions.";


class DMailHelp : public DApplication
{
public:
	enum internetMenu {
		kMailSetup = 1001, kBugMail, kSendMail
		};
		
	void IMailHelp(void);	 
	void DoAboutBox();
	virtual void SetUpMenus(void);	// override
	virtual	Boolean IsMyAction(DTaskMaster* action); //override
	virtual Boolean IsInternetMenu(DTaskMaster* action); 
	virtual	void UpdateMenus(void); // override
};


	// here we go -- the Nlm_Main() program (called from main() in ncbimain.c)
extern "C" short Main(void)
{
  DMailHelp* app = new DMailHelp();
  app->IMailHelp();
  app->Run();
  delete app; 
  return 0;
}




void DMailHelp::IMailHelp(void)
{
	IApplication();
	gMailhost= StrDup(kDefaultMailhost);
}	 

void DMailHelp::DoAboutBox()
{
	DAboutBoxWindow* about = new DAboutBoxWindow(aboutMailHelp); // make instance & it handles rest
}

void DMailHelp::SetUpMenus(void)
{
	DApplication::SetUpMenus();

	DMenu* iMenu = NewMenu(cMail, "Mail");
	iMenu->AddItem( kMailSetup, "Mail setup...");
	iMenu->AddItem( kBugMail, kHelpMenuItem);
	iMenu->AddItem( kSendMail, "Send mail...");
	iMenu->suicide(1);

}


class	DBugMailDialog : public DSendMailDialog
{
public:
	DBugMailDialog(long id, DTaskMaster* itsSuperior) :
		DSendMailDialog(id, itsSuperior, -5, -5, -50, -20, "Mail for Help")
		{}
	
	virtual DView* InstallTo(DView* super, char* toStr)  //override
	{
		return DSendMailDialog::InstallTo(super, kHelpAddress);
	}
};




Boolean DMailHelp::IsInternetMenu(DTaskMaster* action) 
{
	DWindow* win;

	switch(action->Id()) {

		case	kMailSetup:
			{
			win= new DMailSetupDialog(0, this, -10, -10, -20, -20, "Mail Setup");
			win->Open();
			return true;
			}
		case  kBugMail:
			{
			win= new DBugMailDialog( 0, this);
			win->Open();
			return true;
			}
		case 	kSendMail: 
			{
			win= new DSendMailDialog(0, this, -10, -10, -20, -20, "Send Mail");
			win->Open();
			return true;
			}
			
		default:	return false;
		}
}


Boolean DMailHelp::IsMyAction(DTaskMaster* action) 
{
	DWindow* win;
	
	if (action->fSuperior && action->fSuperior->Id() == cMail)
		return IsInternetMenu(action);
		
	switch(action->Id()) {
		default: 
			return DApplication::IsMyAction(action);
		}
}



void DMailHelp::UpdateMenus(void)
{
	DApplication::UpdateMenus();
}
