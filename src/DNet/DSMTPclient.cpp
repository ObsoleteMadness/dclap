// DSMTP.cp 
// SMTP Mail sender
// by D. Gilbert, May 1991
 
 
#include <ncbi.h>
#include <dgg.h>
#include "DSMTPclient.h"

 
const short	kSMTPport = 25;
static const	long	kMaxInt 	= 32000;	




// DSMTP .............................

DSMTP::DSMTP()
{
	InitData();
}

DSMTP::DSMTP( char* SMTPHostName)
{
	InitData();
	ResetHost( SMTPHostName);
}

DSMTP::~DSMTP()
{
	if (fSMTPhostname) MemFree(fSMTPhostname);
	if (fMailerID) MemFree(fMailerID);
}

void DSMTP::ResetHost( char* SMTPHostName)
{
	if (fSMTPhostname) fSMTPhostname= (char*)MemFree( fSMTPhostname);
	if (!NameToAddress(SMTPHostName)) {
		Nlm_Message (MSG_ERROR, "DSMTP::ResetHost: Bad SMTP host address");
		return; 
		}
	fSMTPhostname= StrSave( SMTPHostName);
}

void DSMTP::InitData()
{
  char  buf[256];
  fSMTPhostname= NULL;
	fCheckReturnAddress= false; // forget this for now, fails too often on good addresses
	sprintf( buf, "DClap-SMTP %s", (char*)VersionString());
	fMailerID= StrSave(buf);
}

void DSMTP::CheckReturnAddress( Boolean docheck)
{
	fCheckReturnAddress= docheck;
}



void DSMTP::SendMail( char* theTo,  char*  theFrom,  char*  theSubject, 
									 char* theCCopy,  char*  theMessage)
{
	const short maxEndLine = 81;
	const short minEndLine = 40;
	long  i, nLines;
	short linewidth, maxLines= 100;
	long  hlen= strlen(theMessage);
	
	//if (hlen > kMaxInt) hlen= kMaxInt; // error !?!
	long* hLines= (long*) MemNew( (maxLines+1) * sizeof(long));
	hLines[0]= 0;
	
	char* buf = (char*) theMessage;
	for (i= 1, linewidth= 0, nLines= 0; i <= hlen; buf++, linewidth++, i++ ) {
			
		if (*buf == kCR || *buf == kLF || linewidth > maxEndLine || i == hlen) {
			nLines++;
			if (nLines > maxLines) {
				maxLines= nLines + 100;
				hLines= (long*) Nlm_MemMore( hLines, (maxLines+1) * sizeof(long));
				}
			if (*buf == kCR || *buf == kLF || i == hlen) 
				hLines[nLines]= i;  
			else {
				while ( (*buf > ' ')  &&  (linewidth > minEndLine)) {
					buf--;
					linewidth--;
					i--;
					}
				if (*buf > ' ') {
					short k= maxEndLine - linewidth;
					buf += k;
					linewidth += k;
					i += k;
					}					
				hLines[nLines]= i;
				}
			linewidth= 0;
			}
		}
		
	SendMail( theTo, theFrom, theSubject, theCCopy, theMessage, hLines, nLines);
	MemFree(hLines);
}


 


Boolean DSMTP::WaitHandshake( char OkayCode)
{
	char* aChars= RecvLine(); 
	if (!aChars) {
		return false;
		}
	else if (*aChars != OkayCode) {
		Fail( aChars); 
		MemFree( aChars);
		return false;
		}
	else {
		MemFree( aChars);
		return true;
		}
}


void DSMTP::SendMail( char * theTo,  char * theFrom,  char * theSubject, 
				 char * theCCopy,  char * theMessage, long pLines[], long nLines)
{
	const	short	kBufmax= 512;
  char	buf[kBufmax+1];
  char aName[255], aDate[255];
	long	i, k, bytesread;
	long* pLineStart;
	Boolean mailSent= false;	
	Boolean okay= true;
	char *namep, *line;
	
  if (fSMTPhostname == NULL) {
		Nlm_Message (MSG_ERROR, "DSMTP::SendMail: Bad SMTP host address");
		return;
		}
  if (theFrom == NULL || *theFrom == 0) {
		okay= false;
  	}
	if (okay && fCheckReturnAddress) {
		char* kp= strchr(theFrom,'@');
		okay= (kp != NULL);
		if (okay) okay= (NameToAddress(kp+1) != 0);
		}
	if (!okay) {
		Nlm_Message (MSG_ERROR, "DSMTP::SendMail: Bad return address");
		return;
		}
  if (theMessage == NULL || *theMessage == 0) {
		Nlm_Message (MSG_ERROR, "DSMTP::SendMail: Missing DATA.");
    return;
  	}
  if (theTo == NULL || *theTo == 0) {
		Nlm_Message (MSG_ERROR, "DSMTP::SendMail: Missing TO address");
    return;
  	}
	
	Open(fSMTPhostname, kSMTPport);
	if ( Failed()) return;  
	if (!WaitedForOpen()) return;
	EatResponseLine();  // SMTP SendMail title from host ??
	
	sprintf( buf, "HELO %s"CRLF,theFrom);
	SendStr( buf, kDontAddCRLF);
	EatResponseLine();

	sprintf(buf, "MAIL FROM: <%s>"CRLF, theFrom);
	SendStr(buf, kDontAddCRLF);
	if (!WaitHandshake('2')) goto quitSMTP;	
  
	strcpy(aName,theTo); 
	namep= aName;
	for ( k= 1; k<= 2; k++) {
		while (namep && *namep) {
			line= RecvChars( CharsAvailable(), bytesread);
			MemFree( line);
			
			char* nameend= strchr(namep, ',');
			if (nameend) *nameend= 0;			
			sprintf(buf, "RCPT TO: <%s>"CRLF, namep);
			SendStr(buf, kDontAddCRLF);
			if (!WaitHandshake('2')) goto quitSMTP;	
			if (nameend) namep= nameend+1;
			else namep= NULL;
			}
		if (theCCopy) { strcpy( aName, theCCopy); namep= aName; }
		else namep= NULL;
		}
		
	line= RecvChars( CharsAvailable(), bytesread);
	MemFree( line);
	SendStr("DATA"CRLF, kDontAddCRLF);
	if (!WaitHandshake('3')) goto quitSMTP;	
	
	// send header lines... 
	sprintf(buf, "From: %s"CRLF, theFrom);
  SendStr(buf, kDontAddCRLF);
	sprintf(buf, "To: %s"CRLF, theTo);
	SendStr(buf, kDontAddCRLF);
	if (theCCopy && *theCCopy) {
		sprintf(buf, "Cc: %s"CRLF, theCCopy);
		SendStr(buf, kDontAddCRLF);
		}
	sprintf(buf, "Subject: %s"CRLF, (theSubject)?theSubject:"");
	SendStr(buf, kDontAddCRLF);
	
	if (Nlm_DayTimeStr( aDate, true, true)) {
		sprintf(buf, "Date: %s"CRLF, aDate);
		SendStr(buf, kDontAddCRLF);
		}
		
	sprintf(buf, "X-Mailer: %s"CRLF, fMailerID);
	SendStr(buf, kDontAddCRLF);
	SendCRLF(); //blank line
	
	// send message buffer 
	pLineStart= (long*) pLines;
	for (i= 0; i< nLines; i++) {
		char* bufPtr= theMessage + pLineStart[i];
		long len=  pLineStart[i+1] - pLineStart[i];
		if (i+1 == nLines  &&  pLineStart[i+1] != *LineEnd) len++; // ensure last char is sent.
		//if ((len>0) && (bufPtr[len-1] == *LineEnd)) len--;
		if ((len>0) && (bufPtr[len-1] == kCR || bufPtr[len-1] == kLF)) len--;
		if (len<1)
			SendCRLF();
		else if (*bufPtr == '.') {
			if (len > kBufmax-1) len= kBufmax-1;  
			buf[0]= '.';
			MemCopy( &buf[1], bufPtr, len);
			SendBytes( buf, len);
			SendCRLF();
			}		
		else {
			SendBytes( bufPtr, len);
			SendCRLF();
			}
		}
 
	SendStr("."CRLF, kDontAddCRLF);
	//if (!WaitHandshake('2')) goto quitSMTP;	
	// ^^ Not getting '2' here, but "R?? mail received..."
	//line= RecvLine(); MemFree( line);
 	mailSent= true;

quitSMTP:
	line= RecvChars( CharsAvailable(), bytesread);
	MemFree( line);
	SendStr("QUIT"CRLF, kDontAddCRLF);
	
	//fTimeout= 100; //?? want short timeout here waiting for recvline
	//line= RecvLine(); MemFree( line);
	Close();
	Release();
	return;
}  



