/* Phyle of filogenetic tree calculating functions for CLUSTAL W */
/* DES was here  FEB. 1994 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "clustalw.h"
#include "dayhoff.h"    /* set correction for amino acid distances >= 75% */

/*
 *   Prototypes
 */

extern void *ckalloc(size_t);
extern void ckfree(void *);
extern void error(char *,...);  /* error reporting */
extern int getint(char *, int, int, int);
extern void get_path(char *, char *);
extern FILE * open_output_file(char *, char *, char *, char *);
/*random number routines in random.c */
extern unsigned long addrand(unsigned long);
extern void addrandinit(unsigned long);
extern void   getstr(char *,char *);

void init_trees(void);
void guide_tree(void);
void phylogenetic_tree(void);
void bootstrap_tree(void);
void compare_tree(char **, char **, int *, int);
void tree_gap_delete(void); /*flag all positions in alignment that have a gap */
int dna_distance_matrix(FILE *);
int prot_distance_matrix(FILE *);
void nj_tree(char **, FILE *);
void print_tree(char **, FILE *, int *);
void print_phylip_tree(char **, FILE *, Boolean);
void root_tree(char **, int);
void distance_matrix_output(FILE *);
void two_way_split(char **, FILE *, int, int, Boolean);
Boolean transition(int,int);

/*
 *   Global variables
 */


extern double **tmat;     /* general nxn array of reals; allocated from main */
                          /* this is used as a distance matrix */
extern Boolean dnaflag;   /* TRUE for DNA seqs; FALSE for proteins */
extern Boolean tossgaps;  /* Ignore places in align. where ANY seq. has a gap*/
extern Boolean kimura;    /* Use correction for multiple substitutions */
extern Boolean output_tree_clustal;   /* clustal text output for trees */
extern Boolean output_tree_phylip;    /* phylip nested parentheses format */
extern Boolean output_tree_distances; /* phylip distance matrix */
extern Boolean empty;                 /* any sequences in memory? */
extern Boolean usemenu;   /* interactive (TRUE) or command line (FALSE) */
extern int nseqs;
extern int max_aa;
extern int first_seq, last_seq;
extern int *seqlen_array; /* the lengths of the sequences */
extern char **seq_array;   /* the sequences */
extern char **names;       /* the seq. names */
extern char seqname[];		/* name of input file */
extern char phylip_phy_tree_name[FILENAMELEN+1];
extern FILE 	*phylip_phy_tree_file;

static double 	*av;
static double 	*left_branch, *right_branch;
static double 	*save_left_branch, *save_right_branch;
static int	*boot_totals;
static int 	*kill;
static int 	ran_factor;
static int 	root_sequence;
static int 	*boot_positions;
static FILE 	*clustal_phy_tree_file;
static FILE 	*distances_phy_tree_file;
static char 	clustal_phy_tree_name[FILENAMELEN+1];
static char 	distances_phy_tree_name[FILENAMELEN+1];
static Boolean 	verbose;
static char 	*tree_gaps;
                     /* array of weights; 1 for use this posn.; 0 don't */

extern int boot_ntrials;		/* number of bootstrap trials */
extern unsigned int boot_ran_seed;	/* random number generator seed */

void phylogenetic_tree(void)
/* 
   Calculate a tree using the distances in the nseqs*nseqs array tmat.
   This is the routine for getting the REAL trees after alignment.
*/
{	char path[FILENAMELEN+1];
	int i, j;
	int overspill = 0;
	int total_dists;
	static char **standard_tree;
	char lin2[10];

	if(empty) {
		error("You must load an alignment first");
		return;
	}

	get_path(seqname,path);
	
if(output_tree_clustal)
	if((clustal_phy_tree_file = open_output_file(
		"\nEnter name for CLUSTAL    tree output file  ",path,
		clustal_phy_tree_name,"nj")) == NULL) return;

if(output_tree_phylip)
	if((phylip_phy_tree_file = open_output_file(
		"\nEnter name for PHYLIP     tree output file  ",path,
		phylip_phy_tree_name,"ph")) == NULL) return;

if(output_tree_distances)
	if((distances_phy_tree_file = open_output_file(
		"\nEnter name for distance matrix output file  ",path,
		distances_phy_tree_name,"dst")) == NULL) return;


	boot_positions = (int *)ckalloc( (MAXLEN+1) * sizeof (int) );

	for(j=1; j<=seqlen_array[first_seq]; ++j) 
		boot_positions[j] = j;		

	if(output_tree_clustal) {
		verbose = TRUE;     /* Turn on file output */
		if(dnaflag)
			overspill = dna_distance_matrix(clustal_phy_tree_file);
		else 
			overspill = prot_distance_matrix(clustal_phy_tree_file);
	}

	if(output_tree_phylip) {
		verbose = FALSE;     /* Turn off file output */
		if(dnaflag)
			overspill = dna_distance_matrix(phylip_phy_tree_file);
		else 
			overspill = prot_distance_matrix(phylip_phy_tree_file);
	}

	if(output_tree_distances) {
		verbose = FALSE;     /* Turn off file output */
		if(dnaflag)
			overspill = dna_distance_matrix(distances_phy_tree_file);
		else 
			overspill = prot_distance_matrix(distances_phy_tree_file);
      		distance_matrix_output(distances_phy_tree_file);
	}

/* check if any distances overflowed the distance corrections */
	if ( overspill > 0 ) {
		total_dists = (nseqs*(nseqs-1))/2;
		fprintf(stdout,"\n");
		fprintf(stdout,"\n WARNING: %ld of the distances out of a total of %ld",
		(long)overspill,(long)total_dists);
		fprintf(stdout,"\n were out of range for the distance correction.");
		fprintf(stdout,"\n This may not be fatal but you have been warned!");
		fprintf(stdout,"\n");
		fprintf(stdout,"\n SUGGESTIONS: 1) turn off the correction");
		fprintf(stdout,"\n           or 2) remove the most distant sequences");
		fprintf(stdout,"\n           or 3) use the PHYLIP package.");
		fprintf(stdout,"\n\n");
		if (usemenu) 
			getstr("Press [RETURN] to continue",lin2);
	}

	if(output_tree_clustal) verbose = TRUE;     /* Turn on file output */

	standard_tree   = (char **) ckalloc( (nseqs+1) * sizeof (char *) );
	for(i=0; i<nseqs+1; i++) 
		standard_tree[i]  = (char *) ckalloc( (nseqs+1) * sizeof(char) );

	if(output_tree_clustal || output_tree_phylip) 
		nj_tree(standard_tree,clustal_phy_tree_file);

	if(output_tree_phylip) 
		print_phylip_tree(standard_tree,phylip_phy_tree_file,FALSE);

/*
	print_tree(standard_tree,phy_tree_file);
*/
	ckfree((void *)tree_gaps);
	ckfree((void *)boot_positions);
	ckfree((void *)left_branch);
	ckfree((void *)right_branch);
	ckfree((void *)kill);
	ckfree((void *)av);
	for (i=1;i<nseqs+1;i++)
		ckfree((void *)standard_tree[i]);
	ckfree((void *)standard_tree);

if(output_tree_clustal) {
	fclose(clustal_phy_tree_file);	
	fprintf(stdout,"\nPhylogenetic tree file created:   [%s]\n",clustal_phy_tree_name);
}

if(output_tree_phylip) {
	fclose(phylip_phy_tree_file);	
	fprintf(stdout,"\nPhylogenetic tree file created:   [%s]\n",phylip_phy_tree_name);
}

if(output_tree_distances) {
	fclose(distances_phy_tree_file);	
	fprintf(stdout,"\nDistance matrix  file  created:   [%s]\n",distances_phy_tree_name);
}


}





Boolean transition(int base1, int base2) /* TRUE if transition; else FALSE */
/* 

   assumes that the bases of DNA sequences have been translated as
   a,A = 0;   c,C = 1;   g,G = 2;   t,T,u,U = 3;  N = 4;  

   A <--> G  and  T <--> C  are transitions;  all others are transversions.

*/
{
	if( ((base1 == 0) && (base2 == 2)) || ((base1 == 2) && (base2 == 0)) )
		return TRUE;                                     /* A <--> G */
	if( ((base1 == 3) && (base2 == 1)) || ((base1 == 1) && (base2 == 3)) )
		return TRUE;                                     /* T <--> C */
    return FALSE;
}


void tree_gap_delete(void)   /* flag all positions in alignment that have a gap */
{			  /* in ANY sequence */
	int seqn, posn;

	tree_gaps = (char *)ckalloc( (MAXLEN+1) * sizeof (char) );
        
	for(posn=1; posn<=seqlen_array[first_seq]; ++posn) {
		tree_gaps[posn] = 0;
     	for(seqn=1; seqn<=last_seq-first_seq+1; ++seqn)  {
			if((seq_array[seqn+first_seq-1][posn] < 0) ||
			   (seq_array[seqn+first_seq-1][posn] > max_aa)) {
			   tree_gaps[posn] = 1;
				break;
			}
		}
	}
}

void distance_matrix_output(FILE *ofile)
{
	int i,j;
	
	fprintf(ofile,"%6d",(pint)last_seq-first_seq+1);
	for(i=1;i<=last_seq-first_seq+1;i++) {
		fprintf(ofile,"\n%-10s ",names[i]);
		for(j=1;j<=last_seq-first_seq+1;j++) {
			fprintf(ofile,"%6.3f ",tmat[i][j]);
			if(j % 8 == 0) {
				fprintf(ofile,"\n"); 
				if(j != last_seq-first_seq+1 ) fprintf(ofile,"          ");
			}
		}
	}
}



void nj_tree(char **tree_description, FILE *tree)
{
	register int i;
	int l[4],nude,k;
	int nc,mini,minj,j,j1,ii,jj;
	double fnseqs,fnseqs2,sumd;
	double diq,djq,dij,d2r,dr,dio,djo,da;
	double tmin,total,dmin;
	double bi,bj,b1,b2,b3,branch[4];
	int typei,typej,type[4];             /* 0 = node; 1 = OTU */
	
	fnseqs = (double)last_seq-first_seq+1;

/*********************** First initialisation ***************************/
	
	if(verbose)  {
		fprintf(tree,"\n\n\t\t\tNeighbor-joining Method\n");
		fprintf(tree,"\n Saitou, N. and Nei, M. (1987)");
		fprintf(tree," The Neighbor-joining Method:");
		fprintf(tree,"\n A New Method for Reconstructing Phylogenetic Trees.");
		fprintf(tree,"\n Mol. Biol. Evol., 4(4), 406-425\n");
		fprintf(tree,"\n\n This is an UNROOTED tree\n");
		fprintf(tree,"\n Numbers in parentheses are branch lengths\n\n");
	}	

	mini = minj = 0;

	left_branch 	= (double *) ckalloc( (nseqs+2) * sizeof (double)   );
	right_branch    = (double *) ckalloc( (nseqs+2) * sizeof (double)   );
	kill 		= (int *) ckalloc( (nseqs+1) * sizeof (int) );
	av   		= (double *) ckalloc( (nseqs+1) * sizeof (double)   );

	for(i=1;i<=last_seq-first_seq+1;++i) 
		{
		tmat[i][i] = av[i] = 0.0;
		kill[i] = 0;
		}

/*********************** Enter The Main Cycle ***************************/

 /*	for(nc=1; nc<=(last_seq-first_seq+1-3); ++nc) {  */            	/**start main cycle**/
	for(nc=1; nc<=(last_seq-first_seq+1-3); ++nc) {
		sumd = 0.0;
		for(j=2; j<=last_seq-first_seq+1; ++j)
			for(i=1; i<j; ++i) {
				tmat[j][i] = tmat[i][j];
				sumd = sumd + tmat[i][j];
			}

		tmin = 99999.0;

/*.................compute SMATij values and find the smallest one ........*/

		for(jj=2; jj<=last_seq-first_seq+1; ++jj) 
			if(kill[jj] != 1) 
				for(ii=1; ii<jj; ++ii)
					if(kill[ii] != 1) {
						diq = djq = 0.0;

						for(i=1; i<=last_seq-first_seq+1; ++i) {
							diq = diq + tmat[i][ii];
							djq = djq + tmat[i][jj];
						}

						dij = tmat[ii][jj];
						d2r = diq + djq - (2.0*dij);
						dr  = sumd - dij -d2r;
						fnseqs2 = fnseqs - 2.0;
					        total= d2r+ fnseqs2*dij +dr*2.0;
						total= total / (2.0*fnseqs2);

						if(total < tmin) {
							tmin = total;
							mini = ii;
							minj = jj;
						}
					}
		

/*.................compute branch lengths and print the results ........*/


		dio = djo = 0.0;
		for(i=1; i<=last_seq-first_seq+1; ++i) {
			dio = dio + tmat[i][mini];
			djo = djo + tmat[i][minj];
		}

		dmin = tmat[mini][minj];
		dio = (dio - dmin) / fnseqs2;
		djo = (djo - dmin) / fnseqs2;
		bi = (dmin + dio - djo) * 0.5;
		bj = dmin - bi;
		bi = bi - av[mini];
		bj = bj - av[minj];

		if( av[mini] > 0.0 )
			typei = 0;
		else
			typei = 1;
		if( av[minj] > 0.0 )
			typej = 0;
		else
			typej = 1;

		if(verbose) 
	 	    fprintf(tree,"\n Cycle%4d     = ",(pint)nc);

/* 
   set negative branch lengths to zero.  Also set any tiny positive
   branch lengths to zero.
*/		if( fabs(bi) < 0.0001) bi = 0.0;
		if( fabs(bj) < 0.0001) bj = 0.0;

	    	if(verbose) {
		    if(typei == 0) 
			fprintf(tree,"Node:%4d (%9.5f) joins ",(pint)mini,bi);
		    else 
			fprintf(tree," SEQ:%4d (%9.5f) joins ",(pint)mini,bi);

		    if(typej == 0) 
			fprintf(tree,"Node:%4d (%9.5f)",(pint)minj,bj);
		    else 
			fprintf(tree," SEQ:%4d (%9.5f)",(pint)minj,bj);

		    fprintf(tree,"\n");
	    	}	


	    	left_branch[nc] = bi;
	    	right_branch[nc] = bj;

		for(i=1; i<=last_seq-first_seq+1; i++)
			tree_description[nc][i] = 0;

	     	if(typei == 0) { 
			for(i=nc-1; i>=1; i--)
				if(tree_description[i][mini] == 1) {
					for(j=1; j<=last_seq-first_seq+1; j++)  
					     if(tree_description[i][j] == 1)
						    tree_description[nc][j] = 1;
					break;
				}
		}
		else
			tree_description[nc][mini] = 1;

		if(typej == 0) {
			for(i=nc-1; i>=1; i--) 
				if(tree_description[i][minj] == 1) {
					for(j=1; j<=last_seq-first_seq+1; j++)  
					     if(tree_description[i][j] == 1)
						    tree_description[nc][j] = 1;
					break;
				}
		}
		else
			tree_description[nc][minj] = 1;
			

/* 
   Here is where the -0.00005 branch lengths come from for 3 or more
   identical seqs.
*/
/*		if(dmin <= 0.0) dmin = 0.0001; */
                if(dmin <= 0.0) dmin = 0.000001;
		av[mini] = dmin * 0.5;

/*........................Re-initialisation................................*/

		fnseqs = fnseqs - 1.0;
		kill[minj] = 1;

		for(j=1; j<=last_seq-first_seq+1; ++j) 
			if( kill[j] != 1 ) {
				da = ( tmat[mini][j] + tmat[minj][j] ) * 0.5;
				if( (mini - j) < 0 ) 
					tmat[mini][j] = da;
				if( (mini - j) > 0)
					tmat[j][mini] = da;
			}

		for(j=1; j<=last_seq-first_seq+1; ++j)
			tmat[minj][j] = tmat[j][minj] = 0.0;


/****/	}						/**end main cycle**/

/******************************Last Cycle (3 Seqs. left)********************/

	nude = 1;

	for(i=1; i<=last_seq-first_seq+1; ++i)
		if( kill[i] != 1 ) {
			l[nude] = i;
			nude = nude + 1;
		}

	b1 = (tmat[l[1]][l[2]] + tmat[l[1]][l[3]] - tmat[l[2]][l[3]]) * 0.5;
	b2 =  tmat[l[1]][l[2]] - b1;
	b3 =  tmat[l[1]][l[3]] - b1;
 
	branch[1] = b1 - av[l[1]];
	branch[2] = b2 - av[l[2]];
	branch[3] = b3 - av[l[3]];

/* Reset tiny negative and positive branch lengths to zero */
	if( fabs(branch[1]) < 0.0001) branch[1] = 0.0;
	if( fabs(branch[2]) < 0.0001) branch[2] = 0.0;
	if( fabs(branch[3]) < 0.0001) branch[3] = 0.0;

	left_branch[last_seq-first_seq+1-2] = branch[1];
	left_branch[last_seq-first_seq+1-1] = branch[2];
	left_branch[last_seq-first_seq+1]   = branch[3];

	for(i=1; i<=last_seq-first_seq+1; i++)
		tree_description[last_seq-first_seq+1-2][i] = 0;

	if(verbose)
		fprintf(tree,"\n Cycle%4d (Last cycle, trichotomy):\n",(pint)nc);

	for(i=1; i<=3; ++i) {
	   if( av[l[i]] > 0.0) {
	      	if(verbose)
	      	    fprintf(tree,"\n\t\t Node:%4d (%9.5f) ",(pint)l[i],branch[i]);
		for(k=last_seq-first_seq+1-3; k>=1; k--)
			if(tree_description[k][l[i]] == 1) {
				for(j=1; j<=last_seq-first_seq+1; j++)
				 	if(tree_description[k][j] == 1)
					    tree_description[last_seq-first_seq+1-2][j] = i;
				break;
			}
	   }
	   else  {
	      	if(verbose)
	   	    fprintf(tree,"\n\t\t  SEQ:%4d (%9.5f) ",(pint)l[i],branch[i]);
		tree_description[last_seq-first_seq+1-2][l[i]] = i;
	   }
	   if(i < 3) {
	      	if(verbose)
	            fprintf(tree,"joins");
	   }
	}

	if(verbose)
		fprintf(tree,"\n");

}




void bootstrap_tree(void)
{
	int i,j,ranno;
	int k,l,m,p;
	unsigned int num;
	char lin2[MAXLINE],path[MAXLINE+1];
        char dummy[10];
	static char **sample_tree;
	static char **standard_tree;
	int total_dists, overspill, total_overspill = 0;
	int nfails = 0;

	if(empty) {
		error("You must load an alignment first");
		return;
	}

	if(!output_tree_clustal && !output_tree_phylip) {
		error("You must select either clustal or phylip tree output format");
		return;
	}
	get_path(seqname, path);
	
	if (output_tree_clustal)
		if((clustal_phy_tree_file = open_output_file(
		"\nEnter name for bootstrap output file  ",path,
		clustal_phy_tree_name,"njb")) == NULL) return;

	if (output_tree_phylip)
		if((phylip_phy_tree_file = open_output_file(
		"\nEnter name for bootstrap output file  ",path,
		phylip_phy_tree_name,"phb")) == NULL) return;

	boot_totals    = (int *)ckalloc( (nseqs+1) * sizeof (int) );
	for(i=0;i<nseqs+1;i++)
		boot_totals[i]=0;
		
	boot_positions = (int *)ckalloc( (MAXLEN+1) * sizeof (int) );

	for(j=1; j<=seqlen_array[first_seq]; ++j)  /* First select all positions for */
		boot_positions[j] = j;	   /* the "standard" tree */

	if(output_tree_clustal) {
		verbose = TRUE;     /* Turn on file output */
		if(dnaflag)
			overspill = dna_distance_matrix(clustal_phy_tree_file);
		else 
			overspill = prot_distance_matrix(clustal_phy_tree_file);
	}

	if(output_tree_phylip) {
		verbose = FALSE;     /* Turn off file output */
		if(dnaflag)
			overspill = dna_distance_matrix(phylip_phy_tree_file);
		else 
			overspill = prot_distance_matrix(phylip_phy_tree_file);
	}

/* check if any distances overflowed the distance corrections */
	if ( overspill > 0 ) {
		total_dists = (nseqs*(nseqs-1))/2;
		fprintf(stdout,"\n");
		fprintf(stdout,"\n WARNING: %ld of the distances out of a total of %ld",
		(pint)overspill,(pint)total_dists);
		fprintf(stdout,"\n were out of range for the distance correction.");
		fprintf(stdout,"\n This may not be fatal but you have been warned!");
		fprintf(stdout,"\n");
		fprintf(stdout,"\n SUGGESTIONS: 1) turn off the correction");
		fprintf(stdout,"\n           or 2) remove the most distant sequences");
		fprintf(stdout,"\n           or 3) use the PHYLIP package.");
		fprintf(stdout,"\n\n");
		if (usemenu) 
			getstr("Press [RETURN] to continue",dummy);
	}

	ckfree((void *)tree_gaps);

	if (output_tree_clustal) verbose = TRUE;   /* Turn on screen output */

	standard_tree   = (char **) ckalloc( (nseqs+1) * sizeof (char *) );
	for(i=0; i<nseqs+1; i++) 
		standard_tree[i]   = (char *) ckalloc( (nseqs+1) * sizeof(char) );

/* compute the standard tree */

	if(output_tree_clustal || output_tree_phylip)
		nj_tree(standard_tree,clustal_phy_tree_file);

	if (output_tree_clustal)
		fprintf(clustal_phy_tree_file,"\n\n\t\t\tBootstrap Confidence Limits\n\n");

/* save the left_branch and right_branch for phylip output */
	save_left_branch = (double *) ckalloc( (nseqs+2) * sizeof (double)   );
	save_right_branch = (double *) ckalloc( (nseqs+2) * sizeof (double)   );
	for (i=1;i<=nseqs;i++) {
		save_left_branch[i] = left_branch[i];
		save_right_branch[i] = right_branch[i];
	}
/*  
  The next line is a fossil from the days of using the cc ran()
	ran_factor = RAND_MAX / seqlen_array[first_seq]; 
*/

	if(usemenu) 
   		boot_ran_seed = 
getint("\n\nEnter seed no. for random number generator ",1,1000,boot_ran_seed);

/* do not use the native cc ran()
	srand(boot_ran_seed);
*/
       	addrandinit((unsigned long) boot_ran_seed);

	if (output_tree_clustal)
		fprintf(clustal_phy_tree_file,"\n Random number generator seed = %7u\n",
		boot_ran_seed);

	if(usemenu) 
  		boot_ntrials = 
getint("\n\nEnter number of bootstrap trials ",1,10000,boot_ntrials);

	if (output_tree_clustal) {
  		fprintf(clustal_phy_tree_file,"\n Number of bootstrap trials   = %7d\n",
	(pint)boot_ntrials);

		fprintf(clustal_phy_tree_file,
		"\n\n Diagrammatic representation of the above tree: \n");
		fprintf(clustal_phy_tree_file,"\n Each row represents 1 tree cycle;");
		fprintf(clustal_phy_tree_file," defining 2 groups.\n");
		fprintf(clustal_phy_tree_file,"\n Each column is 1 sequence; ");
		fprintf(clustal_phy_tree_file,"the stars in each line show 1 group; ");
		fprintf(clustal_phy_tree_file,"\n the dots show the other\n");
		fprintf(clustal_phy_tree_file,"\n Numbers show occurences in bootstrap samples.");
	}
/*
	print_tree(standard_tree, clustal_phy_tree_file, boot_totals);
*/
	verbose = FALSE;                   /* Turn OFF screen output */

	ckfree((void *)left_branch);
	ckfree((void *)right_branch);
	ckfree((void *)kill);
	ckfree((void *)av);

	sample_tree   = (char **) ckalloc( (nseqs+1) * sizeof (char *) );
	for(i=0; i<nseqs+1; i++) 
		sample_tree[i]   = (char *) ckalloc( (nseqs+1) * sizeof(char) );

	fprintf(stdout,"\n\nEach dot represents 10 trials\n\n");
        total_overspill = 0;
	nfails = 0;
	for(i=1; i<=boot_ntrials; ++i) {
		for(j=1; j<=seqlen_array[first_seq]; ++j) { /* select alignment */
							    /* positions for */
			ranno = addrand( (unsigned long) seqlen_array[1]) + 1;
			boot_positions[j] = ranno; 	    /* bootstrap sample */
		}
		if(output_tree_clustal) {
			if(dnaflag)
				overspill = dna_distance_matrix(clustal_phy_tree_file);
			else 
				overspill = prot_distance_matrix(clustal_phy_tree_file);
		}
	
		if(output_tree_phylip) {
			if(dnaflag)
				overspill = dna_distance_matrix(phylip_phy_tree_file);
			else 
				overspill = prot_distance_matrix(phylip_phy_tree_file);
		}

		if( overspill > 0) {
			total_overspill = total_overspill + overspill;
			nfails++;
		}			

		ckfree((void *)tree_gaps);

		if(output_tree_clustal || output_tree_phylip) 
			nj_tree(sample_tree,clustal_phy_tree_file);

		compare_tree(standard_tree, sample_tree, boot_totals, last_seq-first_seq+1);
		if(i % 10  == 0) fprintf(stdout,".");
		if(i % 100 == 0) fprintf(stdout,"\n");
	}

/* check if any distances overflowed the distance corrections */
	if ( nfails > 0 ) {
		total_dists = (nseqs*(nseqs-1))/2;
		fprintf(stdout,"\n");
		fprintf(stdout,"\n WARNING: %ld of the distances out of a total of %ld times %ld",
		(long)total_overspill,(long)total_dists,(long)boot_ntrials);
		fprintf(stdout,"\n were out of range for the distance correction.");
		fprintf(stdout,"\n This affected %d out of %d bootstrap trials.",
		(pint)nfails,(pint)boot_ntrials);
		fprintf(stdout,"\n This may not be fatal but you have been warned!");
		fprintf(stdout,"\n");
		fprintf(stdout,"\n SUGGESTIONS: 1) turn off the correction");
		fprintf(stdout,"\n           or 2) remove the most distant sequences");
		fprintf(stdout,"\n           or 3) use the PHYLIP package.");
		fprintf(stdout,"\n\n");
		if (usemenu) 
			getstr("Press [RETURN] to continue",dummy);
	}


	ckfree((void *)boot_positions);

	for (i=1;i<nseqs+1;i++)
		ckfree((void *)sample_tree[i]);
	ckfree((void *)sample_tree);
/*
	fprintf(clustal_phy_tree_file,"\n\n Bootstrap totals for each group\n");
*/
	if (output_tree_clustal)
		print_tree(standard_tree, clustal_phy_tree_file, boot_totals);

	if(output_tree_phylip) {
		for (i=1;i<=nseqs;i++) {
			left_branch[i] = save_left_branch[i];
			right_branch[i] = save_right_branch[i];
		}
		print_phylip_tree(standard_tree,phylip_phy_tree_file,
								 TRUE);
	}

	ckfree((void *)boot_totals);
	ckfree((void *)save_left_branch);
	ckfree((void *)save_right_branch);

	for (i=1;i<nseqs+1;i++)
		ckfree((void *)standard_tree[i]);
	ckfree((void *)standard_tree);

	if (output_tree_clustal)
		fclose(clustal_phy_tree_file);

	if (output_tree_phylip)
		fclose(phylip_phy_tree_file);

	if (output_tree_clustal)
		fprintf(stdout,"\n\nBootstrap output file completed       [%s]"
		,clustal_phy_tree_name);
	if (output_tree_phylip)
		fprintf(stdout,"\n\nBootstrap output file completed       [%s]"
		,phylip_phy_tree_name);
}


void compare_tree(char **tree1, char **tree2, int *hits, int n)
{	
	int i,j,k,l;
	int nhits1, nhits2;

	for(i=1; i<=n-3; i++)  {
		for(j=1; j<=n-3; j++)  {
			nhits1 = 0;
			nhits2 = 0;
			for(k=1; k<=n; k++) {
				if(tree1[i][k] == tree2[j][k]) nhits1++;
				if(tree1[i][k] != tree2[j][k]) nhits2++;
			}
			if((nhits1 == last_seq-first_seq+1) || (nhits2 == last_seq-first_seq+1)) hits[i]++;
		}
	}
}


void print_phylip_tree(char **tree_description, FILE *tree, Boolean bootstrap)
{
	fprintf(tree,"(\n");
 
	two_way_split(tree_description, tree, last_seq-first_seq+1-2,1,bootstrap);
	fprintf(tree,":%7.5f,\n",left_branch[last_seq-first_seq+1-2]);
	two_way_split(tree_description, tree, last_seq-first_seq+1-2,2,bootstrap);
	fprintf(tree,":%7.5f,\n",left_branch[last_seq-first_seq+1-1]);
	two_way_split(tree_description, tree, last_seq-first_seq+1-2,3,bootstrap);

	fprintf(tree,":%7.5f);\n",left_branch[last_seq-first_seq+1]);
}


void two_way_split
(char **tree_description, FILE *tree, int start_row, int flag, Boolean bootstrap)
{
	int row, row2, new_row, col, col2, test_col;
	Boolean single_seq;

	if(start_row != last_seq-first_seq+1-2) fprintf(tree,"(\n"); 

	for(col=1; col<=last_seq-first_seq+1; col++) {
		if(tree_description[start_row][col] == flag) {
			test_col = col;
			break;
		}
	}

	single_seq = TRUE;
	for(row=start_row-1; row>=1; row--) 
		if(tree_description[row][test_col] == 1) {
			single_seq = FALSE;
			new_row = row;
			break;
		}

	if(single_seq) {
		tree_description[start_row][test_col] = 0;
		fprintf(tree,"%.10s",names[test_col+first_seq-1]);
	}
	else {
		for(col=1; col<=last_seq-first_seq+1; col++) {
		    if((tree_description[start_row][col]==1)&&
		       (tree_description[new_row][col]==1))
				tree_description[start_row][col] = 0;
		}
		two_way_split(tree_description, tree, new_row, 1, bootstrap);
	}

	if(start_row == last_seq-first_seq+1-2) return;

	fprintf(tree,":%7.5f,\n",left_branch[start_row]);

	for(col=1; col<=last_seq-first_seq+1; col++) 
		if(tree_description[start_row][col] == flag) {
			test_col = col;
			break;
		}
	
	single_seq = TRUE;
	for(row=start_row-1; row>=1; row--) 
		if(tree_description[row][test_col] == 1) {
			single_seq = FALSE;
			new_row = row;
			break;
		}

	if(single_seq) {
		tree_description[start_row][test_col] = 0;
		fprintf(tree,"%.10s",names[test_col+first_seq-1]);
	}
	else {
		for(col=1; col<=last_seq-first_seq+1; col++) {
		    if((tree_description[start_row][col]==1)&&
		       (tree_description[new_row][col]==1))
				tree_description[start_row][col] = 0;
		}
		two_way_split(tree_description, tree, new_row, 1, bootstrap);
	}

	fprintf(tree,":%7.5f)\n",right_branch[start_row]);
	if (bootstrap && (boot_totals[start_row]>0))
		fprintf(tree,"%d",(pint)boot_totals[start_row]);
}



void print_tree(char **tree_description, FILE *tree, int *totals)
{
	int row,col;

	fprintf(tree,"\n");

	for(row=1; row<=last_seq-first_seq+1-3; row++)  {
		fprintf(tree," \n");
		for(col=1; col<=last_seq-first_seq+1; col++) { 
			if(tree_description[row][col] == 0)
				fprintf(tree,"*");
			else
				fprintf(tree,".");
		}
		if(totals[row] > 0)
			fprintf(tree,"%7d",(pint)totals[row]);
	}
	fprintf(tree," \n");
	for(col=1; col<=last_seq-first_seq+1; col++) 
		fprintf(tree,"%1d",(pint)tree_description[last_seq-first_seq+1-2][col]);
	fprintf(tree,"\n");
}



int dna_distance_matrix(FILE *tree)
{   
	int m,n,j,i;
	int res1, res2;
        int overspill = 0;
	double p,q,e,a,b,k;	

	tree_gap_delete();  /* flag positions with gaps (tree_gaps[i] = 1 ) */
	
	if(verbose) {
		fprintf(tree,"\n");
		fprintf(tree,"\n DIST   = percentage divergence (/100)");
		fprintf(tree,"\n p      = rate of transition (A <-> G; C <-> T)");
		fprintf(tree,"\n q      = rate of transversion");
		fprintf(tree,"\n Length = number of sites used in comparison");
		fprintf(tree,"\n");
	    if(tossgaps) {
		fprintf(tree,"\n All sites with gaps (in any sequence) deleted!");
		fprintf(tree,"\n");
	    }
	    if(kimura) {
		fprintf(tree,"\n Distances corrected by Kimura's 2 parameter model:");
		fprintf(tree,"\n\n Kimura, M. (1980)");
		fprintf(tree," A simple method for estimating evolutionary ");
		fprintf(tree,"rates of base");
		fprintf(tree,"\n substitutions through comparative studies of ");
		fprintf(tree,"nucleotide sequences.");
		fprintf(tree,"\n J. Mol. Evol., 16, 111-120.");
		fprintf(tree,"\n\n");
	    }
	}

	for(m=1;   m<last_seq-first_seq+1;  ++m)     /* for every pair of sequence */
	for(n=m+1; n<=last_seq-first_seq+1; ++n) {
		p = q = e = 0.0;
		tmat[m][n] = tmat[n][m] = 0.0;
		for(i=1; i<=seqlen_array[first_seq]; ++i) {
			j = boot_positions[i];
                    	if(tossgaps && (tree_gaps[j] > 0) ) 
				goto skip;          /* gap position */
			res1 = seq_array[m+first_seq-1][j];
			res2 = seq_array[n+first_seq-1][j];
			if( (res1 < 0) || (res1 > 4) ||
                            (res2 < 0) || (res2 > 4))  
				goto skip;          /* gap in a seq*/
			e = e + 1.0;
                        if(res1 != res2) {
				if(transition(res1,res2))
					p = p + 1.0;
				else
					q = q + 1.0;
			}
		        skip:;
		}


	/* Kimura's 2 parameter correction for multiple substitutions */

		if(!kimura) {
			if (e == 0) {
				fprintf(stdout,"\n WARNING: sequences %d and %d are non-overlapping\n",m,n);
				k = 0.0;
				p = 0.0;
				q = 0.0;
			}
			else {
				k = (p+q)/e;
				if(p > 0.0)
					p = p/e;
				else
					p = 0.0;
				if(q > 0.0)
					q = q/e;
				else
					q = 0.0;
			}
			tmat[m][n] = tmat[n][m] = k;
			if(verbose)                    /* if screen output */
				fprintf(tree,        
 	     "%4d vs.%4d:  DIST = %7.4f; p = %6.4f; q = %6.4f; length = %6.0f\n"
        	                 ,(pint)m,(pint)n,k,p,q,e);
		}
		else {
			if (e == 0) {
				fprintf(stdout,"\n WARNING: sequences %d and %d are non-overlapping\n",m,n);
				p = 0.0;
				q = 0.0;
			}
			else {
				if(p > 0.0)
					p = p/e;
				else
					p = 0.0;
				if(q > 0.0)
					q = q/e;
				else
					q = 0.0;
			}

			if( ((2.0*p)+q) == 1.0 )
				a = 0.0;
			else
				a = 1.0/(1.0-(2.0*p)-q);

			if( q == 0.5 )
				b = 0.0;
			else
				b = 1.0/(1.0-(2.0*q));

/* watch for values going off the scale for the correction. */
			if( (a<=0.0) || (b<=0.0) ) {
				overspill++;
				k = 3.5;  /* arbitrary high score */ 
			}
			else 
				k = 0.5*log(a) + 0.25*log(b);
			tmat[m][n] = tmat[n][m] = k;
			if(verbose)                      /* if screen output */
	   			fprintf(tree,
             "%4d vs.%4d:  DIST = %7.4f; p = %6.4f; q = %6.4f; length = %6.0f\n"
        	                ,(pint)m,(pint)n,k,p,q,e);

		}
	}
	return overspill;	/* return the number of off-scale values */
}


int prot_distance_matrix(FILE *tree)
{
	int m,n,j,i;
	int res1, res2;
        int overspill = 0;
	double p,e,a,b,k, table_entry;	


	tree_gap_delete();  /* flag positions with gaps (tree_gaps[i] = 1 ) */
	
	if(verbose) {
		fprintf(tree,"\n");
		fprintf(tree,"\n DIST   = percentage divergence (/100)");
		fprintf(tree,"\n Length = number of sites used in comparison");
		fprintf(tree,"\n\n");
	        if(tossgaps) {
			fprintf(tree,"\n All sites with gaps (in any sequence) deleted");
			fprintf(tree,"\n");
		}
	    	if(kimura) {
			fprintf(tree,"\n Distances up tp 0.75 corrected by Kimura's empirical method:");
			fprintf(tree,"\n\n Kimura, M. (1983)");
			fprintf(tree," The Neutral Theory of Molecular Evolution.");
			fprintf(tree,"\n Page 75. Cambridge University Press, Cambridge, England.");
			fprintf(tree,"\n\n");
	    	}
	}

	for(m=1;   m<nseqs;  ++m)     /* for every pair of sequence */
	for(n=m+1; n<=nseqs; ++n) {
		p = e = 0.0;
		tmat[m][n] = tmat[n][m] = 0.0;
		for(i=1; i<=seqlen_array[1]; ++i) {
			j = boot_positions[i];
	            	if(tossgaps && (tree_gaps[j] > 0) ) goto skip; /* gap position */
			res1 = seq_array[m][j];
			res2 = seq_array[n][j];
			if( (res1 < 0)     || (res2 < 0) ||
                            (res1 > max_aa) || (res2 > max_aa)) 
                                    goto skip;   /* gap in a seq*/
			e = e + 1.0;
                        if(res1 != res2) p = p + 1.0;
		        skip:;
		}

		if(p <= 0.0) 
			k = 0.0;
		else
			k = p/e;

/* DES debug */
/* fprintf(stdout,"Seq1=%4d Seq2=%4d  k =%7.4f \n",(pint)m,(pint)n,k); */
/* DES debug */

		if(kimura) {
			if(k < 0.75) { /* use Kimura's formula */
				if(k > 0.0) k = - log(1.0 - k - (k * k/5.0) );
			}
			else {
				if(k > 0.930) {
				   overspill++;
				   k = 10.0; /* arbitrarily set to 1000% */
				}
				else {
				   table_entry = (k*1000.0) - 750.0;
                                   k = (double)dayhoff_pams[(int)table_entry];
                                   k = k/100.0;
				}
			}
		}

		tmat[m][n] = tmat[n][m] = k;
		    if(verbose)                    /* if screen output */
			fprintf(tree,        
 	                 "%4d vs.%4d  DIST = %6.4f;  length = %6.0f\n",
 	                 (pint)m,(pint)n,k,e);
	}
	return overspill;
}


void guide_tree(void)
/* 
   Routine for producing unrooted NJ trees from seperately aligned
   pairwise distances.  This produces the GUIDE DENDROGRAMS in
   PHYLIP format.
*/
{       char path[FILENAMELEN+1];
        int i, j;
        static char **standard_tree;

        verbose = FALSE;
  
        if(empty) {
                error("You must load an alignment first");
                return;
        }

        get_path(seqname,path);

        standard_tree   = (char **) ckalloc( (nseqs+1) * sizeof (char *) );
        for(i=0; i<nseqs+1; i++)
                standard_tree[i]  = (char *) ckalloc( (nseqs+1) * sizeof(char));

        nj_tree(standard_tree,clustal_phy_tree_file);

        print_phylip_tree(standard_tree,phylip_phy_tree_file,FALSE);

        ckfree((void *)left_branch);
        ckfree((void *)right_branch);
        ckfree((void *)kill);
        ckfree((void *)av);
        for (i=1;i<nseqs+1;i++)
                ckfree((void *)standard_tree[i]);
        ckfree((void *)standard_tree);

        fclose(phylip_phy_tree_file);

        fprintf(stdout,"\nGuide tree        file created:   [%s]\n",
		phylip_phy_tree_name);
}

