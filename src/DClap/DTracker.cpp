// DTracker.cpp
// d.g.gilbert

// class DTracker  : public DCommand
// mouse tracker, now used by DPanel and children
// from MacApp TTracker

#include <ncbi.h>
#include "DCommand.h"
#include "DPanel.h"
#include "DTracker.h"



static Nlm_PoinT gZeroPt = { 0, 0 };


DTracker::DTracker()
{
	ITracker( 0, NULL, NULL, FALSE, FALSE, NULL);
}  

DTracker::DTracker( long command, DTaskMaster* itsSource, char* itsTitle,
							   Nlm_Boolean canUndo, Nlm_Boolean causesChange,
							   DPanel* itsView)
{
	ITracker( command, itsSource, itsTitle, canUndo, causesChange, itsView);
}


void DTracker::ITracker( long command, DTaskMaster* itsSource, char* itsTitle,
							   Nlm_Boolean canUndo, Nlm_Boolean causesChange,
							   DPanel* itsView)
{
	fView= NULL;
	fTrackNonMovement = FALSE;
	fInitialPt = gZeroPt;
	//fConstrainsMouse = FALSE;
	//fScroller = NULL;
	//fViewConstrain = TRUE;
	//fDeskTopTrackingPort = NULL;
	//fHysteresis = gZeroPt;

	this->ICommand( command, itsSource, itsTitle, canUndo, causesChange);
	fView = itsView;
	fCurrentTracker= this;

	Reset();
}

void DTracker::SetTracker(DTracker* newTracker)
{
	fCurrentTracker= newTracker;
}

void DTracker::Reset()
{
	fTrackPhase = trackBegin;
	fAnchorPoint = gZeroPt;
	fPreviousPoint = gZeroPt;
	fNextPoint = gZeroPt;
	fLastAnchorPoint = gZeroPt;
	fLastPreviousPoint = gZeroPt;
	fLastNextPoint = gZeroPt;
	fMovedOnce = FALSE;	
}

Nlm_Boolean DTracker::IsDoneTracking()
{
	if (fView) return fView->fMouseStillDown;
	else return true;
}  

#if 0
	// this isn't any useful w/ Vibrant as long as it controls mouse tracking...
void DTracker::Process() // override DTask::Process()
{
	DTracker* finalCommand = NULL;
	DCommandHandler* handler = NULL;
	Nlm_PoinT initpt= fInitialPt;
	finalCommand = gApplication->TrackMouse(initpt, gStdHysteresis, this);
		if (finalCommand) {
		handler = finalCommand->fContext;
		if (handler) handler->PerformCommand(finalCommand);
		}
}  
#endif



// the DView/DPanel that owns this tracker does TrackMouse job (if aware)

DTracker* DTracker::TrackMouse(TrackPhase aTrackPhase,
										Nlm_PoinT& anchorPoint, Nlm_PoinT& previousPoint,
										Nlm_PoinT& nextPoint,	Nlm_Boolean mouseDidMove)
{
	fCurrentTracker= this;
	if (fView)
		fView->TrackMouse(aTrackPhase, anchorPoint, previousPoint, nextPoint, mouseDidMove);

	if (aTrackPhase == trackEnd) {
		if (fView == NULL) return NULL;
		else {	
			Nlm_RecT viewr;
			fView->ViewRect(viewr);
			if (!Nlm_PtInRect(nextPoint, &viewr)) return NULL;
			}
		}
	return fCurrentTracker;
}


// the DView/DPanel that owns this tracker does TrackFeedback job (if aware)

void DTracker::TrackFeedback(TrackPhase aTrackPhase,
										const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
										const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn)
{
	if (fView)
		fView->TrackFeedback(aTrackPhase, anchorPoint, previousPoint, nextPoint, mouseDidMove, turnItOn);
}  




DTracker* DTracker::Start( Nlm_PoinT mouse)
{
	if (Nlm_dblClick) {
		//?? return this; ??
		}

	fTrackPhase = trackBegin;
	fMovedOnce = FALSE;
	fAnchorPoint = mouse;
	fPreviousPoint = mouse;
	fNextPoint = mouse;
	if (fInitialPt.x == gZeroPt.x && fInitialPt.y == gZeroPt.y) fInitialPt= mouse; // dgg added
	
	this->BecomeTracker(NULL);
	//this->ConstrainOnce(TRUE);
	// in case Constrain changed the fNextPointÉ guarantee that all 3 are the same on trackBegin
	//fAnchorPoint = fNextPoint;
	//fPreviousPoint = fNextPoint;

	DTracker* theTracker = this->TrackOnce(TRUE);
	if (theTracker) {
		// in case TrackMouse changed the fNextPointÉ guarantee that all 3 are the same on trackBegin
		theTracker->fAnchorPoint = theTracker->fNextPoint;
		theTracker->fPreviousPoint = theTracker->fNextPoint;
		theTracker->FeedbackOnce(TRUE, TRUE);
		}

	return theTracker;
}


DTracker* DTracker::Continue( Nlm_PoinT mouse, Nlm_Boolean mouseMoved)
{
	fTrackPhase = trackContinue;				// supply trackphase to all interested parties
	fNextPoint = mouse;

	if (!fMovedOnce) {
#if 1
		fMovedOnce= mouseMoved;
#else
		//this->ConstrainOnce(fPreviousPoint != fNextPoint);		 
		Nlm_PoinT amtMoved = fNextPoint - fAnchorPoint;
		if ((abs((int)amtMoved.x) >= fHysteresis.x) || (abs((int)amtMoved.y) >= fHysteresis.y))
			fMovedOnce = TRUE;
#endif
		}

#if FIX_LATER
	Nlm_PoinT delta = gZeroPt;
	if (fMovedOnce || fTrackNonMovement) {
		if (fScroller && fView) 	{
			// convert fNextPoint to scroller coordinates
			Nlm_PoinT mouseInScroller = fNextPoint;
			fView->LocalToWindow(mouseInScroller);
			fScroller->WindowToLocal(mouseInScroller);

			// AutoScroll if the CPoint is outside of the scroller's extent
			Nlm_PoinT autoScrollLimit;
			fScroller->GetExtent(autoScrollLimit);
			if (!autoScrollLimit.Contains(mouseInScroller)) {
				fScroller->AutoScroll(mouseInScroller, delta);// Get the amount to autoscroll
				fNextPoint += delta;
				}
			}
		//this->ConstrainOnce(fPreviousPoint != fNextPoint);
		}
#endif

	Boolean willScroll = false; //delta != gZeroPt;
	Boolean didMove = (fPreviousPoint.x != fNextPoint.x || fPreviousPoint.y != fNextPoint.y);

	this->FeedbackOnce( didMove || willScroll, FALSE);

#if FIX_LATER
	if (willScroll) {
		this->AutoScroll(delta);			// OK, now actually do the scrolling 
		if (fView) fView->Update();		// Keep synchronized. ScrollDraw only
		//this->DoFocus();	 
		}
#endif

	DTracker* theTracker = this->TrackOnce(didMove);
	if (theTracker) {
		theTracker->FeedbackOnce( didMove || willScroll, TRUE);
		theTracker->fPreviousPoint = theTracker->fNextPoint;
		}

	return theTracker;
}  



DTracker* DTracker::Finish( Nlm_PoinT mouse)
{
	fTrackPhase = trackEnd;					 
	fNextPoint = mouse;

	this->FeedbackOnce(TRUE, FALSE);
	DTracker* theTracker = this->TrackOnce(TRUE);
	//if (theTracker) theTracker->CleanUpFocus();
	return theTracker;
}  





DTracker* DTracker::TrackOnce( Nlm_Boolean didMouseMove)
{
	DView* oldView = fView;
	Nlm_PoinT anchorPoint = fAnchorPoint;
	Nlm_PoinT previousPoint = fPreviousPoint;
	Nlm_PoinT nextPoint = fNextPoint;

	DTracker * newTracker = 
		this->TrackMouse(fTrackPhase, anchorPoint, previousPoint, nextPoint, didMouseMove);
	
	fAnchorPoint = anchorPoint;
	fPreviousPoint = previousPoint;
	fNextPoint = nextPoint;
	
	if (newTracker == this) {
		if (this->fView != oldView) this->BecomeTracker(this);
		}
	else {
		if (newTracker) newTracker->BecomeTracker(this);
		//else this->CleanUpFocus();
		this->suicide(); //Free();
	}

	return newTracker;
} 


void DTracker::BecomeTracker(DTracker* oldTracker)
{
	if (oldTracker) {
		//fDeskTopTrackingPort = oldTracker->fDeskTopTrackingPort;
		//fHysteresis = oldTracker->fHysteresis;
		fTrackPhase = oldTracker->fTrackPhase;
		fAnchorPoint = oldTracker->fAnchorPoint;
		fPreviousPoint = oldTracker->fPreviousPoint;
		fNextPoint = oldTracker->fNextPoint;
		fLastAnchorPoint = oldTracker->fLastAnchorPoint;
		fLastPreviousPoint = oldTracker->fLastPreviousPoint;
		fLastNextPoint = oldTracker->fLastNextPoint;
		}

	//this->DoFocus();
}  

void DTracker::FeedbackOnce( Boolean mouseDidMove, Boolean turnItOn)
{
	Nlm_PoinT ancr, prev, next;
	//Nlm_InvertMode();
	
	if (turnItOn) {
		ancr= fAnchorPoint;
		prev= fPreviousPoint;
		next= fNextPoint;
		this->TrackFeedback( fTrackPhase, ancr, prev, next, mouseDidMove, turnItOn);
		// save these to turn it off with
		if (mouseDidMove) {
			fLastAnchorPoint = fAnchorPoint;
			fLastPreviousPoint = fPreviousPoint;
			fLastNextPoint = fNextPoint;
			}
		}
	else {
		ancr= fLastAnchorPoint;
		prev= fLastPreviousPoint;
		next= fLastNextPoint;
		this->TrackFeedback( fTrackPhase, ancr, prev, next, mouseDidMove, turnItOn);
		}
}  






#if 0

// usage: ...


somewhere()
{
	// macapp process:
	//finalCommand = gApplication->TrackMouse(fInitialPt.Copy(), gStdHysteresis, this);
	// get handler of finalcommand & call its processor
	
	//vibrant process:
	aDPanel->SetTracker( new DTracker(blah));
	// handle mouse...
	
	// do this in DPanel::Release() call...
	if (aDPanel->IsDoneTracking()) {
		finalCommand= aDPanel->fTracker; 
		if (finalCommand) finalCommand->Process();
		}
	
}

DTracker* DApplication::TrackMouse( Nlm_PoinT theMouse, DTracker* theCommand)
{
	DTracker* currentTracker = NULL;	 

	currentTracker = theCommand->HandleTrackBegin( theMouse); // == Click

	while (currentTracker && !currentTracker->IsDoneTracking())
		currentTracker = currentTracker->HandleTrackContinue(); // == Drag & Hold

	if (currentTracker)
		currentTracker = currentTracker->HandleTrackEnd();  // == Release

	return currentTracker;
}  


void DPanel::SetTracker( DTracker* mouseTracker)
{
	fTracker= mouseTracker;
}

void DPanel::Click(Nlm_PoinT mouse)
{
	// this is always called before Drag/Hold/Release
	// Release is only called if Drag/Hold are called (only if mouse is down for a while)
	fMouseStillDown= false;
	if (fTracker) fTracker= fTracker->Start( mouse);
}

void DPanel::Drag(Nlm_PoinT mouse)
{
	fMouseStillDown= true;
	if (fTracker) fTracker= fTracker->Continue( mouse, true);
}

void DPanel::Hold(Nlm_PoinT mouse)
{
	fMouseStillDown= true;
	if (fTracker) fTracker= fTracker->Continue( mouse, false);
}

void DPanel::Release(Nlm_PoinT mouse)
{
	fMouseStillDown= false;
	if (fTracker) fTracker= fTracker->Finish( mouse);
}

#endif


