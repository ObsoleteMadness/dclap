/* Clustal W #1.4  September 1994 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef THINK_C
#include <console.h>
#endif
#include "clustalw.h"

/*
*	Prototypes
*/

extern void *ckalloc(size_t);
extern void init_amenu(void);
extern void init_matrix(void);
extern void fill_chartab(void);
extern void parse_params(void);
extern void main_menu(void);

static void alloc_mem(void);

/*
*	Global variables
*/
double **tmat;

float revision_level = 1.4;	/* DES SEPT. 1994 */
float           gap_open,      gap_extend;
float           pw_go_penalty, pw_ge_penalty;

FILE *fin,*tree;
FILE *clustal_outfile, *gcg_outfile, *nbrf_outfile, *phylip_outfile,
     *gde_outfile;
FILE     *phylip_phy_tree_file;
int  *seqlen_array;
int usermat[NUMRES*NUMRES], pw_usermat[NUMRES*NUMRES];
int def_aa_xref[NUMRES],aa_xref[NUMRES], pw_aa_xref[NUMRES];
int mat_avscore;
int nseqs;
int first_seq,last_seq;
int nsets;
int max_aa;
int gap_pos1;
int gap_pos2;
int *output_index;
int **sets;
int *seq_weight;
/* prfalign.c statics -- dgg */
int * displ, * gaps, * HH, * DD, * RR, * SS;

Boolean usemenu;
Boolean dnaflag;
Boolean distance_tree;

char  **seq_array;
char **names,**titles;
char *paramstr;
char seqname[FILENAMELEN+1],treename[FILENAMELEN+1];
char phylip_phy_tree_name[FILENAMELEN+1];

static void alloc_mem(void)
{
	register int i;
	
	seqlen_array = (int *)ckalloc( (MAXN+1) * sizeof (int));

	output_index = (int *)ckalloc( (MAXN+1) * sizeof (int));

	seq_array = (char **)ckalloc( (MAXN + 1) * sizeof (char *) );
	for(i=0;i<MAXN+1;i++)
		seq_array[i]=(char *)ckalloc( (MAXLEN +2) * sizeof (char));
				
	names = (char **)ckalloc( (MAXN+1) * sizeof (char *) );
	for(i=0;i<MAXN+1;i++)
		names[i] = (char *)ckalloc(MAXNAMES+1 * sizeof (char));
		
	titles = (char **)ckalloc( (MAXN) * sizeof (char *) );
	for(i=0;i<MAXN;i++)
		titles[i] = (char *)ckalloc(MAXTITLES+1 * sizeof (char));

	tmat = (double **) ckalloc( (MAXN+1) * sizeof (double *) );
	for(i=0;i<MAXN+1;i++)
		tmat[i] = (double *)ckalloc( (MAXN+1) * sizeof (double) );

	paramstr = (char *)ckalloc(1024 * sizeof(char));

	/* statics from prfalign.c -- dgg */
	/* static int 	displ[2*MAXLEN+1];
	 static int    	gaps[MAXLEN];
	 static int HH[MAXLEN+1], DD[MAXLEN+1], RR[MAXLEN+1], SS[MAXLEN+1];
	*/
	displ= (int *) ckalloc( (2*MAXLEN+1) * sizeof(int));
	gaps = ( int* )ckalloc( (MAXLEN+1) * sizeof(int));
	HH = (int*) ckalloc( (MAXLEN+1) * sizeof(int));
	DD = (int*) ckalloc( (MAXLEN+1) * sizeof(int));
	RR = (int*) ckalloc( (MAXLEN+1) * sizeof(int));
	SS = (int*) ckalloc( (MAXLEN+1) * sizeof(int));
}


#ifdef MACINTOSH
void RealMain( int argc, char** argv)
#else
void 	main(argc,argv)
int argc;
char *argv[];
#endif
{
	int i;

#if NOTNOW
#if defined( THINK_C) || defined(__MWERKS__)
	argc=ccommand(&argv);
#endif
#endif

	alloc_mem();
        init_amenu();
        init_matrix();
	
	fill_chartab();
	if(argc>1) {
		paramstr[0]=EOS;
		for(i=1;i<argc;++i) {
			strcat(paramstr,argv[i]);
			/* strcat(paramstr,"/"); /* dgg added this - why is it needed ?? */
			}
		usemenu=FALSE;
		parse_params();
	}
	usemenu=TRUE;

	main_menu();
}
