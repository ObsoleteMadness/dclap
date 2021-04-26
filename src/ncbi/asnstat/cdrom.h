/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "cdrom.h00";
static AsnValxNode avnx[6] = {
    {2,NULL,0,0.0,NULL } ,
    {2,NULL,0,0.0,NULL } ,
    {2,NULL,0,0.0,NULL } ,
    {20,"medline" ,0,0.0,&avnx[4] } ,
    {20,"aa" ,1,0.0,&avnx[5] } ,
    {20,"nt" ,2,0.0,NULL } };

static AsnType atx[48] = {
  {401, "Cdrom-inf" ,1,0,0,0,0,1,0,0,NULL,&atx[37],&atx[1],0,&atx[26]} ,
  {0, "volume-label" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[3]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "version" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[5]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "issue" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[6]} ,
  {0, "format" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[7]} ,
  {0, "descr" ,128,4,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[8]} ,
  {0, "no-compression" ,128,5,0,0,0,0,0,0,NULL,&atx[9],NULL,0,&atx[10]} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "huff-count" ,128,6,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[11]} ,
  {0, "huff-left" ,128,7,0,0,0,0,0,0,NULL,&atx[13],&atx[12],0,&atx[14]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "huff-right" ,128,8,0,0,0,0,0,0,NULL,&atx[13],&atx[15],0,&atx[16]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "type-count" ,128,9,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[17]} ,
  {0, "type-names" ,128,10,0,0,0,0,0,0,NULL,&atx[13],&atx[18],0,&atx[19]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "type-bucket-size" ,128,11,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[20]} ,
  {0, "field-count" ,128,12,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[21]} ,
  {0, "field-names" ,128,13,0,0,0,0,0,0,NULL,&atx[13],&atx[22],0,&atx[23]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "field-bucket-size" ,128,14,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[24]} ,
  {0, "types" ,128,15,0,0,0,0,0,0,NULL,&atx[13],&atx[25],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[26],NULL,0,NULL} ,
  {402, "Typedata" ,1,0,0,0,0,0,0,0,NULL,&atx[37],&atx[27],0,&atx[34]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[28]} ,
  {0, "num-uids" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[29]} ,
  {0, "minuid" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[30]} ,
  {0, "maxuid" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[31]} ,
  {0, "num-bucket" ,128,4,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[32]} ,
  {0, "fields" ,128,5,0,0,0,0,0,0,NULL,&atx[13],&atx[33],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[34],NULL,0,NULL} ,
  {403, "Fielddata" ,1,0,0,0,0,0,0,0,NULL,&atx[37],&atx[35],0,&atx[38]} ,
  {0, "num-terms" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[36]} ,
  {0, "num-bucket" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {404, "Docsum-set" ,1,0,0,0,0,0,0,0,NULL,&atx[13],&atx[39],0,&atx[40]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[40],NULL,0,NULL} ,
  {405, "Docsum" ,1,0,0,0,0,0,0,0,NULL,&atx[37],&atx[41],0,&atx[46]} ,
  {0, "no-abstract" ,128,0,0,0,1,0,0,0,&avnx[0],&atx[9],NULL,0,&atx[42]} ,
  {0, "translated-title" ,128,1,0,0,1,0,0,0,&avnx[1],&atx[9],NULL,0,&atx[43]} ,
  {0, "no-authors" ,128,2,0,0,1,0,0,0,&avnx[2],&atx[9],NULL,0,&atx[44]} ,
  {0, "caption" ,128,3,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[45]} ,
  {0, "title" ,128,4,0,1,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {406, "Doc-type" ,1,0,0,0,0,0,0,0,NULL,&atx[47],&avnx[3],0,NULL} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "NCBI-CdRom" , "cdrom.h00",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module NCBI-CdRom
*
**************************************************/

#define CDROM_INF &at[0]
#define CDROM_INF_volume_label &at[1]
#define CDROM_INF_version &at[3]
#define CDROM_INF_issue &at[5]
#define CDROM_INF_format &at[6]
#define CDROM_INF_descr &at[7]
#define CDROM_INF_no_compression &at[8]
#define CDROM_INF_huff_count &at[10]
#define CDROM_INF_huff_left &at[11]
#define CDROM_INF_huff_left_E &at[12]
#define CDROM_INF_huff_right &at[14]
#define CDROM_INF_huff_right_E &at[15]
#define CDROM_INF_type_count &at[16]
#define CDROM_INF_type_names &at[17]
#define CDROM_INF_type_names_E &at[18]
#define CDROM_INF_type_bucket_size &at[19]
#define CDROM_INF_field_count &at[20]
#define CDROM_INF_field_names &at[21]
#define CDROM_INF_field_names_E &at[22]
#define CDROM_INF_field_bucket_size &at[23]
#define CDROM_INF_types &at[24]
#define CDROM_INF_types_E &at[25]

#define TYPEDATA &at[26]
#define TYPEDATA_num &at[27]
#define TYPEDATA_num_uids &at[28]
#define TYPEDATA_minuid &at[29]
#define TYPEDATA_maxuid &at[30]
#define TYPEDATA_num_bucket &at[31]
#define TYPEDATA_fields &at[32]
#define TYPEDATA_fields_E &at[33]

#define FIELDDATA &at[34]
#define FIELDDATA_num_terms &at[35]
#define FIELDDATA_num_bucket &at[36]

#define DOCSUM_SET &at[38]
#define DOCSUM_SET_E &at[39]

#define DOCSUM &at[40]
#define DOCSUM_no_abstract &at[41]
#define DOCSUM_translated_title &at[42]
#define DOCSUM_no_authors &at[43]
#define DOCSUM_caption &at[44]
#define DOCSUM_title &at[45]

#define DOC_TYPE &at[46]
