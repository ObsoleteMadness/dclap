/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "blast1.l01";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module NCBI-BLAST0
*
**************************************************/

#define BLAST0_REQUEST &at[0]
#define BLAST0_REQUEST_hello &at[1]
#define BLAST0_REQUEST_dblist &at[3]
#define BLAST0_REQUEST_proglist &at[4]
#define BLAST0_REQUEST_search &at[5]
#define BLAST0_REQUEST_goodbye &at[56]

#define BLAST0_SEARCH &at[6]
#define BLAST0_SEARCH_program &at[7]
#define BLAST0_SEARCH_database &at[9]
#define BLAST0_SEARCH_query &at[10]
#define BLAST0_SEARCH_mask &at[37]
#define BLAST0_SEARCH_control &at[42]
#define BLAST0_SEARCH_options &at[54]
#define BLAST0_SEARCH_options_E &at[55]

#define BLAST0_RESPONSE &at[57]
#define BLAST0_RESPONSE_hello &at[58]
#define BLAST0_RESPONSE_down &at[59]
#define BLAST0_RESPONSE_dblist &at[60]
#define BLAST0_RESPONSE_dblist_E &at[61]
#define BLAST0_RESPONSE_proglist &at[71]
#define BLAST0_RESPONSE_proglist_E &at[72]
#define BLAST0_RESPONSE_queued &at[76]
#define BLAST0_RESPONSE_queued_E &at[77]
#define BLAST0_RESPONSE_outblk &at[81]
#define BLAST0_RESPONSE_goodbye &at[162]

#define BLAST0_DBDESC &at[62]
#define BLAST0_DBDESC_name &at[63]
#define BLAST0_DBDESC_type &at[64]
#define BLAST0_DBDESC_def &at[66]
#define BLAST0_DBDESC_dev_date &at[67]
#define BLAST0_DBDESC_bld_date &at[68]
#define BLAST0_DBDESC_count &at[69]
#define BLAST0_DBDESC_length &at[70]

#define BLAST0_PROGDESC &at[73]
#define BLAST0_PROGDESC_program &at[74]
#define BLAST0_PROGDESC_desc &at[75]

#define BLAST0_QUEUED &at[78]
#define BLAST0_QUEUED_desc &at[79]
#define BLAST0_QUEUED_length &at[80]

#define BLAST0_OUTBLK &at[82]
#define BLAST0_OUTBLK_E &at[83]
#define BLAST0_OUTBLK_E_preface &at[84]
#define BLAST0_OUTBLK_E_query_ack &at[94]
#define BLAST0_OUTBLK_E_warning &at[98]
#define BLAST0_OUTBLK_E_kablk &at[102]
#define BLAST0_OUTBLK_E_kablk_E &at[103]
#define BLAST0_OUTBLK_E_dbdesc &at[112]
#define BLAST0_OUTBLK_E_job_start &at[113]
#define BLAST0_OUTBLK_E_job_progress &at[119]
#define BLAST0_OUTBLK_E_job_done &at[123]
#define BLAST0_OUTBLK_E_result &at[124]
#define BLAST0_OUTBLK_E_status &at[158]

#define BLAST0_SEQUENCE &at[11]
#define BLAST0_SEQUENCE_desc &at[12]
#define BLAST0_SEQUENCE_desc_E &at[13]
#define BLAST0_SEQUENCE_length &at[26]
#define BLAST0_SEQUENCE_attrib &at[27]
#define BLAST0_SEQUENCE_attrib_E &at[28]
#define BLAST0_SEQUENCE_seq &at[31]

#define BLAST0_SEQ_INTERVAL &at[38]
#define BLAST0_SEQ_INTERVAL_strand &at[39]
#define BLAST0_SEQ_INTERVAL_from &at[40]
#define BLAST0_SEQ_INTERVAL_to &at[41]

#define BLAST0_CONTROL &at[43]
#define BLAST0_CONTROL_E &at[44]
#define BLAST0_CONTROL_E_ids &at[45]
#define BLAST0_CONTROL_E_defs &at[46]
#define BLAST0_CONTROL_E_q_def &at[47]
#define BLAST0_CONTROL_E_q_seg &at[49]
#define BLAST0_CONTROL_E_q_seg_nox &at[50]
#define BLAST0_CONTROL_E_t_def &at[51]
#define BLAST0_CONTROL_E_t_seg &at[52]
#define BLAST0_CONTROL_E_t_seg_nox &at[53]

#define BLAST0_PREFACE &at[85]
#define BLAST0_PREFACE_program &at[86]
#define BLAST0_PREFACE_version &at[87]
#define BLAST0_PREFACE_dev_date &at[88]
#define BLAST0_PREFACE_bld_date &at[89]
#define BLAST0_PREFACE_cit &at[90]
#define BLAST0_PREFACE_cit_E &at[91]
#define BLAST0_PREFACE_notice &at[92]
#define BLAST0_PREFACE_notice_E &at[93]

#define BLAST0_QUERY_ACK &at[95]
#define BLAST0_QUERY_ACK_name &at[96]
#define BLAST0_QUERY_ACK_len &at[97]

#define BLAST0_WARNING &at[99]
#define BLAST0_WARNING_code &at[100]
#define BLAST0_WARNING_reason &at[101]

#define KA_BLK &at[104]
#define KA_BLK_matid &at[105]
#define KA_BLK_q_frame &at[106]
#define KA_BLK_t_frame &at[107]
#define KA_BLK_lambda &at[108]
#define KA_BLK_k &at[110]
#define KA_BLK_h &at[111]

#define JOB_DESC &at[114]
#define JOB_DESC_desc &at[115]
#define JOB_DESC_units &at[116]
#define JOB_DESC_size &at[118]

#define JOB_PROGRESS &at[120]
#define JOB_PROGRESS_done &at[121]
#define JOB_PROGRESS_positives &at[122]

#define BLAST0_RESULT &at[125]
#define BLAST0_RESULT_parms &at[126]
#define BLAST0_RESULT_parms_E &at[127]
#define BLAST0_RESULT_stats &at[128]
#define BLAST0_RESULT_stats_E &at[129]
#define BLAST0_RESULT_hits &at[130]

#define BLAST0_STATUS &at[159]
#define BLAST0_STATUS_error &at[160]
#define BLAST0_STATUS_reason &at[161]

#define BLAST0_ALPHATYPE &at[65]

#define JOB_UNITS &at[117]

#define HITDATA &at[131]
#define HITDATA_count &at[132]
#define HITDATA_dim &at[133]
#define HITDATA_hitlists &at[134]
#define HITDATA_hitlists_E &at[135]

#define HITLIST &at[136]
#define HITLIST_count &at[137]
#define HITLIST_hsps &at[138]
#define HITLIST_hsps_E &at[139]
#define HITLIST_seqs &at[156]
#define HITLIST_seqs_E &at[157]

#define BLAST0_HSP &at[140]
#define BLAST0_HSP_matid &at[141]
#define BLAST0_HSP_scores &at[142]
#define BLAST0_HSP_scores_E &at[143]
#define BLAST0_HSP_len &at[149]
#define BLAST0_HSP_segs &at[150]
#define BLAST0_HSP_segs_E &at[151]

#define BLAST0_SCORE &at[144]
#define BLAST0_SCORE_type &at[145]
#define BLAST0_SCORE_value &at[146]
#define BLAST0_SCORE_value_i &at[147]
#define BLAST0_SCORE_value_r &at[148]

#define BLAST0_SEGMENT &at[152]
#define BLAST0_SEGMENT_loc &at[153]
#define BLAST0_SEGMENT_str &at[154]
#define BLAST0_SEGMENT_str_nox &at[155]

#define BLAST0_SEQ_DATA &at[32]
#define BLAST0_SEQ_DATA_ncbistdaa &at[33]
#define BLAST0_SEQ_DATA_ncbi2na &at[35]
#define BLAST0_SEQ_DATA_ncbi4na &at[36]

#define SEQ_DESC &at[14]
#define SEQ_DESC_id &at[15]
#define SEQ_DESC_defline &at[23]

#define BLAST0_ATTRIB &at[29]

#define BLAST0_SEQ_ID &at[16]
#define BLAST0_SEQ_ID_E &at[17]
#define BLAST0_SEQ_ID_E_giid &at[18]
#define BLAST0_SEQ_ID_E_textid &at[20]

#define HSP_MEASURE &at[163]

#define BLAST0_MATRIX &at[164]
#define BLAST0_MATRIX_name &at[165]
#define BLAST0_MATRIX_type &at[166]
#define BLAST0_MATRIX_comments &at[167]
#define BLAST0_MATRIX_comments_E &at[168]
#define BLAST0_MATRIX_rowletters &at[169]
#define BLAST0_MATRIX_rowletters_E &at[170]
#define BLAST0_MATRIX_colletters &at[171]
#define BLAST0_MATRIX_colletters_E &at[172]
#define BLAST0_MATRIX_scores &at[173]
#define BLAST0_MATRIX_scores_E &at[174]

#define BLAST0_DATABASE &at[175]
#define BLAST0_DATABASE_desc &at[176]
#define BLAST0_DATABASE_seqs &at[177]
#define BLAST0_DATABASE_seqs_E &at[178]
