// DMegaSequence.h 
// by d.g.gilbert, Oct 1990 -- 1996


#ifndef __UMEGASEQUENCE__
#define __UMEGASEQUENCE__


#include <DSequence.h>
			
//typedef	char 		SeqName[kNameWidth+1];

class	DMegaSequence : public DSequence {

// for LARGE sequences (up to ? 10+ megabases .. suitable for at least microbial genomes)

protected:
	long fNparts, fPartsize;
	void countparts( char* seq, long& len, long& nparts, long& partsize);

public:
	enum { kMegaPartSize = 32000 }; // => 10 parts= 320kb seq, 100 parts= 3.2MB seq...

	DMegaSequence();
	virtual ~DMegaSequence();

	virtual DObject* Clone(); // override 
	virtual void CopyContents( DSequence* fromSeq);
	
	virtual char* Bases() const;
	virtual char* Masks() const;
	
	virtual void SetBases( char*& theBases, Boolean duplicate = false);
	virtual void UpdateBases( char* theBases, long theLength); // caller must know what he does  
	virtual void SetMasks( char*& theMasks, Boolean duplicate = false);
	virtual void FixMasks();

	virtual void SetSelection( long start, long nbases);
	virtual void GetSelection( long& start, long& nbases);
	virtual void ClearSelection();
	virtual void InsertSpacers( long insertPoint, long insertCount, char spacer);
	
	virtual DSequence* CopyRange();
	virtual DSequence* CutRange();
	virtual DSequence* PasteBases( long insertPoint, char* fromBases, char* fromMasks);
	virtual DSequence* Reverse();
	virtual DSequence* Complement();
	virtual DSequence* Dna2Rna( Boolean toRna);
	virtual DSequence* ChangeCase( Boolean toUpper);
	virtual DSequence* Translate(Boolean keepUnselected = true);
	virtual DSequence* Slide( long slideDist);
	virtual DSequence* Compress();
	virtual DSequence* CompressFromMask(short masklevel);
	virtual DSequence* LockIndels( Boolean doLock);
	virtual DSequence* OnlyORF(char nonorf = '~');

	virtual long Search( char* target, Boolean backwards);
	virtual void SearchORF( long& start, long& stop); 
	virtual void NucleicSearch(char* source, char* target, SearchRec& aSearchRec);
	virtual void ProteinSearch(char* source, char* target, SearchRec& aSearchRec);

	virtual void UpdateFlds(); 
	
	virtual short MaskAt(long baseindex, short masklevel= 1);
	virtual void  SetMaskAt(long baseindex, short masklevel= 1, short maskval= 1);
	virtual void  FlipMaskAt(long baseindex, short masklevel= 1);
	virtual void  SetMask(short masklevel= 1, short maskval= 1); 
	virtual void  FlipMask(short masklevel= 1); 
	virtual Boolean IsMasked(unsigned char maskbyte, short masklevel);

	virtual void DoWrite( DFile* aFile, short format);  // revise for ostream
	virtual void DoWriteSelection( DFile* aFile, short format); // revise for ostream
};


#endif
