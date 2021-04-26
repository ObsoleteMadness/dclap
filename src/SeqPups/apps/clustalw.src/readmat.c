#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "clustalw.h"
#include "matrices.h"

/*
 *   Prototypes
 */

void 	init_matrix(void);
int 	get_matrix(int *matptr, int *xref,
                     int matrix[NUMRES][NUMRES], int neg_flag);
int 	read_user_matrix(char *filename, int *usermat, int *xref);
int 	getargs(char *inline1,char *args[],int max);

/*
 *   Global variables
 */

extern char 	*amino_acid_codes;
extern int 	gap_pos1, gap_pos2;
extern int 	max_aa;
extern int 	def_aa_xref[], aa_xref[], pw_aa_xref[];
extern int 	usermat[], pw_usermat[];
extern int 	mat_avscore;
extern int 	debug;

void init_matrix(void)
{

   char c1,c2;
   int i, j, maxres;

   max_aa = strlen(amino_acid_codes)-2;
   gap_pos1 = max_aa + 1;          /* code for gaps inserted by clustalw */
   gap_pos2 = gap_pos1 + 1;           /* code for gaps already in alignment */

/*
   set up cross-reference for default matrices hard-coded in matrices.h
*/
   for (i=0;i<NUMRES;i++) def_aa_xref[i] = -1;

   maxres = 0;
   for (i=0;c1=amino_acid_order[i];i++)
     {
         for (j=0;c2=amino_acid_codes[j];j++)
          {
           if (c1 == c2)
               {
                  def_aa_xref[i] = j;
                  maxres++;
                  break;
               }
          }
         if ((def_aa_xref[i] == -1) && (amino_acid_order[i] != '*'))
            {
                fprintf(stderr,
                "Warning: residue %c in matrices.h is not recognised\n",
                                       amino_acid_order[i]);
            }
     }

}

int get_matrix(int *matptr, int *xref, int matrix[NUMRES][NUMRES], int neg_flag)
{
   double f;
   int gg_score = 1;
   int gr_score = 0;
   int i, j, k, ix = 0;
   int ti, tj;
   int  maxres, min;
   int av1,av2,av3, max;

/*
   default - set all scores to 0
*/
   for (i=0;i<=max_aa;i++)
      for (j=0;j<=max_aa;j++)
          matrix[i][j] = 0;

   ix = 0;
   maxres = 0;
   for (i=0;i<=max_aa;i++)
    {
      ti = xref[i];
      for (j=0;j<=i;j++)
       {
          tj = xref[j]; 
          if ((ti != -1) && (tj != -1))
            {
               k = matptr[ix];
               if (ti==tj)
                  {
                     matrix[ti][ti] = k * 100;
                     maxres++;
                  }
               else
                  {
                     matrix[ti][tj] = k * 100;
                     matrix[tj][ti] = k * 100;
                  }
               ix++;
            }
       }
    }

   av1 = av2 = av3 = 0;
   for (i=0;i<=max_aa;i++)
    {
      for (j=0;j<=i;j++)
       {
           av1 += matrix[i][j];
           if (i==j)
              {
                 av2 += matrix[i][j];
              }
           else
              {
                 av3 += matrix[i][j];
              }
       }
    }

   --maxres;
   av1 /= (maxres*maxres)/2;
   av2 /= maxres;
   av3 /= ((float)(maxres*maxres-maxres))/2;
if (debug>1) fprintf(stderr,"average mismatch score %d\n",(pint)av3);
if (debug>1) fprintf(stderr,"average match score %d\n",(pint)av2);
if (debug>1) fprintf(stderr,"average score %d\n",(pint)av1);

  min = max = matrix[0][0];
  for (i=0;i<=max_aa;i++)
    for (j=1;j<=i;j++)
      {
        if (matrix[i][j] < min) min = matrix[i][j];
        if (matrix[i][j] > max) max = matrix[i][j];
      }
/*
   if requested, make a positive matrix - add -(lowest score) to every entry
*/
  if (neg_flag == FALSE)
   {

if (debug>1) fprintf(stderr,"min %d max %d\n",(pint)min,(pint)max);
      if (min < 0)
        {
           for (i=0;i<=max_aa;i++)
            {
              ti = xref[i];
              if (ti != -1)
                {
                 for (j=0;j<=max_aa;j++)
                   {
                    tj = xref[j];
/*
                    if (tj != -1) matrix[ti][tj] -= (2*av3);
*/
                    if (tj != -1) matrix[ti][tj] -= min;
                   }
                }
            }
        }
/*
       gr_score = av3;
       gg_score = -av3;
*/

   }

  mat_avscore = -av3;


  for (i=0;i<gap_pos1;i++)
   {
      matrix[i][gap_pos1] = gr_score;
      matrix[gap_pos1][i] = gr_score;
      matrix[i][gap_pos2] = gr_score;
      matrix[gap_pos2][i] = gr_score;
   }
  matrix[gap_pos1][gap_pos1] = gg_score;
  matrix[gap_pos2][gap_pos2] = gg_score;
  matrix[gap_pos2][gap_pos1] = gg_score;
  matrix[gap_pos2][gap_pos1] = gg_score;

  maxres += 2;

  return(maxres);
}


int read_user_matrix(char *filename, int *usermat, int *xref)
{
   double f;
   int gg_score = 0;
   int  numargs;
   int i, j, k, ix1, ix = 0;
   FILE *fd;
   char codes[NUMRES];
   char inline1[1024];
   char *args[NUMRES+4];
   char c1,c2;
   int  maxres, min;
   int sum;

   if (filename[0] == '\0')
     {
        fprintf(stderr,"Error: comparison matrix not specified\n");
     }
   else
     {
        if ((fd=fopen(filename,"r"))==NULL) 
          {
             fprintf(stderr,"Error: cannot open %s\n", filename);
             return(0);
          }

        maxres = 0;
        while (fgets(inline1,1024,fd) != NULL)
          {
             if ((inline1[0] == '\0') || (inline1[0] == '#')) continue;
/*
   read residue characters.
*/
             k = 0;
             for (j=0;j<strlen(inline1);j++)
               {
                  if (isalpha((int)inline1[j])) codes[k++] = inline1[j];
                  if (k>NUMRES)
                     {
                        fprintf(stderr,"Error: too many entries in %s\n",filename);
                        return(0);
                     }
               }
             codes[k] = '\0';
             break;
          }

        if (k == 0) 
          {
             fprintf(stderr,"Error: wrong format in %s\n",filename);
             return(0);
          }

/*
   cross-reference the residues
*/
   for (i=0;i<NUMRES;i++) xref[i] = -1;

   maxres = 0;
   for (i=0;c1=codes[i];i++)
     {
         for (j=0;c2=amino_acid_codes[j];j++)
           if (c1 == c2)
               {
                  xref[i] = j;
                  maxres++;
                  break;
               }
         if ((xref[i] == -1) && (codes[i] != '*'))
            {
                fprintf(stderr,"Warning: residue %c in %s not recognised\n",
                                       codes[i],filename);
            }
     }


/*
   get the weights
*/

        ix = ix1 = 0;
        while (fgets(inline1,1024,fd) != NULL)
          {
             if (inline1[0] == '\n') continue;
             numargs = getargs(inline1, args, maxres+1);
             if (numargs == 0)
               {
                  fprintf(stderr,"Error: wrong format in %s\n", filename);
                  return(0);
               }
             for (i=0;i<=ix;i++)
               {
                  if (xref[i] != -1)
                    {
                       f = atof(args[i]);
                       usermat[ix1++] = (int)(f);
                    }
               }
             ix++;
          }
        if (ix != maxres+1)
          {
             fprintf(stderr,"Error: wrong format in %s\n", filename);
             return(0);
          }
     }

    fclose(fd);

  maxres += 2;

  return(maxres);
}

int getargs(char *inline1,char *args[],int max)
{

	char	*inptr;
#ifndef MAC
	char	*strtok(char *s1, const char *s2);
#endif
	int	i;

	inptr=inline1;
	for (i=0;i<=max;i++)
	{
		if ((args[i]=strtok(inptr," \t\n"))==NULL)
			break;
		inptr=NULL;
	}
	if (i!=max) return(0);

	return(i);
}

