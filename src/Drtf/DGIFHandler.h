// DGIFHandler.h
// d.g.gilbert



#ifndef _DGIFHANDLER_
#define _DGIFHANDLER_

#include "DPICTHandler.h"


class DGIFHandler : public DPICTHandler
{
public:
	enum  {
		kGIFformat = kPICTformat + 1
		};
		
	DGIFHandler(DRichView* itsDoc, DFile* savefile);
	virtual char* IsRich(char* buf, ulong buflen = 0);
	virtual DRichprocess* NewProcessor( DFile* itsFile, Nlm_MonitorPtr progress);
	virtual short Format() { return kGIFformat; }
	static void GIFFileSigs( char*& suffix, char*& type, char*& sire) 
		{ suffix= ".gif"; type= "GIFf"; sire= "JVWR"; }
};


#endif

