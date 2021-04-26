// DGopher.cp 
// by d.g. gilbert, Mar 1992 
// version for DClap, Jan 94
/*
			This code is Copyright (C) 1992 by D.G. Gilbert.
			All Rights Reserved.

			You may use this code for your personal use, to provide a non-profit
			service to others, or to use as a test platform for a commercial
			implementation. 

			You may not use this code in a commercial product, nor to provide a
			commercial service, nor may you sell this code without express
			written permission of the author.  
			
			gilbertd@bio.indiana.edu 
			Biology Dept., Indiana University, Bloomington, IN 47405 
*/


#include <ncbi.h>
#include <dgg.h> 

#include "DTCP.h"
#include "DURL.h"
#include "DGopher.h"
#include "DGoList.h"
#include "DGoPlus.h"
#include "DGoInit.h"
#include "DGoClasses.h"

#include <DFile.h>
#include <DList.h>
#include <DIconLib.h>
#include <DView.h>
#include <DChildApp.h>


enum  commonTCPports {
	kUnknownPort = 0,
	kUnsupportedPort = 0,
	kFileport = 21,
	kFTPport = 21,
	kSMTPport = 25,
	kTelnetport = 23,
	kTN3270port = 23,
	kWhoisport= 43,
	kGopherport = 70,
	kFingerport = 79,
	kHTTPport = 80,
	kPOPport	= 110,
	kNNTPport = 119,
	kWAISport	= 210
	};

		//public variables
//Global const	short kGopherPort	= 70;  		// default/prime port #

Global const	long 	kReadMaxbuf  = 1024;

Global DIconList*	gGopherIcons = NULL;
Global short		gGopherIconID;
Global short		gIconSize;

Global Boolean	gUseDigFolder;
Global short		gDigVol;
Global long			gDigDirID;

Global Boolean	gShortFolder = false;
Global Boolean	gListUnknowns = false;
Global Boolean	gDoSuffix2MacMap = false; // !?? started bombing 26Dec94 w/ this != false


#if !defined(OS_MAC) && !defined(OS_UNIX) && !defined(OS_DOS) && !defined(OS_VMS)
Global char   *	gLocalGopherRoot = "";
Global char   *	gAskWaisPath = "";
#endif
#ifdef OS_MAC
Global char   *	gLocalGopherRoot = ":";
Global char   *	gAskWaisPath = ":bin:askwais";
#endif
#ifdef OS_UNIX
Global char   *	gLocalGopherRoot = ".";
Global char   *	gAskWaisPath = "/bin/askwais";
#endif
#if defined(OS_DOS) || defined (OS_NT)
Global char   *	gLocalGopherRoot = ".";
Global char   *	gAskWaisPath = "\bin\askwais";
#endif
#ifdef OS_VMS
// damn, what is vms syntax for local subdir ??
Global char   *	gLocalGopherRoot = "[Gopher-root]";
Global char   *	gAskWaisPath = "[bin]askwais.exe";
#endif


		//private	variables

//Local DGopher	*gCurrentGopher;

Local const char*	gEmptyString	= "";
Local const char* kDefaultLink = "1Title of gopher link\t\tHost.Name.Goes.Here\t70";	
Local const char* kLocalhost = "localhost";

Local const	long 	kReadToClose = DTCP::kTCPStopAtclose;
Local const	long 	kReadToDotCRLF = DTCP::kTCPStopAtdotcrlf;

Local const	long 	kReadMaxbuf1 = 1 * 1024; 
Local const	long 	kMaxInt = 32000;

Local const char *gMonths[13] = 
	{"Jan","Feb","Mar","Apr","May","Jun",  "Jul","Aug","Sep","Oct","Nov","Dec","???"};



// DGopherTalk ------------------------------


DGopherTalk::DGopherTalk(): DTCP()
{
	fLinesRead= 0;
	NullTerm(false);
	//InstallMessageLine( aStatusLine); 
}



void DGopherTalk::OpenQuery( char* host, short port, char* path, char* query, 
														char* viewchoice, char* plus, DFile* plusFile)
{
	void *crlfdotcrlf = (void*) CRLF"."CRLF; //"\015\012";  
	
	ShowMessage("TCP opening connection to host");
	Open((char*)host, port);
	if (Failed()) return;
	if (!WaitedForOpen(0)) return;
	if (path) Send( path, kDontSendNow); 
	if (query) Send( query, kDontSendNow); 
	if (viewchoice) Send( viewchoice, kDontSendNow); 
	
	if (!plus) {
		SendBytes(crlfdotcrlf, 5, kSendNow);	// send dot-cr-lf 
		}
		
	else {
		// if (plus) send plus data header, form depends on plusFile:
		//char *header0 = "\t+\t1\n\r+NBYTES\n\r";
		//char *header1 = "\t+\t1\n\r+-1\n\r";
		//char *header2 = "\t+\t1\n\r+-2\n\r";
	
		if (!plusFile) {
			if (StrChr(plus, kCR)) { // more than one line in plus == ASK block
				char	header[80];
				//long len = strlen(plus);
				//sprintf( header, "\t+\t1\n\r+%d\n\r", len);  // exactly <len> bytes of data
					// ^^ this isn't understood by server ?!?
#if 1
	// 1apr94 fix for bad DGoFolder selectors  
	//    bad:  path|+view|+|1<cr>ask data
	//	 good:  path|+view|1<cr>data or  path|$|1<cr>data  or  path|+|1<cr>data
				char* selector= (viewchoice) ? "" : "\t$"; // or "+";
				sprintf( header, "%s\t1"CRLF"+-1"CRLF, selector); // data until dot-cr-lf
#else
				sprintf( header, "\t+\t1"CRLF"+-1"CRLF); // data until dot-cr-lf
#endif
				Send( header, kDontSendNow);
				Send( plus, kDontSendNow);
				SendBytes(crlfdotcrlf, 5, kSendNow); 
				}
			else {
				Send( plus, kDontSendNow);
				SendBytes(crlfdotcrlf, 5, kSendNow);	 
				}
			}
			
		else {  //if (plusFile)  
			const short 	kMaxBuf = 2048;
			char	buffer[kMaxBuf];
			short	err;
			Boolean fullbuf, done= false;
			long fileType = cTEXT;
			ulong	count, lenFile, fat = 0;
			
			err = plusFile->OpenFile();
			err |= plusFile->GetDataLength( lenFile);
			err |= plusFile->GetFileType( fileType);

			if (fileType == cTEXT)  {
				/// ARrrrgggghhhh -- need to do newline translation here (at least if type==TEXT)
				char	header[80];
				sprintf( header, "\t+\t1"CRLF"+-1"CRLF); // data until dot-cr-lf
				Send( header, kDontSendNow);
				Send( plus, kDontSendNow);
				SendCRLF(false, kDontSendNow);		// ensure newline  
				while (! (done || err)) {
					count= kMaxBuf;
					err |= plusFile->ReadUntil( buffer, count, *LineEnd);
					fullbuf = (count+2 > kMaxBuf);
					if (buffer[count-1] == kCR) {
						if (fullbuf) count--;
						else buffer[count++]= kLF; // add lf;
						}
					else {
						if (!fullbuf) {
							buffer[count++]= kCR; 
							buffer[count++]= kLF; 
							} 
						}
					if (count==3 && 0==memcmp(buffer, "."CRLF, 3))  
						this->SendBytes(".."CRLF, 4, kDontSendNow); // make sure we don't send terminator by mistake
					else
						this->SendBytes(buffer, count, kSendNow); //kDontSendNow
					if (fullbuf) this->SendCRLF(kDontSendNow);
					
					err |= plusFile->GetDataMark(fat);
					done|= fat >= lenFile;
					}
				SendCRLF(false, kDontSendNow);		// ensure newline  
				SendBytes("."CRLF, 3, kSendNow);	// send dot-cr-lf 
				}
			
			else {
				char	header[80];
				long len = strlen(plus) + lenFile;
				//sprintf( header, "\t+\t1"CRLF"+%d"CRLF, len);  // exactly <len> bytes of data
						// ^^ this isn't understood by server ?!?
				sprintf( header, "\t+\t1"CRLF"+-2"CRLF); // data until close
				Send( header, kDontSendNow);
				Send( plus, kDontSendNow);
				//SendCRLF(false, kDontSendNow);		// ??! ensure newline  -- not w/ fixed bytecount...
				while (! (done || err)) {
					count= kMaxBuf;
					err |= plusFile->ReadData( buffer, count);
					this->SendBytes(buffer, count, kSendNow);
					err |= plusFile->GetDataMark(fat);
					done|= fat >= lenFile;
					}
				}
				
			 err= plusFile->CloseFile();
			}
		}
		
//  long ticktock= TickCount() + fTimeout;
//	while (!CharsAvailable() && TickCount() < ticktock && !UserBreak() )
//			StatusMessage();			
	SetShowProgress( true);
 
}


 
 
Boolean DGopherTalk::ReadALine( CharPtr appendToHandle, CharPtr& result)
{	
	result= NULL;
	fResultNew= 0;
	CharPtr data= RecvLine(); //RecvUpTo( true, kLF, NULL);  // includes lf+null at end
	//- fEndofMessage= false; << !?! RecvUpTo NO LONGER sets this true at Linefeed ?
	Boolean readone= (data!=NULL);
	fLinesRead++;
	if (readone) {
		if ( data[0] == '.' &&  data[1] == kCR) fEndofMessage= true; 
				//  ^^ this changes w/ subclasses...

				// some damn terminator patching -- always end string w/ Return, no lf or null 
		long	len= StrLen(CharPtr(data));
		//len--; //drop null, always there from RecvUpTo() << NO, 
		//if (len < kMaxInt) << do we need to stop at 32K ??
		{
			if ( data[len-1] == kLF) len--;
			if ( data[len-1] != kCR) data[len++]= *LineEnd; 
			}
		data= (char*) MemMore( data, Max(0,len));
		
		if (appendToHandle == NULL)
			appendToHandle= data;
		else {
			long oldlen= StrLen(appendToHandle);
			appendToHandle= (char*)MemMore( appendToHandle, oldlen+len+1);
			MemCopy( appendToHandle+oldlen, data, len);
			appendToHandle[oldlen+len]= 0;
			}
		}
	result= appendToHandle;
	return readone;
}


 



// DocLink ------------------------------


DocLink::DocLink() : 
	fType(kNoLink), fParag(0), fChar(0)
{
	string.fString= NULL;
}

DocLink::~DocLink()  
{
	if (fType == kString) MemFree( string.fString);
}

void DocLink::Clear() 
{ 
	if (fType == kString) MemFree( string.fString);
	fType= kNoLink; 
}
		
void DocLink::DupData()
{
	if (fType == kString) 
		string.fString= StrDup( string.fString);
}
		
char * DocLink::GetString( short& skipval) 
{ 
	skipval= string.fSkip; 
	return string.fString; 
}

void DocLink::SetString( const char* thestr, short skipvalue)
{
	Clear();
	fType= kString;
	string.fString= StrDup(thestr);
	string.fSkip= skipvalue;
}

void DocLink::SetSkip( short skipvalue) 
{ 
	if (fType == kString) string.fSkip= skipvalue; 
}

void DocLink::SetRect( Nlm_RecT therect)
{
	Clear();
	fType= kRect;
	rect.fRect= therect;
}

void DocLink::GetLine( short& atparag, short& atchar, short& endparag, short& endchar )
{
	atparag= fParag;
	atchar= fChar;
	endparag= line.fEndParag;
	endchar= line.fEndChar;
}
	
void DocLink::SetLine( short atparag, short atchar, short endparag, short endchar )
{
	Clear();
	fType= kLine;
	fParag= atparag;
	fChar= atchar;
	line.fEndParag= endparag;
	line.fEndChar= endchar;
}

void DocLink::GetLineRect( Nlm_RecT& therect, short& atparag, short& atchar )
{
	therect= rect.fRect;
	atparag= fParag;
	atchar= fChar;
}
	
void DocLink::SetLineRect( Nlm_RecT therect, short atparag, short atchar)
{
	Clear();
	fType= kLineRect;
	fParag= atparag;
	fChar= atchar;
	rect.fRect= therect;
}





// DGopherAbstract ------------------------------

class DGopherAbstract : public DObject
{
public:
	short fLines;
	long	fSize;
	char* fText;
	
	DGopherAbstract( char* text);
	~DGopherAbstract() { Clear(); }
	
	virtual DObject* Clone();
	void Set( const char* text);
	void Clear();
	void Install( const char* text) { Clear(); Set( text); }
	void Draw( Nlm_RecT& area);
	short Height();
};

DGopherAbstract::DGopherAbstract( char* text) : 
		fLines(0), fSize(0), fText(NULL) 
{ 
	Set( text); 
}

void DGopherAbstract::Clear()
{
	MemFree( fText);
	fSize= fLines= 0;
}

DObject* DGopherAbstract::Clone()
{
	DGopherAbstract* myClone= (DGopherAbstract*) DObject::Clone();
	myClone->fText= StrDup(fText);
	return myClone;
}

void DGopherAbstract::Set( const char* text)
{
	char *cp, *dp, c, lastc;
	Boolean havevis= false;
	long bytes= StringLen( text);
	if (bytes>0) {
		fText= (char*) MemNew( bytes+1);
		fLines= 1;
		for (cp= (char*)text, dp= fText, lastc= 0; *cp != 0; cp++) {
			c= *cp;
			if (c == kCR) { 
				if (havevis) { *dp++= NEWLINE;  fLines++; }
				}
			else if (c == kLF) {
				if (lastc == kCR) ; // eat c
				else if (havevis) { *dp++= NEWLINE;  fLines++; }
			  }
			else if (havevis) *dp++= c; 
			else { havevis= isgraph(c); if (havevis) *dp++= c; }
			lastc= c;
			}
		while ( fLines && dp > fText && dp[-1] == NEWLINE) { 
			 // drop trailing blank lines
			fLines--; dp--; 
			}	
	  *dp= 0;
	  fSize= dp - fText;
	  }
}

void DGopherAbstract::Draw( Nlm_RecT& area)
{
	if (fText) {
		//? Nlm_DrawString(&area, fText, 'l', false);
		Dgg_DrawTextbox( &area, fText, fSize, 'l', false);
		}
}

short DGopherAbstract::Height()
{
	if (fLines) 
		return (fLines * Nlm_LineHeight());
	else 
		return 0;
}








 
// DGopher ------------------------------


DGopher::DGopher( char ctype, const char* link) :
	DTaskMaster( 0, NULL, NULL)
{
	Initialize();
	SetLink( ctype, link);
}

DGopher::DGopher() :
	DTaskMaster( 0, NULL, NULL)
{
	Initialize();
	SetLink( *kDefaultLink, kDefaultLink);
}

DGopher::DGopher( DGopher* aGopher)  :
	DTaskMaster( 0, NULL, NULL) 
{
	Initialize();
	CopyGopher( aGopher);
}


DGopher::~DGopher()  
{
	DeleteLink(false);
	fInfo= (char*) MemFree( fInfo);
}

Boolean DGopher::suicide(void) 
{ 
	if (GetOwnerCount() <= 1) { 
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide();
}

Boolean DGopher::suicide(short ownercount)
{
	if (ownercount < 1) {
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide(ownercount);
}
 
DObject* DGopher::Clone()
{
	//DGopher* aGopher= (DGopher*)DObject::Clone();
	DGopher* aGopher= new DGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}



void DGopher::Initialize()
{
	fLink= NULL;
	fLinkLen= 0;
	fItitle= 0;
	fIdate = 0;	 
	fIsize = 0;	 
	fIpath = 0;	 
	fIhost = 0;	 
	fIport = 0;	 
	fIplus = 0; 

	fType	= kTypeError; //== ancestor doesn't know much...
	fProtocol = kGopherprot;
	fIsLocal= false;
	fPort	= kGopherport;
	fURL= NULL;
	fDate= NULL; fDateLong= 0;
	fDataSize= NULL; fSizeLong= 0;
	fInfo= NULL;
	fInfoSize= 0;
	fInfoHandler= NULL;
	
	fOwnerList= NULL;
	fStatusLine= NULL;
 
	fDeleted= false;
	fInUseCount= 1;
	fTalker= NULL;
	fMacType= cTEXT;
	fMacSire= cEDIT;
	fTransferType= kTransferText; //text/lines or binary or none ?
	fSaveToDisk= false;
	fLaunch= false;
	fQuery= NULL;
	fQueryGiven= NULL;
#if 1
	fDocLink= NULL;	//ClearDocLink();
#else
	fMenuString= NULL;
	fMenuStringSkip= 0;
	fDocLinkParag= 0;
	fDocLinkChar= 0;
	fDocLinkLen= 0;
#endif

	fCommand= cNoCommand;
	fThreadState= kThreadNotStarted;
	fBytesExpected= 0;
	fBytesReceived= 0;
	fConnectTime= 0;	
	fAppFile= NULL;
	
		// gopher+
	fShortFolder= gShortFolder; //false;
	fHasAsk= false;
	fIsMap= false;
	fHaveReply= false;
	fReplyFile= NULL;
	fReplyData= NULL;
	fIsPlus= kGopherPlusDontKnow;
	fQueryPlus= NULL;
	fAdminEmail= NULL;
	fViews= NULL;					// list of CGopherItemView
	fViewchoice= 0;
	fAskers	= NULL;
	fAbstract= NULL;
	fGoMenuBlock= NULL;
	fInfo	= (char*)Nlm_MemGet(1, true); 
	fInfoSize= 0; // fInfoSize is size of data not including terminating NULL !@!@$#@# 
}

void DGopher::CopyGopher( DGopher* aGopher)  
{
	if (aGopher) {
	// This memcpy was bad...problem was NULL aGopher !!!
	//memcpy(&fType, &aGopher->fType, sizeof(DGopher)); //<< bad for subclass w/ larger size !

	fType 	= aGopher->fType; 				 
	fItitle	= aGopher->fItitle; 
	fIdate	= aGopher->fIdate;		 
	fIsize	= aGopher->fIsize; 
	fIpath	= aGopher->fIpath; 
	fIhost	= aGopher->fIhost;	 
	fIport	= aGopher->fIport;	 
	fIplus	= aGopher->fIplus;	 
	fPort		= aGopher->fPort; 	 
	fLink		= (char*) MemDup( aGopher->fLink, aGopher->fLinkLen);
	fLinkLen= aGopher->fLinkLen;

	fDate		= (char*) StrDup( (CharPtr)aGopher->fDate);
	fDateLong	= aGopher->fDateLong;	 
	fDataSize	= (char*) StrDup( (CharPtr)aGopher->fDataSize);
	fSizeLong = aGopher->fSizeLong;	 

	fAdminEmail= (char*) StrDup( (CharPtr)aGopher->fAdminEmail);	
	fViews	= aGopher->CloneViews();
	fAskers	= aGopher->CloneAskers();
	if (aGopher->fAbstract) fAbstract = (DGopherAbstract*) aGopher->Clone(); 
	fIsPlus	= aGopher->fIsPlus;		 
	fShortFolder	= aGopher->fShortFolder; 
	fHasAsk	= aGopher->fHasAsk;		 
	fHaveReply = aGopher->fHaveReply;	 
	if (aGopher->fReplyFile) fReplyFile = (DFile*) aGopher->fReplyFile->Clone();	 
	fReplyData = StrDup( aGopher->fReplyData);		 
	if (aGopher->fGoMenuBlock) fGoMenuBlock= new DGoMenuBlock(this, aGopher->fGoMenuBlock->fData);
	else fGoMenuBlock= NULL;

	fProtocol= aGopher->fProtocol; 	 
	fURL		= (char*) StrDup( (CharPtr)aGopher->fURL);
	fIsLocal= aGopher->fIsLocal;
	fIsMap	= aGopher->fIsMap;		 

	MemFree( fInfo); 
	fInfoSize= aGopher->fInfoSize;
	fInfo= (char*)MemDup( aGopher->fInfo, fInfoSize+1);
	fInfoHandler= aGopher->fInfoHandler; //??
	
	fQuery = aGopher->fQuery;			 
	fQueryPlus = aGopher->fQueryPlus;	
	fQueryGiven = StrDup( aGopher->fQueryGiven);		 
	
	//fTransferType	= aGopher->fTransferType;  
	//fDeleted= false;
	//fMacType	= aGopher->fMacType; 			// PRESERVE for CopyToNewKind
	//fMacSire	= aGopher->fMacSire;			// PRESERVE for CopyToNewKind
	//fSaveToDisk	= aGopher->fSaveToDisk;	// PRESERVE for CopyToNewKind
	//fLaunch		= aGopher->fLaunch;				// PRESERVE for CopyToNewKind
	//fCommand	= aGopher->fCommand; 			// PRESERVE for CopyToNewKind
	//fViewchoice	= aGopher->fViewchoice;	// PRESERVE for CopyToNewKind
	//fOwnerList= NULL; //!? 
	fStatusLine= aGopher->fStatusLine; 
	//fTalker= NULL;		//? 
	fBytesExpected= aGopher->fBytesExpected;
	//fBytesReceived= aGopher->fBytesReceived;
	//fConnectTime= aGopher->fConnectTime;
	//fThreadState= aGopher->fThreadState; 
	//fAppFile = NULL; //??
	//fInUseCount= 1;
	fDocLink= aGopher->fDocLink;
	if (fDocLink) fDocLink->DupData();
	}
}

void DGopher::DeleteInfo()
{
	if (fInfo) fInfo= (char*) MemMore( fInfo, 1);
	else fInfo= (char*) MemNew(1);
	*fInfo= 0;
	fInfoSize= 0;
}

void DGopher::DeleteViews()  
{
	if (fViews) {
		fViews->FreeAllObjects();
		fViews->suicide();
		fViews= NULL;
		}
}

DList* DGopher::CloneViews()  
{
	DList* newViews= NULL;
	if (fViews) {
		newViews= new DList();
		long i, n= fViews->GetSize();
		for (i= 0; i<n; i++) newViews->InsertLast( fViews->At(i)->Clone());
		}
	return newViews;
}


void DGopher::DeleteAskers()  
{
	if (fAskers) {
		fAskers->FreeAllObjects();
		fAskers->suicide();
		fAskers= NULL;
		}
}

DList* DGopher::CloneAskers()  
{
	DList* newAskers= NULL;
	if (fAskers) {
		newAskers= new DList();
		long i, n= fAskers->GetSize();
		for (i= 0; i<n; i++) newAskers->InsertLast( fAskers->At(i)->Clone());

		}
	return newAskers;
}



void DGopher::DeleteLink(Boolean onlyInfoline)  
{
	fLink	=  (char*) MemFree( fLink);
	fLinkLen= 0;
		// zero the indices into flink
 	fItitle = fIdate= fIsize= fIpath= fIhost= fIport= fIplus = 0;

	if (!onlyInfoline) {	
		if (fDate) fDate	= (char*)MemFree( (CharPtr)fDate);	
		if (fDataSize) fDataSize= (char*)MemFree( (CharPtr)fDataSize);
		if (fAppFile) fAppFile= (char*)MemFree( fAppFile);
		if (fAdminEmail) fAdminEmail	= (char*)MemFree( (CharPtr)fAdminEmail);	
		if (fGoMenuBlock) { delete fGoMenuBlock; fGoMenuBlock= NULL; }
		if (fAbstract) { delete fAbstract; fAbstract= NULL;  }
		if (fQueryGiven) fQueryGiven= (char*)MemFree( fQueryGiven);
#if 1
		if (fDocLink) delete fDocLink; fDocLink= NULL;
#else
		if (fMenuString) fMenuString= (char*)MemFree( fMenuString);
#endif

		DeleteViews();
		DeleteAskers();
		}
}




Boolean DGopher::HasDocLink()
{
	return (fDocLink && fDocLink->HasLink());
	//return (fDocLinkParag || fDocLinkChar || fDocLinkLen);
}

#if 0
void DGopher::GetDocLink( short& atParagraph, short& atChar, short& length)
{
	atParagraph= fDocLinkParag;
	atChar= fDocLinkChar;
	length= fDocLinkLen;
}

void DGopher::SetDocLink( short atParagraph, short atChar, short length)
{
	fDocLinkParag= atParagraph;
	fDocLinkChar= atChar;
	fDocLinkLen= length;
}

void DGopher::ClearDocLink()
{
	fDocLinkParag= 0;
	fDocLinkChar= 0;
	fDocLinkLen= 0;
}
#endif



void DGopher::AddView( const char* viewkind)
{
	if (!fViews) fViews = new DList();
	DGopherItemView* giv= new DGopherItemView( viewkind);
	fViews->InsertLast( giv);
}


void DGopher::SetPlusInfo( short isPlus, Boolean hasAsk, char* plus)
{
	char *cp, *ep, ec, ecl, *sp, *sep, cep;
	Boolean done= false;
	Boolean isHTMLform= false;
	
#define FindNextPlus(cp)	 { \
	for (ep=cp,ecl=*(cp-1),ec=*ep; ec!=0 && !((ec=='.' || ec=='+') && ecl<' '); ) \
		{ecl= ec; ec= *(++ep);} }
	
	fIsPlus= isPlus;
	fHasAsk= hasAsk;
	ClearAskReply();
	if (!plus) return;

	cp= plus;
	while (*cp!=0) {
		while (*cp!='+' && *cp!=0) cp++;
		
		if (0==Strncasecmp(cp, "+ADMIN:",7)) {
			cp += 7;
			FindNextPlus(cp);
			while (cp < ep) {
				while (*cp <= ' ' && *cp!=0 && cp<ep) cp++; // skip whitespace, newlines
				if (0==Strncasecmp(cp, "Admin:",6)) {
					// Admin: Don Gilbert  <archive@bio.indiana.edu>
					MemFree((CharPtr)fAdminEmail);
					sp = cp + 6;
					for (sep= sp; *sep >= ' ' && sep<ep; sep++) ; // skip over other words
					cep= *sep; *sep= 0;
					fAdminEmail= (char*)StrDup(sp);
					*sep= cep;
					}
					
				else if (0==Strncasecmp(cp, "Mod-Date:",9)) {
					sp= StrChr(cp,'<');
					if (sp) {
							// Mod-Date: Thu Feb  4 13:46:47 1993 <19930204134647>
							// convert <19930204134647> to 04Feb93
						char	sdate[50];
						sp++;
						// save complete date for sorts, etc.
						sep= sp+10; cep= *sep; *sep= 0;
						fDateLong= atol(sp);
						*sep= cep;

						sdate[0]= 0;
						sep= sp+6; //day
						strncat(sdate, sep, 2);
						
						sep= sp+6; cep= *sep; *sep= 0;
						long i= atol(sp+4);  // month
						*sep= cep;
						if (i<1 || i>12) strncat(sdate, gMonths[12], 3);
						else strncat(sdate, gMonths[i-1], 3);
						strncat( sdate, sp+2, 2); // year
						MemFree((CharPtr)fDate);
						fDate= (char*) StrDup( sdate);
						}
					}
				while (*cp >= ' ' && *cp!=0 && cp<ep) cp++; // skip over other words
				}
			} // +ADMIN
			
		else if (0==Strncasecmp(cp, "+VIEWS:",7)) {
			cp += 7;
			FindNextPlus(cp);
			DeleteViews();
			fViews = new DList();
			while (cp < ep) {
				while (*cp <= ' ' && *cp!=0 && cp<ep) cp++; // skip whitespace, newlines
				if (*cp!=0 && cp<ep) {
					for (sep= cp; *sep >= ' ' && sep<ep; sep++) ; // skip over other words
					cep= *sep; *sep= 0;
					DGopherItemView* giv= new DGopherItemView( cp);
					fViews->InsertLast( giv);
					if (fDataSize==NULL) {
						fSizeLong= atol( (char*) giv->DataSize());
						char snum[50];
						if (fSizeLong) sprintf( snum, "%luk", fSizeLong);
						else strcpy(snum,"<1k");
						fDataSize= (char*) StrDup( snum);
						}
					*sep= cep;
					cp= sep;
					}		
				}
			} // +VIEWS
			
		else if (0==Strncasecmp(cp, "+ASKHTML:",9)) {
			// this format is like +ASK except each item has HTML name tag immediately
			// after ask kind:  " Choose:name:label\tchoice1\tchoice2"
			// " Ask:name2:label2\tstring"
			cp += 9; 
			isHTMLform= true;
			goto HandleAskForm;
			}
			
		else if (0==Strncasecmp(cp, "+ASK:",5)) {
			cp += 5;
			isHTMLform= false;
HandleAskForm:
			FindNextPlus(cp);
			DeleteAskers();
			fAskers = new DList;
			while (cp < ep) {
				while (*cp <= ' ' && *cp!=0 && cp<ep) cp++; // skip whitespace, newlines
				if (*cp!=0 && cp<ep) {
					for (sep= cp; (*sep>=' ' || *sep== '\t') && sep<ep; sep++) ; // skip over other words
					cep= *sep; *sep= 0;
					DGopherItemAsk* gia= new DGopherItemAsk( cp, isHTMLform);
					if (gia && gia->fKind == kGopherAskUnknown) delete gia;
					else fAskers->InsertLast( gia);
					*sep= cep;
					cp= sep;
					}		
				}
			}  //+ASK
			
		else if (0==Strncasecmp(cp, "+ABSTRACT:",10)) {
			cp += 10;
			FindNextPlus(cp);
			cep= *ep; *ep= 0;
			if (fAbstract) fAbstract->Install( cp); 
			else fAbstract= new DGopherAbstract( cp); 
			*ep= cep;
			} //+ABSTRACT

		else if (0 == Strncasecmp( cp, "+URL:", 5) ) {
			cp += 5;
			FindNextPlus(cp);
			//if (fURL) MemFree( fURL);
			cep= *ep; *ep= 0;
#if 0
			if ( DURL::ParseURL( this, cp, ep-cp) ) ;
#else
			char* saven= StrDup( (char*)this->GetName());
			if ( DURL::ParseURL( this, cp, ep-cp) ) ;
			if (saven && *saven) this->StoreName(saven); 
			MemFree(saven);
#endif
			*ep= cep;
			}

		else if (0==Strncasecmp(cp, "+ISMAP",6)) {  // dgg addition
			cp += 6;
			FindNextPlus(cp);
			fIsMap= true;
// having problems w/ other settings !?!?
// ?? need special map type ?
			fIsPlus= kGopherPlusYes;
			fTransferType= kTransferBinary; 
			//fCommand= cNewGopherText; // ???
			} //+ISMAP
			
		else if (0==Strncasecmp(cp, "+MENU:",6)) { // dgg addition
			cp += 6;
			//FindNextPlus(cp);
			Boolean found= false;
			ep= cp; 
			ecl= *(cp-1);
			ec= *ep; 
			while (!found) {
				ecl= ec; 
				++ep;
				ec= *ep;
				if (ec == 0) found= true;
				else if ((ecl == kCR || ecl == kLF) && (ec == '+' || ec == '.')) found= true;
				}  

			if (fGoMenuBlock) delete fGoMenuBlock;
			cep= *ep; *ep= 0;
			fGoMenuBlock= new DGoMenuBlock(this, cp);
			*ep= cep;
			} //+MENU
			
		// +MENUSTRING:
		else if (0==Strncasecmp(cp, "+MENUSTRING:",12)) { // dgg addition
			cp += 12;
			while (*cp && *cp <= ' ') cp++;
			if (*cp == '"') {
				cp++; ep= cp;
				while (*ep && *ep != '"') ep++;
				}
			else if (*cp == '\'') {
				cp++; ep= cp;
				while (*ep && *ep != '\'') ep++;
				}
			else {
				ep= cp;
				while (*ep && *ep > ' ') ep++;
				}
			cep= *ep; *ep= 0;
			//if (fMenuString) MemFree(fMenuString);
			//fMenuString= StrDup(cp);
			if (!fDocLink) fDocLink= new DocLink();
			fDocLink->SetString( cp);
			*ep= cep;
			
				// look for skip value
			cp= ep; if (*cp) cp++;
			while (*cp && *cp <= ' ') cp++;
			if (isdigit(*cp)) 
				fDocLink->SetSkip( atol(cp)); 
				//fMenuStringSkip= atol( cp);
			
			FindNextPlus(cp);
			}

			// +MENULINE:  #parag  #startchar  #endparag  #stopchar
		else if (0==Strncasecmp(cp, "+MENULINE:",10)) { // dgg addition
			cp += 10;
			short  atpar, atchar, endpar, endchar;
#if 1
			while (*cp && *cp <= ' ') cp++;
			atpar= atol( cp);
			while (isdigit(*cp)) cp++; while (*cp && *cp <= ' ') cp++;
			atchar= atol( cp);
			while (isdigit(*cp)) cp++; while (*cp && *cp <= ' ') cp++;
			endpar= atol( cp);
			while (isdigit(*cp)) cp++; while (*cp && *cp <= ' ') cp++;
			endchar= atol( cp);
#else
	// bad, skipping 1st number !!
			sscanf( cp, "%d%d%d%d", &atpar, &atchar, &endpar, &endchar);
#endif
			if (!fDocLink) fDocLink= new DocLink();
			fDocLink->SetLine( atpar, atchar, endpar, endchar);
			FindNextPlus(cp);
			}

			// +MENURECT:  #left  #top  #right  #bottom
		else if (0==Strncasecmp(cp, "+MENURECT:",10)) { // dgg addition
			short  left, top, right, bottom;
			Nlm_RecT	r;
			cp += 10;
#if 1
			while (*cp && *cp <= ' ') cp++;
			left= atol( cp);
			while (isdigit(*cp)) cp++; while (*cp && *cp <= ' ') cp++;
			top= atol( cp);
			while (isdigit(*cp)) cp++; while (*cp && *cp <= ' ') cp++;
			right= atol( cp);
			while (isdigit(*cp)) cp++; while (*cp && *cp <= ' ') cp++;
			bottom= atol( cp);
#else
	// bad, skipping 1st number !!
			sscanf( cp, "%d %d %d %d ", &left, &top, &right, &bottom);
#endif
			Nlm_LoadRect( &r, left, top, right, bottom);
			if (!fDocLink) fDocLink= new DocLink();
			fDocLink->SetRect(r);
			FindNextPlus(cp);
			}

			// +MENULINERECT:  #parag #startchar #left  #top  #right  #bottom
		else if (0==Strncasecmp(cp, "+MENULINERECT:",14)) { // dgg addition
			short  atpar, atchar, left, top, right, bottom;
			Nlm_RecT	r;
			cp += 14;
#if 1
			while (*cp && *cp <= ' ') cp++;
			atpar= atol( cp);
			while (isdigit(*cp)) cp++; while (*cp && *cp <= ' ') cp++;
			atchar= atol( cp);
			while (isdigit(*cp)) cp++; while (*cp && *cp <= ' ') cp++;
			left= atol( cp);
			while (isdigit(*cp)) cp++; while (*cp && *cp <= ' ') cp++;
			top= atol( cp);
			while (isdigit(*cp)) cp++; while (*cp && *cp <= ' ') cp++;
			right= atol( cp);
			while (isdigit(*cp)) cp++; while (*cp && *cp <= ' ') cp++;
			bottom= atol( cp);
#else
	// bad, skipping 1st number !!
			sscanf( cp, "%d %d %d %d %d %d ", 
						&atpar, &atchar, &left, &top, &right, &bottom);
#endif
			Nlm_LoadRect( &r, left, top, right, bottom);
			if (!fDocLink) fDocLink= new DocLink();
			fDocLink->SetLineRect(r, atpar, atchar);
			FindNextPlus(cp);
			}

		else if (0==Strncasecmp(cp, "+QUERYSTRING:",13)) { // dgg addition
			cp += 13;
			while (*cp && *cp <= ' ') cp++;
			if (*cp == '"') {
				cp++; ep= cp;
				while (*ep && *ep != '"') ep++;
				}
			else if (*cp == '\'') {
				cp++; ep= cp;
				while (*ep && *ep != '\'') ep++;
				}
			else {
				ep= cp;
				while (*ep && *ep > ' ') ep++;
				}
			cep= *ep; *ep= 0;
			if (fQueryGiven) MemFree(fQueryGiven);
			fQueryGiven= StrDup(cp);
			*ep= cep;
			FindNextPlus(cp);
			}
			
		else if (0==Strncasecmp(cp, "+INFO:",6)) {
			cp += 6;
			// this probably means we messed up packaging plus data for this call
			// ?? should we exit/fail ??
			FindNextPlus(cp);
			} //+INFO
						
		else {
			cp++;
			FindNextPlus(cp);
			} // +OTHER  +COMMENT
			
		cp= ep;	
		}			

#undef FindNextPlus	
}
 

void DGopher::SetLink( char ctype, const char* link)
{
	// link data expected is pointer to standard gopher info line:
	// 0Title<tab>path<tab>host<tab>port<tab>plus...etc...<null | cr-lf>
	char	*cp, *cp0;

	DeleteLink(true); // clean out any old link info

	fLinkLen= StrLen( link) + 1;
	fLink = (char*) MemDup( (void*)link, fLinkLen);
	cp= cp0= fLink;
	fType= ctype; //= *cp;  -- we may be overriding link type
	fItitle= 1; // title always starts at 2nd char in link
	cp= StringChr(cp, '\t'); if (cp) { *cp++= 0; fIpath= cp-cp0; } else fIpath= 0;
	cp= StringChr(cp, '\t'); if (cp) { *cp++= 0; fIhost= cp-cp0; } else fIhost= 0;
	cp= StringChr(cp, '\t'); if (cp) { *cp++= 0; fIport= cp-cp0; } else fIport= 0;
	cp= StringChr(cp, '\t'); if (cp) { *cp++= 0; fIplus= cp-cp0; } else fIplus= 0;

	fIsLocal= ( fIhost==0 
							|| StrICmp(fLink+fIhost,kLocalhost) == 0  
							|| StrICmp(fLink+fIhost,gEmptyString) == 0 );
	
	if (fIport) fPort= (short)atol( cp0 + fIport); else fPort= kGopherport; // ???
		
	// look for Date+Size in title as
	// "File listing  [14Apr92, 58kb]" is an example 
	cp= cp0 + fItitle;
	char *dp= StringRChr(cp, ']');
	if (dp) {
		char *sz= dp-3;
		if (isdigit(*sz) && sz[1] == 'k' && sz[2] == 'b') {
			char *db= StrRChr(cp, '[');
			if (db && db < dp) {
				char *te= db-1; // terminate title
				while (te > cp0 && isspace(*te)) te--;
				*(te+1)= 0;
				db++; //*db++= 0; 
				fIdate= db - cp0;
				while (isdigit(*sz)) sz--;
				fIsize= sz+1 - cp0;
				while (db<sz && *db!=',') db++;
				*db= 0; // terminate date
				*dp= 0; // terminate size
				if (!fDateLong) { 
					long yr = atol(cp0+fIdate+5); 
					long day= atol(cp0+fIdate);
					long mon= 0; 
					while (mon<12 && strncmp(gMonths[mon], cp0+fIdate+2, 3) != 0) mon++;
					fDateLong= 1900000000 + yr * 1000000 + mon * 10000
								+ day * 100; 
					}
				if (!fSizeLong) {
					fSizeLong= atol(cp0 + fIsize);
					char snum[50];
					if (fSizeLong) sprintf( snum, "%luk", fSizeLong);
					else strcpy(snum,"<1k");
					if (fDataSize) MemFree(fDataSize);
					fDataSize= (char*) StrDup( snum);
					}
				}
			}
		}
}





void DGopher::ToText( CharPtr& h, short indent)
{	
	
	if (TRUE) { // if (fIsPlus) 
		char quote, *bufp, *buf, *plus;
			// RISKY having buf at fixed maxlen!, we need better way than sprintf(fixedbuf,...) !!
		long len= MAX( fLinkLen+1, 2048); 
		buf = (char*)MemNew(len);
		if (indent<0) indent= 0;
		if (indent) MemFill( buf, ' ', indent);
		bufp= buf+indent;
		
		if (fIsPlus) plus= "\t+"; else plus= "";
		sprintf( bufp, "+INFO: %c%s\t%s\t%s\t%d%s\n",
				fType, (char*) GetName(), (char*) GetPath(), (char*) GetHost(), fPort, plus);
		StrExtendCat( &h, buf);

		if (fURL && fProtocol != kGopherprot) {
			//if (StrChr(fURL, '"')) quote= '\''; else quote= '"';
			quote= ' ';
			sprintf( bufp, "+URL: %c%s%c\n", quote,fURL,quote); 
			StrExtendCat( &h, buf);
			}
		
		// must store +MENU, +MENUSTRING, +QUERYSTRING and possibly other G+ block info
		// do we care to save +ADMIN, +VIEWS, +ASK, ... ???

		if (HasDocLink()) {
			short atparag, atchar;
			switch ( fDocLink->Kind()) {
							
			case DocLink::kNoLink:
				break;
				
			case DocLink::kLine: {
				short endparag, endchar;
				fDocLink->GetLine( atparag, atchar, endparag, endchar);
				sprintf( bufp, "+MENULINE: %d %d %d %d\n",
								 atparag, atchar, endparag, endchar); 
				StrExtendCat( &h, buf);
				}
				break;

			case DocLink::kLineRect: {
				Nlm_RecT r;
				fDocLink->GetLineRect( r, atparag, atchar);
				sprintf( bufp, "+MENULINERECT: %d %d %d %d %d %d\n",
									atparag, atchar, r.left, r.top, r.right, r.bottom); 
				StrExtendCat( &h, buf);
				}
				break;

			case DocLink::kRect: {
				Nlm_RecT r;
				fDocLink->GetRect( r);
				sprintf( bufp, "+MENURECT: %d %d %d %d\n",
									 r.left, r.top, r.right, r.bottom); 
				StrExtendCat( &h, buf);
				}
				break;
				
			case DocLink::kString: {
				short skipval = 0;
				char* links= fDocLink->GetString( skipval);
				if (StrChr(links, '"')) quote= '\''; else quote= '"';
				if (skipval)
					sprintf( bufp, "+MENUSTRING: %c%s%c %d\n", quote, links, quote, skipval); 
				else
					sprintf( bufp, "+MENUSTRING: %c%s%c\n", quote,links,quote); 
				StrExtendCat( &h, buf);
				}
			break;
			
			}
	
		}
			
		if (fQueryGiven) {
			if (StrChr(fQueryGiven, '"')) quote= '\'';
			else quote= '"';
			sprintf( bufp, "+QUERYSTRING: %c%s%c\n", quote,fQueryGiven,quote); 
			StrExtendCat( &h, buf);
			}
			
		if (fGoMenuBlock) {
			// ...a bit of recursion...
			char* menutext= fGoMenuBlock->fGolist->WriteLinks(indent+1);
			sprintf(bufp, "+MENU:\n");
			StrExtendCat( &h, buf);
			StrExtendCat( &h, menutext);
			MemFree( menutext);
			}

		MemFree(buf);
		}
	else {
		char *buf = (char*)MemNew(fLinkLen+1);
		sprintf( buf, "%c%s\t%s\t%s\t%d\n",
				fType, (char*) GetName(), (char*) GetPath(), (char*) GetHost(), fPort);
		StrExtendCat( &h, buf);
		MemFree(buf);
		}
}


void DGopher::ToServerText( CharPtr& h, short itemNum)
{
	long	maxlen= 7 * 6 + fLinkLen + 600; // is this safely more than we need?
	char 	*buf= new char[ maxlen];
	char	*cp= buf;
	char  quote, cplus = (fIsPlus == kGopherPlusYes) ? '+' : ' ';
	char*	stmp;
		
	sprintf( cp, "Name=%s%s", (char*) GetName(),LineEnd); cp += strlen(cp);  
	if (itemNum) { sprintf( cp, "Numb=%d%s", itemNum,LineEnd); cp += strlen(cp); }
	sprintf( cp, "Type=%c%c%s", fType, cplus,LineEnd); cp += strlen(cp);
	sprintf( cp, "Host=%s%s", (char*) GetHost(),LineEnd);  cp += strlen(cp);  
	sprintf( cp, "Port=%d%s", fPort,LineEnd); cp += strlen(cp);
	sprintf( cp, "Path=%s%s", (char*) GetPath(),LineEnd); cp += strlen(cp);
	StrExtendCat( &h, buf);
	
	if (fAdminEmail) { 
		sprintf( buf, "Admin=%s%s", fAdminEmail,LineEnd);  
		StrExtendCat( &h, buf);
		}
	stmp= (char*)GetDate(); 
	if (stmp && *stmp) {
		sprintf(buf, "#+Mod-Date:%s%s", stmp,LineEnd);
		StrExtendCat( &h, buf);
		}

	if (fURL) {
		//if (StrChr(fURL, '"')) quote= '\''; else quote= '"';
		quote= ' ';
		sprintf( buf, "+URL: %c%s%c\n", quote,fURL,quote); 
		StrExtendCat( &h, buf);
		}

#if 0
	if (fIsPlus == kGopherPlusYes) {
		sprintf( buf, "#+Views:%s",LineEnd);
		StrExtendCat( &h, buf);
		CArrayIterator iter( fViews);
		void* 	iview;
		for (iview = iter.First(); iter.More(); iview = iter.Next())  {
			stmp= (*(DGopherItemView**) iview)->ViewVal();
			sprintf( buf, "# %s%s",  (char*)stmp,LineEnd);
			StrExtendCat( &h, buf);
			}
		}
#endif

	sprintf( buf, "#%s",LineEnd); 
	StrExtendCat( &h, buf);
	delete[] buf;
}



void DGopher::ToPrettyText( CharPtr& h, short /*itemNum*/,
				Boolean showDate, Boolean showSize, Boolean showKind, Boolean showPath, 
				Boolean showHost, Boolean showPort, Boolean showAdmin)
{
	char*	stmp;
	long	maxlen= 8 * 6 + fLinkLen + 600; // is this safely more than we need?
	char 	*buf= new char[ maxlen];
	char	*cp= buf;
	char cplus = (fIsPlus == kGopherPlusYes) ? '+' : ' ';
	
#define SprintIfReal(WHAT)	\
	if (stmp && *stmp) sprintf( cp, "\t%s", stmp); else sprintf( cp, "\t"); \
	cp += strlen(cp); 	
	
	stmp= (char*)GetName(); sprintf( cp, "%s", stmp); cp += strlen(cp); 
	if (showDate) { stmp= (char*)GetDate(); SprintIfReal("Date"); }
	if (showSize) { stmp= (char*)GetDataSize(); SprintIfReal("Size"); }
	if (showKind) { sprintf( cp, "\t%c%c", fType, cplus); cp += strlen(cp); }
	if (showPath) { stmp= (char*)GetPath(); SprintIfReal("Path"); }
	if (showHost) { stmp= (char*)GetHost(); SprintIfReal("Host"); }
	if (showPort) { sprintf( cp, "\t%d", fPort); cp += strlen(cp); }
	if (showAdmin && fAdminEmail) { stmp= fAdminEmail; SprintIfReal("Admin"); }
	sprintf( cp, "%s",LineEnd); cp += strlen(cp);
	StrExtendCat( &h, buf);
	
	delete[] buf;
}

void DGopher::StoreName(char* aStr)
{
	long len= 100 + fLinkLen + StrLen(aStr);
	char *link = (char*) MemNew(len);
	char cplus = (fIsPlus == kGopherPlusYes) ? '+' : ' ';
	sprintf( link, "%c%s\t%s\t%s\t%d\t%c",
		fType, aStr/*(char*) GetName()*/, (char*) GetPath(), (char*) GetHost(), fPort, cplus);
	SetLink( fType, link);
	MemFree( link);
}

void DGopher::StorePath(char* aStr)
{
	long len= 100 + fLinkLen + StrLen(aStr);
	char *link = (char*) MemNew(len);
	char cplus = (fIsPlus == kGopherPlusYes) ? '+' : ' ';
	sprintf( link, "%c%s\t%s\t%s\t%d\t%c",
		fType, (char*) GetName(), aStr/*(char*) GetPath()*/, (char*) GetHost(), fPort, cplus);

	SetLink( fType, link);
	MemFree( link);
}

void DGopher::StoreHost(char* aStr)
{
	long len= 100 + fLinkLen + StrLen(aStr);
	char *link = (char*) MemNew(len);
	char cplus = (fIsPlus == kGopherPlusYes) ? '+' : ' ';
	if (!aStr || !*aStr || StrICmp(aStr,gEmptyString) == 0) 
		aStr= (char*)kLocalhost;
	sprintf( link, "%c%s\t%s\t%s\t%d\t%c",
		fType, (char*) GetName(), (char*) GetPath(), aStr/*(char*) GetHost()*/, fPort, cplus);
	SetLink( fType, link);
	MemFree( link);
	// done in SetLink: fIsLocal= (StrICmp(aStr,kLocalhost) == 0 ); // fProtocol == kFileprot && 
}

void DGopher::StorePort(char* aStr)
{
	long len= 100 + fLinkLen + StrLen(aStr);
	char *link = (char*) MemNew(len);
	char cplus = (fIsPlus == kGopherPlusYes) ? '+' : ' ';
	sprintf( link, "%c%s\t%s\t%s\t%s\t%c",
		fType, (char*) GetName(), (char*) GetPath(), (char*) GetHost(), aStr/*fPort*/, cplus);
	SetLink( fType, link);
	MemFree( link);
}


// static
short DGopher::StandardPort( short protocol)
{
	switch (protocol) {
		case kGopherprot: return kGopherport;
		case kHTTPprot	: return kHTTPport;
		case kSMTPprot	:	return kSMTPport;
		case kFileprot	: return kFileport;
		case kFTPprot		: return kFTPport;
		case kTelnetprot: return kTelnetport;
		case kTN3270prot: return kTN3270port;
		case kWhoisprot	: return kWhoisport;
		case kFingerprot: return kFingerport;
		case kPOPprot		: return kPOPport;
		case kNNTPprot	: return kNNTPport;
		case kWAISprot	: return kWAISport;

		case kUnsupportedProt: return kUnsupportedPort;
		default:
		case kUnknownProt: return kUnknownPort;
		}
}

void DGopher::StoreProtocol( short protocol)
{
	fProtocol= protocol;
	fPort= StandardPort( fProtocol);
	switch (fProtocol) {	
			
		case  kGopherprot	:  
		case  kHTTPprot		:  
		case  kFTPprot		: 
		case  kFileprot		: 
			// need to flag if localhost !? -- for all prots ??
			fIsLocal= (StrICmp(GetHost(),kLocalhost) == 0 
							|| StrICmp(GetHost(),gEmptyString) == 0 );
			break;
			
			// currently unsupported
		case  kNNTPprot		: 
		case  kWAISprot		:  
		case  kTelnetprot	:  
		case  kTN3270prot	: 
		case  kUnknownProt:
		case  kUnsupportedProt:
			fType= kTypeError; // ??
			break;
		}
}


const char* DGopher::GetProtocol()
{
#if 0
	short item= DGopher::GetProtoItem(fProtocol);
	if (item<0) return "unknown://";
	return DGopher::GetProtoName(item);
#else
	switch (fProtocol) {
		case  kNNTPprot		: return "news:"; 
		case  kSMTPprot		: return "mailto:"; 
		case  kGopherprot	: return "gopher://"; 
		case  kHTTPprot		: return "http://"; 
		case  kFTPprot		: return "ftp://";  // should be same as file:// ??
		case  kFileprot		: return "file://"; 
		case  kWAISprot		: return "wais://"; 
		case  kTelnetprot	: return "telnet://"; 
		case  kTN3270prot	: return "tn3270://"; 
		
		case  kUnknownProt: 
		case  kUnsupportedProt: 
		default: 
				return "unknown://";				
		}
#endif
}

void DGopher::StoreURL(char* aStr)
{
	fURL= (char*) MemFree(fURL); 
	if (aStr) fURL= StrDup(aStr);
}

const char* DGopher::GetURL()
{
//	sprintf( buf, "URL: gopher://%s:%d/%c/%s%s", 
//					(char*) GetHost(), fPort, fType, (char*) GetPath(),LineEnd);
	if (!fURL) {
		char cstore[512];
		char* url= StrDup( GetProtocol());
		StrExtendCat( &url, (char*) GetHost());
		if (fPort != StandardPort(fProtocol)) {
			sprintf( cstore, ":%d", fPort);
			StrExtendCat( &url, cstore);
			}
		
		char* epath= DURL::EncodeChars( GetPath());
		switch (fProtocol) {
			case kGopherprot:
				sprintf( cstore, "/%c%s", fType,epath);
				StrExtendCat( &url, cstore);
				break;
				
		 	case kFileprot:
				if (*epath != '/') {
					StrExtendCat( &url, "/");
					// ?? do some fiddling w/ *path ???
					}
			default:
				StrExtendCat( &url, epath);
				break;
			}
			
		MemFree( epath);
		
		if (fQueryGiven) {
			StrExtendCat( &url, "?");
			StrExtendCat( &url, fQueryGiven);
			}
			
		fURL= url;
		}
	return fURL;
}


void DGopher::StorePlus(char *plus)
{
	long len= 10 + fLinkLen + 1;
	char *link = (char*) MemNew(len);
	//char cplus = (fIsPlus == kGopherPlusYes) ? '+' : ' ';
	sprintf( link, "%c%s\t%s\t%s\t%d\t%c",
		fType, (char*) GetName(), (char*) GetPath(), (char*) GetHost(), fPort, *plus);
	SetLink( fType, link);
	MemFree( link);
}

void DGopher::StorePlus(Boolean isplus)
{
	if (isplus) { 
		fIsPlus = kGopherPlusYes; 
		StorePlus("+");
		}
	else { 
		fIsPlus = kGopherPlusNo; 
		StorePlus("");
		}
}



Boolean DGopher::Edit(long dialogID)
{
	Boolean			result= false;

	DGopherEditWindow *aWind= new DGopherEditWindow(this, dialogID != 0);
	result= aWind->PoseModally();
	delete aWind;
	// aWind handles all of rest -- okay button causes this gopher to be updated
	
	return result;
}


Boolean DGopher::Equals(DGopher* other)
{
	if (!other) return false;
	else if (fType != other->fType) return false;
	else if (fPort != other->fPort) return false;
	else if (fProtocol != other->fProtocol) return false;
	else if (StringCmp( fLink+fIhost, other->fLink+other->fIhost)) return false;
	else if (StringCmp( fLink+fIpath, other->fLink+other->fIpath)) return false;
	else if (StringCmp( fQueryGiven, other->fQueryGiven)) return false;
	else return true;
}

Boolean DGopher::operator ==(DGopher* other)
{
	return Equals(other);
}
			

void DGopher::SwapPathType(char oldtype, char newtype)
{
	// this is very risky as protocol says path is opaque to client -- should drop.
	// want it to set binary/text transfer protocol for server
	if (*(fLink+fIpath) == oldtype) *(fLink+fIpath)= newtype;
}


void DGopher::SetOwner( DGopherList* OwnerList)  
{
	fOwnerList= OwnerList;
	if (fOwnerList) fStatusLine= fOwnerList->fStatusLine; // ?? is this only use of fOwnerList?
	gNeedTypeChange= false;
	
	if (gDoSuffix2MacMap && gGopherMap) {
		DGopherMap* mapper;
		mapper= gGopherMap->MatchGopherType( this);
		if (mapper) gNeedTypeChange |= mapper->Map(this);
		mapper= gGopherMap->MatchSuffix( this);
		if (mapper) gNeedTypeChange |= mapper->Map(this);
		}
}



const char* DGopher::GetKindName() 
{ 
	return "Unknown"; 
}

const char* DGopher::GetName()   
{
	if (fItitle) return  fLink + fItitle;
	else return gEmptyString;
}

const char* DGopher::GetDate()   
{
	if (fDate) return fDate;
	else if (fIdate) return  fLink + fIdate;
	else return gEmptyString;
}

const char* DGopher::GetDataSize()   
{
	if (fDataSize) return fDataSize;
	else if (fIsize) return fLink + fIsize;
	else return gEmptyString;
}

const char* DGopher::GetPath()   
{
	if (fIpath) return fLink + fIpath;
	else return gEmptyString;
}

const char* DGopher::GetHost()   
{
	if (fIhost) return fLink + fIhost;
	else return gEmptyString;
}

const char* DGopher::GetPort()   
{
	if (fIport) return fLink + fIport;
	else return gEmptyString;
}

const char* DGopher::GetPlus()   
{
	if (fIplus) return fLink + fIplus;
	else return gEmptyString;
}


const char* DGopher::ShortName()   
{
	static char  shorty[33];
	if (fItitle) {
		strncpy(shorty, fLink + fItitle, 32);
		shorty[33]= 0;
		return shorty;
		}
	else return gEmptyString;
}

void DGopher::ReadALine() 
// this is here for easy subclassing to process line as read thru talker 
{
	char *data = NULL;

	if (fIsLocal && fReplyFile) {
		char dline[1024];
		ulong datalen= 1024;
		fReplyFile->ReadLine( dline, datalen);
		if (datalen>0) {
			dline[datalen]= 0;
			if (fInfo == NULL) {
				fInfo= (char*)MemDup(dline, datalen+1);
				fInfoSize= datalen;
				}
			else {
				MemExtendCat( &fInfo, fInfoSize, dline, datalen+1); // +1 for 0
				fInfoSize += datalen;
				}
			}

		}
	else if (fTalker && fTalker->ReadALine( NULL, data)) {
								// this changes w/ subclasses...
		if ( data[0] == '.' && data[1] == kCR ) 
			fTalker->SetEndOfMessage(true);
		if (!fTalker->EndOfMessage()) {
			long datalen= StrLen(data);
			if (fInfo == NULL) {
				fInfo= data;
				fInfoSize= datalen;
				}
			else {
				MemExtendCat( &fInfo, fInfoSize, data, datalen+1); // +1 for 0
				fInfoSize += datalen;
				MemFree( data);
				}
			}
		}
}


Boolean DGopher::EndOfMessage()
// this is here for easy subclassing to process line as read thru talker 
{
	return fTalker->EndOfMessage();
}



void DGopher::ShowMessage(const char* msg)
{
	if (fStatusLine) fStatusLine->SetTitle( (char*)msg);
}

void DGopher::ShowProgress()
{
	char snum[40], buf[128];
	Dgg_LongToStr( fBytesReceived, snum, 0, 40);
	sprintf( buf, "Bytes sent: %s", snum);
	long csec = fConnectTime; // time / 60;
	if (csec > 0) {
		char buf2[128];
		csec= fBytesReceived / csec;
		Dgg_LongToStr( csec, snum, 0, 40);
		sprintf( buf2, "%s, bytes/sec: %s", buf, snum);
		ShowMessage( buf2);
		}
	else
		ShowMessage( buf);
}



void DGopher::FinishRead()
{
}

const char* DGopher::GetViewChoiceKind()
{
	if (fViewchoice > 0 && fViews && (fViews->GetSize()>0)) {
		DGopherItemView* giv= (DGopherItemView*) fViews->At(fViewchoice-1);
		if (giv) return giv->Kind();
		}
	return NULL; // or use gGopherMap & fType to return default Kind ??
}

void DGopher::CheckViewMappers()
{
	if (fViews) {		
		DGopherItemView* giv;
		unsigned short maxrank= 0;
		long i, maxat= 0, nviews= fViews->GetSize();
		char* cp;
		for (i= 0; i<nviews; i++) {
			char * viewkind;
			unsigned short lastrank = 0; 
			giv= (DGopherItemView*) fViews->At(i);
			viewkind= (char*)giv->Kind();
			DGopherMap* aMapper= gGopherMap->MatchHandlerKind( lastrank, viewkind); 
			if (aMapper) {
				giv->fViewStatus= lastrank;
				if (lastrank > maxrank) { maxrank= lastrank; maxat= i; }
				}
			else
				giv->fViewStatus= DGopherItemView::kUnknownStatus;
			}
		giv= (DGopherItemView*) fViews->At(maxat);
		giv->fViewStatus= DGopherItemView::kMaxStatus;
		if (maxrank>0) fViewchoice= maxat + 1; //???
		}
}



#if 0

// LocalGopher.....

types of calls to handle locally:

	for all calls
		x-- if StrChr( GetPath(), '/') && !defined(OS_UNIX)
		   do filesystem path translation
		x-- must tack on gGopherRoot to top of path ??
		
  for any app call
  	x-- add -v view/choice cmdline param ?
  
  if (path == "R123-456-path/to/data")
  	x-- handle data file subrange
  	
	if (fType == kTypeFolder) 
		|| StrICmp( viewchoice,"application/gopher-menu") == 0
		|| StrICmp( viewchoice,"application/gopher+-menu")==0
		)
		-- do local-filesystem directory to gopher menu translation
		-- some of these types may be to call apps instead (exec+, ?query)
		
  if (fQuery) 
  	-- assume path is to local executable? 
  	x-- may be path wais index.{src,dct,inv} instead !?
  	   ?? can we handle this with call to apps/askwais -d path/to/index ??
  	x-- call DChildApp::Launch() w/ fQuery part of cmdline
  	
  if (fType == kTypeQuery)
  	x-- assume path is to local executable? 
  	x-- may be path wais index.{src,dct,inv} instead !?
		-- should always have (fQuery) ?

	else if (fTypes == othertype)
		 
		 kTypeFile -- read & display local file 
		 kTypeBinary 
		 kTypeSound  
		 kTypeImage 
		 kTypeMovie 
		 kTypeGif		 
		 kTypeHtml   
		 others -- no special handling

  if (StrNICmp( GetPath(), "exec+:", 6)==0)  
  	x-- assume path is to local executable
  	x-- cut cmdline params from exec+:params:/path/to/app
  	x-- call DChildApp::Launch()  
  	
  if (fQueryPlus)
  	-- this is typically either "\t+" or "\t$" or "\t!" or esp. fReplyData
    -- this is of low importance?
    
  if (fHaveReply) // for ASK forms, 
  	x-- see fQueryPlus, but also may have fReplyFile
  	x-- assume path is to app, call DChildApp::Launch()
   
#endif




  
void DGopher::LocalQuery( char* viewchoice)
{
	enum actions { kDoNothing, kShowFile, kShowFileSection, kShowFolder, 
								kLaunchApp, kLaunchWais };
	short action = kDoNothing, kind;
	char * Stdinbuf = NULL, * Stdinfile= NULL;
	char * pathname = StrDup( (char*) GetPath()); 
	char * cmdline  = StrDup("");
	char * shortpath = pathname, * thepath= pathname;
	long	 rootlen= 0;
	char * cp, * dp;
	long   rstart = 0, rend = 0;
	ulong filelen = 0;
	
 	fTalker= NULL;
	fBytesExpected= 0; //kReadToClose;
	fBytesReceived= 0;
	fConnectTime= 0;

	if (!viewchoice && (fType == kTypeHtml || fProtocol == kHTTPprot)) {
		viewchoice= "text/html";
		}

	if (viewchoice) {
		StrExtendCat( &cmdline, " -v ");
		StrExtendCat( &cmdline, viewchoice);
		}

  if (StrNICmp( pathname, "exec+:", 6)==0) {
  	char * cmds, * ep;
  	cmds= pathname+6;
  	ep= StrChr( cmds, ':');
  	if (ep) {
  		*ep++= 0;
			StrExtendCat( &cmdline, " ");
			 // ! this is risky, but how else do we handle paths in the cmdline ??
			DFileManager::UnixToLocalPath( cmds); 
  		StrExtendCat( &cmdline, cmds);
  		Nlm_MemMove( pathname, ep, 1+StrLen(ep));
  		}
		action= kLaunchApp;	 
		}
	else if (*pathname == 'R' 
		&& isdigit(pathname[1])
		&& (cp= StrChr(pathname, '-')) != NULL
		&& isdigit(cp[1])
		&& (dp= StrChr(cp+1, '-')) > cp ) { 
			//handle doc subrange paths like 'R851270-852774-:Flybase:genes:genes.txt'
			rstart= atol( pathname+1);
			rend  = atol( cp+1);
			dp++;
  		Nlm_MemMove( pathname, dp, 1+StrLen(dp));
			action= kShowFileSection;
			}
		
#if 0
  else if (StrNICmp( pathname, "waissrc:", 6)==0) {
		// ?? kLaunchWais on local wais src ??
		}
#endif		

	kind= DFileManager::FileOrFolderExists( pathname);
	if (kind==0) {
		DFileManager::UnixToLocalPath( pathname);
		kind= DFileManager::FileOrFolderExists( pathname);
		if (kind==0) {
			StrPrependCat( &pathname, gLocalGopherRoot);
			rootlen= StrLen(gLocalGopherRoot);
			shortpath= pathname + rootlen;
			thepath= pathname;
			kind= DFileManager::FileOrFolderExists( pathname);
			}
		}
	
	if (kind == DFileManager::kIsFolder) {
		fType= kTypeFolder; // ??
		action= kShowFolder;
		}	
		
	if (action == kDoNothing && fType == kTypeQuery && fQuery) {
		if ( DFileManager::FileExists(pathname) )
			action= kLaunchApp;
		else if ( DFileManager::FileExists(shortpath) ) {
			thepath= shortpath;
			action= kLaunchApp;
			}
		else {
			char path2[512];
			StrNCpy( path2, pathname, sizeof(path2));
			StrNCat( path2, ".src", sizeof(path2));
			if (DFileManager::FileExists( path2)) {
				action= kLaunchWais;
				// add default cmdline for AskWais...
#if 1
				sprintf( path2, " -+i -d %s", pathname);
#else
				sprintf( path2, " -+i -g %s -d %s", gLocalGopherRoot, pathname);
#endif
				StrExtendCat( &cmdline, path2); 
				}
			else if (DFileManager::FileExists( path2+rootlen)) {
				action= kLaunchWais;
#if 1
				sprintf( path2, " -+i -d %s", shortpath);
#else
				sprintf( path2, " -+i -g %s -d %s", gLocalGopherRoot, shortpath);
#endif
				StrExtendCat( &cmdline, path2); 
				}
			}
		}
 
	if (fHasAsk && fHaveReply) {
		action= kLaunchApp;	 
		if (fQueryPlus) Stdinbuf= fQueryPlus;
		else if (fReplyFile) Stdinfile= (char*) fReplyFile->GetName();
	 	}
	else if (fQueryPlus) {
		// typically either "\t+" or "\t$" or  "\t!"
		}

	if (fQuery) {
		*fQuery= ' '; // note: *fQuery == '\t', change to blank for cmdline
		StrExtendCat( &cmdline, " ");
		StrExtendCat( &cmdline, fQuery);
		if (action == kDoNothing) action= kLaunchApp; //?? is just fQuery enough to act this way?
		}
	
	if (action == kDoNothing)  switch (fType) {
		case kTypeFile  :  
		case kTypeBinary:  
		case kTypeSound :  
		case kTypeImage : 
		case kTypeMovie : 
		case kTypeGif		:  
		case kTypeHtml  :
			action= kShowFile; 
			break;  
		case kTypeFolder  :
			action= kShowFolder; 
			// ?? some subcall if fQueryPlus == $ or + or ! ??
			break;  
		}

#if 0
	// ???
	if (action == kDoNothing && ( viewchoice && 
	    (StrICmp( viewchoice,"application/gopher-menu")==0
		|| StrICmp( viewchoice,"application/gopher+-menu")==0 )
		) ) {
		action= kShowFolder;
		}
#endif

	if (fQueryPlus && StrCmp(fQueryPlus,"\t!")==0) {
			// drop info calls for now ?
		action= kDoNothing;
		}
		
	switch (action) {
			
		case kShowFileSection: {
			fReplyFile= new DFile( thepath, "rb"); // don't forget "b" binary, or will fail w/ newline translation for bin files !!
		  fReplyFile->Open("rb");
			//fReplyFile->GetDataLength(filelen); 
			fReplyFile->Seek( rstart);
			fBytesExpected= rend - rstart + 1; // ?? +1 ??  
			}
			break;

		case kShowFile: {
			fReplyFile= new DFile( thepath, "rb"); // don't forget "b" binary, or will fail w/ newline translation for bin files !!
		  fReplyFile->Open("rb");
			fReplyFile->GetDataLength(filelen); 
			fBytesExpected= filelen;  
			}
			break;
			
		case kShowFolder: {
#if 0
	//debug
#include <DApplication.h>
			char* stdoutfile= StrDup("Gopup.localfolder");
			StrPrependCat( &stdoutfile, 
				(char*)DFileManager::PathOnlyFromPath( gApplication->Pathname()));
			fReplyFile= new DFile(stdoutfile);  
#else
			fReplyFile= new DTempFile(); // disk file will be removed on delete
#endif
		  fReplyFile->Open("w");
			DGopherList::LocalFolder2GopherList( fReplyFile, thepath);
		  fReplyFile->Open("r");
			fReplyFile->GetDataLength(filelen); 
			fBytesExpected= filelen;  
			}
			break;
			
		case kLaunchApp: {
			DChildApp* child= new DChildApp( thepath, cmdline, true, Stdinfile, true); 
			if (Stdinbuf) 
				child->AddInputBuffer( DChildFile::kStdin, Stdinbuf, StrLen(Stdinbuf));
			if (!child->Launch())
				Message(MSG_OK, "Failed to launch %s with '%s'", thepath, cmdline);
			}
			break;
			
		case kLaunchWais: {
			if (DFileManager::FileExists( gAskWaisPath))
				thepath= gAskWaisPath;
			else {
				MemFree( pathname);
			  pathname= StrDup( gLocalGopherRoot);
				StrExtendCat( &pathname, gAskWaisPath );
				thepath= pathname;
				}
#if 0
	// debugging
#include <DApplication.h>
			char* stdoutfile= StrDup("Gopup.stdout");
			StrPrependCat( &stdoutfile, 
				(char*)DFileManager::PathOnlyFromPath( gApplication->Pathname()));
			DChildApp* child= new DChildApp( thepath, cmdline, false, NULL, true); 
			DChildFile* cfile= new DChildFile( stdoutfile, DChildFile::kStdout,
										DChildFile::kDontDelete, DChildFile::kOpenText, "w");
			child->AddFile(cfile);
			MemFree(stdoutfile);
#else
			DChildApp* child= new DChildApp( thepath, cmdline, true, NULL, true); 
#endif
			if (!child->Launch())
				Message(MSG_OK, "Failed to launch %s with '%s'", thepath, cmdline);
			}
			break;
			
		case kDoNothing:
		default:
			fBytesExpected= 0;
			break;
		}
				
	fQueryPlus= NULL; // set to zero after each call?
	fQuery= NULL;

	MemFree( pathname);
	MemFree( cmdline);
}
 



void DGopher::OpenQuery()
{
	char* viewchoiceptr = NULL;
	char	viewchoicestore[256];
	
	if (fViewchoice > 0)
	 if (fViews && fViews->GetSize()>1) {
		DGopherItemView* giv= (DGopherItemView*) fViews->At(fViewchoice-1);
		if (giv) {
			strcpy( viewchoicestore, giv->ViewRequest());
			viewchoiceptr= viewchoicestore;
			if (fQueryPlus && (0 == StrCmp(fQueryPlus,"\t+") || 0 == StrCmp(fQueryPlus,"\t$")))
				fQueryPlus= NULL; // !! fix bug of sending <tab>+ after <tab>+viewchoice !!
			}
		}
		
	DeleteInfo();
	
	fThreadState= kThreadStarted;
	if (fIsLocal) {
		LocalQuery( viewchoiceptr);
		return;
		}

	fTalker= new DGopherTalk();  
	switch (fProtocol) {

		case kFingerprot: 
		case kWhoisprot: 
		case kGopherprot:
			fTalker->OpenQuery( fLink+fIhost, fPort, fLink+fIpath, fQuery, 
											viewchoiceptr, fQueryPlus, (fHaveReply)?fReplyFile:NULL);
			break;

		case kHTTPprot: {
			char * postdata = NULL;
			char * httppath= DURL::EncodeChars( GetPath());  // ?? possible double-encoding?
			StrPrependCat( &httppath, "GET ");
			if (fQuery) {
				*fQuery= '?'; // note: *fQuery == '\t', change to '?' for http
				StrExtendCat( &httppath, fQuery);
				}
			if (fHaveReply) {
				Boolean isHTMLPostForm= (fHasAsk == 5);
				//Boolean isHTMLForm= (fHasAsk == 3 || fHasAsk == 5);
				//fQueryPlus= fReplyData;   << this is set in ReadTask
				if (isHTMLPostForm) {
					postdata= fReplyData;
					}
				else { // GET form
					StrExtendCat( &httppath, "?");
					StrExtendCat( &httppath, fReplyData);
					}
				}
				
			fTalker->OpenQuery( fLink+fIhost, fPort, httppath, NULL, 
											NULL, postdata, NULL);
			MemFree(httppath);
			}
			break;
		
			
		case kFTPprot: 	// file:///path:to:file
		case kFileprot: 
		//?? kCSOprot:
		case kWAISprot:
		case kTelnetprot:
		case kTN3270prot:
		case kPOPprot:
		case kNNTPprot:
		case kSMTPprot:
		default:
			break;
			
		}
	fQueryPlus= NULL; // set to zero after each call?
	fQuery= NULL;
	fBytesExpected= kReadToClose;
	fBytesReceived= 0;
	fConnectTime= 0;

	// If this is gopher+ server, then it will send first line as one of these
	//	which we must eat here...
	//	+12345<crlf>		number of bytes of data
	//	+-1<crlf>				data to dot-cr-lf
	//	+-2<crlf>				data to close-of-connection

	//if (fIsPlus != kGopherPlusNo) // << this is bad -- do w/o and trust checks of +##, --1 to work
	if (fProtocol == kGopherprot) 
	{ 
		char* aLine= fTalker->RecvLine();
		if (aLine) {
		
			if (*aLine == '+' && (strlen(aLine) < 20 || fIsPlus == kGopherPlusYes)) {
				fBytesExpected = atol( aLine+1);
				DeleteInfo(); // zero it...
					// bug in kTCPStopAtdotcrlf, switch it to kTCPStopAtclose for now...
				if (fBytesExpected == kReadToDotCRLF) fBytesExpected= kReadToClose;
				else if (fBytesExpected > 0) fTransferType= kTransferBinary; //?? always?
				MemFree( aLine);
				}
				
			else if ( StrStr( aLine,"--1") == aLine) {
						////  error message:
						//--1
						//1 Don Gilbert <Archive@Bio.Indiana.Edu>
						//0- Cannot access that directory
						//  1== Item is not available, 2 == Try again later, 3 == Item has moved...
				char* serror = (char*)gEmptyString;
				char* smail= fTalker->RecvLine();
				if (smail) {
					long len= strlen(smail);
					if (smail[len] == kCR) smail[len]= 0;
					serror= fTalker->RecvLine();
					if (serror) {
						len= strlen(serror);
						if (serror[len] == kCR) serror[len]= 0;
						}
					}
				//CloseQuery();
				DeleteInfo(); // zero it...
				Nlm_Message (MSG_ERROR, "Gopher server error: %s %s", smail, serror);
				fBytesExpected= 0;
				fThreadState= kThreadDoneReading; // need an error state ??
				fTalker->SetEndOfMessage(true);
				MemFree( aLine);
				return;
				}
				
			else {
				//long linelen= StrLen(aLine);
				//MemExtendCat( &fInfo, fInfoSize, aLine, linelen+1);
				MemFree( fInfo);
					// patch for binary data w/ nulls
				fInfoSize= fTalker->NewBytesReceived();
				fInfo= aLine; // aLine is made new in RecvLine()
				if ( MemChr(aLine, 0, fInfoSize-1) != NULL)  
					fTransferType= kTransferBinary; //kTransferText;
				}
			}
		}
}


void DGopher::CloseQuery()
{
	if (fIsLocal) {
		if (fReplyFile) delete fReplyFile;
		fReplyFile= NULL;
		}
	else if (fTalker) {
		fBytesReceived= fTalker->TotalBytesReceived();
		fConnectTime= fTalker->ConnectTime();
		delete fTalker; 
		}
	ClearAskReply();  
	fTalker= NULL; 
	fThreadState= kThreadDoneReading;
}


void DGopher::ReadAChunk( long maxchunk)
{
  //fThreadState= kThreadStarted;
	if (fIsLocal && fReplyFile) {
		char* data;
		ulong datalen;
	
		if (maxchunk <= 0 || maxchunk > 32000) maxchunk = 32000;
		data = (char*)MemNew( maxchunk+1);
		datalen= MIN( fBytesExpected - fBytesReceived, maxchunk);
		fReplyFile->ReadData(data, datalen);
		if (datalen>0) {
			data[datalen]= 0;
			if (fInfo == NULL || fInfoSize == 0) {
				if (fInfo) MemFree( fInfo);
				fInfo= data; data= NULL; //fInfo= (char*) MemDup(data, datalen+1);
				fInfoSize= datalen;
				}
			else {
				MemExtendCat( &fInfo, fInfoSize, data, datalen+1); // +1 for 0
				fInfoSize += datalen;
				}
			fBytesReceived += datalen;
			}
		MemFree( data);
		if (fReplyFile->EndOfFile() || datalen<maxchunk) 	
			fThreadState= kThreadDoneReading;
		}
	else if (fTalker) {
		fInfo= fTalker->ReadWithChecks(fInfoSize, fBytesExpected, (fTransferType == kTransferText), 
										maxchunk, fInfo);
		}
	//fThreadState= kThreadDataOld;
}


void DGopher::ReadTask() 
{	
	fThreadState= kThreadNotStarted; // ! need to reset this somewhere for used gophers
	if (fHasAsk && fHaveReply) {  // && fThreadState == kThreadNotStarted
		fQueryPlus= fReplyData;  
		}
	OpenQuery();
	if (fThreadState == kThreadDoneReading) return;
	DTask* readTask= newTask(cRead, kGopherTask);
	readTask->SetRepeat(true);
	PostTask(readTask);
}

void DGopher::InfoTask() 
{	
	fThreadState= kThreadNotStarted; // ! need to reset this somewhere for used gophers
	fQueryPlus= "\t!"; 
	DGopher::OpenQuery(); // bypass class overrides of OpenQuery, which set fQueryPlus !!
	if (fThreadState == kThreadDoneReading) return;
	DTask* aTask= newTask(cInfo, kGopherTask);
	aTask->SetRepeat(true);
	PostTask(aTask);
}

Boolean DGopher::IsMyTask(DTask* theTask) 
{
	if (theTask->fKind == kGopherTask) {
		ProcessTask( theTask);
		return true;
		}
	else 
		return DTaskMaster::IsMyTask(theTask);
}

void DGopher::ProcessTask(DTask* theTask) 
{
	Boolean more;
	if (theTask->fNumber == cRead) {
		if (fIsLocal)
			more= (fReplyFile && !fReplyFile->EndOfFile() 
						&& (fBytesExpected - fBytesReceived)>0);
		else
			more= (fTalker && !fTalker->EndOfMessage());
		if (more)
			ReadAChunk(kReadMaxbuf1);
		else {
			theTask->SetRepeat(false);
			FinishRead();
			CloseQuery();
			}
		}
		
	else if (theTask->fNumber == cInfo) {
		if (fIsLocal)
			more= false;
		else
			more= (fTalker && !fTalker->EndOfMessage());
		if (more) 
			ReadAChunk(kReadMaxbuf1);
		else {
			theTask->SetRepeat(false);
			FinishRead();
			CloseQuery();
			
			if (fInfo && fInfoSize > 0) {
				if (*fInfo == '+') fIsPlus = kGopherPlusYes;
				else fIsPlus= kGopherPlusNo;
				}
			SetPlusInfo( fIsPlus, fHasAsk, fInfo); // is fInfo safe for use? seems okay
			
			char *info= (char*) MemGet(1,true);
			this->ToServerText( info, 0);
			
			if (fIsPlus == kGopherPlusYes) {
				// tack on all but first +INFO: line
				char *cp, *ep;
				char endbuf[6];
				StrNCpy( endbuf, LineEnd, 4);
				StrCat( endbuf, "#");
				cp = StrChr(fInfo,*LineEnd);
				while (cp) {
					cp++;
					ep= StrChr( cp, *LineEnd);
					if (ep) *ep= 0;
					StrExtendCat( &info, endbuf);
					StrExtendCat( &info, cp);
					cp= ep; if (cp) cp += LineEndSize;
					}
				StrExtendCat( &info, LineEnd);
				}
				
			MemFree( fInfo);
			fInfo= info;
			fInfoSize= StrLen(info);
			fThreadState= kThreadDoneReading;
			}
		}
}







short DGopher::CommandResult()
{
		// ?? mapping of command due to mapping of these values ?? see CGopherMap
		return fCommand;
}


void DGopher::SetAskReply( CharPtr data, DFile* replyFile)
{
	if (fHasAsk && (data || replyFile)) {
		fHaveReply= true;
		fReplyData= data;
		fReplyFile= replyFile;
		}
}

void DGopher::ClearAskReply()
{
	if (fReplyData) {
		fReplyData= (char*) MemFree( fReplyData);
		}
	//fReplyFile= (DFile*) FreeIfObject( fReplyFile); !!NO, leave this to creator of DFile !
	fReplyFile= NULL;
	fHaveReply= false;
}



short DGopher::ThreadProgress(short /*update*/)
{
	if (fTalker) {
		fBytesReceived= fTalker->TotalBytesReceived();
		fConnectTime= fTalker->ConnectTime();
		}
	return fThreadState;
}


short DGopher::ItemForm(short viewchoice)
{
			// we don't want to repeat mapping here and in DoIt() !!
	if (fViews && (fViews->GetSize()>0) && gGopherMap) {
		if (viewchoice<0) fViewchoice= 0; else fViewchoice= viewchoice;
		DGopherMap* mapper= gGopherMap->MatchHandlerKind( this);
		if (mapper) mapper->Map(this);
		}

	fViewchoice= viewchoice;
	if (viewchoice == kGetItemInfo) 
		return cGopherGetInfo;
	else if (fHasAsk) {
		if (fHaveReply)  
			return this->CommandResult();
		else  
			return cNewGopherAsk;
		}
	else
		return this->CommandResult();		
}


//Local ??
inline void GoDrawLine( Nlm_RecT& area, char* str)
{
	if (str) {
		Nlm_DrawString(&area, str, 'l', false);
#ifndef WIN_MAC
			// XWin/MWin DrawString doesn't set NlmPoint (GetPoint fails to move)
		Nlm_MoveTo( area.left + Nlm_StringWidth(str), area.top);
#endif
		}
}

void DGopher::DrawTitle( Nlm_RecT& area)  
{
	::GoDrawLine( area, fLink + fItitle);
}

short DGopher::WidthTitle()  
{
	if (fLink) return Nlm_StringWidth( fLink + fItitle);
	else return 0;
}


void DGopher::DrawAbstract( Nlm_RecT& area)  
{
	if (fAbstract) fAbstract->Draw( area);
}

short DGopher::HeightAbstract()  
{
	if (fAbstract) return fAbstract->Height();
	else return 0;
}


void DGopher::DrawAdmin( Nlm_RecT& area)  
{
	::GoDrawLine( area, fAdminEmail);
}

short DGopher::WidthAdmin()  
{
	if (fAdminEmail) return Nlm_StringWidth( fAdminEmail);
	else return 0;
}


void DGopher::DrawPlus( Nlm_RecT& area)  
{
	char* str= NULL;
	if (fIsMap) str= "#";
	else if (fHasAsk) str= "?";
	else if (fIsPlus >= kGopherPlusYes) str= "+";
	::GoDrawLine( area, str);
}

short DGopher::WidthPlus()  
{
 return Nlm_StringWidth("?");
}


void DGopher::DrawKind( Nlm_RecT&  area)  
{
	::GoDrawLine( area, (char*) GetKindName());
}

short DGopher::WidthKind()  
{
	return Nlm_StringWidth( (char*)GetKindName());
}


void DGopher::DrawDate( Nlm_RecT& area)  
{
	char* str= NULL;
	if (fDate) str= fDate;
	else if (fIdate) str= fLink + fIdate;
	::GoDrawLine( area, str);
}

short DGopher::WidthDate()  
{
	if (fDate) return Nlm_StringWidth( fDate);
	else if (fIdate) return Nlm_StringWidth( fLink + fIdate);
	else return 0;
}


void DGopher::DrawSize( Nlm_RecT& area)  
{
	char* str= NULL;
	if (fDataSize) ::GoDrawLine( area, fDataSize);
	else if (fIsize) ::GoDrawLine( area, fLink + fIsize);
}

short DGopher::WidthSize()  
{
	if (fDataSize) return Nlm_StringWidth( fDataSize);
	else if (fIsize) return Nlm_StringWidth( fLink + fIsize);
	else return 0;
}

void DGopher::DrawPath( Nlm_RecT& area)  
{
 if (fIpath) ::GoDrawLine( area, fLink + fIpath);
}

short DGopher::WidthPath()  
{
	if (fIpath) return Nlm_StringWidth( fLink + fIpath);
	else return 0;
}


void DGopher::DrawHost( Nlm_RecT& area)  
{
	if (fIhost) ::GoDrawLine( area, fLink + fIhost);
}

short DGopher::WidthHost()  
{
	if (fIhost) return Nlm_StringWidth( fLink + fIhost);
	else return 0;
}

void DGopher::DrawPort( Nlm_RecT& area)  
{
	char snum[256];
	Dgg_LongToStr( fPort, snum, 0, 255);
	::GoDrawLine( area, snum);
}

short DGopher::WidthPort()  
{
	char snum[256];
	Dgg_LongToStr( fPort, snum, 0, 255);
	return Nlm_StringWidth(snum);
}

void DGopher::DrawURL( Nlm_RecT& area)  
{
	::GoDrawLine( area, (char*)GetURL());
}


void DGopher::DrawIconSub( Nlm_RecT& area) 
{
	short idplus;
	DIcon* ico= NULL;
	switch (gIconSize) {
		case 0 : idplus= 20000; break;
		case 1 : idplus= 10000; break;
		case 2 : 
		default: idplus= 0; break;
		}
		
	if (gGopherIcons) ico= gGopherIcons->IconById( gGopherIconID + idplus); 
	Nlm_InsetRect( &area,1,1); //? do we really want this?
	if (ico) ico->Draw(area);
}

void DGopher::DrawIcon( Nlm_RecT& area, short size)  
{
	gGopherIconID= kDefaultIcon;
	gIconSize= size;
	DrawIconSub( area);
}

short DGopher::WidthIcon(short size)  
{
	switch (size) {
		case 0 : return 16;  //12 + 4
		case 1 : return 20;  //16 + 4
		case 2 : return 36; 
		default: return 36; 
		}
}






struct goTypeRec {
	char kind;
	const char* name;
	Boolean supported:1;
	};

Local goTypeRec gGopherTypes[] = {
	{kTypeFile, "Document", true},
	{kTypeFolder, "Folder", true},
	{kTypeQuery, "Query", true},
	{kTypeBinary, "Binary file", true},
	{kTypeImage, "Image", true},
	{kTypeSound, "Sound", true},
	{kTypeMovie, "Movie", true},
	{kTypeNote, "Note", true},
	{kTypeHtml, "HTML document", true},
	{kMailType, "Mailto", true},
	{kTypeTelnet, "Telnet link", false},
	{kTypeTn3270, "TN3270 link", false},
#if 0	
	{kTypeCSO, "CSO Phonebook", false},
	{kTypeWhois, "Whois Phonebook", false},
	{kTypeBinhex, "Binhex file", false},
	{kTypeUuencode, "Uuencoded file", false},
#endif
	{kTypeError, "Unknown type", true},
	{0,0} 
	};


//static
const char* DGopher::GetTypeName( short listitem)
{
	if (listitem>=0 && listitem < (sizeof(gGopherTypes) / sizeof(goTypeRec)))
		return gGopherTypes[listitem].name;
	else
		return NULL;
}

char DGopher::GetTypeVal( short listitem)
{
	if (listitem>=0 && listitem < (sizeof(gGopherTypes) / sizeof(goTypeRec)))
		return gGopherTypes[listitem].kind;
	else
		return 0;
}

Boolean DGopher::GetTypeSupport( short listitem)
{
	if (listitem>=0 && listitem < (sizeof(gGopherTypes) / sizeof(goTypeRec)))
		return gGopherTypes[listitem].supported;
	else
		return 0;
}

short DGopher::GetTypeItem( char theType)
{
	short i;
	for (i=0; i < (sizeof(gGopherTypes) / sizeof(goTypeRec)); i++) {
		if (gGopherTypes[i].kind == theType) return i;
	  }
	return -1;
}



Local goTypeRec gProtoTypes[] = {
	{ DGopher::kGopherprot, "gopher://", true },
	{ DGopher::kHTTPprot, "http://", true },
	{ DGopher::kFileprot, "file://", true },
	{ DGopher::kFTPprot, "ftp://", true },
	{ DGopher::kSMTPprot,"mailto:", true },
	{ DGopher::kWAISprot, "wais://", false },
	{ DGopher::kTelnetprot, "telnet://", false },
	{ DGopher::kTN3270prot, "tn3270://", false },
	{ DGopher::kNNTPprot, "news:", false },
	{ DGopher::kFingerprot, "finger:", true },
	{ DGopher::kWhoisprot, "whois:", true },
	{ DGopher::kUnsupportedProt, "unsupported://", true },
	{ DGopher::kUnknownProt, "unknown://", true },
	{0,0}
	};

//static
const char* DGopher::GetProtoName( short listitem)
{
	if (listitem>=0 && listitem < (sizeof(gProtoTypes) / sizeof(goTypeRec)))
		return gProtoTypes[listitem].name;
	else
		return NULL;
}

char DGopher::GetProtoVal( short listitem)
{
	if (listitem>=0 && listitem < (sizeof(gProtoTypes) / sizeof(goTypeRec)))
		return gProtoTypes[listitem].kind;
	else
		return 0;
}

Boolean DGopher::GetProtoSupport( short listitem)
{
	if (listitem>=0 && listitem < (sizeof(gProtoTypes) / sizeof(goTypeRec)))
		return gProtoTypes[listitem].supported;
	else
		return 0;
}

short DGopher::GetProtoItem( char theProto)
{
	short i;
	for (i=0; i < ( sizeof(gProtoTypes) / sizeof(goTypeRec)); i++) {
		if (gProtoTypes[i].kind == theProto) return i;
	  }
	return -1;
}




