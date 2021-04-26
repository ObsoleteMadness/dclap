// DTask.h
// d.g.gilbert

#ifndef _DTASK_
#define _DTASK_

#include "Dvibrant.h"
#include "DObject.h"

class DTaskMaster;

class DTask : public DObject
{	
public:
	enum tasktypes { kNoTask=0, kMenu, kMouseDown, kMouseUp, kKeyDown, kKeyUp, 
			kUpdate, kActivate, kSuspend, kResume, kIdle, kDisk, kTracker, kCommander,
			kOtherTask};	// not sure of the use of this enum
	
	long		fNumber;
	short		fKind;
	long		fExtra;
	DTaskMaster*	fSource;
	Boolean	fRepeat;
	
	DTask(long num, short itsKind, DTaskMaster* itsSource, long extra = 0) :
		fNumber(num),
		fKind(itsKind),
		fSource(itsSource),
		fExtra(extra),
		fRepeat(false)
			{}
		
	virtual void DoIt() {}
	virtual void Undo() {}
	virtual void Redo() { this->DoIt(); }
	virtual void Process();
	virtual void Commit() {}
	void SetRepeat(Boolean turnon= true) { fRepeat= turnon; }
};


#endif
