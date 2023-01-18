// DGopherDoc.h
// d.g.gilbert


#ifndef _DGOLISTDOC_
#define _DGOLISTDOC_


#include <DWindow.h>
#include <DPanel.h>
#include <DTableView.h>
#include <Dvibrant.h>
#include <DGoList.h>
#include <DUtil.h>

class DGopher;
class DFile;
class DPrompt;
class	DMenu;
class DPopupList;


class DGolistView : public DTableView
{
public:
	DGopherList* fItems;
	DGopher*	fDragGopher;
	Boolean		fDragging;

	DGolistView(long id, DView* itsSuperior, DGopherList* itsItems,
							short pixwidth = 250, short pixheight = 200);

	virtual void Drag(Nlm_PoinT mouse);
	virtual void Hold(Nlm_PoinT mouse);
	virtual void Release(Nlm_PoinT mouse);
	
	virtual void SingleClickAt(long row, long col);
	virtual void DoubleClickAt(long row, long col);
	virtual void DrawCell(Nlm_RecT r, long row, long col);

	virtual void SetColWidths();
	virtual void SizeToSuperview( DView* super, Boolean horiz, Boolean vert); 
	virtual void Resize(DView* superview, Nlm_PoinT sizechange);
	virtual void SetLineHeight( long item, DGopher* theGo);

	virtual DGopher* SelectedGopher();
	virtual void OpenGopherLink(long gopherItem);
};



class	DGopherListDoc : public DWindow, public DSaveHandler
{
public:
	static Boolean gOptionIsOn;
	static Boolean gFetchSingles;
	static char* kGopherDocSuffix;
	static Boolean IsGopherDoc(DFile* aFile);
	static void  ProcessFile( DFile* aFile);
	static DMenu * gViewChoiceMenu;		// app submenu
	static DView * gLockWinMenuItem;  // app menu item
	static void ReadItemInfo(DGopher* theGo);
	static void ProcessGopher(DGopher* theGo, short itsViewChoice);
	static Boolean ProcessGopherWithTest( DGopher* theGo);
	static void NewNetdoc( DGopher* theGo, DTask* readTask);
	static void NewGopherDoc();
	static Boolean GopherByViewDialog(DGopher* go, DGopherList* itsList); 
	static short DefaultGopherView(DGopher* aGopher);

	enum glTasks  { kGoListdoc = 210, kViewDefault, kViewChoiceMenu, 
									kViewKindMenu, kViewByDefault, kViewByDate, kViewBySize, kViewByName,
									kViewByKind, kViewByHost, kViewAsText, kViewAsLink,
									cPushpin, cParentDrop, cParentPopup };
	DGolistView	*	fGoview;
 	DGopherList *	fGolist, * fOldlist;
 	DList 			*	fParentlist; // list of DGopherList ??
 	DGopher 		*	fParent;
 	short 			fFirststuff;
	DPrompt			*	fStatus;
	Boolean			fInUse, fPinned;
	DPopupList	* fParentPop;
	DIconButton * fStickpin; 

	DGopherListDoc( long id = 0);
	DGopherListDoc( long id, DGopher* parentGopher);
	DGopherListDoc( long id, DGopherList* activeList, char* name = NULL);
	virtual ~DGopherListDoc();
	virtual void Initialize( char* name);
	virtual void SetInfoLine();

	virtual void Save(DFile* f) { WriteTo( f); } // DSaveHandler
	virtual void WriteTo (DFile* aFile = NULL);

	virtual Boolean CanReadFrom( DFile*& aFile);
	virtual Boolean CanReadFrom( DGopher* itsParent);
	virtual Boolean ReadFrom(DFile* aFile);
	virtual Boolean ReadFrom(DGopher* itsParent);
	virtual void Open( DFile* aFile);
	virtual void Open( DGopher* itsParent);
	virtual void Open( Boolean haveData = false);
	virtual void Close();

	virtual void ResizeWin();
	virtual void Activate();
	virtual void Deactivate();
	virtual void ProcessTask(DTask* theTask);
	virtual Boolean IsMyTask(DTask* theTask);
	virtual Boolean IsMyAction(DTaskMaster* action);
	virtual void ShowMessage(const char* msg);
	virtual void MakeSubviews();
	virtual void PinWindow(Boolean turnon);

	virtual void EmptyData(Boolean saveit= true);
	virtual Boolean ReplaceData( DGopher* parentGopher, char* name, DFile* aFile);
	virtual Boolean ReplaceData( DGopherList* parentlist);
	virtual Boolean PopData( short numitems = 1);
	
	virtual void AddData(char* gopherData, long datalen);
	virtual void UpdateListSize();
	virtual void AddGopherToView( DGopher* aGopher);
	virtual void TopOfGopherHole();
	virtual void InsertNewGopher(Boolean editAll);
	virtual void EditOldGopher();
	virtual void CutClearSelection( Nlm_Boolean saveCut);

	virtual void SortView( DGopherList::Sorts sortorder);
	virtual void ToTextDoc( Boolean ViewAsLink);
	virtual void SetViewMenu(Boolean activate = true);

};






extern  Nlm_RecT  gGopherListDocRect;
extern  DGopherListDoc* gGopherDoc;
extern 	Nlm_Boolean gDoubleClicker, gSingleClicker;


#endif
