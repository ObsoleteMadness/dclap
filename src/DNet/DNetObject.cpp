// DNetObject.cpp
// from DGopher.cp  by d.g. gilbert, Mar 1992 
// version for DClap, Jan 94


#include <ncbi.h>
#include <dgg.h> 

#include "DTCP.h"
#include "DURL.h"
#include "DNetObject.h"

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
	kBOPport	= 7110,
	kNNTPport = 119,
	kWAISport	= 210
	};


enum NetTypes {
 kTypeFile     	= '0',  	// text file  
 kTypeFolder		= '1',  	// menu/directory 
 kTypeCSO      	= '2',		// CSO phone book service 
 kTypeError    	= '3',  	// error 
 kTypeBinhex		= '4',		// binhex encoded Macintosh file 
 kTypeUuencode 	= '6',  	// uuencoded file 
 kTypeQuery    	= '7',  	// question service 
 kTypeTelnet   	= '8',  	// telnet service 
 kTypeBinary   	= '9',  	// binary 
 kTypeSound    	= 's',  	// sound 
 kTypeImage			= 'I',		// image, probably gif
 kTypeMovie			= ',',		// movie
 kTypeNote			= 'i',		// note, information for directory
 kTypeEndOfData	= '.',  	// end of information 
 kTypeGif				= 'g',	// GIF picture
 kTypeWhois   	= 'w',	// WHOIS phone book service -- not official, just easy 
 kTypeTn3270  	= 'T', 	// Telnet variant -- just call diff app ?
 kTypeHtml      = 'h',	// WWW type, ????
 kTypeMime    	= 'M',  // Mime, no mac reader yet
 kMailType			= 'm',		// mailto, dgg addition for gopherpup
	// obsolete types 
 kTypeBinhexpc	= '5',  // binhex encoded MSDos file 
 kTypeEvent    	= 'e',  // ?? 
 kTypeCal			 	= 'c',  // ?? 
 kTypeRedundant = '+',  // a redundant server holding the same information as the previous server
 kTypeNull 			= '\0'	// end of data
};




		//private	variables

Local const char*	kLocalhost	= "localhost";
Local const char*	gEmptyString	= "";

Local const	long 	kReadToClose = DTCP::kTCPStopAtclose;
Local const	long 	kReadToDotCRLF = DTCP::kTCPStopAtdotcrlf;

Local const	long 	kReadMaxbuf1 = 1 * 1024; 
Local const	long 	kMaxInt = 32000;

Local const char *gMonths[13] = 
	{"Jan","Feb","Mar","Apr","May","Jun",  "Jul","Aug","Sep","Oct","Nov","Dec","???"};






 
// DNetOb ------------------------------



DNetOb::DNetOb() :
	DTaskMaster( 0, NULL, NULL)
{
	Initialize();
}

DNetOb::DNetOb( DNetOb* nob)  :
	DTaskMaster( 0, NULL, NULL) 
{
	Initialize();
	Copy( nob);
}


DNetOb::~DNetOb()  
{
	MemFree( fName);
	MemFree( fPath);
	MemFree( fHost);
	MemFree( fURL);
	MemFree( fInfo);
	MemFree( fQueryGiven);
	MemFree( fUser);
	MemFree( fPass);
}


Boolean DNetOb::suicide() 
{ 
	if (GetOwnerCount() <= 1) { 
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide();
}

Boolean DNetOb::suicide(short ownercount)
{
	if (ownercount < 1) {
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide(ownercount);
}
 
DObject* DNetOb::Clone()
{
	DNetOb* nob= new DNetOb();
	nob->Copy( this);
	return nob;
}



void DNetOb::Initialize()
{
	fName= NULL;
	fPath= NULL;
	fHost= NULL;
	fURL= NULL;
	fUser= NULL;
	fPass= NULL;
	
	fType	= kTypeError; // == ancestor doesn't know much...
	fProtocol = kGopherprot;
	fPort= StandardPort( fProtocol);
	fIsLocal= false;
	fInfo= NULL;
	fInfoSize= 0;
	
	fQuery= NULL;
	fQueryGiven= NULL;
	fInfo	= (char*)Nlm_MemGet(1, true); 
	fInfoSize= 0; // fInfoSize is size of data not including terminating NULL !@!@$#@# 
}

void DNetOb::Copy( DNetOb* anob)  
{
	if (anob) {
	fProtocol= anob->fProtocol; 	 
	fType 	= anob->fType; 				 
	fName		= (char*) StrDup( (CharPtr)anob->fName);
	fPath		= (char*) StrDup( (CharPtr)anob->fPath);
	fHost		= (char*) StrDup( (CharPtr)anob->fHost);
	fUser		= (char*) StrDup( (CharPtr)anob->fUser);
	fPass		= (char*) StrDup( (CharPtr)anob->fPass);
	fPort		= anob->fPort; 	 
	fURL		= (char*) StrDup( (CharPtr)anob->fURL);
	fIsLocal= anob->fIsLocal;
	MemFree( fInfo); 
	fInfoSize= anob->fInfoSize;
	fInfo= (char*)MemDup( anob->fInfo, fInfoSize+1);
	fQuery = anob->fQuery;			 
	fQueryGiven = StrDup( anob->fQueryGiven);		 
	}
}

void DNetOb::DeleteInfo()
{
	if (fInfo) fInfo= (char*) MemMore( fInfo, 1);
	else fInfo= (char*) MemNew(1);
	*fInfo= 0;
	fInfoSize= 0;
}




void DNetOb::StoreName(char* aStr)
{
	MemFree( fName);
	fName= StrDup( aStr);
}

void DNetOb::StorePath(char* aStr)
{
	MemFree( fPath);
	fPath= StrDup( aStr);
}

void DNetOb::StoreUser(char* aStr)
{
	MemFree( fUser);
	fUser= StrDup( aStr);
}

void DNetOb::StorePass(char* aStr)
{
	MemFree( fPass);
	fPass= StrDup( aStr);
}


void DNetOb::StoreHost(char* aStr)
{
	if (!aStr || !*aStr || StrICmp(aStr,gEmptyString) == 0) 
		aStr= (char*)kLocalhost;
	MemFree( fHost);
	fHost= StrDup( aStr);
	fIsLocal= ( fHost==NULL 
							|| StrICmp(fHost,kLocalhost) == 0  
							|| StrICmp(fHost,gEmptyString) == 0 );
}

void DNetOb::StorePort(char* aStr)
{
	short port= 0;
	if (aStr) port= (short)atol(aStr); 
	if (port) fPort= port;
}


// static
short DNetOb::StandardPort( short protocol)
{
	switch (protocol) {
		case kGopherprot: return kGopherport;
		case kHTTPprot	: return kHTTPport;
		case kSMTPprot	:	return kSMTPport;
		case kBOPprot		:	return kBOPport;
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

void DNetOb::StoreProtocol( short protocol)
{
	fProtocol= protocol;
	fPort= StandardPort( fProtocol);
	switch (fProtocol) {	
			
		case  kGopherprot	:  
		case  kHTTPprot		:  
		case  kFTPprot		: 
		case  kBOPprot		: 
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


const char* DNetOb::GetProtocol()
{
#if 0
	short item= DNetOb::GetProtoItem(fProtocol);
	if (item<0) return "unknown://";
	return DNetOb::GetProtoName(item);
#else
	switch (fProtocol) {
		case  kNNTPprot		: return "news:"; 
		case  kSMTPprot		: return "mailto:"; 
		case  kBOPprot		: return "bop://"; 
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

void DNetOb::StoreURL(char* aStr)
{
	MemFree(fURL); 
	fURL= StrDup(aStr);
}

const char* DNetOb::GetURL()
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





Boolean DNetOb::Equals(DNetOb* other)
{
	if (!other) return false;
	else if (fType != other->fType) return false;
	else if (fPort != other->fPort) return false;
	else if (fProtocol != other->fProtocol) return false;
	else if (StringCmp( fHost, other->fHost)) return false;
	else if (StringCmp( fPath, other->fPath)) return false;
	else if (StringCmp( fQueryGiven, other->fQueryGiven)) return false;
	else return true;
}

Boolean DNetOb::operator ==(DNetOb* other)
{
	return Equals(other);
}
			



const char* DNetOb::GetKindName() 
{ 
	return "Unknown"; 
}

const char* DNetOb::GetName()   
{
	if (fName) return  fName;
	else return gEmptyString;
}

const char* DNetOb::GetPath()   
{
	if (fPath) return fPath;
	else return gEmptyString;
}

const char* DNetOb::GetHost()   
{
	if (fHost) return fHost;
	else return gEmptyString;
}

const char* DNetOb::GetUser()   
{
	if (fUser) return  fUser;
	else return gEmptyString;
}

const char* DNetOb::GetPass()   
{
	if (fPass) return  fPass;
	else return gEmptyString;
}

const char* DNetOb::GetPort()   
{
	static char sport[20];
	if (fPort) {
		sprintf(sport,"%d",fPort);
		return sport;
		}
	else return gEmptyString;
}


const char* DNetOb::ShortName()   
{
	static char  shorty[33];
	if (fName) {
		strncpy(shorty, fName, 32);
		shorty[33]= 0;
		return shorty;
		}
	else return gEmptyString;
}













struct nobTypeRec {
	char kind;
	const char* name;
	Boolean supported:1;
	};


Local nobTypeRec gProtoTypes[] = {
	{ DNetOb::kGopherprot, "gopher://", true },
	{ DNetOb::kHTTPprot, "http://", true },
	{ DNetOb::kFileprot, "file://", true },
	{ DNetOb::kFTPprot, "ftp://", true },
	{ DNetOb::kSMTPprot,"mailto:", true },
	{ DNetOb::kBOPprot,"bop://", true },
	{ DNetOb::kWAISprot, "wais://", false },
	{ DNetOb::kTelnetprot, "telnet://", false },
	{ DNetOb::kTN3270prot, "tn3270://", false },
	{ DNetOb::kNNTPprot, "news:", false },
	{ DNetOb::kFingerprot, "finger:", true },
	{ DNetOb::kWhoisprot, "whois:", true },
	{ DNetOb::kUnsupportedProt, "unsupported://", true },
	{ DNetOb::kUnknownProt, "unknown://", true },
	{0,0}
	};

//static
const char* DNetOb::GetProtoName( short listitem)
{
	if (listitem>=0 && listitem < (sizeof(gProtoTypes) / sizeof(nobTypeRec)))
		return gProtoTypes[listitem].name;
	else
		return NULL;
}

char DNetOb::GetProtoVal( short listitem)
{
	if (listitem>=0 && listitem < (sizeof(gProtoTypes) / sizeof(nobTypeRec)))
		return gProtoTypes[listitem].kind;
	else
		return 0;
}

Boolean DNetOb::GetProtoSupport( short listitem)
{
	if (listitem>=0 && listitem < (sizeof(gProtoTypes) / sizeof(nobTypeRec)))
		return gProtoTypes[listitem].supported;
	else
		return 0;
}

short DNetOb::GetProtoItem( char theProto)
{
	short i;
	for (i=0; i < ( sizeof(gProtoTypes) / sizeof(nobTypeRec)); i++) {
		if (gProtoTypes[i].kind == theProto) return i;
	  }
	return -1;
}




