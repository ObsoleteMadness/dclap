// DGopherDoc.h
// d.g.gilbert


#ifndef _DGONETDOC_
#define _DGONETDOC_


#include <DWindow.h>
#include <DRichViewNu.h>
#include <DPanel.h>
#include <DTableView.h>
#include <Dvibrant.h>
#include <DGoList.h>
#include <DUtil.h>

class DGopher;
class DFile;
class DPrompt;
class	DMenu;
class DCluster;
class DButton;
class DGopherMap;
class DPopupList;
class DRichStyle;
class DRichView;
class DRichHandler;
class DGopherListDoc;
class DGoLinkedTextDoc;

class	DGopherTextStore : public DTaskMaster
{
public:
	DGopher				* fParentGo, * fStufGo;
	short						fFirststuff;
	DRichView 		* fRichView;
	short						fDocFormat;
	DTempFile			* fDocFile, * fFetchFile;
	DRichHandler	* fRichHandler;
	DRichStyle		* fInlineStyle;
 	DGopherList 	*	fGolist;
	Boolean					fIsFromFile, fIsLinkDoc;
	short						fMapLink;
	DWindow 			* fWindow;
	
	DGopherTextStore( DGopher* itsGopher, DWindow* itsWindow, Boolean havelinks = true);
	virtual ~DGopherTextStore();
	virtual	Boolean suicide(void);  // prefered to delete 
	virtual	Boolean suicide(short ownercount);
	
	virtual Boolean IsMyTask(DTask* theTask);
	virtual void 	  ProcessTask(DTask* theTask);
	virtual char*   ProcessTask1( char* databuf, ulong& datasize);
	virtual void 	  Close();
	virtual Boolean OpenStore();
	virtual void 		SetGolist(DGopherList *theList);
	virtual void    WriteDoc( DFile* toFile, char* readkind, char* writekind);
	virtual void    StartInlineFetch( DGopher* stufGo, DRichStyle* stufStyle);
	virtual const char* GetTitle();
};



class	DGopherTextDoc : public DRichTextDoc, public DSaveHandler
{
public:
	enum gtTasks { kGoTextdoc = 230, cPushpin, cParentDrop, cShiftFore, cShiftBack };
	DGopherTextStore *	fStore;
	DPrompt			*	fStatus;
	Boolean 			fInUse, fPinned;
	DIconButton * fStickpin, * fShiftFore, * fShiftBack; 
	Nlm_PoinT			fTextViewLoc;
	DList				*	fTextStoreList;
	short					fListAt;
	
	
	DGopherTextDoc(long id, DGopher* itsGopher, Boolean havelinks = true);
	virtual ~DGopherTextDoc();
	
	virtual Boolean PushNewDoc( DGopher* itsGopher, DGopherList* itsList, Boolean havelinks = true);
	virtual Boolean PopDoc( short numitems = 1);
	virtual Boolean IsMyAction(DTaskMaster* action); 
	virtual Boolean IsMyTask(DTask* theTask);
	virtual void ProcessTask(DTask* theTask);
	virtual void Open();
	virtual void Close();
	virtual void Activate();
	virtual void Deactivate();
	virtual void ShowMessage(const char* msg);
	virtual void Save(DFile* aFile);
	virtual const char* GetFileToSave(DFile*& aFile);
	virtual void PinWindow(Boolean turnon);
	virtual void AddToList( DGopherTextStore* theStore);
};


class	DGoLinkedTextDoc : public DGopherTextDoc
{
public:
	enum gltTasks { kGoLitextdoc = 245, cMenuPopup };
	DPopupList 		* fMenuList;
	DGopherListDoc * fGopherDoc; // ?? remove
	DList					*	fInlineParts;
	static Boolean gAutoloadInlines;
	static short TestFormat( char* databuf, ulong datasize, char*& datastart);

	DGoLinkedTextDoc(long id, DGopher* itsGopher, DGopherList* itsList);
	virtual ~DGoLinkedTextDoc();

	virtual Boolean PushNewDoc( DGopher* itsGopher, DGopherList* itsList, Boolean havelinks = true); 
	virtual Boolean PushNewLocalDoc( DFile* docFile, DFile* goFile); 
	virtual Boolean IsMyAction(DTaskMaster* action); 
	virtual void ProcessTask(DTask* theTask);
	virtual Boolean ReadFrom(DFile* docFile, DFile* goFile);
	virtual void Open(DFile* docFile, DFile* goFile);
	virtual void Open();

	virtual void AddGolinksToDoc();
	virtual void MarkURLs();
	virtual void InsertLink( short golistItem, DGopher* theGo, short atparag, 
									short atchar, short atlen);
	virtual void InsertLink( DGopher* theGo, short atparag, short atchar, 
									short atlen);
	virtual void InsertLink( short golistItem, DGopher* theGo, Nlm_RecT arect, 
									short atparag, short atchar);
	virtual void InsertLink( DGopher* theGo, Nlm_RecT arect, short atparag, 
									short atchar);

	virtual void LinkToSelection( DGopher* aGopher);
	virtual void ExtractGopherDoc();
	virtual void SetupInlinePart( DRichStyle* st);
	virtual void SetupInlineParts();
	virtual Boolean ReadInlinePart();

	virtual void OpenGopherLink( short gopherItem, Nlm_PoinT* mappt = NULL);
	virtual void OpenGopherLink( DGopher* ag, Nlm_PoinT* mappt = NULL);
	virtual void Save(DFile* aFile);
};


class	DGopherInfoDoc : public DGopherTextDoc
{
public:	
	enum giTasks { kGoInfodoc = 235 };
	short fSavedType;
	DGopherInfoDoc(long id, DGopher* itsGopher) : DGopherTextDoc(id, itsGopher) {}
	virtual void ProcessTask(DTask* theTask);
	virtual void Open();
};


class	DGopherFileDoc : public DWindow
{
public:
	enum gfTasks { kGoFiledoc = 240, kLaunchBut };
	DGopher	* fParentGo;
	short		fFirststuff;
	DFile		* fFile;
	DPrompt * fBytesAvail;
	DPrompt * fBytesGot;
	DPrompt * fTimeSpent;
	DPrompt * fBytesPerSec;
	DButton * fLaunch;
	DGopherMap * fMapper;
	
	DGopherFileDoc(long id, DGopher* itsGopher);
	virtual ~DGopherFileDoc();
	virtual	Boolean suicide(void);  // prefered to delete 
	virtual	Boolean suicide(short ownercount);
	
	virtual Boolean IsMyTask(DTask* theTask);
	virtual void ProcessTask(DTask* theTask);
	virtual Boolean IsMyAction(DTaskMaster* action); 
	virtual void Open();
};


extern  DList*	gTextStoreList;
extern  DGopherTextDoc* gGopherTextDoc;

#endif
