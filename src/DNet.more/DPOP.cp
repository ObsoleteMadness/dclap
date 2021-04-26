// UPOP.cp 
// POP Mail reader
//by D. Gilbert, May 1991


			
/*  ---   general use: ------------
		TPOP		VAR  aPop;
		
		New(aPop); 
		FailNIL(aPop);
		aPop->IPOP( 'cricket.bio.indiana.edu', 'anybody', 'somepass');
		
		Repeat as requested from User commands:
		if aPop.MailWaiting then begin
			nMsg= aPop->NumberOfMessages();
			for iMsg= 1 to nMsg do begin
				hMsg= aPop->ReadTop( iMsg, 0);
				--- collect, display msg directory ----
				end();
				
			-- user select msg to read --
			hMsg= aPop->ReadMail( iMsg, True); 
			
			aPop->Release(); -- when done reading for now
			end();
						
		aPop->Free();  -- when entirely done with mail
*/

#include "includes.h"
#include "UTCP.h"
#include "UPOP.h"

#pragma segment UPOP

 
const short		kPOPport = 110; // telnet port 110 == POP3 

//const int		kTCPwaitingforopen = 64;
const int		kPOPopen = 128;		// fPOPstates 
const int		kPOPclosed = 256;
const int		kPOPtransaction = 512;

const char		chLineFeed	= 10;
const char		chRtn				= 13; 
	
	


TPOP::TPOP( const CStr255& POPAddress, const CStr63& password)
{
	//ITCP();
	fPOPState= kPOPclosed;
	fPopAddress= POPAddress;
	fPassword= password;  
//-	ResetHost( POPaddress, password); << NEED InstallMessageLine before this 
}

TPOP::TPOP()
{
	fPOPState= kPOPclosed;
	fPopAddress= "";
	fPassword= "";  
}
			
TPOP::~TPOP()   
{
	Release();
}




void TPOP::ResetHost( const CStr255& POPAddress, const CStr63& password)
{
	fPopAddress= POPAddress;
	fPassword= password; //save for .ReOpen 
	ReOpen();
}

void TPOP::ReOpen()
{
	Handle		hMsg; 
	CStr255		aHost, aName;
	short 		k;
 	
	k= fPopAddress.Pos("@");
	if (k==0) Fail("Bad POP mail address, missing '@'");
	aName= fPopAddress;
	aName[0]=  k-1;
	aHost= fPopAddress;
	aHost.Delete(1,k);
	
	// ?? make sure we are closed 
	if (Status() != kTCPreleased)  this->Release();
	
  this->ShowMessage( aHost);
	fPOPState= kPOPclosed;
	fMsgCount= 0;
	fPOPhostIP= this->NameToAddress( aHost);
  if (fPOPhostIP == 0) Fail("Bad address for POP Host.");
	else {
		ActiveOpen(fPOPhostIP, kPOPport, 0);
		if (Failed()) return;
		if (!WaitedForOpen()) return;
		fPOPState= kPOPopen;
		DisposeIfHandle( RecvChars( CharsAvailable()));	

		SendStr("USER " + aName, kAddCRLF);
		hMsg= RecvUpTo( true, chLineFeed, NULL);
		this->ShowMessage( (char *) (*hMsg));
		DisposeIfHandle( RecvChars( CharsAvailable()));
		
		if (**hMsg != '+') 
			Fail( CStr63("Bad username for POP: ") + (char *)(*hMsg) );
		else {
			DisposeIfHandle( hMsg);
			SendStr( "PASS " + fPassword, kAddCRLF);
			hMsg= RecvUpTo( true, chLineFeed, NULL);
			aName= CStr255( (char *)(*hMsg));
			this->ShowMessage( aName);
			DisposeIfHandle( hMsg);
			DisposeIfHandle( RecvChars( CharsAvailable()));
			if (aName[1] != '+') 
				Fail( "Bad password for POP: "+ aName);
			else {
				fPOPState= kPOPtransaction;
				GetMessageCount();
				}
			}
		DisposeIfHandle( hMsg);
		}
//- this->ShowMessage( State); 
}


void TPOP::Close() // override 
{
	FailInfo fi;
	if (fi.Try()) {
		if (fPOPState != kPOPclosed) {
			SendStr("QUIT", kAddCRLF);
			fTimeout= 240;
			EatResponseLine();
			fTimeout= gTCPTimeout;
			fPOPState= kPOPclosed;
			}
		TTCP::Close();
		fi.Success();
		}
	else { // fail
		TTCP::Close();
		fi.ReSignal();
		}
}

void TPOP::Release() // override 
{
	FailInfo fi;
	if (fi.Try()) {
		if (fPOPState != kPOPclosed) {
			SendStr("QUIT", kAddCRLF);
			fTimeout= 240;
			EatResponseLine();
			fTimeout= gTCPTimeout;
			fPOPState= kPOPclosed;
			}
		TTCP::Release();
		fi.Success();
		}
	else { // fail
		TTCP::Release();
		fi.ReSignal();
		}
}



			
char* TPOP::StatusString(short state)   // override  
{
	CStr255 aState;
	aState= TTCP::StatusString(state);
	switch (fPOPState) {
		case kPOPclosed	: aState=  aState + "; POP closed"; break;
		case kPOPopen		: aState= aState +"; POP connected"; break;
		case kPOPtransaction	: aState= aState +"; POP mailbox open"; break;
		}
	return (char*) aState;
}


void TPOP::GetMessageCount()
{ 
	fMsgCount= 0;
	if (fPOPState == kPOPtransaction) {
		SendStr("STAT", kAddCRLF);
		Handle hMsg= RecvUpTo( true, chLineFeed, NULL); 
		//CStr255 aStr= (char*) (*hMsg);
		this->ShowMessage((char*) (*hMsg));  
		DisposeIfHandle( RecvChars( CharsAvailable()));
		if (**hMsg == '+') {
			char *cp = *hMsg + 3; // skip 3 chars
			//fMsgCount= atol( cp);
			char *cend= strchr(*hMsg, 0);
			while (cp < cend && *cp == ' ') cp++;
			while (cp < cend && *cp != ' ') cp++;
			*cp= 0;
			long aNum= 0;
			stringtonum( cp, &aNum); //<< finicky...
			fMsgCount= (short) aNum;
#if 0
			short len= aStr.Length();
			short i;
			for (i= 1; i<=3; i++) aStr[i]= ' ';
			i= 4; 
			while (i<len  && aStr[i]==' ') i++;
				//-- aNum= ORD(aStr[i]) - ORD('0'); 
			while (i<len && aStr[i]!=' ') i++;
			if (aStr[i]==' ') i--;
			aStr[0]= i;
			long aNum= 0;
			StringToNum( aStr, aNum); //<< finicky...
			fMsgCount= aNum;
#endif
			}
		DisposHandle( hMsg);
		}
}


short TPOP::NumberOfMessages()
{
	if (fPOPState == kPOPtransaction) return fMsgCount;
	else return 0;
}


Handle TPOP::ReadMail(short msgNum, Boolean andDelete)
{
	CStr255  sNum; 
	Handle		hMsg;
 
	if (fPOPState == kPOPtransaction) {
		NumToString( msgNum, sNum);
		SendStr( "RETR " + sNum, kAddCRLF);
		hMsg= RecvMsg( '+', FreeMem() / 2);
		this->ShowMessage( (char*) (*hMsg));
		if ( hMsg == NULL || **hMsg == '¥') 
			return NULL;  
		else {
			if (andDelete) DeleteMail( msgNum);
			return hMsg;
			}
		}	
	else
		return NULL;
}



void DropTail( short keeplines, char endLine, short maxEndline, Handle h)
{  
	short		n = 0, linewidth; 
	long		i, hlen;
	char		*buf;
	Boolean		pastHeader = false;

	hlen= GetHandleSize(h);
	maxEndline= min( 255, maxEndline);
	for (i= 1, linewidth= 0, buf= *h; 
			i <= hlen; 
			i++, linewidth++, buf++) {
		if (*buf == endLine || linewidth > maxEndline) {
			if (linewidth < 4) pastHeader= true;
			if (pastHeader) n++; 
			linewidth= 0;
			if (n >= keeplines) {
				SetHandleSize(h, i);
				return;
				}
			}
		} 
} 

Handle TPOP::ReadTop(short msgNum, short nLines)
{
	CStr255	sNum, sNum2;
	Handle		hMsg;

 	if (fPOPState == kPOPtransaction) {
		NumToString( msgNum, sNum);
		NumToString( nLines, sNum2);
		SendStr( "TOP " + sNum + " " + sNum2, kAddCRLF);
		hMsg= RecvMsg('+', FreeMem()/2);
		this->ShowMessage( (char*) (*hMsg));
		if (hMsg==NULL || **hMsg == '¥') {
			//'TOP' is Optional POP3 msg, try .ReadMail and cut to nLines...
			hMsg= ReadMail( msgNum, false);
			if (hMsg!=NULL) DropTail( nLines, chRtn, 255, hMsg);
			return hMsg; 
			}		
		else 
			return hMsg;
		}	
	else
		return NULL;
}



void TPOP::DeleteMail(short msgNum)
{
	CStr255	sNum;
	Handle		hMsg;
	if (fPOPState == kPOPtransaction) {
		NumToString(msgNum, sNum);
		SendStr( "DELE "+sNum, kAddCRLF);
		hMsg= RecvUpTo( true, chLineFeed, NULL);
		this->ShowMessage( (char*) (*hMsg));
		if (hMsg!=NULL && **hMsg == '+')  ;
		DisposeIfHandle( hMsg);
		DisposeIfHandle( RecvChars( CharsAvailable()));
		}
}


void TPOP::UnDeleteMail()
{
	Handle	hMsg;
	if (fPOPState == kPOPtransaction) {
		SendStr("RSET",kAddCRLF);
			//count isn't changed by delete/undel >> GetMessageCount; 
		hMsg= RecvUpTo( true, chLineFeed, NULL);
		this->ShowMessage((char*) (*hMsg));
		if (hMsg!=NULL  && **hMsg == '+') ;
		DisposeIfHandle( hMsg);
		DisposeIfHandle( RecvChars( CharsAvailable()));
		}	
}



Boolean TPOP::MailWaiting()
/*general purpose checker:  
	if connection == closed, reopen...
	if mail waiting, return yes && leave open
	if no mail, close connection
---*/
{
	Boolean		gotsome;
	if (fPOPState != kPOPtransaction || Status() != kTCPestablished)
			ReOpen();
	gotsome= NumberOfMessages() > 0; 
	if (!gotsome) Release();
  this->ShowMessage( StatusString(Status()) ); 
	return gotsome;
}