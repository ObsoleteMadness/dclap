#include "valid.h"


/*****************************************************************************
*
*   NOTE: look at all the ValidErr calls with severity=0. Some should be
*   bumped up later. Look also for string "PARSER"
*
*****************************************************************************/



#ifdef VAR_ARGS
#include <varargs.h>
#else
#include <stdarg.h>
#endif

static ValidStructPtr globalvsp;  /* for spell checker */

void ValidateAllElements (SeqEntryPtr sep, ValidStructPtr vsp, Int2 indent);
void CDECL  ValidErr VPROTO((ValidStructPtr vsp, int severity, char * errcode, const char *fmt, ...));
Boolean ValidateBioseq (BioseqPtr bsp, ValidStructPtr vsp);
Boolean CheckDescrOkForBioseq(BioseqPtr bsp, ValidStructPtr vsp);
void CdTransCheck(ValidStructPtr vsp, SeqFeatPtr sfp);
void ValidateFeatureTable (SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent);
Boolean ValidateSeqFeat( ValidStructPtr vsp, SeqFeatPtr sfp);
Boolean ValidateSeqLoc(ValidStructPtr vsp, SeqLocPtr slp);
Boolean PatchBadSequence(BioseqPtr bsp);
CharPtr FindIDForEntry (SeqEntryPtr sep, CharPtr buf);
void SpellCheckDescr(ValidStructPtr vsp, ValNodePtr vnp);
void SpellCheckSeqFeat(ValidStructPtr vsp, SeqFeatPtr sfp);
void SpellCheckString (ValidStructPtr vsp, CharPtr str);

void FindCd (SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent);

/*****************************************************************************
*
*   Perform Validation Checks on a SeqEntry
*
*****************************************************************************/

void ValidStructClear (ValidStructPtr vsp)  /* 0 out a ValidStruct */
{
	CharPtr errbuf;
	FILE * errfile;
	Int2 cutoff;
	Boolean patch_seq;
	SpellCheckFunc spellfunc;
	SpellCallBackFunc spellcallback;

	if (vsp == NULL) return;

	errbuf = vsp->errbuf;
	errfile = vsp->errfile;
	cutoff = vsp->cutoff;
	patch_seq = vsp->patch_seq;
	spellfunc = vsp->spellfunc;
	spellcallback = vsp->spellcallback;
	MemSet((VoidPtr)vsp, 0, sizeof(ValidStruct));
	vsp->errbuf = errbuf;
	vsp->errfile = errfile;
	vsp->cutoff = cutoff;
	vsp->patch_seq = patch_seq;
	vsp->spellfunc = spellfunc;
	vsp->spellcallback = spellcallback;
	return;
}

ValidStructPtr ValidStructNew (void)
{
	ValidStructPtr vsp;

	vsp = (ValidStructPtr)MemNew(sizeof(ValidStruct));
	return vsp;
}

ValidStructPtr ValidStructFree (ValidStructPtr vsp)
{
	if (vsp == NULL) return vsp;

	MemFree(vsp->errbuf);
	return (ValidStructPtr)MemFree(vsp);
}

/*****************************************************************************
*
*   ValidErr()
*
*****************************************************************************/

#ifdef VAR_ARGS
void CDECL  ValidErr (vsp, severity, errcode, fmt, va_alist)
  ValidStructPtr vsp;
  int severity;
  char * errcode;
  const char *fmt;
  va_dcl
#else
void CDECL  ValidErr (ValidStructPtr vsp, int severity, char * errcode, const char *fmt, ...)
#endif
{
    va_list  args;
	CharPtr tmp, ctmp;
	Char idbuf[40];
	Int2 i;
	static CharPtr level[4] = {
		"INFO |",
		"WARN |",
		"ERROR|",
 		"FATAL|" };
	static char * types[] = {
		"???",
		"Gene-ref",
		"Org-ref",
		"CdRegion",
		"Prot-ref",
		"RNA-ref",
		"Pubdesc",
		"SeqXref",
		"Imp-feat",
		"Region",
		"Comment",
		"Bond",
		"Site",
		"Rsite-ref",
		"UserObject",
		"TxInit",
		"Numbering",
		"SecondaryStructure",
		"NonStdResidue",
		"Heterogen" };

	BioseqPtr bsp;
	SeqIdPtr sip;
	ValNode vn;

	if (severity < vsp->cutoff) return;

	if ((vsp->errbuf == NULL) && (vsp->errfile == NULL))
	{
		vsp->errbuf = MemNew(1024);
		if (vsp->errbuf == NULL)
			exit(0);
	}

	if (vsp->errfile == NULL)
	{
		tmp = StringMove(vsp->errbuf, level[severity]);
		tmp = StringMove(tmp, errcode);
		tmp = StringMove(tmp, "|");
	}
	else
		fprintf(vsp->errfile, "%s%s|", level[severity], errcode);
	
	vsp->errors[severity]++;

#ifdef VAR_ARGS
    va_start (args);
#else
    va_start (args, fmt);
#endif

	if (vsp->errfile == NULL)
	{
	    vsprintf (tmp, fmt, args);
		while (*tmp != '\0') tmp++;
	}
	else
		vfprintf(vsp->errfile, fmt, args);
	va_end (args);

	if (vsp->sfp != NULL)
	{
		i = vsp->sfp->data.choice;
		if ((i < 0) || (i > 19))
			i = 0;
		ctmp = SeqLocPrint(vsp->sfp->location);
		if (vsp->errfile == NULL)
			StringMove(tmp, "|Feature\n");
		else
			fprintf(vsp->errfile, "|%s Feature: [%s]",
			types[i], ctmp);
		MemFree(ctmp);
		sip = SeqLocId(vsp->sfp->location);
		if (sip != NULL)
		{
			bsp = BioseqFind(sip);
			if (bsp != NULL)
			{
				SeqIdPrint(bsp->id, idbuf, PRINTID_FASTA_LONG);
				if (vsp->errfile != NULL)
					fprintf(vsp->errfile, "|%s", idbuf);
			}
		}
		if (vsp->errfile != NULL)
			fprintf(vsp->errfile, "\n");
	}
	else if (vsp->bsp != NULL)
	{
		SeqIdPrint(vsp->bsp->id, idbuf, PRINTID_FASTA_LONG);
		if (vsp->errfile == NULL)
		{
			tmp = StringMove(tmp, "|Bioseq ");
			StringMove(tmp, idbuf);
		}
		else
			fprintf(vsp->errfile, "|Bioseq %s\n", idbuf);
	}
	else if (vsp->bsps[vsp->currdepth] != NULL)
	{
		vn.data.ptrvalue = (Pointer)(vsp->bsps[vsp->currdepth]);
		vn.choice = 2;
		vn.next = NULL;
		FindIDForEntry(&vn, idbuf);
		if (vsp->errfile == NULL)
		{
			tmp = StringMove(tmp, "|Set containing ");
			tmp = StringMove(tmp, idbuf);
			StringMove(tmp, "\n");
		}
		else
			fprintf(vsp->errfile, "|Set containing %s\n", idbuf);
	}
	else if (vsp->sep != NULL)
	{
		FindIDForEntry(vsp->sep, idbuf);
		if (vsp->errfile == NULL)
		{
			tmp = StringMove(tmp, "|Entry containing ");
			tmp = StringMove(tmp, idbuf);
			tmp = StringMove(tmp, "\n");
		}
		else
			fprintf(vsp->errfile, "|Entry containing %s\n", idbuf);
	}

	if (vsp->errfile == NULL)
	{
		ErrPost(CTX_NCBIOBJ, 1, "%s", vsp->errbuf);
		vsp->errbuf[0] = '\0';
	}
	else
		fflush(vsp->errfile);

	return;
}

Boolean ValidateSeqEntry(SeqEntryPtr sep, ValidStructPtr vsp)
{
	globalvsp = vsp;   /* for spell checker */

	vsp->sep = sep;
	vsp->bsp = NULL;
	vsp->sfp = NULL;

	if (vsp->non_ascii_chars)  /* non_ascii chars in AsnRead step */
		ValidErr(vsp, 3, "NonAscii", "Non-ascii chars in input ASN.1 strings");
	ValidateAllElements(sep, vsp, 0);
	SeqEntryExplore(sep, (Pointer)vsp, ValidateFeatureTable);
	if (vsp->otherannot)
		ValidErr(vsp, 0, "NotChecked","Seq-annot other than feat table found. Not checked.");

	return TRUE;
}

void ValidateAllElements (SeqEntryPtr sep, ValidStructPtr vsp, Int2 indent)
{
	BioseqPtr bsp;
	BioseqSetPtr bssp;
	ValNodePtr vnp;
	Uint4 descr, tmp;
	SeqEntryPtr sep2;
	static char * errclass = "BioseqSet";

	if ((sep == NULL) || (vsp == NULL)) return;

	vsp->currdepth = indent;
	vsp->nuccnt[indent] = 0;
	vsp->segcnt[indent] = 0;
	vsp->protcnt[indent] = 0;

	if (IS_Bioseq(sep))
	{
		bsp = (BioseqPtr)(sep->data.ptrvalue);
		if (ISA_aa(bsp->mol))
			vsp->protcnt[indent]++;
		else
			vsp->nuccnt[indent]++;
		if (bsp->repr == Seq_repr_seg)
			vsp->segcnt[indent]++;
		
		ValidateBioseq(bsp, vsp);

		SpellCheckDescr(vsp, bsp->descr);

		vsp->bsp = NULL;

	}
	else
	{
		bssp = (BioseqSetPtr)(sep->data.ptrvalue);
		vsp->currdepth = indent;
		vsp->bsps[indent] = bssp;
		descr = 0;
		                         /* set a bit for each descriptor type */
		for (vnp = bssp->descr; vnp != NULL; vnp = vnp->next)
		{
			tmp = 1;
			tmp << (vnp->choice - 1);
			descr |= tmp;
		}

		vsp->descrs[indent] = descr;

		SpellCheckDescr(vsp, bssp->descr);

		for (sep2 = bssp->seq_set; sep2 != NULL; sep2 = sep2->next)
		{
			ValidateAllElements (sep2, vsp, (indent + 1));
			vsp->nuccnt[indent] += vsp->nuccnt[indent + 1];
			vsp->protcnt[indent] += vsp->protcnt[indent + 1];
			vsp->segcnt[indent] += vsp->segcnt[indent + 1];
		}

		if ((vsp->nuccnt[indent] + vsp->protcnt[indent]) == 0)
			ValidErr(vsp, 1, errclass, "No Bioseqs in BioseqSet");

		switch (bssp->_class)
		{
			case 1:     /* nuc-prot */
				if (vsp->nuccnt[indent] == 0)
					ValidErr(vsp, 1, errclass, "No nucleotides in nuc-prot set");
				if (vsp->protcnt[indent] == 0)
					ValidErr(vsp, 1, errclass, "No proteins in nuc-prot set");
				break;
			case 2:     /* seg set */
				if (vsp->segcnt[indent] == 0)
					ValidErr(vsp, 1, errclass, "No segmented Bioseq in segset");
				break;
		}
		                     /* clear lower levels */
		vsp->descrs[indent] = 0;
		vsp->bsps[indent] = NULL;

	}

	vsp->currdepth = indent - 1;
	return;
}

Boolean ValidateBioseq (BioseqPtr bsp, ValidStructPtr vsp)
{
	Boolean retval = TRUE;
	Int2 i;
	Boolean errors[4], check_alphabet;
	static char * repr [7] = {
		"virtual", "raw", "segmented", "constructed",
		"reference", "map" };
	static char * emsgs[] = {
		"Bioseq-ext not allowed on %s Bioseq",
		"Missing or incorrect Bioseq-ext on %s Bioseq",
		"Missing Seq-data on %s Bioseq",
		"Seq-data not allowed on %s Bioseq"};
	static char * errclass = "Seq-inst";
	SeqPortPtr spp;
	Int2 residue, x, termination;
	Int4 len, divisor, len2;
	ValNode head;
	SeqFeatPtr sfp;
	Uint2 partial_val;
	ValNodePtr vnp, vnp2;
	BioseqContextPtr bcp;
	Boolean got_partial;
	int seqtype, terminations;

	if (bsp->id == NULL)
	{
		ValidErr(vsp, 2, errclass, "No ids on a Bioseq");
		return FALSE;
	}

	for (i = 0; i < 4; i++)
		errors[i] = FALSE;

	vsp->bsp = bsp;

	switch (bsp->repr)
	{
		case Seq_repr_virtual:
			if ((bsp->seq_ext_type) || (bsp->seq_ext != NULL))
				errors[0] = TRUE;
			if ((bsp->seq_data_type) || (bsp->seq_data != NULL))
				errors[3] = TRUE;
			break;
		case Seq_repr_map:
			if ((bsp->seq_ext_type != 3) || (bsp->seq_ext == NULL))
				errors[1] = TRUE;
			if ((bsp->seq_data_type) || (bsp->seq_data != NULL))
				errors[3] = TRUE;
			break;
		case Seq_repr_ref:
			if ((bsp->seq_ext_type != 2) || (bsp->seq_ext == NULL))
				errors[1] = TRUE;
			if ((bsp->seq_data_type) || (bsp->seq_data != NULL))
				errors[3] = TRUE;
			break;
		case Seq_repr_seg:
			if ((bsp->seq_ext_type != 1) || (bsp->seq_ext == NULL))
				errors[1] = TRUE;
			if ((bsp->seq_data_type) || (bsp->seq_data != NULL))
				errors[3] = TRUE;
			break;
		case Seq_repr_raw:
		case Seq_repr_const:
			if ((bsp->seq_ext_type) || (bsp->seq_ext != NULL))
				errors[0] = TRUE;
			if ((bsp->seq_data_type < 1) || (bsp->seq_data_type > 11)
				|| (bsp->seq_data == NULL))
				errors[2] = TRUE;
			break;
		default:
			ValidErr(vsp, 2, errclass, "Invalid Bioseq->repr = %d", (int)(bsp->repr));
			return FALSE;
	}

	for (i = 0; i < 4; i++)
	{
		if (errors[i] == TRUE)
		{
			ValidErr(vsp, 2, errclass, emsgs[i], repr[bsp->repr - 1]);
			retval = FALSE;
		}
	}
	if (! retval) return retval;

	if (ISA_aa(bsp->mol))
	{
		if (bsp->topology > 1)   /* not linear */
		{
			ValidErr(vsp, 2, errclass, "Non-linear topology set on protein");
		}
		if (bsp->strand > 1)
		{
			ValidErr(vsp, 2, errclass, "Protein not single stranded");
		}

		        /* check proteins in nuc-prot set have a CdRegion */
		for (i = 0; i < vsp->currdepth; i++)
		{
			if (vsp->bsps[i]->_class == 1)   /* in a nuc-prot set */
			{
				sfp = SeqEntryGetSeqFeat(vsp->sep, 3, NULL, NULL, 1, bsp);
				if (sfp == NULL)   /* no CdRegion points to this bsp */
					ValidErr(vsp, 2, "NoCdRegion",
						"No CdRegion in nuc-prot set points to this protein");
				break;
			}
		}
	}
	                                          /* check sequence alphabet */
	if ((bsp->repr == Seq_repr_raw) || (bsp->repr == Seq_repr_const))
	{
		if (bsp->fuzz != NULL)
		{
			ValidErr(vsp, 2, errclass, "Fuzzy length on %s Bioseq", repr[bsp->repr - 1]);
		}

		if (bsp->length < 1)
		{
			ValidErr(vsp, 2, errclass, "Invalid Bioseq length [%ld]", (long)bsp->length);
		}

		seqtype = (int)(bsp->seq_data_type);
		switch (seqtype)
		{
			case Seq_code_iupacna:
			case Seq_code_ncbi2na:
			case Seq_code_ncbi4na:
			case Seq_code_ncbi8na:
			case Seq_code_ncbipna:
				if (ISA_aa(bsp->mol))
				{
					ValidErr(vsp, 3, errclass,
						"Using a nucleic acid alphabet on a protein sequence");
					return FALSE;
				}
				break;
			case Seq_code_iupacaa:
			case Seq_code_ncbi8aa:
			case Seq_code_ncbieaa:
			case Seq_code_ncbipaa:
			case Seq_code_ncbistdaa:
				if (ISA_na(bsp->mol))
				{
					ValidErr(vsp, 3, errclass,
					"Using a protein alphabet on a nucleic acid");
					return FALSE;
				}
				break;
			default:
				ValidErr(vsp, 3, errclass, "Using illegal sequence alphabet [%d]",
					(int)bsp->seq_data_type);
				return FALSE;
		}

		check_alphabet = FALSE;
		switch (seqtype)
		{
			case Seq_code_iupacaa:
			case Seq_code_iupacna:
			case Seq_code_ncbieaa:
			case Seq_code_ncbistdaa:
				check_alphabet = TRUE;

			case Seq_code_ncbi8na:
			case Seq_code_ncbi8aa:
				divisor = 1;
				break;

			case Seq_code_ncbi4na:
				divisor = 2;
				break;

			case Seq_code_ncbi2na:
				divisor = 4;
				break;

			case Seq_code_ncbipna:
				divisor = 5;
				break;

			case Seq_code_ncbipaa:
				divisor = 21;
				break;
		}

		len = bsp->length;
		if (len % divisor) len += divisor;
		len /= divisor;
		len2 = BSLen(bsp->seq_data);
		if (len > len2)
		{
			ValidErr(vsp, 3, errclass,
			"Bioseq.seq_data too short [%ld] for given length [%ld]",
				(long)(len2 * divisor), (long)bsp->length);
			return FALSE;
		}
		else if (len < len2)
		{
			ValidErr(vsp, 3, errclass,
			"Bioseq.seq_data is larger [%ld] than given length [%ld]",
				(long)(len2 * divisor), (long)bsp->length);
		}

		if (check_alphabet)				  /* check 1 letter alphabets */
		{
			switch (seqtype)
			{
				case Seq_code_iupacaa:
				case Seq_code_ncbieaa:
					termination = '*';
					break;
				case Seq_code_ncbistdaa:
					termination = 25;
					break;

			}
			spp = SeqPortNew(bsp, 0, -1, 0, 0);
			if (spp == NULL)
			{
				ValidErr(vsp, 3, errclass, "Can't open SeqPort");
				return FALSE;
			}

			i = 0;
			terminations = 0;
			for (len=0; len < bsp->length; len++)
			{
				residue = SeqPortGetResidue(spp);
				if (! IS_residue(residue))
				{
					i++;
					if (i > 10)
					{
						ValidErr(vsp, 3, errclass,
						"More than 10 invalid residues. Checking stopped");
						SeqPortFree(spp);
						if (vsp->patch_seq)
							PatchBadSequence(bsp);
						return FALSE;
			 		}
					else
					{
						BSSeek(bsp->seq_data, len, SEEK_SET);
						x = BSGetByte(bsp->seq_data);
						if (bsp->seq_data_type == Seq_code_ncbistdaa)
							ValidErr(vsp, 3, errclass,
							"Invalid residue [%d] in position [%ld]",
							(int)x, (long)(len+1));
						else
							ValidErr(vsp, 3, errclass,
							"Invalid residue [%c] in position [%ld]",
							(char)x, (long)(len+1));
					}
				}
				else if (residue == termination)
					terminations++;
			}
			SeqPortFree(spp);
			if (terminations)
			{
				ValidErr(vsp, 3, "StopInProtein",
					"[%d] termination symbols in protein sequence", terminations);
				if (! i)
					return FALSE;
			}
			if (i)
			{
				if (vsp->patch_seq)
					PatchBadSequence(bsp);
				return FALSE;
			}

		}
	}

	if ((bsp->repr == Seq_repr_seg) ||
		(bsp->repr == Seq_repr_ref))/* check segmented sequence */
	{
		head.choice = SEQLOC_MIX;
		head.data.ptrvalue = bsp->seq_ext;
		head.next = NULL;
		if (! ValidateSeqLoc(vsp, (SeqLocPtr)&head))
			retval = FALSE;

		partial_val = SeqLocPartialCheck((SeqLocPtr)(&head));
		if (partial_val)
		{
			bcp = BioseqContextNew(bsp);
			vnp = NULL;
			got_partial = FALSE;
			while ((vnp = BioseqContextGetSeqDescr(bcp, Seq_descr_modif, vnp, NULL)) != NULL)
			{
				for (vnp2 = (ValNodePtr)(vnp->data.ptrvalue); vnp2 != NULL; vnp2 = vnp2->next)
				{
					switch(vnp2->data.intvalue)
					{
						case 10:   /* partial */
							got_partial = TRUE;
							break;
						case 16:   /* no-left */
							if (! (partial_val & SLP_START))
								ValidErr(vsp, 1, "SegSeq", "GIBB-mod no-left inconsistent with segmented SeqLoc");
							got_partial = TRUE;
							break;
						case 17:   /* no-right */
							if (! (partial_val & SLP_STOP))
								ValidErr(vsp, 1, "SegSeq", "GIBB-mod no-right inconsistent with segmented SeqLoc");
							got_partial = TRUE;
							break;

					}
				}
			}
			if (! got_partial)
				ValidErr(vsp, 1, "SeqSeq", "Partial segmented sequence without GIBB-mod");
		}
	}

	if (! CheckDescrOkForBioseq(bsp, vsp))
		retval = FALSE;

	if (ISA_aa(bsp->mol))
	{
		for (i = 0; i < (vsp->currdepth-1); i++)
		{
			if (vsp->bsps[i]->_class == 1)   /* in a nuc-prot set */
			{
				i = vsp->currdepth;
				sfp = SeqEntryGetSeqFeat(vsp->sep, 3, NULL, NULL, 1, bsp);
				if (sfp == NULL)
					ValidErr(vsp, 1, "NoCdRegion", "No CdRegion for protein in nuc-prot set");
			}
		}
		sfp = SeqEntryGetSeqFeat(vsp->sep, 4, NULL, NULL, 0, bsp);
		if (sfp == NULL)
			ValidErr(vsp, 2, "NoProtRef", "No Prot-ref for protein");

		if ((bsp->length <= 3) && (bsp->length >= 0))
		{
			ValidErr(vsp, 1, "ShortSeq", "Sequence only %ld residues", (long)(bsp->length));
		}

	}
	else
	{
		if ((bsp->length <= 10) && (bsp->length >= 0))
		{
			ValidErr(vsp, 1, "ShortSeq", "Sequence only %ld residues", (long)(bsp->length));
		}
	}

	return retval;
}

Boolean CheckDescrOkForBioseq(BioseqPtr bsp, ValidStructPtr vsp)
{
	int last_na_mol = 0,
		last_na_mod = 0,
		last_organelle = 0,
		last_partialness = 0,
		last_left_right = 0;
	ValNodePtr last_gb = NULL,
		last_embl = NULL,
		last_prf = NULL,
		last_pir = NULL,
		last_sp = NULL,
		last_pdb = NULL,
		last_create = NULL,
		last_update = NULL;
	ValNodePtr vnp, vnp2;
	OrgRefPtr last_org = NULL, this_org;
	Boolean retval = TRUE, is_aa;
	Char buf1[20], buf2[20];
	Int2 indent, warnval;
	int tmpval;
	static char * errclass = "BadSeqDescr";
	static char * badmod = "Inconsistent GIBB-mod [%d] and [%d]";
	Boolean found_pub = FALSE;
	SeqFeatPtr sfp;

	if (ISA_aa(bsp->mol))
		is_aa = TRUE;
	else
		is_aa = FALSE;

	indent = -1;
	do
	{
		if (indent == -1)
			vnp = bsp->descr;
		else if (vsp->bsps[indent] != NULL)
			vnp = vsp->bsps[indent]->descr;
		else
			vnp = NULL;
			
		while (vnp != NULL)
		{
			switch (vnp->choice)
			{
				case Seq_descr_mol_type:
					tmpval = (int)(vnp->data.intvalue);
					switch (tmpval)
					{
						case 8:  /* peptide */
							if (! is_aa)
							{
								ValidErr(vsp, 2, errclass, "Nuclic acid with GIBB-mol = peptide");
								retval = FALSE;
							}
							break;
						case 0:     /* unknown */
						case 255:   /* other */
							ValidErr(vsp, 2, errclass, "GIBB-mol unknown or other used");
							retval = FALSE;
							break;
						default:    /* the rest are nucleic acid */
							if (is_aa)
							{
								ValidErr(vsp, 2, errclass, "GIBB-mol [%d] used on protein",
									tmpval);
								retval = FALSE;
							}
							else
							{
								if (last_na_mol)
								{
									if (last_na_mol != (Int2)vnp->data.intvalue)
									{
										ValidErr(vsp, 2, errclass, "Inconsistent GIBB-mol [%d] and [%d]",
											last_na_mol, tmpval);
										retval = FALSE;
									}
								}
								else
									last_na_mol = tmpval;
							}
							break;
					}
					break;
				case Seq_descr_modif:
					for (vnp2 = (ValNodePtr)(vnp->data.ptrvalue); vnp2 != NULL; vnp2 = vnp2->next)
					{
						tmpval = (int)(vnp2->data.intvalue);
						switch (tmpval)
						{
							case 0:   /* dna */
							case 1:	  /* rna */
								if (is_aa)	   /* only temporarily on 0 */
								{
									ValidErr(vsp, 2, errclass, "Nucleic acid GIBB-mod [%d] on protein",
										tmpval);
								}
								else if (last_na_mod)
								{
									if (tmpval != last_na_mod)
									{
										ValidErr(vsp, 2, errclass, badmod,
											last_na_mod, tmpval);
										retval = FALSE;
									}
								}
								else
									last_na_mod = tmpval;
								break;
							case 4:   /* mitochondria */
							case 5:   /* chloroplast */
							case 6:   /* kinetoplast */
							case 7:   /* cyanelle */
							case 18:  /* macronuclear */
								if (last_organelle)
								{
									if (tmpval != last_na_mod)
									{
										ValidErr(vsp, 2, errclass, badmod,
											last_organelle, tmpval);
										retval = FALSE;
									}
								}
								else
									last_organelle = tmpval;
								break;
							case 10:  /* partial */
							case 11:  /* complete */
								if (last_partialness)
								{
									if (tmpval != last_partialness)
									{
										ValidErr(vsp, 2, errclass, badmod,
											last_partialness, tmpval);
										retval = FALSE;
									}
								}
								else
									last_partialness = tmpval;
								if ((last_left_right) && (tmpval == 11))
								{
									ValidErr(vsp, 2, errclass, badmod,
										last_left_right, tmpval);
									retval = FALSE;
								}
								break;
							case 16:   /* no left */
							case 17:   /* no right */
								if (last_partialness == 11)  /* complete */
								{
									ValidErr(vsp, 2, errclass, badmod,
										last_partialness, tmpval);
									retval = FALSE;
								}
								last_left_right = tmpval;
								break;
							case 255:  /* other */
								ValidErr(vsp, 1, errclass, "GIBB-mod = other used");
								break;
							default:
								break;
		
						}
					}
					break;
				case Seq_descr_method:
					if (! is_aa)
					{
						ValidErr(vsp, 2, errclass, "Nucleic acid with protein sequence method");
						retval = FALSE;
					}
					break;
				case Seq_descr_genbank:
					if (last_gb != NULL)
						ValidErr(vsp, 1, errclass, "Multiple GenBank blocks");
					else
						last_gb = vnp;
					break;
				case Seq_descr_embl:
					if (last_embl != NULL)
						ValidErr(vsp, 1, errclass, "Multiple EMBL blocks");
					else
						last_embl = vnp;
					break;
				case Seq_descr_pir:
					if (last_pir != NULL)
						ValidErr(vsp, 1, errclass, "Multiple PIR blocks");
					else
						last_pir = vnp;
					break;
				case Seq_descr_sp:
					if (last_sp != NULL)
						ValidErr(vsp, 1, errclass, "Multiple SWISS-PROT blocks");
					else
						last_sp = vnp;
					break;
				case Seq_descr_pdb:
					if (last_pdb != NULL)
						ValidErr(vsp, 1, errclass, "Multiple PDB blocks");
					else
						last_pdb = vnp;
					break;
				case Seq_descr_prf:
					if (last_prf != NULL)
						ValidErr(vsp, 1, errclass, "Multiple PRF blocks");
					else
						last_prf = vnp;
					break;
				case Seq_descr_create_date:
					if (last_create != NULL)
					{
						tmpval = (int)DateMatch((DatePtr)vnp->data.ptrvalue,
							(DatePtr)(last_create->data.ptrvalue), FALSE);
						if (tmpval)
						{
							DatePrint((DatePtr)(vnp->data.ptrvalue), buf1);
							DatePrint((DatePtr)(last_create->data.ptrvalue), buf2);
							ValidErr(vsp, 2, errclass, "Inconsistent create_dates [%s] and [%s]",
								buf1, buf2);
							retval = FALSE;
						}
					}
					else
						last_create = vnp;
					if (last_update != NULL)
					{
						tmpval = (int)DateMatch((DatePtr)vnp->data.ptrvalue,
							(DatePtr)(last_update->data.ptrvalue), FALSE);
						if (tmpval == 1)
						{
							DatePrint((DatePtr)(vnp->data.ptrvalue), buf1);
							DatePrint((DatePtr)(last_update->data.ptrvalue), buf2);
							ValidErr(vsp, 2, errclass, "Inconsistent create_date [%s] and update_date [%s]",
								buf1, buf2);
							retval = FALSE;
						}
					}
					break;
				case Seq_descr_update_date:
					if (last_create != NULL)
					{
						tmpval = (int)DateMatch((DatePtr)last_create->data.ptrvalue,
							(DatePtr)(vnp->data.ptrvalue), FALSE);
						if (tmpval == 1)
						{
							DatePrint((DatePtr)(last_create->data.ptrvalue), buf1);
							DatePrint((DatePtr)(vnp->data.ptrvalue), buf2);
							ValidErr(vsp, 2, errclass, "Inconsistent create_date [%s] and update_date [%s]",
								buf1, buf2);
							retval = FALSE;
						}
					}
					if (last_update == NULL)
						last_update = vnp;
					break;
				case Seq_descr_org:
					this_org = (OrgRefPtr)(vnp->data.ptrvalue);
					if (last_org != NULL)
					{
						if ((this_org->taxname != NULL) && (last_org->taxname != NULL))
						{
							if (StringCmp(this_org->taxname, last_org->taxname))
							{
								ValidErr(vsp, 2, errclass, "Inconsistent taxnames [%s] and [%s]",
									this_org->taxname, last_org->taxname);
								retval = FALSE;
							}
						}
					}
					else
						last_org = this_org;
					break;
				case Seq_descr_pub:
					found_pub = TRUE;
					break;
			}
			vnp = vnp->next;
		}

		if (indent == -1)   /* just did Bioseq-descr */
			indent = vsp->currdepth;
		else
			indent--;

	} while (indent >= 0);

	if (last_org == NULL)    /**** Should check features too ****/
	{
		sfp = SeqEntryGetSeqFeat(vsp->sep, 2, NULL, NULL, 0, bsp);
		if (sfp == NULL)
		{
			ValidErr(vsp, 2, "NoOrg", "No Organism Found");
			retval = FALSE;
		}
	}
	if (! found_pub)
	{
		sfp = SeqEntryGetSeqFeat(vsp->sep, 6, NULL, NULL, 0, bsp);
		if (sfp == NULL)
		{
		        /*** PARSER ***/
			if (is_aa)
				warnval = 1;
			else
				warnval = 2;
				
			ValidErr(vsp, warnval, "NoPub", "No Publications Found");
			retval = FALSE;
		}
	}

	return retval;
}


void ValidateFeatureTable (SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent)
{
	SeqAnnotPtr sap;
	BioseqPtr bsp;
	BioseqSetPtr bssp;
	ValidStructPtr vsp;
	SeqFeatPtr sfp;

	vsp = (ValidStructPtr)data;

	if (IS_Bioseq(sep))
	{
		bsp = (BioseqPtr)(sep->data.ptrvalue);
		sap = bsp->annot;
	}
	else
	{
		bssp = (BioseqSetPtr)(sep->data.ptrvalue);
		sap = bssp->annot;
	}

	while (sap != NULL)
	{
		if (sap->type == 1)   /* ftable */
		{
			for (sfp = (SeqFeatPtr)(sap->data); sfp != NULL; sfp = sfp->next)
				ValidateSeqFeat(vsp, sfp);
		}
		else                 /* other seqannot type -- not currently checked */
		{
			vsp->otherannot++;
		}
		sap = sap->next;
	}
	vsp->sfp = NULL;
	return;
}


/*****************************************************************************
*
*   ValidateSeqFeat(vsp, sfp)
*
*****************************************************************************/
Boolean ValidateSeqFeat( ValidStructPtr vsp, SeqFeatPtr sfp)
{
	Boolean retval = TRUE;
	Int2 type, i, j;
	static char * errclass = "BadLoc";
	static char * parterr[2] = {"PartialProduct", "PartialLocation"};
	static char * parterrs[4] = {
		"Start does not include first/last residue of sequence",
		"Stop does not include first/last residue of sequence",
		"Internal partial intervals do not include first/last residue of sequence",
		"Improper use of partial (greater than or less than)" };
	Uint2 partials[2], errtype;
	Char buf[80];
	CharPtr tmp;

	vsp->sfp = sfp;
	type = (Int2)(sfp->data.choice);

	SpellCheckSeqFeat(vsp, sfp);

	if (! ValidateSeqLoc(vsp, sfp->location))
		retval = FALSE;

	if (! ValidateSeqLoc(vsp, sfp->product))
		retval = FALSE;

	partials[0] = SeqLocPartialCheck(sfp->product);
	partials[1] = SeqLocPartialCheck(sfp->location);
	if ((partials[0]) || (partials[1]) || (sfp->partial))  /* partialness */
	{
		if ((! sfp->partial) && (partials[1] != SLP_COMPLETE) &&
			(sfp->location->choice == SEQLOC_WHOLE))
		{
			ValidErr(vsp, 0, "SeqFeatPartial", "On partial Bioseq, SeqFeat.partial should be TRUE");
		}
		else if ((sfp->partial) && (sfp->product != NULL) &&
			(partials[1] == SLP_COMPLETE) && (sfp->product->choice == SEQLOC_WHOLE)
			&& (partials[0] != SLP_COMPLETE))
		{
			ValidErr(vsp, 0, "SeqFeatPartial",
				"When SeqFeat.product is a partial Bioseq, SeqFeat.location should also be partial");
		}
		else if (((partials[0] == SLP_COMPLETE) && (sfp->product != NULL)) ||
			(partials[1] == SLP_COMPLETE) ||
			(! sfp->partial))
		{
			tmp = StringMove(buf, "Inconsistent: ");
			if (sfp->product != NULL)
			{
				tmp = StringMove(tmp, "Product= ");
				if (partials[0])
					tmp = StringMove(tmp, "partial, ");
				else
					tmp = StringMove(tmp, "complete, ");
			}
			tmp = StringMove(tmp, "Location= ");
			if (partials[1])
				tmp = StringMove(tmp, "partial, ");
			else
				tmp = StringMove(tmp, "complete, ");
			tmp = StringMove(tmp, "Feature.partial= ");
			if (sfp->partial)
				tmp = StringMove(tmp, "TRUE");
			else
				tmp = StringMove(tmp, "FALSE");
			ValidErr(vsp, 0, "SeqFeatPartial", buf);
			retval = FALSE;
		}

		for (i = 0; i < 2; i++)
		{
			errtype = SLP_NOSTART;
			for (j = 0; j < 4; j++)
			{
				if (partials[i] & errtype)
				{
					ValidErr(vsp, 0, parterr[i], parterrs[j]);
					retval = FALSE;
				}
				errtype <<= 1;
			}
		}

	}

	switch (type)
	{
		case 1:        /* Gene-ref */
			break;
		case 2:        /* Org-ref */
			break;
		case 3:        /* Cdregion */
			CdTransCheck(vsp, sfp);
			break;
		case 4:        /* Prot-ref */
			break;
		case 5:        /* RNA-ref */
			break;
		case 6:        /* Pub */
			break;
		case 7:        /* Seq */
			break;
		case 8:        /* Imp-feat */
			break;
		case 9:        /* Region */
			break;
		case 10:        /* Comment */
			break;
		case 11:        /* Bond */
			break;
		case 12:        /* Site */
			break;
		case 13:        /* Rsite-ref */
			break;
		case 14:        /* User-object */
			break;
		case 15:        /* TxInit */
			break;
		case 16:        /* Numbering */
			break;
		case 17:        /* Secondary Structure */
			break;
		case 18:        /* NonStdRes*/
			break;
		case 19:        /* Heterogen*/
			break;
		default:
			ValidErr(vsp, 2, "BadSeqFeat", "Invalid SeqFeat type [%d]",
				(int)(type));
			retval = FALSE;
			break;
	}
	return retval;
}

/*****************************************************************************
*
*   CdTransCheck(sfp)
*   	Treatment of terminal 'X'
*          If either the protein or the translation end in 'X' (usually
*          due to partial last codon) it is ignored to minimize conflicts
*          between approaches to add the X or not in this case.
*
*****************************************************************************/
void CdTransCheck(ValidStructPtr vsp, SeqFeatPtr sfp)
{
	ByteStorePtr newprot = NULL;
	BioseqPtr prot1seq=NULL, prot2seq=NULL;
	SeqLocPtr slp=NULL, curr = NULL;
	Int4 prot1len, prot2len, i, len;
	CdRegionPtr crp;
	SeqIdPtr protid=NULL;
	Int2 residue1, residue2, stop_count = 0, mismatch = 0;
	Boolean got_stop = FALSE, test_it = TRUE;
	SeqPortPtr spp=NULL;
	Uint2 part_loc=0, part_prod=0;
	Boolean no_end = FALSE, no_beg = FALSE, show_stop = FALSE,
		got_dash = FALSE;
	GBQualPtr gb;
	ValNodePtr vnp, code;
	int gccode = 0;
	Boolean transl_except = FALSE, prot_ok = TRUE;

	crp = (CdRegionPtr)(sfp->data.value.ptrvalue);
	if (crp->code_break == NULL)  /* check for unparsed transl_except */
	{
		for (gb = sfp->qual; gb != NULL; gb = gb->next)
		{
			if (! StringCmp(gb->qual, "transl_except"))
			{
				transl_except = TRUE;
				break;
			}
		}
	}

	if (crp->genetic_code != NULL)
	{
		for (vnp = crp->genetic_code->data.ptrvalue; ((vnp != NULL) && (! gccode)); vnp = vnp->next)
		{
			switch (vnp->choice)
			{
				case 0:
				    break;
				case 1:     /* name */
					code = GeneticCodeFind(0, (CharPtr)(vnp->data.ptrvalue));
					if (code != NULL)
					{
						for (vnp = code->data.ptrvalue; ((vnp != NULL) && (! gccode)); vnp = vnp->next)
						{
							if (vnp->choice == 2) /* id */
								gccode = (int)(vnp->data.intvalue);
						}
					}
					break;
				case 2:    /* id */
				    gccode = (int)(vnp->data.intvalue);
					break;
				default:
				    gccode = 255;
					break;
			}
		}
	}
					
	newprot = ProteinFromCdRegion(sfp, TRUE);   /* include stop codons */
	if (newprot == NULL)
	{
		ValidErr(vsp, 2, "TransFail", "Unable to translate");
		prot_ok = FALSE;
		goto erret;
	}

	part_loc = SeqLocPartialCheck(sfp->location);
	part_prod = SeqLocPartialCheck(sfp->product);
	if ((part_loc & SLP_STOP) || (part_prod & SLP_STOP))
		no_end = TRUE;

	if ((part_loc & SLP_START) || (part_prod & SLP_START))
		no_beg = TRUE;

	prot2len = BSLen(newprot);
	len = prot2len;
	BSSeek(newprot, 0, SEEK_SET);
	for (i =0 ; i < len; i++)
	{
		residue1 = BSGetByte(newprot);
		if ((len == 0) && (residue1 == '-'))
			got_dash = TRUE;
		if (residue1 == '*')
		{
			if (i == (len - 1))
				got_stop = TRUE;
			else
				stop_count++;
		}
	}

	if (stop_count)
	{
		if (got_dash)
			ValidErr(vsp, 2, "StartCodon",
				"Illegal start codon and %ld internal stops. Probably wrong genetic code [%d]",
				(long)stop_count, gccode);
		else
			ValidErr(vsp, 2, "InternalStop", "%ld internal stops. Genetic code [%d]",
				(long)stop_count, gccode);
		prot_ok = FALSE;
		goto erret;
	}

	show_stop = TRUE;

	protid = SeqLocId(sfp->product);
	if (protid != NULL)
	{
		prot1seq = BioseqFind(protid);
		if (prot1seq != NULL)
			prot1len = prot1seq->length;
	}

	if (prot1seq == NULL)
	{
		ValidErr(vsp, 2, "NoProtein", "No protein given");
		goto erret;
	}

	len = prot2len;

	if ((got_stop)&&(len == (prot1len + 1)))  /* ok, got stop */
	{
		len--;
	}

	spp = SeqPortNew(prot1seq, 0, -1, 0, Seq_code_ncbieaa);
	if (spp == NULL) goto erret;

	    /* ignore terminal 'X' from partial last codon if present */

	SeqPortSeek(spp, (prot1len - 1), SEEK_SET);
	residue1 = SeqPortGetResidue(spp);
	if (residue1 == 'X')   /* remove terminal X */
		prot1len--;
	BSSeek(newprot, (len-1), SEEK_SET);
	residue2 = BSGetByte(newprot);
	if (residue2 == 'X')
		len--;

	if (len == prot1len)   /* could be identical */
	{
		SeqPortSeek(spp, 0, SEEK_SET);
	   	BSSeek(newprot, 0, SEEK_SET);
		for (i = 0; i < len; i++)
		{
			residue1 = BSGetByte(newprot);
			residue2 = SeqPortGetResidue(spp);
			if (residue1 != residue2)
			{
				prot_ok = FALSE;
				if (residue2 == INVALID_RESIDUE)
					residue2 = '?';
				if (mismatch == 10)
				{
					ValidErr(vsp, 2, "MisMatchAA10", "More than 10 mismatches. Genetic code [%d]", gccode);
					break;
				}
				else if (i == 0)
				{
					if ((sfp->partial) && (! no_beg) && (! no_end))  /* ok, it's partial */
						ValidErr(vsp, 1, "FixPart", "Start of location should probably be partial");
					else if (residue1 == '-')
						ValidErr(vsp, 2, "StartCodon", "Illegal start codon used. Wrong genetic code [%d] or protein should be partial", gccode);
					else
						ValidErr(vsp, 2, "MisMatchAA", "Residue %ld in protein [%c] != translation [%c]",
							(long)(i+1), (char)residue2, (char)residue1);
				}
				else
				{
					ValidErr(vsp, 2, "MisMatchAA", "Residue %ld in protein [%c] != translation [%c]",
						(long)(i+1), (char)residue2, (char)residue1);
				}
				mismatch++;
			}
		}
		spp = SeqPortFree(spp);
	}
	else
	{
		ValidErr(vsp, 2, "TransLen", "Given protein length [%ld] does not match translation length [%ld]",
			prot1len, len);
	}

	if ((sfp->partial) && (! mismatch))
	{
		if ((! no_beg) && (! no_end))   /* just didn't label */
		{
			if (! got_stop)
			{
				ValidErr(vsp, 1, "FixPart", "End of location should probably be partial");
			}
			else
			{
				ValidErr(vsp, 1, "FixPart", "This SeqFeat should not be partial");
			}
			show_stop = FALSE;
		}
	}
		

erret:
	if (show_stop)
	{
		if ((! got_stop) && (! no_end))
		{
			ValidErr(vsp, 2, "NoStop", "Missing stop codon");
		}
		else if ((got_stop) && (no_end))
		{
			ValidErr(vsp, 2, "NoEnd", "Got stop codon, but 3'end is partial");
		}
	}

	if (! prot_ok)
	{
		if (transl_except)
			ValidErr(vsp,1,"TranslExcept", "Unparsed transl_except qual. Skipped");
	}

	if (prot2seq != NULL)
		BioseqFree(prot2seq);
	else
		BSFree(newprot);
	SeqPortFree(spp);
	return;
}


/*****************************************************************************
*
*   ValidateSeqLoc(vsp, slp)
*
*****************************************************************************/
Boolean ValidateSeqLoc(ValidStructPtr vsp, SeqLocPtr slp)
{
	SeqLocPtr tmp, prev;
	Boolean retval = TRUE, tmpval, mixed_strand = FALSE, ordered=TRUE;
	CharPtr ctmp;
	Uint1 strand2, strand1;
	SeqIntPtr sip1, sip2;
	SeqPntPtr spp;
	PackSeqPntPtr pspp;
	SeqIdPtr id1 = NULL, id2;

	if (slp == NULL) return TRUE;

	tmp = NULL;
	prev = NULL;
	sip1 = NULL;
	strand1 = Seq_strand_other;
	while ((tmp = SeqLocFindNext(slp, tmp)) != NULL)
	{
		tmpval = TRUE;
		switch (tmp->choice)
		{
			case SEQLOC_INT:
				sip2 = (SeqIntPtr)(tmp->data.ptrvalue);
				strand2 = sip2->strand;
				id2 = sip2->id;
				tmpval = SeqIntCheck (sip2);
				if ((tmpval) && (sip1 != NULL) && (ordered))
				{
					if (SeqIdForSameBioseq(sip1->id, sip2->id))
					{
						if (strand2 == Seq_strand_minus)
						{
							if (sip1->to < sip2->to)
								ordered = FALSE;
						}
						else if (sip1->to > sip2->to)
							ordered = FALSE;
					}
				}
				break;
			case SEQLOC_PNT:
				spp = (SeqPntPtr)(tmp->data.ptrvalue);
				strand2 = spp->strand;
				id2 = spp->id;
				tmpval = SeqPntCheck (spp);
				break;
			case SEQLOC_PACKED_PNT:
				pspp = (PackSeqPntPtr)(tmp->data.ptrvalue);
				strand2 = pspp->strand;
				id2 = pspp->id;
				tmpval = PackSeqPntCheck (pspp);
				break;
			default:
			    strand2 = Seq_strand_other;
				id2 = NULL;
				break;
		}
		if (! tmpval)
		{
			retval = FALSE;
			ctmp = SeqLocPrint(tmp);
			ValidErr(vsp, 3, "RangeErr", "SeqLoc [%s] out of range", ctmp);
			MemFree(ctmp);

		}

		if ((strand1 != Seq_strand_other) && (strand2 != Seq_strand_other))
		{
			if (SeqIdForSameBioseq(id1, id2))
			{
				if (strand1 != strand2)
					mixed_strand = TRUE;
			}
		}

		strand1 = strand2;
		id1 = id2;
	}

	if ((mixed_strand) || (! ordered))
	{
		ctmp = SeqLocPrint(slp);
		if (mixed_strand)
			ValidErr(vsp, 1, "MixedStrand", "Mixed strands in SeqLoc [%s]", ctmp);
		if (! ordered)
			ValidErr(vsp, 1, "SeqLocOrder", "Intervals out of order in SeqLoc [%s]", ctmp);
		MemFree(ctmp);
	}

	return retval;
}

/*****************************************************************************
*
*   PatchBadSequence(bsp)
*
*****************************************************************************/
Boolean PatchBadSequence(BioseqPtr bsp)
{
	ByteStorePtr newseq;
	SeqPortPtr spp;
	Boolean is_na;
	Uint1 seqcode;
	Int2 repchar, residue;
	Int4 i, len;

	if (bsp == NULL) return FALSE;
	if (! ((bsp->repr == Seq_repr_raw) || (bsp->repr == Seq_repr_const)))
		return FALSE;

	is_na = ISA_na(bsp->mol);
	if (is_na)
	{
		seqcode = Seq_code_iupacna;
		repchar = (Int2)'N';   /* N */
	}
	else
	{
		seqcode = Seq_code_iupacaa;
		repchar = (Int2)'X';
	}

	spp = SeqPortNew(bsp, 0, -1, 0, seqcode);
	if (spp == NULL) return FALSE;

	len = bsp->length;
	newseq = BSNew(len);
	if (newseq == NULL)
	{
		SeqPortFree(spp);
		return FALSE;
	}

	for (i = 0; i < len; i++)
	{
		residue = SeqPortGetResidue(spp);
		if (residue == INVALID_RESIDUE)
		{
			residue = repchar;
		}
		BSPutByte(newseq, residue);
	}

	SeqPortFree(spp);
	BSFree(bsp->seq_data);
	bsp->seq_data = newseq;
	bsp->seq_data_type = seqcode;

	BioseqRawPack(bsp);

	return TRUE;
}

static void FindABioseq(SeqEntryPtr sep, Pointer data, Int4 index, Int2 indent)
{
	BioseqPtr PNTR bp;
	BioseqPtr bsp;

	bp = (BioseqPtr PNTR)data;
	if (*bp != NULL)   /* already got one */
		return;

	if (IS_Bioseq(sep))
	{
		bsp = (BioseqPtr)(sep->data.ptrvalue);
		*bp = bsp;
	}
	return;
}

CharPtr FindIDForEntry (SeqEntryPtr sep, CharPtr buf)
{
	BioseqPtr bsp = NULL;
	
	if ((sep == NULL) || (buf == NULL))
		return NULL;

	*buf = '\0';
	SeqEntryExplore (sep, (Pointer)(&bsp), FindABioseq);

	if (bsp == NULL) return NULL;

	SeqIdPrint(bsp->id, buf, PRINTID_FASTA_LONG);
	return buf;
}


void SpellCheckDescr(ValidStructPtr vsp, ValNodePtr vnp)
{
	PubdescPtr pdp;
	CitArtPtr cap;
	ValNodePtr tmp, titles;

	if ((vsp == NULL) || (vnp == NULL))
		return;

	if (vsp->spellfunc == NULL) return;

	while (vnp != NULL)
	{
		switch (vnp->choice)
		{
			case Seq_descr_title:
			case Seq_descr_region:
			case Seq_descr_comment:
				SpellCheckString(vsp, (CharPtr)(vnp->data.ptrvalue));
				break;
			case Seq_descr_pub:
				pdp = (PubdescPtr)(vnp->data.ptrvalue);
				for (tmp = pdp->pub; tmp != NULL; tmp = tmp->next)
				{
					if (tmp->choice == PUB_Article)
					{
						cap = (CitArtPtr)(tmp->data.ptrvalue);
						for (titles = cap->title; titles != NULL; titles = titles->next)
						{
							if (titles->choice == Cit_title_name)
								SpellCheckString(vsp, (CharPtr)(titles->data.ptrvalue));
						}
					}
				}
				break;
			default:
				break;
		}
		vnp = vnp->next;
	}
	return;
}

void SpellCheckSeqFeat(ValidStructPtr vsp, SeqFeatPtr sfp)
{
	PubdescPtr pdp;
	CitArtPtr cap;
	ProtRefPtr prp;
	ValNodePtr vnp, titles;

	if ((vsp == NULL) || (sfp == NULL))
		return;
	if (vsp->spellfunc == NULL) return;

	SpellCheckString(vsp, sfp->comment);

	switch (sfp->data.choice)
	{
		case 1:        /* Gene-ref */
			break;
		case 2:        /* Org-ref */
			break;
		case 3:        /* Cdregion */
			break;
		case 4:        /* Prot-ref */
			prp = (ProtRefPtr)(sfp->data.value.ptrvalue);
			for (vnp = prp->name; vnp != NULL; vnp = vnp->next)
				SpellCheckString(vsp, (CharPtr)(vnp->data.ptrvalue));
			SpellCheckString(vsp, prp->desc);
			break;
		case 5:        /* RNA-ref */
			break;
		case 6:        /* Pub */
				pdp = (PubdescPtr)(sfp->data.value.ptrvalue);
				for (vnp = pdp->pub; vnp != NULL; vnp = vnp->next)
				{
					if (vnp->choice == PUB_Article)
					{
						cap = (CitArtPtr)(vnp->data.ptrvalue);
						for (titles = cap->title; titles != NULL; titles = titles->next)
						{
							if (titles->choice == Cit_title_name)
								SpellCheckString(vsp, (CharPtr)(titles->data.ptrvalue));
						}
					}
				}
				break;
			break;
		case 7:        /* Seq */
			break;
		case 8:        /* Imp-feat */
			break;
		case 9:        /* Region */
			SpellCheckString(vsp, (CharPtr)(sfp->data.value.ptrvalue));
			break;
		case 10:        /* Comment */
			break;
		case 11:        /* Bond */
			break;
		case 12:        /* Site */
			break;
		case 13:        /* Rsite-ref */
			break;
		case 14:        /* User-object */
			break;
		case 15:        /* TxInit */
			break;
		case 16:        /* Numbering */
			break;
		case 17:        /* Secondary Structure */
			break;
		case 18:        /* NonStdRes*/
			break;
		case 19:        /* Heterogen*/
			break;
		default:
			break;
	}

	return;
}

void SpellCheckString (ValidStructPtr vsp, CharPtr str)
{
	if ((vsp == NULL) || (str == NULL))
		return;

	if (vsp->spellfunc == NULL) return;

	(* (vsp->spellfunc))((char *)str, (vsp->spellcallback));

	return;
}

void SpellCallBack (char * str)
{

	ValidErr(globalvsp, 2, "SpellErr", "[ %s ]", (CharPtr)str);
	return;
}
