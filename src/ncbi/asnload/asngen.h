/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asngen.l00";
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
