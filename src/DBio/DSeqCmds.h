// DSeqCmds.h

#ifndef _DSEQCMDS_
#define _DSEQCMDS_


#include "DCommand.h"
#include "DTableView.h"
#include "ncbi.h"

class DSeqDoc;
class DSeqList;
class DSequence;
class DAlnView;

class DSeqChangeCmd : public DCommand
{
public:
	enum { cSeqChange = 30594 };
	DSeqList * fOldSeqs, * fNewSeqs;
	DAlnView * fAlnView;
	DSeqDoc * fSeqDoc;
	
	DSeqChangeCmd( char* title, DSeqDoc* itsAlnDoc, DView* itsView, DSeqList* itsSeqs);
	virtual ~DSeqChangeCmd();
	virtual void DoIt();
	virtual void Undo();
	virtual void Redo();
	virtual void Commit();

	virtual Boolean Initialize();  
	virtual void DoItWork();
	virtual void UndoWork();
	virtual	DSequence* ChangeToNew( DSequence* oldSeq);
};


class DSeqReverseCmd : public DSeqChangeCmd {
public: 
	DSeqReverseCmd( DSeqDoc* itsAlnDoc, DView* itsView, DSeqList* itsSeqs):
		 DSeqChangeCmd("reverse", itsAlnDoc,itsView,itsSeqs) {}
	virtual	DSequence* ChangeToNew( DSequence* oldSeq);
};

class DSeqComplementCmd : public DSeqChangeCmd {
public:
	DSeqComplementCmd( DSeqDoc* itsAlnDoc, DView* itsView, DSeqList* itsSeqs): 
		DSeqChangeCmd("complement", itsAlnDoc,itsView,itsSeqs) {}
	virtual	DSequence* ChangeToNew( DSequence* oldSeq);
};

class DSeqRevComplCmd : public DSeqChangeCmd {
public: 
	DSeqRevComplCmd( DSeqDoc* itsAlnDoc, DView* itsView, DSeqList* itsSeqs):
		DSeqChangeCmd("rev-compl", itsAlnDoc,itsView,itsSeqs) {}
	virtual	DSequence* ChangeToNew( DSequence* oldSeq);
};

class DSeqCompressCmd : public DSeqChangeCmd {
public: 
	DSeqCompressCmd( DSeqDoc* itsAlnDoc, DView* itsView, DSeqList* itsSeqs): 
		DSeqChangeCmd("degap", itsAlnDoc,itsView,itsSeqs) {}
	virtual	DSequence* ChangeToNew( DSequence* oldSeq);
};

class DSeqDna2RnaCmd : public DSeqChangeCmd {
public: 
	DSeqDna2RnaCmd( DSeqDoc* itsAlnDoc, DView* itsView, DSeqList* itsSeqs): 
		DSeqChangeCmd("dna2rna", itsAlnDoc,itsView,itsSeqs) {}
	virtual	DSequence* ChangeToNew( DSequence* oldSeq);
};

class DSeqRna2DnaCmd : public DSeqChangeCmd {
public: 
	DSeqRna2DnaCmd( DSeqDoc* itsAlnDoc, DView* itsView, DSeqList* itsSeqs): 
		DSeqChangeCmd("rna2dna", itsAlnDoc,itsView,itsSeqs) {}
	virtual	DSequence* ChangeToNew( DSequence* oldSeq);
};

class DSeqTranslateCmd : public DSeqChangeCmd {
public: 
	DSeqTranslateCmd( DSeqDoc* itsAlnDoc, DView* itsView, DSeqList* itsSeqs): 
		DSeqChangeCmd("translate", itsAlnDoc,itsView,itsSeqs) {}
	virtual	DSequence* ChangeToNew( DSequence* oldSeq);
};

class DSeqLockIndelsCmd : public DSeqChangeCmd {
public: 
	DSeqLockIndelsCmd( DSeqDoc* itsAlnDoc, DView* itsView, DSeqList* itsSeqs): 
		DSeqChangeCmd("lock indels", itsAlnDoc,itsView,itsSeqs) {}
	virtual	DSequence* ChangeToNew( DSequence* oldSeq);
};

class DSeqUnlockIndelsCmd : public DSeqChangeCmd {
public: 
	DSeqUnlockIndelsCmd( DSeqDoc* itsAlnDoc, DView* itsView, DSeqList* itsSeqs): 
		DSeqChangeCmd("unlock indels", itsAlnDoc,itsView,itsSeqs) {}
	virtual	DSequence* ChangeToNew( DSequence* oldSeq);
};

class DSeqUppercaseCmd : public DSeqChangeCmd {
public: 
	DSeqUppercaseCmd( DSeqDoc* itsAlnDoc, DView* itsView, DSeqList* itsSeqs): 
		DSeqChangeCmd("uppercase", itsAlnDoc,itsView,itsSeqs) {}
	virtual	DSequence* ChangeToNew( DSequence* oldSeq);
};

class DSeqLowercaseCmd : public DSeqChangeCmd {
public: 
	DSeqLowercaseCmd( DSeqDoc* itsAlnDoc, DView* itsView, DSeqList* itsSeqs): 
		DSeqChangeCmd("lowercase", itsAlnDoc,itsView,itsSeqs) {}
	virtual	DSequence* ChangeToNew( DSequence* oldSeq);
};



class DAlnSlider : public DRCshifter 
{
public:
	enum { cAlnShiftCmd = 2451 };
	DAlnView	* fAlnView;
	DSeqDoc		* fSeqDoc;
	DSeqList	* fNewSeqs, * fOldSeqs;
	Nlm_RecT		fOldSelection, fNewSelection; //Nlm_RgN later ...
	Nlm_Boolean	fSlideAll;
	
	DAlnSlider();
	DAlnSlider( DSeqDoc* itsDoc, DTableView* itsView, DAlnView* itsAlnView,  short oldRC);
	virtual ~DAlnSlider();
	virtual void IAlnSlider( DSeqDoc* itsDoc, DTableView* itsView, DAlnView* itsAlnView,  short oldRC);
	virtual void Reset();
	virtual void TrackFeedback( TrackPhase aTrackPhase,
					const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
					const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn);
	virtual void DoIt(); 
	virtual void Undo();
	virtual void Redo(); 

	virtual void DoItWork(); 
	virtual void UndoWork(); 
	virtual void DoSlide();
};
	

class DAlnShifter : public DAlnSlider 
{
public:
	DAlnShifter();
	DAlnShifter( DSeqDoc* itsDoc, DTableView* itsView, DAlnView* itsAlnView,  short oldRC);
	virtual ~DAlnShifter();
	virtual void IAlnSlider( DSeqDoc* itsDoc, DTableView* itsView, DAlnView* itsAlnView,  short oldRC);
	virtual void DoItWork(); 
	virtual void UndoWork(); 
	virtual void DoSlide();
	virtual void TrackFeedback( TrackPhase aTrackPhase,
					const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
					const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn);
};
	


class DAlnEditCommand : public DCommand
{
public:
	enum { cAlnEdit = 31596 };
	DSeqList * fOldList;
	DSeqDoc  * fAlnDoc;
	Nlm_RecT	 fSelection;

	DAlnEditCommand(DSeqDoc* itsDoc, short itsCommand);
	virtual ~DAlnEditCommand();
	virtual void DoIt();
	virtual void Undo();
	virtual void Redo();
	virtual void Commit();

	virtual void CopySelection();
	virtual void DeleteSelection();
	virtual void RestoreSelection();
	virtual void ReSelect();

	virtual void DoItWork();
	virtual void UndoWork();
	virtual void RedoWork();

};

class DAlnPasteCommand : public DCommand
{
public:
	enum { cAlnPaste = 31597 };
	DSeqList * fClipList;
	DSeqDoc  * fAlnDoc;
	short    fInsRow;
	
	DAlnPasteCommand(DSeqDoc* itsDoc);
	virtual ~DAlnPasteCommand();
	virtual void DoIt();
	virtual void Undo();
	virtual void Redo();
	virtual void Commit();

	virtual void DoItWork();
	virtual void UndoWork();
};



#endif

