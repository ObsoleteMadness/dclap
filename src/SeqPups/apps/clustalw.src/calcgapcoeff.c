#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "clustalw.h"

/*
 *   Prototypes
 */
extern void *ckalloc(size_t);
extern void ckfree(void *);

void calc_p_penalties(char **aln, int n, int fs, int ls, int *weight);
void calc_h_penalties(char **aln, int n, int fs, int ls, int *weight);
int  local_penalty(int penalty, int n, int *pweight, int *hweight);
void calc_gap_coeff(char **alignment, int *gaps, int **profile,
        int first_seq, int last_seq, int prf_length, int gapcoef, int lencoef);

/*
 *   Global variables
 */

extern int gap_dist;
extern int max_aa;
extern int debug;
extern Boolean dnaflag;
extern Boolean use_endgaps;
extern Boolean no_hyd_penalties, no_pref_penalties;
extern char hyd_residues[];
extern char *amino_acid_codes;

char   pr[] =     {'A' , 'C', 'D', 'E', 'F', 'G', 'H', 'K', 'I', 'L',
                   'M' , 'N', 'P', 'Q', 'R', 'S', 'T', 'V', 'Y', 'W'};
int    pas_op[] = { 87, 87,104, 69, 80,139,100,104, 68, 79,
                    71,137,126, 93,128,124,111, 75,100, 77};
int    pas_op2[] ={ 88, 57,111, 98, 75,126, 95, 97, 70, 90,
                    60,122,110,107, 91,125,124, 81,106, 88};
int    pal_op[] = { 84, 69,128, 78, 88,176, 53, 95, 55, 49,
                    52,148,147,100, 91,129,105, 51,128, 88};

float reduced_gap = 0.3;

void calc_gap_coeff(char **alignment, int *gaps, int **profile,
         int first_seq, int last_seq, int prf_length, int gapcoef, int lencoef)
{

   char c;
   int i, j;
   int is, ie;
   static int numseq;
   static int *gap_pos;
   static int *p_weight, *h_weight;
   static float scale;

   numseq = last_seq - first_seq;

   for (j=0; j<prf_length; j++)
        gaps[j] = 0;
          
   for (i=first_seq; i<last_seq; i++)
     {
/*
   Include end gaps as gaps ?
*/
        is = 0;
        ie = prf_length;
        if (use_endgaps == FALSE)
        {
          for (j=0; j<prf_length; j++)
            {
              c = alignment[i][j];
              if ((c < 0) || (c > max_aa))
                 is++;
              else
                 break;
            }
          for (j=prf_length-1; j>=0; j--)
            {
              c = alignment[i][j];
              if ((c < 0) || (c > max_aa))
                 ie--;
              else
                 break;
            }
        }

        for (j=is; j<ie; j++)
          {
              if ((alignment[i][j] < 0) || (alignment[i][j] > max_aa))
                 gaps[j]++;
          }
     }

   if ((!dnaflag) && (no_pref_penalties == FALSE))
     {
        p_weight = (int *) ckalloc( (prf_length+2) * sizeof (int) );
        calc_p_penalties(alignment, prf_length, first_seq, last_seq, p_weight);
     }

   if ((!dnaflag) && (no_hyd_penalties == FALSE))
     {
        h_weight = (int *) ckalloc( (prf_length+2) * sizeof (int) );
        calc_h_penalties(alignment, prf_length, first_seq, last_seq, h_weight);
     }

   gap_pos = (int *) ckalloc( (prf_length+2) * sizeof (int) );
/*
    mark the residues close to an existing gap (set gaps[i] = -ve)
*/
   if (dnaflag || (gap_dist <= 0))
     {
       for (i=0;i<prf_length;i++) gap_pos[i] = gaps[i];
     }
   else
     {
       i=0;
       while (i<prf_length)
         {
            if (gaps[i] <= 0)
              {
                 gap_pos[i] = gaps[i];
                 i++;
              }
            else 
              {
                 for (j = -gap_dist+1; j<0; j++)
                  {
                   if ((i+j>=0) && (i+j<prf_length) &&
                       ((gaps[i+j] == 0) || (gaps[i+j] < j))) gap_pos[i+j] = j;
                  }
                 while (gaps[i] > 0)
                    {
                       if (i>=prf_length) break;
                       gap_pos[i] = gaps[i];
                       i++;
                    }
                 for (j = 0; j<gap_dist; j++)
                  {
                   if (gaps[i+j] > 0) break;
                   if ((i+j>=0) && (i+j<prf_length) && 
                       ((gaps[i+j] == 0) || (gaps[i+j] < -j))) gap_pos[i+j] = -j-1;
                  }
                 i += j;
              }
         }
     }

if (debug>1)
{
fprintf(stderr,"gap open %d gap ext %d\n",(pint)gapcoef,(pint)lencoef);
  for(i=0;i<prf_length;i++) fprintf(stderr,"%d ", (pint)gaps[i]);
  fprintf(stderr,"\n");
  for(i=0;i<prf_length;i++) fprintf(stderr,"%d ", (pint)gap_pos[i]);
  fprintf(stderr,"\n");
}

   for (j=0;j<prf_length; j++)
     {
        if (gap_pos[j] <= 0)
          {
/*
    apply residue-specific and hydrophilic gap penalties.
*/
	     if (!dnaflag) {
              	profile[j+1][GAPCOL] = local_penalty(gapcoef, j,
                                                   p_weight, h_weight);
              	profile[j+1][LENCOL] = lencoef;
	     }
	     else {
              	profile[j+1][GAPCOL] = gapcoef;
              	profile[j+1][LENCOL] = lencoef;
	     }

/*
    increase gap penalty near to existing gaps.
*/
             if (gap_pos[j] < 0)
                {
                    profile[j+1][GAPCOL] *= 2.0+2.0*(gap_dist+gap_pos[j])/gap_dist;
                }


          }
        else
          {
             scale = ((float)(numseq-gaps[j])/(float)numseq) * reduced_gap;
             profile[j+1][GAPCOL] = scale*gapcoef;
             profile[j+1][LENCOL] = 0.5 * lencoef;
          }
     }

   profile[0][GAPCOL] = 0;
   profile[0][LENCOL] = 0;

   profile[prf_length][GAPCOL] = 0;
   profile[prf_length][LENCOL] = 0;

if (debug>0)
{
  fprintf(stderr,"Opening penalties:\n");
  for(i=0;i<prf_length;i++) fprintf(stderr,"%d ", (pint)profile[i+1][GAPCOL]);
  fprintf(stderr,"\n");
}
if (debug>0)
{
  fprintf(stderr,"Extension penalties:\n");
  for(i=0;i<prf_length;i++) fprintf(stderr,"%d ", (pint)profile[i+1][LENCOL]);
  fprintf(stderr,"\n");
}
   if ((!dnaflag) && (no_pref_penalties == FALSE))
        ckfree((void *)p_weight);

   if ((!dnaflag) && (no_hyd_penalties == FALSE))
        ckfree((void *)h_weight);

   ckfree((void *)gap_pos);
}              
            
void calc_p_penalties(char **aln, int n, int fs, int ls, int *weight)
{

  int i,j, k, ix;
  int numseq;

  numseq = ls - fs;
  for (i=0;i<n;i++)
    {
      weight[i] = 0;
      for (k=fs;k<ls;k++)
        {
           for (j=0;j<22;j++)
             {
                ix = aln[k][i];
                if ((ix < 0) || (ix > max_aa)) continue;
                if (amino_acid_codes[ix] == pr[j])
                  {
                    weight[i] += (200-pas_op[j]);
/*
                    weight[i] += (200-(pas_op[j]+pas_op[j+1])/2);
                    weight[i] += ((200-pal_op[j])-100)/2+100;
                    weight[i] += (200-pal_op[j]);
*/
                    break;
                  }
             }
        }
      weight[i] /= numseq;
    }

}
            
void calc_h_penalties(char **aln, int n, int fs, int ls, int *weight)
{

/*
   weight[] is the length of the hydrophilic run of residues.
*/

  int nh, h;
  int i,j, ix, k, e, s, *hyd;
  float scale;

  hyd = (int *)ckalloc((n+2) * sizeof(int));
  nh = strlen(hyd_residues);
  for (i=0;i<n;i++)
     weight[i] = 0;

  for (k=fs;k<ls;k++)
    {
       for (i=0;i<n;i++)
         {
             hyd[i] = 0;
             for (j=0;j<nh;j++)
                {
                   ix = aln[k][i];
                   if ((ix < 0) || (ix > max_aa)) continue;
                   if (amino_acid_codes[ix] == hyd_residues[j])
                      {
                         hyd[i] = 1;
                         break;
                      }
                }
          }
       i = 0;
       while (i < n)
         {
            if (hyd[i] == 0) i++;
            else
              {
                 s = i;
                 while ((hyd[i] != 0) && (i<n)) i++;
                 e = i;
                 if (e-s > 3)
                    for (j=s; j<e; j++) weight[j] += 100;
              }
         }
    }

  scale = ls - fs;
  for (i=0;i<n;i++)
     weight[i] /= scale;

  ckfree((void *)hyd);

if (debug>1)
{
  for(i=0;i<n;i++) fprintf(stderr,"%d ", (pint)weight[i]);
  fprintf(stderr,"\n");
}

}
            
int local_penalty(int penalty, int n, int *pweight, int *hweight)
{

  int i,j, h = FALSE;
  float gw;

  if (dnaflag) return(1);

  gw = 1.0;
  if (no_hyd_penalties == FALSE)
    {
        if (hweight[n] > 0)
         {
           gw *= reduced_gap;
           h = TRUE;
         }
    }
  if ((no_pref_penalties == FALSE) && (h==FALSE))
    {
       gw *= ((float)pweight[n]/100.0);
    }

  gw *= penalty;
  return((int)gw);

}

