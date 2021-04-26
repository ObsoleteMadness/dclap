/*
 * CALCPAM.C - Calculate a log-odds matrix for given PAM distance.
 *
 * Copyright David T. Jones, June 1992
 *
 * The basic command syntax is
 *
 * 	calcpam datfile pamdist
 *
 * where pamdist is an integer >= 1
 *
 * References:
 *
 * Jones D.T., Taylor W.R. and Thornton J.M. (1992) "The rapid generation
 * of mutation data matrices from protein sequences". Comput. Appl. Biosci.
 * Vol. 8 No. 3, 275-282.
 *
 * Dayhoff, M. O., Schwartz, R. M. & Orcutt, B. C. (1978).
 * A model of evolutionary change in proteins. In Atlas of Protein Sequence
 * and Structure, Vol. 5, Suppl. 3, Ed. M. O. Dayhoff, pp. 345-352.
 * Natl. Biomed. Res. Found., Washington.
 *
 */

/* Uncomment next line for PC TURBO C */
/* extern unsigned _stklen = 30000; */

#include <stdio.h>
#include <math.h>

char buf[160];

/*	One letter amino acid code	*/

char *rescodes = "ARNDCQEGHILKMFPSTWYVBZX";

/* Normalized frequencies of occurrence */

double freq[20];

/* Relative mutabilities - not actually needed here. */

double mutab[20];

/* Exchange probabilities */

double	pam[20][20], relodds[20][20];

int npams;

#define roundint(x) ((x >= 0) ? (int)(x+0.5) : (int)(x-0.5))
#define skiprems(ifp) while(fgets(buf, 160, ifp) && fgetc(ifp) != ' ');

/* Calculate Relatedness Odds Matrix */
void 
calcrel()
{
    int             i, j;

    for (j = 0; j < 20; j++)
	for (i = 0; i < 20; i++)
	    relodds[i][j] = pam[i][j] / freq[i];
}

/* Raise 1 PAM matrix to given integral power (>= 1). One day I'll
speed this routine up by combining powers of 2. */
void
pampow(mat, power)
double mat[20][20];
int power;
{
    int             i, j, p;
    double          mul[20][20], result[20][20], sum;

    for (i = 0; i < 20; i++)
	for (j = 0; j < 20; j++)
	    mul[i][j] = mat[i][j];

    while (--power)
    {
    	for (i = 0; i < 20; i++)
    	    for (j = 0; j < 20; j++)
	    {
		for (sum = p = 0; p < 20; p++)
		    sum += mul[i][p] * mat[p][j];
		result[i][j] = sum;
	    }

	for (j = 0; j < 20; j++)
	    for (i = 0; i < 20; i++)
		mat[i][j] = result[i][j];
    }
}

FILE *chkopen(name)
char *name;
{
    FILE *f;

    f = fopen(name, "w");
    if (!f)
    {
    	fprintf(stderr, "Unable to open %s for writing!\n", name);
    	exit(1);
    }
    return f;
}

void results()
{
    int i, j;
    char fname[40];
    FILE *ofp;

    sprintf(fname, "mp_%d.mat", npams);
    ofp = chkopen(fname);
    for (i = 0; i < 20; i++)
    {
	for (j = 0; j < 20; j++)
	    fprintf(ofp, "%8.5f ", pam[i][j]);
	fprintf(ofp, "\n");
    }
    fclose(ofp);

    sprintf(fname, "ro_%d.mat", npams);
    ofp = chkopen(fname);
    for (i = 0; i < 20; i++)
    {
	for (j = 0; j < 20; j++)
	    fprintf(ofp, "%9.5f", relodds[i][j]);
	fprintf(ofp, "\n");
    }
    fclose(ofp);

    sprintf(fname, "lo_%d.mat", npams);
    ofp = chkopen(fname);
    for (i = 0; i < 20; i++)
    {
	for (j = 0; j < 20; j++)
	    fprintf(ofp, "%6.1f", 10.0 * log10(relodds[i][j]));
	fprintf(ofp, "\n");
    }
    fclose(ofp);

    sprintf(fname, "md_%d.mat", npams);
    ofp = chkopen(fname);
    for (i = 0; i < 20; i++)
    {
	for (j = 0; j < 20; j++)
	    fprintf(ofp, "%4d", roundint(10.0 * log10(relodds[i][j])));
	fprintf(ofp, "\n");
    }
    fclose(ofp);
}

main(argc, argv)
    int             argc;
    char           *argv[];
{
    int i, j;
    FILE *ifp;

    if (argc != 3)
    {
	fprintf(stderr, "usage: calcpam datfile pamdist\n");
	exit(1);
    }

    sscanf(argv[2], "%d", &npams);
    if (npams < 1)
    {
    	fprintf(stderr, "PAM distance must be > 0!\n");
    	exit(1);
    }

    ifp = fopen(argv[1], "r");
    if (!ifp)
    {
    	fprintf(stderr, "Unable to open datfile!\n");
    	exit(1);
    }

    skiprems(ifp);
    for (i=0; i<20; i++)
    	fscanf(ifp, "%lf", &freq[i]);

    skiprems(ifp);
    for (i=0; i<20; i++)
    	fscanf(ifp, "%lf", &mutab[i]);

    skiprems(ifp);
    for (i=0; i<20; i++)
    	for (j=0; j<20; j++)
	    fscanf(ifp, "%lf", &pam[i][j]);
    fclose(ifp);

    if (npams > 1)
	pampow(pam, npams);
    calcrel();
    results();
}
