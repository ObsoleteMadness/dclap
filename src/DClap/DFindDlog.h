// DFindDlog.h

#ifndef __DFINDDLOG__
#define __DFINDDLOG__


#include "DWindow.h"

class DTaskMaster;
class DEditText;

class DFindDialog : public DWindow {
public:
	enum { 
		kMaxTarg = 5, kMaxReplace = 5,
		findId = 2390,replaceId,replaceFindId,replaceAllId,
		cBackwards, cCaseSense, cFullWord
		};
		
protected:
	static Boolean fBackwards, fCaseSense, fFullWord;
	static char* fTarget[kMaxTarg];
	static char* fReplace[kMaxReplace];
  DEditText * fFindText, * fReplaceText;

public:		
	DFindDialog();
	virtual ~DFindDialog();

	virtual void DoFind() {}
	virtual void DoReplace() {}
	virtual void DoReplaceAll() {}

	virtual void FindAgain();
	virtual Boolean IsMyAction( DTaskMaster* action); 	
	virtual void Open();
	virtual void BuildDlog();
	virtual const char* GetFind();
	virtual const char* GetReplace();

	static void InitFindDialog();
	static Boolean Backwards() { return fBackwards; }
	static Boolean CaseSensitive() { return fCaseSense; }
	static Boolean FullWord() { return fFullWord; }
	static void SetBackwards(Boolean turnon) { fBackwards= turnon; }	
	static void SetCaseSense(Boolean turnon) { fCaseSense= turnon; }	
	static void SetFullWord(Boolean turnon) { fFullWord= turnon; }	
};

#endif
