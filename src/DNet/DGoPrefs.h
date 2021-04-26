// DGopherPrefs.h
// d.g.gilbert


#ifndef _DGOPHERPREFS_
#define _DGOPHERPREFS_


#include <DWindow.h>

class	DMenu;
class DCluster;


class DGoviewPrefs : public DWindow 
{
public:
	DCluster 	* fItemGroup;
	DCluster 	* fTextGroup;
	DMenu			* fGoFontMenu;
	DMenu			* fTextFontMenu;
	Boolean	 		fNeedSave;
	
	DGoviewPrefs();
	virtual ~DGoviewPrefs();
	static void InitGlobals();
	static void SaveGlobals();
	virtual void Initialize();
	virtual void Open();
	virtual void OkayAction();
	virtual Boolean IsMyAction(DTaskMaster* action); 
};


class DGoOtherPrefs : public DWindow 
{
public:
	DCluster * fPlusGroup;
	DCluster * fMiscGroup;
	DCluster * fMapGroup;
	DCluster * fStartGroup;
	DCluster * fNetGroup;
	Boolean	 	 fNeedSave;
	DCluster * fClickerGroup;
	
	DGoOtherPrefs();
	virtual ~DGoOtherPrefs();
	static void InitGlobals();
	static void SaveGlobals();
	virtual void Initialize();
	virtual void Open();
	virtual void OkayAction();
	virtual Boolean IsMyAction(DTaskMaster* action); 
};

		
		
extern DGoviewPrefs* gGoviewPrefs;
extern DGoOtherPrefs* gGoOtherPrefs;

extern	Boolean gShowKind, gShowPath, gShowHost, gShowPort, 
				gShowDate, gShowSize, gShowPlus, gShowAdmin,
				gShowURL, gShowAbstract, gShowExtras,
				gAllowLinkEdits, gWantFonts, gSameWindow;
 
extern Nlm_FonT gGoviewFont;
extern short  gGoviewFontSize;
extern char	*gGoviewFontName;

#endif
