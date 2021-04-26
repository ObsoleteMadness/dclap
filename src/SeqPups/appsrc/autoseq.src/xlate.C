//	============================================================================
//	xlate.C															80 columns
//	Reece Hart	(reece@ibc.wustl.edu)								tab=4 spaces
//	Washington University School of Medicine, St. Louis, Missouri
//	This source is hereby released to the public domain.  Bug reports, code
//	contributions, and suggestions are appreciated (to email address above).
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	% xlate {options_list} {filename}
//	options_list ::= {'-'}{option} {options_list}
//	option ::=
//	fmt {ABI0,ABI1,ABI,SCF}
//		Read in specified format; guess if not specified.
//	o [string prefix]
//		specifies output filename; if omitted, the output file is the input
//		filename.SCF.
//	========================================|===================================

#include	"CTraceFile.H"
#include	"CTrace.H"
#include	"FileFormat.H"
#include	<stdlib.h>
#include	<string.h>
#include	<iostream.h>
#include	<stdio.h>
#include	"RInclude.H"
#include	"RInlines.H"

const char* VERSION		= "94.05.03";
const short	FNLEN		= 200;				// max filename length
const short SUFXLEN		= 5;				// max suffix length
const char	HYPHEN		= '-';				// command line switch marker
const int	digestNoProc = -1;				// returned if we shouldn't process

const char* fmtsw		= "fmt";			// format switch
const char* helpsw		= "help";			// cl switches for: help
const char* leftsw		= "l";				// left cutoff
const char*	outfnsw		= "o";				// output filename
const char* outfmtsw	= "of";				// output format
const char* rightsw		= "r";				// right cutoff
const char* versionsw	= "version";		// get version info

void	help(void);
void	Fatal(char* error)
	{
	cerr << "xlate: FATAL: " << error << endl;
	exit(1);
	}
void	Warning(char* error)
	{
	cerr << "xlate: WARNING: " << error << endl;
	}

struct opts_t
	{
	char	outFN[FNLEN];
	format_t fmt;
	format_t outfmt;
	tracepos left;
	tracepos right;

			opts_t(void);
			~opts_t(void) {}
	int		Digest(int argc, char* argv[]);
	};

inline
opts_t::opts_t(void):
	fmt(unknown), outfmt(SCF),
	left(0), right(0)
	{
	// ensure that all strings are 0-length
	outFN[0]	=	NULL;
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
		Fatal("xlate: bad command line; try % xlate -h");

	// scan for version request
	for(argIndex=1;argIndex<=argc-1;argIndex++)
		{
		flag = argv[argIndex];
		if (*flag == HYPHEN)		flag++;		// peel off the hyphen
		if (strcmp(flag,versionsw)==0)
			{
			cout << "xlate" << endl
				<< "  version " << VERSION
				<< "  [made on " << __DATE__ << " " << __TIME__ << "]" << endl
				<< "  Reece Hart (reece@ibc.wustl.edu)" << endl
				<< "  translates chromatogram files to other formats" << endl
				<< "  use '% xlate -help' for usage instructions" << endl
				<< endl;
			return digestNoProc;
			}
		}

	// scan for help
	for(argIndex=1;argIndex<=argc-1;argIndex++)
		{
		flag = argv[argIndex];
		if (*flag == HYPHEN)		flag++;		// peel off the hyphen
		if (strcmp(flag,helpsw)==0)
			{ help(); return digestNoProc; }
		}

	// return this value, which hopefully points to the first filename.
	// NOTE: loop only through argc-2 so that at least one argument remains
	// (the filename of the input file); we may stop before that if we don't
	// understand a flag, and assume that this flag is the start of the filename
	// list.
	for(argIndex=1; argIndex<argc; argIndex++)
		{
		flag = argv[argIndex];
		hyphenflag = FALSE;

		if (*flag == HYPHEN)
			{
			flag++;
			hyphenflag = TRUE;
			}

		// MODIFIERS
		// These may seg fault if the user omits an argument at the
		// end of the command line
		if (strcmp(flag,fmtsw)==0)
			{
			// sequential search through tracefile format abbreviations to find
			// match, or default to unknown format if not found
			++argIndex;
			for(short i=0;i<(short)unknown;i++)
				if (strcmp(TRACEFILE_FORMATS[i].abbr,argv[argIndex])==0)
					break;
			fmt = (format_t)i;
			// upon loop exit, i is either the format value or unknown
			continue;
			}
		if (strcmp(flag,leftsw)==0)
			{ left = atoi(argv[++argIndex]); continue; }
		if (strcmp(flag,outfnsw)==0)
			{ strcpy(outFN,argv[++argIndex]); continue; }
		if (strcmp(flag,outfmtsw)==0)
			{
			// sequential search through tracefile format abbreviations to find
			// match, or default to unknown format if not found
			++argIndex;
			for(short i=0;i<(short)unknown;i++)
				if (strcmp(TRACEFILE_FORMATS[i].abbr,argv[argIndex])==0)
					break;
			outfmt = (format_t)i;
			// upon loop exit, i is either the format value or unknown
			continue;
			}
		if (strcmp(flag,rightsw)==0)
			{ right = atoi(argv[++argIndex]); continue; }

		// DEFAULT: unrecognized flags... we're done
		if (hyphenflag)
			// we've stripped off a hyphen, which means the user intended this
			// to be a flag, but we didn't understand it.  Call help and tell
			// our caller to not process.
			{
			char buf[100];
			strcpy(buf,"Unrecognized flag '");strcat(buf,flag);strcat(buf,"'.");
			Warning(buf);
			help();
			return digestNoProc;
			}
		break;
		}

	return argIndex;				// we successfully read argIndex params
	}

int
main(int argc, char* argv[])
	{
	opts_t		options;
	int			argr;
	CTraceFile::error_t CTFError = CTraceFile::noError;

	argr = options.Digest(argc, argv);
	if (argr == digestNoProc)
		return 1;

	if (argc-argr != 1)
		Fatal("bad command line arguments -- xlate -help for info");

	char*&		FN = argv[argr];
	format_t	fmt = options.fmt;

	if (unknown == fmt)
		fmt = FileFormat(FN);

	// read file
	cout << "Reading '"<<FN<<"' ("<<FileFormatAbbr(fmt)<< ")..." << flush;
	CTraceFile	 tracefile(FN,fmt);
	CTFError = tracefile.Error();
	cout << "(" << CTFError << ")." << endl;
	if (CTraceFile::noError != CTFError)
		Fatal("Couldn't read file.");

	if (options.left !=0) tracefile.LeftCutoff(options.left);
	if (options.right!=0) tracefile.RightCutoff(options.right);

	// scale file for writing SCF
	for(uint i=A;i<NUM_BASES;i++)
		{
		CTrace<trace_t>&	trace = *(tracefile[i]);
		cout << "Trace " << DNA_BASES[i] << endl;
		cout << "min/max/mean:\t"
			 << trace.Min() << "/" << trace.Max() << "/" << trace.Mean()
			 << "...translated by " << -trace.Min() << endl;
		trace.Translate(-trace.Min());
		cout << "min/max/mean:\t"
			 << trace.Min() << "/" << trace.Max() << "/" << trace.Mean()
			 << "...scaled by " << MAX_SCF_TRACE_VALUE/trace.Max() << endl;
		trace.Scale(MAX_SCF_TRACE_VALUE/trace.Max());
		cout << "min/max/mean:\t"
			 << trace.Min() << "/" << trace.Max() << "/" << trace.Mean()<< endl;
		}

	// read file
	if (strlen(options.outFN)==0)
		{
		strcpy(options.outFN,FN);
		strcat(options.outFN,".SCF");
		}
	cout << "Writing '"<<options.outFN<<"' as "
		<< FileFormatAbbr(options.outfmt) << " file..." << flush;
	CTFError = tracefile.Write(options.outFN,options.outfmt);
	cout << "(" << CTFError << ")." << endl;
	if (CTraceFile::noError != CTFError)
		Fatal("Couldn't write file.");

	exit(0);
	}

void
help(void)
	{
cout
<< "Most help displays give useful information about what a program\n"
<< "does and how to use it.  This one doesn't.  Sorry.\n"
<< "This program converts from ABI to SCF, including converting /raw/ ABI\n"
<< "data with the -fmt ABI0 flag.  xlate is currently unsupported, but the\n"
<< "intrepid are welcome to peruse the source to discover what it does.\n";
	}

