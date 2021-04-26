//	UPOP.p  
// POP Mail reader
// by D. Gilbert, May 1991

#ifndef __UPOP__
#define __UPOP__

 
		
class	TPOP : public TTCP {
public:
	long		fPOPhostIP;
	short		fPOPState; //closed/active/transaction
	short		fMsgCount;
	CStr255	fPopAddress ;
	CStr63	fPassword;
	
	TPOP();
	TPOP( const CStr255& POPAddress, const CStr63& password);
	virtual ~TPOP();
	//	 void Free(); // override */ /* == QUIT when done 
	
	virtual void ResetHost( const CStr255& POPAddress, const CStr63& password);
	virtual void ReOpen();
				
	virtual void Close(); // override 
	virtual void Release(); // override */ /*? don't need to override all ?
	virtual	char* StatusString(short state);  // override  
	virtual Boolean MailWaiting(); //general purpose tester   
	virtual void GetMessageCount();
	virtual short NumberOfMessages();
	virtual Handle ReadMail(short msgNum, Boolean andDelete);
	virtual Handle ReadTop(short msgNum, short nLines);
	virtual void DeleteMail(short msgNum);
	virtual void UnDeleteMail();
	
};


#endif
