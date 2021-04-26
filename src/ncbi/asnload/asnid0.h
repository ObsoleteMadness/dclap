/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnid0.l01";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module NCBI-ID0Access
*
**************************************************/

#define ID0SERVER_REQUEST &at[2]
#define ID0SERVER_REQUEST_init &at[3]
#define ID0SERVER_REQUEST_getgi &at[5]
#define ID0SERVER_REQUEST_getsefromgi &at[6]
#define ID0SERVER_REQUEST_fini &at[14]

#define ID0SERVER_MAXCOMPLEX &at[7]
#define ID0SERVER_MAXCOMPLEX_maxplex &at[8]
#define ID0SERVER_MAXCOMPLEX_gi &at[11]

#define ENTRY_COMPLEXITIES &at[9]

#define ID0SERVER_BACK &at[16]
#define ID0SERVER_BACK_init &at[17]
#define ID0SERVER_BACK_error &at[18]
#define ID0SERVER_BACK_gotgi &at[19]
#define ID0SERVER_BACK_gotseqentry &at[20]
#define ID0SERVER_BACK_gotdeadseqentry &at[21]
#define ID0SERVER_BACK_fini &at[22]

#define ID0SERVER_DEBUG &at[23]
#define ID0SERVER_DEBUG_E &at[24]
