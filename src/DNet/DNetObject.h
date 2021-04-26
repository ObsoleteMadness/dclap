// DNetObject.h  
// Base class for Internet objects
// by D. Gilbert, Mar 1992
// DClap version Jan 94, revised 1996

#ifndef __DNETOBJ__
#define __DNETOBJ__
		
		
#include <DObject.h>
#include <DTCP.h>
#include <DTask.h>
#include <DTaskMaster.h>
#include <Dvibrant.h>



class	DList;
class DFile; 
class DView; 


class DNetOb : public DTaskMaster {
public:

	enum  TCPprotocols {
		kUnknownProt = 0,
		kUnsupportedProt,
		kSMTPprot,
		kGopherprot,
		kHTTPprot,
		kFileprot,  
		kFTPprot,
		kTelnetprot,
		kTN3270prot,
		kWhoisprot,
		kFingerprot,
		kPOPprot,
		kBOPprot,
		kNNTPprot,
		kWAISprot
		};

		// basic object info
	short			fProtocol;		// kGopher, kFTP, kHTTP, kSMTP, kETC
	long 			fType; 				// type of object 
	char*			fName;				 
	char*			fPath;			 
	char*			fHost;				 
	long			fPort; 				// Port number on host   
	char*			fURL;					//
	char*			fUser;			 
	char*			fPass;			 
	Boolean		fIsLocal;			// for type file:///

		// internal data
	char*			fInfo;					// data read from service 
	long			fInfoSize;			// can't use strlen for binary data @!
	char			*fQuery;				// alternate query data ptr, temp
	char			*fQueryGiven;
	Boolean		fDeleted;


	DNetOb();
	DNetOb( DNetOb* anet);
	virtual ~DNetOb();
	virtual	Boolean suicide(void);   
	virtual	Boolean suicide(short ownercount);
	
	static short StandardPort( short protocol);
	static const char* GetProtoName( short listitem);
	static char  GetProtoVal( short listitem);
	static short GetProtoItem( char theProto);
	static Boolean  GetProtoSupport( short listitem);

	virtual	void Initialize();
	virtual void Copy( DNetOb* anet); 
	virtual DObject* Clone();	// override
	virtual void DeleteInfo();
	Boolean Equals(DNetOb* other);
	Boolean operator ==(DNetOb* other);
	
	virtual const char* ShortName();  
	virtual const char* GetName();
	virtual const char* GetPath();
	virtual const char* GetHost();
	virtual const char* GetPort();
	virtual const char* GetUser();
	virtual const char* GetPass();
	virtual const char* GetProtocol();
	virtual const char* GetURL();
	virtual const char* GetKindName();

	virtual void StoreName(char* aStr);
	virtual void StorePath(char* aStr);
	virtual void StoreHost(char* aStr);
	virtual void StoreUser(char* aStr);
	virtual void StorePass(char* aStr);
	virtual void StorePort(char* aStr);
	virtual void StoreProtocol( short protocol);
	virtual void StoreURL(char *plus);
		
};

#endif

