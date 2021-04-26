/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "ni_asn.h06";
static AsnType atx[116] = {
  {401, "SVC-Entry" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[1],0,&atx[18]} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[3]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "minvers" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[5]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "maxvers" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[6]} ,
  {0, "id" ,128,3,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[7]} ,
  {0, "priority" ,128,4,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[8]} ,
  {0, "group" ,128,5,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[9]} ,
  {0, "description" ,128,6,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[10]} ,
  {0, "types" ,128,7,0,1,0,0,0,0,NULL,&atx[12],&atx[11],0,&atx[13]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {314, "SET OF" ,0,17,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "priority-timeout" ,128,8,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[14]} ,
  {0, "priority-penalty" ,128,9,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[15]} ,
  {0, "encryption-supported" ,128,10,0,1,0,0,0,0,NULL,&atx[16],NULL,0,NULL} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {402, "RES-Entry" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[19],0,&atx[26]} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[20]} ,
  {0, "type" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[21]} ,
  {0, "minvers" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[22]} ,
  {0, "maxvers" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[23]} ,
  {0, "id" ,128,4,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[24]} ,
  {0, "group" ,128,5,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[25]} ,
  {0, "description" ,128,6,0,1,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {403, "Toolset" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[27],0,&atx[38]} ,
  {0, "host" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[28]} ,
  {0, "motd" ,128,1,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[29]} ,
  {0, "services" ,128,2,0,1,0,0,0,0,NULL,&atx[12],&atx[30],0,&atx[31]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[0],NULL,0,NULL} ,
  {0, "resources" ,128,3,0,1,0,0,0,0,NULL,&atx[12],&atx[32],0,&atx[33]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,NULL} ,
  {0, "regions" ,128,4,0,1,0,0,0,0,NULL,&atx[12],&atx[34],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[35],NULL,0,NULL} ,
  {416, "Region-Descr" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[36],0,&atx[59]} ,
  {0, "region-name" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[37]} ,
  {0, "priority-delta" ,128,1,0,1,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {404, "Identity" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[39],0,&atx[42]} ,
  {0, "username" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[40]} ,
  {0, "group" ,128,1,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[41]} ,
  {0, "domain" ,128,2,0,1,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {405, "Request" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[43],0,&atx[48]} ,
  {0, "address" ,128,0,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[44]} ,
  {0, "port" ,128,1,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[45]} ,
  {0, "svcentry" ,128,2,0,0,0,0,0,0,NULL,&atx[0],NULL,0,&atx[46]} ,
  {0, "resentry" ,128,3,0,1,0,0,0,0,NULL,&atx[12],&atx[47],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,NULL} ,
  {406, "MSG-ACK" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[49],0,&atx[64]} ,
  {0, "seqno" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[50]} ,
  {0, "disp-info" ,128,1,0,1,0,0,0,0,NULL,&atx[51],NULL,0,NULL} ,
  {418, "Dispatcher-Info" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[52],0,NULL} ,
  {0, "serial-no" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[53]} ,
  {0, "is-alternate-list" ,128,1,0,0,0,0,0,0,NULL,&atx[16],NULL,0,&atx[54]} ,
  {0, "num-dispatchers" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[55]} ,
  {0, "disp-list" ,128,3,0,0,0,0,0,0,NULL,&atx[57],&atx[56],0,&atx[58]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "pub-key" ,128,4,0,1,0,0,0,0,NULL,&atx[59],NULL,0,NULL} ,
  {417, "RSA-Pubkey" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&atx[60],0,&atx[51]} ,
  {0, "bits" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[61]} ,
  {0, "modulus" ,128,1,0,0,0,0,0,0,NULL,&atx[62],NULL,0,&atx[63]} ,
  {304, "OCTET STRING" ,0,4,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "exponent" ,128,2,0,0,0,0,0,0,NULL,&atx[62],NULL,0,NULL} ,
  {407, "MSG-NACK" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[65],0,&atx[69]} ,
  {0, "seqno" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[66]} ,
  {0, "code" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[67]} ,
  {0, "reason" ,128,2,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[68]} ,
  {0, "disp-info" ,128,3,0,1,0,0,0,0,NULL,&atx[51],NULL,0,NULL} ,
  {408, "MSG-Login" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[70],0,&atx[77]} ,
  {0, "seqno" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[71]} ,
  {0, "uid" ,128,1,0,0,0,0,0,0,NULL,&atx[38],NULL,0,&atx[72]} ,
  {0, "password" ,128,2,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[73]} ,
  {0, "disp-serial-no" ,128,3,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[74]} ,
  {0, "encryption-desired" ,128,4,0,1,0,0,0,0,NULL,&atx[16],NULL,0,&atx[75]} ,
  {0, "pub-key" ,128,5,0,1,0,0,0,0,NULL,&atx[59],NULL,0,&atx[76]} ,
  {0, "des-key" ,128,6,0,1,0,0,0,0,NULL,&atx[62],NULL,0,NULL} ,
  {409, "MSG-SVC-List" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[78],0,&atx[80]} ,
  {0, "seqno" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[79]} ,
  {0, "toollist" ,128,1,0,0,0,0,0,0,NULL,&atx[26],NULL,0,NULL} ,
  {410, "MSG-SVC-Request" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[81],0,&atx[88]} ,
  {0, "seqno" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[82]} ,
  {0, "conid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[83]} ,
  {0, "uid" ,128,2,0,0,0,0,0,0,NULL,&atx[38],NULL,0,&atx[84]} ,
  {0, "request" ,128,3,0,0,0,0,0,0,NULL,&atx[42],NULL,0,&atx[85]} ,
  {0, "platform" ,128,4,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[86]} ,
  {0, "appl-id" ,128,5,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[87]} ,
  {0, "des-key" ,128,6,0,1,0,0,0,0,NULL,&atx[62],NULL,0,NULL} ,
  {411, "MSG-SVC-Response" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[89],0,&atx[91]} ,
  {0, "seqno" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[90]} ,
  {0, "request" ,128,1,0,0,0,0,0,0,NULL,&atx[42],NULL,0,NULL} ,
  {412, "MSG-Cmd" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[92],0,&atx[94]} ,
  {0, "seqno" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[93]} ,
  {0, "command" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {413, "MSG-Acct" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[95],0,&atx[100]} ,
  {0, "seqno" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[96]} ,
  {0, "conid" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[97]} ,
  {0, "jobname" ,128,2,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[98]} ,
  {0, "usertime" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[99]} ,
  {0, "systemtime" ,128,4,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {414, "MSG-Catalog" ,1,0,0,0,0,1,0,0,NULL,&atx[17],&atx[101],0,&atx[105]} ,
  {0, "seqno" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[102]} ,
  {0, "motd" ,128,1,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[103]} ,
  {0, "toollists" ,128,2,0,1,0,0,0,0,NULL,&atx[12],&atx[104],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[26],NULL,0,NULL} ,
  {415, "Message" ,1,0,0,0,0,1,0,0,NULL,&atx[115],&atx[106],0,&atx[35]} ,
  {0, "ack" ,128,0,0,0,0,0,0,0,NULL,&atx[48],NULL,0,&atx[107]} ,
  {0, "nack" ,128,1,0,0,0,0,0,0,NULL,&atx[64],NULL,0,&atx[108]} ,
  {0, "login" ,128,2,0,0,0,0,0,0,NULL,&atx[69],NULL,0,&atx[109]} ,
  {0, "svc-list" ,128,3,0,0,0,0,0,0,NULL,&atx[77],NULL,0,&atx[110]} ,
  {0, "svc-request" ,128,4,0,0,0,0,0,0,NULL,&atx[80],NULL,0,&atx[111]} ,
  {0, "svc-response" ,128,5,0,0,0,0,0,0,NULL,&atx[88],NULL,0,&atx[112]} ,
  {0, "command" ,128,6,0,0,0,0,0,0,NULL,&atx[91],NULL,0,&atx[113]} ,
  {0, "acct" ,128,7,0,0,0,0,0,0,NULL,&atx[94],NULL,0,&atx[114]} ,
  {0, "catalog" ,128,8,0,0,0,0,0,0,NULL,&atx[100],NULL,0,NULL} ,
  {315, "CHOICE" ,0,-1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "NCBI-MESSAGE" , "ni_asn.h06",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = NULL;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



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
