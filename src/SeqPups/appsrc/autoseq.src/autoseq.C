
#undef WIN_MAC

//	============================================================================
//	autoseq.C														80 columns
//	Reece Hart	(reece@ibc.wustl.edu)								tab=4 spaces
//	Washington University School of Medicine, St. Louis, Missouri
//	This source is hereby released to the public domain.  Bug reports, code
//	contributions, and suggestions are appreciated (to email address above).
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	Takes a single filename of a tracefile and outputs individual files of the
//	components of the tracefile.  Any format supported by CTraceFile is
//	supported here.
//
//	autoseq was intended to be a moderately robust and complete testbed in which
//	this project could be designed, implemented, and tested.  It was not
//	designed to be everyone's solution to chromatogram analysis.  Nonetheless,
//	it's probably satisfactory for users who seek access to specific elements
//	of tracefiles and those who wish to implement their own peak picking
//	routines.
//
//	SLATED IMPROVEMENTS
//	*	AUTOSEQ env. variable for processing
//
//	MODIFICATION HISTORY
//	93.11.11	Reece	First release
//	========================================|===================================

#include	"CTraceFile.H"
#ifdef WIN_MAC
#include	"CTrace.C"  // dgg
#endif
#include	"DNA.H"
#include	"FileFormat.H"
#include	"RInclude.H"
#include	"RInlines.H"
#include	"Definitions.H"
#include	<iomanip.h>
#include	<iostream.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

const char* VERSION		= "94.05.03";
const char* COMMENT_PREFIX = "// ";			// start of line containing comment
const short	FNLEN		= 200;				// max filename length
const short SUFXLEN		= 5;				// max suffix length
const char	BASE_TOKEN	= '#';				// placeholder for base letter in fn
const char	HYPHEN		= '-';				// command line switch marker
const int	DIGEST_NO_PROC = -1;			// returned if we shouldn't process
const tracepos START_AT_PRIMER = -1;		// set leftCutoff to primer position

											// command line switches:
const char* basesw		= "b";				// base selector
const char* blsw		= "bl";				// baseline
const char* bpdsw		= "bpd";			// base position deltas
const char* bpsw		= "bp";				// base positions in trace
const char* d1sw		= "d1";				// 1st deriv
const char* d2sw		= "d2";				// 2nd deriv
const char* fmtsw		= "fmt";			// format switch
const char* fssw		= "fs";				// file sequence (stored in file)
const char* ftsw		= "ft";				// fluorescence trace
const char* fwsw		= "fw";				// fluorescence model breadth
const char* headsw		= "h";				// include headers with output
const char* helpsw		= "help";			// cl switches for
const char* indsw		= "i";				// individual traces
const char* leftsw		= "l";				// left cutoff
const char* pmcsw		= "pmc";			// peak mean coeff for threshold
const char* outprfxsw	= "o";				// output prefix
const char* prunesw		= "p";				// prune
const char* pssw		= "ps";				// predicted seq (pred by CPeakList)
const char* ptsw		= "pt";				// peak trace
const char* quietsw		= "q";				// quiet (opposite of verbose)
const char* rightsw		= "r";				// right cutoff
const char* rtsw		= "rt";				// residual trace
const char* scalesw		= "s";				// scale switch
const char* smoothsw	= "sm";				// smooth switch
const char* tfssw		= "tfs";			// tracefile summary
const char* tssw		= "ts";				// trace summary
const char* versionsw	= "version";		// get version info
const char* vsw			= "v";				// verbose
const char* xformsw		= "x";				// transpose
const char* zerosw		= "z";				// zero threshold

const tracepos	FLUOR_WIDTH_DEFAULT	= 50;	// exp. flrsnc Gaussian is this wide
const tracepos	SEPARATION_DEFAULT	= 4;	// est'd lower limit of separation
const double	PMC_DEFAULT			= 1.0;	// peak threshold = PMC_DEFAULT*avg
const double	Z_THRESH_DEFAULT	= 0.0;	// 1st-deriv zero epsilon
const ushort	MAX_NBHD			= 2;	// default neighborhood for deltas
const short		SMOOTH_ITER_DEFAULT = 2;	// # of times to smooth

const double SCALES[NUM_BASES] =			// magic scaling values determined
	{										// empirically
	17296.90264180267,						// A
	22057.42514438165,						// C
	16231.00021563674,						// G
	7007.431127025081						// T
	};

const xform_mtx	ORTHO_MATRIX =				// default 4x4 orthogonalization
											// matrix from David States.
	{										// see Transform() for format
	//				CONTRIBUTION TO
	//		 [,A]	 [,C]	 [,G]	 [,T] 
		{	 20.0,	 -9.0,	 -4.0,	  0.0	},	// FROM	[A,]
		{	 -7.0,	 10.0,	  0.5,	  0.0	},	// 		[C,]
		{	-11.0,	  0.0,	 15.0,	 -2.0	},	// 		[G,]
		{	  0.0,	  0.0,	-12.0,	 14.0	}	//		[T,]
	};

struct	opts_t;						// structure for options passed to autoseq

//
// Prototypes
//
void	help(void);
int		process(const char *FN, opts_t& options);
void	Fatal(char* error)
			{ cerr << "autoseq: FATAL: " << error << endl; exit(1); }
void	Warning(char* error);
void	Warning(char* error)
			{ cerr << "autoseq: WARNING: " << error << endl; }
inline
ostream& Banner(ostream& os)
	{
	CTraceFile		tf;
	CTrace<int>		t(0);
	CSequence<int>	s;
	CPeakList		pl;
	os<<COMMENT_PREFIX<<"autoseq       version "<<VERSION<<endl;
	os<<COMMENT_PREFIX<<"CTraceFile   version "<<tf.Version()<<endl;
	os<<COMMENT_PREFIX<<"CTrace       version "<<"unspecified"<<endl;
											 //<<t.Version()<<endl;
	os<<COMMENT_PREFIX<<"CSequence    version "<<s.Version()<<endl;
	os<<COMMENT_PREFIX<<"CPeakList    version "<<pl.Version()<<endl;
	os<<COMMENT_PREFIX<<"by Reece Hart (reece@ibc.wustl.edu)"<<endl;
	os<<COMMENT_PREFIX<<"this executable and its source are public domain.\n";
	return os;
	}

struct opts_t
	{
	// NOTE: bases uses bits 0-3 for bases A,C,G,T respectively so that
	// X=ACGT from DNA.H enum is the corresponding bit and 0x01<<X is a bitmask
	// for a particular base
	bool	verbose;
	bool	firstActionSwitch;
	format_t format;
	bool	headers;
	char	prefix[FNLEN];
	bool	pickPeaks;

	short	bases;
	tracepos left;
	tracepos right;

	// modifiers determine data processing before picking & how they're picked
	bool	bl;			double	baseline;
	bool	smooth;									short	smooth_iterations;
	bool	scale;		char scaleFN[FNLEN];		double	scales[NUM_BASES];
	bool	xform;		char xformFN[FNLEN];		xform_mtx matrix;
	double	PMC;
	double	zthresh;

	// operations which cause output
	// at least one of the following must be true
	bool	bp;			char bpFN[FNLEN];
	bool	ind;		char indFN[FNLEN];
	bool	d1;			char d1FN[FNLEN];
	bool	d2;			char d2FN[FNLEN];
	bool	fs;			char fsFN[FNLEN];
	// these operations cause peaks to be picked:
	bool	ts;			char tsFN[FNLEN];
	bool	pt;			char ptFN[FNLEN];
	bool	ft;			char ftFN[FNLEN];			tracepos fw;
	bool	rt;			char rtFN[FNLEN];
	bool	ps;			char psFN[FNLEN];
	bool	tfs;		char tfsFN[FNLEN];
	bool	bpd;		char bpdFN[FNLEN];			ushort	neighborhood;
						char bpdFNPFX[FNLEN];
	bool	prune;		char pruneFN[FNLEN];		tracepos separation;

			opts_t(void);
			~opts_t(void) {}
	void	ClearActions(void);
	int		Digest(int argc, char* argv[]);
	void	MakeTraceFNs(const char* FNprefix);
	void	MakeTracefileFN(const char* FNprefix);
	friend ostream& operator<<(ostream& os, const opts_t& opt);
	};

inline
opts_t::opts_t(void):
	pickPeaks(FALSE),
	firstActionSwitch(TRUE),
	format(unknown),
	headers(FALSE),
	verbose(TRUE),

	bases((BIT[0]<<A)+(BIT[0]<<C)+(BIT[0]<<G)+(BIT[0]<<T)),
	xform(FALSE),
	scale(FALSE),
	bl(FALSE),
	prune(FALSE),					separation(SEPARATION_DEFAULT),
	smooth(FALSE),					smooth_iterations(SMOOTH_ITER_DEFAULT),
	left(0),
	right(0),
	baseline(0),

	ind(TRUE),
	d1(TRUE),
	d2(TRUE),
	ts(TRUE),
	pt(TRUE),
	ft(TRUE),						fw(FLUOR_WIDTH_DEFAULT),
	rt(TRUE),
	tfs(TRUE),
	bp(FALSE),
	fs(FALSE),
	ps(TRUE),
	bpd(FALSE),						neighborhood(MAX_NBHD),

	PMC(PMC_DEFAULT),
	zthresh(Z_THRESH_DEFAULT)
	{
	// ensure that all strings are 0-length
	prefix[0]	= 	indFN[0]	=	d1FN[0]		= 	d2FN[0]		= \
	tsFN[0]		=	ptFN[0]		= 	ftFN[0]		=	rtFN[0]		= \
	tfsFN[0]	= 	bpFN[0]		=	fsFN[0]		=	psFN[0]		= \
	xformFN[0]	=	scaleFN[0]	=	bpdFN[0]	=	bpdFNPFX[0]	=
	pruneFN[0]	= NULL;
	}

void
opts_t::ClearActions(void)
	// For the first action specified, clear all of the defaults
	{
	if (firstActionSwitch)
		{
		bp = ind = d1 = d2 = fs = ts = pt \
			= ft = rt = ps = tfs = bpd = prune = FALSE;
		firstActionSwitch = FALSE;
		}
	}

int
opts_t::Digest(int argc, char** argv)
	{
	uint	argIndex;
	char*	flag;
	bool	hyphenflag = FALSE;			// if we remove a hyphen and get
										// something we don't understand, then
										// call help

	if (argc<2)
		Fatal("bad command line; try autoseq -help");

	// scan for version request
	for (argIndex=1;argIndex<=argc-1;argIndex++)
		{
		flag = argv[argIndex];
		if (*flag == HYPHEN)		flag++;		// peel off the hyphen
		if (strcmp(flag,versionsw)==0)
			{
			Banner(cout);
			return DIGEST_NO_PROC;
			}
		}

	// scan for help
	for (argIndex=1;argIndex<=argc-1;argIndex++)
		{
		flag = argv[argIndex];
		if (*flag == HYPHEN)		flag++;		// peel off the hyphen
		if (strcmp(flag,helpsw)==0)
			{ help(); return DIGEST_NO_PROC; }
		}

	// scan for verbose/quiet
	for (argIndex=1;argIndex<=argc-1;argIndex++)
		{
		flag = argv[argIndex];
		if (*flag == HYPHEN)		flag++;		// peel off the hyphen
		if (strcmp(flag,vsw)==0)
			{ verbose = TRUE; break; }
		if (strcmp(flag,quietsw)==0)
			{ verbose = FALSE; break; }
		}
	
	// if verbose is on, then echo the command line
	//if (verbose)
	//	{
	//	cout << "% ";
	//	for (uint i=0;i<=argc-1;i++)
	//		cout << argv[i] << " ";
	//	cout << "(" << argc-1 << " arguments)" << endl;
	//	}
	
	// Digest flags from left to right until we can't digest any more.  We'll
	// return this value, which hopefully points to the first filename.
	for (argIndex=1; argIndex<=argc-1; argIndex++)
		{
		flag = argv[argIndex];
		hyphenflag = FALSE;

		if (*flag == HYPHEN)		{ flag++; hyphenflag = TRUE; }

		// ignore verbose and verbose
		if ((strcmp(flag,vsw)==0) || (strcmp(flag,quietsw)==0))
			continue;

		// ACTIONS
		if (strcmp(flag,indsw)==0)
			{ ClearActions(); ind=TRUE; continue; }
		if (strcmp(flag,d1sw)==0)
			{ ClearActions(); d1=TRUE; continue; }
		if (strcmp(flag,d2sw)==0)
			{ ClearActions(); d2=TRUE; continue; }
		if (strcmp(flag,tssw)==0)
			{ ClearActions(); ts=TRUE; continue; }
		if (strcmp(flag,ptsw)==0)
			{ ClearActions(); pt=TRUE; continue; }
		if (strcmp(flag,ftsw)==0)
			{ ClearActions(); ft=TRUE; continue; }
		if (strcmp(flag,rtsw)==0)
			{ ClearActions(); rt=TRUE; continue; }
		if (strcmp(flag,tfssw)==0)
			{ ClearActions(); tfs=TRUE; continue; }
		if (strcmp(flag,bpsw)==0)
			{ ClearActions(); bp=TRUE; continue; }
		if (strcmp(flag,fssw)==0)
			{ ClearActions(); fs=TRUE; continue; }
		if (strcmp(flag,pssw)==0)
			{ ClearActions(); ps=TRUE; continue; }

		if (strcmp(flag,headsw)==0)
			{ headers=TRUE; continue; }

		// MODIFIERS
		// These may seg fault if the user omits an argument at the
		// end of the command line
		if (strcmp(flag,pmcsw)==0)
			{ PMC = atof(argv[++argIndex]); continue; }
		if (strcmp(flag,basesw)==0)
			{
			bases = 0;
			for (const char *base = argv[++argIndex] ; *base; base++)
				if (strchr("ACGT",*base)==NULL)
					Fatal("Invalid base list -- base must be in {A,C,G,T}.");
				else
					for (short i=0;i<NUM_BASES;i++)
						if ((*base == DNA_BASES[i]) && (!(bases>>i & BIT[0])))
							// base matches && !already selected
							{bases += (1<<i); break;}
			if (bases == 0)
				Fatal("Base specification is empty.");

			continue;
			}
		if (strcmp(flag,blsw)==0)
			{
			bl = TRUE;
			if (*argv[argIndex+1] != HYPHEN)
				baseline = atof(argv[++argIndex]);
			continue;
			}
		if (strcmp(flag,bpdsw)==0)
			{
			ClearActions();
			bpd = TRUE;
			if (*argv[argIndex+1] != HYPHEN)	// next argument is a flag
				{
				neighborhood = atoi(argv[++argIndex]);
				if ((neighborhood<1)||(neighborhood>5))
					Fatal("Neighborhood must be in [1,5].");
				}
			continue;
			}
		if (strcmp(flag,fmtsw)==0)
			{
			// sequential search through tracefile format abbreviations to find
			// match, or default to unknown format if not found
			++argIndex;
			short i;
			for (i=0;i<(short)unknown;i++)
				if (strcmp(TRACEFILE_FORMATS[i].abbr,argv[argIndex])==0)
					break;
			format = (format_t)i;
			// if the format string is found, then format is set; else = unknown
			continue;
			}
		if (strcmp(flag,fwsw)==0)
			{ fw = atoi(argv[++argIndex]); continue; }
		if (strcmp(flag,leftsw)==0)
			{
			if (*argv[argIndex+1]=='p')
				left = START_AT_PRIMER;
			else
				left = atoi(argv[argIndex+1]);
			argIndex++;
			continue;
			}
		if (strcmp(flag,outprfxsw)==0)
			{ strcpy(prefix,argv[++argIndex]); continue; }
		if (strcmp(flag,prunesw)==0)
			{
			ClearActions();
			prune=TRUE;
			if (*argv[argIndex+1] != HYPHEN)
				separation = atoi(argv[++argIndex]);
			continue;
			}
		if (strcmp(flag,rightsw)==0)
			{ right = atoi(argv[++argIndex]); continue; }
		if (strcmp(flag,scalesw)==0)
			{
			scale=TRUE;

			if (*argv[argIndex+1] == HYPHEN)	// next argument is a flag
				memmove(scales,SCALES,sizeof(SCALES));
			else
				{
				strcpy(scaleFN,argv[++argIndex]);
				ifstream ifs(scaleFN, ios::in);
				if (ifs.bad())
					Fatal("couldn't open scale file");
				ifs >> scales[A] >> scales[C] >> scales[G] >> scales[T];
				if (ifs.bad())
					Fatal("error reading scale file");
				}
			continue;
			}
		if (strcmp(flag,smoothsw)==0)
			{
			smooth = TRUE;
			if ((argIndex<argc-2) && (*argv[argIndex+1] != HYPHEN))
				smooth_iterations = atoi(argv[++argIndex]);
			continue;
			}
		if (strcmp(flag,xformsw)==0)
			{
			xform=TRUE;
			if (argIndex>argc-2)
				Fatal("bad command line; try autoseq -help");
			if (*argv[argIndex+1] == HYPHEN)
				memmove(matrix,ORTHO_MATRIX,sizeof(xform_mtx));
			else
				{
				strcpy(xformFN,argv[++argIndex]);
				cout << "reading " << xformFN << endl;
				ifstream ifs(xformFN, ios::in);
				if (ifs.bad())
					Fatal("couldn't open transformation file");
				ifs >> matrix[A][A]>>matrix[A][C]>>matrix[A][G]>>matrix[A][T] 
					>> matrix[C][A]>>matrix[C][C]>>matrix[C][G]>>matrix[C][T] 
					>> matrix[G][A]>>matrix[G][C]>>matrix[G][G]>>matrix[G][T] 
					>> matrix[T][A]>>matrix[T][C]>>matrix[T][G]>>matrix[T][T];
				if (ifs.bad())
					Fatal("error reading transformation file");
				}
			continue;
			}
		if (strcmp(flag,zerosw)==0)
			{ zthresh = atof(argv[++argIndex]); continue; }

		// DEFAULT: unrecognized flags... we're done
		if (hyphenflag)
			// we've stripped off a hyphen, which means the user intended this
			// to be a flag, but we didn't understand it.  Call help and tell
			// our caller to not process.
			{
			char buf[100];
			strcpy(buf,"Unrecognized flag '");
			strcat(buf,flag);
			strcat(buf,"'; try autoseq -help.");
			Fatal(buf);
			}
		break;
		}

	// normal exits only
	pickPeaks=(ts || tfs || pt || ft || rt || ps || bpd || prune);

	return argIndex;				// filename list starts at argIndex
	}

void
opts_t::MakeTraceFNs(const char* FNprefix)
	{
	if (strlen(FNprefix)>FNLEN-SUFXLEN)
	  // string too long
	  Fatal("Filename is too long to construct destination names.");

	char	tempFN[FNLEN];
	strcpy(tempFN,FNprefix);

	// Filenames are constructed by replacing the BASE_TOKEN with the base
	// letter.  Thus, every filename which represents a data pertaining to a
	// single base must have a BASE_TOKEN in it.  Check to see if the user
	// provided one; if not, concatenate '.'BASE_TOKEN (ie ".#") to the
	// filename prefix.  The base token will be replaced by the base letter
	// during processing.
	if (NULL == strrchr(tempFN,BASE_TOKEN))
		{
		// BASE_TOKEN not found
		strcat(tempFN,". ");			// leave a space for the BASE_TOKEN
		tempFN[strlen(tempFN)-1]=BASE_TOKEN;
		}

	// build filenames for trace-specific data
	if (ind)	{ strcpy(indFN,tempFN); }
	if (d1)		{ strcpy(d1FN,tempFN); strcat(d1FN,".d1");		}
	if (d2)		{ strcpy(d2FN,tempFN); strcat(d2FN,".d2");		}
	if (ts)		{ strcpy(tsFN,tempFN); strcat(tsFN,".ts");		}
	if (pt)		{ strcpy(ptFN,tempFN); strcat(ptFN,".pt");		}
	if (ft)		{ strcpy(ftFN,tempFN); strcat(ftFN,".ft");		}
	if (rt)		{ strcpy(rtFN,tempFN); strcat(rtFN,".rt");		}
	}

void
opts_t::MakeTracefileFN(const char* FNprefix)
	{
	if (strlen(FNprefix)>FNLEN-SUFXLEN)
	  // string too long
	  Fatal("Filename is too long to construct destination names.");

	char	tempFN[FNLEN];
	strcpy(tempFN,FNprefix);

	// remove # if it exists
	char*	start;
	while (start = strrchr(tempFN,BASE_TOKEN))
		// # was found at start... move memory left 1 character
		while (*start = *(start+1))		// shifts memory left until NULL
			start++;
	
	// The filename prefix is now some string, possibly empty, which doesn't
	// contain the BASE_TOKEN.  If the string is not 0-length, then we'll
	// append a period to it to separate the prefix from the suffix to be added
	// subsequently.
	if ((*tempFN != NULL) &&
		(tempFN[strlen(tempFN)-1] != '.') &&
		(tempFN[strlen(tempFN)-1] != '/'))
		strcat(tempFN,".");

	// build filenames for files which represent tracefile data
	// (as opposed to trace-specific data)
	if (tfs)	{ strcpy(tfsFN,tempFN);		strcat(tfsFN,"tfs");		}
	if (bpd)	{
				strcpy(bpdFNPFX,tempFN);
				strcat(bpdFNPFX,"bpd");
				strcpy(bpdFN,bpdFNPFX);
				if (neighborhood!=1)
					sprintf(&bpdFN[strlen(bpdFN)],"[1-%d]",neighborhood);
				else
					strcat(bpdFN,"1");
				}
	if (bp)		{ strcpy(bpFN,tempFN);		strcat(bpFN,"bp");			}
	if (fs)		{ strcpy(fsFN,tempFN);		strcat(fsFN,"fseq");		}
	if (ps)		{ strcpy(psFN,tempFN);		strcat(psFN,"pseq");		}
	if (prune)	{ strcpy(pruneFN,tempFN);	strcat(pruneFN,"pruned");	}
	}

ostream&
operator<<(ostream& os, const opts_t& opt)
	{
	// general options
		os << "verbose:                  " << YesNo(opt.verbose) << endl;
		os << "headers:                  " << YesNo(opt.headers) << endl;
		os << "file format:              " << FileFormatAbbr(opt.format)
			<< " ["	<< FileFormatDesc(opt.format) << "]" << endl;
	if (opt.prefix)
		os << "file prefix:              " << opt.prefix << endl;

	// baseline
	if (opt.bl)
		{
		os << "translate traces by:      ";
		if (opt.baseline==0)
										os << "min value";
		else
										os << -opt.baseline;
		os << endl;
		}

	// scaling
	if (opt.scale)
		{
		os << "scale A,C,G,T traces by";
		if (NULL != opt.scaleFN)
			os << " '" << opt.scaleFN << "'";
		os << ":" << endl;
		os 	<< "  A: " << opt.scales[A] << endl
			<< "  C: " << opt.scales[C] << endl
			<< "  G: " << opt.scales[G] << endl
			<< "  T: " << opt.scales[T] << endl;
		}

	// smoothing
	if (opt.smooth)
		os << "smooth traces:            "
			<< opt.smooth_iterations
			<< " time" << Plural(opt.smooth_iterations) << endl;

	// transform matrix
	if (opt.xform)
		{
		os << "transform trace vector with ";
		if (NULL == *opt.xformFN)
			os << "standard matrix." << endl;
		else
			os << "'" << opt.xformFN << "'." << endl;
		os	<< "          A        C        G        T" << endl;
		for (uint i=A;i<NUM_BASES;i++)
			{
			cout << DNA_BASES[i] << "   ";
			for (uint j=A;j<NUM_BASES;j++)
				{
				os.setf(ios::right,ios::adjustfield);
				os.width(7);
				os.precision(3);
				os << opt.matrix[i][j] << "  ";
				}
			cout << endl;
			}
		}

	// actions to take, and specification of bases and trace indices
	if (opt.ind || opt.d1 || opt.d2 || opt.ts || opt.pt || opt.ft || opt.rt)
					// only write base specification if it'll be used
					os << "for bases "
						<< (opt.bases>>A & BIT[0] ? "A" : "")
						<< (opt.bases>>C & BIT[0] ? "C" : "")
						<< (opt.bases>>G & BIT[0] ? "G" : "")
						<< (opt.bases>>T & BIT[0] ? "T" : "")
						<< " ";
	if ((0 == opt.left) && (0 == opt.right))
					os << "over entire trace range";
	else
		{
		// print range, with handling for special cases
		os	<< "in trace range [";
		if (opt.left==START_AT_PRIMER)
			os << "primer";
		else
			os << opt.left;
		os << ",";
		if (opt.right<1)
			os << -opt.right << " from end";
		else
			os << opt.right;
		os << "]";
		}
	os << ", do the following:" << endl;

	if (opt.ind) os<<"    individual traces to:     '" << opt.indFN <<"'"<<endl;
	if (opt.d1)  os<<"    1st derivative traces to: '" << opt.d1FN  <<"'"<<endl;
	if (opt.d2)  os<<"    2nd derivative traces to: '" << opt.d2FN  <<"'"<<endl;
	if (opt.ts)  os<<"    trace summaries to:       '" << opt.tsFN  <<"'"<<endl;
	if (opt.pt)  os<<"    peak traces to:           '" << opt.ptFN  <<"'"<<endl;
	if (opt.ft)  os<<"    fluorescence traces to:   '" << opt.ftFN  <<"'"<<endl;
	if (opt.rt)  os<<"    residual traces to:       '" << opt.rtFN  <<"'"<<endl;
	if (opt.bp)  os<<"    base positions to:        '" << opt.bpFN  <<"'"<<endl;
	if (opt.fs)  os<<"    file sequence to:         '" << opt.fsFN  <<"'"<<endl;
	if (opt.ps)  os<<"    predicted sequence to:    '" << opt.psFN  <<"'"<<endl;
	if (opt.tfs) os<<"    tracefile summary to:     '" << opt.tfsFN <<"'"<<endl;
	if (opt.bpd) os<<"    base pos. deltas ("
				   << opt.neighborhood << ") to:  '"   << opt.bpdFN <<"'"<<endl;

	if (opt.pickPeaks)
		// only write these if they'll be used
		{
		if (opt.prune) os<<"    pruned peak list ("
				   << opt.separation   << ") to:  '" << opt.pruneFN <<"'"<<endl;
		os << "Peak height threshold =   " << opt.PMC
			<< " * Mean Trace Value" << endl;
		os << "1d zero threshold =       " << opt.zthresh << endl;
		os << "Gaussian model width =    " << 2 * opt.fw + 1
			<< " (= center pt + 2 * " << opt.fw
			<< " pts each side of center)" << endl;
					}
	return os;
	}

inline
char*
Substitute(const char* src, char* dest, uint base)
	// substitute base letter for place holder in filename
	// I assume that BASE_TOKEN is in the string, which is guaranteed for the
	// use in this source... and guaranteed to seg fault if this assumption is
	// false.
	{
	strcpy(dest,src);
	*strrchr(dest,BASE_TOKEN)=DNA_BASES[base];
	return dest;
	}


#ifdef WIN_MAC
//#include <console.h>
extern "C" int ccommand(char **argv[]);

// use with ChildApp.c
extern "C" int RealMain( int argc, char *argv[])

#else

int
main(int argc, char* argv[])
#endif
	{
	opts_t		options;
	int			argIndex;					// the next arg to read
	char		outpath[FNLEN];

#ifdef WIN_MAC
	//argc= ccommand( &argv);
#endif

	argIndex = options.Digest(argc, argv);

	if (argIndex == DIGEST_NO_PROC)			// we've been told to not process
		return 0;

	if (options.verbose)
		Banner(cout);
	
	while (argIndex<argc)
		{
		if (options.prefix[0] == NULL)
			{
			// no output specified... prefix is just input filename
			// strcpy(outpath,argv[argIndex]);

			// no output specified... prefix is empty
			// (actually, using prefix = # means base, and will be trimmed to
			// empty for output which isn't base-specific)
			outpath[0]=BASE_TOKEN;
			outpath[1]=NULL;
			}
		else
			{
			// output specified... if it ends with '/', then it's a directory
			// prefix and we need to append the filename to it; otherwise, it's
			// a full file prefix specifier
			strcpy(outpath,options.prefix);
			if (outpath[strlen(outpath)-1]=='/')
				{
				//strcat(outpath,argv[argIndex]);
				strcat(outpath,".");
				outpath[strlen(outpath)-1]=BASE_TOKEN;
				}
			}
		// by now, outpath is a full file pathname
		// chop off the .s1 suffix if it exists
// dgg mod >> need to generalize this beyond ".s1" or leave s1 on...
#if 1

#else
		if ((strlen(outpath)>=3) &&
			(strcmp(&outpath[strlen(outpath)-3],".s1")==0))
			outpath[strlen(outpath)-3]=NULL;
#endif

		options.MakeTraceFNs(outpath);
		options.MakeTracefileFN(outpath);
	
		if (options.verbose)
			cout << setfill('=') << setw(80) << "" << setfill(' ') << endl
				<< options;
	
		if (process(argv[argIndex],options))
			Fatal("Error occurred during processing");

		argIndex++;
		}

	if (options.verbose)
		cout << setfill('=') << setw(80) << "" << setfill(' ') << endl;

	return 0;
	}

int
process(const char *FN,	opts_t& options)
	{
	const bool&	verbose = options.verbose;
	const bool&	headers = options.headers;
	const short& bases = options.bases;
	format_t	fmt = options.format;
	uint		traceIndex;
	char		tempFN[FNLEN];
	//ofstream*	os;
	CTraceFile::error_t CTFError = CTraceFile::noError;
	CTError		CTError = noError;
	//CTraceFile	 tracefile(FN,fmt);		// declared when file is read

	//
	// read file
	//
	if (fmt == unknown)
		fmt = FileFormat(FN);

	if (verbose)
		cout << setfill('-') << setw(80) << "" << setfill(' ') << endl
			<<"Reading '"<<FN<<"' ("<<FileFormatAbbr(fmt)<< ")..." << flush;

	CTraceFile	 tracefile(FN,fmt);
	CTFError = tracefile.Error();
	if (verbose)
		cout << "(" << (int)CTFError << ")." << endl;

	// assume file doesn't exist if fmt is FFerr
	if (CTraceFile::noError != CTFError)
		{
		char buf[200];
		strcpy(buf,"error while reading file '");
		strcat(buf,FN);
		strcat(buf,"'; trying to continue.");
		Warning(buf);
		return 0; // continue
		}

	//
	// set cutoffs
	//
	if (options.left !=0)
		{
		tracepos lc = (	options.left==START_AT_PRIMER ?
						tracefile.PrimerPosition() :
						options.left);
		if (lc<0)
			Fatal("Bad left cutoff specification; check trace limits.");
		if (lc>tracefile.NumPoints()-2)
			Fatal("Left cutoff would leave empty trace window.");
		tracefile.LeftCutoff(lc);
		}
	if (options.right!=0)
		{
		// remember... options.right<0, therefore we're shifting it /left/
		tracepos rc = (	options.right>0 ?
						options.right :
						tracefile.NumPoints()+options.right);
		if (rc<tracefile.LeftCutoff())
				Fatal("Attempt to right cutoff < left cutoff.");
		if (rc>tracefile.RightCutoff())
				Fatal("Attempt to set right cutoff > size of trace.");
	
		tracefile.RightCutoff(rc);
		}


	if (verbose)
		cout << tracefile
			<< setfill('-') << setw(80) << "" << setfill(' ') << endl;

	//
	// smooth
	//
	if (options.smooth)
		{
		if (verbose)
			cout << "smoothing ("
				<< options.smooth_iterations
				<< " passes):\t";

		ushort iter = options.smooth_iterations;
		while(iter--)
			for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
				tracefile[traceIndex]->Smooth();

		tracefile.CalculateStats();

		if (verbose)
			cout << "done." << endl;
		}

	//
	// translate
	//
	if (options.bl)
		{
		if (verbose)
			cout << "translating: (";

		for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
			{
			double bl = options.baseline;
			if (0 == bl)
				bl = tracefile[traceIndex]->Min();

			if (verbose)
				cout <<DNA_BASES[traceIndex]<<":"<<-bl;
			if (verbose && (traceIndex!=T)) cout <<"; "<<flush;

			tracefile[traceIndex]->Translate(-bl);
			}

		if (verbose)
			cout << ")" << endl;
		}

	//
	// scaling
	//
	if (options.scale)
		{
		if (verbose)
			cout << "scaling traces:\t";

		for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
			{
			if (verbose)
				cout << 1/options.scales[traceIndex] << " " <<flush;

			tracefile[traceIndex]->Scale(1/options.scales[traceIndex]);
			}

		if (verbose)
			cout << endl;
		}

	//
	// transform
	//
	if (options.xform)
		{
		if (verbose)
			cout << "transforming traces..." << flush;

		CTFError = tracefile.Transform(options.matrix);

		if (verbose)
			cout << "(" << (int)CTFError << "); " << flush;

		if (CTraceFile::noError != CTFError)
			Fatal("Transformation failed...exiting");

		if (verbose)
			cout << "translating: (";

		for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
			{
			double bl = tracefile[traceIndex]->Min();
			if (verbose)	cout <<DNA_BASES[traceIndex]<<":"<<-bl;
			if (verbose && (traceIndex!=T)) cout <<"; "<<flush;
			tracefile[traceIndex]->Translate(-bl);
			}

		if (verbose)
			cout << ")" << endl;
		}

	//
	// write individual traces
	//
	if (options.ind)
		for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
			{
			if (!(bases>>traceIndex & BIT[0]))	continue;

			Substitute(options.indFN,tempFN,traceIndex);
			//os = new ofstream(tempFN,ios::out);
			ofstream os(tempFN);  

			if (verbose)
				cout << "Writing individual trace " << DNA_BASES[traceIndex]
					<< " to '" <<tempFN<<"'..."<<flush;

			if (headers)
				//*os << Banner
				os << Banner
					<< COMMENT_PREFIX << DNA_BASES[traceIndex] << " Trace of "
					<< "'"<<FN<<"'" << endl
					<< *tracefile[traceIndex];

			CTError = tracefile[traceIndex]->WriteAsText(os); //*os 

			if (verbose)
				cout << "(" << (int)CTError << ")." << endl;

			//delete os;

			if (noError != CTError)
				Fatal("Error writing individual trace.");
			}

	//
	// write derivative traces
	//
	if (options.d1 || options.d2)
		{
		for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
			{
			if (!(bases>>traceIndex & BIT[0]))	continue; // skip this base

			CTrace<trace_t>&	trace = *tracefile[traceIndex];
			CTrace<double>*	d1trace=NULL;
			CTrace<double>*	d2trace=NULL;

			// make the 1d trace (need for the d2 even if we don't write d1)
			if (verbose)
				cout << "Trace " << DNA_BASES[traceIndex]
					<< ": making 1st derivative..." << flush;

			CTError = trace.Derivative(&d1trace);

			if (verbose)
				cout << "(" << (int)CTError << "); " << flush;

			if (noError != CTError)
				{
				if (verbose) cout << endl;
				Fatal("Error making derivative.");
				}

			// write the d1 trace if we're supposed to (we may have created
			// d1 just to make d2)
			if (options.d1)
				{
				Substitute(options.d1FN,tempFN,traceIndex);

				if (verbose)
					 cout << "writing '" << tempFN << "'..." << flush;

				//os = new ofstream(tempFN,ios::out);
				ofstream os(tempFN);
				if (headers)
					os << Banner
						<< COMMENT_PREFIX << "1st derivative of "
						<< "'"<<FN<<"'" << endl
						<< *d1trace;
				CTError = d1trace->WriteAsText(os);

				if (verbose)
					cout << "(" << (int)CTError << ")." << endl;

				if (noError != CTError)
					Fatal("Error writing derivative.");
				} // options.d1
			else
				if (verbose)
					cout << "not written." << endl;

			// make and write the d2 trace
			if (options.d2)
				{
				if (verbose)
					cout << "Trace " << DNA_BASES[traceIndex]
						<< ": making 2nd derivative..." << flush;

				CTError = d1trace->Derivative(&d2trace);

				if (verbose)
					cout << "(" << (int)CTError << "); " << flush;

				if (noError != CTError)
					{
					if (verbose) cout << endl;
					Fatal("Error making derivative.");
					}
				Substitute(options.d2FN,tempFN,traceIndex);

				if (verbose)
					cout << "writing '" << tempFN << "'..." << flush;

				ofstream os(tempFN);
				if (headers)
					os << Banner
						<< COMMENT_PREFIX << "2nd derivative of "
						<< "'"<<FN<<"'"
						<< *d2trace << endl;
				CTError = d2trace->WriteAsText(os);

				if (verbose)
					cout << "(" << (int)CTError << ")." << endl;

				if (noError != CTError)
					Fatal("Error writing derivative.");
				delete d2trace;
				} // options.d2
			delete d1trace;
			} // for loop through traces
		}
		// rof (trace loop)
	// fi (d1 || d2)

	//
	// pick peaks
	//
	if (options.pickPeaks)
		{
		// set baseline values
		if (verbose)
			cout << "Setting baseline to " << options.baseline
				<< " for all traces..." << flush;

		for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
			tracefile[traceIndex]->Baseline(options.baseline);

		if (verbose)
			cout << "done." << endl;

		if (verbose)
			cout << "Picking peaks for all traces..." << flush;

		CTFError = tracefile.PickPeaks(options.PMC,options.zthresh);

		if (verbose)
			cout << "(" << (int)CTFError << ").  ["
				<< tracefile.Peaks().Size() << " peaks picked]"
				<< endl;

		if (CTFError != CTraceFile::noError)
			Fatal("Error during peakpicking.");


		//
		// adjust all peak records to record positions relative to leftCutoff
		//
		if (tracefile.LeftCutoff() != 0)
			{
			// translate the tracefile's peak records
			tracefile.Peaks().Offset(-tracefile.LeftCutoff());

			// and the peak records for the individual traces
			for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
				tracefile[traceIndex]->Peaks().Offset(-tracefile.LeftCutoff());
			}


		//
		// prune peaks
		//
		if (options.prune)
			{
			ofstream os(options.pruneFN);

			if (verbose)
				cout << "Pruning peak list (sep=" << options.separation<< ")\n"
					 << "   writing to '" << options.pruneFN << "'..." <<flush;

			ulong	szBefore = tracefile.Peaks().Size();

			if (headers)
				os	<< "minimum pairwise separation = " << options.separation
					<< "; deleted peaks marked with *; output is 100 cols wide"
					<< endl
					<< PeakRecHeader;

			tracefile.PrunePeaks(options.separation, &os);

			if (verbose)
				cout << szBefore << "-"<< tracefile.Peaks().Size()
					 << "=" << szBefore-tracefile.Peaks().Size()
					 << " peaks removed." << endl;
			}

		//
		// write individual peak traces
		//   (do this before shifting peak records in the next step)
		if (options.pt)
			for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
				{
				if (!(bases>>traceIndex & BIT[0]))	continue;
				CTrace<trace_t>&	trace = *tracefile[traceIndex];

				Substitute(options.ptFN,tempFN,traceIndex);

				if (verbose)
					cout << "Trace " << DNA_BASES[traceIndex]
						<< " peak trace: computing..." << flush;

//				trace.Peaks().ComputePTrace(trace.Size());
				trace.Peaks().ComputePTrace(trace);

				if (verbose)
					cout << "writing to '" << tempFN << "'..." << flush;

				ofstream os(tempFN);
				if (headers)
					os << Banner
						<< COMMENT_PREFIX << "Peak trace "
						<< DNA_BASES[traceIndex] << " of file "
						<< "'" << FN << "'" << endl;

				CTError = trace.Peaks().PTrace->WriteAsText(os);

				if (verbose)
					cout << "(" << (int)CTError << ")." << endl;
				}
		}

	//
	// write individual trace summaries
	//
	if (options.ts)
		for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
			{
			if (!(bases>>traceIndex & BIT[0]))	continue;
			CTrace<trace_t>&	trace = *tracefile[traceIndex];
	
			Substitute(options.tsFN,tempFN,traceIndex);

			if (verbose)
				cout << "Writing trace " << DNA_BASES[traceIndex]
					<< " summary (" << trace.Peaks().Size()
					<< " peaks) to '" << tempFN << "'..." << flush;

			ofstream os(tempFN);
			if (headers)
				os << Banner
					<< COMMENT_PREFIX << "Summary of trace "
					<< DNA_BASES[traceIndex] << " of file " << "'"<<FN<<"'"
					<< endl;

			os  
#ifndef WIN_MAC
				<< trace  
#endif
				<< trace.Peaks();
				
			if (verbose)
				cout << "done." << endl;
			}

	//
	// write individual fluorescence traces
	// (already computed from PickPeaks)
	//
	if (options.ft)
		for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
			{
			if (!(bases>>traceIndex & BIT[0]))	continue;
			CTrace<trace_t>&	trace = *tracefile[traceIndex];

			Substitute(options.ftFN,tempFN,traceIndex);

			if (verbose)
				cout << "Trace " << DNA_BASES[traceIndex]
					<< " expected fluorescence: writing to '"
					<< tempFN << "'..." << flush;

			ofstream os(tempFN);
			if (headers)
				os << Banner
					<< COMMENT_PREFIX << "Fluorescence trace "
					<< DNA_BASES[traceIndex] << " of file " << "'"<<FN<<"'"
					<< endl;
			CTError = trace.Peaks().FTrace->WriteAsText(os);

			if (verbose)
				cout << "(" << (int)CTError << ")." << endl;
			}

	//
	// write individual residual traces
	//
	if (options.rt)
		for (traceIndex=A; traceIndex<NUM_BASES; traceIndex++)
			{
			if (!(bases>>traceIndex & BIT[0]))	continue;
			CTrace<trace_t>&	trace = *tracefile[traceIndex];

			Substitute(options.rtFN,tempFN,traceIndex);

			if (verbose)
				cout << "Trace " << DNA_BASES[traceIndex]
					<< " residual: writing to '"
					<< tempFN << "'..." << flush;

			ofstream os(tempFN);
			if (headers)
				os << Banner
					<< COMMENT_PREFIX << "Residual trace "
					<< DNA_BASES[traceIndex] << " of file '" << FN << "'"<<endl;
			CTError = trace.Peaks().RTrace->WriteAsText(os);

			if (verbose)
				cout << "(" << (int)CTError << ")." << endl;
			}

	//
	// write tracefile summary
	//
	if (options.tfs)
		{
		if (verbose)
			cout << "Writing tracefile summary (" << tracefile.NumPeaks()
				<< " peaks) to '" << options.tfsFN << "'..." << flush;

		ofstream os(options.tfsFN);
		if (headers)
			os << Banner
				<< COMMENT_PREFIX << "Summary of file '" << FN <<"'"<< endl;
		os << tracefile;

		if (verbose)
			cout << "done." << endl;
		}

	//
	// write base position deltas
	//
	if (options.bpd)
		for (ushort nbhd=1; nbhd<=options.neighborhood; nbhd++)
			{
			// bpd filename generated by appending the nbhd to bpd
			strcpy(tempFN,options.bpdFNPFX);
			sprintf(&tempFN[strlen(tempFN)],"%d",nbhd);

			if (verbose)
				cout << "Writing base position deltas for nbhd=" << nbhd
					 << " to '" << tempFN << "'..." << flush;

			ofstream os(tempFN);
			if (headers)
				os << Banner
					<< COMMENT_PREFIX << "Base position deltas of file '"
					<< FN << "' for nbhd=" << nbhd << endl;

			tracefile.Peaks().WriteDeltas(os,nbhd,headers);

			if (verbose)
				cout << "done." << endl;
			}


	//
	// write file sequence
	//
	if (options.fs)
		{
		if (verbose)
			cout << "Writing file sequence (" << strlen(tracefile.Sequence())
				<< " bases) to '" << options.fsFN << "'..." << flush;

		ofstream os(options.fsFN);
		if (headers)
			os << Banner
				<< COMMENT_PREFIX << "Sequence of file '" << FN << "'" << endl;
		os << tracefile.Sequence();

		if (verbose)
			cout << "done." << endl;
		}

	//
	// write predicted sequence
	//
	if (options.ps)
		{
		CSequence<PeakRec>&	peaks = tracefile.Peaks();

		if (verbose)
			cout << "Writing predicted sequence (" << tracefile.Peaks().Size()
				<< " bases) to '" << options.psFN << "'..." << flush;

		ofstream os(options.psFN);
		if (headers)
			os << Banner
				<< COMMENT_PREFIX << "Predicted sequence of tracefile '" << FN
					<< "'" << endl;
		char* seq=tracefile.Peaks().Sequence();
		os << seq;
		delete seq;

		if (verbose)
			cout << "done." << endl;
		}

	//
	// write base positions
	//
	if (options.bp)
		{
		if (verbose)
			cout << "Writing base positions (" << strlen(tracefile.Sequence())
				<< " bases) to '" << options.bpFN << "..." << flush;

		ofstream os(options.bpFN);
		if (headers)
			os << Banner
				<<COMMENT_PREFIX<<"DNA base positions of file '"<<FN<<"'"<<endl;
		for (uint i=0;i<strlen(tracefile.Sequence());i++)
			os << tracefile.BasePositions()[i] << "\t"
				<< tracefile.Sequence()[i] << endl;

		if (verbose)
			cout << "done." << endl;
		}

	return 0;
	}

void
help(void)
	{
const int	FLAG_FLD_WIDTH	=4;
#define		INDENT			"    "		// FLG_FLD_WIDTH

// customizable delimiters to facilitate exporting to man page, FrameMaker table
// in order to make these concatenatable, they're defines rather than const
// the output format is:
//		[FP <flag> FS AP [<arglist> AS] DP <first line> [LS <lines>] ER]
// [] = repeated/optional element


#if FALSE
	// tab-delimited fields, return delimited records for flags
	#define FP	<< ""
	#define FS	""
	#define	AP	"\t"
	#define AS	""
	#define DP	"\t"
	#define LS	" "
	#define ER	"\n"
#elif FALSE
	// man page output
	#define FP	<< setw(FLAG_FLD_WIDTH)	<< ".B \-"
	#define FS	" "
	#define	AP	""
	#define AS	"\n" INDENT	FS
	#define DP	""
	#define LS	AS
	#define ER	"\n"
#else
	// standard output
	#define FP	<< setw(FLAG_FLD_WIDTH)	// Flag Prefix; ensures std flag width
	#define FS	" "						// Flag Suffix; at least one space after
	#define	AP	""						// Arg Prefix 
	#define AS	"\n" INDENT	FS			// Arg Suffix
	#define DP	""						// Description Prefix
	#define LS	AS						// Line Separator
	#define ER	"\n"					// End Record
#endif									// end delimiter definitions


cout.setf(ios::left);

cout
<< "autoseq (version " << VERSION << ") help\n"
<< "-------------------------------\n"
<< "  autoseq extracts a variety of information from SCF and ABI files and\n"
<< "  writes these components to files.  It acts as an interface to CTrace,\n"
<< "  CTraceFile, & CPeakList to perform a operations on the data, and the\n"
<< "  results of these operations are also directed to files.\n\n";

cout
<< "command line structure:\n"
<< "% autoseq {flag_list} {filename}\n"
<< "flag_list ::= {'-'} {flag} {flag_list}\n"
<< "flag ::= 1 of the following:\n";

cout
FP << basesw << FS AP "[A,C,G,T]0+" AS
DP "Specifies to which bases individual actions should be performed." 
LS "More than one base may be specified. The default is -bACGT (all bases)." 
<< ER

FP << blsw << FS AP "[short bl=<trace minimum>]" AS
DP "Set the baseline of the selected traces to bl. If bl is omitted then each" 
LS "trace is translated downward by the the minimum value for that trace."
<< ER

FP << bpdsw << FS AP "[+integer maxnbhd=" << MAX_NBHD << "]" AS
DP "For every subsequence of length nbhd (1<=nbhd<=maxnbhd) in the predicted"
LS "sequence, write the index of the 3' most peak center for that subsequence,"
LS "the delta for the subsequence, and the subsequence itself.  The delta"
LS "between bases m and n is defined to be the distance between the centers of"
LS "the peaks corresponding to bases m and n."
<< ER

FP << bpsw << FS AP
DP "Output the base-to-trace-position mapping stored in file [prefix.bp]" 
<< ER

FP << d1sw << FS AP
DP "Compute 1st derivative trace [prefix.#.d1]" 
<< ER

FP << d2sw << FS AP
DP "Compute 2nd derivative trace [prefix.#.d2]" 
<< ER

FP << fmtsw << FS AP "[{ABI0,ABI1,ABI,SCF} fmt=<best guess>]" AS
DP "Read in specified format; guess if not specified." 
LS INDENT << setw(5) << FileFormatAbbr(ABI0) << " - " << FileFormatDesc(ABI0) <<
LS INDENT << setw(5) << FileFormatAbbr(ABI1) << " - " << FileFormatDesc(ABI1) <<
LS INDENT << setw(5) << FileFormatAbbr(ABI)  << " - " << FileFormatDesc(ABI)  <<
LS INDENT << setw(5) << FileFormatAbbr(SCF)  << " - " << FileFormatDesc(SCF)
<< ER

FP << fssw << FS AP
DP "Write the sequence predicted stored in the file. [prefix.fseq]"
<< ER

FP << ftsw << FS AP
DP "Writes the trace of expected fluorescence from the list of selected peaks."
LS "Expected fluorescence is calculated by fitting a Gaussian at the peak"
LS "center using the peak height & width; see "<<fwsw<<". [prefix.#.ft]" 
<< ER

FP << fwsw << FS AP "[+short fw=" << FLUOR_WIDTH_DEFAULT << "]" AS
DP "Specifies the breadth of the Gaussian used to model the fluorescence of" 
LS "each peak (ie. Gaussian limits are [peakcenter - fw , peakcenter + fw])." 
<< ER

FP << headsw << FS AP
DP "Include descriptive headers in output files" 
<< ER

FP << helpsw << FS AP
DP "This help display" 
<< ER

FP << indsw << FS AP
DP "Write the individual traces after translation ("<<blsw<<"), scaling ("<<scalesw<<"), " 
LS "smoothing ("<<smoothsw<<"), and transformation ("<<xformsw<<"). [prefix.#]" 
<< ER

FP << leftsw << FS AP "[+int index]|'p'" AS
DP "Specifies the left cutoff index. Peak positions are reported relative" 
LS "to the left cutoff.  If 'p' is passed as the argument, the left cutoff"
LS "is set to the primer position."
<< ER

FP << outprfxsw << FS AP "[string prefix=<input filename>]" AS
DP "Specifies file prefixes for output filenames.  If the prefix is a file,"
LS "suffixes will be added as appropriate.  If the prefix is a directory (that"
LS "is, ends in a '/'), files will be redirected to that directory and"
LS "the input filename will be used as the filename prefix.  The hash symbol"
LS "('#') may be used a placeholder for the base identifier; if it is omitted,"
LS ".# will be appended to the prefix specified with this flag."
<< ER

FP << pmcsw << FS AP "[+double PMC=" << PMC_DEFAULT << "]" AS
DP "Specifies the minimum peak height as a product of the mean and the" 
LS "Peak Mean Coefficient (PMC).  For instance, if the mean trace value" 
LS "is 20, then -a 1.5 will only pick peaks above 24 =(1.2 * 20)." 
<< ER

FP << prunesw << FS AP "[+int separation=" << SEPARATION_DEFAULT << "]" AS
DP "Prunes peaks by doing a pairwise comparison of adjacent peaks and"
LS "discarding the less-probably peak of each pair which is separated by less"
LS "than separation.  If os is specified, a list of pairwise comparisons which"
LS "resulted in the removal of a peak is output. [prefix.pruned]"
<< ER

FP << pssw << FS AP
DP "Write the predicted sequence (the sequence chosen by CPeakList)" 
<< ER

FP << ptsw << FS AP
DP "Generates traces which represent peaks and their widths & heights." 
LS "Peak traces are especially informative when overlaid with the" 
LS "individual ("<<indsw<<") trace.  [prefix.#.pt]" 
<< ER

FP << quietsw << FS AP
<< "Quiet mode.  Only errors will be displayed." 
<< ER

FP << rightsw << FS AP "[int index]" AS
DP "Specifies the right cutoff index.  If index is <0, the right cutoff" 
LS "will be set to +index from the end of the trace.  See "<<leftsw<<"." 
<< ER

FP << rtsw << FS AP
DP "Output the residual trace computed by subtracting the expected fluorescence"
LS "trace from the observed data [prefix.#.rt]" 
<< ER

FP << scalesw << FS AP "[string filename]" AS
DP "Scale traces using scales from filename (in ACGT order), or default scales"
LS "if not specified." 
<< ER

FP << smoothsw << FS AP "[short iterations="<<SMOOTH_ITER_DEFAULT<<"]" AS
DP "Smooths by using a weighted average of the 3 points about a particular"
LS "index, with the special case of the endpoints handled by throwing out the"
LS "third point and normalizing the weighting coefficients.  The process is"
LS "repeated iterations times."
<< ER

FP << tfssw << FS AP
DP "Output a summary of tracefile statistics and peaks [prefix.tfs]" 
<< ER

FP << tssw << FS AP
DP "Output a summary of individual trace statistics and peaks [prefix.#.ts]" 
<< ER

FP << versionsw << FS AP
DP "Print version info" 
<< ER

FP << vsw << FS AP
DP "(extremely) Verbose mode"
<< ER

FP << xformsw << FS AP "[string filename]" AS
DP "Applies a 4x4 transformation/orthogonalization matrix to the 4 traces,"
LS "producing a new set of traces which replaces the existing set."
LS "Transformation matrics are expected to be 4x4 matrix of the form:"
LS "(file consists of mTS values only)"		
LS INDENT "M =         [,A]    [,C]    [,G]    [,T]"
LS INDENT "    [A,]    mAA     mAC     mAG     mAT"
LS INDENT "    [C,]    mCA     mCC     mCG     mCT"
LS INDENT "    [G,]    mGA     mGC     mGG     mGT"
LS INDENT "    [T,]    mTA     mTC     mTG     mTT"
LS "The general equation for the resulting traces is:"
LS "R = M O  <==>  R(T,i) =      sum     [ mTS x O(S,i) ]"
LS "                         S in {ACGT}"
LS "where R is the resulting vector of 4 traces"
LS "      O is the original vector of 4 traces"
LS "      M is the 4x4 ({ACGT}x{ACGT}) matrix whose elements m(i,j) are"
LS "        the cross-term contributions of channel j to channel i"
LS "      S & T are trace identifiers (Source & Target) in {A,C,G,T}"
LS "      i loops over the indices of the trace"
<< ER

FP << zerosw << FS AP "[+double threshold=" << Z_THRESH_DEFAULT << "]" AS
DP "Specifies the epsilon about zero in which derivatives are considered to"
LS "be exactly zero, and thus the crest (trough) of a local maxima (minima)."
<< ER;

cout <<
LS "Argument specifications are given in square brackets ('[' & ']') with their"
LS "associated flags where applicable.  Usually exactly one argument is"
LS "expected; 0+ means that zero or more arguments may follow; 0,1 indicates"
LS "0 or 1 arguments are expected (ie. it's optional).  If a flag has an"
LS "optional argument which is omitted by the user, the following flag must"
LS "begin with a hyphen ('-'); otherwise, the hyphen is optional.  Default"
LS "values are designated in the argument specification by an equal sign ('=')."
LS "If any operation (ie. a flag that results in an output file) is specified,"
LS "then none of the default operations are performed unless specifically"
LS "requested.  If a flag causes an output file to be written, the default"
LS "filename for that file is indicated in square brackets.  Generally,"
LS "filnames are generated by appending a suffix to the input filename and are"
LS "written to the same directory as the input file; the user may specify an"
LS "alternate prefix or destination directory with the -o flag.  The hash"
LS "symbol ('#') is a placeholder for the base letter when the output file"
LS "writes base-specific information; # may be used on the command line.  If"
LS "filename ends with '.s1' the suffix is removed.  Peaks will be picked"
LS "only if necessary for the requested operations."
LS "WARNING: If file with an output filename already exists, it will be"
LS "         overwritten.";
	}
