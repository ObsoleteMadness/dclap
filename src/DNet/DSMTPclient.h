// DSMTP.h  
// SMTP Mail sender
// by D. Gilbert, May 1991
 

#ifndef __DSMTP__
#define __DSMTP__

#include "DTCP.h"

 

class DSMTP	: public DTCP {
public:
	DSMTP();
	DSMTP( char *SMTPHostName);
	virtual ~DSMTP();

	virtual void InitData();
	virtual void ResetHost( char * SMTPHostName);
	virtual void CheckReturnAddress( Boolean docheck = true);
	virtual void SendMail( char * theTo, char * theFrom, char * theSubject, 
				 char * theCCopy, char * theMessage);
	virtual void SendMail( char * theTo, char * theFrom, char * theSubject, 
				 char * theCCopy, char * theMessage, long pLines[], long nLines);

protected:
	char*		fSMTPhostname;
	char*		fMailerID;
	Boolean	fCheckReturnAddress;
	virtual Boolean WaitHandshake( char OkayCode);
};
			
#endif
