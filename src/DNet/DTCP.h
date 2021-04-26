// DTCP.h  
// by D. Gilbert, 1991-92, with help from code of Harry Chesley, Peter Speck and others 

#ifndef __DTCP__
#define __DTCP__

#include <DObject.h>

class DTaskMaster;


extern "C" {
typedef  Boolean  (*TCPUserBreakHandler)(DTaskMaster* /* itsObject */);		
typedef  void  (*TCPMessageHandler)(DTaskMaster* , long , const char* ); /* itsObject, msgID, msg */
}


enum NetChars { kCR = '\015', kLF = '\012' };


class	DTCP : public DObject {
public:				
		enum TCPlineEndFlags { kDontAddCRLF= false, kAddCRLF = true};
		enum TCPSendFlags { kDontSendNow= false, kSendNow = true};
	
		enum TCPStatusResults { kTCPreleased = -1, kTCPclosed = 0,
			kTCPlistening = 2, kTCPopening = 4, kTCPestablished = 8,
			kTCPclosing = 16, kTCPPleaseClose = 14, kTCPUnknownState = 32,
			kTCPwaitingforopen = 64};
			
		enum TCPExpectedBytesFlags { kTCPStopAtdotcrlf = -1, kTCPStopAtclose = -2};
		
		
		DTCP();
		virtual ~DTCP();
		virtual void Initialize();

		virtual Boolean IsTCPInstalled();
		
		virtual void Fail(long errNo);
		virtual void Fail(const char* msg);
		virtual Boolean Failed();	// ???

		virtual long		Status();
		virtual char*	 	StatusString(short state);  
		virtual long		Version();  
		virtual const char*	VersionString();  //??
		
		virtual void InstallMessageHandler(TCPMessageHandler aproc, DTaskMaster* itsObject);
		virtual void InstallUserBreak(TCPUserBreakHandler aproc, DTaskMaster* itsObject);

		virtual void ShowMessage(const char* msg);
		virtual void StatusMessage();
		virtual void SetShowProgress( Boolean turnOn);
		virtual void ShowProgress( long sendRecvCount);
		virtual void ShowProgress();
		
		virtual Boolean EndOfMessage();
		virtual void	SetEndOfMessage( Boolean isEnded);
		
		virtual Boolean UserBreak();
		virtual void StreamYieldTime(); // makes TCPAbort if aborted from TThread

		virtual long NameToAddress(const char* hostName);
		virtual char* AddressToName(long address);
		virtual char* DotAddrToName(const char* dotAddress);
		virtual char* RemoteName();

		void Open( char* hostname, unsigned short hostport, unsigned short localport = 0);
		//void Open(long hostIP, unsigned short hostport, unsigned short localport = 0);

		virtual Boolean WaitedForOpen(long delayticks);
		virtual Boolean WaitedForOpen();
		virtual void Close();
		virtual void Release();
		virtual void Abort(); 

		virtual void SendBytes(void *data, long datasize, Boolean immediately = true);
		virtual void Send(char *cstring, Boolean immediately = true);
		virtual void SendCRLF( Boolean EvenIfLastSendHadCRLF = true, Boolean immediately = true);
		virtual void SendStr( char* s, Boolean addCRLF = true, Boolean immediately = true);

		virtual long CharsAvailable();
		virtual long TotalBytesReceived(); //??
		virtual long NewBytesReceived();	 //??
		virtual long ConnectTime();
		virtual	void NullTerm( Boolean turnon = true);
		virtual void EatResponseLine();
		
		virtual short ReceiveData( void *data, long datasize, long &bytesReceived,
							Boolean stopatlf = false);  
		virtual char* ReadWithChecks( ulong& bufsize, long expectedbytes = kTCPStopAtclose, 
							Boolean convertnewline = false, long maxbytes = 0, char* oldbuffer = NULL);
		virtual short RecvByte();  
		virtual char* RecvLine(); // read up to first LF
		virtual char* RecvChars( long readCount, long& numread);
#if NOT_USEFUL
		virtual char* RecvChunk( long maxChunk= 0, char* oldChunk= NULL);
#endif


protected:
		long			fSocket;					// socket descriptor
		long			fTimeout;	
		Boolean		fFailed;  				// last op failed if true
		char*			fError; 					// change to number...
		long			fErrNo;
		Boolean		fDoShowProgress;  // if true, ShowProgress 
		Boolean		fEndofMessage;		// true if recv found end-of-message (dot-cr-lf) 

		long			fBytesread;	
		long			fResultSize;			// we don't need all of these counters !!
		long			fResultNew;
		long			fResultTotal;
		long			fMaxResultSize; 
		long			fLimitResultSize; 	// recv buffer ptrs 
		char*			fResultHand;				// recv buffer 
		long			fStartTime;					// time at open connection
		long			fConnectTime;				// == current or close time - start time
		
		TCPMessageHandler fMessageProc;
		DTaskMaster*	fMessageObj;
		TCPUserBreakHandler fBreakProc;
		DTaskMaster*	fBreakObj;

		Boolean		fLastSentCRLF;	// last send ended w/ crlf
		Boolean		fNullTerm;
		
private: 
// from TMacTCP..............
		Boolean fConnectionIsOpen;
		Boolean fStreamIsOpen;
		char		fLastc,fLast2c,fLast3c;
		long		fReadSaveLen;
		char*		fReadSaveBase;
		char*		fReadSave;

};
		
		
extern	long	gTCPTimeout;		// value for fTimeout for each new TTCP

Boolean TCPIsItInstalled();
long 	 	MyIP();
char* 	GetMyDotName();
long 	  DotName2IP(const char* name);
char* 	IP2DotName(long ip);	// this one currently is broken


#endif

