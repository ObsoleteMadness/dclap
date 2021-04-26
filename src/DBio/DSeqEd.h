// DSeqEd.h
// d.g.gilbert


#include <DWindow.h>
#include <DDialogText.h>
#include <DUtil.h>
#include <ncbi.h>

class DSequence;
class DSeqedView;
class DSeqDoc;
//class DSeqIndex;
//class	DSeqHIndex;
class DPopupList;
class DPrompt;
class DButton;
class DEditText;


class DSeqedWindow : public DWindow
{
public:
	enum { kId= 23242 };
	
	DSequence		* fSequence; // seq we are editing
	DSeqedView	* fSeqedView; 
	DEditText		* fSeqName;
	DPrompt 		* fSeqCheck, *fSeqStart, *fSeqEnd, *fSeqLen, *fSeqSel, *fSeqTyp;
	DPopupList	* fCodePop; //dna/rna/iupac/amino/all
	DSeqDoc			*	fMainDoc;

	DSeqedWindow(long id, DTaskMaster* itsSuperior, DSeqDoc* itsSeqDoc, DSequence* aSeq,
			short winwidth = -5, short winheight = -5, short winleft = -50, short wintop = -20, 
			char* title = NULL);
	virtual ~DSeqedWindow();
	
	static void  UpdateEdWinds( DSequence* oldSeq, DSequence* newSeq);
	virtual void ReplaceSeq(DSequence* aSeq);
	virtual void UpdateEdits(); 
	virtual Nlm_Boolean IsMyAction(DTaskMaster* action);
	virtual void Close();
};

		

class DSeqedView : public DDialogScrollText, public DKeyCallback
{
public:
	long			fOldStart, fOldEnd, fOldLen;  //save selStart,End for redraw check
	short			fOldKind;
	unsigned long fOldCheck;
	Nlm_RecT	fOldDest, fOldView; 		//save Dest Rect for SeqIndex redraw check
	DSequence	* fSequence;
	DPrompt 	* fSeqCheck, *fSeqStart, *fSeqEnd, *fSeqLen, *fSeqSel, *fSeqTyp;
	DPopupList	* fCodePop; //dna/rna/iupac/amino/all
	Boolean	fChanged; 			//flag if any edits in view
	DSeqedWindow	* fDoc;
	//DSeqIndex		* fSeqIndex;
	//DSeqHIndex	* fSeqHIndex;
	
	DSeqedView(long id, DView* itsSuperior, DSeqedWindow* itsDoc, DSequence* aSeq, 
							short width = 30, short height = 10);
	virtual ~DSeqedView();
	
	virtual void UpdateCtls(Boolean forceIndex);  //call this when seq indices change
	virtual void SetSeq(DSequence* aSeq); 
	
		//! Override some TEView methods to call UpdateCtls 
	virtual void selectAction();
	virtual void deselectAction();

  virtual void ProcessKey( char c);

	virtual Boolean DoMenuTask(long tasknum, DTask* theTask);
	virtual Boolean IsMyAction(DTaskMaster* action); 
		
		//keep cursor at arrow when teview is not selected (inactive "edittext");
	//virtual void DoSetCursor(VPoint localPoint, RgnHandle cursorRegion);	// override 
			
		//use to select this view for editing in DLog environment 
	//virtual Boolean HandleMouseDown( VPoint theMouse, TToolboxEvent event, hysteresis: Point); // override 

};



enum { kSeqDocPrefInit, kSeqDocPrefDialog };
extern Nlm_FonT  gSeqFont;

void SeqDocPrefs(short id);


#if 0
	TSeqIndex			== OBJECT (TControl);
		fTEView	: TTEView; 
		virtual void TSeqIndex::Initialize(void); // override 
		virtual void TSeqIndex::CalcMinFrame(VRect VAR minFrame); // override 
		virtual void TSeqIndex::Draw(VRect area); // override 
		}
		
	TSeqHIndex			== OBJECT (TControl);
		fTEView	: TTEView;
		virtual void TSeqHIndex::Draw(VRect area); // override 
		}
#endif
