// DSeqViews.h
// d.g.gilbert, 1990-1996

#ifndef _DSEQVIEWS_
#define _DSEQVIEWS_

#include "DSequence.h"
#include "DSeqList.h"
#include <DTableView.h>
#include <DDialogText.h>
#include <DREnzyme.h>
#include <DUtil.h>


class DSeqDoc;
class DAlnSlider;
class DAlnView;


class DAlnSequence : public DTextLine, public DKeyCallback
{
public:
	DSequence* fSeq;
	Boolean  fVisible;
	DAlnView*	fAlnView;
	long		fNcols;
	
	DAlnSequence(long id, DAlnView* itsSuperior, short charwidth = 1);
  virtual void Scroll(Boolean vertical, DView* scrollee, long newval, long oldval);		
	virtual void ShowEdit(Boolean newseq = true);
	virtual void HideEdit();
  virtual void ProcessKey( char c);
	virtual void selectAction();
	virtual void deselectAction();
	virtual void SetColumns( long ncolumns);
	virtual Boolean IsMyAction(DTaskMaster* action);
	virtual void Select(long start, long length);
};


class DAlnView : public DTableView
{
public:
	DSeqList	* fSeqList;  
	DSeqDoc		*	fDoc; 		 
	DAlnSequence * fEditSeq;  
	long			fEditRow;
	short			fMaskLevel;
	Boolean		fLocked, fOwnSeqlist;	 
	DAlnSlider	* fSlider;
	DSequence		* fCurSeq; // temp used in draw, elsewhere?
	unsigned long	*	fColcolors;
	short		fNcolcolors, fColorBases;
	
	enum { kindAlnView = 23345, idAlnView = 22902 };
	enum viewmodes { kModeSlide,kModeEdit,kModeMask1,kModeMask2,kModeMask3,kModeMask4 };
	enum viewcolors { kBaseBlack, kBaseColor, kBaseVarLite };
	
	DAlnView( long id, DView* itsSuper, DSeqDoc* itsDocument, DSeqList* itsSeqList, long pixwidth, long pixheight);
	virtual ~DAlnView();

	virtual DSequence* SeqAt( long aRow) 
		{
		if (fSeqList) return fSeqList->SeqAt(aRow);
		else return NULL;
		}

	virtual void Click(Nlm_PoinT mouse);
	virtual void Drag(Nlm_PoinT mouse);
	virtual void Hold(Nlm_PoinT mouse);
	virtual void Release(Nlm_PoinT mouse);
	virtual void DoubleClickAt(long row, long col);
	virtual void SingleClickAt(long row, long col);
	virtual void CharHandler(char c); 
	virtual void SeqMeter(long row, long col);
	virtual DSequence* SelectedSequence(long& selectedRow);
	virtual DSequence* SelectedSequence() { 
		long selectedRow; return SelectedSequence(selectedRow);
		}
	virtual void Scroll(Boolean vertical, DView* scrollee, long newval, long oldval);

	virtual void UpdateWidth( DSequence* aSeq);
	virtual void UpdateAllWidths(void);
	virtual void UpdateSize(void);
	virtual void CheckViewCharWidth();
	
	virtual void addToAlnList( DSequence* aSeq);
	virtual void registerInsertLast( DSequence* aSeq);
	virtual void MakeConsensus();
	virtual char* FindCommonBases( short minCommonPerCent);
	virtual void HiliteCommonBases();
	virtual void HiliteORFs();
	virtual Boolean MaskCommand(short command);
	virtual void ReplicateMask();
	virtual Boolean IsMasked();

	virtual void DeInstallEditSeq();
	virtual void InstallEditSeq(long row, long selStart, long selEnd, Boolean doLight);
	virtual void SetTextLock( Boolean turnon);
	virtual void SetViewMode( short viewmode);
	virtual void SetViewColor( short colorkind);

		
	virtual void TrackMouse( short aTrackPhase,
					Nlm_PoinT& anchorPoint, Nlm_PoinT& previousPoint,
					Nlm_PoinT& nextPoint,	Nlm_Boolean mouseDidMove);
	virtual void TrackFeedback( short aTrackPhase,
					const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
					const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn);

	virtual void Resize(DView* superview, Nlm_PoinT sizechange);
	virtual void GetReadyToShow();
	virtual void Show();
	virtual void DrawAlnInStyle(baseColors colors, Boolean swapBackColor, 
															char*	pText, long indx, long len, long row);
	virtual void DrawAlnColors(baseColors colors, Boolean swapBackColor, 
															char*	pText, long indx, long len, long row);
	virtual void DrawNoColors(Nlm_RecT r, long row);
	virtual void DrawAllColors(Nlm_RecT r, long row);
	virtual void DrawRow(Nlm_RecT r, long row);
	virtual void Draw();
				
	//virtual Boolean ContainsClipType(ResType aType); // override 
	//virtual void WriteToDeskScrap(void); // override 
	
	//virtual void DoMouseCommand(VPoint VAR theMouse, TToolboxEvent event, Point hysteresis); // override 
	//virtual Boolean HandleMouseDown( VPoint theMouse, TToolboxEvent event, hysteresis: Point); // override 
};


class DAlnITitle : public DTableView
{
public:
	short				fNameWidth, fSizeWidth, fKindWidth;	//max #chars in name column
	DAlnITitle( long id, DView* itsSuper, long pixwidth, long pixheight);
	virtual void GetReadyToShow();
	virtual void Click(Nlm_PoinT mouse);
	virtual void DoubleClickAt(long row, long col);
	virtual void SingleClickAt(long row, long col);
	virtual void DrawCell(Nlm_RecT r, long row, long col);
};

class DAlnIndex : public DTableView  
{
public:
	DSeqList* 	fSeqList; // NOTE: TSeqListDoc "owns" this list
	DSeqDoc*		fDoc; 		// our owner
	short				fNameWidth, fSizeWidth, fKindWidth;	//max #chars in name column
	
	DAlnIndex( long id, DView* itsSuper, DSeqDoc* itsDocument, DSeqList* itsSeqList, long pixwidth, long pixheight);
	virtual void Click(Nlm_PoinT mouse);
	virtual void Drag(Nlm_PoinT mouse);
	virtual void Hold(Nlm_PoinT mouse);
	virtual void Release(Nlm_PoinT mouse);
	virtual void DoubleClickAt(long row, long col);
	virtual void SingleClickAt(long row, long col);
	virtual void Scroll(Boolean vertical, DView* scrollee, long newval, long oldval);
	virtual void GetReadyToShow();
	virtual void Show();
	virtual char* GetItemTitle(short item, char* title = NULL, size_t maxsize = 256);  
	virtual void DrawCell(Nlm_RecT r, long row, long col);
};

class DAlnHIndex : public DPanel  
{
public:
	DSeqList* 	fSeqList; // NOTE: TSeqListDoc "owns" this list
	DSeqDoc*		fDoc; 		// our owner
	long				fLastCol;
	
	DAlnHIndex( long id, DView* itsSuper, DSeqDoc* itsDocument, DSeqList* itsSeqList, long pixwidth, long pixheight);
	virtual void Resize(DView* superview, Nlm_PoinT sizechange);
	virtual void Drag(Nlm_PoinT mouse);
	virtual void Hold(Nlm_PoinT mouse);
	virtual void Release(Nlm_PoinT mouse);
	virtual void Click(Nlm_PoinT mouse);
	virtual void ClickColumn(Nlm_PoinT mouse);
	virtual void Scroll(Boolean vertical, DView* scrollee, long newval, long oldval);
	virtual void Draw();
};




#endif //_DSEQVIEWS_
