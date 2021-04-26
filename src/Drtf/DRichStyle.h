// DRichStyle.h
// d.g.gilbert

#ifndef _DRICHSTYLE_
#define _DRICHSTYLE_

#include "Dvibrant.h"
#include "DObject.h"


class DRichView;


class DStyleObject : public DObject
{
public:
	enum { kDefaultObject = 1 };
	short 	fKind;
	ulong		fSize;
	char*		fData;

	DStyleObject();
	DStyleObject( short kind, void* data, ulong dsize, Boolean owndata= false);
	virtual	Boolean suicide(void);  // prefered to delete 
	virtual	Boolean suicide(short ownercount);
  virtual ~DStyleObject();
	virtual void SetData( short kind, void* data, ulong dsize, Boolean owndata);
  virtual void Draw(Nlm_RecT area);
  virtual Boolean CanDraw();
	virtual Boolean IsNotEqual( DStyleObject* other);
};




class DRichStyle : public DObject
{
public:
	enum ulines {
		kNoUnderline= 0,
		kUnderline= 1,
		kDotuline=2,
		kDbluline=3,
		kWorduline=4
		};

	DStyleObject*	fObject;
	short		nextofs;
  short		pixheight, pixwidth, pixleft;		
  short		linkid;		// gopher/html link id 

  Nlm_Uint4	color;
	Nlm_FonT	font;  					// includes fnt,size,bld/ital/uline 
	short	 		superSub; 			//point offset +/-, +=superscript 
  unsigned int  underline : 3; // 0,1=uline,2=dotuline,3=dbluline,4=worduline
  unsigned int  bold    	: 1;
	unsigned int  italic    : 1;
  unsigned int  outline		: 1;
  unsigned int  shadow		: 1;
  unsigned int  strikeout	: 1;
  unsigned int  smallcaps	: 1;
  unsigned int  allcaps		: 1;	
  unsigned int  boxed			: 1;
  unsigned int  hidden		: 1;
  
  unsigned int  ispict   	: 1;
  unsigned int  ownpict   : 1;
  unsigned int  ismap   	: 1;
  unsigned int  last      : 1;

	DRichStyle();
  virtual ~DRichStyle();
	virtual	Boolean suicide(void);  // prefered to delete 
	virtual	Boolean suicide(short ownercount);
	virtual Boolean IsNotEqual(DRichStyle* other);
	//virtual DObject* Clone();
	//virtual DRichStyle&  operator=( DRichStyle& ast);
};


struct StyleIndex {
	short	styleid;	
	short	nextofs;
};


class DParagraph : public DObject
{
public:
	enum borders {
		kNoBorder = 0,
		kBorderSingle = 1,
		kBorderThick = 2,
		kBorderShadow = 3,
		kBorderDouble = 4,
		kBorderDot = 5,
		kBorderDash = 6,
		kBorderHair = 7
		};
	 
	enum tabs {
	 	kTabLeft = 0,
	 	kTabCenter= 1,
	 	kTabRight= 2,
	 	kTabDecimal = 3,
	 	kTabBar = 4
		};

 	char    just, tablead; 				 
  short		deftabstop, numstops;
 	short	* tabstops;
 	char	* tabkinds;
  short   firstInset,leftInset,rightInset;
	short		spaceBefore,spaceAfter, spaceBetween;
  short 	minLines, minHeight;
  short 	startsAt, numRows;	 
  short 	lineHeight, leadHeight; 
  
  char	* fDataPtr;	 
  char	* fText;			 
	StyleIndex*	fStyleIndex;
	short		fNumStyles;

  unsigned int	borderstyle		: 4;
  unsigned int  bordertop    	: 1;
  unsigned int  borderbottom	: 1;
  unsigned int  borderleft    : 1;
  unsigned int  borderright  	: 1;
  unsigned int  borderbetween : 1;
  unsigned int  borderbar   	: 1;
 	unsigned int  borderbox    	: 1;
  unsigned int  openSpace    : 1;
  unsigned int  keepWithNext : 1;
  unsigned int  keepTogether : 1;
  unsigned int  newPage      : 1;
 	unsigned int  wrap      	 : 1;	
  unsigned int  fDocOwnsData : 1; 
  unsigned int  fDocOwnsStyles : 1; 
  unsigned int  notCached    : 1;  
  unsigned int  neverCached  : 1; 
	unsigned int	spaceMultiply: 1;

	DParagraph();
	DParagraph( char* data, Boolean docOwnsData, short lines, DParagraph* parFmt, 
          		StyleIndex* styles, short numstyle, Boolean docOwnsStyles = true); 
	virtual ~DParagraph();
	virtual	Boolean suicide(void);  // prefered to delete 
	virtual	Boolean suicide(short ownercount);
	void 	Initialize();
	void  Copy(DParagraph* it);
	virtual Boolean DoBorder();
	virtual void DrawBorder(DRichView* itsView, short row, short item, Nlm_RecT* rpara, Nlm_RecT& rct);
	virtual void SetTab( Boolean attab, short rleft, Nlm_RecT& drawr, Nlm_RecT* rpara, 
												 Boolean& bartab, char& just);
	virtual void DrawTab( DRichView* itsView, Nlm_RecT tabr, Nlm_RecT& drawr, Nlm_PoinT& atpt, 
												  short yfont, Boolean bartab, Boolean dodraw);
	//virtual DObject* Clone();
};



#endif

