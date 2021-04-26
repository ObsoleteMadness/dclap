//	============================================================================
//	CPeakList.H														80 columns
//	Reece Hart	(reece@ibc.wustl.edu)								tab=4 spaces
//	Washington University School of Medicine, St. Louis, Missouri
//	This source is hereby released to the public domain.  Bug reports, code
//	contributions, and suggestions are appreciated (to email address above).
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	This class represents a sequence of peaks picked by CTrace and is used by
//	both CTrace and CTraceFile.  It uses CSequence as a base class, but adds
//	many application-specific variables and methods which accommodate
//	statistical analysis of the peaks.
//
//	Specifically, this source collects a list of peaks and computes the
//	following:
//		1. the equation for the exponential decay of peak heights with
//			increasing index.
//		2. the equation for the linear increase in peak widths with	increasing
//			index.
//		3. the expected fluorescence trace by modeling the fluorescence of each
//			peak as a Gaussian
//		4. the peak trace, which qualitatively shows the peak height and width
//			and is useful for superimposing over the observed trace.
//		5. the residual trace, obtained by subtracting the expected fluorescence
//			from the observed fluorescence at each sample.
//		6. P(H|D) from P(H), P(D|NULL), P(D|H)  (See ComputePeakStats)
//
//	NOTES
//	*	This is the most volatile portion of this project.  For that reason,
//		error handling is rudimentary.  I haven't bothered to implement access
//		functions.
//
//	MODIFICATION HISTORY
//	93.11.11	Reece	First release
//	========================================|===================================

#ifndef _H_CPeakList						// include this file only once
#define _H_CPeakList

#include  <stdlib.h>  // dgg - for size_t
#include	<iostream.h>
#include	<iomanip.h>
//#include	"CSequence.H"
#include	"CSequence.C" // dgg
#include	"RInclude.H"
#include	"DNA.H"
#include	"Definitions.H"

template<class T>
class CTrace;								// forward declaration

struct	PeakRec
	{
	base_t		whichTrace;					// which trace (A,C,G,T)
	tracepos	offset;						// leftCutoff offset
	tracepos	center;						// index of peak
	double		height;						// value at index
	double		width;						// width at 1/2 height
	double		leftBound;					// left bound at 1/2 height
	double		rightBound;					// right bound at 1/2 height
	double		PH;							// P(H)
	double		PDN;						// P(D|NULL)
	double		PDH;						// P(D|H)
	double		PHD;						// P(H|D) = P(D|H)*P(H)/P(D|NULL)

				PeakRec(void);				// constructor

	friend
	ostream&	operator<<(ostream& os, const PeakRec& pr);
				// Writes the instance of the peak object in a single line, 100
				// columns across.  See PeakRecHeader just below class.

	friend inline 
	int			operator==(const PeakRec& r1, const PeakRec& r2);
				// Operator specifies conditions for two peaks to be considered
				// identical.
	};

	ostream&	PeakRecHeader(ostream& os);
				// ostream manipulator which writes a line of column headings
				// for the << output operator.
				// usage:  myostream << PeakRecHeader;


class CPeakList : public CSequence<PeakRec>
	{
	private:
	static vrsn	version;

	// inherit CSequence variables
	public:
	double		hmean;						// peak height mean
	double		hvariance;					//   and variance

	double		hm0;						// height mean @ i=0
	double		hmdecay;					// hm exp. decay

	double		hv0;						// height variance @ i=0
	double		hvdecay;					// hv exp. decay

	double		w0;							// width modeled by least sq. fit
	double		wconst;						// w(i)=w0 + wconst*i

	private:
	uint		group1_left;				// group 1 = first third
	uint		group1_right;
	tracepos	group1_leftidx;
	tracepos	group1_rightidx;
	double		group1_hmean;
	double		group1_hvariance;
	double		group1_wd_mean;
	double		group1_index_mean;

	uint		group2_left;				// group 2 = second third
	uint		group2_right;
	tracepos	group2_leftidx;
	tracepos	group2_rightidx;
	double		group2_hmean;
	double		group2_wd_mean;
	double		group2_hvariance;
	double		group2_index_mean;


		// dgg -- these three where private !
	public: 
	CTrace<double>* FTrace;					// computed fluorescence trace
	CTrace<double>* PTrace;					// trace of peak widths/heights
	CTrace<double>* RTrace;					// trace of residuals
	
	public:
	vrsn&		Version();					// return version # of class

				CPeakList(void);			// constructor
				~CPeakList(void);			// destructor


	char*		Sequence(char* buf=NULL);	// return sequence

	inline
	tracepos	Delta(ulong n, ushort nbhd=1);
				// The delta between bases m and n is defined to be the distance
				// between the centers of their corresponding peaks m and n in
				// the trace.  By default, it returns the delta for base n;
				// specifying nbhd returns the delta for the nbhd 3'-most bases.

	inline
	void		RemovePeak(tracepos peakCenter);
				// Removes a peak from a trace.  For the time being, it does
				// so only on the basis of peak center.

	bool		Analyze(CTrace<trace_t>& ct);// Automates the following:
				// Automates the process of computing peak probabilities from a
				// peak list.  It generates the fluorescence and residual traces
				// and (permanently) stores the results in the FTrace and RTrace
				// members.

	void		CalculateStats(void);		// compute peak statistics
				// Computes statistics for the set of peaks in the list.
				// Specifically, it computes:
				//   1. the overall mean and variance of peak heights
				//   2. the linear increase of peak width with index
				//   3. the exponential equation decay of mean peak height decay

	void		CalculatePeakStats(double h_baseline);
				// Computes the individual peak probabilities using a Gaussian
				// model for the peak fluorescence, height distribution, and 
				// noise estimate, and Bayes' Theorem to compute the probability
				// of a particular peak given the observations.

	void		WriteDeltas(ostream& os, uint nbhd, bool header=FALSE);
				// For every subsequence of length nbhd, write the index of the
				// 3' most peak center for that subsequence, the subsequence
				// itself, and the delta for the subsequence.  See Delta(...)
				// If header is TRUE, a simple column header is written first.

	void		Offset(tracepos offset);
				// Horizontally translates all references to trace indices by
				// offset (offset added to each index).  Useful for defining the
				// zero point (ie. the primer position).

	CTrace<double>*
				ComputeFTrace(size_t sz,tracepos extent=50);
				// Computes a trace which represents the expected fluorescence
				// from the list of peaks by assuming a Gaussian distribution at
				// each peak (using the center, height, and width fields of the
				// PeakRec).  The extent parameter specifies the horizontal
				// extent of the Gaussian on each side of center.  The size of
				// the original trace must be passed so that the original and
				// fluorescence traces will be the same size.

	CTrace<double>*
				ComputePTrace(CTrace<trace_t>& src_trace);
//				ComputePTrace(size_t sz);
				// Generates a trace which shows peak height and width for every
				// peak in the peak list. The argument passed to ComputePTrace
				// is the size of the original trace, which will also be the
				// size of the peak trace.  Peak traces are especially
				// informative when overlaid with the trace from which the peaks
				// were picked.

	CTrace<double>*
				ComputeRTrace(CTrace<trace_t>& src_trace);
				// Generates a new trace of residuals (data that cannot be
				// explained by peaks) by subtracting the computed FTrace from
				// the original trace.  A pointer to the result is stored in
				// RTrace and returned (or NULL if error).


	friend
	ostream&	operator<<(ostream& os, CPeakList& cpl);
				//	Display a summary of the peak list and it's members
	};


inline
vrsn&
CPeakList::Version()
	{
	return version;
	}

inline
void
CPeakList::RemovePeak(
	tracepos peakCenter)
	{
	for(ulong index=0;index<size;index++)
		if ((*this)[index].center == peakCenter)
			{ Remove(index); break; }
	}

inline
tracepos
CPeakList::Delta(ulong n, ushort nbhd)
	{
	if ((n-nbhd<0) || (n>size-1))
		return 0;
	else
		return (*this)[n].center-(*this)[n-nbhd].center;
	}

inline int operator==( const PeakRec& r1, const PeakRec& r2)
	{ return (r1.center == r2.center); }

#endif										// conditional inclusion
