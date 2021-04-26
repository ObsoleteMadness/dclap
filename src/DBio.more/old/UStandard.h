// UStandard -- 	selection of standard pas library 
//	MPW/MacApp.3c++ version 
//	© 1989-92 d.g. gilbert

#ifndef __USTANDARD__
#define __USTANDARD__


#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __GEOMETRY__
#include <Geometry.h>
#endif

#ifndef __QUICKDRAW__
#include <QuickDraw.h>
#endif

#ifndef __MEMORY__
#include <Memory.h>
#endif

   
struct	hcFile	{
		Handle	h; 
		long		len;
		Boolean	writing;
};
	 
//#define min(a,b)      (a<b)?a:b
//#define max(a,b)      (a>b)?a:b
#define is_upper(c)	('A'<=(c) && (c)<='Z')
#define is_lower(c)	('a'<=(c) && (c)<='z')
#define to_lower(c)	((char)(is_upper(c) ? (c)+' ' : (c)))
#define to_upper(c)	((char)(is_lower(c) ? (c)-' ' : (c)))


inline long max(long a, long b)
{
	return a > b ? a : b;
}

inline long min(long a, long b)
{
	return a < b ? a : b;
}

#ifdef __cplusplus
extern "C"
{
#endif

pascal short cHeight();
pascal short	cWidth();

pascal Boolean stopKey();  
pascal Boolean capsLockIsDown();
pascal Boolean shiftKeyIsDown();
pascal Boolean cmdKeyIsDown();
pascal Boolean optionKeyIsDown();
//pascal Boolean controlKeyIsDown();

pascal Handle dupHandle(Handle h);
pascal RgnHandle getClipRgn();
pascal void setClipRgn( RgnHandle& r);
pascal void getTeStyle( TextStyle& tStyle);
pascal void CenterSFPut( Point& where);
pascal void DrawIconls( WindowPtr aWindow, CRect& aRect, short icnID, short icnSize);

//pascal int Strcasecmp(const char *a, const char *b); 	
//pascal int Strncasecmp(const char *a, const char *b, long maxn); 

//pascal Str63 osType2Str( OSType os);
//pascal OSType Str2osType(Str63 s);
pascal void StripChar( char dropC, Handle h);
pascal void TranslateChar( char fromC, char toC, Handle h);
pascal Handle Str2Handle( CStr255 s);
pascal CStr255 Handle2Str(Handle h);
pascal void CleanMenuName(CStr255& aName);

pascal void hcOpen( hcFile& f, Handle h);
pascal void hcRewrite( hcFile& f);
pascal void hcClose( hcFile& f);
pascal void hcRewind( hcFile& f);
pascal Boolean hcEof( hcFile& f);
pascal Boolean hcEoln( hcFile& f);
pascal char hcRead( hcFile& f);
pascal long hcReadLong( hcFile& f);
pascal CStr255 hcReadTo( hcFile& f, char tochar);
pascal Handle hcReadToHand( hcFile& f, char tochar);
pascal CStr255 hcReadLn( hcFile& f);
pascal void hcWrite( hcFile& f,  CStr255 s);
pascal void hcWriteHand( hcFile& f, Handle h);
pascal void hcWriteChar( hcFile& f, char ch);
pascal void hcWriteInt( hcFile& f, long n);
pascal void hcWriteln( hcFile& f, CStr255 s);

pascal Boolean TrapAvailable( unsigned short theTrap);
pascal Boolean GestaltFlag( OSType attribute, short optionFlag);

pascal extern Boolean gHasGestalt;

#ifdef __cplusplus
}
#endif

#endif


