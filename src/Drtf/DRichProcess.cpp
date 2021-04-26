// DRichProcess.cpp
// d.g.gilbert


#include "Dvibrant.h"
#include "DFile.h"
#include "DRichProcess.h"
#include "DRichViewNu.h"

#define rtfmapInternal
#include "rtfmaps.h"
#undef rtfmapInternal


// struct StyleRec

//static DParagraph 	gDefParaFmt;
static StyleRec 		gDefaultStyle;
static Nlm_Uint4 		gLinkcolor;

static char		*kDefaultFontname   = "Courier"; //"Times";
static char		*kDefaultFontfamily = "Modern"; //"Roman";	


StyleRec::StyleRec()
{
	style = DRichStyle();  // DRichStyle constructor handles !?
	fontname= kDefaultFontname;
	fontfamily= kDefaultFontfamily;
	fontsize= DRichprocess::kDefaultFontsize;
}

	
	
DRichprocess::DRichprocess( DFile* itsFile, DRichView* itsDoc, Nlm_MonitorPtr progress) :
	fClass(tokUnknown), fMajor(0), fMinor(0), fParam(0),
	fDataFile( itsFile), fDataBuffer( NULL), fDataSize(0), 
	fDoc(itsDoc), fProgress(progress), fParaCount(0),
	fText(NULL), fTextSize(0), fLastTextSize(0), fTextMax(0),
	fLastChar(0), fPushedChar(EOF), fOutMap(NULL),
	fStyleStackSize(0), fTitle(NULL), fNewStyle(true),
	fEndOfData(false)
{ 
	fTextMax= 1024;
	fText= (char*) MemNew(fTextMax+1);

	fStyleMax= 5;
	fStyleCount= 0;
	fStyleArray= (DRichStyle*) Nlm_MemNew( fStyleMax*sizeof(DRichStyle));
	fStyleStackSize= 0;

#if 0
	fStyleRec.style = DRichStyle();
			// don't need this jazz, constructors handle it
	//DRichStyle	aStyle(false);
	//fStyleRec.style= aStyle;
	//fStyleRec.fontname= kDefaultFontname;
	//fStyleRec.fontfamily= kDefaultFontfamily;
	//fStyleRec.fontsize= kDefaultFontsize;
#endif
	fOldStyleRec= fStyleRec;
	gDefaultStyle= fStyleRec;
	
	(void) GetNlmFont();
	
	// !?? static constructors for DParagraph should do all these !?
	// fParaFmt = DParagraph();
	fParaFmt.deftabstop = kDefTabstop;
	// !?? static constructors for DParagraph should do all these !?
	fOldParaFmt= fParaFmt;
	fDefParaFmt= fParaFmt;

		// not for text types ?? (RTF should be ok w/ this...)
	if (fDataFile) {
		ulong fileindex;
		fDataFile->GetDataMark( fileindex);
		fDataFile->Open("rb"); // !!!!! DAMN TEXT TRANSLATION (0d to 0a) WAS SCREWING US UP !!
		fDataFile->SetDataMark( fileindex);
		}
}

	
DRichprocess::~DRichprocess()
{
	Close();
	//if (fDataFile) fDataFile->Close(); // caller may do this?
	MemFree(fText);
	MemFree(fStyleArray);
	MemFree(fTitle);
	
}

void DRichprocess::SetBuffer( char* dataBuffer, ulong dataSize, Boolean endOfData)
{
	fDataSize= dataSize;
	if (dataSize) fDataBuffer= dataBuffer;
	else fDataBuffer= NULL; // prevent GetOneChar trying to read when no data !
	fEndOfData= endOfData;
}

void DRichprocess::Read()
{
	while (GetToken() != tokEOF) 
		RouteToken();
	Close();
}

void DRichprocess::Close()
{
	if (fTextSize>0) {
		PutLitCharWithStyle('\n');
		NewParagraph(); /* push last text into doc */
		}
	if (fProgress && fDataFile) {
		long fat= fDataFile->Tell();
		if (fat>0) (void) Nlm_MonitorIntValue(fProgress, fat);
		}
}


short DRichprocess::GetOneChar()
{
	short	c;

	if (fDataBuffer) {
		c= (unsigned char) *fDataBuffer; // must be unsigned !!
		if (fDataSize) {	// for counted data
			fDataSize--;
			fDataBuffer++;
			if (!fDataSize) fDataBuffer= NULL; //  so EOF next time
			}
		else if (c) fDataBuffer++; // for null-term data
		else if (!fEndOfData) c= tokBreakInData;
		else c= EOF;
		}
	else if (fDataFile) 
		c = fgetc( fDataFile->fFile);
	else {
		if (!fEndOfData) c= tokBreakInData;
		else c= EOF;
		}
	return (c);
}


void DRichprocess::Pushback(char c) // private
{
	if (c != EOF) {
		fPushedChar = c;
		}
}


void DRichprocess::RouteToken()
{
	switch( fClass ) {
		case tokUnknown:  
			handleUnknownClass(); 
			break;
		case tokText: 
			handleTextClass(); 
			break;
		case tokControl:  
			handleControlClass(); 
			break;
		default:
			//RTFPanic ("Unknown class %d: %s ", fClass, fTokenBuf);
			break;
		}	
}

short DRichprocess::GetToken()
{
	do GetToken1(); 
	while (fClass == tokDropchar);
	return (fClass);
}


void DRichprocess::GetToken1() // private
{
	short	c;
	
		/* initialize token vars */
	fClass = tokUnknown;
	fParam = tokNoParam;

		/* get first token character, which may be a pushback from previous token */
	if (fPushedChar != EOF) {
		c = fPushedChar;
		fPushedChar = EOF;
		}
	else 
		c= GetOneChar();
		 
	switch ( c ) {
	
		case EOF:
			fClass = tokEOF;
			fMajor = 0;
			return;
		case tokBreakInData:
			fClass= tokEOF;
			fMajor= tokBreakInData;
			return;
			
		case '\r': //RETURN
		  if (fMinor == '\n') { // nettext /n/r data ...
  			fClass = tokDropchar;
  			fMajor = tokNull; 
  			fMinor = 0;  
  			return;
		    }
		   goto caseNewline;
		   
		case '\n': //NEWLINE:
		  if (fMinor == '\r') { // msdos /r/n data ...
  			fClass = tokDropchar;
  			fMajor = tokNull; 
  			fMinor = 0;  
  			return;
		    }
		caseNewline:
			fClass = tokControl;
			fMajor = tokNewline; //tokSpecialChar;
			fMinor = c; //tokNewline;
			return;
			 
		default: 
			fClass = tokText;
			fMajor = c;
			fMinor = c; // MapChar( c);
			return; 
			
		}
}

 
 
// static
char* DRichprocess::gGenCharMap=  DRichprocess::ReadOutputMap("gen");
char* DRichprocess::gSymCharMap=  DRichprocess::ReadOutputMap("sym");

char* DRichprocess::ReadOutputMap( char *file)
{
// from rtfmaps.h
#ifdef WIN_MAC
# define 	genInMap  mac_gen_CharCode 
# define 	symInMap  mac_sym_CharCode 
#else
# define 	genInMap  ansi_gen_CharCode 
# define 	symInMap  ansi_sym_CharCode 
#endif
	short *inMap, i, val;
	char *outMap = (char*) MemNew( rtfSC_MaxChar*sizeof(char));
	
	if (*file == 's') inMap= symInMap;
	else inMap= genInMap;

	for (i= 0; i<rtfSC_MaxChar; i++) outMap[i]= 0;
	for (i= 255; i >= 0; i--) {
		val= inMap[i];
		if (val>=0 && val<rtfSC_MaxChar) outMap[val]= i;
		}
	return outMap;
}



void DRichprocess::PutLitChar(short c)
{
	if (fTextSize >= fTextMax) {
		fTextMax = fTextSize + 1024;
		fText= (char*) MemMore( fText, fTextMax + 1);
		}
	fText[fTextSize++]= (char) c;
	/* fText[fTextSize]= '\0'; */
}

void DRichprocess::PutLitStr(char *s)
{
	long len = StrLen(s);
	if (len + fTextSize >= fTextMax) {
		fTextMax = fTextSize + len + 1024;
		fText= (Nlm_CharPtr) MemMore( fText, fTextMax + 1);
		}
	Nlm_MemCopy( fText + fTextSize, s, len); /* +1 for nul */
	fTextSize += len;
}

void DRichprocess::PutLitCharWithStyle(short c)
{
	if (IsNewStyle()) StoreStyle(fOldStyleRec.style, FALSE);  
	PutLitChar(c);
}



void DRichprocess::PutStdChar( short stdCode)
{
	if (fOutMap) {
		int och = fOutMap[stdCode];
		if (och == rtfSC_nothing)	{  
			char	buf[80];
			sprintf(buf, "{{%s}}", RTFStdCharName(stdCode));
			PutLitStr(buf);
			}
		else {
			PutLitChar(och);
			}
		}
	else {
		PutLitChar(stdCode); 
		}
}


void DRichprocess::PutStdCharWithStyle(short stdCode)
{
	if (IsNewStyle()) StoreStyle(fOldStyleRec.style, FALSE);  
	PutStdChar(stdCode);
}


void DRichprocess::StoreStyle(DRichStyle& theStyle, Boolean force)
{
	if (force || fTextSize) {  
		if (fStyleCount >= fStyleMax) {
			fStyleMax  = fStyleCount + 10;
			fStyleArray= (DRichStyle*) MemMore(fStyleArray, fStyleMax*sizeof(DRichStyle));
			}
	
		theStyle.nextofs= fTextSize; //fLastTextSize;
		//theStyle.textlen= fTextSize - fLastTextSize;
		fLastTextSize= fTextSize;
		if (!fFont) theStyle.font= GetNlmFont(); //x
			 
		theStyle.last= FALSE; /* make sure... */
		if (fStyleArray) fStyleArray[fStyleCount]= theStyle;
		fStyleCount++; 
		 
		//PutLitChar(chStyleTag);
		}
	fOldStyleRec= fStyleRec; /* gOldStyle= gStyle;  */
	(void) GetNlmFont(); 		 
	NotNewStyle();
}

void DRichprocess::StoreStyleObject(DStyleObject* sob, short width, short height)
{
	fStyleRec.style.ispict= true;
	fStyleRec.style.pixwidth= width; 
	fStyleRec.style.pixheight= height; 
	fStyleRec.style.fObject= sob;
	PutLitChar(' ');  
	StoreStyle(fStyleRec.style, TRUE);
	fStyleRec.style.fObject= NULL;
	fStyleRec.style.pixwidth= 0; 
	fStyleRec.style.pixheight= 0; 
	fStyleRec.style.ispict= FALSE;
	fStyleRec.style.ismap= FALSE;
	fOldStyleRec= fStyleRec;
	NewStyle();
	//PutLitCharWithStyle(' ');
}


Boolean DRichprocess::IsNewStyle() 
{ 
#if 1 //x
	return fNewStyle; 
#else
	return TestNewStyle();
#endif
}

Boolean DRichprocess::TestNewStyle()
{
	fNewStyle = ( fOldStyleRec.fontname != fStyleRec.fontname 
					|| fOldStyleRec.fontfamily != fStyleRec.fontfamily 
					|| fOldStyleRec.fontsize != fStyleRec.fontsize 
					|| fOldStyleRec.style.IsNotEqual( &fStyleRec.style)
					);
	return fNewStyle;
}



Nlm_FonT DRichprocess::GetNlmFont()
{
#if 1  //x
		// GetFont is a time waster, cut down # of these calls !!
	if (!fFont || IsNewStyle()) {
		fFont= Nlm_GetFont( fStyleRec.fontname, fStyleRec.fontsize, 
					fStyleRec.style.bold, fStyleRec.style.italic, fStyleRec.style.underline != 0, 
					fStyleRec.fontfamily);
		}
#else
		fFont= Nlm_GetFont( fStyleRec.fontname, fStyleRec.fontsize, 
					fStyleRec.style.bold, fStyleRec.style.italic, fStyleRec.style.underline != 0, 
					fStyleRec.fontfamily);
#endif
	fStyleRec.style.font= fFont;
	fOldStyleRec= fStyleRec; 
	NotNewStyle();
	return fFont;
}

void DRichprocess::PushStyle()
{
	if (fStyleStackSize<kMaxStyleStack) 
		fStyleStack[fStyleStackSize++]= fStyleRec;
}

void DRichprocess::PopStyle()
{			
	if (fStyleStackSize>0) 
		fStyleRec= fStyleStack[--fStyleStackSize];
	NewStyle();
}


void DRichprocess::DefaultParag()
{
	fParaFmt= fDefParaFmt;
}

void DRichprocess::SetDefaultParag(DParagraph* theDefault)
{
	fDefParaFmt= *theDefault;
}

void DRichprocess::DefaultStyle()
{
	fStyleRec= gDefaultStyle;
	TestNewStyle();
}


void DRichprocess::NewParagraph()
{
	/* when new data forces use of new Dgg_ParData, 
		append current text & para & col to growing doc */
#if 1			
	(void) GetNlmFont();  
	StoreStyle( fStyleRec.style, TRUE);	/* gOldStyle */
	//fTextSize--; /* unput last chStyleTag */
#endif
 
	fStyleArray[fStyleCount-1].last= TRUE;
	fText[fTextSize]= 0;
	
	fDoc->Append( fText, &fParaFmt, fStyleArray, fStyleCount);

	if (fProgress && fDataFile && (fParaCount % 10 == 0)) 
		(void) Nlm_MonitorIntValue(fProgress, fDataFile->Tell());
	fParaCount++;
	fOldParaFmt= fParaFmt;
	fParaFmt.numstops= fTextSize= fLastTextSize= fStyleCount= 0;
}





void DRichprocess::handleUnknownClass()
{
	// nothing
}

void DRichprocess::handleTextClass()
{
	if (IsNewStyle()) StoreStyle(fOldStyleRec.style, FALSE);  
	PutLitChar(fMinor); 
	fLastChar= fMajor;
}

void DRichprocess::handleControlClass()
{
	switch (fMajor) {
		
		case tokNewline:
			PutLitCharWithStyle ('\n');
			NewParagraph();
			break;

		default:
			break;
		}

}


