/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnalign.h01";
static AsnValxNode avnx[8] = {
    {20,"not-set" ,0,0.0,&avnx[1] } ,
    {20,"global" ,1,0.0,&avnx[2] } ,
    {20,"diags" ,2,0.0,&avnx[3] } ,
    {20,"partial" ,3,0.0,&avnx[4] } ,
    {20,"other" ,255,0.0,NULL } ,
    {3,NULL,2,0.0,NULL } ,
    {3,NULL,2,0.0,NULL } ,
    {3,NULL,2,0.0,NULL } };

static AsnType atx[59] = {
  {401, "Seq-align" ,1,0,0,0,0,1,0,0,NULL,&atx[15],&atx[1],0,&atx[7]} ,
  {0, "type" ,128,0,0,0,0,0,0,0,NULL,&atx[2],&avnx[0],0,&atx[3]} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "dim" ,128,1,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[5]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "score" ,128,2,0,1,0,0,0,0,NULL,&atx[16],&atx[6],0,&atx[17]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[7],NULL,0,NULL} ,
  {402, "Score" ,1,0,0,0,0,1,0,0,NULL,&atx[15],&atx[8],0,&atx[24]} ,
  {0, "id" ,128,0,0,1,0,0,0,0,NULL,&atx[9],NULL,0,&atx[10]} ,
  {406, "Object-id" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[20]} ,
  {0, "value" ,128,1,0,0,0,0,0,0,NULL,&atx[14],&atx[11],0,NULL} ,
  {0, "real" ,128,0,0,0,0,0,0,0,NULL,&atx[12],NULL,0,&atx[13]} ,
  {309, "REAL" ,0,9,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "int" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {315, "CHOICE" ,0,-1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {314, "SET OF" ,0,17,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "segs" ,128,3,0,0,0,0,0,0,NULL,&atx[14],&atx[18],0,NULL} ,
  {0, "dendiag" ,128,0,0,0,0,0,0,0,NULL,&atx[25],&atx[19],0,&atx[34]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[20],NULL,0,NULL} ,
  {407, "Dense-diag" ,1,0,0,0,0,0,0,0,NULL,&atx[15],&atx[21],0,&atx[35]} ,
  {0, "dim" ,128,0,0,0,1,0,0,0,&avnx[5],&atx[4],NULL,0,&atx[22]} ,
  {0, "ids" ,128,1,0,0,0,0,0,0,NULL,&atx[25],&atx[23],0,&atx[26]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[24],NULL,0,NULL} ,
  {403, "Seq-id" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[56]} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "starts" ,128,2,0,0,0,0,0,0,NULL,&atx[25],&atx[27],0,&atx[28]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "len" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[29]} ,
  {0, "strands" ,128,4,0,1,0,0,0,0,NULL,&atx[25],&atx[30],0,&atx[32]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[31],NULL,0,NULL} ,
  {405, "Na-strand" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[9]} ,
  {0, "scores" ,128,5,0,1,0,0,0,0,NULL,&atx[16],&atx[33],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[7],NULL,0,NULL} ,
  {0, "denseg" ,128,1,0,0,0,0,0,0,NULL,&atx[35],NULL,0,&atx[48]} ,
  {408, "Dense-seg" ,1,0,0,0,0,0,0,0,NULL,&atx[15],&atx[36],0,&atx[50]} ,
  {0, "dim" ,128,0,0,0,1,0,0,0,&avnx[6],&atx[4],NULL,0,&atx[37]} ,
  {0, "numseg" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[38]} ,
  {0, "ids" ,128,2,0,0,0,0,0,0,NULL,&atx[25],&atx[39],0,&atx[40]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[24],NULL,0,NULL} ,
  {0, "starts" ,128,3,0,0,0,0,0,0,NULL,&atx[25],&atx[41],0,&atx[42]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "lens" ,128,4,0,0,0,0,0,0,NULL,&atx[25],&atx[43],0,&atx[44]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "strands" ,128,5,0,1,0,0,0,0,NULL,&atx[25],&atx[45],0,&atx[46]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[31],NULL,0,NULL} ,
  {0, "scores" ,128,6,0,1,0,0,0,0,NULL,&atx[25],&atx[47],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[7],NULL,0,NULL} ,
  {0, "std" ,128,2,0,0,0,0,0,0,NULL,&atx[25],&atx[49],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[50],NULL,0,NULL} ,
  {409, "Std-seg" ,1,0,0,0,0,0,0,0,NULL,&atx[15],&atx[51],0,NULL} ,
  {0, "dim" ,128,0,0,0,1,0,0,0,&avnx[7],&atx[4],NULL,0,&atx[52]} ,
  {0, "ids" ,128,1,0,1,0,0,0,0,NULL,&atx[25],&atx[53],0,&atx[54]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[24],NULL,0,NULL} ,
  {0, "loc" ,128,2,0,0,0,0,0,0,NULL,&atx[25],&atx[55],0,&atx[57]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[56],NULL,0,NULL} ,
  {404, "Seq-loc" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[31]} ,
  {0, "scores" ,128,3,0,1,0,0,0,0,NULL,&atx[16],&atx[58],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[7],NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "NCBI-Seqalign" , "asnalign.h01",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module NCBI-Seqalign
*
**************************************************/

#define SEQ_ALIGN &at[0]
#define SEQ_ALIGN_type &at[1]
#define SEQ_ALIGN_dim &at[3]
#define SEQ_ALIGN_score &at[5]
#define SEQ_ALIGN_score_E &at[6]
#define SEQ_ALIGN_segs &at[17]
#define SEQ_ALIGN_segs_dendiag &at[18]
#define SEQ_ALIGN_segs_dendiag_E &at[19]
#define SEQ_ALIGN_segs_denseg &at[34]
#define SEQ_ALIGN_segs_std &at[48]
#define SEQ_ALIGN_segs_std_E &at[49]

#define SCORE &at[7]
#define SCORE_id &at[8]
#define SCORE_value &at[10]
#define SCORE_value_real &at[11]
#define SCORE_value_int &at[13]

#define DENSE_DIAG &at[20]
#define DENSE_DIAG_dim &at[21]
#define DENSE_DIAG_ids &at[22]
#define DENSE_DIAG_ids_E &at[23]
#define DENSE_DIAG_starts &at[26]
#define DENSE_DIAG_starts_E &at[27]
#define DENSE_DIAG_len &at[28]
#define DENSE_DIAG_strands &at[29]
#define DENSE_DIAG_strands_E &at[30]
#define DENSE_DIAG_scores &at[32]
#define DENSE_DIAG_scores_E &at[33]

#define DENSE_SEG &at[35]
#define DENSE_SEG_dim &at[36]
#define DENSE_SEG_numseg &at[37]
#define DENSE_SEG_ids &at[38]
#define DENSE_SEG_ids_E &at[39]
#define DENSE_SEG_starts &at[40]
#define DENSE_SEG_starts_E &at[41]
#define DENSE_SEG_lens &at[42]
#define DENSE_SEG_lens_E &at[43]
#define DENSE_SEG_strands &at[44]
#define DENSE_SEG_strands_E &at[45]
#define DENSE_SEG_scores &at[46]
#define DENSE_SEG_scores_E &at[47]

#define STD_SEG &at[50]
#define STD_SEG_dim &at[51]
#define STD_SEG_ids &at[52]
#define STD_SEG_ids_E &at[53]
#define STD_SEG_loc &at[54]
#define STD_SEG_loc_E &at[55]
#define STD_SEG_scores &at[57]
#define STD_SEG_scores_E &at[58]
