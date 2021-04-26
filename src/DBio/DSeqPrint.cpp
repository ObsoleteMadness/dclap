// DSeqPrint.cpp
// d.g.gilbert, 1991-95

#define MASKS 1

#include "DSequence.h"
#include "DSeqList.h"
#include "DSeqDoc.h"
#include "DSeqPrint.h"
#include "DREnzyme.h"

#include <ncbi.h>
#include <dgg.h>
#include <Dvibrant.h>
#include <DControl.h>
#include <DDialogText.h>
#include <DWindow.h>
#include <DTableView.h>
#include <DApplication.h>
#include <DTask.h>
#include <DTracker.h>
#include <DMenu.h>
#include <DUtil.h>


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


#if 0

#define  ShadeInvert()  { PaintRect(myRect);	TextMode(srcBIC); }

					kShadeInvert: BEGIN
						PaintRect(myRect); 
						TextMode(srcBIC);
						END;
#endif




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
			cShowCutpoints,cShowAllZymes,cShowExcludedCutters,cShowNoncutters
			};

	static char *gNameFontName, *gBaseFontName, *gIndexFontName;
	static Nlm_FonT	gNameFont, gBaseFont, gIndexFont;
	static short gNameFontSize, gBaseFontSize, gIndexFontSize;
	static short gNameStyle, gBaseStyle, gIndexStyle;
	static Boolean gNameLeft,gNameRight,gIndexLeft,gIndexRight,gIndexTop, gColored;
	static short gBasesPerLine;
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
	DSwitchBox	* fNameSizeSw, * fBaseSizeSw, *fIndexSizeSw;
	DEditText		* fREMinCuts, * fREMaxCuts, * fBasePerLine;
	Boolean	 		fNeedSave;
	
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

Global DSeqPrintPrefs* gSeqPrintPrefs = NULL;
 
char* DSeqPrintPrefs::gNameFontName = (char*) "times";
char* DSeqPrintPrefs::gBaseFontName = (char*) "courier";
char* DSeqPrintPrefs::gIndexFontName = (char*) "times";
Nlm_FonT 
			DSeqPrintPrefs::gNameFont = NULL, 
 			DSeqPrintPrefs::gBaseFont = NULL, 
  		DSeqPrintPrefs::gIndexFont = NULL;
short 
			DSeqPrintPrefs::gNameFontSize = 10, 
			DSeqPrintPrefs::gBaseFontSize = 10, 
			DSeqPrintPrefs::gIndexFontSize = 9,
			DSeqPrintPrefs::gNameStyle = 0,  
			DSeqPrintPrefs::gBaseStyle = 0,  
			DSeqPrintPrefs::gIndexStyle = 0;
Boolean 	
			DSeqPrintPrefs::gNameLeft = false,
			DSeqPrintPrefs::gNameRight = true,
			DSeqPrintPrefs::gIndexLeft = true,
			DSeqPrintPrefs::gIndexRight = false,
			DSeqPrintPrefs::gIndexTop = true, 
			DSeqPrintPrefs::gColored = true;

	// restmap prefs ..
Boolean  
			DSeqPrintPrefs::gShowComplement = true,
			DSeqPrintPrefs::gThreeLetAA = false,
			DSeqPrintPrefs::gOnlyORF = false,
			DSeqPrintPrefs::gShowAA1 = false,
			DSeqPrintPrefs::gShowAA2 = false,
			DSeqPrintPrefs::gShowAA3 = false,
			DSeqPrintPrefs::gShowCompAA1 = false,
			DSeqPrintPrefs::gShowCompAA2 = false,
			DSeqPrintPrefs::gShowCompAA3 = false,
			DSeqPrintPrefs::gShowAllZymes = false,
			DSeqPrintPrefs::gShowCutpoints = false,
			DSeqPrintPrefs::gShowExcludedCutters = false,
			DSeqPrintPrefs::gShowNoncutters = false;
short	
			DSeqPrintPrefs::gBasesPerLine = 60,
			DSeqPrintPrefs::gREMinCuts = 1, 
			DSeqPrintPrefs::gREMaxCuts = 999;

 




// add this to ?? dgg.c
inline void RectRgn(Nlm_RegioN rgn, Nlm_RectPtr r)
{
	Nlm_LoadRectRgn( rgn, r->left, r->top, r->right, r->bottom);
}

inline short BaseCharWidth()
{
	return Nlm_CharWidth('G');
	//return Nlm_MaxCharWidth(); 
	//return Nlm_stdCharWidth;
}


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

char DDrawMapRow::fLinebuf[DDrawMapRow::kMaxLinebuf+1];


class DDrawSpacer : public DDrawMapRow {
public:
	char *fTitle;
	DDrawSpacer( char* title= NULL, Nlm_FonT itsFont = NULL) : fTitle(title) { fFont= itsFont; }
	virtual void Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem);
	virtual const char* Write( short row, short col, long startitem, long stopitem);
	virtual short GetIndex(short item) { return kNoIndex; }
};

void DDrawSpacer::Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem)
{
	if (fTitle) {
		if (fFont) Nlm_SelectFont(fFont);
		Nlm_MoveTo( r.left, r.bottom-2);
		Nlm_PaintString(fTitle);
		}
}

const char* DDrawSpacer::Write( short row, short col, long startitem, long stopitem)
{ 
	long  len= Min( kMaxLinebuf, stopitem-startitem);
	Nlm_MemFill(fLinebuf, ' ', len);
	if (fTitle) MemCpy( fLinebuf, fTitle, Min( len, StrLen(fTitle)) );
	fLinebuf[len]= 0;
	return fLinebuf;
}




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

void DDrawIndexRow::Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem)
{  
	short atx, aty, cleft, rowtop;
	short up1, up2, upFont;
	
	if (col>0) startitem += col;
 
	if (fFont) Nlm_SelectFont(fFont);
	cleft= r.left; // add a bit to move over bases
	if (fIsUp) {
		up1= -1;
		up2= -3;
		upFont= -4;
		rowtop= r.bottom - kIndexRise; 
		}
	else {
		up1= 1;
		up2= 3;
		upFont= Nlm_FontHeight();
		rowtop= r.top + kIndexRise;
		}
	Nlm_MoveTo( cleft, rowtop);
	Nlm_LineTo( r.right, rowtop);
	
	cleft += fItemWidth / 2; // add a bit to put tics over bases
	for (long at= startitem; at <= stopitem; at++, cleft += fItemWidth) {
		atx= cleft;
		aty= rowtop;
		Nlm_MoveTo( atx, aty);
		if (at % 10 == 4) {
			char  nums[128];
			sprintf(nums, "%d", at+1);  
			short ws= Nlm_StringWidth(nums);
			aty += up1;
			Nlm_LineTo(atx, aty);
			aty += upFont;
			atx -= ws/2;
			Nlm_MoveTo( atx, aty);
			Nlm_PaintString(nums);
			}		
		else {
		  aty += up2;
			Nlm_LineTo(atx, aty);
			}
		}
}
	
const char*  DDrawIndexRow::Write( short row, short col, long startitem, long stopitem)
{ 
	long len, i, at;
	char	* numline, * ticline, *cp;
	
	len= Min( kMaxLinebuf / 2, stopitem-startitem);
	Nlm_MemFill( fLinebuf, ' ', (len + 1)*2);
	if (fIsUp) {
	  numline= fLinebuf;
		ticline= fLinebuf + len + 1;
		}
	else {
	  ticline= fLinebuf;
		numline= fLinebuf + len + 1;
		}
	
	for (i= 0, at=startitem; i < len; i++, at++) {
		if (at % 10 == 4) {
			char nums[128];
			short  ws;
			ticline[i]= '+';
		  sprintf(nums, "%d", at+1);
		  ws= StrLen(nums);
		  cp= numline + i - ws / 2;
		  MemCpy( cp, nums, ws);
			}
		//else if (fIsUp) ticline[i]= '_'; 
		else 
			ticline[i]= '-';
		}

		fLinebuf[len]= '\n'; // newline !?
		len= 2*len + 1;
		fLinebuf[len]= 0;
		return fLinebuf;
}
	
	

class DDrawSeqRow : public DDrawMapRow {
public:
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


DDrawSeqRow::DDrawSeqRow(Nlm_FonT itsFont, DSeqPrintView* itsView, DSequence* itsSeq,
					 								DList* itsStyles, char* name, ulong* colors) : 
		fView(itsView), fColors(colors), 
		fSeq(itsSeq), fTopSeq(NULL), fBotSeq(NULL),
		fStyles( itsStyles), 
		fFirstcommon(NULL), fCommonbase(NULL),
		fBases(NULL), fLength(0)
{  
	if (fSeq) {
		fBases= fSeq->Bases();
		fLength= fSeq->LengthF();
		}
	fFont= itsFont; 
	fName= name; 
	if (fFont) Nlm_SelectFont(fFont);
	fHeight= Nlm_LineHeight();
	fItemWidth= BaseCharWidth();
}
 


DSeqStyle* DDrawSeqRow::GetStyle(long ibase, short masklevel, DSequence* aSeq)
{
	short maskval= 0;
	if (aSeq) maskval= aSeq->MaskAt(ibase, masklevel);
	if (maskval>0)  
		return (DSeqStyle*) fStyles->At(masklevel-1);
	else
		return NULL;
}
 

void DDrawSeqRow::Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem)
{
	enum patvals { kNoPat, kHasFillPat, kHasFramePat };
	char* bases= fBases;
	if (bases && startitem < fLength) {
    char	ch, lastch, *b;
    short	atx, aty, masklevel, maskval;
    long	ibase;
    ulong	newcolor, curcolor, blackcolor;
    patvals  haspat= kNoPat;
 		DSeqStyle * style, * laststyle, * nextstyle, * topstyle, * botstyle;
   
		if (col>0) {
			bases += col; 
			startitem += col;
			}
		if (stopitem > fLength) stopitem= fLength;
		long len= stopitem - startitem;
		atx= r.left;
		aty= r.bottom - 2;
		
		if (fFont) Nlm_SelectFont(fFont);
		Nlm_Black();
		Nlm_TextTransparent();
		curcolor= blackcolor= Nlm_GetColor();
		
#if MASKS
		topstyle= botstyle= NULL;
		laststyle= nextstyle= NULL;
		if (startitem>0) 
		 for (masklevel=1; masklevel<5; masklevel++) {
			laststyle= GetStyle( startitem-1, masklevel, fSeq);
			if (laststyle) break;
			}
#endif
			
		lastch= 0;
		for (b= bases + startitem, ibase=startitem; *b && ibase<stopitem; b++, ibase++) {
			Nlm_MoveTo( atx, aty);
			ch= *b;
			if (ch == DSequence::indelSoft) ch= DSequence::indelHard; //looks better for output...
	
#if MASKS
			Boolean needdraw= true;
			Nlm_RecT crec;
			maskval= fSeq->MaskAt(ibase,0); // val for all masks
			if (maskval>0) 
			 for (masklevel=1; needdraw && masklevel<5; masklevel++) {
					// LATER: want to allow drawing of all mask forms for each base !?
				style= GetStyle( ibase, masklevel, fSeq);
				if (style) {
					Nlm_LoadRect( &crec, atx, r.top, atx+fItemWidth, r.bottom);
					nextstyle= GetStyle( ibase+1, masklevel, fSeq);
					topstyle= GetStyle( ibase, masklevel, fTopSeq);
					botstyle= GetStyle( ibase, masklevel, fBotSeq);

#if 1
					if (style->repeatchar && fItemrow > 0 
						&& fCommonbase && fFirstcommon
						&& fCommonbase[ibase] != '!'
						&& fItemrow > (unsigned char)fFirstcommon[ibase]
						&& toupper(ch) == fCommonbase[ibase]) 
						  ch= style->repeatchar;
#else					
					if (style->repeatchar && fItemrow > 0 
						&& DStyleTable::fToprow 
						&& ibase < DStyleTable::fToprowlen 
						&& ch == DStyleTable::fToprow[ibase]) 
						  ch= style->repeatchar;
#endif
						  
					if (style->uppercase) ch= toupper(ch);
					else if (style->lowercase) ch= tolower(ch);
					
					if (style->font && 
				    (style->font != Nlm_fontInUse || style->font != DStyleTable::fLaststyle.font)) 
						  Nlm_SelectFont(style->font);
						 
						// this is good only w/ backcolor !?
 					if (style->dofontpat) { 
						if ( haspat == kHasFillPat || 
						 (Nlm_MemCmp(style->fontpattern, DStyleTable::fLaststyle.fontpattern, 8) != 0)
						 ) {
						 	Nlm_PenPattern(style->fontpattern);  
 							haspat= kHasFillPat;
 							}
 						}
 					else if (haspat) { Nlm_Solid(); haspat= kNoPat; }
 							
					if (style->dobackcolor) { 
						newcolor= style->backcolor;
						if (curcolor != newcolor) {
							curcolor= newcolor;
							Nlm_SetColor( curcolor);  
							}
						Nlm_PaintRect(&crec);
						}  

					if (style->dofontcolor) newcolor= style->fontcolor;
					else if (fColors) newcolor= fColors[ch-' '];
					else newcolor= blackcolor;  
					if (curcolor != newcolor) {
						curcolor= newcolor;
						Nlm_SetColor( curcolor);  
						}

					//if (style->invertcolor) Nlm_InvertColors();
					//if (style->invertcolor) Nlm_EraseMode();//TextMode(srcBIC);
					
					Nlm_PaintChar(ch);
					needdraw= false;

					//if (style->invertcolor) Nlm_CopyMode();
					if (style->invertcolor) Nlm_InvertRect( &crec);
					
					if (style->frame) { 
						Boolean anyframe, leftframe, rightframe, topframe, botframe;
						leftframe = (!(laststyle && laststyle->frame));
						rightframe= (!(nextstyle && nextstyle->frame));
						topframe  = (!(topstyle && topstyle->frame));
						botframe  = (!(botstyle && botstyle->frame));
						anyframe  = leftframe||rightframe||topframe||botframe;
						
						if (anyframe) { 
						  if (style->doframecolor) newcolor= style->framecolor;
							else newcolor= blackcolor; 
							if (curcolor != newcolor) {
								curcolor= newcolor;
								Nlm_SetColor( curcolor);  
								}
		 					if (style->framestyle) { 
		 						switch (style->framestyle) {
		 							case DSeqStyle::kFrameSolid : Nlm_Solid(); break;
		 							case DSeqStyle::kFrameDark  : Nlm_Dark(); break;
		 							case DSeqStyle::kFrameMedium: Nlm_Medium(); break;
		 							case DSeqStyle::kFrameLight : Nlm_Light(); break;
		 							case DSeqStyle::kFrameDotted: Nlm_Dotted(); break;
		 							case DSeqStyle::kFrameDashed: Nlm_Dashed(); break;
		 							}
		 						}
		 					haspat= kHasFramePat;
							}

						if (leftframe) { 
							Nlm_MoveTo(crec.left,crec.top); 
							Nlm_LineTo(crec.left,crec.bottom); 
							} 
						if (rightframe) { 
							Nlm_MoveTo(crec.right-1,crec.top); 
							Nlm_LineTo(crec.right-1,crec.bottom); 
							} 
						if (topframe) { 
							Nlm_MoveTo(crec.left,crec.top); 
							Nlm_LineTo(crec.right,crec.top); 
							} 
						if (botframe) { 
							Nlm_MoveTo(crec.left,crec.bottom-1); 
							Nlm_LineTo(crec.right,crec.bottom-1); 
							} 
						if (anyframe) Nlm_TextTransparent(); // MSWin LineTo/DrawLine resets Opaque!
						}

					DStyleTable::fLaststyle= *style;
					laststyle= style;
					}
				}
			
			if (needdraw) {
				if (haspat) { Nlm_Solid(); haspat= kNoPat; }
	 			if (Nlm_fontInUse != fFont) Nlm_SelectFont(fFont); 
				if (fColors) {
					newcolor= fColors[ch-' '];
					if (curcolor != newcolor) {
						curcolor= newcolor;
						Nlm_SetColor( curcolor);  
						}
					}
				else if (curcolor != blackcolor) {
					Nlm_Black(); // if WithStyle left a color set.... !!
					curcolor= blackcolor;
					}
				Nlm_PaintChar(ch);
				laststyle= NULL;
				}

#else
 			if (fColors) {
	 			// if (swapBackcolor) Nlm_SetBackColor( fColors[ch-' ']); else 
				if (ch!=lastch) Nlm_SetColor( fColors[ch-' ']);
				}
			Nlm_PaintChar(ch);
#endif
			lastch= ch;
			atx += fItemWidth;   
			}	
		Nlm_TextOpaque();
		Nlm_Black();
		if (haspat) { Nlm_Solid(); haspat= kNoPat; }
		curcolor= blackcolor;
		}
}

const char* DDrawSeqRow::Write( short row, short col, long startitem, long stopitem)
{ 
	long  len, i;
	char* bases= fBases;
	len= Min( kMaxLinebuf, stopitem-startitem);
	if (bases && startitem < fLength) {
    char	ch, *b;
		if (stopitem > fLength) stopitem= fLength;		
		for (b= bases + startitem, i=0; *b && i<len; b++, i++) {
			ch= *b;
			if (ch == DSequence::indelSoft) ch= DSequence::indelHard; //looks better
			fLinebuf[i]= ch;
			}	
		for ( ; i<len; i++) fLinebuf[i]= ' ';
		}
	fLinebuf[len]= 0;
	return fLinebuf;
}
	
	

class DDrawManySeqRow : public DDrawSeqRow {
public:
	DSeqList * fSeqList;
	//short			 fLinesPerparag, fTopPerparag;
	
	DDrawManySeqRow(Nlm_FonT itsFont, DSeqPrintView* itsView, 
									DSeqList* seqlist, long seqIndex, DList* itsStyles, ulong* colors,
									char* commonbases, char* firstcommon);
#if 0
	virtual ~DDrawManySeqRow();
	virtual void Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem);	
	virtual const char* Write( short row, short col, long startitem, long stopitem);
	virtual char* GetName(short row);
#endif
};


DDrawManySeqRow::DDrawManySeqRow(Nlm_FonT itsFont, DSeqPrintView* itsView, 
											DSeqList* seqlist, long seqIndex, DList* itsStyles, ulong* colors,
											char* commonbases, char* firstcommon) : 
	  DDrawSeqRow( itsFont, itsView, NULL, itsStyles, NULL, colors),
		fSeqList(seqlist)
{
	fItemrow= seqIndex;
	fSeq= seqlist->SeqAt( fItemrow);
	if (fSeq) {
		fName= fSeq->Name();
		fBases= fSeq->Bases();
		fLength= fSeq->LengthF();
		}
	fTopSeq= seqlist->SeqAt(fItemrow-1);
	fBotSeq= seqlist->SeqAt(fItemrow+1);
	fCommonbase= commonbases;
	fFirstcommon= firstcommon;
	//fCommonbase= fSeqList->FindCommonBases(DSeqList::gMinCommonPercent, fFirstcommon);
}

#if 0
DDrawManySeqRow::~DDrawManySeqRow()
{
}

char* DDrawManySeqRow::GetName(short row)
{
	//DSequence* aSeq= NULL;
	//short atseq= (row % fLinesPerparag) - fTopPerparag;
	//if (fItemrow>=0) aSeq= (DSequence*) fSeqList->At( fItemrow);
	//if (fSeq) return fSeq->Name(); else 
	return fName;
}

void DDrawManySeqRow::Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem)
{
	//DSequence* aSeq= NULL;
//	short atseq= (row % fLinesPerparag) - fTopPerparag; // == fItemrow
	//if (fItemrow>=0) aSeq= (DSequence*) fSeqList->At(fItemrow);
	if (fSeq) {
		//fSeq= aSeq;
		//fTopSeq= (DSequence*) fSeqList->At(fItemrow-1);
		//fBotSeq= (DSequence*) fSeqList->At(fItemrow+1);
		//fBases= aSeq->Bases();
		//fLength= aSeq->LengthF();
	  DDrawSeqRow::Draw( r, row, col, startitem, stopitem);
		}
}

const char* DDrawManySeqRow::Write( short row, short col, long startitem, long stopitem)
{ 
	//DSequence* aSeq= NULL;
//	short atseq= (row % fLinesPerparag) - fTopPerparag;
	//if (fItemrow>=0) aSeq= (DSequence*) fSeqList->At(fItemrow);
	if (fSeq) {
		//fSeq= aSeq;
		//fBases= aSeq->Bases();
		//fLength= aSeq->LengthF();
	  return DDrawSeqRow::Write( row, col, startitem, stopitem);
		}
	else
		return "";
}

#endif



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


DDrawAminoRow::DDrawAminoRow(Nlm_FonT itsFont, DSequence* itsSeq, short frame) : 
		fAAseq(NULL), fFrame( frame), fLength(0), 
		fOnlyORF(DSeqPrintPrefs::gOnlyORF),
		fThreeLet(DSeqPrintPrefs::gThreeLetAA) 
{
	Boolean iscomp = frame > 2;
	short   offset = frame % 3;
	fFont= itsFont;
	if (fFont) Nlm_SelectFont(fFont);
	fHeight= Nlm_LineHeight();
	fItemWidth= BaseCharWidth();
	sprintf( fNameStore, "aa%d", frame);
	fName= fNameStore;
	if (iscomp) {
		itsSeq->SetSelection( -1,-1);
		itsSeq= itsSeq->Reverse();
			// strange patches...
		if (offset==1) offset= 2;	 
		else if (offset==2) offset= 1; 
		if (offset==2) itsSeq->InsertSpacers(itsSeq->LengthF(),offset,'?');   
		}
	itsSeq->SetSelection( offset, itsSeq->LengthF() - offset);
	fAAseq= itsSeq->Translate(false);
	if (iscomp) { delete itsSeq; itsSeq= NULL; }
	if (fAAseq) {
		fBases= fAAseq->Bases();
		fLength= fAAseq->LengthF();
		if (fOnlyORF) {
			long    i;
			Boolean isorf;
			char startamino= DCodons::startamino();
			for (isorf= true, i= 0; i<fLength; i++) {
				if (isorf) {
				  if (fBases[i] == '*') isorf= false;
				  }
				else {
				 	if (fBases[i] == startamino) isorf= true;
				 	if (!isorf) fBases[i]= ' ';
					}
				}
			}
		if (iscomp) { 
			fAAseq->SetSelection( -1,-1);
			DSequence* unrevAA= fAAseq->Reverse(); 
			delete fAAseq; 
			fAAseq= unrevAA; 
			fBases= fAAseq->Bases();
			fLength= fAAseq->LengthF();
			}
		}
}

void DDrawAminoRow::Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem)
{
	long   len;
	short  leftindent;
	char*  bases= fBases;
	if (bases) {
		if (col) {
			bases += col; 
			startitem += col;
			}
#if 0
  	short indents[3,3] = {
  	                    {0,1,2},
												{2,0,1},
											  {1,2,0}
												};
		// at   0 -> 0,1,2 now & want          0%3=0
		// at  50 -> 2,0,1 now -> 1,2,0 want  50%3=2
		// at 100 -> 1,2,0 now -> 2,0,1 want 100%3=1
		// at 150 -> 0,1,2 now & want        150%3=0
#endif
		leftindent=  (fFrame + (3 - (startitem % 3)) % 3) % 3; 
		
		len= (stopitem - startitem + 2) / 3;
		startitem = (startitem + 2) / 3;  // adjust for 3-to-1 NA-to-AA translate
		if (startitem + len > fLength) len= fLength - startitem;
		if (startitem < fLength) {
	    char	 *b;
	    short  chleft, chvert, chwidth;
			
			chvert= r.bottom-2;
			chwidth = fItemWidth; 
			chleft = r.left + chwidth * leftindent;	
			chwidth *= 3;	
			if (fFont) Nlm_SelectFont(fFont);
			if (fThreeLet) for (b= bases+startitem; *b && len; b++, len--) {
				Nlm_MoveTo( chleft, chvert);
				Nlm_PaintString( (char*) DSequence::Amino123(*b));
				chleft += chwidth;
				}
			else for (b= bases+startitem; *b && len; b++, len--) {
				Nlm_MoveTo( chleft, chvert);
				Nlm_PaintChar(*b);
				chleft += chwidth;
				}		
			}
		}
}

const char* DDrawAminoRow::Write( short row, short col, long startitem, long stopitem)
{ 
	long   len, alen;
	short  leftindent;
	char*  bases= fBases;
	if (bases) {
		len= Min( kMaxLinebuf, 1+stopitem-startitem);
		Nlm_MemFill( fLinebuf, ' ', len);
		
		leftindent=  (fFrame + (3 - (startitem % 3)) % 3) % 3; 
		
		alen= (stopitem - startitem + 2) / 3;
		startitem = (startitem + 2) / 3;  // adjust for 3-to-1 NA-to-AA translate
		if (startitem + alen > fLength) alen= fLength - startitem;
		if (startitem < fLength) {
	    char	*b, *cp;
	    long	i;
			i= leftindent;	
			if (fThreeLet) 
			 for (b= bases+startitem; *b && alen && i<len; b++, alen--, i += 3) {
				char * amino3= (char*) DSequence::Amino123(*b);
				cp= fLinebuf + i;
				MemCpy( cp, amino3, 3);
				}
			else 
			 for (b= bases+startitem; *b && alen && i<len; b++, alen--, i += 3) {
				fLinebuf[i]= *b;
				}		
			}
		}
	fLinebuf[len]= 0;
	return fLinebuf;
}
	


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


DDrawZymeTable::DDrawZymeTable(Nlm_FonT itsFont, DREMap * itsREMap, short rowoffset) :
		fLinelist(NULL)
{
  fName= "";
	fFont= itsFont;
	if (fFont) Nlm_SelectFont(fFont);
	fHeight= Nlm_LineHeight(); 
	fTableTitle= "TABLE of enzymes";
	fRowOffset= rowoffset;
}

DDrawZymeTable::~DDrawZymeTable() 
{ 
	for (short i=0; i<fLinecount; i++) MemFree( fLinelist[i]);
	MemFree( fLinelist); 
}


char* DDrawZymeTable::TableLine(short atline) 
{ 
	//if (atline == 0) return fTableTitle; else 
	if (atline>=0 && atline<fLinecount) return fLinelist[atline];  
	else return "";
}

void  DDrawZymeTable::Clip( Nlm_RecT& r, short row, long startitem, long stopitem,
										 Nlm_RecT& viewr, Nlm_RegioN cliprgn)
{
	//Nlm_ClipRgn( cliprgn);
	Nlm_RecT clipr= r;
	clipr.left= viewr.left + 10;
	clipr.right= viewr.right - 10;
	clipr.bottom= Min( r.bottom, viewr.bottom);
	clipr.top= Max(r.top, viewr.top);
	Nlm_ClipRect( &clipr);
}

void DDrawZymeTable::Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem)
{
	short	 chleft, chvert, atline;
	char  * tabline;
	
	chleft = r.left;
	chvert = r.bottom-2;
	atline = row - fRowOffset;
	tabline= TableLine(atline);
	if (tabline) {
		if (col) { 
		  if (StrLen(tabline)>col) tabline += col; // ??? offset TableLine by # cols ?
			else tabline= "";
			}
		if (fFont) Nlm_SelectFont(fFont);
	  if (atline == 0) {
			Nlm_MoveTo( chleft, chvert-1);
	 		Nlm_PaintString( tabline);
			Nlm_MoveTo( r.left, r.bottom-1);
			Nlm_LineTo( r.right, r.bottom-1);
	  	}
		else if (atline >= 0 && atline < fLinecount) {
			Nlm_MoveTo( chleft, chvert);
			Nlm_PaintString( tabline);
			}
		}
}

const char* DDrawZymeTable::Write( short row, short col, long startitem, long stopitem)
{ 
	long   len, atline;
	char   * cp;

	atline = row - fRowOffset;
	len= Min( kMaxLinebuf, 80);
  if (atline == 0) {
		Nlm_MemFill( fLinebuf, '_', len);
 		StrCpy( fLinebuf, TableLine(atline));
 		cp= StrChr(fLinebuf, '\0');
 		if (cp) *cp= ' ';
  	}
	else if (atline >= 0 && atline < fLinecount) {
		Nlm_MemFill( fLinebuf, ' ', len);
		cp = TableLine(atline);
		StrCpy( fLinebuf, cp);
 		cp= StrChr(fLinebuf, '\0');
 		if (cp) *cp= ' ';
		}
	else {
		Nlm_MemFill( fLinebuf, ' ', len);
		}
	fLinebuf[len]= 0;
	return fLinebuf;
}



DDrawAllZymeTable::DDrawAllZymeTable(Nlm_FonT itsFont, DREMap * itsREMap, short rowoffset) :
		DDrawZymeTable( itsFont, itsREMap, rowoffset)
{ 
	fTableTitle= "TABLE of all enzymes";
	if (1) {
		char  * zymeline = NULL;
		long  i, nzymes, linelen, linecount;
		char  buf[128], comma;

		nzymes= DREMap::fREnzymes->GetSize();
		linecount= 4 + nzymes / 4;
		fLinelist= (char**) MemNew( linecount * sizeof(char*)); 
		fLinecount= 0;
		sprintf( buf, "  TABLE of All Enzymes  (name: cut count)");
		fLinelist[fLinecount++]= StrDup(buf);
		//sprintf( buf, " ");
		//fLinelist[fLinecount++]= StrDup(buf);
	
		for (i=0, comma = ','; i<nzymes; i++) {
			if (i == nzymes-1) comma= ' ';
			DREnzyme* re= (DREnzyme*) DREMap::fREnzymes->At(i);
			if (re) {
				//sprintf( buf, "%10s:%3d%c  ", re->fName, re->fCutcount, comma);
				sprintf( buf, "%9s:%3d  ", re->fName, re->fCutcount, comma);
				if (zymeline)  
					zymeline= Dgg_StrExtendCat( &zymeline, buf);
				else {
					zymeline= StrDup(buf);
					}
			  linelen= StrLen(zymeline);
				if (linelen > 70) {
					fLinelist[fLinecount++]= zymeline;
					zymeline= NULL;
					}
				}

		  if (fLinecount >= linecount) {
		  	linecount += 10;
		  	fLinelist= (char**) Nlm_MemMore( fLinelist, linecount * sizeof(char*));
		  	}
			}
		if (zymeline) fLinelist[fLinecount++]= zymeline;
		}
}


DDrawNocutZymeTable::DDrawNocutZymeTable( Nlm_FonT itsFont, DREMap * itsREMap,
			 short rowoffset, short mincuts, short maxcuts) :
		DDrawZymeTable( itsFont, itsREMap, rowoffset)
{ 
	if (maxcuts) fTableTitle= "TABLE of excluded enzymes";
	else fTableTitle= "TABLE of non-cutting enzymes";
	if (1) {
		char  * zymeline = NULL;
		long  i, nzymes, linelen, linecount;
		char  buf[128], comma;
		Boolean dolist;
		
		nzymes= DREMap::fREnzymes->GetSize();
		linecount= 4 + nzymes / 6;
		fLinelist= (char**) MemNew( linecount * sizeof(char*)); 
		fLinecount= 0;
		if (maxcuts) sprintf( buf, "  TABLE of Excluded Enzymes");
		else sprintf( buf, "  TABLE of Noncutting Enzymes");
		fLinelist[fLinecount++]= StrDup(buf);
		if (maxcuts) {
			sprintf( buf, "  (cut with less than %d or more than %d cuts)", mincuts, maxcuts);
			fLinelist[fLinecount++]= StrDup(buf);
			}
			
		for (i=0, comma = ','; i<nzymes; i++) {
			//if (i == nzymes-1) comma= ' ';
			DREnzyme* re= (DREnzyme*) DREMap::fREnzymes->At(i);
			if (re) {
				if (maxcuts)  
					dolist= (re->fCutcount > 0 && (re->fCutcount < mincuts || re->fCutcount > maxcuts));  
				else 
					dolist= re->fCutcount == 0;
				if (dolist) {
					//sprintf( buf, "%9s%c ", re->fName,comma);
					sprintf( buf, "%9s ", re->fName);
					if (zymeline)  
						zymeline= Dgg_StrExtendCat( &zymeline, buf);
					else {
						zymeline= StrDup(buf);
						}
				  linelen= StrLen(zymeline);
					if (linelen > 60) {
						fLinelist[fLinecount++]= zymeline;
						zymeline= NULL;
						}
					}
				}

		  if (fLinecount >= linecount) {
		  	linecount += 10;
		  	fLinelist= (char**) Nlm_MemMore( fLinelist, linecount * sizeof(char*));
		  	}
			}
		if (zymeline) fLinelist[fLinecount++]= zymeline;
		}
}








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


#ifdef WIN_MSWIN
static int LIBCALLBACK
#else
static int
#endif
zymenameCompare(void* a, void* b)
{
	short diff = StringCmp(((DRECutsItem*)a)->fREnzyme->fName, ((DRECutsItem*)b)->fREnzyme->fName);
	if (diff == 0) {
		return ((DRECutsItem*)a)->fSeqIndex - ((DRECutsItem*)b)->fSeqIndex;
		}
	else 
		return diff;
}



DDrawZymeCutTable::DDrawZymeCutTable(Nlm_FonT itsFont, DREMap * itsREMap, short rowoffset) :
		DDrawZymeTable( itsFont, itsREMap, rowoffset),
		fCutList(NULL), fCutcount(0), fCuttersCount(0)
{ 
	fTableTitle= "TABLE of cut points";
	if (itsREMap) {
		fCutcount= itsREMap->fCutcount;
		fCuttersCount= itsREMap->fCuttersCount;
		fCutList= (DRECutsItem*) Nlm_MemDup( itsREMap->fSeqCuts, fCutcount * sizeof(DRECutsItem));
		if (fCutList) {
			long  i, at, lastat, linecount, linelen;
			char  *name, *zymeline = NULL, *lastname="";
			char  buf[128];
			
			Nlm_HeapSort( fCutList, fCutcount, sizeof(DRECutsItem), zymenameCompare);
			
			// what if we need more than one line for zyme cut list !!
			for (i=0, linecount= 0; i<fCutcount; i++) {
				name= fCutList[i].fREnzyme->fName;
				if (StringCmp(name, lastname)!=0) linecount++;
				lastname= name;
				}
		  linecount += 3;
			fLinelist= (char**) MemNew( linecount * sizeof(char*)); 
			
			fLinecount= 0;
			sprintf( buf, "  TABLE of Cut Points");
			fLinelist[fLinecount++]= StrDup(buf);
			//sprintf( buf, " ");
			//fLinelist[fLinecount++]= StrDup(buf);

		  linelen= 0;
			lastname= ""; lastat= -1;
			for (i=0; i<fCutcount && fLinecount < linecount; i++) {
				name= fCutList[i].fREnzyme->fName;
				at= fCutList[i].fSeqIndex;
				if (StringCmp(name, lastname)==0 && linelen<70) { 
					if (at != lastat) {
					  sprintf(buf, " %d", at);
						if (zymeline) { 
						  zymeline= Dgg_StrExtendCat( &zymeline, buf);
						  linelen= StrLen(zymeline);
						  }
					  }
					}
				else {
					if (zymeline) fLinelist[fLinecount++]= zymeline;
					sprintf(buf, "%s: %d", name, at);
					zymeline= (char*) MemNew(StrLen(buf)+1);
					Nlm_StringCpy(zymeline, buf);
					linelen= StrLen(buf);
					}
					
				lastname= name;
				lastat= at;
			  if (fLinecount >= linecount) {
			  	linecount += 10;
			  	fLinelist= (char**) Nlm_MemMore( fLinelist, linecount * sizeof(char*));
			  	}
				}
			if (zymeline) fLinelist[fLinecount++]= zymeline;
			}
		}
}


DDrawZymeCutTable::~DDrawZymeCutTable() 
{ 
	MemFree( fCutList); 
}

char* DDrawZymeCutTable::TableLine(short atline) 
{ 
	//if (atline == 0) return fTableTitle; else 
	if (atline>=0 && atline<fLinecount) {
		char * name = fLinelist[atline];
		char * cp= StrChr(name,':'); 
		if (cp) cp++; else cp= name;
		return cp;  
		}
	else return "";
}

char* DDrawZymeCutTable::GetName(short row) 
{ 
	enum { kNameLen = 80 };
	static char name[kNameLen];
	short atline = row - fRowOffset;
  if (atline == 0) {
		return "ENZYME";
		}
	else if (atline> 0 && atline < fLinecount) {
		StrNCpy(name, fLinelist[atline], kNameLen-1);
		name[kNameLen-1]= 0;
		char* cp= StrChr(name,':'); if (cp) *cp= 0;
		return name;
		}
	else
		return fName; 
}



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


DDrawZymeRow::DDrawZymeRow(Nlm_FonT itsFont, DSequence* itsSeq) : 
	  fSeq( itsSeq), fGoodMap(false), fCoSeq(NULL), fCutList(NULL),
	  fMinCuts(DSeqPrintPrefs::gREMinCuts), 
	  fMaxCuts(DSeqPrintPrefs::gREMaxCuts)
{ 
  fName= "zyme";
	fFont= itsFont;
	if (fFont) Nlm_SelectFont(fFont);
	fHeight= 4 * Nlm_LineHeight(); // ! need to calc line height for each row !?
	fREMap= new DREMap();
	if (fREMap) {
		fREMap->MapSeq(fSeq);
		fCoSeq= fREMap->fCoSeq;
		fCutList= fREMap->fSeqCuts;
		}
	fGoodMap= (fREMap && fCoSeq && fCutList);
}

void  DDrawZymeRow::Clip( Nlm_RecT& r, short row, long startitem, long stopitem,
										 Nlm_RecT& viewr, Nlm_RegioN cliprgn)
{
	//Nlm_ClipRgn( cliprgn);
	Nlm_RecT clipr= r;
	clipr.left= viewr.left + 10;
	clipr.right= viewr.right - 10;
	clipr.bottom= Min( r.bottom, viewr.bottom);
	clipr.top= Max(r.top, viewr.top);
	Nlm_ClipRect( &clipr);
}


#define znameCenter 0

short DDrawZymeRow::DrawOrMeasure( Nlm_Boolean doDraw, Nlm_RecT& r, short row, long startitem, long stopitem)
{
/*==== 
		gctcggctgctgctcggctg
				||        | ||
			 abcI    cbaII
									hcgX
====*/
 
	short		nameHeight, rowLeft, rowTop;
	short		chwidth, chleft, ncuts, cutindx;
	short		cuts, wd, ws, rowBot, lasti, i;
	Nlm_PoinT		at;
	Boolean 		first,overlap;
	char 			* lastzyme, * zymes;	
	Nlm_RecT		fillRect, bRect, cRect;
	Nlm_RegioN 	filledArea, aRgn;

	if (!fGoodMap) return 0;
	cutindx = 0; //cutindx= fLastZymeCut;
	fREMap->CutsAtBase( startitem, cutindx, ncuts);	//!? don't care if startbase has no cuts
	//fLastZymeCut= cutindx; // ????
	
	if (fFont) Nlm_SelectFont(fFont); 

	//atcellh= col;
	nameHeight= Nlm_FontHeight(); // cHeight;
	rowTop= r.top;
	rowLeft= r.left;
	rowBot= rowTop;
	chwidth= (r.right - r.left) / (stopitem - startitem); 
	
	filledArea= Nlm_CreateRgn();
	aRgn= Nlm_CreateRgn();
	
	chleft= rowLeft + kNucSpace;
	Nlm_LoadRect( &fillRect, chleft, rowTop, chleft+1, rowTop+1);
	RectRgn( filledArea, &fillRect); //must start w/ non-empty rgn
	Nlm_MoveTo( chleft, rowTop);
	for ( i= startitem; i<=stopitem; i++) {		
		while (fCutList[cutindx].fSeqIndex < i) cutindx++;  
		first= true;
		lasti= -1; 
		lastzyme= "";
		while (fCutList[cutindx].fSeqIndex == i) {
			zymes= fCutList[cutindx].fREnzyme->fName;
			cuts = fCutList[cutindx].fREnzyme->fCutcount;
			cutindx++;
			if (cuts < fMinCuts || cuts > fMaxCuts)  goto nextZyme;  
				//! Kludge til we fix REMap to stop dups
			if ( i==lasti && StringCmp(zymes,lastzyme)==0) goto nextZyme; 
			lasti= i; 
			lastzyme= zymes;
			
			if (first) {
				Nlm_MoveTo( chleft, rowTop);
				if (doDraw) Nlm_LineTo( chleft, rowTop+2/*kTicSize*/);
				else Nlm_MoveTo( chleft, rowTop+2);
				}

			ws= Nlm_StringWidth( zymes);
			wd= 2 + ws / 2;
			do {
				Nlm_GetPen( &at);
#if znameCenter
				Nlm_LoadRect( &cRect, at.x-wd, at.y, at.x+wd, at.y+nameHeight);
#else
				Nlm_LoadRect( &cRect, at.x-1, at.y, at.x+ws+1, at.y+nameHeight);
#endif
				overlap= Nlm_RectInRgn( &cRect, filledArea); 
			
				if (doDraw && overlap && !first) {
					//!? replace this w/ draw top to bottom of line, then overwrite zyme names 
					Nlm_LoadRect( &bRect, at.x, at.y-nameHeight, at.x+1, at.y);
					if (!Nlm_RectInRgn( &bRect, filledArea)) {
						Nlm_MoveTo( chleft, at.y-nameHeight+1);
						Nlm_LineTo( chleft, at.y);
						}
					}
				first= false;
				Nlm_MoveTo( chleft, at.y+nameHeight);
			} while (overlap);
	
			RectRgn( aRgn, &cRect);
			Nlm_UnionRgn(filledArea, aRgn, filledArea);
#if znameCenter
			Nlm_MoveTo( chleft - (ws / 2), at.y+nameHeight);
#else
			Nlm_MoveTo( chleft, at.y+nameHeight);
#endif
			if (doDraw) {
				//TextMode(srcCopy);  //erase line overlaps...
				Nlm_PaintString( zymes); 
				//TextMode(srcOr);
				}
			Nlm_GetPen( &at);
			rowBot = Max( rowBot, at.y);   
nextZyme:			
			lasti= i;
			}
			
		//spaceright();
		//chright= chleft + GetColWidth(atCellh) - fColInset;
		//chleft = chright + fColInset;
		//atcellh++;
		chleft += chwidth;
		}
	Nlm_DestroyRgn( filledArea);
	Nlm_DestroyRgn( aRgn);
	return (rowBot - rowTop + 2); //+ 12); /* lineHeight + fudge factor*/	
}




const char* DDrawZymeRow::Write( short row, short col, long startitem, long stopitem)
{ 
/*==== 
		gctcggctgctgctcggctg
				||        | ||
			 abcI    cbaII
									hcgX
====*/

	short		chleft, ncuts, cutindx;
	short		cuts, ws, lasti;
	Boolean 		first,overlap;
	char 			* lastzyme, * zymes;	

	short	linelen;
	long  len, i, nlines, maxlines;
	char ** lines;
	short zlen, zat, atleft, atline;


	if (!fGoodMap) return 0;
	len= Min( kMaxLinebuf, stopitem-startitem);
	Nlm_MemFill( fLinebuf, ' ', kMaxLinebuf);

	linelen= stopitem-startitem + 5; // +2;
	maxlines= kMaxLinebuf / linelen;
	lines= (char**) MemNew( (maxlines+1) * sizeof(char*));
	for (i=0; i<maxlines; i++) lines[i]= fLinebuf + (linelen * i);
	nlines= 0;
	
	cutindx = 0; //cutindx= fLastZymeCut;
	fREMap->CutsAtBase( startitem, cutindx, ncuts);	//!? don't care if startbase has no cuts
	//fLastZymeCut= cutindx; // ????
		
	for ( i= startitem; i<=stopitem; i++) {		
		while (fCutList[cutindx].fSeqIndex < i) cutindx++;  
		first= true;
		lasti= -1; 
		lastzyme= "";
		chleft= i - startitem;
		while (fCutList[cutindx].fSeqIndex == i) {
			zymes= fCutList[cutindx].fREnzyme->fName;
			cuts = fCutList[cutindx].fREnzyme->fCutcount;
			cutindx++;
			if (cuts < fMinCuts || cuts > fMaxCuts)  goto nextZyme;  
				//! Kludge til we fix REMap to stop dups
			if ( i==lasti && StringCmp(zymes,lastzyme)==0) goto nextZyme; 
			lasti= i; 
			lastzyme= zymes;
			
			if (first) {
				lines[0][chleft]= '|';
				first= false;
				}

			ws= StrLen( zymes);
#if znameCenter
			wd= ws / 2;
			atleft= chleft - wd;
#else
			atleft= chleft;
#endif
			atline= 1;
			do {
		  	for (zlen=0, zat=atleft, overlap= false; zlen<ws; zlen++, zat++) 
			  	if (lines[atline][zat] != ' ') { overlap= true; break; }
			  if (overlap) atline++;
			} while (overlap && atline<maxlines);
			MemCpy( lines[atline]+atleft, zymes, ws);
			if (++atline > nlines) nlines= atline;
			first= false;
			
nextZyme:			
			lasti= i;
			}
			
		}

	//return  (rowBot - rowTop + 12); /* lineHeight + fudge factor*/	

	for (i=1; i<nlines; i++) *(lines[i]-1)= '\n';
	if (nlines) lines[nlines-1][linelen-1]= 0;
	MemFree(lines);
	fLinebuf[kMaxLinebuf]= 0;
	return fLinebuf;
}



short DDrawZymeRow::GetHeight( Nlm_RecT& r, short row, long startitem, long stopitem)
{ 
	return DrawOrMeasure( false, r, row, startitem, stopitem);
}

void DDrawZymeRow::Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem)
{
	if (col) { startitem += col; } // ??? 
	(void) DrawOrMeasure( true, r, row, startitem, stopitem);
}





/******** 
methods & fields for various draw objects:
  single-seq
  multi-seq  
	single-seq [+rezymes] [+complement] [+amino-trans] [+... others]
  multi-seq  [+?consensus] [+?? annotations of various kinds for mseq ] 
  
  fSeqsPerParag
  fSeqLinesPerParag
  fSpacersPerParag
  
  DRowDrawer* GetRowDrawer( atseq):
		Spacer, TopIndex, 
		SeqLine(s), ComplSeqLine,
 		AA[1-3], CompAA[1-3], RestEnzyme
  
  make fRowsList [1..fLinesPerParag] of DRowDrawer objects...
  
  calcs:
    //linesPerParag = fSeqLinesPerParag + fSpacersPerParag
    atseq  = (row % fLinesPerParag)
		seqline= (row / fLinesPerParag) * seqsperparag;  
		startcol= seqline * fBasesPerLine;
		stopcol = startcol + fBasesPerLine;
*****/





// class DSeqPrintView

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

	virtual void MakeDrawList();
	virtual void Draw();
	virtual void DrawRow(Nlm_RecT r, short row);
	virtual void IndexFromRow( short row, long& itemrow, long& seqline, long& startitem, long& stopitem);
	virtual void GetReadyToShow();
	virtual void DrawSideIndex( Nlm_RecT& aRect, long atBase, long leftBorder);
	virtual void WriteSideIndex( long atBase, long leftBorder);
	virtual void DrawName( Nlm_RecT& aRect, short rightBorder, char* name);
	virtual void WriteName( short rightBorder, char* name);
	virtual short GetExtraRows() { return fExtrarows; }

	virtual void WriteTo(DFile* aFile);
	virtual	void WriteRow(short row);
};



DSeqPrintView::DSeqPrintView( long id, DView* itsSuper, DSeqPrintDoc* itsDocument,
								 DSeqList* itsSeqList, long firstbase, long nbases,
								 long pixwidth, long pixheight):
	DTableView( id, itsSuper, pixwidth, pixheight, 0, 0,  
							Nlm_stdCharWidth, Nlm_stdLineHeight, true, true),
	fDoc(itsDocument), fSeqList(itsSeqList), fSeq(NULL), 
	fDrawRowList(NULL), fStyles(NULL),
	fCommonbase(NULL), fFirstcommon(NULL),
	fFirstBase(firstbase), fNbases(nbases), fMaxbases(nbases),
	fSeqLinesPerParag(kSeqLinesPerParag), fTopPerparag(2),
	fLinesPerParag(kSeqLinesPerParag+2), fSeqWidth(0),
	fBasesPerLine(DSeqPrintPrefs::gBasesPerLine), fExtrarows(0),
	fIndexWidth(kIndexWidth), fIndexWidthCnt(5),
	fNameWidth(kNameWidth), fNameWidthCnt(8)

{ 
	this->SetSlateBorder( false);
	this->SetResize( DView::relsuper, DView::relsuper);
	this->SetTableFont(gTextFont);
	fBaseWidth= BaseCharWidth();
	fOneseq= fSeqList->GetSize()<2; 
	
  fSeq= (DSequence*) fSeqList->At(0);

	Initialize();  
}


DSeqPrintView::~DSeqPrintView()
{	
	fDrawRowList= FreeListIfObject(fDrawRowList);
	fStyles= FreeListIfObject(fStyles);
	MemFree( fCommonbase);
	MemFree( fFirstcommon);
}

void DSeqPrintView::Initialize()
{ 
	fDoTopIndex= DSeqPrintPrefs::gIndexTop;
	fDoLeftIndex= DSeqPrintPrefs::gIndexLeft; 
	fDoRightIndex= DSeqPrintPrefs::gIndexRight;  
	fDoLeftName= DSeqPrintPrefs::gNameLeft; 
	fDoRightName= DSeqPrintPrefs::gNameRight;
	
	fNameFont = DSeqPrintPrefs::gNameFont;
	fNumFont  = DSeqPrintPrefs::gIndexFont;
	fBaseFont = DSeqPrintPrefs::gBaseFont;
}


void DSeqPrintView::MakeDrawList()
{
	short i, n;
	fDrawRowList= FreeListIfObject(fDrawRowList);
	fDrawRowList= new DList(NULL, DList::kDeleteObjects);
	fDrawRowList->InsertLast( new DDrawSpacer());
	if (fDoTopIndex) 
		fDrawRowList->InsertLast( new DDrawIndexRow(fNumFont,fBaseWidth));

	ulong * colors;
	//if (!fDoc->fUseColor) colors= NULL;
	if (!DSeqPrintPrefs::gColored) colors= NULL;
	else if (fSeq->Kind() == DSequence::kAmino) colors= DBaseColors::gAAcolors;
	else colors= DBaseColors::gNAcolors;

	fStyles= FreeListIfObject(fStyles);
	fStyles= new DList(NULL,DList::kDeleteObjects);
	n= DStyleTable::fStyles->GetSize();
	for (i=0; i<n; i++) {
		DSeqStyle	*	astyle= (DSeqStyle*) DStyleTable::fStyles->At(i);
		astyle= (DSeqStyle*) astyle->Clone();
		fStyles->InsertLast( astyle);
		}
		
	if (!fOneseq) {
		fCommonbase= fSeqList->FindCommonBases(DSeqList::gMinCommonPercent, fFirstcommon);
		for (i= 0; i < fSeqLinesPerParag; i++) {
		  DDrawManySeqRow* dd= new DDrawManySeqRow( fBaseFont, this, fSeqList, i, 
		  		fStyles, colors, fCommonbase, fFirstcommon);
		  fDrawRowList->InsertLast(dd);
			}
		}
	else {
	  for (i= 0; i < fSeqLinesPerParag; i++) { 
			DDrawSeqRow* dd= new DDrawSeqRow( fBaseFont, this, fSeq, fStyles, fSeq->Name(), colors);
		  fDrawRowList->InsertLast( dd);
   		}	
    }
}


void DSeqPrintView::GetReadyToShow()
{
	char nums[50];
	long diff, nrows, nbases= 0;

	if (fOneseq)	{
		fSeqsperparag = fSeqLinesPerParag;
		}
  else {
		fSeqLinesPerParag= fSeqList->GetSize(); //?? or leave to subclasses?
		fSeqsperparag = 1;
		}
	/* if (dospacer) */ fLinesPerParag = 1; fTopPerparag= 1;
	if (fDoTopIndex) { fLinesPerParag++; fTopPerparag++; }
	fLinesPerParag += fSeqLinesPerParag;

#if 1
	if (fNbases) nbases= fNbases;
	else
#endif
	if (fSeqList) {
		short i, nseq= fSeqList->GetSize();
		for (i=0; i<nseq; i++) {
			DSequence* aseq= fSeqList->SeqAt(i);
			nbases= Max(nbases, aseq->LengthF());
			}
		fNbases= nbases; 
		}
	else if (fSeq) { nbases= fSeq->LengthF(); fNbases= nbases; }
	fMaxbases= nbases;
	
	// assume port is set??
	SelectFont();
	if (fNameFont) Nlm_SelectFont(fNameFont);  
	fNameWidth= 5 + Nlm_StringWidth("Sequence");
	fNameWidthCnt= 8;
	
	if (fNumFont) Nlm_SelectFont(fNumFont);  
	sprintf( nums, "%d", fMaxbases);
	fIndexWidthCnt= StrLen(nums);
	fIndexWidth= 5 + fIndexWidthCnt * Nlm_CharWidth('0');

	if (fBaseFont) Nlm_SelectFont(fBaseFont);  
	fBaseWidth= BaseCharWidth();  
	
	short ncols= fBasesPerLine + 4;
	if (ncols > GetMaxCols()) ChangeColSize( -1, ncols-GetMaxCols());

  MakeDrawList();

	SetItemWidth( 0, 1, (fDoLeftName)?fNameWidth:1);
	SetItemWidth( 1, 1, (fDoLeftIndex)?fIndexWidth:1);
	SetItemWidth( 2, fBasesPerLine, fBaseWidth);  
  fSeqWidth= fBasesPerLine * fBaseWidth; //!?!?
	SetItemWidth( fBasesPerLine+2, 1, (fDoRightIndex)?fIndexWidth:1);
	SetItemWidth( fBasesPerLine+3, 1, (fDoRightName)?fNameWidth:1);

	nrows= 1 + (nbases / fBasesPerLine) / fSeqsperparag;
	nrows *= fLinesPerParag;
	nrows += GetExtraRows();
	diff  = Max( 3, nrows) - GetMaxRows();
	ChangeRowSize( -1, diff);  // -1 prevents redraw..else use fMaxRows	

	Nlm_RecT  r;
	ViewRect(r);
	nrows= GetMaxRows();  
	for (long irow=0; irow<nrows; irow++) {
		long itemrow, seqline, startitem, stopitem;
		IndexFromRow( irow, itemrow, seqline, startitem, stopitem);
		DDrawMapRow* drawer= (DDrawMapRow*) fDrawRowList->At(itemrow);
		SetItemHeight( irow, 1, 
			(drawer) ? drawer->GetHeight(r, irow, startitem, stopitem) : Nlm_stdLineHeight);
		}
		
}


void DSeqPrintView::DrawSideIndex( Nlm_RecT& aRect, long index, long leftBorder)
{
	if (index != DDrawMapRow::kNoIndex) {
		char nums[128];
		Nlm_SelectFont(fNumFont); 
		sprintf(nums, "%d", index); // or char* nump= ltoa(index);
		short atx= aRect.right - leftBorder - Nlm_StringWidth(nums);
		Nlm_MoveTo( atx, aRect.bottom - kFontDescent);
		Nlm_PaintString(nums);
		}
}

void DSeqPrintView::WriteSideIndex( long index, long leftBorder)
{
	char fmt[30];
	if (index != DDrawMapRow::kNoIndex) {
		if (leftBorder > 0) sprintf( fmt, " %%%dd",fIndexWidthCnt);
		else sprintf( fmt, " %%-%dd",fIndexWidthCnt);
		fprintf( fFile->fFile, fmt, index);
		}
	else {
		sprintf( fmt, " %%%ds",fIndexWidthCnt);
		fprintf( fFile->fFile, fmt, "");
		}
}

void DSeqPrintView::DrawName( Nlm_RecT& aRect, short rightBorder, char* name)
{
	if (name) {
		Nlm_SelectFont(fNameFont); 
		short ht= 2 * Nlm_stdLineHeight;
		if (aRect.bottom - aRect.top > ht)
			Nlm_MoveTo( aRect.left + rightBorder, aRect.top + ht);
		else 	
			Nlm_MoveTo( aRect.left + rightBorder, aRect.bottom - kFontDescent);
		Nlm_PaintString( name);
		} 
}

void DSeqPrintView::WriteName( short rightBorder, char* name)
{
	char fmt[30];
	if (rightBorder > 0) sprintf( fmt, "  %%-%ds",fNameWidthCnt);
	else sprintf( fmt, "%%%ds ",fNameWidthCnt);
	if (!name) name= "";
	fprintf( fFile->fFile, fmt, name);
}


void DSeqPrintView::IndexFromRow( short row, long& itemrow, long& seqline, long& startitem, long& stopitem)
{
	itemrow	= (row % fLinesPerParag);  
	seqline = (row / fLinesPerParag) * fSeqsperparag;   
	if (fOneseq) seqline += Max( 0, itemrow - fTopPerparag);
#if 1
	startitem = fFirstBase + seqline * fBasesPerLine;
	stopitem  = Min(startitem + fBasesPerLine, fFirstBase+fNbases);
	if (startitem>stopitem) { startitem= stopitem; }
#else
	startitem = seqline * fBasesPerLine;
	stopitem  = startitem + fBasesPerLine;
#endif
}


void DSeqPrintView::Draw()
{
#if MASKS
			// !! toprowbases should be just CONSENSUS/Common bases, not rare ones
#if 0
	short minCommonPerCent= 70;
	char * commons= FindCommonBases( minCommonPerCent); 
	 // ^^ OOPs, this is made new for each draw .. bad news
	DStyleTable::StartDraw( commons, StrLen(commons));
#else
	//DSequence* aSeq= fSeqList->SeqAt(0);
	DStyleTable::StartDraw( fSeq->Bases(), fSeq->LengthF());
#endif
#endif

	DTableView::Draw();
	
#if MASKS
	DStyleTable::EndDraw();
#endif
}

void DSeqPrintView::DrawRow(Nlm_RecT r, short row)
{
	long 			itemrow, seqline, startitem, stopitem;
	short 		col, ncols;
	Nlm_RecT	r1, viewr;
	Nlm_RegioN 	cliprgn;
	DDrawMapRow * drawer;
	
	col = GetLeft(); 
	if (fWidths && col <= fMaxCols) r.right= r.left + fWidths[col];
	else r.right= r.left + fItemWidth;
	ViewRect( viewr);

	IndexFromRow( row, itemrow, seqline, startitem, stopitem);
  cliprgn= Nlm_CreateRgn();
	drawer= (DDrawMapRow*) fDrawRowList->At(itemrow);
  
	if (drawer) {
		drawer->fItemrow= itemrow - fTopPerparag;// so we know which seq line in array
		while (r.left < fRect.right && col < fMaxCols) {
			if (col < 2 || col > fBasesPerLine+1) {
						// do name & index borders
				ncols= 1;
				if (col == 0 && !fDoLeftName && drawer->DoLeftName()) // special case, fiddle w/ clip rgn...
					{ r.right += fItemWidth; }
				(void) Nlm_SectRect( &r, &viewr, &r1);
				RectRgn( cliprgn, &r1);  
				Nlm_SectRgn( cliprgn, Nlm_updateRgn, cliprgn);
				if ( !Nlm_EmptyRgn( cliprgn)) {
					Nlm_ClipRgn(cliprgn);
					r1= r;
					if (col == 0) {
						if (fDoLeftName || drawer->DoLeftName()) DrawName( r1, 0, drawer->GetName(row));
						}
					else if (col == 1) {
						if (fDoLeftIndex) DrawSideIndex( r1, drawer->GetIndex(startitem), kNucBorder);
						}
					else if (col == fBasesPerLine+2) {
						if (fDoRightIndex) DrawSideIndex( r1,  drawer->GetIndex(stopitem), 0);
						}
					else if (col == fBasesPerLine+3) {
						if (fDoRightName) DrawName( r1, kNucBorder, drawer->GetName(row));		
						}	
					}
				}
				
			else if (col < fBasesPerLine+2) {
				r.right= r.left + fSeqWidth;
				ncols= fBasesPerLine; //stopitem - startitem;
				(void) Nlm_SectRect( &r, &viewr, &r1);
				RectRgn( cliprgn, &r1);  
				Nlm_SectRgn( cliprgn, Nlm_updateRgn, cliprgn);
				if ( !Nlm_EmptyRgn( cliprgn)) {
					r1= r;
					//Nlm_ClipRgn(cliprgn);
					//drawer->fItemrow= itemrow - fTopPerparag;// so we know which seq line in array
					drawer->Clip( r1, row, startitem, stopitem, viewr, cliprgn);
					drawer->Draw( r1, row, col-2, startitem, stopitem);
					}
				}
		
		  col += ncols;
			r.left= r.right;
			if (fWidths) r.right += fWidths[col];
			else r.right += fItemWidth;
			}
		}
	//Nlm_ClipRgn(Nlm_updateRgn);
	fColsDrawn= col - GetLeft();
	Nlm_ResetClip();
	Nlm_DestroyRgn(cliprgn);
}


void DSeqPrintView::WriteTo(DFile* aFile)
{
	if (aFile) {
		long irow, nrows;
		fFile= aFile;
		fFile->Open("a");
		nrows= GetMaxRows();  
		for (irow=0; irow<nrows; irow++) WriteRow( irow);
		fFile->Close();
		}
}

void DSeqPrintView::WriteRow(short row)
{
	long 			itemrow, seqline, startitem, stopitem;
	short 		col, ncols;
	DDrawMapRow * drawer;
	char * newline, * atline;
	
	IndexFromRow( row, itemrow, seqline, startitem, stopitem);
	drawer= (DDrawMapRow*) fDrawRowList->At(itemrow);
	if (!drawer) return;
	drawer->fItemrow= itemrow - fTopPerparag;// so we know which seq line in array

	atline= (char*) drawer->Write( row, 0, startitem, stopitem);
	while (atline) {
		newline= StrChr(atline, '\n');
		if (newline) *newline++= 0; // risky: (const char*) drawer->Write()
		
		for (col=0, ncols=1; col<fMaxCols; col += ncols) {
			if (col < 2 || col > fBasesPerLine+1) {
						// do name & index borders
				ncols= 1;
				if (col == 0) {
					if (fDoLeftName || drawer->DoLeftName()) WriteName(  0, drawer->GetName(row));
					}
				else if (col == 1) {
					if (fDoLeftIndex) WriteSideIndex( drawer->GetIndex(startitem), kNucBorder);
					}
				else if (col == fBasesPerLine+2) {
					if (fDoRightIndex) WriteSideIndex(  drawer->GetIndex(stopitem), 0);
					}
				else if (col == fBasesPerLine+3) {
					if (fDoRightName) WriteName(  kNucBorder, drawer->GetName(row));		
					}	
				}
			else {
				ncols= fBasesPerLine; 
				fFile->WriteLine( " ", false);
				fFile->WriteLine( atline, false);
				}
			}
			
		atline= newline;
		fFile->WriteLine("\n");
		}
}









// class DSeqPrintDoc


Nlm_RecT DSeqPrintDoc::fgPrWinRect = { 0, 0, 0, 0 };

DSeqPrintDoc::DSeqPrintDoc( long id, DSeqDoc* itsDoc, DSeqList* itsSeqList, long firstbase, long nbases) :
		DWindow( kSeqPrintDoc, gApplication),
		fView(NULL),  
		fSeqList(itsSeqList),
		fFirstBase(firstbase), fNbases(nbases),
		//fHeadline(NULL), fFormatPop(NULL),
		//fLockButton(NULL), fColorButton(NULL), fMonoButton(NULL),
		fColorCheck(NULL), fLockCheck(NULL), 
		fUseColor(itsDoc->fUseColor)
{ 
	short width= -1, height= -1, left= -10, top= -20; // default window loc
	if (id != 0) fId= id;
	
	//gTextFont = Nlm_GetFont( "Courier", 10, false, false, false, "Fixed");
	//if (gTextFont == NULL) gTextFont= Nlm_programFont;

	if (!Nlm_EmptyRect(&fgPrWinRect))  {
		left= MAX(20,fgPrWinRect.left);
		top = MAX(40,fgPrWinRect.top);
		}
	
	char * winname= NULL;
	this->InitWindow( document, width, height, left, top); //, winname); 

	DSequence * aSeq;
	char title[256];
	if (!fSeqList) {
		fSeqList= new DSeqList();  
		fSeqList->AddNewSeq();	// install 1 blank seq for new doc
		aSeq= NULL;
		}
	else {
  	aSeq= (DSequence*) fSeqList->At(0);
		}
	if (aSeq)  
		StrCpy(title, aSeq->Name());
	else
		itsDoc->GetTitle( title, sizeof(title)); 
	DFileManager::ReplaceSuffix( title, sizeof(title), ".Print");
	SetTitle( title);
}


DSeqPrintDoc::~DSeqPrintDoc()
{
	if (fSeqList) { 
		// we don't own DSeq objects in this list !!
		//fSeqList->FreeAllObjects(); 
		fSeqList->suicide(); 
		}
}


void DSeqPrintDoc::MakeGlobalsCurrent()
{
	ViewRect( fViewrect); // get current rect...
	if (!Nlm_EmptyRect(&fViewrect)) fgPrWinRect= fViewrect;  
	//fgUseColor= fColorCheck->GetStatus();
}

void DSeqPrintDoc::Close()
{
	MakeGlobalsCurrent();
	DWindow::Close();
}

void DSeqPrintDoc::ResizeWin()
{
	DWindow::ResizeWin();
	MakeGlobalsCurrent();
}


// static
void DSeqPrintDoc::GetGlobals()
{

	//char* onoffs= (fgUseColor) ? "1" : "0";
	//fgUseColor= gApplication->GetPrefVal( "fgUseColor", "windows", onoffs);
	{
	char* srect = gApplication->GetPref( "fgPrWinRect", "windows", "30 40 450 220");
#if 1
		// sscanf is failing on Mac/codewar !! used to work
	if (srect) {
		char* cp= srect;
		while (*cp && isspace(*cp)) cp++;
		fgPrWinRect.left= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		fgPrWinRect.top= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		fgPrWinRect.right= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		fgPrWinRect.bottom= atoi( cp);
		}
#else
		if (srect) sscanf( srect, "%d%d%d%d", &fgPrWinRect.left, &fgPrWinRect.top, 
												&fgPrWinRect.right, &fgPrWinRect.bottom);
#endif
		Nlm_MemFree(srect);
	}
}

// static
void DSeqPrintDoc::SaveGlobals()
{
	//gApplication->SetPref( (int) fgUseColor, "fgUseColor","windows");
	if (!Nlm_EmptyRect(&fgPrWinRect)) {
		char  srect[128];
		sprintf( srect, "%d %d %d %d", fgPrWinRect.left, fgPrWinRect.top, 
														fgPrWinRect.right, fgPrWinRect.bottom);
		gApplication->SetPref( srect, "fgPrWinRect", "windows");
		}
}


void DSeqPrintDoc::Open()
{
	DView* 	super;
	short width, height;
	Nlm_PoinT		nps;
	
	//if (!fSeqList) fSeqList= new DSeqList();
	if (!fView) {
		super= this;
	
		width= 450; height= 350;
		super->GetNextPosition( &nps);
		width = MAX( 40, fgPrWinRect.right - fgPrWinRect.left) - Nlm_vScrollBarWidth  - nps.x; 
		height= MAX( 60, fgPrWinRect.bottom - fgPrWinRect.top) - Nlm_hScrollBarHeight - nps.y;   
		fView= new DSeqPrintView(0,super,this,fSeqList,fFirstBase,fNbases,width,height);
		}

	fSaveHandler= this;  
	fPrintHandler= this;
	
	this->Select(); // for motif
	this->CalcWindowSize();
	fView->GetReadyToShow();
	
	DWindow::Open();
}




void DSeqPrintDoc::WriteTo(DFile* aFile)  
{
		// offer user choice of Text or PICT (or other? - PS) output
		// need popup choice in save-as dialog...
	if (gKeys->shift())
		fView->WriteTo( aFile); // TEXT
	else
		fView->WriteToPICT( aFile); // PICT
}


void DSeqPrintDoc::Print()  
{
	fView->Print();
}












// class DAlnPrintView

class DAlnPrintView : public DSeqPrintView {
public:

	DAlnPrintView( long id, DView* itsSuper, DSeqPrintDoc* itsDocument,
								 DSeqList* itsSeqList, long firstbase, long nbases, long pixwidth, long pixheight);
};

DAlnPrintView::DAlnPrintView( long id, DView* itsSuper, DSeqPrintDoc* itsDocument,
								 DSeqList* itsSeqList, long firstbase, long nbases, long pixwidth, long pixheight):
		DSeqPrintView( id, itsSuper, itsDocument, itsSeqList, firstbase, nbases, pixwidth, pixheight)
{ 
	fSeqLinesPerParag= fSeqList->GetSize();
}

// class DAlnPrintDoc

DAlnPrintDoc::DAlnPrintDoc( long id, DSeqDoc* itsDoc, DSeqList* itsSeqList, long firstbase, long nbases) :
	DSeqPrintDoc( id, itsDoc, itsSeqList, firstbase, nbases)
{

	short width= 450, height= 350;
	//super->GetNextPosition( &nps);
	//width = MAX( 40, fgWinRect.right - fgWinRect.left) - Nlm_vScrollBarWidth  - nps.x; 
	//height= MAX( 60, fgWinRect.bottom - fgWinRect.top) - Nlm_hScrollBarHeight - nps.y;   
	fView= new DAlnPrintView(0,this,this,fSeqList,fFirstBase,fNbases,width,height);

	char title[256];
	itsDoc->GetTitle( title, sizeof(title)); 
	DFileManager::ReplaceSuffix( title, sizeof(title), ".Print");
	SetTitle( title);
}









// class DREMapPrintView

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
	virtual void IndexFromRow( short row, long& itemrow, long& seqline, long& startitem, long& stopitem);
};



DREMapPrintView::DREMapPrintView( long id, DView* itsSuper, DSeqPrintDoc* itsDocument,
								 DSeqList* itsSeqList, long firstbase, long nbases, long pixwidth, long pixheight):
	DSeqPrintView( id, itsSuper, itsDocument, itsSeqList, firstbase, 
							nbases, pixwidth, pixheight)
{ 
	Initialize();
}

void DREMapPrintView::IndexFromRow( short row, long& itemrow, long& seqline, long& startitem, long& stopitem)
{
		// !! modify itemrow to return drawer item row for tables after all of sequence as been drawn
  if (row > fMaxseqrow) {
#if 1
		short i, n = fDrawRowList->GetSize();
		itemrow= fLinesPerParag;
    seqline= row - fMaxseqrow - 2; //??
		for (i= fLinesPerParag; i<n; i++) {
			DDrawMapRow* drawer= (DDrawMapRow*) fDrawRowList->At(i);
			if (drawer && 
				  row >= drawer->fRowOffset && row < drawer->fRowOffset + drawer->fLinecount) { 
				  itemrow= i;
				  seqline= row - drawer->fRowOffset; //??
				  break; 
					}
			}
#else
    itemrow= fLinesPerParag; // need to adjust this for each spacer, etc...
  	if (row > fMaxseqrow + 1) itemrow++;
    seqline= row - fMaxseqrow - 2; //??
#endif
		startitem = seqline;
		stopitem  = startitem+1; //??
  	}
  else {
		itemrow	= (row % fLinesPerParag);  
		seqline = (row / fLinesPerParag) * fSeqsperparag;   
		if (fOneseq) seqline += Max( 0, itemrow - fTopPerparag);
#if 1
		startitem = fFirstBase + seqline * fBasesPerLine;
		stopitem  = Min(startitem + fBasesPerLine, fFirstBase+fNbases);
#else
		startitem = seqline * fBasesPerLine;
		stopitem  = startitem + fBasesPerLine;
#endif
		}
}

void DREMapPrintView::Initialize()
{
	DSeqPrintView::Initialize();

	fSeqLinesPerParag= 1;
	fOneseq= false;
	fDoMidIndex= fDoTopIndex;
	fDoTopIndex= false;
	fDoSeqLine= true;
	fDoCoseqLine= DSeqPrintPrefs::gShowComplement;
	fDoZymeLine= true;
  
	fDoAAline[0]= DSeqPrintPrefs::gShowAA1;
	fDoAAline[1]= DSeqPrintPrefs::gShowAA2;
	fDoAAline[2]= DSeqPrintPrefs::gShowAA3;
	fDoAAline[3]= DSeqPrintPrefs::gShowCompAA1;
	fDoAAline[4]= DSeqPrintPrefs::gShowCompAA2;
	fDoAAline[5]= DSeqPrintPrefs::gShowCompAA3;	
	
}

void DREMapPrintView::SetScrollPage()
{
	fItemHeight= Nlm_stdLineHeight;
	DTableView::SetScrollPage();
}

void DREMapPrintView::MakeDrawList()
{
	short i, n;
	ulong * colors = NULL;
	DDrawZymeRow * zymerow = NULL;
	DSequence* coseq = NULL;
	Boolean anyamino;
	
	fDrawRowList= FreeListIfObject(fDrawRowList);
	fDrawRowList= new DList(NULL, DList::kDeleteObjects);
	fDrawRowList->InsertLast(new DDrawSpacer());
	if (fDoTopIndex) 
		fDrawRowList->InsertLast(new DDrawIndexRow(fNumFont,fBaseWidth));

	if (fDoZymeLine) {
		fSeq->SetSelection(fFirstBase, fNbases);  // !? zyme map for only selected section
		zymerow= new DDrawZymeRow( fNameFont, fSeq);
		if (zymerow) {
			if (!zymerow->fGoodMap) { delete zymerow; zymerow= NULL; }
			else coseq= zymerow->fCoSeq;
			}
		}
	else if (fDoCoseqLine) {
		fSeq->SetSelection(0,0); // for Complement...
		coseq= fSeq->Complement();
		}
	
	anyamino= false;
	for (i=0; i<6; i++) if (fDoAAline[i]) anyamino= true;

	fStyles= FreeListIfObject(fStyles);
	fStyles= new DList(NULL,DList::kDeleteObjects);
	n= DStyleTable::fStyles->GetSize();
	for (i=0; i<n; i++) fStyles->InsertLast( DStyleTable::fStyles->At(i));
	
#if 1
	/* if (dospacer) */ fLinesPerParag = 1; fTopPerparag= 1;
	if (fDoTopIndex) { fLinesPerParag++; fTopPerparag++; }
	if (fDoSeqLine)
		fDrawRowList->InsertLast( 
			new DDrawSeqRow( fBaseFont, this, fSeq, fStyles, fSeq->Name(), colors));
	if (fDoSeqLine) { fLinesPerParag++;  }
	if (fDoMidIndex) 
		fDrawRowList->InsertLast(new DDrawIndexRow(fNumFont,fBaseWidth,false));
	if (fDoMidIndex) { fLinesPerParag++;  }
	if (fDoCoseqLine && coseq)
		fDrawRowList->InsertLast( 
			new DDrawSeqRow( fBaseFont, this, coseq, fStyles, "compl", colors));
	if (fDoCoseqLine && coseq) { fLinesPerParag++;}
	if (fDoZymeLine && zymerow)
		fDrawRowList->InsertLast( zymerow);
	if (fDoZymeLine && zymerow) { fLinesPerParag++; }
	if (anyamino)
		fDrawRowList->InsertLast(new DDrawSpacer()); fLinesPerParag++;
		
	for (i= 0; i<3; i++) if (fDoAAline[i])
		fDrawRowList->InsertLast( new DDrawAminoRow( fBaseFont, fSeq, i));
	for (i= 0; i<3; i++) if (fDoAAline[i]) { fLinesPerParag++;  }
	if (fDoMidIndex) 
		fDrawRowList->InsertLast(new DDrawIndexRow(fNumFont,fBaseWidth,false));
	if (fDoMidIndex) { fLinesPerParag++;  }
	for (i= 3; i<6; i++) if (fDoAAline[i] && coseq)
		fDrawRowList->InsertLast( new DDrawAminoRow( fBaseFont, coseq, i));
	for (i= 3; i<6; i++) if (fDoAAline[i]) fLinesPerParag++;

	fDrawRowList->InsertLast(new DDrawSpacer()); fLinesPerParag++;
	
#else
	for (i= 0; i<3; i++) if (fDoAAline[i])
		fDrawRowList->InsertLast( new DDrawAminoRow( fBaseFont, fSeq, i));
	if (fDoSeqLine)
		fDrawRowList->InsertLast( 
			new DDrawSeqRow( fBaseFont, this, fSeq, fStyles, fSeq->Name(), colors));
	if (fDoMidIndex) 
		fDrawRowList->InsertLast(new DDrawIndexRow(fNumFont,fBaseWidth,false));
	if (fDoMidIndex) { fLinesPerParag++;  }
	if (fDoCoseqLine && coseq)
		fDrawRowList->InsertLast( 
			new DDrawSeqRow( fBaseFont, this, coseq, fStyles, "compl", colors));
	for (i= 3; i<6; i++) if (fDoAAline[i] && coseq)
		fDrawRowList->InsertLast( new DDrawAminoRow( fBaseFont, coseq, i));
	if (fDoZymeLine && zymerow)
		fDrawRowList->InsertLast( zymerow);
	/* if (dospacer) */ fLinesPerParag = 1; fTopPerparag= 1;
	if (fDoTopIndex) { fLinesPerParag++; fTopPerparag++; }
	for (i= 0; i<3; i++) if (fDoAAline[i]) { fLinesPerParag++; fTopPerparag++; }
	if (fDoSeqLine) { fLinesPerParag++;  }
	if (fDoCoseqLine && coseq) { fLinesPerParag++;}
	for (i= 3; i<6; i++) if (fDoAAline[i]) fLinesPerParag++;
	if (fDoZymeLine) { fLinesPerParag++; }
#endif

	fMaxseqrow= 1 + (fMaxbases / fBasesPerLine);
	fMaxseqrow *= fLinesPerParag;
	if (zymerow && DSeqPrintPrefs::gShowCutpoints) {
		fDrawRowList->InsertLast(new DDrawSpacer()); fExtrarows++;
		DDrawZymeCutTable* ztab= new DDrawZymeCutTable( gTextFont/*fNumFont*/,  
																	zymerow->fREMap, fMaxseqrow+fExtrarows);  
		fDrawRowList->InsertLast( ztab);  
    fExtrarows += ztab->fLinecount + 1;
    }
	if (zymerow && DSeqPrintPrefs::gShowAllZymes) {
		fDrawRowList->InsertLast(new DDrawSpacer()); fExtrarows++;
		DDrawAllZymeTable* ztab= new DDrawAllZymeTable( gTextFont, zymerow->fREMap, 
																		fMaxseqrow+fExtrarows);  
		fDrawRowList->InsertLast( ztab);  
    fExtrarows += ztab->fLinecount + 1;
    }
	if (zymerow && DSeqPrintPrefs::gShowNoncutters) {
		fDrawRowList->InsertLast(new DDrawSpacer()); fExtrarows++;
		DDrawNocutZymeTable* ztab= new DDrawNocutZymeTable( gTextFont, zymerow->fREMap, 
																		fMaxseqrow+fExtrarows);  
		fDrawRowList->InsertLast( ztab);  
    fExtrarows += ztab->fLinecount + 1;
    }
	if (zymerow && DSeqPrintPrefs::gShowExcludedCutters) {
		fDrawRowList->InsertLast(new DDrawSpacer()); fExtrarows++;
		DDrawNocutZymeTable* ztab= new DDrawNocutZymeTable( gTextFont, zymerow->fREMap, 
																		fMaxseqrow+fExtrarows, 
																		DSeqPrintPrefs::gREMinCuts,
																		DSeqPrintPrefs::gREMaxCuts);  
		fDrawRowList->InsertLast( ztab);  
    fExtrarows += ztab->fLinecount + 1;
    }
	 
}



// class DREMapPrintDoc

DREMapPrintDoc::DREMapPrintDoc( long id, DSeqDoc* itsDoc, DSeqList* itsSeqList, long firstbase, long nbases) :
	DSeqPrintDoc( id, itsDoc, itsSeqList, firstbase, nbases)
{
	short width= 450, height= 350;
	//super->GetNextPosition( &nps);
	//width = MAX( 40, fgWinRect.right - fgWinRect.left) - Nlm_vScrollBarWidth  - nps.x; 
	//height= MAX( 60, fgWinRect.bottom - fgWinRect.top) - Nlm_hScrollBarHeight - nps.y;   
	fView= new DREMapPrintView(0,this,this,fSeqList,fFirstBase,fNbases,width,height);

	char title[256];
	DSequence * aSeq= (DSequence*) fSeqList->At(0);
	if (aSeq)  
		StrCpy(title, aSeq->Name());
	else
		itsDoc->GetTitle( title, sizeof(title)); 
	StrCat( title, " Restr. Map");
	SetTitle( title);
}












//class DSeqPrintPrefs : public DWindow 


 
DSeqPrintPrefs::DSeqPrintPrefs() :
	DWindow( 0, NULL, DWindow::fixed, -10, -10, -50, -20, "SeqPrint prefs", kDontFreeOnClose),
	fNameFontMenu(NULL), fBaseFontMenu(NULL),  fIndexFontMenu(NULL),
	fNameStyleMenu(NULL), fBaseStyleMenu(NULL),  fIndexStyleMenu(NULL),
	fNameSizeSw(NULL), fBaseSizeSw(NULL),  fIndexSizeSw(NULL),
	fREMinCuts(NULL),  fREMaxCuts(NULL), fBasePerLine(NULL),
	fNeedSave(false)
{	
}

DSeqPrintPrefs::~DSeqPrintPrefs()
{
}


inline Nlm_FonT GetAFont( char* fname, short fsize, short fstyle)
{
	if (StrICmp(fname,"System")==0) return Nlm_systemFont;
	else if (StrICmp(fname,"Program")==0) return Nlm_programFont;
	else return Nlm_GetFont( fname, fsize, 
	         fstyle & kBold, fstyle & kItalic, fstyle & kUnderline, NULL);
}

// static
void DSeqPrintPrefs::InitGlobals() 
{ 

	gNameFontName= gApplication->GetPref( "gNameFontName", "fonts", gNameFontName);
	gNameFontSize= gApplication->GetPrefVal( "gNameFontSize", "fonts", "10");
	gNameStyle= gApplication->GetPrefVal( "gNameStyle", "fonts", "0");
	gNameFont= ::GetAFont(gNameFontName,gNameFontSize, gNameStyle);

	gBaseFontName= gApplication->GetPref( "gBaseFontName", "fonts", gBaseFontName);
	gBaseFontSize= gApplication->GetPrefVal( "gBaseFontSize", "fonts", "10");
	gBaseStyle= gApplication->GetPrefVal( "gBaseStyle", "fonts", "0");
	gBaseFont= ::GetAFont(gBaseFontName,gBaseFontSize, gBaseStyle);

	gIndexFontName= gApplication->GetPref( "gIndexFontName", "fonts", gIndexFontName);
	gIndexFontSize= gApplication->GetPrefVal( "gIndexFontSize", "fonts", "9");
	gIndexStyle= gApplication->GetPrefVal( "gIndexStyle", "fonts", "0");
	gIndexFont= ::GetAFont(gIndexFontName,gIndexFontSize, gIndexStyle);
	
	gNameLeft= gApplication->GetPrefVal( "gNameLeft", "seqprint","0");
	gNameRight= gApplication->GetPrefVal( "gNameRight", "seqprint","1");
	gIndexLeft= gApplication->GetPrefVal( "gIndexLeft", "seqprint","1");
	gIndexRight= gApplication->GetPrefVal( "gIndexRight", "seqprint","0");
	gIndexTop= gApplication->GetPrefVal( "gIndexTop", "seqprint","1");
	gColored= gApplication->GetPrefVal( "gColored", "seqprint","1");
	gBasesPerLine= gApplication->GetPrefVal( "gBasesPerLine", "seqprint","60");
  
	gShowComplement= gApplication->GetPrefVal( "gShowComplement", "seqprint","1");
	gThreeLetAA= gApplication->GetPrefVal( "gThreeLetAA", "seqprint","0");
	gOnlyORF= gApplication->GetPrefVal( "gOnlyORF", "seqprint","0");
	gShowAA1= gApplication->GetPrefVal( "gShowAA1", "seqprint","0");
	gShowAA2= gApplication->GetPrefVal( "gShowAA2", "seqprint","0");
	gShowAA3= gApplication->GetPrefVal( "gShowAA3", "seqprint","0");
	gShowCompAA1= gApplication->GetPrefVal( "gShowCompAA1", "seqprint","0");
	gShowCompAA2= gApplication->GetPrefVal( "gShowCompAA2", "seqprint","0");
	gShowCompAA3= gApplication->GetPrefVal( "gShowCompAA3", "seqprint","0");

	gShowAllZymes= gApplication->GetPrefVal( "gShowAllZymes", "remap","0");
	gShowCutpoints= gApplication->GetPrefVal( "gShowCutpoints", "remap","0");
	gShowNoncutters= gApplication->GetPrefVal( "gShowNoncutters", "remap","0");
	gShowExcludedCutters= gApplication->GetPrefVal( "gShowExcludedCutters", "remap","0");

	gREMinCuts= gApplication->GetPrefVal( "gREMinCuts", "remap","1");
	gREMaxCuts= gApplication->GetPrefVal( "gREMaxCuts", "remap","999");

#if 0
	{
	char* srect = gApplication->GetPref( "gSeqPrintDocRect", "windows", "20 20 450 220");
		// sscanf is failing on Mac/codewar !? used to work
	if (srect) {
		char* cp= srect;
		while (*cp && isspace(*cp)) cp++;
		gSeqPrintDocRect.left= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		gSeqPrintDocRect.top= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		gSeqPrintDocRect.right= atoi( cp);
		
		while (*cp && !isspace(*cp)) cp++;
		while (*cp && isspace(*cp)) cp++;
		gSeqPrintDocRect.bottom= atoi( cp);
		}
	MemFree(srect);
	}	
#endif

}


//static
void DSeqPrintPrefs::SaveGlobals() 
{
	gApplication->SetPref( gNameFontName, "gNameFontName", "fonts");
	gApplication->SetPref( gNameFontSize, "gNameFontSize", "fonts");
	gApplication->SetPref( gNameStyle, "gNameStyle", "fonts");
	gApplication->SetPref( gBaseFontName, "gBaseFontName", "fonts");
	gApplication->SetPref( gBaseFontSize, "gBaseFontSize", "fonts");
	gApplication->SetPref( gBaseStyle, "gBaseStyle", "fonts");
	gApplication->SetPref( gIndexFontName, "gIndexFontName", "fonts");
	gApplication->SetPref( gIndexFontSize, "gIndexFontSize", "fonts");
	gApplication->SetPref( gIndexStyle, "gIndexStyle", "fonts");
	
	gApplication->SetPref( gNameLeft, "gNameLeft", "seqprint");
	gApplication->SetPref( gNameRight, "gNameRight", "seqprint");
	gApplication->SetPref( gIndexLeft, "gIndexLeft", "seqprint");
	gApplication->SetPref( gIndexRight, "gIndexRight", "seqprint");
	gApplication->SetPref( gIndexTop, "gIndexTop", "seqprint");
	gApplication->SetPref( gColored, "gColored", "seqprint");
	gApplication->SetPref( gBasesPerLine, "gBasesPerLine", "seqprint");
	
// restrict map
	gApplication->SetPref( gShowComplement, "gShowComplement", "seqprint");
	gApplication->SetPref( gThreeLetAA, "gThreeLetAA", "seqprint");
	gApplication->SetPref( gOnlyORF, "gOnlyORF", "seqprint");
	
	gApplication->SetPref( gShowAA1, "gShowAA1", "seqprint");
	gApplication->SetPref( gShowAA2, "gShowAA2", "seqprint");
	gApplication->SetPref( gShowAA3, "gShowAA3", "seqprint");
	gApplication->SetPref( gShowCompAA1, "gShowCompAA1", "seqprint");
	gApplication->SetPref( gShowCompAA2, "gShowCompAA2", "seqprint");
	gApplication->SetPref( gShowCompAA3, "gShowCompAA3", "seqprint");

// rest. map tables
	gApplication->SetPref( gShowAllZymes, "gShowAllZymes", "remap");
	gApplication->SetPref( gShowCutpoints, "gShowCutpoints", "remap");
	gApplication->SetPref( gShowNoncutters, "gShowNoncutters", "remap");
	gApplication->SetPref( gShowExcludedCutters, "gShowExcludedCutters", "remap");
	gApplication->SetPref( gREMinCuts, "gREMinCuts", "remap");
	gApplication->SetPref( gREMaxCuts, "gREMaxCuts", "remap");

}




void DSeqPrintPrefs::NewFontCluster(char* title, DView* mainview,
					DPopupMenu*& mfont, DPopupMenu*& mstyle, DSwitchBox*& swsize,
					char* fontname, short fontstyle, short fontsize )
{	
		// Name style -- font, font size, font style
	DPopupMenu* popm;
	DSwitchBox* sw;
	DPrompt* pr;
	DView* super;
	
	super= mainview;
	super->NextSubviewBelowLeft();

	DCluster* maincluster= new DCluster( 0, super, 30, 10, true, NULL); //false, title);
	super= maincluster;

	popm= new DPopupMenu( 0, (Nlm_GrouP)super->GetNlmObject(), "Font  ");
	mfont= popm;
	popm->AddFonts();
	popm->SetFontChoice( fontname); 
	//super->NextSubviewToRight();

	popm= new DPopupMenu( 0, (Nlm_GrouP)super->GetNlmObject(), "Style  ");
	mstyle= popm;
	popm->AddItem( kStylePlain, "Plain", true);
	popm->AddItem( kStyleItalic, "Italic", true);
	popm->AddItem( kStyleBold, "Bold", true);
	popm->AddItem( kStyleUnderline, "Underline", true);
	popm->SetItemStatus( kStylePlain, fontstyle==0);
	popm->SetItemStatus( kStyleItalic, fontstyle & kItalic);
	popm->SetItemStatus( kStyleBold, fontstyle & kBold);
	popm->SetItemStatus( kStyleUnderline, fontstyle & kUnderline);
	//super->NextSubviewToRight();
	
	DCluster* cluster= new DCluster(0, super, 30, 10, true, NULL);
	super= cluster;
	
	pr= new DPrompt( 0, super, "font size", 0, 0, Nlm_programFont);	 		
	//super->NextSubviewToRight();
	super->NextSubviewBelowLeft();
	sw = new DSwitchBox(0, super, true, true);
	swsize= sw;
	sw->SetValues(fontsize,99);
	//super->NextSubviewBelowLeft();

	super= mainview;
	super->NextSubviewBelowLeft();
}


void DSeqPrintPrefs::Initialize() 
{ 
	DView* super;
	DPrompt* pr;
	DCheckBox* ck;
	DCluster* clu;
	char nums[128];	

	super= this;
	clu= new DCluster( 0, super, 30, 10, false, "Bases");
	super= clu;

	NewFontCluster( "Base style", super, fBaseFontMenu, fBaseStyleMenu, fBaseSizeSw,
							gBaseFontName, gBaseStyle, gBaseFontSize);

	ck= new DCheckBox(cColored, super, "Color ");
	ck->SetStatus(gColored);
	super->NextSubviewToRight();
	
	sprintf( nums, "%d", gBasesPerLine);
	fBasePerLine= new DEditText( 0, super, nums, 4, Nlm_programFont); 
	this->SetEditText( fBasePerLine);
	super->NextSubviewToRight();
	pr= new DPrompt( 0, super, "bases/line", 0, 0, Nlm_programFont);	 		

	super = this;
	//super->NextSubviewToRight();
	//super->NextSubviewBelowLeft();

	clu= new DCluster( 0, super, 30, 10, false, "Indices");
	super= clu;

	NewFontCluster( "Index style", super, fIndexFontMenu, fIndexStyleMenu, fIndexSizeSw,
							gIndexFontName, gIndexStyle, gIndexFontSize);

	ck= new DCheckBox(cIndexLeft, super, "Left");
	ck->SetStatus(gIndexLeft);
	ck= new DCheckBox(cIndexRight, super, "Right");
	ck->SetStatus(gIndexRight);
	ck= new DCheckBox(cIndexTop, super, "Top");
	ck->SetStatus(gIndexTop);

	super = this;
	//super->NextSubviewToRight();
	//super->NextSubviewBelowLeft();

	clu= new DCluster( 0, super, 30, 10, false, "Names");
	super= clu;

	NewFontCluster( "Name style", super, fNameFontMenu, fNameStyleMenu, fNameSizeSw,
							gNameFontName, gNameStyle, gNameFontSize);

	ck= new DCheckBox(cNameLeft, super, "Left");
	ck->SetStatus(gNameLeft);
	ck= new DCheckBox(cNameRight, super, "Right");
	ck->SetStatus(gNameRight);
	
	super = this;
	//super->NextSubviewToRight();
	//super->NextSubviewBelowLeft();

#if 1		

				// turn this into 2nd dialog window...
				
	clu= new DCluster( 0, super, 30, 10, false, "Restriction Maps");
	super= clu;

	ck= new DCheckBox(cShowComplement, super, "Show Complement");
	ck->SetStatus(gShowComplement);
	super->NextSubviewBelowLeft();
	
	DCluster* pclu= new DCluster( 0, super, 30, 10, false, "Protein translate frame");
	super= pclu;
	pr= new DPrompt( 0, super, " sequence", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
	ck= new DCheckBox(cShowAA1, super, "1st");
	ck->SetStatus(gShowAA1);
	super->NextSubviewToRight();
	ck= new DCheckBox(cShowAA2, super, "2nd");
	ck->SetStatus(gShowAA2);
	super->NextSubviewToRight();
	ck= new DCheckBox(cShowAA3, super, "3rd");
	ck->SetStatus(gShowAA3);
	super->NextSubviewBelowLeft();

	pr= new DPrompt( 0, super, "complement", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
	ck= new DCheckBox(cShowCompAA1, super, "1st");
	ck->SetStatus(gShowCompAA1);
	super->NextSubviewToRight();
	ck= new DCheckBox(cShowCompAA2, super, "2nd");
	ck->SetStatus(gShowCompAA2);
	super->NextSubviewToRight();
	ck= new DCheckBox(cShowCompAA3, super, "3rd");
	ck->SetStatus(gShowCompAA3);

	super->NextSubviewBelowLeft();
	ck= new DCheckBox(cThreeLetAA, super, "3-letter code");
	ck->SetStatus(gThreeLetAA);
	super->NextSubviewToRight();
	ck= new DCheckBox(cOnlyORF, super, "ORFs only");
	ck->SetStatus(gOnlyORF);
	super= clu;
	super->NextSubviewBelowLeft();

	DCluster* rclu= new DCluster( 0, super, 30, 10, false, "Restriction enzymes");
	super= rclu;

	pr= new DPrompt( 0, super, "Min. cuts", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
	sprintf( nums, "%d", gREMinCuts);
	fREMinCuts= new DEditText( 0, super, nums, 4, Nlm_programFont); 
	this->SetEditText( fREMinCuts);
	pr= new DPrompt( 0, super, "Max. cuts", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
	sprintf( nums, "%d", gREMaxCuts);
	fREMaxCuts= new DEditText( 0, super, nums, 4, Nlm_programFont); 
	super->NextSubviewBelowLeft();

	pr= new DPrompt( 0, super, "Tables", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
	ck= new DCheckBox(cShowCutpoints, super, "Cutpoints");
	ck->SetStatus(gShowCutpoints);
	ck= new DCheckBox(cShowAllZymes, super, "All zymes");
	ck->SetStatus(gShowAllZymes);
	super->NextSubviewBelowLeft();
	ck= new DCheckBox(cShowNoncutters, super, "Noncutters");
	ck->SetStatus(gShowNoncutters);
	ck= new DCheckBox(cShowExcludedCutters, super, "Excluded cutters");
	ck->SetStatus(gShowExcludedCutters);

	super= clu;

	super= this;
#endif

	this->AddOkayCancelButtons();
	
}


void DSeqPrintPrefs::OkayAction() 
{ 
	short	 		aSize, aStyle;
	char			name[256], *nums;
	DMenu 	*	aFontMenu, * aStyleMenu;
	Nlm_FonT	aFont;
	
	if (fREMinCuts) {
		nums= fREMinCuts->GetTitle(name, sizeof(name));
		gREMinCuts= atol(nums);
		}
	if (fREMaxCuts) {
		nums= fREMaxCuts->GetTitle(name, sizeof(name));
		gREMaxCuts= atol(nums);
		}
	if (fBasePerLine) {
		nums= fBasePerLine->GetTitle(name, sizeof(name));
		gBasesPerLine= atol(nums);
		}
	
	for (short imenu= 0; imenu<3; imenu++) {
		
		switch (imenu) {
		  case 0:
				aFontMenu= fNameFontMenu; 
				aStyleMenu= fNameStyleMenu; 
				aSize= fNameSizeSw->GetValue();// aSize= gNameFontSize;
		  	break;
		  case 1:
				aFontMenu= fBaseFontMenu; 
				aStyleMenu= fBaseStyleMenu; 
				aSize= fBaseSizeSw->GetValue();// aSize= gBaseFontSize;
		  	break;
		  case 2:
				aFontMenu= fIndexFontMenu; 
				aStyleMenu= fIndexStyleMenu; 
				aSize= fIndexSizeSw->GetValue();// aSize= gIndexFontSize;
		  	break;
		  }
			
		if (aFontMenu && aFontMenu->GetFontChoice(name, sizeof(name))) {
			aStyle= 0;
#ifdef WIN_MAC
			if (!aStyleMenu->GetItemStatus(kStylePlain)) {
				if (aStyleMenu->GetItemStatus(kStyleItalic   )) aStyle |= kItalic;
				if (aStyleMenu->GetItemStatus(kStyleBold     )) aStyle |= kBold;
				if (aStyleMenu->GetItemStatus(kStyleUnderline)) aStyle |= kUnderline;
				}
#else
 			short item= Nlm_GetValue(((DPopupMenu*)aStyleMenu)->fPopup);
 			switch (item) {
 				case 1: aStyle= 0; break;
 				case 2: aStyle |= kItalic; break;
 				case 3: aStyle |= kBold; break;
 				case 4: aStyle |= kUnderline; break;
         }
#endif 			
			if (StringCmp(name,"System")==0) aFont= Nlm_systemFont;
			else if (StringCmp(name,"Program")==0) aFont= Nlm_programFont;
			else aFont= Nlm_GetFont( name, aSize, aStyle & kBold, 
													aStyle & kItalic, aStyle & kUnderline, NULL);
			
			switch (imenu) {
			  case 0: 
			  if (aSize != gNameFontSize || aStyle != gNameStyle || StringCmp(name,gNameFontName)!=0) {
					if (gNameFontName) MemFree(gNameFontName);
					gNameFontName= StrDup(name);
					gNameFont= aFont;
					gNameFontSize= aSize;
					gNameStyle= aStyle;
					fNeedSave= true;
					}
			  	break;
			  	
			  case 1: 
			  if (aSize != gBaseFontSize || aStyle != gBaseStyle || StringCmp(name,gBaseFontName)!=0) {
					if (gBaseFontName) MemFree(gBaseFontName);
					gBaseFontName= StrDup(name);
					gBaseFont= aFont;
					gBaseFontSize= aSize;
					gBaseStyle= aStyle;
					fNeedSave= true;
					}
			  	break;
			  	
			  case 2: 
			  if (aSize != gIndexFontSize || aStyle != gBaseStyle || StringCmp(name,gIndexFontName)!=0) {
					if (gIndexFontName) MemFree(gIndexFontName);
					gIndexFontName= StrDup(name);
					gIndexFont= aFont;
					gIndexFontSize= aSize;
					gIndexStyle= aStyle;
					fNeedSave= true;
					}
			  	break;
			  }
			  
			}
		}
	
#if 0
#if 1
	// CurrentWindow gets prefs dlog window, not one we want	
	//DSeqPrintDoc* awin= (DSeqPrintDoc*) gWindowManager->CurrentWindow();
	DList* wins= gWindowManager->GetWindowList();
	if (wins) {
		short last1= wins->GetSize() - 2;
		if (last1>0) {
			DSeqPrintDoc* awin= (DSeqPrintDoc*) wins->At(last1);
			if (awin && awin->Id() == DSeqPrintDoc::kSeqPrintDoc) {
				awin->fView->Initialize();
				awin->fView->GetReadyToShow();
				awin->fView->Invalidate();
				}
			}
		}
#else	
	DList* wins= gWindowManager->GetWindowList();
	if (wins) {
		long i, nwin= wins->GetSize();
		for (i= nwin-1; i>=0; i--) {
			DSeqPrintDoc* awin= (DSeqPrintDoc*) wins->At(i);
			if (awin && awin->Id() == DSeqPrintDoc::kSeqPrintDoc) {
				awin->fView->Initialize();
				awin->fView->GetReadyToShow();
				awin->fView->Invalidate();
				break; // do only 1st we find ...
				}
			}
		}
#endif		
#endif

}



Boolean DSeqPrintPrefs::IsMyAction(DTaskMaster* action) 
{	
	DView* aview= (DView*) action;
	
	switch (action->Id()) {

		case cIndexLeft : gIndexLeft= aview->GetStatus(); break;
		case cIndexRight: gIndexRight= aview->GetStatus(); break;
		case cIndexTop  : gIndexTop= aview->GetStatus(); break;
		case cNameLeft	: gNameLeft= aview->GetStatus(); break;
		case cNameRight	: gNameRight= aview->GetStatus(); break;
		case cColored		: gColored= aview->GetStatus(); break;

		case cShowComplement: gShowComplement= aview->GetStatus(); break;
		case cThreeLetAA	: gThreeLetAA= aview->GetStatus(); break;
		case cOnlyORF	: gOnlyORF= aview->GetStatus(); break;
		case cShowAA1	: gShowAA1= aview->GetStatus(); break;
		case cShowAA2	: gShowAA2= aview->GetStatus(); break;
		case cShowAA3	: gShowAA3= aview->GetStatus(); break;
		case cShowCompAA1	: gShowCompAA1= aview->GetStatus(); break;
		case cShowCompAA2	: gShowCompAA2= aview->GetStatus(); break;
		case cShowCompAA3	: gShowCompAA3= aview->GetStatus(); break;
		case cShowCutpoints	: gShowCutpoints= aview->GetStatus(); break;
		case cShowAllZymes	: gShowAllZymes= aview->GetStatus(); break;
		case cShowNoncutters	: gShowNoncutters= aview->GetStatus(); break;
		case cShowExcludedCutters	: gShowExcludedCutters= aview->GetStatus(); break;

		default : return DWindow::IsMyAction(action);	
		}
		
	fNeedSave= true;
	return true;
}


void DSeqPrintPrefs::Open()
{
	DWindow::Open();
}

void DSeqPrintPrefs::Close()
{
	if (fNeedSave) {
		DSeqPrintPrefs::SaveGlobals();
		fNeedSave= false;
		}
	DWindow::Close();
}



	// global calling function
void SeqPrintPrefs(short id)
{
	switch (id) {
	
		case kSeqPrintPrefInit: 
			DSeqPrintPrefs::InitGlobals(); 
			break;
	
		case kSeqPrintPrefDialog:
			if (!gSeqPrintPrefs) {
				gSeqPrintPrefs = new DSeqPrintPrefs();
				gSeqPrintPrefs->Initialize();
				}
			if (gSeqPrintPrefs && gSeqPrintPrefs->PoseModally()) ;
			break;
			
		case kAlnPrintPrefDialog:
		case kREMapPrefDialog:
			break;
	}
	
}


