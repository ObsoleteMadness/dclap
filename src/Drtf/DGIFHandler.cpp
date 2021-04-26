// DGIFHandler.cpp
// d.g.gilbert


#include "Dvibrant.h"
#include "DFile.h"
#include "DGIFHandler.h"
#include "DRichProcess.h"
#include "DRichViewNu.h"  
#include "DDrawPICT.h"    
#include "DPICTprocess.h"    
#include <stdio.h>


typedef		unsigned char	byte;
typedef		unsigned char	pixel;
typedef		unsigned short	ushort;
//typedef		unsigned long 	ulong;

struct macRGBColor 
{
	ushort red;
	ushort green;
	ushort blue;
};


class DPictWriter;

class DGifReader
{
public:
	DGifReader( DFile* itsFile, DPictWriter* pictwriter = NULL, short imageNumber = 1);
	~DGifReader();
		
	const char*	 Error() 	{ return fError; }
	short	 Height() 	{ return fHeight; }
	short	 Width()  	{ return fWidth; }
	short  BitsPerPixel()	{ return fBitPixel; }
	char*	 GetPixels() 	{ return fPixels; }
	KolorTable GetColormap() { return fColortable; }
	long   NColors() { return fNColors; }
	
private:
	enum {
		MAXCOLORMAPSIZE=256,
		CM_RED=0,
		CM_GREEN=1,
		CM_BLUE=2,
		MAX_LZW_BITS=12,
		LZW_TABLE_SIZE = (1<<12), //1<<MAX_LZW_BITS
		INTERLACE	=0x40,
		MaxGETCBUF = 280,
		COLORMAPFLAG=0x80
		};
	
	DPictWriter*	fPictWriter;
	ushort	fWidth,fHeight,fBitPixel;
	ushort	fColorResolution,fBackground,fAspectRatio; // not used
	Boolean	fInterlace, fEof;
	char		* fPixels;	
	short		fImageNumber;
	KolorSpec	* fColormap;
	KolorTable	fColortable;
	byte		* fColorswap;
	short		fNColors;
	DFile		* fFile;
	char		* fError;
	struct {
		short	transparent;
		short	delayTime;
		short	inputFlag;
		short	disposal;
		} fGif89;
			// GetCode static vars
	byte	codebuf[MaxGETCBUF];
	long	curbit, lastbit, lastbyte;
	Boolean	out_of_blocks;
			//LZWReadByte static vars
	Boolean	lzwfresh;
	long	code_size, set_code_size, input_code_size;
	long	max_code, max_code_size;
	long	firstcode, oldcode, clear_code, end_code, limit_code;
	ushort lzwhead[LZW_TABLE_SIZE]; /* => table of prefix symbols */
	byte  lzwtail[LZW_TABLE_SIZE]; /* => table of suffix bytes */
	byte  lzwstack[LZW_TABLE_SIZE]; /* stack for symbol expansions */
	byte  *lzwsp;		/* stack pointer */
				
	void 		ReadImage( Boolean ignore);
	void 		ReadGIFtop();
	Boolean ReadColorMap();
	void		OpenPicture();
	void 		StorePoint( long x, long y, short color);
	Boolean DoExtension();
	short 	GetDataBlock( void *buf);
	void  	SkipDataBlocks();
	ulong  	GetCode();
	inline byte MapColor( byte oldcolor);
	long  	LZWReadByte();
	void 		ReInitLZW();
	void 		InitLZWCode( short inCodeSize);
	Boolean ReadOK( void* buffer, ulong len);
	short 	ReadByte();
	Boolean BitSet( byte byte, long bit)	{ return ( (byte & bit) == bit ); }
	ushort 	LM_to_uint( byte a, byte b) { return (ushort) (b << 8) | a ; }
};




class DPictWriter
{
public:
	DPictWriter( Boolean wantPictHeader= false, Boolean ownpict= false);
	DPictWriter( short rows, short cols, short ncolors, KolorTable colortable,
								char* pixels, Boolean wantPictHeader= false, Boolean ownpict= false);
	~DPictWriter();
	byte* Pict();  
	ulong	PictSize() { return fPictSize; }
	void  Open( short rows, short cols, short ncolors, KolorTable colortable);
	void  WriteLine( char* pixelrow) {	fBytesPut += putRow( pixelrow); }
	void	Close()  { PutPictEnd(); }
	
private:
	enum {	
		HEADER_SIZE =	512,
		PICT_clipRgn	=0x01,
		PICT_picVersion=0x11,
		PICT_PackBitsRect=0x98,
		PICT_EndOfPicture=0xFF,
		PICT_headerOp=0x0C00,
		RUN_THRESH = 3,
		MAX_RUN	= 128,		/* 0xff = 2, 0xfe = 3, etc */
		MAX_COUNT = 128		/* 0x00 = 1, 0x01 = 2, etc */
		};
	
	Boolean fWantHeader, fOwnPict;
	byte  * fPict;
	char 	* fPacked;
	ulong 	fPictSize, fPicSizeAt, fPictMax, fBytesPut;
	short		fWidth, fHeight, fRowbytes;
	short 	fNColors;
	KolorSpec * fColormap;
	KolorTable	fColortable;
	DFile*	fPicFile;

	void PutPictTop();
	void PutPictEnd();
	void putChar( byte c);
	void putFill( char c, ulong size);
	void putShort( short i);
	void putLong( long i);
	void putFixed( short in, short frac);
	long putRow( char* pixelrow);
	void putRect( short x1, short x2, short y1, short y2);
};




class DGIFprocess : public DPICTprocess
{
public:
	DGIFprocess( DFile* itsFile, DFile* itsTempData, DRichView* itsDoc, Nlm_MonitorPtr progress);
protected:
	virtual void  ReadDataFromFile();
};



//class DGIFprocess

DGIFprocess::DGIFprocess( DFile* itsFile, DFile* itsTempData, 
									DRichView* itsDoc, Nlm_MonitorPtr progress) :
		DPICTprocess( itsFile, itsTempData, itsDoc, progress)
{ 	
}
			
void DGIFprocess::ReadDataFromFile()
{	
	if (!fDataFile) fDataFile= fTempData;
	if ( fNotDone && fDataFile) {
		fDataFile->Close(); // !?!? are we done writing to fTempData ??
		fDataFile->Open("rb"); 
		fNotDone= false;
#if 1
		DPictWriter* picwriter = new DPictWriter( false, false);
		DGifReader* gif= new DGifReader( fDataFile, picwriter);
#else
		DGifReader* gif= new DGifReader( fDataFile);
		DPictWriter* picwriter = new DPictWriter( gif->Height(), gif->Width(),
					gif->NColors(), gif->GetColormap(), gif->GetPixels(), false, false);
#endif
		delete gif;

		fPict= (char*) picwriter->Pict();
		fPictSize= picwriter->PictSize();
		delete picwriter;
		}
}			



//class  DGIFHandler

DGIFHandler::DGIFHandler(DRichView* itsDoc, DFile* savefile) :
	DPICTHandler( itsDoc, savefile)
{
	fFormatName = "GIF";
	fMinDataToProcess= 10; // ?? only need 1st 6 bytes to check "GIF89a" or "GIF87a"
	//if (fDocFile) fDocFile->Open("wb"); // make sure it is binary
}

char* DGIFHandler::IsRich(char* buf, ulong buflen)
{
		// magic keys are "GIF87a" and "GIF89a" as 1st 6 bytes of data
	if (buflen < 6)
		return NULL;	
	if (buf[0] != 'G' || buf[1] != 'I' || buf[2] != 'F') 
		return NULL;
	if (buf[3] != '8' || (buf[4] != '7' && buf[4] != '9') || buf[5] != 'a') 
		return NULL;
  return buf;
}

DRichprocess* DGIFHandler::NewProcessor( DFile* itsFile, Nlm_MonitorPtr progress)
{
	fProcessor = new DGIFprocess( itsFile, fDocFile, fDoc, progress);
	return fProcessor;
}








/* from gifread.c */
/* +-------------------------------------------------------------------+ */
/* | Copyright 1990, David Koblas.                                     | */
/* |   Permission to use, copy, modify, and distribute this software   | */
/* |   and its documentation for any purpose and without fee is hereby | */
/* |   granted, provided that the above copyright notice appear in all | */
/* |   copies and that both that copyright notice and this permission  | */
/* |   notice appear in supporting documentation.  This software is    | */
/* |   provided "as is" without express or implied warranty.           | */
/* +-------------------------------------------------------------------+ */

/* and from jrdgif.c 
 * Copyright (C) 1991, 1992, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
*/


DGifReader::DGifReader( DFile* itsFile, DPictWriter* pictwriter, short imageNumber)
{
	fWidth = 0;
	fHeight = 0;
	fNColors= 0;
	fBitPixel = 8;
	fColorResolution = 0;
	fBackground = 0; // ? background color ??
	fAspectRatio = 0;
	fPixels = NULL;
	fColorswap = NULL;	
	fColormap = NULL;
	fPictWriter= pictwriter;
	fImageNumber= imageNumber;
	fFile= itsFile;
	fError= NULL;
	fInterlace= false;
	fEof= false;
	fGif89.disposal    = -1;
	fGif89.inputFlag   = -1;
	fGif89.delayTime   = -1;
	fGif89.transparent = 0;
	lzwfresh = false;
	
	if (fFile) ReadGIFtop();
}


DGifReader::~DGifReader()
{
	MemFree( fPixels);  
	MemFree( fColorswap);  
	MemFree( fColormap);  
}

Boolean DGifReader::ReadOK( void* buffer, ulong len)
{
	ulong count= len;
	short err= fFile->ReadData( buffer, count);
	//fEof = fFile->Eof();
	return (count > 0); // (count == len);
}

short DGifReader::ReadByte()
{
	short c = fgetc( fFile->fFile);
	fEof= (c == EOF);
	return c;
}


void DGifReader::ReadGIFtop()
{
	byte	buf[16];
	byte	c;
	short	imageCount = 0;
	
	if (! ReadOK(buf,6)) {
		fError= "error reading magic number";
		return;
		}

	if (buf[0] != 'G' || buf[1] != 'I' || buf[2] != 'F') {
		fError= "not a GIF file";
		return;
		}

	if (buf[3] != '8' || (buf[4] != '7' && buf[4] != '9') || buf[5] != 'a') {
		fError= "bad version number, not '87a' or '89a'";
		return;
		}

	if (! ReadOK(buf,7)) {
		fError= "failed to read screen descriptor";
		return;
		}

	fWidth 					 = LM_to_uint(buf[0],buf[1]); 
	fHeight					 = LM_to_uint(buf[2],buf[3]); 
	fBitPixel        = (buf[4] & 0x07) + 1;
	fColorResolution = ((buf[4] & 0x70) >> 3) + 1;
	fBackground      = buf[5];
	fAspectRatio     = buf[6];

	Boolean haveColorMap = BitSet(buf[4], COLORMAPFLAG);
	if (haveColorMap) {	 
		if (!ReadColorMap()) {
			fError= "error reading global colormap";
			return;
			}
		}

	if (fAspectRatio != 0 && fAspectRatio != 49) {
		float	r;
		r = ( (float) fAspectRatio + 15.0 ) / 64.0;
		//fError= "Warning:  non-square pixels!";
		}

	for (imageCount= 0; imageCount < fImageNumber; ) {
		c= ReadByte();
		if (fEof) {
			fError= "EOF / read error on image data";
			return;
			}

		if (c == ';') {		/* GIF terminator */
			if (imageCount < fImageNumber) {
				fError= "No images found in file";
				return;
				}
			break;
			}

		if (c == '!') { 	/* Extension */
			DoExtension();
			continue;
			}

		if (c != ',') {		/* Not a valid start character */
			//fError= "bogus character 0x%02x, ignoring"; // print( msg, c) 
			continue;
			}

		if (! ReadOK( buf,9)) {
			fError= "couldn't read left/top/width/height";
			return;
			}

		haveColorMap = BitSet(buf[8], COLORMAPFLAG);
		fBitPixel 	= (buf[8] & 0x07) + 1;
			// ^^^^^ if this is 1 ?2?, save lots of space doing bitmap not pixmap ??!
		fWidth 			= LM_to_uint(buf[4],buf[5]);
		fHeight 		= LM_to_uint(buf[6],buf[7]);
		fInterlace	= BitSet(buf[8], INTERLACE);

		if (haveColorMap) {
			if (!ReadColorMap()) {
				fError= "error reading local colormap";
				return;
				}
			} 
			
		++imageCount;			
		if (imageCount <= fImageNumber) {
			ReadImage(imageCount != fImageNumber);
			}
		}
}


Boolean DGifReader::ReadColorMap()
{
	byte	gifrgb[3];
	KolorSpec	kolor;
	long scale = 65536 / MAXCOLORMAPSIZE;
	long	i;

	fNColors = 1 << fBitPixel;
	if (fColormap) MemFree( fColormap);
	//fColormap= (macRGBColor*) MemNew( (ulong)fNColors * sizeof(macRGBColor)); 
	fColormap= (KolorSpec*) MemNew( (ulong)fNColors * sizeof(KolorSpec)); 
  fColortable.ctTable= fColormap;
	fColortable.ctFlags= 0;
	fColortable.ctSeed= 0;
	fColortable.ctSize= fNColors-1;
	
	if (!fColormap) {
		fError= "Can't allocate colormap array";
		return(false);
		}
	for (i = 0; i < fNColors; ++i) {
		if (! ReadOK( gifrgb, sizeof(gifrgb))) {
			fError= "bad colormap";
			return(false);
			}
		kolor.value= i;
		kolor.rgb.red 	= gifrgb[0] * scale;
		kolor.rgb.green = gifrgb[1] * scale;
		kolor.rgb.blue 	= gifrgb[2] * scale;
		fColormap[i] = kolor;
		}
		
#ifdef WIN_MOTIF
	//fColortable.ctFlags = DDrawPICT::kXColorTableFlag;
	MemFree( fColorswap);
	fColorswap= (byte*) MemNew( sizeof(byte)*257);
	DDrawPict::Pixmap2Xcolors( fColortable, fColorswap, 256, true);
#endif

	return true;
}


void DGifReader::StorePoint( long x, long y, short color)
{
	byte *dptr = (byte *)(fPixels + (y * fWidth) + x);
	*dptr = (byte)color;
}


inline byte DGifReader::MapColor( byte oldcolor)
{
#ifdef WIN_MOTIF
	if (fColorswap) return fColorswap[oldcolor];
	else 
#endif
	return oldcolor;
}


void DGifReader::OpenPicture()
{
	if (fPictWriter) {
		fPictWriter->Open( this->Height(), this->Width(), this->NColors(), this->GetColormap());		
		}
	else
		; //Nlm_Message(MSG_FATAL,"GIF Reader: can't write picture");
}



void DGifReader::ReadImage( Boolean ignore)
{
	short		codesize;
	long		xpos = 0, ypos = 0, pass = 0;
  register long col;
	long		row;
	register byte 	* dptr;
	byte 	* dmax;

	codesize= ReadByte();
  if (fEof || codesize < 2 || codesize >= MAX_LZW_BITS) {
		fError= "EOF / read error on image data";
		return;
		}
	InitLZWCode(codesize);

	if (ignore) {
		SkipDataBlocks();
		return;
		}
	
	OpenPicture();

#if 1
	if (fPictWriter && !fInterlace) {
		// we can write each fPixels row to disk as written
		// since ypos increases stepwise
		byte * buf = (byte*)MemNew( fWidth+10);
		for (row = 0; row < fHeight; row++) {
			dptr= (byte*)buf;
			for (col = fWidth; col > 0; col--) {
				*dptr++ = (byte) MapColor( LZWReadByte());
	      }
	    fPictWriter->WriteLine( (char*)buf);
	  	}
		MemFree( buf);
		fPictWriter->Close();
		return;
		}
#endif

			// CHECK FOR LOW MEMORY HERE !!
	ulong width= fWidth;
	ulong height= fHeight;
	ulong pixsize= width * height;
	fPixels = (char*) MemNew(pixsize);
			// ^^^^ THIS CAN BE A BIG MOTHER ! (300 x 300 image == 90K )
	dmax= (byte*) (fPixels + pixsize);
	if (fPixels == NULL) {
		fError= "Cannot allocate space for image data";
		return;
		}

	if (!fInterlace) {
		dptr= (byte*)fPixels;
		for (row = 0; row < height; row++) {
			for (col = width; col > 0; col--) {
				if (dptr > dmax) break;
				else *dptr++ = (byte) MapColor( LZWReadByte());
	      }
	  	}
		}
		
	else {
		xpos= ypos= 0;
		for (row = 0; row < height; row++) {
			for (col = width; col > 0; col--)
				dptr = (byte *) (fPixels + xpos + (ypos * fWidth));
	      *dptr = (byte) MapColor( LZWReadByte());
				++xpos;
				if (xpos == fWidth) {
					xpos = 0;
					if (fInterlace) {
						switch (pass) {
						case 0:
						case 1:
							ypos += 8; break;
						case 2:
							ypos += 4; break;
						case 3:
							ypos += 2; break;
							}
						if (ypos >= fHeight) {
							++pass;
							switch (pass) {
							case 1:
								ypos = 4; break;
							case 2:
								ypos = 2; break;
							case 3:
								ypos = 1; break;
							default:
								goto fini;
							}
						}
					} else 
						++ypos;
				}
				if (ypos >= fHeight)
					break;
	  	}
		}

fini:
	// this is no longer valid??
	if (LZWReadByte()>=0) fError= "too much input data, ignoring extra...";
}



Boolean DGifReader::DoExtension()
{
	static char	buf[256];
	char		*str;

	short label = ReadByte();
	if (fEof) {
		fError= "EOF / read error on extention function code";
		return false;
		}

	switch (label) {
	case 0x01:	 
		str = "Plain Text Extension";
#if 0
		if (GetDataBlock( buf) == 0) ;

		lpos   = LM_to_uint(buf[0], buf[1]);
		tpos   = LM_to_uint(buf[2], buf[3]);
		width  = LM_to_uint(buf[4], buf[5]);
		height = LM_to_uint(buf[6], buf[7]);
		cellw  = buf[8];
		cellh  = buf[9];
		foreground = buf[10];
		background = buf[11];

		while (GetDataBlock( buf) != 0) {
			PPM_ASSIGN(image[ypos][xpos],
					cmap[CM_RED][v],
					cmap[CM_GREEN][v],
					cmap[CM_BLUE][v]);
			++index;
		}

		return false;
#else
		break;
#endif

	case 0xff:		 
		str = "Application Extension";
		break;
		
	case 0xfe:		 
		str = "Comment Extension";
		while (GetDataBlock(  buf) != 0) {
			//if (showComment) fprintf(stderr, "gif comment: %s\n", buf);
			}
		return false;
		
	case 0xf9:		 
		str = "Graphic Control Extension";
		(void) GetDataBlock(  buf);
		fGif89.disposal    = (buf[0] >> 2) & 0x7;
		fGif89.inputFlag   = (buf[0] >> 1) & 0x1;
		fGif89.delayTime   = LM_to_uint(buf[1],buf[2]);
		if ((buf[0] & 0x1) != 0) fGif89.transparent = buf[3];
		SkipDataBlocks();
		return false;
		
	default:
		str = buf;
		//sprintf(buf, "UNKNOWN (0x%02x)", label);
		strcpy(buf,"UNKNOWN Extension");
		break;
	}

	SkipDataBlocks();
	return false;
}



short DGifReader::GetDataBlock( void *buf)
{
	short	count;   
	count= ReadByte();
	if (fEof) {
		fError= "error reading DataBlock size";
		return -1;
		}
	if ( count > 0 && !ReadOK( buf, count)) {
		fError= "error reading DataBlock";
		return -1;
		}
	return count;
}

void DGifReader::SkipDataBlocks()
{
  char buf[256];
  while ( GetDataBlock(buf) > 0) ; /* skip */
}

ulong DGifReader::GetCode()
{
	ulong		ret, count, offs;
  register ulong accum;
	 
	if ( (curbit+code_size) >= lastbit) {
		if (out_of_blocks) {
			fError= "ran out of GIF bits";
			return end_code;
			}
		codebuf[0] = codebuf[lastbyte-2];
		codebuf[1] = codebuf[lastbyte-1];

		if ((count = GetDataBlock( &codebuf[2])) == 0) {
      out_of_blocks = TRUE;
			fError= "ran out of GIF bits";
      return end_code;		/* fake something useful */
			}
		curbit = (curbit - lastbit) + 16;
		lastbyte = 2 + count;
		lastbit = lastbyte * 8;
		}

			/* Form up next 24 bits in accum */
	offs = curbit >> 3;		/* byte containing cur_bit */
  accum = codebuf[offs+2];
  accum <<= 8;
  accum |= codebuf[offs+1];
  accum <<= 8;
  accum |= codebuf[offs];
  		/* Right-align cur_bit in accum, then mask off desired number of bits */
  accum >>= (curbit & 7);
  ret = ((ulong) accum) & ((1 << code_size) - 1);

	curbit += code_size;
	return ret;
}


void DGifReader::ReInitLZW()
/* (Re)initialize LZW state; shared code for startup and Clear processing */
{
  code_size = input_code_size+1;
  limit_code = clear_code << 1;	/* 2^code_size */
  max_code = clear_code + 2;	/* first unused code value */
  lzwsp = lzwstack;		/* init stack to empty */
}


void DGifReader::InitLZWCode( short inCodeSize)
/* Initialize for a series of LZWReadByte (and hence GetCode) calls */
{
  /* GetCode initialization */
  lastbyte = 2;		/* make safe to "recopy last two bytes" */
  lastbit = 0;			/* nothing in the buffer */
  curbit = 0;			/* force buffer load on first call */
  out_of_blocks = FALSE;

	input_code_size= inCodeSize;
  /* LZWReadByte initialization */
  clear_code = 1 << input_code_size; /* compute special code values */
  end_code = clear_code + 1;	/* note that these do not change */
  lzwfresh = TRUE;
  ReInitLZW();
}


long DGifReader::LZWReadByte()
{
	register long  code;
	long incode;   

	if (lzwfresh) {
		lzwfresh = false;
    code = clear_code;		/* enables sharing code with Clear case */
		}
	else {
    	/* If any codes are stacked from a previously read symbol, return them */
    if (lzwsp > lzwstack)
      return (short) *(--lzwsp);

    	/* Time to read a new symbol */
    code = GetCode();
		}
		
  if (code == clear_code) {
    /* Reinit static state, swallow any extra Clear codes, and */
    /* return next code, which is expected to be a raw byte. */
    ReInitLZW();
    do {
      code = GetCode();
    } while (code == clear_code);
    if (code > clear_code) {	/* make sure it is a raw byte */
      fError= "Corrupt data in GIF file";
      code = 0;			/* use something valid */
    	}
    firstcode = oldcode = code;	/* make firstcode, oldcode valid! */
    return code;
  	}

  if (code == end_code) {
    /* Skip the rest of the image, unless GetCode already read terminator */
    if (! out_of_blocks) {
      SkipDataBlocks();
      out_of_blocks = TRUE;
  	  }
    /* Complain that there's not enough data */
    fError= "Premature end of GIF image";
    /* Pad data with 0's */
    return 0;			/* fake something usable */
  	}

  /* Got normal raw byte or LZW symbol */
  incode = code;		/* save for a moment */
  
  if (code >= max_code) {	/* special case for not-yet-defined symbol */
    /* code == max_code is OK; anything bigger is bad data */
    if (code > max_code) {
      fError= "Corrupt data in GIF file";
      incode = 0;		/* prevent creation of loops in symbol table */
    	}
    *lzwsp++ = (byte) firstcode;	/* it will be defined as oldcode/firstcode */
    code = oldcode;
 	 	}

  /* If it's a symbol, expand it into the stack */
  while (code >= clear_code) {
    *lzwsp++ = lzwtail[code];	 /* tail of symbol: a simple byte value */
    code = lzwhead[code];	/* head of symbol: another LZW symbol */
  	}
  /* At this point code just represents a raw byte */
  firstcode = code;		/* save for possible future use */

  /* If there's room in table, */
  if ((code = max_code) < LZW_TABLE_SIZE) {
    /* Define a new symbol = prev sym + head of this sym's expansion */
    lzwhead[code] = oldcode;
    lzwtail[code] = (byte) firstcode;
    max_code++;
    /* Is it time to increase code_size? */
    if ((max_code >= limit_code) && (code_size < MAX_LZW_BITS)) {
      code_size++;
      limit_code <<= 1;		/* keep equal to 2^code_size */
			}
		}
  
  oldcode = incode;		/* save last input symbol for future use */
  return firstcode;		/* return first byte of symbol's expansion */
}











// class DPictWriter

#define PICFILE 1

DPictWriter::DPictWriter( short rows, short cols, short ncolors, KolorTable colortable,
							char* pixels, Boolean wantPictHeader, Boolean ownpict)
{
	fHeight= rows;
	fWidth= cols;
	fNColors= ncolors;
	fColortable= colortable;
	fColormap= colortable.ctTable;
	
	fWantHeader= wantPictHeader;
	fOwnPict= ownpict;
	fPicFile= new DTempFile();
	fPicFile->Open("wb");
	fBytesPut= fPictSize= fPicSizeAt= fPictMax= 0;
	fPict= NULL;
	fPacked = (char*) MemNew( (unsigned)(fWidth + fWidth/MAX_COUNT + 1));
	
	PutPictTop();
	for (short arow = 0; arow < fHeight; arow++) 
		WriteLine( pixels + (arow * fWidth));
		//fBytesPut += putRow( pixels + (arow * fWidth));
					// ^^^^^ for bitmaps, this has to be changed to handle 1 bit pixels
					// but i don't know what the algorithm for RLE of that is !
	PutPictEnd();
}
	
DPictWriter::DPictWriter(  Boolean wantPictHeader, Boolean ownpict)
{
	fWidth= 0;
	fNColors= 0;
	fColormap= NULL;	
	fWantHeader= wantPictHeader;
	fOwnPict= ownpict;
	fBytesPut= fPictSize= fPicSizeAt= fPictMax= 0;
	fPict= NULL;
	fPicFile= NULL;
	fPacked = NULL;  
}

DPictWriter::~DPictWriter()
{
	if (fOwnPict) MemFree( fPict);
	if (fPicFile) {
		fPicFile->Delete();
		delete fPicFile;
		}
	MemFree( fPacked);
}


void  DPictWriter::Open( short rows, short cols, short ncolors, KolorTable colortable) 
{
	fHeight= rows;
	fWidth= cols;
	fNColors= ncolors;
	fColortable= colortable;
	fColormap= colortable.ctTable;
	fPacked = (char*) MemNew( (unsigned)(fWidth + fWidth/MAX_COUNT + 1));
	fPicFile= new DTempFile();
	fPicFile->Open("wb");
	PutPictTop();
}

	
	
byte* DPictWriter::Pict()
{
#if PICFILE
	if (!fPict && fPicFile) {
		fPicFile->Close();
		fPicFile->Open("rb");
		fPict= (byte*) MemNew( fPictSize+1);
		fPicFile->ReadData( fPict, fPictSize);
		}
#endif
	return fPict; 
} 
	

void DPictWriter::putChar( byte c)
{
#if PICFILE
	fputc( c, fPicFile->fFile);
	fPictSize++;
#else
	if (fPictSize >= fPictMax) {
		fPictMax = fPictSize + 1025;
		if (!fPict) fPict= (byte*) Nlm_MemNew(fPictMax);
		else fPict= (byte*) Nlm_MemMore( fPict, fPictMax);
		}
	if (fPict) fPict[fPictSize++]=  c;
#endif
}

void DPictWriter::putFill( char c, ulong size)
{
	for ( ; size>0; size--) putChar( c);
}

void DPictWriter::putShort( short i)
{
	putChar( (i >> 8) & 0xff);
	putChar( i & 0xff);
}

void DPictWriter::putLong( long i)
{
	putChar( (i >> 24) & 0xff);
	putChar( (i >> 16) & 0xff);
	putChar( (i >> 8) & 0xff);
	putChar( i & 0xff);
}

void DPictWriter::putFixed( short in, short frac)
{
	putShort( in);
	putShort( frac);
}

void DPictWriter::putRect( short x1, short x2, short y1, short y2)
{
	putShort( x1);
	putShort( x2);
	putShort( y1);
	putShort( y2);
}


void DPictWriter::PutPictEnd()
{
	if (fBytesPut & 1) putChar(0); //if we wrote an odd number of pixdata bytes, pad 
	putShort( PICT_EndOfPicture);
#if PICFILE
	fPicFile->Seek(fPicSizeAt);
	putShort( (short)(fPictSize & 0xffff));
#else
	ulong saveSize= fPictSize;
	fPictSize= fPicSizeAt;
	putShort( (short)(saveSize & 0xffff));
	fPictSize= saveSize;
#endif
}

void DPictWriter::PutPictTop()
{
	fRowbytes= fWidth;
	//if (fRowbytes & 1)  fRowbytes ++;	// ?????
	if (fWantHeader) putFill( 0, HEADER_SIZE);

			// write picSize and picFrame 
#if PICFILE
	fPicSizeAt= fPicFile->Tell();
#else
	fPicSizeAt= fPictSize;
#endif
	putShort( 0);   // write pictSize again at end when size is known
	putRect(0, 0, fHeight, fWidth);

			// write version op and version 
	putShort( PICT_picVersion);
	putShort( 0x02FF);
	putShort( PICT_headerOp);
	putLong( -1L);
	putFixed( 0, 0);
	putFixed( 0, 0);
	putFixed( fWidth, 0);
	putFixed( fHeight, 0);
	putFill( 0, 4);

			// seems to be needed by many PICT2 programs 
	putShort( PICT_clipRgn);
	putShort( 10);
	putRect( 0, 0, fHeight, fWidth);

	if (false) {  // (fNColors<3)
			 //  save lots of space doing bitmap not pixmap !
		putShort( PICT_PackBitsRect);
		putShort( fRowbytes );  // rowbytes !? make even if odd ??
				// bitmap params
		putRect( 0, 0, fHeight, fWidth);	// bounds
		putRect( 0, 0, fHeight, fWidth);	/* srcRect */
		putRect( 0, 0, fHeight, fWidth);	/* dstRect */
		putShort( 0);			/* mode */
		}
	else {
		putShort( PICT_PackBitsRect);
		putShort( fRowbytes | 0x8000);   // 8 is flag to do pixmap !
				// pixmap params
		putRect( 0, 0, fHeight, fWidth); // bounds
		putShort( 0);	/* pmVersion */
		putShort( 0);	/* packType */
		putLong( 0L);	/* packSize */
		putFixed( 72, 0);	/* hRes */
		putFixed( 72, 0);	/* vRes */
		putShort( 0);	/* pixelType */
		putShort( 8);	// pixelSize  ???????
		putShort( 1);	/* cmpCount */
		putShort( 8);	// cmpSize  -- must == pixelSize
		putLong( 0L);	/* planeBytes */
		putLong( 0L);	/* pmTable */
		putLong( 0L);	/* pmReserved */
	
			 // Write out the colortable. 
		putLong ( fColortable.ctSeed);	 
		putShort( fColortable.ctFlags);	 
		putShort( fColortable.ctSize);	 
		for (unsigned short i = 0; i <= fColortable.ctSize; i++) {
			putShort( fColormap[i].value);
			putShort( fColormap[i].rgb.red);
			putShort( fColormap[i].rgb.green);
			putShort( fColormap[i].rgb.blue);
			}
	
		putRect( 0, 0, fHeight, fWidth);	/* srcRect */
		putRect( 0, 0, fHeight, fWidth);	/* dstRect */
		putShort( 0);			/* mode */
		}
}





#define	RUNLENGTH
#ifdef	RUNLENGTH

#define		runtochar(c)	(257-(c))
#define		counttochar(c)	((c)-1)

long DPictWriter::putRow( char* pixelrow)
{
	register short i;
	register pixel *pP;
	pixel lastp;
	register char *p;
	long packcols, count, run, rep, oc;
	short cols= fRowbytes-1;
	pixel* rowpixels= (pixel*) pixelrow;  //(pixel*) fPixels + (row * fWidth); // * sizeof(pixel);

	run = count = 0;
	for (cols--, i = cols, pP = rowpixels + cols, p = fPacked, lastp = *pP;
			i >= 0; 
			i--, lastp = *pP, pP--) {
		if ( lastp == *pP )
			run++;
		else if (run < RUN_THRESH) {
			while (run > 0) {
				*p++ = lastp; //ppm_lookupcolor(cht, &lastp);
				run--;
				count++;
				if (count == MAX_COUNT) {
					*p++ = counttochar(MAX_COUNT);
					count -= MAX_COUNT;
					}
				}
			run = 1;
			}
		else {
			if (count > 0) *p++ = counttochar(count);
			count = 0;
			while (run > 0) {
				rep = run > MAX_RUN ? MAX_RUN : run;
				*p++ = lastp; //ppm_lookupcolor(cht, &lastp);
				*p++ = runtochar(rep);
				run -= rep;
				}
			run = 1;
			}
		}
		
	if (run < RUN_THRESH) {
		while (run > 0) {
			*p++ = lastp; //ppm_lookupcolor(cht, &lastp);
			run--;
			count++;
			if (count == MAX_COUNT) {
				*p++ = counttochar(MAX_COUNT);
				count -= MAX_COUNT;
				}
			}
		}
	else {
		if (count > 0) *p++ = counttochar(count);
		count = 0;
		while (run > 0) {
			rep = run > MAX_RUN ? MAX_RUN : run;
			*p++ = lastp; //ppm_lookupcolor(cht, &lastp);
			*p++ = runtochar(rep);
			run -= rep;
			}
		run = 1;
		}
	if (count > 0) 	*p++ = counttochar(count);

	packcols = p -  fPacked;		/* how many did we write? */
	if (cols > 250) {
		putShort( packcols);
		oc = packcols + 2;
		}
	else {
		putChar(packcols);
		oc = packcols + 1;
		}

	/* now write out the packed row */
	while (p != fPacked) {
		--p;
		putChar(*p);
		}

	return (oc);
}


#else	/* RUNLENGTH */

/* real dumb putRow with no compression */
long DPictWriter::putRow( char* pixelrow)
{
	register short i, j, bc, oc;
	register pixel *pP;
 	short cols= fRowbytes;
	pixel* rowpixels= (pixel*) pixelrow;  //(pixel*) fPixels + (row * fWidth); // * sizeof(pixel);

	bc = cols + (cols + MAX_COUNT - 1) / MAX_COUNT;
	if (bc > 250) {
		putShort( bc);
		oc = bc + 2;
		}
	else {
		putChar(bc);
		oc = bc + 1;
		}
	for (i = 0, pP = rowpixels; i < cols; ) {
		if (cols - i > MAX_COUNT) {
			putChar(MAX_COUNT - 1);
			for (j = 0; j < MAX_COUNT; j++) {
				putChar( *pP); //ppm_lookupcolor(cht, pP)
				pP++;
				}
			i += MAX_COUNT;
			}
		else {
			putChar(cols - i - 1);
			for (j = 0; j < cols - i; j++) {
				putChar( *pP);  //ppm_lookupcolor(cht, pP)
				pP++;
				}
			i = cols;
			}
		}
	return (oc);
}

#endif	/* RUNLENGTH */
