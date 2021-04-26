/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnsubmt.h00";
static AsnValxNode avnx[1] = {
    {2,NULL,0,0.0,NULL } };

static AsnType atx[37] = {
  {401, "Seq-submit" ,1,0,0,0,0,1,0,0,NULL,&atx[18],&atx[1],0,&atx[20]} ,
  {0, "sub" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[25]} ,
  {408, "Submit-block" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[3],0,&atx[4]} ,
  {0, "contact" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[19]} ,
  {409, "Contact-info" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[5],0,NULL} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[7]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "address" ,128,1,0,0,0,0,0,0,NULL,&atx[9],&atx[8],0,&atx[10]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "phone" ,128,2,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[11]} ,
  {0, "fax" ,128,3,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[12]} ,
  {0, "email" ,128,4,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[13]} ,
  {0, "telex" ,128,5,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[14]} ,
  {0, "owner-id" ,128,6,0,1,0,0,0,0,NULL,&atx[15],NULL,0,&atx[16]} ,
  {404, "Object-id" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[32]} ,
  {0, "password" ,128,7,0,1,0,0,0,0,NULL,&atx[17],NULL,0,NULL} ,
  {304, "OCTET STRING" ,0,4,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "cit" ,128,1,0,0,0,0,0,0,NULL,&atx[20],NULL,0,&atx[21]} ,
  {402, "Cit-sub" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[24]} ,
  {0, "hup" ,128,2,0,0,1,0,0,0,&avnx[0],&atx[22],NULL,0,&atx[23]} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "reldate" ,128,3,0,1,0,0,0,0,NULL,&atx[24],NULL,0,NULL} ,
  {403, "Date" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[15]} ,
  {0, "data" ,128,1,0,0,0,0,0,0,NULL,&atx[36],&atx[26],0,NULL} ,
  {0, "entrys" ,128,0,0,0,0,0,0,0,NULL,&atx[29],&atx[27],0,&atx[30]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[28],NULL,0,NULL} ,
  {407, "Seq-entry" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[2]} ,
  {314, "SET OF" ,0,17,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "annots" ,128,1,0,0,0,0,0,0,NULL,&atx[29],&atx[31],0,&atx[33]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[32],NULL,0,NULL} ,
  {405, "Seq-annot" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[35]} ,
  {0, "delete" ,128,2,0,0,0,0,0,0,NULL,&atx[29],&atx[34],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[35],NULL,0,NULL} ,
  {406, "Seq-id" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[28]} ,
  {315, "CHOICE" ,0,-1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "NCBI-Submit" , "asnsubmt.h00",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module NCBI-Submit
*
**************************************************/

#define SEQ_SUBMIT &at[0]
#define SEQ_SUBMIT_sub &at[1]
#define SEQ_SUBMIT_data &at[25]
#define SEQ_SUBMIT_data_entrys &at[26]
#define SEQ_SUBMIT_data_entrys_E &at[27]
#define SEQ_SUBMIT_data_annots &at[30]
#define SEQ_SUBMIT_data_annots_E &at[31]
#define SEQ_SUBMIT_data_delete &at[33]
#define SEQ_SUBMIT_data_delete_E &at[34]

#define SUBMIT_BLOCK &at[2]
#define SUBMIT_BLOCK_contact &at[3]
#define SUBMIT_BLOCK_cit &at[19]
#define SUBMIT_BLOCK_hup &at[21]
#define SUBMIT_BLOCK_reldate &at[23]

#define CONTACT_INFO &at[4]
#define CONTACT_INFO_name &at[5]
#define CONTACT_INFO_address &at[7]
#define CONTACT_INFO_address_E &at[8]
#define CONTACT_INFO_phone &at[10]
#define CONTACT_INFO_fax &at[11]
#define CONTACT_INFO_email &at[12]
#define CONTACT_INFO_telex &at[13]
#define CONTACT_INFO_owner_id &at[14]
#define CONTACT_INFO_password &at[16]
