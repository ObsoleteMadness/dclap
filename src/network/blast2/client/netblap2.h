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
* File Name: netblap2.h
*
* Author:  Jonathan Epstein, Tom Madden
*
* Version Creation Date:   06/16/95
*
* $Revision: 4.1 $
*
* File Description: 
*       Application Programming Interface (API) for BLAST network server
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
* $Log: netblap2.h,v $
 * Revision 4.1  1995/07/26  22:29:03  kans
 * added prototype for print_usage
 *
 * Revision 4.0  1995/07/26  13:55:34  ostell
 * force revision to 4.0
 *
 * Revision 1.9  1995/07/24  16:34:49  madden
 * removed define for NETBLAP1_BUFFER_SIZE.
 *
 * Revision 1.8  1995/07/18  20:05:36  madden
 * Added "start" (for start of SeqLoc) to PrepRequestInfoPtr.
 *
 * Revision 1.7  1995/07/12  17:44:32  madden
 * Changed prototypes to allow SeqLocPtr for masking of sequence.
 *
 * Revision 1.6  1995/06/28  18:28:07  madden
 * Changed BlastBioseq2 and BlastSeqLoc2 to take an additional argument:
 * a "BLAST0ResponsePtr PNTR".
 *
 * Revision 1.5  1995/06/23  21:39:51  madden
 * Add defines BLAST_SERVER_OMIT_MATRIX, BLAST_SERVER_OMIT_QUERY etc.
 *
 * Revision 1.4  1995/06/22  16:08:08  madden
 * Added prototypes for SubmitResultBlastRequest and SubmitSeqAlignBlastRequest.
 *
 * Revision 1.3  1995/06/22  13:19:01  madden
 * Added five Booleans to PrepRequestInfoPtr that control the amount of
 * output, these correspond to the five Uint1's on BLAST0SearchPtr that
 * determine whether a matrix, query seq, db seq, etc is returned.
 *
 * Revision 1.2  1995/06/22  12:54:45  madden
 * Changed HitDataPtr to BLAST0ResultPtr.
 *
 * Revision 1.1  1995/06/16  11:27:08  epstein
 * Initial revision
 *
 * Revision 1.15  95/05/17  17:59:30  epstein
 * add RCS log revision history
 * 
*/

#include <objblst2.h>
#include <objseq.h>
#include <seqport.h>
#include <prtutil.h>

#define BLAST_SERVER_OMIT_MATRIX 1
#define BLAST_SERVER_OMIT_QUERY 2
#define BLAST_SERVER_OMIT_QUERY_SEQ_IN_SEG 4
#define BLAST_SERVER_OMIT_DB_SEQ_IN_SEG 8

/*------------the structure for the function PrepareRequest (netblap2.c)---*/

typedef struct preprequestinfo {
        Boolean is_na;	/* Is this a nucleic acid? */
	CharPtr defline, /* The fasta definition line. */
		options, /* BLAST command line options */
		dbname, /* name of the database */
		textid, /* textid of the entry */
		progname; /* BLAST program name (e.g., blastn, blastp...) */
	Int4 	gi,	/* gi number */
		length, /* number of residues in entry */
		start;	/* BLAST run starts at this residue */ 
	SeqPortPtr spp; /* SeqPort to get sequence data from. */
/* These Booleans determine the amount of output */
	Boolean return_matrix,		/* Should matrix be returned? */
		return_query, 		/* Should query be sent back? */
		return_BLAST0result,	/* Should Blast0ResultPtr be returned;
					if not, then a Seq-align is returned */
		return_query_seq_in_seg, /* Should the query sequence for a hit
					be returned? */ 
		return_db_seq_in_seg;	/* Should the db sequence for a hit be
					returned? */
	SeqLocPtr mask;	/* which positions should be masked */
} PrepRequestInfo, PNTR PrepRequestInfoPtr;


/* the following are for backwards-compatability */
#define HitDataPtr BLAST0ResultPtr
#define HitDataFree BLAST0ResultFree

typedef Boolean (LIBCALLBACK *ProgressCallback) PROTO((BLAST0ResponsePtr));

Boolean LIBCALL BlastInit PROTO((CharPtr clientId, Boolean ignoreErrs));
Boolean LIBCALL BlastFini PROTO((void));
BLAST0ResultPtr LIBCALL BlastBioseq PROTO((BioseqPtr bsp, CharPtr progname, CharPtr dbname, CharPtr cmdLineOptions, BLAST0ResponsePtr PNTR blrespPtr, SeqLocPtr mask_seqloc, Uint4 output, ProgressCallback progCallback));
BLAST0ResultPtr LIBCALL SimpleBlastBioseq PROTO((BioseqPtr bsp, CharPtr progname, CharPtr dbname, CharPtr cmdLineOptions, Boolean useMonitors));
SeqAnnotPtr LIBCALL BlastBioseq2 PROTO ((BioseqPtr bsp, CharPtr progname, CharPtr dbname, CharPtr blast_params, BLAST0ResponsePtr PNTR blrespPtr, SeqLocPtr mask_seqloc, Boolean useMonitors));
SeqAnnotPtr LIBCALL BlastSeqLoc2 PROTO ((SeqLocPtr slp, CharPtr progname, CharPtr dbname, CharPtr blast_params, BLAST0ResponsePtr PNTR blrespPtr, SeqLocPtr mask_seqloc, Boolean useMonitors));


SeqAnnotPtr LIBCALL HitDataToSeqAnnot PROTO((BLAST0ResultPtr, SeqIdPtr));
SeqAnnotPtr LIBCALL HitDataToSeqAnnotAlignment PROTO((BLAST0ResultPtr, SeqIdPtr));

Boolean LIBCALL TraditionalBlastOutput PROTO((BLAST0ResultPtr hdp, BLAST0ResponsePtr blresp, CharPtr program, FILE *fp));
CharPtr FormatResultWithTemplate PROTO ((BLAST0ResultPtr blresp, StdPrintOptionsPtr Spop));

BLAST0ResponsePtr SubmitInfoRequest PROTO ((BLAST0RequestPtr blreqp));

BLAST0ResultPtr SubmitResultBlastRequest PROTO ((BLAST0RequestPtr blreqp, BLAST0ResponsePtr PNTR blrespPtr, ProgressCallback progCallback));

SeqAlignPtr SubmitSeqAlignBlastRequest PROTO ((BLAST0RequestPtr blreqp, BLAST0ResponsePtr PNTR blrespPtr, ProgressCallback progCallback));

int print_usage(FILE *fp, ValNodePtr vnp);

