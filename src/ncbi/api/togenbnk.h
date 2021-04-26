/*

 MAINTENACE PAIN ALERT:
FlatOrganelle in tognbk2.c has constant names that need to
be kept in sync with asn.1

$Log:	togenbnk.h,v $
 * Revision 2.45  94/02/24  12:25:32  sirotkin
 * Removed local codon from Trna-ext.index lokup
 * replaced with seqport (ostell) function
 * 
 * Revision 2.44  94/02/16  17:25:19  sirotkin
 * Changed Keyword handling prototypes
 * to takes nodes pointing out, so GBBlocks
 * out of Bioseq could be used.
 * 
 * Revision 2.43  94/02/09  11:59:16  sirotkin
 * FlatIndirectStringCmp to LIBCALLBACK, not LIBCALL as per kans
 * 
 * Revision 2.42  94/02/07  14:10:53  sirotkin
 * Tom Madden's fix_pages_installed
 * 
 * Revision 2.41  93/12/09  14:05:08  sirotkin
 * SeqPort changes including invalid residue, tRNA formating and date priority changing
 * 
 * Revision 2.40  93/11/10  16:09:28  sirotkin
 * tRNA from ASN.1
 * 
 * Revision 2.39  93/11/04  15:20:03  ostell
 * added FlatGenBankBlockFind() which looks for a genbank block at higher
 * levels of bag. Called only to find taxonomy line.
 * 
 * Revision 2.38  93/10/19  11:05:00  sirotkin
 * GeneRef.desc that are in xref no longer generate misc_features
 * 
 * Revision 2.37  93/09/27  14:01:33  sirotkin
 * expanding tognbk3.c and tweaks to patent -m 4 format
 * 
 * Revision 2.36  93/09/01  16:51:48  sirotkin
 * has patent office format in ifdefs
 * 
 * Revision 2.35  93/07/12  10:47:53  sirotkin
 * No OrgRef forces /organism unknown
 * 
 * Revision 2.34  93/07/10  15:50:30  kans
 * prototypes for FlatGBQualsCntinAnnot and FlatGBQualsGetinAnnot
 * 
 * Revision 2.33  93/07/09  17:21:14  ostell
 * fix for type mismatches and prototypes
 * 
 * Revision 2.32  93/07/09  12:15:25  sirotkin
 * checks GeneRefs against known GBQuals before misc_feature
 * 
 * Revision 2.31  93/07/07  12:39:01  sirotkin
 * Patent DEFINITION line generated
 * 
 * Revision 2.30  93/07/01  14:59:57  sirotkin
 * FlatSourceFeatDoQual proto fix
 * 
 * Revision 2.29  93/07/01  14:56:45  sirotkin
 * split tognbk3 from tognbk1 for MacIntosh 32K limit
 * 
 * Revision 2.28  93/06/29  22:10:08  sirotkin
 * fixed FlatOrganelleDescr prototype
 * 
 * Revision 2.27  93/06/29  13:31:09  sirotkin
 * source feature now used both Seq_descr_mod and OrgRef.mod [tag value]
 * to generate qualifiers
 * 
 * Revision 2.26  93/06/25  14:31:24  kans
 * fixed prototype in FlatSource
 * 
 * Revision 2.25  93/06/25  13:15:55  sirotkin
 * source feature added, if none already
 * 
 * Revision 2.24  93/06/07  16:09:20  sirotkin
 * nit in proto for FlatCommendPubDesc
 * 
 * Revision 2.23  93/06/07  15:54:07  sirotkin
 * PUbDescPtr -> PubdecPtr
 * 
 * Revision 2.22  93/06/04  12:30:19  sirotkin
 * FlatCommendPubDesc added as
 *           ^ (Oh well)
 * function to hanlde all comments from PUbdesc and called from
 * FlatCommentAnnot (not just FlatCommentDesc).
 * 
 * Revision 2.21  93/05/25  15:29:31  sirotkin
 * gene.maploc no longer redundant with /map="text"
 * 
 * Revision 2.20  93/05/12  09:08:15  kans
 * Removed Flat_ErrPost
 * 
 * Revision 2.19  93/05/10  16:12:31  sirotkin
 * added rsite and region as misc_features
 * 
 * Revision 2.18  93/04/21  11:46:13  schuler
 * expression_binding_free declared as LIBCALLBACK
 * 
 * Revision 2.17  93/04/20  11:12:28  schuler
 * PASCAL --> LIBCALLBACK
 * 
 * Revision 2.16  93/04/19  11:38:47  sirotkin
 * embl right side numbers working
 * 
 * Revision 2.15  93/04/15  12:54:49  schuler
 * Chnaged a _cdecl to a PASCAL .
 * 
 * Revision 2.14  93/04/12  12:10:02  sirotkin
 * Locus names collisions names picked up in SEQID_GENERAL as made by id database
 * 
 * Revision 2.13  93/04/06  13:15:05  sirotkin
 *  hyphen to N fix
 * 
 * Revision 2.12  93/04/02  16:59:59  sirotkin
 * No more SeqIdFindBest in asn2gnbk, SeqIdPrint now handles local.str as accession
 * 
 * Revision 2.11  93/03/10  15:19:11  sirotkin
 * embl mode started
 * 
 * Revision 2.6  93/01/13  09:12:17  sirotkin
 * before starting on asn2{embl,flat}
 * 
 * Revision 2.5  93/01/05  15:35:31  sirotkin
 * *** empty log message ***
 * 
 * Revision 2.4  92/12/28  11:38:23  sirotkin
 * intermeldiate, for safety
 * 
 * Revision 2.3  92/11/25  08:51:09  sirotkin
 * communication be GBQuals and ComposeNote
 * 
 * Revision 2.2  92/11/20  09:08:16  sirotkin
 * prototype fixes, ala Kans
 * 
 * Revision 2.1  92/11/17  16:09:22  sirotkin
 * Major changes to the way /note and /standard_name interact with
 * /product. etc.
 * 
 * Revision 2.0  92/10/06  17:37:26  ostell
 * force to 2.0
 * 
 * Revision 1.47  92/10/02  09:40:05  sirotkin
 * fixed /note="" trouble two ways
 * 
 * Revision 1.46  92/10/01  14:08:51  sirotkin
 * Goes with tognbk1.c 1.58
 * 
 * Revision 1.45  92/10/01  11:06:51  sirotkin
 * With SeqFeatXref, FlatAnnot rearrangment, before virtual preprossing
 * 
 * Revision 1.44  92/09/22  14:45:42  sirotkin
 * Origin wrap, /note extra wrap, consistent use of LOCUS S\#
 * 
 * Revision 1.43  92/08/28  10:05:04  sirotkin
 * xref accessions for GenBank, EMBL, and DDBJ now as 2ndary on ACCESSION line
 * 
 * Revision 1.42  92/07/10  17:27:00  sirotkin
 * Now deals with reuse of genes and ProtRefs from multiple CDS better, but will no longer accept ProtRefs on DNA
 * 
 * Revision 1.41  92/06/22  15:39:03  ostell
 * cplusplus
 * 
 * Revision 1.40  92/06/19  15:17:18  ostell
 * *** empty log message ***
 * 
 * Revision 1.39  92/05/13  16:06:59  kans
 * *** empty log message ***
 * 
 * Revision 1.38  92/05/08  16:48:46  sirotkin
 * Julie changes all incorporated
 * 
 * Revision 1.37  92/05/06  12:15:54  kans
 * *** empty log message ***
 * 
 * Revision 1.36  92/05/06  11:08:04  kans
 * *** empty log message ***
 * 
 * Revision 1.35  92/05/06  10:08:48  sirotkin
 * merging in Jonathan's changes
 * 
 * Revision 1.34  92/05/06  09:56:15  sirotkin
 * seems to work
 * 
 * Revision 1.32  92/04/16  10:19:00  sirotkin
 * works daily update output, has _class
 * 
 * Revision 1.31  92/04/09  13:57:00  sirotkin
 * added DateFromCreate() for  LOCUS date, cleaned up MolId for seg_set LOCUS name
 * 
 * Revision 1.30  92/04/02  11:34:07  sirotkin
 * Added capibility to get Org info from Annot
 * 
 * Revision 1.29  92/03/06  09:34:37  ostell
 * AsnNode=ValNode,AsnValue=DataVal,AsnValueNode=AsnValxNode
 * 
 * Revision 1.28  92/02/13  15:33:58  sirotkin
 * more PROTO tweaks
 * 
 * Revision 1.27  92/02/13  15:19:36  sirotkin
 * PROTO change for FLATLOCUS for AsnOption stuff
 * 
 * Revision 1.26  92/02/13  12:22:51  sirotkin
 * FlatSafeASnThere added
 * 
 * Revision 1.25  92/02/13  12:00:21  sirotkin
 * FlatCommentOptions PROTO added.
 * 
 * Revision 1.24  92/02/13  11:35:30  sirotkin
 * added option for passing comment through
 * 
 * Revision 1.23  92/02/12  11:12:04  sirotkin
 * additional Organnelle prototype
 * 
 * Revision 1.22  92/02/11  17:29:47  sirotkin
 * *** empty log message ***
 * 
 * Revision 1.21  92/02/11  17:26:43  sirotkin
 * Now deals with Organelle names
 * 
 * Revision 1.20  92/02/06  11:03:56  kans
 * *** empty log message ***
 * 
 * Revision 1.19  92/01/03  16:03:17  sirotkin
 * Unconditionally uses segment number on LOCUS if not genbank
 * 
 * Revision 1.18  92/01/02  21:13:08  sirotkin
 * many header bugs fixed
 * 
 * Revision 1.17  92/01/02  17:56:56  sirotkin
 * has hooks for header and shortdir information
 * 
 * Revision 1.16  92/01/02  11:24:57  ostell
 * *** empty log message ***
 * 
 * Revision 1.15  92/01/01  12:06:30  kans
 * *** empty log message ***
 * 
 * Revision 1.14  91/12/31  14:58:43  ostell
 * #includes
 * 
 * Revision 1.12  91/12/26  10:01:34  ostell
 * prototype fixes
 * 
 * Revision 1.11  91/12/20  10:54:06  sirotkin
 * intermediate fixes for GenBank
 * 
 * Revision 1.10  91/12/11  11:22:56  sirotkin
 * fixed stray character within prototype for Joanthan
 * 
 * Revision 1.9  91/12/06  08:51:17  sirotkin
 * snapshot for Warren
 * 
 * Revision 1.8  91/12/03  16:52:54  sirotkin
 * has all prototypes for tognbk[12].c files added
 * 
 * Revision 1.7  91/11/27  13:20:19  sirotkin
 * error message contexts
 * 
 * Revision 1.6  91/11/01  15:53:18  sirotkin
 * T10 format done
 * 
 * Revision 1.5  91/10/29  14:26:08  sirotkin
 * new option define
 * 
 * Revision 1.4  91/10/22  09:52:50  sirotkin
 * Changed prototypes to implement Options and
 * to add (bases 1 to <length>) on reference line
 * 
 * Revision 1.3  91/10/02  16:38:56  sirotkin
 * added CharPtr param to PROTO
 * 
 * Revision 1.2  91/09/25  09:13:47  sirotkin
 * Name change of prototype
 * 
*/

#include <seqport.h>

#ifdef __cplusplus
extern "C" {
#endif


#define OP_TOGENBNK_DIV ( (Int2)-1)
#define OP_TOGENBNK_DATE ( (Int2)-2)
#define OP_TOGENBNK_NO_NCBI ( (Int2)-3)
#define OP_TOGENBNK_QUIET ( (Int2)-4)
#define OP_TOGENBNK_BINDINGS ( (Int2)-5)
#define OP_TOGENBNK_INDEX ( (Int2) -7) 
#define OP_TOGENBNK_HEADER ( (Int2) -9) 
#define OP_TOGENBNK_COMMENT ( (Int2) -10) 
#define OP_TOGENBNK_LOCUS_S ( (Int2) -11) 
#define OP_TOGENBNK_FORMAT ( (Int2) -12) 
#define OP_TOGENBNK_HYBRID ( (Int2) -13) 
#define OP_TOGENBNK_RELEASE ( (Int2) -14) 
#define OP_TOGENBNK_ORGREF ( (Int2) -15) 
#define OP_TOGENBNK_SOURCE_FOUND ( (Int2) -16) 
#define OP_TOGENBNK_SEQ_DESCR ( (Int2) -17) 
#define OP_TOGENBNK_GENE_GBQUAL ( (Int2) -18) 
#define OP_TOGENBNK_MAP_GBQUAL ( (Int2) -19) 
#define OP_TOGENBNK_NUCL_PROT ( (Int2) -20) 

/*-------removed (at least for now....
		#define OP_TOGENBNK_CDROM ( (Int2) -8) 
		#define OP_TOGENBNK_T10 ( (Int2)-6)
-----------*/


#define CTX_2GB_MISSING_GIBBSQ 1
#define CTX_2GB_NOT_SEQ 2
#define CTX_2GB_BAD_REPR 3
#define CTX_2GB_NO_LOCUS 4
#define CTX_2GB_NOT_IMPLEMENTED 5
#define CTX_2GB_NOT_NA 6
#define CTX_2GB_NOT_BIOSEQ 7
#define CTX_2GB_BAD_CHOICE 8
#define CTX_2GB_LOCATION_TROUBLE 9
#define CTX_2GB_INTERNAL 10
#define CTX_2GB_NO_ACCESSION 11
#define CTX_2GB_XREF_BACKBONE 12
#define CTX_2GB_INDEX_FILE_OPEN 13
#define CTX_2GB_DUP_SERIAL 14
#define CTX_2GB_ARG_TROUBLE 15
#define CTX_2GB_LOCUS_LEN  16
#define CTX_2GB_BAD_RESIDUE 17
#define CTX_2GB_DATE_STRING 18
#define CTX_2GB_BAD_LOC 19
#define CTX_2GB_IMP_FEAT_CDS 20

#define ASN2GNBK_FORMAT_EMBL 1
#define ASN2GNBK_FORMAT_EMBL_SEQ_LEFT 2
#define ASN2GNBK_FORMAT_EMBL_SEQ_RIGHT 3
#define ASN2GNBK_FORMAT_PATENT_EMBL 4

typedef struct seq_entry_stack {
		struct seq_entry_stack PNTR last;
		int max_seg, this_seg;
		ValNodePtr desc_nodes;
		SeqAnnotPtr annot_nodes;
		SeqEntryPtr seg_seq;       /* segmented seq of which this is a part */
} SEQ_ENTRY_STACK, PNTR SEQ_ENTRY_STACK_PTR;

typedef struct struct_gene_qual_known {
		Int4 count;
		CharPtr PNTR array;
} GeneQualKnown, PNTR GeneQualKnownPtr;

/*--------types_bindings ()------------*/

typedef struct bind_feat_count {
	int cdregions;
	int genes;
	int prots;
} BIND_FEAT_COUNTS, PNTR BIND_FEAT_COUNTS_PTR;

typedef struct bind_feat{
	int dex_cdregions;
	int dex_genes;
	int dex_prots;
	SeqFeatPtr PNTR cdregions;
	SeqFeatPtr PNTR genes;
	SeqFeatPtr PNTR prots;
} BIND_FEAT, PNTR BIND_FEAT_PTR;

typedef struct bindings {
	int PNTR dex_genes; /* array of indeces into cdregions, or -1 */
	int PNTR dex_prots; /* array of indeces into cdregions, or -1 */
	Boolean PNTR gene_xreffed;  /* if dex -1, set TRUE if in cds xref */
	Boolean PNTR prot_xreffed;
} ImplicitBindings, PNTR ImplicitBindingsPtr;

typedef struct option_expressons {
	ImplicitBindingsPtr bindings;
	BIND_FEAT_PTR features;
} Expressons, PNTR ExpressonsPtr;

typedef struct option_header {
	Int4 loci;
	Int4 bases;
} OptionHeader, PNTR OptionHeaderPtr;


/*-------- FeatureState_defined()---*/

typedef struct feature_states {
	Uint1 type;
	int last_dex;
	int target_cds_dex;
	SeqFeatXrefPtr head_xref, this_xref; /* priority is to follow this chain */
	ImplicitBindingsPtr bindings;
	BIND_FEAT_PTR features;
} FeatureState, PNTR FeatureStatePtr;

/*-- need to delay call to FlatPubOut until sort on serial number 
----------*/
typedef struct pub_store {
   struct pub_store PNTR next;
   Int2 serial_number;
   Int4 muid; /* alternative identifier */
	Boolean site_only;
   ValNodePtr the_pub;  /* not Pubdesc */
   Int4 length;
   ValNodePtr location;
		Int4 pat_seqid;  /* for patents, the seqid from bs.id */
} FLAT_PUB_STORE, PNTR FLAT_PUB_STORE_PTR, 
		PNTR PNTR FLAT_PUB_STORE_PTRPTR;


#define SOURCE_FEATURE_NAME "source"
#define COMMENT_FEATURE_NAME "misc_feature"
#define PROT_FEATURE_NAME "misc_feature"
#define XREF_FEATURE_NAME "misc_feature"
#define RSITE_FEATURE_NAME "misc_feature"
#define REGION_FEATURE_NAME "misc_feature"

#define GENE_FEATURE_NAME "misc_feature"
#define GENE_NOTE_QUAL_PREFIX "This feature applies to a gene which lacks a coding region feature. "

#define MACRO_FLAT_LINE_OUT(TEMP,LTR,CONT,KLUDGE) \
		{ if (Flat_do_Flat){\
   TEMP = KLUDGE = MemNew ( (StringLen(LTR) +\
          StringLen (CONT) + 2) );\
   TEMP = StringMove (TEMP, LTR);\
   TEMP = StringMove (TEMP, CONT);\
   TEMP = StringMove (TEMP, "\n");\
		if (Flat_format != 0) \
			KLUDGE = EMBL_Cont_Cleanup (KLUDGE);\
   FilePuts(KLUDGE, fp);\
   if (KLUDGE)\
      MemFree(KLUDGE);\
		}}

int SeqEntryToGenbank PROTO((FILE *, SeqEntryPtr, AsnOptionPtr));
CharPtr FlatCont PROTO((CharPtr Line, Boolean do_tilde, AsnOptionPtr aop));
CharPtr do_FlatCont PROTO((int limit, int blanks, CharPtr line, Boolean do_tilde, AsnOptionPtr aop));
CharPtr EMBL_Cont_Cleanup PROTO((CharPtr line));
CharPtr do_FlatContKey PROTO((int limit, int start, CharPtr Line, AsnOptionPtr aop));
CharPtr do_FlatContOrigin PROTO((int limit, int start, CharPtr Line, AsnOptionPtr aop));
GBBlockPtr FlatGenBankBlock PROTO (( ValNodePtr descr , AsnOptionPtr aop));
EMBLBlockPtr FlatEMBLBlock PROTO (( ValNodePtr descr , AsnOptionPtr aop));
TextSeqIdPtr FlatGenBankIds PROTO(( SeqIdPtr id, AsnOptionPtr aop));
CharPtr FlatStringParanGroup PROTO(( CharPtr Start, ValNodePtr head,
	CharPtr first_delimit, CharPtr delimit, CharPtr period, AsnOptionPtr aop));
CharPtr FlatStringGroup PROTO(( CharPtr Start, ValNodePtr head,
						CharPtr delimit, CharPtr period, AsnOptionPtr aop));
CharPtr FlatStringGroupTerm PROTO(( CharPtr Start, ValNodePtr head,
			CharPtr delimit, CharPtr period, CharPtr term, AsnOptionPtr aop));
Boolean FlatVirtLoc PROTO (( SeqEntryPtr the_seq , ValNodePtr location ));
CharPtr FlatLoc PROTO (( SeqEntryPtr the_seq , ValNodePtr location,
					AsnOptionPtr aop));
CharPtr FlatLocNote PROTO (( SeqEntryPtr the_seq , ValNodePtr location,
					AsnOptionPtr aop));
OrgRefPtr FlatGenBankOrgDescr PROTO ((ValNodePtr fhead, AsnOptionPtr aop));
OrgRefPtr FlatGenBankOrgAnnot PROTO ((SeqAnnotPtr annot, AsnOptionPtr aop));
Int4 SeqLocSubset PROTO((SeqLocPtr outer_loc, SeqLocPtr inner_loc));
Boolean this_gene_is_bound PROTO((SeqFeatPtr gene_sfp,
					AsnOptionPtr optionHead));
Boolean this_prot_is_bound PROTO((SeqFeatPtr prot_sfp,
					AsnOptionPtr optionHead));
FeatureStatePtr bound_gene_next PROTO((GeneRefPtr PNTR bound_genePtr,
					FeatureStatePtr state));
FeatureStatePtr find_bound_genes_init PROTO((SeqFeatPtr sfp, Boolean did_qual_gene,
					GeneRefPtr PNTR bound_genePtr, AsnOptionPtr optionHead));
FeatureStatePtr bound_prot_next PROTO((ProtRefPtr PNTR bound_protPtr,
					FeatureStatePtr state));
FeatureStatePtr find_bound_prots_init PROTO((SeqFeatPtr sfp, Boolean did_qual_product,
					ProtRefPtr PNTR bound_protPtr, AsnOptionPtr optionHead));
int find_xref PROTO((SeqFeatPtr feat, SeqFeatPtr PNTR cds, int num, int prot_or_gene));
int find_binding PROTO((SeqFeatPtr feat, SeqFeatPtr PNTR cds, int num, Int4Ptr scores, int prot_or_gene, int PNTR found_already, int num_found));
void match_feats_with_cdregions PROTO((ExpressonsPtr express));
void LIBCALLBACK expresson_binding_free PROTO((ExpressonsPtr expressonsptr));
void bind_feature_counter PROTO((SeqEntryPtr sep, Pointer in_data, Int4 index,
																Int2 indent));
void count_feats_in_annot PROTO((SeqAnnotPtr annot, BIND_FEAT_COUNTS_PTR data));
void bind_feature_finder PROTO((SeqEntryPtr sep, Pointer in_data, Int4 index,
																Int2 indent));
void store_feats_in_annot PROTO((SeqAnnotPtr annot, BIND_FEAT_PTR data));
Boolean insure_expresson_binding PROTO((SeqEntryPtr the_set,
				SEQ_ENTRY_STACK_PTR data, AsnOptionPtr PNTR  optionHeadPtr));
Boolean check_for_expresson_binding PROTO((SeqEntryPtr the_set,
				SEQ_ENTRY_STACK_PTR data, AsnOptionPtr PNTR  optionHeadPtr));
CharPtr FlatLocHalf PROTO((CharPtr, Int4, IntFuzzPtr, AsnOptionPtr));
CharPtr FlatLocHalfCaret PROTO((CharPtr, Int4, IntFuzzPtr, AsnOptionPtr));
Boolean FlatLocPoint PROTO((SeqIdPtr, SeqIdPtr, CharPtr, Int4,
									IntFuzzPtr, AsnOptionPtr));
Boolean FlatLocCaret PROTO((SeqIdPtr, SeqIdPtr, CharPtr, Int4, IntFuzzPtr, AsnOptionPtr));
CharPtr FlatSmartStringMove PROTO((CharPtr, Int4Ptr, CharPtr, CharPtr));
Boolean FlatNullAhead PROTO((SeqEntryPtr the_seq, ValNodePtr location));
CharPtr FlatPackedPoint PROTO((CharPtr, CharPtr, Int4Ptr, PackSeqPntPtr, SeqIdPtr, AsnOptionPtr));
Boolean FlatLocElement PROTO((SeqEntryPtr, ValNodePtr, CharPtr, AsnOptionPtr));
CharPtr complement_FlatLoc PROTO (( SeqIdPtr, Boolean PNTR, CharPtr, CharPtr, Int4Ptr, SeqEntryPtr, ValNodePtr, AsnOptionPtr));
CharPtr group_FlatLoc PROTO(( SeqIdPtr, Boolean PNTR, int, CharPtr, CharPtr, Int4Ptr, SeqEntryPtr, ValNodePtr, AsnOptionPtr));
Boolean starts_complement PROTO((ValNodePtr));
CharPtr FlatLoc PROTO((SeqEntryPtr, ValNodePtr, AsnOptionPtr));
CharPtr do_FlatLoc PROTO((Boolean PNTR, Boolean, SeqIdPtr, CharPtr, CharPtr, Int4Ptr, SeqEntryPtr, ValNodePtr, AsnOptionPtr));
CharPtr FlatStringGroup PROTO((CharPtr, ValNodePtr, CharPtr, CharPtr,
												AsnOptionPtr));
int FlatResidues PROTO((FILE *, SeqEntryPtr, AsnOptionPtr));
int sqputc PROTO((FILE *, int));
int FlatAnnot PROTO((FILE *, SeqEntryPtr, SeqAnnotPtr, Int2 PNTR, Boolean,
											FLAT_PUB_STORE_PTR, AsnOptionPtr PNTR));
int FlatFeatures PROTO((FILE *, SeqEntryPtr, SEQ_ENTRY_STACK_PTR,
											FLAT_PUB_STORE_PTR, AsnOptionPtr PNTR));
Boolean non_quote_qual PROTO((CharPtr qual));
CharPtr FlatMainAccession PROTO((SeqEntryPtr, AsnOptionPtr));
CharPtr FlatAllAccessions PROTO((SeqEntryPtr, SEQ_ENTRY_STACK_PTR, AsnOptionPtr));
CharPtr FlatXrefAccession PROTO (( SeqFeatPtr sfp, ValNodePtr location , Int4Ptr lenPt, CharPtr temp));
int FlatAccession PROTO((FILE *, SeqEntryPtr, SEQ_ENTRY_STACK_PTR, AsnOptionPtr));
int FlatBaseCount PROTO((FILE *, SeqEntryPtr, AsnOptionPtr));
CharPtr FlatCommentAnnot PROTO((SeqAnnotPtr, FLAT_PUB_STORE_PTR, SeqEntryPtr, AsnOptionPtr));
void FlatCommentPubDesc PROTO ((PubdescPtr pd, FLAT_PUB_STORE_PTR refhead, CharPtr PNTR tempPt, Int4Ptr lengthPt, AsnOptionPtr optionHead));
CharPtr FlatCommentDescr PROTO((ValNodePtr, FLAT_PUB_STORE_PTR, AsnOptionPtr));
int FlatComment PROTO((FILE *, SeqEntryPtr, SEQ_ENTRY_STACK_PTR,
											FLAT_PUB_STORE_PTR, AsnOptionPtr));
void FlatSetComment PROTO((FILE *, SeqEntryPtr, Boolean PNTR , SEQ_ENTRY_STACK_PTR,
										FLAT_PUB_STORE_PTR, AsnOptionPtr));
ValNodePtr FlatTitleNode PROTO((ValNodePtr, AsnOptionPtr));
CharPtr FlatTitle PROTO((SeqEntryPtr, SEQ_ENTRY_STACK_PTR, AsnOptionPtr));
int FlatDefinition PROTO((FILE *, SeqEntryPtr, SEQ_ENTRY_STACK_PTR, AsnOptionPtr optionHead));
CharPtr FlatDefPat PROTO((SeqEntryPtr the_seq, AsnOptionPtr optionHead));
int IS_minus_strand PROTO((SeqLocPtr location, AsnOptionPtr aop));
SeqIdPtr SeqFirstLocId PROTO((SeqLocPtr location, AsnOptionPtr aop));
int do_this_feature_now  PROTO((SeqEntryPtr the_seq, SeqLocPtr location,
														AsnOptionPtr aop));
void FlatGBQualsCntinAnnot PROTO((SeqEntryPtr the_seq, SeqAnnotPtr annot, AsnOptionPtr PNTR optionHead, Int4Ptr cnt_gene, Int4Ptr cnt_map));
void FlatGBQualsGetinAnnot PROTO((SeqEntryPtr the_seq, SeqAnnotPtr annot, AsnOptionPtr PNTR optionHead, CharPtr PNTR gene_quals,
		Int4Ptr cnt_gene, CharPtr PNTR map_quals, Int4Ptr cnt_map));
CharPtr FlatAllKeys PROTO((SeqEntryPtr, SEQ_ENTRY_STACK_PTR,AsnOptionPtr));
int FlatKeys PROTO((FILE *, SeqEntryPtr, SEQ_ENTRY_STACK_PTR, AsnOptionPtr));
GBBlockPtr FlatGenBankBlock PROTO((ValNodePtr, AsnOptionPtr));
GBBlockPtr FlatGenBankBlockFind PROTO((ValNodePtr, AsnOptionPtr, SEQ_ENTRY_STACK_PTR));
CharPtr FlatLocusName PROTO((SeqEntryPtr, SeqEntryPtr, AsnOptionPtr));
Int4 FlatLocusBases PROTO((SeqEntryPtr, AsnOptionPtr));
CharPtr FlatLocusMolType PROTO((char[7 ], SeqEntryPtr, AsnOptionPtr));
CharPtr FlatLocusCircular PROTO((CharPtr, SeqEntryPtr, AsnOptionPtr));
CharPtr FlatLocusDate PROTO(( CharPtr, SeqEntryPtr, SEQ_ENTRY_STACK_PTR , AsnOptionPtr));
CharPtr FlatDateFromCreate PROTO(( CharPtr, NCBI_DatePtr));
CharPtr FlatLocusDiv PROTO((SeqEntryPtr, SEQ_ENTRY_STACK_PTR , AsnOptionPtr));
CharPtr FlatLocusDivDescr PROTO ((ValNodePtr descr, AsnOptionPtr optionHead));
NCBI_DatePtr FlatLocusDateDescr PROTO ((CharPtr default_date, ValNodePtr descr, Boolean PNTR used_strPt, CharPtr PNTR best_stringPt, AsnOptionPtr optionHead));
CharPtr FlatEMBLClass PROTO((SeqEntryPtr the_seq, SeqEntryPtr seg_seq, AsnOptionPtr optionHead ));
int FlatLocus PROTO((CharPtr, CharPtr, FILE *, SeqEntryPtr, SEQ_ENTRY_STACK_PTR , SeqEntryPtr, int, int,Boolean, Int4Ptr, AsnOptionPtr PNTR));
int add_seg_digits PROTO((SeqEntryPtr, CharPtr, int, AsnOptionPtr));
OrgRefPtr FlatGenBankOrgDescr PROTO((ValNodePtr, AsnOptionPtr));
CharPtr FlatSourceLine PROTO((SeqEntryPtr, SEQ_ENTRY_STACK_PTR,
											OrgRefPtr PNTR, AsnOptionPtr));
Boolean FlatMoreOrganism PROTO((SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, 
								OrgRefPtr PNTR OrgRefPtrPt, OrgRefPtr last_or, 
								AsnOptionPtr optionHead));
int FlatSource PROTO((FILE *fp, SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, AsnOptionPtr PNTR optionHeadPt));
int FlattenSeq PROTO((CharPtr, CharPtr, FILE *, SeqEntryPtr,
						SEQ_ENTRY_STACK_PTR, Boolean, SeqEntryPtr, int, int, AsnOptionPtr));
int FlatOrigin PROTO((FILE *, SeqEntryPtr, AsnOptionPtr));
int SeqEntryTracker PROTO((CharPtr, CharPtr, FILE *, SeqEntryPtr,
										SEQ_ENTRY_STACK_PTR, AsnOptionPtr));
OrgRefPtr FlatOrgUse PROTO((SeqEntryPtr, SEQ_ENTRY_STACK_PTR, AsnOptionPtr));
void FlatSetPubs PROTO((SEQ_ENTRY_STACK_PTR, Int4, FLAT_PUB_STORE_PTRPTR,
		BioseqPtr bs, AsnOptionPtr));
int FlatSetFeats PROTO((FILE *, SeqEntryPtr, Int2Ptr, SEQ_ENTRY_STACK_PTR,
											FLAT_PUB_STORE_PTR, AsnOptionPtr PNTR));
void FlatSourceFeatDoQual PROTO((FILE * fp, CharPtr qual_use, CharPtr val, int dex));
void FlatInsureSourceFeature PROTO ((FILE *fp, SeqEntryPtr the_seq, Int2Ptr feat_count, AsnOptionPtr optionHead));
CharPtr FlatAuthors PROTO((ValNodePtr, AuthListPtr, AsnOptionPtr));
CharPtr FlatPubTitle PROTO((ValNodePtr, AsnOptionPtr));
CharPtr FlatJournal PROTO((ValNodePtr, Int4, AsnOptionPtr));
Boolean FlatIgnoreThisPatentPub PROTO (( BioseqPtr bsp, ValNodePtr best, Int4Ptr seqidPt));
Int2 fix_pages PROTO((CharPtr, CharPtr));
void FlatPubOut PROTO((FILE *, ValNodePtr, Int4 , ValNodePtr, BioseqPtr, Int4,
			AsnOptionPtr, Boolean, Int4));
int  FlatRefGet PROTO((PubdescPtr pd, FLAT_PUB_STORE_PTR refhead));
void FlatDescrPubs PROTO(( ValNodePtr , Int4, FLAT_PUB_STORE_PTRPTR, 
		BioseqPtr bs, AsnOptionPtr));
void FlatFeatPubs PROTO((SeqAnnotPtr, Int4, FLAT_PUB_STORE_PTRPTR, 
		BioseqPtr bs, AsnOptionPtr));
int FlatRefs PROTO((FILE *, SeqEntryPtr, SEQ_ENTRY_STACK_PTR,
									FLAT_PUB_STORE_PTRPTR , AsnOptionPtr));
Boolean is_not_virtual PROTO((SeqEntryPtr));
Boolean is_real_id PROTO((SeqIdPtr, SeqIdPtr));
CharPtr FlatCommentNum PROTO((PubdescPtr));
CharPtr FlatCommentOptions PROTO(( Boolean had_comment, AsnOptionPtr optionHead));
CharPtr FlatCleanEquals PROTO((CharPtr));
CharPtr FlatXrefName PROTO((ValNodePtr , int));
CharPtr Flat_Clean_store PROTO(( CharPtr target, CharPtr data));
Boolean Flat_is_book PROTO((CharPtr value));
void Flat_out_index_line PROTO((FILE * fp, CharPtr value));
int LIBCALLBACK COMP_serial PROTO((VoidPtr v1, VoidPtr v2));
void FlatRefClear  PROTO((FLAT_PUB_STORE_PTR head));

ValNodePtr FlatRefBest PROTO (( ValNodePtr equiv));
void FlatStoreEquivPub PROTO (( ValNodePtr equiv ,
		FLAT_PUB_STORE_PTRPTR refheadPtr, BioseqPtr bs, 
		Int4 length, ValNodePtr location, Boolean site_only));
void FlatStoreThisPub PROTO(( FLAT_PUB_STORE_PTRPTR refheadPtr,
	ValNodePtr best, Int2 serial_number, Int4 muid, BioseqPtr bsp,
		Int4 length, ValNodePtr location, Boolean site_only));

void FlatDumpPubs PROTO(( FILE *, Int4, FLAT_PUB_STORE_PTR, 
		BioseqPtr , AsnOptionPtr));
Boolean non_quote_val PROTO ((CharPtr));
CharPtr FlatOrganelle PROTO (( SeqEntryPtr the_seq, SEQ_ENTRY_STACK_PTR the_nodes, AsnOptionPtr PNTR optionHead));
CharPtr FlatOrganelleDescr PROTO (( ValNodePtr descr, AsnOptionPtr PNTR optionHead));
Boolean FlatSafeThere PROTO (( CharPtr name));
Boolean FlatSafeAsnThere PROTO (( ValNodePtr name));
Boolean FlatSeqFeatHasXref PROTO (( SeqFeatPtr sfp, Uint1 choice));
Boolean FlatSeqFeatHasQual PROTO (( SeqFeatPtr sfp, CharPtr));
void FlatGBQualCntinAnnot PROTO((SeqEntryPtr the_seq, SeqAnnotPtr annot, AsnOptionPtr PNTR optionHead, Int4Ptr cnt_gene, Int4Ptr cnt_map));
void FlatGBQualGetinAnnot PROTO((SeqEntryPtr the_seq, SeqAnnotPtr annot, AsnOptionPtr PNTR optionHead, CharPtr PNTR gene_quals, Int4Ptr cnt_gene, CharPtr PNTR map_quals, Int4Ptr cnt_map));
Int2 FlatBinSearch PROTO ((Int4 count, CharPtr PNTR list, CharPtr query));
int LIBCALLBACK FlatIndirectStringCmp PROTO((VoidPtr v1, VoidPtr v2));
void FlatAnnotGBQual PROTO ((FILE *fp, SeqFeatPtr  sfp,
	Boolean PNTR did_partial, CharPtr PNTR note_qual_text,
	CharPtr PNTR standard_name_qual_text, CharPtr PNTR gene_qual_text,
	CharPtr PNTR map_qual_text,
	Boolean PNTR did_gbqual_gene, Boolean PNTR did_gbqual_product,
	Boolean doing_CDS_from_product,
	AsnOptionPtr optionHead));
Boolean FlatEndsPunc PROTO(( CharPtr string));
void FlatAnnotComposeNote PROTO ((SeqEntryPtr the_seq, SeqFeatPtr sfp, BioseqPtr protein,
	CdRegionPtr cds, CharPtr note_text, CharPtr standard_name_text,
	CharPtr gene_text, Boolean did_gbqual_product, Boolean did_gbqual_gene,
	CharPtr feat_type, CharPtr total_buf, Int4Ptr lenPt, CharPtr temp,
	FLAT_PUB_STORE_PTR refhead, Boolean doing_CDS_from_product,
	 AsnOptionPtr optionHead));
Boolean FlatAnnotAvoidPartial PROTO (( SeqFeatPtr sfp ));
void FlatPubPatStart PROTO ((FILE * fp, ValNodePtr the_pub, Int4 pat_seqid, AsnOptionPtr optionHead));
ValNodePtr GBGetAuthNames PROTO((AuthListPtr ap));
Boolean FlatCheckMol PROTO ((BioseqPtr bsp));
CharPtr FlatComposetRNANote PROTO (( tRNAPtr trna, CharPtr temp, CharPtr total_buf, Int4Ptr lenPt, int PNTR need_spPt));
CharPtr SeqCodeNameGet PROTO ((SeqCodeTablePtr table, Uint1 residue));
CharPtr SeqCodeSymbolGet PROTO ((SeqCodeTablePtr table, Uint1 residue));


#define XREF_BACKBONE 1
#define XREF_OBJECT 2


#define TOUPPER(c) ( isupper(c)?(c):toupper(c))

#define GENBANK_ACC_BASE_LTR    "ACCESSION   "
#define GENBANK_COM_BASE_LTR    "COMMENT     "
#define GENBANK_COM_BLANK_LTR   "            "
#define GENBANK_DEF_BASE_LTR    "DEFINITION  "
#define GENBANK_Blank_BASE_LTR  "     "
#define GENBANK_Key_BASE_LTR    "KEYWORDS    "
#define GENBANK_ORI_BASE_LTR    "ORIGIN      "
#define GENBANK_REF_BASE_LTR    "REFERENCE   "
#define GENBANK_AUTHOR_LTR      "  AUTHORS   "
#define GENBANK_TITLE_LTR       "  TITLE     "
#define GENBANK_JOURNAL_LTR     "  JOURNAL   "
#define GENBANK_STANDARD_LTR    "  STANDARD  "
#define GENBANK_SOU_BASE_LTR    "SOURCE      "
#define GENBANK_ORG_LTR         "  ORGANISM  "
#define GENBANK_FEAT_CONT_BLANKS "                     "

#define EMBL_ACC_BASE_LTR    "AC   "
#define EMBL_COM_BASE_LTR    "CC   "
#define EMBL_COM_BLANK_LTR   "CC   "
#define EMBL_DEF_BASE_LTR    "DE   "
#define EMBL_Blank_BASE_LTR  "FT   "
#define EMBL_Key_BASE_LTR    "KW   "
#define EMBL_REF_BASE_LTR    "RN   "
#define EMBL_REF_BASE_LTR_BP "RP   "
#define EMBL_AUTHOR_LTR      "RA   "
#define EMBL_TITLE_LTR       "RT   "
#define EMBL_JOURNAL_LTR     "RL   "
#define EMBL_ORG_LTR         "OS   "
#define EMBL_ORG_LTR_nel     "OG   "
#define EMBL_ORG_LTR_tax     "OC   "
#define EMBL_FEAT_CONT_BLANKS "FT                   "

#define PATENT_FORMAT_PN "PN   "
#define PATENT_FORMAT_PD "PD   "
#define PATENT_FORMAT_PF "PF   "
#define PATENT_FORMAT_PR "PR   "
#define PATENT_FORMAT_PA "PA   "
#define PATENT_FORMAT_PI "PI   "
#define PATENT_FORMAT_PT "PT   "
#define PATENT_FORMAT_PC "PC   "

#define SeqFeatGeneChoice 1
#define SeqFeatProtChoice 4

/*-- this is for campatibility is IGG/T10 software ---*/
#define SORT_SEPARATOR "          "
/************************************************************
 ****               LOCUS name collisions                ****
 **** To note that an alternative LOCUS name is to be    ****
 **** used in a release, the following construction is to****
 **** appear in the ASN.1:                               ****
 ****   general {                                        ****
 ****     db "GenBank FlatFile LOCUS" ,                  ****
 ****     tag                                            ****
 ****       str "<NEW_NAME>  } } ,                       ****
 ****                                                    ****
 **** The general.db to be used is captured in the       ****
 **** following #define. This is used by both            ****
 **** idbatchload and asn2gnbk.                          ****
 ****                                                    ****
 ************************************************************/
#define LOCUS_COLLISION_DB_NAME "GenBank FlatFile LOCUS"


#define MAX_LOCUS_NAME_LEN 10
#define EMBL_MAX_LOCUS_NAME_LEN 9
#define NULL_SERIAL 9999
#define ASN2GNBK_PROTEIN 1
#define ASN2GNBK_GENE 2

#ifdef __cplusplus
}
#endif

