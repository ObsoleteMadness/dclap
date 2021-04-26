/* childmain.c : main implementation file
//
// for SeqPup child apps (Mac and MSWin)
//
*/

#define WinTitle  "ChildApp"
#define AppPrompt "SeqPup child application"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef DEBUG

extern void RealMain( int argc, char *argv[]);

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


void main(int argc, char *argv[])
{
 	 char  *cp, *ep, *sin, *sout, *serr;
   int   newargc, i;
#ifdef DEBUG
   FILE* ferr;
   ferr= fopen("childbug.txt","w");
#endif

#ifdef _WINIO
    /* for Win16 Console Applications, _WINIO is defined */
    /* By default, Winio sets the title bar to the exe name, use winio_settitle to change it */
    winio_settitle(__hMainWnd, WinTitle);
    /* Your about box by default shows the exe name, you can change it by resetting __szModule 
       and passing the reset value to winio_about */
    strcpy (__szModule, WinTitle);
    winio_about(__szModule);
#endif

 	    sout= sin= serr= NULL;

      for (i= 0, newargc= 0; i<argc; i++) {
        cp= argv[i];
#ifdef DEBUG
fprintf(ferr, "arg[%d] = '%s'\n", i, argv[i]);
#endif
        if (*cp == '2' && cp[1] == '>') {
          if (!newargc) newargc= i;
          cp += 2;
          if (*cp) serr= cp;
          else serr= argv[++i];
          }
       	else if (*cp == '>') {
          if (!newargc) newargc= i;
					cp++;
					if (*cp) sout= cp;
					else sout= argv[++i];
       		}
       	else if (*cp == '<') {
          if (!newargc) newargc= i;
					cp++;
					if (*cp) sin= cp;
					else sin= argv[++i];
       		}
        }
      
      if (newargc) argc= newargc;
 	    if (sin)  freopen( sin, "r", stdin);
	    if (serr) freopen( serr, "a", stderr);  
			if (sout) freopen( sout, "a", stdout);  
#ifdef DEBUG
 fprintf(ferr,"argc= %d, newargc= %d",argc, newargc);
 if (sin ) fprintf(ferr," stdin=%s",sin);
 if (serr) fprintf(ferr," stderr=%s",serr);
 if (sout) fprintf(ferr," stdout=%s",sout);
 fprintf(ferr,"\n");
 fclose(ferr);
#endif
			RealMain(argc, argv);

			if (sout) fflush(stdout);
      if (serr) fflush(stderr);	
	
#ifdef _CONSOLE  
	/* for Win32 Console Applications, _CONSOLE is defined */
   if (!(sin || sout || serr)) {
     int  c;
     fprintf (stderr,"press Return or Ctrl-C to exit: ");
     c = getc(stdin); /* this is okay from seqpup call, but missed from mswin startup */
     fprintf(stderr,"...  bye.\n");
     }
#endif
}

