/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asngen.h00";
static AsnValxNode avnx[6] = {
    {20,"unk" ,0,0.0,&avnx[1] } ,
    {20,"gt" ,1,0.0,&avnx[2] } ,
    {20,"lt" ,2,0.0,&avnx[3] } ,
    {20,"tr" ,3,0.0,&avnx[4] } ,
    {20,"tl" ,4,0.0,&avnx[5] } ,
    {20,"other" ,255,0.0,NULL } };

static AsnType atx[70] = {
  {401, "Date" ,1,0,0,0,0,1,0,0,NULL,&atx[11],&atx[1],0,&atx[12]} ,
  {0, "str" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[3]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "std" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {407, "Date-std" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[5],0,&atx[21]} ,
  {0, "year" ,128,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[7]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "month" ,128,1,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[8]} ,
  {0, "day" ,128,2,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[9]} ,
  {0, "season" ,128,3,0,1,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {315, "CHOICE" ,0,-1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {402, "Person-id" ,1,0,0,0,0,1,0,0,NULL,&atx[11],&atx[13],0,&atx[17]} ,
  {0, "dbtag" ,128,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[20]} ,
  {404, "Dbtag" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[15],0,&atx[31]} ,
  {0, "db" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[16]} ,
  {0, "tag" ,128,1,0,0,0,0,0,0,NULL,&atx[17],NULL,0,NULL} ,
  {403, "Object-id" ,1,0,0,0,0,1,0,0,NULL,&atx[11],&atx[18],0,&atx[14]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[19]} ,
  {0, "str" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "name" ,128,1,0,0,0,0,0,0,NULL,&atx[21],NULL,0,&atx[29]} ,
  {408, "Name-std" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[22],0,&atx[44]} ,
  {0, "last" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[23]} ,
  {0, "first" ,128,1,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[24]} ,
  {0, "middle" ,128,2,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[25]} ,
  {0, "full" ,128,3,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[26]} ,
  {0, "initials" ,128,4,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[27]} ,
  {0, "suffix" ,128,5,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[28]} ,
  {0, "title" ,128,6,0,1,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "ml" ,128,2,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[30]} ,
  {0, "str" ,128,3,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {405, "Int-fuzz" ,1,0,0,0,0,1,0,0,NULL,&atx[11],&atx[32],0,&atx[39]} ,
  {0, "p-m" ,128,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[33]} ,
  {0, "range" ,128,1,0,0,0,0,0,0,NULL,&atx[10],&atx[34],0,&atx[36]} ,
  {0, "max" ,128,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[35]} ,
  {0, "min" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {0, "pct" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[37]} ,
  {0, "lim" ,128,3,0,0,0,0,0,0,NULL,&atx[38],&avnx[0],0,NULL} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {406, "User-object" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[40],0,&atx[4]} ,
  {0, "class" ,128,0,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[41]} ,
  {0, "type" ,128,1,0,0,0,0,0,0,NULL,&atx[17],NULL,0,&atx[42]} ,
  {0, "data" ,128,2,0,0,0,0,0,0,NULL,&atx[59],&atx[43],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[44],NULL,0,NULL} ,
  {409, "User-field" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[45],0,NULL} ,
  {0, "label" ,128,0,0,0,0,0,0,0,NULL,&atx[17],NULL,0,&atx[46]} ,
  {0, "num" ,128,1,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[47]} ,
  {0, "data" ,128,2,0,0,0,0,0,0,NULL,&atx[11],&atx[48],0,NULL} ,
  {0, "str" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[49]} ,
  {0, "int" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[50]} ,
  {0, "real" ,128,2,0,0,0,0,0,0,NULL,&atx[51],NULL,0,&atx[52]} ,
  {309, "REAL" ,0,9,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "bool" ,128,3,0,0,0,0,0,0,NULL,&atx[53],NULL,0,&atx[54]} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "os" ,128,4,0,0,0,0,0,0,NULL,&atx[55],NULL,0,&atx[56]} ,
  {304, "OCTET STRING" ,0,4,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "object" ,128,5,0,0,0,0,0,0,NULL,&atx[39],NULL,0,&atx[57]} ,
  {0, "strs" ,128,6,0,0,0,0,0,0,NULL,&atx[59],&atx[58],0,&atx[60]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "ints" ,128,7,0,0,0,0,0,0,NULL,&atx[59],&atx[61],0,&atx[62]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {0, "reals" ,128,8,0,0,0,0,0,0,NULL,&atx[59],&atx[63],0,&atx[64]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[51],NULL,0,NULL} ,
  {0, "oss" ,128,9,0,0,0,0,0,0,NULL,&atx[59],&atx[65],0,&atx[66]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[55],NULL,0,NULL} ,
  {0, "fields" ,128,10,0,0,0,0,0,0,NULL,&atx[59],&atx[67],0,&atx[68]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[44],NULL,0,NULL} ,
  {0, "objects" ,128,11,0,0,0,0,0,0,NULL,&atx[59],&atx[69],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[39],NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "NCBI-General" , "asngen.h00",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



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
