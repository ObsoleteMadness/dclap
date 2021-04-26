// DPICTHandler.cpp
// d.g.gilbert


#include "Dvibrant.h"
#include "DFile.h"
#include "DPICTHandler.h"
#include "DPICTprocess.h"
#include "DRichViewNu.h" //  DRichViewP
#include "DDrawPICT.h"   // DDrawPICTst
#include "DRichMoreStyle.h"




//class  DPICTHandler

DPICTHandler::DPICTHandler(DRichView* itsDoc, DFile* savefile) :
	DRichHandler( itsDoc, savefile)
{
	fFormatName = "PICT";
	fMinDataToProcess= 612;
	if (fDocFile) fDocFile->Open("wb"); // make sure savefile is binary
}

char* DPICTHandler::IsRich(char* buf, ulong buflen)
{
	DDrawPict* aDrawPict = new DDrawPict();
	char* picat = (char*) aDrawPict->IsPICT(buf, buflen, (Nlm_Boolean)false/*dontTestForHead*/);
	delete aDrawPict;
	return picat;
}

DRichprocess* DPICTHandler::NewProcessor( DFile* itsFile, Nlm_MonitorPtr progress)
{
		fProcessor = new DPICTprocess( itsFile, fDocFile, fDoc, progress);
		return fProcessor;
}

void DPICTHandler::InstallInStyle( DRichView* theDoc, DRichStyle* theStyle)
{
	if (fProcessor) {
		((DPICTprocess*)fProcessor)->InstallPicStyle(theStyle);  
		delete fProcessor; 
		fProcessor= NULL; 
			// must search thru all paragraphs in theDoc and update item heights for
		theDoc->UpdateStyleHeight(theStyle);
		theDoc->SetAutoAdjust(true); 
		//theDoc->AdjustScroll();
		}  
}


Boolean DPICTHandler::ProcessData( char* cbeg, char* cend, Boolean endOfData, 
					ulong& dataRemaining) 
{

	if (Format() == kPICTformat && fProcessStage == kAtStart && fDocFile) {
			// write empty pict header !?
		char zerobuf[512];
		ulong count= 512;
		Nlm_MemFill( zerobuf, 0, count);
		fDocFile->WriteData( zerobuf, count);
		fProcessStage= kAfterHeader;
		}

#if 1			
	if (!fProcessor) { 
		fDoc->SetAutoAdjust(false);
		fProcessor= this->NewProcessor((DFile*)NULL,(Nlm_MonitorPtr)NULL); 
		fProcessStage = kInBody;
		}

	if (dataRemaining && fDocFile) {
		ulong count= dataRemaining;  
		fDocFile->WriteData( cbeg, count);
		dataRemaining= 0;
		}
		
	if (endOfData && fProcessor) { 
		if (fDocFile) fDocFile->Close();
	
		switch (fTasknum) {
		
			case 1: // inline image
				// fProcessor->InstallPicStyle(theStyle); is called by others
				break;
				
			default: 
				fProcessor->Close(); 
				if (fProcessor->GetTitle()) fDoc->SetTitle( (char*)fProcessor->GetTitle());
				delete fProcessor; 
				fProcessor= NULL; 
				fDoc->AdjustScroll();
				fDoc->SetAutoAdjust(true);   
				break;
			}
			
		return true;
		} 
	return 3; //<< flag update date but not view; // true;
	
#else	

			// Dgg_DrawPict is not currently written to draw partial PICT data
			// So for now, this handler should always return false until endOfData is true
	if (!endOfData) return false;
 
	return DRichHandler::ProcessData( cbeg, cend, endOfData, dataRemaining);		
#endif
}








//class  DPICTprocess


DPICTprocess::DPICTprocess( DFile* itsFile, DFile* itsTempData, 
										DRichView* itsDoc, Nlm_MonitorPtr progress) :
		DRichprocess( itsFile, itsDoc, progress),
		fPictMax(0), fPictSize(0), fPict(NULL), 
		fTempData( itsTempData),
		fNotDone(true), fInPic(true) 
{ 	
}
	

DPICTprocess::~DPICTprocess()
{
	//////// nooooo //// Nlm_MemFree(fPict); // data is passed to displayer
}

void DPICTprocess::Close()
{
	(void) PutPicture();
}

void DPICTprocess::Read()
{
	(void) PutPicture();
}



void DPICTprocess::PutPicChar(short c)
{
	if (1 + fPictSize >= fPictMax) {
		fPictMax = fPictSize + 1025;
		if (!fPict) fPict= (char*) Nlm_MemNew(fPictMax);
		else fPict= (char*) Nlm_MemMore( fPict, fPictMax);
		}
	if (fPict) fPict[fPictSize++]= (char) c;
}


void DPICTprocess::handleTextClass()
{
	if (fInPic) {
		PutPicChar( fMajor);
		}
	else {
		if (IsNewStyle()) StoreStyle(fOldStyleRec.style, FALSE);  
		PutLitChar(fMinor); 
		fLastChar= fMajor; //??
		}
}

void DPICTprocess::handleControlClass()
{
	if (fInPic) 
		PutPicChar( fMinor);
	else  
		DRichprocess::handleControlClass();
}


Boolean DPICTprocess::StuffInStyle( DRichStyle* theStyle)
{			
	if (fPictSize) {
		fPict[fPictSize]= 0;
		DPictStyle* newPict= 
				new DPictStyle( DPictStyle::kPictMac, fPict, fPictSize, true);  
		Nlm_RecT picrect;
		DDrawPict* aDrawPict = new DDrawPict();
		aDrawPict->GetFrame( &picrect, fPict, fPictSize, DDrawPict::kPictMac);
		delete aDrawPict;
		theStyle->pixwidth= picrect.right - picrect.left;
		theStyle->pixheight= picrect.bottom - picrect.top;

		DPictStyle* oldPict= (DPictStyle*) theStyle->fObject;
		if (oldPict) {
			Nlm_RecT loc;
			// netpict info we need to keep ?!
			// this is an html pict!?, reset loc for netlinks 
			Boolean resetLoc= (oldPict->fLinks[0].fKind == DPictStyle::kPictNetPic);
			short i, n = oldPict->fNumLinks;
			for (i= 0; i<n; i++) {
				if (resetLoc) loc = picrect;
				else loc= oldPict->fLinks[i].fLoc;
				if (oldPict->fLinks[i].fKind == DPictStyle::kPictNetLink
				 || oldPict->fLinks[i].fKind == DPictStyle::kPictMapLink)
					newPict->AddLink( oldPict->fLinks[i].fKind, oldPict->fLinks[i].fLinkid, loc);
				}
			oldPict->suicide(); // ?? delete
			}
		theStyle->ispict= TRUE;
		theStyle->fObject= newPict;
		
		fPictSize= 0;
		fPict= NULL; //??
		return true;
		}
	else
		return false;
}

void DPICTprocess::ReadDataFromFile()
{	
	if (!fDataFile) fDataFile= fTempData;
	if (fNotDone && fDataFile) {
		ulong filelen, fileindex;
		fDataFile->Close(); // !?!? are we done writing to fTempData ??
		fDataFile->Open("rb"); 
		fDataFile->GetDataLength( filelen);
		
		fPictMax= filelen+1;
		fPict= (char*) Nlm_MemNew(fPictMax);		

			//// need to skip MacDraw 512 byte header, if it exists 
		fPictSize= MIN(filelen, 1024);
		fDataFile->ReadData( fPict, fPictSize);
		DDrawPict* aDrawPict = new DDrawPict();
		char* picat = (char*) aDrawPict->IsPICT( fPict, fPictSize, false/*dontTestForHead*/);
		delete aDrawPict;
		fileindex= picat - fPict;
		fDataFile->SetDataMark( fileindex);
		fPictSize= filelen - fileindex;
		fDataFile->ReadData( fPict, fPictSize);
		}
}			
		

Boolean DPICTprocess::StuffInDocument()
{	
	if (fPictSize) {
		//PutLitStr("\n");  
		//NewParagraph();

		(void) StuffInStyle( &fStyleRec.style);

		PutLitChar(' '); // (chPictTag); /* put a space for some real text ? */
		StoreStyle(fStyleRec.style, TRUE);

		fPictSize= 0;
		fStyleRec.style.fObject= NULL;
		fStyleRec.style.ispict= FALSE;
		fStyleRec.style.pixwidth= 0; 
		fStyleRec.style.pixheight= 0; 
		fOldStyleRec= fStyleRec;
		fOldStyleRec.fontname= NULL; /* ?? force new style ? */
		fInPic= FALSE;

		StoreStyle(fStyleRec.style, TRUE);
		PutLitChar('\n'); /* put a space for some real text ? */
		NewParagraph();

		//PutLitStr("\n");  
		//NewParagraph();
		return true;
		}
	else
		return false;
}

			
Boolean DPICTprocess::InstallPicStyle( DRichStyle* theStyle)
{	
	ReadDataFromFile();
	if (fPictSize && theStyle)  
		return StuffInStyle( theStyle);
	else
		return false;
}

Boolean DPICTprocess::PutPicture()
{	
	ReadDataFromFile();
	if (fPictSize)  
		return StuffInDocument();
	else
		return false;
}



