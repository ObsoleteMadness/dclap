/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnalign.l01";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module NCBI-Seqalign
*
**************************************************/

#define SEQ_ALIGN &at[0]
#define SEQ_ALIGN_type &at[1]
#define SEQ_ALIGN_dim &at[3]
#define SEQ_ALIGN_score &at[5]
#define SEQ_ALIGN_score_E &at[6]
#define SEQ_ALIGN_segs &at[17]
#define SEQ_ALIGN_segs_dendiag &at[18]
#define SEQ_ALIGN_segs_dendiag_E &at[19]
#define SEQ_ALIGN_segs_denseg &at[34]
#define SEQ_ALIGN_segs_std &at[48]
#define SEQ_ALIGN_segs_std_E &at[49]

#define SCORE &at[7]
#define SCORE_id &at[8]
#define SCORE_value &at[10]
#define SCORE_value_real &at[11]
#define SCORE_value_int &at[13]

#define DENSE_DIAG &at[20]
#define DENSE_DIAG_dim &at[21]
#define DENSE_DIAG_ids &at[22]
#define DENSE_DIAG_ids_E &at[23]
#define DENSE_DIAG_starts &at[26]
#define DENSE_DIAG_starts_E &at[27]
#define DENSE_DIAG_len &at[28]
#define DENSE_DIAG_strands &at[29]
#define DENSE_DIAG_strands_E &at[30]
#define DENSE_DIAG_scores &at[32]
#define DENSE_DIAG_scores_E &at[33]

#define DENSE_SEG &at[35]
#define DENSE_SEG_dim &at[36]
#define DENSE_SEG_numseg &at[37]
#define DENSE_SEG_ids &at[38]
#define DENSE_SEG_ids_E &at[39]
#define DENSE_SEG_starts &at[40]
#define DENSE_SEG_starts_E &at[41]
#define DENSE_SEG_lens &at[42]
#define DENSE_SEG_lens_E &at[43]
#define DENSE_SEG_strands &at[44]
#define DENSE_SEG_strands_E &at[45]
#define DENSE_SEG_scores &at[46]
#define DENSE_SEG_scores_E &at[47]

#define STD_SEG &at[50]
#define STD_SEG_dim &at[51]
#define STD_SEG_ids &at[52]
#define STD_SEG_ids_E &at[53]
#define STD_SEG_loc &at[54]
#define STD_SEG_loc_E &at[55]
#define STD_SEG_scores &at[57]
#define STD_SEG_scores_E &at[58]
