/* File: ureadseq.h
 *
 * Header for module UReadSeq
 */

#ifndef UREADSEQ_H
#define UREADSEQ_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef boolean
#ifdef _NCBI_
#define boolean Nlm_Boolean
#else
typedef unsigned char boolean;
#endif
#endif

#ifndef false
#define false 0
#endif
#ifndef true
#define true  1
#endif

#ifndef NEWLINE
#define NEWLINE         '\n'
#endif
#ifndef min
#define min(a,b)      (a<b)?a:b
#endif
#ifndef max
#define max(a,b)      (a>b)?a:b
#endif
#ifndef skipwhitespace
#define skipwhitespace(string)  {while (*string <= ' ' && *string != 0) string++;}
#endif

  /* NLM strings */
#ifndef is_upper
#define is_upper(c) ('A'<=(c) && (c)<='Z')
#endif
#ifndef is_lower
#define is_lower(c) ('a'<=(c) && (c)<='z')
#endif
#ifndef to_lower
#define to_lower(c) ((char)(is_upper(c) ? (c)+' ' : (c)))
#endif
#ifndef to_upper
#define to_upper(c) ((char)(is_lower(c) ? (c)-' ' : (c)))
#endif


  /* readSeq errors */
#define eFileNotFound   -1
#define eNoData         -2
#define eMemFull        -3
#define eItemNotFound   -4
#define eOneFormat      -5
#define eUnequalSize    -6
#define eFileCreate     -7
#define eUnknownFormat  -8
#define eOptionBad      -9
#define eASNerr         -10

  /* magic number for readSeq(whichEntry) to give seq list */
#define kListSequences  -1


	/* name space collisions !! damn defines */
#ifdef UREADSEQ_G

  /* sequence types parsed by getseqtype */
#define kOtherSeq   0
#define kDNA        1
#define kRNA        2
#define kNucleic    3
#define kAmino      4

  /* formats known to readSeq */
#define kIG             1
#define kGenBank        2
#define kNBRF           3
#define kEMBL           4
#define kGCG            5
#define kStrider        6
#define kFitch          7
#define kPearson        8
#define kZuker          9
#define kOlsen          10
#define kPhylip2        11
#define kPhylip4        12
#define kPhylip3        kPhylip4
#define kPhylip         kPhylip4
#define kPlain          13  /* keep this at #13 */
#define kPIR            14
#define kMSF            15
#define kASN1           16
#define kPAUP           17
#define kPretty         18

#define kMaxFormat      18
#define kMinFormat      1
#define kNoformat       -1    /* format not tested */
#define kUnknown        0     /* format not determinable */

  /* subsidiary types */
#define kASNseqentry    51
#define kASNseqset      52

#define kPhylipInterleave 61
#define kPhylipSequential 62


#define kRSFile_Eof			1
#define kRSFile_Read		2
#define kRSFile_Write		3
#define kRSFile_Seek		4
#define kRSFile_Rewind		5
#define kRSFile_Tell		6

#endif


typedef struct {
  boolean isactive, baseonlynum,
		numright, numleft, numtop, numbot,
		nameright, nameleft, nametop,
		noleaves, domatch, degap;
  char  matchchar, gapchar;
  short numline, atseq,
		namewidth, numwidth,
		interline, spacer, seqwidth, tab;
  } prettyopts;


#define gPrettyInit(p) { \
  p.isactive=false;\
  p.baseonlynum=true;\
  p.numline= p.atseq= 0;\
  p.numright= p.numtop= true; \
  p.numleft= p.numbot= false;\
  p.nameright= true; \
  p.nameleft= p.nametop= false;\
  p.noleaves= p.domatch= p.degap= false;\
  p.matchchar='.';\
  p.gapchar='-';\
  p.namewidth=8;\
  p.numwidth=5;\
  p.interline=1;\
  p.spacer=10;\
  p.seqwidth=50;\
  p.tab=0; }

#ifdef UREADSEQ_G
prettyopts  gPretty;
#else
extern  prettyopts  gPretty;
#endif



typedef  long (*ReadWriteProc)( char* line, long maxline, short action);

extern short seqFileFormat(const char *filename, long *skiplines, short *error );
extern short seqFileFormatFp(FILE *fseq, long  *skiplines, short *error );
extern short seqFileFormatCall(ReadWriteProc reader, long *skiplines, short *error );

extern char *listSeqs(const char *filename, const long skiplines,
                       const short format, short *nseq, short *error );
											 
extern char *listSeqsCall(ReadWriteProc reader, const long skiplines,
                       const short format, short *nseq, short *error );


extern char *readSeq(const short whichEntry, const char *filename,
                      const long skiplines, const short format,
                      long *seqlen, short *nseq, short *error, char *seqid );

extern char *readSeqFp(const short whichEntry, FILE  *fp,
 									const long  skiplines, const short format,
       						long  *seqlen,  short *nseq, short *error, char *seqid );

extern char *readSeqCall(const short whichEntry, ReadWriteProc reader,
                      const long skiplines, const short format,
                      long *seqlen, short *nseq, short *error, char *seqid );


								
extern short writeSeq(FILE *outf, const char *seq, const long seqlen,
                       const short outform, const char *seqid );

extern short writeSeqCall(ReadWriteProc writer, const char *seq, const long seqlen,
                const short outform, const char *seqid);

extern unsigned long CRC32checksum(const char *seq, const long seqlen, unsigned long *checktotal);
extern unsigned long GCGchecksum(const char *seq, const long seqlen, unsigned long *checktotal);

#ifdef SMALLCHECKSUM
#define seqchecksum  GCGchecksum
#else
#define seqchecksum  CRC32checksum
#endif

extern short getseqtype(const char *seq, const long seqlen );
extern void GetSeqStats( const char* seq, const long seqlen, 
							short* seqtype, unsigned long* checksum, long* basecount);
extern char *compressSeq( const char gapc, const char *seq, const long seqlen, long *newlen);


#ifdef NCBI

extern char *listASNSeqs(const char *filename, const long skiplines,
                  const short format, short *nseq, short *error );

extern char *readASNSeq(const short whichEntry, const char *filename,
                const long skiplines, const short format,
                long *seqlen, short *nseq, short *error, char **seqid );
#endif


  /* patches for some missing string.h stuff */
/* extern int Strcasecmp(const char *a, const char *b); */
/* extern int Strncasecmp(const char *a, const char *b, long maxn); */

#ifdef __cplusplus
}
#endif

#endif /*UREADSEQ_H*/

