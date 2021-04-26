// DSeqFile.h 
// by d.g.gilbert, Oct 1990 


#ifndef __DSEQFILE__
#define __DSEQFILE__


		

#include "DSequence.h"
#include <DFile.h>
#include <DList.h>

class DSeqList;
class DSequence;

	// reader/writer formats

class DSeqFile 
{
public:

 enum seqformats {
	 kIG            =  1,
	 kGenBank       =  2,
	 kNBRF          =  3,
	 kEMBL          =  4,
	 kGCG           =  5,
	 kStrider       =  6,
	 kFitch         =  7,
	 kPearson       =  8,
	 kZuker         =  9,
	 kOlsen         =  10,
	 kPhylip2       =  11,
	 kPhylip4       =  12,
	 kPhylip3       =  12, //kPhylip4,
	 kPhylip        =  12, //kPhylip4,
	 kPlain         =  13, /* keep this at #13  */
	 kPIR           =  14,
	 kMSF           =  15,
	 kASN1          =  16,
	 kPAUP          =  17,
	 kPretty        =  18,
	
	 kMaxFormat     =  18,
	 kMinFormat     =  1,
	 kNoformat      =  -1,    /* format not tested */
	 kUnknown       =  0,     /* format not determinable */
	
		/* subsidiary types */
	 kASNseqentry   =  51,
	 kASNseqset     =  52,
	 kPhylipInterleave = 61,
	 kPhylipSequential = 62 
	 };

	static Boolean gWriteMasks;
	static char* fgMacSire;
	
	static void DontSaveMasks();
	static void DoSaveMasks();

	static const char* FormatName(short index); 
	static short FormatFromName(const char* name); 

	static DSequence* MakeSequence(char* name, char*& bases, char* info, long modtime);
	
	static short SeqFileFormatWrapper(DFile* aFile); 
	
	static short ReadSeqFile( DFile* aFile, DSeqList* aSeqList); 
	
	static void	WriteSeqHeader( DFile* aFile,
								short format, short nseqs, long nbases,	char* outputName,
								Boolean& needSameSize, Boolean& isInterleaved);
								
	static void	WriteSeqTrailer(  DFile* aFile,
								short format, short nseqs, short nlinesperseq, long nbases);
								
	static void WriteSeqWrapper(  DFile* aFile,
								char *seq, long seqlen, short outform, char *seqid);

	static void WriteMaskWrapper(  DFile* aFile,
								char *mask, long seqlen, short outform, char *seqid);
																
	static void	DoInterleave( DFile* aFile, DFile* fromFile,
								short outform, short seqtype, short nseqs, long nbases,
								short nlines, short noutindex, long* outindex);
private:
	static char*  FixSeqID( char* s, short leftmargin = 0);
	static Boolean gSaveMasks;
};


extern char* gOutputName;

#endif
