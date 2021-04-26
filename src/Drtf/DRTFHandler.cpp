// DRTFHandler.cpp
// d.g.gilbert


#include "Dvibrant.h"
#include "DFile.h"
#include "DRTFHandler.h"
#include "DRichProcess.h"
#include "DRichViewNu.h"
#include "DRichMoreStyle.h"



# define	rtfInternal
#define STDARG
# include	"rtf.h"
# undef		rtfInternal



enum {
	RTFBreakInData = -6,		// mark end of data buffer, but not EOF 
	chPictTag = 26,
	kWaitTables = DRichHandler::kAtEnd + 1
 	};
 	
 	
class DRTFprocess : public DRichprocess
{
public:
	static void RTFWriterInit(char* mapfilepath, char* charMapFile, char* symMapFile);
	static void RTFWriterEnd();

	DRTFprocess( DFile* itsFile, DRichView* itsDoc, Nlm_MonitorPtr progress);
	virtual ~DRTFprocess();

	short GetToken(); // temp override
	void  RouteToken(); // temp override
	void  SetToken();
	
	void	handleTextClass();
	void	handleControlClass(); 
	void  handleGroupClass();

protected:
	char* 	fPict;
	ulong 	fPictMax, fPictSize;
	long 		fPicNibble;
	short 	fLastNibble, fPictKind;
	Boolean	fInPic, fGlobalsInUse;
	
	void  PutPicChar(short c);
	void  PutStdChar(short stdCode);

  void  Destination();
  void  SpecialChar();
  void  DocAttr();
  void  SectAttr();
  void  TblAttr();
  void  ParAttr();
  void  CharAttr();
  void  PictAttr();
  void  BookmarkAttr();
  void  NeXTGrAttr();
  void  FieldAttr();
  void  TOCAttr();
  void  PosAttr();
  void  ObjAttr();
  void  FNoteAttr();
  void  KeyCodeAttr();
  void  ACharAttr();
  void  FontAttr();
  void  FileAttr();
  void  FileSource();
  void  DrawAttr();
  void  IndexAttr();
  void  SelectLanguage();
};



//class DRTFsetup

static char* kRTFgenmap = "show-gen";
static char* kRTFsymmap = "show-sym";
static char* kRTFfolder = "rtf-maps";

class DRTFsetup  
{
public:
	DRTFsetup();
	~DRTFsetup();
};

static DRTFsetup* gRTFsetup = NULL;

#if 0
static DRTFsetup  rtfSetup; // initialize here
#endif

DRTFsetup::DRTFsetup()
{
	char *progpath;
	
	progpath= (char*) DFileManager::GetProgramPath();  
	progpath= (char*) DFileManager::PathOnlyFromPath( progpath);
	progpath= (char*) DFileManager::BuildPath( progpath, kRTFfolder, NULL);
	// progpath so far is all stored in DFileManager static var
	// progpath= StrDup( progpath);
	DRTFprocess::RTFWriterInit(progpath, kRTFgenmap, kRTFsymmap);		// once-only writer initialization 
	//MemFree(progpath);
}

DRTFsetup::~DRTFsetup()
{
	DRTFprocess::RTFWriterEnd();			// once-only writer cleanup 
}




//class  DRTFHandler

DRTFHandler::DRTFHandler(DRichView* itsDoc, DFile* savefile) :
	DRichHandler( itsDoc, savefile)
{
	fFormatName = "RTF";
	fMinDataToProcess= 128; // no fixed min, need to look at data...
  if (gRTFsetup==NULL) gRTFsetup= new DRTFsetup();
}

char* DRTFHandler::IsRich(char* buf, ulong buflen)
{
	if (buflen==0) buflen= strlen(buf);
	ulong len = buflen;
	while (len>3) {
		char* cp= (char*) MemChr(buf,'{',len);
		if (!cp)
			return NULL;		
		else if (cp[1] == '\\' && cp[2] == 'r' && cp[3] == 't' && cp[4] == 'f') 
			return cp;
		else {
			cp++;
			len -= (cp - buf);
			buf = cp;
			}
		}
	return NULL;
}


DRichprocess* DRTFHandler::NewProcessor( DFile* itsFile, Nlm_MonitorPtr progress)
{
		fProcessor = new DRTFprocess( itsFile, fDoc, progress);
		return fProcessor;
}

Boolean DRTFHandler::ProcessData( char* cbeg, char* cend, Boolean endOfData, 
					ulong& dataRemaining) 
{
 
	char  * cp;
	//char csave = 0;  
	//ulong clen;

	//return DRichHandler::ProcessData( cbeg, cend, endOfData, dataRemaining);		

	
		// RTFRouteToken() is failing in stylesheets because it 
		// calls RTFGetToken when we have no more !!!...
		// ReadStyleSheet, ReadColorTbl, ReadFontTbl all call RTFGetToken !!!
		// need to patch this to parse the RTF for {\fonttbl...}{\colortbl...}{\stylesheet...} 
		// and read until they are all collected !? (at top of \rtf file)
		
	if (fProcessStage == kAtStart && !endOfData) {  
		// patch to wait for {\fonttbl...}{\colortbl...}{\stylesheet...}
		// assume \stylesheet always last ?!
		Nlm_Uint4	foundat = 0;
		Boolean 	found = Nlm_StrngPos(cbeg, "{\\stylesheet", 0, false, &foundat);
		if (!found) 
			return false; // wait for more data ...
		else {
			Boolean notdone= true;
			short level= 1;
			cp= cbeg + foundat + 12;
			while (cp < cend && notdone) {
				cp++;
				if (*cp == '{') level++;
				else if (*cp == '}') { level--; if (level<0) notdone= false; }
				}	
			if (notdone) return false; // wait for more data ...
			}
		}
		 
	cp= cend; 
	
	if (!endOfData) while (cp - cbeg > fLastScanto && *cp != '}') cp--;
	if (cp - cbeg <= fLastScanto) {
		fLastScanto= cend - cbeg;  
		if (!endOfData) return false; // !??! don't process unterminated rtf block
		else cp= cend; 
		}

	if (*cp) cp++;
	fcsave= *cp; 
	*cp= 0;
	fclen= cp - cbeg; // ?? +1
	
	if (fclen && fDocFile) {
		//fclen= cp - cbeg;
		if (cbeg[fclen-1] == 0) fclen--;
		fDocFile->WriteData( cbeg, fclen);
		}	

	return this->ProcessData2( cp, cbeg, cend, endOfData, dataRemaining);
}







//class  DRTFprocess

//static char		gGenCharMap[rtfSC_MaxChar];
//static char		gSymCharMap[rtfSC_MaxChar];
//static char * gOutMap = gGenCharMap;
static char	* gMapfilePath = NULL;

// !! DAMN, we can't deal with this kind of global && multiple processors

DRTFprocess* gRTFprocess = NULL;


extern "C" void rtfHandleUnknownClass()
{
	gRTFprocess->SetToken();
	gRTFprocess->handleUnknownClass();
}

extern "C" void rtfHandleTextClass()
{
#if 0 //x
	gRTFprocess->SetToken();
#else
	// inline for speed here...
	gRTFprocess->fClass= rtfClass; 
	gRTFprocess->fMajor= rtfMajor;
	gRTFprocess->fMinor= rtfMinor;
	gRTFprocess->fParam= rtfParam;
#endif

	gRTFprocess->handleTextClass();
}

extern "C" void rtfHandleControlClass()
{
	gRTFprocess->SetToken();
	gRTFprocess->handleControlClass();
}

extern "C" void rtfHandleGroupClass()
{
	gRTFprocess->SetToken();
	gRTFprocess->handleGroupClass();
}


extern "C" short rtfGetOneChar()
{
#if 0 //x
			// this is slower than we like -- this is called a lot
	return gRTFprocess->GetOneChar();
#else
	short	c = EOF;

	if (gRTFprocess->fDataBuffer) {
		c= *gRTFprocess->fDataBuffer;
		if (gRTFprocess->fDataSize) {	// for counted data
			gRTFprocess->fDataSize--;
			gRTFprocess->fDataBuffer++;
			if (!gRTFprocess->fDataSize) gRTFprocess->fDataBuffer= NULL; //  so EOF next time
			}
		else if (c) gRTFprocess->fDataBuffer++; // for null-term data
		else if (!gRTFprocess->fEndOfData) c= DRichprocess::tokBreakInData;
		else c= EOF;
		}
	else if (gRTFprocess->fDataFile) 
		c = fgetc( gRTFprocess->fDataFile->fFile);
	return (c);
#endif
}



enum {
	kMaxstops = 30
	};


DRTFprocess::DRTFprocess( DFile* itsFile, DRichView* itsDoc, Nlm_MonitorPtr progress) :
	DRichprocess( itsFile, itsDoc, progress),
	fPict(NULL), fPictKind(0), fInPic(false)
{ 	
	fPictMax= 0;
	fPictSize= 0;
	fPicNibble= 0;
	fLastNibble= 0;
	fGlobalsInUse= false;

#if 1
	// damn, this is a mess	
	fParaFmt.tabstops= (short*) MemNew( kMaxstops * sizeof(short));
	fParaFmt.tabkinds= (char*) MemNew( kMaxstops * sizeof(char));
	fOldParaFmt= fParaFmt;
	fDefParaFmt= fParaFmt;
#endif
	
		// !! DAMN, we can't deal with this kind of global && multiple processors
	if (gRTFprocess) {
		Nlm_Message( MSG_OK, "%s", "RTFprocess globals in use by other processor"); 
		fGlobalsInUse= true;
		return;
		}
	else 
		gRTFprocess= this; 
	
	fOutMap= DRichprocess::gGenCharMap;

	RTFInit();
	
	/* install class callbacks */
	RTFSetClassCallback(rtfUnknown, rtfHandleUnknownClass);
	RTFSetClassCallback(rtfText, rtfHandleTextClass);
	RTFSetClassCallback(rtfControl, rtfHandleControlClass);
	RTFSetClassCallback(rtfGroup, rtfHandleGroupClass); 
}

DRTFprocess::~DRTFprocess()
{
	Nlm_MemFree(fPict);
	gRTFprocess= NULL;
}


void DRTFprocess::SetToken()  // temp override
{
		// damn -- some calls to RTFrouteToken bypass our GetToken @!
	fClass= rtfClass; 
	fMajor= rtfMajor;
	fMinor= rtfMinor;
	fParam= rtfParam;
}

short DRTFprocess::GetToken()  // temp override
{
	short tok;
	if (fGlobalsInUse) tok= tokEOF;
	else tok= RTFGetToken();
		// DAMN must convert rtftoken vals to new tok vals
	if (tok == rtfEOF) tok= tokEOF;
	
	fClass= tok;
	fMajor= rtfMajor;
	fMinor= rtfMinor;
	fParam= rtfParam;
	return tok;
}

void  DRTFprocess::RouteToken() // temp override
{
	RTFRouteToken();
}

	
void DRTFprocess::PutPicChar(short c)
{
	if (fPictSize >= fPictMax) {
		fPictMax = fPictSize + 1024;
		if (!fPict) fPict= (char*) Nlm_MemNew(fPictMax+1);
		else fPict= (char*) Nlm_MemMore( fPict, fPictMax+1);
		}
	fPict[fPictSize++]= (char) c;
}



void DRTFprocess::PutStdChar(short stdCode)
{
#if 1
	DRichprocess::PutStdChar(stdCode);
#else
	int och = gOutMap[stdCode];
	if (och == rtfSC_nothing)	{  /* no output sequence in map */
		char	buf[rtfBufSiz];
		sprintf(buf, "{{%s}}", RTFStdCharName (stdCode));
		PutLitStr(buf);
		}
	else {
		PutLitChar(och);
		}
#endif
}





#define	defaultCharMapFile	"show-gen"  /* should be calling parameter to WriterInit() ? */
#define	defaultSymMapFile		"show-sym"  /* should be calling parameter to WriterInit() ? */
#define defaultRTFfolder 		"rtf-maps" 



extern "C" FILE* defaultOpenLibfileProc(char* file, char* mode)
{
	char *pathname, namebuf[512];
	StrNCpy(namebuf, gMapfilePath, 500);
	pathname= Nlm_FileBuildPath(namebuf, NULL, file);
	return Nlm_FileOpen( pathname, mode);
}






void DRTFprocess::RTFWriterInit(char* mapfilepath, char* charMapFile, char* symMapFile)
{	
	/* perform once-only initializations here */

	if (mapfilepath)	
		gMapfilePath= StrDup(mapfilepath);
	else {
		char pathname[512], *progpath;
		char* nameonly;
		long len;
		
#if 1
		progpath= (char*) DFileManager::GetProgramPath();  
		progpath= (char*) DFileManager::PathOnlyFromPath( progpath);
		mapfilepath= (char*) DFileManager::BuildPath( progpath, kRTFfolder, NULL);
#else		
		Nlm_ProgramPath( (char*)pathname, 512);
		nameonly= Nlm_FileNameFind( (char*)pathname);
		len= StrLen(pathname) - StrLen(nameonly);
		pathname[len]= 0;
		mapfilepath= Nlm_FileBuildPath(pathname, defaultRTFfolder, NULL);
#endif

		gMapfilePath= StrDup(mapfilepath);
		}

	/* optionally: RTFSetReadHook( rtfReaderProgressProc); */
	/*  ^^^ if we want to show progress bar or further handle token */
	
	RTFSetOpenLibFileProc( defaultOpenLibfileProc);

#if 1

	//DRichprocess::gGenCharMap= DRichprocess::ReadOutputMap("gen");
	//DRichprocess::gSymCharMap= DRichprocess::ReadOutputMap("sym");
  //gOutMap= gGenCharMap;
	
#else
	if (charMapFile == NULL) charMapFile= defaultCharMapFile;
	RTFSetCharSetMap( charMapFile, rtfCSGeneral);
	if (RTFReadOutputMap (charMapFile, gGenCharMap, 1) == 0)
		RTFPanic ("Cannot read output map %s", charMapFile);
  gOutMap= gGenCharMap;
  
	if (symMapFile == NULL) symMapFile= defaultSymMapFile;
	RTFSetCharSetMap( symMapFile, rtfCSSymbol);
	if (RTFReadOutputMap (symMapFile, gSymCharMap, 1) == 0)
		RTFPanic ("Cannot read output map %s", symMapFile);
#endif		
		
#if 0
	/* one time generate charsets in .h format */
	RTFReadCharSetMap ("mac-gen", rtfCSGeneral);
	RTFReadCharSetMap ("mac-sym", rtfCSSymbol);		// superset of ansi-sym !
	RTFReadCharSetMap ("mwin-gen", rtfCSGeneral); // superset of ansi-gen 
	RTFReadCharSetMap ("pc-gen", rtfCSGeneral);
	RTFReadCharSetMap ("pc-sym", rtfCSSymbol);		// == ansi-sym
	RTFReadCharSetMap ("pca-gen", rtfCSGeneral);
	RTFReadCharSetMap ("pca-sym", rtfCSSymbol);		// == ansi-sym
	RTFReadCharSetMap ("ansi-gen", rtfCSGeneral);
	RTFReadCharSetMap ("ansi-sym", rtfCSSymbol);
	//RTFReadCharSetMap ("show-gen", rtfCSGeneral);
	//RTFReadCharSetMap ("show-sym", rtfCSSymbol);
#endif

}



void DRTFprocess::RTFWriterEnd()
{
	Nlm_MemFree( gMapfilePath);
}






void DRTFprocess::handleTextClass()
{
	if (fInPic) {
			// this is encoded pic data, need opts for binary pic data !
		if (fPicNibble & 1) {
			short abyte = RTFCharToHex(fLastNibble) * 16 + RTFCharToHex(fMajor);
			PutPicChar( abyte);
			}
		else  
			fLastNibble= fMajor;
		fPicNibble++;
		}
		
	else {
		if (IsNewStyle()) StoreStyle(fOldStyleRec.style, FALSE); 
		
		if (fMinor != rtfSC_nothing)
			PutStdChar(fMinor); 
		else {
			char	buf[rtfBufSiz];
			if (fMajor < 128)	/* in ASCII range */
				sprintf(buf, "[[%c]]", fMajor);
			else
				sprintf(buf, "[[\\'%02x]]", fMajor);
			PutLitStr(buf);
			}
		}
}


void DRTFprocess::handleGroupClass()
{
 
	switch (fMajor) {
	
	case rtfBeginGroup:
		/* push gStyleRec .. */
		if (fStyleStackSize<kMaxStyleStack) fStyleStack[fStyleStackSize++]= fStyleRec;
		break;
		
	case rtfEndGroup:
	
		if (fInPic) {
			/* end of pic data here... */
 
			fPict[fPictSize]= 0;
			fStyleRec.style.font= fFont; /* !?!? */
			fStyleRec.style.ispict= TRUE;
#if 1
			fStyleRec.style.fObject= 
					new DPictStyle( fPictKind, fPict, fPictSize, false); 
#else
			fStyleRec.style.picdata= (char*) MemDup(fPict, fPictSize); 
			fStyleRec.style.picsize= fPictSize;
			fStyleRec.style.pickind= fPictKind;
#endif

			PutLitChar(' '); // (chPictTag); /* put a space for some real text ? */
			StoreStyle(fStyleRec.style, TRUE);

			fPictSize= 0;
			fPicNibble= 0;
			fLastNibble= 0;
#if 1
			fStyleRec.style.fObject= NULL;
#else
			fStyleRec.style.picdata= 0;
			fStyleRec.style.picsize= 0;
			fStyleRec.style.pickind= 0;
#endif
			fStyleRec.style.ispict= FALSE;
			fStyleRec.style.pixwidth= 0; 
			fStyleRec.style.pixheight= 0; 
			fOldStyleRec= fStyleRec;
			fOldStyleRec.fontname= NULL; /* ?? force new style ? */
			fInPic= FALSE;
			StoreStyle(fStyleRec.style, TRUE);
			PutLitChar(' '); /* put a space for some real text ? */
			}
			
		if (fStyleStackSize>0) fStyleRec= fStyleStack[--fStyleStackSize];
			
		if (StrNICmp (fStyleRec.fontname, "Symbol", 6) == 0) {
			RTFSetCharSet(rtfCSSymbol);
			if (fOutMap != gSymCharMap) fOutMap= gSymCharMap;
			}
		else {
			RTFSetCharSet(rtfCSGeneral);
			if (fOutMap != gGenCharMap) fOutMap= gGenCharMap;
			}
		NewStyle();
		break;	
		}
 
}


void DRTFprocess::handleControlClass()
{
	switch (fMajor) {

	case rtfVersion:
		break;
	case rtfDefFont:
		/*
		 * fParam contains the default font number.  Beware
		 * that there is no guarantee that this font will actually
		 * be listed in the font table!
		 */
		break;
	case rtfCharSet:
		/* handled by reader */
		break;
	case rtfDestination:
		Destination();
		break;
	case rtfFontFamily:
		/* only occurs within font table - can ignore */
		break;
	case rtfColorName:
		/* only occurs within color table - can ignore */
		break;
	case rtfSpecialChar:
		SpecialChar();
		break;
	case rtfStyleAttr:
		/* only occurs within stylesheet - can ignore */
		break;
	case rtfDocAttr:
		DocAttr();
		break;
	case rtfSectAttr:
		SectAttr();
		break;
	case rtfTblAttr:
		TblAttr();
		break;
	case rtfParAttr:
		ParAttr();
		break;
	case rtfCharAttr:
		CharAttr();
		break;
	case rtfPictAttr:
		PictAttr();
		break;
	case rtfBookmarkAttr:
		BookmarkAttr();
		break;
	case rtfNeXTGrAttr:
		NeXTGrAttr();
		break;
	case rtfFieldAttr:
		FieldAttr();
		break;
	case rtfTOCAttr:
		TOCAttr();
		break;
	case rtfPosAttr:
		PosAttr();
		break;
	case rtfObjAttr:
		ObjAttr();
		break;
	case rtfFNoteAttr:
		FNoteAttr();
		break;
	case rtfKeyCodeAttr:
		KeyCodeAttr();
		break;
	case rtfACharAttr:
		ACharAttr();
		break;
	case rtfFontAttr:
		FontAttr();
		break;
	case rtfFileAttr:
		FileAttr();
		break;
	case rtfFileSource:
		FileSource();
		break;
	case rtfDrawAttr:
		DrawAttr();
		break;
	case rtfIndexAttr:
		IndexAttr();
		break;
	}

}


/*
 * Control class major number handlers.  Each one switches on
 * the minor numbers that occur within the major number.
 */
/*
 * This function notices destinations that should be ignored
 * and skips to their ends.  This keeps, for instance, picture
 * data from being considered as plain text.
 */


void DRTFprocess::Destination()
{
	switch (fMinor)
	{
	
	case rtfFontTbl:
		/* will never occur because of default destination reader */
		break;
	case rtfFontAltName:
		break;
	case rtfEmbeddedFont:
		break;
	case rtfFontFile:
		break;
	case rtfFileTbl:
		break;
	case rtfFileInfo:
		break;
	case rtfColorTbl:
		/* will never occur because of default destination reader */
		break;
	case rtfStyleSheet:
		/* will never occur because of default destination reader */
		break;
	case rtfKeyCode:
		/*
		 * Currently will never occur because the stylesheet
		 * reader eats the keycode group.
		 */
		break;
	case rtfRevisionTbl:
		break;
	case rtfInfo:
		RTFSkipGroup();
		break;
	case rtfITitle:
		RTFSkipGroup();
		break;
	case rtfISubject:
		RTFSkipGroup();
		break;
	case rtfIAuthor:
		break;
		RTFSkipGroup();
	case rtfIOperator:
		RTFSkipGroup();
		break;
	case rtfIKeywords:
		RTFSkipGroup();
		break;
	case rtfIComment:
		RTFSkipGroup();
		break;
	case rtfIVersion:
		RTFSkipGroup();
		break;
	case rtfIDoccomm:
		RTFSkipGroup();
		break;
	case rtfIVerscomm:
		break;
	case rtfNextFile:
		RTFSkipGroup();	/* dgg */
		break;
	case rtfTemplate:
		RTFSkipGroup();	/* dgg */
		break;
	case rtfFNSep:
		break;
	case rtfFNContSep:
		RTFSkipGroup();
		break;
	case rtfFNContNotice:
		RTFSkipGroup();
		break;
	case rtfENSep:
		break;
	case rtfENContSep:
		break;
	case rtfENContNotice:
		break;
	case rtfPageNumLevel:
		break;
	case rtfParNumLevelStyle:
		break;
		
	case rtfHeader:
		break;
	case rtfFooter:
		break;
	case rtfHeaderLeft:
		break;
	case rtfHeaderRight:
		break;
	case rtfHeaderFirst:
		break;
	case rtfFooterLeft:
		break;
	case rtfFooterRight:
		break;
	case rtfFooterFirst:
		break;
		
	case rtfParNumText:
		break;
	case rtfParNumbering:
		break;
	case rtfParNumTextAfter:
		break;
	case rtfParNumTextBefore:
		break;
	case rtfBookmarkStart:
		break;
	case rtfBookmarkEnd:
		break;
		
	case rtfPict:
		/* at first, parse group and pull out pict size: \pict\macpict\picw34\pich21\picscaled 
		/* RTFSkipGroup();   /* !! fix this later to process pict */
		/* NewParagraph();  ?? */
		break;
		
	case rtfObject:
		break;
	case rtfObjClass:
		break;
	case rtfObjName:
		break;
	case rtfObjTime:
		break;
	case rtfObjData:
		break;
	case rtfObjAlias:
		break;
	case rtfObjSection:
		break;
	case rtfObjResult:
		break;
	case rtfObjItem:
		break;
	case rtfObjTopic:
		break;
	case rtfDrawObject:
		break;
	case rtfFootnote:
		break;
	case rtfAnnotRefStart:
		break;
	case rtfAnnotRefEnd:
		break;
	case rtfAnnotID:
		break;
	case rtfAnnotAuthor:
		break;
	case rtfAnnotation:
		break;
	case rtfAnnotRef:
		break;
	case rtfAnnotTime:
		break;
	case rtfAnnotIcon:
		break;
		
	case rtfField:
		break;
	case rtfFieldInst:
		RTFSkipGroup(); /* dgg, 27may94 */
		break;
	case rtfFieldResult:
		/* this text should be displayed... */
		break;
	case rtfDataField:
		RTFSkipGroup(); /* ?? binary data?? dgg, 27may94 */
		break;
		
	case rtfIndex:
		break;
	case rtfIndexText:
		break;
	case rtfIndexRange:
		RTFSkipGroup();
		break;
	case rtfTOC:
		break;
	case rtfNeXTGraphic:
		break;
	}
}


void DRTFprocess::SpecialChar()
{
	switch (fMinor)
	{
	case rtfIIntVersion:
		break;
	case rtfICreateTime:
		break;
	case rtfIRevisionTime:
		break;
	case rtfIPrintTime:
		break;
	case rtfIBackupTime:
		break;
	case rtfIEditTime:
		break;
	case rtfIYear:
		break;
	case rtfIMonth:
		break;
	case rtfIDay:
		break;
	case rtfIHour:
		break;
	case rtfIMinute:
		break;
	case rtfISecond:
		break;
	case rtfINPages:
		break;
	case rtfINWords:
		break;
	case rtfINChars:
		break;
	case rtfIIntID:
		break;
	case rtfCurHeadDate:
		break;
	case rtfCurHeadDateLong:
		break;
	case rtfCurHeadDateAbbrev:
		break;
	case rtfCurHeadTime:
		break;
	case rtfCurHeadPage:
		break;
	case rtfSectNum:
		break;
	case rtfCurFNote:
		break;
	case rtfCurAnnotRef:
		break;
	case rtfFNoteSep:
		break;
	case rtfFNoteCont:
		break;
	case rtfCell:
		PutStdCharWithStyle (rtfSC_space);	/* make sure cells are separated */
		break;
		
	case rtfRow:
		PutLitCharWithStyle ('\n');
		break;
	case rtfPar:
		PutLitCharWithStyle ('\n');
		NewParagraph();
		break;
	case rtfSect:
		PutLitCharWithStyle ('\n');
		NewParagraph(); /* ?? */
		break;
	case rtfPage:
		PutLitCharWithStyle ('\n');
		fParaFmt.newPage= TRUE; /* ?? ??*/
		NewParagraph();
		fParaFmt.newPage= FALSE; /* ?? ?? */
		break;
	case rtfColumn:
		break;
	case rtfLine:
		PutLitCharWithStyle ('\n');
		break;
		
	case rtfSoftPage:
		break;
	case rtfSoftColumn:
		break;
	case rtfSoftLine:
		break;
	case rtfSoftLineHt:
		break;
		
	case rtfTab:
		PutLitCharWithStyle ('\t'); 
		break;
		
	case rtfEmDash:
		PutStdCharWithStyle (rtfSC_emdash);
		break;
	case rtfEnDash:
		PutStdCharWithStyle (rtfSC_endash);
		break;
	case rtfEmSpace:
		break;
	case rtfEnSpace:
		break;
	case rtfBullet:
		PutStdCharWithStyle (rtfSC_bullet);
		break;
	case rtfLQuote:
		PutStdCharWithStyle (rtfSC_quoteleft);
		break;
	case rtfRQuote:
		PutStdCharWithStyle (rtfSC_quoteright);
		break;
	case rtfLDblQuote:
		PutStdCharWithStyle (rtfSC_quotedblleft);
		break;
	case rtfRDblQuote:
		PutStdCharWithStyle (rtfSC_quotedblright);
		break;
	case rtfFormula:
		break;
	case rtfNoBrkSpace:
		PutStdCharWithStyle (rtfSC_nobrkspace);
		break;
	case rtfNoReqHyphen:
		break;
	case rtfNoBrkHyphen:
		PutStdCharWithStyle (rtfSC_nobrkhyphen);
		break;
	case rtfOptDest:
		break;
	case rtfLTRMark:
		break;
	case rtfRTLMark:
		break;
	case rtfNoWidthJoiner:
		break;
	case rtfNoWidthNonJoiner:
		break;
	case rtfCurHeadPict:
		break;
	}
}


void DRTFprocess::DocAttr()
{
	switch (fMinor)
	{
	case rtfDefTab:
		fParaFmt.deftabstop = (int) fParam / 20;
		break;
		
	case rtfHyphHotZone:
		break;
	case rtfHyphConsecLines:
		break;
	case rtfHyphCaps:
		break;
	case rtfHyphAuto:
		break;
	case rtfLineStart:
		break;
	case rtfFracWidth:
		break;
	case rtfMakeBackup:
		break;
	case rtfRTFDefault:
		break;
	case rtfPSOverlay:
		break;
	case rtfDocTemplate:
		break;
	case rtfDefLanguage:
		break;
	case rtfFENoteType:
		break;
	case rtfFNoteEndSect:
		break;
	case rtfFNoteEndDoc:
		break;
	case rtfFNoteText:
		break;
	case rtfFNoteBottom:
		break;
	case rtfENoteEndSect:
		break;
	case rtfENoteEndDoc:
		break;
	case rtfENoteText:
		break;
	case rtfENoteBottom:
		break;
	case rtfFNoteStart:
		break;
	case rtfENoteStart:
		break;
	case rtfFNoteRestartPage:
		break;
	case rtfFNoteRestart:
		break;
	case rtfFNoteRestartCont:
		break;
	case rtfENoteRestart:
		break;
	case rtfENoteRestartCont:
		break;
	case rtfFNoteNumArabic:
		break;
	case rtfFNoteNumLLetter:
		break;
	case rtfFNoteNumULetter:
		break;
	case rtfFNoteNumLRoman:
		break;
	case rtfFNoteNumURoman:
		break;
	case rtfFNoteNumChicago:
		break;
	case rtfENoteNumArabic:
		break;
	case rtfENoteNumLLetter:
		break;
	case rtfENoteNumULetter:
		break;
	case rtfENoteNumLRoman:
		break;
	case rtfENoteNumURoman:
		break;
	case rtfENoteNumChicago:
		break;
		
	case rtfPaperWidth:
		break;
	case rtfPaperHeight:
		break;
	case rtfPaperSize:
		break;
	case rtfLeftMargin:
		break;
	case rtfRightMargin:
		break;
	case rtfTopMargin:
		break;
	case rtfBottomMargin:
		break;
	case rtfFacingPage:
		break;
	case rtfGutterWid:
		break;
	case rtfMirrorMargin:
		break;
	case rtfLandscape:
		break;
	case rtfPageStart:
		break;
	case rtfWidowCtrl:
		break;
	case rtfLinkStyles:
		break;
	case rtfNoAutoTabIndent:
		break;
	case rtfWrapSpaces:
		break;
	case rtfPrintColorsBlack:
		break;
	case rtfNoExtraSpaceRL:
		break;
	case rtfNoColumnBalance:
		break;
	case rtfCvtMailMergeQuote:
		break;
	case rtfSuppressTopSpace:
		break;
	case rtfSuppressPreParSpace:
		break;
	case rtfCombineTblBorders:
		break;
	case rtfTranspMetafiles:
		break;
	case rtfSwapBorders:
		break;
	case rtfShowHardBreaks:
		break;
	case rtfFormProtected:
		break;
	case rtfAllProtected:
		break;
	case rtfFormShading:
		break;
	case rtfFormDisplay:
		break;
	case rtfPrintData:
		break;
	case rtfRevProtected:
		break;
	case rtfRevisions:
		break;
	case rtfRevDisplay:
		break;
	case rtfRevBar:
		break;
	case rtfAnnotProtected:
		break;
	case rtfRTLDoc:
		break;
	case rtfLTRDoc:
		break;
	}
}


void DRTFprocess::SectAttr()
{
	switch (fMinor)
	{
	case rtfSectDef:
		break;
	case rtfENoteHere:
		break;
	case rtfPrtBinFirst:
		break;
	case rtfPrtBin:
		break;
	case rtfSectStyleNum:
		break;
	case rtfNoBreak:
		break;
	case rtfColBreak:
		break;
	case rtfPageBreak:
		break;
	case rtfEvenBreak:
		break;
	case rtfOddBreak:
		break;
	case rtfColumns:
		break;
	case rtfColumnSpace:
		break;
	case rtfColumnNumber:
		break;
	case rtfColumnSpRight:
		break;
	case rtfColumnWidth:
		break;
	case rtfColumnLine:
		break;
	case rtfLineModulus:
		break;
	case rtfLineDist:
		break;
	case rtfLineStarts:
		break;
	case rtfLineRestart:
		break;
	case rtfLineRestartPg:
		break;
	case rtfLineCont:
		break;
	case rtfSectPageWid:
		break;
	case rtfSectPageHt:
		break;
	case rtfSectMarginLeft:
		break;
	case rtfSectMarginRight:
		break;
	case rtfSectMarginTop:
		break;
	case rtfSectMarginBottom:
		break;
	case rtfSectMarginGutter:
		break;
	case rtfSectLandscape:
		break;
	case rtfTitleSpecial:
		break;
	case rtfHeaderY:
		break;
	case rtfFooterY:
		break;
	case rtfPageStarts:
		break;
	case rtfPageCont:
		break;
	case rtfPageRestart:
		break;
	case rtfPageNumRight:
		break;
	case rtfPageNumTop:
		break;
	case rtfPageDecimal:
		break;
	case rtfPageURoman:
		break;
	case rtfPageLRoman:
		break;
	case rtfPageULetter:
		break;
	case rtfPageLLetter:
		break;
	case rtfPageNumHyphSep:
		break;
	case rtfPageNumSpaceSep:
		break;
	case rtfPageNumColonSep:
		break;
	case rtfPageNumEmdashSep:
		break;
	case rtfPageNumEndashSep:
		break;
	case rtfTopVAlign:
		break;
	case rtfBottomVAlign:
		break;
	case rtfCenterVAlign:
		break;
	case rtfJustVAlign:
		break;
	case rtfRTLSect:
		break;
	case rtfLTRSect:
		break;
	}
}


void DRTFprocess::TblAttr()
{
	switch (fMinor)
	{
	case rtfRowDef:
		break;
	case rtfRowGapH:
		break;
	case rtfCellPos:
		break;
	case rtfMergeRngFirst:
		break;
	case rtfMergePrevious:
		break;
	case rtfRowLeft:
		break;
	case rtfRowRight:
		break;
	case rtfRowCenter:
		break;
	case rtfRowLeftEdge:
		break;
	case rtfRowHt:
		break;
	case rtfRowHeader:
		break;
	case rtfRowKeep:
		break;
	case rtfRTLRow:
		break;
	case rtfLTRRow:
		break;
	case rtfRowBordTop:
		break;
	case rtfRowBordLeft:
		break;
	case rtfRowBordBottom:
		break;
	case rtfRowBordRight:
		break;
	case rtfRowBordHoriz:
		break;
	case rtfRowBordVert:
		break;
	case rtfCellBordBottom:
		break;
	case rtfCellBordTop:
		break;
	case rtfCellBordLeft:
		break;
	case rtfCellBordRight:
		break;
	case rtfCellShading:
		break;
	case rtfCellBgPatH:
		break;
	case rtfCellBgPatV:
		break;
	case rtfCellFwdDiagBgPat:
		break;
	case rtfCellBwdDiagBgPat:
		break;
	case rtfCellHatchBgPat:
		break;
	case rtfCellDiagHatchBgPat:
		break;
	case rtfCellDarkBgPatH:
		break;
	case rtfCellDarkBgPatV:
		break;
	case rtfCellFwdDarkBgPat:
		break;
	case rtfCellBwdDarkBgPat:
		break;
	case rtfCellDarkHatchBgPat:
		break;
	case rtfCellDarkDiagHatchBgPat:
		break;
	case rtfCellBgPatLineColor:
		break;
	case rtfCellBgPatColor:
		break;
	}
}


void DRTFprocess::ParAttr()
{
	switch (fMinor)
	{
	case rtfParDef:
		DefaultParag();
		break;
	case rtfStyleNum:
		RTFExpandStyle (fParam);
		break;
		
	case rtfHyphenate:
		break;
	case rtfInTable:
		break;
	case rtfKeep:
		break;
	case rtfNoWidowControl:
		break;
	case rtfKeepNext:
		break;
	case rtfOutlineLevel:
		break;
	case rtfNoLineNum:
		break;
	case rtfPBBefore:
		break;
	case rtfSideBySide:
		break;
	case rtfQuadLeft:
		fParaFmt.just= 'l';
		break;
	case rtfQuadRight:
		fParaFmt.just= 'r';
		break;
	case rtfQuadJust:
		fParaFmt.just= 'l';
		break;
	case rtfQuadCenter:
		fParaFmt.just= 'c';
		break;

	case rtfFirstIndent:
		fParaFmt.firstInset= fParam / 20;  
		break;
	case rtfLeftIndent:
		fParaFmt.leftInset= fParam / 20;  
		break;
	case rtfRightIndent:
		fParaFmt.rightInset= fParam / 20; 
		break;

#if 1		
	case rtfSpaceBefore:
		fParaFmt.spaceBefore= fParam / 20; 
		break;
	case rtfSpaceAfter:
		fParaFmt.spaceAfter= fParam / 20; 
		break;
#endif
	case rtfSpaceBetween:
		fParaFmt.spaceBetween= fParam / 20; 
		break;
	case rtfSpaceMultiply:
		fParaFmt.spaceMultiply= (fParam > 0); 
		break;
	case rtfSubDocument:
		break;
	case rtfRTLPar:
		break;
	case rtfLTRPar:
		break;
		
	case rtfTabPos:
		if (fParaFmt.numstops<kMaxstops) {
		fParaFmt.tabkinds[fParaFmt.numstops]= DParagraph::kTabLeft;   
		fParaFmt.tabstops[fParaFmt.numstops++]= fParam / 20; 
		} 
		break;
	case rtfTabLeft:
		if (fParaFmt.numstops<kMaxstops) 
		fParaFmt.tabkinds[fParaFmt.numstops]= DParagraph::kTabLeft;  
		break;
	case rtfTabRight:
		if (fParaFmt.numstops<kMaxstops) 
		fParaFmt.tabkinds[fParaFmt.numstops]= DParagraph::kTabRight;  
		break;
	case rtfTabCenter:
		if (fParaFmt.numstops<kMaxstops) 
		fParaFmt.tabkinds[fParaFmt.numstops]= DParagraph::kTabCenter;  
		break;
	case rtfTabDecimal:
		if (fParaFmt.numstops<kMaxstops) 
		fParaFmt.tabkinds[fParaFmt.numstops]= DParagraph::kTabDecimal;  
		break;
	case rtfTabBar:
		if (fParaFmt.numstops<kMaxstops) {
		fParaFmt.tabkinds[fParaFmt.numstops]= DParagraph::kTabBar;  
		fParaFmt.tabstops[fParaFmt.numstops++]= fParam / 20;  
		}
		break;
		
	case rtfLeaderDot:
		fParaFmt.tablead= '.';
		break;
	case rtfLeaderHyphen:
		fParaFmt.tablead= '-';
		break;
	case rtfLeaderUnder:
		fParaFmt.tablead= '_';
		break;
	case rtfLeaderThick:
		fParaFmt.tablead= '=';
		break;
	case rtfLeaderEqual:
		fParaFmt.tablead= '=';
		break;
		
	case rtfParLevel:
		break;
	case rtfParBullet:
		break;
	case rtfParSimple:
		break;
	case rtfParNumCont:
		break;
	case rtfParNumOnce:
		break;
	case rtfParNumAcross:
		break;
	case rtfParHangIndent:
		break;
	case rtfParNumRestart:
		break;
	case rtfParNumCardinal:
		break;
	case rtfParNumDecimal:
		break;
	case rtfParNumULetter:
		break;
	case rtfParNumURoman:
		break;
	case rtfParNumLLetter:
		break;
	case rtfParNumLRoman:
		break;
	case rtfParNumOrdinal:
		break;
	case rtfParNumOrdinalText:
		break;
	case rtfParNumBold:
		break;
	case rtfParNumItalic:
		break;
	case rtfParNumAllCaps:
		break;
	case rtfParNumSmallCaps:
		break;
	case rtfParNumUnder:
		break;
	case rtfParNumDotUnder:
		break;
	case rtfParNumDbUnder:
		break;
	case rtfParNumNoUnder:
		break;
	case rtfParNumWordUnder:
		break;
	case rtfParNumStrikethru:
		break;
	case rtfParNumForeColor:
		break;
	case rtfParNumFont:
		break;
	case rtfParNumFontSize:
		break;
	case rtfParNumIndent:
		break;
	case rtfParNumSpacing:
		break;
	case rtfParNumInclPrev:
		break;
	case rtfParNumCenter:
		break;
	case rtfParNumLeft:
		break;
	case rtfParNumRight:
		break;
	case rtfParNumStartAt:
		break;

  		
	case rtfBorderTop:
		fParaFmt.bordertop= TRUE;
		break;
	case rtfBorderBottom:
		fParaFmt.borderbottom= TRUE;
		/* no help to missing bordr... fParaFmt.leadHeight = 3; /* depends on style... */
		break;
	case rtfBorderLeft:
		fParaFmt.borderleft= TRUE;
		break;
	case rtfBorderRight:
		fParaFmt.borderright= TRUE;
		break;
	case rtfBorderBetween:
		fParaFmt.borderbetween= TRUE;
		break;
	case rtfBorderBar:
		fParaFmt.borderbar= TRUE;
		break;
	case rtfBorderBox:
		fParaFmt.borderbox= TRUE;
		break;
		
	case rtfBorderSingle:
		fParaFmt.borderstyle = DParagraph::kBorderSingle;
		break;
	case rtfBorderThick:
		fParaFmt.borderstyle = DParagraph::kBorderThick;
		break;
	case rtfBorderShadow:
		fParaFmt.borderstyle = DParagraph::kBorderShadow;
		break;
	case rtfBorderDouble:
		fParaFmt.borderstyle = DParagraph::kBorderDouble;
		break;
	case rtfBorderDot:
		fParaFmt.borderstyle = DParagraph::kBorderDot;
		break;
	case rtfBorderDash:
		fParaFmt.borderstyle = DParagraph::kBorderDash;
		break;
	case rtfBorderHair:
		fParaFmt.borderstyle = DParagraph::kBorderHair;
		break;
	case rtfBorderWidth:
		break;
	case rtfBorderColor:
		break;
	case rtfBorderSpace:
		break;
 
		
	case rtfShading:
		break;
	case rtfBgPatH:
		break;
	case rtfBgPatV:
		break;
	case rtfFwdDiagBgPat:
		break;
	case rtfBwdDiagBgPat:
		break;
	case rtfHatchBgPat:
		break;
	case rtfDiagHatchBgPat:
		break;
	case rtfDarkBgPatH:
		break;
	case rtfDarkBgPatV:
		break;
	case rtfFwdDarkBgPat:
		break;
	case rtfBwdDarkBgPat:
		break;
	case rtfDarkHatchBgPat:
		break;
	case rtfDarkDiagHatchBgPat:
		break;
	case rtfBgPatLineColor:
		break;
	case rtfBgPatColor:
		break;
	}
}


/*
 * Several of the attributes can be turned off with param value
 * of zero (e.g., \b vs. \b0), but since the value of fParam
 * is 0 if no param is given, test the text of the token directly.
 */



void DRTFprocess::CharAttr()
{
	int	turnOn = (rtfTextBuf[rtfTextLen-1] != '0'); 
	//NewStyle(); //x

	switch (fMinor)
	{
	case rtfFontNum:
		{
		RTFFont * rfnt= RTFGetFont (fParam);
		if (rfnt) {
			fStyleRec.fontname= rfnt->rtfFName;
			switch (rfnt->rtfFFamily) {
				case rtfFFRoman : fStyleRec.fontfamily= "Roman"; break;
				case rtfFFSwiss : fStyleRec.fontfamily= "Swiss"; break;
				case rtfFFModern: fStyleRec.fontfamily= "Modern"; break;
				case rtfFFDecor : fStyleRec.fontfamily= "Decorative"; break;
				case rtfFFTech 	:
				case rtfFFNil 	: 
				default					: fStyleRec.fontfamily= NULL; break;
				}
			/* !! add default font size ... msword-rtf often leaves out fsize if == 12 pts ! */
		  /* fStyleRec.fontsize= kDefaultFontsize; */
			}
		NewStyle();
			
		switch (RTFGetCharSet()) {
			default:		 
			case rtfCSGeneral:
				if (fOutMap != gGenCharMap) fOutMap= gGenCharMap;
				break;
			case rtfCSSymbol:
				if (fOutMap != gSymCharMap) fOutMap= gSymCharMap;
				break;
			}
		}
		break;
		
	case rtfFontSize:
		NewStyle();
		/* sizes are in half-points, convert to whole points */
		fStyleRec.fontsize = (int) (fParam / 2);
		if (fStyleRec.fontsize <= 0) fStyleRec.fontsize = kDefaultFontsize;	 /* default size */
		break;

	case rtfPlain:
		NewStyle();
		fStyleRec.style.bold= FALSE;
		fStyleRec.style.italic= FALSE;
		fStyleRec.style.underline= 0;
		fStyleRec.style.hidden= FALSE;
		fStyleRec.style.outline= FALSE;
		fStyleRec.style.strikeout= FALSE;
		fStyleRec.style.shadow= FALSE;
		fStyleRec.style.allcaps= FALSE;
		fStyleRec.style.smallcaps= FALSE;
		fStyleRec.style.superSub= 0; 
		fStyleRec.style.color= kBlackColor;  
		fStyleRec.fontsize= kDefaultFontsize; 	/* ?? is this part of plain ? */
		break;
		
	case rtfBold:
		NewStyle();
		fStyleRec.style.bold= turnOn;
		break;
	case rtfItalic:
		NewStyle();
		fStyleRec.style.italic= turnOn;
		break;
	case rtfUnderline:
		NewStyle();
		if (turnOn) fStyleRec.style.underline = DRichStyle::kUnderline;
		else fStyleRec.style.underline= 0;
		break;
	case rtfWordUnderline:
		NewStyle();
		if (turnOn) fStyleRec.style.underline = DRichStyle::kWorduline;
		else fStyleRec.style.underline= 0;
		break;
	case rtfDotUnderline:
		NewStyle();
		if (turnOn) fStyleRec.style.underline = DRichStyle::kDotuline;
		else fStyleRec.style.underline= 0;
		break;
	case rtfDbUnderline:
		NewStyle();
		if (turnOn) fStyleRec.style.underline = DRichStyle::kDbluline;
		else fStyleRec.style.underline= 0;
		break;
	case rtfNoUnderline:
		NewStyle();
		fStyleRec.style.underline= 0;
		break;
	case rtfInvisible:
		NewStyle();
		fStyleRec.style.hidden= turnOn;
		break;

	case rtfOutline:
		NewStyle();
		fStyleRec.style.outline= turnOn;
		break;
	case rtfAllCaps:
		NewStyle();
		fStyleRec.style.allcaps= turnOn;
		break;
	case rtfSmallCaps:
		NewStyle();
		fStyleRec.style.smallcaps= turnOn;
		break;
	case rtfShadow:
		NewStyle();
		fStyleRec.style.shadow= turnOn;
		break;
	case rtfStrikeThru:
		NewStyle();
		fStyleRec.style.strikeout= turnOn;
		break;
		
	case rtfForeColor:
		{
		RTFColor *rclr;		
		NewStyle();
		rclr= RTFGetColor (fParam);
		if (rclr) {
			/* damn, we now have to select color to read it's nlm value... */
			Nlm_SelectColor( rclr->rtfCRed, rclr->rtfCGreen, rclr->rtfCBlue);
			fStyleRec.style.color= Nlm_GetColor();
			}
		}
		break;
	case rtfBackColor:
		break;

	case rtfSubScript:
		NewStyle();
		fStyleRec.style.superSub=  (int) fParam / 2 ;
		break;
	case rtfSubScrShrink:
		NewStyle();
		fStyleRec.style.superSub=  (int) (fParam/ 2 );  
		break;
	case rtfSuperScript:
		NewStyle();
		fStyleRec.style.superSub=  - (int) (fParam/ 2 ); 
		break;
	case rtfSuperScrShrink:
		NewStyle();
		fStyleRec.style.superSub=  - (int) (fParam/ 2 ); 
		break;
	case rtfNoSuperSub:
		NewStyle();
		fStyleRec.style.superSub= 0;
		break;

		
	case rtfDeleted:
		break;
	case rtfExpand:
		break;
	case rtfExpandTwips:
		break;
	case rtfKerning:
		break;
	case rtfRevised:
		break;
	case rtfRevAuthor:
		break;
	case rtfRevDTTM:
		break;
	case rtfRTLChar:
		break;
	case rtfLTRChar:
		break;
	case rtfCharStyleNum:
		break;
	case rtfCharCharSet:
		break;
	case rtfLanguage:
		SelectLanguage();
		break;
	case rtfGray:
		break;
	}
}


void DRTFprocess::PictAttr()
{

	/* all RTF sizes are in Twentieths of a Point (twip), assume 72 points/inch */
	/* !! assume point == pixel size, bad assumption ?*/
	fInPic= TRUE;
	//x NewStyle();

	switch (fMinor)
	{
	case rtfMacQD:
		fPictKind= DPictStyle::kPictMac;
		break;
	case rtfPMMetafile:
		fPictKind= DPictStyle::kPictPMMeta;
		break;
	case rtfWinMetafile:
		fPictKind= DPictStyle::kPictWinMeta;
		break;
	case rtfDevIndBitmap:
		fPictKind= DPictStyle::kPictDIBits;
		break;
	case rtfWinBitmap:
		fPictKind= DPictStyle::kPictWinBits;
		break;
		
	case rtfPixelBits:
		break;
	case rtfBitmapPlanes:
		break;
	case rtfBitmapWid:
		break;
		
	case rtfPicWid:
		fStyleRec.style.pixwidth= fParam; 
		break;
	case rtfPicHt:
		fStyleRec.style.pixheight= fParam;
		break;
		
	case rtfPicGoalWid:
		break;
	case rtfPicGoalHt:
		break;
	case rtfPicScaleX:
		break;
	case rtfPicScaleY:
		break;
	case rtfPicScaled:
		break;
		
	case rtfPicCropTop:
		break;
	case rtfPicCropBottom:
		break;
	case rtfPicCropLeft:
		break;
	case rtfPicCropRight:
		break;
	case rtfPicMFHasBitmap:
		break;
	case rtfPicMFBitsPerPixel:
		break;
	case rtfPicBinary:
		break;
	}
}


void DRTFprocess::BookmarkAttr()
{
	switch (fMinor)
	{
	case rtfBookmarkFirstCol:
		break;
	case rtfBookmarkLastCol:
		break;
	}
}


void DRTFprocess::NeXTGrAttr()
{
	switch (fMinor)
	{
	case rtfNeXTGWidth:
		break;
	case rtfNeXTGHeight:
		break;
	}
}


void DRTFprocess::FieldAttr()
{
	switch (fMinor)
	{
	case rtfFieldDirty:
		break;
	case rtfFieldEdited:
		break;
	case rtfFieldLocked:
		break;
	case rtfFieldPrivate:
		break;
	case rtfFieldAlt:
		break;
	}
}


void DRTFprocess::TOCAttr()
{
	switch (fMinor)
	{
	case rtfTOCType:
		break;
	case rtfTOCLevel:
		break;
	}
}


void DRTFprocess::PosAttr()
{
	switch (fMinor)
	{
	case rtfAbsWid:
		break;
	case rtfAbsHt:
		break;
	case rtfRPosMargH:
		break;
	case rtfRPosPageH:
		break;
	case rtfRPosColH:
		break;
	case rtfPosX:
		break;
	case rtfPosNegX:
		break;
	case rtfPosXCenter:
		break;
	case rtfPosXInside:
		break;
	case rtfPosXOutSide:
		break;
	case rtfPosXRight:
		break;
	case rtfPosXLeft:
		break;
	case rtfRPosMargV:
		break;
	case rtfRPosPageV:
		break;
	case rtfRPosParaV:
		break;
	case rtfPosY:
		break;
	case rtfPosNegY:
		break;
	case rtfPosYInline:
		break;
	case rtfPosYTop:
		break;
	case rtfPosYCenter:
		break;
	case rtfPosYBottom:
		break;
	case rtfNoWrap:
		break;
	case rtfDistFromTextAll:
		break;
	case rtfDistFromTextX:
		break;
	case rtfDistFromTextY:
		break;
	case rtfTextDistY:
		break;
	case rtfDropCapLines:
		break;
	case rtfDropCapType:
		break;
	}
}


void DRTFprocess::ObjAttr()
{
	switch (fMinor)
	{
	case rtfObjEmb:
		break;
	case rtfObjLink:
		break;
	case rtfObjAutoLink:
		break;
	case rtfObjSubscriber:
		break;
	case rtfObjPublisher:
		break;
	case rtfObjICEmb:
		break;
	case rtfObjLinkSelf:
		break;
	case rtfObjLock:
		break;
	case rtfObjUpdate:
		break;
	case rtfObjHt:
		break;
	case rtfObjWid:
		break;
	case rtfObjSetSize:
		break;
	case rtfObjAlign:
		break;
	case rtfObjTransposeY:
		break;
	case rtfObjCropTop:
		break;
	case rtfObjCropBottom:
		break;
	case rtfObjCropLeft:
		break;
	case rtfObjCropRight:
		break;
	case rtfObjScaleX:
		break;
	case rtfObjScaleY:
		break;
	case rtfObjResRTF:
		break;
	case rtfObjResPict:
		break;
	case rtfObjResBitmap:
		break;
	case rtfObjResText:
		break;
	case rtfObjResMerge:
		break;
	case rtfObjBookmarkPubObj:
		break;
	case rtfObjPubAutoUpdate:
		break;
	}
}


void DRTFprocess::FNoteAttr()
{
	switch (fMinor)
	{
	case rtfFNAlt:
		break;
	}
}


void DRTFprocess::KeyCodeAttr()
{
	switch (fMinor)
	{
	case rtfAltKey:
		break;
	case rtfShiftKey:
		break;
	case rtfControlKey:
		break;
	case rtfFunctionKey:
		break;
	}
}


void DRTFprocess::ACharAttr()
{
	switch (fMinor)
	{
	case rtfACBold:
		break;
	case rtfACAllCaps:
		break;
	case rtfACForeColor:
		break;
	case rtfACSubScript:
		break;
	case rtfACExpand:
		break;
	case rtfACFontNum:
		break;
	case rtfACFontSize:
		break;
	case rtfACItalic:
		break;
	case rtfACLanguage:
		break;
	case rtfACOutline:
		break;
	case rtfACSmallCaps:
		break;
	case rtfACShadow:
		break;
	case rtfACStrikeThru:
		break;
	case rtfACUnderline:
		break;
	case rtfACDotUnderline:
		break;
	case rtfACDbUnderline:
		break;
	case rtfACNoUnderline:
		break;
	case rtfACWordUnderline:
		break;
	case rtfACSuperScript:
		break;
	}
}


void DRTFprocess::FontAttr()
{
	switch (fMinor)
	{
	case rtfFontCharSet:
		break;
	case rtfFontPitch:
		break;
	case rtfFontCodePage:
		break;
	case rtfFTypeNil:
		break;
	case rtfFTypeTrueType:
		break;
	}
}


void DRTFprocess::FileAttr()
{
	switch (fMinor)
	{
	case rtfFileNum:
		break;
	case rtfFileRelPath:
		break;
	case rtfFileOSNum:
		break;
	}
}


void DRTFprocess::FileSource()
{
	switch (fMinor)
	{
	case rtfSrcMacintosh:
		break;
	case rtfSrcDOS:
		break;
	case rtfSrcNTFS:
		break;
	case rtfSrcHPFS:
		break;
	case rtfSrcNetwork:
		break;
	}
}


void DRTFprocess::DrawAttr()
{
	switch (fMinor)
	{
	case rtfDrawLock:
		break;
	case rtfDrawPageRelX:
		break;
	case rtfDrawColumnRelX:
		break;
	case rtfDrawMarginRelX:
		break;
	case rtfDrawPageRelY:
		break;
	case rtfDrawColumnRelY:
		break;
	case rtfDrawMarginRelY:
		break;
	case rtfDrawHeight:
		break;
	case rtfDrawBeginGroup:
		break;
	case rtfDrawGroupCount:
		break;
	case rtfDrawEndGroup:
		break;
	case rtfDrawArc:
		break;
	case rtfDrawCallout:
		break;
	case rtfDrawEllipse:
		break;
	case rtfDrawLine:
		break;
	case rtfDrawPolygon:
		break;
	case rtfDrawPolyLine:
		break;
	case rtfDrawRect:
		break;
	case rtfDrawTextBox:
		break;
	case rtfDrawOffsetX:
		break;
	case rtfDrawSizeX:
		break;
	case rtfDrawOffsetY:
		break;
	case rtfDrawSizeY:
		break;
	case rtfCOAngle:
		break;
	case rtfCOAccentBar:
		break;
	case rtfCOBestFit:
		break;
	case rtfCOBorder:
		break;
	case rtfCOAttachAbsDist:
		break;
	case rtfCOAttachBottom:
		break;
	case rtfCOAttachCenter:
		break;
	case rtfCOAttachTop:
		break;
	case rtfCOLength:
		break;
	case rtfCONegXQuadrant:
		break;
	case rtfCONegYQuadrant:
		break;
	case rtfCOOffset:
		break;
	case rtfCOAttachSmart:
		break;
	case rtfCODoubleLine:
		break;
	case rtfCORightAngle:
		break;
	case rtfCOSingleLine:
		break;
	case rtfCOTripleLine:
		break;
	case rtfDrawTextBoxMargin:
		break;
	case rtfDrawTextBoxText:
		break;
	case rtfDrawRoundRect:
		break;
	case rtfDrawPointX:
		break;
	case rtfDrawPointY:
		break;
	case rtfDrawPolyCount:
		break;
	case rtfDrawArcFlipX:
		break;
	case rtfDrawArcFlipY:
		break;
	case rtfDrawLineBlue:
		break;
	case rtfDrawLineGreen:
		break;
	case rtfDrawLineRed:
		break;
	case rtfDrawLinePalette:
		break;
	case rtfDrawLineDashDot:
		break;
	case rtfDrawLineDashDotDot:
		break;
	case rtfDrawLineDash:
		break;
	case rtfDrawLineDot:
		break;
	case rtfDrawLineGray:
		break;
	case rtfDrawLineHollow:
		break;
	case rtfDrawLineSolid:
		break;
	case rtfDrawLineWidth:
		break;
	case rtfDrawHollowEndArrow:
		break;
	case rtfDrawEndArrowLength:
		break;
	case rtfDrawSolidEndArrow:
		break;
	case rtfDrawEndArrowWidth:
		break;
	case rtfDrawHollowStartArrow:
		break;
	case rtfDrawStartArrowLength:
		break;
	case rtfDrawSolidStartArrow:
		break;
	case rtfDrawStartArrowWidth:
		break;
	case rtfDrawBgFillBlue:
		break;
	case rtfDrawBgFillGreen:
		break;
	case rtfDrawBgFillRed:
		break;
	case rtfDrawBgFillPalette:
		break;
	case rtfDrawBgFillGray:
		break;
	case rtfDrawFgFillBlue:
		break;
	case rtfDrawFgFillGreen:
		break;
	case rtfDrawFgFillRed:
		break;
	case rtfDrawFgFillPalette:
		break;
	case rtfDrawFgFillGray:
		break;
	case rtfDrawFillPatIndex:
		break;
	case rtfDrawShadow:
		break;
	case rtfDrawShadowXOffset:
		break;
	case rtfDrawShadowYOffset:
		break;
	}
}


void DRTFprocess::IndexAttr()
{
	switch (fMinor)
	{
	case rtfIndexNumber:
		break;
	case rtfIndexBold:
		break;
	case rtfIndexItalic:
		break;
	}
}


void DRTFprocess::SelectLanguage()
{
	switch (fParam)
	{
	case rtfLangNoLang:
		break;
	case rtfLangAlbanian:
		break;
	case rtfLangArabic:
		break;
	case rtfLangBahasa:
		break;
	case rtfLangBelgianDutch:
		break;
	case rtfLangBelgianFrench:
		break;
	case rtfLangBrazilianPortuguese:
		break;
	case rtfLangBulgarian:
		break;
	case rtfLangCatalan:
		break;
	case rtfLangLatinCroatoSerbian:
		break;
	case rtfLangCzech:
		break;
	case rtfLangDanish:
		break;
	case rtfLangDutch:
		break;
	case rtfLangAustralianEnglish:
		break;
	case rtfLangUKEnglish:
		break;
	case rtfLangUSEnglish:
		break;
	case rtfLangFinnish:
		break;
	case rtfLangFrench:
		break;
	case rtfLangCanadianFrench:
		break;
	case rtfLangGerman:
		break;
	case rtfLangGreek:
		break;
	case rtfLangHebrew:
		break;
	case rtfLangHungarian:
		break;
	case rtfLangIcelandic:
		break;
	case rtfLangItalian:
		break;
	case rtfLangJapanese:
		break;
	case rtfLangKorean:
		break;
	case rtfLangBokmalNorwegian:
		break;
	case rtfLangNynorskNorwegian:
		break;
	case rtfLangPolish:
		break;
	case rtfLangPortuguese:
		break;
	case rtfLangRhaetoRomanic:
		break;
	case rtfLangRomanian:
		break;
	case rtfLangRussian:
		break;
	case rtfLangCyrillicSerboCroatian:
		break;
	case rtfLangSimplifiedChinese:
		break;
	case rtfLangSlovak:
		break;
	case rtfLangCastilianSpanish:
		break;
	case rtfLangMexicanSpanish:
		break;
	case rtfLangSwedish:
		break;
	case rtfLangSwissFrench:
		break;
	case rtfLangSwissGerman:
		break;
	case rtfLangSwissItalian:
		break;
	case rtfLangThai:
		break;
	case rtfLangTraditionalChinese:
		break;
	case rtfLangTurkish:
		break;
	case rtfLangUrdu:
		break;
	}
}

