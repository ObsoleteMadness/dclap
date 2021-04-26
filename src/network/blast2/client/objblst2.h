#ifndef _objblst2_ 
#define _objblst2_ 

#ifdef __cplusplus
extern "C" { /* } */
#endif


/**************************************************
*
*    Generated objects for Module NCBI-BLAST-1
*    Generated using ASNCODE Revision: 1.17 at Jun 28, 1995  2:37 PM
*
**************************************************/

Boolean LIBCALL
objblst2AsnLoad PROTO((void));


/**************************************************
*
*    BLAST0Preface
*
**************************************************/
typedef struct struct_BLAST0_Preface {
   struct struct_BLAST0_Preface PNTR next;
   Uint4 OBbits__;
   CharPtr   program;
   CharPtr   desc;
   CharPtr   version;
   CharPtr   dev_date;
   CharPtr   bld_date;
   ValNodePtr   cit;
   ValNodePtr   notice;
   ValNodePtr   prog_usage;
   struct struct_BLAST0_Seq_usage PNTR   susage;
   struct struct_BLAST0_Seq_usage PNTR   qusage;
} BLAST0Preface, PNTR BLAST0PrefacePtr;


BLAST0PrefacePtr LIBCALL BLAST0PrefaceFree PROTO ((BLAST0PrefacePtr ));
BLAST0PrefacePtr LIBCALL BLAST0PrefaceNew PROTO (( void ));
BLAST0PrefacePtr LIBCALL BLAST0PrefaceAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0PrefaceAsnWrite PROTO (( BLAST0PrefacePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0JobDesc
*
**************************************************/
typedef struct struct_BLAST0_Job_desc {
   Uint4 OBbits__;
   Uint2   jid;
   /* following #defines are for enumerated type, not used by object loaders */
#define BLAST0_Job_desc_jid_not_set 0
#define BLAST0_Job_desc_jid_neighborhood 1
#define BLAST0_Job_desc_jid_search 2
#define BLAST0_Job_desc_jid_threecomps 3

   CharPtr   desc;
   Int4   size;
} BLAST0JobDesc, PNTR BLAST0JobDescPtr;


BLAST0JobDescPtr LIBCALL BLAST0JobDescFree PROTO ((BLAST0JobDescPtr ));
BLAST0JobDescPtr LIBCALL BLAST0JobDescNew PROTO (( void ));
BLAST0JobDescPtr LIBCALL BLAST0JobDescAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0JobDescAsnWrite PROTO (( BLAST0JobDescPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0JobProgress
*
**************************************************/
typedef struct struct_BLAST0_Job_progress {
   Uint4 OBbits__;
   Int4   done;
   Int4   positives;
} BLAST0JobProgress, PNTR BLAST0JobProgressPtr;


BLAST0JobProgressPtr LIBCALL BLAST0JobProgressFree PROTO ((BLAST0JobProgressPtr ));
BLAST0JobProgressPtr LIBCALL BLAST0JobProgressNew PROTO (( void ));
BLAST0JobProgressPtr LIBCALL BLAST0JobProgressAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0JobProgressAsnWrite PROTO (( BLAST0JobProgressPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0Sequence
*
**************************************************/
typedef struct struct_BLAST0_Sequence {
   struct struct_BLAST0_Sequence PNTR next;
   Uint4 OBbits__;
   struct struct_BLAST0_Seq_desc PNTR   desc;
   Int4   length;
#define OB__BLAST0_Sequence_gcode 0

   Int4   gcode;
   ValNodePtr   seq;
} BLAST0Sequence, PNTR BLAST0SequencePtr;


BLAST0SequencePtr LIBCALL BLAST0SequenceFree PROTO ((BLAST0SequencePtr ));
BLAST0SequencePtr LIBCALL BLAST0SequenceNew PROTO (( void ));
BLAST0SequencePtr LIBCALL BLAST0SequenceAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0SequenceAsnWrite PROTO (( BLAST0SequencePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0KABlk
*
**************************************************/
typedef struct struct_BLAST0_KA_Blk {
   struct struct_BLAST0_KA_Blk PNTR next;
   Uint4 OBbits__;
   Int4   matid;
   ValNodePtr   frames;
   FloatHi   lambda;
   FloatHi   k;
   FloatHi   h;
} BLAST0KABlk, PNTR BLAST0KABlkPtr;


BLAST0KABlkPtr LIBCALL BLAST0KABlkFree PROTO ((BLAST0KABlkPtr ));
BLAST0KABlkPtr LIBCALL BLAST0KABlkNew PROTO (( void ));
BLAST0KABlkPtr LIBCALL BLAST0KABlkAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0KABlkAsnWrite PROTO (( BLAST0KABlkPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0DbDesc
*
**************************************************/
typedef struct struct_BLAST0_Db_Desc {
   struct struct_BLAST0_Db_Desc PNTR next;
   Uint4 OBbits__;
   CharPtr   name;
   Uint2   type;
   CharPtr   def;
   CharPtr   rel_date;
   CharPtr   bld_date;
#define OB__BLAST0_Db_Desc_count 0

   Int4   count;
#define OB__BLAST0_Db_Desc_totlen 1

   Int4   totlen;
#define OB__BLAST0_Db_Desc_maxlen 2

   Int4   maxlen;
} BLAST0DbDesc, PNTR BLAST0DbDescPtr;


BLAST0DbDescPtr LIBCALL BLAST0DbDescFree PROTO ((BLAST0DbDescPtr ));
BLAST0DbDescPtr LIBCALL BLAST0DbDescNew PROTO (( void ));
BLAST0DbDescPtr LIBCALL BLAST0DbDescAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0DbDescAsnWrite PROTO (( BLAST0DbDescPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0Result
*
**************************************************/
typedef struct struct_BLAST0_Result {
   Uint4 OBbits__;
   struct struct_BLAST0_Histogram PNTR   hist;
   Int4   count;
   struct struct_BLAST0_HitList PNTR   hitlists;
} BLAST0Result, PNTR BLAST0ResultPtr;


BLAST0ResultPtr LIBCALL BLAST0ResultFree PROTO ((BLAST0ResultPtr ));
BLAST0ResultPtr LIBCALL BLAST0ResultNew PROTO (( void ));
BLAST0ResultPtr LIBCALL BLAST0ResultAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0ResultAsnWrite PROTO (( BLAST0ResultPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0Matrix
*
**************************************************/
typedef struct struct_BLAST0_Matrix {
   struct struct_BLAST0_Matrix PNTR next;
   Uint4 OBbits__;
   Int4   matid;
   CharPtr   name;
   ValNodePtr   comments;
   Uint2   qalpha;
   Uint2   salpha;
   ValNodePtr   Scores_scores;
} BLAST0Matrix, PNTR BLAST0MatrixPtr;


BLAST0MatrixPtr LIBCALL BLAST0MatrixFree PROTO ((BLAST0MatrixPtr ));
BLAST0MatrixPtr LIBCALL BLAST0MatrixNew PROTO (( void ));
BLAST0MatrixPtr LIBCALL BLAST0MatrixAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0MatrixAsnWrite PROTO (( BLAST0MatrixPtr , AsnIoPtr, AsnTypePtr));


#ifdef NLM_GENERATED_CODE_PROTO

typedef ValNodePtr Scores_scoresPtr;
typedef ValNode Scores_scores;

#endif /* NLM_GENERATED_CODE_PROTO */

#define Scores_scores_Scores_ScaledInts 1
#define Scores_scores_reals 2

#ifdef NLM_GENERATED_CODE_PROTO

static Scores_scoresPtr LIBCALL Scores_scoresFree PROTO ((Scores_scoresPtr ));
static Scores_scoresPtr LIBCALL Scores_scoresAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
static Boolean LIBCALL Scores_scoresAsnWrite PROTO (( Scores_scoresPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    Scores_scaled_ints
*
**************************************************/

#ifdef NLM_GENERATED_CODE_PROTO

typedef struct struct_Scores_ScaledInts {
   Uint4 OBbits__;
   FloatHi   scale;
   ValNodePtr   ints;
} Scores_scaled_ints, PNTR Scores_scaled_intsPtr;
#endif /* NLM_GENERATED_CODE_PROTO */

#ifdef NLM_GENERATED_CODE_PROTO

static Scores_scaled_intsPtr LIBCALL Scores_scaled_intsFree PROTO ((Scores_scaled_intsPtr ));
static Scores_scaled_intsPtr LIBCALL Scores_scaled_intsNew PROTO (( void ));
static Scores_scaled_intsPtr LIBCALL Scores_scaled_intsAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
static Boolean LIBCALL Scores_scaled_intsAsnWrite PROTO (( Scores_scaled_intsPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */



/**************************************************
*
*    BLAST0Warning
*
**************************************************/
#define BLAST0Warning  BLAST0Status
#define BLAST0WarningPtr  BLAST0StatusPtr
#define BLAST0WarningFree  BLAST0StatusFree
#define BLAST0WarningNew  BLAST0StatusNew
#define BLAST0WarningAsnRead  BLAST0StatusAsnRead
#define BLAST0WarningAsnWrite  BLAST0StatusAsnWrite


/**************************************************
*
*    BLAST0Status
*
**************************************************/
typedef struct struct_BLAST0_Status {
   Uint4 OBbits__;
   Int4   code;
   CharPtr   reason;
} BLAST0Status, PNTR BLAST0StatusPtr;


BLAST0StatusPtr LIBCALL BLAST0StatusFree PROTO ((BLAST0StatusPtr ));
BLAST0StatusPtr LIBCALL BLAST0StatusNew PROTO (( void ));
BLAST0StatusPtr LIBCALL BLAST0StatusAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0StatusAsnWrite PROTO (( BLAST0StatusPtr , AsnIoPtr, AsnTypePtr));

typedef ValNodePtr BLAST0OutblkPtr;
typedef ValNode BLAST0Outblk;

#ifdef NLM_GENERATED_CODE_PROTO

BLAST0OutblkPtr LIBCALL BLAST0OutblkFree PROTO ((BLAST0OutblkPtr ));
BLAST0OutblkPtr LIBCALL BLAST0OutblkAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0OutblkAsnWrite PROTO (( BLAST0OutblkPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */


#ifdef NLM_GENERATED_CODE_PROTO

typedef ValNodePtr BLAST0Outblk_elementPtr;
typedef ValNode BLAST0Outblk_element;

#endif /* NLM_GENERATED_CODE_PROTO */

#define BLAST0Outblk_preface 1
#define BLAST0Outblk_query 2
#define BLAST0Outblk_dbdesc 3
#define BLAST0Outblk_matrix 4
#define BLAST0Outblk_kablk 5
#define BLAST0Outblk_job_start 6
#define BLAST0Outblk_job_progress 7
#define BLAST0Outblk_job_done 8
#define BLAST0Outblk_result 9
#define BLAST0Outblk_parms 10
#define BLAST0Outblk_stats 11
#define BLAST0Outblk_warning 12
#define BLAST0Outblk_status 13

#ifdef NLM_GENERATED_CODE_PROTO

BLAST0Outblk_elementPtr LIBCALL BLAST0Outblk_elementFree PROTO ((BLAST0Outblk_elementPtr ));
BLAST0Outblk_elementPtr LIBCALL BLAST0Outblk_elementAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0Outblk_elementAsnWrite PROTO (( BLAST0Outblk_elementPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */

typedef ValNodePtr BLAST0RequestPtr;
typedef ValNode BLAST0Request;
#define BLAST0Request_hello 1
#define BLAST0Request_motd 2
#define BLAST0Request_prog_info 3
#define BLAST0Request_usage_info 4
#define BLAST0Request_db_info 5
#define BLAST0Request_matrix_info 6
#define BLAST0Request_matrix_get 7
#define BLAST0Request_search 8
#define BLAST0Request_goodbye 9


BLAST0RequestPtr LIBCALL BLAST0RequestFree PROTO ((BLAST0RequestPtr ));
BLAST0RequestPtr LIBCALL BLAST0RequestAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0RequestAsnWrite PROTO (( BLAST0RequestPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0Search
*
**************************************************/
typedef struct struct_BLAST0_Search {
   Uint4 OBbits__;
   CharPtr   program;
   CharPtr   database;
   struct struct_BLAST0_Sequence PNTR   query;
   ValNodePtr   options;
   Uint1   return_matrix;
   Uint1   return_query;
   Uint1   return_BLAST0result;
   Uint1   return_query_seq_in_seg;
   Uint1   return_db_seq_in_seg;
} BLAST0Search, PNTR BLAST0SearchPtr;


BLAST0SearchPtr LIBCALL BLAST0SearchFree PROTO ((BLAST0SearchPtr ));
BLAST0SearchPtr LIBCALL BLAST0SearchNew PROTO (( void ));
BLAST0SearchPtr LIBCALL BLAST0SearchAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0SearchAsnWrite PROTO (( BLAST0SearchPtr , AsnIoPtr, AsnTypePtr));

typedef ValNodePtr BLAST0ResponsePtr;
typedef ValNode BLAST0Response;
#define BLAST0Response_hello 1
#define BLAST0Response_motd 2
#define BLAST0Response_prog_info 3
#define BLAST0Response_usage_info 4
#define BLAST0Response_db_info 5
#define BLAST0Response_matrix_info 6
#define BLAST0Response_ack 7
#define BLAST0Response_goodbye 8
#define BLAST0Response_queued 9
#define BLAST0Response_preface 10
#define BLAST0Response_query 11
#define BLAST0Response_dbdesc 12
#define BLAST0Response_matrix 13
#define BLAST0Response_kablk 14
#define BLAST0Response_job_start 15
#define BLAST0Response_job_progress 16
#define BLAST0Response_job_done 17
#define BLAST0Response_score_defs 18
#define BLAST0Response_result 19
#define BLAST0Response_seqalign 20
#define BLAST0Response_parms 21
#define BLAST0Response_stats 22
#define BLAST0Response_warning 23
#define BLAST0Response_status 24


BLAST0ResponsePtr LIBCALL BLAST0ResponseFree PROTO ((BLAST0ResponsePtr ));
BLAST0ResponsePtr LIBCALL BLAST0ResponseAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0ResponseAsnWrite PROTO (( BLAST0ResponsePtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0Ack
*
**************************************************/
typedef struct struct_BLAST0_Ack {
   Uint4 OBbits__;
   Int4   code;
   CharPtr   reason;
} BLAST0Ack, PNTR BLAST0AckPtr;


BLAST0AckPtr LIBCALL BLAST0AckFree PROTO ((BLAST0AckPtr ));
BLAST0AckPtr LIBCALL BLAST0AckNew PROTO (( void ));
BLAST0AckPtr LIBCALL BLAST0AckAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0AckAsnWrite PROTO (( BLAST0AckPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0Queued
*
**************************************************/
typedef struct struct_BLAST0_Queued {
   Uint4 OBbits__;
   CharPtr   name;
   Int4   length;
} BLAST0Queued, PNTR BLAST0QueuedPtr;


BLAST0QueuedPtr LIBCALL BLAST0QueuedFree PROTO ((BLAST0QueuedPtr ));
BLAST0QueuedPtr LIBCALL BLAST0QueuedNew PROTO (( void ));
BLAST0QueuedPtr LIBCALL BLAST0QueuedAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0QueuedAsnWrite PROTO (( BLAST0QueuedPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0ScoreInfo
*
**************************************************/
typedef struct struct_BLAST0_Score_Info {
   struct struct_BLAST0_Score_Info PNTR next;
   Uint4 OBbits__;
   Int4   sid;
   CharPtr   tag;
   CharPtr   desc;
} BLAST0ScoreInfo, PNTR BLAST0ScoreInfoPtr;


BLAST0ScoreInfoPtr LIBCALL BLAST0ScoreInfoFree PROTO ((BLAST0ScoreInfoPtr ));
BLAST0ScoreInfoPtr LIBCALL BLAST0ScoreInfoNew PROTO (( void ));
BLAST0ScoreInfoPtr LIBCALL BLAST0ScoreInfoAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0ScoreInfoAsnWrite PROTO (( BLAST0ScoreInfoPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0SeqUsage
*
**************************************************/
typedef struct struct_BLAST0_Seq_usage {
   Uint4 OBbits__;
   Uint2   raw;
   Uint2   cooked;
} BLAST0SeqUsage, PNTR BLAST0SeqUsagePtr;


BLAST0SeqUsagePtr LIBCALL BLAST0SeqUsageFree PROTO ((BLAST0SeqUsagePtr ));
BLAST0SeqUsagePtr LIBCALL BLAST0SeqUsageNew PROTO (( void ));
BLAST0SeqUsagePtr LIBCALL BLAST0SeqUsageAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0SeqUsageAsnWrite PROTO (( BLAST0SeqUsagePtr , AsnIoPtr, AsnTypePtr));

/* following #defines are for enumerated type, not used by object loaders */
#define BLAST0_Alphatype_not_set 0
#define BLAST0_Alphatype_amino_acid 1
#define BLAST0_Alphatype_nucleic_acid 2
#define BLAST0_Alphatype_other 255



/**************************************************
*
*    BLAST0Histogram
*
**************************************************/
typedef struct struct_BLAST0_Histogram {
   Uint4 OBbits__;
   FloatHi   expect;
   Int4   observed;
   Int4   base;
   Int4   nbars;
   struct struct_BLAST0_Histogram_bar PNTR   bar;
} BLAST0Histogram, PNTR BLAST0HistogramPtr;


BLAST0HistogramPtr LIBCALL BLAST0HistogramFree PROTO ((BLAST0HistogramPtr ));
BLAST0HistogramPtr LIBCALL BLAST0HistogramNew PROTO (( void ));
BLAST0HistogramPtr LIBCALL BLAST0HistogramAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0HistogramAsnWrite PROTO (( BLAST0HistogramPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0HitList
*
**************************************************/
typedef struct struct_BLAST0_HitList {
   struct struct_BLAST0_HitList PNTR next;
   Uint4 OBbits__;
   Int4   count;
   struct struct_BLAST0_KA_Blk PNTR   kablk;
   struct struct_BLAST0_HSP PNTR   hsps;
   struct struct_BLAST0_Sequence PNTR   seqs;
} BLAST0HitList, PNTR BLAST0HitListPtr;


BLAST0HitListPtr LIBCALL BLAST0HitListFree PROTO ((BLAST0HitListPtr ));
BLAST0HitListPtr LIBCALL BLAST0HitListNew PROTO (( void ));
BLAST0HitListPtr LIBCALL BLAST0HitListAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0HitListAsnWrite PROTO (( BLAST0HitListPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0HistogramBar
*
**************************************************/
typedef struct struct_BLAST0_Histogram_bar {
   struct struct_BLAST0_Histogram_bar PNTR next;
   Uint4 OBbits__;
   FloatHi   x;
   Int4   n;
} BLAST0HistogramBar, PNTR BLAST0HistogramBarPtr;


BLAST0HistogramBarPtr LIBCALL BLAST0HistogramBarFree PROTO ((BLAST0HistogramBarPtr ));
BLAST0HistogramBarPtr LIBCALL BLAST0HistogramBarNew PROTO (( void ));
BLAST0HistogramBarPtr LIBCALL BLAST0HistogramBarAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0HistogramBarAsnWrite PROTO (( BLAST0HistogramBarPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0HSP
*
**************************************************/
typedef struct struct_BLAST0_HSP {
   struct struct_BLAST0_HSP PNTR next;
   Uint4 OBbits__;
   Int4   matid;
   struct struct_Score PNTR   scores;
   Int4   len;
   struct struct_BLAST0_Segment PNTR   segs;
} BLAST0HSP, PNTR BLAST0HSPPtr;


BLAST0HSPPtr LIBCALL BLAST0HSPFree PROTO ((BLAST0HSPPtr ));
BLAST0HSPPtr LIBCALL BLAST0HSPNew PROTO (( void ));
BLAST0HSPPtr LIBCALL BLAST0HSPAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0HSPAsnWrite PROTO (( BLAST0HSPPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0Segment
*
**************************************************/
typedef struct struct_BLAST0_Segment {
   struct struct_BLAST0_Segment PNTR next;
   Uint4 OBbits__;
   struct struct_BLAST0_Seq_interval PNTR   loc;
   ValNodePtr   str;
   ValNodePtr   str_raw;
} BLAST0Segment, PNTR BLAST0SegmentPtr;


BLAST0SegmentPtr LIBCALL BLAST0SegmentFree PROTO ((BLAST0SegmentPtr ));
BLAST0SegmentPtr LIBCALL BLAST0SegmentNew PROTO (( void ));
BLAST0SegmentPtr LIBCALL BLAST0SegmentAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0SegmentAsnWrite PROTO (( BLAST0SegmentPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0SeqInterval
*
**************************************************/
typedef struct struct_BLAST0_Seq_interval {
   Uint4 OBbits__;
#define OB__BLAST0_Seq_interval_strand 0

   Uint2   strand;
   /* following #defines are for enumerated type, not used by object loaders */
#define BLAST0_Seq_interval_strand_plus 1
#define BLAST0_Seq_interval_strand_minus 2
#define BLAST0_Seq_interval_strand_both 3
#define BLAST0_Seq_interval_strand_plus_rf 5
#define BLAST0_Seq_interval_strand_minus_rf 6

   Int4   from;
   Int4   to;
} BLAST0SeqInterval, PNTR BLAST0SeqIntervalPtr;


BLAST0SeqIntervalPtr LIBCALL BLAST0SeqIntervalFree PROTO ((BLAST0SeqIntervalPtr ));
BLAST0SeqIntervalPtr LIBCALL BLAST0SeqIntervalNew PROTO (( void ));
BLAST0SeqIntervalPtr LIBCALL BLAST0SeqIntervalAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0SeqIntervalAsnWrite PROTO (( BLAST0SeqIntervalPtr , AsnIoPtr, AsnTypePtr));

typedef ValNodePtr BLAST0SeqDataPtr;
typedef ValNode BLAST0SeqData;
#define BLAST0SeqData_ncbistdaa 1
#define BLAST0SeqData_ncbi2na 2
#define BLAST0SeqData_ncbi4na 3


BLAST0SeqDataPtr LIBCALL BLAST0SeqDataFree PROTO ((BLAST0SeqDataPtr ));
BLAST0SeqDataPtr LIBCALL BLAST0SeqDataAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0SeqDataAsnWrite PROTO (( BLAST0SeqDataPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    BLAST0SeqDesc
*
**************************************************/
typedef struct struct_BLAST0_Seq_desc {
   struct struct_BLAST0_Seq_desc PNTR next;
   Uint4 OBbits__;
   ValNodePtr   id;
   CharPtr   defline;
} BLAST0SeqDesc, PNTR BLAST0SeqDescPtr;


BLAST0SeqDescPtr LIBCALL BLAST0SeqDescFree PROTO ((BLAST0SeqDescPtr ));
BLAST0SeqDescPtr LIBCALL BLAST0SeqDescNew PROTO (( void ));
BLAST0SeqDescPtr LIBCALL BLAST0SeqDescAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0SeqDescAsnWrite PROTO (( BLAST0SeqDescPtr , AsnIoPtr, AsnTypePtr));

typedef ValNodePtr BLAST0SeqIdPtr;
typedef ValNode BLAST0SeqId;

#ifdef NLM_GENERATED_CODE_PROTO

BLAST0SeqIdPtr LIBCALL BLAST0SeqIdFree PROTO ((BLAST0SeqIdPtr ));
BLAST0SeqIdPtr LIBCALL BLAST0SeqIdAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0SeqIdAsnWrite PROTO (( BLAST0SeqIdPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */


#ifdef NLM_GENERATED_CODE_PROTO

typedef ValNodePtr BLAST0SeqId_elementPtr;
typedef ValNode BLAST0SeqId_element;

#endif /* NLM_GENERATED_CODE_PROTO */

#define BLAST0SeqId_giid 1
#define BLAST0SeqId_textid 2

#ifdef NLM_GENERATED_CODE_PROTO

BLAST0SeqId_elementPtr LIBCALL BLAST0SeqId_elementFree PROTO ((BLAST0SeqId_elementPtr ));
BLAST0SeqId_elementPtr LIBCALL BLAST0SeqId_elementAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL BLAST0SeqId_elementAsnWrite PROTO (( BLAST0SeqId_elementPtr , AsnIoPtr, AsnTypePtr));

#endif /* NLM_GENERATED_CODE_PROTO */

/* following #defines are for enumerated type, not used by object loaders */
#define BLAST0_Alpha_ID_ncbi4na 4
#define BLAST0_Alpha_ID_ncbistdaa 11

#ifdef __cplusplus
/* { */ }
#endif

#endif /* _objblst2_ */
