#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "clustalw.h"

/*
 *   Prototypes
 */

extern void *ckalloc(size_t bytes);
extern void ckfree(void *);

void calc_prf1(int **profile, char **alignment, int *gaps,
  int matrix[NUMRES][NUMRES],
  int *seq_weight, int prf_length, int first_seq, int last_seq);

/*
 *   Global variables
 */

extern int max_aa,gap_pos1,gap_pos2;

void calc_prf1(int **profile, char **alignment, int *gaps,
  int matrix[NUMRES][NUMRES],
  int *seq_weight, int prf_length, int first_seq, int last_seq)
{

  int **weighting; 
  int f, sum2;		
  float scale;

  int i, d, pos, res, count;
  int   r, numseq;

  weighting = (int **) ckalloc( (NUMRES+2) * sizeof (int *) );
  for (i=0;i<NUMRES+2;i++)
    weighting[i] = (int *) ckalloc( (prf_length+2) * sizeof (int) );

  numseq = last_seq-first_seq;

  sum2 = 0.0;
  for (i=first_seq; i<last_seq; i++)
       sum2 += seq_weight[i];

  for (r=0; r<prf_length; r++)
   {
      for (d=0; d<=max_aa; d++)
        {
            weighting[d][r] = 0;
            for (i=first_seq; i<last_seq; i++)
               if (d == alignment[i][r]) weighting[d][r] += seq_weight[i];
        }
      weighting[gap_pos1][r] = 0;
      for (i=first_seq; i<last_seq; i++)
         if (gap_pos1 == alignment[i][r]) weighting[gap_pos1][r] += seq_weight[i];
      weighting[gap_pos2][r] = 0;
      for (i=first_seq; i<last_seq; i++)
         if (gap_pos2 == alignment[i][r]) weighting[gap_pos2][r] += seq_weight[i];
   }

  for (pos=0; pos< prf_length; pos++)
    {
      if (gaps[pos] == numseq)
        {
           for (res=0; res<=max_aa; res++)
             {
                profile[pos+1][res] = matrix[res][gap_pos1];
             }
           profile[pos+1][gap_pos1] = matrix[gap_pos1][gap_pos1];
           profile[pos+1][gap_pos2] = matrix[gap_pos2][gap_pos1];
        }
      else
        {
           scale = (float)(numseq-gaps[pos]) / (float)numseq;
           for (res=0; res<=max_aa; res++)
             {
                f = 0.0;
                for (d=0; d<=max_aa; d++)
                     f += (weighting[d][pos] * matrix[d][res]);
                f += (weighting[gap_pos1][pos] * matrix[gap_pos1][res]);
                f += (weighting[gap_pos2][pos] * matrix[gap_pos2][res]);
                profile[pos+1][res] = (int  )(((float)f / (float)sum2)*scale);
             }
           f = 0.0;
           for (d=0; d<=max_aa; d++)
                f += (weighting[d][pos] * matrix[d][gap_pos1]);
           f += (weighting[gap_pos1][pos] * matrix[gap_pos1][gap_pos1]);
           f += (weighting[gap_pos2][pos] * matrix[gap_pos2][gap_pos1]);
           profile[pos+1][gap_pos1] = (int )(((float)f / (float)sum2)*scale);
           f = 0.0;
           for (d=0; d<=max_aa; d++)
                f += (weighting[d][pos] * matrix[d][gap_pos2]);
           f += (weighting[gap_pos1][pos] * matrix[gap_pos1][gap_pos2]);
           f += (weighting[gap_pos2][pos] * matrix[gap_pos2][gap_pos2]);
           profile[pos+1][gap_pos2] = (int )(((float)f / (float)sum2)*scale);
        }
    }

  for (i=0;i<=max_aa;i++)
    ckfree((void *)weighting[i]);
  ckfree((void *)weighting);

}

