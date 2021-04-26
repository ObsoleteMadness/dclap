/* File: ureadseq.c
 *
 * Reads and writes nucleic/protein sequence in various
 * formats. Data files may have multiple sequences.
 *
 * Copyright 1990 by d.g.gilbert
 * biology dept., indiana university, bloomington, in 47405
 * e-mail: gilbertd@bio.indiana.edu
 *
 * This program may be freely copied and used by anyone.
 * Developers are encourged to incorporate parts in their
 * programs, rather than devise their own private sequence
 * format.
 *
 * This should compile and run with any ANSI C compiler.
 *
 */


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define UREADSEQ_G
#include "ureadseq.h"



int Strcasecmp(const char *a, const char *b)  /* from Nlm_StrICmp */
{
  int diff, done;
  if (a == b)  return 0;
  done = 0;
  while (! done) {
    diff = to_upper(*a) - to_upper(*b);
    if (diff) return diff;
    if (*a == '\0') done = 1;
    else { a++; b++; }
    }
  return 0;
}

int Strncasecmp(const char *a, const char *b, long maxn) /* from Nlm_StrNICmp */
{
  int diff, done;
  if (a == b)  return 0;
  done = 0;
  while (! done) {
    diff = to_upper(*a) - to_upper(*b);
    if (diff) return diff;
    if (*a == '\0') done = 1;
    else {
      a++; b++; maxn--;
      if (! maxn) done = 1;
      }
    }
  return 0;
}



#	define MALLOC(size)			(char*)malloc(size)
#	define REALLOC(p,size) 	p= (char*)realloc(p,size)
# define FREE(p)					free(p)

#ifndef Local
# define Local      static    /* local functions */
#endif

#define kStartLength 500 	/* 500000 to temp fix Unix bug */

const char *aminos      = "ABCDEFGHIKLMNPQRSTVWXYZ*";
const char *primenuc    = "ACGTU";
const char *protonly    = "EFIPQZ";

const char kNocountsymbols[5]  = "_.-?";
const char stdsymbols[6]  = "_.-*?";
const char allsymbols[32] = "_.-*?<>{}[]()!@#$%^&=+;:'/|`~\"\\";
static const char *seqsymbols   = allsymbols;

const char nummask[11]   = "0123456789";
const char nonummask[11] = "~!@#$%^&*(";

FILE	* gFile;
long	gLinestart = 0;




/*
    use general form of isseqchar -- all chars + symbols.
    no formats except nbrf (?) use symbols in data area as
    anything other than sequence chars.
*/



                          /* Local variables for readSeq: */
struct ReadSeqVars {
  short choice, err, nseq;
  long  seqlen, maxseq, seqlencount;
  short topnseq;
  long  topseqlen;
  const char *fname;
  char *seq, *seqid, matchchar;
  boolean allDone, done, filestart, addit;
  FILE  *f;
  long  linestart;
  char  s[256], *sp;

  int (*isseqchar)(int);  
	ReadWriteProc	reader;
};



#define EOFreader(V)	(boolean)(*V->reader)((char *)NULL, 0L, kRSFile_Eof)
#define REWINDreader(V)	(*V->reader)((char *)NULL, 0L, kRSFile_Rewind)

Local long  readWriteFromFile(char* line, long maxline, short action)
{
	static boolean geof = false;
	long   bytes;
	/*fprintf(stderr," r/w action= %d,  maxline= %ld ", action, maxline);*/
	/* ! one case where _Read returns 0 bytes at end of file, but _Eof returns false !! */
	
	switch (action) {
	
	case kRSFile_Eof:
		if (geof) {  /* fix for bad feof() ! */
			char ch= fgetc( gFile);
			if (ch != EOF) { ungetc( ch, gFile); geof= false; }
			}
		else geof= feof( gFile);
		return geof;

	case kRSFile_Read:
		bytes= (long) fgets (line, (int)maxline, gFile);
		if (!bytes) geof= true; /* fix for bad feof() ! */
		return bytes;
	
	case kRSFile_Write:
		geof= false;
		fputs( line, gFile);
		return 0;

	case kRSFile_Seek:
		geof= false;
		fseek( gFile, maxline, 0);
		return 0;

	case kRSFile_Rewind:
		rewind(gFile);
		geof= false;
		return 0;

	case kRSFile_Tell:
		return ftell(gFile);
		
	default:
		return 0;
	}
}




int isSeqChar(int c)
{
 /*  return (isalpha(c) || strchr(seqsymbols,c)); */
  if (isalpha(c) || strchr(seqsymbols,c)) return c;
  else return 0;
}

int isGCGSeqChar(int c)
{
 /*  return (isalpha(c) || strchr(seqsymbols,c)); */
  if (isalpha(c) || strchr(seqsymbols,c))  {
    if (c == '.') return '-'; /* do the indel translate */
    else return c;
    }
  else return 0;
}

int isSeqNumChar(int c)
{
  /* return (isalnum(c) || strchr(seqsymbols,c)); */
  if (isalnum(c) || strchr(seqsymbols,c)) return c;
  else return 0;
}


int isAnyChar(int c)
{
 /*  return isprint(c); /* wrap in case isascii is macro */
 if (isprint(c)) return c;
 else return 0;
}


Local void callreadline( ReadWriteProc reader, char *s)
{
  char  *cp;

	/*fprintf(stderr,"callreadline &reader= %ld \n", reader);*/
	
	gLinestart= (*reader)( (char*)NULL, 0L, kRSFile_Tell);
	if (0 == (*reader)( s, 256L, kRSFile_Read) )
		*s = 0;
  else {
    cp = strchr(s, '\n');
    if (cp != NULL) *cp = 0;
    }
}

/********
Local void readline(FILE *f, char *s, long *linestart)
{
  char  *cp;

	*linestart= ftell(f);
	if (NULL == fgets(s, 256, f))
    *s = 0;
  else {
    cp = strchr(s, '\n');
    if (cp != NULL) *cp = 0;
    }
}
******/

Local void getline(struct ReadSeqVars *V)
{
  /*readline(V->f, V->s, &V->linestart);  */
	callreadline(V->reader, V->s);
}

Local void ungetline(struct ReadSeqVars *V)
{
	/*fseek(V->f, V->linestart, 0); */
	(void) (*(V->reader))( (char*)NULL, gLinestart, kRSFile_Seek);  
}


Local void addseq(char *s, struct ReadSeqVars *V)
{
  char  *ptr;
  register char seqc;
  
  if (V->addit) while (*s != 0) {
    if ((seqc= (V->isseqchar)(*s)) != 0) {
      if (V->seqlen >= V->maxseq) {
        V->maxseq += kStartLength;
#ifdef NOTmacintosh
				SetPtrSize( (Ptr)V->seq, V->maxseq+1);
				if (MemError() != 0) {
          V->err = eMemFull;
          return;
          }
#else
        ptr = (char*) realloc(V->seq, V->maxseq+1);
        if (ptr==NULL) {
          V->err = eMemFull;
          return;
          }
        else V->seq = ptr;
#endif
				}
      V->seq[(V->seqlen)++] = seqc; /* *s */
      }
    s++;
    }
}

Local void countseq(char *s, struct ReadSeqVars *V)
 /* this must count all valid seq chars, for some formats (paup-sequential) even
    if we are skipping seq... */
{
  while (*s != 0) {
    if ((V->isseqchar)(*s)) {
      (V->seqlencount)++;
      }
    s++;
    }
}


Local void addinfo(char *s, struct ReadSeqVars *V)
{
  char s2[256], *si;
  boolean saveadd;
  int (*oldIsSeqChar)(int);  
  
  si = s2;
  while (*s == ' ') s++;
  sprintf(si, " %d)  %s\n", V->nseq, s);

  saveadd = V->addit;
  V->addit = true;
  oldIsSeqChar= V->isseqchar;
  V->isseqchar = isAnyChar;
  addseq( si, V);
  V->addit = saveadd;
  V->isseqchar = oldIsSeqChar;  
}




Local void readLoop(short margin, boolean addfirst,
            boolean (*endTest)(boolean *addend, boolean *ungetend, struct ReadSeqVars *V),
            struct ReadSeqVars *V)
{
  boolean addend = false;
  boolean ungetend = false;

  V->nseq++;
  if (V->choice == kListSequences) V->addit = false;
  else V->addit = (V->nseq == V->choice);
  if (V->addit) V->seqlen = 0;

  if (addfirst) addseq(V->s, V);
  do {
    getline(V);
    V->done = EOFreader(V); /* feof(V->f); */
    V->done |= (*endTest)( &addend, &ungetend, V);
    if (V->addit && (addend || !V->done) && (strlen(V->s) > margin)) {
      addseq( (V->s)+margin, V);
    }
  } while (!V->done);

  if (V->choice == kListSequences) addinfo(V->seqid, V);
  else {
    V->allDone = (V->nseq >= V->choice);
    if (V->allDone && ungetend) ungetline(V);
    }
}



Local boolean endIG( boolean *addend, boolean *ungetend, struct ReadSeqVars *V)
{
  *addend = true; /* 1 or 2 occur in line w/ bases */
  *ungetend= false;
  return((strchr(V->s,'1')!=NULL) || (strchr(V->s,'2')!=NULL));
}

Local void readIG(struct ReadSeqVars *V)
{
/* 18Aug92: new IG format -- ^L between sequences in place of ";" */
  char  *si;

  while (!V->allDone) {
    do {
      getline(V);
      for (si= V->s; *si != 0 && *si < ' '; si++) *si= ' '; /* drop controls */
      if (*si == 0) *V->s= 0; /* chop line to empty */
    } while (! (EOFreader(V) || ((*V->s != 0) && (*V->s != ';') ) ));
    if (EOFreader(V))
      V->allDone = true;
    else {
      strcpy(V->seqid, V->s);
      readLoop(0, false, endIG, V);
      }
  }
}



Local boolean endStrider( boolean *addend, boolean *ungetend, struct ReadSeqVars *V)
{
  *addend = false;
  *ungetend= false;
  return (strstr( V->s, "//") != NULL);
}

Local void readStrider(struct ReadSeqVars *V)
{ /* ? only 1 seq/file ? */

  while (!V->allDone) {
    getline(V);
    if (strstr(V->s,"; DNA sequence  ") == V->s)
      strcpy(V->seqid, (V->s)+16);
    else
      strcpy(V->seqid, (V->s)+1);
    while ((!EOFreader(V)) && (*V->s == ';')) {
      getline(V);
      }
    if (EOFreader(V)) V->allDone = true;
    else readLoop(0, true, endStrider, V);
  }
}


Local boolean endPIR( boolean *addend, boolean *ungetend, struct ReadSeqVars *V)
{
  *addend = false;
  *ungetend= (strstr(V->s,"ENTRY") == V->s);
  return ((strstr(V->s,"///") != NULL) || *ungetend);
}

Local void readPIR(struct ReadSeqVars *V)
{ /*PIR -- many seqs/file */

  while (!V->allDone) {
    while (! (EOFreader(V) 
				|| strstr(V->s,"ENTRY")  || strstr(V->s,"SEQUENCE")) )
      getline(V);
    strcpy(V->seqid, (V->s)+16);
    while (! (EOFreader(V) || strstr(V->s,"SEQUENCE") == V->s))
      getline(V);
    readLoop(0, false, endPIR, V);

    if (!V->allDone) {
     while (! (EOFreader(V) || ((*V->s != 0)
       && (strstr( V->s,"ENTRY") == V->s))))
        getline(V);
      }
    if (EOFreader(V)) V->allDone = true;
  }
}


Local boolean endGB( boolean *addend, boolean *ungetend, struct ReadSeqVars *V)
{
  *addend = false;
  *ungetend= (strstr(V->s,"LOCUS") == V->s);
  return ((strstr(V->s,"//") != NULL) || *ungetend);
}

Local void readGenBank(struct ReadSeqVars *V)
{ /*GenBank -- many seqs/file */

  while (!V->allDone) {
    strcpy(V->seqid, (V->s)+12);
    while (! (EOFreader(V) || strstr(V->s,"ORIGIN") == V->s))
      getline(V);
    readLoop(0, false, endGB, V);

    if (!V->allDone) {
     while (! (EOFreader(V) || ((*V->s != 0)
       && (strstr( V->s,"LOCUS") == V->s))))
        getline(V);
      }
    if (EOFreader(V)) V->allDone = true;
  }
}


Local boolean endNBRF( boolean *addend, boolean *ungetend, struct ReadSeqVars *V)
{
  char  *a;

  if ((a = strchr(V->s, '*')) != NULL) { /* end of 1st seq */
    /* "*" can be valid base symbol, drop it here */
    *a = 0;
    *addend = true;
    *ungetend= false;
    return(true);
    }
  else if (*V->s == '>') { /* start of next seq */
    *addend = false;
    *ungetend= true;
    return(true);
    }
  else
    return(false);
}

Local void readNBRF(struct ReadSeqVars *V)
{
  while (!V->allDone) {
    strcpy(V->seqid, (V->s)+4);
    getline(V);   /*skip title-junk line*/
    readLoop(0, false, endNBRF, V);
    if (!V->allDone) {
     while (!(EOFreader(V) || (*V->s != 0 && *V->s == '>')))
        getline(V);
      }
    if (EOFreader(V)) V->allDone = true;
  }
}



Local boolean endPearson( boolean *addend, boolean *ungetend, struct ReadSeqVars *V)
{
  *addend = false;
  *ungetend= true;
  return(*V->s == '>');
}

Local void readPearson(struct ReadSeqVars *V)
{
  while (!V->allDone) {
    strcpy(V->seqid, (V->s)+1);
    readLoop(0, false, endPearson, V);
    if (!V->allDone) {
     while (!(EOFreader(V) || ((*V->s != 0) && (*V->s == '>'))))
        getline(V);
      }
    if (EOFreader(V)) V->allDone = true;
  }
}



Local boolean endEMBL( boolean *addend, boolean *ungetend, struct ReadSeqVars *V)
{
  *addend = false;
  *ungetend= (strstr(V->s,"ID   ") == V->s);
  return ((strstr(V->s,"//") != NULL) || *ungetend);
}

Local void readEMBL(struct ReadSeqVars *V)
{
  while (!V->allDone) {
    strcpy(V->seqid, (V->s)+5);
    do {
      getline(V);
    } while (!(EOFreader(V) || (strstr(V->s,"SQ   ") == V->s)));

    readLoop(0, false, endEMBL, V);
    if (!V->allDone) {
      while (!(EOFreader(V) ||
         ((*V->s != '\0') && (strstr(V->s,"ID   ") == V->s))))
      getline(V);
    }
    if (EOFreader(V)) V->allDone = true;
  }
}



Local boolean endZuker( boolean *addend, boolean *ungetend, struct ReadSeqVars *V)
{
  *addend = false;
  *ungetend= true;
  return( *V->s == '(' );
}

Local void readZuker(struct ReadSeqVars *V)
{
  /*! 1st string is Zuker's Fortran format */

  while (!V->allDone) {
    getline(V);  /*s == "seqLen seqid string..."*/
    strcpy(V->seqid, (V->s)+6);
    readLoop(0, false, endZuker, V);
    if (!V->allDone) {
      while (!(EOFreader(V) |
        ((*V->s != '\0') && (*V->s == '('))))
          getline(V);
      }
    if (EOFreader(V)) V->allDone = true;
  }
}



Local boolean endFitch( boolean *addend, boolean *ungetend, struct ReadSeqVars *V)
{
  /* this is a somewhat shaky end,
    1st char of line is non-blank for seq. title
  */
  *addend = false;
  *ungetend= true;
  return( *V->s != ' ' );
}

Local void readFitch(struct ReadSeqVars *V)
{
  boolean first;

  first = true;
  while (!V->allDone) {
    if (!first) strcpy(V->seqid, V->s);
    readLoop(0, first, endFitch, V);
    if (EOFreader(V)) V->allDone = true;
    first = false;
    }
}


Local void readPlain(struct ReadSeqVars *V)
{
  V->nseq++;
  V->addit = (V->choice > 0);
  if (V->addit) V->seqlen = 0;
  addseq(V->seqid, V);   /*from above..*/
  if (V->fname!=NULL) sprintf(V->seqid, "%s  [Unknown form]", V->fname);
  else sprintf(V->seqid, "  [Unknown form]");
  do {
    addseq(V->s, V);
    V->done = EOFreader(V);
    getline(V);
  } while (!V->done);
  if (V->choice == kListSequences) addinfo(V->seqid, V);
  V->allDone = true;
}


Local void readUWGCG(struct ReadSeqVars *V)
{
/*
10nov91: Reading GCG files casued duplication of last line when
         EOF followed that line !!!
    fix: getline now sets *V->s = 0
*/
  char  *si;

  V->nseq++;
  V->addit = (V->choice > 0);
  if (V->addit) V->seqlen = 0;
  strcpy(V->seqid, V->s);
  /*writeseq: "    %s  Length: %d  (today)  Check: %d  ..\n" */
  /*drop above or ".." from id*/
  if ((si = strstr(V->seqid,"  Length: ")) != 0) *si = 0;
  else if ((si = strstr(V->seqid,"..")) != 0) *si = 0;
  do {
    V->done = EOFreader(V);
    getline(V);
    if (!V->done) addseq((V->s), V);
  } while (!V->done);
  if (V->choice == kListSequences) addinfo(V->seqid, V);
  V->allDone = true;
}


Local void readOlsen(struct ReadSeqVars *V)
{ /* G. Olsen /print output from multiple sequence editor */

  char    *si, *sj, *sk, *sm, sid[40], snum[20];
  boolean indata = false;
  short snumlen = 0;

  V->addit = (V->choice > 0);
  if (V->addit) V->seqlen = 0;
  REWINDreader(V); 
	V->nseq= 0;
  do {
    getline(V);
    V->done = EOFreader(V);

    if (V->done && !(*V->s)) break;
    else if (indata) {
      if ( (si= strstr(V->s, sid))
        && ( (sm= strstr(V->s, snum)) != NULL ) && (sm < si - snumlen) ) {
        /* && (strstr(V->s, snum) == si - snumlen - 1) ) */

        /* Spaces are valid alignment data !! */
/* 17Oct91: Error, the left margin is 21 not 22! */
/* dropped some nucs up to now -- my example file was right shifted ! */
/* variable right id margin, drop id-2 spaces at end */
/*
  VMS CC COMPILER (VAXC031) mess up:
  -- Index of 21 is chopping 1st nuc on VMS systems Only!
  Byte-for-byte same ame rnasep.olsen sequence file !
*/

        /* si = (V->s)+21; < was this before VMS CC wasted my time */
        si += 10;  /* use strstr index plus offset to outfox VMS CC bug */

        if ((sk = strstr(si, sid))!=0) *(sk-2) = 0;
        for (sk = si; *sk != 0; sk++) {
           if (*sk == ' ') *sk = '.';
           /* 18aug92: !! some olsen masks are NUMBERS !! which addseq eats */
           else if (isdigit(*sk)) *sk= nonummask[*sk - '0'];
           }

        addseq(si, V);
        }
      }

    else if ((sk = strstr(V->s, "): "))!=0) {  /* seq info header line */
  /* 18aug92: correct for diff seqs w/ same name -- use number, e.g. */
  /*   3 (Agr.tume):  agrobacterium.prna  18-JUN-1987 16:12 */
  /* 328 (Agr.tume):  agrobacterium.prna XYZ  19-DEC-1992   */
      (V->nseq)++;
      si = 1 + strchr(V->s,'(');
      *sk = ' ';
      if (V->choice == kListSequences) addinfo( si, V);
      else if (V->nseq == V->choice) {
        strcpy(V->seqid, si);
        sj = strchr(V->seqid, ':');
        while (*(--sj) == ' ') ;
        while (--sj != V->seqid) { if (*sj == ' ') *sj = '_'; }

        *sk = 0;
        while (*(--sk) == ' ') *sk = 0;
        strcpy(sid, si);

        si= V->s;
        while ((*si <= ' ') && (*si != 0)) si++;
        snumlen=0;
        while (si[snumlen] > ' ' && snumlen<20)
         { snum[snumlen]= si[snumlen]; snumlen++; }
        snum[snumlen]= 0;
        }

      }

    else if (strstr(V->s,"identity:   Data:")) {
      indata = true;
      if (V->choice == kListSequences) V->done = true;
      }

  } while (!V->done);

  V->allDone = true;
} /*readOlsen*/


Local void readMSF(struct ReadSeqVars *V)
{ /* gcg's MSF, mult. sequence format, interleaved ! */

  char    *si, *sj, sid[128];
  boolean indata = false;
  int     atseq= 0, iline= 0;

  V->addit = (V->choice > 0);
  if (V->addit) V->seqlen = 0;
  REWINDreader(V); 
	V->nseq= 0;
  do {
    getline(V);
    V->done = EOFreader(V);

    if (V->done && !(*V->s)) break;
    else if (indata) {
      /*somename  ...gpvedai .......t.. aaigr..vad tvgtgptnse aipaltaaet */
      /*       E  gvenae.kgv tentna.tad fvaqpvylpe .nqt...... kv.affynrs */

      si= V->s;
      skipwhitespace(si);
      /* for (sj= si; isalnum(*sj); sj++) ; bug -- delwiche uses "-", "_" and others in names*/
      for (sj= si; *sj > ' '; sj++) ;
      *sj= 0;
      if ( *si ) {
        if ( (0==strcmp(si, sid)) ) {
          addseq(sj+1, V);
          }
        iline++;
        }
      }

    else if (NULL != (si = strstr(V->s, "Name: "))) {  /* seq info header line */
      /* Name: somename      Len:   100  Check: 7009  Weight:  1.00 */

      (V->nseq)++;
      si += 6;
      if (V->choice == kListSequences) addinfo( si, V);
      else if (V->nseq == V->choice) {
        strcpy(V->seqid, si);
        si = V->seqid;
        skipwhitespace(si);
        /* for (sj= si; isalnum(*sj); sj++) ; -- bug */
        for (sj= si; *sj > ' '; sj++) ;
        *sj= 0;
        strcpy(sid, si);
        }
      }

    else if ( strstr(V->s,"//") /*== V->s*/ )  {
      indata = true;
      iline= 0;
      if (V->choice == kListSequences) V->done = true;
      }

  } while (!V->done);


  V->allDone = true;
} /*readMSF*/



Local void readPAUPinterleaved(struct ReadSeqVars *V)
{ /* PAUP mult. sequence format, interleaved or sequential! */

  char    *si, *sj, *send, sid[40], sid1[40], saveseq[255];
  boolean first = true, indata = false, domatch;
  int     atseq= 0, iline= 0, ifmc, saveseqlen=0;

#define fixmatchchar(s) { \
  for (ifmc=0; ifmc<saveseqlen; ifmc++) \
    if (s[ifmc] == V->matchchar) s[ifmc]= saveseq[ifmc]; }

  V->addit = (V->choice > 0);
  V->seqlencount = 0;
  if (V->addit) V->seqlen = 0;
  /* rewind(V->f); V->nseq= 0;  << do in caller !*/
  indata= true; /* call here after we find "matrix" */
  domatch= (V->matchchar > 0);

  do {
    getline(V);
    V->done = EOFreader(V);

    if (V->done && !(*V->s)) break;
    else if (indata) {
      /* [         1                    1                    1         ]*/
      /* human     aagcttcaccggcgcagtca ttctcataatcgcccacggR cttacatcct*/
      /* chimp     ................a.t. .c.................a ..........*/
      /* !! need to correct for V->matchchar */
      si= V->s;
      skipwhitespace(si);
      if (strchr(si,';')) indata= false;

      if (isalnum(*si))  {
        /* valid data line starts w/ a left-justified seq name in columns [0..8] */
        if (first) {
          (V->nseq)++;
          if (V->nseq >= V->topnseq) first= false;
          for (sj = si; isalnum(*sj); sj++) ;
          send= sj;
          skipwhitespace(sj);
          if (V->choice == kListSequences) {
            *send= 0;
            addinfo( si, V);
            }
          else if (V->nseq == V->choice) {
            if (domatch) {
              if (V->nseq == 1) { strcpy( saveseq, sj); saveseqlen= strlen(saveseq); }
              else fixmatchchar( sj);
              }
            addseq(sj, V);
            *send= 0;
            strcpy(V->seqid, si);
            strcpy(sid, si);
            if (V->nseq == 1) strcpy(sid1, sid);
            }
          }

        else if ( (strstr(si, sid) == si) ){
          while (isalnum(*si)) si++;
          skipwhitespace(si);
          if (domatch) {
            if (V->nseq == 1) { strcpy( saveseq, si); saveseqlen= strlen(saveseq); }
            else fixmatchchar( si);
            }
          addseq(si, V);
          }

        else if (domatch && (strstr(si, sid1) == si)) {
          strcpy( saveseq, si);
          saveseqlen= strlen(saveseq);
          }

        iline++;
        }
      }

    else if ( strstr(V->s,"matrix") )  {
      indata = true;
      iline= 0;
      if (V->choice == kListSequences) V->done = true;
      }

  } while (!V->done);

  V->allDone = true;
} /*readPAUPinterleaved*/



Local void readPAUPsequential(struct ReadSeqVars *V)
{ /* PAUP mult. sequence format, interleaved or sequential! */
  char    *si, *sj;
  boolean atname = true, indata = false;

  V->addit = (V->choice > 0);
  if (V->addit) V->seqlen = 0;
  V->seqlencount = 0;
  /* REWINDreader(V); V->nseq= 0;  << do in caller !*/
  indata= true; /* call here after we find "matrix" */
  do {
    getline(V);
    V->done = EOFreader(V);

    if (V->done && !(*V->s)) break;
    else if (indata) {
      /* [         1                    1                    1         ]*/
      /* human     aagcttcaccggcgcagtca ttctcataatcgcccacggR cttacatcct*/
      /*           aagcttcaccggcgcagtca ttctcataatcgcccacggR cttacatcct*/
      /* chimp     ................a.t. .c.................a ..........*/
      /*           ................a.t. .c.................a ..........*/

      si= V->s;
      skipwhitespace(si);
      if (strchr(si,';')) indata= false;
      if (isalnum(*si))  {
        /* valid data line starts w/ a left-justified seq name in columns [0..8] */
        if (atname) {
          (V->nseq)++;
          V->seqlencount = 0;
          atname= false;
          sj= si+1;
          while (isalnum(*sj)) sj++;
          if (V->choice == kListSequences) {
            /* !! we must count bases to know when topseqlen is reached ! */
            countseq(sj, V);
            if (V->seqlencount >= V->topseqlen) atname= true;
            *sj= 0;
            addinfo( si, V);
            }
          else if (V->nseq == V->choice) {
            addseq(sj, V);
            V->seqlencount= V->seqlen;
            if (V->seqlencount >= V->topseqlen) atname= true;
            *sj= 0;
            strcpy(V->seqid, si);
            }
          else {
            countseq(sj, V);
            if (V->seqlencount >= V->topseqlen) atname= true;
            }
          }

        else if (V->nseq == V->choice) {
          addseq(V->s, V);
          V->seqlencount= V->seqlen;
          if (V->seqlencount >= V->topseqlen) atname= true;
          }
        else {
          countseq(V->s, V);
          if (V->seqlencount >= V->topseqlen) atname= true;
          }
        }
      }

    else if ( strstr(V->s,"matrix") )  {
      indata = true;
      atname= true;
      if (V->choice == kListSequences) V->done = true;
      }

  } while (!V->done);

  V->allDone = true;
} /*readPAUPsequential*/


Local void readPhylipInterleaved(struct ReadSeqVars *V)
{
  char    *si, *sj;
  boolean first = true;
  int     iline= 0;

  V->addit = (V->choice > 0);
  if (V->addit) V->seqlen = 0;
  V->seqlencount = 0;
  /* sscanf( V->s, "%d%d", &V->topnseq, &V->topseqlen); << topnseq == 0 !!! bad scan !! */
  si= V->s;
  skipwhitespace(si);
  V->topnseq= atoi(si);
  while (isdigit(*si)) si++;
  skipwhitespace(si);
  V->topseqlen= atol(si);
  /* fprintf(stderr,"Phylip-ileaf: topnseq=%d  topseqlen=%d\n",V->topnseq, V->topseqlen); */

  do {
    getline(V);
    V->done = EOFreader(V);

    if (V->done && !(*V->s)) break;
    si= V->s;
    skipwhitespace(si);
    if (*si != 0) {

      if (first) {  /* collect seq names + seq, as fprintf(outf,"%-10s  ",seqname); */
        (V->nseq)++;
        if (V->nseq >= V->topnseq) first= false;
        sj= V->s+10;  /* past name, start of data */
        if (V->choice == kListSequences) {
          *sj= 0;
          addinfo( si, V);
          }
        else if (V->nseq == V->choice) {
          addseq(sj, V);
          *sj= 0;
          strcpy(V->seqid, si);
          }
        }
      else if ( iline % V->nseq == V->choice -1 ) {
        addseq(si, V);
        }
      iline++;
    }
  } while (!V->done);

  V->allDone = true;
} /*readPhylipInterleaved*/



Local boolean endPhylipSequential( boolean *addend, boolean *ungetend, struct ReadSeqVars *V)
{
  boolean done;
	/* *addend = false; << need this true if EOF(file) on last dataline */
  *ungetend= false;
  countseq( V->s, V);
	done= (V->seqlencount >= V->topseqlen);
  *addend = !done;
	return done;
}

Local void readPhylipSequential(struct ReadSeqVars *V)
{
  short  i;
  char  *si;
  /* sscanf( V->s, "%d%d", &V->topnseq, &V->topseqlen); < ? bad sscan ? */
  si= V->s;
  skipwhitespace(si);
  V->topnseq= atoi(si);
  while (isdigit(*si)) si++;
  skipwhitespace(si);
  V->topseqlen= atol(si);
  getline(V);
  while (!V->allDone) {
    V->seqlencount= 0;
    strncpy(V->seqid, (V->s), 10);
    V->seqid[10]= 0;
    for (i=0; i<10 && V->s[i]; i++) V->s[i]= ' ';
    readLoop(0, true, endPhylipSequential, V);
    if (EOFreader(V)) V->allDone = true;
    }
}




Local void readSeqMain(
      struct ReadSeqVars *V,
      const long  skiplines,
      const short format)
{
#define tolowerstr(s) { long Itlwr, Ntlwr= strlen(s); \
  for (Itlwr=0; Itlwr<Ntlwr; Itlwr++) s[Itlwr]= to_lower(s[Itlwr]); }

  boolean gotuw;
  long l;

	/*fprintf(stderr,"readSeqMain &reader= %ld  &gFile= %ld\n", V->reader, gFile);*/

  V->linestart= 0;
  V->matchchar= 0;
  if (V->reader == NULL)
    V->err = eFileNotFound;
  else {

    for (l = skiplines; l > 0; l--) getline( V);

    do {
      getline( V);
      for (l= strlen(V->s); (l > 0) && (V->s[l] == ' '); l--) ;
    } while ((l == 0) && !EOFreader(V));

    if (EOFreader(V)) V->err = eNoData;

    else switch (format) {
      case kPlain : readPlain(V); break;
      case kIG    : readIG(V); break;
      case kStrider: readStrider(V); break;
      case kGenBank: readGenBank(V); break;
      case kPIR   : readPIR(V); break;
      case kNBRF  : readNBRF(V); break;
      case kPearson: readPearson(V); break;
      case kEMBL  : readEMBL(V); break;
      case kZuker : readZuker(V); break;
      case kOlsen : readOlsen(V); break;
      case kMSF   : 
      	V->isseqchar = isGCGSeqChar;
				readMSF(V); 
				break;

      case kPAUP    : {
        boolean done= false;
        boolean interleaved= false;
        char  *cp;
        /* REWINDreader(V); V->nseq= 0; ?? assume it is at top ?? skiplines ... */
        while (!done) {
          getline( V);
          tolowerstr( V->s);
          if (strstr( V->s, "matrix")) done= true;
          if (strstr( V->s, "interleav")) interleaved= true;
          if (NULL != (cp=strstr( V->s, "ntax=")) )  V->topnseq= atoi(cp+5);
          if (NULL != (cp=strstr( V->s, "nchar=")) )  V->topseqlen= atoi(cp+6);
          if (NULL != (cp=strstr( V->s, "matchchar=")) )  {
            cp += 10;
            if (*cp=='\'') cp++;
            else if (*cp=='"') cp++;
            V->matchchar= *cp;
            }
          }
        if (interleaved) readPAUPinterleaved(V);
        else readPAUPsequential(V);
        }
        break;

      /* kPhylip: ! can't determine in middle of file which type it is...*/
      /* test for interleave or sequential and use Phylip4(ileave) or Phylip2 */
      case kPhylip2:
        readPhylipSequential(V);
        break;
      case kPhylip4: /* == kPhylip3 */
        readPhylipInterleaved(V);
        break;

      default:
        V->err = eUnknownFormat;
        break;

      case kFitch :
        strcpy(V->seqid, V->s); getline(V);
        readFitch(V);
        break;

      case kGCG:
        V->isseqchar = isGCGSeqChar;
        do {
          gotuw = (strstr(V->s,"..") != NULL);
          if (gotuw) readUWGCG(V);
          getline(V);
        } while (!(EOFreader(V) || V->allDone));
        break;
      }
    }

  V->filestart= false;
  V->seq[V->seqlen] = 0; /* stick a string terminator on it */
}


char *readSeqFp(
      const short whichEntry,  /* index to sequence in file */
      FILE  *fp,   /* pointer to open seq file */
      const long  skiplines,
      const short format,      /* sequence file format */
      long  *seqlen,     /* return seq size */
      short *nseq,       /* number of seqs in file, for listSeqs() */
      short *error,      /* return error */
      char  *seqid)      /* return seq name/info */
{
  struct ReadSeqVars V;

	gFile= fp; gLinestart = 0;
	V.reader = readWriteFromFile;
	
  if (format < kMinFormat || format > kMaxFormat) {
    *error = eUnknownFormat;
    *seqlen = 0;
    return NULL;
    }

  V.choice = whichEntry;
  V.fname  = NULL;  /* don't know */
  V.seq    = (char*) MALLOC(kStartLength+1);
  V.maxseq = kStartLength;
  V.seqlen = 0;
  V.seqid  = seqid;

  V.f = fp;
  V.filestart= (ftell( fp) == 0); 
  /* !! in sequential read, must remove current seq position from choice/whichEntry counter !! ... */
  if (V.filestart)  V.nseq = 0;
  else V.nseq= *nseq;  /* track where we are in file...*/

  *V.seqid = '\0';
  V.err = 0;
  V.nseq = 0;
  V.isseqchar = isSeqChar;
  if (V.choice == kListSequences) ; /* leave as is */
  else if (V.choice <= 0) V.choice = 1; /* default ?? */
  V.addit = (V.choice > 0);
  V.allDone = false;

  readSeqMain(&V, skiplines, format);

  *error = V.err;
  *seqlen = V.seqlen;
  *nseq = V.nseq;
  return V.seq;
}

char *readSeq(
      const short whichEntry,  /* index to sequence in file */
      const char  *filename,   /* file name */
      const long  skiplines,
      const short format,      /* sequence file format */
      long  *seqlen,     /* return seq size */
      short *nseq,       /* number of seqs in file, for listSeqs() */
      short *error,      /* return error */
      char  *seqid)      /* return seq name/info */
{
  struct ReadSeqVars V;

  if (format < kMinFormat || format > kMaxFormat) {
    *error = eUnknownFormat;
    *seqlen = 0;
    return NULL;
    }

  V.choice = whichEntry;
  V.fname  = filename;  /* don't need to copy string, just ptr to it */
  V.seq    = (char*) MALLOC( kStartLength+1);
  V.maxseq = kStartLength;
  V.seqlen = 0;
  V.seqid  = seqid;

  V.f = NULL;
  *V.seqid = '\0';
  V.err = 0;
  V.nseq = 0;
  V.isseqchar = isSeqChar;
  if (V.choice == kListSequences) ; /* leave as is */
  else if (V.choice <= 0) V.choice = 1; /* default ?? */
  V.addit = (V.choice > 0);
  V.allDone = false;

  V.f = fopen(V.fname, "r");
  V.filestart= true;
	gFile= V.f; gLinestart = 0;
	V.reader = readWriteFromFile;

  readSeqMain(&V, skiplines, format);

  if (V.f != NULL) fclose(V.f);
  *error = V.err;
  *seqlen = V.seqlen;
  *nseq = V.nseq;
  return V.seq;
}



char *readSeqCall(
      const short whichEntry,  /* index to sequence in file */
	    ReadWriteProc reader,
      const long  skiplines,
      const short format,      /* sequence file format */
      long  *seqlen,     /* return seq size */
      short *nseq,       /* number of seqs in file, for listSeqs() */
      short *error,      /* return error */
      char  *seqid)      /* return seq name/info */
{
  struct ReadSeqVars V;

  if (format < kMinFormat || format > kMaxFormat) {
    *error = eUnknownFormat;
    *seqlen = 0;
    return NULL;
    }

  V.choice = whichEntry;
  V.fname  = NULL;  /* don't know */
  V.seq    = (char*) MALLOC( kStartLength+1);
  V.maxseq = kStartLength;
  V.seqlen = 0;
  V.seqid  = seqid;
  *V.seqid = '\0';

  V.f = NULL;
  V.err = 0;
  V.nseq = 0;
  V.isseqchar = isSeqChar;
  if (V.choice == kListSequences) ; /* leave as is */
  else if (V.choice <= 0) V.choice = 1; /* default ?? */
  V.addit = (V.choice > 0);
  V.allDone = false;

	gFile= NULL; gLinestart = 0;
	V.reader = reader;
 	V.filestart= (0 == (*reader)((char*)NULL, 0L, kRSFile_Tell));

  readSeqMain(&V, skiplines, format);

  *error = V.err;
  *seqlen = V.seqlen;
  *nseq = V.nseq;
  return V.seq;
}



char *listSeqs(
      const char  *filename,   /* file name */
      const long skiplines,
      const short format,      /* sequence file format */
      short *nseq,       /* number of seqs in file, for listSeqs() */
      short *error)      /* return error */
{
  char  seqid[256];
  long  seqlen;

	/*fprintf(stderr,"listSeqs filename= %s  format= %d  nseq= %d\n", filename, format, *nseq);*/
  return readSeq( kListSequences, filename, skiplines, format,
                  &seqlen, nseq, error, seqid);
}

char *listSeqsCall(
      ReadWriteProc	reader,
      const long skiplines,
      const short format,      /* sequence file format */
      short *nseq,       /* number of seqs in file, for listSeqs() */
      short *error)      /* return error */
{
  char  seqid[256];
  long  seqlen;

  return readSeqCall( kListSequences, reader, skiplines, format,
                  &seqlen, nseq, error, seqid);
}




short seqFileFormat(    /* return sequence format number, see ureadseq.h */
    const char *filename,
    long  *skiplines,   /* return #lines to skip any junk like mail header */
    short *error)       /* return any error value or 0 */
{
  FILE      *fseq;
  short      format;

  fseq  = fopen(filename, "r");
  format= seqFileFormatFp( fseq, skiplines, error);
  if (fseq!=NULL) fclose(fseq);
  return format;
}

short seqFileFormatFp(
    FILE *fseq,
    long  *skiplines,   /* return #lines to skip any junk like mail header */
    short *error)       /* return any error value or 0 */
{
	gFile = fseq; gLinestart = 0;
	return seqFileFormatCall( readWriteFromFile, skiplines, error);
}

		

short seqFileFormatCall(
    ReadWriteProc reader,
    long  *skiplines,   /* return #lines to skip any junk like mail header */
    short *error)       /* return any error value or 0 */
{
  boolean   foundDNA= false, foundIG= false, foundStrider= false,
            foundGB= false, foundPIR= false, foundEMBL= false, foundNBRF= false,
            foundPearson= false, foundFitch= false, foundPhylip= false, foundZuker= false,
            gotolsen= false, gotpaup = false, gotasn1 = false, gotuw= false, gotMSF= false,
            isfitch= false,  isphylip= false, done= false;
  short     format= kUnknown;
  int       nlines= 0, k, splen= 0, otherlines= 0, aminolines= 0, dnalines= 0;
  char      sp[256];
  long      linestart=0;
  int     	maxlines2check=500;

#define ReadOneLine(sp)   \
 		{ done |= (boolean)((*reader)((char*)NULL, 0L, kRSFile_Eof)); \
 		callreadline( reader, sp);  \
    if (!done) { splen = strlen(sp); ++nlines; } }

/*  { done |= feof(fp); \ */
/*  	readline( fp, sp, &linestart); \ */

	/*fprintf(stderr,"seqFileFormatCall &reader= %ld  &gFile= %ld\n", reader, gFile); */

  *skiplines = 0;
  *error = 0;
  if (reader == NULL) { *error = eFileNotFound;  return kNoformat; }

  while ( !done ) {
    ReadOneLine(sp);

    /* check for mailer head & skip past if found */
    if (nlines < 4 && !done) {
      if ((strstr(sp,"From ") == sp) || (strstr(sp,"Received:") == sp)) {
        do {
          /* skip all lines until find one blank line */
          ReadOneLine(sp);
          if (!done) for (k=0; (k<splen) && (sp[k]==' '); k++) ;
          } while ((!done) && (k < splen));
        *skiplines = nlines; /* !? do we want #lines or #bytes ?? */
        }
      }

    if (sp==NULL || *sp==0)
      ; /* nada */

    /* high probability identities: */

    else if ( strstr(sp,"MSF:") && strstr(sp,"Type:") && strstr(sp,"Check:") )
      gotMSF= true;

    else if ((strstr(sp,"..") != NULL) && (strstr(sp,"Check:") != NULL))
      gotuw= true;

    else if (strstr(sp,"identity:   Data:") != NULL)
      gotolsen= true;

    else if ( strstr(sp,"::=") &&
      (strstr(sp,"Bioseq") ||       /* Bioseq or Bioseq-set */
       strstr(sp,"Seq-entry") ||
       strstr(sp,"Seq-submit") ) )  /* can we read submit format? */
          gotasn1= true;

    else if ( strstr(sp,"#NEXUS") == sp )
      gotpaup= true;

    /* uncertain identities: */

    else if (*sp ==';') {
      if (strstr(sp,"Strider") !=NULL) foundStrider= true;
      else foundIG= true;
      }

    else if (strstr(sp,"LOCUS") == sp)
      foundGB= true;
    else if (strstr(sp,"ORIGIN") == sp)
      foundGB= true;

    else if (strstr(sp,"ENTRY   ") == sp)  /* ? also (strcmp(sp,"\\\\\\")==0) */
      foundPIR= true;
    else if (strstr(sp,"SEQUENCE") == sp)
      foundPIR= true;

    else if (*sp == '>') {
      if (sp[3] == ';') foundNBRF= true;
      else foundPearson= true;
      }

    else if (strstr(sp,"ID   ") == sp)
      foundEMBL= true;
    else if (strstr(sp,"SQ   ") == sp)
      foundEMBL= true;

    else if (*sp == '(')
      foundZuker= true;

    else {
      if (nlines - *skiplines == 1) {
        int ispp= 0, ilen= 0;
        sscanf( sp, "%d%d", &ispp, &ilen);
        if (ispp > 0 && ilen > 0) isphylip= true;
        }
      else if (isphylip && nlines - *skiplines == 2) {
        int  tseq;
        tseq= getseqtype(sp+10, strlen(sp+10));
        if ( isalpha(*sp)     /* 1st letter in 2nd line must be of a name */
         && (tseq != kOtherSeq))  /* sequence section must be okay */
            foundPhylip= true;
        }

      for (k=0, isfitch= true; isfitch && (k < splen); k++) {
        if (k % 4 == 0) isfitch &= (sp[k] == ' ');
        else isfitch &= (sp[k] != ' ');
        }
      if (isfitch && (splen > 20)) foundFitch= true;

      /* kRNA && kDNA are fairly certain...*/
      switch (getseqtype( sp, splen)) {
        case kOtherSeq: otherlines++; break;
        case kAmino   : if (splen>20) aminolines++; break;
        case kDNA     :
        case kRNA     : if (splen>20) dnalines++; break;
        case kNucleic : break; /* not much info ? */
        }

      }

                    /* pretty certain */
    if (gotolsen) {
      format= kOlsen;
      done= true;
      }
    else if (gotMSF) {
      format= kMSF;
      done= true;
      }
    else if (gotasn1) {
      /* !! we need to look further and return  kASNseqentry | kASNseqset */
      /*
        seqentry key is Seq-entry ::=
        seqset key is Bioseq-set ::=
        ?? can't read these yet w/ ncbi tools ??
          Seq-submit ::=
          Bioseq ::=  << fails both bioseq-seq and seq-entry parsers !
      */
      if (strstr(sp,"Bioseq-set")) format= kASNseqset;
      else if (strstr(sp,"Seq-entry")) format= kASNseqentry;
      else format= kASN1;  /* other form, we can't yet read... */
      done= true;
      }
    else if (gotpaup) {
      format= kPAUP;
      done= true;
      }

    else if (gotuw) {
      if (foundIG) format= kIG;  /* a TOIG file from GCG for certain */
      else format= kGCG;
      done= true;
      }

    else if ((dnalines > 1) || done || (nlines > maxlines2check)) {
          /* decide on most likely format */
          /* multichar idents: */
      if (foundStrider) format= kStrider;
      else if (foundGB) format= kGenBank;
      else if (foundPIR) format= kPIR;
      else if (foundEMBL) format= kEMBL;
      else if (foundNBRF) format= kNBRF;
          /* single char idents: */
      else if (foundIG) format= kIG;
      else if (foundPearson) format= kPearson;
      else if (foundZuker) format= kZuker;
          /* digit ident: */
      else if (foundPhylip) format= kPhylip;
          /* spacing ident: */
      else if (foundFitch) format= kFitch;
          /* no format chars: */
      else if (otherlines > 0) format= kUnknown;
      else if (dnalines > 1) format= kPlain;
      else if (aminolines > 1) format= kPlain;
      else format= kUnknown;

      done= true;
      }

    /* need this for possible long header in olsen format */
     else if (strstr(sp,"): ") != NULL)
       maxlines2check++;
    }

  if (format == kPhylip) {
    /* check for interleaved or sequential -- really messy */
    int tname, tseq;
    long i, j, nspp= 0, nlen= 0, ilen, leaf= 0, seq= 0;
    char  *ps;

		(void) (*reader)((char*)NULL, 0L, kRSFile_Rewind);
		/*rewind(fp);*/
    for (i=0; i < *skiplines; i++) ReadOneLine(sp);
    nlines= 0;
    ReadOneLine(sp);
    sscanf( sp, "%d%d", &nspp, &nlen);
    ReadOneLine(sp); /* 1st seq line */
    for (ps= sp+10, ilen=0; *ps!=0; ps++) if (isprint(*ps)) ilen++;

    for (i= 1; i<nspp; i++) {
      ReadOneLine(sp);

      tseq= getseqtype(sp+10, strlen(sp+10));
      tname= getseqtype(sp, 10);
      for (j=0, ps= sp; isspace(*ps) && j<10; ps++, j++)  ;
      for (ps= sp; *ps!=0; ps++) if (isprint(*ps)) ilen++;

      /* find probable interleaf or sequential ... */
      if (j>=9) seq += 10; /* pretty certain not ileaf */
      else {
        if (tseq != tname) leaf++; else seq++;
        if (tname == kDNA || tname == kRNA) seq++; else leaf++;
        }

      if (ilen <= nlen && j<9) {
        if (tname == kOtherSeq) leaf += 10;
        else if (tname == kAmino || tname == kDNA || tname == kRNA) seq++; else leaf++;
        }
      else if (ilen > nlen) {
        ilen= 0;
        }
      }
    for ( nspp *= 2 ; i<nspp; i++) {  /* this should be only bases if interleaf */
      ReadOneLine(sp);

      tseq= getseqtype(sp+10, strlen(sp+10));
      tname= getseqtype(sp, 10);
      for (ps= sp; *ps!=0; ps++) if (isprint(*ps)) ilen++;
      for (j=0, ps= sp; isspace(*ps) && j<10; ps++, j++)  ;
      if (j<9) {
        if (tname == kOtherSeq) seq += 10;
        if (tseq != tname) seq++; else leaf++;
        if (tname == kDNA || tname == kRNA) leaf++; else seq++;
        }
      if (ilen > nlen) {
        if (j>9) leaf += 10; /* must be a name here for sequent */
        else if (tname == kOtherSeq) seq += 10;
        ilen= 0;
        }
      }

    if (leaf > seq) format= kPhylip4;
    else format= kPhylip2;
    }

	/*fprintf(stderr, "seqFileFormatCall &reader= %ld  &gFile= %ld  format= %ld\n", reader, gFile, format);*/
  return(format);
#undef  ReadOneLine
} /* SeqFileFormat */




unsigned long GCGchecksum( const char *seq, const long seqlen, unsigned long *checktotal)
/* GCGchecksum */
{
  register long  i, check = 0, count = 0;
	register char *sp = (char*) seq;

  for (i = 0; i < seqlen; i++) {
    count++;
		check += count * to_upper(*sp);
	  sp++;
    if (count == 57) count = 0;
    }
  check %= 10000;
  *checktotal += check;
  *checktotal %= 10000;
  return check;
}

/* Table of CRC-32's of all single byte values (made by makecrc.c of ZIP source) */
const unsigned long crctab[] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

unsigned long CRC32checksum(const char *seq, const long seqlen, unsigned long *checktotal)
/*CRC32checksum: modified from CRC-32 algorithm found in ZIP compression source */
{
  register unsigned long c = 0xffffffffL;
  register long n = seqlen;
	register char *sp = (char*) seq;

	while (n--) {
		c = crctab[((int)c ^ (to_upper(*sp))) & 0xff] ^ (c >> 8);
		sp++;
		}
  c= c ^ 0xffffffffL;
  *checktotal += c;
  return c;
}




short getseqtype( const char *seq, const long seqlen)
{ /* return sequence kind: kDNA, kRNA, kProtein, kOtherSeq, ??? */
  char  c;
	register char *sp = (char*) seq;
  short i, maxtest;
  short na = 0, aa = 0, po = 0, nt = 0, nu = 0, ns = 0, no = 0;

  maxtest = (short) min(300, seqlen);
  for (i = 0; i < maxtest; i++) {
		c = to_upper(*sp);  sp++;
    if (strchr(protonly, c)) po++;
    else if (strchr(primenuc,c)) {
      na++;
      if (c == 'T') nt++;
      else if (c == 'U') nu++;
      }
    else if (strchr(aminos,c)) aa++;
    else if (strchr(seqsymbols,c)) ns++;
    else if (isalpha(c)) no++;
    }

  if ((no > 0) || (po+aa+na == 0)) return kOtherSeq;
  /* ?? test for probability of kOtherSeq ?, e.g.,
  else if (po+aa+na / maxtest < 0.70) return kOtherSeq;
  */
  else if (po > 0) return kAmino;
  else if (aa == 0) {
    if (nu > nt) return kRNA;
    else return kDNA;
    }
  else if (na > aa) return kNucleic;
  else return kAmino;
} /* getseqtype */


void GetSeqStats( const char* seq, const long seqlen, 
							short* seqtype, unsigned long* checksum, long* basecount)
{
	register unsigned long chk = 0xffffffffL;
  register long n = seqlen;
	register char c, *seqp;
	short		 na = 0, aa= 0, no= 0, ns= 0, nt= 0, po= 0, nu= 0;
	short		 seqt = kOtherSeq;
	unsigned long checks= 0;
	long     basec= 0;
	
	if (seq) {
		na= aa= no= ns= nt= po= nu= 0;
		seqp= (char*) seq;
		while (n--) {
			c= to_upper(*seqp);
			seqp++;
			if (c > ' '  && (c < '0' || c > '9')) { 
				chk = crctab[((int)chk ^ c) & 0xff] ^ (chk >> 8);
			
				basec++;
				if (strchr( protonly, c)) po++;
				else if (strchr(primenuc, c)) {
					na++;
					if (c == 'T') nt++;
					else if (c == 'U') nu++;
					}
				else if (strchr(aminos, c))  aa++;  
				else if (strchr(seqsymbols, c)) ns++;
				else if (isalpha(c)) no++;
				}
			}
	  /* checks= chk ^ 0xffffffffL; */
		if (no > 0 || po+aa+na == 0) seqt = kOtherSeq;
		else if (po > 0) seqt = kAmino;
		else if (aa == 0) {
			if (nu > nt) seqt = kRNA;
			else seqt = kDNA;
			}
		else if (na > aa) seqt = kNucleic;
		else seqt = kAmino;
		}
	if (seqtype) *seqtype= seqt;
	if (checksum) *checksum= chk ^ 0xffffffffL;
	if (basecount) *basecount= basec;
}


char* compressSeq( const char gapc, const char *seq, const long seqlen, long *newlen)
{
  register char *a, *b;
  register long i;
  char  *newseq;

  *newlen= 0;
  if (!seq) return NULL;
  newseq = (char*) MALLOC(seqlen+1);
  if (!newseq) return NULL;
  for (a= (char*)seq, b=newseq, i=0; *a!=0; a++)
    if (*a != gapc) {
      *b++= *a;
      i++;
      }
  *b= '\0';
#ifdef NOTmacintosh
	SetPtrSize( (Ptr)newseq, i+1);
#else
	REALLOC(newseq, i+1);
#endif
  *newlen= i;
  return newseq;
}



/***
char *rtfhead = "{\\rtf1\\defformat\\mac\\deff2 \
{\\fonttbl\
  {\\f1\\fmodern Courier;}{\\f2\\fmodern Monaco;}\
  {\\f3\\fswiss Helvetica;}{\\f4\\fswiss Geneva;}\
  {\\f5\\froman Times;}{\\f6\\froman Palatino;}\
  {\\f7\\froman New Century Schlbk;}{\\f8\\ftech Symbol;}}\
{\\stylesheet\
  {\\s1 \\f5\\fs20 \\sbasedon0\\snext1 name;}\
  {\\s2 \\f3\\fs20 \\sbasedon0\\snext2 num;}\
  {\\s3 \\f1\\f21 \\sbasedon0\\snext3 seq;}}";

char *rtftail = "}";
****/






short writeSeq(FILE *outf, const char *seq, const long seqlen,
                const short outform, const char *seqid)
{
	gFile = outf; gLinestart = 0;
	return writeSeqCall( readWriteFromFile, seq, seqlen, outform, seqid);
}


short writeSeqCall(
								ReadWriteProc writer, 
								const char *seq, const long seqlen,
                const short outform, const char *seqid)
/* dump sequence to standard output */
{
  const short kSpaceAll = -9;
#define kMaxseqwidth  250

  boolean baseonlynum= false; /* nocountsymbols -- only count true bases, not "-" */
  short  numline = 0; /* only true if we are writing seq number line (for interleave) */
  boolean numright = false, numleft = false;
  boolean nameright = false, nameleft = false, dumb = true;
  short   namewidth = 8, numwidth = 8;
  short   spacer = 0, width  = 50, tab = 0;
  /* new parameters: width, spacer, those above... */
	boolean dofreeseq= false;
	
  short linesout = 0, seqtype = kNucleic;
  long  i, j, l, l1, ibase;
  char  idword[31], endstr[30];
  char  seqnamestore[128], *seqname = seqnamestore;
  char  s[kMaxseqwidth], *cp;
  char  nameform[30], numform[30], nocountsymbols[30];
  unsigned long checksum = 0, checktotal = 0;
	char	outbuf[512];

#define FPUTC(c)	{outbuf[0]=c; outbuf[1]=0; (void)(*writer)(outbuf, 255, kRSFile_Write);}
#define FPUTS(s)	(void)(*writer)(outbuf, 255L, kRSFile_Write)

	/*fprintf(stderr,"writeSeqCall &writer= %ld  &gFile= %ld\n", writer, gFile);*/

  gPretty.atseq++;
  /* skipwhitespace(seqid); //???? */
  strncpy( seqnamestore, seqid, sizeof(seqnamestore));
  seqname[sizeof(seqnamestore)-1] = 0;

  sscanf( seqname, "%30s", idword);
  sprintf(numform, "%d", seqlen);
  numwidth= strlen(numform)+1;
  nameform[0]= '\0';

  if (strstr(seqname,"checksum") != NULL) {
    cp = strstr(seqname,"bases");
    if (cp!=NULL) {
      for ( ; (cp!=seqname) && (*cp!=','); cp--) ;
      if (cp!=seqname) *cp=0;
      }
    }

  strcpy( endstr,"");
  l1 = 0;

  if (outform == kGCG || outform == kMSF) {
  	/* ! translate quasi-standard '-' indel to '.' that gcg uses */
  	/* must do before checksum... */
  	if (memchr(seq,'-',seqlen)) {
  		char *newseq, *nc, *sc;
  		newseq = (char*) MALLOC(seqlen+1);
			for (i=0, sc= (char*)seq, nc= newseq; i<=seqlen; i++, sc++, nc++) {
				if (*sc == '-') *nc= '.'; 
				else *nc= *sc;
				}
			seq= (const char*) newseq;
			dofreeseq= true;
  		}
    checksum = GCGchecksum(seq, seqlen, &checktotal);
    }
  else
    checksum = seqchecksum(seq, seqlen, &checktotal);

  switch (outform) {

    case kPlain:
    case kUnknown:    /* no header, just sequence */
      strcpy(endstr,"\n"); /* end w/ extra blank line */
      break;

    case kOlsen:  /* Olsen seq. editor takes plain nucs OR Genbank  */
    case kGenBank:
      sprintf(outbuf,"LOCUS       %s       %d bp\n", idword, seqlen);  FPUTS(outbuf);
      sprintf(outbuf,"DEFINITION  %s  %d bases  %X checksum\n",  
						seqname, seqlen, checksum); FPUTS(outbuf);
   /* sprintf(outbuf,"ACCESSION   %s\n", accnum); FPUTS(outbuf); */
      sprintf(outbuf,"ORIGIN      \n" ); FPUTS(outbuf);
      spacer = 11;
      numleft = true;
      numwidth = 8;  /* dgg. 1Feb93, patch for GDE fail to read short numwidth */
      strcpy(endstr, "\n//");
      linesout += 4;
      break;

    case kPIR:
      /* somewhat like genbank... \\\*/
      /* sprintf(outbuf,"\\\\\\\n"); FPUTS(outbuf); << only at top of file, not each entry... */
      sprintf(outbuf,"ENTRY           %s \n", idword); FPUTS(outbuf);
      sprintf(outbuf,"TITLE           %s  %d bases  %X checksum\n", 
					seqname, seqlen, checksum); FPUTS(outbuf);
   /* sprintf(outbuf,"ACCESSION       %s\n", accnum); FPUTS(outbuf); */
      sprintf(outbuf,"SEQUENCE        \n" ); FPUTS(outbuf);
      numwidth = 7;
      width= 30;
      spacer = kSpaceAll;
      numleft = true;
      strcpy(endstr, "\n///");
      /* run a top number line for PIR */
      for (j=0; j<numwidth; j++) FPUTC(' ');
      for (j= 5; j<=width; j += 5) { sprintf(outbuf,"%10d", j ); FPUTS(outbuf); }
      FPUTC('\n');
      linesout += 5;
      break;

    case kNBRF:
      if (getseqtype(seq, seqlen) == kAmino)
        { sprintf(outbuf,">P1;%s\n", idword );  FPUTS(outbuf);}
      else
        { sprintf(outbuf,">DL;%s\n", idword );  FPUTS(outbuf);}
      sprintf(outbuf,"%s  %d bases  %X checksum\n", 
				seqname, seqlen, checksum); FPUTS(outbuf);
      spacer = 11;
      strcpy(endstr,"*\n");
      linesout += 3;
      break;

    case kEMBL:
      sprintf(outbuf,"ID   %s\n", idword ); FPUTS(outbuf);
  /*  sprintf(outbuf,"AC   %s\n", accnum ); FPUTS(outbuf); */
      sprintf(outbuf,"DE   %s  %d bases %X checksum\n", 
						seqname, seqlen, checksum); FPUTS(outbuf);
      sprintf(outbuf,"SQ             %d BP\n", seqlen ); FPUTS(outbuf);
      strcpy(endstr, "\n//"); /* 11Oct90: bug fix*/
      tab = 4;     /** added 31jan91 */
      spacer = 11; /** added 31jan91 */
      width = 60;
      linesout += 4;
      break;

    case kGCG:
      sprintf(outbuf,"%s\n", seqname ); FPUTS(outbuf);
   /* sprintf(outbuf,"ACCESSION   %s\n", accnum );  FPUTS(outbuf);*/
      sprintf(outbuf,"    %s  Length: %d  (today)  Check: %d  ..\n", 
				idword, seqlen, checksum ); FPUTS(outbuf);
      spacer = 11;
      numleft = true;
      strcpy(endstr, "\n");  /* this is insurance to help prevent misreads at eof */
      linesout += 3;
      break;

    case kStrider: /* ?? map ?*/
      sprintf(outbuf,"; ### from DNA Strider ;-)\n" ); FPUTS(outbuf);
      sprintf(outbuf,"; DNA sequence  %s  %d bases  %X checksum\n;\n", 
				seqname, seqlen, checksum); FPUTS(outbuf);
      strcpy(endstr, "\n//");
      linesout += 3;
      break;

    case kFitch:
      sprintf(outbuf,"%s  %d bases  %X checksum\n", 
				seqname, seqlen, checksum); FPUTS(outbuf);
      spacer = 4;
      width = 60;
      linesout += 1;
      break;

    case kPhylip2:
    case kPhylip4:
      /* this is version 3.2/3.4 -- simplest way to write
        version 3.3 is to write as version 3.2, then
        re-read file and interleave the species lines */
      if (strlen(idword)>10) idword[10] = 0;
      sprintf(outbuf,"%-10s  ", idword ); FPUTS(outbuf);
      l1  = -1;
      tab = 12;
      spacer = 11;
      break;

    case kASN1:
      seqtype= getseqtype(seq, seqlen);
      switch (seqtype) {
        case kDNA     : cp= "dna"; break;
        case kRNA     : cp= "rna"; break;
        case kNucleic : cp= "na"; break;
        case kAmino   : cp= "aa"; break;
        case kOtherSeq: cp= "not-set"; break;
        }
      sprintf(outbuf,"  seq {\n" ); FPUTS(outbuf);
      sprintf(outbuf,"    id { local id %d },\n", gPretty.atseq ); FPUTS(outbuf);
      sprintf(outbuf,"    descr { title \"%s\" },\n",  seqid ); FPUTS(outbuf);
      sprintf(outbuf,"    inst {\n", dumb ); FPUTS(outbuf);
      sprintf(outbuf,"      repr raw, mol %s, length %d, topology linear,\n", 
				cp, seqlen ); FPUTS(outbuf);
      sprintf(outbuf,"      seq-data\n" ); FPUTS(outbuf);
      if (seqtype == kAmino)
        { sprintf(outbuf,"        iupacaa \""); FPUTS(outbuf);}
      else
        { sprintf(outbuf,"        iupacna \"" ); FPUTS(outbuf);}
      l1  = 17;
      spacer = 0;
      width  = 78;
      tab  = 0;
      strcpy(endstr,"\"\n      } } ,");
      linesout += 7;
      break;

    case kPAUP:
      nameleft= true;
      namewidth = 9;
      spacer = 21;
      width  = 100;
      tab  = 0; /* 1; */
      /* strcpy(endstr,";\nend;"); << this is end of all seqs.. */
      /* do a header comment line for paup */
      sprintf(outbuf,"[Name: %-16s  Len:%6d  Check: %8X]\n", 
				idword, seqlen, checksum ); FPUTS(outbuf);
      linesout += 1;
      break;

    case kPretty:
      numline= gPretty.numline;
      baseonlynum= gPretty.baseonlynum;
      namewidth = gPretty.namewidth;
      numright = gPretty.numright;
      numleft = gPretty.numleft;
      nameright = gPretty.nameright;
      nameleft = gPretty.nameleft;
      spacer = gPretty.spacer + 1;
      width  = gPretty.seqwidth;
      tab  = gPretty.tab;
      /* also add rtf formatting w/ font, size, style */
      if (gPretty.nametop) {
        sprintf(outbuf,"Name: %-16s  Len:%6d  Check: %8X\n", 
					idword, seqlen, checksum ); FPUTS(outbuf);
        linesout++;
        }
      break;

    case kMSF:
      sprintf(outbuf," Name: %-16s Len:%6d  Check:%5d  Weight:  1.00\n",
                     idword, seqlen, checksum ); FPUTS(outbuf);
      linesout++;
      nameleft= true;
      namewidth= 15; /* need MAX namewidth here... */
      sprintf(nameform, "%%+%ds ",namewidth);
      spacer = 11;
      width  = 50;
      tab = 0; /* 1; */
      break;

    case kIG:
      sprintf(outbuf,";%s  %d bases  %X checksum\n",  
				seqname, seqlen, checksum ); FPUTS(outbuf);
      sprintf(outbuf,"%s\n",  idword ); FPUTS(outbuf);
      strcpy(endstr,"1"); /* == linear dna */
      linesout += 2;
      break;

    default :
    case kZuker: /* don't attempt Zuker's ftn format */
    case kPearson:
      sprintf(outbuf,">%s  %d bases  %X checksum\n",  
				seqname, seqlen, checksum ); FPUTS(outbuf);
      linesout += 1;
      break;
    }

  if (*nameform==0) sprintf(nameform, "%%%d.%ds ",namewidth,namewidth);
  if (numline) sprintf(numform, "%%%ds ",numwidth);
  else sprintf(numform, "%%%dd ",numwidth);
  strcpy( nocountsymbols, kNocountsymbols);
  if (baseonlynum) {
    if (strchr(nocountsymbols,gPretty.gapchar)==NULL) {
      strcat(nocountsymbols," ");
      nocountsymbols[strlen(nocountsymbols)-1]= gPretty.gapchar;
      }
    if (gPretty.domatch && (cp=strchr(nocountsymbols,gPretty.matchchar))!=NULL) {
      *cp= ' ';
      }
    }

  if (numline) {
   *idword= 0;
   }

  width = min(width,kMaxseqwidth);
  for (i=0, l=0, ibase = 1; i < seqlen; ) {

    if (l1 < 0) l1 = 0;
    else if (l1 == 0) {
      if (nameleft) { sprintf(outbuf, nameform,  idword ); FPUTS(outbuf);}
      if (numleft) { if (numline) { sprintf(outbuf, numform,  "" ); FPUTS(outbuf);}
                    else { sprintf(outbuf, numform, ibase );  FPUTS(outbuf);}
										}
      for (j=0; j<tab; j++) FPUTC(' ');
      }

    l1++;                 /* don't count spaces for width*/
    if (numline) {
      if (spacer==kSpaceAll || (spacer != 0 && (l+1) % spacer == 1)) {
        if (numline==1) FPUTC(' ');
        s[l++] = ' ';
        }
      if (l1 % 10 == 1 || l1 == width) {
        if (numline==1) { sprintf(outbuf,"%-9d ", i+1 ); FPUTS(outbuf);}
        s[l++]= '|'; /* == put a number here */
        }
      else s[l++]= ' ';
      i++;
      }

    else {
      if (spacer==kSpaceAll || (spacer != 0 && (l+1) % spacer == 1))
        s[l++] = ' ';
      if (!baseonlynum) ibase++;
      else if (0==strchr(nocountsymbols,seq[i])) ibase++;
      s[l++] = seq[i++];
      }

    if (l1 == width || i == seqlen) {
      if (outform==kPretty) for ( ; l1<width; l1++) {
        if (spacer==kSpaceAll || (spacer != 0 && (l+1) % spacer == 1))
          s[l++] = ' ';
        s[l++]=' '; /* pad w/ blanks */
        }
      s[l] = '\0';
      l = 0; l1 = 0;

      if (numline) {
        if (numline==2) { sprintf(outbuf,"%s", s );  FPUTS(outbuf);} /* finish numberline ! and | */
        }
      else {
        if (i == seqlen) { sprintf(outbuf,"%s%s", s,endstr ); FPUTS(outbuf); }
        else { sprintf(outbuf,"%s", s ); FPUTS(outbuf);}
        if (numright || nameright) FPUTC(' ');
        if (numright)  { sprintf(outbuf,numform,  ibase-1 ); FPUTS(outbuf);}
        if (nameright) { sprintf(outbuf, nameform, idword ); FPUTS(outbuf);}
        }
      FPUTC('\n');
      linesout++;
      }
    }
  if (dofreeseq) FREE((char*)seq);
  return linesout;
	
#undef FPRINTF 
#undef FPUTC 
}  /*writeSeq*/



/* End file: ureadseq.c */
