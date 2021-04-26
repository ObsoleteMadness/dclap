// DGopherPlus.cp
// d.g.gilbert



#include <ncbi.h>
#include <dgg.h>
#include <Dvibrant.h>

#include "DTCP.h"
#include "DGopher.h"
#include "DGoList.h"
#include "DGoInit.h"
#include "DGoClasses.h"

#include "DGoPlus.h"





// DGoMenuBlock ------------------------------

#if 0
class DGoMenuBlock : public DObject {
public:
	char*	fData;	// raw from server
	DGopherList*	fGolist;
	DGopher* 	fParent;
	
	DGoMenuBlock(DGopher* itsParent, const char* itsData);
	virtual ~DGoMenuBlock();
};
#endif

DGoMenuBlock::DGoMenuBlock(DGopher* itsParent, const char* itsData)
{
	fData= StrDup(itsData); //??
	fParent= itsParent; //?? newOwner
	fGolist= new DGopherList(fParent);
	// fix fData so leading space is gone from it, 
	// +MENU:cr
	//  +INFO:...
	//  +INFO:...
	char *cp, *newp, thisc, lastc;
	for (cp= newp= fData, lastc= 0; *cp; cp++) {
		thisc= *cp;
		if (!(thisc == ' ' && (lastc == kCR || lastc == kLF))) 
			*newp++= thisc;
		lastc= thisc;
		}
	*newp= 0;
	fGolist->ReadLinks(fData);
}

DGoMenuBlock::~DGoMenuBlock()
{
	MemFree(fData);
	if (fGolist) { fGolist->FreeAllObjects(); fGolist->suicide(); }
}




#if 0

class DGoMenuItem : public DObject {
public:
	Boolean	fIsDefault;
	DGoMenuItem() {}
};

class DGoMenuRect : public DGoMenuItem {
public:
	Nlm_RecT	fRect;
	DGoMenuRect(const char* itsData);
};

class DGoMenuBytes : public DGoMenuItem {
public:
	long	fCharstart, fCharend;
	DGoMenuBytes(const char* itsData);
};

class DGoMenuLine : public DGoMenuItem {
public:
	long	fLinestart, fCharstart, fCharend;
	DGoMenuLine(const char* itsData);
};

class DGoMenuString : public DGoMenuItem {
public:
	char*	fFindstring;
	short	fFindcount;
	DGoMenuString(const char* itsData);
};

#endif



// DGopherItemView ------------------------------


DGopherItemView::DGopherItemView()
{
	Initialize("Text/plain");
}


DGopherItemView::DGopherItemView(const char* data) 
{
	Initialize( data);
}

DObject* DGopherItemView::Clone()
{
	DGopherItemView* aClone = new DGopherItemView(this->fViewVal);
	return aClone;
}

DGopherItemView::~DGopherItemView()
{
	fViewVal= (char*) MemFree(fViewVal);
}

//#define StrFromPointers(dst,src,sindex,slen,dmax)	Nlm_StrngSeg(dst,src,sindex,slen,dmax)
 
const char*	DGopherItemView::ViewVal()
{
	return  fViewVal;
}

const char*	DGopherItemView::Kind() 
{
	static char gItembuf[256];
	Nlm_StrngSeg( gItembuf, fViewVal, fIkind, fNkind, 256);
	StrLocase(gItembuf); // added 1jan95
	return gItembuf;
}

const char* DGopherItemView::Handler()
{
	// this is obsolete -- we now keep this info w/ Kind()
	static char gItembuf[256];
	Nlm_StrngSeg( gItembuf, fViewVal, fIhandler, fNhandler, 256);
	return gItembuf;
}

const char*	DGopherItemView::ViewRequest() 
{
	//gItembuf = "\t+" + CStr255((unsigned char*)**fViewVal).Copy(fIkind, fNrequest);
	// something is failing here b/n StrngSeg and return ?!
	
	static char gItembuf[256];
	Nlm_StrngSeg( gItembuf+2, fViewVal, fIkind, fNrequest, 254);
	gItembuf[0]= '\t';
	gItembuf[1]= '+';
	return gItembuf;
}


const char*	DGopherItemView::ViewStatus() 
{
	if (fViewStatus == kUnknownStatus) return "?";
  else if (fViewStatus == kMaxStatus) return "*";
  else return " "; 
}


const char*	DGopherItemView::Language()
{
	static char gItembuf[256];
	Nlm_StrngSeg( gItembuf, fViewVal, fIlanguage, fNlanguage, 256);
	return gItembuf;
}

const char*	DGopherItemView::DataSize()
{
	static char gItembuf[256];
	Nlm_StrngSeg( gItembuf, fViewVal, fIdatasize, fNdatasize, 256);
	return gItembuf;
}


void DGopherItemView::Initialize(const char* line) 
{
	// "Text/Postscript German: <100k>"	
	char *cp, *dp, *ep;
	
	fViewVal= NULL;
	fViewStatus= kUnknownStatus;

	for (cp = (char*)line; *cp!=0 && *cp<=' '; cp++) ;
	fViewVal= StrDup( cp);
	StrLocase(fViewVal);
	cp= fViewVal;
	fIdatasize= 0;
	fNdatasize= 0;

	dp= strchr( cp, ':');
	if (dp) { 
		char *sp = strchr( dp, '<');
		if (sp) {
			fIdatasize= long(sp+1 - cp);
			ep= strchr( sp,'>'); 
			if (ep) fNdatasize= long(ep - sp) - 1;
			else fNdatasize= strlen(cp) - fIdatasize; 
			}
		}
		
	for (ep= cp; *ep!=0 && *ep>' ' && *ep!=':'; ep++) ;
	Boolean dolang= (*ep != ':');
	
	// 14Mar94: drop separation of kind/handler 
	// gopher+ & mime both use "/" as integral part of kind  
#if 0
	dp= strchr(cp, '/');
	if (dp) {
		fIkind= 0; // 1 + cp - cp
		fNkind= long(dp - cp);
		fIhandler= long(dp+1 - cp);
		fNhandler= long(ep - dp) - 1;
		fNrequest= long(ep - cp);
		}
	else
#endif
	 {
		fIkind= 0;
		fNkind= long(ep - cp);
		fIhandler= 0;
		fNhandler= 0;
		fNrequest= fNkind;
		}
	
	if (dolang) {
		for (dp = ep+1; *dp!=0 && *dp<=' '; dp++) ;
		for (ep= dp; *ep!=0 && *ep>' ' && *ep!=':'; ep++) ;
		fIlanguage= long(dp - cp);
		fNlanguage= long(ep - dp) - 1;
		fNrequest= long(ep - cp);
		}
	else {
		fIlanguage= 0;
		fNlanguage= 0;
		}
}





 
 
 
// DGopherItemAsk ------------------------------
 

DGopherItemAsk::DGopherItemAsk()
{
	Initialize(kGopherAskUnknown, "", "","");
}

DGopherItemAsk::DGopherItemAsk(short kind, const char* question, 
									const char* answer, const char* nametag, const char* valuetag)
{
	Initialize(kind, question, answer, nametag, valuetag);
}

DGopherItemAsk::DGopherItemAsk(const char* data, Boolean isHTMLform)
{
	Initialize( data, isHTMLform);
}

DGopherItemAsk::~DGopherItemAsk()
{
	MemFree( fQuestion);	
	MemFree( fAnswer);	
	MemFree( fNametag);
	MemFree( fValuetag);
}

DObject* DGopherItemAsk::Clone()
{
	DGopherItemAsk* aClone = new DGopherItemAsk(fKind, fQuestion, fAnswer, fNametag, fValuetag);
	return aClone;
}

void DGopherItemAsk::Initialize(const char* line, Boolean isHTMLform)
{
	//e.g., "Choose: Deliver electric shock to administrator now?|Yes|Not!"
	char *cp, *ep, *myline, *quest = NULL, *ans = NULL, *name = NULL, *value= NULL;
	short	kind = kGopherAskUnknown;
	
	for (cp = (char*)line; *cp!=0 && *cp<=' '; cp++) ;
	cp= myline= StrDup( (char*)line);
	
	ep= StrChr( cp, ':');
	if (ep) { 
		*ep= 0;
		if (0 == Strcasecmp( cp, "Ask") ) {
			kind = kGopherAskAsk;
			}
		else if (0 == Strcasecmp( cp, "Choose") ) {
			kind = kGopherAskChoose;
			}
		else if (0 == Strcasecmp( cp, "AskF") ) {
			kind = kGopherAskAskF;
			}
		else if (0 == Strcasecmp( cp, "AskP") ) {
			kind = kGopherAskAskP;
			}
		else if (0 == Strcasecmp( cp, "AskL") ) {
			kind = kGopherAskAskL;
			}
		else if (0 == Strcasecmp( cp, "Select") ) {
			kind = kGopherAskSelect;
			}
		else if (0 == Strcasecmp( cp, "ChooseF") ) {
			kind = kGopherAskChooseF;
			}
		else if (0 == Strcasecmp( cp, "Note") ||
						 0 == Strcasecmp( cp, "Comment") ) {
			kind = kGopherAskNote;
			}
		cp= ep+1;
		*ep= ':';

		if (isHTMLform) {
			ep= StrChr( cp, ':');
			if (ep) { 
				*ep= 0;
				name= StrDup(cp);
				cp= ep+1;
				*ep= ':';
				}
			ep= StrChr( cp, ':');
			if (ep) { 
				*ep= 0;
				value= StrDup(cp);
				cp= ep+1;
				*ep= ':';
				}
			}

		}
			  
	if (kind == kGopherAskUnknown) {
		quest= StrDup("");
		ans= StrDup("");
		}
	else if ((kind == kGopherAskSelect) && (NULL!=(ep= strchr( cp, ':'))) ) { 
		*ep= 0;
		quest= StrDup(cp);
		cp= ep+1;
		ans= StrDup(cp);
		*ep= ':';
		}
	else if ((ep= strchr( cp, '\t'))!=NULL) { 
		*ep= 0;
		quest= StrDup(cp);
		cp= ep+1;
		ans= StrDup(cp);
		*ep= '\t';
		}
	else {
		quest= StrDup(cp);
		ans= StrDup("");
		}
	MemFree(myline);

#if SILLYBOY
	Initialize(kind,quest,ans,name);
	MemFree(quest);
	MemFree(ans);
	MemFree(name);
#else
	fKind= kind;
	fChoice= 1;
	fQuestion	= quest;
	fAnswer = ans;
  fNametag= name;
  fValuetag= value;
#endif
}

void DGopherItemAsk::Initialize(short kind, const char* question, 
									const char* answer, const char* nametag, const char* valuetag)
{
	fKind= kind;
	fChoice= 1;
	fQuestion	= StrDup((char*) question);
	fAnswer = StrDup((char*) answer);
	fNametag	= StrDup((char*) nametag);
	fValuetag	= StrDup((char*) valuetag);
}
 
 
void DGopherItemAsk::SetAnswer( const char* answer)
{
	MemFree( fAnswer);
	fAnswer= StrDup((char*) answer);
}


void DGopherItemAsk::ItemChoice( short choice)
{
	fChoice= choice;
}




 