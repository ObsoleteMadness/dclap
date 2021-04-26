/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnblock.l00";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module EMBL-General
*
**************************************************/

#define EMBL_DBNAME &at[0]
#define EMBL_DBNAME_code &at[1]
#define EMBL_DBNAME_name &at[3]

#define EMBL_XREF &at[6]
#define EMBL_XREF_dbname &at[7]
#define EMBL_XREF_id &at[8]
#define EMBL_XREF_id_E &at[9]

#define EMBL_BLOCK &at[13]
#define EMBL_BLOCK_class &at[14]
#define EMBL_BLOCK_div &at[15]
#define EMBL_BLOCK_creation_date &at[16]
#define EMBL_BLOCK_update_date &at[18]
#define EMBL_BLOCK_extra_acc &at[19]
#define EMBL_BLOCK_extra_acc_E &at[20]
#define EMBL_BLOCK_keywords &at[21]
#define EMBL_BLOCK_keywords_E &at[22]
#define EMBL_BLOCK_xref &at[23]
#define EMBL_BLOCK_xref_E &at[24]


/**************************************************
*
*    Defines for Module SP-General
*
**************************************************/

#define SP_BLOCK &at[25]
#define SP_BLOCK_class &at[26]
#define SP_BLOCK_extra_acc &at[27]
#define SP_BLOCK_extra_acc_E &at[28]
#define SP_BLOCK_imeth &at[30]
#define SP_BLOCK_plasnm &at[32]
#define SP_BLOCK_plasnm_E &at[33]
#define SP_BLOCK_seqref &at[34]
#define SP_BLOCK_seqref_E &at[35]
#define SP_BLOCK_dbref &at[37]
#define SP_BLOCK_dbref_E &at[38]
#define SP_BLOCK_keywords &at[40]
#define SP_BLOCK_keywords_E &at[41]
#define SP_BLOCK_created &at[42]
#define SP_BLOCK_sequpd &at[44]
#define SP_BLOCK_annotupd &at[45]


/**************************************************
*
*    Defines for Module PIR-General
*
**************************************************/

#define PIR_BLOCK &at[46]
#define PIR_BLOCK_had_punct &at[47]
#define PIR_BLOCK_host &at[48]
#define PIR_BLOCK_source &at[49]
#define PIR_BLOCK_summary &at[50]
#define PIR_BLOCK_genetic &at[51]
#define PIR_BLOCK_includes &at[52]
#define PIR_BLOCK_placement &at[53]
#define PIR_BLOCK_superfamily &at[54]
#define PIR_BLOCK_keywords &at[55]
#define PIR_BLOCK_keywords_E &at[56]
#define PIR_BLOCK_cross_reference &at[57]
#define PIR_BLOCK_date &at[58]
#define PIR_BLOCK_seq_raw &at[59]
#define PIR_BLOCK_seqref &at[60]
#define PIR_BLOCK_seqref_E &at[61]


/**************************************************
*
*    Defines for Module GenBank-General
*
**************************************************/

#define GB_BLOCK &at[63]
#define GB_BLOCK_extra_accessions &at[64]
#define GB_BLOCK_extra_accessions_E &at[65]
#define GB_BLOCK_source &at[66]
#define GB_BLOCK_keywords &at[67]
#define GB_BLOCK_keywords_E &at[68]
#define GB_BLOCK_origin &at[69]
#define GB_BLOCK_date &at[70]
#define GB_BLOCK_entry_date &at[71]
#define GB_BLOCK_div &at[73]
#define GB_BLOCK_taxonomy &at[74]


/**************************************************
*
*    Defines for Module PRF-General
*
**************************************************/

#define PRF_BLOCK &at[75]
#define PRF_BLOCK_extra_src &at[76]
#define PRF_BLOCK_keywords &at[83]
#define PRF_BLOCK_keywords_E &at[84]

#define PRF_EXTRASRC &at[77]
#define PRF_EXTRASRC_host &at[78]
#define PRF_EXTRASRC_part &at[79]
#define PRF_EXTRASRC_state &at[80]
#define PRF_EXTRASRC_strain &at[81]
#define PRF_EXTRASRC_taxon &at[82]


/**************************************************
*
*    Defines for Module PDB-General
*
**************************************************/

#define PDB_BLOCK &at[85]
#define PDB_BLOCK_deposition &at[86]
#define PDB_BLOCK_class &at[88]
#define PDB_BLOCK_compound &at[89]
#define PDB_BLOCK_compound_E &at[90]
#define PDB_BLOCK_source &at[91]
#define PDB_BLOCK_source_E &at[92]
#define PDB_BLOCK_exp_method &at[93]
#define PDB_BLOCK_replace &at[94]

#define PDB_REPLACE &at[95]
#define PDB_REPLACE_date &at[96]
#define PDB_REPLACE_ids &at[97]
#define PDB_REPLACE_ids_E &at[98]
