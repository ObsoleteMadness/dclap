// DCommand.h
// d.g.gilbert

#ifndef _DCOMMAND_
#define _DCOMMAND_

#include "DTask.h"


class DCommand : public DTask
{
public:
  enum {
  	kCantUndo= 0, kCanUndo = 1,
  	kDoesntChange= 0, kCausesChange = 1
  	};
	Boolean fCanUndo, fCausesChange, fCommandDone;
	char * fTitle;
	
	DCommand();
	DCommand( long command, DTaskMaster* itsSource, char* title = NULL,
						Nlm_Boolean canUndo = 0, Nlm_Boolean causesChange = 0);
	void ICommand( long command, DTaskMaster* itsSource, char* title = NULL,
						Nlm_Boolean canUndo = 0, Nlm_Boolean causesChange = 0);
	virtual ~DCommand();
	virtual void DoIt();
	virtual void Undo();
	virtual void Redo();
	virtual void UndoRedo();
	virtual void Commit();

protected:
	void SetupMenu(char* undo);
};



extern DCommand* gLastCommand;

#endif
