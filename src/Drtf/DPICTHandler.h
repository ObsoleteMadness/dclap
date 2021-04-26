// DPICTHandler.h
// d.g.gilbert



#ifndef _DPICTHANDLER_
#define _DPICTHANDLER_

#include "DRichHandler.h"


class DPICTHandler : public DRichHandler
{
public:
	enum  {
		kPICTformat = kTextformat + 3,
		kAfterHeader = kAtEnd + 1
		};
		
	DPICTHandler(DRichView* itsDoc, DFile* savefile);
	virtual char* IsRich(char* buf, ulong buflen = 0);
	virtual DRichprocess* NewProcessor( DFile* itsFile, Nlm_MonitorPtr progress);
	virtual Boolean ProcessData( char* cbeg, char* cend, Boolean endOfData, 
					ulong& dataRemaining);
	virtual void InstallInStyle( DRichView* theDoc, DRichStyle* theStyle);
	virtual short Format() { return kPICTformat; }
	static void PICTFileSigs( char*& suffix, char*& type, char*& sire) 
		{ suffix= ".pict"; type= "PICT"; sire= "ttxt"; }
};


#endif
