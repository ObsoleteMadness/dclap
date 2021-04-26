// DSeqAsmDoc.cp

#define POPBASECALL

#include "DSeqAsmDoc.h"

#include <ncbi.h>
#include <dgg.h>
#include <Dvibrant.h>
#include <DApplication.h>
#include <DClipboard.h>
#include <DControl.h>
#include <DWindow.h>
#include <DRichViewNu.h>
#include <DTableView.h>
#include <DViewCentral.h>
#include <DTask.h>
#include <DTracker.h>
#include <DMenu.h>
#include <DUtil.h>
#include <DFindDlog.h>
#include "DRichHandler.h"
#include "DSeqFile.h"
#include "DSeqEd.h"
#include "DSeqMail.h"
#include "DSeqCmds.h"
#include "DSeqChildApp.h"
#include "DSeqPrint.h"
#include "DSeqPict.h"
#include "DSeqViews.h"

#include "DSeqAsmView.h"
#include "DAsmSequence.h"

#ifdef POPBASECALL
#include "DBaseCallPop.h"
#endif

static char gDefSeqNameStore[128];
Boolean  DSeqAsmDoc::fgTestAutoseqFile = false; // turn on/off seq-test for OpenDocument()


enum { kModeAsmPopup = 3310, kColorAsmPopup, kBaseOnTrace };


class DAsmModePopup : public DPopupList
{
public:
	DAsmModePopup(long id, DView* superior);
	virtual short GetValue() { return DAlnView::kModeSlide + DPopupList::GetValue() - 1; }
};

DAsmModePopup::DAsmModePopup(long id, DView* superior) :
			DPopupList(id,superior,true)
{
	AddItem( "Slide bases");
	AddItem( "Edit bases");
	AddItem( "Bases + Traces");
	AddItem( "A Bases + Trace");
	AddItem( "C Bases + Trace");
	AddItem( "G Bases + Trace");
	AddItem( "T Bases + Trace");
	this->SetValue(3);
}


class DAsmColorPopup : public DPopupList
{
public:
	DAsmColorPopup(long id, DView* superior, short defaultvalue);
	virtual short GetValue() { return DAlnView::kBaseBlack + DPopupList::GetValue() - 1; }
};

DAsmColorPopup::DAsmColorPopup(long id, DView* superior, short defaultvalue) :
			DPopupList(id,superior,true)
{
	AddItem( "Black");
	AddItem( "Color");
	this->SetValue(2); // (defaultvalue + 1 - DAlnView::kBaseBlack);
}

// this dups in DSeqDoc
class DSeqFormatPopup1 : public DPopupList
{
public:
	DSeqFormatPopup1(long id, DView* superior, short defaultFormat);
	virtual short GetValue() { return DSeqFile::kMinFormat + DPopupList::GetValue() - 1; }
};

DSeqFormatPopup1::DSeqFormatPopup1(long id, DView* superior, short defaultFormat) :
			DPopupList(id,superior,true)
{
	for (short i= DSeqFile::kMinFormat; i<=DSeqFile::kMaxFormat; i++) 
		this->AddItem( (char*)DSeqFile::FormatName(i));
	this->SetValue(defaultFormat);
}



#ifdef notnowPOPBASECALL

enum { kBaseCallPopup = 4987 };

class DBaseCallPop1 : public DPopupList {
public:
	Boolean fVisible;
	DAsmView* fAsmView;
	
	DBaseCallPop1(long id, DView* itsSuperior);
	virtual short GetValue() { return /*basemode + */ DPopupList::GetValue() - 1; }
	virtual Boolean IsMyAction(DTaskMaster* action);
};

DBaseCallPop1* gBaseCallPop1 = NULL; // convert to field in fAsmDoc


DBaseCallPop1::DBaseCallPop1(long id, DView* itsSuperior):
		DPopupList(id, itsSuperior), 
		fVisible(false), fAsmView(NULL) 
{
	//fAsmView= itsSuperior;
	AddItem("clear bases");
	AddItem("set A");
	AddItem("set C");
	AddItem("set G");
	AddItem("set T");
	// ?? stats display at position, other functions at trace position?
}


Boolean DBaseCallPop1::IsMyAction(DTaskMaster* action) 
{
	switch(action->Id()) {
	
	 case kBaseCallPopup: {
		Message(MSG_OK,"DBaseCallPop1::action not ready.");
		short mode= this->GetValue();  
		//fAsmView->SetViewMode( mode);
		switch (mode) {
			case 0: break;
			case 1: break; // set a
			case 2: break; // set c
			case 3: break;
			case 4: break;
			}
  	return true;
		}
		
#if 0
		case DApplication::kFind:
		case DApplication::kFindAgain:
			return gApplication->DoMenuTask(action->Id(), NULL);
		case DSeqDoc::cFindORF:
			return fAlnView->fDoc->DoMenuTask( action->Id(), NULL);
#endif
		default:
			return DPopupList::IsMyAction(action);
		}
}

#endif





//class DSeqAsmDoc


DSeqAsmDoc::DSeqAsmDoc( long id, DSeqList* itsSeqList, char* name) :
		DSeqDoc( id, itsSeqList, name),
		fAsmSeq(NULL), fAutoseqFilesRead(0) , fNtrace(0), fHaveAsmseq(false)
{ 
}

DSeqAsmDoc::~DSeqAsmDoc()
{
}

void DSeqAsmDoc::FreeData()
{
	DSeqDoc::FreeData();
}


// static
Boolean DSeqAsmDoc::IsAutoseqFile(DFile* aFile)
{
	if (aFile && aFile->Exists()) {
		if (fgTestAutoseqFile) return true;
		aFile->OpenFile("r");
		Boolean isautoseq= true;
		short  nline= 0;
		while (!aFile->Eof() && nline<4 && isautoseq) {
			char  buf[256];
			aFile->ReadLine( buf, sizeof(buf)); nline++;
			switch (nline) {
				case 1: 
					if (StrNCmp(buf, "filename:", 9) != 0) isautoseq= false;
					break;
				case 2: 
					if (StrNCmp(buf, "native format:", 14) != 0) isautoseq= false;
					break;
				case 3: 
					if (StrNCmp(buf, "strand:", 7) != 0) isautoseq= false;
					break;
				case 4: 
					if (StrNCmp(buf, "total size:", 11) != 0) isautoseq= false;
					break;
				}
			}
		aFile->Close();
		return isautoseq;
		}
	else {
		return false;
		}
}


// static
void DSeqAsmDoc::NewSeqAsmDoc()
{
	DSeqAsmDoc* newdoc= new DSeqAsmDoc( DSeqAsmDoc::kSeqAsmdoc, NULL, (char*)DFileManager::kUntitled);
	newdoc->Open();
	//newdoc->EditSeqs(); //?? make it real obvious to beginners?
}

// static
void DSeqAsmDoc::GetSeqAsmGlobals()
{
}

// static
void DSeqAsmDoc::SaveSeqAsmGlobals()
{
}


void DSeqAsmDoc::MakeGlobalsCurrent()
{
	DSeqDoc::MakeGlobalsCurrent();
}

void DSeqAsmDoc::Close()
{
	DSeqDoc::Close();
}


void DSeqAsmDoc::Open(DFile* aFile)
{
	if (DSeqAsmDoc::fgTestAutoseqFile) 
		DSeqDoc::Open(aFile);
	else {
		fAutoseqFilesRead= 0; // always in this case, but don't know when coming from autoseq

		short gotTrace= 0;
		Boolean isempty= (fSeqList->GetSize() < 1);
		aFile->fType="autoseq/tfs";  
		
		//DSeqDoc::Open(aFile);
		if ( !ReadFrom( aFile, true) ) {
				if (isempty) this->suicide();
				return;
				}

		// user selected autoseq.tfs file .. now look for .A, .C, .. files
		char *fname, buf[512];
		StrNCpy( buf, aFile->GetName(), sizeof(buf));
		DFileManager::ReplaceSuffix( buf, sizeof(buf), ".A");
		DFile bFile( buf);
		if (bFile.Exists()) {
			bFile.fType= "autoseq/atrace";  
			//DSeqDoc::Open(&bFile);
			if ( ReadFrom( &bFile, true) ) gotTrace++;
			}
		
		DFileManager::ReplaceSuffix( buf, sizeof(buf), ".C");
		bFile.SetName( buf);
		if (bFile.Exists()) {
			bFile.fType= "autoseq/ctrace";  
			//DSeqDoc::Open(&bFile);
			if ( ReadFrom( &bFile, true) ) gotTrace++;
			}

		DFileManager::ReplaceSuffix( buf, sizeof(buf), ".G");
		bFile.SetName( buf);
		if (bFile.Exists()) {
			bFile.fType= "autoseq/gtrace";  
			//DSeqDoc::Open(&bFile);
			if ( ReadFrom( &bFile, true) ) gotTrace++;
			}

		DFileManager::ReplaceSuffix( buf, sizeof(buf), ".T");
		bFile.SetName( buf);
		if (bFile.Exists()) {
			bFile.fType= "autoseq/ttrace";  
			//DSeqDoc::Open(&bFile);
			if ( ReadFrom( &bFile, true) ) gotTrace++;
			}

		if (gotTrace) {
			if (isempty) {
				this->SetTitle((char*)aFile->GetShortname());
				this->SetFilename((char*)aFile->GetName());
				}
			this->Open();
			}
		else if (isempty) this->suicide();
		} 
}


void DSeqAsmDoc::Revert()
{
	DSeqDoc::Revert();
}

void DSeqAsmDoc::AddAlnIndex(DView* super, short width, short height)
{
	fAlnIndex= new DAlnIndex(0, super, this, fSeqList, width, height); 
}

void DSeqAsmDoc::AddAlnView(DView* super, short width, short height)
{
	if (fHaveAsmseq)
		fAlnView= new DAsmView(0, super, this, fSeqList, width, height); 
	else
		fAlnView= new DAlnView(0, super, this, fSeqList, width, height); 
}

void DSeqAsmDoc::AddModePopup(DView* super)
{
	fModePop= new DAsmModePopup(kModeAsmPopup, super);
}


void DSeqAsmDoc::AddTopViews(DView* super)
{
	DPrompt* pr;

	pr= new DPrompt(0,super,"Color:",0, 0, Nlm_programFont);
	super->NextSubviewToRight();
	fColorPop= new DAsmColorPopup(kColorPopup,super,fgUseColor); //kColorAsmPopup
	fUseColor= fgUseColor; // not used??
	super->NextSubviewToRight();

	pr= new DPrompt(0,super," View:",0, 0, Nlm_programFont);
	super->NextSubviewToRight();
	this->AddModePopup(super); 
	fLockCheck= NULL;

	super->NextSubviewToRight();
	pr= new DPrompt(0,super,"  File format:",0, 0, Nlm_programFont);

	DCluster* metergrp= new DCluster( 0, this, 0, 0, true);   // was 0,0 for w,h
	pr= new DPrompt(0,metergrp,"Point#",0, 0, Nlm_programFont);
	metergrp->NextSubviewToRight();
	fSeqMeter= new DPrompt(0,metergrp,"0000000",0,0,Nlm_programFont,justright); //60,14

	super->NextSubviewToRight();
	fFormatPop= new DSeqFormatPopup1(0,super,fInFormat);
	
	super->NextSubviewBelowLeft();
}


void DSeqAsmDoc::AddViews()
{
	Boolean needviews= (!fAlnView || !fAlnIndex || !fAlnHIndex || !fAlnITitle);
	if (needviews) {
		new DPrompt(0,this,"  * PRELIMINARY Sequence Assembly *  ",0, 0, Nlm_systemFont);

		if (fHaveAsmseq) {
			DView* super= this;
			super->NextSubviewToRight();
			DCheckBox* ck= new DCheckBox(kBaseOnTrace,super, "");
			ck->SetStatus(true); // default behavior, no sticky pref yet
			super->NextSubviewToRight();
			(void) new DPrompt(0, super, "Bases on trace",0, 0, Nlm_programFont);
			}
		this->NextSubviewBelowLeft();
		}
		
	DSeqDoc::AddViews();
}


void DSeqAsmDoc::Open()
{
#if 0	
	DSeqDoc::Open();
#else
	Boolean selectone= false;
	if (!fSeqList) fSeqList= new DSeqList();
	if (fSeqList->GetSize() == 0) {
		fSeqList->AddNewSeq();	// install 1 blank seq for new doc
		selectone= true;
		}
	
	AddViews();
	fSaveHandler= this;  
	fPrintHandler= this; 

	this->Select(); // for motif
	this->CalcWindowSize();
	fAlnView->GetReadyToShow(); // do before others ...
	if (fAlnIndex) fAlnIndex->GetReadyToShow();
	fAlnITitle->GetReadyToShow();

	fAlnView->SetViewColor(fgUseColor);
	fAlnView->SetViewMode(fgViewMode);
	if (selectone && fAlnIndex) fAlnIndex->SelectCells(0,0);
	fSeqMeter->Show();  
	
	DWindow::Open();
#endif
}


void DSeqAsmDoc::Activate()
{
	DSeqDoc::Activate();
}

void DSeqAsmDoc::Deactivate()
{
	DSeqDoc::Deactivate();
}

void DSeqAsmDoc::Print()
{
	DSeqDoc::Print();
}

Boolean DSeqAsmDoc::IsMyTask(DTask* theTask) 
{
	return DSeqDoc::IsMyTask(theTask);
}

void DSeqAsmDoc::ProcessTask(DTask* theTask) 
{
	if (theTask->fNumber == -87654) {

		}
	else {
		DSeqDoc::ProcessTask(theTask);
		}
}


void DSeqAsmDoc::SortView(DSeqList::Sorts sortorder)
{
	//DSeqDoc::SortView(sortorder);
}


void DSeqAsmDoc::AddSeqToList(DSequence* item)
{
	DSeqDoc::AddSeqToList(item);
}	



void DSeqAsmDoc::AddNewSeqToList()
{
	DSeqDoc::AddNewSeqToList();
}

  
void DSeqAsmDoc::GetSelection( long selectFlags, DSeqList*& aSeqList, 
							long& start, long& nbases)
{
	DSeqDoc::GetSelection( selectFlags, aSeqList, start, nbases);
}
			

short DSeqAsmDoc::SelectionToFile(Boolean AllatNoSelection, char* aFileName, 
			short seqFormat)  
{
	return DSeqDoc::SelectionToFile( AllatNoSelection, aFileName, seqFormat);
}


void DSeqAsmDoc::WriteTo(DFile* aFile)  
{
#if 0
	DSeqDoc::WriteTo( aFile);
#else	
	long i, n = fSeqList->GetSize();
	for (i=0; i<n; i++) {
		char * name, newname[512], backname[512];
		fAsmSeq= (DAsmSequence*) fSeqList->SeqAt(i);
		Boolean dotrace= fAsmSeq->ShowTrace() && (fAsmSeq->Kind() == DAsmSequence::kAutoseq);
		if (!dotrace)  
			; //DSeqDoc::WriteTo( aFile); << not for single sequences...
		else {
			fAsmSeq->TraceToBases();
			name = (char*)fAsmSeq->TracefileName();
			//if (!name) name= "myseq.tfs";
			DFile fromFile(name);
			if (i>0) {
				sprintf( newname, "%s.new", name);
				sprintf(backname, "%s.old", name);
				aFile= new DFile(newname);
				}
			ReadTraceStats( &fromFile, true, aFile);  
			if (i>0) {
				DFileManager::Rename( name, backname);
				DFileManager::Rename( newname, name);
				}
			}
		}
	this->NotDirty();  
#endif
}




void DSeqAsmDoc::ReadTrace(DFile* aFile, short tracenum)	 
{
	char   * cp, buf[512];
	TraceBase  tb, * tbuf = NULL;  
	long   np= 0, maxp = 0;
	Boolean traceokay;
	short ht, maxht = 0;
	
	aFile->Open("r"); fAutoseqFilesRead++;
	tbuf= fAsmSeq->Trace();
	np= 0; // NO: fAsmSeq->NumPoints();  
	maxp= fAsmSeq->MaxPoints();
	traceokay= (tracenum>=TraceBase::tA && tracenum<=TraceBase::tT); 
	while (!aFile->Eof()) {
		aFile->ReadLine( buf, sizeof(buf));
		cp= buf; 
		ht= atoi(cp); maxht= Max( maxht, ht);
		
		if (np >= maxp) {	
			if (fNtrace && !maxp)maxp= fNtrace+1;// read maxp from ReadTraceStats()
			else maxp += 1000;
#if 1
			fAsmSeq->UpdateTrace( tbuf, np, maxp);
#else
			TraceBase* tmpb= new TraceBase[ maxp];
			if (tbuf) {
				MemCpy( tmpb, tbuf, np * sizeof(TraceBase));
				delete tbuf;
				}
			//fAsmSeq->FreeTrace();
			tbuf= tmpb;
#endif
			if (!tbuf) traceokay= false;
			}
		if (traceokay) {
			tb= tbuf[np];
			tb.tr[tracenum]= ht;
			tbuf[np++]= tb;
			}
		}
		
	if (tbuf && maxp) {
#if 1
		fAsmSeq->UpdateTrace( tbuf, np, maxp); 
#else
		fAsmSeq->SetTrace( tbuf, np, maxp, false); 
#endif
		fAsmSeq->SetMaxHeight( tracenum, maxht);
		}
}

		 
void DSeqAsmDoc::ReadTraceStats(DFile* aFile, Boolean rewrite, DFile* toFile)	 
{
#if 0
// autoseq/tfs format
// various doc and values at top...
total size:	5079
window:		[0,5078]  (5079 points)
min/max/mean:	0/255/29  (within window)
sequence len:	454
///
peaks picked:	495
//#    X    ht    ctr    left   right  width      P(H) P(D|NULL)    P(D|H)    P(H|D)
1    G    42      0     0.0     2.6    2.6  1.00e+00  1.97e-03  8.92e-01  9.98e-01
2    T    39      0     0.0     4.3    4.3  1.00e+00  6.58e-05  3.67e+04  1.00e+00
3    A    66     15     8.6    18.7   10.1  1.00e+00  1.41e-04  2.13e+02  1.00e+00
4    G   128     15    10.4    21.0   10.6  1.00e+00  7.01e-12  1.84e+16  1.00e+00
5    T    26     17    12.2    22.0    9.8  1.00e+00  2.20e-03  2.72e+01  1.00e+00
6    C    36     25    19.9    37.0   17.1  1.00e+00  5.36e-03  1.53e-01  9.66e-01
7    G    44     28    21.0    31.8   10.8  1.00e+00  1.57e-03  1.41e+00  9.99e-01
8    T    37     28    22.0    33.8   11.8  1.00e+00  1.24e-04  1.02e+04  1.00e+00
9    A    80     32    28.1    35.6    7.6  1.00e+00  1.42e-05  1.99e+04  1.00e+00
10   C    24     52    48.6    55.8    7.2  1.00e+00  1.01e-02  3.84e-02  7.92e-01
... to 495
#endif

	char  base, buf[512], buf2[128];
	int		num, ht, ctr, npeaks = 0, npoints = 0; // int for sscanf(%d)
	float		left, right, width;
	float		ph, pd, phd, pdh;
	Boolean	incolumns = false, gotpeaks = false;
	char	* bseq = NULL;
	short	* cseq = NULL;
	TraceBase * tbuf = NULL;
	long		atpt, num1, ns= 0, maxs= 0;
	
	aFile->Open("r"); 
	fAutoseqFilesRead++;
	if (rewrite) {
		if (!toFile) return;
		toFile->Open("w");
		atpt= num1 = 0;
		tbuf= fAsmSeq->Trace();
		}
		
	while (!aFile->Eof()) {
		MemSet( buf, 0, sizeof(buf));
		aFile->ReadLine( buf, sizeof(buf));
		if (*buf < ' ') ;
		else if (incolumns) {
			const char* scanform  = "%d %c %d %d %f %f %f %e %e %e %e";
			const char* printform = 
				"%-4d %c %5d %6d %7.1f %7.1f %6.1f %10.2e %10.2e %10.2e %10.2e\n";
			num= ht= ctr= 0;
			//MemSet( buf2, 0, sizeof(buf2));
			sscanf( buf, scanform,
						 &num, &base, &ht, &ctr, &left, &right, &width, &ph, &pd, &pdh, &phd);
			// num should be in [1..npeaks]
	
			if (rewrite) {
				num1++; // need to replace num w/ num1 in buf !
				sprintf( buf, printform,
						 		 num1, base, ht, ctr, left, right, width, ph, pd, pdh, phd);
				toFile->WriteLine( buf);
				*buf= 0;
					 		 	
						// added points
				while (atpt < ctr) {
					TraceBase tr= tbuf[atpt];
					if (tr.base) {
						long  ht = 0;
						//char *rest= " 1.0       0.0       0.0       0.0     ";
						switch (tr.base) {
							case 'A': ht= tr.tr[0]; break;
							case 'C': ht= tr.tr[1]; break;
							case 'G': ht= tr.tr[2]; break;
							case 'T': ht= tr.tr[3]; break;
							}
						num1++;
						MemSet( buf2, 0, sizeof(buf2));
						sprintf( buf2, printform,
						 			num1, tr.base, ht, atpt, 
						 			(float)atpt-1, (float)atpt+1, 3.0, 1.0, 0.0, 0.0, 1.0);
						toFile->WriteLine( buf2);
						}
					atpt++;
					}
						// deleted points -- tbuf[ctr].base == null !!
				if (atpt > ctr) {
						// !? what of extra old points we had to shift up?
					ctr= atpt; //??
					}

				if (!tbuf[ctr].base) *buf= 0;
				else atpt++; //??
				}
			else {
				if (ns >= maxs) {
					if (maxs == 0) maxs = 1+npeaks;
					else maxs += 1000;
					if (bseq) bseq= (char*) MemMore(bseq, maxs * sizeof(char));
					else bseq= (char*) MemNew( maxs * sizeof(char));
					if (cseq) cseq= (short*) MemMore(cseq, maxs * sizeof(short));
					else cseq= (short*) MemNew( maxs * sizeof(short));
					}
				bseq[ns]= base;
				cseq[ns]= ctr;
				}
			ns++;
			if (ns >= npeaks) incolumns= false; // !?
			}
		else if (gotpeaks) {
			if (*buf == '#') incolumns= true;
			}
		else if (StrNICmp(buf,"peaks picked:",13) == 0) {
			sscanf( buf, "peaks picked: %d", &npeaks);
			gotpeaks= true;
			if (rewrite) sprintf( buf, "peaks picked: %d\n", fAsmSeq->LengthF());
			}	
		else if (StrNICmp(buf,"total size:",11) == 0) {
			npoints= 0;
			sscanf( buf, "total size: %d", &npoints);
			fNtrace= npoints;
			}	
			
		if (rewrite && *buf) toFile->WriteLine( buf);
		}
		
	if (rewrite) toFile->Close();
	else if (ns && bseq && cseq) {
		bseq[ns]= 0;
		cseq[ns]= 0;
		fAsmSeq->SetBases( bseq, false); 
		fAsmSeq->SetTrCenter( cseq, ns, false);  // don't need now??
		fAsmSeq->SetPoints( npoints);
		fAsmSeq->SetTracefileName( (char*)aFile->GetName());
		fAsmSeq->SetName( (char*)aFile->GetShortname());
		}
}



Boolean DSeqAsmDoc::ReadFrom(DFile* aFile, Boolean append)	// revise for iostreams
{
	if (!append) FreeData();   
	gDefSeqName= this->GetTitle(gDefSeqNameStore,sizeof(gDefSeqNameStore));
	if (!fSeqList) fSeqList= new DSeqList();
	if (!fAsmSeq) {
		fAsmSeq= new DAsmSequence();
		fSeqList->InsertLast( fAsmSeq);
		}

	char* ft= aFile->fType; // fType holds autoseq/type string !?
	if (StrICmp(ft, "autoseq/pseq")==0)  
		; // fInFormat= DSeqFile::ReadSeqFile(aFile, fSeqList);   << don't need
	else if (StrICmp(ft, "autoseq/tfs")==0)	 ReadTraceStats( aFile);  
	else if (StrICmp(ft, "autoseq/atrace")==0)	ReadTrace( aFile, TraceBase::tA);
	else if (StrICmp(ft, "autoseq/ctrace")==0) 	ReadTrace( aFile, TraceBase::tC);
	else if (StrICmp(ft, "autoseq/gtrace")==0)	ReadTrace( aFile, TraceBase::tG);
	else if (StrICmp(ft, "autoseq/ttrace")==0)	ReadTrace( aFile, TraceBase::tT);
	else return false;
	
	fHaveAsmseq= true;
	// can we remove this mem leak now?
	// NO: This is too risky -- caller should solve the leak
	// MemFree(aFile->fType); aFile->fType=NULL;

	if (fAutoseqFilesRead >= 5) {
		// got all files for fAsmSeq, reset for next autoseq set...
		// set fAsmSeq->Trace().base using TrCenter
		TraceBase * tr = fAsmSeq->Trace();
		short * trc = fAsmSeq->TrCenter();
		long ntr= fAsmSeq->NumPoints();
		for (long i=0; i<fAsmSeq->LengthF(); i++) {
			short ctr= trc[i];
			if (ctr>=0 && ctr<ntr) {
				while (tr[ctr].base > ' ' && ctr<ntr) ctr++; // can't overlap bases !
				tr[ctr].base= fAsmSeq->Bases()[i];
				}
			}
		fAsmSeq= NULL; 
		fAutoseqFilesRead= 0;
		}	
		
	return true;  
}

 
void DSeqAsmDoc::OpenSeqedWindow(DSequence* aSeq)
{
	// disallow this, for now... trashes DAsmSequence
}


void DSeqAsmDoc::MakeSeqPrint(Boolean doREMap)
{
	DSeqDoc::MakeSeqPrint( doREMap);
}


// static
void DSeqAsmDoc::SetUpMenu(short menuId, DMenu*& aMenu) 
{
	// all done in DSeqDoc::SeqUpMenu() ??
}


Boolean DSeqAsmDoc::DoMenuTask(long tasknum, DTask* theTask)
{
	return DSeqDoc::DoMenuTask( tasknum, theTask);
}


Boolean DSeqAsmDoc::IsMyAction(DTaskMaster* action) 
{
	long  menuid= 0, menuitem = action->Id();

	if (menuitem == kModeAsmPopup) {
		short mode= fModePop->GetValue();  
		fAlnView->SetViewMode( mode);
  	return true;
  	}
	else if (fHaveAsmseq && menuitem == kBaseOnTrace) {
		((DAsmView*)fAlnView)->fBaseOnTrace= ((DView*)action)->GetStatus();
		((DAsmView*)fAlnView)->Invalidate();
  	return true;
		}
		
#ifdef POPBASECALL
	else if (menuitem == kBaseCallPopup) {
		return ((DAsmView*)fAlnView)->fBaseCallPop->IsMyAction( action);
		}
#endif

#if 0
	else if (menuitem == kColorAsmPopup) {
		fgUseColor= fColorPop->GetValue();
		fAlnView->SetViewColor( fgUseColor);
  	return true;
		}
#endif
	else
		return DSeqDoc::IsMyAction( action);
}
			



