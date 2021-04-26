/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "all.l02";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module NCBI-Access
*
**************************************************/

#define LINK_SET &at[0]
#define LINK_SET_num &at[1]
#define LINK_SET_uids &at[3]
#define LINK_SET_uids_E &at[4]
#define LINK_SET_weights &at[6]
#define LINK_SET_weights_E &at[7]


/**************************************************
*
*    Defines for Module NCBI-Biblio
*
**************************************************/

#define CIT_ART &at[9]
#define CIT_ART_title &at[10]
#define CIT_ART_authors &at[26]
#define CIT_ART_from &at[73]
#define CIT_ART_from_journal &at[74]
#define CIT_ART_from_book &at[98]
#define CIT_ART_from_proc &at[104]

#define CIT_JOUR &at[75]
#define CIT_JOUR_title &at[76]
#define CIT_JOUR_imp &at[77]

#define CIT_BOOK &at[99]
#define CIT_BOOK_title &at[100]
#define CIT_BOOK_coll &at[101]
#define CIT_BOOK_authors &at[102]
#define CIT_BOOK_imp &at[103]

#define CIT_PAT &at[112]
#define CIT_PAT_title &at[113]
#define CIT_PAT_authors &at[114]
#define CIT_PAT_country &at[115]
#define CIT_PAT_doc_type &at[116]
#define CIT_PAT_number &at[117]
#define CIT_PAT_date_issue &at[118]
#define CIT_PAT_class &at[119]
#define CIT_PAT_app_number &at[120]
#define CIT_PAT_app_date &at[121]

#define CIT_LET &at[122]
#define CIT_LET_cit &at[123]
#define CIT_LET_man_id &at[124]
#define CIT_LET_type &at[125]

#define ID_PAT &at[126]
#define ID_PAT_country &at[127]
#define ID_PAT_id &at[128]
#define ID_PAT_id_number &at[129]
#define ID_PAT_id_app_number &at[130]

#define CIT_GEN &at[131]
#define CIT_GEN_cit &at[132]
#define CIT_GEN_authors &at[133]
#define CIT_GEN_muid &at[134]
#define CIT_GEN_journal &at[135]
#define CIT_GEN_volume &at[136]
#define CIT_GEN_issue &at[137]
#define CIT_GEN_pages &at[138]
#define CIT_GEN_date &at[139]
#define CIT_GEN_serial_number &at[140]
#define CIT_GEN_title &at[141]

#define CIT_PROC &at[105]
#define CIT_PROC_book &at[106]
#define CIT_PROC_meet &at[107]

#define CIT_SUB &at[142]
#define CIT_SUB_authors &at[143]
#define CIT_SUB_imp &at[144]
#define CIT_SUB_medium &at[145]

#define TITLE &at[11]
#define TITLE_E &at[12]
#define TITLE_E_name &at[13]
#define TITLE_E_tsub &at[15]
#define TITLE_E_trans &at[16]
#define TITLE_E_jta &at[17]
#define TITLE_E_iso_jta &at[18]
#define TITLE_E_ml_jta &at[19]
#define TITLE_E_coden &at[20]
#define TITLE_E_issn &at[21]
#define TITLE_E_abr &at[22]
#define TITLE_E_isbn &at[23]

#define AUTH_LIST &at[27]
#define AUTH_LIST_names &at[28]
#define AUTH_LIST_names_std &at[29]
#define AUTH_LIST_names_std_E &at[30]
#define AUTH_LIST_names_ml &at[68]
#define AUTH_LIST_names_ml_E &at[69]
#define AUTH_LIST_names_str &at[70]
#define AUTH_LIST_names_str_E &at[71]
#define AUTH_LIST_affil &at[72]

#define IMPRINT &at[78]
#define IMPRINT_date &at[79]
#define IMPRINT_volume &at[89]
#define IMPRINT_issue &at[90]
#define IMPRINT_pages &at[91]
#define IMPRINT_section &at[92]
#define IMPRINT_pub &at[93]
#define IMPRINT_cprt &at[94]
#define IMPRINT_part_sup &at[95]
#define IMPRINT_language &at[96]
#define IMPRINT_prepub &at[97]

#define MEETING &at[108]
#define MEETING_number &at[109]
#define MEETING_date &at[110]
#define MEETING_place &at[111]

#define AUTHOR &at[31]
#define AUTHOR_name &at[32]
#define AUTHOR_level &at[53]
#define AUTHOR_role &at[55]
#define AUTHOR_affil &at[56]
#define AUTHOR_is_corr &at[66]

#define AFFIL &at[57]
#define AFFIL_str &at[58]
#define AFFIL_std &at[59]
#define AFFIL_std_affil &at[60]
#define AFFIL_std_div &at[61]
#define AFFIL_std_city &at[62]
#define AFFIL_std_sub &at[63]
#define AFFIL_std_country &at[64]
#define AFFIL_std_street &at[65]


/**************************************************
*
*    Defines for Module NCBI-General
*
**************************************************/

#define DATE &at[81]
#define DATE_str &at[82]
#define DATE_std &at[83]

#define PERSON_ID &at[34]
#define PERSON_ID_dbtag &at[35]
#define PERSON_ID_name &at[42]
#define PERSON_ID_ml &at[51]
#define PERSON_ID_str &at[52]

#define OBJECT_ID &at[39]
#define OBJECT_ID_id &at[40]
#define OBJECT_ID_str &at[41]

#define DBTAG &at[36]
#define DBTAG_db &at[37]
#define DBTAG_tag &at[38]

#define INT_FUZZ &at[146]
#define INT_FUZZ_p_m &at[147]
#define INT_FUZZ_range &at[148]
#define INT_FUZZ_range_max &at[149]
#define INT_FUZZ_range_min &at[150]
#define INT_FUZZ_pct &at[151]
#define INT_FUZZ_lim &at[152]

#define USER_OBJECT &at[153]
#define USER_OBJECT_class &at[154]
#define USER_OBJECT_type &at[155]
#define USER_OBJECT_data &at[156]
#define USER_OBJECT_data_E &at[157]

#define DATE_STD &at[84]
#define DATE_STD_year &at[85]
#define DATE_STD_month &at[86]
#define DATE_STD_day &at[87]
#define DATE_STD_season &at[88]

#define NAME_STD &at[43]
#define NAME_STD_last &at[44]
#define NAME_STD_first &at[45]
#define NAME_STD_middle &at[46]
#define NAME_STD_full &at[47]
#define NAME_STD_initials &at[48]
#define NAME_STD_suffix &at[49]
#define NAME_STD_title &at[50]

#define USER_FIELD &at[158]
#define USER_FIELD_label &at[159]
#define USER_FIELD_num &at[160]
#define USER_FIELD_data &at[161]
#define USER_FIELD_data_str &at[162]
#define USER_FIELD_data_int &at[163]
#define USER_FIELD_data_real &at[164]
#define USER_FIELD_data_bool &at[166]
#define USER_FIELD_data_os &at[167]
#define USER_FIELD_data_object &at[169]
#define USER_FIELD_data_strs &at[170]
#define USER_FIELD_data_strs_E &at[171]
#define USER_FIELD_data_ints &at[172]
#define USER_FIELD_data_ints_E &at[173]
#define USER_FIELD_data_reals &at[174]
#define USER_FIELD_data_reals_E &at[175]
#define USER_FIELD_data_oss &at[176]
#define USER_FIELD_data_oss_E &at[177]
#define USER_FIELD_data_fields &at[178]
#define USER_FIELD_data_fields_E &at[179]
#define USER_FIELD_data_objects &at[180]
#define USER_FIELD_data_objects_E &at[181]


/**************************************************
*
*    Defines for Module NCBI-Medline
*
**************************************************/

#define MEDLINE_ENTRY &at[182]
#define MEDLINE_ENTRY_uid &at[183]
#define MEDLINE_ENTRY_em &at[184]
#define MEDLINE_ENTRY_cit &at[186]
#define MEDLINE_ENTRY_abstract &at[188]
#define MEDLINE_ENTRY_mesh &at[189]
#define MEDLINE_ENTRY_mesh_E &at[190]
#define MEDLINE_ENTRY_substance &at[199]
#define MEDLINE_ENTRY_substance_E &at[200]
#define MEDLINE_ENTRY_xref &at[205]
#define MEDLINE_ENTRY_xref_E &at[206]
#define MEDLINE_ENTRY_idnum &at[210]
#define MEDLINE_ENTRY_idnum_E &at[211]
#define MEDLINE_ENTRY_gene &at[212]
#define MEDLINE_ENTRY_gene_E &at[213]

#define MEDLINE_MESH &at[191]
#define MEDLINE_MESH_mp &at[192]
#define MEDLINE_MESH_term &at[193]
#define MEDLINE_MESH_qual &at[194]
#define MEDLINE_MESH_qual_E &at[195]

#define MEDLINE_RN &at[201]
#define MEDLINE_RN_type &at[202]
#define MEDLINE_RN_cit &at[203]
#define MEDLINE_RN_name &at[204]

#define MEDLINE_SI &at[207]
#define MEDLINE_SI_type &at[208]
#define MEDLINE_SI_cit &at[209]

#define MEDLINE_QUAL &at[196]
#define MEDLINE_QUAL_mp &at[197]
#define MEDLINE_QUAL_subh &at[198]


/**************************************************
*
*    Defines for Module NCBI-ObjPrt
*
**************************************************/

#define PRINTTEMPLATE &at[214]
#define PRINTTEMPLATE_name &at[215]
#define PRINTTEMPLATE_labelfrom &at[217]
#define PRINTTEMPLATE_format &at[218]

#define PRINTTEMPLATESET &at[249]
#define PRINTTEMPLATESET_E &at[250]

#define TEMPLATENAME &at[216]

#define PRINTFORMAT &at[219]
#define PRINTFORMAT_asn1 &at[220]
#define PRINTFORMAT_label &at[221]
#define PRINTFORMAT_prefix &at[222]
#define PRINTFORMAT_suffix &at[223]
#define PRINTFORMAT_form &at[224]

#define PRINTFORM &at[225]
#define PRINTFORM_block &at[226]
#define PRINTFORM_boolean &at[231]
#define PRINTFORM_enum &at[235]
#define PRINTFORM_text &at[239]
#define PRINTFORM_use_template &at[242]
#define PRINTFORM_user &at[243]
#define PRINTFORM_null &at[247]

#define PRINTFORMBLOCK &at[227]
#define PRINTFORMBLOCK_separator &at[228]
#define PRINTFORMBLOCK_components &at[229]
#define PRINTFORMBLOCK_components_E &at[230]

#define PRINTFORMBOOLEAN &at[232]
#define PRINTFORMBOOLEAN_true &at[233]
#define PRINTFORMBOOLEAN_false &at[234]

#define PRINTFORMENUM &at[236]
#define PRINTFORMENUM_values &at[237]
#define PRINTFORMENUM_values_E &at[238]

#define PRINTFORMTEXT &at[240]
#define PRINTFORMTEXT_textfunc &at[241]

#define USERFORMAT &at[244]
#define USERFORMAT_printfunc &at[245]
#define USERFORMAT_defaultfunc &at[246]


/**************************************************
*
*    Defines for Module NCBI-Pub
*
**************************************************/

#define PUB &at[251]
#define PUB_gen &at[252]
#define PUB_sub &at[254]
#define PUB_medline &at[256]
#define PUB_muid &at[258]
#define PUB_article &at[259]
#define PUB_journal &at[261]
#define PUB_book &at[263]
#define PUB_proc &at[265]
#define PUB_patent &at[267]
#define PUB_pat_id &at[269]
#define PUB_man &at[271]
#define PUB_equiv &at[273]

#define PUB_SET &at[276]
#define PUB_SET_pub &at[277]
#define PUB_SET_pub_E &at[278]
#define PUB_SET_medline &at[279]
#define PUB_SET_medline_E &at[280]
#define PUB_SET_article &at[281]
#define PUB_SET_article_E &at[282]
#define PUB_SET_journal &at[283]
#define PUB_SET_journal_E &at[284]
#define PUB_SET_book &at[285]
#define PUB_SET_book_E &at[286]
#define PUB_SET_proc &at[287]
#define PUB_SET_proc_E &at[288]
#define PUB_SET_patent &at[289]
#define PUB_SET_patent_E &at[290]

#define PUB_EQUIV &at[274]
#define PUB_EQUIV_E &at[275]


/**************************************************
*
*    Defines for Module NCBI-Sequence
*
**************************************************/

#define BIOSEQ &at[291]
#define BIOSEQ_id &at[292]
#define BIOSEQ_id_E &at[293]
#define BIOSEQ_descr &at[332]
#define BIOSEQ_inst &at[611]
#define BIOSEQ_annot &at[818]
#define BIOSEQ_annot_E &at[819]

#define SEQ_ANNOT &at[820]
#define SEQ_ANNOT_id &at[821]
#define SEQ_ANNOT_db &at[823]
#define SEQ_ANNOT_name &at[824]
#define SEQ_ANNOT_desc &at[825]
#define SEQ_ANNOT_data &at[826]
#define SEQ_ANNOT_data_ftable &at[827]
#define SEQ_ANNOT_data_ftable_E &at[828]
#define SEQ_ANNOT_data_align &at[829]
#define SEQ_ANNOT_data_align_E &at[830]
#define SEQ_ANNOT_data_graph &at[831]
#define SEQ_ANNOT_data_graph_E &at[832]

#define PUBDESC &at[517]
#define PUBDESC_pub &at[518]
#define PUBDESC_name &at[520]
#define PUBDESC_fig &at[521]
#define PUBDESC_num &at[522]
#define PUBDESC_numexc &at[523]
#define PUBDESC_poly_a &at[524]
#define PUBDESC_maploc &at[525]
#define PUBDESC_seq_raw &at[526]
#define PUBDESC_align_group &at[528]
#define PUBDESC_comment &at[529]

#define SEQ_DESCR &at[333]
#define SEQ_DESCR_E &at[334]
#define SEQ_DESCR_E_mol_type &at[335]
#define SEQ_DESCR_E_modif &at[337]
#define SEQ_DESCR_E_modif_E &at[338]
#define SEQ_DESCR_E_method &at[340]
#define SEQ_DESCR_E_name &at[342]
#define SEQ_DESCR_E_title &at[343]
#define SEQ_DESCR_E_org &at[344]
#define SEQ_DESCR_E_comment &at[356]
#define SEQ_DESCR_E_num &at[357]
#define SEQ_DESCR_E_maploc &at[481]
#define SEQ_DESCR_E_pir &at[483]
#define SEQ_DESCR_E_genbank &at[502]
#define SEQ_DESCR_E_pub &at[516]
#define SEQ_DESCR_E_region &at[530]
#define SEQ_DESCR_E_user &at[531]
#define SEQ_DESCR_E_sp &at[533]
#define SEQ_DESCR_E_neighbors &at[554]
#define SEQ_DESCR_E_embl &at[556]
#define SEQ_DESCR_E_create_date &at[578]
#define SEQ_DESCR_E_update_date &at[580]
#define SEQ_DESCR_E_prf &at[581]
#define SEQ_DESCR_E_pdb &at[593]
#define SEQ_DESCR_E_het &at[609]

#define NUMBERING &at[358]
#define NUMBERING_cont &at[359]
#define NUMBERING_enum &at[364]
#define NUMBERING_ref &at[369]
#define NUMBERING_real &at[476]

#define HETEROGEN &at[610]

#define SEQ_INST &at[612]
#define SEQ_INST_repr &at[613]
#define SEQ_INST_mol &at[614]
#define SEQ_INST_length &at[615]
#define SEQ_INST_fuzz &at[616]
#define SEQ_INST_topology &at[618]
#define SEQ_INST_strand &at[619]
#define SEQ_INST_seq_data &at[620]
#define SEQ_INST_ext &at[642]
#define SEQ_INST_hist &at[805]

#define GIBB_MOL &at[336]

#define GIBB_MOD &at[339]

#define GIBB_METHOD &at[341]

#define NUM_CONT &at[360]
#define NUM_CONT_refnum &at[361]
#define NUM_CONT_has_zero &at[362]
#define NUM_CONT_ascending &at[363]

#define NUM_ENUM &at[365]
#define NUM_ENUM_num &at[366]
#define NUM_ENUM_names &at[367]
#define NUM_ENUM_names_E &at[368]

#define NUM_REF &at[370]
#define NUM_REF_type &at[371]
#define NUM_REF_aligns &at[372]

#define NUM_REAL &at[477]
#define NUM_REAL_a &at[478]
#define NUM_REAL_b &at[479]
#define NUM_REAL_units &at[480]

#define SEQ_DATA &at[621]
#define SEQ_DATA_iupacna &at[622]
#define SEQ_DATA_iupacaa &at[624]
#define SEQ_DATA_ncbi2na &at[626]
#define SEQ_DATA_ncbi4na &at[628]
#define SEQ_DATA_ncbi8na &at[630]
#define SEQ_DATA_ncbipna &at[632]
#define SEQ_DATA_ncbi8aa &at[634]
#define SEQ_DATA_ncbieaa &at[636]
#define SEQ_DATA_ncbipaa &at[638]
#define SEQ_DATA_ncbistdaa &at[640]

#define SEQ_EXT &at[643]
#define SEQ_EXT_seg &at[644]
#define SEQ_EXT_ref &at[648]
#define SEQ_EXT_map &at[650]

#define SEQ_HIST &at[806]
#define SEQ_HIST_assembly &at[807]
#define SEQ_HIST_assembly_E &at[808]
#define SEQ_HIST_replaces &at[809]
#define SEQ_HIST_replaced_by &at[814]
#define SEQ_HIST_deleted &at[815]
#define SEQ_HIST_deleted_bool &at[816]
#define SEQ_HIST_deleted_date &at[817]

#define SEG_EXT &at[645]
#define SEG_EXT_E &at[646]

#define REF_EXT &at[649]

#define MAP_EXT &at[651]
#define MAP_EXT_E &at[652]

#define SEQ_HIST_REC &at[810]
#define SEQ_HIST_REC_date &at[811]
#define SEQ_HIST_REC_ids &at[812]
#define SEQ_HIST_REC_ids_E &at[813]

#define IUPACNA &at[623]

#define IUPACAA &at[625]

#define NCBI2NA &at[627]

#define NCBI4NA &at[629]

#define NCBI8NA &at[631]

#define NCBIPNA &at[633]

#define NCBI8AA &at[635]

#define NCBIEAA &at[637]

#define NCBIPAA &at[639]

#define NCBISTDAA &at[641]


/**************************************************
*
*    Defines for Module NCBI-Seqalign
*
**************************************************/

#define SEQ_ALIGN &at[374]
#define SEQ_ALIGN_type &at[375]
#define SEQ_ALIGN_dim &at[376]
#define SEQ_ALIGN_score &at[377]
#define SEQ_ALIGN_score_E &at[378]
#define SEQ_ALIGN_segs &at[385]
#define SEQ_ALIGN_segs_dendiag &at[386]
#define SEQ_ALIGN_segs_dendiag_E &at[387]
#define SEQ_ALIGN_segs_denseg &at[402]
#define SEQ_ALIGN_segs_std &at[416]
#define SEQ_ALIGN_segs_std_E &at[417]

#define SCORE &at[379]
#define SCORE_id &at[380]
#define SCORE_value &at[382]
#define SCORE_value_real &at[383]
#define SCORE_value_int &at[384]

#define DENSE_DIAG &at[388]
#define DENSE_DIAG_dim &at[389]
#define DENSE_DIAG_ids &at[390]
#define DENSE_DIAG_ids_E &at[391]
#define DENSE_DIAG_starts &at[393]
#define DENSE_DIAG_starts_E &at[394]
#define DENSE_DIAG_len &at[395]
#define DENSE_DIAG_strands &at[396]
#define DENSE_DIAG_strands_E &at[397]
#define DENSE_DIAG_scores &at[400]
#define DENSE_DIAG_scores_E &at[401]

#define DENSE_SEG &at[403]
#define DENSE_SEG_dim &at[404]
#define DENSE_SEG_numseg &at[405]
#define DENSE_SEG_ids &at[406]
#define DENSE_SEG_ids_E &at[407]
#define DENSE_SEG_starts &at[408]
#define DENSE_SEG_starts_E &at[409]
#define DENSE_SEG_lens &at[410]
#define DENSE_SEG_lens_E &at[411]
#define DENSE_SEG_strands &at[412]
#define DENSE_SEG_strands_E &at[413]
#define DENSE_SEG_scores &at[414]
#define DENSE_SEG_scores_E &at[415]

#define STD_SEG &at[418]
#define STD_SEG_dim &at[419]
#define STD_SEG_ids &at[420]
#define STD_SEG_ids_E &at[421]
#define STD_SEG_loc &at[422]
#define STD_SEG_loc_E &at[423]
#define STD_SEG_scores &at[474]
#define STD_SEG_scores_E &at[475]


/**************************************************
*
*    Defines for Module EMBL-General
*
**************************************************/

#define EMBL_DBNAME &at[572]
#define EMBL_DBNAME_code &at[573]
#define EMBL_DBNAME_name &at[574]

#define EMBL_XREF &at[570]
#define EMBL_XREF_dbname &at[571]
#define EMBL_XREF_id &at[575]
#define EMBL_XREF_id_E &at[576]

#define EMBL_BLOCK &at[558]
#define EMBL_BLOCK_class &at[559]
#define EMBL_BLOCK_div &at[560]
#define EMBL_BLOCK_creation_date &at[561]
#define EMBL_BLOCK_update_date &at[563]
#define EMBL_BLOCK_extra_acc &at[564]
#define EMBL_BLOCK_extra_acc_E &at[565]
#define EMBL_BLOCK_keywords &at[566]
#define EMBL_BLOCK_keywords_E &at[567]
#define EMBL_BLOCK_xref &at[568]
#define EMBL_BLOCK_xref_E &at[569]


/**************************************************
*
*    Defines for Module SP-General
*
**************************************************/

#define SP_BLOCK &at[535]
#define SP_BLOCK_class &at[536]
#define SP_BLOCK_extra_acc &at[537]
#define SP_BLOCK_extra_acc_E &at[538]
#define SP_BLOCK_imeth &at[539]
#define SP_BLOCK_plasnm &at[540]
#define SP_BLOCK_plasnm_E &at[541]
#define SP_BLOCK_seqref &at[542]
#define SP_BLOCK_seqref_E &at[543]
#define SP_BLOCK_dbref &at[545]
#define SP_BLOCK_dbref_E &at[546]
#define SP_BLOCK_keywords &at[548]
#define SP_BLOCK_keywords_E &at[549]
#define SP_BLOCK_created &at[550]
#define SP_BLOCK_sequpd &at[552]
#define SP_BLOCK_annotupd &at[553]


/**************************************************
*
*    Defines for Module PIR-General
*
**************************************************/

#define PIR_BLOCK &at[485]
#define PIR_BLOCK_had_punct &at[486]
#define PIR_BLOCK_host &at[487]
#define PIR_BLOCK_source &at[488]
#define PIR_BLOCK_summary &at[489]
#define PIR_BLOCK_genetic &at[490]
#define PIR_BLOCK_includes &at[491]
#define PIR_BLOCK_placement &at[492]
#define PIR_BLOCK_superfamily &at[493]
#define PIR_BLOCK_keywords &at[494]
#define PIR_BLOCK_keywords_E &at[495]
#define PIR_BLOCK_cross_reference &at[496]
#define PIR_BLOCK_date &at[497]
#define PIR_BLOCK_seq_raw &at[498]
#define PIR_BLOCK_seqref &at[499]
#define PIR_BLOCK_seqref_E &at[500]


/**************************************************
*
*    Defines for Module GenBank-General
*
**************************************************/

#define GB_BLOCK &at[504]
#define GB_BLOCK_extra_accessions &at[505]
#define GB_BLOCK_extra_accessions_E &at[506]
#define GB_BLOCK_source &at[507]
#define GB_BLOCK_keywords &at[508]
#define GB_BLOCK_keywords_E &at[509]
#define GB_BLOCK_origin &at[510]
#define GB_BLOCK_date &at[511]
#define GB_BLOCK_entry_date &at[512]
#define GB_BLOCK_div &at[514]
#define GB_BLOCK_taxonomy &at[515]


/**************************************************
*
*    Defines for Module PRF-General
*
**************************************************/

#define PRF_BLOCK &at[583]
#define PRF_BLOCK_extra_src &at[584]
#define PRF_BLOCK_keywords &at[591]
#define PRF_BLOCK_keywords_E &at[592]

#define PRF_EXTRASRC &at[585]
#define PRF_EXTRASRC_host &at[586]
#define PRF_EXTRASRC_part &at[587]
#define PRF_EXTRASRC_state &at[588]
#define PRF_EXTRASRC_strain &at[589]
#define PRF_EXTRASRC_taxon &at[590]


/**************************************************
*
*    Defines for Module PDB-General
*
**************************************************/

#define PDB_BLOCK &at[595]
#define PDB_BLOCK_deposition &at[596]
#define PDB_BLOCK_class &at[598]
#define PDB_BLOCK_compound &at[599]
#define PDB_BLOCK_compound_E &at[600]
#define PDB_BLOCK_source &at[601]
#define PDB_BLOCK_source_E &at[602]
#define PDB_BLOCK_exp_method &at[603]
#define PDB_BLOCK_replace &at[604]

#define PDB_REPLACE &at[605]
#define PDB_REPLACE_date &at[606]
#define PDB_REPLACE_ids &at[607]
#define PDB_REPLACE_ids_E &at[608]


/**************************************************
*
*    Defines for Module NCBI-SeqCode
*
**************************************************/

#define SEQ_CODE_TABLE &at[866]
#define SEQ_CODE_TABLE_code &at[867]
#define SEQ_CODE_TABLE_num &at[869]
#define SEQ_CODE_TABLE_one_letter &at[870]
#define SEQ_CODE_TABLE_start_at &at[871]
#define SEQ_CODE_TABLE_table &at[872]
#define SEQ_CODE_TABLE_table_E &at[873]
#define SEQ_CODE_TABLE_table_E_symbol &at[874]
#define SEQ_CODE_TABLE_table_E_name &at[875]
#define SEQ_CODE_TABLE_comps &at[876]
#define SEQ_CODE_TABLE_comps_E &at[877]

#define SEQ_MAP_TABLE &at[878]
#define SEQ_MAP_TABLE_from &at[879]
#define SEQ_MAP_TABLE_to &at[880]
#define SEQ_MAP_TABLE_num &at[881]
#define SEQ_MAP_TABLE_start_at &at[882]
#define SEQ_MAP_TABLE_table &at[883]
#define SEQ_MAP_TABLE_table_E &at[884]

#define SEQ_CODE_SET &at[885]
#define SEQ_CODE_SET_codes &at[886]
#define SEQ_CODE_SET_codes_E &at[887]
#define SEQ_CODE_SET_maps &at[888]
#define SEQ_CODE_SET_maps_E &at[889]

#define SEQ_CODE_TYPE &at[868]


/**************************************************
*
*    Defines for Module NCBI-Seqfeat
*
**************************************************/

#define SEQ_FEAT &at[654]
#define SEQ_FEAT_id &at[655]
#define SEQ_FEAT_data &at[656]
#define SEQ_FEAT_partial &at[785]
#define SEQ_FEAT_except &at[786]
#define SEQ_FEAT_comment &at[787]
#define SEQ_FEAT_product &at[788]
#define SEQ_FEAT_location &at[789]
#define SEQ_FEAT_qual &at[790]
#define SEQ_FEAT_qual_E &at[791]
#define SEQ_FEAT_title &at[795]
#define SEQ_FEAT_ext &at[796]
#define SEQ_FEAT_cit &at[797]
#define SEQ_FEAT_exp_ev &at[799]
#define SEQ_FEAT_xref &at[800]
#define SEQ_FEAT_xref_E &at[801]

#define FEAT_ID &at[466]
#define FEAT_ID_gibb &at[467]
#define FEAT_ID_giim &at[468]
#define FEAT_ID_local &at[470]
#define FEAT_ID_general &at[472]

#define SEQFEATDATA &at[657]
#define SEQFEATDATA_gene &at[658]
#define SEQFEATDATA_org &at[671]
#define SEQFEATDATA_cdregion &at[673]
#define SEQFEATDATA_prot &at[701]
#define SEQFEATDATA_rna &at[714]
#define SEQFEATDATA_pub &at[730]
#define SEQFEATDATA_seq &at[732]
#define SEQFEATDATA_imp &at[733]
#define SEQFEATDATA_region &at[738]
#define SEQFEATDATA_comment &at[739]
#define SEQFEATDATA_bond &at[740]
#define SEQFEATDATA_site &at[741]
#define SEQFEATDATA_rsite &at[742]
#define SEQFEATDATA_user &at[748]
#define SEQFEATDATA_txinit &at[750]
#define SEQFEATDATA_num &at[779]
#define SEQFEATDATA_psec_str &at[781]
#define SEQFEATDATA_non_std_residue &at[782]
#define SEQFEATDATA_het &at[783]

#define GB_QUAL &at[792]
#define GB_QUAL_qual &at[793]
#define GB_QUAL_val &at[794]

#define SEQFEATXREF &at[802]
#define SEQFEATXREF_id &at[803]
#define SEQFEATXREF_data &at[804]

#define CDREGION &at[674]
#define CDREGION_orf &at[675]
#define CDREGION_frame &at[676]
#define CDREGION_conflict &at[677]
#define CDREGION_gaps &at[678]
#define CDREGION_mismatch &at[679]
#define CDREGION_code &at[680]
#define CDREGION_code_break &at[691]
#define CDREGION_code_break_E &at[692]
#define CDREGION_stops &at[700]

#define IMP_FEAT &at[734]
#define IMP_FEAT_key &at[735]
#define IMP_FEAT_loc &at[736]
#define IMP_FEAT_descr &at[737]

#define GENETIC_CODE &at[681]
#define GENETIC_CODE_E &at[682]
#define GENETIC_CODE_E_name &at[683]
#define GENETIC_CODE_E_id &at[684]
#define GENETIC_CODE_E_ncbieaa &at[685]
#define GENETIC_CODE_E_ncbi8aa &at[686]
#define GENETIC_CODE_E_ncbistdaa &at[687]
#define GENETIC_CODE_E_sncbieaa &at[688]
#define GENETIC_CODE_E_sncbi8aa &at[689]
#define GENETIC_CODE_E_sncbistdaa &at[690]

#define CODE_BREAK &at[693]
#define CODE_BREAK_loc &at[694]
#define CODE_BREAK_aa &at[696]
#define CODE_BREAK_aa_ncbieaa &at[697]
#define CODE_BREAK_aa_ncbi8aa &at[698]
#define CODE_BREAK_aa_ncbistdaa &at[699]

#define GENETIC_CODE_TABLE &at[890]
#define GENETIC_CODE_TABLE_E &at[891]


/**************************************************
*
*    Defines for Module NCBI-Rsite
*
**************************************************/

#define RSITE_REF &at[744]
#define RSITE_REF_str &at[745]
#define RSITE_REF_db &at[746]


/**************************************************
*
*    Defines for Module NCBI-RNA
*
**************************************************/

#define RNA_REF &at[716]
#define RNA_REF_type &at[717]
#define RNA_REF_pseudo &at[718]
#define RNA_REF_ext &at[719]
#define RNA_REF_ext_name &at[720]
#define RNA_REF_ext_tRNA &at[721]

#define TRNA_EXT &at[722]
#define TRNA_EXT_aa &at[723]
#define TRNA_EXT_aa_iupacaa &at[724]
#define TRNA_EXT_aa_ncbieaa &at[725]
#define TRNA_EXT_aa_ncbi8aa &at[726]
#define TRNA_EXT_aa_ncbistdaa &at[727]
#define TRNA_EXT_codon &at[728]
#define TRNA_EXT_codon_E &at[729]


/**************************************************
*
*    Defines for Module NCBI-Gene
*
**************************************************/

#define GENE_REF &at[660]
#define GENE_REF_locus &at[661]
#define GENE_REF_allele &at[662]
#define GENE_REF_desc &at[663]
#define GENE_REF_maploc &at[664]
#define GENE_REF_pseudo &at[665]
#define GENE_REF_db &at[666]
#define GENE_REF_db_E &at[667]
#define GENE_REF_syn &at[669]
#define GENE_REF_syn_E &at[670]


/**************************************************
*
*    Defines for Module NCBI-Organism
*
**************************************************/

#define ORG_REF &at[346]
#define ORG_REF_taxname &at[347]
#define ORG_REF_common &at[348]
#define ORG_REF_mod &at[349]
#define ORG_REF_mod_E &at[350]
#define ORG_REF_db &at[351]
#define ORG_REF_db_E &at[352]
#define ORG_REF_syn &at[354]
#define ORG_REF_syn_E &at[355]


/**************************************************
*
*    Defines for Module NCBI-Protein
*
**************************************************/

#define PROT_REF &at[703]
#define PROT_REF_name &at[704]
#define PROT_REF_name_E &at[705]
#define PROT_REF_desc &at[706]
#define PROT_REF_ec &at[707]
#define PROT_REF_ec_E &at[708]
#define PROT_REF_activity &at[709]
#define PROT_REF_activity_E &at[710]
#define PROT_REF_db &at[711]
#define PROT_REF_db_E &at[712]


/**************************************************
*
*    Defines for Module NCBI-TxInit
*
**************************************************/

#define TXINIT &at[752]
#define TXINIT_name &at[753]
#define TXINIT_syn &at[754]
#define TXINIT_syn_E &at[755]
#define TXINIT_gene &at[756]
#define TXINIT_gene_E &at[757]
#define TXINIT_protein &at[759]
#define TXINIT_protein_E &at[760]
#define TXINIT_rna &at[762]
#define TXINIT_rna_E &at[763]
#define TXINIT_expression &at[764]
#define TXINIT_txsystem &at[765]
#define TXINIT_txdescr &at[766]
#define TXINIT_txorg &at[767]
#define TXINIT_mapping_precise &at[769]
#define TXINIT_location_accurate &at[770]
#define TXINIT_inittype &at[771]
#define TXINIT_evidence &at[772]
#define TXINIT_evidence_E &at[773]

#define TX_EVIDENCE &at[774]
#define TX_EVIDENCE_exp_code &at[775]
#define TX_EVIDENCE_expression_system &at[776]
#define TX_EVIDENCE_low_prec_data &at[777]
#define TX_EVIDENCE_from_homolog &at[778]


/**************************************************
*
*    Defines for Module NCBI-Seqloc
*
**************************************************/

#define SEQ_ID &at[295]
#define SEQ_ID_local &at[296]
#define SEQ_ID_gibbsq &at[298]
#define SEQ_ID_gibbmt &at[299]
#define SEQ_ID_giim &at[300]
#define SEQ_ID_genbank &at[305]
#define SEQ_ID_embl &at[311]
#define SEQ_ID_pir &at[312]
#define SEQ_ID_swissprot &at[313]
#define SEQ_ID_patent &at[314]
#define SEQ_ID_other &at[319]
#define SEQ_ID_general &at[320]
#define SEQ_ID_gi &at[322]
#define SEQ_ID_ddbj &at[323]
#define SEQ_ID_prf &at[324]
#define SEQ_ID_pdb &at[325]

#define SEQ_LOC &at[425]
#define SEQ_LOC_null &at[426]
#define SEQ_LOC_empty &at[427]
#define SEQ_LOC_whole &at[428]
#define SEQ_LOC_int &at[429]
#define SEQ_LOC_packed_int &at[438]
#define SEQ_LOC_pnt &at[441]
#define SEQ_LOC_packed_pnt &at[447]
#define SEQ_LOC_mix &at[454]
#define SEQ_LOC_equiv &at[457]
#define SEQ_LOC_bond &at[460]
#define SEQ_LOC_feat &at[464]

#define SEQ_INTERVAL &at[430]
#define SEQ_INTERVAL_from &at[431]
#define SEQ_INTERVAL_to &at[432]
#define SEQ_INTERVAL_strand &at[433]
#define SEQ_INTERVAL_id &at[434]
#define SEQ_INTERVAL_fuzz_from &at[435]
#define SEQ_INTERVAL_fuzz_to &at[437]

#define PACKED_SEQINT &at[439]
#define PACKED_SEQINT_E &at[440]

#define SEQ_POINT &at[442]
#define SEQ_POINT_point &at[443]
#define SEQ_POINT_strand &at[444]
#define SEQ_POINT_id &at[445]
#define SEQ_POINT_fuzz &at[446]

#define PACKED_SEQPNT &at[448]
#define PACKED_SEQPNT_strand &at[449]
#define PACKED_SEQPNT_id &at[450]
#define PACKED_SEQPNT_fuzz &at[451]
#define PACKED_SEQPNT_points &at[452]
#define PACKED_SEQPNT_points_E &at[453]

#define NA_STRAND &at[399]

#define GIIMPORT_ID &at[301]
#define GIIMPORT_ID_id &at[302]
#define GIIMPORT_ID_db &at[303]
#define GIIMPORT_ID_release &at[304]

#define TEXTSEQ_ID &at[306]
#define TEXTSEQ_ID_name &at[307]
#define TEXTSEQ_ID_accession &at[308]
#define TEXTSEQ_ID_release &at[309]
#define TEXTSEQ_ID_version &at[310]

#define PATENT_SEQ_ID &at[315]
#define PATENT_SEQ_ID_seqid &at[316]
#define PATENT_SEQ_ID_cit &at[317]

#define PDB_SEQ_ID &at[326]
#define PDB_SEQ_ID_mol &at[327]
#define PDB_SEQ_ID_chain &at[329]
#define PDB_SEQ_ID_rel &at[330]

#define PDB_MOL_ID &at[328]

#define SEQ_LOC_MIX &at[455]
#define SEQ_LOC_MIX_E &at[456]

#define SEQ_LOC_EQUIV &at[458]
#define SEQ_LOC_EQUIV_E &at[459]

#define SEQ_BOND &at[461]
#define SEQ_BOND_a &at[462]
#define SEQ_BOND_b &at[463]


/**************************************************
*
*    Defines for Module NCBI-Seqres
*
**************************************************/

#define SEQ_GRAPH &at[834]
#define SEQ_GRAPH_title &at[835]
#define SEQ_GRAPH_comment &at[836]
#define SEQ_GRAPH_loc &at[837]
#define SEQ_GRAPH_title_x &at[839]
#define SEQ_GRAPH_title_y &at[840]
#define SEQ_GRAPH_comp &at[841]
#define SEQ_GRAPH_a &at[842]
#define SEQ_GRAPH_b &at[843]
#define SEQ_GRAPH_numval &at[844]
#define SEQ_GRAPH_graph &at[845]
#define SEQ_GRAPH_graph_real &at[846]
#define SEQ_GRAPH_graph_int &at[853]
#define SEQ_GRAPH_graph_byte &at[860]

#define REAL_GRAPH &at[847]
#define REAL_GRAPH_max &at[848]
#define REAL_GRAPH_min &at[849]
#define REAL_GRAPH_axis &at[850]
#define REAL_GRAPH_values &at[851]
#define REAL_GRAPH_values_E &at[852]

#define INT_GRAPH &at[854]
#define INT_GRAPH_max &at[855]
#define INT_GRAPH_min &at[856]
#define INT_GRAPH_axis &at[857]
#define INT_GRAPH_values &at[858]
#define INT_GRAPH_values_E &at[859]

#define BYTE_GRAPH &at[861]
#define BYTE_GRAPH_max &at[862]
#define BYTE_GRAPH_min &at[863]
#define BYTE_GRAPH_axis &at[864]
#define BYTE_GRAPH_values &at[865]


/**************************************************
*
*    Defines for Module NCBI-Seqset
*
**************************************************/

#define BIOSEQ_SET &at[892]
#define BIOSEQ_SET_id &at[893]
#define BIOSEQ_SET_coll &at[895]
#define BIOSEQ_SET_level &at[897]
#define BIOSEQ_SET_class &at[898]
#define BIOSEQ_SET_release &at[899]
#define BIOSEQ_SET_date &at[900]
#define BIOSEQ_SET_descr &at[902]
#define BIOSEQ_SET_seq_set &at[904]
#define BIOSEQ_SET_seq_set_E &at[905]
#define BIOSEQ_SET_annot &at[910]
#define BIOSEQ_SET_annot_E &at[911]

#define SEQ_ENTRY &at[906]
#define SEQ_ENTRY_seq &at[907]
#define SEQ_ENTRY_set &at[909]


/**************************************************
*
*    Defines for Module NCBI-Submit
*
**************************************************/

#define SEQ_SUBMIT &at[913]
#define SEQ_SUBMIT_sub &at[914]
#define SEQ_SUBMIT_data &at[933]
#define SEQ_SUBMIT_data_entrys &at[934]
#define SEQ_SUBMIT_data_entrys_E &at[935]
#define SEQ_SUBMIT_data_annots &at[937]
#define SEQ_SUBMIT_data_annots_E &at[938]
#define SEQ_SUBMIT_data_delete &at[940]
#define SEQ_SUBMIT_data_delete_E &at[941]

#define SUBMIT_BLOCK &at[915]
#define SUBMIT_BLOCK_contact &at[916]
#define SUBMIT_BLOCK_cit &at[928]
#define SUBMIT_BLOCK_hup &at[930]
#define SUBMIT_BLOCK_reldate &at[931]

#define CONTACT_INFO &at[917]
#define CONTACT_INFO_name &at[918]
#define CONTACT_INFO_address &at[919]
#define CONTACT_INFO_address_E &at[920]
#define CONTACT_INFO_phone &at[921]
#define CONTACT_INFO_fax &at[922]
#define CONTACT_INFO_email &at[923]
#define CONTACT_INFO_telex &at[924]
#define CONTACT_INFO_owner_id &at[925]
#define CONTACT_INFO_password &at[927]
