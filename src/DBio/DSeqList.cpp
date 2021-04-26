// DSeqList.cp
// d.g.gilbert


#include "DSeqList.h"
#include "DSequence.h"
#include "DSeqFile.h"
#include <ncbi.h>
#include <dgg.h>
#include <DList.h>
#include <DFile.h>
#include <ureadseq.h>



Global short gLinesPerSeqWritten = 0;
short DSeqList::gMinCommonPercent = 70;

// DSeqList  -------------------------------------

DSeqList::DSeqList() :
	DList(),
	fSortOrder(kSortByItem)
{
}

DSeqList::~DSeqList()
{
		// PLUG MEMORY LEAK !! ~DList doesn't call object destructors !!!!
	if (fDeleteObjects) {
		long i, n= GetSize();
		for (i=0; i<n; i++) {
			DSequence* aseq= this->SeqAt(i);
			delete aseq;
			}
		fDeleteObjects= false;
		}
}

Boolean DSeqList::IsEmpty()
{
	if (GetSize() > 1) 
		return FALSE;
	else if (GetSize() == 1) {
		DSequence* aSeq= (DSequence*) this->First();
		return (aSeq && aSeq->LengthF() > 0);
		}	
	else
	  return TRUE;
}





class CFileIndex 
{
public:
	long*	fIndices;
	long	fMax, fNum;

	CFileIndex();
	~CFileIndex();
 	long* Indices() { return fIndices; }
 	long	IndexCount() { return fNum; }
	void Indexit( long index);
	void Indexit( DFile* aFile);
	void IndexEOF( DFile* aFile);
};

CFileIndex::CFileIndex() 
{
	fNum= 0;
	fMax= 0;
	fIndices= (long*) MemGet(sizeof(long),true);
}

CFileIndex::~CFileIndex()
{
	if (fIndices) MemFree( fIndices);
}
 	
void CFileIndex::Indexit( long index)
{
	if (fNum >= fMax){
		fMax += 20; 
		fIndices= (long*) MemMore( fIndices, sizeof(long)*fMax); 
		}
	if (fIndices) fIndices[fNum++]= index;
}

void CFileIndex::Indexit( DFile* aFile)
{
	Indexit(aFile->Tell());
}

void CFileIndex::IndexEOF( DFile* aFile)
{
	Indexit(aFile->Tell());
	fNum--;
}
	


	

void DSeqList::DoWrite( DFile* aFile, short format)  // change to iostreams !!
{
	Boolean		needSameSize = false, sizesDiffer = false, isInterleaved = false;
	short 		iseq, nseqs;
	short			seqtype = DSequence::kOtherSeq;
	long  		minbases = -1;
	long	 		start, nbases;
	DSequence * aSeq;
	DFile 		* outfile;
	DFile			* tmpfile = NULL;
	CFileIndex aFileIndex;

	
	nseqs= this->GetSize();
	outfile= aFile;
	
	if (nseqs>1) // deal w/ one-per-file formats 
		switch (format) {
			case DSeqFile::kGCG			: format= DSeqFile::kMSF; break;
			case DSeqFile::kStrider	: format= DSeqFile::kIG; break;
			case DSeqFile::kNoformat:
			case DSeqFile::kPlain:
			case DSeqFile::kUnknown : format= DSeqFile::kGenBank; break;
			default				:	break;
			}
		
	//this->Each( findMinSize);	
	for (iseq= 0; iseq<nseqs; iseq++) {
		aSeq= this->SeqAt(iseq);
		aSeq->GetSelection( start, nbases);
		seqtype= aSeq->Kind();
		//if (minbases<0) minbases= nbases;
		if (minbases<=0) minbases= nbases; // ?? temp fix problem w/ empty seq
		else {
			if (nbases!=minbases) sizesDiffer= true;
			minbases= Min( nbases, minbases);
			}
		}
		
	if (!gOutputName) gOutputName= (char*)aFile->GetShortname();
	DSeqFile::WriteSeqHeader( aFile, format, nseqs, minbases, gOutputName, needSameSize, isInterleaved);
	gOutputName= NULL;
	
	if (isInterleaved) {
		tmpfile= new DTempFile();
		outfile= tmpfile;
		}
	
	if ((needSameSize || isInterleaved) && sizesDiffer) {
		//this->Each( SetSameSize);
		for (iseq= 0; iseq<nseqs; iseq++) {
			aSeq= this->SeqAt(iseq);
			aSeq->GetSelection( start, nbases);
			aSeq->SetSelection( start, minbases); 
			}
		}

	if (gPretty.isactive && gPretty.numtop) {
		aSeq= this->SeqAt(nseqs-1);
		gPretty.numline = 1;
		if (isInterleaved) aFileIndex.Indexit(outfile);
		aSeq->DoWriteSelection( outfile, format);
		gPretty.numline = 2;
		if (isInterleaved) aFileIndex.Indexit(outfile);
		aSeq->DoWriteSelection( outfile, format);
		gPretty.numline = 0;
		}

	for (iseq= 0; iseq<nseqs; iseq++) {
		aSeq= this->SeqAt(iseq);
		if (isInterleaved) aFileIndex.Indexit(outfile);
		aSeq->DoWriteSelection( outfile, format);
		}
				
	if (gPretty.isactive && gPretty.numbot) {
		aSeq= this->SeqAt(nseqs-1);
		gPretty.numline = 2;
		if (isInterleaved) aFileIndex.Indexit(outfile);
		aSeq->DoWriteSelection( outfile, format);
		gPretty.numline = 1;
		if (isInterleaved) aFileIndex.Indexit(outfile);
		aSeq->DoWriteSelection( outfile, format);
		gPretty.numline = 0;
		}

	if (isInterleaved) {
		aFileIndex.IndexEOF( outfile);
		//tmpfile->Close();   // don't need
		//tmpfile->Open("r"); // don't need
		DSeqFile::DoInterleave( aFile, tmpfile, format, seqtype, nseqs,
									minbases, gLinesPerSeqWritten, 
									aFileIndex.IndexCount(), aFileIndex.Indices());
		outfile= aFile;
		//tmpfile->Close(); // don't need
		tmpfile->Delete();  // debug tmp
		}
		
	DSeqFile::WriteSeqTrailer( aFile, format, this->GetSize(), gLinesPerSeqWritten, minbases);
}



void DSeqList::DoWrite( char* aFileName, short format)  
{
	gOutputName= aFileName;
	DFile* aFile= new DFile( aFileName, "w");
	aFile->Open();
	DoWrite( aFile, format);
	aFile->Close();
}


#if 0
Handle DSeqList::doWriteHandle( integer format)  
VAR
		Handle		aHand, handSave;
		Boolean		needSameSize, sizesDiffer, isInterleaved;
		integer		aRow, nseqs, seqtype, err, vref, tempFileRef;
		longint		minbases, dirID;
		
	void findMinSize( DSequence* aSeq)
	longint		var  start, nbases;
	{
		aSeq->GetSelection( start, nbases);
		seqtype= aSeq->fKind;
		if (minbases<0) then minbases= nbases
		else {
			if (nbases!=minbases) then sizesDiffer= true;
			minbases= min( nbases, minbases);
			}
	}
	
	void SetSameSize( DSequence* aSeq)
	longint		var  start, nbases;
	{
		aSeq->GetSelection( start, nbases);
		aSeq->SetSelection( start, minbases);
	}
	
	void writeRangeToHand( DSequence* aSeq)
	longint		VAR index;
	{
		if (isInterleaved) then begin
			if (0!=GetFPos( tempFileRef, index)) then ;
			Indexit(index);
			aSeq->doWriteSelection( tempFileRef, format);
			end
		else
			aSeq->doWriteSelectionHandle( aHand, format);
	}
		
	FailInfo		VAR  fi;
	void HndlFailure(OSErr error, long message)		
	{
		IndexCleanup(); 
		aHand= DisposeIfHandle( aHand);
	}
	
	
{
	gOutindex= NULL; //make sure for failure
	aHand= NULL;
	minbases= -1; 
	needSameSize= false;
	isInterleaved= false;
	sizesDiffer= false;
	seqtype= kOtherSeq;
	nseqs= this->GetSize();
	this->Each( findMinSize);		

	if (nseqs>1)) // deal w/ one-per-file formats 
		switch (format) {
			kGCG		: format= kMSF;
			kStrider: format= kIG;
			kNoformat, kPlain, kUnknown: format= kGenbank;
			}

	CatchFailures(fi, HndlFailure);
	aHand= NewPermHandle(0);
	FailMemError();
	
	DSeqFile::WriteSeqHeader(longint(aHand), TRUE, format, nseqs, minbases, gOutputName,
			needSameSize, isInterleaved);
			
	if (isInterleaved) {
		IndexSetup();
		err= FindFolder(kOnSystemDisk,kTemporaryFolderType,kCreateFolder,vRef,dirID);
		if (err!=0)) { vref= gAppVolRef; dirID= gAppDirID; }
		if 0!=HDelete(vRef, dirID, kInterleaveTemp) then ;
		FailOSErr( HCreate( vRef, dirID, kInterleaveTemp, kSappSig, 'TEXT'));
		FailOSErr( HOpen( vRef, dirID, kInterleaveTemp, fsRdWrPerm, tempFileRef));
		}
	
	if (((needSameSize || isInterleaved/*?*/) && sizesDiffer)) {
		/*---
		ParamText('all sequences must have same # bases for this format', '', 'WriteSeq','');	
		Failure( -1, msgMyError); 
		---*/
		this->Each( SetSameSize);
		}

	this->Each( writeRangeToHand);		

	if (isInterleaved) {
		indexEOF( tempFileRef);
		DoInterleave( longint(aHand), TRUE/*isHandle*/, format, seqtype, nseqs,
									minbases, gLinesPerSeqWritten, 
									gNoutindex, Handle(gOutindex), tempFileRef);

		FailOSErr( FSClose(tempFileRef));
		if 0!=HDelete(vRef, dirID, kInterleaveTemp) then ;
		IndexCleanup();
		}

	WriteSeqTrailer(longint(aHand), TRUE, format, this->GetSize(), gLinesPerSeqWritten, minbases);

	Success(fi);	
	doWriteHandle= aHand;
}

#endif


void DSeqList::AddNewSeq()
{
	DSequence* aSeq = new DSequence();
	InsertLast( aSeq);
	//aSeq->fIndex= this->GetSize();
}


void DSeqList::ClearSelections()
{
	long i, nseq= GetSize();
	for (i= 0; i<nseq; i++)  
		SeqAt(i)->ClearSelection();
}

short DSeqList::ZeroOrigin()
{
	DSequence* aSeq;
	short shiftall= 0;
	long i, nseq= GetSize();
	
	for (i= 0; i<nseq; i++) {
		aSeq= SeqAt(i);
		if (aSeq->Origin()<0) shiftall= Min(shiftall, aSeq->Origin());
		}
		
	for (i= 0; i<nseq; i++) {
		aSeq= SeqAt(i);
		aSeq->SetOrigin(aSeq->Origin() - shiftall); // -= shiftall;
		if (aSeq->Origin() > 0) {
			aSeq->InsertSpacers( 0, aSeq->Origin(), DSequence::indelEdge);
			aSeq->SetOrigin(0);
			}
		}
	return shiftall;
}



short dslCompareByName( void* item1, void* item2)
{
	char*	name1= (char*) ((DSequence*)item1)->Name();
	char*	name2= (char*) ((DSequence*)item2)->Name();
	return StrICmp( name1, name2);
}

short dslCompareBySize( void* item1, void* item2)
{
		// inverted sort, largest 1st
	long name1= ((DSequence*)item1)->LengthF();
	long name2= ((DSequence*)item2)->LengthF();
	if (name1 > name2) return -1;
	else if (name1 < name2) return 1;
	else  return 0;
}

short dslCompareByKind( void* item1, void* item2)
{
	short name1= ((DSequence*)item1)->Kind();
	short name2= ((DSequence*)item2)->Kind();
	if (name1 > name2) return 1;
	else if (name1 < name2) return -1;
	else  return 0;
}

short dslCompareByDate( void* item1, void* item2)
{
		// inverted sort, latest 1st
	unsigned long name1= ((DSequence*)item1)->ModTime();
	unsigned long name2= ((DSequence*)item2)->ModTime();
	if (name1 > name2) return -1;
	else if (name1 < name2) return 1;
	else  return 0;
}


Boolean DSeqList::SortList(Sorts sortorder)
{
	if (sortorder == fSortOrder)
		return false;
	else {
		fSortOrder= sortorder;
		switch( sortorder) {
			case kSortByKind :
				this->SortBy( dslCompareByKind);
				break;
			case kSortBySize : 
				this->SortBy( dslCompareBySize);
				break;
			case kSortByDate :
				this->SortBy( dslCompareByDate);
				break;
			case kSortByName : 
				this->SortBy( dslCompareByName);
				break;
			case kSortByItem: 
			default:
				this->Sort();
				break;
			}
		return true;
		}
}

DSequence* DSeqList::FindNamedSeq(char* name, Boolean respectCase)
{	
	if (name && *name) {
		DSequence* aSeq;
		long i, nseq= GetSize();
		if (respectCase) for (i= 0; i<nseq; i++) {
			aSeq= SeqAt(i);
		  if (Nlm_StringCmp(aSeq->Name(), name)==0) return aSeq;
			}
		else for (i= 0; i<nseq; i++) {
			aSeq= SeqAt(i);
			if (Nlm_StringICmp(aSeq->Name(), name)==0) return aSeq;
			}
		}
	return NULL;	
}

DSequence* DSeqList::Consensus(void)
{	
	long i, nseq= GetSize();
	for (i= 0; i<nseq; i++) {
		DSequence* aSeq= SeqAt(i);
		if (aSeq->IsConsensus()) return aSeq;
		}
	return NULL;	
}


short DSeqList::ConsensusRow()
{
	DSequence* cons= this->Consensus();		
	if (cons) 
		return this->GetIdentityItemNo( cons);
	else
		return -1;
}



	
void DSeqList::MakeConsensus()
{		
	DSequence* cons= this->Consensus();		
	if (!cons) {
		cons = new DSequence(); 
		cons->SetName(DSequence::kConsensus);
		InsertLast( cons); 
		}
		
	if (cons) {
		//short arow= this->GetIdentityItemNo( cons);
		char* hCon= cons->Bases();   
		long conlen= StrLen( hCon); //?? use aSeq->fBaseLength;
		long count= 0;
		long i, iseq, nseq= GetSize();
		for (iseq= 0; iseq<nseq; iseq++) {
			DSequence* aSeq= this->SeqAt(iseq);
			if (!aSeq->IsConsensus() && aSeq->Kind() != DSequence::kOtherSeq) {
				count++;
				char* hSeq= aSeq->Bases();
				long  len = aSeq->LengthF();
				if (len > conlen) {
					conlen= len;
					hCon= (char*) MemMore( hCon, conlen);
					cons->UpdateBases( hCon, conlen);
					}
				if (count==1) {
					for (i= 0; i<len; i++) hCon[i]= hSeq[i];
					}			
				else {
					for (i= 0; i<len; i++) 
// !! THis is only good for Nucleic bases, not for Aminos !!
// change to table of matches !!
						hCon[i]= DSequence::NucleicConsensus(
							DSequence::NucleicBits(hCon[i]), 
							DSequence::NucleicBits(hSeq[i]));  
					}
				}
			}
		}
}





char* DSeqList::Distances( short correct, Boolean doSimilarity)
{  
	struct NucCounts {
		float apct, cpct, gpct, tpct;
		};
	long iseq, nseq= GetSize();
	NucCounts* nuccounts = NULL;
	char	linebuf[256], * buf = NULL;
	char	name0[128];
	
	if (!nseq) return NULL;
	if (correct == DColsen) nuccounts= new NucCounts[ nseq];
	buf= (char*) MemNew(1);
	*buf= 0;
	*name0= 0;
	
	if (correct == DColsen) {
		for (iseq=0; iseq<nseq; iseq++) {
			long	na, nc, ng, nt, ntotal;
			DSequence* aSeq= this->SeqAt(iseq);
			char* bases= aSeq->Bases();
			long  ibase, nbases= aSeq->LengthF();
			na= nc= ng= nt= ntotal=0;
					// create Olsen correction array
			for (ibase=0; ibase<nbases; ibase++) {
				long baseval= DSequence::NucleicBits(bases[ibase]);
				if (baseval & DSequence::kMaskA) na++;
				if (baseval & DSequence::kMaskC) nc++;
				if (baseval & DSequence::kMaskG) ng++;
				if (baseval & DSequence::kMaskT) nt++;
				if (baseval) ntotal++;
				}
			nuccounts[iseq].apct = (float) na / (float) ntotal;
			nuccounts[iseq].cpct = (float) nc / (float) ntotal;
			nuccounts[iseq].gpct = (float) ng / (float) ntotal;
			nuccounts[iseq].tpct = (float) nt / (float) ntotal;
			}
		}
	
	for (long j=1; j<nseq; j++)
		for (long k=0; k<j; k++) {
			long jk;
			long numer= 0, denom= 0;
			DSequence* jseq= this->SeqAt( j);
			DSequence* kseq= this->SeqAt( k);
			long len= MIN( jseq->LengthF(), kseq->LengthF());
			for (jk= 0; jk < len; jk++) {
				long jval= jseq->NucleicBits(jseq->Bases()[jk]);
				long kval= kseq->NucleicBits(kseq->Bases()[jk]);
				if ( jval & kval) numer++;
				if ( jval | kval) denom++;
				}
				
			float distv, cor;
			switch (correct) {
			  case DColsen: 
		  		cor = nuccounts[j].apct * nuccounts[k].apct
		  		    + nuccounts[j].cpct * nuccounts[k].cpct
		  		    + nuccounts[j].gpct * nuccounts[k].gpct
		  		    + nuccounts[j].tpct * nuccounts[k].tpct;
					break;
				case DCjukes: cor = 0.25; break;
				case DCnone: 
				default: 			cor = 0.0;  break;
				}
		
			if (correct == DCnone)
				distv= 1.0 - (float)numer / (float)denom;
			else
				distv= -(1.0-cor) * log( 1.0 / (1.0 - cor)
					* ((float)numer / (float)denom - cor) );
			
			if (k == 0) {
				if (j == 1) {  
					sprintf( name0, "%2d=%-15s",k+1,kseq->Name());
					//StrExtendCat( &buf, linebuf);
					}
				sprintf( linebuf, "%2d:%-15s|",j+1,jseq->Name());
				StrExtendCat( &buf, linebuf);
				}
				
			if (doSimilarity)
				sprintf(linebuf, "%6.1f",100 - distv*100.0);
			else
				sprintf(linebuf, "%6.1f",distv*100.0);
			StrExtendCat( &buf, linebuf);
		
			if (k == j-1) {
				StrExtendCat( &buf, " \n");
				}
				
			}

	sprintf( linebuf, "   %-15s ","-------------");
	StrExtendCat( &buf, linebuf);
	for (iseq=1; iseq<nseq; iseq++) {
		sprintf(linebuf, " %5s", "----");
		StrExtendCat( &buf, linebuf);
		}
	sprintf( linebuf, "\n%-18s ",name0);
	StrExtendCat( &buf, linebuf);
	for (iseq=1; iseq<nseq; iseq++) {
		sprintf(linebuf, "%5d ",iseq);
		StrExtendCat( &buf, linebuf);
		}
		
	char * cors, *skind;
	switch (correct) {
		case DCjukes: cors= "(Jukes correction)"; break;
		case DColsen: cors= "(Olsen correction)"; break;
		default:
		case DCnone:  cors= ""; break;
		}
	if (doSimilarity) skind= "Similarity"; else skind="Distance";
	sprintf( linebuf, "\n  %s matrix %s\n", skind, cors);
	StrExtendCat( &buf, linebuf);
			
	delete[] nuccounts;
	return buf;
}
 
 
 
 
 
char* DSeqList::FindCommonBases(short minCommonPerCent, char*& firstCommon)
{  
	enum { kLetrange = 26, kLetlast = 255 };
	char    * hSeq,  * hMaxbase;
	char		ch;
	long		len, maxlen, ibase;
	short		spccount, nilcount;
	short		maxlet, let, nseq, maxcnt;
	short		letcount[kLetrange], letfirst[kLetrange];
	float 	rmaxc;
 
	DSequence* cons= this->Consensus();		
	if (!cons) cons= (DSequence*) this->First(); //any seq will do...
	
	hMaxbase= NULL;
	firstCommon= NULL;
	if (cons) {		
		maxlen= cons->LengthF();
		hMaxbase= (char*) Nlm_MemGet(maxlen+1,true); 
		firstCommon= (char*) Nlm_MemGet(maxlen+1,true); 
		
		for (ibase= 0; ibase<maxlen; ibase++) {
			for (let= 0; let < kLetrange; let++) { letcount[let]= 0; letfirst[let]= kLetlast; }
			nilcount= 0; 
			spccount= 0;
			nseq= 0;
			
			long iseq, maxseq= GetSize();
			for (iseq=0; iseq<maxseq; iseq++) {
				DSequence* aSeq= this->SeqAt(iseq);
				if (!aSeq->IsConsensus() && aSeq->Kind() != DSequence::kOtherSeq) {
					hSeq= aSeq->Bases();
					len= aSeq->LengthF();
					if (ibase<len) ch= toupper( hSeq[ibase]);
					else ch= 0;
					if (ch >= 'A' && ch <= 'Z') {
						let = ch - 'A';
						letcount[let]++;
						if (letfirst[let] == kLetlast) letfirst[let]= Min(kLetlast, iseq);
						}
					else if (
					     ch == DSequence::indelSoft 
						|| ch == DSequence::indelHard 
						|| ch == DSequence::indelEdge 
						|| ch == ' ' || ch == '_' )  
						spccount++;
					else 
						nilcount++;
					nseq++;
					}
				}
				
			for (let= 0, maxlet= 0, maxcnt= -1; let < kLetrange; let++) 
				if (letcount[let] > maxcnt) { 
					maxcnt= letcount[let]; 
					maxlet= let; 
					}
					
			if (maxcnt>0) {
				firstCommon[ibase]= letfirst[maxlet];
				rmaxc= (100.0 * maxcnt) / nseq;
				if (rmaxc >= minCommonPerCent)   //mark maxlet's
					hMaxbase[ibase]= maxlet + 'A';
				else
					hMaxbase[ibase]= '!';
				}
			else {
				firstCommon[ibase]= kLetlast;
				hMaxbase[ibase]= '!';
				}
			}
			
		hMaxbase[maxlen]= 0;
		firstCommon[maxlen]= 0;
		}
	
	return hMaxbase;
}
 
