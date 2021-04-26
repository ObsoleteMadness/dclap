// DDrawPICT.cpp
//  translate Mac QuickDraw PICT data format to NCBI Draw methods 


// debugging -- file list
#undef PLIST

#include <ncbi.h>
#include <ncbidraw.h>
#include <ncbiwin.h>
#include <ncbiport.h>
#include <vibtypes.h>
#include <vibincld.h>

// avoid mess with document.h
#define _DOCUMENT_

#include "Dvibrant.h"
#include "DDrawPICT.h"

#ifdef WIN_MAC
#include <Palettes.h>
#endif

#define byte 			unsigned char
#define word 			unsigned short
#define longword 	unsigned long


#ifdef PLIST
FILE* gPlist= NULL;
Boolean gDoPlist = true;
#endif



enum PICTops {
	oNOP =	0,
	oClip,
	oBkPat,
	oTxFont,
	oTxFace,
	oTxMode,
	oSpExtra,
	oPnSize,
	oPnMode,
	oPnPat,
	oFillPat,
	oOvSize,
	oOrigin,
	oTxSize,
	oFgColor,
	oBkColor,
	oTxRatio,
	oVersion,
	oBkPixPat,
	oPnPixPat,
	oFillPixPat,
	oPnLocHFrac,
	oChExtra,
	r17,r18,r19,
	oRGBFgCol	= 0x1a,
	oRGBBkCol,
	oHiliteMode,
	oHiliteColor,
	oDefHilite,
	oOpColor,
	oLine,
	oLineFrom,
	oShortLine,
	oShortLineFrom,
	r24,r25,r26,r27,
	oLongText	= 0x28,
	oDHText,
	oDVText,
	oDHDVText,
	oFontName, /* was res2c */
	r2d,
	or2e_mov,  /* was res2e -- just a GUESS !!! */
	r2f,
	oframeRect	= 0x30,
	opaintRect,
	oeraseRect,
	oinvertRect,
	ofillRect,
	r35,r36,r37,
	oframeSameRect = 0x38,
	opaintSameRect,
	oeraseSameRect,
	oinvertSameRect,
	ofillSameRect,
	r3d,r3e,r3f,
	oframeRRect	 = 0x40,
	opaintRRect,
	oeraseRRect,
	oinvertRRect,
	ofillRRect,
	r45,r46,r47,
	oframeSameRRect	= 0x48,
	opaintSameRRect,
	oeraseSameRRect,
	oinvertSameRRect,
	ofillSameRRect,
	r4d,r4e,r4f,
	oframeOval = 0x50,
	opaintOval,
	oeraseOval,
	oinvertOval,
	ofillOval,
	r55,r56,r57,
	oframeSameOval = 0x58,
	opaintSameOval,
	oeraseSameOval,
	oinvertSameOval,
	ofillSameOval,
	r5d,r5e,r5f,
	oframeArc = 0x60,
	opaintArc,
	oeraseArc,
	oinvertArc,
	ofillArc,
	r65,r66,r67,
	oframeSameArc = 0x68,
	opaintSameArc,
	oeraseSameArc,
	oinvertSameArc,
	ofillSameArc,
	r6d,r6e,r6f,
	oframePoly = 0x70,
	opaintPoly ,
	oerasePoly,
	oinvertPoly,
	ofillPoly,
	r75,r76,r77,
	oframeSamePoly = 0x78,
	opaintSamePoly,
	oeraseSamePoly,
	oinvertSamePoly,
	ofillSamePoly,
	r7d,r7e,r7f,
	oframeRgn = 0x80,
	opaintRgn,
	oeraseRgn,
	oinvertRgn,
	ofillRgn,
	r85,r86,r87,
	oframeSameRgn = 0x88,
	opaintSameRgn,
	oeraseSameRgn,
	oinvertSameRgn,
	ofillSameRgn,
	r8d,r8e,r8f,
	oBitsRect = 0x90,
	oBitsRgn,
	r92,r93,r94,r95,r96,r97,
	oPackBitsRect = 0x98,
	oPackBitsRgn,
	oOpcode9A,
	r9b,r9c,r9d,r9e,r9f,
	oShortComment = 0xa0,
	oLongComment,
	/* a2..af,b0..fe unused */
	oopEndPic	= 0x00ff,
	/* 0100..ffff unused */
	oHeaderOp	= 0x0c00
	};
	

#ifdef PLIST
static char* gOpnames[] = {
 "oNOP",
 "oClip",
 "oBkPat",
 "oTxFont",
 "oTxFace",
 "oTxMode",
 "oSpExtra",
 "oPnSize",
 "oPnMode",
 "oPnPat",
 "oFillPat",
 "oOvSize",
 "oOrigin",
 "oTxSize",
 "oFgColor",
 "oBkColor",
 "oTxRatio",
 "oVersion",
 "oBkPixPat",
 "oPnPixPat",
 "oFillPixPat",
 "oPnLocHFrac",
 "oChExtra",
 "r17","r18","r19",
 "oRGBFgCol",
 "oRGBBkCol",
 "oHiliteMode",
 "oHiliteColor",
 "oDefHilite",
 "oOpColor",
 "oLine",
 "oLineFrom",
 "oShortLine",
 "oShortLineFrom",
	"r24","r25","r26","r27",
 "oLongText",
 "oDHText",
 "oDVText",
 "oDHDVText",
 "oFontName",  
	"r2d",
 "or2e_mov", 
	"r2f",
 "oframeRect",
 "opaintRect",
 "oeraseRect",
 "oinvertRect",
 "ofillRect",
	"r35","r36","r37",
 "oframeSameRect",
 "opaintSameRect",
 "oeraseSameRect",
 "oinvertSameRect",
 "ofillSameRect",
	"r3d","r3e","r3f",
 "oframeRRect",
 "opaintRRect",
 "oeraseRRect",
 "oinvertRRect",
 "ofillRRect",
	"r45","r46","r47",
 "oframeSameRRect",
 "opaintSameRRect",
 "oeraseSameRRect",
 "oinvertSameRRect",
 "ofillSameRRect",
	"r4d","r4e","r4f",
 "oframeOval",
 "opaintOval",
 "oeraseOval",
 "oinvertOval",
 "ofillOval",
	"r55","r56","r57",
 "oframeSameOval",
 "opaintSameOval",
 "oeraseSameOval",
 "oinvertSameOval",
 "ofillSameOval",
	"r5d","r5e","r5f",
 "oframeArc",
 "opaintArc",
 "oeraseArc",
 "oinvertArc",
 "ofillArc",
	"r65","r66","r67",
 "oframeSameArc",
 "opaintSameArc",
 "oeraseSameArc",
 "oinvertSameArc",
 "ofillSameArc",
	"r6d","r6e","r6f",
 "oframePoly",
 "opaintPoly",
 "oerasePoly",
 "oinvertPoly",
 "ofillPoly",
	"r75","r76","r77",
 "oframeSamePoly",
 "opaintSamePoly",
 "oeraseSamePoly",
 "oinvertSamePoly",
 "ofillSamePoly",
	"r7d","r7e","r7f",
 "oframeRgn",
 "opaintRgn",
 "oeraseRgn",
 "oinvertRgn",
 "ofillRgn",
	"r85","r86","r87",
 "oframeSameRgn",
 "opaintSameRgn",
 "oeraseSameRgn",
 "oinvertSameRgn",
 "ofillSameRgn",
	"r8d","r8e","r8f",
 "oBitsRect",
 "oBitsRgn",
	"r92","r93","r94","r95","r96","r97",
 "oPackBitsRect",
 "oPackBitsRgn",
 "oOpcode9A",
	"r9b","r9c","r9d","r9e","r9f",
 "oShortComment",
 "oLongComment",
  0 };
 
//	/* a2..af",b0..fe unused */
// "oopEndPic	= 0x00ff",
//	/* 0100..ffff unused */
// "oHeaderOp	= 0x0c00
#endif
	
	
typedef struct valName {
	short	value;
	char* name;
} ValName;

static ValName QuickDrawFont[] = {
	{ 0,	"helvetica" }, 	/* Chicago */
	{ 1,	"helvetica" },  /* Geneva */
	{ 2,	"times" },			/* newYork */
	{ 3,	"helvetica" },	/* geneva */
	{ 4,	"courier" },		/* monaco */
	{ 5,	"times" },			/* venice */
	{ 6,	"times" },			/* london */
	{ 7,	"times" },			/* athens */
	{ 8,	"times" },			/* sanFran */
	{ 9,	"times" },			/* toronto */
	{ 11,	"times" },			/* cairo */
	{ 12,	"times" },			/* losAngeles */
	{ 13,	"times" },			/* Zapf Dingbats */
	{ 14,	"bookman" },		/* bookman */
	{ 16,	"palatino" },		/* palatino */
	{ 18,	"times" },			/* Zapf Chancery */
	{ 19,	"times" },			/* Sourvenir */
	{ 20,	"times" },			/* times */
	{ 21,	"helvetica" },
	{ 22,	"courier" },
	{ 23,	"symbol" },
	{ 24,	"times" },		 /* taliesin */
	{ 26,	"times" },		 /* Lubalin Graph */
	{ 33,	"avant garde" },		 /* Avant Garde */
	{ 34,	"New Century Schlbk" },		 /* New Century Schlbk */
	{ -1,	0 }
};

#if 0
Font[0] = 'Chicago'
Font[1] = 'Geneva'
Font[2] = 'New York'
Font[3] = 'Geneva'
Font[4] = 'Monaco'
Font[13] = 'Zapf Dingbats'
Font[14] = 'Bookman'
Font[16] = 'Palatino'
Font[18] = 'Zapf Chancery'
Font[19] = 'Souvenir'
Font[20] = 'Times'
Font[21] = 'Helvetica'
Font[22] = 'Courier'
Font[23] = 'Symbol'
Font[26] = 'Lubalin Graph'
Font[33] = 'Avant Garde'
Font[34] = 'New Century Schlbk'
#endif
	

Nlm_Boolean gPixColorsDone = false; // a hack !

#ifdef WIN_MOTIF
static short gAllocatedColors[256];
static short gNAllocatedColors = 0;
#endif


DDrawPict::DDrawPict() :
	fVerbose(true), fPenshown(true), fUnclipped(true),
	fPict(NULL), fAlign(0),
	fFont(NULL), fFontname(NULL), fFontfamily(NULL),
	fFontsize(12), fFontstyle(0), fLastFontnum(-1),
	fNeedInstallFont(false),
	fPictVersion(0), fPicsize(0)
{
	Nlm_LoadRect( &fPicRect, 0, 0, 0, 0);
	Nlm_LoadRect( &fPicFrame, 0, 0, 0, 0);
	Nlm_LoadRect( &fTheRect, 0, 0, 0, 0);
	fOvSize.x= 0; 
	fOvSize.y= 0;
	fFontname= "Times";
	fFontfamily= "Roman";
	MakeXByteFlip();
  gPixColorsDone= false;
}

DDrawPict::~DDrawPict()
{

}

//static
void DDrawPict::Cleanup()
{
#ifdef WIN_MOTIF
	if (gNAllocatedColors) {
		short 	i, j;
  	Display * display 	= Nlm_currentXDisplay;
  	short     screenNum	= DefaultScreen( display);
   	Visual  * visual		= DefaultVisual( display, screenNum);
   	Colormap 	colormap 	= DefaultColormap( display, screenNum);  
   	  
		for (i=0; i<256; i++) {
			short nalloc= gAllocatedColors[i];
			unsigned long pixel= i;
			for ( ; nalloc>0; nalloc--) 
				XFreeColors( display, colormap, &pixel, 1, 0L);
			gAllocatedColors[i]= 0;
			}
		gNAllocatedColors= 0;
		}
#endif	
}



byte	DDrawPict::getByte()
{
	byte b = *fPict++;
	fAlign++;
#ifdef PLIST
	if (gDoPlist) fprintf( gPlist, " %2X", b); 
#endif
	return b;
}

word DDrawPict::getWord()
{
	word b = *fPict++;
	fAlign += 2;
	b=  (b << 8) | *fPict++;
#ifdef PLIST
	if (gDoPlist) fprintf( gPlist, " %4X", b); 
#endif
	return b;
}

longword DDrawPict::getLong()
{
	longword i = getWord();
	i=  (i << 16) | getWord();
	return i;
}


void DDrawPict::skipbytes( word count)
{
#ifdef PLIST
	if (gDoPlist) {
	fprintf( gPlist, " (skip %d)", count); 
	for (short i=0; i< MIN(count,10); i++)
		fprintf(gPlist, " %2X", fPict[i]);
	}
#endif
	fAlign += count;
	fPict += count;
}

void DDrawPict::getbytes(char* buf, long bufsize)
{
#ifdef PLIST
	if (gDoPlist) {
	fprintf( gPlist, " (get %d)", bufsize); 
	for (short i=0; i< MIN(bufsize,10); i++)
		fprintf(gPlist, " %2X", fPict[i]);
	}
#endif
	fAlign += bufsize;
	MemCpy(buf, fPict, bufsize);
	fPict += bufsize;
}

word DDrawPict::getOpcode()
{
	word op;
	
#ifdef PLIST
	if (gDoPlist) fprintf( gPlist, "\n"); 
#endif
	if (fPictVersion == 1)
		op= getByte();
	else {
		if (fAlign & 1) {
#ifdef PLIST
			Boolean savep= gDoPlist; gDoPlist= false;
#endif
			(void) getByte();
#ifdef PLIST
			gDoPlist= savep;
#endif
			}
		op= getWord();
		}
		
#ifdef PLIST
	if (!gDoPlist) ;
	else if (op < 0xa2) fprintf( gPlist, "%#7x %-16s: ", fAlign, gOpnames[op]);
	else fprintf( gPlist, "%#7x %#-16x: ",fAlign, op);
#endif
	return op;
}





void DDrawPict::getRGBKolor( RGBkolor* acolor)
{
	acolor->red = getWord();
	acolor->green= getWord();
	acolor->blue= getWord();
}

void DDrawPict::getPointAsis( Nlm_PointPtr apt)
{
	apt->y	= getWord();
	apt->x	= getWord();
}

void DDrawPict::getPointLocal( Nlm_PointPtr apt)
{
	long y	= ((signed short) getWord()) - fPicFrame.top  + fPicRect.top;
	long x	= ((signed short) getWord()) - fPicFrame.left + fPicRect.left;
	apt->y= MAX(-32565, MIN(32565, y)); //y;
	apt->x= MAX(-32565, MIN(32565, x)); //x;
}

void DDrawPict::getRectLocal( Nlm_RectPtr arect)
{
	long top		= ((signed short) getWord()); top  += fPicRect.top - fPicFrame.top;
	long left		= ((signed short) getWord()); left += fPicRect.left - fPicFrame.left;
	long bottom	= ((signed short) getWord()); bottom += fPicRect.top - fPicFrame.top;
	long right	= ((signed short) getWord()); right += fPicRect.left - fPicFrame.left;
	arect->top 		= MAX(-32565, MIN(32565, top));
	arect->left		= MAX(-32565, MIN(32565, left)); //(signed short)left;
	arect->bottom	= MAX(-32565, MIN(32565, bottom)); //(signed short)bottom;
	arect->right	= MAX(-32565, MIN(32565, right)); //(signed short)right;
}

void DDrawPict::getRectAsis( Nlm_RectPtr arect)
{
	arect->top		= getWord();
	arect->left		= getWord();
	arect->bottom	= getWord();
	arect->right	= getWord();
}


void DDrawPict::skipPolyOrRegion(void)
{
	skipbytes( getWord() - 2);
}


#ifdef WIN_MOTIF
#define COPY_MODE   1
#define MERGE_MODE  2
#define INVERT_MODE 3
#define ERASE_MODE  4

static byte    XByteFlip [256];

void DDrawPict::MakeXByteFlip()
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
}

#else

void DDrawPict::MakeXByteFlip()
{
}
#endif


byte* DDrawPict::expandbuf(byte* buf, short* len, short bitsPerPixel)
{
	static byte pixbuf[256 * 8];
	register byte* src;
	register byte* dst;
	register int i;

	src = buf;
	dst = pixbuf;

	switch (bitsPerPixel) {
	case 8:
	case 16:
	case 32:
		return buf;
	case 4:
		for (i = 0; i < *len; i++) {
			*dst++ = (*src >> 4) & 15;
			*dst++ = *src++ & 15;
		}
		*len *= 2;
		break;
	case 2:
		for (i = 0; i < *len; i++) {
			*dst++ = (*src >> 6) & 3;
			*dst++ = (*src >> 4) & 3;
			*dst++ = (*src >> 2) & 3;
			*dst++ = *src++ & 3;
		}
		*len *= 4;
		break;
	case 1:
		for (i = 0; i < *len; i++) {
			*dst++ = (*src >> 7) & 1;
			*dst++ = (*src >> 6) & 1;
			*dst++ = (*src >> 5) & 1;
			*dst++ = (*src >> 4) & 1;
			*dst++ = (*src >> 3) & 1;
			*dst++ = (*src >> 2) & 1;
			*dst++ = (*src >> 1) & 1;
			*dst++ = *src++ & 1;
		}
		*len *= 8;
		break;
	default:
		if (fVerbose) { Nlm_Message(MSG_OK,"drawpict:bad bits per pixel in expandbuf"); fVerbose= false; }
	}
	return pixbuf;
}


#define DONTEXPAND 1


void DDrawPict::unpackSmallLine( long irow, unsigned char* linebuf, unsigned char* bits, unsigned char* bitsend, 
																	unsigned short rowBytes, short pixelSize)
{
	short buflen = rowBytes;
	byte 	abyte, *pbits, *bytepixels;
	Nlm_Boolean isBitmap = pixelSize < 2;
	long	j;
	
	getbytes((char*) linebuf, buflen);
#if DONTEXPAND
#ifdef WIN_MSWIN
	if (!isBitmap) pbits= bitsend - irow*rowBytes;
	else
#endif
	pbits = bits + irow*rowBytes; 
	bytepixels= linebuf;
#else
	pbits = bits + irow * pixwidth;
	bytepixels = expandbuf( linebuf, &buflen, pixelSize);
#endif

	for (j = 0; j < buflen; j++) {
		abyte= *bytepixels++;
#ifdef WIN_MSWIN
		if (isBitmap) abyte= (byte) ~abyte;
#endif
#ifdef WIN_MOTIF
		if (isBitmap) abyte= XByteFlip[abyte];
#endif
		*pbits++ = abyte;
		}
}


void DDrawPict::unpackBigLine( long irow, unsigned char* linebuf, unsigned char* bits, unsigned char* bitsend, 
																unsigned short rowBytes, short pixelSize, short pkpixsize)
{
	enum { kFudgebuf = 300 };
	word   buflen = rowBytes;
	byte 	 abyte, *pbits, *bytepixels;
	Nlm_Boolean isBitmap = pixelSize < 2;
	long	 j, k, l;
	short  linelen, len;
	
#ifdef WIN_MSWIN
	if (!isBitmap) pbits= bitsend - irow*rowBytes;
	else
#endif
	pbits = bits + irow * rowBytes; // pixwidth; 

	if (rowBytes > 250 || pixelSize > 8)
		linelen = getWord();
	else
		linelen = getByte();

	if (linelen >= kFudgebuf+rowBytes)   
		Nlm_Message(MSG_FATAL,"DDrawPict::linelen > rowbytes");

	getbytes((char*) linebuf, linelen);
	
	for (j = 0; j < linelen; ) {
		if (linebuf[j] & 0x80) {
			len = ((linebuf[j] ^ 255) & 255) + 2;
			buflen = pkpixsize;
#if DONTEXPAND
			bytepixels= linebuf + j+1;
#else
			bytepixels = expandbuf(linebuf + j+1, &buflen, pixelSize);
#endif

			for (k = 0; k < len; k++) {
				for (l = 0; l < buflen; l++) {
					abyte= *bytepixels++;
#ifdef WIN_MSWIN
					if (isBitmap) abyte= (byte) ~abyte;
#endif
#ifdef WIN_MOTIF
					if (isBitmap) abyte= XByteFlip[abyte];
#endif
					*pbits++ = abyte;
					}
				bytepixels -= buflen;
				}
			j += 1 + pkpixsize;
			}
		else {
			len = (linebuf[j] & 255) + 1;
			buflen = len * pkpixsize;
#if DONTEXPAND
			bytepixels= linebuf + j+1;
#else
			bytepixels = expandbuf(linebuf + j+1, &buflen, pixelSize);
#endif
			for (k = 0; k < buflen; k++) {
					abyte= *bytepixels++;
#ifdef WIN_MSWIN
					if (isBitmap) abyte= (byte) ~abyte;
#endif
#ifdef WIN_MOTIF
					if (isBitmap) abyte= XByteFlip[abyte];
#endif
					*pbits++ = abyte;
					}
			j += len * pkpixsize + 1;
			}
		}
}



byte* DDrawPict::unpackbits( Nlm_RectPtr bounds, word rowBytes, short pixelSize, 
														ulong& unpackSize)
{
	enum { kFudgebuf = 300 };
	byte *linebuf;
	byte abyte, *bits, *pbits, *bitsend;
	register long irow,j,k,l;
	word  pixwidth;
	short linelen, len;
	byte* bytepixels;
	short buflen, pkpixsize, rowsize;
	Nlm_Boolean isBitmap = (pixelSize < 2);
	
  unpackSize= 0;
	if (pixelSize <= 8) rowBytes &= 0x7fff;
	pixwidth = bounds->right - bounds->left;

	pkpixsize = 1;
	if (pixelSize == 16) {
		pkpixsize = 2;
		pixwidth *= 2;
		}
	else if (pixelSize == 32)
		pixwidth *= 3;
	
	if (rowBytes == 0) rowBytes = pixwidth;

#if DONTEXPAND
	/* dgg -- don't need 1byte/pixel, not expanding... */
	rowsize = rowBytes;
#else
	rowsize = pixwidth;
	if (rowBytes < 8) rowsize = 8 * rowBytes;	/* worst case expansion factor */
#endif

	unpackSize= rowsize * (bounds->bottom - bounds->top);
	bits = (byte*)Nlm_MemNew( (unpackSize + 8) * sizeof(byte) );
	if (!bits) Nlm_Message(MSG_FATAL, "drawpict:no mem for packbits rectangle");
		
	linebuf = (byte*)Nlm_MemNew(rowBytes + kFudgebuf);
	if (!linebuf) Nlm_Message(MSG_FATAL,"drawpict:can't allocate memory for line buffer");

	bitsend= bits + rowBytes * (bounds->bottom - bounds->top - 1);
	if (rowBytes < 8) {
		for (irow = 0; irow < bounds->bottom - bounds->top; irow++)
			//unpackSmallLine(irow, linebuf, bits, bitsend, rowBytes, pixelSize);
///***********
		{
			buflen = rowBytes;
			getbytes((char*) linebuf, buflen);
#if DONTEXPAND
#ifdef WIN_MSWIN
			if (!isBitmap) pbits= bitsend - irow*rowBytes;
			else
#endif
			pbits = bits + irow * rowBytes; // pixwidth; 
			bytepixels= linebuf;
#else
			pbits = bits + irow * pixwidth;
			bytepixels = expandbuf(linebuf, &buflen, pixelSize);
#endif

			for (j = 0; j < buflen; j++) {
				abyte= *bytepixels++;
#ifdef WIN_MSWIN
				if (isBitmap) abyte= (byte) ~abyte;
#endif
#ifdef WIN_MOTIF
				if (isBitmap) abyte= XByteFlip[abyte];
#endif
 				*pbits++ = abyte;
				}
			}
//****************/

		}
		
	else {
		for (irow = 0; irow < bounds->bottom - bounds->top; irow++) 
			//unpackBigLine(irow, linebuf, bits, bitsend, rowBytes, pixelSize, pkpixsize);

		{
#if 1
#ifdef WIN_MSWIN
			if (!isBitmap) pbits= bitsend - irow*rowBytes;
			else
#endif
			pbits = bits + irow * rowBytes;  
#else
			pbits = bits + irow * pixwidth;
#endif

			if (rowBytes > 250 || pixelSize > 8)
				linelen = getWord();
			else
				linelen = getByte();

			if (linelen >= kFudgebuf+rowBytes)   
				Nlm_Message(MSG_FATAL,"drawpict:linelen > rowbytes");
	 
			getbytes((char*) linebuf, linelen);
			
			for (j = 0; j < linelen; ) {
				if (linebuf[j] & 0x80) {
					len = ((linebuf[j] ^ 255) & 255) + 2;
					buflen = pkpixsize;
#if DONTEXPAND
					bytepixels= linebuf + j+1;
#else
					bytepixels = expandbuf(linebuf + j+1, &buflen, pixelSize);
#endif
					for (k = 0; k < len; k++) {
						for (l = 0; l < buflen; l++) {
							abyte= *bytepixels++;
#ifdef WIN_MSWIN
							if (isBitmap) abyte= (byte) ~abyte;
#endif
#ifdef WIN_MOTIF
							if (isBitmap) abyte= XByteFlip[abyte];
#endif
							*pbits++ = abyte;
							}
						bytepixels -= buflen;
						}
					j += 1 + pkpixsize;
					}
				else {
					len = (linebuf[j] & 255) + 1;
					buflen = len * pkpixsize;
#if DONTEXPAND
					bytepixels= linebuf + j+1;
#else
					bytepixels = expandbuf(linebuf + j+1, &buflen, pixelSize);
#endif
					for (k = 0; k < buflen; k++) {
							abyte= *bytepixels++;
#ifdef WIN_MSWIN
							if (isBitmap) abyte= (byte) ~abyte;
#endif
#ifdef WIN_MOTIF
							if (isBitmap) abyte= XByteFlip[abyte];
#endif
							*pbits++ = abyte;
							}
					j += len * pkpixsize + 1;
					}
				}
			}
///************************/

		}

	Nlm_MemFree(linebuf);
	return bits;
}




void DDrawPict::read_pixmap(PIXMapPtr p, word* rowBytes)
{
	Nlm_RecT	arec;
	if (rowBytes != NULL) *rowBytes = getWord();

	getRectAsis( &arec);
	p->Bounds.left= arec.left;
	p->Bounds.top= arec.top;
	p->Bounds.right= arec.right;
	p->Bounds.bottom= arec.bottom;
	p->pmVersion = getWord();
	p->packType = getWord();
	p->packSize = getLong();
	p->hRes = getLong();
	p->vRes = getLong();
	p->pixelType = getWord();
	p->pixelSize = getWord();
	p->cmpCount = getWord();
	p->cmpSize = getWord();
	p->planeBytes = getLong();
	p->pmTable = getLong();
	p->pmReserved = getLong();

	if (p->pixelType != 0)
		Nlm_Message(MSG_FATAL,"drawpict: sorry, I only do chunky format");
	if (p->cmpCount != 1)
		Nlm_Message(MSG_FATAL,"drawpict: sorry, cmpCount != 1");
	if (p->pixelSize != p->cmpSize)
		Nlm_Message(MSG_FATAL,"drawpict: oops, pixelSize != cmpSize");
}


KolorTable DDrawPict::read_colour_table(void)
{
	longword ctSeed;
	word ctFlags;
	word ctSize;
	word val;
	long i;
	/* RGBKolorPtr colour_table;*/
	KolorTable	kolorTab;
	KolorSpec*	kolors;
	
	kolorTab.ctSeed = ctSeed = getLong();
	kolorTab.ctFlags = ctFlags =  getWord();
	kolorTab.ctSize = ctSize = getWord();

#if 0
	kolorTab.ctSeed = ctSeed= 1023; /* == minSeed in qd colormanager */
#endif

	kolors = (KolorSpec*) Nlm_MemNew( sizeof(KolorSpec) * (ctSize + 1));
	if (kolors == NULL)
		Nlm_Message(MSG_FATAL,"drawpict: no memory for colors table");
	kolorTab.ctTable= kolors;
	
	for (i = 0; i <= ctSize; i++) {
		val = getWord();
		if (ctFlags & 0x8000) val = i;
		if (val > ctSize)
			Nlm_Message(MSG_FATAL,"drawpict: pixel value greater than colour table size");
		kolors[val].value= val;
		kolors[val].rgb.red = getWord();
		kolors[val].rgb.green = getWord();
		kolors[val].rgb.blue = getWord();
		}

	return kolorTab;
}


void DDrawPict::read_pattern(void)
{
	word 		PatType;
	word 		rowBytes;
	PIXMap 	p;
	byte* 		bits;
	KolorTable 	ct;
	Nlm_RecT	arec;
	ulong	unpackSize;
	
	PatType = getWord();
	switch (PatType) {
	case 2:
		skipbytes(8); /* old pattern data */
		skipbytes(5); /* RGB for pattern */
		break;
	case 1:
		skipbytes(8); /* old pattern data */
		read_pixmap(&p, &rowBytes);
		ct = read_colour_table();
		arec.left= p.Bounds.left;
		arec.top= p.Bounds.top;
		arec.right= p.Bounds.right;
		arec.bottom= p.Bounds.bottom;
		bits = unpackbits(&arec, rowBytes, p.pixelSize, unpackSize);
		Nlm_MemFree(bits);
		Nlm_MemFree(ct.ctTable);
		break;
	default:
		Nlm_Message(MSG_FATAL,"drawpict: unknown pattern type in read_pattern");
	}
}




#ifdef WIN_MOTIF
// static
Local void FindXColor( Display *dsp, Colormap cmap, XColor* colr)
{
 	long  i, match, cindx, numCells;
  long  rd, gd, bd, dist, mindist;
  static XColor defColrs[256];
  static Boolean haveColrs= false;

  			// must leave some of 256 colors for other apps
  			// damn X for its messy color handling !
  if (gNAllocatedColors < 248) 
  	match= XAllocColor( dsp, cmap, colr);
  else
  	match= 0;
  	
  if (match == 0) {
    numCells= DisplayCells( dsp, DefaultScreen(dsp));
    if (!haveColrs) {
      for (i=0; i<numCells; i++) defColrs[i].pixel= i;
      XQueryColors( dsp, cmap, defColrs, numCells);
      haveColrs= true;
      }
    mindist= 196608; // 256 * 256 * 3  
    //mindist= 768; // 256 * 3 */
    cindx= colr->pixel;
    for (i= 0; i<numCells; i++) {
      rd= (defColrs[i].red >> 8) - (colr->red >> 8);
      gd= (defColrs[i].green >> 8) - (colr->green >> 8);
      bd= (defColrs[i].blue >> 8) - (colr->blue >> 8);
      dist= (rd * rd) + (gd * gd) + (bd * bd);
      //dist= abs(rd) + abs(gd) + abs(bd);
      if (dist < mindist) {
         mindist= dist;
         cindx= defColrs[i].pixel;
         if (dist==0) break;
         }
      }
    colr->pixel= cindx;
    colr->red  = defColrs[cindx].red;
    colr->green= defColrs[cindx].green;
    colr->blue = defColrs[cindx].blue;
    }
  else {
    if (gNAllocatedColors==0) {
    	for (i=0; i<256; i++) gAllocatedColors[i]= 0;
    	}
    gAllocatedColors[colr->pixel]++;
    if (gAllocatedColors[colr->pixel] == 1) {
    	gNAllocatedColors++;
    	haveColrs= 0;
    	}
    }
}
#endif


// static
void DDrawPict::Pixmap2Xcolors( KolorTable& kolorTab, unsigned char* bits, ulong bitsize,
																	Nlm_Boolean returnXtabInBits)
{
#ifdef WIN_MOTIF
	if (!(kolorTab.ctFlags & kXColorTableFlag)) {
		byte 		xtab[256], ival, abyte;
	  short 	ic;
	  XColor	xcol;
	  Display * display = Nlm_currentXDisplay;
	  short   screenNum= DefaultScreen( display);
		Visual 	* visual= DefaultVisual( display, screenNum);
		Colormap colormap = DefaultColormap( display, screenNum);    
	
		xcol.flags= DoRed | DoGreen | DoBlue;
		for (ic=0; ic <= kolorTab.ctSize; ic++) {
			ival= (byte) kolorTab.ctTable[ic].value;
			xcol.red=   kolorTab.ctTable[ic].rgb.red;
			xcol.green= kolorTab.ctTable[ic].rgb.green;
			xcol.blue=  kolorTab.ctTable[ic].rgb.blue;
			FindXColor( display, colormap, &xcol);
			xtab[ival]= (byte) xcol.pixel;
			}
		kolorTab.ctFlags |= kXColorTableFlag;
		
		if (returnXtabInBits && bits && bitsize >= 256) {
			for (ic= 0; ic < 256; ic++) bits[ic]= xtab[ic];
			}
		else if (bits) {
			register byte* pbits= bits;
			for (long ib= 0; ib<bitsize; ib++) { 
				//abyte= XByteFlip[*pbits]; //must use unflipped vals, fix now in unpackbits
				*pbits= (byte) xtab[*pbits];
				pbits++;
				}
			}
		}
#endif
}
 



void DDrawPict::drawPixmap( short version, word rowBytes, short isregion )
{
	word 		mode;
	PIXMap 	p;
	word 		pixwidth;
	byte		* bits;
	KolorTable	kolorTab;
	Nlm_RecT	arec, sourcerec, destrec;
	ulong		unpackSize;
	
	read_pixmap(&p, NULL);
	pixwidth = p.Bounds.right - p.Bounds.left;
	kolorTab = read_colour_table();

	getRectAsis(&sourcerec);
	getRectLocal( &destrec);
	mode = getWord();
	if (isregion) skipPolyOrRegion();

	arec.left= p.Bounds.left;
	arec.top= p.Bounds.top;
	arec.right= p.Bounds.right;
	arec.bottom= p.Bounds.bottom;
	bits = unpackbits( &arec, rowBytes, p.pixelSize, unpackSize);
	if (!bits) return;

#ifdef WIN_MAC
	{
	Nlm_RectTool   srcRect, dstRect;
	PixMap    	srcBits;
	GrafPtr   	port;
	CTabHandle	cthand;
	long				ic, ctsize;
	//PaletteHandle oldph,ph;
	
	cthand= NULL;
	ctsize = (kolorTab.ctSize + 1) * sizeof(ColorSpec);
	cthand= (CTabHandle) NewHandle(sizeof(ColorTable) + ctsize);
	if (cthand) {
		(**cthand).ctSeed = kolorTab.ctSeed;
		(**cthand).ctFlags= kolorTab.ctFlags;
		(**cthand).ctSize = kolorTab.ctSize;
		/* MemCpy( &(**cthand).ctTable, &(kolorTab.ctTable), ctsize); */
		for (ic=0; ic <= kolorTab.ctSize; ic++) {
			(**cthand).ctTable[ic].value= kolorTab.ctTable[ic].value;
			(**cthand).ctTable[ic].rgb.red= kolorTab.ctTable[ic].rgb.red;
			(**cthand).ctTable[ic].rgb.green= kolorTab.ctTable[ic].rgb.green;
			(**cthand).ctTable[ic].rgb.blue= kolorTab.ctTable[ic].rgb.blue;
			}
		}
		
	Nlm_RecTToRectTool( &sourcerec, &srcRect);
	Nlm_RecTToRectTool( &destrec, &dstRect);

	srcBits.pmVersion= p.pmVersion;
	srcBits.packType= p.packType;
	srcBits.packSize= p.packSize;
	srcBits.hRes= p.hRes;
	srcBits.vRes= p.vRes;
	srcBits.pixelType= p.pixelType;
	srcBits.pixelSize= p.pixelSize;
	srcBits.cmpCount= p.cmpCount;
	srcBits.cmpSize= p.cmpSize;
	srcBits.planeBytes= p.planeBytes;
	srcBits.pmTable= cthand; 
	srcBits.pmReserved= p.pmReserved;
	
	srcBits.bounds.left= p.Bounds.left;
	srcBits.bounds.top = p.Bounds.top;
	srcBits.bounds.right= p.Bounds.right;
	srcBits.bounds.bottom= p.Bounds.bottom;
	srcBits.baseAddr = (Ptr) bits;
	srcBits.rowBytes = rowBytes;  
	
	GetPort (&port);

	CopyBits( (BitMap*) &srcBits, &port->portBits, &srcRect, &dstRect, mode, NULL);
	if (cthand) DisposHandle((Handle)cthand);
	Nlm_MemFree( bits);

	}
#endif


#ifdef WIN_MSWIN
  {
	HBITMAP       hBitmap, hOldBitmap;
	HDC           hMemoryDC, hDC;
	Nlm_Int4      dbisize, wmode;
	Nlm_Int2      ic, dwidth, dheight, swidth, sheight, rows, depth;
	BITMAPINFO		*dbi;

	  depth= p.pixelSize; /* ?! check display depth */
		switch (mode) {
			/* translate apple to x modes */
			case 0: case 8: wmode = SRCCOPY; break;  /* srcCopy */
			case 1: case 9: wmode = SRCAND; break; /* srcOr */
			case 2: case 10: wmode = 0x00990066; break;   /* srcXor */
			case 3: case 11: wmode = MERGEPAINT; break;   /* srcBic */
			default: 	wmode = SRCCOPY; /*WHITENESS;*/ break;
			}

	hDC= GetDC( Nlm_currentHWnd); //Nlm_currentHDC; /*GetDC( Nlm_currentHWnd); */
	if (bits && hDC) {
			dwidth= destrec.right - destrec.left;
			dheight= destrec.bottom - destrec.top;
			swidth= p.Bounds.right - p.Bounds.left;
			sheight= p.Bounds.bottom - p.Bounds.top;

#if 1
			dbisize= sizeof(BITMAPINFOHEADER) + (kolorTab.ctSize+1) * sizeof(RGBQUAD);
			dbi= (BITMAPINFO*) MemGet( dbisize, TRUE);
			dbi->bmiHeader.biSize= sizeof(BITMAPINFOHEADER); /* dbisize; */
			dbi->bmiHeader.biWidth= swidth;
			dbi->bmiHeader.biHeight= sheight;
			dbi->bmiHeader.biPlanes= 1; /*p.planeBytes; */
			dbi->bmiHeader.biBitCount=  p.pixelSize;
			dbi->bmiHeader.biCompression= 0;
			dbi->bmiHeader.biSizeImage= 0;
			dbi->bmiHeader.biClrUsed= kolorTab.ctSize + 1; 
			for (ic=0; ic <= kolorTab.ctSize; ic++) {
				/* !??! need to remap kolorTab if (table[ic].value != ic) !!! */
				dbi->bmiColors[ic].rgbRed= (byte) (kolorTab.ctTable[ic].rgb.red >>8);
				dbi->bmiColors[ic].rgbGreen= (byte) (kolorTab.ctTable[ic].rgb.green >>8);
				dbi->bmiColors[ic].rgbBlue= (byte) (kolorTab.ctTable[ic].rgb.blue >>8);
				}

#if 1
			hMemoryDC = CreateCompatibleDC (hDC);
#if 0
			// ?? is the 1st param == hDC or hMemoryDC
			hBitmap= CreateDIBitmap( hDC, &(dbi->bmiHeader), 0, NULL, NULL, 0);
			if (hBitmap) {
				 SetDIBits( hDC, hBitmap, 0, sheight, bits, dbi, DIB_RGB_COLORS);
				}
#else
			hBitmap= CreateDIBitmap( hDC, &(dbi->bmiHeader), CBM_INIT, bits, dbi, DIB_RGB_COLORS);
#endif

			hOldBitmap = (HBITMAP) SelectObject (hMemoryDC, hBitmap);
			if (hOldBitmap != NULL) {
				/*SetViewportOrg( hDC, 0, destrec.bottom);*/
				BitBlt ( hDC, destrec.left, destrec.top, swidth, sheight,
								hMemoryDC, 0, 0, wmode);
			  /*SetViewportOrg( hDC, 0, 0); */ 
				SelectObject (hMemoryDC, hOldBitmap);
				}
			DeleteDC (hMemoryDC);
			DeleteObject (hBitmap);
			/* ReleaseDC( Nlm_currentHWnd, hDC); */
#else
			/* hDC= CreateCompatibleDC(Nlm_currentHDC);*/ /*?? or BeginPaint()... */

			/* SetViewportOrg( hDC, 0, -destrec.bottom); */
			SetDIBitsToDevice( hDC, destrec.left, destrec.top, swidth, sheight,
					0, 0, swidth, sheight, (LPSTR) bits, (LPBITMAPINFO) dbi, DIB_RGB_COLORS
					); /* , wmode); */
			/* SetDIBitsToDevice StretchDIBits */
			/* SetViewportOrg( hDC, 0, 0);   */

			ReleaseDC( Nlm_currentHWnd, hDC);
			/*DeleteDC (hMemoryDC); */
#endif

			MemFree(dbi);
#else
			hBitmap = CreateBitmap ( swidth, sheight, 1, depth, (LPSTR) bits);
			hMemoryDC = CreateCompatibleDC (Nlm_currentHDC);
      hOldBitmap = SelectObject (hMemoryDC, hBitmap);
      if (hOldBitmap != NULL) {
				BitBlt (Nlm_currentHDC, destrec.left, destrec.top, swidth, sheight,
								hMemoryDC, 0, 0, wmode);
				SelectObject (hMemoryDC, hOldBitmap);
				}
			DeleteDC (hMemoryDC);
			DeleteObject (hBitmap);
#endif
			}
	}
	Nlm_MemFree( bits);
#endif


#ifdef WIN_MOTIF
	{
	short   depth, width, height, xmode;
	XImage	*ximage;
	Pixmap  pixmap;
	GC 			drawGC;
  short		ic, flag;
  XColor	xcol;
  byte  * pbits = bits;
  Display *display = Nlm_currentXDisplay;
  short   screenNum= DefaultScreen(display);
	Visual * visual= DefaultVisual( display, screenNum);
	Colormap colormap = DefaultColormap( display, screenNum);    

  width  = destrec.right - destrec.left;
  height = destrec.bottom - destrec.top;
  depth  = p.pixelSize; /* ?! check display depth */
  switch (mode) {
		/* translate apple to x modes */
	  default: xmode= COPY_MODE;
	  }


	Pixmap2Xcolors( kolorTab, bits, unpackSize, false);
#if 0
	{
	byte xtab[256], ival, abyte;
	xcol.flags= DoRed | DoGreen | DoBlue;
	for (ic=0; ic <= kolorTab.ctSize; ic++) {
		ival= (byte) kolorTab.ctTable[ic].value;
		xcol.red=   kolorTab.ctTable[ic].rgb.red;
		xcol.green= kolorTab.ctTable[ic].rgb.green;
		xcol.blue=  kolorTab.ctTable[ic].rgb.blue;
		FindXColor( display, colormap, &xcol);
		xtab[ival]= (byte) xcol.pixel;
		}
		
	long isize= unpackSize; //width * height;
	pbits= bits;
	for (long ib= 0; ib<isize; ib++) { 
		//abyte= XByteFlip[*pbits]; // !! YES !! ?? do we need to unflip this for pixmap !? (not for bitmap)
		*pbits= (byte) xtab[*pbits];
		pbits++;
		}
	pbits= bits;
	}
#endif

#if 0
	if (!gPixColorsDone &&  p.pixelSize <= 8)  {
	 // this jazz is a hack good only for 8bit pixels (or less??) !!
   byte xtab[256], ival;
   gPixColorsDone= true;

/* need to do something about color table -- in GC ?? */
    xcol.flags= DoRed | DoGreen | DoBlue;
		for (ic=0; ic <= kolorTab.ctSize; ic++) {
        ival= kolorTab.ctTable[ic].value;
				xcol.red=   kolorTab.ctTable[ic].rgb.red;
				xcol.green= kolorTab.ctTable[ic].rgb.green;
				xcol.blue=  kolorTab.ctTable[ic].rgb.blue;
// damn xwin won't let me use colortab mapping !? stupid get
 				//XStoreColor( display, colormap, &xcol);
        XAllocColor( display, colormap, &xcol);
        xtab[ival]= xcol.pixel;
       }

// damn so we do it the hard way and change all pixmap bits values !
// Double Damn -- can't do this on each redraw !! only once !!!
   byte cval;
   for (long ib= 0; ib<unpackSize; ib++) {
	 	  cval= (byte) bits[ib];
      bits[ib]= xtab[cval];
      }
}
#endif

		short rowpad = 8; // 8,16 or 32 bits.  Use 16 if rowbytes == 1 + width !? for short pad
    short bitsoffset= 0;
    ximage = XCreateImage ( display, visual,
								depth, ZPixmap, bitsoffset, (char *) pbits, 
				        width, height, rowpad, rowBytes & 0x7fff);
		if (ximage) {
			pixmap = XCreatePixmap(display, Nlm_currentXWindow, width, height, depth);
			drawGC = XCreateGC(display, Nlm_currentXWindow, 0, NULL); 
			XSetFunction(display, drawGC, GXcopy); 
			XPutImage(display, pixmap, drawGC, ximage, 0,0, 0,0, width, height);
			XCopyArea(display, pixmap, Nlm_currentXWindow, drawGC, 0, 0, 
					width, height, destrec.left - Nlm_XOffset, destrec.top - Nlm_YOffset);
			XFreeGC(display, drawGC); 
      XDestroyImage (ximage);
      XFreePixmap (display, pixmap);
			}
	}

#endif

	Nlm_MemFree( bits);
	Nlm_MemFree( kolorTab.ctTable);
}



void DDrawPict::drawPixmapOp9A( short version )
{
	word 		mode;
	PIXMap 	p;
	word 		pixwidth;
	byte	* bits;
	//KolorTable kolorTab;
	Nlm_RecT 	arec, sourcerec, destrec;
	ulong		unpackSize;
	
	skipbytes(4);
	p.pmVersion= getWord();	/* version */
	/* read_pixmap(&p, NULL); << not quite same for op9a */
	getRectAsis( &arec);
	p.Bounds.left= arec.left;
	p.Bounds.top= arec.top;
	p.Bounds.right= arec.right;
	p.Bounds.bottom= arec.bottom;
	/* p.pmVersion = getWord(); */
	p.packType = getWord();
	p.packSize = getLong();
	p.hRes = getLong();
	p.vRes = getLong();
	p.pixelType = getWord();
	p.pixelSize = getWord();
	p.pixelSize = getWord(); /* ?? is this right, two words ? */
	p.cmpCount = getWord();
	p.cmpSize = getWord();
	p.planeBytes = getLong();
	p.pmTable = getLong();
	p.pmReserved = getLong();

	pixwidth = p.Bounds.right - p.Bounds.left;
	/* kolorTab = read_colour_table(); << !! no color table in this op */
	
	getRectAsis( &sourcerec);
	getRectLocal( &destrec);
	mode = getWord();

	arec.left= p.Bounds.left;
	arec.top= p.Bounds.top;
	arec.right= p.Bounds.right;
	arec.bottom= p.Bounds.bottom;
	bits = unpackbits( &arec, 0, p.pixelSize, unpackSize);
	if (!bits) return;

#ifdef WIN_MAC
	{
  Nlm_RectTool   srcRect, dstRect;
  PixMap    srcBits;
  GrafPtr   port;
	CTabPtr		ctptr;
	
	Nlm_RecTToRectTool( &sourcerec, &srcRect);
	Nlm_RecTToRectTool( &destrec, &dstRect);
	ctptr= NULL; /* = (CTabPtr) &kolorTab; */
	
	/* ?? srcBits= *((PixMapPtr) &p); */
	srcBits.pmVersion= p.pmVersion;
	srcBits.packType= p.packType;
	srcBits.packSize= p.packSize;
	srcBits.hRes= p.hRes;
	srcBits.vRes= p.vRes;
	srcBits.pixelType= p.pixelType;
	srcBits.pixelSize= p.pixelSize;
	srcBits.cmpCount= p.cmpCount;
	srcBits.cmpSize= p.cmpSize;
	srcBits.planeBytes= p.planeBytes;
	srcBits.pmTable= &ctptr; /* p.pmTable;*/
	srcBits.pmReserved= p.pmReserved;
	
	srcBits.bounds.left= p.Bounds.left;
	srcBits.bounds.top = p.Bounds.top;
	srcBits.bounds.right= p.Bounds.right;
	srcBits.bounds.bottom= p.Bounds.bottom;
	srcBits.baseAddr = (Ptr) bits;
	/* srcBits.rowBytes = rowBytes;  */
	srcBits.rowBytes = (p.Bounds.right - p.Bounds.left - 1) / 8 + 1;
	
	GetPort (&port);
	CopyBits( (BitMap*) &srcBits, &port->portBits, &srcRect, &dstRect, mode, NULL);
	}
#endif
#ifdef WIN_MSWIN
#endif
#ifdef WIN_MOTIF
#endif

	Nlm_MemFree( bits);
	/* Nlm_MemFree( kolorTab.ctTable); */
}




void DDrawPict::drawBitmap(short version, short rowBytes, Nlm_Boolean isregion)
{
	Nlm_RecT bounds, sourcerec, destrec;
	word mode;
	byte *bits;
	ulong	unpackSize;
	
	getRectAsis(&bounds);
	getRectAsis(&sourcerec);
	getRectLocal( &destrec);
	mode = getWord();
	if (isregion) skipPolyOrRegion();

	bits = unpackbits( &bounds, rowBytes, 1, unpackSize);
	if (!bits) return;
	
#ifdef WIN_MAC
	{
  Nlm_RectTool   boundsRect, srcRect, dstRect;
  BitMap    srcBits;
  GrafPtr   port;

	Nlm_RecTToRectTool( &bounds, &boundsRect);
	Nlm_RecTToRectTool( &sourcerec, &srcRect);
	Nlm_RecTToRectTool( &destrec, &dstRect);
	srcBits.baseAddr = (Ptr) bits;
	srcBits.rowBytes = rowBytes;  
	srcBits.bounds = boundsRect;
	GetPort (&port);
	/* mode= srcCopy; /* testing... */
	CopyBits( &srcBits, &port->portBits, &srcRect, &dstRect, mode, NULL);
	}
#endif
#ifdef WIN_MSWIN
	/* Nlm_CopyBits( &destrec, bits); /* this is a hack -- should do directly */
  {
	HBITMAP       hBitmap, hOldBitmap;
	HDC           hMemoryDC;
	Nlm_Int4      wmode;
	Nlm_Int2      rop2, width, height, rows, depth;

		depth= 1; /* ?! check display depth */
		switch (mode) {
			/* translate apple to x modes */
			case 0: case 8: wmode = SRCCOPY; break;  /* srcCopy */
			case 1: case 9: wmode = SRCAND; break; /* srcOr */
			case 2: case 10: wmode = 0x00990066; break;   /* srcXor */
			case 3: case 11: wmode = MERGEPAINT; break;   /* srcBic */
			default: 	wmode = SRCCOPY; /*WHITENESS;*/ break;
			}

	if (bits && Nlm_currentHDC) {
			width= destrec.right - destrec.left;
			height= destrec.bottom - destrec.top;

			hBitmap = CreateBitmap ( width, height, 1, depth, (LPSTR) bits);
			hMemoryDC = CreateCompatibleDC (Nlm_currentHDC);
			hOldBitmap = (HBITMAP) SelectObject (hMemoryDC, hBitmap);
      if (hOldBitmap != NULL) {
				BitBlt (Nlm_currentHDC, destrec.left, destrec.top, width, height,
								hMemoryDC, 0, 0, wmode);
				SelectObject (hMemoryDC, hOldBitmap);
				}
			DeleteDC (hMemoryDC);
      DeleteObject (hBitmap);
			}
	}

#endif
#ifdef WIN_MOTIF
	/* Nlm_CopyBits( &destrec, bits); /* this is a hack -- should do directly */

	{
	short  width, height, xmode;
  	Pixmap        pixmap;

		switch (mode) {
			/* translate apple to x modes */
			default: xmode= COPY_MODE;
			}

      width = destrec.right - destrec.left;
      height = destrec.bottom - destrec.top;
      pixmap = XCreateBitmapFromData (Nlm_currentXDisplay, Nlm_currentXWindow,
                                      (char *) bits, width, height);
      if (xmode != MERGE_MODE) {
        XCopyPlane (Nlm_currentXDisplay, pixmap, Nlm_currentXWindow,
                    Nlm_currentXGC, 0, 0, width, height,
                    destrec.left - Nlm_XOffset, destrec.top - Nlm_YOffset, 1);
      	} 
			else {
        XSetClipOrigin (Nlm_currentXDisplay, Nlm_currentXGC,
                        destrec.left - Nlm_XOffset, destrec.top - Nlm_YOffset);
        XSetClipMask (Nlm_currentXDisplay, Nlm_currentXGC, pixmap);
        XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, GXcopy);
        XCopyPlane (Nlm_currentXDisplay, pixmap, Nlm_currentXWindow,
                    Nlm_currentXGC, 0, 0, width, height,
                    destrec.left - Nlm_XOffset, destrec.top - Nlm_YOffset, 1);
        XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, GXand);
        XSetClipOrigin (Nlm_currentXDisplay, Nlm_currentXGC, 0, 0);
        if (Nlm_clpRgn != NULL) {
          XSetRegion (Nlm_currentXDisplay, Nlm_currentXGC, (Nlm_RgnTool) Nlm_clpRgn);
        } else {
          XSetClipMask (Nlm_currentXDisplay, Nlm_currentXGC, None);
        }
      }
      XFreePixmap (Nlm_currentXDisplay, pixmap);
	}

#endif

	Nlm_MemFree( bits);
}




enum fontStyles {
	kFontBold = 1,
	kFontItalic = 2,
	kFontUline = 4,
	kFontOutline = 8,
	kFontShadow = 16,
	kFontCondense = 32,
	kFontExtend = 64
	};



void DDrawPict::InstallFont()
{
	fFont= Nlm_GetFont(fFontname, fFontsize, fFontstyle & kFontBold,
			fFontstyle & kFontItalic, fFontstyle & kFontUline, fFontfamily);
	Nlm_SelectFont( fFont);	
	fNeedInstallFont= FALSE;		
}

void DDrawPict::SetFont(word fontnum)
{
	short i;
	if (fontnum == fLastFontnum) {
		fFontname= fLastFontname;
		fNeedInstallFont= TRUE;	
		fLastFontnum = -1; /* clear it after 1 use ? */	
		return;
		}		
	else for ( i=0; QuickDrawFont[i].value>=0; i++) {
		if (fontnum == QuickDrawFont[i].value) {
			fFontname= QuickDrawFont[i].name;
			fNeedInstallFont= TRUE;		
			return;
			}
		}
}

void DDrawPict::SetFontStyle(byte fstyle)
{
	fFontstyle= fstyle;
	fNeedInstallFont= TRUE;		
}

void DDrawPict::SetFontSize(word fsize)
{
	fFontsize= fsize;
	fNeedInstallFont= TRUE;		
}




void DDrawPict::getAArc(Nlm_RectPtr arec, Nlm_PointPtr pt, Nlm_PointPtr pt1)   
{ 
	/* need some dang trigonometry here... */
	/* ?? just stuff a 45o angle for now ?? */
	short				arcStart, arcEnd;
	arcStart = getWord(); 
	arcEnd   = getWord();  
	pt->x = (arec->left + arec->right) >> 1; 
	pt->y = arec->top;  
	pt1->x= arec->right; 
	pt1->y= (arec->top + arec->bottom) >> 1; 
}

void DDrawPict::getAPoly(Nlm_RectPtr polybox, short* polycount, Nlm_PointPtr* thePoly) 
{ 
	short 				sizepoly,	npoly, ip;
	Nlm_PoinT  		polypt;
	Nlm_PointPtr 	apoly;
	
	apoly= *thePoly;
	sizepoly= getWord(); 
	getRectLocal( polybox); 
	npoly = (sizepoly - 10) >> 2;   
	if (!apoly) apoly= (Nlm_PoinT*) Nlm_MemNew( npoly * sizeof(Nlm_PoinT));  
	else apoly=  (Nlm_PoinT*) Nlm_MemMore( apoly, npoly * sizeof(Nlm_PoinT)); 
	if (apoly) for (ip= 0; ip<npoly; ip++) {  
		getPointLocal( &polypt);  
		apoly[ip]= polypt;  
		} 
	*thePoly= apoly;
	*polycount = npoly;
		/* debug TESTING... */
	/* *polycount= 0; */
}
 	
 	
void DDrawPict::getARgn(Nlm_RectPtr rgnbox, Nlm_RegioN* theRgn, Nlm_Boolean forclip)
{ 
  /* ! each win system has (?) own region format !! need to translate mac to others ! */
  /* just skip for now ! */
	short rgnsize;
	Nlm_RegioN	argn;
	
	argn= *theRgn;
	rgnsize= getWord(); 
	getRectLocal( rgnbox);
	
		/* this fix mainly for clip regions -- make sure it falls inside fPicRect */
	if (forclip) {
		if (rgnbox->left < fPicRect.left) rgnbox->left= fPicRect.left;
		if (rgnbox->top < fPicRect.top) rgnbox->top= fPicRect.top;
		if (rgnbox->right > fPicRect.right) rgnbox->right= fPicRect.right;
		if (rgnbox->bottom > fPicRect.bottom) rgnbox->bottom= fPicRect.bottom;
		}
	
	if (!argn) argn= Nlm_CreateRgn(); 
	else Nlm_ClearRgn( argn);  
	Nlm_LoadRectRgn( argn, rgnbox->left, rgnbox->top, rgnbox->right, rgnbox->bottom);  
	skipbytes( rgnsize - 10);  /* skip rgn data */
	*theRgn= argn;
}



void DDrawPict::getForeColor()
{
	switch (getLong()) {
		case 33: Nlm_Black(); break;
		case 30: Nlm_White(); break;
		case 205: Nlm_Red(); break;
		case 341: Nlm_Green(); break;
		case 409: Nlm_Blue(); break;
		case 273: Nlm_Cyan(); break;
		case 137: Nlm_Magenta(); break;
		case 69: Nlm_Yellow(); break;
		}
}


void DDrawPict::getBackColor()
{
	switch (getLong()) {
		case 33: Nlm_SelectBackColor(0,0,0); break;
		case 30: Nlm_SelectBackColor(255,255,255); break;
		case 205: Nlm_SelectBackColor(255,0,0); break;
		case 341: Nlm_SelectBackColor(0,255,0); break;
		case 409: Nlm_SelectBackColor(0,0,255); break;
		case 273: Nlm_SelectBackColor(0,255,255); break;
		case 137: Nlm_SelectBackColor(255,0,255); break;
		case 69: Nlm_SelectBackColor(255,255,0); break;
		}
}

void DDrawPict::getPenMode()
{
	switch (getWord()) {
		case 0: Nlm_CopyMode(); break;
		case 1: Nlm_MergeMode(); break;
		case 2: Nlm_InvertMode(); break;
		case 3: Nlm_EraseMode(); break;
		}
}


void DDrawPict::handleRectOpcode( word opcode)
{
	Nlm_PoinT		pt, pt1;

	getRectLocal( &fTheRect);

#if 0
	Nlm_MoveTo(fTheRect.left,fTheRect.top); /* ?? */
#endif
	switch (opcode) {
		case oframeRect:   if (fPenshown && fUnclipped) Nlm_FrameRect( &fTheRect); break;		
		case oeraseRect:   if (fUnclipped) Nlm_EraseRect( &fTheRect); break;		
		case opaintRect:   if (fUnclipped) Nlm_PaintRect( &fTheRect); break;		
		case oinvertRect:  if (fUnclipped) Nlm_InvertRect( &fTheRect); break;		
		case ofillRect:    /* setPattern(); Nlm_PaintRect( &fTheRect);*/  break;		

		case oframeRRect:   if (fPenshown && fUnclipped) Nlm_FrameRoundRect( &fTheRect, fOvSize.x, fOvSize.y); break;		
		case oeraseRRect:   if (fUnclipped) Nlm_EraseRoundRect( &fTheRect, fOvSize.x, fOvSize.y); break;		
		case opaintRRect:   if (fUnclipped) Nlm_PaintRoundRect( &fTheRect, fOvSize.x, fOvSize.y); break;		
		case oinvertRRect:  if (fUnclipped) Nlm_InvertRoundRect( &fTheRect, fOvSize.x, fOvSize.y); break;		
		case ofillRRect:    /* setPattern(); Nlm_PaintRoundRect( &fTheRect, fOvSize.x, fOvSize.y); */ break;		

		case oframeOval:   if (fPenshown && fUnclipped) Nlm_FrameOval( &fTheRect); break;		
		case oeraseOval:   if (fUnclipped) Nlm_EraseOval( &fTheRect); break;		
		case opaintOval:   if (fUnclipped) Nlm_PaintOval( &fTheRect); break;		
		case oinvertOval:  if (fUnclipped) Nlm_InvertOval( &fTheRect); break;		
		case ofillOval:    /* setPattern(); Nlm_PaintOval( &fTheRect); */ break;		

		case oframeArc:   getAArc(&fTheRect,&pt,&pt1); if (fPenshown && fUnclipped) Nlm_FrameArc( &fTheRect, pt, pt1); break;		
		case oeraseArc:   getAArc(&fTheRect,&pt,&pt1); if (fUnclipped) Nlm_EraseArc( &fTheRect, pt, pt1); break;		
		case opaintArc:   getAArc(&fTheRect,&pt,&pt1); if (fUnclipped) Nlm_PaintArc( &fTheRect, pt, pt1); break;		
		case oinvertArc:  getAArc(&fTheRect,&pt,&pt1); if (fUnclipped) Nlm_InvertArc( &fTheRect, pt, pt1); break;		
		case ofillArc:    getAArc(&fTheRect,&pt,&pt1); /* setPattern(); Nlm_PaintArc( &fTheRect, pt, pt1); */ break;		
	}
}

void DDrawPict::handleSameRectOpcode( word opcode)
{
	Nlm_PoinT		pt, pt1;

	switch (opcode) {

		case oframeSameRect:  if (fPenshown && fUnclipped) Nlm_FrameRect( &fTheRect); break;		
		case oeraseSameRect:  if (fUnclipped) Nlm_EraseRect( &fTheRect); break;		
		case opaintSameRect:  if (fUnclipped) Nlm_PaintRect( &fTheRect); break;		
		case oinvertSameRect: if (fUnclipped) Nlm_InvertRect( &fTheRect); break;		
		case ofillSameRect: /* setPattern(); Nlm_PaintRect( &fTheRect); */ break;		

		case oframeSameRRect:  if (fPenshown && fUnclipped) Nlm_FrameRoundRect( &fTheRect, fOvSize.x, fOvSize.y); break;		
		case oeraseSameRRect:  if (fUnclipped) Nlm_EraseRoundRect( &fTheRect, fOvSize.x, fOvSize.y); break;		
		case opaintSameRRect:  if (fUnclipped) Nlm_PaintRoundRect( &fTheRect, fOvSize.x, fOvSize.y); break;		
		case oinvertSameRRect: if (fUnclipped) Nlm_InvertRoundRect( &fTheRect, fOvSize.x, fOvSize.y); break;		
		case ofillSameRRect: /* setPattern(); Nlm_PaintRoundRect( &fTheRect, fOvSize.x, fOvSize.y); */  break;		

		case oframeSameOval:  if (fPenshown && fUnclipped) Nlm_FrameOval( &fTheRect); break;		
		case oeraseSameOval:  if (fUnclipped) Nlm_EraseOval( &fTheRect); break;		
		case opaintSameOval:  if (fUnclipped) Nlm_PaintOval( &fTheRect); break;		
		case oinvertSameOval: if (fUnclipped) Nlm_InvertOval( &fTheRect); break;		
		case ofillSameOval: /* setPattern(); Nlm_PaintOval( &fTheRect); */ break;		

		case oframeSameArc:  getAArc(&fTheRect,&pt,&pt1); if (fPenshown && fUnclipped) Nlm_FrameArc( &fTheRect, pt, pt1); break;		
		case oeraseSameArc:  getAArc(&fTheRect,&pt,&pt1); if (fUnclipped) Nlm_EraseArc( &fTheRect, pt, pt1); break;		
		case opaintSameArc:  getAArc(&fTheRect,&pt,&pt1); if (fUnclipped) Nlm_PaintArc( &fTheRect, pt, pt1); break;		
		case oinvertSameArc: getAArc(&fTheRect,&pt,&pt1); if (fUnclipped) Nlm_InvertArc( &fTheRect, pt, pt1); break;		
		case ofillSameArc: /* setPattern();  Nlm_PaintArc( &fTheRect, pt, pt1); */ break;		

		}
}



void* DDrawPict::IsPICT( void* pictdata, ulong datasize, Nlm_Boolean dontTestForHeader)
{	
	unsigned char		b;
	Nlm_RecT				aPicFrame;
	unsigned short	picversion, picSize;
	
	if (!pictdata) return NULL;
TestAgain:
	fPict=  (byte*) pictdata;
	picSize= getWord(); 
	getRectAsis( &aPicFrame);
#if ANOTHER_POSSIBLE_PICT_TEST
	if (picSize == 0) goto TestHead; 
	if (fPicRect.right - fPicRect.left <= 0 
	 || fPicRect.bottom - fPicRect.top <= 0)  
		goto TestHead;  
#endif

	while ((b = getByte()) == 0) ;
	if (b != 0x011) goto TestHead; //return NULL;
	b = getByte();
	switch (b) {
		case 1:
			picversion= 1;
			return pictdata;
		case 2:
			picversion = 2;
			if (getByte() != 0xff) goto TestHead; //return NULL;
			return pictdata;
		default:
			goto TestHead; //return NULL;
		}

TestHead:
	if (dontTestForHeader || datasize<512) 
		return NULL;
	else { // skip possible 512-byte MacDraw header...
		pictdata = (Nlm_VoidPtr) ((unsigned long)pictdata + 512); 
		datasize -= 512; 	
		dontTestForHeader= TRUE;
		goto TestAgain;
		}

}


void DDrawPict::GetFrame( Nlm_RectPtr frame, void* pictdata, ulong picsize, short kind)
{
	if (!pictdata || !frame) return;
	if (kind != kPictMac) return;
	fAlign= 0;
	fPicsize= picsize;
	fPict= (byte*) pictdata;
	short dummyPicSize= getWord();  
	getRectAsis( &fPicFrame);
	*frame= fPicFrame;
}


void DDrawPict::Draw( Nlm_RectPtr r, void* pictdata, ulong picsize, short kind)
{
	/* translator from Apple Quickdraw PICT data format to NCBIDraw methods */
	/* based on PICT I and II opcodes described in Inside Macintosh vol. V */
	/* some code from picttoppm.c, Copyright 1993 George Phillips <phillips@cs.ubc.ca> */
	
	enum { kMaxBuf = 257 };

	byte				b, *bp;
	word			 	w, opcode;
	longword	 	curpat;
	short				i, dummyPicSize;
	Nlm_PoinT		pt, pt1;
	Nlm_PoinT		curpt; //-- track all moves w/ this?
	word			 	red, green, blue;
	short				dh, dv;
	Nlm_Boolean			dorgn, done, clipWasSet, colored;
	char				*cp, buf[kMaxBuf];
	
	Nlm_PointPtr	apoly = NULL;
	Nlm_RecT		polybox;
	short				npoly;

	Nlm_RegioN	argn = NULL;
	Nlm_RecT		rgnbox;	
	
	
	if (!pictdata) return;
	if (kind != kPictMac) return;
	
#ifdef PLIST
	char fname[128];
	strcpy(fname,"work:+PicList-");
	if (gDoPlist) gPlist= fopen( fname, "w");
#endif

	fAlign= 0;
	fPicsize= picsize;
	fNeedInstallFont= true;
	fFontname = "Times"; 
	fFontfamily = "Roman";
	fFontsize = 12; 
	fFontstyle = 0;
	
	fPicRect=  *r;
	fPict= (byte*) pictdata;
	dummyPicSize= getWord();  
	getRectAsis( &fPicFrame);
	clipWasSet= FALSE; 
	fPenshown= TRUE;
	fUnclipped = TRUE;
	curpat= 0;
	colored= false;
	Nlm_MoveTo(fPicRect.left,fPicRect.top); /* ?? */
#ifdef PLIST
	if (gDoPlist) fprintf(gPlist, "\n global left,top: %d,%d\n",fPicRect.left,fPicRect.top);
	if (gDoPlist) fprintf(gPlist, " frame  left,top: %d,%d\n",fPicFrame.left,fPicFrame.top);
#endif

	while ((b = getByte()) == 0) ;
	if (b != 0x011) {
		bp=  (byte*) pictdata;
		cp= buf;
		for (i= 0; i<12; i++) { sprintf(cp, "%x%x ", *bp, *(bp+1)); cp += 5; bp += 2; }
		if (fVerbose) { Nlm_Message(MSG_OK,"drawpict: No version number %s", buf); fVerbose= false; }
		return;
		}

	b = getByte();
	switch (b) {
		case 1:
			fPictVersion= 1;
			break;
		case 2:
			fPictVersion = 2;
			if (getByte() != 0xff) {
				if (fVerbose) { Nlm_Message(MSG_OK,"drawpict: can only do version 2, subcode 0xff"); fVerbose= false; }
				return;
				}
			break;
		default:
			fPictVersion= 0;
			bp= (byte*) pictdata;
			cp= buf;
			for (i= 0; i<12; i++) { sprintf(cp, "%x%x ", *bp, *(bp+1)); cp += 5; bp += 2; }
			if (fVerbose) { Nlm_Message(MSG_OK,"drawpict: unknown format %s", buf); fVerbose= false; }
			return;
		}
				
	curpt.x= curpt.y= 0;
	done= false;
	while (true) { 
	
		if (done || (fPicsize && fAlign>=fPicsize)) {
#ifdef PLIST
			if (gDoPlist) fprintf(gPlist, "\n\n");
			fclose(gPlist);
			if (!done) Nlm_Beep();
#endif
			if (argn) argn= Nlm_DestroyRgn(argn);  
		  if (apoly) Nlm_MemFree(apoly);  
		  if (clipWasSet) Nlm_ResetClip();  
			/* ^^ Nlm_ResetClip IS BAD, can lead to messes, but NCBI Draw hasn't any GetClip() yet*/
			return;
			}
				
		opcode= getOpcode();

		if (fNeedInstallFont && opcode != oTxFont && opcode != oTxFace 
		    && opcode != oTxSize && opcode != oTxMode)  
			InstallFont();
			
		switch (opcode) {
		
			case oopEndPic: 
				done= true;
			  break; 

			case oNOP			: break; 
			case oVersion	: (void) getByte(); break;
			case oHeaderOp: skipbytes(24); break;
			
			case oClip		: 
				/* usually 1st pict opcode after header is clip to fPicRect */
				/* BUT some picts, a few icons, have this clip offset outside of true drawing rgn !! */
				/* whose bug is this */
				getARgn(&rgnbox, &argn, TRUE);
#if 1
				//fUnclipped= ( ! Nlm_EmptyRect( &rgnbox));
				fUnclipped = (rgnbox.left < rgnbox.right && rgnbox.top < rgnbox.bottom);
#else
				Nlm_ClipRgn(argn); // << leaving unclipped for debugging 
				clipWasSet= TRUE; 
#endif
				break;  
				
			case oOvSize	: getPointAsis( &fOvSize);  break;
			case oOrigin	:	 // oOrigin data is dh:word, dv:word
				{
				short  dh, dv;
				dh= (signed short) getWord();
				dv= (signed short) getWord();
				Nlm_OffsetRect( &fPicFrame, dh, dv);
					// we also must move current point...
				curpt.x -= dh;
				curpt.y -= dv;
				Nlm_GetPen( &pt); // bad for X,?MWin?
				pt.x -= dh; // this one is bad !? mirror of 
				pt.y -= dv;
				Nlm_MoveTo( pt.x, pt.y); 
#ifdef PLIST
				if (gDoPlist) {
					fprintf( gPlist, " dh,v: %d,%d  or.left,top: %d,%d, pt.x,y %d,%d", 
							dh,dv, fPicFrame.left, fPicFrame.top, pt.x, pt.y); 
					}
#endif
				
				}
				break;
				
			case oChExtra	:	(void) getWord(); break;
			case oPnLocHFrac: (void) getWord(); break;
	
			case oBkPat		: skipbytes(8); break;
			case oFillPat	: skipbytes(8); break;
			case oPnPat		: 
#if 1
				for (i=0, curpat=0; i<8; i++) {
					curpat <<= 1;
					curpat += getByte();
					}
#else
				skipbytes(8); /* patterns COULD be supported in NCBI draw w/ some extra code */
#endif
				break;
			case oBkPixPat	: read_pattern(); break;
			case oPnPixPat	: read_pattern(); break;
			case oFillPixPat: read_pattern(); break;

			case oHiliteMode	: break;
			case oHiliteColor	: skipbytes(6); break;
			case oDefHilite		: break;
			case oOpColor			: getRGBKolor( &fOpcolor); break; 
			case oShortComment: skipbytes(2); break;
			case oLongComment : 
				w= getWord(); /* kind */
			 	w= getWord(); /* size */
			 	skipbytes(w); 
			 	break;


			case oPackBitsRgn:
			case oBitsRgn:
				dorgn= TRUE;
				goto readbits;
			case oPackBitsRect:
			case oBitsRect:	
				dorgn= FALSE;
			readbits:
				{
				word rowbytes= getWord();
				if (rowbytes & 0x8000)  
					drawPixmap( fPictVersion, rowbytes, dorgn);
				else  
					drawBitmap( fPictVersion, rowbytes, dorgn);
				}
				break;
			case oOpcode9A: drawPixmapOp9A( fPictVersion); break;			
				
							
			case oTxFont	: w= getWord(); SetFont(w); break;
			case oTxFace	: b= getByte(); SetFontStyle(b); break;
			case oTxSize	: w= getWord(); SetFontSize(w); break;
			case oTxRatio	: skipbytes(8); break;	
			case oSpExtra	: skipbytes(4); break;
			case oTxMode	:	getPenMode();	break;
					
			case oFontName: /* was res2c */
				w= getWord();
				if (w>3) {
					fLastFontnum= getWord();
					w -= 2;
					b= getByte();
					w -= 1 + b;
					getbytes( fLastFontname, b); 
					fLastFontname[b]= 0;
					}
				if (w>0) skipbytes(w);				
 				break;
			
			
			case oPnSize: 
				getPointAsis( &pt); 
		 		Nlm_WidePen(pt.x); 
		 		fPenshown= (pt.x != 0);
				break;
			case oPnMode: 
				getPenMode();
				break;
			
				/* these are PICT 1 oldstyle colors, not translatable to RGB values !! */
			case oFgColor: 
				getForeColor();
				colored= true; 
				break;

			case oBkColor: 
				getBackColor();
				break;
				
			case oRGBFgCol:
				/* getRGBKolor( &aColor); */
				red= getWord() >> 8; 
				green= getWord() >> 8; 
				blue= getWord() >> 8;
				Nlm_SelectColor(red,green,blue);
				colored= true; 
				break;
			case oRGBBkCol:  
				red= getWord() >> 8; 
				green= getWord() >> 8; 
				blue= getWord() >> 8;
				Nlm_SelectBackColor(red,green,blue);
				break;

			case oLine:
				getPointLocal( &pt); 
				getPointLocal( &pt1);
				if (fPenshown && fUnclipped) Nlm_DrawLine( pt, pt1);
				else Nlm_MoveTo( pt1.x, pt1.y); 
				curpt= pt1;
				break;
			case oLineFrom:	
				getPointLocal( &pt); 
				if (fPenshown && fUnclipped) Nlm_LineTo(pt.x, pt.y);
				else Nlm_MoveTo( pt.x, pt.y); 
				curpt= pt;
				break;
				
			case oShortLine: 
				getPointLocal( &pt); 
				dh= (signed char) getByte();
				dv= (signed char) getByte();
				pt1.x = pt.x + dh;
				pt1.y = pt.y + dv;
#if 0
				if (fPenshown && fUnclipped) {
					Nlm_GetPen( &pt0);
					Nlm_DrawLine( pt, pt1);
					Nlm_MoveTo( pt0.x, pt0.y); 
					}
#else
				if (fPenshown && fUnclipped) Nlm_DrawLine( pt, pt1);
				else Nlm_MoveTo( pt1.x, pt1.y); 
				//curpt= pt1;  				// !! can't move here or DH/DVText is screwed !!!!
#endif
				break;
				
			case oShortLineFrom:
				dh= (signed char) getByte();
				dv= (signed char) getByte();
				Nlm_GetPen( &pt);  // bad for X,?MWin?
				pt1.x= pt.x + dh;
				pt1.y= pt.y + dv;
#if 0
				if (fPenshown && fUnclipped) {
					Nlm_LineTo( pt1.x, pt1.y);
					Nlm_MoveTo( pt.x, pt.y); 
					}
#else
				if (fPenshown && fUnclipped) Nlm_LineTo(pt1.x, pt1.y);
				else Nlm_MoveTo( pt1.x, pt1.y); 
				//curpt= pt1;// !! can't move here or DH/DVText is screwed !!!!
#endif
				break;

 				
			case or2e_mov:  /* was r2e -- ?? a moveto??  */
				w= getWord();
				if (w>0) skipbytes(w);
				break;

			case oLongText:
				getPointLocal( &pt); 
				curpt= pt;
				goto caseDrawString;				
			case oDHDVText:
			case oDVText:
			case oDHText:
#if 1
				pt= curpt;
#else
				Nlm_GetPen( &pt);
#endif
				dh= getByte();
				if (opcode == oDHDVText){
					pt.x += dh;
					dv= getByte();
					pt.y += dv;
					}
				else if (opcode == oDVText) pt.y += dh;
				else pt.x += dh;
			caseDrawString:
				Nlm_MoveTo( pt.x, pt.y);
				b= getByte();
				getbytes( buf, b); 
				buf[b]= 0;
#ifdef PLIST
				if (gDoPlist) fprintf( gPlist, " x,y: %d,%d",pt.x, pt.y); 
				if (gDoPlist) fprintf( gPlist, " '%s'",buf); 
#endif
				if (fUnclipped) Nlm_PaintString( buf);
				Nlm_MoveTo( pt.x, pt.y); // Nlm_PaintString moves to pt.x + stringlen(buf)
				curpt= pt;
				break;	
	
			
			/* fill...() are disabled until we get setPattern working... */
			case opaintRect:  	
				if (!fPenshown && !colored && curpat == 0) opcode= oeraseRect; // quick hack to fix null patterns
			case oframeRect:  	
			case oeraseRect:   	
			case oinvertRect: 	
			case ofillRect:  	
					handleRectOpcode( opcode);
					break;
				
			case opaintRRect:  	
				if (!fPenshown && !colored && curpat == 0) opcode= oeraseRRect; // quick hack to fix null patterns
			case oframeRRect:  
			case oeraseRRect:  	
			case oinvertRRect: 
			case ofillRRect:  
					handleRectOpcode( opcode);
					break;

			case opaintOval:  
				if (!fPenshown && !colored && curpat == 0) opcode= oeraseOval; // quick hack to fix null patterns
			case oframeOval:  	
			case oeraseOval: 
			case oinvertOval: 	
			case ofillOval:  
					handleRectOpcode( opcode);
					break;

			case opaintArc:  	
				if (!fPenshown && !colored && curpat == 0) opcode= oeraseArc; // quick hack to fix null patterns
			case oframeArc:   		
			case oeraseArc:  		
			case oinvertArc:  	
			case ofillArc:  
					handleRectOpcode( opcode);
					break;


			case opaintSameRect:  	
				if (!fPenshown && !colored && curpat == 0) opcode= oeraseSameRect; // quick hack to fix null patterns
			case oframeSameRect:  	
			case oeraseSameRect:   	
			case oinvertSameRect:  	
			case ofillSameRect: 
					handleSameRectOpcode(opcode);
					break;

			case opaintSameRRect:  	
				if (!fPenshown && !colored && curpat == 0) opcode= oeraseSameRRect; // quick hack to fix null patterns
			case oframeSameRRect:  
			case oeraseSameRRect:   
			case oinvertSameRRect:  	
			case ofillSameRRect:  	
					handleSameRectOpcode(opcode);
					break;

			case opaintSameOval:  	
				if (!fPenshown && !colored && curpat == 0) opcode= oeraseSameOval; // quick hack to fix null patterns
			case oframeSameOval:  	
			case oeraseSameOval:  	
			case oinvertSameOval:		
			case ofillSameOval:  
					handleSameRectOpcode(opcode);
					break;

			case opaintSameArc:  	
				if (!fPenshown && !colored && curpat == 0) opcode= oeraseSameArc; // quick hack to fix null patterns
			case oframeSameArc: 	
			case oeraseSameArc:  	
			case oinvertSameArc:  		
			case ofillSameArc: 
					handleSameRectOpcode(opcode);
					break;

			case oframePoly:  getAPoly(&polybox, &npoly, &apoly); if (fPenshown && fUnclipped) Nlm_FramePoly( npoly, apoly); break;		
			case oerasePoly:  getAPoly(&polybox, &npoly, &apoly); if (fUnclipped) Nlm_ErasePoly( npoly, apoly); break;		
			case opaintPoly:  getAPoly(&polybox, &npoly, &apoly); 
					if (fUnclipped) {
						 if (!fPenshown && !colored && curpat == 0) Nlm_ErasePoly( npoly, apoly);
						else Nlm_PaintPoly( npoly, apoly); 
						}
					break;		
			case oinvertPoly: getAPoly(&polybox, &npoly, &apoly); if (fUnclipped) Nlm_InvertPoly( npoly, apoly); break;		
			case ofillPoly:   getAPoly(&polybox, &npoly, &apoly); /* setPattern();  Nlm_PaintPoly( npoly, apoly); */ break;		

			case oframeSamePoly:  if (fPenshown && fUnclipped) Nlm_FramePoly( npoly, apoly); break;		
			case oeraseSamePoly:  if (fUnclipped) Nlm_ErasePoly( npoly, apoly); break;		
			case opaintSamePoly:  
					if (fUnclipped) {
						if (!fPenshown && !colored && curpat == 0) Nlm_ErasePoly( npoly, apoly);
						else Nlm_PaintPoly( npoly, apoly); 
						}
					 break;	
			case oinvertSamePoly: if (fUnclipped) Nlm_InvertPoly( npoly, apoly); break;		
			case ofillSamePoly: /* setPattern();  Nlm_PaintPoly( npoly, apoly); */  break;		

			case oframeRgn:  getARgn(&rgnbox, &argn, FALSE); if (fPenshown && fUnclipped) Nlm_FrameRgn( argn); break;		
			case oeraseRgn:  getARgn(&rgnbox, &argn, FALSE); if (fUnclipped) Nlm_EraseRgn( argn); break;		
			case opaintRgn:  getARgn(&rgnbox, &argn, FALSE); 
				if (fUnclipped) {
					if (!fPenshown && !colored && curpat == 0) Nlm_EraseRgn(argn);
					else Nlm_PaintRgn( argn);
					}
				break;		
			case oinvertRgn: getARgn(&rgnbox, &argn, FALSE); if (fUnclipped) Nlm_InvertRgn( argn); break;		
			case ofillRgn:   getARgn(&rgnbox, &argn, FALSE); /* setPattern(); Nlm_PaintRgn( argn); */ break;		

			case oframeSameRgn:  if (fPenshown && fUnclipped) Nlm_FrameRgn( argn); break;		
			case oeraseSameRgn:  if (fUnclipped) Nlm_EraseRgn( argn); break;		
			case opaintSameRgn:  
				if (fUnclipped) {
					if (!fPenshown && !colored && curpat == 0) Nlm_EraseRgn(argn);
					else Nlm_PaintRgn( argn);
					}
				break;		
			case oinvertSameRgn: if (fUnclipped) Nlm_InvertRgn( argn); break;		
			case ofillSameRgn: /* setPattern(); Nlm_PaintRgn( argn); */  break;		

			case r17: case r18: case r19: break;				
			case r24: case r25: case r26: case r27:		skipbytes( getWord()); break;
			case r2d: case r2f: skipbytes( getWord()); break;
			case r35: case r36: case r37: skipbytes( 8); break;
			case r3d: case r3e: case r3f: break;
			case r45: case r46: case r47: skipbytes( 8); break;
			case r4d: case r4e: case r4f: break;
			case r55: case r56: case r57: skipbytes( 8); break;
			case r5d: case r5e: case r5f: break;
			case r65: case r66: case r67: skipbytes( 12); break;
			case r6d: case r6e: case r6f: skipbytes( 4); break;
			case r75: case r76: case r77: skipPolyOrRegion(); break;
			case r7d: case r7e: case r7f: break;
			case r85: case r86: case r87: skipPolyOrRegion(); break;
			case r8d: case r8e: case r8f: break;
			case r92: case r93: case r94: 
			case r95: case r96: case r97:	skipbytes( getWord()); break;
			case r9b: case r9c: case r9d: case r9e: case r9f:	skipbytes( getWord()); break;
				
			default : {
				if (opcode >= 0xa2 && opcode <= 0xaf) skipbytes( getWord());
				else if (opcode >= 0x0b0 && opcode <= 0x0cf) ; 
				else if (opcode >= 0x0d0 && opcode <= 0x0fe) 	skipbytes( getLong());
				else if (opcode >= 0x100 && opcode <= 0x7fff) skipbytes( (opcode >> 7) & 255);
				else if (opcode >= 0x8000 && opcode <= 0x80ff) ; 
 				else if (opcode >= 0x8100 && opcode <= 0xffff) skipbytes( getLong());
				else 
					Nlm_Message(MSG_FATAL,"drawpict: don't know opcode %x", opcode);
				}

			}
		}  
		
}


