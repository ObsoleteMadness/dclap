//	============================================================================
//	CPeakList.C														80 columns
//	Reece Hart	(reece@ibc.wustl.edu)								tab=4 spaces
//	Washington University School of Medicine, St. Louis, Missouri
//	This source is hereby released to the public domain.  Bug reports, code
//	contributions, and suggestions are appreciated (to email address above).
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	Please see CPeakList.H for a description of the CPeakList class.
//	========================================|===================================


#include	"CPeakList.H"
#ifndef WIN_MAC
#include	"CTrace.H"
#else
#include	"CTrace.C"  // dgg
#endif
#include	"RInlines.H"
#include	<math.h>
#include	<stdlib.h>
#include	<stdio.h>

vrsn CPeakList::version = "94.05.03";

CPeakList::CPeakList(void):
	hmean(0),
	hvariance(0),

	hm0(0),
	hmdecay(0),

	hv0(0),
	hvdecay(0),

	w0(0),
	wconst(0),

	group1_left(0),
	group1_leftidx(0),
	group1_right(0),
	group1_rightidx(0),
	group1_hmean(0),
	group1_hvariance(0),
	group1_wd_mean(0),
	group1_index_mean(0),

	group2_left(0),
	group2_leftidx(0),
	group2_right(0),
	group2_rightidx(0),
	group2_hmean(0),
	group2_wd_mean(0),
	group2_hvariance(0),
	group2_index_mean(0),

	FTrace(NULL),
	PTrace(NULL),
	RTrace(NULL)
	{}

CPeakList::~CPeakList(void)
	{
	// no error to delete NULL
	delete FTrace;
	delete PTrace;
	delete RTrace;
	}


//	============================================================================
//	Analyze
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Automates the process of computing peak probabilities from a peak list.
//	========================================|===================================
bool
CPeakList::Analyze(
	CTrace<trace_t>& ct)
	{
	if (ct.Peaks().Size() != 0)
		{
		ComputeFTrace(ct.Size());
		FTrace->LeftCutoff(ct.LeftCutoff());
		FTrace->RightCutoff(ct.RightCutoff());

		ComputeRTrace(ct);
		RTrace->LeftCutoff(ct.LeftCutoff());
		RTrace->RightCutoff(ct.RightCutoff());

		CalculateStats();
		CalculatePeakStats(ct.Min());
		}
	return TRUE;
	}

//	============================================================================
//	CalculateStats
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	This is a big ugly function.  What we need to determine is:
//		1. the overall mean and variance of peak heights
//		2. the linear increase of peak width with index
//		3. the exponential decay equation for the mean (& variance) of peak
//			 heights
//	For 1, we just iterate over the whole list and get the mean & var.  Easy.
//	For 2 & 3, we wanted to divide the peak list population into 3 groups, and
//	fit the equations to means from the first and last third.  This turns out
//	to be very inaccurate because of large fluctuations in both height and
//	width in the last third.  We therefore resort to fitting the equation
//	between the first and second thirds of the population.
//	We initially tried to use the peak widths to model the decay with an
//	assumption of constant peak area, but for the same reason, this method was
//	error prone.
//	========================================|===================================
void
CPeakList::CalculateStats(void)
	{
	uint		index;
	uint		population_size;
	double		sum;
	CPeakList&	peaks = *this;

	if (size < 2)
		return;

	//
	// compute the mean & variance of height for all peaks (not groups)
	//
	for (sum=0,index=0; index < size; index++)
		sum += peaks[index].height;
	hmean = sum/size;
	for (sum=0,index=0; index < size; index++)
		sum += pow(peaks[index].height - hmean,2);
	hvariance = sum/(size-1);


	//
	// divide peaks into two groups
	// the groups will be considered as two populations for the purpose
	// of calculating means, etc.  From those, we'll try to fit an
	// exponential decay to the height and a linear function to width
	// (by least squares).
	//
	group1_left		= 0;					// group 1 = first third
	group1_right	= (uint)size/3-1;
	group1_leftidx	= (*this)[group1_left].center;
	group1_rightidx	= (*this)[group1_right].center;

	group2_left		= group1_right + 1;		// group 2 = second third
	group2_right	= (uint)size*2/3;
	group2_leftidx	= (*this)[group2_left].center;
	group2_rightidx	= (*this)[group2_right].center;


	//
	// model peak width expansion by linear least squares fit of widths
	// ie.  w(i) = wd0 + wd_const * i;
	//
	double	si	=0;							// sum of index
	double	si2	=0;							// sum of index^2
	double	sw	=0;							// sum of width
	double	sw2	=0;							// sum of width^2
	double	siw	=0;							// sum of width * index
	for (index=0; index < size; index++)
		{
		si	+= peaks[index].center;
		si2	+= pow(peaks[index].center,2);
		sw	+= peaks[index].width;
		siw	+= peaks[index].width * peaks[index].center;
		}
	wconst = (si*sw - size*siw)/(pow(si,2)*si2);
	w0 = (siw - si2*wconst)/si;


	//
	// model peak height decay with exponential
	// ie.  corrected height = hc(i) = hm0 * exp(-h_decay*i)
	// 1. divide peaks into two distinct groups (as above)
	// 2. compute mean height & variance and mean index for each group
	//		(index is necessary to compensate for skewed distribution of peaks)
	// 3. fit exponential decay to mean from the two groups
	// 4. compute hm0, the height at i=0
	//

	//
	// group 1 calculations
	//
	population_size = group1_right - group1_left + 1;

	// height mean
	for (sum=0,index=group1_left; index <= group1_right; index++)
		sum += peaks[index].height;
	group1_hmean = sum/population_size;

	// height variance
	for (sum=0,index=group1_left; index <= group1_right; index++)
		sum += pow(peaks[index].height - group1_hmean,2);
	group1_hvariance = sum/(population_size-1);

	// width mean
	for (sum=0,index=group1_left; index <= group1_right; index++)
		sum += peaks[index].width;
	group1_wd_mean = sum/population_size;

	// index mean
	for (sum=0,index=group1_left; index <= group1_right; index++)
		sum += peaks[index].center;
	group1_index_mean = sum/population_size;


	//
	// group 2 calculations
	//
	population_size = group2_right - group2_left + 1;

	// height mean
	for (sum=0,index=group2_left; index <= group2_right; index++)
		sum += peaks[index].height;
	group2_hmean = sum/population_size;

	// height variance
	for (sum=0,index=group2_left; index <= group2_right; index++)
		sum += pow(peaks[index].height - group2_hmean,2);
	group2_hvariance = sum/(population_size-1);

	// width mean
	for (sum=0,index=group2_left; index <= group2_right; index++)
		sum += peaks[index].width;
	group2_wd_mean = sum/population_size;

	// index mean
	for (sum=0,index=group2_left; index <= group2_right; index++)
		sum += peaks[index].center;
	group2_index_mean = sum/population_size;


	//
	// fit exponential
	// determine hmdecay, hm0, hvdecay, and hv0
	//
	// the following attempts to use peak width to preserve area under peak
	// hdecay = log(group1_hmean*group1_wd_mean / group2_hmean*group2_wd_mean)/
	//			(group2_index_mean - group1_index_mean);

	hmdecay = log(group1_hmean/group2_hmean)/
							(group2_index_mean - group1_index_mean);
	hm0 = group1_hmean/exp(-hmdecay * group1_index_mean);
	hvdecay = log(group1_hvariance/group2_hvariance)/
							(group2_index_mean - group1_index_mean);
	hv0 = group1_hvariance/exp(-hvdecay * group1_index_mean);
	}


//	============================================================================
//	CalculatePeakStats
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Computes P(H), P(D|NULL), P(D|H), and P(H|D) for all peaks.
//
//	Peaks are initially chosen by a concave down method (see PickPeaks).  From
//	these initial selections, the mean and variance of peak heights are
//	calculated, as are the terms for an exponential decay of peak height with
//	trace position and terms for a linear expansion of peak width.
//
//	h			= arbitrary height (ie. h=h(3)=height at position 3)
//	h(i)		= height at index i
//	h_mean		= mean of peak heights computed by CalculateStats
//	h_mean(i)	= mean of peak heights corrected for exponential decay
//	h_var		= variance of peak heights computed by CalculateStats
//	h_baseline	= baseline height
//	noise_var	= variance in signal noise; determined empirically by
//					subtracting the FTrace from observed signal.
//	FT(i)		= expected fluorescence trace of chosen peaks (see
//					ComputeFTrace)
//
//	P(H), P(D|H), and P(D|NULL) are computed using some method (see below) and
//	then plugged into Bayes' Theorem.
//	  P(H|D) = P(H) * P(D|H) / P(D|NULL)
//	========================================|===================================
void
CPeakList::CalculatePeakStats(
	double	h_baseline)
	{
	// we need the FTrace and RTrace; exit if they're not NULL
	if ((NULL == FTrace) || (NULL == RTrace))
		return;

	double noisevar=RTrace->Variance();

	for (uint index=0;index<size;index++)
		{
		PeakRec& peak = (*this)[index];		// current peak record

		// height & height variance decay exponentially
		double	fluorexp	= ExpDecay(hm0,hmdecay,index);
		double	fluorvar	= ExpDecay(hv0,hvdecay,index);

		double	varprod  = hvariance * noisevar;

		// the following are terms defined in the project report and have
		// no particular meaning other than to aggregate terms.
		// See the project report (p. 27) for details.
		double	A,B,C,D,E,F;
		A = (hvariance + noisevar)/2/varprod;
		B = (hvariance*(peak.height-h_baseline) + noisevar*fluorexp)/varprod;
		C = -(hvariance*pow((peak.height-h_baseline),2) + 
						noisevar*pow(fluorexp,2)) / 2 / varprod;
		D = sqrt(A);
		E = B/2/D;
		F = C-pow(B,2)/4/A;

		peak.PH  = 1.0;
//		peak.PH  = GaussianV((peak.height-h_baseline),fluorexp,fluorvar);
		peak.PDN = GaussianV(peak.height-h_baseline,0,noisevar);
//		peak.PDH = GaussianV(peak.height-h_baseline-fluorexp,0,noisevar);

#ifdef WIN_MAC
		peak.PDH = exp(-F)/4/D/sqrt(varprod*_PI);  // dgg - no erfc() ..
#else
		peak.PDH = exp(-F)/4/D/sqrt(varprod*_PI)*erfc(E);   
#endif
// debug print: printf("%f\t%f\t%f\t%f\t%f\t%f\n",A,B,C,D,E,F);

		// and finally Bayes' Theorem:
//		peak.PHD = peak.PH * peak.PDH / peak.PDN;
		peak.PHD = peak.PH * peak.PDH / ( peak.PDH+peak.PDN);
		}
	}

//	============================================================================
//	WriteDeltas
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	For every subsequence of length nbhd, write the index of the 3' most peak
//	center for that subsequence, the subsequence itself, and the delta for the
//	subsequence.
//	The delta between bases m and n is defined to be the distance between the
//	centers of their corresponding peaks m and n in the trace.
//	========================================|===================================
void
CPeakList::WriteDeltas(
	ostream& os,
	uint	nbhd,
	bool	header)
	{
	char*	seq = Sequence();

	if (header)
		os	<< "Pk Ctr"
			<< "\t"
			<< "Delta"
			<< "\t"
			<< "Sequence"
			<< endl;

	for(ulong index=nbhd;index<size;index++)
		{
		os	<< (*this)[index].center
			<< "\t"
			<< Delta(index,nbhd)
			<< "\t";
		for (ulong base=index-nbhd+1;base<=index;base++)
			os << seq[base];
		os	<< endl;
		}
	delete seq;
	}


//	============================================================================
//	ComputeFTrace
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Computes a trace which represents the expected fluorescence from the list
//	of peaks by assuming a Gaussian distribution at each peak (using the center,
//	height, and width fields of the PeakRec).  The extent parameter specifies
//	the horizontal extent of the Gaussian on each side of center.  The size of
//	the original trace must be passed so that the original and fluorescence
//	traces will be the same size.
//	========================================|===================================
inline
double
PeakModel(
	trace_t		height,
	tracepos	center,
	tracepos	width,
	tracepos	index)
	{
	// 								 2
	//					  -(i-center)  / (2 x width)
	// F(i,p) =	height x e
	//
	// F(i,p) is fluorescence at index i as a result of peak p
	// height = height of peak p
	// center = center of peak p
	// width  = width of peak p
	// 
	return height * exp(-pow((index-center),2)/width/2);
	}

CTrace<double>*
CPeakList::ComputeFTrace(
	size_t		sz,
	tracepos	extent)
	{
	delete FTrace; FTrace = NULL;			// recreate from scratch

	if (FTrace = new CTrace<double>(sz))
		{
		CTrace<double>&	trace=*FTrace;
		tracepos index;
		for (index=0; index<sz; index++)	// zero all indices of the FT
			trace[index]=0;

		for (size_t peak_no=0; peak_no<size; peak_no++)
			{
			PeakRec&	peak = (*this)[peak_no];	// the current peak to
													// model with Gaussian

			tracepos leftBound = peak.center-extent;
			tracepos rightBound = peak.center+extent;

			if (leftBound<0)		leftBound = 0;
			if (rightBound>sz-1)	rightBound = sz-1;

			for (index=leftBound;index<=rightBound;index++)
				trace[index] +=
						PeakModel(peak.height,peak.center,peak.width,index);
			}

		FTrace->CalculateStats();
		}
	return FTrace;
	}


//	============================================================================
//	ComputePTrace
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Generates a qualitative trace which shows peak height and width for every
//	peak in the peak list. The argument passed to ComputePTrace is the size of
//	the original trace, which will also be the size of the peak trace. 
//	========================================|===================================
CTrace<double>*
CPeakList::ComputePTrace(
	CTrace<trace_t>&	src_trace)
//	size_t		sz)
	{
	delete PTrace; PTrace = NULL;			// recreate from scratch

	tracepos	sz=src_trace.Size();
	tracepos	offset=src_trace.LeftCutoff();

	if (PTrace = new CTrace<double>((size_t)sz))
		{
		CTrace<double>& ptrace = *PTrace;

		for (tracepos index = 0; index<sz; index++)
			ptrace[index]=0;

		for (uint peak_num=0; peak_num < size; peak_num++)
			{
			PeakRec&	pr = (*this)[peak_num];
			double		height = pr.height;
			double		half_height = height/2.0;
			tracepos	midpoint = pr.center;
			tracepos	left = (tracepos)pr.leftBound;
			tracepos	right = (tracepos)pr.rightBound;
		
			for (tracepos index=left; index<=right; index++)
				if (index<sz)	ptrace[index+offset]=half_height;

			if (midpoint<sz)	ptrace[midpoint+offset]=height;
			}

		PTrace->LeftCutoff(src_trace.LeftCutoff());
		PTrace->RightCutoff(src_trace.RightCutoff());

		PTrace->CalculateStats();
		}

	return PTrace;
	}

//	============================================================================
//	ComputeRTrace
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Generates a new trace of residuals (data that cannot be explained by peaks)
//	by subtracting the computed FTrace from the original trace.  A pointer to
//	the result is stored in RTrace and returned (or NULL if error).
//	========================================|===================================
CTrace<double>*
CPeakList::ComputeRTrace(
	CTrace<trace_t>& src_trace)
	{
	// FTrace must already be computed
	if (NULL == FTrace)
		return NULL;

	delete PTrace; PTrace = NULL;			// recreate from scratch

	if (RTrace = new CTrace<double>(src_trace.Size()))
		{
		for (uint index=0;index<src_trace.Size();index++)
			(*RTrace)[index] = src_trace[index]-(*FTrace)[index];
	
		RTrace->CalculateStats();
		}

	return RTrace;		
	}


//	============================================================================
//	Offset
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Translate the trace position-dependent fields of every PeakRec to account
//	for the left cutoff.
//	========================================|===================================
void
CPeakList::Offset(
	tracepos	offset)
	{
	for (uint index = 0; index < size ; index++)
		{
		PeakRec&	pr = (*this)[index];
		pr.center += offset;
		pr.leftBound += offset;
		pr.rightBound += offset;
		}

	group1_leftidx += offset;
	group1_rightidx += offset;
	group1_index_mean += offset;
	group2_leftidx += offset;
	group2_rightidx += offset;
	group2_index_mean += offset;
	}	


//	============================================================================
//	Sequence
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Writes the NULL-terminated sequence to buf.  If buf is NULL on entry, it
//	will be created and it is the caller's responsibility to deallocate it.
//	If successful, a pointer to buf is returned; otherwise NULL is returned.
//	NOTE: use delete to deallocate, NOT free.
//	========================================|===================================
char*
CPeakList::Sequence(char* buf)
	{
	if (buf == NULL)
		buf=new char[size+1];

	if (buf != NULL)
		{
		uint i;
		for (i = 0; i<size; i++)
			buf[i]=DNA_BASES[(*this)[i].whichTrace];
		buf[i]=NULL;						// i = size (not size-1) upon exit
		}

	return buf;
	}

//	============================================================================
//	operator<<
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	Display a summary of the peak list and it's members
//	========================================|===================================
ostream&
operator<<(
	ostream&	os,
	CPeakList&	cpl)
	{
	os	<< "Peak report" << endl;
	os	<< "-----------" << endl;

	if (cpl.size == 0)
		return os << "None picked." << endl;

	// else...
	os	<< "Number picked:\t" << cpl.size << endl
		<< "height:\t\t" << cpl.hmean << "+/-" << sqrt(cpl.hvariance) << endl
		<< "ht mean decay:\t" << cpl.hm0 << " * exp(-"
								<< cpl.hmdecay << " * x)" << endl
		<< "ht var decay:\t" << cpl.hv0 << " * exp(-"
								<< cpl.hvdecay << " * x)" << endl
		<< "width:\t\t" << cpl.w0 << " + " << cpl.wconst << " * x" << endl;
	os	<< "group1:" << endl
		<< "  peaks:      [" << cpl.group1_left << "," << cpl.group1_right
			<< "]" << endl
		<< "  indices:   [" << cpl.group1_leftidx << ","
							 << cpl.group1_rightidx << "]" << endl
		<< "  height =     " << cpl.group1_hmean << "+/-"
							 << sqrt(cpl.group1_hvariance) << endl
		<< "  wdmean =     " << cpl.group1_wd_mean << endl
		<< "  index mean = " << cpl.group1_index_mean << endl;
	os	<< "group2:" << endl
		<< "  peaks:      [" << cpl.group2_left << "," << cpl.group2_right
			<< "]" << endl
		<< "  indices:   [" << cpl.group2_leftidx << ","
							 << cpl.group2_rightidx << "]" << endl
		<< "  height =     " << cpl.group2_hmean << "+/-"
							 << sqrt(cpl.group2_hvariance) << endl
		<< "  wdmean =     " << cpl.group2_wd_mean << endl
		<< "  index mean = " << cpl.group2_index_mean << endl;

	if (cpl.RTrace)
		os << "noise:\t\t" << cpl.RTrace->Mean()
			<< "+/-" << sqrt(cpl.RTrace->Variance()) << endl;

	if (cpl.size != 0)
		{
		os << setw(3) << "#" << "  " << PeakRecHeader;
		uint i=1;
		for (SeqNode<PeakRec>* sn = cpl.first;
			NULL != sn;
			sn=sn->next, i++)
			os << setw(3) << i << "  " << sn->data;

		return os;
		}

	return os;
	}


//	============================================================================
//	PeakRec::PeakRec (constructor)
//	========================================|===================================
PeakRec::PeakRec(void):
		whichTrace(X),
		center(0),
		height(0),
		width(0),
		leftBound(0),
		rightBound(0),
		PH(0),
		PDN(0),
		PDH(0),
		PHD(0)
		{}


//	============================================================================
//	PeakRecHeader Manipulator and operator<< for PeakRec
//	========================================|===================================
const int BASE_FW=6;						// base field width
const int INDEX_FW=8;						// index field width
const int PROB_FW=10;						// width for P(X) fields

ostream&
PeakRecHeader(
	ostream& os)
	// ostream manipulator to put column headings onto stream
	{
	char	buf[120];
	sprintf(buf,"%-2s%5s%7s%8s%8s%7s%10s%10s%10s%10s\n",
		"X",
		"ht",
		"ctr",
		"left",
		"right",
		"width",
		"P(H)",
		"P(D|NULL)",
		"P(D|H)",
		"P(H|D)");
	return os << buf;
#if (FALSE)
	return os
		<< setiosflags(ios::left)
		<< setw(BASE_FW)	<< "base"
		<< setiosflags(ios::right)
		<< setw(INDEX_FW)	<< "height"
		<< setw(INDEX_FW)	<< "center"
		<< setw(INDEX_FW)	<< "left"
		<< setw(INDEX_FW)	<< "right"
		<< setw(INDEX_FW)	<< "width"
		<< setw(PROB_FW)	<< "P(H)"
		<< setw(PROB_FW)	<< "P(D|NULL)"
		<< setw(PROB_FW)	<< "P(D|H)"
		<< setw(PROB_FW)	<< "P(H|D)"
		<< endl;
#endif
	}

ostream&
operator<<(
	ostream& os,
	const PeakRec& pr)
	{
	char	buf[120];
	sprintf(buf,"%-2c%5.0f%7d%8.1f%8.1f%7.1f%10.2e%10.2e%10.2e%10.2e\n",
		DNA_BASES[pr.whichTrace],
		pr.height,
		pr.center,
		pr.leftBound,
		pr.rightBound,
		pr.width,
		pr.PH,
		pr.PDN,
		pr.PDH,
		pr.PHD);
	return os << buf;
#if (FALSE)
	return os
		<< setiosflags(ios::right)
		<< setw(BASE_FW)	<< DNA_BASES[pr.whichTrace]
		<< setprecision(5)
		<< resetiosflags(ios::fixed|ios::scientific)
		<< setiosflags(ios::right)
		<< setw(INDEX_FW)	<< pr.height
		<< setw(INDEX_FW)	<< pr.center
		<< setw(INDEX_FW)	<< pr.leftBound
		<< setw(INDEX_FW)	<< pr.rightBound
		<< setw(INDEX_FW)	<< pr.width
		<< setprecision(2)					// always use 2-place scientific
		<< setiosflags(ios::scientific)		// notation for probabilities
		<< setw(PROB_FW)	<< pr.PH
		<< setw(PROB_FW)	<< pr.PDN
		<< setw(PROB_FW)	<< pr.PDH
		<< setw(PROB_FW)	<< pr.PHD
		<< endl;
#endif
	}


