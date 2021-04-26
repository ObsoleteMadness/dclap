/*  subutil.c
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
* File Name:  subutil.c
*
* Author:  James Ostell
*   
* Version Creation Date: 11/3/93
*
* $Revision: 1.1 $
*
* File Description: Utilities for creating ASN.1 submissions
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#include "subutil.h"
#include "valid.h"
#include <seqport.h>

/*****************************************************************************
*
*   prototypes for internal functions
*
*****************************************************************************/
BioseqPtr NCBISubNewBioseq (
	NCBISubPtr submission ,
	CharPtr local_name ,
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int2 molecule_class,
	Int2 molecule_type ,
	Int4 length ,
	Int2 topology ,
	Int2 strandedness );

Boolean StripNonAsciiChars (CharPtr str);
ValNodePtr NewDescrOnSeqEntry (SeqEntryPtr entry, Int2 type);
BioseqPtr GetBioseqFromChoice (SeqEntryPtr the_seq, CharPtr local_name, CharPtr the_routine);
Boolean AddIntToSeqFeat (SeqFeatPtr sfp, Int4 from, Int4 to, BioseqPtr bsp,
							Int2 fuzz_from, Int2 fuzz_to, Int2 strand);
Boolean AddFeatureToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	SeqFeatPtr feature );


/*****************************************************************************
*
*   DefaultSubErrorFunc(msg)
*   	default error handler
*
*****************************************************************************/
Boolean DefaultSubErrorFunc (CharPtr msg)
{
	Message(MSG_ERROR, msg);
	return TRUE;
}

/*****************************************************************************
*
*   Create/Free the SubmitBlock
*
*****************************************************************************/

NCBISubPtr NCBISubBuild (
	CharPtr name,
	CharPtr PNTR address ,
	CharPtr phone ,
	CharPtr fax ,
	CharPtr email,
	Boolean hold_until_publish ,
	Int2 release_month,			  /* all 0 if no hold date */
	Int2 release_day,
	Int2 release_year )
{
	NCBISubPtr nsp;
	SeqSubmitPtr ssp;
	SubmitBlockPtr sbp;
	ContactInfoPtr cip;
	DatePtr dp;
	ValNodePtr vnp;

	ErrSetOpts(ERR_ADVISE, ERR_LOG_OFF);

	if (! SeqEntryLoad())     /* can't find ncbi files */
	{
		Message(MSG_ERROR, "Can't load NCBI data files");
		return NULL;
	}

	nsp = MemNew(sizeof(NCBISub));

	ssp = SeqSubmitNew();
	nsp->ssp = ssp;
	ssp->datatype = 1;    /* seqentrys */

	sbp = SubmitBlockNew();
	ssp->sub = sbp;
	sbp->hup = hold_until_publish;

	if (release_year != 0)
	{
		dp = DateNew();
		if (! DateWrite(dp, release_year, release_month, release_day, NULL))
		{
			ErrShow();
			DateFree(dp);
		}
		else
			sbp->reldate = dp;
	}

	cip = ContactInfoNew();
	sbp->contact = cip;

	cip->name = StringSave(name);
	while (* address != NULL)
	{
		vnp = ValNodeNew(cip->address);
		if (cip->address == NULL)
			cip->address = vnp;
		vnp->data.ptrvalue = (Pointer)(StringSave(*address));
		address++;
	}

	cip->phone = StringSave(phone);
	cip->fax = StringSave(fax);
	cip->email = StringSave(email);

	return nsp;
}

/*****************************************************************************
*
*   Write the Submission to a disk file
*
*****************************************************************************/
Boolean NCBISubWrite (
	NCBISubPtr ssp,
	CharPtr filename )
{
	AsnIoPtr aip;
	Boolean retval = FALSE;

	aip = AsnIoOpen(filename, "w");
	if (aip == NULL)
	{
		ErrShow();
		return retval;
	}

	if (! SeqSubmitAsnWrite(ssp->ssp, aip, NULL))
	{
		ErrShow();
	}
	else
		retval = TRUE;
	AsnIoClose(aip);
	return retval;
}

/*****************************************************************************
*
*   Free the NCBISub and associated data
*
*****************************************************************************/
NCBISubPtr NCBISubFree (
	NCBISubPtr ssp )
{
	SeqSubmitFree(ssp->ssp);
	return (NCBISubPtr)(MemFree(ssp));
}

/*****************************************************************************
*
*   add the Cit-sub to the submission
*
*****************************************************************************/
Boolean CitSubForSubmission (
	NCBISubPtr submission,
	PubPtr cit_sub )
{
	if ((submission == NULL) || (cit_sub == NULL)) return FALSE;

	if ((cit_sub->choice != PUB_Sub) || (cit_sub->data.ptrvalue == NULL))
	{
		Message(MSG_ERROR, "You must give a Cit-sub to CitSubForSubmission()");
		return FALSE;
	}
	submission->ssp->sub->cit = (CitSubPtr)(cit_sub->data.ptrvalue);
	ValNodeFree(cit_sub);   /* free just the Pub part */
	return TRUE;
}

/*****************************************************************************
*
*   Add Entries to the Submission
*   Add Sequences to the Entries
*
*****************************************************************************/

/*****************************************************************************
*
*   utility function for making all new Bioseqs here
*
*****************************************************************************/
BioseqPtr NCBISubNewBioseq (
	NCBISubPtr submission ,
	CharPtr local_name ,
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int2 molecule_class,
	Int2 molecule_type ,
	Int4 length ,
	Int2 topology ,
	Int2 strandedness )
{
	BioseqPtr bsp;
	TextSeqIdPtr tsip;
	ValNodePtr vnp;
	ObjectIdPtr oip;
	Int2 i;
	CharPtr tmp;
	Boolean fail;

	bsp = BioseqNew();
	bsp->mol = (Uint1) molecule_class;
	bsp->repr = Seq_repr_raw;   /* default case */
	if (molecule_type != 0)
	{
		vnp = ValNodeNew(NULL);
		bsp->descr = vnp;
		vnp->choice = Seq_descr_mol_type;
		vnp->data.intvalue = molecule_type;
	}
	bsp->length = length;
	bsp->topology = (Uint1)topology;
	bsp->strand = (Uint1)strandedness;

	vnp = NULL;

	if (local_name != NULL)
	{
		vnp = ValNodeNew(bsp->id);
		if (bsp->id == NULL)
			bsp->id = vnp;
		vnp->choice = SEQID_LOCAL;
		oip = ObjectIdNew();
		vnp->data.ptrvalue = oip;
		oip->str = StringSave(local_name);
	}

	if (genbank_locus != NULL)
	{
		fail = FALSE;
		if (! IS_UPPER(*genbank_locus))
			fail = TRUE;

		i = 0;
		for (tmp = genbank_locus; *tmp != '\0'; tmp++, i++)
		{
			if (! ((IS_UPPER(*tmp)) || (IS_DIGIT(*tmp))))
			{
				fail = TRUE;
				break;
			}
		}
		if ((i < 1) || (i > 10))
			fail = TRUE;
		if (fail)
		{
			Message(MSG_ERROR,
				"A GenBank LOCUS is an upper case letter, then up to 9 upper case letters or digits [%s]",
				genbank_locus);
			genbank_locus = NULL;
		}
	}
	if (genbank_accession != NULL)
	{
		fail = FALSE;
		if (! IS_UPPER(*genbank_accession))
			fail = TRUE;
		tmp = genbank_accession + 1;
		for (i = 0; i < 5; i++, tmp++)
		{
			if (! IS_DIGIT(*tmp))
				fail = TRUE;
		}
		if (*tmp != '\0')
			fail = TRUE;
		if (fail)
		{
			Message(MSG_ERROR, "A GenBank accession is an upper case letter followed by 5 digits [%s]",
				genbank_accession);
			genbank_accession = NULL;
		}
	}

	if ((genbank_locus != NULL) || (genbank_accession != NULL))
	{
		vnp = ValNodeNew(bsp->id);
		if (bsp->id == NULL)
			bsp->id = vnp;
		vnp->choice = SEQID_GENBANK;
		tsip = TextSeqIdNew();
		vnp->data.ptrvalue = tsip;
		if (genbank_locus != NULL)
			tsip->name = StringSave(genbank_locus);
		if (genbank_accession != NULL)
			tsip->accession = StringSave(genbank_accession);
	}

	if (gi_number > 0)
	{
		vnp = ValNodeNew(bsp->id);
		if (bsp->id == NULL)
			bsp->id = vnp;
		vnp->choice = SEQID_GI;
		vnp->data.intvalue = gi_number;
	}

	if (vnp == NULL)   /* no ids */
	{
		Message(MSG_ERROR, "You must have some type of ID to create a Bioseq");
		bsp = BioseqFree(bsp);
	}

	return bsp;
}

/*****************************************************************************
*
*   One Bioseq in the entry
*
*****************************************************************************/
SeqEntryPtr AddSeqOnlyToSubmission (
	NCBISubPtr submission,
	CharPtr local_name ,
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int2 molecule_class,
	Int2 molecule_type ,
	Int4 length ,
	Int2 topology ,
	Int2 strandedness )
{
	BioseqPtr bsp;
	SeqEntryPtr sep, tmp;

	sep = SeqEntryNew();
	sep->choice = 1;    /* Bioseq */

	if (submission->ssp->data == NULL)
		submission->ssp->data = (Pointer)sep;
	else
	{
		for (tmp = submission->ssp->data; tmp->next != NULL; tmp = tmp->next)
			continue;
		tmp->next = sep;
	}

	bsp = NCBISubNewBioseq (submission , local_name, genbank_locus,
		genbank_accession, gi_number, molecule_class, molecule_type,
		length, topology, strandedness);

	sep->data.ptrvalue = bsp;
	
	return sep;
}

/*****************************************************************************
*
*   Segmented Set
*
*****************************************************************************/
SeqEntryPtr AddSegmentedSeqToSubmission (
	NCBISubPtr submission ,
	CharPtr local_name ,
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int2 molecule_class,
	Int2 molecule_type ,
	Int4 length ,
	Int2 topology ,
	Int2 strandedness )
{
	BioseqPtr bsp;
	BioseqSetPtr bssp;
	SeqEntryPtr sep, the_set, tmp;

	bssp = BioseqSetNew();
	bssp->_class = 2;       /* segset */
	the_set = SeqEntryNew();
	the_set->choice = 2;    /* set */
	the_set->data.ptrvalue = bssp;

	bsp = NCBISubNewBioseq (submission , local_name, genbank_locus,
		genbank_accession, gi_number, molecule_class, molecule_type,
		length, topology, strandedness);
	bsp->repr = Seq_repr_seg;
	bsp->seq_ext_type = 1;

	sep = SeqEntryNew();
	sep->choice = 1;   /* Bioseq */
	sep->data.ptrvalue = bsp;
	bssp->seq_set = sep;

	if (submission->ssp->data == NULL)
		submission->ssp->data = (Pointer)the_set;
	else
	{
		for (tmp = (SeqEntryPtr) submission->ssp->data; tmp->next != NULL; tmp = tmp->next)
			continue;
		tmp->next = the_set;
	}

	return the_set;
}

SeqEntryPtr AddSeqToSegmentedEntry (
	NCBISubPtr submission,
	SeqEntryPtr segmented_seq_entry,
	CharPtr local_name ,
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int2 molecule_class,
	Int2 molecule_type ,
	Int4 length ,
	Int2 topology ,
	Int2 strandedness )
{
	BioseqSetPtr segset, parts = NULL, tp;
	BioseqPtr bsp, segseq= NULL;
	SeqEntryPtr sep, tmp, prev;
	SeqLocPtr slp, curr;
	SeqIntPtr sip;

	if (segmented_seq_entry == NULL) return NULL;
	if (IS_Bioseq(segmented_seq_entry)) return NULL;
	if (segmented_seq_entry->data.ptrvalue == NULL) return NULL;

	segset = (BioseqSetPtr)(segmented_seq_entry->data.ptrvalue);
	if (segset->_class != 2) return NULL;

	prev = NULL;
	for (tmp = segset->seq_set; tmp != NULL; tmp = tmp->next)
	{
		if (IS_Bioseq(tmp))
		{
			bsp = (BioseqPtr)(tmp->data.ptrvalue);
			if (bsp->repr == Seq_repr_seg)
				segseq = bsp;
		}
		else
		{
			tp = (BioseqSetPtr)(tmp->data.ptrvalue);
			if (tp->_class == 4)
				parts = tp;
		}
		prev = tmp;
	}

	if (segseq == NULL)
	{
		Message(MSG_ERROR, "AddSeqToSegmentedEntry: no seg seq found");
		return NULL;
	}

	if (parts == NULL)   /* first member of segset */
	{
		parts = BioseqSetNew();
		parts->_class = 4;
		sep = SeqEntryNew();
		sep->choice = 2;
		sep->data.ptrvalue = (Pointer)parts;
		prev->next = sep;
	}

	prev = NULL;
	for (tmp = parts->seq_set; tmp != NULL; tmp = tmp->next)
	{
		if (! IS_Bioseq(tmp))
		{
			Message(MSG_ERROR, "BioseqSet in Parts");
			return NULL;
		}

		prev = tmp;
	}

	sep = SeqEntryNew();
	if (prev == NULL)      /* first one */
		parts->seq_set = sep;
	else
		prev->next = sep;

	bsp = NCBISubNewBioseq (submission , local_name, genbank_locus,
		genbank_accession, gi_number, molecule_class, molecule_type,
		length, topology, strandedness);
	sep->choice = 1;
	sep->data.ptrvalue = (Pointer)bsp;

								/* add pointer to segmented seq */
	if (segseq->seq_ext != NULL)
	{
		for (curr = (SeqLocPtr)segseq->seq_ext; curr->next != NULL; curr = curr->next)
			continue;
	}
	else
		curr = NULL;

	slp = ValNodeNew(NULL);
	if (curr == NULL)
		segseq->seq_ext = slp;
	else
		curr->next = slp;
	slp->choice = SEQLOC_INT;
	sip = SeqIntNew();
	slp->data.ptrvalue = sip;
	sip->id = SeqIdDup(bsp->id);
	sip->from = 0;
	sip->to = length - 1;

	segseq->length += length;   /* add the segment */

	return sep;
}

Boolean AddGapToSegmentedEntry (
	NCBISubPtr submission,
	SeqEntryPtr segmented_seq_entry,
	Int4 length_of_gap )    /** 0 if not known */
{
	BioseqSetPtr segset, parts = NULL, tp;
	BioseqPtr bsp, segseq= NULL;
	SeqEntryPtr sep, tmp, prev;
	ValNodePtr vnp;
	SeqLocPtr slp, curr;
	SeqIntPtr sip;
	Char local_name[40];
	Int2 mol_type=0;

	if (segmented_seq_entry == NULL) return FALSE;
	if (IS_Bioseq(segmented_seq_entry)) return FALSE;
	if (segmented_seq_entry->data.ptrvalue == NULL) return FALSE;

	segset = (BioseqSetPtr)(segmented_seq_entry->data.ptrvalue);
	if (segset->_class != 2) return FALSE;

	prev = NULL;
	for (tmp = segset->seq_set; tmp != NULL; tmp = tmp->next)
	{
		if (IS_Bioseq(tmp))
		{
			bsp = (BioseqPtr)(tmp->data.ptrvalue);
			if (bsp->repr == Seq_repr_seg)
				segseq = bsp;
		}
		else
		{
			tp = (BioseqSetPtr)(tmp->data.ptrvalue);
			if (tp->_class == 4)
				parts = tp;
		}
		prev = tmp;
	}

	if (segseq == NULL)
	{
		Message(MSG_ERROR, "AddGapToSegmentedEntry: no seg seq found");
		return FALSE;
	}

	if ((parts == NULL) && (length_of_gap <= 0))
	{
		Message(MSG_ERROR, "Do not start or end a segmented set with unknown gaps");
		return FALSE;
	}

	if (length_of_gap > 0)     /* add a virtual sequence */
	{
		if (parts == NULL)   /* first member of segset */
		{
			parts = BioseqSetNew();
			parts->_class = 4;
			sep = SeqEntryNew();
			sep->choice = 2;
			sep->data.ptrvalue = (Pointer)parts;
			prev->next = sep;
		}

		prev = NULL;
		for (tmp = parts->seq_set; tmp != NULL; tmp = tmp->next)
		{
			if (! IS_Bioseq(tmp))
			{
				Message(MSG_ERROR, "BioseqSet in Parts");
				return FALSE;
			}

			prev = tmp;
		}

		sep = SeqEntryNew();
		if (prev == NULL)      /* first one */
			parts->seq_set = sep;
		else
			prev->next = sep;

		for (vnp = segseq->descr; vnp != NULL; vnp = vnp->next)
		{
			if (vnp->choice == Seq_descr_mol_type)
			{
				mol_type = (Int2)(vnp->data.intvalue);
				break;
			}
		}

		submission->gap_count++;
		sprintf(local_name, "Gap_%d", (int)(submission->gap_count));

		bsp = NCBISubNewBioseq (submission , local_name, NULL,
			NULL, 0, mol_type, (Int2)(segseq->mol), length_of_gap,
			(Int2)(segseq->topology), (Int2)(segseq->strand));
		bsp->repr = Seq_repr_virtual;
		sep->choice = 1;
		sep->data.ptrvalue = (Pointer)bsp;
	}
								/* add pointer to segmented seq */
	if (segseq->seq_ext != NULL)
	{
		for (curr = (SeqLocPtr)segseq->seq_ext; curr->next != NULL; curr = curr->next)
			continue;
	}
	else
		curr = NULL;

	slp = ValNodeNew(NULL);
	if (curr == NULL)
		segseq->seq_ext = slp;
	else
		curr->next = slp;
	if (length_of_gap > 0)
	{
		slp->choice = SEQLOC_INT;
		sip = SeqIntNew();
		slp->data.ptrvalue = sip;
		sip->id = SeqIdDup(bsp->id);
		sip->from = 0;
		sip->to = length_of_gap - 1;
	}
	else
	{
		slp->choice = SEQLOC_NULL;
	}

	return TRUE;
}

Boolean AddReferenceToSegmentedEntry (
	NCBISubPtr submission ,
	SeqEntryPtr segmented_seq_entry,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int4 from ,
	Int4 to ,
	Boolean on_plus_strand )
{
	BioseqSetPtr segset;
	BioseqPtr bsp, segseq= NULL;
	SeqEntryPtr tmp, prev;
	SeqLocPtr slp, curr;
	SeqIntPtr sip;
	TextSeqIdPtr tsip;


	if (segmented_seq_entry == NULL) return FALSE;
	if (IS_Bioseq(segmented_seq_entry)) return FALSE;
	if (segmented_seq_entry->data.ptrvalue == NULL) return FALSE;

	segset = (BioseqSetPtr)(segmented_seq_entry->data.ptrvalue);
	if (segset->_class != 2) return FALSE;

	prev = NULL;
	for (tmp = segset->seq_set; tmp != NULL; tmp = tmp->next)
	{
		if (IS_Bioseq(tmp))
		{
			bsp = (BioseqPtr)(tmp->data.ptrvalue);
			if (bsp->repr == Seq_repr_seg)
				segseq = bsp;
		}
		prev = tmp;
	}

	if (segseq == NULL)
	{
		Message(MSG_ERROR, "AddRefToSegmentedEntry: no seg seq found");
		return FALSE;
	}

								/* add pointer to segmented seq */
	if (segseq->seq_ext != NULL)
	{
		for (curr = (SeqLocPtr)segseq->seq_ext; curr->next != NULL; curr = curr->next)
			continue;
	}
	else
		curr = NULL;

	slp = ValNodeNew(NULL);
	if (curr == NULL)
		segseq->seq_ext = slp;
	else
		curr->next = slp;

	slp->choice = SEQLOC_INT;
	sip = SeqIntNew();
	slp->data.ptrvalue = sip;
	sip->from = from;
	sip->to = to;
	if (on_plus_strand)
		sip->strand = Seq_strand_plus;
	else
		sip->strand = Seq_strand_minus;

	curr = ValNodeNew(NULL);
	sip->id = curr;
	if (gi_number > 0)    /* gis are better */
	{
		curr->choice = SEQID_GI;
		curr->data.intvalue = gi_number;
	}
	else
	{
		curr->choice = SEQID_GENBANK;
		tsip = TextSeqIdNew();
		tsip->accession = StringSave(genbank_accession);
		curr->data.ptrvalue = tsip;
	}

	return TRUE;
}

/*****************************************************************************
*
*   Build nuc-prot sets
*
*****************************************************************************/
SeqEntryPtr AddNucProtToSubmission (
	NCBISubPtr submission )
{
	BioseqSetPtr bssp;
	SeqEntryPtr tmp, the_set;

	bssp = BioseqSetNew();
	bssp->_class = 1;       /* nuc-prot */
	the_set = SeqEntryNew();
	the_set->choice = 2;    /* set */
	the_set->data.ptrvalue = bssp;

	if (submission->ssp->data == NULL)
		submission->ssp->data = (Pointer)the_set;
	else
	{
		for (tmp = (SeqEntryPtr) submission->ssp->data; tmp->next != NULL; tmp = tmp->next)
			continue;
		tmp->next = the_set;
	}

	return the_set;
}

SeqEntryPtr AddSeqToNucProtEntry (   /** add unsegmented nuc or prot bioseq */
	NCBISubPtr submission,
	SeqEntryPtr nuc_prot_entry,
	CharPtr local_name ,
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int2 molecule_class,
	Int2 molecule_type ,
	Int4 length ,
	Int2 topology ,
	Int2 strandedness )
{
	BioseqSetPtr nucprot, tp;
	BioseqPtr bsp, segseq= NULL;
	SeqEntryPtr sep, tmp, prev, tmp2;
	Boolean is_nuc;

	if (nuc_prot_entry == NULL) return NULL;
	if (IS_Bioseq(nuc_prot_entry)) return NULL;
	if (nuc_prot_entry->data.ptrvalue == NULL) return NULL;

	nucprot = (BioseqSetPtr)(nuc_prot_entry->data.ptrvalue);
	if (nucprot->_class != 1) return NULL;

	if (ISA_na(molecule_class))
		is_nuc = TRUE;
	else
		is_nuc = FALSE;

	prev = NULL;
	for (tmp = nucprot->seq_set; tmp != NULL; tmp = tmp->next)
	{
		if (is_nuc)   /* check for multiple nucleotides */
		{
			if (IS_Bioseq(tmp))
			{
				bsp = (BioseqPtr)(tmp->data.ptrvalue);
				if (ISA_na(bsp->mol))
				{
					Message(MSG_ERROR, "AddSeqToNucProt: adding more than one nucleotide seq");
					return NULL;
				}
			}
			else
			{
				tp = (BioseqSetPtr)(tmp->data.ptrvalue);
				if (tp->_class == 2)  /* seg-set */
				{
					for (tmp2 = tp->seq_set; tmp2 != NULL; tmp2 = tmp2->next)
					{
						if (IS_Bioseq(tmp2))
						{
							bsp = (BioseqPtr)(tmp2->data.ptrvalue);
							if (ISA_na(bsp->mol))
							{
								Message(MSG_ERROR, "AddSeqToNucProt: adding more than one nucleotide seq");
								return NULL;
							}
						}
					}
				}
			}
		}
		prev = tmp;
	}

	sep = SeqEntryNew();
	if (prev == NULL)      /* first one */
		nucprot->seq_set = sep;
	else
		prev->next = sep;

	bsp = NCBISubNewBioseq (submission , local_name, genbank_locus,
		genbank_accession, gi_number, molecule_class, molecule_type,
		length, topology, strandedness);
	sep->choice = 1;
	sep->data.ptrvalue = (Pointer)bsp;

	return sep;

}
								  /** add segmented nuc or prot bioseq set */

SeqEntryPtr AddSegmentedSeqToNucProtEntry (
	NCBISubPtr submission,
	SeqEntryPtr nuc_prot_entry ,
	CharPtr local_name ,
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number ,
	Int2 molecule_class,
	Int2 molecule_type ,
	Int4 length ,
	Int2 topology ,
	Int2 strandedness )
{
	BioseqSetPtr nucprot, tp;
	BioseqPtr bsp;
	SeqEntryPtr sep, tmp, prev, tmp2, the_set;
	Boolean is_nuc;
	BioseqSetPtr bssp;

	if (nuc_prot_entry == NULL) return NULL;
	if (IS_Bioseq(nuc_prot_entry)) return NULL;
	if (nuc_prot_entry->data.ptrvalue == NULL) return NULL;

	nucprot = (BioseqSetPtr)(nuc_prot_entry->data.ptrvalue);
	if (nucprot->_class != 1) return NULL;

	if (ISA_na(molecule_class))
		is_nuc = TRUE;
	else
		is_nuc = FALSE;

	prev = NULL;
	for (tmp = nucprot->seq_set; tmp != NULL; tmp = tmp->next)
	{
		if (is_nuc)   /* check for multiple nucleotides */
		{
			if (IS_Bioseq(tmp))
			{
				bsp = (BioseqPtr)(tmp->data.ptrvalue);
				if (ISA_na(bsp->mol))
				{
					Message(MSG_ERROR, "AddSeqToNucProt: adding more than one nucleotide seq");
					return NULL;
				}
			}
			else
			{
				tp = (BioseqSetPtr)(tmp->data.ptrvalue);
				if (tp->_class == 2)  /* seg-set */
				{
					for (tmp2 = tp->seq_set; tmp2 != NULL; tmp2 = tmp2->next)
					{
						if (IS_Bioseq(tmp2))
						{
							bsp = (BioseqPtr)(tmp2->data.ptrvalue);
							if (ISA_na(bsp->mol))
							{
								Message(MSG_ERROR, "AddSeqToNucProt: adding more than one nucleotide seq");
								return NULL;
							}
						}
					}
				}
			}
		}
		prev = tmp;
	}

	bssp = BioseqSetNew();
	bssp->_class = 2;       /* segset */
	the_set = SeqEntryNew();
	the_set->choice = 2;    /* set */
	the_set->data.ptrvalue = bssp;
	if (prev == NULL)      /* first one */
		nucprot->seq_set = the_set;
	else
		prev->next = the_set;

	bsp = NCBISubNewBioseq (submission , local_name, genbank_locus,
		genbank_accession, gi_number, molecule_class, molecule_type,
		length, topology, strandedness);
	bsp->repr = Seq_repr_seg;
	bsp->seq_ext_type = 1;

	sep = SeqEntryNew();
	sep->choice = 1;   /* Bioseq */
	sep->data.ptrvalue = bsp;
	bssp->seq_set = sep;

	return the_set;
}

/*****************************************************************************
*
*   Add residues to raw Bioseqs
*
*****************************************************************************/
Boolean AddBasesToBioseq (
	NCBISubPtr submission ,
	SeqEntryPtr the_seq ,
	CharPtr the_bases )
{
	CharPtr tmp;
	BioseqPtr bsp;

	if ((the_seq == NULL) || (the_bases == NULL))
		return FALSE;

	if (! IS_Bioseq(the_seq))
	{
		Message(MSG_ERROR, "Adding bases to a Bioseq-set");
		return FALSE;
	}

	bsp = (BioseqPtr)(the_seq->data.ptrvalue);

	if (! ISA_na(bsp->mol))
	{
		Message(MSG_ERROR, "Adding bases to a protein");
		return FALSE;
	}

	if (bsp->repr != Seq_repr_raw)
	{
		Message(MSG_ERROR, "Adding residues to non-raw Bioseq");
		return FALSE;
	}

	if (bsp->seq_data == NULL)
	{
		bsp->seq_data = BSNew(bsp->length);
		bsp->seq_data_type = Seq_code_iupacna;
	}

	for (tmp = the_bases; *tmp != '\0'; tmp++)
	{
		*tmp = TO_UPPER(*tmp);
		if (*tmp == 'U') *tmp = 'T';
		if ((*tmp >= 'A') && (*tmp <= 'Y'))
			BSPutByte(bsp->seq_data, (Int2)(*tmp));
	}

	return TRUE;
}

Boolean AddAminoAcidsToBioseq (
	NCBISubPtr submission ,
	SeqEntryPtr the_seq ,
	CharPtr the_aas )
{
	CharPtr tmp;
	BioseqPtr bsp;

	if ((the_seq == NULL) || (the_aas == NULL))
		return FALSE;

	if (! IS_Bioseq(the_seq))
	{
		Message(MSG_ERROR, "Adding amino acids to a Bioseq-set");
		return FALSE;
	}

	bsp = (BioseqPtr)(the_seq->data.ptrvalue);

	if (ISA_na(bsp->mol))
	{
		Message(MSG_ERROR, "Adding amino acids to a nucleotide");
		return FALSE;
	}

	if (bsp->repr != Seq_repr_raw)
	{
		Message(MSG_ERROR, "Adding residues to non-raw Bioseq");
		return FALSE;
	}

	if (bsp->seq_data == NULL)
	{
		bsp->seq_data = BSNew(bsp->length);
		bsp->seq_data_type = Seq_code_iupacaa;
	}

	for (tmp = the_aas; *tmp != '\0'; tmp++)
	{
		*tmp = TO_UPPER(*tmp);
		if ((*tmp >= 'A') && (*tmp <= 'Z'))
			BSPutByte(bsp->seq_data, (Int2)(*tmp));
	}

	return TRUE;
}


/*****************************************************************************
*
*   Add Annotations to Entries
*
*****************************************************************************/

Boolean StripNonAsciiChars (CharPtr str)
{
	Boolean retval = TRUE;

	while (*str != '\0')
	{
		if ((*str < ' ') || (*str > '~'))
		{
			retval = FALSE;
			if (*str == '\n')
				*str = '~';
			else
				*str = '#';
		}
		str++;
	}
	return retval;
}

ValNodePtr NewDescrOnSeqEntry (SeqEntryPtr entry, Int2 type)
{
	ValNodePtr vnp;
	BioseqPtr bsp;
	BioseqSetPtr bssp;

	if (entry == NULL)
		return NULL;

	if (IS_Bioseq(entry))
	{
		bsp = (BioseqPtr)(entry->data.ptrvalue);
		vnp = ValNodeAdd(&(bsp->descr));
	}
	else
	{
		bssp = (BioseqSetPtr)(entry->data.ptrvalue);
		vnp = ValNodeAdd(&(bssp->descr));
	}
	if (vnp != NULL)
		vnp->choice = (Uint1)type;
	return vnp;
}

Boolean AddTitleToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	CharPtr title )
{
	ValNodePtr vnp;

	if ((submission == NULL) || (entry == NULL) || (title == NULL))
		return FALSE;

	vnp = NewDescrOnSeqEntry (entry, Seq_descr_title);

	vnp->data.ptrvalue = (Pointer)(StringSave(title));
	StripNonAsciiChars((CharPtr)(vnp->data.ptrvalue));
	return TRUE;
}

Boolean AddCommentToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	CharPtr comment )
{
	ValNodePtr vnp;

	if ((submission == NULL) || (entry == NULL) || (comment == NULL))
		return FALSE;

	vnp = NewDescrOnSeqEntry (entry, Seq_descr_comment);

	vnp->data.ptrvalue = (Pointer)(StringSave(comment));
	StripNonAsciiChars((CharPtr)(vnp->data.ptrvalue));
	return TRUE;
}

Boolean AddOrganismToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	CharPtr scientific_name ,
	CharPtr common_name ,
	CharPtr virus_name ,
	CharPtr strain ,
	CharPtr synonym1,
	CharPtr synonym2,
	CharPtr synonym3)
{
	ValNodePtr vnp;
	OrgRefPtr orp;
	Char buf[80];

	if ((submission == NULL) || (entry == NULL))
		return FALSE;

	vnp = NewDescrOnSeqEntry (entry, Seq_descr_org);

	orp = OrgRefNew();
	vnp->data.ptrvalue = (Pointer)orp;

	orp->taxname = StringSave(scientific_name);
	orp->common = StringSave(common_name);
	if (virus_name != NULL)    /* kludge for old Org-ref */
	{
		orp->taxname = StringSave(virus_name);
		ValNodeCopyStr(&orp->syn, 0, scientific_name);
	}

	if (strain != NULL)
	{
		sprintf(buf, "strain=%s", strain);
		ValNodeCopyStr(&orp->mod, 0, buf);
	}

	ValNodeCopyStr(&orp->syn, 0, synonym1);
	ValNodeCopyStr(&orp->syn, 0, synonym2);
	ValNodeCopyStr(&orp->syn, 0, synonym3);

 	return TRUE;
}

Boolean AddGenBankBlockToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	CharPtr taxonomy ,
	CharPtr division ,
	CharPtr keyword1 ,
	CharPtr keyword2 ,
	CharPtr keyword3 )
{
	ValNodePtr vnp;
	GBBlockPtr gbp;

	if ((submission == NULL) || (entry == NULL))
		return FALSE;

	vnp = NewDescrOnSeqEntry (entry, Seq_descr_genbank);

	gbp = GBBlockNew();
	vnp->data.ptrvalue = (Pointer)gbp;

	gbp->taxonomy = StringSave(taxonomy);
	gbp->div = StringSave(division);

	ValNodeCopyStr(&gbp->keywords, 0, keyword1);
	ValNodeCopyStr(&gbp->keywords, 0, keyword2);
	ValNodeCopyStr(&gbp->keywords, 0, keyword3);

	return TRUE;
}

Boolean AddCreateDateToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	Int2 month ,
	Int2 day ,
	Int2 year )
{
	ValNodePtr vnp;
	DatePtr dp;

	if ((submission == NULL) || (entry == NULL))
		return FALSE;

	vnp = NewDescrOnSeqEntry (entry, Seq_descr_create_date);

	dp = DateNew();
	DateWrite(dp, year, month, day, NULL);
	vnp->data.ptrvalue = (Pointer)dp;

	return TRUE;
}

Boolean AddModifierToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	Int2 modifier )
{
	ValNodePtr vnp, tmp;
	BioseqPtr bsp;
	BioseqSetPtr bssp;

	if ((submission == NULL) || (entry == NULL))
		return FALSE;

	if (IS_Bioseq(entry))
	{
		bsp = (BioseqPtr)(entry->data.ptrvalue);
		tmp = bsp->descr;
	}
	else
	{
		bssp = (BioseqSetPtr)(entry->data.ptrvalue);
		tmp = bssp->descr;
	}

	vnp = NULL;
	while (tmp != NULL)
	{
		if (tmp->choice == Seq_descr_modif)  /* have one already */
		{
			vnp = ValNodeNew((ValNodePtr)(tmp->data.ptrvalue));
			break;
		}
		tmp = tmp->next;
	}

	if (vnp == NULL)   /* first one */
	{
		tmp = NewDescrOnSeqEntry (entry, Seq_descr_modif);
		vnp = ValNodeNew((ValNodePtr)(tmp->data.ptrvalue));
		tmp->data.ptrvalue = (Pointer)vnp;
	}

	vnp->data.intvalue = (Int4)modifier;

	return TRUE;
}

Boolean AddPubToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	PubPtr pub )
{
	ValNodePtr vnp;
	PubdescPtr pdp;

	if ((submission == NULL) || (entry == NULL) || (pub == NULL))
		return FALSE;

	vnp = NewDescrOnSeqEntry (entry, Seq_descr_pub);
	pdp = PubdescNew();
	if (pub->choice == PUB_Equiv)   /* already a Pub-equiv */
	{
		pdp->pub = (ValNodePtr)(pub->data.ptrvalue);
		MemFree(pub);
	}
	else                     /* make a Pub-equiv of one member */
		pdp->pub = pub;
	vnp->data.ptrvalue = (Pointer)pdp;

	return TRUE;
}

PubPtr CitSubBuild (
	NCBISubPtr submission,
	Int2 month,
	Int2 day,
	Int2 year ,
	Int2 medium)
{
	ValNodePtr vnp;
	CitSubPtr csp;
	DatePtr dp;
	ImprintPtr ip;

	vnp = ValNodeNew(NULL);
	vnp->choice = PUB_Sub;   /* Cit-sub */
	csp = CitSubNew();
	vnp->data.ptrvalue = (Pointer)csp;

	csp->medium = (Uint1)medium;
	ip = ImprintNew();
	csp->imp = ip;
	dp = DateNew();
	ip->date = dp;
	DateWrite(dp, year, month, day, NULL);
	return vnp;
}

PubPtr CitArtBuild (
	NCBISubPtr submission,
	CharPtr title ,
	CharPtr journal ,
	CharPtr volume ,
	CharPtr issue ,
	CharPtr pages ,
	Int2 month ,
	Int2 day ,
	Int2 year ,
	Int2 status )
{
	ValNodePtr vnp, tmp;
	CitArtPtr cap;
	CitJourPtr cjp;
	CitGenPtr cgp;
	DatePtr dp;
	ImprintPtr ip;

	vnp = ValNodeNew(NULL);

	if (status == PUB_STATUS_UNPUBLISHED)   /* can't do a Cit-art */
	{
		vnp->choice = PUB_Gen;   /* Cit-gen */
		cgp = CitGenNew();
		vnp->data.ptrvalue = cgp;
		cgp->cit = StringSave("Unpublished");
		cgp->title = StringSave(title);
		tmp = ValNodeCopyStr (&cgp->journal, Cit_title_jta, journal);
		cgp->volume = StringSave(volume);
		cgp->issue = StringSave(issue);
		cgp->pages = StringSave(pages);
		if (year > 0)
		{
			dp = DateNew();
			cgp->date = dp;
			DateWrite(dp, year, month, day, NULL);
		}
	}
	else                                  /* regular pub */
	{
		vnp->choice = PUB_Article;   /* Cit-art */
		cap = CitArtNew();
		vnp->data.ptrvalue = cap;
		cap->from = 1;     /* from journal only */
		tmp = ValNodeCopyStr (&cap->title, Cit_title_name, title);
		cjp = CitJourNew();
		cap->fromptr = (Pointer)cjp;
		tmp = ValNodeCopyStr (&cjp->title, Cit_title_jta, journal);
		ip = ImprintNew();
		cjp->imp = ip;
		dp = DateNew();
		ip->date = dp;
		DateWrite(dp, year, month, day, NULL);
		ip->volume = StringSave(volume);
		ip->issue = StringSave(issue);
		ip->pages = StringSave(pages);
		if (status == PUB_STATUS_SUBMITTED)
			ip->prepub = (Uint1)1;
		else if (status == PUB_STATUS_IN_PRESS)
			ip->prepub = (Uint1)2;
	}
	return vnp;
}

Boolean AddAuthorToPub (    /* call once for each author, in order */
	NCBISubPtr submission,
	PubPtr the_pub,
	CharPtr last_name,
	CharPtr first_name,
	CharPtr middle_name,
	CharPtr initials,  /* separated by periods, no initial for last name */
	CharPtr suffix )   /* Jr. Sr. III */
{
	CitGenPtr cgp;
	CitSubPtr csp;
	CitArtPtr cap;
	AuthListPtr alp;
	AuthorPtr ap;
	PersonIdPtr pip;
	NameStdPtr nsp;
	ValNodePtr vnp;

	if ((submission == NULL) || (the_pub == NULL) || (last_name == NULL))
		return FALSE;

	switch (the_pub->choice)
	{
		case PUB_Sub:
			csp = (CitSubPtr)(the_pub->data.ptrvalue);
			if (csp->authors == NULL)
			{
				csp->authors = AuthListNew();
				csp->authors->choice = 1;   /* std */
			}
			alp = csp->authors;
			break;
		case PUB_Gen:
			cgp = (CitGenPtr)(the_pub->data.ptrvalue);
			if (cgp->authors == NULL)
			{
				cgp->authors = AuthListNew();
				cgp->authors->choice = 1;   /* std */
			}
			alp = cgp->authors;
			break;
		case PUB_Article:
			cap = (CitArtPtr)(the_pub->data.ptrvalue);
			if (cap->authors == NULL)
			{
				cap->authors = AuthListNew();
				cap->authors->choice = 1;   /* std */
			}
			alp = cap->authors;
			break;
		default:
			Message(MSG_ERROR, "AddAuthorToPub: Unsupported Pub type [%d]",
				(int)the_pub->choice);
			return FALSE;
	}

	nsp = NameStdNew();
	nsp->names[0] = StringSave(last_name);
	nsp->names[1] = StringSave(first_name);
	nsp->names[2] = StringSave(middle_name);
	nsp->names[4] = StringSave(initials);
	nsp->names[5] = StringSave(suffix);

	pip = PersonIdNew();
	pip->choice = 2;   /* name */
	pip->data = (Pointer)nsp;

	ap = AuthorNew();
	ap->name = pip;

	vnp = ValNodeAdd(&alp->names);
	vnp->data.ptrvalue = (Pointer)ap;

	return TRUE;
}

Boolean AddAffiliationToPub (  /* call once per pub */
	NCBISubPtr submission,
	PubPtr the_pub,
	CharPtr affil,        /* e.g. "Xyz University" */
	CharPtr div,          /* e.g. "Dept of Biology" */
	CharPtr street,       /* e.g. "123 Academic Road" */
	CharPtr city,         /* e.g. "Metropolis" */
	CharPtr sub,          /* e.g. "Massachusetts" */
	CharPtr country )     /* e.g. "USA" */
{
	CitGenPtr cgp;
	CitSubPtr csp;
	CitArtPtr cap;
	AuthListPtr alp;
	AffilPtr ap;

	if ((submission == NULL) || (the_pub == NULL))
		return FALSE;

	switch (the_pub->choice)
	{
		case PUB_Sub:
			csp = (CitSubPtr)(the_pub->data.ptrvalue);
			if (csp->authors == NULL)
			{
				csp->authors = AuthListNew();
				csp->authors->choice = 1;   /* std */
			}
			alp = csp->authors;
			break;
		case PUB_Gen:
			cgp = (CitGenPtr)(the_pub->data.ptrvalue);
			if (cgp->authors == NULL)
			{
				cgp->authors = AuthListNew();
				cgp->authors->choice = 1;   /* std */
			}
			alp = cgp->authors;
			break;
		case PUB_Article:
			cap = (CitArtPtr)(the_pub->data.ptrvalue);
			if (cap->authors == NULL)
			{
				cap->authors = AuthListNew();
				cap->authors->choice = 1;   /* std */
			}
			alp = cap->authors;
			break;
		default:
			Message(MSG_ERROR, "AddAffilToPub: Unsupported Pub type [%d]",
				(int)the_pub->choice);
			return FALSE;
	}

	if (alp->affil != NULL)
	{
		Message(MSG_ERROR, "AddAffilToPub: Pub already has affil");
		return FALSE;
	}

	ap = AffilNew();
	ap->choice = 2;    /* std affil */
	ap->affil = StringSave(affil);
	ap->div = StringSave(div);
	ap->city = StringSave(city);
	ap->sub = StringSave(sub);
	ap->country = StringSave(country);
	ap->street = StringSave(street);

	return TRUE;
}


/*****************************************************************************
*
*   Add Features to the entry
*
*****************************************************************************/
Boolean AddFeatureToEntry (
	NCBISubPtr submission,
	SeqEntryPtr entry ,
	SeqFeatPtr feature )
{
	SeqFeatPtr sfp;
	SeqAnnotPtr sap;
	BioseqPtr bsp;
	BioseqSetPtr bssp;

	if ((submission == NULL) || (entry == NULL) || (feature == NULL))
		return FALSE;

	if (IS_Bioseq(entry))
	{
		bsp = (BioseqPtr)(entry->data.ptrvalue);
		for (sap = bsp->annot; sap != NULL; sap = sap->next)
		{
			if (sap->type == 1)   /* feature table */
				break;
		}
		if (sap == NULL)
		{
			sap = SeqAnnotNew();
			sap->type = 1;
			bsp->annot = sap;
		}
	}
	else
	{
		bssp = (BioseqSetPtr)(entry->data.ptrvalue);
		for (sap = bssp->annot; sap != NULL; sap = sap->next)
		{
			if (sap->type == 1)   /* feature table */
				break;
		}
		if (sap == NULL)
		{
			sap = SeqAnnotNew();
			sap->type = 1;
			bssp->annot = sap;
		}
	}

	sfp = (SeqFeatPtr)(sap->data);
	if (sfp == NULL)
		sap->data = (Pointer)feature;
	else
	{
		while (sfp->next != NULL)
			sfp = sfp->next;
		sfp->next = feature;
	}

	return TRUE;
}

SeqFeatPtr FeatureBuild (
	NCBISubPtr submission,
	SeqEntryPtr entry_to_put_feature,
	Boolean feature_is_partial,
	Uint1 evidence_is_experimental,
	Boolean biological_exception,
	CharPtr comment )
{
	SeqFeatPtr sfp;
	static char * ev [2] = {
		"experimental", "not_experimental" };

	if (entry_to_put_feature == NULL) return NULL;

	sfp = SeqFeatNew();

	sfp->partial = feature_is_partial;
	if ((evidence_is_experimental == EVIDENCE_EXPERIMENTAL)
		|| (evidence_is_experimental == EVIDENCE_NOT_EXPERIMENTAL))
	{
		sfp->exp_ev = evidence_is_experimental;
		AddQualToImpFeature(submission, sfp, "evidence",
			ev[evidence_is_experimental - 1]);
	}
	sfp->excpt = biological_exception;
	if (comment != NULL)
	{
		if (*comment != '\0')
			sfp->comment = StringSave(comment);
	}

	AddFeatureToEntry(submission, entry_to_put_feature, sfp);

	return sfp;
}


Boolean AddIntervalToFeature (
	NCBISubPtr submission,
	SeqFeatPtr sfp,
	SeqEntryPtr the_seq ,
	CharPtr local_name ,
	Int4 from ,
	Int4 to ,
	Boolean on_plus_strand ,
	Boolean start_before_from ,
	Boolean stop_after_to )
{
	BioseqPtr bsp;
	Int4 tmp_from, tmp_to;
	Int2 tmp_fuzz_from, tmp_fuzz_to, fuzz_from, fuzz_to, strand;


	if (sfp == NULL) return FALSE;

	bsp = GetBioseqFromChoice(the_seq, local_name, "AddIntervalToFeature");
	if (bsp == NULL) return FALSE;

	if (to == -1)
		to = BioseqGetLen(bsp);

	    /** allow to input numbering from 1 */

	to -= 1;
	from -= 1;
	fuzz_from = -1;     /* not-set */
	fuzz_to = -1;
	if (! on_plus_strand)
	{
		strand = Seq_strand_minus;
		tmp_from = from;
		from = to;
		to = tmp_from;
		if (start_before_from)
			fuzz_to = 1;    /* gt */
		if (stop_after_to)
			fuzz_from = 2;  /* lt */
	}
	else
	{
		strand = 0;
		if (start_before_from)
			fuzz_from = 2;    /* lt */
		if (stop_after_to)
			fuzz_to = 2;  /* gt */

	}

	if (to < from)    /* go around origin on circular sequence */
	{
		if ((bsp->topology != 2) && (bsp->topology != 3))
		{
			Message(MSG_ERROR, "Attempt to go around origin of non-circular sequence");
			return FALSE;
		}
		tmp_fuzz_from = -1;
		tmp_fuzz_to = -1;

		if (on_plus_strand)
		{
			tmp_from = from;
			tmp_to = (BioseqGetLen(bsp) - 1);
			from = 0;
			tmp_fuzz_from = fuzz_from;
			fuzz_from = -1;
		}
		else
		{
			tmp_from = 0;
			tmp_to = to;
			to = (BioseqGetLen(bsp) - 1);
			tmp_fuzz_to = fuzz_to;
			fuzz_to = -1;
		}

		if (! AddIntToSeqFeat(sfp, tmp_from, tmp_to, bsp, tmp_fuzz_from, tmp_fuzz_to, strand))
			return FALSE;
	}

	return AddIntToSeqFeat(sfp, from, to, bsp, fuzz_from, fuzz_to, strand);
}

Boolean AddIntToSeqFeat (SeqFeatPtr sfp, Int4 from, Int4 to, BioseqPtr bsp,
							Int2 fuzz_from, Int2 fuzz_to, Int2 strand)
{
	SeqLocPtr slp, tmp, tmp2;
	SeqIntPtr sintp;
	IntFuzzPtr ifp;

	sintp = SeqIntNew();
	sintp->from = from;
	sintp->to = to;
	sintp->id = SeqIdDup(SeqIdFindBest(bsp->id, 0));
	sintp->strand = (Uint1)strand;
	if (fuzz_from >= 0)
	{
		ifp = IntFuzzNew();
		ifp->choice = 4;   /* lim */
		ifp->a = (Int4)fuzz_from;
		sintp->if_from = ifp;
	}
	if (fuzz_to >= 0)
	{
		ifp = IntFuzzNew();
		ifp->choice = 4;   /* lim */
		ifp->a = (Int4)fuzz_to;
		sintp->if_to = ifp;
	}

	slp = ValNodeNew(NULL);
	slp->choice = SEQLOC_INT;
	slp->data.ptrvalue = (Pointer)sintp;

	if (sfp->location == NULL)
	{
		sfp->location = slp;
		return TRUE;
	}

	tmp = sfp->location;
	if (tmp->choice == SEQLOC_MIX)   /* second one already */
	{
		tmp2 = (ValNodePtr)(tmp->data.ptrvalue);
		while (tmp2->next != NULL)
			tmp2 = tmp2->next;
		tmp2->next = slp;
	}
	else                             /* create a chain */
	{
		tmp2 = ValNodeNew(NULL);
		tmp2->choice = SEQLOC_MIX;
		tmp2->data.ptrvalue = (Pointer)tmp;
		tmp->next = slp;
		sfp->location = tmp2;
	}

	return TRUE;
}

BioseqPtr GetBioseqFromChoice (SeqEntryPtr the_seq, CharPtr local_name, CharPtr the_routine)
{
	BioseqPtr bsp;
	ValNode vn;
	ObjectId oid;

	if (the_seq != NULL)
	{
		if (! IS_Bioseq(the_seq))
		{
			Message(MSG_ERROR, "%s: Gave Seq-entry which is not a Bioseq",
				the_routine);
			return NULL;
		}
		bsp = (BioseqPtr)(the_seq->data.ptrvalue);
	}
	else if (local_name != NULL)
	{
		vn.next = NULL;
		vn.choice = SEQID_LOCAL;
		vn.data.ptrvalue = (Pointer)(& oid);
		oid.str = local_name;

		bsp = BioseqFind((SeqIdPtr)(& vn));
		if (bsp == NULL)
		{
			Message(MSG_ERROR, "%s: Can't find Bioseq [%s]",
				the_routine, local_name);
			return NULL;
		}

	}
	else
	{
		Message(MSG_ERROR, "%s: No the_seq or local_name given",
			the_routine);
		return NULL;
	}

	return bsp;
}

Boolean AddPointToFeature (
	NCBISubPtr submission,
	SeqFeatPtr sfp,
	SeqEntryPtr the_seq ,
	CharPtr local_name ,
	Int4 location ,
	Boolean on_plus_strand ,
	Boolean is_after_location ,
	Boolean is_before_location )
{
	BioseqPtr bsp;
	SeqLocPtr slp, tmp, tmp2;
	SeqPntPtr spp;
	IntFuzzPtr ifp;


	if (sfp == NULL) return FALSE;

	bsp = GetBioseqFromChoice(the_seq, local_name, "AddPointToFeature");
	if (bsp == NULL) return FALSE;

	spp = SeqPntNew();
	if (location == -1)
		location = BioseqGetLen(bsp);
	spp->point = location - 1;
	
	spp->id = SeqIdDup(SeqIdFindBest(bsp->id, 0));
	if (! on_plus_strand)
		spp->strand = Seq_strand_minus;
	if (is_before_location)
	{
		ifp = IntFuzzNew();
		ifp->choice = 4;   /* lim */
		ifp->a = 2;        /* lt */
		spp->fuzz = ifp;
	}
	else if (is_after_location)
	{
		ifp = IntFuzzNew();
		ifp->choice = 4;   /* lim */
		ifp->a = 1;        /* gt */
		spp->fuzz = ifp;
	}

	slp = ValNodeNew(NULL);
	slp->choice = SEQLOC_PNT;
	slp->data.ptrvalue = (Pointer)spp;

	if (sfp->location == NULL)
	{
		sfp->location = slp;
		return TRUE;
	}

	tmp = sfp->location;
	if (tmp->choice == SEQLOC_MIX)   /* second one already */
	{
		tmp2 = (ValNodePtr)(tmp->data.ptrvalue);
		while (tmp2->next != NULL)
			tmp2 = tmp2->next;
		tmp2->next = slp;
	}
	else                             /* create a chain */
	{
		tmp2 = ValNodeNew(NULL);
		tmp2->choice = SEQLOC_MIX;
		tmp2->data.ptrvalue = (Pointer)tmp;
		tmp->next = slp;
		sfp->location = tmp2;
	}
	return TRUE;
}


Boolean MakeCdRegionFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature,
	Int2 frame ,
	Int2 genetic_code ,			
	SeqEntryPtr protein_product , 	 /* give id of protein. if NULL, call */
	CharPtr local_id_for_protein )  /* function below to create by transl */
{
	BioseqPtr bsp;
	CdRegionPtr crp;
	ValNodePtr vnp;
	Char buf[40];

	if (feature == NULL) return FALSE;

	if (feature->data.choice)
	{
		Message(MSG_ERROR, "MakeCdRegionFeature: feature already has data");
		return FALSE;
	}

	crp = CdRegionNew();
	crp->frame = (Uint1)frame;
	if (genetic_code)
	{
		crp->genetic_code = GeneticCodeNew();
		vnp = ValNodeAdd ((ValNodePtr PNTR)&(crp->genetic_code->data.ptrvalue));
		vnp->choice = 2;
		vnp->data.intvalue = (Int4)genetic_code;
	}

	feature->data.choice = 3;   /* cdregion */
	feature->data.value.ptrvalue = (Pointer)crp;

	if ((protein_product != NULL) || (local_id_for_protein != NULL))
	{
		bsp = GetBioseqFromChoice (protein_product, local_id_for_protein,
			"MakeCdRegionFeature");
		if (bsp != NULL)
		{
			if (! ISA_aa(bsp->mol))
			{
				SeqIdPrint(bsp->id, buf, PRINTID_FASTA_LONG);
				Message(MSG_ERROR, "Using non-protein [%s] for cdregion product",
					buf);
			}
			else
			{
				vnp = ValNodeNew(NULL);
				feature->product = vnp;
				vnp->choice = SEQLOC_WHOLE;
				vnp->data.ptrvalue = SeqIdDup(SeqIdFindBest(bsp->id, 0));
			}
		}
	}

	return TRUE;
}
           
		   /******************************************************************
		   *
		   *   Special function to make protein from CdRegion feature
		   *
		   ******************************************************************/

SeqEntryPtr TranslateCdRegion (
	NCBISubPtr submission ,
	SeqFeatPtr cdregion_feature ,
	SeqEntryPtr nuc_prot_entry_to_put_sequence ,
	CharPtr local_name ,             /* for protein sequence */
	CharPtr genbank_locus ,
	CharPtr genbank_accession ,
	Int4 gi_number )
{
	SeqEntryPtr sep;
	BioseqPtr bsp;
	ByteStorePtr bp;
	Int4 protlen;
	ValNodePtr vnp;

	if ((cdregion_feature == NULL) || (nuc_prot_entry_to_put_sequence == NULL))
		return NULL;

	bp = ProteinFromCdRegion (cdregion_feature, FALSE);
	if (bp == NULL)
	{
		Message(MSG_ERROR, "TranslateCdRegion: Couldn't translate CdRegion");
		return NULL;
	}

	protlen = BSLen(bp);

	sep = AddSeqToNucProtEntry (submission , nuc_prot_entry_to_put_sequence,
		local_name, genbank_locus,
		genbank_accession, gi_number, MOLECULE_CLASS_PROTEIN,
		MOLECULE_TYPE_PEPTIDE ,	protlen, 0, 0);

	bsp = (BioseqPtr)(sep->data.ptrvalue);

	bsp->seq_data = bp;
	bsp->seq_data_type = Seq_code_iupacaa;

	vnp = ValNodeNew(NULL);
	cdregion_feature->product = vnp;
	vnp->choice = SEQLOC_WHOLE;
	vnp->data.ptrvalue = SeqIdDup(SeqIdFindBest(bsp->id, 0));

	return sep;
}


Boolean MakeRNAFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature,
	Int2 rna_type ,
	Boolean is_pseudo_gene,
	CharPtr rna_name ,
	CharPtr AA_for_tRNA ,
	CharPtr codon_for_tRNA )
{
	RnaRefPtr rrp;
	tRNAPtr trp;
	Int2 i, index;
	CharPtr tmp;

	if (feature == NULL) return FALSE;

	if (feature->data.choice)
	{
		Message(MSG_ERROR, "MakeRNAFeature: feature already has data");
		return FALSE;
	}

	rrp = RnaRefNew();
	feature->data.choice = 5;
	feature->data.value.ptrvalue = (Pointer)rrp;

	rrp->type = (Uint1) rna_type;
	rrp->pseudo = is_pseudo_gene;
	if (rna_name != NULL)
	{
		rrp->ext.choice = 1;
		rrp->ext.value.ptrvalue = (Pointer)StringSave(rna_name);
	}
	else if (rna_type == RNA_TYPE_tRNA)
	{
		if ((AA_for_tRNA != NULL ) || (codon_for_tRNA != NULL))
		{
			trp = MemNew(sizeof(tRNA));
			rrp->ext.choice = 2;
			rrp->ext.value.ptrvalue = (Pointer)trp;
			for (i = 0; i < 6; i++)
				trp->codon[i] = 255;
			if (AA_for_tRNA != '\0')
			{
				trp->aatype = 1;    /* iupacaa */
				trp->aa = (Uint1)TO_UPPER(*AA_for_tRNA);
			}
			if (codon_for_tRNA != NULL)
			{
				tmp = codon_for_tRNA;
				index = 0;
				for (i = 16; i > 0; i /= 4, tmp++)
				{
					switch (TO_UPPER(*tmp))
					{
						case 'T':
							break;    /* 0 */
						case 'C':
							index += (i * 1);
							break;
						case 'A':
							index += (i * 2);
							break;
						case 'G':
							index += (i * 3);
							break;
						default:
							Message(MSG_ERROR, "Invalid tRNA codon [%s]",
								codon_for_tRNA);
							return TRUE;
					}
				}
				trp->codon[0] = (Uint1)index;
			}
		}
	}

	return TRUE;
}

Boolean MakeGeneFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature,
	CharPtr gene_symbol_for_locus ,
	CharPtr allele ,
	CharPtr descriptive_name ,
	CharPtr map_location ,
	Boolean is_pseudo_gene ,
	CharPtr genetic_database ,
	CharPtr gene_id_in_genetic_database ,
	CharPtr synonym1 ,
	CharPtr synonym2 ,
	CharPtr synonym3 )
{
	GeneRefPtr grp;
	ValNodePtr vnp;
	DbtagPtr dbt;
	ObjectIdPtr oip;

	if (feature == NULL) return FALSE;

	if (feature->data.choice)
	{
		Message(MSG_ERROR, "MakeGeneFeature: feature already has data");
		return FALSE;
	}

	grp = GeneRefNew();
	feature->data.choice = 1;
	feature->data.value.ptrvalue = grp;

	grp->locus = StringSave(gene_symbol_for_locus);
	grp->allele = StringSave(allele);
	grp->desc = StringSave(descriptive_name);
	grp->maploc = StringSave(map_location);
	grp->pseudo = is_pseudo_gene;
	if ((genetic_database != NULL) && (gene_id_in_genetic_database != NULL))
	{
		vnp = ValNodeAdd(&grp->db);
		dbt = DbtagNew();
		vnp->data.ptrvalue = dbt;
		dbt->db = StringSave(genetic_database);
		oip = ObjectIdNew();
		dbt->tag = oip;
		oip->str = StringSave(gene_id_in_genetic_database);
	}

	ValNodeCopyStr(&grp->syn, 0, synonym1);
	ValNodeCopyStr(&grp->syn, 0, synonym2);
	ValNodeCopyStr(&grp->syn, 0, synonym3);

	return TRUE;
}

Boolean MakeProteinFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature ,
	CharPtr protein_name1,
	CharPtr protein_name2,
	CharPtr protein_name3,
	CharPtr descriptive_name,
	CharPtr ECnum1,
	CharPtr ECnum2,
	CharPtr activity1,
	CharPtr activity2,
	CharPtr protein_database,
	CharPtr id_in_protein_database)
{
	ProtRefPtr prp;
	ValNodePtr vnp;
	DbtagPtr dbt;
	ObjectIdPtr oip;
	CharPtr tmp;
	Int2 i;

	if (feature == NULL) return FALSE;

	if (feature->data.choice)
	{
		Message(MSG_ERROR, "MakeProteinFeature: feature already has data");
		return FALSE;
	}

	prp = ProtRefNew();
	feature->data.choice = 4;
	feature->data.value.ptrvalue = prp;

	ValNodeCopyStr(&prp->name, 0, protein_name1);
	ValNodeCopyStr(&prp->name, 0, protein_name2);
	ValNodeCopyStr(&prp->name, 0, protein_name3);

	prp->desc = StringSave(descriptive_name);

	tmp = ECnum1;
	for (i = 0; i < 2; i++)
	{
		if (tmp != NULL)
		{					 /* skip any leading E.C. type stuff */
		 	while ((! IS_DIGIT(*tmp)))
			{
				if (*tmp == '\0') break;
				tmp++;
			}
			if (*tmp != '\0')
				ValNodeCopyStr(&prp->ec, 0, tmp);
		}
		tmp = ECnum2;
	}

	ValNodeCopyStr(&prp->activity, 0, activity1);
	ValNodeCopyStr(&prp->activity, 0, activity2);

	if ((protein_database != NULL) && (id_in_protein_database != NULL))
	{
		vnp = ValNodeAdd(&prp->db);
		dbt = DbtagNew();
		vnp->data.ptrvalue = dbt;
		dbt->db = StringSave(protein_database);
		oip = ObjectIdNew();
		dbt->tag = oip;
		oip->str = StringSave(id_in_protein_database);
	}

	return TRUE;
}

Boolean MakeRegionFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature ,
	CharPtr region_name )
{
	if (feature == NULL) return FALSE;

	if (feature->data.choice)
	{
		Message(MSG_ERROR, "MakeRegionFeature: feature already has data");
		return FALSE;
	}

	feature->data.choice = 9;
	feature->data.value.ptrvalue = (Pointer)StringSave(region_name);
	return TRUE;
}

Boolean MakeSiteFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature ,
	Int2 site_type )
{
	if (feature == NULL) return FALSE;

	if (feature->data.choice)
	{
		Message(MSG_ERROR, "MakeSiteFeature: feature already has data");
		return FALSE;
	}

	feature->data.choice = 12;
	feature->data.value.intvalue = (Int4)site_type;
	return TRUE;
}

Boolean MakeImpFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature ,
	CharPtr key )
{
	ImpFeatPtr ifp;

	if (feature == NULL) return FALSE;

	if (feature->data.choice)
	{
		Message(MSG_ERROR, "MakeImpFeature: feature already has data");
		return FALSE;
	}

	ifp = ImpFeatNew();
	ifp->key = StringSave(key);

	feature->data.choice = 8;
	feature->data.value.ptrvalue = (Pointer)ifp;
	return TRUE;
}

Boolean AddQualToImpFeature (
	NCBISubPtr submission,
	SeqFeatPtr imp_feature ,
	CharPtr qualifier ,
	CharPtr value )
{
	GBQualPtr gbp, tmp;

	if ((imp_feature == NULL) || (qualifier == NULL))
		return FALSE;

	gbp = GBQualNew();
	gbp->qual = StringSave(qualifier);
	gbp->val = StringSave(value);

	if (imp_feature->qual == NULL)
		imp_feature->qual = gbp;
	else
	{
		for (tmp = imp_feature->qual; tmp->next != NULL; tmp = tmp->next)
			continue;
		tmp->next = gbp;
	}

	return TRUE;
}

Boolean MakePubFeature (
	NCBISubPtr submission,
	SeqFeatPtr feature,
	PubPtr pub )
{

	PubdescPtr pdp;

	if (feature == NULL) return FALSE;

	if (feature->data.choice)
	{
		Message(MSG_ERROR, "MakePubFeature: feature already has data");
		return FALSE;
	}

	feature->data.choice = 6;
	pdp = PubdescNew();
	if (pub->choice == PUB_Equiv)   /* already a Pub-equiv */
	{
		pdp->pub = (ValNodePtr)(pub->data.ptrvalue);
		MemFree(pub);
	}
	else                     /* make a Pub-equiv of one member */
		pdp->pub = pub;
	feature->data.value.ptrvalue = (Pointer)pdp;

	return TRUE;
}

/*****************************************************************************
*
*   NCBISubValidate (nsp, errfile)
*   	Validate a submission
*
*****************************************************************************/
Int2 NCBISubValidate (NCBISubPtr nsp, FILE * errfile)
{
	Int2 numerrors = 0, i;
	ValidStructPtr vsp;
	SeqEntryPtr sep;

	vsp = ValidStructNew();
	vsp->errfile = errfile;
	for (sep = (SeqEntryPtr)(nsp->ssp->data); sep != NULL; sep = sep->next)
	{
	 	ValidateSeqEntry(sep, vsp);
		for (i = 0; i <=3; i++)
		{
			numerrors += vsp->errors[i];
		}
		ValidStructClear(vsp);
	}
	ValidStructFree(vsp);
	return numerrors;
}

