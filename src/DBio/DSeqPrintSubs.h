// DSeqPrintSubs.h
// d.g.gilbert 

#ifndef _DSEQPRINTSUBS_
#define _DSEQPRINTSUBS_

#include <ncbi.h>
#include <DTableView.h>
#include <DWindow.h>
#include <DMethods.h>

class DPopupList;

enum FonStyles { 
	kPlain = 0, kItalic = 1, kBold = 2, kUnderline = 4 
	};

enum {
	kShadeInvert = 1,
	kShadeGray50 = 2,
	kShadeGray25 = 3,
	kShadeGray75 = 4,
	kShadeStipple50	= 5,
	kShadeStipple25	= 6,
	kShadeStipple75	= 7,
	
	mColorButHit = 1001,
	mMonoButHit	= 1002,

	kFontDescent = 2,
	kIndexRise = 1,

	kOutPICT = 1,
	kOutText = 2,
	
  kNameWidth  = 80,
  kIndexWidth = 40,
  kItemWidth  = 12,
	kNucSpace 	= 0, //was 2
	kBasesPerLine = 60, // was 50
  //kSeqWidth = kItemWidth * kBasesPerLine,
	kNucBorder	= 6,
	kSeqLinesPerParag = 5,
	kLinesPerParag= 7, // kSeqLinesPerParag + 2; // 5 seq + top index + top spacer

	kMacdrawHeaderSize= 512
	};

enum SeqRowType { kSpacer, kTopline, kSeqline };





class DSeqPrintPrefs : public DWindow {
public:
	enum { 
			kSeqPrintPrefID = 1232,
			kStylePlain,kStyleItalic,kStyleBold,kStyleUnderline,
			cIndexLeft, cIndexRight, cIndexTop, 
			cNameLeft, cNameRight,
			cColored,
			cShowComplement,cThreeLetAA,cOnlyORF,
			cShowAA1,cShowAA2,cShowAA3,
			cShowCompAA1,cShowCompAA2,cShowCompAA3,
			cShowCutpoints,cShowAllZymes,cShowExcludedCutters,cShowNoncutters,
			cOutText, cOutPICT
			};

	static char *gNameFontName, *gBaseFontName, *gIndexFontName;
	static Nlm_FonT	gNameFont, gBaseFont, gIndexFont;
	static short gNameFontSize, gBaseFontSize, gIndexFontSize;
	static short gNameStyle, gBaseStyle, gIndexStyle;
	static Boolean gNameLeft,gNameRight,gIndexLeft,gIndexRight,gIndexTop, gColored;
	static short gBasesPerLine, gOutFormat;
		// restmap prefs ..
	static Boolean  gShowComplement,gThreeLetAA, gOnlyORF,
				gShowAA1,gShowAA2,gShowAA3,
				gShowCompAA1,gShowCompAA2,gShowCompAA3,
				gShowAllZymes,gShowCutpoints,gShowExcludedCutters,gShowNoncutters;
	static short	gREMinCuts, gREMaxCuts;
	
	static void InitGlobals();
	static void SaveGlobals();

	DPopupMenu  * fNameFontMenu, * fBaseFontMenu, *fIndexFontMenu,
							* fNameStyleMenu, *fBaseStyleMenu, *fIndexStyleMenu;
	DPopupList  * fOutFormPop;
	DSwitchBox	* fNameSizeSw, * fBaseSizeSw, *fIndexSizeSw;
	DEditText		* fREMinCuts, * fREMaxCuts, * fBasePerLine;
	Boolean	 		  fNeedSave;
	
	DSeqPrintPrefs();
	virtual ~DSeqPrintPrefs();
	virtual void Initialize();
	virtual void Open();
	virtual void Close();
	virtual void OkayAction();
	virtual Boolean IsMyAction(DTaskMaster* action); 
	virtual void NewFontCluster(char* title, DView* mainview,
					DPopupMenu*& mfont, DPopupMenu*& mstyle, DSwitchBox*& swsize,
					char* fontname, short fontstyle, short fontsize );
					
};


class DDrawMapRow : public DObject {
public:
	enum { kNoIndex = -999, kMaxLinebuf= 1024 };
	static char fLinebuf[kMaxLinebuf+1];
	Nlm_FonT	fFont; 
	char	* fName;
	short	  fHeight, fRowOffset, fLinecount, fItemrow;

	DDrawMapRow() : 
		fFont(NULL), fName(NULL), fHeight(Nlm_stdLineHeight), 
		fRowOffset(-1), fLinecount(0), fItemrow(0) 
		{}
	virtual void Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem) {}
	virtual const char* Write( short row, short col, long startitem, long stopitem) { return ""; }
	virtual char* GetName(short row) { return fName; }
	virtual short GetIndex(short item) { return item; }
	virtual short GetHeight( Nlm_RecT& r, short row, long startitem, long stopitem) { return fHeight; }
	virtual Boolean DoLeftName() { return false; }
	virtual void  Clip( Nlm_RecT& r, short row, long startitem, long stopitem,
										 Nlm_RecT& viewr, Nlm_RegioN cliprgn)
	{
		Nlm_ClipRgn( cliprgn);
	}
};


class DDrawSpacer : public DDrawMapRow {
public:
	char *fTitle;
	DDrawSpacer( char* title= NULL, Nlm_FonT itsFont = NULL) : fTitle(title) { fFont= itsFont; }
	virtual void Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem);
	virtual const char* Write( short row, short col, long startitem, long stopitem);
	virtual short GetIndex(short item) { return kNoIndex; }
};


class DDrawIndexRow : public DDrawMapRow {
public:
	Nlm_Boolean fIsUp;
	short	fItemWidth;
	DDrawIndexRow( Nlm_FonT itsFont, short itemWidth, Nlm_Boolean upright= true) : 
		fIsUp(upright),fItemWidth(itemWidth) 
		{ 
			fFont= itsFont; 
			if (fFont) Nlm_SelectFont(fFont);
			fHeight= 3 + Nlm_LineHeight();
		}
	virtual void Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem);
	virtual const char* Write( short row, short col, long startitem, long stopitem);
	virtual short GetIndex(short item) { return kNoIndex; }
};


class DDrawSeqRow : public DDrawMapRow {
public:
	char		fNameStore[20];
	DSeqPrintView * fView;
	ulong	* fColors;
	DSequence * fSeq, * fTopSeq, * fBotSeq;
	char	* fBases;
	char	* fFirstcommon, * fCommonbase;
	long 		fLength;
	short		fItemWidth;
	DList *	fStyles;
	
	DDrawSeqRow(Nlm_FonT itsFont, DSeqPrintView* itsView, DSequence* itsSeq,
					 			DList* itsStyles, char* name, ulong* colors);
	virtual void Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem);	
	virtual const char* Write( short row, short col, long startitem, long stopitem);
	virtual DSeqStyle* GetStyle(long ibase, short masklevel, DSequence* aSeq);
};


class DDrawManySeqRow : public DDrawSeqRow {
public:
	DSeqList * fSeqList;
	//short			 fLinesPerparag, fTopPerparag;
	
	DDrawManySeqRow(Nlm_FonT itsFont, DSeqPrintView* itsView, 
									DSeqList* seqlist, long seqIndex, DList* itsStyles, ulong* colors,
									char* commonbases, char* firstcommon);
};


class DDrawAminoRow : public DDrawMapRow {
public:
	char		fNameStore[20];
	char		* fBases;
	short		fFrame, fItemWidth;
	Boolean fThreeLet, fOnlyORF;
	DSequence * fAAseq;
	long		fLength;
	
	DDrawAminoRow(Nlm_FonT itsFont, DSequence* itsSeq, short frame);
	virtual ~DDrawAminoRow() { if (fAAseq) delete fAAseq; }
	virtual void Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem);	
	virtual const char* Write( short row, short col, long startitem, long stopitem);
	virtual short GetIndex(short item) { return kNoIndex; }
};

class DDrawORFRow : public DDrawSeqRow {
public:
	char		fNameStore[20];
	DSequence * fAAseq;
	short		fFrame;

	DDrawORFRow(Nlm_FonT itsFont, DSeqPrintView* itsView, DSequence* itsSeq,
					 			DList* itsStyles, char* name, ulong* colors, short frame);
	virtual ~DDrawORFRow();
	virtual void Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem);	
	virtual const char* Write( short row, short col, long startitem, long stopitem);
	virtual short GetIndex(short item) { return kNoIndex; }
};


class DDrawZymeTable : public DDrawMapRow { 
public:
	char** fLinelist;
	char * fTableTitle;
	
	DDrawZymeTable(Nlm_FonT itsFont, DREMap* itsREMap, short rowoffset);
	virtual ~DDrawZymeTable();
	virtual void Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem);	
	virtual const char* Write( short row, short col, long startitem, long stopitem);
	virtual short GetIndex(short item) { return kNoIndex; }
	virtual char* TableLine(short atline); 
	virtual void  Clip( Nlm_RecT& r, short row, long startitem, long stopitem,
										 	Nlm_RecT& viewr, Nlm_RegioN cliprgn);
};

class DDrawAllZymeTable : public DDrawZymeTable { 
public:
	DDrawAllZymeTable(Nlm_FonT itsFont, DREMap* itsREMap, short rowoffset);
};

class DDrawNocutZymeTable : public DDrawZymeTable { 
public:
	DDrawNocutZymeTable(Nlm_FonT itsFont, DREMap* itsREMap, short rowoffset, 
											short mincuts = 0, short maxcuts = 0);
};


class DDrawZymeCutTable : public DDrawZymeTable { 
public:
	DRECutsItem * fCutList;  			 
	long	fCutcount;
	short	fCuttersCount; 			//# zymes that cut
	
	DDrawZymeCutTable(Nlm_FonT itsFont, DREMap* itsREMap, short rowoffset);
	virtual ~DDrawZymeCutTable();
	virtual char* TableLine(short atline); 
	virtual char* GetName(short row); 
	virtual Boolean DoLeftName() { return true; }
};

class DDrawZymeRow : public DDrawMapRow { 
public:
  DSequence * fSeq, * fCoSeq;
	DREMap * fREMap;
	short		fMinCuts, fMaxCuts;
	DRECutsItem * fCutList;
	Boolean 	fGoodMap;
	
	DDrawZymeRow( Nlm_FonT itsFont, DSequence* itsSeq);
	virtual ~DDrawZymeRow() { delete fREMap; }
	virtual short DrawOrMeasure( Nlm_Boolean doDraw, Nlm_RecT& r, short row, long startitem, long stopitem);	
	virtual void  Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem);	
	virtual const char* Write( short row, short col, long startitem, long stopitem);
	virtual short GetIndex(short item) { return kNoIndex; }
	virtual short GetHeight( Nlm_RecT& r, short row, long startitem, long stopitem); 
	virtual void  Clip( Nlm_RecT& r, short row, long startitem, long stopitem,
										 	Nlm_RecT& viewr, Nlm_RegioN cliprgn);
};


class DSeqPrintView : public DTableView 
{
public:
	Boolean		fOneseq, fDoLeftName, fDoRightName, 
						fDoTopIndex, fDoLeftIndex, fDoRightIndex; 
	Nlm_FonT	fNameFont, fNumFont, fBaseFont;
	short 		fBaseWidth, fIndexWidth, fIndexWidthCnt, fNameWidth, fNameWidthCnt, 
						fSeqsperparag, fExtrarows, fSeqLinesPerParag, fLinesPerParag, 
						fTopPerparag, fBasesPerLine, fTenSpacer;
	long 			fFirstBase, fNbases, fMaxbases, fSeqWidth;
	DSeqPrintDoc	* fDoc;
	DSeqList	* fSeqList;  
	DSequence * fSeq;
	DList			* fDrawRowList, * fStyles;
	DFile			* fFile;
	char			* fCommonbase, * fFirstcommon;

	//DCheckBox*		fLeftName, fRightName, fTopIndex, fLeftIndex, fRightIndex ;
	//DDlogTextView*		fStyleName, fStyleBase, fStyleNums; 
	
	DSeqPrintView( long id, DView* itsSuper, DSeqPrintDoc* itsDocument,
								 DSeqList* itsSeqList, long firstbase, long nbases,
								 long pixwidth, long pixheight);
	virtual ~DSeqPrintView();
	virtual void Initialize();
	virtual void FreeLists();

	virtual void MakeDrawList();
	virtual void Draw();
	virtual void DrawRow(Nlm_RecT r, long row);
	virtual void IndexFromRow( long row, long& itemrow, long& seqline, long& startitem, long& stopitem);
	virtual void GetReadyToShow();
	virtual void DrawSideIndex( Nlm_RecT& aRect, long atBase, long leftBorder);
	virtual void WriteSideIndex( long atBase, long leftBorder);
	virtual void DrawName( Nlm_RecT& aRect, short rightBorder, char* name);
	virtual void WriteName( short rightBorder, char* name);
	virtual short GetExtraRows() { return fExtrarows; }

	virtual void WriteTo(DFile* aFile);
	virtual	void WriteRow(long row);
};


class DAlnPrintView : public DSeqPrintView {
public:

	DAlnPrintView( long id, DView* itsSuper, DSeqPrintDoc* itsDocument,
								 DSeqList* itsSeqList, long firstbase, long nbases, long pixwidth, long pixheight);
};



class DREMapPrintView : public DSeqPrintView {
public:
	Boolean fDoAAline[6];
	Boolean fDoSeqLine,fDoMidIndex,fDoCoseqLine,fDoZymeLine;
	short		fMaxseqrow;
	
	DREMapPrintView( long id, DView* itsSuper, DSeqPrintDoc* itsDocument,
					DSeqList* itsSeqList, long firstbase, long nbases, long pixwidth, long pixheight);
	virtual void Initialize();
	virtual void MakeDrawList();
	virtual void SetScrollPage();
	virtual void IndexFromRow( long row, long& itemrow, long& seqline, long& startitem, long& stopitem);
};





class DAsmPrintView : public DSeqPrintView {
public:
	Boolean fDoAAline[6];
	Boolean fDoSeqLine,fDoMidIndex,fDoCoseqLine,fDoZymeLine;
	short		fMaxseqrow;
	
	DAsmPrintView( long id, DView* itsSuper, DSeqPrintDoc* itsDocument,
					DSeqList* itsSeqList, long firstbase, long nbases, long pixwidth, long pixheight);
	virtual void Initialize();
	virtual void MakeDrawList();
	virtual void SetScrollPage();
	virtual void IndexFromRow( long row, long& itemrow, long& seqline, long& startitem, long& stopitem);
};


#endif  


