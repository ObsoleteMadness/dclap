// DHTMLHandler.h
// d.g.gilbert


#ifndef _DHTMLHANDLER_
#define _DHTMLHANDLER_

#include "DRichHandler.h"


class DHTMLHandler : public DRichHandler
{
public:
	enum {
		kHTMLformat = kTextformat + 2
		};
		
	DHTMLHandler( DRichView* itsDoc, DFile* savefile);
	virtual ~DHTMLHandler();
	virtual char* IsRich(char* buf, ulong buflen = 0);
	virtual DRichprocess* NewProcessor( DFile* itsFile, Nlm_MonitorPtr progress);
	virtual Boolean ProcessData( char* cbeg, char* cend, Boolean endOfData, 
					ulong& dataRemaining);
	virtual short Format() { return kHTMLformat; }
	static void HTMLFileSigs( char*& suffix, char*& type, char*& sire) 
		{ suffix= ".html"; type= "TEXT"; sire= "ttxt"; }
};

#endif
