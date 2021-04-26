#define MAXARGS 100

typedef struct {
	char str[64];
	int *flag;
	int type;
	char **arg;
} cmd_line_data;

/* 
   command line switches
*/
int setoptions = -1;
int sethelp = -1;
int setinteractive = -1;
int setgapopen = -1;
int setgapext = -1;
int setpwgapopen = -1;
int setpwgapext = -1;
int setoutorder = -1;
int setpwmatrix = -1;
int setmatrix = -1;
int setnegative = -1;
int setoutput = -1;
int setoutputtree = -1;
int setquicktree = -1;
int settype = -1;
int setcase = -1;
int settransitions = -1;
int setseed = -1;
int setscore = -1;
int setwindow = -1;
int setktuple = -1;
int setkimura = -1;
int settopdiags = -1;
int setpairgap = -1;
int settossgaps = -1;
int setnopgap = -1;
int setnohgap = -1;
int sethgapres = -1;
int setuseendgaps = -1;
int setmaxdiv = -1;
int setgapdist = -1;
int setdebug = -1;
int setoutfile = -1;
int setinfile = -1;
int setprofile1 = -1;
int setprofile2 = -1;
int setalign = -1;
int setnewtree = -1;
int setusetree = -1;
int setbootstrap = -1;
int settree = -1;

/*
   multiple alignment parameters
*/
float 		dna_gap_open = 10.0,  dna_gap_extend = 5.0;
float 		prot_gap_open = 10.0, prot_gap_extend = 0.05;
int 		neg_matrix = FALSE;
int 		gap_dist = 8;
int 		output_order   = INPUT;
int    		divergence_cutoff = 40;
int	        matnum = 1;
char 		mtrxname[FILENAMELEN+1] = "blosum";
char 		hyd_residues[] = "GPSNDQEKR";
Boolean		no_hyd_penalties = FALSE;
Boolean		no_pref_penalties = FALSE;
Boolean		use_endgaps = FALSE;
Boolean		reset_alignments  = TRUE;		/* DES */

/*
   pairwise alignment parameters
*/
float  		dna_pw_go_penalty = 10.0,  dna_pw_ge_penalty = 5.0;
float 		prot_pw_go_penalty = 10.0, prot_pw_ge_penalty = 0.1;
Boolean  	quick_pairalign = FALSE;
int	        pw_matnum = 1;
char 		pw_mtrxname[FILENAMELEN+1] = "blosum";
Boolean		is_weight = TRUE;
int		new_seq;

/*
   quick pairwise alignment parameters
*/
int   	     	dna_ktup      = 2;   /* default parameters for DNA */
int    	    	dna_wind_gap  = 5;
int    	    	dna_signif    = 4;
int    	    	dna_window    = 4;

int        	prot_ktup     = 1;   /* default parameters for proteins */
int        	prot_wind_gap = 3;
int        	prot_signif   = 5;
int        	prot_window   = 5;
Boolean         percent=TRUE;
Boolean		tossgaps = FALSE;
Boolean		kimura = FALSE;


int	        boot_ntrials  = 1000;
unsigned int    boot_ran_seed = 111;


int    		debug = 0;

Boolean        	explicit_dnaflag = FALSE; /* Explicit setting of sequence type on comm.line*/
Boolean        	lowercase = TRUE; /* Flag for GDE output - set on comm. line*/

Boolean        	explicit_outfile = FALSE; /* Explicit setting of output file on comm.line*/
Boolean        	explicit_treefile = FALSE; /* Explicit setting of guide file on comm.line*/

Boolean        	output_clustal = TRUE;
Boolean        	output_gcg     = FALSE;
Boolean        	output_phylip  = FALSE;
Boolean        	output_nbrf    = FALSE;
Boolean        	output_gde     = FALSE;
Boolean         showaln        = TRUE;
Boolean         save_parameters = FALSE;

/* DES */
Boolean        	output_tree_clustal   = FALSE;
Boolean        	output_tree_phylip    = TRUE;
Boolean        	output_tree_distances = FALSE;

static char *res_cat[] = {
                "ACST",
                "NT",
                "EQ",
                "NQ",
                "ED",
                "KRQH",
                "GN",
                "NH",
                "FY",
                "IVLM",
                NULL };
             /* "ACVILMF",
                "STN",
                "GACSTV",
                "NDEQ",
                "FYW",  */



static char *type_arg[] = {
                "protein",
                "dna",
		""};

static char *outorder_arg[] = {
                "input",
                "aligned",
		""};

static char *case_arg[] = {
                "lower",
                "upper",
		""};

static char *score_arg[] = {
                "percent",
                "absolute",
		""};

static char *output_arg[] = {
                "gcg",
                "gde",
                "pir",
                "phylip",
		""};

static char *outputtree_arg[] = {
                "nj",
                "phylip",
                "dist",
		""};

/*
     command line initialisation

     type = 0    no argument
     type = 1    integer argument
     type = 2    float argument
     type = 3    string argument
     type = 4    filename
     type = 5    opts
*/
#define NOARG 0
#define INTARG 1
#define FLTARG 2
#define STRARG 3
#define FILARG 4
#define OPTARG 5

cmd_line_data cmd_line[] = {
     "help",		&sethelp,		NOARG,	NULL,
     "check",        	&sethelp,    		NOARG,	NULL,
     "options",		&setoptions,		NOARG,	NULL,

/* command line switches for DATA       **************************/
     "infile",		&setinfile,		FILARG,	NULL,
     "profile1",	&setprofile1,		FILARG,	NULL,
     "profile2",	&setprofile2,		FILARG,	NULL,

/* command line switches for VERBS      **************************/
     "align",		&setalign,		NOARG,	NULL,
     "newtree",		&setnewtree,		FILARG,	NULL,
     "usetree",		&setusetree,		FILARG,	NULL,
     "bootstrap",	&setbootstrap,		NOARG,	NULL,
     "tree",		&settree, 		NOARG,	NULL,
     "quicktree",	&setquicktree,		NOARG,	NULL,
     "interactive",	&setinteractive,	NOARG,	NULL,

/* command line switches for PARAMETERS **************************/
     "type",		&settype,		OPTARG,	type_arg,
     "matrix",		&setmatrix,		FILARG,	NULL,
     "negative",	&setnegative,		NOARG,	NULL,
     "gapopen", 	&setgapopen,		FLTARG,	NULL,
     "gapext",		&setgapext,		FLTARG,	NULL,
     "endgaps",		&setuseendgaps,		NOARG,	NULL,
     "norgap",		&setnopgap,		NOARG,	NULL,
     "nohgap",		&setnohgap,		NOARG,	NULL,
     "hgapresidues",	&sethgapres,		STRARG,	NULL,
     "maxdiv",		&setmaxdiv,		INTARG,	NULL,
     "gapdist",		&setgapdist,		INTARG,	NULL,
     "pwmatrix",	&setpwmatrix,		FILARG,	NULL,
     "pwgapopen",	&setpwgapopen,		FLTARG,	NULL,
     "pwgapext",	&setpwgapext,		FLTARG,	NULL,
     "ktuple",		&setktuple,		INTARG,	NULL,
     "window",		&setwindow,		INTARG,	NULL,
     "pairgap",		&setpairgap,		INTARG,	NULL,
     "topdiags",	&settopdiags,		INTARG,	NULL,
     "score",		&setscore,		OPTARG,	score_arg,
     "transitions",	&settransitions,	NOARG,	NULL,
     "seed",		&setseed,		INTARG,	NULL,
     "kimura",		&setkimura,		NOARG,	NULL,
     "tossgaps",	&settossgaps,		NOARG,	NULL,
     "debug",		&setdebug,		INTARG,	NULL,
     "output",		&setoutput,		OPTARG,	output_arg,
     "outputtree",	&setoutputtree,		OPTARG,	outputtree_arg,
     "outfile",		&setoutfile,		FILARG,	NULL,
     "outorder",	&setoutorder,		OPTARG,	outorder_arg,
     "case",		&setcase,		OPTARG,	case_arg,

     "",		NULL,			-1};

