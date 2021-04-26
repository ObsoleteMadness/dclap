// DBOPclient.h  
// by D. Gilbert, May 1996
 

#ifndef __DBOP__
#define __DBOP__

#include "DTCP.h"


class DList;
class DMsg;

class DBOP	: public DTCP {
public:
	enum BOPstatus {
		kUnknown,
		kError,
		kClosed,
		kConnected,
		kTransaction,
		kMsgOpen,
		kProcDone
		};
	static  char* gUsername;
	static  char* gPassword;
	static  char* gHost;
	static  long	gPort;
		
	DBOP();
	DBOP(char *username, char *password,  char *hostname, long port = 0);
	virtual ~DBOP();

	virtual void ResetHost(char *username, char *password, char *hostname, long port = 0);

	virtual short Connect();
	virtual short GetLogon();
	virtual short Quit();  
	virtual short Execute( char * cmdline, DList* childFiles);
	virtual short OpenMsg( short msgNum);
	virtual short CloseMsg();  
	virtual	short CheckStatus( long processNum);
	virtual	short CheckMsgStatus( short msgNum);
	virtual short GetOutput( long processNum, DList* childFiles);
	virtual short GetMsgOutput( short msgNum, DList* childFiles);
	virtual short DeleteMsg(short msgNum);
	virtual short Delete( long processNum);
 	virtual short CurrentMsg();
 	virtual long  ProcessNum();
	virtual short MsgFromProcnum( long processNum);
	virtual DMsg* MsgFromMsgNum( short msgNum);

	virtual const char* GetUser() { return fUsername; }
	virtual const char* GetPass() { return fPassword; }
	virtual const char* GetHost() { return fHostname; }

 	////// ??
	//virtual short NewMsg();  
	//virtual short OpenMsg(short msgnum);  
	//virtual short NumberOfMessages();
	//virtual Boolean MsgWaiting(); //general purpose tester   
	//virtual char* ListFiles();
	//virtual	char* StatusString(short state);   
	//virtual short UnDeleteMsgs();
	
protected:
	char	* fHostname, * fUsername, * fPassword, * fId;
	short		fPort, fDone, fState; //closed/active/transaction
	short		fCurrentMsg, fMsgCount;
	long		fProcessNum;
	char	* fHandshake;
	DList * fMsgList;
	
	virtual Boolean WaitHandshake( char OkayCode, Boolean showfail = true);
	virtual void InitData();
};

void BOPSetLogon();

			
#endif
