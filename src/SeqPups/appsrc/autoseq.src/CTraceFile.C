//	============================================================================
//	CTraceFile.C													80 columns
//	Reece Hart	(reece@ibc.wustl.edu)								tab=4 spaces
//	Washington University School of Medicine, St. Louis, Missouri
//	This source is hereby released to the public domain.  Bug reports, code
//	contributions, and suggestions are appreciated (to email address above).
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	Please see CTraceFile.H for a description of the CTraceFile class.
//
//	NOTES
//	*	I make frequent use of the following construct:
//			return (error = <error_t>);
//		This causes the error flag of this instance of CTraceFile to be set
//		to the appropriate error, and then returns this value.
//		although CPeakList contains a lot that's irrelevant to CTraceFile.
//	*	WriteSCF should scale the traces for writing to [0,255] range without
//		affecting the trace permanently.
//	========================================|===================================
#include	"CTraceFile.H"
#include	<iostream.h>
#include	<fstream.h>
#include	<string.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<stdlib.h>
#include	<time.h>
#include	"CPeakList.H"
#include	"RInclude.H"
#include	"RInlines.H"
#include	"DNA.H"

vrsn CTraceFile::version = "94.05.03";

const bool DEBUG = !TRUE;

//	============================================================================
//	CTraceFile (constructor)
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	This constructor takes up to two arguments which specify filename and format
//	for initialization.  If neither is provided, the instance is initialized
//	with empty values.  If a filename is provided, then an attempt is made to
//	read the file in the format specified by fmt, or in a format inferred by
//	Read if fmt is not specified.  ie. The following are valid:
//		CTraceFile	myTrace();			// empty instance
//		CTraceFile	myTrace("myFile");	// attempt read myFile, guessing format
//		CTraceFile	myTrace("myFile",SCF);	// attempt to read myFile in SCF fmt
//	========================================|===================================
CTraceFile::CTraceFile(const char* fn, format_t fmt):
	filename(NULL),
	error(noError),
	nativeFormat(unknown),
	whichStrand(top),
	numPoints(0),
	min(0),
	max(0),
	mean(0),
	leftCutoff(0),
	rightCutoff(0),
	numBases(0),
	sequence(NULL),
	basePositions(NULL),
	comments(NULL)
	{
	for (uint traceIndex=0; traceIndex<NUM_BASES; traceIndex++)
		trace[traceIndex]=NULL;

	if (fn)
		error = Read(fn,fmt);
	}

//	============================================================================
//	~CTraceFile (destructor)
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Call Deallocate to clean up our dynamically-allocated space.
//	Note that the lifetime of peaks (a CSequence) is the same as the lifetime
//	of CTraceFile because it is automatically allocated when an instance of
//	CTraceFile is allocated.  Therefore, it will be disposed of automatically
//	its destructor will free all space allocated for the peak records.
//	========================================|===================================
CTraceFile::~CTraceFile(void)
	{
	Deallocate();
	}

//	============================================================================
//	Allocate
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Allocate space for the traces, bases, base_positions, and comment.  This is
//	all-or-none: on success, we've allocated all (and returned noError); on
//	failure we've allocated nothing (and returned memError);
//	========================================|===================================
CTraceFile::error_t
CTraceFile::Allocate(size_t pt_sz, size_t base_sz, size_t comment_sz)
	{
	// Allocate space for trace[]
	for (uint traceIndex=0; traceIndex<NUM_BASES; traceIndex++)
		if ( NULL == (trace[traceIndex]=new CTrace<trace_t>(pt_sz)) )
			// allocate failed
			{
			Deallocate();
			return (error = memError);
			}
		
	// Allocate space for sequence and base_positions
	if (	!(sequence = new char[base_sz]) ||
			!(basePositions = new tracepos[base_sz]) ||
			!(comments = new char[comment_sz+1]) )
		{
		Deallocate();
		return (error = memError);
		}
	
	numPoints = pt_sz;
	numBases = base_sz;
	rightCutoff = pt_sz-1;

	return (error = noError);
	}

//	============================================================================
//	Deallocate
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Make Mom proud and put away all of our toys... ensure that all of our space
//	is neatly reclaimed.  This method is provided separately so that in the case
//	of errors, we can deallocate stuff in one call.  Note that this doesn't
//	deallocate peaks; the reasoning is twofold:
//	1) if we're ready to calculate the peaks, then we've already got our data
//		allocated in its entirety. Thus, there's no reason to call this except
//		to cause problems.  You wouldn't want to do that.
//	2) It's probably not kosher to deallocate peaks... it's been allocated
//		automatically and doing so might wreak havoc.
//	========================================|===================================
void
CTraceFile::Deallocate(void)
	{
	delete filename;
	for (uint traceIndex=0; traceIndex<NUM_BASES; traceIndex++)
		delete trace[traceIndex];
	delete sequence;
	delete basePositions;
	delete comments;
	}

//	============================================================================
//	CalculateStats
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Calculates the stats for individual traces and then calculates the overall
//	stats for the combination of the four.  Theoretically, sum may overflow,
//	although it's unlikely... if we have that many values, something's amiss.
//	========================================|===================================
void
CTraceFile::CalculateStats(void)
	{
	double		sum = 0;
	
	for (uint traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
		{
		trace[traceIndex]->CalculateStats();

		if ((traceIndex == A) ||				// this is first pass ||
			(trace[traceIndex]->Max() > max))	//    Max() for this trace > max
			max = trace[traceIndex]->Max();		// set the new max

		if ((traceIndex == A) ||				// ditto for min values
			(trace[traceIndex]->Min() < min))
			min = trace[traceIndex]->Min();

		sum += trace[traceIndex]->Mean();		// cumulative value for means
		}

	mean = (trace_t)sum/NUM_BASES;			// mean over 4 traces
	}

//	============================================================================
//	Transform
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Applies a 4x4 transformation/orthogonalization matrix to the 4 traces,
//	producing a new set of traces which replaces the existing set.
//	Transformation matrics are expected to be 4x4 matrix of the form:
//		M =			[,A]	[,C]	[,G]	[,T]
//			[A,]	mAA		mAC		mAG		mAT
//			[C,]	mCA		mCC		mCG		mCT
//			[G,]	mGA		mGC		mGG		mGT
//			[T,]	mTA		mTC		mTG		mTT
//	The general equation for the resulting traces is:
//		R = M O  <==>  R(T,i) =      sum     [ mTS x O(S,i) ]
//								S in {ACGT}
//	where R is the resulting vector of 4 traces
//	      O is the original vector of 4 traces
//	      M is the 4x4 ({ACGT}x{ACGT}) matrix whose elements m(i,j) are
//	        the cross-term contributions of channel j to channel i
//	      S & T are trace identifiers (Source & Target) in {A,C,G,T}
//	      i loops over the indices of the trace
//
//	!	The existing traces are discarded and replaced by the transformed data.
//	========================================|===================================
CTraceFile::error_t
CTraceFile::Transform(xform_mtx& matrix)
	{
	CTrace<trace_t>*	results[NUM_BASES];

	// initialize the results array
	for (uint traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
		if ( NULL == (results[traceIndex]=new CTrace<trace_t>(numPoints)) )
			// allocate failed... deallocate any already new'd
			{
			do
				delete results[--traceIndex];
			while(traceIndex);

			return memError;
			}

	// do the transformations
	uint targetTrace;
	for ( targetTrace = A;targetTrace<NUM_BASES;targetTrace++)
		// targetTrace is the index of the resulting trace
		for (tracepos index=0; index<numPoints; index++)
			// index is the sample point being transformed
			{
			double	sum = 0;
			for (uint sourceTrace=A; sourceTrace<NUM_BASES; sourceTrace++)
				// sourceTrace is the index of traces contributing to the result
				sum +=	 matrix[targetTrace][sourceTrace]
										 * (*trace[sourceTrace])[index];

			(*results[targetTrace])[index] = (trace_t)sum;
			}
	
	// replace the original traces with the transformed ones.
	for (targetTrace = A; targetTrace<NUM_BASES;targetTrace++)
		{
		delete trace[targetTrace];
		trace[targetTrace] = results[targetTrace];
		trace[targetTrace]->LeftCutoff(leftCutoff);
		trace[targetTrace]->RightCutoff(rightCutoff);
		}

	// Calculate the stats for each trace; if mean is <0, then scale by -1
	// orthogonalization will not be affected
//	for (targetTrace = A; targetTrace<NUM_BASES;targetTrace++)
//		{
//		trace[targetTrace]->CalculateStats();
//		if (trace[targetTrace]->Mean()<0)
//			trace[targetTrace]->Scale(-1);
//		}

	CalculateStats();
	
	return noError;
	}

//	============================================================================
//	PickPeaks
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Calls CTrace<>::PickPeaks for each trace and assimilates the results into a
//	single list in order of occurrence (a merge sort of the peaks).
//	!	Because this is still being designed, error handling is lax, although
//		the skeleton for it exists.
//	========================================|===================================
CTraceFile::error_t
CTraceFile::PickPeaks(double PeakMeanCoefficient, double ZeroThreshold)
	{
	uint		traceIndex=0;
	//
	// Calculates stats to ensure that each trace has correct min, max, &
	// mean values.
	//
	CalculateStats();

	//
	// Generate a list of peaks for each trace
	//
	for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
		{
		trace_t MinPeakHeight = (trace_t)(PeakMeanCoefficient
											 * trace[traceIndex]->Mean());
		if (trace[traceIndex]->PickPeaks(MinPeakHeight,ZeroThreshold)==noError)
			trace[traceIndex]->Peaks().Analyze(*trace[traceIndex]);
		else
			return peakPickError;
		}

	return AssimilatePeaks();
	}

//	============================================================================
//	AssimilatePeaks
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Assumes that each trace in the tracefile has a list of peaks and collates
//	them into our own list for the tracefile.
//	========================================|===================================
CTraceFile::error_t
CTraceFile::AssimilatePeaks()
	{
	uint		traceIndex=0;
	//
	// Assimilate the peaks into one CSequence of PeakRec's
	// by popping the 'least' peak from the front of the appropriate
	// list.  When we've done sum total pops, we're done. (This is easier
	// and faster than checking if all lists are empty at every iteration.)
	//
	ulong		sum=0;
	uint		next_least[NUM_BASES]={0};		// next least elem in each trace

	for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
		sum += trace[traceIndex]->Peaks().Size();

	while (sum)
		{
		uint	 least_trace = A;
		tracepos least_pos = numPoints;		// just want something that's
												// guaranteed to be larger than
												// our first real hit

		// Search for the trace which has the least element (ie. the trace
		// with a peak at an index less than that for peaks of any other trace)
		for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
			{
			CPeakList&	ind_peaks = trace[traceIndex]->Peaks();
			if ( (next_least[traceIndex] < ind_peaks.Size()) &&
				 (ind_peaks[next_least[traceIndex]].center < least_pos) )
				{
				least_trace = traceIndex;
				least_pos = ind_peaks[next_least[traceIndex]].center;
				}
			}

		// So now the least peak has been identified.
		// We'll fill in the whichTrace field and append it to the assimilated
		// list of peaks
		PeakRec pr = trace[least_trace]->Peaks()[next_least[least_trace]];
		pr.whichTrace = (base_t)least_trace;
		peaks.Append(pr);

		// Set the next_least index to the successor of this victim and
		// decrement the sum counter... 1 down and sum [sic] more to go.  :-|
		next_least[least_trace]++;
		sum--;
		}
	return noError;
	}

//	============================================================================
//	PrunePeaks
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Simply step through the sequence of peaks and in the case where adjacent
//	peaks are less than minSeparation apart, remove the peak with the lesser
//	probability.
//	A list of the pairwise peak contentions and their resolution will be output
//	if an output stream is provided.
//
//	ALGORITHM:
//	Given a set of peak records indexed 0..s  with fields:
//		center - position in trace of the center of the peak
//		PH - probability of the peak
//	and a minimum separation, minSep, prune the list by removing the less
//	probable peak when adjacent peak centers are within minSep.
//	let peakA be the index of the first peak in the comparison
//	let peakA=0
//	while peakA<=s-2 do
//		let peakB = peakA + 1
//		if peakB.center - peakA.center <= minSep then
//			if peakA.PH < peakB.PH then
//				remove peakA
//			else
//				remove peakB
//			fi
//		else
//			increment peakA
//		fi
//	elihw
//
//	!	This is intended to be only a first cut at a pruning algorithm.  It has
//		many problems, including the possibility that a series of peaks all
//		separated by less than minSeparation between adjacent members will
//		result in only 1 of the members being selected, when perhaps more than
//		one peak should result from the collection.
//	========================================|===================================
CTraceFile::error_t
CTraceFile::PrunePeaks(
	tracepos	minSeparation,
	ostream*	os)
	{
	ulong peakNo=0;

	while(peakNo<=peaks.Size()-2)
		if (peaks[peakNo+1].center-peaks[peakNo].center < minSeparation)
			{
			// victim is the peak which is less probable [ie. lesser P(H|D)]
			ulong victim = (peaks[peakNo+1].PHD < peaks[peakNo].PHD ?
							peakNo+1:
							peakNo);
			base_t victimTrace = peaks[victim].whichTrace;

			if (os)
				{
				*os << setfill('-') << setw(80) << "" << setfill(' ') << endl;
				*os << Tag(victim==peakNo)   << peaks[peakNo];
				*os << Tag(victim==peakNo+1) << peaks[peakNo+1];
				}

			// remove the peak record from the trace's peak list
			(*trace[victimTrace]).Peaks().RemovePeak(peaks[victim].center);

			// and remove the peak from the tracefile's peak list
			peaks.Remove(victim);
			}
		else
			peakNo++;
		// end while

	if (os)
		*os << setfill('-') << setw(80) << "" << setfill(' ') << endl;

	return noError;
	}

ostream&
operator<<(ostream& os, CTraceFile& ctf)
	{
	tracepos length=ctf.rightCutoff - ctf.leftCutoff + 1;
  		os	<< "filename:\t" 	<< ctf.filename				<< endl
		<< "native format:\t" << FileFormatAbbr(ctf.nativeFormat)
			<< " [" << FileFormatDesc(ctf.nativeFormat) << "]" << endl
		<< "strand:\t\t"	<< (short)ctf.whichStrand			<< endl
		<< "total size:\t"	<< ctf.numPoints			<< endl
		<< "window:\t\t"
			<< "[" << ctf.leftCutoff << "," << ctf.rightCutoff << "]"
			<< "  (" << length << " point" << Plural(length) << ")" << endl
		<< "min/max/mean:\t" << ctf.min << "/"
							<< ctf.max << "/"
							<< ctf.mean << "  (within window)" << endl
		<< "sequence len:\t"<< ctf.numBases			<< endl
		<< "comments" << endl << "--------" << endl << ctf.comments << endl
		<< "peaks picked:\t"<< ctf.peaks.Size()			<< endl;

	if (ctf.peaks.Size() != 0)
		{
		os << setw(3) << "#" << "  " << PeakRecHeader;
		for(uint i = 0; i<ctf.peaks.Size(); i++)
			os << setw(3) << i+1 << "  " << ctf.peaks[i];
		}

	return os;
	}

//	============================================================================
//	###        # #######
//	 #        #  #     #
//	 #       #   #     #
//	 #      #    #     #
//	 #     #     #     #
//	 #    #      #     #
//	###  #       #######
//	
//	General I/O Methods
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	public:
//	Read(const char*, format) - Reads from named file in specified format, or
//		inferred format if not specified.
//	Write(const char*, format) - Writes to named file in specified format, or
//		native format if not specified.
//
//	private:
//	Read(FILE*, format) - Reads from FILE* (opened with at least "rb"
//		permissions in specified format (mandatory).  This just dispatches to
//		the appropriate file format reader (ie. ReadSCF).
//	Write(FILE*, format) - Writes to FILE* (opened with at least "wb"
//		permissions) in specified format (mandatory).  This just dispatches to
//		the appropriate file format writer (ie. WriteSCF).
//
//	NOTES
//	*	File I/O must be handled through the const char* reader and writer.
//		All other methods are declared private.
//	*	The private methods do not set the class's error flag; this allows the
//		calling function (ie. the public methods) to handle the error as
//		appropriate, or to set the error flag if desired.  (ie. we may wish to
//		retry with a different format.
//	?	Currently, if an error occurs during reading, it's possible to return
//		(with error) an incompletely-read trace.  I think that this is not
//		desired, but I'm still weighing the merits.
//	========================================|===================================

//	============================================================================
//	Read(const char*, format_t)
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Takes a filename and optional format.  If the format is not specified, I
//	attempt to determine it by FileFormat (see FileFormat.H).  If it still can't
//	be determined, we're sunk; if it can be determined, we attempt to read it.
//
//	NOTES
//	?	should the trace be complete deallocated if it fails?
//	========================================|===================================
CTraceFile::error_t
CTraceFile::Read(const char* fn, format_t fmt)
	{
	error_t		err;
	FILE*		fp;

	if (unknown == fmt)						// user didn't tell us format... try
		{									//   to determine it for ourselves
		fmt = FileFormat(fn);
		if (unknown == fmt)					// drat! still unknown
			return (error = unkFmtError);
		}

	if (fp = fopen(fn,"rb"))
		{
		// file opened successfully
		err = Read(fp,fmt);
		fclose(fp);
		}
	else
		{
		err = fileDoesntExistError;
		}

	if (!err)
		{
		filename = new char[strlen(fn)];
		strcpy(filename,fn);
		nativeFormat = fmt;

		CalculateStats();
		}
	
	return (error = err);
	}

//	============================================================================
//	Write(const char*, format_t)
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Takes a filename and optional format.  If the format is not specified, the
//	native format is assumed.  If we're writing in format other than the one
//	used to read the tracefile, the comment field is duplicated and a temporary
//	version is constructed which annotates the conversion process.
//
//	NOTES
//	!	I don't check to see if the file exists.  I should do this...
//	========================================|===================================
CTraceFile::error_t
CTraceFile::Write(const char* fn, format_t fmt)
	{
	error_t		err;
	FILE*		fp;
	char*		tempComment=NULL;			// temporary comment
	char*		origComment=comments;		// saved pointer to original comment

	if (DEBUG)	cerr << "Write(char*,format_t): Entering" << endl;

	if ( 0 == numPoints )
		return (error = emptyError);
	
	if (fmt == unknown)						// user didn't tell us format...
		fmt = nativeFormat;					//   assume nativeFormat
	else
		if (fmt != nativeFormat)
			// user specified a format different from native... make a comment
			// that this was translated by CTraceFile.
			{
			char buf[1000] = {0};

			strcat(buf,"conv = by CTraceFile ");
			strcat(buf,version);
			strcat(buf," from ");
			strcat(buf,FileFormatDesc(nativeFormat));
			strcat(buf," (");
			strcat(buf,FileFormatAbbr(nativeFormat));
			strcat(buf,")");
			strcat(buf," to ");
			strcat(buf,FileFormatDesc(fmt));
			strcat(buf," (");
			strcat(buf,FileFormatAbbr(fmt));
			strcat(buf,")");
			strcat(buf," on ");
			time_t tm = time(NULL);
//			char* tmsp = ctime(&tm);
//			strcat(buf,tmsp);
			struct tm* tp=localtime(&tm);
			strftime(buf+strlen(buf),1000-strlen(buf),"%Y-%b-%d %H:%M%Z\n",tp);

//			This is the original conversion comment; I've opted for a style
//			that more closely matches makeSCF's for consistency.
//			strcat(buf,tmsp);
//			strcpy(buf+strlen(buf)-1,"- tracefile converted from ");
//			strcat(buf,FileFormatDesc(nativeFormat));
//			strcat(buf," to ");
//			strcat(buf,FileFormatDesc(fmt));
//			strcat(buf," by CTraceFile.\n");
						
			// make space for the temporary comment
			tempComment = new char[strlen(comments)+strlen(buf)+1];
			if (tempComment == NULL)
				return memError;
			strcpy(tempComment,comments);	// copy old comment to new comment
			strcat(tempComment,buf);		// add the conversion comment

			comments = tempComment;			// we'll write, then swap them back
			}

	if (DEBUG)	cerr << "Write(char*,format_t): Writing" << endl;

	if ((fp = fopen(fn,"wb")) != NULL)
		{
		// file opened successfully
		if ((err = Write(fp,fmt)) != noError)
			{
			fclose(fp);
			remove(fn);						// half-complete files are useless
			}
		else
			fclose(fp);						// write succeeded
		}
	else
		err = ioError;

	if (DEBUG)	cerr << "Write(char*,format_t): Exiting" << endl;

	if (tempComment != NULL)				// this write involved a conversion
		{									// we need to restore the original
		comments=origComment;				// comment and kill the space for
		delete tempComment;					// the temporary
		}

	return (error = err);
	}

//	============================================================================
//	Read(FILE*, format_t) [private]
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	This is the dispatcher to the appropriate format reader.  It is called by
//	Read(const char*, format_t), but not called directly.
//	========================================|===================================
CTraceFile::error_t
CTraceFile::Read(FILE* fp, format_t fmt)
	{
	if (DEBUG)	cerr << "Read(FILE*,format_t): Entering; fmt = "<< (short)fmt<< endl;

	if (NULL == fp)
		return ioError; 

	// else...
	error_t	err = noError;
	switch (fmt)
		{
		case unknown:
			err = unkFmtError;
			break;
		case SCF:
			err = ReadSCF(fp);
			break;
		case ABI0:
			err = ReadABI(fp,0);
			break;
		case ABI1:
			err = ReadABI(fp,1);
			break;
		case ABI:
			err = ReadABI(fp,2);
			break;
		case ALF:
			err = ReadALF(fp);
			break;
		default:
			err = fmtNotSuppError;
		}

	if (DEBUG)	cerr << "Read(FILE*,format_t): Exiting" << endl;
	return err;
	}

//	============================================================================
//	Write(FILE*, format_t) [private]
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	This is the dispatcher to the appropriate format writer.  It will most
//	likely be called by Write(const char*, format_t), but not called directly.
//	========================================|===================================
CTraceFile::error_t
CTraceFile::Write(FILE* fp, format_t fmt)
	{
	error_t	err = noError;

	if (NULL == fp)
		return ioError;

	// else...
	if (DEBUG)	cerr << "Write(FILE*,format_t): Entering" << endl;

	switch (fmt)
		{
		case unknown:
			err = unkFmtError;
			break;
		case SCF:
			err = WriteSCF(fp);
			break;
		case ABI:	
			err = WriteABI(fp);
			break;
		case ALF:
			err = WriteALF(fp);
			break;
		default:
			err = fmtNotSuppError;
		}

	if (DEBUG)	cerr << "Write(FILE*,format_t): Exiting" << endl;
	return err;
	}


//	============================================================================
//	 #####   #####  #######           ###         # #######
//	#     # #     # #                  #         #  #     #
//	#       #       #                  #        #   #     #
//	 #####  #       #####              #       #    #     #
//	      # #       #                  #      #     #     #
//	#     # #     # #                  #     #      #     #
//	 #####   #####  #                 ###   #       #######
//	
//	SCF (Standard Chromatogram Format) I/O Methods
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	File format inferred from ted source (see CTraceFile.H).
//
//	SCF_File = 
//		offset			description
//		---------------	--------------------------------
//		0				<SCF_Header>
//		trace_offset	<SCF_Sample> x num_samples
//		sequence_offset	<SCF_Bases> x num_bases
//		comment_offset	comment of length comment_length
//	========================================|===================================
const long SCF_MAGIC_COOKIE = ((('.'<<8)+'s'<<8)+'c'<<8)+'f'; 

struct	SCF_Header
	{
	long	magic_cookie;					// = SCF_MAGIC_COOKIE
	long	num_points;						// number of trace points (per base)
	long	trace_offset;					// position of trace data in file
	long	num_bases;						// number of bases in sequence
	long	left_cutoff;					// unreadable < left cutoff
	long	right_cutoff;					// unreadable > right cutoff
	long	sequence_offset;				// position of sequence data in file
	long	comment_length;					// strlen of comment
	long	comment_offset;					// position of comment in file
	long	spare[23];

	friend size_t fread(FILE* fp, SCF_Header& header)  
		{ return ::fread(&header,sizeof(SCF_Header),1,fp); }
	friend size_t fwrite(FILE* fp, const SCF_Header& header)  
		{ return ::fwrite(&header,sizeof(SCF_Header),1,fp); }
	};

struct	SCF_Sample							// note that the values for each
	{										// trace are interleaved. ie
	byte	sample_A;						// A1-C1-G1-T1-A2-C2-G2-...-Gn-Tn
	byte	sample_C;						// where Ni = value of point i of
	byte	sample_G;						// trace N.
	byte	sample_T;

	friend size_t	fread(FILE* fp, SCF_Sample& sample)  
		{ return ::fread(&sample,sizeof(SCF_Sample),1,fp); }
	friend size_t	fwrite(FILE* fp, const SCF_Sample& sample)
		{ return ::fwrite(&sample,sizeof(SCF_Sample),1,fp); }
	};

struct	SCF_Base
	{
	long	trace_index;					// location of base as called
	byte	prob_A;							// probability that base = A
	byte	prob_C;							//						   C
	byte	prob_G;							//						   G
	byte	prob_T;							//						   T
	char	base;							// base that was chosen
	byte	_unused1;						// unused?
	byte	_unused2;						// unused?
	byte	_unused3;						// unused?

	friend size_t fread(FILE* fp, SCF_Base& base)  
		{ return ::fread(&base,sizeof(SCF_Base),1,fp); }
	friend size_t fwrite(FILE* fp, const SCF_Base& base) 
		{ return ::fwrite(&base,sizeof(SCF_Base),1,fp); }
	};

//	============================================================================
//	ReadSCF [private]
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Read a SCF file; called exclusively by Read(FILE*,format_t).
//	========================================|===================================
CTraceFile::error_t
CTraceFile::ReadSCF(FILE* fp)
	{
	uint		position;

	if (ferror(fp))
		return ioError;

	//
	// Read header
	//
	SCF_Header	header;

	rewind(fp);								// take it from the top...
	fread(fp,header);						// Read the header
	if (ferror(fp))							// check for error
		return ioError;

	nativeFormat = SCF;						// set our instance's data members
	whichStrand = top;


	//
	// Allocate space for traces, sequence, etc.
	//   this is all-or-none: we either want all our required space or none at
	//   all.
	error_t err = Allocate((size_t)header.num_points,\
						   (size_t)header.num_bases,\
						   (size_t)header.comment_length);
	if (noError != err)
		return err;

	//
	// read the traces
	//
	fseek(fp,header.trace_offset,SEEK_SET);
	for (position=0; position<numPoints; position++)
		{
		SCF_Sample	sample;
		fread(fp,sample);
		(*trace[A])[position]=sample.sample_A;
		(*trace[C])[position]=sample.sample_C;
		(*trace[G])[position]=sample.sample_G;
		(*trace[T])[position]=sample.sample_T;
		}
	if (ferror(fp))
		return ioError;

	//
	// read the sequence & base positions
	//
	fseek(fp,header.sequence_offset,SEEK_SET);
	for (uint traceIndex=0; traceIndex<numBases; traceIndex++)
		{
		SCF_Base	base;
		fread(fp,base);
		sequence[traceIndex]=base.base;
		basePositions[traceIndex]=(tracepos)base.trace_index;
		}
	if (ferror(fp))
		return ioError;

	//
	// read the comment
	//
	fseek(fp,header.comment_offset,SEEK_SET);
	fread(comments,(size_t)header.comment_length,1,fp);
	if (ferror(fp))
		return ioError;

	return noError;
	}

//	============================================================================
//	WriteSCF [private]
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Write a SCF file; called exclusively by Write(FILE*,format_t).
//	========================================|===================================
CTraceFile::error_t
CTraceFile::WriteSCF(FILE* fp)
	{
	SCF_Header	header;
	uint		position;

	if (DEBUG)	cerr << "WriteSCF: Entering" << endl;

	//
	// prepare the header and calculate file offsets for trace information
	//
	header.magic_cookie		= SCF_MAGIC_COOKIE;
	header.num_points		= numPoints;
	header.num_bases		= numBases;
	header.left_cutoff		= leftCutoff;
	header.right_cutoff		= rightCutoff;
	header.comment_length	= strlen(comments);
	header.trace_offset		= sizeof(SCF_Header);
	header.sequence_offset	= header.trace_offset+sizeof(SCF_Sample)*numPoints;
	header.comment_offset	= header.sequence_offset+sizeof(SCF_Base)*numBases;

	if (DEBUG)	cerr << "WriteSCF: header" << endl;

	//
	// write the header
	//
	rewind(fp);								// take it from the top...
	fwrite(fp, header);						// write the header
	if (ferror(fp))							// check for error
		return ioError;

	if (DEBUG)	cerr << "WriteSCF: traces" << endl;

	//
	// write the traces
	//
	fseek(fp,header.trace_offset,SEEK_SET);	// move to traces
	for (position=0; position<numPoints; position++)
		{
		SCF_Sample	sample;
		sample.sample_A=(byte)(*trace[A])[position];
		sample.sample_C=(byte)(*trace[C])[position];
		sample.sample_G=(byte)(*trace[G])[position];
		sample.sample_T=(byte)(*trace[T])[position];
		fwrite(fp,sample);
		}
	if (ferror(fp))
		return ioError;

	if (DEBUG)	cerr << "WriteSCF: sequence" << endl;

	//
	// write the sequence & base positions
	//
	fseek(fp,header.sequence_offset,SEEK_SET);	// move to traces
	for (position=0; position<numBases; position++)
		{
		SCF_Base	base;
		base.base=sequence[position];
		base.trace_index=basePositions[position];
		fwrite(fp,base);
		}
	if (ferror(fp))
		return ioError;

	if (DEBUG)	cerr << "WriteSCF: comment" << endl;

	//
	// write the comment
	//
	fseek(fp,header.comment_offset,SEEK_SET);
	fwrite(comments,(size_t)header.comment_length,1,fp);
	if (ferror(fp))
		return ioError;

	if (DEBUG)	cerr << "WriteSCF: Exiting" << endl;

	return noError;
	}

//	============================================================================
//	   #    ######    ###             ###         # #######
//	  # #   #     #    #               #         #  #     #
//	 #   #  #     #    #               #        #   #     #
//	#     # ######     #               #       #    #     #
//	####### #     #    #               #      #     #     #
//	#     # #     #    #               #     #      #     #
//	#     # ######    ###             ###   #       #######
//
//	ABI format I/O methods
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	These routines are copied almost verbatim from seqIOABI.[hc] written by
//	LaDeana Hillier, although I've made extensive modifications to certain
//	sections.
//	========================================|===================================
typedef ushort int2;						// Two byte short assumption
typedef ulong  int4;						// Four byte long assumption

const long DATAtag			= ((long) ((((('D'<<8)+'A')<<8)+'T')<<8)+'A');
const long BASEtag			= ((long) ((((('P'<<8)+'B')<<8)+'A')<<8)+'S');
const long BASEPOStag		= ((long) ((((('P'<<8)+'L')<<8)+'O')<<8)+'C');
const long SPACINGtag		= ((long) ((((('S'<<8)+'P')<<8)+'A')<<8)+'C');
const long SIGNALtag		= ((long) ((((('S'<<8)+'/')<<8)+'N')<<8)+'%');
const long FWO_tag			= ((long) ((((('F'<<8)+'W')<<8)+'O')<<8)+'_');
const long MCHNtag			= ((long) ((((('M'<<8)+'C')<<8)+'H')<<8)+'N');
const long PDMFtag			= ((long) ((((('P'<<8)+'D')<<8)+'M')<<8)+'F');
const long SMPLtag			= ((long) ((((('S'<<8)+'M')<<8)+'P')<<8)+'L');
const long PPOStag			= ((long) ((((('P'<<8)+'P')<<8)+'O')<<8)+'S');
const long PRIMERtag		= ((long) ((((('P'<<8)+'P')<<8)+'O')<<8)+'S');

const size_t INDEXPO		= 26;			// offset of offset to index
const int	INDEX_ENTRY_LENGTH= 28;

inline
short
baseIndex(char B)
	{
	return ((B)=='C'?0:(B)=='A'?1:(B)=='G'?2:3);
	}

//	============================================================================
//	readABIInt2
//	========================================|===================================
static
bool
readABIInt2(
	FILE	*fp,
	int2	*i2)
	{
	unsigned char buf[sizeof(int2)];

	if (fread(buf, sizeof(buf), 1, fp) != 1) return (FALSE);
	*i2 = (int2)
		(((unsigned short)buf[1]) +
		 ((unsigned short)buf[0]<<8));
	return (TRUE);
	}

//	============================================================================
//	readABIInt4
//	========================================|===================================
static
bool
readABIInt4(
	FILE	*fp,
	int4	*i4)
	{
	unsigned char buf[sizeof(int4)];

	if (fread(buf, sizeof(buf), 1, fp) != 1) return (FALSE);
	*i4 = (int4)
		(((unsigned long)buf[3]) +
		 ((unsigned long)buf[2]<<8) +
		 ((unsigned long)buf[1]<<16) +
		 ((unsigned long)buf[0]<<24));
	return (TRUE);
	}

//	============================================================================
//	getIndexEntryLW
//	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -
//	From the ABI results file connected to `fp' whose index starts at byte
//	offset `indexO', return in `val' the `lw'th long word from the `count'th
//	entry labeled `label'. The result indicates success.
//	========================================|===================================
bool
getIndexEntryLW (
	FILE*	fp,
	long	indexO,
	long	label,
	long	count,
	int		lw,
	int4*	val)
	{
	int		entryNum=-1;
	int		i;
	int4	entryLabel, entryLw1;

//	if (DEBUG)
//cerr<<"getIndexEntryLW(label="<<label<<",count="<<count<<",lw="<<lw<<")"<< endl;

	do
		{
		entryNum++;
		if (fseek(fp, indexO+(entryNum*INDEX_ENTRY_LENGTH), SEEK_SET) != 0)
			return FALSE;
		if (!readABIInt4(fp, &entryLabel))
			return FALSE;
		if (!readABIInt4(fp, &entryLw1))
			return FALSE;
		} while (!(entryLabel == label && entryLw1 == count));

	for (i=2; i<=lw; i++)
		if (!readABIInt4(fp, val)) return FALSE;

	return TRUE;
	}

//	============================================================================
//	getIndexEntryW
//	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -
//	From the ALF results file connected to `fp' whose index starts at byte
//	offset `indexO', return in `val' the `lw'th	 word (int2) from the entry
//	labeled `label'. The result indicates success.
//	========================================|===================================
bool
getIndexEntryW (
	FILE*	fp,
	long	indexO,
	long	label,
	int		lw,
	int2*	val)
	{
	int		entryNum=-1;
	int		i;
	int4	entryLabel;
	int4	jval;

	do
		{
		entryNum++;
		if (fseek(fp, indexO+(entryNum*INDEX_ENTRY_LENGTH), SEEK_SET) != 0)
			return FALSE;
		if (!readABIInt4(fp, &entryLabel))
			return FALSE;
		}
		while (entryLabel != label);


	for (i=2; i<lw; i++)
		if (!readABIInt4(fp, &jval)) return FALSE;
	
	if (!readABIInt2(fp, val)) return FALSE;

	return TRUE;
	}

//	============================================================================
//	ReadABI [private]
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Read a ABI file; called exclusively by Read(FILE*,format_t).
//	Read the ABI format sequence with name `fn' into `seq'.
//
//	This code is a rehash of seqIOABI.c, significantly modified for use here.
//	bottom strand designations are completely ignored here.
//	========================================|===================================
CTraceFile::error_t
CTraceFile::ReadABI(FILE* fp, short whichSet)
	{
	if (DEBUG)	cerr << "ReadABI: Entering; set = " << whichSet << endl;

	const size_t COMMENT_LEN	= 2048;		// max comment length;
	const short TRACE_INDEX		= NUM_BASES*whichSet+1;
									// index to offset of 1st trace in set

	error_t	err = noError;
	char	line[128];
	int4	ppos;

	bool	bottom = FALSE;
	char*	enzyme = NULL;

	uint	whichTrace;
	ulong	numPoints;
	ulong	numBases;
//	int2	primerPos;
	int2	traceValue;

	int4	fwo_;							// base -> lane mapping
	int4	signalO;
	int4	indexO;							// index
	int4	baseO;							// bases
	int4	basePosO;						// base positions
	int4	dataCO;							// C trace
	int4	dataAO;							// A trace
	int4	dataGO;							// G trace
	int4	dataTO;							// T trace
	int4	MCHN_O;							// machine name
	int4	PDMF_O;							// dye primer guff
	int4	SMPL_O;							// sample name

	int		i;

	if (DEBUG)	cerr << "ReadABI: 1.0" << endl;
	// get the index offset
	if ((fseek(fp, INDEXPO, SEEK_SET) != 0) || (!readABIInt4(fp, &indexO)))
		return ioError;

	if (DEBUG)	cerr << "ReadABI: 1.1" << endl;
	// get the number of points
	if (!getIndexEntryLW(fp,indexO,DATAtag,TRACE_INDEX,3,&numPoints))
		return ioError;
	
	// get the number of bases
	if (DEBUG)	cerr << "ReadABI: 1.2" << endl;
	if (!getIndexEntryLW(fp,indexO,BASEtag,1,3,&numBases))
		return ioError;

	//	not needed; in fact it's apparently superfluous and incorrect when
	//	compared with similar code to do the same thing, below.  -- Reece
	#if (FALSE)
	// get Primer Position...where the ABI file found the start of
	if (DEBUG)	cerr << "ReadABI: 1.3" << endl;
	if (!getIndexEntryW(fp,indexO,PPOStag,6,&primerPos))
		return ioError;
	#endif

	if (DEBUG)	cerr << "ReadABI: 1.4" << endl;
	// get bases offset
	if (!getIndexEntryLW(fp,indexO,BASEtag,1,5,&baseO))
		return ioError;

	if (DEBUG)	cerr << "ReadABI: 1.5" << endl;
	// get base positions offset
	if (!getIndexEntryLW(fp,indexO,BASEPOStag,1,5,&basePosO))
		return ioError;

	if (DEBUG)	cerr << "ReadABI: 1.6" << endl;
	// The order of the DATA fields is determined by the field FWO_
	// Juggle around with data pointers to get it right
	int4*	dataxO[NUM_BASES];
	dataxO[0] = &dataCO;
	dataxO[1] = &dataAO;
	dataxO[2] = &dataGO;
	dataxO[3] = &dataTO;

	// Get the Freak World Out (FWO?) field ...
	if (!getIndexEntryLW(fp,indexO,FWO_tag,1,5,&fwo_))
		return ioError;
	//Get the positions of the four traces
	if (!(
		getIndexEntryLW(fp,indexO,DATAtag,TRACE_INDEX,
						5,dataxO[baseIndex((char)(fwo_>>24&BYTE[0]))]) &&
		getIndexEntryLW(fp,indexO,DATAtag,TRACE_INDEX+1,
						5,dataxO[baseIndex((char)(fwo_>>16&BYTE[0]))]) &&
		getIndexEntryLW(fp,indexO,DATAtag,TRACE_INDEX+2,
						5,dataxO[baseIndex((char)(fwo_>>8&BYTE[0]))]) &&
		getIndexEntryLW(fp,indexO,DATAtag,TRACE_INDEX+3,
						5,dataxO[baseIndex((char)(fwo_&BYTE[0]))]) ))
		return ioError;


	//
	// Offsets for critical data have been obtained.  Offsets for non-critical
	// info will be determined later, mostly for comment field.
	// Now we're reade to get the critical information.
	//
	if (DEBUG)	cerr << "ReadABI: 2-allocate" << endl;

	//
	// Allocate space for data, bases, and comments
	//
	if (err = Allocate((size_t)numPoints,(size_t)numBases,COMMENT_LEN))
		return err;

	//
	// Read in the traces using the offsets above
	//
	int4	dataOffset[NUM_BASES];
			dataOffset[A]=dataAO;	dataOffset[C]=dataCO;
			dataOffset[G]=dataGO;	dataOffset[T]=dataTO;

	for (whichTrace=A;whichTrace<=T;whichTrace++)
		{
		if (DEBUG)	cerr << "ReadABI: 2" << DNA_BASES[whichTrace]
						<< "; offset = " << dataOffset[whichTrace] << endl;

		if (fseek(fp, dataOffset[whichTrace], SEEK_SET))
			return ioError;
		for (i=0;i<numPoints;i++)
			if (readABIInt2(fp, &traceValue))
				(*trace[whichTrace])[i] = traceValue;
			else
				return ioError;
		}

	//
	// read in the sequence
	//
	if (DEBUG)	cerr << "ReadABI: 3" << endl;
	if (fseek(fp, baseO, SEEK_SET))
		return ioError;
	for (i=0;i<numBases;i++)
		{
		int		ch;
		if ((ch = fgetc(fp)) == EOF)
			return ioError;
		sequence[i] = (ch == 'N') ? '-' : ch;
		}

	//
	// read base positions
	//
	if (DEBUG)	cerr << "ReadABI: 4" << endl;
	for (i=0;i<numBases;i++)
		if (!readABIInt2(fp, (int2*)&basePositions[i]))
			return ioError;


	//
	// Gather useful information
	// such as signal strength, machine name, etc.  We don't have the file
	// offsets for these yet, so we must get them for each item.  Because
	// they're optional, errors are ignored after this point.
	//
	if (DEBUG)	cerr << "ReadABI: 5" << endl;
	// Get Signal Strength Offset
	if (getIndexEntryLW(fp,indexO,SIGNALtag,1,5,&signalO))
		{
		int2 C,A,G,T;
		int2* base[4];
		base[0] = &C;
		base[1] = &A;
		base[2] = &G;
		base[3] = &T;
		if (fseek(fp, signalO, SEEK_SET) >= 0 &&
			readABIInt2(fp, base[baseIndex((char)(fwo_>>24&255))]) &&
			readABIInt2(fp, base[baseIndex((char)(fwo_>>16&255))]) &&
			readABIInt2(fp, base[baseIndex((char)(fwo_>>8&255))]) &&
			readABIInt2(fp, base[baseIndex((char)(fwo_&255))]))
			{
			sprintf(line,"avg_signal_strength = C:%d A:%d G:%d T:%d\n",C,A,G,T);
			strcat(comments,line);
			}
		} // signal strength

	// the following returns absurd results (ie. on order of 10E10)
	#if (FALSE)
	// Get the spacing.. it's a float but don't worry yet
	int4	spacing;
	if (getIndexEntryLW(fp,indexO,SPACINGtag,1,5,&spacing))
		{
		sprintf(line,"avg_spacing = %lf\n",spacing);
		strcat(comments,line);
		}
	#endif

	// Get primer position
	if (getIndexEntryLW(fp,indexO,PPOStag,1,5,&ppos))
		{
		// ppos stored in MBShort of pointer
		primerPosition = ppos>>16;

		sprintf(line,"primer_position = %d\n",primerPosition);
		strcat(comments,line);
		}

	// machine name
	if ((getIndexEntryLW(fp,indexO,MCHNtag,1,5,&MCHN_O)) &&
		(fseek(fp, MCHN_O, SEEK_SET) >= 0))
		{
		unsigned char l;
		char buffer[256];					// Pascal-type string; len=buffer[0]
		fread(&l,sizeof(char),1,fp);
		fread(buffer,l,1,fp);
		sprintf(line,"machine_name = %.*s\n",l,buffer);
		strcat(comments,line);
		}

	// Dye Primer Offset
	if ((getIndexEntryLW(fp,indexO,PDMFtag,1,5,&PDMF_O)) &&
		(fseek(fp, PDMF_O, SEEK_SET) >= 0))
		{
		unsigned char l;
		char buffer[256];					// Pascal-type string; len=buffer[0]
		fread(&l,sizeof(char),1,fp);
		fread(buffer,l,1,fp);
		sprintf(line,"dye_primer = %.*s\n",l,buffer);
		strcat(comments,line);
		}

	// sample name
	if ((getIndexEntryLW(fp,indexO,SMPLtag,1,5,&SMPL_O)) &&
		(fseek(fp, SMPL_O, SEEK_SET) >= 0))
		{
		unsigned char l;
		char buffer[256];					// Pascal-type string; len=buffer[0]
		fread(&l,sizeof(char),1,fp);
		fread(buffer,l,1,fp);
		sprintf(line,"sample_name = %.*s\n",l,buffer);
		strcat(comments,line);
		}

	if (DEBUG)	cerr << "ReadABI: Exiting normally" << endl;
	return noError;
	}

//	============================================================================
//	WriteABI [private]
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Write a ABI file; called exclusively by Write(FILE*,format_t).
//	========================================|===================================
CTraceFile::error_t
CTraceFile::WriteABI(FILE* fp)
	{
	fp = NULL;
	return fmtNotSuppError;
	}


//	============================================================================
//	   #    #       #######           ###         # #######
//	  # #   #       #                  #         #  #     #
//	 #   #  #       #                  #        #   #     #
//	#     # #       #####              #       #    #     #
//	####### #       #                  #      #     #     #
//	#     # #       #                  #     #      #     #
//	#     # ####### #                 ###   #       #######
//
//	ALF format I/O methods
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	
//	========================================|===================================

//	============================================================================
//	ReadALF [private]
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Read a ALF file; called exclusively by Read(FILE*,format_t).
//	========================================|===================================
CTraceFile::error_t
CTraceFile::ReadALF(FILE* fp)
	{
	fp = NULL;
	return fmtNotSuppError;
	}

//	============================================================================
//	WriteALF [private]
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Write a ALF file; called exclusively by Write(FILE*,format_t).
//	========================================|===================================
CTraceFile::error_t
CTraceFile::WriteALF(FILE* fp)
	{
	fp = NULL;
	return fmtNotSuppError;
	}
