// DRichHandler.h
// d.g.gilbert



#ifndef _DRichHANDLER_
#define _DRichHANDLER_

#include "DObject.h"

class DFile;
class DRichView;
class DRichprocess;
class DRichStyle;


class DRichsetup  
{
public:
	DRichsetup();
	~DRichsetup();
};


class DRichHandler : public DObject
{
public:
	enum docFormats {
		kUnknownformat,
		kTextformat
		};

	enum stages {
		kAtStart,
		kInBody,
		kAtEnd
		};
		
	short 				fProcessStage, fTasknum;  
	DRichView 		* fDoc;
	DFile					* fDocFile;
	DRichprocess	* fProcessor;
	char					* fFormatName;
	short					fMinDataToProcess;
	long 					fLastScanto;
	
	DRichHandler(DRichView* itsDoc, DFile* savefile);
	virtual ~DRichHandler();
	virtual Boolean ProcessData( char* cbeg, char* cend, Boolean endOfData, 
					ulong& dataRemaining);
	virtual Boolean ProcessFile( char* filename);
	virtual Boolean ProcessFile( DFile* theFile); 
	virtual DRichprocess* NewProcessor( DFile* itsFile, Nlm_MonitorPtr progress);
	virtual void InstallInStyle( DRichView* theDoc, DRichStyle* theStyle);

	virtual char* IsRich( char* buf, ulong buflen = 0);
	virtual long  IsRichFile( DFile* theFile);
	virtual long  IsRichFile( char* filename);
	
	virtual short Format() { return kTextformat; }
	static void RichFileSigs( char*& suffix, char*& type, char*& sire) 
		{ suffix= ".text"; type= "TEXT"; sire= "ttxt"; }
	
protected:
	char  	fcsave;  
	ulong 	fclen;
	
	virtual Boolean ProcessData2( char* cp, char* cbeg, char* cend, Boolean endOfData, 
					ulong& dataRemaining);

};



#endif
