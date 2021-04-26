/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnmla.l01";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module NCBI-MedArchive
*
**************************************************/

#define MLA_REQUEST &at[3]
#define MLA_REQUEST_init &at[4]
#define MLA_REQUEST_getmle &at[6]
#define MLA_REQUEST_getpub &at[8]
#define MLA_REQUEST_gettitle &at[9]
#define MLA_REQUEST_citmatch &at[16]
#define MLA_REQUEST_fini &at[17]

#define TITLE_MSG &at[10]
#define TITLE_MSG_type &at[11]
#define TITLE_MSG_title &at[14]

#define TITLE_TYPE &at[12]

#define TITLE_MSG_LIST &at[19]
#define TITLE_MSG_LIST_num &at[20]
#define TITLE_MSG_LIST_titles &at[21]
#define TITLE_MSG_LIST_titles_E &at[22]

#define ERROR_VAL &at[24]

#define MLA_BACK &at[25]
#define MLA_BACK_init &at[26]
#define MLA_BACK_error &at[27]
#define MLA_BACK_getmle &at[28]
#define MLA_BACK_getpub &at[29]
#define MLA_BACK_gettitle &at[30]
#define MLA_BACK_citmatch &at[31]
#define MLA_BACK_fini &at[32]
