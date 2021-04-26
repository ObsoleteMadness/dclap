/* Dvibrant.c */
/* dgg additions to ncbi/vibrant */


#include "Dvibrant.h"

#ifdef COMP_CWI
#define _asm  asm
#endif

#include <ncbi.h>
#include <ncbidraw.h>
#include <ncbiwin.h>
#include <ncbiport.h>
#include <vibincld.h>

void Dgg_PenNormal()  /* nlmdraw addition */
{
	Nlm_Black();
	Nlm_Solid();
	Nlm_WidePen(1); /* << !? need Nlm_PenNormal() */
}

extern void Dgg_DrawTextbox (Nlm_RectPtr r, Nlm_CharPtr text,
                          	 ulong len, Nlm_Char jst, Nlm_Boolean gray)
{
/* from Nlm_DrawText which mostly does this, but XWin XDrawString primitive doesn't know about
   multiline text */
   
#ifdef WIN_MAC
  Nlm_Int2      just;
  PenState      pnState;
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_EraseRect (r);
    if (text != NULL && len > 0) {
      Nlm_RecTToRectTool (r, &rtool);
      switch (jst) {
        case 'r':
          just = -1;
          break;
        case 'l':
          just = 0;
          break;
        case 'c':
          just = 1;
          break;
        default:
          just = 0;
          break;
      }
      TextBox (text, len, &rtool, just);
      if (gray) {
        GetPenState (&pnState);
        PenMode (patBic);
        /*PenPat ((ConstPatternParam) grayPat);*/
				Nlm_Medium();
        PaintRect (&rtool);
        SetPenState (&pnState);
      }
    }
  }
#endif
#ifdef WIN_MSWIN
  Nlm_Int2      format;
  Nlm_Int4      oldcolor;
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentHDC != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    if (Nlm_currentHWnd != NULL) {
      Nlm_EraseRect (r);
    } else {
      FillRect (Nlm_currentHDC, &rtool, GetStockObject (WHITE_BRUSH));
    }
    if (text != NULL && len > 0) {
      switch (jst) {
        case 'r':
          format = DT_RIGHT;
          break;
        case 'l':
          format = DT_LEFT;
          break;
        case 'c':
          format = DT_CENTER;
          break;
        default:
          format = DT_LEFT;
          break;
      }
      if (gray) {
        oldcolor = SetTextColor (Nlm_currentHDC, GetSysColor (COLOR_GRAYTEXT));
      }
      DrawText (Nlm_currentHDC, text, (Nlm_Int2) len, &rtool,
                format | DT_VCENTER | DT_NOPREFIX);
      if (gray) {
        SetTextColor (Nlm_currentHDC, oldcolor);
      }
    }
  }
#endif

#ifdef WIN_MOTIF
  Nlm_Int2   delta;
  Nlm_Int2   height;
  Nlm_Int2   limit;
  Pixmap     pix;
  Nlm_PoinT  pt;
  Nlm_Int2   width;

  if (r != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXWindow != 0 && Nlm_currentXGC != NULL) {
    Nlm_EraseRect (r);
    if (text != NULL && len > 0) {
			Nlm_Int4  widlimit, pixwid, linewid, nlines;
			Nlm_Int2	atx, liney;
			char	*cp, *cp0;
			
      pt.x = r->left - Nlm_XOffset;
      pt.y = r->top - Nlm_YOffset;
			nlines= 0;
			cp0= text;
			do {
				nlines++;
				cp= Nlm_StrChr(cp0, '\n');  
				if (cp) cp0= 1 + cp;
			} while (cp);

     	width= 0;
    	widlimit = ABS(r->right - r->left);
			limit = ABS(r->bottom - r->top);
      height = Nlm_LineHeight();
			delta = limit - nlines * height; 
			if (delta > 0) pt.y += delta / 2;   
			liney= pt.y + Nlm_Ascent();
			
			cp0= text;
	    if (gray) Nlm_Medium();
			do {
				nlines++;
				cp= Nlm_StrChr(cp0, '\n');  
				if (cp) linewid= cp - cp0;
				else linewid= len - (cp0 - text);
					
				pixwid= Nlm_TextWidth (cp0, linewid);
				if (pixwid > widlimit) {
					linewid= Min(linewid, 1 + widlimit / Nlm_MaxCharWidth());
					pixwid= Nlm_TextWidth(cp0, linewid);
					while (linewid > 0 && pixwid > widlimit) {
	        	linewid--;
	        	pixwid = Nlm_TextWidth (cp0, linewid);
	      		}
					}
				
				delta = widlimit - pixwid;
				if (delta>0) switch (jst) {
					case 'r':
						atx = pt.x + delta;
						break;
					default:
					case 'l':
						atx = pt.x;
						break;
					case 'c':
						atx = pt.x + delta / 2;
						break;
					}
				 
				XDrawString (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
										 atx, liney, cp0, (int) linewid);
				liney += height; 
				
				if (cp) {
					if (linewid < cp - cp0) cp0 += linewid; 
					else cp0 = 1+cp;
					}
				else if (cp0 + linewid < text + len) {
					cp0= cp0 + linewid;
					cp= cp0;
					}
			} while (cp);
    if (gray) Nlm_Solid();
    
    }
  }
#endif
}




extern void Dgg_SetParentWindow (Nlm_GraphiC a, Nlm_WindoW newwindow)
{
  Nlm_GraphiC  child, g, p;

  g = a;
  if (a != NULL) {
 		 child= g;
    p = Nlm_GetParent (g);
    while (p != NULL) {
  			 child= g;
      g = p;
      p = Nlm_GetParent (g);
    }
  /* g == window, child == a primary subview in window */
  if (child != NULL) Nlm_SetParent( child, (Nlm_GraphiC) newwindow);
  }
}



#ifdef WIN_MAC

/* LaunchFileThruFinder perpares and sends the Finder OpenSelection event */

#include <Processes.h>
#include <GestaltEqu.h>
#include <AppleEvents.h>
#include <Aliases.h>


		// from MacAppUtilities.cp
OSErr FindProcessBySignature(OSType sig, ProcessSerialNumber* psn, FSSpec* fileSpec)
{
	OSErr err;
	ProcessInfoRec info;

	psn->highLongOfPSN = 0;
	psn->lowLongOfPSN  = kNoProcess;
	do {
		err= GetNextProcess(psn);
		if( err == noErr ) {
			info.processInfoLength = sizeof(ProcessInfoRec);
			info.processName = NULL;
			info.processAppSpec = fileSpec;
			err= GetProcessInformation(psn, &info);
		}
	} while( (err == noErr) && info.processSignature != sig );

	if( err == noErr ) (*psn) = info.processNumber;

	return err;
}  


OSErr LaunchFileSpecThruFinder( FSSpec fileToLaunch)
{    				
	const		OSType aeSelectionKeyword	= 'fsel';
	const		OSType kAEOpenSelection		= 'sope';
	const		OSType kFinderType 				= 'FNDR';
	const		OSType kSysCreator 				= 'MACS';
	
	AppleEvent	aeEvent, aeReply;
	AEDesc			myAddressDesc, aeDirDesc, listElem;
	FSSpec			dirSpec, procSpec;
	AEDesc			fileList;
	OSErr				myErr;
	ProcessSerialNumber		finderSN;
	AliasHandle	DirAlias, FileAlias;
				
	DirAlias= NULL;
	FileAlias= NULL;
	aeEvent.dataHandle= NULL;
	
		// find the finder on this machine 
// failing here w/ no finder !?
	myErr= FindProcessBySignature(kFinderType, &finderSN, &procSpec);
//	if (myErr!=0) return procNotFound;
	if (myErr != 0) { // do the hard way...
		ProcessSerialNumber	processSN;
    ProcessInfoRec infoRec;
    Str31 processName;
    FSSpec procSpec;
    Boolean notFinder = true;

    processSN.lowLongOfPSN = kNoProcess;
    processSN.highLongOfPSN = kNoProcess;
		do {
        myErr = GetNextProcess(&processSN);
				infoRec.processInfoLength = sizeof(ProcessInfoRec);
				infoRec.processName = (StringPtr)&processName;
				infoRec.processAppSpec = &procSpec;
        GetProcessInformation(&processSN, &infoRec);
				if (infoRec.processSignature == kSysCreator 
				 && infoRec.processType == kFinderType) {
					finderSN = processSN;
					notFinder = false;
					}
			} while (notFinder && !myErr);	
		}
		
		
	myErr= AECreateDesc(typeProcessSerialNumber, (Ptr) &finderSN, sizeof(finderSN), 
												&myAddressDesc);
	if (myErr != noErr) return myErr;
	
			// Create the FinderEvent 
	myErr= AECreateAppleEvent(kFinderType, kAEOpenSelection, &myAddressDesc, 
											kAutoGenerateReturnID, kAnyTransactionID, &aeEvent);
	if (myErr != noErr) return myErr;
	
	// Now we build all the bits of an OpenSelection event. 
	//  Basically, we need to create an alias for the item to open, and an alias to the parent 
	// folder (directory) of that item.  We can also pass a list of files if we want.  
	// Youll notice that for opening a finder window, the file and directory alias both point at the 
	// folder itself. Make a spec for the parent folder  
	 
		//  create the folder descriptor 
	myErr= FSMakeFSSpec( fileToLaunch.vRefNum, fileToLaunch.parID, NULL, &dirSpec);
	
	myErr= NewAlias( NULL, &dirSpec, &DirAlias);
	HLock( (Handle) DirAlias);
	myErr= AECreateDesc(typeAlias, (Ptr) *DirAlias, GetHandleSize( (Handle) DirAlias), 
								&aeDirDesc);
	HUnlock( (Handle)DirAlias);
	DisposHandle( (Handle) DirAlias);
	
	myErr= AEPutParamDesc( &aeEvent, keyDirectObject, &aeDirDesc);
	if (myErr != noErr) return myErr;
	myErr= AEDisposeDesc( &aeDirDesc);
	
			// create the file descriptor and add to aliasList 
	myErr= NewAlias( NULL, &fileToLaunch, &FileAlias);
	HLock( (Handle)FileAlias);
	myErr= AECreateDesc(typeAlias, (Ptr)*FileAlias, GetHandleSize( (Handle)FileAlias), 
									&listElem);
	HUnlock( (Handle)FileAlias);
	DisposHandle( (Handle)FileAlias);
	
		// Create the file  list 
	myErr= AECreateList(NULL, 0, false, &fileList);
	if (myErr != noErr) return myErr;
	
	myErr= AEPutDesc( &fileList, 0, &listElem);
	(void) AEDisposeDesc( &listElem);
	if (myErr != noErr) return myErr;
	
		// Add the file alias list to the event 
	myErr= AEPutParamDesc( &aeEvent, aeSelectionKeyword, &fileList);
	(void) AEDisposeDesc( &fileList);
	if (myErr != noErr) return myErr;

	myErr= AESend( &aeEvent, &aeReply, kAENoReply + kAEAlwaysInteract + kAECanSwitchLayer,
										 kAENormalPriority, kAEDefaultTimeout, NULL, NULL);
		
	(void) AEDisposeDesc( &aeEvent);
	return myErr;
}

#endif



const char*  MacDirIDtoName( short vRefNum, long dirID, char* fname)
{
#ifdef WIN_MAC
	static char	directory[PATH_MAX];
  char 	temp[PATH_MAX], * tmp;
  CInfoPBRec   params;
	short		err;
	
  temp[0] = '\0';
  params.dirInfo.ioNamePtr = (StringPtr) directory;
  params.dirInfo.ioDrParID = dirID;
  do {
    params.dirInfo.ioVRefNum = vRefNum;
    params.dirInfo.ioFDirIndex = -1;
    params.dirInfo.ioDrDirID = params.dirInfo.ioDrParID;
    err = PBGetCatInfo (&params, FALSE);
#ifdef COMP_THINKC
		PtoCstr ((StringPtr) directory);
#else
		p2cstr ((StringPtr) directory);
#endif
    Nlm_StringCat(directory, DIRDELIMSTR);
    Nlm_StringCat(directory, temp);
    Nlm_StringCpy(temp, directory);
  } while (params.dirInfo.ioDrDirID != fsRtDirID);

  tmp = Nlm_StringMove(directory, temp);
  tmp = Nlm_StringMove(tmp, fname);
  return directory;
#else
	return NULL;
#endif
} 



typedef void (*FileFunc)(const char*);

short HandleFileEvent( long inEvent, long outReply, long inNumber,
											long fileFunc)
{    				
#ifdef WIN_MAC
	AppleEvent	*aeEvent, *aeReply;
	AEDescList	docList;			
	short				err;
	long				nitems, item;
	FileFunc	* callFileFunc;
	
	if (!inEvent || !fileFunc) return;
	aeEvent = (AppleEvent*) inEvent;
	aeReply = (AppleEvent*) outReply;
	callFileFunc = (FileFunc*) fileFunc;
	
	err= AEGetParamDesc( aeEvent, keyDirectObject, typeAEList, &docList);
	if (err) return;
	err= AECountItems( &docList, &nitems);
	if (err) return;
	for (item= 1; item<=nitems; item++) {
		AEKeyword		keywd;
		DescType		returnedType;
		FSSpec			aFileSpec;
		Size				actualSize;
		const char * pathname;
		
		err= AEGetNthPtr( &docList, item, typeFSS, &keywd, &returnedType,
											(void*)&aFileSpec, sizeof(aFileSpec), &actualSize);
		if (err) break; // continue ?
		
#ifdef COMP_THINKC
		PtoCstr ((StringPtr) aFileSpec.name);
#else
		p2cstr ((StringPtr) aFileSpec.name);
#endif
		pathname= MacDirIDtoName( aFileSpec.vRefNum, aFileSpec.parID,
														 (char*) aFileSpec.name);
		(*callFileFunc)( pathname); // this->OpenDocument( filename);
		}
	(void) AEDisposeDesc( &docList);
	return err;
#else
	return -1;
#endif
}



#ifdef WIN_MAC
/* quick hack to check child apps by psn */
ProcessSerialNumber  gChildProcessSN;
#else
long 		gChildProcessSN;
#endif


char* Fullpathname( char* filename, Nlm_Boolean forceprogpath)
{
	static char	pathname [512]; /* ?? is there a 256 char limit on mac paths ?? */
  Nlm_CharPtr          ptr;
  Nlm_Boolean addpath;
  
  addpath= forceprogpath 
    || (Nlm_StringChr(filename, DIRDELIMCHR) == NULL);
#ifdef WIN_MSWIN
  if (filename[1] == ':' || filename[0] == DIRDELIMCHR)
    addpath= false;
#endif
#ifdef OS_UNIX
  if (filename[0] == DIRDELIMCHR) addpath= false;
#endif

	if (!addpath) 
		Nlm_StringNCpy ((Nlm_CharPtr) pathname, filename, sizeof (pathname));
	else {
		Nlm_ProgramPath((Nlm_CharPtr) pathname, sizeof (pathname));
		ptr = Nlm_StringRChr((Nlm_CharPtr) pathname, DIRDELIMCHR);
		if (ptr)  *ptr = '\0';
		Nlm_FileBuildPath((Nlm_CharPtr) pathname, NULL, filename);
		}
	return pathname;
}



extern Nlm_Boolean Dgg_LaunchApp( char* appName, char* commandline,
													char* Stdin, char* Stdout, char* Stderr)
{
#ifdef WIN_MAC
  OSErr 							 err;
  FSSpec               fsspec;
  long                 gval;
  LaunchParamBlockRec  lparm;
	//unsigned char *      pathname[256];  
  Nlm_CharPtr          pathname, ptr;
  Nlm_Boolean          rsult;

  rsult = FALSE;
  if (Gestalt (gestaltSystemVersion, &gval) == noErr && (short) gval >= 7 * 256) {
    if (appName && *appName) {
			pathname= Fullpathname( appName, false);
			Nlm_CtoPstr((Nlm_CharPtr) pathname);
			err = FSMakeFSSpec(0, 0, (unsigned char*)pathname, &(fsspec));
			
			if (Stdin || Stdout || Stderr) {
				if (!commandline) commandline= MemNew(1);
				if (Stdin) {
					pathname= Fullpathname(Stdin,false);
					commandline= MemMore( commandline, StrLen(commandline) + 6 + StrLen(pathname));
					StrCat( commandline, " < '");
					StrCat( commandline, pathname);
					StrCat( commandline, "'");
					}
				if (Stdout) {
					pathname= Fullpathname(Stdout,false);
					commandline= MemMore( commandline, StrLen(commandline) + 6 + StrLen(pathname));
					StrCat( commandline, " > '");
					StrCat( commandline, pathname);
					StrCat( commandline, "'");
					}
				if (Stderr) {
					pathname= Fullpathname(Stderr,false);
					commandline= MemMore( commandline, StrLen(commandline) + 6 + StrLen(pathname));
					StrCat( commandline, " 2> '");
					StrCat( commandline, pathname);
					StrCat( commandline, "'");
					}
				}
				
			if (commandline) {
				/* only good for launching an application, not a document... */
				EventRecord hlevt;
  			AppParametersPtr appParams;
				long	len;

				MemSet( &lparm, 0, sizeof(lparm));
  			len= 2 + StrLen(appName) + StrLen( commandline);
				hlevt.what= kHighLevelEvent;
				hlevt.message= 'DClp'; // ID of sender == DClap application
				hlevt.where.v= 'Cm';  
				hlevt.where.h= 'dl'; //(Point)'Cmdl'; 	// command line event type
				
  			appParams= (AppParametersPtr) MemNew( sizeof(AppParameters) + len + 2);
				appParams->theMsgEvent= hlevt;
				appParams->eventRefCon= 'Cmdl'; // ?? again
				ptr= (char*) &(appParams->messageLength) + sizeof(long);
				StrCpy( ptr, appName);
				StrCat( ptr, " ");
				StrCat( ptr, commandline); 
				appParams->messageLength= len;
				lparm.launchAppParameters = appParams;
					
				lparm.launchBlockID = extendedBlock;
				lparm.launchEPBLength = extendedBlockLen;
				lparm.launchFileFlags = 0;
				lparm.launchControlFlags = launchContinue + launchNoFileFlags;
				lparm.launchAppSpec = &fsspec;
				err= LaunchApplication (&lparm);
				rsult= (err == noErr);
				gChildProcessSN= lparm.launchProcessSN;
				
				MemFree( appParams);
				}
			else {
 				rsult = (Nlm_Boolean) (LaunchFileSpecThruFinder( fsspec)  == noErr);
 				}
    	}
  	}
  return rsult;
#endif

#ifdef WIN_MSWIN
  Nlm_Char     ch;
  Nlm_Int2     i, j, k;
  Nlm_Uint2		 val;
  char         lpszCmdLine[256];
  char      *  pathname, *ptail;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  if ((appName && *appName) || (commandline && *commandline)) {
  
		if (Stdin || Stdout || Stderr) {
			if (!commandline) commandline= MemNew(1);
			if (Stdin) {
				pathname= Fullpathname(Stdin,false);
				commandline= MemMore( commandline, StrLen(commandline) + 6 + StrLen(pathname));
				StrCat( commandline, " < ");
				StrCat( commandline, pathname);
				}
			if (Stdout) {
				pathname= Fullpathname(Stdout,false);
				commandline= MemMore( commandline, StrLen(commandline) + 6 + StrLen(pathname));
				StrCat( commandline, " > ");
				StrCat( commandline, pathname);
				}
			if (Stderr) {
				pathname= Fullpathname(Stderr,false);
				commandline= MemMore( commandline, StrLen(commandline) + 6 + StrLen(pathname));
				StrCat( commandline, " 2> ");
				StrCat( commandline, pathname);
				}
			}

#ifdef WIN16
		*lpszCmdLine= 0;
		if (appName) { 
	    Nlm_StringNCpy( lpszCmdLine, appName, sizeof(lpszCmdLine));
	    Nlm_StringNCat( lpszCmdLine, " ", sizeof(lpszCmdLine));
	    }
    Nlm_StringNCat( lpszCmdLine, commandline, sizeof(lpszCmdLine));
    
    val= WinExec (lpszCmdLine, SW_SHOW);
    gChildProcessSN= val;
    rsult= (val >= 32);
#else
    {
    STARTUPINFO  si;
    PROCESS_INFORMATION pri;
    
    memset(&pri, 0, sizeof(pri));
    memset(&si, 0, sizeof(si));
    si.cb= sizeof(si); /* !?? */
    si.wShowWindow= SW_SHOW;
#if 0
		if (Stdin)  { freopen( Stdin, "r", stdin); si.hStdInput= stdin; }
		if (Stdout) { freopen( Stdout, "a", stdout); si.hStdOutput= stdout; }
		if (Stderr) { freopen( Stderr, "a", stderr); si.hStdError= stderr; }
    si.dwFlags= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 
#else
    si.dwFlags= 0; 
#endif

#if 0
	  pathname= Fullpathname(appName,false);
	  ptail = pathname + StrLen(pathname) - 4;
	  if (StrICmp( ptail, ".exe") != 0)  StrCat( pathname, ".exe");
    Nlm_StringNCpy( lpszCmdLine, commandline, sizeof(lpszCmdLine));
    /* ! cmdline redirects are being intercepted by win-os ! */
    rsult= CreateProcess( pathname, lpszCmdLine, NULL, NULL, true, 
#else
		*lpszCmdLine= 0;
		if (appName) { 
	    Nlm_StringNCpy( lpszCmdLine, appName, sizeof(lpszCmdLine));
	    Nlm_StringNCat( lpszCmdLine, " ", sizeof(lpszCmdLine));
	    }
    Nlm_StringNCat( lpszCmdLine, commandline, sizeof(lpszCmdLine));

    rsult= CreateProcess( NULL, lpszCmdLine, NULL, NULL, true, 
#endif
            DETACHED_PROCESS,/*CREATE_NEW_CONSOLE, */ 
            NULL/*Env*/, 
            NULL/*CurDir*/, 
            &si, &pri);
    if (rsult) {
      gChildProcessSN= (long) pri.dwProcessId; /* hProcess ; */
      }
    }
#endif
  }
  return rsult;
#endif


#ifdef WIN_MOTIF
  Nlm_Boolean  rsult;
  long  			status;

  rsult = FALSE;
  if ((appName && *appName) || (commandline && *commandline)) {
    int	  	narg = 0, done;
    char		*cmdline, **argv, *cp, *ep;
    ulong 	len;
    
		len= StrLen(appName) + StrLen(commandline) + 3;
		cmdline= MemNew(len);
		*cmdline= 0;
		if (appName) { 
			Nlm_StringCpy( cmdline, appName);
			Nlm_StringCat( cmdline, " ");
			}
		Nlm_StringCat( cmdline, commandline);
        
   	/* need to pop the cmdline args into a vector !*/
    cp= cmdline; done= FALSE;
    argv= (char**) Nlm_MemNew(sizeof(Nlm_CharPtr));
    while (!done) {
       while (*cp && *cp <= ' ') cp++;
       if (*cp == '"') { for (ep= cp+1; *ep && *ep != '"'; ep++) ; }
       else if (*cp == '\'') { for (ep= cp+1; *ep && *ep != '\''; ep++) ; }
       else if (*cp) { for (ep= cp+1; *ep && *ep>' '; ep++) ; }
       if (*ep) *ep= '\0';  else done= TRUE;
       narg++;
       argv= (char**) Nlm_MemMore(argv, (narg+1) * sizeof(Nlm_CharPtr));
/*xdebugp("Dgg_LaunchFile argv[i]= %s\n",cp);*/
       argv[narg-1]= cp;
       cp= ep+1;
       }
    argv[narg]= NULL;

#ifdef OS_UNIX
    status =  fork();
    if (status == 0) {
    	/* if status==0, we are now the child process */
			if (Stdin) { freopen( Stdin, "r", stdin); }
			if (Stdout) { freopen( Stdout, "a", stdout); }
			if (Stderr) { freopen( Stderr, "a", stderr); }
#if 0
/* DEBUG */
			fprintf( stdout, "\n This is new stdout=%s\n",Stdout);
			fprintf( stderr, "\n This is new stderr=%s\n",Stderr);
			fprintf( stdout, "\n Dgg_LaunchApp(%s,%s,\n    %s,%s,%s);\n",
					appName,commandline, Stdin, Stdout, Stderr);
			fprintf( stdout, " arg[0]=%s\n",argv[0]);
			fflush( stdout); 
			fflush( stderr);
			/*_exit(-1); */		
#endif
      execvp( argv[0], argv);
      /* if (stillhere) Nlm_Message (MSG_FATAL, "Application launch failed"); */
      _exit(-1); /* app launch failed if we get here... */
    	} 
    else if (status != -1) {
    	/* here status == child process id, need to save for child app handler */
    	gChildProcessSN= status;
      rsult = TRUE;
    	}
    else
   		Nlm_Message (MSG_OK, "Application launch failed");  

#endif
#ifdef OS_VMS
    	/* ?? for VMS -- how do we change child process stdin/stdout/stderr ?? */
    status = execvp( argv[0], argv);
    if ( status == -1 ) {
      /*Nlm_Message (MSG_FATAL, "Application launch failed"); */
    	} 
    else {
    	gChildProcessSN= status;
    	/* here status == child process id, need to save for child app handler */
      rsult = TRUE;
    	}
#endif
    Nlm_MemFree(cmdline);
  }
  return rsult;
#endif
}
