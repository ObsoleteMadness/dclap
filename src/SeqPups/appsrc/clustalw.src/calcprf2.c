#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "clustalw.h"

/*
 *   Prototypes
 */

void calc_prf2(int **profile, char **alignment,
  int *seq_weight,int prf_length, int first_seq, int last_seq);
/*
 *   Global variables
 */

extern int max_aa,gap_pos1,gap_pos2;

void calc_prf2(int **profile, char **alignment,
  int *seq_weight,int prf_length, int first_seq, int last_seq)
{

  int sum1, sum2;	

  int i, d;
  int   r;


  for (r=0; r<prf_length; r++)
    {
/*
   calculate sum2 = number of residues found in this column
*/
       sum2 = 0;
       for (i=first_seq; i<last_seq; i++)
         {
            sum2 += seq_weight[i];
         }
/*
   only include matrix comparison scores for those residue types found in this
   column
*/
       if (sum2 == 0)
         {
           for (d=0; d<=max_aa; d++)
             profile[r+1][d] = 0;
           profile[r+1][gap_pos1] = 0;
           profile[r+1][gap_pos2] = 0;
         }
       else
         {
           for (d=0; d<=max_aa; d++)
             {
                sum1 = 0;
                for (i=first_seq; i<last_seq; i++)
                 {
                  if (d == alignment[i][r]) sum1 += seq_weight[i];
                 }
                profile[r+1][d] = (int)(10 * (float)sum1 / (float)sum2);
             }
           sum1 = 0;
           for (i=first_seq; i<last_seq; i++)
            {
             if (gap_pos1 == alignment[i][r]) sum1 += seq_weight[i];
            }
           profile[r+1][gap_pos1] = (int)(10 * (float)sum1 / (float)sum2);
           sum1 = 0;
           for (i=first_seq; i<last_seq; i++)
            {
             if (gap_pos2 == alignment[i][r]) sum1 += seq_weight[i];
            }
           profile[r+1][gap_pos2] = (int)(10 * (float)sum1 / (float)sum2);
         }
    }
}

