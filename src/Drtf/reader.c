/*
 * - Need to document error code meanings.
 * - Need to do something with \* on destinations.
 * - Make the parameter a long?
 *
 * reader.c - RTF file reader.  Release 1.10.
 *
 * ASCII 10 (\n) and 13 (\r) are ignored and silently discarded.
 * Nulls are also discarded.
 * (although the read hook will still get a look at them.)
 *
 * "\:" is not a ":", it's a control symbol.  But some versions of
 * Word seem to write "\:" for ":".  This reader treats "\:" as a
 * plain text ":"
 *
 * 19 Mar 93
 * - Add hack to skip "{\*\keycode ... }" group in stylesheet.
 * This is probably the wrong thing to do, but it's simple.
 * 13 Jul 93
 * - Add THINK C awareness to malloc() declaration.  Necessary so
 * compiler knows the malloc argument is 4 bytes.  Ugh.
 * 07 Sep 93
 * - Text characters are mapped onto standard codes, which are placed
 * in rtfMinor.
 * - Eliminated use of index() function.
 * 05 Mar 94
 * - Added zillions of new symbols (those defined in RTF spec 1.2).
 * 14 Mar 94
 * - Public functions RTFMsg() and RTFPanic() now take variable arguments.
 * This means RTFPanic() now is used in place of what was formerly the
 * internal function Error().
 * - 8-bit characters are now legal, so they're not converted to \'xx
 * hex char representation now.
 * 01 Apr 94
 * - Added public variables rtfLineNum and rtfLinePos.
 * - #include string.h or strings.h, avoiding strncmp() problem where
 * last argument is treated as zero when prototype isn't available.
 * 04 Apr 94
 * - Treat style numbers 222 and 0 properly as "no style" and "normal".
 
 * -- 24apr94 -- d.g.gilbert, gilbertd@bio.indiana.edu
 * -- modifications for use w/ DCLAP & NCBI cross-platform toolkits
 * -- all changes affecting function incased in #ifdef DCLAP
  
 */


#define DCLAP

#ifdef DCLAP
#include <ncbi.h>
#define RTFBreakInData -6 		/* mark end of data buffer, but not EOF */
#define STDARG
#endif



# ifndef STRING_H
# define STRING_H <string.h>
# endif

# include	<stdio.h>
# include	<ctype.h>
# include	STRING_H
# ifdef STDARG
# include	<stdarg.h>
# else
# ifdef	VARARGS
# include	<varargs.h>
# endif	/* VARARGS */
# endif	/* STDARG */


# define	rtfInternal
# include	"rtf.h"
# include "rtfmaps.h"    /* dgg */
# undef		rtfInternal


/* # include	"tokenscan.h" */
/*----------- bundled this one in -----------------*/
#define	tsNoConcatDelims	0x01	/* all delimiters are significant */
typedef	struct TSScanner	TSScanner;

struct TSScanner
{
	void	(*scanInit) (char	*p);
	char	*(*scanScan) ();
	char	*scanDelim;
	char	*scanQuote;
	char	*scanEscape;
	char	*scanEos;
	short	scanFlags;
};


static void TSScanInit (char	*p);
static void TSSetScanner (TSScanner	*p);
static void TSGetScanner (TSScanner	*p);
static void TSSetScanPos (char	*p);

static char	*TSScan (void);
static char	*TSGetScanPos (void);
static short TSIsScanDelim (char c);
static short TSIsScanQuote (char c);
static short TSIsScanEscape (char c);
static short TSIsScanEos (char c);
static short TSTestScanFlags (short flags);

/*----------------------------*/



/*
 * Return pointer to new element of type t, or NULL
 * if no memory available.
 */

# define	New(t)	((t *) RTFAlloc ((short) sizeof (t)))


/* maximum number of character values representable in a byte */

# define	charSetSize		256

/* charset stack size */

# define	maxCSStack		10


#ifndef DCLAP
#ifndef THINK_C
extern char	*malloc ();
#else
extern void	*malloc(size_t);
#endif
#endif

static void	_RTFGetToken ();
static void	_RTFGetToken2 ();
static short	GetChar ();
static void	ReadFontTbl ();
static void	ReadColorTbl ();
static void	ReadStyleSheet ();
static void	ReadInfoGroup ();
static void	ReadPictGroup ();
static void	ReadObjGroup ();
static void	LookupInit ();
static void Lookup (char	*s);

static short	Hash (char	* s);

static void	CharSetInit ();
static void	ReadCharSetMaps ();


/*
 * Public variables (listed in rtf.h)
 */

short	rtfClass;
short	rtfMajor;
short	rtfMinor;
short	rtfParam;
char	*rtfTextBuf = NULL;
short	rtfTextLen;

long	rtfLineNum;
short	rtfLinePos;


/*
 * Private stuff
 */

static short	pushedChar;	/* pushback char if read too far */

static short	pushedClass;	/* pushed token info for RTFUngetToken() */
static short	pushedMajor;
static short	pushedMinor;
static short	pushedParam;
static char	*pushedTextBuf = NULL;

static short	prevChar;
static short	bumpLine;


static RTFFont	*fontList = NULL;	/* these lists MUST be */
static RTFColor	*colorList =  NULL;	/* initialized to NULL */
static RTFStyle	*styleList =  NULL;


static FILE	*rtffp = stdin;

static char	*inputName =  NULL;
static char	*outputName =  NULL;

/*
 * This array is used to map standard character names onto their numeric codes.
 * The position of the name within the array is the code.
 * stdcharnames.h is generated in the ../h directory.
 */

static char	*stdCharName[] = {
# include	"stdcharnames.h"
	 NULL
};





/*
 * These arrays are used to map RTF input character values onto the standard
 * character names represented by the values.  Input character values are
 * used as indices into the arrays to produce standard character codes.
 */


static char	*genCharSetFile =  NULL;
#if 0 
static short	genCharCode[charSetSize];
#else
static short  * genCharCode = ansi_gen_CharCode;
#endif
static short	haveGenCharSet = 0;

static char	*symCharSetFile = (char *) NULL;
#if 0 
static short	symCharCode[charSetSize];	
#else
static short  *symCharCode = ansi_sym_CharCode;
#endif
static short	haveSymCharSet = 0;

static short	curCharSet = rtfCSGeneral;
static short	* curCharCode = ansi_gen_CharCode;

/*
 * By default, the reader is configured to handle charset mapping invisibly,
 * including reading the charset files and switching charset maps as necessary
 * for Symbol font.
 */

static short	autoCharSetFlags;

/*
 * Stack for keeping track of charset map on group begin/end.  This is
 * necessary because group termination reverts the font to the previous
 * value, which may implicitly change it.
 */

static short	csStack[maxCSStack];
static short	csTop = 0;


/*
 * Initialize the reader.  This may be called multiple times,
 * to read multiple files.  The only thing not reset is the input
 * stream; that must be done with RTFSetStream().
 */

void
RTFInit ()
{
short	i;
RTFColor	*cp;
RTFFont		*fp;
RTFStyle	*sp;
RTFStyleElt	*eltList, *ep;


	if (rtfTextBuf == NULL)	/* initialize the text buffers */
	{
		rtfTextBuf = RTFAlloc (rtfBufSiz);
		pushedTextBuf = RTFAlloc (rtfBufSiz);
		if (rtfTextBuf ==  NULL
			|| pushedTextBuf ==  NULL)
			RTFPanic ("Cannot allocate text buffers.");
		rtfTextBuf[0] = pushedTextBuf[0] = '\0';
	}

	RTFFree (inputName);
	RTFFree (outputName);
	inputName = outputName =  NULL;
			
	/* initialize lookup table */
	LookupInit ();

	for (i = 0; i < rtfMaxClass; i++)
		RTFSetClassCallback (i,  NULL);
	for (i = 0; i < rtfMaxDestination; i++)
		RTFSetDestinationCallback (i, NULL);

	/* install built-in destination readers */
	RTFSetDestinationCallback (rtfFontTbl, ReadFontTbl);
	RTFSetDestinationCallback (rtfColorTbl, ReadColorTbl);
	RTFSetDestinationCallback (rtfStyleSheet, ReadStyleSheet);
	RTFSetDestinationCallback (rtfInfo, ReadInfoGroup);
	/* dgg -- want to process pict...  RTFSetDestinationCallback (rtfPict, ReadPictGroup); */
	RTFSetDestinationCallback (rtfObject, ReadObjGroup);


	RTFSetReadHook ( NULL);

	/* dump old lists if necessary */

	while (fontList !=  NULL)
	{
		fp = fontList->rtfNextFont;
		RTFFree (fontList->rtfFName);
		RTFFree ((char *) fontList);
		fontList = fp;
	}
	while (colorList !=  NULL)
	{
		cp = colorList->rtfNextColor;
		RTFFree ((char *) colorList);
		colorList = cp;
	}
	while (styleList !=  NULL)
	{
		sp = styleList->rtfNextStyle;
		eltList = styleList->rtfSSEList;
		while (eltList !=  NULL)
		{
			ep = eltList->rtfNextSE;
			RTFFree (eltList->rtfSEText);
			RTFFree ((char *) eltList);
			eltList = ep;
		}
		RTFFree (styleList->rtfSName);
		RTFFree ((char *) styleList);
		styleList = sp;
	}

	rtfClass = -1;
	pushedClass = -1;
	pushedChar = EOF;

	rtfLineNum = 0;
	rtfLinePos = 0;
	prevChar = EOF;
	bumpLine = 0;

	CharSetInit ();
	csTop = 0;
}


/*
 * Set the reader's input stream to the given stream.  Can
 * be used to redirect to other than the default (stdin).
 */

void RTFSetStream (FILE	*stream)
{
	rtffp = stream;
}


/*
 * Set or get the input or output file name.  These are never guaranteed
 * to be accurate, only insofar as the calling program makes them so.
 */

void
RTFSetInputName (char	* name)
{
	if ((inputName = RTFStrSave (name)) == NULL)
		RTFPanic ("RTFSetInputName: out of memory");
}


char *
RTFGetInputName ()
{
	return (inputName);
}


void
RTFSetOutputName (char	*name)
{
	if ((outputName = RTFStrSave (name)) ==  NULL)
		RTFPanic ("RTFSetOutputName: out of memory");
}


char *
RTFGetOutputName ()
{
	return (outputName);
}



/* ---------------------------------------------------------------------- */

/*
 * Callback table manipulation routines
 */


/*
 * Install or return a writer callback for a token class
 */


static RTFFuncPtr	ccb[rtfMaxClass];		/* class callbacks */


void
RTFSetClassCallback (short aclass, RTFFuncPtr callback)
{
	if (aclass >= 0 && aclass < rtfMaxClass)
		ccb[aclass] = callback;
}


RTFFuncPtr
RTFGetClassCallback (short aclass)
{
	if (aclass >= 0 && aclass < rtfMaxClass)
		return (ccb[aclass]);
	return ((RTFFuncPtr) NULL);
}


/*
 * Install or return a writer callback for a destination type
 */

static RTFFuncPtr	dcb[rtfMaxDestination];	/* destination callbacks */


void
RTFSetDestinationCallback (short dest, RTFFuncPtr callback)
{
	if (dest >= 0 && dest < rtfMaxDestination)
		dcb[dest] = callback;
}


RTFFuncPtr
RTFGetDestinationCallback (short dest)
{
	if (dest >= 0 && dest < rtfMaxDestination)
		return (dcb[dest]);
	return ( NULL);
}


/* ---------------------------------------------------------------------- */

/*
 * Token reading routines
 */


/*
 * Read the input stream, invoking the writer's callbacks
 * where appropriate.
 */

void
RTFRead ()
{
	while (RTFGetToken () != rtfEOF)
		RTFRouteToken ();
}


/*
 * Route a token.  If it's a destination for which a reader is
 * installed, process the destination internally, otherwise
 * pass the token to the writer's class callback.
 */

void
RTFRouteToken ()
{
RTFFuncPtr	p;

	if (rtfClass < 0 || rtfClass >= rtfMaxClass)	/* watchdog */
	{
		rtfTextBuf[rtfTextLen] = '\0'; 
		RTFPanic ("Unknown class %d: %s (reader malfunction)",
							rtfClass, rtfTextBuf);
	}
	if (RTFCheckCM (rtfControl, rtfDestination)) /* this could be define/inline call */
	{
		/* invoke destination-specific callback if there is one */
		if ((p = RTFGetDestinationCallback (rtfMinor)) != NULL)
		{
			(*p) ();
			return;
		}
	}
	/* invoke class callback if there is one */
	if ((p = RTFGetClassCallback (rtfClass)) != NULL)
		(*p) ();
}


/*
 * Skip to the end of the current group.  When this returns,
 * writers that maintain a state stack may want to call their
 * state unstacker; global vars will still be set to the group's
 * closing brace.
 */

void
RTFSkipGroup ()
{
short	level = 1;

	while (RTFGetToken () != rtfEOF)
	{
		if (rtfClass == rtfGroup)
		{
			if (rtfMajor == rtfBeginGroup)
				++level;
			else if (rtfMajor == rtfEndGroup)
			{
				if (--level < 1)
					break;	/* end of initial group */
			}
		}
	}
}


/*
 * Read one token.  Call the read hook if there is one.  The
 * token class is the return value.  Returns rtfEOF when there
 * are no more tokens.
 */

short
RTFGetToken ()
{
RTFFuncPtr	p;

	for (;;)
	{
		_RTFGetToken ();
		if ((p = RTFGetReadHook ()) != (RTFFuncPtr) NULL)
			(*p) ();	/* give read hook a look at token */

		/* Silently discard newlines, carriage returns, nulls.  */
		if (!(rtfClass == rtfText
			&& (rtfMajor == '\n' || rtfMajor == '\r'
						|| rtfMajor == '\0')))
			break;
	}
	return (rtfClass);
}


/*
 * Install or return a token reader hook.
 */

static RTFFuncPtr	readHook;


void
RTFSetReadHook (RTFFuncPtr f)
{
	readHook = f;
}


RTFFuncPtr
RTFGetReadHook ()
{
	return (readHook);
}


void
RTFUngetToken ()
{
	if (pushedClass >= 0)	/* there's already an ungotten token */
		RTFPanic ("cannot unget two tokens");
	if (rtfClass < 0)
		RTFPanic ("no token to unget");
	pushedClass = rtfClass;
	pushedMajor = rtfMajor;
	pushedMinor = rtfMinor;
	pushedParam = rtfParam;
	rtfTextBuf[rtfTextLen] = '\0'; 
	(void) strcpy (pushedTextBuf, rtfTextBuf);
}


short
RTFPeekToken ()
{
	_RTFGetToken ();
	RTFUngetToken ();
	return (rtfClass);
}


static void
_RTFGetToken ()
{
RTFFont	*fp;

	/* first check for pushed token from RTFUngetToken() */

	if (pushedClass >= 0)
	{
		rtfClass = pushedClass;
		rtfMajor = pushedMajor;
		rtfMinor = pushedMinor;
		rtfParam = pushedParam;
		(void) strcpy (rtfTextBuf, pushedTextBuf);
		rtfTextLen = strlen (rtfTextBuf);
		pushedClass = -1;
		return;
	}

	/*
	 * Beyond this point, no token is ever seen twice, which is
	 * important, e.g., for making sure no "}" pops the font stack twice.
	 */

	_RTFGetToken2 ();
	if (rtfClass == rtfText)	/* map RTF char to standard code */
		rtfMinor = RTFMapChar (rtfMajor);

	/*
	 * If auto-charset stuff is activated, see if anything needs doing,
	 * like reading the charset maps or switching between them.
	 */

	if (autoCharSetFlags == 0)
		return;

	if ((autoCharSetFlags & rtfReadCharSet)
		&& RTFCheckCM (rtfControl, rtfCharSet))
	{
		ReadCharSetMaps ();
	}
	else if ((autoCharSetFlags & rtfSwitchCharSet)
		&& RTFCheckCMM (rtfControl, rtfCharAttr, rtfFontNum))
	{
		if ((fp = RTFGetFont (rtfParam)) != NULL)
		{
			if (strncmp (fp->rtfFName, "Symbol", 6) == 0)
				curCharSet = rtfCSSymbol;
			else
				curCharSet = rtfCSGeneral;
			RTFSetCharSet (curCharSet);
		}
	}
	else if ((autoCharSetFlags & rtfSwitchCharSet) && rtfClass == rtfGroup)
	{
		switch (rtfMajor)
		{
		case rtfBeginGroup:
			if (csTop >= maxCSStack)
				RTFPanic ("_RTFGetToken: stack overflow");
			csStack[csTop++] = curCharSet;
			break;
		case rtfEndGroup:
			if (csTop <= 0)
				RTFPanic ("_RTFGetToken: stack underflow");
			curCharSet = csStack[--csTop];
			RTFSetCharSet (curCharSet);
			break;
		}
	}
}


/* this shouldn't be called anywhere but from _RTFGetToken() */

static void
_RTFGetToken2 ()
{
short	sign;
short	c;

	/* initialize token vars */

	rtfClass = rtfUnknown;
	rtfParam = rtfNoParam;
	rtfTextLen = 0;
  /* rtfTextBuf[0] = '\0'; */

	/* get first character, which may be a pushback from previous token */

	if (pushedChar != EOF)
	{
		c = pushedChar;
		rtfTextBuf[rtfTextLen++] = c;
		/* rtfTextBuf[rtfTextLen] = '\0';    //x */
		pushedChar = EOF;
	}
	else if ((c = GetChar ()) == EOF)
	{
		rtfClass = rtfEOF;
		return;
	}
	
#ifdef DCLAP
	if (c == RTFBreakInData)
	{
		rtfClass = rtfEOF;
		rtfParam = RTFBreakInData;
		return;
	}
#endif

	if (c == '{')
	{
		rtfClass = rtfGroup;
		rtfMajor = rtfBeginGroup;
		return;
	}
	if (c == '}')
	{
		rtfClass = rtfGroup;
		rtfMajor = rtfEndGroup;
		return;
	}
	if (c != '\\')
	{
		/*
		 * Two possibilities here:
		 * 1) ASCII 9, effectively like \tab control symbol
		 * 2) literal text char
		 */
		if (c == '\t')			/* ASCII 9 */
		{
			rtfClass = rtfControl;
			rtfMajor = rtfSpecialChar;
			rtfMinor = rtfTab;
		}
		else
		{
			rtfClass = rtfText;
			rtfMajor = c;
		}
		return;
	}

	if ((c = GetChar ()) == EOF)
	{
		/* early eof, whoops (class is rtfUnknown) */
		return;
	}
	if (!isalpha (c))
	{
		/*
		 * Three possibilities here:
		 * 1) hex encoded text char, e.g., \'d5, \'d3
		 * 2) special escaped text char, e.g., \{, \}
		 * 3) control symbol, e.g., \_, \-, \|, \<10>
		 */
		if (c == '\'')				/* hex char */
		{
		short	c2;

			if ((c = GetChar ()) != EOF && (c2 = GetChar ()) != EOF)
			{
				/* should do isxdigit check! */
				rtfClass = rtfText;
				rtfMajor = RTFCharToHex (c) * 16
						+ RTFCharToHex (c2);
				return;
			}
			/* early eof, whoops (class is rtfUnknown) */
			return;
		}

		/* escaped char */
		/*if (index (":{}\\", c) !=  NULL) /* escaped char */
		if (c == ':' || c == '{' || c == '}' || c == '\\')
		{
			rtfClass = rtfText;
			rtfMajor = c;
			return;
		}

		/* control symbol */
		rtfTextBuf[rtfTextLen] = '\0';  
		Lookup (rtfTextBuf);	/* sets class, major, minor */
		return;
	}
	/* control word */
	while (isalpha (c))
	{
		if ((c = GetChar ()) == EOF)
			break;
	}

	/*
	 * At this point, the control word is all collected, so the
	 * major/minor numbers are determined before the parameter
	 * (if any) is scanned.  There will be one too many characters
	 * in the buffer, though, so fix up before and restore after
	 * looking up.
	 */

	if (c != EOF)
		rtfTextBuf[rtfTextLen-1] = '\0';
	else
		rtfTextBuf[rtfTextLen] = '\0';  
	Lookup (rtfTextBuf);	/* sets class, major, minor */
	if (c != EOF)
		rtfTextBuf[rtfTextLen-1] = c;

	/*
	 * Should be looking at first digit of parameter if there
	 * is one, unless it's negative.  In that case, next char
	 * is '-', so need to gobble next char, and remember sign.
	 */

	sign = 1;
	if (c == '-')
	{
		sign = -1;
		c = GetChar ();
	}
	if (c != EOF && isdigit (c))
	{
		rtfParam = 0;
		while (isdigit (c))	/* gobble parameter */
		{
			rtfParam = rtfParam * 10 + c - '0';
			if ((c = GetChar ()) == EOF)
				break;
		}
		rtfParam *= sign;
	}
	/*
	 * If control symbol delimiter was a blank, gobble it.
	 * Otherwise the character is first char of next token, so
	 * push it back for next call.  In either case, delete the
	 * delimiter from the token buffer.
	 */
	if (c != EOF)
	{
		if (c != ' ') pushedChar = c;
		rtfTextBuf[--rtfTextLen] = '\0';
	}
}


/*
 * Read the next character from the input.  This handles setting the
 * current line and position-within-line variables.  Those variable are
 * set correctly whether lines end with CR, LF, or CRLF (the last being
 * the tricky case).
 *
 * bumpLine indicates whether the line number should be incremented on
 * the *next* input character.
 */

static short
GetChar ()
{
short	c;
short	oldBumpLine;

#ifdef DCLAP
	/* if (gReaderIsDead) c= EOF; else */
  c= rtfGetOneChar() & 0xFFFF; /* !! DAMN INTs, short to short conversion screws this up ! */
	if (c != EOF)
#else
	if ((c = getc (rtffp)) != EOF)
#endif
	{
		rtfTextBuf[rtfTextLen++] = c;
	  /* rtfTextBuf[rtfTextLen] = '\0';  //x drop this for speed !? */
	}

#if 0   
		/* original code looks slow here...*/
	if (prevChar == EOF) bumpLine = 1;
	oldBumpLine = bumpLine;	/* non-zero if prev char was line ending */
	bumpLine = 0;
	if (c == '\r') bumpLine = 1;
	else if (c == '\n') {
		bumpLine = 1;
		if (prevChar == '\r')		/* oops, previous \r wasn't */
			oldBumpLine = 0;	/* really a line ending */
		}
	++rtfLinePos;
	if (oldBumpLine)	/* were we supposed to increment the */
	{			/* line count on this char? */
		++rtfLineNum;
		rtfLinePos = 1;
	}
#else
	rtfLinePos++;
	if (bumpLine || prevChar == EOF) {  /* prevchar == EOF only on 1st call !? */
		if (! (c == '\n' && prevChar == '\r') ) {
			++rtfLineNum;
			rtfLinePos = 1;
			}
		bumpLine= 0;
		}
	if (c == '\r' || c == '\n') bumpLine= 1;
#endif

	prevChar = c;
	return (c);
}


/*
 * Synthesize a token by setting the global variables to the
 * values supplied.  Typically this is followed with a call
 * to RTFRouteToken().
 *
 * If a param value other than rtfNoParam is passed, it becomes
 * part of the token text.
 */

void RTFSetToken (short aclass,short major,short minor,short param,char* text)
{
	rtfClass = aclass;
	rtfMajor = major;
	rtfMinor = minor;
	rtfParam = param;
	rtfTextBuf[rtfTextLen] = '\0'; 
	if (param == rtfNoParam)
		(void) strcpy (rtfTextBuf, text);
	else
		sprintf (rtfTextBuf, "%s%d", text, param);
	rtfTextLen = rtfTextLen; /* strlen (rtfTextBuf); */
}


/* ---------------------------------------------------------------------- */

/*
 * Routines to handle mapping of RTF character sets
 * onto standard characters.
 *
 * RTFStdCharCode(name)	given char name, produce numeric code
 * RTFStdCharName(code)	given char code, return name
 * RTFMapChar(c)	map input (RTF) char code to std code
 * RTFSetCharSet(id)	select given charset map
 * RTFGetCharSet()	get current charset map
 *
 * See ../h/README for more information about charset names and codes.
 */


/*
 * Initialize charset stuff.
 */

static void
CharSetInit ()
{
	autoCharSetFlags = (rtfReadCharSet | rtfSwitchCharSet);
	RTFFree (genCharSetFile);
	genCharSetFile = NULL;
	haveGenCharSet = 0;
	RTFFree (symCharSetFile);
	symCharSetFile =  NULL;
	haveSymCharSet = 0;
	curCharSet = rtfCSGeneral;
	curCharCode = genCharCode;
	RTFSetCharSet(curCharSet); /* dgg added */
}


/*
 * Specify the name of a file to be read when auto-charset-file reading is
 * done.
 */

void
RTFSetCharSetMap (char	* name,short csId)
{
	if ((name = RTFStrSave (name)) ==  NULL)	/* make copy */
		RTFPanic ("RTFSetCharSetMap: out of memory");
	switch (csId)
	{
	case rtfCSGeneral:
		RTFFree (genCharSetFile);	/* free any previous value */
		genCharSetFile = name;
		break;
	case rtfCSSymbol:
		RTFFree (symCharSetFile);	/* free any previous value */
		symCharSetFile = name;
		break;
	}
}


/*
 * Do auto-charset-file reading.
 */

static void
ReadCharSetMaps ()
{
char	buf[rtfBufSiz];

	rtfTextBuf[rtfTextLen] = '\0'; 
	if (genCharSetFile != NULL)
		strcpy (buf, genCharSetFile);
	else
		sprintf (buf, "%s-gen", &rtfTextBuf[1]);
	if (RTFReadCharSetMap (buf, rtfCSGeneral) == 0)
		RTFPanic ("ReadCharSetMaps: Cannot read charset map %s", buf);
	if (symCharSetFile !=  NULL)
		strcpy (buf, symCharSetFile);
	else
		sprintf (buf, "%s-sym", &rtfTextBuf[1]);
	if (RTFReadCharSetMap (buf, rtfCSSymbol) == 0)
		RTFPanic ("ReadCharSetMaps: Cannot read charset map %s", buf);
}



/*
 * Read in a file describing an RTF character set map.  Lines consist of pairs
 * associating character names with character values.
 *
 * If the filename is an absolute pathname, look in the specified location
 * only.  Otherwise try to find the file in the current directory or library.
 */

short
RTFReadCharSetMap (char	* file, short csId)
{

#if 1
	enum flags { kunknown, kmac, kansi, kpc, kpca };
	short	flag = kunknown;

	if (strncmp(file,"mac-",4) == 0) flag= kmac;
  else if (strncmp(file,"ansi-",5) == 0) flag= kansi; 
  else if (strncmp(file,"pca-",4) == 0) flag= kpca;
  else if (strncmp(file,"pc-",3) == 0) flag= kpc;

	switch (csId)
	{
	default:
		return (0);	/* illegal charset id */

	case rtfCSGeneral: {
		switch (flag) {
			case kmac	: genCharCode= mac_gen_CharCode; break;
			case kpca	: genCharCode= pca_gen_CharCode; break;
			case kpc	: genCharCode= pc_gen_CharCode; break;
			default:
			case kansi: genCharCode= ansi_gen_CharCode; break;
			}
		}
		break;

	case rtfCSSymbol: {
		switch (flag) {
			case kmac	: symCharCode= mac_sym_CharCode; break;
			case kpca	: symCharCode= pca_sym_CharCode; break;
			case kpc	: symCharCode= pc_sym_CharCode; break;
			default:
			case kansi: symCharCode= ansi_sym_CharCode; break;
			}
    }
		break;
	}

#else
	/* obsolete.... */
FILE	*f;
char	buf[rtfBufSiz];
char	*name;
unsigned char *p;
short	*stdCodeArray;
short	stdCode;
short	radix;
short	value;
short	i;
TSScanner	scanner;
char		*scanEscape;
char	*fn = "RTFReadCharSetMap";


	switch (csId)
	{
	default:
		return (0);	/* illegal charset id */
	case rtfCSGeneral:
		stdCodeArray = genCharCode;
		break;
	case rtfCSSymbol:
		stdCodeArray = symCharCode;
		break;
	}

	if ((f = RTFOpenLibFile (file, "r")) == NULL)
		return (0);

	/* clobber current mapping */

	for (i = 0; i < charSetSize; i++)
	{
		stdCodeArray[i] = rtfSC_nothing;
	}

	/*
	 * Turn off scanner's backslash escape mechanism while reading
	 * charset file.  Restore it later.
	 */
	TSGetScanner (&scanner);
	scanEscape = scanner.scanEscape;
	scanner.scanEscape = "";
	TSSetScanner (&scanner);

	/* read file */

	while (fgets (buf, (short) sizeof (buf), f) !=  NULL)
	{
		if(buf[0] == '#')	/* skip comment lines */
			continue;
		TSScanInit (buf);
		if ((name = TSScan ()) == NULL)
			continue;	/* skip blank lines */
		if ((stdCode = RTFStdCharCode (name)) < 0)
		{
			RTFPanic ("%s: unknown character name: %s", fn, name);
			continue;
		}
		if ((p = (unsigned char*)TSScan ()) == NULL)
		{
			RTFPanic ("%s: malformed charset map line for character %s",
								fn, name);
			continue;
		}
		if (p[1] == '\0')	/* single char - use ascii value */
			value = (unsigned char) p[0];
		else
		{
			radix = 10;
			if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
			{
				radix = 16;
				p += 2;
			}
			value = 0;
			while (*p != '\0')
				value = value * radix + RTFCharToHex(*p++);
		}
		if (value >= charSetSize)
		{
			RTFMsg ("%s: character value %d for %s too high\n",
							fn, value, name);
			RTFPanic ("maximum value is %d", charSetSize - 1);
		}
		stdCodeArray[value] = stdCode;
	}
	scanner.scanEscape = scanEscape;
	TSSetScanner (&scanner);
	fclose(f);

#if 0
	strncpy( buf, file, rtfBufSiz);
	strncat( buf, ".h", rtfBufSiz);
	if ( (f = RTFOpenLibFile (buf, "w")) != NULL) {
		/* dgg -- write array to disk for later use */
		fprintf(f, "/* %s -- mapping charset to std. codes */\n\n", buf);
		fprintf(f, "static short %sCharCode[%d] = {\n", file, charSetSize);
		for (i = 0; i < charSetSize; i++) {
			char cend = '\n';
			/* if (i % 20 == 19) cend= '\n'; else cend= ' '; */
			fprintf(f, "%#3x,%c", stdCodeArray[i], cend);
			}
		fprintf(f, " };\n");
		fclose(f);
		}
#endif

#endif /* obsolete */


	switch (csId)
	{
	case rtfCSGeneral:
		haveGenCharSet = 1;
		break;
	case rtfCSSymbol:
		haveSymCharSet = 1;
		break;
	}

	return (1);
}


/*
 * Given a standard character name (a string), find its code (a number).
 * Return -1 if name is unknown.
 */

#if 0
	/* moved to rtfmap.h ... */
short
RTFStdCharCode (char	*name)
{
short	i;

	for (i = 0; i < rtfSC_MaxChar; i++)
	{
		if (strcmp (name, stdCharName[i]) == 0)
			return (i);
	}
	return (-1);
}


/*
 * Given a standard character code (a number), find its name (a string).
 * Return NULL if code is unknown.
 */

char * RTFStdCharName (short code)
{
	if (code < 0 || code >= rtfSC_MaxChar)
		return (  NULL);
	return (stdCharName[code]);
}

#endif


/*
 * Given an RTF input character code, find standard character code.
 * The translator should read the appropriate charset maps when it finds a
 * charset control.  However, the file might not contain one.  In this
 * case, no map will be available.  When the first attempt is made to
 * map a character under these circumstances, RTFMapChar() assumes ANSI
 * and reads the map as necessary.
 */

short
RTFMapChar (short c)
{
#if 0 /* //x DGG_DROP_FOR_SPEED */
/* don't need for each char !? */
	switch (curCharSet)
	{
	case rtfCSGeneral:
		if (!haveGenCharSet)
		{
			if (RTFReadCharSetMap ("ansi-gen", rtfCSGeneral) == 0)
				RTFPanic ("RTFMapChar: cannot read ansi-gen");
		}
		break;
	case rtfCSSymbol:
		if (!haveSymCharSet)
		{
			if (RTFReadCharSetMap ("ansi-sym", rtfCSSymbol) == 0)
				RTFPanic ("RTFMapChar: cannot read ansi-sym");
		}
		break;
	}
#endif
	if (c < 0 || c >= charSetSize)
		return (rtfSC_nothing);
	return (curCharCode[c]);
}


/*
 * Set the current character set.  If csId is illegal, uses general charset.
 */

void
RTFSetCharSet (short csId)
{
	switch (csId)
	{
	default:		/* use general if csId unknown */
	case rtfCSGeneral:
		curCharCode = genCharCode;
		curCharSet = csId;
		break;
	case rtfCSSymbol:
		curCharCode = symCharCode;
		curCharSet = csId;
		break;
	}

#if 1 /* //x */
/* moved here from MapChar */
	switch (curCharSet)
	{
	case rtfCSGeneral:
		if (!haveGenCharSet)
		{
			if (RTFReadCharSetMap ("ansi-gen", rtfCSGeneral) == 0)
				RTFPanic ("RTFMapChar: cannot read ansi-gen");
		}
		break;
	case rtfCSSymbol:
		if (!haveSymCharSet)
		{
			if (RTFReadCharSetMap ("ansi-sym", rtfCSSymbol) == 0)
				RTFPanic ("RTFMapChar: cannot read ansi-sym");
		}
		break;
	}
#endif
}


short
RTFGetCharSet ()
{
	return (curCharSet);
}


/* ---------------------------------------------------------------------- */

/*
 * Special destination readers.  They gobble the destination so the
 * writer doesn't have to deal with them.  That's wrong for any
 * translator that wants to process any of these itself.  In that
 * case, these readers should be overridden by installing a different
 * destination callback.
 *
 * NOTE: The last token read by each of these reader will be the
 * destination's terminating '}', which will then be the current token.
 * That '}' token is passed to RTFRouteToken() - the writer has already
 * seen the '{' that began the destination group, and may have pushed a
 * state; it also needs to know at the end of the group that a state
 * should be popped.
 *
 * It's important that rtf.h and the control token lookup table list
 * as many symbols as possible, because these destination readers
 * unfortunately make strict assumptions about the input they expect,
 * and a token of class rtfUnknown will throw them off easily.
 */


/*
 * Read { \fonttbl ... } destination.  Old font tables don't have
 * braces around each table entry; try to adjust for that.
 */

static void
ReadFontTbl ()
{
RTFFont	*fp;
char	buf[rtfBufSiz], *bp;
short	old = -1;
char	*fn = "ReadFontTbl";

	for (;;)
	{
		(void) RTFGetToken ();
		if (RTFCheckCM (rtfGroup, rtfEndGroup))
			break;
		if (old < 0)		/* first entry - determine tbl type */
		{
			if (RTFCheckCMM (rtfControl, rtfCharAttr, rtfFontNum))
				old = 1;	/* no brace */
			else if (RTFCheckCM (rtfGroup, rtfBeginGroup))
				old = 0;	/* brace */
			else			/* can't tell! */
				RTFPanic ("%s: Cannot determine format", fn);
		}
		if (old == 0)		/* need to find "{" here */
		{
			if (!RTFCheckCM (rtfGroup, rtfBeginGroup))
				RTFPanic ("%s: missing \"{\"", fn);
			(void) RTFGetToken ();	/* yes, skip to next token */
		}
		if ((fp = New (RTFFont)) == NULL)
			RTFPanic ("%s: cannot allocate font entry", fn);

		fp->rtfNextFont = fontList;
		fontList = fp;

		fp->rtfFName =   NULL;
		fp->rtfFAltName =  NULL;
		fp->rtfFNum = -1;
		fp->rtfFFamily = 0;
		fp->rtfFCharSet = 0;
		fp->rtfFPitch = 0;
		fp->rtfFType = 0;
		fp->rtfFCodePage = 0;

		while (rtfClass != rtfEOF && !RTFCheckCM (rtfText, ';'))
		{
			if (rtfClass == rtfControl)
			{
				switch (rtfMajor)
				{
				default:
					rtfTextBuf[rtfTextLen] = '\0'; 
					/* ignore token but announce it */
					RTFMsg ("%s: unknown token \"%s\"\n",
							fn, rtfTextBuf);
				case rtfFontFamily:
					fp->rtfFFamily = rtfMinor;
					break;
				case rtfCharAttr:
					switch (rtfMinor)
					{
					default:
						break;	/* ignore unknown? */
					case rtfFontNum:
						fp->rtfFNum = rtfParam;
						break;
					}
					break;
				case rtfFontAttr:
					switch (rtfMinor)
					{
					default:
						break;	/* ignore unknown? */
					case rtfFontCharSet:
						fp->rtfFCharSet = rtfParam;
						break;
					case rtfFontPitch:
						fp->rtfFPitch = rtfParam;
						break;
					case rtfFontCodePage:
						fp->rtfFCodePage = rtfParam;
						break;
					case rtfFTypeNil:
					case rtfFTypeTrueType:
						fp->rtfFType = rtfParam;
						break;
					}
					break;
				}
			}
			else if (RTFCheckCM (rtfGroup, rtfBeginGroup))	/* dest */
			{
				RTFSkipGroup ();	/* ignore for now */
			}
			else if (rtfClass == rtfText)	/* font name */
			{
				bp = buf;
				while (rtfClass != rtfEOF
					&& !RTFCheckCM (rtfText, ';'))
				{
					*bp++ = rtfMajor;
					(void) RTFGetToken ();
				}
				*bp = '\0';
				fp->rtfFName = RTFStrSave (buf);
				if (fp->rtfFName ==  NULL)
					RTFPanic ("%s: cannot allocate font name", fn);
				/* already have next token; don't read one */
				/* at bottom of loop */
				continue;
			}
			else
			{
				rtfTextBuf[rtfTextLen] = '\0'; 
				/* ignore token but announce it */
				RTFMsg ("%s: unknown token \"%s\"\n",
							fn, rtfTextBuf);
			}
			(void) RTFGetToken ();
		}
		if (old == 0)	/* need to see "}" here */
		{
			(void) RTFGetToken ();
			if (!RTFCheckCM (rtfGroup, rtfEndGroup))
				RTFPanic ("%s: missing \"}\"", fn);
		}
	}
	if (fp->rtfFNum == -1)
		RTFPanic ("%s: missing font number", fn);
/*
 * Could check other pieces of structure here, too, I suppose.
 */
	RTFRouteToken ();	/* feed "}" back to router */
}


/*
 * The color table entries have color values of -1 if
 * the default color should be used for the entry (only
 * a semi-colon is given in the definition, no color values).
 * There will be a problem if a partial entry (1 or 2 but
 * not 3 color values) is given.  The possibility is ignored
 * here.
 */

static void
ReadColorTbl ()
{
RTFColor	*cp;
short		cnum = 0;
char		*fn = "ReadColorTbl";

	for (;;)
	{
		(void) RTFGetToken ();
		if (RTFCheckCM (rtfGroup, rtfEndGroup))
			break;
		if ((cp = New (RTFColor)) == NULL)
			RTFPanic ("%s: cannot allocate color entry", fn);
		cp->rtfCNum = cnum++;
		cp->rtfCRed = cp->rtfCGreen = cp->rtfCBlue = -1;
		cp->rtfNextColor = colorList;
		colorList = cp;
		while (RTFCheckCM (rtfControl, rtfColorName))
		{
			switch (rtfMinor)
			{
			case rtfRed:	cp->rtfCRed = rtfParam; break;
			case rtfGreen:	cp->rtfCGreen = rtfParam; break;
			case rtfBlue:	cp->rtfCBlue = rtfParam; break;
			}
			RTFGetToken ();
		}
		if (!RTFCheckCM (rtfText, (short) ';'))
			RTFPanic ("%s: malformed entry", fn);
	}
	RTFRouteToken ();	/* feed "}" back to router */
}


/*
 * The "Normal" style definition doesn't contain any style number,
 * all others do.  Normal style is given style rtfNormalStyleNum.
 */

static void
ReadStyleSheet ()
{
RTFStyle	*sp;
RTFStyleElt	*sep, *sepLast;
char		buf[rtfBufSiz], *bp;
char		*fn = "ReadStyleSheet";
#ifdef DCLAP
short			lastWasFontnum;
#endif

	for (;;)
	{
		(void) RTFGetToken ();
		if (RTFCheckCM (rtfGroup, rtfEndGroup))
			break;
		if ((sp = New (RTFStyle)) ==  NULL)
			RTFPanic ("%s: cannot allocate stylesheet entry", fn);
		sp->rtfSName = NULL;
		sp->rtfSNum = -1;
		sp->rtfSType = rtfParStyle;
		sp->rtfSAdditive = 0;
		sp->rtfSBasedOn = rtfNoStyleNum;
		sp->rtfSNextPar = -1;
		sp->rtfSSEList = sepLast =  NULL;
		sp->rtfNextStyle = styleList;
		sp->rtfExpanding = 0;
		styleList = sp;
		if (!RTFCheckCM (rtfGroup, rtfBeginGroup))
			RTFPanic ("%s: missing \"{\"", fn);
#ifdef DCLAP
		lastWasFontnum = 0;
#endif

		for (;;)
		{
			(void) RTFGetToken ();

#ifdef DCLAP
	/* obscure problem w/ MSWord-Mac-v5 
	-- it doesn't write \fs24 in the stylesheet (fs24 = default size)
	-- look for any font spec \f# and append a \fs24 here (even if it is followed by fs?)
	*/
				if (lastWasFontnum) {
				 if (rtfMinor != rtfFontSize) {
					if ((sep = New (RTFStyleElt)) ==  NULL)
						RTFPanic ("%s: cannot allocate style element", fn);
					sep->rtfSEClass = rtfControl;
					sep->rtfSEMajor = rtfCharAttr;
					sep->rtfSEMinor = rtfFontSize;
					sep->rtfSEParam = 24;
					if ((sep->rtfSEText = RTFStrSave ("24")) == NULL)
						RTFPanic ("%s: cannot allocate style element text", fn);
					sepLast->rtfNextSE = sep;
					sep->rtfNextSE =  NULL;
					sepLast = sep;
					}
					}
				lastWasFontnum = RTFCheckMM( rtfCharAttr, rtfFontNum);
				if (lastWasFontnum) {
					/* break here for debug... */
					lastWasFontnum = 1;
					}
#endif

			if (rtfClass == rtfEOF
				|| RTFCheckCM (rtfText, ';'))
				break;
			if (rtfClass == rtfControl)
			{
				if (RTFCheckMM (rtfSpecialChar, rtfOptDest))
					continue;	/* ignore "\*" */
				if (RTFCheckMM (rtfParAttr, rtfStyleNum))
				{
					sp->rtfSNum = rtfParam;
					sp->rtfSType = rtfParStyle;
					continue;
				}
				if (RTFCheckMM (rtfCharAttr, rtfCharStyleNum))
				{
					sp->rtfSNum = rtfParam;
					sp->rtfSType = rtfCharStyle;
					continue;
				}
				if (RTFCheckMM (rtfSectAttr, rtfSectStyleNum))
				{
					sp->rtfSNum = rtfParam;
					sp->rtfSType = rtfSectStyle;
					continue;
				}
				if (RTFCheckMM (rtfStyleAttr, rtfBasedOn))
				{
					sp->rtfSBasedOn = rtfParam;
					continue;
				}
				if (RTFCheckMM (rtfStyleAttr, rtfAdditive))
				{
					sp->rtfSAdditive = 1;
					continue;
				}
				if (RTFCheckMM (rtfStyleAttr, rtfNext))
				{
					sp->rtfSNextPar = rtfParam;
					continue;
				}
				

				if ((sep = New (RTFStyleElt)) ==  NULL)
					RTFPanic ("%s: cannot allocate style element", fn);
				sep->rtfSEClass = rtfClass;
				sep->rtfSEMajor = rtfMajor;
				sep->rtfSEMinor = rtfMinor;
				sep->rtfSEParam = rtfParam;
				rtfTextBuf[rtfTextLen] = '\0'; 
				if ((sep->rtfSEText = RTFStrSave (rtfTextBuf))
								== NULL)
					RTFPanic ("%s: cannot allocate style element text", fn);
				if (sepLast ==  NULL)
					sp->rtfSSEList = sep;	/* first element */
				else				/* add to end */
					sepLast->rtfNextSE = sep;
				sep->rtfNextSE = NULL;
				sepLast = sep;
			}
			else if (RTFCheckCM (rtfGroup, rtfBeginGroup))
			{
				/*
				 * This passes over "{\*\keycode ... }, among
				 * other things. A temporary (perhaps) hack.
				 */
				RTFSkipGroup ();
				continue;
			}
			else if (rtfClass == rtfText)	/* style name */
			{
				bp = buf;
				while (rtfClass == rtfText)
				{
					if (rtfMajor == ';')
					{
						/* put back for "for" loop */
						(void) RTFUngetToken ();
						break;
					}
					*bp++ = rtfMajor;
					(void) RTFGetToken ();
				}
				*bp = '\0';
				if ((sp->rtfSName = RTFStrSave (buf)) == NULL)
					RTFPanic ("%s: cannot allocate style name", fn);
			}
			else		/* unrecognized */
			{
				/* ignore token but announce it */
				rtfTextBuf[rtfTextLen] = '\0'; 
 				RTFMsg ("%s: unknown token \"%s\"\n",
							fn, rtfTextBuf);
			}
		}
		(void) RTFGetToken ();
		if (!RTFCheckCM (rtfGroup, rtfEndGroup))
			RTFPanic ("%s: missing \"}\"", fn);

		/*
		 * Check over the style structure.  A name is a must.
		 * If no style number was specified, check whether it's the
		 * Normal style (in which case it's given style number
		 * rtfNormalStyleNum).  Note that some "normal" style names
		 * just begin with "Normal" and can have other stuff following,
		 * e.g., "Normal,Times 10 point".  Ugh.
		 *
		 * Some German RTF writers use "Standard" instead of "Normal".
		 */
		if (sp->rtfSName == NULL)
			RTFPanic ("%s: missing style name", fn);
		if (sp->rtfSNum < 0)
		{
			if (strncmp (buf, "Normal", 6) != 0
				&& strncmp (buf, "Standard", 8) != 0)
				RTFPanic ("%s: missing style number", fn);
			sp->rtfSNum = rtfNormalStyleNum;
		}
		if (sp->rtfSNextPar == -1)	/* if \snext not given, */
			sp->rtfSNextPar = sp->rtfSNum;	/* next is itself */
	}
	RTFRouteToken ();	/* feed "}" back to router */
}


static void
ReadInfoGroup ()
{
	RTFSkipGroup ();
	RTFRouteToken ();	/* feed "}" back to router */
}


static void
ReadPictGroup ()
{
	RTFSkipGroup ();
	RTFRouteToken ();	/* feed "}" back to router */
}


static void
ReadObjGroup ()
{
	RTFSkipGroup ();
	RTFRouteToken ();	/* feed "}" back to router */
}


/* ---------------------------------------------------------------------- */

/*
 * Routines to return pieces of stylesheet, or font or color tables.
 * References to style 0 are mapped onto the Normal style.
 */


RTFStyle *
RTFGetStyle (short num)
{
RTFStyle	*s;

	if (num == -1)
		return (styleList);
	for (s = styleList; s !=  NULL; s = s->rtfNextStyle)
	{
		if (s->rtfSNum == num)
			break;
	}
	return (s);		/* NULL if not found */
}


RTFFont *
RTFGetFont (short num)
{
RTFFont	*f;

	if (num == -1)
		return (fontList);
	for (f = fontList; f != NULL; f = f->rtfNextFont)
	{
		if (f->rtfFNum == num)
			break;
	}
	return (f);		/* NULL if not found */
}


RTFColor *
RTFGetColor (short num)
{
RTFColor	*c;

	if (num == -1)
		return (colorList);
	for (c = colorList; c !=  NULL; c = c->rtfNextColor)
	{
		if (c->rtfCNum == num)
			break;
	}
	return (c);		/* NULL if not found */
}


/* ---------------------------------------------------------------------- */


/*
 * Expand style n, if there is such a style.
 */

void
RTFExpandStyle (short n)
{
RTFStyle	*s;
RTFStyleElt	*se;

	if (n == -1 || (s = RTFGetStyle (n)) ==  NULL)
		return;
	if (s->rtfExpanding != 0)
		RTFPanic ("Style expansion loop, style %d", n);
	s->rtfExpanding = 1;	/* set expansion flag for loop detection */
	/*
	 * Expand "based-on" style (unless it's the same as the current
	 * style -- Normal style usually gives itself as its own based-on
	 * style).  Based-on style expansion is done by synthesizing
	 * the token that the writer needs to see in order to trigger
	 * another style expansion, and feeding to token back through
	 * the router so the writer sees it.
	 */
	if (n != s->rtfSBasedOn)
	{
		RTFSetToken (rtfControl, rtfParAttr, rtfStyleNum,
							s->rtfSBasedOn, "\\s");
		RTFRouteToken ();
	}
	/*
	 * Now route the tokens unique to this style.  RTFSetToken()
	 * isn't used because it would add the param value to the end
	 * of the token text, which already has it in.
	 */
	for (se = s->rtfSSEList; se !=   NULL; se = se->rtfNextSE)
	{
		rtfClass = se->rtfSEClass;
		rtfMajor = se->rtfSEMajor;
		rtfMinor = se->rtfSEMinor;
		rtfParam = se->rtfSEParam;
		(void) strcpy (rtfTextBuf, se->rtfSEText);
		rtfTextLen = strlen (rtfTextBuf);
		RTFRouteToken ();
	}
	s->rtfExpanding = 0;	/* done - clear expansion flag */
}


/* ---------------------------------------------------------------------- */

/*
 * Control symbol lookup routines
 */


typedef struct RTFKey	RTFKey;

struct RTFKey
{
	short	rtfKMajor;	/* major number */
	short	rtfKMinor;	/* minor number */
	char	*rtfKStr;	/* symbol name */
	short	rtfKHash;	/* symbol name hash value */
};

/*
 * A minor number of -1 means the token has no minor number
 * (all valid minor numbers are >= 0).
 */

#ifdef COMP_SYMC
static RTFKey rtfKey[] =
#else
static FAR RTFKey	rtfKey[] =
#endif
{
	/*
	 * Special characters
	 */

	rtfSpecialChar,	rtfIIntVersion,		"vern",		0,
	rtfSpecialChar,	rtfICreateTime,		"creatim",	0,
	rtfSpecialChar,	rtfIRevisionTime,	"revtim",	0,
	rtfSpecialChar,	rtfIPrintTime,		"printim",	0,
	rtfSpecialChar,	rtfIBackupTime,		"buptim",	0,
	rtfSpecialChar,	rtfIEditTime,		"edmins",	0,
	rtfSpecialChar,	rtfIYear,		"yr",		0,
	rtfSpecialChar,	rtfIMonth,		"mo",		0,
	rtfSpecialChar,	rtfIDay,		"dy",		0,
	rtfSpecialChar,	rtfIHour,		"hr",		0,
	rtfSpecialChar,	rtfIMinute,		"min",		0,
	rtfSpecialChar,	rtfISecond,		"sec",		0,
	rtfSpecialChar,	rtfINPages,		"nofpages",	0,
	rtfSpecialChar,	rtfINWords,		"nofwords",	0,
	rtfSpecialChar,	rtfINChars,		"nofchars",	0,
	rtfSpecialChar,	rtfIIntID,		"id",		0,

	rtfSpecialChar,	rtfCurHeadDate,		"chdate",	0,
	rtfSpecialChar,	rtfCurHeadDateLong,	"chdpl",	0,
	rtfSpecialChar,	rtfCurHeadDateAbbrev,	"chdpa",	0,
	rtfSpecialChar,	rtfCurHeadTime,		"chtime",	0,
	rtfSpecialChar,	rtfCurHeadPage,		"chpgn",	0,
	rtfSpecialChar,	rtfSectNum,		"sectnum",	0,
	rtfSpecialChar,	rtfCurFNote,		"chftn",	0,
	rtfSpecialChar,	rtfCurAnnotRef,		"chatn",	0,
	rtfSpecialChar,	rtfFNoteSep,		"chftnsep",	0,
	rtfSpecialChar,	rtfFNoteCont,		"chftnsepc",	0,
	rtfSpecialChar,	rtfCell,		"cell",		0,
	rtfSpecialChar,	rtfRow,			"row",		0,
	rtfSpecialChar,	rtfPar,			"par",		0,
	/* newline and carriage return are synonyms for */
	/* \par when they are preceded by a \ character */
	rtfSpecialChar,	rtfPar,			"\n",		0,
	rtfSpecialChar,	rtfPar,			"\r",		0,
	rtfSpecialChar,	rtfSect,		"sect",		0,
	rtfSpecialChar,	rtfPage,		"page",		0,
	rtfSpecialChar,	rtfColumn,		"column",	0,
	rtfSpecialChar,	rtfLine,		"line",		0,
	rtfSpecialChar,	rtfSoftPage,		"softpage",	0,
	rtfSpecialChar,	rtfSoftColumn,		"softcol",	0,
	rtfSpecialChar,	rtfSoftLine,		"softline",	0,
	rtfSpecialChar,	rtfSoftLineHt,		"softlheight",	0,
	rtfSpecialChar,	rtfTab,			"tab",		0,
	rtfSpecialChar,	rtfEmDash,		"emdash",	0,
	rtfSpecialChar,	rtfEnDash,		"endash",	0,
	rtfSpecialChar,	rtfEmSpace,		"emspace",	0,
	rtfSpecialChar,	rtfEnSpace,		"enspace",	0,
	rtfSpecialChar,	rtfBullet,		"bullet",	0,
	rtfSpecialChar,	rtfLQuote,		"lquote",	0,
	rtfSpecialChar,	rtfRQuote,		"rquote",	0,
	rtfSpecialChar,	rtfLDblQuote,		"ldblquote",	0,
	rtfSpecialChar,	rtfRDblQuote,		"rdblquote",	0,
	rtfSpecialChar,	rtfFormula,		"|",		0,
	rtfSpecialChar,	rtfNoBrkSpace,		"~",		0,
	rtfSpecialChar,	rtfNoReqHyphen,		"-",		0,
	rtfSpecialChar,	rtfNoBrkHyphen,		"_",		0,
	rtfSpecialChar,	rtfOptDest,		"*",		0,
	rtfSpecialChar,	rtfLTRMark,		"ltrmark",	0,
	rtfSpecialChar,	rtfRTLMark,		"rtlmark",	0,
	rtfSpecialChar,	rtfNoWidthJoiner,	"zwj",		0,
	rtfSpecialChar,	rtfNoWidthNonJoiner,	"zwnj",		0,
	/* is this valid? */
	rtfSpecialChar,	rtfCurHeadPict,		"chpict",	0,

	/*
	 * Character formatting attributes
	 */

	rtfCharAttr,	rtfPlain,		"plain",	0,
	rtfCharAttr,	rtfBold,		"b",		0,
	rtfCharAttr,	rtfAllCaps,		"caps",		0,
	rtfCharAttr,	rtfDeleted,		"deleted",	0,
	rtfCharAttr,	rtfSubScript,		"dn",		0,
	rtfCharAttr,	rtfSubScrShrink,	"sub",		0,
	rtfCharAttr,	rtfNoSuperSub,		"nosupersub",	0,
	rtfCharAttr,	rtfExpand,		"expnd",	0,
	rtfCharAttr,	rtfExpandTwips,		"expndtw",	0,
	rtfCharAttr,	rtfKerning,		"kerning",	0,
	rtfCharAttr,	rtfFontNum,		"f",		0,
	rtfCharAttr,	rtfFontSize,		"fs",		0,
	rtfCharAttr,	rtfItalic,		"i",		0,
	rtfCharAttr,	rtfOutline,		"outl",		0,
	rtfCharAttr,	rtfRevised,		"revised",	0,
	rtfCharAttr,	rtfRevAuthor,		"revauth",	0,
	rtfCharAttr,	rtfRevDTTM,		"revdttm",	0,
	rtfCharAttr,	rtfSmallCaps,		"scaps",	0,
	rtfCharAttr,	rtfShadow,		"shad",		0,
	rtfCharAttr,	rtfStrikeThru,		"strike",	0,
	rtfCharAttr,	rtfUnderline,		"ul",		0,
	rtfCharAttr,	rtfDotUnderline,	"uld",		0,
	rtfCharAttr,	rtfDbUnderline,		"uldb",		0,
	rtfCharAttr,	rtfNoUnderline,		"ulnone",	0,
	rtfCharAttr,	rtfWordUnderline,	"ulw",		0,
	rtfCharAttr,	rtfSuperScript,		"up",		0,
	rtfCharAttr,	rtfSuperScrShrink,	"super",	0,
	rtfCharAttr,	rtfInvisible,		"v",		0,
	rtfCharAttr,	rtfForeColor,		"cf",		0,
	rtfCharAttr,	rtfBackColor,		"cb",		0,
	rtfCharAttr,	rtfRTLChar,		"rtlch",	0,
	rtfCharAttr,	rtfLTRChar,		"ltrch",	0,
	rtfCharAttr,	rtfCharStyleNum,	"cs",		0,
	rtfCharAttr,	rtfCharCharSet,		"cchs",		0,
	rtfCharAttr,	rtfLanguage,		"lang",		0,
	/* this has disappeared from spec 1.2 */
	rtfCharAttr,	rtfGray,		"gray",		0,

	/*
	 * Paragraph formatting attributes
	 */

	rtfParAttr,	rtfParDef,		"pard",		0,
	rtfParAttr,	rtfStyleNum,		"s",		0,
	rtfParAttr,	rtfHyphenate,		"hyphpar",	0,
	rtfParAttr,	rtfInTable,		"intbl",	0,
	rtfParAttr,	rtfKeep,		"keep",		0,
	rtfParAttr,	rtfNoWidowControl,	"nowidctlpar",	0,
	rtfParAttr,	rtfKeepNext,		"keepn",	0,
	rtfParAttr,	rtfOutlineLevel,	"level",	0,
	rtfParAttr,	rtfNoLineNum,		"noline",	0,
	rtfParAttr,	rtfPBBefore,		"pagebb",	0,
	rtfParAttr,	rtfSideBySide,		"sbys",		0,
	rtfParAttr,	rtfQuadLeft,		"ql",		0,
	rtfParAttr,	rtfQuadRight,		"qr",		0,
	rtfParAttr,	rtfQuadJust,		"qj",		0,
	rtfParAttr,	rtfQuadCenter,		"qc",		0,
	rtfParAttr,	rtfFirstIndent,		"fi",		0,
	rtfParAttr,	rtfLeftIndent,		"li",		0,
	rtfParAttr,	rtfRightIndent,		"ri",		0,
	rtfParAttr,	rtfSpaceBefore,		"sb",		0,
	rtfParAttr,	rtfSpaceAfter,		"sa",		0,
	rtfParAttr,	rtfSpaceBetween,	"sl",		0,
	rtfParAttr,	rtfSpaceMultiply,	"slmult",	0,

	rtfParAttr,	rtfSubDocument,		"subdocument",	0,

	rtfParAttr,	rtfRTLPar,		"rtlpar",	0,
	rtfParAttr,	rtfLTRPar,		"ltrpar",	0,

	rtfParAttr,	rtfTabPos,		"tx",		0,
	/*
	 * FrameMaker writes \tql (to mean left-justified tab, apparently)
	 * although it's not in the spec.  It's also redundant, since lj
	 * tabs are the default.
	 */
	rtfParAttr,	rtfTabLeft,		"tql",		0,
	rtfParAttr,	rtfTabRight,		"tqr",		0,
	rtfParAttr,	rtfTabCenter,		"tqc",		0,
	rtfParAttr,	rtfTabDecimal,		"tqdec",	0,
	rtfParAttr,	rtfTabBar,		"tb",		0,
	rtfParAttr,	rtfLeaderDot,		"tldot",	0,
	rtfParAttr,	rtfLeaderHyphen,	"tlhyph",	0,
	rtfParAttr,	rtfLeaderUnder,		"tlul",		0,
	rtfParAttr,	rtfLeaderThick,		"tlth",		0,
	rtfParAttr,	rtfLeaderEqual,		"tleq",		0,

	rtfParAttr,	rtfParLevel,		"pnlvl",	0,
	rtfParAttr,	rtfParBullet,		"pnlvlblt",	0,
	rtfParAttr,	rtfParSimple,		"pnlvlbody",	0,
	rtfParAttr,	rtfParNumCont,		"pnlvlcont",	0,
	rtfParAttr,	rtfParNumOnce,		"pnnumonce",	0,
	rtfParAttr,	rtfParNumAcross,	"pnacross",	0,
	rtfParAttr,	rtfParHangIndent,	"pnhang",	0,
	rtfParAttr,	rtfParNumRestart,	"pnrestart",	0,
	rtfParAttr,	rtfParNumCardinal,	"pncard",	0,
	rtfParAttr,	rtfParNumDecimal,	"pndec",	0,
	rtfParAttr,	rtfParNumULetter,	"pnucltr",	0,
	rtfParAttr,	rtfParNumURoman,	"pnucrm",	0,
	rtfParAttr,	rtfParNumLLetter,	"pnlcltr",	0,
	rtfParAttr,	rtfParNumLRoman,	"pnlcrm",	0,
	rtfParAttr,	rtfParNumOrdinal,	"pnord",	0,
	rtfParAttr,	rtfParNumOrdinalText,	"pnordt",	0,
	rtfParAttr,	rtfParNumBold,		"pnb",		0,
	rtfParAttr,	rtfParNumItalic,	"pni",		0,
	rtfParAttr,	rtfParNumAllCaps,	"pncaps",	0,
	rtfParAttr,	rtfParNumSmallCaps,	"pnscaps",	0,
	rtfParAttr,	rtfParNumUnder,		"pnul",		0,
	rtfParAttr,	rtfParNumDotUnder,	"pnuld",	0,
	rtfParAttr,	rtfParNumDbUnder,	"pnuldb",	0,
	rtfParAttr,	rtfParNumNoUnder,	"pnulnone",	0,
	rtfParAttr,	rtfParNumWordUnder,	"pnulw",	0,
	rtfParAttr,	rtfParNumStrikethru,	"pnstrike",	0,
	rtfParAttr,	rtfParNumForeColor,	"pncf",		0,
	rtfParAttr,	rtfParNumFont,		"pnf",		0,
	rtfParAttr,	rtfParNumFontSize,	"pnfs",		0,
	rtfParAttr,	rtfParNumIndent,	"pnindent",	0,
	rtfParAttr,	rtfParNumSpacing,	"pnsp",		0,
	rtfParAttr,	rtfParNumInclPrev,	"pnprev",	0,
	rtfParAttr,	rtfParNumCenter,	"pnqc",		0,
	rtfParAttr,	rtfParNumLeft,		"pnql",		0,
	rtfParAttr,	rtfParNumRight,		"pnqr",		0,
	rtfParAttr,	rtfParNumStartAt,	"pnstart",	0,

	rtfParAttr,	rtfBorderTop,		"brdrt",	0,
	rtfParAttr,	rtfBorderBottom,	"brdrb",	0,
	rtfParAttr,	rtfBorderLeft,		"brdrl",	0,
	rtfParAttr,	rtfBorderRight,		"brdrr",	0,
	rtfParAttr,	rtfBorderBetween,	"brdrbtw",	0,
	rtfParAttr,	rtfBorderBar,		"brdrbar",	0,
	rtfParAttr,	rtfBorderBox,		"box",		0,
	rtfParAttr,	rtfBorderSingle,	"brdrs",	0,
	rtfParAttr,	rtfBorderThick,		"brdrth",	0,
	rtfParAttr,	rtfBorderShadow,	"brdrsh",	0,
	rtfParAttr,	rtfBorderDouble,	"brdrdb",	0,
	rtfParAttr,	rtfBorderDot,		"brdrdot",	0,
	rtfParAttr,	rtfBorderDot,		"brdrdash",	0,
	rtfParAttr,	rtfBorderHair,		"brdrhair",	0,
	rtfParAttr,	rtfBorderWidth,		"brdrw",	0,
	rtfParAttr,	rtfBorderColor,		"brdrcf",	0,
	rtfParAttr,	rtfBorderSpace,		"brsp",		0,

	rtfParAttr,	rtfShading,		"shading",	0,
	rtfParAttr,	rtfBgPatH,		"bghoriz",	0,
	rtfParAttr,	rtfBgPatV,		"bgvert",	0,
	rtfParAttr,	rtfFwdDiagBgPat,	"bgfdiag",	0,
	rtfParAttr,	rtfBwdDiagBgPat,	"bgbdiag",	0,
	rtfParAttr,	rtfHatchBgPat,		"bgcross",	0,
	rtfParAttr,	rtfDiagHatchBgPat,	"bgdcross",	0,
	rtfParAttr,	rtfDarkBgPatH,		"bgdkhoriz",	0,
	rtfParAttr,	rtfDarkBgPatV,		"bgdkvert",	0,
	rtfParAttr,	rtfFwdDarkBgPat,	"bgdkfdiag",	0,
	rtfParAttr,	rtfBwdDarkBgPat,	"bgdkbdiag",	0,
	rtfParAttr,	rtfDarkHatchBgPat,	"bgdkcross",	0,
	rtfParAttr,	rtfDarkDiagHatchBgPat,	"bgdkdcross",	0,
	rtfParAttr,	rtfBgPatLineColor,	"cfpat",	0,
	rtfParAttr,	rtfBgPatColor,		"cbpat",	0,

	/*
	 * Section formatting attributes
	 */

	rtfSectAttr,	rtfSectDef,		"sectd",	0,
	rtfSectAttr,	rtfENoteHere,		"endnhere",	0,
	rtfSectAttr,	rtfPrtBinFirst,		"binfsxn",	0,
	rtfSectAttr,	rtfPrtBin,		"binsxn",	0,
	rtfSectAttr,	rtfSectStyleNum,	"ds",		0,

	rtfSectAttr,	rtfNoBreak,		"sbknone",	0,
	rtfSectAttr,	rtfColBreak,		"sbkcol",	0,
	rtfSectAttr,	rtfPageBreak,		"sbkpage",	0,
	rtfSectAttr,	rtfEvenBreak,		"sbkeven",	0,
	rtfSectAttr,	rtfOddBreak,		"sbkodd",	0,

	rtfSectAttr,	rtfColumns,		"cols",		0,
	rtfSectAttr,	rtfColumnSpace,		"colsx",	0,
	rtfSectAttr,	rtfColumnNumber,	"colno",	0,
	rtfSectAttr,	rtfColumnSpRight,	"colsr",	0,
	rtfSectAttr,	rtfColumnWidth,		"colw",		0,
	rtfSectAttr,	rtfColumnLine,		"linebetcol",	0,

	rtfSectAttr,	rtfLineModulus,		"linemod",	0,
	rtfSectAttr,	rtfLineDist,		"linex",	0,
	rtfSectAttr,	rtfLineStarts,		"linestarts",	0,
	rtfSectAttr,	rtfLineRestart,		"linerestart",	0,
	rtfSectAttr,	rtfLineRestartPg,	"lineppage",	0,
	rtfSectAttr,	rtfLineCont,		"linecont",	0,

	rtfSectAttr,	rtfSectPageWid,		"pgwsxn",	0,
	rtfSectAttr,	rtfSectPageHt,		"pghsxn",	0,
	rtfSectAttr,	rtfSectMarginLeft,	"marglsxn",	0,
	rtfSectAttr,	rtfSectMarginRight,	"margrsxn",	0,
	rtfSectAttr,	rtfSectMarginTop,	"margtsxn",	0,
	rtfSectAttr,	rtfSectMarginBottom,	"margbsxn",	0,
	rtfSectAttr,	rtfSectMarginGutter,	"guttersxn",	0,
	rtfSectAttr,	rtfSectLandscape,	"lndscpsxn",	0,
	rtfSectAttr,	rtfTitleSpecial,	"titlepg",	0,
	rtfSectAttr,	rtfHeaderY,		"headery",	0,
	rtfSectAttr,	rtfFooterY,		"footery",	0,

	rtfSectAttr,	rtfPageStarts,		"pgnstarts",	0,
	rtfSectAttr,	rtfPageCont,		"pgncont",	0,
	rtfSectAttr,	rtfPageRestart,		"pgnrestart",	0,
	rtfSectAttr,	rtfPageNumRight,	"pgnx",		0,
	rtfSectAttr,	rtfPageNumTop,		"pgny",		0,
	rtfSectAttr,	rtfPageDecimal,		"pgndec",	0,
	rtfSectAttr,	rtfPageURoman,		"pgnucrm",	0,
	rtfSectAttr,	rtfPageLRoman,		"pgnlcrm",	0,
	rtfSectAttr,	rtfPageULetter,		"pgnucltr",	0,
	rtfSectAttr,	rtfPageLLetter,		"pgnlcltr",	0,
	rtfSectAttr,	rtfPageNumHyphSep,	"pgnhnsh",	0,
	rtfSectAttr,	rtfPageNumSpaceSep,	"pgnhnsp",	0,
	rtfSectAttr,	rtfPageNumColonSep,	"pgnhnsc",	0,
	rtfSectAttr,	rtfPageNumEmdashSep,	"pgnhnsm",	0,
	rtfSectAttr,	rtfPageNumEndashSep,	"pgnhnsn",	0,

	rtfSectAttr,	rtfTopVAlign,		"vertalt",	0,
	/* misspelled as "vertal" in specification 1.0 */
	rtfSectAttr,	rtfBottomVAlign,	"vertalb",	0,
	rtfSectAttr,	rtfCenterVAlign,	"vertalc",	0,
	rtfSectAttr,	rtfJustVAlign,		"vertalj",	0,

	rtfSectAttr,	rtfRTLSect,		"rtlsect",	0,
	rtfSectAttr,	rtfLTRSect,		"ltrsect",	0,

	/* I've seen these in an old spec, but not in real files... */
	/*rtfSectAttr,	rtfNoBreak,		"nobreak",	0,*/
	/*rtfSectAttr,	rtfColBreak,		"colbreak",	0,*/
	/*rtfSectAttr,	rtfPageBreak,		"pagebreak",	0,*/
	/*rtfSectAttr,	rtfEvenBreak,		"evenbreak",	0,*/
	/*rtfSectAttr,	rtfOddBreak,		"oddbreak",	0,*/

	/*
	 * Document formatting attributes
	 */

	rtfDocAttr,	rtfDefTab,		"deftab",	0,
	rtfDocAttr,	rtfHyphHotZone,		"hyphhotz",	0,
	rtfDocAttr,	rtfHyphConsecLines,	"hyphconsec",	0,
	rtfDocAttr,	rtfHyphCaps,		"hyphcaps",	0,
	rtfDocAttr,	rtfHyphAuto,		"hyphauto",	0,
	rtfDocAttr,	rtfLineStart,		"linestart",	0,
	rtfDocAttr,	rtfFracWidth,		"fracwidth",	0,
	/* \makeback was given in old version of spec, it's now */
	/* listed as \makebackup */
	rtfDocAttr,	rtfMakeBackup,		"makeback",	0,
	rtfDocAttr,	rtfMakeBackup,		"makebackup",	0,
	rtfDocAttr,	rtfRTFDefault,		"defformat",	0,
	rtfDocAttr,	rtfPSOverlay,		"psover",	0,
	rtfDocAttr,	rtfDocTemplate,		"doctemp",	0,
	rtfDocAttr,	rtfDefLanguage,		"deflang",	0,

	rtfDocAttr,	rtfFENoteType,		"fet",		0,
	rtfDocAttr,	rtfFNoteEndSect,	"endnotes",	0,
	rtfDocAttr,	rtfFNoteEndDoc,		"enddoc",	0,
	rtfDocAttr,	rtfFNoteText,		"ftntj",	0,
	rtfDocAttr,	rtfFNoteBottom,		"ftnbj",	0,
	rtfDocAttr,	rtfENoteEndSect,	"aendnotes",	0,
	rtfDocAttr,	rtfENoteEndDoc,		"aenddoc",	0,
	rtfDocAttr,	rtfENoteText,		"aftntj",	0,
	rtfDocAttr,	rtfENoteBottom,		"aftnbj",	0,
	rtfDocAttr,	rtfFNoteStart,		"ftnstart",	0,
	rtfDocAttr,	rtfENoteStart,		"aftnstart",	0,
	rtfDocAttr,	rtfFNoteRestartPage,	"ftnrstpg",	0,
	rtfDocAttr,	rtfFNoteRestart,	"ftnrestart",	0,
	rtfDocAttr,	rtfFNoteRestartCont,	"ftnrstcont",	0,
	rtfDocAttr,	rtfENoteRestart,	"aftnrestart",	0,
	rtfDocAttr,	rtfENoteRestartCont,	"aftnrstcont",	0,
	rtfDocAttr,	rtfFNoteNumArabic,	"ftnnar",	0,
	rtfDocAttr,	rtfFNoteNumLLetter,	"ftnnalc",	0,
	rtfDocAttr,	rtfFNoteNumULetter,	"ftnnauc",	0,
	rtfDocAttr,	rtfFNoteNumLRoman,	"ftnnrlc",	0,
	rtfDocAttr,	rtfFNoteNumURoman,	"ftnnruc",	0,
	rtfDocAttr,	rtfFNoteNumChicago,	"ftnnchi",	0,
	rtfDocAttr,	rtfENoteNumArabic,	"aftnnar",	0,
	rtfDocAttr,	rtfENoteNumLLetter,	"aftnnalc",	0,
	rtfDocAttr,	rtfENoteNumULetter,	"aftnnauc",	0,
	rtfDocAttr,	rtfENoteNumLRoman,	"aftnnrlc",	0,
	rtfDocAttr,	rtfENoteNumURoman,	"aftnnruc",	0,
	rtfDocAttr,	rtfENoteNumChicago,	"aftnnchi",	0,

	rtfDocAttr,	rtfPaperWidth,		"paperw",	0,
	rtfDocAttr,	rtfPaperHeight,		"paperh",	0,
	rtfDocAttr,	rtfPaperSize,		"psz",		0,
	rtfDocAttr,	rtfLeftMargin,		"margl",	0,
	rtfDocAttr,	rtfRightMargin,		"margr",	0,
	rtfDocAttr,	rtfTopMargin,		"margt",	0,
	rtfDocAttr,	rtfBottomMargin,	"margb",	0,
	rtfDocAttr,	rtfFacingPage,		"facingp",	0,
	rtfDocAttr,	rtfGutterWid,		"gutter",	0,
	rtfDocAttr,	rtfMirrorMargin,	"margmirror",	0,
	rtfDocAttr,	rtfLandscape,		"landscape",	0,
	rtfDocAttr,	rtfPageStart,		"pgnstart",	0,
	rtfDocAttr,	rtfWidowCtrl,		"widowctrl",	0,

	rtfDocAttr,	rtfLinkStyles,		"linkstyles",	0,

	rtfDocAttr,	rtfNoAutoTabIndent,	"notabind",	0,
	rtfDocAttr,	rtfWrapSpaces,		"wraptrsp",	0,
	rtfDocAttr,	rtfPrintColorsBlack,	"prcolbl",	0,
	rtfDocAttr,	rtfNoExtraSpaceRL,	"noextrasprl",	0,
	rtfDocAttr,	rtfNoColumnBalance,	"nocolbal",	0,
	rtfDocAttr,	rtfCvtMailMergeQuote,	"cvmme",	0,
	rtfDocAttr,	rtfSuppressTopSpace,	"sprstsp",	0,
	rtfDocAttr,	rtfSuppressPreParSpace,	"sprsspbf",	0,
	rtfDocAttr,	rtfCombineTblBorders,	"otblrul",	0,
	rtfDocAttr,	rtfTranspMetafiles,	"transmf",	0,
	rtfDocAttr,	rtfSwapBorders,		"swpbdr",	0,
	rtfDocAttr,	rtfShowHardBreaks,	"brkfrm",	0,

	rtfDocAttr,	rtfFormProtected,	"formprot",	0,
	rtfDocAttr,	rtfAllProtected,	"allprot",	0,
	rtfDocAttr,	rtfFormShading,		"formshade",	0,
	rtfDocAttr,	rtfFormDisplay,		"formdisp",	0,
	rtfDocAttr,	rtfPrintData,		"printdata",	0,

	rtfDocAttr,	rtfRevProtected,	"revprot",	0,
	rtfDocAttr,	rtfRevisions,		"revisions",	0,
	rtfDocAttr,	rtfRevDisplay,		"revprop",	0,
	rtfDocAttr,	rtfRevBar,		"revbar",	0,

	rtfDocAttr,	rtfAnnotProtected,	"annotprot",	0,

	rtfDocAttr,	rtfRTLDoc,		"rtldoc",	0,
	rtfDocAttr,	rtfLTRDoc,		"ltrdoc",	0,

	/*
	 * Style attributes
	 */

	rtfStyleAttr,	rtfAdditive,		"additive",	0,
	rtfStyleAttr,	rtfBasedOn,		"sbasedon",	0,
	rtfStyleAttr,	rtfNext,		"snext",	0,

	/*
	 * Picture attributes
	 */

	rtfPictAttr,	rtfMacQD,		"macpict",	0,
	rtfPictAttr,	rtfPMMetafile,		"pmmetafile",	0,
	rtfPictAttr,	rtfWinMetafile,		"wmetafile",	0,
	rtfPictAttr,	rtfDevIndBitmap,	"dibitmap",	0,
	rtfPictAttr,	rtfWinBitmap,		"wbitmap",	0,
	rtfPictAttr,	rtfPixelBits,		"wbmbitspixel",	0,
	rtfPictAttr,	rtfBitmapPlanes,	"wbmplanes",	0,
	rtfPictAttr,	rtfBitmapWid,		"wbmwidthbytes", 0,

	rtfPictAttr,	rtfPicWid,		"picw",		0,
	rtfPictAttr,	rtfPicHt,		"pich",		0,
	rtfPictAttr,	rtfPicGoalWid,		"picwgoal",	0,
	rtfPictAttr,	rtfPicGoalHt,		"pichgoal",	0,
	/* these two aren't in the spec, but some writers emit them */
	rtfPictAttr,	rtfPicGoalWid,		"picwGoal",	0,
	rtfPictAttr,	rtfPicGoalHt,		"pichGoal",	0,
	rtfPictAttr,	rtfPicScaleX,		"picscalex",	0,
	rtfPictAttr,	rtfPicScaleY,		"picscaley",	0,
	rtfPictAttr,	rtfPicScaled,		"picscaled",	0,
	rtfPictAttr,	rtfPicCropTop,		"piccropt",	0,
	rtfPictAttr,	rtfPicCropBottom,	"piccropb",	0,
	rtfPictAttr,	rtfPicCropLeft,		"piccropl",	0,
	rtfPictAttr,	rtfPicCropRight,	"piccropr",	0,

	rtfPictAttr,	rtfPicMFHasBitmap,	"picbmp",	0,
	rtfPictAttr,	rtfPicMFBitsPerPixel,	"picbpp",	0,

	rtfPictAttr,	rtfPicBinary,		"bin",		0,

	/*
	 * NeXT graphic attributes
	 */

	rtfNeXTGrAttr,	rtfNeXTGWidth,		"width",	0,
	rtfNeXTGrAttr,	rtfNeXTGHeight,		"height",	0,

	/*
	 * Destinations
	 */

	rtfDestination,	rtfFontTbl,		"fonttbl",	0,
	rtfDestination,	rtfFontAltName,		"falt",		0,
	rtfDestination,	rtfEmbeddedFont,	"fonteb",	0,
	rtfDestination,	rtfFontFile,		"fontfile",	0,
	rtfDestination,	rtfFileTbl,		"filetbl",	0,
	rtfDestination,	rtfFileInfo,		"file",		0,
	rtfDestination,	rtfColorTbl,		"colortbl",	0,
	rtfDestination,	rtfStyleSheet,		"stylesheet",	0,
	rtfDestination,	rtfKeyCode,		"keycode",	0,
	rtfDestination,	rtfRevisionTbl,		"revtbl",	0,
	rtfDestination,	rtfInfo,		"info",		0,
	rtfDestination,	rtfITitle,		"title",	0,
	rtfDestination,	rtfISubject,		"subject",	0,
	rtfDestination,	rtfIAuthor,		"author",	0,
	rtfDestination,	rtfIOperator,		"operator",	0,
	rtfDestination,	rtfIKeywords,		"keywords",	0,
	rtfDestination,	rtfIComment,		"comment",	0,
	rtfDestination,	rtfIVersion,		"version",	0,
	rtfDestination,	rtfIDoccomm,		"doccomm",	0,
	/* \verscomm may not exist -- was seen in earlier spec version */
	rtfDestination,	rtfIVerscomm,		"verscomm",	0,
	rtfDestination,	rtfNextFile,		"nextfile",	0,
	rtfDestination,	rtfTemplate,		"template",	0,
	rtfDestination,	rtfFNSep,		"ftnsep",	0,
	rtfDestination,	rtfFNContSep,		"ftnsepc",	0,
	rtfDestination,	rtfFNContNotice,	"ftncn",	0,
	rtfDestination,	rtfENSep,		"aftnsep",	0,
	rtfDestination,	rtfENContSep,		"aftnsepc",	0,
	rtfDestination,	rtfENContNotice,	"aftncn",	0,
	rtfDestination,	rtfPageNumLevel,	"pgnhn",	0,
	rtfDestination,	rtfParNumLevelStyle,	"pnseclvl",	0,
	rtfDestination,	rtfHeader,		"header",	0,
	rtfDestination,	rtfFooter,		"footer",	0,
	rtfDestination,	rtfHeaderLeft,		"headerl",	0,
	rtfDestination,	rtfHeaderRight,		"headerr",	0,
	rtfDestination,	rtfHeaderFirst,		"headerf",	0,
	rtfDestination,	rtfFooterLeft,		"footerl",	0,
	rtfDestination,	rtfFooterRight,		"footerr",	0,
	rtfDestination,	rtfFooterFirst,		"footerf",	0,
	rtfDestination,	rtfParNumText,		"pntext",	0,
	rtfDestination,	rtfParNumbering,	"pn",		0,
	rtfDestination,	rtfParNumTextAfter,	"pntexta",	0,
	rtfDestination,	rtfParNumTextBefore,	"pntextb",	0,
	rtfDestination,	rtfBookmarkStart,	"bkmkstart",	0,
	rtfDestination,	rtfBookmarkEnd,		"bkmkend",	0,
	rtfDestination,	rtfPict,		"pict",		0,
	rtfDestination,	rtfObject,		"object",	0,
	rtfDestination,	rtfObjClass,		"objclass",	0,
	rtfDestination,	rtfObjName,		"objname",	0,
	rtfObjAttr,	rtfObjTime,		"objtime",	0,
	rtfDestination,	rtfObjData,		"objdata",	0,
	rtfDestination,	rtfObjAlias,		"objalias",	0,
	rtfDestination,	rtfObjSection,		"objsect",	0,
	/* objitem and objtopic aren't documented in the spec! */
	rtfDestination,	rtfObjItem,		"objitem",	0,
	rtfDestination,	rtfObjTopic,		"objtopic",	0,
	rtfDestination,	rtfObjResult,		"result",	0,
	rtfDestination,	rtfDrawObject,		"do",		0,
	rtfDestination,	rtfFootnote,		"footnote",	0,
	rtfDestination,	rtfAnnotRefStart,	"atrfstart",	0,
	rtfDestination,	rtfAnnotRefEnd,		"atrfend",	0,
	rtfDestination,	rtfAnnotID,		"atnid",	0,
	rtfDestination,	rtfAnnotAuthor,		"atnauthor",	0,
	rtfDestination,	rtfAnnotation,		"annotation",	0,
	rtfDestination,	rtfAnnotRef,		"atnref",	0,
	rtfDestination,	rtfAnnotTime,		"atntime",	0,
	rtfDestination,	rtfAnnotIcon,		"atnicn",	0,
	rtfDestination,	rtfField,		"field",	0,
	rtfDestination,	rtfFieldInst,		"fldinst",	0,
	rtfDestination,	rtfFieldResult,		"fldrslt",	0,
	rtfDestination,	rtfDataField,		"datafield",	0,
	rtfDestination,	rtfIndex,		"xe",		0,
	rtfDestination,	rtfIndexText,		"txe",		0,
	rtfDestination,	rtfIndexRange,		"rxe",		0,
	rtfDestination,	rtfTOC,			"tc",		0,
	rtfDestination,	rtfNeXTGraphic,		"NeXTGraphic",	0,

	/*
	 * Font families
	 */

	rtfFontFamily,	rtfFFNil,		"fnil",		0,
	rtfFontFamily,	rtfFFRoman,		"froman",	0,
	rtfFontFamily,	rtfFFSwiss,		"fswiss",	0,
	rtfFontFamily,	rtfFFModern,		"fmodern",	0,
	rtfFontFamily,	rtfFFScript,		"fscript",	0,
	rtfFontFamily,	rtfFFDecor,		"fdecor",	0,
	rtfFontFamily,	rtfFFTech,		"ftech",	0,
	rtfFontFamily,	rtfFFBidirectional,	"fbidi",	0,

	/*
	 * Font attributes
	 */

	rtfFontAttr,	rtfFontCharSet,		"fcharset",	0,
	rtfFontAttr,	rtfFontPitch,		"fprq",		0,
	rtfFontAttr,	rtfFontCodePage,	"cpg",		0,
	rtfFontAttr,	rtfFTypeNil,		"ftnil",	0,
	rtfFontAttr,	rtfFTypeTrueType,	"fttruetype",	0,

	/*
	 * File table attributes
	 */

	rtfFileAttr,	rtfFileNum,		"fid",		0,
	rtfFileAttr,	rtfFileRelPath,		"frelative",	0,
	rtfFileAttr,	rtfFileOSNum,		"fosnum",	0,

	/*
	 * File sources
	 */

	rtfFileSource,	rtfSrcMacintosh,	"fvalidmac",	0,
	rtfFileSource,	rtfSrcDOS,		"fvaliddos",	0,
	rtfFileSource,	rtfSrcNTFS,		"fvalidntfs",	0,
	rtfFileSource,	rtfSrcHPFS,		"fvalidhpfs",	0,
	rtfFileSource,	rtfSrcNetwork,		"fnetwork",	0,

	/*
	 * Color names
	 */

	rtfColorName,	rtfRed,			"red",		0,
	rtfColorName,	rtfGreen,		"green",	0,
	rtfColorName,	rtfBlue,		"blue",		0,

	/*
	 * Charset names
	 */

	rtfCharSet,	rtfMacCharSet,		"mac",		0,
	rtfCharSet,	rtfAnsiCharSet,		"ansi",		0,
	rtfCharSet,	rtfPcCharSet,		"pc",		0,
	rtfCharSet,	rtfPcaCharSet,		"pca",		0,

	/*
	 * Table attributes
	 */

	rtfTblAttr,	rtfRowDef,		"trowd",	0,
	rtfTblAttr,	rtfRowGapH,		"trgaph",	0,
	rtfTblAttr,	rtfCellPos,		"cellx",	0,
	rtfTblAttr,	rtfMergeRngFirst,	"clmgf",	0,
	rtfTblAttr,	rtfMergePrevious,	"clmrg",	0,

	rtfTblAttr,	rtfRowLeft,		"trql",		0,
	rtfTblAttr,	rtfRowRight,		"trqr",		0,
	rtfTblAttr,	rtfRowCenter,		"trqc",		0,
	rtfTblAttr,	rtfRowLeftEdge,		"trleft",	0,
	rtfTblAttr,	rtfRowHt,		"trrh",		0,
	rtfTblAttr,	rtfRowHeader,		"trhdr",	0,
	rtfTblAttr,	rtfRowKeep,		"trkeep",	0,

	rtfTblAttr,	rtfRTLRow,		"rtlrow",	0,
	rtfTblAttr,	rtfLTRRow,		"ltrrow",	0,

	rtfTblAttr,	rtfRowBordTop,		"trbrdrt",	0,
	rtfTblAttr,	rtfRowBordLeft,		"trbrdrl",	0,
	rtfTblAttr,	rtfRowBordBottom,	"trbrdrb",	0,
	rtfTblAttr,	rtfRowBordRight,	"trbrdrr",	0,
	rtfTblAttr,	rtfRowBordHoriz,	"trbrdrh",	0,
	rtfTblAttr,	rtfRowBordVert,		"trbrdrv",	0,

	rtfTblAttr,	rtfCellBordBottom,	"clbrdrb",	0,
	rtfTblAttr,	rtfCellBordTop,		"clbrdrt",	0,
	rtfTblAttr,	rtfCellBordLeft,	"clbrdrl",	0,
	rtfTblAttr,	rtfCellBordRight,	"clbrdrr",	0,

	rtfTblAttr,	rtfCellShading,		"clshdng",	0,
	rtfTblAttr,	rtfCellBgPatH,		"clbghoriz",	0,
	rtfTblAttr,	rtfCellBgPatV,		"clbgvert",	0,
	rtfTblAttr,	rtfCellFwdDiagBgPat,	"clbgfdiag",	0,
	rtfTblAttr,	rtfCellBwdDiagBgPat,	"clbgbdiag",	0,
	rtfTblAttr,	rtfCellHatchBgPat,	"clbgcross",	0,
	rtfTblAttr,	rtfCellDiagHatchBgPat,	"clbgdcross",	0,
	/*
	 * The spec lists "clbgdkhor", but the corresponding non-cell
	 * control is "bgdkhoriz".  At any rate Macintosh Word seems
	 * to accept both "clbgdkhor" and "clbgdkhoriz".
	 */
	rtfTblAttr,	rtfCellDarkBgPatH,	"clbgdkhoriz",	0,
	rtfTblAttr,	rtfCellDarkBgPatH,	"clbgdkhor",	0,
	rtfTblAttr,	rtfCellDarkBgPatV,	"clbgdkvert",	0,
	rtfTblAttr,	rtfCellFwdDarkBgPat,	"clbgdkfdiag",	0,
	rtfTblAttr,	rtfCellBwdDarkBgPat,	"clbgdkbdiag",	0,
	rtfTblAttr,	rtfCellDarkHatchBgPat,	"clbgdkcross",	0,
	rtfTblAttr,	rtfCellDarkDiagHatchBgPat, "clbgdkdcross",	0,
	rtfTblAttr,	rtfCellBgPatLineColor, "clcfpat",	0,
	rtfTblAttr,	rtfCellBgPatColor,	"clcbpat",	0,

	/*
	 * Field attributes
	 */

	rtfFieldAttr,	rtfFieldDirty,		"flddirty",	0,
	rtfFieldAttr,	rtfFieldEdited,		"fldedit",	0,
	rtfFieldAttr,	rtfFieldLocked,		"fldlock",	0,
	rtfFieldAttr,	rtfFieldPrivate,	"fldpriv",	0,
	rtfFieldAttr,	rtfFieldAlt,		"fldalt",	0,

	/*
	 * Positioning attributes
	 */

	rtfPosAttr,	rtfAbsWid,		"absw",		0,
	rtfPosAttr,	rtfAbsHt,		"absh",		0,

	rtfPosAttr,	rtfRPosMargH,		"phmrg",	0,
	rtfPosAttr,	rtfRPosPageH,		"phpg",		0,
	rtfPosAttr,	rtfRPosColH,		"phcol",	0,
	rtfPosAttr,	rtfPosX,		"posx",		0,
	rtfPosAttr,	rtfPosNegX,		"posnegx",	0,
	rtfPosAttr,	rtfPosXCenter,		"posxc",	0,
	rtfPosAttr,	rtfPosXInside,		"posxi",	0,
	rtfPosAttr,	rtfPosXOutSide,		"posxo",	0,
	rtfPosAttr,	rtfPosXRight,		"posxr",	0,
	rtfPosAttr,	rtfPosXLeft,		"posxl",	0,

	rtfPosAttr,	rtfRPosMargV,		"pvmrg",	0,
	rtfPosAttr,	rtfRPosPageV,		"pvpg",		0,
	rtfPosAttr,	rtfRPosParaV,		"pvpara",	0,
	rtfPosAttr,	rtfPosY,		"posy",		0,
	rtfPosAttr,	rtfPosNegY,		"posnegy",	0,
	rtfPosAttr,	rtfPosYInline,		"posyil",	0,
	rtfPosAttr,	rtfPosYTop,		"posyt",	0,
	rtfPosAttr,	rtfPosYCenter,		"posyc",	0,
	rtfPosAttr,	rtfPosYBottom,		"posyb",	0,

	rtfPosAttr,	rtfNoWrap,		"nowrap",	0,
	rtfPosAttr,	rtfDistFromTextAll,	"dxfrtext",	0,
	rtfPosAttr,	rtfDistFromTextX,	"dfrmtxtx",	0,
	rtfPosAttr,	rtfDistFromTextY,	"dfrmtxty",	0,
	/* \dyfrtext no longer exists in spec 1.2, apparently */
	/* replaced by \dfrmtextx and \dfrmtexty. */
	rtfPosAttr,	rtfTextDistY,		"dyfrtext",	0,

	rtfPosAttr,	rtfDropCapLines,	"dropcapli",	0,
	rtfPosAttr,	rtfDropCapType,		"dropcapt",	0,

	/*
	 * Object controls
	 */

	rtfObjAttr,	rtfObjEmb,		"objemb",	0,
	rtfObjAttr,	rtfObjLink,		"objlink",	0,
	rtfObjAttr,	rtfObjAutoLink,		"objautlink",	0,
	rtfObjAttr,	rtfObjSubscriber,	"objsub",	0,
	rtfObjAttr,	rtfObjPublisher,	"objpub",	0,
	rtfObjAttr,	rtfObjICEmb,		"objicemb",	0,

	rtfObjAttr,	rtfObjLinkSelf,		"linkself",	0,
	rtfObjAttr,	rtfObjLock,		"objupdate",	0,
	rtfObjAttr,	rtfObjUpdate,		"objlock",	0,

	rtfObjAttr,	rtfObjHt,		"objh",		0,
	rtfObjAttr,	rtfObjWid,		"objw",		0,
	rtfObjAttr,	rtfObjSetSize,		"objsetsize",	0,
	rtfObjAttr,	rtfObjAlign,		"objalign",	0,
	rtfObjAttr,	rtfObjTransposeY,	"objtransy",	0,
	rtfObjAttr,	rtfObjCropTop,		"objcropt",	0,
	rtfObjAttr,	rtfObjCropBottom,	"objcropb",	0,
	rtfObjAttr,	rtfObjCropLeft,		"objcropl",	0,
	rtfObjAttr,	rtfObjCropRight,	"objcropr",	0,
	rtfObjAttr,	rtfObjScaleX,		"objscalex",	0,
	rtfObjAttr,	rtfObjScaleY,		"objscaley",	0,

	rtfObjAttr,	rtfObjResRTF,		"rsltrtf",	0,
	rtfObjAttr,	rtfObjResPict,		"rsltpict",	0,
	rtfObjAttr,	rtfObjResBitmap,	"rsltbmp",	0,
	rtfObjAttr,	rtfObjResText,		"rslttxt",	0,
	rtfObjAttr,	rtfObjResMerge,		"rsltmerge",	0,

	rtfObjAttr,	rtfObjBookmarkPubObj,	"bkmkpub",	0,
	rtfObjAttr,	rtfObjPubAutoUpdate,	"pubauto",	0,

	/*
	 * Associated character formatting attributes
	 */

	rtfACharAttr,	rtfACBold,		"ab",		0,
	rtfACharAttr,	rtfACAllCaps,		"caps",		0,
	rtfACharAttr,	rtfACForeColor,		"acf",		0,
	rtfACharAttr,	rtfACSubScript,		"adn",		0,
	rtfACharAttr,	rtfACExpand,		"aexpnd",	0,
	rtfACharAttr,	rtfACFontNum,		"af",		0,
	rtfACharAttr,	rtfACFontSize,		"afs",		0,
	rtfACharAttr,	rtfACItalic,		"ai",		0,
	rtfACharAttr,	rtfACLanguage,		"alang",	0,
	rtfACharAttr,	rtfACOutline,		"aoutl",	0,
	rtfACharAttr,	rtfACSmallCaps,		"ascaps",	0,
	rtfACharAttr,	rtfACShadow,		"ashad",	0,
	rtfACharAttr,	rtfACStrikeThru,	"astrike",	0,
	rtfACharAttr,	rtfACUnderline,		"aul",		0,
	rtfACharAttr,	rtfACDotUnderline,	"auld",		0,
	rtfACharAttr,	rtfACDbUnderline,	"auldb",	0,
	rtfACharAttr,	rtfACNoUnderline,	"aulnone",	0,
	rtfACharAttr,	rtfACWordUnderline,	"aulw",		0,
	rtfACharAttr,	rtfACSuperScript,	"aup",		0,

	/*
	 * Footnote attributes
	 */

	rtfFNoteAttr,	rtfFNAlt,		"ftnalt",	0,

	/*
	 * Key code attributes
	 */

	rtfKeyCodeAttr,	rtfAltKey,		"alt",		0,
	rtfKeyCodeAttr,	rtfShiftKey,		"shift",	0,
	rtfKeyCodeAttr,	rtfControlKey,		"ctrl",		0,
	rtfKeyCodeAttr,	rtfFunctionKey,		"fn",		0,

	/*
	 * Bookmark attributes
	 */

	rtfBookmarkAttr, rtfBookmarkFirstCol,	"bkmkcolf",	0,
	rtfBookmarkAttr, rtfBookmarkLastCol,	"bkmkcoll",	0,

	/*
	 * Index entry attributes
	 */

	rtfIndexAttr,	rtfIndexNumber,		"xef",		0,
	rtfIndexAttr,	rtfIndexBold,		"bxe",		0,
	rtfIndexAttr,	rtfIndexItalic,		"ixe",		0,

	/*
	 * Table of contents attributes
	 */

	rtfTOCAttr,	rtfTOCType,		"tcf",		0,
	rtfTOCAttr,	rtfTOCLevel,		"tcl",		0,

	/*
	 * Drawing object attributes
	 */

	rtfDrawAttr,	rtfDrawLock,		"dolock",	0,
	rtfDrawAttr,	rtfDrawPageRelX,	"doxpage",	0,
	rtfDrawAttr,	rtfDrawColumnRelX,	"dobxcolumn",	0,
	rtfDrawAttr,	rtfDrawMarginRelX,	"dobxmargin",	0,
	rtfDrawAttr,	rtfDrawPageRelY,	"dobypage",	0,
	rtfDrawAttr,	rtfDrawColumnRelY,	"dobycolumn",	0,
	rtfDrawAttr,	rtfDrawMarginRelY,	"dobymargin",	0,
	rtfDrawAttr,	rtfDrawHeight,		"dobhgt",	0,

	rtfDrawAttr,	rtfDrawBeginGroup,	"dpgroup",	0,
	rtfDrawAttr,	rtfDrawGroupCount,	"dpcount",	0,
	rtfDrawAttr,	rtfDrawEndGroup,	"dpendgroup",	0,
	rtfDrawAttr,	rtfDrawArc,		"dparc",	0,
	rtfDrawAttr,	rtfDrawCallout,		"dpcallout",	0,
	rtfDrawAttr,	rtfDrawEllipse,		"dpellipse",	0,
	rtfDrawAttr,	rtfDrawLine,		"dpline",	0,
	rtfDrawAttr,	rtfDrawPolygon,		"dppolygon",	0,
	rtfDrawAttr,	rtfDrawPolyLine,	"dppolyline",	0,
	rtfDrawAttr,	rtfDrawRect,		"dprect",	0,
	rtfDrawAttr,	rtfDrawTextBox,		"dptxbx",	0,

	rtfDrawAttr,	rtfDrawOffsetX,		"dpx",		0,
	rtfDrawAttr,	rtfDrawSizeX,		"dpxsize",	0,
	rtfDrawAttr,	rtfDrawOffsetY,		"dpy",		0,
	rtfDrawAttr,	rtfDrawSizeY,		"dpysize",	0,

	rtfDrawAttr,	rtfCOAngle,		"dpcoa",	0,
	rtfDrawAttr,	rtfCOAccentBar,		"dpcoaccent",	0,
	rtfDrawAttr,	rtfCOBestFit,		"dpcobestfit",	0,
	rtfDrawAttr,	rtfCOBorder,		"dpcoborder",	0,
	rtfDrawAttr,	rtfCOAttachAbsDist,	"dpcodabs",	0,
	rtfDrawAttr,	rtfCOAttachBottom,	"dpcodbottom",	0,
	rtfDrawAttr,	rtfCOAttachCenter,	"dpcodcenter",	0,
	rtfDrawAttr,	rtfCOAttachTop,		"dpcodtop",	0,
	rtfDrawAttr,	rtfCOLength,		"dpcolength",	0,
	rtfDrawAttr,	rtfCONegXQuadrant,	"dpcominusx",	0,
	rtfDrawAttr,	rtfCONegYQuadrant,	"dpcominusy",	0,
	rtfDrawAttr,	rtfCOOffset,		"dpcooffset",	0,
	rtfDrawAttr,	rtfCOAttachSmart,	"dpcosmarta",	0,
	rtfDrawAttr,	rtfCODoubleLine,	"dpcotdouble",	0,
	rtfDrawAttr,	rtfCORightAngle,	"dpcotright",	0,
	rtfDrawAttr,	rtfCOSingleLine,	"dpcotsingle",	0,
	rtfDrawAttr,	rtfCOTripleLine,	"dpcottriple",	0,

	rtfDrawAttr,	rtfDrawTextBoxMargin,	"dptxbxmar",	0,
	rtfDrawAttr,	rtfDrawTextBoxText,	"dptxbxtext",	0,
	rtfDrawAttr,	rtfDrawRoundRect,	"dproundr",	0,

	rtfDrawAttr,	rtfDrawPointX,		"dpptx",	0,
	rtfDrawAttr,	rtfDrawPointY,		"dppty",	0,
	rtfDrawAttr,	rtfDrawPolyCount,	"dppolycount",	0,

	rtfDrawAttr,	rtfDrawArcFlipX,	"dparcflipx",	0,
	rtfDrawAttr,	rtfDrawArcFlipY,	"dparcflipy",	0,

	rtfDrawAttr,	rtfDrawLineBlue,	"dplinecob",	0,
	rtfDrawAttr,	rtfDrawLineGreen,	"dplinecog",	0,
	rtfDrawAttr,	rtfDrawLineRed,		"dplinecor",	0,
	rtfDrawAttr,	rtfDrawLinePalette,	"dplinepal",	0,
	rtfDrawAttr,	rtfDrawLineDashDot,	"dplinedado",	0,
	rtfDrawAttr,	rtfDrawLineDashDotDot,	"dplinedadodo",	0,
	rtfDrawAttr,	rtfDrawLineDash,	"dplinedash",	0,
	rtfDrawAttr,	rtfDrawLineDot,		"dplinedot",	0,
	rtfDrawAttr,	rtfDrawLineGray,	"dplinegray",	0,
	rtfDrawAttr,	rtfDrawLineHollow,	"dplinehollow",	0,
	rtfDrawAttr,	rtfDrawLineSolid,	"dplinesolid",	0,
	rtfDrawAttr,	rtfDrawLineWidth,	"dplinew",	0,

	rtfDrawAttr,	rtfDrawHollowEndArrow,	"dpaendhol",	0,
	rtfDrawAttr,	rtfDrawEndArrowLength,	"dpaendl",	0,
	rtfDrawAttr,	rtfDrawSolidEndArrow,	"dpaendsol",	0,
	rtfDrawAttr,	rtfDrawEndArrowWidth,	"dpaendw",	0,
	rtfDrawAttr,	rtfDrawHollowStartArrow,"dpastarthol",	0,
	rtfDrawAttr,	rtfDrawStartArrowLength,"dpastartl",	0,
	rtfDrawAttr,	rtfDrawSolidStartArrow,	"dpastartsol",	0,
	rtfDrawAttr,	rtfDrawStartArrowWidth,	"dpastartw",	0,

	rtfDrawAttr,	rtfDrawBgFillBlue,	"dpfillbgcb",	0,
	rtfDrawAttr,	rtfDrawBgFillGreen,	"dpfillbgcg",	0,
	rtfDrawAttr,	rtfDrawBgFillRed,	"dpfillbgcr",	0,
	rtfDrawAttr,	rtfDrawBgFillPalette,	"dpfillbgpal",	0,
	rtfDrawAttr,	rtfDrawBgFillGray,	"dpfillbggray",	0,
	rtfDrawAttr,	rtfDrawFgFillBlue,	"dpfillfgcb",	0,
	rtfDrawAttr,	rtfDrawFgFillGreen,	"dpfillfgcg",	0,
	rtfDrawAttr,	rtfDrawFgFillRed,	"dpfillfgcr",	0,
	rtfDrawAttr,	rtfDrawFgFillPalette,	"dpfillfgpal",	0,
	rtfDrawAttr,	rtfDrawFgFillGray,	"dpfillfggray",	0,
	rtfDrawAttr,	rtfDrawFillPatIndex,	"dpfillpat",	0,

	rtfDrawAttr,	rtfDrawShadow,		"dpshadow",	0,
	rtfDrawAttr,	rtfDrawShadowXOffset,	"dpshadx",	0,
	rtfDrawAttr,	rtfDrawShadowYOffset,	"dpshady",	0,

	rtfVersion,	-1,			"rtf",		0,
	rtfDefFont,	-1,			"deff",		0,

	0,		-1,			NULL,	0
};


/*
 * Initialize lookup table hash values.  Only need to do this once.
 */

static void
LookupInit ()
{
static short	inited = 0;
RTFKey	*rp;

	if (inited == 0)
	{
		for (rp = rtfKey; rp->rtfKStr != NULL; rp++)
			rp->rtfKHash = Hash (rp->rtfKStr);
		++inited;
	}
}


/*
 * Determine major and minor number of control token.  If it's
 * not found, the class turns into rtfUnknown.
 */

static void
Lookup (char	*s)
{
RTFKey	*rp;
short	hash;

	++s;			/* skip over the leading \ character */
	hash = Hash (s);
	for (rp = rtfKey; rp->rtfKStr != NULL; rp++)
	{
		if (hash == rp->rtfKHash && strcmp (s, rp->rtfKStr) == 0)
		{
			rtfClass = rtfControl;
			rtfMajor = rp->rtfKMajor;
			rtfMinor = rp->rtfKMinor;
			return;
		}
	}
	rtfClass = rtfUnknown;
}


/*
 * Compute hash value of symbol
 */

static short
Hash (char	* s)
{
char	c;
short	val = 0;

	while ((c = *s++) != '\0')
		val += (short) c;
	return (val);
}


/* ---------------------------------------------------------------------- */

/*
 * Memory allocation routines
 */


/*
 * Return pointer to block of size bytes, or NULL if there's
 * not enough memory available.
 *
 * This is called through RTFAlloc(), a define which coerces the
 * argument to short.  This avoids the persistent problem of allocation
 * failing under THINK C when a long is passed.
 */

char *
_RTFAlloc (short size)
{
#ifdef DCLAP
	return ((char *) Nlm_MemNew(size));
#else
	return ((char *) malloc (size));
#endif
}


/*
 * Saves a string on the heap and returns a pointer to it.
 */


char *
RTFStrSave (char	*s)
{
#ifdef DCLAP
	return  Nlm_StringSave(s);
#else
char	*p;

	if ((p = RTFAlloc ((short) (strlen (s) + 1))) == NULL)
		return ( NULL);
	return (strcpy (p, s));
#endif
}


void
RTFFree (char	*p)
{
#ifdef DCLAP
	(void) Nlm_MemFree(p);
#else
	if (p != NULL)
		free (p);
#endif
}


/* ---------------------------------------------------------------------- */


/*
 * Token comparison routines
 */

short
RTFCheckCM (short aclass, short major)
{
	return (rtfClass == aclass && rtfMajor == major);
}


short
RTFCheckCMM (short aclass, short major, short minor)
{
	return (rtfClass == aclass && rtfMajor == major && rtfMinor == minor);
}


short
RTFCheckMM (short major, short minor)
{
	return (rtfMajor == major && rtfMinor == minor);
}


/* ---------------------------------------------------------------------- */


short
RTFCharToHex (char c)
{
	if (isupper (c))
		c = tolower (c);
	if (isdigit (c))
		return (c - '0');	/* '0'..'9' */
	return (c - 'a' + 10);		/* 'a'..'f' */
}


short
RTFHexToChar (short i)
{
	if (i < 10)
		return (i + '0');
	return (i - 10 + 'a');
}




/* ---------------------------------------------------------------------- */


/*
 * RTFReadOutputMap() -- Read output translation map
 */


/*
 * Read in a file describing the relation between the standard character set
 * and an RTF translator's corresponding output sequences.  Each line consists
 * of a standard character name and the output sequence for that character.
 *
 * outMap is an array of strings into which the sequences should be placed.
 * It should be declared like this in the calling program:
 *
 *	char *outMap[rtfSC_MaxChar];
 *
 * reinit should be non-zero if outMap should be initialized before reading the
 * file, zero otherwise.  (This allows the map to be constructed by reading
 * several files.)  It's assumed that any existing strings in the map were
 * allocated by RTFStrSave().  The map is initialized BEFORE any attempt is
 * made to read the file.
 *
 * If the filename is an absolute pathname, look in the specified location
 * only.  Otherwise try to find the file in the current directory or the
 * library directory.
 */

short
RTFReadOutputMap (char	*file, char	* outMap[], short reinit)
{
#if 0
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
FILE	*f;
char	buf[rtfBufSiz];
char	*name, *seq;
short	stdCode;
short	i;
TSScanner	scanner;
char		*scanEscape;
char		*fn = "RTFReadOutputMap";



	/* clobber current mapping */

	if (reinit)
	{
		for (i = 0; i < rtfSC_MaxChar; i++)
		{
			RTFFree (outMap[i]);
			outMap[i] =  NULL;
		}
	}

	if ((f = RTFOpenLibFile (file, "r")) == NULL)
		return (0);

	/*
	 * Turn off scanner's backslash escape mechanism while reading
	 * file.  Restore it later.
	 */
	TSGetScanner (&scanner);
	scanEscape = scanner.scanEscape;
	scanner.scanEscape = "";
	TSSetScanner (&scanner);

	/* read file */

	while (fgets (buf, (short) sizeof (buf), f) != NULL)
	{
		if(buf[0] == '#')	/* skip comment lines */
			continue;
		TSScanInit (buf);
		if ((name = TSScan ()) == NULL)
			continue;	/* skip blank lines */
		if ((stdCode = RTFStdCharCode (name)) < 0)
		{
			RTFMsg ("%s: unknown character name: %s\n", fn, name);
			continue;
		}
		if ((seq = TSScan ()) == NULL)
		{
			RTFMsg ("%s: malformed output sequence line for character %s\n", fn, name);
			continue;
		}

#ifdef DCLAP
/* dgg ++ let us use same input & output maps, & read the 0xAB char codes back to real chars */
		if (isdigit(seq[0]) && seq[1] > ' ')	/* convert to ascii value */
		{
			char* p = seq;
			short   value;
			short   radix = 10;
			if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
				radix = 16;
				p += 2;
				}
			value = 0;
			while (*p != '\0') 
				value = value * radix + RTFCharToHex(*p++);
			if (value < charSetSize) {
				sprintf(buf,"%c", (unsigned char)value);
				seq= buf;
				}
		}
#endif
		if ((seq = RTFStrSave (seq)) == NULL)
			RTFPanic ("%s: out of memory", fn);
		outMap[stdCode] = seq;
	}
	scanner.scanEscape = scanEscape;
	TSSetScanner (&scanner);
	fclose(f);
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#endif
	return (1);
}



/* ---------------------------------------------------------------------- */

/*
 * Open a library file.
 */


static FILE	*(*libFileOpen) (char* file, char* mode) = NULL;



void
RTFSetOpenLibFileProc (LibFileOpen proc)
{
	libFileOpen = proc;
}


FILE * RTFOpenLibFile (char	*file, char	* mode)
{
	if (libFileOpen == NULL)
		return ( NULL);
	return ((*libFileOpen) (file, mode));
}


/* ---------------------------------------------------------------------- */

/*
 * Print message.  Default is to send message to stderr
 * but this may be overridden with RTFSetMsgProc().
 *
 * Message should include linefeeds as necessary.  If the default
 * function is overridden, the overriding function may want to
 * map linefeeds to another line ending character or sequence if
 * the host system doesn't use linefeeds.
 */


static void DefaultMsgProc (char	* s)
{
#ifdef DCLAP
	(void) Nlm_Message(MSG_OK, "%s", s);
#else
	fprintf (stderr, "%s", s);
#endif
}


static RTFFuncCharPtr	msgProc = DefaultMsgProc;


void
RTFSetMsgProc (RTFFuncCharPtr proc)
{
	msgProc = proc;
}


# ifdef STDARG

/*
 * This version is for systems with stdarg
 */

void
RTFMsg (char *fmt, ...)
{
char	buf[rtfBufSiz];

	va_list args;
	va_start (args,fmt);
	vsprintf (buf, fmt, args);
	va_end (args);
	(*msgProc) (buf);
}

# else /* !STDARG */

# ifdef	VARARGS


/*
 * This version is for systems that have varargs.
 */

void
RTFMsg (va_alist)
va_dcl
{
va_list	args;
char	*fmt;
char	buf[rtfBufSiz];

	va_start (args);
	fmt = va_arg (args, char *);
	vsprintf (buf, fmt, args);
	va_end (args);
	(*msgProc) (buf);
}

# else	/* !VARARGS */

/*
 * This version is for systems that don't have varargs.
 */

void
RTFMsg (fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9)
char	*fmt;
char	*a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9;
{
char	buf[rtfBufSiz];

	sprintf (buf, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	(*msgProc) (buf);
}

# endif	/* !VARARGS */
# endif /* !STDARG */


/* ---------------------------------------------------------------------- */


/*
 * Process termination.  Print error message and exit.  Also prints
 * current token, and current input line number and position within
 * line if any input has been read from the current file.  (No input
 * has been read if prevChar is EOF).
 */

static void DefaultPanicProc (char	*s)
{
#ifdef DCLAP
	/* ?? give use a choice of dying or not w/ MSG_YESNO ?? */
	/* (void) Nlm_Message(MSG_FATAL, "%s", s); */
	(void) Nlm_Message( MSG_OK, "%s", s); 
	/* gReaderIsDead= 1; */
#else
	fprintf (stderr, "%s", s);
	exit (1);
#endif
}


static RTFFuncCharPtr	panicProc = DefaultPanicProc;


void
RTFSetPanicProc (RTFFuncCharPtr proc)
{
	panicProc = proc;
}


# ifdef STDARG

/*
 * This version is for systems with stdarg
 */

void
RTFPanic (char *fmt, ...)
{
char	buf[rtfBufSiz];

	va_list args;
	va_start (args,fmt);
	vsprintf (buf, fmt, args);
	va_end (args);
	(void) strcat (buf, "\n");
	if (prevChar != EOF && rtfTextBuf !=  NULL)
	{
		rtfTextBuf[rtfTextLen] = '\0'; 
		sprintf (buf + strlen (buf),
			"Last token read was \"%s\" near line %ld, position %d.\n",
			rtfTextBuf, rtfLineNum, rtfLinePos);
	}
	(*panicProc) (buf);
}

# else /* !STDARG */

# ifdef	VARARGS


/*
 * This version is for systems that have varargs.
 */

void
RTFPanic (va_alist)
va_dcl
{
va_list	args;
char	*fmt;
char	buf[rtfBufSiz];

	va_start (args);
	fmt = va_arg (args, char *);
	vsprintf (buf, fmt, args);
	va_end (args);
	(void) strcat (buf, "\n");
	if (prevChar != EOF && rtfTextBuf !=  NULL)
	{
		rtfTextBuf[rtfTextLen] = '\0'; 
		sprintf (buf + strlen (buf),
			"Last token read was \"%s\" near line %ld, position %d.\n",
			rtfTextBuf, rtfLineNum, rtfLinePos);
	}
	(*panicProc) (buf);
}

# else	/* !VARARGS */

/*
 * This version is for systems that don't have varargs.
 */

void
RTFPanic (fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9)
char	*fmt;
char	*a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9;
{
char	buf[rtfBufSiz];

	sprintf (buf, fmt, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	(void) strcat (buf, "\n");
	if (prevChar != EOF && rtfTextBuf !=  NULL)
	{
		rtfTextBuf[rtfTextLen] = '\0'; 
		sprintf (buf + strlen (buf),
			"Last token read was \"%s\" near line %ld, position %d.\n",
			rtfTextBuf, rtfLineNum, rtfLinePos);
	}
	(*panicProc) (buf);
}

# endif	/* !VARARGS */
# endif /* !STDARG */







/* ------------------- TokenScan -------------------*/

typedef void (*VoidProc) (char	*p);
typedef char * (*CharPtrProc) (char	*p);


static short Search (char* s, char c);
static char	*CanonScan ();

static TSScanner	defScan =
	{ NULL, CanonScan, " \t", "\"'", "\\", "\n\r", 0 };
static TSScanner	curScan =
	{ NULL, CanonScan, " \t", "\"'", "\\", "\n\r", 0 };

static char	*curPos;
static short	emptyToken;


static void TSScanInit (char	*p)
{
	curPos = p;
	emptyToken = 0;
	if (curScan.scanInit != NULL)
		(*curScan.scanInit) (p);
}


static void TSSetScanner (TSScanner	*p)
{
	if (p == NULL || p->scanInit == NULL)
		curScan.scanInit = defScan.scanInit;
	else
		curScan.scanInit = p->scanInit;
	if (p == NULL || p->scanScan == NULL)
		curScan.scanScan = defScan.scanScan;
	else
		curScan.scanScan = p->scanScan;

	if (p == (TSScanner *) NULL || p->scanDelim == (char *) NULL)
		curScan.scanDelim = defScan.scanDelim;
	else
		curScan.scanDelim = p->scanDelim;
	if (p == (TSScanner *) NULL || p->scanQuote == (char *) NULL)
		curScan.scanQuote = defScan.scanQuote;
	else
		curScan.scanQuote = p->scanQuote;
	if (p == (TSScanner *) NULL || p->scanEscape == (char *) NULL)
		curScan.scanEscape = defScan.scanEscape;
	else
		curScan.scanEscape = p->scanEscape;
	if (p == (TSScanner *) NULL || p->scanEos == (char *) NULL)
		curScan.scanEos = defScan.scanEos;
	else
		curScan.scanEos = p->scanEos;

	if (p == (TSScanner *) NULL || p->scanFlags == 0)
		curScan.scanFlags = defScan.scanFlags;
	else
		curScan.scanFlags = p->scanFlags;
}


static void TSGetScanner (TSScanner	*p)
{
	p->scanInit = curScan.scanInit;
	p->scanScan = curScan.scanScan;

	p->scanDelim = curScan.scanDelim;
	p->scanQuote = curScan.scanQuote;
	p->scanEscape = curScan.scanEscape;
	p->scanEos = curScan.scanEos;

	p->scanFlags = curScan.scanFlags;
}


static void TSSetScanPos (char	*p)
{
	curPos = p;
}


static char * TSGetScanPos ()
{
	return (curPos);
}


/*
 * Search a (possibly NULL) string for a character.
 */

static short Search (char* s, char c)
{
char	c2;

	if (s != (char *) NULL)
	{
		while ((c2 = *s++) != '\0')
		{
			if (c == c2)
				return (1);
		}
	}
	return (0);
}


static short TSIsScanDelim (char c)
{
	return (Search (curScan.scanDelim, c));
}


static short TSIsScanQuote (char c)
{
	return (Search (curScan.scanQuote, c));
}


static short TSIsScanEscape (char c)
{
	return (Search (curScan.scanEscape, c));
}


static short TSIsScanEos (char c)
{
	if (c == '\0')		/* null character ALWAYS terminates string */
		return (1);
	return (Search (curScan.scanEos, c));
}


static short TSTestScanFlags (short flags)
{
	return ((curScan.scanFlags & flags) == flags);
}


static char *
TSScan ()
{
	return ((*curScan.scanScan) ());
}


static char *
CanonScan ()
{
char	*pos, *start, *p, c, quote = 0;
short	escape = 0, haveToken = 0;

	if (emptyToken)		/* fix to point into buffer */
	{
		emptyToken = 0;
		return ("");
	}
	pos = TSGetScanPos ();

	if (!TSTestScanFlags (tsNoConcatDelims))
	{
		while (TSIsScanDelim (*pos))		/* skip delimiters */
			++pos;
	}
	start = p = pos;			/* start of token */
	while (!TSIsScanEos (c = *pos))		/* eos terminates any token */
	{
		haveToken = 1;
		if (escape)			/* previous char was escape; */
		{				/* pass this one literally */
			*p++ = *pos++;
			escape = 0;
		}
		else if (TSIsScanEscape (c))	/* this char is escape; */
		{				/* pass next one literally */
			++pos;
			escape = 1;
		}
		else if (quote)			/* in quote mode */
		{
			if (c == quote)		/* end quote mode */
				quote = 0;
			else			/* no end quote yet */
				*p++ = *pos;	/* add char */
			++pos;			/* skip to next char */
		}
		else if (TSIsScanQuote (c))	/* begin quote mode */
		{
			++pos;
			quote = c;
		}
		else if (TSIsScanDelim (c))	/* end of token */
		{
			++pos;			/* skip past delimiter */
			/*
			 * If also end of string and all delims are significant,
			 * there's an implicit empty token at end.  We won't be
			 * able to tell that from looking at the string itself,
			 * so set a flag for next call.
			 */
			if (TSIsScanEos (*pos)
				&& TSTestScanFlags (tsNoConcatDelims))
				++emptyToken;
			break;
		}
		else
			*p++ = *pos++;
	}
	*p = '\0';
	TSSetScanPos (pos);
	return (haveToken ? start : (char *) NULL);
}

