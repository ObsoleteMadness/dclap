// DBOPclient.cpp
// BOP tcp client
// by D. Gilbert, May 1996
 
 
#include <ncbi.h>
#include <dgg.h>
#include "DList.h"
#include <DControl.h>
#include <DTCP.h>
#include <DWindow.h>
#include <DDialogText.h>
#include <DPanel.h>
#include <DUtil.h>
#include <DChildApp.h>
#include <DTaskCentral.h>
#include <DApplication.h>

#include "DBOPclient.h"

#define TRIALBOPNOTE

const short	kBOPport = 7110;
Local char* gBopHostMarker = "bophost";

enum bopperstates {  
	unknown,
	procnew,
	procexec,
	procdone,
	prockill
	};


class DMsg : public DObject {
public:
	long fNum, fPid, fState;
	char* fName;
	DMsg(): fNum(0), fPid(0), fState(0), fName(NULL) {}
	DMsg( long num, long pid, long state, char* name) : 
		fNum(num), fPid(pid), fState(state)
		{ fName= StrDup(name); }
	~DMsg()  { MemFree(fName); } 
};



// need to call GetDefaults() from some inits.

class	DBopSetupDialog : public DWindow
{
public:
	DView *fUsertext, *fPasstext, *fPorttext, *fHosttext, *fSavePass;
	char * fHostname;
	//static char * fgUser, * fgPass;
	static void GetDefaults();

	DBopSetupDialog(long id, DTaskMaster* itsSuperior, char* hostname,
		short width = -5, short height = -5, short left = -50, short top = -20, 
		char* title = NULL);
	virtual ~DBopSetupDialog();
	
	virtual DView* InstallUsername(DView* super);
	virtual DView* InstallPassword(DView* super);
	
	virtual void Open(); 
	virtual void 		OkayAction(); 	// override to handle 'OKAY' message
	virtual Boolean IsMyAction(DTaskMaster* action);  
	virtual void SetDlogItems( DView* host, DView* address);
  virtual Nlm_Boolean PoseModally();
	
};

//char* DBopSetupDialog::fgUser = NULL;
//char* DBopSetupDialog::fgPass = NULL;
 

/*
//usage
			win= new DBopSetupDialog(0, this, hostname, -10, -10, -20, -20, "BOP Logon");
			if (win) win->Open();
*/


// DBOP .............................

char* DBOP::gUsername = NULL;
char* DBOP::gPassword = NULL;
char* DBOP::gHost = NULL;
long  DBOP::gPort = kBOPport;

DBOP::DBOP()
{
	InitData();
}

DBOP::DBOP(char *username, char *password, char *hostname, long port)
{
	InitData();
	ResetHost( username, password, hostname, port);
}

void DBOP::InitData()
{
  char  buf[256];
	fHostname = NULL;
	fUsername= NULL;
	fPassword= NULL;
	fMsgCount= 0;
	fPort= kBOPport;
	fState= kClosed;
	fHandshake= NULL;
	fProcessNum= 0;
	fCurrentMsg= 0;
	fDone= 0;
	fMsgList= new DList(); // NULL, DList::kDeleteObjects << doesnt' call destructor!
	sprintf( buf, "DClap-BOP %s", (char*)VersionString());
	fId= StrDup(buf);
}

DBOP::~DBOP()
{
	MemFree(fHostname);
	MemFree(fUsername);
	MemFree(fPassword);
	MemFree(fHandshake);
	MemFree(fId);
  if (fMsgList) {
		short i, n = fMsgList->GetSize();
		for (i=0; i<n; i++) {
			DMsg* m= (DMsg*) fMsgList->At(i);
			delete m;
			}
  	delete fMsgList;
  	}
}



void DBOP::ResetHost(char *username, char *password, char *hostname, long port)
{
	Boolean doreset= false;
	
	if (hostname && (!fHostname || StrCmp(fHostname, hostname)!=0)) {
		MemFree( fHostname);
		if (StrICmp(hostname, gBopHostMarker)==0) 
			fHostname= NULL; // pick it up thru GetLogon()
		else
			fHostname= StrDup( hostname);
		doreset= true;
		}
	if (username && (!fUsername || StrCmp(fUsername, username)!=0)) {
		MemFree( fUsername);
		fUsername= StrDup( username);
		doreset= true;
		}
	if (password && (!fPassword || StrCmp(fPassword, password)!=0)) {
		MemFree( fPassword);
		fPassword= StrDup( password);
		doreset= true;
		}
	if (port && port != fPort) {
		fPort= port;
		doreset= true;
		}
	
	if (doreset) {
		if (fState > kClosed) { Close(); Release(); }
		fState= kClosed;
		}
		
	//Connect(); //??
	if (!fHostname || !*fHostname 
		||!fUsername || !*fUsername 
		||!fPassword) GetLogon();
}



Boolean DBOP::WaitHandshake( char OkayCode, Boolean showfail)
{
	MemFree(fHandshake);
	fHandshake= RecvLine(); 
	if (!fHandshake) 
		return false;
	else if (*fHandshake != OkayCode) {
		if (showfail) Fail( fHandshake); 
		if (*fHandshake == '-' && StrStr(fHandshake,"signing off")!=NULL) {
			fState= kClosed;
			Close(); Release();
			}
		return false;
		}
	else {
		if (*fHandshake == '+' && StrStr(fHandshake,"signing off")!=NULL) {
			Close(); Release();
			fState= kClosed;
			}
		return true;
		}
}


short DBOP::Quit()
{
 	if (Status() == kTCPestablished) SendStr("QUIT"CRLF, kDontAddCRLF);
	Close(); Release();
	fState= kClosed;
	return fState;
}


short DBOP::GetLogon()
{
	Boolean okay= false;
	DBopSetupDialog* win= 
		new DBopSetupDialog(0, NULL, fHostname, -10, -10, -20, -20, "BOP Logon");
	if (win && win->PoseModally()) {
		MemFree(fUsername); fUsername= StrDup( gUsername);  
		MemFree(fPassword); fPassword= StrDup( gPassword); 
		MemFree(fHostname); fHostname= StrDup( gHost); 
		fPort= gPort;
		okay= true;
		}
#ifdef WIN_MAC
  delete win;// already deleted in MSWIN !?  & Unix !!
#endif
	return okay;
}


short DBOP::Connect()
{
  char	buf[256];

	if (Status() == kTCPestablished && CharsAvailable()) {
		// ???
		}
		
 	if (Status() != kTCPestablished) {
  		// connect to host
	  if (fHostname == NULL) return(kClosed); 
		Open(fHostname, fPort);
		if (Failed()) return(kClosed); 
		if (!WaitedForOpen())  return(kClosed);  
		if (!WaitHandshake('+')) return(Quit());
		fState= kConnected;
		}
		
  if (fState < kTransaction) {
  		// log on as user
		Boolean retry= true;
		if (!fHostname || !*fHostname || 
			!fUsername || !*fUsername || !fPassword) GetLogon();
logon:
		sprintf( buf, "USER %s"CRLF,fUsername);
		SendStr( buf, kDontAddCRLF);
		if (!WaitHandshake('+'))  return(Quit());
	
		sprintf( buf, "PASS %s"CRLF,fPassword);
		SendStr( buf, kDontAddCRLF);
		if (!WaitHandshake('+')) {
			if (retry && GetLogon()) goto logon;
			return(Quit());
			}
	
				// get list of existing processes
		SendStr( "LIST"CRLF, kDontAddCRLF);
		if (WaitHandshake('+',false)) {
			long number, pid, state;
			char name[256];
			char* data;
			Boolean done= false;
			while (!done) {
				data= RecvLine();
		  	if (data) {	
			  	done= (*data == '.');
			  	if (!done) {
			  		number= pid= state= 0; *name= 0;
			  		sscanf(data,"%u %s %u %u", &number, &pid, &state, name);
			  		DMsg* msg= new DMsg(number, pid, state, name);
			  		fMsgList->InsertLast(msg);
			  		fMsgCount++;
						}
					MemFree(data);
					}
				else done= true;
				}
			}

		fState= kTransaction;
		}
	return(fState); 
}
  
  
short DBOP::CloseMsg()
{
	if (fState == kMsgOpen) {
		SendStr("CLOS"CRLF, kDontAddCRLF);
		if (!WaitHandshake('+')) return(Quit());	
		fState= kTransaction;
		}
	return fState;
}


short DBOP::CheckStatus( long processNum)
{
	short msgNum= MsgFromProcnum( processNum);
	return CheckMsgStatus( msgNum);
}

short DBOP::CheckMsgStatus( short msgNum)
{
  char	buf[256];

	fDone= 0; // ?? or save this b/n calls ? - but not same for each msg !
	if (!msgNum) return 0;
	for (short itest=0; itest<2; itest++) {
		if (Connect() < kTransaction) return(fState);
		sprintf( buf, "STAT %d"CRLF, msgNum);
		SendStr( buf, kDontAddCRLF);
		if (WaitHandshake('+')) 
			break; // done with Connect test
		else {
			if (fHandshake == NULL) { 
				Close(); Release();
				fState= kClosed; // try to Connect again
				}
	 		else {
	 			fDone= kProcDone+1; //!?!?! what do we want here
				return(fDone); // don't quit?
				}
			}
		}
		
	int num, state = 0;
	*buf= 0;
	sscanf( fHandshake, "+OK %s", buf);
	if (StrNICmp(buf,"done",4)==0) { fDone= kProcDone; fState= kProcDone; }
	
	return(fDone);
}


short DBOP::OpenMsg( short msgNum)
{
  char	buf[256];

	if (!msgNum) return 0;
	if (Connect() < kTransaction) return(fState);
	if (fState == kMsgOpen) 
		if (CloseMsg() != kTransaction) return(Quit());

	sprintf( buf, "OPEN %d"CRLF, msgNum);
	SendStr( buf, kDontAddCRLF);
	if (!WaitHandshake('+')) return(fState); // don't quit?
	
	fState= kMsgOpen;
	fCurrentMsg= msgNum;
	return fState;	
}


short DBOP::CurrentMsg()
{
#if 1
	return fCurrentMsg;  // need this result after Execute() and Quit() !!
#else
 	 if (fState == kMsgOpen) return fCurrentMsg; 
 	 else return 0;
#endif
}


long DBOP::ProcessNum()
{
	return fProcessNum;   
}

short DBOP::MsgFromProcnum( long processNum)
{
	if (fMsgList && processNum) {
		short i, n = fMsgList->GetSize();
		for (i=0; i<n; i++) {
			DMsg* msg= (DMsg*) fMsgList->At(i);
			if (msg->fPid == processNum) {
				return msg->fNum; 
				}
			}
		}
	return 0;
}


DMsg* DBOP::MsgFromMsgNum( short msgNum)
{
	DMsg* msg;
	if (fMsgList && msgNum) {
		short i, n = fMsgList->GetSize();
		for (i=0; i<n; i++) {
			msg= (DMsg*) fMsgList->At(i);
			if (msg->fNum == msgNum) return msg; 
			}
		}
	return NULL;
}


short DBOP::Delete( long processNum)
{
	short msgNum= MsgFromProcnum( processNum);
	return DeleteMsg( msgNum);
}

short DBOP::DeleteMsg( short msgNum)
{
	char buf[128];
	
	if (!msgNum) return 0;
	if (Connect() < kTransaction) return(fState);
	if (fState == kMsgOpen && msgNum == fCurrentMsg) CloseMsg();
	sprintf( buf, "DELE %d"CRLF, msgNum);
	SendStr( buf, kDontAddCRLF);
	if (!WaitHandshake('+',false)) ;
	
	DMsg* msg= MsgFromMsgNum( msgNum);
	if (msg) { msg->fPid= 0; msg->fNum= 0; }
	
	return(fState);
}


short DBOP::GetOutput( long processNum, DList* childFiles)
{
	short msgNum= MsgFromProcnum( processNum);
	return GetMsgOutput( msgNum, childFiles);
}

short DBOP::GetMsgOutput( short msgNum, DList* childFiles)
{
  char	buf[512], * name, * mode;
	
	if (!msgNum) return 0;
	if (OpenMsg( msgNum) < kMsgOpen) return(fState);

	long n= (childFiles) ? childFiles->GetSize() : 0;
	for (long i=0; i<n; i++) {
		DChildFile * cf= (DChildFile*) childFiles->At(i);
		if (cf && (cf->fKind == DChildFile::kOutput 
		|| cf->fKind == DChildFile::kStdout || cf->fKind == DChildFile::kStderr)) {
		
				// for each output file...
				// !! use server LIST of files for fine names ??
				// instead of client file names...
				// ?? and use LIST & fetch files not on input or output list, after outputs?

					// patch for special file names used by bopper
			Boolean dofreename= false;
			switch (cf->fKind) {
				case DChildFile::kStdout: name= "stdout"; mode= "a"; break;
				case DChildFile::kStderr: name= "stderr"; mode= "a"; break;
				default: 
#if 1
					name= (char*)cf->GetShortname();  
#else
					name= StrDup((char*)cf->GetShortname()); // ? problem w/ cf.fName trashing !
					dofreename= true;
#endif
					mode= "w"; 
					break;
				}
			
			sprintf( buf, "GET %s"CRLF, name);
			SendStr( buf, kDontAddCRLF);
			if (dofreename) { MemFree(name); name=NULL; }
			if (WaitHandshake('+',false)) {
				ulong datasize, datatotal = 0;
				Boolean done= false;
				char * data;
				short err= cf->Open(mode); // problem here ??
				do {
						// only good for ascii data...
			  	data= ReadWithChecks( datasize, kTCPStopAtdotcrlf, true);
			  	
			  		// ReadWithChecks isn't finding stop sign !!
					data[datasize]= 0;
					if (datasize==0) done= true;
					else if (EndOfMessage()) done= true;
			  	else if (StrStr( data, LINEEND"."LINEEND) != NULL) done= true;
			  	
			  	cf->WriteData( data, datasize);
			  	datatotal += datasize;
			  	MemFree(data);
				} while (!done);
				cf->Close();
				if (!WaitHandshake('+',false)) ;		// flush +OK after file?
				
				if (datatotal == 0) {
					cf->fDelete= true;
					cf->fAction= DChildFile::kNoAction; // don't try to display null file
					}
				}
			}
		}
	
	return(CloseMsg());
}


short DBOP::Execute( char * cmdline, DList* childFiles)
{
  char	buf[512];
	
	fDone= 0;
	if (Connect() < kTransaction) return(fState);
	if (fState > kTransaction) 
		if (CloseMsg() != kTransaction) return(Quit());
	
  		// new command
	SendStr("NEW"CRLF, kDontAddCRLF);
	if (!WaitHandshake('+')) return(Quit());	
	fState= kMsgOpen;
	int msgnum = 0;
	sscanf( fHandshake, "+OK Process %d ", &msgnum);
	fCurrentMsg= msgnum;

	sprintf( buf, "CMD %s"CRLF, cmdline);
	SendStr( buf, kDontAddCRLF);
	if (!WaitHandshake('+')) return(Quit());	

	long n= (childFiles) ? childFiles->GetSize() : 0;
	for (long i=0; i<n; i++) {
		DChildFile * cf= (DChildFile*) childFiles->At(i);
		if (cf && (cf->fKind == DChildFile::kInput || cf->fKind == DChildFile::kStdin)) {
				// for each input file...
			sprintf( buf, "PUT %s"CRLF, cf->GetShortname());
			SendStr( buf, kDontAddCRLF);
			if (!WaitHandshake('+')) return(Quit());	
	
			cf->Open("r");
			while (!cf->Eof()) {
				cf->ReadLine( buf, sizeof(buf)); // buf has NEWLINE !?
				SendStr(buf, kDontAddCRLF);  
				}
			cf->Close();
			SendCRLF(); 	// make sure of final newline ?
			SendStr("."CRLF, kDontAddCRLF);
			if (!WaitHandshake('+')) return(Quit());		
			}
		}
		
		// execute application
	SendStr("EXEC"CRLF, kDontAddCRLF);
	if (!WaitHandshake('+')) return(Quit());		

	long pid = 0;
	sscanf( fHandshake, "+OK pid=%u", &pid);
	fProcessNum= pid;
	
	sprintf(buf, "%d", msgnum);
	DMsg* msg= new DMsg( msgnum, pid, procexec, buf);
	fMsgList->InsertLast(msg);
	fMsgCount++;

#if 0
		// ! NOTE: current bopper status is returning erroneous "executing" state if no quit
		//   between execute & status call -- fix bopper !		
	return( Quit()); // close up connection to reduce net problems !?
#else		
		// wait a bit for results ??
	return(CloseMsg());
#endif
}  










// class DBopSetupDialog .............................


DBopSetupDialog::DBopSetupDialog(long id, DTaskMaster* itsSuperior, char* hostname,
		short width, short height, short left, short top, char* title) :
	DWindow( id, itsSuperior, fixed, width, height, left, top, title, kFreeOnClose)
{
	fUsertext = fPasstext= NULL;
	fHostname= hostname;
	fSavePass= NULL;
	GetDefaults();
}

DBopSetupDialog::~DBopSetupDialog() 
{
}


void DBopSetupDialog::GetDefaults() 
{
	if (!DBOP::gUsername || !DBOP::gPassword|| !DBOP::gHost || !DBOP::gPort) 
	{
		DBOP::gUsername= gApplication->GetPref("Username", "BOPPER", "");
		DBOP::gPassword= gApplication->GetPref("Password", "BOPPER", "");
		DBOP::gHost= gApplication->GetPref("Host", "BOPPER", "");
		DBOP::gPort= gApplication->GetPrefVal("Port", "BOPPER", "7110");
		}
}

void DBopSetupDialog::SetDlogItems( DView* username, DView* password) 
{
	fPasstext = password;
	fUsertext = username;
}

#if 0
void dapp_ProcessTasks(void)
{
	Nlm_ProcessEventOrIdle();
#ifndef WIN_MSWIN
	Nlm_RemoveDyingWindows();
#endif
	gTaskCentral->NextTask();
}
#endif

Nlm_Boolean DBopSetupDialog::PoseModally() 
{ 
#if 1
	return DWindow::PoseModally();
#else
	fModal = true;
	this->Open();
	while (fModal)  
		gApplication->ProcessTasks();  //dapp_ProcessTasks();  
	return fOkay; 
#endif
}

void DBopSetupDialog::OkayAction() 
{	
	if (fPasstext)	{
		MemFree(DBOP::gPassword); 
		DBOP::gPassword= fPasstext->GetText(); //StrDup(fgPass); 
			// !! MUST ENCRYPT/Decrypt password if storing to file
		if (0 && fSavePass->GetStatus())
 			gApplication->SetPref( DBOP::gPassword, "Password", "BOPPER");
		}
	if (fUsertext) {
		MemFree(DBOP::gUsername); 
		DBOP::gUsername= fUsertext->GetText(); //StrDup(fgUser); 
		gApplication->SetPref( DBOP::gUsername, "Username", "BOPPER");
		}
	if (fHosttext) {
		MemFree(DBOP::gHost); 
		DBOP::gHost= fHosttext->GetText();  
		gApplication->SetPref( DBOP::gHost, "Host", "BOPPER");
		}
	if (fPorttext) {
		char* sval= fPorttext->GetText();  
		long val= atol(sval);
		MemFree( sval);
		if (val) {
			DBOP::gPort= val;
			gApplication->SetPref( DBOP::gPort, "Port", "BOPPER");
			}
		}
}


Boolean DBopSetupDialog::IsMyAction(DTaskMaster* action) 
{	
	return DWindow::IsMyAction(action);	
}


DView* DBopSetupDialog::InstallUsername(DView* super)
{
	DPrompt* pr= new DPrompt(0, super, "Username:", 0, 0, Nlm_programFont);
	super->NextSubviewToRight();
	
	char* word = (DBOP::gUsername) ? DBOP::gUsername : "";
	DDialogText* ed= new DEditText( 0, super, word, 10);
	this->SetEditText(ed);
	super->NextSubviewBelowLeft();
	return ed;
}


DView* DBopSetupDialog::InstallPassword(DView* super)
{
	DPrompt* pr= new DPrompt(0, super, "Password:", 0, 0, Nlm_programFont);
	super->NextSubviewToRight();

	char* word = (DBOP::gPassword) ? DBOP::gPassword : "";
	DDialogText* ed= new DPasswordText( 0, super, word, 10);
	this->SetEditText(ed);
	super->NextSubviewBelowLeft();
	fSavePass= new DCheckBox(0, super, "Remember password?");
	fSavePass->Disable(); // till we figure out how to encrypt/decrypt passwd
	super->NextSubviewBelowLeft();
	return ed;
}

void DBopSetupDialog::Open()
{
	DView *user, *pass;
	char	buf[128];
	
	StrCpy(buf, "BOP logon");
	if (fHostname && *fHostname) { StrCat(buf, " to "); StrCat(buf, fHostname); }
	DPrompt* pr= new DPrompt(0, this, buf, 0, 0, Nlm_programFont);
	this->NextSubviewBelowLeft();
	
#ifdef TRIALBOPNOTE
 	{
#ifdef WIN_MAC
#define WRAP_LINE
#else
// need LINEEND's for MSWin's NotePanel !
#define WRAP_LINE   LINEEND
#endif

	Nlm_DayTime dt;
	if ( Nlm_GetDayTime( &dt) 
		&& dt.tm_year == 96
		&& dt.tm_mon < 8 /* sept */
		) {
			char* title= "Trial BOP services";
			char* desc = 
"For a limited time, you can try BOP services at "WRAP_LINE
"weed.bio.indiana.edu, port 7110.  To log on, use "LINEEND
"   Username: bop"LINEEND
"   Password: seqpup"LINEEND
"If you find this service useful, contact your local "WRAP_LINE
"biocomputing service provider to install a BOP "WRAP_LINE
"server (available from ftp:// iubio.bio.indiana.edu "WRAP_LINE
"/util/dclap/source/.)  This trial service at Indiana "WRAP_LINE
"will end in September 1996.  Access to GCG software "WRAP_LINE
"there after this trial is limited to Indiana U."WRAP_LINE
;
			DCluster* clu= new DCluster( 0, this, 0, 0, false, title); 
			new DNotePanel(0, clu, desc, 290, 150);	
			this->NextSubviewBelowLeft();
			}
	} 
#endif

	user= this->InstallUsername(this);
	pass= this->InstallPassword(this);
	SetDlogItems( user, pass);
	
	if (1) { //!fHostname || !*fHostname)  // ?? do only if fHostname == NULL ??
		char * word, buf[128];
		DView* super= this;
		(void) new DPrompt(0, super, "BOP Host:", 0, 0, Nlm_programFont);
		super->NextSubviewToRight();
		word= (DBOP::gHost) ? DBOP::gHost : "";
		fHosttext= new DEditText( 0, super, word, 15);
		super->NextSubviewBelowLeft();

		(void) new DPrompt(0, super, "BOP Port:", 0, 0, Nlm_programFont);
		super->NextSubviewToRight();
		if (!DBOP::gPort) DBOP::gPort= kBOPport;
		sprintf(buf, "%d", DBOP::gPort);
		fPorttext= new DEditText( 0, super, buf, 10);
		super->NextSubviewBelowLeft();
		}
	
	AddOkayCancelButtons();
	DWindow::Open();
}


void BOPSetLogon()
{
	Boolean okay= false;
	DBopSetupDialog* win= 
		new DBopSetupDialog(0, gApplication, NULL, -10, -10, -20, -20, "BOP Logon");
	if (win && win->PoseModally()) {
		okay= true;
		}
#ifdef WIN_MAC
  delete win;// already deleted in MSWIN !? & UNIX !!
#endif
}

