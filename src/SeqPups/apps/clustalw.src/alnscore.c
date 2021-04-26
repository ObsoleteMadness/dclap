#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "clustalw.h"

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

/*
 *       Prototypes
 */

extern void errout();
extern void *ckalloc(size_t bytes);
extern void ckfree(void *);
extern int  get_matrix(int *matptr, int *xref, int matrix[NUMRES][NUMRES], int neg_flag);

void aln_score(void);
static int  count_gaps(int s1, int s2, int l);

/*
 *       Global Variables
 */

extern float gap_open;
extern int   nseqs;
extern int   *seqlen_array;
extern int   blosum45mt[];
extern int   def_aa_xref[];
extern int   debug;
extern int   max_aa;
extern char  **seq_array;


void aln_score(void)
{
  static int    *mat_xref;
  static int    matrix[NUMRES][NUMRES];
  static int    maxres, ngaps;
  static int    l1,l2,s1,s2,c1,c2;
  static int    score;

  int i;
  int    *matptr;

  matptr = blosum45mt;
  mat_xref = def_aa_xref;
  maxres = get_matrix(matptr, mat_xref, matrix, TRUE);
  if (maxres == 0)
    {
       fprintf(stderr,"Error: matrix blosum30 not found\n");
       return;
    }

  score=0.0;
  for (s1=1;s1<=nseqs;s1++)
   {
    for (s2=1;s2<s1;s2++)
      {

        l1 = seqlen_array[s1];
        l2 = seqlen_array[s2];
        for (i=1;i<l1 && i<l2;i++)
          {
            c1 = seq_array[s1][i];
            c2 = seq_array[s2][i];
            if ((c1>=0) && (c1<=max_aa) && (c2>=0) && (c2<=max_aa))
                score += matrix[c1][c2];
          }

        ngaps = count_gaps(s1, s2, l1);

        score += 100 * gap_open * ngaps;

      }
   }

  score /= 100;

  fprintf(stderr,"\nAlignment Score %d\n", (pint)score);

}

static int count_gaps(int s1, int s2, int l)
{
    int i, g;
    int q, r, *Q, *R;


    Q = (int *)ckalloc((l+2) * sizeof(int));
    R = (int *)ckalloc((l+2) * sizeof(int));

    Q[0] = R[0] = g = 0;

    for (i=1;i<l;i++)
      {
         if (seq_array[s1][i] > max_aa) q = 1;
         else q = 0;
         if (seq_array[s2][i] > max_aa) r = 1;
         else r = 0;

         if ((Q[i-1] <= R[i-1]) && (q != 0) && (1-r != 0) ||
             (Q[i-1] >= R[i-1]) && (1-q != 0) && (r != 0))
             g += 1;
         if (q != 0) Q[i] = Q[i-1]+1;
         else Q[i] = 0;

         if (r != 0) R[i] = R[i-1]+1;
         else R[i] = 0;
     }

   ckfree((void *)Q);
   ckfree((void *)R);

   return(g);
}
          
