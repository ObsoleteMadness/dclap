// DTaskCentral.h
// d.g.gilbert

#ifndef _DTASKCENTRAL_
#define _DTASKCENTRAL_

#include "DTask.h"

class DList;
class DTaskMaster;

class DTaskCentral : public DObject
{	
	DList*		fTaskQueue;
public:
	DTaskCentral();
	virtual ~DTaskCentral();
	virtual void AddTask(DTask* theTask);
	virtual void NextTask();
	virtual void FinishTasksByOwner(DTaskMaster* taskOwner);
	virtual DTask* FindTask(long num, short itsKind, DTaskMaster* taskOwner); 
	virtual DTask* FindTask(DTask* thisTask); 
};


extern DTaskCentral* gTaskCentral;

#endif

