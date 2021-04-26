/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnmla.h01";
static AsnValxNode avnx[14] = {
    {20,"not-set" ,0,0.0,&avnx[1] } ,
    {20,"name" ,1,0.0,&avnx[2] } ,
    {20,"tsub" ,2,0.0,&avnx[3] } ,
    {20,"trans" ,3,0.0,&avnx[4] } ,
    {20,"jta" ,4,0.0,&avnx[5] } ,
    {20,"iso-jta" ,5,0.0,&avnx[6] } ,
    {20,"ml-jta" ,6,0.0,&avnx[7] } ,
    {20,"coden" ,7,0.0,&avnx[8] } ,
    {20,"issn" ,8,0.0,&avnx[9] } ,
    {20,"abr" ,9,0.0,&avnx[10] } ,
    {20,"isbn" ,10,0.0,&avnx[11] } ,
    {20,"all" ,255,0.0,NULL } ,
    {20,"not-found" ,0,0.0,&avnx[13] } ,
    {20,"operational-error" ,1,0.0,NULL } };

static AsnType atx[33] = {
  {401, "Medline-entry" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[1]} ,
  {402, "Pub" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[2]} ,
  {403, "Title" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[3]} ,
  {404, "Mla-request" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[4],0,&atx[10]} ,
  {0, "init" ,128,0,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[6]} ,
  {305, "NULL" ,0,5,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "getmle" ,128,1,0,0,0,0,0,0,NULL,&atx[7],NULL,0,&atx[8]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "getpub" ,128,2,0,0,0,0,0,0,NULL,&atx[7],NULL,0,&atx[9]} ,
  {0, "gettitle" ,128,3,0,0,0,0,0,0,NULL,&atx[10],NULL,0,&atx[16]} ,
  {405, "Title-msg" ,1,0,0,0,0,0,0,0,NULL,&atx[15],&atx[11],0,&atx[12]} ,
  {0, "type" ,128,0,0,0,0,0,0,0,NULL,&atx[12],NULL,0,&atx[14]} ,
  {406, "Title-type" ,1,0,0,0,0,0,0,0,NULL,&atx[13],&avnx[0],0,&atx[19]} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "title" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "citmatch" ,128,4,0,0,0,0,0,0,NULL,&atx[1],NULL,0,&atx[17]} ,
  {0, "fini" ,128,5,0,0,0,0,0,0,NULL,&atx[5],NULL,0,NULL} ,
  {315, "CHOICE" ,0,-1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {407, "Title-msg-list" ,1,0,0,0,0,0,0,0,NULL,&atx[15],&atx[20],0,&atx[24]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[7],NULL,0,&atx[21]} ,
  {0, "titles" ,128,1,0,0,0,0,0,0,NULL,&atx[23],&atx[22],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[10],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {408, "Error-val" ,1,0,0,0,0,0,0,0,NULL,&atx[13],&avnx[12],0,&atx[25]} ,
  {409, "Mla-back" ,1,0,0,0,0,0,0,0,NULL,&atx[18],&atx[26],0,NULL} ,
  {0, "init" ,128,0,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[27]} ,
  {0, "error" ,128,1,0,0,0,0,0,0,NULL,&atx[24],NULL,0,&atx[28]} ,
  {0, "getmle" ,128,2,0,0,0,0,0,0,NULL,&atx[0],NULL,0,&atx[29]} ,
  {0, "getpub" ,128,3,0,0,0,0,0,0,NULL,&atx[1],NULL,0,&atx[30]} ,
  {0, "gettitle" ,128,4,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[31]} ,
  {0, "citmatch" ,128,5,0,0,0,0,0,0,NULL,&atx[7],NULL,0,&atx[32]} ,
  {0, "fini" ,128,6,0,0,0,0,0,0,NULL,&atx[5],NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "NCBI-MedArchive" , "asnmla.h01",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



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
