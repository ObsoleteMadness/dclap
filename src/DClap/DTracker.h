// DTracker.h
// d.g.gilbert
// mouse tracker, now used by DPanel and children


#ifndef _DTRACKER_
#define _DTRACKER_

#include <ncbi.h>
#include "DCommand.h"

class DPanel;

class DTracker : public DCommand
{
public:
	enum TrackPhase { trackNone, trackBegin, trackContinue, trackEnd };
	TrackPhase 	fTrackPhase;
	Nlm_Boolean fTrackNonMovement, fMovedOnce;
	Nlm_PoinT  	fInitialPt, fAnchorPoint, fPreviousPoint, fNextPoint,
							fLastAnchorPoint, fLastPreviousPoint, fLastNextPoint;							
	DPanel		* fView;
	DTracker  * fCurrentTracker;
	
	DTracker();
	DTracker( long command, DTaskMaster* itsSource, char* itsTitle,
							   Nlm_Boolean canUndo, Nlm_Boolean causesChange,
							   DPanel* itsView);
	virtual void ITracker( long command, DTaskMaster* itsSource, char* itsTitle,
							   Nlm_Boolean canUndo, Nlm_Boolean causesChange,
							   DPanel* itsView);
	virtual void Reset();
	virtual void SetTracker(DTracker* newTracker);

	virtual DTracker* TrackMouse( TrackPhase aTrackPhase,
										Nlm_PoinT& anchorPoint, Nlm_PoinT& previousPoint,
										Nlm_PoinT& nextPoint,	Nlm_Boolean mouseDidMove);
	virtual void TrackFeedback(TrackPhase aTrackPhase,
										const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
										const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn);
	virtual DTracker* Start( Nlm_PoinT mouse);
	virtual DTracker* Continue( Nlm_PoinT mouse, Nlm_Boolean mouseMoved);
	virtual DTracker* Finish( Nlm_PoinT mouse);
	virtual Nlm_Boolean IsDoneTracking();
	
	virtual DTracker* TrackOnce( Nlm_Boolean didMouseMove);
	virtual void FeedbackOnce( Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn);
	virtual void BecomeTracker( DTracker* oldTracker);
};



#endif

