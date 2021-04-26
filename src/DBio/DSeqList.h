// DSeqList.h

#ifndef _DSEQLIST_
#define _DSEQLIST_

#include <DList.h>
//#include "DSeqFile.h"

class DFile;
class DSequence;

class DSeqList : public DList //, public DSeqFile
{
public:
	enum Sorts {
		kSortByItem = 0,
		kSortByName = 1,
		kSortBySize = 2,
		kSortByKind = 3,
		kSortByDate = 4
		};
	enum DistanceCorrection { DCnone, DCjukes, DColsen };
	short fSortOrder;
	static short gMinCommonPercent, gMinORFsize;

	DSeqList();
	virtual ~DSeqList();

	virtual Boolean IsEmpty();
	virtual DSequence* SeqAt( short index) { return (DSequence*) this->At( index); }

	virtual void DoWrite( DFile* aFile, short format); //ostream os
	virtual void DoWrite( char* aFileName, short format); 
	
	virtual Boolean SortList(Sorts sortorder);
	virtual void AddNewSeq();
	virtual void ClearSelections();
	
	virtual short ZeroOrigin();
	virtual void MakeConsensus();
	virtual DSequence* Consensus();   
	virtual short ConsensusRow();  
	virtual DSequence* FindNamedSeq(char* name, Boolean respectCase = true);
	virtual char* FindCommonBases( short minCommonPerCent, char*& firstCommon);
	virtual char* Distances( short correct = DCnone, Boolean doSimilarity = false);
	virtual char* Similarities( short correct = DCnone) 
											  { return Distances( correct, true); }
};


extern short gLinesPerSeqWritten;

#endif
