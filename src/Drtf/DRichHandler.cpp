// DPICTHandler.cpp
// d.g.gilbert


#include "Dvibrant.h"
#include "DFile.h"
#include "DRichHandler.h"
#include "DRichViewNu.h"
#include "DRichProcess.h"




//class  DRichHandler

enum {
	kNoData = -1
	};
			

DRichHandler::DRichHandler(DRichView* itsDoc, DFile* savefile) :
	fDoc(itsDoc), fDocFile(savefile),
	fProcessor(NULL), fMinDataToProcess(1), fLastScanto( 0),
	fProcessStage(kAtStart), fTasknum(0)
{
	fFormatName = "Text";
}

DRichHandler::~DRichHandler()
{
	if (fProcessor) delete fProcessor;
}

	
char* DRichHandler::IsRich(char* buf, ulong buflen)
{
#if 0
	char *cp, *cend = buf + MIN(buflen-1, 500);
	for (cp= buf; cp<cend; cp++) {
		//if (*cp < ' ' && *cp != '\n' && *cp != '\r' && *cp != '\t') 
		if (! (*cp >= ' ' || isspace(*cp)) )  
			return NULL;
		}
#endif
	return buf; // ?? assume plain text? or test for garbage (nulls, etc)
}

long DRichHandler::IsRichFile( DFile* theFile)
{
	long	startat = kNoData; // no data
	
	theFile->Open();
	if (theFile->IsOpen()) {
		char buf[1024];
		ulong count= 1024; 

		theFile->ReadData(buf, count);
		char* cp= IsRich( buf, count);
		if (cp) startat= cp - buf;
		theFile->Close();
		}
	return startat;
}

long DRichHandler::IsRichFile( char* filename)
{
	DFile afile(filename, "r");
	return IsRichFile( &afile);
}

Boolean DRichHandler::ProcessData( char* cbeg, char* cend, Boolean endOfData, 
										ulong& dataRemaining) 
{
	char		* cp;
	//char csave = 0;  
	//ulong 	clen;

	fcsave= 0;	
	fclen= cend - cbeg; // ?? +1
	cp= cend;
	if (!endOfData) {
		if ((fclen==0) || ( fProcessStage == kAtStart && fclen<fMinDataToProcess)) 
			return false;
		// scan for last newline and stop there !!
		while (cp - cbeg > fLastScanto && !(*cp == '\n' || *cp == '\r')) cp--;
		}
		
	if (cp - cbeg <= fLastScanto) {
		fLastScanto= cend - cbeg;  
		if (!endOfData) return false; // !??! don't process line w/o end
		else cp= cend; 
		}
	fclen= cp - cbeg; // ?? +1
			
	if (fclen && fDocFile) fDocFile->WriteData( cbeg, fclen);

	if (!fProcessor) { // this may be bad 
		cp= IsRich(cbeg, fclen);
		if (!cp) return false;
		cbeg= cp;
		fclen= cend - cbeg; //?? +1
		}
		
	return this->ProcessData2( cp, cbeg, cend, endOfData, dataRemaining);
}

Boolean DRichHandler::ProcessData2( char* cp, char* cbeg, char* cend, Boolean endOfData, 
										ulong& dataRemaining) 
{

	if (!fProcessor) {  
		fDoc->SetAutoAdjust(false);
		//fProcessor= this->NewProcessor(NULL,NULL); 
		fProcessor= this->NewProcessor((DFile*)NULL,(Nlm_MonitorPtr)NULL); 
		//FailNIL( fProcessor); //??
		fProcessStage = kInBody;
		}
		
  if (fclen) {
	  fProcessor->SetBuffer(cbeg, fclen, endOfData);
		while (fProcessor->GetToken() != DRichprocess::tokEOF) 
			fProcessor->RouteToken();
		}
		
	if (endOfData) { 
		fProcessor->Close(); 
		if (fProcessor->GetTitle()) 
			fDoc->GetWindow()->SetTitle( (char*)fProcessor->GetTitle());
		delete fProcessor; 
		fProcessor= NULL; 
		} 

	fDoc->AdjustScroll();
	if (endOfData) fDoc->SetAutoAdjust(true);   
			
	if (cp > cbeg) { // (fcsave) {
		if (fcsave) *cp= fcsave;   
		if (cp > cend) fclen= 0; 
		else fclen= cend - cp; // + 1;
		Nlm_MemMove( cbeg, cp, fclen);
		}
	else fclen= 0;
	//cbeg[fclen]= 0;
	
	dataRemaining= fclen;	
	return true;
}


DRichprocess* DRichHandler::NewProcessor( DFile* itsFile, Nlm_MonitorPtr progress)
{
		fProcessor = new DRichprocess( itsFile, fDoc, progress);
		return fProcessor;
}

void DRichHandler::InstallInStyle( DRichView* theDoc, DRichStyle* theStyle)
{
	if (fProcessor) {
		// dummy proc for subclassing
		}  
}


Boolean DRichHandler::ProcessFile( char* filename) 
{
	DFile afile(filename, "r");
	return ProcessFile( &afile);
}


Boolean DRichHandler::ProcessFile( DFile* theFile) 
{
	Boolean result= false;
	long startat= kNoData;
	if (theFile) startat= IsRichFile(theFile);
	if (fDoc && startat > kNoData) {
		fDoc->SetAutoAdjust(false);  
		
		ulong nbytes; 
		theFile->GetDataLength(nbytes);
		if (nbytes) {
			Nlm_MonitorPtr progress= NULL;
			char prompt[128];
			sprintf( prompt, "Converting %s format...", fFormatName);
			progress= Nlm_MonitorIntNew( prompt, 0, nbytes);
			theFile->Open();
			theFile->Seek(startat);
		
			fProcessor= this->NewProcessor(theFile, progress);
			if ( NULL != fProcessor) {
				fProcessor->Read();
				if (fProcessor->GetTitle()) 
					fDoc->GetWindow()->SetTitle( (char*)fProcessor->GetTitle());
				delete fProcessor; 
				fProcessor= NULL;
				result= true;
				}
			
			theFile->Close();
			Nlm_MonitorFree(progress);
			}		
		fProcessStage= kAtEnd;
		
		fDoc->Select();
		fDoc->Invalidate();
		fDoc->AdjustScroll();
		fDoc->SetAutoAdjust(true);  
		}
	return result;
}






