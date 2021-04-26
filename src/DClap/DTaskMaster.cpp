// DTaskMaster.cp
// d.g.gilbert


#include "Dvibrant.h"
#include "DList.h"
#include "DTask.h"
#include "DTaskMaster.h"
#include "DTaskCentral.h"
#include "DCommand.h"



//class DTaskMaster : public DObject

DTaskMaster::DTaskMaster(long id, DTaskMaster* superior, DList* subordinates) :
	fId(id),
	fSuperior(superior),				 
	fSubordinates(subordinates) 
{
	if (superior) superior->AddSubordinate(this); 
}

DTaskMaster::~DTaskMaster() 
{
	if (fSubordinates) fSubordinates->suicide(); //delete fSubordinates;
#if 1
	gTaskCentral->FinishTasksByOwner(this); 
#else
	if (gLastCommand && gLastCommand->fSource == this) {
		delete gLastCommand; 
		gLastCommand= NULL;
		}
#endif
	fSubordinates= NULL; //?
}
	
	
void DTaskMaster::AddSubordinate(DTaskMaster* subordinate)
{
	if (!fSubordinates) fSubordinates= new DList();
	fSubordinates->InsertLast(subordinate);
}

Boolean DTaskMaster::IsSuperior(DTaskMaster* item) 
{
	// recursion ALERT
	if (this == item) 
		return true;  // do we want case of starter == superior??
	if (fSuperior) {
		if (fSuperior == item)
			return true;
		else if (fSuperior->IsSuperior(item))
			return true;
		}
	return false;
}

DTaskMaster* DTaskMaster::FindSuperior(long superid) 
{
	// recursion ALERT
	if (fId == superid) 
		return this;  // do we want case of starter == superior??
	if (fSuperior) {
		DTaskMaster* super= fSuperior;
		if (super->fId == superid)
			return super;
		else if ((super= super->FindSuperior(superid)) != NULL)
			return super;
		}
	return NULL;
}



Boolean DTaskMaster::IsSubordinate(DTaskMaster* item) 
{
	// recursion ALERT
	if (this == item) 
		return true;   
	if (fSubordinates) {
		long i, n= fSubordinates->GetSize();
		for (i= 0; i<n; i++) {
			DTaskMaster* subord= (DTaskMaster*)fSubordinates->At(i);
			if (subord == item) 
				return true;
			else if (subord->IsSubordinate(item))
				return true;
			}		
		}
	return false;
}


DTaskMaster* DTaskMaster::FindSubordinate(long subid) 
{
	// recursion ALERT
	if (fId == subid) 
		return this;   
	else if (fSubordinates) {
		long i, n= fSubordinates->GetSize();
		for (i= 0; i<n; i++) {
			DTaskMaster* subord= (DTaskMaster*)fSubordinates->At(i);
			if (subord->fId == subid) 
				return subord;
			else if ((subord= subord->FindSubordinate(subid)) !=NULL)
				return subord;
			}		
		}
	return NULL;
}


Boolean DTaskMaster::IsMyAction(DTaskMaster* action) 
{

#if 0
	//general scheme that subclasses can use for this method
	switch(action->fId) {
	case kMyAction1: handleaction1(); return true;
	case kMyAction2: handleaction2(); return true;
	default: return Inherited::IsMyAction(action);
	}
#endif

		// this is base class method, we must look to (a) subordinates, then (b) superior
		// to see if they know how to handle this action
#if 0 
		// !! THIS IS BAD when a subordinate passes this message to me and
		// i try to go back to subordinates !!!  ?? Don't ask subords if they
		// can handle? -- assume focused view first got message and passed
		// buck up line of superiors...
	if (fSubordinates) {
		long i, n= fSubordinates->GetSize();
		for (i= 0; i<n; i++) {
			DTaskMaster* subord= (DTaskMaster*)fSubordinates->At(i);
			if (subord->IsMyAction(action)) return true;
			}		
		}
#endif

	if (fSuperior) 
		return fSuperior->IsMyAction(action);
	return false;
}

	
DTask* DTaskMaster::newTask(long tasknum, short kind, long extra) 
{
	return new DTask(tasknum, kind, this, extra);
}

void DTaskMaster::PostTask(DTask* theTask) 
{
	if (theTask) gTaskCentral->AddTask(theTask);
}

void DTaskMaster::ProcessTask(DTask* theTask) 
{
	if (theTask) {
		if (theTask->fKind == DTask::kCommander 
		 && theTask != gLastCommand
		 && ((DCommand*)theTask)->fCausesChange ) // new 18jun95
		 {
			if (gLastCommand) delete gLastCommand; 
			gLastCommand= (DCommand*) theTask;
			gLastCommand->newOwner(); // avoid the TaskCentral Reaper  !!				
			}
		theTask->DoIt();
		}
}

Boolean DTaskMaster::IsMyTask(DTask* theTask) 
{
		// assume that subclasses will override DoTask/DoMenuTask/DoMouseTask methods
		// to handle their own tasks.  This handler needs to do some overall processing,
		// then pass on to next handler if task isn't taken care of...
	if (!theTask) 
		return true; //??
	else if (theTask->fKind == DTask::kMenu) 
		return this->DoMenuTask(theTask->fNumber, theTask);
#if 1
	else if (theTask->fKind == DTask::kCommander) {
		this->ProcessTask(theTask);
		return true;
		}
#endif
#if 0
	else if (theTask->fKind == DTask::kTracker) {
		//this->ProcessTask(theTask);
		return true;
		}
#endif
	else 
		return false;
}
		
Boolean DTaskMaster::DoMenuTask(long tasknum, DTask* theTask) 
{
	return false;
}
