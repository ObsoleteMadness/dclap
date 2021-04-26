/* asndhuff.c
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
* File Name: asndhuff.h
*
* Author:  Greg Schuler
*
* Version Creation Date: 9/23/92
*
* $Revision: 2.0 $
*
* File Description:
	asndhuff  --  decompresses a compressed ASN,1 (CASN) file.

	A Simple program to demonstrated the functions in casn.c
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#include <casn.h>

void do_medline PROTO((CASN_Handle casnh, AsnIoPtr aout));
void do_sequence PROTO((CASN_Handle casnh, AsnIoPtr aout));
void stream_medline PROTO((AsnIoPtr aip));
void stream_sequence PROTO((AsnIoPtr aip));

#define NUMARGS 2
Args myargs[NUMARGS] = {
	{ "Input file", NULL, NULL, NULL, FALSE, 'i', ARG_FILE_IN, 0.0,0,NULL},
	{ "Output file", NULL, NULL, NULL, TRUE, 'o', ARG_FILE_OUT, 0.0,0,NULL}};


Int2 Main ()
{
	CASN_Handle casnh;
	AsnIoPtr     aout, aip;

    if (! GetArgs("AsnDHuff 1.0", NUMARGS, myargs))
		return 1;

	if (! SeqEntryLoad())
	{
		ErrShow();
		return 1;
	}

	if ((casnh = CASN_Open(myargs[0].strvalue)) == NULL)
	{
		ErrShow();
		return 1;
	}

	/*****************************************************************
	*
	*   this part reads complete entries from compressed file and writes
	*   them to an uncompressed file
	*     if output file not given, it skips this step
	******************************************************************/

	if (myargs[1].strvalue != NULL)
	{
		if ((aout = AsnIoOpen(myargs[1].strvalue,"wb")) ==NULL)
		{
			Message(MSG_FATAL, "Can't create %s\n", myargs[1].strvalue);
			return 1;
		}

		switch (CASN_DocType(casnh))
		{
			case CASN_TypeMed :
				do_medline(casnh,aout);
				break;
		
			case CASN_TypeSeq :
				do_sequence(casnh,aout);
				break;

			default :
				Message(MSG_ERROR,"Unknown document type");
				break;
		}

		AsnIoClose(aout);

		CASN_Close(casnh);     /* close and reopen file */
		if ((casnh = CASN_Open(myargs[0].strvalue)) == NULL)
		{
			ErrShow();
			return 1;
		}
	}

	
	/*************************************************************************
	*
	*   This part processes the compressed file as a regular asn stream
	*   displaying title lines for sequence or medline records
	*
	*************************************************************************/

	aip = CASN_GetAsnIoPtr(casnh);    /* get the stream pointer */

	switch (CASN_DocType(casnh))
	{
		case CASN_TypeMed :
			stream_medline(aip);
			break;
	
		case CASN_TypeSeq :
			stream_sequence(aip);
			break;

		default :
			Message(MSG_ERROR,"Unknown document type");
			break;
	}


	CASN_Close(casnh);

	return 0;
}


void do_medline (CASN_Handle casnh, AsnIoPtr aout)
{
	MedlineEntryPtr entry;
	DataVal val;
	Int4 total, count;
	AsnModulePtr amp = AsnAllModPtr();
	AsnTypePtr typePubSet = AsnTypeFind(amp,"Pub-set");
	AsnTypePtr typeMedline = AsnTypeFind(amp,"Pub-set.medline");
	AsnTypePtr typeMedlineE = AsnTypeFind(amp,"Pub-set.medline.E");
	Handle pmonitor;

	total = CASN_DocCount(casnh);

	pmonitor = MonitorIntNew("MEDLINE Entries Decompressed", (Int4)0, total);

	AsnWrite(aout,typePubSet,&val);
	AsnStartStruct(aout,typeMedline);

	for (count=0; entry=CASN_NextMedlineEntry(casnh); ++count)
	{
		MedlineEntryAsnWrite(entry,aout,typeMedlineE);
		MedlineEntryFree(entry);
		MonitorIntValue(pmonitor, count+1);
	}

	AsnEndStruct(aout,typeMedline);
	MonitorFree(pmonitor);
	if (count != total)
		ErrPost(1,1, "Only %ld of %ld records read", count, total);
	return;
}


void do_sequence (CASN_Handle casnh, AsnIoPtr aout)
{
	SeqEntryPtr entry;
	DataVal val;
	Int4 total, count;
	AsnModulePtr amp = AsnAllModPtr();
	AsnTypePtr typeSet = AsnTypeFind(amp,"Bioseq-set");
	AsnTypePtr typeSetSet = AsnTypeFind(amp,"Bioseq-set.seq-set");
	AsnTypePtr typeSetSetE = AsnTypeFind(amp,"Bioseq-set.seq-set.E");
	Handle pmonitor;

	if (typeSet==NULL || typeSetSet==NULL || typeSetSetE==NULL)
	{
		Message(MSG_ERROR, "one or more atp's is NULL\n");
		return;
	}

	total = CASN_DocCount(casnh);

	pmonitor = MonitorIntNew("SEQUENCE Entries Decompressed", (Int4)0, total);

	AsnStartStruct(aout,typeSet);
	AsnStartStruct(aout,typeSetSet);

	for (count=0; entry=CASN_NextSeqEntry(casnh); ++count)
	{
		SeqEntryAsnWrite(entry,aout,typeSetSetE);
		SeqEntryFree(entry);
		MonitorIntValue(pmonitor, count+1);
	}

	AsnEndStruct(aout,typeSetSet);
	AsnEndStruct(aout,typeSet);

	MonitorFree(pmonitor);

	if (count != total)
		ErrPost(1,1, "Only %ld of %ld records read", count, total);
	return;
}

void stream_medline (AsnIoPtr aip)
{
	AsnTypePtr atp;
	DataVal val;
	AsnModulePtr amp = AsnAllModPtr();
	AsnTypePtr typePubSet = AsnTypeFind(amp,"Medline-entry");
	AsnTypePtr title = AsnTypeFind(amp,"Title.E.name");
	Handle pmonitor;

	pmonitor = MonitorStrNew("MEDLINE Titles Decompressed", 60);

	atp = typePubSet;
	while ((atp = AsnReadId(aip, amp, atp)) != NULL)
	{
		if (atp == title)
		{
			AsnReadVal(aip, atp, &val);
			MonitorStrValue(pmonitor, (CharPtr)val.ptrvalue);
			MemFree(val.ptrvalue);
		}
		else
			AsnReadVal(aip, atp, NULL);
	}
	MonitorFree(pmonitor);
	return;
}

void stream_sequence (AsnIoPtr aip)
{
	AsnTypePtr atp;
	DataVal val;
	AsnModulePtr amp = AsnAllModPtr();
	AsnTypePtr typeSeqEntry = AsnTypeFind(amp,"Seq-entry");
	AsnTypePtr title = AsnTypeFind(amp,"Seq-descr.E.title");
	Handle pmonitor;

	pmonitor = MonitorStrNew("Sequence Titles Decompressed", 60);

	atp = typeSeqEntry;
	while ((atp = AsnReadId(aip, amp, atp)) != NULL)
	{
		if (atp == title)
		{
			AsnReadVal(aip, atp, &val);
			MonitorStrValue(pmonitor, (CharPtr)val.ptrvalue);
			MemFree(val.ptrvalue);
		}
		else
			AsnReadVal(aip, atp, NULL);
		if (! AsnGetLevel(aip))       /* finished reading a Seq-entry */
			atp = typeSeqEntry;		  /* reset to start next one */
	}
	MonitorFree(pmonitor);
	return;
}



