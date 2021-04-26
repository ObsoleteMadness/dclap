// DGopherClasses.h
// d.g.gilbert

#ifndef __DGOPHERCLASSES__
#define __DGOPHERCLASSES__

#include <DGopher.h>
#include <Dvibrant.h>

			// Types of objects returned from a Gopher server 
			// This Jack should be private !! implementation only 
enum GopherType {
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
	//experimental
 kTypeGif				= 'g',	// GIF picture
 kTypeWhois   	= 'w',	// WHOIS phone book service -- not official, just easy 
 kTypeTn3270  	= 'T', 	// Telnet variant -- just call diff app ?
 kTypeHtml      = 'h',	// WWW type, ????
 kTypeMime    	= 'M',  // Mime, no mac reader yet

 kMailType			= 'm',		// mailto, dgg addition for gopherpup
	
	//? obsolete types ?
 kTypeBinhexpc	= '5',  // binhex encoded MSDos file 
 kTypeEvent    	= 'e',  // ?? 
 kTypeCal			 	= 'c',  // ?? 
 kTypeRedundant = '+',  // a redundant server holding the same information as the previous server
 kTypeNull 			= '\0'	// end of data
};


enum GopherIconID {
	kTextIcon				= 2200,	// resource fork must have these icon/cicon 
	kFolderIcon			= 2201,
	kPhonebookIcon	= 2202, //CSO phonebook
	kDefaultIcon		= 2203,
	kBinhexIcon			= 2204,
	kUuencodeIcon 	= 2206,
	kIndexIcon 			= 2207,
	kTelnetIcon			= 2208,
	kSoundIcon			= 2212,
	kWhoisPhonebookIcon	= 2213,
	kBinaryIcon			= 2214,
	kImageIcon			= 2215,
	kMovieIcon			= 2216,
	kHTMLIcon			= 2217,
	kNoteIcon				= 2218,
	kFirstIcon			= kTextIcon,
	kLastIcon				= kNoteIcon
};

//const	long	kMaxInt					= 32000;	






		// Three primary subclasses for further subclassing... other gopher objects private??			

class DFolderGopher : public DGopher {
public:
	DFolderGopher( char ctype, const char* link);
	DFolderGopher();
	DFolderGopher( DFolderGopher* aGopher);
	virtual	void Initialize();	// override 
	virtual DObject* Clone();
	virtual void OpenQuery(); // override 
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
};

class DTextGopher : public DGopher {
public:
	static long	kMacType, kMacSire;
	static char* kSuffix;
	DTextGopher( char ctype, const char* link);
	DTextGopher();
	DTextGopher( DTextGopher* aGopher);
	virtual	void Initialize();	// override 
	virtual DObject* Clone();
	virtual void OpenQuery(); // override 
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
	static  void InitSignature();	 
};
			
class DBinaryGopher : public DGopher {
public:
	static long	kMacType, kMacSire;
	static char* kSuffix;
	DBinaryGopher( char ctype, const char* link);
	DBinaryGopher();
	DBinaryGopher( DBinaryGopher* aGopher);
	virtual	void Initialize();	// override 
	virtual DObject* Clone();
	virtual void OpenQuery(); // override 
	virtual void DrawIcon(Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
	static void InitSignature();	 
};
			



	// keep specific gopher types private here, so we can add/remove
	// ones as desired... 
	
class DIndexGopher : public DFolderGopher {
public:
	DIndexGopher( char ctype, const char* link);
	DIndexGopher();
	DIndexGopher( DIndexGopher* aGopher);
	virtual	void Initialize();	// override 
	virtual DObject* Clone();
	virtual	void OpenQuery();
	virtual void CloseQuery();
	virtual void InfoTask();
	virtual Boolean PoseQuestion(CharPtr& query);
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
};



class DTelnetGopher : public DGopher {
public:
	DTelnetGopher( char ctype, const char* link);
	DTelnetGopher();
	DTelnetGopher( DTelnetGopher* aGopher);
	virtual	void Initialize();	// override 
	virtual DObject* Clone();
	virtual void InfoTask();
	virtual void promptUser(char* msg1, char* msg2);
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
};

class DBinhexGopher : public DGopher {
public:
	Boolean fGotBinhexKey;
	unsigned char 	*fHqxbuf, *fBufptr, *fBufend, *fBufstart;
	long 	fLinecount, fFilecount;
	long 	fSavestate, fTotalbytes, fSaveRunLength, fSaveNibble;
	short fDecodeStatus;

	void hqxFini();
	void hqxInit();
	short fill_hqxbuf();
	long check_hqx_crc();
	short hqx2FinderInfo( char *binfname, long *fauth, long *ftype, 
										unsigned long *hqx_datalen, unsigned long  *hqx_rsrclen); 
	short hqx2fork( short fileref, unsigned long nbytes);

	DBinhexGopher( char ctype, const char* link);
	DBinhexGopher();
	DBinhexGopher( DBinhexGopher* aGopher);
	virtual	void Initialize();	// override 
	virtual DObject* Clone();
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
};
	
#if FIX_LATER
class DCSOPhoneBookGopher : public DTextGopher {
public:
	DCSOPhoneBookGopher( char ctype, const char* link);
	DCSOPhoneBookGopher();
	DCSOPhoneBookGopher( DCSOPhoneBookGopher* aGopher);
	virtual	void Initialize();	// override 
	virtual DObject* Clone();
	virtual void ReadALine();  // override 
	virtual void ReadAChunk( long maxchunk);  // override 
	virtual void addToInfo( char *cp, short len);
	virtual void addQuery( long qname, char* query, DWindow* aWind);
	virtual void InfoTask();
	virtual Boolean queryUser( CStr255& query);
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
};
#endif

#if FIX_LATER
class CWhoisPhoneBookGopher : public CTextGopher {
public:
	CWhoisPhoneBookGopher( char ctype, const char* link);
	CWhoisPhoneBookGopher();
	CWhoisPhoneBookGopher( CWhoisPhoneBookGopher* aGopher);
	virtual	void Initialize();	// override 
	virtual void InfoTask();
	virtual CGopher* Clone();
	virtual Boolean PoseQuestion(CStr255& query);
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
};
#endif

#if FIX_LATER
class CUuencodeGopher : public CTextGopher {
public:
	CUuencodeGopher( char ctype, const char* link);
	CUuencodeGopher();
	CUuencodeGopher( CUuencodeGopher* aGopher);
	virtual	void Initialize();	// override 
	virtual CGopher* Clone();
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
};
#endif

	
class DSoundGopher : public DBinaryGopher {
public:
	static long	kMacType, kMacSire;
	static char* kSuffix;
	DSoundGopher( char ctype, const char* link);
	DSoundGopher();
	DSoundGopher( DSoundGopher* aGopher);
	virtual	void Initialize();	// override 
	virtual DObject* Clone();
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
	static void InitSignature();	 
};
			

class DImageGopher : public DBinaryGopher {
public:
	static long	kMacType, kMacSire;
	static char* kSuffix;
	DImageGopher( char ctype, const char* link);
	DImageGopher();
	DImageGopher( DImageGopher* aGopher);
	virtual DObject* Clone();
	virtual	void Initialize();	// override 
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
	static void InitSignature();	 
};

class DMovieGopher : public DBinaryGopher {
public:
	static long	kMacType, kMacSire;
	static char* kSuffix;
	DMovieGopher( char ctype, const char* link);
	DMovieGopher();
	DMovieGopher( DMovieGopher* aGopher);
	virtual DObject* Clone();
	virtual	void Initialize();	// override 
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
	static  void InitSignature();	 
};


class DNoteGopher : public DGopher {
public:
	DNoteGopher( char ctype, const char* link);
	DNoteGopher();
	DNoteGopher( DNoteGopher* aGopher);
	virtual DObject* Clone();
	virtual	void Initialize();	// override 
	virtual void InfoTask();
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
};

class DHtmlGopher : public DTextGopher {
public:
	DHtmlGopher( char ctype, const char* link);
	DHtmlGopher();
	DHtmlGopher( DHtmlGopher* aGopher);
	virtual	void Initialize();	// override 
	virtual DObject* Clone();
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
};

class DMailGopher : public DGopher {
public:
	DMailGopher( char ctype, const char* link);
	DMailGopher();
	DMailGopher( DMailGopher* aGopher);
	virtual DObject* Clone();
	virtual	void Initialize();	// override 
	virtual	void OpenQuery();	// override 
	virtual void InfoTask();
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
};

class DHTTPGopher : public DGopher {
public:
	DHTTPGopher( char ctype, const char* link);
	DHTTPGopher();
	DHTTPGopher( DHTTPGopher* aGopher);
	virtual DObject* Clone();
	virtual	void Initialize();	// override 
	virtual	void OpenQuery();	// override 
	virtual void InfoTask();
	virtual void DrawIcon( Nlm_RecT& area, short size); // override 
	virtual const char* GetKindName();
};



#endif
