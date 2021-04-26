//DSeqPrint.h
// d.g.gilbert, 1990-1995


#ifndef _DSEQPRINT_
#define _DSEQPRINT_

#include <ncbi.h>

#include <DTableView.h>
#include <DWindow.h>
#include <DMethods.h>


class DSeqList;
class DSeqPrintDoc;
class DSeqDoc;
class DCheckBox;
class DSwitchBox;
class DSeqPrintView;
class DPopupMenu;
class DEditText;

class	DSeqPrintDoc : public DWindow, public DSaveHandler, public DPrintHandler {
public:
	enum { kSeqPrintDoc = 31439 };
	
	DSeqPrintView	*	fView;
	//DSeqFormatPopup	* fFormatPop; 		 
	DCheckBox * fColorCheck, * fLockCheck;
	Boolean 	fUseColor;
	DSeqList* fSeqList;
	long 			fFirstBase, fNbases;

	//static Boolean fgUseColor;
	static Nlm_RecT	fgPrWinRect;
	static void GetGlobals();
	static void SaveGlobals();

	DSeqPrintDoc( long id, DSeqDoc* itsDoc, DSeqList* itsSeqList, long firstbase, long nbases);
	virtual ~DSeqPrintDoc();

	virtual void Open();
	virtual void Close();
	virtual void ResizeWin();
	virtual void Save(DFile* f) { WriteTo( f); }  	 
	virtual void Print();
	virtual void WriteTo(DFile* aFile);			// revise for iostreams
	
	virtual void MakeGlobalsCurrent();

	//virtual void ProcessTask(DTask* theTask);
	//virtual Boolean IsMyTask(DTask* theTask);
	//virtual Boolean DoMenuTask(long tasknum, DTask* theTask);
	//virtual Boolean IsMyAction(DTaskMaster* action);

	//virtual void ToTextDoc();

};


class	DAlnPrintDoc : public DSeqPrintDoc {
public:
	DAlnPrintDoc( long id, DSeqDoc* itsDoc, DSeqList* itsSeqList, long firstbase, long nbases);
};


class	DREMapPrintDoc : public DSeqPrintDoc {
public:
	DREMapPrintDoc( long id, DSeqDoc* itsDoc, DSeqList* itsSeqList, long firstbase, long nbases);
};


enum { kSeqPrintPrefInit, kSeqPrintPrefDialog, kAlnPrintPrefDialog, kREMapPrefDialog };

void SeqPrintPrefs(short id);


#endif

