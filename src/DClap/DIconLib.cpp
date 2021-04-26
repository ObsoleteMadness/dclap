// DIconLib.cp



#include "Dvibrant.h"
#include "DIconLib.h"
#include "DApplication.h"
#include "DList.h"
#include "DFile.h"
#include <stdio.h>

typedef	unsigned char	byte;
enum  BitFormat			{ macbits, msbits, xbits };
enum  BitmapFormat 	{ unknown, xbitmapfile, maciconrezfile};
 
DIconList* gIconList = NULL;

class DIconGlobals {
public:
   DIconGlobals() { gIconList = new DIconList(); }
};

static DIconGlobals	globals; // initializes globals here

// Unix CC and gcc have problems linking DIcon.o w/ only data
// change this to DIcons.h ??
#ifdef OS_UNIX
#ifdef OS_UNIX_IRIX
#include "DIcons.c++"
#else
#include "DIcons.cc"
#endif
#endif

#ifdef OS_UNIX_OSF1
#define StrPosInt unsigned int
#else
#define StrPosInt Nlm_Uint4
#endif


#if 0
static unsigned short testicons[] = {
	22213, 0, 13, 13, 12,
 0xff00, 0x8180, 0x81c0, 0x9040, 0xa840, 0xa840, 0x9240, 0x8240,
 0x8240, 0x8240, 0x8040, 0xffc0 
,
  1215, 0, 32, 32, 64,
 0x1fff, 0xfc00, 0x1000,  0x600, 0x13a0,  0x500, 0x13a0,  0x480,
 0x13a8, 0x2440, 0x13a0,  0x420, 0x13a0, 0xe7f0, 0x13a1, 0x1010,
 0x13a2,  0x810, 0x13a2,  0x810, 0x13aa,  0x890, 0x13a1, 0x1010,
 0x13e0, 0xe010, 0x1220,   0x10, 0x1220,   0x10, 0x1220,   0x10,
 0x122b, 0xfe90, 0x1222,  0x210, 0x11c2,  0x210, 0x1322, 0x1fd0,
 0x1322, 0x1050, 0x1212, 0x1050, 0x121b, 0xf250, 0x1210, 0x1050,
 0x1230, 0x1050, 0x1120, 0x1fd0, 0x11a0,   0x10, 0x10e0,   0x10,
 0x1230, 0x2090, 0x1000,   0x10, 0x1000,   0x10, 0x1fff, 0xfff0
,
  0,0,0,0,0,0 
};
#endif

static Boolean gByteFlipMade = false;

#ifdef NOTNOW_WIN_MOTIF

static byte    XByteFlip [256];

void MakeXByteFlip()
{
	short i, j;
  byte val, inv;

  for (i = 0; i < 256; i++) {
    inv = 0;
    val = (byte) i;
    for (j = 0; j < 8; j++) {
      inv = (inv << 1);
      inv += (val % 2);
      val = (val >> 1);
    }
    XByteFlip[i] = inv;
  }
  gByteFlipMade= true;
}

#else

void  MakeXByteFlip()
{
  gByteFlipMade= true;
}
#endif


void DIconList::WriteAsCode( char* filename) 
{
	if (!filename) 
		filename= (char*) gFileManager->GetOutputFileName("icons.c");
	if (filename) {
		long i, n= this->GetSize();
		char *sbreak= ",\n";
		DFile  iconf( filename, "w", "TEXT", "MMCC");
		iconf.Open();
		char buf[128];
		sprintf(buf, "static unsigned short iconlist[] = { /* %d icons */ {\n", n);
		iconf.WriteLine(buf);
		for (i=0; i<n; i++) {
			DIcon* ic= (DIcon*)IconAt(i);
			ic->WriteCode( &iconf);
			if (i == n-1) sbreak= ",\n 0, 0, 0, 0, 0, 0\n};\n";
			iconf.WriteLine( sbreak );
			}
		iconf.Close();
		}
}

void DIcon::WriteCode( DFile* iconf)
{
	short i, nwords= 1 + ((fBitmapSize-1) / sizeof(short) );
	char buf[512];
	
	sprintf( buf, " %d, %d, %d, %d, %d,\n", 
						fId, fSmall, fWidth, fHeight, nwords );
	iconf->WriteLine( buf);
	unsigned short *w = (unsigned short*) fBitmap;
	char comma= ',';
	for (i=0; i<nwords; i++) {
		Boolean last= (i == nwords-1);
		Boolean newline= (i % 8 == 7) || last;
		if (last) comma= ' ';
		sprintf( buf, " %#6x%c", *w, comma);
		iconf->WriteLine( buf, newline);
		w++;
		}
	 
}


//class DIcon : public DObject

DIcon::DIcon() :
	DObject(),
	fId(0), fWidth(0), fHeight(0), fSmall(false),
	fBitmapSize(0), fBitmap(NULL)
{ 
}
	
DIcon::DIcon( long id, short width, short height, const char* bitmap, long bitmapsize) : 
	DObject(),
	fId(id), fWidth(width), fHeight(height),fSmall(false),
	fBitmapSize(bitmapsize), fBitmap(NULL)
{
	Install( width, height, bitmap, bitmapsize);
}

DIcon::DIcon( IconStore* store) :
	DObject(), fBitmapSize(0), fBitmap(NULL)
{
	Install( -(store->width), store->height, (char*) &(store->bitmap), 
					 store->bitmapsize * sizeof(short));
	fId= store->id;
	fSmall= store->smalli;
	if (gIconList) gIconList->InsertLast( this);
}

	
DIcon::~DIcon() 
{ 
	fBitmap= (char*)Nlm_MemFree(fBitmap); 
}


void DIcon::Draw(Nlm_RecT area)
{
	area.right  = Min( area.right, area.left + fWidth);
	area.bottom = Min( area.bottom, area.top + fHeight);
	Nlm_CopyBits( &area, fBitmap);
}
		
void DIcon::Install( short width, short height, const char* bitmap, long bitmapsize) 
{
  Boolean flipbyte= (width<0);
  if (flipbyte) width = -width;

	fWidth = width;
	fHeight= height;
	if (fBitmap) fBitmap= (char*)Nlm_MemFree(fBitmap);
	fBitmapSize= 0;

	if (bitmap) {		
		fBitmapSize = bitmapsize;
		if ((fBitmap = (char*) Nlm_MemGet(fBitmapSize+2, FALSE)) != NULL) {

#ifdef IS_LITTLE_ENDIAN
// damn, really I should just revise the compiled data form to use bytes instead of words
// this is just a word little/big endian problem....
//#if defined(WIN_MOTIF) || defined(WIN_MSWIN)
       if (flipbyte) {
         byte *storebyte, *bp, *bpend;
         //if (!gByteFlipMade) MakeXByteFlip();  
         storebyte= (byte*) fBitmap;
         bp = (byte*) bitmap;
         bpend= bp + bitmapsize;
         while (bp < bpend) { 
         		*storebyte++= bp[1];
         		*storebyte++= bp[0];
         		bp += 2;
					}
         }
       else
#endif
				Nlm_MemCpy (fBitmap, bitmap, fBitmapSize); // +1 bad for mswin?
				fBitmap[fBitmapSize]= 0;
	    	}
		}
} 
 



//class DIconList : public DList

DIconList::DIconList()
{
}


DIcon* DIconList::IconById(long id) 
{
	if (id) {
		long i, n= this->GetSize();
		for (i= 0; i<n; i++) {
			if (IconAt(i)->fId == id) return IconAt(i);
			}
	}
	return NULL;
}

void DIconList::StoreFromCode( unsigned short* ilist, short nlist) 
{
	Boolean notdone;
	IconStore* store;
	unsigned short* next;
	short i= 0;
	
	store= (IconStore*) ilist;	
	notdone= (store->bitmapsize) && (nlist==0 || (i<nlist));
	while (notdone) {
		next= ilist + sizeof(IconStore) + (store->bitmapsize - 1) * sizeof(short);
		DIcon* ic= new DIcon( store);
		this->InsertLast( ic);
		i++;
		ilist= next;
		store= (IconStore*) ilist;
		notdone= (store->bitmapsize) && (nlist==0 || (i<nlist));
		}
}


void DIconList::ReadAppIcons() 
{
#ifdef OS_DOS
  char *suffix = ".icn"; // ?? or ".ico", which is also MSWin own format
#else
  char *suffix = ".icons";
#endif
	char *iconfile = NULL;

  //MakeXByteFlip();
	long len= StrLen(  (char*)gApplication->Shortname()) + StrLen(suffix);
	iconfile= (char*) MemNew(len+2);
	StrCpy(iconfile, (char*)gApplication->Shortname());
	StrCat(iconfile, suffix);
	this->Read(iconfile);
	Nlm_MemFree(iconfile);
}
	
	

 
Boolean DIconList::Read(char* filename) 
{
	BitmapFormat	fileformat= unknown;
	FILE *f= FileOpen( filename, "r");
	if (f) {
		char line[512];
		Boolean more= true;
		short	atline = 0;
		while (more && FileGets( line, 512, f)) {
			StrPosInt index = 0;
			atline++;

			if (Nlm_StrngPos(line, "char ", 0, true, &index)  
			|| Nlm_StrngPos(line, "static ", 0, true, &index)) {
				fileformat= xbitmapfile;	
				more= false;
				}
				
			else if (Nlm_StrngPos(line, "resource ", 0, true, &index))  {
				fileformat= maciconrezfile;	
				more= false;
				}
			
			if (atline > 50) more= false;		
			}
		}
	FileClose( f);
	
	switch (fileformat) {
		case xbitmapfile		: return this->ReadXbitmapFormat( filename);
		case maciconrezfile	: return this->ReadMacRezFormat( filename);
		default							: return false;
		}
		
}



Boolean DIconList::ReadMacRezFormat(char* filename) 
{
/******
mac rez file format:
resource 'ICN#' (128, "Calc Family", purgeable) {
{	$"..."
	$"...",
	// icon mask 
	$"..."
	$"..."
  }
};

resource 'ics#' (128, "Calc Family", purgeable) {
};

resource 'ICON' (kSeedIconId, purgeable) {
	$"0001 0000 0002 8000 0004 4000 0008 2000 0010 1000 0020 0800 0040 0400 0080 0200"
	$"0100 0100 0200 0080 0400 0040 0800 0020 1000 0010 2000 0008 4000 3F04 8000 4082"
	$"4000 8041 2001 3022 1001 C814 080E 7F8F 0402 3007 0201 0007 0100 8007 0080 6007"
	$"0040 1FE7 0020 021F 0010 0407 0008 0800 0004 1000 0002 2000 0001 4000 0000 8000"
};
******/

	const	short	bufsize = 512;
	char  line[512];
	byte	buf[bufsize];
	byte *bitsp= NULL, *bitmap = NULL;
	Boolean inbits= false, instring= false;
	short	width = 0, height = 0;
	long 	id = 0;
	char* linep= line;
	long 	bytesize= 0;
	long 	atsize= 0;
	Boolean gotdata= false;
	StrPosInt index;

	FILE *f= FileOpen( filename, "r");
	if (f) 
	 while (FileGets( line, 512, f)) {
		// assume may be multiple bitmaps / file
		linep= line;
		
		if (inbits) {
	suckbits:			
	
				// here we must watch for 
				//	"$" == flag for hex data following
				//	'"' == start/end of string data
				//	',' == end of one icon, another follows ?? we only get 1st & ingore mask?
				//	'}' ';' == end of resource
				
			char ec, *ep;
			char *cp= linep;
			unsigned short card;
			short rslt;
			
			if (!atsize || !bitsp) {
				bitsp= buf;
				atsize= 0;
				}
				
			while (*cp) {
			
				if (instring) {
					short ic;
					while (*cp && !isalnum(*cp)) cp++;
					for (ic= 0, ep= cp; 
						ic<2 && (*ep) && isalnum(*ep); 
						ep++, ic++) ;
					ec= *ep; *ep= 0; card= 0;
					rslt= sscanf( cp, "%hx", &card);
					*ep= ec;
					if ( atsize<bufsize && rslt>0 ) { 
						*bitsp++ = (byte) card;
						atsize++;
						}
					cp = ep;
					}	
				else
					cp++;
					
				if (*cp == '"') instring= !instring;
				else if (*cp == '}' || *cp == ';') inbits= instring = false;
				else if (*cp == ',') { 
						// skip rest of data in this rez.. ??
					while (*cp && !(*cp == '}' || *cp == ';')) cp++; 
					inbits= instring= false;
					} 
				
				if (atsize >= bufsize || !inbits) {
					//move buf[] data to bitmap store...
					long offset= bytesize;
					bytesize += atsize;
					if (!bitmap) 
						bitmap= (byte*) Nlm_MemGet( bytesize, false);
					else 
						bitmap= (byte*) Nlm_MemMore( bitmap, bytesize);
					Nlm_MemCopy( bitmap+offset, buf, atsize);
					bitsp= buf;
					atsize= 0;
					}
					
				if (!inbits) {					
						// guess at width/height
#if 1
					width= height= short( sqrt(bytesize * 8));
#else			
						// ?? do a simple int square root -- bad for non-base-2 values
					long w= bytesize * 8;
					long h= 1;
					while (w>h) {  
						w >>= 1;
						h <<= 1;
						}
					width = w;
					height= h;
#endif					
					DIcon* ic= new DIcon( id, width, height, (const char*)bitmap, bytesize);
					this->InsertLast( ic);
					width= height= id= 0;
					bitmap= (byte*) Nlm_MemFree(bitmap);
					bitsp= NULL;
					bytesize= atsize= 0;
					inbits= instring= false;
					gotdata= true;
					*cp= 0; // done
					}
					
				}
			} // end suckbits
			
						
		else if ( (linep= strchr(line,'{')) != NULL  && 
			 ( Nlm_StrngPos(line, "resource 'ICN#'", 0, true, &index)
			|| Nlm_StrngPos(line, "resource 'ics#'", 0, true, &index)
			|| Nlm_StrngPos(line, "resource 'ICON'", 0, true, &index) ) )
			{
			// !! get id value == first # after "("
			char *nump= strchr(line,'(');
			if (nump) {
				char *ep, ec;
				nump++;
				for (ep= nump; isdigit(*ep); ep++) ;
				ec= *ep; *ep= 0;
				id= atol(nump);
				*ep= ec;
				}
			linep++;
			inbits= true;
			goto suckbits; 
			}

		
		}
		
	FileClose(f);
	return gotdata;
}



Boolean DIconList::ReadXbitmapFormat(char* filename) 
{
/******
Xbitmap file format:
#define  somename_width  16
#define  somename_height 32
#define  somename_bittype  x    #< optional addition to specify BitFormat
static char somename [] = {			#< data starts after "{", ends after "}"
   0x0f, 0xc0, 0x12, 0xff };    #< bitmap data as hex or decimal string of chars
******/
	Boolean gotdata= false;
	char line[512];
	byte *bitsp= NULL, *bitmap = NULL;
	Boolean inbits= false;
	short	width = 0, height = 0;
	long id = 0;
	char* linep= line;
	long 	bytesize= 0;
	long 	atsize= 0;
	short	bswap = 0;
	BitFormat bitstype = macbits;

                // set win system defaults -- override if file says to.
#ifdef  WIN_MAC
	bitstype = macbits;
#endif
#ifdef WIN_MSWIN
	bitstype = msbits;
#endif
#ifdef WIN_MOTIF
	bitstype = xbits;
#endif

	FILE *f= FileOpen( filename, "r");
	if (f) 
	 while (FileGets( line, 512, f)) {
		// assume may be multiple bitmaps / file
		linep= line;
		
		if (inbits) {
	suckbits:			
	  			// data looks like: 0x80, 0x01, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00};
			char* cp= linep;
			if (!bitmap) {
				bytesize= height * ((width+7) / 8); // bytes/row + rowcount
				bitmap= (byte*) Nlm_MemGet( bytesize+1, false);
				bitsp= bitmap;
				}
				
			while (*cp) {
			
				if (*cp == '}' || *cp == ';') {
					// install data into new icon...
					*bitsp= 0; // 0 terminate? -- this isn't really useful, since bitmap can have nulls
					DIcon* ic= new DIcon( id, width, height, (const char*)bitmap, atsize);
					this->InsertLast( ic);
					width= height= id= 0;
					bitmap= (byte*) Nlm_MemFree(bitmap);
					bitsp= NULL;
					bswap= bytesize= atsize= 0;
					inbits= false;
					gotdata= true;
					*cp= 0; // done
					}
					
				else if (isdigit(*cp)) {
					short rslt= 0;
					unsigned short card= 0;
					char *fmt = "%hu";
					char ec, *ep = cp;
					Boolean okay= true;
					
					while (okay) {
						ep++;
						if (*ep == 'x' || *ep == 'X') fmt= "%hx";  
						else if (!*ep || !isalnum(*ep)) okay= false;
						}
					ec= *ep; *ep= 0;
					rslt= sscanf( cp, fmt, &card);
					*ep= ec;
					if ( atsize<bytesize && rslt>0 ) { 
						switch (bitstype) {
							case xbits: {
								// X bitmaps have WEIRD storage format !! see Nlm_CopyBits
								byte drac, i;
								for (i=0, drac= 0; i<8; i++) { drac <<= 1; drac += card & 1; card >>= 1; }
								*bitsp++ = (byte) drac;
								}break;
							case msbits: {
								*bitsp++ = ~(byte) card; // mswin is NOT
								}break;
							case macbits: {
								*bitsp++ = (byte) card;
								}break;
							}
						atsize++;
						}
					cp = ep;
					}
					
				//else if (*cp == ',') cp++;
				else cp++; // spaces, all else...
				}
			}
			
		else if (width && height && (linep= strchr(line,'{')) != NULL) {
			linep++;
			inbits= true;
			goto suckbits; 
			}
			
		else {
			//look for string w/ "#define somename_width 16" << "width" is key
			//or "#define somename_height 16" << "height" is key
			StrPosInt index = 0;
			char* cp;
			if (Nlm_StrngPos(line, "width", 0, false, &index)) { //false==case-insensitive
				cp= line + index + 5; //length(width)
				while (*cp && !isdigit(*cp)) cp++;
				width= atol(cp); //?? what is better than atol()	
				}			 	
			else if (Nlm_StrngPos(line, "height", 0, false, &index)) {
				cp= line + index + 5;  
				while (*cp && !isdigit(*cp)) cp++;
				height= atol(cp); //?? what is better than atol()	
				}			 	
			else if (Nlm_StrngPos(line, "ident", 0, false, &index)) { 
				cp= line + index + 5; 
				while (*cp & !isdigit(*cp)) cp++;
				id= atol(cp);
				}			 	
			else if (Nlm_StrngPos(line, "bitstype", 0, false, &index)) { 
				cp= line + index + 8; 
				index= 0;
				if (Nlm_StrngPos(cp, "mac", 0, false, &index)) bitstype= macbits; 
				else if (Nlm_StrngPos(cp, "x", 0, false, &index)) bitstype= xbits; 
				else if (Nlm_StrngPos(cp, "ms", 0, false, &index)) bitstype= msbits; 
				}			 	
			}
		
		}
		
	FileClose(f);
	return gotdata;
}
		


#if 0


// Xlibrary standard bitmap form:
// (width+7) div 8 = # bytes/row
#define left_ptr_width 16
#define left_ptr_height 16
#define left_ptr_x_hot 3
#define left_ptr_y_hot 1
static char left_ptr_bits[] = {
   0x00, 0x00, 0x08, 0x00, 0x18, 0x00, 0x38, 0x00, 0x78, 0x00, 0xf8, 0x00,
   0xf8, 0x01, 0xf8, 0x03, 0xf8, 0x07, 0xf8, 0x00, 0xd8, 0x00, 0x88, 0x01,
   0x80, 0x01, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00};

  
#endif
