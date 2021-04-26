
from lynx:

/* universal document id types */
#define HTTP_URL_TYPE     1
#define FILE_URL_TYPE     2
#define FTP_URL_TYPE      3
#define WAIS_URL_TYPE     4
#define PROSPERO_URL_TYPE 5
#define NEWS_URL_TYPE     6
#define TELNET_URL_TYPE   7
#define TN3270_URL_TYPE   8
#define GOPHER_URL_TYPE   9
#define HTML_GOPHER_URL_TYPE 10
#define TELNET_GOPHER_URL_TYPE 11
#define INDEX_GOPHER_URL_TYPE 12
#define AFS_URL_TYPE      13
#define MAILTO_URL_TYPE   14
#define RLOGIN_URL_TYPE   15

#define NEWSPOST_URL_TYPE      19

// convert to Protocol enums ??
enum	urltypes {
	kUnknownUrl = 0,
	kFileUrl,
	kFtpUrl,
	kTelnetUrl,
	kTN3270Url,
	kGopherUrl,
	kMailtoUrl,
	kHttpUrl,
	kWaisUrl,
	kNewsUrl,
	kNewspostUrl,
	kProsperoUrl,
	kAfsUrl,
	kRloginUrl
	};
	
	
short IsURL(char* name)
{	// from Lynx
    char *cp= name;

    /* don't crash on an empty argument */
    if (cp == NULL || *cp == '\0')
        return(kUnknownUrl);

    /* kill beginning spaces */
    while(isspace(*cp)) cp++;

    if(!strncmp(cp,"news:",5)) {
        return(kNewsUrl);

    } else if(!strncmp(cp,"mailto:",7)) {
        return(kMailtoUrl);

    } else if(!strncmp(cp,"newspost:",9)) {
        return(kNewspostUrl);

        /* if it doesn't contain ":/" then it can't be a url
         * except for the ones above here
         */
    } else if(!strstr(cp+3,":/")) {
        return(kUnknownUrl);

    } else if(!strncmp(cp,"http",4)) {
        return(kHttpUrl);

    } else if(!strncmp(cp,"file",4)) {
        return(kFileUrl);

    } else if(!strncmp(cp,"gopher",6)) {
    	return(kGopherUrl);

    } else if(!strncmp(cp,"ftp",3)) {
        return(kFtpUrl);

    } else if(!strncmp(cp,"wais",4)) {
        return(kWaisUrl);

    } else if(!strncmp(cp,"telnet",6)) {
        return(kTelnetUrl);

    } else if(!strncmp(cp,"tn3270",6)) {
        return(kTN3270Url);

    } else if(!strncmp(cp,"rlogin",6)) {
        return(kRloginUrl);

    } else if(!strncmp(cp,"afs",3)) {
        return(kAfsUrl);

    } else if(!strncmp(cp,"prospero",8)) {
        return(kProsperoUrl);

    } else {
        return(kUnknownUrl);
    }
}



from WWW lib:

/*	Parse a Name relative to another name
**	-------------------------------------
**
**	This returns those parts of a name which are given (and requested)
**	substituting bits from the related name where necessary.
**
** On entry,
**	aName		A filename given
**      relatedName     A name relative to which aName is to be parsed
**      wanted          A mask for the bits which are wanted.
**
** On exit,
**	returns		A pointer to a malloc'd string which MUST BE FREED
*/
#ifdef __STDC__
char * HTParse(const char * aName, const char * relatedName, int wanted)
#else
char * HTParse(aName, relatedName, wanted)
    char * aName;
    char * relatedName;
    int wanted;
#endif

{
    char * result = 0;
    char * return_value = 0;
    int len;
    char * name = 0;
    char * rel = 0;
    char * p;
    char * access;
    struct struct_parts given, related;
    
    /* Make working copies of input strings to cut up:
    */
    len = strlen(aName)+strlen(relatedName)+10;
    result=(char *)malloc(len);		/* Lots of space: more than enough */
    if (result == NULL) outofmem(__FILE__, "HTParse");
    
    StrAllocCopy(name, aName);
    StrAllocCopy(rel, relatedName);
    
    scan(name, &given);
    scan(rel,  &related); 
    result[0]=0;		/* Clear string  */
    access = given.access ? given.access : related.access;
    if (wanted & PARSE_ACCESS)
        if (access) {
	    strcat(result, access);
	    if(wanted & PARSE_PUNCTUATION) strcat(result, ":");
	}
	
    if (given.access && related.access)	/* If different, inherit nothing. */
        if (strcmp(given.access, related.access)!=0) {
	    related.host=0;
	    related.absolute=0;
	    related.relative=0;
	    related.anchor=0;
	}
	
    if (wanted & PARSE_HOST)
        if(given.host || related.host) {
	    char * tail = result + strlen(result);
	    if(wanted & PARSE_PUNCTUATION) strcat(result, "//");
	    strcat(result, given.host ? given.host : related.host);
#define CLEAN_URLS
#ifdef CLEAN_URLS
	    /* Ignore default port numbers, and trailing dots on FQDNs
	       which will only cause identical adreesses to look different */
	    {
	    	char * p;
		p = strchr(tail, ':');
		if (p && access) {		/* Port specified */
		    if (  (   strcmp(access, "http") == 0
		    	   && strcmp(p, ":80") == 0 )
			||
		          (   strcmp(access, "gopher") == 0
		    	   && strcmp(p, ":70") == 0 )
		    	)
		    *p = (char)0;	/* It is the default: ignore it */
		}
		if (!p) p = tail + strlen(tail); /* After hostname */
		if (*p) {				  /* Henrik 17/04-94 */
		    p--;				/* End of hostname */
		    if (*p == '.') *p = (char)0; /* chop final . */
		}
	    }
#endif
	}
	
    if (given.host && related.host)  /* If different hosts, inherit no path. */
        if (strcmp(given.host, related.host)!=0) {
	    related.absolute=0;
	    related.relative=0;
	    related.anchor=0;
	}
	
    if (wanted & PARSE_PATH) {
        if(given.absolute) {				/* All is given */
	    if(wanted & PARSE_PUNCTUATION) strcat(result, "/");
	    strcat(result, given.absolute);
	} else if(related.absolute) {	/* Adopt path not name */
	    strcat(result, "/");
	    strcat(result, related.absolute);
	    if (given.relative) {
		p = strchr(result, '?');	/* Search part? */
		if (!p) p=result+strlen(result)-1;
		for (; *p!='/'; p--);	/* last / */
		p[1]=0;					/* Remove filename */
		strcat(result, given.relative);		/* Add given one */
		HTSimplify (result);
	    }
	} else if(given.relative) {
	    strcat(result, given.relative);		/* what we've got */
	} else if(related.relative) {
	    strcat(result, related.relative);
	} else {  /* No inheritance */
	    strcat(result, "/");
	}
    }
		
    if (wanted & PARSE_ANCHOR)
        if(given.anchor || related.anchor) {
	    if(wanted & PARSE_PUNCTUATION) strcat(result, "#");
	    strcat(result, given.anchor ? given.anchor : related.anchor);
	}
    free(rel);
    free(name);
    
    StrAllocCopy(return_value, result);
    free(result);
    return return_value;		/* exactly the right length */
}


/* masks for HTEscape */

#define URL_XALPHAS     (unsigned char) 1
#define URL_XPALPHAS    (unsigned char) 2
#define URL_PATH        (unsigned char) 4


/*		Escape undesirable characters using %		HTEscape()
**		-------------------------------------
**
**	This function takes a pointer to a string in which
**	some characters may be unacceptable unescaped.
**	It returns a string which has these characters
**	represented by a '%' character followed by two hex digits.
**
**	In the tradition of being conservative in what you do and liberal
**	in what you accept, we encode some characters which in fact are
**	allowed in URLs unencoded -- so DON'T use the table below for
**	parsing! 
**
**	Unlike HTUnEscape(), this routine returns a malloced string.
**
*/

/* Not BOTH static AND const at the same time in gcc :-(, Henrik 18/03-94 
**  code gen error in gcc when making random access to
**  static const table(!!)  */
/* PRIVATE CONST unsigned char isAcceptable[96] = */
PRIVATE unsigned char isAcceptable[96] =

/* Overencodes */
/*	Bit 0		xalpha		-- see HTFile.h
**	Bit 1		xpalpha		-- as xalpha but with plus.
**	Bit 2 ...	path		-- as xpalpha but with /
*/
    /*   0 1 2 3 4 5 6 7 8 9 A B C D E F */
    {    0,0,0,0,0,0,0,0,0,0,7,6,0,7,7,4,	/* 2x   !"#$%&'()*+,-./	 */
         7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,	/* 3x  0123456789:;<=>?	 */
	 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,	/* 4x  @ABCDEFGHIJKLMNO  */
	 7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,7,	/* 5X  PQRSTUVWXYZ[\]^_	 */
	 0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,	/* 6x  `abcdefghijklmno	 */
	 7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0 };	/* 7X  pqrstuvwxyz{\}~	DEL */

PRIVATE char *hex = "0123456789ABCDEF";

PUBLIC char * HTEscape ARGS2 (CONST char *, str,
	unsigned char, mask)
{
#define ACCEPTABLE(a)	( a>=32 && a<128 && ((isAcceptable[a-32]) & mask))
    CONST char * p;
    char * q;
    char * result;
    int unacceptable = 0;
    for(p=str; *p; p++)
        if (!ACCEPTABLE((unsigned char)TOASCII(*p)))
		unacceptable++;
    result = (char *) malloc(p-str + unacceptable+ unacceptable + 1);
    if (result == NULL) outofmem(__FILE__, "HTEscape");
    for(q=result, p=str; *p; p++) {
    	unsigned char a = TOASCII(*p);
	if (!ACCEPTABLE(a)) {
	    *q++ = HEX_ESCAPE;	/* Means hex commming */
	    *q++ = hex[a >> 4];
	    *q++ = hex[a & 15];
	}
	else *q++ = *p;
    }
    *q++ = 0;			/* Terminate */
    return result;
}


/*		Decode %xx escaped characters			HTUnEscape()
**		-----------------------------
**
**	This function takes a pointer to a string in which some
**	characters may have been encoded in %xy form, where xy is
**	the acsii hex code for character 16x+y.
**	The string is converted in place, as it will never grow.
*/

PRIVATE char from_hex ARGS1(char, c)
{
    return  c >= '0' && c <= '9' ?  c - '0' 
    	    : c >= 'A' && c <= 'F'? c - 'A' + 10
    	    : c - 'a' + 10;	/* accept small letters just in case */
}

PUBLIC char * HTUnEscape ARGS1( char *, str)
{
    char * p = str;
    char * q = str;

    if (!str) {					      /* Just for safety ;-) */
	if (TRACE)
	    fprintf(stderr, "HTUnEscape.. Called with NULL argument.\n");
	return "";
    }
    while(*p) {
        if (*p == HEX_ESCAPE) {
	    p++;
	    if (*p) *q = from_hex(*p++) * 16;
	    if (*p) *q = FROMASCII(*q + from_hex(*p++));
	    q++;
	} else {
	    *q++ = *p++; 
	}
    }
    
    *q++ = 0;
    return str;
    
} /* HTUnEscape */







/********************************************************************
 * lindner
 * 3.7
 * 1994/03/04 17:42:57
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/url.c,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: url.c
 * Simplified method of getting urls..
 *********************************************************************
 * Revision History:
 * url.c,v
 * Revision 3.7  1994/03/04  17:42:57  lindner
 * Fixes from Alan Coopersmith
 *
 * Revision 3.6  1994/01/21  04:25:41  lindner
 * Add support for tn3270 and better gopher url handling
 *
 * Revision 3.5  1993/12/30  04:18:08  lindner
 * translate telnet url correctly
 *
 * Revision 3.4  1993/12/27  16:14:03  lindner
 * Enlarge buffer size, make html docs on gopher server into http: refs
 *
 * Revision 3.3  1993/11/02  06:14:09  lindner
 * Add url html hack
 *
 *
 *********************************************************************/

#include "url.h"
#include "GSgopherobj.h"
#include "Malloc.h"
#include "String.h"

Url *
URLnew()
{
     Url *temp;

     temp = (Url *) malloc(sizeof(Url));
     temp->url = STRnew();
     
     return(temp);
}


void
URLdestroy(url)
  Url *url;
{
     STRdestroy(url->url);
     free(url);
}

/*
 * Make a default url from the current Gopher Object...
 */

void
URLfromGS(url, gs)
  Url       *url;
  GopherObj *gs;
{
     char u[2048], *path, *cp;

     *u = '\0';
     path  = GSgetPath(gs);

     if (path == NULL)
	  return;

     if (GSgetType(gs) == A_TELNET) {
	  if (*path == '\0')
	       sprintf(u, "telnet://%s:%d", GSgetHost(gs), GSgetPort(gs));
	  else
	       sprintf(u, "telnet://%s@%s:%d", path, GSgetHost(gs),
		       GSgetPort(gs));
     } else if (GSgetType(gs) == A_TN3270) {
	  if (*path == '\0')
	       sprintf(u, "tn3270://%s:%d", GSgetHost(gs), GSgetPort(gs));
	  else
	       sprintf(u, "tn3270://%s@%s:%d", path, GSgetHost(gs),
		       GSgetPort(gs));
     } else if (strncmp(path, "GET /", 5) == 0) {
	  sprintf(u, "http://%s:%d/%s", GSgetHost(gs), GSgetPort(gs),
		  path+5);
     }
     else {
	  sprintf(u, "gopher://%s:%d/%c", GSgetHost(gs), GSgetPort(gs),
		  GSgetType(gs));
	  cp = u + strlen(u);
	  Tohexstr(path, cp);
     }

     URLset(url, u);
}

/*
 * Hack gopher directories into an HTML type...
 */

void
URLmakeHTML(url)
  Url *url;
{
     char *cp = URLget(url);
     char *host;

     if (cp == NULL) 
	  return;

     if (strncmp(cp, "gopher://", 9) != 0)
	  return;

     /** find the type character **/
     cp = strchr(cp+10, '/');
     
     if (cp ==NULL)
	  return;

     host = cp+10;
     
     /** Test link for current host **/
/*     if (strcasecmp(host, hostname) != 0) 
	  return;*/
     
     cp ++;
     /** cp is now pointed at the type character **/
     
     if (*cp == '1' && *(cp+1) == '1') {
	  /** It's a directory **/
	  *cp = 'h';
	  *(cp+1) = 'h';
     }
}


/*
 * Get the transport of the specified URL
 */

char *
URLgetTransport(url)
  Url *url;
{
     static char trans[16];  /** Shouldn't need more than that.. yet.. */
     char *urlcp, *cp;

     urlcp = URLget(url);
     if (urlcp == NULL)
	  return(NULL);

     cp = strchr(urlcp, ':');
     if (cp == NULL)
	  return(NULL);
     
     strncpy(trans, urlcp, (int)(cp-urlcp));
     return (trans);
}
     
     
     


