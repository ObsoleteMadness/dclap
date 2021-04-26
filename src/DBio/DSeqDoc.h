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
class DPopupList;

class	DSeqDoc : public DWindow, public DSaveHandler, public DPrintHandler
{
public:

	enum sdTasks  { 
		kSeqdoc = 310, 
		kSeqMenu, kInternetMenu, 
		cSeqPrefs, cNewSeq, cEditSeq, cSaveSel, cRevert,
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
		cSel2Mask, cMask2Sel, cMaskOrSel, cMaskAndSel, cMaskReplicate, cMaskCompress,
		kModePopup, kColorPopup,
	  kLockButHit, kColorButHit, kMonoButHit 
		};

	enum  selectionFlags { 
		kSeqSel = 1, kNoSeqSel = 0,
		kMaskSel = 2, kNoMaskSel= 0,
		kIndexSel = 4, kNoIndexSel = 0,
		kAllIfNone = 8, kNoneIfNone = 0,
		kEqualCount = 16, kUnequalCount = 0,
		};
  
	static Boolean fgTestSeqFile, fgLockText, fgStartDoc;
	static short fgViewMode,fgUseColor,fgMatKind,fgDistCor;
	static char *fgSire;
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
	DPopupList	* fFormatPop; 			//seq output format
	DPopupList	* fModePop;
	DPopupList	* fColorPop;
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
	virtual void Revert();
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
	
	virtual void AddSeqToList(DSequence* item);
	virtual void AddNewSeqToList();
	virtual Boolean IsEmpty() { 
		if (fSeqList) return fSeqList->IsEmpty();
		else return true;
		}
	 					
				// revise for iostreams !?
	virtual short SelectionToFile(Boolean AllatNoSelection, 
													char* aFileName, short seqFormat); //return # written 
	virtual void GetSelection( long selectFlags, 
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
	virtual void NotDirty();

protected:
	virtual void AddViews(); 
	virtual void AddTopViews(DView* super);
	virtual void AddModePopup(DView* super);
	virtual void AddAlnIndex(DView* super, short width, short height);
	virtual void AddAlnView(DView* super, short width, short height);
	virtual void AddSeqAtToList( long aRow, long start1, long nbases1, 
										DSeqList*& aSeqList, long& start, long& nbases);
	virtual void AddMaskedSeqToList( long aRow, short masklevel,
										DSeqList*& aSeqList, long& start, long& nbases);
};


extern char* gDefSeqName;


#endif //_DSEQDOC_
