// DTaskCentral.cp
// d.g.gilbert


#include "DTaskCentral.h"
#include "DTask.h"
#include "DTaskMaster.h"
#include "DList.h"
#include "DCommand.h"

DTaskCentral *gTaskCentral = NULL;



//class DTaskCentral : public DObject

DTaskCentral::DTaskCentral() 
{
	fTaskQueue= new DList();
}

DTaskCentral::~DTaskCentral() 
{
	fTaskQueue->FreeAllObjects();
	delete fTaskQueue;
}

void DTaskCentral::AddTask(DTask* theTask) 
{
	fTaskQueue->Queue( theTask);
}
		
void DTaskCentral::NextTask() 
{
	DTask* next= (DTask*) fTaskQueue->Dequeue();  
	if (next) {
		if (next->fSource && next->fSource->IsMyTask(next)) {
			if (next->fRepeat) AddTask(next);
			else next->suicide();
			}
		else {
			// if next->fSource doesn't handle (or its handler chain)
			// then pass task on to a co-handler chain???		
			next->Process(); // ? added mar'95
			next->suicide();
			}
		}
}


void DTaskCentral::FinishTasksByOwner(DTaskMaster* taskOwner) 
{
	if (taskOwner) {
		DTask* taskat;
		long i, n= fTaskQueue->GetSize();
			// first handle owner tasks in order
		for (i=0; i<n; i++) {
			taskat= (DTask*) fTaskQueue->At(i);  
			if (taskOwner == taskat->fSource) {
				taskat->fSource->IsMyTask(taskat);
				taskat->suicide();
				}
			}
			// now delete owner tasks, in back-order
		for (i=n-1; i>=0; i--) {
			taskat= (DTask*) fTaskQueue->At(i);  
			if (taskOwner == taskat->fSource)  
				fTaskQueue->AtDelete(i);
			}
			
		if (gLastCommand && gLastCommand->fSource == taskOwner) {
			delete gLastCommand; 
			gLastCommand= NULL; 
			}  
		}
}


DTask* DTaskCentral::FindTask(DTask* thisTask)
{
	return FindTask( thisTask->fNumber, thisTask->fKind, thisTask->fSource);
} 

DTask* DTaskCentral::FindTask(long num, short itsKind, DTaskMaster* taskOwner) 
{
	DTask* taskat;
	long i, n= fTaskQueue->GetSize();
		// first handle owner tasks in order
	for (i=0; i<n; i++) {
		taskat= (DTask*) fTaskQueue->At(i);  
		if (taskOwner == taskat->fSource
			&& num == taskat->fNumber 
			&& itsKind == taskat->fKind)
			{
				return taskat;
			}
		}
	return NULL;
}

