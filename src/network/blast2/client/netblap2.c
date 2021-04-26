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
* File Name: netblap1.c
*
* Author:  Epstein, Madden
*
* Version Creation Date:   06/16/95
*
* $Revision: 4.0 $
*
* File Description: 
*       API for BLAST service
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 2/24/94  Tatusov     Moved "traditional" BLAST output to blastout.c
* 8/12/94  Epstein     Fixed order of blresp messages in BlastBioseq
* 2/17/95  Madden      Addition of BlastBioseq2
*
* ==========================================================================
*
*
* RCS Modification History:
* $Log: netblap2.c,v $
 * Revision 4.0  1995/07/26  13:55:34  ostell
 * force revision to 4.0
 *
 * Revision 1.19  1995/07/25  15:02:28  madden
 * Added print_usage to print out options.
 *
 * Revision 1.17  1995/07/24  17:33:08  madden
 * removed debugging code that AsnWrites the response to a temp file.
 *
 * Revision 1.16  1995/07/24  16:34:25  madden
 * replaced NETBLAP1_BUFFER_SIZE by NETBLAP2_BUFLEN.
 *
 * Revision 1.15  1995/07/24  16:26:35  madden
 * Added MaskTheResidues to mask a number of residues at once.  Removed
 * MaskThisResidue.
 *
 * Revision 1.14  1995/07/20  18:52:00  madden
 * Replaced SeqIdPrint by SeqIdWrite.
 *
 * Revision 1.13  1995/07/19  14:03:56  madden
 * Changed AdjustOffSetInSeqAnnot to adjust StdSegPtr.
 *
 * Revision 1.12  1995/07/18  21:06:03  madden
 * Added call to AdjustOffSetInSeqAnnot to BlastSeqLoc2.
 *
 * Revision 1.11  1995/07/18  20:05:36  madden
 * Changed masking of BlastSeqLoc2 for cases where blast does not start at
 * the beginning of the SeqLoc.
 *
 * Revision 1.10  1995/07/12  17:42:29  madden
 * Added parameter to BlastBioseq, BlastSeqLoc, BlastBioseq2, and BlastSeqLoc2
 * to allow parts of sequence to be masked.  Also added the function
 * MaskThisResidue to mask sequences with "N" or "X".
 *
 * Revision 1.9  1995/06/28  20:08:38  madden
 * Added Boolean get_seqalign argument to SubmitBlastRequest.
 *
 * Revision 1.8  1995/06/28  18:28:07  madden
 * Changed BlastBioseq2 and BlastSeqLoc2 to take an additional argument:
 * a "BLAST0ResponsePtr PNTR".
 *
 * Revision 1.7  1995/06/28  15:38:17  madden
 * BlastBioseq2 and BlastSeqLoc2 return NULL if no results.
 *
 * Revision 1.6  1995/06/23  22:14:53  madden
 * Fixed problems identified by lint.
 *
 * Revision 1.5  1995/06/23  21:39:51  madden
 * Sixth argument in BlastBioseq now takes a Uint4 rather than a BoolPtr.
 *
 * Revision 1.4  1995/06/22  17:06:04  madden
 * Added functions SubmitSeqAlignBlastRequest and SubmitResultBlastRequest,
 * changed BlastBioseq2 and BlastSeqLoc2 to take SeqAlignPtr directly from
 * the BLAST output.
 *
 * Revision 1.3  1995/06/22  13:19:01  madden
 * Added five Booleans to PrepRequestInfoPtr that control the amount of
 * output, these correspond to the five Uint1's on BLAST0SearchPtr that
 * determine whether a matrix, query seq, db seq, etc is returned.
 *
 * Revision 1.2  1995/06/22  12:54:45  madden
 * Changed HitDataPtr to BLAST0ResultPtr.
 *
 * Revision 1.1  1995/06/16  11:26:59  epstein
 * Initial revision
 *
 * Revision 1.28  95/05/17  17:59:27  epstein
 * add RCS log revision history
 * 
*/

#include <asn.h>
#include <ncbi.h>
#include <objseq.h>
#include <objsset.h>
#include <prtutil.h>
#include <seqport.h>
#include <sequtil.h>
#include <netblap2.h>
#include <ncbinet.h>


#define NETBLAP2_BUFLEN 200
#define STANDALONE

extern void MsgSetReadTimeout PROTO((NI_HandPtr mh, int t));

static Boolean ReestablishNetBlast PROTO((void));
static Boolean NetInit PROTO((void));
static Boolean ForceNetInit PROTO((void));
static Boolean NetFini PROTO((void));
static Boolean GenericReestablishNet PROTO((CharPtr svcName, Boolean showErrs));

static ByteStorePtr StoreResiduesInBS PROTO ((Uint1Ptr buffer, Int4 buf_len, ByteStorePtr bsp, Uint1Ptr bitctr_to, Uint1 newcode, Uint1 oldcode, Int4 len));

static BLAST0RequestPtr PrepareRequest PROTO ((PrepRequestInfoPtr prip));

static VoidPtr SubmitBlastRequest PROTO((BLAST0RequestPtr blreqp, BLAST0ResponsePtr PNTR blrespPtr, ProgressCallback progCallback, Boolean get_seqalign));

static BLAST0ResultPtr LIBCALL BlastSeqLoc PROTO ((SeqLocPtr slp, CharPtr progname, CharPtr dbname, CharPtr cmdLineOptions, BLAST0ResponsePtr PNTR blrespPtr, SeqLocPtr mask_seqloc, ProgressCallback progCallback));

static void MaskTheResidues PROTO((Uint1Ptr buffer, Int4 max_length, Uint1 mask_residue, Int4 offset, SeqLocPtr mask_slp));


static void AdjustOffSetInSeqAnnot PROTO ((SeqAnnotPtr sap, SeqLocPtr slp));

static NI_HandPtr svcp = NULL;
static AsnIoPtr   asnin = NULL;
static AsnIoPtr   asnout = NULL;
static Boolean num_attached = 0;
static Boolean reallyFinal = TRUE;
static NI_DispatcherPtr dispatcher;


/*****************************************************************************
*
*   BlastInit ()
*
*****************************************************************************/

Boolean LIBCALL
BlastInit (CharPtr clientId, Boolean ignoreErrs)

{
    if (clientId != NULL && StrCmp(clientId, "blasttest") == 0)
    {
        asnin  = AsnIoOpen("blserver.inp", "r");
        asnout = AsnIoOpen("blclient.out", "w");
        
        if (asnin == NULL || asnout == NULL)
          return FALSE;
    } else {
        if (! NetInit())
            return FALSE;
    
        svcp = NI_GenericGetService(dispatcher, NULL, "BLAST", "blast2", FALSE);
        if (svcp == NULL)
        {
            ErrPost(CTX_UNKNOWN, 1, "NI_ServiceGet [%s] (%s)", ni_errlist[ni_errno], ni_errtext);
            BlastFini();
            return FALSE;
        }
    
	    MsgSetReadTimeout(svcp, 600);
        asnin = svcp->raip;
        asnout = svcp->waip;
    }
    
    return TRUE;
}

/*****************************************************************************
*
*   BlastFini ()
*
*****************************************************************************/

Boolean LIBCALL
BlastFini()

{
    NetFini();
    return TRUE;
}


static Boolean LIBCALLBACK
callbackWithMon (BLAST0ResponsePtr brp)
{
    static MonitorPtr mon = NULL;
    BLAST0JobProgressPtr jprogp;
    BLAST0JobDescPtr jdescp;

    switch (brp->choice)
    {
    case BLAST0Response_job_start:
        if ((jdescp = (BLAST0JobDescPtr) brp->data.ptrvalue) == NULL)
        {
            return TRUE;
        }
        if (mon != NULL)
        {
            MonitorFree(mon);
            mon = NULL;
        }
        mon = MonitorIntNew(jdescp->desc == NULL ? "BLAST" : jdescp->desc,
                            0, jdescp->size);
        return TRUE;
    case BLAST0Response_job_progress:
        if (mon != NULL)
        {
            if ((jprogp = (BLAST0JobProgressPtr) brp->data.ptrvalue) != NULL)
            {
                MonitorIntValue(mon, jprogp->done);
            }
        }
        return TRUE;
    case BLAST0Response_job_done:
        if (mon != NULL)
        {
            MonitorFree(mon);
            mon = NULL;
        }
        return TRUE;
    default:
        return FALSE;
    }
}


BLAST0ResultPtr LIBCALL
SimpleBlastBioseq(BioseqPtr bsp, CharPtr progname, CharPtr dbname, CharPtr cmdLineOptions, Boolean useMonitors)
{
    return BlastBioseq(bsp, progname, dbname, cmdLineOptions, NULL, NULL, 15, useMonitors ? callbackWithMon : NULL);
}

/***************************************************************************
*
*	Run BLAST on the BioseqPtr with the program "progname" and database
*	"dbname".  By setting bits on the Uint4 one can determine the 
*	amount of output returned.  These correspond to four of the five 
*	Uint1's in BLAST0Search:
*
*    	1st bit, if set omit matrix (return_matrix is FALSE);
*    	2nd bit, if set omit query (return_query is FALSE);
*    	3rd bit, if set omit query seq (return_query_seq_in_seg is FALSE);
*    	4th bit, if set omit db seq (return_db_seq_in_seg is FALSE);
*
*	"mask_seqloc" gives the position of the sequence to be masked.
*	A filtering program (e.g., dust or seg should return a SeqLocPtr),
*	indicating which sequence should be masked. 
*
************************************************************************/

BLAST0ResultPtr LIBCALL
BlastBioseq(BioseqPtr bsp, CharPtr progname, CharPtr dbname, CharPtr cmdLineOptions, BLAST0ResponsePtr PNTR blrespPtr, SeqLocPtr mask_seqloc, Uint4 output, ProgressCallback progCallback)
{
    BLAST0RequestPtr blreqp;
    Boolean is_na;
    BLAST0ResultPtr retval = NULL;
    Int4 gi = -1;
    Char textid[NETBLAP2_BUFLEN];
    PrepRequestInfo pri;
    SeqPortPtr spp;
    ValNodePtr vnp;

    if (bsp == NULL)
        return NULL;

    if (ISA_na(bsp->mol)) 
	is_na = TRUE;
    else
	is_na = FALSE;

    if (is_na)
    	spp = SeqPortNew(bsp, 0, -1, 0, Seq_code_iupacna);
    else
    	spp = SeqPortNew(bsp, 0, -1, 0, Seq_code_iupacaa);

    if (spp == NULL)
    {
        ErrPostEx(SEV_ERROR, 0, 0, "BlastBioseq: Unable to open SeqPort");
	return NULL;
    }


    SeqIdWrite(bsp->id, textid, PRINTID_FASTA_LONG, NETBLAP2_BUFLEN);

    pri.is_na = is_na;
    pri.defline = BioseqGetTitle(bsp);
    pri.options = cmdLineOptions;
    pri.dbname = dbname;
    pri.textid = StringSave(textid);
    pri.progname = progname;
    pri.length = BioseqGetLen(bsp);
    if ((vnp = bsp->id) != NULL) 
    {
	while (vnp)
	{
		if (vnp->choice == SEQID_GI)
		{
      			gi = vnp->data.intvalue;
			break;
		}
		vnp = vnp->next;
	}
    }
    pri.gi = gi;
    pri.spp = spp;

/* Set flags in "pri" that control the amount returned. */

    if (output & BLAST_SERVER_OMIT_MATRIX)
    	pri.return_matrix = FALSE;
    else
    	pri.return_matrix = TRUE;

    if (output & BLAST_SERVER_OMIT_QUERY)
    	pri.return_query = FALSE;
    else
    	pri.return_query = TRUE;

    if (output & BLAST_SERVER_OMIT_QUERY_SEQ_IN_SEG)
    	pri.return_query_seq_in_seg = FALSE;
    else
    	pri.return_query_seq_in_seg = TRUE;

    if (output & BLAST_SERVER_OMIT_DB_SEQ_IN_SEG)
    	pri.return_db_seq_in_seg = FALSE;
    else
    	pri.return_db_seq_in_seg = TRUE;

    pri.return_BLAST0result = TRUE;	/* always a BLAST0Result */

    pri.mask = mask_seqloc;
    pri.start = 0;

    blreqp = PrepareRequest(&pri);
    
    if (pri.spp)
	pri.spp = SeqPortFree(pri.spp);

    retval = SubmitResultBlastRequest(blreqp, blrespPtr, progCallback);

    BLAST0RequestFree(blreqp);

    return retval;
}

/************************************************************************
*	Perform BLAST on the sequence from a SeqLoc.  The use of SeqLocId
*	ensures that each SeqLoc is associated with only one SeqId,
*	otherwise NULL is returned.
************************************************************************/

static BLAST0ResultPtr LIBCALL
BlastSeqLoc(SeqLocPtr slp, CharPtr progname, CharPtr dbname, CharPtr cmdLineOptions, BLAST0ResponsePtr PNTR blrespPtr, SeqLocPtr mask_seqloc, ProgressCallback progCallback)
{
    BLAST0RequestPtr blreqp;
    BioseqPtr bsp;
    Boolean is_na;
    BLAST0ResultPtr retval = NULL;
    Int2 mol_type;
    Int4 gi = -1;
    Char textid[NETBLAP2_BUFLEN];
    PrepRequestInfo pri;
    SeqIdPtr sip;
    SeqPortPtr spp;
    ValNodePtr vnp;

    sip = SeqLocId(slp);
    if (sip == NULL)
    {
        ErrPostEx(SEV_ERROR, 0, 0, "BlastSeqLoc: Unable to get SeqId");
        return NULL;
    }

/* Find the bsp for the title (i.e., defline) */
    bsp = BioseqLockById(sip);
    if (bsp == NULL)
    {
        ErrPostEx(SEV_ERROR, 0, 0, "BlastSeqLoc: Unable to get Bioseq");
        return NULL;
    }

    mol_type = SeqLocMol(slp);
	
    if (ISA_na(mol_type)) 
	is_na = TRUE;
    else
	is_na = FALSE;

    if (is_na)
    	spp = SeqPortNewByLoc(slp, Seq_code_iupacna);
    else
    	spp = SeqPortNewByLoc(slp, Seq_code_iupacaa);

    if (spp == NULL)
    {
        ErrPostEx(SEV_ERROR, 0, 0, "BlastBioseq: Unable to open SeqPort");
	return NULL;
    }


    SeqIdWrite(sip, textid, PRINTID_FASTA_LONG, NETBLAP2_BUFLEN);

    pri.is_na = is_na;
    pri.defline = BioseqGetTitle(bsp);
    pri.options = cmdLineOptions;
    pri.dbname = dbname;
    pri.textid = StringSave(textid);
    pri.progname = progname;
    pri.length = SeqLocLen(slp);
    if ((vnp = sip) != NULL) 
    {
	while (vnp)
	{
		if (vnp->choice == SEQID_GI)
		{
      			gi = vnp->data.intvalue;
			break;
		}
		vnp = vnp->next;
	}
    }
    pri.gi = gi;
    pri.spp = spp;

    pri.mask = mask_seqloc;
    pri.start = 0;

    blreqp = PrepareRequest(&pri);

    retval = SubmitResultBlastRequest(blreqp, blrespPtr, progCallback);

    BLAST0RequestFree(blreqp);

    return retval;
}

/*****************************************************************
*	Submits a BLAST0RequestPtr and returns BLAST0ResponsePtr.
*	This function should not be used to submit search requests,
*	(i.e., perform a BLAST search, BLAST0-Request.search)
*	but for database inquiries etc.  To submit a search request
*	use BlastBioseq, BlastBioseq2, SubmitSearchRequest, etc.
*****************************************************************/

BLAST0ResponsePtr SubmitInfoRequest (BLAST0RequestPtr blreqp)

{
    Boolean success;
    BLAST0ResponsePtr blresp;

    success = BLAST0RequestAsnWrite (blreqp, asnout, NULL);
    if (success == FALSE)
    {
           ErrPostEx(SEV_WARNING, 0, 0, "BLAST0RequestAsnWrite failed");
	   return NULL;
    }
    AsnIoReset (asnout);
    /* read back response */
    blresp = BLAST0ResponseAsnRead(asnin, NULL);

    return blresp;
}

/********************************************************************
*
*	Prepare a request from the information in the PrepRequestInfoPtr.
*	Note that only two alphabets (Seq_code_ncbi4na and 
*	Seq_code_ncbistdaa) are supported.
*********************************************************************/
static BLAST0RequestPtr PrepareRequest (PrepRequestInfoPtr prip)

{

    Boolean is_na = prip->is_na;
    BLAST0RequestPtr blreqp;
    BLAST0SearchPtr blsrchp;
    BLAST0SequencePtr blseqp;
    BLAST0SeqDescPtr seqdesc;
    Uint1 seqAlphabet;
    Int4 index, start, total;
    Int2 cmdLineLen;
    Uint1 buffer[NETBLAP2_BUFLEN];
    CharPtr p, pSave, q;
    CharPtr cmdLineOptions = prip->options;
    SeqLocPtr mask_slp = prip->mask;
    Uint1 bitctr, residue, mask_residue;
    ValNodePtr node;
    SeqPortPtr spp = prip->spp;

    /* computed values */
    ByteStorePtr theSequence;
    Int2 alphabet;

    
    alphabet = is_na ? BLAST0SeqData_ncbi4na : BLAST0SeqData_ncbistdaa;
    seqAlphabet = is_na ? Seq_code_ncbi4na : Seq_code_ncbistdaa;

    if (is_na)
	mask_residue = 'N';
    else
	mask_residue = 'X';

    theSequence = NULL;
    bitctr = 0;
    total=0;
    start = prip->start;
    while (total < prip->length)
    {
	index=0;
    	while ((residue=SeqPortGetResidue(spp)) != SEQPORT_EOF)
    	{
		if (IS_residue(residue))
		{
    			buffer[index] = residue;
			index++;
		}
                else if (residue == SEQPORT_EOS)
                        ErrPostEx(SEV_INFO, 0, 0,"[Segment boundary]\n");
                else if (residue == SEQPORT_VIRT)
                        ErrPostEx(SEV_INFO, 0, 0,"[Virtual Sequence]\n");
                else
                        ErrPostEx(SEV_INFO, 
				0, 0,"Unrecognized residue [%d]\n", residue);

		if (index == NETBLAP2_BUFLEN)
			break;
    	}
	if (index > 0)
		MaskTheResidues(buffer, index, mask_residue, (start+total), mask_slp);
	total += index;
	if (index > 0)
	{
    	    if (is_na)
    	    	theSequence = StoreResiduesInBS(buffer, index, theSequence, &bitctr, seqAlphabet, Seq_code_iupacna, prip->length);
    	     else
    	    	theSequence = StoreResiduesInBS(buffer, index, theSequence, &bitctr, seqAlphabet, Seq_code_iupacaa, prip->length);
	}
	else if (total == 0)
	{
            ErrPostEx(SEV_ERROR, 0, 0, "PrepareRequest: no sequence");
	    return NULL;
	}
    }
    
    blreqp = ValNodeNew(NULL);
    blreqp->choice = BLAST0Request_search;
    blsrchp = BLAST0SearchNew();
    blreqp->data.ptrvalue = blsrchp;
    blsrchp->program = StringSave(prip->progname);
    blsrchp->database = StringSave(prip->dbname);
    blseqp = BLAST0SequenceNew();
    blsrchp->query = blseqp;
    blsrchp->options = NULL;
/* Go through the string cmdLineOptions taking off leading spaces. */
/* If cmdLineOptions consists only of spaces it will be rejected. */
    if (cmdLineOptions)
    	while (*cmdLineOptions == ' ')
		cmdLineOptions++;
    if (cmdLineOptions != NULL && *cmdLineOptions != NULLB)
    { /* parse the string */
        /* avoid using strtok(), since it's not reentrant */
        cmdLineLen = StrLen(cmdLineOptions);
        /* make a copy, and append a space to simplify parsing */
        pSave = MemNew(cmdLineLen + 2);
        StrCpy(pSave, cmdLineOptions);
        pSave[cmdLineLen] = ' ';
        pSave[cmdLineLen+1] = '\0';
        p = pSave;
        while ((q = StrPBrk(p, " \t")) != NULL)
        {
          *q = '\0';
          if (blsrchp->options == NULL)
          {
            node = ValNodeNew(NULL);
            blsrchp->options = node;
          } else {
            node = ValNodeNew(blsrchp->options);
          }
          node->data.ptrvalue = (VoidPtr) StringSave(p);
          p = q + 1;
          /* skip over the white space */
          while (*p && ( *p == ' ' || *p == '\t'))
            p++;
        }
        MemFree(pSave);
    }

/* Set the Booleans that determine the amount of output. */
    blsrchp->return_matrix = prip->return_matrix;
    blsrchp->return_query = prip->return_query;
    blsrchp->return_BLAST0result = prip->return_BLAST0result;
    blsrchp->return_query_seq_in_seg = prip->return_query_seq_in_seg;
    blsrchp->return_db_seq_in_seg = prip->return_db_seq_in_seg;

    seqdesc = BLAST0SeqDescNew();
    blseqp->desc = seqdesc;

    seqdesc->id = ValNodeNew(NULL);
    if (prip->gi != -1)
    {
      seqdesc->id->choice = BLAST0SeqId_giid;
      seqdesc->id->data.intvalue = prip->gi;
      seqdesc->id->next = ValNodeNew(NULL);
      seqdesc->id = seqdesc->id->next;
    }
    seqdesc->id->choice = BLAST0SeqId_textid;
    seqdesc->id->data.ptrvalue = prip->textid;
    seqdesc->defline = StringSave(prip->defline);

    blseqp->length = prip->length;
    blseqp->gcode = 1;
    blseqp->seq = ValNodeNew(NULL);
    blseqp->seq->choice = alphabet;
    blseqp->seq->data.ptrvalue = theSequence;

    return blreqp;

}

/*************************************************************************
*
*	MaskTheResidues masks up to max_length residues in buffer.
*	The residue to be used for masking (generally 'N' for nucleotides
*	and 'X' for proteins) is mask_residue.  offset tells how far
*	along the sequence the first residue in buffer is.  mask_slp
*	specifies which parts of the sequence to mask.
*
*************************************************************************/

static void
MaskTheResidues(Uint1Ptr buffer, Int4 max_length, Uint1 mask_residue, Int4 offset, SeqLocPtr mask_slp)

{
	SeqLocPtr slp=NULL;
        Int4 index, position, start, stop;
       
	while (mask_slp)
	{
        	while((slp = SeqLocFindNext(mask_slp, slp))!=NULL)
        	{
              		start = SeqLocStart(slp);
              		stop = SeqLocStop(slp);
			for (index=0; index<max_length; index++)
			{
				position = index+offset;
				if (position >= start) 
				{
					if (position <= stop)
						buffer[index] = mask_residue;
					else if (position > stop)
						break;
				}
			}
        	}
		mask_slp = mask_slp->next;
	}

}


/**************************************************************************
*
*	Submits a BLAST0RequestPtr and returns a SeqAlignPtr.
*	Checking is done that the BLAST0SearchPtr really specifies that a
*	SeqAlignPtr should be returned.
************************************************************************/
SeqAlignPtr
SubmitSeqAlignBlastRequest(BLAST0RequestPtr blreqp, BLAST0ResponsePtr PNTR blrespPtr, ProgressCallback progCallback)

{
	BLAST0SearchPtr search;
	SeqAlignPtr seqalign;

	if (blreqp && blreqp->choice == BLAST0Request_search)
	{
	   search = (BLAST0SearchPtr) blreqp->data.ptrvalue;
	   if (search->return_BLAST0result == FALSE)
	   	seqalign = (SeqAlignPtr) SubmitBlastRequest(blreqp, blrespPtr, progCallback, TRUE);
	   else
           	ErrPostEx(SEV_WARNING, 0, 0, "SubmitSeqAlignBlastRequest: wrong type of BLAST0Search");
	}
	return seqalign;
}

/**************************************************************************
*
*	Submits a BLAST0RequestPtr and returns a BLAST0ResultPtr.
*	Checking is done that the BLAST0SearchPtr really specifies that a
*	BLAST0ResultPtr should be returned.
************************************************************************/
BLAST0ResultPtr
SubmitResultBlastRequest(BLAST0RequestPtr blreqp, BLAST0ResponsePtr PNTR blrespPtr, ProgressCallback progCallback)

{
	BLAST0SearchPtr search;
	BLAST0ResultPtr result;

	if (blreqp && blreqp->choice == BLAST0Request_search)
	{
	   search = (BLAST0SearchPtr) blreqp->data.ptrvalue;
	   if (search->return_BLAST0result == TRUE)
	   	result = (BLAST0ResultPtr) SubmitBlastRequest(blreqp, blrespPtr, progCallback, FALSE);
	   else
           	ErrPostEx(SEV_WARNING, 0, 0, "SubmitResultBlastRequest: wrong type of BLAST0Search");
	}
	return result;
}

/**************************************************************************
*	
*	Submit a BLAST search request (i.e., a BLAST0-Search) using a 
*	BLAST0RequestPtr.  The return value is a VoidPtr, which must
*	be recast as either a BLAST0ResultPtr or a SeqAlignPtr by
*	the calling function.  The type of data returned is determined
*	by BLAST0Search.return_BLAST0result.  This should be checked by
*	the submitting function!
*
*************************************************************************/	

static VoidPtr
SubmitBlastRequest(BLAST0RequestPtr blreqp, BLAST0ResponsePtr PNTR blrespPtr, ProgressCallback progCallback, Boolean get_seqalign)

{
    Boolean done = FALSE, freeIt, success;
    BLAST0ResponsePtr bllist = NULL, blresp;
    BLAST0ResultPtr blastResult;
    SeqAlignPtr seqalign;
    VoidPtr retval = NULL;

    success = BLAST0RequestAsnWrite (blreqp, asnout, NULL);
    if (success == FALSE || asnout == NULL)
    {
           ErrPostEx(SEV_WARNING, 0, 0, "BLAST0RequestAsnWrite failed");
	   return NULL;
    }
    AsnIoReset (asnout);    
    
    /* now read back response */
    while (!done && (blresp = BLAST0ResponseAsnRead(asnin, NULL)) != NULL)
    {
        switch (blresp->choice) {
        case BLAST0Response_ack:
            done = TRUE;
            break;
        case BLAST0Response_result:
            blastResult = (BLAST0ResultPtr) blresp->data.ptrvalue;
            if (blastResult != NULL)
            {
                retval = (VoidPtr) blastResult;
                blresp->data.ptrvalue = NULL; /* for clean free */
		    
                BLAST0ResponseFree(blresp);
            }
            break;
        case BLAST0Response_seqalign:
            seqalign = (SeqAlignPtr) blresp->data.ptrvalue;
            if (seqalign != NULL)
            {
                retval = (VoidPtr) seqalign;
                blresp->data.ptrvalue = NULL; /* for clean free */
		    
                BLAST0ResponseFree(blresp);
            }
            break;
    
        case BLAST0Response_job_start:
        case BLAST0Response_job_progress:
        case BLAST0Response_job_done:
        default:
            freeIt = TRUE;
            if (progCallback != NULL)
            {
                if (! progCallback(blresp))
                { /* otherwise, make a linked list of other portions of response */
                    if (blrespPtr != NULL && get_seqalign == FALSE)
                    {
                        if (bllist != NULL)
                        {
                            bllist->next = blresp;
                        } else {
                            *blrespPtr = blresp;
                        }

                        blresp->next = NULL;
                        bllist = blresp;
                        freeIt = FALSE;
                    }
                }
            }
/* get_seqalign TRUE (BlastBioseq2 or BlastSeqLoc2); only warnings are saved. */
	    if (get_seqalign == TRUE && 
		blresp->choice == BLAST0Response_status)
	    {
                *blrespPtr = blresp;
		break;
	    }
            if (freeIt)
            {
                BLAST0ResponseFree(blresp);
            }
            break;
        }
    }
    
    return retval;
}

/***************************************************************************
*
*	Produce a SeqAnnot from the BLAST data, that was produced using a
*	Bioseq.
*
*	BLAST0ResponsePtr PNTR blrespPtr is used to return ONLY the 
*	BLAST0ResponsePtr of type warning ("BLAST0Response_warning"),
*	if an error occurs (e.g., bad option).
*
***************************************************************************/
SeqAnnotPtr LIBCALL
BlastBioseq2(BioseqPtr bsp, CharPtr progname, CharPtr dbname, CharPtr blast_params, BLAST0ResponsePtr PNTR blrespPtr, SeqLocPtr mask_seqloc, Boolean useMonitors)

{
    BLAST0RequestPtr blreqp;
    Boolean is_na;
    SeqAnnotPtr seqannot;
    SeqAlignPtr seqalign;
    Int4 gi = -1;
    Char textid[NETBLAP2_BUFLEN];
    PrepRequestInfo pri;
    SeqPortPtr spp;
    ValNodePtr vnp;

    if (bsp == NULL)
        return NULL;

    if (ISA_na(bsp->mol)) 
	is_na = TRUE;
    else
	is_na = FALSE;

    if (is_na)
    	spp = SeqPortNew(bsp, 0, -1, 0, Seq_code_iupacna);
    else
    	spp = SeqPortNew(bsp, 0, -1, 0, Seq_code_iupacaa);

    if (spp == NULL)
    {
        ErrPostEx(SEV_ERROR, 0, 0, "BlastBioseq: Unable to open SeqPort");
	return NULL;
    }


    SeqIdWrite(bsp->id, textid, PRINTID_FASTA_LONG, NETBLAP2_BUFLEN);

    pri.is_na = is_na;
    pri.defline = BioseqGetTitle(bsp);
    pri.options = blast_params;
    pri.dbname = dbname;
    pri.textid = StringSave(textid);
    pri.progname = progname;
    pri.length = BioseqGetLen(bsp);
    if ((vnp = bsp->id) != NULL) 
    {
	while (vnp)
	{
		if (vnp->choice == SEQID_GI)
		{
      			gi = vnp->data.intvalue;
			break;
		}
		vnp = vnp->next;
	}
    }
    pri.gi = gi;
    pri.spp = spp;

/* None of these are required. */
    pri.return_matrix = FALSE;
    pri.return_query = FALSE;
    pri.return_query_seq_in_seg = FALSE;
    pri.return_db_seq_in_seg = FALSE;
    pri.return_BLAST0result = FALSE;
    pri.mask = mask_seqloc;
    pri.start = 0;

    blreqp = PrepareRequest(&pri);
    
    if (pri.spp)
	pri.spp = SeqPortFree(pri.spp);

    seqalign = SubmitSeqAlignBlastRequest(blreqp, blrespPtr, useMonitors ? callbackWithMon : NULL);

    if (seqalign)
    {
    	seqannot = SeqAnnotNew();
    	seqannot->type = 2;
    	seqannot->data = seqalign;
    }
    else
	seqannot = NULL;

    BLAST0RequestFree(blreqp);

    return seqannot;
}


/***************************************************************************
*
*	Produce a SeqAnnot from the BLAST data, that was produced using a
*	SeqLoc.
*
*	BLAST0ResponsePtr PNTR blrespPtr is used to return ONLY the 
*	BLAST0ResponsePtr of type warning ("BLAST0Response_warning"),
*	if an error occurs (e.g., bad option).
*
***************************************************************************/
SeqAnnotPtr LIBCALL
BlastSeqLoc2(SeqLocPtr slp, CharPtr progname, CharPtr dbname, CharPtr blast_params, BLAST0ResponsePtr PNTR blrespPtr, SeqLocPtr mask_seqloc, Boolean useMonitors)

{
    SeqAlignPtr seqalign;
    SeqAnnotPtr seqannot;
    BioseqPtr bsp;
    BLAST0RequestPtr blreqp;
    Boolean is_na;
    Int2 mol_type;
    Int4 gi = -1;
    Char textid[NETBLAP2_BUFLEN];
    PrepRequestInfo pri;
    SeqIdPtr sip;
    SeqPortPtr spp;
    ValNodePtr vnp;

    sip = SeqLocId(slp);
    if (sip == NULL)
    {
        ErrPostEx(SEV_ERROR, 0, 0, "BlastSeqLoc: Unable to get SeqId");
        return NULL;
    }

/* Find the bsp for the title (i.e., defline) */
    bsp = BioseqLockById(sip);
    if (bsp == NULL)
    {
        ErrPostEx(SEV_ERROR, 0, 0, "BlastSeqLoc: Unable to get Bioseq");
        return NULL;
    }

    mol_type = SeqLocMol(slp);
	
    if (ISA_na(mol_type)) 
	is_na = TRUE;
    else
	is_na = FALSE;

    if (is_na)
    	spp = SeqPortNewByLoc(slp, Seq_code_iupacna);
    else
    	spp = SeqPortNewByLoc(slp, Seq_code_iupacaa);

    if (spp == NULL)
    {
        ErrPostEx(SEV_ERROR, 0, 0, "BlastBioseq: Unable to open SeqPort");
	return NULL;
    }


    SeqIdWrite(sip, textid, PRINTID_FASTA_LONG, NETBLAP2_BUFLEN);


    pri.is_na = is_na;
    pri.defline = BioseqGetTitle(bsp);
    pri.options = blast_params;
    pri.dbname = dbname;
    pri.textid = StringSave(textid);
    pri.progname = progname;
    pri.length = SeqLocLen(slp);
    if ((vnp = sip) != NULL) 
    {
	while (vnp)
	{
		if (vnp->choice == SEQID_GI)
		{
      			gi = vnp->data.intvalue;
			break;
		}
		vnp = vnp->next;
	}
    }
    pri.gi = gi;
    pri.spp = spp;

    pri.return_matrix = FALSE;
    pri.return_query = FALSE;
    pri.return_query_seq_in_seg = FALSE;
    pri.return_db_seq_in_seg = FALSE;
    pri.return_BLAST0result = FALSE;
    pri.mask = mask_seqloc;
    pri.start = SeqLocStart(slp);

    blreqp = PrepareRequest(&pri);
    
    if (pri.spp)
	pri.spp = SeqPortFree(pri.spp);

    seqalign = SubmitSeqAlignBlastRequest(blreqp, blrespPtr, useMonitors ? callbackWithMon : NULL);

    if (seqalign)
    {
    	seqannot = SeqAnnotNew();
    	seqannot->type = 2;
    	seqannot->data = seqalign;
    }
    else
	seqannot = NULL;

    if (seqannot)
	AdjustOffSetInSeqAnnot(seqannot, slp);

    BLAST0RequestFree(blreqp);

    return seqannot;
}

/***********************************************************************
*
*	Adjust the Offset in the SeqAnnot to correspond to the beginning
*	of the sequence and not where BLAST started. 
*
*	This function works on SeqAnnot's that contain SeqAlignPtr's of
*	type DenseDiagPtr and StdSegPtr, which is what BLAST returns.	
*
*	The first id in each DenseDiag or StdSeg should correspond to the 
*	original BLAST'ed sequence.  The offset is only found once as it 
*	should be the same for every pair of hits.
*
**********************************************************************/

static void
AdjustOffSetInSeqAnnot(SeqAnnotPtr sap, SeqLocPtr slp)

{
	DenseDiagPtr ddp;
	Int4 offset;
	SeqAlignPtr salp;
	SeqIdPtr sip=NULL;
	SeqIntPtr seq_int;
	SeqLocPtr seqloc, whole_slp;
	StdSegPtr ssp;
	
        if (sap != NULL)
        {
            if (sap->type == 2)
            {   
                salp = sap->data;
                while (salp)
                {
                   if (salp->segtype == 1)
                   {
                      ddp = salp->segs;
                      while (ddp)
                      {	/* Get the offset on the first call. */
			if (sip == NULL)
			{
			     sip = ddp->id;
			     whole_slp = 
				ValNodeAddPointer(NULL, SEQLOC_WHOLE, sip);
			     offset = 
				GetOffsetInLoc(slp, whole_slp, SEQLOC_START);
			     whole_slp = ValNodeFree(whole_slp);
			}
			ddp->starts[0] += offset;
                        ddp = ddp->next;
                      }
                   }   
		   else if (salp->segtype == 3)
		   {
			ssp = salp->segs;
			while (ssp)
			{
			     if (sip == NULL)
			     {
			        sip = ssp->ids;
			     	whole_slp = 
				   ValNodeAddPointer(NULL, SEQLOC_WHOLE, sip);
			    	offset = 
			    	   GetOffsetInLoc(slp, whole_slp, SEQLOC_START);
			        whole_slp = ValNodeFree(whole_slp);
			     }
			     seqloc = ssp->loc;
			     seq_int = seqloc->data.ptrvalue;
			     seq_int->from += offset;
			     seq_int->to += offset;
                             ssp = ssp->next;
			}
		   }
                   salp = salp->next;
                }
             }   
        }
}


/********************************************************************
*CharPtr FormatResultWithTemplate (HitdataPtr hitdata, StdPrintOptionsPtr Spop)
*
*	This function formats the data in the BLAST0ResultPtr using the
*	PrintTemplate functions found in the NCBI toolbox.  The data
*	in the BLAST0ResultPtr corresponds to the data in the ASN.1 
*	structure "BLAST0-Result".
***********************************************************************/

CharPtr 
FormatResultWithTemplate (BLAST0ResultPtr blresp, StdPrintOptionsPtr Spop)

{
	if (blresp == NULL || Spop == NULL)
		return NULL;

        if (! StdFormatPrint((Pointer)blresp,
                (AsnWriteFunc)BLAST0ResultAsnWrite, "StdBLAST0Result", Spop))
                        ErrPostEx(SEV_ERROR, 0, 0, "StdFormatPrint failed");

        if (Spop->ptr != NULL && *((CharPtr) (Spop->ptr)) != '\0')
                return Spop->ptr;
        else
                return NULL;
}	/* FormatResultWithTemplate */


/* convert from BLAST format to SeqFeat format */
static Int2
ConvertStrand(Int2 strand)
{
    switch (strand)
    {
    case BLAST0_Seq_interval_strand_plus:
    case BLAST0_Seq_interval_strand_plus_rf:
        return 1; /* plus */
    case BLAST0_Seq_interval_strand_minus:
    case BLAST0_Seq_interval_strand_minus_rf:
        return 2; /* minus */
    case BLAST0_Seq_interval_strand_both:
        return 3; /* both */
    default: /* unknown, or unable to convert */
        return 0;
    }
}


SeqAnnotPtr LIBCALL HitDataToSeqAnnotAlignment(BLAST0ResultPtr hdp, SeqIdPtr sip)
{
    SeqAnnotPtr sep;
    SeqAlignPtr sap;
    SeqAlignPtr last;
    /* Int4 dim; */ /* number of entries per BLAST0-segment */
    DenseDiagPtr ddp, lastddp;
    ScorePtr dstscore, lastscore;
    BLAST0HitListPtr hlp;
    BLAST0HSPPtr hsp;
    BLAST0SegmentPtr segp;
    BLAST0SeqIntervalPtr query;
    BLAST0SeqIntervalPtr hit;
    ScorePtr srcscore;
    CharPtr p;
    Boolean giidNotFound;
    BLAST0SeqDescPtr sdp;
    ValNodePtr vnp;
    SeqIdPtr hitid;

    if (hdp == NULL || hdp->hitlists == NULL)
        return NULL;

    SeqAsnLoad();

    sep = SeqAnnotNew();
    sep->type = 2; /* alignments */

    /* dim = hdp->dim;*/ /* should be 2, or 3 for BLAST3 */

    for (hlp = hdp->hitlists; hlp != NULL; hlp = hlp->next)
    {
        sap = SeqAlignNew();
        if (sep->data == NULL)
        {
            sep->data = sap;
        } else {
            last->next = sap;
        }
        last = sap;
        sap->type = 2; /* diags */
        sap->segtype = 1; /* dendiag */
/*        sap->dim = dim;*/
        sap->score = NULL;
        sap->segs = NULL;
        for (hsp = hlp->hsps; hsp != NULL; hsp = hsp->next)
        {
            segp = hsp->segs;
            if (segp != NULL && segp->next != NULL &&
                (query = segp->loc) != NULL && (hit = segp->next->loc))
            {
                ddp = DenseDiagNew();
                if (sap->segs == NULL)
                {
                    sap->segs = ddp;
                } else {
                    lastddp->next = ddp;
                }
                lastddp = ddp;
/*                ddp->dim = dim;*/
                ddp->len = hsp->len;
                ddp->scores = NULL;
                ddp->starts = (Int4Ptr) MemNew(2 * sizeof(Int4));
                ddp->starts[0] = query->from;
                ddp->starts[1] = hit->from;
                ddp->strands = (Uint1Ptr) MemNew(2 * sizeof(Uint1));
                ddp->strands[0] = ConvertStrand(query->strand);
                ddp->strands[1] = ConvertStrand(hit->strand);

                if (hlp->seqs != NULL)
                {
                    sdp = hlp->seqs->desc;
                    giidNotFound = TRUE;
                    hitid = NULL;
                    for (vnp = sdp->id; vnp != NULL; vnp = vnp->next)
                    {
                        if (vnp->choice == BLAST0SeqId_giid &&
                            vnp->data.intvalue > 0)
                        {
                            SeqIdFree(hitid);
                            hitid = ValNodeNew(NULL);
                            hitid->choice = SEQID_GI;
                            hitid->data.intvalue = vnp->data.intvalue;
                            giidNotFound = FALSE;
                        } else {
                            if (vnp->choice == BLAST0SeqId_textid &&
                                giidNotFound && vnp->data.ptrvalue != NULL)
                            {
                                hitid = SeqIdParse(vnp->data.ptrvalue);
                            }
                        }
                    }
                    ddp->id = SeqIdDup(sip);
                    ddp->id->next = hitid;
                }
                for (srcscore = (ScorePtr) hsp->scores; srcscore != NULL; srcscore = srcscore->next)
                { /* just copy the scores */
                    dstscore = ScoreNew();
                    if (ddp->scores == NULL)
                    {
                        ddp->scores = dstscore;
                    } else {
                        lastscore->next = dstscore;
                    }
                    lastscore = dstscore;
                    dstscore->id = ObjectIdDup(srcscore->id);
                    dstscore->value = srcscore->value;
                    dstscore->choice = srcscore->choice;
                }
            }
        }
    }

    return sep;
}

SeqAnnotPtr LIBCALL HitDataToSeqAnnot(BLAST0ResultPtr hdp, SeqIdPtr sip)
{
    SeqAnnotPtr sep;
    SeqFeatPtr sfp, last;
    BLAST0HitListPtr hlp;
    BLAST0SegmentPtr segp;
    /*    Int4 dim;*/ /* number of entries per BLAST0-segment */
    BLAST0SeqIntervalPtr query;
    BLAST0SeqIntervalPtr hit;
    SeqLocPtr querySeqLoc;
    SeqLocPtr hitSeqLoc;
    SeqIntPtr querySeqInt;
    SeqIntPtr hitSeqInt;
    CharPtr p;
    Boolean giidNotFound;
    BLAST0SeqDescPtr sdp;
    ValNodePtr vnp;

    if (hdp == NULL || hdp->hitlists == NULL)
        return NULL;

    SeqAsnLoad();

    sep = SeqAnnotNew();
    sep->type = 1; /* Feature table */

/*    dim = hdp->dim;*/ /* should be 2, or 3 for BLAST3 */

    for (hlp = hdp->hitlists; hlp != NULL; hlp = hlp->next)
    {
        if (hlp->hsps != NULL)
        {
            segp = hlp->hsps->segs; /* ignore any other HSPs */
            if (segp != NULL && segp->next != NULL &&
                (query = segp->loc) != NULL && (hit = segp->next->loc))
            {
                sfp = SeqFeatNew();
                if (sep->data == NULL)
                {
                    sep->data = sfp;
                } else {
                    last->next = sfp;
                }
                last = sfp;

                sfp->data.choice = SEQFEAT_SEQ; /* value for seq int */
                sfp->data.value.ptrvalue = (Pointer) ValNodeNew(NULL);
                hitSeqLoc = sfp->data.value.ptrvalue;
                hitSeqLoc->choice = SEQLOC_INT;
                hitSeqInt = SeqIntNew();
                hitSeqLoc->data.ptrvalue = (Pointer) hitSeqInt;
                hitSeqInt->strand = ConvertStrand(hit->strand);
                hitSeqInt->from = hit->from;
                hitSeqInt->to = hit->to;

                if (hlp->seqs != NULL)
                {
                    sdp = hlp->seqs->desc;
                    if (sdp->defline != NULL)
                    {
                        sfp->comment = StringSave(sdp->defline);
                    }
                    giidNotFound = TRUE;
                    for (vnp = sdp->id; vnp != NULL; vnp = vnp->next)
                    {
                        if (vnp->choice == BLAST0SeqId_giid &&
                            vnp->data.intvalue > 0)
                        {
                            SeqIdFree(hitSeqInt->id);
                            hitSeqInt->id = ValNodeNew(NULL);
                            hitSeqInt->id->choice = SEQID_GI;
                            hitSeqInt->id->data.intvalue = vnp->data.intvalue;
                            giidNotFound = FALSE;
                        } else {
                            if (vnp->choice == BLAST0SeqId_textid &&
                                giidNotFound && vnp->data.ptrvalue != NULL)
                            {
                                hitSeqInt->id = SeqIdParse(vnp->data.ptrvalue);
                            }
                        }
                    }
                }

                sfp->location = ValNodeNew(NULL);
                querySeqLoc = sfp->location;
                querySeqLoc->choice = SEQLOC_INT;
                querySeqInt = SeqIntNew();
                querySeqLoc->data.ptrvalue = (Pointer) querySeqInt;
                querySeqInt->strand = ConvertStrand(query->strand);
                querySeqInt->from = query->from;
                querySeqInt->to = query->to;
                querySeqInt->id = SeqIdDup(sip);
            }
        }
    }

    return sep;
}


static Boolean ReestablishNetBlast(void)
{
    return GenericReestablishNet("blast1", TRUE);
}

static Boolean GenericReestablishNet(CharPtr svcName, Boolean showErrs)
{
    MonitorPtr mon = NULL;
    Boolean retval;
    CharPtr buf;

    buf = MemNew(2 * StrLen(svcName) + 60);

    if (showErrs) {
        sprintf (buf, "Re-establishing %s Service", svcName);
        mon = MonitorStrNew(buf, 40);
        sprintf (buf, "Requesting %s service", svcName);
        MonitorStrValue(mon, buf);
    }
    NetFini();
    retval = TRUE;

    if (! NetInit())
    {
        sprintf (buf, "%s get failed; re-contacting dispatcher", svcName);
        MonitorStrValue(mon, buf);
        retval = FALSE;
        if (ForceNetInit())
        { /* successfully established contact w/dispatcher */
            sprintf (buf, "%s get failed; re-requesting %s service",
                     svcName, svcName);
            MonitorStrValue(mon, buf);
            retval = NetInit();
        }
        else {
            ErrPost(CTX_UNKNOWN, 1, "Unable to re-contact dispatcher");
            if (showErrs) {
                ErrShow();
            }
        }
    }

    MonitorFree(mon);

    if (! retval )
    {
        sprintf (buf, "Unable to re-establish %s service", svcName);
        ErrPost(CTX_UNKNOWN, 1, buf);
        if (showErrs) {
            ErrShow();
        }
    }

    MemFree(buf);
    return retval;
}

static Boolean
NetInit(void)
{
    if (num_attached++ > 0)
        return TRUE;

    return ((dispatcher = NI_GenericInit(NULL, NULL, TRUE, NULL, 0)) != NULL);
}


static Boolean ForceNetInit(void)
{
    Boolean retval;

    reallyFinal = FALSE;
    num_attached = 0; /* force re-attempt to contact dispatcher */
    retval = NetInit();
    reallyFinal = TRUE;

    return retval;
}

static Boolean NetFini(void)
{
    if (num_attached > 0)
        num_attached--;

    if (num_attached == 0)
    {
        NI_ServiceDisconnect(svcp);
        svcp = NULL;
        NI_EndServices (dispatcher);
        dispatcher = NULL;
    }

    return TRUE;
}
/*****************************************************************************
*
*   StoreResiduesInBS(Uint1Ptr, newcode, oldcode, len)
	Transfers the residues in buffer to the ByteStore "to". 
*	On the first call the ByteStore "to" should be NULL
*	and len should indicate the total number of residues in
*	the sequence.  bitctr_to may be any number.  On subsequent
*	calls bitctr_to  and to should be passed back in.
*	On every call buf_len should indicate how many residues are
*	to be transferred from buffer to "to".  
*
*	At present (2/22/95) this only works with oldcode of iupac[na]a.
*
*****************************************************************************/
static ByteStorePtr StoreResiduesInBS (Uint1Ptr buffer, Int4 buf_len, ByteStorePtr to, Uint1Ptr bitctr_to, Uint1 newcode, Uint1 oldcode, Int4 len)

{
    Boolean first_time;
    Int4 index, storelen;
    Uint1 byte_from, residue_from, bitctr_from, mask_from, lshift_from,
          rshift_from, bc_from ,
          byte_to, lshift_to[5], bc_to, byte_tmp;
    SeqMapTablePtr smtp;

    if ((! oldcode) || (! newcode) || (len <= 0))
        return NULL;

    if (to)
	first_time = FALSE;
    else
	first_time = TRUE;

    if ((smtp = SeqMapTableFind(newcode, oldcode)) == NULL)
        return NULL;

    if (newcode == Seq_code_ncbi2na)
        storelen = (len / 4) + 1;
    else if (newcode == Seq_code_ncbi4na)
        storelen = (len / 2) + 1;
    else
        storelen = len;

    if (first_time)
    	to = BSNew((Uint4)storelen);

    if (to == NULL)
        return NULL;

    switch (oldcode)
    {
        case Seq_code_ncbi2na:
            bc_from = 4;            /* bit shifts needed */
            rshift_from = 6;
            lshift_from = 2;
            mask_from = 192;
            break;
        case Seq_code_ncbi4na:
            bc_from = 2;
            rshift_from = 4;
            lshift_from = 4;
            mask_from = 240;
            break;
        default:
            bc_from = 1;
            rshift_from = 0;
            lshift_from = 0;
            mask_from = 255;
            break;
    }

    lshift_to[1] = 0;
    switch (newcode)
    {
        case Seq_code_ncbi2na:
            bc_to = 4;            /* bit shifts needed */
            lshift_to[2] = 2;
            lshift_to[3] = 4;
            lshift_to[4] = 6;
            break;
        case Seq_code_ncbi4na:
            bc_to = 2;
            lshift_to[2] = 4;
            break;
        default:
            bc_to = 1;
            break;
    }

    if (first_time)
    	*bitctr_to = bc_to;
    byte_to = 0;
    bitctr_from = 0;

    index = 0;
    while (index < buf_len)
    {
        if (! bitctr_from)        /* need a new byte */
        {
            byte_from = buffer[index];
            bitctr_from = bc_from;
        }
        residue_from = byte_from & mask_from;
        residue_from >>= rshift_from;
        byte_from <<= lshift_from;
        bitctr_from--;

        byte_tmp = SeqMapTableConvert(smtp, residue_from);
		if (byte_tmp == INVALID_RESIDUE)
		{
			ErrPostEx(SEV_ERROR, 0, 0, "StoreResiduesInBS: invalid residue [%d=%c]",
				(int)residue_from, (char)residue_from);
			BSFree(to);
			return NULL;
		}
        byte_tmp <<= lshift_to[*bitctr_to];
		byte_to |= byte_tmp;
        (*bitctr_to)--;
        if (! *bitctr_to)
        {
            BSPutByte(to, byte_to);
            *bitctr_to = bc_to;
            byte_to = 0;
        }
    
        index++;
    }

    if (*bitctr_to != bc_to)      /* partial byte not written */
        BSPutByte(to, byte_to);

    return to;
}

/**************************************************************************
*
*	format the usage for a blast program

*	THis function is a clone of print_usage in busage.c in blastapp.
*	This function should be moved to "blast.c" soon, then both
*	server and client versions of BLAST can share it.
**************************************************************************/
int
print_usage(FILE *fp, ValNodePtr vnp)
{
	Char buf[512];
	CharPtr string, ptr;

	if (fp == NULL)
		return 0;

	if (vnp == NULL)
		return 0;

	fflush(stdout);
	fflush(fp);

	while (vnp)
	{
		string = vnp->data.ptrvalue;
		ptr = buf;
		while (*string != NULLB)
		{
/* If the character is a tilde, do a line return; if no tilde precedes the
end of the string, print without a line return! */
			if (*string == '~')
			{
				*ptr = NULLB;
				fprintf(fp, "%s\n", buf);
				ptr = buf;
				string++;
				while (*string == '~')
				{
					fprintf(fp, "\n");
					string++;
				}
				if (*string == NULLB)
					break;
			}
			else if (*(string+1) == NULLB)
			{
				ptr++;
				*ptr = NULLB;
				fprintf(fp, "%s", buf);
			}
			*ptr = *string;
			ptr++; string++;
		}
		
		vnp = vnp->next;
	}

	return 0;
}
