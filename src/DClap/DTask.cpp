// DTask.cp
// d.g.gilbert

#include "Dvibrant.h"
#include "DTask.h"
#include "DTaskMaster.h"

#include "DCommand.h"
#include "DView.h"
#include "DViewCentral.h"
#include "DApplication.h"



//class DTask : public DObject
		
void DTask::Process() 
{ 
	if (fSource) fSource->ProcessTask(this); 
}
	



// class DCommand : public DTask

Global DCommand* gLastCommand;


DCommand::DCommand() :
		DTask( 0, kCommander, NULL, 0),
		fTitle( NULL),
		fCanUndo( false), 
		fCausesChange( false), 
		fCommandDone(0)
		{}

DCommand::DCommand( long command, DTaskMaster* itsSource, char* title ,
						Nlm_Boolean canUndo, Nlm_Boolean causesChange) :
		DTask( command, kCommander, itsSource, 0),
		fTitle( title),
		fCanUndo( canUndo), 
		fCausesChange( causesChange), 
		fCommandDone(0)
		{}


void DCommand::ICommand( long command, DTaskMaster* itsSource, char* title,
						Nlm_Boolean canUndo, Nlm_Boolean causesChange) 
{
	fNumber= command; 
	fSource= itsSource;
	fTitle = title;
	fCanUndo = canUndo;
	fCausesChange = causesChange;
}
		
DCommand::~DCommand() 
{ 
	Commit(); 
	if (this == gLastCommand) gLastCommand= NULL;
}

void DCommand::Commit() 
{
	if (this == gLastCommand) {
		gViewCentral->DisableView(DApplication::kUndo); 
		fCommandDone= true; //??
		fCanUndo= false;
		// can we delete/NULL gLastCommand ??  
		}
}

void DCommand::SetupMenu(char* undo) 
{ 
	DView* 	undomenu;
	if (fCanUndo) {
		//gViewCentral->EnableView(DApplication::kUndo);   
		undomenu= gViewCentral->GetView(DApplication::kUndo);
		if (undomenu) {
			if (fTitle) {
				char title[80];
				sprintf( title, "%s %s", undo, fTitle);
				undomenu->SetTitle( title);
				}
			else 
				undomenu->SetTitle( undo);
			undomenu->Enable();
			}
		}
	else {
		//gViewCentral->DisableView(DApplication::kUndo);   
		undomenu= gViewCentral->GetView(DApplication::kUndo);
		if (undomenu) {
			undomenu->SetTitle("Undo");
			undomenu->Disable();
			}
		}

}

void DCommand::UndoRedo() 
{ 
	if (fCanUndo) {
		if (fCommandDone) Undo(); 
		else Redo();
		}
}

void DCommand::DoIt() 
{ 
	fCommandDone= true; 
	SetupMenu("Undo");
}

void DCommand::Undo() 
{ 
	fCommandDone= false; 
	SetupMenu("Redo");
}

void DCommand::Redo() 
{ 
	fCommandDone= true; 
	SetupMenu("Undo");
}









