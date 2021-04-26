// DSeqAsmView.h
// d.g.gilbert, 1990-1996

#ifndef _DSEQASMVIEW_
#define _DSEQASMVIEW_

#include "DSeqViews.h"


class DBaseCallPop;

class DAsmView : public DAlnView
{
public:
	enum { 
		kShowTraces= kModeMask1, 
		kShowTraceA, kShowTraceC,
		kShowTraceG, kShowTraceT
		};
		
	Boolean fHaveAutoseq, fBaseOnTrace;
	Boolean fShowTrace[4];
	DBaseCallPop*	fBaseCallPop;
	
	DAsmView( long id, DView* itsSuper, DSeqDoc* itsDocument, DSeqList* itsSeqList, long pixwidth, long pixheight);
	virtual ~DAsmView();

	virtual void Click(Nlm_PoinT mouse);
	virtual void Drag(Nlm_PoinT mouse);
	virtual void Hold(Nlm_PoinT mouse);
	virtual void Release(Nlm_PoinT mouse);
	virtual void DoubleClickAt(long row, long col);
	virtual void SingleClickAt(long row, long col);
	virtual void Scroll(Boolean vertical, DView* scrollee, long newval, long oldval);

	virtual void UpdateWidth( DSequence* aSeq);
	virtual void UpdateAllWidths(void);
	virtual void CheckViewCharWidth();
	virtual void GetReadyToShow();
	virtual void SetViewMode( short viewmode);

	virtual void TrackMouse( short aTrackPhase,
					Nlm_PoinT& anchorPoint, Nlm_PoinT& previousPoint,
					Nlm_PoinT& nextPoint,	Nlm_Boolean mouseDidMove);
	virtual void TrackFeedback( short aTrackPhase,
					const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
					const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn);

	virtual void DrawNoColors(Nlm_RecT r, long row);
	virtual void DrawAllColors(Nlm_RecT r, long row);
	virtual void DrawRow(Nlm_RecT r, long row);
	virtual void Draw();
				
};





#endif 
