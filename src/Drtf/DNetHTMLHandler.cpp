// DNetHTMLHandler.cpp
// d.g.gilbert


#include "Dvibrant.h"
#include "DFile.h"
#include "DRichProcess.h"
#include "DRichViewNu.h"
#include "DHTMLprocess.h"

#include "DGopher.h"
#include "DGoList.h"
#include "DGoPlus.h"
#include "DGoClasses.h"
#include "DGoInit.h"
#include "DURL.h"

#include "DNetHTMLHandler.h"


class DNetHTMLprocess : public DHTMLprocess
{
public:
	DGopherList * fGolist;
	
	DNetHTMLprocess( DFile* itsFile, DRichView* itsDoc, Nlm_MonitorPtr progress,
										DGopherList* itsLinks);

protected:
	virtual void	handleEndControl( short attr);  
};


static unsigned long gLinkcolor = (0<<16) + (170<<8) + 0;
																// red     green    blue



//class  DNetHTMLHandler


DNetHTMLHandler::DNetHTMLHandler( DRichView* itsDoc, DFile* savefile, DGopherList * itsList) :
	DHTMLHandler( itsDoc, savefile),
	fGolist(itsList)
{
}
	
 		

DRichprocess* DNetHTMLHandler::NewProcessor( DFile* itsFile, Nlm_MonitorPtr progress)
{
	fProcessor = new DNetHTMLprocess( itsFile, fDoc, progress, this->fGolist);
	return fProcessor;
}



// class DNetHTMLprocess

DNetHTMLprocess::DNetHTMLprocess( DFile* itsFile, DRichView* itsDoc, Nlm_MonitorPtr progress,
										DGopherList* itsLinks) :
	DHTMLprocess( itsFile,  itsDoc, progress),
	fGolist(itsLinks)
{ 
}




void DNetHTMLprocess::handleEndControl(short attr)
{
	// e.g, > end of any control

	if (fInForm && (fCurControl == htmlForm 
	 || fCurControl == htmlTextarea 
	 || fCurControl == htmlSelect 
	 || fCurControl == htmlInput)) {
		// if fCurControl == htmlForm) need to save fURLstore, as it is the url for the form ...
		// this comes at start of form block
		// need to process later EndControls as part of this form until
		// fInForm is set false.
		if (fCurControl == htmlForm) {
			MemFree( fFormURL);
			fFormURL= fURLstore;
			fURLstore= NULL;
			DHTMLprocess::handleEndControl( attr); // this handles image stuff !?
			}
		else {
			//this->handleLinkAttr( fCurControl); //!! attr == -1 here !
			// this is on the tricky side, but lets us trap text outside of the <control>
			// for use in the form label...
			fInControl= true;
			fQuote = (char)234; // kInFormText
			// DHTMLprocess::handleEndControl( attr); // this handles image stuff !?
			}
		return;
		}

// !!! MUST DISTINGUISH fURLstore for LINK (anchor) && fURLstore for picture !!
// picture can be inside of a link (& clickable for that link)
// !!! when an image is linked it is inside htmlAnchor
//     currently htmlImage inside Anchor wipes out anchor link info !
//     need to push anchor link &/or tie it in to image link !

	if (true && (  // if (fURLstore && ...)
	 fCurControl == htmlAnchor || 
	 fCurControl == htmlImage || 
	 fCurControl == htmlForm)) {
	  
		if (fCurControl == htmlForm) {
			MemFree(fURLstore);
			fURLstore= fFormURL;
			fFormURL= NULL;
			}
			
	  DGopher* go= NULL;
	  ulong urlsize= StringLen(fURLstore);	  
		if (true) { // if (urlsize)
			char* url = NULL;
			char* ref = NULL;
			Boolean haveIndexItem = false;
			
			ref = StringChr(fURLstore,'#');
			if (ref) {
						// ref is stored as gopher fQueryGiven field -- ??
				*ref++= 0;
				ref= StrDup(ref);
				urlsize= StrLen(fURLstore);
				}
				
			if ((!urlsize || fIsIndex && StringCmp(fURLstore,"?")==0) && fGolist->fParentMenu) {
				// refer to local/base url
				MemFree(fURLstore);
				fURLstore= StrDup( fGolist->fParentMenu->GetURL());
				urlsize= StrLen(fURLstore);
				haveIndexItem= true;
				}
				
			else if (DURL::IsURL( fURLstore, url, urlsize) == DGopher::kUnknownProt) {
				if (StrStr(fURLstore,"://")==NULL && fGolist->fParentMenu) {
#if 0
					char *cp, *ep, *path;
					
					if (*fURLstore == '"' || *fURLstore == '\'') {
						cp= fURLstore + 1;
						ep= StrChr( cp, *fURLstore);
						if (ep) *ep= 0;
						MemMove( fURLstore, cp, StrLen(cp)+1);
						}
					
					path= StrDup( fGolist->fParentMenu->GetURL());
					cp= StrRChr( path, '/');
					if (cp) { cp++; *cp= 0; }
					StrPrependCat( &fURLstore, path);
					MemFree( path);
#else
					const char * baseurl= fGolist->fParentMenu->GetURL();
					char * parturl= NULL;
					char *cp, *ep;

					parturl= DURL::GetParts(baseurl, 
							DURL::kPartProtocol +DURL::kPartHost +DURL::kPartPort);
							
						// !? need to drop " or ' from fURLstore for use with GetParts...
					if (*fURLstore == '"' || *fURLstore == '\'') {
						cp= fURLstore + 1;
						ep= StrChr( cp, *fURLstore);
						if (ep) *ep= 0;
						MemMove( fURLstore, cp, StrLen(cp)+1);
						}
						
						// if (fURLstore is relative to base path...) 
						// ?? is this bizarre mess really part of html syntax?
					if (*fURLstore != '/') { 
						char* pathname= DURL::GetParts(baseurl, DURL::kPartPath);
						char* path = pathname;
						cp= StrRChr( pathname, '/');
						if (cp) { cp++; *cp= 0; }
						else path= "";
						StrPrependCat( &fURLstore, path);
						MemFree(pathname);
						}
					else {
						long lenpart= StrLen(parturl)-1;
						if (parturl[lenpart] == '/') parturl[lenpart]= 0;
						}
					StrPrependCat(&fURLstore, parturl);
					MemFree(parturl);
#endif					
					} 
				urlsize= StrLen(fURLstore);
				}
				
			char gotype;
			if (fCurControl == htmlImage) gotype= kTypeImage;
			else if (haveIndexItem) gotype= kTypeQuery; //??!! this also for Forms?
			else gotype= 0;

		  if (fGolist && urlsize) 
		  	go= DGopherList::GopherFromURL( fURLstore, urlsize, fGolist, false, gotype); 
			if (go) {
			
				if (fCurControl == htmlImage) {
						// must we infer image type from path suffix ??
					go->AddView("image/gif"); // what about image/jpg & others ??
					if (fIsMap) go->fIsMap= true;
					}
					
				else if (fCurControl == htmlForm) {  
					char hasaskform= 3;  
					if (StrICmp(fMethodstore, "POST")==0) hasaskform= 5;
					go->SetPlusInfo( kGopherPlusYes, hasaskform, fGoplusStore);
					}
				// else go->AddView("text/html"); // not always !? gopher links, etc

					// screen out duplicate links
				short linkid= kEmptyIndex;
#if 1
				{
				short i, n= fGolist->GetSize();
				for (i=0; i<n; i++) {
					if ( go->Equals( fGolist->GopherAt(i)) ) {
						linkid= i;
						break;
						}
					}
				}
#else
					// this isn't catching dups !
				linkid= fGolist->GetEqualItemNo( (DObject*) go);	
#endif

				if (linkid == kEmptyIndex) {
					fGolist->InsertLast(go);
					linkid= fGolist->GetSize(); // note this is +1 of real list index
					}
				else linkid++; // +1 !
				if (fNamestore) go->StoreName(fNamestore);	// THIS was BAD !!?!?!
				if (ref) go->fQueryGiven= StrDup(ref);
				
				if (fCurControl == htmlImage) {
					go->fTransferType= kTransferBinary;
					fPicLink= linkid; //1 + fGolist->GetIdentityItemNo( (DObject*) go);	
					}
				else {
						
						// !? if (fCurControl == htmlForm)  //?? add some "Click here for Form" stuff?
						
					StoreStyle(fOldStyleRec.style, FALSE);  // required !
					fStyleRec.style.color= gLinkcolor;
					fStyleRec.style.linkid= linkid; //1 + fGolist->GetIdentityItemNo( (DObject*) go);	
					PutLitChar(' '); // ?? need for link style			
					StoreStyle(fStyleRec.style, TRUE); 
					fAnchorLink= linkid; 
#if 0 
						// we are missing links on case where other styles appear before </a>
						// try w/ linkid in style until popped by </a>
					fStyleRec.style.color= 0;
					fStyleRec.style.linkid= 0;				
#endif
#if 1
					if (fCurControl == htmlForm) {  //?? add some "Click here for Form" stuff?
						MemFree(fPushBufstore);
						fPushBufstore= StrDup("<P> Click HERE to fill out form</A><P>");
						fPushBuf= fPushBufstore;
						fPushSize= StrLen(fPushBuf);
						}
#endif
					}
				
				}
			MemFree( ref);
			}

		fURLstore= (char*) MemFree(fURLstore);
		}

	 DHTMLprocess::handleEndControl( attr); // this handles image stuff !?
	 	
}



