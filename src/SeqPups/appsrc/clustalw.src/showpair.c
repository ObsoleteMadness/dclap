#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "clustalw.h"	


/*
*	Prototypes
*/

extern void *ckalloc(size_t);
extern void  ckfree(void *);
extern void error(char *,...);
extern int *seqlen_array;
extern char **seq_array;

void show_pair(void);
static void make_p_ptrs(int *,int *,int,int);
static void make_n_ptrs(int *,int *,int,int);
static void put_frag(int,int,int,int);
static int frag_rel_pos(int,int,int,int);
static void pair_align(int,int,int);
static void des_quick_sort(int *, int *, int);


/*
*	 Global variables
*/
extern int  dna_ktup, dna_window, dna_wind_gap, dna_signif; /* params for DNA */
extern int prot_ktup,prot_window,prot_wind_gap,prot_signif; /* params for prots */
extern int 	nseqs;
extern Boolean 	dnaflag;
extern double 	**tmat;
extern int 	max_aa;

static int 	next;
static int 	curr_frag,maxsf,vatend;
static int 	**accum;
static int 	*diag_index;
static char 	*slopes;

int ktup,window,wind_gap,signif;    		      /* Pairwise aln. params */
static int *displ;
int *zza, *zzb, *zzc, *zzd;

extern Boolean percent;


static void make_p_ptrs(int *tptr,int *pl,int naseq,int l)
{
	static int a[10];
	int i,j,limit,code,flag;
	char residue;
	
	for (i=1;i<=wind_gap;i++)
           a[i] = (int) pow((double)(max_aa+1),(double)(i-1));

	limit = (int) pow((double)(max_aa+1),(double)ktup);
	for(i=1;i<=limit;++i)
		pl[i]=0;
	for(i=1;i<=l;++i)
		tptr[i]=0;
	
	for(i=1;i<=(l-ktup+1);++i) {
		code=0;
		flag=FALSE;
		for(j=1;j<=ktup;++j) {
			residue = seq_array[naseq][i+j-1];
			if((residue<0) || (residue > max_aa)){
				flag=TRUE;
				break;
			}
			code += ((residue) * a[j]);
		}
		if(flag)
			continue;
		++code;
		if(pl[code]!=0)
			tptr[i]=pl[code];
		pl[code]=i;
	}
}


static void make_n_ptrs(int *tptr,int *pl,int naseq,int len)
{
	static int pot[]={ 0, 1, 4, 16, 64, 256, 1024, 4096 };
	int i,j,limit,code,flag;
	char residue;
	
	limit = (int) pow((double)4,(double)ktup);
	
	for(i=1;i<=limit;++i)
		pl[i]=0;
	for(i=1;i<=len;++i)
		tptr[i]=0;
	
	for(i=1;i<=len-ktup+1;++i) {
		code=0;
		flag=FALSE;
		for(j=1;j<=ktup;++j) {
			residue = seq_array[naseq][i+j-1];
			if((residue<0) || (residue>max_aa)){
				flag=TRUE;
				break;
			}
			code += ((residue) * pot[j]);  /* DES */
		}
		if(flag)
			continue;
		++code;
		if(pl[code]!=0)
			tptr[i]=pl[code];
		pl[code]=i;
	}
}


static void put_frag(int fs,int v1,int v2,int flen)
{
	int end;
	
	accum[0][curr_frag]=fs;
	accum[1][curr_frag]=v1;
	accum[2][curr_frag]=v2;
	accum[3][curr_frag]=flen;
	
	if(!maxsf) {
		maxsf=1;
		accum[4][curr_frag]=0;
		return;
	}
	
        if(fs >= accum[0][maxsf]) {
		accum[4][curr_frag]=maxsf;
		maxsf=curr_frag;
		return;
	}
	else {
		next=maxsf;
		while(TRUE) {
			end=next;
			next=accum[4][next];
			if(fs>=accum[0][next])
				break;
		}
		accum[4][curr_frag]=next;
		accum[4][end]=curr_frag;
	}
}


static int frag_rel_pos(int a1,int b1,int a2,int b2)
{
	int ret;
	
	ret=FALSE;
	if(a1-b1==a2-b2) {
		if(a2<a1)
			ret=TRUE;
	}
	else {
		if(a2+ktup-1<a1 && b2+ktup-1<b1)
			ret=TRUE;
	}
	return ret;
}


static void des_quick_sort(int *array1, int *array2, int array_size)
/*  */
/* Quicksort routine, adapted from chapter 4, page 115 of software tools */
/* by Kernighan and Plauger, (1986) */
/* Sort the elements of array1 and sort the */
/* elements of array2 accordingly */
/*  */
{
	int temp1, temp2;
	int p, pivlin;
	int i, j;
	int lst[50], ust[50];       /* the maximum no. of elements must be*/
								/* < log(base2) of 50 */

	lst[1] = 1;
	ust[1] = array_size;
	p = 1;

	while(p > 0) {
		if(lst[p] >= ust[p])
			p--;
		else {
			i = lst[p] - 1;
			j = ust[p];
			pivlin = array1[j];
			while(i < j) {
				for(i=i+1; array1[i] < pivlin; i++)
					;
				for(j=j-1; j > i; j--)
					if(array1[j] <= pivlin) break;
				if(i < j) {
					temp1     = array1[i];
					array1[i] = array1[j];
					array1[j] = temp1;
					
					temp2     = array2[i];
					array2[i] = array2[j];
					array2[j] = temp2;
				}
			}
			
			j = ust[p];

			temp1     = array1[i];
			array1[i] = array1[j];
			array1[j] = temp1;

			temp2     = array2[i];
			array2[i] = array2[j];
			array2[j] = temp2;

			if(i-lst[p] < ust[p] - i) {
				lst[p+1] = lst[p];
				ust[p+1] = i - 1;
				lst[p]   = i + 1;
			}
			else {
				lst[p+1] = i + 1;
				ust[p+1] = ust[p];
				ust[p]   = i - 1;
			}
			p = p + 1;
		}
	}
	return;

}





static void pair_align(int seq_no,int l1,int l2)
{
	int pot[8],i,j,k,l,m,flag,limit,pos,tl1,vn1,vn2,flen,osptr,fs;
	int tv1,tv2,encrypt,subt1,subt2,rmndr;
	char residue;
	
	if(dnaflag) {
		for(i=1;i<=ktup;++i)
			pot[i] = (int) pow((double)4,(double)(i-1));
		limit = (int) pow((double)4,(double)ktup);
	}
	else {
		for (i=1;i<=wind_gap;i++)
           		pot[i] = (int) pow((double)(max_aa+1),(double)(i-1));
		limit = (int) pow((double)(max_aa+1),(double)ktup);
	}
	
	tl1 = (l1+l2)-1;
	
	for(i=1;i<=tl1;++i) {
		slopes[i]=displ[i]=0;
		diag_index[i] = i;
	}
	

/* increment diagonal score for each k_tuple match */

	for(i=1;i<=limit;++i) {
		vn1=zzc[i];
		while(TRUE) {
			if(!vn1) break;
			vn2=zzd[i];
			while(vn2 != 0) {
				osptr=vn1-vn2+l2;
				++displ[osptr];
				vn2=zzb[vn2];
			}
			vn1=zza[vn1];
		}
	}

/* choose the top SIGNIF diagonals */

	des_quick_sort(displ, diag_index, tl1);

	j = tl1 - signif + 1;
	if(j < 1) j = 1;
 
/* flag all diagonals within WINDOW of a top diagonal */

	for(i=tl1; i>=j; i--) 
		if(displ[i] > 0) {
			pos = diag_index[i];
			l = (1  >pos-window) ? 1   : pos-window;
			m = (tl1<pos+window) ? tl1 : pos+window;
			for(; l <= m; l++) 
				slopes[l] = 1;
		}

	for(i=1; i<=tl1; i++)  displ[i] = 0;

	
	curr_frag=maxsf=0;
	
	for(i=1;i<=(l1-ktup+1);++i) {
		encrypt=flag=0;
		for(j=1;j<=ktup;++j) {
			residue = seq_array[seq_no][i+j-1];
			if((residue<0) || (residue>max_aa)) {
				flag=TRUE;
				break;
			}
			encrypt += ((residue)*pot[j]);
		}
		if(flag) continue;
		++encrypt;
	
		vn2=zzd[encrypt];
	
		flag=FALSE;
		while(TRUE) {
			if(!vn2) {
				flag=TRUE;
				break;
			}
			osptr=i-vn2+l2;
			if(slopes[osptr]!=1) {
				vn2=zzb[vn2];
				continue;
			}
			flen=0;
			fs=ktup;
			next=maxsf;
		
		
		/*
		* A-loop
		*/
		
			while(TRUE) {
				if(!next) {
					++curr_frag;
					if(curr_frag>=FSIZE) {
						fprintf(stdout,"(Partial alignment)");
						vatend=1;
						return;
					}
					displ[osptr]=curr_frag;
					put_frag(fs,i,vn2,flen);
				}
				else {
					tv1=accum[1][next];
					tv2=accum[2][next];
					if(frag_rel_pos(i,vn2,tv1,tv2)) {
						if(i-vn2==accum[1][next]-accum[2][next]) {
							if(i>accum[1][next]+(ktup-1))
								fs=accum[0][next]+ktup;
							else {
								rmndr=i-accum[1][next];
								fs=accum[0][next]+rmndr;
							}
							flen=next;
							next=0;
							continue;
						}
						else {
							if(displ[osptr]==0)
								subt1=ktup;
							else {
								if(i>accum[1][displ[osptr]]+(ktup-1))
									subt1=accum[0][displ[osptr]]+ktup;
								else {
									rmndr=i-accum[1][displ[osptr]];
									subt1=accum[0][displ[osptr]]+rmndr;
								}
							}
							subt2=accum[0][next]-wind_gap+ktup;
							if(subt2>subt1) {
								flen=next;
								fs=subt2;
							}
							else {
								flen=displ[osptr];
								fs=subt1;
							}
							next=0;
							continue;
						}
					}
					else {
						next=accum[4][next];
						continue;
					}
				}
				break;
			}
		/*
		* End of Aloop
		*/
		
			vn2=zzb[vn2];
		}
	}
	vatend=0;
}		 

void show_pair(void)
{
	int i,j,k,dsr;
	double calc_score;
	
	accum = (int **)ckalloc( 5*sizeof (int *) );
	for (i=0;i<5;i++)
		accum[i] = (int *) ckalloc(FSIZE * sizeof (int) );

	displ      = (int *) ckalloc( (2*MAXLEN +1) * sizeof (int) );
	slopes     = (char *)ckalloc( (2*MAXLEN +1) * sizeof (char));
	diag_index = (int *) ckalloc( (2*MAXLEN +1) * sizeof (int) );

	zza = (int *)ckalloc( (MAXLEN+1) * sizeof (int) );
	zzb = (int *)ckalloc( (MAXLEN+1) * sizeof (int) );

	zzc = (int *)ckalloc( (MAXLEN+1) * sizeof (int) );
	zzd = (int *)ckalloc( (MAXLEN+1) * sizeof (int) );

        if(dnaflag) {
                ktup     = dna_ktup;
                window   = dna_window;
                signif   = dna_signif;
                wind_gap = dna_wind_gap;
        }
        else {
                ktup     = prot_ktup;
                window   = prot_window;
                signif   = prot_signif;
                wind_gap = prot_wind_gap;
        }

	fprintf(stdout,"\n\n");
	
	for(i=1;i<=nseqs;++i) {
		if(dnaflag)
			make_n_ptrs(zza,zzc,i,seqlen_array[i]);
		else
			make_p_ptrs(zza,zzc,i,seqlen_array[i]);
		for(j=i+1;j<=nseqs;++j) {
			if(dnaflag)
				make_n_ptrs(zzb,zzd,j,seqlen_array[j]);
			else
				make_p_ptrs(zzb,zzd,j,seqlen_array[j]);
			pair_align(i,seqlen_array[i],seqlen_array[j]);
			if(!maxsf)
				calc_score=0.0;
			else {
				calc_score=(double)accum[0][maxsf];
				if(percent) {
					dsr=(seqlen_array[i]<seqlen_array[j]) ?
							seqlen_array[i] : seqlen_array[j];
				calc_score = (calc_score/(double)dsr) * 100.0;
				}
			}
/*
			tmat[i][j]=calc_score;
			tmat[j][i]=calc_score;
*/

                        tmat[i][j] = (100.0 - calc_score)/100.0;
                        tmat[j][i] = (100.0 - calc_score)/100.0;
			if(calc_score>0.1) 
				fprintf(stdout,"Sequences (%d:%d) Aligned. Score: %lg\n",
               			(pint)i,(pint)j,calc_score);
			else
				fprintf(stdout,"Sequences (%d:%d) Not Aligned\n",
						(pint)i,(pint)j);
		}
	}

	for (i=0;i<5;i++)
	   ckfree((void *)accum[i]);
	ckfree((void *)accum);

	ckfree((void *)displ);
	ckfree((void *)slopes);
	ckfree((void *)diag_index);

	ckfree((void *)zza);
	ckfree((void *)zzb);
	ckfree((void *)zzc);
	ckfree((void *)zzd);
}

