/* Menus and command line interface for Clustal W  */
/* DES was here MARCH. 1994 */
/* DES was here SEPT.  1994 */
/* >> no, dgg jan96: made strcmp(params) into stricmp(params) throughout, 
   	     to permit CaseSensitive Unix filenames 
  >> stricmp/strcasecmp isn't part of ansi std string.h lib
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include "clustalw.h"
#include "param.h"

static jmp_buf jmpbuf;
#define BADSIG (void (*)())-1

static void jumper()
{
        longjmp(jmpbuf,1);
}


/*
*	Prototypes
*/

extern double countid(int, int);
extern void   *ckalloc(size_t);
extern void   ckfree(void *);
extern void   getstr(char *,char *);
extern double getreal(char *,double,double,double);
extern int    getint(char *,int,int,int);
extern void   do_system(void);
extern int    readseqs(int);
extern void   get_path(char *,char *);
extern void   bootstrap_tree(void);
extern void   error(char *,...);
extern int    SeqGCGCheckSum(char *, int);
extern void   show_pair(void);
extern int    malign(int istart);
extern int    palign1(void);
extern int    palign2(void);
extern void   pairalign(int istart, int iend, int jstart, int jend);
extern int    read_user_matrix(char *, int *, int *);
extern void   guide_tree(void);
extern void   phylogenetic_tree(void);
extern int    read_tree(char *, int, int);
extern int    calc_distances(int);
extern void   clear_tree(treeptr);



void 	init_amenu(void);
void 	parse_params(void);
void 	main_menu(void);
FILE 	*open_output_file(char *, char *, char *, char *);
FILE 	*open_explicit_file(char *);
#ifdef UNIX
FILE 	*open_path(char *);
#endif

static int check_param(char *inline1,char *params[], char *param_arg[]);
static int find_match(char *probe, char *list[], int n);
static void get_help(char);			/* Help procedure */
static void show_aln(void);
static void pair_menu(void);
static void multi_menu(void);
static void gap_penalties_menu(void);
static void multiple_align_menu(void);          /* multiple alignments menu */
static void profile_align_menu(void);           /* profile       "      "   */
static void phylogenetic_tree_menu(void);       /* NJ trees/distances menu  */
static int  read_matrix(char **,char *,int, int *, int *);
static Boolean user_mat(char *, int *, int *);
static void seq_input(void);
static void align(void);
static void new_sequence_align(void);
static void make_tree(void);
static void get_tree(void);
static void clustal_out(FILE *, int);
static void nbrf_out(FILE *, int);
static void gcg_out(FILE *, int);
static void phylip_out(FILE *, int);
static void gde_out(FILE *, int);
static Boolean open_alignment_output(char *);
static void create_alignment_output(int);
static void create_parameter_output(void);
static void reset(void);
static void profile_input(int);   		/* read a profile */
static void format_options_menu(void);          /* format of alignment output */
static void tree_format_options_menu(void);     /* format of tree output */
static void profile_align(void);                /* Align 2 alignments */

/*
*	 Global variables
*/

extern double  **tmat;
extern float    gap_open,      gap_extend;
extern float  	dna_gap_open,  dna_gap_extend;
extern float 	prot_gap_open, prot_gap_extend;
extern float    pw_go_penalty,      pw_ge_penalty;
extern float  	dna_pw_go_penalty,  dna_pw_ge_penalty;
extern float 	prot_pw_go_penalty, prot_pw_ge_penalty;
extern float 	revision_level;
extern int      wind_gap,ktup,window,signif;
extern int      dna_wind_gap, dna_ktup, dna_window, dna_signif;
extern int      prot_wind_gap,prot_ktup,prot_window,prot_signif;
extern int 	boot_ntrials;		/* number of bootstrap trials */
extern int	nseqs;
extern int	new_seq;
extern int 	*seqlen_array;
extern int 	divergence_cutoff;
extern int 	debug;
extern int 	neg_matrix;
extern Boolean  quick_pairalign;
extern Boolean	reset_alignments;		/* DES */
extern int 	gap_dist;
extern Boolean 	no_hyd_penalties, no_pref_penalties;
extern int 	first_seq, last_seq;
extern int 	max_aa, gap_pos1, gap_pos2;
extern int 	*output_index, output_order;
extern int 	usermat[], pw_usermat[];
extern int 	aa_xref[], pw_aa_xref[];

extern Boolean 	lowercase; /* Flag for GDE output - set on comm. line*/
extern Boolean 	output_clustal, output_nbrf, output_phylip, output_gcg, output_gde;
extern Boolean 	output_tree_clustal, output_tree_phylip, output_tree_distances;
extern Boolean 	tossgaps, kimura;
extern Boolean  percent;
extern Boolean 	explicit_dnaflag;  /* Explicit setting of sequence type on comm.line*/
extern Boolean 	explicit_outfile;  /* Explicit setting of output file on comm.line*/
extern Boolean 	explicit_treefile; /* Explicit setting of guide file on comm.line*/
extern Boolean 	usemenu;
extern Boolean 	showaln, save_parameters;
extern Boolean	dnaflag,is_weight;
extern unsigned int boot_ran_seed;


extern FILE 	*tree;
extern FILE 	*clustal_outfile, *gcg_outfile, *nbrf_outfile, *phylip_outfile;
extern FILE 	*gde_outfile;
extern FILE     *phylip_phy_tree_file;

extern char 	hyd_residues[];
extern char 	*amino_acid_codes;
extern char 	*nucleic_acid_order;
extern char 	mtrxname[], pw_mtrxname[];
extern char 	*paramstr;
extern char	seqname[];
extern char 	phylip_phy_tree_name[];
extern char 	**seq_array;
extern char 	**names, **titles;

static char 	outfile_name[FILENAMELEN+1];
static char 	clustal_outname[FILENAMELEN+1], gcg_outname[FILENAMELEN+1];
static char  	phylip_outname[FILENAMELEN+1],nbrf_outname[FILENAMELEN+1];
static char  	gde_outname[FILENAMELEN+1];

static char  	profile1_name[FILENAMELEN+1];
static char  	profile2_name[FILENAMELEN+1];

static int numparams;
static char *params[MAXARGS];
static char *param_arg[MAXARGS];

static char *cmd_line_type[] = {
                " ",
                "=n ",
                "=f ",
                "=string ",
                "=filename ",
                ""};

static char *matrix_txt[] = {
                "BLOSUM series",
                "PAM series",
                "Identity matrix",
                "user defined"   };

static char *pw_matrix_txt[] = {
                "BLOSUM30",
                "PAM350",
                "Identity matrix",
                "user defined"   };

int 	profile1_nseqs;	       /* have been filled; the no. of seqs in prof 1*/
Boolean empty;
Boolean profile1_empty, profile2_empty;   /* whether or not profiles   */

char *lin1, *lin2, *lin3;

void init_amenu(void)
{
	empty=TRUE;
     
	profile1_empty = TRUE;     /*  */
	profile2_empty = TRUE;     /*  */

	lin1 = (char *)ckalloc( (MAXLINE+1) * sizeof (char) );
	lin2 = (char *)ckalloc( (MAXLINE+1) * sizeof (char) );
	lin3 = (char *)ckalloc( (MAXLINE+1) * sizeof (char) );
}




static int check_param(char *inline1,char *params[], char *param_arg[])
{

        char    *inptr, *cc;
#ifndef MAC
        char *strtok(char *s1, const char *s2);
#endif
        int     temp,len,i,j,n,match;
	int 	name1 = FALSE;
	
	if (inline1[0] != '/') name1 = TRUE;

        inptr=inline1;
        for (i=0;i<MAXARGS;i++) {
                if ((params[i]=strtok(inptr,"/"))==NULL)
                        break;
                inptr=NULL;
        }
        if (i==MAXARGS) {
		fprintf(stderr,"Error: too many command line arguments\n");
 		return(-1);
	}

	n = i;
        for (i=0;i<n;i++) {
                param_arg[i] = NULL;
		len = strlen(params[i]);
		for(j=0; j<len; j++)
			if(params[i][j] == '=') {
				param_arg[i] = (char *)ckalloc((len-j) * sizeof(char));
				strncpy(param_arg[i],&params[i][j+1],len-j-1);
				params[i][j] = EOS;
				param_arg[i][len-j-1] = EOS;
				break;
			}
			
		/* dgg - lower param for strcmp */
		/* for (cc= params[i]; *cc; cc++) *cc= tolower(*cc); */
/*
    search the parameter list for a match 
*/
		if ((i==0) && (name1 == TRUE)) continue;
		j = 0;
		match = -1;
		for(;;) {
			if (cmd_line[j].str[0] == '\0') break;
			
			if (!strcmp(params[i],cmd_line[j].str)) {  
				match = j;
				*cmd_line[match].flag = i;
				if ((cmd_line[match].type != NOARG) &&
                                    (param_arg[i] == NULL)) {
					fprintf(stderr,
                       				 "Error: parameter required for /%s\n",params[i]);
					exit(1);
				}
				break;
			}
			j++;
		}
		if (match == -1) {
			fprintf(stderr,
                        "Error: unknown option /%s\n",params[i]);
			exit(1);
		}
	}

        return(n);
}


 
#ifdef UNIX
FILE *open_path(char *fname)  /* to open in read-only file fname searching for 
				 it through all path directories */
{
#define Mxdir 70
        char dir[Mxdir+1], *path, *deb, *fin;
        FILE *fich;
        int lf, ltot;
 
        path=getenv("PATH"); 	/* get the list of path directories, 
					separated by :
    				*/
        if (path == NULL ) return fopen(fname,"r");
        lf=strlen(fname);
        deb=path;
        do
                {
                fin=strchr(deb,':');
                if(fin!=NULL)
                        { strncpy(dir,deb,fin-deb); ltot=fin-deb; }
                else
                        { strcpy(dir,deb); ltot=strlen(dir); }
                /* now one directory is in string dir */
                if( ltot + lf + 1 <= Mxdir)
                        {
                        dir[ltot]='/';
                        strcpy(dir+ltot+1,fname); /* now dir is appended with fi
   lename */
                        if( (fich = fopen(dir,"r") ) != NULL) break;
                        }
                else fich = NULL;
                deb=fin+1;
                }
        while (fin != NULL);
        return fich;
}
#endif


static void get_help(char help_pointer)    /* Help procedure */
{	
	FILE *help_file;
	int  i, number, nlines;
	Boolean found_help;
	char temp[MAXLINE+1];
	char token;
	char *digits = "0123456789ABCD";
	char *help_marker    = ">>HELP";

#ifdef MSDOS
	char *help_file_name = "clustalw.hlp";
#else
	char *help_file_name = "clustalw_help";
#endif


#ifdef VMS
        if((help_file=fopen(help_file_name,"r","rat=cr","rfm=var"))==NULL) {
            error("Cannot open help file [%s]",help_file_name);
            return;
        }
#else

#ifdef UNIX
        if((help_file=open_path(help_file_name))==NULL) {
             if((help_file=fopen(help_file_name,"r"))==NULL) {
                  error("Cannot open help file [%s]",help_file_name);
                  return;
             }
        }
#else
        if((help_file=fopen(help_file_name,"r"))==NULL) {
            error("Cannot open help file [%s]",help_file_name);
            return;
        }
#endif

#endif
/*		error("Cannot open help file [%s]",help_file_name);
		return;
	}
*/
	nlines = 0;
	number = -1;
	found_help = FALSE;

	while(TRUE) {
		if(fgets(temp,MAXLINE+1,help_file) == NULL) {
			if(!found_help)
				error("No help found in help file");
			fclose(help_file);
			return;
		}
		if(strstr(temp,help_marker)) {
                        token = ' ';
			for(i=0; i<8; i++)
				if(strchr(digits, temp[i])) {
					token = temp[i];
					break;
				}
		}
		if(token == help_pointer) {
			found_help = TRUE;
			while(fgets(temp,MAXLINE+1,help_file)) {
				if(strstr(temp, help_marker)){
				  	if(usemenu) {
						fprintf(stdout,"\n");
				    		getstr("Press [RETURN] to continue",lin2);
				  	}
					fclose(help_file);
					return;
				}
			       fputs(temp,stdout);
			       ++nlines;
			       if(usemenu) {
			          if(nlines >= PAGE_LEN) {
				     	   fprintf(stdout,"\n");
			 	  	   getstr("Press [RETURN] to continue or  X  to stop",lin2);
				  	   if(toupper(*lin2) == 'X') {
						   fclose(help_file);
						   return;
				  	   }
				  	   else
						   nlines = 0;
				   }
			       }
			}
			if(usemenu) {
				fprintf(stdout,"\n");
				getstr("Press [RETURN] to continue",lin2);
			}
			fclose(help_file);
		}
	}
}

static void show_aln(void)         /* Alignment screen display procedure */
{
        FILE *file;
        int  i, nlines;
        char temp[MAXLINE+1];
        char file_name[FILENAMELEN+1];

        if(output_clustal) strcpy(file_name,clustal_outname);
        else if(output_nbrf) strcpy(file_name,nbrf_outname);
        else if(output_gcg) strcpy(file_name,gcg_outname);
        else if(output_phylip) strcpy(file_name,phylip_outname);
        else if(output_gde) strcpy(file_name,gde_outname);

#ifdef VMS
    if((file=fopen(file_name,"r","rat=cr","rfm=var"))==NULL) {
#else
    if((file=fopen(file_name,"r"))==NULL) {
#endif
                error("Cannot open file [%s]",file_name);
                return;
        }

        fprintf(stdout,"\n\n");
        nlines = 0;

        while(fgets(temp,MAXLINE+1,file)) {
                fputs(temp,stdout);
                ++nlines;
                if(nlines >= PAGE_LEN) {
                        fprintf(stdout,"\n");
                        getstr("Press [RETURN] to continue or  X  to stop",lin2);
                        if(toupper(*lin2) == 'X') {
                                fclose(file);
                                return;
                        }
                        else
                                nlines = 0;
                }
        }
        fclose(file);
        fprintf(stdout,"\n");
        getstr("Press [RETURN] to continue",lin2);
}


void parse_params()
{
	char path[FILENAMELEN+1];
	int i,j,k,c,len,lenp,temp;
	float ftemp;

	Boolean do_align, do_align_only, do_tree_only, do_tree, do_boot, do_profile, do_something;
	fprintf(stdout,"\n\n\n");
	fprintf(stdout," CLUSTAL W(%2.1f) Multiple Sequence Alignments\n\n\n", revision_level);

	do_align = do_align_only = do_tree_only = do_tree = do_boot = do_profile = do_something = FALSE;

	*seqname=EOS;

	len=strlen(paramstr);
	for(i=0;i<len;++i) paramstr[i]=tolower(paramstr[i]); /* dgg - tolower is bad for Unix where filename case is important ! */

        numparams = check_param(paramstr, params, param_arg);
	if (numparams <0) exit(1);

	if(sethelp != -1) {
		get_help('9');
		exit(1);
	}

	if(setoptions != -1) {
		fprintf(stderr,"clustalw option list:-\n");
		for (i=0;cmd_line[i].str[0] != '\0';i++) {
			fprintf(stderr,"\t\t/%s%s",cmd_line[i].str,cmd_line_type[cmd_line[i].type]);
			if (cmd_line[i].type == OPTARG) {
				if (cmd_line[i].arg[0][0] != '\0')
					fprintf(stderr,"=%s",cmd_line[i].arg[0]);
				for (j=1;cmd_line[i].arg[j][0] != '\0';j++)
					fprintf(stderr," OR %s",cmd_line[i].arg[j]);
			}
			fprintf(stderr,"\n");
		}
		exit(1);
	}


/*****************************************************************************/
/*  Check to see if sequence type is explicitely stated..override ************/
/* the automatic checking (DNA or Protein).   /type=d or /type=p *************/
/*****************************************************************************/
	if(settype != -1)
		if(strlen(param_arg[settype])>0) {
			temp = find_match(param_arg[settype],type_arg,2);
			if(temp == 0) {
				dnaflag = FALSE;
				explicit_dnaflag = TRUE;
				fprintf(stdout,
				"\nSequence type explicitly set to Protein\n");
			}
			else if(temp == 1) {
				fprintf(stdout,
				"\nSequence type explicitly set to DNA\n");
				dnaflag = TRUE;
				explicit_dnaflag = TRUE;
			}
			else
				fprintf(stdout,"\nUnknown sequence type %s\n",
				param_arg[settype]);
		}


/***************************************************************************
*   check to see if 1st parameter does not start with '/' i.e. look for an *
*   input file as first parameter.   The input file can also be specified  *
*   by /infile=fname.                                                      *
****************************************************************************/

	if(paramstr[0] != '/') {
		strcpy(seqname, params[0]);
	}

/**************************************************/
/*  Look for /infile=file.ext on the command line */
/**************************************************/

	if(setinfile != -1) {
		if(strlen(param_arg[setinfile]) <= 0) {
			error("Bad sequence file name");
			exit(1);
		}
		strcpy(seqname, param_arg[setinfile]);
	}

	if(*seqname != EOS) {
		nseqs = readseqs(1);
		if(nseqs < 2) {
                	fprintf(stderr,
			"\nNo. of seqs. read = %d. No alignment!\n",(pint)nseqs);
			exit(1);
		}
		for(i = 1; i<=nseqs; i++) 
			fprintf(stdout,"Sequence %d: %-*s   %6.d %s\n",
			(pint)i,MAXNAMES,names[i],(pint)seqlen_array[i],dnaflag?"bp":"aa");
		empty = FALSE;
                first_seq = 1;
                last_seq = nseqs;
		do_something = TRUE;
	}

/*********************************************************/
/* Look for /profile1=file.ext  AND  /profile2=file2.ext */
/* You must give both file names OR neither.             */
/*********************************************************/

	if(setprofile1 != -1) {
		if(strlen(param_arg[setprofile1]) <= 0) {
			error("Bad profile 1 file name");
			exit(1);
		}
		strcpy(seqname, param_arg[setprofile1]);
		profile_input(1);
		if(nseqs <= 0) 
			exit(1);
	}

	if(setprofile2 != -1) {
		if(strlen(param_arg[setprofile2]) <= 0) {
			error("Bad profile 2 file name");
			exit(1);
		}
		if(profile1_empty) {
			error("Only 1 profile file (profile 2) specified.");
			exit(1);
		}
		strcpy(seqname, param_arg[setprofile2]);
		profile_input(2);
		if(nseqs > profile1_nseqs) 
			do_something = do_profile = TRUE;
		else {
			error("No sequences read from profile 2");
			exit(1);
		}
	}

/*************************************************************************/
/* Look for /tree or /bootstrap or /align or /usetree ******************/
/*************************************************************************/

	if (setinteractive != -1) {
		settree = -1;
		setbootstrap = -1;
		setalign = -1;
		setusetree = -1;
		setnewtree = -1;
	}

	if(settree != -1 )
		if(empty) {
			error("Cannot draw tree.  No input alignment file");
			exit(1);
		}
		else 
			do_tree = TRUE;

	if(setbootstrap != -1)
		if(empty) {
			error("Cannot bootstrap tree. No input alignment file");
			exit(1);
		}
		else {
			temp = 0;
			if(strlen(param_arg[setbootstrap]) > 0)
				 sscanf(param_arg[setbootstrap],"%d",&temp);
			if(temp > 0)          boot_ntrials = temp;
			do_boot = TRUE;
		}

	if(setalign != -1)
		if(empty) {
			error("Cannot align sequences.  No input file");
			exit(1);
		}
		else 
			do_align = TRUE;

	if(setusetree != -1)
		if(empty) {
			error("Cannot align sequences.  No input file");
			exit(1);
		}
		else  {
		        if(strlen(param_arg[setusetree]) == 0) {
				error("Cannot align sequences.  No tree file specified");
				exit(1);
		        }
                        else {
			        strcpy(phylip_phy_tree_name, param_arg[setusetree]);
		        }
		        do_align_only = TRUE;
		}

	if(setnewtree != -1)
		if(empty) {
			error("Cannot align sequences.  No input file");
			exit(1);
		}
		else  {
		        if(strlen(param_arg[setnewtree]) == 0) {
				error("Cannot align sequences.  No tree file specified");
				exit(1);
		        }
                        else {
			        strcpy(phylip_phy_tree_name, param_arg[setnewtree]);
				explicit_treefile = TRUE;
		        }
			do_tree_only = TRUE;
		}

	if( (!do_tree) && (!do_boot) && (!empty) && (!do_profile) && (!do_align_only) && (!do_tree_only)) 
		do_align = TRUE;

	if(!do_something && (setinteractive == -1)) {
		error("No input file(s) specified");
		exit(1);
	}

/*** ? /quicktree  */
        if(setquicktree != -1)
		quick_pairalign = TRUE;

	
	if(dnaflag) {
		gap_open   = dna_gap_open;
		gap_extend = dna_gap_extend;
		pw_go_penalty  = dna_pw_go_penalty;
		pw_ge_penalty  = dna_pw_ge_penalty;
                ktup       = dna_ktup;
                window     = dna_window;
                signif     = dna_signif;
                wind_gap   = dna_wind_gap;

	}
	else {
		gap_open   = prot_gap_open;
		gap_extend = prot_gap_extend;
		pw_go_penalty  = prot_pw_go_penalty;
		pw_ge_penalty  = prot_pw_ge_penalty;
                ktup       = prot_ktup;
                window     = prot_window;
                signif     = prot_signif;
                wind_gap   = prot_wind_gap;
	}


/****************************************************************************/
/* look for parameters on command line  e.g. gap penalties, k-tuple etc.    */
/****************************************************************************/

/*** ? /score=percent or /score=absolute */
        if(setscore != -1)
                if(strlen(param_arg[setscore]) > 0) {
			temp = find_match(param_arg[setscore],score_arg,2);
                        if(temp == 0)
                                percent = TRUE;
                        else if(temp == 1)
                                percent = FALSE;
                        else
                                fprintf(stdout,"\nUnknown SCORE type: %s\n",
                                param_arg[setscore]);
                }

/*** ? /seed=n */
        if(setseed != -1) {
                temp = 0;
                if(strlen(param_arg[setseed]) > 0)
                         sscanf(param_arg[setseed],"%d",&temp);
                if(temp > 0) boot_ran_seed = temp;
        fprintf(stdout,"\ntemp = %d; seed = %u;\n",(pint)temp,boot_ran_seed);
        }




/*** ? /output=PIR, GCG, GDE or PHYLIP  Only 1 can be switched on at a time */
	if(setoutput != -1)
		if(strlen(param_arg[setoutput]) > 0) {
			temp = find_match(param_arg[setoutput],output_arg,4);
			switch (temp) {
				case 0: /* GCG */
					output_gcg     = TRUE;
					output_clustal = FALSE;
					break;
				case 1: /* GDE */
					output_gde     = TRUE;
					output_clustal = FALSE;
					break;
				case 2: /* PIR */
					output_nbrf    = TRUE;
					output_clustal = FALSE;
					break;
				case 3: /* PHYLIP */
					output_phylip  = TRUE;
					output_clustal = FALSE;
					break;
				default:
					fprintf(stdout,"\nUnknown OUTPUT type: %s\n",
					param_arg[setoutput]);
			}
		}

/*** ? /outputtree=NJ or PHYLIP or DIST Only 1 can be switched on at a time */
	if(setoutputtree != -1)
		if(strlen(param_arg[setoutputtree]) > 0) {
			temp = find_match(param_arg[setoutputtree],outputtree_arg,3);
			switch (temp) {
				case 0: /* NJ */
					output_tree_clustal = TRUE;
					output_tree_phylip = FALSE;
					output_tree_distances = FALSE;
					break;
				case 1: /* PHYLIP */
					output_tree_phylip  = TRUE;
					output_tree_clustal = FALSE;
					output_tree_distances = FALSE;
					break;
				case 2: /* PHYLIP */
					output_tree_distances = TRUE;
					output_tree_phylip  = FALSE;
					output_tree_clustal = FALSE;
					break;
				default:
					fprintf(stdout,"\nUnknown OUTPUT TREE type: %s\n",
					param_arg[setoutputtree]);
			}
		}

/*** ? /ktuple=n */
        if(setktuple != -1) {
                temp = 0;
                if(strlen(param_arg[setktuple]) > 0)
			 sscanf(param_arg[setktuple],"%d",&temp);
                if(temp > 0) {
                        if(dnaflag) {
                                if(temp <= 4) {
                                        ktup         = temp;
                                        dna_ktup     = ktup;
                                        wind_gap     = ktup + 4;
                                        dna_wind_gap = wind_gap;
                                }
                        }
                        else {
                                if(temp <= 2) {
                                        ktup          = temp;
                                        prot_ktup     = ktup;
                                        wind_gap      = ktup + 3;
                                        prot_wind_gap = wind_gap;
                                }
                        }
                }
        }

/*** ? /pairgap=n */
        if(setpairgap != -1) {
                temp = 0;
                if(strlen(param_arg[setpairgap]) > 0)
			 sscanf(param_arg[setpairgap],"%d",&temp);
                if(temp > 0)
                        if(dnaflag) {
                                if(temp > ktup) {
                                        wind_gap     = temp;
                                        dna_wind_gap = wind_gap;
                                }
                        }
                        else {
                                if(temp > ktup) {
                                        wind_gap      = temp;
                                        prot_wind_gap = wind_gap;
                                }
                        }
        }


/*** ? /topdiags=n   */
        if(settopdiags != -1) {
                temp = 0;
                if(strlen(param_arg[settopdiags]) > 0)
			 sscanf(param_arg[settopdiags],"%d",&temp);
                if(temp > 0)
                        if(dnaflag) {
                                if(temp > ktup) {
                                        signif       = temp;
                                        dna_signif   = signif;
                                }
                        }
                        else {
                                if(temp > ktup) {
                                        signif        = temp;
                                        prot_signif   = signif;
                                }
                        }
        }


/*** ? /window=n  */
        if(setwindow != -1) {
                temp = 0;
                if(strlen(param_arg[setwindow]) > 0)
			 sscanf(param_arg[setwindow],"%d",&temp);
                if(temp > 0)
                        if(dnaflag) {
                                if(temp > ktup) {
                                        window       = temp;
                                        dna_window   = window;
                                }
                        }
                        else {
                                if(temp > ktup) {
                                        window        = temp;
                                        prot_window   = window;
                                }
                        }
        }

/*** ? /transitions */
	if(settransitions != -1)
		is_weight = FALSE;

/*** ? /kimura */
	if(setkimura != -1)
		kimura = TRUE;

/*** ? /tossgaps */
	if(settossgaps != -1)
		tossgaps = TRUE;


/*** ? /negative  */
	if(setnegative != -1)
		neg_matrix = TRUE;


/*** ? /pwmatrix=ID (user's file)  */
	if(setpwmatrix != -1)
		if(strlen(param_arg[setpwmatrix]) > 0) {
                        if (strcmp(param_arg[setpwmatrix],"blosum")==0) {
                                strcpy(pw_mtrxname, param_arg[setpwmatrix]);
                                pw_matnum = 1;
                        }
                        else if (strcmp(param_arg[setpwmatrix],"pam")==0) {
                                strcpy(pw_mtrxname, param_arg[setpwmatrix]);
                                pw_matnum = 2;
                        }
                        else if (strcmp(param_arg[setpwmatrix],"id")==0) {
                                strcpy(pw_mtrxname, param_arg[setpwmatrix]);
                                pw_matnum = 3;
                        }
			else {
                                if(user_mat(param_arg[setpwmatrix], pw_usermat, pw_aa_xref))
                                  {
                                     strcpy(pw_mtrxname,param_arg[setpwmatrix]);
                                     pw_matnum=4;
                                  }
				else exit(1);
			}

		}


/*** ? /matrix=ID (user's file)  */
	if(setmatrix != -1)
		if(strlen(param_arg[setmatrix]) > 0) {
                        if (strcmp(param_arg[setmatrix],"blosum")==0) {
                                strcpy(mtrxname, param_arg[setmatrix]);
                                matnum = 1;
                        }
                        else if (strcmp(param_arg[setmatrix],"pam")==0) {
                                strcpy(mtrxname, param_arg[setmatrix]);
                                matnum = 2;
                        }
                        else if (strcmp(param_arg[setmatrix],"id")==0) {
                                strcpy(mtrxname, param_arg[setmatrix]);
                                matnum = 3;
                        }
			else {
                                if(user_mat(param_arg[setmatrix], usermat, aa_xref))
                                  {
                                     strcpy(mtrxname,param_arg[setmatrix]);
                                     matnum=4;
                                  }
				else exit(1);
			}

		}

/*** ? /maxdiv= n */
	if(setmaxdiv != -1) {
		temp = 0;
		if(strlen(param_arg[setmaxdiv]) > 0)
			sscanf(param_arg[setmaxdiv],"%d",&temp);
		if (temp >= 0)
			divergence_cutoff = temp;
	}

/*** ? /gapdist= n */
	if(setgapdist != -1) {
		temp = 0;
		if(strlen(param_arg[setgapdist]) > 0)
			sscanf(param_arg[setgapdist],"%d",&temp);
		if (temp >= 0)
			gap_dist = temp;
	}

/*** ? /debug= n */
	if(setdebug != -1) {
		temp = 0;
		if(strlen(param_arg[setdebug]) > 0)
			sscanf(param_arg[setdebug],"%d",&temp);
		if (temp >= 0)
			debug = temp;
	}

/*** ? /outfile= (user's file)  */
	if(setoutfile != -1)
		if(strlen(param_arg[setoutfile]) > 0) {
			explicit_outfile = TRUE;
                        strcpy(outfile_name, param_arg[setoutfile]);
		}

/*** ? /case= lower/upper  */
	if(setcase != -1) 
		if(strlen(param_arg[setcase]) > 0) {
			temp = find_match(param_arg[setcase],case_arg,2);
			if(temp == 0) {
				lowercase = TRUE;
			}
			else if(temp = 1) {
				lowercase = FALSE;
			}
			else
				fprintf(stdout,"\nUnknown case %s\n",
				param_arg);
		}


/*** ? /gapopen=n  */
	if(setgapopen != -1) {
		ftemp = 0.0;
		if(strlen(param_arg[setgapopen]) > 0)
			sscanf(param_arg[setgapopen],"%f",&ftemp);
		if(ftemp >= 0.0)
			if(dnaflag) {
					gap_open     = ftemp;
					dna_gap_open = gap_open;
			}
			else {
					gap_open      = ftemp;
					prot_gap_open = gap_open;
			}
	}


/*** ? /gapext=n   */
	if(setgapext != -1) {
		ftemp = 0.0;
		if(strlen(param_arg[setgapext]) > 0)
			sscanf(param_arg[setgapext],"%f",&ftemp);
		if(ftemp >= 0)
			if(dnaflag) {
					gap_extend      = ftemp;
					dna_gap_extend  = gap_extend;
			}
			else {
					gap_extend      = ftemp;
					prot_gap_extend = gap_extend;
			}
	}

/*** ? /pwgapopen=n  */
	if(setpwgapopen != -1) {
		ftemp = 0.0;
		if(strlen(param_arg[setpwgapopen]) > 0)
			sscanf(param_arg[setpwgapopen],"%f",&ftemp);
		if(ftemp >= 0.0)
			if(dnaflag) {
					pw_go_penalty  = ftemp;
                                        dna_pw_go_penalty = pw_go_penalty;
			}
			else {
					pw_go_penalty  = ftemp;
                                        prot_pw_go_penalty = pw_go_penalty;
			}
	}


/*** ? /gapext=n   */
	if(setpwgapext != -1) {
		ftemp = 0.0;
		if(strlen(param_arg[setpwgapext]) > 0)
			sscanf(param_arg[setpwgapext],"%f",&ftemp);
		if(ftemp >= 0)
			if(dnaflag) {
					pw_ge_penalty  = ftemp;
                                        dna_pw_ge_penalty = pw_ge_penalty;
			}
			else {
					pw_ge_penalty  = ftemp;
                                        prot_pw_ge_penalty = pw_ge_penalty;
			}
	}



/*** ? /outorder=n  */
	if(setoutorder != -1) {
		if(strlen(param_arg[setoutorder]) > 0)
			temp = find_match(param_arg[setoutorder],outorder_arg,2);
			if(temp == 0)  {	
				output_order   = INPUT;
			}
			else if(temp == 1)  {	
				output_order   = ALIGNED;
			}
			else
				fprintf(stdout,"\nUnknown OUTPUT ORDER type %s\n",
				param_arg[setoutorder]);
	}

/*** ? /endgaps */
	if(setuseendgaps != -1)
		use_endgaps = FALSE;

/*** ? /nopgap  */
	if(setnopgap != -1)
		no_pref_penalties = TRUE;

/*** ? /nohgap  */
	if(setnohgap != -1)
		no_hyd_penalties = TRUE;

/*** ? /hgapresidues="string"  */
	if(sethgapres != -1)
		if(strlen(param_arg[sethgapres]) > 0) {
			for (i=0;i<strlen(hyd_residues) && i<26;i++) {
				c = param_arg[sethgapres][i];
				if (isalpha(c))
					hyd_residues[i] = (char)toupper(c);
				else
					break;
			}
		}

/****************************************************************************/
/* Now do whatever has been requested ***************************************/
/****************************************************************************/

	if(setinteractive != -1) {
		usemenu = TRUE;
		return;
	}

	if(do_profile)
		profile_align();

	else if(do_align)
		align();

        else if (do_align_only)
                get_tree();

	else if(do_tree_only)
		make_tree();

	else if(do_tree)
		phylogenetic_tree();

	else if(do_boot)
		bootstrap_tree();

	exit(1);

/*******whew!***now*go*home****/
}







void main_menu(void)
{
        void jumper();
        int catchint;

        catchint = signal(SIGINT, SIG_IGN) != SIG_IGN;
        if (catchint) {
                if (setjmp(jmpbuf) != 0)
                        fprintf(stderr,"\n.. Interrupt\n");
#ifdef UNIX
                if (signal(SIGINT,jumper) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#else
                if (signal(SIGINT,SIG_DFL) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#endif
        }

	while(TRUE) {
		fprintf(stdout,"\n\n\n");
		fprintf(stdout," **************************************************************\n");
		fprintf(stdout," ******** CLUSTAL W(%2.1f) Multiple Sequence Alignments  ********\n",revision_level);
		fprintf(stdout," **************************************************************\n");
		fprintf(stdout,"\n\n");
		
		fprintf(stdout,"     1. Sequence Input From Disc\n");
		fprintf(stdout,"     2. Multiple Alignments\n");
		fprintf(stdout,"     3. Profile Alignments\n");
		fprintf(stdout,"     4. Phylogenetic trees\n");
		fprintf(stdout,"\n");
		fprintf(stdout,"     S. Execute a system command\n");
		fprintf(stdout,"     H. HELP\n");
		fprintf(stdout,"     X. EXIT (leave program)\n\n\n");
		
		getstr("Your choice",lin1);

		switch(toupper(*lin1)) {
			case '1': seq_input();
				break;
			case '2': multiple_align_menu();
				break;
			case '3': profile_align_menu();
				break;
			case '4': phylogenetic_tree_menu();
				break;
			case 'S': do_system();
				break;
			case '?':
			case 'H': get_help('1');
				break;
			case 'Q':
			case 'X': exit(0);
				break;
			default: fprintf(stderr,"\n\nUnrecognised Command\n\n");
				break;
		}
	}
}









static void multiple_align_menu(void)
{
        void jumper();
        int catchint;

        catchint = signal(SIGINT, SIG_IGN) != SIG_IGN;
        if (catchint) {
                if (setjmp(jmpbuf) != 0)
                        fprintf(stderr,"\n.. Interrupt\n");
#ifdef UNIX
                if (signal(SIGINT,jumper) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#else
                if (signal(SIGINT,SIG_DFL) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#endif
        }


    while(TRUE)
    {
        fprintf(stdout,"\n\n\n");
        fprintf(stdout,"****** MULTIPLE ALIGNMENT MENU ******\n");
        fprintf(stdout,"\n\n");


        fprintf(stdout,"    1.  Do complete multiple alignment now (%s)\n",
                        (!quick_pairalign) ? "Slow/Accurate" : "Fast/Approximate");
        fprintf(stdout,"    2.  Produce guide tree file only\n");
        fprintf(stdout,"    3.  Do alignment using old guide tree file\n\n");
        fprintf(stdout,"    4.  Toggle Slow/Fast pairwise alignments = %s\n\n",
                                        (!quick_pairalign) ? "SLOW" : "FAST");
        fprintf(stdout,"    5.  Pairwise alignment parameters\n");
        fprintf(stdout,"    6.  Multiple alignment parameters\n\n");
	fprintf(stdout,"    7.  Reset gaps between alignments?");
	if(reset_alignments)
		fprintf(stdout," = ON\n");
	else
		fprintf(stdout," = OFF\n");
        fprintf(stdout,"    8.  Toggle screen display          = %s\n",
                                        (!showaln) ? "OFF" : "ON");
        fprintf(stdout,"    9.  Output format options\n");
        fprintf(stdout,"\n");

        fprintf(stdout,"    S.  Execute a system command\n");
        fprintf(stdout,"    H.  HELP\n");
        fprintf(stdout,"    or press [RETURN] to go back to main menu\n\n\n");

        getstr("Your choice",lin1);
        if(*lin1 == EOS) return;

        switch(toupper(*lin1))
        {
        case '1': align();
            break;
        case '2': make_tree();
            break;
        case '3': get_tree();
            break;
        case '4': quick_pairalign ^= TRUE;
            break;
        case '5': pair_menu();
            break;
        case '6': multi_menu();
            break;
	case '7': reset_alignments ^= TRUE;
            break;
        case '8': showaln ^= TRUE;
	    break;
        case '9': format_options_menu();
            break;
        case 'S': do_system();
            break;
        case '?':
        case 'H': get_help('2');
            break;
        case 'Q':
        case 'X': return;

        default: fprintf(stderr,"\n\nUnrecognised Command\n\n");
            break;
        }
    }
}









static void profile_align_menu(void)
{
        void jumper();
        int catchint;

        catchint = signal(SIGINT, SIG_IGN) != SIG_IGN;
        if (catchint) {
                if (setjmp(jmpbuf) != 0)
                        fprintf(stderr,"\n.. Interrupt\n");
#ifdef UNIX
                if (signal(SIGINT,jumper) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#else
                if (signal(SIGINT,SIG_DFL) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#endif
        }


    while(TRUE)
    {
	fprintf(stdout,"\n\n\n");
        fprintf(stdout,"****** PROFILE ALIGNMENT MENU ******\n");
        fprintf(stdout,"\n\n");

        fprintf(stdout,"    1.  Input 1st. profile\n");
        fprintf(stdout,"    2.  Input 2nd. profile/sequences\n\n");
        fprintf(stdout,"    3.  Align 2nd. profile to 1st. profile\n");
        fprintf(stdout,"    4.  Align sequences to 1st. profile (%s)\n\n",
                        (!quick_pairalign) ? "Slow/Accurate" : "Fast/Approximate");
        fprintf(stdout,"    5.  Toggle Slow/Fast pairwise alignments = %s\n\n",
                                        (!quick_pairalign) ? "SLOW" : "FAST");
        fprintf(stdout,"    6.  Pairwise alignment parameters\n");
        fprintf(stdout,"    7.  Multiple alignment parameters\n\n");
        fprintf(stdout,"    8.  Toggle screen display                = %s\n",
                                        (!showaln) ? "OFF" : "ON");
        fprintf(stdout,"    9.  Output format options\n");
        fprintf(stdout,"\n");
        fprintf(stdout,"    S.  Execute a system command\n");
        fprintf(stdout,"    H.  HELP\n");
        fprintf(stdout,"    or press [RETURN] to go back to main menu\n\n\n");

        getstr("Your choice",lin1);
        if(*lin1 == EOS) return;

        switch(toupper(*lin1))
        {
        case '1': profile_input(1);      /* 1 => 1st profile */
		  strcpy(profile1_name, seqname);
            break;
        case '2': profile_input(2);      /* 2 => 2nd profile */
		  strcpy(profile2_name, seqname);
            break;
        case '3': profile_align();       /* align the 2 alignments now */
            break;
        case '4': new_sequence_align();  /* align new sequences to profile 1 */
            break;
        case '5': quick_pairalign ^= TRUE;
	    break;
        case '6': pair_menu();
            break;
        case '7': multi_menu();
            break;
        case '8': showaln ^= TRUE;
	    break;
        case '9': format_options_menu();
            break;
        case 'S': do_system();
            break;
        case '?':
        case 'H': get_help('6');
            break;
        case 'Q':
        case 'X': return;

        default: fprintf(stderr,"\n\nUnrecognised Command\n\n");
            break;
        }
    }
}











static void phylogenetic_tree_menu(void)
{
        void jumper();
        int catchint;

        catchint = signal(SIGINT, SIG_IGN) != SIG_IGN;
        if (catchint) {
                if (setjmp(jmpbuf) != 0)
                        fprintf(stderr,"\n.. Interrupt\n");
#ifdef UNIX
                if (signal(SIGINT,jumper) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#else
                if (signal(SIGINT,SIG_DFL) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#endif
        }


    while(TRUE)
    {
        fprintf(stdout,"\n\n\n");
        fprintf(stdout,"****** PHYLOGENETIC TREE MENU ******\n");
        fprintf(stdout,"\n\n");

        fprintf(stdout,"    1.  Input an alignment\n");
        fprintf(stdout,"    2.  Exclude positions with gaps?        ");
	if(tossgaps)
		fprintf(stdout,"= ON\n");
	else
		fprintf(stdout,"= OFF\n");
        fprintf(stdout,"    3.  Correct for multiple substitutions? ");
	if(kimura)
		fprintf(stdout,"= ON\n");
	else
		fprintf(stdout,"= OFF\n");
        fprintf(stdout,"    4.  Draw tree now\n");
        fprintf(stdout,"    5.  Bootstrap tree\n");
	fprintf(stdout,"    6.  Output format options\n");
        fprintf(stdout,"\n");
        fprintf(stdout,"    S.  Execute a system command\n");
        fprintf(stdout,"    H.  HELP\n");
        fprintf(stdout,"    or press [RETURN] to go back to main menu\n\n\n");

        getstr("Your choice",lin1);
        if(*lin1 == EOS) return;

        switch(toupper(*lin1))
        {
       	 	case '1': seq_input();
         	   	break;
        	case '2': tossgaps ^= TRUE;
          	  	break;
      		case '3': kimura ^= TRUE;;
            		break;
        	case '4': phylogenetic_tree();
            		break;
        	case '5': bootstrap_tree();
            		break;
		case '6': tree_format_options_menu();
			break;
        	case 'S': do_system();
            		break;
            	case '?':
        	case 'H': get_help('7');
            		break;
            	case 'Q':
        	case 'X': return;

        	default: fprintf(stderr,"\n\nUnrecognised Command\n\n");
            	break;
        }
    }
}






static void tree_format_options_menu(void)      /* format of tree output */
{	
        void jumper();
        int catchint;
	char path[FILENAMELEN+1];

        catchint = signal(SIGINT, SIG_IGN) != SIG_IGN;
        if (catchint) {
                if (setjmp(jmpbuf) != 0)
                        fprintf(stderr,"\n.. Interrupt\n");
#ifdef UNIX
                if (signal(SIGINT,jumper) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#else
                if (signal(SIGINT,SIG_DFL) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#endif
        }


	while(TRUE) {
	fprintf(stdout,"\n\n\n");
	fprintf(stdout," ****** Format of Phylogenetic Tree Output ******\n");
	fprintf(stdout,"\n\n");
	fprintf(stdout,"     1. Toggle CLUSTAL format tree output    =  %s\n",
					(!output_tree_clustal)  ? "OFF" : "ON");
	fprintf(stdout,"     2. Toggle Phylip format tree output     =  %s\n",
					(!output_tree_phylip)   ? "OFF" : "ON");
	fprintf(stdout,"     3. Toggle Phylip distance matrix output =  %s\n\n",
					(!output_tree_distances)? "OFF" : "ON");
	fprintf(stdout,"\n");
	fprintf(stdout,"     H. HELP\n\n\n");	
	
		getstr("Enter number (or [RETURN] to exit)",lin2);
		if(*lin2 == EOS) return;
		
		switch(toupper(*lin2)) {
			case '1':
				output_tree_clustal   ^= TRUE;
				break;
			case '2':
              			output_tree_phylip    ^= TRUE;
			  	break;
			case '3':
              			output_tree_distances ^= TRUE;
			  	break;
			case '?':
			case 'H':
				get_help('0');
				break;
			default:
				fprintf(stderr,"\n\nUnrecognised Command\n\n");
				break;
		}
	}
}


static void format_options_menu(void)      /* format of alignment output */
{	
	int i,length = 0;
	char path[FILENAMELEN+1];
        void jumper();
        int catchint;

        catchint = signal(SIGINT, SIG_IGN) != SIG_IGN;
        if (catchint) {
                if (setjmp(jmpbuf) != 0)
                        fprintf(stderr,"\n.. Interrupt\n");
#ifdef UNIX
                if (signal(SIGINT,jumper) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#else
                if (signal(SIGINT,SIG_DFL) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#endif
        }


	while(TRUE) {
	fprintf(stdout,"\n\n\n");
	fprintf(stdout," ********* Format of Alignment Output *********\n");
	fprintf(stdout,"\n\n");
	fprintf(stdout,"     1. Toggle CLUSTAL format output   =  %s\n",
					(!output_clustal) ? "OFF" : "ON");
	fprintf(stdout,"     2. Toggle NBRF/PIR format output  =  %s\n",
					(!output_nbrf) ? "OFF" : "ON");
	fprintf(stdout,"     3. Toggle GCG/MSF format output   =  %s\n",
					(!output_gcg) ? "OFF" : "ON");
	fprintf(stdout,"     4. Toggle PHYLIP format output    =  %s\n",
					(!output_phylip) ? "OFF" : "ON");
	fprintf(stdout,"     5. Toggle GDE format output       =  %s\n\n",
					(!output_gde) ? "OFF" : "ON");
	fprintf(stdout,"     6. Toggle GDE output case         =  %s\n",
					(!lowercase) ? "UPPER" : "LOWER");
	fprintf(stdout,"     7. Toggle output order            =  %s\n\n",
					(output_order==0) ? "INPUT FILE" : "ALIGNED");
	fprintf(stdout,"     8. Create alignment output file(s) now?\n\n");
        fprintf(stdout,"     9. Toggle parameter output        = %s\n",
                                        (!save_parameters) ? "OFF" : "ON");
	fprintf(stdout,"\n");
	fprintf(stdout,"     H. HELP\n\n\n");	
	
		getstr("Enter number (or [RETURN] to exit)",lin2);
		if(*lin2 == EOS) return;
		
		switch(toupper(*lin2)) {
			case '1':
				output_clustal ^= TRUE;
				break;
			case '2':
              			output_nbrf ^= TRUE;
			  	break;
			case '3':
              			output_gcg ^= TRUE;
			  	break;
			case '4':
              			output_phylip ^= TRUE;
			  	break;
			case '5':
              			output_gde ^= TRUE;
			  	break;
			case '6':
              			lowercase ^= TRUE;
			  	break;
			case '7':
                                if (output_order == INPUT) output_order = ALIGNED;
              			else output_order = INPUT;
			  	break;
			case '8':		/* DES */
				if(empty) break;
				get_path(seqname,path);
				if(!open_alignment_output(path)) break;
				for (i=1;i<=nseqs;i++)
					if (length < seqlen_array[i]) 
						length = seqlen_array[i];
/* DES DEBUG
	fprintf(stdout,"\n\nLength = %d",(pint)length);
*/
				create_alignment_output(length);
				break;
        		case '9': save_parameters ^= TRUE;
	   			 break;
			case '?':
			case 'H':
				get_help('5');
				break;
			default:
				fprintf(stderr,"\n\nUnrecognised Command\n\n");
				break;
		}
	}
}












static void pair_menu(void)
{
        void jumper();
        int catchint;

        catchint = signal(SIGINT, SIG_IGN) != SIG_IGN;
        if (catchint) {
                if (setjmp(jmpbuf) != 0)
                        fprintf(stderr,"\n.. Interrupt\n");
#ifdef UNIX
                if (signal(SIGINT,jumper) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#else
                if (signal(SIGINT,SIG_DFL) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#endif
        }


        if(dnaflag) {
                pw_go_penalty     = dna_pw_go_penalty;
                pw_ge_penalty     = dna_pw_ge_penalty;
                ktup       = dna_ktup;
                window     = dna_window;
                signif     = dna_signif;
                wind_gap   = dna_wind_gap;

        }
        else {
                pw_go_penalty     = prot_pw_go_penalty;
                pw_ge_penalty     = prot_pw_ge_penalty;
                ktup       = prot_ktup;
                window     = prot_window;
                signif     = prot_signif;
                wind_gap   = prot_wind_gap;

        }

	while(TRUE) {
	
		fprintf(stdout,"\n\n\n");
		fprintf(stdout," ********* PAIRWISE ALIGNMENT PARAMETERS *********\n");
		fprintf(stdout,"\n\n");

		fprintf(stdout,"     Slow/Accurate alignments:\n\n");

		fprintf(stdout,"     1. Gap Open Penalty       :%4.2f\n",pw_go_penalty);
		fprintf(stdout,"     2. Gap Extension Penalty  :%4.2f\n",pw_ge_penalty);
		fprintf(stdout,"     3. Protein weight matrix  :%s\n\n" ,
                                        pw_matrix_txt[pw_matnum-1]);

		fprintf(stdout,"     Fast/Approximate alignments:\n\n");

		fprintf(stdout,"     4. Gap penalty            :%d\n",(pint)wind_gap);
		fprintf(stdout,"     5. K-tuple (word) size    :%d\n",(pint)ktup);
		fprintf(stdout,"     6. No. of top diagonals   :%d\n",(pint)signif);
		fprintf(stdout,"     7. Window size            :%d\n\n",(pint)window);

                fprintf(stdout,"     8. Toggle Slow/Fast pairwise alignments ");
                if(quick_pairalign)
                      fprintf(stdout,"= FAST\n\n");
                else
                      fprintf(stdout,"= SLOW\n\n");


		fprintf(stdout,"     H. HELP\n\n\n");
		
		getstr("Enter number (or [RETURN] to exit)",lin2);
		if( *lin2 == EOS) {
                        if(dnaflag) {
                                dna_pw_go_penalty     = pw_go_penalty;
                                dna_pw_ge_penalty     = pw_ge_penalty;
                		dna_ktup       = ktup;
                		dna_window     = window;
                		dna_signif     = signif;
                		dna_wind_gap   = wind_gap;

                        }
                        else {
                                prot_pw_go_penalty     = pw_go_penalty;
                                prot_pw_ge_penalty     = pw_ge_penalty;
                		prot_ktup       = ktup;
                		prot_window     = window;
                		prot_signif     = signif;
                		prot_wind_gap   = wind_gap;

                        }
 
			return;
		}
		
		switch(toupper(*lin2)) {
			case '1':
				fprintf(stdout,"Gap Open Penalty Currently: %4.2f\n",pw_go_penalty);
				pw_go_penalty=(float)getreal("Enter number",(double)0.0,(double)100.0,(double)pw_go_penalty);
				break;
			case '2':
				fprintf(stdout,"Gap Extension Penalty Currently: %4.2f\n",pw_ge_penalty);
				pw_ge_penalty=(float)getreal("Enter number",(double)0.0,(double)10.0,(double)pw_ge_penalty);
				break;
                        case '3':
                                pw_matnum = read_matrix(pw_matrix_txt,pw_mtrxname,pw_matnum,pw_usermat,pw_aa_xref);
                                break;
			case '4':
                                fprintf(stdout,"Gap Penalty Currently: %d\n",(pint)wind_gap);
                                wind_gap=getint("Enter number",1,500,wind_gap);
				break;
			case '5':
                                fprintf(stdout,"K-tuple Currently: %d\n",(pint)ktup);
                                if(dnaflag)
                                     ktup=getint("Enter number",1,4,ktup);
                                else
                                     ktup=getint("Enter number",1,2,ktup);                                     
				break;
			case '6':
                                fprintf(stdout,"Top diagonals Currently: %d\n",(pint)signif);
                                signif=getint("Enter number",1,50,signif);
				break;
			case '7':
                                fprintf(stdout,"Window size Currently: %d\n",(pint)window);
                                window=getint("Enter number",1,50,window);
				break;
                        case '8': quick_pairalign ^= TRUE;
                                break;
			case '?':
			case 'H':
				get_help('3');
				break;
			default:
				fprintf(stderr,"\n\nUnrecognised Command\n\n");
				break;
		}
	}
}





static void multi_menu(void)
{
	char c;
	int i;
        void jumper();
        int catchint;

        catchint = signal(SIGINT, SIG_IGN) != SIG_IGN;
        if (catchint) {
                if (setjmp(jmpbuf) != 0)
                        fprintf(stderr,"\n.. Interrupt\n");
#ifdef UNIX
                if (signal(SIGINT,jumper) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#else
                if (signal(SIGINT,SIG_DFL) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#endif
        }


	if(dnaflag) {
		gap_open   = dna_gap_open;
		gap_extend = dna_gap_extend;
	}
	else {
		gap_open   = prot_gap_open;
		gap_extend = prot_gap_extend;
	}

	while(TRUE) {
                lin3 = is_weight ? "Weighted" :"Unweighted";

		fprintf(stdout,"\n\n\n");
		fprintf(stdout," ********* MULTIPLE ALIGNMENT PARAMETERS *********\n");
		fprintf(stdout,"\n\n");
		
		fprintf(stdout,"     1. Gap Opening Penalty              :%4.2f\n",gap_open);
		fprintf(stdout,"     2. Gap Extension Penalty            :%4.2f\n",gap_extend);

		fprintf(stdout,"     3. Delay divergent sequences        :%d %%\n\n",(pint)divergence_cutoff);

                fprintf(stdout,"     4. Toggle Transitions (DNA)	 :%s\n\n",lin3);
                fprintf(stdout,"     5. Protein weight matrix            :%s\n"
                                        ,matrix_txt[matnum-1]);
		fprintf(stdout,"     6. Use negative matrix              :%s\n",(!neg_matrix) ? "OFF" : "ON");
                fprintf(stdout,"     7. Protein Gap Parameters\n\n");
		fprintf(stdout,"     H. HELP\n\n\n");		

		getstr("Enter number (or [RETURN] to exit)",lin2);

		if(*lin2 == EOS) {
			if(dnaflag) {
				dna_gap_open    = gap_open;
				dna_gap_extend  = gap_extend;
			}
			else {
				prot_gap_open   = gap_open;
				prot_gap_extend = gap_extend;
			}
			return;
		}
		
		switch(toupper(*lin2)) {
			case '1':
			fprintf(stdout,"Gap Opening Penalty Currently: %4.2f\n",gap_open);
				gap_open=(float)getreal("Enter number",(double)0.0,(double)100.0,(double)gap_open);
				break;
			case '2':
				fprintf(stdout,"Gap Extension Penalty Currently: %4.2f\n",gap_extend);
				gap_extend=(float)getreal("Enter number",(double)0.0,(double)10.0,(double)gap_extend);
				break;
			case '3':
				fprintf(stdout,"Min Identity Currently: %d\n",(pint)divergence_cutoff);
				divergence_cutoff=getint("Enter number",0,100,divergence_cutoff);
				break;
                        case '4':
                                is_weight ^= TRUE;
                                break;
			case '5':
                                matnum = read_matrix(matrix_txt,mtrxname,matnum,usermat,aa_xref);
				break;
			case '6':
				neg_matrix ^= TRUE;
				break;
			case '7':
                                gap_penalties_menu();
				break;
			case '?':
			case 'H':
				get_help('4');
				break;
			default:
				fprintf(stderr,"\n\nUnrecognised Command\n\n");
				break;
		}
	}
}






static void gap_penalties_menu(void)
{
	char c;
	int i;
        void jumper();
        int catchint;

        catchint = signal(SIGINT, SIG_IGN) != SIG_IGN;
        if (catchint) {
                if (setjmp(jmpbuf) != 0)
                        fprintf(stderr,"\n.. Interrupt\n");
#ifdef UNIX
                if (signal(SIGINT,jumper) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#else
                if (signal(SIGINT,SIG_DFL) == BADSIG)
                        fprintf(stderr,"Error: signal\n");
#endif
        }


	while(TRUE) {

		fprintf(stdout,"\n\n\n");
		fprintf(stdout," ********* PROTEIN GAP PARAMETERS *********\n");
		fprintf(stdout,"\n\n\n");

		fprintf(stdout,"     1. Toggle Residue-Specific Penalties :%s\n\n",(no_pref_penalties) ? "OFF" : "ON");
		fprintf(stdout,"     2. Toggle Hydrophilic Penalties      :%s\n",(no_hyd_penalties) ? "OFF" : "ON");
		fprintf(stdout,"     3. Hydrophilic Residues              :%s\n\n"
					,hyd_residues);
		fprintf(stdout,"     4. Gap Separation Distance           :%d\n",(pint)gap_dist);
		fprintf(stdout,"     5. Toggle End Gap Separation         :%s\n\n",(!use_endgaps) ? "OFF" : "ON");
		fprintf(stdout,"     H. HELP\n\n\n");		

		getstr("Enter number (or [RETURN] to exit)",lin2);

		if(*lin2 == EOS) return;
		
		switch(toupper(*lin2)) {
			case '1':
				no_pref_penalties ^= TRUE;
				break;
			case '2':
				no_hyd_penalties ^= TRUE;
				break;
			case '3':
				fprintf(stdout,"Hydrophilic Residues Currently: %s\n",hyd_residues);

				getstr("Enter residues (or [RETURN] to quit)",lin1);
                                if (*lin1 != EOS) {
                                        for (i=0;i<strlen(hyd_residues) && i<26;i++) {
                                        c = lin1[i];
                                        if (isalpha(c))
                                                hyd_residues[i] = (char)toupper(c);
                                        else
                                                break;
                                        }
                                        hyd_residues[i] = EOS;
                                }
                                break;
			case '4':
				fprintf(stdout,"Gap Separation Distance Currently: %d\n",(pint)gap_dist);
				gap_dist=getint("Enter number",0,100,gap_dist);
				break;
			case '5':
				use_endgaps ^= TRUE;
				break;
			case '?':
			case 'H':
				get_help('A');
				break;
			default:
				fprintf(stderr,"\n\nUnrecognised Command\n\n");
				break;
		}
	}
}




static int read_matrix(char **mattxt, char *matnam, int matn, int *mat, int *xref)
{       static char userfile[FILENAMELEN+1];

        while(TRUE)
        {
                fprintf(stdout,"\n\n\n");
                fprintf(stdout," ********* PROTEIN WEIGHT MATRIX MENU *********\n");
                fprintf(stdout,"\n\n");


                fprintf(stdout,"     1. %s\n",mattxt[0]);
                fprintf(stdout,"     2. %s\n",mattxt[1]);
                fprintf(stdout,"     3. %s\n",mattxt[2]);
                fprintf(stdout,"     4. %s\n\n",mattxt[3]);
                fprintf(stdout,"     H. HELP\n\n");
                fprintf(stdout,
"     -- Current matrix is the %s ",mattxt[matn-1]);
                if(matn == 5) fprintf(stdout,"(file = %s)",userfile);
                fprintf(stdout,"--\n");


                getstr("\n\nEnter number (or [RETURN] to exit)",lin2);
                if(*lin2 == EOS) return(matn);

                switch(toupper(*lin2))  {
                        case '1':
                                strcpy(matnam,"blosum");
                                matn=1;
                                break;
                        case '2':
                                strcpy(matnam,"pam");
                                matn=2;
                                break;
                        case '3':
                                strcpy(matnam,"id");
                                matn=3;
                                break;
                        case '4':
                                if(user_mat(userfile, mat, xref))
                                  {
                                     strcpy(matnam,userfile);
                                     matn=4;
                                  }
                                break;
                        case '?':
                        case 'H':
                                get_help('8');
                                break;
                        default:
                                fprintf(stderr,"\n\nUnrecognised Command\n\n");
                                break;
                }
        }
}

static Boolean user_mat(char *str, int *mat, int *xref)
{
        int i,j,nv,pos,idx,val;
        int maxres;

        FILE *infile;

        if(usemenu)
                getstr("Enter name of the matrix file",lin2);
        else
                strcpy(lin2,str);

        if(*lin2 == EOS) return FALSE;

        if((infile=fopen(lin2,"r"))==NULL) {
                error("Cannot find matrix file [%s]",lin2);
                return FALSE;
        }

	strcpy(str, lin2);

	maxres = read_user_matrix(str, mat, xref);
        if (maxres <= 0) return FALSE;

	return TRUE;
}






static void seq_input(void)
{
	char c;
        int i;
	
	if(usemenu) {
fprintf(stdout,"\n\nSequences should all be in 1 file.\n"); 
fprintf(stdout,"\n6 formats accepted: \n");
fprintf(stdout,
"NBRF/PIR, EMBL/SwissProt, Pearson (Fasta), GDE, Clustal, GCG/MSF.\n\n\n");
/*fprintf(stdout,
"\nGCG users should use TOPIR to convert their sequence files before use.\n\n\n");*/
	}

	
       nseqs = readseqs(1);        /*  1 is the first seq to be read */
       if(nseqs < 0)               /* file could not be opened */
           { 
               nseqs = 0;
               empty = TRUE;
           }
       else if(nseqs == 0)         /* no sequences */
           {
	       error("No sequences in file!  Bad format?");
               empty = TRUE;
           }
       else if(nseqs == 1)
           {
               error("Only one sequence in file!");
               empty = TRUE;
               nseqs = 0;
           }
       else 
           {
		fprintf(stdout,"\nSequences assumed to be %s \n\n",
			dnaflag?"DNA":"PROTEIN");
                for(i=1; i<=nseqs; i++) {
/* DES                         fprintf(stdout,"%s: = ",names[i]); */
                        fprintf(stdout,"Sequence %d: %-*s   %6.d %s\n",
                        (pint)i,MAXNAMES,names[i],(pint)seqlen_array[i],dnaflag?"bp":"aa");
                }	
			if(dnaflag) {
				gap_open   = dna_gap_open;
				gap_extend = dna_gap_extend;
			}
			else {
				gap_open   = prot_gap_open;
				gap_extend = prot_gap_extend;
			}
			empty=FALSE;
                first_seq = 1;
                last_seq = nseqs;
	   }
	
}







static void profile_input(int profile_no)   /* read a profile   */
{                                           /* profile_no is 1 or 2  */
	char c;
        int local_nseqs, i;
	
        if(profile_no == 2 && profile1_empty) 
           {
             error("You must read in profile number 1 first");
             return;
           }


    if(profile_no == 1)     /* for the 1st profile */
      {
       local_nseqs = readseqs(1); /* (1) means 1st seq to be read = no. 1 */
       if(local_nseqs < 0)               /* file could not be opened */
	       return;
       else if(local_nseqs == 0)         /* no sequences  */
           {
	       error("No sequences in file!  Bad format?");
	       return;
           }
       else 
           { 				/* success; found some seqs. */
                nseqs = profile1_nseqs = local_nseqs;
		fprintf(stdout,"\nNo. of seqs=%d\n",(pint)nseqs);
		profile1_empty=FALSE;
                first_seq = 1;
                last_seq = nseqs;
		profile2_empty=TRUE;
	   }
      }
    else
      {			        /* first seq to be read = profile1_nseqs + 1 */
       local_nseqs = readseqs(profile1_nseqs+1); 
       if(local_nseqs < 0)               /* file could not be opened */
               profile2_empty = TRUE;
       else if(local_nseqs == 0)         /* no sequences */
           {
	       error("No sequences in file!  Bad format?");
               profile2_empty = TRUE;
           }
       else 
           {
		fprintf(stdout,"\nNo. of seqs in profile=%d\n",(pint)local_nseqs);
                nseqs = profile1_nseqs + local_nseqs;
                fprintf(stdout,"\nTotal no. of seqs     =%d\n",(pint)nseqs);
		profile2_empty=FALSE;
		empty = FALSE;
                first_seq = 1;
                last_seq = nseqs;
	   }

      }
	
	fprintf(stdout,"\nSequences assumed to be %s \n\n",
		dnaflag?"DNA":"PROTEIN");
        for(i=profile2_empty?1:profile1_nseqs+1; i<=nseqs; i++) {
                fprintf(stdout,"Sequence %d: %-*s   %6.d %s\n",
                   (pint)i,MAXNAMES,names[i],(pint)seqlen_array[i],dnaflag?"bp":"aa");
        }	
	if(dnaflag) {
		gap_open   = dna_gap_open;
		gap_extend = dna_gap_extend;
	}
	else {
		gap_open   = prot_gap_open;
		gap_extend = prot_gap_extend;
	}
}






FILE *  open_output_file(char *prompt,      char *path, 
				char *file_name,   char *file_extension)
 
{	static char temp[FILENAMELEN+1];
	static char local_prompt[MAXLINE];
	int useprompt = FALSE;
	FILE * file_handle;

/*	if (*file_name == EOS) {
*/		strcpy(file_name,path);
		strcat(file_name,file_extension);
/*	}
*/
	if(strcmp(file_name,seqname)==0) {
		strcpy(local_prompt,"\nError: Output file name is the same as input file.\n\nEnter new name to avoid overwriting ");
		useprompt = TRUE;
	}
	else 
		strcpy(local_prompt,prompt);
	strcat(local_prompt," [%s]: ");          

	if(usemenu || useprompt) {
		fprintf(stdout,local_prompt,file_name);
		gets(temp);
		if(*temp != EOS) strcpy(file_name,temp);
	}

#ifdef VMS
	if((file_handle=fopen(file_name,"w","rat=cr","rfm=var"))==NULL) {
#else
	if((file_handle=fopen(file_name,"w"))==NULL) {
#endif
		error("Cannot open output file [%s]",file_name);
		return NULL;
	}
	return file_handle;
}



FILE *  open_explicit_file(char *file_name)
{ 
	FILE * file_handle;

	if (*file_name == EOS) {
		error("Bad output file [%s]",file_name);
		return NULL;
	}
#ifdef VMS
	if((file_handle=fopen(file_name,"w","rat=cr","rfm=var"))==NULL) {
#else
	if((file_handle=fopen(file_name,"w"))==NULL) {
#endif
		error("Cannot open output file [%s]",file_name);
		return NULL;
	}
	return file_handle;
}









static void align(void)
{ 
	FILE *tree;
	char path[FILENAMELEN+1],temp[FILENAMELEN+1];
	int i,count,length = 0;
	
	if(empty && usemenu) {
		error("No sequences in memory. Load sequences first.");
		return;
	}

        get_path(seqname,path);
/* DES DEBUG 
	fprintf(stdout,"\n\n Seqname = %s  \n Path = %s \n\n",seqname,path);
*/
        if(!open_alignment_output(path)) return;

	if (nseqs > 3)
	if (explicit_treefile) {
		if((phylip_phy_tree_file = open_explicit_file(
		phylip_phy_tree_name))==NULL) return;
	}
	else {
        	if((phylip_phy_tree_file = open_output_file(
                "\nEnter name for GUIDE TREE          file  ",path,
                phylip_phy_tree_name,"dnd")) == NULL) return;
	}

	if (save_parameters) create_parameter_output();

	if(reset_alignments) reset();

        fprintf(stdout,"\nStart of Pairwise alignments\n");
        fprintf(stdout,"Aligning...\n");
        if(dnaflag) {
                gap_open   = dna_gap_open;
                gap_extend = dna_gap_extend;
                pw_go_penalty  = dna_pw_go_penalty;
                pw_ge_penalty  = dna_pw_ge_penalty;
                ktup       = dna_ktup;
                window     = dna_window;
                signif     = dna_signif;
                wind_gap   = dna_wind_gap;

        }
        else {
                gap_open   = prot_gap_open;
                gap_extend = prot_gap_extend;
                pw_go_penalty  = prot_pw_go_penalty;
                pw_ge_penalty  = prot_pw_ge_penalty;
                ktup       = prot_ktup;
                window     = prot_window;
                signif     = prot_signif;
                wind_gap   = prot_wind_gap;

        }

        if (quick_pairalign)
           show_pair();
        else
           pairalign(0,nseqs,0,nseqs);

	if (nseqs > 3) guide_tree();
	
	count = malign(0);
	
	if (count <= 0) return;

	fprintf(stdout,"\n\n\n");
        for (i=1;i<=nseqs;i++)
           if (length < seqlen_array[i]) length = seqlen_array[i];
	fprintf(stdout,"Consensus length = %d\n",(pint)length);
	
	create_alignment_output(length);
        if (showaln && usemenu) show_aln();
}





static void new_sequence_align(void)
{ 
	FILE *tree;
	char path[FILENAMELEN+1],temp[FILENAMELEN+1];
        char line[MAXLINE];
	int i,j,count,status,length = 0;
	Boolean tclustal, tdistance, tphylip;
	double dscore;
	
	if(profile1_empty && usemenu) {
		error("No profile in memory. Input 1st profile first.");
		return;
	}

	if(profile2_empty && usemenu) {
		error("No sequences in memory. Input sequences first.");
		return;
	}

        get_path(profile2_name,path);

        if(!open_alignment_output(path)) return;

	new_seq = profile1_nseqs+1;

/* get the phylogenetic tree from *.dnd */
/*
	if (nseqs > 3) {
        	get_path(profile1_name,path);
                strcpy(phylip_phy_tree_name,path);
                strcat(phylip_phy_tree_name,"dnd");

		tdistance = output_tree_distances;
		tclustal = output_tree_clustal;
		tphylip = output_tree_phylip;
		output_tree_distances = output_tree_clustal = FALSE;
		output_tree_phylip = TRUE;

		phylogenetic_tree();
		output_tree_distances = tdistance;
		output_tree_clustal = tclustal;
		output_tree_phylip = tphylip;
		status = read_tree(phylip_phy_tree_name, 0, nseqs);
  		if (status == 0) return;
	}
*/

	for (i=1;i<=new_seq;i++) {
     		for (j=i+1;j<=new_seq;j++) {
       			dscore = countid(i,j);
       			tmat[i][j] = (100.0 - dscore)/100.0;
       			tmat[j][i] = tmat[i][j];
     		}
   	}



/* calculate tmat matrix as distance matrix */

/*
	status = calc_distances(new_seq-1);
	if (status == 0) return;

	if (nseqs > 3) {
		clear_tree(NULL);
	}
*/

/* open the new tree file */

	if (nseqs > 3)
	if (explicit_treefile) {
		if((phylip_phy_tree_file = open_explicit_file(
		phylip_phy_tree_name))==NULL) return;
	}
	else {
        	get_path(profile2_name,path);
                strcpy(phylip_phy_tree_name,path);
                strcat(phylip_phy_tree_name,"dnd");
        	if((phylip_phy_tree_file = open_output_file(
                "\nEnter name for new GUIDE TREE          file  ",path,
                phylip_phy_tree_name,"dnd")) == NULL) return;
	}

	if (save_parameters) create_parameter_output();

	if(reset_alignments) reset();

        fprintf(stdout,"\nStart of Pairwise alignments\n");
        fprintf(stdout,"Aligning...\n");
        if(dnaflag) {
                gap_open   = dna_gap_open;
                gap_extend = dna_gap_extend;
                pw_go_penalty  = dna_pw_go_penalty;
                pw_ge_penalty  = dna_pw_ge_penalty;
                ktup       = dna_ktup;
                window     = dna_window;
                signif     = dna_signif;
                wind_gap   = dna_wind_gap;

        }
        else {
                gap_open   = prot_gap_open;
                gap_extend = prot_gap_extend;
                pw_go_penalty  = prot_pw_go_penalty;
                pw_ge_penalty  = prot_pw_ge_penalty;
                ktup       = prot_ktup;
                window     = prot_window;
                signif     = prot_signif;
                wind_gap   = prot_wind_gap;

        }

        if (quick_pairalign)
           show_pair();
        else
           pairalign(0,nseqs,new_seq-2,nseqs);

	if (nseqs > 3) guide_tree();
	
	count = malign(new_seq-2);
	
	if (count <= 0) return;

	fprintf(stdout,"\n\n\n");
        for (i=1;i<=nseqs;i++)
           if (length < seqlen_array[i]) length = seqlen_array[i];
	fprintf(stdout,"Consensus length = %d\n",(pint)length);
	
	create_alignment_output(length);
        if (showaln && usemenu) show_aln();

}









static void make_tree(void)
{
	char path[FILENAMELEN+1],temp[FILENAMELEN+1];
	
	if(empty) {
		error("No sequences in memory. Load sequences first.");
		return;
	}

	if(reset_alignments) reset();

        get_path(seqname,path);

	if (nseqs <= 3) {
		error("Less than 3 sequences in memory. Phylogenetic tree cannot be built.");
		return;
	}

	if (explicit_treefile) {
		if((phylip_phy_tree_file = open_explicit_file(
		phylip_phy_tree_name))==NULL) return;
	}
	else {
       		 if((phylip_phy_tree_file = open_output_file(
                "\nEnter name for GUIDE TREE          file  ",path,
                phylip_phy_tree_name,"dnd")) == NULL) return;
	}

	if (save_parameters) create_parameter_output();

	fprintf(stdout,"\nStart of Pairwise alignments\n");
	fprintf(stdout,"Aligning...\n");
        if(dnaflag) {
                gap_open   = dna_gap_open;
                gap_extend = dna_gap_extend;
                pw_go_penalty  = dna_pw_go_penalty;
                pw_ge_penalty  = dna_pw_ge_penalty;
                ktup       = dna_ktup;
                window     = dna_window;
                signif     = dna_signif;
                wind_gap   = dna_wind_gap;

        }
        else {
                gap_open   = prot_gap_open;
                gap_extend = prot_gap_extend;
                pw_go_penalty  = prot_pw_go_penalty;
                pw_ge_penalty  = prot_pw_ge_penalty;
                ktup       = prot_ktup;
                window     = prot_window;
                signif     = prot_signif;
                wind_gap   = prot_wind_gap;


        }
   
        if (quick_pairalign)
          show_pair();
        else
          pairalign(0,nseqs,0,nseqs);

        if (nseqs > 3) guide_tree();
	
	if(reset_alignments) reset();

}









static void get_tree(void)
{
	char path[FILENAMELEN+1],temp[MAXLINE+1];
	int i,count,length = 0;
	
	if(empty) {
		error("No sequences in memory. Load sequences first.");
		return;
	}

        get_path(seqname,path);

	if(!open_alignment_output(path)) return;

	if(reset_alignments) reset();

        get_path(seqname,path);

        if (nseqs > 3) {
          
        	if(usemenu) {
       			strcpy(phylip_phy_tree_name,path);
       			strcat(phylip_phy_tree_name,"dnd");

            fprintf(stdout,"\nEnter a name for the guide tree file [%s]: ",
                                           phylip_phy_tree_name);
                	gets(temp);
                	if(*temp != EOS)
                        	strcpy(phylip_phy_tree_name,temp);
        	}

#ifdef VMS
        	if((tree=fopen(phylip_phy_tree_name,"r","rat=cr","rfm=var"))==NULL) {
#else
        	if((tree=fopen(phylip_phy_tree_name,"r"))==NULL) {
#endif
                	error("Cannot open file [%s]",phylip_phy_tree_name);
                	return;
        	}
	}
	else {
        	fprintf(stdout,"\nStart of Pairwise alignments\n");
        	fprintf(stdout,"Aligning...\n");
        	if(dnaflag) {
                	gap_open   = dna_gap_open;
                	gap_extend = dna_gap_extend;
                	pw_go_penalty  = dna_pw_go_penalty;
                	pw_ge_penalty  = dna_pw_ge_penalty;
                	ktup       = dna_ktup;
                	window     = dna_window;
                	signif     = dna_signif;
                	wind_gap   = dna_wind_gap;

        	}
        	else {
                	gap_open   = prot_gap_open;
                	gap_extend = prot_gap_extend;
                	pw_go_penalty  = prot_pw_go_penalty;
                	pw_ge_penalty  = prot_pw_ge_penalty;
                	ktup       = prot_ktup;
                	window     = prot_window;
                	signif     = prot_signif;
                	wind_gap   = prot_wind_gap;

        	}

                if (quick_pairalign)
                   show_pair();
                else
		   pairalign(0,nseqs,0,nseqs);
	}

	if (save_parameters) create_parameter_output();

	count = malign(0);
	if (count <= 0) return;

        for (i=1;i<=nseqs;i++)
           if (length < seqlen_array[i]) length = seqlen_array[i];
	fprintf(stdout,"\n\n\n");
	fprintf(stdout,"Consensus length = %d\n",(pint)length);

	create_alignment_output(length);
        if (showaln && usemenu) show_aln();

}










static void profile_align(void)
{
	char path[FILENAMELEN+1],temp[MAXLINE+1];
	int i,count,length = 0;
	static int sav_oc, sav_od, sav_op;
	
	if(profile2_empty) {
		error("No sequences in memory. Load sequences first.");
		return;
	}

	get_path(seqname,path);
	
	if(!open_alignment_output(path)) return;

	if(reset_alignments) reset();

        if (nseqs > 3)
        if((phylip_phy_tree_file = open_output_file(
                "\nEnter a name for the GUIDE TREE     file ",path,
                phylip_phy_tree_name,"dnd")) == NULL) return;

	if (save_parameters) create_parameter_output();

        count = palign1();
        if (count == 0) return;

        if (nseqs > 3) guide_tree();

        count = palign2();

	if (count == 0) return;

        for (i=1;i<=nseqs;i++)
           if (length < seqlen_array[i])
               length = seqlen_array[i];
	fprintf(stdout,"\n\n\n");
	fprintf(stdout,"Consensus length = %d\n",(pint)length);

	create_alignment_output(length);
        if (showaln && usemenu) show_aln();

}








static void clustal_out(FILE *clusout, int len)
{
	static char seq1[MAXLEN+1];
	char 	    temp[MAXLINE];
	char c;
	int val,i,ii,j,k,l,a,b;
	int chunks,ident,catident[NUMRES],lv1,pos,ptr,copt,flag;

/*
 stop doing this ...... opens duplicate files in VMS  DES
fclose(clusout);
if ((clusout=fopen(clustal_outname,"w")) == NULL)
  {
    fprintf(stderr,"Error opening %s\n",clustal_outfile);
    return;
  }
*/
	fprintf(clusout,"CLUSTAL W(%2.1f) multiple sequence alignment\n\n\n",
                                                revision_level);

	chunks = len/LINELENGTH;
	if(len % LINELENGTH != 0)
		++chunks;
		
	for(lv1=1;lv1<=chunks;++lv1) {
		pos = ((lv1-1)*LINELENGTH)+1;
		ptr = (len<pos+LINELENGTH-1) ? len : pos+LINELENGTH-1;
		for(ii=1;ii<=nseqs;++ii) {
                        i=output_index[ii];
			for(j=pos;j<=ptr;++j) {
				val=seq_array[i][j];
				if((val == -3) || (val == 253)) break;
				else if((val < 0) || (val > max_aa))
                                        seq1[j]='-';
				else {
					if(dnaflag)
						seq1[j]=nucleic_acid_order[val];
					else
						seq1[j]=amino_acid_codes[val];
				}
			}
			for(;j<=ptr;++j) seq1[j]='-';
			strncpy(temp,&seq1[pos],ptr-pos+1);
			temp[ptr-pos+1]=EOS;
			fprintf(clusout,"%-15s %s\n",names[i],temp);
		}
	
		for(i=pos;i<=ptr;++i) {
			seq1[i]=' ';
			ident=0;
			for(j=1;res_cat[j-1]!=NULL;j++) catident[j-1] = 0;
			for(j=1;j<=nseqs;++j) {
				if((seq_array[1][i] >=0) && 
				   (seq_array[1][i] <= max_aa)) {
					if(seq_array[1][i] == seq_array[j][i])
					++ident;
					for(k=1;res_cat[k-1]!=NULL;k++) {
					        for(l=0;c=res_cat[k-1][l];l++) {
							if (amino_acid_codes[seq_array[j][i]]==c)
							{
								catident[k-1]++;
								break;
							}
						}
					}
				}
			}
			if(ident==nseqs)
				seq1[i]='*';
			else if (!dnaflag) {
				for(k=1;res_cat[k-1]!=NULL;k++) {
					if (catident[k-1]==nseqs) {
						seq1[i]='.';
						break;
					}
				}
			}
		}
		strncpy(temp,&seq1[pos],ptr-pos+1);
		temp[ptr-pos+1]=EOS;
		fprintf(clusout,"                %s\n\n",temp);
	}

/* DES	ckfree(output_index); */
	
}






static void gcg_out(FILE *gcgout, int len)
{
/*        static char *aacids = "XCSTPAGNDEQHRKMILVFYW";*/
/*	static char *nbases = "XACGT";	*/
	char seq[MAXLEN+1], residue;
	int all_checks[MAXN+1];
	int i,ii,j,k,val,check,chunks,block,pos1,pos2;	
	long grand_checksum;
	
	for(i=1; i<=nseqs; i++) {
		for(j=1; j<=len; j++) {
			val = seq_array[i][j];
			if((val == -3) || (val == 253)) break;
			else if((val < 0) || (val > max_aa))
				residue = '.';
                        else {
				if(dnaflag)
					residue = nucleic_acid_order[val];
				else
					residue = amino_acid_codes[val];
			}
			seq[j] = residue;
		}
		all_checks[i] = SeqGCGCheckSum(seq+1, len);
	}	

	grand_checksum = 0;
	for(i=1; i<=nseqs; i++) grand_checksum += all_checks[output_index[i]];
	grand_checksum = grand_checksum % 10000;
        fprintf(gcgout,"PileUp\n\n");
	fprintf(gcgout,"\n\n   MSF:%5d  Type: ",(pint)len);
	if(dnaflag)
		fprintf(gcgout,"N");
	else
		fprintf(gcgout,"P");
	fprintf(gcgout,"    Check:%6ld   .. \n\n", (long)grand_checksum);
	for(ii=1; ii<=nseqs; ii++)  {
                i = output_index[ii];
/*		for(j=0; j<MAXNAMES; j++) 
			if(names[i][j] == ' ')  names[i][j] = '_';     */
		fprintf(gcgout,
			" Name: %-15s oo  Len:%5d  Check:%6ld  Weight:  1.00\n",
			names[i],(pint)len,(long)all_checks[i]);
        }
	fprintf(gcgout,"\n//\n");  

	chunks = len/GCG_LINELENGTH;
	if(len % GCG_LINELENGTH != 0) ++chunks;

	for(block=1; block<=chunks; block++) {
		fprintf(gcgout,"\n\n");
		pos1 = ((block-1) * GCG_LINELENGTH) + 1;
		pos2 = (len<pos1+GCG_LINELENGTH-1)? len : pos1+GCG_LINELENGTH-1;
		for(ii=1; ii<=nseqs; ii++) {
                        i = output_index[ii];
			fprintf(gcgout,"\n%-15s ",names[i]);
			for(j=pos1, k=1; j<=pos2; j++, k++) {
/*
    JULIE -
    check for short sequences - pad out with '.' characters to end of alignment
*/
				if (j<=seqlen_array[i])
					val = seq_array[i][j];
				else val = -3;
				if((val == -3) || (val == 253))
					residue = '.';
				else if((val < 0) || (val > max_aa))
					residue = '.';
				else {
					if(dnaflag)
						residue = nucleic_acid_order[val];
					else
						residue = amino_acid_codes[val];
				}
				fprintf(gcgout,"%c",residue);
				if(j % 10 == 0) fprintf(gcgout," ");
			}
		}
	}
/* DES	ckfree(output_index); */
	fprintf(gcgout,"\n\n");
}



static void phylip_out(FILE *phyout, int len)
{
/*      static char *aacids = "XCSTPAGNDEQHRKMILVFYW";*/
/*		static char *nbases = "XACGT";	*/
	char residue;
	int i,ii,j,k,val,chunks,block,pos1,pos2;	
	
	len = seqlen_array[1];


	chunks = len/GCG_LINELENGTH;
	if(len % GCG_LINELENGTH != 0) ++chunks;

	fprintf(phyout,"%6d %6d",(pint)nseqs,(pint)len);

	for(block=1; block<=chunks; block++) {
		pos1 = ((block-1) * GCG_LINELENGTH) + 1;
		pos2 = (len<pos1+GCG_LINELENGTH-1)? len : pos1+GCG_LINELENGTH-1;
		for(ii=1; ii<=nseqs; ii++)  {
                        i = output_index[ii];
			if(block == 1) 
				fprintf(phyout,"\n%-10s ",names[i]);
			else
				fprintf(phyout,"\n           ");
			for(j=pos1, k=1; j<=pos2; j++, k++) {
				val = seq_array[i][j];
				if((val == -3) || (val == 253))
					break;
				else if((val < 0) || (val > max_aa))
					residue = '-';
				else {
					if(dnaflag)
						residue = nucleic_acid_order[val];
					else
						residue = amino_acid_codes[val];
				}
				fprintf(phyout,"%c",residue);
				if(j % 10 == 0) fprintf(phyout," ");
			}
		}
		fprintf(phyout,"\n");
	}
/* DES	ckfree(output_index); */
}





static void nbrf_out(FILE *nbout, int len)
{
/*      static char *aacids = "XCSTPAGNDEQHRKMILVFYW";*/
/*		static char *nbases = "XACGT";	*/
	char seq[MAXLEN+1], residue;
	int i,ii,j,val, slen;	
	
	for(ii=1; ii<=nseqs; ii++) {
                i = output_index[ii];
		fprintf(nbout, dnaflag ? ">DL;" : ">P1;");
		fprintf(nbout, "%s\n%s\n", names[i], titles[i]);
		slen = 0;
		for(j=1; j<=len; j++) {
			val = seq_array[i][j];
			if((val == -3) || (val == 253))
				break;
			else if((val < 0) || (val > max_aa))
				residue = '-';
			else {
				if(dnaflag)
					residue = nucleic_acid_order[val];
				else
					residue = amino_acid_codes[val];
			}
			seq[j] = residue;
			slen++;
		}
		for(j=1; j<=slen; j++) {
			fprintf(nbout,"%c",seq[j]);
			if((j % LINELENGTH == 0) || (j == slen)) 
				fprintf(nbout,"\n");
		}
		fprintf(nbout,"*\n");
	}	
/* DES	ckfree(output_index);  */
}


static void gde_out(FILE *gdeout, int len)
{
/*      static char *aacids = "XCSTPAGNDEQHRKMILVFYW";*/
/*		static char *nbases = "XACGT";	*/
	char seq[MAXLEN+1], residue;
	int i,ii,j,val, slen;	
	
	for(ii=1; ii<=nseqs; ii++) {
                i = output_index[ii];
		fprintf(gdeout, dnaflag ? "#" : "%%");
		fprintf(gdeout, "%s\n", names[i]);
		slen = 0;
		for(j=1; j<=len; j++) {
			val = seq_array[i][j];
			if((val == -3) || (val == 253))
				break;
			else if((val < 0) || (val > max_aa))
				residue = '-';
			else {
				if(dnaflag)
					residue = nucleic_acid_order[val];
				else
					residue = amino_acid_codes[val];
			}
			if (lowercase)
				seq[j] = (char)tolower((int)residue);
			else
				seq[j] = residue;
			slen++;
		}
		for(j=1; j<=slen; j++) {
			fprintf(gdeout,"%c",seq[j]);
			if((j % LINELENGTH == 0) || (j == slen)) 
				fprintf(gdeout,"\n");
		}
	}	
/* DES	ckfree(output_index); */
}


static Boolean open_alignment_output(char *path)
{

	if(!output_clustal && !output_nbrf && !output_gcg &&
		 !output_phylip && !output_gde) {
                error("You must select an alignment output format");
                return FALSE;
        }

	if(output_clustal) 
		if (explicit_outfile) {
			strcpy(clustal_outname,outfile_name);
			if((clustal_outfile = open_explicit_file(
			clustal_outname))==NULL) return FALSE;
		}
		else {
/* DES DEBUG 
fprintf(stdout,"\n\n path = %s\n clustal_outname = %s\n\n",
path,clustal_outname);
*/
		   	if((clustal_outfile = open_output_file(
		    	"\nEnter a name for the CLUSTAL output file ",path,
			clustal_outname,"aln"))==NULL) return FALSE;
/* DES DEBUG 
fprintf(stdout,"\n\n path = %s\n clustal_outname = %s\n\n",
path,clustal_outname);
*/
		}
	if(output_nbrf) 
		if (explicit_outfile) {
			strcpy(nbrf_outname,outfile_name);
			if((nbrf_outfile = open_explicit_file(
			nbrf_outname))==NULL) return FALSE;
		}
		else
			if((nbrf_outfile = open_output_file(
		    	"\nEnter a name for the NBRF/PIR output file",path,
			nbrf_outname,"pir"))==NULL) return FALSE;
	if(output_gcg) 
		if (explicit_outfile) {
			strcpy(gcg_outname,outfile_name);
			if((gcg_outfile = open_explicit_file(
			gcg_outname))==NULL) return FALSE;
		}
		else
			if((gcg_outfile = open_output_file(
		    	"\nEnter a name for the GCG output file     ",path,
			gcg_outname,"msf"))==NULL) return FALSE;
	if(output_phylip) 
		if (explicit_outfile) {
			strcpy(phylip_outname,outfile_name);
			if((phylip_outfile = open_explicit_file(
			phylip_outname))==NULL) return FALSE;
		}
		else
			if((phylip_outfile = open_output_file(
		    	"\nEnter a name for the PHYLIP output file  ",path,
			phylip_outname,"phy"))==NULL) return FALSE;
	if(output_gde) 
		if (explicit_outfile) {
			strcpy(gde_outname,outfile_name);
			if((gde_outfile = open_explicit_file(
			gde_outname))==NULL) return FALSE;
		}
		else
			if((gde_outfile = open_output_file(
		    	"\nEnter a name for the GDE output file     ",path,
			gde_outname,"gde"))==NULL) return FALSE;
	return TRUE;
}





static void create_alignment_output(int length)
{
	if(output_clustal) {
		clustal_out(clustal_outfile, length);
		fclose(clustal_outfile);
		fprintf(stdout,"\nCLUSTAL-Alignment file created  [%s]\n",clustal_outname);
	}
	if(output_nbrf)  {
		nbrf_out(nbrf_outfile, length);
		fclose(nbrf_outfile);
		fprintf(stdout,"\nNBRF/PIR-Alignment file created [%s]\n",nbrf_outname);
	}
	if(output_gcg)  {
		gcg_out(gcg_outfile, length);
		fclose(gcg_outfile);
		fprintf(stdout,"\nGCG-Alignment file created      [%s]\n",gcg_outname);
	}
	if(output_phylip)  {
		phylip_out(phylip_outfile, length);
		fclose(phylip_outfile);
		fprintf(stdout,"\nPHYLIP-Alignment file created   [%s]\n",phylip_outname);
	}
	if(output_gde)  {
		gde_out(gde_outfile, length);
		fclose(gde_outfile);
		fprintf(stdout,"\nGDE-Alignment file created      [%s]\n",gde_outname);
	}
}






static void reset(void)   /* remove gaps from older alignments (code = gap_pos1) */
{		      /* EXCEPT for gaps that were INPUT with the seqs.*/
	register int i,j,sl;   		     /* which have  code = gap_pos2  */
	
	for(i=1;i<=nseqs;++i) {
		sl=0;
		for(j=1;j<=seqlen_array[i];++j) {
			if(seq_array[i][j] == gap_pos1) continue;
			++sl;
			seq_array[i][sl]=seq_array[i][j];
		}
		seqlen_array[i]=sl;
	}
}

static int find_match(char *probe, char *list[], int n)
{
	int i,j,len;
	int count,match;

	len = strlen(probe);
	for (i=0;i<len;i++) {
		count = 0;
		for (j=0;j<n;j++) {
			if (probe[i] == list[j][i]) {
				match = j;
				count++;
			}
		}
		if (count == 1) return(match);
	}
	return(-1);
}

static void create_parameter_output(void)
{
	int i, fd;
	char parname[FILENAMELEN+1], temp[FILENAMELEN+1];
	char path[FILENAMELEN+1];
	FILE *parout;

        get_path(seqname,path);
        strcpy(parname,path);
        strcat(parname,"par");

	if(usemenu) {
        	fprintf(stdout,"\nEnter a name for the parameter output file [%s]: ",
                                           parname);
               	gets(temp);
               	if(*temp != EOS)
                       	strcpy(parname,temp);
       	}

/* create a file with execute permissions first */
	remove(parname);
/*
	fd = creat(parname, 0777);
	close(fd);
*/

        if((parout = open_explicit_file(parname))==NULL) return;

        fprintf(parout,"clustalw \\\n");
	if (!empty && profile1_empty) fprintf(parout,"/infile=%s \\\n",seqname);
	if (!profile1_empty) fprintf(parout,"/profile1=%s\\\n",profile1_name);
	if (!profile2_empty) fprintf(parout,"/profile2=%s \\\n",profile2_name);
	if (dnaflag == TRUE) fprintf(parout,"/type=dna \\\n");
	else                 fprintf(parout,"/type=protein \\\n");

	if (quick_pairalign) {
		fprintf(parout,"/quicktree \\\n");
		fprintf(parout,"/ktuple=%d \\\n",(pint)ktup);
     		fprintf(parout,"/window=%d \\\n",(pint)window);
     		fprintf(parout,"/pairgap=%d \\\n",(pint)wind_gap);
     		fprintf(parout,"/topdiags=%d \\\n",(pint)signif);    
     		if (percent) fprintf(parout,"/score=percent \\\n");      
     		else         fprintf(parout,"/score=absolute \\\n");      
	}
	else {
		if (!dnaflag) {
			fprintf(parout,"/pwmatrix=%s \\\n",pw_mtrxname);
			fprintf(parout,"/pwgapopen=%.2f \\\n",prot_pw_go_penalty);
			fprintf(parout,"/pwgapext=%.2f \\\n",prot_pw_ge_penalty);
		}
		else {
			fprintf(parout,"/pwgapopen=%.2f \\\n",pw_go_penalty);
			fprintf(parout,"/pwgapext=%.2f \\\n",pw_ge_penalty);
		}
	}

	if (!dnaflag) {
		fprintf(parout,"/matrix=%s \\\n",mtrxname);
		fprintf(parout,"/gapopen=%.2f \\\n",prot_gap_open);
		fprintf(parout,"/gapext=%.2f \\\n",prot_gap_extend);
	}
	else {
		fprintf(parout,"/gapopen=%.2f \\\n",dna_gap_open);
		fprintf(parout,"/gapext=%.2f \\\n",dna_gap_extend);
	}

	fprintf(parout,"/maxdiv=%d \\\n",(pint)divergence_cutoff);
	if (!use_endgaps) fprintf(parout,"/endgaps \\\n");    

	if (!dnaflag) {
     		if (neg_matrix) fprintf(parout,"/negative \\\n");   
     		if (no_pref_penalties) fprintf(parout,"/nopgap \\\n");     
     		if (no_hyd_penalties) fprintf(parout,"/nohgap \\\n");     
    		fprintf(parout,"/hgapresidues=%s \\\n",hyd_residues);
     		fprintf(parout,"/gapdist=%d \\\n",(pint)gap_dist);     
	}
	else {
     		if (!is_weight) fprintf(parout,"/transitions \\\n");
	}

     	if (output_gcg) fprintf(parout,"/output=gcg \\\n");
     	else if (output_gde) fprintf(parout,"/output=gde \\\n");
     	else if (output_nbrf) fprintf(parout,"/output=pir \\\n");
     	else if (output_phylip) fprintf(parout,"/output=phylip \\\n");
     	if (explicit_outfile) fprintf(parout,"/outfile=%s \\\n",outfile_name);
     	if (output_order==ALIGNED) fprintf(parout,"/outorder=aligned \\\n");  
     	else                      fprintf(parout,"/outorder=input \\\n");  
     	if (output_gde)
		if (lowercase) fprintf(parout,"/case=lower \\\n");
		else           fprintf(parout,"/case=upper \\\n");


        fprintf(parout,"/interactive\n");

	fclose(parout);
/*
  	if (kimura) fprintf(parout,"/kimura \\\n");     
	if (tossgaps) fprintf(parout,"/tossgaps \\\n");   
	fprintf(parout,"/seed=%d \\\n",(pint)boot_ran_seed);
	fprintf(parout,"/bootstrap=%d \\\n",(pint)boot_ntrials);
*/
}

