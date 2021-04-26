// DRTFHandler.h
// d.g.gilbert



#ifndef _DRTFHANDLER_
#define _DRTFHANDLER_

#include "DRichHandler.h"


class DRTFHandler : public DRichHandler
{
public:
	enum  {
		kRTFformat = kTextformat + 1
		};
	
	DRTFHandler(DRichView* itsDoc, DFile* savefile);
	virtual char* IsRich(char* buf, ulong buflen = 0);
	virtual DRichprocess* NewProcessor( DFile* itsFile, Nlm_MonitorPtr progress);
	virtual Boolean ProcessData( char* cbeg, char* cend, Boolean endOfData, 
					ulong& dataRemaining);
	virtual short Format() { return kRTFformat; }
	static void RTFFileSigs( char*& suffix, char*& type, char*& sire) 
		{ suffix= ".rtf"; type= "TEXT"; sire= "MSWD"; }
};



#endif
