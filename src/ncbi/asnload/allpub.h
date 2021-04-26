/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "allpub.l00";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module NCBI-General
*
**************************************************/

#define DATE &at[0]
#define DATE_str &at[1]
#define DATE_std &at[3]

#define PERSON_ID &at[12]
#define PERSON_ID_dbtag &at[13]
#define PERSON_ID_name &at[20]
#define PERSON_ID_ml &at[29]
#define PERSON_ID_str &at[30]

#define OBJECT_ID &at[17]
#define OBJECT_ID_id &at[18]
#define OBJECT_ID_str &at[19]

#define DBTAG &at[14]
#define DBTAG_db &at[15]
#define DBTAG_tag &at[16]

#define INT_FUZZ &at[31]
#define INT_FUZZ_p_m &at[32]
#define INT_FUZZ_range &at[33]
#define INT_FUZZ_range_max &at[34]
#define INT_FUZZ_range_min &at[35]
#define INT_FUZZ_pct &at[36]
#define INT_FUZZ_lim &at[37]

#define USER_OBJECT &at[39]
#define USER_OBJECT_class &at[40]
#define USER_OBJECT_type &at[41]
#define USER_OBJECT_data &at[42]
#define USER_OBJECT_data_E &at[43]

#define DATE_STD &at[4]
#define DATE_STD_year &at[5]
#define DATE_STD_month &at[7]
#define DATE_STD_day &at[8]
#define DATE_STD_season &at[9]

#define NAME_STD &at[21]
#define NAME_STD_last &at[22]
#define NAME_STD_first &at[23]
#define NAME_STD_middle &at[24]
#define NAME_STD_full &at[25]
#define NAME_STD_initials &at[26]
#define NAME_STD_suffix &at[27]
#define NAME_STD_title &at[28]

#define USER_FIELD &at[44]
#define USER_FIELD_label &at[45]
#define USER_FIELD_num &at[46]
#define USER_FIELD_data &at[47]
#define USER_FIELD_data_str &at[48]
#define USER_FIELD_data_int &at[49]
#define USER_FIELD_data_real &at[50]
#define USER_FIELD_data_bool &at[52]
#define USER_FIELD_data_os &at[54]
#define USER_FIELD_data_object &at[56]
#define USER_FIELD_data_strs &at[57]
#define USER_FIELD_data_strs_E &at[58]
#define USER_FIELD_data_ints &at[60]
#define USER_FIELD_data_ints_E &at[61]
#define USER_FIELD_data_reals &at[62]
#define USER_FIELD_data_reals_E &at[63]
#define USER_FIELD_data_oss &at[64]
#define USER_FIELD_data_oss_E &at[65]
#define USER_FIELD_data_fields &at[66]
#define USER_FIELD_data_fields_E &at[67]
#define USER_FIELD_data_objects &at[68]
#define USER_FIELD_data_objects_E &at[69]


/**************************************************
*
*    Defines for Module NCBI-Pub
*
**************************************************/

#define PUB &at[70]
#define PUB_gen &at[71]
#define PUB_sub &at[123]
#define PUB_medline &at[140]
#define PUB_muid &at[196]
#define PUB_article &at[197]
#define PUB_journal &at[199]
#define PUB_book &at[201]
#define PUB_proc &at[203]
#define PUB_patent &at[205]
#define PUB_pat_id &at[217]
#define PUB_man &at[224]
#define PUB_equiv &at[230]

#define PUB_SET &at[233]
#define PUB_SET_pub &at[234]
#define PUB_SET_pub_E &at[235]
#define PUB_SET_medline &at[236]
#define PUB_SET_medline_E &at[237]
#define PUB_SET_article &at[238]
#define PUB_SET_article_E &at[239]
#define PUB_SET_journal &at[240]
#define PUB_SET_journal_E &at[241]
#define PUB_SET_book &at[242]
#define PUB_SET_book_E &at[243]
#define PUB_SET_proc &at[244]
#define PUB_SET_proc_E &at[245]
#define PUB_SET_patent &at[246]
#define PUB_SET_patent_E &at[247]

#define PUB_EQUIV &at[231]
#define PUB_EQUIV_E &at[232]


/**************************************************
*
*    Defines for Module NCBI-Biblio
*
**************************************************/

#define CIT_ART &at[148]
#define CIT_ART_title &at[149]
#define CIT_ART_authors &at[150]
#define CIT_ART_from &at[151]
#define CIT_ART_from_journal &at[152]
#define CIT_ART_from_book &at[156]
#define CIT_ART_from_proc &at[162]

#define CIT_JOUR &at[153]
#define CIT_JOUR_title &at[154]
#define CIT_JOUR_imp &at[155]

#define CIT_BOOK &at[157]
#define CIT_BOOK_title &at[158]
#define CIT_BOOK_coll &at[159]
#define CIT_BOOK_authors &at[160]
#define CIT_BOOK_imp &at[161]

#define CIT_PAT &at[207]
#define CIT_PAT_title &at[208]
#define CIT_PAT_authors &at[209]
#define CIT_PAT_country &at[210]
#define CIT_PAT_doc_type &at[211]
#define CIT_PAT_number &at[212]
#define CIT_PAT_date_issue &at[213]
#define CIT_PAT_class &at[214]
#define CIT_PAT_app_number &at[215]
#define CIT_PAT_app_date &at[216]

#define CIT_LET &at[226]
#define CIT_LET_cit &at[227]
#define CIT_LET_man_id &at[228]
#define CIT_LET_type &at[229]

#define ID_PAT &at[219]
#define ID_PAT_country &at[220]
#define ID_PAT_id &at[221]
#define ID_PAT_id_number &at[222]
#define ID_PAT_id_app_number &at[223]

#define CIT_GEN &at[73]
#define CIT_GEN_cit &at[74]
#define CIT_GEN_authors &at[75]
#define CIT_GEN_muid &at[101]
#define CIT_GEN_journal &at[102]
#define CIT_GEN_volume &at[116]
#define CIT_GEN_issue &at[117]
#define CIT_GEN_pages &at[118]
#define CIT_GEN_date &at[119]
#define CIT_GEN_serial_number &at[121]
#define CIT_GEN_title &at[122]

#define CIT_PROC &at[163]
#define CIT_PROC_book &at[164]
#define CIT_PROC_meet &at[165]

#define CIT_SUB &at[125]
#define CIT_SUB_authors &at[126]
#define CIT_SUB_imp &at[127]
#define CIT_SUB_medium &at[139]

#define TITLE &at[103]
#define TITLE_E &at[104]
#define TITLE_E_name &at[105]
#define TITLE_E_tsub &at[106]
#define TITLE_E_trans &at[107]
#define TITLE_E_jta &at[108]
#define TITLE_E_iso_jta &at[109]
#define TITLE_E_ml_jta &at[110]
#define TITLE_E_coden &at[111]
#define TITLE_E_issn &at[112]
#define TITLE_E_abr &at[113]
#define TITLE_E_isbn &at[114]

#define AUTH_LIST &at[76]
#define AUTH_LIST_names &at[77]
#define AUTH_LIST_names_std &at[78]
#define AUTH_LIST_names_std_E &at[79]
#define AUTH_LIST_names_ml &at[96]
#define AUTH_LIST_names_ml_E &at[97]
#define AUTH_LIST_names_str &at[98]
#define AUTH_LIST_names_str_E &at[99]
#define AUTH_LIST_affil &at[100]

#define IMPRINT &at[128]
#define IMPRINT_date &at[129]
#define IMPRINT_volume &at[130]
#define IMPRINT_issue &at[131]
#define IMPRINT_pages &at[132]
#define IMPRINT_section &at[133]
#define IMPRINT_pub &at[134]
#define IMPRINT_cprt &at[135]
#define IMPRINT_part_sup &at[136]
#define IMPRINT_language &at[137]
#define IMPRINT_prepub &at[138]

#define MEETING &at[166]
#define MEETING_number &at[167]
#define MEETING_date &at[168]
#define MEETING_place &at[169]

#define AUTHOR &at[80]
#define AUTHOR_name &at[81]
#define AUTHOR_level &at[83]
#define AUTHOR_role &at[84]
#define AUTHOR_affil &at[85]
#define AUTHOR_is_corr &at[95]

#define AFFIL &at[86]
#define AFFIL_str &at[87]
#define AFFIL_std &at[88]
#define AFFIL_std_affil &at[89]
#define AFFIL_std_div &at[90]
#define AFFIL_std_city &at[91]
#define AFFIL_std_sub &at[92]
#define AFFIL_std_country &at[93]
#define AFFIL_std_street &at[94]


/**************************************************
*
*    Defines for Module NCBI-Medline
*
**************************************************/

#define MEDLINE_ENTRY &at[142]
#define MEDLINE_ENTRY_uid &at[143]
#define MEDLINE_ENTRY_em &at[144]
#define MEDLINE_ENTRY_cit &at[146]
#define MEDLINE_ENTRY_abstract &at[170]
#define MEDLINE_ENTRY_mesh &at[171]
#define MEDLINE_ENTRY_mesh_E &at[172]
#define MEDLINE_ENTRY_substance &at[181]
#define MEDLINE_ENTRY_substance_E &at[182]
#define MEDLINE_ENTRY_xref &at[187]
#define MEDLINE_ENTRY_xref_E &at[188]
#define MEDLINE_ENTRY_idnum &at[192]
#define MEDLINE_ENTRY_idnum_E &at[193]
#define MEDLINE_ENTRY_gene &at[194]
#define MEDLINE_ENTRY_gene_E &at[195]

#define MEDLINE_MESH &at[173]
#define MEDLINE_MESH_mp &at[174]
#define MEDLINE_MESH_term &at[175]
#define MEDLINE_MESH_qual &at[176]
#define MEDLINE_MESH_qual_E &at[177]

#define MEDLINE_RN &at[183]
#define MEDLINE_RN_type &at[184]
#define MEDLINE_RN_cit &at[185]
#define MEDLINE_RN_name &at[186]

#define MEDLINE_SI &at[189]
#define MEDLINE_SI_type &at[190]
#define MEDLINE_SI_cit &at[191]

#define MEDLINE_QUAL &at[178]
#define MEDLINE_QUAL_mp &at[179]
#define MEDLINE_QUAL_subh &at[180]
