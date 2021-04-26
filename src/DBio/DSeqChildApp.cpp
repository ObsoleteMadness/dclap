// DSeqChildApp.cpp
// d.g.gilbert


#include "DSeqChildApp.h"

#include <ncbi.h>
#include <dgg.h>
#include <Dvibrant.h>
#include <DControl.h>
#include <DApplication.h>
#include <DTask.h>
#include <DMenu.h>
#include <DPanel.h>
#include <DUtil.h>
#include <DWindow.h>
#include <DChildApp.h>
#include <DChildDlogDoc.h>
#include <DRichMoreStyle.h>
#include <DRichViewNu.h>

#include "DSeqFile.h"
#include "DSeqList.h"
#include "DSeqDoc.h"



#if 0
 need 3 dialog methods here --
  * run app -- simple dlog w/ description, run button, help button, ?cancel button
  * edit child list -- list dlog w/ list of child apps, edit button, delete button, add button
  * edit child -- dlog w/ child app params -- pathname, desc, cmdline,
					 in/out files, min seqs, seqformat, other??

 when redoing childapp menu, mac can replace menu, others can't yet (thru vibrant)
 if (WIN_MAC) menu->Reset(); // to remove all menu items, then rebuild...
 
#endif


class DSeqChildApp : public DChildApp
{
public:
	//char  * fName, * fCmdline;
	//const char * fStdin, * fStdout, * fStderr;
	enum seqactions { kOpenSequence = 11, kOpenTree };
	char 	* fMenuname, * fDescription, * fSeqformat, *fHelpfile;
	char	* fForm, * fFormName;
	ulong		fFormLength;
	short		fMinseq, fMenucmd;
	DFile * fInfile;
	
	DSeqChildApp() :
		DChildApp(),
		fMenuname(NULL), fDescription(NULL), fSeqformat(NULL), fHelpfile(NULL),
		fInfile(NULL), fMinseq(0), fMenucmd(0), 
		fFormName(NULL), fForm(NULL), fFormLength(0)
		{
			fFiles= new DList();
		}
	virtual ~DSeqChildApp();
	virtual void LaunchDialog( DSeqList* aSeqList); 
	virtual void LaunchAfterDialog( DSeqList* aSeqList); 
	virtual void FileAction( DChildFile* aFile);
	virtual void CollectOptions( DList* optList);
	virtual char* CollectFilePath(char* value, Boolean checkOutType, short& action);
	virtual char* SubstituteVars(char* data, DList* optList);
};





// class DLaunchDialog

class DLaunchDialog : public DWindow {
	enum { cHELP = 3222 };
	char *fHelpfile;
	DSeqChildApp* fChild;
public: 
	DLaunchDialog( DSeqChildApp* child);
	virtual void Open();			
	virtual Nlm_Boolean IsMyAction(DTaskMaster* action); 
};
		
		
		
DLaunchDialog::DLaunchDialog( DSeqChildApp* child) :
	DWindow( 0, gApplication, fixed, -10, -10, -50, -20, "Launch app"),
	fChild(child)
	{
	}

void DLaunchDialog::Open()
{
	char buf[128];
	
	if (fChild->fDescription) { 
		DCluster* clu= new DCluster( 0, this, 0, 0, false, "Description");   //0,0 == wid,hi
		new DNotePanel(0, clu, fChild->fDescription, 200, 50);	
		this->NextSubviewBelowLeft();
		}
	sprintf( buf, "Launch %s?",fChild->fMenuname);
	new DPrompt(0, this, buf, 0, 0, Nlm_systemFont);	 		
	this->NextSubviewBelowLeft();
			
	this->AddOkayCancelButtons(cOKAY,"Launch",cCANC,"Cancel");
	if (fChild->fHelpfile) {
		fHelpfile= fChild->fHelpfile;
		this->NextSubviewToRight();
		new DButton( cHELP, this, "Help");
		}
	else
		fHelpfile= NULL;
		
	DWindow::Open();
}

Nlm_Boolean DLaunchDialog::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {
		case cHELP:
			gApplication->OpenDocument(fHelpfile);
			return true;
		default:
			return DWindow::IsMyAction(action);	
		}
}







DSeqChildApp::~DSeqChildApp()
{
	MemFree( fMenuname);
	MemFree( fDescription);
	MemFree( fSeqformat);
	MemFree( fHelpfile);
	MemFree( fFormName);
	MemFree( fForm);
}


enum { kDistanceMat = -89, kSimilarityMat, kBiotree };

void DSeqChildApp::LaunchDialog( DSeqList* aSeqList) 
{
	DLaunchDialog* win= NULL;
	Boolean okay= true;
	
#ifdef WIN_MAC
		// This works for MAC but fails for MSWIN && UNIX!
	win= new DLaunchDialog(this);
	okay= win->PoseModally();
	delete win;  // for MSWin, MUST delete win BEFORE LAUNCH !! -- now mswin fails here
#else
#if defined( WIN_MOTIF ) || defined( WIN_MSWIN)
	// bad problems in Unix/XWin w/ this dialog mangling child launch process...
	okay= true;
#else		
		// this works for MSWIN but fails on MAC !
	DLaunchDialog ldlog(this);
	okay= ldlog.PoseModally();
#endif
#endif

	if (okay) LaunchAfterDialog( aSeqList);
}


void DSeqChildApp::LaunchAfterDialog( DSeqList* aSeqList) 
{
	if (fMinseq>0 && (!aSeqList || aSeqList->GetSize()<fMinseq)) {
		Message(MSG_OK, "%s needs a selection of at least %d sequences", 
							fMenuname, fMinseq);
		return;
		}
			// write aSeqList to this->inputfile...
	DFile* aFile = this->fInfile;
	if (aFile) {
		char  * data = NULL;
		short  outformat= 0;
		
		aFile->Open("w");
		if (StrNICmp(fSeqformat, "distance", 8)==0) outformat= kDistanceMat;
		else if (StrNICmp(fSeqformat, "similar", 7)==0) outformat= kSimilarityMat;
		else if (StrNICmp(fSeqformat, "biotree", 7)==0) outformat= kBiotree;
		else outformat= DSeqFile::FormatFromName(fSeqformat);
		if (outformat == DSeqFile::kUnknown) 
			outformat= DSeqFile::kPearson; //?? or kPearson ?
			
		if (aSeqList) aSeqList->ClearSelections(); //! make sure we write all of seq from this call !?
		switch (outformat) {
		
			case kSimilarityMat:
				if (!aSeqList) return;
				data= aSeqList->Similarities(DSeqDoc::fgDistCor);  
				break;
				
			case kDistanceMat:
				if (!aSeqList) return;
				data= aSeqList->Distances(DSeqDoc::fgDistCor); 
				break;
				
			case kBiotree:
				{
						// data is text in front richtext window !?
				DRichTextDoc* rtdoc = (DRichTextDoc*) gWindowManager->CurrentWindow();
				rtdoc->fRichView->Save(aFile);
				//data= rtdoc->fRichView->GetSelectedText();
				}
				break;
				
			default:
				if (!aSeqList) return;
				DSeqFile::DontSaveMasks();
				aSeqList->DoWrite( aFile, outformat);
				DSeqFile::DoSaveMasks();
				break;
			}
			
		if (data) {
			ulong count= StrLen(data);
			aFile->WriteData( data, count);
			MemFree(data);
			}
		aFile->Close();
		aFile->SetMode("r");
		}
	
	if (!this->Launch())
		Message(MSG_OK, "Failed to launch %s with '%s'", fName, fCmdline);
	  
}



void DSeqChildApp::FileAction( DChildFile* aFile)
{
	switch (aFile->fAction) {
	  case kOpenSequence:
			DSeqDoc::fgTestSeqFile= true; // ???
			gApplication->OpenDocument( aFile);
			DSeqDoc::fgTestSeqFile= false; 
			if (aFile->fDelete) {
				DWindow* win= gWindowManager->CurrentWindow();
				if (win && win->fSaveHandler) win->fSaveHandler->Dirty();
				}
			break;
		
	  case kOpenTree:
	  	// fix later...
		default:
			DChildApp::FileAction( aFile);
			break;
		}
}



char* DSeqChildApp::CollectFilePath(char* value, Boolean checkOutType, short& action)
{
	if (checkOutType) {
		char* ftype= StrChr(value,'\t'); // look for mime/type after name
		if (!ftype) ftype= StrChr(value,' '); // !? can we also scan for spaces ??
		if (ftype) {
			*ftype++= 0;
			while (*ftype && *ftype <= ' ') ftype++;
			if (StrNICmp(ftype,"biosequence",11) == 0 
			 || StrNICmp(ftype,"sequence",8) == 0)
					action= DSeqChildApp::kOpenSequence;
			else if (StrNICmp(ftype,"biotree",7) == 0 
			 || StrNICmp(ftype,"tree",4) == 0)
					action= DSeqChildApp::kOpenTree;
			}
		}
	char* cp= value; 
	while (isspace(*cp) || *cp == '$') cp++;
	return gApplication->ConvertStdFilePath(cp);
}				



char* DSeqChildApp::SubstituteVars(char* data, DList* optList)
{
	char * cp, * item, * value, * newpath;
	short action, kind;
	DControlStyle* cont;
	
	if (!optList) return data;
	short i, n= optList->GetSize();
	
	for (i=0; i<n; i++) {
			// pass 1, collect non-hidden control vars == user opts
			
		cont= (DControlStyle*) optList->At(i);
		item = (char*)cont->Varname();
		value= (char*)cont->Value();
		kind= cont->fKind;

		if (item && kind != DControlStyle::kHidden) { 
			Boolean istest;
			char * ep, * dp, * itemp, * iteme;
			char * trueval, * falseval;
			char * newvalue = StrDup("");
			long itemsize= StrLen(item);
			dp= cp = data;
			do {
				istest= false;
				trueval= falseval= NULL;
				ep= StrChr( dp, '$');
				if (!ep)
					StrExtendCat( &newvalue, cp);
				else {
					if (ep[1] == '{' /*}*/) { 
						itemp= ep+2; 
						iteme= itemp; 
						while (*iteme && *iteme != /*{*/'}') iteme++;
						if (iteme - itemp != itemsize) itemp= NULL; 
						if (*iteme == /*{*/'}') iteme++; 
						}
					else if (ep[1] == '?') {
						// $?var1:valtrue:valfalse  -> ($var)?valtrue:valfalse
						itemp= ep+2;
						iteme= itemp + itemsize;
						istest= true;
						}
					else {
						itemp= ep+1;
						iteme= itemp + itemsize;
						}
						
					if (itemp && StrNICmp( item, itemp, itemsize) == 0) {
						if (istest) {
							trueval= iteme;
							if (*trueval == ':') {
								*trueval++= ' ';
								}
							falseval= StrChr(trueval,':');
							if (falseval) {
								*falseval++ = ' ';
								iteme= StrChr(falseval,':');
								if (iteme) *iteme++ = ' ';
								else iteme= itemp + itemsize;
								}
							}
						*ep++ = 0;
						StrExtendCat( &newvalue, cp);  // add on string before '$'
						if (value) {
							if (trueval) iteme= trueval; //StrExtendCat( &newvalue, trueval);
							else StrExtendCat( &newvalue, value);
							}
						else {
							if (falseval) iteme= falseval;
							}
						// else we drop the variable marker !
						cp= dp= iteme;
						}
					else
						dp= ep+1;
						
					}
			} while (ep);
			MemFree( data);
			data= newvalue;
			}
		}
	return data;
}
				
				
void DSeqChildApp::CollectOptions( DList* optList)
{
	char * cp, * item, * value, * newpath;
	short action, kind;
	DControlStyle* cont;
	char * execapp = NULL;
	DChildFile 		* cfile;
	Boolean 	noStderr= true, noStdout= true, noStdin= true;
	
	if (!optList) return;
	short i, n= optList->GetSize();
	
	for (i=0; i<n; i++) {
			// pass 0, collect command line !
			
		cont= (DControlStyle*) optList->At(i);
		item = (char*)cont->Varname();
		value= (char*)cont->Value();
		if (value) while (isspace(*value)) value++;
		kind= cont->fKind;
		
		if (item && value && kind == DControlStyle::kHidden) { 
			if (StrICmp(item,"localexec")==0 || StrICmp(item,"exec")==0) {
				execapp= StrDup(value);
				}
			else if (StrICmp(item,"get")==0) {
				execapp= StrDup(value);
				}
			else if (StrICmp(item,"post")==0) {
				execapp= StrDup(value);
				}
			}
		}
	if (!execapp) return; // ??
	
	execapp= SubstituteVars( execapp, optList);
		
		// cut app + options into 2 parts
	cp= execapp;
	while (*cp && !isspace(*cp)) cp++;
	if (*cp) *cp++= 0;
	MemFree( fName);
	fName= CollectFilePath( execapp, false, action);
	
		// substitute full file paths in cmdline 
	{
		while (*cp && !isspace(*cp)) cp++;
		char * ep;
		char * newvalue = StrDup("");
		do {
			ep= StrChr( cp, '$');
			if (ep) {
				*ep++ = 0;
				StrExtendCat( &newvalue, cp);  // add on string before '$'
					// find & substitute any pathname
				char savec, *np= ep; 
				while (isgraph(*ep)) ep++; 
				savec= *ep; *ep= 0; // skip past path:filename
				newpath= gApplication->ConvertStdFilePath(np);
				*ep= savec;
				if (newpath && *newpath) {
					char* dquote= NULL;
					if (StrChr( newpath, ' ')) {
						dquote= "'";
						// ! must find start of parameter, e.g. [/infile=$temp:data]
						//  where newpath is for $temp:data, requires we start quote 
						// at ['/infile=new path:data']
						char* pp= newvalue + StrLen(newvalue);
						while (pp > newvalue && isspace(*pp)) pp--;
						while (pp > newvalue && !isspace(*pp)) pp--;
						if (isspace(*pp)) {
							*pp++= 0;
							char* saveparm= StrDup(pp); // extend(newval) will wipe out pp !
							StrExtendCat( &newvalue, dquote);
							StrExtendCat( &newvalue, saveparm);
							MemFree( saveparm);
							}
						else
							StrExtendCat( &newvalue, dquote);
						}
					StrExtendCat( &newvalue, newpath);
					if (dquote) StrExtendCat( &newvalue, dquote);
					MemFree( newpath);
					}
				else {
					// this is another variable not replaced above... 
					// it is deleted from fCmdline
					}
					
				cp= ep;
				}
			else
				StrExtendCat( &newvalue, cp);
		} while (ep);
		MemFree( fCmdline);
		fCmdline= newvalue;
	}
	MemFree( execapp);

	
	for (i=0; i<n; i++) {
			// pass 2, collect only hidden control vars == DSeqChildApp vars
			
		cont= (DControlStyle*) optList->At(i);
		item = (char*)cont->Varname();
		value= (char*)cont->Value();
		kind= cont->fKind;
		
		if (item && value && kind == DControlStyle::kHidden) { 
				// !? ignore all controls w/ NULL value !?

			if (StrICmp(item,"seqformat")==0) 	{
				MemFree(fSeqformat);
				fSeqformat= StrDup(value);
				}
				
#if 0		
			else if (StrICmp(item,"path")==0) {
				MemFree(fName);
				fName= CollectFilePath(value, false, action);
				}
#endif

			else if (StrICmp(item,"stdcmdin")==0) {
				noStdin= false;
				newpath= CollectFilePath( "$temp:stdcmdin", false, action);
				cfile= new DChildFile( newpath, DChildFile::kStdin, DChildFile::kDeleteWhenDone,
																DChildFile::kNoAction, "r", "TEXT", "Spup");

	 			// stuff value into file...
				value= SubstituteVars( StrDup(value), optList);
				ulong count= StrLen(value);
				cfile->Open("w");
				cfile->WriteData( value, count);
				cfile->Close();
				MemFree( value);
				//fInfile= cfile;  
				AddFile(cfile);
				MemFree( newpath);
				}
													
			else if (StrICmp(item,"minseq")==0) {
				fMinseq= atoi(value);
				}
											
			else if (StrNICmp(item,"infile",6)==0) {
				newpath= CollectFilePath( value, false, action);
				cfile= new DChildFile( newpath, DChildFile::kInput, DChildFile::kDeleteWhenDone,
																DChildFile::kNoAction, "r", "TEXT", "Spup");
				AddFile(cfile);
				fInfile= cfile; 
				MemFree( newpath);
				}

			else if (StrNICmp(item,"stdin",5)==0) {
				noStdin= false;
				newpath= CollectFilePath( value, false, action);
				cfile= new DChildFile( newpath, DChildFile::kStdin, DChildFile::kDeleteWhenDone,
																DChildFile::kNoAction, "r", "TEXT", "Spup");
				fInfile= cfile;  
				AddFile(cfile);
				MemFree( newpath);
				}

			else if (StrNICmp(item,"stderr",6)==0) {
				noStderr= false;
				action = DChildFile::kOpenText;
				newpath= CollectFilePath( value, true, action);
				cfile= new DChildFile( newpath, DChildFile::kStderr, DChildFile::kDeleteWhenDone,
																action, "r", "TEXT", "Spup");
				AddFile(cfile);
				MemFree( newpath);
				}

			else if (StrNICmp(item,"stdout",6)==0) {
				noStdout= false;
				action = DChildFile::kOpenText;
				newpath= CollectFilePath( value, true, action);
				cfile= new DChildFile( newpath, DChildFile::kStdout, DChildFile::kDeleteWhenDone,
																action, "r", "TEXT", "Spup");
				AddFile(cfile);
				MemFree( newpath);
				}
					
			else if (StrNICmp(item,"outfile",7)==0) {
				action = DChildFile::kOpenText;
				newpath= CollectFilePath( value, true, action);
				cfile= new DChildFile( newpath, DChildFile::kOutput, DChildFile::kDeleteWhenDone,
																action, "r", "TEXT", "Spup");
				AddFile(cfile);
				MemFree( newpath);
				}
								
			}	
		}
	
	if (noStderr)	AddFile(DChildFile::kStderr, NULL);
	if (noStdout)	AddFile(DChildFile::kStdout, NULL);
}
	
	


// class DSeqApps : public DObject


// static
void DSeqApps::CallChildApp(short menucmd, DSeqList* aSeqList) 
{
	DChildDlogDoc* doc = NULL;
	DSeqChildApp * child = NULL;
	short i, n = DChildAppManager::gChildList->GetSize();
	for (i= 0; i<n; i++) {
		child= (DSeqChildApp*) DChildAppManager::gChildList->At(i);
		if (child->fMenucmd == menucmd) {

			if (child->fForm || child->fFormName) {
				if (!child->fForm) {
					//doc->Open(child->fFormName);
					char* fbuf;
					ulong flen;
					DFile ff(child->fFormName);
					ff.Open();
					ff.GetDataLength(flen);
					if (!flen) return;
					fbuf= (char*) MemNew(flen+1);
					ff.ReadData(fbuf, flen);
					ff.Close();
					fbuf[flen]= 0;
					child->fFormLength= flen;
					child->fForm= fbuf;
					}
					//?? child->LaunchAfterForm( aSeqList);
				doc= new DChildDlogDoc( 0, false, gTextFont);
				doc->OpenText(child->fForm, child->fFormLength);
				Boolean okay= doc->PoseModally();
				
					// collect user options from doc controls !
				if (okay) child->CollectOptions(doc->GetControls());
				delete doc;   
				
				if (okay) child->LaunchAfterDialog( aSeqList);
				}
			else 
				child->LaunchDialog( aSeqList); // old method
				
			return;
			}
		}
	Message(MSG_OK, "Child app not found");
}



// static
void DSeqApps::SetUpMenu(short menuId, DMenu*& aMenu) 
{
	char  * kAppSection = "apps";
	long 	atsection;
	ulong sectlen = 0;
	char	* value, * item, * appsection, * appvalues, * sections,
				* newpath,  * cp;
	short menuitem= kChildMenuBaseID;
	DSeqChildApp 	* child;
	DChildFile 		* cfile;
	Boolean 	noStderr= true, noStdout= true, noStdin= true;
	
	
	//DWindow::SetUpMenu(menuId, aMenu);
	if (!aMenu) aMenu = gApplication->NewMenu( menuId, "ChildApps");

	sections= gApplication->GetPrefSection( kAppSection, sectlen);
	atsection= 0;
	while (atsection<sectlen) {
		appsection= sections + atsection;
		if (appsection) {
			long 	atvalue;
			ulong applen= 0;
			
			child= new DSeqChildApp();
			// child is stored in DChildAppManager::gChildList
			
			value= gApplication->GetPref( appsection, kAppSection);
			if (!value) value= StrDup( appsection);
			aMenu->AddItem( menuitem, value);
			child->fMenuname= value;
			child->fMenucmd= menuitem;
			menuitem++;
	
			child->fReusable= true; // don't delete after one use...
		
			appvalues= gApplication->GetPrefSection( appsection, applen);
			atvalue= 0;
			while (atvalue<applen) {
				item= appvalues + atvalue;
				value= gApplication->GetPref( item, appsection);
				
				if (!value)
					;
				else if (StrICmp(item,"desc")==0)  
					child->fDescription= value;
				else if (StrICmp(item,"path")==0) {
					cp= value; while (isspace(*cp) || *cp == '$') cp++;
					child->fName= gApplication->ConvertStdFilePath(cp);
					MemFree( value);
					}
				else if (StrICmp(item,"form")==0) {
					cp= value; while (isspace(*cp) || *cp == '$') cp++;
					child->fFormName= gApplication->ConvertStdFilePath(cp);
					MemFree( value);
					}
				else if (StrICmp(item,"help")==0) {
					cp= value; while (isspace(*cp) || *cp == '$') cp++;
					child->fHelpfile= gApplication->ConvertStdFilePath(cp);
					MemFree( value);
					}
					
				else if (StrICmp(item,"seqformat")==0) 	
					child->fSeqformat= value;
				else if (StrICmp(item,"minseq")==0) {
					child->fMinseq= atoi(value);
					MemFree( value);
					}
			
				else if (StrICmp(item,"cmd")==0)	{
					// convert variables of form $word[delimiter] ...
					// especially file path variables
					char * ep;
					char * newvalue = StrDup("");
					cp = value;
					do {
						ep= StrChr( cp, '$');
						if (ep) {
							*ep++ = 0;
							StrExtendCat( &newvalue, cp);  // add on string before '$'
							
								// find & substitute any pathname
							newpath= gApplication->ConvertStdFilePath(ep);
								// newpath is all of remaining string ... cut pathname from remainder of str
							//char* np= newpath;
							//while (isgraph(*np)) np++;  *np= 0;
							StrExtendCat( &newvalue, newpath);
							MemFree( newpath);
							
								// locate next part of str to parse -- cue on whitespace
							while (isgraph(*ep)) ep++;  
							cp= ep;
							}
						else
							StrExtendCat( &newvalue, cp);
					} while (ep);
					MemFree( value);
					value= newvalue;
					child->fCmdline= value;
					}
					
					// need also to handle dialog control items !!, e.g., dlog=control spec...
					// and need to handle dlog control substitutions in cmdline...
							
				else if (StrNICmp(item,"infile",6)==0) {
					cp= value; while (isspace(*cp) || *cp == '$') cp++;
					newpath= gApplication->ConvertStdFilePath(cp);
					cfile= new DChildFile( newpath, DChildFile::kInput, DChildFile::kDeleteWhenDone,
																	DChildFile::kNoAction, "r", "TEXT", "Spup");
					child->AddFile(cfile);
					child->fInfile= cfile;  
					MemFree( newpath);
					MemFree( value);
					}

				else if (StrNICmp(item,"stdin",5)==0) {
					noStdin= false;
					cp= value; while (isspace(*cp) || *cp == '$') cp++;
					newpath= gApplication->ConvertStdFilePath(cp);
					cfile= new DChildFile( newpath, DChildFile::kStdin, DChildFile::kDontDelete,
																	DChildFile::kNoAction, "r", "TEXT", "Spup");
					child->AddFile(cfile);
					child->fInfile= cfile;  
					MemFree( newpath);
					MemFree( value);
					}

				else if (StrNICmp(item,"stderr",6)==0) {
					noStderr= false;
					short action = DChildFile::kOpenText;
					char* ftype= StrChr(value,'\t'); // look for mime/type after name
					if (!ftype) ftype= StrChr(value,' '); // !? can we also scan for spaces ??
					if (ftype) {
						*ftype++= 0;
						while (*ftype && *ftype <= ' ') ftype++;
						if (StrNICmp(ftype,"biosequence",11) == 0 
						 || StrNICmp(ftype,"sequence",8) == 0)
								action= DSeqChildApp::kOpenSequence;
						else if (StrNICmp(ftype,"biotree",7) == 0 
						 || StrNICmp(ftype,"tree",4) == 0)
								action= DSeqChildApp::kOpenTree;
						}
					cp= value; while (isspace(*cp) || *cp == '$') cp++;
					newpath= gApplication->ConvertStdFilePath(cp);
					cfile= new DChildFile( newpath, DChildFile::kStderr, DChildFile::kDontDelete,
																	action, "r", "TEXT", "Spup");
					child->AddFile(cfile);
					MemFree( newpath);
					MemFree( value);
					}

				else if (StrNICmp(item,"stdout",6)==0) {
					noStdout= false;
					short action = DChildFile::kOpenText;
					char* ftype= StrChr(value,'\t'); // look for mime/type after name
					if (!ftype) ftype= StrChr(value,' '); // !? can we also scan for spaces ??
					if (ftype) {
						*ftype++= 0;
						while (*ftype && *ftype <= ' ') ftype++;
						if (StrNICmp(ftype,"biosequence",11) == 0 
						 || StrNICmp(ftype,"sequence",8) == 0)
								action= DSeqChildApp::kOpenSequence;
						else if (StrNICmp(ftype,"biotree",7) == 0 
						 || StrNICmp(ftype,"tree",4) == 0)
								action= DSeqChildApp::kOpenTree;
						}
					cp= value; while (isspace(*cp) || *cp == '$') cp++;
					newpath= gApplication->ConvertStdFilePath(cp);
					cfile= new DChildFile( newpath, DChildFile::kStdout, DChildFile::kDontDelete,
																	action, "r", "TEXT", "Spup");
					child->AddFile(cfile);
					MemFree( newpath);
					MemFree( value);
					}
					
				else if (StrNICmp(item,"outfile",7)==0) {
					// combine stdout & stderr w/ this code...
					short action = DChildFile::kOpenText;
					char* ftype= StrChr(value,'\t'); // look for mime/type after name
					if (!ftype) ftype= StrChr(value,' '); // !? can we also scan for spaces ??
					if (ftype) {
						*ftype++= 0;
						while (*ftype && *ftype <= ' ') ftype++;
						if (StrNICmp(ftype,"biosequence",11) == 0 
						 || StrNICmp(ftype,"sequence",8) == 0)
								action= DSeqChildApp::kOpenSequence;
						else if (StrNICmp(ftype,"biotree",7) == 0 
						 || StrNICmp(ftype,"tree",4) == 0)
								action= DSeqChildApp::kOpenTree;
						}
					cp= value; while (isspace(*cp) || *cp == '$') cp++;
					newpath= gApplication->ConvertStdFilePath(cp);
					cfile= new DChildFile( newpath, DChildFile::kOutput, DChildFile::kDontDelete,
																	action, "r", "TEXT", "Spup");
					child->AddFile(cfile);
					MemFree( newpath);
					MemFree( value);
					}
					
			
				atvalue += StrLen(item)+1;
				}
			
			if (noStderr) child->AddFile(DChildFile::kStderr, NULL);
			if (noStdout) child->AddFile(DChildFile::kStdout, NULL);
			
			MemFree( appvalues);
			}
		atsection += StrLen(appsection)+1;
		}
	MemFree( sections);
}




#if 0

			/////// prefs file example ///////
[apps]
;localvar=menu title
clustal=Multiple align...
cap=Contig assembly...
dnaml=Phylip DNAML...

[clustal]
;file path to application 
path=apps:clustalw
help=apps:clustalw_help
;description for dialog
desc=A multiple sequence alignment application
;dialog controls (HTML forms format?)

;command line switches
cmd=/infile=temp:clustal.inseq /outfile=clustal.outseq /output=gcg /align

;sequence selection to input file 
infile=clustal.inseq
;sequence format to send to app
seqformat=nbrf
;minimum number of selected sequences to activate this command
minseq=2
;return data files, with doc/type
outfile1=clustal.outseq	text/plain
outfile2=clustal.dnd	text/plain


#endif
