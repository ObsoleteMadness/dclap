// DUtil.h
// d.g.gilbert

#ifndef _DUTIL_
#define _DUTIL_

#include "Dvibrant.h"
#include "DObject.h"
#include "DWindow.h"

class DFile;

class DCursor : public DObject
{	
public:
	void arrow() { Nlm_ArrowCursor(); }
	void cross() { Nlm_CrossCursor(); }
	void ibeam() { Nlm_IBeamCursor(); }
	void plus()  { Nlm_PlusCursor(); }
	void watch() { Nlm_WatchCursor(); }
};


class	DKeys	: public	DObject
{
public:
	char	current()		{ return Nlm_currentKey; }
	Boolean	shift()		{ return Nlm_shftKey; }
	Boolean	control()	{ return Nlm_ctrlKey; }
	Boolean	option()	{ return Nlm_optKey; }
	Boolean	command()	{ return Nlm_cmmdKey; }
};


	// use this as a secondary parent for DView classes needing key handling
	// vibrant makes this available...
class DKeyCallback
{
public:
	DKeyCallback();
	virtual ~DKeyCallback();
	virtual void SetKeyHandler(DKeyCallback* handler);
  virtual void ProcessKey( char c) = 0;
};



class	DMouse	: public	DObject
{
public:
	void mouseat(Nlm_PoinT& pt) {
		Nlm_MousePosition(&pt);
		}
	void mouseat(short& x, short& y) {
		Nlm_PoinT pt;
		Nlm_MousePosition(&pt);
		x= pt.x; y= pt.y;
		}
	Boolean mousedown() 	{ return Nlm_MouseButton(); }
	Boolean	doubleclick() { return Nlm_dblClick; }
};



class	DPrefManager	: public DObject
{
public:
	virtual char* GetAppPref(char* type, char* section, char* appname, char* defaultvalue = NULL,
														char* prefvalue= NULL, ulong maxsize = 256);
	virtual char* GetPref(char* type, char* section, char* defaultvalue = NULL,
												char* prefvalue= NULL, ulong maxsize = 256);
	virtual Boolean SetAppPref(char* prefvalue, char* type, char* section, char* appname);
	virtual Boolean SetPref(char* prefvalue, char* type, char* section);
	virtual Boolean RemovePrefSection(char* section, char* appname);
	virtual char* GetAppPrefSection(char* section, ulong& sectlen, char* appname, char* defaultvalue = NULL,
														char* prefvalue= NULL, ulong maxsize = 4048);
	virtual char* GetPrefSection(char* section, ulong& sectlen, char* defaultvalue = NULL,
												char* prefvalue= NULL, ulong maxsize = 4048);
	virtual char** GetPrefFileLines(const char* prefFile, short& nlines); 
};



class	DPrintManager	: public DObject
{
public:
	Nlm_RecT fPageRect;
	
	DPrintManager();
	DWindow* StartPrinting();
	void DonePrinting(DWindow* savedWindow);
	Boolean StartPage();
	Boolean DonePage();
	Boolean PrintRect(Nlm_RecT &rpt);
	short	PageHeight(Boolean onScreen = TRUE);
	short PageWidth(Boolean onScreen = TRUE);
};


class DString : public DObject
{
	char*	fStr;
public:
	DString() { fStr= NULL; }
	DString(const char* s) { fStr= StrDup((char*) s); }
	virtual ~DString() { if (fStr) MemFree(fStr); }
	virtual const char* Get() { return fStr; }
	virtual void Set(const char* s) { if (fStr) MemFree(fStr); fStr= StrDup((char*) s); }
};


class	DUtil	: public	DObject
{
	char	fVers[128];
public:
	DUtil() { 
		Nlm_Version(fVers, 128); 
		}
		
	virtual const char* version() const
		{ return (char*) fVers; 
		}
	long time()		
		{ return Nlm_ComputerTime(); 
		}
	void screensize( short& width, short& height) 
		{
		width = Nlm_screenRect.right - Nlm_screenRect.left;
		height= Nlm_screenRect.bottom - Nlm_screenRect.top;
		}
};

class DErrorManager	: public DObject
{
	short	fErract;
	short	fErrlog;
public:
	void TurnOff();
	void TurnOn();
};

class	DClapGlobals	: public	DObject
{
public:
	DClapGlobals();
	virtual ~DClapGlobals();
};

extern	DCursor*	gCursor;
extern	DMouse*		gMouse;
extern	DKeys*		gKeys;
extern	DUtil*		gUtil;
extern	DPrintManager*	gPrintManager;
extern	DErrorManager*	gErrorManager;
extern	DPrefManager*		gPrefManager;
extern  DKeyCallback* 	gKeyHandlerObject;


extern Nlm_FonT gTextFont; 
extern char		*	gTextFontName;
extern short  	gTextFontSize;
extern short		gTextTabStops;



#endif
