// DHTMLprocess.h
// d.g.gilbert

#ifndef _DHTMLPROCESS_
#define _DHTMLPROCESS_

#include "DRichProcess.h"


struct HTMLKey
{
	short	htmlKMajor;	/* major number */
	short	htmlKMinor;	/* minor number */
	char	*htmlKStr;	/* symbol name */
	short	htmlKStop;	/* type of key termination */
	short	htmlKHash;	/* symbol name hash value */
};


 	
class DHTMLprocess : public DRichprocess
{
public:
	static void InitKeys();

	DHTMLprocess( DFile* itsFile, DRichView* itsDoc, Nlm_MonitorPtr progress);
								//DGopherList* itsLinks
	virtual ~DHTMLprocess();

	virtual void	handleTextClass();
	virtual void	handleControlClass(); 

protected:
	
	enum majorKeys {
		htmlDocAttr,
		htmlLinkAttr,
		htmlFormAttr,
		htmlCharAttr,
		htmlParAttr,
		htmlListAttr,
		htmlPictAttr,
		htmlSpecialChar,
		htmlEndControl
		};
	
	enum minorKeys {
		htmlHTML, htmlTitle, htmlHead, htmlBody, htmlIsIndex, htmlLink, htmlNextID, htmlBase,
		htmlAnchor, htmlHref, htmlName, htmlRel, htmlRev, htmlUrn, htmlMethods,
		htmlBold,htmlEmphasis,htmlMoreEmphasis,htmlItalic,htmlFixedwidth,
		htmlPlain, htmlSampleFont,htmlUserFont,htmlVarFont,htmlDefineFont,htmlCiteFont,
		htmlH1,htmlH2,htmlH3,htmlH4,htmlH5,htmlH6,htmlAddress,htmlBlockquote,
		htmlPreformat,htmlLinewidth, 
		htmlDL,htmlCompact,htmlDT,htmlDD,
		htmlBullList,htmlNumList,htmlMenu,htmlDir,htmlListItem,
		htmlImage,htmlImageSrc,htmlImageAlt,htmlAlign, htmlIsMap,
		htmlForm, htmlInput, htmlSelect, htmlTextarea, htmlValue, 
		htmlType, htmlOption, htmlAction, htmlMethod,
		htmlChecked,htmlSelected,htmlMultiple,htmlSize,htmlMaxlength,htmlRows,htmlCols,
		htmlInvisible, htmlComment,
		htmlNoKey = -1
		};
	
	enum specialChars {
		htmlPar= 300, htmlHBar, htmlNewline, htmlPage, htmlTab, 
		htmlNoChar = -1
		};
		
	enum	stopKeys {
		kNoStop,
		kSlashStop,
		kAmperChar // special characters of &char; style
		};
	
	static HTMLKey htmlKeys[];
	static HTMLKey htmlAmperChars[];
	static short Hash( char	*s);

	Boolean fInControl, fTurnOn, fPreformat, fIsIndex, fInForm, 
					fInHead, fInParam, fIsMap,
					fIsChecked, fIsMultiple;
	char	  fQuote, fTokenBuf[kMaxTokenBuf];
	long		fTokenLen;
	short		fListType, fListNum, fDLStyle, // these need to be stacks ??
					fStop, fCurControl, fCurField, fCurItem, fSelectitem, fPicLink, fAnchorLink;
 	char	* fURLstore, * fNamestore, * fFormURL, * fGoplusStore,
				* fSizestore, * fMaxlengthstore, * fRowsstore, * fColsstore,
 			  * fMethodstore, * fValstore, * fTypestore, *fOptionstore;
  char  * fPushBuf, * fPushBufstore;
  long    fPushSize;
  
	//DGopherList * fGolist;

	virtual void	handleDocAttr( short attr);  
	virtual void	handleLinkAttr( short attr); 
	virtual void	handleFormAttr( short attr); 
	virtual void	handleCharAttr( short attr); 
	virtual void	handleParAttr( short attr);  
	virtual void	handleListAttr( short attr);  
	virtual void	handlePictAttr( short attr); 
	virtual void	handleSpecialChar( short code); 
	virtual void	handleEndControl( short attr);  
 
	virtual void  GetToken1(); // private
	virtual short GotTokenOrParam(); // private
	virtual short GetOneChar();
	virtual void  Pushback(char c);
	virtual void  Lookup( char *s, HTMLKey* keyset);
};



#endif
