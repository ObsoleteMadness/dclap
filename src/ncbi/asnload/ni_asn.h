/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "ni_asn.l06";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module NCBI-MESSAGE
*
**************************************************/

#define SVC_ENTRY &at[0]
#define SVC_ENTRY_name &at[1]
#define SVC_ENTRY_minvers &at[3]
#define SVC_ENTRY_maxvers &at[5]
#define SVC_ENTRY_id &at[6]
#define SVC_ENTRY_priority &at[7]
#define SVC_ENTRY_group &at[8]
#define SVC_ENTRY_description &at[9]
#define SVC_ENTRY_types &at[10]
#define SVC_ENTRY_types_E &at[11]
#define SVC_ENTRY_priority_timeout &at[13]
#define SVC_ENTRY_priority_penalty &at[14]
#define SVC_ENTRY_encryption_supported &at[15]

#define RES_ENTRY &at[18]
#define RES_ENTRY_name &at[19]
#define RES_ENTRY_type &at[20]
#define RES_ENTRY_minvers &at[21]
#define RES_ENTRY_maxvers &at[22]
#define RES_ENTRY_id &at[23]
#define RES_ENTRY_group &at[24]
#define RES_ENTRY_description &at[25]

#define TOOLSET &at[26]
#define TOOLSET_host &at[27]
#define TOOLSET_motd &at[28]
#define TOOLSET_services &at[29]
#define TOOLSET_services_E &at[30]
#define TOOLSET_resources &at[31]
#define TOOLSET_resources_E &at[32]
#define TOOLSET_regions &at[33]
#define TOOLSET_regions_E &at[34]

#define IDENTITY &at[38]
#define IDENTITY_username &at[39]
#define IDENTITY_group &at[40]
#define IDENTITY_domain &at[41]

#define REQUEST &at[42]
#define REQUEST_address &at[43]
#define REQUEST_port &at[44]
#define REQUEST_svcentry &at[45]
#define REQUEST_resentry &at[46]
#define REQUEST_resentry_E &at[47]

#define MSG_ACK &at[48]
#define MSG_ACK_seqno &at[49]
#define MSG_ACK_disp_info &at[50]

#define MSG_NACK &at[64]
#define MSG_NACK_seqno &at[65]
#define MSG_NACK_code &at[66]
#define MSG_NACK_reason &at[67]
#define MSG_NACK_disp_info &at[68]

#define MSG_LOGIN &at[69]
#define MSG_LOGIN_seqno &at[70]
#define MSG_LOGIN_uid &at[71]
#define MSG_LOGIN_password &at[72]
#define MSG_LOGIN_disp_serial_no &at[73]
#define MSG_LOGIN_encryption_desired &at[74]
#define MSG_LOGIN_pub_key &at[75]
#define MSG_LOGIN_des_key &at[76]

#define MSG_SVC_LIST &at[77]
#define MSG_SVC_LIST_seqno &at[78]
#define MSG_SVC_LIST_toollist &at[79]

#define MSG_SVC_REQUEST &at[80]
#define MSG_SVC_REQUEST_seqno &at[81]
#define MSG_SVC_REQUEST_conid &at[82]
#define MSG_SVC_REQUEST_uid &at[83]
#define MSG_SVC_REQUEST_request &at[84]
#define MSG_SVC_REQUEST_platform &at[85]
#define MSG_SVC_REQUEST_appl_id &at[86]
#define MSG_SVC_REQUEST_des_key &at[87]

#define MSG_SVC_RESPONSE &at[88]
#define MSG_SVC_RESPONSE_seqno &at[89]
#define MSG_SVC_RESPONSE_request &at[90]

#define MSG_CMD &at[91]
#define MSG_CMD_seqno &at[92]
#define MSG_CMD_command &at[93]

#define MSG_ACCT &at[94]
#define MSG_ACCT_seqno &at[95]
#define MSG_ACCT_conid &at[96]
#define MSG_ACCT_jobname &at[97]
#define MSG_ACCT_usertime &at[98]
#define MSG_ACCT_systemtime &at[99]

#define MSG_CATALOG &at[100]
#define MSG_CATALOG_seqno &at[101]
#define MSG_CATALOG_motd &at[102]
#define MSG_CATALOG_toollists &at[103]
#define MSG_CATALOG_toollists_E &at[104]

#define MESSAGE &at[105]
#define MESSAGE_ack &at[106]
#define MESSAGE_nack &at[107]
#define MESSAGE_login &at[108]
#define MESSAGE_svc_list &at[109]
#define MESSAGE_svc_request &at[110]
#define MESSAGE_svc_response &at[111]
#define MESSAGE_command &at[112]
#define MESSAGE_acct &at[113]
#define MESSAGE_catalog &at[114]

#define REGION_DESCR &at[35]
#define REGION_DESCR_region_name &at[36]
#define REGION_DESCR_priority_delta &at[37]

#define RSA_PUBKEY &at[59]
#define RSA_PUBKEY_bits &at[60]
#define RSA_PUBKEY_modulus &at[61]
#define RSA_PUBKEY_exponent &at[63]

#define DISPATCHER_INFO &at[51]
#define DISPATCHER_INFO_serial_no &at[52]
#define INFO_is_alternate_list &at[53]
#define DISPATCHER_INFO_num_dispatchers &at[54]
#define DISPATCHER_INFO_disp_list &at[55]
#define DISPATCHER_INFO_disp_list_E &at[56]
#define DISPATCHER_INFO_pub_key &at[58]
