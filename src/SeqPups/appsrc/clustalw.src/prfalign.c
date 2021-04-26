#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "clustalw.h"
#define ENDALN 127

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

/*
 *   Prototypes
 */

extern void calc_prf1(int **Profile, char **alignment, int *gaps, 
            int matrix[NUMRES][NUMRES],
             int *weight, int prf_length, int ThisSeq, int LastSeq);
extern void calc_prf2(int **Profile, char **alignment, int *weight,
                      int prf_length, int ThisSeq, int LastSeq);
extern void calc_gap_coeff(char **alignment, int *gaps, int **Profile,
                           int FirstSeq, int LastSeq, int prf_length,
                           int gapcoef, int lencoef);
extern int  get_matrix(int *matptr, int *xref, int matrix[NUMRES][NUMRES], int neg_flag);


extern void *ckalloc(size_t bytes);
extern void *ckrealloc(void *ptr, size_t bytes);
extern void ckfree(void *);

int prfalign(int *group, int *aligned);
static int 	pdiff(short A,short B,short i,short j);
static int 	prfscore(short n, short m);
static int 	gap_penalty1(short i, short j,short k);
static int 	open_penalty1(short i, short j);
static int 	ext_penalty1(short i, short j);
static int 	gap_penalty2(short i, short j,short k);
static int 	open_penalty2(short i, short j);
static int 	ext_penalty2(short i, short j);
static void 	padd(short k);
static void 	pdel(short k);
static void 	palign(void);
static void 	ptracepath(short se1, short se2, short *alen);
static void 	add_ggaps(int n1, int n2);

/*
 *   Global variables
 */
extern double 	**tmat;
extern float 	gap_open, gap_extend;
extern int 	gap_pos1, gap_pos2, max_aa;
extern int 	nseqs;
extern int 	*seqlen_array;
extern int 	*seq_weight;
extern int    	debug;
extern int 	neg_matrix;
extern int 	mat_avscore;
extern int  	blosum30mt[], blosum35mt[], blosum40mt[], blosum45mt[];
extern int  	blosum50mt[], blosum55mt[], blosum62mt2[], blosum70mt[];
extern int  	blosum75mt[], blosum80mt[], blosum85mt[], blosum90mt[];
extern int  	pam20mt[], pam60mt[];
extern int  	pam120mt[], pam160mt[], pam250mt[], pam350mt[];
extern int  	md_350mt[], md_250mt[], md_120mt[], md_40mt[];
extern int  	idmat[], usermat[];
extern int	def_aa_xref[], aa_xref[];
extern Boolean	distance_tree;
extern Boolean	dnaflag, is_weight;
extern char 	mtrxname[];
extern char 	**seq_array;
extern char 	*amino_acid_codes;

static int 	print_ptr,last_print;
/* static int 	displ[2*MAXLEN+1]; */
extern int   *displ;


static char   	**alignment;
static int    	*aln_len;
static int    	*aln_weight;
static char   	*aln_path1, *aln_path2;
static short    	alignment_len;
static int    	**profile1, **profile2;
static int    	matrix[NUMRES][NUMRES];
static int    	nseqs1, nseqs2;
static int    	prf_length1, prf_length2;
static int    	verbose = FALSE;
/* static int    	gaps[MAXLEN]; */
extern int * gaps;

static int    	gapcoef;
static int    	lencoef;

int prfalign(int *group, int *aligned)
{

  static int    i, j, k, count = 0;
  static int    NumSeq,temp,len,insert;
  static int    winlength;
  static short    se1, se2, sb1, sb2;
  static int    maxres;
  static int    *matptr;
  static int	*mat_xref;
  static char   c;
  static char   reply[FILENAMELEN+1];
  static int    score;
  static float  scale,ftemp;
  static double logmin,logdiff;
  static double pcid;


  alignment = (char **) ckalloc( nseqs * sizeof (char *) );
  aln_len = (int *) ckalloc( nseqs * sizeof (int) );
  aln_weight = (int *) ckalloc( nseqs * sizeof (int) );

  for (i=0;i<nseqs;i++)
     if (aligned[i+1] == 0) group[i+1] = 0;

  nseqs1 = nseqs2 = 0;
  for (i=0;i<nseqs;i++)
    {
        if (group[i+1] == 1) nseqs1++;
        else if (group[i+1] == 2) nseqs2++;
    }

  if ((nseqs1 == 0) || (nseqs2 == 0)) return(0.0);

  if (nseqs2 > nseqs1)
    {
     for (i=0;i<nseqs;i++)
       {
          if (group[i+1] == 1) group[i+1] = 2;
          else if (group[i+1] == 2) group[i+1] = 1;
       }
    }

  if (dnaflag)
     {
       for(i=0;i<5;++i)
         {
            for(j=0;j<5;++j) {
                 if(i==j)
                     matrix[i][j]=1000;
                 else
                     matrix[j][i]=0;
             }
            if(is_weight) {
                matrix[1][3]=500;
                matrix[1][4]=500;
                matrix[3][1]=500;
                matrix[4][1]=500;
                matrix[2][0]=500;
                matrix[0][2]=500;
             }
         }
    }
  else
    {

/*
   calculate the mean of the sequence pc identities between the two groups
*/
        count = 0;
        pcid = 0.0;
        for (i=0;i<nseqs;i++)
          {
             if (group[i+1] == 1)
             for (j=0;j<nseqs;j++)
               if (group[j+1] == 2)
                    {
                       count++;
                       if (pcid < tmat[i+1][j+1]) pcid = tmat[i+1][j+1];
/*
                       pcid += tmat[i+1][j+1];
*/
                    }
          }
/*
  pcid = pcid/(float)count;
*/

if (debug>0) fprintf(stderr,"mean tmat %3.1f\n", pcid);

       scale = 0.5;
       if (strcmp(mtrxname, "blosum") == 0)
        {
           if (distance_tree == FALSE) matptr = blosum40mt;
           else if (pcid > 80.0)
             {
                matptr = blosum80mt;
                scale *= -3.5 + pcid/20.0;
             }
           else if (pcid > 60.0)
             {
                matptr = blosum62mt2;
                scale *= -2.5 + pcid/20.0;
             }
           else if (pcid > 30.0)
             {
                matptr = blosum45mt;
                scale *= -0.4 + pcid/30.0;
             }
           else if (pcid > 10.0)
             {
                matptr = blosum30mt;
                scale *= pcid/30.0;
             }
           else
             {
                matptr = blosum30mt;
                scale *= 0.30;
             }
           mat_xref = def_aa_xref;

        }
       else if (strcmp(mtrxname, "md") == 0)
        {
           if (distance_tree == FALSE) matptr = md_250mt;
           else if (pcid > 80.0) matptr = md_40mt;
           else if (pcid > 60.0) matptr = md_120mt;
           else if (pcid > 40.0) matptr = md_250mt;
           else matptr = md_350mt;
           mat_xref = def_aa_xref;
        }
       else if (strcmp(mtrxname, "pam") == 0)
        {
           if (distance_tree == FALSE) matptr = pam120mt;
           else if (pcid > 80.0) matptr = pam20mt;
           else if (pcid > 60.0) matptr = pam60mt;
           else if (pcid > 40.0) matptr = pam120mt;
           else matptr = pam350mt;
           mat_xref = def_aa_xref;
        }
       else if (strcmp(mtrxname, "id") == 0)
        {
           matptr = idmat;
           mat_xref = def_aa_xref;
        }
       else 
        {
           matptr = usermat;
           mat_xref = aa_xref;
        }

      maxres = get_matrix(matptr, mat_xref, matrix, neg_matrix);
      if (maxres == 0)
        {
           fprintf(stderr,"Error: matrix %s not found\n", mtrxname);
           return(-1.0);
        }
    }

/*
  Make the first profile.
*/
  prf_length1 = 0;
  nseqs1 = 0;
  for (i=0;i<nseqs;i++)
    {
       if (group[i+1] == 1)
          {
             len = seqlen_array[i+1];
             alignment[nseqs1] = (char *) ckalloc( (len+2) * sizeof (char) );
             for (j=0;j<len;j++)
               alignment[nseqs1][j] = seq_array[i+1][j+1];
             alignment[nseqs1][j] = ENDALN;
             aln_len[nseqs1] = len;
             aln_weight[nseqs1] = seq_weight[i];
             if (len > prf_length1) prf_length1 = len;
             nseqs1++;
          }
    }

/*
  Make the second profile.
*/
  prf_length2 = 0;
  nseqs2 = 0;
  for (i=0;i<nseqs;i++)
    {
       if (group[i+1] == 2)
          {
             len = seqlen_array[i+1];
             alignment[nseqs1+nseqs2] =
                   (char *) ckalloc( (len+2) * sizeof (char) );
             for (j=0;j<len;j++)
               alignment[nseqs1+nseqs2][j] = seq_array[i+1][j+1];
             alignment[nseqs1+nseqs2][j] = ENDALN;
             aln_len[nseqs1+nseqs2] = len;
             aln_weight[nseqs1+nseqs2] = seq_weight[i];
             if (len > prf_length2) prf_length2 = len;
             nseqs2++;
          }
    }

if (debug>0) fprintf(stderr,"average: %d\n",(pint)mat_avscore);

  logmin = log((double)(MIN(prf_length1,prf_length2)));
  if (prf_length1<=prf_length2)
     logdiff = 1.0-log((double)((float)prf_length1/(float)prf_length2));
  else
     logdiff = 1.0-log((double)((float)prf_length2/(float)prf_length1));

/*
    round logdiff to the nearest integer
*/
  if ((logdiff-(int)logdiff) > 0.5) logdiff=ceil(logdiff);
  else                             logdiff=floor(logdiff);

if (debug>0) fprintf(stderr,"%d %d logmin %f   logdiff %f\n",
(pint)prf_length1,(pint)prf_length2, logmin,logdiff);

  if (dnaflag)
    {
          gapcoef = 100.0 * gap_open;
          lencoef = 100.0 * gap_extend;
    }
  else
    {
     if (neg_matrix == TRUE)
       {
          if (mat_avscore <= 0)
              gapcoef = 200.0 * (gap_open + logmin);
          else
              gapcoef = 200.0 * (gap_open + logmin);
          lencoef = 200.0 * gap_extend * logdiff;
       }
     else
       {
          if (mat_avscore <= 0)
              gapcoef = 100.0 * (float)(gap_open + logmin);
          else
              gapcoef = scale * mat_avscore * (float)(gap_open + logmin);
          lencoef = 100.0 * gap_extend * logdiff;
       }
    }

if (debug>0)
{
fprintf(stderr,"Gap Open %d    Gap Extend %d\n", (pint)gapcoef,(pint)lencoef);
fprintf(stderr,"Window length %d\n", (pint)winlength);
fprintf(stderr,"Matrix  %s\n", mtrxname);
}

  profile1 = (int **) ckalloc( (prf_length1+2) * sizeof (int *) );
  for(i=0; i<prf_length1+2; i++)
       profile1[i] = (int *) ckalloc( (LENCOL+2) * sizeof(int) );

  profile2 = (int **) ckalloc( (prf_length2+2) * sizeof (int *) );
  for(i=0; i<prf_length2+2; i++)
       profile2[i] = (int *) ckalloc( (LENCOL+2) * sizeof(int) );

/*
  calculate the Gap Coefficients.
*/

     calc_gap_coeff(alignment, gaps, profile1,
           0, nseqs1, prf_length1, gapcoef, lencoef);

/*
  calculate the profile matrix.
*/
     calc_prf1(profile1, alignment, gaps, matrix,
          aln_weight, prf_length1, 0, nseqs1);

if (debug>4)
{
extern char *amino_acid_codes;
  for (j=0;j<=max_aa;j++)
    fprintf(stderr,"%c    ", amino_acid_codes[j]);
 fprintf(stderr,"\n");
  for (i=0;i<prf_length1;i++)
   {
    for (j=0;j<=max_aa;j++)
      fprintf(stderr,"%d ", (pint)profile1[i+1][j]);
    fprintf(stderr,"%d ", (pint)profile1[i+1][gap_pos1]);
    fprintf(stderr,"%d ", (pint)profile1[i+1][gap_pos2]);
    fprintf(stderr,"%d %d\n",(pint)profile1[i+1][GAPCOL],(pint)profile1[i+1][LENCOL]);
   }
}

/*
  calculate the Gap Coefficients.
*/

     calc_gap_coeff(alignment, gaps, profile2,
           nseqs1, nseqs1+nseqs2, prf_length2, gapcoef, lencoef);

/*
  calculate the profile matrix.
*/
     calc_prf2(profile2, alignment, aln_weight,
           prf_length2, nseqs1, nseqs1+nseqs2);

  aln_path1 = (char *) ckalloc( (prf_length1 + prf_length2) * sizeof(char) );
  aln_path2 = (char *) ckalloc( (prf_length1 + prf_length2) * sizeof(char) );

if (debug>4)
{
extern char *amino_acid_codes;
  for (j=0;j<=max_aa;j++)
    fprintf(stderr,"%c    ", amino_acid_codes[j]);
 fprintf(stderr,"\n");
  for (i=0;i<prf_length2;i++)
   {
    for (j=0;j<=max_aa;j++)
      fprintf(stderr,"%d ", (pint)profile2[i+1][j]);
    fprintf(stderr,"%d ", (pint)profile2[i+1][gap_pos1]);
    fprintf(stderr,"%d ", (pint)profile2[i+1][gap_pos2]);
    fprintf(stderr,"%d %d\n",(pint)profile2[i+1][GAPCOL],(pint)profile2[i+1][LENCOL]);
   }
}

/*
   align the profiles
*/
/* use Myers and Miller to align two sequences */

  last_print = 0;
  print_ptr = 1;

  sb1 = sb2 = 0;
  se1 = prf_length1;
  se2 = prf_length2;

  score = pdiff(sb1, sb2, se1-sb1, se2-sb2);

  ptracepath(se1, se2, &alignment_len);

  add_ggaps(nseqs1, nseqs2);

  for (i=0;i<prf_length1+2;i++)
     ckfree((void *)profile1[i]);
  ckfree((void *)profile1);

  for (i=0;i<prf_length2+2;i++)
     ckfree((void *)profile2[i]);
  ckfree((void *)profile2);

  prf_length1 = alignment_len;

  ckfree((void *)aln_path1);
  ckfree((void *)aln_path2);

  NumSeq = 0;
  for (j=0;j<nseqs;j++)
    {
       if (group[j+1]  == 1)
         {
            seqlen_array[j+1] = prf_length1;
            for (i=0;i<prf_length1;i++)
              seq_array[j+1][i+1] = alignment[NumSeq][i];
            NumSeq++;
         }
    }
  for (j=0;j<nseqs;j++)
    {
       if (group[j+1]  == 2)
         {
            seqlen_array[j+1] = prf_length1;
            for (i=0;i<prf_length1;i++)
              seq_array[j+1][i+1] = alignment[NumSeq][i];
            NumSeq++;
         }
    }

  for (i=0;i<nseqs1+nseqs2;i++)
     ckfree((void *)alignment[i]);
  ckfree((void *)alignment);

  ckfree((void *)aln_len);

  return(score/100);

}

static void add_ggaps(int n1, int n2)
{
   int i,j,k,ix;
   int len;
   char *ta;

   ta = (char *) ckalloc( MAXLEN * sizeof (char) );

   for (j=0;j<nseqs1;j++)
     {
      ix = 0;
      for (i=0;i<alignment_len;i++)
        {
           if (aln_path1[i] == 2)
              {
                 if (ix < aln_len[j])
                    ta[i] = alignment[j][ix];
                 else 
                    ta[i] = ENDALN;
                 ix++;
              }
           else if (aln_path1[i] == 1)
              {
/*
   insertion in first alignment...
*/
                 ta[i] = gap_pos1;
              }
           else
              {
                 fprintf(stderr,"Error in aln_path\n");
              }
         }
       ta[i] = ENDALN;
       
       len = alignment_len;
       if (len >= MAXLEN)
	  {
             fprintf(stderr,"Error: alignment is too long (Max. %d)\n",MAXLEN);
             exit(1);
          }
       alignment[j] = (char *)ckrealloc(alignment[j], (len+2) * sizeof (char));
       for (i=0;i<len;i++)
         alignment[j][i] = ta[i];
       alignment[j][i] = ENDALN;
       aln_len[j] = len;
      }


   for (j=nseqs1;j<nseqs1+nseqs2;j++)
     {
      ix = 0;
      for (i=0;i<alignment_len;i++)
        {
           if (aln_path2[i] == 2)
              {
                 if (ix < aln_len[j])
                    ta[i] = alignment[j][ix];
                 else 
                    ta[i] = ENDALN;
                 ix++;
              }
           else if (aln_path2[i] == 1)
              {
/*
   insertion in second alignment...
*/
                 ta[i] = gap_pos1;
              }
           else
              {
                 fprintf(stderr,"Error in aln_path\n");
              }
         }
       ta[i] = ENDALN;
       
       ckfree((void *)alignment[j]);
       len = alignment_len;
       alignment[j] = (char *) ckalloc( (len+2) * sizeof (char) );
       for (i=0;i<len;i++)
         alignment[j][i] = ta[i];
       alignment[j][i] = ENDALN;
       aln_len[j] = len;
      }

   ckfree((void *)ta);

if (debug>0)
{
  char c;
  extern char *amino_acid_codes;

   for (i=0;i<nseqs1+nseqs2;i++)
     {
      for (j=0;j<alignment_len;j++)
       {
        if (alignment[i][j] == ENDALN) break;
        else if ((alignment[i][j] < 0) || (alignment[i][j] > max_aa))  c = '-';
        else c = amino_acid_codes[alignment[i][j]];
        fprintf(stderr,"%c", c);
       }
      fprintf(stderr,"\n\n");
     }
}

}                  

static int prfscore(short n, short m)
{
   short    ix, iy;
   int  score;

   score = 0.0;
   for (ix=0; ix<=max_aa; ix++)
     {
         score += (profile1[n][ix] * profile2[m][ix])/10;
     }
   score += (profile1[n][gap_pos1] * profile2[m][gap_pos1])/10;
   score += (profile1[n][gap_pos2] * profile2[m][gap_pos2])/10;
   return(score);
   
}

static void ptracepath(short se1, short se2, short *alen)
{
  short i,j,k,i1,i2,pos,to_do,len;

    pos = 0;

    to_do=print_ptr-1;

    for(i=1;i<=to_do;++i) {
if (debug>0) fprintf(stderr,"%d ",(pint)displ[i]);
            if(displ[i]==0) {
                    aln_path1[pos]=2;
                    aln_path2[pos]=2;
                    ++pos;
            }
            else {
                    if((k=displ[i])>0) {
                            for(j=0;j<=k-1;++j) {
                                    aln_path2[pos+j]=2;
                                    aln_path1[pos+j]=1;
                            }
                            pos += k;
                    }
                    else {
                            k = (displ[i]<0) ? displ[i] * -1 : displ[i];
                            for(j=0;j<=k-1;++j) {
                                    aln_path1[pos+j]=2;
                                    aln_path2[pos+j]=1;
                            }
                            pos += k;
                    }
            }
    }
if (debug>0) fprintf(stderr,"\n");

   (*alen) = pos;

}

static void pdel(short k)
{
        if(last_print<0)
                last_print = displ[print_ptr-1] -= k;
        else
                last_print = displ[print_ptr++] = -(k);
}

static void padd(short k)
{

        if(last_print<0) {
                displ[print_ptr-1] = k;
                displ[print_ptr++] = last_print;
        }
        else
                last_print = displ[print_ptr++] = k;
}

static void palign(void)
{
        displ[print_ptr++] = last_print = 0;
}


/* static int HH[MAXLEN+1], DD[MAXLEN+1], RR[MAXLEN+1], SS[MAXLEN+1]; */
extern int * HH, * DD, * RR, * SS;

static int pdiff(short A,short B,short M,short N)
{
        short midi,midj,type;
        int midh;

        static int t, tl, g, h;

{	static short i,j;
        static int hh, f, e, s;

/* Boundary cases: M <= 1 or N == 0 */
if (debug>2) fprintf(stderr,"A %d B %d M %d N %d midi %d\n", 
(pint)A,(pint)B,(pint)M,(pint)N,(pint)M/2);

/* if sequence B is empty....                                            */

        if(N<=0)  {

/* if sequence A is not empty....                                        */

                if(M>0) {

/* delete residues A[1] to A[M]                                          */

                        pdel(M);
                }

                return(-gap_penalty1(A,B,M));
        }

/* if sequence A is empty....                                            */

        if(M<=1) {
                if(M<=0) {

/* insert residues B[1] to B[N]                                          */

                        padd(N);
                        return(-gap_penalty2(A,B,N));
                }

/* if sequence A has just one residue....                                */

                midh =  -gap_penalty2(A+1,B,1)-gap_penalty1(A+1,B+1,N);
                hh   =  -gap_penalty1(A,B+1,N)-gap_penalty2(A+1,B+N,1);
                if(hh>midh) midh = hh;
                midj = 0;
                for(j=1;j<=N;j++) {
                        hh = -gap_penalty1(A,B+1,j-1) + prfscore(A+1,B+j)
                            -gap_penalty1(A+1,B+j+1,N-j);
                        if(hh>midh) {
                                midh = hh;
                                midj = j;
                        }
                }

                if(midj==0) {
                        pdel(1);
                        padd(N);
                }
                else {
                        if(midj>1) padd(midj-1);
                        palign();
                        if(midj<N) padd(N-midj);
                }
                return midh;
        }


/* Divide sequence A in half: midi */

        midi = M / 2;

/* In a forward phase, calculate all HH[j] and HH[j] */

        HH[0] = 0.0;
        t = -open_penalty1(A,B+1);
        tl = -ext_penalty1(A,B+1);
        for(j=1;j<=N;j++) {
                HH[j] = t = t+tl;
                DD[j] = t-open_penalty2(A+1,B+j);
        }

        t = -open_penalty2(A+1,B);
        tl = -ext_penalty2(A+1,B);
        for(i=1;i<=midi;i++) {
                s = HH[0];
                HH[0] = hh = t = t+tl;
                f = t-open_penalty1(A+i,B+1);

                for(j=1;j<=N;j++) {
                	g = open_penalty1(A+i,B+j);
                	h = ext_penalty1(A+i,B+j);
                        if ((hh=hh-g-h) > (f=f-h)) f=hh;
                	g = open_penalty2(A+i,B+j);
                	h = ext_penalty2(A+i,B+j);
                        if ((hh=HH[j]-g-h) > (e=DD[j]-h)) e=hh;
                        hh = s + prfscore(A+i, B+j);
                        if (f>hh) hh = f;
                        if (e>hh) hh = e;

                        s = HH[j];
                        HH[j] = hh;
                        DD[j] = e;

                }
        }

        DD[0]=HH[0];

/* In a reverse phase, calculate all RR[j] and SS[j] */

        RR[N]=0.0;
        tl = 0.0;
        for(j=N-1;j>=0;j--) {
                g = -open_penalty1(A+M,B+j+1);
                tl -= ext_penalty1(A+M,B+j+1);
                RR[j] = g+tl;
                SS[j] = RR[j]-open_penalty2(A+M,B+j);
        }

        tl = 0.0;
        for(i=M-1;i>=midi;i--) {
                s = RR[N];
                g = -open_penalty2(A+i+1,B+N);
                tl -= ext_penalty2(A+i+1,B+N);
                RR[N] = hh = g+tl;
                t = open_penalty1(A+i,B+N);
                f = RR[N]-t;

                for(j=N-1;j>=0;j--) {
                	g = open_penalty1(A+i,B+j+1);
                	h = ext_penalty1(A+i,B+j+1);
                        if ((hh=hh-g-h) > (f=f-h-g+t)) f=hh;
                        t = g;
                	g = open_penalty2(A+i+1,B+j);
                	h = ext_penalty2(A+i+1,B+j);
                        hh=RR[j]-g-h;
                        if (i==(M-1)) e=SS[j]-h;
                        else e=SS[j]-h-g+open_penalty2(A+i+2,B+j);
                        if (hh > e) e=hh;
                        hh = s + prfscore(A+i+1, B+j+1);
                        if (f>hh) hh = f;
                        if (e>hh) hh = e;

                        s = RR[j];
                        RR[j] = hh;
                        SS[j] = e;

                }
        }
        SS[N]=RR[N];

/* find midj, such that HH[j]+RR[j] or DD[j]+SS[j]+gap is the maximum */

        midh=HH[0]+RR[0];
        midj=0;
        type=1;
        for(j=0;j<=N;j++) {
                hh = HH[j] + RR[j];
                if(hh>=midh)
                        if(hh>midh || HH[j]!=DD[j] && RR[j]==SS[j]) {
                                midh=hh;
                                midj=j;
                        }
        }

        for(j=N;j>=0;j--) {
                g = open_penalty2(A+midi+1,B+j);
                hh = DD[j] + SS[j] + g;
                if(hh>midh) {
                        midh=hh;
                        midj=j;
                        type=2;
                }
        }
}

/* Conquer recursively around midpoint                                   */


        if(type==1) {             /* Type 1 gaps  */
if (debug>2) fprintf(stderr,"Type 1,1: midj %d\n",(pint)midj);
                pdiff(A,B,midi,midj);
if (debug>2) fprintf(stderr,"Type 1,2: midj %d\n",(pint)midj);
                pdiff(A+midi,B+midj,M-midi,N-midj);
        }
        else {
if (debug>2) fprintf(stderr,"setting to 0: midj %d %d\n",(pint)B+midj,(pint) B+midj+1);
                profile2[B+midj][GAPCOL] = 0.0;
if (debug>2) fprintf(stderr,"Type 2,1: midj %d\n",(pint)midj);
                pdiff(A,B,midi-1,midj);
                pdel(2);
if (debug>2) fprintf(stderr,"Type 2,2: midj %d\n",(pint)midj);
                pdiff(A+midi+1,B+midj,M-midi-1,N-midj);
        }

        return midh;       /* Return the score of the best alignment */
}

/* calculate the score for opening a gap at residues A[i] and B[j]       */

static int open_penalty1(short i, short j)
{
   int g;

   if (i==0 || i==prf_length1) return(0);

   g = profile2[j][GAPCOL] + profile1[i][GAPCOL];
   return(g);
}

/* calculate the score for extending an existing gap at A[i] and B[j]    */

static int ext_penalty1(short i, short j)
{
   int h;

   if (i==0 || i==prf_length1) return(0);

   h = profile2[j][LENCOL];
   return(h);
}

/* calculate the score for a gap of length k, at residues A[i] and B[j]  */

static int gap_penalty1(short i, short j, short k)
{
   short ix;
   int gp;
   int g, h;

   if (k <= 0) return(0);
   if (i==0 || i==prf_length1) return(0);

   g = profile2[j][GAPCOL] + profile1[i][GAPCOL];
   for (ix=0;ix<k && ix+j<prf_length2;ix++)
      h = profile2[ix+j][LENCOL];

   gp = g + h * k;
   return(gp);
}
/* calculate the score for opening a gap at residues A[i] and B[j]       */

static int open_penalty2(short i, short j)
{
   int g;

   if (j==0 || j==prf_length2) return(0);

   g = profile1[i][GAPCOL] + profile2[j][GAPCOL];
   return(g);
}

/* calculate the score for extending an existing gap at A[i] and B[j]    */

static int ext_penalty2(short i, short j)
{
   int h;

   if (j==0 || j==prf_length2) return(0);

   h = profile1[i][LENCOL];
   return(h);
}

/* calculate the score for a gap of length k, at residues A[i] and B[j]  */

static int gap_penalty2(short i, short j, short k)
{
   short ix;
   int gp;
   int g, h;

   if (k <= 0) return(0);
   if (j==0 || j==prf_length2) return(0);

   g = profile1[i][GAPCOL] + profile2[j][GAPCOL];
   for (ix=0;ix<k && ix+i<prf_length1;ix++)
      h = profile1[ix+i][LENCOL];

   gp = g + h * k;
   return(gp);
}
