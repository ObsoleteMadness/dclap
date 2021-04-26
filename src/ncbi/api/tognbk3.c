/*

OPTION USED: (The internal ones are in a separate linked 
list than the ones passed to SeqEntryToGenBank)

The first for or simply to pass parameters in that define defaults
and error reporting.

THESE ARE PASSED TO SeqEntryToGenBank, but NOT used internally.

#define OP_TOGENBNK_DIV ( (Int2)-1)
#define OP_TOGENBNK_DATE ( (Int2)-2)
#define OP_TOGENBNK_NO_NCBI ( (Int2)-3)
#define OP_TOGENBNK_QUIET ( (Int2)-4)

This one is more complex: (AND IS USED INTERNALLY)

#define OP_TOGENBNK_BINDINGS ( (Int2)-5)

-6 to -8 now used, see togenbk.h

To uitlize the information from the Backbone style ASN.1, the 
Gene and Prot-Ref must be bound to the CdRegion.  This option
flags that this binding has been done and carries the information
about who is bound (or orphaned) to whom.

Search for "Expresson" to see structure and usages.

	$Log:	tognbk3.c,v $
 * Revision 1.24  94/02/16  17:26:52  sirotkin
 * Keywords fro out of BIoseq used`
 * 
 * Revision 1.23  94/02/09  11:59:35  sirotkin
 * FlatIndirectStringCmp to LIBCALLBACK, not LIBCALL as per kans
 * 
 * Revision 1.22  94/02/08  13:37:49  sirotkin
 * undergoing qa for release, includes new date logic
 * NCBI gi added to CDS
 * 
 * Revision 1.21  94/02/07  14:11:09  sirotkin
 * Tom Madden's fix_pages_installed
 * 
 * Revision 1.20  94/01/21  14:15:18  sirotkin
 * Moved free of namehead in book author case till after last use
 * 
 * Revision 1.19  94/01/14  15:49:49  sirotkin
 * added, in release mode (-o) NCBI gi: %d to COMMENT
 * 
 * Revision 1.18  93/12/09  14:05:26  sirotkin
 * SeqPort changes including invalid residue, tRNA formating and date priority changing
 * 
 * Revision 1.17  93/11/15  10:20:58  sirotkin
 * INVALID_RESIDUE and "GenBank staff at the National Library of Medicine" message
 * 
 * Revision 1.16  93/11/02  13:34:57  sirotkin
 * Fixed CitSub full affil case.
 * 
 * Revision 1.15  93/10/13  15:55:45  sirotkin
 * Allows titleless CitGens and maps scRAN -> RNA
 * 
 * Revision 1.14  93/09/27  14:02:52  sirotkin
 * expanding tognbk3.c and tweaks to patent -m 4 format
 * 
 * Revision 1.13  93/09/10  11:36:58  epstein
 * Avoid obselete '=&' construct, per Will Gilbert
 * 
 * Revision 1.12  93/07/29  10:11:24  sirotkin
 * xrefs use SeqIdPrint for safety, esp. with gi nubmers.
 * 
 * Revision 1.11  93/07/28  11:42:53  sirotkin
 * nits for embl format
 * ,
 * 
 * Revision 1.10  93/07/14  13:46:33  schuler
 * MemFree cannot be used as an argument to AsnOptionNew.  Changed to
 * DefAsnOptionFree in two places.
 * 
 * Revision 1.9  93/07/12  12:28:00  sirotkin
 * fix to GetQual stuff
 * 
 * Revision 1.8  93/07/12  10:48:15  sirotkin
 * No OrgRef forces /organism unknown
 * 
 * Revision 1.7  93/07/09  17:22:03  ostell
 * ,
 * 
 * Revision 1.6  93/07/09  12:15:51  sirotkin
 * checks GeneRefs against known GBQuals before misc_feature
 * 
 * Revision 1.5  93/07/07  12:39:03  sirotkin
 * Patent DEFINITION line generated
 * 
 * Revision 1.4  93/07/02  15:26:33  ostell
 * more msc nit fixes
 * 
 * Revision 1.2  93/07/02  13:18:05  sirotkin
 * attempt to fix Microsoft C compiler nit fusses)
 * 
 * Revision 1.1  93/07/01  14:57:21  sirotkin
 * Initial revision
 * 
 * 
*/
#include <math.h>
#include <togenbnk.h>


extern Uint1 Flat_Be_quiet;

extern Uint1 Locus_magic_cookie, Accession_magic_cookie;
#ifdef T10_and_CDROM
extern Uint1 Flat_T10, Flat_CdRom;
#endif

extern Uint1 Flat_do_Flat;
	extern FILE * Flat_Index_accession;
	extern FILE * Flat_Index_keyword;
	extern FILE * Flat_Index_author;
	extern FILE * Flat_Index_journal;
	extern FILE * Flat_Index_gene;
extern CharPtr Flat_last_div , Flat_last_locus , 
		Flat_last_accession ;
extern Int4 Number_loci , Total_bases ;

extern Boolean Fuzz_found;

extern Uint1 Flat_format, Flat_nuc_prot_which;
extern int Flat_line_blank; /* 12 for GenBank, 5 for EMBL */
extern Uint1 Flat_release;

extern CharPtr ACC_BASE_LTR ;
extern CharPtr COM_BLANK_LTR;
extern CharPtr COM_BASE_LTR;
extern CharPtr DEF_BASE_LTR;
extern CharPtr Blank_BASE_LTR;
extern CharPtr Key_BASE_LTR;
extern CharPtr ORI_BASE_LTR;
extern CharPtr REF_BASE_LTR;
extern CharPtr REF_BASE_LTR_BP;
extern CharPtr AUTHOR_LTR;
extern CharPtr TITLE_LTR;
extern CharPtr JOURNAL_LTR;
extern CharPtr STANDARD_LTR;
extern CharPtr SOU_BASE_LTR;
extern CharPtr ORG_LTR;
extern CharPtr ORG_LTR_nel;
extern CharPtr ORG_LTR_tax;
extern CharPtr FEAT_CONT_BLANKS;


/*----------- FlatMainAccession  ()------*/
CharPtr FlatMainAccession 
(SeqEntryPtr the_seq, AsnOptionPtr optionHead)
{
	CharPtr retval = NULL;
	BioseqPtr bs;
	SeqIdPtr id, best_id;
	char buf [20];
	static Boolean order_initialized = FALSE;
	static Uint1 order[18];
	
if ( ! order_initialized){
	int dex;
	for (dex=0; dex < 18; dex ++)
		order[dex] = 255;
	order_initialized = TRUE;
		order[SEQID_GENBANK ] = 1;
		order[SEQID_EMBL ] = 2;
		order[SEQID_DDBJ ] = 3;
		order[SEQID_LOCAL ] =4;
		order[SEQID_GI ] =5;
		order[SEQID_GIBBSQ ] =6;
		order[SEQID_GIBBMT ] =7;
		order[SEQID_PRF ] =8;
		order[SEQID_PDB ] =9;
		order[SEQID_PIR ] =10;
		order[SEQID_SWISSPROT ] =11;
		order[SEQID_PATENT ] =12;
		order[SEQID_OTHER ] =13;
		order[SEQID_GENERAL ] =14;
		order[SEQID_GIIM ] =15;
}

   bs = the_seq -> data.ptrvalue;

   id = bs -> id;
   if ( id ){
			best_id = SeqIdSelect( id, order,18);
			SeqIdPrint (best_id, buf, PRINTID_TEXTID_ACCESSION);
			if ( * buf){
				retval = StringSave(buf);
			}
   }
   if ( ! retval){
#ifdef ERROR_IF_NO_MAIN_ACCESSION
		if ( !  (Flat_Be_quiet&1))
      ErrPost(CTX_NCBI2GB,CTX_2GB_NO_ACCESSION,
			"FlatMainAccession: Could not get main ACCESSION");
#else
			retval = StringSave("?00000");
#endif
   }
#ifdef ERROR_IF_NO_MAIN_ACCESSION
else{
#endif
			Flat_last_accession =  Flat_Clean_store (Flat_last_accession,
				 retval);
			Accession_magic_cookie =
				Nlm_ErrUserInstall (Flat_last_accession, 
					Accession_magic_cookie);
			Flat_out_index_line ( Flat_Index_accession, retval);
#ifdef ERROR_IF_NO_MAIN_ACCESSION
		}
#endif
   return retval;
}

/*------------ FlatXrefAccession()-------------*/

CharPtr
FlatXrefAccession ( SeqFeatPtr sfp, ValNodePtr location , 
		Int4Ptr lenPt, CharPtr temp)
{
	TextSeqIdPtr text ;
	SeqLocPtr xref;
	SeqIdPtr xid;

		if (SeqLocCompare(sfp -> location, location)
				!= SLC_NO_MATCH ){
			xref = (SeqLocPtr ) sfp -> data.value.ptrvalue;
			if (xref -> choice == SEQLOC_WHOLE){
				xid = (SeqIdPtr) xref -> data.ptrvalue;
				if (xid -> choice == 5 || xid -> choice == 6
						|| xid -> choice == 13){
					text = (TextSeqIdPtr) xid -> data.ptrvalue;
					if (lenPt){
						* lenPt += 1 + StringLen ( text -> accession);
					}
					if (temp){
						temp = StringMove(temp, " ");
						temp = StringMove(temp, text -> accession);
						Flat_out_index_line ( Flat_Index_accession, 
							text -> accession);
					}
				}
			}
		}

	return temp;
}

/*----------- FlatAllAccessions  ()------*/
CharPtr FlatAllAccessions 
(SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, 
		AsnOptionPtr optionHead)
{
		SEQ_ENTRY_STACK_PTR this_node;
   CharPtr retval = NULL ;
   BioseqPtr bs;
   ValNodePtr descr, xtras = NULL, more_acc;
   GBBlockPtr gb;
		EMBLBlockPtr eb;
		CharPtr mainacc;
		ValNodePtr location = NULL;
		SeqIntPtr si;
		SeqFeatPtr sfp;
		SeqAnnotPtr annot;
		CharPtr xref_accs=NULL, temp;
		Int4 len=0;

   bs = the_seq -> data.ptrvalue;
			location = ValNodeNew (NULL);
	
			si = SeqIntNew();
			location -> choice = SEQLOC_INT;
			location -> data.ptrvalue = si;
			
			si -> from = 0;
			si -> to = bs -> length -1;
			si -> id = bs -> id; /* WARNING may not now use normal free */

   descr = bs -> descr;
   if ( descr ){
      gb = FlatGenBankBlock(descr, optionHead);
      if ( gb ){
				if ( gb -> extra_accessions){
					xtras = gb -> extra_accessions;
				}
      }
			if (xtras == NULL){
				eb = FlatEMBLBlock (descr, optionHead);
				if (eb){
					xtras = eb -> extra_acc;
				}
			}
   }   

		mainacc =  FlatMainAccession(the_seq, optionHead);
		if (mainacc){
			for (this_node= the_nodes; this_node; 
					this_node = this_node -> last){
				for (annot=this_node -> annot_nodes ; 
							annot; annot = annot -> next){
						for ( sfp = (SeqFeatPtr) annot -> data;sfp; 
								sfp = sfp -> next){
							if ( sfp -> data.choice == 7){ /* xref feature */
								FlatXrefAccession ( sfp, location , & len, NULL);
							}
						}
					}
			}

			for (annot=bs -> annot ; annot; annot = annot -> next){
				for ( sfp = (SeqFeatPtr) annot -> data;sfp; 
						sfp = sfp -> next){
					if ( sfp -> data.choice == 7){ /* xref feature */
						FlatXrefAccession ( sfp, location , & len, NULL);
					}
				}
			}
			if (len > 0){
					temp = xref_accs = MemNew ((size_t)(len+1));
				for (this_node= the_nodes; this_node; 
						this_node = this_node -> last){
					for (annot=this_node -> annot_nodes ; 
								annot; annot = annot -> next){
							for ( sfp = (SeqFeatPtr) annot -> data;sfp; 
									sfp = sfp -> next){
								if ( sfp -> data.choice == 7){ /* xref feature */
									temp =FlatXrefAccession ( sfp, location , 
										NULL, temp);
								}
							}
						}
				}

				for (annot=bs -> annot ; annot; annot = annot -> next){
					for ( sfp = (SeqFeatPtr) annot -> data;sfp; 
							sfp = sfp -> next){
						if ( sfp -> data.choice == 7){ /* xref feature */
								temp =FlatXrefAccession ( sfp, location , 
									NULL, temp);
						}
					}
				}
			}
			retval = FlatStringGroup ( mainacc, xtras, " ", xref_accs, 
				optionHead);
			for ( more_acc = xtras; more_acc; more_acc = more_acc -> next){
				Flat_out_index_line ( Flat_Index_accession, 
					more_acc -> data.ptrvalue);
			}
			MemFree(mainacc);
		}

		if (location){
			si -> id = NULL;
			SeqIntFree(si);
			ValNodeFree(location);
		}
		MemFree(xref_accs);

   return retval;
}

/*----------- FlatAccession  ()------*/
FlatAccession 
(FILE *fp, SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, 
		AsnOptionPtr optionHead)
{
	CharPtr AllAccession=NULL;
	CharPtr  cont=NULL;
	CharPtr WindowsKludge, tempwin;
	int retval = TRUE;

		AllAccession = FlatAllAccessions (the_seq, the_nodes, optionHead);
		if (AllAccession){
			cont= FlatCont(AllAccession, FALSE, optionHead);

			MACRO_FLAT_LINE_OUT(tempwin,ACC_BASE_LTR,cont,WindowsKludge)
/*---
	fprintf(fp, "ACCESSION   %s\n", cont);
---*/
		}else{
			retval = FALSE;
		}
	
	fflush(fp);

	if (AllAccession)
		MemFree(AllAccession);

	if (cont)
		MemFree(cont);

	return retval;
}
/*----------- FlatBaseCount  ()------*/
FlatBaseCount 
(FILE *fp, SeqEntryPtr the_seq, AsnOptionPtr optionHead)
{
		int i, retval = TRUE;

		BioseqPtr bsp;
		Int4 counts[5] ;
    SeqPortPtr spp;
    Uint1 repr, code, residue;
		static first_call = 1;


		if (first_call){
			first_call =0;
			BioseqLoad();
		}

				if (! Flat_do_Flat) return retval;

	for ( i=0; i < 5; i ++) counts[i] = (Int4) 0;

	if( the_seq -> choice ==1){
      bsp = (BioseqPtr) the_seq -> data.ptrvalue; 
    repr = Bioseq_repr(bsp);
    if (! ((repr == Seq_repr_raw) || (repr == Seq_repr_const))){
		if ( !  (Flat_Be_quiet&1))
			ErrPost(CTX_NCBI2GB,CTX_2GB_BAD_REPR,
			"FlatBaseCount: bad repr %d\n", (int) repr);
			retval = FALSE;
			goto BYEBYE;
		}

    if ( ! ISA_na(bsp->mol) && ! Flat_format){
			goto BYEBYE;
		}

		if ( ISA_na(bsp->mol)){
     code = Seq_code_iupacna;

    spp = SeqPortNew(bsp, 0, -1, 0, code);
    SeqPortSeek(spp, 0, SEEK_SET);
    while ((residue = SeqPortGetResidue(spp)) != SEQPORT_EOF)
    {
			if (residue == SEQPORT_EOS || residue == SEQPORT_VIRT)
				break;
			if ( !IS_residue(residue) && residue != INVALID_RESIDUE)
				continue;
			switch ( residue){
		case 'A':
			counts[0] ++;
     break;
		case 'C':
			counts[1] ++;
     break;
		case 'G':
			counts[2] ++;
     break;
		case 'T':
			counts[3] ++;
     break;
		default:
			counts[4] ++;
			}
    }
    SeqPortFree(spp);
		}
/*---
BASE COUNT     1891 a   1236 c   1308 g   1878 t      1 others
---*/
		if ( ! counts[4] &&! Flat_format){
				if (Flat_do_Flat )
					if (Flat_format==0){
					fprintf(fp,
			"BASE COUNT  %7ld a%7ld c%7ld g%7ld t\n",
			counts[0], counts[1], counts[2], counts[3]);
				}else{
					if (ISA_na(bsp->mol)){
fprintf(fp,
"SQ   Sequence %ld BP; %ld a; %ld c; %ld g; %ld t\n",
 (long) (FlatLocusBases(the_seq,  optionHead)),
counts[0], counts[1], counts[2], counts[3]);
					}else{
fprintf(fp,
"SQ   Sequence %ld AA; \n",
 (long) (FlatLocusBases(the_seq,  optionHead)));
					}
				}
		}else{
				if (Flat_do_Flat )
					if (Flat_format==0){
					fprintf(fp,
			"BASE COUNT  %7ld a%7ld c%7ld g%7ld t%7ld others\n",
			counts[0], counts[1], counts[2], counts[3], counts[4]);
				}else{
					if (ISA_na(bsp->mol)){
		fprintf(fp,
"SQ   Sequence %ld BP;  %ld A; %ld C; %ld G; %ld T; %ld other\n",
 (long) (FlatLocusBases(the_seq,  optionHead)),
counts[0], counts[1], counts[2], counts[3], counts[4]);
					}else{
fprintf(fp,
"SQ   Sequence %ld AA; \n",
 (long) (FlatLocusBases(the_seq,  optionHead)));
					}
				}
		}

	fflush(fp);
	}else{
		if ( !  (Flat_Be_quiet&1))
			ErrPost(CTX_NCBI2GB, CTX_2GB_NOT_BIOSEQ,
			"FlatBaseCount: non-Bioseq SeqEntry\n");
			retval = FALSE;
			goto BYEBYE;
	}

	BYEBYE:
		return retval;
}

/*--------FlatXrefName () --------*/

CharPtr FlatXrefName 
(ValNodePtr node, int context)
{
	CharPtr retval = NULL;

	if ( context == XREF_BACKBONE ){
#ifdef USE_XREF_BACKBONE
	switch ( node -> choice){
		case 1:
			retval = StringSave("ddbj");
			break;
		case 2:
			retval = StringSave("carbbank");
			break;
		case 3:
			retval = StringSave("embl");
			break;
		case 4:
			retval = StringSave("hdb");
			break;
		case 5:
			retval = StringSave("genbank");
			break;
		case 6:
			retval = StringSave("hgml");
			break;
		case 7:
			retval = StringSave("mim");
			break;
		case 8:
			retval = StringSave("msd");
			break;
		case 9:
			retval = StringSave("pdb");
			break;
		case 10:
			retval = StringSave("pir");
			break;
		case 11:
			retval = StringSave("prfseqdb");
			break;
		case 12:
			retval = StringSave("psd");
			break;
		case 13:
			retval = StringSave("swissprot");
			break;
	}
#else
		ErrPost(CTX_NCBI2GB, CTX_2GB_XREF_BACKBONE, 
			"FlatXrefName: XREF_BACKBONE NOT ACTIVE, yet called");
#endif
	}else if (context ==XREF_OBJECT)  {
		switch ( node -> choice){
			case 5:
				retval = StringSave("genbank");
				break;
			case 6:
				retval = StringSave("embl");
				break;
			default:
			case 10:
				retval = StringSave("other");
				break;
			case 12:
				retval = StringSave("NCBI");
				break;
			case 13:
				retval = StringSave("ddbj");
				break;
	}
	}
	

	return retval;
}
/*-------- FlatCommentNum() ------------*/

CharPtr FlatCommentNum
(PubdescPtr pd)
{
	CharPtr retval = NULL, temp;
	NumContPtr ncp;
	ValNodePtr numnode;
	int len=0;
	char * base = "The authors begin their numbering at ";
	char * zero = "Zero is present in the numbering.";
	char * nozero = "Zero is not present in the numbering.";
	char * descending = "The numbering descends.";
	char num [10];

	if ( pd -> num){
		for (numnode = pd -> num; numnode; numnode = numnode -> next){
			if ( numnode -> choice == 1){
				ncp = (NumContPtr) numnode -> data.ptrvalue;
				if ( ncp -> refnum != 1 ||  ! ncp -> ascending){
					if ( ncp -> ascending){
						if (ncp -> refnum <= 0 ){
							if ( ncp -> has_zero){
								len += 4 + StringLen (zero);
							}else {
								len += 4 + StringLen(nozero);
							}
						}
					}else{
						len += 4 + StringLen(descending);
						if ( ncp -> has_zero){
							len += 2 + StringLen (zero);
						}else {
							len += 2 + StringLen(nozero);
						}
					}
					len += 9 + StringLen(base);
					
					temp = retval = MemNew(len +1);

					temp = StringMove(temp, base);
					sprintf(num,"%ld",(long) ncp -> refnum);
					temp = StringMove(temp, num);
					temp = StringMove(temp,".");
						if ( ncp -> ascending){
							if (ncp -> refnum <= 0 ){
								if ( ncp -> has_zero){
									temp = StringMove(temp, "  ");
									temp = StringMove(temp, zero);
								}else {
									temp = StringMove(temp, "  ");
									temp = StringMove(temp, nozero);
								}
							}
						}else{
							temp = StringMove(temp, "  ");
							temp = StringMove(temp, descending);
							if ( ncp -> has_zero){
								temp = StringMove(temp, "  ");
								temp = StringMove(temp, zero);
							}else {
								temp = StringMove(temp, "  ");
								temp = StringMove(temp, nozero);
							}
						}
				
				}
				break;
			}
		}
	}

	return retval;
}

/*----------- FlatCommentPubDesc() -----*/


void
FlatCommentPubDesc(PubdescPtr pd, FLAT_PUB_STORE_PTR refhead, CharPtr PNTR tempPt, Int4Ptr lengthPt, AsnOptionPtr optionHead)
{
	int pubnum;
	CharPtr cite ;
	Boolean new_sent = TRUE;
	CharPtr numbering = NULL;
	CharPtr temp = NULL;
	ValNodePtr pub;
	char buf [12];
	

	if (tempPt)
		temp = * tempPt;
	if (pd -> fig || pd -> poly_a || pd -> maploc ||numbering){
		pubnum = 0;
		if (refhead) if (refhead -> next){
			pubnum = FlatRefGet(pd, refhead);
			temp = FlatSmartStringMove (temp, lengthPt, temp,"According to ");
			new_sent = FALSE;
			if ( pubnum ==0){
				for(pub = pd -> pub; pub; pub = pub -> next){
					if (pub -> choice == 4) 
						continue; /* muid only */
					cite = FlatJournal (pd -> pub, 0, optionHead);
					break;
				}
				temp = FlatSmartStringMove (temp, lengthPt, temp,"[");
				temp = FlatSmartStringMove (temp, lengthPt, temp,cite);
				MemFree(cite);
				temp = FlatSmartStringMove (temp, lengthPt, temp,"]");
			}else{
				temp = FlatSmartStringMove(temp, lengthPt, temp, " Ref. ");
				sprintf(buf, "%ld", (long) pubnum);
				temp = FlatSmartStringMove (temp, lengthPt, temp,buf);
			}
			temp = FlatSmartStringMove (temp, lengthPt, temp,", ");
		}
		if (pd -> fig){
			if (new_sent){
				temp = FlatSmartStringMove (temp, lengthPt, temp,"This sequence ");
			}else{
				temp = FlatSmartStringMove (temp, lengthPt, temp,"this sequence ");
			}
			temp = FlatSmartStringMove (temp, lengthPt, temp, "comes from ");
			temp = FlatSmartStringMove (temp, lengthPt, temp, pd -> fig);
			temp = FlatSmartStringMove (temp, lengthPt, temp, ".");
			new_sent = TRUE;
		}
		if (pd -> poly_a){
			new_sent = TRUE;
			temp = FlatSmartStringMove (temp, lengthPt, temp,
"  Polyadenylate residues occuring in the figure were omitted from the sequence.");
		}
		if (pd -> maploc){
			if (new_sent){
				temp = FlatSmartStringMove (temp, lengthPt, temp,"  Map location: ");
			}else{
				temp = FlatSmartStringMove (temp, lengthPt, temp,"map location: ");
			}
			temp = FlatSmartStringMove (temp, lengthPt, temp, pd -> maploc);
			temp = FlatSmartStringMove (temp, lengthPt, temp, ".");
		}
		if (numbering){
			temp = FlatSmartStringMove (temp, lengthPt, temp, "  ");
			temp = FlatSmartStringMove (temp, lengthPt, temp, numbering);
		}
		temp = FlatSmartStringMove (temp, lengthPt, temp,"\n");
	}

	MemFree(numbering);
	if (tempPt)
		* tempPt = temp;
}

/*----------- FlatCommentDescr ()------*/

CharPtr FlatCommentDescr 
(ValNodePtr head, FLAT_PUB_STORE_PTR refhead, AsnOptionPtr optionHead)
{
	CharPtr temp, retval = NULL;
	Int4 len=0;
	ValNodePtr an;
	DbtagPtr the_dbtag;
	ObjectIdPtr oid;
	char buf [12];
	PubdescPtr pd;

   for (an = head; an; an = an -> next){
			if ( an -> choice == Seq_descr_region){ 
				len += 18 + StringLen( an -> data.ptrvalue);
			}else if ( an -> choice == Seq_descr_method){
				if (an->data.intvalue > 1)   /* not conceptual translation */
					len += 18 + StringLen(StringForSeqMethod((Int2) an->data.intvalue));
			}else if ( an -> choice == Seq_descr_maploc){ 
				the_dbtag = (DbtagPtr) an -> data.ptrvalue;
				len += 18;
				if (the_dbtag -> db){
					len += 18 + StringLen(the_dbtag -> db);
				}
				if (the_dbtag -> tag){
					oid = the_dbtag -> tag;
					if (oid -> id != 0){
						len += 12;
					}
					if (oid -> str){
						len += 5 + StringLen(oid -> str);
					}
				}
			}else if ( an -> choice == Seq_descr_comment){ 
				if (FlatSafeAsnThere(an )){
					len += 5 + StringLen( an -> data.ptrvalue);
				}
			}else if ( an -> choice == Seq_descr_pub){ 
				pd = ( PubdescPtr) an -> data.ptrvalue;
				FlatCommentPubDesc(pd, refhead, NULL, & len, optionHead);
			}
		}
	if (len > 0){
		retval = temp = MemNew((size_t)(len+3));
   for (an = head; an; an = an -> next){
			if ( an -> choice == Seq_descr_region){ 
				temp = StringMove(temp,"Region: ");
				temp = StringMove(temp, an -> data.ptrvalue);
				temp = StringMove(temp,".  ");
			}else if ( an -> choice == Seq_descr_method){
				if (an->data.intvalue > 1)   /* not conceptual translation */
				{
					temp = StringMove(temp,"Method: ");
					temp = StringMove(temp, StringForSeqMethod((Int2) an->data.intvalue));
					temp = StringMove(temp,".  ");
				}
			}else if ( an -> choice == Seq_descr_maploc){ 
				temp = StringMove(temp,"Map location: ");
				the_dbtag = (DbtagPtr) an -> data.ptrvalue;
				if (the_dbtag -> db){
					temp = StringMove(temp,"(Database ");
					temp = StringMove(temp, the_dbtag -> db);
					if ( ! the_dbtag -> tag){
						temp = StringMove(temp,") ");
					}
				}
				if (the_dbtag -> tag){
					oid = the_dbtag -> tag;
					if (oid -> id != 0){
						temp = StringMove(temp,"; id # ");
						sprintf(buf,"%ld",(long) oid -> id);
						temp = StringMove(temp,buf);
					}
					temp = StringMove(temp,") ");
					if (oid -> str){
						temp = StringMove(temp, oid -> str);
					}
				}
				temp = StringMove(temp,"\n");
			}else if ( an -> choice == Seq_descr_comment){ 
				if (FlatSafeAsnThere(an )){
					temp =  StringMove(temp, an -> data.ptrvalue);
					if ( ! FlatEndsPunc(an -> data.ptrvalue))
						temp = StringMove(temp,".");
					temp = StringMove(temp,"\n");
				}
			}else if ( an -> choice == Seq_descr_pub){ 
				pd = ( PubdescPtr) an -> data.ptrvalue;
				FlatCommentPubDesc(pd, refhead, &temp, NULL, optionHead);
			}
		}
	}


   return FlatCleanEquals(retval);
}

/*----------- FlatCommentOptions ()------*/

CharPtr 
FlatCommentOptions (Boolean had_comment, AsnOptionPtr optionHead)
{
	CharPtr temp, retval = NULL;
	int len=0;
	AsnOptionPtr this_option=NULL;

	while (( this_option = 
         AsnOptionGet(optionHead, OP_TOGENBNK, OP_TOGENBNK_COMMENT, 
					this_option)) != NULL){
				len += 3 + StringLen( this_option -> data.ptrvalue);
	}
	this_option= NULL;
	if (len > 0){
		retval = temp = MemNew(len+2);
		while (( this_option = 
				AsnOptionGet(optionHead, OP_TOGENBNK, 
					OP_TOGENBNK_COMMENT, this_option)) != NULL){
				if (had_comment)
					temp = StringMove(temp,"\n");
				temp = StringMove(temp,this_option -> data.ptrvalue);
		}
	}

   return FlatCleanEquals(retval);
}

/*----------- SeqLocSubset  ()------*/
/* -1 if no subset, 0 on match, else number of bases extra 
   in first parameter 
Note that the first parameter is the location from the
gene feature or the ProtRef feature.  The cdregion, 2nd parameter location,
must be a subset of the first for a non-negative return.  Also it would be
ok if you changed the order of the parameters, the name of the fuction
and the specific return values, I need to distinguish the three cases.
SeqLocCompare(a,b) which takes two seqlocs, a and b.  It has the following
integer returns
  SLC_NO_MATCH - no part of a and b overlap
  SLC_A_EQ_B   - a is identical to b
  SLC_A_IN_B   - a is completely contained in b
  SLC_B_IN_A   - b is completely contained in a
  SLC_A_OVERLAP_B - a and b overlap but one is not contained in the other

*/
Int4 SeqLocSubset 
(SeqLocPtr outer_loc, SeqLocPtr inner_loc)
{
	Int4 retval = -1;

	switch ( SeqLocCompare(outer_loc, inner_loc)){
		case SLC_A_EQ_B:
			retval = 0;
			break;
		case SLC_B_IN_A:
			retval = SeqLocLen(outer_loc) - SeqLocLen(inner_loc);
			break;
	}

	return retval;
}
	

/*----------- FlatCommentAnnot ()------*/

CharPtr FlatCommentAnnot 
(SeqAnnotPtr head, FLAT_PUB_STORE_PTR refhead, SeqEntryPtr the_seq, AsnOptionPtr optionHead)
{
   CharPtr retval = NULL, temp, p;
		SeqFeatPtr sfp;
		int dex, num_com = 0, num_ref=0;
		Int4 len = 0;
		ValNodePtr location = NULL;
		SeqIntPtr si;
		BioseqPtr bs;
		SeqIdPtr xid;
		SeqLocPtr xref;
		TextSeqIdPtr text;
		SeqAnnotPtr annot;
		char * preface = "Cross-reference: ";
		char printbuf[41];
		int rt;

		if (the_seq -> choice ==1){
			bs = the_seq -> data.ptrvalue;
			location = ValNodeNew (NULL);
	
			si = SeqIntNew();
			location -> choice = SEQLOC_INT;
			location -> data.ptrvalue = si;
			
			si -> from = 0;
			si -> to = bs -> length -1;
			si -> id = bs -> id; /* WARNING may not now use normal free */
		}

   for (annot=head ; annot; annot = annot -> next){
			for ( sfp = (SeqFeatPtr) annot -> data;sfp; sfp = sfp -> next){
				if ( sfp -> data.choice == 7){ /* xref feature */
					rt=SeqLocCompare(sfp -> location, location);
					if (rt == SLC_B_IN_A ||  rt == SLC_A_EQ_B){
						xref = (SeqLocPtr ) sfp -> data.value.ptrvalue;
						if (xref -> choice == SEQLOC_WHOLE){
							xid = (SeqIdPtr) xref -> data.ptrvalue;
							if (xid -> choice != 5 && xid -> choice != 6
									&& xid -> choice != 13){
								p=FlatXrefName(xid, XREF_OBJECT );
								len += 6 + StringLen(p);
								MemFree(p);
								SeqIdPrint(xid,printbuf,PRINTID_FASTA_SHORT);
								len += 30+ StringLen ( printbuf);
								if ( ! num_ref){
									len += StringLen(preface);
								}
								num_ref ++;
							}
						}
					}
				}else if ( sfp -> data.choice == 6 ){ /* pub == Pubdesc */
					FlatCommentPubDesc(( PubdescPtr) 
						sfp -> data.value.ptrvalue, refhead,
						 NULL, & len, optionHead);
				}else if ( sfp -> data.choice == 10){ /* Comment feature */
					if (SeqLocCompare(sfp -> location, location)
							== SLC_A_EQ_B ){
						len += 1+ StringLen(sfp ->comment);
						num_com ++;
					}
				}
			}
   }
	if (len > 0){
		retval = temp = MemNew ((size_t)( 1 + len));
		for (annot=head, dex = 0; annot; annot = annot -> next){
				for ( sfp = (SeqFeatPtr) annot -> data;sfp; 
						sfp = sfp -> next){
					if ( sfp -> data.choice == 7){ /* xref feature */
						rt=SeqLocCompare(sfp -> location, location);
						if (rt == SLC_B_IN_A ||  rt == SLC_A_EQ_B){
							xref = (SeqLocPtr ) sfp -> data.value.ptrvalue;
							if (xref -> choice == SEQLOC_WHOLE){
								xid = (SeqIdPtr) xref -> data.ptrvalue;
								if (xid -> choice != 5 && xid -> choice != 6
										&& xid -> choice != 13){
									if ( dex ==0){
										temp = StringMove(temp,preface);
									}
									p=FlatXrefName(xid, XREF_OBJECT );
									temp = StringMove(temp,p );
									temp = StringMove(temp,": ");
									MemFree(p);
									SeqIdPrint(xid,printbuf,PRINTID_FASTA_SHORT);
									temp = StringMove(temp,printbuf);
									dex ++;
									if ( dex == num_ref -1){
										temp = StringMove(temp,".\n");
									}else{
										temp = StringMove(temp,"; " );
									}
								}
							}
						}
					}

			
			}
		}
		for (annot=head, dex = 0; annot; annot = annot -> next){
				for ( sfp = (SeqFeatPtr) annot -> data;sfp; 
						sfp = sfp -> next){
					if ( sfp -> data.choice == 6){ /* Pubdesc feature */
					FlatCommentPubDesc(( PubdescPtr) 
						sfp -> data.value.ptrvalue, refhead,
						 &temp, NULL, optionHead);
					}
				}
		}
		for (annot=head, dex = 0; annot; annot = annot -> next){
				for ( sfp = (SeqFeatPtr) annot -> data;sfp; 
						sfp = sfp -> next){
					if ( sfp -> data.choice == 10){ /* Comment feature */
						if (SeqLocCompare(sfp -> location, location)
								!= SLC_NO_MATCH ){
							temp = StringMove(temp,sfp ->comment);
							dex ++;
							if ( dex < num_com)
								temp = StringMove(temp,"\n");
						}
					}
				}
		}
	}

		if (location){
			si -> id = NULL;
			SeqIntFree(si);
			ValNodeFree(location);
		}
   return FlatCleanEquals(retval);
}


/*----------- FlatComment  ()------*/
int FlatComment 
(FILE *fp, SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, FLAT_PUB_STORE_PTR refhead, AsnOptionPtr optionHead)
{
	BioseqPtr bs;
	ValNodePtr descr;
	CharPtr p, comment, com_cont = NULL;
	CharPtr temp,kludge;
	int retval = TRUE;
	Boolean had_comment = FALSE;
	CharPtr left_side;
	SeqIdPtr id;
	Int4 gi=0, gibbsq=0;

	   bs = the_seq -> data.ptrvalue;
/*------this code used to depend on:
		if (Flat_T10 || Flat_CdRom).........
---*/

			if ( the_seq -> choice == 1){
				for ( id = bs -> id; id; id = id -> next){
					if ( id -> choice == SEQID_GI){
						gi = id -> data.intvalue;
					}
					if ( id -> choice == SEQID_GIBBSQ){
						gibbsq = id -> data.intvalue;
					}
				}
			}

			if ( gibbsq > 0){
				char t10 [200];

				sprintf(t10,
#ifdef OLD_attribution
"This entry incorporates information provided by the National Library of Medicine's GenInfo Backbone as NCBI gibbsq %ld."
"This entry [NCBI gibbsq %ld] was created by the journal scanning component of GenBank at the National Library of Medicine."
#else
"GenBank staff at the National Library of Medicine created this entry [NCBI gibbsq %ld] from the original journal article."
#endif

, (long) gibbsq);
				com_cont = FlatCont(t10, FALSE, optionHead);
				MACRO_FLAT_LINE_OUT(temp,COM_BASE_LTR,com_cont,kludge)
				had_comment = TRUE;
				if (com_cont){
					MemFree(com_cont);
					com_cont =  NULL;
				}
			}

   descr = bs -> descr;
   if ( descr ){
         comment = FlatCommentDescr(descr, refhead, optionHead);
			if (comment ){
				if ( (Flat_Be_quiet&2)){
					for (p=comment; *p; p++){
						if (StringNCmp(p, "NCBI DETECTED", (size_t) 9) ==0){
							*p = '\0';
							break;
						}
					}
				}
				com_cont = FlatCont(comment, TRUE, optionHead);
				left_side = COM_BASE_LTR;
				if (had_comment)
					left_side = COM_BLANK_LTR;
				MACRO_FLAT_LINE_OUT(temp,left_side,com_cont,kludge)
			/*---
			*		fprintf(fp, "COMMENT     %s\n",com_cont);
			----*/
				had_comment = TRUE;
				if (com_cont){
					MemFree(com_cont);
					com_cont =  NULL;
				}
				MemFree(comment);
			}
		}
		
		if (bs -> annot){

			comment = FlatCommentAnnot(bs -> annot, refhead, the_seq, optionHead);

			if (comment){
				com_cont = FlatCont(comment, TRUE, optionHead);
				left_side = COM_BASE_LTR;
				if (had_comment)
					left_side = COM_BLANK_LTR;
				had_comment = TRUE;
				MACRO_FLAT_LINE_OUT(temp,left_side,com_cont,kludge)
				MemFree(comment);
			}
		}

	if (the_nodes)
		FlatSetComment(fp, the_seq, & had_comment, the_nodes, refhead, optionHead);

			comment = FlatCommentOptions(had_comment, optionHead);

			if (comment){
				com_cont = FlatCont(comment, TRUE, optionHead);
				left_side = COM_BASE_LTR;
				if (had_comment)
					left_side = COM_BLANK_LTR;
				had_comment = TRUE;
				MACRO_FLAT_LINE_OUT(temp,left_side,com_cont,kludge)
				MemFree(comment);
			}

/*---release 81 interum NCBI gi formatting---*/
	if (gi > 0){
				char ncbi_gi [200];

				sprintf(ncbi_gi, "NCBI gi: %ld", (long) gi);
				com_cont = FlatCont(ncbi_gi, FALSE, optionHead);
				left_side = COM_BASE_LTR;
				if (had_comment)
					left_side = COM_BLANK_LTR;
				had_comment = TRUE;
				MACRO_FLAT_LINE_OUT(temp,left_side,com_cont,kludge)
				had_comment = TRUE;
				if (com_cont){
					MemFree(com_cont);
					com_cont =  NULL;
				}
	}
	if (com_cont)
		MemFree(com_cont);

	fflush(fp);

	return retval;
}

/*----------- FlatTitleNode ()------*/
ValNodePtr FlatTitleNode 
(ValNodePtr head, AsnOptionPtr optionHead)
{
   ValNodePtr retval = NULL;

   for (; head; head = head -> next){
      if ( head -> choice == 5){
         retval = head;
         break;
      }
   }   

   return retval;
}


/*----------- FlatTitle ()------*/
CharPtr FlatTitle 
(SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, AsnOptionPtr optionHead)
{
	CharPtr retval = NULL;
	BioseqPtr bs;
	ValNodePtr descr, title;
	SEQ_ENTRY_STACK_PTR a_node;

	bs = the_seq -> data.ptrvalue;

	descr = bs -> descr;
	if ( descr ){
		title = FlatTitleNode(descr, optionHead);
		if ( title ){
			retval = (CharPtr) title -> data.ptrvalue;
		}else{
			for(a_node=the_nodes; a_node && ! title;
					a_node = a_node -> last){
				title = FlatTitleNode(a_node -> desc_nodes ,optionHead);
			}
			if ( title ){
				retval = (CharPtr) title -> data.ptrvalue;
			}
		}
	}

	return FlatCleanEquals(retval);
}

/*---------- FlatDefPat ()---------*/

CharPtr
FlatDefPat (SeqEntryPtr the_seq, AsnOptionPtr optionHead)
{
	CharPtr retval = NULL;
	BioseqPtr bs;
	SeqIdPtr sip;
	PatentSeqIdPtr psip;
	IdPatPtr ipp;
	Int4 len = 0;
	CharPtr temp=NULL;
	int try;
	Int4Ptr lenPt_use = &len;
	CharPtr buf_use = NULL;
	char nbuf[20];

	if (the_seq -> choice == 1){
		bs = (BioseqPtr) the_seq -> data.ptrvalue;
		for (sip = bs -> id; sip; sip = sip -> next) {
			if (sip -> choice == 9){
				psip = (PatentSeqIdPtr) sip -> data.ptrvalue;
				ipp = psip -> cit;
				sprintf(nbuf,"Sequence %d ", (int) psip -> seqid);
				for (try = 0; try < 2; try ++){
					temp = FlatSmartStringMove(buf_use, lenPt_use, temp,nbuf);
					temp = FlatSmartStringMove(buf_use, lenPt_use, temp, 
						"from patent ");
					temp = FlatSmartStringMove(buf_use, lenPt_use, temp, ipp -> country);
					temp = FlatSmartStringMove(buf_use, lenPt_use, temp, " ");
					temp = FlatSmartStringMove(buf_use, lenPt_use, temp, ipp -> number);
					temp = FlatSmartStringMove(buf_use, lenPt_use, temp, ipp -> app_number);
					if ( try == 1)
						break;
					lenPt_use = NULL;
					retval = temp = buf_use = MemNew((size_t)(len +1));
				}
				
				break;
			}
		}
	}

	return retval;
}

/*----------- FlatDefinition  ()------*/
int FlatDefinition 
(FILE *fp, SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, AsnOptionPtr optionHead)
{
	CharPtr pre_def= FlatTitle(the_seq, the_nodes, optionHead);
	CharPtr def ;
	CharPtr temp,kludge;
	int retval = TRUE;

	if (pre_def == NULL){
		pre_def = FlatDefPat (the_seq, optionHead);
	}
	if (pre_def == NULL){
		pre_def = ".";
	}
	def = FlatCont(pre_def, FALSE, optionHead);

					MACRO_FLAT_LINE_OUT(temp,DEF_BASE_LTR,def,kludge)
/*---
	fprintf(fp,
"DEFINITION  %s\n", def);
 ---*/
	fflush(fp);

	if (def)
		MemFree(def);
	return retval;
}

/*----------- IS_minus_strand ()------*/
int IS_minus_strand 
(SeqLocPtr location, AsnOptionPtr optionHead)
{
	int retval = FALSE;
	SeqIntPtr sip;
	SeqPntPtr spp;
	PackSeqPntPtr pspp;
	ValNodePtr temp_node;

	switch ( location -> choice){
		case  SEQLOC_NULL:
		case  SEQLOC_EMPTY:
		case  SEQLOC_WHOLE:
		case SEQLOC_BOND:
		case SEQLOC_FEAT:
			break;
		case  SEQLOC_PNT:
			spp = (SeqPntPtr) location -> data.ptrvalue;
			retval = ( spp -> strand == Seq_strand_minus)?TRUE:FALSE;
			break;
		case  SEQLOC_INT:
			sip =  (SeqIntPtr) location -> data.ptrvalue;
			retval = ( sip -> strand == Seq_strand_minus)?TRUE:FALSE;
		break;
		case SEQLOC_PACKED_INT:
		case SEQLOC_MIX:
		case SEQLOC_EQUIV:
			for ( temp_node =  (ValNodePtr) location -> data.ptrvalue;
				 temp_node; temp_node = (temp_node) -> next){
	/* ignore next set of NULL entries  or pointers to null entries */
				if ( (temp_node) -> choice != SEQLOC_NULL){
						break;
				}
			}
			if (temp_node){
				sip =  (SeqIntPtr) temp_node -> data.ptrvalue;
				retval = ( sip -> strand == Seq_strand_minus)?TRUE:FALSE;
			}
			break;
		case  SEQLOC_PACKED_PNT:
		pspp = (PackSeqPntPtr) ( location -> data.ptrvalue);
		retval = ( pspp -> strand == Seq_strand_minus)?TRUE:FALSE;
			break;
	}
	
	return retval;
}

/*----------- SeqFirstLocId  ()------*/
SeqIdPtr SeqFirstLocId 
(SeqLocPtr location, AsnOptionPtr optionHead)
{
	SeqIdPtr retval = NULL;
	SeqIntPtr sip;
	SeqPntPtr spp;
	PackSeqPntPtr pspp;
	ValNodePtr temp_node;

	switch ( location -> choice){
		case  SEQLOC_NULL:
		case SEQLOC_BOND:
		case SEQLOC_FEAT:
			break;
		case  SEQLOC_EMPTY:
		case  SEQLOC_WHOLE:
			retval = (SeqIdPtr)location->data.ptrvalue;
			break;
		case  SEQLOC_PNT:
			spp = (SeqPntPtr) location -> data.ptrvalue;
			retval = ( spp -> id);
			break;
		case  SEQLOC_INT:
			sip =  (SeqIntPtr) location -> data.ptrvalue;
			retval = ( sip -> id);
		break;
		case SEQLOC_PACKED_INT:
		case SEQLOC_MIX:
		case SEQLOC_EQUIV:
			for ( temp_node =  (ValNodePtr) location -> data.ptrvalue;
				 temp_node; temp_node = (temp_node) -> next){
	/* ignore next set of NULL entries  or pointers to null entries */
				if ( (temp_node) -> choice != SEQLOC_NULL){
						break;
				}
			}
			if (temp_node){
				sip =  (SeqIntPtr) temp_node -> data.ptrvalue;
				retval = ( sip -> id);
			}
			break;
		case  SEQLOC_PACKED_PNT:
		pspp = (PackSeqPntPtr) ( location -> data.ptrvalue);
		retval = ( pspp -> id);
			break;
	}
	
	return retval;
}

/*----------------- FlatGBQualsCntinAnnot()-------------*/

void
FlatGBQualsCntinAnnot(SeqEntryPtr the_seq, SeqAnnotPtr annot, AsnOptionPtr PNTR optionHead, Int4Ptr cnt_gene, Int4Ptr cnt_map)
{
	SeqFeatPtr sfp;
	GBQualPtr qual;

	for ( sfp = (SeqFeatPtr) annot -> data;
			sfp; sfp = sfp -> next){
		for (qual = sfp -> qual; qual; qual = qual -> next){
			if( StringCmp(qual -> qual, "gene" ) == 0 ){
					(*cnt_gene) ++;
			}else if( StringCmp(qual -> qual, "map" ) == 0 ){
					(*cnt_map) ++;
			}
				
		}
	}

}

/*----------------- FlatGBQualsGetinAnnot()-------------*/

void
FlatGBQualsGetinAnnot(SeqEntryPtr the_seq, SeqAnnotPtr annot, AsnOptionPtr PNTR optionHead, CharPtr PNTR gene_quals, Int4Ptr cnt_gene, CharPtr PNTR map_quals, Int4Ptr cnt_map)
{
	SeqFeatPtr sfp;
	GBQualPtr qual;

	for ( sfp = (SeqFeatPtr) annot -> data;
			sfp; sfp = sfp -> next){
		for (qual = sfp -> qual; qual; qual = qual -> next){
			if( StringCmp(qual -> qual, "gene" ) == 0 ){
					gene_quals[*cnt_gene] = qual -> val;
					(*cnt_gene) ++;
			}else if( StringCmp(qual -> qual, "map" ) == 0 ){
					map_quals[*cnt_map] = qual -> val;
					(*cnt_map) ++;
			}
				
		}
	}

}

/*----------- FlatIndirectStringCmp ()---------*/

int LIBCALLBACK FlatIndirectStringCmp (VoidPtr v1, VoidPtr v2)
{
	CharPtr PNTR p1, PNTR p2;

	p1 = (CharPtr PNTR)v1;
	p2 = (CharPtr PNTR)v2;
	return StringCmp ( *p1, * p2);
}

/*----------- FlatFeatures  ()------*/
int FlatFeatures 
(FILE *fp, SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR head_nodes, FLAT_PUB_STORE_PTR refhead, AsnOptionPtr PNTR optionHead)
{
	BioseqPtr bs;
	SeqAnnotPtr annot;
	Int2 feat_count=0;
	int retval = TRUE;
	AsnOptionPtr qual_opt=NULL;
	Int4 num_gene_qual=0, num_map_qual=0;
	CharPtr PNTR gene_quals= NULL;
	CharPtr PNTR map_quals= NULL;
	DataVal asndata;
	SEQ_ENTRY_STACK_PTR the_nodes;
	GeneQualKnownPtr genes, maps;

	bs = the_seq -> data.ptrvalue;


/*--- send list of all unique map and gene quals to options --*/
	/*--- first count them ---*/
	for ( annot = bs -> annot; annot; annot = annot -> next){
		if (annot -> type != 1)
			continue;
		FlatGBQualsCntinAnnot(the_seq, annot, optionHead, 
				& num_gene_qual, & num_map_qual);
	}
	for (the_nodes = head_nodes; the_nodes && retval; the_nodes = the_nodes -> last){
      annot = the_nodes -> annot_nodes;
			if (annot){
				if (annot -> type != 1)
					continue;
				FlatGBQualsCntinAnnot(the_seq, annot, optionHead, 
					& num_gene_qual, & num_map_qual);
			}
	}
/*--- leave space for NULL at end ---*/
	if (num_gene_qual){
		gene_quals =  MemNew(sizeof(CharPtr) * (size_t)(num_gene_qual+1));
	}
	if (num_map_qual){
		map_quals =  MemNew(sizeof(CharPtr) * (size_t)(num_map_qual+1) );
	}
	/*-------now get them ---*/
	num_gene_qual=num_map_qual=0;
	for ( annot = bs -> annot; annot; annot = annot -> next){
		if (annot -> type != 1)
			continue;
		FlatGBQualsGetinAnnot(the_seq, annot, optionHead, 
				gene_quals, & num_gene_qual, map_quals, & num_map_qual);
	}
	for (the_nodes = head_nodes; the_nodes && retval; the_nodes = the_nodes -> last){
      annot = the_nodes -> annot_nodes;
			if (annot){
				if (annot -> type != 1)
					continue;
				FlatGBQualsGetinAnnot(the_seq, annot, optionHead, 
					gene_quals,& num_gene_qual, 
					map_quals, &num_map_qual);
			}
	}
/*--- now sort them ----*/
	HeapSort ((VoidPtr)gene_quals, (size_t)num_gene_qual, sizeof(CharPtr), FlatIndirectStringCmp);
	HeapSort ((VoidPtr)map_quals, (size_t)num_map_qual, sizeof(CharPtr), FlatIndirectStringCmp);

	genes = MemNew(sizeof(GeneQualKnown));
	genes -> count = num_gene_qual;
	genes -> array = gene_quals;
	asndata.ptrvalue = genes;
	AsnOptionNew ( optionHead, OP_TOGENBNK,
		OP_TOGENBNK_GENE_GBQUAL, asndata, DefAsnOptionFree);

	maps = MemNew(sizeof(GeneQualKnown));
	maps -> count = num_map_qual;
	maps -> array = map_quals;
	asndata.ptrvalue = maps;
	AsnOptionNew ( optionHead, OP_TOGENBNK,
		OP_TOGENBNK_MAP_GBQUAL, asndata, DefAsnOptionFree);

	for ( annot = bs -> annot; annot; annot = annot -> next){
		if (annot -> type != 1)
			continue;
		retval = FlatAnnot(fp, the_seq, annot, & feat_count, FALSE, 
			refhead, optionHead);
   if ( retval == FALSE)
		goto BYEBYE;
	}
			
	if (head_nodes){
		retval = FlatSetFeats (fp, the_seq, &feat_count, head_nodes, 
			refhead, optionHead);
	}

	FlatInsureSourceFeature(fp, the_seq, & feat_count, * optionHead);
	BYEBYE:
		fflush(fp);

		AsnOptionFree (optionHead, OP_TOGENBNK, 
			OP_TOGENBNK_GENE_GBQUAL);
		AsnOptionFree (optionHead, OP_TOGENBNK, 
			OP_TOGENBNK_MAP_GBQUAL);
		MemFree(gene_quals);
		MemFree(map_quals);
		return retval;
}

/*----------- FlatSetFeats ()------*/
int FlatSetFeats 
(FILE *fp, SeqEntryPtr the_seq, Int2Ptr feat_num, SEQ_ENTRY_STACK_PTR the_nodes, FLAT_PUB_STORE_PTR refhead, AsnOptionPtr PNTR optionHead)
{
   SeqAnnotPtr annot;
		int retval = TRUE;

	for (; the_nodes && retval; the_nodes = the_nodes -> last){
      annot = the_nodes -> annot_nodes;
			if (annot){
				if (annot -> type != 1)
					continue;
				retval = FlatAnnot(fp, the_seq, annot, feat_num, TRUE, 
					refhead, optionHead);
			}
	}
	return retval;
}

/*----------- FlatAuthors ()------*/
CharPtr 
FlatAuthors 
(ValNodePtr the_pub, AuthListPtr ap, AsnOptionPtr optionHead)
{
	CharPtr tmp, retval = NULL;
	ValNodePtr spare, this_name, namehead;
	CitArtPtr ca;
	CitBookPtr cb;
	CitGenPtr cg;	
	MedlineEntryPtr ml;
	CitSubPtr cs;
	CharPtr ret_save, clean_temp, temp;
	CitPatPtr cp;
	int len;

	if (ap == NULL){
	switch ( the_pub -> choice){

	case 9:
		cp = (CitPatPtr) the_pub -> data.ptrvalue;
		ap = cp  -> authors;
		break;
	case 11:
		cb = (CitBookPtr) the_pub -> data.ptrvalue;
		ap = cb -> authors;
		break;
	case 5:
	case 3:
		if ( the_pub -> choice == 3){
			ml = (MedlineEntryPtr) the_pub -> data.ptrvalue;
			ca = (CitArtPtr) ml -> cit;
		}else{
			ca = (CitArtPtr) the_pub -> data.ptrvalue;
		}
		ap = ca -> authors;
		break;
	case 1: 
		cg = (CitGenPtr) the_pub -> data.ptrvalue;
		ap = cg -> authors;
		break;
	case 2:
		cs = (CitSubPtr)  the_pub -> data.ptrvalue;
		ap = cs -> authors;
		break;
	default:
		if ( !  (Flat_Be_quiet&1))
		ErrPost(CTX_NCBI2GB, CTX_2GB_NOT_IMPLEMENTED,
		"FlatAuthors: Unimplemented pub type=%d\n", 
		(int) the_pub -> choice);
	}
	}

	if (ap){
		if (ap->choice != 1){   /* just strings */
			for (spare = ap->names, namehead = NULL; 
					spare; spare = spare -> next){
				this_name = ValNodeNew(namehead);
				if (namehead == NULL)
					namehead = this_name;
				this_name -> data.ptrvalue = StringSave(spare -> data.ptrvalue);
				if (Flat_format){
					for (tmp = (CharPtr) this_name->data.ptrvalue; *tmp; tmp ++){
						if (*tmp == ','){
							*tmp = ' ';
							break;
						}
					}
				}
			}
		} else                  /* structured names */
			namehead = GBGetAuthNames(ap);
		retval = FlatStringGroupTerm(NULL, namehead, ", ", Flat_format?";":NULL, 
			Flat_format?", ":" and ", optionHead);
		if (Flat_Index_author)
		for ( spare = namehead; spare ; spare = spare -> next){
			if (StringCmp("et,al.", spare -> data.ptrvalue) != 0){
				clean_temp = StringSave (spare -> data.ptrvalue );
				ret_save = FlatCleanEquals(clean_temp) ;
				Flat_out_index_line ( Flat_Index_author, ret_save);
				MemFree(ret_save);
			}
		}

  		/* ALL name now */
			ValNodeFreeData(namehead);     /* remove name list */
	}
	if (retval) {
		len = StringLen(retval);
		if (len > 0){
			if ( retval[len-1] != '.' && Flat_format ==0) {
				ret_save = retval;
				temp = retval = MemNew(len+2);
				temp = StringMove(temp,ret_save);
				temp = StringMove(temp,".");
				MemFree (ret_save);
			}
		}
	}

	return FlatCleanEquals(retval);
}

/*----------- FlatPubTitle ()------*/
CharPtr FlatPubTitle 
(ValNodePtr the_pub, AsnOptionPtr optionHead)
{
	CharPtr retval = NULL;
	CitArtPtr ca;
	CitBookPtr book;
	CitGenPtr cg;
	CharPtr str_ret, p;
	MedlineEntryPtr ml;
	CitPatPtr cp;

	switch ( the_pub -> choice){

	case 9:
		cp = (CitPatPtr) the_pub -> data.ptrvalue;
		retval = StringSave(cp -> title);
		break;
	case 11:
	book = (CitBookPtr) the_pub -> data.ptrvalue;
	if (book ->  title )
         if (book -> title -> data.ptrvalue)
            retval = StringSave(book -> title -> data.ptrvalue);
      break;

	case 5:
	case 3:
		if ( the_pub -> choice == 3){
			ml = (MedlineEntryPtr) the_pub -> data.ptrvalue;
			ca = (CitArtPtr) ml -> cit;
		}else{
			ca = (CitArtPtr) the_pub -> data.ptrvalue;
		}
		if (ca -> title )
			if (ca -> title -> data.ptrvalue)
				retval = StringSave(ca -> title -> data.ptrvalue);
		break;
	case 1: 
		cg = (CitGenPtr) the_pub -> data.ptrvalue;
		if (cg->title)
			retval = StringSave(cg->title);
		else
		{
			str_ret = NULL;
			if (cg -> cit)
				str_ret = StrStr(cg -> cit ,"Title=\"");
			if (str_ret){
				retval = StringSave(str_ret + 7);
				for (p=retval; *p; p++)
					if (*p == '"'){
						*p = '\0';
						break;
					}
			}
		}
		break;
	case 2: /* submission */
		retval = StringSave(Flat_format?"":"Direct Submission");
		break;
	default:
		if ( !  (Flat_Be_quiet&1))
		ErrPost(CTX_NCBI2GB, CTX_2GB_NOT_IMPLEMENTED,
		"FlatPubTitle: Unimplemented pub type=%d\n", 
		(int) the_pub -> choice);
		
	}
	retval =  FlatCleanEquals(retval);
	if(retval) if (*retval)
	for (p=retval + StringLen(retval)-1 ; p> retval+2; p--){
		if (*p == ' '){
			*p = '\0';
		}else if (*p == '.'){
			Boolean remove_it = FALSE;
			if ( p > retval +5){
				if ( *(p -1 ) != '.' || * (p-2) != '.')
					remove_it = TRUE;
			}
			if (remove_it)
				*p = '\0';
			break;
		}else{
			break;
		}
	}
	return (retval);
}

/*----------- FlatJournal ()------*/
CharPtr FlatJournal 
(ValNodePtr the_pub, Int4 pat_seqid, AsnOptionPtr optionHead)
{
	CharPtr retval = NULL;
	CitArtPtr ca;
	CitGenPtr cg;
	CitSubPtr cs;
	CitJourPtr jp;
	ImprintPtr ip;
	DatePtr dp;
	CharPtr date_std = NULL;
	char year[5];
	CharPtr str_ret, p, unp = NULL;
	CharPtr temp, dup_pages=NULL;
	MedlineEntryPtr ml;
	size_t len_page = 0;
	CitBookPtr book , cb;
	AuthListPtr ap = NULL;
	CharPtr book_authors = NULL;
	CharPtr book_tit = NULL;
	ValNodePtr namehead;
	int aflen=0;
	CitPatPtr cp;
	int patlen;
	AffilPtr afp ;
	AffilPtr citgen_afp = NULL;
	char buf[10];
	CharPtr pat_date = NULL;
	Boolean first_std = TRUE;
	Boolean found_journal = FALSE;
	Boolean used_cit = FALSE;

	year[0] = '\0';
	switch ( the_pub -> choice){
  /* Jill: Cit-sub */
    case 2:
      cs = (CitSubPtr) the_pub -> data.ptrvalue;
  
      ip = cs -> imp;
      if (ip ->date){
         dp = ip -> date;
         date_std = FlatDateFromCreate (NULL, (NCBI_DatePtr) dp);
      }
   
			afp = NULL;
			if ( ip -> pub){

				afp = ip -> pub;
				aflen = 10;   /* "Submitted " */
			}
			if (afp == NULL){
				if (cs -> authors)
					if (cs -> authors -> affil)
						afp = cs -> authors -> affil;
			}

			if (afp){
				if (afp -> choice == 1){
					if (afp -> affil){
						aflen += StringLen(afp -> affil);
					}
				}else if (afp -> choice == 2){
					aflen += 25 +
						StringLen (afp -> affil) + 
						StringLen (afp -> div) + 
						StringLen (afp -> city) + 
						StringLen (afp -> sub) + 
						StringLen (afp -> street) + 
						StringLen (afp -> country);
				}
			}
  
      temp = retval = MemNew( (size_t) (60 + 2 +
                    StringLen( date_std) + aflen) );
      temp = StringMove(temp,"Submitted");
      temp = StringMove(temp," (");
      temp = StringMove(temp,date_std);
      temp = StringMove(temp,") ");
			if (Flat_format){
				temp = StringMove(temp," on tape to the EMBL Data Library by:\n");
			}
  
			if ( afp){

				if ( afp -> choice == 1){
					 if (afp -> affil){
						 temp = StringMove(temp, afp -> affil);
					 }
				}else if (afp -> choice == 2){
					Boolean need_comma = FALSE;

					if( afp -> div) { 
						if ( need_comma){
							temp = StringMove(temp,", ");
						}
						temp = StringMove (temp, afp -> div) ; 
						need_comma = TRUE;
					}

					if(afp -> affil) { 
						if ( need_comma){
							temp = StringMove(temp,", ");
						}
						temp = StringMove(temp,afp -> affil) ; 
						need_comma = TRUE;
					}

					if(afp -> street) { 
						if ( need_comma){
							temp = StringMove(temp,", ");
						}
						temp = StringMove (temp,afp -> street) ; 
						need_comma = TRUE;
					}
					if( afp -> city) { 
						if ( need_comma){
							temp = StringMove(temp,", ");
						}
						temp = StringMove (temp, afp -> city) ; 
						need_comma = TRUE;
					}

					if( afp -> sub) { 
						if ( need_comma){
							temp = StringMove(temp,", ");
						}
						temp = StringMove (temp, afp -> sub) ; 
						need_comma = TRUE;
					}

					if( afp -> country){
						if ( need_comma){
							temp = StringMove(temp,", ");
						}
						temp = StringMove (temp, afp -> country);
						need_comma = TRUE;
					}	
				}
			}
      break;
	case 9: /* Patent */
		cp = (CitPatPtr) the_pub -> data.ptrvalue;
		if (cp){
			if (Flat_format == ASN2GNBK_FORMAT_PATENT_EMBL){
#ifdef ASN30
		retval = FlatStringGroupTerm(NULL, cp -> _class, ", ", ";", 
		", ", optionHead);
#else
				patlen = 3 + StringLen( cp -> _class);
#endif
			}else{
				patlen = 50;
				patlen += StringLen(cp -> country) ;
				if (cp -> number){
					patlen += StringLen(cp -> number);
				}else if (cp -> app_number){
					patlen += StringLen(cp -> app_number);
				}
				if ( cp -> doc_type){
					patlen += StringLen(cp -> doc_type);
				}
				ap = cp -> authors;
				if (ap){
					afp = ap -> affil;
					if (afp){
						patlen += StringLen(afp -> affil);
						if (afp -> choice == 2){
							patlen += StringLen(afp -> div);
							patlen += StringLen(afp -> city);
							patlen += StringLen(afp -> sub );
							patlen += StringLen(afp -> country);
							patlen += StringLen(afp -> street);
						}
					}
				}
			}
			
#ifdef ASN30
			if (Flat_format != ASN2GNBK_FORMAT_PATENT_EMBL){
#endif
			temp = retval = MemNew(patlen);
			temp = StringMove(temp,Flat_format == 0?"Patent: ":"Patent number ");
			temp = StringMove(temp,cp -> country);
			if (Flat_format ==0)
				temp = StringMove(temp," ");
			if (cp -> number){
				temp = StringMove(temp, cp -> number);
			}else if (cp -> app_number){
				temp = StringMove(temp,"(");
				temp = StringMove(temp, cp -> app_number);
				temp = StringMove(temp,")");
			}
			if ( cp -> doc_type) if ( * (cp -> doc_type)){
				temp = StringMove(temp,"-");
				temp = StringMove(temp, cp -> doc_type);
			}
			sprintf(buf,"%s%d ",Flat_format == 0?" ":"/", pat_seqid);
			temp = StringMove(temp,buf);
			if (cp -> date_issue){
				pat_date = FlatDateFromCreate(NULL, cp -> date_issue);
			}else if (cp -> app_date){
				pat_date = FlatDateFromCreate(NULL, cp -> app_date);
			}
			if (pat_date){
				temp = StringMove(temp,pat_date);
			}
			temp = StringMove(temp,Flat_format == 0?";\n":".\n");
			ap = cp -> authors;
			if (ap){
				afp = ap -> affil;
				if (afp){
					temp = StringMove(temp,afp -> affil);
					if (afp -> choice == 2){
						if (afp -> affil)
							temp = StringMove(temp,";\n");
						if (afp -> street){
							temp = StringMove(temp,afp -> street);
							temp = StringMove(temp,";\n");
						}
						if (afp -> div){
							temp = StringMove(temp,afp -> div);
							temp = StringMove(temp,";\n");
						}
						if (afp -> city){
							temp = StringMove(temp,afp -> city);
							temp = StringMove(temp,", ");
						}
						if (afp -> sub){
							temp = StringMove(temp,afp -> sub );
							temp = StringMove(temp,";\n");
						}
						if (afp ->country){
							temp = StringMove(temp,afp -> country);
							temp = StringMove(temp,";");
						}
					}
				}
			}
			
			MemFree(pat_date);
#ifdef ASN30
			}
#endif
		}
		break;
	/* end Patent */
	case 11: /* thesis */
	cb = (CitBookPtr) the_pub -> data.ptrvalue;
	if ( cb -> othertype == 2 && cb -> let_type == 3){ /* match thesis */
		CharPtr p;
		ip = cb -> imp;
		dp = ip -> date;
		if ( dp -> data[0] == 1){
			sprintf(year,"%d",(int) ( 1900+dp -> data[1]));
		}else{
			StringNCpy( (CharPtr) year, (CharPtr) dp -> str, (size_t) 4);
			year[4] = '\0';
		}
		ap = cb -> authors;
		if (ap->choice != 1)   /* just strings */
			namehead = ap->names;
		else                  /* structured names */
			namehead = GBGetAuthNames(ap);
		book_authors = FlatStringGroupTerm(NULL, namehead, ", ", NULL, 
		" and ", optionHead);
		if (ap->choice == 1)    /* std names */
			ValNodeFreeData(namehead);
			
		book_tit = StringSave(cb -> title -> data.ptrvalue);
		if ( ip -> pub){
			afp = ip -> pub;
			aflen = 7;

			if (afp -> choice == 1){
				if (afp -> affil){
					aflen += StringLen(afp -> affil) + 6;
				}

			}else{
				aflen = 22;
			}
		}
		if (ip->prepub == 2)   /* In press */
			aflen += 10;

		temp = retval = MemNew( (size_t) (60+
			 StringLen( year) + aflen) );
/*--make book title all caps */

		for ( p = book_tit; *p; p++){
			*p = TOUPPER(*p);
		}
		temp = StringMove(temp,"Thesis");
		temp = StringMove(temp," (");
		temp = StringMove(temp,year);
		temp = StringMove(temp,")");

		if ( ip -> pub){
			first_std = TRUE;
			afp = ip -> pub;

			if ( afp -> choice == 1){
				if (afp -> affil){
					if (StringLen( afp -> affil) > 7){
						temp = StringMove(temp, " ");
						temp = StringMove(temp, afp -> affil);
						if (ip->prepub == 2)   /* In press */
							temp = StringMove(temp, ", In press");
						first_std = FALSE;
					}
				}
			}

		}else{
			temp = StringMove(temp,"\naddress unknown, (");
		}
	}

	break;
/* end Cit-let == Thesis */	

	case 5:
	case 3:
		if ( the_pub -> choice == 3){
			ml = (MedlineEntryPtr) the_pub -> data.ptrvalue;
			ca = (CitArtPtr) ml -> cit;
		}else{
			ca = (CitArtPtr) the_pub -> data.ptrvalue;
		}
		if ( ca -> from != 1){
				book = (CitBookPtr) ca -> fromptr;
				ip = book -> imp;
		}else{
			jp = (CitJourPtr) ca -> fromptr;
			ip = jp -> imp;
		}
		dp = ip -> date;
		if ( dp -> data[0] == 1){
			sprintf(year,"%d",(int) ( 1900+dp -> data[1]));
		}else{
			StringNCpy( (CharPtr) year, (CharPtr) dp -> str, (size_t) 4);
			year[4] = '\0';
		}

		if (ip -> pages){
			len_page = StringLen( ip -> pages);
			dup_pages = MemNew ( 2*len_page );
			fix_pages(dup_pages, ip ->pages);
		}

		if ( ca -> from != 1){
			CharPtr p;
			ap = book -> authors;
			if (ap->choice != 1)   /* just strings */
				namehead = ap->names;
			else                  /* structured names */
				namehead = GBGetAuthNames(ap);
			book_authors = FlatStringGroupTerm(NULL, namehead, ", ", NULL, 
			" and ", optionHead);
				
			book_tit = StringSave(book -> title -> data.ptrvalue);
			
			if ( ip -> pub){
				afp = ip -> pub;
				aflen = StringLen(afp -> affil)+ 5;
				if ( afp -> choice == 2){
					aflen += 3 + StringLen(afp -> div);
					aflen += 3 + StringLen(afp -> street);
					aflen += 3 + StringLen(afp -> city);
					aflen += 3 + StringLen(afp -> sub);
					aflen += 3 + StringLen(afp -> country);
				}
			}else{
				aflen = 22;
			}
			temp = retval = MemNew( (size_t) (60+
				 StringLen (book_tit)+ StringLen(book_authors) +
				StringLen( ip -> volume) +
				2*len_page + StringLen( year) + aflen) );
/*--make book title all caps */
			for ( p = book_tit; *p; p++){
				*p = TOUPPER(*p);
			}
			temp = StringMove(temp,"(in) ");
			temp = StringMove(temp,book_authors);
			temp = StringMove(temp,namehead -> next? " (Eds.);\n":" (Ed.);\n");
			if (ap->choice == 1)    /* std names */
				ValNodeFreeData(namehead);

			temp = StringMove(temp,book_tit);
			if (ip->pages != NULL)
			{
				temp = StringMove(temp,":  ");
				temp = StringMove(temp,dup_pages);
				temp = StringMove(temp, ",");
			}
			if ( ip -> pub){
				first_std = TRUE;
				afp = ip -> pub;
				temp = StringMove(temp, "\n");
				if (afp -> affil){
					temp = StringMove(temp, afp -> affil);
					first_std = FALSE;
				}
				if ( afp -> choice == 2){
					if (afp -> div){
						if ( ! first_std)
							temp = StringMove(temp,", ");
						temp =  StringMove(temp,afp -> div);
						first_std = FALSE;
					}
					if (afp -> street){
						if ( ! first_std)
							temp = StringMove(temp,", ");
						temp =  StringMove(temp,afp -> street);
						first_std = FALSE;
					}
					if (afp -> city){
						if ( ! first_std)
							temp = StringMove(temp,", ");
						temp =  StringMove(temp,afp -> city);
						first_std = FALSE;
					}
					if (afp -> sub){
						if ( ! first_std)
							temp = StringMove(temp,", ");
						temp =  StringMove(temp,afp -> sub);
						first_std = FALSE;
					}
					if (afp -> country){
						if ( ! first_std)
							temp = StringMove(temp,", ");
						temp =  StringMove(temp,afp -> country);
						first_std = FALSE;
					}
					
				}
				if (Flat_format ==0){
					temp = StringMove(temp," (");
				}else{
					temp = StringMove(temp," (");
				}
			}else{
				temp = StringMove(temp,"\naddress unknown, (");
			}
			temp = StringMove(temp,year);
			temp = StringMove(temp,")");
		}else{
			temp = retval = MemNew( (size_t) (19+
				 StringLen ((CharPtr) (jp -> title -> data.ptrvalue)) +
				StringLen( ip -> volume) +
				2*len_page + StringLen( year)) );

			temp = StringMove(temp, (CharPtr) (jp -> title -> data.ptrvalue));
			if ((ip->volume != NULL) || (ip->pages != NULL))
			{
				temp = StringMove(temp," ");
				if (ip->volume != NULL){
					temp = StringMove(temp, ip -> volume);
				}
				if (ip->pages != NULL){
					if (Flat_format ==0){
						temp = StringMove(temp,", ");
					}else{
						temp = StringMove(temp,":");
					}
				}
			}
			if (ip->pages != NULL)
				temp = StringMove(temp, dup_pages);
			if (Flat_format ==0){
				temp = StringMove(temp," (");
			}else{
				temp = StringMove(temp,"(");
			}
			temp = StringMove(temp, year);
			if (Flat_format ==0){
				temp = StringMove(temp,")");
			}else{
				temp = StringMove(temp,").");
			}
			if (ip->prepub == 2)   /* In press */
				temp = StringMove(temp, " In press");
			else if (ip->prepub == 1)
				temp = StringMove(temp, " Submitted");
		}
		
		break;
	case 1: 
		cg = (CitGenPtr) the_pub -> data.ptrvalue;
		year[0] = '\0';
		if (cg->date)
		{
			dp = cg -> date;
			if ( dp -> data[0] == 1){
				sprintf(year,"%d",(int) ( 1900+dp -> data[1]));
			}else{
				StringNCpy( (CharPtr) year, (CharPtr) dp -> str, (size_t) 4);
				year[4] = '\0';
			}
		}
		if (cg->pages) {
			len_page = StringLen( cg -> pages);
			dup_pages = MemNew ( 2*len_page );
			fix_pages(dup_pages, cg ->pages);
		}
		if (cg->journal){
			p = (CharPtr)(cg->journal->data.ptrvalue);
			found_journal = TRUE;
		}else{
			p = NULL;
		}

		str_ret = NULL;
		if (cg -> cit) {
			str_ret = StrStr(cg -> cit ,"Journal=\"");
			if (str_ret){
			retval = StringSave(str_ret + 9);
			found_journal =used_cit = TRUE;
			for (p=retval; *p; p++)
				if (*p == '=' || *p == '\"'){
					*p = '\0';
				}
			} else if (p == NULL){   /* no journal field, use cit */
				used_cit = TRUE;
				if (strlen(cg->cit) > (size_t) 12){ /* get rest of Unpub entry */
					if (strncmp("Unpublished ", cg->cit, (unsigned) 12) == 0){
						unp = cg->cit;
						unp += 11;
					}
				}else{
					p = cg->cit;
					unp = NULL;
				}
			}
		}

		if (cg -> authors )
			if (cg -> authors -> affil)
         citgen_afp = cg -> authors -> affil;

		aflen = 2;
			if(citgen_afp != NULL){
				aflen = StringLen(citgen_afp -> affil)+ 5;
				if ( citgen_afp -> choice == 2){
					aflen += 3 + StringLen(citgen_afp -> div);
					aflen += 3 + StringLen(citgen_afp -> street);
					aflen += 3 + StringLen(citgen_afp -> city);
					aflen += 3 + StringLen(citgen_afp -> sub);
					aflen += 3 + StringLen(citgen_afp -> country);
				}
			}
		if ((retval == NULL) && ( (p != NULL) || unp != NULL))   {
			/* use cit or journal */
			if (unp == NULL){
				temp = retval = MemNew( (size_t) (9+
					 StringLen (p) + aflen +
					StringLen( cg -> volume) +
					2*len_page + StringLen( year)) );
			}else if (cg -> cit && ! used_cit && found_journal ){
            temp = retval = MemNew( (size_t) (9+
               11 + StringLen( cg -> volume) + aflen + 
						1+ StringLen (cg -> cit) +
               2*len_page + StringLen( year) + StringLen (unp)) );
			}else{ /* tack extra unpub entry to end of year */
            temp = retval = MemNew( (size_t) (9+
               11 + StringLen( cg -> volume) + aflen + 
               2*len_page + StringLen( year) + StringLen (unp)) );
			}

			temp = StringMove(temp, p);
			if (unp != NULL){
				temp = StringMove(temp, "Unpublished");
			}
			if (cg->volume)
			{
				temp = StringMove(temp," ");
				temp = StringMove(temp, cg -> volume);
			}
			if (dup_pages != NULL)
			{
				if (Flat_format ==0){
					temp = StringMove(temp,", ");
				}else{
					temp = StringMove(temp,":");
				}
				temp = StringMove(temp, dup_pages);
			}
			if (year[0] != '\0')
			{
				temp = StringMove(temp," (");
				temp = StringMove(temp, year);
				temp = StringMove(temp,")");
			}
		}
		if (unp != NULL){
			temp = StringMove(temp, unp);
		}
		if (cg -> cit && ! used_cit && found_journal ){
			temp = StringMove(temp," ");
			temp = StringMove(temp, cg -> cit);
		}
			if(citgen_afp != NULL){
				temp = StringMove(temp, " ");
				if (citgen_afp -> affil){
					temp = StringMove(temp, citgen_afp -> affil);
					first_std = FALSE;
				}
				if ( citgen_afp -> choice == 2){
					if (citgen_afp -> div){
						if ( ! first_std)
							temp = StringMove(temp,", ");
						temp =  StringMove(temp,citgen_afp -> div);
						first_std = FALSE;
					}
					if (citgen_afp -> street){
						if ( ! first_std)
							temp = StringMove(temp,", ");
						temp =  StringMove(temp,citgen_afp -> street);
						first_std = FALSE;
					}
					if (citgen_afp -> city){
						if ( ! first_std)
							temp = StringMove(temp,", ");
						temp =  StringMove(temp,citgen_afp -> city);
						first_std = FALSE;
					}
					if (citgen_afp -> sub){
						if ( ! first_std)
							temp = StringMove(temp,", ");
						temp =  StringMove(temp,citgen_afp -> sub);
						first_std = FALSE;
					}
					if (citgen_afp -> country){
						if ( ! first_std)
							temp = StringMove(temp,", ");
						temp =  StringMove(temp,citgen_afp -> country);
						first_std = FALSE;
					}
					
				}
			}
		break;
	default:
		if ( !  (Flat_Be_quiet&1))
		ErrPost(CTX_NCBI2GB, CTX_2GB_NOT_IMPLEMENTED,
		"FlatJournal: Unimplemented pub type=%d\n", 
		(int) the_pub -> choice);
	}

	MemFree(dup_pages);
	MemFree(book_authors );
	MemFree(book_tit );
	MemFree(date_std);
	if (retval){
		CharPtr hold, p , q;

		retval = FlatCleanEquals(retval);
		hold = p = MemNew(StringLen(retval)+1);

		for (q=retval; *q; q++){
			if (*q != '\n'){
				*p = *q;
			}else{
				*p = ' ';
			}
			p ++;
		}
		*p = '\0';
		Flat_out_index_line ( Flat_Index_journal, hold);
		MemFree(hold);
	}

	return (retval);

}

#define MAX_PAGE_DIGITS 12

/*************************************************************************
*
*	fix_pages (CharPtr out_pages, CharPtr in_pages)
*
*	in_pages: input page numbering. 
*	out_pages: output page numbering; "out_pages" should already
*	have allocated space when fix_pages is called (twice as many 
*	characters as "in_pages").
*
*	medline type page numbering is expanded (e.g., 125-35 -> 125-135,
*	F124-34 -> F124-F134, 12a-c -> 12a-12c).
*	If only one page is given, this is output without a dash.
*	Expanded numbering is validated to ensure that the
*	first number is smaller than or equal to the second and
*	that the first letter is less than or identical to the second
*	(i.e., a < c).  If the input is all letters (i.e., roman numerals)
*	this is not validated.
*
*	Return values:
*	 0 : valid page numbering.
*	-1 : invalid page numbering.
************************************************************************/
Int2 fix_pages (CharPtr out_pages, CharPtr in_pages)

{
	Boolean dash=TRUE, first_alpha;
	Char firstbegin[MAX_PAGE_DIGITS];
	Char secondbegin[MAX_PAGE_DIGITS];
	Char firstend[MAX_PAGE_DIGITS];
	Char secondend[MAX_PAGE_DIGITS];
	Char temp[MAX_PAGE_DIGITS];
	CharPtr alphabegin, numbegin, alphaend, numend, ptr, in=in_pages;
	Int2 diff, index, retval=0;
	Int2 length, length_nb, length_ab, length_ne, length_ae; 
	Int4 num1=0, num2=0;

	while (*in != '\0')
	{			/* Check for digits in input*/
		if (IS_DIGIT(*in))
			break;
		in++;
	}

	if (*in == '\0')
	{		/* if all letters (i.e. roman numerals), put out. */
		out_pages = StringCpy(out_pages, in_pages);
		return retval;
	}

	in = in_pages;
	if (IS_DIGIT(*in))
	{			/* Do digits come first? */
		first_alpha = FALSE;
		index=0;
		while (IS_DIGIT(*in) || *in == ' ')
		{
			firstbegin[index] = *in;
			if (*in != ' ')
				index++;
			in++;
			if (*in == '-')
				break;

		}
		firstbegin[index] = '\0';
		index=0;
		if (*in != '-')
		{		/* After digits look for letters. */
			while (IS_ALPHA(*in)  || *in == ' ')
			{
				secondbegin[index] = *in;
				index++;
				in++;
				if (*in == '-')
					break;
			}
		}
		secondbegin[index] = '\0';
		if (*in == '-')		/* if dash is not present, note */
			in++;
		else
			dash=FALSE;
		index=0;
		while (IS_DIGIT(*in) || *in == ' ')
		{			/* Look for digits.	*/
			firstend[index] = *in;
			if (*in != ' ')
				index++;
			in++;
		}
		firstend[index] = '\0';
		index=0;
		if (*in != '\0')
		{			/* Look for letters again. */
			while (IS_ALPHA(*in)  || *in == ' ')
			{
				secondend[index] = *in;
				index++;
				in++;
			}
		}
		secondend[index] = '\0';
	}
	else
	{			/* Do letters come first? */
		first_alpha = TRUE;
		index=0;
		while (IS_ALPHA(*in) || *in == ' ')
		{
			firstbegin[index] = *in;
			index++;
			in++;
			if (*in == '-')
				break;
		}
		firstbegin[index] = '\0';
		index=0;
		if (*in != '-')
		{		/* After letters look for digits. 	*/
			while (IS_DIGIT(*in)  || *in == ' ')
			{
				secondbegin[index] = *in;
				if (*in != ' ')
					index++;
				in++;
				if (*in == '-')
					break;
			}
		}
		secondbegin[index] = '\0';
		if (*in == '-')		/* Note if dash is missing. */
			in++;
		else
			dash=FALSE;
		index=0;
		while (IS_ALPHA(*in) || *in == ' ')
		{		/* Look for letters again. */
			firstend[index] = *in;
			index++;
			in++;
		}
		firstend[index] = '\0';
		index=0;
		if (*in != '\0')
		{		/* Any digits here? */
			while (IS_DIGIT(*in)  || *in == ' ')
			{
				secondend[index] = *in;
				if (*in != ' ')
					index++;
				in++;
			}
		}
		secondend[index] = '\0';
	}

	if (first_alpha)
	{
		alphabegin = firstbegin;
		numbegin = secondbegin;
		alphaend = firstend;
		numend = secondend;
	}
	else
	{
		numbegin = firstbegin;
		alphabegin = secondbegin;
		numend = firstend;
		alphaend = secondend;
	}

	length_nb = StringLen(numbegin);
	length_ab = StringLen(alphabegin);
	length_ne = StringLen(numend);
	length_ae = StringLen(alphaend);

	/* If no dash, but second letters or numbers present, reject. */
	if (dash == FALSE)
	{
		if (length_ne != 0 || length_ae != 0)
			retval = -1;
	}
	/* Check for situations like "AAA-123" or "222-ABC". */
	if (dash == TRUE)
	{
		if (length_ne == 0 && length_ab == 0)
			retval = -1;
		else if (length_ae == 0 && length_nb == 0)
			retval = -1;
	}

	/* The following expands "F502-512" into "F502-F512" and
	checks, for entries like "12a-12c" that a > c.  "12aa-12ab",
	"125G-137A", "125-G137" would be rejected. */
	if (retval == 0)
	{
		if (length_ab > 0)
		{
			if (length_ae > 0) 	
			{ 
				if (StringCmp(alphabegin, alphaend) != 0)
				{
					if (length_ab != 1 || length_ae != 1)
						retval = -1;
					else if (*alphabegin > *alphaend)
						retval = -1;
				}
			}
			else
			{
				alphaend = alphabegin;
				length_ae = length_ab;
			}
		} 
		else if (length_ae > 0) 
			retval = -1;
	}

/* The following expands "125-37" into "125-137".	*/
	if (retval == 0)
	{
		if (length_nb > 0)
		{
			if (length_ne > 0)
			{
				diff = length_nb - length_ne;
				if (diff > 0)
				{
					index=0;
					while (numend[index] != '\0')
					{
						temp[index+diff] = numend[index];
						index++;
					}
					temp[index+diff] = numend[index];
					for (index=0; index<diff; index++)
						temp[index] = numbegin[index];
					index=0;
					while (temp[index] != '\0')
					{
						numend[index] = temp[index];
						index++;
					}
					numend[index] = temp[index];
				}
			}
			else
			{
				numend = numbegin;
				length_ne = length_nb;
			}
		
		}
		else if (length_ne > 0)
			retval = -1;
	/* Check that the first number is <= the second (expanded) number. */
		if (retval == 0)
		{
			sscanf(numbegin, "%d", &num1);
			sscanf(	numend, "%d", &num2);
			if (num2 < num1)
				retval = -1;
		}
	}

	if (retval == -1)
	{
		out_pages = StringCpy(out_pages, in_pages);
	}
	else
	{
		ptr = out_pages;
	/* Place expanded and validated page numbers into "out_pages". */
		if (first_alpha)
		{
			while (*alphabegin != '\0')
			{
				*ptr = *alphabegin;
				alphabegin++;
				ptr++;
			}
			while (*numbegin != '\0')
			{
				*ptr = *numbegin;
				numbegin++;
				ptr++;
			}
			if (dash == TRUE)
			{
				*ptr = '-';
				ptr++;
				while (*alphaend != '\0')
				{
					*ptr = *alphaend;
					alphaend++;
					ptr++;
				}
				while (*numend != '\0')
				{
					*ptr = *numend;
					numend++;
					ptr++;
				}
			}
			*ptr = '\0';
		}
		else 
		{
			while (*numbegin != '\0')
			{
				*ptr = *numbegin;
				numbegin++;
				ptr++;
			}
			while (*alphabegin != '\0')
			{
				*ptr = *alphabegin;
				alphabegin++;
				ptr++;
			}
			if (dash == TRUE)
			{
				*ptr = '-';
				ptr++;
				while (*numend != '\0')
				{
					*ptr = *numend;
					numend++;
					ptr++;
				}
				while (*alphaend != '\0')
				{
					*ptr = *alphaend;
					alphaend++;
					ptr++;
				}
			}
			*ptr = '\0';
		}
	}
	return retval;
}

/*-------FlatDumpPubs()--------*/

void FlatDumpPubs
(FILE *fp, Int4 length, FLAT_PUB_STORE_PTR refhead, BioseqPtr bsp, AsnOptionPtr optionHead)
{
	int now, num=0;
	FLAT_PUB_STORE_PTR scan;
	FLAT_PUB_STORE_PTRPTR array = NULL;
	Int2 next_serial=1;

	for (scan = refhead; scan; scan = scan -> next){
		num ++;
	}
	if (num){

		array = MemNew(num * sizeof (FLAT_PUB_STORE_PTR) );
		for (num=0, scan = refhead; scan; scan = scan -> next){
			array [num ++] = scan;
		}

		HeapSort (array, num, sizeof(FLAT_PUB_STORE_PTR), COMP_serial);
		for (now=0; now < num; now++){
			if ( (array[now]) -> serial_number == NULL_SERIAL ||
				(array[now]) -> serial_number < 0 ){
				(array[now]) -> serial_number = next_serial ++;
			}else{
				if ((array[now]) -> serial_number < next_serial){
				  ErrPost(CTX_NCBI2GB,CTX_2GB_DUP_SERIAL,
					"FlatDumpPubs: serial number collision on %d\n", 
					(int) next_serial);
				}
				next_serial = (array[now]) -> serial_number + 1;
			}

		}
		for (now=0; now < num; now++){
			FlatPubOut(fp, (array[now]) -> the_pub,
				(Int4) ((array[now]) -> serial_number),
			(array[now]) ->location, bsp, length, optionHead, 
			(array[now]) -> site_only, (array[now]) ->pat_seqid);
			if (Flat_format !=0)
					FilePuts("XX\n",fp);
		}
	}
	if ( array)
		MemFree(array);
}

/*--------------- FlatPubPatStart()-----*/

void
FlatPubPatStart (FILE * fp, ValNodePtr the_pub, Int4 pat_seqid, AsnOptionPtr optionHead)
{
	CitPatPtr cp;
	CharPtr temp_date;
#ifdef ASN30
	PatPriorityPtr pri;
	AuthListPtr ap = NULL;
	CharPtr authors = NULL, authors_cont = NULL;
#endif
	CharPtr temp, t_temp, kludge;

	switch ( the_pub -> choice){

		case 9:
			cp = (CitPatPtr) the_pub -> data.ptrvalue;
			fprintf(fp, "PN   %s%s-%s/%d\n",
				cp ->country?cp ->country:"??" ,
				cp -> number? cp -> number:"MISSING-PAT-NUMBER",
				cp -> doc_type?cp -> doc_type:"",(int) pat_seqid);
			temp_date = FlatDateFromCreate (NULL, cp -> date_issue);
			if (temp_date){
				fprintf(fp, "PD   %s\n",temp_date);
			}
			fprintf(fp, "PF   ");
			if (cp -> app_date){
				temp_date = FlatDateFromCreate (NULL, cp -> app_date);
				if (temp_date){
					fprintf(fp, "%s",temp_date);
				}
				if (cp -> app_number){
					fprintf(fp, "%s%s%s",temp_date?" ":"",
					cp ->country,  cp -> app_number);
				}
			}
			fprintf(fp, "\n");
#ifdef ASN30
			pri = cp -> priority;
			if (pri){
				fprintf(fp, "PR   ");
				if (pri -> date){
				temp_date = FlatDateFromCreate (NULL, pri -> date);
				if (temp_date){
					fprintf(fp, "%s",temp_date);
				}
				if (cp -> app_number){
					fprintf(fp, "%s%s%s",temp_date?" ":"",
					pri -> country?pri -> country:"",pri -> number);
				}
				}
				fprintf(fp, "\n");
			}
			if (StringICmp(cp -> country, "US") == 0 && cp -> applicants){
				ap = cp -> applicants;
			}else if (cp -> assignees){
				ap = cp -> assignees;
			}else if (cp -> applicants){
				ap = cp -> applicants;
			}
			if (ap != NULL){
				authors = FlatAuthors(the_pub, ap, optionHead);
				if (authors ){
					authors_cont = FlatCont(authors, FALSE, optionHead);
					MACRO_FLAT_LINE_OUT(temp,"PA   ",authors_cont,kludge)
					fflush(fp);
				}
			}
#endif
		break;
		default:
			break;
	}
}

/*----------- FlatPubOut  ()------*/
void FlatPubOut 
(FILE *fp, ValNodePtr the_pub, Int4 num, ValNodePtr location, BioseqPtr bsp, Int4 length, AsnOptionPtr optionHead, Boolean site_only, Int4 pat_seqid)
{
	Int2 num_locs=0;
	char line [80];
	CharPtr p, formmed_locs = NULL, cont_locs=NULL;
	Char formmed_buf [20];
	CharPtr authors = NULL, authors_cont = NULL,
		title = NULL, title_cont = NULL,
		journal = NULL, journal_cont = NULL;
	CharPtr temp, t_temp, kludge;
	SeqLocPtr loc = NULL;
	CharPtr title_use= NULL;

	formmed_buf [0] = '\0';
	if (site_only){
		if (Flat_format == 0){
			fprintf(fp, "REFERENCE   %ld  (sites)\n", (long) (num) );
		}else{
			fprintf(fp, "RN   [%ld]\n", (long) num);
		}
	}else if (Flat_format == ASN2GNBK_FORMAT_PATENT_EMBL){
		FlatPubPatStart(fp, the_pub, pat_seqid, optionHead);
		if (the_pub -> choice != 9)
			fprintf(fp, "RN   [%ld]\n", (long) num);
	}else if (location){
		if (Flat_format==0){
			for (loc =SeqLocFindNext (location, NULL) ; 
					loc; loc = SeqLocFindNext (location, loc)){
				if (SeqIdIn(SeqLocId(loc), bsp -> id))
					num_locs ++;
			}
			p = formmed_locs = MemNew (22 + num_locs * 25);

			sprintf(formmed_buf, 
				"%ld  (bases ", (long) (num) );
			
			p = StringMove(p, formmed_buf);

			for (loc =SeqLocFindNext (location, NULL) ; 
					loc; loc = SeqLocFindNext (location, loc)){
				if (!SeqIdIn(SeqLocId(loc), bsp -> id))
					continue;
				sprintf(line,
					"%ld to %ld", (long) SeqLocStart(loc) + 1,
						(long) SeqLocStop(loc) + 1);
				p = StringMove(p,line);
				if (SeqLocFindNext (location, loc)){
					p = StringMove(p, "; ");
				}else{
					if (Flat_format ==0)
						p = StringMove(p, ")");
				}
			}

			cont_locs = FlatCont(formmed_locs, FALSE, optionHead);
			MACRO_FLAT_LINE_OUT(temp,REF_BASE_LTR,cont_locs,kludge)
		}else{
			fprintf(fp, "RN   [%ld]\n", (long) num);
			for (loc =SeqLocFindNext (location, NULL) ; 
					loc; loc = SeqLocFindNext (location, loc)){
				if (SeqIdIn(SeqLocId(loc), bsp -> id))
					num_locs ++;
			}
			p = formmed_locs = MemNew (22 + num_locs * 25);

			for (loc =SeqLocFindNext (location, NULL) ; 
					loc; loc = SeqLocFindNext (location, loc)){
				if (!SeqIdIn(SeqLocId(loc), bsp -> id))
					continue;
				sprintf(line,
					"%ld-%ld", (long) SeqLocStart(loc) + 1,
						(long) SeqLocStop(loc) + 1);
				p = StringMove(p,line);
				if (SeqLocFindNext (location, loc)){
					p = StringMove(p, ", ");
				}
			}

			cont_locs = FlatCont(formmed_locs, FALSE, optionHead);
			MACRO_FLAT_LINE_OUT(temp,EMBL_REF_BASE_LTR_BP,cont_locs,kludge)
		}
		fflush(fp);
/*----
		fprintf(fp, "REFERENCE   %s\n", cont_locs);
-----*/

	}else{
		if (Flat_do_Flat)
		if (Flat_format == 0){
			fprintf(fp, "REFERENCE   %ld  (bases 1 to %ld)\n", 
			(long) (num) , (long) length);
		}else{
			fprintf(fp, "RN   [%ld]\nRP   1-%ld\n",
			(long) (num) , (long) length);
		}
		fflush(fp);
	}

	authors = FlatAuthors(the_pub, NULL, optionHead);
	if (authors ){
		CharPtr hold_ltr = AUTHOR_LTR;
		authors_cont = FlatCont(authors, FALSE, optionHead);
		if ( Flat_format == ASN2GNBK_FORMAT_PATENT_EMBL	){
			AUTHOR_LTR = "PI   ";
		}
		MACRO_FLAT_LINE_OUT(temp,AUTHOR_LTR,authors_cont,kludge)
		AUTHOR_LTR = hold_ltr;
		fflush(fp);
/*---
		fprintf(fp, "  AUTHORS   %s\n",authors_cont);
---*/
	}else{
		MACRO_FLAT_LINE_OUT(temp,AUTHOR_LTR,".",kludge)
	}

	title = FlatPubTitle(the_pub, optionHead);
		if (title){
			if ( *title || Flat_format){
				if (Flat_format != 0){
					CharPtr hold_ltr = TITLE_LTR;
					t_temp = title_use = MemNew(StringLen(title)+6);
					if (*title != '\0'){
						t_temp = StringMove(t_temp,"\"");
						t_temp = StringMove(t_temp,title);
						t_temp = StringMove(t_temp,"\"");
					}
					t_temp = StringMove(t_temp,";");
					title_cont = FlatCont(title_use, FALSE, optionHead);
					if ( Flat_format == ASN2GNBK_FORMAT_PATENT_EMBL	){
						TITLE_LTR = "PT   ";
					}
					MACRO_FLAT_LINE_OUT(temp,TITLE_LTR,title_cont,kludge)
					TITLE_LTR=hold_ltr ;
				}else{
					title_cont = FlatCont(title, FALSE, optionHead);
					MACRO_FLAT_LINE_OUT(temp,TITLE_LTR,title_cont,kludge)
				}
				fflush(fp);
			}
	}
/*----
		fprintf(fp, "  TITLE     %s\n",title_cont);
----*/


	journal = FlatJournal(the_pub, pat_seqid, optionHead);
	if (journal ){
			CharPtr hold_ltr = JOURNAL_LTR;
		journal_cont = FlatCont(journal, FALSE, optionHead);
		if ( Flat_format == ASN2GNBK_FORMAT_PATENT_EMBL	){
			JOURNAL_LTR = "PC   ";
		}
		MACRO_FLAT_LINE_OUT(temp,JOURNAL_LTR,journal_cont,kludge)
		JOURNAL_LTR = hold_ltr;
/*---
		fprintf(fp, "  JOURNAL   %s\n",journal_cont);
---*/
	}

	if (Flat_format == 0){
		MACRO_FLAT_LINE_OUT(temp,STANDARD_LTR,"full automatic",kludge)
	}
	fflush(fp);

		MemFree(title_use);
		MemFree(formmed_locs);
		MemFree(cont_locs);
		MemFree(authors) ;
		MemFree(authors_cont);
		MemFree(title) ;
		MemFree(title_cont);
		MemFree(journal) ;
		MemFree(journal_cont );
}

/*----------- FlatDescrPubs ()------*/

void FlatDescrPubs 
(ValNodePtr descr, Int4 length, FLAT_PUB_STORE_PTRPTR refheadPtr, BioseqPtr bs, AsnOptionPtr optionHead)
{
	ValNodePtr an;
	PubdescPtr pd;
   
   if ( descr ){
			for (an = descr; an; an = an -> next){
				if ( an -> choice == Seq_descr_pub){
					pd = (PubdescPtr) an -> data.ptrvalue;
/*--is an pub equiv, so should only put out one ---*/
					FlatStoreEquivPub(pd -> pub, refheadPtr, bs, length, NULL, FALSE);
				}
			}
		}
}
/*------FlatStoreEquivPub()--------*/

void FlatStoreEquivPub
(ValNodePtr equiv, FLAT_PUB_STORE_PTRPTR refheadPtr, BioseqPtr bsp, Int4 length, ValNodePtr location, Boolean site_only)
{
	Int4 muid = 0;
	Int2 serial_number = NULL_SERIAL;  
	ValNodePtr best, pub;
	CitGenPtr cg;	

	best = FlatRefBest (equiv);
	if (best){
		for (pub = equiv; pub; pub = pub -> next){
			switch ( pub -> choice){
				case 1:
					cg = (CitGenPtr) pub -> data.ptrvalue;
					if ( cg -> muid > 0 )
						muid = cg -> muid;
					if ( cg -> serial_number > 0)
						serial_number = cg -> serial_number;
					break;
				case 4:
					muid = (Int4) pub -> data.intvalue;
					break;
			}
		}
		FlatStoreThisPub ( refheadPtr, best, serial_number, 
			muid, bsp, length, location, site_only);
	}

}

/*-------------FlatIgnoreThisPatentPub()---------------*/

Boolean
FlatIgnoreThisPatentPub (BioseqPtr bsp, ValNodePtr best, Int4Ptr seqidPt)
{
	Boolean retval = FALSE;
	CitPatPtr cp;
	SeqIdPtr sip;
	PatentSeqIdPtr psip;
	IdPatPtr ip;

	if (seqidPt){
		* seqidPt = 0;
	}
	if (best -> choice == 9){
		cp = (CitPatPtr) best -> data.ptrvalue;
		retval = TRUE;
		for ( sip = bsp -> id; sip; sip = sip -> next){
			if (sip -> choice == SEQID_PATENT){
				psip = (PatentSeqIdPtr) sip -> data.ptrvalue;
				ip = psip -> cit;
				if (ip){
					if (ip -> number){
						if (StringCmp(ip -> number, cp -> number) == 0){
							retval = FALSE;
							if (seqidPt){
								*seqidPt = psip -> seqid;
							}
							break;
						}
					}else if (ip -> app_number){
						if (StringCmp(ip -> app_number, cp -> app_number) == 0){
							retval = FALSE;
							if (seqidPt){
								*seqidPt = (psip -> seqid);
							}
							break;
						}
					}
				}
			}
		}
	}

	return retval;
}

/*--------------FlatStoreThisPub ()------*/
void FlatStoreThisPub 
(FLAT_PUB_STORE_PTRPTR refheadPtr, ValNodePtr best, Int2 serial_number, Int4 muid, BioseqPtr bsp, Int4 length, ValNodePtr location, Boolean site_only)
{
	FLAT_PUB_STORE_PTR this_store;
	FLAT_PUB_STORE_PTR scan_store, use_store;
	int num_locs = 0;
	SeqLocPtr loc;
	Int4 seqid;
	Boolean ignore_this;

	if (location){
		for (loc =SeqLocFindNext (location, NULL) ; 
				loc; loc = SeqLocFindNext (location, loc)){
			if (SeqIdIn(SeqLocId(loc), bsp -> id))
				num_locs ++;
		}
		if (num_locs == 0) return;
	}
	ignore_this = FlatIgnoreThisPatentPub (bsp, best, & seqid);
	if (best -> choice != 4  /* muid alone should be ignored */
			|| ignore_this){
		this_store = MemNew ( sizeof (FLAT_PUB_STORE));

		if ( ! (*refheadPtr)){
			(*refheadPtr) = this_store;
		}else{
			for ( use_store = scan_store = (*refheadPtr);
					scan_store; scan_store= scan_store -> next){
				use_store = scan_store;
			}
			use_store -> next = this_store;
		}
		this_store -> serial_number = serial_number;
		this_store -> the_pub = best;
		this_store -> muid = muid;
		this_store -> length = length;
		this_store -> location = location;
		this_store -> site_only = site_only;
		this_store -> pat_seqid = seqid;
	}
}

/*----------- FlatFeatPubs ()------*/

void FlatFeatPubs 
(SeqAnnotPtr annot, Int4 length, FLAT_PUB_STORE_PTRPTR refheadPtr, BioseqPtr bs, AsnOptionPtr optionHead)
{
	PubdescPtr pd;
	ValNodePtr pub, pub_in_set;
	SeqFeatPtr sfp;
   

   for ( ; annot; annot = annot -> next){
			if (annot -> type != 1) 
				continue;
			for ( sfp = (SeqFeatPtr) annot -> data;
					sfp; sfp = sfp -> next){
					for ( pub = sfp -> cit; pub; pub = pub -> next){
						if ( pub -> choice == 1){
							for ( pub_in_set = (ValNodePtr) 
									pub -> data.ptrvalue; pub_in_set;
									pub_in_set = pub_in_set -> next){
								if ( pub_in_set -> choice == 12){
		/* -----pub-equiv ----*/
									FlatStoreEquivPub ((ValNodePtr) 
									pub_in_set -> data.ptrvalue, 
									refheadPtr, bs, length,
									sfp -> location, TRUE);
								}else {
									FlatStoreThisPub ( refheadPtr, pub_in_set, NULL_SERIAL, 
											0, bs, length, sfp -> location, TRUE);
								}
							}
						} else {
								FlatStoreThisPub ( refheadPtr, pub, NULL_SERIAL, 
										0, bs, length, sfp -> location, TRUE);
						}
					}
				if (sfp -> data.choice != 6)
					continue;
				pd = sfp -> data.value.ptrvalue;
				if (pd ){
/*--is an pub equiv, so should only put out one ---*/
					FlatStoreEquivPub(pd -> pub, refheadPtr, bs, 
						length, sfp -> location, FALSE);
				}
			}
		}
}

/*--------FlatRefGet ()------*/

int FlatRefGet
(PubdescPtr pd, FLAT_PUB_STORE_PTR refhead)
{
	int retval = 0;
	ValNodePtr best;
	FLAT_PUB_STORE_PTR scan;

	best = FlatRefBest (pd -> pub);

	for ( scan = refhead; scan; scan = scan -> next){
		if ( scan -> the_pub == best){
			retval = scan -> serial_number;
			break;
		}
	}


	return retval;
}

/*----------- FlatRefs  ()------*/

/*----------
there are now 3 possible places (at different possible
levels to look for REFERENCE information
(inst-desc annot-pub annot-imp(AND OTHERS)-cit) 
	and now pointers to all of
these must be stored with serial_number information before
they can be output.
------------*/
int FlatRefs 
(FILE *fp, SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, FLAT_PUB_STORE_PTRPTR refheadPtr, AsnOptionPtr optionHead)
{
	BioseqPtr bs;
	SeqAnnotPtr annot;
	ValNodePtr descr;
	int retval = TRUE;
	Int4 length;

	length = (Int4) FlatLocusBases(the_seq, optionHead);
	
	if ( the_seq -> choice ==1){
		bs = (BioseqPtr) the_seq -> data.ptrvalue;
		annot = bs -> annot;
		descr = bs -> descr;
		FlatDescrPubs(descr,length, refheadPtr, bs, optionHead);
		FlatFeatPubs(annot, length, refheadPtr, bs, optionHead);
	}
	if (the_nodes)
		FlatSetPubs(the_nodes, length, refheadPtr, bs, optionHead);
	FlatDumpPubs(fp, length, * refheadPtr, bs, optionHead);

	fflush(fp);
	return retval;
}

