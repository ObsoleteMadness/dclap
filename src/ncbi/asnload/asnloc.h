/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnloc.l00";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module NCBI-Seqloc
*
**************************************************/

#define SEQ_ID &at[0]
#define SEQ_ID_local &at[1]
#define SEQ_ID_gibbsq &at[3]
#define SEQ_ID_gibbmt &at[5]
#define SEQ_ID_giim &at[6]
#define SEQ_ID_genbank &at[13]
#define SEQ_ID_embl &at[19]
#define SEQ_ID_pir &at[20]
#define SEQ_ID_swissprot &at[21]
#define SEQ_ID_patent &at[22]
#define SEQ_ID_other &at[27]
#define SEQ_ID_general &at[28]
#define SEQ_ID_gi &at[30]
#define SEQ_ID_ddbj &at[31]
#define SEQ_ID_prf &at[32]
#define SEQ_ID_pdb &at[33]

#define SEQ_LOC &at[41]
#define SEQ_LOC_null &at[42]
#define SEQ_LOC_empty &at[44]
#define SEQ_LOC_whole &at[45]
#define SEQ_LOC_int &at[46]
#define SEQ_LOC_packed_int &at[57]
#define SEQ_LOC_pnt &at[61]
#define SEQ_LOC_packed_pnt &at[67]
#define SEQ_LOC_mix &at[74]
#define SEQ_LOC_equiv &at[77]
#define SEQ_LOC_bond &at[81]
#define SEQ_LOC_feat &at[85]

#define SEQ_INTERVAL &at[47]
#define SEQ_INTERVAL_from &at[48]
#define SEQ_INTERVAL_to &at[49]
#define SEQ_INTERVAL_strand &at[50]
#define SEQ_INTERVAL_id &at[53]
#define SEQ_INTERVAL_fuzz_from &at[54]
#define SEQ_INTERVAL_fuzz_to &at[56]

#define PACKED_SEQINT &at[58]
#define PACKED_SEQINT_E &at[59]

#define SEQ_POINT &at[62]
#define SEQ_POINT_point &at[63]
#define SEQ_POINT_strand &at[64]
#define SEQ_POINT_id &at[65]
#define SEQ_POINT_fuzz &at[66]

#define PACKED_SEQPNT &at[68]
#define PACKED_SEQPNT_strand &at[69]
#define PACKED_SEQPNT_id &at[70]
#define PACKED_SEQPNT_fuzz &at[71]
#define PACKED_SEQPNT_points &at[72]
#define PACKED_SEQPNT_points_E &at[73]

#define NA_STRAND &at[51]

#define GIIMPORT_ID &at[7]
#define GIIMPORT_ID_id &at[8]
#define GIIMPORT_ID_db &at[9]
#define GIIMPORT_ID_release &at[11]

#define TEXTSEQ_ID &at[14]
#define TEXTSEQ_ID_name &at[15]
#define TEXTSEQ_ID_accession &at[16]
#define TEXTSEQ_ID_release &at[17]
#define TEXTSEQ_ID_version &at[18]

#define PATENT_SEQ_ID &at[23]
#define PATENT_SEQ_ID_seqid &at[24]
#define PATENT_SEQ_ID_cit &at[25]

#define PDB_SEQ_ID &at[34]
#define PDB_SEQ_ID_mol &at[35]
#define PDB_SEQ_ID_chain &at[37]
#define PDB_SEQ_ID_rel &at[38]

#define PDB_MOL_ID &at[36]

#define SEQ_LOC_MIX &at[75]
#define SEQ_LOC_MIX_E &at[76]

#define SEQ_LOC_EQUIV &at[78]
#define SEQ_LOC_EQUIV_E &at[79]

#define SEQ_BOND &at[82]
#define SEQ_BOND_a &at[83]
#define SEQ_BOND_b &at[84]
