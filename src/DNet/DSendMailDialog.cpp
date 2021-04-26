// DSendMailDialog.cp
// d.gilbert


#include <ncbi.h>
#include <dgg.h>
#include <DControl.h>
#include <DTCP.h>
#include <DSMTPclient.h>
#include <DUtil.h>

#include "DSendMailDialog.h"

char* gUseraddress = NULL;
char* gMailhost = NULL;
Local Boolean	gCopySelf = false;
Local const char* kCopySelf = "Copy to self";


//#define	Inherited	DWindow

DSendMailDialog::DSendMailDialog(long id, DTaskMaster* itsSuperior,
		short width, short height, short left, short top, char* title) :
	DWindow( id, itsSuperior, fixed, width, height, left, top, title, kFreeOnClose)
{
	fTo = fFrom = fSubj = fCCopy= NULL; 
	fMsg =  NULL; 

	if (!gMailhost || !gUseraddress) {
		gMailhost= gPrefManager->GetPref("MAILHOST", "MAIL");
		gUseraddress= gPrefManager->GetPref("USERADDRESS", "MAIL");
		}
	fMailhost= StrDup(gMailhost);
}

DSendMailDialog::~DSendMailDialog() 
{
	if (fMailhost) MemFree(fMailhost);
}


void DSendMailDialog::SetDlogItems( DView* to, DView* from, DView* ccopy, 
						DView* subj, DView* msg) 
{
	fTo 	= to;
	fFrom = from;
	fSubj = subj;
	fCCopy= ccopy;  
	fMsg 	= msg;
}

void  DSendMailDialog::SetMailhost(char* hostname)
{
	if (fMailhost) MemFree(fMailhost);
	fMailhost= StrDup(hostname);
}


char* DSendMailDialog::BuildMessage()
{
	if (fMsg) return fMsg->GetText(); 
	else return NULL;
}

void DSendMailDialog::SendMessage()
{	
	char	*to = NULL, *from = NULL, *ccopy = NULL, *subj = NULL, 
				*msg = NULL, *mailhost = NULL;

	if (fTo) 		to	= fTo->GetText(); 
	if (fFrom) 	from= fFrom->GetText(); 
	if (fSubj) 	subj= fSubj->GetText(); 
	
	if (fCCopy) {
		ccopy= fCCopy->GetText(); 
		if (StrICmp(ccopy, kCopySelf) == 0) {
			ccopy= (char*) MemFree(ccopy);
			Boolean curcopy= fCCopy->GetStatus();
			if (curcopy != gCopySelf) {
				gCopySelf= curcopy;
				gPrefManager->SetPref( (gCopySelf) ? "1":"0" ,"COPYSELF", "MAIL");
				}
			if (gCopySelf) ccopy= StrDup(from);
			}
		}
		
	msg= BuildMessage();  // subclasses can build their own message to send
	
	if (fMailhost) mailhost= fMailhost;
	else if (from) {
		mailhost= StrChr( from, '@'); // a quick hack -- won't work frequently
		if (mailhost) ++mailhost;
		}
	if (mailhost) {
		DSMTP* smailer = new DSMTP(mailhost);
		smailer->SendMail( to, from, subj, ccopy, msg);
		delete smailer;
		}
	else 
		Message(MSG_OK, "Can't find mail host");
		
	MemFree( to);
	MemFree( from);
	MemFree( ccopy);
	MemFree( subj);
	MemFree( msg);
}


Boolean DSendMailDialog::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {
		case cSEND:
			gCursor->watch();
			this->SendMessage();
			// fall thru to next case
		case cCANC:
			this->CloseAndFree();
			gCursor->arrow();
			return true;
		default:
			return DWindow::IsMyAction(action);	
		}
}

DView* DSendMailDialog::InstallTo(DView* super, char* toStr)
{
	if (fTo)
		return fTo;
	else {
		DPrompt* pr= new DPrompt(0, super, "To:", 0, 0, Nlm_programFont);
		super->NextSubviewToRight();
	
		DDialogText* eto= new DEditText(cTo, super, toStr, 20);
		this->SetEditText(eto);
		super->NextSubviewBelowLeft();
		fTo= eto;
		return eto;
		}
}

DView* DSendMailDialog::InstallFrom(DView* super, char* fromStr)
{
	if (fFrom)
		return fFrom;
	else {
		DPrompt* pr= new DPrompt(0, super, "From:", 0, 0, Nlm_programFont);
		super->NextSubviewToRight();
		
		char* myaddr = (gUseraddress) ? gUseraddress : fromStr;
		DDialogText* efrom= new DEditText(cFrom, super, myaddr, 20);
		this->SetEditText(efrom);
		super->NextSubviewBelowLeft();
		fFrom= efrom;
		return efrom;
		}
}

DView* DSendMailDialog::InstallSubject(DView* super, char* subjStr)
{
	if (fSubj)
		return fSubj;
	else {
		DPrompt* pr= new DPrompt(0, super, "Subject:", 0, 0, Nlm_programFont);
		super->NextSubviewToRight();
	
		DDialogText* esubj= new DEditText(cSubj, super, subjStr, 20);
		this->SetEditText(esubj);
		super->NextSubviewBelowLeft();
		fSubj= esubj;
		return esubj;
		}
}

DView* DSendMailDialog::InstallCCopy(DView* super, char* ccopyStr)
{
	if (ccopyStr) {
		DPrompt* pr= new DPrompt(0, super, "Copy to:", 0, 0, Nlm_programFont);
		super->NextSubviewToRight();
		DDialogText* esubj= new DEditText(cCCopy, super, ccopyStr, 20);
		this->SetEditText(esubj);
		super->NextSubviewBelowLeft();
		return esubj;
		}
	else {
		DCheckBox* chk= new DCheckBox( cCopySelf, super, (char*)kCopySelf);
		chk->SetStatus(gCopySelf);
		super->NextSubviewBelowLeft();
		return chk;
		}
}


DView* DSendMailDialog::InstallMessage(DView* super, char* msgStr)
{
	if (fMsg)
		return fMsg;
	else {
		DDialogScrollText* emsg = new DDialogScrollText(cMsg, super,
												 		50, 6, Nlm_programFont, true/*kDoWrap*/);
		SetEditText(emsg); 
		if (msgStr) emsg->SetTitle(msgStr);
		fMsg= emsg;
		return emsg;
		}
}



void DSendMailDialog::Open()
{
	DView *eto, *efrom, *esubj, *eccopy, *emsg;

	eto  = this->InstallTo(this);
	efrom= this->InstallFrom(this);
	esubj= this->InstallSubject(this);
	eccopy= this->InstallCCopy(this);
	emsg= this->InstallMessage(this);
	SetDlogItems( eto, efrom, eccopy, esubj, emsg);
	AddOkayCancelButtons(cSEND,"Send");
	DWindow::Open();
}






// class DMailSetupDialog


DMailSetupDialog::DMailSetupDialog(long id, DTaskMaster* itsSuperior,
		short width, short height, short left, short top, char* title) :
	DWindow( id, itsSuperior, fixed, width, height, left, top, title, kFreeOnClose)
{
	fMailhost = fUseraddress= NULL; 
	GetDefaults();
}

DMailSetupDialog::~DMailSetupDialog() 
{
}


void DMailSetupDialog::GetDefaults() 
{
	if (!gMailhost || !gUseraddress) {
		gMailhost	  = gPrefManager->GetPref("MAILHOST", "MAIL",">> mail.sending.computer <<");
		gUseraddress= gPrefManager->GetPref("USERADDRESS", "MAIL", ">> YourAcct@Your.Mail.Host <<");
		char* copyself= gPrefManager->GetPref("COPYSELF", "MAIL","0");
		if (copyself) gCopySelf= (*copyself == '1');
		}
}

void DMailSetupDialog::SetDlogItems( DView* host, DView* address) 
{
	fMailhost 	= host;
	fUseraddress = address;
}

Boolean DMailSetupDialog::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {
		 
		case cOKAY:
			if (fMailhost)	{
				if (gMailhost) MemFree(gMailhost);
				gMailhost	= fMailhost->GetText(); 
				gPrefManager->SetPref( gMailhost, "MAILHOST", "MAIL");
				}
			if (fUseraddress) {
				if (gUseraddress) MemFree(gUseraddress);
				gUseraddress= fUseraddress->GetText(); 
				gPrefManager->SetPref( gUseraddress, "USERADDRESS", "MAIL");
				}
			// fall thru to next case
			
		case cCANC:
			this->CloseAndFree();
			return true;
			
		default:
			return DWindow::IsMyAction(action);	
		}
}

DView* DMailSetupDialog::InstallHost(DView* super)
{
	DPrompt* pr= new DPrompt(0, super, "Send mail host:", 0, 0, Nlm_programFont);
	super->NextSubviewToRight();

	char* myhost = (gMailhost) ? gMailhost : ">> some.computer.address <<";
	DDialogText* eto= new DEditText(cTo, super, myhost, 20);
	this->SetEditText(eto);
	super->NextSubviewBelowLeft();
	return eto;
}

DView* DMailSetupDialog::InstallAddress(DView* super)
{
	DPrompt* pr= new DPrompt(0, super, "Your mail address:", 0, 0, Nlm_programFont);
	super->NextSubviewToRight();
	
	char* myaddr = (gUseraddress) ? gUseraddress : ">> your@email.address.here <<";
	DDialogText* efrom= new DEditText(cFrom, super, myaddr, 20);
	this->SetEditText(efrom);
	super->NextSubviewBelowLeft();
	return efrom;
}



void DMailSetupDialog::Open()
{
	DView *address, *mailhost;
	mailhost= this->InstallHost(this);
	address = this->InstallAddress(this);
	SetDlogItems( mailhost, address);
	AddOkayCancelButtons();
	DWindow::Open();
}



