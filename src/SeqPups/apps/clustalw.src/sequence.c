/********* Sequence input routines for CLUSTAL W *******************/
/* DES was here.  FEB. 1994 */
/* Now reads PILEUP/MSF and CLUSTAL alignment files */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "clustalw.h"	

#define MIN(a,b) ((a)<(b)?(a):(b))

/*
*	Prototypes
*/

extern Boolean linetype(char *,char *);
extern Boolean blankline(char *);
extern void warning(char *,...);
extern void error(char *,...);
extern char *	rtrim(char *);
extern char *	blank_to_(char *);
extern void 	getstr(char *,char *);

void fill_chartab(void);
int readseqs(int);

static void         get_seq(char *,char *,int *,char *);
static void get_clustal_seq(char *,char *,int *,char *,int);
static void     get_msf_seq(char *,char *,int *,char *,int);
static void check_infile(int *);
static void p_encode(char *, char *, int);
static void n_encode(char *, char *, int);
static int res_index(char *,char);
static Boolean check_dnaflag(char *, int);
static int count_clustal_seqs(void);
static int count_msf_seqs(void);

/*
 *	Global variables
 */

extern FILE *fin;
extern Boolean usemenu, dnaflag, explicit_dnaflag;
extern char seqname[];
extern int nseqs;
extern int *seqlen_array;
extern int *output_index;
extern char **names,**titles;
extern char **seq_array;
extern profile1_empty, profile2_empty;
extern int max_aa;
extern int gap_pos2;

char *amino_acid_codes   =    "ABCDEFGHIKLMNPQRSTUVWXYZ-";  /* DES */
char *nucleic_acid_order = 	  "ACGTUN";
static int seqFormat;
static char chartab[128];
static char *formatNames[] = {"unknown","EMBL/Swiss-Prot","PIR",
			      "Pearson","GDE","Clustal","Pileup/MSF","USER"};

void fill_chartab(void)	/* Create translation and check table */
{
	register int i;
	register char c;
	
	for(i=0;i<128;chartab[i++]=0);
	for(i=0;c=amino_acid_codes[i];i++)
		chartab[c]=chartab[tolower(c)]=c;
}

static void get_msf_seq(char *sname,char *seq,int *len,char *tit,int seqno)
/* read the seqno_th. sequence from a PILEUP multiple alignment file */
{
	static char line[MAXLINE+1];
	int i,j,k;
	unsigned char c;

	fseek(fin,0,0); 		/* start at the beginning */

	*len=0;				/* initialise length to zero */
        for(i=0;;i++) {
		if(fgets(line,MAXLINE+1,fin)==NULL) return; /* read the title*/
		if(linetype(line,"//") ) break;		    /* lines...ignore*/
	}

	while (fgets(line,MAXLINE+1,fin) != NULL) {
		if(!blankline(line)) {

			for(i=1;i<seqno;i++) fgets(line,MAXLINE+1,fin);
                        for(j=0;j<=strlen(line);j++) if(line[j] != ' ') break;
			for(k=j;k<=strlen(line);k++) if(line[k] == ' ') break;
			strncpy(sname,line+j,MIN(MAXNAMES,k-j)); 
			sname[MIN(MAXNAMES,k-j)]=EOS;
			rtrim(sname);
                       	blank_to_(sname);

			for(i=k;*len < MAXLEN;i++) {
				c=line[i];
				if(c == '.') c = '-';
				if(c == '*') c = 'X';
				if(c == '\n' || c == EOS) break; /* EOL */
				if( (c=chartab[c])) seq[++(*len)]=c;
			}
			if(*len == MAXLEN) return;

			for(i=0;;i++) {
				if(fgets(line,MAXLINE+1,fin)==NULL) return;
				if(blankline(line)) break;
			}
		}
	}
}


static void get_clustal_seq(char *sname,char *seq,int *len,char *tit,int seqno)
/* read the seqno_th. sequence from a clustal multiple alignment file */
{
	static char line[MAXLINE+1];
	int i,j,k;
	unsigned char c;

	fseek(fin,0,0); 		/* start at the beginning */

	*len=0;				/* initialise length to zero */
	fgets(line,MAXLINE+1,fin);	/* read the title line...ignore it */

	while (fgets(line,MAXLINE+1,fin) != NULL) {
		if(!blankline(line)) {

			for(i=1;i<seqno;i++) fgets(line,MAXLINE+1,fin);
                        for(j=0;j<=strlen(line);j++) if(line[j] != ' ') break;
			strncpy(sname,line+j,MAXNAMES-j); /* remember entryname */
			sname[MAXNAMES]=EOS;
			rtrim(sname);
                       	blank_to_(sname);

			for(i=14;*len < MAXLEN;i++) {
				c=line[i];
				if(c == '\n' || c == EOS) break; /* EOL */
				if( (c=chartab[c])) seq[++(*len)]=c;
			}
			if(*len == MAXLEN) return;

			for(i=0;;i++) {
				if(fgets(line,MAXLINE+1,fin)==NULL) return;
				if(blankline(line)) break;
			}
		}
	}
}


static void get_seq(char *sname,char *seq,int *len,char *tit)
{
	static char line[MAXLINE+1];
	int i, offset;
        unsigned char c;

	switch(seqFormat) {

/************************************/
		case EMBLSWISS:
			while( !linetype(line,"ID") )
				fgets(line,MAXLINE+1,fin);
			
                        for(i=5;i<=strlen(line);i++)  /* DES */
				if(line[i] != ' ') break;
			strncpy(sname,line+i,MAXNAMES); /* remember entryname */
			sname[MAXNAMES]=EOS;
			rtrim(sname);
                        blank_to_(sname);

			while( !linetype(line,"SQ") )
				fgets(line,MAXLINE+1,fin);
			
			*len=0;
			while(fgets(line,MAXLINE+1,fin)) {
				for(i=0;*len < MAXLEN;i++) {
					c=line[i];
				if(c == '\n' || c == EOS || c == '/')
					break;			/* EOL */
				if( (c=chartab[c]))
					seq[++(*len)]=c;
				}
			if(*len == MAXLEN || c == '/') break;
			}
		break;
		
/************************************/
		case PIR:
			while(*line != '>')
				fgets(line,MAXLINE+1,fin);			
                        for(i=4;i<=strlen(line);i++)  /* DES */
				if(line[i] != ' ') break;
			strncpy(sname,line+i,MAXNAMES); /* remember entryname */
			sname[MAXNAMES]=EOS;
			rtrim(sname);
                        blank_to_(sname);

			fgets(line,MAXLINE+1,fin);
			strncpy(tit,line,MAXTITLES);
			tit[MAXTITLES]=EOS;
			i=strlen(tit);
			if(tit[i-1]=='\n') tit[i-1]=EOS;
			
			*len=0;
			while(fgets(line,MAXLINE+1,fin)) {
				for(i=0;*len < MAXLEN;i++) {
					c=line[i];
				if(c == '\n' || c == EOS || c == '*')
					break;			/* EOL */
			
				if( (c=chartab[c]))
					seq[++(*len)]=c;
				}
			if(*len == MAXLEN || c == '*') break;
			}
		break;
/***********************************************/
		case PEARSON:
			while(*line != '>')
				fgets(line,MAXLINE+1,fin);
			
                        for(i=1;i<=strlen(line);i++)  /* DES */
				if(line[i] != ' ') break;
			strncpy(sname,line+i,MAXNAMES); /* remember entryname */
			sname[MAXNAMES]=EOS;
			rtrim(sname);
                        blank_to_(sname);

			*tit=EOS;
			
			*len=0;
			while(fgets(line,MAXLINE+1,fin)) {
				for(i=0;*len < MAXLEN;i++) {
					c=line[i];
				if(c == '\n' || c == EOS || c == '>')
					break;			/* EOL */
			
				if( (c=chartab[c]))
					seq[++(*len)]=c;
			}
			if(*len == MAXLEN || c == '>') break;
			}
		break;
/**********************************************/
		case GDE:
			if (dnaflag) {
				while(*line != '#')
					fgets(line,MAXLINE+1,fin);
			}
			else {
				while(*line != '%')
					fgets(line,MAXLINE+1,fin);
			}
			
			for (i=1;i<=MAXNAMES;i++) {
				if (line[i] == '(' || line[i] == '\n')
                                  {
                                    i--;
                                    break;
                                  }
				sname[i-1] = line[i];
			}
			sname[i]=EOS;
			if (sname[i-1] == '(') sscanf(&line[i],"%d",&offset); /* dgg, added & to offset */
			else offset = 0;
			for(i=MAXNAMES-1;i > 0;i--) 
				if(isspace(sname[i])) {
					sname[i]=EOS;	
					break;
				}		
                        blank_to_(sname);


			*tit=EOS;
			
			*len=0;
			for (i=0;i<offset;i++) seq[++(*len)] = '-';
			while(fgets(line,MAXLINE+1,fin)) {
			if(*line == '%' || *line == '#' || *line == '"') break;
				for(i=0;*len < MAXLEN;i++) {
					c=line[i];
				if(c == '\n' || c == EOS) 
					break;			/* EOL */
			
				if( (c=chartab[c]))
					seq[++(*len)]=c;
				}
			if(*len == MAXLEN) break;
			}
		break;
/***********************************************/
	}
	
	if(*len == MAXLEN)
		warning("Sequence %s truncated to %d residues",
				sname,(pint)MAXLEN);
				
	seq[*len+1]=EOS;
}


int readseqs(int first_seq) /*first_seq is the #no. of the first seq. to read */
{
	char line[FILENAMELEN+1];
	static char seq1[MAXLEN+2],sname1[MAXNAMES+1],title[MAXTITLES+1];
	int i,j,no_seqs;
	static int l1;
	static Boolean dnaflag1;
	
	if(usemenu)
		getstr("Enter the name of the sequence file",line);
	else
		strcpy(line,seqname);
	if(*line == EOS) return -1;
	
	if((fin=fopen(line,"r"))==NULL) {
		error("Could not open sequence file %s",line);
		return -1;      /* DES -1 => file not found */
	}
	strcpy(seqname,line);
	no_seqs=0;
	check_infile(&no_seqs);
	printf("\nSequence format is %s\n",formatNames[seqFormat]);

/* DES DEBUG 
	fprintf(stdout,"\n\n File name = %s\n\n",seqname);
*/
	if(no_seqs == 0)
		return 0;       /* return the number of seqs. (zero here)*/

	if((no_seqs + first_seq -1) > MAXN) {
		error("Too many sequences. Maximum is %d",(pint)MAXN);
		return 0;       /* also return zero if too many */
	}

/* DES */
/*	if(seqFormat == CLUSTAL) {
		fprintf(stdout," \n no of sequences = %d",(pint)no_seqs);
		return no_seqs;
	}
*/

	for(i=first_seq;i<=first_seq+no_seqs-1;i++) {    /* get the seqs now*/
		output_index[i] = i;	/* default output order */
		if(seqFormat == CLUSTAL) 
			get_clustal_seq(sname1,seq1,&l1,title,i-first_seq+1);
		if(seqFormat == MSF)
			    get_msf_seq(sname1,seq1,&l1,title,i-first_seq+1);
		else
			get_seq(sname1,seq1,&l1,title);
		if(l1 > MAXLEN) {
			error("Sequence too long. Maximum is %d",(pint)MAXLEN);
			return 0;       /* also return zero if too many */
		}
		seqlen_array[i]=l1;                   /* store the length */
		strcpy(names[i],sname1);              /*    "   "  name   */
		strcpy(titles[i],title);              /*    "   "  title  */

		if(!explicit_dnaflag) {
			dnaflag1 = check_dnaflag(seq1,l1); /* check DNA/Prot */
		        if(i == 1) dnaflag = dnaflag1;
		}			/* type decided by first seq*/
		else
			dnaflag1 = dnaflag;

		if( (!explicit_dnaflag) && (dnaflag1 != dnaflag) )
			warning(
	"Sequence %d [%s] appears to be of different type to sequence 1",
			(pint)i,sname1);

		if(dnaflag)
			n_encode(seq1,seq_array[i],l1); /* encode the sequence*/
		else					/* as ints  */
			p_encode(seq1,seq_array[i],l1);
	}

	fclose(fin);
/*
   JULIE
   check sequence names are all different - otherwise phylip tree is 
   confused.
*/
	for(i=first_seq;i<=first_seq+no_seqs-1;i++) {
		for(j=i+1;j<=first_seq+no_seqs-1;j++) {
			if (strncmp(names[i],names[j],MAXNAMES) == 0) {
				error("Multiple sequences found with same name (first %d chars are significant)", MAXNAMES);
				return -1;
			}
		}
	}
			
	return no_seqs;    /* return the number of seqs. read in this call */
}


static Boolean check_dnaflag(char *seq, int slen)
/* check if DNA or Protein
   The decision is based on counting all A,C,G,T,U or N. 
   If >= 85% of all characters (except -) are as above => DNA  */
{
	int i, c, nresidues, nbases;
	float ratio;
	
	nresidues = nbases = 0;	
	for(i=1; i <= slen; i++) {
		if(seq[i] != '-') {
			nresidues++;
			if(seq[i] == 'N')
				nbases++;
			else {
				c = res_index(nucleic_acid_order, seq[i]);
				if(c >= 0)
					nbases++;
			}
		}
	}
	if( (nbases == 0) || (nresidues == 0) ) return FALSE;
	ratio = (float)nbases/(float)nresidues;
/* DES 	fprintf(stdout,"\n nbases = %d, nresidues = %d, ratio = %f\n",
		(pint)nbases,(pint)nresidues,(pint)ratio); */
	if(ratio >= 0.85) 
		return TRUE;
	else
		return FALSE;
}



static void check_infile(int *nseqs)
{
	char line[MAXLINE+1];
	int i;	

	*nseqs=0;
	while (fgets(line,MAXLINE+1,fin) != NULL) {
		if ((*line != '\n') && (*line != ' ') && (*line != '\t'))
			break;
	}
        
	for(i=0;i<=6;i++) line[i] = toupper(line[i]);

	if( linetype(line,"ID") ) {					/* EMBL/Swiss-Prot format ? */
		seqFormat=EMBLSWISS;
		(*nseqs)++;
	}
        else if( linetype(line,"CLUSTAL") ) {
		seqFormat=CLUSTAL;
	}
        else if( linetype(line,"PILEUP") ) {
		seqFormat = MSF;
	}
	else if(*line == '>') {						/* no */
		seqFormat=(line[3] == ';')?PIR:PEARSON; /* distinguish PIR and Pearson */
		(*nseqs)++;
	}
	else if((*line == '"') || (*line == '%') || (*line == '#')) {
		seqFormat=GDE; /* GDE format */
		if (*line == '%') {
                        (*nseqs)++;
			dnaflag = FALSE;
			explicit_dnaflag = TRUE;
		}
		else if (*line == '#') {
			(*nseqs)++;
			dnaflag = TRUE;
			explicit_dnaflag = TRUE;
		}
	}
	else {
		seqFormat=UNKNOWN;
		return;
	}

	while(fgets(line,MAXLINE+1,fin) != NULL) {
		switch(seqFormat) {
			case EMBLSWISS:
				if( linetype(line,"ID") )
					(*nseqs)++;
				break;
			case PIR:
			case PEARSON:
				if( *line == '>' )
					(*nseqs)++;
				break;
			case GDE:
				if(( *line == '%' ) && ( dnaflag == FALSE))
					(*nseqs)++;
				else if (( *line == '#') && ( dnaflag == TRUE))
					(*nseqs)++;
				break;
			case CLUSTAL:
				*nseqs = count_clustal_seqs();
/* DES */ 			/* fprintf(stdout,"\nnseqs = %d\n",(pint)*nseqs); */
				fseek(fin,0,0);
				return;
				break;
			case MSF:
				*nseqs = count_msf_seqs();
				fseek(fin,0,0);
				return;
				break;
			case USER:
			default:
				break;
		}
	}
	fseek(fin,0,0);
}


static int count_clustal_seqs(void)
/* count the number of sequences in a clustal alignment file */
{
	char line[MAXLINE+1];
	int  nseqs;

	while (fgets(line,MAXLINE+1,fin) != NULL) {
		if(!blankline(line)) break;		/* Look for next non- */
	}						/* blank line */
	nseqs = 1;

	while (fgets(line,MAXLINE+1,fin) != NULL) {
		if(blankline(line)) return nseqs;
		nseqs++;
	}

	return 0;	/* if you got to here-funny format/no seqs.*/
}

static int count_msf_seqs(void)
{
/* count the number of sequences in a PILEUP alignment file */

	char line[MAXLINE+1];
	int  nseqs;

	while (fgets(line,MAXLINE+1,fin) != NULL) {
		if(linetype(line,"//")) break;
	}

	while (fgets(line,MAXLINE+1,fin) != NULL) {
		if(!blankline(line)) break;		/* Look for next non- */
	}						/* blank line */
	nseqs = 1;

	while (fgets(line,MAXLINE+1,fin) != NULL) {
		if(blankline(line)) return nseqs;
		nseqs++;
	}

	return 0;	/* if you got to here-funny format/no seqs.*/
}

static void p_encode(char *seq, char *naseq, int l)
{				/* code seq as ints .. use gap_pos2 for gap */
	register int i;
/*	static char *aacids="CSTPAGNDEQHRKMILVFYW";*/
	
	for(i=1;i<=l;i++)
		if(seq[i] == '-')
			naseq[i] = gap_pos2;
		else
			naseq[i] = res_index(amino_acid_codes,seq[i]);
	naseq[i] = -3;
}

static void n_encode(char *seq,char *naseq,int l)
{				/* code seq as ints .. use gap_pos2 for gap */
	register int i,c;
/*	static char *nucs="ACGTU";	*/
	
	for(i=1;i<=l;i++) {
    	if(seq[i] == '-')          	   /* if a gap character -> code = gap_pos2 */
			naseq[i] = gap_pos2;   /* this is the code for a gap in */
		else {                     /* the input files */
			c=res_index(nucleic_acid_order,seq[i]);
			if (c == 4) c=3;	/* T = U */
			if(c >= 0) naseq[i]=c;
			else       naseq[i]=5;	/* N */
		}
	}
	naseq[i] = -3;
}

static int res_index(char *t,char c)
{
	register int i;
	
	for(i=0;t[i] && t[i] != c;i++)
		;
	if(t[i]) return(i);
	else return -1;
}
