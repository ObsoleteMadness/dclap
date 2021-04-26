/*
* ===========================================================================
*
*                            PUBLIC DOMAIN NOTICE                          
*               National Center for Biotechnology Information
*                                                                          
*  This software/database is a "United States Government Work" under the   
*  terms of the United States Copyright Act.  It was written as part of    
*  the author's official duties as a United States Government employee and 
*  thus cannot be copyrighted.  This software/database is freely available 
*  to the public for use. The National Library of Medicine and the U.S.    
*  Government have not placed any restriction on its use or reproduction.  
*                                                                          
*  Although all reasonable efforts have been taken to ensure the accuracy  
*  and reliability of the software and data, the NLM and the U.S.          
*  Government do not and cannot warrant the performance or results that    
*  may be obtained by using this software or data. The NLM and the U.S.    
*  Government disclaim all warranties, express or implied, including       
*  warranties of performance, merchantability or fitness for any particular
*  purpose.                                                                
*                                                                          
*  Please cite the author in any work or product based on this material.   
*
* ===========================================================================
*
* File Name: blstout2.c
*
* Author:  Roman L. Tatusov, Warren Gish, Jonathan Epstein, Tom Madden, Yuri Sadykov
*
* Version Creation Date:   06/16/95
*
* $Revision: 4.0 $
*
* File Description: 
*       Creating BLAST report
*       Warren Gish's utilities needed for producing BLAST output
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*
*
* RCS Modification History:
* $Log: blstout2.c,v $
 * Revision 4.0  1995/07/26  13:55:34  ostell
 * force revision to 4.0
 *
 * Revision 1.4  1995/07/19  15:00:44  madden
 * Changed some spacings in TraditionalHistBlastOutput.
 *
 * Revision 1.3  1995/07/18  21:06:51  madden
 * changed p-value etc. to p_value etc.
 *
 * Revision 1.2  1995/06/22  17:03:44  madden
 * HitDataPtr replaced by BLAST0ResultPtr.
 *
 * Revision 1.1  1995/06/16  11:26:47  epstein
 * Initial revision
 *
*/

#include <ncbi.h>
#include <objseq.h>
#include <objsset.h>
#include <sequtil.h>
#include <seqport.h>
#define NLM_GENERATED_CODE_PROTO
#include <netblap2.h>
#include <objblst2.h>

#define BLASTCLI_MATRIX_SIZE 26

static char ralpha[256];
static Int4 matr[BLASTCLI_MATRIX_SIZE+1][BLASTCLI_MATRIX_SIZE+1];
static int prots;
double la2;

/*************************************************************
*                                                             *
*  Warren Gish's utilities needed for producing BLAST output  *
*                                                             *
 *************************************************************/

/* wrap -- wordwrap lines of output */

static void	doindent PROTO((FILE *fp, int ncols));
static CharPtr	gish_str_strand PROTO ((Uint2 strand));
static Int2 format_an_id PROTO ((CharPtr id, ValNodePtr vnp, Int2 max_id_length));
static Int2 get_max_ID_length PROTO ((BLAST0HitListPtr hlp));
static void get_frame PROTO ((BLAST0SeqIntervalPtr loc, CharPtr array, Int4 total_length));

static int _cdecl
wrap(fp, title, s, slen, linelen, indent)
	FILE	*fp;	/* the output stream */
	char	*title;	/* title string */
	char	*s;	/* pointer to null-terminated string for output */
	int		slen;	/* strlen(s), or -1 */
	int		linelen,	/* max. length of an output line before each '\n' */
			indent;		/* no. of columns to indent any continuation lines */
{
	register char	*savep, *savep2;
	register char	*cp, ch;
	int		outlen, len, olinelen;
	int		titlelen;
	Boolean	once = TRUE;

	if (title != NULL) {
		cp = title;
		len = 0;
		while ((ch = *cp++) != NULLB) {
			fputc(ch, fp);
			if (ch == '\n')
				len = cp - title;
		}
		titlelen = (cp - title) - len - 1;
	}
	else
		titlelen = 0;

	if (slen < 0)
		slen = strlen(s);

	if (indent >= linelen) {
		indent = MIN(1 + titlelen, linelen-5);
		indent = MAX(indent, 0);
	}
	olinelen = linelen;
	linelen -= titlelen;
	for (cp = s; cp < s + slen;) {
		/* Skip leading white space */
		for (;; ++cp) {
			if ((ch = *cp) == NULLB)
				goto Done;
			if (!IS_WHITESP(ch))
				break;
		}

		outlen = cp - s;
		if (slen - outlen <= linelen) {
			/* Remainder is short enough to fit on one line */
			if (!once)
				doindent(fp, indent);
			fprintf(fp, "%s\n", cp);
			break;
		}
		else {
			savep2 = cp + linelen;
			ch = *savep2;
			if (IS_WHITESP(ch)) {
Phase2:
				for (savep = savep2; savep >= cp; --savep)
					if (!IS_WHITESP(*savep)) {
						++savep;
						break;
					}
			}
			else {
				for (savep = savep2; savep >= cp; --savep)
					if (IS_WHITESP(*savep)) {
						savep2 = savep;
						goto Phase2;
					}
				/* a _very_ long word here! */
				savep = savep2;
			}
		}
		if (!once)
			doindent(fp, indent);
		once = FALSE;
		while (cp < savep)
			fputc(*cp++, fp);
		putc('\n', fp);
		cp = savep2;
		linelen = olinelen - indent;
	}

Done:
	return ferror(fp);
}

static void
doindent(fp, ncols)
	register FILE	*fp;
	register int	ncols;
{
	while (ncols-- > 0)
		fputc(' ', fp);
}

/**** MISC  ****/

static CharPtr
gish_str_strand(Uint2 strand)
{
    switch (strand)
    {
    case BLAST0_Seq_interval_strand_plus:
    case BLAST0_Seq_interval_strand_plus_rf:
        return "Plus"; /* plus */
    case BLAST0_Seq_interval_strand_minus:
    case BLAST0_Seq_interval_strand_minus_rf:
        return "Minus"; /* minus */
    case BLAST0_Seq_interval_strand_both:
    default: /* unknown, or unable to convert */
        return "Undefined";
    }
}

static BLAST0ResponsePtr _find(BLAST0ResponsePtr p, Uint1 v)
{
	BLAST0ResponsePtr b;

	for (b = p; b != NULL && b->choice != v; b = b->next) 
		;
	return b;
}

static Pointer find(BLAST0ResponsePtr p, Uint1 v)
{
	BLAST0ResponsePtr b;

	for (b = p; b != NULL && b->choice != v; b = b->next) 
		;

	return (b == NULL) ? NULL : b->data.ptrvalue;
}

static CharPtr
print_double(CharPtr pCh, double dX, int iWidth, int iPrecision)
{
   double dY;
   int    i;
   
   dY = log(dX * 1.000000001) / NCBIMATH_LN10;
   i = dY;
   dY = Nlm_Powi(10., i - iPrecision + 1);
   dX = Nlm_Nint(dX / dY) * dY;
   dY = Nlm_Powi(10., iPrecision - 1) - 1.e-7;
   if (dX >= dY)
      sprintf(pCh, "%*.0lf", iWidth, dX);
   else
      sprintf(pCh, "%*.*lf", iWidth, iPrecision - i - 1, dX);
   
   return pCh;
}

/*
The following function serves a very important purpose.  It translates
the different alphabets into ncbistdaa.  This is necessary for 
calculations involving the BLOSUM/PAM matrics.
*/

static void get_ralpha(char *ralpha, Uint1 code)
{
	SeqCodeTablePtr sctp;
	int index_i, index_j;

	sctp = SeqCodeTableFind(code);
	for (index_i = 0; index_i < 256; index_i++) {
		index_j = GetResidueForSymbol(sctp, index_i);
		if (index_j == 255) 
			index_j = 0;
		ralpha[index_i] = index_j;
	}
}

/***************************************************************************
*
*	Format the top part of the "traditional" BLAST output.
*	This includes the name of the program, the version, the compile
*	date, the reference down through "Searching...":
*
*BLASTN 1.4.7MP [16-Oct-94] [Build 16:01:48 Dec 16 1994]
*
*Reference:  Altschul, Stephen F., Warren Gish, Webb Miller, Eugene W. Myers,
*and David J. Lipman (1990).  Basic local alignment search tool.  J. Mol. Biol.
*215:403-10.
*
*Notice:  this program and its default parameter settings are optimized to find
*nearly identical sequences rapidly.  To identify weak similarities encoded in
*nucleic acid, use BLASTX, TBLASTN or TBLASTX.
*
*Query=  195747|Mitochondrion Rupicapra rupi
*        (646 letters)
*
*Database:  Non-redundant PDB+GBupdate+GenBank+EMBLupdate+EMBL
*           248,447 sequences; 236,652,708 total letters.
*Searching..................................................done
*
*	The matrix used for the BLAST run is also read in.
*	
**************************************************************************/
static Boolean
TraditionalHeadBlastOutput(BLAST0ResponsePtr blresp, Int4Ptr query_length, CharPtr program, FILE *fp)
{
	BLAST0ResponsePtr a;
	Scores_scaled_intsPtr sco;
   	ValNodePtr   ints, sc;
	BLAST0MatrixPtr ma;
	BLAST0KABlkPtr ka;
	BLAST0StatusPtr status;
	BLAST0PrefacePtr pre;
	BLAST0SequencePtr qu;
	BLAST0DbDescPtr db;
	BLAST0JobDescPtr jobd;
	Int2 index1, index2, length;
	Int4 progr;
	Char buf[30];
	CharPtr string;
	ValNodePtr node;
	CharPtr defl;
	
	if ((pre = find(blresp, BLAST0Response_preface)) != NULL) {
		prots = (pre->program[5] != 'N');
		fprintf(fp, "%s %s [%s]", pre->program, pre->version, pre->dev_date);
		if (pre->bld_date != NULL) {
			fprintf(fp, " [Build %s]", pre->bld_date);
		}
		fprintf(fp, "\n");
		for (node = pre->cit; node != NULL; node = node->next) {
			string = (CharPtr) node->data.ptrvalue;
			if (node == pre->cit)
				wrap(fp, "\nReference:  ", string, strlen(string), 79, 0);
			else
				wrap(fp, "", string, strlen(string), 79, 0);
		}
		for (node = pre->notice; node != NULL; node = node->next) {
			string = (CharPtr) node->data.ptrvalue;
			wrap(fp, "\nNotice:  ", string, strlen(string), 79, 0);
		}
	}
	if ((qu = find(blresp, BLAST0Response_query)) != NULL) {
		fprintf(fp, "\nQuery=  ");
		if (qu != NULL && qu->desc != NULL &&
		    qu->desc->defline != NULL) {
		    node = qu->desc->id;
		    while (node)
		    {
			if (node->choice == BLAST0SeqId_giid)
			{
				sprintf(buf, "gi|%ld|", (long) node->data.intvalue);
				string = &buf[0];
			}
			else if (node->choice == BLAST0SeqId_textid)
			{
		    		string = node->data.ptrvalue;
		    		if (strncmp(string, "lcl|", 4) == 0)
				{
					sprintf(buf, "%s ", string+4);
					string = &buf[0];
				}
			}
		    	fprintf(fp, "%s", string);
			node = node->next;
		    }
		    defl = qu->desc->defline;
		    wrap(fp, " ", defl, strlen(defl), 79, 8);
		} else {
			fprintf(fp, "Unknown\n");
		}
		*query_length = qu->length;
		fprintf(fp, "        (%s letters)\n", Nlm_Ultostr(qu->length, 1));
	}

	if (StringCmp(program, "tblastx") == 0)
	{
		fprintf(fp, "\n  Translating both strands of query sequence in all 6 reading frames\n");
	}

	if ((db = find(blresp, BLAST0Response_dbdesc)) != NULL) {
		string = db->def;
		wrap(fp, "\nDatabase:  ", string, strlen(string), 79, 11);
		fprintf(fp, "           %s sequences; %s total letters.\n",
			Nlm_Ultostr((long)(db->count), 1), Nlm_Ultostr(db->totlen, 1));
	}

	a = blresp;
	while ((a = _find(a, BLAST0Response_job_start)) != NULL) {
		jobd = a->data.ptrvalue;
		if (jobd->jid != BLAST0_Job_desc_jid_search) {
			a = a->next;
			continue;
		}
		fprintf(fp, "%s", jobd->desc);
		fflush(fp);
		progr = BLAST0Response_job_progress;
		for (a = a->next; a != NULL && a->choice == progr; a = a->next) {
			fprintf(fp, ".");
			fflush(fp);
		}
		if (a != NULL && a->choice == BLAST0Response_job_done) {
			fprintf(fp, "done\n");
		} else {
			fprintf(fp, "Interrupted!!!\n");
		}
		fflush(fp);
	}
	
	if ((status = find(blresp, BLAST0Response_status)) != NULL) {
		if (status->code != 0) {
			fprintf(fp, "ERROR: %ld\n", (long) status->code);
			fprintf(fp, "reason: %s\n", status->reason);
		}
	}
	fflush(fp);
	if ((ma = find(blresp, BLAST0Response_matrix)) != NULL) {
		sc = ma->Scores_scores;
		sco = sc->data.ptrvalue;
		ints = sco->ints;
		if (ints)
		{
		   for (index1=0; index1<BLASTCLI_MATRIX_SIZE; index1++)
		       for (index2=0; index2<BLASTCLI_MATRIX_SIZE; index2++)
		       {
		   	   matr[index1][index2] = ints->data.intvalue;
			   if (ints->next == NULL)
			   {	/* Get out of both loops! */
				index1 = BLASTCLI_MATRIX_SIZE;
				index2 = BLASTCLI_MATRIX_SIZE;
				break;
			   }
			   ints = ints->next;
		       }
		}
		get_ralpha(ralpha, Seq_code_ncbistdaa);
	}
	if ((ka = find(blresp, BLAST0Response_kablk)) != NULL) {
		la2 = ka->lambda / log(2.);  /* lambda / ln2  */
	}
	return TRUE;
}

#define PAGE_W 80
#define EXPECT_W 6
#define EXPECT_PRCSN 3
#define SEPARATOR_SYMBOL '|'
#define DRAWING_SYMBOL '='
#define SMALLDRAWING_SYMBOL ':'
/**************************************************************************
 *    The function formats histgram data and is part of the "traditional"
 * BLAST output.
 * 
 * Note:
 *    If pointer on the histogram structure == NULL it doesn't seem as
 * error, simply it means no histogram data have been requested. In this
 * case corresponding message is printed to output.
 * 
 **************************************************************************/
static Boolean
TraditionalHistBlastOutput(BLAST0ResultPtr hdp, FILE *fp)
{
   BLAST0HistogramPtr pHist = hdp->hist;
   Int4 nBars;     /* Number of histogram bars */
   Int4 i;
   Int4 nMaxObserved;
   Int4 nMaxHist;
   Int4 nHistValue;
   Int2 nObsWidth; /* Width (in symbols) of field wich represents observed value */
   Int2 nHistWidth;/* Width (in symbols) of field wich represents histogram value */
   Int2 nCols;     /* Width of space (in symbols) where histogram is painted */
   Int2 nPerCol;   /* Number of sequences per unit */
   Int2 nUnits;    /* Number units needed to represent histogram bar */
   Int2 j;
   FloatLo fDelta;
   Char aCh[20];
   Boolean bNeedCheck = TRUE;
   BLAST0HistogramBarPtr pBar;
      
   
   if (pHist == NULL)
      return TRUE;
   
   
   putc('\n', fp);
   putc('\n', fp);
   fprintf(fp, "     Observed Numbers of Database Sequences Satisfying\n");
   fprintf(fp, "    Various EXPECTation Thresholds (E parameter values)\n");
   putc('\n', fp);

   nBars  = pHist->nbars;
   
   if (nBars == 0) {
      fprintf(fp, "\n\n***  histogram slots are all empty ***\n\n");
      return TRUE;
   }
      
   /* Find out max observed and histogram data values
    */
   for (i = 0, nMaxObserved = 0, nMaxHist = 0, pBar = pHist->bar;
	i < nBars; i++) {
      nMaxObserved = MAX(nMaxObserved, pBar->n);
       
      if (i == nBars - 1) /* the last one */
	 nMaxHist = MAX(nMaxHist, pBar->n - pHist->base);
      else
	 nMaxHist = MAX(nMaxHist, pBar->n - pBar->next->n);
       
      pBar = pBar->next;
   }
   
   nObsWidth = Nlm_Ulwidth(nMaxObserved, 0);
   nObsWidth = MAX(nObsWidth, 2);
   nHistWidth = Nlm_Ulwidth(nMaxHist, 0);
   nHistWidth = MAX(nHistWidth, 2);
   
   nCols = PAGE_W - (EXPECT_W+1 + nObsWidth+1 + nHistWidth+1 + 2);
   fDelta = (float)nMaxHist / (float)nCols;
   nPerCol = ceil(fDelta);
   nPerCol = MAX(nPerCol, 1);
   
   fprintf(fp, "        Histogram units:      %c %d Sequence%s",
	   DRAWING_SYMBOL, nPerCol, (nPerCol == 1 ? "" : "s"));

    if (fDelta > 1.0)
      fprintf(fp, "     %c less than %d sequences\n", SMALLDRAWING_SYMBOL, nPerCol);
    else
      putc('\n', fp);
    
   putc('\n', fp);
   fprintf(fp, " EXPECTation Threshold\n");
   fprintf(fp, " (E parameter)\n");
   fprintf(fp, "    |\n");
   fprintf(fp, "    V   Observed Counts-->\n");
   
   /* Draw the histogram
    */
   for (i = 0, pBar = pHist->bar; i < nBars; i++) 
   {
      if ( bNeedCheck && (pBar->x <= pHist->expect + 1e-6)) {
	 /* This line must appear only once
	  */
	 fprintf(fp,
		 " >>>>>>>>>>>>>>>>>>>>>  Expect = %#0.3lg, Observed = %lu  <<<<<<<<<<<<<<<<<\n",
		 pHist->expect, pHist->observed);
	 bNeedCheck = FALSE;
      }

      nHistValue = (i < nBars - 1) ? (pBar->n - pBar->next->n) : (pBar->n - pHist->base);
      fprintf(fp, " %s %*lu %*lu %c", print_double(aCh, pBar->x, 6, 3),
	      nObsWidth, pBar->n, nHistWidth, nHistValue, SEPARATOR_SYMBOL);

      /* Draw long line
       */
      if (fDelta <= 1.00) {
	  /* Don't stretch on whole width
	   */
	  for (j = 0; j < nHistValue; j++)
	    putc(DRAWING_SYMBOL, fp);
      } else {
	  /* Stretch on whole width
	   */
	  nUnits = (FloatLo) nHistValue / fDelta;
	  for (j = 0; j < nUnits; j++)
	    putc(DRAWING_SYMBOL, fp);
      }
          
      /* Draw tiny line if needed
       */
      if (j == 0 && nHistValue > 0 && fDelta > 1.00)
	putc(SMALLDRAWING_SYMBOL, fp);
      
      putc('\n', fp);
      
      pBar = pBar->next;
   }
   
   return TRUE;
}

#define DEF_CUTOFF 43
#define DEF_AND_ID_CUTOFF 59
#define BLASTCLI_ID_MAX 25

/**************************************************************************
*	
*	This function formats the One-line summaries of the traditional
*	BLAST output.  This includes the High-Scoring Segment Pair (HSP) 
*	id, the defline of the HSP, the high score of that HSP, the P 
*	value, and the number of HSP's found.  If a translation 
*	was performed on the database, the frame is also printed.  An
*	example is:
*
*                                                                     Smallest
*                                                                       Sum
*                                                              High  Probability
*Sequences producing High-scoring Segment Pairs:              Score  P(N)      N
*
*dbj|D32197|RURMTCB27 Mitochondrion Rupicapra rupicapra ge...  3230  7.0e-264  1
*dbj|D32191|CCRMTCB21 Mitochondrion Capricornis crispus ge...  2609  1.8e-212  1
*gb|U17862|OMU17862   Ovibos moschatus moschatus cytochrom...  2600  1.0e-211  1
*dbj|D32195|CCRMTCB25 Mitochondrion Capricornis sumatrensi...  2582  5.6e-210  1
*gb|U17861|NCU17861   Nemorhaedus caudatus cytochrome b ge...  2573  1.8e-209  1
*
*	and so on.
******************************************************************************/

static Boolean
TraditionalTopBlastOutput(BLAST0ResultPtr hdp, Int4 query_length, CharPtr program, FILE *fp)
{
    BLAST0HitListPtr hlp;
    BLAST0SegmentPtr segp;
    /* Int4 dim; */
    BLAST0SeqIntervalPtr hit;
    BLAST0SeqDescPtr sdp, sdp1;
    CharPtr defline=NULL, ptr;
    Char frame_array[3], buffer[DEF_CUTOFF+5];
    ScorePtr score; 
    CharPtr scoreDescr;
    BLAST0HSPPtr hsp;
    Int4 highScore, string_length;
    FloatHi poissonScore;
    Int2 index, max_id_length, max_def_length, length, length_left, old_length, total_length;
    Int4 nScore;
    Int4 TemphighScore;
    FloatHi TemppoissonScore;
    Int4 TempnScore;
    CharPtr str1;
    Char str2[DEF_CUTOFF+4];
    Char id[BLASTCLI_ID_MAX+1];

    fprintf(fp, "\n%-69sSmallest\n%-71sSum\n", "", "");
    if (StringCmp("blastn", program) == 0 || 
	StringCmp("blastp", program) == 0)
    {
    	fprintf(fp, "%-62sHigh  Probability\n", "");
        fprintf(fp, "Sequences producing High-scoring Segment Pairs:%-14sScore  P(N)      N\n\n", "");
    }
    else
    {
    	fprintf(fp, "%-53sReading  High  Probability\n", "");
        fprintf(fp, "Sequences producing High-scoring Segment Pairs:%-8sFrame Score  P(N)      N\n\n", "");
    }

    if (hdp == NULL || hdp->hitlists == NULL) {
	    if (hdp->hitlists == NULL) {
	    	fprintf(fp, "\n      *** NONE ***\n");
	    }
        return FALSE;
    }
 
/*    dim = hdp->dim;*/ /* should be 2, or 3 for BLAST3 */

    max_id_length = get_max_ID_length(hdp->hitlists);
    if (StringCmp("blastn", program) == 0 || 
	StringCmp("blastp", program) == 0)
    	max_def_length = DEF_AND_ID_CUTOFF - max_id_length;
    else
    	max_def_length = DEF_AND_ID_CUTOFF - max_id_length - 3;
    if (max_def_length > DEF_CUTOFF)
	max_def_length = DEF_CUTOFF;
 
    for (hlp = hdp->hitlists; hlp != NULL; hlp = hlp->next)
    {
        if (hlp->hsps != NULL)
        {
            hsp = hlp->hsps; /* first HSP */
            segp = hsp->segs;
            if (segp != NULL && segp->next != NULL &&
                (hit = segp->next->loc) != NULL)
            {
                if (hlp->seqs != NULL)
                {
                    defline = NULL;
                    sdp = hlp->seqs->desc;
                    if (sdp->defline != NULL) {
                    	defline = sdp->defline;
                    }
                }

                highScore = -1;
                TemphighScore = highScore;
                poissonScore = 99999;
                TemppoissonScore = poissonScore;
                nScore = 0;
                for (; hsp != NULL; hsp = hsp->next)
                {   /* If value associated with BLAST0_Score_sid_sum_n is 1,
			then this is not set in the ASN.1 */
		    TempnScore = 1; 
            	    segp = hsp->segs;
                    for (score = (ScorePtr) hsp->scores; score != NULL; score = score->next)
                    {
			if (score->id->str == NULL)
			    scoreDescr = "";
			else
			    scoreDescr = score->id->str;
			if (StrCmp(scoreDescr, "score") == 0)
			{
                            TemphighScore = score->value.intvalue;
			} else if (StrCmp(scoreDescr, "p_value") == 0 ||
			           StrCmp(scoreDescr, "poisson_p") == 0 ||
			           StrCmp(scoreDescr, "sum_p") == 0)
			{
                            TemppoissonScore = score->value.realvalue;
			} else if (StrCmp(scoreDescr, "poisson_n") == 0 ||
			           StrCmp(scoreDescr, "sum_n") == 0)
			{
                            TempnScore = score->value.intvalue;
			} else { /* default */
			}
                    }

                    if (TemphighScore > highScore)
                    {
                        highScore = TemphighScore;
			if (segp != NULL)
			{
			   if (StringCmp(program, "blastx") == 0)
				get_frame(segp->loc, frame_array, query_length);
			   else if (StringCmp(program, "tblastn") == 0)
				get_frame(segp->next->loc, frame_array, hlp->seqs->length);
			   else if (StringCmp(program, "tblastx") == 0)
				get_frame(segp->next->loc, frame_array, hlp->seqs->length);
			}
		    }
		    if (TemppoissonScore < poissonScore)
		    {
                        poissonScore = TemppoissonScore;
                        nScore = TempnScore;
                    }
                }

/* Add secondary id's (and their deflines) onto the first defline if it isn't 
too long already.  */
		str1 = defline;
		string_length = 0;
                if (str1 != NULL)
                {
		   	string_length = StringLen(str1);
                   	if (string_length <= max_def_length)
			{
				if (sdp && sdp->next)
				{	
				    ptr = &buffer[0];
				    StrNCpy(ptr, str1, string_length);
				    ptr += string_length;
				    total_length = string_length;
				    for (sdp1=sdp->next; sdp1; sdp1=sdp1->next)
				    {
					length = format_an_id(&id[0], sdp1->id, 0);
					old_length = total_length;
					total_length += length;
					total_length += 2;
					StringCpy(ptr, " >");
					ptr += 2;
					if (total_length > max_def_length)
					{
/* Add on part of last (too long) id to agree with Traditional Output.
There must be some slack (extra room) in buffer for this. */
					    length_left = max_def_length-old_length;
					    if (length_left > 0)
					    {
					    	StrNCpy(ptr, id, length_left);
						ptr += length_left;
						*ptr = '\0';
					    }
					    else
					    {
					    	StrNCpy(ptr, "...", 3);
						ptr += 3;
						*ptr = '\0';
					    }
					    break;
					}
					StrNCpy(ptr, id, length);
					ptr += length;
					*ptr = ' '; ptr++;
					total_length++;
					length = StringLen(sdp1->defline);
					total_length += length; 
					if (total_length > max_def_length)
					{
/* Add on part of last (too long) id to agree with Traditional Output.
There must be some slack (extra room) in buffer for this. */
					    length = total_length-length;
					    length = max_def_length-length;
					    if (length > 0)
					    	StrNCpy(ptr, sdp1->defline, length);
					    else
					    	StrNCpy(ptr, sdp1->defline, 1);
					    break;
					}
					StrNCpy(ptr, sdp1->defline, length);
					ptr += length;
				    	*ptr = '\0';
				    }
				    str1 = &buffer[0];
				}
			}
		}

                if (str1 != NULL)
		   	string_length = StringLen(str1);
		else
		   	string_length = 0;
                if (string_length > max_def_length)
                {
                      	StrNCpy (str2, str1, max_def_length);
                        str2[max_def_length-3] = '.';
                        str2[max_def_length-2] = '.';
                        str2[max_def_length-1] = '.';
                        str2[max_def_length] = '\0';
		}
		else
		{
                	if (str1 != NULL)
                      		StrNCpy (str2, str1, string_length);
			for (index=string_length; index<max_def_length; index++)
			{
				str2[index] = ' ';
			}
			str2[index] = '\0';
		}
		str1 = str2;

		

		format_an_id(&id[0], sdp->id, max_id_length);

/* Cutoff the id's after max_id_length characters to agree with Traditional 
Output */

		id[max_id_length] = '\0';
		if (StringCmp("blastn", program) == 0 || 
			StringCmp("blastp", program) == 0)
                	fprintf(fp, "%s %s %5d", id, str1, (int) highScore);
		else
                	fprintf(fp, "%s %s %s %5d", id, str1, frame_array, (int) highScore);

/* The following code was lifted from "print_header" in blastapp/lib/prt_hdr.c to match the traditional output. */
        	if (poissonScore <= 0.99) 
		{
               	    if (poissonScore != 0.)
                        fprintf(fp, "  %#-8.2lg", (double) poissonScore);
               	    else
                        fprintf(fp, "  %#-8.2lg", 0.);
        	}
        	else if (poissonScore <= 0.999)
               		fprintf(fp, "  %#-8.3lg", (double) poissonScore);
        	else if (poissonScore <= 0.9999)
                	fprintf(fp, "  %#-8.4lg", (double) poissonScore);
        	else if (poissonScore <= 0.99999)
                	fprintf(fp, "  %#-8.5lg", (double) poissonScore);
        	else if (poissonScore <  0.9999995)
                	fprintf(fp, "  %#-8.6lg", (double) poissonScore);
        	else
                	fprintf(fp, "  %#-8.7lg", (double) 1.0);
		fprintf(fp, "%3d\n", (int) nScore);
            }
        }
    }
    return TRUE;
}

/* The traditional BLAST output uses fasta style id's and not gi's.  Look 
for a textid (i.e., fasta) and use this, otherwise use the first id. 
This function operates in two modes: if id is NULL, then the length of 
the id is returned; if id is not NULL, then the formatted id is returned in 
"id", which should already contain space for this operation.  The ValNodePtr 
vnp is actually BLAST0SeqDesc.id 
The parameter max_id_length should be greater than zero if id is not NULL
and one wishes to pad the end of the id with white spaces.
*/

static Int2
format_an_id (CharPtr id, ValNodePtr vnp, Int2 max_id_length)

{
	Boolean found_id;
	Char temp[100];
	Int2 index, length;
	ValNodePtr vnp1;

	if (id != NULL)
		id[0] = '\0';

	found_id = FALSE;

	for (vnp1=vnp; vnp1 != NULL; vnp1=vnp1->next)
	{
		if (vnp1->choice == BLAST0SeqId_textid)
		{
		    if (id != NULL)
	  	    	sprintf(temp, "%s", vnp1->data.ptrvalue);
		    length = StringLen(vnp1->data.ptrvalue);
		    found_id = TRUE;
		    break;
		}
	}

	if (found_id == FALSE)
	    switch (vnp->choice) 
	    {
		default:
		    	if (id != NULL)
				sprintf(temp, "Unknown");
			length = 7;
			break;
		case BLAST0SeqId_giid:
		    	if (id != NULL)
				sprintf(temp, "gi|%ld", (long) vnp->data.intvalue);
			length = 9;
			break;
		case BLAST0SeqId_textid:
		    	if (id != NULL)
	  	    		sprintf(temp, "%s", vnp->data.ptrvalue);
		    	length = StringLen(vnp1->data.ptrvalue);
		    break;
	    }

	if (max_id_length > 0)
	{
		if (max_id_length < length)
			length = max_id_length;
	}
	else
	{
		if (BLASTCLI_ID_MAX < length)
			length = BLASTCLI_ID_MAX;
	}

	if (id)
	{
		for (index=0; index<length; index++)
			id[index] = temp[index];
/* Pad out the rest of id so white spaces will keep everything neat in the
one-line def's */
		while (index < max_id_length)
		{
			id[index] = ' ';
			index++;
		}
		id[index] = '\0';	
	}
	return length;
}

/***************************************************************************
*	Get the length of an id, truncate if necessary.
***************************************************************************/
static Int2 
get_max_ID_length(BLAST0HitListPtr hlp)

{
	BLAST0SeqDescPtr sdp;
	Int2 length, id_length=0;

	for (; hlp != NULL; hlp=hlp->next)
	{
		if (hlp->seqs != NULL)
		{
			sdp = hlp->seqs->desc;
			if (sdp)
			{
				length = format_an_id(NULL, sdp->id, 0);
				if (length > BLASTCLI_ID_MAX)
					id_length = BLASTCLI_ID_MAX;
				if (length > id_length)
					id_length = length;
			}
		}
	}
	return id_length;
}

static Int4 get_pos(BLAST0SeqIntervalPtr loc, Int4 interval)
{

    Int4 position=0;

    position = loc->from + interval + 1; /* plus, default */

    switch (loc->strand)
    {
    	case BLAST0_Seq_interval_strand_minus:
    	case BLAST0_Seq_interval_strand_minus_rf:
        	position = loc->to - interval + 1; /* minus */
		break;
	default:
		break;
    }

    return position;
}

/* 
"get_frame" gets the frame and puts it into an array, which the user
should provide.  The array should have room for three characters.
*/

static void get_frame(BLAST0SeqIntervalPtr loc, CharPtr array, Int4 total_length)
{

    Int2 frame;
    Int4 from;

    switch (loc->strand)
    {
    	case BLAST0_Seq_interval_strand_minus:
    	case BLAST0_Seq_interval_strand_minus_rf:
		from = total_length-(loc->to + 1);
    		frame = (Int2) (from%3) + 1;
		array[0] = '-';
		if (frame == 1)
			array[1] = '1';
		else if (frame == 2)
			array[1] = '2';
		else if (frame == 3)
			array[1] = '3';
		break;
	default:
    		from = loc->from;
    		frame = (Int2) (from%3) + 1;
		array[0] = '+';
		if (frame == 1)
			array[1] = '1';
		else if (frame == 2)
			array[1] = '2';
		else if (frame == 3)
			array[1] = '3';
		break;
    }
    array[2] = '\0';
}

/************************************************************************
*	Set up a SeqPort to allow the user to retrive residues/basepairs
*	one at a time.  Note that the BioseqPtr is really just there
*	to allow the opening of the SeqPortPtr, and is just a skeletal
*	Bioseq.
*********************************************************************/
static SeqPortPtr portnew(BioseqPtr bsp, ValNodePtr str, Int4 len)
{
	Uint1 code;

	bsp->length = len;
	switch (str->choice) {
		case BLAST0SeqData_ncbistdaa: 
			bsp->seq_data_type = Seq_code_ncbistdaa;
			bsp->mol = Seq_mol_aa;
			code = Seq_code_ncbieaa;
			break;
		case BLAST0SeqData_ncbi2na:
			fprintf(stderr, "Not implemented\n");
			return NULL;
		case BLAST0SeqData_ncbi4na: 
			bsp->seq_data_type = Seq_code_ncbi4na;
			bsp->mol = Seq_mol_na;
			code = Seq_code_iupacna;
			break;
		default:
			return NULL;
	}
	bsp->seq_data = str->data.ptrvalue;
	bsp->repr = Seq_repr_raw;
	return SeqPortNew(bsp, 0, -1, 0, code);
}

static void
TraditionalBlastWarning(BLAST0ResponsePtr blresp)

{
	BLAST0ResponsePtr var;
	BLAST0WarningPtr bwp;
	CharPtr warning;

	for (var=blresp; var; var=var->next)
	{
		if (var->choice == BLAST0Response_warning)
		{
			bwp = var->data.ptrvalue;
			warning = bwp->reason;
			wrap(stderr, "WARNING: ", warning, StringLen(warning), 79, 8);
		}
	}
}

#define BLSTOUT_PRINT_LEN 60

/************************************************************************
*	
*	Print out the alignment portion of the traditional BLAST output,
*	that is:
*
*>dbj|D32197|RURMTCB27 Mitochondrion Rupicapra rupicapra gene for cytochrome b.
*            >emb|D32197|MIRRCB27 Mitochondrion Rupicapra rupicapra gene for
*            cytochrome b.
*            Length = 646
*
*  Plus Strand HSPs:
*
* Score = 3230 (892.5 bits), Expect = 7.0e-264, P = 7.0e-264
* Identities = 646/646 (100%), Positives = 646/646 (100%), Strand = Plus / Plus
*
*Query:     1 AATACACTACACATCCGATACAGCAACAGCATTCTCCTCTGTAACCCACATTTGCCGAGA 60
*             ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
*Sbjct:     1 AATACACTACACATCCGATACAGCAACAGCATTCTCCTCTGTAACCCACATTTGCCGAGA 60
*
*Query:    61 TGTAAACTACGGCTGAATCATCCGATACATACATGCAAATGGAGCATCAATATTTTTCAT 120
*             ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
*Sbjct:    61 TGTAAACTACGGCTGAATCATCCGATACATACATGCAAATGGAGCATCAATATTTTTCAT 120
*
* and so on.
***************************************************************************/
static Boolean
TraditionalBottomBlastOutput(BLAST0ResultPtr hdp, Int4 query_length, CharPtr program, FILE *fp)
{
	SeqPortPtr spps, sppq, spp;
	Int2 num_open, num, sum_n;
	static Bioseq bios, bioq;
	BLAST0SegmentPtr seg;
	BLAST0SeqIntervalPtr loc, loc1, loc2, loc_next, loc_s;
	BLAST0HitListPtr hlp;
	Int4 nbr_ident, nbr_pos, pos_temp;
    BLAST0SeqDescPtr sdp;
    Char ch, chh, qchh, chh_std, qchh_std;
    Char frame_array[3];
    CharPtr defline, cp, head, s;
    CharPtr strand1, strand2;
    ScorePtr score;
    CharPtr scoreDescr;
    BLAST0HSPPtr hsp;
    int i, sc=0, ldef;	/* Left as "int" for use in Warren's wrap */
    FloatHi pv=0, ev=0;
    Int4 j, pos, range, spos, from, to; 
    Int4 first, width;
    Int4 hsp_length, qposition, sposition;
    Uint2 prev_strand;
    ValNodePtr vnp;
	
	
/* Start of main loop that runs throughout the function. */
	for (hlp = hdp->hitlists; hlp != NULL; hlp = hlp->next) 	{
		if ((hsp = hlp->hsps) == NULL) {
			continue;
		}
		if (hlp->seqs == NULL) {
			continue;
		}
		if ((sdp = hlp->seqs->desc) == NULL) {
			continue;
		}
		ldef = 0;
		for (sdp = hlp->seqs->desc; sdp != NULL; sdp = sdp->next) {
			ldef += strlen(sdp->defline) + 1;
			ldef += format_an_id(NULL, sdp->id, 0);
			ldef += 2;
		}
		s = defline = MemNew(ldef + 1);
		for (sdp = hlp->seqs->desc; sdp != NULL; sdp = sdp->next) 
		{
			if (s != defline) {
				sprintf(s++, " ");
			}
			sprintf(s++, "%c", '>');
/* Don't pad the id here, as it's secondary and goes into the def line */
			format_an_id(s, sdp->id, 0);
			s += StringLen(s);
			sprintf(s, " %s", sdp->defline);
			s += StringLen(s);
		}
		
		if (defline != NULL) {
			for (cp = defline; ((ch = *cp) != NULLB && !IS_WHITESP(ch)); cp++);
			for (; IS_WHITESP(*cp); cp++) ;
			i = cp - defline;
			i = MIN(i, 12);
			fprintf(fp, "\n\n");
			wrap(fp, "", defline, ldef, 79, i);
			defline = MemFree(defline);
		}
		fprintf(fp, "%*sLength = %s\n", i, "", 
			Ltostr(hlp->seqs->length, 1));
		prev_strand = UINT2_MAX;	/* Value that will not be used*/
		for (; hsp != NULL; hsp = hsp->next) {
			sum_n = 1;	/* Default (one) not in structure */
			for (score = (ScorePtr) hsp->scores; score != NULL; score = score->next) {
			    if (score->id->str == NULL)
			        scoreDescr = "";
			    else
			        scoreDescr = score->id->str;
			    if (StrCmp(scoreDescr, "score") == 0)
			    {
                                sc = score->value.intvalue;
			    } else if (StrCmp(scoreDescr, "p_value") == 0 ||
			               StrCmp(scoreDescr, "poisson_p") == 0 ||
			               StrCmp(scoreDescr, "sum_p") == 0)
			    {
                                pv = score->value.realvalue;
			    } else if (StrCmp(scoreDescr, "e_value") == 0 ||
			               StrCmp(scoreDescr, "poisson_e") == 0 ||
			               StrCmp(scoreDescr, "sum_e") == 0)
			    {
                                ev = score->value.realvalue;
			    } else if (StrCmp(scoreDescr, "poisson_n") == 0 ||
			               StrCmp(scoreDescr, "sum_n") == 0)
			    {
                                sum_n = score->value.intvalue;
			    } else { /* default */
			    }
			}
			hsp_length = hsp->len;
/* loc1 is the query; loc2 is the subject. */
			loc1 = hsp->segs->loc;
			loc2 = hsp->segs->next->loc;
			if (StringCmp(program, "blastn") == 0 ||
			    StringCmp(program, "blastp") == 0)
			{
				loc = loc1;
				loc_s = loc2;
			}
			else if (StringCmp(program, "blastx") == 0) 
			{
				loc = loc2;
				loc_s = loc1;
			}
			else if (StringCmp(program, "tblastn") == 0) 
			{
				loc = loc1;
				loc_s = loc2;
			}
			else if (StringCmp(program, "tblastx") == 0) 
			{
				loc = loc1;
				loc_s = loc2;
			}

			if (loc_s->strand != 0 && loc_s->strand != prev_strand) 
			{
				prev_strand = loc_s->strand;
				fprintf(fp, "\n  %s Strand HSPs:\n", 
					gish_str_strand(loc_s->strand));
			}
			fprintf(fp, "\n Score = %ld (%.1lf bits), Expect = %#0.2lg, ",
				(long)sc, (double) sc * la2, (double) ev);

			if (sum_n < 2) {
				fprintf(fp, "P = %#0.2lg\n", (double) pv);
			} else {
				fprintf(fp, "Sum P(%u) = %#0.2lg\n", sum_n, (double) pv);
			}

			nbr_ident = nbr_pos = 0;
			if ((sppq = portnew(&bioq, hsp->segs->str, hsp_length)) == NULL) {
				return FALSE;
			}
			if ((spps = portnew(&bios, hsp->segs->next->str, hsp_length)) == NULL) {
				return FALSE;
			}
			
			range = loc->to - loc->from;
			if (StringCmp(program, "tblastx") == 0) 
				range /= 3;
			for (pos=0; pos <= range; pos++) 
			{
				chh = SeqPortGetResidue(spps);
				qchh = SeqPortGetResidue(sppq);
				if (chh == qchh) 
				{
					nbr_ident++;
					if (StringCmp(program, "blastn") == 0) 
						nbr_pos++;
				}
				if (StringCmp(program, "blastn") != 0) 
				{
				     chh_std = ralpha[ chh];
				    qchh_std = ralpha[qchh];
				    if (matr[chh_std][qchh_std] > 0) 
					    nbr_pos++;
				}
			}

			fprintf(fp,
			" Identities = %ld/%ld (%ld%%), Positives = %ld/%ld (%ld%%)",
			(long) nbr_ident, (long) pos, (long)((100*nbr_ident)/pos),
			(long) nbr_pos, (long) pos, (long)((100*nbr_pos)/pos) );
			if (StringCmp( program, "blastn") == 0)
			{
			    strand1 = gish_str_strand(loc1->strand);
			    strand2 = gish_str_strand(loc2->strand);
			    if (StringCmp(strand1, "Undefined") != 0 &&
			        StringCmp(strand2, "Undefined") != 0)
				    fprintf(fp, ", Strand = %s / %s", strand1, strand2);
			}
			else if (StringCmp( program, "blastx") == 0)
			{
				loc = hsp->segs->loc;
				get_frame(loc, frame_array, query_length);
				fprintf(fp, ", Frame = %s", frame_array);
			}
			else if (StringCmp( program, "tblastn") == 0)
			{
				loc = hsp->segs->next->loc;
				get_frame(loc, frame_array, hlp->seqs->length);
				fprintf(fp, ", Frame = %s", frame_array);
			}
			else if (StringCmp( program, "tblastx") == 0)
			{
				loc = hsp->segs->loc;
				get_frame(loc, frame_array, query_length);
				fprintf(fp, ", Frame = %s", frame_array);
				loc = hsp->segs->next->loc;
				get_frame(loc, frame_array, hlp->seqs->length);
				fprintf(fp, " / %s", frame_array);
			}
			putc('\n', fp);
			num_open = 1;
/* Print out the actual alignment. */ 
			for (pos=0; pos < hsp_length; pos += BLSTOUT_PRINT_LEN) {
				head = "Query";
				first = TRUE;
				num = 0;
				for (seg = hsp->segs; seg != NULL; seg = seg->next, num++) {
					loc = seg->loc;
					from = loc->from;
					to = loc->to;
					if (first)
					{
					     qposition = get_pos(loc, pos);
					     width = MAX(5, Lwidth(qposition, 1));
					     loc_next = seg->next->loc;
					     sposition = get_pos(loc_next, pos);
					     width = MAX(width, Lwidth(sposition, 1));
					}
					if (!first) {
						SeqPortSeek(sppq, pos, 0);
						if (num_open != num) {
							spps = portnew(&bios, seg->str, hsp_length);
							if (spps == NULL) {
								return FALSE;
							}
							num_open = num;
						}
						SeqPortSeek(spps, pos, 0);
						for (j=0; j<(width+8); j++)
							putc(' ', fp);
/*
						fprintf(fp, "%s", " ");
*/
						for (j=0; j+pos < hsp_length && j < BLSTOUT_PRINT_LEN; j++) {
							 chh = SeqPortGetResidue(spps);
							qchh = SeqPortGetResidue(sppq);
							if (prots) {
								if (chh == qchh) {
									putc(chh, fp);
								} else {
									chh_std = ralpha[ chh];
									qchh_std = ralpha[qchh];
									putc((matr[chh_std][qchh_std]>0) ? '+' : ' ', fp);
								}
							} else {
								putc((chh == qchh) ? '|' : ' ', fp);
							}
						}
						spp = spps;
					} else {
						spp = sppq;
					}
					fprintf(fp, "\n");
					SeqPortSeek(spp, pos, 0);
					if (pos+BLSTOUT_PRINT_LEN < hsp_length)
						spos = BLSTOUT_PRINT_LEN - 1;
					else
						spos = hsp_length - pos - 1;
					pos_temp = pos;
				        if (StringCmp( program, "tblastx") == 0)
					{
						pos_temp *= 3;
					}
					else if (StringCmp(head, "Query") == 0)
					{
					     if (StringCmp(program, "blastx") == 0)
					     {
							pos_temp *= 3;
					     }
					}
					else if (StringCmp(head, "Sbjct") == 0)
					{
					     if (StringCmp(program, "tblastn") == 0)
					     {
							pos_temp *= 3;
					     }
					}
					qposition = get_pos(loc, pos_temp);
					fprintf(fp, "%s: %*ld ", head, (int) width, (long) qposition);
					for (j=0; j <= spos; j++) {
						chh = SeqPortGetResidue(spp);
						fputc(chh ,fp);
					}
					pos_temp = j-1+pos;
				        if (StringCmp( program, "tblastx") == 0)
					{
						pos_temp *= 3;
						pos_temp += 2;
					}
					else if (StringCmp(head, "Query") == 0)
					{
					     if (StringCmp(program, "blastx") == 0 ||
				                  StringCmp( program, "tblastx") == 0)
						{
							pos_temp *= 3;
							pos_temp += 2;
						}
					}
					else if (StringCmp(head, "Sbjct") == 0)
					{
					     if (StringCmp(program, "tblastn") == 0)
					     {
							pos_temp *= 3;
							pos_temp += 2;
					     }
					}
					qposition = get_pos(loc, pos_temp);
					fprintf(fp, " %ld\n", (long) qposition);
					head = "Sbjct";
					first = FALSE;
				}
			}
			SeqPortFree(sppq);
			SeqPortFree(spps);
		}
	}
	fprintf(fp, "\n");
    return TRUE;
}

/********************************************************************
*	
*	Formats the parameters (e.g., command line options) sent 
*	back by the server and the statistics found at the bottom
*	of the output.
*********************************************************************/
static Boolean
TraditionalTailBlastOutput(BLAST0ResponsePtr blresp, FILE *fp)
{
	ValNodePtr res;
	ValNodePtr parms, stats;
	CharPtr s;
	Boolean new;

	if ((res = find(blresp, BLAST0Response_parms)) != NULL) {
		if ((parms = res) != NULL) {
			fprintf(fp, "\nParameters:\n");
			new = TRUE;
			for (; parms != NULL; parms = parms->next) {
				s = parms->data.ptrvalue;
				if (*s == '\0') {
					fprintf(fp, "\n", s);
					new = TRUE;
				} else {
					if (new) {
						fprintf(fp, "  ");
					}
					fprintf(fp, "%s", s);
					new = FALSE;
				}
			}
		} else {
			fprintf(fp, "No parameters received from server\n");
		}
	}
	if ((res = find(blresp, BLAST0Response_stats)) != NULL) {
		if ((stats = res) != NULL) {
			fprintf(fp, "\nStatistics:\n");
			new = TRUE;
			for (; stats != NULL; stats = stats->next) {
				s = stats->data.ptrvalue;
				if (*s == '\0') {
					fprintf(fp, "\n", s);
					new = TRUE;
				} else {
					if (new) {
						fprintf(fp, "  ");
					}
					fprintf(fp, "%s", s);
					new = FALSE;
				}
			}
		} else {
			fprintf(fp, "No statistics received from server\n");
		}
	}
    return TRUE;
}

Boolean LIBCALL TraditionalBlastOutput(BLAST0ResultPtr hdp, BLAST0ResponsePtr blresp, CharPtr program, FILE *fp)
{
	Int4 query_length=0;

	if (!TraditionalHeadBlastOutput(blresp, &query_length, program, fp)) {
		return FALSE;
	}
   
   if (!TraditionalHistBlastOutput(hdp, fp)) {
      return FALSE;
   }
   
  
	if (TraditionalTopBlastOutput(hdp, query_length, program, fp)) 
	{
		TraditionalBlastWarning(blresp);
		if (!TraditionalBottomBlastOutput(hdp, query_length, program, fp)) {
			return FALSE;
		}
    }
    if (!TraditionalTailBlastOutput(blresp, fp)) {
    	return FALSE;
    }
    return TRUE;
}
