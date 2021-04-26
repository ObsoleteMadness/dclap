/* ChildAppJ.c
	 d.g.gilbert, Dec'94
	 
	 Version for SeqPup [java], Aug'97
	 -- using MRJ Java Runtime.exec( String[] commands);
	 -- where commands[1..n1] are passed as filenames to open to app[0]
	 -- use MacOS trap: AppFile af= getappfiles(i);
	 
	 
   program main call for Mac apps needing to handle command-line parameters
   true main should be renamed to
   	void RealMain(int argc, char** argv);
   used HighLevel event called "Cmdl" that calling application sends to this
   child application.  This isn't an appleevent.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#undef DEBUG

/* use this only w/ Mac apps */
#define MACINTOSH 1

#include <EPPC.h> /* Apple highlevel events */
#include <quickdraw.h> 
#include <dialogs.h> 
#include <Files.h> 
#include <SegLoad.h> 

extern int RealMain( int argc, char *argv[]);
extern int ccommand(char **argv[]);
static char* gFinderFile = NULL;
static char* gResultFile = NULL;
static char* gResultMsg = "child app";

short	GetFullPath(short vRefNum, long dirID, ConstStr255Param name,
							short *fullPathLength, Handle *fullPath);


#ifdef DEBUG
char* long2str( long val);
char* long2str( long val)
{
	static char buf[5];
  buf[0]= (val >> 24) & 0xff;
  buf[1]= (val >> 16) & 0xff;
  buf[2]= (val >> 8) & 0xff;
  buf[3]= val & 0xff;
  buf[4]= 0;
	return buf;	
}
#endif

char* FindWordEnd( char** start);
char* FindWordEnd( char** start)
{
	char *ep, *cp;
	ep= cp= *start;
	if (*cp == '"' || *cp == '\'') { 
		char key= *cp;
		cp++; 
		(*start)++;  
		for (ep= cp; *ep && *ep != key; ep++) ; 
		}
	else if (*cp) { 
		for (ep= cp+1; 
			*ep && *ep>' ' && *ep!='>' && *ep!='<' && *ep != '"' && *ep != '\''; 
			ep++) ; 
		}
	return ep;
}   

char* FindWordStart( char* cp);
char* FindWordStart( char* cp)
{
	while (*cp && *cp <= ' ') cp++;
	return cp;
}


void displayCommandline(char* cmdline, char* param1, char* param2, char* param3);
void displayCommandline(char* cmdline, char* param1, char* param2, char* param3)
{
	enum { kDlogID = 100, kMaxBuflen = 512 };
	WindowPtr wPort;
	char buf[kMaxBuflen];
	CursHandle hCurs;

	if (param1==NULL) param1="";
	if (param2==NULL) param2="";
	if (param3==NULL) param3="";
	memset(buf,0,kMaxBuflen);
	sprintf( buf, "Running %s ...", cmdline);
	paramtext( buf, param1, param2, param3);
	wPort= GetNewDialog( kDlogID, NULL, (GrafPtr)-1);
	if (wPort) DrawDialog( wPort);
	hCurs = GetCursor(watchCursor);
	if (hCurs) SetCursor(*hCurs);
}


void childExit(int err);
void childExit(int err) 
{
	/* exit(0); -- leaves us in SOIUX shell */
#ifdef DEBUG
fprintf(stdout, " childExit \n");
#endif
	if (gResultFile != NULL) {
		/* notify caller we are finished */
		/*FILE* fp= fopen( gResultFile, "w");
		fprintf( fp, "exit value= %d\n", err);
		fclose( fp); */
		unlink(gResultFile);
#ifdef DEBUG
fprintf(stdout, " childExit result file %s \n", gResultFile);
#endif
		}
	ExitToShell(); 
}


int callMain(int argc, char *argv[], char* sin, char* sout, char* serr);
int callMain(int argc, char *argv[], char* sin, char* sout, char* serr)
{
  int i, err = 0;

#ifdef DEBUG
fprintf(stdout, " +comment: stderr '%s' \n", serr);
if (sout) fprintf(stdout, "+comment: new stdout is '%s'\n",sout);
if (serr) fprintf(stdout, "+comment: new stderr is '%s'\n",serr);
if (sin) fprintf(stdout, "+comment: new stdin is '%s'\n",sin);
for (i=0; i<argc; i++) fprintf(stdout, "arg[%d]= '%s'\n", i, argv[i]);
#endif

	
  if (sin) freopen( sin, "r", stdin);
  if (serr) freopen( serr, "a", stderr);  
	if (sout) freopen( sout, "a", stdout);  
	err= RealMain(argc, argv);

#ifdef DEBUG
fprintf(stdout, "+debug: Test stdout from ChildApp.c --\n+debug: where is stdout going to??\n");
fprintf(stderr, "+error: Test stderr from ChildApp.c --\n+error: where is stderr going to??\n");
#endif
	childExit(err);   
}


void tryThruAppEvent();
void tryThruAppEvent()
{
	enum { kMaxBuflen = 512 };
	Boolean	doloop = true, flag;
	char 	**myargv, * cmdline;
	int		i, err= 0, trynum, myargc;	
	long	time;
	unsigned long msgRefcon, buflen;
	EventRecord evt;
	TargetID	sender;


	trynum= 0;
	buflen= kMaxBuflen;
	cmdline= (char*) malloc(buflen+1);
	memset(cmdline,0,buflen+1);
	
	while (WaitNextEvent( highLevelEventMask, &evt, 2, NULL) || trynum<3)  { 
		trynum++;	
		err= AcceptHighLevelEvent( &sender, &msgRefcon, cmdline, &buflen);

		if (msgRefcon == 'Cmdl') {
			if (err == bufferIsSmall) {
				free(cmdline);
				buflen= kMaxBuflen;
				cmdline= (char*) malloc( buflen+1);
				err= AcceptHighLevelEvent( &sender, &msgRefcon, cmdline, &buflen);
				}
				
			if (!err) {
				/* push buf into argv, separating words */
		    char  *cp, *ep, *sin, *sout, *serr, * maxp, savec;
		    char  done, nextStderr, nextStdin, nextStdout;

		    if (buflen>kMaxBuflen) buflen= kMaxBuflen;
		    cmdline[buflen]= 0;	    
		    sout= sin= serr= NULL;
		    nextStderr= nextStdin = nextStdout= 0;
		    done= 0;
		    myargc= 0;
		    myargv= (char**) malloc(  (myargc+2) * sizeof(char*));
		    cp= cmdline; 
		    maxp= cmdline + buflen;
		    	
		   	displayCommandline(cmdline,NULL,NULL,NULL);

#ifdef DEBUG 
fprintf(stdout, " Cmdline: '%s' \n", cmdline);
#endif
		    while (!done && cp<maxp) {
		    	cp= FindWordStart( cp);
					ep= FindWordEnd( &cp);
					
#ifdef DEBUG
savec= *ep; *ep= 0; 
fprintf(stdout, " +cmdline word: '%s' \n", cp);
*ep= savec;
#endif
					if (nextStderr) {
						nextStderr= 0;
						serr= cp;
						if (*ep) *ep= 0; else done= 1;
						}
					else if (nextStdout) {
						nextStdout= 0;
						sout= cp;
						if (*ep) *ep= 0; else done= 1;
						}
					else if (nextStdin) {
						nextStdin= 0;
						sin= cp;
						if (*ep) *ep= 0; else done= 1;
						}
					else if (*cp == '2' && cp[1] == '>') {
						nextStderr= 1;
						ep= cp+1;
						}
	       	else if (*cp == '>') {
						nextStdout= 1;
						ep= cp;
	       		}
	       	else if (*cp == '<') {
						nextStdin= 1;
						ep= cp;
	       		}
	       	else {
						if (*ep) *ep= 0; else done= 1;
						if (*cp) {
							myargc++;
							myargv= (char**) realloc(myargv, (myargc+2) * sizeof(char*));
							myargv[myargc-1]= cp;
							}
						}
					cp= ep+1;
					}
		    myargv[myargc]= NULL;

				err= callMain(myargc, myargv,  sin,  sout, serr);
				}
			childExit(err);  
			/* quit here, even if no callMain, cause we got trigger HighLevelEvent */
		}
	}
	
}




#include <AppleEvents.h>
#include <Events.h>


enum {
	ae_OpenApp			= 1001,
	ae_OpenDoc			= 1002,
	ae_PrintDoc			= 1003,
	ae_Quit				= 1004 
	};

static AEDescList	gAEDocList;
static long gAEndoc, gAEidoc;

static Boolean SetAFileEvent( long inEvent, long outReply);
static Boolean SetAFileEvent( long inEvent, long outReply)
{    				
	AppleEvent	*aeEvent, *aeReply;
	short				err;
	gAEndoc = gAEidoc = 0;
	if (!inEvent) return false;
	aeEvent = (AppleEvent*) inEvent;
	err= AEGetParamDesc( aeEvent, keyDirectObject, typeAEList, &gAEDocList);
	if (err) return false;
	err= AECountItems( &gAEDocList, &gAEndoc);
	return (err == 0);
}

static void ClearAFileEvent();
static void ClearAFileEvent()
{    				
	if (gAEndoc) {
		(void) AEDisposeDesc( &gAEDocList);
		gAEndoc= 0;
		}
}

static char* GetNextAEFile();
static char* GetNextAEFile()
{    				
	static char buf[2048];
	short				err;
	AEKeyword		keywd;
	DescType		returnedType;
	FSSpec			aFileSpec;
	Size				actualSize;
	short			j, pathlen;
	Handle		pathname;
	char			*name;
	
	if (gAEidoc < gAEndoc) {
		gAEidoc++;
		err= AEGetNthPtr( &gAEDocList, gAEidoc, typeFSS, &keywd, &returnedType,
											(void*)&aFileSpec, sizeof(aFileSpec), &actualSize);
		if (err) return NULL; 
		err=	GetFullPath(aFileSpec.vRefNum, aFileSpec.parID, aFileSpec.name, 
							&pathlen, &pathname);
		/*for (j=	0; j<pathlen; j++) buf[j]= (*pathname)[j];	
		buf[pathlen]= 0;
		name= (char*) malloc(pathlen+1);
		strcpy(name, buf);
		*/
		name= (char*) malloc(pathlen+1);
		memcpy(name, *pathname, pathlen);
		name[pathlen]= 0;
		return name;
		}
	else
		return NULL;
}


static void HandleFileEvent(long inEvent, long outReply, short tasknum);
static void HandleFileEvent(long inEvent, long outReply, short tasknum)
{
	if ( SetAFileEvent( inEvent, outReply)) {
		char* path;
		int nf= 0;
		do {
			path= GetNextAEFile();
			if (path) {
				nf++;
				if (gFinderFile==NULL) gFinderFile= path;
				else if (gResultFile==NULL) gResultFile= path;
#ifdef DEBUG 
fprintf(stdout, " HandleFileEvent got: '%s' \n", path);
#endif
				/*
				DTask* filetask= 
				  gApplication->newTask( tasknum, DTask::kMenu, (long)path);
				gApplication->DoMenuTask( tasknum, filetask);
				delete filetask;
				gApplication->fDidOpenStartDoc= true;
				*/
				}
		} while (path);
		ClearAFileEvent();
		}
}

static short int HandleSpecialEvent( long inEvent, long outReply, long inNumber);
static short int HandleSpecialEvent( long inEvent, long outReply, long inNumber)
{	
#ifdef DEBUG
  fprintf(stdout, "+debug: HandleSpecialEvent %d\n", inNumber);
#endif
	switch (inNumber) {
		case ae_Quit: childExit(0); break;
		case ae_OpenApp: break;
		case ae_PrintDoc:
		case ae_OpenDoc:
			HandleFileEvent(inEvent, outReply, inNumber);
			break;
		}
	return 0;
}


EventRecord  currentEvent;

void getAEStartEvents();
void getAEStartEvents()
{
	int nae, ni;

#if 0	 
	while (EventAvail (highLevelEventMask, &currentEvent)) 
		if (WaitNextEvent (highLevelEventMask, &currentEvent, 0, NULL))  
   	  AEProcessAppleEvent (&currentEvent);
#else
	nae= ni= 0;
	while (true) {
	  if (EventAvail (everyEvent, &currentEvent)) {
	    if (WaitNextEvent (everyEvent, &currentEvent, 0, NULL)) {
	   	  if (currentEvent.what == kHighLevelEvent) {
	   	  	AEProcessAppleEvent (&currentEvent);
	   	  	nae++;
	   	  	if (nae>3) return;
	   	  	}
	    }
	  } else {
  		ni++;
  		if (ni>10000) return;
	  }
  }
#endif
}

void InitSpecialEvents();
void InitSpecialEvents()
{
#define aeproccall(x) NewAEEventHandlerProc(x)
	short err;	

	err= AEInstallEventHandler( kCoreEventClass, kAEOpenApplication,
								aeproccall( HandleSpecialEvent), 
								ae_OpenApp, false);
	err= AEInstallEventHandler( kCoreEventClass, kAEQuitApplication, 
								aeproccall( HandleSpecialEvent), 
								ae_Quit, false);

	/*err= AEInstallEventHandler( kCoreEventClass, kAEApplicationDied, 
								aeproccall( HandleChildDiedEvent), 
								ae_ApplicationDied, false);
	*/
								
	err = AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments,
								aeproccall( HandleSpecialEvent), 
								ae_OpenDoc, false);
	err =  AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments,
								aeproccall( HandleSpecialEvent), 
								ae_PrintDoc, false);
}


/*
struct AppParam {
	short message;
	short count;
	AppFile appfiles[1];
	};
typedef struct AppParam AppParam;
*/	
	
void tryThruFinderFiles();
void tryThruFinderFiles()
{
	char 	**myargv, * cmdline, *apName, *fileName, *sin, *sout, *serr;
	int		myargc;	
	short n, i, err, apRefNum;
	Handle  apParam;
		
	myargc= 0;
	myargv= NULL;
	sin= sout= serr= NULL;
	i= 0; n= 0; apParam= NULL;
	fileName= NULL;
	if (gFinderFile!=NULL) { n= 1; fileName= gFinderFile; }
	
	if (n>0) {
 		i= 1;  /* only read 1st file == command line info */
		{
			enum { kBufsize = 1024 };
			short j, fref, len;
			long count;
			char buf[kBufsize];
			Handle path;
			FILE * fp;
			char * arg;
						
#ifdef DEBUG
fprintf(stdout, "fopen: %s \n", fileName);
#endif

			fp= fopen( fileName, "r");
	    myargv= (char**) malloc(  (myargc+3) * sizeof(char*));
	    
			while (!feof(fp)) {
				buf[0]= 0;
				fgets( buf, sizeof(buf), fp);

#ifdef DEBUG
fprintf(stdout, "fgets: '%s'", buf);
#endif
				count= strlen(buf);
				if (count>1) {
					--count; /* drop newline ? test for it? */
					arg= (char*) malloc(count+1);
					memcpy( arg, buf, count);  
					arg[count]= 0;
					     if (strncmp(arg,"stdin=", 6)==0) sin= arg+6;
					else if (strncmp(arg,"stdout=",7)==0) sout= arg+7;
					else if (strncmp(arg,"stderr=",7)==0) serr= arg+7;
					else {
						myargv[myargc++]= arg;
						myargv= (char**) realloc(myargv, (myargc+3) * sizeof(char*));
						}
					}
				}
		}
			
		myargv[myargc]= NULL;
		displayCommandline(myargv[0], myargv[1], myargv[2], myargv[3]);
		err= callMain( myargc, myargv, sin, sout, serr);
		}
}



void initMac();
void initMac()
{
	MaxApplZone();
	InitGraf((Ptr)&qd.thePort);
	InitFonts();
  FlushEvents (everyEvent, 0);
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();
}





int main(int argc, char *argv[])
{
	int err= 0;
	initMac();
	
#ifdef DEBUG
  freopen( "ChildApp.stdout", "w", stdout);  
  fprintf(stdout, "+debug: Test output from ChildApp.c --\n+debug: where is stdout going to??\n");
#endif

	InitSpecialEvents();
	getAEStartEvents();
	tryThruFinderFiles();
	
	tryThruAppEvent();
			/* if we are still here, run as standard commandline app */
 
#ifdef DEBUG
  /* freopen( null, "w", stdout); */
#endif
	while (true) 
	{  
  	int myargc;
  	char 	**myargv;
  	
  	myargc = ccommand( &myargv);
		err= RealMain( myargc, myargv);
		fflush(stdout);
	}
 childExit(err); /* ?? */
 return 0;	 
}


 
 

short	FSpGetFullPath(const FSSpec *spec, short *fullPathLength, Handle *fullPath);
short	FSpGetFullPath(const FSSpec *spec, short *fullPathLength, Handle *fullPath)
{
	short		result;
	FSSpec		tempSpec;
	CInfoPBRec	pb;
	*fullPathLength = 0;
	*fullPath = NULL;
	/* Make a copy of the input FSSpec that can be modified */
	BlockMoveData(spec, &tempSpec, sizeof(FSSpec));
	
	if ( tempSpec.parID == fsRtParID ) {
		/* The object is a volume */
		
		/* Add a colon to make it a full pathname */
		++tempSpec.name[0];
		tempSpec.name[tempSpec.name[0]] = ':';
		
		/* We're done */
		result = PtrToHand(&tempSpec.name[1], fullPath, tempSpec.name[0]);
		}
	else {
		/* The object isn't a volume */
		
		/* Is the object a file or a directory? */
		pb.dirInfo.ioNamePtr = tempSpec.name;
		pb.dirInfo.ioVRefNum = tempSpec.vRefNum;
		pb.dirInfo.ioDrDirID = tempSpec.parID;
		pb.dirInfo.ioFDirIndex = 0;
		result = PBGetCatInfoSync(&pb);
		if ( result == noErr )
		{
			/* if the object is a directory, append a colon so full pathname ends with colon */
			if ( (pb.hFileInfo.ioFlAttrib & ioDirMask) != 0 )
			{
				++tempSpec.name[0];
				tempSpec.name[tempSpec.name[0]] = ':';
			}
			
			/* Put the object name in first */
			result = PtrToHand(&tempSpec.name[1], fullPath, tempSpec.name[0]);
			if ( result == noErr )
			{
				/* Get the ancestor directory names */
				pb.dirInfo.ioNamePtr = tempSpec.name;
				pb.dirInfo.ioVRefNum = tempSpec.vRefNum;
				pb.dirInfo.ioDrParID = tempSpec.parID;
				do	/* loop until we have an error or find the root directory */
				{
					pb.dirInfo.ioFDirIndex = -1;
					pb.dirInfo.ioDrDirID = pb.dirInfo.ioDrParID;
					result = PBGetCatInfoSync(&pb);
					if ( result == noErr )
					{
						/* Append colon to directory name */
						++tempSpec.name[0];
						tempSpec.name[tempSpec.name[0]] = ':';
						
						/* Add directory name to beginning of fullPath */
						(void) Munger(*fullPath, 0, NULL, 0, &tempSpec.name[1], tempSpec.name[0]);
						result = MemError();
					}
				} while ( (result == noErr) && (pb.dirInfo.ioDrDirID != fsRtDirID) );
			}
		}
	}
	if ( result == noErr )
	{
		/* Return the length */
		*fullPathLength = InlineGetHandleSize(*fullPath);
	}
	else
	{
		/* Dispose of the handle and return NULL and zero length */
		if ( *fullPath != NULL )
		{
			DisposeHandle(*fullPath);
		}
		*fullPath = NULL;
		*fullPathLength = 0;
	}
	return ( result );
}
 
 
short	GetFullPath(short vRefNum, long dirID, ConstStr255Param name, 
							short *fullPathLength, Handle *fullPath)
{
	short		result;
	FSSpec  spec;
	*fullPathLength = 0;
	*fullPath = NULL;
	result = FSMakeFSSpec(vRefNum, dirID, name, &spec);
	if ( result == noErr )  
		result = FSpGetFullPath(&spec, fullPathLength, fullPath);
	return ( result );
}

