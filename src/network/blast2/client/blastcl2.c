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
* File Name: blastcli.c
*
* Author:  Roman L. Tatusov, Jonathan Epstein, Tom Madden
*
* Version Creation Date:   06/16/95
*
* $Revision: 4.0 $
*
* File Description: 
*       Simulates "traditional" BLAST output
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
* $Log: blastcl2.c,v $
 * Revision 4.0  1995/07/26  13:55:34  ostell
 * force revision to 4.0
 *
 * Revision 1.6  1995/07/25  15:02:28  madden
 * Error messages returned from server printed out.
 *
 * Revision 1.5  1995/07/24  17:34:02  madden
 * Changed HitData to BLAST0Result
 *
 * Revision 1.4  1995/07/12  17:45:25  madden
 * Call BlastBioseq with new argument to perform masking.
 *
 * Revision 1.3  1995/06/23  22:14:00  madden
 * sixth argument in BlastBioseq call is now zero, rather than NULL.
 *
 * Revision 1.2  1995/06/22  17:08:16  madden
 * Added "output" argument to call to BlastBioseq.
 *
 * Revision 1.1  1995/06/16  11:26:33  epstein
 * Initial revision
 *
 * Revision 1.16  95/05/17  17:59:18  epstein
 * add RCS log revision history
 * 
*/

#include <sequtil.h>
#include <prtutil.h>
#include <tofasta.h>
#include <netblap2.h>

#define MAX_SEQ_LEN 20000

/* find the last nucleotide bioseq in the bioseqset */
static void FindNuc(SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent)
{
    BioseqPtr PNTR bp;
    BioseqPtr local_bsp;

    bp = (BioseqPtr PNTR) data;
    if (IS_Bioseq(sep))
    {
        local_bsp = (BioseqPtr) sep->data.ptrvalue;
        if (ISA_na(local_bsp->mol))
          *bp = local_bsp;
    }
}

/* find the last protein bioseq in the bioseqset */
static void FindProt(SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent)
{
    BioseqPtr PNTR bp;
    BioseqPtr local_bsp;

    bp = (BioseqPtr PNTR) data;
    if (IS_Bioseq(sep))
    {
        local_bsp = (BioseqPtr) sep->data.ptrvalue;
        if (ISA_aa(local_bsp->mol))
          *bp = local_bsp;
    }
}


static Boolean LIBCALLBACK
callback (BLAST0ResponsePtr brp)
{
	return FALSE;
}


static Int2 blast(BioseqPtr bsp, CharPtr blast_program, CharPtr blast_database,
CharPtr cmd_options, CharPtr outfile, StdPrintOptionsPtr Spop, Boolean filter, Boolean standalone)
{
	BLAST0PrefacePtr preface;
	BLAST0RequestPtr blreqp;
	BLAST0ResponsePtr brp, brp1, blastResponse;
	CharPtr ptr;
	BLAST0ResultPtr result;
	BLAST0StatusPtr status;
	SeqAnnotPtr sap;
	FILE *outputfile;
	ValNodePtr vnp;

	if (bsp == NULL) {
		ErrPostEx(SEV_FATAL, 0, 0, "Couldn't read sequences");
		return (5);
	}
#ifdef MAX_SEQ_LEN
	if (bsp != NULL && bsp->length > MAX_SEQ_LEN) {
		ErrPostEx(SEV_FATAL, 0, 0, "Cannot process sequences > %d base pairs (this sequence is %d bp long)", MAX_SEQ_LEN, bsp->length);
		return (4);
	}
#endif
	if (! BlastInit(standalone ? "blasttest" : "blastcl2", FALSE)) {
		ErrPostEx(SEV_FATAL, 0, 0, "Unable to initialize BLAST service");
		return (1);
	}
	brp = NULL;
	result = BlastBioseq(bsp, blast_program, blast_database, cmd_options, &brp, NULL, 0, callback);
	if (result != NULL) 
	{
#ifdef DEBUG
		AsnIoPtr aip;

		aip = AsnIoOpen("blastout.asn", "w");
		BLAST0ResultAsnWrite(result, aip, NULL);
		AsnIoClose(aip);
#endif /* DEBUG */
		outputfile = FileOpen (outfile, "w");
		if (outputfile != NULL) 
		{
			if (Spop != NULL)
			{
			    ptr = FormatResultWithTemplate(result, Spop);
			    if (ptr != NULL)
			    {
			    	fprintf(outputfile, "%s", ptr);
			    	ptr = MemFree(ptr);
			    }
			}
			else
			{
			    TraditionalBlastOutput(result, brp, blast_program, outputfile);
			    BLAST0ResultFree(result);
			    FileClose (outputfile);
			}
			while (brp)
			{
				brp1 = brp;
				brp = brp->next;
				brp1->next = NULL;
				BLAST0ResponseFree(brp1);
			}
		} 
		else 
		{
			BlastFini();
			ErrPostEx(SEV_FATAL, 0, 0, "FileOpen failed");
			return (3);
		}
	}
	else
	{
		if (brp)
		{
			if (brp->choice == BLAST0Response_status)
			{
				blreqp = ValNodeNew(NULL);
				blreqp->choice = BLAST0Response_usage_info;
				blreqp->data.ptrvalue = StringSave(blast_program);
				blastResponse = SubmitInfoRequest(blreqp);
				preface = blastResponse->data.ptrvalue;
				vnp = preface->prog_usage;
				print_usage(stderr, vnp);
				status = brp->data.ptrvalue;
				fprintf(stderr, "\nFATAL:  %s\n", status->reason);
				fprintf(stderr, "\nEXIT CODE %ld\n", (long) status->code);
			}
			else
			{
				brp1 = brp->next;
				while (brp1)
				{
				     if (brp1->choice == BLAST0Response_status)
				     {
					status = brp1->data.ptrvalue;
					fprintf(stderr, "\nFATAL:  %s\n", status->reason);
					fprintf(stderr, "\nEXIT CODE %ld\n", (long) status->code);
				     }
				     brp1 = brp1->next;
				}
			}
		}
		
		else
		{
			ErrPostEx(SEV_FATAL, 0, 0, "BLAST search failed");
			return (3);
		}
	}

	BlastFini();
	return 0;
}

#define NUMARG 7

static Args myargs [NUMARG] = {
  { "Program Name (blastn,blastp,tblasn,blastx)", "", NULL, NULL, FALSE, 'p', ARG_STRING, 0.0, 0, NULL},
  { "Database", "nr", NULL, NULL, FALSE, 'd', ARG_STRING, 0.0, 0, NULL},
  { "Query File", "stdin", NULL, NULL, FALSE, 'i', ARG_FILE_IN, 0.0, 0, NULL},
  { "Output File", "stdout", NULL, NULL, FALSE, 'o', ARG_FILE_OUT, 0.0, 0, NULL},
  { "Print Template", NULL, NULL, NULL, TRUE, 't', ARG_STRING, 0.0, 0, NULL} ,
  { "BLAST options (enclosed in double quotes, separated by spaces)", NULL, NULL, NULL, TRUE, 'b', ARG_STRING, 0.0, 0, NULL} ,
  { "Standalone mode", "F", NULL, NULL, TRUE, 's', ARG_BOOLEAN, 0.0,0,NULL}
};

/*********************************************************************
*	"main" function to call blast for the client.  The first "main"
*	uses GetArgs and will run on Mac's and PC's; will use a 
*	Print Template if that is given as an argument; and (eventually)
*	will accept multiple fasta sequences from a single file.
*********************************************************************/
#if !defined(OLD_BLAST)
Int2 Main (void)

{
	Boolean filter=FALSE;
        CharPtr blast_program;
        CharPtr blast_database;
        CharPtr blast_inputfile;
        CharPtr blast_outputfile;
        CharPtr blast_params;
        CharPtr print_template = NULL;
        Boolean isprot = FALSE;
	BioseqPtr bsp;
	SeqEntryPtr sep;
	StdPrintOptionsPtr Spop = NULL;
	FILE *fp;

	if (! GetArgs ("blastcli", NUMARG, myargs)) 
	{
		exit(1);
	}

	blast_program = myargs [0].strvalue;

	if (strcmp(blast_program, "blastp") == 0 ) {
		isprot = TRUE;
	} else if (strcmp(blast_program, "blastx") == 0) {
		isprot = FALSE;
	} else if (strcmp(blast_program, "blastn") == 0) {
		isprot = FALSE;
	} else if (strcmp(blast_program, "tblastn") == 0) {
		isprot = TRUE;
	} else if (strcmp(blast_program, "tblastx") == 0) {
		isprot = FALSE;
	} else {
		ErrPostEx(SEV_FATAL, 0, 0, "blast: Bad name for BLAST program: \"%s\"\n", blast_program);
		exit(1);
	}
	blast_database = myargs [1].strvalue;
	blast_inputfile = myargs [2].strvalue;
	blast_outputfile = myargs [3].strvalue;
	print_template = myargs [4].strvalue;
	if (myargs [5].intvalue)
		filter = TRUE;
	blast_params = NULL;

	if ((fp = FileOpen(blast_inputfile, "r")) == NULL)
	{
		ErrPostEx(SEV_FATAL, 0, 0, "blast: Unable to open input file %s\n", blast_inputfile);
		exit(1);
	}

/* Initialize Print Template stuff */
	if (print_template != NULL)
	{
                if (! PrintTemplateSetLoad (print_template))
                {
			ErrPostEx(SEV_FATAL, 0, 0, "blast: Unable to open print_template file %s\n", print_template);
			exit(1);
                }
                Spop = StdPrintOptionsNew(NULL);
                if (Spop)
                {
                        Spop->linelength = 0;
                        Spop->indent = "";
                        Spop->newline = "\n";
                        Spop->tab = "\t";
                }
                else
                {
                        ErrPostEx(SEV_FATAL, 0, 0, "blast: StdPrintOptionsNew failed");
			exit(1);
                }
	}
/* Next task, change this to accept multiple fasta entries. */

	sep = FastaToSeqEntry(fp, !isprot);
	FileClose(fp);
	bsp = NULL;
	SeqEntryExplore(sep, &bsp, isprot? FindProt : FindNuc);
	if (bsp == NULL)
	{
	   ErrPostEx(SEV_FATAL, 0, 0, "Unable to obtain bioseq\n");
	   exit (2);
	}
	blast_params = StringSave(myargs[5].strvalue);
	return (blast(bsp, blast_program, blast_database, blast_params, blast_outputfile, Spop, filter, myargs[6].intvalue));
}
#else

void main(int argc, char *argv[])
{
        CharPtr blast_program;
        CharPtr blast_database;
        CharPtr blast_inputfile;
        CharPtr blast_params;
        int isprot = FALSE;
	BioseqPtr bsp;
	SeqEntryPtr sep;
	FILE *fp;
	int parmLen = 0;
	int arg;

	if (argc < 4) {
		fprintf (stderr, "Usage: %s {blastn,blastp,tblasn,blastx} database queryfile [options]\n", argv[0]);
		exit(1);
	}
	blast_program = argv[1];

	if (strcmp(blast_program, "blastp") == 0 ) {
		isprot = TRUE;
	} else if (strcmp(blast_program, "blastx") == 0) {
		isprot = FALSE;
	} else if (strcmp(blast_program, "blastn") == 0) {
		isprot = FALSE;
	} else if (strcmp(blast_program, "tblastn") == 0) {
		isprot = TRUE;
	} else if (strcmp(blast_program, "tblastx") == 0) {
		isprot = FALSE;
	} else {
		fprintf(stderr, "%s: Bad name for BLAST program: \"%s\"\n", argv[0], blast_program);
		exit(1);
	}
	blast_database = argv[2];
	blast_inputfile = argv[3];
	if ((fp = FileOpen(blast_inputfile, "r")) == NULL)
	{
		fprintf(stderr, "%s: Unable to open input file %s\n", argv[0],
			blast_inputfile);
		exit(1);
	}
	sep = FastaToSeqEntry(fp, !isprot);
	FileClose(fp);
	bsp = NULL;
	SeqEntryExplore(sep, &bsp, isprot? FindProt : FindNuc);
	if (bsp == NULL)
	{
	   fprintf (stderr, "Unable to obtain bioseq\n");
	   exit (2);
	}
	for (arg = 4; arg < argc; arg++)
	{
	    parmLen += StrLen(argv[arg]) + 1;
	}
	blast_params = MemNew(parmLen);
	parmLen = 0;
	for (arg = 4; arg < argc; arg++)
	{
	    StrCpy(&blast_params[parmLen], argv[arg]);
	    parmLen += StrLen(argv[arg]);
	    if (arg < argc - 1)
	    {
		blast_params[parmLen++] = ' ';
	    }
	}
	exit (blast(bsp, blast_program, blast_database, blast_params, "stdout", NULL, FALSE, FALSE));
}
#endif

