// DPICTprocess.h
// d.g.gilbert



#ifndef _DPICTPROCESS_
#define _DPICTPROCESS_

#include "DRichProcess.h"

class DPICTprocess : public DRichprocess
{
public:
	DPICTprocess( DFile* itsFile, DFile* itsTempData, 
						 		DRichView* itsDoc, Nlm_MonitorPtr progress);
	virtual ~DPICTprocess();
	virtual void Close();
	virtual void Read();
	virtual void handleTextClass();
	virtual void handleControlClass();
	virtual Boolean InstallPicStyle( DRichStyle* theStyle);
 	virtual Boolean PutPicture();
	
protected:
	char* 	fPict;
	ulong 	fPictMax, fPictSize;
	Boolean	fInPic, fNotDone;
	DFile*	fTempData;
	
	virtual Boolean  StuffInStyle( DRichStyle* theStyle);
	virtual Boolean  StuffInDocument();
	virtual void  ReadDataFromFile();
	virtual void  PutPicChar(short c);
};


#endif
