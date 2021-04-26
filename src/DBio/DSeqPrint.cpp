// DSeqPrint.cpp
// d.g.gilbert, 1991-95

#define MASKS 1

#include "DSequence.h"
#include "DSeqList.h"
#include "DSeqDoc.h"
#include "DSeqPrint.h"
#include "DSeqPrintSubs.h"
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



#undef TESTZOOM



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
#ifdef WIN_MAC
			DSeqPrintPrefs::gOutFormat = cOutPICT,
#else
			DSeqPrintPrefs::gOutFormat = cOutText,
#endif			
			DSeqPrintPrefs::gREMinCuts = 1, 
			DSeqPrintPrefs::gREMaxCuts = 999;

 
char DDrawMapRow::fLinebuf[DDrawMapRow::kMaxLinebuf+1];



// add this to ?? dgg.c
inline void RectRgn(Nlm_RegioN rgn, Nlm_RectPtr r)
{
	Nlm_LoadRectRgn( rgn, r->left, r->top, r->right, r->bottom);
}


static short gBaseCharWidth = 12;
static short gLineHeight = 12;


//inline 
static short BaseCharWidth()
{
#ifdef TESTZOOM
#if 1
	 gBaseCharWidth= 1;
#else
		// TESTING...
	if (gKeys->shift()) gBaseCharWidth= 1;	else gBaseCharWidth= 12; 
	if (gBaseCharWidth>1) gBaseCharWidth= Nlm_CharWidth('G');
#endif
#else
	gBaseCharWidth= Nlm_CharWidth('G');
#endif
	return gBaseCharWidth;
}

//inline 
static short BaseLineHeight()
{
	gLineHeight = Nlm_LineHeight();
	return gLineHeight;
}

//inline 
static void BaseDraw( char b)
{
#ifdef TESTZOOM
	if (gBaseCharWidth < 5) {
		Nlm_PoinT pt;
		Nlm_GetPen (&pt);
    Nlm_MoveTo (pt.x, pt.y);
    pt.y -= gLineHeight; //Nlm_LineHeight();
    Nlm_LineTo (pt.x, pt.y);
		}
	else 
#endif
		Nlm_PaintChar(b);
}





// class DDrawSpacer

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




// class DDrawIndexRow 

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
	
	

// class DDrawSeqRow

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
	if (name) StrNCpy( fNameStore, name, sizeof(fNameStore)); 
	else *fNameStore= 0;
	fName= fNameStore; 
	if (fFont) Nlm_SelectFont(fFont);
	fHeight= BaseLineHeight();
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
					
					BaseDraw(ch);
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
				BaseDraw(ch);
				laststyle= NULL;
				}

#else
 			if (fColors) {
	 			// if (swapBackcolor) Nlm_SetBackColor( fColors[ch-' ']); else 
				if (ch!=lastch) Nlm_SetColor( fColors[ch-' ']);
				}
			BaseDraw(ch);
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
	
	

// class DDrawManySeqRow



DDrawManySeqRow::DDrawManySeqRow(Nlm_FonT itsFont, DSeqPrintView* itsView, 
											DSeqList* seqlist, long seqIndex, DList* itsStyles, ulong* colors,
											char* commonbases, char* firstcommon) : 
	  DDrawSeqRow( itsFont, itsView, NULL, itsStyles, NULL, colors),
		fSeqList(seqlist)
{
	fItemrow= seqIndex;
	fSeq= seqlist->SeqAt( fItemrow);
	if (fSeq) {
		StrNCpy( fNameStore, fSeq->Name(), sizeof(fNameStore));
		fName= fNameStore;
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



// class DDrawAminoRow


DDrawAminoRow::DDrawAminoRow(Nlm_FonT itsFont, DSequence* itsSeq, short frame) : 
		fAAseq(NULL), fFrame( frame), fLength(0), 
		fOnlyORF(DSeqPrintPrefs::gOnlyORF),
		fThreeLet(DSeqPrintPrefs::gThreeLetAA) 
{
	Boolean iscomp = frame > 2;
	short   offset = frame % 3;
	fFont= itsFont;
	if (fFont) Nlm_SelectFont(fFont);
	fHeight= BaseLineHeight();
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
				BaseDraw(*b);
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
		// testing reduced view
	if (gKeys->shift()) gBaseCharWidth= 1;	else gBaseCharWidth= 12; 

	this->SetSlateBorder( false);
	this->SetResize( DView::relsuper, DView::relsuper);
	this->SetTableFont(gTextFont);
	fBaseWidth= BaseCharWidth();
	fOneseq= fSeqList->GetSize()<2; 
	
  fSeq= (DSequence*) fSeqList->At(0);

	Initialize();  
}


void DSeqPrintView::FreeLists()
{
	short i, n;
	if (fDrawRowList) {
		n= fDrawRowList->GetSize();
		for (i=0; i<n; i++) {
			DDrawMapRow	*	arow= (DDrawMapRow*) fDrawRowList->At(i);
			delete arow;
			}
		delete fDrawRowList;
		}
	if (fStyles) {
		n= DStyleTable::fStyles->GetSize();
		for (i=0; i<n; i++) {
			DSeqStyle	*	astyle= (DSeqStyle*) DStyleTable::fStyles->At(i);
			delete astyle;
			}
		delete fStyles;
		}
}

DSeqPrintView::~DSeqPrintView()
{	
	//fDrawRowList= FreeListIfObject(fDrawRowList);
	//fStyles= FreeListIfObject(fStyles);
	FreeLists();
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

	FreeLists();
	//fDrawRowList= FreeListIfObject(fDrawRowList);
	fDrawRowList= new DList(); // NULL, DList::kDeleteObjects << bad, doesn't call destruct
	fDrawRowList->InsertLast( new DDrawSpacer());
	if (fDoTopIndex) 
		fDrawRowList->InsertLast( new DDrawIndexRow(fNumFont,fBaseWidth));

	ulong * colors;
	//if (!fDoc->fUseColor) colors= NULL;
	if (!DSeqPrintPrefs::gColored) colors= NULL;
	else if (fSeq->Kind() == DSequence::kAmino) colors= DBaseColors::gAAcolors;
	else colors= DBaseColors::gNAcolors;

	//fStyles= FreeListIfObject(fStyles);
	fStyles= new DList(); // NULL,DList::kDeleteObjects << bad
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


void DSeqPrintView::IndexFromRow( long row, long& itemrow, long& seqline, long& startitem, long& stopitem)
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

void DSeqPrintView::DrawRow(Nlm_RecT r, long row)
{
	long 			itemrow, seqline, startitem, stopitem;
	long 		col, ncols;
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

void DSeqPrintView::WriteRow(long row)
{
	long 			itemrow, seqline, startitem, stopitem;
	long 		col, ncols;
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

		// testing reduced view
	if (gKeys->shift()) gBaseCharWidth= 1;	else gBaseCharWidth= 12; 

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
		
	short outform= DSeqPrintPrefs::gOutFormat;
	if (gKeys->shift()) {
		if (outform == DSeqPrintPrefs::cOutText) outform= DSeqPrintPrefs::cOutPICT;
		else outform= DSeqPrintPrefs::cOutText; 
		}
	switch (outform) {
		case DSeqPrintPrefs::cOutPICT:
			fView->WriteToPICT( aFile);
			break;
		case DSeqPrintPrefs::cOutText:
		default:
			fView->WriteTo( aFile); 
			break;
		}
}
		


void DSeqPrintDoc::Print()  
{
	fView->Print();
}












// class DAlnPrintView


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


class DOutFormatPopup : public DPopupList
{
public:
	DOutFormatPopup(long id, DView* superior, short defaultFormat);
	virtual short GetValue() {
	 	return  DSeqPrintPrefs::cOutText + DPopupList::GetValue() - 1;
		}
};

DOutFormatPopup::DOutFormatPopup(long id, DView* superior, short defaultFormat) :
			DPopupList(id,superior,true)
{
	this->AddItem( "Text");
	this->AddItem( "PICT");
	this->SetValue(defaultFormat - DSeqPrintPrefs::cOutText + 1);
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
	short outform= gApplication->GetPrefVal( "gOutFormat", "seqprint","0");
  if (outform) gOutFormat= outform;
  
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
	gApplication->SetPref( gOutFormat, "gOutFormat", "seqprint");
	
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
	DPopupMenu* popm;

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

	clu= new DCluster( 0, super, 0, 0, true);
	super= clu;
	pr= new DPrompt(0,super,"File format:",0, 0, Nlm_programFont);
	super->NextSubviewToRight();
	DOutFormatPopup* apop = new DOutFormatPopup( cOutText, super, gOutFormat);
	super = this;
	super->NextSubviewBelowLeft();

	//super->NextSubviewToRight();

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
 			// ^^ same as aStyleMenu->GetValue(); //??
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
		case cOutText: gOutFormat= aview->GetValue(); break;
  	
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


