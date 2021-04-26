// DREPrint.cpp
// d.g.gilbert, 1991-95


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



// add this to ?? dgg.c
inline void RectRgn(Nlm_RegioN rgn, Nlm_RectPtr r)
{ 	Nlm_LoadRectRgn( rgn, r->left, r->top, r->right, r->bottom); }



// class DDrawZymeTable

// class DDrawAllZymeTable

// class DDrawNocutZymeTable



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








// class DDrawZymeCutTable


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



// class DDrawZymeRow


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




// class DDrawORFRow

 

DDrawORFRow::DDrawORFRow(Nlm_FonT itsFont, DSeqPrintView* itsView, DSequence* itsSeq,
					 			DList* itsStyles, char* name, ulong* colors, short frame) : 
	DDrawSeqRow( itsFont, itsView, itsSeq, itsStyles, name, colors),
	fAAseq(NULL), fFrame( frame) 
{
	Boolean iscomp = frame > 2;
	short   offset = frame % 3;
	sprintf( fNameStore, "orf%d", frame);
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
	fAAseq= itsSeq->OnlyORF(' ');
	if (iscomp) { delete itsSeq; itsSeq= NULL; }
	if (fAAseq) {
		fBases= fAAseq->Bases();
		fLength= fAAseq->LengthF();
		if (iscomp) { 
			fAAseq->SetSelection( -1,-1);
			DSequence* unrevAA= fAAseq->Reverse(); 
			delete fAAseq; 
			fAAseq= unrevAA; 
			fBases= fAAseq->Bases();
			fLength= fAAseq->LengthF();
			}
			
		// ?? convert orf bases to some symbol? e.g.,  ...[#####]...
		}
}

DDrawORFRow::~DDrawORFRow() 
{ 
	if (fAAseq) delete fAAseq; 
}

void DDrawORFRow::Draw( Nlm_RecT& r, short row, short col, long startitem, long stopitem)
{
	DDrawSeqRow::Draw(r, row, col, startitem, stopitem );
}

const char* DDrawORFRow::Write( short row, short col, long startitem, long stopitem)
{ 
/*==== 
		gctcggctgctgctcggctg
				[---------] [--]
			   	[-----------]
====*/

	return DDrawSeqRow::Write( row, col, startitem, stopitem );
}
	







// class DREMapPrintView


DREMapPrintView::DREMapPrintView( long id, DView* itsSuper, DSeqPrintDoc* itsDocument,
								 DSeqList* itsSeqList, long firstbase, long nbases, long pixwidth, long pixheight):
	DSeqPrintView( id, itsSuper, itsDocument, itsSeqList, firstbase, 
							nbases, pixwidth, pixheight)
{ 
	Initialize();
}

void DREMapPrintView::IndexFromRow( long row, long& itemrow, long& seqline, long& startitem, long& stopitem)
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
	
	FreeLists();
	//fDrawRowList= FreeListIfObject(fDrawRowList);
	fDrawRowList= new DList(); // NULL, DList::kDeleteObjects << bad
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

	//fStyles= FreeListIfObject(fStyles);
	fStyles= new DList(); // NULL,DList::kDeleteObjects << bad
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








// class DAsmPrintView


DAsmPrintView::DAsmPrintView( long id, DView* itsSuper, DSeqPrintDoc* itsDocument,
								 DSeqList* itsSeqList, long firstbase, long nbases, long pixwidth, long pixheight):
	DSeqPrintView( id, itsSuper, itsDocument, itsSeqList, firstbase, 
							nbases, pixwidth, pixheight)
{ 
	Initialize();
}

void DAsmPrintView::IndexFromRow( long row, long& itemrow, long& seqline, long& startitem, long& stopitem)
{
		// !! modify itemrow to return drawer item row for tables after all of sequence as been drawn
  if (row > fMaxseqrow) {
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
		startitem = seqline;
		stopitem  = startitem+1; //??
  	}
  else {
		itemrow	= (row % fLinesPerParag);  
		seqline = (row / fLinesPerParag) * fSeqsperparag;   
		if (fOneseq) seqline += Max( 0, itemrow - fTopPerparag);
		startitem = fFirstBase + seqline * fBasesPerLine;
		stopitem  = Min(startitem + fBasesPerLine, fFirstBase+fNbases);
		}
}

void DAsmPrintView::Initialize()
{
	DSeqPrintView::Initialize();

	fSeqLinesPerParag= 1;
	fOneseq= false;
	fDoMidIndex= fDoTopIndex;
	fDoTopIndex= false;
	fDoSeqLine= true;
	fDoCoseqLine= DSeqPrintPrefs::gShowComplement;
	fDoZymeLine= true;
  
	fDoAAline[0]= false;
	fDoAAline[1]= false;
	fDoAAline[2]= false;
	fDoAAline[3]= false;
	fDoAAline[4]= false;
	fDoAAline[5]= false;	
	
}

void DAsmPrintView::SetScrollPage()
{
	fItemHeight= Nlm_stdLineHeight;
	DTableView::SetScrollPage();
}

void DAsmPrintView::MakeDrawList()
{
	short i, n;
	ulong * colors = NULL;
	DDrawZymeRow * zymerow = NULL;
	DSequence* coseq = NULL;
	Boolean anyamino;
	
	FreeLists();
	fDrawRowList= new DList(); // NULL, DList::kDeleteObjects << bad
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

	fStyles= new DList(); // NULL,DList::kDeleteObjects << bad
	n= DStyleTable::fStyles->GetSize();
	for (i=0; i<n; i++) fStyles->InsertLast( DStyleTable::fStyles->At(i));
	
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

	fDrawRowList->InsertLast(new DDrawSpacer()); fLinesPerParag++;
	
	fMaxseqrow= 1 + (fMaxbases / fBasesPerLine);
	fMaxseqrow *= fLinesPerParag;

	 
}



// class DAsmPrintDoc 

DAsmPrintDoc::DAsmPrintDoc( long id, DSeqDoc* itsDoc, DSeqList* itsSeqList, long firstbase, long nbases) :
	DSeqPrintDoc( id, itsDoc, itsSeqList, firstbase, nbases)
{
	short width= 450, height= 350;
	fView= new DAsmPrintView(0,this,this,fSeqList,fFirstBase,fNbases,width,height);

	char title[256];
	DSequence * aSeq= (DSequence*) fSeqList->At(0);
	if (aSeq)  
		StrCpy(title, aSeq->Name());
	else
		itsDoc->GetTitle( title, sizeof(title)); 
	StrCat( title, " Assembly");
	SetTitle( title);
}



