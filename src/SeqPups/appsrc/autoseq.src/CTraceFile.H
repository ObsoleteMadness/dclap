//	============================================================================
//	CTraceFile.H													80 columns
//	Reece Hart	(reece@ibc.wustl.edu)								tab=4 spaces
//	Washington University School of Medicine, St. Louis, Missouri
//	This source is hereby released to the public domain.  Bug reports, code
//	contributions, and suggestions are appreciated (to email address above).
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	CTraceFile represents chromatogram data from automated DNA sequencers.  It
//	currently supports reading and writing files in ABI and SCF formats.
//	Thanks to LaDeana Hillier for providing the code from which these file
//	formats were inferred.
//
//	NOTES
//	!	This source makes no attempt to accommodate machine independent I/O
//		because it was more important to tackle other issues first.  Nonetheless
//		I have tried to implement most I/O functions in a way that will make
//		migration to machine-independent I/O.
//	?	bottom flag is unimplemented.  the seqIO implementation of reading 
//		each trace in reverse for bottoms is, ahem, interesting.  I'd suggest 
//		using the template function Invert in RInlines to invert both sequences
//		and traces.
//	*	CTraceFile is generic in that it can represent several file formats.
//		Some fields of the abi format have been omitted and are copied into the
//		comments field during reading.  Before this class can correctly write
//		abi files, the field integrity must be maintained.  This may be done
//		by (1) making a derived class which declares these members and
//		appropriate read & write methods, or (2) make this class a superset of
//		all file formats.
//	*	SCF doesn't currently read or write comments
//
//	MODIFICATION HISTORY
//	93.11.11	Reece	First release
//	========================================|===================================

#ifndef _H_CTraceFile						// include this file only once
#define _H_CTraceFile

#include	<stddef.h>
//#include	"CTrace.H"
#include	"CTrace.C"  // dgg
#include	"CPeakList.H"
#include	"DNA.H"
#include	"FileFormat.H"
#include	"RInclude.H"

typedef		double xform_mtx[NUM_BASES][NUM_BASES]; // See Transform(), below

static const double MAX_SCF_TRACE_VALUE = 255.0;

class	CTraceFile
	{
	public:
	enum	strand_t
		{ top, bottom };

	enum	error_t
		{
		noError,							// no error occurred
		memError,							// memory couldn't be allocated
		ioError,							// file/strm access error
		fileExistsError	,					// file already exists
		fileDoesntExistError,				// no such file
		emptyError,							// I'm empty and can't do that
		fmtError,							// I don't understand data format
		unkFmtError,						// can't figure out the format
		fmtNotSuppError,					// format not yet supported
		peakPickError						// error in peakpicking
		};

	//
	// Instance variables
	//
	private:
	static vrsn	version;
	error_t	error;							// error flag

	char*	filename;						// filename
	format_t	nativeFormat;				// format used to read this data
	strand_t	whichStrand;				// which strand are we looking at?

	CTrace<trace_t>* trace[4];				// the traces
	size_t		numPoints;					// # of points in traces
	tracepos	leftCutoff;					// left cutoff
	tracepos	rightCutoff;				// right cutoff
	tracepos	primerPosition;				// start of primer
	CPeakList	peaks;						// assimilated (ACGT) list of peaks
	trace_t		min;						// min,
	trace_t		max;						// max,
	trace_t		mean;						// & mean for the 4 traces

	size_t		numBases;					// # of bases
	char*		sequence;					// the bases as called by mfr
	tracepos*	basePositions;				// base # <-> trace point # rel'n

	char*		comments;					// miscellaneous comments


	//
	// Methods
	//
	public:
	vrsn&		Version();					// return class version
	error_t		Error(void);
	void		Error(error_t new_error);	//   set/clear error

				CTraceFile(					// constructor
					const char* fn=NULL,
					format_t fmt=unknown);
				~CTraceFile(void);			// destructor

	error_t		Allocate(					// allocate traces,
					size_t points,			// base positions,
					size_t bases,			// and comment string
					size_t comment);
	void		Deallocate(void);			// deallocate space

	void		NumPoints(size_t np);		// set and...
	size_t		NumPoints(void);			//   get # of points in traces
	trace_t		Mean(void);					// get mean,
	trace_t		Min(void);					//   min, &
	trace_t		Max(void);					//   max values over all 4 traces
	void		CalculateStats(void);		// calc. min/max/mean for all traces

	CPeakList&	Peaks(void);				// get the list of peaks
	size_t		NumPeaks(void);				// # of peaks picked 
	void		NumBases(size_t nb);		// set and...
	size_t		NumBases(void);				//   get the number of bases
	void		Sequence(char* seq);		// set and...
	char*		Sequence(void);				//   get the sequence
	void		Comments(char* newComment);	// set and...
	char*		Comments(void);				//   get the comment

	CTrace<trace_t>*
				operator[](enum_t whichTrace);	// trace selector

	tracepos	LeftCutoff(void);			// get and...
	void		LeftCutoff(tracepos t);		//   set left cutoff
	tracepos	RightCutoff(void);			// get and...
	void		RightCutoff(tracepos t);	//   set right cutoff

	tracepos	PrimerPosition(void);		// return the position of the
											// primer (if stored in file)
	tracepos*	BasePositions(void);		// return base positions array

	error_t		Transform(xform_mtx& matrix);
				// Applies a 4x4 transformation/orthogonalization matrix to the
				// 4 traces, producing a new set of traces which replaces the
				// existing set.
				// Transformation matrics are expected to be 4x4 matrix of the
				// form:
				//     M =         [,A]    [,C]    [,G]    [,T]
				//         [A,]    mAA     mAC     mAG     mAT
				//         [C,]    mCA     mCC     mCG     mCT
				//         [G,]    mGA     mGC     mGG     mGT
				//         [T,]    mTA     mTC     mTG     mTT
				// The general equation for the resulting traces is:
				// R = M O  <==>  R(T,i) =      sum     [ mTS x O(S,i) ]
				//                          S in {ACGT}
				// where R is the resulting vector of 4 traces
				//       O is the original vector of 4 traces
				//       M is the 4x4 ({ACGT}x{ACGT}) matrix whose elements
				//         m(i,j) are the cross-term contributions of channel j
				//         to channel i
				//       S & T are trace identifiers (Source & Target)
				//         in {A,C,G,T}
				//       i loops over the indices of the trace


	error_t		PickPeaks(
					double PeakMeanCoefficient = 1.5,
					double ZeroThreshold = 0.0);
				// Calls PickPeaks on each trace in the tracefile, then
				// assimilates the peaks into a list of peaks for the entire
				// set sorted by index in trace.  No pruning in performed.

	error_t		AssimilatePeaks(void);
				// Performs a merge sort of the list of peaks stored in each
				// trace's peak list and puts the result in this tracefile's
				// own (assimilated) peak list.

	error_t		PrunePeaks(
					tracepos minSeparation,
					ostream* os=NULL);
				// Prunes peaks by doing a pairwise comparison of adjacent peaks
				// and discarding the less-probably peak of each pair which is
				// separated by less than minSeparation.  If os is specified,
				// a list of pairwise comparisons which resulted in the
				// removal of a peak is output.

	//
	// I/O methods
	//
	error_t		Read(
					const char* filename,
					format_t fmt=unknown);
				// Top-level read routine.  Attempts to read the named file
				// in the specified format, or in the format guessed by
				// FileFormat() (see FileFormat.H).
	error_t		Write(
					const char* filename,
					format_t fmt=unknown);
				// Top-level write routine.  Attempts to write the named file
				// in the specified format, or in the native format if
				// unspecified.


	private:
				// The following are private I/O methods.  Users should not
				// need access to any of these.
	error_t		Read(						// read from file, format known
					FILE* fp,
					format_t fmt);
	error_t		Write(						// write to file, format known
					FILE* fp,
					format_t fmt);
	error_t		ReadSCF(FILE* fp);			// Read as SCF file
	error_t		WriteSCF(FILE* fp);			// Write as SCF file
	error_t		ReadABI(FILE* fp, short whichSet=0);	// Read as ABI file
	error_t		WriteABI(FILE* fp);			// Write as ABI file
	error_t		ReadALF(FILE* fp);			// Read as ALF file
	error_t		WriteALF(FILE* fp);			// Write as ALF file

	public:
	friend ostream& operator<<(ostream& os, CTraceFile& ctf);
				// Places an easy-to-read summary of the tracefile contents
				// on the specified output stream.
	};


//
//	INLINE FUNCTION DEFINITIONS
//	===========================
//

inline
vrsn&
CTraceFile::Version()
	{
	return version;
	}

inline
void
CTraceFile::Error(error_t new_error)
	{
	error = new_error;
	}

inline
CTraceFile::error_t
CTraceFile::Error(void)
	{
	return error;
	}

inline
void
CTraceFile::NumPoints(size_t np)
	{
	numPoints = np;
	}
inline
size_t
CTraceFile::NumPoints(void)
	{
	return numPoints;
	}
inline
trace_t
CTraceFile::Mean(void)
	{
	return mean;
	}
inline
trace_t
CTraceFile::Min(void)
	{
	return max;
	}

inline
trace_t
CTraceFile::Max(void)
	{
	return max;
	}

inline
tracepos
CTraceFile::LeftCutoff(void)
	{
	return leftCutoff;
	}
inline
void
CTraceFile::LeftCutoff(tracepos t)
	{
	leftCutoff = t;
	for(uint i=A;i<NUM_BASES;i++) trace[i]->LeftCutoff(t);
	CalculateStats();
	}

inline
tracepos
CTraceFile::RightCutoff(void)
	{
	return rightCutoff;
	}

inline
void
CTraceFile::RightCutoff(tracepos t)
	{
	rightCutoff = t;
	for(uint i=A;i<NUM_BASES;i++) trace[i]->RightCutoff(t);
	CalculateStats();
	}

inline
tracepos
CTraceFile::PrimerPosition(void)
	{
	return primerPosition;
	}

inline
CPeakList&
CTraceFile::Peaks(void)
	{
	return peaks;
	}

inline
size_t
CTraceFile::NumPeaks(void)
	{
	return peaks.Size();
	}

inline
CTrace<trace_t>*
CTraceFile::operator[](enum_t whichTrace)
	{
	return trace[whichTrace];
	}

inline
void
CTraceFile::NumBases(size_t nb)
	{
	numBases = nb;
	}

inline
size_t
CTraceFile::NumBases(void)
	{
	return numBases;
	}

inline
void
CTraceFile::Sequence(char* seq)
	{
	sequence = seq;
	}
	
inline
char*
CTraceFile::Sequence(void)
	{
	return sequence;
	}

inline
void
CTraceFile::Comments(char* newComment)
	{
	comments = newComment;
	}

inline
char*
CTraceFile::Comments(void)
	{
	return comments;
	}

inline
tracepos*
CTraceFile::BasePositions(void)
	{
	return basePositions;
	}

#endif
