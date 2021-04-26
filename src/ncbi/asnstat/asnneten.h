/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnneten.h03";
static AsnValxNode avnx[29] = {
    {20,"medline" ,0,0.0,&avnx[1] } ,
    {20,"aa" ,1,0.0,&avnx[2] } ,
    {20,"na" ,2,0.0,NULL } ,
    {20,"lparen" ,1,0.0,&avnx[4] } ,
    {20,"rparen" ,2,0.0,&avnx[5] } ,
    {20,"andsymbl" ,3,0.0,&avnx[6] } ,
    {20,"orsymbl" ,4,0.0,&avnx[7] } ,
    {20,"butnotsymbl" ,5,0.0,NULL } ,
    {20,"word" ,0,0.0,&avnx[9] } ,
    {20,"mesh" ,1,0.0,&avnx[10] } ,
    {20,"keyword" ,2,0.0,&avnx[11] } ,
    {20,"author" ,3,0.0,&avnx[12] } ,
    {20,"journal" ,4,0.0,&avnx[13] } ,
    {20,"org" ,5,0.0,&avnx[14] } ,
    {20,"accn" ,6,0.0,&avnx[15] } ,
    {20,"gene" ,7,0.0,&avnx[16] } ,
    {20,"prot" ,8,0.0,&avnx[17] } ,
    {20,"ecno" ,9,0.0,&avnx[18] } ,
    {20,"glossary" ,10,0.0,NULL } ,
    {2,NULL,0,0.0,NULL } ,
    {20,"entry" ,0,0.0,&avnx[21] } ,
    {20,"bioseq" ,1,0.0,&avnx[22] } ,
    {20,"bioseq-set" ,2,0.0,&avnx[23] } ,
    {20,"nuc-prot" ,3,0.0,&avnx[24] } ,
    {20,"pub-set" ,4,0.0,NULL } ,
    {3,NULL,0,0.0,NULL } ,
    {2,NULL,0,0.0,NULL } ,
    {2,NULL,0,0.0,NULL } ,
    {2,NULL,0,0.0,NULL } };

static AsnType atx[154] = {
  {401, "Seq-entry" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[1]} ,
  {402, "Medline-entry" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[2]} ,
  {403, "Link-set" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[3]} ,
  {404, "Cdrom-inf" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[4]} ,
  {405, "Entrez-request" ,1,0,0,0,0,0,0,0,NULL,&atx[34],&atx[5],0,&atx[12]} ,
  {0, "init" ,128,0,0,0,0,0,0,0,NULL,&atx[8],&atx[6],0,&atx[9]} ,
  {0, "version" ,128,0,0,1,0,0,0,0,NULL,&atx[7],NULL,0,NULL} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "maxlinks" ,128,1,0,0,0,0,0,0,NULL,&atx[10],NULL,0,&atx[11]} ,
  {305, "NULL" ,0,5,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "eval" ,128,2,0,0,0,0,0,0,NULL,&atx[12],NULL,0,&atx[36]} ,
  {406, "Entrez-termget" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[13],0,&atx[37]} ,
  {0, "max" ,128,0,0,1,0,0,0,0,NULL,&atx[14],NULL,0,&atx[15]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "cls" ,128,1,0,0,0,0,0,0,NULL,&atx[16],NULL,0,&atx[18]} ,
  {414, "Entrez-class" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&avnx[0],0,&atx[42]} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "terms" ,128,2,0,0,0,0,0,0,NULL,&atx[19],NULL,0,NULL} ,
  {416, "Entrez-term-list" ,1,0,0,0,0,0,0,0,NULL,&atx[35],&atx[20],0,&atx[27]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[34],&atx[21],0,NULL} ,
  {0, "operator" ,128,0,0,0,0,0,0,0,NULL,&atx[22],NULL,0,&atx[23]} ,
  {418, "Entrez-operator" ,1,0,0,0,0,0,0,0,NULL,&atx[17],&avnx[3],0,&atx[24]} ,
  {0, "sp-operand" ,128,1,0,0,0,0,0,0,NULL,&atx[24],NULL,0,&atx[29]} ,
  {419, "Special-operand" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[25],0,&atx[30]} ,
  {0, "term" ,128,0,0,0,0,0,0,0,NULL,&atx[7],NULL,0,&atx[26]} ,
  {0, "fld" ,128,1,0,0,0,0,0,0,NULL,&atx[27],NULL,0,&atx[28]} ,
  {417, "Entrez-field" ,1,0,0,0,0,0,0,0,NULL,&atx[14],&avnx[8],0,&atx[22]} ,
  {0, "type" ,128,2,0,0,0,0,0,0,NULL,&atx[16],NULL,0,NULL} ,
  {0, "tot-operand" ,128,2,0,0,0,0,0,0,NULL,&atx[30],NULL,0,NULL} ,
  {420, "Total-operand" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[31],0,&atx[87]} ,
  {0, "term" ,128,0,0,0,0,0,0,0,NULL,&atx[7],NULL,0,&atx[32]} ,
  {0, "fld" ,128,1,0,0,0,0,0,0,NULL,&atx[27],NULL,0,&atx[33]} ,
  {0, "type" ,128,2,0,0,0,0,0,0,NULL,&atx[16],NULL,0,NULL} ,
  {315, "CHOICE" ,0,-1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "docsum" ,128,3,0,0,0,0,0,0,NULL,&atx[37],NULL,0,&atx[47]} ,
  {407, "Entrez-docget" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[38],0,&atx[48]} ,
  {0, "class" ,128,0,0,0,0,0,0,0,NULL,&atx[16],NULL,0,&atx[39]} ,
  {0, "mark-missing" ,128,1,0,1,0,0,0,0,NULL,&atx[40],NULL,0,&atx[41]} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "ids" ,128,2,0,0,0,0,0,0,NULL,&atx[42],NULL,0,&atx[46]} ,
  {415, "Entrez-ids" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[43],0,&atx[19]} ,
  {0, "numid" ,128,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[44]} ,
  {0, "ids" ,128,1,0,0,0,0,0,0,NULL,&atx[35],&atx[45],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,NULL} ,
  {0, "defer-count" ,128,3,0,1,0,0,0,0,NULL,&atx[14],NULL,0,NULL} ,
  {0, "linkuidlist" ,128,4,0,0,0,0,0,0,NULL,&atx[48],NULL,0,&atx[56]} ,
  {408, "Link-setget" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[49],0,&atx[58]} ,
  {0, "max" ,128,0,0,1,0,0,0,0,NULL,&atx[14],NULL,0,&atx[50]} ,
  {0, "link-to-cls" ,128,1,0,0,0,0,0,0,NULL,&atx[16],NULL,0,&atx[51]} ,
  {0, "query-cls" ,128,2,0,0,0,0,0,0,NULL,&atx[16],NULL,0,&atx[52]} ,
  {0, "mark-missing" ,128,3,0,0,1,0,0,0,&avnx[19],&atx[40],NULL,0,&atx[53]} ,
  {0, "query-size" ,128,4,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[54]} ,
  {0, "query" ,128,5,0,0,0,0,0,0,NULL,&atx[35],&atx[55],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,NULL} ,
  {0, "uidlinks" ,128,5,0,0,0,0,0,0,NULL,&atx[48],NULL,0,&atx[57]} ,
  {0, "byterm" ,128,6,0,0,0,0,0,0,NULL,&atx[58],NULL,0,&atx[63]} ,
  {409, "Entrez-term-by-term" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[59],0,&atx[64]} ,
  {0, "type" ,128,0,0,0,0,0,0,0,NULL,&atx[16],NULL,0,&atx[60]} ,
  {0, "fld" ,128,1,0,0,0,0,0,0,NULL,&atx[27],NULL,0,&atx[61]} ,
  {0, "term" ,128,2,0,0,0,0,0,0,NULL,&atx[7],NULL,0,&atx[62]} ,
  {0, "num-terms" ,128,3,0,0,0,0,0,0,NULL,&atx[14],NULL,0,NULL} ,
  {0, "bypage" ,128,7,0,0,0,0,0,0,NULL,&atx[64],NULL,0,&atx[69]} ,
  {410, "Entrez-term-by-page" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[65],0,&atx[70]} ,
  {0, "type" ,128,0,0,0,0,0,0,0,NULL,&atx[16],NULL,0,&atx[66]} ,
  {0, "fld" ,128,1,0,0,0,0,0,0,NULL,&atx[27],NULL,0,&atx[67]} ,
  {0, "page" ,128,2,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[68]} ,
  {0, "num-pages" ,128,3,0,0,0,0,0,0,NULL,&atx[14],NULL,0,NULL} ,
  {0, "findterm" ,128,8,0,0,0,0,0,0,NULL,&atx[70],NULL,0,&atx[74]} ,
  {411, "Term-lookup" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[71],0,&atx[76]} ,
  {0, "type" ,128,0,0,0,0,0,0,0,NULL,&atx[16],NULL,0,&atx[72]} ,
  {0, "fld" ,128,1,0,0,0,0,0,0,NULL,&atx[27],NULL,0,&atx[73]} ,
  {0, "term" ,128,2,0,0,0,0,0,0,NULL,&atx[7],NULL,0,NULL} ,
  {0, "fini" ,128,9,0,0,0,0,0,0,NULL,&atx[10],NULL,0,&atx[75]} ,
  {0, "createnamed" ,128,10,0,0,0,0,0,0,NULL,&atx[76],NULL,0,&atx[81]} ,
  {412, "Entrez-named-list" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[77],0,&atx[83]} ,
  {0, "term" ,128,0,0,0,0,0,0,0,NULL,&atx[7],NULL,0,&atx[78]} ,
  {0, "type" ,128,1,0,0,0,0,0,0,NULL,&atx[16],NULL,0,&atx[79]} ,
  {0, "fld" ,128,2,0,0,0,0,0,0,NULL,&atx[27],NULL,0,&atx[80]} ,
  {0, "uids" ,128,3,0,0,0,0,0,0,NULL,&atx[42],NULL,0,NULL} ,
  {0, "getmle" ,128,11,0,0,0,0,0,0,NULL,&atx[37],NULL,0,&atx[82]} ,
  {0, "getseq" ,128,12,0,0,0,0,0,0,NULL,&atx[83],NULL,0,NULL} ,
  {413, "Entrez-seqget" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[84],0,&atx[16]} ,
  {0, "retype" ,128,0,0,0,1,0,0,0,&avnx[25],&atx[17],&avnx[20],0,&atx[85]} ,
  {0, "mark-missing" ,128,1,0,1,0,0,0,0,NULL,&atx[40],NULL,0,&atx[86]} ,
  {0, "ids" ,128,2,0,0,0,0,0,0,NULL,&atx[42],NULL,0,NULL} ,
  {421, "Entrez-back" ,1,0,0,0,0,0,0,0,NULL,&atx[34],&atx[88],0,&atx[97]} ,
  {0, "error" ,128,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[89]} ,
  {0, "init" ,128,1,0,0,0,0,0,0,NULL,&atx[8],&atx[90],0,&atx[91]} ,
  {0, "e-info" ,128,0,0,1,0,0,0,0,NULL,&atx[3],NULL,0,NULL} ,
  {0, "maxlinks" ,128,2,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[92]} ,
  {0, "eval" ,128,3,0,0,0,0,0,0,NULL,&atx[34],&atx[93],0,&atx[95]} ,
  {0, "bad-count" ,128,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[94]} ,
  {0, "link-set" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "docsum" ,128,4,0,0,0,0,0,0,NULL,&atx[34],&atx[96],0,&atx[117]} ,
  {0, "ml" ,128,0,0,0,0,0,0,0,NULL,&atx[97],NULL,0,&atx[108]} ,
  {422, "Ml-summary-list" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[98],0,&atx[109]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[99]} ,
  {0, "data" ,128,1,0,0,0,0,0,0,NULL,&atx[35],&atx[100],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[101],NULL,0,NULL} ,
  {429, "Ml-summary" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[102],0,&atx[113]} ,
  {0, "muid" ,128,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[103]} ,
  {0, "no-abstract" ,128,1,0,0,1,0,0,0,&avnx[26],&atx[40],NULL,0,&atx[104]} ,
  {0, "translated-title" ,128,2,0,0,1,0,0,0,&avnx[27],&atx[40],NULL,0,&atx[105]} ,
  {0, "no-authors" ,128,3,0,0,1,0,0,0,&avnx[28],&atx[40],NULL,0,&atx[106]} ,
  {0, "caption" ,128,4,0,1,0,0,0,0,NULL,&atx[7],NULL,0,&atx[107]} ,
  {0, "title" ,128,5,0,1,0,0,0,0,NULL,&atx[7],NULL,0,NULL} ,
  {0, "seq" ,128,1,0,0,0,0,0,0,NULL,&atx[109],NULL,0,NULL} ,
  {423, "Seq-summary-list" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[110],0,&atx[118]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[111]} ,
  {0, "data" ,128,1,0,0,0,0,0,0,NULL,&atx[35],&atx[112],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[113],NULL,0,NULL} ,
  {430, "Seq-summary" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[114],0,&atx[130]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[115]} ,
  {0, "caption" ,128,1,0,1,0,0,0,0,NULL,&atx[7],NULL,0,&atx[116]} ,
  {0, "title" ,128,2,0,1,0,0,0,0,NULL,&atx[7],NULL,0,NULL} ,
  {0, "linkuidlist" ,128,5,0,0,0,0,0,0,NULL,&atx[118],NULL,0,&atx[122]} ,
  {424, "Marked-link-set" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[119],0,&atx[124]} ,
  {0, "link-set" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[120]} ,
  {0, "uids-processed" ,128,1,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[121]} ,
  {0, "marked-missing" ,128,2,0,1,0,0,0,0,NULL,&atx[42],NULL,0,NULL} ,
  {0, "uidlinks" ,128,6,0,0,0,0,0,0,NULL,&atx[118],NULL,0,&atx[123]} ,
  {0, "byterm" ,128,7,0,0,0,0,0,0,NULL,&atx[124],NULL,0,&atx[134]} ,
  {425, "Entrez-term-resp" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[125],0,&atx[136]} ,
  {0, "num-terms" ,128,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[126]} ,
  {0, "first-page" ,128,1,0,1,0,0,0,0,NULL,&atx[14],NULL,0,&atx[127]} ,
  {0, "pages-read" ,128,2,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[128]} ,
  {0, "info" ,128,3,0,0,0,0,0,0,NULL,&atx[35],&atx[129],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[130],NULL,0,NULL} ,
  {431, "Term-page-info" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[131],0,NULL} ,
  {0, "term" ,128,0,0,0,0,0,0,0,NULL,&atx[7],NULL,0,&atx[132]} ,
  {0, "spec-count" ,128,1,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[133]} ,
  {0, "tot-count" ,128,2,0,0,0,0,0,0,NULL,&atx[14],NULL,0,NULL} ,
  {0, "bypage" ,128,8,0,0,0,0,0,0,NULL,&atx[124],NULL,0,&atx[135]} ,
  {0, "findterm" ,128,9,0,0,0,0,0,0,NULL,&atx[136],NULL,0,&atx[140]} ,
  {426, "Term-counts" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[137],0,&atx[143]} ,
  {0, "found" ,128,0,0,0,0,0,0,0,NULL,&atx[40],NULL,0,&atx[138]} ,
  {0, "spec-count" ,128,1,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[139]} ,
  {0, "tot-count" ,128,2,0,0,0,0,0,0,NULL,&atx[14],NULL,0,NULL} ,
  {0, "fini" ,128,10,0,0,0,0,0,0,NULL,&atx[10],NULL,0,&atx[141]} ,
  {0, "createnamed" ,128,11,0,0,0,0,0,0,NULL,&atx[10],NULL,0,&atx[142]} ,
  {0, "getmle" ,128,12,0,0,0,0,0,0,NULL,&atx[143],NULL,0,&atx[148]} ,
  {427, "Entrez-Medline-entry-list" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[144],0,&atx[149]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[145]} ,
  {0, "data" ,128,1,0,0,0,0,0,0,NULL,&atx[35],&atx[146],0,&atx[147]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[1],NULL,0,NULL} ,
  {0, "marked-missing" ,128,2,0,1,0,0,0,0,NULL,&atx[42],NULL,0,NULL} ,
  {0, "getseq" ,128,13,0,0,0,0,0,0,NULL,&atx[149],NULL,0,NULL} ,
  {428, "Entrez-Seq-entry-list" ,1,0,0,0,0,0,0,0,NULL,&atx[8],&atx[150],0,&atx[101]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[151]} ,
  {0, "data" ,128,1,0,0,0,0,0,0,NULL,&atx[35],&atx[152],0,&atx[153]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[0],NULL,0,NULL} ,
  {0, "marked-missing" ,128,2,0,1,0,0,0,0,NULL,&atx[42],NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "NCBI-Entrez" , "asnneten.h03",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module NCBI-Entrez
*
**************************************************/

#define ENTREZ_REQUEST &at[4]
#define ENTREZ_REQUEST_init &at[5]
#define ENTREZ_REQUEST_init_version &at[6]
#define ENTREZ_REQUEST_maxlinks &at[9]
#define ENTREZ_REQUEST_eval &at[11]
#define ENTREZ_REQUEST_docsum &at[36]
#define ENTREZ_REQUEST_linkuidlist &at[47]
#define ENTREZ_REQUEST_uidlinks &at[56]
#define ENTREZ_REQUEST_byterm &at[57]
#define ENTREZ_REQUEST_bypage &at[63]
#define ENTREZ_REQUEST_findterm &at[69]
#define ENTREZ_REQUEST_fini &at[74]
#define ENTREZ_REQUEST_createnamed &at[75]
#define ENTREZ_REQUEST_getmle &at[81]
#define ENTREZ_REQUEST_getseq &at[82]

#define ENTREZ_TERMGET &at[12]
#define ENTREZ_TERMGET_max &at[13]
#define ENTREZ_TERMGET_cls &at[15]
#define ENTREZ_TERMGET_terms &at[18]

#define ENTREZ_DOCGET &at[37]
#define ENTREZ_DOCGET_class &at[38]
#define ENTREZ_DOCGET_mark_missing &at[39]
#define ENTREZ_DOCGET_ids &at[41]
#define ENTREZ_DOCGET_defer_count &at[46]

#define LINK_SETGET &at[48]
#define LINK_SETGET_max &at[49]
#define LINK_SETGET_link_to_cls &at[50]
#define LINK_SETGET_query_cls &at[51]
#define LINK_SETGET_mark_missing &at[52]
#define LINK_SETGET_query_size &at[53]
#define LINK_SETGET_query &at[54]
#define LINK_SETGET_query_E &at[55]

#define ENTREZ_TERM_BY_TERM &at[58]
#define ENTREZ_TERM_BY_TERM_type &at[59]
#define ENTREZ_TERM_BY_TERM_fld &at[60]
#define ENTREZ_TERM_BY_TERM_term &at[61]
#define ENTREZ_TERM_BY_TERM_num_terms &at[62]

#define ENTREZ_TERM_BY_PAGE &at[64]
#define ENTREZ_TERM_BY_PAGE_type &at[65]
#define ENTREZ_TERM_BY_PAGE_fld &at[66]
#define ENTREZ_TERM_BY_PAGE_page &at[67]
#define ENTREZ_TERM_BY_PAGE_num_pages &at[68]

#define TERM_LOOKUP &at[70]
#define TERM_LOOKUP_type &at[71]
#define TERM_LOOKUP_fld &at[72]
#define TERM_LOOKUP_term &at[73]

#define ENTREZ_NAMED_LIST &at[76]
#define ENTREZ_NAMED_LIST_term &at[77]
#define ENTREZ_NAMED_LIST_type &at[78]
#define ENTREZ_NAMED_LIST_fld &at[79]
#define ENTREZ_NAMED_LIST_uids &at[80]

#define ENTREZ_SEQGET &at[83]
#define ENTREZ_SEQGET_retype &at[84]
#define ENTREZ_SEQGET_mark_missing &at[85]
#define ENTREZ_SEQGET_ids &at[86]

#define ENTREZ_CLASS &at[16]

#define ENTREZ_IDS &at[42]
#define ENTREZ_IDS_numid &at[43]
#define ENTREZ_IDS_ids &at[44]
#define ENTREZ_IDS_ids_E &at[45]

#define ENTREZ_TERM_LIST &at[19]
#define ENTREZ_TERM_LIST_E &at[20]
#define ENTREZ_TERM_LIST_E_operator &at[21]
#define ENTREZ_TERM_LIST_E_sp_operand &at[23]
#define ENTREZ_TERM_LIST_E_tot_operand &at[29]

#define ENTREZ_FIELD &at[27]

#define ENTREZ_OPERATOR &at[22]

#define SPECIAL_OPERAND &at[24]
#define SPECIAL_OPERAND_term &at[25]
#define SPECIAL_OPERAND_fld &at[26]
#define SPECIAL_OPERAND_type &at[28]

#define TOTAL_OPERAND &at[30]
#define TOTAL_OPERAND_term &at[31]
#define TOTAL_OPERAND_fld &at[32]
#define TOTAL_OPERAND_type &at[33]

#define ENTREZ_BACK &at[87]
#define ENTREZ_BACK_error &at[88]
#define ENTREZ_BACK_init &at[89]
#define ENTREZ_BACK_init_e_info &at[90]
#define ENTREZ_BACK_maxlinks &at[91]
#define ENTREZ_BACK_eval &at[92]
#define ENTREZ_BACK_eval_bad_count &at[93]
#define ENTREZ_BACK_eval_link_set &at[94]
#define ENTREZ_BACK_docsum &at[95]
#define ENTREZ_BACK_docsum_ml &at[96]
#define ENTREZ_BACK_docsum_seq &at[108]
#define ENTREZ_BACK_linkuidlist &at[117]
#define ENTREZ_BACK_uidlinks &at[122]
#define ENTREZ_BACK_byterm &at[123]
#define ENTREZ_BACK_bypage &at[134]
#define ENTREZ_BACK_findterm &at[135]
#define ENTREZ_BACK_fini &at[140]
#define ENTREZ_BACK_createnamed &at[141]
#define ENTREZ_BACK_getmle &at[142]
#define ENTREZ_BACK_getseq &at[148]

#define ML_SUMMARY_LIST &at[97]
#define ML_SUMMARY_LIST_num &at[98]
#define ML_SUMMARY_LIST_data &at[99]
#define ML_SUMMARY_LIST_data_E &at[100]

#define SEQ_SUMMARY_LIST &at[109]
#define SEQ_SUMMARY_LIST_num &at[110]
#define SEQ_SUMMARY_LIST_data &at[111]
#define SEQ_SUMMARY_LIST_data_E &at[112]

#define MARKED_LINK_SET &at[118]
#define MARKED_LINK_SET_link_set &at[119]
#define MARKED_LINK_SET_uids_processed &at[120]
#define MARKED_LINK_SET_marked_missing &at[121]

#define ENTREZ_TERM_RESP &at[124]
#define ENTREZ_TERM_RESP_num_terms &at[125]
#define ENTREZ_TERM_RESP_first_page &at[126]
#define ENTREZ_TERM_RESP_pages_read &at[127]
#define ENTREZ_TERM_RESP_info &at[128]
#define ENTREZ_TERM_RESP_info_E &at[129]

#define TERM_COUNTS &at[136]
#define TERM_COUNTS_found &at[137]
#define TERM_COUNTS_spec_count &at[138]
#define TERM_COUNTS_tot_count &at[139]

#define ENTREZ_MEDLINE_ENTRY_LIST &at[143]
#define ENTREZ_MEDLINE_ENTRY_LIST_num &at[144]
#define ENTREZ_MEDLINE_ENTRY_LIST_data &at[145]
#define MEDLINE_ENTRY_LIST_data_E &at[146]
#define ENTRY_LIST_marked_missing &at[147]

#define ENTREZ_SEQ_ENTRY_LIST &at[149]
#define ENTREZ_SEQ_ENTRY_LIST_num &at[150]
#define ENTREZ_SEQ_ENTRY_LIST_data &at[151]
#define ENTREZ_SEQ_ENTRY_LIST_data_E &at[152]
#define SEQ_ENTRY_LIST_marked_missing &at[153]

#define ML_SUMMARY &at[101]
#define ML_SUMMARY_muid &at[102]
#define ML_SUMMARY_no_abstract &at[103]
#define ML_SUMMARY_translated_title &at[104]
#define ML_SUMMARY_no_authors &at[105]
#define ML_SUMMARY_caption &at[106]
#define ML_SUMMARY_title &at[107]

#define SEQ_SUMMARY &at[113]
#define SEQ_SUMMARY_id &at[114]
#define SEQ_SUMMARY_caption &at[115]
#define SEQ_SUMMARY_title &at[116]

#define TERM_PAGE_INFO &at[130]
#define TERM_PAGE_INFO_term &at[131]
#define TERM_PAGE_INFO_spec_count &at[132]
#define TERM_PAGE_INFO_tot_count &at[133]
