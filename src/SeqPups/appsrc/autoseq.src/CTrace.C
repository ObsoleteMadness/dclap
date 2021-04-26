//	============================================================================
//	CTrace.C														80 columns
//	Reece Hart	(reece@ibc.wustl.edu)								tab=4 spaces
//	Washington University School of Medicine, St. Louis, Missouri
//	This source is hereby released to the public domain.  Bug reports, code
//	contributions, and suggestions are appreciated (to email address above).
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	Please see CTrace.H for a description of the CTrace object.
//	========================================|===================================
#include	"CTrace.H"
#include	"CPeakList.H"
#include	"RInclude.H"
#include	"RInlines.H"
#include	<stdlib.h>

//template<class T>							// ATT C++ 3.01 doesn't like this
//vrsn CTrace<T>::version = "94.05.03";		// unfortunately

//	============================================================================
//	CTrace (CTrace constructor)
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	This method is called upon creation of a new instance of CTrace.  We'll
//	just make sure that the class is properly initialized and allocate a trace
//	if we're given a size.
//	========================================|===================================
template<class T>
CTrace<T>::CTrace(size_t sz) :
	trace(NULL),
	size(0),
	scale(1),
	mean(0),
	variance(0),
	max(0),
	min(0),
	baseline(0),
	leftCutoff(0),
	rightCutoff(0),
	error_flag(noError)
	{
	error_flag = AllocateTrace(sz);			// okay if sz = 0
	}

//	============================================================================
//	~CTrace (CTrace destructor)
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	Called just before deletion of this instance of CTrace.  For now, we'll just
//	ensure that the trace has been deallocated.  In the future, we may wish to
//	implement a dirty bit and check that for writing the trace, etc.
//	========================================|===================================
template<class T>
CTrace<T>::~CTrace(void)
	{
	delete(trace);							// free allocated space;
	trace = NULL;							//   and clear the pointer
	}

//	============================================================================
//	AllocateTrace
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	Allocates a trace of specified size and sets the size data member.
//	========================================|===================================
template<class T>
CTError
CTrace<T>::AllocateTrace(size_t sz)
	{
	if (0 != sz)
		{
		if ( (trace = new T[sz]) == NULL )	// allocate space
			return (error_flag=memError);	// ... failed
		size = sz;
		rightCutoff = sz - 1;
		}
	return noError;
	}

//	============================================================================
//	CalculateStats
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	Computes mean, max, min values for entire trace.
//	NOTES
//	!	It is possible that the average will be incorrectly computed for traces
//		which cause the sum variable to overflow.  This was not a concern for
//		my traces.  Overflow will depend on the size of the trace and the 
//		average value of the points.  One method to overcome this limitation is
//		to keep waypoint averages which do not overflow.
//		ie. Many machines have long double (=double) limits in the approximate
//		range [DBL_MIN = 2.2E-308, DBL_MAX = 1.8E+308].  Assuming an average
//		trace value of 1000, then we could (theoretically) store traces 2E+305
//		points long.  However, assuming an average trace value of 2E+308, we
//		can store only 1 point.  Hopefully your problem will fit somewhere in
//		this range.  ;-)
//	========================================|===================================
template<class T>
CTError
CTrace<T>::CalculateStats(void)
	{
	register tracepos index;
	long double sum;

	if ((size == 0) || (trace == NULL))
		return (error_flag=traceEmpty);

	// else...
	max = min = trace[leftCutoff];			// initialize max & min vars
	for (sum=0,index=leftCutoff; index<=rightCutoff; index++)
		{
		if (trace[index] > max)	max = trace[index];
		if (trace[index] < min)	min = trace[index];
								sum += trace[index];	// see notes
		}
	mean = (double)sum/(rightCutoff-leftCutoff+1);

	for (sum=0,index=leftCutoff; index<rightCutoff; index++)
		sum	+= pow(trace[index] - mean,2);
	if (rightCutoff-leftCutoff > 2)
		variance = (double)sum/(rightCutoff-leftCutoff);
	else
		variance = 0;

	return noError;
	} 

//	============================================================================
//	Derivative
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	Calculate the derivative of the trace using the slope of the line between
//	point i-1 and point i+1 as an approximation for the derivative at point i.
//	Returns the derivatives in a CTrace class.
//	
//	+	The trace is returned in the reference parameter.  This should be a
//		(empty) pointer to a CTrace class.
//	+	The derivatives of the first and last points cannot be computed by the
//		method above; therefore the derivative is 2 points shorter than the
//		 source trace.
//	!	For the above reason, the derivative at point i is at index i-1 of the
//		derivative trace.
//	!	User is responsible for disposing of *deriv.
//	========================================|===================================
template<class T>
CTError
CTrace<T>::Derivative(CTrace<double>** deriv)
	{
	double*		dt;							// pointer to the deriv trace
	tracepos	index;						// index of source point

	if (0 == size)
		return rangeError;

	*deriv = new CTrace<double>(size-2);
	if (*deriv == NULL)						// deriv size = original size
		return (error_flag=memError);

	dt = (*deriv)->Trace();

	for (index=1;index<=size-2;index++)
		// deriv @ i = slope of line between i-1, i+1
		//   but deriv @ i = dt[i-1]
		dt[index-1] = ((double)trace[index+1]-(double)trace[index-1])/2.0;

	(*deriv)->LeftCutoff(leftCutoff-1);
	(*deriv)->RightCutoff(rightCutoff-1);
	(*deriv)->CalculateStats();

	return noError;
	}

//	============================================================================
//	PickPeakIndices
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	Picks local maxima by a concave down method.  Returns in a
//	CSequence<tracepos> all indices, i, for which the derivative at the point
//	i-1 is positive and the derivative at i+1 is negative (note that this
//	implies the second derivative is negative).
//
//	The algorithm is currently as follows:
//	1. for a trace t with indices in [0,t_size-1], we generate a derivative,
//		dt, whose values in index positions [0,t_size-3] correspond to the
//		1st derivatives at t[1..t_size-2].  That is, dt[x] is the derivative at
//		t[x+1].
//	2.	loop for i in [1,t_size-2]
//		  if t[i] >= MinPeakHeight then
//			if fabs(dt[i-1]) < ZeroThreshold then
//			  the derivative here is 'near' zero... call it a peak
//			  append i to peak list
//			else
//			  if (i<=t-size-3) and
//				 (dt[i-1] > 0 and dt[i] < 0)
//				we've crossed 0... take index of max(t[i],t[i+1])
//				 if t[i] > t[i+1], add i to peak list
//				 else add i+1 to peak list (t[i+1]>=t[i])
//			
//	If a first derivative is within +/- ZeroThreshold, the peak is added without
//	further consideration.
//
//	!	User is responsible for disposing of *peaks (passed as an argument).
//	*	Remember that the derivative at index i of t is dt[i-1] for the reason
//		mentioned in the Derivative method (above), and that we're using 0-based
//		numbering.
//	!	This function works as described, but several extension to a more
//		general peak picking method are planned.  ie.  concave up & concave down
//		maxima/minima windows, 2nd derivative 0-crossings
//	============================================================================
template<class T>
CTError
CTrace<T>::PickPeakIndices(
	T			MinPeakHeight,				// consider only peaks >= this value
	double		ZeroThreshold,				// (abs(height) <= this) ==> peak
	CSequence<tracepos>** peaks)			// peak indices returned here
	{
	CTrace<double>*	dt=NULL;
	CTError		CTErr=noError;
	bool		CSErr=FALSE;
	tracepos	index=0;

	if (0 == size)
		{
		cerr << "CTrace::PeakPick (size_t): Fatal: Empty trace." << endl;
		return (error_flag=traceEmpty);
		}

	CTErr = Derivative(&dt);				// make the derivative and
	if (noError != CTErr)					// check for errors
		{
		cerr << "CTrace::PeakPick (size_T): Fatal: Derivative failed." << endl;
		delete dt;
		return (error_flag=CTErr);
		}

	*peaks = new CSequence<tracepos>;		// make a new CSequence to put
	if (NULL == *peaks)						// the peaks into
		{
		delete dt;
		return (error_flag=memError);
		}

	for (index=leftCutoff;index<=rightCutoff-2;index++)	// index in the trace
		{
		if (trace[index] < MinPeakHeight)	// reject heights < MinPeakHeight
			continue;

		// Any derivatives within +/- ZeroThreshold are considered to be
		// first derivative zeros; note that this implicitly includes the
		// case where the derivative exactly equals 0.  This test alone would
		// not discriminate between concave up and concave down (although the
		// MinPeakHeight test would bias toward concave down); for this reason,
		// the second & third tests are required
		if ((fabs((*dt)[index-1]) <= ZeroThreshold)		// deriv is within ZT	
			&& ((index>2) && ((*dt)[index-2]>0))		// deriv to left > 0
			&& ((*dt)[index]<0) )						// deriv to right < 0
			CSErr = (*peaks)->Append(index);			// implies concave-down
		else
			// points are outside our ZeroThreshold. Now check to see if
			// derivatives at t[index] and t[index+1] straddle zero; if so,
			// we're at a peak and we'll append index or index+1 to our peak
			// list by comparing t[index] & t[index+1].
			if ( ((*dt)[index-1] > 0) && ((*dt)[index] < 0) )
				if (trace[index] > trace[index+1])
					// first point is closer to 0
					CSErr = (*peaks)->Append(index);
				else
					// second point is closer to 0
					CSErr = (*peaks)->Append(index+1);

		if (CSErr)							// append failed...
			return (error_flag=memError);	//   assume memory error
		}

	return CTErr;
	}

//	============================================================================
//	PickPeaks
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	Calls PeakPicksIndices to get a sequence of tracepos's.  This sequence is
//	converted into a sequence of peak records and the bounds and width of the
//	peak are computed.
//	========================================|===================================
template<class T>
CTError
CTrace<T>::PickPeaks(
	T		MinPeakHeight,
	double	ZeroThreshold)
	{
	CSequence<tracepos>*	peak_positions;
	CTError					error;
	uint					index;
	const double			ONE_HALF = 0.5;
	const tracepos			MAX_WIDTH = 0;

	// pick peaks and get just a list of their positions
	error = PickPeakIndices(MinPeakHeight,ZeroThreshold,&peak_positions);
	if (noError != error)
		{
		cerr << "CTrace::PeakPick: Error from size_t PeakPick = "
			<< (int) error << endl;
		delete peak_positions;
		return (error_flag = error);
		}

	// get the maximum bounds on right and left of each peak
	for (index=0;index<peak_positions->Size();index++)
		{
		PeakRec		pr;
		double		l,r;
		CTError		pwerror;

		pr.center = (*peak_positions)[index];
		pr.height = trace[pr.center];
		pwerror = PeakBounds(pr.center,(T)(ONE_HALF*(pr.height+baseline)),\
															l,r,MAX_WIDTH);
		if (noError != pwerror)
			{
			cerr << "CTrace::PeakPick: Fatal error from PeakBounds (" << (int)pwerror
				<< ")." << endl;
			delete peak_positions;
			return (error = pwerror);
			}
		pr.leftBound = l;
		pr.rightBound = r;

		peaks.Append(pr);
		}

	delete peak_positions;					// we're done with the peak list

	// clip left and right bounds to the midpoint between two peaks in cases
	// where the right bound of peak i overlaps with the left bound of i+1.
	// I know you're thinking that I don't need the peaks.Size() if test, but
	// actually I do... it's unsigned and (unsigned long)(0-1) causes problems.
	if (peaks.Size() > 0)
		for (index=0;index<peaks.Size()-1;index++)
			{
			PeakRec&	pr1 = peaks[index];
			PeakRec&	pr2 = peaks[index+1];
	
			if (pr1.rightBound > pr2.leftBound)
				pr1.rightBound = pr2.leftBound = (pr1.center+pr2.center)/2;
			}

	// compute the peak widths
	for (index=0;index<peaks.Size();index++)
		peaks[index].width = peaks[index].rightBound - peaks[index].leftBound;

	return noError;
	}

//	============================================================================
//	PeakBounds
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Begins a lateral search within CenterOfSearch +/- MaxWidth (inclusive),
//	looking for either:
//		1. the (real) indices of trace data points which exactly equal
//			Elevation; or,
//		2. the approximation of the positions of the intersection determined
//			by linear interpolation between two points which straddle the
//			desired height.
//	The function returns the peak bounds in the formal parameters
//	LeftIntersection and RightIntersection; the actual return value of the
//	function is an error code.
//	
//	If no bound can be found for an intersection, CenterOfSearch is returned for
//	that value.  This may occur if the peak is near the end of the trace or
//	the MaxWidth is too narrow.
//
//	Specifying 0 for MaxWidth causes PeakBounds to search between CenterOfSearch
//	and the limits of the trace.  It is acceptable to provide a MaxWidth which
//	causes one boundary to be out of range; it will be adjusted to the trace
//	limit automatically.  If both are out of range, an error is returned.
//	An error is also returned if the Elevation is outside [min,max]. (min and
//	max are assumed to be correctly computed already.)
//	========================================|===================================
template<class T>
CTError	
CTrace<T>::PeakBounds(
	tracepos	CenterOfSearch,
	T			Elevation,
	double&		LeftIntersection,
	double&		RightIntersection,
	tracepos	MaxWidth)
	{
	tracepos	index;
	tracepos	left_bound  = (MaxWidth == 0 ?
								0 : CenterOfSearch - MaxWidth);
	tracepos	right_bound = (MaxWidth == 0 ?
								size-1 : CenterOfSearch + MaxWidth);

	//
	// check ranges
	//
	// return an error if:
	// 1. the user starts a search outside the trace limits, or
	// 2. /both/ bounds are outside trace limits, or
	// 3. Elevation < min
	// 4. Elevation > trace value at CenterOfSearch
	if ( (CenterOfSearch < 0) || (CenterOfSearch > size-1) )
		return rangeError;

	if ( (left_bound < 0) && (right_bound > size-1) )
		{
		cerr << "PeakBounds: left or right bound error." << endl;
		return rangeError;
		}

	if ( (Elevation < min) || (Elevation > trace[CenterOfSearch]) )
		{
		cerr << "PeakBounds: Elevation out of bounds." << endl;
		return rangeError;
		}

	//
	// adjust bounds
	// at least 1 bound is reasonable; adjust the other if necessary
	//
	if (left_bound < 0)			left_bound = 0;
	if (right_bound > size-1)	right_bound = size-1;

	//
	// set intersections to CenterOfSearch by default; correct values will
	// be determined or this will be returned to indicate failure to find
	// one or both intersections
	//
	LeftIntersection = RightIntersection = CenterOfSearch;

	//
	// begin search in [left_bound,CenterOfSearch]
	//
	for (index=CenterOfSearch; index>=left_bound; index--)
		{
		if (trace[index] == Elevation)
			{
			LeftIntersection = index;
			break;
			}
		// else...
		// if index != left_bound, then check at index-1
		if ( (index != left_bound) &&
			 ( (trace[index] > Elevation) && (trace[index-1] < Elevation) ) )
			// we straddle the Elevation
			{
			LeftIntersection =\
			  Interpolate(trace[index],index,trace[index-1],index-1,Elevation);
			break;
			}

		if (0 == index)
			break;						// otherwise we'll underflow
		}

	//
	// begin search in [CenterOfSearch,right_bound]
	//
	for (index = CenterOfSearch; index<=right_bound; index++)
		{
		if (trace[index] == Elevation)
			{
			RightIntersection = index;
			break;
			}
		// else...
		// if index != right_bound, then check at index+1
		if ( (index != right_bound) &&
			 ( (trace[index] > Elevation) && (trace[index+1] < Elevation) ) )
			// we straddle the Elevation
			{
			RightIntersection = \
			  Interpolate(trace[index],index,trace[index+1],index+1,Elevation);
			break;
			}
		}

	return noError;
	}

//	============================================================================
//	Scale
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Scales all trace values by a constant value.
//	========================================|===================================
template<class T>
void
CTrace<T>::Scale(double relative_scale)
	{
	for (tracepos index=0;index<size;index++)	// scale trace values
		trace[index] = (T)(trace[index] * relative_scale);

	scale *= relative_scale;				// new /absolute/ scale

	CalculateStats();
	}	

//	============================================================================
//	Translate
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Adds constant to all trace values
//	========================================|===================================
template<class T>
void
CTrace<T>::Translate(T bl)
	{
	if (bl == 0)
		return;

	for (tracepos index=0;index<size;index++)	// translate trace values
		trace[index] += bl;

	mean += bl;								// correct mean, max, min
	max += bl;
	min += bl;
	}	

//	============================================================================
//	Smooth
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	This is an extremely simplistic smoothing routine.  It should be embellished
//	to accommodate the abstract case of arbitrary smoothing windows with
//	arbitrary weighting matrix.  The routine currently smoothes by using a
//	weighted average of the 3 points about a particular index, with the special
//	case of the endpoints handled by throwing out the third points and
//	normalizing.
//
//	!	Computes the new statistics through every iteration.  Although this is
//		a potential bottleneck, it was left in place to ensure consistency of
//		the trace statistics.
//	========================================|===================================
template<class T>
CTError
CTrace<T>::Smooth(void)
	{
	const double _wt[] = { 0.25, 0.50, 0.25 };
	const double* wt = &_wt[1];				// now reference by wt[-1], wt[ 0],
											// and wt[+1].

	T*		strace;							// destination trace

	if (0 == size)
		return rangeError;

	if ( (strace = new T[size]) == NULL )	// allocate space
		return memError;					// ... failed

	// compute the first and last smoothed points before smoothing the rest of
	// the trace
	// we must divide by the sum of the two weights used to normalize
	strace[0] = (T)((wt[ 0]*trace[ 0]+wt[+1]*trace[+1])/(wt[ 0]+wt[+1]));
	strace[1] = (T)((wt[-1]*trace[-1]+wt[ 0]*trace[ 0])/(wt[-1]+wt[ 0]));

	// compute smoothed points for everything in between (that is, [0,size-2])
	for (tracepos index=1;index<=size-2;index++)
		// deriv @ i = slope of line between i-1, i+1
		//   but deriv @ i = dt[i-1]
		strace[index] = (T)(	wt[-1]*trace[index-1] \
							+	wt[ 0]*trace[index  ] \
							+	wt[+1]*trace[index+1] );

	delete trace;							// delete original data
	trace = strace;							// & put the smoothed data in place

	CalculateStats();

	return noError;
	}

//	============================================================================
//	ReadTrace & ReadAsText
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	ReadTrace reads size bytes from the file ifp (already opened with rb
//	permissions). Space is allocated for the trace, the size instance variable
//	is adjusted, and the trace is read.  Read trace is fast because it reads a
//	block of data (size of block = # data points * sizeof(data type)).
//
//	ReadAsText reads an endl-delimited list of values from an input file stream.
//	========================================|===================================
template<class T>
CTError
CTrace<T>::ReadTrace(FILE* ifp, size_t sz)
	{
	CTError		err;

	if (trace != NULL)						// ensure that we don't
		return (error_flag=traceNotEmpty);	// overwrite existing trace

	//else...
	if (ifp == NULL)						// check for valid file pointer
		return (error_flag=badFile);
	
	// else...
	err = AllocateTrace(sz);
	if (noError != err)
		return (error_flag=err);

	// else...
	if ( fread(trace, sizeof(T), sz, ifp) != sz )	// fread & error check
		{
		free(trace);						// fread failed to read size
		size=0;
		return (error_flag=ioError);		// bytes.
		}

	CalculateStats();

	return noError;
	}

template<class T>
CTError
CTrace<T>::ReadAsText(ifstream& is, size_t sz)
	{
	tracepos	index;
	CTError		err;

	if (NULL != trace)						// ensure that we don't
		return (error_flag=traceNotEmpty);	// overwrite existing trace

	//else...
	if ((NULL == is) || (is.bad()))			// check for valid file pointer
		return (error_flag=badFile);
	
	// else...
	err = AllocateTrace(sz);
	if (noError != err)
		return (error_flag=err);

	// else...
	for (index=0;index<sz;index++)
		{
		is >> trace[index] >> ws;
		if (is.bad())
			{
			DeallocateTrace();
			return (error_flag=ioError);
			}
		}
	CalculateStats();

	return noError;
	}

//	============================================================================
//	WriteTrace & WriteAsText
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	Writes the data pointed to by the trace data member.  These are the write
//	analogs of ReadTrace and ReadAsText; see them for more info.
//	========================================|===================================
template<class T>
CTError
CTrace<T>::WriteTrace(FILE* ofp)
	{
	if (trace == NULL)						// check for non-empty trace
		return (error_flag=traceEmpty);

	//else...
	if (ofp == NULL)						// check for valid file pointer
		return (error_flag=badFile);
	
	//else...
	tracepos length = rightCutoff - leftCutoff + 1;
	if ( fwrite(&trace[leftCutoff], sizeof(T), (size_t)length, ofp) != length )
		return (error_flag=ioError);		// fwrite & error ck

	//else...
	return noError;
	}

template<class T>
CTError
CTrace<T>::WriteAsText(ofstream& os)
	{
	tracepos	index;

	if (trace == NULL)						// check for non-empty trace
		return (error_flag=traceEmpty);

	//else...
	//if ((NULL == os) || (os.bad()))			// check for valid file pointer
	// dgg - ^^ that isn't proper -- os isn't a pointer but an object
	if ((os.bad()))			// check for valid file pointer
		return (error_flag=badFile);

	//else...
	for (index=leftCutoff;index<=rightCutoff;index++)
		{
		os << trace[index] << endl;
		if (os.bad())
			return (error_flag=ioError);
		}

	//else...
	return noError;
	}
