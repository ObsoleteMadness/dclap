/* Change int h to int gh everywhere  DES June 1994 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "clustalw.h"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define gap(k)  ((k) <= 0 ? 0 : g + gh * (k))
#define tbgap(k)  ((k) <= 0 ? 0 : tb + gh * (k))
#define tegap(k)  ((k) <= 0 ? 0 : te + gh * (k))


/*
*	Prototypes
*/

extern void *ckalloc(size_t);
extern void *ckfree(void *);
extern int  get_matrix(int *matptr, int *xref, int matrix[NUMRES][NUMRES], int pos_flag);

void pairalign(int istart, int iend, int jstart, int jend);
static double 	tracepath(short sb1, short sb2);
static void 	add(short);
static void 	del(short);
static int   	calc_score(short,short,short,short);
static int   	diff(short,short,short,short,short,short);
static void 	forward_pass(char *ia, char *ib, short n, short m);
static void 	reverse_pass(char *ia, char *ib, short n, short m);

/*
 *   Global variables
 */

extern double   **tmat;
extern float    pw_go_penalty;
extern float    pw_ge_penalty;
extern int 	nseqs;
extern int 	max_aa;
extern int 	*seqlen_array;
extern int 	debug;
extern int  	mat_avscore;
extern int 	blosum30mt[], pam350mt[], idmat[], pw_usermat[];
extern int	blosum45mt[], md_350mt[];
extern int	def_aa_xref[], pw_aa_xref[];
extern Boolean  dnaflag, is_weight;
extern char 	**seq_array;
extern char 	*amino_acid_codes;
extern char 	pw_mtrxname[];

static double 	mm_score;
static int 	print_ptr,last_print;
/* static int 	displ[2*MAXLEN];   */
/* static int 	HH[MAXLEN+1], DD[MAXLEN+1], RR[MAXLEN+1], SS[MAXLEN+1]; */
extern int *displ, *HH, *DD, *RR, *SS;   /* dgg made these extern */

static short 	g, gh;
static int   	seq1, seq2;
static int    	matrix[NUMRES][NUMRES];
static int    	maxscore;
static short    	sb1, sb2, se1, se2;

void pairalign(int istart, int iend, int jstart, int jend)
{
  int	 *mat_xref;
  static short    si, sj, i, j, k;
  static short    n,m,len1,len2;
  static short    length, itemp, end = FALSE;
  static short    maxres;
  static int    *matptr;
  static char   c;
  static int    sum;
  char matfile[FILENAMELEN];

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
                matrix[0][2]=500;
                matrix[2][0]=500;
                matrix[1][3]=500;
                matrix[3][1]=500;
                matrix[1][4]=500;
                matrix[4][1]=500;
             }
         }
    }
  else
    {
if (debug>1) fprintf(stderr,"matrix %s\n",pw_mtrxname);
       if (strcmp(pw_mtrxname, "blosum") == 0)
          {
             matptr = blosum30mt;
             mat_xref = def_aa_xref;
          }
       else if (strcmp(pw_mtrxname, "md") == 0)
          {
             matptr = md_350mt;
             mat_xref = def_aa_xref;
          }
       else if (strcmp(pw_mtrxname, "pam") == 0)
          {
             matptr = pam350mt;
             mat_xref = def_aa_xref;
          }
       else if (strcmp(pw_mtrxname, "id") == 0)
          {
             matptr = idmat;
             mat_xref = def_aa_xref;
          }
       else
          {
             matptr = pw_usermat;
             mat_xref = pw_aa_xref;
          }

       maxres = get_matrix(matptr, mat_xref, matrix, TRUE);
       if (maxres == 0) exit(-1);
    }


  for (si=MAX(0,istart);si<nseqs && si<iend;si++)
   {
     n = seqlen_array[si+1];
     len1 = 0;
     for (i=1;i<=n;i++) {
	c = seq_array[si+1][i];
	if ((c>=0) && (c <= max_aa)) len1++;
     }

     for (sj=MAX(si+1,jstart+1);sj<nseqs && sj<jend;sj++)
      {
        m = seqlen_array[sj+1];
	len2 = 0;
	for (i=1;i<=m;i++) {
		c = seq_array[sj+1][i];
		if ((c>=0) && (c <= max_aa)) len2++;
	}

if (n>MAXLEN) fprintf(stderr,"Error: MAXLEN too small, seq 1 is %d long\n",(pint)n);
if (m>MAXLEN) fprintf(stderr,"Error: MAXLEN too small, seq 1 is %d long\n",(pint)m);

        if (dnaflag) {
           g = 200.0 * (float)pw_go_penalty;
           gh = pw_ge_penalty * 100.0;
        }
        else {
           if (mat_avscore <= 0)
              g = 200.0 * (float)(pw_go_penalty + log((double)(MIN(n,m))));
           else
              g = 2.0 * mat_avscore * (float)(pw_go_penalty +
                    log((double)(MIN(n,m))));
           gh = pw_ge_penalty * 100.0;
        }

if (debug>1) fprintf(stderr,"go %d ge %d\n",(pint)g,(pint)gh);

/*
   align the sequences
*/
        seq1 = si+1;
        seq2 = sj+1;

        forward_pass(&seq_array[seq1][0], &seq_array[seq2][0],
           n, m);

        reverse_pass(&seq_array[seq1][0], &seq_array[seq2][0],
           n, m);

        last_print = 0;
	print_ptr = 1;
/*
        sb1 = sb2 = 1;
        se1 = n-1;
        se2 = m-1;
*/

/* use Myers and Miller to align two sequences */

        maxscore = diff(sb1-1, sb2-1, se1-sb1+1, se2-sb2+1, 
        (short)0, (short)0);

/* calculate percentage residue identity */

        mm_score = tracepath(sb1,sb2);

	mm_score /= (double)MIN(len1,len2);

        tmat[si+1][sj+1] = ((double)100.0 - mm_score)/(double)100.0;
        tmat[sj+1][si+1] = ((double)100.0 - mm_score)/(double)100.0;

if (debug>0)
{
        fprintf(stdout,"Sequences (%d:%d) Aligned. Score: %d CompScore:  %d\n",
                           (pint)si+1,(pint)sj+1, 
                           (pint)mm_score, 
                           (pint)maxscore/(MIN(len1,len2)*100));
}
else
{
        fprintf(stdout,"Sequences (%d:%d) Aligned. Score:  %d\n",
                                      (pint)si+1,(pint)sj+1, 
                                      (pint)mm_score);
}

   }
  }

  return;
}

static void add(short v)
{

        if(last_print<0) {
                displ[print_ptr-1] = v;
                displ[print_ptr++] = last_print;
        }
        else
                last_print = displ[print_ptr++] = v;
}

static int calc_score(short iat,short jat,short v1,short v2)
{
        short ipos,jpos;
	int ret;

        ipos = v1 + iat;
        jpos = v2 + jat;

        ret=matrix[seq_array[seq1][ipos]][seq_array[seq2][jpos]];

	return(ret);
}


static double tracepath(short tsb1, short tsb2)
{
	char c1,c2;
        short  i1,i2;
        short i,j,k,r,pos,to_do;
	short count, total;
	double score;
char s1[MAXLEN*2+1], s2[MAXLEN*2+1];

if ((print_ptr>MAXLEN*2)||(print_ptr<0))
 fprintf(stderr,"Error: displ array too small %d\n",(pint)print_ptr);
        to_do=print_ptr-1;
        i1 = tsb1;
        i2 = tsb2;

	pos = 0;
	count = 0;
        for(i=1;i<=to_do;++i) {
if ((i1>MAXLEN)||(i1<0)) fprintf(stderr,"Error: index i1 %d\n",(pint)i1);
if ((i2>MAXLEN)||(i2<0)) fprintf(stderr,"Error: index i2 %d\n",(pint)i2);
if ((pos>MAXLEN*2)||(pos<0)) fprintf(stderr,"Error: index pos %d\n",(pint)pos);

if (debug>1) fprintf(stderr,"%d ",(pint)displ[i]);
                if(displ[i]==0) {
			c1 = seq_array[seq1][i1];
			c2 = seq_array[seq2][i2];
if (debug>1)
{
if (c1>max_aa) s1[pos] = '-';
else s1[pos]=amino_acid_codes[c1];
if (c2>max_aa) s2[pos] = '-';
else s2[pos]=amino_acid_codes[c2];
}
			if ((c1>=0) && (c1 <= max_aa) &&
                                    (c1 == c2)) count++;
                        ++i1;
                        ++i2;
                        ++pos;
                }
                else {
                        if((k=displ[i])>0) {
if (debug>1)
for (r=0;r<k;r++)
{
s1[pos+r]='-';
if (seq_array[seq2][i2+r]>max_aa) s2[pos+r] = '-';
else s2[pos+r]=amino_acid_codes[seq_array[seq2][i2+r]];
}
                                i2 += k;
                                pos += k;
                                count--;
                        }
                        else {
if (debug>1)
for (r=0;r<(-k);r++)
{
s2[pos+r]='-';
if (seq_array[seq1][i1+r]>max_aa) s1[pos+r] = '-';
else s1[pos+r]=amino_acid_codes[seq_array[seq1][i1+r]];
}
                                i1 -= k;
                                pos -= k;
                                count--;
                        }
                }
        }
if (debug>1) fprintf(stderr,"\n");
if (debug>1) 
{
for (i=0;i<pos;i++) fprintf(stderr,"%c",s1[i]);
fprintf(stderr,"\n");
for (i=0;i<pos;i++) fprintf(stderr,"%c",s2[i]);
fprintf(stderr,"\n");
}

        if (count <= 0) count = 1;
	score = (double)100.0 * (double)count;
	return(score);
}


static void forward_pass(char *ia, char *ib, short n, short m)
{

  short i,j,k;
  int f,hh,p,t;

  maxscore = 0;
  se1 = se2 = 0;
  for (i=0;i<=m;i++)
    {
       HH[i] = 0;
       DD[i] = -g;
    }

  for (i=1;i<=n;i++)
     {
        hh = p = 0;
	f = -g;

        for (j=1;j<=m;j++)
           {

              f -= gh; 
              t = hh - g - gh;
              if (f<t) f = t;

              DD[j] -= gh;
              t = HH[j] - g - gh;
              if (DD[j]<t) DD[j] = t;

              hh = p + matrix[ia[i]][ib[j]];
              if (hh<f) hh = f;
              if (hh<DD[j]) hh = DD[j];
              if (hh<0) hh = 0;

              p = HH[j];
              HH[j] = hh;

              if (hh > maxscore)
                {
                   maxscore = hh;
                   se1 = i;
                   se2 = j;
                }
           }
     }

}


static void reverse_pass(char *ia, char *ib, short n, short m)
{

  short i,j,k;
  int f,hh,p,t;
  int cost;

  cost = 0;
  sb1 = sb2 = 0;
  for (i=se2;i>0;i--)
    {
       HH[i] = -1;
       DD[i] = -1;
    }

  for (i=se1;i>0;i--)
     {
        hh = f = -1;
        if (i == se1) p = 0;
        else p = -1;

        for (j=se2;j>0;j--)
           {

              f -= gh; 
              t = hh - g - gh;
              if (f<t) f = t;

              DD[j] -= gh;
              t = HH[j] - g - gh;
              if (DD[j]<t) DD[j] = t;

              hh = p + matrix[ia[i]][ib[j]];
              if (hh<f) hh = f;
              if (hh<DD[j]) hh = DD[j];

              p = HH[j];
              HH[j] = hh;

              if (hh > cost)
                {
                   cost = hh;
                   sb1 = i;
                   sb2 = j;
                   if (cost >= maxscore) break;
                }
           }
        if (cost >= maxscore) break;
     }

}

static int diff(short A,short B,short M,short N,short tb,short te)
{
        short midi,midj,i,j,type;
        int midh;
        static int f, hh, c, e, s, t;

        if(N<=0)  {
                if(M>0) {
                        del(M);
                }

                return(-tbgap(M));
        }

        if(M<=1) {
                if(M<=0) {
                        add(N);
                        return(-tbgap(N));
                }

                midh = -(tb+gh) - tegap(N);
                hh = -(te+gh) - tbgap(N);
		if (hh>midh) midh = hh;
                midj = 0;
                for(j=1;j<=N;j++) {
                        hh = calc_score(1,j,A,B)
                                    - tegap(N-j) - tbgap(j-1);
                        if(hh>midh) {
                                midh = hh;
                                midj = j;
                        }
                }

                if(midj==0) {
                        del(1);
                        add(N);
                }
                else {
                        if(midj>1)
                                add(midj-1);
                        displ[print_ptr++] = last_print = 0;
                        if(midj<N)
                                add(N-midj);
                }
                return midh;
        }

/* Divide: Find optimum midpoint (midi,midj) of cost midh */

        midi = M / 2;
if (N>MAXLEN) fprintf(stderr,"Error: MAXLEN too small, seq 1 is %d long\n",(pint)N);
if (M>MAXLEN) fprintf(stderr,"Error: MAXLEN too small, seq 2 is %d long\n",(pint)M);
        HH[0] = 0.0;
        t = -tb;
        for(j=1;j<=N;j++) {
                HH[j] = t = t-gh;
                DD[j] = t-g;
        }

        t = -tb;
        for(i=1;i<=midi;i++) {
                s=HH[0];
                HH[0] = hh = t = t-gh;
                f = t-g;
                for(j=1;j<=N;j++) {
                        if ((hh=hh-g-gh) > (f=f-gh)) f=hh;
                        if ((hh=HH[j]-g-gh) > (e=DD[j]-gh)) e=hh;
                        hh = s + calc_score(i,j,A,B);
                        if (f>hh) hh = f;
                        if (e>hh) hh = e;

                        s = HH[j];
                        HH[j] = hh;
                        DD[j] = e;
                }
        }

        DD[0]=HH[0];

        RR[N]=0;
        t = -te;
        for(j=N-1;j>=0;j--) {
                RR[j] = t = t-gh;
                SS[j] = t-g;
        }

        t = -te;
        for(i=M-1;i>=midi;i--) {
                s = RR[N];
                RR[N] = hh = t = t-gh;
                f = t-g;

                for(j=N-1;j>=0;j--) {

                        if ((hh=hh-g-gh) > (f=f-gh)) f=hh;
                        if ((hh=RR[j]-g-gh) > (e=SS[j]-gh)) e=hh;
                        hh = s + calc_score(i+1,j+1,A,B);
                        if (f>hh) hh = f;
                        if (e>hh) hh = e;

                        s = RR[j];
                        RR[j] = hh;
                        SS[j] = e;

                }
        }

        SS[N]=RR[N];

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
                hh = DD[j] + SS[j] + g;
                if(hh>midh) {
                        midh=hh;
                        midj=j;
                        type=2;
                }
        }

        /* Conquer recursively around midpoint  */


        if(type==1) {             /* Type 1 gaps  */
                diff(A,B,midi,midj,tb,g);
                diff(A+midi,B+midj,M-midi,N-midj,g,te);
        }
        else {
                diff(A,B,midi-1,midj,tb,0.0);
                del(2);
                diff(A+midi+1,B+midj,M-midi-1,N-midj,0.0,te);
        }

        return midh;       /* Return the score of the best alignment */
}

static void del(short k)
{
        if(last_print<0)
                last_print = displ[print_ptr-1] -= k;
        else
                last_print = displ[print_ptr++] = -(k);
}


