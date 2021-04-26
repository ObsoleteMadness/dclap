/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnseq.l01";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module NCBI-Sequence
*
**************************************************/

#define BIOSEQ &at[0]
#define BIOSEQ_id &at[1]
#define BIOSEQ_id_E &at[2]
#define BIOSEQ_descr &at[5]
#define BIOSEQ_inst &at[88]
#define BIOSEQ_annot &at[145]
#define BIOSEQ_annot_E &at[146]

#define SEQ_ANNOT &at[147]
#define SEQ_ANNOT_id &at[148]
#define SEQ_ANNOT_db &at[150]
#define SEQ_ANNOT_name &at[151]
#define SEQ_ANNOT_desc &at[152]
#define SEQ_ANNOT_data &at[153]
#define SEQ_ANNOT_data_ftable &at[154]
#define SEQ_ANNOT_data_ftable_E &at[155]
#define SEQ_ANNOT_data_align &at[156]
#define SEQ_ANNOT_data_align_E &at[157]
#define SEQ_ANNOT_data_graph &at[158]
#define SEQ_ANNOT_data_graph_E &at[159]

#define PUBDESC &at[57]
#define PUBDESC_pub &at[58]
#define PUBDESC_name &at[60]
#define PUBDESC_fig &at[61]
#define PUBDESC_num &at[62]
#define PUBDESC_numexc &at[63]
#define PUBDESC_poly_a &at[64]
#define PUBDESC_maploc &at[65]
#define PUBDESC_seq_raw &at[66]
#define PUBDESC_align_group &at[68]
#define PUBDESC_comment &at[69]

#define SEQ_DESCR &at[6]
#define SEQ_DESCR_E &at[7]
#define SEQ_DESCR_E_mol_type &at[8]
#define SEQ_DESCR_E_modif &at[11]
#define SEQ_DESCR_E_modif_E &at[12]
#define SEQ_DESCR_E_method &at[14]
#define SEQ_DESCR_E_name &at[16]
#define SEQ_DESCR_E_title &at[18]
#define SEQ_DESCR_E_org &at[19]
#define SEQ_DESCR_E_comment &at[21]
#define SEQ_DESCR_E_num &at[22]
#define SEQ_DESCR_E_maploc &at[50]
#define SEQ_DESCR_E_pir &at[52]
#define SEQ_DESCR_E_genbank &at[54]
#define SEQ_DESCR_E_pub &at[56]
#define SEQ_DESCR_E_region &at[70]
#define SEQ_DESCR_E_user &at[71]
#define SEQ_DESCR_E_sp &at[73]
#define SEQ_DESCR_E_neighbors &at[75]
#define SEQ_DESCR_E_embl &at[77]
#define SEQ_DESCR_E_create_date &at[79]
#define SEQ_DESCR_E_update_date &at[81]
#define SEQ_DESCR_E_prf &at[82]
#define SEQ_DESCR_E_pdb &at[84]
#define SEQ_DESCR_E_het &at[86]

#define NUMBERING &at[23]
#define NUMBERING_cont &at[24]
#define NUMBERING_enum &at[32]
#define NUMBERING_ref &at[38]
#define NUMBERING_real &at[43]

#define HETEROGEN &at[87]

#define SEQ_INST &at[89]
#define SEQ_INST_repr &at[90]
#define SEQ_INST_mol &at[91]
#define SEQ_INST_length &at[92]
#define SEQ_INST_fuzz &at[93]
#define SEQ_INST_topology &at[95]
#define SEQ_INST_strand &at[96]
#define SEQ_INST_seq_data &at[97]
#define SEQ_INST_ext &at[120]
#define SEQ_INST_hist &at[132]

#define GIBB_MOL &at[9]

#define GIBB_MOD &at[13]

#define GIBB_METHOD &at[15]

#define NUM_CONT &at[25]
#define NUM_CONT_refnum &at[26]
#define NUM_CONT_has_zero &at[28]
#define NUM_CONT_ascending &at[30]

#define NUM_ENUM &at[33]
#define NUM_ENUM_num &at[34]
#define NUM_ENUM_names &at[35]
#define NUM_ENUM_names_E &at[36]

#define NUM_REF &at[39]
#define NUM_REF_type &at[40]
#define NUM_REF_aligns &at[41]

#define NUM_REAL &at[44]
#define NUM_REAL_a &at[45]
#define NUM_REAL_b &at[47]
#define NUM_REAL_units &at[48]

#define SEQ_DATA &at[98]
#define SEQ_DATA_iupacna &at[99]
#define SEQ_DATA_iupacaa &at[101]
#define SEQ_DATA_ncbi2na &at[103]
#define SEQ_DATA_ncbi4na &at[106]
#define SEQ_DATA_ncbi8na &at[108]
#define SEQ_DATA_ncbipna &at[110]
#define SEQ_DATA_ncbi8aa &at[112]
#define SEQ_DATA_ncbieaa &at[114]
#define SEQ_DATA_ncbipaa &at[116]
#define SEQ_DATA_ncbistdaa &at[118]

#define SEQ_EXT &at[121]
#define SEQ_EXT_seg &at[122]
#define SEQ_EXT_ref &at[126]
#define SEQ_EXT_map &at[128]

#define SEQ_HIST &at[133]
#define SEQ_HIST_assembly &at[134]
#define SEQ_HIST_assembly_E &at[135]
#define SEQ_HIST_replaces &at[136]
#define SEQ_HIST_replaced_by &at[141]
#define SEQ_HIST_deleted &at[142]
#define SEQ_HIST_deleted_bool &at[143]
#define SEQ_HIST_deleted_date &at[144]

#define SEG_EXT &at[123]
#define SEG_EXT_E &at[124]

#define REF_EXT &at[127]

#define MAP_EXT &at[129]
#define MAP_EXT_E &at[130]

#define SEQ_HIST_REC &at[137]
#define SEQ_HIST_REC_date &at[138]
#define SEQ_HIST_REC_ids &at[139]
#define SEQ_HIST_REC_ids_E &at[140]

#define IUPACNA &at[100]

#define IUPACAA &at[102]

#define NCBI2NA &at[104]

#define NCBI4NA &at[107]

#define NCBI8NA &at[109]

#define NCBIPNA &at[111]

#define NCBI8AA &at[113]

#define NCBIEAA &at[115]

#define NCBIPAA &at[117]

#define NCBISTDAA &at[119]
