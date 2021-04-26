/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnbibli.l01";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module NCBI-Biblio
*
**************************************************/

#define CIT_ART &at[0]
#define CIT_ART_title &at[1]
#define CIT_ART_authors &at[17]
#define CIT_ART_from &at[47]
#define CIT_ART_from_journal &at[48]
#define CIT_ART_from_book &at[64]
#define CIT_ART_from_proc &at[70]

#define CIT_JOUR &at[49]
#define CIT_JOUR_title &at[50]
#define CIT_JOUR_imp &at[51]

#define CIT_BOOK &at[65]
#define CIT_BOOK_title &at[66]
#define CIT_BOOK_coll &at[67]
#define CIT_BOOK_authors &at[68]
#define CIT_BOOK_imp &at[69]

#define CIT_PAT &at[78]
#define CIT_PAT_title &at[79]
#define CIT_PAT_authors &at[80]
#define CIT_PAT_country &at[81]
#define CIT_PAT_doc_type &at[82]
#define CIT_PAT_number &at[83]
#define CIT_PAT_date_issue &at[84]
#define CIT_PAT_class &at[85]
#define CIT_PAT_app_number &at[86]
#define CIT_PAT_app_date &at[87]

#define CIT_LET &at[88]
#define CIT_LET_cit &at[89]
#define CIT_LET_man_id &at[90]
#define CIT_LET_type &at[91]

#define ID_PAT &at[92]
#define ID_PAT_country &at[93]
#define ID_PAT_id &at[94]
#define ID_PAT_id_number &at[95]
#define ID_PAT_id_app_number &at[96]

#define CIT_GEN &at[97]
#define CIT_GEN_cit &at[98]
#define CIT_GEN_authors &at[99]
#define CIT_GEN_muid &at[100]
#define CIT_GEN_journal &at[102]
#define CIT_GEN_volume &at[103]
#define CIT_GEN_issue &at[104]
#define CIT_GEN_pages &at[105]
#define CIT_GEN_date &at[106]
#define CIT_GEN_serial_number &at[107]
#define CIT_GEN_title &at[108]

#define CIT_PROC &at[71]
#define CIT_PROC_book &at[72]
#define CIT_PROC_meet &at[73]

#define CIT_SUB &at[109]
#define CIT_SUB_authors &at[110]
#define CIT_SUB_imp &at[111]
#define CIT_SUB_medium &at[112]

#define TITLE &at[2]
#define TITLE_E &at[3]
#define TITLE_E_name &at[4]
#define TITLE_E_tsub &at[6]
#define TITLE_E_trans &at[7]
#define TITLE_E_jta &at[8]
#define TITLE_E_iso_jta &at[9]
#define TITLE_E_ml_jta &at[10]
#define TITLE_E_coden &at[11]
#define TITLE_E_issn &at[12]
#define TITLE_E_abr &at[13]
#define TITLE_E_isbn &at[14]

#define AUTH_LIST &at[18]
#define AUTH_LIST_names &at[19]
#define AUTH_LIST_names_std &at[20]
#define AUTH_LIST_names_std_E &at[21]
#define AUTH_LIST_names_ml &at[42]
#define AUTH_LIST_names_ml_E &at[43]
#define AUTH_LIST_names_str &at[44]
#define AUTH_LIST_names_str_E &at[45]
#define AUTH_LIST_affil &at[46]

#define IMPRINT &at[52]
#define IMPRINT_date &at[53]
#define IMPRINT_volume &at[55]
#define IMPRINT_issue &at[56]
#define IMPRINT_pages &at[57]
#define IMPRINT_section &at[58]
#define IMPRINT_pub &at[59]
#define IMPRINT_cprt &at[60]
#define IMPRINT_part_sup &at[61]
#define IMPRINT_language &at[62]
#define IMPRINT_prepub &at[63]

#define MEETING &at[74]
#define MEETING_number &at[75]
#define MEETING_date &at[76]
#define MEETING_place &at[77]

#define AUTHOR &at[22]
#define AUTHOR_name &at[23]
#define AUTHOR_level &at[25]
#define AUTHOR_role &at[27]
#define AUTHOR_affil &at[28]
#define AUTHOR_is_corr &at[39]

#define AFFIL &at[29]
#define AFFIL_str &at[30]
#define AFFIL_std &at[31]
#define AFFIL_std_affil &at[32]
#define AFFIL_std_div &at[33]
#define AFFIL_std_city &at[34]
#define AFFIL_std_sub &at[35]
#define AFFIL_std_country &at[36]
#define AFFIL_std_street &at[37]
