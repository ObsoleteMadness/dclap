// DNetHTMLHandler.h
// d.g.gilbert


#ifndef _DNetHTMLHANDLER_
#define _DNetHTMLHANDLER_

#include "DHTMLHandler.h"

class DGopherList;

class DNetHTMLHandler : public DHTMLHandler
{
public:
	DGopherList	* fGolist;

	DNetHTMLHandler( DRichView* itsDoc, DFile* savefile, DGopherList* itsList);
	virtual DRichprocess* NewProcessor( DFile* itsFile, Nlm_MonitorPtr progress);
};

#endif

