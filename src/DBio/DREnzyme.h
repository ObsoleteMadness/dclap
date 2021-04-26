// DREnzyme.h
// d.g.gilbert

#ifndef _DREENZYME_
#define _DREENZYME_
	
#include <DObject.h>

class DSequence;
class DList;


#if 0
class DTableClass 
{
public:
	enum  { kUnread, kOkay, kNodata };
	short fState;
	char * fType,* fSection, * fDefaultvalue;

	virtual Nlm_Boolean NotAvailable();
	virtual void  Initialize(char* type, char* section, char* defaultvalue);
	virtual void  ReadTable( char* tableFile);
	virtual void  ReadTable( DFile* aFile) = 0;
	virtual void  TableChoice() = 0;
};

#endif


class	DREnzyme : public DObject 
{
public:
	enum {
		kMaxSite = 64, //?? what is max site size
		kCut3EqualsCutpoint = -1  //REnzyme fCut3 flag 
		};
		
	char*			fName			;		// ? maxlen=10 chars?; /sort
	char*			fSite			; 	//recognition site in nuc codes 
	char*			fVendors	; 	//vendor codes, max 63?
	short			fCutpoint	;		//cut point from start of fSite 
	short			fCut3from5;		//diff of 3' from 5' cutpoint 
	long			fCutcount	;		//temp use w/ REMap of seq/sort
	char*			fCoSite		; 	//rev-compl. of fSite, if needed
	short			fCoCutpoint; 	//cut point from start of fCoSite 
	
	DREnzyme();
	virtual ~DREnzyme();
	virtual Boolean Parse(char* line);
};

class	DREnzymeVendor : public DObject
{
public:
	char		fCode;
	char*		fName;
	
	DREnzymeVendor();
	virtual ~DREnzymeVendor();
	virtual Boolean Parse( char* line);
};


struct DRECutsItem {
	short			fSeqIndex ;
	DREnzyme*	fREnzyme; 
};
	
//typedef RECutsItem 	RECutsList[1];
//typedef RECutsList	*RECutsPtr;
	
class	DREMap : public DObject
{
public:
	enum  { kUnread, kOkay, kNodata };
	static short fState;
	static char 	* fType, * fSection, * fDefaultvalue;
	static DList	* fREnzymes;					// of DREnzyme ?? same as gREnzymes
	static DList	* fREnzymeVendors ; 	// of DREnzymeVendor

	static  void FreeEnzymeList();
	static  Nlm_Boolean NotAvailable();
	static	void Initialize(char* type = "renzyme", char* section = "data",
					 					 		  char* defaultvalue = "tables:renzyme.table");
	static  void ReadTable( char* tableFile);  
	static  void ReadTable(DFile* aFile);
	static  void TableChoice();
	
	DSequence 	* fSeq, * fCoSeq;  //Reverse+Complement of fSeq
	DRECutsItem * fSeqCuts;  			 
	long	fMaxcuts, fCutcount;
	short	fCuttersCount; 			//# zymes that cut
	char	fTargetbase;
	
	DREMap();
	virtual ~DREMap(); 
	//DObject* Clone(); 

	void FreeTempData(); 
	void MapSeq( DSequence* aSeq);
	void CutsAtBase( short atBase, short& firstCut, short& nCuts);

private:
	void HuntCut( short n, short x, short& jlo);
	long QuickSearch( Nlm_Boolean first, char* target, long targlen, 
										char* source, long sourcelen);
	void SearchStrand( DSequence* bSeq, char* aTarget, char* sourcebits, long sourcelen,
											DREnzyme* zyme, short cutAdd, long& zymecuts);
	void FindCuts( DREnzyme* zyme, char* sourcebits, long sourcelen);
};




struct CodonStat
{
	char 	codon[4]; 		//3 letter nucleic codon
	char	amino;			 	//1 letter amino name
	float numPerK; 			//frequency in 10000
	
	CodonStat( char* codn = "---", char ami = 'N', float numk = 0);
};

class DCodons 
{
public:
	enum  { kUnread, kOkay, kNodata };
	static short fState;
	static char * fType,* fSection, * fDefaultvalue;
	static short fStartcodon;
	static CodonStat *fCodons;

	static Nlm_Boolean NotAvailable();
	static void  Initialize(char* type = "codon", char* section = "data",
					 					 		  char* defaultvalue = "tables:codon.table");
	static void  ReadTable( char* tableFile);
	static void  ReadTable( DFile* aFile);
	static void  TableChoice();
	
	static Boolean badindex(short i) { return (fState != kOkay || i<0 || i>63); }
	static char* codon(short i)   { if (badindex(i)) return "   "; else return fCodons[i].codon; }
	static char  amino(short i)   { if (badindex(i)) return ' '; else return fCodons[i].amino; }
	static float numPerK(short i) { if (badindex(i)) return 0; else return fCodons[i].numPerK; }
	static const char* FindBestCodon( char matchamino);
	static char* startcodon() { 
		if (badindex(fStartcodon)) return "ATG"; 
		else return fCodons[fStartcodon].codon; 
		}
	static char  startamino() { 
		if (badindex(fStartcodon)) return 'M'; 
		else return fCodons[fStartcodon].amino; 
		}
};


typedef unsigned long baseColors['~'-' '+1];

// ?? turn this into a generic class for reading data files ??
class DBaseColors
{
public:
	enum  { kUnread, kOkay, kNodata };
	static short fState;
	static char * fType,* fSection, * fDefaultvalue;
	static baseColors gAAcolors;
	static baseColors gNAcolors;

	static Nlm_Boolean NotAvailable();
	static void  Initialize(char* type = "color", char* section = "data",
					 					 		  char* defaultvalue = "tables:color.table");
	static void  ReadTable( char* tableFile);
	static void  ReadTable( DFile* aFile);
	static void  TableChoice();
	
	static void  InitColors();
};



class DSeqStyle : public DObject {
public:
	enum { kFramenone, kFramebox, kFrameoval, kFramerrect };
	enum { kFrameSolid, kFrameDotted, kFrameDashed, kFrameDark, kFrameMedium, kFrameLight };
	char	* name, * description;
	char	* fontname;
	short		fontsize;
	Nlm_FonT	font;
	Boolean	bold:1;
	Boolean	italic:1;
	Boolean	uline:1;
	Boolean	uppercase:1;
	Boolean	lowercase:1;
	Boolean dofontpat:1;
	Boolean	dofontcolor:1;
	Boolean	dobackcolor:1;
	Boolean	doframecolor:1;
	Boolean	invertcolor:1;
	ulong		fontcolor;
	ulong		backcolor;
	ulong		framecolor;
	short		framestyle;
	char		repeatchar;
	char		fontpattern[8];
	short		frame;

	DSeqStyle();
	virtual ~DSeqStyle(); // delete font, fontname
	void GetFont();
	void ClearDrawing();
	DObject* Clone();
};

class DStyleTable 
{
public:
	enum  { kUnread, kOkay, kNodata };
	static short fState;
	static char * fType,* fSection, * fDefaultvalue;
	static DList* fStyles;  // list of DSeqStyle
	static DSeqStyle fLaststyle;
	static char		fLastch;
	static char * fToprow;
	static long	  fToprowlen;
	
	static Nlm_Boolean NotAvailable();
	static void  Initialize(char* type = "seqmasks", char* section = "data",
					 					 		  char* defaultvalue = "tables:seqmasks.table");
	static void ReadTable( char* tableFile);
	static void ReadTable( DFile* aFile);
	static void TableChoice();
	static void StartDraw(char* toprowbases, long maxrow);
	static void EndDraw();
	static void DrawBaseWithStyle( char ch, long baseindex, short maskval, 	
											  						Nlm_RecT&	crec, short atrow);
};

	
#endif

