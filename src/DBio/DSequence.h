// DSequence.h 
// by d.g.gilbert, Oct 1990 -- 1994


#ifndef __USEQUENCE__
#define __USEQUENCE__


#include <DObject.h>
			
//typedef	char 		SeqName[kNameWidth+1];

struct SearchRec {
	long		lim,indx,step;
	char* 	targ;
	}; 

	
class DFile;

class	DSequence : public DObject {
private:		
	char* 			fInfo; 				//sequence documentation 
	char* 			fBases;				//the bases
	char* 			fMasks;				//style masks
	char* 			fName;  
	short 			fKind;				//dna/rna/amino/...
	long 				fLength;			//base count
	unsigned long	fChecksum;	//checksum for the bases
	unsigned long	fModTime;		//date/time last modified
	long 				fOrigin;			//offset of start from 0
	long 				fSelStart;		//selection start -- tmp
	long 				fSelBases;		//selection size
	short 			fIndex;				//index into SeqList (temp?)
	SearchRec		fSearchRec;
	Boolean 		fDeleted:1;		//? Whether the seq has been deleted 
	Boolean 		fOpen:1;			//? seq is open in a view
	Boolean 		fChanged:1;		//modified since updt
	Boolean			fMasksOk:1;		//if fMasks is in a good state

public:
	static char* kConsensus;
	static char	indelHard, indelSoft, indelEdge;
	
	enum seqType {
		kDNA 			= 1,
		kRNA 			= 2,
		kNucleic  = 3, //IUB/IUPAC codes
		kAmino 		= 4,
		kOtherSeq	= 5
		};
	enum nucBits {
		/* NucleicBits (values in 0..31):  
				%00001 = A
				%11000 = kMaskU = U (rna) 
				%10000 = kMaskIndel = indel (-)
				%01111 = kMaskNucs = N or .
				%00000 = non-nucleic */
		kBitA	= 0,  kMaskA = 1, 	
		kBitC = 1,	kMaskC = 2,
		kBitG	= 2,	kMaskG = 4,
		kBitT	= 3,	kMaskT = 8,		
		kBitExtra = 4, kMaskExtra = 16,
		kMaskIndel= 16, //kMaskExtra,
		kMaskU 		= 8+16, kMaskRna = 8+16, // kMaskT + kMaskExtra, kMaskRna = kMaskU,
		kMaskNucs = 1+2+4+8, //kMaskA + kMaskC + kMaskG + kMaskT,
		kBit5 = 5,  kMask5 = 32,  //unused NucleicBits in byte
		kBit6 = 6,	kMask6 = 64,
		kBit7 = 7,	kMask7 = 128
		};
	enum {
		kNameWidth	= 31,
		kMaxSeqShow = 10,
		kMaxSname	= 20
		};

	DSequence();
	virtual ~DSequence();

	static long 	NucleicBits( char nuc);
	static Boolean IsNucleicMatch( long xNucBits, long yNucBits);
	static char 	NucleicConsensus( long xNucBits, long yNucBits);
	static void 	NucleicComplement( Boolean isrna, char* fromSeq, char* toSeq, long len);
	static char* 	Nucleic23Protein( char* nucs, long nbases);
	static const char* 	Amino123( char amino1);
	static char  	Amino321( char* amino);

	virtual DObject* Clone(); // override 
	virtual void CopyContents( DSequence* fromSeq);
	
	char* Bases() const { return fBases; }
	char* Masks() const { return fMasks; }
	char* Name() const { return fName; }
	char* Info() const { return fInfo; }
	long 	LengthF() const { return fLength; }
	short Kind() const { return fKind; }
	short Index() const { return fIndex; }
	char* KindStr() const;
	Boolean Changed() const { return fChanged; }
	unsigned long ModTime() const { return fModTime; }
	unsigned long Checksum() const { return fChecksum; }
	long Origin() const { return fOrigin; }
	Boolean IsAmino() { return (fKind == kAmino); }
	Boolean IsDeleted() { return fDeleted; }
	
	void SetBases( char* theBases);
	void UpdateBases( char* theBases, long theLength); // caller must know what he does  
	void SetMasks( char* theMasks);
	void FixMasks();
	void SetInfo( char* theInfo);
	void SetName( char* theName);
	void SetKind( short theKind);
	void SetIndex( short theIndex);
	void SetTime( unsigned long theTime);
	void SetOrigin( long theOrigin);
	void SetDeleted( Boolean turnon) { fDeleted= turnon; }
	
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
	virtual DSequence* LockIndels( Boolean doLock);

	virtual long Search( char* target, Boolean backwards);
	virtual long SearchAgain(); 
	virtual void SearchORF( long& start, long& stop); 
	virtual void NucleicSearch(char* source, char* target, SearchRec& aSearchRec);
	virtual void ProteinSearch(char* source, char* target, SearchRec& aSearchRec);

	virtual Boolean IsConsensus();
	virtual Boolean GoodChar(char& aChar); 
	virtual void Reformat(short format);
	virtual void Modified();  //set fModTime
	virtual void UpdateFlds(); 
	
	Boolean MasksOk() { return fMasksOk; }
	virtual short MaskAt(long baseindex, short masklevel= 1);
	virtual void  SetMaskAt(long baseindex, short masklevel= 1, short maskval= 1);
	virtual void  FlipMaskAt(long baseindex, short masklevel= 1);
	virtual void  ClearMaskAt(long baseindex, short masklevel= 1) 
								{ SetMaskAt(baseindex, masklevel, 0); }
	virtual void  SetMask(short masklevel= 1, short maskval= 1); 
	virtual void  FlipMask(short masklevel= 1); 
	virtual void  ClearMask(short masklevel= 1) { SetMask(masklevel, 0); }

	virtual void DoWrite( DFile* aFile, short format);  // revise for ostream
	virtual void DoWriteSelection( DFile* aFile, short format); // revise for ostream
};


extern char*	gDefSeqName;
extern short  gLinesPerSeqWritten;
	
DSequence* MakeSequence(char* name, char* bases, char* info, long modtime);
	

#endif
