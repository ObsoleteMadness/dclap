// DRichStyle.cpp
// d.g.gilbert


#include "Dvibrant.h"
#include "DWindow.h"
#include "DPanel.h"
#include "DControl.h"
#include "DFile.h"
#include "DList.h"
#include "DUtil.h"
#include "DDialogText.h"

#include "DDrawPICT.h"

#include "DRichStyle.h"
#include "DRichMoreStyle.h"
#include "DRichViewNu.h"

#include "DIconLib.h"
#include "DIcons.h"


DDrawPict* gDrawPict = NULL; 


extern "C" void Nlm_DoDraw (Nlm_GraphiC a);



// struct DRichStyle

DRichStyle::DRichStyle()
{
  //SetClass("DRichStyle",sizeof(DRichStyle));
	ispict= false;
	ismap= false;
	ownpict= false;
	fObject= NULL; 

	boxed= false;
	hidden= false;
	last= false; //??
	color= 0; // black ??
	nextofs= 0;
	pixheight= pixwidth= pixleft= 0;
	linkid= 0;
	font= NULL;
	superSub= 0;
	underline= kNoUnderline;
	bold= false;
	italic= false;
	outline= false;
	shadow= false;
	strikeout= false;
	smallcaps= false;
	allcaps= false;
}

DRichStyle::~DRichStyle()
{
	// this was killing things due to large # of static variables of this type
	// commonly used as save temps -- was deleting same data more than once !!
#if 1
	if (ownpict && fObject) {
		fObject->suicide(); 
		// if (ownpict) ownpict= true; //  debug trap 
		}
#else
	//if (ownpict) MemFree(picdata);  
#endif
}

Boolean DRichStyle::suicide(void) 
{ 
	if (GetOwnerCount() <= 1) { 
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide();
}

Boolean DRichStyle::suicide(short ownercount)
{
	if (ownercount < 1) {
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide(ownercount);
}

#if 0
// this bombs......
DRichStyle& DRichStyle::operator=( DRichStyle& ast)
{
	if (this != &ast) {
		//*this ::= ast; 
		//this->DObject::operator=(ast);
	  MemCpy( this, &ast, sizeof(DRichStyle)); //<<! subclass bad !!
		this->ownpict= false;
		}
	return *this;
}
#endif

#if 0
DObject* DRichStyle::Clone()
{
	DRichStyle* result = (DRichStyle*) DObject::Clone();
	if (picdata) {
		 result->picdata= (char*) MemDup( picdata,  picsize);
		 result->ownpict= true;
		 }
	return result;
}
#endif

Boolean DRichStyle::IsNotEqual(DRichStyle* other)
{
	if ( 0 == MemCmp( this, other, sizeof(DRichStyle)) )
		return false;
	else if (ispict && other->ispict) {
		// ?? need some better way to test if two picts are the same ??
		return ( linkid != other->linkid
					|| ismap != other->ismap
					|| (fObject && fObject->IsNotEqual(other->fObject))
					);
		}
	else
	  return (  linkid != other->linkid 
					|| ispict != other->ispict
					|| ismap != other->ismap
					|| color != other->color 
					|| superSub != other->superSub 
					|| underline != other->underline
					|| bold != other->bold
					|| italic != other->italic
					|| hidden != other->hidden
					|| outline != other->outline
					|| shadow != other->shadow
					|| strikeout != other->strikeout
					|| smallcaps != other->smallcaps
					|| allcaps != other->allcaps
					|| boxed != other->boxed
					);
}







// class DStyleObject 

DStyleObject::DStyleObject()
{
	fKind= kDefaultObject;  // kPictMac
	fSize= 0;
	fData= NULL;
}

DStyleObject::DStyleObject( short kind, void* data, ulong dsize, Boolean owndata)
{
	fKind= kind;
	fSize= 0;
	fData= NULL;
	SetData( kind, data, dsize, owndata);
}

DStyleObject::~DStyleObject()
{
	MemFree( fData);
}

Boolean DStyleObject::suicide(void) 
{ 
	if (GetOwnerCount() <= 1) { 
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide();
}

Boolean DStyleObject::suicide(short ownercount)
{
	if (ownercount < 1) {
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide(ownercount);
}

Boolean DStyleObject::IsNotEqual(DStyleObject* other)
{
	if ( !other 
		|| fSize != other->fSize 
		|| fKind != other->fKind   
		|| (fData == NULL) != (other->fData == NULL)
		)  return true;
	if (fData && other->fData)  
		return (0 != MemCmp( fData, other->fData, fSize));
	else
		return false; 
}

void DStyleObject::SetData( short kind, void* data, ulong dsize, Boolean owndata)
{
	if (fData) MemFree( fData); 
	fKind= kind;
	if (owndata) fData= (char*) data;   // this was bad ?!?
	else fData= (char*) MemDup(data, dsize); 
	fSize= dsize;
}


Boolean DStyleObject::CanDraw()
{	
	return (false);  
}

void DStyleObject::Draw( Nlm_RecT area)
{
	gRightTriangle.Draw( area); // need own icon??
}







// class DPictStyle

DPictStyle::DPictStyle()
{
	fKind= kPictOther;  // kPictMac
	fLinks = NULL;
	fNumLinks= 0;
}

DPictStyle::DPictStyle( short kind, void* data, ulong dsize, Boolean owndata)
{
	fKind= kind;
	fLinks = NULL;
	fNumLinks= 0;
	SetData( kind, data, dsize, owndata);
}

DPictStyle::~DPictStyle()
{
	MemFree( fLinks);
}


short DPictStyle::GetLink( Nlm_PoinT atp, short& kind)
{
	for (short i= 0; i<fNumLinks; i++) {
		if (Nlm_PtInRect( atp, &fLinks[i].fLoc)) {
			kind= fLinks[i].fKind;
			return fLinks[i].fLinkid;
			}
		}
	kind= 0;
	return 0;
}

short DPictStyle::GetLink( short index, short& kind, Nlm_RecT& loc)
{
	if (index>=0 && index<fNumLinks) {
		kind= fLinks[index].fKind;
		loc= fLinks[index].fLoc;
		return fLinks[index].fLinkid;
		}
	else {
		kind= 0;
		return 0;
		}
}

void DPictStyle::AddLink( short kind, short linkid, Nlm_RecT loc)
{
	PictLinkIndex lnk;
	lnk.fLinkid= linkid;
	lnk.fLoc= loc;
	lnk.fKind= kind;
	fNumLinks++;
	if ( fLinks )
		fLinks= (PictLinkIndex*) MemMore( fLinks, fNumLinks * sizeof(PictLinkIndex));
	else
		fLinks= (PictLinkIndex*) MemNew( fNumLinks * sizeof(PictLinkIndex));
	fLinks[fNumLinks-1]= lnk;
}

Boolean DPictStyle::HasNetPict()
{	
	return (fLinks && 
		(fKind == DPictStyle::kPictNetLink 
	|| fKind == DPictStyle::kPictNetPic));
}

Boolean DPictStyle::CanDraw()
{	
	return (fKind == kPictMac); // add more later...
}

void DPictStyle::Draw( Nlm_RecT area)
{
	if (fKind == kPictMac) {
		// !! kPictMac value differs in DPictStyle & DDrawPict classes !!
		gDrawPict->Draw( &area, fData, fSize, DDrawPict::kPictMac);
		}
	else if (fKind == kPictNetLink) gNetpicIcon.Draw( area); 
	else gPictureIcon.Draw( area);
}





// class DControlStyle

DControlStyle::DControlStyle(DView* super)
{
	fSuperview= super;
	fKind= kButton;  
	fControl= NULL;
	fDefaultText= NULL;
	fVarname= NULL;
	fId = 0;
	fWidth= fHeight= 0;
	fSelected= 0;
	fInstalled= false;
	fData2= NULL;
	fSize2= 0;
}

DControlStyle::DControlStyle(DView* super, short kind, void* data, ulong dsize, Boolean owndata)
{
	fControl= NULL;
	fDefaultText= NULL;
	fVarname= NULL;
	fWidth= fHeight= 0;
	fSelected= 0;
	fInstalled= false;
	fSuperview= super;
	fKind= kind;
	fId = fKind;
	SetData( kind, data, dsize, owndata);
}

DControlStyle::~DControlStyle()
{
	//MemFree( fDefaultText); == fData ??
}

void DControlStyle::ControlData( char* varName, char* defaultText, char* text2)
{
	ulong dlen, dlen2;
	MemFree( fVarname); 
	fVarname= (char*) StrDup(varName); 
	if (defaultText) dlen= 1 + Nlm_StringLen(defaultText);
	else dlen= 0;
	if (text2) dlen2= 1 + Nlm_StringLen(text2);
	else dlen2= 0;
	SetData( fKind, defaultText, dlen, false, text2, dlen2);
}

void DControlStyle::SetData( short kind, void* data, ulong dsize, Boolean owndata,
								void* data2, ulong dsize2)
{
	fKind= kind;
	if (!fId) fId= fKind;
	MemFree( fData); 
	MemFree( fData2); 
	if (owndata) { fData= (char*) data; fData2= (char*) data2; }
	else { 
		fData= (char*) MemDup(data, dsize);  
		fData2= (char*) MemDup(data2, dsize2); 
		} 
	fSize= dsize;
	fSize2= dsize2;
	fDefaultText= fData;
}


Boolean DControlStyle::CanDraw()
{	
	return (!fInstalled || fControl); 
}



void DControlStyle::Draw( Nlm_RecT area)
{
	//fControl->Draw();
	if (!fInstalled) {
		Install();
		fInstalled= true;
		if (fControl) {
			fControl->fKind= (long) this; // store self reference; is fKind is usable??
			fControl->SetPosition( area);
			//fControl->Show(); //<< calls Inval() and Nlm_Update(), which are callers of this !
#ifdef notWIN_MOTIF
			fControl->Select();
#endif
			Nlm_Show( (Nlm_GraphiC)fControl->fNlmObject); 
			}
		}
	else if (fControl) {
		// this is mac-only and not needed..
		//Nlm_DoDraw( (Nlm_GraphiC)fControl->fNlmObject);
		}
}

short DControlStyle::Height()
{
	if (!fHeight) getsize();
	return fHeight;
}

short DControlStyle::Width()
{
	if (!fWidth) getsize();
	return fWidth;
}

void DControlStyle::getsize()
{
	if (fControl) {
		Nlm_RecT viewr;
		fControl->ViewRect(viewr);
		fWidth= viewr.right - viewr.left;
		fHeight= viewr.bottom - viewr.top;
		}
}


void DControlStyle::Install()
{
	//fInstalled= true;
	//fControl = new DButton( fId, fSuperview, fDefaultText); //??
}


const char* DControlStyle::Varname()
{
	return fVarname;
}

const char* DControlStyle::Value()
{
	if (fDefaultText && *fDefaultText == '\0') return NULL;
	return fDefaultText;
}


void DPromptCStyle::Install()
{
	if (!fControl) 
	fControl = new DPrompt( fId, fSuperview, fDefaultText, 0, 0, gTextFont);
	//fInstalled= true;
}

void DHiddenCStyle::Install()
{
	fWidth= 1;
	fHeight= 1;
}

const char* DHiddenCStyle::Value()
{
	if (fDefaultText && *fDefaultText == '\0') return NULL;
	return fDefaultText;
}


void DEditTextCStyle::Install()
{
	if (!fControl) {
	if (!fWidth) fWidth= 15;
	fControl = new DEditText( fId, fSuperview, NULL, fWidth, gTextFont);
	fControl->SetTitle( fDefaultText);
	//fControl->SetResize( DView::relsuper, DView::fixed);
	fWidth= fHeight= 0; //getsize();

		//??
	if (fSuperview) fSuperview->GetWindow()->SetEditText( (DEditText*) fControl);
	}
}

const char* DEditTextCStyle::Value()
{
	if (fControl) {
		MemFree(fDefaultText);
		fDefaultText= fControl->GetText();
		}
	if (fDefaultText && *fDefaultText == '\0') return NULL;
	return fDefaultText;
}


void DDialogTextCStyle::Install()
{
	if (!fControl) {
	if (!fWidth) fWidth= 30;
	if (!fHeight) fHeight= 2;
	fControl = new DDialogScrollText( fId, fSuperview, 
								fWidth, fHeight, gTextFont, true); //true == wrap == no hor.scroll
	fControl->SetTitle( fDefaultText);
	//fControl->SetResize( DView::relsuper, DView::fixed);
	fWidth= fHeight= 0; //getsize();
	}
}

const char* DDialogTextCStyle::Value()
{
	if (fControl) {
		MemFree(fDefaultText);
		fDefaultText= fControl->GetText();
		}
	if (fDefaultText && *fDefaultText == '\0') return NULL;
	return fDefaultText;
}


void DButtonCStyle::Install()
{
	if (!fControl) 
		fControl = new DButton( fId, fSuperview, fDefaultText);
}

void DDefaultButtonCStyle::Install()
{
	if (!fControl) {
		fId= cOKAY; // !? this is the Okay button equivalent !?
		fControl = new DDefaultButton( fId, fSuperview, fDefaultText);
		}
}

void DCheckboxCStyle::Install()
{
	if (!fControl) {
	fControl = new DCheckBox( fId, fSuperview, " "); //fDefaultText == value
	fControl->SetStatus( fSelected);
	}
}

const char* DCheckboxCStyle::Value()
{
	if (fControl && fControl->GetStatus()) return fDefaultText;
	else return NULL;
}


void DRadioCStyle::Install()
{
	if (!fControl) {
	fControl = new DRadioButton( fId, fSuperview, " "); //fDefaultText == value
	fControl->SetStatus( fSelected);
	}
}

const char* DRadioCStyle::Value()
{
	if (fControl && fControl->GetStatus()) return fDefaultText;
	else return NULL;
}


void DFileCStyle::Install()
{
	if (!fControl) {
	DView* super= fSuperview;
	fControl = new DButton( fId, fSuperview, "Choose...");  
	if (fSuperview) fSuperview->NextSubviewToRight();	
	//DCluster* cl= new DCluster( 0, fSuperview, 0, 0, false, "");
	//super= cl;
	char* defname= fDefaultText; //  "default file";
	fNamebox = new DPrompt( 0, super, defname, 350, 20, gTextFont);
	}
}

void DFileCStyle::Draw( Nlm_RecT area)
{
	Boolean notinst= !fInstalled;
	//area.right -= 150;  //??
	DControlStyle::Draw(area);	
	if (notinst && fControl) {
		area.left += 100; area.right += 450; // button width !?!?
		fNamebox->SetPosition( area);
		Nlm_Show( (Nlm_GraphiC)fNamebox->fNlmObject); 
		}
}

const char* DFileCStyle::Value()
{
	if (fNamebox) {
		MemFree(fDefaultText);
		fDefaultText= fNamebox->GetText();
		}
	if (fDefaultText && *fDefaultText == '\0') return NULL;
	return fDefaultText;
}


void DPopupCStyle::Install()
{
	if (!fControl) {
	char *cp, *ep, *cp2, *ep2;
	Boolean done;
	DPopupList* aPopup = new DPopupList( fId, fSuperview, true);

	char *words= StrDup( fDefaultText);
	char *words2= StrDup( fData2);
	
	// ?? replace this w/ StrTok( words, "\t"); 
	for (cp= words, cp2= words2, done=false; !done && *cp!=0; ) {
		ep= StrChr( cp, '\t');
		if (!ep) {
			ep=StrChr( cp, '\0');
			done= true;
			}
			
		if (cp2) {
			ep2= StrChr(cp2, '\t');
			if (!ep2) ep2= StrChr( cp2, '\0');
			if (ep2) *ep2= 0;
			}
		else ep2= NULL;
		
	  if (ep) {
			*ep= 0;	
			aPopup->AddItem( cp, cp2);
			if (!done) { 
				cp= ep+1; 
				if (ep2) cp2= ep2+1; else cp2= NULL; 
				}
			}
		else
			done= true;
		}
	MemFree( words);
	MemFree( words2);
	
	if (!fSelected) fSelected= 1;
	aPopup->SetValue(fSelected); 
	fControl= aPopup;
	}
}

const char* DPopupCStyle::Value()
{
	static char name[128];
	if (fControl) { 
	  short item;
		*name= 0;
		((DPopupList*)fControl)->GetSelectedValue(item, name, sizeof(name));
		if (!*name)
			((DPopupList*)fControl)->GetSelectedItem(item, name, sizeof(name));
		return name;
 		}
	else return NULL;
}

			
void DPasswordCStyle::Install()
{
	if (!fControl) {
	if (!fWidth) fWidth= 15;
  fControl = new DPasswordText( fId, fSuperview, NULL, fWidth);
	fControl->SetTitle( fDefaultText);
	//fControl->SetResize( DView::relsuper, DView::fixed);
	fWidth= fHeight= 0; //getsize();
	}
}

const char* DPasswordCStyle::Value()
{
	if (fControl) {
		MemFree(fDefaultText);
		fDefaultText= fControl->GetText();
		}
	if (fDefaultText && *fDefaultText == '\0') return NULL;
	return fDefaultText;
}





// struct DParagraph

void DParagraph::Initialize()
{
  //SetClass("DParagraph", sizeof(DParagraph));  
	just = 'l';
 	tablead = ' '; 				 
	deftabstop = 36;
  numstops = 0;
	tabstops = NULL;
	tabkinds = NULL;
	firstInset= leftInset= rightInset= 0;
	spaceBefore= spaceAfter= spaceBetween = 0;
	minLines = minHeight = 0;
	startsAt = numRows = 0;	 
	lineHeight = leadHeight = 0; 
  
  fDataPtr = NULL;	 
	fText = NULL;			 
	fStyleIndex = NULL;
	fNumStyles = 0;

	borderstyle	= kNoBorder;
	bordertop  = false;
	borderbottom = false;
	borderleft  = false;
	borderright = false;
	borderbetween = false;
	borderbar = false;
	borderbox = false;
 	
	openSpace = FALSE; // was TRUE;
	keepWithNext = FALSE;
	keepTogether = FALSE;
	newPage = FALSE;
	minLines = 0;
	minHeight = 0;
	just = 'l';
	leftInset = 0;
	wrap =  TRUE;

	notCached = TRUE;
	neverCached = TRUE;
	fDocOwnsData= false;
	fDocOwnsStyles= true;
}


DParagraph::~DParagraph()
{
#if 1
	if (fDocOwnsData) MemFree( fDataPtr);
	if (fDocOwnsStyles) MemFree( fStyleIndex);
	MemFree( fText);
	MemFree( tabkinds);
	MemFree( tabstops);
#endif
}

Boolean DParagraph::suicide(void) 
{ 
	if (GetOwnerCount() <= 1) { 
		delete this; 
		return true; 
		}
	else
		return DObject::suicide();
}

Boolean DParagraph::suicide(short ownercount)
{
	if (ownercount < 1) {
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide(ownercount);
}

#if 0
DObject* DParagraph::Clone()
{
	DParagraph* result = (DParagraph*) DObject::Clone();
	
	if (fDocOwnsStyles) {
		result->fStyleIndex= (StyleIndex*) MemDup( fStyleIndex, fNumStyles*sizeof(StyleIndex));
		}
	result->fText= (char*) StrDup( fText);
	if (fDocOwnsData) result->fDataPtr= (char*) StrDup( fDataPtr);
	result->tabkinds=  (char*) MemDup( tabkinds, numstops*sizeof(char) );
	result->tabstops=  (short*) MemDup( tabstops, numstops*sizeof(short) );
	return result;
}
#endif


#if 0
DParagraph& DParagraph::operator=( const DParagraph &val)
{
	this = (DParagraph*) val.Clone();
	return *this; // ??????????
}
#endif

 
void DParagraph::Copy(DParagraph* it)
{
	just = it->just;
 	tablead = it->tablead; 				 
	deftabstop = it->deftabstop;
  numstops = it->numstops;
	tabstops = it->tabstops;
	tabkinds = it->tabkinds;
	firstInset= it->firstInset;
	leftInset= it->leftInset;
	rightInset= it->rightInset;
	spaceBefore= it->spaceBefore;
	spaceAfter= it->spaceAfter;
	spaceBetween =spaceBetween;
	minLines = it->minLines;
	minHeight = it->minHeight;
	startsAt = it->startsAt;
	numRows = it->numRows;	 
	lineHeight = it->lineHeight;
	leadHeight = it->leadHeight; 
  
  fDataPtr = it->fDataPtr;	 
	fText = it->fText;			 
	fStyleIndex = it->fStyleIndex;
	fNumStyles = it->fNumStyles;

	borderstyle	= it->borderstyle;
	bordertop  = it->bordertop;
	borderbottom = it->borderbottom;
	borderleft  = it->borderleft;
	borderright = it->borderright;
	borderbetween = it->borderbetween;
	borderbar = it->borderbar;
	borderbox = it->borderbox;
 	
	openSpace = it->openSpace;
	keepWithNext = it->keepWithNext;
	keepTogether = it->keepTogether;
	newPage = it->newPage;
	minLines = it->minLines;
	minHeight = it->minHeight;
	wrap = it-> wrap;

	notCached = it->notCached;
	neverCached = it->neverCached;
	fDocOwnsData= it->fDocOwnsData;
	fDocOwnsStyles= it->fDocOwnsStyles;
}



DParagraph::DParagraph()
{
	Initialize();
}

DParagraph::DParagraph( char* data, Boolean docOwnsData, short lines, DParagraph* parFmt, 
                       StyleIndex* styles, short numstyle, Boolean docOwnsStyles) 
{
	Initialize();
	if (parFmt) {
		//*this= *parFmt;
		this->Copy( parFmt);
    minLines= MAX( parFmt->minLines, (short) 1);
    fText= NULL;
    tabkinds= (char*) MemDup( parFmt->tabkinds, numstops*sizeof(char) );
    tabstops= (short*)MemDup( parFmt->tabstops, numstops*sizeof(short) );
  	} 
 	fDataPtr= data;
	fDocOwnsData = docOwnsData;
	fDocOwnsStyles = docOwnsStyles;
 	fStyleIndex= styles;
 	fNumStyles= numstyle;
	numRows = MAX(lines, 1);
}

Boolean DParagraph::DoBorder()
{
	return borderright || borderleft || borderbottom || bordertop || borderbox;
}

void DParagraph::DrawBorder(DRichView* itsView, short row, short item, Nlm_RecT* rpara, Nlm_RecT& rct)
{
	Nlm_PoinT		pt1, pt2;
	DParagraph *otherItem;

	if (!DoBorder()) return;
	switch (borderstyle) {
		case kBorderThick	: Nlm_WidePen(3); break;
		case kBorderDot		: Nlm_Dotted(); break;
		case kBorderDash	: Nlm_Dashed(); break;
		case kBorderDouble: break;
		case kBorderSingle: break;
		case kBorderShadow: break;
		case kBorderHair	: break;
		}

	if (row == 0 && (bordertop || borderbox)) {
  	otherItem = itsView->GetParaPtr(item-2);  //! item# is +1 of parag# 
  	if (!otherItem || !(otherItem->bordertop || otherItem->borderbox)) {
			pt1.x= rpara->left;
			pt2.x= rpara->right-1;
			pt1.y= pt2.y= rct.top;
			Nlm_DrawLine(pt1,pt2);
			if (borderstyle == kBorderDouble) {
				pt1.y= pt2.y= rct.top-2;
				Nlm_DrawLine(pt1,pt2);
				}
			}
		}
		
	if (row == numRows-1 && (borderbottom ||borderbox)) {
  	otherItem =  itsView->GetParaPtr(item);    //! item# is +1 of parag# 
		if (!otherItem || !(otherItem->borderbottom || otherItem->borderbox)) {
			pt1.x= rpara->left;
			pt2.x= rpara->right-1;
			pt1.y= pt2.y= rct.bottom;
			Nlm_DrawLine(pt1,pt2);
			if (borderstyle == kBorderDouble) {
				pt1.y= pt2.y= rct.bottom+2;
				Nlm_DrawLine(pt1,pt2);
				}
			}
		}
		
	if (borderleft || borderbox) {
		pt1.x= pt2.x= rpara->left;
		pt1.y= rct.top;
		pt2.y= rct.bottom;
		Nlm_DrawLine(pt1,pt2);
		if (borderstyle == kBorderDouble) {
			pt1.x= pt2.x= rpara->left-2;
			Nlm_DrawLine(pt1,pt2);
			}
		}
		
	if (borderright || borderbox) {
		pt1.x= pt2.x= rpara->right-1; /* past scrollbar...*/
		pt1.y= rct.top;
		pt2.y= rct.bottom;
		Nlm_DrawLine(pt1,pt2);
		if (borderstyle == kBorderDouble) {
			pt1.x= pt2.x= rpara->right+2;
			Nlm_DrawLine(pt1,pt2);
			}
		}
		
	Nlm_ResetDrawingTools();
}


void DParagraph::SetTab( Boolean attab, short rleft, Nlm_RecT& drawr, Nlm_RecT* rpara, 
												 Boolean& bartab, char& just)
{
	if (drawr.left < rleft) // rpara->left + leftInset + firstInset 
		drawr.left= rleft;
		
	else if (numstops) {
	  short istop, nextstop;
	  istop= 0;
	  do { 
	  	// nextstop = rpara->left + leftInset + tabstops[istop]; 
	  	nextstop = rleft + tabstops[istop];
	  	istop++; 
	  	}
	  while (istop<numstops && nextstop>0 && nextstop<drawr.left);
	  
	  // ?? 27may
	  nextstop = MIN(nextstop, rpara->right - rightInset/* - width*/);
	  
	  if (attab)
	  	drawr.left= nextstop;
	  else switch (tabkinds[istop-1]) {
	  	case kTabBar:
	  			bartab= true;
	  	case kTabDecimal: 
	  	case kTabLeft	: 
	  			just= 'l'; 
	 				drawr.left= nextstop;
					break;
	  	case kTabRight: 
	  			just= 'r'; 
	  			drawr.right= nextstop;
	  			break;
	  	case kTabCenter: 
	  			just= 'c'; 
	  			drawr.right= nextstop + (nextstop - drawr.left);
	  			break;
	  	}
		}
		
	else { 
	  short nextstop= rleft; // rpara->left + >leftInset
	  if (deftabstop>0) {
			do { nextstop += deftabstop; }
			while (nextstop>0 && drawr.left > nextstop);
			}
	  drawr.left= nextstop;
	  }
}


void DParagraph::DrawTab( DRichView* itsView, Nlm_RecT tabr, Nlm_RecT& drawr, Nlm_PoinT& atpt, 
												  short yfont, Boolean bartab, Boolean dodraw)
{
  if (tablead > ' ') {
  	char	tabbuf[256];
  	short	cwidth, tabcount= 0;
  	
  	cwidth= itsView->CharWidth(tablead);
		tabr.right= drawr.left;
		tabcount= MIN(254, MAX(1, (tabr.right - tabr.left - 1) / cwidth + 1));
		Nlm_MemFill( tabbuf, tablead, tabcount);
		tabbuf[tabcount]= 0;
 		Nlm_MoveTo(tabr.left, yfont); // need to adjust for leading
 		if (dodraw) Nlm_PaintString(tabbuf);
  	}
   
	if (bartab) {
		Nlm_PoinT pt1, pt2;
		pt1.x= pt2.x= drawr.left;
		pt1.y= tabr.top;
		pt2.y= tabr.bottom;
		if (dodraw) Nlm_DrawLine(pt1,pt2);
		atpt.x= ++drawr.left;
		}
}


