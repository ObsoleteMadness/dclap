// DRichView.cpp
// d.g.gilbert


#include "Dvibrant.h"
#include "DWindow.h"
#include "DPanel.h"
#include "DControl.h"
#include "DFile.h"
#include "DFindDlog.h"
#include "DList.h"
#include "DUtil.h"
#include "DDialogText.h"
#include "DViewCentral.h"

#include "DDrawPICT.h"

#include "DRichStyle.h"
#include "DRichMoreStyle.h"
#include "DRichViewNu.h"
#include "DRTFHandler.h"
#include "DPICTHandler.h"
#include "DHTMLHandler.h"
#include "DGIFHandler.h"

#include "DIconLib.h"
#include "DIcons.h"


extern DDrawPict* gDrawPict; 

static Boolean gNotInitted = true;

#ifdef DBUG
#define Debug(x)  fprintf(stderr,x)
#define Debug1(x,y)  fprintf(stderr,x,y);
#else
#define Debug(x) 
#define Debug1(x,y)
#endif

enum {
	chPictTag = 26,
	MAXFONTS  = 32
	};

static struct fontheights {
  Nlm_FonT  font;
  short	  	height;
	} gFontHeights[MAXFONTS];

extern "C" Nlm_Boolean Nlm_DoClick (Nlm_GraphiC a, Nlm_PoinT pt); // vibutils.c

//short DRichView::GetFontHeight(Nlm_FonT font)
static short GetFontHeight(Nlm_FonT font)
{
			// Returns the pixel height of a font, caching for quicker access
  short  height, i;
  height = 0;
  i = 0;
  while (i < MAXFONTS && gFontHeights[i].font != font
		 && gFontHeights[i].font != NULL)
			i++;
  if (gFontHeights[i].font == font) 
    return gFontHeights[i].height;
	else {
    Nlm_SelectFont(font);
    height = Nlm_LineHeight();
    if (i < MAXFONTS) {
      gFontHeights[i].font = font;
      gFontHeights[i].height = height;
    	}
   	return height;
		}
}






//class DRichView : public DPanel


extern "C" void richPanelClickProc(Nlm_PaneL item, Nlm_PoinT mouse)
{
	DRichView *obj= (DRichView*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Click(mouse);
}

extern "C" void richPanelDragProc(Nlm_PaneL item, Nlm_PoinT mouse)
{
	DRichView *obj= (DRichView*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Drag(mouse);
}

extern "C" void richPanelHoldProc(Nlm_PaneL item, Nlm_PoinT mouse)
{
	DRichView *obj= (DRichView*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Hold(mouse);
}

extern "C" void richPanelReleaseProc(Nlm_PaneL item, Nlm_PoinT mouse)
{
	DRichView *obj= (DRichView*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Release(mouse);
}

extern "C" void richPanelDrawProc(Nlm_PaneL item)
{
	DRichView *obj= (DRichView*) Nlm_GetObject( (Nlm_GraphiC)item);
	if (obj) obj->Draw();
}

extern "C" void richPanelScrollProc(Nlm_BaR sb, Nlm_SlatE slate, short newval, short oldval)
{
	DRichView *obj= (DRichView*) Nlm_GetObject( (Nlm_GraphiC)slate);
	if (obj) obj->ScrollDoc(sb, newval, oldval);
	//if (obj) obj->Scroll(true, obj, newval, oldval);
}

extern "C" void richPanelResetProc(Nlm_PaneL item)
{
	DRichView *obj= (DRichView*) Nlm_GetObject( (Nlm_GraphiC)item);
		// damn, this is called by Vibrant cleaner-upper AFTER DRichView::~DRichView() !!
	if (obj) obj->ResetDoc();
}


//typedef	short (*CompareFunc)( void*, void*);	 // compare two list/array Items 

short richPanelCompareStyles( void* item1, void* item2)
{
#if 0
	if ( ((DRichStyle*) item1)->IsNotEqual( (DRichStyle*) item2) ) { 
		if (item1 > item2) return 1;
		else return -1;
		}
	else 
		return 0;
#else
	return MemCmp( item1, item2, sizeof(DRichStyle));
#endif
} 


 
DRichView::DRichView(long id, DView* itsSuperior, short pixwidth, short pixheight,
			Boolean hasVscroll):
	DPanel(id, itsSuperior, pixwidth, pixheight, DPanel::subclasscreates)
{
 	Nlm_SltScrlProc scrollproc;
 	fNumItems= 0;
	fNumLines= 0;
	fBarmax= 0;
	fDocFormat= 0;
	fTabCount= 1;
  fAutoAdjust= true; 
  fIsVirtual= false; 
  fDodraw= true;
	fStyles= NULL; 
	fParags= NULL; 
	fCurrentStyle= NULL;
	fSelParag= fSelChar= fSelEndParag= fSelEndChar= 0;
	fSelected= fSelInPict= false;
	ZeroCharWidths();

#if 0	
	if (!Nlm_EmptyRect( &DRichTextDoc::gRichDocRect)) {
		pixwidth= DRichTextDoc::gRichDocRect.right - DRichTextDoc::gRichDocRect.left;
		pixheight= DRichTextDoc::gRichDocRect.bottom - DRichTextDoc::gRichDocRect.top;
		}
#endif		
	if (pixwidth ==0) pixwidth = Nlm_screenRect.right - Nlm_screenRect.left - 60;
	if (pixheight==0) pixheight= Nlm_screenRect.bottom - Nlm_screenRect.top - 120;
			// these -## adjustments are not very good -- need to know what other
			// views are in the given window and adjust for their size !?

	if (!itsSuperior) return; 
	Nlm_GrouP prnt= (Nlm_GrouP)itsSuperior->GetNlmObject();
	if (!prnt) return; 
  Nlm_WindoW tempPort = Nlm_SavePort(prnt);
  if (hasVscroll) scrollproc= richPanelScrollProc;
  else scrollproc= NULL;
  fPanel= Nlm_AutonomousPanel(prnt, pixwidth, pixheight, richPanelDrawProc,
                               scrollproc, NULL, 0,
                               NULL/*richPanelResetProc*/, NULL);
  if (!fPanel) return; 
  if (tempPort) Nlm_RestorePort(tempPort);
  
	Nlm_SetPanelClick(fPanel, richPanelClickProc, richPanelDragProc, richPanelHoldProc, 
										richPanelReleaseProc);
	this->SetNlmObject(fPanel);
	this->SetAutoAdjust(true); // used after append/insert/replace/delete
	this->SetSlateBorder(true);
		
	fStyles= new DList( ::richPanelCompareStyles ); //, DList::kDeleteObjects; // + DList::kOwnObjects 
	fParags= new DList( ); // NULL, DList::kDeleteObjects;
	//fStyles->fArrayIncrement= 30;
	//fParags->fArrayIncrement= 30;
	
	if (gNotInitted) InitRichView();
	fCurrentStyle= new DRichStyle();
	ClearCurrentStyle();

	Nlm_LoadRect( &fDrawr, 0,0,0,0);
	fAtpt.x= fAtpt.y= 0;
			
	DWindow* win= this->GetWindow();
	if (win) {
		win->fPrintHandler= this;
		win->fSaveHandler= this;
		}
}
	



DRichView::~DRichView()
{
	//?? need for SetDocData's DocFreeProc call...
	//if (fStyles) delete fStyles; fStyles= NULL;
	//if (fParags) delete fParags; fParags= NULL;
		// damn, delete fList isn't calling object destructors !!!!
	long i, n;
	if (fStyles) {
		n= fStyles->GetSize();
		for (i=0; i<n; i++) {
			DRichStyle* st= (DRichStyle*) fStyles->At(i);
			if (st) delete st;
			}
		delete fStyles;
		}
	if (fParags) {
		n= fParags->GetSize();
		for (i=0; i<n; i++) {
			DParagraph* pa= (DParagraph*) fParags->At(i);
			if (pa) delete pa;
			}
		delete fParags;
		}
	if (fCurrentStyle) {
		fCurrentStyle->ownpict= false;
		delete fCurrentStyle;
		}
}

void  DRichView::DeleteAll() 
{
	short i, n;
	if (fStyles) {
		n= fStyles->GetSize();
		for (i=0; i<n; i++) {
			DRichStyle* st= (DRichStyle*) fStyles->At(i);
			delete st;
			}
		fStyles->DeleteAll();
		//delete fStyles;
		}
	if (fParags) {
		n= fParags->GetSize();
		for (i=0; i<n; i++) {
			DParagraph* pa= (DParagraph*) fParags->At(i);
			delete pa;
			}
		fParags->DeleteAll();
		//delete fParags;
		}
	fNumItems= fNumLines= 0;
}



void DRichView::InitRichView()
{
	gNotInitted= true;
  for (short i = 0; i < MAXFONTS; i++) {
    gFontHeights[i].font = NULL;
    gFontHeights[i].height = 0;
  	}
	gDrawPict = new DDrawPict();
}



void DRichView::ViewRect( Nlm_RecT& r)  
{ 
	DPanel::ViewRect( r);
	Nlm_InsetRect(&r, 10, 4); 
}



	
void DRichView::SetSlateBorder(Boolean turnon) 
{
 	Nlm_SetSlateBorder((Nlm_SlatE)fPanel, turnon);  
}

void DRichView::SizeToSuperview( DView* super, Boolean horiz, Boolean vert)
{
#if 0
#ifdef WIN_MOTIF
	DPanel::SizeToSuperview( super, horiz, vert);
#else
	// size only lower and right sides == leave top area...
	Nlm_RecT r, myr;
	super->ViewRect(r);
	GetPosition( myr); // ViewRect(myr); <<!!NO, diff from Get/SetPos
	if (horiz) 	myr.right= (r.right - r.left) +1;
	// myr.right -= (Nlm_vScrollBarWidth - 8); // mswin patch !?
	if (vert) myr.bottom =  (r.bottom - r.top)  +1; 
	SetPosition( myr);
#endif
#endif
}
  

void DRichView::Resize(DView* superview, Nlm_PoinT sizechange)
{
	DPanel::Resize( superview, sizechange);
}





DRichStyle* DRichView::GetStyle( short styleid)
{
	return (DRichStyle*) fStyles->At(styleid);
}

DRichStyle* DRichView::GetStyleAtIndex(DParagraph* aParag, short styleindex)
{	
	short id= -1;
	if (aParag) {
		id= aParag->fStyleIndex[styleindex].styleid;
		return (DRichStyle*) fStyles->At(id);  // id-1?? NO
		}
  else return NULL;
}

DRichStyle* DRichView::GetStyleAtChar( DParagraph* aParag, short atchar)
{
	short id= -1;
	if (aParag) {
		short i;
		for (i= aParag->fNumStyles-1; i>0; i--) { 
			if (atchar >= aParag->fStyleIndex[i].nextofs) 
				break;
			}
		id= aParag->fStyleIndex[i].styleid; 
		}
	return (DRichStyle*) fStyles->At(id);  // id-1?? NO
}

DRichStyle* DRichView::GetStyleAtChar( short atparag, short atchar)
{
	return GetStyleAtChar( (DParagraph*)fParags->At(atparag), atchar);
}

short DRichView::FindOrAddStyle( DRichStyle* theStyle)
{ 
	short styleid= fStyles->GetEqualItemNo( theStyle);
	if (styleid == kEmptyIndex) {
		DRichStyle* bst= new DRichStyle();
		*bst= *theStyle;   // need a smart assign/copy method !! ?? a FIXED clone() ??
		bst->ownpict= true;
		theStyle->fObject= NULL; //theStyle->picdata= NULL;
		fStyles->InsertLast(bst);
		styleid= fStyles->GetSize() - 1;
		}
	return styleid;
}


DParagraph* DRichView::GetParaPtr( short item)
{ 
	return (DParagraph*) fParags->At(item); 
}



DParagraph* DRichView::MakeNewParag( void* data, short lines, 
						DParagraph* parFormat, DRichStyle* styles, short nstyles, Boolean docOwnsData)
{
	short ist, nst=0;
	//if (styles) do nst++; while (!styles[nst-1].last);
	StyleIndex* styleindex= (StyleIndex*) MemNew( nstyles*sizeof(StyleIndex));
	for (ist=0; ist<nstyles; ist++) {
		short id= FindOrAddStyle( &styles[ist]);
		styleindex[ist].styleid= id;
		styleindex[ist].nextofs= styles[ist].nextofs;
		}
	
	return  new DParagraph( (char*)data, docOwnsData, lines, parFormat, styleindex, nstyles);
}


void 	DRichView::AppendParag( DParagraph* aParag)
{	
  if (aParag) {
		aParag->startsAt= fNumLines;
    fNumLines += aParag->numRows;
		fParags->InsertLast(aParag);
    fNumItems++; // == fParags->GetSize();
  	}
}

void 	DRichView::Append( void* data, short lines, DParagraph* parFormat,  
						DRichStyle* styles, short nstyles, Boolean docOwnsData)
{	
	DParagraph* aParag= MakeNewParag( data, lines, parFormat, styles, nstyles, docOwnsData);
  if (aParag) {
    UpdateItemHeights(aParag);
    AppendParag( aParag);
    if (fAutoAdjust) AdjustScroll();
  	}
}

void 	DRichView::Insert( short item, void* data, short lines, DParagraph* parFormat, 
						DRichStyle* styles, short nstyles, Boolean docOwnsData)
{
	DParagraph* aParag= MakeNewParag( data, lines, parFormat, styles, nstyles, docOwnsData);
  if (aParag) {
    UpdateItemHeights(aParag);
		DParagraph* pat= (DParagraph*)fParags->At(item);
		if (pat) {
			aParag->startsAt= pat->startsAt;
			fParags->InsertBefore(item, aParag);
	    fNumLines = UpdateLineStarts(item);
			fNumItems++; // == fParags->GetSize();
	    }
	  else {
			AppendParag( aParag);	
	  	}
    if (fAutoAdjust) AdjustScroll();
  	}
}

void 	DRichView::Replace( short item, void* data, short lines, DParagraph* parFormat,  
				DRichStyle* styles, short nstyles, Boolean docOwnsData)
{
	DParagraph* aParag= MakeNewParag( data, lines, parFormat, styles, nstyles, docOwnsData);
  if (aParag) {
    UpdateItemHeights(aParag);
		DParagraph* pat= (DParagraph*) fParags->At(item);
		if (pat) {
			aParag->startsAt= pat->startsAt;
			fParags->AtPut(item, aParag);
	    fNumLines = UpdateLineStarts(item);
			delete pat; // !? should DList do this on AtPut ?? it doesn't
	    }
	  else {
			AppendParag( aParag);
	  	}
    if (fAutoAdjust) AdjustScroll();
  	}
}


void 	DRichView::UpdateStyleHeight( DRichStyle* theStyle)
{
  if (theStyle) {
  	short theid= fStyles->GetIdentityItemNo( theStyle);
		if (theid == kEmptyIndex) return;
		
  	short j, item, nitem= fParags->GetSize();
  	for (item= 0; item<nitem; item++) {
			DParagraph* pat= (DParagraph*) fParags->At(item);
			for (j= 0; j< pat->fNumStyles; j++) {
				if (pat->fStyleIndex[j].styleid == theid) {
			    short rtop, rbottom, firstline;
			    UpdateItemHeights(pat);
			    if (IsVisible( item, rtop, rbottom, firstline )) {
				    Nlm_RecT	rec;
			    	ViewRect( rec);
						rec.top= rtop;
						InvalRect( rec);
			    	}
			    break;
			    }
		    }
	    }
    if (fAutoAdjust) AdjustScroll();
  	}
}





short DRichView::GetItemNum(short desiredLine)
{
	// Returns the number of the paragraph containing the desired line

  short mid = 1;
  short left = 1;
  short right = fNumItems;
  Boolean goOn = TRUE;
  while (left <= right && goOn) {
    mid = (left + right) / 2;
    DParagraph* aParag = GetParaPtr(mid - 1);
    if (aParag) {
      if (desiredLine < aParag->startsAt + aParag->numRows)  
        right = mid - 1;
      if (desiredLine >= aParag->startsAt)  
        left = mid + 1;
    	} 
    else {
      goOn = FALSE;
      mid = 1;
    	}
  }
  return mid - 1;
}



void DRichView::SetStyle(DRichStyle* theStyle)
{
	if (!theStyle || theStyle->linkid || theStyle->ispict) 
		return; /* do these styles separately */
	if (theStyle->font != fCurrentStyle->font) 
		Nlm_SelectFont(theStyle->font);
	if (theStyle->color != fCurrentStyle->color)
		Nlm_SetColor(theStyle->color);

	ZeroCharWidths();
	*fCurrentStyle = *theStyle;
}


void DRichView::ClearCurrentStyle()
{
	fCurrentStyle->ispict= false;
	fCurrentStyle->ismap= false;
	fCurrentStyle->ownpict= false;
	fCurrentStyle->fObject= NULL; //fCurrentStyle->picdata= NULL;
	fCurrentStyle->font= NULL;
	fCurrentStyle->superSub= 0;
	fCurrentStyle->color= 0;
}



short DRichView::CharWidth(char ch)
{
	unsigned char cw= fCharWidths[ch];
	if (cw == kNoCharWidth) {
		cw= Nlm_CharWidth(ch);
	 	fCharWidths[ch]= cw;
	 	}
	return cw;
}

void DRichView::ZeroCharWidths()
{
	Nlm_MemFill( fCharWidths, kNoCharWidth, 256); // sizeof(fCharWidths)
}

short DRichView::StyleLineWidth(char* texts, short maxwid, DParagraph* itemPtr, short& istyle)
{
  Char 			  ch;
  short  			i,swid,wid,nextofs,numStyles;
	Boolean 		styleset;
	DRichStyle * atstyle, savestyle;
	
  styleset= FALSE;
  i = wid = swid = 0;
	nextofs= itemPtr->fStyleIndex[istyle].nextofs;
	numStyles= itemPtr->fNumStyles;
	atstyle= GetStyleAtIndex(itemPtr, istyle);
	savestyle= *atstyle;
	
 	if (atstyle->ispict) wid += atstyle->pixwidth; 
	if (texts && maxwid > 0) {
    ch = texts[i];
    while (ch != '\0' && ch != '\n' && ch != '\r' && wid <= maxwid) {

      if (i>=nextofs && istyle<numStyles-1) {  // && !atstyle->last    
				++istyle;
				nextofs= itemPtr->fStyleIndex[istyle].nextofs;
				atstyle= GetStyleAtIndex(itemPtr, istyle);
     		SetStyle(atstyle);
     		styleset= TRUE;
  			if (atstyle->ispict) wid += atstyle->pixwidth; 
        }
        
      else if (!atstyle->hidden) {
		    if (ch == '\t') {
						// fix this tab width !!!
		  		}
		    else wid += CharWidth(ch);
      	}
      
      i++;
      ch = texts [i];
    	}
  	}
	if (styleset) SetStyle( &savestyle);
	savestyle.ownpict= false;
  return wid;
}



short DRichView::FitTextToWidth(char* texts, short maxwid, Boolean byPixels,
                          DParagraph* itemPtr, short& istyle)
{
			// Returns the number of characters in the next block, including 
			// trailing spaces (which are trimmed back later)
  Char  ch;
  short  i,j,wid, swid, nextofs, numStyles;
	Boolean wordWrap, styleset;
	DRichStyle *atstyle, savestyle;
	
	nextofs= itemPtr->fStyleIndex[istyle].nextofs;
	numStyles= itemPtr->fNumStyles;
	atstyle= GetStyleAtIndex(itemPtr, istyle);
	SetStyle(atstyle); // be sure ??
	savestyle= *atstyle;
	wordWrap= itemPtr->wrap;
  i = j = 0;
  wid = swid = 0;
  if (texts && maxwid > 0) {
    ch = texts [i];
    while (ch != '\0' && ch != '\n' && ch != '\r' && wid <= maxwid) {
      if (wordWrap) {
      	if (i>=nextofs && istyle<numStyles-1) {   //if (ch == chStyleTag) 
					++istyle;
					nextofs= itemPtr->fStyleIndex[istyle].nextofs;
					atstyle= GetStyleAtIndex(itemPtr, istyle);
       		SetStyle(atstyle);
       		styleset= TRUE;
    			if (atstyle->ispict && byPixels) wid += atstyle->pixwidth; 
        	}
        	
        else if (!atstyle->hidden) {
	      	if (ch == '\t') {
						/* !!! FIX THIS !?!?! */
	        	}
          else if (byPixels) wid += CharWidth (ch);
          else wid++;
	        }
      	}
      i++;
      ch = texts [i];
    	}
      	
    j = i;
    if (wordWrap && wid > maxwid) {
    	// don't break in a word, and leave trailing spaces at end of line 
      j--;
      if (byPixels)  
        while (Nlm_TextWidth(texts, i) >= maxwid)  i--;
      else 
        while (i >= maxwid) i--;
      while (i > 0 && texts[i - 1] != ' ' && texts[i - 1] != '-')  i--;
      while (texts [i] == ' ')  i++;
    	}
    	
  	}
	if (styleset) SetStyle( &savestyle);
	savestyle.ownpict= false; //!!
  if (i > 0 && i < j) return i;
	else if (j > 0) return j;
	else  return 0;
}






void DRichView::ParseText(DParagraph* itemPtr, char* text, Boolean byPixels, Nlm_RecT r)
{
			//  Parses text that contains \t, \r and \n characters into a list of
			//  cells organized by row and column

  char    	ch, just;
  short   	inset,
            istyle, numStyles, col, blklen, len,width,maxwid,
  					numRows,tabRow,returnRow,row;
  Uint2    	start;
  Boolean  	wrap;
	char 	 	* newtex;
	ulong	 	maxsize, newsize;
	
  if (itemPtr && text && *text != '\0') {
  	numStyles= itemPtr->fNumStyles;
   	istyle= 0;
    if (numStyles) {
    	DRichStyle savestyle;
    	savestyle= *fCurrentStyle;
    	ClearCurrentStyle();
    	SetStyle(GetStyleAtIndex(itemPtr, istyle));
    	
    	if (*text == '\n' && text[1] == '\0') {
    	    // ?? Win32 bug - failing w/ multiple '\n's ??!!
  		  newtex = (char*) MemNew (5 + itemPtr->minLines);
   			newtex[0]= '\n';
  			newtex[1]= '\0';
        itemPtr->numRows = MAX (1, itemPtr->minLines);
  			itemPtr->fText = newtex;
  			SetStyle(&savestyle);
  			savestyle.ownpict= false; // !!
  			return;
  			}
  			
  		maxsize= (ulong) (StrLen(text) + 50 + itemPtr->minLines);
  		newsize= 0;
  		newtex = (char*) MemNew (maxsize);
     	
      width = 0;
      inset = itemPtr->leftInset; /* pixLeft, pixFirst ?? */
      just = itemPtr->just; /*'l';*/
      wrap = itemPtr->wrap; /* TRUE; /* fix this as choice ... */
      start = 0;
      row = 0;
      tabRow = 0;
      returnRow = 1;
      col = 0;
      numRows = 0;
      while (text[start] != '\0') {
        ch = text[start];
        if (ch != '\0' && ch != '\n' && ch != '\r'  ) {
          maxwid =  r.right - r.left - 15; // !!! Need PAGEWIDTH here !!
          short istyle0= istyle;   	         	
          blklen =  FitTextToWidth(text + start, maxwid, byPixels, 
          					itemPtr, istyle);
    			if (istyle!=istyle0) SetStyle(GetStyleAtIndex(itemPtr, istyle));
          len = blklen;
          
          if (len > 0) {
            if (text[start + len] != '\0') { // eat trailing spaces 
              while (len > 0 && text [start + len - 1] == ' ') len--;
              if (len == 0) len = blklen;
            	}
            if (newsize+len > maxsize) {
            	maxsize= newsize+len+100;
            	newtex= (char*) MemMore(newtex, maxsize);  
            	}
            MemCpy( newtex+newsize, text+start, len);
            newsize += len;
            start += blklen;
          	} 
          	
          else { // len==0, shouldn't be true ? 
 
          	}
        	}
        	
        ch = text [start];
        if (ch == '\n') {
     			newtex[newsize++]= '\n';
	      	start++;
          row = returnRow;
          tabRow = row;
          returnRow++;
          col = 0;
        	} 
        else if (ch == '\r') {
     			newtex[newsize++]= '\n';
          start++;
          row++;
          returnRow = MAX (returnRow, row + 1);
	       	} 
#if 0
        else if (ch == '\t') {
          start++;
          row = tabRow;
      		newtex[newsize++]= '\t';
          if (text [start] == '\0') row = returnRow;
        	} 
#endif
        else if (ch != '\0' && wrap) {
      		newtex[newsize++]= '\n';
          if (len == 0) start++;
          row++;
          returnRow = MAX (returnRow, row + 1);
        	} 
        else if (ch != '\0') 
        	start++;
        else if (text [start - 1] != '\n' && text [start - 1] != '\r')  
          row = returnRow;
         
        numRows = MAX (numRows, row);
      	}
      itemPtr->numRows = MAX (numRows, itemPtr->minLines);
			newtex[newsize++]= '\0';
			itemPtr->fText = newtex;
			SetStyle(&savestyle);
			savestyle.ownpict= false; // !!
    	}
  	} 
  else if (itemPtr != NULL)  
    itemPtr->numRows = 0;
}





// temp globals...

static short gMapOfs = 0;
//static DRichStyle* gMapStyle = NULL;

short DRichView::DrawStyledItem(DParagraph* itemPtr, Nlm_RectPtr rpara, short item,
                          			short frst, Boolean forPrinter, Nlm_PoinT* mapPoint)
{
				// Draws a given paragraph(item) on the screen, using parag & char styles 
  char     	ch, just, csave;
  Boolean  	isnewline, bartab, dotab, mapPt, drawSelection, gray, doborder;
  char 		* ptext, * text;
  unsigned long			nextofs;
  short     lineHeight, num, row, rsult, rleft, rright, rbottom, width, maxwid,
					 	istyle,styleid, istyle2, numStyles, superSub, txtwidth, yfont; 
					  
  DRichStyle	* astyle;  
  Nlm_RecT     	rct; // drw
	Nlm_PoinT			pt1, pt2; //, atpt;
	
	mapPt= (mapPoint != NULL);
	if (mapPt) fMapStyle= NULL;
	fDodraw= (mapPoint == NULL);
	drawSelection= (fSelected && fDodraw && !forPrinter 
									&& item >= fSelParag && item <= fSelEndParag);
  rsult = 0;
  if (itemPtr && rpara) {
    text = itemPtr->fText;
    numStyles= itemPtr->fNumStyles;
    
    if (text && *text != '\0') {
    	just= itemPtr->just;
      lineHeight = itemPtr->lineHeight;
      rct.top = rpara->top;
      rct.bottom = rct.top;
			rbottom = rct.top + lineHeight * (itemPtr->numRows - frst);
 			rleft= fAtpt.x = rpara->left + itemPtr->leftInset + itemPtr->firstInset;
      fAtpt.y = rpara->top;
      row = 0;
      txtwidth= 0;
      ptext = text;
      ch = *ptext;
  		gray = FALSE;
			isnewline= TRUE;

//Debug("1");
     	istyle= 0;
     	styleid= itemPtr->fStyleIndex[istyle].styleid;
     	nextofs= itemPtr->fStyleIndex[istyle].nextofs;
 			astyle= GetStyle(styleid);

			Nlm_LoadRect(&fDrawr, rleft, rpara->top, rpara->right, rbottom);
			
      if (itemPtr->newPage) {
      	/* probably on line by self... */
      	/* record footnote text somewhere and dump out here before page break... */
      	if (fDodraw && !forPrinter) {
					Nlm_Dashed(); // Nlm_Dotted(); 
	    		Nlm_FrameRect(&fDrawr);
					Nlm_ResetDrawingTools();
					}
      	}
      	 			
     	ClearCurrentStyle();
     	SetStyle(astyle);
			superSub= astyle->superSub;
			
      while (ch != '\0') {
      
        if (ch == '\n') {
          rct.top = rpara->top + (row - frst) * lineHeight;
          rct.bottom = rct.top + lineHeight;
      	  rleft= fAtpt.x = rpara->left + itemPtr->leftInset;
          row++;
          ptext++; 
          isnewline= TRUE;
//Debug("/");
        	} 
					
        else if (ch == chPictTag) {
          ptext++;
        	} 
       	
        else  {
        
					while ( (ptext - text >= nextofs) && (istyle < numStyles-1)) {
//Debug("s");
			     	istyle++;
			     	styleid= itemPtr->fStyleIndex[istyle].styleid;
			     	nextofs= itemPtr->fStyleIndex[istyle].nextofs;
			     	if (!((ptext - text >= nextofs) && (istyle < numStyles-1))) {
				 			astyle= GetStyle(styleid);
	     				SetStyle(astyle);
							superSub= astyle->superSub;
							}
						}

  				if (isnewline) {
			 			doborder= itemPtr->DoBorder();

	      	  if (just != 'l') {
	      	  	istyle2= istyle;
	          	maxwid=  rpara->right - rpara->left; //!!! Need PAGEWIDTH here !!
	 						width= StyleLineWidth( ptext, maxwid, itemPtr, istyle2);
	 						if (width<maxwid) {
		 						switch (just) {
		 							case 'r': 
		 								fAtpt.x= /*rleft= */ MAX( rleft, rpara->right - itemPtr->rightInset - width); 
		 								break;
		 							case 'c': { 
		 								short rhalf= ((rpara->right - itemPtr->rightInset - rleft) - width) / 2; 
		 								if (rhalf>0) fAtpt.x= rleft += rhalf;
		 								}
		 								break;
		 							}
		 						}
	      	  	}
  					isnewline= FALSE;
  					}
  					
    			num = 0;
    			while ( ch == '\t') ch= ptext[++num];
          while ( ch != '\0' && ch != '\n' && ch != '\t' 
          	&& ( (num + ptext - text) < nextofs))   
            	ch = ptext[++num];
 
          if (row >= frst) {
				    Boolean inrect;

 //Debug("r");
            rct.top = rpara->top + (row - frst) * lineHeight;
            rct.bottom = rct.top + lineHeight;
            rct.left = fAtpt.x;  
            rct.right = rpara->right - itemPtr->rightInset; 
						if (doborder && row == itemPtr->numRows-1 
							&& (itemPtr->borderbottom || itemPtr->borderbox)) 
								rct.bottom--;
            Nlm_LoadRect (&fDrawr, MAX (rct.left, rpara->left), rct.top,
                      MIN (rct.right, rpara->right), rct.bottom);

#if 1
						inrect= (rct.top <= rpara->bottom && rct.bottom > rpara->top);
#else
					 	inrect= Nlm_SectRect( &fDrawr, rpara, &rsect);
#endif
            if ( inrect && (mapPt || forPrinter || Nlm_updateRgn == NULL ||
            		 Nlm_RectInRgn (&rct, Nlm_updateRgn))) {
              fDrawr = rct;
							fDrawr.top += superSub; 
							fDrawr.bottom += superSub;  
// Debug1("%d",num);
              if (num > 0  && !(astyle->hidden)) {
// Debug("i");
              	just= 'l';
      	     		bartab= FALSE;
             		dotab= FALSE;

        				/* yfont= fDrawr.top + Nlm_Ascent();  /* a. large font mismatch */
       					/* yfont= fDrawr.bottom - Nlm_Descent() - Nlm_Leading(); /* b. better than a */
       					/* yfont= fDrawr.bottom - Nlm_Descent(); /* c. almost perfect, slight font mismatch */
        				/* yfont= fDrawr.top + Nlm_FontHeight(); /* d. bad, too low, font mismatch */
       					yfont= fDrawr.bottom - 4; /* Nlm_stdDescent; /* e. !! YES, but what is right ## ?? */
       					/* yfont= fDrawr.top + Nlm_FontHeight() - Nlm_Descent(); /* f. ?? */

                while (num > 0 && *ptext == '\t') {
                  dotab= TRUE;
                  num--;
                  ptext++;
									itemPtr->SetTab( *ptext == '\t', rleft, fDrawr, rpara, bartab, just);
	                fAtpt.x = fDrawr.left; /* ??? */
                	}
                	
    						if (dotab) 
    							itemPtr->DrawTab( this, rct, fDrawr, fAtpt, yfont, bartab, fDodraw);

		            if (astyle->allcaps || astyle->smallcaps) { 
		            	char* cp= ptext;
		            	for (short i=0; i<num; i++) cp[i]= to_upper(cp[i]);
		            		// need to shift fontsize down for lowercases 
		            	}

        				txtwidth= Nlm_TextWidth(ptext, num); 
        				 
              	switch (just) {
             			case 'c': 
             				rright= (fDrawr.left + txtwidth + fDrawr.right)/ 2; 
										fDrawr.left += (fDrawr.right - fDrawr.left - txtwidth) / 2; 
             				break;
             			case 'r':
             				rright = fDrawr.right; 
             				fDrawr.left = fDrawr.right - txtwidth; 
             				break;
             			default:
             			case 'l':
       							/* rright= fDrawr.left + astyle->pixwidth; */
        						rright= fDrawr.left + txtwidth + 1;
             				break;
             			}
             			
             		/* these are no good for multilines w/ same style, & resizing changes */
								if (astyle->pixwidth==0) astyle->pixwidth= rright - fAtpt.x;
								astyle->pixleft= fAtpt.x;

#if 0
	// this looks too messy !!
	// ?? quick fix for html text that is off screen to left or right ?
								if (rright > rpara->right) Nlm_OffsetRect( &fDrawr, rpara->right-rright, 0);
								if (fDrawr.left < rpara->left) fDrawr.left= rpara->left;
#endif
								
             		Nlm_MoveTo(fDrawr.left, yfont);  // yfont == adjust for leading
             		if (fDodraw) {
             			csave= ptext[num]; ptext[num]= 0;
             			Nlm_PaintString(ptext);
                 	ptext[num]= csave;									
			      			}

             		else if (mapPoint && Nlm_PtInRect( *mapPoint, &fDrawr)) {
             			if (astyle->ispict) {
#if 0
										astyle->fObject->FindPt( fDrawr, pt);
										// return isInPict && point-rel to pict && astyle or styleid !!
#endif
		        				fAtpt.x= rright;
		             		fDrawr.left= fAtpt.x;  
	             			gMapOfs= (short)(ptext - text); // pict point ??
            		 	  fMapStyle= astyle;
	             			return gMapOfs; // text point..
             				}
             			else {
	             			// return also num chars !?
	             			short atx= mapPoint->x - fDrawr.left;
	             			short	i= num;
	             			if (Nlm_TextWidth(ptext, i) >= atx) {
		             			while (i>0 && Nlm_TextWidth(ptext, i) > atx) i--;
		             			gMapOfs= (short)(ptext + i - text);
											fMapStyle= astyle;
	
		             				// !! not finding proper style (for links)
		             			short ist= istyle;
		             			short nof= nextofs;
		             			short sid= styleid;
		             			while ( gMapOfs >= nof && ist < numStyles-1 ) {
									     	ist++;
									     	sid= itemPtr->fStyleIndex[ist].styleid;
									     	nof= itemPtr->fStyleIndex[ist].nextofs;
												fMapStyle= GetStyle(sid);
												}
		             			return gMapOfs; // text point..
	             				}
	             			else {
	             				// ??? continue on for more of ptext, if proper line ...
	             				// there must be some mistake in fDrawr if we get here??
	             				}
	             			}
             			}

								if (astyle->linkid && fDodraw) {
								  if (!forPrinter) {
 										Nlm_SetColor(astyle->color);
#if 1
										Nlm_WidePen(2);
										/* Nlm_Dotted(); /* Nlm_Dashed(); */
										Nlm_MoveTo( fDrawr.left, fDrawr.bottom-2);
										Nlm_LineTo( rright-1, fDrawr.bottom-2);
										Nlm_WidePen(1);
										Nlm_Solid();
#else
										fDrawr.right= rright-1;
										Nlm_FrameRect( &fDrawr);
#endif
										Nlm_SetColor(fCurrentStyle->color);
										}
									}
								else if (astyle->boxed && fDodraw) {
									fDrawr.right= rright-1;
									Nlm_FrameRect( &fDrawr);
									}

								Nlm_Black(); /* need this or something like to fix color bug in XWin */

								if (drawSelection) {
									short textofs= ptext - text; 
									short textend= textofs + num; 
									Nlm_RecT selr= fDrawr;
									Boolean showsel= true;
									short  baseleft= selr.left;
									
									if (item == fSelParag) {
										if (textend < fSelChar) showsel= false;
										if (showsel && textofs < fSelChar) 
											selr.left = MIN( selr.right, 
												baseleft + Nlm_TextWidth( ptext, fSelChar - textofs));
										}
										
									if (showsel && item == fSelEndParag) {
									  if (textofs > fSelEndChar) showsel= false;
										if (showsel && textofs < fSelEndChar) //textend > fSelEndChar
											selr.right = MAX(selr.left, 
												baseleft + Nlm_TextWidth( ptext, fSelEndChar - textofs));
										}

									if (showsel) Nlm_InvertRect( &selr);
									}
									
									
        				fAtpt.x= rright;

               	if (astyle->strikeout && fDodraw) {
               		short dmid= (fDrawr.top + fDrawr.bottom) / 2;
               		Nlm_MoveTo( fDrawr.left, dmid);
               		Nlm_LineTo( fAtpt.x, dmid);
               		}
             		fDrawr.left= fAtpt.x;  
 
								if (astyle->ispict) {
									fDrawr.right = fDrawr.left + astyle->pixwidth;
									fDrawr.bottom= fDrawr.top + astyle->pixheight;
 					     		fAtpt.x= fDrawr.right+1;
									if (fDodraw) {
										if (astyle->fObject) astyle->fObject->Draw( fDrawr);
										Nlm_ResetDrawingTools();
										}
									fDrawr.left= fAtpt.x;
									}
               	}
             	fDrawr.right = rct.right;
              
							if (doborder && fDodraw) {
								itemPtr->DrawBorder( this, row, item, rpara, rct);
 								doborder= FALSE;
								}

              }
          	}
					if (!num) ptext++; else ptext += num;
        	}
        ch = *ptext;
     		}
      rct.top = rct.bottom;
      rsult = rct.top - rpara->top;
    	}
    Nlm_SelectFont(Nlm_programFont);  //Nlm_systemFont
  	}
  if (mapPt) return -1;   // couldn't find mapPoint if we got here
  else return rsult;
}





char* DRichView::DataToChar(char* ptr)  
{
  if (ptr) 
    return Nlm_StringSave((char*)ptr);
  else 
    return NULL;
}


void DRichView::FormatText( short item, DParagraph* itemPtr, Boolean byPixels)
{
		// parses and rearranges the text so that it can be displayed, saved, or printed
  char       	ch;
  Boolean    	needToParse;
  short       numRows;
  char*    		text;
	Nlm_RecT		r;
	
	/* dgg &fix this */

  if (itemPtr && itemPtr->fText == NULL) {
		this->ViewRect( r);
    needToParse = TRUE;
		text = this->DataToChar(itemPtr->fDataPtr);
  		
    if (needToParse && text && *text != '\0') {
	  	if (*text == '\n' && text[1] == '\0') {
	      itemPtr->fText = text;
	      itemPtr->numRows = 1;
	   		}
      else {
      	ParseText(itemPtr, text, byPixels, r);
      	MemFree(text);
      	}
      itemPtr->notCached = FALSE;
      itemPtr->neverCached = FALSE;
    	} 
    else  
      itemPtr->fText = text;
     
    text = itemPtr->fText;
    if (text == NULL || *text == '\0') {
      MemFree(itemPtr->fText);
      itemPtr->fText = StringSave(" \n");
      itemPtr->numRows = 1;
      itemPtr->notCached = FALSE;
      itemPtr->neverCached = FALSE;
    	} 
    else if (fIsVirtual) {
      numRows = 0;
      ch = *text;
      while (ch != '\0') {
        if (ch == '\n') numRows++;
        text++;
        ch = *text;
      	}
      numRows = MAX (1, numRows);
      itemPtr->numRows = MAX (numRows, itemPtr->minLines);
    	}
  }
}


short DRichView::GetLineStart(short item)
{
			// Returns the first line of the item
  DParagraph*  itemPtr = GetParaPtr(item);  
  if (itemPtr) return itemPtr->startsAt;
  else return 0;
}

short DRichView::GetNumRows(short item)
{
				//   Returns the number of rows in the item
  DParagraph*  itemPtr  = GetParaPtr( item);  
  if (itemPtr)  return itemPtr->numRows;
  else return 0;
}

short DRichView::GetLineHeight(short item)
{
		//     Returns the pixel height of the item
  DParagraph*  itemPtr  = GetParaPtr( item);  
  if (itemPtr)  return itemPtr->lineHeight;
  else return 0;
}




	
Boolean DRichView::Doc2ViewOffset(short& xoff, short& yoff)
{
  Nlm_RecT 			viewr, rct;
  short     		atline, itemNum, leadHeight, lineHeight, numRows, off;
  Nlm_BaR      	sb;
  DParagraph*  	itemPtr;
	
	ViewRect( viewr);
	Nlm_LoadRect( &rct, 0, 0, viewr.right - viewr.left, 0);
  sb = Nlm_GetSlateVScrollBar((Nlm_SlatE) fPanel);
	if (sb) off = Nlm_GetValue(sb); // line offset of view
	else off= 0;
	
	//for (atline=0; atline<= off; atline++)  
	atline = 0;
	leadHeight = 0;
	itemNum = 0; //= GetItemNum( atline);
	while (itemNum < fNumItems && atline < off) {
		itemPtr = GetParaPtr( itemNum);
	  if (itemPtr) {
			rct.top = rct.bottom + leadHeight;
      CacheIfNever( itemNum + 1, itemPtr, &viewr);
  		if (atline>0) rct.top += itemPtr->spaceBefore;
      lineHeight = itemPtr->lineHeight;
      rct.top += leadHeight;
      numRows = itemPtr->numRows;
      rct.bottom = rct.top + lineHeight * (numRows) + itemPtr->spaceAfter;
   		leadHeight = itemPtr->leadHeight;
  		atline += numRows;
			}
		itemNum++;
		}

	xoff = rct.left;
	yoff = rct.top; // - viewr.top; // ??? was +top
	return true;
}	

	
Boolean DRichView::MapDoc2View( Nlm_RecT& selr)
{
  Nlm_RecT 	viewr, rct;
	short		xoff, yoff;

	Doc2ViewOffset(xoff, yoff);
	selr.left	 	-= xoff;
	selr.right 	-= xoff;
	selr.top	 	-= yoff;
	selr.bottom -= yoff;
	
	ViewRect( viewr);
#ifdef DBUG
	return true;		// !! SectRect always false for WIN_MOTIF !!!!!		
#else
	Boolean visible= Nlm_SectRect( &selr, &viewr, &rct);
	return visible;
#endif
}

void DRichView::MapView2Doc( Nlm_RecT& selr)
{
	short		xoff, yoff;
	
	Doc2ViewOffset(xoff, yoff);
	selr.left	 	+= xoff;
	selr.right 	+= xoff;
	selr.top	 	+= yoff;
	selr.bottom += yoff;
}
	
void DRichView::MapView2Doc( Nlm_PoinT& mpt)
{
	short		xoff, yoff;
	
	Doc2ViewOffset(xoff, yoff);
	mpt.x	 	+= xoff;
	mpt.y	 	+= yoff;
}


Boolean	DRichView::HiliteText(short startparag, short startchar, 
															short endparag, short endchar, Boolean turnon) 
{ 
			// Finds the visible region of an startparag in a document
  short     		firstItem, fstLine, itemNum, leadHeight,lineHeight, numRows, off;
  Nlm_RecT     	viewr, rct;
  Boolean  			goOn, rsult;
  Nlm_BaR      	sb;
  DParagraph*  	itemPtr;

  rsult = FALSE;
 	//top = 0;
 	//bottom = 0;
  //firstline = 0;
 
	ViewRect( viewr);
  sb = Nlm_GetSlateVScrollBar((Nlm_SlatE) fPanel);
  if (sb) off = Nlm_GetValue(sb);
  else off= 0;
  if (startparag >= 0 && startparag < fNumItems) {
    goOn = TRUE;
    Nlm_LoadRect(&rct, viewr.left, viewr.top, viewr.right, viewr.top);
    itemNum = GetItemNum( off);
    itemPtr = GetParaPtr( itemNum);
    if (itemPtr) {
      fstLine = off - itemPtr->startsAt;
      firstItem = itemNum;
      lineHeight = 0;
      leadHeight = 0;
      numRows = 0;
      while (goOn && itemNum < fNumItems && itemNum <= startparag) {
        itemPtr = GetParaPtr( itemNum);
        if (itemPtr) {
          CacheIfNever( itemNum + 1, itemPtr, &viewr);
          if (itemNum > firstItem)  
            leadHeight = itemPtr->leadHeight;
        	else  
            leadHeight = 0;
      		if (fstLine==0) rct.top += itemPtr->spaceBefore;
          
          lineHeight = itemPtr->lineHeight;
          rct.top += leadHeight;
          numRows = MIN (itemPtr->numRows,
                         MAX ((viewr.bottom - rct.top) / lineHeight, 0));
          rct.bottom = rct.top + lineHeight * (numRows - fstLine);
      		rct.bottom += itemPtr->spaceAfter;
          if (itemNum == startparag) {
            //goOn = FALSE;
            if (numRows > 0) {
            	//top = rct.top;
            	//bottom = rct.bottom;
            	//firstline = fstLine;
            	fSelRect.top= rct.top;
            	fSelRect.bottom= rct.bottom;
            	//gMapOfs
            	fSelRect.left= rct.left; // this is bogus, just is left edge...
            	fSelRect.right= rct.right; // this also is bogus
              rsult = TRUE;
            	}
          	} 
          else  
            rct.top = rct.bottom;
          if (numRows < itemPtr->numRows)  
            goOn = FALSE;
        	}
        itemNum++;
        fstLine = 0;
      	}
    	}
  	}
	fSelRect.bottom= rct.bottom; //????
 	MapView2Doc( fSelRect); //?? need to get it working ??
  return rsult;
}




void DRichView::HiliteSelection(Boolean turnon)
{
	if (fSelected) {
		Nlm_RecT viewr, selr;
		
		selr= fSelRect;
		Boolean visible= MapDoc2View( selr);
		// !! must map sel location to current view (w/ sb) and see if it
		// is in view window !!
		
		if (!visible) 
			return;
		else {
				// getting cases where this is called when this view's window is NOT front
			//DWindow* savewin=  gWindowManager->SavePort(this);
			if (fSelInPict) {
				//Nlm_InvertRect( &selr);
				Nlm_InvertMode();
				Nlm_FrameRect( &selr);
				Nlm_CopyMode();
				}
			else {
				ViewRect( viewr);
				selr.left= viewr.left;
				selr.right= viewr.right;
				selr.top= MAX( viewr.top, selr.top - 20);  // expand inval area for full line
				selr.bottom = MIN( viewr.bottom, selr.bottom ); 
				InvalRect( selr); // let DrawStyledItem do work ?
				Boolean savesel= fSelected;
				fSelected= turnon;
				Nlm_Update();  // this->Draw();
				fSelected= savesel;
				}
			//if (savewin) gWindowManager->RestorePort(savewin);
			}
		}
}

void DRichView::ClearSelection()
{
		//?? before unHilite to cure draw bug ??
	fSelParag= fSelChar= fSelEndParag= fSelEndChar= 0;
	HiliteSelection(false);
	//fSelParag= fSelChar= fSelEndParag= fSelEndChar= 0;
	fSelInPict= false;
	fSelected= false;
}


void DRichView::SetSelectedText( short startparag, short startchar, 
																	short endparag, short endchar)
{
	ClearSelection(); // clear any old sel -- i.e., wipe view markings
	fSelInPict= false; //!! check if selrect is in a picture !!
	fSelParag	= Max(0, Min(startparag, fNumItems)); 	 
	fSelChar	= Max(0, startchar); 	 	// check max !? 
	
	fSelEndParag	= Max(0, Min(endparag, fNumItems)); 	 
	// if endchar==0 endchar== maxchar ??
	fSelEndChar	  = Max(0, endchar); 	// check max !? 
	fSelected= (fSelParag > 0 || fSelChar > 0 || fSelEndChar > 0);

			// !! NEED THIS parag/char -> fSelRect mapper !!
	if (HiliteText( fSelParag, fSelChar, fSelEndParag, fSelEndChar, false) ) ;

	HiliteSelection(fSelected);
}

void DRichView::SelectAll()
{
	short					itemNum, leadHeight, atline;
  Nlm_RecT			viewr, rct;
  DParagraph*  	itemPtr;
	
	ClearSelection(); // clear any old sel -- i.e., wipe view markings
	ViewRect( viewr);
	Nlm_LoadRect( &rct, 0, 0, viewr.right - viewr.left, 0);
	atline = 0;
	leadHeight = 0; 
	itemNum = 0; //= GetItemNum( atline);
	while (itemNum < fNumItems) {
		itemPtr = GetParaPtr( itemNum);
	  if (itemPtr) {
      CacheIfNever( itemNum + 1, itemPtr, &viewr); // this counts item->numRows
      if (atline>0) rct.bottom += itemPtr->spaceBefore; 
      rct.bottom += leadHeight  
      						+ itemPtr->lineHeight * (itemPtr->numRows) 
      						+ itemPtr->spaceAfter;
   		leadHeight = itemPtr->leadHeight;
  		atline += itemPtr->numRows;
			}
		itemNum++;
		}

	fSelRect= rct;  
	//MapView2Doc( fSelRect);
	fSelInPict= false; //!! check if selrect is in a picture !!
	fSelParag= 1; 	// parag # are +1 from MapPoint !!
	fSelChar= 0; 	 
	fSelEndParag= fNumItems; 		// parag # are +1 of item# from MapPoint !!
	fSelEndChar= 0;  	 
	fSelected= (fSelParag > 0 && fSelEndParag > 0);
	HiliteSelection(fSelected);
}


void DRichView::MarkSelection( Nlm_RecT selrect)
{
	Nlm_PoinT pt;
	short			row, parag;
	Nlm_RecT	arect;
	char			word[50];
	
	ClearSelection(); // clear any old sel -- i.e., wipe view markings
	fSelRect= selrect; // ?? don't we need to map selrect in view coords to doc coords !?
	MapView2Doc( fSelRect);
	fSelInPict= false; //!! check if selrect is in a picture !!

	pt.x= selrect.left;
	pt.y= selrect.top;
	MapPoint( pt, parag, row, &arect, word, 40, &fSelStyle);
	fSelParag= parag; 	// parag # are +1 from MapPoint !!
	fSelChar= gMapOfs; 	// dang global... fix mappoint to return it!
						// fSelChar was correct
	if (fSelStyle.ispict) {
		fSelInPict= true;
			// ??? must make fSelRect relative to pict frame
		//fSelRect.left -= fDrawr.left;
		//fSelRect.right -= fDrawr.left;
		//fSelRect.top -= fDrawr.top;
		//fSelRect.bottom -= fDrawr.top;
		fSelEndParag= 0;
		fSelEndChar= 0;
		fSelected= (fSelParag > 0);
		}
	else {	
		pt.x= selrect.right;
		pt.y= selrect.bottom;
		MapPoint( pt, parag, row, &arect, word, 40, NULL);
		fSelEndParag= parag; 		// parag # are +1 of item# from MapPoint !!
		fSelEndChar= gMapOfs;  	// fSelEndChar is too large ???
		fSelected= (fSelParag > 0 && fSelEndParag > 0);
		}
	HiliteSelection(fSelected);
}


Boolean DRichView::GetSelection( Boolean& inpict,
					short& selparag, short& selchar, 
					short& selendparag, short& selendchar,
					Nlm_RecT& selrect, DRichStyle* selstyle)
{
	inpict= fSelInPict;
	selparag= fSelParag - 1; // !!!!!! FIX DAMN +1 used above !!!!!!
	selchar= fSelChar;
	selendparag= fSelEndParag - 1; // !!!!!! FIX DAMN +1 used above !!!!!!
	selendchar= fSelEndChar;
	selrect= fSelRect; // !? must we map from doc coords to view coords ??
	if (selstyle) { *selstyle = fSelStyle; } //style at start of selection (for pict !?)
	return fSelected;
}

char* DRichView::GetSelectedText()
{
	long startp, endp, startc, endc;
	char* theText= NULL;

	if (fSelInPict) {
		return NULL; // for now, fix to copy pict to system clipboard
		}
		
	if (fSelParag) {
		// if selection == NULL ?? return all of text ????
		startp= fSelParag - 1; 
		endp= fSelEndParag - 1;
		startc= fSelChar;
		endc= fSelEndChar;
		}
	else {
		startp= 0; 
		endp= fNumItems-1;
		startc= 0;
		endc= 0;
		}
	
	for (short par= startp; par<= endp; par++) {
			// need to handle start/end chars ...
		DParagraph* itemPtr = GetParaPtr( par);
		if (itemPtr) {
			Boolean first= (par == startp);
			Boolean last = (par == endp);
			char  savec = 0;
			char* partext= (char*) itemPtr->fDataPtr;
#if 1
			if (first) partext += startc;
			if (last && endc) {
				savec= partext[endc+1];
				partext[endc+1]= 0;
				}
#endif
			if (theText) StrExtendCat( &theText, partext);
			else theText= StrDup( partext);
			StrExtendCat( &theText, LINEEND); // append newline for each parag ?
			if (savec) partext[endc+1]= savec;
			}
		}
		 
	return theText;
}



short DRichView::UpdateLineStarts(short first)
{
			//  Recalculates the first lines as a running sum of the number of rows
  short     i;
  DParagraph*  itemPtr;
  short     totalNumLines;

  totalNumLines = 0;
	itemPtr  = GetParaPtr( first);
  if (itemPtr) totalNumLines = itemPtr->startsAt;
  for (i = first; i < fNumItems; i++) {
    itemPtr = GetParaPtr( i);
    if (itemPtr) {
      itemPtr->startsAt = totalNumLines;
      totalNumLines += itemPtr->numRows;
    	}
  	}
  return totalNumLines;
}


void DRichView::CollectRange(short item, short& lowest, short& highest)
{
  lowest  = MIN(lowest, item);
  highest = MAX(highest, item);
}



void DRichView::CacheAndFormat(short item, DParagraph* itemPtr, Nlm_RectPtr r)
{
				//  Formats text (for drawing) if the item text is not currently cached
  if (itemPtr && r && itemPtr->notCached) FormatText(item, itemPtr, TRUE);
}

void DRichView::CacheIfNever( short item, DParagraph* itemPtr, Nlm_RectPtr r)
{
		//     Formats text (for obtaining the correct number of rows) if the item
		//     text has never been cached
  if (itemPtr && r && itemPtr->neverCached) FormatText(item, itemPtr, TRUE);
}


void DRichView::FreeCachedItem( short item, short& lowest, short& highest)
{
		// Frees the formatted text string for an item that is not currently visible
  DParagraph*  itemPtr = GetParaPtr( item);
  if (itemPtr) {
    itemPtr->fText = (char*) MemFree (itemPtr->fText);
    itemPtr->notCached = TRUE;
    if (fIsVirtual && fNumItems > 0) {
      itemPtr->numRows = 30000 / fNumItems;
      CollectRange(item, lowest, highest);
    	}
  	}
}


short DRichView::VisLinesAbove(Nlm_RectPtr r, short item,
                           short line, short& lowest, short& highest)
{
		//    Returns the number of lines visible on the screen if the given line
		//    in the given item is the last line visible
  DParagraph*  itemPtr;
  short     pixels, vis;

  vis = 0;
  if ( r && item < fNumItems) {
    pixels = r->bottom - r->top;
    while (pixels > 0 && item >= 0) {
      itemPtr = GetParaPtr( item);
      if (itemPtr) {
        CollectRange( item, lowest, highest);
        CacheIfNever( item + 1, itemPtr, r);
        if (line < 0)  line = itemPtr->numRows - 1;
        while (pixels > 0 && line >= 0 && line < itemPtr->numRows) {
          pixels -= itemPtr->lineHeight;
          if (pixels >= 0)  vis++;
          line--;
        	}
        pixels -= itemPtr->leadHeight;
        pixels -= itemPtr->spaceBefore + itemPtr->spaceAfter;
      	}
      item--;
    	}
  	}
  return vis;
}



short DRichView::VisLinesBelow( Nlm_RectPtr r, short item,
                           			short line, short& lowest, short& highest)
{
		//    Returns the number of lines visible on the screen if the given line
		//    in the given item is the first line visible
  DParagraph*  itemPtr;
  short     pixels, vis;

  vis = 0;
  if (r && item < fNumItems && item >= 0) {
    pixels = r->bottom - r->top;
    while (pixels > 0 && item < fNumItems) {
      itemPtr = GetParaPtr( item);
      if (itemPtr) {
        CollectRange(item, lowest, highest);
        CacheIfNever(item + 1, itemPtr, r);
        if (line  < 0) {
          line = 0;
          pixels -= itemPtr->leadHeight;
        	pixels -= itemPtr->spaceBefore + itemPtr->spaceAfter;
        	}
        while (pixels > 0 && line >= 0 && line < itemPtr->numRows) {
          pixels -= itemPtr->lineHeight;
          if (pixels >= 0) vis++;
          line++;
        	}
        line = -1;
      	}
      item++;
    	}
  	}
  return vis;
}



short DRichView::PixelsBetween( Nlm_RectPtr r, short firstLine,
                           short lastLine, short& lowest, short& highest)
{
 			//   Returns the number of pixels between two lines
 	short     count;
  short     item;
  DParagraph*  itemPtr;
  short     line;
  short     numLines;
  short     onItem;
  short     pixels;

  pixels = 0;
  if ( r ) {
  	// dgg -- are these line no.s off by one??  came from Scroll()
  	// firstLine--; lastLine--;
    count = ABS (lastLine - firstLine);
    item = GetItemNum( MIN (firstLine, lastLine));
    itemPtr = GetParaPtr( item);
    onItem = item;
    if (itemPtr != NULL && item >= 0) {
      CollectRange( item, lowest, highest);
      CacheIfNever( item + 1, itemPtr, r);
      line = MIN (firstLine, lastLine) - itemPtr->startsAt;
      while (count > 0 && item < fNumItems) {
        if (item != onItem) {
          itemPtr = GetParaPtr( item);
          onItem = item;
        	}
        if ( itemPtr ) {
          CollectRange (item, lowest, highest);
          CacheIfNever ( item + 1, itemPtr, r);
          numLines = itemPtr->numRows;
#if 1
					if (line == 0) pixels += itemPtr->spaceBefore;
					if (MAX(firstLine,lastLine) >= numLines + itemPtr->startsAt)
						pixels += itemPtr->spaceAfter;
#else
		// ?? old here is bad -- don't do spaceBefore unless this is 1st line in parag
		// likewise spaceafter if this is last
		    	pixels += itemPtr->spaceBefore + itemPtr->spaceAfter;  
#endif
          while (count > 0 && line >= 0 && line < numLines) {
            pixels += itemPtr->lineHeight;
            line++;
            count--;
          	}
        	}
        item++;
        if (line >= numLines) {
          itemPtr = GetParaPtr( item);
          onItem = item;
          if (itemPtr != NULL) {
            CollectRange (item, lowest, highest);
            CacheIfNever ( item + 1, itemPtr, r);
            pixels += itemPtr->leadHeight;
          	}
        	}
        line = 0;
      	}
      if (lastLine < firstLine)  pixels = -pixels;
	    }
  	}
  return pixels;
}



void DRichView::Draw()
{
			//    Panel callback that traverses the item list and draws visible items
			//    Clipping is to the intersection of the Nlm_updateRgn (the portion of the
			//    panel exposed) and the inset area of the document, leaving a four-
			//    pixel margin

  Nlm_RegioN   dst;
  DParagraph*  itemPtr;
  short     	 firstLine, highest, item, lowest, off, pixels;
  Nlm_RecT     r, rct;
  Nlm_BaR      sb;
  Nlm_RegioN   src;

  if (fNumItems > 0) {
//Debug("DRichView::Draw\n");
		ViewRect( r);
    sb = Nlm_GetSlateVScrollBar((Nlm_SlatE) fPanel);
		if (sb) off = Nlm_GetValue(sb);
		else off= 0;
    src = Nlm_CreateRgn ();
    dst = Nlm_CreateRgn ();
    Nlm_LoadRectRgn (src, r.left, r.top, r.right, r.bottom);
    Nlm_SectRgn (src, Nlm_updateRgn, dst);
    Nlm_ClipRgn (dst);
    Nlm_DestroyRgn (src);
    Nlm_DestroyRgn (dst);
    pixels = 0;
    lowest = 30000; //INT2_MAX; // change for mswin - need??
		highest = -10000; //INT2_MIN;
    item = GetItemNum( off);
    itemPtr = GetParaPtr( item);
    if (itemPtr) {
      firstLine = off - itemPtr->startsAt;
      do {
        if (itemPtr) {
          if (itemPtr->neverCached)  CollectRange(item, lowest, highest);
          CacheAndFormat( item+1, itemPtr, &r);
          if (pixels != 0)  r.top += itemPtr->leadHeight;
  				if (firstLine == 0) r.top += itemPtr->spaceBefore;
          rct = r;
          rct.bottom = rct.top + itemPtr->lineHeight * (itemPtr->numRows - firstLine);
          Boolean inrgn= (Nlm_updateRgn == NULL || Nlm_RectInRgn (&rct, Nlm_updateRgn));
          if (inrgn) {
//Debug(".");
            pixels = DrawStyledItem( itemPtr, &r, item+1, firstLine, FALSE, NULL);
          	} 
          else  
            pixels = rct.bottom - rct.top;
           
          r.top += pixels;
  				r.top += itemPtr->spaceAfter;
        	}
        item++;
        firstLine = 0;
        itemPtr = GetParaPtr( item);
      } while (r.top < r.bottom && item < fNumItems);
      if (lowest < INT2_MAX) 
        fNumLines = UpdateLineStarts( lowest);
     
    }
    Nlm_ResetClip ();
     
  }
	Nlm_SelectFont(Nlm_programFont);  //Nlm_systemFont
//Debug("DRichView::Draw done\n");
}








void  DRichView::AdjustScroll() 
{
			// Calculates an estimate for the scroll bar maximum
  short     barmax,barval,firstShown,highest, item;
  short     line,lineInto,lowest;
  short     pgDn, pgUp, vis;
  DParagraph*  itemPtr;
  Nlm_RecT     r;
  Nlm_BaR      sb;
 
  sb = Nlm_GetSlateVScrollBar((Nlm_SlatE) fPanel);
  if (sb) {
    if (fNumLines > 0 && fNumItems > 0) {
  		Nlm_SelectFont(Nlm_programFont);  //Nlm_systemFont
			ViewRect( r);
      barval = Nlm_GetValue (sb);
      item = GetItemNum ( barval);
      line = barval - GetLineStart( item);
      firstShown = item;
      lineInto = line;
      lowest = INT2_MAX;
      highest = INT2_MIN;
      itemPtr = GetParaPtr(fNumItems - 1);
      if (itemPtr) 
        vis = VisLinesAbove ( &r, fNumItems - 1,
                             itemPtr->numRows - 1, lowest, highest);
      pgUp = VisLinesAbove ( &r, item, line, lowest, highest) - 1;
      pgDn = VisLinesBelow ( &r, item, line, lowest, highest) - 1;
      if (pgDn < 1) pgDn = 1;
      if (pgUp < 1) pgUp = 1;
      if (lowest < INT2_MAX) 
        fNumLines = UpdateLineStarts( lowest);
      
      itemPtr = GetParaPtr(fNumItems - 1);
      if (itemPtr) {
        barmax = 0;
			  ViewRect( r);
        lowest = INT2_MAX;
        highest = INT2_MIN;
        vis = VisLinesAbove ( &r, fNumItems - 1,
                             itemPtr->numRows - 1, lowest,  highest);
        if (lowest < INT2_MAX) 
          fNumLines = UpdateLineStarts ( lowest);
         
        barmax = fNumLines - vis;
        barval = GetLineStart(firstShown) + lineInto;
        Nlm_CorrectBarPage (sb, pgUp, pgDn);
        if (barval > fBarmax) {
          Nlm_CorrectBarMax (sb, barmax);
          Nlm_CorrectBarValue (sb, barval);
        	} 
        else {
          Nlm_CorrectBarValue (sb, barval);
          Nlm_CorrectBarMax (sb, barmax);
        	}
        fBarmax = barmax;
      	}
    	} 
    else  
      Nlm_Reset (sb);
  	}
}






void  DRichView::UpdateItemHeights(DParagraph* aParag)
{
			// Returns the maximum height of all fonts used in an item
  short     	i, linhgt,maxhgt;

  if (aParag) {
    maxhgt = 0;
    i = aParag->fNumStyles;
    while (i > 0) {
      i--;
      DRichStyle* st= GetStyleAtIndex(aParag, i);
   		linhgt = GetFontHeight(st->font) + abs(st->superSub); 
      if (st->ispict) 
 				linhgt = MAX(linhgt, st->pixheight + 2); // +2 for leading !?
      if (linhgt > maxhgt) maxhgt = linhgt;
    	}
    if (maxhgt == 0)  maxhgt = Nlm_stdLineHeight;
    aParag->lineHeight = MAX(maxhgt, aParag->minHeight);
    if ( aParag->openSpace)  
      aParag->leadHeight = maxhgt;
    else 
      aParag->leadHeight = 0;
  	}
}

 
short DRichView::SkipPastNewLine(char* text, short cnt)
{
  char  ch;
  ch = *(text + cnt);
  while (ch != '\0' && ch != '\n' && cnt < 16380) {
    cnt++;
    ch = *(text + cnt);
  	}
  while ((ch == '\n' || ch == '\r') && cnt < 16380) {
    cnt++;
    ch = *(text + cnt);
  	}
  return cnt;
}

void 	DRichView::HandleAddInsRep( handleMode mode, short item, char* text, 
					DParagraph* parFormat, DRichStyle* styles, short nstyles, Nlm_FonT aFont) 
{
	DRichStyle	* aStyle = NULL;
  DParagraph 	* para = NULL;
	
	if (!parFormat) {
		para= new DParagraph();
		parFormat= para;
		}
	
	if (!styles) {
		aStyle= new DRichStyle();
		if (aFont) aStyle->font= aFont;
		aStyle->last= true;
		styles= aStyle;
		nstyles= 1;
		}
		
	if (text && *text != '\0') {
	  long start = 0;
	  short atstyle= 0;
	  long cntr = StringLen (text);
	  long cnt = MIN (cntr, 16000);
	  cnt = SkipPastNewLine (text + start, cnt);
	  while (cnt > 0) {
	    char* txt = (char*) MemNew(cnt + 1);
	    MemCopy(txt, text + start, cnt);
	    
	    // damn !!! have to fiddle w/ styles[...].nextofs for this newline cutting stuff !!
	    short ist= atstyle, nst= 0;
	    if (nstyles) {
		    do ist++; 
		    while (ist<nstyles && styles[ist].nextofs <= start + cnt);
		    nst= ist - atstyle;	 
		    }
	    	
	    switch (mode) {
	    	case kAppend: 
	    		Append( txt, (cnt / 50) + 1, parFormat, &styles[atstyle], nst, true);
	    		break;
	    	case kReplace:
					Replace( item, txt, (cnt / 50) + 1, parFormat, &styles[atstyle], nst, true);
	    		break;
    		case kInsert:
					Insert( item, txt, (cnt / 50) + 1, parFormat, &styles[atstyle], nst, true);
	    		break;
	    	}
	    	
	    atstyle += nst - 1;
	    start += cnt;
	    cntr -= cnt;
	    cnt = MIN (cntr, 16000);
	    cnt = SkipPastNewLine (text + start, cnt);
	  	}
		} 
	else switch (mode) {
  	case kAppend: 
			Append( "", 1, parFormat, styles, nstyles, false);
  		break;
  	case kReplace:
			Replace( item, "", 1, parFormat, styles, nstyles, false);
  		break;
		case kInsert:
			Insert( item, "", 1, parFormat, styles, nstyles, false);
  		break;
  	}
  	
  if (para) delete para;
  //if (aStyle) delete aStyle;
}


void 	DRichView::Append( char* text, DParagraph* parFormat, DRichStyle* styles, 
					short nstyles, Nlm_FonT aFont) 
{
	HandleAddInsRep( kAppend, 0,  text, parFormat, styles, nstyles, aFont);
}

void 	DRichView::Replace( short item, char* text, DParagraph* parFormat, 
								DRichStyle* styles, short nstyles, Nlm_FonT aFont) 
{
	HandleAddInsRep( kReplace, item, text, parFormat, styles, nstyles, aFont);
}

void 	DRichView::Insert( short item, char* text, DParagraph* parFormat,
			 DRichStyle* styles, short nstyles, Nlm_FonT aFont) 
{
	HandleAddInsRep( kInsert, item, text, parFormat, styles, nstyles, aFont);
}



void  DRichView::Delete( short item) 
{
				//  Deletes an item from a document
  if (item >= 0 && item < fNumItems) {
		fParags->AtDelete(item);
    fNumItems = fParags->GetSize(); 
    fNumLines = UpdateLineStarts( item);
    if (fAutoAdjust) AdjustScroll();
    }
}


void  DRichView::GetDocStats( short& numParags, short& numLines) 
{
  numParags = fNumItems;
  numLines = fNumLines;  
}



void  DRichView::GetItemStats( short item, short& startsAt, short& numRows, 
		 													short& lineHeight) 
{
	DParagraph* itemPtr = GetParaPtr( item );  // item starts at 0
  if (itemPtr) {
    startsAt = itemPtr->startsAt;
    numRows = itemPtr->numRows;
    lineHeight = itemPtr->lineHeight;
  	}
  else {
	  startsAt = 0;
	  numRows = 0;
	  lineHeight = 0;  
  	}
}





void 	DRichView::MapPoint( Nlm_PoinT pt, short& item, short& row, Nlm_RectPtr rct,
					char* wordAt, short maxword, DRichStyle* styleAt) 
{
				// Converts a mouse point to a item, row positions  within a document
  short     cl,firstItem,firstLine;
  short     itemNum,rw,itm,leadHeight,lineHeight,numRows,off;
  Boolean  	goOn;
  Nlm_RecT 	viewr, paragr, rc;
  Nlm_BaR  	sb;
  DParagraph*  itemPtr;

  itm = 0;
  rw = 0;
  cl = 0;
  Nlm_LoadRect (&rc, 0, 0, 0, 0);

	ViewRect( viewr);
  if (Nlm_PtInRect (pt, &viewr)) {
    sb = Nlm_GetSlateVScrollBar ((Nlm_SlatE) fPanel);
		if (sb) off = Nlm_GetValue(sb);
		else off= 0;
    goOn = TRUE;
    itemNum = GetItemNum( off);
    itemPtr = GetParaPtr( itemNum);
    if (itemPtr) {
			paragr= viewr;
      firstLine = off - itemPtr->startsAt;
      firstItem = itemNum;
      while (goOn && itemNum < fNumItems) {
        itemPtr = GetParaPtr( itemNum);
        if (itemPtr) {
          CacheIfNever( itemNum + 1, itemPtr, &paragr);
          if (itemNum > firstItem)  
            leadHeight = itemPtr->leadHeight;
        	else 
            leadHeight = 0;
          if (firstLine==0) paragr.top += itemPtr->spaceBefore;
       
          lineHeight = itemPtr->lineHeight;
          numRows = MIN (itemPtr->numRows - firstLine,
                         MAX ((viewr.bottom - paragr.top - leadHeight) / lineHeight, 1));
          paragr.bottom = paragr.top + leadHeight + lineHeight * numRows;
       		paragr.bottom += itemPtr->spaceAfter;
       		
         	if (Nlm_PtInRect(pt, &paragr) && numRows > 0) {
            goOn = FALSE;
            itm = itemNum + 1;
            if (pt.y <= paragr.top + leadHeight)  
              rw = 0;
            else  
              rw = firstLine + ((pt.y - paragr.top - leadHeight) / lineHeight) + 1;

          	rc = paragr;
          	if (rw) rc.top += (rw-1) * lineHeight;
          	rc.bottom = rc.top + lineHeight;
          	
            // dgg -- map to style section                 
            if (wordAt || styleAt) {
         	 		short textofs = DrawStyledItem( itemPtr, &paragr, itm/*itemNum+1*/, 
         	 									firstLine/*rw*/, FALSE, &pt);
							if (wordAt) {
								if (textofs<0) {
									StrNCpy(wordAt, "**not found", maxword);
             			wordAt[maxword-1]= 0;
									}
								else {
									char *cbase= itemPtr->fText;
									char *cstart= cbase + textofs;
									if (*cstart <= ' ') {
										if (cstart[1] > ' ') cstart++;
										else if (cstart[-1] > ' ') cstart--;
										}
									char *cend= cstart;
               		while (cstart > cbase && cstart[-1] > ' ') cstart--;
          				while (*cend > ' ') cend++;
               		short len= MAX(1, MIN( maxword-1, cend - cstart));
             			StrNCpy(wordAt, cstart, len);
             			wordAt[len]= 0;
             			}
           			}
              if (styleAt && fMapStyle) {
              	*styleAt= *fMapStyle; // GetStyleAtChar( itemPtr, textofs);
              	}
           		}
           		
          	} 
          else  
            paragr.top = paragr.bottom;
           
          if (numRows < itemPtr->numRows - firstLine)  
            goOn = FALSE;
        	}
        itemNum++;
        firstLine = 0;
        }
      }
    }
 
	item = itm;
	row = rw;
	if (rct) *rct = rc;
}




Boolean DRichView::Find( char* match, short& itemat, short& charat,
							short startitem, short startchar, Boolean caseSensitive) 
{
  DParagraph*		itemPtr;
  short        	i, stop;
	
  if (match) {
		char* tempmatch= StrDup( match);
    if (fNumItems > 0) {
      	stop = fNumItems;
        if (startitem < 0) startitem = 0;
        for (i = startitem; i < stop; i++) {
          itemPtr = GetParaPtr( i);
          if (itemPtr) {
 						char* text;
						text= (char*) itemPtr->fDataPtr;
            if (text) {
							Nlm_Uint4		foundat;
							Boolean 		found 
									= Nlm_StrngPos(text, tempmatch, startchar, caseSensitive, &foundat);
              if (found) {
              	itemat= i;
            		charat= foundat;
								MemFree( tempmatch);
              	return TRUE;
              	} 	 
          		}
          	}
          startchar= 0; // ! zero it !! or we miss others ! 
          }
				}
		MemFree( tempmatch);
		}
	return FALSE;
}




Boolean 	DRichView::FindURL(short& itemat, short& charat, short& charlen,
														short startitem, short startchar) 
{
  DParagraph* 	itemPtr;
  short        	i, stop;
	
	if (fNumItems > 0) {
  	stop = fNumItems;
    if (startitem < 0) startitem = 0;
    for (i = startitem; i < stop; i++) {
      itemPtr = GetParaPtr( i);
      if (itemPtr) {
				char* text;
				// text= (char*) itemPtr->fDataPtr; << this is bad, need fText !
				FormatText( i, itemPtr, TRUE);
				text = itemPtr->fText;
	      if (text) {
					char 			*cp, *ep;

					for (cp= text+startchar+3; *cp; cp++) { 
						if (*cp == ':') {
							if ( (cp[1] == '/' && cp[2] == '/') 
								|| (0 == Nlm_StringNICmp( MAX(text, cp-6), "mailto", 6))
								|| (0 == Nlm_StringNICmp( MAX(text, cp-4), "news", 4))
								/* ^^^^ Should use DURL::IsURL(cp) !! */
								) {
								  cp--;
									while (isalnum(*cp) && cp>text) cp--;
									if (*cp == '"') ep= strchr(cp+1,'"');
									else if (*cp == '\'') ep= strchr(cp+1,'\'');
									else if (*cp == '<') ep= strchr(cp+1,'>');
									else if (*cp == '=' 
										&& 0 == Nlm_StringNICmp( MAX(text,cp-4), "href=", 4 )) 
											ep= strchr(cp+1,'>');
									else ep= NULL;
									if (!isalnum(*cp)) cp++;
								  if (ep == NULL) for (ep=cp; isgraph(*ep); ep++) ;
	              	itemat= i;
	              	charat= cp - text;
									charlen= ep ? ep - cp : 0;
	              	return TRUE;
	              	}
							} 
						}	 
      		}
      	}
      startchar= 0; // ! zero it !! or we miss others ! 
      }
		}

	return FALSE;
}





void 	DRichView::InsertStyle( short atitem, short atchar, short atlength, 
															DRichStyle* theStyle)
{
  DParagraph* 	itemPtr;
	short					numStyles;
	
	itemPtr = GetParaPtr( atitem);
  if (theStyle && itemPtr) {
		short ist, jst;
		numStyles= itemPtr->fNumStyles;
		StyleIndex* si = itemPtr->fStyleIndex;
		short newofs= atchar + atlength;
		short newnum= numStyles + 2;
		StyleIndex* sinew=	
			(StyleIndex*) MemNew( newnum*sizeof(StyleIndex)); 

		short styleid= FindOrAddStyle(theStyle);
		Boolean notat= true, notnew= true;
		for (ist=0, jst= 0; ist<numStyles && jst<newnum; ist++) {
			short ofs= si[ist].nextofs;
			
			if (atchar <= ofs && notat) {
				if (atchar < ofs) {
					sinew[jst]= si[ist];
					sinew[jst].nextofs= atchar;
					if (jst<newnum) jst++;
					}
				else {
					sinew[jst]= si[ist];
					if (jst<newnum) jst++;
					}
				sinew[jst].nextofs= newofs;
				sinew[jst].styleid= styleid;
				if (jst<newnum) jst++;
				notat= false;
				}
			else {  // atchar > ofs
				sinew[jst]= si[ist];
				if (jst<newnum) jst++;
				}
			
			if (newofs <= ofs && notnew) {
				sinew[jst]= si[ist];
				if (jst<newnum) jst++;
				notnew= false;
				}
			}
			
		itemPtr->fStyleIndex= sinew;
		itemPtr->fNumStyles = jst; // newnum
		MemFree( si);
		
		
  	}
	 
}







char* DRichView::GetText(short item, short row) 
{
			//   Returns a string with text from a document.  If item, row
			//   parameters are 0, text is taken from any item, row,
			//   respectively.  The string must be freed by the application
  ByteStorePtr  	bsp;
  char          	ch;
  short          	cl, i;
  short          	k, num;
  short          	rw, start, stop;
  char					* text;
  char					* ptr;
  Nlm_RecT    		r;
  DParagraph*    	itemPtr;

  text = NULL;
  if (item >= 0) {
		ViewRect( r);
    if (fNumItems > 0) {
      bsp = BSNew (0);
      if (bsp != NULL) {
        if (item == 0) {
          start = 0;
          stop = fNumItems;
        	} 
        else if (item <= fNumItems) {
          start = item - 1;
          stop = item;
        	} 
        else {
          start = 0;
          stop = 0;
        	}

#if 1
			if (row < 0) {
        itemPtr = GetParaPtr( start);
        if (itemPtr) {
					text= (char*) itemPtr->fDataPtr;
 		 			// if (text) text= MemDup(text);   //!? caller expects to own this data !? 
					}
				return text;
				}
#else
			if (row < 0) {
        for (i = start; i < stop; i++) {
          itemPtr = GetParaPtr( i);
          if (itemPtr) {
  					if (!text) text = DataToChar(itemPtr->fDataPtr);
						}
					}
	 		 	return text;
				}
#endif

        for (i = start; i < stop; i++) {
          itemPtr = GetParaPtr( i);
          if (itemPtr) {
						DParagraph itemData( itemPtr->fDataPtr, false, itemPtr->numRows, itemPtr, 
												itemPtr->fStyleIndex, itemPtr->fNumStyles, false);
            FormatText(i, &itemData, TRUE);
            text = itemData.fText;
            if (text && *text != '\0') {
              if (i > start && itemPtr->openSpace > 0 && item == 0) 
                BSPutByte (bsp, (short) '\n');
              rw = 0;
              cl = 0;
              ptr = text;
              ch = *ptr;
              while (ch != '\0') {
                if (ch == '\n') {
                  if ((rw + 1 == row) || row == 0)  
                    BSPutByte (bsp, (short) '\n');
                  cl = 0;
                  rw++;
                  ptr++;
                	} 
                else if (ch == '\t') {
                  if ((rw + 1 == row || row == 0)) 
                    BSPutByte (bsp, (short) '\t');
                   
                  cl++;
                  ptr++;
                	} 
                else {
                  num = 0;
                  while (ch != '\0' && ch != '\t' && ch != '\n') {
                    num++;
                    ch = ptr [num];
                  	}
                  if (num > 0  &&
                      (rw + 1 == row || row == 0)) {
                    for (k = 0; k < num; k++) 
                      BSPutByte (bsp, (short) ptr [k]);
                    }
                  ptr += num;
                	}
                ch = *ptr;
              	}
            	}
          	}
        	}
        text = (char*) BSMerge (bsp, NULL);
        BSFree (bsp);
     	 }
   	 }
 	 }
  return text;
}





Boolean	DRichView::IsVisible( short item, short& top, short& bottom, short& firstline) 
{
			// Finds the visible region of an item in a document
  short     		firstItem, fstLine, itemNum, leadHeight,lineHeight, numRows,off;
  Nlm_RecT     	r, rct;
  Boolean  			goOn, rsult;
  Nlm_BaR      	sb;
  DParagraph*  	itemPtr;

  rsult = FALSE;
 	top = 0;
 	bottom = 0;
  firstline = 0;
 
	ViewRect(  r);
  sb = Nlm_GetSlateVScrollBar ((Nlm_SlatE) fPanel);
  if (sb) off = Nlm_GetValue (sb);
  else off= 0;
  if (item > 0 && item <= fNumItems) {
    item--;
    goOn = TRUE;
    Nlm_LoadRect (&rct, r.left, r.top, r.right, r.top);
    itemNum = GetItemNum ( off);
    itemPtr = GetParaPtr( itemNum);
    if (itemPtr) {
      fstLine = off - itemPtr->startsAt;
      firstItem = itemNum;
      lineHeight = 0;
      leadHeight = 0;
      numRows = 0;
      while (goOn && itemNum < fNumItems && itemNum <= item) {
        itemPtr = GetParaPtr( itemNum);
        if (itemPtr ) {
          CacheIfNever ( itemNum + 1, itemPtr, &r);
          if (itemNum > firstItem)  
            leadHeight = itemPtr->leadHeight;
        	else  
            leadHeight = 0;
      		if (fstLine==0) rct.top += itemPtr->spaceBefore;
          
          lineHeight = itemPtr->lineHeight;
          rct.top += leadHeight;
          numRows = MIN (itemPtr->numRows,
                         MAX ((r.bottom - rct.top) / lineHeight, 0));
          rct.bottom = rct.top + lineHeight * (numRows - fstLine);
      		rct.bottom += itemPtr->spaceAfter;
          if (itemNum == item) {
            goOn = FALSE;
            if (numRows > 0) {
            	top = rct.top;
            	bottom = rct.bottom;
            	firstline = fstLine;
              rsult = TRUE;
            	}
          	} 
          else  
            rct.top = rct.bottom;
          if (numRows < itemPtr->numRows)  
            goOn = FALSE;
        	}
        itemNum++;
        fstLine = 0;
      	}
    	}
  	}
  
  return rsult;
}





Boolean DRichView::GetScrollStats( short& offset, short& firstShown, short& firstLine) 
{
  short     item, line,off;
  Boolean  	rsult;
  Nlm_BaR		sb;

  sb = Nlm_GetSlateVScrollBar((Nlm_SlatE)fPanel);
  if (sb) {
    off = Nlm_GetValue (sb);
    item = GetItemNum ( off);
    line = off - GetLineStart(item);
    rsult = TRUE;
  	}
  else {
	  off = 0;
	  item = 0;
	  line = 0;  
  	rsult = FALSE;
  	}
  offset = off;
  firstShown = item + 1;
  firstLine = line;
  return rsult;
}






void  DRichView::Update( short from, short to) 
{
			// Invalidates and updates items in a document
  short     	bottom,item,start,stop,swap,top, f1;
  DParagraph*  itemPtr;
  Nlm_RecT     r;
  Nlm_WindoW   tempPort;

  if (from >= 0 && to >= 0) {
    if (from == 0 || from > fNumItems) {
      start = 0;
      from = 0;
    	} 
    else  
      start = from - 1;
     
    if (to == 0 || to > fNumItems) 
      stop = to = fNumItems;
    else  
      stop = to;
    
    for (item = start; item < stop; item++) {
      itemPtr = GetParaPtr(item);
      if (itemPtr) {
        itemPtr->fText = (char*) MemFree (itemPtr->fText);
        itemPtr->notCached = TRUE;
        itemPtr->neverCached = TRUE;
      	}
    	}
    if (Nlm_Enabled(fPanel) && Nlm_AllParentsEnabled(fPanel) &&
        Nlm_Visible(fPanel) && Nlm_AllParentsVisible(fPanel)) {
      tempPort = Nlm_SavePort(fPanel);
			ViewRect( r);
      Nlm_Select(fPanel);
      if (from > to && from != 0 && to != fNumItems) {
        swap = from;
        from = to;
        to = swap;
      	}
      if (from > 0 && from <= fNumItems && IsVisible( from, top, bottom, f1))  
        r.top = top;
      if (to > 0 && to < fNumItems && IsVisible( to, top, bottom, f1))  
        r.bottom = bottom;
      Nlm_InsetRect(&r, -1, -1);
      Nlm_InvalRect(&r);
      if (tempPort) Nlm_RestorePort(tempPort);
    	}
    AdjustScroll();
  	}
}



void DRichView::SaveTableItem(DParagraph* itemPtr, FILE *f, Boolean tabStops)
{
				//    Reformats and saves an item to a file
  char     	ch;
  short     i;
  short     next,num,pos,row;
  char	* 	ptr;
  char	*		text;

  next= 0;
  if (itemPtr && f) {
    text = itemPtr->fText;
    if (text && *text != '\0') {
      pos = 0;
      row = 0;
      ptr = text;
      ch = *ptr;
      while (ch != '\0') {
        if (ch == '\n') {
          row++;
          ptr++;
          pos = 0;
          fputc ('\n', f);
        	} 
        else  {
          num = 0;
          while (ch != '\0' && ch != '\n') {
            num++;
            ch = ptr [num];
          	}
          if (num > 0) {
            while (num > 0 && *ptr == '\t') {
              num--;
              ptr++;
              next += fTabCount;
            	}
            while (pos < next) {
              fputc (' ', f);
              pos++;
            	}
            for (i = 0; i < num; i++) {
              fputc (ptr [i], f);
              pos++;
            	}
          	}
          ptr += num;
        	}
        ch = *ptr;
      }
    }
  }
}




void DRichView::Save(DFile* f) 
{ 
  if ( f ) {
    Boolean savevirt= fIsVirtual;
    fIsVirtual = FALSE;
    for (short i = 0; i < fNumItems; i++) {
      DParagraph* itemPtr = GetParaPtr( i);
      if (itemPtr) {
				DParagraph itemData( itemPtr->fDataPtr, false, itemPtr->numRows, itemPtr, 
										itemPtr->fStyleIndex, itemPtr->fNumStyles, false);
       	FormatText(0, &itemData, FALSE);
				if (i > 0 && itemData.openSpace > 0) fputc ('\n', f->fFile);
       	SaveTableItem(&itemData, f->fFile,  TRUE);
        }
      }
		fIsVirtual= savevirt;
    }
}





void DRichView::Print()	
{ 
#if (defined(WIN_MAC) || defined (WIN_MSWIN))
  Boolean   goOn;
  short      i, item, line,pixels, visBelow;
  DParagraph*   itemPtr;
  Boolean   newPage;
  Nlm_RecT      r, rct;
  FloatHi   		scale;
  Nlm_WindoW    w;

  Boolean savevirt= fIsVirtual;
  fIsVirtual = FALSE;
	if (fNumItems > 0) {
		ViewRect( rct);
    w = Nlm_StartPrinting ();
    if (w != NULL) {
      for (i = 0; i < MAXFONTS; i++) {
        gFontHeights[i].font = NULL;
        gFontHeights[i].height = 0;
      	}
      goOn = TRUE;
      pixels = 0;
      item = 0;
      newPage = TRUE;
      while (item < fNumItems && goOn) {
        if (newPage) {
          goOn = Nlm_StartPage ();
          newPage = FALSE;
          Nlm_PrintingRect (&r);
       	 }
        if (goOn) {
          itemPtr = GetParaPtr( item);
          if (itemPtr) {
						DParagraph itemData( itemPtr->fDataPtr, false, itemPtr->numRows, itemPtr, 
																itemPtr->fStyleIndex, itemPtr->fNumStyles, false);
            UpdateItemHeights (&itemData);
            scale = (FloatHi) (r.right - r.left) / (FloatHi) (rct.right - rct.left);
		       	FormatText(0, &itemData, TRUE);

            if (pixels != 0)  r.top += itemData.leadHeight;
        		r.top += itemData.spaceBefore;
             
            visBelow = (r.bottom - r.top) / itemData.lineHeight;
            if ((itemData.keepTogether && visBelow < itemData.numRows) ||
                itemData.newPage) {
              goOn = Nlm_EndPage ();
              if (goOn)  goOn = Nlm_StartPage ();
              newPage = FALSE;
              Nlm_PrintingRect (&r);
              visBelow = (r.bottom - r.top) / itemData.lineHeight;
            	}
            line = 0;
            while (visBelow + line < itemData.numRows && goOn) {
          		pixels = DrawStyledItem( &itemData, &r, item+1, line, TRUE, NULL);
              r.top += pixels;
        			r.top += itemData.spaceAfter;
              line += visBelow;
              goOn = Nlm_EndPage ();
              if (goOn) Nlm_StartPage ();
              newPage = FALSE;
              Nlm_PrintingRect (&r);
              visBelow = (r.bottom - r.top) / itemData.lineHeight;
            	}
            if (visBelow > 0 && goOn) {
          		pixels = DrawStyledItem( &itemData, &r, item+1, line, TRUE, NULL);
              r.top += pixels;
        			r.top += itemData.spaceAfter;
            	}
          	 
          	}
        	}
        item++;
        if (r.top >= r.bottom) {
          goOn = Nlm_EndPage ();
          newPage = TRUE;
        	}
      	}
      if ((!newPage) && goOn) 
        goOn = Nlm_EndPage ();
       
      Nlm_EndPrinting (w);
      for (i = 0; i < MAXFONTS; i++) {
        gFontHeights [i].font = NULL;
        gFontHeights [i].height = 0;
        }
    	}
		}
	fIsVirtual= savevirt;
   
#else
	Nlm_Message(MSG_OK,"Printing is not yet defined for this window system");
#endif
}



void  DRichView::SetTabs( short theTabstops) 
{ 
	fTabCount= theTabstops;
}


#if 0
enum docFormats {
	kUnknownformat,
	kTextformat,
	kRTFformat,
	kPICTformat,
	kHTMLformat,
	kMaxDocformat
	};
#endif

void  DRichView::ShowFile( char* filename, Nlm_FonT font) 
{
	fDocFormat= DRichHandler::kUnknownformat;
	DRTFHandler rtfhandler( this, NULL);
	if ( rtfhandler.ProcessFile( filename) ) 
		fDocFormat= rtfhandler.Format();
	else {
		DPICTHandler picthandler( this, NULL);
		if ( picthandler.ProcessFile( filename) )
			fDocFormat= picthandler.Format();
		else  { 
		  DGIFHandler gifhandler( this, NULL);
		  if ( gifhandler.ProcessFile( filename) ) 
		  	fDocFormat= gifhandler.Format(); 
		  else {
			  DHTMLHandler htmlhandler( this, NULL);
			  if ( htmlhandler.ProcessFile( filename) ) 
			  	fDocFormat= htmlhandler.Format(); 
			  else {
#if 1
				  DRichHandler texthandler( this, NULL);
				  if ( texthandler.ProcessFile( filename) ) 
				  	fDocFormat= texthandler.Format(); 
#else
			    ShowTextFile( filename, font);
			  	fDocFormat= DRichHandler::kTextformat; 
#endif
			  	}
		  	}
		  }
		}
}

void  DRichView::ShowDoc( char* doctext, ulong doclength) 
{
	if (!doctext) return;
	if (!doclength) doclength= StrLen( doctext);
	char*	docend= doctext + doclength;
	
	if (fDocFormat) switch (fDocFormat) {
	
		case DRichHandler::kTextformat: {
			DRichHandler texthandler( this, NULL);
			if ( texthandler.ProcessData( doctext, docend, true, doclength) ) return;
			}
			break;

		case DPICTHandler::kPICTformat: {
			DPICTHandler picthandler( this, NULL);
			if ( picthandler.ProcessData( doctext, docend, true, doclength) ) return;
			}
			break;
			
		case DGIFHandler::kGIFformat: {
		  DGIFHandler gifhandler( this, NULL);
			if ( gifhandler.ProcessData( doctext, docend, true, doclength) ) return;
			}
			break;

		case DHTMLHandler::kHTMLformat: {
			DHTMLHandler htmlhandler( this, NULL);
			if ( htmlhandler.ProcessData( doctext, docend, true, doclength) ) return;
			}
			break;

		case DRTFHandler::kRTFformat: { 
			DRTFHandler rtfhandler( this, NULL);
			if ( rtfhandler.ProcessData( doctext, docend, true, doclength) ) return;
			}
			break;
		}
		
	fDocFormat= DRichHandler::kUnknownformat;
	
	DRTFHandler rtfhandler( this, NULL);
	if ( rtfhandler.ProcessData( doctext, docend, true, doclength) ) 
		fDocFormat= rtfhandler.Format();
	else {
		DPICTHandler picthandler( this, NULL);
		if ( picthandler.ProcessData( doctext, docend, true, doclength) )
			fDocFormat= picthandler.Format();
		else  { 
		  DGIFHandler gifhandler( this, NULL);
		  if ( gifhandler.ProcessData( doctext, docend, true, doclength) ) 
		  	fDocFormat= gifhandler.Format(); 
		  else {
			  DHTMLHandler htmlhandler( this, NULL);
			  if ( htmlhandler.ProcessData( doctext, docend, true, doclength) ) 
			  	fDocFormat= htmlhandler.Format(); 
			  else {
				  DRichHandler texthandler( this, NULL);
				  if ( texthandler.ProcessData( doctext, docend, true, doclength) ) 
				  	fDocFormat= texthandler.Format(); 
			  	}
		  	}
		  }
		}
}



void DRichView::ShowTextFile( char* filename, Nlm_FonT font)
{	
	DParagraph	para;
	DRichStyle	aStyle;
	if (font) aStyle.font= font;
	aStyle.last= true;
  ShowFileFancy( filename, &para, &aStyle, 1);
}





void  DRichView::ShowFileFancy( char* filename, DParagraph* parFormat,  
																DRichStyle* styles, short nstyles, short tabStops) 
{
  char     	ch;
  short   	actual, cnt, leftOver;
  long     	cntr;
  FILE     	*fp;
  Nlm_RecT     r;
  Nlm_WindoW   tempPort;
  char  	* text,  * txt;
  Nlm_FonT		 font;
#ifdef COMP_MPW
  char*  p;
#endif
#if (defined(OS_DOS) || defined (OS_NT))
  char*  p;
  char*  q;
#endif
	DParagraph	para;

  if ( filename  && filename [0] != '\0') {
 		fTabCount = tabStops;
    if (parFormat == NULL) parFormat = &para;
    Nlm_Reset (fPanel);
    tempPort = Nlm_SavePort (fPanel);
		ViewRect( r);
    if (styles) font= styles->font;
    if (font == NULL) font = Nlm_programFont; //Nlm_systemFont;
     
    text = (char*) MemNew (16000);
    if (text) {
      fp = FileOpen (filename, "r");
      if (fp) {
        leftOver = 0;
        cntr = FileLength (filename);
        cnt = (short) MIN (cntr, 15000L);

        while (cnt > 0 && cntr > 0) {
          txt = text + leftOver;
          actual = (short) FileRead (txt, 1, cnt, fp);
          if (actual > 0) {
            cnt = actual;
            txt [cnt] = '\0';
            ch = fgetc (fp);
            while (ch != '\0' && ch != '\n' && cnt < 15900) {
              txt [cnt] = ch;
              cnt++;
              ch = fgetc (fp);
            	}
            while ((ch == '\n' || ch == '\r') && cnt < 15900) {
              txt [cnt] = ch;
              cnt++;
              ch = fgetc (fp);
            	}
            txt [cnt] = '\0';
#if (defined(OS_DOS) || defined (OS_NT))
            p = text;
            q = text;
            while (*p) {
              if (*p == '\r') {
                p++;
              } else {
                *q = *p;
                p++;
                q++;
              }
            }
            *q = '\0';
#endif
#ifdef COMP_MPW
            p = text;
            while (*p) {
              if (*p == '\r') {
                *p = '\n';
              }
              p++;
            }
#endif
            Append( text, parFormat, styles, nstyles, NULL); 
            	/* dgg -- need to fiddle w/ styles to match to subset of text !! */
            leftOver = 1;
            text [0] = ch;
            cntr -= cnt;
            cnt = (short) MIN (cntr, 15000L);
          	} 
          else {
            cnt = 0;
            cntr = 0;
          	}
        	}
        FileClose (fp);
      	}
      text = (char*) MemFree (text);
    	}
    if (!fAutoAdjust) AdjustScroll();
    if (tempPort) Nlm_RestorePort (tempPort);
  	}
}



		
	
	
void DRichView::DoubleClick(Nlm_PoinT mouse)
{
	short			 	item, row, atch;
	Nlm_RecT		rct;
	char				wordAt[512];
	DRichStyle 	* styleAt = NULL;

		// sickly demo
	wordAt[0]= 0;
	MapPoint( mouse, item, row, &rct, wordAt, 512, NULL/*styleAt*/);
	atch= gMapOfs;  
	SetSelectedText( item, atch, item, atch); 
	if (fMapStyle) {
		if (fMapStyle->ispict) {
			Nlm_PoinT atp;
			atp.x= mouse.x - fDrawr.left;
			atp.y= mouse.y - fDrawr.top;
			// if fMapStyle->ismap, do things w/ gopher link
#if 0
			Message(MSG_OK, "dblClik in picture x=%d y=%d, picwd=%d picht=%d", 
					atp.x, atp.y, fMapStyle->pixwidth, fMapStyle->pixheight);
#endif
			}
		else {
			Nlm_InvertRect(&rct);
#if 0
			Message(MSG_OK, "dblClik text at prg=%d row=%d sto=%d '%s'",
							 item, row, fMapStyle->nextofs, wordAt);
#endif
			InvalRect( rct);
			}
		}
	//this->Select();
	this->GetWindow()->Select();
}

void DRichView::SingleClick(Nlm_PoinT mouse) 
{
	short 			item, row, atch;
	Nlm_RecT		rct;
	char				wordAt[512];

	wordAt[0]= 0;
	MapPoint( mouse, item, row, &rct, wordAt, 512, NULL/*styleAt*/);
	atch= gMapOfs;  
		SetSelectedText( item, atch, item, atch); 
}

void DRichView::Click(Nlm_PoinT mouse)
{
	ClearCurrentStyle(); // Vibrant click calls change FONT !!
	if (Nlm_dblClick) DoubleClick( mouse);
	else SingleClick(mouse);		
}
	
void  DRichView::SetAutoAdjust( Boolean autoAdjust) 
{
	fAutoAdjust= autoAdjust;
}


#if 0
void DRichView::Show()
{
  Nlm_Update(); //!? need since took out vibwnds autoupdate for RestorePort
}
#endif

// this is DPanel form:
//void DRichView::Scroll( Boolean vertical, DView* scrollee, short newval, short oldval)

void DRichView::ScrollDoc(Nlm_BaR sb, short newval, short oldval)
{
		// Scroll bar callback that takes suppressed partial lines into account
		// this is good only for vertical==true
  Boolean  	goToEnd;
  short     barmax, barval, firstShown;
  short     height,highest,highFree,item, lo1, hi1;
  short     line, lineInto,lowest,lowFree;
  short     pixels,pgDn,pgUp,vis;
  Nlm_RecT 	r;
  DParagraph*  itemPtr;
	
	// Nlm_SlatE s= (Nlm_SlatE) fPanel;
	
  if (oldval != newval) {
    if (Nlm_Visible (fPanel) && Nlm_AllParentsVisible (fPanel)) {
			ViewRect( r);
      height = r.bottom - r.top;
      item = GetItemNum ( newval);
      line = newval - GetLineStart( item);
      firstShown = item;
      lineInto = line;
      lowest = INT2_MAX;
      highest = INT2_MIN;
      pgUp = VisLinesAbove ( &r, item, line, lowest, highest) - 1;
      pgDn = VisLinesBelow ( &r, item, line, lowest, highest) - 1;
      if (pgDn < 1)  pgDn = 1;
      if (pgUp < 1)  pgUp = 1;
      if (lowest < INT2_MAX)  
        fNumLines = UpdateLineStarts ( lowest);
      Nlm_Select (fPanel);
      if ((newval > oldval && newval - pgUp <= oldval) ||
          (newval < oldval && newval + pgDn >= oldval)) {
        pixels = PixelsBetween ( &r, newval, oldval, lo1, hi1);
        if (ABS (pixels) < height) {
          Nlm_ScrollRect (&r, 0, pixels);
          if (pixels<0) r.top = r.bottom + pixels; // - 5;
          else r.bottom = r.top + pixels; // + 5;
          Nlm_InsetRect (&r, -1, -1);
          Nlm_InvalRect (&r);
        	} 
        else {
          Nlm_InsetRect (&r, -1, -1);
          Nlm_InvalRect (&r);
        	}
      	} 
      else {
        Nlm_InsetRect (&r, -1, -1);
        Nlm_InvalRect (&r);
      	}
      lowFree = INT2_MAX;
      highFree = INT2_MIN;
      for (item = 0; item < lowest; item++)  
        FreeCachedItem ( item, lowFree, highFree);
      for (item = highest + 1; item < fNumItems; item++) 
        FreeCachedItem ( item, lowFree, highFree);
      if (lowFree < INT2_MAX) 
        fNumLines = UpdateLineStarts ( lowFree);
      
      barmax = 0;
      goToEnd = (Boolean) (newval == fBarmax);
      if (fNumLines > 0 && fNumItems > 0) {
        itemPtr = GetParaPtr( fNumItems - 1);
        if (itemPtr) {
					ViewRect( r);
          lowest = INT2_MAX;
          highest = INT2_MIN;
          vis = VisLinesAbove ( &r, fNumItems - 1,
                               itemPtr->numRows - 1, lowest, highest);
          if (lowest < INT2_MAX)  
            fNumLines = UpdateLineStarts ( lowest);
          barmax = fNumLines - vis;
        	}
      	}
      barval = GetLineStart( firstShown) + lineInto;
      if (goToEnd) barval = barmax;
    
      if (barval > fBarmax) {
        Nlm_CorrectBarMax (sb, barmax);
        Nlm_CorrectBarValue (sb, barval);
      	} 
      else {
        Nlm_CorrectBarValue (sb, barval);
        Nlm_CorrectBarMax (sb, barmax);
      	}
      Nlm_CorrectBarPage (sb, pgUp, pgDn);
      fBarmax = barmax;
      Nlm_Update ();
    }
  }
}



// Performs garbage collection on document data

void DRichView::ResetDoc()
{
	DeleteAll();
  fNumItems = 0;
  fNumLines = 0;
  fBarmax = 0;
  //DRichView::InitRichView();
}









// class DRichFindDialog

class DRichFindDialog : public DFindDialog {
public:
	DRichView* fView;
	char	* fLastTarget;
	short		fLastParag, fLastChar;
	DRichFindDialog(  DRichView* itsView);
	virtual void DoFind();
	virtual void BuildDlog();
};

DRichFindDialog::DRichFindDialog(  DRichView* itsView) :
		fView( itsView), fLastTarget(NULL),  fLastParag(-1), fLastChar(-1)
{
}


void DRichFindDialog::BuildDlog()
{
	DFindDialog::BuildDlog();

	DView* av;
	av= FindSubview(replaceId); if (av) { av->Disable(); } //av->Hide();
	av= FindSubview(replaceFindId); if (av) { av->Disable();  }
	av= FindSubview(replaceAllId); if (av) { av->Disable(); }
	av= FindSubview(cBackwards); 	if (av) { av->Disable(); }
	av= FindSubview(cFullWord); if (av) { av->Disable(); }
	//av= FindSubview(cCaseSense); if (av) { av->Disable(); }
	if (fReplaceText) fReplaceText->Disable();
}

void DRichFindDialog::DoFind() 
{
	const char	*target;
	Boolean 	casesense, back, fullword, found;
	short			paragat, charat;
	long			len;
	
	target= this->GetFind();
	if (target && *target) {
		casesense	= this->CaseSensitive();
		back= this->Backwards(); 		// not yet supported
		fullword= this->FullWord(); // not yet supported
		paragat= fView->fSelParag - 1; // offset by -1 for real parag# -- MESS !
		charat= fView->fSelChar;
		len= StrLen(target);
		if (fLastTarget && StringCmp(target, fLastTarget)==0 
		 && paragat == fLastParag && charat == fLastChar) {
			charat += len;
			found= fView->Find( (char*)target, paragat, charat, paragat, charat, casesense); 
			}
		else {
			if (fLastTarget) MemFree(fLastTarget);
			fLastTarget= StrDup(target);
			found= fView->Find( (char*)target, paragat, charat, paragat, charat, casesense); 
			}
		fLastParag= paragat;
		fLastChar = charat;
		if (found) {
			fView->SetSelectedText( paragat+1, charat, paragat+1, charat + len);
			}
		}
}				



//class	DRichTextDoc : public DWindow



Nlm_RecT DRichTextDoc::gRichDocRect = { 0, 0, 0, 0 };


DRichTextDoc::DRichTextDoc( long id, Boolean makeRichView, Nlm_FonT itsFont) :
		DWindow( id, gApplication, document),
		fRichView(NULL), fFloatingEditText(NULL)
{ 	
	Nlm_PoinT nps;
	fFont= itsFont;
	this->GetNextPosition( &nps);
	fFloatingEditText= (DDialogText*) new DHiddenText(0,this,"");
	this->SetNextPosition( nps);
	fFloatingEditText->Hide();
	//this->SetEditText(fFloatingEditText);
	
	if (makeRichView) {
		this->GetNextPosition( &nps);
		nps.x= 0; // fix for odd offset
		this->SetNextPosition( nps);
		fRichView= new DRichView( 0, this, gPrintManager->PageWidth(), 0);
		fRichView->SetSlateBorder( true);
		fRichView->SetResize( DView::matchsuper, DView::relsuper);
		//fRichView->SetTabs( gTextTabStops);
		//fRichView->SizeToSuperview( fWindow, true, false); // this needs to adjust for if view has scrollbar !
		//fSaveHandler= fRichView; //DRichView() installs these handlers !
		//fPrintHandler= fRichView;
		}
}

                              
void DRichTextDoc::Open()
{
	this->CalcWindowSize();
	if (!fFindDlog) fFindDlog= new DRichFindDialog( fRichView);
	fRichView->SizeToSuperview( this, true, false); // this needs to adjust for if view has scrollbar !
	DWindow::Open();
}

void DRichTextDoc::OpenText( char* doctext, ulong doclength)
{
	if (doctext) {
		fRichView->ShowDoc( doctext, doclength);
		Open();
		}
}


void DRichTextDoc::Open( DFile* aFile)
{
	if (aFile) {
		SetTitle( (char*)aFile->GetShortname());
		SetFilename( (char*)aFile->GetName());
		fRichView->ShowFile( (char*)aFile->GetName(), fFont);
		}
	Open();
}

void  DRichTextDoc::Open( char* filename) 
{
	SetTitle( (char*)DFileManager::FilenameFromPath(filename));
	SetFilename( (char*) filename);
	fRichView->ShowFile( filename, fFont);
  Open();
}

void DRichTextDoc::Close()
{
	//if (!Nlm_EmptyRect(&fViewrect)) gRichDocRect= fViewrect;  
	if (fRichView) fRichView->ViewRect( gRichDocRect);  // GetPosition
	DWindow::Close();
}

void DRichTextDoc::ResizeWin()
{
	DWindow::ResizeWin();
	if (fRichView) fRichView->ViewRect( gRichDocRect);
	//if (!Nlm_EmptyRect(&fViewrect)) gRichDocRect= fViewrect;  
}

Boolean DRichTextDoc::IsMyAction(DTaskMaster* action) 
{
	switch(action->Id()) {
		case DApplication::kUndo:
			Message(MSG_OK,"DRichTextDoc::Undo not ready.");
			return true;
		case DApplication::kCut:
			Message(MSG_OK,"DRichTextDoc::Cut not ready.");
			//this->CutText(); 
			return true;
			
		case DApplication::kCopy: {
			char* theText= NULL;
			if (fRichView) theText= fRichView->GetSelectedText();
			if (theText) {
				fFloatingEditText->SetText(theText);
				fFloatingEditText->SetSelection(0,32000); 
	      fFloatingEditText->CopyText(); 
	      MemFree( theText);
				fFloatingEditText->SetText(""); // don't need to keep extra copies around
				//Message(MSG_OK,"DRichTextDoc::Copied text selection.");
	      }
	    }
			return true;
			
		case DApplication::kPaste:
			Message(MSG_OK,"DRichTextDoc::Paste not ready.");
			//this->PasteText(); 
			return true;
		case DApplication::kClear:
			Message(MSG_OK,"DRichTextDoc::Clear not ready.");
			//this->ClearText(); 
			return true;
		case DApplication::kSelectAll:
			//Message(MSG_OK,"DRichTextDoc::Select all not ready.");
			if (fRichView) fRichView->SelectAll();
			//this->SetSelection(0,0,32000,32000); 
			return true;
		default:
			return DWindow::IsMyAction(action);
		}
}


// class DRichApplication : public DApplication

 
DRichApplication::DRichApplication()
{
	//fAcceptableFileTypes= "TEXTPICTGIFfRTF "; /* up to 4 types, or null for all types */
	// ^^ not working, only 1st seen !?
	//fAcceptableFileTypes= NULL;
}


void DRichApplication::OpenDocument(DFile* aFile)
{
	if (aFile && aFile->Exists()) {
		gCursor->watch();
		DRichTextDoc* aDoc= new DRichTextDoc();
		aDoc->Open( aFile);
		gCursor->arrow();
		}
}


