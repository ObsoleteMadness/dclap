// DTaskMaster.h
// d.g.gilbert

#ifndef _DTASKMASTER_
#define _DTASKMASTER_

#include "DObject.h"
#include "DId.h"

class DTask;
class DList;

class DTaskMaster : public DObject
{	
public:
	long					fId;				 
	DTaskMaster*	fSuperior;	 
	DList*				fSubordinates; 
	
	DTaskMaster(long id, DTaskMaster* superior = NULL, DList* subordinates = NULL);
	virtual ~DTaskMaster();

	long Id(void) { return fId; }
	
	virtual void AddSubordinate(DTaskMaster* subordinate);

	virtual Boolean IsSuperior(DTaskMaster* item);
	virtual Boolean IsSubordinate(DTaskMaster* item);

	virtual	DTaskMaster* FindSuperior(long subid);
	virtual	DTaskMaster* FindSubordinate(long subid);

	virtual Boolean IsMyAction(DTaskMaster* action);
	
	virtual DTask* newTask(long tasknum, short kind, long extra = 0);
	virtual	void PostTask(DTask* theTask);
	virtual Boolean IsMyTask(DTask* theTask);
	virtual void ProcessTask(DTask* theTask);
	virtual Boolean DoMenuTask(long tasknum, DTask* theTask);
};


#endif
