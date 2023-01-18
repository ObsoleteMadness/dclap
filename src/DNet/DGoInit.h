// DGopherInit.h
// d.g.gilbert


#ifndef __DGOPHERINIT__
#define __DGOPHERINIT__


#include <DObject.h>
#include <DList.h>
#include <DControl.h>
#include <DWindow.h>
#include <DTableView.h>
 

enum GopherTransferKind {
	kTransferBinary, kTransferText, kTransferNone
	};

class DGodocKindList;


class DGopherMap : public DObject {
public:
	long		fId;						// internal id of this type
	short		fHandlerType;   // none,internal,launch=external,???	 
	unsigned short fPreference; // priority over other view types (0=never .. 64000 highest pri)
	DGodocKindList	*	fKind;						// go+, main/subtype
	DGodocKindList	*	fServerSuffixes;  // list of any server suffixes to map to this kind 
	char		fServerType;

		// local info
	char		fLocalType; 	 	// gopher type
	char		fMapSuffixWhen; // suffix-local type map=never,default,always,??
	short		fTransferType;  // default=binary,text
	long		fMacType, fMacSire;  // mac file typing
	char	*	fSuffix;				// unix,dos filename suffix
	char	*	fHandlerName;		// mac,other? handler-app name
	short		fCommand;				// gopher command #  
	Boolean	fSaveToDisk; 		// option for internal handlers
	char	*	fParameters;		// temp data for reading parameters
	short		fStatus;				// changed,added,deleted flag
	Boolean fServerSuffixesChanged, fServerLocalChanged;
		
	DGopherMap();
	virtual ~DGopherMap();
	virtual	Boolean suicide(void);  // prefered to delete 
	virtual	Boolean suicide(short ownercount);
	
	virtual	void Initialize();
	virtual void ReadData(short datakind);
	virtual void ReadKind(); 
	virtual void ReadServerType();
	virtual void ReadServerSuffix();
	virtual void ReadLocalHandling();

	virtual	Boolean Map(DGopher* aGopher);
	
	virtual const char* GetID();
	virtual void  SetID( char* s);
	virtual void  SetParameters( char* params);
	virtual const char* GetParameters();

	virtual const char* GetKind();
	virtual void  SetKind( char* s);
	virtual	const char* GetHandlerType();
	virtual void  SetHandlerType( char* s);
	virtual const char* GetHandlerName();
	virtual void  SetHandlerName( char* s);
	virtual const char* GetPreference();
	virtual void  SetPreference( char* s);
	
	virtual	const char*	GetServerType();
	virtual	void  SetServerType( char* s);
	virtual	const char*	GetLocalType();
	virtual	void  SetLocalType( char* s);
	virtual	const char*	GetMapWhen();
	virtual	void  SetMapWhen( char* when);
	virtual	const char* GetMacTypeNSire();
	virtual	void  SetMacTypeNSire( char* s);
	virtual const char*	GetSuffix();
	virtual void  SetSuffix( char* s);
	virtual const char*	GetServerSuffixes();
	virtual void  SetServerSuffixes( char* s);
	virtual	Boolean GetTransferBinary();
	virtual	void  SetTransferBinary(Boolean turnon = true);
	virtual	Boolean GetSaveToDisk();
	void SetSaveToDisk(Boolean turnon = true);
	virtual	Boolean GetDisplay();
	//virtual	void  SetDisplay(Boolean turnon);
	virtual	short	GetCommand();
	virtual	void  SetCommand();
};



class DGopherMapList : public DList {
public:
	DGopherMapList();
	DGopherMap* GopherMapAt(long index) { return (DGopherMap*) At(index); }
	
	void ReadParams(char* theId, char* params, short datakind);
	void DeleteItem(char* theId);

	virtual DGopherMap* MatchID(long theId);  

	virtual DGopherMap* MatchHandlerKind(unsigned short& lastrank, char* aKind);  
	virtual DGopherMap* MatchHandlerKind(DGopher* aGopher);  
	virtual DGopherMap* MatchGopherType(char theType, Boolean matchAny);
	virtual DGopherMap* MatchGopherType(DGopher* aGopher);
	virtual DGopherMap* MatchSuffix(const char* pathname, char gopherType);  
	virtual DGopherMap* MatchSuffix(DGopher* aGopher);  

	virtual DGopherMap*  GetPreferedFiletype(char* aKind, 
						long& macFileType, long& macCreator, char*& suffix);
	virtual DGopherMap*  GetPreferedFiletype(DGopher* aGopher, 
						long& macFileType, long& macCreator, char*& suffix);
};




class DGoMapHandlerView;

class DGopherMapDoc : public DWindow 
{
public:
	DGopherMapList * fItems;
	DGoMapHandlerView * fListView;
	DCheckBox * fServerTypeEnable;
	DCheckBox * fServerSuffixEnable;
	Boolean	fChanged;

	static  void UpdateGopherMap(short item, DGopherMap *gm);
	static  Boolean EditHandler( char* itsKind = "text/plain", DGopherMap* item = NULL, short dlogKind = 4);
	virtual void EditLocalHandling(DGopherMap* item, short dlogKind);
	
	DGopherMapDoc( long id, DGopherMapList* itsMappers);
	//virtual ~DGopherMapDoc();

	virtual void Open();
	virtual void Close();
	virtual Boolean IsMyAction(DTaskMaster* action);
};


class DGoMapHandlerView : public DTableView 
{
public:
	DGopherMapDoc *	fDoc;
	DGopherMapList * fItems;
	
	DGoMapHandlerView(long id, DGopherMapDoc* itsDoc, short pixwidth, short pixheight);

	virtual void Drag(Nlm_PoinT mouse);
	virtual void Hold(Nlm_PoinT mouse);
	virtual void Release(Nlm_PoinT mouse);
	virtual void DoubleClickAt(long row, long col);
	virtual void SingleClickAt(long row, long col);
	virtual DGopherMap* SelectedMapper();
	virtual void GetReadyToShow();
	 void DrawCell(Nlm_RecT r, long row, long col);
};





class DGopherEditView : public DCluster {		
public:
	DGopher*	fGopher;
	char*			fOldURL;
	
	DGopherEditView(long id, DView* itsSuperior, DGopher* itsGopher,
			const char* oldURL, short width, short height, Boolean hidden = false, 
			char* title = NULL);
	virtual ~DGopherEditView();
	virtual void InstallControls();
	virtual void Answers();
};


class DGopherEditWindow : public DWindow 
{
public:
	const char 	*fName, *fHost, *fPort, *fPath, *fURL;
	DGopher			*fGopher;
	Boolean			fEditAll;
	DGopherEditView	* fView;
	
	DGopherEditWindow(DGopher* itsGopher, Boolean editAll);
	virtual ~DGopherEditWindow();
	virtual void OkayAction();
	virtual void Open();
};



extern  Boolean		gDoSuffix2MacMap, gNeedTypeChange;
extern  DGopherMapList*		gGopherMap;

void EditGopherMap();
void InitializeUGopher();



#endif
