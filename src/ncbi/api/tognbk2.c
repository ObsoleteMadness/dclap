/*
	tognbk2.c

$Log:	tognbk2.c,v $
 * Revision 2.68  94/02/24  12:26:27  sirotkin
 * LOCUS date logic made more complete to deal
 * with gb-blocks anywhere, etc.  still seqrches
 * for std, but will use string, if all can find.
 * 
 * Revision 2.67  94/02/16  17:25:45  sirotkin
 * Changed encoding of TRNA-ext.codon
 * and logic for misc_feature of xref
 * and added errPost in relase mode for CDS imp=-feats
 * 
 * Revision 2.66  94/02/08  13:33:50  sirotkin
 * undergoing qa for release, includes new date logic
 * NCBI gi added to CDS
 * 
 * Revision 2.65  94/01/24  09:41:02  sirotkin
 * Removed bogus error message in release mode about dates
 * 
 * Revision 2.64  93/12/09  14:07:06  sirotkin
 * SeqPort changes including invalid residue, tRNA formating and date priority changing
 * 
 * Revision 2.63  93/11/29  10:36:23  sirotkin
 * bug fix 'bs' -> bss line 2980
 * 
 * Revision 2.62  93/11/17  18:27:49  sirotkin
 * safety in Tracker code - ready for qa
 * 
 * Revision 2.61  93/11/17  17:06:22  sirotkin
 * fix to Tracker for bbone seg-sets that start with accesison that
 * ends with "1"
 * 
 * Revision 2.60  93/11/15  10:20:41  sirotkin
 * INVALID_RESIDUE and "GenBank staff at the National Library of Medicine" message
 * 
 * Revision 2.59  93/11/10  15:55:16  sirotkin
 * tRNA from ASN.1
 * 
 * Revision 2.58  93/11/04  15:29:51  ostell
 * minor fix to FlatGenBankBlockFind
 * 
 * Revision 2.57  93/11/04  15:20:47  ostell
 * added FlatGenBankBlockFind() which looks for a genbank block at higher
 * levels of bag. Called only to find taxonomy line.
 * 
 * Revision 2.56  93/10/19  11:05:54  sirotkin
 * GeneRef.desc that are in xref no longer generate misc_features
 * 
 * Revision 2.55  93/10/13  15:54:51  sirotkin
 * Allows titleless CitGens and maps scRAN -> RNA
 * 
 * Revision 2.54  93/10/08  16:36:32  sirotkin
 * GeneRefs without locus or alleles no longer implicitly bound.
 * 
 * Revision 2.53  93/09/27  14:02:20  sirotkin
 * expanding tognbk3.c and tweaks to patent -m 4 format
 * 
 * Revision 2.52  93/09/15  17:06:55  sirotkin
 * scRNA ready
 * 
 * Revision 2.51  93/09/13  14:01:46  sirotkin
 * Handles Cit-sub affil.
 * 
 * 
 * Revision 2.49  93/09/01  16:52:06  sirotkin
 * has patent office format in ifdefs
 * 
 * Revision 2.48  93/07/28  11:42:30  sirotkin
 * nits for embl format
 * ,
 * 
 * Revision 2.47  93/07/12  10:48:06  sirotkin
 * No OrgRef forces /organism unknown
 * 
 * Revision 2.46  93/07/09  12:15:38  sirotkin
 * checks GeneRefs against known GBQuals before misc_feature
 * 
 * Revision 2.45  93/06/29  13:32:30  sirotkin
 * source feature now used both Seq_descr_mod and OrgRef.mod [tag value]
 * to generate qualifiers
 * 
 * Revision 2.44  93/06/25  13:15:17  sirotkin
 * source feature added, if none already, include taxfom common
 * 
 * Revision 2.43  93/06/23  17:45:35  sirotkin
 * CitArt base range embl format fixes
 * 
 * Revision 2.42  93/06/21  12:32:24  schuler
 * Made a static copy of NCBI_months[] in this file (because it cannot
 * be exported from ncbitime.c when linked as a DLL).
 * 
 * Revision 2.41  93/06/07  16:54:13  sirotkin
 * nit cast
 * 
 * Revision 2.40  93/06/07  15:50:20  sirotkin
 * fix to GBGetAuthNames() StingLen names[5]
 * 
 * Revision 2.39  93/06/04  12:30:06  sirotkin
 * FlatCommendPubDesc added as
 *           ^ (Oh well)
 * function to hanlde all comments from PUbdesc and called from
 * FlatCommentAnnot (not just FlatCommentDesc).
 * 
 * Revision 2.38  93/05/27  10:20:56  sirotkin
 * ignored \'Not given\' in update date
 * 
 * Revision 2.37  93/05/25  21:52:46  ostell
 * better support for NameStd in author names
 * 
 * Revision 2.36  93/05/21  17:05:15  schuler
 * Use DefAsnOptionFree instead of MemFree as arg to AsnOptionNew.
 * 
 * Revision 2.35  93/04/22  11:16:37  schuler
 * Restored (AsnOptFreeFunc) typecast in call to AsnOptionNew just in
 * the case of expresson_binding_free to eliminate a warning.
 * 
 * Revision 2.34  93/04/21  11:46:38  schuler
 * expression_binding_free declared as LIBCALLBACK.
 * removed (AsnOptFreeFunc) typecasts from calls to AsnOptionNew
 *   (to enable compile-time type checking).
 * 
 * Revision 2.33  93/04/20  11:12:51  schuler
 * PASCAL --> LIBCALLBACK
 * 
 * Revision 2.32  93/04/19  11:38:58  sirotkin
 * embl right side numbers working
 * 
 * Revision 2.31  93/04/15  12:55:05  schuler
 * Chnaged a _cdecl to a PASCAL .
 * 
 * Revision 2.30  93/04/12  12:10:04  sirotkin
 * Locus names collisions names picked up in SEQID_GENERAL as made by id database
 * 
 * Revision 2.29  93/04/09  07:57:55  sirotkin
 * not used auto var nits
 * 
 * Revision 2.28  93/04/06  13:15:06  sirotkin
 *  hyphen to N fix
 * 
 * Revision 2.27  93/04/05  15:13:24  sirotkin
 * fix to FlatLocusName for backbone
 * fix to SeqEntry Tracker for NULL not giving false
 * positive for mixxed owner ship sets.
 * NMot null, but vertuals non-null.
 * 
 * Revision 2.26  93/04/02  16:59:55  sirotkin
 * No more SeqIdFindBest in asn2gnbk, SeqIdPrint now handles local.str as accession
 * 
 * Revision 2.25  93/03/25  14:55:15  sirotkin
 * Flat_line_blank nits
 * 
 * Revision 2.24  93/03/22  14:16:12  sirotkin
 * deals with SeqId.local types for mixed ownership bags 
 * and Cit-gen now works with In
 * 
 * Revision 2.23  93/03/12  15:06:25  sirotkin
 * added GenBank format AUTHOR check to force terminal period
 * 
 * Revision 2.22  93/03/10  15:19:13  sirotkin
 * embl mode started
 * 
 * Revision 2.10  93/01/13  09:12:23  sirotkin
 * before starting on asn2{embl,flat}
 * 
 * Revision 2.9  93/01/13  09:11:35  sirotkin
 * some citation parsing addtions make by Shermer
 * 
 * Revision 2.8  93/01/05  15:46:02  schuler
 * Changed _fstrstr to StrStr  [GS]
 * 
 * 
 * Revision 2.7  93/01/05  15:35:37  sirotkin
 * *** empty log message ***
 * 
 * Revision 2.6  92/12/29  14:50:30  sirotkin
 * removed fuss about eb -> div >= 0
 * 
 * Revision 2.5  92/12/28  11:38:32  sirotkin
 * intermeldiate, for safety
 * 
 * Revision 2.4  92/12/02  16:21:48  sirotkin
 * cast near take_two for Microsoft compiler
 * 
 * Revision 2.3  92/12/02  09:32:50  sirotkin
 * Cite.book LOCUS SOURCE changes
 * 
 * Revision 2.2  92/11/06  15:48:04  sirotkin
 * removed some blank lines in comments, made robust against muid alone pubs.
 * 
 * Revision 2.1  92/10/08  15:15:40  sirotkin
 * EXPERIMENTAL VERSION, treats RNA featurs as if they were CDS for binding
 * 
 * Revision 2.0  92/10/06  17:37:33  ostell
 * force to 2.0
 * 
 * Revision 1.59  92/10/01  14:09:34  sirotkin
 * Has SeqFeatXref, goes with tognbk1 1.58
 * 
 * Revision 1.58  92/09/24  09:53:08  ostell
 * fixed entry date for day == 9
 * 
 * Revision 1.57  92/09/23  11:01:38  sirotkin
 * Memory leak fix in FlatLocus
 * 
 * Revision 1.55  92/09/11  11:24:19  ostell
 * fix for Authlist.std, In press, removal of , when no pages
 * 
 * Revision 1.53  92/08/28  10:04:56  sirotkin
 * xref accessions for GenBank, EMBL, and DDBJ now as 2ndary on ACCESSION line
 * 
 * Revision 1.52  92/08/12  12:07:48  ostell
 * *** empty log message ***
 * 
 * Revision 1.51  92/08/06  11:51:19  sirotkin
 * /product and genes no distribute evenly, if intervals same.
 * 
 * Revision 1.50  92/07/10  17:27:02  sirotkin
 * Now deals with reuse of genes and ProtRefs from multiple CDS better, but will no longer accept ProtRefs on DNA
 * 
 * Revision 1.49  92/07/01  10:30:00  ostell
 * added support for Genbank-block.entry-date
 * 
 * Revision 1.48  92/06/19  15:17:28  ostell
 * *** empty log message ***
 * 
 * Revision 1.47  92/06/11  16:49:27  ostell
 * fix for moving Org in segmented seq.
 * 
 * Revision 1.46  92/06/08  16:17:26  ostell
 * made ANSI functions
 * 
 * Revision 1.45  92/05/28  13:54:01  sirotkin
 * removed shortdir stuff
 * 
 * Revision 1.44  92/05/15  11:53:37  kans
 * *** empty log message ***
 * 
 * Revision 1.43  92/05/15  11:09:41  sirotkin
 * ready to install again
 * 
 * Revision 1.41  92/05/08  16:48:23  sirotkin
 * Julie changes all incorporated
 * 
 * Revision 1.40  92/05/06  12:57:42  kans
 * *** empty log message ***
 * 
 * Revision 1.39  92/05/06  12:16:06  kans
 * *** empty log message ***
 * 
 * Revision 1.38  92/05/06  11:08:37  kans
 * *** empty log message ***
 * 
 * Revision 1.37  92/05/06  10:23:48  sirotkin
 * added STANDARD  full automatic to regs
 * 
 * Revision 1.36  92/04/20  10:02:27  kans
 * *** empty log message ***
 * 
 * Revision 1.35  92/04/16  10:19:04  sirotkin
 * works daily update output, has _class
 * 
 * Revision 1.33  92/04/09  13:56:28  sirotkin
 * added DateFromCreate() for  LOCUS date, cleaned up MolId for seg_set LOCUS name
 * 
 * Revision 1.32  92/04/02  11:34:14  sirotkin
 * Added capibility to get Org info from Annot
 * 
 * Revision 1.31  92/03/16  10:11:03  sirotkin
 * authors now optional, if none "." prints out
 * 
 * Revision 1.30  92/03/06  09:34:43  ostell
 * AsnNode=ValNode,AsnValue=DataVal,AsnValueNode=AsnValxNode
 * 
 * Revision 1.29  92/03/05  12:03:17  kans
 * *** empty log message ***
 * 
 * Revision 1.28  92/03/05  08:57:24  sirotkin
 * Now does not put out single aa entry as if were ss-mRNA, it
 * just ignores it as it should!
 * 
 * Revision 1.27  92/03/02  09:04:16  sirotkin
 * new LOCUS line conventions, and minor fixes
 * 
 * Revision 1.26  92/02/21  08:49:53  sirotkin
 * ambig assignemnts fixed as per gish
 * 
 * Revision 1.25  92/02/19  09:39:42  sirotkin
 * at time rev 2.0 taken for gbparse/gbdiff
 * 
 * Revision 1.24  92/02/12  11:12:16  sirotkin
 * additionalle function to look up the outer_bag "nodes" for 
 * organelle modifiers
 * 
 * Revision 1.23  92/02/11  17:26:52  sirotkin
 * Now deals with Organelle names
 * 
 * Revision 1.22  92/02/06  11:04:15  kans
 * *** empty log message ***
 * 
 * Revision 1.21  92/02/05  09:40:03  sirotkin
 * ErrPost #else fix
 * 
 * Revision 1.20  92/01/27  15:33:57  sirotkin
 * 3 tweaks for T-10
 * 
 * Revision 1.19  92/01/27  14:57:58  sirotkin
 * MEM macros out
 * 
 * Revision 1.18  92/01/27  14:20:26  sirotkin
 * column length tweaks
 * 
 * Revision 1.17  92/01/15  18:53:16  sirotkin
 * should have mem leaks out, before chaning do_FlatCont
 * 
 * Revision 1.16  92/01/07  18:11:02  sirotkin
 * incremental for Jim
 * 
 * Revision 1.15  92/01/07  15:02:19  sirotkin
 * incremental fixes for safety
 * 
 * Revision 1.14  92/01/03  16:03:19  sirotkin
 * Unconditionally uses segment number on LOCUS if not genbank
 * 
 * Revision 1.13  92/01/02  21:13:12  sirotkin
 * many header bugs fixed
 * 
 * Revision 1.11  92/01/02  11:24:46  ostell
 * *** empty log message ***
 * 
 * Revision 1.10  92/01/01  12:06:56  kans
 * *** empty log message ***
 * 
 * Revision 1.9  91/12/31  16:55:56  sirotkin
 * fixed (I hope) LOCUS extra blank bug, no 2ndarf accession bug
 * and added DEBUG as a marcro to PC stack trouble debugging
 * 
 * Revision 1.8  91/12/26  17:08:07  sirotkin
 * bosc ready\?
 * 
 * Revision 1.7  91/12/23  16:31:25  sirotkin
 * no pub in if insufficinet info
 * 
 * Revision 1.6  91/12/20  10:54:10  sirotkin
 * intermediate fixes for GenBank
 * 
 * Revision 1.5  91/12/12  16:15:50  sirotkin
 * nit and CdRom stuff
 * `.
 * 
 * Revision 1.4  91/12/09  16:59:42  sirotkin
 * makes raw index files
 * 
*/

#include <math.h>
#include <ctype.h>
#include <togenbnk.h>

extern Uint1 Flat_Be_quiet, Flat_release;
extern Uint1 Locus_magic_cookie, Accession_magic_cookie;
#ifdef T10_and_CDROM
extern Uint1 Flat_T10, Flat_CdRom;
#endif
extern Uint1 Flat_do_Flat;
extern FILE * Flat_Index_accession, * Flat_Index_keyword,
	* Flat_Index_author, * Flat_Index_journal, * Flat_Index_gene;
extern CharPtr Flat_last_div, Flat_last_locus, Flat_last_accession;

extern Int4 Number_loci , Total_bases , Number_loci ;
extern Uint1 Flat_format, Flat_nuc_prot_which;
extern int Flat_line_blank;

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

static char * NCBI_months[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };


/*--------COMP_serial ()------------*/

int LIBCALLBACK COMP_serial 
(VoidPtr v1, VoidPtr v2)
{
	FLAT_PUB_STORE_PTRPTR p1, p2;
	p1 = (FLAT_PUB_STORE_PTRPTR) v1;
	p2 = (FLAT_PUB_STORE_PTRPTR) v2;
	return (*p1) -> serial_number - (*p2) -> serial_number;
}

/*---FlatRefClear()----------*/

void FlatRefClear 
(FLAT_PUB_STORE_PTR head)
{
	FLAT_PUB_STORE_PTR hold , now;
	
	for ( now = head; now; now = hold ){
		hold = now -> next;
		MemFree (now);
	}

}

/*--------FlatRefBest()----*/

/*
	returns ValNodePtr to best (for FlatFile production)
	pub in a equiv set.
*/
ValNodePtr FlatRefBest 
(ValNodePtr equiv)
{
	ValNodePtr the_pub, retval = NULL;
	CitBookPtr cb;
	CitSubPtr cs;
	CitGenPtr cg;	
	CitArtPtr ca;
	MedlineEntryPtr ml;
	CitJourPtr jp;
	ImprintPtr ip;
	Boolean good_one;
	Int1 bad_one= 0;
	CharPtr str_ret;


	for (the_pub = equiv, good_one = FALSE; the_pub && ! good_one
			; the_pub = the_pub -> next){

		switch ( the_pub -> choice){

      case 2:
         cs = (CitSubPtr) the_pub -> data.ptrvalue;
			if ( cs -> imp){
				ip = cs -> imp;
				if ( ip -> date){
					retval = the_pub;
					good_one = TRUE; /* good for submitted */
				}
			}
         break;
	/*  thesis */
		case 11:
			cb = (CitBookPtr) the_pub -> data.ptrvalue;
			if ( cb -> imp){
				ip = cb -> imp;
				if ( ip -> date){
					retval = the_pub;
					good_one = TRUE; /* good for thesis */
				}
			}
		break;
		case 9:
			retval = the_pub;
			good_one = TRUE; /* might exclude later...*/
		break;

/*  CitArts  or CitBook */
		case 5:
		case 3:
			if ( the_pub -> choice == 3){
				ml = (MedlineEntryPtr) the_pub -> data.ptrvalue;
				ca = (CitArtPtr) ml -> cit;

			}else{
				ca = (CitArtPtr) the_pub -> data.ptrvalue;
			}
		if( ca -> fromptr){
			if ( ca -> from ==1){
				jp = (CitJourPtr) ca -> fromptr;
				if ( jp -> imp){
					ip = jp -> imp;
					if ( ip -> date){
						retval = the_pub;
						good_one = TRUE; /* good as it gets */
					}
				}
			}else {
				CitBookPtr book = (CitBookPtr) ca -> fromptr;
					if ( book -> imp){
						ip = book -> imp;
						if ( ip -> date){
							retval = the_pub;
							good_one = TRUE; /* good for book */
						}
					}
				
			}
		}
			break;
		case 1: 
			cg = (CitGenPtr) the_pub -> data.ptrvalue;
			if (cg -> cit){

						retval = the_pub;  /*unless something better */
				}else if (cg -> journal){
						retval = the_pub;  /*unless something better */
				}

			break;
		case 8:  /* proc */
			bad_one = the_pub -> choice;
			break;
		}
	}

	if ( ! retval && bad_one != 0){
		ErrPost(CTX_NCBI2GB, CTX_2GB_NOT_IMPLEMENTED,
		"FlatRefBest: Unimplemented pub type = %d", bad_one);
	}
	return retval;
}

/*----------- FlatAllKeys  ()------*/
CharPtr FlatAllKeys 
(SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, AsnOptionPtr optionHead)
{
   CharPtr retval = NULL ;
   BioseqPtr bs;
   ValNodePtr descr, keys = NULL, spare;
   GBBlockPtr gb;
		EMBLBlockPtr eb;


   bs = the_seq -> data.ptrvalue;

   descr = bs -> descr;
   if ( descr ){
      gb = FlatGenBankBlock(descr, optionHead);
      if ( gb ){
            keys = gb -> keywords;
      }
   }   
		if (keys == NULL){
			eb = FlatEMBLBlock (descr, optionHead); 
			if (eb){
				keys = eb -> keywords;
			}
		}

	if (keys == NULL)
	for (; the_nodes && keys==NULL; the_nodes = the_nodes -> last){
		descr = the_nodes -> desc_nodes;
      gb = FlatGenBankBlock(descr, optionHead);
      if ( gb ){
				keys = gb -> keywords;
				break;
      }
			eb = FlatEMBLBlock (descr, optionHead); 
			if (eb){
				keys = eb -> keywords;
			}
	}



		for ( spare = keys; spare ; spare = spare -> next){
			Flat_out_index_line ( Flat_Index_keyword, 
				spare -> data.ptrvalue);
		}

      retval = FlatStringGroup ( NULL, keys, "; ", ".", optionHead);

   return retval;
}
/*----------- FlatKeys  ()------*/
FlatKeys 
(FILE *fp, SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, AsnOptionPtr optionHead)
{
	CharPtr AllKeys = NULL, cont_key = NULL;
	CharPtr temp,kludge;
	int retval = TRUE;

	AllKeys = FlatAllKeys (the_seq, the_nodes, optionHead);
	cont_key = do_FlatContKey(67, 12, AllKeys, optionHead);

	MACRO_FLAT_LINE_OUT(temp,Key_BASE_LTR,cont_key,kludge)

	fflush(fp);

	if (AllKeys)
		MemFree(AllKeys);

	if (cont_key)
		MemFree(cont_key);
	return retval;

}

/*----------- FlatGenBankBlock ()------*/
GBBlockPtr FlatGenBankBlock 
(ValNodePtr head, AsnOptionPtr optionHead)
{
	GBBlockPtr retval = NULL;

	for (; head; head = head -> next){
		if ( head -> choice == Seq_descr_genbank){
			retval = (GBBlockPtr) head -> data.ptrvalue;
			break;
		}
	}

	return retval;
}
/*----------- FlatGenBankBlockFind  ()------*/
GBBlockPtr FlatGenBankBlockFind
(ValNodePtr head, AsnOptionPtr optionHead, SEQ_ENTRY_STACK_PTR the_nodes)
{
	GBBlockPtr retval = NULL ;
	SEQ_ENTRY_STACK_PTR a_node;

	retval = FlatGenBankBlock (head, optionHead);
	if (retval == NULL){
		for(a_node=the_nodes; a_node && ! retval;
				a_node = a_node -> last){
			retval = FlatGenBankBlock(a_node -> desc_nodes ,optionHead);
		}
	}

	return retval;
}

/*----------- FlatEMBLBlock ()------*/
EMBLBlockPtr FlatEMBLBlock 
(ValNodePtr head, AsnOptionPtr optionHead)
{
	EMBLBlockPtr retval = NULL;

	for (; head; head = head -> next){
		if ( head -> choice == Seq_descr_embl){
			retval = (EMBLBlockPtr) head -> data.ptrvalue;
			break;
		}
	}

	return retval;
}

	
/*----------- FlatLocusName  ()------*/
CharPtr 
FlatLocusName 
(SeqEntryPtr the_seq, SeqEntryPtr seg_seq, AsnOptionPtr optionHead)
{
	CharPtr retval = NULL;
	BioseqPtr bs;
	SeqIdPtr id, best_id;
	char locbuf [80];
	char * locptr;
	int difflen;
	static Boolean order_initialized = FALSE;
	static Uint1 loc_order[18];
	static Uint1 acc_order[18];
	static Uint1 rel_order[18];
	
if ( ! order_initialized){
	int dex;
	for (dex=0; dex < 18; dex ++){
		acc_order[dex] = 255;
		rel_order[dex] = 255;
		loc_order[dex] = 255;
	}
		order_initialized = TRUE;
		loc_order[SEQID_GENBANK ] = 1;
		loc_order[SEQID_EMBL ] = 2;
		loc_order[SEQID_DDBJ ] = 3;

		acc_order[SEQID_GENBANK ] = 1;
		acc_order[SEQID_EMBL ] = 2;
		acc_order[SEQID_DDBJ ] = 3;
		acc_order[SEQID_LOCAL ] =4;
		acc_order[SEQID_GI ] =5;
		acc_order[SEQID_GIBBSQ ] =6;
		acc_order[SEQID_GIBBMT ] =7;
		acc_order[SEQID_PRF ] =8;
		acc_order[SEQID_PDB ] =9;
		acc_order[SEQID_PIR ] =10;
		acc_order[SEQID_SWISSPROT ] =11;
		acc_order[SEQID_PATENT ] =12;
		acc_order[SEQID_OTHER ] =13;
		acc_order[SEQID_GENERAL ] =14;
		acc_order[SEQID_GIIM ] =15;

		rel_order[SEQID_GENERAL ] =14;
}

	bs = the_seq -> data.ptrvalue;
	locptr = &locbuf[0];
	id = bs -> id;
	if ( (id )){
		locbuf[0] = '\0';
/*******release code**************/
		if (Flat_release){
			best_id = SeqIdSelect( id, rel_order,18);
			if (best_id != NULL) {
				if (best_id -> choice == SEQID_GENERAL){ /* always! */
					DbtagPtr db;
					ObjectIdPtr ob;

					db = (DbtagPtr) best_id -> data.ptrvalue;
					if (StringCmp(db -> db, LOCUS_COLLISION_DB_NAME) == 0){
						ob = db -> tag;
						if ( ob != NULL){
							StringNCpy(locbuf, ob -> str, MAX_LOCUS_NAME_LEN);
						}
					}
				}
			}
		}
/*******release code**************/
		if (locbuf[0] == '\0'){
			best_id = SeqIdSelect( id, loc_order,18);
			if (best_id != NULL) {
				SeqIdPrint (best_id, locbuf,  PRINTID_TEXTID_LOCUS);
			}
			if (locbuf[0] == '\0')
			 if ( (seg_seq != NULL)) {
				if (seg_seq -> next != NULL)
				if (seg_seq -> next -> choice == 2){
					BioseqSetPtr bssp = (BioseqSetPtr) (seg_seq -> next) 
						-> data.ptrvalue;
					if (bssp -> seq_set)
					if (bssp -> seq_set -> choice == 1)
					if (((BioseqPtr)(bssp -> seq_set->data.ptrvalue))->id 
							!= NULL)
					id = ((BioseqPtr)(bssp -> seq_set->data.ptrvalue))->id;
				}
			}
		}
		while ( locbuf[0] == '\0'){
			best_id = SeqIdSelect( id, acc_order,18);
			if (best_id == NULL)
				break;
			SeqIdPrint (best_id, locbuf, PRINTID_TEXTID_ACCESSION);
			if (locbuf[0] == '\0'){
				order_initialized = FALSE;
				acc_order[best_id->choice] = 255;
			}
		}
		if (*locptr){
			difflen = (int)StringLen(locptr);
			while ( (difflen > 7 && * locptr == '_') || 
					difflen >  MAX_LOCUS_NAME_LEN) {
				difflen -= 1;
				locptr ++ ;  /* _ then high characters */
			}
			retval = StringSave(locptr);
		}
	}
	if ( ! retval ){
		if ( !  (Flat_Be_quiet&1))
		ErrPost(CTX_NCBI2GB,CTX_2GB_NO_LOCUS,
			"FlatLocusName: Could not get LOCUS name");
	}
		return retval;
}

/*----------- FlatLocusBases  ()------*/
Int4 FlatLocusBases 
(SeqEntryPtr the_seq, AsnOptionPtr optionHead)
{
	Int4 retval = 0;
	BioseqPtr bs;

	bs = the_seq -> data.ptrvalue;

	retval = bs -> length;

	return retval;
}

	char * strand [4]= { "   ", "ss-", "ds-","ms-"};
	char * mol [9] = {"    ", "DNA ", "RNA ", "AA  ", "mRNA", "rRNA", "tRNA", "uRNA", "scRNA"};
	char * embl_mol [8] = {"xxx", "DNA", "RNA", "AA ", "RNA", 
		"RNA", "RNA", "RNA"};
/*----------- FlatLocusMolType  ()------*/
CharPtr 
FlatLocusMolType 
(char type_string[7 ], SeqEntryPtr the_seq, AsnOptionPtr optionHead)
{
	CharPtr retval;
	BioseqPtr bs;
	ValNodePtr an, descr;
	int is_strand, is_mol;

	retval = type_string;
	StringCpy(retval,"       ");

	bs = the_seq -> data.ptrvalue;

	is_strand = bs -> strand;
	if (is_strand > 3) is_strand = 0;

	is_mol = bs -> mol;
	if (is_mol > 3) is_mol = 0;

	descr = bs -> descr;
	if ( descr ){
		for (an = descr; an; an = an -> next){
			if (an -> choice == Seq_descr_mol_type){
				if (an -> data.intvalue >= 3 && 
				an -> data.intvalue <= 6 ){
					is_mol = (int) (an -> data.intvalue +1);
				break;
				/*--------
					248      unknown (0) ,
					249      genomic (1) ,
					250      pre-mRNA (2) ,
					251      mRNA (3) ,
					252      rRNA (4) ,
					253      tRNA (5) ,
					254      snRNA (6) ,
					255      scRNA (7) ,
					256      peptide (8) ,
					257   other-genetic (9) ,      -- other genetic material
					258   genomic-mRNA (10) ,      -- reported a mix of genomic and cdna sequence   259      index-seq (11) ,         -- GenInfo Backbone index sequence
					260      other (255)
				----------*/
				}
			}	
		}
	}

	if (Flat_format == 0){
		retval = StringMove(retval, strand[is_strand]);
		retval = StringMove(retval, mol[is_mol]);
	}else{
		retval = StringMove(retval, embl_mol[is_mol]);
	}
	
	return type_string;
}

/*----------- FlatLocusCircular  ()------*/
CharPtr FlatLocusCircular 
(CharPtr placer, SeqEntryPtr the_seq, AsnOptionPtr optionHead)
{
	CharPtr circ= Flat_format==0?"Circular":" circular";
	CharPtr blank= Flat_format==0?"        ":"";
	CharPtr retval ;
	BioseqPtr bs;

	retval = placer;
	StringCpy(retval,blank);
	bs = the_seq -> data.ptrvalue;

	if (bs -> topology == 2)
		StringCpy(retval, circ);

	return retval;
}

/*----------- FlatLocusDate  ()------*/
CharPtr FlatLocusDate 
(CharPtr default_date, SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, AsnOptionPtr optionHead)
{
	CharPtr retval = NULL ;
 Boolean used_str = FALSE;
	BioseqPtr bs;
	ValNodePtr descr;
	SEQ_ENTRY_STACK_PTR a_node;
	NCBI_DatePtr dp = NULL;
	CharPtr best_string = NULL;
	CharPtr best_other_string = NULL;

	bs = the_seq -> data.ptrvalue;

	descr = bs -> descr;

	dp = FlatLocusDateDescr( default_date, descr, & used_str,  & best_string,
		optionHead);
	if (dp == NULL || used_str){
		for(a_node=the_nodes; a_node && used_str ;
				a_node = a_node -> last){
			dp = FlatLocusDateDescr(default_date, a_node -> desc_nodes ,
					 & used_str,  & best_other_string, optionHead);
			if (best_string == NULL){
				best_string = best_other_string;
			}
		}
	}
	if (used_str){
		if (best_string){
			retval = StringSave(best_string);
		}
		if ( ! best_string){
			retval = StringSave(default_date);
		}
		if (Flat_release){
		ErrPost(CTX_NCBI2GB, CTX_2GB_DATE_STRING,
			"WARNING unparsed or default string for LOCUS date <%s>",
			retval);
		}
	}else{
		retval = FlatDateFromCreate(default_date, dp);
	}

	return retval;
}

/*----------- FlatLocusDateDescr  ()------*/

NCBI_DatePtr FlatLocusDateDescr
(CharPtr default_date, ValNodePtr descr, Boolean PNTR used_strPt, CharPtr PNTR best_stringPt, AsnOptionPtr optionHead)
{
	GBBlockPtr gb=NULL;
	ValNodePtr head;
	EMBLBlockPtr eb=NULL;
 DatePtr dp= NULL;
	Int2 d;
 CharPtr best_string=NULL;
	Boolean use_string = TRUE;

	for (head = descr; head; head = head -> next){
		if ( head -> choice == Seq_descr_update_date){
       dp = head->data.ptrvalue;
			if ( dp -> data[0] == 0){
				best_string = 	dp -> str;
			}else{
				use_string = FALSE;
			}
			break;
		}
	}

	for (head = descr; head; head = head -> next){
		if ( head -> choice == Seq_descr_create_date){
       if (dp){
		   	d = DateMatch(dp, head->data.ptrvalue, FALSE);
				if (d == -1)   /* dp < head->data.ptrvalue */
					dp = head->data.ptrvalue;
			}else{
          dp = head->data.ptrvalue;
			}
			if ( dp -> data[0] == 0){
				if (best_string == NULL ){
					best_string = 	dp -> str;
				}
			}else{
				use_string = FALSE;
			}
			break;
		}
	}
	if ( descr ){
		gb = FlatGenBankBlock(descr, optionHead);
		if ( gb ){
			if (gb->entry_date != NULL){
				if (dp){
					d = DateMatch(dp, gb->entry_date, FALSE);
					if (d == -1)   /* dp < gb->entry_date */
						dp = gb->entry_date;
				}else{
					dp = gb->entry_date;
				}
				if ( dp -> data[0] == 0){
					if (best_string == NULL ){
						best_string = 	dp -> str;
					}
				}else{
					use_string = FALSE;
				}
			}
		}
			eb = FlatEMBLBlock (descr, optionHead); 
			if (eb){
				if (eb -> update_date != NULL){
					if (dp){
						d = DateMatch(dp, eb -> update_date, FALSE);
						if (d == -1)   /* dp < eb -> update_date */
							dp = eb -> update_date;
					}else{
						dp = eb -> update_date;
					}
					if ( dp -> data[0] == 0){
						if (best_string == NULL ){
							best_string = 	dp -> str;
						}
					}else{
						use_string = FALSE;
					}
				}
				if (eb -> creation_date != NULL){
					if (dp){
						d = DateMatch(dp, eb -> creation_date, FALSE);
						if (d == -1)   /* dp < eb -> creation_date */
							dp = eb -> creation_date;
					}else{
						dp = eb -> creation_date;
					}
					if ( dp -> data[0] == 0){
						if (best_string == NULL ){
							best_string = 	dp -> str;
						}
					}else{
						use_string = FALSE;
					}
				}
			}
	}

 if (gb && use_string && ! dp ){
		if (gb -> date){
			best_string = (gb -> date);
		}
	}

	if (best_stringPt != NULL){
		if (*best_stringPt == NULL && best_string != NULL){
			*best_stringPt = best_string;
		} 
	}
	if (used_strPt != NULL){
		* used_strPt =  use_string;
	}
	return dp;
}

/*---------- FlatDateFromCreate() -----------*/

CharPtr
FlatDateFromCreate (CharPtr default_date, NCBI_DatePtr flat_date)
{
	CharPtr retval = NULL;
	char month[4], year[5]  , day[3];
	CharPtr daypt = & day[0];
	extern char * NCBI_months[12];
	char result[14];
	char localbuf[14];

	if ( flat_date -> data[0] == 0){
/*---string---*/
		if (StringICmp(flat_date -> str,"Not given") != 0){
			retval = StringSave(flat_date -> str);
			if (Flat_release){
			ErrPost(CTX_NCBI2GB, CTX_2GB_DATE_STRING,
				"WARNING unparsed string for LOCUS date <%s>",
				retval);
			}
		}
	}else{
/*---standard---*/
		if (! default_date  || 
				(flat_date -> data[1] &&flat_date -> data[2]
					&& flat_date -> data[3])){
			if (flat_date -> data[1]){
				sprintf(year, "%4d", (int) (flat_date -> data[1] + 1900));
			}else{
				sprintf(year, "????");
			}
			if (flat_date -> data[3]){
				if (flat_date -> data[3] <= 9){
					*daypt = '0';
					daypt ++;
				}
				sprintf(localbuf, "%d", (int) (flat_date -> data[3] ));
			}else{
				sprintf(localbuf, "??");
			}
			StringCpy (daypt, localbuf);
			if ( flat_date -> data[2] ){
				StringCpy(month, NCBI_months[flat_date -> data[2] -1 ]);
				month[1] = TO_UPPER(month[1]);
				month[2] = TO_UPPER(month[2]);
			}else{
				sprintf(month, "??");
			}
			sprintf(result,"%s-%s-%s",day,month, year);
			retval = StringSave(result);
		}
	}
	return retval;
}


/*----------- FlatLocusDiv  ()------*/
CharPtr FlatLocusDiv 
(SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, AsnOptionPtr optionHead)
{
	CharPtr retval = NULL ;
	BioseqPtr bs;
	ValNodePtr descr;
	SEQ_ENTRY_STACK_PTR a_node;

	bs = the_seq -> data.ptrvalue;

	descr = bs -> descr;

	retval = FlatLocusDivDescr (descr, optionHead);
	if (retval == NULL){
		for(a_node=the_nodes; a_node && ! retval;
				a_node = a_node -> last){
			retval = FlatLocusDivDescr(a_node -> desc_nodes ,optionHead);
		}
	}

	return retval;
}

/*----------- FlatLocusDivDescr  ()------*/

CharPtr FlatLocusDivDescr
(ValNodePtr descr, AsnOptionPtr optionHead)
{
	CharPtr retval = NULL ;
	GBBlockPtr gb;
	EMBLBlockPtr eb;
	static CharPtr embl_embl_divs [] = {
	"FUN",
	"INV",
	"MAM",
	"ORG",
	"PHG",
	"PLN",
	"PRI",
	"PRO",
	"ROD",
	"SYN",
	"UNA",
	"VRL",
	"VRT"
	};
	static CharPtr genbank_embl_divs [] = {
	"PLN",
	"INV",
	"MAM",
	"ORG",
	"PHG",
	"PLN",
	"PRI",
	"BCT",
	"ROD",
	"SYN",
	"UNA",
	"VRL",
	"VRT"
	};
	CharPtr PNTR div_use;
	CharPtr gb_div= NULL, embl_div= NULL;

	if ( descr ){
		gb = FlatGenBankBlock(descr, optionHead);
		if ( gb ){
			gb_div = gb -> div;
		}

		eb = FlatEMBLBlock (descr, optionHead); 
		if (eb){
			if (eb -> div != 255 && eb -> div <= 12){
				if (Flat_format != 0){
					div_use = embl_embl_divs;
				}else{
					div_use = genbank_embl_divs;
				}
				embl_div = div_use[eb -> div];
			}
		}
	}

	if (Flat_format != 0){
		if (embl_div != NULL){
			retval = embl_div;
		}else{
/*****************no way to go PLN -> FUN, without much lookup ****/
			if (gb_div != NULL){
					if (StringCmp(gb_div,"BCT") == 0){
						retval = embl_embl_divs[7];
					}else{
						retval = gb_div;
					}
			}
		}
	}else{
		if (gb_div != NULL){
			retval = gb_div;
		}else{
			retval = embl_div;
		}
	}

	return retval;
}

/*-------- FlatEMBLClass()-------*/

CharPtr
FlatEMBLClass (SeqEntryPtr the_seq, SeqEntryPtr seg_seq,
	AsnOptionPtr optionHead)
{
	static CharPtr standard =  "standard";
	static CharPtr unaanot =  "unannotated";
	CharPtr retval = standard;

	BioseqPtr bs;
	ValNodePtr descr;
	EMBLBlockPtr eb;

	bs = the_seq -> data.ptrvalue;

	descr = bs -> descr;
	if ( descr ){

		eb = FlatEMBLBlock (descr, optionHead); 
		if (eb){
			if (eb -> _class == 2)
				retval = unaanot;
		}
	}


	return retval;
}

/*----------- FlatLocus  ()------*/
int FlatLocus 
(CharPtr div, CharPtr default_date, FILE *fp, 
		SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, SeqEntryPtr seg_seq, 
		int this_seg, int max_seg, 
		Boolean for_real, Int4Ptr count_addedSnumsPt,
		AsnOptionPtr PNTR optionHeadPt)
{
	char type_string[12];	
	char mol_string[12];
	CharPtr the_div= FlatLocusDiv (the_seq, the_nodes, * optionHeadPt);
	CharPtr use_name, raw_name_hold = FlatLocusName(the_seq, seg_seq, * optionHeadPt);
	CharPtr found_date = NULL;
	char our_div[4];
	int seg_digits;
	char use_locus[MAX_LOCUS_NAME_LEN + 1];
	char format [88];
	char mol_type [8];
	char circ [12];
	char date [12];
	char do_use_name [MAX_LOCUS_NAME_LEN + 2];
	int retval = TRUE;
	int locus_short;
	char locus_pad[12];
	int i;
	Int4 name_len, length;
	DataVal loc_change;
	CharPtr raw_name;
	char segment_prefix [3];
 Boolean add_seg_here, do_by_option, opt_hybrid;

	StringCpy (segment_prefix, "S");

	if (raw_name_hold) if (! *raw_name_hold){
		MemFree (raw_name_hold);
		raw_name_hold = NULL;
	}
	raw_name = raw_name_hold;
	if (  raw_name){
		StringCpy ( mol_type, FlatLocusMolType(mol_string, the_seq, * optionHeadPt));
		StringCpy(circ, FlatLocusCircular(type_string, the_seq, * optionHeadPt));
		found_date = FlatLocusDate(default_date, the_seq, the_nodes, * optionHeadPt);
		if (found_date) {
			StringNCpy(date, found_date, (size_t) 12);
			date[11] = '\0';
		}else if (default_date){
			StringNCpy(date,default_date, (size_t) 12);
			date[11] = '\0';
		}else{
			StringCpy(date,"01-JAN-1900");
		}

		use_name = raw_name;

		if (div){
			StringNCpy(our_div, div, (size_t) 3);
		}

		if (the_div){
			StringNCpy(our_div, the_div, (size_t) 3);
		}

		if(StringLen(raw_name) > 3){
			if (raw_name[2] == ':'){
				StringNCpy(our_div, raw_name, (size_t) 3);
				raw_name += 3;
			}
		}

		our_div[3] = '\0';


		 add_seg_here = do_by_option = opt_hybrid = FALSE;
		if ( AsnOptionGet( * optionHeadPt, OP_TOGENBNK,
					OP_TOGENBNK_HYBRID , NULL) != NULL )
			opt_hybrid = TRUE;

		if (add_seg_digits(the_seq, use_name, this_seg, * optionHeadPt))
			add_seg_here = TRUE;

		if ( AsnOptionGet( * optionHeadPt, OP_TOGENBNK, 
					OP_TOGENBNK_LOCUS_S, NULL)  != NULL)
			do_by_option = TRUE;

		if ( max_seg > 0 && Flat_format == 0 && ! opt_hybrid &&
				(add_seg_here || do_by_option)){

			seg_digits = 1 + (int) (floor (log10((double)max_seg)));
			name_len = StringLen(raw_name);
			if (  isdigit(raw_name[name_len - 1]) ){
				if (for_real)
				if ( MAX_LOCUS_NAME_LEN - name_len - 1 - seg_digits < 0){
					char  loc_msg [45];

					ErrPost(CTX_NCBI2GB, CTX_2GB_LOCUS_LEN,
					"FlatLocus: %s too long to add \'S\' and %d digits",
					raw_name, seg_digits);
					segment_prefix [0]= '\0';
					sprintf(loc_msg,"LOCUS name changed from %s.", raw_name);
					loc_change.ptrvalue = StringSave (loc_msg);
					AsnOptionNew ( optionHeadPt, OP_TOGENBNK, 
						OP_TOGENBNK_COMMENT, loc_change, 
						DefAsnOptionFree);
				}
			}else{
				segment_prefix [0] = '\0';
				if (for_real)
				if ( MAX_LOCUS_NAME_LEN - name_len - seg_digits < 0){
					ErrPost(CTX_NCBI2GB, CTX_2GB_LOCUS_LEN,
					"FlatLocus: %s too long to add %d digits",
					raw_name, seg_digits);
				}
			}
			StringNCpy ( use_locus, raw_name, (size_t)
				 ( MAX_LOCUS_NAME_LEN-seg_digits));
			use_locus[MAX_LOCUS_NAME_LEN-seg_digits] = '\0';
			use_locus[9] = use_locus[10] = '\0';
			locus_short = MAX_LOCUS_NAME_LEN -  StringLen(segment_prefix) -
				seg_digits-StringLen(use_locus);
			for (i=0; i < locus_short; i++)
				locus_pad[i] = ' ';
			locus_pad[i] = '\0';
			if (locus_short < 0)
				locus_short = 0;
			sprintf(format,
			"LOCUS       %%-%ds%s%%0%dd%%s %%6ld bp %%-7s  %%8s  %%3s       %%11s\n",
				(int) (MAX_LOCUS_NAME_LEN - StringLen(segment_prefix)
					-seg_digits - locus_short) , segment_prefix,  (int) seg_digits);
			if (count_addedSnumsPt && segment_prefix [0] == 'S')
				(* count_addedSnumsPt) ++;
		if (for_real)
		if (Flat_do_Flat)
				fprintf(fp,format,
					use_locus, this_seg,locus_pad,
					(long) (length=FlatLocusBases(the_seq, * optionHeadPt)) ,
					mol_type, circ, our_div, date
			);
			sprintf(format,
			"%%-%ds%s%%0%dd",
			(int) (MAX_LOCUS_NAME_LEN - StringLen(segment_prefix)
				-seg_digits - locus_short), segment_prefix,  (int) seg_digits);
			sprintf(do_use_name ,format, use_locus, (int) this_seg);
			do_use_name[MAX_LOCUS_NAME_LEN] = '\0';
		}else{
			StringNCpy ( do_use_name, use_name, (int) (
				sizeof (do_use_name) - 1) );
			do_use_name[Flat_format==0?MAX_LOCUS_NAME_LEN:EMBL_MAX_LOCUS_NAME_LEN] = '\0';
			if ( StringLen(use_name) > (size_t) (Flat_format==0?MAX_LOCUS_NAME_LEN
						:EMBL_MAX_LOCUS_NAME_LEN ) ){
					char  loc_msg [50];

					ErrPost(CTX_NCBI2GB, CTX_2GB_LOCUS_LEN,
					"FlatLocus: %s too long", 
					use_name);
					sprintf(loc_msg,"LOCUS name changed by GenBank from %s.", use_name);
					loc_change.ptrvalue = StringSave (loc_msg);
					AsnOptionNew ( optionHeadPt, OP_TOGENBNK, 
						OP_TOGENBNK_COMMENT, loc_change, 
						DefAsnOptionFree);
			}
		if (for_real)
		if (Flat_do_Flat){
			if (Flat_format ==0){
				fprintf(fp,
				"LOCUS       %-10s %6ld bp %-7s  %8s  %3s       %11s\n",
					do_use_name,
					(long) (length=FlatLocusBases(the_seq, * optionHeadPt)) ,
					mol_type, circ, our_div, date);
			}else{
				fprintf(fp,
				"ID   %-9s  %s;%s %s; %s; %ld BP.\n",
				do_use_name, FlatEMBLClass (the_seq, seg_seq, * optionHeadPt),
				circ, mol_type, our_div, 
				(long) (length=FlatLocusBases(the_seq, * optionHeadPt))) ;
			}
		}

		}
		
		if ( ! for_real)
			goto DONE;

		Flat_last_locus =  Flat_Clean_store (Flat_last_locus, 
			do_use_name) ;
   Locus_magic_cookie = 
      Nlm_ErrUserInstall ( Flat_last_locus, Locus_magic_cookie);
		Flat_last_div =  Flat_Clean_store (Flat_last_div, our_div) ;
	}else{
		if (for_real)
		ErrPost(CTX_NCBI2GB, CTX_2GB_NO_LOCUS,
		"Could not find a LOCUS Name");
			retval = FALSE;
	}
	if (retval){
		Number_loci ++ ;
		Total_bases += length;
	}
	fflush(fp);
DONE:
	if (raw_name_hold)
		MemFree (raw_name_hold);
	if (found_date )
		MemFree(found_date );
	return retval;
}

/*----------- add_seg_digits ()------*/
int add_seg_digits 
(SeqEntryPtr the_seq, CharPtr use_name, int this_seg, AsnOptionPtr optionHead)
{
	int retval = 0;
	char buf[12];
	int name_len = StringLen(use_name) , buf_len;
	int name_dex, buf_dex ;
	BioseqPtr bs;
	SeqIdPtr id;

	bs = the_seq -> data.ptrvalue;

	id = bs -> id;
	if ( (id )){
		id = SeqIdFindBest( id, SEQID_GENBANK);
		if ( id -> choice < SEQID_GENBANK)
			retval = 1;
	}
	if ( ! retval){
		sprintf(buf,"%d", (int) this_seg);

		buf_len = StringLen(buf);

		if (buf_len > name_len){
			retval = 1;
		}else{
			for ( name_dex = name_len - buf_len, buf_dex = 0; 
					name_dex < name_len; name_dex ++, buf_dex ++ ){
				if ( use_name[name_dex] != buf[buf_dex] ){
					retval = 1;
					break;
				}
			}
		}
	}

	return retval;
}

/*----------- FlatOrigin  ()------*/
int FlatOrigin 
(FILE *fp, SeqEntryPtr the_seq, AsnOptionPtr optionHead)
{
	BioseqPtr bs;
	ValNodePtr descr;
	GBBlockPtr gb;
	CharPtr origin = NULL;
	CharPtr temp,kludge;
	int retval = TRUE;
	CharPtr cont_origin = NULL;

	if (Flat_format == 0){
		bs = the_seq -> data.ptrvalue;

		descr = bs -> descr;
		if ( descr ){
			gb = FlatGenBankBlock(descr, optionHead);
			if ( gb ){
					origin = gb -> origin;
			}
		}   

		if (origin)
			cont_origin = do_FlatContKey(67, 12, origin, optionHead);
		MACRO_FLAT_LINE_OUT(temp,ORI_BASE_LTR,cont_origin,kludge)
		fflush(fp);

		if (cont_origin)
			MemFree(cont_origin);
	}
	return retval;
}

/*----------- FlatResidues  ()------*/
int FlatResidues 
(FILE *fp, SeqEntryPtr the_seq, AsnOptionPtr optionHead)
{
		BioseqPtr bsp;
    SeqPortPtr spp;
    Uint1 repr, code, residue;
		int retval = TRUE;
		Int4 cnt=0;

		if (! Flat_do_Flat) return retval;

	if( the_seq -> choice ==1){
      bsp = (BioseqPtr) the_seq -> data.ptrvalue; 
    repr = Bioseq_repr(bsp);
    if (! ((repr == Seq_repr_raw) || (repr == Seq_repr_const))){
		if ( !  (Flat_Be_quiet&1))
		  ErrPost(CTX_NCBI2GB,CTX_2GB_BAD_REPR,
			"FlatResidues: bad repr %d\n", (int) repr);
			retval = FALSE;
			goto BYEBYE;
		}

    if ( ! FlatCheckMol(bsp)){
		if ( !  (Flat_Be_quiet&1))
			ErrPost(CTX_NCBI2GB,CTX_2GB_NOT_NA,
			"FlatResidues: WRONG code mol = %d, doing code=%d\n", 
				(int) bsp -> mol, (int) Flat_nuc_prot_which);
			retval = FALSE;
			goto BYEBYE;
		}

     code = ISA_na(bsp->mol)?Seq_code_iupacna:Seq_code_iupacaa;

    spp = SeqPortNew(bsp, 0, -1, 0, code);
    SeqPortSeek(spp, 0, SEEK_SET);
    while ((residue = SeqPortGetResidue(spp)) != SEQPORT_EOF) {
			if (residue == SEQPORT_EOS || residue == SEQPORT_VIRT)
				break;
			if ( !IS_residue(residue) && residue != INVALID_RESIDUE)
				continue;
			cnt ++;
			if (residue == '-' || residue == INVALID_RESIDUE){
				ErrPost (CTX_DEBUG,CTX_2GB_BAD_RESIDUE,
				"FlatResidues: invalid residue converted at residue %d", cnt);
				residue = 'N';
			}
			sqputc(fp, (int) (TO_LOWER(residue) ) );
    }
		sqputc(fp,(int) EOF);
		 SeqPortFree(spp);

		fflush(fp);
	}else{
		if ( !  (Flat_Be_quiet&1))
			ErrPost(CTX_NCBI2GB,CTX_2GB_NOT_BIOSEQ,
			"FlatResidues: non-Bioseq SeqEntry\n");
			retval = FALSE;
			goto BYEBYE;
	}
	BYEBYE:
		return retval;
}
/*----------- sqputc ()------*/
/*---------------------------------------------------------*
 * SQPUTC()  -  Write out a base of sequence in GenBank format.
 * If c==EOF the current sequence is terminated.  Returns # of bases 
 * written so far on this entry.
 *
 * grabed from original T-10 code
 *---------------------------------------------------------*/
int sqputc 
(FILE *fp, int c)
{
   static long count=0L;   /* number of bases of this sequence written */
   long l;
		char embl_form[12];

  /*----- On c==EOF reset for next seq */
   if ( c==EOF && count!=0L )
   {  
			if (Flat_format == ASN2GNBK_FORMAT_EMBL_SEQ_RIGHT ){
				if ( (count/60)*60 != count ){
/*   line did not end exactly */
					sprintf(embl_form,"%%%dld\n", 71-(count- ((count/60)*60)));
					fprintf(fp,embl_form,count);
				}
			}else{
				putc('\n',fp);
			}
      l = count;
      count = 0L;
      return (int) l;
   }

  /*----- Write out any other c */
   if ( (count/60)*60 == count )       /* time to start new line */
   {  
			if ( count && Flat_format != ASN2GNBK_FORMAT_EMBL_SEQ_RIGHT)
			  putc('\n',fp);
			if (Flat_format == 0 || Flat_format == ASN2GNBK_FORMAT_EMBL_SEQ_LEFT){
				fprintf(fp,"%9ld",count+1);
			}else{
				if (Flat_format == ASN2GNBK_FORMAT_EMBL_SEQ_RIGHT && count){
					fprintf(fp," %9ld\n",count);
				}
				fprintf(fp,"%4s"," ");
			}
   }
   if ( (count/10)*10 == count )       /* at beginning of 10 base block */
      putc(' ',fp);
   putc(c,fp);
   count++;
   return (int)count;
}

/*----------- FlattenSeq  ()------*/
int FlattenSeq 
(CharPtr div, CharPtr default_date, FILE *fp, SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, Boolean pickup_set_feats, SeqEntryPtr seg_seq, int this_seg, int max_seg, AsnOptionPtr optionHead)
{
	int retval = TRUE;
	FLAT_PUB_STORE_PTR refhead=NULL;

#ifdef DEBUG
ErrPost(CTX_DEBUG,1,"before LOCUS");
#endif
	retval = FlatLocus (div, default_date, fp,the_seq, the_nodes, seg_seq, this_seg, max_seg, TRUE, NULL, & optionHead);
	if (retval != TRUE)
		goto BYEBYE;
if (Flat_format !=0)
		FilePuts("XX\n",fp);
#ifdef DEBUG
ErrPost(CTX_DEBUG,1,"before LOCUS");
#endif
if (Flat_format == 0){
	retval = FlatDefinition	 (fp,the_seq, the_nodes, optionHead);
	if (retval != TRUE)
		goto BYEBYE;
#ifdef DEBUG
ErrPost(CTX_DEBUG,1,"before ACCESSION");
#endif
	retval = FlatAccession (fp,the_seq, the_nodes, optionHead);
	if (retval != TRUE)
		goto BYEBYE;
}else{
	retval = FlatAccession (fp,the_seq, the_nodes, optionHead);
	if (retval != TRUE)
		goto BYEBYE;
	FilePuts("XX\n",fp);
	retval = FlatDefinition	 (fp,the_seq, the_nodes, optionHead);
	if (retval != TRUE)
		goto BYEBYE;
	FilePuts("XX\n",fp);
}
#ifdef DEBUG
ErrPost(CTX_DEBUG,1,"before KEYS");
#endif
	retval = FlatKeys (fp,the_seq, the_nodes, optionHead);
	if (retval != TRUE)
		goto BYEBYE;
if (Flat_format !=0)
		FilePuts("XX\n",fp);
	if (max_seg && Flat_format == 0){
		if (Flat_do_Flat)
		fprintf(fp,
		"SEGMENT     %d of %d\n", this_seg, max_seg);
	}
#ifdef DEBUG
ErrPost(CTX_DEBUG,1,"before SOURCE");
#endif
	retval = FlatSource (fp,the_seq,the_nodes, & optionHead);
	if (retval != TRUE)
		goto BYEBYE;
if (Flat_format !=0)
		FilePuts("XX\n",fp);
#ifdef DEBUG
ErrPost(CTX_DEBUG,1,"before REFS");
#endif
	retval = FlatRefs (fp,the_seq,the_nodes, & refhead, optionHead);
	if (retval != TRUE)
		goto BYEBYE;
#ifdef DEBUG
ErrPost(CTX_DEBUG,1,"before COMMENT");
#endif
	retval = FlatComment (fp,the_seq, the_nodes, refhead, optionHead);
	if (retval != TRUE)
		goto BYEBYE;
if (Flat_format !=0)
		FilePuts("XX\n",fp);
#ifdef DEBUG
ErrPost(CTX_DEBUG,1,"before FEATURES");
#endif
	retval = FlatFeatures (fp,the_seq,pickup_set_feats?the_nodes:NULL,
		 refhead, & optionHead);
	AsnOptionFree (& optionHead, OP_TOGENBNK, OP_TOGENBNK_ORGREF);
	AsnOptionFree (& optionHead, OP_TOGENBNK, OP_TOGENBNK_SOURCE_FOUND);
	if (retval != TRUE)
		goto BYEBYE;
if (Flat_format !=0)
		FilePuts("XX\n",fp);
#ifdef DEBUG
ErrPost(CTX_DEBUG,1,"before BASE COUNT");
#endif
	retval = FlatBaseCount (fp,the_seq, optionHead);
	if (retval != TRUE)
		goto BYEBYE;
	retval = FlatOrigin (fp,the_seq, optionHead);
	if (retval != TRUE)
		goto BYEBYE;
#ifdef DEBUG
ErrPost(CTX_DEBUG,1,"before RESIDUE");
#endif
	retval = FlatResidues (fp,the_seq, optionHead);
	if (retval != TRUE)
		goto BYEBYE;

	fprintf(fp,"//\n");

	BYEBYE:
		AsnOptionFree (& optionHead, OP_TOGENBNK, OP_TOGENBNK_COMMENT);
		FlatRefClear(refhead);
		if (Flat_do_Flat)
		fflush(fp);
		return retval;
}

/*----------- FlatGenBankOrgDescr ()------*/
OrgRefPtr FlatGenBankOrgDescr 
(ValNodePtr head, AsnOptionPtr optionHead)
{
   OrgRefPtr retval = NULL;

   for (; head; head = head -> next){
      if ( head -> choice == 6){
         retval = (OrgRefPtr) head -> data.ptrvalue;
         break;
      }
   }

   return retval;
}

/*----------- FlatGenBankOrgAnnot ()------*/
OrgRefPtr FlatGenBankOrgAnnot 
(SeqAnnotPtr head, AsnOptionPtr optionHead)
{
   OrgRefPtr retval = NULL;
		SeqFeatPtr sfp;

   for (; head && ! retval; head = head -> next){
			for (sfp = (SeqFeatPtr) head -> data; sfp; 
					sfp = sfp -> next){
				if ( sfp -> data.choice == 2){
					retval = (OrgRefPtr) sfp ->  data.value.ptrvalue;
					break;
				}
			}
   }

   return retval;
}

/*--------FlatOrganelleDescr()---------*/

CharPtr
FlatOrganelleDescr(ValNodePtr descr, AsnOptionPtr PNTR optionHead)
{
	CharPtr retval = NULL;
	ValNodePtr man, an;
	static char * organelle_names [] = {
		 "Mitochondrion " ,
    "Chloroplast " ,
    "Kinetoplast ",
    "Cyanelle "};
	DataVal asndata;

	if ( descr ){
		for (an = descr; an; an = an -> next){
			if (an -> choice == Seq_descr_modif){
				for (man = (ValNodePtr) an-> data.ptrvalue; man; man = man -> next){
/*  from ASN.1 defs:
    plasmid (3) ,
    mitochondrial (4) ,
    chloroplast (5) ,
    kinetoplast (6) ,
    cyanelle (7) ,
    synthetic (8) ,
    recombinant (9) ,
    partial (10) ,
    transposon (14) ,
    insertion-seq (15) ,
   macronuclear (18) ,
   proviral (19) ,
---------------------------*/
					switch (man -> data.intvalue){
						case 4: case 5: case 6: case 7:
						if (! retval )
							retval = StringSave(organelle_names
								[man->data.intvalue-4]);
				/* no break on purpose */
						case 18: case 14: case 15: case 19: case 3:
						asndata.intvalue = man -> data.intvalue;
						AsnOptionNew ( optionHead, OP_TOGENBNK, 
							OP_TOGENBNK_SEQ_DESCR, asndata, 
							/*(AsnOptFreeFunc)*/ NULL);

						break;
					}
				}
				break;
			}	
		}
	}
	return retval;
}


/*-------FlatOrganelle() --------*/
CharPtr
FlatOrganelle (SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, AsnOptionPtr PNTR optionHead)
{
	CharPtr retval = NULL;
	BioseqPtr bs;
	ValNodePtr descr;

	bs = the_seq -> data.ptrvalue;

	descr = bs -> descr;
	retval = FlatOrganelleDescr(descr, optionHead);
	for (; the_nodes && ! retval; the_nodes = the_nodes -> last){
		descr = the_nodes -> desc_nodes;
		retval = FlatOrganelleDescr(descr, optionHead);
	}

	return retval;
}

/*----------- FlatSourceLine  ()------*/
CharPtr FlatSourceLine 
(SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, OrgRefPtr PNTR OrgRefPtrPt, AsnOptionPtr optionHead)
{
   CharPtr retval = NULL ;
   BioseqPtr bs;
   ValNodePtr descr;
   GBBlockPtr gb;
		OrgRefPtr or=NULL;
		CharPtr source = NULL;
	
		* OrgRefPtrPt = NULL;

   bs = the_seq -> data.ptrvalue;

   descr = bs -> descr;
   if ( descr ){
      gb = FlatGenBankBlock(descr, optionHead);
      if ( gb ){
            source = gb -> source;
      }
		or = FlatGenBankOrgDescr(descr, optionHead);
		if ( ! or && bs -> annot)
			or = FlatGenBankOrgAnnot(bs -> annot, optionHead);
		if ( ! or && the_nodes ){
			or = FlatOrgUse(the_seq, the_nodes, optionHead);
		}
		* OrgRefPtrPt = or;
   }   

		if (source){
			retval = StringSave(source);
		}else if (or) {
			retval = FlatStringGroup (or -> common?or->common:or ->taxname
				, or -> mod, " ", NULL, optionHead);
		}

		if ( ! retval)
			retval = StringSave(".");

   return retval;
}

Boolean
FlatMoreOrganism(SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, 
		OrgRefPtr PNTR OrgRefPtrPt, OrgRefPtr last_or, 
		AsnOptionPtr optionHead)
{
	Boolean retval = FALSE;
	BioseqPtr bs;
	ValNodePtr descr;
	OrgRefPtr or=NULL;
	Boolean found_last = FALSE;
	ValNodePtr head;
	
		* OrgRefPtrPt = NULL;

   bs = the_seq -> data.ptrvalue;

   descr = bs -> descr;
   if ( descr ){
		for (head = descr; head; head = head -> next){
			if ( head -> choice == 6){
				or = (OrgRefPtr) head -> data.ptrvalue;
					if ( or == last_or){
						found_last = TRUE;
						or = NULL;
					}else if (found_last){
						break;
					}
			}
		}
			* OrgRefPtrPt = or;
   }   

		if (found_last && or != NULL)
			retval = TRUE;

	return retval;
}

/*----------- FlatSource  ()------*/
int FlatSource 
(FILE *fp, SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, AsnOptionPtr PNTR optionHeadPt)
{
	OrgRefPtr or;
	CharPtr sl = FlatSourceLine (the_seq, the_nodes, & or, *optionHeadPt);
	CharPtr cont_sl = NULL;
	CharPtr temp,kludge;
	BioseqPtr bs;
	ValNodePtr descr;
	GBBlockPtr gb;
	CharPtr organelle = FlatOrganelle(the_seq, the_nodes, optionHeadPt);
	Boolean had_org = FALSE, had_tax=FALSE;
	CharPtr mess = NULL, mess_cont=NULL;
	static CharPtr unclass = "Unclassified.";
	int retval = TRUE;
	int len;
	OrgRefPtr last_or= NULL;
	DataVal asndata;
	AsnOptionPtr optionHead = * optionHeadPt;

	asndata.ptrvalue = or;
	AsnOptionNew ( optionHeadPt, OP_TOGENBNK, 
		OP_TOGENBNK_ORGREF, asndata, 
		/*(AsnOptFreeFunc)*/ NULL);

	if (or )
		if (or -> common && ! or -> taxname)
			or -> taxname = TaxNameFromCommon(or -> common);

	if (Flat_format == 0 ){
		cont_sl = FlatCont(sl, FALSE, optionHead);

		MACRO_FLAT_LINE_OUT(temp,SOU_BASE_LTR,cont_sl,kludge)
	}

/*----
	fprintf(fp, "SOURCE      %s\n", cont_sl);
---*/
	
	do {

		if (or) if (or -> taxname){
			had_org = TRUE;
			if (Flat_format ==0){
				mess = FlatStringGroup(organelle, NULL, NULL, 
					or -> taxname, optionHead);
				mess_cont = FlatCont(mess, FALSE, optionHead);
				MACRO_FLAT_LINE_OUT(temp,ORG_LTR,mess_cont,kludge)
			}else{
				len = StringLen(or -> taxname) + 6 + StringLen(or -> common);
				temp = mess = MemNew(len);
				temp = StringMove(temp,or->taxname);
				if (or -> common){
					temp = StringMove(temp," (");
					temp = StringMove(temp,or -> common);
					temp = StringMove(temp,")");
				}
				mess_cont = FlatCont(mess, FALSE, optionHead);
				MACRO_FLAT_LINE_OUT(temp,ORG_LTR,mess_cont,kludge)
			}
		}else{
			break;
		}
		if (Flat_format !=0)
			FilePuts("XX\n",fp);
		last_or = or;
	}while (Flat_format > 0 && FlatMoreOrganism(the_seq, the_nodes, 
			&or, last_or, optionHead));

	if (Flat_format != 0)
		if (organelle != NULL ){
			MACRO_FLAT_LINE_OUT(temp,ORG_LTR_nel,organelle,kludge)
			FilePuts("XX\n",fp);
		}
/*------
		fprintf(fp, "  ORGANISM  %s\n", or -> taxname);
------*/
	if ( ! had_org){
		MACRO_FLAT_LINE_OUT(temp,ORG_LTR,unclass,kludge)
	}

	bs = the_seq -> data.ptrvalue;
	descr = bs -> descr;
	gb = FlatGenBankBlockFind(descr, optionHead, the_nodes);
	if (gb) if (gb -> taxonomy){
		CharPtr temp, p, new_tax = StringSave(gb -> taxonomy);

/*------ check for period at end of taxonomy ---*/
		for (p = new_tax +StringLen(new_tax)-1; p > new_tax; p --){
			if (*p == ' ' || *p == '\t'){
				*p = '\0';
			}else{
				break;
			}
		}
		if (*p != '.'){
			p = temp = MemNew(StringLen(new_tax) +2);
			temp = StringMove(temp, new_tax);
			temp = StringMove(temp, ".");
			MemFree(new_tax);
			new_tax = p;
		}
		MemFree(cont_sl);
		cont_sl = FlatCont( new_tax, FALSE, optionHead);
		had_tax = TRUE;
		MACRO_FLAT_LINE_OUT(temp,Flat_format == 0?(COM_BLANK_LTR):(EMBL_ORG_LTR_tax),cont_sl,kludge)
		MemFree(new_tax);
	}
	if ( ! had_tax){
		MACRO_FLAT_LINE_OUT(temp,Flat_format == 0?(COM_BLANK_LTR):(EMBL_ORG_LTR_tax),unclass,kludge)
	}


	fflush(fp);

	if (sl)
		MemFree(sl);

	if (cont_sl)
		MemFree(cont_sl);
	if (mess_cont)
		MemFree(mess_cont);
	if (mess)
		MemFree(mess);
	if (organelle)
		MemFree(organelle);
	return retval;
}


/*----------- find_binding ()------*/

int find_binding 
(SeqFeatPtr feat, SeqFeatPtr PNTR cds, int num, Int4Ptr scores,
   int protein_or_gene, int PNTR found_already, int found_num)
{
	int retval = -1;
	Int4 s, best, i_am_best;
	int i, j;
	Boolean best_new, current_new;

	if (num > 0){
		best = -1;
		i_am_best = -1;
		for (i=0; i< num; i++){
			s = scores[i] = SeqLocSubset(feat-> location, 
         (protein_or_gene == ASN2GNBK_GENE?
         (cds[i])->location:
            (cds[i]->product)) ); /* -1 if no subset, 0 on match, 
                               else number of bases extra 
                              in first parameter */
			if ( s != -1){
				if (best == -1 || s < best){
					best = s;
					i_am_best = i;
				}else if ( s == best){
/*--- equal score, take cds that is not used yet ---*/
					for ( j=0, current_new=TRUE; j < found_num; j++ ){
						if (found_already[j] == i){
							current_new= FALSE;
							break;
						}
					}
					if ( current_new && ! best_new){
/*--- same, but do not have this CDS covered yet---*/
						best = s;
						i_am_best = i;
					}
				}
				for ( j=0, best_new=TRUE; j < found_num; j++ ){
					if (found_already[j] == i_am_best){
						best_new = FALSE;
						break;
					}
				}
			}
		}
		if ( best != -1)
			retval = (int) i_am_best;
	}

	return retval;
}

/*----------- find_xref ()------*/

int find_xref 
(SeqFeatPtr feat, SeqFeatPtr PNTR cds, int num, int protein_or_gene)
{
	int retval = FALSE;
	int i;
	SeqFeatXrefPtr xrp;
	GeneRefPtr g, xg;
	ProtRefPtr p, xp;
	CharPtr locus, allele, desc, firstprotname = NULL;

	if (protein_or_gene == ASN2GNBK_GENE){
		g = (GeneRefPtr) feat -> data.value.ptrvalue;
		locus = g -> locus;
		allele = g -> allele;
	}else{
		p = (ProtRefPtr) feat -> data.value.ptrvalue;
		if ( p -> name){
			firstprotname = (CharPtr) (p -> name -> data.ptrvalue);
		}
		
	}

	for (i=0; i < num && ! retval; i ++){
		for (xrp = (cds[i]) -> xref; xrp; xrp = xrp -> next){
			if (protein_or_gene == ASN2GNBK_GENE){
				if (xrp -> data.choice != 1){
					continue;
				}
				xg = (GeneRefPtr ) xrp -> data.value.ptrvalue;
#define FLAT_MACRO_SAFE_COMAPARE(block,slot) \
				if ( block -> slot){ \
					if (slot){ \
						if (StringCmp(block -> slot, slot) != 0){ \
							continue; \
						} \
					}else{ \
						continue; \
					} \
				}else{ \
					if (slot){ \
						continue; \
					} \
				}
FLAT_MACRO_SAFE_COMAPARE(xg,locus)
FLAT_MACRO_SAFE_COMAPARE(xg,allele)
		desc = g -> desc;
FLAT_MACRO_SAFE_COMAPARE(xg,desc)
			

			}else{
				if (xrp -> data.choice != 4){
					continue;
				}
				xp = (ProtRefPtr ) xrp -> data.value.ptrvalue;
				if ( xp -> name)
				if ( xp -> name -> data.ptrvalue){ 
					if (firstprotname){ 
						if (StringCmp(xp -> name -> data.ptrvalue, firstprotname) != 0){ 
							continue; 
						} 
					}else{ 
						continue; 
					} 
				}else{ 
					if (firstprotname){ 
						continue; 
					} 
				}
		desc = p -> desc;
FLAT_MACRO_SAFE_COMAPARE(xp,desc)
			}
			retval = TRUE;
			break;
		}

	}

	return retval;
}

/*--------match_feats_with_cdregions()--------------*/

void 
match_feats_with_cdregions (ExpressonsPtr express)
{
	BIND_FEAT_PTR feats;
	ImplicitBindingsPtr  bindings;
	int PNTR dex_genes;
	int PNTR dex_prots;
	Boolean PNTR gene_xreffed;
	Boolean PNTR prot_xreffed;
	Int4Ptr scores = NULL;
	int i;
#ifdef ASN2GNBK_PROTS_ON_DNA
	SeqIdPtr sid= NULL;
	ValNodePtr product = NULL;
	int j;
	BioseqPtr protein;
	SeqAnnotPtr prot_annot;
	SeqFeatPtr sfp, prot_sfp;
#endif

	if (express){
		feats = express -> features;
		bindings = express -> bindings =
			MemNew ( sizeof (ImplicitBindings));
		dex_genes = bindings -> dex_genes = MemNew ( sizeof(int) 
			* feats -> dex_genes);
		gene_xreffed = bindings -> gene_xreffed = MemNew ( sizeof(Boolean) 
			* feats -> dex_genes);
		scores = MemNew (sizeof(Int4) * feats -> dex_cdregions);
		for (i=0; i < feats -> dex_genes; i ++ ){
			gene_xreffed [i] = FALSE;
			dex_genes [i] = find_binding( (feats -> genes) [i],
				feats->cdregions, feats -> dex_cdregions, scores,
				ASN2GNBK_GENE, dex_genes, i);
			if ( dex_genes [i] == -1){
				gene_xreffed [i] = find_xref ((feats -> genes) [i],
				feats->cdregions, feats -> dex_cdregions, ASN2GNBK_GENE);
			}
		}
		dex_prots = bindings -> dex_prots = MemNew ( sizeof(int) 
			* feats -> dex_prots);
		prot_xreffed = bindings -> prot_xreffed = MemNew ( sizeof(Boolean) 
			* feats -> dex_prots);
		for (i=0; i < feats -> dex_prots; i ++ ){
			prot_xreffed [i] = FALSE;
			dex_prots [i] = find_binding( (feats -> prots) [i],
				feats->cdregions, feats -> dex_cdregions, scores,
				ASN2GNBK_PROTEIN, dex_prots,i);
			if ( dex_prots [i] == -1){
				prot_xreffed [i] = find_xref ((feats -> prots) [i],
				feats->cdregions, feats -> dex_cdregions, ASN2GNBK_PROTEIN);
			}
#ifdef ASN2GNBK_PROTS_ON_DNA
/*--- if this is activated, the case of 2 cdregions
   pointing to the same protein needing to share the
   protein and not point to same CDS must be fixed ---*/
			see above comment;
      
			for (j=0; j < feats -> dex_cdregions; j ++){
				sfp = (feats->cdregions) [j];
				product = sfp -> product;
				if (product){
					sid = (SeqIdPtr) ( product -> data.ptrvalue);
					protein = BioseqFind (sid);
					if (protein){
						prot_annot = protein -> annot;
							if (prot_annot){
								for ( prot_sfp = (SeqFeatPtr) prot_annot -> 
										data; prot_sfp; prot_sfp = 
										prot_sfp -> next){
									if ( prot_sfp -> data.choice == 4){
										if ( (feats -> prots) [i]
												== prot_sfp){
											dex_prots [i] = j;
										}
									}
								}
							}
					}
				}
			}
#endif
		}
		
	}
	if (scores)
		MemFree(scores);
}
/*--------expresson_binding_free()--------------*/

void LIBCALL
expresson_binding_free (ExpressonsPtr expressonsptr)
{
	ImplicitBindingsPtr bindingsptr;
	BIND_FEAT_PTR bound_feat_ptr;

	if (expressonsptr){
		bound_feat_ptr = expressonsptr -> features;
		if (bound_feat_ptr){
			if (bound_feat_ptr -> cdregions)
				MemFree(bound_feat_ptr -> cdregions);
			if (bound_feat_ptr -> genes)
				MemFree(bound_feat_ptr -> genes);
			if (bound_feat_ptr -> prots)
				MemFree(bound_feat_ptr -> prots);
			MemFree(bound_feat_ptr);
		}
		bindingsptr = expressonsptr -> bindings;
		if (bindingsptr){
			if (bindingsptr -> dex_genes)
				MemFree (bindingsptr -> dex_genes);
			if (bindingsptr -> dex_prots)
				MemFree (bindingsptr -> dex_prots);
			if (bindingsptr -> gene_xreffed)
				MemFree(bindingsptr -> gene_xreffed);
			if (bindingsptr -> prot_xreffed)
				MemFree(bindingsptr -> prot_xreffed);
			MemFree(bindingsptr);
		}
		MemFree(expressonsptr);
	}

}

	
/*--------bind_feature_counter()--------------*/

void 
bind_feature_counter (SeqEntryPtr sep, Pointer in_data, Int4 index, Int2 indent)
{
   BIND_FEAT_COUNTS_PTR data = (BIND_FEAT_COUNTS_PTR) in_data;
   BioseqPtr bs;
   BioseqSetPtr bss;
   ValNodePtr AsnFindNode();
   SeqAnnotPtr annot;

   if ( IS_Bioseq(sep) ){
      bs = sep -> data.ptrvalue;
			annot = bs -> annot;
   }else{
		bss = sep -> data.ptrvalue;
		annot = bss -> annot;
   }
	count_feats_in_annot(annot,data);
}


/*------count_feats_in_annot()----------*/
void 
count_feats_in_annot (SeqAnnotPtr annot, BIND_FEAT_COUNTS_PTR data)
{
	SeqFeatPtr sfp;
	ValNodePtr product = NULL;
	SeqIdPtr sid= NULL;
	BioseqPtr protein;
	SeqAnnotPtr prot_annot;
	SeqFeatPtr prot_sfp;
	ImpFeatPtr imp;
	GeneRefPtr gp;

		for ( ; annot; annot = annot -> next){
			for ( sfp = (SeqFeatPtr) annot -> data;
					 sfp; sfp = sfp -> next){
				switch ( sfp -> data.choice){

				case 1: 
					 gp = (GeneRefPtr)  sfp -> data.value.ptrvalue;
#ifdef LIMIT_GENEBINDING
					if (gp -> locus || gp -> allele){
#endif
						data -> genes ++;
#ifdef LIMIT_GENEBINDING
					}
#endif
					break;
				case 8:
					imp = (ImpFeatPtr) (sfp -> data.value.ptrvalue);
					if (StringCmp(imp ->key, "CDS") != 0)
						break;
				case 3: 
				case 5: /* RNA cases */
					data -> cdregions ++;
					product = sfp -> product;
					if (product){
						sid = (SeqIdPtr) ( product -> data.ptrvalue);
						protein = BioseqFind (sid);
						if (protein){
							prot_annot = protein -> annot;
								if (prot_annot){
									for ( prot_sfp = (SeqFeatPtr) prot_annot -> 
											data; prot_sfp; prot_sfp = 
											prot_sfp -> next){
										if ( prot_sfp -> data.choice == 4){
											data -> prots ++;
										}
									}
								}
						}
					}
						break;
#ifdef ASN2GNBK_PROTS_ON_DNA
				case 4: 
					data -> prots ++;
					break;
#endif
				}
			}
		}
}

/*--------bind_feature_finder()--------------*/

void 
bind_feature_finder (SeqEntryPtr sep, Pointer in_data, Int4 index, Int2 indent)
{
   BIND_FEAT_PTR data = (BIND_FEAT_PTR) in_data;
   BioseqPtr bs;
   BioseqSetPtr bss;
   ValNodePtr AsnFindNode();
   SeqAnnotPtr annot;

   if ( IS_Bioseq(sep) ){
      bs = sep -> data.ptrvalue;
			annot = bs -> annot;
   }else{
		bss = sep -> data.ptrvalue;
		annot = bss -> annot;
   }
	store_feats_in_annot(annot,data);
}

/*------store_feats_in_annot()----------*/

void 
store_feats_in_annot (SeqAnnotPtr annot, BIND_FEAT_PTR data)
{
	SeqFeatPtr sfp;
	ValNodePtr product = NULL;
	SeqIdPtr sid= NULL;
	BioseqPtr protein;
	SeqAnnotPtr prot_annot;
	SeqFeatPtr prot_sfp;
	ImpFeatPtr imp;
	GeneRefPtr gp;

		for ( ; annot; annot = annot -> next){
			for ( sfp = (SeqFeatPtr) annot -> data;
					 sfp; sfp = sfp -> next){
				switch ( sfp -> data.choice){

				case 1:
					gp = (GeneRefPtr)  sfp -> data.value.ptrvalue;
#ifdef LIMIT_GENEBINDING
					if (gp -> locus || gp -> allele){
#endif
						(data -> genes) [data -> dex_genes ++] = sfp;
#ifdef LIMIT_GENEBINDING
					}
#endif
					break;
				case 8:
					imp = (ImpFeatPtr) (sfp -> data.value.ptrvalue);
					if (StringCmp(imp ->key, "CDS") != 0)
						break;
				case 3: 
				case 5:
					(data -> cdregions) [data -> dex_cdregions ++] = sfp;
					product = sfp -> product;
					if (product){
						sid = (SeqIdPtr) ( product -> data.ptrvalue);
						protein = BioseqFind (sid);
						if (protein){
							prot_annot = protein -> annot;
								if (prot_annot){
									for ( prot_sfp = (SeqFeatPtr) prot_annot -> 
											data; prot_sfp; prot_sfp = 
											prot_sfp -> next){
										if ( prot_sfp -> data.choice == 4){
											(data -> prots) [data -> dex_prots ++] 
												= prot_sfp;
										}
									}
								}
						}
					}
					break;
#ifdef ASN2GNBK_PROTS_ON_DNA
				case 4: 
					(data -> prots) [data -> dex_prots ++] = sfp;
					break;
#endif
				}
			}
		}
}
	

/*--------------insure_expresson_binding()--------------*/

Boolean 
insure_expresson_binding
(SeqEntryPtr the_set, SEQ_ENTRY_STACK_PTR data, AsnOptionPtr PNTR optionHeadPtr)
{
	Boolean retval = FALSE;
	BIND_FEAT_COUNTS counts;
	BIND_FEAT_PTR featsptr;
	ExpressonsPtr expressonsptr;
	DataVal asndata;
	SEQ_ENTRY_STACK_PTR this_data;

	if ( !  AsnOptionGet( * optionHeadPtr, OP_TOGENBNK,
			 OP_TOGENBNK_BINDINGS, NULL)){
		counts.cdregions = counts.genes = counts.prots = 0;
		SeqEntryList (the_set, (Pointer) & counts, 
			(SeqEntryFunc) bind_feature_counter, 0, 0);
		for (this_data=data; this_data; this_data = this_data -> last){
			if ( this_data -> annot_nodes){
				count_feats_in_annot(this_data -> annot_nodes, & counts);
			}
		}
		if (counts.cdregions || counts.genes || counts.prots ){
			featsptr = MemNew (sizeof(BIND_FEAT));
			featsptr -> dex_cdregions = featsptr -> dex_genes = 
				featsptr -> dex_prots = 0;
			featsptr -> cdregions = MemNew ( counts . cdregions * 
				sizeof(SeqFeatPtr));
			featsptr -> genes = 
				MemNew ( counts . genes * sizeof(SeqFeatPtr));
			featsptr -> prots = 
				MemNew ( counts . prots * sizeof(SeqFeatPtr));
			SeqEntryList (the_set, (Pointer) featsptr, 
				(SeqEntryFunc) bind_feature_finder, 0, 0);
			for (this_data=data; this_data; 
					this_data = this_data -> last){
				if ( this_data -> annot_nodes){
					store_feats_in_annot(this_data -> annot_nodes, featsptr);
				}
			}
			asndata.ptrvalue = expressonsptr = MemNew(sizeof(Expressons));
			expressonsptr -> features = featsptr;
			match_feats_with_cdregions(expressonsptr);
			AsnOptionNew ( optionHeadPtr, OP_TOGENBNK,
				 OP_TOGENBNK_BINDINGS, asndata, (AsnOptFreeFunc)
					expresson_binding_free);
			retval = TRUE;
		}
	}

	return retval;
}


/*----------check_for_expresson_binding()------------*/

Boolean check_for_expresson_binding
(SeqEntryPtr the_set, SEQ_ENTRY_STACK_PTR data, AsnOptionPtr PNTR optionHeadPtr)
{
	Boolean retval = FALSE;
	SEQ_ENTRY_STACK_PTR stack;

	if ( !  AsnOptionGet( * optionHeadPtr, OP_TOGENBNK,
			 OP_TOGENBNK_BINDINGS, NULL)){
		for (stack = data; stack; stack = stack -> last){
			if ( stack -> annot_nodes){
				retval = TRUE;
				break;
			}
		}
	}
	if (retval){
		retval = insure_expresson_binding (the_set, 
			stack, optionHeadPtr);
	}

	return retval;
}

/*--------SeqEntryTracker()--------------*/
/*
	This code is based on the paradigm of SeqEntryList, however,
 it is different in that it keeps a backwards pointing list
 of data structures to point up the stack of relevent desc and
 annot nodes.
------*/

int SeqEntryTracker
(CharPtr div, CharPtr default_date, FILE *fp, SeqEntryPtr the_set, SEQ_ENTRY_STACK_PTR data, AsnOptionPtr optionHead)
{
	int retval = TRUE;
	BioseqSetPtr bss;
	BioseqPtr bs;
	ValNodePtr AsnFindNode();
	SEQ_ENTRY_STACK this_data;
	SEQ_ENTRY_STACK more_data;
	SEQ_ENTRY_STACK_PTR xtra_stack = & more_data;
	Boolean is_seg_set = FALSE;
	int max_seg=0, this_seg=0;
	SeqEntryPtr the_seq, seg_seq = NULL;
	Uint1 repr;
	Boolean just_did_expresson_bindings = FALSE;
	ValNodePtr temp_node;
	BioseqSetPtr temp_bss;
	DataVal locus_s;
	Int4 count_locus_s=0;
	Boolean mixxed, mixxed_set;


		this_data.last = data;
		this_data.desc_nodes = NULL;
		this_data.annot_nodes = NULL;
		this_data.max_seg = 0;
		this_data.this_seg = 0;
		this_data.seg_seq = NULL;
   if (IS_Bioseq(the_set) ){
			  bs = (BioseqPtr) ( the_set->  data.ptrvalue) ;
         repr = Bioseq_repr(bs);
         if (((repr == Seq_repr_raw) || (repr == Seq_repr_const)) 
					&& FlatCheckMol(bs)){
					if ( data){
						max_seg = data -> max_seg;
						this_seg = data -> this_seg;
						seg_seq = data -> seg_seq;
					}
					just_did_expresson_bindings  = insure_expresson_binding
						(the_set, & this_data,  &optionHead);
					retval = FlattenSeq (div, default_date, fp,the_set,
						 & this_data, TRUE,  seg_seq, this_seg, max_seg, optionHead);
				}
		}else{
			bss = (BioseqSetPtr) the_set -> data.ptrvalue;
			this_data.desc_nodes = bss -> descr;
			this_data.annot_nodes = bss -> annot;
			the_set = bss -> seq_set;
			if (bss -> _class ==2){ /* segmented set */
				if (the_set -> choice == 1){
					if (the_set -> next -> choice ==2){
						if (the_set -> next -> next == NULL){
							temp_node = the_set -> next;
							temp_bss = (BioseqSetPtr) temp_node -> data.ptrvalue;
							if ( temp_bss -> _class == 4){ /* parts */
								is_seg_set = TRUE;
								if (((BioseqPtr)(the_set->data.ptrvalue))->repr == Seq_repr_seg)
									this_data.seg_seq = the_set;
							}
						}
					}
				}
			}
			if (is_seg_set){
			  bs = (BioseqPtr) ( the_set->  data.ptrvalue) ;
         repr = Bioseq_repr(bs);
         if (((repr == Seq_repr_raw) || (repr == Seq_repr_const)) 
					&& FlatCheckMol(bs)){
				if ( data){
					max_seg = data -> max_seg;
					this_seg = data -> this_seg;
					seg_seq = data->seg_seq;
				}
				just_did_expresson_bindings  = insure_expresson_binding
					(the_set, & this_data,  &optionHead);
				retval = FlattenSeq (div, default_date, fp,the_set,
					  & this_data, (int) FALSE,  seg_seq, this_seg, max_seg, optionHead);
				}
				if (retval){

					the_set = the_set -> next;
					temp_bss = (BioseqSetPtr) the_set -> data.ptrvalue;
					xtra_stack -> desc_nodes = temp_bss -> descr;
					xtra_stack -> annot_nodes = temp_bss -> annot;
					xtra_stack -> last = & this_data;
					xtra_stack -> seg_seq = this_data.seg_seq;
					just_did_expresson_bindings  = 
						check_for_expresson_binding(the_set, & this_data,  
						&optionHead);  /* will point to stack, may start NULL */
					the_set = temp_bss -> seq_set;
					mixxed_set = FALSE;
					for ( xtra_stack -> max_seg=0, the_seq = temp_bss -> seq_set; 
							the_seq; the_seq = the_seq -> next){
						mixxed = FALSE;

/*******release code**************/
						if (Flat_release){
							BioseqPtr bs_chk;
							SeqIdPtr sip_chk;

							mixxed = TRUE;
							if ( the_seq -> choice == 1){
								if ( ! is_not_virtual(the_seq)){
									mixxed  = FALSE;
								}else{
									bs_chk = (BioseqPtr) the_seq -> data.ptrvalue;
									for (sip_chk = bs_chk -> id; sip_chk;
											sip_chk = sip_chk -> next){
										if ( sip_chk -> choice == SEQID_GENBANK ||
												sip_chk -> choice == SEQID_EMBL ||
												sip_chk -> choice == SEQID_DDBJ){
											mixxed = FALSE;
											break;
										}
									}
								}
							}
							if (mixxed){
								mixxed_set = TRUE;
							}
						}
/*******release code**************/
						if (!mixxed)
						if (is_not_virtual(the_seq)){
							xtra_stack -> max_seg ++;
						}
					}
					if ( ! mixxed_set)
					for ( xtra_stack -> this_seg=0, the_seq = temp_bss -> seq_set; 
							the_seq; the_seq = the_seq -> next){
						if (!mixxed)
						if (is_not_virtual(the_seq)){
							xtra_stack -> this_seg ++;
							if (xtra_stack -> max_seg ==1){
								xtra_stack -> max_seg = 0;
								xtra_stack -> this_seg = 0;
							}
							FlatLocus (div, default_date, NULL,the_seq, NULL, bss -> seq_set, 
									xtra_stack -> this_seg, xtra_stack -> max_seg, 
									FALSE, & count_locus_s, & optionHead);
						}
					}
					if (count_locus_s > 0){
						AsnOptionNew ( & optionHead, OP_TOGENBNK, 
							OP_TOGENBNK_LOCUS_S, locus_s, 
							/*(AsnOptFreeFunc)*/ NULL);
					}
					if (mixxed_set){
						AsnOptionNew ( & optionHead, OP_TOGENBNK, 
							OP_TOGENBNK_HYBRID , locus_s, 
							/*(AsnOptFreeFunc)*/ NULL);
					}
					for ( xtra_stack -> this_seg=0, the_seq = temp_bss -> seq_set; 
							the_seq; the_seq = the_seq -> next){
/*******release code**************/
						if (Flat_release){
							BioseqPtr bs_chk;
							SeqIdPtr sip_chk;

							mixxed = TRUE;
							if ( the_seq -> choice == 1){
								bs_chk = (BioseqPtr) the_seq -> data.ptrvalue;
								for (sip_chk = bs_chk -> id; sip_chk;
										sip_chk = sip_chk -> next){
									if ( sip_chk -> choice == SEQID_GENBANK ||
											sip_chk -> choice == SEQID_EMBL ||
											sip_chk -> choice == SEQID_DDBJ){
										mixxed = FALSE;
										break;
									}
								}
							}
						}
/*******release code**************/
						if (!mixxed)
						if (is_not_virtual(the_seq)){
							xtra_stack -> this_seg ++;
							if (xtra_stack -> max_seg ==1){
								xtra_stack -> max_seg = 0;
								xtra_stack -> this_seg = 0;
							}
							SeqEntryTracker(div, default_date, fp, the_seq, 
								xtra_stack, optionHead);
						}
					}
			AsnOptionFree (& optionHead, OP_TOGENBNK, OP_TOGENBNK_LOCUS_S);
			AsnOptionFree (& optionHead, OP_TOGENBNK, OP_TOGENBNK_HYBRID);
			AsnOptionFree (& optionHead, OP_TOGENBNK, OP_TOGENBNK_SEQ_DESCR);

				}

			}else{

				for ( the_seq = bss -> seq_set; 
						the_seq && retval ; the_seq = the_seq -> next){
					retval = SeqEntryTracker(div, default_date, fp, the_seq, 
						& this_data, optionHead);
				}
			}
			
	}
	if (just_did_expresson_bindings 	)
		AsnOptionFree ( & optionHead, OP_TOGENBNK, OP_TOGENBNK_BINDINGS);
	return retval;
}

/*------Flat_Clean_store ()--------*/

CharPtr Flat_Clean_store
(CharPtr target, CharPtr data)
{
	CharPtr retval = target;

	if (target )
		MemFree(target);

		if (data){
			retval = StringSave(data);
		}else{
			retval = NULL;
		}
		

	return retval;
}

/*----------Flat_is_book()----*/
Boolean Flat_is_book
(CharPtr value)
{
	Boolean retval = FALSE;

	if (StringLen(value) > 8){
		if ( * value == '(' && value[3] == ')'){
			if ( (value[1] == 'i' && value[2] == 'n') ||
			 (value[1] == 'I' && value[2] == 'N') )
				retval = TRUE;
		}
	}

	return retval;
}

/*------Flat_out_index_line()------*/

void Flat_out_index_line 
(FILE *fp, CharPtr value)
{
	int len;
	CharPtr buf = NULL, use=value;
	CharPtr dup= NULL, scanner, semi;

	if (fp && value && Flat_last_locus && 
			Flat_last_div && Flat_last_accession){
		if ( fp == Flat_Index_journal && Flat_is_book (value) ){ 
			/* is this the journal index file and we have a book? */
			if (dup)
				MemFree(dup);
			dup = StringSave(value);
			for (semi = dup; * semi; semi ++){
				if ( *semi == ';')
					break;
			}
			if ( * semi && semi-dup > 3){
				*(semi-3)='(';
				*(semi-2)='I';
				*(semi-1)='N';
				*(semi)=')';
				value = semi-3;
			}
		}
		if (buf)
			MemFree(buf);
		buf = MemNew(81);
		StringNCpy(buf,value,80);
		buf[80] = '\0';
		use = buf;
		len = StringLen(buf);
		for (scanner=buf+len-1; scanner>buf; scanner -- ){
			if ( * scanner == ' '){
				*scanner = '\0';
			}else{
				break;
			}
		}

		if (*use){
			for ( scanner = use; * scanner ; scanner ++) {
				*scanner = TO_UPPER(*scanner);
			}
			fprintf(fp, "%s%s%-10s %-3s %s\n",
		use,SORT_SEPARATOR, Flat_last_locus, Flat_last_div, Flat_last_accession);
		}
	}
	if (buf)
		MemFree(buf);
	if (dup)
		MemFree(dup);
}

/*----------- SeqEntryToGenbank  ()------*/
int SeqEntryToGenbank 
(FILE *fp, SeqEntryPtr the_set, AsnOptionPtr optionHead)
{
	int retval=TRUE;
	CharPtr div=NULL, default_date=NULL;
	AsnOptionPtr header_option = NULL, this_option= NULL;
	AsnOptionPtr internal_options=NULL;
	CharPtr str_ret;
	Boolean do_header = FALSE;
	OptionHeaderPtr headerPt=NULL;
	BioseqPtr bs;
	Uint1 repr;

		Flat_Be_quiet = 0;
		Flat_release = 0;
#ifdef T10_and_CDROM
		Flat_T10 = 0;
		Flat_CdRom = 0;
#endif
	 Flat_do_Flat= 1 ;

	Flat_format = 0;
	Flat_line_blank = 12;

	ACC_BASE_LTR = GENBANK_ACC_BASE_LTR;
	COM_BLANK_LTR= GENBANK_COM_BLANK_LTR;
	COM_BASE_LTR= GENBANK_COM_BASE_LTR;
	DEF_BASE_LTR= GENBANK_DEF_BASE_LTR;
	Blank_BASE_LTR= GENBANK_Blank_BASE_LTR;
	Key_BASE_LTR= GENBANK_Key_BASE_LTR;
	ORI_BASE_LTR= GENBANK_ORI_BASE_LTR;
	REF_BASE_LTR= GENBANK_REF_BASE_LTR;
	AUTHOR_LTR= GENBANK_AUTHOR_LTR;
	TITLE_LTR= GENBANK_TITLE_LTR;
	JOURNAL_LTR= GENBANK_JOURNAL_LTR;
	STANDARD_LTR= GENBANK_STANDARD_LTR;
	SOU_BASE_LTR= GENBANK_SOU_BASE_LTR;
	ORG_LTR= GENBANK_ORG_LTR;
	FEAT_CONT_BLANKS = GENBANK_FEAT_CONT_BLANKS;

	  Flat_Index_accession=  Flat_Index_keyword,
	 Flat_Index_author=  Flat_Index_journal,  Flat_Index_gene = NULL;
	Flat_last_div =  Flat_Clean_store (Flat_last_div, NULL) ;
	Flat_last_locus =  Flat_Clean_store (Flat_last_locus, NULL) ;
	Flat_last_accession =  Flat_Clean_store (Flat_last_accession, NULL) ;

/*-------the magic_cookies indentify the User installed
 *       error messages for ErrPost   */
   Nlm_ErrUserDelete(Accession_magic_cookie);
   Accession_magic_cookie = 0;

   Nlm_ErrUserDelete(Locus_magic_cookie);
   Locus_magic_cookie = 0;

			Number_loci = Total_bases = 0L;

		while (( this_option = 
			AsnOptionGet(optionHead, OP_TOGENBNK, (Int2) 0, this_option)) != NULL){
			switch ( this_option -> type){
				case OP_TOGENBNK_DIV :
					div = this_option -> data.ptrvalue;
					break;
				case OP_TOGENBNK_DATE :
					default_date = this_option -> data.ptrvalue;
					break;
				case OP_TOGENBNK_NO_NCBI :
					Flat_Be_quiet |= 2;
					break;
				case OP_TOGENBNK_QUIET :
					Flat_Be_quiet |= 1;
					break;
				case OP_TOGENBNK_FORMAT:
					switch (this_option -> data.intvalue){
						case ASN2GNBK_FORMAT_EMBL:
							Flat_format = ASN2GNBK_FORMAT_EMBL;
embl_format:
							Flat_line_blank = 5;
							ACC_BASE_LTR = EMBL_ACC_BASE_LTR;
							COM_BLANK_LTR= EMBL_COM_BLANK_LTR;
							COM_BASE_LTR= EMBL_COM_BASE_LTR;
							DEF_BASE_LTR= EMBL_DEF_BASE_LTR;
							Blank_BASE_LTR= EMBL_Blank_BASE_LTR;
							Key_BASE_LTR= EMBL_Key_BASE_LTR;
							REF_BASE_LTR= EMBL_REF_BASE_LTR;
							REF_BASE_LTR_BP= EMBL_REF_BASE_LTR_BP;
							AUTHOR_LTR= EMBL_AUTHOR_LTR;
							TITLE_LTR= EMBL_TITLE_LTR;
							JOURNAL_LTR= EMBL_JOURNAL_LTR;
							ORG_LTR= EMBL_ORG_LTR;
							ORG_LTR_nel= EMBL_ORG_LTR_nel;
							ORG_LTR_tax= EMBL_ORG_LTR_tax;
							FEAT_CONT_BLANKS = EMBL_FEAT_CONT_BLANKS;
							break;
						case ASN2GNBK_FORMAT_EMBL_SEQ_LEFT:
							Flat_format = ASN2GNBK_FORMAT_EMBL_SEQ_LEFT;
							goto embl_format;
						case ASN2GNBK_FORMAT_EMBL_SEQ_RIGHT:
							Flat_format = ASN2GNBK_FORMAT_EMBL_SEQ_RIGHT;
							goto embl_format;

						case ASN2GNBK_FORMAT_PATENT_EMBL:
							Flat_format = ASN2GNBK_FORMAT_PATENT_EMBL;
							goto embl_format;
							break;
						default:
						ErrPost(CTX_NCBI2GB,CTX_2GB_ARG_TROUBLE, 
		"%d is an unknown format option, GenBank, the default to be used",  
							this_option -> data.intvalue);
					}
					break;

				case OP_TOGENBNK_NUCL_PROT:
				Flat_nuc_prot_which = this_option -> data.intvalue;
				break;

				case OP_TOGENBNK_RELEASE:
					Flat_release = 1;
					break;
#ifdef T10_and_CDROM
				case OP_TOGENBNK_T10 :
					Flat_T10 = 1;
					break;
				case OP_TOGENBNK_CDROM :
					Flat_CdRom = 1;
					break;
#endif
				case OP_TOGENBNK_INDEX:
#define MACRO_DO_INDEX(THIS_FILE,LETTER,FILENAME)\
			str_ret = StrStr(this_option -> data.ptrvalue ,LETTER);\
			if (str_ret){\
				THIS_FILE = FileOpen(FILENAME,"a");\
				if ( ! THIS_FILE)\
					ErrPost(CTX_NCBI2GB,CTX_2GB_INDEX_FILE_OPEN, \
						"Could not open %", FILENAME);\
			}
/* this one is exceptional, so no MACRO */
			str_ret = StrStr(this_option -> data.ptrvalue ,":");
			if (str_ret){
				Flat_do_Flat = 0; /* turns of all output in MACRO_FLAT_LINE_OUT */
			}
			MACRO_DO_INDEX(Flat_Index_accession,"C","accession.raw")
			MACRO_DO_INDEX(Flat_Index_keyword,"K","keyword.raw")
			MACRO_DO_INDEX(Flat_Index_author,"A","author.raw")
			MACRO_DO_INDEX(Flat_Index_journal,"J","journal.raw")
			MACRO_DO_INDEX(Flat_Index_gene,"G","nomgen.raw")
					break;
				case OP_TOGENBNK_HEADER :
				do_header = TRUE;
				header_option = this_option;
				headerPt = (OptionHeaderPtr) (this_option -> data.ptrvalue);
					break;
			}
		}

		switch (the_set -> choice){
      case 1:   
			  bs = (BioseqPtr) ( the_set->  data.ptrvalue) ;
         repr = Bioseq_repr(bs);
         if (((repr == Seq_repr_raw) || (repr == Seq_repr_const)) 
					&& FlatCheckMol(bs)){
					insure_expresson_binding (the_set, NULL,  
						&internal_options);
					retval = FlattenSeq(div, default_date, fp, the_set, 
						NULL, FALSE, NULL, 0, 0 , internal_options);
				}
         break;

      case 2:
				retval = SeqEntryTracker(div, default_date, fp, 
					the_set, NULL, NULL);
         break;
      default:
		if ( !  (Flat_Be_quiet&1))
   ErrPost(CTX_NCBI2GB,CTX_2GB_BAD_CHOICE,
   "SeqEntryToGenbank: bad choice for SeqEntry=%d",
			(int) the_set -> choice);
		retval = FALSE;
   }

  if (Flat_Index_accession) {
  	FileClose(Flat_Index_accession);
	}

  if (Flat_Index_keyword) {
  	FileClose(Flat_Index_keyword);
	}

	 if (Flat_Index_author) {
	 	FileClose(Flat_Index_author);
	}

  if (Flat_Index_journal) {
  	FileClose(Flat_Index_journal);
	}

  if (Flat_Index_gene ) {
  	FileClose(Flat_Index_gene );
	}
	if (do_header ){
			headerPt -> loci += Number_loci;
			headerPt -> bases += Total_bases;
	}

   Nlm_ErrUserDelete(Accession_magic_cookie);
   Accession_magic_cookie = 0;

   Nlm_ErrUserDelete(Locus_magic_cookie);
   Locus_magic_cookie = 0;

	if (internal_options)
	AsnOptionFree ( & internal_options, OP_TOGENBNK, 
		OP_TOGENBNK_BINDINGS);
	return(retval);
}

/*----------- FlatOrgUse ()------*/
OrgRefPtr FlatOrgUse 
(SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, AsnOptionPtr optionHead)
{
	OrgRefPtr retval = NULL;
	ValNodePtr descr;
	BioseqPtr bs;
	BioseqSetPtr bss;
	SeqAnnotPtr annot = NULL;
	SEQ_ENTRY_STACK_PTR this_node;
	
	for ( this_node=the_nodes; this_node && ! retval; 
			this_node = this_node -> last){
		descr = this_node -> desc_nodes;
		if ( descr ){
			retval = FlatGenBankOrgDescr(descr, optionHead);
		}
	}
	if ( ! retval){
		switch (the_seq -> choice){
      case 1:   
			  bs = (BioseqPtr) ( the_seq->  data.ptrvalue) ;
			  annot = bs -> annot;
         break;

      case 2:
			  bss = (BioseqSetPtr) ( the_seq->  data.ptrvalue) ;
			  annot = bss -> annot;
         break;
   }
		if (annot){
			retval = FlatGenBankOrgAnnot(annot, optionHead);
		}
	for ( this_node=the_nodes; this_node && ! retval; 
			this_node = this_node -> last){
		annot = this_node -> annot_nodes;
		if ( annot ){
			retval = FlatGenBankOrgAnnot(annot, optionHead);
		}
	}
	}

	return retval;
}
/*----------- FlatSetPubs ()------*/

void FlatSetPubs 
(SEQ_ENTRY_STACK_PTR the_nodes, Int4 length, FLAT_PUB_STORE_PTRPTR refheadPtr, BioseqPtr bs, AsnOptionPtr optionHead)
{
   SeqAnnotPtr annot;
   ValNodePtr descr;

	for (; the_nodes ; the_nodes = the_nodes -> last){
      annot = the_nodes -> annot_nodes;
      descr = the_nodes -> desc_nodes;
      FlatDescrPubs(descr,length, refheadPtr, bs, optionHead);
      FlatFeatPubs(annot, length, refheadPtr, bs, optionHead);

	}
}
/*----------- FlatSetComment ()------*/

void FlatSetComment 
(FILE *fp, SeqEntryPtr the_seq, Boolean PNTR had_commentPt, SEQ_ENTRY_STACK_PTR the_nodes, FLAT_PUB_STORE_PTR refhead, AsnOptionPtr optionHead)
{
	ValNodePtr descr;
	CharPtr p, comment, com_cont = NULL;
	CharPtr left_side;
	CharPtr temp,kludge;

	for (; the_nodes ; the_nodes = the_nodes -> last){
		descr = the_nodes -> desc_nodes;
   if ( descr ){
      comment = FlatCommentDescr(descr, refhead, optionHead);
      if (comment ){
			if ( (Flat_Be_quiet&2)){
				for (p=comment; *p; p++){
					if (StringNCmp(p, "NCBI DETECTED", (size_t) 9) ==0 ||
					 		StringNCmp(p, "GenBank DETECTED", (size_t) 15) ==0){
						*p = '\0';
						break;
					}
				}
			}
				
			com_cont = FlatCont(comment, TRUE, optionHead);
			left_side = COM_BASE_LTR;
			if ( (* had_commentPt) )
				left_side = COM_BLANK_LTR;
			 (* had_commentPt)  = TRUE;
			MACRO_FLAT_LINE_OUT(temp,left_side,com_cont,kludge)
			if (com_cont){
				MemFree(com_cont);
				com_cont = NULL;
			}
			MemFree(comment);
		}
			/*---
			*		fprintf(fp, "COMMENT     %s\n",com_cont);
			----*/
		}
		comment = FlatCommentAnnot(the_nodes -> annot_nodes, refhead,
			 the_seq, optionHead);

		if (comment){
			com_cont = FlatCont(comment, TRUE, optionHead);
			left_side = COM_BASE_LTR;
			if ( (* had_commentPt) )
				left_side = COM_BLANK_LTR;
			MACRO_FLAT_LINE_OUT(temp,left_side,com_cont,kludge)

			if (com_cont){
				MemFree(com_cont);
				com_cont = NULL;
			}
			 (* had_commentPt)  = TRUE;
			MemFree(comment);
		}
		fflush(fp);

	}
}

/*****************************************************************************
*
*   GBGetAuthNames(AuthListPtr)
*   	Allocates a linked list of ValNode
*   	Each ->data.ptrvalue will have a string containing a formatted name
*   	Processes only AuthList of type "std"
*   		Returns NULL on any other type
*   	Caller must free returned linked list (call ValNodeFreeData())
*
*****************************************************************************/
ValNodePtr 
GBGetAuthNames (AuthListPtr ap)
{
	ValNodePtr namehead, cur, spare;
	CharPtr tmp;
	AuthorPtr authptr;
	PersonIdPtr pid;
	NameStdPtr nsp;

	if (ap == NULL)
		return NULL;

	namehead = NULL;   /* convert to strings */
	if (ap->choice != 1)
		return NULL;

	cur = NULL;
	for (spare = ap->names; spare; spare = spare->next)
	{
		cur = ValNodeNew(cur);
		if (namehead == NULL)
			namehead = cur;
		authptr = (AuthorPtr) spare->data.ptrvalue;
		pid = authptr->name;
		if (pid->choice == 2)  /* structured name */
		{
			nsp = (NameStdPtr) pid->data;
			if (nsp->names[0] != NULL)   /* last name */
			{
				tmp = MemNew((size_t)(StringLen(nsp->names[0]) + StringLen(nsp->names[4]) + StringLen(nsp->names[5]) + 3));
				cur->data.ptrvalue = tmp;
				tmp = StringMove(tmp, nsp->names[0]);
				if (nsp->names[4])
				{
					tmp = StringMove(tmp, Flat_format ?" ":",");
					tmp = StringMove(tmp, nsp->names[4]);
				}
				if (nsp->names[5]) /* suffix */
				{
					tmp = StringMove(tmp, " ");
					tmp = StringMove(tmp, nsp->names[5]);
				}
			}
			else if (nsp->names[3]) /* full name */
			{
				cur->data.ptrvalue = (Pointer)StringSave(nsp->names[3]);
			}
		}
		else if ((pid->choice == 3) || (pid->choice == 4)){
			cur->data.ptrvalue = (Pointer)StringSave((CharPtr)pid->data);
			if (Flat_format){
				for (tmp = (CharPtr) cur->data.ptrvalue; *tmp; tmp ++){
					if (*tmp == ','){
						*tmp = ' ';
						break;
					}
				}
			}
		}
	}
	return namehead;
}

