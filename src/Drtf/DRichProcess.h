// DRichProcess.h
// d.g.gilbert



#ifndef _DRichPROCESS_
#define _DRichPROCESS_

#include "DObject.h"
#include "Dvibrant.h"
#include "DRichViewNu.h"

class DFile;



struct StyleRec {
	DRichStyle	style;
	char*	fontname;
	char*	fontfamily;
	short fontsize;
	StyleRec();
};

class DRichprocess {
public:
	enum {
		kMaxStyleStack = 50, 
		kMaxTokenBuf = 1024, 	
		kDefaultFontsize = 10,
		kBlackColor = 0,
		kDefTabstop= 36 
		};
		
	enum classes {
		tokUnknown = 0,  // match rtfClass values !
		tokGroup = 1,
		tokText = 2,
		tokControl = 3,
		tokDropchar = 4,
		tokMaxClass,
		tokNoParam = 0,
		tokBreakInData = -6,
		tokEOF = EOF	
		};

	enum controls {
		tokNull,
		tokNewline
		};
 	
	DRichprocess( DFile* itsFile, DRichView* itsDoc, Nlm_MonitorPtr progress);
	virtual ~DRichprocess();

	static char* gGenCharMap;
	static char* gSymCharMap;
	static char* ReadOutputMap( char *file);
	
	virtual void  Read();
	virtual void  Close();
	virtual short GetToken();
	virtual void  RouteToken();
	virtual void	handleUnknownClass(); 
	virtual void	handleTextClass();
	virtual void	handleControlClass(); 
	
	virtual const char*  GetTitle() { return fTitle; }
	virtual void	SetBuffer( char* dataBuffer, ulong dataSize, Boolean endOfData);
 
public:
	short 			fClass, fMajor, fMinor;
	long	  		fParam;
	Boolean			fEndOfData;
	DFile				* fDataFile;
	char 				* fDataBuffer;
	long				fDataSize;
	DRichView		* fDoc;
	Nlm_MonitorPtr fProgress;
	long 				fTextMax, fTextSize, fLastTextSize;
	char				* fText;
	char				fLastChar, fPushedChar;
	StyleRec		fStyleRec, fOldStyleRec,  fStyleStack[kMaxStyleStack];
	short				fStyleStackSize;
	DParagraph	fParaFmt, fOldParaFmt, fDefParaFmt;
	short				fParaCount;
	DRichStyle * fStyleArray;
	short				fStyleCount, fStyleMax;
	Nlm_FonT		fFont;  
	Boolean			fNewStyle;
 	char			* fTitle;
  char			* fOutMap;

	virtual void  PutLitChar(short c);
	virtual void  PutLitStr(char *s);
	virtual void  PutLitCharWithStyle(short c);
	virtual void  PutStdChar(short stdCode);
	virtual void  PutStdCharWithStyle(short stdCode);

	virtual void	StoreStyle(DRichStyle& theStyle, Boolean force);
	virtual void  StoreStyleObject(DStyleObject* sob, short width, short height);
	Boolean IsNewStyle();
	void NewStyle() { fNewStyle= true; }
	void NotNewStyle() { fNewStyle= false; }
	virtual Boolean TestNewStyle();
	virtual Nlm_FonT GetNlmFont();
	virtual void 	PushStyle();
	virtual void	PopStyle();
	virtual void	DefaultStyle();
	virtual void	DefaultParag();
	virtual void	NewParagraph();
	virtual void  SetDefaultParag(DParagraph* theDefault);

	virtual void  GetToken1(); 			// private
	virtual short GetOneChar(); 		// private
	virtual void 	Pushback(char c); // private
};



#endif
