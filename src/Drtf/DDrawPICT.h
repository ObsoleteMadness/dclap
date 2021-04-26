// DDrawPICT.h

#ifndef _DDRAWPICT_
#define _DDRAWPICT_


#include <Dvibrant.h>


struct PictHeader {
	short picsize;  // real size may be > 32k
	short top;
	short left;
	short bottom;
	short right;
	char  data[4]; // lots of data here (4 is dummy val)
};

struct RGBkolor {
	unsigned short red;
	unsigned short green;
	unsigned short blue;
};

struct qRect {
	unsigned short top;
	unsigned short left;
	unsigned short bottom;
	unsigned short right;
};

struct PIXMap {
	struct qRect Bounds;  
	unsigned short pmVersion;
	unsigned short packType;
	unsigned long packSize;
	unsigned long hRes;
	unsigned long vRes;
	unsigned short pixelType;
	unsigned short pixelSize;
	unsigned short cmpCount;
	unsigned short cmpSize;
	unsigned long planeBytes;
	unsigned long pmTable;
	unsigned long pmReserved;
};

typedef PIXMap *PIXMapPtr;

struct KolorSpec {
	unsigned short value;
	RGBkolor 	rgb;
};

struct KolorTable {
	unsigned long		ctSeed;
	unsigned short	ctFlags;
	unsigned short	ctSize;
	KolorSpec	* ctTable;
};
	


class DDrawPict 
{
public:
			// DRichStyles uses same kinds !
	enum kinds {
		kPictOther= 0,
	 	kPictMac= 1,
	 	kPictWinMeta=2,
	 	kPictPMMeta=3,
	 	kPictDIBits=4,
	 	kPictWinBits=5,
	 	kPictGIF=6
		};
	enum { kXColorTableFlag = 1024 };
		
	static  void Pixmap2Xcolors( KolorTable& kolorTab, unsigned char* bits, ulong bitsize,
																Nlm_Boolean returnXtabInBits = false);
	static	void Cleanup(); // call at program termination, or when done using DrawPict stuff

	DDrawPict();
	virtual ~DDrawPict();
	void*   IsPICT( void* pictdata, ulong datasize, 
									Nlm_Boolean dontTestForHeader = false);
	virtual void Draw( Nlm_RectPtr r, void* pictdata, ulong picsize, short kind);
	virtual void GetFrame( Nlm_RectPtr r, void* pictdata, ulong picsize, short kind);
	
protected:
	short			fPictVersion;
	RGBkolor 	fOpcolor;
	Nlm_Boolean		fVerbose, fPenshown, fUnclipped;
	unsigned char		* fPict;
	ulong		fPicsize, fAlign;
	Nlm_RecT	fPicRect, fPicFrame, fTheRect;
	Nlm_PoinT	fOvSize;
	Nlm_FonT	fFont;
	char 		*	fFontname,  * fFontfamily;
	short			fFontsize, fFontstyle;
	char			fLastFontname[257];
	short			fLastFontnum;
	Nlm_Boolean		fNeedInstallFont;
	
	unsigned char	getByte();
	unsigned short 	getWord();
	unsigned long getLong();
	void 	skipbytes( unsigned short count);
	void 	getbytes( char* buf, long bufsize);
	unsigned short 	getOpcode();
	void 	getRGBKolor( RGBkolor* acolor);
	void	getForeColor();
	void	getBackColor();
	void	getPenMode();
	void 	getPointAsis( Nlm_PointPtr apt);
	void 	getPointLocal( Nlm_PointPtr apt);
	void 	getRectLocal( Nlm_RectPtr arect);
	void 	getRectAsis( Nlm_RectPtr arect);
	void 	skipPolyOrRegion();

	void	handleRectOpcode( unsigned short opcode);
	void	handleSameRectOpcode( unsigned short opcode);

	unsigned char* expandbuf( unsigned char* buf, short* len, short bitsPerPixel);
	unsigned char* unpackbits( Nlm_RectPtr bounds, unsigned short rowBytes, 
															short pixelSize, ulong& unpackSize);
	void unpackSmallLine( long irow, unsigned char* linebuf, unsigned char* bits, unsigned char* bitsend, 
												unsigned short rowBytes, short pixelSize);
	void unpackBigLine( long irow, unsigned char* linebuf, unsigned char* bits, unsigned char* bitsend, 
									   unsigned short rowBytes, short pixelSize, short pkpixsize);
												 
	void 	read_pixmap( PIXMapPtr p, unsigned short* rowBytes);
	KolorTable read_colour_table();
	void 	read_pattern();
	void 	drawPixmap( short version, unsigned short rowBytes, short isregion );
	void 	drawBitmap( short version, short rowBytes, Nlm_Boolean isregion);
	void	drawPixmapOp9A( short version );
	void 	InstallFont();
	void 	SetFont( unsigned short fontnum);
	void 	SetFontStyle( unsigned char fstyle);
	void 	SetFontSize( unsigned short fsize);
	void 	getAArc( Nlm_RectPtr arec, Nlm_PointPtr pt, Nlm_PointPtr pt1);
	void 	getAPoly( Nlm_RectPtr polybox, short* polycount, Nlm_PointPtr* thePoly);
	void 	getARgn( Nlm_RectPtr rgnbox, Nlm_RegioN* theRgn, Nlm_Boolean forclip);
	void  MakeXByteFlip();


};


#endif

