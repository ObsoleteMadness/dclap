/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnneten.l03";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module NCBI-Entrez
*
**************************************************/

#define ENTREZ_REQUEST &at[4]
#define ENTREZ_REQUEST_init &at[5]
#define ENTREZ_REQUEST_init_version &at[6]
#define ENTREZ_REQUEST_maxlinks &at[9]
#define ENTREZ_REQUEST_eval &at[11]
#define ENTREZ_REQUEST_docsum &at[36]
#define ENTREZ_REQUEST_linkuidlist &at[47]
#define ENTREZ_REQUEST_uidlinks &at[56]
#define ENTREZ_REQUEST_byterm &at[57]
#define ENTREZ_REQUEST_bypage &at[63]
#define ENTREZ_REQUEST_findterm &at[69]
#define ENTREZ_REQUEST_fini &at[74]
#define ENTREZ_REQUEST_createnamed &at[75]
#define ENTREZ_REQUEST_getmle &at[81]
#define ENTREZ_REQUEST_getseq &at[82]

#define ENTREZ_TERMGET &at[12]
#define ENTREZ_TERMGET_max &at[13]
#define ENTREZ_TERMGET_cls &at[15]
#define ENTREZ_TERMGET_terms &at[18]

#define ENTREZ_DOCGET &at[37]
#define ENTREZ_DOCGET_class &at[38]
#define ENTREZ_DOCGET_mark_missing &at[39]
#define ENTREZ_DOCGET_ids &at[41]
#define ENTREZ_DOCGET_defer_count &at[46]

#define LINK_SETGET &at[48]
#define LINK_SETGET_max &at[49]
#define LINK_SETGET_link_to_cls &at[50]
#define LINK_SETGET_query_cls &at[51]
#define LINK_SETGET_mark_missing &at[52]
#define LINK_SETGET_query_size &at[53]
#define LINK_SETGET_query &at[54]
#define LINK_SETGET_query_E &at[55]

#define ENTREZ_TERM_BY_TERM &at[58]
#define ENTREZ_TERM_BY_TERM_type &at[59]
#define ENTREZ_TERM_BY_TERM_fld &at[60]
#define ENTREZ_TERM_BY_TERM_term &at[61]
#define ENTREZ_TERM_BY_TERM_num_terms &at[62]

#define ENTREZ_TERM_BY_PAGE &at[64]
#define ENTREZ_TERM_BY_PAGE_type &at[65]
#define ENTREZ_TERM_BY_PAGE_fld &at[66]
#define ENTREZ_TERM_BY_PAGE_page &at[67]
#define ENTREZ_TERM_BY_PAGE_num_pages &at[68]

#define TERM_LOOKUP &at[70]
#define TERM_LOOKUP_type &at[71]
#define TERM_LOOKUP_fld &at[72]
#define TERM_LOOKUP_term &at[73]

#define ENTREZ_NAMED_LIST &at[76]
#define ENTREZ_NAMED_LIST_term &at[77]
#define ENTREZ_NAMED_LIST_type &at[78]
#define ENTREZ_NAMED_LIST_fld &at[79]
#define ENTREZ_NAMED_LIST_uids &at[80]

#define ENTREZ_SEQGET &at[83]
#define ENTREZ_SEQGET_retype &at[84]
#define ENTREZ_SEQGET_mark_missing &at[85]
#define ENTREZ_SEQGET_ids &at[86]

#define ENTREZ_CLASS &at[16]

#define ENTREZ_IDS &at[42]
#define ENTREZ_IDS_numid &at[43]
#define ENTREZ_IDS_ids &at[44]
#define ENTREZ_IDS_ids_E &at[45]

#define ENTREZ_TERM_LIST &at[19]
#define ENTREZ_TERM_LIST_E &at[20]
#define ENTREZ_TERM_LIST_E_operator &at[21]
#define ENTREZ_TERM_LIST_E_sp_operand &at[23]
#define ENTREZ_TERM_LIST_E_tot_operand &at[29]

#define ENTREZ_FIELD &at[27]

#define ENTREZ_OPERATOR &at[22]

#define SPECIAL_OPERAND &at[24]
#define SPECIAL_OPERAND_term &at[25]
#define SPECIAL_OPERAND_fld &at[26]
#define SPECIAL_OPERAND_type &at[28]

#define TOTAL_OPERAND &at[30]
#define TOTAL_OPERAND_term &at[31]
#define TOTAL_OPERAND_fld &at[32]
#define TOTAL_OPERAND_type &at[33]

#define ENTREZ_BACK &at[87]
#define ENTREZ_BACK_error &at[88]
#define ENTREZ_BACK_init &at[89]
#define ENTREZ_BACK_init_e_info &at[90]
#define ENTREZ_BACK_maxlinks &at[91]
#define ENTREZ_BACK_eval &at[92]
#define ENTREZ_BACK_eval_bad_count &at[93]
#define ENTREZ_BACK_eval_link_set &at[94]
#define ENTREZ_BACK_docsum &at[95]
#define ENTREZ_BACK_docsum_ml &at[96]
#define ENTREZ_BACK_docsum_seq &at[108]
#define ENTREZ_BACK_linkuidlist &at[117]
#define ENTREZ_BACK_uidlinks &at[122]
#define ENTREZ_BACK_byterm &at[123]
#define ENTREZ_BACK_bypage &at[134]
#define ENTREZ_BACK_findterm &at[135]
#define ENTREZ_BACK_fini &at[140]
#define ENTREZ_BACK_createnamed &at[141]
#define ENTREZ_BACK_getmle &at[142]
#define ENTREZ_BACK_getseq &at[148]

#define ML_SUMMARY_LIST &at[97]
#define ML_SUMMARY_LIST_num &at[98]
#define ML_SUMMARY_LIST_data &at[99]
#define ML_SUMMARY_LIST_data_E &at[100]

#define SEQ_SUMMARY_LIST &at[109]
#define SEQ_SUMMARY_LIST_num &at[110]
#define SEQ_SUMMARY_LIST_data &at[111]
#define SEQ_SUMMARY_LIST_data_E &at[112]

#define MARKED_LINK_SET &at[118]
#define MARKED_LINK_SET_link_set &at[119]
#define MARKED_LINK_SET_uids_processed &at[120]
#define MARKED_LINK_SET_marked_missing &at[121]

#define ENTREZ_TERM_RESP &at[124]
#define ENTREZ_TERM_RESP_num_terms &at[125]
#define ENTREZ_TERM_RESP_first_page &at[126]
#define ENTREZ_TERM_RESP_pages_read &at[127]
#define ENTREZ_TERM_RESP_info &at[128]
#define ENTREZ_TERM_RESP_info_E &at[129]

#define TERM_COUNTS &at[136]
#define TERM_COUNTS_found &at[137]
#define TERM_COUNTS_spec_count &at[138]
#define TERM_COUNTS_tot_count &at[139]

#define ENTREZ_MEDLINE_ENTRY_LIST &at[143]
#define ENTREZ_MEDLINE_ENTRY_LIST_num &at[144]
#define ENTREZ_MEDLINE_ENTRY_LIST_data &at[145]
#define MEDLINE_ENTRY_LIST_data_E &at[146]
#define ENTRY_LIST_marked_missing &at[147]

#define ENTREZ_SEQ_ENTRY_LIST &at[149]
#define ENTREZ_SEQ_ENTRY_LIST_num &at[150]
#define ENTREZ_SEQ_ENTRY_LIST_data &at[151]
#define ENTREZ_SEQ_ENTRY_LIST_data_E &at[152]
#define SEQ_ENTRY_LIST_marked_missing &at[153]

#define ML_SUMMARY &at[101]
#define ML_SUMMARY_muid &at[102]
#define ML_SUMMARY_no_abstract &at[103]
#define ML_SUMMARY_translated_title &at[104]
#define ML_SUMMARY_no_authors &at[105]
#define ML_SUMMARY_caption &at[106]
#define ML_SUMMARY_title &at[107]

#define SEQ_SUMMARY &at[113]
#define SEQ_SUMMARY_id &at[114]
#define SEQ_SUMMARY_caption &at[115]
#define SEQ_SUMMARY_title &at[116]

#define TERM_PAGE_INFO &at[130]
#define TERM_PAGE_INFO_term &at[131]
#define TERM_PAGE_INFO_spec_count &at[132]
#define TERM_PAGE_INFO_tot_count &at[133]
