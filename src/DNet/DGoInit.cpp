// DGopherInit.cp
// d.g.gilbert


#include <ncbi.h>
#include <dgg.h>
#include <Dvibrant.h>

#include "DTCP.h"
#include "DGopher.h"
#include "DGoList.h"
#include "DGoPlus.h"
#include "DGoClasses.h"
#include "DGoPrefs.h"
#include <DApplication.h>
#include <DDialogText.h>
#include "DUtil.h"
#include <DIconLib.h>

#include "DGoInit.h"

 


enum GomapStatus {
	kMapNever, kMapDefault, kMapAlways 
	};
	
enum GoMapDataKind { 
	kKindData=110, kServerTypeData, kServerSuffixData, kLocalData,
	kdKindData, kdServerTypeData, kdServerSuffixData, kdLocalData,
	kDontCheckDups = kdKindData
	};

enum GopherHandlerKind { 
	kNoHandler = 0, kInternalHandler, kExternalHandler
	};
	
enum GoMapStatus {
	kNochange, kChanged, kAdded, kDeleted
	};
	
enum GopherInternalHandlers {
	kErrHandler, kTextHandler, kFolderHandler, kFolderPlusHandler, kQueryHandler, 
	kCSOHandler, kWhoisHandler, kFingerHandler, kNoteHandler, 
	kPictHandler, kRTFHandler, kHTMLHandler, kMailHandler, kHTTPHandler, kImageHandler,
	kGifHandler,
  kNetTextHandler, kNetRTFHandler, kNetPictHandler, kNetGifHandler,
	kMaxInternalHandler
 	};	
	
	// this list *must* match GopherInternalHandlers list
	// in most cases, we'd just do kTextHandler = 'text', but that damn 16bitter won't let us
	// also config list must use 4char ids, including spaces 

		// !! this is a DAMN weak point -- need to remember to add here when
		// add new internal types 

Local char* gInternalHandlerIDs[kMaxInternalHandler+1] = {
	"err ", "text", "goph", "gopl", "ques", 
	"cso ", "who ", "fing", "note", 
	"pict", "rtf ", "html", "mail", "http", "imag", "gif ",
  "ntxt", "nrtf", "npic", "ngif",
	NULL
	};



Boolean			gNeedTypeChange = false;	// global used in DGopher & DGoList

DGopherMapList*		gGopherMap = NULL;

#if 0
Local DList* gNetKinds = NULL;
Local DList* gServerSuffixes = NULL;
Local DList* gServerType = NULL;
#endif


Local char*	gYesNo[2]; // = { "No", "Yes" };


Local	char* gDefaultTypes = 
"goph=application/gopher-menu internet/gopher-menu\n" 
"gopl=application gopher+-menu internet/gopher+-menu\n"
"cso=internet/cso\n"
"err=internet/error\n"
"ques=internet/gopher-query\n"
"who=internet/whois\n"
"fing=internet/finger\n"
"note=internet/gopher-note\n"
"mail=internet/mail\n"
"ftp =internet/ftp\n"
"html=text/html internet/html\n"
"text=text/plain text/-\n"
"file=file/-\n"
"imag=image/-\n"
"gif=image/gif\n"
"moov=video/-\n"
"sond=audio/-\n"
"teln=terminal/telnet\n"
"tibm=terminal/tn3270\n"
;

Local	char* gDefaultServerTypes = 
"text=	0	0	\n"
"goph=	1	1	\n"
"cso =	2	2 \n"
"err =	3	3	\n"
"hqx =	4	4 \n"
"file=	5	9 \n"
"file=	6	9 \n"
"ques=	7	7 \n"
"teln=	8	8 \n"
"file=	9	9 \n"
"sond=	s	s \n"
"who	=	w	w \n"
"imag=	I	I \n"
"gif =	g	I \n"
"note=	i	i \n"
"tibm=	T	T \n"
"html=	h	h \n"
"moov=	;	; \n"
;

Local	char* gDefaultSuffixes = 
"text=	.txt	.text	.doc\n"
"post=	.ps\n"
"rtf =	.rtf\n"
"jpeg=	.jpg	.jpeg	.jfif\n"
"gif =	.gif\n"
"hqx =	.hqx\n"
;


//[gpmap-local]
//#type=	precedence	handling	MacType	MacCreator	HandlerApp
// codewarrior can't deal w/ an #ifdef inside of a char data declaration 

#ifdef WIN_MAC
Local	char* gDefaultLocal = 
"gopl=	1	internal\n"
"goph=	2	internal\n"
"err=	1	internal\n"
"ques=	1	internal\n"
"note=	1 internal\n"
"mail=	0	none\n"
"ftp=	0	none\n"
"cso=	0	none\n"
"who=	1 internal\n"
"fing=	1	internal\n"
"text=	1	internal	TEXT/ttxt	TeachText\n"
;

//"html=	1	internal  TEXT/MWEB	MacWeb\n"
//"pict=	1	internal	PICT/ttxt	TeachText\n"
#else

Local	char* gDefaultLocal = 
"gopl=	1	internal\n"
"goph=	2	internal\n"
"err=	1	internal\n"
"ques=	1	internal\n"
"note=	1 internal\n"
"mail=	0	none\n"
"ftp=	0	none\n"
"cso=	0	none\n"
"who=	1 internal\n"
"fing=	1	internal\n"
"text=	1	internal	.txt\n"
;

//"html=	1	internal  .html\n"
//"pict=	1	internal	.pict\n"
#endif


#if 0
class DMapName : public DObject
{
public:
	long 		fId;
	char	* fName;
	DMapName( long localid, char* name) {
		fId= localid; 
		fName= StrDup( name);
		}
	~DMapName() { MemFree( fName); }
};


// static
short dgiCompareMapNames( DMapName* a, DMapName* b)
{
#if 0
	return ((long)a - (long)b);
#else
	if (a->fId == b->fId) 
		return Nlm_StringCmp(a->fName, b->fName);
	else 
		return a->fId - b->fId;
#endif
} 
#endif



class DGodocKindList : public DObject
{
public:
	enum	{ kNodata, kNamedata, kListdata };
	char	fDatatype;
	void	* fData;
	
	DGodocKindList() { fDatatype= kNodata; fData= NULL; }
	DGodocKindList( char* name, Boolean lowercase = false) {
		fDatatype= kNodata; fData= NULL;
		AddName( name, lowercase);
		}
	~DGodocKindList();
	void AddName( char* name, Boolean lowercase = false);
	const char* GetName();
	void GetAllNames(char* buf, ulong maxbuf);
	Boolean Match(char* name, Boolean ignorecase = false);
};

DGodocKindList::~DGodocKindList() 
{ 
	if (fData) {
		if (fDatatype == kListdata) {
			DList* dl= (DList*) fData;
			long i, n= dl->GetSize();
			for (i=0; i<n; i++) { DString* ds= (DString*) dl->At(i); delete ds; }
			delete dl;
			} 
		else MemFree( fData);  
		}
}

const char* DGodocKindList::GetName()
{
	switch (fDatatype) {
		case kNamedata: 
			return (char*) fData;
		case kListdata: {
			DString* ds= (DString*) ((DList*) fData)->First();
			if (ds) return ds->Get();
			else return NULL;
			}
		default:
			return NULL;
		}
}


void DGodocKindList::GetAllNames(char* buf, ulong maxbuf)
{
	if (buf && maxbuf) 
	 switch (fDatatype) {
		case kNamedata: 
			StrNCpy( buf, (char*) fData, maxbuf);
			break;
			
		case kListdata: {
			long i, n= ((DList*)fData)->GetSize();
			buf[0]= 0;
			for (i= 0; i<n; i++) {
				DString* ds= (DString*) ((DList*) fData)->At(i);
				StrNCat( buf, ds->Get(), maxbuf);
				StrNCat( buf, " ", maxbuf);
				}
			break;
			}
			
		default:
			buf[0]= 0;
			break;
		}
}
	
	
Boolean DGodocKindList::Match(char* name, Boolean ignorecase)
{
	switch (fDatatype) {
		case kNamedata: 
			if (ignorecase) 
				return (Nlm_StringICmp( (char*) fData, name) == 0);
			else 
				return (Nlm_StringCmp( (char*) fData, name) == 0);
				
		case kListdata: 
			if (name == NULL) return false;
			else {
				Boolean match;
				long i, n= ((DList*)fData)->GetSize();
				for (i= 0; i<n; i++) {
					DString* ds= (DString*) ((DList*) fData)->At(i);
					if (ignorecase) 
						match= (Nlm_StringICmp( (char*) ds->Get(), name) == 0);
					else 
						match= (Nlm_StringCmp( (char*) ds->Get(), name) == 0);
					if (match) return true;
					}
				return false;
				}
			
		default:
			return (name == NULL);
		}
}


void DGodocKindList::AddName( char* name, Boolean lowercase)
{
	DString* ds;
	if (!name) return;
	else if (!fData || fDatatype==kNodata) {
		fDatatype= kNamedata;
		fData= StrDup( name);
		if (lowercase) StrLocase( (char*)fData);
		return;
		}
	else if (fDatatype == kNamedata) {
		ds= new DString((char*)fData);
		MemFree(fData);
		fDatatype= kListdata;
		fData= new DList();
		((DList*) fData)->InsertLast(ds);
		//goto StuffList;
		}
		
	if (fDatatype == kListdata) {
//StuffList:
		DList* dl= (DList*) fData;
		ds= new DString(name);
 		if (lowercase) StrLocase( (char*)ds->Get());
		long i, n= dl->GetSize();
		Boolean match= false;
		for (i= 0; i<n && !match; i++) {
			DString* d2= (DString*) dl->At(i);
			match= (Nlm_StringCmp( (char*) d2->Get(), (char*)ds->Get()) == 0);
			}
		if (!match) dl->InsertLast(ds);
		}
}



		// local dialog...
		
class DGoMapEditView : public DCluster {		
public:
	DGopherMap	*	fMapper;
	short		fForm;
	Boolean fInstallKind;
	
	DGoMapEditView(long id, DView* itsSuperior, short itsForm, DGopherMap* itsMapper,
			short width, short height, Boolean hidden = false, char* title = NULL);
	virtual ~DGoMapEditView();
	virtual void InstallControls();
	virtual void Answers();
};


class DGoMapEditDlog : public DWindow 
{
public:
	enum GoMapEditForms { kFormLocal=1, kFormServerSuffixes, kFormServerLocalMap, kFormNew };
	DGopherMap			* fMapper, * fNewMapper;
	DGoMapEditView	* fView;
	char 	* fOtherKinds;
	short		fForm;
 	
	DGoMapEditDlog(DGopherMap* itsMapper, short itsForm, 
				char* itsKind = "text/plain");
	virtual ~DGoMapEditDlog();
	virtual Boolean IsMyAction(DTaskMaster* action); 
	virtual void OkayAction();
	virtual void Open();
};







void NewGopherMap() 
{
#if 0
	long i;
	if (gNetKinds) {
		for (i=0; i<gNetKinds->GetSize(); i++)
			delete (DMapName*)gNetKinds->At(i);
		delete gNetKinds;
		}
	gNetKinds= new DList(dgiCompareMapNames);

	if (gServerType) {
		for (i=0; i<gServerType->GetSize(); i++)
			delete (DMapName*)gServerType->At(i);
		delete gServerType;
		}
	gServerType= new DList(dgiCompareMapNames);
	
	if (gServerSuffixes) {
		for (i=0; i<gServerSuffixes->GetSize(); i++)
			delete (DMapName*)gServerSuffixes->At(i);
		delete gServerSuffixes;
		}
	gServerSuffixes= new DList(dgiCompareMapNames);
#endif

	if (gGopherMap) {
		gGopherMap->FreeAllObjects();
		delete gGopherMap;
		}
	gGopherMap= new DGopherMapList();
}


void ReadGoMapPrefSection(char* section, short dataKind, char* defaultData) 
{
	char *sectnames, *sectend, *params, *name, *ep;
	ulong sectlen;
 
	sectnames= gApplication->GetPrefSection(section, sectlen, NULL);  
	if (sectnames) {
		sectend= sectnames + sectlen;
		name= sectnames;
		while (name && name < sectend) {
			ep= StrChr(name,'\0');
			params= gApplication->GetPref( name, section, NULL);
			gGopherMap->ReadParams( name, params, dataKind);  
			MemFree(params);
			name= ep+1;  
			}
		MemFree(sectnames);
		}
		
	else if (defaultData) {
		sectend= defaultData + StrLen(defaultData);
		name= defaultData;
		while (name && name < sectend) {
			ep= StrChr(name,'\n');  
			if (!ep) ep= StrChr(name,'\0');
			params= StrChr(name,'=');
			if (params) params++;
			gGopherMap->ReadParams( name, params, dataKind);  
			name= ep+1;  
			}
		}
}



void StoreGopherIcons()
{
#if 0
enum GopherIconID {
	kTextIcon				= 2200,	// resource fork must have these icon/cicon 
	kFolderIcon			= 2201,
	kPhonebookIcon	= 2202, //CSO phonebook
	kDefaultIcon		= 2203,
	kBinhexIcon			= 2204,
	kUuencodeIcon 	= 2206,
	kIndexIcon 			= 2207,
	kTelnetIcon			= 2208,
	kSoundIcon			= 2212,
	kWhoisPhonebookIcon	= 2213,
	kBinaryIcon			= 2214,
	kImageIcon			= 2215,
	kMovieIcon			= 2216,
	kHTMLIcon				= 2217,
	kNoteIcon				= 2218,
	kFirstIcon			= kTextIcon,
	kLastIcon				= kNoteIcon
};
#endif

unsigned short cTextIcon[] = {
 kTextIcon, 0, 32, 32, 64,
 0x1fff, 0xfc00, 0x1000,  0x600, 0x1000,  0x500, 0x1000,  0x480,
 0x1000,  0x440, 0x1000,  0x420, 0x1000,  0x7f0, 0x101f, 0xf810,
 0x1000,   0x10, 0x107f, 0xf810, 0x1000,   0x10, 0x107f, 0xf810,
 0x1000,   0x10, 0x107f, 0xf810, 0x1000,   0x10, 0x107f, 0xf810,
 0x1000,   0x10, 0x101f, 0xf810, 0x1000,   0x10, 0x107f, 0xf810,
 0x1000,   0x10, 0x107f, 0xf810, 0x1000,   0x10, 0x107f, 0xf810,
 0x1000,   0x10, 0x107f, 0xf810, 0x1000,   0x10, 0x1000,   0x10,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0x1fff, 0xfff0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cTextIcon); // data is stored in gIconList

unsigned short cFolderIcon[] = {
 kFolderIcon, 0, 32, 32, 64,
      0,      0,      0,      0,      0,      0,      0,      0,
      0,      0,      0,      0,      0,      0,  0x7f0,      0,
  0x808,      0, 0x1004,      0, 0x2002,      0, 0x4001, 0xfffc,
 0x8000,    0x2, 0x8000,    0x2, 0x8000,    0x2, 0x8000,    0x2,
 0x8000,    0x2, 0x8000,    0x2, 0x8000,    0x2, 0x8000,    0x2,
 0x8000,    0x2, 0x8000,    0x2, 0x8000,    0x2, 0x8000,    0x2,
 0x8000,    0x2, 0x8000,    0x2, 0x8000,    0x2, 0x8000,    0x2,
 0x8000,    0x2, 0x8000,    0x2, 0x8000,    0x2, 0xffff, 0xfffe 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cFolderIcon); 

unsigned short cPhonebookIcon[] = {
 kPhonebookIcon, 0, 32, 32, 64,
      0,      0,      0,      0,      0,      0,      0,      0,
      0,      0,   0x3f, 0xfe00,  0x1ff, 0xffc0,  0x3ff, 0xffe0,
  0x7ec, 0x1ff0,  0x7ef, 0xfbf0,  0x3df, 0xfde0,   0x18,  0xc00,
   0x3a, 0xae00,   0x38,  0xe00,   0x7a, 0xaf00,   0x78,  0xf00,
   0x7f, 0xff00,   0x7f, 0xff00,   0x7f, 0xff00,   0x60,  0x300,
 0x7fff, 0xffff, 0x3fff, 0xfffe, 0x2000,    0x2, 0x3c00,    0x2,
 0x2870,    0x2, 0x2021, 0xc002, 0x2000, 0x8702, 0x2000,  0x21e,
 0x2000,    0xa, 0x2000,    0x2, 0x3fff, 0xfffe, 0x7fff, 0xffff 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cPhonebookIcon); // data is stored in gIconList

unsigned short cSoundIcon[] = {
 kSoundIcon, 0, 29, 29, 54,
      0,      0,      0,  0x300,      0,  0xf00,      0, 0x3f00,
      0, 0xfd00,    0x3, 0xf100, 0xffff, 0xffff, 0xe003, 0x8100,
 0xe002,  0x100, 0xe002,  0x100, 0xe002,  0x100, 0xffff, 0xffff,
 0xe002,  0x100, 0xec02,  0x100, 0xec02,  0x700, 0xe002,  0xf00,
 0xffff, 0xffff, 0xe01e, 0x1f00, 0xec3e, 0x1e00, 0xec3e, 0x1c00,
 0xe03c,      0, 0xffff, 0xffff, 0xe000,      0, 0xe000,      0,
 0xe000,      0, 0xe000,      0, 0xffff, 0xffff 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cSoundIcon);

unsigned short cBinhexIcon[] = {
 kBinhexIcon, 0, 31, 31, 62,
 0x1fff, 0xfc00, 0x1000,  0x600, 0x1000,  0x500, 0x1000,  0x480,
 0x1000,  0x440, 0x1000,  0x420, 0x1652, 0xafb0, 0x155a, 0xa8b0,
 0x155a, 0xa8f0, 0x1756, 0xee50, 0x1556, 0xa8f0, 0x1556, 0xa8b0,
 0x1652, 0xaeb0, 0x1000,   0x10, 0x107e,   0x10, 0x1043,   0x10,
 0x1042, 0x8010, 0x1043, 0xc010, 0x1040, 0x5010, 0x1040, 0x6810,
 0x1040, 0x4410, 0x1040, 0x8210, 0x1041,  0xf10, 0x1042, 0x1190,
 0x1041, 0x3d10, 0x1040, 0x8b90, 0x107f, 0xc790, 0x1000, 0x2990,
 0x1000, 0x1010, 0x1000,   0x10, 0x1fff, 0xfff0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cBinhexIcon);

unsigned short cDefaultIcon[] = {
 kDefaultIcon, 0, 32, 32, 64,
 0x1fff, 0xfc00, 0x1000,  0x600, 0x1000,  0x500, 0x1000,  0x480,
 0x1000,  0x440, 0x1000,  0x420, 0x1000,  0x7f0, 0x1000,   0x10,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0x1fff, 0xfff0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cDefaultIcon);

unsigned short cUuencodeIcon[] = {
 kUuencodeIcon, 0, 31, 31, 62,
 0x1fff, 0xfc00, 0x1000,  0x600, 0x1000,  0x500, 0x1000,  0x480,
 0x1000,  0x440, 0x1000,  0x420, 0x1556, 0x97f0, 0x1554, 0xd490,
 0x1554, 0xd490, 0x1556, 0xf490, 0x1554, 0xb490, 0x1554, 0xb490,
 0x1226, 0x9670, 0x1000,   0x10, 0x107e,   0x10, 0x1043,   0x10,
 0x1042, 0x8010, 0x1043, 0xc010, 0x1040, 0x5010, 0x1040, 0x6810,
 0x1040, 0x4410, 0x1040, 0x8210, 0x1041,  0xf10, 0x1042, 0x1190,
 0x1041, 0x3d10, 0x1040, 0x8b90, 0x107f, 0xc790, 0x1000, 0x2990,
 0x1000, 0x1010, 0x1000,   0x10, 0x1fff, 0xfff0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cUuencodeIcon);

unsigned short cIndexIcon[] = {
 kIndexIcon, 0, 30, 30, 60,
      0,      0,      0,      0, 0x3fff, 0xfffc, 0x7fff, 0xfffc,
 0x7fff, 0xfffc, 0x6000,   0x1c, 0x6000,   0x1c, 0x601f, 0xc01c,
 0x603f, 0xf01c, 0x607f, 0xf81c, 0x60f8, 0xfc1c, 0x60f0, 0x7c1c,
 0x60f0, 0x7c1c, 0x60f0, 0x7c1c, 0x6000, 0xf81c, 0x6001, 0xf01c,
 0x6003, 0xe01c, 0x6007, 0xc01c, 0x6007, 0x801c, 0x6007, 0x801c,
 0x6007, 0x801c, 0x6000,   0x1c, 0x6000,   0x1c, 0x6000,   0x1c,
 0x6007, 0x801c, 0x6007, 0x801c, 0x6007, 0x801c, 0x6000,   0x1c,
 0x6000,   0x18, 0x3fff, 0xfff0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cIndexIcon);

unsigned short cTelnetIcon[] = {
 kTelnetIcon, 0, 32, 32, 64,
 0x3fc0, 0x7f80, 0x2040, 0x4080, 0x2f40, 0x5e80, 0x2f40, 0x5e80,
 0x2040, 0x4080, 0xfff1, 0xffe0, 0xb01f, 0x603e, 0x80f1,  0x3e2,
 0xfff1, 0xffe2,      0,    0x2,   0x7f, 0xf002,   0x55, 0x5002,
   0x55, 0x5002,   0x55, 0x5002,   0x55, 0x5002,   0x55, 0x5002,
   0x55, 0x5002,   0x55, 0x5007,  0x3ff, 0xffff,  0x200,  0x207,
  0x200,  0x202,  0x3ff, 0xfe02,      0,    0x2, 0xfe1f, 0xc3fa,
 0x8210, 0x420a, 0xba17, 0x42ea, 0xba17, 0x42ee, 0xbbf7, 0x7ee8,
 0x8210, 0x4208, 0xfe1f, 0xc3f8, 0x8610, 0x4208, 0xfe1f, 0xc3f8 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cTelnetIcon);

unsigned short cWhoisPhonebookIcon[] = {
 kWhoisPhonebookIcon, 0, 32, 32, 64,
      0,      0,      0,      0,      0,      0,      0,      0,
      0,      0,   0x3f, 0xfe00,  0x1ff, 0xffc0,  0x3ff, 0xffe0,
  0x7ec, 0x1ff0,  0x7ef, 0xfbf0,  0x3df, 0xfde0,   0x18,  0xc00,
   0x3a, 0xae00,   0x38,  0xe00,   0x7a, 0xaf00,   0x78,  0xf00,
   0x7f, 0xff00,   0x7f, 0xff00,   0x7f, 0xff00,   0x60,  0x300,
 0x7fff, 0xffff, 0x3fff, 0xfffe, 0x2000,    0x2, 0x2000,    0x2,
 0x2000,    0x2, 0x2000,    0x2, 0x2000,    0x2, 0x2000,    0x2,
 0x2000,    0x2, 0x2000,    0x2, 0x3fff, 0xfffe, 0x7fff, 0xffff 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cWhoisPhonebookIcon);

unsigned short cBinaryIcon[] = {
 kBinaryIcon, 0, 32, 32, 64,
 0x1fff, 0xfc00, 0x1000,  0x600, 0x1000,  0x500, 0x1000,  0x480,
 0x1000,  0x440, 0x1000,  0x420, 0x1000,  0x7f0, 0x1000,   0x10,
 0x1049, 0x2410, 0x10aa, 0xaa10, 0x10aa, 0xaa10, 0x1049, 0x2410,
 0x1000,   0x10, 0x1000,   0x10, 0x1092, 0x9210, 0x10aa, 0xaa10,
 0x10aa, 0xaa10, 0x1092, 0x9210, 0x1000,   0x10, 0x1000,   0x10,
 0x1049, 0x2410, 0x10aa, 0xaa10, 0x10aa, 0xae10, 0x1049, 0x2a10,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0x1fff, 0xfff0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cBinaryIcon);

unsigned short cImageIcon[] = {
 kImageIcon, 0, 32, 32, 64,
 0x1fff, 0xfc00, 0x1000,  0x600, 0x13a0,  0x500, 0x13a0,  0x480,
 0x13a8, 0x2440, 0x13a0,  0x420, 0x13a0, 0xe7f0, 0x13a1, 0x1010,
 0x13a2,  0x810, 0x13a2,  0x810, 0x13aa,  0x890, 0x13a1, 0x1010,
 0x13e0, 0xe010, 0x1220,   0x10, 0x1220,   0x10, 0x1220,   0x10,
 0x122b, 0xfe90, 0x1222,  0x210, 0x11c2,  0x210, 0x1322, 0x1fd0,
 0x1322, 0x1050, 0x1212, 0x1050, 0x121b, 0xf250, 0x1210, 0x1050,
 0x1230, 0x1050, 0x1120, 0x1fd0, 0x11a0,   0x10, 0x10e0,   0x10,
 0x1230, 0x2090, 0x1000,   0x10, 0x1000,   0x10, 0x1fff, 0xfff0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cImageIcon);

unsigned short cMovieIcon[] = {
 kMovieIcon, 0, 32, 32, 64,
 0x1fff, 0xfc00, 0x1200,  0x600, 0x1200,  0x500, 0x1e7f, 0xfc80,
 0x1e7f, 0xfc40, 0x127f, 0xfc20, 0x127f, 0xfff0, 0x1e7f, 0xfcf0,
 0x1e00,   0xf0, 0x1200,   0x90, 0x13ff, 0xff90, 0x1e00,   0xf0,
 0x1e00,   0xf0, 0x127f, 0xfc90, 0x127f, 0xfc90, 0x1e7f, 0xfcf0,
 0x1e7f, 0xfcf0, 0x127f, 0xfc90, 0x127f, 0xfc90, 0x1e00,   0xf0,
 0x1e00,   0xf0, 0x13ff, 0xff90, 0x1200,   0x90, 0x1e00,   0xf0,
 0x1e55, 0x54f0, 0x122a, 0xa890, 0x1255, 0x5490, 0x1e2a, 0xa8f0,
 0x1e55, 0x54f0, 0x1200,   0x90, 0x1200,   0x90, 0x1fff, 0xfff0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cMovieIcon);

unsigned short cHTMLIcon[] = {
 kHTMLIcon, 0, 32, 32, 64,
 0x1fff, 0xfe00, 0x1000,  0x300, 0x1000,  0x280, 0x101f, 0xfa40,
 0x1000,  0x220, 0x107f, 0xfbf0, 0x1000,   0x10, 0x107f, 0xf810,
 0x1000,   0x10, 0x107f, 0xf810, 0x1000,   0x10, 0x101f, 0xf810,
 0x1000,   0x10, 0x107f, 0xf810, 0x1000,   0x10, 0x107f, 0xf810,
 0x1000,   0x10, 0x107f, 0xf810, 0x1000,   0x10, 0x107f, 0xf810,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0xffff, 0xfffe,
    0x8, 0x8000,  0x808, 0x8040, 0x1808, 0x8060, 0x2ff0, 0x7fd0,
 0x4000,    0x8, 0x2fff, 0xffd0, 0x1800,   0x60,  0x800,   0x40 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cHTMLIcon);

unsigned short cNoteIcon[] = {
 kNoteIcon, 0, 32, 32, 64,
 0xffff, 0xffff, 0x807f, 0xffff, 0x807f, 0xffff, 0x807f, 0xffff,
 0x807f, 0xffff, 0x807f, 0xc0ff, 0x887f,   0x3f, 0x887e,   0x1f,
 0x887c,    0xf, 0x8078,    0x7, 0x8078,    0x7, 0x8070,    0x3,
 0x8071, 0xddc3, 0x8070,    0x3, 0x8070,    0x3, 0x8071, 0xdd43,
 0x8070,    0x3, 0x8070,    0x3, 0x8071, 0xd703, 0x8070,    0x3,
 0x87f0,    0x3, 0x81f1, 0xeec3, 0x81f0,    0x7, 0x81f0,    0x7,
 0x81f0,    0xf, 0x81e0,   0x1f, 0x8f80,   0x7f, 0x81ff, 0xffff,
 0x81ff, 0xffff, 0x81ff, 0xffff, 0x81ff, 0xffff, 0xffff, 0xffff 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) cNoteIcon);



	// small icons
unsigned short rTextIcon[] = {
 10000+kTextIcon, 0, 16, 16, 16,
 0x7ff0, 0x4038, 0x402c, 0x47fc, 0x4004, 0x4fe4, 0x4004, 0x47e4,
 0x4004, 0x4fe4, 0x4004, 0x4fe4, 0x4004, 0x4004, 0x4004, 0x7ffc 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rTextIcon);

unsigned short rFolderIcon[] = {
 10000+kFolderIcon, 0, 16, 16, 16,
      0,      0,      0, 0x3c00, 0x6200, 0xc1fe, 0x8001, 0x8001,
 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0xffff 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rFolderIcon);

unsigned short rPhonebookIcon[] = {
 10000+kPhonebookIcon, 0, 16, 16, 16,
      0,      0,  0x7f0, 0x1ffc, 0x3ffc, 0x1ffc,  0x7f0,  0xff0,
  0xff0,  0xff0, 0xffff, 0x7001, 0x4601, 0x4061, 0x4007, 0xffff 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rPhonebookIcon);

unsigned short rSoundIcon[] = {
 10000+kSoundIcon, 0, 14, 14, 14,
   0x10,   0x70,  0x1f0, 0xffff, 0xc110, 0xffff, 0xe110, 0xe130,
 0xffff, 0xe770, 0xffff, 0xc000, 0xc000, 0xffff 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rSoundIcon);

unsigned short rBinhexIcon[] = {
 10000+kBinhexIcon, 0, 16, 16, 16,
 0x7ff0, 0x4008, 0x4004, 0x7ffc, 0x7ffc, 0x7fec, 0x7dfc, 0x4f04,
 0x4984, 0x48e4, 0x48b4, 0x497c, 0x49fc, 0x4ffc, 0x4044, 0x7ffc 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rBinhexIcon);

unsigned short rDefaultIcon[] = {
 10000+kDefaultIcon, 0, 16, 16, 16,
 0x7ff0, 0x4038, 0x402c, 0x403c, 0x4004, 0x4004, 0x4004, 0x4004,
 0x4004, 0x4004, 0x4004, 0x4004, 0x4004, 0x4004, 0x4004, 0x7ffc 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rDefaultIcon);

unsigned short rUuencodeIcon[] = {
 10000+kUuencodeIcon, 0, 16, 16, 16,
 0x7ff0, 0x4038, 0x402c, 0x7ffc, 0x7fec, 0x7eec, 0x57fc, 0x4f04,
 0x4984, 0x48e4, 0x48b4, 0x497c, 0x49fc, 0x4ffc, 0x4044, 0x7ffc 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rUuencodeIcon);

unsigned short rIndexIcon[] = {
 10000+kIndexIcon, 0, 15, 15, 15,
      0, 0xfffe, 0xfffe, 0xc006, 0xc7c6, 0xcc66, 0xcc66, 0xc0c6,
 0xc186, 0xc106, 0xc006, 0xc006, 0xc106, 0xc006, 0xfffe 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rIndexIcon);

unsigned short rTelnetIcon[] = {
 10000+kTelnetIcon, 0, 16, 16, 16,
 0x78f8, 0x78f8, 0xfdfc, 0xffff, 0xfdfd,  0xfc1,  0xfc1,  0xfc1,
  0xfc3, 0x1fff, 0x1ff1, 0xf79f, 0xf79f, 0xffff, 0xf79e, 0xf79e 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rTelnetIcon);

unsigned short rWhoisPhonebookIcon[] = {
 10000+kWhoisPhonebookIcon, 0, 16, 16, 16,
      0,      0,  0x7f0, 0x1ffc, 0x3ffc, 0x1ffc,  0x7f0,  0xff0,
  0xff0,  0xff0, 0xffff,    0x1,    0x1,    0x1,    0x1, 0xffff 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rWhoisPhonebookIcon);

unsigned short rBinaryIcon[] = {
 10000+kBinaryIcon, 0, 16, 16, 16,
 0x7ff0, 0x4038, 0x402c, 0x403c, 0x4ff4, 0x4ff4, 0x4004, 0x4ff4,
 0x4ff4, 0x4004, 0x4ff4, 0x4ff4, 0x4004, 0x4004, 0x4004, 0x7ffc 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rBinaryIcon);

unsigned short rImageIcon[] = {
 10000+kImageIcon, 0, 16, 16, 16,
 0x7ff0, 0x5c38, 0x5c2c, 0x5dfc, 0x5d24, 0x5d64, 0x5cc4, 0x5404,
 0x55f4, 0x5d7c, 0x554c, 0x55dc, 0x5474, 0x5c04, 0x4404, 0x7ffc 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rImageIcon);

unsigned short rMovieIcon[] = {
 10000+kMovieIcon, 0, 16, 16, 16,
 0x7fe0, 0x7030, 0x57e8, 0x77fc, 0x5014, 0x7ffc, 0x5014, 0x77dc,
 0x57d4, 0x77dc, 0x5014, 0x7ffc, 0x5014, 0x755c, 0x5014, 0x7ffc 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rMovieIcon);

unsigned short rHTMLIcon[] = {
 10000+kHTMLIcon, 0, 16, 16, 16,
 0x7ff0, 0x4038, 0x47fc, 0x4fe4, 0x4fe4, 0x47e4, 0x4fe4, 0x4fe4,
 0x4fe4, 0x4fe4, 0x4004, 0xffff, 0x2288, 0x7efc, 0xfffe, 0x600c 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rHTMLIcon);

unsigned short rNoteIcon[] = {
 10000+kNoteIcon, 0, 16, 16, 16,
 0xffff, 0x8fff, 0x8fff, 0xaf07, 0xae03, 0x8e03, 0x8df9, 0x8df9,
 0x8c01, 0x8df1, 0xbdf9, 0x9c03, 0x9c07, 0xbfff, 0x9fff, 0xffff 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) rNoteIcon);





	// tiny icons

unsigned short sTextIcon[] = {
 20000+kTextIcon, 0, 13, 13, 12,
 0xff00, 0x8180, 0x81c0, 0xbe40, 0x8040, 0xbe40, 0x8040, 0xbe40,
 0x8040, 0xbe40, 0x8040, 0xffc0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sTextIcon);

unsigned short sFolderIcon[] = {
 20000+kFolderIcon, 0, 12, 12, 10,
 0x3800, 0x4400, 0x83f0, 0x8010, 0x8010, 0x8010, 0x8010, 0x8010,
 0x8010, 0xfff0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sFolderIcon);

unsigned short sPhonebookIcon[] = {
 20000+kPhonebookIcon, 0, 12, 12, 10,
 0x7f80, 0xffc0, 0x7f80, 0x3f00, 0x3f00, 0xffe0, 0x8020, 0x8020,
 0x8020, 0xffe0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sPhonebookIcon);

unsigned short sSoundIcon[] = {
 20000+kSoundIcon, 0, 13, 13, 12,
 0xff00, 0x8180, 0x81c0, 0x8040, 0x8040, 0x8040, 0x8040, 0x8040,
 0x8040, 0x8040, 0x8040, 0xffc0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sSoundIcon);

unsigned short sBinhexIcon[] = {
 20000+kBinhexIcon, 0, 13, 13, 12,
 0xff00, 0x8180, 0x81c0, 0x8040, 0x8040, 0x8040, 0x8040, 0x8040,
 0x8040, 0x8040, 0x8040, 0xffc0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sBinhexIcon);

unsigned short sDefaultIcon[] = {
 20000+kDefaultIcon, 0, 13, 13, 11,
 0xfff0, 0x8030, 0x9e30, 0xb330, 0xb330, 0x8230, 0x8c30, 0x8c30,
 0x8c30, 0x8030, 0xffe0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sDefaultIcon);

unsigned short sUuencodeIcon[] = {
 20000+kUuencodeIcon, 0, 13, 13, 12,
 0x7f80, 0xbf40, 0xfec0, 0xf8c0, 0xf0c0, 0xf8c0, 0xffc0, 0x8040,
 0x8740, 0x8040, 0xffc0, 0x4080 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sUuencodeIcon);

unsigned short sIndexIcon[] = {
 20000+kIndexIcon, 0, 13, 13, 11,
   0x80,  0x380, 0xfff0, 0x8480, 0xe4b0, 0x8480, 0xedb0, 0x8d80,
 0xe030, 0x8000, 0xfff0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sIndexIcon);

unsigned short sTelnetIcon[] = {
 20000+kTelnetIcon, 0, 12, 12, 10,
 0x7f80, 0xffc0, 0x7f80, 0x3f00, 0x3f00, 0xffe0, 0x8020, 0x8020,
 0x8020, 0xffe0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sTelnetIcon);

unsigned short sWhoisPhonebookIcon[] = {
 20000+kWhoisPhonebookIcon, 0, 13, 13, 12,
 0xff00, 0x8180, 0x81c0, 0x9040, 0xa840, 0xa840, 0x9240, 0x8240,
 0x8240, 0x8240, 0x8040, 0xffc0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sWhoisPhonebookIcon);

unsigned short sBinaryIcon[] = {
 20000+kBinaryIcon, 0, 13, 13, 12,
 0xff00, 0x8180, 0x81c0, 0x8040, 0x8040, 0x8040, 0x8040, 0x8040,
 0x8040, 0x8040, 0x8040, 0xffc0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sBinaryIcon);

unsigned short sImageIcon[] = {
 20000+kImageIcon, 0, 13, 13, 12,
 0xff00, 0xb180, 0xb1c0, 0xb640, 0xb640, 0xb040, 0xb040, 0x8240,
 0xb740, 0xb240, 0x9040, 0xffc0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sImageIcon);

unsigned short sMovieIcon[] = {
 20000+kMovieIcon, 0, 13, 13, 12,
 0xff00, 0xbf80, 0xe1c0, 0xe1c0, 0xbf40, 0xe1c0, 0xe1c0, 0xbf40,
 0xe1c0, 0xe1c0, 0xbf40, 0xffc0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sMovieIcon);

unsigned short sHTMLIcon[] = {
 20000+kHTMLIcon, 0, 14, 14, 14,
 0xff00, 0x8180, 0x9fc0, 0xbf40, 0x9f40, 0xbf40, 0xbf40, 0x8040,
 0xffc0,  0xc00, 0x4c80, 0xffc0, 0x4080, 0x0000 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sHTMLIcon);

unsigned short sNoteIcon[] = {
 20000+kNoteIcon, 0, 13, 13, 12,
 0xfff0, 0x8ff0, 0x8e30, 0xac30, 0x8d90, 0x8c10, 0x8dd0, 0x9c10,
 0x9d90, 0xb030, 0x9ff0, 0xfff0 
  ,0,0,0,0,0,0 
	};
	new DIcon( (IconStore*) sNoteIcon);
}


void InitializeUGopher()
{
		// this should be flexible enough for other oses, default is "gmap-local"
		char* kLocalMapSection= "gpmap-local";
#ifdef WIN_MAC
		kLocalMapSection = "gpmap-local-mac";
#endif
#ifdef OS_UNIX
		kLocalMapSection = "gpmap-local-unix";
#endif
#ifdef OS_DOS
		kLocalMapSection = "gpmap-local-msdos";
#endif

			// call from app.inits 
	gDoSuffix2MacMap= false;
	gNeedTypeChange= false;
	gGopherMap = NULL;
	gYesNo[false]= "No";
	gYesNo[true ]= "Yes";

	NewGopherMap();

	ReadGoMapPrefSection("gpmap-types", kdKindData, gDefaultTypes);
	ReadGoMapPrefSection("gpmap-gopher0", kdServerTypeData, gDefaultServerTypes);
	ReadGoMapPrefSection("gpmap-server-suffix", kdServerSuffixData, gDefaultSuffixes);
	ReadGoMapPrefSection(kLocalMapSection, kdLocalData, gDefaultLocal);
	ReadGoMapPrefSection("gpmap-local", kdLocalData, NULL);

	gGopherIconID= kDefaultIcon;
  StoreGopherIcons();
	gGopherIcons= gIconList; 	// is this what we want??
	gIconSize= 2; 						// need to read in some app prefs  
}






// DGopherMap ------------------------------


DGopherMap::DGopherMap()
{
	Initialize();
}

DGopherMap::~DGopherMap()
{
	MemFree(fSuffix); 
	MemFree(fHandlerName); 
	MemFree(fParameters); 
//DGodocKindList	
	if (fKind) delete fKind; 
	if (fServerSuffixes) delete fServerSuffixes; 
}

Boolean DGopherMap::suicide(void) 
{ 
	if (GetOwnerCount() <= 1) { 
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide();
}

Boolean DGopherMap::suicide(short ownercount)
{
	if (ownercount < 1) {
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide(ownercount);
}

void DGopherMap::Initialize()
{
	fId= 0;
	fHandlerType= kNoHandler;
	fKind = new DGodocKindList();
	fPreference= 0;
	fTransferType= kTransferBinary; 
	fServerSuffixes= NULL;
	fHandlerName= NULL;
	fParameters= NULL;
	fSuffix= NULL;
#ifdef WIN_MAC
	fMacType= '\?\?\?\?';  
	fMacSire= '\?\?\?\?';
#else
		// damn 16bit os/compiler won't understand 'abcd' as a long
	fMacType= fMacSire= 0;
#endif
	fLocalType = '0'; 	 
	fServerType = '0'; 	 
	fMapSuffixWhen= kMapNever; 
	fSaveToDisk= false;
	fCommand= DGopher::cNewGopherFile;
	fStatus= kNochange;
	fServerLocalChanged= false;
	fServerSuffixesChanged= false;
}



void DGopherMap::ReadData(short datakind)
{
	switch (datakind) {
		case kdKindData:
		case kKindData:
		 	ReadKind(); 
		 	break;
		case kdServerTypeData: 
		case kServerTypeData: 
			ReadServerType(); 
			break;
		case kdServerSuffixData: 
		case kServerSuffixData:
			ReadServerSuffix(); 
			break;
		case kdLocalData: 
		case kLocalData: 
			ReadLocalHandling(); 
			break;
		}
}


void DGopherMap::ReadKind()
{
	const	char* wordbreaks = " \t\r\n";
	char 	*word;
#if 1	
	//line format: "text/plain text/anykind text/unknown text/jive"
	word= strtok( fParameters, wordbreaks); 
	while (word) {
		if (!fKind) fKind= new DGodocKindList(word, true);
		else fKind->AddName(word, true);
		word= strtok( NULL, wordbreaks); 
		}
#else
	//line format: "text/plain text"
	word= strtok( fParameters, wordbreaks); if (!word) return; 
	SetKind(word); 
	word= strtok( NULL, wordbreaks); if (!word) return; 
	SetTransferBinary( to_lower(*word) != 't');
#endif
}
 
 
void DGopherMap::ReadServerType()
{
	//line format: "0	0"
	const	char* wordbreaks = " \t\r\n";
	char 	*word;
	
	word= strtok( fParameters, wordbreaks); if (!word) return; 
	SetServerType( word); //fServerType= *word;
	word= strtok( NULL, wordbreaks); if (!word) return; 
	SetLocalType( word); //fLocalType= *word;
}


void DGopherMap::ReadServerSuffix()
{
	//line format: " .txt	.text	.doc"
	const	char* wordbreaks = " \t\r\n";
	char 	*word;
	
	word= strtok( fParameters, wordbreaks); 
	while (word) {
#if 1
		if (!fServerSuffixes) fServerSuffixes= new DGodocKindList(word);
		else fServerSuffixes->AddName(word);
#else
		if (!gServerSuffixes) gServerSuffixes= new DList(dgiCompareMapNames,DList::kDeleteObjects);
		DMapName* aStr= new DMapName( fId, word);
		gServerSuffixes->InsertLast( aStr);
#endif
		word= strtok( NULL, wordbreaks); 
		}
}


void DGopherMap::ReadLocalHandling()
{
	const	char* wordbreaks = " \t\r\n";
	char 	*word;
	short	num;
	
	word= strtok( fParameters, wordbreaks); if (!word) return; 
	SetPreference(word);

	word= strtok( NULL, wordbreaks); if (!word) return; 
	SetTransferBinary( to_lower(*word) != 't');

	word= strtok( NULL, wordbreaks); if (!word) return; 
	SetHandlerType( word);
	
	//Mac    line format: "rtf = 2	text   launch		TEXT/MSWD	'MicroSoft Word'"
	//msdos  line format: "rtf = 0	text   launch	.rtf	appname.exe"
	//unix   line format: "gif = 1	binary launch	.gif	'xv $file'"
	word= strtok( NULL, wordbreaks);  if (!word) return; 
	SetSuffix(word);

	if (word) {
		// all oses -- last param is fHandlerName string
		// need to watch for names w/ embedded spaces, etc -- look for ' and " delimiters  
		char* namebreaks;
		char* cp= word + StrLen(word) + 1;
		while (cp && *cp && isspace(*cp)) cp++;
		if (*cp == '"') namebreaks = "\"\r\n";
		else if (*cp == '\'') namebreaks = "\'\r\n";
		else namebreaks = " \t\r\n";
		word= strtok( cp, namebreaks);  
		if (word) SetHandlerName(word);	
		}
}


void DGopherMap::SetParameters( char* params)
{
	if (fParameters) MemFree(fParameters);
	if (params) fParameters= StrDup( (char*) params);
	else fParameters= NULL;
}

const char* DGopherMap::GetParameters()
{
	return fParameters;
}




void DGopherMap::SetCommand()
{
	fCommand= GetCommand();
}

short	DGopherMap::GetCommand()
{
	if (fHandlerType == kInternalHandler)
		switch (fId) {
					// ?? don't really need all these, just map "text/html" to internal "text"!
  
			case kNetTextHandler:
			case kNetRTFHandler:
			case kNetPictHandler:
			case kNetGifHandler:	return DGopher::cGopherNetDoc;

			case kPictHandler:
			case kGifHandler:
			case kRTFHandler:
			case kHTMLHandler:
			case kMailHandler:
			case kHTTPHandler:
			case kFingerHandler	:
			case kWhoisHandler	:
			case kTextHandler		:	return DGopher::cNewGopherText;

			case kImageHandler	:	return DGopher::cNewGopherImage;

			case kCSOHandler		:
			case kQueryHandler	:
			case kFolderPlusHandler:
			case kFolderHandler	:	return DGopher::cNewGopherFolder;
			
			default:
			case kNoteHandler		: 
			case kErrHandler		: return DGopher::cNoCommand;
			}
	else 
		return DGopher::cNewGopherFile;
}


#if 0
short DGopherMap::GetHandlerType()
{
	return fHandlerType;
}
#endif

const char* DGopherMap::GetHandlerType()
{
	switch (fHandlerType) {
		case kInternalHandler	: return "Internal";
		case kExternalHandler	: return "External";
		default:
		case kNoHandler				: return "None";
		}
}

void DGopherMap::SetHandlerType( char* s)
{
		// internal, external==launch, none, ???
	if (s) switch (*s) {
		case 'i':
		case 'I': fHandlerType= kInternalHandler; break;
		case 'e':
		case 'E':  
		case 'l':
		case 'L': fHandlerType= kExternalHandler;  break;
		case 'n':
		case 'N':  
		default : fHandlerType= kNoHandler;  break;
		}
}

	
const char* DGopherMap::GetLocalType()
{
	static char s[2]; 
	s[0]= fLocalType;
	s[1]= 0;
	return s;
}

void DGopherMap::SetLocalType( char* s)
{
	if (s) fLocalType= s[0];
}

const char* DGopherMap::GetServerType()
{
#if 0
	long i, n= gServerType->GetSize();
	for (i=0; i<n; i++) {
		DMapName* stype= (DMapName*) gServerType->At(i);
		if (stype && stype->fId == fId)
			return stype->fName;
		}
	return NULL;
#else
	static char s[2]; 
	s[0]= fServerType;
	s[1]= 0;
	return s;
#endif
}


void DGopherMap::SetServerType( char* s)
{
#if 0
	if (!gServerType) gServerType= new DList(dgiCompareMapNames,DList::kDeleteObjects);
	DMapName* kind= new DMapName( fId, s);
	//if (!gServerType->GetEqualItemNo(kind)) //<< requires special DList compare func
	gServerType->InsertLast( kind);
#else
	if (s) fServerType= s[0];
#endif
}

const char* DGopherMap::GetMapWhen()
{
	switch (fMapSuffixWhen) {
		case kMapAlways	: return "Always";  
		case kMapDefault: return "Default";
		default:
		case kMapNever 	: return "Never";
		}
}

void DGopherMap::SetMapWhen(char* when)
{
	if (when) switch (*when) {
		case 'a':
		case 'A': fMapSuffixWhen= kMapAlways; break;
		case 'd':
		case 'D': fMapSuffixWhen= kMapDefault; break;
		default : fMapSuffixWhen= kMapNever; break;
		}
}


void DGopherMap::SetSaveToDisk(Boolean turnon) 
{ 
	fSaveToDisk= turnon;
}

Boolean DGopherMap::GetSaveToDisk()
{
	//??
	return  (fHandlerType != kInternalHandler || fSaveToDisk); 
}

Boolean DGopherMap::GetTransferBinary() 
{ 
	return (fTransferType == kTransferBinary);
}

void DGopherMap::SetTransferBinary(Boolean turnon) 
{ 
	if (turnon) fTransferType = kTransferBinary;
	else fTransferType = kTransferText; 
}


Boolean DGopherMap::GetDisplay()
{
	return  (fHandlerType == kInternalHandler); 
}


Boolean DGopherMap::Map(DGopher* aGopher)
{
	Boolean changedtype= false;
	aGopher->fMacType= fMacType;  
	aGopher->fMacSire= fMacSire;
	aGopher->fSaveToDisk= fSaveToDisk;
	aGopher->fLaunch= (fHandlerType == kExternalHandler);
	aGopher->fCommand= GetCommand();
	short oldtt= aGopher->fTransferType;
	aGopher->fTransferType= fTransferType;
	
  if (aGopher->fType != kTypeQuery) { //?? do we always want to do this ?
		changedtype= (aGopher->fType != fLocalType);
		aGopher->fType= fLocalType;
		}

	if (oldtt != fTransferType)
 	 switch (fTransferType) {
		case kTransferBinary:  
			aGopher->SwapPathType('0','9');
			break;
		case kTransferText:  
			aGopher->SwapPathType('9','0');
			break;
		}					
	return changedtype;
}


const char* DGopherMap::GetID() 
{ 
	if (fHandlerType == kInternalHandler && (fId>=0 && fId < kMaxInternalHandler))
		return gInternalHandlerIDs[fId];
	else
		return Idtype2Str(fId);
}

void DGopherMap::SetID( char* s)
{
	char sid[5];
	short i;
	for (i=0; i<4; i++) if (s[i]<' ') sid[i]=' '; else sid[i]= s[i];
	sid[4]= 0;
  for ( i= 0; i<kMaxInternalHandler; i++) 
  	if (StrNCmp(sid, gInternalHandlerIDs[i], 4) == 0) {
  		fId= i;
  		fHandlerType= kInternalHandler;
  		return;
  		}
	fId= Str2Idtype(sid);
	if (fHandlerType==kInternalHandler) fHandlerType= kNoHandler;
}


const char* DGopherMap::GetMacTypeNSire() 
{ 
	// "TEXT/R*ch",  "WDBN/MSWD", "APPL/IGo4"
	static char typensire[10];
	StrNCpy(typensire, Idtype2Str(fMacType), 4);
	typensire[4]= '/';
	StrNCpy(typensire+5, Idtype2Str(fMacSire), 4);
	typensire[9]= 0;
	return typensire;
}

void DGopherMap::SetMacTypeNSire( char* s)
{
	// "TEXT/R*ch",  "WDBN/MSWD", "APPL/IGo4"
	if (s) {
		fMacType= Str2Idtype( s);
		if (StrLen(s) > 5) fMacSire= Str2Idtype( s+5);
		}
}


const char* DGopherMap::GetSuffix()
{
#ifdef WIN_MAC
	return GetMacTypeNSire();
#else
	return  fSuffix; 
#endif
}

void DGopherMap::SetSuffix( char* s)
{
#ifdef WIN_MAC
	SetMacTypeNSire(s);
#else
	if (fSuffix) MemFree(fSuffix);
	fSuffix= StrDup(s);
#endif
}


const char* DGopherMap::GetServerSuffixes()
{
		// FIX LATER

	return  NULL; 
}

void DGopherMap::SetServerSuffixes( char* s)
{
		// FIX LATER
}


const char* DGopherMap::GetKind()
{
	if (fStatus == kDeleted) return "*** DELETED ***";
#if 1
	else if (fKind) return fKind->GetName();
	else return "type/unknown";
#else
	else if (gNetKinds) {
		long i, n= gNetKinds->GetSize();
		for (i=0; i<n; i++) {
			DMapName* kind= (DMapName*) gNetKinds->At(i);
			if (kind->fId == fId) return kind->fName;
			}
		}
	return "type/unknown";
#endif
}

void DGopherMap::SetKind( char* s)
{
#if 1
	const	char* wordbreaks = " \t\r\n";
	char 	*word;
	s= StrDup(s);
	word= strtok( s, wordbreaks); 
	while (word) {
		if (!fKind) fKind= new DGodocKindList(word, true);
		else fKind->AddName(word, true);
		word= strtok( NULL, wordbreaks); 
		}
	MemFree(s);
	
#else
#if 1
	if (!gNetKinds) gNetKinds= new DList(dgiCompareMapNames,DList::kDeleteObjects);
	DMapName* kind= new DMapName( fId, s);
	StrLocase(kind->fName);
	//if (!gNetKinds->GetEqualItemNo(kind)) //<< requires special DList compare func
	long i, n= gNetKinds->GetSize();
	for (i= 0; i<n; i++) {
		DMapName* old= (DMapName*) gNetKinds->At(i);
		if (Nlm_StringCmp(kind->fName, old->fName)==0) {
			old->fId= fId;
			return;
			}
		}
	gNetKinds->InsertLast( kind);
#else
	if (fKind) {
		// ?? assume fKind is primary kind, and s is alternate name
		//MemFree(fKind);
		if (fAltKind) MemFree(fAltKind);
		fAltKind= StrDup(s);
		StrLocase(fAltKind);
		}
	else {
		fKind= StrDup(s);
		StrLocase(fKind);
		}
#endif
#endif
}

const char* DGopherMap::GetHandlerName()
{
	return fHandlerName;  
}

void DGopherMap::SetHandlerName( char* s)
{
	if (fHandlerName) MemFree(fHandlerName);
	fHandlerName= StrDup(s);
	//StrLocase(fHandlerName);
}

const char* DGopherMap::GetPreference()
{
	static char snum[40]; 
	Dgg_LongToStr( fPreference, snum, 0, 40);
	return snum;
}

void DGopherMap::SetPreference( char* s)
{
	fPreference = atol( s);  
}






// DGopherMapList ------------------


DGopherMapList::DGopherMapList() :
	DList(NULL,DList::kDeleteObjects)
{
}


void DGopherMapList::DeleteItem(char* theId)
{
	DGopherMap* mapper= new DGopherMap();
		if (mapper) {
		mapper->SetID( theId);
		long i, n= this->GetSize();
		for (i= 0; i<n; i++) {
			DGopherMap* current= this->GopherMapAt(i);
			if (current && mapper->fId == current->fId) {
				if (current->fStatus == kDeleted) current->fStatus= kNochange;  
				else current->fStatus= kDeleted; 
				//this->AtDelete(i); // this deletes the current object 
				break;
				}
			}
		delete mapper;
		}
}

void DGopherMapList::ReadParams(char* theId, char* params, short datakind)
{
	DGopherMap* mapper= new DGopherMap();
	if (mapper) {
		mapper->SetID( theId);
		
		if (true) { // (datakind < kDontCheckDups) 
			long i, n= this->GetSize();
			for (i= 0; i<n; i++) {
				DGopherMap* current= this->GopherMapAt(i);
				if (current && mapper->fId == current->fId) {
					current->SetParameters(params);
					current->ReadData(datakind);
					delete mapper;
					return;
					}
				}
			}
		mapper->SetParameters(params);
		mapper->ReadData(datakind);
		this->InsertLast( mapper);
		}
}


DGopherMap* DGopherMapList::MatchHandlerKind(unsigned short& lastrank, char* aKind)
{
	Boolean okay, gotKind, gotHand;
#if 1
	long i, n= this->GetSize();
	for (i= 0; i<n; i++) {
		DGopherMap* mapper= this->GopherMapAt(i);
		gotKind= (mapper->fKind) ? mapper->fKind->Match(aKind) : false;
		if (gotKind && mapper->fPreference) {
			lastrank= mapper->fPreference;
			return mapper;
			}		
		}
#else
#if 1
	long i, n= gNetKinds->GetSize();
	for (i= 0; i<n; i++) {
		DMapName* kind= (DMapName*) gNetKinds->At(i);
		if (kind && Nlm_StringCmp(kind->fName,aKind) == 0) {
			DGopherMap* mapper= this->MatchID( kind->fId);
			if (mapper && mapper->fPreference > lastrank) {
				lastrank= mapper->fPreference;
				return mapper;
				}
		 	}
		}
#else	
	long i, n= this->GetSize();
	for (i= 0; i<n; i++) {
		DGopherMap* mapper= this->GopherMapAt(i);
		gotKind= (mapper->fKind) ? 
			Nlm_StringCmp(mapper->fKind,aKind) == 0 : false;
		if (!gotKind) gotKind= (mapper->fAltKind) ? 
				Nlm_StringCmp(mapper->fAltKind,aKind) == 0 : false;
		okay= gotKind;
		unsigned short rank= mapper->fPreference;
		if (okay) okay= (rank > lastrank);
		if (okay) {
			lastrank= rank;
			return mapper;
			}
		}
#endif
#endif
	return NULL;
}

DGopherMap* DGopherMapList::MatchHandlerKind(DGopher* aGopher)
{ 
	// need to decide which criteria to match on ...
	if (aGopher->fViews && (aGopher->fViews->GetSize()>0)) {
		unsigned short lastrank = 0;
		short choice= aGopher->fViewchoice;
		if (choice) {
			DGopherItemView* giv= (DGopherItemView*) aGopher->fViews->At(choice-1);
			if (giv) {
				char* aKind = StrDup(giv->Kind()); 
				StrLocase(aKind);
				char* cp= StrChr(aKind,' '); if (cp) *cp= 0;
					// add Language match !?!!
				DGopherMap* result= this->MatchHandlerKind( lastrank, aKind); 
				MemFree( aKind);
				return result;
				}
			else return NULL;
			}
			
		else { // stroll thru this map, look for prefered match to aGopher->fViews...
			short maxchoice= aGopher->fViews->GetSize();
			DGopherMap* bestmapper = NULL;
			for (choice = 1; choice <= maxchoice; choice++) {
				DGopherItemView* giv= (DGopherItemView*) aGopher->fViews->At(choice-1);
				if (giv) {
					char* aKind = StrDup(giv->Kind()); 
					StrLocase(aKind);					 
					char* cp= StrChr(aKind,' '); if (cp) *cp= 0;
					DGopherMap* amapper= this->MatchHandlerKind( lastrank, aKind); 
					if (amapper) bestmapper= amapper;
					MemFree( aKind);
					}
				}
			return bestmapper;
			}
		}
	return NULL;
}

DGopherMap* DGopherMapList::MatchID( long theId)
{ 
	long i, n= this->GetSize();
	for (i= 0; i<n; i++) {
		DGopherMap* mapper= this->GopherMapAt(i);
		if (theId == mapper->fId)	return mapper;
		}
	return NULL;
}


DGopherMap* DGopherMapList::MatchGopherType( char theType, Boolean matchAny)
{ 
#if 0
  if (gServerType) {
  	long i, n= gServerType->GetSize();
		for (i= 0; i<n; i++) {
			DMapName* stype= (DMapName*) gServerType->At(i);
			if (stype && *stype->fName == theType) {
				DGopherMap* mapper= this->MatchID( stype->fId);
				if (mapper && (matchAny || mapper->fMapSuffixWhen == kMapAlways))
					return mapper;
				else
					return NULL;
				}
			}
  	}
#else
	long i, n= this->GetSize();
	for (i= 0; i<n; i++) {
		DGopherMap* mapper= this->GopherMapAt(i);
		if (theType == mapper->fServerType &&
			(matchAny || mapper->fMapSuffixWhen == kMapAlways))
						    //  ^^^^^^^^^^^^^ NEED TO CHANGE TO ANOTHER FIELD, e.g. fMapServerTypeWhen 
			return mapper;
		}
#endif
	return NULL;
}

 
DGopherMap* DGopherMapList::MatchGopherType(DGopher* aGopher)
{ 
	return MatchGopherType( aGopher->fType, false);
}
 
 

DGopherMap* DGopherMapList::MatchSuffix(const char* pathname, char gopherType)
{ 
			///  FIX THIS LATER
#if 0
	char* suffix= StrRChr( pathname, '.');
	if (!suffix) suffix= (char*) pathname;
	long i, n= gServerSuffixes->GetSize();
	for (i= 0; i<n; i++) {
		DMapName* kind= (DMapName*) gServerSuffixes->At(i);
		if (kind && Nlm_StringCmp(kind->fName,suffix) == 0) {
			DGopherMap* mapper= this->MatchID( kind->fId);
			if (mapper && (mapper->fMapSuffixWhen == kMapAlways)
			 || (mapper->fMapSuffixWhen == kMapDefault && gopherType == kTypeFile))
					return mapper;
		 	}
		}
#endif
	return NULL;
}


DGopherMap* DGopherMapList::MatchSuffix(DGopher* aGopher)
{ 
	return MatchSuffix( aGopher->GetPath(), aGopher->fType);
}




DGopherMap* DGopherMapList::GetPreferedFiletype(char* aKind, 
							 long& macFileType, long& macCreator, char*& suffix)
{
	unsigned short lastrank = 0;
	DGopherMap* gmap = NULL;
	aKind= StrDup(aKind);
	StrLocase(aKind);
	char* cp= StrChr(aKind,' '); if (cp) *cp= 0;
	gmap= this->MatchHandlerKind( lastrank, aKind);
	MemFree(aKind);
	
	if (gmap) {
		macFileType= gmap->fMacType;
		macCreator = gmap->fMacSire;
		suffix= StrDup(gmap->GetSuffix());
		return gmap;
		}
	else 
		return NULL;
}


DGopherMap* DGopherMapList::GetPreferedFiletype(DGopher* aGopher, 
							 long& macFileType, long& macCreator, char*& suffix)
{
	unsigned short lastrank = 0;
	DGopherMap* gmap = NULL;
	if (!aGopher) return false;
	char* aKind= StrDup(aGopher->GetViewChoiceKind());
	if (aKind) {
		StrLocase(aKind);
		char* cp= StrChr(aKind,' '); if (cp) *cp= 0;
		gmap= this->MatchHandlerKind( lastrank, aKind);
		MemFree(aKind);
		}
	if (!gmap) 
		gmap= this->MatchGopherType( aGopher->fType, true);
	if (gmap) {
		macFileType= gmap->fMacType;
		macCreator = gmap->fMacSire;
		suffix= StrDup(gmap->GetSuffix());
			//?? do we want to set these in caller's data !?
		//aGopher->fMacType= macFileType;
		//aGopher->fMacSire= macCreator;
		//aGopher->fLaunch= (gmap->fHandlerType == kExternalHandler);
		return gmap;
		}
	else 
		return NULL;
}

			
	










//class DGoMapHandlerView : public DTableView

enum GoDocMessages {
	kEditButHit = 4560, kAddButHit, kRemoveButHit, 
	kServerTypeEnableHit, kServerTypeButHit,
	kServerSuffixEnableHit,kServerSuffixButHit
	};

Local char*	kGopherTypeExample= "application/gopher+-menu";
Local char*	kGopherHandlerExample= "MicroSoft Excel 1234567890";


DGoMapHandlerView::DGoMapHandlerView(long id, DGopherMapDoc* itsDoc, 
												short pixwidth, short pixheight):
	DTableView( id, itsDoc, pixwidth, pixheight, 10, 2, 50, 20, true, false),
	fItems(NULL),
	fDoc(itsDoc)
{	
	fItems= itsDoc->fItems;
	this->SetSlateBorder(true); 
	this->SetResize( DView::relsuper, DView::relsuper);
	this->SetTableFont(gGoviewFont);
	this->GetReadyToShow(); // ??
}


void DGoMapHandlerView::Drag(Nlm_PoinT mouse)
{
	// this is called on drag, if click/release are ignored
	//short row, col;
	//PointToCell(mouse, row, col);
	DTableView::Drag(mouse);
}

void DGoMapHandlerView::Hold(Nlm_PoinT mouse)
{
	//short row, col;
	//PointToCell(mouse, row, col);
	DTableView::Hold(mouse);
}

void DGoMapHandlerView::Release(Nlm_PoinT mouse)
{
	//short row, col;
	//PointToCell(mouse, row, col);
	DTableView::Release(mouse);
}

void DGoMapHandlerView::DoubleClickAt(short row, short col)
{
	DGopherMap* ag= fItems->GopherMapAt(row); 
	//if (ag) Message(MSG_OK, "Click on '%s'", (char*)ag->GetName());
	//if (gKeys->option()) itsViewchoice= DGopher::kGetItemInfo;  
	if (ag) fDoc->EditLocalHandling(ag, DGoMapEditDlog::kFormLocal);
}

void DGoMapHandlerView::SingleClickAt(short row, short col)
{
	DTableView::SingleClickAt(row,  col);
}


DGopherMap* DGoMapHandlerView::SelectedMapper()
{
	if (GetSelectedRow() != kNoSelection) 
		return fItems->GopherMapAt( GetSelectedRow());
	else 
		return NULL;
}


void DGoMapHandlerView::GetReadyToShow()
{
 	//Nlm_WindoW w= Nlm_SavePort( GetNlmObject());
	SelectFont();
	short charheight= Nlm_FontHeight();
	if (fItems && fItems->GetSize() > 0) SetTableSize( fItems->GetSize(), GetMaxCols());
	SetItemHeight(0, GetMaxRows(), charheight);

#if THIS_IS_DONE_IN_DOC
	short  wid;
	wid= Nlm_StringWidth(kGopherTypeExample);
	this->SetItemWidth( 0, 1, wid);
#if 0	 
	wid= Nlm_StringWidth("999");
	this->SetItemWidth( 1, 1, wid);  
#endif
	wid= Nlm_StringWidth(kGopherHandlerExample);
	this->SetItemWidth( 1, 1, wid); 	 
#endif
	
	//Nlm_RestorePort( w);
}

void DGoMapHandlerView::DrawCell(Nlm_RecT r, short row, short col)
{
	// need a DTableView field to handle cell rect insets !
	char *name;
	if (col) r.left += 2; // quick fix for cells against each
	DGopherMap* mp= fItems->GopherMapAt(row); 
	if (mp) switch (col) {
		case 0:  
			Nlm_DrawString(&r, (char*)mp->GetKind(), 'l', false);
			break;   
		case 1: 
#if 0 
			Nlm_DrawString(&r, (char*)mp->GetPreference(), 'l', false);
			break;   
		case 2: 
#endif 
			name= (char*)mp->GetHandlerType();
			if (*name=='E') name=(char*)mp->GetHandlerName();
			Nlm_DrawString(&r, name, 'l', false);
			break;   
		}
}







// DGopherMapDoc ------------------------


DGopherMapDoc::DGopherMapDoc( long id, DGopherMapList* itsMappers) :
	DWindow(id, gApplication),
	fListView(NULL),
	fServerTypeEnable(NULL),
	fServerSuffixEnable(NULL),
	fChanged(false),
	fItems(itsMappers)  // itsMappers == gGopherMap always ??
{ 
	short width= -1, height= -1, left= -10, top= -20; // default window loc
	this->InitWindow(document, width, height, left, top, "Gopher Type Handling"); 
}


//static
void DGopherMapDoc::UpdateGopherMap(short item, DGopherMap *gm)
{
	char	name[20], buf[512], *s2, *fmt;

	switch (gm->fStatus) {

		case kChanged:
		case kAdded:
			StrNCpy(name, (char*)gm->GetID(), 20);
			gm->fKind->GetAllNames( buf, sizeof(buf));
			gApplication->SetPref( buf, name, "gpmap-types");
			s2= (char*)gm->GetHandlerName();
			if (StrChr( s2, ' ') != NULL ) {
				if (StrChr( s2, '\'') != NULL ) fmt= "%s %s %s \"%s\"";
				else fmt= "%s %s %s %s '%s'";
				}
			else
				fmt= "%s %s %s %s %s";
				
			if (!gm->GetTransferBinary()) s2= "text"; else s2= "binary";
			sprintf(buf, fmt,
					(char*)gm->GetPreference(), 
					s2,
					(char*)gm->GetHandlerType(),
					(char*)gm->GetSuffix(),
					(char*)gm->GetHandlerName()
					);
			gApplication->SetPref( buf,  name, "gpmap-local");

				// !! set this only if requested !!
			if (gm->fServerLocalChanged) {
				sprintf( buf, " %c %c", *(gm->GetServerType()), *(gm->GetLocalType()));
				gApplication->SetPref( buf,  name, "gpmap-gopher0");
				gm->fServerLocalChanged= false;
				}
				// !! set this only if requested !!
			if (gm->fServerSuffixesChanged) {
				gApplication->SetPref( (char*)gm->GetServerSuffixes(), name, "gpmap-server-suffix");
				gm->fServerSuffixesChanged= false;
				}
			
			gm->fStatus= kNochange;
			break;
			
		case kDeleted: 
			StrNCpy(name, (char*)gm->GetID(), 20);
			gApplication->SetPref( (char*)NULL, name, "gpmap-types");
			gApplication->SetPref( (char*)NULL, name, "gpmap-gopher0");
			gApplication->SetPref( (char*)NULL, name, "gpmap-server-suffix");
			gApplication->SetPref( (char*)NULL, name, "gpmap-local");
			if (item>kEmptyIndex) gGopherMap->AtDelete(item); //fItems
			break;
		}
}


void DGopherMapDoc::Close()
{
	if (fChanged) {
		// save all mappers to disk? -- need to track deleted/changed/added for this
		long i, n= fItems->GetSize();
		for (i=0; i<n; i++) UpdateGopherMap( i, fItems->GopherMapAt(i));
		}
		
	DWindow::Close();
}


void DGopherMapDoc::Open()
{
	DView * super;
	DButton * butt;
 	DCluster * clu;
	DPrompt* pr;
	short  wid,col1wid,col2wid,height;

	super= this;
 	Nlm_SelectFont(gGoviewFont);
	col1wid= Nlm_StringWidth(kGopherTypeExample);
	pr= new DPrompt(0,super,"Net Document Type", col1wid, 0, gGoviewFont);
	super->NextSubviewToRight();
	
	col2wid= Nlm_StringWidth(kGopherHandlerExample);
	pr= new DPrompt(0,super,"Handler", col2wid, 0, gGoviewFont);
	super->NextSubviewBelowLeft();
	
	// DGoMapHandlerView goes here...
	wid= col1wid + col2wid + 10;
	height= Nlm_FontHeight() * 10;
	fListView= new DGoMapHandlerView(0,this,wid,height);
	fListView->SetItemWidth( 0, 1, col1wid);
	fListView->SetItemWidth( 1, 1, col2wid);

	super= this;
	super->NextSubviewBelowLeft();
	Nlm_PoinT nps;
	this->GetNextPosition( &nps);
  nps.y += 8; // view border is cut off by buttons
	this->SetNextPosition( nps);
	butt= new DButton(kEditButHit,super,"Edit");
	butt->SetResize( DView::moveinsuper, DView::moveinsuper);
	super->NextSubviewToRight();
	butt= new DButton(kAddButHit,super,"Add");
	butt->SetResize( DView::moveinsuper, DView::moveinsuper);
	super->NextSubviewToRight();
	butt= new DButton(kRemoveButHit,super,"Remove");
	butt->SetResize( DView::moveinsuper, DView::moveinsuper);
	super->NextSubviewBelowLeft();

#if 0
	fServerTypeEnable= new DCheckBox(kServerTypeEnableHit,super,"Use Server-Local Gopher0 map");
	fServerTypeEnable->SetStatus(gDoSuffix2MacMap);
	fServerTypeEnable->SetResize( DView::moveinsuper, DView::moveinsuper);

	fServerSuffixEnable= new DCheckBox(kServerSuffixEnableHit,super,"Use Server suffix map");
	fServerSuffixEnable->SetStatus(gDoSuffix2MacMap);
	fServerSuffixEnable->SetResize( DView::moveinsuper, DView::moveinsuper);
	
#else	
	clu= new DCluster( 0, super, 0, 0, false,"Server-Local type map");
	clu->SetResize( DView::moveinsuper, DView::moveinsuper);
	super= clu;  
	fServerTypeEnable= new DCheckBox(kServerTypeEnableHit,super,"Enable");
	fServerTypeEnable->SetStatus(gDoSuffix2MacMap);
	super->NextSubviewToRight();
	butt= new DButton(kServerTypeButHit,super,"Edit map");
	//if (!gDoSuffix2MacMap) butt->Disable();

	super= this;
	super->NextSubviewToRight();

	clu= new DCluster( 0, super, 0, 0, false,"Server suffix map");
	clu->SetResize( DView::moveinsuper, DView::moveinsuper);
	super= clu;  
	fServerSuffixEnable= new DCheckBox(kServerSuffixEnableHit,super,"Enable");
	fServerSuffixEnable->SetStatus(gDoSuffix2MacMap);
	super->NextSubviewToRight();
	butt= new DButton(kServerSuffixButHit,super,"Edit map");
	//if (!gDoSuffix2MacMap) butt->Disable();
#endif
	
	super= this;		 

	AddOkayCancelButtons();
	DWindow::Open();
}


//static
Boolean DGopherMapDoc::EditHandler( char* itsKind, DGopherMap* item, short dlogKind)
{
	Boolean didEdit= false;
	if (gGopherMap && (item || dlogKind == DGoMapEditDlog::kFormNew)) {
		DGoMapEditDlog* dlg= new DGoMapEditDlog( item, dlogKind, itsKind);
		if ( dlg->PoseModally() ) {
			item= dlg->fMapper;
			if (item->fStatus == kAdded) {
				// must add to list & also add line to listview...
				item->newOwner();
				item->fStatus = kChanged; // ?? otherwise edits cause new inserts !?
				gGopherMap->InsertLast(item); // fItems->
				//fListView->ChangeRowSize( fListView->GetMaxRows(), 1);
				}
			//fListView->Invalidate();
			if (dlogKind == DGoMapEditDlog::kFormServerSuffixes)
				item->fServerSuffixesChanged= true;
			else if (dlogKind == DGoMapEditDlog::kFormServerLocalMap)
				item->fServerLocalChanged= true;
			DGopherMapDoc::UpdateGopherMap( kEmptyIndex, item); //fChanged= true;
			didEdit= true;
			}
		delete dlg; 
		}
	return didEdit;
}

 
void DGopherMapDoc::EditLocalHandling(DGopherMap* item, short dlogKind)
{
	if ( item || dlogKind == DGoMapEditDlog::kFormNew) {
		DGoMapEditDlog* dlg= new DGoMapEditDlog(item, dlogKind);
		if ( dlg->PoseModally() ) {
			item= dlg->fMapper;
			if (item->fStatus == kAdded) {
				// must add to list & also add line to listview...
				item->newOwner();
				item->fStatus = kChanged; // ?? otherwise edits cause new inserts !?
				fItems->InsertLast(item);  
				fListView->ChangeRowSize( fListView->GetMaxRows(), 1);
				}
			fListView->Invalidate();
			fChanged= true;
			if (dlogKind == DGoMapEditDlog::kFormServerSuffixes)
				item->fServerSuffixesChanged= true;
			else if (dlogKind == DGoMapEditDlog::kFormServerLocalMap)
				item->fServerLocalChanged= true;
			}
		delete dlg; 
		}
}



Boolean DGopherMapDoc::IsMyAction(DTaskMaster* action) 
{
	DGopherMap* mapper;
	DGoMapEditDlog* dlg;
	
	switch (action->Id()) {
		case kEditButHit:
			EditLocalHandling( fListView->SelectedMapper(), DGoMapEditDlog::kFormLocal);
			return true;
			
		case kAddButHit:
			EditLocalHandling( NULL, DGoMapEditDlog::kFormNew);
			return true;
		
		case kRemoveButHit:
			mapper= fListView->SelectedMapper();
			if (mapper) {
				fItems->DeleteItem( (char*)mapper->GetID());
				fListView->Invalidate();
				fChanged= true;
				}
			return true;
		
		case kServerTypeButHit:
			EditLocalHandling( fListView->SelectedMapper(), DGoMapEditDlog::kFormServerLocalMap);
			return true;
			
		case kServerSuffixButHit:
			EditLocalHandling( fListView->SelectedMapper(), DGoMapEditDlog::kFormServerSuffixes);
		  return true;

		case kServerTypeEnableHit:
		case kServerSuffixEnableHit:
		  return true;
		  
		default: 
			return DWindow::IsMyAction(action);		
		}
}





void EditGopherMap()
{
	if (gGopherMap) {
		DGopherMapDoc* doc= new DGopherMapDoc(0, gGopherMap);
		doc->Open();
		}
}







#if FIX_LATER


Boolean DGopherMapView::IsMyAction(DTaskMaster* action) 
{
	
  if (action->Id() == 'appn') {
		// do nonStandardFileGet('APPL') -- search for Appl signature & doc types 
		// in chosen appl rez fork
		// stuff Sire & Type
		//call this for special sfget dialog... need to have application handle specially

#if FIX_LATER
		const short	cChooseAppType = 4322; // this is in UGopherApp.cp
		TList * aFileList = NULL;
		if (gHasAppleEvents && gApplication->ChooseDocument(cChooseAppType, &aFileList)) {
			CStr63	aName;
			TEditText * et;
			TButton   * bt;
			TPopup	* pop;
			TView		* super = ((TView*) source)->fSuperView;
			TFile		* aFile= (TFile*) aFileList->First();
			long		sire, sig, kind= '\?\?\?\?';
			short		err, bundlecount;
			
			err= aFile->GetFileCreator(sire);
			if (et= (TEditText*) super->FindSubview('sire')) {
				et->SetTitle( CStr31( (long)sire), kDontRedraw);
				et->ForceRedraw();
				}
			aFile->GetName( aName);
			if (bt= (TButton*) super->FindSubview('appn')) {
				bt->SetTitle( aName, kDontRedraw);
				bt->ForceRedraw();
				}
 
			BNDLptr bundles= ReadBundle( aFile, sig, bundlecount);
			if (bundles) { 		//&& sig == sire)  
				if (pop= (TPopup*) super->FindSubview('ptyp')) {
					kind=  SetupTypeMenu( pop, fDefaultTypes, bundles, bundlecount);
					if (et= (TEditText*) super->FindSubview('type')) {
						et->SetTitle( CStr31((long) kind), kDontRedraw);
						et->ForceRedraw();
						}
					}
				}
			if (bundles) free(bundles); // made w/ malloc
			aFile = (TFile *)FreeIfObject(aFile);
			aFileList = (TList *)(FreeIfObject(aFileList));
			}
#endif
		return true;
		}
		
  else if (action->Id() == 'ptyp') {
		char  aName[256];
		DEditText * et;
		DPopupList * pop = (DPopupList*) action;
		DView * super = ((DView*) action)->fSuperior;
		pop->GetItemTitle(pop->GetValue(), aName, 255);
		if (strlen(aName) > 4) aname[4]= 0;
		if (et= (DEditText*) super->FindSubview('type')) {
			et->SetTitle( aName);
			//et->ForceRedraw();
			}
		return true;
		}
		
	else
		return Inherited::IsMyAction(action);
}



#endif // FIX_LATER

 
		
						






// DGopherEditWindow ------------------------


enum DGopherEditCmds {
	citem=1,cProto,cTITL,cHOST,cPATH,cKIND,ctran,cisGp,cPORT,ctype,csire, cURL
	};


DGopherEditWindow::DGopherEditWindow(DGopher* itsGopher, Boolean editAll) :
	DWindow( 0, NULL, DWindow::fixed, -10, -10, -50, -20, NULL, kDontFreeOnClose),
	fEditAll(editAll),
	fView(NULL),
	fGopher(itsGopher)
{	
	if (fGopher && fGopher->fType != kTypeError) {
		fGopher->newOwner();
		if (fEditAll) this->SetTitle("Edit network link");
		else this->SetTitle("Open network service");
		fName= fGopher->GetName();
		fHost= fGopher->GetHost();
		fPort= fGopher->GetPort();
		fPath= fGopher->GetPath();
		fURL= fGopher->GetURL();
		}
	else {
		if (fEditAll) this->SetTitle("New network link");
		else this->SetTitle("New network service");
		fName= "Title of link";
		fHost= "Host.Name.Goes.Here";
		fPort= "70";
		fPath= "";	
		fURL= "gopher://host.name.here/00path/to/doc/here";
		}
}

DGopherEditWindow::~DGopherEditWindow()
{
	//if (fView) delete fView; //NNNNNOOOOOOOOO --- DView::deletesubviews does this now
	if (fGopher) fGopher->suicide();
}

void DGopherEditWindow::OkayAction()
{
	fView->Answers();
}


void DGopherEditWindow::Open()
{
	DView			* super;
	DEditText	* et;
	DPrompt		* pr;
	DCheckBox	* ck;
	DPopupList* pop;
	Nlm_FonT	promptFont = Nlm_programFont; //Nlm_systemFont
	Nlm_FonT	editFont = Nlm_programFont; 
	short 		listitem;
	char		* name;


	fView= new DGopherEditView(citem, this, fGopher, fURL, 50, 20, true, NULL);
	super= fView;
	
	pr= new DPrompt(0, super, "Protocol:", 0, 0, promptFont);	 		
	super->NextSubviewToRight();
	pop= new DPopupList(cProto, super, true);
	listitem= 0;
	do {
	  name= (char*)DGopher::GetProtoName( listitem);
    if (name) {
			pop->AddItem( name);
			if (!DGopher::GetProtoSupport(listitem)) 
				pop->SetItemStatus( listitem+1, false);
			listitem++;
			}
	} while (name);
	super->NextSubviewBelowLeft();

	pr= new DPrompt(0, super, "Name:", 0, 0, promptFont);	 		
	super->NextSubviewToRight();
	et= new DEditText(cTITL, super, (char*)fName, 30, editFont);
	this->SetEditText(et);
	super->NextSubviewBelowLeft();
	
	pr= new DPrompt(0, super, "Host:", 0, 0, promptFont);	 		
	super->NextSubviewToRight();
	et= new DEditText(cHOST, super, (char*)fHost, 30, editFont);
	this->SetEditText(et);
	super->NextSubviewBelowLeft();

	if (fEditAll) {
		pr= new DPrompt(0, super, "Path:", 0, 0, promptFont);	 		
		super->NextSubviewToRight();
		et= new DEditText(cPATH, super, (char*)fPath, 30, editFont);
		this->SetEditText(et);
		super->NextSubviewBelowLeft();
		
		pr= new DPrompt(0, super, "Type:", 0, 0, promptFont);	 		
		super->NextSubviewToRight();
		pop= new DPopupList(cKIND, super, true);

#if 1		
		listitem= 0;
		do {
		  name= (char*)DGopher::GetTypeName( listitem);
      if (name) {
				pop->AddItem( name);
				if (!DGopher::GetTypeSupport(listitem)) 
					pop->SetItemStatus( listitem+1, false);
				listitem++;
				}
		} while (name);

#else
					// AARRRRGGGHHHHH !!!!!!!!
					// there should be DGopher STATIC method(s) 
					// to return gopher class list/name/item#
					// so we can add/change classes w/o having to edit unrelated files like this one !
		pop->AddItem("Document");	 
		pop->AddItem("Folder");	 
		pop->AddItem("Query");	 
		pop->AddItem("Binary file");	 
		pop->AddItem("Image");	 
		pop->AddItem("Sound");	 
		pop->AddItem("Movie");	 
		pop->AddItem("Note");	 
		pop->AddItem("Html");	 
		pop->AddItem("Mailto");	 
		pop->AddItem("Telnet link");	 
		pop->AddItem("TN3270 link");	 
		///
		pop->AddItem("CSO Phonebook");	 
		pop->AddItem("Whois Phonebook");	 
		pop->AddItem("Binhex file");	 
		pop->AddItem("Uuencoded file");	 

		pop->AddItem("Unknown type");	 
#endif
		super->NextSubviewToRight();

		ck= new DCheckBox(ctran, super, "Use binary transfer");
		ck->SetStatus(false);
		super->NextSubviewBelowLeft();
		}
	
	pr= new DPrompt(0, super, "Port:", 0, 0, promptFont);	 		
	super->NextSubviewToRight();
	et= new DEditText(cPORT, super, (char*)fPort, 4, editFont);
	this->SetEditText(et);
	super->NextSubviewToRight();

		// ?? make new method  DView::NextSubviewOffset( x, y);
		// ?? make new method  DView::NextSubviewMoveto( x, y);
	Nlm_PoinT nps;
	super->GetNextPosition( &nps);
	nps.x += 15; 
	super->SetNextPosition( nps);

	ck= new DCheckBox(cisGp, super, "Is Gopher+ link");
	ck->SetStatus(false);
	super->NextSubviewBelowLeft();

	pr= new DPrompt(0, super, " -- or enter universal resource locator -- ",
			 0, 0, promptFont);	 		
	super->NextSubviewBelowLeft();

	pr= new DPrompt(0, super, "URL:", 0, 0, promptFont);	 		
	super->NextSubviewToRight();
	et= new DEditText(cURL, super, (char*)fURL, 60, editFont);
	this->SetEditText(et);
	super->NextSubviewBelowLeft();

	this->AddOkayCancelButtons();
	fView->InstallControls();
	
	DWindow::Open();
}

 
 
// DGopherEditView ------------------------


DGopherEditView::DGopherEditView(long id, DView* itsSuperior, DGopher* itsGopher,
			const char* oldURL, short width, short height, Boolean hidden, char* title) :
	DCluster( id, itsSuperior, width, height, hidden, title),
	fOldURL(NULL), fGopher(itsGopher)
{
	fOldURL= StrDup( (char*) oldURL);
}

DGopherEditView::~DGopherEditView()
{
	MemFree( fOldURL);
}

void	DGopherEditView::InstallControls()
{
	DControl* nextline= this;
	DGopher*  go= fGopher;

	if (go) {
		DEditText * et;
		DEditText * nt; //DNumberText
		DCheckBox * cb;
		DPopupList * pop;
		DButton   * bt;
		char   	  aLine[256];
		short			item;
		
#define go2EditText(ID, GOVAL)		\
{ if (NULL != (et= (DEditText*)nextline->FindSubview(ID))) \
	et->SetTitle( (char*)GOVAL); }

#define go2NumbText(ID, GOVAL)		\
{ if (NULL != (nt= (DEditText*)nextline->FindSubview(ID))) \
	sprintf(aLine, "%d", GOVAL); nt->SetTitle( aLine); }

#define go2CheckBox(ID, GOVAL)		\
{ if (NULL != (cb= (DCheckBox*)nextline->FindSubview(ID))) \
	cb->SetStatus( GOVAL ); }

#define go2Popup(ID, GOVAL)		\
{ if (NULL != (pop= (DPopupList*)nextline->FindSubview(ID))) \
	pop->SetValue( GOVAL ); }

		item= 1 + DGopher::GetProtoItem( go->fProtocol);
		go2Popup( cProto, item);

		go2EditText( cTITL, go->GetName());
		go2EditText( cHOST, go->GetHost());
		go2EditText( cPATH, go->GetPath());
		go2EditText( cPORT, go->GetPort());
		go2EditText( cURL, go->GetURL());

#if 1
		item= 1 + DGopher::GetTypeItem( go->fType);
#else
		switch (go->fType) {
		
					// AARRRRGGGHHHHH !!!!!!!!
					// there should be DGopher STATIC method(s) 
					// to return gopher class list/name/item#
					// so we can add/change classes w/o having to edit unrelated files like this one !

			case kTypeFile   		: item= 1; break;
			case kTypeFolder		: item= 2; break;
			case kTypeQuery    	: item= 3; break;
			case kTypeBinary   	: item= 4; break;
			case kTypeImage			: item= 5; break;
			case kTypeGif				: item= 5; break; // kTypeImage
			case kTypeSound   	: item= 6; break;
			case kTypeMovie			: item= 7; break;
			case kTypeNote			: item= 8; break;

			case kTypeHtml			: item= 9; break;
			case kMailType			: item= 10; break;
			case kTypeTelnet   	: item= 11; break;
			case kTypeTn3270		: item= 12; break; // ?? or kTypeTelnet
			case kTypeMime			: item= 1; break;

			case kTypeCSO			  : item= 13; break;
			case kTypeWhois		  : item= 14; break;
			case kTypeBinhex	  : item= 15; break;
			case kTypeUuencode  : item= 16; break;

			default							: item= 17; break;
			}
#endif
		go2Popup( cKIND, item);
			
		go2CheckBox( ctran, go->fTransferType == kTransferBinary);
		go2CheckBox( cisGp, go->fIsPlus == kGopherPlusYes);

		go2EditText( ctype, Idtype2Str( go->fMacType));
		go2EditText( csire, Idtype2Str( go->fMacSire));
		//go2Button( cappn, GetComment);

#undef go2Popup			
#undef go2CheckBox
#undef go2NumbText
#undef go2EditText			
		}
}


	
void	DGopherEditView::Answers()
{
	DControl* cline= this;
	DGopher*  go= fGopher;
	
	if (cline && go) {
		DEditText * et;
		DEditText * nt;
		DCheckBox * cb;
		DPopupList * pop;
		DButton   * bt;
		char 			aLine[256];
		short			item;
			
#define EditText2Go(ID, MAPFUNC)		\
	if (NULL != (et= (DEditText*)cline->FindSubview(ID))) \
		{ et->GetTitle( aLine, 256); go->MAPFUNC(aLine); }
				
#define EditText2Golong(ID, VAR)		\
	if (NULL != (et= (DEditText*)cline->FindSubview(ID))) \
		{ et->GetTitle( aLine, 256); VAR = Str2Idtype(aLine); }

#define CheckBox2Go(ID, MAPFUNC)		\
	if (NULL != (cb= (DCheckBox*)cline->FindSubview(ID))) \
	  { go->MAPFUNC(cb->GetStatus()); }

		if (NULL != (pop= (DPopupList*)cline->FindSubview(cProto)))
			go->fProtocol= DGopher::GetProtoVal(pop->GetValue()-1);

		EditText2Go( cTITL, StoreName);
		EditText2Go( cPATH, StorePath);
		EditText2Go( cHOST, StoreHost);
		EditText2Go( cPORT, StorePort);
		CheckBox2Go( cisGp, StorePlus);

		EditText2Golong( ctype, go->fMacType);
		EditText2Golong( csire, go->fMacSire);
		if (NULL != (cb= (DCheckBox*)cline->FindSubview(ctran)))  {
			if (cb->GetStatus()) go->fTransferType= kTransferBinary;
			else go->fTransferType= kTransferText;
			}

		if (NULL != (pop= (DPopupList*)cline->FindSubview(cKIND)))
#if 1
			go->fType= DGopher::GetTypeVal(pop->GetValue()-1);
#else
			switch (pop->GetValue()) {
				case  1: go->fType= kTypeFile; break;
				case  2: go->fType= kTypeFolder; break;
				case  3: go->fType= kTypeQuery; break;
				case  4: go->fType= kTypeBinary; break;
				case  5: go->fType= kTypeImage; break;
				case  6: go->fType= kTypeSound; break;
				case  7: go->fType= kTypeMovie; break;
				case  8: go->fType= kTypeNote; break;
				case  9: go->fType= kTypeHtml; break;
				case 10: go->fType= kMailType; break;
				case 11: go->fType= kTypeTelnet; break;
				case 12: go->fType= kTypeTn3270; break;

				case 13: go->fType= kTypeCSO; break;
				case 14: go->fType= kTypeWhois; break;
				case 15: go->fType= kTypeBinhex; break;
				case 16: go->fType= kTypeUuencode; break;

				default:
				case 17: go->fType= kTypeError; break; 
				}
#endif

				// check URL last ?? so it can override other opts ??
		//EditText2Go( cURL, StoreURL);  
		if (NULL != (et= (DEditText*)cline->FindSubview(cURL))) { 
			char 	aURL[512];
			et->GetTitle( aURL, 512); 
			long urllen= StrLen( aURL);
			if ( urllen < 5 || StrCmp( aURL, fOldURL) == 0 ) 
				go->StoreURL( NULL);
			else {
				// go->StoreURL( aURL); // redundant
				DGopher* newgo= DGopherList::GopherFromURL( aURL, urllen, NULL, false);
				go->CopyGopher(newgo);
				}
			}
				

#undef EditText2Go							
#undef CheckBox2Go							
#undef EditText2Golong							
		 }
}

 










// DGoMapEditDlog ------------------------


enum DGoMapEditCmds {
	cHandID = 512, cGoplusKind,cHandType,cHandName,cHandSig,cTrans,cPref,
	cLocalType,cServerType,cListOld,
	cServerSuffixes
	};


DGoMapEditDlog::DGoMapEditDlog(DGopherMap* itsMapper, short itsForm, char* itsKind) :
	DWindow( 0, NULL, DWindow::fixed, -10, -10, -50, -20, NULL, kDontFreeOnClose),
	fView(NULL), fOtherKinds(NULL),
	fForm(itsForm),
	fMapper(itsMapper), fNewMapper(NULL)
{	
	if (fMapper) {
		//fMapper->newOwner(); //??
		//fForm= itsForm;
		switch (fForm) {
			case kFormLocal: this->SetTitle("Edit document type handler"); break;
			case kFormServerSuffixes: this->SetTitle("Edit server suffix mapping"); break;
			case kFormServerLocalMap: this->SetTitle("Edit server-local type mapping"); break;
			}
		}
	else {
		fForm= kFormNew; //kFormLocal;
		fNewMapper= new DGopherMap();  
		fMapper= fNewMapper;
		//fMapper->newOwner(); //??
		fMapper->fStatus= kAdded;
		this->SetTitle("New document type handler");
			// this one will trash current text handler ??
		//fMapper->SetHandlerType("Internal");
		//fMapper->SetID( gInternalHandlerIDs[kTextHandler]); // ??  if Internal, must be valid !?
		fMapper->SetHandlerType("External");
		fMapper->SetID( "abcd"); 
			// ??  if Internal, must be valid !?
		fMapper->SetKind( itsKind);
		fMapper->SetHandlerName("Application name");
		fMapper->SetPreference("1");
		fMapper->SetServerType("0");
		fMapper->SetLocalType("0");
		fMapper->SetSuffix(".txt");
		fMapper->SetMacTypeNSire("TEXT/ttxt");
		fMapper->SetServerSuffixes(".txt");
		fMapper->SetTransferBinary();
		}
}

DGoMapEditDlog::~DGoMapEditDlog()
{
	//if (fView) delete fView; //!!!!! NOOOOOOO we delete all subviews now !!!!!
	if (fNewMapper) fNewMapper->suicide();
	MemFree( fOtherKinds);
}

void DGoMapEditDlog::OkayAction()
{
	fView->Answers();
	if (fForm == kFormNew && fOtherKinds)  
		fMapper->SetKind( fOtherKinds);
	if (fMapper->fStatus == kNochange) 
		fMapper->fStatus= kChanged;
}

Boolean DGoMapEditDlog::IsMyAction( DTaskMaster* action) 
{	
	switch (action->Id()) {

		case cListOld: {
			DGopherMap * mp= NULL;
			DPopupList * pop = (DPopupList*) action;
			short val= pop->GetValue()-1;
			if (val) mp= gGopherMap->GopherMapAt( val-1); // -1 ??????
			else mp= fNewMapper;
			if (mp) {
				// !!!! MUST Fiddle w/ mapper::fKind so that caller's original itsKind isn't trashed
				char	buf[512];
				mp->fKind->GetAllNames( buf, sizeof(buf));
				if (fOtherKinds) MemFree(fOtherKinds);
				fOtherKinds= StrDup( buf);
				
				//fMapper->suicide(); //??
				fMapper= mp;
				//fMapper->newOwner(); //??
				fMapper->fStatus= kNochange;
				fView->fMapper= fMapper;
				fView->fInstallKind= false;
				fView->InstallControls();
				}
			return true;
			}

		default:
			return DWindow::IsMyAction(action);
		}
}

void DGoMapEditDlog::Open()
{
	DView* super;
	DEditText* et;
	DPrompt* pr;
	DCheckBox* ck;
	DPopupList* pop;
	Nlm_FonT	promptFont = Nlm_programFont; //Nlm_systemFont;
	Nlm_FonT	editFont = Nlm_programFont;
	Boolean		doListOld;
	
	doListOld= (fForm == kFormNew);
	//if (doListOld) fForm= kFormLocal;
	fView= new DGoMapEditView(0, this, fForm, fMapper, 50, 20, true, NULL);
	super= fView;

	pr= new DPrompt(0, super, "Document Kind:", 0, 0, promptFont);	 		
	super->NextSubviewToRight();
	et= new DEditText(cGoplusKind, super, "", 30, editFont);
	this->SetEditText(et);
	super->NextSubviewBelowLeft();
	
	if (doListOld) {
		pr= new DPrompt(0, super, "Handle as this kind:", 0, 0, promptFont);	 		
		super->NextSubviewToRight();
		pop= new DPopupList(cListOld, super, true);
		long i, n= gGopherMap->GetSize();
		pop->AddItem("New local kind");	
		for (i=0;i<n;i++) { 
			DGopherMap* mp= gGopherMap->GopherMapAt(i); 
			pop->AddItem((char*)mp->GetKind());	
			} 
		super->NextSubviewBelowLeft(); 
		
		}
		
	if (fForm == kFormLocal || fForm == kFormNew) {
		DCluster* clu= new DCluster( 0, super, 0, 0, false,"Edit local handler values");
		//clu->SetResize( DView::moveinsuper, DView::moveinsuper);
		super= clu;  
		pr= new DPrompt(0, super, "Local ID:", 0, 0, promptFont);	 		
		super->NextSubviewToRight();
		et= new DEditText(cHandID, super, "", 5, editFont);
		this->SetEditText(et);
		super->NextSubviewBelowLeft(); //NextSubviewToRight();

		pr= new DPrompt(0, super, "Preference for this kind:", 0, 0, promptFont);	 		
		super->NextSubviewToRight();
		et= new DEditText(cPref, super, "", 8, editFont);
		super->NextSubviewBelowLeft();
		
		ck= new DCheckBox(cTrans, super, "Use binary transfer");
		ck->SetStatus(true);
		super->NextSubviewBelowLeft();

		pr= new DPrompt(0, super, "Handler Type:", 0, 0, promptFont);	 		
		super->NextSubviewToRight();
		pop= new DPopupList(cHandType, super, true);
		pop->AddItem("External");	 
		pop->AddItem("Internal");	 
		pop->AddItem("None");	 
		super->NextSubviewBelowLeft(); 
	
		
		pr= new DPrompt(0, super, " For External handlers ----------", 0, 0, promptFont);	 		
		super->NextSubviewBelowLeft();
		 
		// Button for user file/appl get dialog...
		
		pr= new DPrompt(0, super, "Handler App:", 0, 0, promptFont);	 		
		super->NextSubviewToRight();
		et= new DEditText(cHandName, super, "", 30, editFont);
		this->SetEditText(et);
		super->NextSubviewBelowLeft();
	
	#ifdef WIN_MAC
		pr= new DPrompt(0, super, "File signature:", 0, 0, promptFont);	 		
	#else
		pr= new DPrompt(0, super, "File suffix:", 0, 0, promptFont);	 		
	#endif
		super->NextSubviewToRight();
		et= new DEditText(cHandSig, super, "", 9, editFont);
		this->SetEditText(et);
		super->NextSubviewBelowLeft();
		}
	
	else if (fForm == kFormServerSuffixes) {
		//pr= new DPrompt(0, super, "--- Mapping from server to local types ---", 0, 0, promptFont);	 		
		//super->NextSubviewBelowLeft();
	
		pr= new DPrompt(0, super, "Server suffixes to map:", 0, 0, promptFont);	 		
		super->NextSubviewToRight();
		et= new DEditText(cServerSuffixes, super, "", 20, editFont);
		this->SetEditText(et);
		}
		
	else if (fForm == kFormServerLocalMap) {
		//pr= new DPrompt(0, super, "--- Mapping from server to local types ---", 0, 0, promptFont);	 		
		//super->NextSubviewBelowLeft();

		pr= new DPrompt(0, super, "Server Gopher0 type to map:", 0, 0, promptFont);	 		
		super->NextSubviewToRight();
		et= new DEditText(cServerType, super, "", 2, editFont);
		super->NextSubviewBelowLeft();
		
		pr= new DPrompt(0, super, "to Local Gopher0 Type:", 0, 0, promptFont);	 		
		super->NextSubviewToRight();
		et= new DEditText(cLocalType, super, "", 2, editFont);
		this->SetEditText(et);
		super->NextSubviewBelowLeft();
		}
	
	this->AddOkayCancelButtons();
	fView->InstallControls();
	
	DWindow::Open();
}

 
 
// DGoMapEditView ------------------------

DGoMapEditView::DGoMapEditView(long id, DView* itsSuperior, short itsForm, DGopherMap* itsMapper,
			short width, short height, Boolean hidden, char* title) :
	DCluster( id, itsSuperior, width, height, hidden, title),
	fForm(itsForm), fInstallKind(true),
	fMapper(itsMapper)
{
}


DGoMapEditView::~DGoMapEditView()
{
}


void	DGoMapEditView::InstallControls()
{
	DControl* nextline= this;
	DGopherMap*  go= fMapper;

	if (go) {
		DEditText * et;
		DEditText * nt; //DNumberText
		DCheckBox * cb;
		DPopupList * pop;
		DButton   * bt;
		char   	  aLine[256];
		short			item;
		
#define go2EditText(ID, GOVAL)		\
{ if (NULL != (et= (DEditText*)nextline->FindSubview(ID))) \
	et->SetTitle( (char*)GOVAL); }

#define go2NumbText(ID, GOVAL)		\
{ if (NULL != (nt= (DEditText*)nextline->FindSubview(ID))) \
	sprintf(aLine, "%d", GOVAL); nt->SetTitle( aLine); }

#define go2CheckBox(ID, GOVAL)		\
{ if (NULL != (cb= (DCheckBox*)nextline->FindSubview(ID))) \
	cb->SetStatus( GOVAL ); }

#define go2Popup(ID, GOVAL)		\
{ if (NULL != (pop= (DPopupList*)nextline->FindSubview(ID))) \
	pop->SetValue( GOVAL ); }

	if (fInstallKind) go2EditText( cGoplusKind, go->GetKind());
	if (fForm != DGoMapEditDlog::kFormLocal
	  	&& NULL != (et= (DEditText*)nextline->FindSubview(cGoplusKind))) 
	  		et->Disable();
	  		
	if (fForm == DGoMapEditDlog::kFormLocal ||
		  fForm == DGoMapEditDlog::kFormNew) {
		go2EditText( cHandID, go->GetID());  
		go2EditText( cHandName, go->GetHandlerName());
		go2EditText( cPref, go->GetPreference());
		go2EditText( cHandSig, go->GetSuffix());
		
		switch (*(go->GetHandlerType())) {
			case 'L'	:
			case 'l'	:
			case 'e'	:
			case 'E'	: item= 1; break;
			case 'i'	:
			case 'I'	: item= 2; break;
			case 'n'	:
			case 'N'	: item= 3; break;
			default		: item= 1; break;
			}
		go2Popup( cHandType, item);
		go2CheckBox( cTrans, go->GetTransferBinary());
		}
	
	else if (fForm == DGoMapEditDlog::kFormServerSuffixes) {
		go2EditText( cServerSuffixes, go->GetServerSuffixes());
		}
	else if (fForm == DGoMapEditDlog::kFormServerLocalMap) {
		go2EditText( cServerType, go->GetServerType());
		go2EditText( cLocalType, go->GetLocalType());
		}

#undef go2Popup			
#undef go2CheckBox
#undef go2NumbText
#undef go2EditText			
		}
}


	
void	DGoMapEditView::Answers()
{
	DControl* cline= this;
	DGopherMap*  go= fMapper;
	
	if (cline && go) {
		DEditText * et;
		DEditText * nt;
		DCheckBox * cb;
		DPopupList * pop;
		DButton   * bt;
		char 			aLine[256];
		short			item;
			
#define EditText2Go(ID, MAPFUNC)		\
	if (NULL != (et= (DEditText*)cline->FindSubview(ID))) \
		{ et->GetTitle( aLine, 256); go->MAPFUNC(aLine); }
				
#define EditText2Golong(ID, VAR)		\
	if (NULL != (et= (DEditText*)cline->FindSubview(ID))) \
		{ et->GetTitle( aLine, 256); VAR = Str2Idtype(aLine); }

#define CheckBox2Go(ID, MAPFUNC)		\
	if (NULL != (cb= (DCheckBox*)cline->FindSubview(ID))) \
	  { go->MAPFUNC(cb->GetStatus()); }

	if (fForm == DGoMapEditDlog::kFormLocal 
		|| fForm == DGoMapEditDlog::kFormNew) {
		//if (go->fKind) { MemFree(go->fKind); go->fKind= NULL; } // SetKind bug, sets alt kind if fKind !
		EditText2Go( cGoplusKind, SetKind);
		EditText2Go( cHandID, SetID);
		EditText2Go( cHandName, SetHandlerName);
		EditText2Go( cPref, SetPreference);
		EditText2Go( cHandSig, SetSuffix);
		CheckBox2Go( cTrans, SetTransferBinary);

		if (NULL != (pop= (DPopupList*)cline->FindSubview(cHandType)))
			switch (pop->GetValue()) {
				case  1: go->SetHandlerType("External"); break;
				case  2: go->SetHandlerType("Internal"); break;
				default:
				case  3: go->SetHandlerType("None"); break;
				}
		}			
	else if (fForm == DGoMapEditDlog::kFormServerSuffixes) {
		EditText2Go( cServerSuffixes, SetServerSuffixes);
		}
	else if (fForm == DGoMapEditDlog::kFormServerLocalMap) {
		EditText2Go( cServerType, SetServerType);
		EditText2Go( cLocalType, SetLocalType);
		}
		
#undef EditText2Go							
#undef CheckBox2Go							
#undef EditText2Golong							
		 }
}

 
