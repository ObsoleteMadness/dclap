// DGopherList.cp 
// by d.g. gilbert, Mar 1992 
// dclap version jan 94
/*
			This code is Copyright (C) 1992 by D.G. Gilbert.
			All Rights Reserved.

			You may use this code for your personal use, to provide a non-profit
			service to others, or to use as a test platform for a commercial
			implementation. 

			You may not use this code in a commercial product, nor to provide a
			commercial service, nor may you sell this code without express
			written permission of the author.  
			
			gilbertd@bio.indiana.edu 
			Biology Dept., Indiana University, Bloomington, IN 47405 
*/


#include <ncbi.h>
#include <dgg.h>

#include "DTCP.h"
#include "DGopher.h"
#include "DURL.h"
#include "DGoList.h"
#include "DGoPlus.h"
#include "DGoInit.h"
#include "DGoClasses.h"

#include <DList.h>
//#include <DView.h>

// this dup's DGopher.cpp ! Beware -- need public const DGopher::kLocalhost
Local const char* kLocalhost = "localhost";


// DGopherList ------------------------------

#define Inherited DList

// static
short dglCompareGophers( DGopher* gopher1, DGopher* gopher2)
{
	if ( gopher1->Equals( gopher2) ) 
		return 0;
#if 0
	char*	name1= (char*) (gopher1)->GetName();
	char*	name2= (char*) (gopher2)->GetName();
	short val= StrICmp( name1, name2);
	if (val) return val;
#endif
	else if (gopher1 > gopher2) return 1;
	else return -1;
} 


DGopherList::DGopherList(  DGopher* parentMenu) :
	DList(), 			//::dglCompareGophers),  // comparitor !?
	fStatusLine(NULL),
	fNewGopher(NULL),
	fListUnknowns(gListUnknowns),
	fSortOrder(kSortByItem),
	fParentMenu(parentMenu)
{
	gNeedTypeChange= false;
}

void DGopherList::FreeAllObjects()
{
	long i, n= this->GetSize();
	for (i= 0; i<n; i++) {
		DGopher* obj= GopherAt(i);
		if (obj->GetOwnerCount() <= 1) delete obj;
		else obj->suicide(); // this alone doens't call obj destructors !!
		}
	this->DeleteAll();
} 

Boolean DGopherList::suicide(void) 
{ 
	if (GetOwnerCount() <= 1) { 
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide();
}

Boolean DGopherList::suicide(short ownercount)
{
	if (ownercount < 1) {
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide(ownercount);
}

void DGopherList::SetStatus(DView *statusline)
{
	fStatusLine= statusline;
}


// static
DGopher* DGopherList::NewGopher(char itemType, char* info, char* plusinfo,
	 short& status, DGopherList* ownerList)
{
	DGopher*	theGopher= NULL;
	short		isPlus = status;
	Boolean	hasAsk = false;
	
	status= DGopher::kNewGopherOkay;
		/* Recursion note: could this sometimes lead to inf. loop w/ wrong choice
			of server && local gopher types? */

	if (itemType == '+') {
		// ?? a read error on gopher+ item ...
		}

	if (gDoSuffix2MacMap && gGopherMap) {
		DGopherMap* mapper= gGopherMap->MatchGopherType( itemType, false);
		if (mapper) itemType= mapper->fLocalType;
		}

	switch (itemType) {
		case kTypeNull:
		case kTypeEndOfData	:  
			theGopher= NULL;
			status= DGopher::kNewGopherEnd;  //'.', end of information 
			break;
			
		case kTypeFile   	: theGopher= new DTextGopher; break;
		case kTypeFolder	: theGopher= new DFolderGopher; break;
		case kTypeQuery   : theGopher= new DIndexGopher; break;
		case kTypeBinhex	: theGopher= new DBinhexGopher; break;
		case kTypeTelnet  : theGopher= new DTelnetGopher; break;
#if FIX_LATER
		case kTypeUuencode: theGopher= new DUuencodeGopher; break;
		case kTypeCSO			: theGopher= new DCSOPhoneBookGopher; break;
		case kTypeWhois		: theGopher= new DWhoisPhoneBookGopher; break;
#endif // FIX_LATER
		case kTypeBinary  : theGopher= new DBinaryGopher; break;
		case kTypeSound 	: theGopher= new DSoundGopher; break;
		case kTypeGif			: 			
		case kTypeImage		: theGopher= new DImageGopher; break;
		case kTypeMovie		: theGopher= new DMovieGopher; break;
		case kTypeNote		: theGopher= new DNoteGopher; break;
		case kMailType		: theGopher= new DMailGopher; break;
		case kTypeHtml   	: theGopher= new DHtmlGopher; break;
		//case kTypeTn3270 	: theGopher= new DTN3270Gopher; break;
		//case kTypeMime   	: theGopher= new DGopher; break;
		//case kTypeBinhexpc: theGopher= new DGopher; break;

		case kTypeError : 	 
		default	: 
			if (gListUnknowns) theGopher= new DGopher;  
			else {
				theGopher= NULL;
				status= DGopher::kNewGopherNone;
				}
			break;
		}

	if (status == DGopher::kNewGopherOkay && info) { 
		theGopher->SetLink( itemType, info);
		if (theGopher->fIplus) {   
			char *pp= theGopher->fLink + theGopher->fIplus; 
			hasAsk= (*pp == '?');
			if (hasAsk || *pp == '+' || *pp == '#') isPlus= kGopherPlusYes;
			}
		theGopher->SetOwner( ownerList);  
		theGopher->SetPlusInfo( isPlus, hasAsk, plusinfo);
		
		if (theGopher->fIpath == 0 && theGopher->fIhost == 0) {
			delete theGopher; 
			theGopher= NULL;
			status= DGopher::kNewGopherNone;
			}
		else if (gNeedTypeChange) {  
			// Recursion alert: CopyToNewKind calls back NewGopher, but not to here...info==nil
		  DGopher* bGopher= 
		  	DGopherList::CopyToNewKind( theGopher->fType, theGopher, ownerList);
		  if (bGopher!=NULL) {
				delete theGopher; 
				theGopher= bGopher;
				}
			}
 		gNeedTypeChange= false; 
		}
	return theGopher;	
}


// static
DGopher* DGopherList::CopyToNewKind(char itemType, DGopher* srcGopher, 
									DGopherList* ownerList, Boolean copyMacStuff)
{
	short	status = srcGopher->fIsPlus;
	DGopher* destGopher= DGopherList::NewGopher( itemType, NULL, NULL, status, ownerList);
	if (status == DGopher::kNewGopherOkay) {
			//destGopher->SetLink( itemType, *srcGopher->fLink);
		destGopher->CopyGopher( srcGopher);

			// THIS IS a QUICK FIX for moving srcGopher to destGopher + delete srcGopher...
			// this code is probably no longer needed, as we clone fViews,fAskers in CopyGopher
		if (srcGopher->fViews && !destGopher->fViews) { 
			destGopher->fViews= srcGopher->fViews; 
			srcGopher->fViews= NULL;
			}
		if (srcGopher->fAskers && !destGopher->fAskers) { 
			destGopher->fAskers= srcGopher->fAskers; 
			srcGopher->fAskers= NULL;
			}
		if (srcGopher->fAbstract && !destGopher->fAbstract) { 
			destGopher->fAbstract= srcGopher->fAbstract; 
			srcGopher->fAbstract= NULL;
			}
		
			// !! Need to change VIEW types to coincide w/ this (e.g., from text/plain to app/...)
		destGopher->fType= itemType;
		if (itemType != srcGopher->fType) {
			destGopher->DeleteViews();
			}

		if (copyMacStuff) {
			destGopher->fTransferType	= srcGopher->fTransferType;  
			destGopher->fMacType	= srcGopher->fMacType; 			 
			destGopher->fMacSire	= srcGopher->fMacSire;	 
			destGopher->fSaveToDisk	= srcGopher->fSaveToDisk;	 
			destGopher->fLaunch		= srcGopher->fLaunch;	 
			}

		return destGopher;
		}	
	else
		return NULL;	
}


// static
short DGopherList::CompareGophers( void* gopher1, void* gopher2)
{
	if ( ((DGopher*) gopher1)->Equals( (DGopher*) gopher2) ) 
		return 0;

	char*	name1= (char*) ((DGopher*)gopher1)->GetName();
	char*	name2= (char*) ((DGopher*)gopher2)->GetName();
	short val= StrICmp( name1, name2);
	if (val) return val;
	else if (gopher1 > gopher2) return 1;
	else return -1;
} 



short dglCompareByType( void* item1, void* item2)
{
	char	name1= ((DGopher*)item1)->fType;
	char	name2= ((DGopher*)item2)->fType;
	if (name1 < name2) return -1;
	else if (name1 > name2) return 1;
	else  return 0;
}


short dglCompareBySize( void* item1, void* item2)
{
	// We want INVERTED sort -- greatest size first
	unsigned long	name1= ((DGopher*)item1)->fSizeLong;
	unsigned long	name2= ((DGopher*)item2)->fSizeLong;
	if (name1 > name2) return -1;
	else if (name1 < name2) return 1;
	else  return 0;
}

short dglCompareByDate( void* item1, void* item2)
{
	// We want INVERTED sort -- greatest date first
	unsigned long	name1= ((DGopher*)item1)->fDateLong;
	unsigned long	name2= ((DGopher*)item2)->fDateLong;
	if (name1 > name2) return -1;
	else if (name1 < name2) return 1;
	else  return 0;
}

short dglCompareByHost(void* item1, void* item2)
{
	char*	name1= (char*) ((DGopher*)item1)->GetHost();
	char*	name2= (char*) ((DGopher*)item2)->GetHost();
	return StrICmp( name1, name2);
}

short dglCompareByName( void* item1, void* item2)
{
	char*	name1= (char*) ((DGopher*)item1)->GetName();
	char*	name2= (char*) ((DGopher*)item2)->GetName();
	return StrICmp( name1, name2);
}


Boolean DGopherList::SortList(Sorts sortorder)
{
	if (sortorder == fSortOrder)
		return false;
	else {
		fSortOrder= sortorder;
		switch( sortorder) {
			case kSortByKind :
				this->SortBy( dglCompareByType);
				break;
			case kSortBySize : 
				this->SortBy( dglCompareBySize);
				break;
			case kSortByDate :
				this->SortBy( dglCompareByDate);
				break;
			case kSortByHost : 
				this->SortBy( dglCompareByHost);
				break;
			case kSortByName : 
				this->SortBy( dglCompareByName);
				break;
			case kSortByItem: 
			default:
				this->Sort();
				break;
			}
		return true;
		}
}



Boolean DGopherList::MakeNewGopher( char *pInfo, char *eInfo, 
													char *pPlus, char *ePlus, short status)
{
	char	termInfo, termPlus;
	if (eInfo) { termInfo= *eInfo; *eInfo= 0; }
	if (ePlus) { termPlus = *ePlus; *ePlus= 0;  } 
	fNewGopher= DGopherList::NewGopher( *pInfo, pInfo, pPlus, status, this);
	if (eInfo) *eInfo= termInfo;
	if (ePlus) *ePlus= termPlus;
	switch (status) {
		case DGopher::kNewGopherEnd : return false;
		case DGopher::kNewGopherNone: return true; 
		case DGopher::kNewGopherOkay: this->InsertLast( fNewGopher); return true;
		}
	return false;
}



Boolean DGopherList::IsGopherLine( char *line)
{
	// this is a static function, usable w/o a DGopherListDoc object
	// look for something like ( | == tab)
	// start w/ optional "+INFO: "
	// 1Any title string|optional path string|required host string|required port string|optional +
	enum { kTitle, kPath, kHost, kPort, kPlus };
	const short	kTabsInGopherItem = 3;

	char *np, *ep, *cp = line;
	np= StrChr( cp, '\n'); 
	if (!np) np= StrChr( cp, '\r');
	if (!np) np= StrChr( cp, '\0'); // ?? or return false here
	for (short itab=0; itab<kTabsInGopherItem; itab++) {
		if ( (ep= StrChr( cp, '\t')) == NULL) 
			return false;
		else if (ep > np)
			return false;
		else switch (itab) {
			case kTitle: if (ep < cp+2) return false; break;
			case kPath: break; // can be null
			case kHost: break; //<< null here in error type... //if (ep < cp+2) return false;
			case kPort: if (ep < cp+1) return false; break;
			}
		cp = ep+1;
		}
	return true;
}


// static
DGopher* DGopherList::GopherFromURL( char* url, long urlsize, 
													DGopherList* itsList, Boolean doInsertLast, char itemType)
{
	short	status;
	char	calltype;
	char *info = "0Link from URL\t0localdoc\tlocalhost\t70";
	char *plusinfo = NULL;
	DGopher* theGo;
	
	calltype= itemType;
	if (!itemType) itemType= '0';
	theGo= DGopherList::NewGopher( itemType, info, plusinfo, status, itsList);
	switch (status) {

		case DGopher::kNewGopherOkay: {
				char oldtype= theGo->fType; 
				if (DURL::ParseURL( theGo, url, urlsize, true)) {
				  if (calltype) 
						theGo->fType= calltype;
					else if (theGo->fType != oldtype) {
						DGopher* bGopher= 
							DGopherList::CopyToNewKind( theGo->fType, theGo, itsList);
					  if (bGopher) {
							delete theGo; 
							theGo= bGopher;
							}
						}
					if (doInsertLast && itsList) itsList->InsertLast( theGo); 
					}
				else {
					delete theGo; 
					theGo= NULL;
					}	
				}
				break;

		case DGopher::kNewGopherEnd :  
		case DGopher::kNewGopherNone: 
				if (theGo) delete theGo; 
				theGo= NULL;
				break;
		}
	if (itsList) itsList->fNewGopher= theGo;
	return theGo;
}

// static

DGopher*  DGopherList::LocalGopher( char* name)
{	
	char docurl[1024];
	DGopher* theGo= NULL;
	char   quote= '\'';	
	if (StrChr( name, quote)) quote= '"';
	StrCpy(docurl, " file:///");
	StrNCat(docurl, name, 1000);
	StrNCat(docurl, " ", 1000); 
	docurl[1023]= 0;
	long doclen= StrLen(docurl);
	docurl[0]= quote; // so parseURL doesn't eat spaces
	docurl[doclen-1]= quote;
	theGo= DGopherList::GopherFromURL( docurl, doclen, NULL, false, 0);
	if (theGo) theGo->StoreName( (char*)gFileManager->FilenameFromPath(name));
	return theGo;
}

DGopher*  DGopherList::LocalGopher( DFile* theFile)
{	
	return LocalGopher(  (char*) theFile->GetName());
}





void DGopherList::ReadLinks( char* linkptr)
{	
	if (!linkptr) return;
	ReadLinks( linkptr, StrLen(linkptr));
}

void DGopherList::ReadLinks( char *linkptr, long len)
{
	Boolean done= false, haveinfo= false;
	register char 	*cp, *ci;
	char		*cpend, *ep= NULL, *pInfo= NULL, *eInfo= NULL, *pPlus= NULL;
	short 	pluskind = kGopherPlusDontKnow;
	
			// ?? parse different possible formats:
			//	\n+INFO: 
			//  \n?...name...\t...path...\t...host...\t...#port...+maybe more\n
			//  \nName=...\nHost=...\nPath=...\nPort=...\nType=...\nOther=
			//	URL:? &/or   gopher://   or  ftp://  or other://
			//
			// ? some gopher+ server fails to send CR-LF, just sends LF ?!?
			// also check for 3 tabs in first/info line -- skip till found to avoid bad data
			// from some stupid servers.
			
	if (!linkptr) return;
	cp= pInfo= linkptr;
	cpend= cp + len; //-1;  
	Boolean isPlus= (*cp == '+');

	while (!done && cp) {
		ep= cp;
		while (*cp < ' ' && *cp != 0 && cp != cpend) cp++;	// leave leading spaces in line !?!!
		if (*cp == 0 || cp == cpend) done= true; // break;  !! need to process data yet
		if (*cp == '+' && (*ep == kCR || *ep == kLF) && 0 == Strncasecmp( cp, "+INFO:", 6)) 
			  isPlus= true; 

		if (isPlus) {
			Boolean atinfo = (0 == Strncasecmp( cp, "+INFO:", 6) );
			
			if (haveinfo && (atinfo || done)) {
				char* ePlus= (ep > pPlus) ? ep : pPlus;
				pluskind = kGopherPlusDontKnow; //kGopherPlusYes
				if (! MakeNewGopher( pInfo, eInfo, pPlus, ePlus, pluskind))  
					done= true; 
				haveinfo= false;
				pInfo= eInfo= pPlus= NULL;
				}
			
			if (done) 
				break; // nada
				
			else if (atinfo) {
				haveinfo= true;
				cp += 6;
				while (*cp <= ' ' && *cp != 0 && cp != cpend) cp++;
				pInfo= cp;
				//eInfo= strchr(pInfo, kCR); if (!eInfo) eInfo= strchr(pInfo, kLF);
				for (ci= pInfo, eInfo= NULL; ci<cpend && *ci!=0; ci++) 
					if (*ci == kCR || *ci == kLF) { eInfo= ci; ci= cpend;}
						
				if (eInfo) {
					pPlus= eInfo+1; 
					while (*pPlus <= ' ' && *pPlus != 0 && pPlus != cpend) pPlus++;
					}
				else pPlus= NULL;
				}
	 
			else if (*cp != '+' && *cp != ' ') { 
					// this may be Directory- list from gopher+ server
				char	*tp = NULL, *lastt;
				short  itab = 0;
				for (ci= cp; ci<cpend && *ci!=0; ci++) if (*ci == '\t') { tp= ci; ci= cpend; }
				if (tp) {
					lastt = tp+1;
					for (itab=0; itab<4 && tp!=NULL; itab++) { 
						lastt= tp+1; 
						tp= NULL;
						for (ci= lastt; ci<cpend && *ci!=0; ci++) if (*ci == '\t') { tp= ci; ci= cpend; }
						}
					}
				if (itab>2) {
#if 1
					pluskind =kGopherPlusDontKnow;
#else
					if (itab > 3 && *lastt == '+') pluskind = kGopherPlusYes;
					else pluskind = kGopherPlusNo;
#endif
					pInfo= cp;
					for (ci= pInfo, eInfo= NULL; ci<cpend && *ci!=0; ci++) 
						if (*ci == kCR || *ci == kLF) { eInfo= ci; ci= cpend;}
					if (! MakeNewGopher( pInfo, eInfo, NULL, NULL, pluskind)) done= true;		
					}
				}
 
			}
			
		else if (done) 
				break; 
				
		else if (IsGopherLine( cp)) {
			pInfo= cp;
			pluskind =kGopherPlusDontKnow;
			for (ci= pInfo, eInfo= NULL; ci<cpend && *ci!=0; ci++) 
				if (*ci == kCR || *ci == kLF) { eInfo= ci; ci= cpend;}
			if (! MakeNewGopher( pInfo, eInfo, NULL, NULL, pluskind))
				done= true;		
			}

		else if (0 == Strncasecmp( cp, "+URL:", 5) ) {
			cp += 5;
			for (ci= cp, eInfo= NULL; ci<cpend && *ci!=0; ci++) 
				if (*ci == kCR || *ci == kLF) { eInfo= ci; ci= cpend;}
			char* saven= StrDup( (char*)fNewGopher->GetName());
			if (eInfo && DURL::ParseURL( fNewGopher, cp, eInfo-cp)) ;
			if (saven && *saven) fNewGopher->StoreName(saven); 
			MemFree(saven);
			}

		//cp= strchr(cp, kCR); if (!cp) cp= strchr(cp, kLF);
		for (ci= cp, cp= NULL; ci<cpend && *ci!=0; ci++) 
			if (*ci == kCR || *ci == kLF) { cp= ci; ci= cpend;}
		}
}



char* DGopherList::WriteLinks(short indent)
{
	char* 	h= (char*) MemGet(1,true);
	long i, n= this->GetSize();
	for (i= 0; i<n; i++) {
		DGopher* ag= (DGopher*) this->At(i);
		ag->ToText(h, indent);
		}
	return h;
}



char* DGopherList::WriteLinksForServer()
//Note: cannot currently READ this format -- .link form on unix servers 
{
	short		itemnum= 0;
	char* 	h= (char*) MemGet(1,true);
	long i, n= this->GetSize();
	for (i= 0; i<n; i++) {
		DGopher* ag= (DGopher*) this->At(i);
		ag->ToServerText(h, i+1);
		}
	return h;
}


char* DGopherList::WriteLinksForDisplay( 
				Boolean showDate, Boolean showSize, Boolean showKind, Boolean showPath, 
				Boolean showHost, Boolean showPort, Boolean showAdmin) // pretty print
{
	short		itemnum= 0;
	char* 	h= (char*) MemGet(1,true);
	long i, n= this->GetSize();
	for (i= 0; i<n; i++) {
		DGopher* ag= (DGopher*) this->At(i);
		ag->ToPrettyText( h, i+1, 
				showDate, showSize, showKind,
				showPath, showHost, showPort, showAdmin);
		}
	return h;
}









#ifdef OS_MAC

#ifndef __TYPES__
#include <Types.h>
#endif
#ifndef __MEMORY__
#include <Memory.h>
#endif
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __FOLDERS__
#include <Folders.h>
#endif

#endif

#if defined(OS_DOS) || defined (OS_NT)
#include <dir.h>
#include <dos.h>
#include <io.h>
#endif

#ifdef OS_UNIX
#include <dirent.h>
#include <unistd.h>
#endif
 

class DLoGoInfo : public DObject
{
public:
	char	fKind;
	char * fName, * fDate, * fView;
	const char * fTitle;
	long	fSize;
	Boolean fPlus, fIsAsk, fHasAbs;
	DLoGoInfo(char kind, const char* name, const char* title, 
						const char* view, const char* date, long size) {
		fKind= kind;
		fSize= size;
		fView= (char*) view; // note no StrDup
		fName= StrDup(name);
		if (title) fTitle= title; else fTitle= fName;
		fDate= StrDup(date);
		fPlus= true;
		fIsAsk= fHasAbs= false;
		}
	~DLoGoInfo() {
		MemFree(fName);
		MemFree(fDate);
		}
};


short dglLoGoCompareByName( void* item1, void* item2)
{
	char*	name1= (char*) ((DLoGoInfo*)item1)->fTitle; //fName;
	char*	name2= (char*) ((DLoGoInfo*)item2)->fTitle; //fName;
	return StrICmp( name1, name2);
}


Local void StoreLoGoWais( short startwais, short endwais, DList* dirlist)
{
	short 	j;
	char	* suf;
	DLoGoInfo * jinfo;
	
	jinfo= (DLoGoInfo*) dirlist->At(startwais);
	jinfo->fKind= kTypeQuery;
	jinfo->fView= "application/gopher-menu"; //???
	//jinfo->fPlus= false; // ?? so app doesn't query for view info
	suf= StrRChr( jinfo->fName, '.');
	if (suf) *suf= 0;
	for (j= startwais+1; j<endwais; j++) {
		jinfo= (DLoGoInfo*) dirlist->At(j);
		jinfo->fKind= 0;
		}
	//nwais= 0;
}


// static
short DGopherList::LocalFolder2GopherList( DFile* outFile, char* folderpath)
{
	DList* dirlist= NULL;
	DLoGoInfo* info;
	char	namestore[256], pathstore[256], *nameat, buf[512];
	long	pathlen, date, ksize;
	char	* path, * view, * title, gokind;
	char* kUpfolderTitle =  " <-- Parent folder";
	
	if (!outFile || !folderpath || !*folderpath) 
		return -1;
	short port= DGopher::StandardPort( DGopher::kGopherprot);
	pathlen= StrLen(folderpath);
	StrNCpy(namestore, folderpath, 240);
	StrNCpy(pathstore, folderpath, 240);

#ifdef OS_MAC
{
	OSErr 				error = noErr;
	long 					dirID = 0, theDirID;
	short 				vRefNum = 0, itemIndex = 1; /* start with 1, then use what's returned */
	CInfoPBRec 		pb;
	Boolean 			isDirectory;
	StringPtr 		name;
	DateTimeRec  	dt;

	if (pathstore[pathlen-1] != ':') {
		pathstore[pathlen++]= ':'; 
		pathstore[pathlen]= 0;
		}
	nameat= pathstore+pathlen;
	
	Nlm_CtoPstr(namestore);
	name= (StringPtr) namestore;
	error = DetermineVRefNum( name, vRefNum, &pb.hFileInfo.ioVRefNum);
	if ( error != noErr ) return error;
	
	error = GetDirID( vRefNum, dirID, name, &theDirID, &isDirectory);
	if ( error != noErr ) return error;
	else if ( !isDirectory ) return error;

	dirlist= new DList();
	while ( error == noErr ) {  
		pb.hFileInfo.ioNamePtr = name;
		pb.hFileInfo.ioDirID = theDirID;
		pb.hFileInfo.ioFDirIndex = itemIndex;
		error = PBGetCatInfoSync(&pb);
		if ( error == noErr ) {
			if (itemIndex==1) {
				info= new DLoGoInfo( kTypeFolder, ":", kUpfolderTitle,
										"application/gopher+-menu", "19530424134647", 0);
				dirlist->InsertLast( info);
				}
			++itemIndex;	/* prepare to get next item in directory */
			
			//title= pb.hFileInfo.ioNamePtr; == name
			Nlm_PtoCstr( namestore);
			//StrNCpy( nameat, namestore, 256-pathlen);
			
			if ( (pb.hFileInfo.ioFlAttrib & ioDirMask) != 0 ) { 
				gokind= kTypeFolder;
				view= "application/gopher+-menu";
				date= pb.dirInfo.ioDrMdDat;
				ksize= pb.dirInfo.ioDrNmFls;
				}
				
			else {	
				//?? need to handle some special file types --
				// !! esp. need to map waissrc files to query type ??
				// -- map Mac Types: TEXT, ...? to gopher 0
				// map GIFf, PICT, BINA, ????, to various go types...
				// ?? use mapper classes !!?
				// !! need to check for .ASK blocks & ASK exec/scripts & handle that !?
				// !? need some form of .LINK file for changing go type/name !?
				
				gokind= kTypeFile;
				view= "text/plain";
				date= pb.hFileInfo.ioFlMdDat;
				ksize= pb.hFileInfo.ioFlLgLen / 1024;
				}
					
			// Mod-Date: Thu Feb  4 13:46:47 1993 <19930204134647>
			// write <19930204134647> == 1993 02 04 13 46 47
			Secs2Date( date, &dt); 
			sprintf( buf, "%04d%02d%02d%02d%02d%02d",  
							dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
			info= new DLoGoInfo( gokind, namestore, NULL, view, buf, ksize);
			dirlist->InsertLast( info);
			}
		}
}
#endif

#ifdef OS_UNIX
{
	// use readdir() routines to list directory
	// and stat(filename,info) to get size/date/ of file
	struct stat    statbuf;
  DIR		* theDir;
	struct dirent  *afile;
	char 		timeval[16];
	struct tm *tmthing;
	char	curdir[512];
	char	*name1;
  
	if (pathstore[pathlen-1] != '/') {
		pathstore[pathlen++]= '/'; 
		pathstore[pathlen]= 0;
		}
	nameat= pathstore+pathlen;

#define USECHDIR 1
#ifdef USECHDIR
  getcwd( curdir, sizeof(curdir));
	if (chdir(namestore)<0) return -1; // ??
	theDir = opendir(".");
#else
	theDir = opendir(namestore);
#endif
	if (!theDir) return -1;
	
	dirlist= new DList();
	for (afile = readdir(theDir); afile != NULL; afile = readdir(theDir)) {

	  StrNCpy( namestore, afile->d_name, sizeof(namestore));
		name1= namestore;
#ifndef USECHDIR
		StrNCpy( nameat, namestore, sizeof(pathstore)-pathlen);
		name1= nameat;
#endif

	  if (! stat( name1, &statbuf)) {
			if (StrCmp(namestore, ".")==0) {
				gokind= 0;
				view= NULL;
				}
			else if (StrCmp(namestore, "..")==0) {
				gokind= kTypeFolder;
				view= "application/gopher+-menu";
				title= kUpfolderTitle;
				}
			else if ( S_ISDIR(statbuf.st_mode)) {
				gokind= kTypeFolder;
				view= "application/gopher+-menu";
				}
			else {
				gokind= kTypeFile;
				view= "text/plain";
				}
			ksize= statbuf.st_size / 1024;
			
				// Mod-Date: Thu Feb  4 13:46:47 1993 <19930204134647>
			tmthing = localtime(&(statbuf.st_mtime));
			strftime( timeval, sizeof(timeval), "%Y%m%d%H%M%S", tmthing);
			sprintf( buf, "%s", timeval);
			info= new DLoGoInfo( gokind, namestore, NULL, view, buf, ksize);
			dirlist->InsertLast( info);
			}
		}
#ifdef USECHDIR
	chdir(curdir);
#endif
}
#endif

#if defined(OS_DOS) || defined (OS_NT)
{
	struct  ffblk  finfo;
	long		ltime;
	struct	ftime	dt, *dtp;
	
	if ((pathstore[pathlen-1] != '\\') && (pathstore[pathlen-1] != '*')) {
		pathstore[pathlen++]= '\\';   // "/" alone will get folder
		pathstore[pathlen++]= '*';   // ?? "/*.*" will get folder' contents
		pathstore[pathlen++]= '.';
		pathstore[pathlen++]= '*';
		pathstore[pathlen]= 0;
    pathlen -= 3; // drop *.* for nameat !!
		}
	nameat= pathstore+pathlen;
	
	short err= findfirst( pathstore, &finfo, FA_RDONLY | FA_DIREC);
	if (err) return -1;
	dirlist= new DList();
	while ( err==0 ) {
	
	  StrNCpy( namestore, finfo.ff_name, sizeof(namestore));
		//StrNCpy( nameat, namestore, sizeof(pathstore)-pathlen);
	  title= NULL;
		if (StrCmp(namestore, ".")==0) {
			gokind= 0;
			view= NULL;
			}
		else if (StrCmp(namestore, "..")==0) {
			gokind= kTypeFolder;
			view= "application/gopher+-menu";
			title= kUpfolderTitle;
			}
		else if ( finfo.ff_attrib & FA_DIREC) {
			gokind= kTypeFolder;
			view= "application/gopher+-menu";
			}
		else {
			gokind= kTypeFile;
			view= "text/plain";
			}
		ksize= finfo.ff_fsize / 1024;
				
		// Mod-Date: Thu Feb  4 13:46:47 1993 <19930204134647>
		// dos file times are weird
		ltime= (finfo.ff_fdate << 16 + finfo.ff_ftime); // this is a GUESS !! probly wrong
		//dt = (ftime) ltime;
    MemCpy( &dt, &ltime, sizeof(dt));
		sprintf(buf, "%04d%02d%02d%02d%02d%02d",
             (dt.ft_year + 1980), dt.ft_month, dt.ft_day, 
							dt.ft_hour, dt.ft_min, dt.ft_tsec*2);
		info= new DLoGoInfo( gokind, namestore, title, view, buf, ksize);
		dirlist->InsertLast( info);
		
		err= findnext( &finfo);
		}
}
#endif

		// other OSes, don't know how to write directory
		
		
	if (dirlist) {
		// do various checks here:
		// -- handle .ASK blocks
		// -- handle WAIS index.{dct,dlm,doc,fmt,fni,fnn,hl,inv,src,query.lock}
		//      and index_field_*.dct, index_field_*.inv
		// -- handle any .LINK files

		short		startwais, stopwais, nwais= 0;
		Boolean	gotwais;
		char		indexname[256];
		char		* name, * suf, * plus;
		long		namelen, indexlen;
		DLoGoInfo * info, * jinfo;
		short i, n= dirlist->GetSize();

		dirlist->SortBy( dglLoGoCompareByName);
		
		for (i=0; i <= n; i++) {
			gotwais= false;
			if (i<n) {
				info= (DLoGoInfo*) dirlist->At(i);
				name= info->fName;
				suf= StrRChr( name, '.');
				namelen= StrLen(name);
				}
				
			if (suf) {
				namelen= MIN( sizeof(indexname), namelen-StrLen(suf));
				suf++;
				
				if (StrICmp( suf, "ask") == 0 && i>0) {
					// handle .ask
					jinfo= (DLoGoInfo*) dirlist->At(i-1);
					if (StrNCmp(jinfo->fName, info->fName, namelen)==0) {
						jinfo->fIsAsk= true; // jinfo is the application for the .ask block !?
						info->fKind= 0; // don't show
						}
					}
					
#if defined(OS_DOS) || defined(OS_NT)
 				if (StrICmp( suf, "abs") == 0 && i>0)
#else
				if (StrICmp( suf, "abstract") == 0 && i>0)
#endif
					{
					jinfo= (DLoGoInfo*) dirlist->At(i-1);
					if (StrNCmp(jinfo->fName, info->fName, namelen)==0) {
						jinfo->fHasAbs= true; // jinfo is the application for the .abs block !?
 						info->fKind= 0; // don't show
						}
 					}
					
				else if ( nwais && StrStr(name,indexname) == name) {
					if ( ( namelen == indexlen || StrStr(name,"_field_") != NULL 
						|| StrICmp(suf,"lock") == 0) 
					&& (StrICmp( suf, "dlm")==0
					 || StrICmp( suf, "dct")==0	// this is for field, but watch for new nonfld !
					 || StrICmp( suf, "doc")==0
					 || StrICmp( suf, "fmt")==0
					 || StrICmp( suf, "fni")==0
					 || StrICmp( suf, "fnn")==0
					 || StrICmp( suf, "hl" )==0
					 || StrICmp( suf, "inv")==0
					 || StrICmp( suf, "lock")==0
					 || StrICmp( suf, "src")==0 )
					 ) {
						nwais++;
						gotwais= true;
						}
					}
				else if ( StrICmp( suf, "dct")==0) { 
					// always 1st in sorted list of wais names
					// do this "dct" test after above, as "_field_" dct would conflict
					if (nwais) {
						StoreLoGoWais( startwais, i, dirlist);
						nwais= 0;
						}
					gotwais= true;
					nwais= 1;
					startwais= i;
					StrNCpy(indexname, name, namelen);
					indexname[namelen]= 0;
					indexlen= namelen;
					}
				}
				
			if (nwais && !gotwais) {
				StoreLoGoWais( startwais, i, dirlist);
				nwais= 0;
				}
			}
					
		outFile->Open("w");
		for (i=0; i<n; i++) {
			info= (DLoGoInfo*) dirlist->At(i);
			if (info->fKind) {		
				const char * title;	
				StrNCpy( nameat, info->fName, sizeof(pathstore)-pathlen);
				if (info->fIsAsk) plus= "\t?";
				else if (info->fPlus) plus= "\t+"; 
				else plus= "";
				
				sprintf(buf, "+INFO: %c%s\t%s\t%s\t%d%s\n", 
									info->fKind, info->fTitle, pathstore, kLocalhost, port, plus);
				outFile->WriteLine( buf);
				
				sprintf(buf, "+ADMIN:\n Mod-Date: %s <%s>\n", "", info->fDate);
				outFile->WriteLine( buf);
				
				sprintf(buf, "+VIEWS:\n %s En_US: <%dk>\n", info->fView, info->fSize);
				outFile->WriteLine( buf);
				
				if (info->fIsAsk) {
					StrNCpy( namestore, pathstore, sizeof(namestore));
					StrNCat( namestore, ".ask", sizeof(namestore));
					DFile* askf= new DFile( namestore);
					if (askf->Exists()) {
						char* buf1;
						askf->Open("r");
						outFile->WriteLine( "+ASK:\n");
						buf[0]= ' ';
						buf1= buf+1;
						while (!askf->Eof()) {
							*buf1= 0;
							if (0 == askf->ReadLine( buf1, sizeof(buf)-1))
							 outFile->WriteLine( buf);
							}
						outFile->WriteLine( " \n");
						askf->Close();
						}
					delete askf;
					}
					

				if (info->fHasAbs) {
					StrNCpy( namestore, pathstore, sizeof(namestore));
					StrNCat( namestore, ".abstract", sizeof(namestore));
					DFile* askf= new DFile( namestore);
					if (askf->Exists()) {
						char* buf1;
						askf->Open("r");
						outFile->WriteLine( "+ABSTRACT:\n");
						buf[0]= ' ';
						buf1= buf+1;
						while (!askf->Eof()) {
							*buf1= 0;
							if (0 == askf->ReadLine( buf1, sizeof(buf)-1))
								outFile->WriteLine( buf);
							}
						outFile->WriteLine( " \n");
						askf->Close();
						}
					delete askf;
					}
					
				}
			delete info;
			}
		//outFile->Close();
		delete dirlist;
		return 0;
		}
	else
		return -1;
}



