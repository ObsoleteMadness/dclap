// DGopher.h  
// Internet Gopher client
// by D. Gilbert, Mar 1992
// DClap version Jan 94

#ifndef __DGOPHER__
#define __DGOPHER__
		
		
#include <DObject.h>
#include <DTCP.h>
#include <DTask.h>
#include <DTaskMaster.h>
#include <Dvibrant.h>


extern  const	long 		kReadMaxbuf;
extern  const	short 	kGopherPort;  		// default/prime port #

class	DList;
class	DGopherList;
class DFile; 
class DIconList;
class DView; 
class	DGoMenuBlock;
class DGopherAbstract;


class DGopherTalk : public DTCP {
public:
	long   fLinesRead;
	DGopherTalk();
	virtual void OpenQuery( char* host, short port, char* path, char* query, 
													char* viewchoice, char* plus, DFile* plusFile);
	virtual Boolean ReadALine( char* appendToHandle, char*& result);
};



struct DocLink {
	enum DocLinkType { kNoLink, kString, kLine, kRect, kLineRect };
	DocLinkType 	fType;
	short					fParag, fChar;
  union {
		struct {
			char*		fString;		
			short		fSkip;
			} string;
		struct {
			short fEndParag, fEndChar;
			} line;
		struct {
			Nlm_RecT	fRect;
			} rect;
		};

	DocLink();
	~DocLink();
	Boolean HasLink() { return fType != kNoLink; }
	short Kind() { return fType; }
	void Clear();
	void DupData();
	char *GetString( short& skipval);
	void SetString( const char* thestr, short skipvalue= 0);
	void SetSkip( short skipvalue);
	void GetRect( Nlm_RecT& therect)	{ therect= rect.fRect; }
	void SetRect( Nlm_RecT therect);
	void GetLine( short& atparag, short& atchar, short& endparag, short& endchar );
	void SetLine( short atparag, short atchar, short endparag, short endchar );
	void GetLineRect( Nlm_RecT& therect, short& atparag, short& atchar );
	void SetLineRect( Nlm_RecT therect, short atparag, short atchar);
};



class DGopher : public DTaskMaster {
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
		kNNTPprot,
		kWAISprot
		};

	enum	tasks { 
		kGopherTask= 2200,
				// obsolete??
		cNoCommand = 0,
		cNewGopherFolder	= 2211,   // data is in gGopherData 
		cNewGopherText		= 2212,		// " 
		cNewGopherAsk			= 2213,		// data is in gCurrentGopher
		cNewGopherFile		= 2214,		// " 
		cNewGopherImage		= 2215,		// " 
		cOpenGopherFolder	= 2221,	  // data is in gGopherFile 
		cOpenGopherText		= 2222,		// " 
		cOpenGopherAsk		= 2223,		// data is in gCurrentGopher
		cOpenGopherFile		= 2224,		// " 
		cOpenGopherImage  = 2225,		// " 
		cGopherNetDoc 		= 2230,		// new dec'94
		cGopherGetInfo 		= 2242
		};
		
	enum viewIDs {
		kServerMapID = 1001,
		kGopherMapID = 1002,
		kGopherPlusMapID = 1003 
		};
	
	enum	newResult {
		kNewGopherOkay = 0,
		kNewGopherNone = 1,
		kNewGopherEnd  = -1,
		kGetItemInfo = -2 
		};
		
	enum	threadState {
		kThreadNotStarted = 0,
		kThreadDataOld,
		kThreadDataNew,
		kThreadDone,
		kThreadStarted,
		kThreadDoneReading,
		kThreadReadingToDisk
		};
	
		// basic gopher info, +INFO
	char			fType; 				// The type of object (A_FILE, A_INDEX, etc) 
	long			fItitle;			// index into fLink to null-terminated string for Title
	long			fIdate;				// "  [Date,Size] in title 
	long			fIsize;				// "  [Date,Size] in title
	long			fIpath;				// "  path
	long			fIhost;				// "  host
	long			fIport;				// "  port
	long			fIplus;				// "  plus info
	short			fPort; 				// Port number on host   
	char*			fLink;				// title/0path/0host/0port/0plus/0???/0etc...
	long			fLinkLen;			// keep true length of fLink 
	
		// gopher+ info
	char*			fDate ;	 			// Date of object on host 
	long			fDateLong;
	char*			fDataSize;		// Size of object on host 
	long			fSizeLong;
	
	char*			fAdminEmail;		// +ADMIN
	DList* 		fViews;		// +VIEWS, list of CGopherItemView 
	DList* 		fAskers;	// +ASK, list of CGopherItemAsk
	short			fIsPlus;				// is a gopher+ link: yes, no, don't know
	Boolean 	fShortFolder;		// for minimum data, false is default	
	Boolean		fHasAsk;				// gopher+, any item can also have ASK block
	Boolean		fHaveReply;			// gopher+, if ASK dialog (fInfo) ready to reply
	DFile*		fReplyFile;			// ASK reply file, if any
	char*			fReplyData;			// ASK reply data other than file
	DGoMenuBlock* fGoMenuBlock;	// +MENU data
	DGopherAbstract* fAbstract;	// +ABSTRACT		
	
		// multi-protocol support
	short			fProtocol;			// kGopher, kFTP, kHTTP, kSMTP, kETC
	char*			fURL;						// from +URL or otherways
	Boolean		fIsLocal;				// for type file:///
	Boolean		fIsMap;				  // html, go+ item that sends x,y or rect as query

		// internal data
	char*			fInfo;					// data read from service 
	long			fInfoSize;			// can't use strlen for binary data @!
	//InfoProc	fInfoHandler; 	// app proc that handles incoming data during reads
	DTask*		fInfoHandler;
	
	char			*fQuery;				// alternate query data ptr, temp
	char			*fQueryPlus;		// append to query for +, local temp
	char			*fQueryGiven;		// default value given in +QUERYSTRING
	short			fTransferType; 	//text/lines or binary or none ?
	Boolean		fDeleted;
	long			fMacType;
	long			fMacSire;
	Boolean		fSaveToDisk;
	Boolean		fLaunch;
	short			fCommand;
	short			fViewchoice;
	DGopherList	*fOwnerList;
	DView 		*fStatusLine;
	DGopherTalk	*fTalker;
	long			fBytesExpected;
	long			fBytesReceived;
	long			fConnectTime;
	short			fThreadState;  
	char*			fAppFile;				
	short			fInUseCount;
	DocLink*	fDocLink;


	DGopher();
	DGopher( char ctype, const char* link);
	DGopher( DGopher* aGopher);
	virtual ~DGopher();
	virtual	Boolean suicide(void);   
	virtual	Boolean suicide(short ownercount);
	
	static short StandardPort( short protocol);
	static const char* GetTypeName( short listitem);
	static char  GetTypeVal( short listitem);
	static short GetTypeItem( char theType);
	static Boolean  GetTypeSupport( short listitem);
	static const char* GetProtoName( short listitem);
	static char  GetProtoVal( short listitem);
	static short GetProtoItem( char theProto);
	static Boolean  GetProtoSupport( short listitem);

	virtual	void Initialize();
	virtual void CopyGopher( DGopher* aGopher); 
	virtual DObject* Clone();	// override
	virtual void SetLink( char ctype, const char* link);
	virtual void DeleteLink(Boolean onlyInfoline = false);
	virtual void DeleteInfo();
	
	virtual void SetPlusInfo( short isPlus, Boolean hasAsk, char* plus);
	virtual void DeleteViews();
	virtual void DeleteAskers();
	DList* CloneViews();
	DList* CloneAskers();

	virtual const char* ShortName();  
	virtual const char* GetName();
	virtual const char* GetDate();
	virtual const char* GetDataSize();
	virtual const char* GetPath();
	virtual const char* GetHost();
	virtual const char* GetPort();
	virtual const char* GetPlus();
	virtual const char* GetProtocol();
	virtual const char* GetURL();
	virtual const char* GetKindName();

	virtual void StoreName(char* aStr);
	virtual void StorePath(char* aStr);
	virtual void StoreHost(char* aStr);
	virtual void StorePort(char* aStr);
	virtual void StorePlus(char *plus);
	virtual void StorePlus(Boolean isplus);
	virtual void StoreProtocol( short protocol);
	virtual void StoreURL(char *plus);

	virtual void SetOwner( DGopherList* OwnerList);  
	virtual Boolean Edit(long dialogID);
	virtual Boolean Equals(DGopher* other);
	virtual Boolean operator ==(DGopher* other);

	virtual void ToText(CharPtr& h, short indent = 0);
	virtual void ToServerText(CharPtr& h, short itemNum);
	virtual void ToPrettyText(CharPtr& h, short itemNum,
				Boolean showDate, Boolean showSize, Boolean showKind, Boolean showPath, 
				Boolean showHost, Boolean showPort, Boolean showAdmin);
	
	virtual void OpenQuery();
	virtual void CloseQuery();
	virtual void LocalQuery( char* viewchoice);
	virtual void FinishRead();
	virtual Boolean EndOfMessage();

	virtual void ReadALine(); 
	virtual void ReadAChunk( long maxchunk);

	virtual void InfoTask();
	virtual void ReadTask();
	virtual Boolean IsMyTask(DTask* theTask); // override 
	virtual void ProcessTask(DTask* theTask);  // override

	virtual void SetInfoHandler(DTask* handlerTask) { fInfoHandler= handlerTask; }
	virtual void ShowMessage(const char* msg);
	virtual void ShowProgress();
	
	virtual short CommandResult();
	virtual short ItemForm(short viewchoice = 0);
	virtual	void  SetAskReply( char* data, DFile* replyFile);
	virtual	void  ClearAskReply();
	virtual	void  SwapPathType(char oldtype, char newtype);
	virtual short ThreadProgress(short update = 0);
	virtual const char* GetViewChoiceKind();
	virtual void  AddView( const char* viewkind);
	virtual void  CheckViewMappers();
	
	virtual Boolean HasDocLink();

	virtual void DrawIcon( Nlm_RecT& area, short size);  
	virtual void DrawIconSub( Nlm_RecT& area);  
	virtual void DrawTitle( Nlm_RecT& area);  
	virtual void DrawKind( Nlm_RecT& area);  
	virtual void DrawDate( Nlm_RecT& area);  
	virtual void DrawSize( Nlm_RecT& area);  
	virtual void DrawPath( Nlm_RecT& area);  
	virtual void DrawHost( Nlm_RecT& area);  
	virtual void DrawPort( Nlm_RecT& area);  
	virtual void DrawPlus( Nlm_RecT& area);  
	virtual void DrawAdmin( Nlm_RecT& area);  
	virtual void DrawAbstract( Nlm_RecT& area);  
	virtual void DrawURL( Nlm_RecT& area);  

	virtual short WidthIcon(short size);
	virtual short WidthTitle();  
	virtual short WidthKind();  
	virtual short WidthDate();  
	virtual short WidthSize();  
	virtual short WidthPath();  
	virtual short WidthHost();  
	virtual short WidthPort();  
	virtual short WidthPlus();  
	virtual short WidthAdmin();  
	virtual short HeightAbstract();  
	
};



		// these should be private, but need in UGopherClass.cp & UGopher.cp
//extern void*		gIcons[];
//extern void*		gGopherIcon; //pointer to gIcons[i], may be CIconHandle or Handle 
//extern Boolean	gIconIsCicon;
extern DIconList*	gGopherIcons;
extern short		gGopherIconID;
extern short		gIconSize;

extern Boolean	gUseDigFolder;
extern short		gDigVol;
extern long			gDigDirID;
///
extern Boolean	gListUnknowns;
extern Boolean	gDoSuffix2MacMap;
extern Boolean	gShortFolder;
extern char*		gLocalGopherRoot;
extern char*		gAskWaisPath;

#endif
