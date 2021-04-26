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

	$Log:	tognbk1.c,v $
 * Revision 2.68  94/02/24  16:14:03  sirotkin
 * tweak for NCBI GI formatting in /note
 * 
 * Revision 2.66  94/02/24  12:25:57  sirotkin
 * Used ostell lookup for codon from Tran-ext
 * 
 * Revision 2.65  94/02/16  17:28:04  sirotkin
 * >> Changed encoding of TRNA-ext.codon
 * >> and logic for misc_feature of xref
 * >> and added errPost in relase mode for CDS imp=-feats
 * 
 * Revision 2.64  94/02/09  14:25:54  sirotkin
 * no more double ;; before NCBI gi in CDS notes
 * 
 * Revision 2.63  94/02/08  13:32:20  sirotkin
 * undergoing qa for release, includes new date logic
 * NCBI gi added to CDS
 * 
 * Revision 2.62  94/01/25  11:45:52  sirotkin
 * now let's group Imp-feat.locs out
 * 
 * Revision 2.61  93/12/09  14:05:11  sirotkin
 * SeqPort changes including invalid residue, tRNA formating and date priority changing
 * 
 * Revision 2.60  93/12/03  16:28:05  sirotkin
 * Fuzz_found only on "<" or ">", noit the rest
 * 
 * Revision 2.59  93/11/18  13:18:53  sirotkin
 * tRNA spacing nit
 * 
 * Revision 2.58  93/11/17  15:49:41  sirotkin
 * Fixed   2954      case  SEQLOC_WHOLE: so in FlatLocNote context
 * acts reasonably with SeqId where bsp not in memory.
 * 
 * Revision 2.57  93/11/15  10:19:59  sirotkin
 * INVALID_RESIDUE and "GenBank staff at the National Library of Medicine" message
 * 
 * Revision 2.56  93/11/10  15:55:00  sirotkin
 * tRNA from ASN.1
 * 
 * Revision 2.55  93/10/19  11:05:24  sirotkin
 * GeneRef.desc that are in xref no longer generate misc_features
 * 
 * Revision 2.54  93/09/29  16:53:02  sirotkin
 * To expand avoidance of adding  /partial if fuzz found to also
 * mean suppression of /partial qualifiers if fuzz is found
 * 
 * Revision 2.53  93/09/27  14:01:42  sirotkin
 * expanding tognbk3.c and tweaks to patent -m 4 format
 * 
 * Revision 2.52  93/09/14  15:52:32  ostell
 * remove "For the protein sequence"
 * 
 * Revision 2.51  93/09/03  17:22:13  sirotkin
 * Fixed core dump when no portein annoation after some.
 * Lack of initialization.
 * 
 * Revision 2.50  93/09/01  16:51:51  sirotkin
 * has patent office format in ifdefs
 * 
 * Revision 2.49  93/07/30  14:06:22  sirotkin
 * fixed misc_feature removal so does not leave bbbone
 * 
 * Revision 2.48  93/07/12  10:47:55  sirotkin
 * No OrgRef forces /organism unknown
 * 
 * Revision 2.47  93/07/09  17:21:30  ostell
 * ,
 * 
 * Revision 2.46  93/07/09  12:15:28  sirotkin
 * checks GeneRefs against known GBQuals before misc_feature
 * 
 * Revision 2.45  93/07/02  13:17:54  sirotkin
 * attempt to fix Microsoft C compiler nit fusses)
 * 
 * Revision 2.44  93/07/01  15:00:07  sirotkin
 * FlatSourceFeatDoQual proto fix
 * 
 * Revision 2.43  93/07/01  14:57:08  sirotkin
 * split tognbk3 from tognbk1 for MacIntosh 32K limit
 * 
 * Revision 2.42  93/06/29  13:32:17  sirotkin
 * source feature now used both Seq_descr_mod and OrgRef.mod [tag value]
 * to generate qualifiers
 * 
 * Revision 2.41  93/06/25  13:16:10  sirotkin
 * source feature added, if none already
 * 
 * Revision 2.40  93/06/23  12:22:52  sirotkin
 * as per cavanaug, ErrPost on empty loc
 * 
 * Revision 2.39  93/06/23  11:54:07  sirotkin
 * failsafte in FlatAnnot so locs =="" makes feature skipped
 * 
 * Revision 2.38  93/06/08  08:25:32  kans
 * *** empty log message ***
 * 
 * Revision 2.37  93/06/04  12:28:51  sirotkin
 * FlatCommendPubDesc added as
 *           ^ (Oh well)
 * function to hanlde all comments from PUbdesc and called from
 * FlatCommentAnnot (not just FlatCommentDesc).
 * 
 * Revision 2.36  93/05/28  14:58:15  sirotkin
 * added to non-quote-quals
 * 
 * Revision 2.35  93/05/25  15:29:05  sirotkin
 * gene.maploc no longer redundant with /map="text"
 * 
 * Revision 2.34  93/05/10  16:12:15  sirotkin
 * added rsite and region as misc_features
 * 
 * Revision 2.33  93/04/28  11:37:57  sirotkin
 * tweaks to do_FlatCont which causes more heap to be
 * used, but makes it more robust
 * 
 * Revision 2.32  93/04/26  14:39:49  sirotkin
 * local ids in accession now handled properly.
 * 
 * Revision 2.31  93/04/19  11:38:50  sirotkin
 * embl right side numbers working
 * 
 * Revision 2.30  93/04/09  07:57:50  sirotkin
 * not used auto var nits
 * 
 * Revision 2.29  93/04/02  16:59:51  sirotkin
 * No more SeqIdFindBest in asn2gnbk, SeqIdPrint now handles local.str as accession
 * 
 * Revision 2.28  93/03/30  10:31:52  kans
 * kans - _except to excpt (was also Win32 keyword)
 * 
 * Revision 2.27  93/03/25  14:55:01  sirotkin
 * Flat_line_blank nits
 * 
 * Revision 2.26  93/03/24  19:52:17  kans
 * *** empty log message ***
 * 
 * Revision 2.25  93/03/24  14:25:49  kans
 * *** empty log message ***
 * 
 * Revision 2.24  93/03/24  09:09:32  sirotkin
 * Fixed size Preferred_Locus_order_seq in FlatMainAccession()
 * 
 * Revision 2.23  93/03/22  14:13:39  sirotkin
 * xref bug fixed FaltAnnot
 * 
 * Revision 2.22  93/03/11  17:19:22  kans
 * *** empty log message ***
 * 
 * Revision 2.21  93/03/11  16:45:10  ostell
 * changed except to _except
 * 
 * Revision 2.20  93/03/10  15:20:02  sirotkin
 * embl mode started
 * 
 * Revision 2.13  93/01/13  09:12:18  sirotkin
 * before starting on asn2{embl,flat}
 * 
 * Revision 2.12  93/01/05  15:35:14  sirotkin
 * teeak to Do_FlatCont()
 * 
 * Revision 2.11  92/12/28  11:38:25  sirotkin
 * intermeldiate, for safety
 * 
 * Revision 2.10  92/12/02  11:34:04  sirotkin
 * Lack of Main ACCESSION lead to "?00000", instead of fatal error.
 * 
 * Revision 2.9  92/12/02  09:32:24  sirotkin
 * attribution change
 * 
 * Revision 2.8  92/11/20  10:27:34  sirotkin
 * note_pro entrez testing version
 * 
 * Revision 2.6  92/11/17  16:09:41  sirotkin
 * Major changes to the way /note and /standard_name interact with
 * /product. etc.
 * 
 * Revision 2.5  92/11/06  15:47:03  sirotkin
 * removed some blank lines in comments, made robust against muid alone pubs.
 * 
 * Revision 2.4  92/10/16  09:30:54  sirotkin
 * Fixed ';' in Allelle in ComposeNote
 * 
 * Revision 2.3  92/10/09  15:53:47  sirotkin
 * ProtRef.desc now goes to /note, if only one field, no punc in /tnoe
 * 
 * Revision 2.2  92/10/08  15:15:35  sirotkin
 * EXPERIMENTAL VERSION, treats RNA featurs as if they were CDS for binding
 * 
 * Revision 2.1  92/10/08  08:37:46  sirotkin
 * fixes to formatting and dealing with initial null or
 * virtual sequences.  Format fixes to /note and deal with
 * puncuation.
 * 
 * Revision 2.0  92/10/06  17:37:28  ostell
 * force to 2.0
 * 
 * Revision 1.61  92/10/02  10:48:06  sirotkin
 * missing period and ' ; "' to '."' nit
 * 
 * Revision 1.60  92/10/02  09:39:58  sirotkin
 * fixed /note="" trouble two ways
 * 
 * Revision 1.59  92/10/01  21:57:53  ostell
 * minor int size fixes
 * 
 * Revision 1.58  92/10/01  14:07:53  sirotkin
 * Should be ready for release, has VirtLoc as NULL, no DNA3 FeatLoc as Annot, Check for EndPunc
 * 
 * Revision 1.56  92/09/25  12:10:50  ostell
 * added sequenceing method to note
 * 
 * Revision 1.55  92/09/22  14:44:05  sirotkin
 * Origin wrap, /note extra wrap, consistent use of LOCUS S\#
 * 
 * Revision 1.54  92/09/21  12:05:48  sirotkin
 * ')' now counted in FlatLoc
 * 
 * Revision 1.53  92/09/08  10:04:43  sirotkin
 * Comment in Block only on whole feature
 * 
 * Revision 1.52  92/08/28  15:31:22  ostell
 * *** empty log message ***
 * 
 * Revision 1.51  92/08/28  10:04:49  sirotkin
 * xref accessions for GenBank, EMBL, and DDBJ now as 2ndary on ACCESSION line
 * 
 * Revision 1.50  92/08/27  13:24:22  sirotkin
 * when both conflict and exception, adds \n
 * 
 * Revision 1.49  92/08/17  09:08:08  ostell
 * changed conflict and exception messages
 * 
 * Revision 1.48  92/07/13  11:44:29  sirotkin
 * a ',' now blocks use of gene desc properly
 * 
 * Revision 1.46  92/06/20  23:04:09  ostell
 * fix to genetic code
 * 
 * Revision 1.45  92/06/08  16:17:20  ostell
 * made ANSI functions
 * 
 * Revision 1.44  92/05/28  13:53:57  sirotkin
 * removed shortdir stuff
 * 
 * Revision 1.43  92/05/15  10:51:23  kans
 * *** empty log message ***
 * 
 * Revision 1.42  92/05/15  10:05:12  sirotkin
 * ready to install again
 * 
 * Revision 1.41  92/05/08  16:48:05  sirotkin
 * Julie changes all incorporated
 * 
 * Revision 1.40  92/05/07  13:53:31  sirotkin
 * Interval conversions reworked with full recursion
 * 
 * Revision 1.39  92/05/06  11:08:20  kans
 * *** empty log message ***
 * 
 * Revision 1.38  92/05/06  10:08:44  sirotkin
 * merging in Jonathan's changes
 * 
 * Revision 1.37  92/05/06  09:56:17  sirotkin
 * seems to work
 * 
 * Revision 1.33  92/04/16  10:19:01  sirotkin
 * works daily update output, has _class
 * 
 * Revision 1.32  92/04/09  13:56:25  sirotkin
 * added DateFromCreate() for  LOCUS date, cleaned up MolId for seg_set LOCUS name
 * 
 * Revision 1.31  92/04/06  14:54:46  sirotkin
 * moved /note stuff so CDS /note looks
 * better.
 * 
 * Revision 1.30  92/04/02  11:34:08  sirotkin
 * Added capibility to get Org info from Annot
 * 
 * Revision 1.29  92/03/30  11:47:26  sirotkin
 * Changed LO comment for Comment field for export to T10
 * 
 * Revision 1.28  92/03/20  12:23:17  kans
 * *** empty log message ***
 * 
 * Revision 1.27  92/03/06  14:28:52  sirotkin
 * fixed memory leak in FlatAllAccessions
 * 
 * Revision 1.26  92/03/06  09:34:39  ostell
 * AsnNode=ValNode,AsnValue=DataVal,AsnValueNode=AsnValxNode
 * 
 * Revision 1.25  92/03/02  09:04:13  sirotkin
 * new LOCUS line conventions, and minor fixes
 * 
 * Revision 1.24  92/02/21  08:49:48  sirotkin
 * ambig assignemnts fixed as per gish
 * 
 * Revision 1.23  92/02/20  17:32:56  sirotkin
 * fixed COMMENT formatting bug caused by multiple '\n's
 * 
 * Revision 1.22  92/02/19  09:39:39  sirotkin
 * at time rev 2.0 taken for gbparse/gbdiff
 * 
 * Revision 1.21  92/02/06  14:19:54  sirotkin
 * finxing map="<text>"
 * 
 * Revision 1.20  92/02/04  16:43:57  sirotkin
 * /codon_start moved
 * 
 * Revision 1.19  92/01/27  15:33:54  sirotkin
 * 3 tweaks for T-10
 * 
 * Revision 1.18  92/01/27  14:57:54  sirotkin
 * MEM macros out
 * 
 * Revision 1.17  92/01/27  14:20:13  sirotkin
 * column length tweaks
 * 
 * Revision 1.15  92/01/09  17:51:48  sirotkin
 * fixed longer lines if ends with comma right at end.  do_flatCOnt changed
 * 
 * Revision 1.13  92/01/07  18:11:00  sirotkin
 * incremental for Jim
 * 
 * Revision 1.11  92/01/02  21:13:09  sirotkin
 * many header bugs fixed
 * 
 * Revision 1.9  92/01/02  11:24:40  ostell
 * *** empty log message ***
 * 
 * Revision 1.8  91/12/31  16:55:24  sirotkin
 * fixed (I hope) LOCUS extra blank bug, no 2ndarf accession bug
 * and added DEBUG as a marcro to PC stack trouble debugging
 * 
 * Revision 1.7  91/12/26  17:08:04  sirotkin
 * bosc ready\?
 * 
 * Revision 1.6  91/12/24  11:52:31  sirotkin
 * some added no quote qualifiers
 * 
 * Revision 1.4  91/12/12  16:16:12  sirotkin
 * cdrom stuff
 * 
 * Revision 1.3  91/12/09  16:59:34  sirotkin
 * makes raw index files
 * 
 * Revision 1.2  91/12/05  15:19:25  sirotkin
 * intermediate with index files
 * 
 * Revision 1.1  91/12/03  16:53:58  sirotkin
 * Initial revision
 * 
 * Revision 1.29  91/12/02  17:00:03  sirotkin
 * fixed INterval problems with leagal and illegal null SeqLoc''s
 * in Mixes
 * 
 * Revision 1.28  91/11/22  17:05:46  sirotkin
 * many changes including Accession output in COMMENT or feature
 * for ftype 20 of backbone
 * 
 * Revision 1.27  91/11/18  14:15:27  kans
 * *** empty log message ***
 * 
 * Revision 1.26  91/11/18  13:39:57  ostell
 * prototypes fixed
 * 
 * Revision 1.25  91/11/18  11:12:29  sirotkin
 * Multiple changes put back in after backup lost
 * 
 * Revision 1.28  91/11/14  11:54:09  sirotkin
 * Includes looking for special Name1 (name2, name3) GenDescs
 * 
 * Revision 1.27  91/11/13  17:28:25  sirotkin
 * makight be incremental, might be good
 * trying for no redundancy and EMBL allele
 * 
 * Revision 1.26  91/11/08  17:33:00  sirotkin
 * definitions in segmented entries (and others) look up
 * for title if not found at current level.
 * 
 * another check for product in.
 * 
 * 
 * Revision 1.25  91/11/08  09:35:56  sirotkin
 * Fixed stuff so NOKEY segmented entreis do not
 * screw up count or appear in intervals.
 * 
 * ACCESSION back with -t option
 * 
 * Revision 1.24  91/11/01  16:08:27  sirotkin
 * all better now COMMENT blocks work even without -t
 * 
 * Revision 1.23  91/11/01  16:00:05  sirotkin
 * proper COMMENT blocks when comments from multiple contexts
 * 
 * Revision 1.22  91/11/01  15:53:14  sirotkin
 * T10 format done
 * 
 * Revision 1.21  91/10/31  16:45:42  kans
 * *** empty log message ***
 * 
 * Revision 1.20  91/10/31  11:16:13  sirotkin
 * Has Jim's fixed prototypes adn oen that was missing.
 * 
 * Revision 1.19  91/10/30  10:28:59  sirotkin
 * internal doc in FlatAnnot()
 * 
 * Revision 1.18  91/10/29  14:25:12  sirotkin
 * now dumps more completely from ASN.1 from the backbone (bb2asn output)
 * 
 * Revision 1.17  91/10/22  16:49:11  sirotkin
 * Now fixes pages ranges to be complete so 1217-42 becomes
 * 1217-1242
 * 
 * Revision 1.16  91/10/22  09:52:07  sirotkin
 * Changed prototypes to implement Options and
 * to add (bases 1 to <length>) on reference line
 * 
 * Revision 1.15  91/10/17  11:32:18  sirotkin
 * one blank less before division
 * 
 * Revision 1.14  91/10/02  16:13:51  sirotkin
 * shuold be ready for developers
 * 
 * Revision 1.12  91/10/02  14:24:45  sirotkin
 * Almost complete, checking in for safety.
 * new SeqLocs in last pointer for descr and annot still not
 * always right
 * 
 * Revision 1.11  91/09/25  17:20:26  sirotkin
 * fixed header
 * 
 * Revision 1.10  91/09/25  12:49:09  sirotkin
 * Name change, virtual fields ignored in seg-sets.
 * LOCUS line ok seg-sets from backbone
 * 
 * Revision 1.9  91/09/23  19:24:20  ostell
 * return to sync by kans
 * 
 * Revision 1.8  91/09/21  20:00:57  ostell
 * fixed integer type mismatches
 * 
 * Revision 1.4  91/09/16  17:15:33  sirotkin
 * Annotation from a set of nuc-prot bags within a pub set now
 * comes out right
 * 
 * Revision 1.3  91/09/16  14:16:58  sirotkin
 * This version works with MS_windows and
 * gets comments from seg-sets.
 * 
 * Revision 1.2  91/09/16  10:04:07  sirotkin
 * Works on Mac, needs tweaks for windows.  Quiet parameter
 * now has 1 bit interpretation 0x01 to quiet messages
 * 0x02 to quiet NCBI DETECTED ERRORS
 * 
*/
#include <math.h>
#include <togenbnk.h>


Uint1 Flat_Be_quiet;

Uint1 Locus_magic_cookie=0, Accession_magic_cookie=0;
#ifdef T10_and_CDROM
Uint1 Flat_T10, Flat_CdRom;
#endif

Uint1 Flat_do_Flat;
	FILE * Flat_Index_accession;
	FILE * Flat_Index_keyword;
	FILE * Flat_Index_author;
	FILE * Flat_Index_journal;
	FILE * Flat_Index_gene;
CharPtr Flat_last_div = NULL, Flat_last_locus = NULL, 
		Flat_last_accession = NULL;
Int4 Number_loci , Total_bases ;

Boolean Fuzz_found;

Uint1 Flat_format, Flat_nuc_prot_which;
int Flat_line_blank; /* 12 for GenBank, 5 for EMBL */
Uint1 Flat_release;

CharPtr ACC_BASE_LTR ;
CharPtr COM_BLANK_LTR;
CharPtr COM_BASE_LTR;
CharPtr DEF_BASE_LTR;
CharPtr Blank_BASE_LTR;
CharPtr Key_BASE_LTR;
CharPtr ORI_BASE_LTR;
CharPtr REF_BASE_LTR;
CharPtr REF_BASE_LTR_BP;
CharPtr AUTHOR_LTR;
CharPtr TITLE_LTR;
CharPtr JOURNAL_LTR;
CharPtr STANDARD_LTR;
CharPtr SOU_BASE_LTR;
CharPtr ORG_LTR;
CharPtr ORG_LTR_nel;
CharPtr ORG_LTR_tax;
CharPtr FEAT_CONT_BLANKS;

#define FLATLOC_CONTEXT_LOC 1
#define FLATLOC_CONTEXT_NOTE 2

static int FlatLoc_context=FLATLOC_CONTEXT_NOTE ;

/*------- FlatCheckMol () ---------*/

Boolean
FlatCheckMol (BioseqPtr bsp)
{
	Boolean retval = FALSE;

	if (ISA_na(bsp -> mol)) {
		if (Flat_nuc_prot_which == 1 || Flat_nuc_prot_which == 3){
			retval = TRUE;
		}
	}else if( bsp -> mol == 3){
		if ( Flat_nuc_prot_which == 2  || Flat_nuc_prot_which == 3){
			retval = TRUE;
		}
	}

	return retval;
}

/*---- non_quote_qual () --------*/
Boolean
non_quote_qual (CharPtr qual)
{
	Boolean retval = FALSE;
	static CharPtr non_quote_qual [] = {
	"anticodon",
	"citation",
	"codon",
	"codon_start",
	"cons_splice",
	"direction",
	"evidence",
	"frequency",
	"label",
	"mod_base",
	"number",
	"rpt_type",
	"rpt_unit",
	"transl_table",
	"transl_except",
	"used_in",
	"usedin"
	};
static int num_non_quote_qual = sizeof(non_quote_qual)
	/sizeof(non_quote_qual[0]);
	int i;

	for ( i = 0; i <  num_non_quote_qual; i ++ ){
		if ( StringCmp (non_quote_qual[i], qual) == 0){
			retval = TRUE;
			break;
		}
	}

	return retval;
}


/*----------- do_this_feature_now  ()------*/
int 
do_this_feature_now 
(SeqEntryPtr the_seq, SeqLocPtr location, AsnOptionPtr optionHead)
{
	int retval = FALSE;
	SeqIdPtr sid;
	SeqLocPtr last_loc, loc ;
	BioseqPtr bs;

	if (IS_minus_strand (location, optionHead)){
		sid = SeqFirstLocId(location, optionHead);
	}else{
		for (loc = SeqLocFindNext(location, NULL); loc != NULL; loc = SeqLocFindNext (location, loc)){
			last_loc = loc;
		}
		sid =  SeqFirstLocId(last_loc, optionHead);
	}
	if ( the_seq -> choice != 1){
		if ( !  (Flat_Be_quiet&1))
     ErrPost(CTX_NCBI2GB,CTX_2GB_NOT_SEQ,
			"FlatAnnot: Seq sent is not a Bioseq\n");
			goto BYEBYE;
	}
	bs = the_seq -> data.ptrvalue;
	if (SeqIdIn(sid, bs -> id)){
		retval = TRUE;
   }else if ( ! BioseqFind(sid)){
/*************************************
 **** the Bioseq which "should" get 
 **** this feature is not loaded *
**************************************/
      BioseqPtr loc_bsp;
         
      for (loc = SeqLocFindNext(location, NULL); loc != NULL; 
            loc = SeqLocFindNext (location, loc)){
         sid = SeqFirstLocId(loc, optionHead);
         loc_bsp= BioseqFind(sid);
         if (loc_bsp != NULL){
            if (bs == loc_bsp){
               retval = TRUE;
            }
            break;
         }
      }
   }

	BYEBYE:
	return retval;
}

/*----------- FlatBinSearch()---------*/

Int2 FlatBinSearch (Int4 count, CharPtr PNTR list, CharPtr query)
{
	Int2 l,r,k;
	int rt;

	l = 0;               /* binary search */
	r = (Int2)(count - 1);
	k = 0;
	while (l <= r)
	{
		k = (l + r)/ 2;
		rt = StringCmp(query,list[k]);
		if (rt == 0)
			return k;
		if (rt < 0)
			r = k - 1;
		else
			l = k + 1;
	}

	return -1;
}

/*----------- this_gene_is_bound  ()------*/

Boolean this_gene_is_bound 
(SeqFeatPtr gene_sfp, AsnOptionPtr optionHead)
{
	Boolean retval = FALSE;
	ExpressonsPtr expressons = NULL;
	AsnOptionPtr express_opt;
	ImplicitBindingsPtr bindings;
	BIND_FEAT_PTR features;
	int dex= -1;
	AsnOptionPtr gene_opt, map_opt;
	GeneRefPtr gp;
	Boolean gene_covered = FALSE;
	Boolean map_covered = FALSE;
	GeneQualKnownPtr genes, maps;

	express_opt = AsnOptionGet(optionHead, OP_TOGENBNK, 
		OP_TOGENBNK_BINDINGS , NULL);

	if (express_opt){
		expressons = (ExpressonsPtr) express_opt -> data.ptrvalue;
		bindings = expressons -> bindings;
		features = expressons -> features;
		if (bindings && features){
			for (dex=0; dex < features -> dex_genes; dex ++ ){
				if (gene_sfp == (features -> genes) [dex]){
					break;
				}
			}
			if ( dex < features -> dex_genes){
				if ( (bindings -> dex_genes) [dex] > -1)
					retval = TRUE;
				if ( (bindings -> gene_xreffed) [dex] )
					retval = TRUE;
			}else if (find_xref (gene_sfp, features ->cdregions, 
					features -> dex_cdregions, ASN2GNBK_GENE)) {
				retval = TRUE;
			}
		}
	}
	if ( ! retval){
/*-- if not bound by intervals, GBQual might already have */
			gp = (GeneRefPtr) gene_sfp -> data.value.ptrvalue;
			if (gp -> locus != NULL || gp -> maploc != NULL){
				if (gp -> locus){
					gene_opt = AsnOptionGet(optionHead, OP_TOGENBNK,
						OP_TOGENBNK_GENE_GBQUAL,NULL);
					if (gene_opt){
						genes = (GeneQualKnownPtr) gene_opt 
							-> data.ptrvalue;
						if ( FlatBinSearch (genes-> count, 
								genes -> array, gp -> locus) >= 0){
							gene_covered = TRUE;
						}
					}

				}else{
					gene_covered = TRUE;
				}
				if (gp -> maploc){
					map_opt = AsnOptionGet(optionHead, OP_TOGENBNK,
						OP_TOGENBNK_MAP_GBQUAL,NULL);
					if (map_opt){
						maps = (GeneQualKnownPtr) map_opt 
							-> data.ptrvalue;
						if ( FlatBinSearch (maps-> count, 
								maps -> array, gp -> maploc) >= 0){
							map_covered = TRUE;
						}
					}
				}else{
					map_covered = TRUE;
				}
				if (gene_covered && map_covered)
					retval = TRUE;
			}
	}
	
	return retval;
}
	
/*----------- this_prot_is_bound  ()------*/
Boolean this_prot_is_bound 
(SeqFeatPtr prot_sfp, AsnOptionPtr optionHead)
{
	Boolean retval = FALSE;
	ExpressonsPtr expressons = NULL;
	AsnOptionPtr express_opt;
	ImplicitBindingsPtr bindings;
	BIND_FEAT_PTR features;
	int dex= -1;

	express_opt = AsnOptionGet(optionHead, OP_TOGENBNK, 
		OP_TOGENBNK_BINDINGS , NULL);

	if (express_opt){
		expressons = (ExpressonsPtr) express_opt -> data.ptrvalue;
		bindings = expressons -> bindings;
		features = expressons -> features;
		if (bindings && features){
			for (dex=0; dex < features -> dex_prots; dex ++ ){
				if (prot_sfp == (features -> prots) [dex]){
					break;
				}
			}
			if ( dex < features -> dex_prots){
				if ( (bindings -> dex_prots) [dex] > -1)
					retval = TRUE;
				if ( (bindings -> prot_xreffed) [dex] )
					retval = TRUE;
			}else if (find_xref (prot_sfp, features ->cdregions, 
					features -> dex_cdregions, ASN2GNBK_PROTEIN)) {
				retval = TRUE;
			}
		}
	}
	
	return retval;
}

/*----------- bound_gene_next ()------*/
FeatureStatePtr bound_gene_next 
(GeneRefPtr PNTR bound_genePtr, FeatureStatePtr state)
{
	FeatureStatePtr retval = state;
	SeqFeatPtr gene_sfp;

	* bound_genePtr = NULL;

	if (state){
		if ( state -> type == 1 ||  state -> type == 3
					|| state -> type == 5){
			if (state -> head_xref){
				* bound_genePtr = NULL;
				if (state -> this_xref){
					do {
						if ( state -> this_xref -> data.choice ==1){
							* bound_genePtr = (GeneRefPtr) 
								(state -> this_xref -> data.value.ptrvalue);
						}
						state -> this_xref = state -> this_xref -> next;
						if (  * bound_genePtr){
							break;
						}
					}while( state -> this_xref);
				}
			}else{
				for (state -> last_dex ++; 
						state -> last_dex < state -> features -> dex_genes;
						state -> last_dex ++){
					if ( (state -> bindings -> dex_genes) 
							[ state -> last_dex] == 
							state -> target_cds_dex){
						gene_sfp = (state -> features -> genes) 
							[state -> last_dex];
						* bound_genePtr = (GeneRefPtr) gene_sfp -> 
							data.value.ptrvalue;
						break;
					}
				}
			}
		}
	}

	return retval;
}

/*----------- find_bound_genes_init ()------*/

FeatureStatePtr find_bound_genes_init 
(SeqFeatPtr sfp, Boolean did_qual_gene, GeneRefPtr PNTR bound_genePtr, AsnOptionPtr optionHead)
{
	FeatureStatePtr retval = NULL;
	AsnOptionPtr bindings_option;
	int dex;
	ExpressonsPtr expressons;

	* bound_genePtr = NULL;

	if ( sfp -> data.choice == 1 ){
		retval = MemNew (sizeof(FeatureState));
		* bound_genePtr = (GeneRefPtr) sfp -> data.value.ptrvalue;
	}else if (sfp -> data.choice ==  3
			|| sfp -> data.choice ==  5) {
		if ( !did_qual_gene ||  sfp -> xref)	{
			bindings_option = AsnOptionGet(optionHead, OP_TOGENBNK, 
				OP_TOGENBNK_BINDINGS , NULL);

			if (bindings_option){
				retval = MemNew (sizeof(FeatureState));
				retval -> target_cds_dex = -1;
				expressons = (ExpressonsPtr) bindings_option -> data.ptrvalue;
				retval -> bindings = expressons -> bindings;
				retval -> features = expressons -> features;
				retval -> head_xref = retval -> this_xref = sfp -> xref;
				if (retval -> bindings && retval -> features){
					retval->type = sfp ->data.choice;/* marks as good state */
					retval -> last_dex = -1;
					for ( dex=0; dex < retval -> features -> dex_cdregions;
							 dex ++ ) {
						if ( (retval -> features -> cdregions) [dex] == sfp){
							retval -> target_cds_dex = dex;
							break;
						}
					}
					if ( retval -> target_cds_dex >= 0)
						if (retval -> head_xref){
							retval -> this_xref = retval -> head_xref;
						}
						retval = bound_gene_next(bound_genePtr, retval);
				}
			}
		}
	}

	return retval;
}

	
/*----------- bound_prot_next ()------*/
FeatureStatePtr bound_prot_next 
(ProtRefPtr PNTR bound_protPtr, FeatureStatePtr state)
{
	FeatureStatePtr retval = state;
	SeqFeatPtr prot_sfp;

	* bound_protPtr = NULL;

	if (state){
		if ( state -> type == 4 ||  state -> type == 3
			|| state -> type ==  5){
			if (state -> head_xref){
				* bound_protPtr = NULL;
				if (state -> this_xref){
					do {
						if ( state -> this_xref -> data.choice == 4){
							* bound_protPtr = (ProtRefPtr) 
								(state -> this_xref -> data.value.ptrvalue);
						}
						state -> this_xref = state -> this_xref -> next;
						if (  * bound_protPtr){
							break;
						}
					}while (state -> this_xref);
				}
			}else{
				for (state -> last_dex ++; 
						state -> last_dex < state -> features -> dex_prots;
						state -> last_dex ++){
					if ( (state -> bindings -> dex_prots) [ state -> last_dex]
							== state -> target_cds_dex){
						prot_sfp = (state -> features -> prots) 
							[state -> last_dex];
						* bound_protPtr = (ProtRefPtr) prot_sfp -> 
							data.value.ptrvalue;
						break;
					}
				}
			}
		}
	}

	return retval;
}

/*----------- find_bound_prots_init ()------*/

FeatureStatePtr find_bound_prots_init 
(SeqFeatPtr sfp, Boolean did_qual_product, ProtRefPtr PNTR bound_protPtr, AsnOptionPtr optionHead)
{
	FeatureStatePtr retval = NULL;
	AsnOptionPtr bindings_option;
	int dex;
	ExpressonsPtr expressons;

	* bound_protPtr = NULL;

	if ( sfp -> data.choice == 4 ){
		retval = MemNew (sizeof(FeatureState));
		* bound_protPtr = (ProtRefPtr) sfp -> data.value.ptrvalue;
	}else if (sfp -> data.choice ==  3
			|| sfp -> data.choice ==  5) {
		if ( !did_qual_product ||  sfp -> xref)	{
			bindings_option = AsnOptionGet(optionHead, OP_TOGENBNK, 
				OP_TOGENBNK_BINDINGS , NULL);

			if (bindings_option){
				retval = MemNew (sizeof(FeatureState));
				expressons = (ExpressonsPtr) bindings_option -> data.ptrvalue;
				retval -> type = sfp -> data.choice;
				retval -> head_xref = retval -> this_xref = sfp -> xref;
				retval -> bindings = expressons -> bindings;
				retval -> features = expressons -> features;
				if (retval -> bindings && retval -> features){
					retval->type = sfp ->data.choice;/* marks as good state */
					retval -> last_dex = -1;
					for ( dex=0; dex < retval -> features -> dex_cdregions; 
							dex ++ ) {
						if ( (retval -> features -> cdregions) [dex] == sfp){
							retval -> target_cds_dex = dex;
							break;
						}
					}
					if ( retval -> target_cds_dex >= 0)
						if (retval -> head_xref){
							retval -> this_xref = retval -> head_xref;
						}
						retval = bound_prot_next(bound_protPtr, retval);
				}
			}
		}
	}

	return retval;
}

/*-------FlatSafeThere()-----*/

Boolean
FlatSafeThere (CharPtr name)
{
	Boolean retval = FALSE;

	if (name) 
		if (*name)
			retval = TRUE;

return retval;
}

/*-------FlatSafeAsnThere()-----*/

Boolean
FlatSafeAsnThere (ValNodePtr name)
{
	Boolean retval = FALSE;

	if (name) 
		if (name -> data.ptrvalue)
			if ( * ((CharPtr) (name -> data.ptrvalue)) )
				retval = TRUE;

return retval;
}

/*------------FlatSeqFeatHasXref()------*/

Boolean 
FlatSeqFeatHasXref ( SeqFeatPtr sfp, Uint1 choice)
{
	Boolean retval = FALSE;
	SeqFeatXrefPtr xrp ;

	for (xrp = sfp -> xref; xrp; xrp = xrp -> next){
		if ( xrp ->data.choice == choice){
			retval = TRUE;
			break;
		}
	}
	return retval;
}

/*-----------------FlatSeqFeatHasQual()---------------*/

Boolean 
FlatSeqFeatHasQual ( SeqFeatPtr sfp, CharPtr qual)
{
	Boolean retval = FALSE;
	GBQualPtr q;

	if (qual)
	for (q = sfp -> qual; q; q = q -> next){
		if (StringCmp(q -> qual, qual) == 0){
			retval = TRUE;
			break;
		}
	}

	return retval;
}

/*-------------FlatAnnotGBQual()----------------*/

void
FlatAnnotGBQual (FILE *fp, SeqFeatPtr  sfp, 
	Boolean PNTR did_partial, CharPtr PNTR note_qual_text,
	CharPtr PNTR standard_name_qual_text, CharPtr PNTR gene_qual_text,
	CharPtr PNTR map_qual_text,
	Boolean PNTR did_gbqual_gene, Boolean PNTR did_gbqual_product,
	Boolean doing_CDS_from_product ,
	AsnOptionPtr optionHead)
{
	CharPtr temp, line, kludge;
	CharPtr qual_start = NULL;
	GBQualPtr qual;

		for (qual = sfp -> qual; qual; qual = qual -> next){
			if ( FlatSeqFeatHasXref(sfp,SeqFeatGeneChoice) &&
				 ! StringCmp(qual -> qual, "gene" )){
				continue;
			}
			if ( Fuzz_found  && ! StringCmp(qual -> qual, "partial" )){
				continue;
			}
			if ( FlatSeqFeatHasXref(sfp,SeqFeatProtChoice) &&
				 ! StringCmp(qual -> qual, "product" )){
				continue;
			}
			if ( ! StringCmp(qual -> qual, "note" )){
				if ( * note_qual_text == NULL){
				 * note_qual_text = StringSave(qual -> val);
				}else{
					CharPtr hold_note = * note_qual_text, temp;

					* note_qual_text = temp = MemNew (StringLen(hold_note) +
							StringLen(qual -> val) + 5);
					temp = StringMove(temp,hold_note);
					temp = StringMove(temp,"; ");
					temp = StringMove(temp,qual -> val);
					MemFree(hold_note);
				}
				continue;
			}
			if (doing_CDS_from_product 
					&& ! StringCmp(qual -> qual, "product" )){
				 * did_gbqual_product = TRUE;
				continue;
			}
			if (qual_start)
				MemFree(qual_start);
			if ( qual -> val){
				if ( * (qual -> val)){
					temp= qual_start = MemNew ( (size_t) (
						StringLen (qual -> qual) + 5+
						StringLen(qual-> val) )  );
					
					temp = StringMove (temp, "/");
					temp = StringMove(temp, qual -> qual);
					if ( non_quote_qual(qual -> qual) ) {
						temp = StringMove (temp, "=");
						temp = StringMove(temp, qual -> val);
					}else{
						temp = StringMove (temp, "=\"");
						temp = StringMove(temp, qual -> val);
						temp = StringMove(temp, "\"");
					}
					if ( ! StringCmp(qual -> qual, "product" )){
						 * did_gbqual_product = TRUE;
					}
					if ( ! StringCmp(qual -> qual, "map" )){
						 * map_qual_text = qual -> val;
					}
					if ( ! StringCmp(qual -> qual, "standard_name" )){
						 * standard_name_qual_text = qual -> val;
					}
						
					if ( ! StringCmp(qual -> qual, "gene" )){
						 * did_gbqual_gene = TRUE;

/*  this only takes last one, which interacts favorably with
    what gbparse does with duplicate genes */
						* gene_qual_text = qual -> val;

						Flat_out_index_line ( Flat_Index_gene, 
							qual -> val);
					}
				}else{
					temp= qual_start = MemNew ( (size_t) (
						StringLen (qual -> qual) +3));
					temp = StringMove (temp, "/");
					temp = StringMove(temp, qual -> qual);
					if ( StringCmp (qual -> qual, "partial") == 0)
						* did_partial = TRUE;
				}
			}else{
					temp= qual_start = MemNew ( (size_t) (
						StringLen (qual -> qual) +2));
					temp = StringMove (temp, "/");
					temp = StringMove(temp, qual -> qual);
			}
			line = do_FlatCont(58,21,qual_start, FALSE, optionHead);
			MACRO_FLAT_LINE_OUT(temp,FEAT_CONT_BLANKS, line,kludge)
			fflush(fp);
			MemFree(qual_start);
			qual_start = NULL;
			MemFree(line);
		}
}


/*-------------FlatEndsPunc()---------------*/

Boolean
FlatEndsPunc (CharPtr string)
{
	Boolean retval = FALSE;
	static char * punc =".;,:";
	char ch;
	CharPtr p ;

	if (string){
		ch = * (string + StringLen(string) -1);
		for (p = punc; *p; p ++){
			if ( ch == *p){
				retval = TRUE;
				break;
			}
		}
	}

	return retval;
}

/*------------ SeqCodeNameGet () ---------------*/

/* ---- Jim, don't these need some sanity range chaecks???*/

CharPtr
SeqCodeNameGet (SeqCodeTablePtr table, Uint1 residue)
{
	int index=residue - table -> start_at;
	static CharPtr oops = "?";

	if (index >= 0 && index < (int) table -> num){
		return (table -> names) [index];
	}else{
		ErrPost(CTX_NCBI2GB, CTX_2GB_BAD_RESIDUE,
		"asn2gnbk: %c(%d) > max in SeqCode table=%d",
		(char) residue, (int) residue, (int) table -> num);
		return oops;
	}
}

/*------------ SeqCodeSymbolGet () ---------------*/

CharPtr
SeqCodeSymbolGet (SeqCodeTablePtr table, Uint1 residue)
{
	int index=residue - table -> start_at;
	static CharPtr oops = "?";

	if (index >= 0 && index < (int) table -> num){
		return (table -> symbols) [index];
	}else{
		ErrPost(CTX_NCBI2GB, CTX_2GB_BAD_RESIDUE,
		"asn2gnbk: %c(%d) > max in SeqCode table=%d",
		(char) residue, (int) residue, (int) table -> num);
		return oops;
	}

}

/*-------------- FlatComposetRNANote() -----------------*/

CharPtr
FlatComposetRNANote( tRNAPtr trna, CharPtr temp, CharPtr total_buf, Int4Ptr lenPt, int PNTR need_spPt)
{
	Uint1 SeqCodeTypeChoice=0;
	SeqCodeTablePtr table;
	int dex;
	Uint1 codon [4];
	
	if ( (trna -> codon)[0] != 255){
		codon[3] = '\0';
		if ((*need_spPt))
			temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
		temp = FlatSmartStringMove(total_buf,lenPt,temp,"codon");
		(*need_spPt) ++;
		if ( (trna -> codon)[1] != 255){
			temp = FlatSmartStringMove(total_buf,lenPt,temp,"s");
		}
		temp = FlatSmartStringMove(total_buf,lenPt,temp," recognized: ");
		for (dex=0; dex < 6; dex ++){
			if ((trna -> codon)[dex] == 255)
				break;
			if (CodonForIndex((trna -> codon)[dex],Seq_code_iupacna,codon)){
				temp = FlatSmartStringMove(total_buf,lenPt,temp, (CharPtr) codon);
			}else{
				temp = FlatSmartStringMove(total_buf,lenPt,temp, "?");
				ErrPost(CTX_NCBI2GB, CTX_2GB_BAD_RESIDUE,
					"Unable to convert index to codon in Trna-ext, index=%d",
					(trna -> codon)[dex]);
			}
			if (dex <5) 
				if ((trna -> codon)[dex+1] != 255){
					temp = FlatSmartStringMove(total_buf,lenPt,temp,", ");
				}
		}
	}
/*
  The choice values used in the tRNA structure do NOT corresond to
  the choice(==ENUMs) of Seq-code_type, and the latter are used
  by all the utility functions, so we map them...
*/
	if (trna != NULL)
	switch (trna -> aatype){
		case 1:
			SeqCodeTypeChoice = 2;
			break;
		case 2:
			SeqCodeTypeChoice = 8;
			break;
		case 3:
			SeqCodeTypeChoice = 7;
			break;
		case 4:
			SeqCodeTypeChoice = 11;
			break;
	}

/*  if not set or something we are not ready for, ignore this one */
	if (SeqCodeTypeChoice != 0){
		table = SeqCodeTableFind (SeqCodeTypeChoice);
		if (table != NULL){
			if ( (trna -> codon)[0] != 255){
				temp = FlatSmartStringMove(total_buf,lenPt,temp,"; ");
			}else if (*need_spPt){
				temp = FlatSmartStringMove(total_buf,lenPt,temp," ");
			}
			(*need_spPt) ++;
			temp = FlatSmartStringMove(total_buf,lenPt,temp,"aa: ");
			temp = FlatSmartStringMove(total_buf,lenPt,temp,
				SeqCodeNameGet(table, trna -> aa));
			temp = FlatSmartStringMove(total_buf,lenPt,temp,";");
		}
	}
	
	return temp;
}

/*----------FlatAnnotComposeNote()---------------------*/

/***************************************************
 ****                                           ****
 **** Currently, the parameters:                ****
 ****   did_gbqual_product and did_gbqual_gene  ****
 **** are being ignored, as these quals should  ****
 **** not affect what is output for /note.      ****
 ****                                           ****
 ***************************************************/
void
FlatAnnotComposeNote(SeqEntryPtr the_seq, SeqFeatPtr sfp, BioseqPtr protein, 
		CdRegionPtr cds, CharPtr note_text, CharPtr standard_name_text,
		CharPtr gene_text,
		Boolean did_gbqual_product, Boolean did_gbqual_gene,
		CharPtr feat_type, CharPtr total_buf, 
		Int4Ptr lenPt, CharPtr temp, FLAT_PUB_STORE_PTR refhead, 
		Boolean doing_CDS_from_product ,
		AsnOptionPtr optionHead)
{
	CharPtr pro_comment= NULL, p;
	CharPtr qual_start = NULL;
	int need_spaces=0;
	FeatureStatePtr prot_state, gene_state;
	GeneRefPtr bound_gene;
	ProtRefPtr bound_prot;
	Boolean found;
	Int4 gi=0;
	Int2 sgml_allele_len ;
	size_t len_prot;
	SeqAnnotPtr prot_annot;
	SeqLocPtr xref;
	ValNodePtr prot_name;
	ValNodePtr rsite;
	SeqFeatPtr prot_sfp;
	SeqIdPtr id;
	char pro_num[12];
	ValNodePtr descr;
	CharPtr conflict_msg_no_protein="Author-provided coding region translates with internal stops.";
	CharPtr except_msg_no_protein="Author-provided coding region translates with internal stops for reasons explained in citation.";
	CharPtr conflict_msg="Author-given protein sequence is in conflict with the conceptual translation.";
	CharPtr except_msg="Author-given protein sequence differs from the conceptual translation for reasons explained in citation.";
	CharPtr scratch;
	char * preface = "Cross-reference: ";
	ObjectIdPtr oid;
	DbtagPtr the_dbtag;
	RnaRefPtr rna;
	tRNAPtr trna;
 SeqPortPtr spp;
 Uint1 residue;
 Int2 i, invalid_cnt, first_bad;

	temp = FlatSmartStringMove(total_buf,lenPt,temp,"/note=\"");
	if ( sfp -> data.choice == 1){ /* GENE feature */
		temp = FlatSmartStringMove(total_buf,lenPt,
			temp, GENE_NOTE_QUAL_PREFIX);
	}
	if (sfp -> comment && !
		 (sfp -> data.choice ==13 || sfp -> data.choice ==9) ){
		scratch = FlatCleanEquals(sfp -> comment);
		temp = FlatSmartStringMove(total_buf,lenPt,temp,scratch);
		if ( ! FlatEndsPunc (scratch))
			temp = FlatSmartStringMove(total_buf,lenPt,temp,".");
		need_spaces ++ ;
	}
	for ( gene_state = find_bound_genes_init (sfp, FALSE,
				 & bound_gene,  optionHead);bound_gene;
			gene_state = bound_gene_next(& bound_gene, gene_state) ){
		if ( bound_gene -> desc){
/*********************************************
 ****                                     ****
 **** If there is a gene desc, must check ****
 **** for redundancy with protein info    ****
 **** before adding it.                   ****
 ****                                     ****
 **** Only with ProtRef.name, as          ****
 **** ProtRef.desc no longer comes out    ****
 **** with /product                       ****
 ****                                     ****
 *********************************************/
			found = FALSE;
			if (gene_text){
				if (StringCmp(gene_text, bound_gene -> desc) == 0){
					found = TRUE;
				}
			}
			if ( ! found){
				for ( prot_state = find_bound_prots_init(sfp,FALSE, 
							& bound_prot,  optionHead); bound_prot;
							prot_state = bound_prot_next(& bound_prot, 
							prot_state) ){
					if ( bound_prot -> name){
						if ( ! StringCmp(bound_gene -> desc, 
								bound_prot -> name -> data.ptrvalue) ){
							found = TRUE;
							break;
						}else {
							len_prot = StringLen(bound_prot -> 
										name -> data.ptrvalue);
							if (!StringNCmp(bound_gene -> desc,
									bound_prot -> name -> data.ptrvalue,
									len_prot)){
	/*------
	* if the genedesc was generated as is done in bb2asn for
	* bound genes from a list of ProtNames
	* using Name1 (name2, name3) do not use gene desc
	*--------*/
								found = TRUE;
								for (p = bound_gene -> desc + len_prot;
										*p; p ++){
									if ( *p == ' ') 
										continue;
									if (*p == ',')
										break;
									if (*p == '(')
										break;
									found = FALSE;
									break;
								}
								if ( found)
									break;
							}
						}
					}
				}
				MemFree(prot_state);
			}
			if ( ! found){
				if (need_spaces)
					temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
				temp = FlatSmartStringMove(total_buf,lenPt,temp,"Description: ");
				temp = FlatSmartStringMove(total_buf,lenPt,temp, bound_gene -> desc);
				if ( ! FlatEndsPunc(bound_gene -> desc))
					temp = FlatSmartStringMove(total_buf,lenPt,temp,";");
				need_spaces ++ ;
			}
		}
		if (bound_gene -> allele){
				if (need_spaces)
					temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
			temp = FlatSmartStringMove(total_buf,lenPt,temp,"Allele: ");
			sgml_allele_len = Sgml2AsciiLen(bound_gene -> allele);
			if (total_buf){
				temp=Sgml2Ascii(bound_gene -> allele, 
					temp, sgml_allele_len+1);
			}
			temp = FlatSmartStringMove(total_buf,lenPt,temp,";");
			if (lenPt){
				(*lenPt) += sgml_allele_len;
			}
			need_spaces ++ ;
		}
	}
	MemFree(gene_state);
	for ( prot_state = find_bound_prots_init(sfp,FALSE, & bound_prot,  
				optionHead);bound_prot;
			prot_state = bound_prot_next(& bound_prot, prot_state) ){
		if ( bound_prot -> name ){
			for (prot_name = bound_prot -> name -> next ; /* first 
								 one not in note */
					prot_name; prot_name = prot_name -> next){
/*********************************************
 ****                                     ****
 **** If there are >1 ProtName, must check****
 **** for redundancy with gene info       ****
 **** before adding them.                 ****
 ****                                     ****
 *********************************************/
				found = FALSE;
				for ( gene_state = find_bound_genes_init (sfp, FALSE, 
						& bound_gene,  optionHead);bound_gene;
						gene_state = bound_gene_next(& bound_gene, 
						gene_state) ){
					if ( bound_gene -> desc){
						if ( ! StringCmp(bound_gene -> desc, 
								prot_name -> data.ptrvalue) ){
							found = TRUE;
							break;
						}
					}
				}
				MemFree(gene_state);
				if ( ! found) {
					if (need_spaces)
						temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
					temp = FlatSmartStringMove(total_buf,lenPt,temp, prot_name -> 
						data.ptrvalue);
					temp = FlatSmartStringMove(total_buf,lenPt,temp,";");
					need_spaces ++ ;
				}
			}
		}
		if ( bound_prot -> desc){
/*********************************************
 ****                                     ****
 **** If there is a Prot desc, must check ****
 **** for redundancy with gene info       ****
 **** before adding it.                   ****
 ****                                     ****
 *********************************************/
			found = FALSE;
			if (gene_text)
				if (StringCmp(gene_text, bound_prot -> desc) == 0){
					found = TRUE;
				}
			if (!found)
			if (standard_name_text)
				if (StringCmp(standard_name_text, bound_prot -> desc) == 0){
					found = TRUE;
				}
			if (!found){
				for ( gene_state = find_bound_genes_init (sfp, FALSE, 
						& bound_gene,  optionHead);bound_gene;
						gene_state = bound_gene_next(& bound_gene, 
						gene_state) ){
					if ( bound_gene -> desc){
						if ( ! StringCmp(bound_gene -> desc, 
								bound_prot -> desc) ){
							found = TRUE;
							break;
						}
					}
					if ( bound_gene -> locus){
						if ( ! StringCmp(bound_gene -> locus, 
								bound_prot -> desc) ){
							found = TRUE;
							break;
						}
					}
				}
				MemFree(gene_state);
			}
			if ( ! found){
				if (need_spaces)
					temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
				temp = FlatSmartStringMove(total_buf,lenPt,temp, 
					bound_prot -> desc);
				if ( ! FlatEndsPunc ( bound_prot -> desc))
					temp = FlatSmartStringMove(total_buf,lenPt,temp,";");
				need_spaces ++;
			}
		}
	}
	MemFree(prot_state);
	if (protein && ! doing_CDS_from_product ){
		prot_annot = protein -> annot;
		if (prot_annot){
			for ( prot_sfp = (SeqFeatPtr) prot_annot -> data;
					 prot_sfp; prot_sfp = prot_sfp -> next){
				if ( prot_sfp -> data.choice == 10){
				if (need_spaces)
					temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
					temp = FlatSmartStringMove(total_buf,lenPt,temp,prot_sfp -> comment);
					if ( ! FlatEndsPunc (prot_sfp -> comment))
						temp = FlatSmartStringMove(total_buf,lenPt,temp,"." );
					need_spaces ++ ;
				}
			}
		}
		if (protein){
			descr = protein -> descr;
			if ( descr ){
					pro_comment = FlatCommentDescr(descr, refhead, 
						optionHead);
			}
		}
		if (protein){
			for (gi= 0, id = protein -> id; id; id = id -> next){
				if ( id -> choice == SEQID_GI){
					gi = id -> data.intvalue;
             break;
				}
			}
			
			if (pro_comment){
				if (need_spaces)
					temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
				temp = FlatSmartStringMove(total_buf,lenPt,temp, pro_comment);
				need_spaces =0;
			}
			MemFree(pro_comment);
			pro_comment=NULL;

		}
	}
	if (sfp -> data.choice !=  8 &&  ! StringCmp("CDS", feat_type)){
		if ( sfp ->excpt){
			if (need_spaces)
				temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
			temp = FlatSmartStringMove(total_buf,lenPt,temp,protein?except_msg:except_msg_no_protein);
			need_spaces ++;
		}
		if (sfp ->excpt && cds -> conflict && protein){
			temp = FlatSmartStringMove(total_buf,lenPt,temp,"\n");
			need_spaces ++ ;
		}else{
			if ( cds -> conflict && (protein || ! sfp ->excpt) ){
				if (need_spaces)
					temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
			}
		}
		if ( cds -> conflict && (protein || ! sfp ->excpt) ){
			temp = FlatSmartStringMove(total_buf,lenPt,temp,
				protein?conflict_msg:conflict_msg_no_protein);
			need_spaces ++;
		}
/*************
 *
 * Checking CDS /translation Bioseq for invalid residues
 *
 ****************/
		if (protein != NULL){
		 spp = SeqPortNew(protein, 0, -1, 0, Seq_code_iupacaa);
		 i = invalid_cnt = 0;
		 SeqPortSeek(spp, 0, SEEK_SET);
		 while ((residue = SeqPortGetResidue(spp)) != SEQPORT_EOF) {
			if (residue == SEQPORT_EOS || residue == SEQPORT_VIRT)
				break;
			i ++;
	/* need to check for INVALID_RESIDUE */
			  if (residue == INVALID_RESIDUE){
					if (invalid_cnt == 0){
						first_bad = i;
					}
				 invalid_cnt ++;
			  }
		 }
			SeqPortFree(spp);
	/************
	 * 
	 * at least one bad residue, add to /note and ErrPost about it
	 *
	 ***************/
			if (invalid_cnt){
				CharPtr bad_buf = MemNew(51);
				CharPtr bad_buf_to = MemNew(51);

				sprintf(bad_buf,"%d invalid residue%s in /translation;",
					(int) invalid_cnt, invalid_cnt > 1?"s":"");
				if (need_spaces)
						temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
				temp = FlatSmartStringMove(total_buf,lenPt,temp,bad_buf);
				need_spaces ++;
				sprintf(bad_buf_to,"first invalid residue changed to \'X\' at %d",
					(int) first_bad);
		
				if (total_buf)
				ErrPost(CTX_NCBI2GB,CTX_2GB_BAD_RESIDUE,
					"asn2gnbk: %s  %s.",bad_buf, bad_buf_to);
				MemFree (bad_buf);
				MemFree (bad_buf_to);

			}
		}
	}else if (sfp -> data.choice == 5){
		 /* RNA types */
			rna = (RnaRefPtr) (sfp -> data.value.ptrvalue);
			if ( rna -> type == 3){ 
				if ( rna -> ext.choice == 2){
					trna = rna -> ext.value.ptrvalue;
					temp = FlatComposetRNANote(trna,temp,total_buf,
						lenPt, &need_spaces);
				}else if ( rna -> ext.choice == 1){
					if (need_spaces)
						temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
					temp = FlatSmartStringMove(total_buf,lenPt,temp,
							(CharPtr)rna -> ext.value.ptrvalue);
					temp = FlatSmartStringMove(total_buf,lenPt,temp,";");
					need_spaces ++ ;
				}
			}
	}else if (sfp -> data.choice ==9){ /* region */
		if (need_spaces)
			temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
		need_spaces = 0;
		temp = FlatSmartStringMove(total_buf,lenPt,temp,
			(CharPtr) sfp -> data.value.ptrvalue);
		temp = FlatSmartStringMove(total_buf,lenPt,temp," region;");
		need_spaces ++ ;
		if (need_spaces)
			temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
		scratch = FlatCleanEquals(sfp -> comment);
		temp = FlatSmartStringMove(total_buf,lenPt,temp,scratch);
		if ( ! FlatEndsPunc (scratch))
			temp = FlatSmartStringMove(total_buf,lenPt,temp,".");
	}else if (sfp -> data.choice ==13){ /* rsite */
		rsite = (ValNodePtr) sfp -> data.value.ptrvalue;
		if (need_spaces)
			temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
		need_spaces = 0;
		if (rsite -> choice == 1){
			temp = FlatSmartStringMove(total_buf,lenPt,temp,
				(CharPtr) rsite -> data.ptrvalue);
			temp = FlatSmartStringMove(total_buf,lenPt,temp," site;");
			need_spaces ++ ;
		}else{
				the_dbtag = (DbtagPtr) rsite -> data.ptrvalue;
				if (the_dbtag -> db){
					temp = FlatSmartStringMove(total_buf,lenPt,temp,
						the_dbtag -> db);
					temp = FlatSmartStringMove(total_buf,lenPt,temp,":");
				}
				oid = the_dbtag -> tag;
				if (oid -> id != 0){
					sprintf(pro_num,"%d",oid -> id);
					temp = FlatSmartStringMove(total_buf,lenPt,temp,pro_num);
					temp = FlatSmartStringMove(total_buf,lenPt,temp,";");
				}
				if (oid -> str){
					temp = FlatSmartStringMove(total_buf,lenPt,temp, oid -> str);
					temp = FlatSmartStringMove(total_buf,lenPt,temp,";");
				}
			need_spaces ++ ;
		}
		if (need_spaces)
			temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
		scratch = FlatCleanEquals(sfp -> comment);
		temp = FlatSmartStringMove(total_buf,lenPt,temp,scratch);
		if ( ! FlatEndsPunc (scratch))
			temp = FlatSmartStringMove(total_buf,lenPt,temp,".");
	}else if (sfp -> data.choice ==7){
		xref = (SeqLocPtr ) sfp -> data.value.ptrvalue;
		p = FlatLocNote(the_seq,xref,optionHead);
		if (need_spaces)
			temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
		temp = FlatSmartStringMove(total_buf,lenPt,temp,preface);
		temp = FlatSmartStringMove(total_buf,lenPt,temp,p );
		MemFree(p);
		temp = FlatSmartStringMove(total_buf,lenPt,temp,".");
		need_spaces ++;
	}
	if (note_text != NULL){
		if (need_spaces)
			temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
		temp = FlatSmartStringMove(total_buf,lenPt,temp,note_text);
		need_spaces ++;
	}
	if (gi > 0){
    if (total_buf){
			if (need_spaces)
			if (FlatEndsPunc (total_buf)){
				temp = FlatSmartStringMove(total_buf,lenPt,temp,"  ");
			}else{
				temp = FlatSmartStringMove(total_buf,lenPt,temp,";  ");
			}
		}else{
			if (need_spaces)
				temp = FlatSmartStringMove(total_buf,lenPt,temp,";  ");
		}
		temp = FlatSmartStringMove(total_buf,lenPt,temp,"NCBI gi: ");
		sprintf(pro_num, "%ld",(long) gi);
		temp = FlatSmartStringMove(total_buf,lenPt,temp, pro_num);
		temp = FlatSmartStringMove(total_buf,lenPt,temp,";");
		need_spaces ++;
	}
	if (total_buf){
		if (need_spaces > 1){
			if ( *(temp-1) == ';')
				*(temp-1) = '.';
		}else{
			if (FlatEndsPunc (total_buf)){
				* (total_buf + StringLen(total_buf)-1) = '\0';
				temp --;
			}
		}
	}
	temp = FlatSmartStringMove(total_buf,lenPt,temp, "\"");

}

/*----------------------- FlatAnnotAvoidPartial()-----------*/
Boolean
FlatAnnotAvoidPartial (SeqFeatPtr sfp)
{
	Boolean retval = FALSE;
	CharPtr str;
	ImpFeatPtr imp;

	if ( sfp -> data.choice == 8){
/*  Imp-feat    */
		imp = (ImpFeatPtr) (sfp -> data.value.ptrvalue);
		if (imp ->loc != NULL)
		for( str = imp -> loc; *str; str ++){
			if ( *str == '<' || *str == '>'){
				retval = TRUE;
				break;
			}else if (*str == 'r' && *(str +1) == 'e'){
				if (StringNCmp ("replace",str, (size_t) 7) == 0){
					retval = TRUE;
					break;
				}
			}
		}
	}

	return retval;
}

/*--------------------- FlatAnnot()--------------------------*/

int 
FlatAnnot 
(FILE *fp, SeqEntryPtr the_seq, SeqAnnotPtr annot, Int2 PNTR countPt, Boolean check_last_interval, FLAT_PUB_STORE_PTR refhead, AsnOptionPtr PNTR optionHeadPt)
{
	SeqFeatPtr sfp;
	CharPtr feat_type=NULL, sgml_temp, temp, locs, line;
	ValNodePtr product = NULL;
	SeqIdPtr sid= NULL, pro_sid;
	BioseqPtr protein = NULL;
	RnaRefPtr rna;
	ImpFeatPtr imp;
	CharPtr qual_start = NULL;
	CdRegionPtr cds = NULL;
	CharPtr kludge;
	char feat_buf[18];
	int dex;
	int retval = TRUE;
	CharPtr pro_comment = NULL;
    SeqPortPtr spp;
    char localbuf[85], buf [ 80];
    Int2 i;
    Uint1 repr, code, residue;
		BioseqPtr bsp;
		SeqIntPtr si;
		ValNodePtr location = NULL;
		Int4 note_len;
		Int2 sgml_len;
		BioseqPtr bs;
		FeatureStatePtr prot_state, gene_state;
		GeneRefPtr bound_gene;
		ProtRefPtr bound_prot;
		ValNodePtr code_name = NULL;
		Boolean did_partial, did_gbqual_product, did_gbqual_gene;
	SeqLocPtr xref;
	SeqIdPtr xid;
	CharPtr note_text = NULL;
	CharPtr standard_name_text = NULL;
	CharPtr gene_text = NULL;
	CharPtr map_text = NULL;
	Boolean avoid_maploc = FALSE;
	DataVal asndata;
	AsnOptionPtr optionHead = * optionHeadPt;
	Boolean doing_CDS_from_product = FALSE;
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

	for ( sfp = (SeqFeatPtr) annot -> data;
/*---MemFree can handle NULLs, and the 
			continue forces inclusion here ---*/
			MemFree(feat_type), feat_type=NULL,  sfp; sfp = sfp -> next){
		MemFree(note_text);
		note_text = NULL;
		avoid_maploc = FALSE;
		if (pro_comment){
			MemFree(pro_comment);
			pro_comment = NULL;
		}
		if (check_last_interval)
			if ( ! do_this_feature_now (the_seq,sfp -> location, 
					optionHead)) {
				if ( Flat_nuc_prot_which < 2){
					continue;
				}else if ( ! do_this_feature_now (the_seq,sfp -> product, 
						optionHead)) {
					continue;
				}else{
					doing_CDS_from_product = TRUE;
				}
	 		}
		sid= NULL;
		product = NULL;
		protein = NULL;
		Fuzz_found = FALSE;
		switch ( sfp -> data.choice){

/*----------------------
			The whole set is at the end of this comment, the
       only types taken to the flat file are:

	CDS from 3
 RNA types from 4
 imp from imp-feat, from 8, where the rest of the features were
     put when genbank goes to asn.1.

    0 = not set
    1 = gene, data.ptrvalue = Gene-ref ,

Output with CDS, if bound, else as misc_feature

    2 = org , data.ptrvalue = Org-ref ,

Output in SOURCE line IF promoted to desc, which only does
if normal case.  If left as feature, is lost.

    3 = cdregion, data.ptrvalue = Cdregion ,

Well represented.

    4 = prot , data.ptrvalue = Prot-ref ,

Output with CDS, if bound, else as misc_feature (if on DNA)
LOST if on Protein not bound to DNA

    5 = rna, data.ptrvalue = RNA-ref ,
        unknown (0) ,  LOST
        premsg (1) ,
				case 1:
					feat_type = StringSave("prim_transcript");
        mRNA (2) ,
				case 2:
					feat_type = StringSave("mRNA");
        tRNA (3) ,
				case 3:
					feat_type = StringSave("tRNA");
        rRNA (4) ,
				case 4:
					feat_type = StringSave("rRNA");
        snRNA (5) ,
				case 5:
					feat_type = StringSave("snRNA");
in use now....
        scRNA (6) ,  <<<<LOST (should it be misc_RNA?)
        other (255)  ,
				case 255:
					feat_type = StringSave("misc_RNA");
    6 = pub, data.ptrvalue = Pubdesc ,  -- publication applies to this seq 

in REFERENCE BLOCK

    7 = seq, data.ptrvalue = Seq-loc ,  -- for tracking source of a seq.
			For Xrefs, look for special TextSeqIds by type.
    8 = imp, data.ptrvalue = Imp-feat ,
Canonical GenBank from gbparse
    9 = region, data.ptrvalue= VisibleString,      -- for a name
now misc_feature
    10 = comment, data.ptrvalue= NULL ,             -- just a comment
Now captured to COMMENT block, if this entire sequence, else
/note on a misc_feature.
    11 = bond, data.intvalue = ENUMERATED 
        disulfide (1) ,
        thiolester (2) ,
        xlink (3) ,
        other (255)  ,
LOST

    12 = site, data.intvalue = ENUMERATED
        active (1) ,
        binding (2) ,
        cleavage (3) ,
        inhibit (4) ,
        modified (5),
        other (255)  ,
LOST

    13 = rsite, data.ptrvalue = Rsite-ref
now goes to misc_feature...

    14 = user, data.ptrvalue = UserObjectPtr
LOST

------------*/
		case 1: /* gene */
/*----- also checks qualifier list in options ---*/
			if (this_gene_is_bound (sfp, optionHead))
				continue;
			feat_type = StringSave(GENE_FEATURE_NAME);
			locs = FlatLoc (the_seq,  sfp -> location, optionHead);
			break;
		case 3: 
			feat_type = StringSave("CDS");
			locs = FlatLoc (the_seq,  sfp -> location, optionHead);
			cds = (CdRegionPtr ) sfp -> data.value.ptrvalue;
			product = sfp -> product;
			if (product)
			if (product -> choice == 3){
/*--- whole, the only CDS that makes sense ---*/
				sid = (SeqIdPtr) ( product -> data.ptrvalue);
				protein = BioseqFind (sid);
			}
			break;
		case 4: /* prot */
			if (this_prot_is_bound (sfp, optionHead))
				continue;
			feat_type = StringSave(PROT_FEATURE_NAME);
			locs = FlatLoc (the_seq,  sfp -> location, optionHead);
			break;
		case 5: /* RNA types */
			rna = (RnaRefPtr) (sfp -> data.value.ptrvalue);
			locs = FlatLoc (the_seq,  sfp -> location, optionHead);
			switch ( rna -> type){ /* order of case n: matches tests in
                                 is_RNA_type() of genasn.c in
                                 GenBankConversion directory */
				case 2:
					feat_type = StringSave("mRNA");
					break;
				case 255:
					if (rna ->ext.choice == 1){
						feat_type = 
							StringSave(rna ->ext.value.ptrvalue);
					}else{
						feat_type = StringSave("misc_RNA");
					}
					break;
				case 3:
					feat_type = StringSave("tRNA");
					break;
				case 4:
					feat_type = StringSave("rRNA");
					break;
				case 1:
					feat_type = StringSave("prim_transcript");
					break;
				case 5:
					feat_type = StringSave("snRNA");
					break;
				case 6:
					feat_type = StringSave("scRNA");
					break;
			}
			break;
		case 7:  /* seq cross reference == accession */
				rt=SeqLocCompare(sfp -> location, location);
				if (rt != SLC_B_IN_A && rt != SLC_A_EQ_B &&
						rt != SLC_NO_MATCH){
					xref = (SeqLocPtr ) sfp -> data.value.ptrvalue;
					xid = (SeqIdPtr) xref -> data.ptrvalue;
               if (xid -> choice != 5 && xid -> choice != 6
                     && xid -> choice != 13){
                  feat_type = StringSave(XREF_FEATURE_NAME);
                  locs = FlatLoc (the_seq,  sfp -> location, optionHead);
               }
				}
			break;
		case 8: /* Import feature */
			imp = (ImpFeatPtr) (sfp -> data.value.ptrvalue);
			if ( StringCmp(imp -> key, "Site-ref") == 0)
				break;	/* only for Ref, gotten in FlatFeatPubs() */
			feat_type = StringSave ( imp -> key);
			if ( StringCmp(imp -> key, SOURCE_FEATURE_NAME) == 0){
				asndata.ptrvalue = imp;
				AsnOptionNew ( optionHeadPt, OP_TOGENBNK, 
					OP_TOGENBNK_SOURCE_FOUND, asndata, 
					/*(AsnOptFreeFunc)*/ NULL);
			}

			if (imp -> loc){
				locs = StringSave(imp -> loc);
				if (Flat_release){
					Boolean keep_loc = FALSE;
					CharPtr back_loc;

					if (!StringCmp("CDS", feat_type)){
						ErrPost(CTX_NCBI2GB, CTX_2GB_IMP_FEAT_CDS,
 					"Imp-feat-CDS loc=",locs?locs:"-bad loc-");
					}
					if (locs)
						if (*locs){
							if (StringNCmp(locs,"replace",7) == 0 ){
								for (back_loc = locs + StringLen(locs) -1;
										back_loc > locs; back_loc --){
									if (*back_loc == ' '){
										continue;
									}
  /* ( */  						if (*back_loc == ')'){
											keep_loc = TRUE;
										}
								}
							}else{
								keep_loc = TRUE;
							}
						}
						if (! keep_loc){
         ErrPost(CTX_NCBI2GB, CTX_2GB_BAD_LOC,
         "WARNING dropping feature <%s> for bad loc <%s>",
         feat_type, locs);
							MemFree(locs);
							locs = NULL;
							MemFree(feat_type);
							feat_type = NULL;
						}
				}
			}else
				locs = FlatLoc (the_seq,  sfp -> location, optionHead);
			break;
		case 9:  /* region */
			feat_type = StringSave(REGION_FEATURE_NAME);
			locs = FlatLoc (the_seq,  sfp -> location, optionHead);
			break;
		case 10: /* comment */
				if (SeqLocCompare(sfp -> location, location) != SLC_A_EQ_B ){
					feat_type = StringSave(COMMENT_FEATURE_NAME);
					locs = FlatLoc (the_seq,  sfp -> location, optionHead);
				}
			break;
		case 13:
			feat_type = StringSave(RSITE_FEATURE_NAME);
			locs = FlatLoc (the_seq,  sfp -> location, optionHead);
			break;
		}
		if ( ! feat_type || ! locs) 
			continue;
		if (*locs == '\0'){
			ErrPost(CTX_NCBI2GB,CTX_2GB_LOCATION_TROUBLE,
			"Skipping empty location field for a %s feature",
			feat_type);
			continue;
		}
		if ( ! *countPt){
				if (Flat_do_Flat)
			if (Flat_format ==0){
				fprintf(fp,
				"FEATURES             Location/Qualifiers\n");
			}else{
				fprintf(fp,
				"FH   Key             Location/Qualifiers\nFH   \n");
			}
			fflush(fp);
		}
		(*countPt) ++;
		line = do_FlatCont(58,21,locs, FALSE, optionHead);
		for (dex=0; dex < 17; dex++) feat_buf[dex] = ' ';
		feat_buf[16] = '\0';
		
	for( dex=0, temp = feat_type, kludge = feat_buf;
			* temp && dex < 17; dex++, temp ++, kludge ++)
		*kludge = *temp;

   kludge = temp = MemNew ( (size_t) (StringLen(Blank_BASE_LTR) + 18 +
          StringLen (line) + 2) );

   temp = StringMove (temp, Blank_BASE_LTR);
		temp = StringMove (temp, feat_buf);
   temp = StringMove (temp, line);
   temp = StringMove (temp, "\n");
		kludge = EMBL_Cont_Cleanup(kludge);
				if (Flat_do_Flat)
   FilePuts(kludge, fp);
		fflush(fp);
   if (kludge)
      MemFree(kludge);
/****
		fprintf(fp,
		"     %-17s%s\n", feat_type,line);
*****/
		MemFree(locs);
		MemFree(line);
		did_partial = did_gbqual_product = did_gbqual_gene = FALSE;
		FlatAnnotGBQual (fp, sfp, & did_partial, & note_text, 
			& standard_name_text, & gene_text, & map_text,
			& did_gbqual_gene, & did_gbqual_product,doing_CDS_from_product,
			  optionHead);
		if (sfp -> partial && ! did_partial && ! Fuzz_found &&
				 ! FlatAnnotAvoidPartial (sfp)){
			line = do_FlatCont(58,21,"/partial", FALSE, optionHead);
			MACRO_FLAT_LINE_OUT(temp,FEAT_CONT_BLANKS, line,kludge)
			fflush(fp);
			MemFree(line);
		}
		note_len = 0;
		temp = NULL;
		FlatAnnotComposeNote(the_seq, sfp, protein, cds,note_text, 
				standard_name_text, gene_text,
				did_gbqual_product, did_gbqual_gene, feat_type, 
				 NULL,  & note_len, temp, refhead,doing_CDS_from_product,
					  optionHead);
		if (note_len > 8){
			temp = qual_start = MemNew( (size_t) ( 2 + note_len));
			note_len = 0;  /* not absolutely needed... */
			FlatAnnotComposeNote(the_seq, sfp, protein, cds,note_text, 
				standard_name_text, gene_text,
				did_gbqual_product, did_gbqual_gene, feat_type, 
				 qual_start,  & note_len, temp, refhead,doing_CDS_from_product,
			  optionHead);
			line = do_FlatCont(58,21,qual_start, FALSE, optionHead);
			MACRO_FLAT_LINE_OUT(temp,FEAT_CONT_BLANKS, line,kludge)
			fflush(fp);
			MemFree(qual_start);
			qual_start = NULL;
			MemFree(line);
		}
		for ( gene_state = find_bound_genes_init (sfp, did_gbqual_gene, & bound_gene,  
					optionHead);bound_gene;
				gene_state = bound_gene_next(& bound_gene, gene_state) ){
			temp = NULL;
			if (bound_gene -> locus){
				sgml_len = Sgml2AsciiLen(bound_gene -> locus);
				temp = qual_start = MemNew( (size_t) ( 
					9 + sgml_len + 1));
				sgml_temp = temp = StringMove(temp,"/gene=\"");
				temp = Sgml2Ascii(bound_gene -> locus, 
					temp, sgml_len+1);
				if (! temp){
					ErrPost(CTX_NCBI2GB,CTX_2GB_INTERNAL,
					"temp killed by Sgml2Ascii");
				}
				Flat_out_index_line ( Flat_Index_gene, sgml_temp);
			}
			if (temp){
				temp = StringMove(temp, "\"");
				line = do_FlatCont(58,21,qual_start, FALSE, optionHead);
				MACRO_FLAT_LINE_OUT(temp,FEAT_CONT_BLANKS, line,kludge)
			fflush(fp);
				MemFree(qual_start);
				qual_start = NULL;
				MemFree(line);
			}
			if (bound_gene -> maploc){
				if (map_text != NULL)
					if (StringCmp(map_text,bound_gene -> maploc) == 0)
						avoid_maploc = TRUE;
				if ( ! avoid_maploc ){
					temp = qual_start = MemNew( (size_t) ( 
						8 + StringLen(bound_gene -> maploc)
						));
					temp = StringMove(temp,"/map=\"");
					temp = StringMove(temp, bound_gene -> maploc);
					temp = StringMove(temp, "\"");
					line = do_FlatCont(58,21,qual_start, FALSE, optionHead);
					MACRO_FLAT_LINE_OUT(temp,FEAT_CONT_BLANKS, line,kludge)
					fflush(fp);
					MemFree(qual_start);
					qual_start = NULL;
					MemFree(line);
				}
			}
		}
		MemFree(gene_state);
		if (!doing_CDS_from_product ){
		for ( prot_state = find_bound_prots_init(sfp,did_gbqual_product, & bound_prot,  
					optionHead);bound_prot;
				prot_state = bound_prot_next(& bound_prot, prot_state) ){
				temp = NULL;
				if (FlatSafeAsnThere(bound_prot -> name )){
					temp = qual_start = MemNew( (size_t) ( 
						12 + StringLen(bound_prot -> name -> data.ptrvalue)
						));
					temp = StringMove(temp,"/product=\"");
					temp = StringMove(temp, bound_prot -> 
						name -> data.ptrvalue);
				}
			if (temp){
				temp = StringMove(temp, "\"");
				line = do_FlatCont(58,21,qual_start, FALSE, optionHead);
				MACRO_FLAT_LINE_OUT(temp,FEAT_CONT_BLANKS, line,kludge)
			fflush(fp);
				MemFree(qual_start);
				qual_start = NULL;
				MemFree(line);
			}
				
		}
		MemFree(prot_state);
		}
		if (sfp -> data.choice ==  8 ||    StringCmp("CDS", feat_type))
			continue;
				if (Flat_do_Flat)
		if (cds -> genetic_code != NULL) {/* name we can take */
			code_name = cds->genetic_code;
			while ((code_name != NULL) && (code_name->choice != 1))
				code_name = code_name->next;
			if (code_name != NULL)
				note_len += 2 + StringLen(code_name->data.ptrvalue);
		}
		
		if ( ! protein)
			continue;
		bsp = protein;	
    repr = Bioseq_repr(bsp);
    if (! ((repr == Seq_repr_raw) || (repr == Seq_repr_const))){
			pro_sid = bsp -> id;
		 if (pro_sid)
			SeqIdPrint (pro_sid, buf, PRINTID_FASTA_LONG );
		else
			StringCpy(buf,"<NO ID>");
		if ( !  (Flat_Be_quiet&1))
     ErrPost(CTX_NCBI2GB,CTX_2GB_BAD_REPR,
			"FlatAnnot: Protein not raw or const: %s\n", buf);
			retval = FALSE;
			goto BYEBYE;
		}

    if (ISA_na(bsp->mol))
				continue;
		if (doing_CDS_from_product )
			continue;

	  code = Seq_code_iupacaa;

		if (Flat_do_Flat){
			fprintf(fp,
			"%-21s/codon_start=%d\n", Flat_format==0?" ":"FT",cds->frame > 1?
			(int) ( cds -> frame):1 );
			fflush(fp);
		}

    spp = SeqPortNew(bsp, 0, -1, 0, code);
    SeqPortSeek(spp, 0, SEEK_SET);
		sprintf(localbuf,
			"%-21s/translation=\"", Flat_format==0?" ":"FT");
    i = StringLen(localbuf);
    while ((residue = SeqPortGetResidue(spp)) != SEQPORT_EOF)
    {
			if (residue == SEQPORT_EOS || residue == SEQPORT_VIRT)
				break;
			if ( !IS_residue(residue) && residue != INVALID_RESIDUE)
				continue;
/* need to check for INVALID_RESIDUE */
        if (residue == INVALID_RESIDUE){
				residue = (Uint1) 'X';
        }
        if (i == 79) {
            localbuf[i] = '\0';
				if (Flat_do_Flat)
            fprintf(fp, "%s\n", localbuf);
			fflush(fp);
					sprintf(localbuf,
						"%-21s", Flat_format==0?" ":"FT");
				 i = StringLen(localbuf);
        }
        localbuf[i] = residue;
        i++;
    }
        if (i == 79) {
            localbuf[i] = '\0';
				if (Flat_do_Flat)
            fprintf(fp, "%s\n", localbuf);
			fflush(fp);
					sprintf(localbuf,
						"%-21s", Flat_format==0?" ":"FT");
				 i = StringLen(localbuf);
        }
        localbuf[i] = '\"';
        i++;
			localbuf[i] = '\0';
				if (Flat_do_Flat)
        fprintf(fp, "%s\n", localbuf);
			fflush(fp);
    SeqPortFree(spp);
	}
	BYEBYE:
		if (location){
			si -> id = NULL;
			SeqIntFree(si);
			ValNodeFree(location);
		}
		MemFree(note_text);
		MemFree(pro_comment);
		return retval;
}

	typedef struct flat_gb_qual {
		CharPtr name;
		Int1 is_text;
		int seq_mod_val;
		Boolean used;
	} FlatQual, PNTR FlatQualPtr;

#define LONER_QUAL 1
#define TEXT_QUAL 2
#define CITE_QUAL 3
#define NONE_TEXT_QUAL 4
	static FlatQual Known_qual[38] = {
 "cell_line", TEXT_QUAL, 0, FALSE
, "cell_type", TEXT_QUAL, 0, FALSE
, "chromosome", TEXT_QUAL, 0, FALSE
, "chloroplast", LONER_QUAL, 5, FALSE
, "citation", CITE_QUAL, 0, FALSE
, "clone", TEXT_QUAL, 0, FALSE
, "clone_lib", TEXT_QUAL, 0, FALSE
, "cultivar", TEXT_QUAL, 0, FALSE
, "cyanelle", TEXT_QUAL, 7, FALSE
, "dev_stage", TEXT_QUAL, 0, FALSE
, "germline",LONER_QUAL, 0, FALSE
, "haplotype", TEXT_QUAL, 0, FALSE
, "insertion_seq", LONER_QUAL, 15, FALSE
, "isolate", TEXT_QUAL, 0, FALSE
, "kinetoplast", LONER_QUAL, 6, FALSE
, "label",NONE_TEXT_QUAL, 0, FALSE
, "lab_host",TEXT_QUAL, 0, FALSE
, "map", TEXT_QUAL, 0, FALSE
, "macronuclear",LONER_QUAL, 18, FALSE
, "mitochondrion", LONER_QUAL, 4, FALSE
, "note", TEXT_QUAL, 0, FALSE
, "organism", TEXT_QUAL, 0, FALSE
, "partial",LONER_QUAL, 0, FALSE
, "pop_variant", TEXT_QUAL, 0, FALSE
, "plasmid", TEXT_QUAL, 3, FALSE
, "proviral", LONER_QUAL, 19, FALSE
, "sex", TEXT_QUAL, 0, FALSE
, "sequenced_mol", TEXT_QUAL, 0, FALSE
, "specific_host", TEXT_QUAL, 0, FALSE
, "strain", TEXT_QUAL, 0, FALSE
, "sub_clone", TEXT_QUAL, 0, FALSE
, "sub_species", TEXT_QUAL, 0, FALSE
, "sub_strain", TEXT_QUAL, 0, FALSE
, "tissue_lib", TEXT_QUAL, 0, FALSE
, "tissue_type", TEXT_QUAL, 0, FALSE
, "transposon", TEXT_QUAL, 14, FALSE
, "usedin", NONE_TEXT_QUAL, 0, FALSE
, "variety", TEXT_QUAL, 0, FALSE
};
void
FlatSourceFeatDoQual(FILE * fp, CharPtr qual_use, CharPtr val, int dex)
{
	CharPtr line = NULL;
	CharPtr qual_start, temp;
	CharPtr kludge= NULL;

		temp= qual_start = MemNew ( (size_t) (
			StringLen (qual_use ) + 5+
			StringLen(val) )  );
		temp = StringMove (temp, "/");
		temp = StringMove(temp, qual_use);
		if (Known_qual[dex].is_text > LONER_QUAL)
			temp = StringMove (temp, "=");
		if (Known_qual[dex].is_text == TEXT_QUAL)
			temp = StringMove (temp, "\"");
		if (Known_qual[dex].is_text > LONER_QUAL)
			temp = StringMove(temp, val);
		if (Known_qual[dex].is_text == TEXT_QUAL)
			temp = StringMove(temp, "\"");
#if CITE_REALLU_USED
		if (  Known_qual[dex].is_text == CITE_QUAL)
			if ( qual -> val)
				if ( *(qual -> val) != '[')
		ErrPost(CTX_DEBUG,17,"Warning, improper citation value =<%s>",
			qual -> val);
#endif

		line = do_FlatCont(58,21,qual_start, FALSE, NULL);
		MACRO_FLAT_LINE_OUT(temp,FEAT_CONT_BLANKS, line,kludge)
		MemFree(line);
		MemFree(qual_start);
}


/*----------------- FlatInsureSourceFeature ()-------*/

void 
FlatInsureSourceFeature (FILE *fp, SeqEntryPtr the_seq, Int2Ptr feat_count, AsnOptionPtr optionHead)
{
	AsnOptionPtr source_opt;
	AsnOptionPtr org_opt;
	AsnOptionPtr seq_desc_opt = NULL;
	OrgRefPtr or;
	BioseqPtr bsp;
	CharPtr kludge= NULL;
	CharPtr line = NULL;
	CharPtr qual_start, temp;
	CharPtr found_tax = NULL;
	CharPtr qual_name;
	CharPtr note_name = "note";
	CharPtr org_name = "organism";
	CharPtr qual_text = NULL;
	int dex;
	ValNodePtr mod;
	static int num_known_qual = 38;
 static Boolean first = TRUE;
	Boolean was_virus = FALSE;
 static int PNTR len_known_qual;
	CharPtr p;
	CharPtr qual_use = NULL;
	int seq_desc_mod;

   if (first){
      first = FALSE;
      len_known_qual = MemNew(sizeof(Int4) * num_known_qual);
      for (dex = 0; dex < num_known_qual; dex ++){
         len_known_qual[dex] = StringLen(Known_qual[dex].name);
      }
   }
	for (dex = 0; dex < num_known_qual; dex ++){
		Known_qual[dex].used = FALSE;
	}

	source_opt = AsnOptionGet(optionHead, OP_TOGENBNK, 
		OP_TOGENBNK_SOURCE_FOUND , NULL);
	if (the_seq -> choice == 1){
		bsp = the_seq -> data.ptrvalue;
		if (source_opt == NULL){
			org_opt = AsnOptionGet(optionHead, OP_TOGENBNK, 
				OP_TOGENBNK_ORGREF , NULL);
			or = (OrgRefPtr) org_opt -> data.ptrvalue;
			if ( ! *feat_count){
				if (Flat_do_Flat)
				if (Flat_format ==0){
					fprintf(fp,
					"FEATURES             Location/Qualifiers\n");
				}else{
					fprintf(fp,
					"FH   Key             Location/Qualifiers\nFH   \n");
				}
				fflush(fp);
			}
			(*feat_count) ++;
			line = MemNew(80);
			sprintf(line,"1..%ld", (long) bsp -> length);
			temp = kludge = MemNew(80);

			temp = StringMove (temp, Blank_BASE_LTR);
			temp = StringMove (temp, SOURCE_FEATURE_NAME);
			temp = StringMove (temp, "          ");
			temp = StringMove (temp, line);
			temp = StringMove (temp, "\n");
			kludge = EMBL_Cont_Cleanup(kludge);
			if (Flat_do_Flat)
				FilePuts(kludge, fp);
			fflush(fp);
			MemFree(kludge);
			MemFree(line);
			if (or) {
				if (or -> taxname){
					found_tax = or -> taxname;
					qual_name = org_name;
					qual_text = found_tax;
				}
				if ( ! or -> taxname && or -> common){
					if (
StrStr(or -> common, "virus") ||
StrStr(or -> common, "Virus") ||
StrStr(or -> common, "phage") ||
StrStr(or -> common, "Phage") ||
StrStr(or -> common, "viroid") ||
StrStr(or -> common, "Viroid") 
					){
						was_virus = TRUE;
						found_tax = or -> common;
						qual_name = org_name;
						qual_text = found_tax;
					}
				}
			}
			if (!found_tax){
				qual_name = org_name;
				qual_text = "unknown";
			}
			temp= qual_start = MemNew ( (size_t) (
				StringLen (qual_name ) + 5+
				StringLen(qual_text) )  );
			
			temp = StringMove (temp, "/");
			temp = StringMove(temp, qual_name);
			temp = StringMove (temp, "=\"");
			temp = StringMove(temp, qual_text);
			temp = StringMove(temp, "\"");
			
			line = do_FlatCont(58,21,qual_start, FALSE, optionHead);
			MACRO_FLAT_LINE_OUT(temp,FEAT_CONT_BLANKS, line,kludge)
			fflush(fp);
			MemFree(line);
			MemFree(qual_start);
			if (or){
				if ( or -> common && ! was_virus ){
					if ( * or -> common){
						qual_name = note_name;
						qual_text = or -> common;
						temp= qual_start = MemNew ( (size_t) (
							StringLen (qual_name ) + 5+
							StringLen(or-> common) )  );
						
						temp = StringMove (temp, "/");
						temp = StringMove(temp, qual_name);
						temp = StringMove (temp, "=\"");
						temp = StringMove(temp, or -> common);
						temp = StringMove(temp, "\"");
						line = do_FlatCont(58,21,qual_start, FALSE, optionHead);
						MACRO_FLAT_LINE_OUT(temp,FEAT_CONT_BLANKS, line,kludge)
						MemFree(line);
						MemFree(qual_start);
						fflush(fp);
					}
				}
		}
		
		if (or){  /* now deal with OrgRef.mod */
			for (mod = or -> mod; mod; mod = mod -> next){
				p = (CharPtr) mod -> data.ptrvalue;
				for (dex = 0; dex < 38; dex ++){
					if (StringNCmp(p, Known_qual[dex].name, 
							len_known_qual[dex]) == 0 &&

/* inhibit recognition of citation qualifiers ****/
							(int) (Known_qual[dex].is_text) != CITE_QUAL){
						break;
					}
				}
				if (dex < 38){
					p += len_known_qual[dex];
					if (* p == ' ' || *p == '='){
						p ++;
/*---- got a qual to use ---*/
						Known_qual[dex].used = TRUE;;
						qual_use = MemNew(len_known_qual[dex] + 2);
						StringNCpy(qual_use,
							(CharPtr) mod -> data.ptrvalue,
							len_known_qual[dex]);
							FlatSourceFeatDoQual(fp, qual_use, p, dex);
					}
				}
			}
						
		}
			while ( (seq_desc_opt  = AsnOptionGet(optionHead, 
						OP_TOGENBNK, OP_TOGENBNK_SEQ_DESCR, 
						seq_desc_opt)) != NULL){
				seq_desc_mod = (int) (seq_desc_opt -> data.intvalue);
				if (seq_desc_mod <= 0)
					continue;
				for (dex = 0; dex < 38; dex ++){
					if (Known_qual[dex].seq_mod_val == seq_desc_mod){
						break;
					}
				}
				if (dex < 38){
					if ( !Known_qual[dex].used){
							Known_qual[dex].used = TRUE;;
							FlatSourceFeatDoQual(fp, Known_qual[dex].name,
								 "", dex);
					}
				}

			}
		}  /* end action if no source feature */
	}
}



/*--- the number of characters per location element is less
      than 130.  The maximum would be for a bond with both accessions
      and both with (n.m) locations.
----*/
#define MAX_CHAR_LOCATION 135

	static char * lim_str [5] = {"", ">","<", ">", "<"};

/*----------- FlatLocHalf ()------*/
static void move_some PROTO (( CharPtr to, CharPtr from, Int4 start, Int4 end, Int4 PNTR countPt, AsnOptionPtr aop) );
static void add_line_break PROTO((CharPtr to, Int4 PNTR countPt, int blanks, AsnOptionPtr aop));
CharPtr FlatLocHalf 
(CharPtr buf, Int4 base, IntFuzzPtr fuzz, AsnOptionPtr optionHead)
{
	char localbuf [30];
	Uint1 index;

/*------
typedef struct intfuzz {
   Uint1 choice;        1=p-m, 2=range, 3=pct, 4=lim 
   Int4 a, b;           a=p-m,max,pct,orlim, b=min 
} IntFuzz, PNTR IntFuzzPtr;

Int-fuzz ::= CHOICE {
    p-m INTEGER ,                    -- plus or minus fixed amount
    range SEQUENCE {                 -- max to min
        max INTEGER ,
        min INTEGER } ,
    pct INTEGER ,                    -- % plus or minus (x10) 0-1000
    lim ENUMERATED {                 -- some limit value
        unk (0) ,                    -- unknown
        gt (1) ,                     -- greater than
        lt (2) ,                     -- less than
        tr (3) ,                     -- space to right of position
        tl (4) ,                     -- space to left of position
        other (255) } }              -- something else
-------*/

	localbuf[0] = '\0';
	buf[0] = '\0';

	if (fuzz){
		switch (fuzz -> choice){
			case 1:
				sprintf(localbuf,"(%ld.%ld)", (long) (base - fuzz -> a),
					(long) (base + fuzz -> a));
				break;
			case 2:
				sprintf(localbuf,"(%ld.%ld)", (long) (1+fuzz -> b),
					(long) (1+fuzz -> a));
				break;
			case 3:
				sprintf(localbuf,"(%ld.%ld)", 
					(long) (base - base* ((double) fuzz -> a/1000.0 )),
					(long) (base +base*( (double) fuzz -> a/1000.0 )));
				break;
			case 4:
				index = (Uint1) fuzz -> a;
				Fuzz_found = TRUE;
				if (index > 4) index = 0;
				sprintf(localbuf,"%s%ld", lim_str[index], (long) base);
				break;
			default:
			sprintf(localbuf,"%ld", (long) base);
		}
	}else{
		sprintf(localbuf,"%ld", (long) base);
	}

	StringMove(buf, localbuf);

	return buf;
}

/*----------- FlatLocHalfCaret ()------*/

CharPtr FlatLocHalfCaret
(CharPtr buf, Int4 base, IntFuzzPtr fuzz, AsnOptionPtr optionHead)
{
	char localbuf [30];
	Uint1 index;

	if (fuzz){
		switch (fuzz -> choice){
			case 1:
				break;
			case 2:
				sprintf(localbuf,"%ld^%ld", (long) (1+fuzz -> b),
					(long) (1+fuzz -> a));
				break;
			case 3:
				break;
			case 4:
				if (base > 1){
					sprintf(localbuf,"%ld^%ld", base-1, base);
				}else{
					index = (Uint1) fuzz -> a;
					if (index > 4) index = 0;
					sprintf(localbuf,"%s%ld", 
						lim_str[index], (long) base);
				}
				break;
			default:
			sprintf(localbuf,"%ld", (long) base);
		}
	}else{
		sprintf(localbuf,"%ld", (long) base);
	}

	StringMove(buf, localbuf);

	return buf;
}

/*----------- FlatLocPoint  ()------*/

Boolean 
FlatLocPoint 
(SeqIdPtr pointIdPtr, SeqIdPtr this_sidp, CharPtr piecebuf, Int4 point, IntFuzzPtr pointfuzzPtr, AsnOptionPtr optionHead)
{
	BioseqPtr bs;
	SeqIdPtr use_id;
	CharPtr buf, halfbuf, temp;
	Boolean OK = FALSE;
	static Boolean order_initialized = FALSE;
	static Uint1 order[18];
	ObjectIdPtr ob;
	
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

	buf = MemNew( (size_t)(MAX_CHAR_LOCATION +1));
	halfbuf = MemNew( (size_t)(MAX_CHAR_LOCATION +1));
	piecebuf[0] = '\0';
	buf[0] = '\0';
	temp = buf;

		if (pointIdPtr)
		if ( ! SeqIdIn ( pointIdPtr, this_sidp)){
			use_id = pointIdPtr;
			bs = BioseqFind ( use_id);
			if ( bs ){
				use_id = SeqIdSelect ( bs -> id, order,18);
			}
			SeqIdPrint( use_id, buf, PRINTID_TEXTID_ACCESSION);
			if (buf) if (*buf)
				OK = TRUE;
			if (! OK){
				if (FlatLoc_context == FLATLOC_CONTEXT_LOC ){
					if (use_id -> choice == SEQID_LOCAL){
						ob = (ObjectIdPtr) use_id -> data.ptrvalue;
						if (ob ->str){
							if (*ob -> str){
								StringNCpy(buf, ob ->str,12);
								OK = TRUE;
							}
						}
					}
					if (!OK)
						StringCpy(buf,"?00000");
				}else{
					SeqIdPrint( use_id, buf, PRINTID_TEXTID_LOCUS);
					if (buf) if (*buf)
						OK = TRUE;
					if (! OK){
						StringCpy(buf,"??????");
					}
				}
			}
			temp = StringMove (temp, buf);
			temp = StringMove(temp,":");
		}
		FlatLocHalf(halfbuf, point+1, pointfuzzPtr, optionHead);
    temp = StringMove(temp, halfbuf);
		StringMove(piecebuf, buf);

	MemFree(buf);
	MemFree(halfbuf);

	return TRUE;
}

/*----------- FlatLocCaret  ()------*/

Boolean 
FlatLocCaret 
(SeqIdPtr pointIdPtr, SeqIdPtr this_sidp, CharPtr piecebuf, Int4 point, IntFuzzPtr pointfuzzPtr, AsnOptionPtr optionHead)
{
	BioseqPtr bs;
	SeqIdPtr use_id;
	CharPtr buf, halfbuf, temp;
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

	buf = MemNew( (size_t)(MAX_CHAR_LOCATION +1));
	halfbuf = MemNew( (size_t)(MAX_CHAR_LOCATION +1));
	piecebuf[0] = '\0';
	buf[0] = '\0';
	temp = buf;

		if (pointIdPtr)
		if ( ! SeqIdIn ( pointIdPtr, this_sidp)){
			use_id = pointIdPtr;
			bs = BioseqFind ( use_id);
			if ( bs ){
				use_id = SeqIdSelect ( bs -> id, order,18);
			}
			SeqIdPrint( use_id, buf, PRINTID_TEXTID_ACCESSION);
			temp = StringMove (temp, buf);
			temp = StringMove(temp,":");
		}
		FlatLocHalfCaret(halfbuf, point+1, pointfuzzPtr, optionHead);
    temp = StringMove(temp, halfbuf);
		StringMove(piecebuf, buf);

	MemFree(buf);
	MemFree(halfbuf);

	return TRUE;
}

/*----------- FlatVirtLoc()------*/

Boolean 
FlatVirtLoc
(SeqEntryPtr the_seq, ValNodePtr location)
{
	Boolean retval = FALSE;
	SeqIntPtr sintp;
	BioseqPtr this_bsp=NULL;
	SeqIdPtr this_sidp=NULL, sidp = NULL;
	SeqPntPtr spp;

	if (the_seq  -> choice == 1){
		this_bsp = (BioseqPtr) the_seq -> data.ptrvalue;
		this_sidp = this_bsp -> id;
	}


	switch ( location -> choice){
		case SEQLOC_MIX:
		case SEQLOC_EQUIV:
		case SEQLOC_PACKED_INT:
		case  SEQLOC_PACKED_PNT:
		case  SEQLOC_NULL:
		break;
		case  SEQLOC_EMPTY:
			break;
		case  SEQLOC_WHOLE:
			      sidp = (SeqIdPtr) location -> data.ptrvalue;
      if (! sidp){
         if ( !  (Flat_Be_quiet&1))
ErrPost(CTX_NCBI2GB,CTX_2GB_LOCATION_TROUBLE,
"FlatLocElement: whole location without ID:");
            retval = TRUE;
            break;
      }
/*--- no break on purpose ---*/
		case  SEQLOC_INT:
			if ( location -> choice == SEQLOC_INT){
				sintp = (SeqIntPtr) location -> data.ptrvalue;
				sidp = sintp-> id;
			}
		
			if ( ! is_real_id(sidp, this_sidp)){
				retval = TRUE;
			}

			break;
		case  SEQLOC_PNT:
			spp = (SeqPntPtr) ( location -> data.ptrvalue);
			if ( ! is_real_id(spp-> id, this_sidp)){
				retval = TRUE;
			}
			break;
		case SEQLOC_BOND:
			break;
		case SEQLOC_FEAT:
		if ( !  (Flat_Be_quiet&1))
		ErrPost(CTX_NCBI2GB, CTX_2GB_NOT_IMPLEMENTED,
			"FlatVirtLoc:SEQLOC_FEAT not implemented");
			break;
	}


	return retval;
}

/*----------- FlatLocElement ()------*/

Boolean 
FlatLocElement 
(SeqEntryPtr the_seq, ValNodePtr location, CharPtr buf, AsnOptionPtr optionHead)
{
	CharPtr localbuf , piecebuf ;
	CharPtr temp ;
	SeqIntPtr sintp;
	SeqPntPtr spp;
	BioseqPtr this_bsp=NULL, bsp=NULL;
	SeqIdPtr this_sidp=NULL, sidp=NULL;
	SeqBondPtr bondp;
	SeqIntPtr whole_intPtr =NULL;
	Boolean retval=TRUE;
	Boolean whole_trouble;

	localbuf = MemNew ( (size_t) ( MAX_CHAR_LOCATION + 1));
	piecebuf = MemNew ( (size_t) ( MAX_CHAR_LOCATION + 1));
	temp = localbuf;

	if (the_seq  -> choice == 1){
		this_bsp = (BioseqPtr) the_seq -> data.ptrvalue;
		this_sidp = this_bsp -> id;
		bsp = this_bsp;
		sidp = this_sidp;
	}

	localbuf[0] = '\0';
	buf[0] = '\0';

	switch ( location -> choice){
		case SEQLOC_MIX:
		case SEQLOC_EQUIV:
		case SEQLOC_PACKED_INT:
		case  SEQLOC_PACKED_PNT:
		case  SEQLOC_NULL:
			ErrPost(CTX_NCBI2GB,CTX_2GB_LOCATION_TROUBLE,
			"Unexpected internal complex type");
			retval = FALSE;
		break;
		case  SEQLOC_EMPTY:
			break;
		case  SEQLOC_WHOLE:
    whole_trouble = TRUE;
		sidp = (SeqIdPtr) location -> data.ptrvalue;
		if (sidp){
			bsp = BioseqFind(sidp);
			if (bsp){
				sintp = whole_intPtr = MemNew( sizeof(SeqInt) );
				whole_intPtr -> id = sidp;
				whole_intPtr -> from = 0;
				whole_intPtr -> to = -1;
				if ( bsp -> length > 0){
					whole_intPtr -> to = bsp -> length -1 ;
					 whole_trouble = FALSE;
				}
			}
		}
		if (whole_trouble && FlatLoc_context == FLATLOC_CONTEXT_LOC ){
			if ( !  (Flat_Be_quiet&1))
ErrPost(CTX_NCBI2GB,CTX_2GB_LOCATION_TROUBLE,
"FlatLocElement: whole location without being able to look up limits: %s", sidp?SeqIdPrint (sidp, localbuf, PRINTID_FASTA_LONG):"No Id");
				retval = FALSE;
				break;
		}else if (whole_trouble){
				SeqIdPrint (sidp, localbuf, PRINTID_FASTA_LONG);
			break;
		}
/*--- no break on purpose ---*/
		case  SEQLOC_INT:
			if ( location -> choice == SEQLOC_INT){
				sintp = (SeqIntPtr) location -> data.ptrvalue;
			}
		
			if (is_real_id(sintp-> id, this_sidp)){
				if (sintp -> strand == 2) /* minus strand */
					temp = StringMove(temp, "complement("); /* ) vi match */
				FlatLocPoint ( sintp -> id, this_sidp, piecebuf, 
						sintp -> from, sintp -> if_from , optionHead);
				temp = StringMove(temp,piecebuf);
				if ( sintp -> to >0 && (sintp -> to != sintp -> from 
						|| sintp -> if_from ||  sintp -> if_to )){
					temp = StringMove(temp,"..");
					FlatLocPoint(NULL, this_sidp, piecebuf, 
						sintp -> to, sintp -> if_to, optionHead);
					temp = StringMove(temp, piecebuf);
				}
				if (sintp -> strand == 2) /* minus strand */
					/* ( vi match */ temp = StringMove(temp, ")");
			}else{
#ifdef VIRTUALS_NOT_TREATED_AS_NULLS
				StringCpy(localbuf,"No id");
				if (sintp -> id){
					SeqIdPrint (sintp -> id, localbuf, PRINTID_FASTA_LONG);
						ErrPost(CTX_NCBI2GB,CTX_2GB_LOCATION_TROUBLE,
					"FlatLocElement: interval without being able to use id: %s", 
						localbuf);
				}
				retval = FALSE;
#endif
			}

			break;
		case  SEQLOC_PNT:
			spp = (SeqPntPtr) ( location -> data.ptrvalue);
			if (is_real_id(spp-> id, this_sidp)){
				if (spp -> strand == 2) /* minus strand */
					temp = StringMove(temp, "complement("); /* ) vi match */
				if ( spp -> fuzz){
/*--------
 *  points with fuzz treated as if always come from '^': 
 *  not best, perhaps, but pretty close
 *-------*/
					FlatLocCaret ( spp -> id, this_sidp, piecebuf, 
							spp -> point, spp -> fuzz , optionHead);
				}else{
					FlatLocPoint ( spp -> id, this_sidp, piecebuf, 
							spp -> point, spp -> fuzz , optionHead);
				}
				temp = StringMove(temp, piecebuf);
				if (spp -> strand == 2) /* minus strand */
					/* ( vi match */ temp = StringMove(temp, ")");
			}else{
#ifdef VIRTUALS_NOT_TREATED_AS_NULLS
ErrPost(CTX_NCBI2GB,CTX_2GB_LOCATION_TROUBLE,
"FlatLocElement: point without being able to use id: %s", sidp?SeqIdPrint (sidp, localbuf, PRINTID_FASTA_LONG):"No Id");
				retval = FALSE;
#endif
			}
			break;
		case SEQLOC_BOND:
		bondp = (SeqBondPtr) location -> data.ptrvalue;
		spp = bondp -> a;
		FlatLocPoint ( spp -> id, this_sidp, piecebuf, 
				spp -> point, spp -> fuzz , optionHead);
		temp = StringMove(temp, piecebuf);
		StringMove(temp,",");
		spp = bondp -> b;
		FlatLocPoint ( NULL, this_sidp, piecebuf, 
				spp -> point, spp -> fuzz , optionHead);
		temp = StringMove(temp, piecebuf);
			break;
		case SEQLOC_FEAT:
		if ( !  (Flat_Be_quiet&1))
		ErrPost(CTX_NCBI2GB, CTX_2GB_NOT_IMPLEMENTED,
			"FlatLocElement:SEQLOC_FEAT not implemented");
			break;
	}

	if (whole_intPtr)
		MemFree(whole_intPtr);  /* NOT object free ! ! ! */
	
	if (! retval){
		*buf = '\0';
	}else{
		StringMove(buf, localbuf);
	}

	MemFree(localbuf);
	MemFree(piecebuf);

	return retval;
}

/*----------- FlatLoc  ()------*/

CharPtr FlatLoc 
(SeqEntryPtr the_seq, ValNodePtr location, AsnOptionPtr optionHead)
{
	CharPtr retval = NULL;
	Int4 max_length, len_used;
	Boolean is_ok = TRUE;
	BioseqPtr this_bsp;
	SeqIdPtr this_sidp;

	FlatLoc_context = FLATLOC_CONTEXT_LOC ;

	if (location){
		if (the_seq  -> choice == 1){
			this_bsp = (BioseqPtr) the_seq -> data.ptrvalue;
			this_sidp = this_bsp -> id;
		}
		max_length = 0;
		do_FlatLoc(& is_ok, TRUE, this_sidp, NULL, NULL, 
			& max_length, the_seq, location, optionHead);
		if (is_ok){
			retval = (CharPtr) MemNew((size_t) (max_length + 10));
			len_used = 0;
			do_FlatLoc(&is_ok, TRUE, this_sidp,  retval, retval, 
					&len_used, the_seq, location, optionHead);
		}
	}

	return retval;
}

/*----------- FlatLocNote  ()------*/

CharPtr FlatLocNote
(SeqEntryPtr the_seq, ValNodePtr location, AsnOptionPtr optionHead)
{
	CharPtr retval = NULL;
	Int4 max_length, len_used;
	Boolean is_ok = TRUE;
	BioseqPtr this_bsp;
	SeqIdPtr this_sidp;

	FlatLoc_context = FLATLOC_CONTEXT_NOTE ;

	if (location){
		if (the_seq  -> choice == 1){
			this_bsp = (BioseqPtr) the_seq -> data.ptrvalue;
			this_sidp = this_bsp -> id;
		}
		max_length = 0;
		do_FlatLoc(& is_ok, TRUE, this_sidp, NULL, NULL, 
			& max_length, the_seq, location, optionHead);
		if (is_ok){
			retval = (CharPtr) MemNew((size_t) (max_length + 10));
			len_used = 0;
			do_FlatLoc(&is_ok, TRUE, this_sidp,  retval, retval, 
					&len_used, the_seq, location, optionHead);
		}
	}

	return retval;
}


/*-------------complement_FlatLoc ()---------------*/

CharPtr
complement_FlatLoc (SeqIdPtr this_sidp, Boolean PNTR is_okPt, CharPtr total_buf, CharPtr temp, Int4Ptr lengthPt, SeqEntryPtr the_seq, ValNodePtr location, AsnOptionPtr optionHead)
{
	SeqLocRevCmp(location);

	temp = FlatSmartStringMove(total_buf, lengthPt, temp,"complement(");
	temp = do_FlatLoc (is_okPt, FALSE, this_sidp, total_buf, 
		temp, lengthPt, the_seq, location, optionHead);
	temp = FlatSmartStringMove(total_buf, lengthPt, temp,")");

	SeqLocRevCmp(location);

	return temp;
}
/*-------- FlatSmartStringMove()-------*/

CharPtr 
FlatSmartStringMove(CharPtr total_buf, Int4Ptr lengthPt, CharPtr temp, CharPtr string)
{
	CharPtr retval = temp;

	if ( lengthPt){
		(*lengthPt) += StringLen(string);
	}
	if (total_buf){
		retval = StringMove(temp,string);
	}

	return retval;
}
#define FLAT_EQUIV 0
#define FLAT_JOIN 1
#define FLAT_ORDER 2
static CharPtr group_names []= {
"one-of","join","order"};

/*--------- FlatNullAhead()---------*/

Boolean
FlatNullAhead(SeqEntryPtr the_seq, ValNodePtr location)
{
	Boolean retval = FALSE;
	SeqLocPtr next;
	if (location){
		next = location -> next;
		if (next){
			if ( next -> choice == SEQLOC_NULL)
				retval = TRUE;
			if (FlatVirtLoc(the_seq, next))
				retval = TRUE;
		}else{
			retval = TRUE;  /* last one always true */
		}
	}

	return retval;
}

								
/*---------- FlatPackedPoint() -----------*/

CharPtr
FlatPackedPoint (CharPtr total_buf, CharPtr temp, Int4Ptr lengthPt, PackSeqPntPtr pspp, SeqIdPtr this_sidp, AsnOptionPtr optionHead)
{
	int dex;
	CharPtr buf;

	buf = MemNew( (size_t)(MAX_CHAR_LOCATION +1));
	for (dex=0; dex < (int) pspp -> used; dex ++){
		FlatLocPoint ( pspp -> id, 
			this_sidp, buf, (pspp->pnts)[dex], pspp -> fuzz, 
			optionHead);
		 temp = FlatSmartStringMove(total_buf, 
					lengthPt, temp,buf);
	}
	MemFree(buf);

	return temp;
}
/*--------- group_FlatLoc ()-----------*/

CharPtr 
group_FlatLoc (SeqIdPtr this_sidp, Boolean PNTR is_okPt, int which, CharPtr total_buf, CharPtr temp, Int4Ptr lengthPt, SeqEntryPtr the_seq, ValNodePtr location, AsnOptionPtr optionHead)
{
	SeqLocPtr this_loc;
	Boolean special_mode = FALSE; /* join in order */
	int parens = 1;
	CharPtr buf;
	Boolean found_non_virt = FALSE;

	buf = MemNew( (size_t)(MAX_CHAR_LOCATION +1));
	
	 temp = FlatSmartStringMove(total_buf, lengthPt,
		temp,group_names[which]);
	 temp = FlatSmartStringMove(total_buf, lengthPt,
		temp,"(");
	
	for (this_loc = (SeqLocPtr) location -> data.ptrvalue;
			this_loc && *is_okPt; this_loc = this_loc -> next){
		if ( FlatVirtLoc(the_seq, this_loc)) {
			if ( this_loc != location && this_loc -> next){
				if (special_mode ){
					special_mode = FALSE;
					temp = FlatSmartStringMove(total_buf, lengthPt,
						temp,")");
					parens --;
				}
			}
			continue;
		}
		if ( found_non_virt &&
				this_loc -> choice !=  SEQLOC_EMPTY &&
				this_loc -> choice !=  SEQLOC_NULL ){
			temp = FlatSmartStringMove(total_buf, lengthPt,
				temp,",");
		}
		switch ( this_loc -> choice ){
		case SEQLOC_NULL : 
			if ( this_loc != location && this_loc -> next){
				if (special_mode ){
					special_mode = FALSE;
					temp = FlatSmartStringMove(total_buf, lengthPt,
						temp,")");
					parens --;
				}
			}
			break;
		case SEQLOC_EMPTY :
			break;
		case SEQLOC_WHOLE : 
		case SEQLOC_PNT : 
		case SEQLOC_BOND :
		case SEQLOC_FEAT :
		 found_non_virt = TRUE;
			if (FlatVirtLoc(the_seq,this_loc)){
				if ( this_loc != location && this_loc -> next){
					if (special_mode ){
						special_mode = FALSE;
						temp = FlatSmartStringMove(total_buf, lengthPt,
							temp,"),");
						parens --;
					}
				}
			}else{
				if( FlatLocElement(the_seq,this_loc, buf, 
						optionHead)){
					temp = FlatSmartStringMove(total_buf, lengthPt, 
						temp,buf);
				}else{
					temp = NULL;
					* is_okPt = FALSE;
				}
			}
			break;
		case SEQLOC_INT :
		 found_non_virt = TRUE;
			if ( which == FLAT_ORDER 
					&& ! FlatNullAhead(the_seq, this_loc)){
				special_mode = TRUE;
				 temp = FlatSmartStringMove(total_buf, lengthPt,
					temp,group_names[FLAT_JOIN]);
				 temp = FlatSmartStringMove(total_buf, lengthPt,
					temp,"(");
				parens ++;
			}
		
			if( FlatLocElement(the_seq, this_loc, buf, 
					optionHead)){
				temp = FlatSmartStringMove(total_buf, lengthPt, 
					temp,buf);
			}else{
				temp = NULL;
				* is_okPt = FALSE;
			}
			break;

		case SEQLOC_PACKED_PNT :
		 found_non_virt = TRUE;
				temp = FlatPackedPoint
					( total_buf, temp, lengthPt,
					(PackSeqPntPtr)this_loc->data.ptrvalue,
					this_sidp, optionHead);
			break;
		case SEQLOC_PACKED_INT :
		case SEQLOC_MIX :
		case SEQLOC_EQUIV :
		{
			ValNodePtr hold_next = this_loc -> next;
			 found_non_virt = TRUE;
			this_loc -> next = NULL;
			temp = do_FlatLoc(is_okPt, FALSE, this_sidp, total_buf, temp, lengthPt, 
				the_seq, this_loc, optionHead);
			this_loc-> next = hold_next;
		}
			break;
		}
	}

	if (* is_okPt){
		while ( parens > 0){
		 temp = FlatSmartStringMove(total_buf, lengthPt,
			temp,")");
			parens --;
		}
	}

	MemFree(buf);
	return temp;
}

CharPtr 
do_FlatLoc (Boolean PNTR is_okPt, Boolean ok_to_complement, SeqIdPtr this_sidp, CharPtr total_buf, CharPtr temp, Int4Ptr lengthPt, SeqEntryPtr the_seq, ValNodePtr location, AsnOptionPtr optionHead)
{
	SeqLocPtr slp;
	CharPtr buf;
	SeqLocPtr next_loc = NULL;
	Boolean did_complement = FALSE;

	buf = MemNew( (size_t)(MAX_CHAR_LOCATION +1));

	if (location){
		if ( ok_to_complement && 
				SeqLocStrand(location) == Seq_strand_minus ){
			temp = complement_FlatLoc (this_sidp, is_okPt, total_buf, 
				temp, lengthPt, the_seq, location, optionHead);
			did_complement = TRUE;
		}
		if ( ! did_complement)
		for ( slp = location; slp && *is_okPt; slp = slp -> next){
			if ( slp -> choice == SEQLOC_NULL ||
					FlatVirtLoc(the_seq, slp))
				continue;
			if ( slp != location){
				 temp = FlatSmartStringMove(total_buf, lengthPt,
					temp,",");
			}
			switch( slp -> choice){
				case  SEQLOC_NULL:
					break;
				case SEQLOC_MIX:
				case SEQLOC_PACKED_INT:
					{
						Boolean found_null = FALSE;
						for (next_loc = (SeqLocPtr) slp -> 
								data.ptrvalue; next_loc;
								next_loc = next_loc -> next){
							if ( next_loc -> choice == SEQLOC_NULL
									|| FlatVirtLoc( the_seq, next_loc)){
								found_null = TRUE;
								temp=group_FlatLoc(this_sidp, is_okPt,
											FLAT_ORDER, 
									total_buf, temp, lengthPt, the_seq, 
									slp, optionHead);
								break;
							}
						}
						if ( ! found_null){
							temp=group_FlatLoc(this_sidp, is_okPt,
											FLAT_JOIN, total_buf, 
								temp, lengthPt, the_seq, 
								slp, optionHead);
						}
					}
					break;
				case SEQLOC_EQUIV:
					temp=group_FlatLoc(this_sidp, is_okPt,
											FLAT_EQUIV, total_buf, temp,
						lengthPt, the_seq, slp, optionHead);
					break;
				case  SEQLOC_PACKED_PNT:
				temp = FlatPackedPoint
					( total_buf, temp, lengthPt,
					(PackSeqPntPtr)slp->data.ptrvalue,
					this_sidp, optionHead);
				break;
				default:
				if ( ! FlatVirtLoc( the_seq, slp))
				if( FlatLocElement(the_seq,slp, buf, optionHead)){
					temp = FlatSmartStringMove(total_buf, lengthPt, 
						temp,buf);
				}else{
					temp = NULL;
					*is_okPt = FALSE;
				}
			}
		}
	}


	MemFree(buf);

	return temp;
}

/*----------- FlatCont  ()------*/
CharPtr FlatCont 
(CharPtr line, Boolean do_tilde, AsnOptionPtr optionHead)
{
	CharPtr retval;

	retval =  do_FlatCont(67,Flat_line_blank,line, do_tilde, optionHead);

	return retval;
}

/*-------- EMBL_Cont_Cleanup ()-------------*/

CharPtr
EMBL_Cont_Cleanup(CharPtr line)
{

	if (Flat_format == ASN2GNBK_FORMAT_EMBL ||
			Flat_format == ASN2GNBK_FORMAT_EMBL_SEQ_LEFT ||
			Flat_format == ASN2GNBK_FORMAT_EMBL_SEQ_RIGHT ||
			Flat_format == ASN2GNBK_FORMAT_PATENT_EMBL
){
		CharPtr last_line, now_at;

		for (last_line = now_at = line; * now_at; now_at ++){
			if (* now_at == '\n'){
				if (*(now_at+1) ) if (* (now_at +1) == ' ') 
				if (*(now_at+2) ) if (* (now_at +2) == ' ') 
				if (now_at-last_line >= Flat_line_blank){
					*(now_at+1) = * last_line;
					*(now_at+2) = * (last_line+1);
					last_line = now_at+1;
				}
			}
		}
	}

	return line;
}

/*----------- do_FlatCont  ()------*/
CharPtr do_FlatCont 
(int limit, int blanks, CharPtr line, Boolean do_tilde, AsnOptionPtr optionHead)
{
	CharPtr retval ;
	Int4 len = StringLen(line), maxlen;
	Int4 last_blank = -1, last_comma = -1,  column, 
#ifdef USE_LAST_BUT
	Int4 last_but_comma = -1;
#endif
		restart, outcol;
	Boolean just_hit_limit = FALSE;

		retval = MemNew ( (size_t)(maxlen=(3 + 
			 (len + (6*( ((len/limit) +1)*(blanks +1 )  ))))));

		for (column = 0, outcol=0, restart = 0; 
				line[column] && column < len; column ++ ){
			if (column - restart >= limit ){
				just_hit_limit = TRUE;
				if (line[column] == ' ' ){
					last_blank = column;
				}else if (last_blank <= 0){
					if (last_comma > 1){
						last_blank=last_comma+1;
#ifdef USE_LAST_BUT
						if ( last_blank >= column){
							if ( last_but_comma > 1){
								last_blank=last_but_comma+1;
							}else{
								last_blank=restart + limit;
							}
						}
#endif
					}else{
						last_blank=restart + limit;
					}
				}
				move_some(retval,line, restart, last_blank, & outcol, optionHead);
				add_line_break(retval, & outcol, blanks, optionHead);
				if (line[last_blank] == ' ') {
					restart = last_blank +1;
				}else{
					restart = last_blank;
				}
				last_blank = last_comma = 
#ifdef USE_LAST_BUT
last_but_comma = 
#endif

0;
/****************************************
 ****                                ****
 **** If just hit limit, eat, without****
 **** affect, a \n that would have   ****
 **** forced a file break.           ****
 ****                                ****
 ****************************************/
				if (line[restart] == '\n'){
					restart ++;
					 column ++;
				}
			}else{
				just_hit_limit = FALSE;
			}
			if ( line[column] == ','){
#ifdef USE_LAST_BUT
				if ( last_comma > 0){
					last_but_comma = last_comma;
				}
#endif
				last_comma = column;
			}else if ( line[column] == ' '){
				last_blank = column;
			}else if ( line [column] == '\n' || 
					(do_tilde && line [column] == '~' ) ){
				move_some(retval,line, restart, column, & outcol, optionHead);
				do { 
					column ++;
				} while (column < len &&
					 (line [column] == '\n' || 
						(do_tilde && line [column] == '~') ) );
				if ( column+1 < len)
					add_line_break(retval, & outcol, blanks, optionHead);
				restart = column ;
				last_blank = last_comma = 
#ifdef USE_LAST_BUT
last_but_comma = 
#endif
0;
			}
			if (column >= len) 
				break;
		}
		move_some(retval,line, restart, column, & outcol, optionHead);
		if ( outcol >= maxlen){
		if ( !  (Flat_Be_quiet&1))
			ErrPost(CTX_NCBI2GB,CTX_2GB_INTERNAL,
				"FlatCont max %ld out %ld OOPS\n",
			(long) maxlen, (long) outcol);
			outcol = maxlen-1;
		}
		retval[outcol+1] = '\0';

	for (; outcol >0 && 
			(retval[outcol-1] == ' ' || retval[outcol-1] == '\n')
			; outcol --){
		retval[outcol-1] = '\0';
	}

	return retval;
}

/*----------- do_FlatContOrigin  ()------*/
CharPtr do_FlatContOrigin 
(int limit, int blanks, CharPtr line, AsnOptionPtr optionHead)
{
	CharPtr retval ;
	Int4 len = StringLen(line), maxlen;
	Int4 last_blank = -1, last_comma = -1,  column, 
#ifdef USE_LAST_BUT
	Int4 last_but_comma = -1;
#endif
		restart, outcol;

		retval = MemNew ( (size_t)(maxlen=(3 + 
			 (len + (3*( ((len/limit) +1)*(blanks +1 )  ))))));

		for (column = 0, outcol=0, restart = 0; 
				line[column] && column < len; column ++ ){
			if (column - restart >= limit ){
				if (line[column] == ' ' ){
					last_blank = column;
				}else if (last_blank <= 0){
					if (last_comma > 1){
						last_blank=last_comma+1;
#ifdef USE_LAST_BUT
						if ( last_blank >= column){
							if ( last_but_comma > 1){
								last_blank=last_but_comma+1;
							}else{
								last_blank=restart + limit;
							}
						}
#endif
					}else{
						last_blank=restart + limit;
					}
				}
				move_some(retval,line, restart, last_blank, & outcol, optionHead);
				add_line_break(retval, & outcol, blanks, optionHead);
				if (line[last_blank] == ' ') {
					restart = last_blank +1;
				}else{
					restart = last_blank;
				}
				last_blank = last_comma = 
#ifdef USE_LAST_BUT
last_but_comma = 
#endif

0;
			}
			if ( line[column] == ','){
#ifdef USE_LAST_BUT
				if ( last_comma > 0){
					last_but_comma = last_comma;
				}
#endif
				last_comma = column;
			}else if ( line[column] == ' '){
				last_blank = column;
			}else if ( line [column] == '\n' ){
				move_some(retval,line, restart, column, & outcol, optionHead);
				do { 
					column ++;
				} while (column < len && line [column] == '\n' );
				if ( column+1 < len)
					add_line_break(retval, & outcol, blanks, optionHead);
				restart = column ;
				last_blank = last_comma = 
#ifdef USE_LAST_BUT
last_but_comma = 
#endif
0;
			}
			if (column >= len) 
				break;
		}
		move_some(retval,line, restart, column, & outcol, optionHead);
		if ( outcol >= maxlen){
		if ( !  (Flat_Be_quiet&1))
			ErrPost(CTX_NCBI2GB,CTX_2GB_INTERNAL,
				"FlatCont max %ld out %ld OOPS\n",
			(long) maxlen, (long) outcol);
			outcol = maxlen-1;
		}
		retval[outcol+1] = '\0';

	for (; outcol >0 && retval[outcol] == ' '; outcol --){
		retval[outcol] = '\0';
	}

	return retval;
}

/*----------- do_FlatContKey  ()------*/
CharPtr 
do_FlatContKey (int limit, int blanks, CharPtr line, AsnOptionPtr optionHead)
{
	CharPtr retval ;
	Int4 len = StringLen(line), maxlen;
	Int4 last_blank, last_semi = -1,  column, 
		restart, outcol;

		retval = MemNew ( (size_t)(maxlen=(3 + 
			 (len + (3*( ((len/limit) +1)*(blanks +1 )  ))))));

		for (column = 0, outcol=0, restart = 0; 
				line[column] && column < len; column ++ ){
			if (column - restart >= limit ){
				if (last_semi > 1){
					last_blank=last_semi+1;
				}else{
					last_blank=restart + limit;
				}
				move_some(retval,line, restart, last_blank, & outcol, optionHead);
				add_line_break(retval, & outcol, blanks, optionHead);
				restart = last_blank;
				last_blank = last_semi = 0;
				if (line[restart] == ' ') {
					restart = restart +1;
				}
			}
			if ( line[column] == ';'){
				last_semi = column;
			}else if ( line [column] == '\n' ){
				move_some(retval,line, restart, column, & outcol, optionHead);
				do { 
					column ++;
				} while (column < len &&
					 (line [column] == '\n') );
				if ( column+1 < len)
					add_line_break(retval, & outcol, blanks, optionHead);
				restart = column ;
				last_blank = last_semi = 0;
			}
			if (column >= len) 
				break;
		}
		move_some(retval,line, restart, column, & outcol, optionHead);
		if ( outcol >= maxlen){
		if ( !  (Flat_Be_quiet&1))
			ErrPost(CTX_NCBI2GB,CTX_2GB_INTERNAL,
				"FlatCont max %ld out %ld OOPS\n",
			(long) maxlen, (long) outcol);
			outcol = maxlen-1;
		}
		retval[outcol+1] = '\0';

	for (; outcol >0 && retval[outcol] == ' '; outcol --){
		retval[outcol] = '\0';
	}

	return retval;
}

/*----------- move_some ()------*/

static void 
move_some (CharPtr to, CharPtr from, Int4 start, Int4 end, Int4 PNTR countPt, AsnOptionPtr optionHead)
{
	Int4 dex;

	for (dex=start; dex < end; dex ++, (*countPt) ++)
		to[ *countPt] = from[dex];

}
/*----------- add_line_break  ()------*/

static void 
add_line_break 
(CharPtr to, Int4 PNTR countPt, int blanks, AsnOptionPtr optionHead)
{
	int dex;
	Boolean killed_line_break = FALSE;
	int cnt=1, ldex;
	
	while ( *countPt > 1 && 
			(to [ *(countPt)-1] == ' '||to [ *(countPt)-1] == '\n')
			){
		if (to [ *(countPt)-1] == '\n'){
			killed_line_break = TRUE;
		}
		(*countPt) --;
	}
	if (killed_line_break){
		cnt = 2;
	}
	for (ldex = 0; ldex < cnt; ldex ++){
		if (ldex){  
/* is blank data of saved blank line */
			to [*countPt] = ' ';
			(*countPt) ++;
		}
		
		to [*countPt] = '\n';
		(*countPt) ++;

		for (dex = 0; dex < blanks; dex ++, (*countPt) ++ )
			to [*countPt] = ' ';
	}

	
}

/*----------- FlatStringParanGroup  ()------*/
CharPtr FlatStringParanGroup 
(CharPtr start, ValNodePtr head, CharPtr start_delimit, CharPtr delimit, CharPtr period, AsnOptionPtr optionHead)
{
	CharPtr retval = NULL, p;
	Int4 len = 0, lenstart = 0, lenst=0, lende = 0, lenper=0;
	ValNodePtr an;

	if (delimit)
		lende = StringLen(delimit);

	if (start_delimit)
		lenst=StringLen(start_delimit);

	if (period)
		lenper=StringLen(period);

	len = lenst + lenper;

	if ( ! lenst && lende)
		len += lende;

	if (start)
		len += (lenstart = StringLen(start))  ;

	for (an = head; an; an = an -> next)
		if (an -> data.ptrvalue)
			len += lende + StringLen( (CharPtr) ( an -> data.ptrvalue));


/*--will be lende too long - who cares --- */

	p = retval = MemNew ( (size_t)(len+1));
	if (start){
		StringCpy (p, start);
		p += lenstart;
		if (head){
			StringCpy(p, start_delimit);
			p += lenst;
		}
	}

	for (an = head; an; an = an -> next)
		if (an -> data.ptrvalue){
			StringCpy(p, (CharPtr) (an -> data.ptrvalue) );
			p += StringLen( (CharPtr) ( an -> data.ptrvalue));
			if (an -> next){
				StringCpy(p, delimit);
				p += lende;
			}
		}

	if (period){
		StringCpy(p, period);
		p += lenper;
	}

	return retval;
}	

/*----------- FlatStringGroup  ()------*/
CharPtr FlatStringGroup 
(CharPtr start, ValNodePtr head, CharPtr delimit, CharPtr period, AsnOptionPtr optionHead)
{
	return FlatStringParanGroup(start , head, delimit, 
		delimit, period, optionHead);
}

/*----------- FlatStringGroupTerm  ()------*/
CharPtr FlatStringGroupTerm 
(CharPtr start, ValNodePtr head, CharPtr delimit, CharPtr period, CharPtr term, AsnOptionPtr optionHead)
{
	CharPtr retval = NULL, p;
	Int4 lenmax, len = 0, lenstart = 0, lende = 0, lente=0, lenper=0;
	ValNodePtr an;

	if (delimit)
		lende = StringLen(delimit);

	if (term)
		lente = StringLen(term);

	lenmax = lende;
	if (lente > lende)
		lenmax = lente;

	if (period)
		lenper=StringLen(period);

	len =  lenmax + lenper;

	if (start)
		len += (lenstart = StringLen(start))  ;

	for (an = head; an; an = an -> next)
		if (an -> data.ptrvalue)
			len += lenmax + StringLen( (CharPtr) ( an -> data.ptrvalue));


/*--will be lende too long - who cares --- */

	p = retval = MemNew ((size_t)(len+1));
	if (start){
		StringCpy (p, start);
		p += lenstart;
		StringCpy(p, delimit);
		p += lende;
	}

	for (an = head; an; an = an -> next)
		if (an -> data.ptrvalue){
			StringCpy(p, (CharPtr) (an -> data.ptrvalue) );
			p += StringLen( (CharPtr) ( an -> data.ptrvalue));
			if (an -> next){
				if ( an -> next -> next){
					StringCpy(p, delimit);
					p += lende;
				}else{
					StringCpy(p, term);
					p += lente;
				}
			}
		}

	if (period){
		StringCpy(p, period);
		p += lenper;
	}

	return retval;
}	

/*------is_not_virtual ()----------*/

Boolean is_not_virtual
(SeqEntryPtr the_seq)
{
	Boolean retval = TRUE;
	BioseqPtr bs;

	if ( the_seq -> choice == 1){
		bs = (BioseqPtr) the_seq -> data.ptrvalue;
		if (bs -> repr == Seq_repr_virtual)
			retval = FALSE;
	}

	return retval;
}

/*-----is_real_id()---*/

Boolean is_real_id
(SeqIdPtr pointIdPtr, SeqIdPtr this_sidp)
{
	Boolean retval = TRUE;
	BioseqPtr bs;
	SeqIdPtr use_id;

		if ( ! SeqIdIn ( pointIdPtr, this_sidp)){
			use_id = pointIdPtr;
			bs = BioseqFind ( use_id);
			if ( bs ){
			if (bs -> repr == Seq_repr_virtual)
				retval = FALSE;
			}
		}
	return retval;
}
	/*--------FlatCleanEquals()----------*/

CharPtr FlatCleanEquals
(CharPtr retval)
{
	CharPtr p=retval;

	if (p){
		for (;*p; p++){
			if (*p == '\"')
				*p = '\'';
		}
	}

return retval;
}
