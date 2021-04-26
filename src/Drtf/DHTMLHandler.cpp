// DHTMLHandler.cpp
// d.g.gilbert


#include "Dvibrant.h"
#include "DFile.h"
#include "DRichProcess.h"
#include "DRichViewNu.h"
#include "DHTMLHandler.h"
#include "DHTMLprocess.h"
#include "rtfchars.h" 
#include "DRichMoreStyle.h"


	
class DHTMLsetup  
{
public:
	DHTMLsetup();
};

static DHTMLsetup* gHTMLsetup = NULL;
#if 0
static DHTMLsetup theSetup; // construct it now !
#endif

DHTMLsetup::DHTMLsetup()
{
	DHTMLprocess::InitKeys(); 
	//ReadOutputMap("gen", gGenCharMap);
  //gOutMap= gGenCharMap;
	////
	//DRichprocess::gGenCharMap= DRichprocess::ReadOutputMap("gen");
	//DRichprocess::gSymCharMap= DRichprocess::ReadOutputMap("sym");
	gHTMLsetup = this;
}






//class  DHTMLHandler


char* DHTMLHandler::IsRich(char* buf, ulong buflen)
{
// HTML doesn't have a fixed magic string -- usage is variable
// some diagnostic strings (all case-insensitive)
// </a> </head>
// <html>  <head>  <body>  <title> 
// uncertain: </ <a
//	href=

	char* cp, *atbuf= buf;
	if (buflen==0) buflen= StrLen(buf);
	ulong len = buflen;
	while (len>3) {
		cp= (char*) MemChr(atbuf,'<',len);
		if (cp) {
			if (cp[1] == '/' && (cp[2] == 'a' || cp[2] == 'A') && cp[3] == '>') 
				return buf;
			else if (!StrNICmp( cp, "<html>", 6)) return buf;
			else if (!StrNICmp( cp, "<head>", 6)) return buf;
			else if (!StrNICmp( cp, "<title>", 7)) return buf;
			else if (!StrNICmp( cp, "<p>", 3)) return buf;   // ?? weak test but need
			else if (!StrNICmp( cp, "<a href=", 9)) return buf;
			//if (!StrNICmp( cp, "<a ", 3)) {  
			//  unsigned long	at;
			//	if (Nlm_StrngPos( cp, "</a>", 3, false, &at)) return true;
			//	}
			cp++;
			len -= (cp - atbuf);
			atbuf = cp;
			}
		else 
			return NULL;	
		}
	return NULL;
}

			

DHTMLHandler::DHTMLHandler( DRichView* itsDoc, DFile* savefile) :
	DRichHandler( itsDoc, savefile)
{
	fFormatName = "HTML";
	fMinDataToProcess= 128; // no fixed min, need to look at data...
	if (!gHTMLsetup) gHTMLsetup= new DHTMLsetup();
}

DHTMLHandler::~DHTMLHandler()
{
}

	
DRichprocess* DHTMLHandler::NewProcessor( DFile* itsFile, Nlm_MonitorPtr progress)
{
	fProcessor = new DHTMLprocess( itsFile, fDoc, progress);
	return fProcessor;
}


Boolean DHTMLHandler::ProcessData( char* cbeg, char* cend, Boolean endOfData, 
												ulong& dataRemaining) 
{
	char  * cp;  

	fcsave= 0;	
	fclen= cend - cbeg; // + 1;
	cp= cend; 
	if (!endOfData) {
		if ((fclen==0) || ( fProcessStage == kAtStart && fclen<fMinDataToProcess)) 
			return false;
		// scan for last newline and stop there !!
#if 1 //TEST w/out << NEED THIS for at least some
		while (cp - cbeg > fLastScanto && !(*cp == '\n' || *cp == '\r')) cp--;
		if (cp < cend && (*cp == '\n' || *cp == '\r')) cp++;
#endif
		}

#if 1 //TEST w/out << NEED THIS for at least some
	if (cp - cbeg <= fLastScanto) {
		fLastScanto= cend - cbeg;  
		if (!endOfData) return false; // !??! don't process line w/o end
		else cp= cend; 
		}
#endif

#if 0 // bad nulls ??
	if (*cp) cp++;
	fcsave= *cp; 
	*cp= 0;
#endif

	fclen= cp - cbeg; // + 1; 
	
	if (fclen && fDocFile) {
		if (cbeg[fclen-1] == 0) fclen--;  //?? 
		fDocFile->WriteData( cbeg, fclen);
		}	
		
	return this->ProcessData2( cp, cbeg, cend, endOfData, dataRemaining);
}





// class DHTMLprocess



//static char		gGenCharMap[rtfSC_MaxChar];
//static char * gOutMap = gGenCharMap;

DHTMLprocess::DHTMLprocess( DFile* itsFile, DRichView* itsDoc, Nlm_MonitorPtr progress) :
	DRichprocess( itsFile, itsDoc, progress),
	fStop(0), fTokenLen(0), fListType(0), fListNum(0), fDLStyle(0), fQuote(0),
	fCurControl(0), fCurField(0), fLastField(0), fPicLink(0), fAnchorLink(0),
	fURLstore(NULL), fNamestore(NULL), fFormURL(NULL), fGoplusStore(NULL),
	fMethodstore(NULL), fValstore(NULL), fTypestore(NULL), fOptionstore(NULL),
	fSizestore(NULL),fMaxlengthstore(NULL),fRowsstore(NULL),fColsstore(NULL),
	fPushSize(0), fPushBuf(NULL), fPushBufstore(NULL),
	fInHead(true), fInParam(false), fInForm(false), fInControl(false), fIsMap(false),
	fIsChecked(false), fSelectitem(0), fCurItem(0), fIsMultiple(false),
	fTurnOn(true), fIsIndex(false), fPreformat(false)
{ 
	//fOutMap= DRichprocess::gGenCharMap;
}



	
DHTMLprocess::~DHTMLprocess()
{
	MemFree(fURLstore);
	MemFree(fNamestore);
	MemFree(fMethodstore);
	MemFree(fGoplusStore);
	MemFree(fFormURL);
	MemFree(fValstore);
	MemFree(fTypestore);
	MemFree(fOptionstore);
	MemFree(fPushBufstore);
	MemFree(fSizestore);
	MemFree(fMaxlengthstore);
	MemFree(fRowsstore);
	MemFree(fColsstore);
}


//static
short DHTMLprocess::Hash( char	*s)
{
	char	c;
	short	val = 0;
	while ((c = *s++) != '\0') val += (int) tolower(c);
	return (val);
}

//static
void DHTMLprocess::InitKeys()
{
	HTMLKey	*rp;
	
	for (rp = htmlKeys; rp->htmlKStr != NULL; rp++) {
		rp->htmlKHash = Hash(rp->htmlKStr);
		}
	for (rp = htmlAmperChars; rp->htmlKStr != NULL; rp++) {
		rp->htmlKHash = Hash(rp->htmlKStr);
		}
}


void DHTMLprocess::Lookup( char	*s, HTMLKey* keyset)
{
	HTMLKey	*rp;
	short	hash;
	
#ifdef COMP_BOR  /* ?? dgg */  
	int LIBCALL (*cmp)(const char*,const char*);
#else
#ifdef COMP_SYMC
  int (* LIBCALL cmp)(const char*, const char*);
#else
	int (*cmp)(const char*,const char*);
#endif
#endif
	if (keyset == htmlAmperChars) cmp= Nlm_StringCmp;
	else cmp= Nlm_StringICmp;
	
	fClass = tokUnknown;
	if (*s == 0) return;
	hash = Hash(s);
	for (rp = keyset; rp->htmlKStr != NULL; rp++) {
		if (hash == rp->htmlKHash && cmp(s, rp->htmlKStr) == 0) {
			fClass = tokControl;
			fMajor = rp->htmlKMajor;
			fMinor = rp->htmlKMinor;
			fStop  = rp->htmlKStop;
			return;
		}
	}
}




void DHTMLprocess::Pushback(char c) // private
{
	if (c != EOF) {
		fPushedChar = c;
		}
}



enum {
	kGotNone = 0,
	kGotToken,
	kGotParam
	};
	
short DHTMLprocess::GotTokenOrParam() // private
{
	if (fTokenLen>0) { // need to process last token ! 
		fTokenBuf[fTokenLen] = '\0';
		long i, tokenlen= fTokenLen;
		fTokenLen= 0;
		if (fInParam) {
			switch (fCurField) {
			
				case htmlIsMap:
					fIsMap= true;
					break;
					
				case htmlAction:
				case htmlImageSrc:
				case htmlHref:
					for (i=0; i<tokenlen; i++) if (fTokenBuf[i]<' ') fTokenBuf[i]= ' ';
					MemFree(fURLstore);
					fURLstore= StrDup(fTokenBuf);
					break;

				//case htmlInput:
				//case htmlSelect:
				//case htmlTextarea:
				
					// form tags -- stuff data where ??
				case htmlTitle:
				case htmlName:
					MemFree(fNamestore);
					fNamestore= StrDup(fTokenBuf);
					break;
  
				case htmlMethod:
					MemFree(fMethodstore);
					fMethodstore= StrDup(fTokenBuf);
					break;
				case htmlValue:
					MemFree(fValstore);
					fValstore= StrDup(fTokenBuf);
					if (fLastField==htmlOption) fCurField= fLastField;
					break;
				case htmlType:
					MemFree(fTypestore);
					fTypestore= StrDup(fTokenBuf);
					break;
				
				case htmlChecked:
					fIsChecked= true;
					break;
				case htmlSelected:
					fSelectitem= fCurItem;
					break;
				case htmlMultiple:
					fIsMultiple= true;
					break;
				case htmlSize:
					MemFree(fSizestore);
					fSizestore= StrDup(fTokenBuf);
					break;
				case htmlMaxlength:
					MemFree(fMaxlengthstore);
					fMaxlengthstore= StrDup(fTokenBuf);
					break;
				case htmlRows:
					MemFree(fRowsstore);
					fRowsstore= StrDup(fTokenBuf);
					break;
				case htmlCols:
					MemFree(fColsstore);
					fColsstore= StrDup(fTokenBuf);
					break;
					
				case htmlOption:
					fTokenBuf[tokenlen++]= '\t';
					fTokenBuf[tokenlen]= '\0';
					if (!fOptionstore) fOptionstore= StrDup(fTokenBuf);
					else StrExtendCat( &fOptionstore, fTokenBuf);
							// need to collect BOTH option text and values !
					if (fValstore) {
						tokenlen= StrLen( fValstore);
						StrCpy( fTokenBuf, fValstore);
						}
					else
						tokenlen= 0;
					fTokenBuf[tokenlen++]= '\t';
					fTokenBuf[tokenlen]= '\0';
					if (!fRowsstore) fRowsstore= StrDup(fTokenBuf);
					else StrExtendCat( &fRowsstore, fTokenBuf);
					break;

				}
			fInParam= false;
			return kGotParam;
			}
		else {
			Lookup( fTokenBuf, htmlKeys);	 // sets class, major, minor 
			return kGotToken;
			}
		}
	else
		return kGotNone;
}



void DHTMLprocess::GetToken1() // private
{
	short	c;

	fClass = tokUnknown;   // initialize token vars  
	fParam = tokNoParam;

	if (fPushedChar != EOF) {
		c = fPushedChar;
		fPushedChar = EOF;
		}
	else 
		c= GetOneChar();
		 
	switch ( c ) {
	
		case EOF:
			fClass = tokEOF;
			fMajor = tokEOF;
			return;
			
		case tokBreakInData:
			fClass= tokEOF;
			fMajor= tokBreakInData;
			return;
							
		case '>':
			{
			if (fInControl && fQuote) goto cInQuote;
			if (!fInControl) goto cHandleText;
			if (GotTokenOrParam() != kGotNone) {
				Pushback(c);
				return;
				}
			if (fCurField == htmlOption) { // fCurControl == htmlSelect && 
				Pushback('='); // turn following text into option Param...
				return; 
				}
			fInControl= false;
			fClass = tokControl;
			fMajor = htmlEndControl;
			fMinor = -1;
			return;
			}
		
		case '<':
			if (fInControl && fQuote) goto cInQuote;
			if (fQuote == (char)234) { // kInFormText
				fInControl= false;
				this->handleLinkAttr( fCurControl);  
				}
			fInControl= true;
			fTurnOn= true;
			fInParam= false;
			fQuote= 0;
			fCurControl= 0;
			fCurField= 0;
			fTokenLen= 0;
			fPicLink= 0;
			//fAnchorLink= 0;
			fClass = tokDropchar;
			return;

		default: 
		
			if (fInControl) {
cInQuote:					
				// !! need to deal with complex  options in <Control ... > syntax
				// e.g. <a href="some.string" name="some.string" etc. > text here </a>
				// e.g. < IMG SRC ="triangle.gif" ALT="Warning:"> text here...
			  // < A HREF="Go">< IMG SRC ="Button"> Press to start</A>
				
			  if ( (fQuote && c != fQuote)  
			 	 || (fInParam && c != fQuote && c != '"' && c != '\'' && c > ' ') 
			   || isalnum(c) ) {
			  	// store in token buf until unquoted
					if (fTokenLen<kMaxTokenBuf) fTokenBuf[fTokenLen++] = c;
					if (fQuote == (char)234) { // kInFormText
						goto labelNoControl; // also stuff c into texthandler stream
						}
					else
						fClass = tokDropchar;
					return; 
			  	}

				if (GotTokenOrParam() == kGotToken) {
				  Pushback( c);  // ! need to handle terminator char
				  return;  
					}
					
				if (fQuote && c == fQuote) {
					// must be sure we handle unquote ! after storing param
					fQuote = 0;
					fClass = tokDropchar;
					return; 
					}

				switch ( c ) {
					case '/': fTurnOn= false; break;
					case '=': fInParam= true; break;
					case '"':  
					case '\'': 
							// ?? these may be good only if (fInParam) ???
						if (fQuote) fQuote= 0; // end of quote..
						else fQuote= c; 
						break;
					
					default:
						//if (isspace(c))  break; 
						break;
					}
					
				fClass = tokDropchar;
				return; 
		 		}
		 		
		 	else {
labelNoControl:
		 		switch ( c ) {

				case '&':
					//wordat= fTokenBuf+fTokenLen;
					fTokenLen= 0;
					do { 
						c = GetOneChar(); 
						if (fTokenLen<kMaxTokenBuf) fTokenBuf[fTokenLen++] = c;
					} while (c > EOF && isalnum(c));
					fTokenBuf[fTokenLen] = '\0';
					Lookup( fTokenBuf, htmlAmperChars);	 // sets class, major, minor 
					return;

				case '\t':
					fClass = tokControl;
					fMajor = htmlSpecialChar;
					fMinor = htmlTab;
					return;
					
				case '\r': 
				case '\n': //NEWLINE:
					if (fPreformat) {
						fClass = tokControl;
						fMajor = htmlSpecialChar;
						fMinor = htmlNewline;
						}
					else if (fTextSize>0) { 
							// messy fix for that cutesy horizontal bar 
						if ( fTextSize > 15 && fText[15] == '_'
							  && fText[1] == '_' && fText[fTextSize-2] == '_') {
							fTextSize= 0;
							handleSpecialChar( htmlHBar);	
							fClass = tokDropchar;
							}
						else 
							// put space in, if this isn't new parag
							goto cHandleSpace;
						}	
					else
						fClass = tokDropchar;
					return;
				
				case ' ':
				cHandleSpace:
					if (fPreformat || fLastChar != ' ') {
						fClass = tokText;
						fMajor = ' ';
						fMinor = ' '; // MapChar( c);
						}
					else
						fClass = tokDropchar;
					return;
					
				case '\0':
					fClass = tokDropchar;
					return;
					
				default:
				cHandleText:
					fClass = tokText;
					fMajor = c;
					fMinor = c; // MapChar( c);
					return;
			 	}
			 }
		 	}
}









short DHTMLprocess::GetOneChar()
{
#if 1
	if (fPushSize && fPushBuf) {
		short c= (unsigned char) *fPushBuf; // must be unsigned !!
		fPushSize--;
		fPushBuf++;
		return c;
		}
	else
		return DRichprocess::GetOneChar();
#else
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
#endif
}


 

 


void DHTMLprocess::handleTextClass()
{
	if (IsNewStyle()) StoreStyle(fOldStyleRec.style, FALSE);  
	
	if (fMinor != rtfSC_nothing)
		PutStdChar(fMinor); 
	else {
		char	buf[128];
		if (fMajor < 128)	/* in ASCII range */
			sprintf (buf, "[[%c]]", fMajor);
		else
			sprintf (buf, "[[\\'%02x]]", fMajor);
		PutLitStr (buf);
		}
	fLastChar= fMajor;
}


void DHTMLprocess::handleControlClass()
{
	switch (fMajor) {
		case htmlDocAttr	: handleDocAttr(fMinor); break;
		case htmlLinkAttr	: handleLinkAttr(fMinor); break;
		case htmlFormAttr	: handleFormAttr(fMinor); break;
		case htmlCharAttr	: handleCharAttr(fMinor); break;
		case htmlParAttr	: handleParAttr(fMinor); break;
		case htmlListAttr	: handleListAttr(fMinor); break;
		case htmlPictAttr	: handlePictAttr(fMinor); break;
		case htmlSpecialChar: handleSpecialChar(fMinor); break;
		case htmlEndControl: handleEndControl(fMinor); break;
		}
}



void DHTMLprocess::handleEndControl(short attr)
{
	// e.g, > end of any control
 
	if (fCurControl == htmlForm
	 || fCurControl == htmlInput 
	 //|| fCurControl == htmlSelect 
	 //|| fCurControl == htmlTextarea
	 ) {
		// call here AFTER DNetHTMLprocess::handleEndControl
		this->handleFormAttr( fCurControl);  //?? !? why this mess here
		}
		
	else if (fCurControl == htmlImage) {
		short kind;
		char * dummypict = (char*) MemNew( 1);
		*dummypict= 0;
		if (fAnchorLink) kind= DPictStyle::kPictNetLink;
		else kind= DPictStyle::kPictNetPic;
		DPictStyle* thePic= new DPictStyle( kind, dummypict, 1, true);   

		if (!fParaCount) NewParagraph();
			
		if (fPicLink) {
			Nlm_RecT loc;
			Nlm_LoadRect( &loc, 0, 0, 35, 22); // top is to fetch picture
			thePic->AddLink( DPictStyle::kPictNetPic, fPicLink, loc);
			if (fAnchorLink) {
				Nlm_LoadRect( &loc, 0, 23, 35, 35); // bottom is to fetch link info
				thePic->AddLink( DPictStyle::kPictNetLink, fAnchorLink, loc);
				}
			fPicLink= 0;
			}
			
			// !? in html, is damn url suffix supposed indicate TYPE of data !?
			// or can we assume all these are GIF pictures ?
		fStyleRec.style.ismap= fIsMap;
		StoreStyleObject( thePic, 35, 35);
		}

	fCurControl= 0;
	fCurField= 0;
	fIsMap= false;
}






void DHTMLprocess::handleDocAttr(short attr)
{
	switch (attr) {
	
		case htmlHTML: 	
			if (fTurnOn) fCurControl= attr;
			break;
		case htmlHead:	fInHead= fTurnOn; break;
		case htmlBody:  fInHead= false; break;

		case htmlTitle:  	
			// ! this htmlTitle can occur both in Head and Body/Anchor
			if (fTurnOn) {
				fCurControl= attr; //??
				fNamestore= (char*) MemFree(fNamestore);  
				}
			else {
#if 0
				fTitle= fNamestore;
				fNamestore= NULL;
#else				
				if (fTextSize) { // fInHead &&
					MemFree(fTitle);  
					fText[fTextSize]= '\0';
					fTitle= StrDup( fText); 
					fTextSize= 0;
					}
#endif
				}
			break;
			
		case htmlIsIndex: {
			fIsIndex= true;
			MemFree(fPushBufstore);
			fPushBufstore= StrDup(" <A HREF=?>Click here to query</A> ");
			fPushBuf= fPushBufstore;
			fPushSize= StrLen(fPushBuf);
			//MemFree(fURLstore);
			//fURLstore= StrDup("#");
			// entire doc is somehow flagged as a query document, and need
			// some kind of button to send query to server from this doc !?
			// go->fType= kTypeQuery;
			}
			break;
			
		case htmlBase: 
			break;

		case htmlLink: 
		case htmlNextID: 
			break;
			
		//case htmlHref: // !! bug in processing tokens -- arrive here after finding Link/HREF !
		}
}



#if 0
///////////  html forms stuff ///////////////
///// convert to gopher ask forms ///////////

<form  action="partial-url-to-handle-data" method=get or post> 

<input name="variable_name"> // one line text input (type=text)
<input type=text name="variable_name" value='default text'> // one line text input
	>> Ask: variable_name:\t default text

<input type=password name="variable_name-name"> // one line password text input
	>> AskP: variable_name
	
<input type=submit value='button title'> // button to send form
	>> default ask form "okay" button
<input type=reset value='button title'>  // button to clear current choices
	>> no ask equivalent
	
<input type=checkbox name=variable_name value="box title" checked>
	>> Select: box title:0 or 1
	
<input type=radio name=var_name value='title 1'>
<input type=radio name=var_name value='title 2'>
	>> Choose: var_name: \t title 1 \t title 2 ...
	
<textarea name=var_name cols=40 rows=2>  // text input box
default text
</textarea>
  >> AskL: var_name \t default text
  
<select>
<option selected> name1
<option> name2
<option value=n3> name2
</select>
	>> Choose: var_name: \t name1 \t name2 ...


</form>

		// forms tags
  htmlLinkAttr,	htmlForm, "form", kSlashStop, 0,
	  htmlLinkAttr,	htmlInput, "input", kNoStop, 0,
	  htmlLinkAttr,	htmlSelect, "select", kSlashStop, 0,
		htmlLinkAttr,	htmlTextarea, "textarea", kSlashStop, 0,
		htmlLinkAttr,	htmlValue, "value", kNoStop, 0,
		htmlLinkAttr,	htmlType, "type", kNoStop, 0,
		htmlLinkAttr,	htmlOption, "option", kNoStop, 0,
		htmlLinkAttr,	htmlAction, "action", kNoStop, 0,
		htmlLinkAttr,	htmlMethod, "method", kNoStop, 0,

#endif




void DHTMLprocess::handleFormAttr(short attr)
{
	char buf[512];
	DWindow* fWin;
	DControlStyle* cs;
	
	switch (attr) {
	
		case htmlForm: 
			fCurControl= attr;
			fCurField= attr;
			fInForm= fTurnOn;
			if (fInForm) {
				if (fInControl) {
					fURLstore= (char*) MemFree(fURLstore);  
					fMethodstore= (char*) MemFree(fMethodstore);  
					fFormURL= (char*) MemFree(fFormURL);
					//fGoplusStore= (char*) MemFree(fGoplusStore);
					//fGoplusStore= StrDup("+ASKHTML:\n");  
					PushStyle();
					fIsChecked= fIsMultiple= false; fSelectitem= 0;
					}
				else {
					// <FORM METHOD=get/post/localexec ACTION="proto://path/function opts">  
					cs = NULL;
					fWin= fDoc->GetWindow();
					cs= new DHiddenCStyle( fWin); 
					if (!fMethodstore) cs->ControlData( "GET", fURLstore);  
					else cs->ControlData( fMethodstore, fURLstore);  
					cs->Install();
					StoreStyleObject( cs, cs->Width(), cs->Height());
					}
				}
			else {
				//fStyleRec.style.color= 0; // ?? 
				//fStyleRec.style.linkid= 0;				
				//PopStyle();
				}
			break;

//   need to collect other Forms control tags:
// input: TYPE= NAME= VALUE= CHECKED  SIZE=  MAXLENGTH=
// option: VALUE= (or value after ">")  SELECTED
// select: NAME=  SIZE= (#items to show)  MULTIPLE
// textarea: NAME=  COLS= ROWS=  

		case htmlInput:
			if (fInControl) {
			  fCurControl= attr; //?? 
				fCurField= attr;
				fTypestore= (char*) MemFree(fTypestore);  
				fNamestore= (char*) MemFree(fNamestore);  
				fMethodstore= (char*) MemFree(fMethodstore);  
				fOptionstore= (char*) MemFree(fOptionstore);  
				fValstore= (char*) MemFree(fValstore);  
				fSizestore= (char*) MemFree(fSizestore);  
				fMaxlengthstore= (char*) MemFree(fMaxlengthstore);  
				fRowsstore= (char*) MemFree(fRowsstore);  
				fColsstore= (char*) MemFree(fColsstore);  
				fIsChecked= fIsMultiple= false; fSelectitem= 0;
				}
				
			else {
			// this are HandleEndControl, e.g., at "...>", methods, 
			// not !fInControl start methods (e.g., at "</control..." 
			
				cs = NULL;
				fWin= fDoc->GetWindow();
				
				if (!fTypestore || StrICmp( fTypestore, "text")==0) {
					// <input type=text name="variable_name" value='default text' size=10>
					cs= new DEditTextCStyle( fWin); 
					if (fSizestore) cs->fWidth= atol(fSizestore);
					else if (fMaxlengthstore) cs->fWidth= atol(fMaxlengthstore);
					cs->ControlData( fNamestore, fValstore); // installs in view !?
					}

				else if (StrICmp( fTypestore, "password")==0) {
					// <input type=password name="variable_name">  
					cs= new DPasswordCStyle( fWin); 
					if (fSizestore) cs->fWidth= atol(fSizestore);
					else if (fMaxlengthstore) cs->fWidth= atol(fMaxlengthstore);
					cs->ControlData( fNamestore, fValstore); // installs in view !?
					}

				else if (StrICmp( fTypestore, "hidden")==0) {
					// <input type=hidden name="variable_name" value="whatever">  
					cs= new DHiddenCStyle( fWin); 
					cs->ControlData( fNamestore, fValstore); 
					}

				else if (StrICmp( fTypestore, "submit")==0) {
					// <input type=submit value='button title'> // button to send form
					cs= new DDefaultButtonCStyle( fWin); 
					if (!fValstore) cs->ControlData( fNamestore, "Submit"); 
					else cs->ControlData( fNamestore, fValstore); 
					}
					
				else if (StrICmp( fTypestore, "reset")==0) {
					// <input type=reset value='button title'>  // button to clear current choices
					cs= new DButtonCStyle( fWin); 
					if (!fValstore) cs->ControlData( fNamestore, "Reset"); 
					else cs->ControlData( fNamestore, fValstore); 
					}

				else if (StrICmp( fTypestore, "checkbox")==0) {
					//<input type=checkbox name=variable_name value="box title" checked>
					cs= new DCheckboxCStyle( fWin); 
					cs->fSelected= fIsChecked;
					if (!fValstore)  cs->ControlData( fNamestore, "on");  
					else cs->ControlData( fNamestore, fValstore); 
					}
					
				else if (StrICmp( fTypestore, "radio")==0) {
					//<input type=radio name=var_name value='title 2'>
					cs= new DRadioCStyle( fWin); 
					cs->fSelected= fIsChecked;
					if (!fValstore)  cs->ControlData( fNamestore, "on");  
					else cs->ControlData( fNamestore, fValstore); 
					}
					
				else if (StrICmp( fTypestore, "file")==0) {
					//<input type=file name=default_file_name value='anything'>
					cs= new DFileCStyle( fWin); 
					if (!fValstore)  cs->ControlData( fNamestore, "default-file"); 
					else cs->ControlData( fNamestore, fValstore); 
					}
					
				if (cs) {
					cs->Install();
					StoreStyleObject( cs, cs->Width(), cs->Height());
					}
				

				}
			break;


		case htmlSelect: 
				//<select name="myname">
				//<option selected> name1
				//<option> name2
				//<option value=n3> name3
				//</select>
			if (fTurnOn) { // fInControl
			  fCurControl= attr; //?? 
				fCurField= attr;
				fCurItem= 0;
				fLastField= 0;
				//fSomething= fTurnOn;
				fNamestore= (char*) MemFree(fNamestore); 
				fOptionstore= (char*) MemFree(fOptionstore); 
				fValstore= (char*) MemFree(fValstore);  
				fSizestore= (char*) MemFree(fSizestore);  
				fMaxlengthstore= (char*) MemFree(fMaxlengthstore);  
				fRowsstore= (char*) MemFree(fRowsstore);  
				fColsstore= (char*) MemFree(fColsstore);  
				fIsChecked= fIsMultiple= false; fSelectitem= 0;
				}
			else {
				fLastField= 0;
				cs = NULL;
				fWin= fDoc->GetWindow();
				cs= new DPopupCStyle( fWin); 
					// ! need to options == popup selections -> from fOptionstore
				cs->fSelected= fSelectitem;
				cs->ControlData( fNamestore, fOptionstore, fRowsstore); //? fOption not fValstore
				cs->Install();
				StoreStyleObject( cs, cs->Width(), cs->Height());
				}
			break;
			
		case htmlOption: 
			if (fInControl) {
				fCurField= attr;
				fCurItem++;
				fValstore= (char*) MemFree(fValstore);  
				}
			else {
				fLastField= 0;
				}
			break;


		case htmlTextarea: 
				//<textarea name=var_name cols=40 rows=2>  
				//default text
				//</textarea>
			if (fTurnOn) { // fInControl)
			  fCurControl= attr; //?? 
				fCurField= attr;
				//fSomething= fTurnOn;
				fNamestore= (char*) MemFree(fNamestore);  
				fValstore= (char*) MemFree(fValstore);  
				fSizestore= (char*) MemFree(fSizestore);  
				fMaxlengthstore= (char*) MemFree(fMaxlengthstore);  
				fRowsstore= (char*) MemFree(fRowsstore);  
				fColsstore= (char*) MemFree(fColsstore);  
				fIsChecked=  fIsMultiple= false; fSelectitem= 0;
				}
			else {
				cs = NULL;
				fWin= fDoc->GetWindow();
				cs= new DDialogTextCStyle( fWin); 
				cs->ControlData( fNamestore, fValstore); 
				if (fColsstore) cs->fWidth= atol(fColsstore); // # chars wide
				if (fRowsstore) cs->fHeight= atol(fRowsstore); // # chars tall
				cs->Install();
				StoreStyleObject( cs, cs->Width(), cs->Height());
  			}
			break;

			
		case htmlMethod:
			fCurField= attr;
			// this is GET or POST -- only those two?
			break;
			
		case htmlAction:
			fCurField= attr;
			fURLstore= (char*) MemFree(fURLstore);  
			break;
			
		case htmlChecked:
			fIsChecked= true;
			fCurField= attr;
			break;
		case htmlSelected:
			fSelectitem= fCurItem;
			//if (fCurField!=htmlOption) fCurField= attr;
			break;
		case htmlMultiple:
			fIsMultiple= true;
			fCurField= attr;
			break;
			
		case htmlSize:
		case htmlMaxlength:
		case htmlRows:
		case htmlCols:
		// case htmlName: // handled		
		case htmlValue:
			//if (fCurField!=htmlOption) 
			fLastField= fCurField;
			fCurField= attr;
			break;

			
		case htmlType:
 			// record param
			fCurField= attr;
			fTypestore= (char*) MemFree(fTypestore);  
			break;

		}
}



void DHTMLprocess::handleLinkAttr(short attr)
{
	char buf[512];
	
	switch (attr) {
	
		case htmlAnchor: 
			if (fTurnOn) {
				fAnchorLink= 0;
				fCurControl= attr;
				// DAMN, need to push/pop something here -- having style changes inside
				// anchor causes it to disappear now !!!
				PushStyle();
				//PutLitCharWithStyle('['); // [] for debugging
				}
			else {
				fStyleRec.style.color= 0; // ?? 
				fStyleRec.style.linkid= 0;				
				//PutLitCharWithStyle(']'); // [] for debugging
				PopStyle();
				fAnchorLink= 0;
				}
			break;

		case htmlHref: 
			fCurField= attr;
			// store: ="url://bob.was/here"
			fURLstore= (char*) MemFree(fURLstore);  
			break;

		case htmlName: 
			fCurField= attr;
			// store: ="some name"
			fNamestore= (char*) MemFree(fNamestore);  
			break;
			
		case htmlRel: 
			fCurField= attr;
			//fNamestore= (char*) MemFree(fNamestore);  
			break;
		case htmlRev: 
			fCurField= attr;
			//fNamestore= (char*) MemFree(fNamestore);  
			break;
		case htmlUrn: 
			fCurField= attr;
			//fNamestore= (char*) MemFree(fNamestore);  
			break;
		case htmlMethods: 
			fCurField= attr;
			//fNamestore= (char*) MemFree(fNamestore);  
			break;
		//case htmlTitle: // handled in other section !! 
		}
}



void DHTMLprocess::handlePictAttr(short attr)
{
	switch (attr) {
		case htmlImage: {
			// main tag
			fCurControl= attr;
			fCurField= attr;
			}
			break;
			
		case htmlImageSrc: 
			// this is the URL to remote picture ??
			fCurField= attr;
			// store: ="url://bob.was/here"
			fURLstore= (char*) MemFree(fURLstore);  
			break;
			
		case htmlImageAlt: 
			// text name for non-graphic clients?
		case htmlIsMap: 
		case htmlAlign: 
			fCurField= attr;
			break;
		}
}

#ifdef TESTDEF
#define  DEFAULTS() { DefaultParag(); DefaultStyle(); GetNlmFont(); }
#else
#define DEFAULTS()	{}
#endif
			
void DHTMLprocess::handleParAttr(short attr)
{
	switch (attr) {
	
		case htmlH1: 
			if (fTurnOn) {
				PushStyle();
				handleSpecialChar(htmlPage);
				DefaultStyle();
				fParaFmt.just= 'c';
				fParaFmt.spaceBefore= 20; 
				fStyleRec.style.bold= true;
				fStyleRec.fontsize= 2 * kDefaultFontsize;
				}
			else {
				fParaFmt.spaceAfter= 10; 
				PutLitCharWithStyle('\n');
				NewParagraph();
				fParaFmt.just= 'l';
				fParaFmt.spaceBefore= 0; 
				fParaFmt.spaceAfter= 0; 
				PopStyle();
				DEFAULTS();
				}
			NewStyle();
			break;
			
		case htmlH2: 
 			if (fTurnOn) {
				PushStyle();
				PutLitCharWithStyle ('\n');
				NewParagraph();
				DefaultStyle();
				fParaFmt.spaceBefore= 10; 
				fStyleRec.style.bold= true;
				fStyleRec.fontsize= 6 + kDefaultFontsize;
				}
			else {
				fParaFmt.spaceAfter= 5; 
				PutLitCharWithStyle ('\n');
				NewParagraph();
				fParaFmt.spaceBefore= 0; 
				fParaFmt.spaceAfter= 0; 
				PopStyle();
				DEFAULTS();
				}
			NewStyle();
			break;
                        
		case htmlH3: 
 			if (fTurnOn) {
				PushStyle();
				PutLitCharWithStyle ('\n');
				NewParagraph();
				DefaultStyle();
				fParaFmt.spaceBefore= 6; 
				fStyleRec.style.italic= true;
				fStyleRec.fontsize= 6 + kDefaultFontsize;
				}
			else {
				fParaFmt.spaceAfter= 4; 
				PutLitCharWithStyle ('\n');
				NewParagraph();
				fParaFmt.spaceBefore= 0; 
				fParaFmt.spaceAfter= 0; 
				PopStyle();
				DEFAULTS();
				}
			NewStyle();
			break;
	
		case htmlH4:                          
			if (fTurnOn) {
				PushStyle();
				PutLitCharWithStyle('\n');
				NewParagraph();
				DefaultStyle();
				fStyleRec.style.bold= true;
				fStyleRec.fontsize= 2 + kDefaultFontsize;
				}
			else {
				fParaFmt.spaceAfter= 4; 
				PutLitCharWithStyle('\n');
				NewParagraph();
				fParaFmt.spaceBefore= 0; 
				fParaFmt.spaceAfter= 0; 
				PopStyle();
				DEFAULTS();
				}
			NewStyle();
			break;

		case htmlH5: 
			if (fTurnOn) {
				PushStyle();
				PutLitCharWithStyle('\n');
				NewParagraph();
				DefaultStyle();
				fStyleRec.fontsize= 2 + kDefaultFontsize;
				//fStyleRec.style.italic= true;
				}
			else {
				fParaFmt.spaceAfter= 2; 
				PutLitCharWithStyle('\n');
				fParaFmt.spaceBefore= 0; 
				fParaFmt.spaceAfter= 0; 
				PopStyle();
				DEFAULTS();
				}
			NewStyle();
			break;
			
		case htmlH6: 
			if (fTurnOn) {
				PushStyle();
				PutLitCharWithStyle('\n');
				NewParagraph();
				DefaultStyle();
				//fStyleRec.style.bold= true;
				}
			else {
				fParaFmt.spaceAfter= 2; 
				PutLitCharWithStyle('\n');
				PopStyle();
				fParaFmt.spaceBefore= 0; 
				fParaFmt.spaceAfter= 0; 
				DEFAULTS();
				}
			NewStyle();
			break;


		case htmlAddress: 
			if (fTurnOn) {
				PushStyle();
				PutLitCharWithStyle('\n');
				NewParagraph();
				DefaultStyle();
				fParaFmt.just= 'r';
				fStyleRec.style.italic= true;
				}
			else {
				PutLitCharWithStyle('\n');
				NewParagraph();
				fParaFmt.just= 'l';
				PopStyle();
				DEFAULTS();
				}
			NewStyle();
			break;

		case htmlBlockquote: 
			if (fTurnOn) {
				PushStyle();
				PutLitCharWithStyle('\n');
				fParaFmt.spaceBefore= 10; 
				//fParaFmt.borderleft= TRUE;
				NewParagraph();
				fParaFmt.leftInset  += 30;  
				fParaFmt.rightInset += 50;  
				fStyleRec.style.italic= true;
				}
			else {
				fParaFmt.spaceAfter= 10; 
				PutLitCharWithStyle('\n');
				NewParagraph();
				fParaFmt.spaceBefore= 0; 
				fParaFmt.spaceAfter= 0; 
				//fParaFmt.borderleft= false;
				fParaFmt.leftInset = MAX( 0, fParaFmt.leftInset-30);  
				fParaFmt.rightInset= MAX( 0, fParaFmt.rightInset-50);  
				PopStyle();
				}
			NewStyle();
			break;

		case htmlPreformat: 
			fPreformat= fTurnOn;
			handleCharAttr(htmlFixedwidth);
			NewStyle();
			break;

		case htmlLinewidth: 
			// this is attrib of htmlPreformat
			break;
 
			// lists/tables ?
 		case htmlDL:
 			if (fTurnOn) {
				fParaFmt.leftInset  += kDefTabstop;  
				fParaFmt.firstInset -= kDefTabstop;  
 				fDLStyle= htmlDL;
 				}
 			else {
				PutLitCharWithStyle('\n');
				NewParagraph();
				fParaFmt.leftInset  = MAX(0, fParaFmt.leftInset - kDefTabstop);  
				fParaFmt.firstInset = MIN(0, fParaFmt.firstInset + kDefTabstop);  
 				}
 				
 			// attribs of htmlDL
 		case htmlCompact:
 			fDLStyle= htmlCompact;
 			break;
 		case htmlDT:
			PutLitCharWithStyle('\n');
			NewParagraph();
			fParaFmt.leftInset  = MAX(0, fParaFmt.leftInset - kDefTabstop);  
			fParaFmt.firstInset = MIN(0, fParaFmt.firstInset + kDefTabstop);  
			break;
 		case htmlDD:
 			if (true) { //fDLStyle != htmlCompact) {
	 			PutLitCharWithStyle('\n');
				NewParagraph();
				fParaFmt.leftInset  += kDefTabstop;  
				fParaFmt.firstInset -= kDefTabstop;  
				}
			PutStdCharWithStyle('\t');
 			break;
		}
}


void DHTMLprocess::handleListAttr(short attr)
{
	switch (attr) {
	
		case htmlNumList:
		case htmlMenu:
		case htmlDir:
		case htmlBullList:
			fListType= attr;
			fListNum= 0;
 			if (fTurnOn) {
				PutLitCharWithStyle('\n');
				NewParagraph();
				fParaFmt.leftInset  += kDefTabstop;  
				fParaFmt.firstInset -= kDefTabstop - 20;  
 				}
 			else {
				PutLitCharWithStyle('\n');
				NewParagraph();
				fParaFmt.leftInset  = MAX(0, fParaFmt.leftInset-kDefTabstop);  
				fParaFmt.firstInset = MIN(0, fParaFmt.firstInset+kDefTabstop+20);  
				PutLitCharWithStyle('\n');
				NewParagraph();
 				}		
			break;

	
		case htmlListItem:
			PutLitCharWithStyle('\n');
			NewParagraph();
			fListNum++; // count isn't nested, nor is fListType !!
			// !!? need push/pop for list vars !!!!!!
			if (fListType == htmlNumList) {
				char nums[30];
				sprintf( nums, "%d\t", fListNum);
				PutLitStr( nums);
				}
			else {
				PutStdCharWithStyle( gGenCharMap[rtfSC_bullet]); //'¥'
				PutStdChar('\t');
				}
			break;
			
		}
}




void DHTMLprocess::handleCharAttr(short attr)
{
	switch (attr) {

	case htmlFixedwidth: //rtfFontNum:
		if (fTurnOn) {
			PushStyle();
			fStyleRec.fontname= "courier";
			fStyleRec.fontfamily= "Modern";
			fStyleRec.fontsize= kDefaultFontsize; 
			}
		else {
			PopStyle();
			} 
		NewStyle();
		//(void) GetNlmFont();  
		break;
				
	case htmlSampleFont:
 	case htmlUserFont:
		if (fTurnOn) {
			PushStyle();
			fStyleRec.fontname= "helvetica";
			fStyleRec.fontfamily= "Swiss";
			fStyleRec.fontsize= kDefaultFontsize; 
			}
		else {
			PopStyle();
			} 
		NewStyle();
		//(void) GetNlmFont();  
		break;

	case htmlPlain:
		DefaultStyle();
		NewStyle();
		break;
		
	case htmlVarFont: //rtfSmallCaps:
		NewStyle();
		fStyleRec.style.smallcaps= fTurnOn;
		break;

	case htmlEmphasis:
	case htmlBold:
		NewStyle();
		fStyleRec.style.bold= fTurnOn;
		break;
		
	case htmlDefineFont:
	case htmlCiteFont:
		// ?? also use diff font??
	case htmlItalic:
		NewStyle();
		fStyleRec.style.italic= fTurnOn;
		break;
	case htmlMoreEmphasis: //rtfUnderline:
		NewStyle();
		fStyleRec.style.bold= fTurnOn;
		if (fTurnOn) fStyleRec.style.underline = DRichStyle::kUnderline;
		else fStyleRec.style.underline= 0;
		break;
	case htmlInvisible:
		NewStyle();
		fStyleRec.style.hidden= fTurnOn;
		break;
	}
}


void DHTMLprocess::handleSpecialChar(short code)
{
	switch (code) {
		case htmlPar: 
			fParaFmt.spaceAfter += 5; 
			PutLitCharWithStyle('\n');
			NewParagraph();
			fParaFmt.spaceAfter -= 5; 
			fLastChar= ' ';
			break;
		case htmlNewline:
			PutLitCharWithStyle('\n');
			NewParagraph();
			fLastChar= ' ';
			break;
			
		case htmlHBar:
			fParaFmt.borderstyle = DParagraph::kBorderSingle; 
			fParaFmt.borderbottom = TRUE; 
			PutLitCharWithStyle('\n');
			PutLitChar(' ');
			NewParagraph();
			fParaFmt.borderstyle = DParagraph::kNoBorder; 
			fParaFmt.borderbottom = FALSE; 
			PutLitCharWithStyle('\n');
			NewParagraph();
			fLastChar= ' ';
			break;
			
		case htmlPage: 
			PutLitCharWithStyle ('\n');
			fParaFmt.newPage= TRUE;  
			NewParagraph();
			fParaFmt.newPage= FALSE; 
			fLastChar= ' ';
			break;
		case htmlTab:
			PutLitCharWithStyle('\t'); // tab isn't defined in HTML world...
			fLastChar= ' ';
			break;

#if 0
		case rtfSC_less:
			PutLitCharWithStyle('<');
			break;
		case rtfSC_greater:
			PutLitCharWithStyle('>');
			break;
		case rtfSC_quotedbl:
			PutLitCharWithStyle('"');
			break;
		case rtfSC_ampersand:
			PutLitCharWithStyle('&');
			break;
#endif
		default:
			PutLitCharWithStyle( gGenCharMap[code]);
			fLastChar= gGenCharMap[code];
			break;
		}
};

	


HTMLKey DHTMLprocess::htmlKeys[] =
{
		// document
	htmlDocAttr, htmlHTML,	"html", kSlashStop, 0,	
	htmlDocAttr, htmlTitle,	"title", kSlashStop, 0,	
	htmlDocAttr, htmlHead,	"head", kSlashStop, 0,	
	htmlDocAttr, htmlBody,	"body", kSlashStop, 0,	
	htmlDocAttr, htmlIsIndex,	"isindex", kNoStop, 0,	
	htmlDocAttr, htmlLink,	"link", kNoStop, 0,	
	htmlDocAttr, htmlNextID,	"nextid", kNoStop, 0,	
	htmlDocAttr, htmlBase,	"base", kNoStop, 0,	
	  //htmlDocAttr,	htmlHref, "href=", kNoStop, 0,
	htmlDocAttr,	htmlComment,	"!", kNoStop, 0,

		// anchors & links
	htmlLinkAttr,	htmlAnchor,	"a", kSlashStop, 0,
	  htmlLinkAttr,	htmlHref, "href", kNoStop, 0,
	  htmlLinkAttr,	htmlName, "name", kNoStop, 0,
		htmlLinkAttr,	htmlRel, "rel", kNoStop, 0,
		htmlLinkAttr,	htmlRev, "rev", kNoStop, 0,
		htmlLinkAttr,	htmlUrn, "urn", kNoStop, 0,	
		htmlLinkAttr,	htmlMethods, "methods", kNoStop, 0,	
		//htmlLinkAttr, htmlTitle,	"title", kNoStop, 0,	
	
		// forms tags
  htmlFormAttr,	htmlForm, "form", kSlashStop, 0,
	  htmlFormAttr,	htmlInput, "input", kNoStop, 0,
	  htmlFormAttr,	htmlSelect, "select", kSlashStop, 0,
		htmlFormAttr,	htmlTextarea, "textarea", kSlashStop, 0,
		htmlFormAttr,	htmlValue, "value", kNoStop, 0,
		htmlFormAttr,	htmlType, "type", kNoStop, 0,
		htmlFormAttr,	htmlOption, "option", kNoStop, 0,
		htmlFormAttr,	htmlAction, "action", kNoStop, 0,
		htmlFormAttr,	htmlMethod, "method", kNoStop, 0,
		
		htmlFormAttr,	htmlChecked, "checked", kNoStop, 0,
		htmlFormAttr,	htmlSelected, "selected", kNoStop, 0,
		htmlFormAttr,	htmlMultiple, "multiple", kNoStop, 0,
		htmlFormAttr,	htmlSize, "size", kNoStop, 0,
		htmlFormAttr,	htmlMaxlength, "maxlength", kNoStop, 0,
		htmlFormAttr,	htmlRows, "rows", kNoStop, 0,
		htmlFormAttr,	htmlCols, "cols", kNoStop, 0,
		
		// font styles
	htmlCharAttr,	htmlBold,		"b",		kSlashStop, 0,
	htmlCharAttr,	htmlEmphasis,		"em",	kSlashStop, 	0,
	htmlCharAttr,	htmlMoreEmphasis,		"strong",	kSlashStop, 	0,
	htmlCharAttr,	htmlItalic,		"i",	kSlashStop, 	0,
	htmlCharAttr,	htmlFixedwidth,		"tt",	kSlashStop, 	0, // courier font !?
	htmlCharAttr,	htmlFixedwidth,		"code",	kSlashStop, 	0, // courier font !?
	htmlCharAttr,	htmlSampleFont,		"samp",	kSlashStop, 	0, 
	htmlCharAttr,	htmlUserFont,		"kbd",	kSlashStop, 	0, 
	htmlCharAttr,	htmlVarFont,		"var",	kSlashStop, 	0, 
	htmlCharAttr,	htmlDefineFont,		"dfn",	kSlashStop, 	0, // bold
	htmlCharAttr,	htmlCiteFont,		"cite",	kSlashStop, 	0, // italic
	htmlCharAttr,	htmlPlain,		"plaintext",	kSlashStop, 	0,  

		// paragraph styles
	htmlParAttr,	htmlH1,	"h1", kSlashStop, 	0,
	htmlParAttr,	htmlH2,	"h2", kSlashStop, 0,
	htmlParAttr,	htmlH3,	"h3", kSlashStop, 0,
	htmlParAttr,	htmlH4,	"h4", kSlashStop, 0,
	htmlParAttr,	htmlH5,	"h5", kSlashStop, 0,
	htmlParAttr,	htmlH6,	"h6", kSlashStop, 0,
	htmlParAttr,	htmlAddress,	"address", kSlashStop, 0,
	htmlParAttr,	htmlBlockquote,	"blockquote", kSlashStop, 0,

	htmlParAttr,	htmlPreformat,	"pre", kSlashStop, 0,
	  htmlParAttr,	htmlLinewidth,	"width", kNoStop, 0,
	htmlParAttr,	htmlPreformat,		"listing",		kSlashStop, 0,  
	htmlParAttr,	htmlPreformat,		"xmp",		kSlashStop, 0,  

		// lists/tables ?
	htmlParAttr,	htmlDL,	"dl", kSlashStop, 0,
	  htmlParAttr,	htmlCompact,	"compact",  kNoStop,0,
	  htmlParAttr,	htmlDT,	"dt", kNoStop, 0,
	  htmlParAttr,	htmlDD,	"dd", kNoStop, 0,

		// lists
	htmlListAttr,	htmlBullList,	"ul",  kSlashStop, 0,
	htmlListAttr,	htmlNumList,	"ol",  kSlashStop, 0,
	htmlListAttr,	htmlMenu,	"menu",  kSlashStop, 0,
	htmlListAttr,	htmlDir,	"dir",  kSlashStop, 0,
	htmlListAttr,	htmlListItem,	"li",  kNoStop,0,

		// pictures
	htmlPictAttr,	htmlImage,		"img", kNoStop,	0,
	  htmlPictAttr,	htmlImageSrc,		"src", kNoStop,	0,
	  htmlPictAttr,	htmlImageAlt,		"alt", kNoStop,	0,
		htmlPictAttr,	htmlAlign,		"aln", kNoStop,	0,
		htmlPictAttr,	htmlAlign,		"align", kNoStop,	0,
	  htmlPictAttr,	htmlIsMap,		"ismap", kNoStop,	0,

		// characters
	htmlSpecialChar,	htmlPar,		"p", kNoStop,	0, // parag
	htmlSpecialChar,	htmlHBar,		"hr", kNoStop,	0, // paragraph rule
	0,		-1,			NULL,	0
};

	

HTMLKey DHTMLprocess::htmlAmperChars[] =
{
	htmlSpecialChar,	rtfSC_less, "lt;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_greater, "gt;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_ampersand, "amp;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_quotedbl, "quot;", kAmperChar,		0,

	htmlSpecialChar,	rtfSC_AE, "AElig;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Aacute, "Aacute;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Acircumflex, "Acirc;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Agrave, "Agrave;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Aring, "Aring;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Atilde, "Atilde;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Adieresis, "Auml;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Ccedilla, "Ccedil;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Eth, "ETH;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Eacute, "Eacute;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Ecircumflex, "Ecirc;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Egrave, "Egrave;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Edieresis, "Euml;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Iacute, "Iacute;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Icircumflex, "Icirc;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Igrave, "Igrave;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Idieresis, "Iuml;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Ntilde, "Ntilde;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Oacute, "Oacute;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Ocircumflex, "Ocirc;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Ograve, "Ograve;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Oslash, "Oslash;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Otilde, "Otilde;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Odieresis, "Ouml;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Thorn, "THORN;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Uacute, "Uacute;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Ucircumflex, "Ucirc;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Ugrave, "Ugrave;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Udieresis, "Uuml;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_Yacute, "Yacute;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_aacute, "aacute;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_acircumflex, "acirc;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_ae, "aelig;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_agrave, "agrave;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_aring, "aring;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_atilde, "atilde;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_adieresis, "auml;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_ccedilla, "ccedil;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_eacute, "eacute;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_ecircumflex, "ecirc;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_egrave, "egrave;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_eth, "eth;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_edieresis, "euml;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_iacute, "iacute;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_icircumflex, "icirc;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_igrave, "igrave;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_idieresis, "iuml;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_ntilde, "ntilde;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_oacute, "oacute;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_ocircumflex, "ocirc;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_ograve, "ograve;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_oslash, "oslash;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_otilde, "otilde;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_odieresis, "ouml;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_nothing, "szlig;", kAmperChar,		0, // ! no rtf equiv.
	htmlSpecialChar,	rtfSC_thorn, "thorn;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_uacute, "uacute;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_ucircumflex, "ucirc;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_ugrave, "ugrave;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_udieresis, "uuml;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_yacute, "yacute;", kAmperChar,		0,
	htmlSpecialChar,	rtfSC_ydieresis, "yuml;", kAmperChar,		0,
	0,		-1,			NULL,	0
};



