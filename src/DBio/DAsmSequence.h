// DAsmSequence.h 
// by d.g.gilbert, Oct 1990 -- 1994


#ifndef __DASMSEQUENCE__
#define __DASMSEQUENCE__


#include <DSequence.h>
			
			
struct TraceBase {
	enum { tA, tC, tG, tT, tMax };
	short tr[4]; 
	char	base;
	
	TraceBase( short Aht, short Cht, short Ght, short Tht, char aBase = 0)  
	{
		base= aBase;
		tr[tA]= Aht; tr[tC]=Cht; tr[tG]=Ght; tr[tT]=Tht;
	}

	TraceBase()   
	{
		base= 0;
		tr[tA]= tr[tC]= tr[tG]= tr[tT]=0;
	}
		
};


class	DAsmSequence : public DSequence {
public:
	enum asmseqType {
		kAutoseq	= kOtherSeq+1,
		};
		
protected:		
	TraceBase	* fTrace;   	// autoseq traces for A,C,G,T 
	long				fNTrace, fMaxTrace;	 // # points/trace  
	short			*	fTrCenter;		// base positions in traces (fLength == #bases)
	long				fTrlength;		// should! be same as fLength
	short				fMaxHeight[4];	// max trace value, for scaling
	char			* fTFName;
public:
	
	DAsmSequence();
	virtual ~DAsmSequence();

	virtual DObject* Clone(); // override 
	virtual void CopyContents( DAsmSequence* fromSeq);
	
	virtual TraceBase* Trace() const { return fTrace; }
	virtual TraceBase TraceAt(long item);
	virtual void 	SetTraceAt(long item,TraceBase tr); 
	virtual short* TrCenter() const { return fTrCenter; }
	virtual long 	NumPoints() { return fNTrace; }
	virtual long 	MaxPoints() { return fMaxTrace; }
	virtual void 	SetPoints(long np) { fNTrace= np; }
	virtual short MaxHeight(short trace);
	virtual void  SetMaxHeight(short trace, short max);
	virtual const char*  TracefileName();
	virtual void 	SetTracefileName(char* name); 
	virtual void  TraceToBases(); 

	virtual void SetTrace( TraceBase*& theTrace, long npoints, long maxtrace,
							Boolean duplicate = false);
	virtual void SetTrCenter( short*& theTrCenter, long trlength, 
							Boolean duplicate = false);
	virtual void UpdateTrace( TraceBase*& theTrace, long npoints, long maxtrace);
	
	virtual void DoWrite( DFile* aFile, short format);  // revise for ostream
	virtual void DoWriteSelection( DFile* aFile, short format); // revise for ostream
};


#endif
