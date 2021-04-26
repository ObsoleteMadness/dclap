

#define CHILDAPP 1  /* dgg - for use w/ seqpup */
#if  defined(__MWERKS__) || defined(THINK_C)  || defined(MPW)
#define MACINTOSH 1
#endif

/*
        PROGRAM LSADT
Fortran->C conversion by Mike Maciukenas, CPGA, Microbiology at University
  of Illinois.
C-----------------------------------------------------------------------
C
C       LEAST SQUARES ALGORITHM FOR FITTING ADDITIVE TREES TO
C       PROXIMITY DATA
C
C       GEERT DE SOETE  --  VERSION 1.01 - FEB. 1983
C                           VERSION 1.02 - JUNE 1983
C                           VERSION 1.03 - JULY 1983
C
C       REFERENCE: DE SOETE, G. A LEAST SQUARES ALGORITHM FOR FITTING
C          ADDITIVE TREES TO PROXIMITY DATA. PSYCHOMETRIKA, 1983, 48,
C          621-626.
C          DE SOETE, G. ADDITIVE TREE REPRESENTATIONS OF INCOMPLETE
C          DISSIMILARITY DATA. QUALITY AND QUANTITY, 1984, 18,
C          387-393.
C       REMARKS
C       -------
C       2) UNIFORMLY DISTRIBUTED RANDOM NUMBERS ARE GENERATED BY A
C          PROCEDURE DUE TO SCHRAGE. CF.
C          SCHRAGE, L.  A MORE PORTABLE FORTRAN RANDOM NUMBER GENERATOR.
C          ACM TRANS. ON MATH. SOFTW., 1979, 5, 132-138.
C       3) SUBROUTINES VA14AD AND VA14AC (translated into minfungra) ARE
C          ADAPTED FROM THE HARWELL SUBROUTINE LIBRARY (1979 EDITION).
C       4) ALTHOUGH THIS PROGRAM HAS BEEN CAREFULLY TESTED, THE
C          AUTHOR DISCLAIMS ANY RESPONSABILITY FOR POSSIBLE
C          ERRORS.
C
C-----------------------------------------------------------------------
*/
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>

#ifdef MACINTOSH
	/* dgg - I think this may be ANSI definition - float.h standard header -- IEEE 754 version */
#include <float.h>
#define MAXDOUBLE 	DBL_MAX
#define MINDOUBLE  DBL_MIN
#else
#include <values.h>
#endif

#define BUFLEN	1024
#define NAMELEN	256
#define MAXLEAVES	256

static int m, n, dissim, pr, start, save, seed, nempty;
static double ps1, ps2, f, empty, tol, c;
static char fname[1000];
static char *names[MAXLEAVES];
static double *delta[MAXLEAVES];
static double **d;
static double **g;
static double **dold;
static FILE *reportf;
static int report;
extern int errno;
double nfac;

extern double strtod();

double dabs(a)
double a;
{
	return((a<0.0) ? -a : a);
}

double sqr(a)
double a;
{
	return(a*a);
}

double max(a, b)
double a;
double b;
{
	return((a>b)?a:b);
}

int imin(a, b)
int a;
int b;
{
	return((a<b)?a:b);
}

double min(a, b)
double a;
double b;
{
	return((a<b)?a:b);
}

float runif()
{
#define A 16807
#define P 2147483647
#define B15 32768
#define B16 65536

	int xhi, xalo, leftlo, fhi, k;
	float t1, t2;
	
	xhi = seed/B16;
	xalo = (seed - (xhi * B16)) * A;
	leftlo = xalo / B16;
	fhi = xhi*A+leftlo;
	k = fhi/B15;
	seed = (((xalo - leftlo*B16) - P) + (fhi - k*B15)*B16) + k;
	if(seed<0) seed += P;
	t1 = seed;
	t2 = t1 * 4.656612875e-10;
	return (t2); /* seed * (1/(2**31-1))*/
}

float rnorm()
{
	static float t1, t2;
	static float n1, n2;
	static int second = 0;

	if(second)
	{
		second = 0;
		n1 = sin(t2);
		n2 = t1*n1;
		n1 = t1*sin(t2);
		return(n2);
	}
	else
	{
		t1 = runif();
		t1 = sqrt(-2.0*log(t1));
		n2 = 6.283185308;
		t2 = runif()*n2;
		n1 = cos(t2);
		n2 = t1*n1;
		n1 = t1*cos(t2);
		second = 1;
		return(n2);
	}
}

double gd(i, j)
int i, j;
{
	if(j<i)
		return(d[i][j]);
	else if(j>i)
		return(d[j][i]);
	else
#if 1
	{
		printf("\n>>>>ERROR:\n>>>>gd: i=j -- programmer screwed up!\n");
		printf("continuing...\n");
		return 0.0;
	}
#else
		show_error("gd: i=j -- programmer screwed up!");
#endif
}

char *repeatch(string, ch, num)
char *string;
int ch;
int num;
{
	for(string[num--] = '\0'; num >= 0; string[num--] = ch);
	return(string);
}

int getachar()
/* skips comments! */
{
	static int oldchar = '\0';
	int ch;
	int more=1;

	while(more)
	{
		ch = getchar();
		if(oldchar == '\n' && ch == '#')
		{
			while(ch!='\n'&&ch!=EOF)
				ch=getchar();
			oldchar = ch;
		}
		else if(oldchar == '\n' && isspace(ch))
			;
		else more=0;
	}
	oldchar = ch;
	return(ch);
}

int skip_space()
{
	int ch;

	while(isspace(ch=getachar()));
	return(ch);
}

int getaword(string, len)
/* 0 if failed, 1 if data was read, -1 if data read to end of file */
char *string;
int len;
{
	int i;
	int ch;
	ch = skip_space();
	if(ch == EOF)
		return(0);
	for(i=0; !isspace(ch) && ch != EOF; i++)
	{
		if(i<len-1)
		{
			string[i] = ch;
		}
		ch = getachar();
	}
	string[i<len?i:len-1] = '\0';
	if(ch==EOF)
		return(-1);
	else
		return(1);
}

int readtobar(string, len)
/* 0 if failed, 1 if data was read */
char *string;
int len;
{
	int i;
	int ch;

	ch = skip_space();
	if(ch==EOF)
		return(0);
	for(i=0; ch!=EOF && ch!='|'; i++)
	{
		if(ch=='\n'||ch=='\r'||ch=='\t')
			i--;
		else
		{
			if(i<len-1)
				string[i]=ch;
		}
		ch=getachar();
	}
	len = i<len?i:len-1;
	for(i=len-1;i>=0 && isspace(string[i]); i--);
	string[i+1] = '\0';
	if(ch==EOF)
		return(-1);
	else
		return(1);
}

int readtobarorcolon(string, len)
/* 0 if failed, 1 if data was read */
char *string;
int len;
{
	int i;
	int ch;

	ch = skip_space();
	if(ch==EOF)
		return(0);
	for(i=0; ch!=EOF && ch!='|' && ch!=':'; i++)
	{
		if(ch=='\n'||ch=='\r'||ch=='\t')
			i--;
		else
		{
			if(i<len-1)
				string[i]=ch;
		}
		ch=getachar();
	}
	len = i<len?i:len-1;
	for(i=len-1;i>=0 && isspace(string[i]); i--);
	string[i+1] = '\0';
	if(ch==EOF)
		return(-1);
	else
		return(1);
}

char *getmem(nelem, elsize)
unsigned nelem, elsize;
{
	char *temp;

	temp = (char *)calloc(nelem+1, elsize);
	if(temp == NULL) show_error("Couldn't allocate memory.");
	else return(temp);
}

int get_parms(argc, argv)
int argc;
char **argv;
{
	int i;
	int cur_arg;

	/* codes for current argument:
	** 0 = no current argument
	** 1 = pr
	** 2 = start
	** 3 = seed
	** 4 = ps1
	** 5 = ps2
	** 6 = empty
	** 7 = filename */

	dissim = 0;
	pr = 0;
	start = 2;
	save = 0;
	seed = 12345;
	ps1 = 0.0001;
	ps2 = 0.0001;
	empty = 0.0;
	n = 0;
	cur_arg = 0;
	for(i=1; i<argc; i++)
		switch(cur_arg) {
		case 0:
			if(strcmp(argv[i],"-d")==0)
				dissim = 0;
			else if(strcmp(argv[i],"-s")==0)
				dissim = 1;
			else if(strcmp(argv[i],"-print")==0)
				cur_arg = 1;
			else if(strcmp(argv[i],"-init")==0)
				cur_arg = 2;
			else if(strcmp(argv[i],"-save")==0)
				save = 1;
			else if(strcmp(argv[i],"-seed")==0)
				cur_arg = 3;
			else if(strcmp(argv[i],"-ps1")==0)
				cur_arg = 4;
			else if(strcmp(argv[i],"-ps2")==0)
				cur_arg = 5;
			else if(strcmp(argv[i],"-empty")==0)
				cur_arg = 6;
			else if(strcmp(argv[i],"-f")==0)
				cur_arg = 7;
			else if(strcmp(argv[i],"-help")==0)
				show_help();
			else
			if (argv[i] && *argv[i]) /* dgg added */
			{
				printf("\nargc= %d, argi= %d",argc, i);
				printf("\nlsadt - bad option: '%s'\n",argv[i]);
				show_help();
			}
			break;
		case 1:
			pr = atoi(argv[i]);
			cur_arg = 0;
			break;
		case 2:
			start = atoi(argv[i]);
			cur_arg = 0;
			break;
		case 3:
			seed = atoi(argv[i]);
			cur_arg = 0;
			break;
		case 4:
			ps1 = atof(argv[i]);
			cur_arg = 0;
			break;
		case 5:
			ps2 = atof(argv[i]);
			cur_arg = 0;
			break;
		case 6:
			empty = atof(argv[i]);
			cur_arg = 0;
			break;
		case 7:
			strcpy(fname, argv[i]);
			cur_arg = 0;
			break;
		default:
			printf("\nlsadt: bad option\n");
			show_help();
			break;
		}


/* check validity of parameters */
	if(ps1<0.0) ps1 = 0.0001;
	if(ps2<0.0) ps2 = 0.0001;
	if(dissim<0) dissim = 0;
	if(start < 1 || start > 3) start = 3;
	if(save != 1) save = 0;
	if(seed < 0) seed = 12345;

/*printf("dissim=%d\n", dissim);*/
/*printf("pr=%d\n", pr);*/
/*printf("start=%d\n", start);*/
/*printf("save=%d\n", save);*/
/*printf("seed=%d\n", seed);*/
/*printf("ps1=%f\n", ps1);*/
/*printf("ps2=%f\n", ps2);*/
/*printf("empty=%f\n", empty);*/
}

int get_data()
{
	int i, j, more;
	char buf[BUFLEN];
	char *ptr;
	char ch;
	int result;
	double temp, nfactor, datmin, datmax;
	

	nempty = n = 0;
	more = 1;
	ptr = &ch;
	while(more)
	{
		result=readtobarorcolon(buf, BUFLEN);
		if(result == 0 || result == -1)
			more = 0;
		else
		{
			n++;
			names[n] = getmem(NAMELEN, 1);
			result=readtobar(buf, NAMELEN);
			if(result != 1)
				show_error("get_data: bad name syntax, or missing '|'");
			strcpy(names[n], buf);
			if(n>1)
				delta[n]=(double *)getmem(n, sizeof(double));
			else
				delta[n]=NULL;
			for(j=1; j<n; j++)
			{
				if(!getaword(buf, BUFLEN))
					show_error("get_data: missing data values.\n");
				delta[n][j] = strtod(buf, &ptr);
				if(ptr == buf)
					show_error("get_data: invalid data value.\n");
				if(delta[n][j] == empty) nempty++;
			}
		}
	}
	if(n<4) show_error("Must be at least 4 nodes.");
	m = n * (n - 1) / 2;
	/* make all positive */
	datmin = MAXDOUBLE;
	for(i=2; i<=n; i++)
		for(j=1; j<i; j++)
			if(delta[i][j] < datmin && delta[i][j] != empty)
				datmin = delta[i][j];
	if(datmin < 0.0)
		for(i=2; i<=n; i++)
			for(j=1; j<i; j++)
				if(delta[i][j] != empty)
					delta[i][j] -= datmin;
	/* convert dissimilarities into similarities if -s option specified */
	if(dissim)
	{
		datmax = MINDOUBLE;
		for(i=2; i<=n; i++)
			for(j=1; j<i; j++)
				if(delta[i][j] > datmax && delta[i][j] != empty)
					datmax = delta[i][j];
		datmax += 1.0;
		for(i=2; i<=n; i++)
			for(j=1; j<i; j++)
				if(delta[i][j] != empty)
					delta[i][j] = datmax - delta[i][j];
	}


	/* make sum of squared deviations from delta to average(delta) = 1 */
	temp = 0.0;
	for(i=2; i<=n; i++)
		for(j=1; j<i; j++)
			if(delta[i][j] != empty)
				temp += delta[i][j];
	temp = temp/(double)(m-nempty);
	/* now temp = average of all non-empty cells */
	nfactor = 0.0;
	for(i=2; i<=n; i++)
		for(j=1; j<i; j++)
			if(delta[i][j] != empty)
				nfactor+=sqr(delta[i][j]-temp);
	nfactor = 1.0/sqrt(nfactor);
	nfac = nfactor;
	/* now nfactor is the normalization factor */
	if(report)
		fprintf(reportf, "Normalization factor: %15.10f\n", nfactor);
	for(i=2; i<=n; i++)
		for(j=1; j<i; j++)
			if(delta[i][j] != empty)
				delta[i][j] *= nfactor;
	/* now all is normalized */
}

int initd()
{

	int i, j, k;
	double t1, t2;
	int emp, nemp;
	double dmax;
	double efac, estd;


	efac = 0.333333333333333;
	if(start == 1)
	{
		for(i=2; i<=n; i++)
			for(j=1; j<i; j++)
				d[i][j] = runif();
		return;
	}
	if(nempty == 0 && start == 2)
	{
		estd = sqrt(efac/m);
		for(i=2; i<=n; i++)
			for(j=1; j<i; j++)
				d[i][j] = delta[i][j] + rnorm()*estd;
		return;
	}
	for(i=2; i<=n; i++)
		for(j=1; j<i; j++)
			d[i][j] = delta[i][j];
	if(start==3 && nempty==0) return;
	emp=nempty;
	nemp=0;
	while(nemp<emp)
		for(i=2; i<=n; i++)
			for(j=1; j<i; j++)
				if(d[i][j] == empty)
				{
					dmax = MAXDOUBLE;
					for(k=1; k<=n; k++)
					{
						t1 = gd(i,k);
						t2 = gd(j,k);
						if(t1!=empty && t2!=empty)
						  dmax = min(dmax, max(t1,t2));
					}
					if(dmax!=MAXDOUBLE)
						d[i][j] = dmax;
					else
						nemp++;
				}
	if(nemp!=0)
	{
		t1 = 0.0;
		for(i=2; i<=n; i++)
			for(j=1; j<i; j++)
				if(d[i][j]!=empty)
					t1+=d[i][j];
		t1 /= m-nemp;
		for(i=2; i<=n; i++)
			for(j=1; j<i; j++)
				if(d[i][j]==empty)
					d[i][j]=t1;
	}
	if(start!=3)
	{
		estd=sqrt(efac/(m-nempty));
		for(i=2; i<=n; i++)
			for(j=1; j<i; j++)
				d[i][j]+= rnorm()*estd;
	}
	return;
}

static int nm0, nm1, nm2, nm3;
static double fitl, fitp, r;

fungra()
{
	double fac;
	int i, j, k, l;
	double wijkl;

	for(i=2;i<=n;i++)
		for(j=1;j<i;j++)
			g[i][j] = 0.0;
	fitl = 0.0;
	fac = 2.0;
	for(i=2;i<=n;i++)
		for(j=1;j<i;j++)
			if(delta[i][j] != empty)
			{
				fitl += sqr(d[i][j] - delta[i][j]);
				g[i][j] += fac*(d[i][j] - delta[i][j]);
			}
	fitp = 0.0;
	fac = 2.0*r;
	for(i=1;i<=nm3;i++)
		for(j=i+1;j<=nm2;j++)
			for(k=j+1;k<=nm1;k++)
				for(l=k+1;l<=nm0;l++)
					if((d[j][i]+d[l][k]>=d[l][i]+d[k][j])&&
					   (d[k][i]+d[l][j]>=d[l][i]+d[k][j]))
					{
						wijkl=d[j][i]+d[l][k]
							-d[k][i]-d[l][j];
						fitp+=sqr(wijkl);
						g[j][i]+=fac*wijkl;
						g[l][k]+=fac*wijkl;
						g[k][i]-=fac*wijkl;
						g[l][j]-=fac*wijkl;
					}
					else
					if((d[j][i]+d[l][k]>=d[k][i]+d[l][j])&&
					   (d[l][i]+d[k][j]>=d[k][i]+d[l][j]))
					{
						wijkl=d[j][i]+d[l][k]
							-d[l][i]-d[k][j];
						fitp+=sqr(wijkl);
						g[j][i]+=fac*wijkl;
						g[l][k]+=fac*wijkl;
						g[k][j]-=fac*wijkl;
						g[l][i]-=fac*wijkl;
					}
					else
					if((d[k][i]+d[l][j]>=d[j][i]+d[l][k])&&
					   (d[l][i]+d[k][j]>=d[j][i]+d[l][k]))
					{
						wijkl=d[k][i]+d[l][j]
							-d[l][i]-d[k][j];
						fitp+=sqr(wijkl);
						g[k][i]+=fac*wijkl;
						g[l][j]+=fac*wijkl;
						g[l][i]-=fac*wijkl;
						g[k][j]-=fac*wijkl;
					}
	f = fitl+r*fitp;
}

static double **dr, **dgr, **d1, **gs, **xx, **gg;
static int iterc, prc;
print_iter(maxfnc, f)
int maxfnc;
double f;
{
	int i, j;

	if(pr == 0)
	{
		iterc++;
	}
	else if(prc < abs(pr))
	{
		prc++;
		iterc++;
	}
	else
	{
		printf("Iteration %6d", iterc);
		printf(": function values %6d", maxfnc);
		printf(" f = %24.16e\n", f);
		if(pr < 0)
		{
			printf("  d[] looks like this:\n");
			for(i=2;i<=n;i++)
			{
				printf("  ");
				for(j=1;j<i;j++)
					printf("%24.16e ", d[i][j]);
				printf("\n  ");
			}
			printf("  g[] looks like this:\n");
			for(i=2;i<=n;i++)
			{
				printf("  ");
				for(j=1;j<i;j++)
					printf("%24.16e ", g[i][j]);
				printf("\n  ");
			}
		}
		prc = 1;
		iterc++;
	}
}

minfungra(dfn, acc, maxfn)
double dfn, acc;
int maxfn;
{
	double beta, c, clt, dginit, dgstep, dtest, finit;
	double fmin, gamden, gamma, ggstar, gm, gmin, gnew;
	double gsinit, gsumsq, xbound, xmin, xnew, xold;
	int itcrs, flag, retry, maxfnk, maxfnc;
	int i, j;

	flag = 0;
	retry = -2;
	iterc = 0;
	maxfnc = 1;
	prc = abs(pr);
	goto L190;

	L10:
	xnew = dabs(dfn+dfn)/gsumsq;
	dgstep = -gsumsq;
	itcrs = m+1;
	L30:
	if(maxfnk<maxfnc)
	{
		f = fmin;
		for(i=2;i<=n;i++)
			for(j=1;j<i;j++)
			{
				d[i][j] = xx[i][j];
				g[i][j] = gg[i][j];
			}
	}
	if(flag > 0)
	{
		prc = 0;
		print_iter(maxfnc, f);
		return;
	}
	if(itcrs>m)
	{
		for(i=2;i<=n;i++)
			for(j=1;j<i;j++)
				d1[i][j] = -g[i][j];
				}
	print_iter(maxfnc, f);
	dginit = 0.0;
	for(i=2;i<=n;i++)
		for(j=1;j<i;j++)
		{
			gs[i][j] = g[i][j];
			dginit += d1[i][j]*g[i][j];
		}
	if(dginit >= 0.0)
	{
		retry = -retry;
		if(imin(retry, maxfnk)<2)
		{
			printf("minfungra: \
				gradient wrong or acc too small\n");
			flag = 2;
		}
		else
			itcrs = m+1;
		goto L30;
	}
	xmin = 0.0;
	fmin = f;
	finit = f;
	gsinit = gsumsq;
	gmin = dginit;
	gm = dginit;
	xbound = -1.0;
	xnew = xnew * min(1.0, dgstep/dginit);
	dgstep = dginit;
	L170:
	c = xnew-xmin;
	dtest = 0.0;
	for(i=1;i<=n;i++)
		for(j=1;j<i;j++)
		{
			d[i][j] = xx[i][j]+c*d1[i][j];
			dtest += dabs(d[i][j]-xx[i][j]);
		}
	if(dtest<=0.0)
	{
		clt = .7;
		goto L300;
	}
	if(max(xbound, xmin-c) < 0.0) {
	clt = 0.1;
	}
	maxfnc++;
	L190:
	fungra();

	if(maxfnc>1)
	{
		for(gnew = 0.0,i=1;i<=n;i++)
			for(j=1;j<i;j++)
				gnew += d1[i][j]*g[i][j];
				}
	if(maxfnc<=1 ||
	   (maxfnc>1 && f<fmin) ||
	   (maxfnc>1 && f==fmin && dabs(gnew) <= dabs(gmin)))
	{
		maxfnk = maxfnc;
		gsumsq = 0.0;
		for(i=1;i<=n;i++)
			for(j=1;j<i;j++)
				gsumsq += sqr(g[i][j]);
		if(gsumsq <= acc)
		{
			prc = 0;
			print_iter(maxfnc, f);
			return;
		}
	}
	if(maxfnc==maxfn)
	{
		printf("minfungra: \
			maxfn function values have been calculated\n");
		flag = 1;
		goto L30;
	}
	if(maxfnk < maxfnc) goto L300;
	for(i=1;i<=n;i++)
		for(j=1;j<i;j++)
		{
			xx[i][j] = d[i][j];
			gg[i][j] = g[i][j];
		}
	if(maxfnc <= 1) goto L10;
	gm = gnew;
	if(gm<=dginit) goto L310;
	ggstar = 0.0;
	for(i=1;i<=n;i++)
		for(j=1;j<i;j++)
			ggstar += gs[i][j]*g[i][j];
	beta = (gsumsq-ggstar)/(gm-dginit);
	L300:
	if(dabs(gm)>clt*dabs(dginit) ||
	   (dabs(gm)<=clt*dabs(dginit) && dabs(gm*beta) >= clt*gsumsq))
	{
		L310:
		clt += 0.3;
		if(clt>0.8)
		{
			retry = -retry;
			if(imin(retry, maxfnk)<2)
			{
				printf("minfungra: \
					gradient wrong or acc too small\n");
				flag = 2;
			}
			else
				itcrs = m+1;
			goto L30;
		}
		xold = xnew;
		xnew = .5*(xmin+xold);
		if(maxfnk >= maxfnc && gmin*gnew > 0.0)
		{
			xnew = 10.0*xold;
			if(xbound>=0.0) {
			xnew = 0.5*(xold+xbound);
			}
		}
		c = gnew-(3.0*gnew + gmin-4.0*(f-fmin)/(xold-xmin))*
			 (xold-xnew)/(xold-xmin);
		if(maxfnk>=maxfnc)
		{
			if(gmin*gnew<=0.0) {
			xbound = xmin;
			}
			xmin = xold;
			fmin = f;
			gmin = gnew;
		}
		else
			xbound = xold;
		if(c*gmin < 0.0)
			xnew = (xmin*c-xnew*gmin)/(c-gmin);
		goto L170;
	}
	if(min(f, fmin)<finit ||
	   (min(f, fmin)>=finit && gsumsq < gsinit))
	{
		if(itcrs<m && dabs(ggstar) < .2*gsumsq)
		{
			gamma = 0.0;
			c = 0.0;
			for(i=1;i<=n;i++)
				for(j=1;j<i;j++)
				{
					gamma += gg[i][j] * dgr[i][j];
					c += gg[i][j]*dr[i][j];
				}
			gamma /= gamden;
			if(dabs(beta*gm+gamma*c)<.2*gsumsq)
			{
				for(i=1;i<=n;i++)
					for(j=1;j<i;j++)
						d1[i][j] = -gg[i][j] +
								beta*d1[i][j] +
								gamma*dr[i][j];
				itcrs++;
			}
			else
			{
				itcrs = 2;
				gamden = gm-dginit;
				for(i=1;i<=n;i++)
					for(j=1;j<i;j++)
					{
					    dr[i][j] = d1[i][j];
					    dgr[i][j] = gg[i][j]-gs[i][j];
					    d1[i][j] = -gg[i][j]+beta*d1[i][j];
					}
			}
		}
		else
		{
			itcrs = 2;
			gamden = gm-dginit;
			for(i=1;i<=n;i++)
				for(j=1;j<i;j++)
				{
					dr[i][j] = d1[i][j];
					dgr[i][j] = gg[i][j]-gs[i][j];
					d1[i][j] = -gg[i][j]+beta*d1[i][j];
				}
		}
		goto L30;
	}
	else 
	{
		retry = -retry;
		if(imin(retry, maxfnk)<2)
		{
			printf("minfungra: \
				gradient wrong or acc too small\n");
			flag = 2;
		}
		else
			itcrs = m+1;
		goto L30;
	}
}

get_dist()
{
	static double cons = 1.0;
	static int maxfn = 500;
	int i, j, iter;
	double dif;


	dr = (double **)getmem(n, sizeof(delta[1]));
	dgr = (double **)getmem(n, sizeof(delta[1]));
	d1 = (double **)getmem(n, sizeof(delta[1]));
	gs = (double **)getmem(n, sizeof(delta[1]));
	xx = (double **)getmem(n, sizeof(delta[1]));
	gg = (double **)getmem(n, sizeof(delta[1]));
	dr[1] = NULL;
	dgr[1] = NULL;
	d1[1] = NULL;
	gs[1] = NULL;
	xx[1] = NULL;
	gg[1] = NULL;
	for(i=2; i<=n; i++)
	{
		dr[i] = (double *)getmem(i-1, sizeof(double));
		dgr[i] = (double *)getmem(i-1, sizeof(double));
		d1[i] = (double *)getmem(i-1, sizeof(double));
		gs[i] = (double *)getmem(i-1, sizeof(double));
		xx[i] = (double *)getmem(i-1, sizeof(double));
		gg[i] = (double *)getmem(i-1, sizeof(double));
	}
	nm0 = n;
	nm1 = n-1;
	nm2 = n-2;
	nm3 = n-3;
	if(start==3)
	{
		r = 0.001;
		fungra();
	}
	else
	{
		r = 1.0;
		fungra();
		r = cons*fitl/fitp;
		f = (1.0+cons)*fitl;
	}
	iter=1;
	do
	{
		for(i=1;i<=n;i++)
			for(j=1;j<i;j++)
				dold[i][j] = d[i][j];
		minfungra(0.5*f, ps1, maxfn);
		dif = 0.0;
		for(i=1;i<=n;i++)
			for(j=1;j<i;j++)
				dif +=sqr(d[i][j] - dold[i][j]);
		dif = sqrt(dif);
		if(dif>ps2)
		{
			iter++;
			r*=10.0;
		}
	} while (dif>ps2);
	fungra();
	for(i=2; i<=n; i++)
	{
		free(dr[i]);
		free(dgr[i]);
		free(d1[i]);
		free(gs[i]);
		free(xx[i]);
		free(gg[i]);
	}
	free(dr);
	free(dgr);
	free(d1);
	free(gs);
	free(xx);
	free(gg);
}

double gttol()
{
	double result;
	int i, j, k, l;

	result = 0.0;
	nm0 = n;
	nm1 = n-1;
	nm2 = n-2;
	nm3 = n-3;
	for(i=1;i<=nm3;i++)
	    for(j=i+1;j<=nm2;j++)
		for(k=j+1;k<=nm1;k++)
		    for(l=k+1;l<=nm0;l++)
			if((d[j][i]+d[l][k]>=d[l][i]+d[k][j])&&
			   (d[k][i]+d[l][j]>=d[l][i]+d[k][j]))
				result=max(result,
				    dabs(d[j][i]+d[l][k]-d[k][i]-d[l][j]));
			else
			if((d[j][i]+d[l][k]>=d[k][i]+d[l][j])&&
			   (d[l][i]+d[k][j]>=d[k][i]+d[l][j]))
				result=max(result,
				    dabs(d[j][i]+d[l][k]-d[l][i]-d[k][j]));
			else
			if((d[k][i]+d[l][j]>=d[j][i]+d[l][k])&&
			   (d[l][i]+d[k][j]>=d[j][i]+d[l][k]))
				result=max(result,
				    dabs(d[k][i]+d[l][j]-d[l][i]-d[k][j]));
	return(result);
}

gtcord()
{
	double sumx, sumy, ssqx, ssqy, scp, fn;
	int i, j;

	sumx = sumy = ssqx = ssqy = scp = 0.0;
	for(i=1;i<=n;i++)
		for(j=1;j<i;j++)
			if(delta[i][j]!=empty)
			{
				sumx+=delta[i][j];
				sumy+=d[i][j];
				ssqx+=sqr(delta[i][j]);
				ssqy+=sqr(d[i][j]);
				scp+=delta[i][j]*d[i][j];
			}
	fn=(double)(m-nempty);
	if((fn*ssqy-sumx*sumy)<=0.0)
		show_error("gtcord: path length distances have zero variance");
	else
		return((fn*scp-sumx*sumy)/
			sqrt((fn*ssqx-sqr(sumx))*(fn*ssqy-sqr(sumy))));
}

goodfit()
{
	double r, rsq;

	r=gtcord();
	rsq=r*r*100.0;
	tol=gttol();
}

additive_const()
{
	int i, j, k;

	c=0.0;
	for(i=1;i<=n;i++)
		for(j=1;j<i;j++)
			for(k=1;k<=n;k++)
				if(k!=i && k!=j)
					c=max(c,gd( i, j)-
						gd( i, k)-
						gd( j, k));
	if(report)
		fprintf(reportf, "Additive Constant: %15.10f\n", c);
	if(c!=0.0)
		for(i=1;i<=n;i++)
			for(j=1;j<i;j++)
				d[i][j]+=c;
}

static int *mergei, *mergej;
static int ninv;
get_tree()
{
#define false 0
#define true 1
	int i, j, k, l, im, jm, km, lm, count;
	int maxnode, maxcnt, nnode, nact;
	double arci, arcj, arcim, arcjm;
	char *act;

	maxnode = 2*n-2;
	mergei = (int *)getmem(maxnode, sizeof(int));
	mergej = (int *)getmem(maxnode, sizeof(int));
	act = (char *)getmem(maxnode, sizeof(char));
	for(i=1;i<=maxnode;i++)
		act[i] = false;
	nact=n;
	ninv=0;
	nnode=n;
	while(nact>4)
	{
	    maxcnt=0;
	    for(i=1;i<=nnode;i++) if(!act[i])
		for(j=1;j<i;j++) if(!act[j])
		{
		    count=0;
		    arci=0.0;
		    arcj=0.0;
		    for(k=2;k<=nnode;k++) if(!act[k] && k!=i && k!=j)
			for(l=1;l<k;l++) if(!act[l] && l!= i && l!= j)
			    if(gd(i,j)+gd(k,l)<=gd(i,k)+gd(j,l) &&
				gd(i,j)+gd(k,l)<=gd(i,l)+gd(j,k))
			    {
			        count++;
			        arci+=(gd(i,j)+gd(i,k)-gd(j,k))/2.0;
			        arcj+=(gd(i,j)+gd(j,l)-gd(i,l))/2.0;
			    }
		    if(count>maxcnt)
		    {
			maxcnt = count;
			arcim=max(0.0, arci/count);
			arcjm=max(0.0, arcj/count);
			im=i;
			jm=j;
		    }
		}
		
	    nnode++;
	    if(nnode+2>maxnode)
		    show_error("get_tree: number of nodes exceeds 2N-2");
	    ninv++;
	    mergei[ninv]=im;
	    mergej[ninv]=jm;
	    act[im]=true;
	    act[jm]=true;
	    d[nnode]=(double *)getmem(nnode-1, sizeof(double));
	    d[nnode][im]=arcim;
	    d[nnode][jm]=arcjm;
	    for(i=1;i<=nnode-1;i++)
		    if(!act[i])
			    d[nnode][i] = max(0.0, gd(im,i)-arcim);
	    nact--;
	}

	for(i=1;act[i];i++)
	    if(i>nnode)
		show_error("get_tree: can't find last two invisible nodes");
	im=i;
	for(i=im+1;act[i];i++)
	    if(i>nnode)
		show_error("get_tree: can't find last two invisible nodes");
	jm=i;
	for(i=jm+1;act[i];i++)
	    if(i>nnode)
		show_error("get_tree: can't find last two invisible nodes");
	km=i;
	for(i=km+1;act[i];i++)
	    if(i>nnode)
		show_error("get_tree: can't find last two invisible nodes");
	lm=i;
	if(gd(im,jm)+gd(km,lm)<=gd(im,km)+gd(jm,lm)+tol &&
	   gd(im,jm)+gd(km,lm)<=gd(im,lm)+gd(jm,km)+tol)
	{
		i=im;
		j=jm;
		k=km;
		l=lm;
	}
	else if(gd(im,lm)+gd(jm,km)<=gd(im,km)+gd(jm,lm)+tol &&
	        gd(im,lm)+gd(jm,km)<=gd(im,jm)+gd(km,lm)+tol)
	{
		i=im;
		j=lm;
		k=km;
		l=jm;
	}
	else if(gd(im,km)+gd(jm,lm)<=gd(im,jm)+gd(km,lm)+tol &&
	        gd(im,km)+gd(jm,lm)<=gd(im,lm)+gd(jm,km)+tol)
	{
		i=im;
		j=km;
		k=lm;
		l=jm;
	}

	nnode++;
	ninv++;
	mergei[ninv]=i;
	mergej[ninv]=j;
	d[nnode]=(double *)getmem(nnode-1, sizeof(double));
	d[nnode][i] = max(0.0, (gd(i,j)+gd(i,k)-gd(j,k))/2.0);
	d[nnode][j] = max(0.0, (gd(i,j)+gd(j,l)-gd(i,l))/2.0);
	nnode++;
	ninv++;
	mergei[ninv]=k;
	mergej[ninv]=l;
	d[nnode]=(double *)getmem(nnode-1, sizeof(double));
	d[nnode][k] = max(0.0, (gd(k,l)+gd(i,k)-gd(i,l))/2.0);
	d[nnode][l] = max(0.0, (gd(k,l)+gd(j,l)-gd(j,k))/2.0);
	d[nnode][nnode-1] = max(0.0, (gd(i,k)+gd(j,l)-gd(i,j)-gd(k,l))/2.0);

}

print_node(node, dist, indent)
int node;
double dist;
int indent;
{
	static char buf[BUFLEN];

	if(node<=n)
		printf("%s%s:%6.4f",
			repeatch(buf, '\t', indent),
			names[node],
			dist/nfac);
	else
	{
		printf("%s(\n",
			repeatch(buf, '\t', indent));
		print_node(mergei[node-n], gd(node, mergei[node-n]), indent+1);
		printf(",\n");
		print_node(mergej[node-n], gd(node, mergej[node-n]), indent+1);
		printf("\n%s):%6.4f", repeatch(buf, '\t', indent),
			dist/nfac);
	}
}

show_tree()
{
	int i, j, current;
	int ij[2];

	current=0;
	for(i=1;current<2;i++)
	{
		for(j=1;(mergei[j]!=i && mergej[j] != i) && j<=ninv;j++);
		if(j>ninv)
			ij[current++]=i;
	}
	printf("(\n");
	print_node(ij[0], gd(ij[0],ij[1])/2.0, 1);
	printf(",\n");
	print_node(ij[1], gd(ij[0],ij[1])/2.0, 1);
	printf("\n);\n");
}

show_help()
{
	printf("\nlsadt--options:\n");
	printf("  -f file  - write report to 'file'\n");
	printf("  -d       - treat data as dissimilarities (default)\n");
	printf("  -s       - treat data as similarities\n");
	printf("  -print 0 - don't print out iteration history (default)\n");
	printf("  -print n>0 - print out iteration history every n iterations\n");
	printf("  -print n<0 - print out iteration history every n iterations\n");
	printf("               (including current distance estimates & gradients)\n");
	printf("  -init  n - initial parameter estimates (default = 3)\n");
	printf("         n=1 - uniformly distributed random numbers\n");
	printf("         n=2 - error-perturbed data\n");
	printf("         n=3 - original distance data from input matrix\n");
	printf("  -save    - save final parameter estimates (default is don't save)\n");
	printf("  -seed  n - seed for random number generator (default = 12345)\n");
	printf("  -ps1   n - convergence criterion for inner iterations (default = 0.0001)\n");
	printf("  -ps2   n - convergence criterion for major iterations (default = 0.0001)\n");
	printf("  -empty n - missing data indicator (default = 0.0)\n");
	printf("  -help    - show this help text\n");
	exit(0);
}

show_error(message)
char *message;
{
	printf("\n>>>>ERROR:\n>>>>%s\n", message);
	exit(0);
}

#ifdef CHILDAPP
int RealMain(int argc, char** argv)
#else
main(argc, argv)
int argc;
char **argv;
#endif
{
	int i;

	strcpy(fname, "");
	get_parms(argc, argv);
	if(strcmp(fname, ""))
	{
		report=1;
		reportf = fopen(fname, "w");
		if(reportf==NULL)
		{
			perror("lsadt");
			exit(0);
		}

	}
	else
		report=0;
	get_data();
	d = (double **)getmem(n, sizeof(delta[1]));
	g = (double **)getmem(n, sizeof(delta[1]));
	dold = (double **)getmem(n, sizeof(delta[1]));
	d[1]=NULL;
	g[1]=NULL;
	dold[1]=NULL;
	for(i=2; i<=n; i++)
	{
		d[i]=(double *)getmem(i-1, sizeof(double));
		g[i]=(double *)getmem(i-1, sizeof(double));
		dold[i]=(double *)getmem(i-1, sizeof(double));
	}
	initd();
	get_dist();
	goodfit();
	additive_const();
	get_tree();
	show_tree();
	if(report)
		close(reportf);
}
