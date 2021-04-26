//	============================================================================
//	CTrace.H														80 columns
//	Reece Hart	(reece@ibc.wustl.edu)								tab=4 spaces
//	Washington University School of Medicine, St. Louis, Missouri
//	This source is hereby released to the public domain.  Bug reports, code
//	contributions, and suggestions are appreciated (to email address above).
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	CTrace is a C++ template designed to store a sequence of numbers and perform
//	simple operations on it.  It was originally designed for chromatogram data
//	from automated DNA sequencing projects.
//
//	It was designed as a template for several reasons, but the most practical
//	advantage is that we can generate a CTrace of doubles for the derivatives,
//	even in cases where the original traces are shorts (for example).  One may
//	then take the derivative of that (ie. the second derivative) by an identical
//	call.  The behavior of using CTrace to store anything other than numbers is
//	undefined at best, and at worst may subject you to immense ridicule from
//	your peers (unless it works to solve the solution to the universe.)
//
//	CTrace maintains a pointer to an array of the parameterized type.  There are
//	three ways to get data into this class:
//		1. read the data yourself and pass the pointer to CTrace; don't forget
//			to use the Size(ulong) method to specify the size. (This is space
//			that you have allocated; do not pass temporary space to CTrace.)
//			You should then call CalculateStats to complete the installation.
//		2. open a file which contains a block dump of the specified type and
//			pass the FILE* and size to read to ReadTrace. (CTrace allocates the
//			space.)  By block dump, I mean something written with the equivalent
//			of fwrite(YourDataPtr,ElementSize,#ofElements,FILE*).
//		3. open an ifstream and pass it and the size to ReadAsText. (CTrace
//			will allocate the space.)
//	Data can also be written using the WriteTrace and WriteAsText methods.
//	
//	NOTES
//	!	The destructor /always/ attempts to free space pointed to by the trace
//		data member.  Set the trace pointer to NULL with Trace(NULL) if you
//		wish to assume responsibility for disposal of the space.
//	+	Statistics are automatically recalculated when necessary (ie. after
//		setting the left clipping).
//	*	Derivatives of an entire trace contain n-2 points; however, derivatives
//		of clipped traces contain the full trace size.
//
//	SLATED IMPROVEMENTS
//	*	better error mechanism (current method dirties namespace).
//			- exceptions?
//			- conserve global namespace by hiding error codes
//	*	should ensure that peak list is empty before picking
//	*	stats dirty flag in lieu of automatic recalc? (maybe faster...)
//	*	derivative size inconsistency should be fixed.  If uncalculatable deriv
//		values are filled, then it's wrong; but, we're forced to unnecissarily
//		toss deriv values in the clipped trace just to maintain consistency.
//		peakpicking may have to be modified if deriv numbering is changed.
//
//	MODIFICATION HISTORY
//	93.06.30	Reece	Original coding
//	93.07.29-31	Reece	Added Derivative, ReadAsText, WriteAsText methods
//						Converted to template, *AsText methods now use fstreams,
//						[] operator for trace access (returns lvalue).
//	93.11.11	Reece	First release
//	========================================|===================================

#ifndef _H_CTrace							// include this file only once
#define _H_CTrace

#include	<stdio.h>
#include	<iostream.h>
#include	<fstream.h>
#include	<math.h>
#include	"CSequence.H"
#include	"CPeakList.H"
#include	"DNA.H"
#include	"RInclude.H"					// homegrown definitions
#include	"RInlines.H"
#include	"Definitions.H"

enum CTError
	{
	noError,								// no error occurred
	traceEmpty,								// no trace to do that on
	traceNotEmpty,							// can't overwrite trace
	badFile,								// bogus file
	memError,								// memory error occurred
	ioError,								// i/o error occurred
	rangeError,								// something's out of bounds
	analysisError							// couldn't analyze peaks
	};


template<class T>
class	CTrace
	{
	//private:								// see note with Version(), below
	//static vrsn	version;

	//
	// Instance variables
	//
	CTError		error_flag;					// error flag
	T*			trace;						// pointer to trace
	size_t		size;						// number of trace points
	double		scale;						// scale of trace
	double		mean;						// mean trace value
	T			max;						// max trace value
	T			min;						// min trace value
	double		variance;					// variance of trace values
	T			baseline;					// baseline correction
	CPeakList	peaks;						// list of peaks
	tracepos	leftCutoff;					// ignore indices before leftCutoff
	tracepos	rightCutoff;				//   and after rightCutoff (0 based)
	
	//
	// Methods
	//
	public:
	//vrsn&		Version();					// return class version (broken)
											// there's a prize for figuring out
											// why this works for other classes
											// but not here...
	CTError		Error(void);				// get and ...
//	inline void	Error(CTError new_error=noError); //   set/clear error
	inline void	Error(CTError new_error); //   set/clear error

//				CTrace(size_t sz=0);		// constructor, allocate space
				CTrace(size_t sz);			// constructor, allocate space
				~CTrace(void);				// destructor

	CTError		AllocateTrace(size_t);		// allocate trace, update size, etc
	inline void	DeallocateTrace(void);		// deallocate & update size, etc.

	inline size_t	Size(void);				// get and ...
	inline void		Size(size_t new_size);	//   set trace size
	inline T*		Trace(void);			// get and ...
	inline void		Trace(T* tp);			//   set trace pointer (see NOTES)
	inline CPeakList& Peaks(void);			// return reference to peak list

	inline T		Min(void);				// access functions for min
	inline T		Max(void);				//   max,
	inline double	Mean(void);				//   mean, and
	inline double	Variance(void);			//   variance of trace values
	CTError			CalculateStats(void);	// compute min,max,mean,variance

	inline tracepos	LeftCutoff(void);		// get and ...
	inline void		LeftCutoff(tracepos t);	//   set left cutoff

	inline tracepos	RightCutoff(void);		// get and ...
	inline void		RightCutoff(tracepos t);//   set right cutoff

	inline T&	operator[](tracepos index);	// trace value by index

	inline void		Baseline(T bl);			// get and ...
	inline T		Baseline(void);			//   set baseline
	inline double	Scale(void);			// get trace scale
	void			Scale(double scale);	// scale trace; ie. 0.5 = scale 50%
	void			Translate(T bl);		// add bl to each element

	CTError		Smooth(void);
				// Smooths by using a weighted average of the 3 points about a
				// particular index, with the special case of the endpoints
				// handled by throwing out the third point and normalizing the
				// weighting coefficients.

	CTError		Derivative(CTrace<double>** deriv);
				// Compute the derivative of the trace and return it in a
				// CTrace<double>.  deriv will be allocated automatically.
				// If deriv can be allocated, the derivative will be generated
				// and noError returned; otherwise, an appropriate error is
				// returned.

	CTError		PickPeakIndices(
					T		MinPeakHeight,
					double	ZeroThreshold,
					CSequence<tracepos>** peaks);
				// Picks local maxima>MinPeakHeight by a concave down method.
				// Returns in a CSequence<tracepos> all indices, i, for which
				// 1) the derivative at point i is within +/- ZeroThreshold or
				// 2) derivative at the point i-1 is positive and the derivative
				// at i+1 is negative (note that this implies the second
				// derivative is negative).

	CTError		PickPeaks(
					T		MinPeakHeight,
					double	ZeroThreshold);
				// Calls PickPeakIndices to get a sequence of tracepos.  This
				// sequence is converted into a sequence of peak records and
				// the bounds and width of the peak are computed.  The arguments
				// have the same meaning as those for PickPeaks.

	CTError		PeakBounds(
					tracepos	CenterOfSearch,
					T			Elevation,
					double&		LeftIntersection,
					double&		RightIntersection,
//					tracepos	MaxWidth = 0);
					tracepos	MaxWidth);
				// Determines the left and right bounds of a concave-down peak
				// at Elevation by searching laterally no more than MaxWidth on
				// either side of CenterOfSearch (ie. [COS-MW,COS+MW]).  If no
				// data point exactly equals Elevation, the approximate bound is
				// determined by linear interpolation.  The resulting left and
				// right bounds are returned in Left~ & RightIntersection.
				// MaxWidth=0 (default) specifies no limits (ie. search is
				// exhaustive, limited only by limits of trace).  If the search
				// bounds [COS-MW,COS+MW] exceed the bounds of the trace, they
				// are automatically truncated to the nearest limit.


	// I/O
	CTError		WriteAsText(ofstream& os);
				// Writes trace from a stream as a return-delimited list of
				// points.  All numerical data types are supported.
	CTError		ReadAsText(ifstream& is, size_t);
				// Read analog of ReadAsText.

	CTError		WriteTrace(FILE*);
				// Writes trace as a block of numbers.  The good news is that
				// it's extremely fast; the bad news is that the file is
				// essentially a dump of the internal representation of the
				// numerical array and is difficult (for humans) to interpret.
	CTError		ReadTrace(FILE*, size_t);
				// The read analog to the above.

	friend ostream& operator<<(ostream& os, CTrace<T>& t);
				// Writes a user-friendly summary of CTrace data members and
				// the list of picked peaks if there are any.
	};

//
//	INLINE FUNCTION DEFINITIONS
//	===========================
//

//template<class T>
//inline
//vrsn
//CTrace<T>::Version()
//	{
//	return version;
//	}

template<class T>
inline
void
CTrace<T>::DeallocateTrace(void)
	{
	delete trace; size = 0; min = max = 0; mean = 0;
	}

template<class T>
inline
CTError
CTrace<T>::Error(void)
	{
	return error_flag;
	}

template<class T>
inline
void
CTrace<T>::Error(CTError new_error)
	{
	error_flag = new_error;
	}

template<class T>
inline
double
CTrace<T>::Mean(void)
	{
	return mean;
	}

template<class T>
inline
double
CTrace<T>::Variance(void)
	{
	return variance;
	}

template<class T>
inline
T
CTrace<T>::	Max(void)
	{
	return max;
	}

template<class T>
inline
T
CTrace<T>::	Min(void)
	{
	return min;
	}

template<class T>
inline
T&
CTrace<T>::operator[](tracepos index)
	{
	return trace[index];
	}

template<class T>
inline
void
CTrace<T>::LeftCutoff(tracepos t)
	{
	if ((t>=0) && (t<=size-1))
		{
		leftCutoff = t;
		CalculateStats();
		}
	}

template<class T>
inline
void
CTrace<T>::RightCutoff(tracepos t)
	{
	if ((t>=0) && (t<=size-1))
		{
		rightCutoff = t;
		CalculateStats();
		}
	}

template<class T>
inline
CPeakList&
CTrace<T>::Peaks(void)
	{
	return peaks;
	}

template<class T>
inline
size_t
CTrace<T>::Size(void)
	{
	return size;
	}

template<class T>
inline
void
CTrace<T>::Size(size_t new_size)
	{
	size = new_size;
	}

template<class T>
inline
T*
CTrace<T>::Trace(void)
	{
	return trace;
	}

template<class T>
inline
void
CTrace<T>::Trace(T* tp)
	{
	trace = tp;
	}

template<class T>
inline
double
CTrace<T>::Scale(void)
	{
	return scale;
	}

template<class T>
inline
void
CTrace<T>::Baseline(T bl)
	{
	baseline = bl;
	}

template<class T>
inline
T
CTrace<T>::	Baseline(void)
	{
	return baseline;
	}

template<class T>
inline
tracepos
CTrace<T>::LeftCutoff(void)
	{
	return leftCutoff;
	}

template<class T>
inline
tracepos
CTrace<T>::RightCutoff(void)
	{
	return rightCutoff;
	}

template<class T>
ostream&
operator<<(ostream& os, CTrace<T>& t)
	{
	tracepos length = t.rightCutoff-t.leftCutoff+1;
	os	<< "trace size:\t"
			<< t.Size() << endl
		<< "Window:\t\t"
			<< "[" << t.leftCutoff << "," << t.rightCutoff << "]"
			<< "  (" << length << " point" << Plural(length) << ")" << endl
		<< "min/max/mean/var: "
			<< t.Min() << "/"
			<< t.Max() << "/"
			<< t.Mean() << "/"
			<< t.Variance() << endl;
	return os;
	}

#endif										// conditional inclusion
