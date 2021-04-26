/* ChildApp.c
	 d.g.gilbert, Dec'94
	 
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

void  BeFriendly();   // use for multiprocessing
Boolean StopKey();		// check if user wants to quit in middle



#ifdef DEBUG
char* long2str( long val)
{
	static char buf[4];
  buf[0]= (val >> 24) & 0xff;
  buf[1]= (val >> 16) & 0xff;
  buf[2]= (val >> 8) & 0xff;
  buf[3]= val & 0xff;
	return buf;	
}
#endif

char* FindWordEnd( char** start)
{
	char *ep, *cp;
	ep= cp= *start;
	if (*cp == '"' || *cp == '\'') { 
		char key= *cp;
		cp++; 
		(*start)++; // need to move word start up 1
		for (ep= cp; *ep && *ep != key; ep++) ; 
		}
	else if (*cp) { 
		for (ep= cp+1; 
			*ep && *ep>' ' && *ep!='>' && *ep!='<' && *ep != '"' && *ep != '\''; 
			ep++) ; 
		}
	return ep;
}   

char* FindWordStart( char* cp)
{
	while (*cp && *cp <= ' ') cp++;
	return cp;
}


main(int argc, char *argv[])
{
	enum { kMaxBuflen = 512 };
	Boolean	doloop = true, flag;
	char 	**myargv, * cmdline;
	int		i, err, try, myargc = argc;	
	long	time;
	unsigned long msgRefcon, buflen;
	EventRecord evt;
	TargetID	sender;

		// need this to prevent crashes
	MaxApplZone();
	InitGraf((Ptr)&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();

#ifdef DEBUG
  freopen( "ChildApp.stdout", "w", stdout);  
  fprintf(stdout, "+debug: Test output from ChildApp.c --\n+debug: where is stdout going to??\n");
#endif

	try= 0;
	buflen= kMaxBuflen;
	cmdline= (char*) malloc(buflen+1);
	memset(cmdline,0,buflen+1);
	while (WaitNextEvent( highLevelEventMask, &evt, 2, NULL) || try<3)  { 
		try++;	
		err= AcceptHighLevelEvent( &sender, &msgRefcon, cmdline, &buflen);

#ifdef DEBUG
		fprintf(stderr, " HL Event %s ", long2str(evt.message));
		fprintf(stderr, " HL message %s ", long2str(msgRefcon));
		fprintf(stderr, " error= %d \n", err);
#endif	
	
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
	    
#ifdef DEBUG 
fprintf(stderr, " Cmdline: '%s' \n", cmdline);
#endif
	    while (!done && cp<maxp) {
	    	cp= FindWordStart( cp);
				ep= FindWordEnd( &cp);
				
#ifdef DEBUG
savec= *ep; *ep= 0; 
fprintf(stderr, " +cmdline word: '%s' \n", cp);
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

#ifdef DEBUG
fprintf(stderr, " +comment: stderr '%s' \n", serr);
if (sout) fprintf(stderr, "+comment: new stdout is '%s'\n",sout);
if (serr) fprintf(stderr, "+comment: new stderr is '%s'\n",serr);
if (sin) fprintf(stderr, "+comment: new stdin is '%s'\n",sin);
for (i=0; i<myargc; i++) fprintf(stderr, "arg[%d]= '%s'\n", i, myargv[i]);
#endif
	    if (sin) freopen( sin, "r", stdin);
	    if (serr) freopen( serr, "a", stderr);  
			if (sout) freopen( sout, "a", stdout);  //where oh where is stdout going !????
			
			err= RealMain(myargc, myargv);

#ifdef DEBUG
fprintf(stdout, "+debug: Test stdout from ChildApp.c --\n+debug: where is stdout going to??\n");
fprintf(stderr, "+error: Test stderr from ChildApp.c --\n+error: where is stderr going to??\n");
#endif
			//fflush(stdout);
			//fclose(stdout); //??
			exit(0);   
			}
		exit(0);  
		}
	}

#if 1		
	do {
  	myargc = ccommand( &myargv);
		
		err= RealMain( myargc, myargv);
		fflush(stdout);
		} while (doloop);
		
#endif
}



Boolean StopKey()
{	
EventRecord	ev;

	if (EventAvail( keyDownMask+autoKeyMask, &ev)) {
	  if (  (ev.modifiers & cmdKey)  
	   && ((char)(ev.message & charCodeMask) == '.') ) {
	  	  SysBeep(1);
		  (void) GetNextEvent( keyDownMask+autoKeyMask, &ev);
		  return true;
		  }
	  }
	return false;
}

Boolean Keypress()
{	EventRecord	ev;
	return EventAvail( keyDownMask+keyUpMask+autoKeyMask, &ev);
}

/******
From dowdy@apple.com Wed Sep  4 21:06:10 1991
Received: from apple.com by sunflower.bio.indiana.edu
        (4.1/9.5jsm) id AA04821; Wed, 4 Sep 91 21:06:08 EST
Received: from [90.10.20.25] by apple.com with SMTP (5.61/25-eef)
        id AA07198; Wed, 4 Sep 91 19:06:53 -0700
        for gilbertd@sunflower.bio.indiana.edu
Date: Wed, 4 Sep 91 19:06:53 -0700
Message-Id: <9109050206.AA07198@apple.com>
From: dowdy@apple.com (Tom Dowdy)
To: gilbertd@sunflower.bio.indiana.edu
Subject: Re: MPW SIOW library
References: <1991Sep1.174155.13408@gn.ecn.purdue.edu> <6260@dftsrv.gsfc.nasa.gov
> <1991Sep4.233506.25878@bronze.ucs.indiana.edu>
Organization: Apple Computer, Inc.
Status: R

In article <1991Sep4.233506.25878@bronze.ucs.indiana.edu>, gilbertd@sunflower.bi
o.indiana.edu (Don Gilbert) writes:
> In article <6260@dftsrv.gsfc.nasa.gov> rdominy@kong.gsfc.nasa.gov (Robert Domi
ny) writes:
> >
> >I have tried calling the routine EventAvail from an SIOW application to
> >provide background processing time without success.  I suspect you
>
> I just put a commandline c program into SIOW and sprinkled some calls
> to WaitNextEvent thru the time consuming procedures.  It seems to work
> okay, though a bit cranky, for getting the app to work in the background.
> When I say cranky, it will miss some os events telling it to shift into
> background, but if I beat the mouse on the desktop, it will eventually
> shift into background.

This is just a guess, but the process manager will continue to call
you with update events if you fail to handle them.  After you have
ignore something like 120 requests to update a window of yours,
the PM will assume you are a "MultiFinder hostile" application
and stop giving them to you until the update rgn changes.

I'm wondering if this is causing your initial strangeness.  I know
a fellow programmer here got himself into the same fix and
this ended up being the reason he saw his application perform so slowly
in the background.  Once the events were cleared, things were fine again.

 Tom Dowdy                 Internet:  dowdy@apple.COM
 Apple Computer MS:81EQ    UUCP:      {sun,voder,amdahl,decwrl}!apple!dowdy
 20525 Mariani Ave         AppleLink: DOWDY1
 Cupertino, CA 95014
 "The 'Ooh-Ah' Bird is so called because it lays square eggs."
***************/                                                                  
																																
void  BeFriendly(void)
/* make this app multifinder-friendly w/ background processing */
/* this can slow the app down quite a bit -- don't use too liberally */
{
	const int cmdKey = 256;  /* command-period == halt */
	const long charCodeMask = 0x000000FF;
	int	eventMask = osMask; /*? +keyDownMask+highLevelEventMask*/
	long	sleepTime	= 1; /* # x 1/60 to sleep for others */
	EventRecord	theEvent;
	
	if (WaitNextEvent( eventMask,	&theEvent, sleepTime, NULL)) {  
	/*****
		if (theEvent.what==keyDown) { 
			if ((theEvent.message & charCodeMask == '.') 
				&& (theEvent.modifiers & cmdKey !=0)) 
					exit(1); 
			}
		******/
		}
	/* SpinCursor(1); */ /* !!!? not seeing this, need .rsrc ACUR cursor ? */ 
}

 