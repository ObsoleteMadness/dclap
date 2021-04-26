// DChildDlogDoc.h
// d.g.gilbert

#ifndef _DCHILDDLOGDOC_
#define _DCHILDDLOGDOC_

#include "Dvibrant.h"
#include "DWindow.h"

class DChildDlogView;
class DFile;
class DDialogText;
class DList;

class	DChildDlogDoc : public DWindow
{
protected:
	DChildDlogView* fDlog;
	Nlm_FonT	fFont;
	DList* 	fControls;
public:

	DChildDlogDoc( long id = 0, Boolean freeOnClose = true, 
								Nlm_FonT itsFont = Nlm_programFont);
 	virtual ~DChildDlogDoc();
	
	virtual void OpenText( char* doctext, ulong doclength = 0);
	virtual void Open( DFile* aFile);
	virtual void Open( char* filename);
	virtual void Open();
	virtual void Close();
	virtual void CloseAndFree();
	virtual void ResizeWin();
	virtual Nlm_Boolean PoseModally();
	virtual Boolean IsMyAction(DTaskMaster* action); 
	virtual void AddSubordinate(DTaskMaster* subordinate);
	virtual DList* GetControls();
};

#endif
