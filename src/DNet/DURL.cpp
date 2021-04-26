// DURL
// d.g.gilbert, Sep.94

#include <ncbi.h>
#include <dgg.h>

#include <DFile.h>
#include "DTCP.h"
#include "DNetObject.h"
#include "DGoClasses.h"
#include "DURL.h"




// DURL ------------------------------


DURL::DURL()
{
}


short DURL::IsURL( const char *line, char*& url, long maxline)
{	// adapated from Lynx
  char *cp, *lineend;
	
  // don't crash on an empty argument 
  if (line == NULL || *line == '\0') return(DNetOb::kUnknownProt);
	if (maxline) lineend = (char*)line + maxline;
	else lineend= StrChr( (char*)line,'\0');
	
	
	/// god damn over duplication -- combine this w/ NetOb's list of protos
	
	
	for (cp= (char*)line; cp < lineend && *cp!= ':'; cp++) ;
	if (*cp != ':') { url= (char*)line; return(DNetOb::kUnknownProt); }
	else if (cp[1] != '/' && cp[2] != '/') {
      // these are only ones that don't contain "://"  
	  if (!Nlm_StrNICmp( (url=cp-4),"news",4))					return(DNetOb::kNNTPprot);
	  else if (!Nlm_StrNICmp( (url=cp-6),"mailto",6))		return(DNetOb::kSMTPprot);
	  else if (!Nlm_StrNICmp( (url=cp-5),"whois",5))		return(DNetOb::kWhoisprot);
	  else if (!Nlm_StrNICmp( (url=cp-6),"finger",6))		return(DNetOb::kFingerprot);
	  else if (!Nlm_StrNICmp( (url=cp-4),"nntp",4))			return(DNetOb::kNNTPprot);
	  else if (!Nlm_StrNICmp( (url=cp-9),"newspost",8))	return(DNetOb::kUnsupportedProt);
		else 	{  
	  	while (cp>line && isalpha(cp[-1])) cp--;
	  	url= cp;
	  	return(DNetOb::kUnknownProt); 
	  	}
		}

  else if (!Nlm_StrNICmp( (url=cp-6),"gopher",6))		return(DNetOb::kGopherprot);
  else if (!Nlm_StrNICmp( (url=cp-4),"http",4))			return(DNetOb::kHTTPprot);
  else if (!Nlm_StrNICmp( (url=cp-3),"bop",3))			return(DNetOb::kBOPprot);
  else if (!Nlm_StrNICmp( (url=cp-3),"pop",3))			return(DNetOb::kPOPprot);
  else if (!Nlm_StrNICmp( (url=cp-3),"ftp",3))		 	return(DNetOb::kFTPprot);
  else if (!Nlm_StrNICmp( (url=cp-4),"file",4))	 		return(DNetOb::kFileprot);
  else if (!Nlm_StrNICmp( (url=cp-4),"wais",4))	 		return(DNetOb::kWAISprot);
  else if (!Nlm_StrNICmp( (url=cp-6),"telnet",6))		return(DNetOb::kTelnetprot);
  else if (!Nlm_StrNICmp( (url=cp-6),"tn3270",6))		return(DNetOb::kTN3270prot);
  else if (!Nlm_StrNICmp( (url=cp-6),"rlogin",6))	 	return(DNetOb::kUnsupportedProt);
  else if (!Nlm_StrNICmp( (url=cp-3),"afs",3))	 		return(DNetOb::kUnsupportedProt);
  else if (!Nlm_StrNICmp( (url=cp-8),"prospero",8))	return(DNetOb::kUnsupportedProt);
		// !! Need also to support User-Added protocols via Type/Handler method !
  else	{ 
  	while (cp>line && isalpha(cp[-1])) cp--;
  	url= cp;
  	return(DNetOb::kUnknownProt); 
  	}
}



// from Lynx

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

	
static unsigned char isAcceptable[96] =
/* Overencodes */
/*	Bit 0		alphaChars		-- see HTFile.h
**	Bit 1		alphaPlusChars -- as alphaChars but with plus.
**	Bit 2 ...	pathChars		-- as alphaPlusChars but with /
*/
/* 0 1 2 3 4 5 6 7 8 9 A B C D E F */
{  0,0,0,0,0,0,0,0,0,0,7,6,0,7,7,4,	/* 2x   !"#$%&'()*+,-./	 */
	 7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,	/* 3x  0123456789:;<=>?	 */
	 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,	/* 4x  @ABCDEFGHIJKLMNO  */
	 7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,7,	/* 5X  PQRSTUVWXYZ[\]^_	 */
	 0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,	/* 6x  `abcdefghijklmno	 */
	 7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0 };	/* 7X  pqrstuvwxyz{\}~	DEL */

#define OKAYCHAR(a,mask)	( a>=32 && a<128 && ((isAcceptable[a-32]) & mask))

char* DURL::EncodeChars( const char* str, unsigned char mask)
{
	static char *hexchars = "0123456789ABCDEF";
  const char * p;
  char * q;
  char * result;
  int unacceptable = 0;

  for (p=str; *p; p++)
 		if (!OKAYCHAR( (unsigned char)*p, mask)) 
			unacceptable++;
  result = (char *) MemNew( p - str + unacceptable + unacceptable + 1);
  if (result == NULL) return "";

  for (q=result, p=str; *p; p++) {
  	unsigned char a = *p;
		if (!OKAYCHAR(a, mask)) {
    	*q++ = HEX_ESCAPE;	
    	*q++ = hexchars[a >> 4];
    	*q++ = hexchars[a & 15];
			}
		else *q++ = *p;
  	}
  *q++ = 0;			
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

static char from_hex(char c)
{
    return  c >= '0' && c <= '9' ?  c - '0' 
    	    : c >= 'A' && c <= 'F'? c - 'A' + 10
    	    : c - 'a' + 10;	 
}

//static
char* DURL::DecodeChars( char * str)
{
  char * p = str;
  char * q = str;

  if (!str) return "";
  while(*p) {
   	if (*p == HEX_ESCAPE) {
    	p++;
    	if (*p) *q  = from_hex(*p++) * 16;
   	  if (*p) *q += from_hex(*p++);
      q++;
			} 
   else 
		*q++ = *p++; 
	 }
  *q++ = 0;
  return str;
} 


char* DURL::GetParts( const char* url, long whichparts, long urlsize)
{
	char *cp, *ep, sep, *newurl, *line, *newline;
	
  if (url == NULL || *url == '\0') return NULL;
	if (!urlsize) urlsize= StrLen(url);
	line= (char*) Nlm_MemNew(urlsize+1);
	Nlm_MemCopy(line, url, urlsize);
  line[urlsize]= 0;
	newline= (char*) Nlm_MemNew(urlsize+1);
  *newline= 0;
  
	long prot= IsURL( line, newurl);
	cp= newurl;
	ep= StrChr( newurl+3, ':');
	if (ep) ep++;
	if (*ep == '/') ep++;
	if (*ep == '/') ep++;
	if (whichparts & kPartProtocol) {
		sep= *ep; *ep= 0;
		StrCat( newline, cp);
		*ep= sep;
		}
	cp= ep;
	
	while (isspace(*cp)) cp++;
	ep= cp;
	while ( *ep && (OKAYCHAR(*ep, alphaPlusChars)) ) ep++;
	//while (*ep && (isalnum(*ep) || *ep == '.')) ep++;
	while (isspace(*ep)) ep++;
	if (whichparts & kPartHost) {
		sep= *ep; *ep= 0;
		StrCat( newline, cp);		
		*ep= sep;
		}
	cp= ep;  

	if (*cp == ':') {	// port num
		ep = cp + 1;
		while (isspace(*ep)) ep++;
		while (isdigit(*ep)) ep++;
		if (whichparts & kPartPort) {
			sep= *ep; *ep= 0;
			StrCat( newline, cp);		
			*ep= sep;
			}
		cp= ep;  
		}		
	while (isspace(*cp)) cp++;

	if (*newline && *cp == '/') {
		StrCat( newline, "/");		
		}
		
	if (whichparts & kPartPath) {
		if (*cp == '/') cp++;
		StrCat( newline, cp);		
		}

	MemFree( line);
	return newline;
}



Boolean DURL::ParseURL( DNetOb* nob, const char *url, long urlsize, Boolean verbatim)
{
  char *cp, *ep, *qp, sep, *line, *newurl;
	short prot;
	Boolean isLocalhost;
	
  if (!nob || url == NULL || *url == '\0') return false;
  if (!urlsize) urlsize= StrLen(url);

	  // can we do this space check safely?
  newurl= (char*)url;  while (isspace(*newurl)) { newurl++; urlsize--; }
  ep= newurl + urlsize - 1;  while (isspace(*ep)) { ep--; urlsize--; }
  
	line= (char*) Nlm_MemNew(urlsize+1);
	Nlm_MemCopy(line, newurl, urlsize);
  line[urlsize]= 0;
	prot= IsURL( line, newurl);
	if (prot <= DNetOb::kUnknownProt) {
		MemFree( line);
		return false;
		}
	else {
		// need to drop "", '', <> or other delimiters from url end
		// also filter out newlines, controls, and spaces unless "" or ''
		{
			cp= newurl;
			if (cp>line) do { cp--; } while (cp>line && !isgraph(*cp));
			ep= cp;
			switch (*cp) {
				case '"': 
					newurl= cp+1;  
					do { if (isprint(*cp)) *ep++= *cp; cp++; } while (*cp && *cp!='"');
					*ep= 0;
					break;
				case '\'':
					newurl= cp+1;  
					do { if (isprint(*cp)) *ep++= *cp; cp++; } while (*cp && *cp!='\'');
					*ep= 0;
					break;
				case '<':
					newurl= cp+1; 
					if (verbatim)
						do { if (isprint(*cp)) *ep++= *cp; cp++; } while (*cp && *cp!='>');
					else 
						do { if (isgraph(*cp)) *ep++= *cp; cp++; } while (*cp && *cp!='>');
					*ep= 0;
					break;
				default:
					newurl= cp;  
					if (verbatim)
						do { if (isprint(*cp)) *ep++= *cp; cp++; } while (*cp);
					else 
						do { if (isgraph(*cp)) *ep++= *cp; cp++; } while (*cp);
					*ep= 0;
					break;
				}
			}
		nob->StoreProtocol( prot);
 		nob->StoreURL(newurl);
		cp= StrChr(newurl+3,':');
		cp++;
		if (*cp == '/') cp++;
		if (*cp == '/') cp++;
		while (isspace(*cp)) cp++;
		if (prot == DNetOb::kSMTPprot) {
			DecodeChars(cp);
			nob->fType= kMailType; // mailto gopher kind  
			nob->StorePath(cp);
			MemFree( line);
			return true;
			}
			
		ep= cp;
		while ( *ep && (OKAYCHAR(*ep, alphaPlusChars)) ) ep++;
		//while (*ep && (isalnum(*ep) || *ep == '.' || *ep == '-')) ep++;

 		while (isspace(*ep)) ep++;
		sep= *ep; *ep= 0;
		isLocalhost = (ep - cp < 2); // || StrICmp( cp, "localhost")==0 );
		nob->StoreHost( cp);	
		*ep= sep;
		cp= ep; // NO: +1;

		if (sep == ':') {	// port num
			ep= ++cp;
			while (isspace(*ep)) ep++;
			while (isdigit(*ep)) ep++;
			sep= *ep; *ep= 0;
			nob->StorePort( cp);	
			*ep= sep;
			cp= ep; //+1;
			}		
		if (sep == '/') ;
		while (isspace(*cp)) cp++;

		qp= StrChr(cp, '?');
		if (qp) {
			*qp++= 0;
			nob->fQueryGiven= StrDup(qp);
			}
			
		DecodeChars(cp);	// ???

		switch (prot) {
			case DNetOb::kGopherprot: // gopher://host.name:70/00/path/to/data
				if (*cp) cp++;
				if (*cp == 0) {
					nob->fType= kTypeFolder;  
					nob->StorePath(""); 
					}
				else {
					nob->fType= *cp;  
					nob->StorePath( cp+1); 
					}
				break;
				
			case DNetOb::kFTPprot: // file:///path:to:file
			case DNetOb::kFileprot: // file:///path:to:file
				if (*cp == '/') { 		 
						// this leading slash is artifact of silly url syntax !!
						// skip for all????? but sometimes (non-local) this is valid
#if 1
					if (isLocalhost) cp++;
#else
#ifdef OS_VMS
					cp++; // vms, skip leading /
#endif
#ifdef OS_MAC
					cp++; // mac, skip leading /
#endif
#ifdef OS_DOS
					if (StrChr(cp,':')) cp++; // have "/C:\dos\path" form
					else *cp= '\\'; // have a "/dos\path" form
#endif
#endif
					}
				nob->fType= kTypeFile;  // !! need ftp folder handling !?
				nob->StorePath( cp);  
				break;

			case DNetOb::kHTTPprot:	// http://host.name:80/path/to/data
				nob->fType= kTypeHtml;  
				if (*cp == 0) nob->StorePath( "/");
				else nob->StorePath( cp);  
				break;

			case DNetOb::kTelnetprot:
				nob->fType= kTypeTelnet;  
				nob->StorePath( cp);  
				break;
			case DNetOb::kTN3270prot:
				nob->fType= kTypeTn3270;  
				nob->StorePath( cp);  
				break;

			case DNetOb::kFingerprot:
			case DNetOb::kWhoisprot:
				nob->fType= kTypeQuery;  
				nob->StorePath( cp);  
				break;
			
			case DNetOb::kWAISprot: // ?? or need Folder type or Waisdir & WaisDoc types?
			default:
				nob->fType= kTypeFile;  
				nob->StorePath( cp);  
				break;
			}
			
				// set a usable title !?
		const char *name= nob->GetPath();
		if ( name == NULL || *name == 0 || StrCmp(name, "/") == 0) 
			name= nob->GetHost();
		else 
			name= gFileManager->FilenameFromPath(name);
		if ( name && *name != 0 ) 
			nob->StoreName( (char*)name);  
			 
		}
	
	MemFree( line);
	return true;
}
