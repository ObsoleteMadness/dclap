/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnloc.h00";
static AsnValxNode avnx[7] = {
    {3,NULL,32,0.0,NULL } ,
    {20,"unknown" ,0,0.0,&avnx[2] } ,
    {20,"plus" ,1,0.0,&avnx[3] } ,
    {20,"minus" ,2,0.0,&avnx[4] } ,
    {20,"both" ,3,0.0,&avnx[5] } ,
    {20,"both-rev" ,4,0.0,&avnx[6] } ,
    {20,"other" ,255,0.0,NULL } };

static AsnType atx[87] = {
  {401, "Seq-id" ,1,0,0,0,0,1,0,0,NULL,&atx[40],&atx[1],0,&atx[41]} ,
  {0, "local" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[3]} ,
  {409, "Object-id" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[55]} ,
  {0, "gibbsq" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[5]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "gibbmt" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[6]} ,
  {0, "giim" ,128,3,0,0,0,0,0,0,NULL,&atx[7],NULL,0,&atx[13]} ,
  {408, "Giimport-id" ,1,0,0,0,0,1,0,0,NULL,&atx[12],&atx[8],0,&atx[2]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[9]} ,
  {0, "db" ,128,1,0,1,0,0,0,0,NULL,&atx[10],NULL,0,&atx[11]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "release" ,128,2,0,1,0,0,0,0,NULL,&atx[10],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "genbank" ,128,4,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[19]} ,
  {415, "Textseq-id" ,1,0,0,0,0,0,0,0,NULL,&atx[12],&atx[15],0,&atx[23]} ,
  {0, "name" ,128,0,0,1,0,0,0,0,NULL,&atx[10],NULL,0,&atx[16]} ,
  {0, "accession" ,128,1,0,1,0,0,0,0,NULL,&atx[10],NULL,0,&atx[17]} ,
  {0, "release" ,128,2,0,1,0,0,0,0,NULL,&atx[10],NULL,0,&atx[18]} ,
  {0, "version" ,128,3,0,1,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "embl" ,128,5,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[20]} ,
  {0, "pir" ,128,6,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[21]} ,
  {0, "swissprot" ,128,7,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[22]} ,
  {0, "patent" ,128,8,0,0,0,0,0,0,NULL,&atx[23],NULL,0,&atx[27]} ,
  {416, "Patent-seq-id" ,1,0,0,0,0,0,0,0,NULL,&atx[12],&atx[24],0,&atx[34]} ,
  {0, "seqid" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[25]} ,
  {0, "cit" ,128,1,0,0,0,0,0,0,NULL,&atx[26],NULL,0,NULL} ,
  {413, "Id-pat" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[86]} ,
  {0, "other" ,128,9,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[28]} ,
  {0, "general" ,128,10,0,0,0,0,0,0,NULL,&atx[29],NULL,0,&atx[30]} ,
  {411, "Dbtag" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[39]} ,
  {0, "gi" ,128,11,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[31]} ,
  {0, "ddbj" ,128,12,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[32]} ,
  {0, "prf" ,128,13,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[33]} ,
  {0, "pdb" ,128,14,0,0,0,0,0,0,NULL,&atx[34],NULL,0,NULL} ,
  {417, "PDB-seq-id" ,1,0,0,0,0,0,0,0,NULL,&atx[12],&atx[35],0,&atx[36]} ,
  {0, "mol" ,128,0,0,0,0,0,0,0,NULL,&atx[36],NULL,0,&atx[37]} ,
  {418, "PDB-mol-id" ,1,0,0,0,0,0,0,0,NULL,&atx[10],NULL,0,&atx[75]} ,
  {0, "chain" ,128,1,0,0,1,0,0,0,&avnx[0],&atx[4],NULL,0,&atx[38]} ,
  {0, "rel" ,128,2,0,1,0,0,0,0,NULL,&atx[39],NULL,0,NULL} ,
  {412, "Date" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[26]} ,
  {315, "CHOICE" ,0,-1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {402, "Seq-loc" ,1,0,0,0,0,1,0,0,NULL,&atx[40],&atx[42],0,&atx[47]} ,
  {0, "null" ,128,0,0,0,0,0,0,0,NULL,&atx[43],NULL,0,&atx[44]} ,
  {305, "NULL" ,0,5,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "empty" ,128,1,0,0,0,0,0,0,NULL,&atx[0],NULL,0,&atx[45]} ,
  {0, "whole" ,128,2,0,0,0,0,0,0,NULL,&atx[0],NULL,0,&atx[46]} ,
  {0, "int" ,128,3,0,0,0,0,0,0,NULL,&atx[47],NULL,0,&atx[57]} ,
  {403, "Seq-interval" ,1,0,0,0,0,1,0,0,NULL,&atx[12],&atx[48],0,&atx[58]} ,
  {0, "from" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[49]} ,
  {0, "to" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[50]} ,
  {0, "strand" ,128,2,0,1,0,0,0,0,NULL,&atx[51],NULL,0,&atx[53]} ,
  {407, "Na-strand" ,1,0,0,0,0,1,0,0,NULL,&atx[52],&avnx[1],0,&atx[7]} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "id" ,128,3,0,0,0,0,0,0,NULL,&atx[0],NULL,0,&atx[54]} ,
  {0, "fuzz-from" ,128,4,0,1,0,0,0,0,NULL,&atx[55],NULL,0,&atx[56]} ,
  {410, "Int-fuzz" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[29]} ,
  {0, "fuzz-to" ,128,5,0,1,0,0,0,0,NULL,&atx[55],NULL,0,NULL} ,
  {0, "packed-int" ,128,4,0,0,0,0,0,0,NULL,&atx[58],NULL,0,&atx[61]} ,
  {404, "Packed-seqint" ,1,0,0,0,0,1,0,0,NULL,&atx[60],&atx[59],0,&atx[62]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[47],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "pnt" ,128,5,0,0,0,0,0,0,NULL,&atx[62],NULL,0,&atx[67]} ,
  {405, "Seq-point" ,1,0,0,0,0,1,0,0,NULL,&atx[12],&atx[63],0,&atx[68]} ,
  {0, "point" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[64]} ,
  {0, "strand" ,128,1,0,1,0,0,0,0,NULL,&atx[51],NULL,0,&atx[65]} ,
  {0, "id" ,128,2,0,0,0,0,0,0,NULL,&atx[0],NULL,0,&atx[66]} ,
  {0, "fuzz" ,128,3,0,1,0,0,0,0,NULL,&atx[55],NULL,0,NULL} ,
  {0, "packed-pnt" ,128,6,0,0,0,0,0,0,NULL,&atx[68],NULL,0,&atx[74]} ,
  {406, "Packed-seqpnt" ,1,0,0,0,0,1,0,0,NULL,&atx[12],&atx[69],0,&atx[51]} ,
  {0, "strand" ,128,0,0,1,0,0,0,0,NULL,&atx[51],NULL,0,&atx[70]} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[0],NULL,0,&atx[71]} ,
  {0, "fuzz" ,128,2,0,1,0,0,0,0,NULL,&atx[55],NULL,0,&atx[72]} ,
  {0, "points" ,128,3,0,0,0,0,0,0,NULL,&atx[60],&atx[73],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "mix" ,128,7,0,0,0,0,0,0,NULL,&atx[75],NULL,0,&atx[77]} ,
  {419, "Seq-loc-mix" ,1,0,0,0,0,0,0,0,NULL,&atx[60],&atx[76],0,&atx[78]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[41],NULL,0,NULL} ,
  {0, "equiv" ,128,8,0,0,0,0,0,0,NULL,&atx[78],NULL,0,&atx[81]} ,
  {420, "Seq-loc-equiv" ,1,0,0,0,0,0,0,0,NULL,&atx[80],&atx[79],0,&atx[82]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[41],NULL,0,NULL} ,
  {314, "SET OF" ,0,17,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "bond" ,128,9,0,0,0,0,0,0,NULL,&atx[82],NULL,0,&atx[85]} ,
  {421, "Seq-bond" ,1,0,0,0,0,0,0,0,NULL,&atx[12],&atx[83],0,NULL} ,
  {0, "a" ,128,0,0,0,0,0,0,0,NULL,&atx[62],NULL,0,&atx[84]} ,
  {0, "b" ,128,1,0,1,0,0,0,0,NULL,&atx[62],NULL,0,NULL} ,
  {0, "feat" ,128,10,0,0,0,0,0,0,NULL,&atx[86],NULL,0,NULL} ,
  {414, "Feat-id" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[14]} };

static AsnModule ampx[1] = {
  { "NCBI-Seqloc" , "asnloc.h00",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



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
