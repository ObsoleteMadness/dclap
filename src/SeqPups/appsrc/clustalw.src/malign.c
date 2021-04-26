#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "clustalw.h"

/*
 *       Prototypes
 */
extern  void    *ckalloc(size_t);
extern  void	ckfree(void *);
extern  int  	prfalign(int *, int *);
extern  void    calc_seq_weights(int, int);
extern  int	calc_similarities(int);
extern  void	create_sets(int, int);
extern  void    aln_score(void);
extern  void    clear_tree(treeptr);
extern  int	read_tree(char *, int, int);

int malign(int istart);
int palign1(void);
int palign2(void);
double countid(int s1, int s2);

/*
 *       Global Variables
 */

extern double  **tmat;
extern int     debug;
extern int     max_aa;
extern int     nseqs;
extern int     profile1_nseqs;
extern int     nsets;
extern int     **sets;
extern int     divergence_cutoff;
extern int     *seq_weight;
extern int     output_order, *output_index;
extern Boolean distance_tree;
extern char    phylip_phy_tree_name[];
extern char    seqname[],treename[];
extern int     *seqlen_array;
extern char    **seq_array;

int malign(int istart) /* full progressive alignment*/
{
   static 	int *aligned;
   static 	int *group;
   static 	int ix;

   double 	*maxid, max;
   int 		val,i,j,k,ns,len,set,status,iseq;
   int 		pos,entries,ptr;
   int		score = 0;


   fprintf(stdout,"\nStart of Multiple Alignment\n");

   seq_weight = (int *) ckalloc( (nseqs) * sizeof(int) );

/* get the phylogenetic tree from *.ph */

   if (nseqs > 3) 
     {
       status = read_tree(phylip_phy_tree_name, 0, nseqs);
       if (status == 0) return(0);
     }

/* calculate sequence weights according to branch lengths of the tree -
   weights in global variable seq_weight normalised to sum to 100 */

   calc_seq_weights(0, nseqs);

/* recalculate tmat matrix as percent similarity matrix */

   status = calc_similarities(nseqs);
   if (status == 0) return(0);

/* for each sequence, find the most closely related sequence */

   maxid = (double *)ckalloc( (nseqs+1) * sizeof (double));
   for (i=1;i<=nseqs;i++)
     {
         maxid[i] = 0.0;
         for (j=1;j<=nseqs;j++) 
           if (maxid[i] < tmat[i][j]) maxid[i] = tmat[i][j];
     }

/* group the sequences according to their relative divergence */

   if (istart == 0)
     {
        sets = (int **) ckalloc( (nseqs+1) * sizeof (int *) );
        for(i=0;i<=nseqs;i++)
           sets[i] = (int *)ckalloc( (nseqs+1) * sizeof (int) );

        create_sets(0,nseqs);
        fprintf(stdout,"There are %d groups\n",(pint)nsets);

/* clear the memory used for the phylogenetic tree */

        if (nseqs > 3)
             clear_tree(NULL);

/* start the multiple alignments.........  */

        fprintf(stdout,"Aligning...\n");

/* first pass, align closely related sequences first.... */

        ix = 0;
        aligned = (int *)ckalloc( (nseqs+1) * sizeof (int) );
        for (i=0;i<=nseqs;i++) aligned[i] = 0;

        for(set=1;set<=nsets;++set)
         {
          entries=0;
          for (i=1;i<=nseqs;i++)
            {
               if ((sets[set][i] != 0) && (maxid[i] > divergence_cutoff))
                 {
                    entries++;
                    if  (aligned[i] == 0)
                       {
                          if (output_order==INPUT)
                            {
                              ++ix;
                              output_index[i] = i;
                            }
                          else output_index[++ix] = i;
                          aligned[i] = 1;
                       }
                 }
            }

          score = prfalign(sets[set], aligned);

/* negative score means fatal error... exit now!  */

          if (score < 0) 
             {
                return(-1);
             }
          if ((entries > 0) && (score > 0))
             fprintf(stdout,"Group %d: Sequences:%4d      Score:%d\n",
             (pint)set,(pint)entries,(pint)score);
          else
             fprintf(stdout,"Group %d:                     Delayed\n",
             (pint)set);
        }

        for (i=0;i<=nseqs;i++)
          ckfree((void *)sets[i]);
        ckfree(sets);
     }
   else
     {
/* clear the memory used for the phylogenetic tree */

        if (nseqs > 3)
             clear_tree(NULL);

        aligned = (int *)ckalloc( (nseqs+1) * sizeof (int) );
        ix = 0;
        for (i=1;i<=istart+1;i++)
         {
           aligned[i] = 1;
           ++ix;
           output_index[i] = i;
         }
        for (i=istart+2;i<=nseqs;i++) aligned[i] = 0;
     }

/* second pass - align remaining, more divergent sequences..... */

/* if not all sequences were aligned, for each unaligned sequence,
   find it's closest pair amongst the aligned sequences.  */

    group = (int *)ckalloc( (nseqs+1) * sizeof (int));

    while (ix < nseqs)
      {
        if (ix > 0) 
          {
             for (i=1;i<=nseqs;i++) {
                if (aligned[i] == 0)
                  {
                     maxid[i] = 0.0;
                     for (j=1;j<=nseqs;j++) 
                        if ((maxid[i] < tmat[i][j]) && (aligned[j] != 0))
                            maxid[i] = tmat[i][j];
                  }
              }
          }

/* find the most closely related sequence to those already aligned */

         max = 0;
         for (i=1;i<=nseqs;i++)
           {
             if ((aligned[i] == 0) && (maxid[i] > max))
               {
                  max = maxid[i];
                  iseq = i;
               }
           }

/* align this sequence to the existing alignment */

         entries = 0;
         for (j=1;j<=nseqs;j++)
           if (aligned[j] != 0)
              {
                 group[j] = 1;
                 entries++;
              }
           else if (iseq==j)
              {
                 group[j] = 2;
                 entries++;
              }
         aligned[iseq] = 1;
         score = prfalign(group, aligned);
         fprintf(stdout,"Sequence:%d     Score:%d\n",(pint)iseq,(pint)score);
         if (output_order == INPUT)
          {
            ++ix;
            output_index[iseq] = iseq;
          }
         else
            output_index[++ix] = iseq;
      }

   ckfree((void *)group);
   ckfree((void *)seq_weight);
   ckfree((void *)aligned);
   ckfree((void *)maxid);

   aln_score();

/* make the rest (output stuff) into routine clustal_out in file amenu.c */

   return(nseqs);

}

int palign1(void)  /* a profile alignment */
{
   int 		i,j,k,temp,status;
   int 		entries,score,ptr;
   int 		*aligned, *group;
   double       dscore;

   fprintf(stdout,"\nStart of Initial Alignment\n");

/* calculate sequence weights according to branch lengths of the tree -
   weights in global variable seq_weight normalised to sum to 1000 */

   seq_weight = (int *) ckalloc( (nseqs) * sizeof(int) );

   temp = 1000/nseqs;
   for (i=0;i<nseqs;i++) seq_weight[i] = temp;

   distance_tree = FALSE;

/* do the initial alignment.........  */

   group = (int *)ckalloc( (nseqs+1) * sizeof (int));

   for(i=1; i<=profile1_nseqs; ++i)
         group[i] = 1;
   for(i=profile1_nseqs+1; i<=nseqs; ++i)
         group[i] = 2;
   entries = nseqs;

   aligned = (int *)ckalloc( (nseqs+1) * sizeof (int) );
   for (i=1;i<=nseqs;i++) aligned[i] = 1;

   score = prfalign(group, aligned);
   fprintf(stdout,"Sequences:%d      Score:%d\n",(pint)entries,(pint)score);
   ckfree((void *)group);
   ckfree((void *)seq_weight);
   ckfree((void *)aligned);

   for (i=1;i<=nseqs;i++) {
     for (j=i+1;j<=nseqs;j++) {
       dscore = countid(i,j);
       tmat[i][j] = (100.0 - dscore)/100.0;
       tmat[j][i] = tmat[i][j];
     }
   }

   return(nseqs);
}

double countid(int s1, int s2)
{
   char c1,c2;
   int i;
   int count,total;
   double score;

   count = total = 0;
   for (i=1;i<=seqlen_array[s1];i++) {
     c1 = seq_array[s1][i];
     c2 = seq_array[s2][i];
     if ((c1>=0) && (c1<=max_aa)) {
       total++;
       if (c1 == c2) count++;
     }

   }

   score = 100.0 * (double)count / (double)total;
   return(score);

}
int palign2(void)  /* a profile alignment */
{
   int 		i,j,k,status;
   int 		entries,score,ptr;
   int 		*aligned, *group;

   fprintf(stdout,"\nStart of Multiple Alignment\n");

/* get the phylogenetic tree from *.ph */

   if (nseqs > 3)
     {
        status = read_tree(phylip_phy_tree_name, 0, nseqs);
        if (status == 0) return(0);
     }

/* calculate sequence weights according to branch lengths of the tree -
   weights in global variable seq_weight normalised to sum to 100 */

   seq_weight = (int *) ckalloc( (nseqs) * sizeof(int) );

   calc_seq_weights(0, nseqs);

/* recalculate tmat matrix as percent similarity matrix */

   status = calc_similarities(nseqs);
   if (status == 0) return(0);

/* clear the memory for the phylogenetic tree */

   if (nseqs > 3)
     {
        clear_tree(NULL);
     }

/* do the alignment.........  */

   fprintf(stdout,"Aligning...\n");

   group = (int *)ckalloc( (nseqs+1) * sizeof (int));

   for(i=1; i<=profile1_nseqs; ++i)
         group[i] = 1;
   for(i=profile1_nseqs+1; i<=nseqs; ++i)
         group[i] = 2;
   entries = nseqs;

   aligned = (int *)ckalloc( (nseqs+1) * sizeof (int) );
   for (i=1;i<=nseqs;i++) aligned[i] = 1;

   score = prfalign(group, aligned);
   fprintf(stdout,"Sequences:%d      Score:%d\n",(pint)entries,(pint)score);
   ckfree((void *)group);
   ckfree((void *)seq_weight);
   ckfree((void *)aligned);

/* DES   output_index = (int *)ckalloc( (nseqs+1) * sizeof (int)); */
   for (i=1;i<=nseqs;i++) output_index[i] = i;

   return(nseqs);
}

