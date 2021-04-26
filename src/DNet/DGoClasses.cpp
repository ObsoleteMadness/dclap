// DGopherClasses.cp

#include <ncbi.h>
#include <dgg.h>
#include <Dvibrant.h>

#include "DTCP.h"
#include "DGopher.h"
#include "DGoList.h"
#include "DGoInit.h"
#include "DGoPlus.h"
#include "DGoClasses.h"

#include <DWindow.h>
#include <DControl.h>
#include <DDialogText.h>
#include <DSendMailDialog.h>
#include <DApplication.h>
#include <DUtil.h>


static const	long kReadMaxbuf1 = 1 * 1024;
static const	long kMaxMemory	 = 20 * 1024; // sure wish we could measure app free memory !!

const long	kGopherFileType	 =
#ifdef WIN_MSWIN
1234;
#else
'IGo4'; // dup of SeqGopher.h const
#endif


//typedef	AppFile	*AppFilePtr;

// DFolderGopher -----------------------------


void DFolderGopher::Initialize()
{
	fMacType= kGopherFileType; //TEXT  
	fMacSire= kGopherFileType; //EDIT
	fTransferType= kTransferBinary; //kTransferText; //text/lines or binary or none ?
	fSaveToDisk= false;
	fLaunch= false;
	fCommand= cNewGopherFolder;
}



DFolderGopher::DFolderGopher() 
{
	Initialize(); // my mistake -- 
		// the base class Initialize() is called by DGopher() 
		// this subclassed Initialize() must be called by this subclass constructor
		// I should have known that and written this differently (IGopher() and IFolderGopher() ??)
}

DFolderGopher::DFolderGopher( DFolderGopher* aGopher): DGopher(aGopher)
{
	//Initialize(); -- no DGopher(aGopher) will set fields way we want (i hope)
}

DFolderGopher::DFolderGopher( char ctype, const char* link):
	DGopher( ctype, link)
{
	Initialize();
}


DObject* DFolderGopher::Clone()
{
	DFolderGopher* aGopher= new DFolderGopher();
	aGopher->CopyGopher( this);
	return aGopher;
	//return new DFolderGopher(this); << BAD, mangles this
}

void DFolderGopher::DrawIcon( Nlm_RecT& area, short size)  
{
	gGopherIconID= kFolderIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* DFolderGopher::GetKindName() 
{ 
	return "Folder";
}


void DFolderGopher::OpenQuery()
{
	if (fQueryPlus != NULL)  ; 			// if wrapper has set this (for +ASK reply) leave it
	else if (fIsPlus != kGopherPlusYes) fQueryPlus= NULL;
	else {
		// if sending viewform, can't send fQueryPlus !
		if (fViewchoice) fQueryPlus= NULL; 	// viewchoice conflicts w/ queryplus !!
		else if (fShortFolder) fQueryPlus= "\t+"; // gopher+, call for short data
		else fQueryPlus= "\t$"; 									// gopher+, call for all needed data
		}
		
	DGopher::OpenQuery();		
}

#if GOOBSOLETE
short DFolderGopher::GopherIt() // override 
{
	ReadAll();
	if (fInfo && fInfoSize>10) 
		return cNewGopherFolder;
	else  
		return cNoCommand;
}

#endif



// DTextGopher -----------------------------
 
#ifdef WIN_MSWIN
long	DTextGopher::kMacType = 1234;
long	DTextGopher::kMacSire = 4321;
#else
long	DTextGopher::kMacType = 'TEXT';
long	DTextGopher::kMacSire = 'EDIT';
#endif
char * DTextGopher::kSuffix = ".text";

void DTextGopher::InitSignature()	 
{
	if (gGopherMap) gGopherMap->GetPreferedFiletype("text/plain", kMacType, kMacSire, kSuffix); 	
}

void DTextGopher::Initialize()	// override 
{
	//? fType= kTypeFile; 
	fMacType= kMacType; 
	fMacSire= kMacSire; 
	fTransferType= kTransferText; 
	fSaveToDisk= false;
	fLaunch= false;
	fCommand= cNewGopherText;
}

DTextGopher::DTextGopher(): DGopher()
{
	Initialize();
}

DTextGopher::DTextGopher( DTextGopher* aGopher): DGopher(aGopher)
{
	//Initialize();
}

DTextGopher::DTextGopher( char ctype, const char* link):
	DGopher( ctype, link)
{
	Initialize();
}


DObject* DTextGopher::Clone()
{
	DTextGopher* aGopher= new DTextGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}

void DTextGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kTextIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* DTextGopher::GetKindName() 
{ 
	return "Text document";
}


void DTextGopher::OpenQuery()
{
	if (fQueryPlus != NULL)  ; 			// if wrapper has set this (for +ASK reply) leave it
	else if (fIsPlus != kGopherPlusYes) fQueryPlus= NULL;
	else fQueryPlus= "\t+"; // gopher+, this form forces server to send data size first

	DGopher::OpenQuery();		
	if (fTalker) fTalker->NullTerm(true); //!?
	if (fTransferType == kTransferText 
			&& fInfo && fInfoSize>0 && fInfo[fInfoSize] == kLF) 
				fInfo[fInfoSize--]= 0;  
}



#if GOOBSOLETE
short DTextGopher::GopherIt()
{
	short	command= cNoCommand;
	Boolean tooBig= false;
	Boolean FileIsMade= false;
	long chunksize = kReadMaxbuf1; //Min( FreeMem()/2, kReadMaxbuf1);
	long maxmemtext = kMaxMemory; //FreeMem();
	//maxmemtext = maxmemtext - (maxmemtext / 3);

	OpenQuery();		
	
	if (fBytesExpected > 0) tooBig= (fBytesExpected > maxmemtext);

#if 1
	if (fInfoHandler)
	 while (!EndOfMessage()) {
			ReadAChunk( maxmemtext);
			//(fInfoHandler)(this);
			fInfoHandler->Process();
			}
	else
#endif	
	
	while (!EndOfMessage()) {
		if (tooBig || fSaveToDisk) {
			fThreadState= kThreadReadingToDisk;
			if (tooBig && fTalker) fTalker->ShowMessage("More text than app can display.");
			FileIsMade= ReadToFile( chunksize);
			if (FileIsMade) command= cOpenGopherText;
			else command= cNewGopherText;
			if (fTalker) fTalker->SetEndOfMessage(true);
			}
		else {
			ReadAChunk( maxmemtext);
			//tooBig= (!EndOfMessage());
			if (fTalker) tooBig = (fTalker->NewBytesReceived() == -1);
			command= cNewGopherText;
			}
		} 
	
	FinishRead();
	if (fLaunch) { (void) this->LaunchFile(); command= cNoCommand; }
	CloseQuery();
	
	return command;
}
#endif // GOOBSOLETE










//	DIndexGopher	-----------------

			
const short	kGopherSearchID	= 2204;	// dialog window for search query 
																		// do we want this public or private ?

class DIndexDialog : public DWindow
{
public:
	static char *gLastQuery;
	DEditText* fQuery;
	
	DIndexDialog( long id, DGopher* itsGoph) :
		DWindow( id, gApplication, fixed, -10, -10, -50, -20, "Gopher Query", kDontFreeOnClose)
	{
		char *kSpacers = "                            "; // 28 spc
		char	buf[255];
		sprintf(buf,"%s?", (char*)itsGoph->GetName());
		DPrompt* pr= new DPrompt(0, this, buf, 0, 0, Nlm_programFont);	 		
		this->NextSubviewBelowLeft();
		fQuery= new DEditText(0, this, kSpacers, 28); // save last query default???
		fQuery->SetTitle(gLastQuery);
		this->SetEditText(fQuery);
		this->AddOkayCancelButtons(cOKAY,"Send");
	}
			
	char* Result()
	{
		if (fOkay) {
			//return fQuery->GetText();
			long len = fQuery->TextLength();
			if (!len) return NULL;
			char *txt = (char*) MemNew(len+2);
			*txt= '\t'; // precede word w/ tab for gopher
			fQuery->GetTitle( txt+1, len+1);
			if (gLastQuery) MemFree(gLastQuery);
			gLastQuery= StrDup( txt+1);
			return txt;
			}
		else return NULL;
	}
	
};

char* DIndexDialog::gLastQuery = NULL;


Boolean DIndexGopher::PoseQuestion(CharPtr& query)
{
	query= NULL; //?? we create this storage

	DIndexDialog* win= new DIndexDialog(0, this);
	if ( win->PoseModally() ) ;
	query= win->Result();
	delete win;  
	return (query != NULL);
}
		
void DIndexGopher::InfoTask() 
{	
	fThreadState= kThreadDoneReading; 
}


void DIndexGopher::CloseQuery()
{
	DFolderGopher::CloseQuery();
	if (fQuery) fQuery= (char*) MemFree( fQuery);

#if 0
	char* kBombBugPatch = "i(Quick bug patch for obscure bomb)\t\t\t0"CRLF;	

	
	DGopher* ago= new DGopher();
	ago->SetLink( *fInfo, fInfo);
	char link1[512];
	sprintf( link1, "%c%s\t%s\t%s\t%d\t%c",
		ago->fType, (char*) ago->GetName(), (char*) ago->GetPath(), 
		(char*) ago->GetHost(), ago->fPort, ' ');
	delete ago;
	
	char *next= fInfo;
	next= StrChr(next, kLF);
	if (next) next++;
	
	char *tmp= StrDup(kBombBugPatch);

	StrExtendCat( &tmp, link1);
	StrExtendCat( &tmp, next);
	//StrExtendCat( &tmp, fInfo);
	MemFree(fInfo);
	fInfo= tmp;
	fInfoSize= StrLen(fInfo);

#endif

}

void DIndexGopher::OpenQuery()
{
	char*		query;				//?? need to save user query for doc word highlighting 
	gCursor->arrow();
	
	if (fQueryGiven) {
		fQuery= (char*) MemNew( 2 + StrLen(fQueryGiven));
		fQuery[0]= '\t';
		StrCpy(fQuery+1,fQueryGiven);
		//DFolderGopher::OpenQuery(); << no, sends \t+ which leads client to ask for !info, which server can't produce
		DGopher::OpenQuery();
		}
	else if (PoseQuestion(query)) { // this query str is made new for us
		gCursor->watch();
		fQuery= query;
		//DFolderGopher::OpenQuery(); << no, sends \t+ which leads client to ask for !info, which server can't produce
		DGopher::OpenQuery();
		}
	else {
		fBytesExpected= 0;
		fBytesReceived= 0;
		if (fTalker) fTalker->SetEndOfMessage(true);
		fThreadState= kThreadDoneReading;
		//?? what will kill read??
		}
}
		
		
#if GOOBSOLETE
short DIndexGopher::GopherIt() // override 
{
	char*		query;				//?? need to save user query for doc word highlighting 
	short	result= cNoCommand;
	
	result= DFolderGopher::GopherIt();
	return result;
}
#endif// GOOBSOLETE


void DIndexGopher::Initialize() 
{
}

DIndexGopher::DIndexGopher(): DFolderGopher()
{
	Initialize();
}

DIndexGopher::DIndexGopher(DIndexGopher* aGopher): DFolderGopher(aGopher)
{
	//Initialize();
}

DIndexGopher::DIndexGopher( char ctype, const char* link): DFolderGopher( ctype, link)
{
	Initialize();
}

DObject* DIndexGopher::Clone()
{
	DIndexGopher* aGopher= new DIndexGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}

void DIndexGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kIndexIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* DIndexGopher::GetKindName() 
{ 
	return "Query";
}



#ifdef WIN_MSWIN
long	DBinaryGopher::kMacType = 1235;
long	DBinaryGopher::kMacSire = 3215;
#else
long	DBinaryGopher::kMacType = 'BINA';
long	DBinaryGopher::kMacSire = '    ';
#endif
char * DBinaryGopher::kSuffix = ".binary";

void DBinaryGopher::InitSignature()	 
{
	if (gGopherMap) gGopherMap->GetPreferedFiletype("file/-", kMacType, kMacSire, kSuffix); 	
}

void DBinaryGopher::Initialize()	// override 
{
	fMacType= kMacType; //'BINA';  
	fMacSire= kMacSire; //'????';
	fTransferType= kTransferBinary; //text/lines or binary or none ?
	fSaveToDisk= true;
	fLaunch= false;
	fCommand= cNewGopherFile;
}


DBinaryGopher::DBinaryGopher(): DGopher()
{
	Initialize();
}

DBinaryGopher::DBinaryGopher(DBinaryGopher* aGopher): DGopher(aGopher)
{
	//Initialize();
}


DBinaryGopher::DBinaryGopher( char ctype, const char* link):
	DGopher( ctype, link)
{
	Initialize();
}


DObject* DBinaryGopher::Clone()
{
	DBinaryGopher* aGopher= new DBinaryGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}


void DBinaryGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kBinaryIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* DBinaryGopher::GetKindName() 
{ 
	return "Binary file";
}




void DBinaryGopher::OpenQuery()
{
	if (fQueryPlus != NULL)  ; 			// if wrapper has set this (for +ASK reply) leave it
	else if (fIsPlus != kGopherPlusYes) fQueryPlus= NULL;
	else fQueryPlus= "\t+"; // gopher+, this form forces server to send data size first

	DGopher::OpenQuery();		
}

#if GOOBSOLETE
short DBinaryGopher::GopherIt()
{	
	short	result= cNoCommand;
	Boolean FileIsMade= false;
	long chunksize = kReadMaxbuf1; //Min( FreeMem()/2, kReadMaxbuf1);

	OpenQuery();		
#if 1
	if (fInfoHandler)
	 while (!EndOfMessage()) {
			ReadAChunk( chunksize);
			//(fInfoHandler)(this);
			fInfoHandler->Process();
			}
	else
#endif	
		FileIsMade= ReadToFile( chunksize);
	if (fLaunch) (void) this->LaunchFile();
	CloseQuery();
	return result;
}
#endif  // GOOBSOLETE





#ifdef WIN_MSWIN
long	DSoundGopher::kMacType = 4333;
long	DSoundGopher::kMacSire = 3334;
#else
long	DSoundGopher::kMacType = 'snd ';
long	DSoundGopher::kMacSire = '    ';
#endif
char * DSoundGopher::kSuffix = ".sound";

void DSoundGopher::InitSignature()	 
{
	if (gGopherMap) gGopherMap->GetPreferedFiletype("audio/-", kMacType, kMacSire, kSuffix); 	
}

void DSoundGopher::Initialize()	// override 
{
	//? fType= kTypeSound; 
	fMacType= kMacType; //'snd ';   
	fMacSire= kMacSire; //'    ';
	fTransferType= kTransferBinary; //text/lines or binary or none ?
	fSaveToDisk= true;
	fLaunch= false; //?? true, but how ?
	fCommand= cNewGopherFile;
}

DSoundGopher::DSoundGopher(): DBinaryGopher()
{
	Initialize();
}

DSoundGopher::DSoundGopher(DSoundGopher* aGopher): DBinaryGopher(aGopher)
{
	//Initialize();
}

DSoundGopher::DSoundGopher( char ctype, const char* link):
	DBinaryGopher( ctype, link)
{
	Initialize();
}

DObject* DSoundGopher::Clone()
{
	DSoundGopher* aGopher= new DSoundGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}

void DSoundGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kSoundIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* DSoundGopher::GetKindName() 
{ 
	return "Sound";
}

#if GOOBSOLETE
short DSoundGopher::GopherIt(void) // override 
{
	return DBinaryGopher::GopherIt();
}
#endif  // GOOBSOLETE




#ifdef WIN_MSWIN
long	DImageGopher::kMacType = 5555;
long	DImageGopher::kMacSire = 3333;
#else
long	DImageGopher::kMacType = 'GIFf';
long	DImageGopher::kMacSire = 'JVWR';
#endif
char * DImageGopher::kSuffix = ".image";

void DImageGopher::InitSignature()	 
{
	if (gGopherMap) gGopherMap->GetPreferedFiletype("image/-", kMacType, kMacSire, kSuffix); 	
}

void DImageGopher::Initialize()	// override 
{
	//? fType= kTypeImage; 
	fMacType= kMacType; //'GIFf';   
	fMacSire= kMacSire; //'Bozo';  
	fTransferType= kTransferBinary; //text/lines or binary or none ?
	fSaveToDisk= true;
	fLaunch= false; 
	fCommand= cNewGopherImage;
}

DImageGopher::DImageGopher(): DBinaryGopher()
{
	Initialize();
}

DImageGopher::DImageGopher(DImageGopher* aGopher): DBinaryGopher(aGopher)
{
	//Initialize();
}

DImageGopher::DImageGopher( char ctype, const char* link):
	DBinaryGopher( ctype, link)
{
	Initialize();
}

DObject* DImageGopher::Clone()
{
	DImageGopher* aGopher= new DImageGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}

void DImageGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kImageIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* DImageGopher::GetKindName() 
{ 
	return "Image";
}

#if GOOBSOLETE
short DImageGopher::GopherIt() // override 
{
	return DBinaryGopher::GopherIt();
}
#endif





#ifdef WIN_MSWIN
long	DMovieGopher::kMacType = 3322;
long	DMovieGopher::kMacSire = 2233;
#else
long	DMovieGopher::kMacType = 'MooV';
long	DMovieGopher::kMacSire = 'TVOD';
#endif
char * DMovieGopher::kSuffix = ".movie";

void DMovieGopher::InitSignature()	 
{
	if (gGopherMap) gGopherMap->GetPreferedFiletype("movie/-", kMacType, kMacSire, kSuffix); 	
}

void DMovieGopher::Initialize()	// override 
{
	//? fType= kTypeMovie; 
	fMacType= kMacType; //'MooV';   
	fMacSire= kMacSire; //'TVOD';   
	fTransferType= kTransferBinary; //text/lines or binary or none ?
	fSaveToDisk= true;
	fLaunch= false; 
	fCommand= cNewGopherFile;
}

DMovieGopher::DMovieGopher(): DBinaryGopher()
{
	Initialize();
}

DMovieGopher::DMovieGopher(DMovieGopher* aGopher): DBinaryGopher(aGopher)
{
	//Initialize();
}

DMovieGopher::DMovieGopher( char ctype, const char* link):
	DBinaryGopher( ctype, link)
{
	Initialize();
}

DObject* DMovieGopher::Clone()
{
	DMovieGopher* aGopher= new DMovieGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}

void DMovieGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kMovieIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* DMovieGopher::GetKindName() 
{ 
	return "Movie";
}

#if GOOBSOLETE
short DMovieGopher::GopherIt() // override 
{
	return DBinaryGopher::GopherIt();
}
#endif



void DNoteGopher::Initialize()	// override 
{
	//? fType= kTypeNote; 
#ifdef WIN_MSWIN
#else
	fMacType= 'TEXT';
	fMacSire= 'Rich';
#endif
	fTransferType= kTransferText;  
	fSaveToDisk= true;
	fLaunch= false; 
	fCommand= cNoCommand;
}

DNoteGopher::DNoteGopher(): DGopher()
{
	Initialize();
}

DNoteGopher::DNoteGopher(DNoteGopher* aGopher): DGopher(aGopher)
{
	//Initialize();
}

DNoteGopher::DNoteGopher( char ctype, const char* link):
	DGopher( ctype, link)
{
	Initialize();
}

DObject* DNoteGopher::Clone()
{
	DNoteGopher* aGopher= new DNoteGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}

void DNoteGopher::InfoTask() 
{	
	fThreadState= kThreadDoneReading; 
}

void DNoteGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kNoteIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* DNoteGopher::GetKindName() 
{ 
	return "Note";
}

#if GOOBSOLETE
short DNoteGopher::GopherIt() // override 
{
	return cNoCommand; //DGopher::GopherIt();
}
#endif




// DHtmlGopher

void DHtmlGopher::Initialize()	// override 
{
	//? fType= kTypeNote; 
#ifdef WIN_MSWIN
#else
	fMacType= 'TEXT';
	fMacSire= 'Rich';
#endif
	fTransferType= kTransferText;  
	fSaveToDisk= false;
	fLaunch= false; 
	fCommand= cNewGopherText;
}

DHtmlGopher::DHtmlGopher(): DTextGopher()
{
	Initialize();
}

DHtmlGopher::DHtmlGopher(DHtmlGopher* aGopher): DTextGopher(aGopher)
{
	//Initialize();
}

DHtmlGopher::DHtmlGopher( char ctype, const char* link):
	DTextGopher( ctype, link)
{
	Initialize();
}

DObject* DHtmlGopher::Clone()
{
	DHtmlGopher* aGopher= new DHtmlGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}


void DHtmlGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kHTMLIcon; 
	gIconSize= size;
	DrawIconSub( area);
}

const char* DHtmlGopher::GetKindName() 
{ 
	return "HTML document";
}




void DMailGopher::Initialize()	// override 
{
	//? fType= kTypeNote; 
#ifdef WIN_MSWIN
#else
	fMacType= 'TEXT';
	fMacSire= 'Rich';
#endif
	fTransferType= kTransferText;  
	fSaveToDisk= true;
	fLaunch= false; 
	//fCommand= cNoCommand;
	fCommand= kGopherTask; // this is wrong, but need something for processing
}

DMailGopher::DMailGopher(): DGopher()
{
	Initialize();
}

DMailGopher::DMailGopher(DMailGopher* aGopher): DGopher(aGopher)
{
	//Initialize();
}

DMailGopher::DMailGopher( char ctype, const char* link):
	DGopher( ctype, link)
{
	Initialize();
}

void DMailGopher::InfoTask() 
{	
	fThreadState= kThreadDoneReading; 
}


void DMailGopher::OpenQuery()
{
	char *mailto= (char*) this->GetPath();
	char *wintitle= (char*)this->ShortName(); //"Mail To";
	DSendMailDialog* win= 
		new DSendMailDialog(0, gApplication, -10, -10, -20, -20, wintitle);
	win->InstallTo(win, mailto);
	win->InstallSubject(win, (char*)this->GetName());
	if (fQueryPlus) win->InstallMessage(win, fQueryPlus);// use this handle for mail message?
	win->Open();
	
	fThreadState= kThreadDoneReading;
}

DObject* DMailGopher::Clone()
{
	DMailGopher* aGopher= new DMailGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}

void DMailGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kNoteIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* DMailGopher::GetKindName() 
{ 
	return "Mailto";
}






void DHTTPGopher::Initialize()	// override 
{
	//? fType= kTypeNote; 
#ifdef WIN_MSWIN
#else
	fMacType= 'TEXT';
	fMacSire= 'Rich';
#endif
	fTransferType= kTransferText;  
	fProtocol= kHTTPprot;
	fSaveToDisk= true;
	fLaunch= false; 
	//fCommand= cNoCommand;
	fCommand= kGopherTask; // this is wrong, but need something for processing
}

DHTTPGopher::DHTTPGopher(): DGopher()
{
	Initialize();
}

DHTTPGopher::DHTTPGopher(DHTTPGopher* aGopher): DGopher(aGopher)
{
	//Initialize();
}

DHTTPGopher::DHTTPGopher( char ctype, const char* link):
	DGopher( ctype, link)
{
	Initialize();
}

void DHTTPGopher::InfoTask() 
{	
	fThreadState= kThreadDoneReading; 
}


void DHTTPGopher::OpenQuery()
{
	DGopher::OpenQuery();		
	if (fTalker) fTalker->NullTerm(true); //!?
	if (fTransferType == kTransferText 
			&& fInfo && fInfoSize>0 && fInfo[fInfoSize] == kLF) 
				fInfo[fInfoSize--]= 0;  
}

DObject* DHTTPGopher::Clone()
{
	DHTTPGopher* aGopher= new DHTTPGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}

void DHTTPGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kTextIcon; //kHTTPIcon; // ! want own icon
	gIconSize= size;
	DrawIconSub( area);
}

const char* DHTTPGopher::GetKindName() 
{ 
	return "HTTP link";
}






// DBinhexGopher.cp
//   class to convert HQX encoded data to Mac file


void DBinhexGopher::Initialize()	// override 
{
	//? fType= kTypeBinhex; 
#ifdef WIN_MSWIN
#else
	fMacType= 'TEXT';
	fMacSire= 'EDIT';
#endif
	fTransferType= kTransferBinary;  //< should be okay for binhex translator 
	fSaveToDisk= true;
	fLaunch= false; //true; ??
	fCommand= cNewGopherFile;

	fHqxbuf= NULL;
}

DBinhexGopher::DBinhexGopher(): DGopher()
{
	Initialize();
}
 
DBinhexGopher::DBinhexGopher(DBinhexGopher* aGopher): DGopher(aGopher)
{
	//Initialize();
	fHqxbuf= NULL;
}

DBinhexGopher::DBinhexGopher( char ctype, const char* link):
	DGopher( ctype, link)
{
	Initialize();
}

DObject* DBinhexGopher::Clone()
{
	DBinhexGopher* aGopher= new DBinhexGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}

void DBinhexGopher::DrawIcon( Nlm_RecT& area, short size)  //override 
{
	gGopherIconID= kBinhexIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* DBinhexGopher::GetKindName() 
{ 
	return "Binhex file";
}



#ifdef FIX_LATER

const long kNeedMoreHQXData = -1;
const long msgMyError = 0x80001234;


/* from hqxify.c in
 * mcvert.c - version 1.05 - 10 January, 1990 modified 12 March, 1990 by NP
 * Written by Doug Moore - Rice University - dougm@rice.edu - April '87
 */
 
//#include "debinhex.h"
/* debinhex.h -- from mactypes.h of mcvert */

/* Useful, though not particularly Mac related, values */
typedef unsigned char byte;     /* one byte, obviously */
typedef unsigned short word;    /* must be 2 bytes */
typedef unsigned long ulong;    /* 4 bytes */
#define CR 0x0d
#define LF 0x0a


// mpwc
#define bcopy(a,b,n)  memcpy(b,a,n)
#define bzero(p,n)  	memset(p, 0, n)     
#define min(a,b)      (a<b)?a:b
#define max(a,b)      (a>b)?a:b


#define NAMELEN 63              /* maximum legal Mac file name length */
#define BINNAMELEN 68           /* NAMELEN + len(".bin\0") */

/* Format of a bin file:
A bin file is composed of 128 byte blocks.  The first block is the
info_header (see below).  Then comes the data fork, null padded to fill the
last block.  Then comes the resource fork, padded to fill the last block.  A
proposal to follow with the text of the Get Info box has not been implemented,
to the best of my knowledge.  Version, zero1 and zero2 are what the receiving
program looks at to determine if a MacBinary transfer is being initiated.
*/ 
typedef struct {     /* info file header (128 bytes). Unfortunately, these
                        longs don't align to word boundaries */
            byte version;           /* there is only a version 0 at this time */
            byte nlen;              /* Length of filename. */
            byte name[NAMELEN];     /* Filename (only 1st nlen are significant)*/
            byte type[4];           /* File type. */
            byte auth[4];           /* File creator. */
            byte flags;             /* file flags: LkIvBnSyBzByChIt */
            byte zero1;             /* Locked, Invisible,Bundle, System */
                                    /* Bozo, Busy, Changed, Init */
            byte icon_vert[2];      /* Vertical icon position within window */
            byte icon_horiz[2];     /* Horizontal icon postion in window */
            byte window_id[2];      /* Window or folder ID. */
            byte protect;           /* = 1 for protected file, 0 otherwise */
            byte zero2;
            byte dlen[4];           /* Data Fork length (bytes) -   most sig.  */
            byte rlen[4];           /* Resource Fork length         byte first */
            byte ctim[4];           /* File's creation date. */
            byte mtim[4];           /* File's "last modified" date. */
            byte ilen[2];           /* GetInfo message length */
						byte flags2;            /* Finder flags, bits 0-7 */
						byte unused[14];       
						byte packlen[4];        /* length of total files when unpacked */
						byte headlen[2];        /* length of secondary header */
						byte uploadvers;        /* Version of MacBinary II that the uploading program is written for */
						byte readvers;          /* Minimum MacBinary II version needed to read this file */
						byte crc[2];            /* CRC of the previous 124 bytes */
						byte padding[2];        /* two trailing unused bytes */
						} info_header;

/* The *.info file of a MacTerminal file transfer either has exactly this
structure or has the protect bit in bit 6 (near the sign bit) of byte zero1.
The code I have for macbin suggests the difference, but I'm not so sure */

/* Format of a hqx file:
It begins with a line that begins "(This file
and the rest is 64 character lines (except possibly the last, and not
including newlines) where the first begins and the last ends with a colon.
The characters between colons should be only from the set in tr86, below,
each of which corresponds to 6 bits of data.  Once that is translated to
8 bit bytes, you have the real data, except that the byte 0x90 may 
indicate, if the following character is nonzero, that the previous
byte is to be repeated 1 to 255 times.  The byte 0x90 is represented by
0x9000.  The information in the file is the hqx_buf (see below),
a CRC word, the data fork, a CRC word, the resource fork, and a CRC word.
There is considerable confusion about the flags.  An official looking document
unclearly states that the init bit is always clear, as is the following byte.
The experience of others suggests, however, that this is not the case.
*/

#define HQXLINELEN 64
typedef struct {
            byte version;           /* there is only a version 0 at this time */
            byte type[4];           /* File type. */
            byte auth[4];           /* File creator. */
            byte flags;             /* file flags: LkIvBnSyBzByChIt */
            byte protect;           /* ?Pr??????, don't know what ? bits mean */
            byte dlen[4];           /* Data Fork length (bytes) -   most sig.  */
            byte rlen[4];           /* Resource Fork length         byte first */
            byte bugblank;             /* to fix obscure sun 3/60 problem
                                          that always makes sizeof(hqx_header
                                          even */
            } hqx_header;

typedef struct {     /* hqx file header buffer (includes file name) */
            byte nlen;              /* Length of filename. */
            byte name[NAMELEN];     /* Filename: only nlen actually appear */
            hqx_header all_the_rest;/* and all the rest follows immediately */
            } hqx_buf;


/* An array useful for CRC calculations that use 0x1021 as the "seed" */
static unsigned short gMagicCRC[] = {
    0x0000,  0x1021,  0x2042,  0x3063,  0x4084,  0x50a5,  0x60c6,  0x70e7,
    0x8108,  0x9129,  0xa14a,  0xb16b,  0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
    0x1231,  0x0210,  0x3273,  0x2252,  0x52b5,  0x4294,  0x72f7,  0x62d6,
    0x9339,  0x8318,  0xb37b,  0xa35a,  0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
    0x2462,  0x3443,  0x0420,  0x1401,  0x64e6,  0x74c7,  0x44a4,  0x5485,
    0xa56a,  0xb54b,  0x8528,  0x9509,  0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
    0x3653,  0x2672,  0x1611,  0x0630,  0x76d7,  0x66f6,  0x5695,  0x46b4,
    0xb75b,  0xa77a,  0x9719,  0x8738,  0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
    0x48c4,  0x58e5,  0x6886,  0x78a7,  0x0840,  0x1861,  0x2802,  0x3823,
    0xc9cc,  0xd9ed,  0xe98e,  0xf9af,  0x8948,  0x9969,  0xa90a,  0xb92b,
    0x5af5,  0x4ad4,  0x7ab7,  0x6a96,  0x1a71,  0x0a50,  0x3a33,  0x2a12,
    0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,  0x9b79,  0x8b58,  0xbb3b,  0xab1a,
    0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,  0x2c22,  0x3c03,  0x0c60,  0x1c41,
    0xedae,  0xfd8f,  0xcdec,  0xddcd,  0xad2a,  0xbd0b,  0x8d68,  0x9d49,
    0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,  0x3e13,  0x2e32,  0x1e51,  0x0e70,
    0xff9f,  0xefbe,  0xdfdd,  0xcffc,  0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
    0x9188,  0x81a9,  0xb1ca,  0xa1eb,  0xd10c,  0xc12d,  0xf14e,  0xe16f,
    0x1080,  0x00a1,  0x30c2,  0x20e3,  0x5004,  0x4025,  0x7046,  0x6067,
    0x83b9,  0x9398,  0xa3fb,  0xb3da,  0xc33d,  0xd31c,  0xe37f,  0xf35e,
    0x02b1,  0x1290,  0x22f3,  0x32d2,  0x4235,  0x5214,  0x6277,  0x7256,
    0xb5ea,  0xa5cb,  0x95a8,  0x8589,  0xf56e,  0xe54f,  0xd52c,  0xc50d,
    0x34e2,  0x24c3,  0x14a0,  0x0481,  0x7466,  0x6447,  0x5424,  0x4405,
    0xa7db,  0xb7fa,  0x8799,  0x97b8,  0xe75f,  0xf77e,  0xc71d,  0xd73c,
    0x26d3,  0x36f2,  0x0691,  0x16b0,  0x6657,  0x7676,  0x4615,  0x5634,
    0xd94c,  0xc96d,  0xf90e,  0xe92f,  0x99c8,  0x89e9,  0xb98a,  0xa9ab,
    0x5844,  0x4865,  0x7806,  0x6827,  0x18c0,  0x08e1,  0x3882,  0x28a3,
    0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,  0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
    0x4a75,  0x5a54,  0x6a37,  0x7a16,  0x0af1,  0x1ad0,  0x2ab3,  0x3a92,
    0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,  0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
    0x7c26,  0x6c07,  0x5c64,  0x4c45,  0x3ca2,  0x2c83,  0x1ce0,  0x0cc1,
    0xef1f,  0xff3e,  0xcf5d,  0xdf7c,  0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
    0x6e17,  0x7e36,  0x4e55,  0x5e74,  0x2e93,  0x3eb2,  0x0ed1,  0x1ef0
    };


/* Essentials for Binhex 8to6 run length encoding */
#define RUNCHAR 0x90
#define MAXRUN 255
#define IS_LEGAL <0x40
#define ISNT_LEGAL >0x3f
#define DONE 0x7F 		/* gTrans68[':'] = DONE, since Binhex terminator is ':' */
#define SKIP 0x7E 		/* gTrans68['\n'|'\r'] = SKIP, i. e. end of line char.  */
#define FAIL 0x7D 		/* character illegal in binhex file */
#define NULLTERM 0x7C /* end of buffer -- need to read more */

//static byte tr86[] =
//		"!\"#$%&'()*+,-012345689@ABCDEFGHIJKLMNPQRSTUVXYZ[`abcdefhijklmpqr"; 
		
static byte gTrans68[] = {
    NULLTERM, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, SKIP, FAIL, FAIL, SKIP, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, FAIL, FAIL,
    0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, FAIL,
    0x14, 0x15, DONE, FAIL, FAIL, FAIL, FAIL, FAIL,
    0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
    0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, FAIL,
    0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, FAIL,
    0x2C, 0x2D, 0x2E, 0x2F, FAIL, FAIL, FAIL, FAIL,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, FAIL,
    0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, FAIL, FAIL,
    0x3D, 0x3E, 0x3F, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    };


#define READING 0
#define SKIPPING 1
#define FIND_START_COLON 2

#define	CRCERROR 1
#define READERR -1
#define BINHEX_KEY	"must be converted with BinHex"

const long 	kHqxMaxChunk = kReadMaxbuf1;
const long  HQXBUFLEN= kHqxMaxChunk;

#endif // FIX_LATER



void DBinhexGopher::hqxFini()
{
#ifdef FIX_LATER
	fHqxbuf= (byte*) MemFree( (Ptr)fHqxbuf);
#endif // FIX_LATER
}

void DBinhexGopher::hqxInit() /* call before each hqx file is processed */
{
#ifdef FIX_LATER
	fDecodeStatus = FIND_START_COLON;
	fHqxbuf= (byte*) MemGet( HQXBUFLEN+1, true);
	fBufptr= fBufstart= fHqxbuf+1;
	fBufend= fBufstart + HQXBUFLEN;

	//fLine= NewPtrClear(MAXLINE+1);
	//fLinestart= fLine+1;
	//fLineend= fLinestart + MAXLINE;
	//fLineptr = fLinestart;
	//fLineptr[0] = SKIP;
	
	fLinecount= 0; //fInoffset = 0; // need to rename this field
	
	fTotalbytes = 0;
	fSavestate = 0;
	fSaveRunLength = 0;
	fGotBinhexKey = false;

	if (fTalker) fTalker->NullTerm( true);  
	ReadAChunk( kHqxMaxChunk);	
#endif 	// FIX_LATER
}


short DBinhexGopher::fill_hqxbuf()
{   
#ifdef FIX_LATER
	register unsigned long c, nibble;
	register int 		not_in_a_run = true, state68;
	register byte		*inptr, *outptr, *bp;
	byte 						*instart, *inend, *outend;
	Boolean 				okay;
	SignedByte			savedState;	

	if (!fInfo) return false; // make sure fInfo has data before this
	savedState = LockHandleHigh(fInfo);  
	instart= (byte*) *fInfo;
	inptr= instart + fLinecount; //fInoffset;
	inend= instart + GetHandleSize( fInfo) - 1;
	okay= true; //(instart < inend);
 
	while (!fGotBinhexKey && okay) {
		c = 0;
		while (inptr < inend  && !(*inptr == ':' && (c == kCR || c == kLF))) 
			 c= *inptr++; 

		if (inptr >= inend) {
				// need to expand fInfo, not replace it
			HUnlock(fInfo);
			if (*inend == 0) SetHandleSize(fInfo, GetHandleSize( fInfo) - 1); // drop null
			ReadAChunk( kHqxMaxChunk); 
			HLock( fInfo);
			long temp= inptr - instart;
			instart= (byte*) *fInfo;
			inptr= instart + temp;
			byte* tmp1= instart + GetHandleSize( fInfo) - 1;
			okay = (tmp1 > inend);
			inend= tmp1;
			}
			
		else { // found key char
			char* keyptr= (char*) Max( (long)instart, (long)inptr - 82);
			keyptr= strstr( keyptr, BINHEX_KEY);
			inptr++; // skip start colon
			if (keyptr) {
				fGotBinhexKey= true;
				for (bp= inptr; bp <= inend; bp++) *bp = gTrans68[*bp]; // translate current inputs
				}
			}	 	
		}
	if (!okay) return false; // ran out of input before found start of valid data
		
	fDecodeStatus = READING;
	outptr= fBufptr= fBufstart;
	outend= fBufend= fBufstart + HQXBUFLEN;
	state68 = fSavestate;
	nibble  = fSaveNibble;
	if (fSaveRunLength > 0) {
		c = fSaveRunLength;
		fSaveRunLength = 0;
		goto continue_run;
		}

	while (okay && outptr < outend) {

next_char:	

		if (inptr >= inend) { // (?? || c == NULLTERM)
			//read & translate data
			HUnlock(fInfo);
			SetHandleSize(fInfo, 0); // no need to save old inputs
			ReadAChunk( kHqxMaxChunk); // sticks null at end of read?!
			if (!fInfo || GetHandleSize(fInfo)<2) { okay= false;  goto data_end; } //out of data
			HLock( fInfo);
			inptr= instart= bp= (byte*) *fInfo;
			inend= instart + GetHandleSize( fInfo) - 1;
			//while ((*bp = gTrans68[*bp]) IS_LEGAL) bp++;
			for (bp= inptr; bp <= inend; bp++) *bp = gTrans68[*bp];
			}

	  c = *inptr++;
		
	  if (c ISNT_LEGAL) {
			if (c == DONE) { //kStartOrEnd
				fDecodeStatus = READING; //??
				okay= false;
				}
			else if (c == NULLTERM) {
				//read & translate chunk
				//this should be handled by (inptr >= inend)
				}
			else if (c == SKIP) {
				// newline
				}
			}
			
		else { // (c IS_LEGAL) 
			if (fDecodeStatus == READING) {
				//nibble c;
				//stuff c into buf;
				//outptr++;
				
        switch (state68++) {
        case 0:
            nibble = c;
            goto next_char;
        case 1:
            nibble = (nibble << 6) | c;
            c = nibble >> 4;
            break;
        case 2:
            nibble = (nibble << 6) | c;
            c = (nibble >> 2) & 0xff;
            break;
        case 3:
            c = (nibble << 6) & 0xff | c;
            state68 = 0;
            break;
            }
						
        if (not_in_a_run)
            if (c != RUNCHAR) *outptr++ = c;
            else {not_in_a_run = false; goto next_char;}
        else {
            if (c--) {
                not_in_a_run = outend - outptr;
                if (c > not_in_a_run) {
                    fSaveRunLength = c - not_in_a_run;
                    c = not_in_a_run;
                    }
				continue_run:
                not_in_a_run = outptr[-1];
                while (c--) *outptr++ = not_in_a_run;
                }
            else *outptr++ = RUNCHAR;
            not_in_a_run = true;
            }

				}
				
			else if (fDecodeStatus == SKIPPING) {
				//???
				}
			
			}
		}	
		
data_end:
	fLinecount = inptr - instart; //fInoffset  // save offset from start of fInfo of our move to fHqxbuf
	if (outptr < outend) fBufend= outptr;
	fTotalbytes += fBufend - fBufptr; // don't need, dups fBytesread
	fBufstart[-1] = outptr[-1]; //?? what is this, saving some data b/n calls?
	fSavestate = state68;
	fSaveNibble = nibble;
	HSetState( fInfo, savedState);
	return (fBufptr < fBufend);
#endif // FIX_LATER
	return 0;
}


long DBinhexGopher::check_hqx_crc()
{   
#ifdef FIX_LATER
		word read_crc;
		
    if (fBufptr >= fBufend) { if (fill_hqxbuf() == false) return READERR;  }
    read_crc  = *fBufptr++ << 8;
    if (fBufptr >= fBufend) { if (fill_hqxbuf() == false) return READERR;  }
    read_crc |= *fBufptr++;
		return read_crc;
#endif // FIX_LATER
	return 0;
}

/* This routine reads the header of a hqxed file and appropriately twiddles it,
    determines if it has CRC problems & returns needed Mac file info */

short DBinhexGopher::hqx2FinderInfo( char *binfname, long *fauth, long *ftype, 
										unsigned long *hqx_datalen, unsigned long  *hqx_rsrclen) 
{   
#ifdef FIX_LATER
		register byte *hqx_ptr, *hqx_end;
    register ulong calc_crc;
		word				hqx_crc;
		int					len;
    hqx_buf 		*hqx_block;
    hqx_header 	*hqx;

    /* read the hqx header, assuming that I won't exhaust hqxbuf in so doing */
		if (fill_hqxbuf() == false) return READERR;
		
    hqx_block = (hqx_buf *) fBufptr;
  	hqx = (hqx_header *) (hqx_block->name + hqx_block->nlen); 
    hqx_ptr = fBufptr;
    hqx_end = (byte *) hqx + sizeof(hqx_header) - 1;
    calc_crc = 0;
    while (hqx_ptr < hqx_end)
        calc_crc = (((calc_crc&0xff) << 8) | *hqx_ptr++) ^ gMagicCRC[calc_crc >> 8];
    calc_crc = ((calc_crc&0xff) << 8) ^ gMagicCRC[calc_crc >> 8];
    calc_crc = ((calc_crc&0xff) << 8) ^ gMagicCRC[calc_crc >> 8];
    fBufptr = hqx_ptr;

		len= min( BINNAMELEN-1, hqx_block->nlen);
    strncpy( binfname, (char*)hqx_block->name, len);  
		binfname[len]= 0;
		bcopy( hqx->auth, fauth, 4);
		bcopy( hqx->type, ftype, 4);
    bcopy( hqx->dlen, hqx_datalen, 4);         
    bcopy( hqx->rlen, hqx_rsrclen, 4);         

		hqx_crc= check_hqx_crc();
		if (hqx_crc == (word) calc_crc) return 0;
		else return CRCERROR;
#endif	// FIX_LATER
	return 0;
}


/* This routine copies bytes from the decoded input stream to the output. */
		
short DBinhexGopher::hqx2fork( short fileref, unsigned long nbytes) 
{   
#ifdef FIX_LATER
		register byte *c;
    register ulong calc_crc;
    register ulong c_length;
		word	hqx_crc;
		long	buflen;

    calc_crc = 0;
    for (;;) {
        c = fBufptr;
        buflen = c_length = (c + nbytes > fBufend) ? fBufend - c : nbytes;
        nbytes -= c_length;
				(void) FSWrite( fileref, &buflen, c); 
				
        while (c_length--)
            calc_crc = (((calc_crc&0xff) << 8) | *c++) ^ gMagicCRC[calc_crc >> 8];
        if (!nbytes) break;
				if (fill_hqxbuf() == false) return READERR;
        }
    fBufptr = c;
    calc_crc = ((calc_crc&0xff) << 8) ^ gMagicCRC[calc_crc >> 8];
    calc_crc = ((calc_crc&0xff) << 8) ^ gMagicCRC[calc_crc >> 8];

		hqx_crc= check_hqx_crc();
    if (hqx_crc == (word) calc_crc) return 0;
		else return CRCERROR;
#endif	// FIX_LATER
	return 0;
}



#if GOOBSOLETE
short DBinhexGopher::GopherIt() // override 
{	
#ifdef FIX_LATER
	short		err,i, fileRef, volRefnum;
	unsigned long		hqxdatalen, hqxrsrclen;
	long	fileAuth, fileType;
	CStr255	fileName;
	FailInfo	fi;
	SFReply	reply;
	Boolean FileIsMade= false;

	//short saveIsPlus = fIsPlus;
	//fIsPlus= kGopherPlusNo; // god damn stupid ass bug what the fuck is going on here
	
	if (fQueryPlus != NULL)  ; 			// if wrapper has set this (for +ASK reply) leave it
	else if (fIsPlus != kGopherPlusYes) fQueryPlus= NULL;
	else fQueryPlus= "\t+"; // gopher+, this form forces server to send data size first
	
	if (fi.Try()) {
		OpenQuery();		

		//if (fTalker->CharsAvailable() == 0) 
		//	fTalker->ShowMessage("TCP connection timed out");  
		//else 
		{
			if (fTalker) fTalker->ShowMessage("Gopher is digging for data");
			fileName[1]= 0;
			
			hqxInit(); 
			err= hqx2FinderInfo( (char*)&fileName[1], &fileAuth, &fileType, 
															&hqxdatalen, &hqxrsrclen);	
			for (i= 1;  (i<80) && (fileName[i]!=0); i++) ;
			fileName[0]= i-1;
		
			if (fTalker) fTalker->ShowMessage( "Found Binhex file "+fileName);
			FailOSErr( err); //?
	
			// file put dialog: where to save, or cancel...
			if (gUseDigFolder || !BringAppFrontmost( 0)) {
				memcpy(reply.fName, (unsigned char*)fileName, 1+fileName[0]); //reply.fName= fileName;
				reply.good= true;  
				err= OpenWD( gDigVol, gDigDirID, 0, &reply.vRefNum);
				}		
			else {
				CPoint		where;
				CenterSFPut( where);
				SFPutFile( where,  CStr31("Save fetched file as:"), fileName, NULL, &reply);
				gApplication->UpdateAllWindows();
				}
			
			if (reply.good) {
				fileName = reply.fName;
				volRefnum = reply.vRefNum;
				(void) FSDelete( fileName, volRefnum) ;
				FailOSErr( Create( fileName, volRefnum, fileAuth, fileType));
				FileIsMade= true;
				
				if (hqxdatalen>0) {		// data fork
					FailOSErr( FSOpen( fileName, volRefnum, &fileRef));
					if (fTalker) fTalker->ShowMessage("Reading data fork");
					err= hqx2fork( fileRef, hqxdatalen);
					(void) FSClose( fileRef);
					FailOSErr( err);
					}
				
				if (hqxrsrclen>0) {
					FailOSErr( OpenRF( fileName, volRefnum, &fileRef));
					if (fTalker) fTalker->ShowMessage("Reading rsrc fork");
					err= hqx2fork( fileRef, hqxrsrclen);
					(void) FSClose( fileRef);
					if (hqxrsrclen > 0) FailOSErr( err); 
					}
				
				// AppFile *pFile = new Appfile;
				AppFile	theFile;
				DisposeIfHandle( fAppFile);
				FailNIL(fAppFile = NewHandle(sizeof(AppFile)));
				theFile.vRefNum= volRefnum;  
				theFile.fType= fileType;
				//theFile.fName= fileName;
				memcpy( theFile.fName, &fileName[0], 1+fileName[0]);
				memcpy( *fAppFile, &theFile, sizeof(AppFile));

				if (fLaunch) (void) this->LaunchFile();
				}
			}

		hqxFini(); 
		fi.Success();
		CloseQuery();
		}
	else { // fail
		hqxFini(); 
		CloseQuery();
		if (FileIsMade) (void) FSDelete( fileName, volRefnum);
		fi.ReSignal();	
		}

	//fIsPlus= 	saveIsPlus;
#endif	// FIX_LATER
	return cNoCommand;
}
#endif






const short kGopherOkayID	= 2206;	// dialog window for messages 


void DTelnetGopher::Initialize()	// override 
{
	//? fType= kTypeTelnet; 
#ifdef WIN_MSWIN
#else
	fMacType= 'CONF';
	fMacSire= 'NCSA';
#endif
	fTransferType= kTransferText; //text/lines or binary or none ?
	fSaveToDisk= true;
	fLaunch= true;
	fCommand= cNewGopherFile;
}

DTelnetGopher::DTelnetGopher(): DGopher()
{
	Initialize();
}

DTelnetGopher::DTelnetGopher(DTelnetGopher* aGopher): DGopher(aGopher)
{
	//Initialize();
}

DTelnetGopher::DTelnetGopher( char ctype, const char* link):
	DGopher( ctype, link)
{
	Initialize();
}

DObject* DTelnetGopher::Clone()
{
	DTelnetGopher* aGopher= new DTelnetGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}

void DTelnetGopher::InfoTask() 
{	
	fThreadState= kThreadDoneReading; 
}


void DTelnetGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kTelnetIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* DTelnetGopher::GetKindName() 
{ 
	return "Telnet session";
}

void DTelnetGopher::promptUser(char* msg1, char* msg2)
{
#ifdef FIX_LATER
	TWindow *aWind = (TWindow *)gViewServer->NewTemplateWindow(kGopherOkayID, NULL);
	FailNIL( aWind); 		 
	TStaticText	*aTitle = (TStaticText*) aWind->FindSubView('msg1');
	if (aTitle!=NULL) aTitle->SetText( msg1, kDontRedraw);
	aTitle = (TStaticText*) aWind->FindSubView('msg2');
	if (aTitle!=NULL) aTitle->SetText( msg2, kDontRedraw);
	//TDialogView	*aDlog = (TDialogView*) aWind->FindSubView('DLOG');
	aWind->SetModality( true); //PoseModally will fail !!
	IDType dismisser = aWind->PoseModally();
	//if (dismisser == 'OKAY') ;
	aWind->CloseAndFree(); 
	gApplication->UpdateAllWindows();
#endif	// FIX_LATER
}


#if GOOBSOLETE
short DTelnetGopher::GopherIt() // override 
/*---- 
Example Telnet gopher links:
Name=CUline, University of Colorado,Boulder
Type=8
Port=852
Path=
Host=culine.Colorado.EDU

Name=Columbia University
Type=8
Port=0
Path=calendar  << account name to log on as ...
Host=cal.cc.columbia.edu
------*/
{
#ifdef FIX_LATER
	short			i, fileRef, vRefNum;
	long			dirID;
	CPoint		where;
	SFReply		reply;
	Boolean		okay, hasSystem7;
	CStr255		msg1, msg2;
#define		  fileName	msg1
	CStr255 	myName= this->GetName();
	CStr255 	myHost= this->GetHost();
	
	fileName= myName;  
	if (fileName.Length() > 31) fileName[0]= 31;
	for (i= 1; i<= fileName.Length(); i++) if (fileName[i] == ':') fileName[i]= '-';
	dirID= 0;
	vRefNum= 0;
	okay= false;
	
	fThreadState= kThreadDoneReading; // this method doesn't read tcp directly
	
	if (gUseDigFolder) {
		vRefNum= gDigVol;
		dirID= gDigDirID;
		okay= true;  
		}
	/*****		
	else if (GestaltFlag( gestaltFindFolderAttr, gestaltFindFolderPresent)) {
		//! can't use kTemporaryFolderType == app can't find files there
		okay= noErr == FindFolder(kOnSystemDisk,kPreferencesFolderType, 
																	kCreateFolder,vRefNum,dirID);
		}
	*****/
	
	if (!okay) {
		if (!BringAppFrontmost( 0)) {
			okay= true;
			vRefNum= gDigVol;
			dirID= gDigDirID;
			}		
		else {
			CenterSFPut( where);
			SFPutFile( where, CStr31("Save Telnet session as:"), fileName, NULL, &reply);
			gApplication->UpdateAllWindows();
			if (reply.good) {
				memcpy((unsigned char*)fileName, reply.fName, 1+reply.fName[0]); //fileName = reply.fName;
				vRefNum  = reply.vRefNum;
				okay= true;
				}
			}
		}
	
	if (okay) {
		unsigned int  bufmax = 300 + myName.Length() + myHost.Length();
		char* buf= (char*) malloc( bufmax);
		char* cp= buf;
		sprintf( cp, "name=\"%s\"\n", (char*)myName);  cp += strlen(cp);
		sprintf( cp, "host=\"%s", (char*)myHost); cp += strlen(cp);
		if (fPort != 0) { sprintf( cp, " %d", fPort); cp += strlen(cp);}
		sprintf( cp, "\"\n\0"); cp += strlen(cp);
		long buflen= strlen( buf);
		
		if (0 != HDelete( vRefNum, dirID, fileName)) ;
		FailOSErr( HCreate( vRefNum, dirID, fileName, fMacSire, fMacType));
		FailOSErr( HOpen( vRefNum, dirID, fileName, fsWrPerm, &fileRef));
		FailOSErr( FSWrite( fileRef, &buflen, buf)); 
		FailOSErr( FSClose( fileRef));
		free( buf);
		
		if (fLaunch && gHasAppleEvents) 
			hasSystem7= (noErr == LaunchFileThruFinder(vRefNum, dirID, fileName));
		else
			hasSystem7= false;
			
		if (hasSystem7) msg1= "";
		else msg1= "To use, launch the telnet session file '" + fileName + "'";
		//if ( *(*fLink+fIpath)  == 0) msg2= "";
		if (this->GetPath().Length() == 0) msg2= "";
		else msg2= "At the telnet login/username prompt, log on as '" 
								+ this->GetPath() + CStr31("'");
		if (!msg1.IsEmpty() || !msg2.IsEmpty()) promptUser(msg1, msg2);

		//delete temp file...can we do it yet? must be sure finder has launched...
		//- if (hasSystem7) if (0!=HDelete( vRefNum, dirID, fileName)) ; 
		// what we really need is a list of garbage/temp files to delete on quitting
		//	program... include childApp temp files
		}
		
	//SetString( gGopherTitle, myName); 
	//gGopherData= NULL;
#endif	// FIX_LATER

	return cNoCommand;
}
#endif // GOOBSOLETE






#ifdef FIX_LATER
const short kGopherPhoneID	= 2207;	// dialog window for search query 

void CCSOPhoneBookGopher::addToInfo( char *cp, short len)
{
	Handle h= fInfo;
	if (h==NULL) FailOSErr( PtrToHand( cp, &h, len));
	else FailOSErr( PtrAndHand( cp, h, len));
	if ( cp[len-1] != chRtn ) FailOSErr( PtrAndHand( &chRtn, h, 1));
	fInfo= h;
}



void CCSOPhoneBookGopher::ReadALine() 
{
		CStr255		aStr; 
		char		**data;

	if (fTalker && fTalker->ReadALine( NULL, data)) {
		switch (**data) {
			case '2': if (fTalker) fTalker->SetEndOfMessage(true); break;
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':  
					fTalker->SetEndOfMessage(true);
					addToInfo( (*fLink+fIpath), (short)strlen(*fLink+fIpath));
					addToInfo( *data + 4, (short) GetHandleSize( data)-4);
					break;
			case '-':  
					if (fTalker) fTalker->SetEndOfMessage(false);
					addToInfo( *data+7, (short) GetHandleSize( data)-7);
					break;
			}
		}
}


void CCSOPhoneBookGopher::ReadAChunk( long maxchunk) // override  
{
	Boolean tooBig;
	//!! need to read by lines to parse CSO, ? use Inherited (RecvChunk) at all ?
	do {
		ReadALine();
		if (fInfo == NULL) tooBig= false;
		else tooBig= (GetHandleSize( fInfo) >= maxchunk);
	} while (!(tooBig || EndOfMessage())); 
}


void CCSOPhoneBookGopher::addQuery( long qname, CStr255& query, TWindow* aWind)
{
	const short	kOperContains = 1;
	const short	kOperStarts	= 2;
	const short	kOperEquals	= 3;
	CStr255		aStr, bStr;

	TCluster	*aClus = (TCluster*) aWind->FindSubView(qname);
	if (aClus!=NULL) {
		TEditText	*aValue = (TEditText*) aClus->FindSubView('TEXT');
		aValue->GetText( aStr);
		if (!aStr.IsEmpty()) {
			TPopup *aTitle = (TPopup*) aClus->FindSubView('NAME');
			aTitle->GetItemText( aTitle->GetCurrentItem(), bStr);
			query += " " + bStr + "=";
			TPopup *aOper= (TPopup*) aClus->FindSubView('OPER');
			switch (aOper->GetCurrentItem()) {
				case kOperContains: query += "\"*" + aStr + "*\""; break;
				case kOperStarts:		query += "\""  + aStr + "*\""; break;
				case kOperEquals:		query += "\""  + aStr + "\""; break;
				}
			}
		}
}

Boolean CCSOPhoneBookGopher::queryUser( CStr255& query)
{
	CStr255		aStr;
	Boolean		result= false;
	
	TWindow* aWind = (TWindow *)gViewServer->NewTemplateWindow(kGopherPhoneID, NULL);
	FailNIL( aWind); 		 
	//TDialogView	*aDlog = (TDialogView*) aWind->FindSubView('DLOG');
	aWind->SetModality( true); //PoseModally will fail !!
	IDType dismisser = aWind->PoseModally();
	if (dismisser == 'OKAY') {
		TCheckBox	*aAnd = (TCheckBox*) aWind->FindSubView('cAND');
		query= "";
		addQuery('cNM1', query, aWind);
		if (aAnd->IsOn()) addQuery('cNM2',query, aWind);
		if (!query.IsEmpty()) {
			query.Insert("query ", 1);
			//?? add 'quit' command to close up CSO ??
			aStr= "12quit"; 
			aStr[1]= chRtn;
			aStr[2]= chLineFeed;
			query= query + aStr;
			result= true;
			}			
		else
			result= false;
		}		
	else
		result= false;
	aWind->CloseAndFree(); 
	gApplication->UpdateAllWindows();
	return result;
}



#if GOOBSOLETE
short CCSOPhoneBookGopher::GopherIt() // override 
/********
example gopher entries for CSO:
Name=University of Illinois Urbana-Champaign
Type=2
Port=105
Path=
Host=ns.uiuc.edu

Simpe gopher CSO dialog:

  1. Name    :
  2. Phone   :
  3. E-Mail  :
  4. Address :

    Press 1-4 to change a field, Return to accept fields and continue
***********/
{
	CStr255 	query;			 
	CommandNumber	result= cNoCommand;
	
 	if (queryUser(query)) {
		long len = query.Length() + 2;
		char* queryptr= NewPtr(len);
		FailNIL(queryptr);
		strcpy( queryptr, "\t");
		strcat( queryptr, query);
		
		fQuery= queryptr; //(char*) &query[1];	
		result= DTextGopher::GopherIt(); //Handle as TextGopher 
		DisposePtr(queryptr);
		}
	return result;
}
#endif // GOOBSOLETE


void CCSOPhoneBookGopher::Initialize()	// override 
{
	fMacType= 'TEXT';  
	fMacSire= 'EDIT';
	fTransferType= kTransferText; //text/lines or binary or none ?
	fSaveToDisk= false;
	fLaunch= false;
	fCommand= cNewGopherFile;
}

CCSOPhoneBookGopher::CCSOPhoneBookGopher(): DTextGopher()
{
	Initialize();
}

CCSOPhoneBookGopher::CCSOPhoneBookGopher(CCSOPhoneBookGopher* aGopher): DTextGopher(aGopher)
{
	//Initialize();
}

CCSOPhoneBookGopher::CCSOPhoneBookGopher( char ctype, const char* link): DTextGopher( ctype, link)
{
	Initialize();
}

void CCSOPhoneBookGopher::InfoTask() 
{	
	fThreadState= kThreadDoneReading; 
}

DObject* CCSOPhoneBookGopher::Clone()
{
	CCSOPhoneBookGopher* aGopher= new CCSOPhoneBookGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}

void CCSOPhoneBookGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kPhonebookIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* CCSOPhoneBookGopher::GetKindName() 
{ 
	return "CSO Phonebook";
}

#endif	// FIX_LATER




#ifdef FIX_LATER
	
Boolean CWhoisPhoneBookGopher::PoseQuestion(CStr255& query) //from DIndexGopher
{
	TWindow	*aWind = (TWindow *)gViewServer->NewTemplateWindow(kGopherSearchID, NULL);
	FailNIL( aWind); 		 
	TStaticText	*aTitle = (TStaticText*) aWind->FindSubView('TITL');
	if (aTitle && fLink) 	aTitle->SetText( this->GetName() + CStr31("?"), kRedraw);
	TEditText	*aFindString = (TEditText*) aWind->FindSubView('QEST');
	//TDialogView	*aDlog = (TDialogView*) aWind->FindSubView('DLOG');
	aWind->SetModality( true); //PoseModally will fail !!
	IDType dismisser = aWind->PoseModally();
	if (dismisser == 'OKAY') {
		aFindString->GetText( query);  
		}		
	aWind->CloseAndFree(); 
	gApplication->UpdateAllWindows();
	return  (dismisser == 'OKAY');
}


#if GOOBSOLETE
short CWhoisPhoneBookGopher::GopherIt() // override 
{
	CStr255 	query;	
	CommandNumber	result= cNoCommand;
	
	if (PoseQuestion(query)) { 
		long len = query.Length() + 2;
		char* queryptr= NewPtr(len);
		FailNIL(queryptr);
		strcpy( queryptr, "\t");
		strcat( queryptr, query);
		
		fQuery= queryptr;  
		result= DTextGopher::GopherIt(); //Handle as TextGopher 
		DisposePtr( queryptr);
		}
	return result;
}
#endif // GOOBSOLETE

void CWhoisPhoneBookGopher::Initialize()	// override 
{
	fMacType= 'TEXT';  
	fMacSire= 'EDIT';
	fTransferType= kTransferText; //text/lines or binary or none ?
	fSaveToDisk= false;
	fLaunch= false;
	fCommand= cNewGopherFile;
}

CWhoisPhoneBookGopher::CWhoisPhoneBookGopher(): DTextGopher()
{
	Initialize();
}

CWhoisPhoneBookGopher::CWhoisPhoneBookGopher(CWhoisPhoneBookGopher* aGopher): DTextGopher(aGopher)
{
	//Initialize();
}

CWhoisPhoneBookGopher::CWhoisPhoneBookGopher( char ctype, const char* link): DTextGopher( ctype, link)
{
	Initialize();
}

DObject* CWhoisPhoneBookGopher::Clone()
{
	CWhoisPhoneBookGopher* aGopher= new CWhoisPhoneBookGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}

void CWhoisPhoneBookGopher::InfoTask() 
{	
	fThreadState= kThreadDoneReading; 
}

void CWhoisPhoneBookGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kWhoisPhonebookIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* CWhoisPhoneBookGopher::GetKindName() 
{ 
	return "Whois Phonebook";
}

#endif	// FIX_LATER





#ifdef FIX_LATER

void CUuencodeGopher::Initialize()	// override 
{
	//? fType= kTypeUuencode; 
	fMacType= 'TEXT';  
	fMacSire= 'EDIT';
	fTransferType= kTransferText; //text/lines or binary or none ?
	fSaveToDisk= false;
	fLaunch= false;
	fCommand= cNewGopherFile;
}

CUuencodeGopher::CUuencodeGopher(): DTextGopher()
{
	Initialize();
}

CUuencodeGopher::CUuencodeGopher(CUuencodeGopher* aGopher): DTextGopher(aGopher)
{
	//Initialize();
}

CUuencodeGopher::CUuencodeGopher( char ctype, const char* link):
	DTextGopher( ctype, link)
{
	Initialize();
}

DObject* CUuencodeGopher::Clone()
{
	CUuencodeGopher* aGopher= new CUuencodeGopher();
	aGopher->CopyGopher( this);
	return aGopher;
}

void CUuencodeGopher::DrawIcon( Nlm_RecT& area, short size)  /* override */ 
{
	gGopherIconID= kUuencodeIcon;
	gIconSize= size;
	DrawIconSub( area);
}

const char* CUuencodeGopher::GetKindName() 
{ 
	return "UU encoded file";
}

#if GOOBSOLETE
short CUuencodeGopher::GopherIt(void) // override 
{
	return DTextGopher::GopherIt();
}
#endif // GOOBSOLETE

#endif	// FIX_LATER





/** gopher+ view_names...
          case kTypeFile:
          case kTypeCSO:    view = "Text"; break;
          case kTypeGif:    view = "image/gif"; break;
          case kTypeImage:  view = "image";  break;
          case kTypeSound:  view = "audio/mulaw"; break;
          case kTypeTn3270: view = "terminal/tn3270";  break;
          case kTypeTelnet: view = "terminal/telnet";  break;

# An example gopherd+.conf file
#

# Different Languages
viewext: .txt.spanish 0 0 Text Es_ES
viewext: .txt.german 0 0 Text De_DE
viewext: .txt.french 0 0 Text Fr_FR


#data formats
#viewext: Filename extension / Advertised type-char / transfer type / +VIEWS type

viewext: .hqx 4 0 file/hqx

#
# Telnet file formats
#
viewext: .telnet 8 0 terminal/telnet
viewext: .tn3270 T 0 terminal/tn3270

#
# Graphics file formats
#
viewext: .gif I 9 image/gif
viewext: .pcx I 9 image/pcx
viewext: .pict I 9 image/PICT
viewext: .tiff I 9 image/TIFF
viewext: .jpg I 9 image/JPEG
viewext: .ppm I 9 image/ppm
viewext: .pgm I 9 image/pgm
viewext: .pbm I 9 image/pgm

#
# Sounds
#
viewext: .au s s audio/mulaw
viewext: .snd s s audio/mulaw
viewext: .wav s s audio/wave

#
# Movies
#
viewext: .quicktime 9 9 video/quicktime
viewext: .mpg 9 9 video/mpeg

#
# Binary files
#
viewext: .tar.Z 9 9 file/tar.Z
viewext: .tar 9 9 file/tar
viewext: .zip 5 9 file/PKzip
viewext: .zoo 5 9 file/Zoo
viewext: .arc 5 9 file/Arc
viewext: .lzh 5 9 file/Lharc
viewext: .exe 5 9 file/PCEXE
viewext: .mcb 9 9 file/MacBinary
viewext: .uu 9 9 file/uuencode


#
# Various forms of text
#
viewext: .ps 0 0 Text/postscript
viewext: .tex 0 0 Text/tex
viewext: .dvi 0 9 Text/dvi
viewext: .troff 0 0 Text/troff
viewext: .unicode 0 0 Text/unicode
viewext: .sjis 0 0 Text/sjis
viewext: .jis 0 0 Text/jis
viewext: .euc 0 0 Text/jis
viewext: .big5 0 0 Text/big-5
viewext: .rtf 0 0 Text/rtf
viewext: .word5 0 0 Text/MSWord5
viewext: .word4 0 0 Text/MSWord4
viewext: .mw 0 0 Text/MacWrite
viewext: .wp 0 0 Text/WordPerfect51

viewext: .smell 9 9 smell/funky

#
#
viewext: .mindex 7 mindex: Directory
viewext: .src 7 waissrc: Directory
viewext: .html h GET /h application/WWW


#
# These are for the experimental gview
#
viewext: .gview 1 1 Directory+/gview
blockext: .spot GVIEWSPOT

# Map files to certain blocks
#
blockext: .abstract ABSTRACT
blockext: .ask ASK

****/
