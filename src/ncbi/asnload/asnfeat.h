/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnfeat.l00";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module NCBI-Seqfeat
*
**************************************************/

#define SEQ_FEAT &at[0]
#define SEQ_FEAT_id &at[1]
#define SEQ_FEAT_data &at[12]
#define SEQ_FEAT_partial &at[159]
#define SEQ_FEAT_except &at[160]
#define SEQ_FEAT_comment &at[161]
#define SEQ_FEAT_product &at[162]
#define SEQ_FEAT_location &at[163]
#define SEQ_FEAT_qual &at[164]
#define SEQ_FEAT_qual_E &at[165]
#define SEQ_FEAT_title &at[169]
#define SEQ_FEAT_ext &at[170]
#define SEQ_FEAT_cit &at[171]
#define SEQ_FEAT_exp_ev &at[173]
#define SEQ_FEAT_xref &at[174]
#define SEQ_FEAT_xref_E &at[175]

#define FEAT_ID &at[2]
#define FEAT_ID_gibb &at[3]
#define FEAT_ID_giim &at[5]
#define FEAT_ID_local &at[7]
#define FEAT_ID_general &at[9]

#define SEQFEATDATA &at[13]
#define SEQFEATDATA_gene &at[14]
#define SEQFEATDATA_org &at[31]
#define SEQFEATDATA_cdregion &at[43]
#define SEQFEATDATA_prot &at[74]
#define SEQFEATDATA_rna &at[87]
#define SEQFEATDATA_pub &at[103]
#define SEQFEATDATA_seq &at[105]
#define SEQFEATDATA_imp &at[106]
#define SEQFEATDATA_region &at[111]
#define SEQFEATDATA_comment &at[112]
#define SEQFEATDATA_bond &at[114]
#define SEQFEATDATA_site &at[115]
#define SEQFEATDATA_rsite &at[116]
#define SEQFEATDATA_user &at[122]
#define SEQFEATDATA_txinit &at[124]
#define SEQFEATDATA_num &at[153]
#define SEQFEATDATA_psec_str &at[155]
#define SEQFEATDATA_non_std_residue &at[156]
#define SEQFEATDATA_het &at[157]

#define GB_QUAL &at[166]
#define GB_QUAL_qual &at[167]
#define GB_QUAL_val &at[168]

#define SEQFEATXREF &at[176]
#define SEQFEATXREF_id &at[177]
#define SEQFEATXREF_data &at[178]

#define CDREGION &at[44]
#define CDREGION_orf &at[45]
#define CDREGION_frame &at[46]
#define CDREGION_conflict &at[48]
#define CDREGION_gaps &at[49]
#define CDREGION_mismatch &at[50]
#define CDREGION_code &at[51]
#define CDREGION_code_break &at[63]
#define CDREGION_code_break_E &at[64]
#define CDREGION_stops &at[73]

#define IMP_FEAT &at[107]
#define IMP_FEAT_key &at[108]
#define IMP_FEAT_loc &at[109]
#define IMP_FEAT_descr &at[110]

#define GENETIC_CODE &at[52]
#define GENETIC_CODE_E &at[53]
#define GENETIC_CODE_E_name &at[54]
#define GENETIC_CODE_E_id &at[55]
#define GENETIC_CODE_E_ncbieaa &at[56]
#define GENETIC_CODE_E_ncbi8aa &at[57]
#define GENETIC_CODE_E_ncbistdaa &at[59]
#define GENETIC_CODE_E_sncbieaa &at[60]
#define GENETIC_CODE_E_sncbi8aa &at[61]
#define GENETIC_CODE_E_sncbistdaa &at[62]

#define CODE_BREAK &at[65]
#define CODE_BREAK_loc &at[66]
#define CODE_BREAK_aa &at[68]
#define CODE_BREAK_aa_ncbieaa &at[69]
#define CODE_BREAK_aa_ncbi8aa &at[70]
#define CODE_BREAK_aa_ncbistdaa &at[71]

#define GENETIC_CODE_TABLE &at[179]
#define GENETIC_CODE_TABLE_E &at[180]


/**************************************************
*
*    Defines for Module NCBI-Rsite
*
**************************************************/

#define RSITE_REF &at[118]
#define RSITE_REF_str &at[119]
#define RSITE_REF_db &at[120]


/**************************************************
*
*    Defines for Module NCBI-RNA
*
**************************************************/

#define RNA_REF &at[89]
#define RNA_REF_type &at[90]
#define RNA_REF_pseudo &at[91]
#define RNA_REF_ext &at[92]
#define RNA_REF_ext_name &at[93]
#define RNA_REF_ext_tRNA &at[94]

#define TRNA_EXT &at[95]
#define TRNA_EXT_aa &at[96]
#define TRNA_EXT_aa_iupacaa &at[97]
#define TRNA_EXT_aa_ncbieaa &at[98]
#define TRNA_EXT_aa_ncbi8aa &at[99]
#define TRNA_EXT_aa_ncbistdaa &at[100]
#define TRNA_EXT_codon &at[101]
#define TRNA_EXT_codon_E &at[102]


/**************************************************
*
*    Defines for Module NCBI-Gene
*
**************************************************/

#define GENE_REF &at[16]
#define GENE_REF_locus &at[17]
#define GENE_REF_allele &at[19]
#define GENE_REF_desc &at[20]
#define GENE_REF_maploc &at[21]
#define GENE_REF_pseudo &at[22]
#define GENE_REF_db &at[24]
#define GENE_REF_db_E &at[25]
#define GENE_REF_syn &at[28]
#define GENE_REF_syn_E &at[29]


/**************************************************
*
*    Defines for Module NCBI-Organism
*
**************************************************/

#define ORG_REF &at[33]
#define ORG_REF_taxname &at[34]
#define ORG_REF_common &at[35]
#define ORG_REF_mod &at[36]
#define ORG_REF_mod_E &at[37]
#define ORG_REF_db &at[38]
#define ORG_REF_db_E &at[39]
#define ORG_REF_syn &at[41]
#define ORG_REF_syn_E &at[42]


/**************************************************
*
*    Defines for Module NCBI-Protein
*
**************************************************/

#define PROT_REF &at[76]
#define PROT_REF_name &at[77]
#define PROT_REF_name_E &at[78]
#define PROT_REF_desc &at[79]
#define PROT_REF_ec &at[80]
#define PROT_REF_ec_E &at[81]
#define PROT_REF_activity &at[82]
#define PROT_REF_activity_E &at[83]
#define PROT_REF_db &at[84]
#define PROT_REF_db_E &at[85]


/**************************************************
*
*    Defines for Module NCBI-TxInit
*
**************************************************/

#define TXINIT &at[126]
#define TXINIT_name &at[127]
#define TXINIT_syn &at[128]
#define TXINIT_syn_E &at[129]
#define TXINIT_gene &at[130]
#define TXINIT_gene_E &at[131]
#define TXINIT_protein &at[133]
#define TXINIT_protein_E &at[134]
#define TXINIT_rna &at[136]
#define TXINIT_rna_E &at[137]
#define TXINIT_expression &at[138]
#define TXINIT_txsystem &at[139]
#define TXINIT_txdescr &at[140]
#define TXINIT_txorg &at[141]
#define TXINIT_mapping_precise &at[143]
#define TXINIT_location_accurate &at[144]
#define TXINIT_inittype &at[145]
#define TXINIT_evidence &at[146]
#define TXINIT_evidence_E &at[147]

#define TX_EVIDENCE &at[148]
#define TX_EVIDENCE_exp_code &at[149]
#define TX_EVIDENCE_expression_system &at[150]
#define TX_EVIDENCE_low_prec_data &at[151]
#define TX_EVIDENCE_from_homolog &at[152]
