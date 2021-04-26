/* dgg.h */
/* dggilbert additions to ncbi tools*/

#ifndef _DGGADD_
#define _DGGADD_

#include <ncbi.h>

#ifdef __cplusplus
extern "C" {
#endif

/* NOTE: ncbiwin.h has #undef true, #undef false to live w/ Type.h include that defines these */

#ifndef true
#define true   1
#endif

#ifndef false
#define false  0
#endif

#ifndef Global
#define Global
#endif 
 
#ifndef Local
#define Local  static
#endif 

/* d*mn 16bit mswin/borc defines size_t as 16bit uint ! I've been using it as ulong ! */ 
#define ulong  unsigned long
/* ?? or is a typdef preferred ??:  typedef		unsigned long 	ulong; */

#ifndef Max
#define Max	MAX
#endif

#ifndef Min
#define Min	MIN
#endif

#ifndef MinMax
#define MinMax(xmin,x,xmax) ((x>xmax) ? (xmax) : ((x<xmin) ? (xmin) : (x)))   
#endif
 
#ifndef odd
#define odd(x) ((x) & 0x00000001)
#endif

#ifndef is_upper
#define is_upper(c)	('A'<=(c) && (c)<='Z')
#endif

#ifndef is_lower
#define is_lower(c)	('a'<=(c) && (c)<='z')
#endif

#ifndef to_lower
#define to_lower(c)	((char)(is_upper(c) ? (c)+' ' : (c)))
#endif

#ifndef to_upper
#define to_upper(c)	((char)(is_lower(c) ? (c)-' ' : (c)))
#endif

#ifdef WIN_MOTIF
#define xdebug(x)   fprintf(stderr,"%s\n", x)
#define xdebugp(fmt,p) fprintf(stderr,fmt,p)
#else
#define xdebug(x)
#define xdebugp(fmt,p)
#endif

	/* deal w/ line termination variation among op systems */
extern char*	LineEnd;
extern short  LineEndSize;

	/* and do it as constants rather than a variable... */
	/* p.s., looks like we can assume LineEndSize == 1 always, mswin seems happy w/ LF only */
#ifdef WIN_MAC
#define LINEEND	"\015"
#define NEWLINE '\015'
#else
#define LINEEND "\012"
#define NEWLINE '\012'
#endif

	/* to bypass compilers that do strange things to "\r\n" */
#ifndef CRLF
#define CRLF	"\015\012"
#endif


#define StrDup	Nlm_StringSave	
#define Strcasecmp Nlm_StringICmp
#define Strncasecmp Nlm_StringNICmp

Nlm_CharPtr LIBCALL Dgg_StrExtendCat PROTO(( Nlm_CharPtr *dest, char *src));
#define StrExtendCat	Dgg_StrExtendCat	

Nlm_CharPtr LIBCALL Dgg_StrPrependCat PROTO(( Nlm_CharPtr *dest, char *src));
#define StrPrependCat	Dgg_StrPrependCat	

Nlm_CharPtr LIBCALL Dgg_MemExtendCat PROTO(( Nlm_CharPtr *dest, long dlen, char *src, long slen));
#define MemExtendCat	Dgg_MemExtendCat	

Nlm_CharPtr LIBCALL  Dgg_StrAppend PROTO((char *to, char *from));
#define StrAppend	Dgg_StrAppend	

long LIBCALL Dgg_Str2Idtype PROTO(( char *s));
#define Str2Idtype	Dgg_Str2Idtype	

const Nlm_CharPtr LIBCALL Dgg_Idtype2Str PROTO(( long id));
#define Idtype2Str	Dgg_Idtype2Str	

void Dgg_StrUpcase PROTO(( char *s));
#define StrUpcase	Dgg_StrUpcase	

void Dgg_StrLocase PROTO(( char *s));
#define StrLocase	Dgg_StrLocase	

extern Nlm_CharPtr* Dgg_ReadDefaultPrefs PROTO((FILE *fp, Nlm_Int2* nlines));
#define ReadDefaultPrefs	Dgg_ReadDefaultPrefs	

	/* gcc/sun linker has problems finding Nlm_ version of these */
void Dgg_IntToStr PROTO((Nlm_Int2 intval, Nlm_CharPtr str, Nlm_Int2 length, ulong maxsize));

void Dgg_LongToStr PROTO((Nlm_Int4 longval, Nlm_CharPtr str,  Nlm_Int2 length, ulong maxsize));


#ifdef OS_MAC
pascal	short	DetermineVRefNum( char* pathname, short vRefNum, short *realVRefNum);
pascal	short	GetDirID( short vRefNum, long dirID, char* name, long *theDirID, 
													Boolean *isDirectory);
extern void Nlm_PtoCstr (Nlm_CharPtr str); // in vibutils.c
extern void Nlm_CtoPstr (Nlm_CharPtr str); // in vibutils.c
#endif


#ifdef __cplusplus
}
#endif


#endif
