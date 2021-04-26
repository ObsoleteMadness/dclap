// DSeqDoc.h
// d.g.gilbert, 1990-1994

#ifndef _DSEQDOC_
#define _DSEQDOC_

#include "DSequence.h"
#include "DSeqList.h"
#include <DTableView.h>
#include <DWindow.h>
#include <DMethods.h>
#include <DREnzyme.h>
#include <DSeqViews.h>


class DPrompt;
class DCheckBox;
class DMenu;
class DSeqFormatPopup;
class DAlnModePopup;
class DAlnColorPopup;

class	DSeqDoc : public DWindow, public DSaveHandler, public DPrintHandler
{
public:

	enum sdTasks  { 
		kSeqdoc = 310, 
		kSeqMenu, kInternetMenu, 
		cSeqPrefs, cNewSeq, cEditSeq, cSaveSel,
		cRevSeq,cCompSeq,cRevCompSeq,cDna2Rna,cRna2Dna,cToUpper,cToLower,cDegap,
		cLockIndels,cUnlockIndels,cConsensus,cTranslate, cDistance, cSimilarity,
		cPrettyPrint,cREMap,cDotPlot,cNAcodes, cAAcodes,
		cFindORF,  
		
		cNCBIfetch, cNCBIblast, 
		cEMBLfetch, cEMBLfasta, cEMBLblitz, cEMBLquicks,
		cFHCRCfetch, cFHCRCblocks,
		cGeneidSearch, cGenmarkSearch, cGrailSearch,cPythiaSearch, 

		kViewKindMenu, kViewByDefault,kViewByDate,kViewBySize,kViewByName,kViewByKind,kViewAsText,
		kSeqMaskMenu, cMaskSelCommon,cMaskSelORF, cMaskSelAll,cMaskInvert,cMaskClear,
		cSel2Mask, cMask2Sel, cMaskOrSel, cMaskAndSel,
		kModePopup, kColorPopup,
	  kLockButHit, kColorButHit, kMonoButHit 
		};

	static Boolean fgTestSeqFile, fgLockText, fgStartDoc;
	static short fgViewMode,fgUseColor,fgMatKind,fgDistCor;
	static Nlm_RecT	fgWinRect;
	static void GetGlobals();
	static void SaveGlobals();
	static void NewSeqDoc();

 	DSeqList	*	fSeqList;
	DAlnView	*	fAlnView;
	DAlnIndex 	* fAlnIndex;
	DAlnHIndex 	* fAlnHIndex;
	DAlnITitle	*	fAlnITitle;
	DPrompt	*	fHeadline; 				//seqlist header
	DSeqFormatPopup	* fFormatPop; 			//seq output format
	DAlnModePopup	* fModePop;
	DAlnColorPopup	* fColorPop;
	DPrompt	*	fSeqMeter; 				//list base#, other info?

	long			fUpdateTime;			//time of last .showreverted/.updateFlds 
	short			fInFormat;				//file input format 
	Boolean		fSaveSelection, fUseColor;
	DCheckBox * fColorCheck, * fLockCheck;
	//DIconButton		* fLockButton, * fColorButton, * fMonoButton;
	DWindow		*	fPrintDoc;
	char			*	fDocTitle;
	
	DSeqDoc( long id, DSeqList* itsSeqList, char* name = NULL);
	virtual ~DSeqDoc();

	static Boolean IsSeqFile(DFile* aFile);
	static void SetUpMenu(short menuId, DMenu*& aMenu); // !! can't do virtual statics !!

	virtual void Save(DFile* f) { WriteTo( f); }  // revise for iostreams
	virtual void WriteTo (DFile* aFile = NULL);			// revise for iostreams
	virtual Boolean ReadFrom(DFile* aFile = NULL, Boolean append = true);	// revise for iostreams

	virtual void Open();
	virtual void Open(DFile* aFile);
	virtual void Close();
	virtual void ResizeWin();
	virtual void Activate();
	virtual void Deactivate();
	virtual void ProcessTask(DTask* theTask);
	virtual Boolean IsMyTask(DTask* theTask);
	virtual Boolean DoMenuTask(long tasknum, DTask* theTask);
	virtual Boolean IsMyAction(DTaskMaster* action);
	virtual void FreeData();
	virtual char* GetTitle(char* title, ulong maxsize);

	virtual void MakeGlobalsCurrent();
	virtual void SortView( DSeqList::Sorts sortorder);
	virtual void ToTextDoc();

////		
	
	virtual void AddSeqToList(DSequence* item);
	virtual void AddNewSeqToList();
	virtual Boolean IsEmpty() { 
		if (fSeqList) return fSeqList->IsEmpty();
		else return true;
		}
	 
	//virtual void DoSeqSelectCommand(); //override this to open sequences selected w/ dblclk
	//virtual void ShowReverted(); // override 
	//virtual void DoMakeViews(Boolean forPrinting); // override 
					
////

				// revise for iostreams !?
	virtual short SelectionToFile(Boolean AllatNoSelection, 
													char* aFileName, short seqFormat); //return # written 

	virtual void GetSelection(Boolean equalCount, Boolean allAtNoSelection,
														DSeqList*& aSeqList, long& start, long& nbases);
	virtual void FirstSelection( DSequence*& aSeq, long& start, long& nbases);		
	virtual void OpenSeqedWindow(DSequence* aSeq);
	virtual void EditSeqs(); 
	virtual void FindORF(); 
	virtual void MakeConsensus(); 
	virtual void MakeSeqPrint(Boolean doREMap);
	virtual void MakeAlnPrint();
	virtual void Print(); 
	virtual void DistanceMatrix(short form);
	
	//virtual void DoEvent(EventNumber eventNumber, TEventHandler source; TEvent event) // override 
	//virtual void DoMenuCommand(short aCommandNumber); // override 
	//virtual void DoSetupMenus(); // override 

private: 		 
	void AddSeqAtToList( short aRow, long start1, long nbases1, 
										DSeqList*& aSeqList, long& start, long& nbases);
};


extern char* gDefSeqName;


#endif //_DSEQDOC_
