// DTCP.cp 
// by D. Gilbert, 1991-92, with help from code of Harry Chesley and others 
// 1994 -- rewritten for multi-platform tcp-sockets

 
#include "DTCP.h"
#include <DUtil.h>
#include <ncbi.h>
#include <dgg.h>
#include <Dversion.h>

#include "dnettcp.h"


Local const	short		kUTCPVersion	= 3;
Local const	char*		kUTCPVersionString = 
#ifdef WIN_MAC
"v.3.3d, Jan 95 (mac)";
#endif
#ifdef WIN_MSWIN
"v.3.3d, Jan 95 (mswin)";
#endif
#ifdef WIN_MOTIF
"v.3.3d, Jan 95 (xwin)";
#endif


enum tcpErrors {	
	kErrConnectionBroken = -1, //common way for gopher server to end transmission 
	kErrTimedOut = -2,
	kErrUserBreak = -3,
	kErrMemFull = -4,
	kErrOpenTimeout = -5
	};

Local const short		kRecvChunkSize = 500;   	// bytes/ handle increment
Local const	short 	kTCPDefaultTimeout = 1200;	// default # ticks (1/60sec) to wait for TCP remote
Local const long 		kDefaultOpenDelay = 20;

		// this is a guess -- no generic way to find memory available
Local const	long	kMaxReadBuffer = 4048; //32000;
						


	// Public Global vars
Global long		gTCPTimeout = kTCPDefaultTimeout;

Local short 	gMacTCPRefNum = 0;		// not a global now...

	// Private global vars
Local long  	gMyIP = 0;
Local char*	 	gMyDotName = NULL; 
Local Boolean gResolverIsOpen = false;




short OpenMacTCP()
{
	if (gMacTCPRefNum !=0 ) 
		return 0;
	else {
		short err= 0;
		return err;
		}
}

void InitializeTCP() 		// InitUMacTCP()
{
	gMyDotName = NULL;
	gMacTCPRefNum= 0;
	short err= OpenMacTCP(); 
}

void FinishTCP() 				// CloseDownUMacTCP()
{
	gMacTCPRefNum = 0;
}

Boolean TCPIsItInstalled() 		// IsMacTCPInstalled()
{
	return (0 == OpenMacTCP());
}

long MyIP()
{
	if (!OpenMacTCP() || gMyIP)  return gMyIP;
	gMyIP = MyIPaddress();
	return gMyIP;
}

void GetMyDotName(char* myDotName)
{
	if (!OpenMacTCP()) {
		myDotName = gMyDotName = ".";
		return;
		}
	if (myDotName) gMyDotName= IP2DotName(MyIP());
	myDotName = gMyDotName;
}	


char* IP2DotName(long ip)
{
	char *name = NULL;
	
	// ?? do we need this proc
	return name;
}


long DotName2IP(const char* name)
{
	return Hostname2IP( (char*)name);
}



 
// DTCP ..........................

void DTCP::Initialize()
{
	fSocket= -1;
	fError= NULL;
	fErrNo= 0;
	fFailed= false;
	fTimeout= gTCPTimeout;
	fLastSentCRLF= false;
	
	fDoShowProgress= false;
	fResultSize= 0;
	fMaxResultSize= 0;
	fResultNew= 0;
	fBytesread= 0;
	fResultTotal= 0; // this is now a ShowProgress::LastBytesRead counter
	fLimitResultSize= 0;
	fEndofMessage= false;
	fResultHand= NULL;
	fNullTerm= true; //??
	fLastc = fLast2c= fLast3c = 0; // receive -- CRLF conversion record
	fStartTime= 0;
	fConnectTime= 0;

	fMessageProc = NULL;
	fMessageObj = NULL;
	fBreakProc = NULL;
	fBreakObj = NULL;
	fReadSaveLen = 0;
	fReadSaveBase = NULL;
	fReadSave = NULL;

	(void) OpenMacTCP();
}


DTCP::DTCP()
{
	Initialize();
}


DTCP::~DTCP() 
{
	if (fConnectionIsOpen)  
		Close();
	if (fStreamIsOpen)  
		Release();
}


Boolean DTCP::UserBreak()
{  
	if (fBreakProc)
		return (fBreakProc)(fBreakObj);
	else
		return false;
}	

void DTCP::StreamYieldTime()
{
	if (UserBreak()) Fail(-1); //??
}


void DTCP::Fail(long errNo)
{
	if (errNo) {
		fErrNo= errNo;
		fError= NULL;
		fFailed= true;
		Nlm_Message (MSG_ERROR, "Network DTCP error # %d",errNo);
		}
}


void DTCP::Fail(const char* msg)
{
	if (msg) {
		fErrNo= 0;
		fError= (char*) msg;
		fFailed= true;
		Nlm_Message (MSG_ERROR, "Network DTCP error:\n%s", (char*) msg);
		}
}

Boolean DTCP::Failed()
{
	return fFailed;
}



void DTCP::InstallUserBreak(TCPUserBreakHandler aproc, DTaskMaster* itsObject)
{
	fBreakProc= aproc;
	fBreakObj= itsObject;
}

void DTCP::InstallMessageHandler(TCPMessageHandler aproc, DTaskMaster* itsObject)
{
	fMessageProc= aproc;
	fMessageObj= itsObject;
}

void DTCP::ShowMessage(const char* msg)
{
	if (fMessageProc) (fMessageProc)(fMessageObj, 0, msg);
}

Boolean DTCP::IsTCPInstalled()
{
	return TCPIsItInstalled();
}


long DTCP::NameToAddress(const char* hostName)
{
	return DotName2IP(hostName);
}

	// non-functional IP2DotName...
char* DTCP::AddressToName(long address)
{
	return IP2DotName(address);
}

	// non-functional IP2DotName...
char* DTCP::DotAddrToName(const char* dotAddress)
{
	return IP2DotName( DotName2IP( dotAddress));
}

char* DTCP::RemoteName()
{
	char* name = NULL;
	if (fSocket>=0) {
		const long namelen = 256;
		name= (char*) MemNew(namelen);
		short err= SockHostname(fSocket, name, namelen);
		}
	return name;
}


		// do we want both of these??
long DTCP::Version()
{
#if 1
	return kDCLAPVersion;
#else
	return kUTCPVersion;
#endif
}

const char* DTCP::VersionString()
{
#if 1
	return kDCLAPVersionString;
#else
	return kUTCPVersionString;
#endif
}


char* DTCP::StatusString(short state)
{
	switch (state) {
		case kTCPlistening	: return "TCP listening";
		case kTCPwaitingforopen: return "TCP waiting for open";
		case kTCPopening		: return "TCP opening";
	 	case kTCPestablished: return "TCP established";
		case kTCPPleaseClose: return "TCP please close";
		case kTCPclosing		: return "TCP closing";
		case kTCPclosed			: return "TCP closed";
		case kTCPreleased		: return "TCP released";
		case kTCPUnknownState:
		default							: return "TCP unknown state";
		}
}


long DTCP::Status()
{
	if (!fStreamIsOpen)				return kTCPreleased;
	else if (!fConnectionIsOpen) 	return kTCPPleaseClose;
	else  										return kTCPestablished;
}

void DTCP::StatusMessage(void)
{
	ShowMessage( StatusString(Status()));
}

Boolean DTCP::EndOfMessage()
{
	return fEndofMessage;
	//?? return (fEndofMessage || (NewBytesReceived == 0));
	//^^ bad when linefeeds are received but don't count in NewBytesRec...!!!
}

void DTCP::SetEndOfMessage( Boolean isEnded)
{
	fEndofMessage= isEnded;
}


//ncbi.h: 	time_t Nlm_GetSecs() == time() call, should be okay on all ansi c systems?

Boolean DTCP::WaitedForOpen(long delayticks)
{
	short		currStat, oldStat;
	
	oldStat= -123;
	while (true) {
		// Delay(5, &aTicks); << what is ncbi/generic equivalent ??
		//for (aTicks= GetSecs() + 5; GetSecs() < aTicks; ) ;
		
		currStat= (short)Status();
		if (currStat!=oldStat) ShowMessage( StatusString(currStat));
		switch (currStat) {
		
			case kTCPestablished:  
#if FIX_LATER
				//if (delayticks) 
				//for (aTicks= GetSecs() + delayticks; GetSecs() < aTicks; ) ;
				// ^^ this is TOO LONG
#endif
				return true;

			case kTCPUnknownState:
			case kTCPPleaseClose:
			case kTCPreleased:
			case kTCPclosed	:  
				Fail("Couldn't connect to TCP service.");
				Release();
				return false;
				
			default: if (UserBreak()) {
				Release();
				return false;
				}
			}
		oldStat= currStat;
		}
	return false;
}

Boolean DTCP::WaitedForOpen()
{
	return WaitedForOpen(kDefaultOpenDelay);
}



void DTCP::SetShowProgress( Boolean turnOn)
{
	fDoShowProgress= turnOn;
}

void DTCP::ShowProgress( long sendRecvCount)
{
	char msg[128];
	sprintf( msg, "TCP bytes sent: %d", sendRecvCount);
	ShowMessage(msg);
}

void DTCP::ShowProgress()
{
	if (fBytesread > fResultTotal + 1024) {
		ShowProgress( fBytesread);
		fResultTotal= fBytesread;
		}
}




void DTCP::Release()
{
	short err;
	fBytesread= 0;
	fStreamIsOpen = false;
	if (fSocket>=0) err= SockClose(fSocket); 
	fSocket= -1;
	fConnectionIsOpen = false;
}



void DTCP::Open( char* hostname, unsigned short hostport, unsigned short localport)
{
	fBytesread= 0;
	fLastc= fLast2c= fLast3c= 0;
	fConnectTime= 0;
	fStartTime=	GetSecs();
	fLastSentCRLF= false;
	fConnectionIsOpen= false;
	
	fSocket= SockOpen( hostname, hostport);
	if (fSocket<0) {
		char buf[256];
		if (!hostname) hostname="NULL";
		switch (fSocket) {
			case errHost		:  
				sprintf(buf, "Can't get IP# for host '%s', port %d", 
					hostname, hostport);
				Fail(buf); return;
			case errSocket	: 
				Fail("Can't create IP socket"); return;
			case errConnect	: 
				sprintf(buf, "Can't connect to host '%s', port %d", 
						hostname, hostport);
				Fail(buf); return;
			default					: 
				Fail("Can't open IP connection"); return;
			}
		}
	fStreamIsOpen = true;
	fConnectionIsOpen = true;
}



// CloseConnection aborts the connection if it could not be closed
void DTCP::Close()
{
	short err;
	fConnectTime = GetSecs() - fStartTime;
	if (fSocket>=0)  err= SockClose(fSocket); 
	fSocket= -1;
	fConnectionIsOpen = false;
}


void DTCP::Abort()
{
	short err;
	if (fSocket>=0) err= SockClose(fSocket); 
	fSocket= -1;
	fConnectionIsOpen = false;
}





// Send functions ......................................


void DTCP::SendBytes(void *data, long datasize, Boolean immediately)
{
	long len= SockWrite( fSocket, data, datasize);
	char*	pp =  (char*) data;
	fLastSentCRLF= (pp[datasize-1] == kLF && pp[datasize-2] == kCR);
}


void DTCP::Send( char* cstring, Boolean immediately)
{
	SendBytes( cstring, StrLen(cstring), immediately);
}

void DTCP::SendCRLF(Boolean EvenIfLastSendHadCRLF, Boolean immediately)
{
	static char	buf[2] = {kCR,kLF};
	if (EvenIfLastSendHadCRLF || !fLastSentCRLF)
		SendBytes( &buf, 2, immediately);
}

void DTCP::SendStr( char* s, Boolean addCRLF, Boolean immediately)
{
	SendBytes( s, strlen(s), immediately);
	if (addCRLF)	SendCRLF( true, immediately);
}








// Receive status functions ......................................

long DTCP::CharsAvailable() 
{		
	if (!fStreamIsOpen) 
		return 0;
	else if (!fConnectionIsOpen)  
		return 0;
	else {
		long charsleft =  0;
		
		if (fReadSave) charsleft= fReadSaveLen;
		
			// ?? how do we deal w/ this in sockets ??
			// do we need to use select(n, readfds, writefds, errfds, timeout);
			// doesn't select() set a file descripttor for async operations??
#if 0
		long readarray[1] = { fSocket };
		long maxtime = 1;
		short result= SockSelect( 1, readarray, NULL, NULL, maxtime);
		if (result > 0) charsleft++; // don't know how many...
#endif

		return charsleft;
		}
}

void DTCP::EatResponseLine(void)
{
	long bytesread;
	(void) MemFree( RecvLine()); // hanging up here !!?? in sendmail
	(void) MemFree( RecvChars( CharsAvailable(), bytesread ));	
}


long DTCP::TotalBytesReceived()
// total over life of the DTCP (since init), or multiple calls to RecvUpTo, RecvChunk ...
{
	return fBytesread; // fResultTotal;
}

long DTCP::NewBytesReceived()
// total only for last read 
{
	return fResultNew;
}

void DTCP::NullTerm( Boolean turnon)
{
	fNullTerm = turnon;
}

long DTCP::ConnectTime()
{
	if (fConnectionIsOpen) fConnectTime = GetSecs() - fStartTime;
	return fConnectTime;
}




// Receive functions ......................................


short DTCP::ReceiveData(void *data, long datasize, long &bytesReceived, Boolean stopatlf)
{
	short	err= 0;
	long startTick= GetSecs();
	long oldBytes = fBytesread;
	Boolean done	= datasize < 1;
	bytesReceived = 0;
	char*	datap		= (char*) data;
		
	long count;
	
	if (fReadSave) {
		// read from local buffer...
		count= Min( datasize, fReadSaveLen);
		if (stopatlf) {
			char* lfp= (char*) MemChr( fReadSave, kLF, count);
			if (lfp) { 
				count= lfp - fReadSave + 1;
				done= true;
				}
			}
		MemCpy( datap, fReadSave, count);
		datap += count;
		bytesReceived += count;
		datasize -= count;
		if (datasize<1) done= true;
		fReadSaveLen -= count;
		if (fReadSaveLen > 0) {
			fReadSave += count; // just push up pointer... keep fReadSaveBase around
			}
		else {
			fReadSaveBase= (char*) MemFree( fReadSaveBase);
			fReadSave= fReadSaveBase;
			fReadSaveLen= 0;
			}
		}

	while ( !done ) {
		count= SockRead( fSocket, datap, datasize);
	  if (count > 0) {
	  
			if (stopatlf) {
				char *lfp= (char*) MemChr(datap, kLF, count);
				if (lfp) {
					long newcount= lfp - datap + 1;
					if (newcount < count) {
						fReadSaveLen= count - newcount;
						fReadSaveBase=	(char*) MemDup( lfp+1, fReadSaveLen);
						fReadSave= fReadSaveBase;
						count= newcount;
						}
					done= true;
					}
				}
				
			datap 	+= count;
			bytesReceived += count;
			datasize -= count;
			if (datasize<1) done= true;
			}
		else {
			done= true; 
			if (count < 0) { err= count; fEndofMessage= true; } //??
			else if (count == 0) fEndofMessage= true; // !! need this for unix/win_motif
			}
		}
	
	fBytesread= oldBytes + bytesReceived;
		
	StreamYieldTime();
	return err;
}





		//     CAN'T RELY ON NULL TERMINATED STRINGS w/ BINARY DATA !!!
			
	
char* DTCP::ReadWithChecks( 
						ulong& bufsize,
						long 	expectedbytes/* = kTCPStopAtclose*/, 
						Boolean convertnewline/* = false*/, 
						long 	maxbytes/* = 0*/, 
						char* oldbuffer/* = NULL*/)
{
	enum { kDropChar = 22 };
	long	bytesread = 0, bufadditions = 0, newbytes= 0;
	long 	oldbytes= fBytesread;
	Boolean stopAtdotcrlf = (expectedbytes == kTCPStopAtdotcrlf);
	Boolean done = false, dodropchar= false;
	short	err = 0;
	long 	count, newcount;
	char * bufat, * from, * tob;
	long 	startTick = GetSecs();

	if (maxbytes <= 0) maxbytes= kMaxReadBuffer;
	else maxbytes= Min(maxbytes, kMaxReadBuffer);
	fResultNew= 0; //??

	if (oldbuffer == NULL) { 
		bufsize= 0; 
		oldbuffer= (char*) MemGet(1, true);
		}
	else {
		// --- NOW, assume caller passes true size of oldbuffer -- for data w/ nulls
		//bufsize= strlen(oldbuffer); // <<!!! Implies null term !!!
	  //if (bufsize && oldbuffer[bufsize-1] == 0) bufsize--;
		}
		
	gErrorManager->TurnOff();		
	do {
		count= maxbytes - newbytes; 
		if (count>0) {
			bufat= (char*) MemMore( oldbuffer, bufsize + count + 1);
			if (bufat) oldbuffer= bufat;
			else { 
				fResultNew= -1; // flag for out-of-mem
				goto finish; 		// out of mem
				}
			bufat= oldbuffer + bufsize;
			newcount = 0;

			err= ReceiveData( bufat, count, newcount, stopAtdotcrlf);
			
			if (newcount>0 && stopAtdotcrlf && (bufat[0] == '.')) {
				if (bufat[1] == kCR || bufat[1] == kLF) {
					newcount= 0;
					fEndofMessage= true;
					}
				else if (bufat[1] == '.') {
					bufat[0] = kDropChar;
					dodropchar= true;
					}
				} 
			count= newcount;
			
			bufsize  += count;
			newbytes += count;
			StreamYieldTime();
			}
	} while (err == 0 && count > 0);
	fResultNew = newbytes;
		
finish:
	gErrorManager->TurnOn();		
	
	if (dodropchar) {
		tob= from= oldbuffer;
		for (count= 0; count<bufsize; count++, from++) {
			if (*from != kDropChar)  *tob++= *from;
			}
		*tob= 0;
		bufsize= tob - oldbuffer; 
		dodropchar= false;
		}
		
	if (convertnewline && bufsize) { 
		tob= from= oldbuffer;
		
		if (LineEndSize == 2) { // what the net lineend is
			if (LineEnd[0] == kCR && LineEnd[1] == kLF) ;
			else ; // convert CRLF to LineEnd
			}
			
		else if (LineEndSize == 1) {
 
			if (*LineEnd == kLF) {
				for (count= 0; count<bufsize; count++, from++) {
					if (*from != kCR) 
						*tob++= *from;
				  else if (from[1] != kLF) 
				  	*tob++= kLF;  // fails at count==bufsize-1
				  }
				}
				
			else if (*LineEnd == kCR) {
				for (count= 0; count<bufsize; count++, from++) {
					if (*from != kLF) 
						*tob++= *from;
				  else if (from[-1] != kCR) 
				  	*tob++= kCR;   
				  }
				}

			*tob= 0;
			bufsize= tob - oldbuffer; 
			}
		}	
		
			// shrink it down to bufsize..
	oldbuffer= (char*) MemMore( oldbuffer, bufsize + 1);
	if (true) { //fNullTerm !!!
		oldbuffer[bufsize] = '\0'; // always now, no such GetPtrSize(p) for generic systems
		}

	if (err != 0)  {
#if FIX_LATER
		fEndofMessage= (Status() != kTCPestablished);
#else
		fEndofMessage= true;
#endif
		}
		
	return oldbuffer;
}




short DTCP::RecvByte()
{
	unsigned char b;
	long bytesread= 0;
	short err= ReceiveData( &b, 1, bytesread);
	if (bytesread>0) return b; 
	else return -1;			
}

 
char* DTCP::RecvLine() // read up to kLF 
{
	const	short	kBufSize = 511;
	char	buf[kBufSize+1];
 	short	err= 0;
 	
	if (!fConnectionIsOpen) return false;
	fResultSize= 0;
	fMaxResultSize= 0;
	fResultNew= fResultSize;  //save for close calculation
	fEndofMessage= false;

	long 	bytesread = 0;
	err= ReceiveData( buf, kBufSize, bytesread, true);
	if (bytesread > 0) {
		fResultSize += bytesread;
		fResultNew = fResultSize;
		buf[bytesread]= 0;
		return (char*)MemDup(buf,bytesread+1);
		}
	else
		return NULL;
}


  
char* DTCP::RecvChars(long readCount, long& numread)
{ 
	long  thisread;
	long  bufsize= readCount;
	short err= 0;
	
	numread = 0;
	if (!fConnectionIsOpen) return NULL; 
	gErrorManager->TurnOff();		
	fResultHand= (char*) MemNew(bufsize+1);
	gErrorManager->TurnOn();
	if (!fResultHand) 
		return NULL;
		
	char* p= fResultHand;
	if (readCount > 0) do {
		thisread= 0;
		err= ReceiveData( p, bufsize, thisread);
		p += thisread;
		bufsize -= thisread;
		numread += thisread;
	} while (thisread > 0 && bufsize > 0);
		
	if (true) { //fNullTerm)
		fResultHand[numread] = '\0';
		}
	return fResultHand;
}


#if FIX_MAYBE_LATER_OR_TRASH

char* DTCP::RecvChunk( long maxChunk, char* oldChunk)
{	
	Boolean done; 
	short err= 0;
	long	thisread, count, oldchunksize, newchunksize= 0;
 	// note: maxChunk >= newchunksize, oldchunksize is separate
	
	fResultNew= 0;
	fResultHand= oldChunk;
	if (!fConnectionIsOpen) return oldChunk;
	
	if (maxChunk <= 0) 
		maxChunk = kMaxReadBuffer; //maxChunk= FreeMem() - (FreeMem() / 5); 
	
	if (oldChunk == NULL) { 
		oldchunksize= 0; 
		oldChunk= (char*) MemNew(oldchunksize+1);
		oldChunk[oldchunksize]= '\0';		
		}
	else 
		oldchunksize= strlen( oldChunk); // !! this implies NULL termination for all data !

	gErrorManager->TurnOff();		
	do {
		//count= CharsAvailable();
		//if (count+newchunksize > maxChunk) count= maxChunk-newchunksize;
		count= maxChunk - newchunksize;
		if (count>0) {
			
			char* p= (char*) MemMore( oldChunk, oldchunksize + newchunksize + count + 1);
			if (p) oldChunk= p;
			else { // out of memory...
				fResultNew= -1;
				goto finish;
				}
			p= oldChunk + oldchunksize + newchunksize;
			thisread = 0;
			err= ReceiveData( p, count, thisread);
			newchunksize += thisread;
			}

		fEndofMessage= (Status() != kTCPestablished);
		if (fDoShowProgress) ShowProgress(); 
		done= (err != 0 || newchunksize >= maxChunk || thisread < 1);
		if (UserBreak()) {
			done= true;
			Fail("User break");  // ?? fail or proceed w/ short read?
			}
	} while (!(done || fEndofMessage)); 	
	fResultNew= newchunksize;
	
finish:
	gErrorManager->TurnOn();		

	if (true) { //fNullTerm) 
		oldChunk[oldchunksize + newchunksize] = '\0';
		}
	fResultHand= oldChunk;
	return oldChunk;
}

#endif

