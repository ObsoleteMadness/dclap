/* dgg.c 
   additions to ncbi tool library
   d.g.gilbert
*/

#include <dgg.h>
#include <ncbi.h>



#ifdef OS_MAC
char* LineEnd = "\015";
short LineEndSize = 1;
#endif

#ifdef OS_UNIX
char* LineEnd = "\012";
short LineEndSize = 1;
#endif

#ifdef OS_DOS
/* do msdos apps now deal w/ lf only as lineend ??  */
char* LineEnd = "\012";
short LineEndSize = 1;
/*char* LineEnd = "\015\012";  */
/*short LineEndSize = 2; */
#endif

/* I don't know what VMS/ohters use -- guess at LF */

#if !defined(OS_MAC) && !defined(OS_UNIX) && !defined(OS_DOS)
char* LineEnd = "\012";
short LineEndSize = 1;
#endif


/* damn stupid sun/gcc linker can't find these two in libvibrant.a... */
void Dgg_IntToStr (Nlm_Int2 intval, Nlm_CharPtr str,
                          Nlm_Int2 length, ulong maxsize)
{
  Nlm_Char  temp [80];

  sprintf (temp, "%*d", length, intval);
  Nlm_StringNCpy (str, temp, maxsize);
}

void Dgg_LongToStr (Nlm_Int4 longval, Nlm_CharPtr str,
                           Nlm_Int2 length, ulong maxsize)
{
  Nlm_Char  temp [80];

  sprintf (temp, "%*ld", length, longval);
  Nlm_StringNCpy (str, temp, maxsize);
}



void Dgg_StrUpcase( char FAR *s)
{
	if (s) for ( ; *s; s++) *s= to_upper(*s);
}

void Dgg_StrLocase( char FAR *s)
{
	if (s) for ( ; *s; s++) *s= to_lower(*s);
}
			

long LIBCALL Dgg_Str2Idtype( char FAR *s)
{
	/* arrrggghhhh -- 16bit os needs typecast to long */
	if (s) {
		long id;
		id= ((((long)s[0])<<24) | (((long)s[1])<<16) | (((long)s[2])<<8) | s[3]);
		return id;
		}
	else return 0;
}

		
const Nlm_CharPtr LIBCALL Dgg_Idtype2Str( long id)
{
	static char buf[5];
	buf[0] = (id>>24) & 0xff;
	buf[1] = (id>>16) & 0xff;
	buf[2] = (id>> 8) & 0xff;
	buf[3] = (id    ) & 0xff;
	buf[4]= '\0';
	return buf;
}

Nlm_CharPtr LIBCALL Dgg_StrPrependCat( Nlm_CharPtr *dest, char FAR *src)
{
	long newlen, oldlen;
	if (dest == NULL) return NULL;
	newlen= Nlm_StrLen( src);
	if (*dest) {
		oldlen= Nlm_StrLen( *dest);
		*dest= (Nlm_CharPtr) Nlm_MemMore( *dest, oldlen+newlen+1);
		if (*dest) {
			Nlm_MemMove( *dest + newlen, *dest, oldlen+1);
			Nlm_MemCopy( *dest, src, newlen);
			}
		}
	else {
		*dest= (Nlm_CharPtr) Nlm_MemDup( src, newlen);
		}
	return *dest;		
}

Nlm_CharPtr LIBCALL Dgg_StrExtendCat( Nlm_CharPtr *dest, char FAR *src)
{
	long newlen, oldlen;
	if (dest == NULL) return NULL;
	newlen= 1 + Nlm_StrLen( src);
	if (*dest) {
		oldlen= Nlm_StrLen( *dest);
		*dest= (Nlm_CharPtr) Nlm_MemMore( *dest, oldlen+newlen);
		if (*dest) Nlm_MemCopy( *dest+oldlen, src, newlen);
		}
	else {
		*dest= (Nlm_CharPtr) Nlm_MemDup( src, newlen);
		}
	return *dest;		
}


Nlm_CharPtr LIBCALL Dgg_MemExtendCat( Nlm_CharPtr *dest, long dlen, char FAR *src, long slen)
{
	if (dest == NULL) return NULL;
	if (*dest) {
		*dest= (Nlm_CharPtr) Nlm_MemMore( *dest, dlen+slen);
		if (*dest) Nlm_MemCopy( *dest+dlen, src, slen);
		}
	else {
		*dest= (Nlm_CharPtr) Nlm_MemDup( src, slen);
		}
	return *dest;		
}


Nlm_CharPtr LIBCALL  Dgg_StrAppend (char FAR *to,  char FAR *from)
{
	if (to) {
	 long fromlen, tolen;
	 if (!from) return to;
	 fromlen= 1+StrLen(from);
	 tolen= StrLen(to);
	 to= (Nlm_CharPtr) MemMore( to, tolen+fromlen);
	 MemCopy( to+tolen, from, fromlen);
	 return to;
	 }
	else
	 return NULL;
}



/*****************************************************************************
*
*   DCLAP needs to be able to move prefs data from distribution file to
*   user prefs file.
*
*	Result is list of all valid prefs lines from file.
*   Section name lines start w/ '[', name == line+1
*   Variable names include a '=' between name & paramaters
*   Comment lines are ignored
*   Recommend using [version=123] to signify new versions of prefs
*   data in same file.
*
*   This is from NCBI's coretools/ncbienv.c: Nlm_ReadConfigFile (fp)
*
*****************************************************************************/

#ifdef TESTING__MWERKS__
/* testing bug in fgets -- trashed data after a while... */
char* dgFgets( char* str, ulong strsize, FILE* fp)
{
#ifdef __MPW__
#define kNewline '\r'
#else
#define kNewline '\n'
#endif

	register int c = 0;
	register char* cp = str;
	
	if (!strsize) return NULL;
	c= getc( fp);
	strsize--;
	if (c == EOF) return NULL;
  		
	while (c != EOF && strsize) {
		*cp++= c;
		strsize--;
		if (c == kNewline) strsize= 0;
		if (strsize) c= getc( fp);
		}
	*cp= 0;
	return str;
}

#else
#define dgFgets(a,b,c) fgets(a,b,c)
#endif


extern Nlm_CharPtr* Dgg_ReadDefaultPrefs(FILE *fp, Nlm_Int2* nlines)
{
  Nlm_Char       	ch;
  Nlm_Char      	str [256];
  Nlm_CharPtr  		tmp, mid;
  Nlm_CharPtr	*  	linelist;
  Nlm_Int2		   	nlin, maxl;
  
  nlin = 0;
  linelist= NULL;
  if (fp) {
    maxl= 50;
  	linelist= (Nlm_CharPtr*) Nlm_MemNew(maxl * sizeof(Nlm_CharPtr));
    while (linelist && dgFgets (str, sizeof(str), fp)) {
      ch = *str;
      
     	if (ch < ' ' || ch == ';') {  /* ch == '\n' || ch == '\r' */ 
     		/* empty lines & comments -- skip */
     		}
     		
	    else if (ch == '[') {
      	/* this is all we want -- dgg */
     		tmp = str;
     		while (ch != '\0' && ch != ']') {
     			tmp++;
     			ch = *tmp;
       		}
     		*tmp = '\0';
      	nlin++;
      	if (nlin>=maxl) {
      		maxl= nlin+50;
	  			linelist= (Nlm_CharPtr*) Nlm_MemMore( linelist, maxl*sizeof(Nlm_CharPtr));
	  			}
      	if (linelist) linelist[ nlin-1 ]= Nlm_StringSave( str ); /* + 1);  -- ! leave leading '[' as key to sect */
        }
        
 			else {
    		tmp = str;
      	mid = NULL;
      	while (ch != '\0' && ch != '\n' && ch != '\r') {
	      	if (ch == '=') {
	       		mid = tmp;
	       		/* *mid = '\0'; */
	         	mid++;
	       		}
	      	tmp++;
	       	ch = *tmp;
	     		}
    		*tmp = '\0';
 				if (tmp>str) { /* was: if (mid) */
 					/* ?? save only variable=parameters lines - !!NO need 'var' alone to remove old vars*/
	       	nlin++;
	      	if (nlin>=maxl) {
	      		maxl= nlin+50;
		  			linelist= (Nlm_CharPtr*) Nlm_MemMore( linelist, maxl*sizeof(Nlm_CharPtr));
		  			}
	      	if (linelist) linelist[ nlin-1 ]= Nlm_StringSave( str);        
					}
 				
 				}
    	}
  	}
  *nlines= nlin;
  return linelist;
}

 


 /* damn dummy funcs for damn libXm which can't find damn /usr/ccs/lib/libgen funcs */


char * regcmp(const char *a, ...)
 {
   return NULL;
 }
 
char * regex(const char *a, const char *b, ...)
 {
 	return NULL;
 }



#ifdef OS_MAC

	// undef some conflicts b/n Types.h && NlmTypes
#undef Handle
//#undef true
//#undef false
//#undef Boolean
#include <Types.h>
#include <Memory.h>
#include <OSUtils.h>
#include <Files.h>
#include <Folders.h>

pascal	short	DetermineVRefNum( char* pathname, short vRefNum, short *realVRefNum)
{
	HParamBlockRec pb;
	Str255 tempPathname;
	short error;

	pb.volumeParam.ioVRefNum = vRefNum;
	if ( pathname == NULL ) {
		pb.volumeParam.ioNamePtr = NULL;
		pb.volumeParam.ioVolIndex = 0;		/* use ioVRefNum only */
		}
	else {
		BlockMoveData(pathname, tempPathname, pathname[0] + 1);	/* make a copy of the string and */
		pb.volumeParam.ioNamePtr = (StringPtr)tempPathname;	/* use the copy so original isn't trashed */
		pb.volumeParam.ioVolIndex = -1;	/* use ioNamePtr/ioVRefNum combination */
		}
	error = PBHGetVInfoSync(&pb);
	*realVRefNum = pb.volumeParam.ioVRefNum;
	return ( error );
}

pascal	short	GetDirID(short vRefNum, long dirID,
						 char* name, long *theDirID, Boolean *isDirectory)
{
	CInfoPBRec pb;
	short error;

	pb.hFileInfo.ioNamePtr = (StringPtr) name;
	pb.hFileInfo.ioVRefNum = vRefNum;
	pb.hFileInfo.ioDirID = dirID;
	pb.hFileInfo.ioFDirIndex = 0;	/* use ioNamePtr and ioDirID */
	error = PBGetCatInfoSync(&pb);
	*theDirID = pb.hFileInfo.ioDirID;
	*isDirectory = (pb.hFileInfo.ioFlAttrib & ioDirMask) != 0;
	return ( error );
}


#endif
