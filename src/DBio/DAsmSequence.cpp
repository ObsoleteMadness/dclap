// DAsmSequence.cp 
// d.g.gilbert



#include "DAsmSequence.h"
#include "DSeqFile.h"
#include "DSeqList.h"
#include "DREnzyme.h"
#include <ncbi.h>
#include <dgg.h>
#include <DUtil.h>
#include <DFile.h>
#include "ureadseq.h"
 





// DAsmSequence -----------------


DAsmSequence::DAsmSequence()
{
	fTrace= NULL;  
	fTFName= NULL;
  fNTrace= 0;
  fMaxTrace= 0;
  fTrCenter= NULL;
  fTrlength= 0;
	fShowTrace= TRUE;
	fKind= kAutoseq;  
	for (short t=0; t<4; t++) fMaxHeight[t]= 0;
}

DAsmSequence::~DAsmSequence()  
{
	delete[] fTrace; //MemFree(fTrace);
	MemFree(fTrCenter);
	MemFree(fTFName);
}

DObject* DAsmSequence::Clone() // override 
{	
	DAsmSequence* aSeq= (DAsmSequence*) DSequence::Clone();
	aSeq->fTrace= NULL;  aSeq->SetTrace( fTrace, fNTrace, fMaxTrace, true);  
	aSeq->fTrCenter= NULL; aSeq->SetTrCenter( fTrCenter, fTrlength, true); 
	aSeq->fTFName= StrDup( fTFName);    
	return aSeq;
}

void DAsmSequence::CopyContents( DAsmSequence* fromSeq)
{
	DSequence::CopyContents( fromSeq);
	fTFName= StrDup(fromSeq->fTFName);
	SetTrace( fromSeq->fTrace, fromSeq->fNTrace, fromSeq->fMaxTrace, true);  
	SetTrCenter( fromSeq->fTrCenter, fromSeq->fTrlength, true); 
}

const char* DAsmSequence::TracefileName() 
{ 
	return fTFName;
}

void DAsmSequence::SetTracefileName(char* name) 
{ 
	MemFree( fTFName);
	fTFName= StrDup( name);
}

TraceBase DAsmSequence::TraceAt(long item) 
{ 
	TraceBase tr;
	if (item >=0 && item <= fNTrace) return fTrace[item];
	else return tr;
}

void DAsmSequence::SetTraceAt(long item, TraceBase tr) 
{ 
	if (item >=0 && item <= fNTrace) fTrace[item] = tr;
}

void DAsmSequence::TraceToBases() 
{ 
	if (fNTrace && fTrace) {
		char *buf = (char*) MemNew( fNTrace+1);
		long	i, len= 0;
		for (i=0; i<fNTrace; i++)  
			if (fTrace[i].base > ' ')  
				buf[len++]= fTrace[i].base;
		buf[len]= 0;
		buf= (char*) MemMore( buf, len+1); // size down
		SetBases( buf, false);
		}
}

short DAsmSequence::MaxHeight(short trace) 
{ 
	if (trace>=0 && trace<4) return fMaxHeight[trace];
	else {
		short maxt= 0;
		for (short t=0; t<4; t++) if (fMaxHeight[t]>maxt) maxt= fMaxHeight[t];
		return maxt;
		}
}

void  DAsmSequence::SetMaxHeight(short trace, short max) 
{ 
	if (trace>=0 && trace<4) fMaxHeight[trace]= max; 
}

void DAsmSequence::SetTrCenter( short*& theTrCenter, long trlength, Boolean duplicate)
{
	ulong bytesize= trlength * sizeof(short);
	if (fTrCenter) { 
		if (fTrlength>0 && 
			(fTrlength != trlength || MemCmp( theTrCenter, fTrCenter, bytesize)!= 0))
				fChanged= TRUE;
		MemFree(fTrCenter); 
		fTrCenter= NULL; 
		fTrlength= 0;
		}
	if (theTrCenter) {
		if (duplicate) fTrCenter = (short*) MemDup(theTrCenter, bytesize); 
		else { fTrCenter= theTrCenter; theTrCenter= NULL; }
		fTrlength= trlength;  
		}
	fModTime= gUtil->time();
}


void DAsmSequence::UpdateTrace( TraceBase*& theTrace, long npoints, long maxtrace)
{
	ulong bytesize= npoints * sizeof(TraceBase);
	
	if (theTrace && theTrace != fTrace) {
		SetTrace( theTrace, npoints, maxtrace, false);
		theTrace= fTrace;
		return;
		}
	
	if (maxtrace > fMaxTrace) {
		TraceBase* tmpb= new TraceBase[ maxtrace];
		if (!tmpb) return;
		if (fTrace && bytesize) {
			MemCpy( tmpb, fTrace, bytesize);
			delete[] fTrace;
			}
		fTrace= tmpb;
		theTrace= fTrace;
		fMaxTrace= maxtrace;
		}
	// ?? reduce fTrace if (maxtrace < fMaxTrace) ??

	fNTrace= Min( fMaxTrace, npoints);
	fModTime= gUtil->time();
}


void DAsmSequence::SetTrace( TraceBase*& theTrace, long npoints, long maxtrace,
							Boolean duplicate)
{
	ulong bytesize= npoints * sizeof(TraceBase);
	if (fTrace) { 
		if (fNTrace>0 && 
			(fNTrace != npoints || MemCmp( theTrace, fTrace, bytesize)!= 0))
				fChanged= TRUE;
		delete[] fTrace;  //MemFree(e);  
		fTrace= NULL; 
		fNTrace= 0;
		fMaxTrace= 0; 
		}
	if (theTrace) {
		if (duplicate) {
			fTrace = new TraceBase[ maxtrace];
			MemCpy( fTrace, theTrace, bytesize);
			}
		else { fTrace= theTrace; theTrace= NULL; }
		fNTrace= npoints;   
		fMaxTrace= maxtrace; 
		}
	fModTime= gUtil->time();
}






void DAsmSequence::DoWrite(DFile* aFile, short format)
{  
	DSequence::DoWrite( aFile, format);
#if 0	
	if (fLength>0) {
		DSeqFile::WriteSeqWrapper( aFile, fBases, fLength, format, fName);
		if (fMasks && fMasksOk && DSeqFile::gWriteMasks) 
			DSeqFile::WriteMaskWrapper( aFile, fMasks, fLength, format, fName);
		}
#endif
}


void DAsmSequence::DoWriteSelection(DFile* aFile, short format)
{
	DSequence::DoWriteSelection( aFile, format);
#if 0	
	if (fSelBases==0) 
		DoWrite( aFile, format);
	else if (fLength>0) {
		long aStart= Min( fSelStart, fLength);
		long aLength= Min( fSelBases, fLength - fSelStart);
		DSeqFile::WriteSeqWrapper(  aFile, fBases+aStart, aLength, format, fName);
		if (fMasks && fMasksOk && DSeqFile::gWriteMasks)  
			DSeqFile::WriteMaskWrapper( aFile, fMasks+aStart, aLength, format, fName);
		}
#endif
}




