// DREnzyme.cp
// d.g.gilbert


#include <ncbi.h>
#include <dgg.h>
#include <DFile.h>
#include <DList.h>
#include <DApplication.h>
#include <DWindow.h>
#include <DControl.h>
#include <DPanel.h>
#include <DUtil.h>
#include "DSequence.h"
#include "DREnzyme.h"
#include <DDialogText.h>

#define EDITTABLE



class DREnzymeGlobals {
public:
   DREnzymeGlobals() 
   	{ 
   	DCodons::Initialize();
   	//DREMap::Initialize();
   	}
};

static DREnzymeGlobals	globals; // initializes globals here
extern Nlm_FonT  Nlm_fontInUse;
extern "C" void Nlm_TextEnableNewlines(Nlm_TexT t, Nlm_Boolean turnon);


 
	// ?? use hash table for codons, other table lookups !?
static long Hash( char	*s)
{
	register char	c;
	register long	val = 0;
	while ((c = *s++) != '\0') val += (int) tolower(c);
	return (val);
}


// class DLocateFile

class DLocateFile : public DWindow {
public: 
	char * fName;
	DLocateFile(char* fname, char* desc);	
	~DLocateFile();
	void OkayAction();
};
		
		
DLocateFile::DLocateFile(char* format, char* name) :
	DWindow( 0, gApplication, fixed, -10, -10, -50, -20, "Locate data", kDontFreeOnClose)
{
	char buf[512];
	DView* super = this;

	fName= StrDup(name);
	sprintf(buf, format, name);
	new DNotePanel(0, super, buf, 280, 50);	
	this->NextSubviewBelowLeft();
	new DPrompt( 0, super, "Would you like to locate it?", 280);
	this->NextSubviewBelowLeft();
	this->AddOkayCancelButtons(cOKAY,"Locate",cCANC,"Cancel");
}

DLocateFile::~DLocateFile()
{
	MemFree( fName);
}

void DLocateFile::OkayAction() 
{ 
	DWindow::OkayAction();
	char * suffix= (char*) DFileManager::FileSuffix( fName);
	char * name= (char*) DFileManager::GetInputFileName( suffix, "TEXT");
	MemFree( fName);
	fName= StrDup( name);
}



// class DReplaceFileDlog

class DReplaceFileDlog : public DWindow {
public: DReplaceFileDlog(char* title);
};

DReplaceFileDlog::DReplaceFileDlog(char* title) :
	DWindow( 0, NULL, DWindow::modal, -10, -10, -50, -20, "", kDontFreeOnClose) // DWindow::fixed
{
	char str[128];
	if (title && *title)  
		sprintf( str, "Replace file '%s'?", title);
  else
	  StrCpy( str, "Replace this file?");
	new DPrompt( 0, this, str, 0, 0, Nlm_systemFont);	 		
	this->AddOkayCancelButtons(cOKAY,"Yes",cCANC,"No");	
	DWindow::Open();
}




// class DTableChoiceDialog

class DTableChoiceDialog : public DWindow {
public: 
	enum { 
		cSetDefault = 3225, cChoose, cSave,
		kEditNone= 0, kEditOpen, kEditChanged
		};
	DPrompt	* fName;
	DFile	* fFile;
	Boolean fSetPref;
	short fEditData;
	ulong fEditLen;
	long	fEditChk;
	DDialogScrollText* fText;
	
	DTableChoiceDialog(char* title, char* desc=NULL, char* curfile = NULL);	
	~DTableChoiceDialog();		
	Nlm_Boolean IsMyAction(DTaskMaster* action);
	void OkayAction();
	Boolean OpenText();
};
		
	
DTableChoiceDialog::DTableChoiceDialog(char* title, char* desc, char* curfile) :
	DWindow( 0, gApplication, fixed, -10, -10, -50, -20, title, kDontFreeOnClose),
	fFile(NULL), fName(NULL), fSetPref(false), fEditData(kEditNone), fEditChk(0),
	fText(NULL)
{
	enum { cluwidth= 430 };
	DCluster* clu;
	DView* super;

	if (curfile) fFile= new DFile(curfile,"r"); // in case user wants default !

	if (desc) {
		char buf[128];
		sprintf(buf, "%s data", title);
		clu= new DCluster( 0, this, 0, 0, false, buf);   
		if (clu) super= clu; else super= this;
#ifdef EDITTABLE
		DDialogScrollText* dt= new DDialogScrollText( 0, super, 50, 15, gTextFont, false);
		fText= dt;
		//Nlm_TextEnableNewlines( dt->fText, true);
		if (!OpenText()) {
			fText->SetText( desc);  
			fText->Disable();// make fText un-editable...
			sprintf(buf, "%s format", title);
			clu->SetTitle( buf);
			}
#else
		new DNotePanel(0, super, desc, cluwidth, 120);	
#endif
		this->NextSubviewBelowLeft();
		}

	clu= new DCluster( 0, this, 0, 0, false, "Current table");   
	if (clu) super= clu; else super= this;
	fName= new DPrompt( 0, super, curfile, cluwidth);
	this->NextSubviewBelowLeft();

	new DButton( cChoose, this, "Choose file");
	this->NextSubviewToRight();
	new DCheckBox( cSetDefault,this,"Make this default table");
	this->NextSubviewBelowLeft();
	this->AddOkayCancelButtons(cOKAY,"Okay",cCANC,"Cancel");
}

DTableChoiceDialog::~DTableChoiceDialog()
{
	if (fFile) delete fFile;
}

Boolean DTableChoiceDialog::OpenText() 
{
	Boolean okay= false;
	fEditData= kEditNone;
			// use "rb" to avoid newline translation !!
	if (fText && fFile->Open("rb") == 0) {
		ulong len= fFile->LengthF();
		
				// check for overly long files!? (10K is big...)
				// REnzyme.table is too big !! (>32k)
		if (len>10000) return false; // quick fix...
		
		char* buf= (char*) MemNew( len+1);
			// Damn codewarrior stdio is translating mac newline to unix newline here!!
		fFile->ReadData( buf, len);
		fFile->Close();
		buf[len]= 0;
		fEditData= kEditOpen;
		fEditLen= len;
		fEditChk= Hash( buf);
		
		fText->SetText( buf);  
		fText->Enable(); 
		MemFree( buf);
		okay= true;
		}
	return okay;
}


Nlm_Boolean DTableChoiceDialog::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {
		case cSave:
			return true;
			
		case cChoose:
			if (fFile) delete fFile;
			if (gApplication->ChooseFile( fFile, NULL, "TEXT")) { 
			  fName->SetTitle( (char*)fFile->GetName());
#ifdef EDITTABLE
			  if (!OpenText()) ;
#else
			  gApplication->OpenDocument( fFile); // view it ??
#endif
			  }
			return true;
			
		default:
			return DWindow::IsMyAction(action);	
		}
}



void DTableChoiceDialog::OkayAction() 
{ 
	DWindow::OkayAction();
	DCheckBox* ck = (DCheckBox*) this->FindSubview(cSetDefault);
	if (ck && ck->GetStatus() && fFile && fFile->Exists()) {
		fSetPref= true; 
		}
		
#ifdef EDITTABLE
	if (fFile && fText && fEditData >= kEditOpen) {
		Boolean okay= true;
		char *buf= fText->GetText();
		ulong len= StrLen(buf);
		if (len == fEditLen) {
			long check= Hash( buf);
			if (check == fEditChk) return;
			}
		// ask user if s/he wants to save it...
		if (fFile->Exists()) {
			DReplaceFileDlog* dlog= new DReplaceFileDlog( (char*)fFile->GetName());
			okay= dlog->PoseModally();
 			delete dlog;  
 			if (!okay) {
				//if (fFile) delete fFile;
				const char* newname= gFileManager->GetOutputFileName(fFile->GetShortname());
				if (newname) { 
					fFile->SetName( newname);
					//if (fFile->Exists()) fFile->Delete(); //??
				  //fName->SetTitle( (char*)fFile->GetName());
				  //if (!OpenText()) ;
				  }
			  }
			}
		if (okay) {
			//fFile->Delete(); // need this !?
			if (fFile->Exists()) {
				char newname[512];
				StrNCpy(newname, fFile->GetName(), sizeof(newname));
				DFileManager::ReplaceSuffix( newname, sizeof(newname), ".old");
				DFileManager::Rename( fFile->GetName(), newname);
				}
			fFile->Open("wb");
			fFile->WriteData( buf, len);
			fFile->Close();
			}
		}
#endif		
}


	
inline char* FlushLine( char* cp)
{
	char * ep;
	while (*cp  && *cp <= ' ') cp++;
	ep= StrChr(cp, '\0') - 1;
	while (ep >= cp && *ep <= ' ') ep--;
	ep[1]= 0;
	//ep= StrChr(cp, ' '); if (ep) *ep= 0;
	return cp;
}




#if 0
// this isn't useful till we revise subclasses to be non-statics		 
// class DDataTable	

Nlm_Boolean DTableClass::NotAvailable()
{
	if (fState == kUnread) {
		char * tablefile= gApplication->GetFilePref( fType, fSection, fDefaultvalue);
		if (tablefile) ReadTable( tablefile);
		else fState= kNodata;
		if (fState != kOkay) Message(MSG_OK,"Could not read table file '%s'",tablefile);
		}
	return (fState != kOkay);
}

void DTableClass::Initialize(char* type, char* section, char* defaultvalue)
{
	fState= kUnread;
	fType= type;
	fSection= section;
	fDefaultvalue= defaultvalue;
}

void DTableClass::ReadTable( char* tableFile)
{
 	DFile aFile( tableFile, "r");
	ReadTable( &aFile);
}

//void DTableClass::TableChoice() = 0;
//void DTableClass::ReadTable(DFile* aFile) = 0

#endif





	
// struct CodonStat	
	
CodonStat::CodonStat( char* codn, char ami, float numk):
	amino(ami), numPerK( numk)
{
	if (!codn) codn= "---";
	codon[0]= codn[0]; 
	codon[1]= codn[1]; 
	codon[2]= codn[2]; 
	codon[3]= codn[3]; 
}	 
		 
		 
// class DCodons	

short DCodons::fState= DCodons::kUnread;
short DCodons::fStartcodon= -1;
CodonStat* DCodons::fCodons= NULL;
char* DCodons::fType= "codon";
char* DCodons::fSection= "data";
char* DCodons::fDefaultvalue= "tables:codon.table";

Nlm_Boolean DCodons::NotAvailable()
{
	if (fState == kUnread) {
		char * tablefile= gApplication->GetFilePref( fType, fSection, fDefaultvalue);
		if (tablefile) {
			ReadTable( tablefile);
			if (fState != kOkay) {
				// dialog - would you like to locate data?
				DLocateFile* win= new DLocateFile("Could not read table file '%s'", tablefile);
				if (win->PoseModally()) {
					ReadTable( win->fName);
					if (fState == kOkay)
						gApplication->SetPref( win->fName, fType, fSection); //??
					}
				delete win;
				}
			}
		else 
			fState= kNodata;
		if (fState != kOkay)
			 Message(MSG_OK,"Could not read table file '%s'",tablefile);
		MemFree( tablefile);
		}
	return (fState != kOkay);
}

void DCodons::Initialize(char* type, char* section, char* defaultvalue)
{
	fState= kUnread;
	fType= type;
	fSection= section;
	fDefaultvalue= defaultvalue;
	if (fCodons==NULL) fCodons= (CodonStat*) MemNew( 64 * sizeof(CodonStat));
}

const char* DCodons::FindBestCodon( char matchamino)
{
	//shorten this x64 loop w/ sorted CodonTable sorted by most freq form/aa 
	short k, bestk= -1;
	float bestNumPerK= 0;
	Boolean nomatch;
	
	for (k= 0, nomatch= true; k<64 && nomatch; k++) {
		if (fCodons[k].amino == matchamino) {
		 	if (bestk<0 || fCodons[k].numPerK > bestNumPerK) {
				bestk= k;
				bestNumPerK= fCodons[k].numPerK;
				}
			}
		else if (fCodons[k].amino > matchamino) {
			nomatch= false;
			}
		}
	if (bestk<0) return "NNN";  
	else return fCodons[bestk].codon;
}



void DCodons::TableChoice()
{
	char* title= "Codon Table";
	char* desc = 
"  gcg style, '..' signals start of data"LINEEND
"AmAcid  Codon     Number    /1000     Fraction   .."LINEEND
"Gly     GGG     1743.00      9.38      0.13"LINEEND
"Gly     GGA     1290.00      6.94      0.09"LINEEND
;
	char * tablefile= gApplication->GetFilePref( fType, fSection, fDefaultvalue);
	
	DTableChoiceDialog* win= new DTableChoiceDialog(title, desc, tablefile);
	if (win->PoseModally()) {
		ReadTable( win->fFile);
		if (fState != kOkay) {
			Message(MSG_OK, "Table wasn't properly loaded.  Please try again");
			}
		else if (win->fSetPref)
			gApplication->SetPref((char*)win->fFile->GetName(), fType, fSection);
		}
	MemFree( tablefile);
	delete win;
}


void DCodons::ReadTable( char* tableFile)
{
 	DFile aFile( tableFile, "r");
	ReadTable( &aFile);
	//delete aFile;
}

#ifdef WIN_MSWIN
static int LIBCALLBACK
#else
static int
#endif
aminosort(void* a, void* b)
{
	return ( ((CodonStat*)a)->amino -  ((CodonStat*)b)->amino);
}

void DCodons::ReadTable(DFile* aFile)
{
/*------ file format for codon table (gcg style)
AmAcid  Codon     Number    /1000     Fraction   ..
         1         2         3         4   
12345678901234567890123456789012345678901234567890
Gly     GGG     1743.00      9.38      0.13
Gly     GGA     1290.00      6.94      0.09
------*/
	const	short kLinemax= 512;
 	Boolean done= FALSE;
	char		amino3[5], codon[5], startc;
	char		line[kLinemax];
	short		na;
	float		number, numPerK, frac;
	
	//fState= kNodata;
	if (!aFile || !aFile->Exists()) return;
	aFile->OpenFile();
	do {
		aFile->ReadLine( line, kLinemax);
		done= (StrStr(line,"..") != NULL);
	} while (!done && !aFile->Eof());
	
	done= FALSE;
	startc= 0;
	na= 0; 
	while (!done && !aFile->Eof()) {
		number= -1;
		aFile->ReadLine( line, kLinemax); // eat line
		if (*line > ' ') {
			sscanf( line, "%3s %3s %f %f %f", &amino3, &codon, &number, &numPerK, &frac);
			}
		if (number > 0) {
					// use GCG practice of signifying start codon w/ all lowercase
			fCodons[na].amino= DSequence::Amino321( amino3);
		  if (islower(amino3[0]) && islower(amino3[1]) && islower(amino3[2])) 
				startc= fCodons[na].amino;
			codon[0]= toupper(codon[0]);
			codon[1]= toupper(codon[1]);
			codon[2]= toupper(codon[2]);
			StrNCpy( fCodons[na].codon,codon,4);
			fCodons[na].numPerK= numPerK;
			na++;
			}
		done= na>= 64;
		}
	
	if (done) {
		Nlm_HeapSort(fCodons, 64, sizeof(CodonStat), aminosort);
		if (!startc) startc= 'M'; // Met is usual start codon
		for (short i= 0; i<64; i++) 
		  if (fCodons[i].amino == startc) { fStartcodon= i; break; }
		fState= kOkay;
		}
	else fState= kNodata;
}





// class DREnzyme


DREnzyme::DREnzyme()
{
	fName= NULL;
	fSite= NULL;
	fCoSite= NULL;
	fVendors= NULL;
	fCutpoint = -1;
	fCoCutpoint= -1;
	fCut3from5 = 0;
	fCutcount= 0;
}

DREnzyme::~DREnzyme()
{
	MemFree( fName);
	MemFree( fSite);
	MemFree( fCoSite);
	MemFree( fVendors);
}

	
Boolean DREnzyme::Parse( char* line)
/*----
[Name			Cut  Site					? !   Isoschizomers?					>vendors ]
;AatI      3 AGG'CCT        0 !  Eco147I,StuI                 >OU
AatII      5 G_ACGT'C      -4 !		       >EGJLMNOPRSUVX
AccI       2 GT'mk_AC       2 !		       >ABDEGIJKLMNOPQRSUVXY
;AccII     2 CG'CG          0 !  Bsp50I,BstUI,MvnI,ThaI      >DEGJKQVXY

? skip lines starting w/ ;
----*/
{
	//assume caller has eliminated blank lines 
	short		i, k, l, len, sitel;
	char		sitebuf[kMaxSite];
	
	if (*line == ';')  return false;
	fCutpoint= 0;
	fCut3from5= 0;

	i= 0; 
	len= StrLen(line);
	while (i<len && line[i]<=' ') i++;
	k= i;
	while (i<len && line[i]>' ') i++;
	l= i-k;
	fName= (char*) MemNew(l+1);
	StrNCpy( fName, line+k, l); 
	fName[l]= 0;
	
		//get cutpoint ?
	while (i<len && line[i]<=' ') i++;
	k= i;
	while (i<len && line[i]>' ') i++;
	l= i-k;
	// word= copy(line,k,l); StringToNumber(word, long); fCutpoint= long; 
	fCutpoint= atol(line+k);
	
		// get site && cutpoint 
	while (i<len && line[i]<=' ') i++;	
	k= i;
	for (l= 0; i<len && line[i]>' ' && l<kMaxSite; i++) {
		if (isalpha(line[i])) sitebuf[l++]= line[i];
		}
	sitel= l;
	sitebuf[sitel]= 0;
	fSite= (char*) MemNew(sitel+1);
	MemCpy(fSite, sitebuf, sitel+1);
	
		//skip fCut3from5 value ?
	while (i<len && line[i]<=' ') i++;
	k= i;
	while (i<len && line[i]>' ') i++;
	//l= i-k;
	fCut3from5= atol(line+k);

	//Boolean unsym= (fCut3from5 != 0 || sitel & 1);
	if (1) {
#if 1
		long i;
		char* aCoSite= (char*) MemNew(sitel+1);  
		for (i=0; i<sitel; i++) aCoSite[sitel-i-1]= fSite[i];
		aCoSite[sitel]= 0;
			// ?? do complement before or after strcmp ?? likewise, reverse when?
		DSequence::NucleicComplement( false, aCoSite, aCoSite, sitel);
		if ( StringCmp(aCoSite, fSite) != 0) fCoSite= aCoSite;
		else MemFree(aCoSite); 
#else
		long i, j;
		char* aCoSite= (char*) MemNew(sitel+1);  
		for ( i=0; i<sitel; i++) aCoSite[i]= fSite[i];
		aCoSite[sitel]= 0;
			// ?? do complement before or after strcmp ?? likewise, reverse when?
		DSequence::NucleicComplement( false, aCoSite, aCoSite, sitel);
		if ( StringCmp(aCoSite, fSite) != 0) {
			for (i=0, j= sitel-1; i<j; i++, j--) {
				char c= aCoSite[i];
				aCoSite[i]= aCoSite[j];
				aCoSite[j]= c;
				}
			aCoSite[sitel]= 0; // be sure ...
			fCoSite= aCoSite;
			}
		else MemFree(aCoSite); 
#endif

#if 1
		fCoCutpoint= sitel - (fCutpoint + fCut3from5); 
#else
		if (fCut3from5) fCoCutpoint= sitel - (fCutpoint + fCut3from5); 
		else fCoCutpoint= fCutpoint; // ?????? 
#endif
		}
	
	
		//skip past "!"
	while (i<len && line[i] != '!') i++;	
	i++;
		
		//get Isoschizomers? -- not all have them! ?
	while (i<len && line[i]<=' ') i++;
	if (line[i] != '>') {
		k= i;
		while (i<len && line[i]>' ') i++;
		l= i-k;
			//skip past ">"
		while (i<len && line[i] != '>') i++;
		}
	i++;
	
		//get vendors  
	while (i<len && line[i]<=' ') i++;
	k= i;
	while (i<len && line[i]>' ') i++;
	l= i-k; //Min(63,i-k);
	fVendors= (char*) MemNew(l+1);
	StrNCpy( fVendors, line+k, l); 
	fVendors[l]= 0;
	
	return true;
}
		


// class DREnzymeVendor


DREnzymeVendor::DREnzymeVendor()
{
	fCode = '!';
	fName	= NULL;
}

DREnzymeVendor::~DREnzymeVendor()
{
	if (fName) MemFree( fName);
}

Boolean DREnzymeVendor::Parse( char* line)
// e.g.,  "A	Amersham (4/91)" 
{
	//assume caller has eliminated blank lines & flushed blanks from front & back 
#if 1
	char *cp= line;
	fCode= *line;
	line++;
	while (*line && *line<=' ') line++;
	fName= StrDup( line);
#else
	short		i, j, k, l, len;
	len= StrLen(line);
	for (i=0; i<len && line[i]<=' '; i++) ;
	fCode= line[i];
	i++;
	while (i<len && line[i]<=' ') i++;
	l= len-i;
	fName= (char*) MemNew(l+1);
	StrNCpy( fName, line+i, l); 
	fName[l]= 0;
#endif

	return true;
}









// DREMap ---------------------------------
		

short DREMap::fState= DREMap::kUnread;
char* DREMap::fType= "renzyme";
char* DREMap::fSection= "data";
char* DREMap::fDefaultvalue= "tables:renzyme.table";
DList	* DREMap::fREnzymes = NULL ;				// of DREnzyme ?? same as gREnzymes
DList	* DREMap::fREnzymeVendors = NULL ; 	// of DREnzymeVendor
	
	
//static
void DREMap::Initialize(char* type, char* section, char* defaultvalue)
{
	fREnzymes= NULL; //?
	fREnzymeVendors= NULL; //?
	fState= kUnread;
	fType= type;
	fSection= section;
	fDefaultvalue= defaultvalue;
}


 
DREMap::DREMap()
{
	fSeq= NULL;
	fCoSeq= NULL;
	fSeqCuts= NULL;
	fCutcount= 0;
	fCuttersCount= 0;
	fType= "renzyme";
	fSection= "data";
	fDefaultvalue= "tables:renzyme.table";
}

DREMap::~DREMap()  
{
	FreeTempData();
	//FreeEnzymeList(); 
}



//static
void DREMap::FreeEnzymeList()
{
	fREnzymes = FreeListIfObject(fREnzymes); 
	fREnzymeVendors = FreeListIfObject(fREnzymeVendors); 
}

void DREMap::FreeTempData() 
{
	fSeq= NULL;
	delete fCoSeq; fCoSeq= NULL;
	MemFree(fSeqCuts); fSeqCuts= NULL;
	fCutcount= 0;
	fCuttersCount= 0;
}

//static
Nlm_Boolean DREMap::NotAvailable()
{
	if (fState == kUnread) {
		char * tablefile= gApplication->GetFilePref( fType, fSection, fDefaultvalue);
		if (tablefile) {
		  ReadTable( tablefile);
			if (fState != kOkay) {
				// dialog - would you like to locate data?
				DLocateFile* win= new DLocateFile("Could not read table file '%s'", tablefile);
				if (win->PoseModally()) {
					ReadTable( win->fName);
					if (fState == kOkay)
						gApplication->SetPref( win->fName, fType, fSection); //??
					}
				delete win;
				}
			}
		else fState= kNodata;
		if (fState != kOkay) Message(MSG_OK,"Could not read table file '%s'",tablefile);
		MemFree( tablefile);
		}
	return (fState != kOkay);
}

//static
void DREMap::TableChoice()
{
	char* title= "R.Enzyme Table";
	char* desc = 
"    gcg format, '..' signals start of data"LINEEND
"REBASE abbreviations for commercial sources of restriction enzymes"LINEEND
"                A        Amersham (5/93)"LINEEND
"                Y        P.C. Bio (9/91)"LINEEND
"[Name			Cut  Site		 Cut3-5 !   Isoschizomers?     >vendors]"LINEEND
".."LINEEND
";AatI      3 AGG'CCT        0 !  Eco147I,StuI         >OU"LINEEND
"AatII      5 G_ACGT'C      -4 !                       >ELMNOPRSUX"LINEEND
"AccI       2 GT'mk_AC       2 !                       >ABDEGKLMNOPQRSUX"LINEEND
";AccII     2 CG'CG          0 !  BstUI,MvnI,ThaI,Bsh1236I  >KQX"LINEEND
;
	char * tablefile= gApplication->GetFilePref( fType, fSection, fDefaultvalue);

	DTableChoiceDialog* win= new DTableChoiceDialog(title, desc, tablefile);
	if (win->PoseModally()) {
		ReadTable( win->fFile);
		if (fState != kOkay) {
			Message(MSG_OK, "REnzyme table wasn't properly loaded.  Please try again");
			}
		else if (win->fSetPref)
			gApplication->SetPref((char*)win->fFile->GetName(), fType, fSection);
		}
	MemFree( tablefile);
	delete win;
}

//static
void DREMap::ReadTable( char* tableFile)
{
 	DFile aFile( tableFile, "r");
	ReadTable( &aFile);
	//delete aFile;
}

//static
void DREMap::ReadTable(DFile* aFile)  
/*------ file format for Restriction Enzyme table (REbase in wisconsin/GCG format)
blah, blah

Commercial sources of restriction enzymes are abbreviated as follows:
 
		A	Amersham (4/91)
		:		:
 		Y	P.C. Bio (9/91)
 
[Name			Cut  Site		 Cut3-5 !   Isoschizomers?					>vendors \newline]
..
;AatI      3 AGG'CCT        0 !  Eco147I,StuI                      >OU
AatII      5 G_ACGT'C      -4 !		 >EGJLMNOPRSUVX
AccI       2 GT'mk_AC       2 !		 >ABDEGIJKLMNOPQRSUVXY
;AccII     2 CG'CG          0 !  Bsp50I,BstUI,MvnI,ThaI            >DEGJKQVXY
	:						:
----------------*/
{
	const	short kLinemax = 512;
	char				line[kLinemax+1];
	Boolean 		done, gotfirst;
	char	* cp;
 
	//fState= kNodata;
 	if (!aFile || !aFile->Exists()) return;
 	
	FreeEnzymeList();
	fREnzymeVendors = new DList(); // of StringHandle 
	fREnzymes	= new DList();	// of REnzymeHandle 

	aFile->OpenFile();
	done= FALSE;			
	gotfirst= FALSE;
	while (!done && !aFile->Eof()) {
		aFile->ReadLine( line, kLinemax);
		done= StrStr(line, "sources of restriction enzymes") != NULL;
		if (!done) {
			gotfirst= TRUE;
			done= StrStr(line, "Amersham") != NULL;  //vendor, maybe key line was changed...
			}
		}

		// Read vendor codes 
	for (done= false; !done && !aFile->Eof(); ) {
		if (!gotfirst) aFile->ReadLine( line, kLinemax);
		gotfirst= false;
		cp= FlushLine(line);
		if (*cp) {
			DREnzymeVendor* rev = new DREnzymeVendor();
			if (rev->Parse( cp))
				fREnzymeVendors->InsertLast( rev);
			else
				delete rev;
			}
		done= (StrStr(cp, "..") != NULL);  // GCG format data separator 
		}
	
		// Read Enzymes 
	for (done= false; !done && !aFile->Eof(); ) {
		aFile->ReadLine( line, kLinemax);
		cp= FlushLine( line);
		if (*cp) {
			DREnzyme*	re = new DREnzyme();
			if (re->Parse( cp))
				fREnzymes->InsertLast( re);
			else
				delete re;
			}
		}
		
	done= true; // !! need some test of acurrate read !!
	if (done) {
		fState= kOkay;
		}
	else fState= kNodata;
}


void DREMap::HuntCut( short n, short x, short& jlo)
{	
	short		jm, jhi, inc;
	Boolean		done;

	jlo= 0; //TESTING....
	
	//- ascend= ((*fSeqCuts)^[n].fSeqIndex > (*fSeqCuts)^[1].fSeqIndex); 
	if (jlo <= 0 || jlo > n) {
		jlo= 0; 
		jhi= n+1;
		}
		
	else { // hunt from last low value section 
		inc= 1;
		if ( x >= fSeqCuts[jlo].fSeqIndex ) // ascend
			do {
				jhi= jlo + inc;
				done= TRUE; 
				if (jhi > n)  
					jhi= n+1;
				else if ( x >= fSeqCuts[jhi].fSeqIndex ) {
					jlo= jhi; inc= inc + inc;
					done= FALSE; 
					}
			} while (!done);
			
		else {
			jhi= jlo;
			do {
				jlo= jhi - inc;
				done= TRUE; 
				if (jlo < 1)
					jlo= 0;
				else if ( x < fSeqCuts[jlo].fSeqIndex) { //== ascend
					jhi= jlo; inc= inc + inc;
					done= FALSE;
					}
			} while (!done);
			}
		}
			// bisection 
	while (jhi - jlo > 1) {
		jm= (jhi + jlo) / 2;
		if ( x > fSeqCuts[jm].fSeqIndex ) jlo= jm;
		else if ( x == fSeqCuts[jm].fSeqIndex ) { jlo= jm; return; }
		else jhi= jm;
		}
}		


void DREMap::CutsAtBase( short atBase, short& firstCut, short& nCuts)
{
	short icut;
		
	nCuts= 0;
	if ( fSeqCuts && fCutcount>0 ) {
		/*-----
		while ( ((*fSeqCuts)^[icut].fSeqIndex < atBase) do icut= icut+1;  
		firstCut= icut;
		------*/
		HuntCut( fCutcount, atBase, firstCut);	
			//hunt may find middle of a run
		while ( firstCut>1 && fSeqCuts[firstCut-1].fSeqIndex == atBase )
			firstCut--;
		icut= firstCut;  
		while (fSeqCuts[icut].fSeqIndex == atBase) {
			nCuts++;
			icut++;
			}
		}
	if (nCuts == 0) firstCut= 0; //??
}



enum { 	kSearchNotFound = -1, kStep = 1 };

long DREMap::QuickSearch( Nlm_Boolean first, char* target, long targlen, 
							char* source, long sourcelen)
{
	static long qIndex;
	register char tBase; // fTargetbase
	
	tBase = target[0];
	if (first) {
		qIndex= -kStep;   
		if (!tBase) return kSearchNotFound;
		}

lNextStep:
	qIndex += kStep; 
	for (register long j= qIndex; j<sourcelen; j++) {  
		if (tBase & source[j]) {
			qIndex = j; 
						//? enough source left to match full target 
			if (sourcelen-qIndex < targlen) 
				return kSearchNotFound;
			else {
			  register char * sp = source + qIndex + 1;
			  register char * tp = target + 1;
			  register long k = targlen-1;
			  long k1= sourcelen - qIndex - 1;
			  if (k1<k) k= k1;
			  if (k<1) goto lNextStep;
			  for ( ; (k); k--) 
			  	if ((*sp++ & *tp++) == 0) goto lNextStep; // no match
			  	
			  if (source[qIndex] == DSequence::kMaskNucs) {
					  // screen out all N's here -- don't want to match these !?
			  	sp = source + qIndex + 1;
				  for (k= targlen-1; (k); k--) 
				  	if (*sp++ != DSequence::kMaskNucs) return qIndex;  // non-NNN match
				  goto lNextStep; // got all Ns .. continue scan
			  	}
			  else
					return qIndex; // matched
				}
			}
		}
	return kSearchNotFound;
} 

	

void DREMap::SearchStrand( DSequence* bSeq, char* aTarget, char* sourcebits, long sourcelen,
											DREnzyme* zyme, short cutAdd, long& zymecuts)
{
	long  targlen, i, len;
  char   tb, * targbits;
  
  len= StrLen(aTarget);
  targbits= (char*) MemNew(len+1);
	for ( i=0, targlen = 0; i<len; i++) { 
		tb = (char) DSequence::kMaskNucs & DSequence::NucleicBits(aTarget[i]);
		if (tb) targbits[targlen++]= tb;
		}
  targbits[targlen]= 0;
  
	long indx= QuickSearch( true, targbits, targlen, sourcebits, sourcelen);
	while (indx != kSearchNotFound) {
		if (fCutcount >= fMaxcuts) {
			fMaxcuts += 100;
			fSeqCuts= (DRECutsItem*) MemMore( fSeqCuts, fMaxcuts * sizeof(DRECutsItem));
			}
		fSeqCuts[fCutcount].fSeqIndex= indx + cutAdd;
		fSeqCuts[fCutcount].fREnzyme= zyme;
		fCutcount++;
		zymecuts++;
		indx= QuickSearch( false, targbits, targlen, sourcebits, sourcelen);
		}
	MemFree(targbits);
}
		

void DREMap::FindCuts( DREnzyme* zyme, char* sourcebits, long sourcelen)
{
 	long    zymecuts;

	zymecuts= 0;
	SearchStrand( fSeq, zyme->fSite, sourcebits, sourcelen, zyme, zyme->fCutpoint, zymecuts);

	if (zyme->fCoSite) {
		// only do when 3' differs from 5' 
		//!? ignore symmetric sites -- are these symmetric if fCut3!=fCut5
		//!? also need to screen sites that are symmetric about cut points !?

		SearchStrand( fSeq, zyme->fCoSite, sourcebits, sourcelen, zyme, 
									zyme->fCoCutpoint, zymecuts);
		}
	zyme->fCutcount= zymecuts;
	if (zymecuts>0) fCuttersCount++;
}


#ifdef WIN_MSWIN
static int LIBCALLBACK
#else
static int
#endif
 
cutterCompare(void* a, void* b)
{
	short diff= ((DRECutsItem*)a)->fSeqIndex - ((DRECutsItem*)b)->fSeqIndex;
	if (diff == 0) {
		return StringCmp(((DRECutsItem*)a)->fREnzyme->fName, 
								  ((DRECutsItem*)b)->fREnzyme->fName);
		}
	else
		return diff;
}


void DREMap::MapSeq( DSequence* aSeq)
{
	long  i, nzymes, len, sourcelen, aStart, aBases;
	char * sourcebits, sb;
	
	FreeTempData();
	if (NotAvailable()) return;
	fSeq= aSeq;
	fSeq->GetSelection( aStart, aBases);
	
		
#if 1
	fCoSeq= aSeq->Complement();
#else
	aSeq->SetSelection(0,0); // for Complement...
	fCoSeq= aSeq->Complement();
	aSeq->SetSelection(aStart,aBases);  
#endif
	
	// make a NucBits form of fBases, once  
#if 0
		// this needs work -- draw currently expects start of seq/remap == 0, not aStart
	if (aBases) {
		len= aBases;
		sourcebits= (char*) MemNew(len+1);
		StrNCpy( sourcebits, aStart+fSeq->Bases(), len);
		sourcebits[len]= 0;
		}
	else
#endif
	{
	sourcebits= StrDup( fSeq->Bases());
	len= StrLen(sourcebits);
	}
	for (i= 0, sourcelen= 0; i<len; i++) {
		sb = (char) DSequence::kMaskNucs & DSequence::NucleicBits(sourcebits[i]);
		if (sb) sourcebits[sourcelen++]= sb;
		}
  	// drop trailing "..." Ns
	while (sourcelen && sourcebits[sourcelen-1] == DSequence::kMaskNucs) 
		sourcelen--;
  sourcebits[sourcelen]= 0;
		
	fCutcount= 0; 
	fMaxcuts = 0;
	fSeqCuts= (DRECutsItem*) MemNew(sizeof(DRECutsItem));
	nzymes= fREnzymes->GetSize();
	for (i=0; i<nzymes; i++) {
		DREnzyme* re= (DREnzyme*) fREnzymes->At(i);
		FindCuts( re, sourcebits, sourcelen);
		}
	MemFree( sourcebits);
  
	Nlm_HeapSort( fSeqCuts, fCutcount, sizeof(DRECutsItem), cutterCompare);
	
		// !! check for ident. zyme cuts at same site ?!!!
  short at, lastat= -1;
  char *name, *lastname=NULL;
  for (i=0; i<fCutcount; i++) {
  	at= fSeqCuts[i].fSeqIndex;
  	name= fSeqCuts[i].fREnzyme->fName;
  	if (lastat == at && StringCmp(name, lastname)==0)
  	  fSeqCuts[i].fREnzyme->fCutcount--;
  	lastname= name;
  	lastat= at;
  	}
}







// class DBaseColors

short DBaseColors::fState= DBaseColors::kUnread;
char* DBaseColors::fType= "color";
char* DBaseColors::fSection= "data";
char* DBaseColors::fDefaultvalue= "tables:color.table";
baseColors DBaseColors::gAAcolors;
baseColors DBaseColors::gNAcolors;
	
typedef unsigned long  colorVal;

Local colorVal		kBlack 	= 0; 
Local colorVal		kRed 		= (255<<16);
Local colorVal		kGreen 	= (255<<8);
Local colorVal		kBlue 	= (255<<0);
Local colorVal		kWhite 	= 0xffffff00;
Local colorVal	 	kYellow = (255<<16) | (255<<8);
Local colorVal	 	kMagenta= (255<<16) | (255<<0);
Local colorVal	 	kCyan 	= (255<<8) | (255<<0);
Local colorVal	 	kOrange = (255<<16) | (110<<8); // | 15<<0;
Local colorVal	 	kOrange1 = (200<<16) | (110<<8); // | 15<<0; // this is brownish orange
Local colorVal	 	kGreen1	= (200<<8) | (31<<16) | (21<<0);
Local colorVal		kGray 	= (127<<8) | (127<<16) | (127<<0);
Local colorVal		kLtGray = (191<<8) | (191<<16) | (191<<0);
Local colorVal		kDkGray = (63<<8) | (63<<16) | (63<<0);


Nlm_Boolean DBaseColors::NotAvailable()
{
	if (fState == kUnread) {
		char * tablefile= gApplication->GetFilePref( fType, fSection, fDefaultvalue);
		if (tablefile) {
			ReadTable( tablefile);
			if (fState != kOkay) {
				DLocateFile* win= new DLocateFile("Could not read table file '%s'", tablefile);
				if (win->PoseModally()) {
					ReadTable( win->fName);
					if (fState == kOkay)
						gApplication->SetPref( win->fName, fType, fSection); //??
					}
				delete win;
				}
			}
		else fState= kNodata;
		if (fState != kOkay) Message(MSG_OK,"Could not read table file '%s'",tablefile);
		MemFree( tablefile);
		}
	return (fState != kOkay);
}

void DBaseColors::Initialize(char* type, char* section, char* defaultvalue)
{
	fState= kUnread;
	fType= type;
	fSection= section;
	fDefaultvalue= defaultvalue;
	InitColors();
}

// static
void DBaseColors::TableChoice()
{
	char * title = "Base Color Table";
	char* desc = 
"[nacolors]"LINEEND
"; nucleic acid colors"LINEEND
"; base=color value ;add line for each base desired in set [' '..'~']"LINEEND
"; put two color values on a line for UPPER and lowercase bases"LINEEND
"A=0xffaa88"LINEEND
"C=0xaaff88"LINEEND
"G=0x8877ff"LINEEND
"T=0x88aaaa"LINEEND
"[aacolors]"LINEEND
"; amino acid colors"LINEEND
"A=0xaaff88"LINEEND
"B=0xffaa88"LINEEND
"; ... etc ..."LINEEND
; 

	char * tablefile= gApplication->GetFilePref( fType, fSection, fDefaultvalue);
	DTableChoiceDialog* win= new DTableChoiceDialog(title,desc,tablefile);
	if (win->PoseModally()) {
		ReadTable( win->fFile);
		if (fState != kOkay) {
			Message(MSG_OK, "Table wasn't properly loaded.  Please try again");
			}
		else if (win->fSetPref)
			gApplication->SetPref((char*)win->fFile->GetName(), fType, fSection);
		}
	MemFree( tablefile);
	delete win;
}



static void ReadColors( char* colorstr, colorVal& ncolor1, colorVal& ncolor2, Boolean& twocolors)
{
		// this is failing to get correct color !!
		// sometimes no where near correct (grey -> black or white)
		// colorstr is read correctly into color1/color2
		// but SelectColor/GetColor pair loses it...
		
	unsigned short cr, cg, cb;
	//Nlm_Uint1  cr, cg, cb;
  unsigned long color1 = 0, color2 = 0;
	char * ep;

	ep= StrChr(colorstr,';'); if (ep) *ep= 0;
	colorstr= FlushLine( colorstr);
	twocolors= (StrChr(colorstr,' ') != NULL);
 		// this works on Sol2, but %lx doesn't !
  sscanf( colorstr, "%li %li", &color1, &color2);  
  
	cr= (color1>>16) & 0xff;
	cg= (color1>>8) & 0xff;
	cb= (color1) & 0xff;
#ifdef WIN_MAC
	// color1 is correct format
#endif
#ifdef WIN_MSWIN
  color1 = (cr | cg << 8 | ((ulong)cb) << 16); //RGB (cr, cg, cb);
#endif
#ifdef WIN_MOTIF
	// X uses mapped values thru setcolor -- need to do the select/get dance
	Nlm_SelectColor( cr, cg, cb);
	color1= Nlm_GetColor();
#endif
	ncolor1= color1;

	if (twocolors) {
		cr= (color2>>16) & 0xff;
		cg= (color2>>8) & 0xff;
		cb= (color2) & 0xff;
#ifdef WIN_MAC
	// color2 is correct format
#endif
#ifdef WIN_MSWIN
  color2 = (cr | cg << 8 | ((ulong)cb) << 16); //RGB (cr, cg, cb);
#endif
#ifdef WIN_MOTIF
	// X uses mapped values thru setcolor -- need to do the select/get dance
		Nlm_SelectColor( cr, cg, cb);
		color2= Nlm_GetColor();
#endif
		ncolor2= color2;
		}
}


void DBaseColors::ReadTable( char* tableFile)
{
 	DFile aFile( tableFile, "r");
	ReadTable( &aFile);
}


void DBaseColors::ReadTable(DFile* aFile)
{
	Boolean done= false;
	unsigned long*	curcolors;
	short  	iline, nlines = 0;
	char	* line;
 	char**	linelist;

	//fState= kNodata;
	if (!aFile || !aFile->Exists()) return;
	colorVal savecolor= Nlm_GetColor();	
	
	aFile->Open("r");
 	linelist= Dgg_ReadDefaultPrefs( aFile->fFile, &nlines);
	
	for (iline= 0; iline<nlines; iline++) {
		line= linelist[iline];
		if (line && *line) {
			if (*line == '[') { // sect name 
				if (StrNICmp(line+1,"nacolor",7)==0)  
					curcolors= &gNAcolors[0];
				else if (StrNICmp(line+1,"aacolor",7)==0) 
					curcolors= &gAAcolors[0];
				else 
					curcolors= NULL;
				}
			else if (*line == ';') {
				// skip comment
				}
			else if (curcolors) {
				char* colorval= StrChr(line, '=');
				if (colorval) {
						// look for 2 values in color value, up & lowcase char colors !?
					Boolean 	twocolors= false;
					colorVal	ncolor1, ncolor2;
					char basec= *line;
					::ReadColors( colorval+1, ncolor1, ncolor2, twocolors);
					curcolors[ toupper(basec) - ' ']= ncolor1;
					if (twocolors) ncolor1= ncolor2;
					curcolors[ tolower(basec) - ' ']= ncolor1;	
					}
				}
			MemFree(line);
			}
		}
	MemFree(linelist);

	
	Nlm_SetColor(savecolor);

	done= true; // !! need some test of acurrate read !!
	if (done) {
		fState= kOkay;
		}
	else fState= kNodata;
}


	// make color palette like these into class
	// w/ file open/save methods, dialog choose color method
	// simplest dialog -- use r,g,b and gray switches

//static
void DBaseColors::InitColors() 
{
		// a static method for static vars
		// should call this Init routine from some local initializer class at startup
		// can't use the const values above -- not same for diff win systems
		//Nlm_SelectColor (Nlm_Uint1 red, Nlm_Uint1 green, Nlm_Uint1 blue)
		// ^^ this is safe for all win sys	
	colorVal savecolor= Nlm_GetColor();
	Nlm_SelectColor(   0,   0,   0); kBlack= Nlm_GetColor();
	Nlm_SelectColor( 255, 255, 255); kWhite= Nlm_GetColor();
	Nlm_SelectColor( 255,   0,   0); kRed= Nlm_GetColor();
	Nlm_SelectColor(   0, 255,   0); kGreen= Nlm_GetColor();
	Nlm_SelectColor(   0,   0, 255); kBlue= Nlm_GetColor();
	Nlm_SelectColor( 255, 255,   0); kYellow= Nlm_GetColor();
	Nlm_SelectColor( 255,   0, 255); kMagenta= Nlm_GetColor();
	Nlm_SelectColor(   0, 255, 255); kCyan= Nlm_GetColor();
	Nlm_SelectColor( 200,  89,  15); kOrange= Nlm_GetColor();
	Nlm_SelectColor(  31, 200,  31); kGreen1= Nlm_GetColor();
	Nlm_SelectColor( 127, 127, 127); kGray= Nlm_GetColor();
	Nlm_SelectColor( 191, 191, 191); kLtGray= Nlm_GetColor();
	Nlm_SelectColor(  63,  63,  63); kDkGray= Nlm_GetColor();
	Nlm_SetColor(savecolor);
	
	char ch;
	for (ch= ' '; ch <= '~'; ch++) { 
		gAAcolors[ch-' ']= kBlack; 
		gNAcolors[ch-' ']= kBlack; 
		}
	gNAcolors['A'-' ']= kRed; 	 
	gNAcolors['a'-' ']= kRed;
	gNAcolors['C'-' ']= kBlue;	 
	gNAcolors['c'-' ']= kBlue;
	gNAcolors['G'-' ']= kGreen1; 
	gNAcolors['g'-' ']= kGreen1;
	gNAcolors['T'-' ']= kOrange;  
	gNAcolors['t'-' ']= kOrange;
	gNAcolors['U'-' ']= kOrange;
	gNAcolors['u'-' ']= kOrange;
}






// class DSeqStyle

short DStyleTable::fState= DStyleTable::kUnread;
char* DStyleTable::fType= "seqmasks";
char* DStyleTable::fSection= "data";
char* DStyleTable::fDefaultvalue= "tables:seqmasks.table";
DList* DStyleTable::fStyles= NULL;
DSeqStyle DStyleTable::fLaststyle;
char	DStyleTable::fLastch= 0;
char* DStyleTable::fToprow = NULL;
long	DStyleTable::fToprowlen= 0;


DSeqStyle::DSeqStyle() :
	name(NULL), description(NULL),
	fontname(NULL),fontsize(10), font(NULL),
	bold(false), italic(false), uline(false), uppercase(false), lowercase(false),
	dofontpat(false),invertcolor(false), framestyle(kFrameSolid),
	fontcolor(0), backcolor(0), framecolor(0), 
	doframecolor(false), dofontcolor(false), dobackcolor(false),
	repeatchar(0),frame(kFramenone)
{
	MemFill(fontpattern,0,sizeof(fontpattern));
}


DSeqStyle::~DSeqStyle()
{
	MemFree( name);
	MemFree( description);
	MemFree( fontname);
}

void DSeqStyle::GetFont()
{
	if (fontname) 
		font= Nlm_GetFont( fontname, fontsize, bold, italic, uline, NULL);
	if (dofontpat && !dobackcolor) {
		dobackcolor= true;
		backcolor= 0; // black!? always or not?
		}
}

DObject* DSeqStyle::Clone()  
{	
	DSeqStyle* astyle= (DSeqStyle*) DObject::Clone();
	astyle->name= StrDup( name);
	astyle->description= StrDup( description);
	astyle->fontname= StrDup( fontname);
	return astyle;
}



Nlm_Boolean DStyleTable::NotAvailable()
{
	if (fState == kUnread) {
		char * tablefile= gApplication->GetFilePref( fType, fSection, fDefaultvalue);
		if (tablefile) {
			ReadTable( tablefile);
			if (fState != kOkay) {
				DLocateFile* win= new DLocateFile("Could not read table file '%s'", tablefile);
				if (win->PoseModally()) {
					ReadTable( win->fName);
					if (fState == kOkay)
						gApplication->SetPref( win->fName, fType, fSection); //??
					}
				delete win;
				}
			}
		else fState= kNodata;
		if (fState != kOkay) Message(MSG_OK,"Could not read table file '%s'",tablefile);
		MemFree( tablefile);
		}
	return (fState != kOkay);
}

void DStyleTable::Initialize(char* type, char* section, char* defaultvalue)
{
	fState= kUnread;
	fType= type;
	fSection= section;
	fDefaultvalue= defaultvalue;
	fStyles= new DList();
	fLastch= 0;
	fToprow = NULL;
	fToprowlen= 0;
}

void DStyleTable::ReadTable( char* tableFile)
{
 	DFile aFile( tableFile, "r");
	ReadTable( &aFile);
}

void DStyleTable::ReadTable(DFile* aFile)
{
	Boolean done= false;
	short  	iline, nlines = 0;
	char	* line;
 	char ** linelist;
	DSeqStyle	*	astyle = NULL;
	char			* cp;
	Boolean 	twocolors;
	colorVal	ncolor1, ncolor2;
	
	//fState= kNodata;
	if (!aFile || !aFile->Exists()) return;
	colorVal savecolor= Nlm_GetColor();	
	
	//fStyles->FreeAllObjects();  // MEM LEAK !
	{
	long i, n= fStyles->GetSize();
	for (i=0; i<n; i++) {
		DSeqStyle* ast= (DSeqStyle*) fStyles->At(i);
		delete ast;
		}
	fStyles->DeleteAll();
	}
	
	aFile->Open("r");
 	linelist= Dgg_ReadDefaultPrefs( aFile->fFile, &nlines);
	
	for (iline= 0; iline<nlines; iline++) {
		line= linelist[iline];
		if (line && *line) {
		
			if (*line == '[') { // sect name 
				if (astyle) { astyle->GetFont(); fStyles->InsertLast(astyle); }  // save last one
				astyle= new DSeqStyle();
				cp= StrChr(line,']'); if (cp) *cp= 0;
				astyle->name= StrDup(line+1); // ?? or do we use a "name=" variable?
				}
				
			else if (*line == ';') { // skip comment
				}
				
			else if (astyle) {
				char* val= StrChr(line, '=');
				if (val) {
					*val++= 0;
					while (isspace(*val)) val++;
					
					if (StrNICmp(line,"descr",5)==0) {
						astyle->description= StrDup(val);
						}
					else if (StrNICmp(line,"repeat",6)==0) {
						astyle->repeatchar= *val;
						}
					else if (StrNICmp(line,"frame",5)==0) {
						if (StrNICmp(val,"box",3)==0) astyle->frame= DSeqStyle::kFramebox ;
						else if (StrNICmp(val,"oval",3)==0) astyle->frame= DSeqStyle::kFrameoval ;
						else if (StrNICmp(val,"round",3)==0) astyle->frame= DSeqStyle::kFramerrect ;
						}
					else if (StrNICmp(line,"boxstyle",5)==0) {
						if (StrNICmp(val,"dashed",3)==0) astyle->framestyle= DSeqStyle::kFrameDashed ;
						else if (StrNICmp(val,"dotted",3)==0) astyle->framestyle= DSeqStyle::kFrameDotted ;
						else if (StrNICmp(val,"solid",3)==0) astyle->framestyle= DSeqStyle::kFrameSolid ;
						else if (StrNICmp(val,"dark",3)==0) astyle->framestyle= DSeqStyle::kFrameDark ;
						else if (StrNICmp(val,"medium",3)==0) astyle->framestyle= DSeqStyle::kFrameMedium ;
						else if (StrNICmp(val,"light",3)==0) astyle->framestyle= DSeqStyle::kFrameLight ;
						}
					else if (StrNICmp(line,"style",5)==0) {
						char* delims= ",;\t ";
						char* word = StrTok(val, delims);
						while (word) {
							if (StrNICmp(word,"bold",3)==0) astyle->bold= true ;
							else if (StrNICmp(word,"italic",3)==0) astyle->italic= true ;
							else if (StrNICmp(word,"underline",3)==0) astyle->uline= true ;
							else if (StrNICmp(word,"uppercase",3)==0) astyle->uppercase= true ;
							else if (StrNICmp(word,"lowercase",3)==0) astyle->lowercase= true ;
							else if (StrNICmp(word,"box",3)==0) astyle->frame= DSeqStyle::kFramebox ;
							else if (StrNICmp(word,"frame",3)==0) astyle->frame= DSeqStyle::kFramebox ;
							else if (StrNICmp(word,"invertcolor",3)==0) astyle->invertcolor= true ;
							word= StrTok( NULL, delims);
							}
						}
					else if (StrNICmp(line,"fontcolor",7)==0) {
						::ReadColors( val, ncolor1, ncolor2, twocolors);
						astyle->dofontcolor= true;
						astyle->fontcolor= ncolor1;
						}
					else if (StrNICmp(line,"backcolor",7)==0) {
						::ReadColors( val, ncolor1, ncolor2, twocolors);
						astyle->dobackcolor= true;
						astyle->backcolor= ncolor1;
						}
					else if (StrNICmp(line,"framecolor",7)==0 
								|| StrNICmp(line,"boxcolor",6)==0) {
						::ReadColors( val, ncolor1, ncolor2, twocolors);
						astyle->doframecolor= true;
						astyle->framecolor= ncolor1;
						}
					else if (StrNICmp(line,"fontnam",7)==0) {
						astyle->fontname= StrDup(val);
						}
					else if (StrNICmp(line,"fontsiz",7)==0) {
						astyle->fontsize= atol(val);
						}
					else if (StrNICmp(line,"fillpat",7)==0  
							  || StrNICmp(line,"fontpat",7)==0) {
						astyle->dofontpat= true;
						char * pat= astyle->fontpattern;
  					sscanf( val, "%li %li", pat, pat+4);  
						}

					}
				}
			MemFree(line);
			}
		}
	MemFree(linelist);
	if (astyle) { astyle->GetFont(); fStyles->InsertLast(astyle); }  // save last one
	Nlm_SetColor(savecolor);

	done= true; // !! need some test of acurrate read !!
	if (done)  fState= kOkay;
	else fState= kNodata;
}


// static
void DStyleTable::TableChoice()
{
	char * title = "Sequence Style Table";
	char * desc = 
"[mask1]"LINEEND
"description=This is my first mask"LINEEND
"style=bold,italic,box,underline,uppercase/lowercase,invertcolor"LINEEND
"repeatchar=. ; use if you want mult-align repeated chars set to this"LINEEND
"fontname=Times"LINEEND
"fontsize=12"LINEEND
"fontcolor=0xff0000"LINEEND
"backcolor=0x80e0e0"LINEEND
"boxcolor=0x00ff00"LINEEND
"fillpattern=0xffaa8877 0xccddeeff ; use 2 hex-long values for this 8-byte pattern"LINEEND
"[anothermask]"LINEEND
"description=my second mask"LINEEND
"; ... etc ..."LINEEND
; 

	char * tablefile= gApplication->GetFilePref( fType, fSection, fDefaultvalue);
	DTableChoiceDialog* win= new DTableChoiceDialog(title,desc,tablefile);
	if (win->PoseModally()) {
		ReadTable( win->fFile);
		if (fState != kOkay) {
			Message(MSG_OK, "Table wasn't properly loaded.  Please try again");
			}
		else if (win->fSetPref)
			gApplication->SetPref((char*)win->fFile->GetName(), fType, fSection);
		}
	MemFree( tablefile);
	delete win;
}


void DStyleTable::StartDraw(char* toprowbases, long maxrow)
{
	fToprow= toprowbases; // any need to dup it?
	fToprowlen= maxrow;
	fLastch= 0;
}

void DStyleTable::EndDraw()
{
	fToprow= NULL;
	fLaststyle.ClearDrawing();
	fLastch= 0;
}

void DSeqStyle::ClearDrawing()
{
		// reverse the draw screen state
	if (font) ; // set saved font !?
	font= NULL;
	//if (invertcolor) Nlm_InvertColors(); // do before Black()
	Nlm_Black();
	fontcolor= 0; // == black
	if (dobackcolor) ; //??
	backcolor= 0;
	if (dofontpat) Nlm_Solid();
	dofontpat= false;
}

void DStyleTable::DrawBaseWithStyle( char ch, long baseindex, short maskval, 	
											  						Nlm_RecT&	crec, short atrow)
{
	//short maskval= aseq->MaskAt(baseindex, masklevel);
	DSeqStyle* style= NULL;
	if (maskval) style= (DSeqStyle*) fStyles->At(maskval-1);
	if (!style) {
		Nlm_PaintChar(ch);
		return;
		}
		
	if (style->repeatchar && atrow>0 && fToprow && baseindex<fToprowlen && ch == fToprow[baseindex]) 
		ch= style->repeatchar;
	if (style->uppercase) ch= toupper(ch);
	else if (style->lowercase) ch= tolower(ch);
	
	if (style->font && (style->font != Nlm_fontInUse || style->font != fLaststyle.font)) 
		Nlm_SelectFont(style->font);  
	if (style->dofontcolor) {
		//if (style->fontcolor != fLaststyle.fontcolor) 
		Nlm_SetColor(style->fontcolor);
		}
	else {
		// !! Fix this -- pass "colors" param
		//if (ch!=fLastch) Nlm_SetColor( colors[ch-' ']);
		}
		
	if (style->invertcolor && !fLaststyle.invertcolor) Nlm_InvertColors();
	// ?? &/or Nlm_InvertRect( &crec);
	// #define  ShadeInvert()  { PaintRect(myRect);	TextMode(srcBIC); }
	// if (style->dobackcolor) { Nlm_SetColor(style->backcolor); Nlm_PaintRect(&crec);	TextMode(srcBIC); } //???
	// if (style->dofontpat) Dgg_PenPattern(fontpattern);  
	
	Nlm_PaintChar(ch);
	
	if (style->frame) switch (style->frame) {
		case DSeqStyle::kFramebox:	 Nlm_FrameRect( &crec); break;
		case DSeqStyle::kFrameoval:  Nlm_FrameOval( &crec); break;
		case DSeqStyle::kFramerrect: Nlm_FrameRoundRect( &crec, 4,4); break;
		default: break;
		}
		
	fLastch= ch;
	fLaststyle= *style;
}			



