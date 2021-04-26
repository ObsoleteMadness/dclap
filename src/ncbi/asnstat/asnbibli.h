/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnbibli.h01";
static AsnValxNode avnx[18] = {
    {20,"primary" ,1,0.0,&avnx[1] } ,
    {20,"secondary" ,2,0.0,NULL } ,
    {20,"compiler" ,1,0.0,&avnx[3] } ,
    {20,"editor" ,2,0.0,&avnx[4] } ,
    {20,"patent-assignee" ,3,0.0,&avnx[5] } ,
    {20,"translator" ,4,0.0,NULL } ,
    {1,"ENG" ,0,0.0,NULL } ,
    {20,"submitted" ,1,0.0,&avnx[8] } ,
    {20,"in-press" ,2,0.0,&avnx[9] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"manuscript" ,1,0.0,&avnx[11] } ,
    {20,"letter" ,2,0.0,&avnx[12] } ,
    {20,"thesis" ,3,0.0,NULL } ,
    {20,"paper" ,1,0.0,&avnx[14] } ,
    {20,"tape" ,2,0.0,&avnx[15] } ,
    {20,"floppy" ,3,0.0,&avnx[16] } ,
    {20,"email" ,4,0.0,&avnx[17] } ,
    {20,"other" ,255,0.0,NULL } };

static AsnType atx[113] = {
  {401, "Cit-art" ,1,0,0,0,0,1,0,0,NULL,&atx[38],&atx[1],0,&atx[49]} ,
  {0, "title" ,128,0,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[17]} ,
  {410, "Title" ,1,0,0,0,0,1,0,0,NULL,&atx[16],&atx[3],0,&atx[24]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[15],&atx[4],0,NULL} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[6]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "tsub" ,128,1,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[7]} ,
  {0, "trans" ,128,2,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[8]} ,
  {0, "jta" ,128,3,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[9]} ,
  {0, "iso-jta" ,128,4,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[10]} ,
  {0, "ml-jta" ,128,5,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[11]} ,
  {0, "coden" ,128,6,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[12]} ,
  {0, "issn" ,128,7,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[13]} ,
  {0, "abr" ,128,8,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[14]} ,
  {0, "isbn" ,128,9,0,0,0,0,0,0,NULL,&atx[5],NULL,0,NULL} ,
  {315, "CHOICE" ,0,-1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {314, "SET OF" ,0,17,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "authors" ,128,1,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[47]} ,
  {413, "Auth-list" ,1,0,0,0,0,0,0,0,NULL,&atx[38],&atx[19],0,&atx[52]} ,
  {0, "names" ,128,0,0,0,0,0,0,0,NULL,&atx[15],&atx[20],0,&atx[46]} ,
  {0, "std" ,128,0,0,0,0,0,0,0,NULL,&atx[41],&atx[21],0,&atx[42]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[22],NULL,0,NULL} ,
  {416, "Author" ,1,0,0,0,0,0,0,0,NULL,&atx[38],&atx[23],0,&atx[29]} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[24],NULL,0,&atx[25]} ,
  {411, "Person-id" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[54]} ,
  {0, "level" ,128,1,0,1,0,0,0,0,NULL,&atx[26],&avnx[0],0,&atx[27]} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "role" ,128,2,0,1,0,0,0,0,NULL,&atx[26],&avnx[2],0,&atx[28]} ,
  {0, "affil" ,128,3,0,1,0,0,0,0,NULL,&atx[29],NULL,0,&atx[39]} ,
  {417, "Affil" ,1,0,0,0,0,0,0,0,NULL,&atx[15],&atx[30],0,NULL} ,
  {0, "str" ,128,0,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[31]} ,
  {0, "std" ,128,1,0,0,0,0,0,0,NULL,&atx[38],&atx[32],0,NULL} ,
  {0, "affil" ,128,0,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[33]} ,
  {0, "div" ,128,1,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[34]} ,
  {0, "city" ,128,2,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[35]} ,
  {0, "sub" ,128,3,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[36]} ,
  {0, "country" ,128,4,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[37]} ,
  {0, "street" ,128,5,0,1,0,0,0,0,NULL,&atx[5],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "is-corr" ,128,4,0,1,0,0,0,0,NULL,&atx[40],NULL,0,NULL} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "ml" ,128,1,0,0,0,0,0,0,NULL,&atx[41],&atx[43],0,&atx[44]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[5],NULL,0,NULL} ,
  {0, "str" ,128,2,0,0,0,0,0,0,NULL,&atx[41],&atx[45],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[5],NULL,0,NULL} ,
  {0, "affil" ,128,1,0,1,0,0,0,0,NULL,&atx[29],NULL,0,NULL} ,
  {0, "from" ,128,2,0,0,0,0,0,0,NULL,&atx[15],&atx[48],0,NULL} ,
  {0, "journal" ,128,0,0,0,0,0,0,0,NULL,&atx[49],NULL,0,&atx[64]} ,
  {402, "Cit-jour" ,1,0,0,0,0,1,0,0,NULL,&atx[38],&atx[50],0,&atx[65]} ,
  {0, "title" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[51]} ,
  {0, "imp" ,128,1,0,0,0,0,0,0,NULL,&atx[52],NULL,0,NULL} ,
  {414, "Imprint" ,1,0,0,0,0,0,0,0,NULL,&atx[38],&atx[53],0,&atx[74]} ,
  {0, "date" ,128,0,0,0,0,0,0,0,NULL,&atx[54],NULL,0,&atx[55]} ,
  {412, "Date" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[18]} ,
  {0, "volume" ,128,1,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[56]} ,
  {0, "issue" ,128,2,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[57]} ,
  {0, "pages" ,128,3,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[58]} ,
  {0, "section" ,128,4,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[59]} ,
  {0, "pub" ,128,5,0,1,0,0,0,0,NULL,&atx[29],NULL,0,&atx[60]} ,
  {0, "cprt" ,128,6,0,1,0,0,0,0,NULL,&atx[54],NULL,0,&atx[61]} ,
  {0, "part-sup" ,128,7,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[62]} ,
  {0, "language" ,128,8,0,0,1,0,0,0,&avnx[6],&atx[5],NULL,0,&atx[63]} ,
  {0, "prepub" ,128,9,0,1,0,0,0,0,NULL,&atx[26],&avnx[7],0,NULL} ,
  {0, "book" ,128,1,0,0,0,0,0,0,NULL,&atx[65],NULL,0,&atx[70]} ,
  {403, "Cit-book" ,1,0,0,0,0,1,0,0,NULL,&atx[38],&atx[66],0,&atx[78]} ,
  {0, "title" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[67]} ,
  {0, "coll" ,128,1,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[68]} ,
  {0, "authors" ,128,2,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[69]} ,
  {0, "imp" ,128,3,0,0,0,0,0,0,NULL,&atx[52],NULL,0,NULL} ,
  {0, "proc" ,128,2,0,0,0,0,0,0,NULL,&atx[71],NULL,0,NULL} ,
  {408, "Cit-proc" ,1,0,0,0,0,1,0,0,NULL,&atx[38],&atx[72],0,&atx[109]} ,
  {0, "book" ,128,0,0,0,0,0,0,0,NULL,&atx[65],NULL,0,&atx[73]} ,
  {0, "meet" ,128,1,0,0,0,0,0,0,NULL,&atx[74],NULL,0,NULL} ,
  {415, "Meeting" ,1,0,0,0,0,0,0,0,NULL,&atx[38],&atx[75],0,&atx[22]} ,
  {0, "number" ,128,0,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[76]} ,
  {0, "date" ,128,1,0,0,0,0,0,0,NULL,&atx[54],NULL,0,&atx[77]} ,
  {0, "place" ,128,2,0,1,0,0,0,0,NULL,&atx[29],NULL,0,NULL} ,
  {404, "Cit-pat" ,1,0,0,0,0,1,0,0,NULL,&atx[38],&atx[79],0,&atx[88]} ,
  {0, "title" ,128,0,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[80]} ,
  {0, "authors" ,128,1,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[81]} ,
  {0, "country" ,128,2,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[82]} ,
  {0, "doc-type" ,128,3,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[83]} ,
  {0, "number" ,128,4,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[84]} ,
  {0, "date-issue" ,128,5,0,0,0,0,0,0,NULL,&atx[54],NULL,0,&atx[85]} ,
  {0, "class" ,128,6,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[86]} ,
  {0, "app-number" ,128,7,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[87]} ,
  {0, "app-date" ,128,8,0,1,0,0,0,0,NULL,&atx[54],NULL,0,NULL} ,
  {405, "Cit-let" ,1,0,0,0,0,1,0,0,NULL,&atx[38],&atx[89],0,&atx[92]} ,
  {0, "cit" ,128,0,0,0,0,0,0,0,NULL,&atx[65],NULL,0,&atx[90]} ,
  {0, "man-id" ,128,1,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[91]} ,
  {0, "type" ,128,2,0,1,0,0,0,0,NULL,&atx[26],&avnx[10],0,NULL} ,
  {406, "Id-pat" ,1,0,0,0,0,1,0,0,NULL,&atx[38],&atx[93],0,&atx[97]} ,
  {0, "country" ,128,0,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[94]} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[15],&atx[95],0,NULL} ,
  {0, "number" ,128,0,0,0,0,0,0,0,NULL,&atx[5],NULL,0,&atx[96]} ,
  {0, "app-number" ,128,1,0,0,0,0,0,0,NULL,&atx[5],NULL,0,NULL} ,
  {407, "Cit-gen" ,1,0,0,0,0,1,0,0,NULL,&atx[38],&atx[98],0,&atx[71]} ,
  {0, "cit" ,128,0,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[99]} ,
  {0, "authors" ,128,1,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[100]} ,
  {0, "muid" ,128,2,0,1,0,0,0,0,NULL,&atx[101],NULL,0,&atx[102]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "journal" ,128,3,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[103]} ,
  {0, "volume" ,128,4,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[104]} ,
  {0, "issue" ,128,5,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[105]} ,
  {0, "pages" ,128,6,0,1,0,0,0,0,NULL,&atx[5],NULL,0,&atx[106]} ,
  {0, "date" ,128,7,0,1,0,0,0,0,NULL,&atx[54],NULL,0,&atx[107]} ,
  {0, "serial-number" ,128,8,0,1,0,0,0,0,NULL,&atx[101],NULL,0,&atx[108]} ,
  {0, "title" ,128,9,0,1,0,0,0,0,NULL,&atx[5],NULL,0,NULL} ,
  {409, "Cit-sub" ,1,0,0,0,0,1,0,0,NULL,&atx[38],&atx[110],0,&atx[2]} ,
  {0, "authors" ,128,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[111]} ,
  {0, "imp" ,128,1,0,0,0,0,0,0,NULL,&atx[52],NULL,0,&atx[112]} ,
  {0, "medium" ,128,2,0,1,0,0,0,0,NULL,&atx[26],&avnx[13],0,NULL} };

static AsnModule ampx[1] = {
  { "NCBI-Biblio" , "asnbibli.h01",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module NCBI-Biblio
*
**************************************************/

#define CIT_ART &at[0]
#define CIT_ART_title &at[1]
#define CIT_ART_authors &at[17]
#define CIT_ART_from &at[47]
#define CIT_ART_from_journal &at[48]
#define CIT_ART_from_book &at[64]
#define CIT_ART_from_proc &at[70]

#define CIT_JOUR &at[49]
#define CIT_JOUR_title &at[50]
#define CIT_JOUR_imp &at[51]

#define CIT_BOOK &at[65]
#define CIT_BOOK_title &at[66]
#define CIT_BOOK_coll &at[67]
#define CIT_BOOK_authors &at[68]
#define CIT_BOOK_imp &at[69]

#define CIT_PAT &at[78]
#define CIT_PAT_title &at[79]
#define CIT_PAT_authors &at[80]
#define CIT_PAT_country &at[81]
#define CIT_PAT_doc_type &at[82]
#define CIT_PAT_number &at[83]
#define CIT_PAT_date_issue &at[84]
#define CIT_PAT_class &at[85]
#define CIT_PAT_app_number &at[86]
#define CIT_PAT_app_date &at[87]

#define CIT_LET &at[88]
#define CIT_LET_cit &at[89]
#define CIT_LET_man_id &at[90]
#define CIT_LET_type &at[91]

#define ID_PAT &at[92]
#define ID_PAT_country &at[93]
#define ID_PAT_id &at[94]
#define ID_PAT_id_number &at[95]
#define ID_PAT_id_app_number &at[96]

#define CIT_GEN &at[97]
#define CIT_GEN_cit &at[98]
#define CIT_GEN_authors &at[99]
#define CIT_GEN_muid &at[100]
#define CIT_GEN_journal &at[102]
#define CIT_GEN_volume &at[103]
#define CIT_GEN_issue &at[104]
#define CIT_GEN_pages &at[105]
#define CIT_GEN_date &at[106]
#define CIT_GEN_serial_number &at[107]
#define CIT_GEN_title &at[108]

#define CIT_PROC &at[71]
#define CIT_PROC_book &at[72]
#define CIT_PROC_meet &at[73]

#define CIT_SUB &at[109]
#define CIT_SUB_authors &at[110]
#define CIT_SUB_imp &at[111]
#define CIT_SUB_medium &at[112]

#define TITLE &at[2]
#define TITLE_E &at[3]
#define TITLE_E_name &at[4]
#define TITLE_E_tsub &at[6]
#define TITLE_E_trans &at[7]
#define TITLE_E_jta &at[8]
#define TITLE_E_iso_jta &at[9]
#define TITLE_E_ml_jta &at[10]
#define TITLE_E_coden &at[11]
#define TITLE_E_issn &at[12]
#define TITLE_E_abr &at[13]
#define TITLE_E_isbn &at[14]

#define AUTH_LIST &at[18]
#define AUTH_LIST_names &at[19]
#define AUTH_LIST_names_std &at[20]
#define AUTH_LIST_names_std_E &at[21]
#define AUTH_LIST_names_ml &at[42]
#define AUTH_LIST_names_ml_E &at[43]
#define AUTH_LIST_names_str &at[44]
#define AUTH_LIST_names_str_E &at[45]
#define AUTH_LIST_affil &at[46]

#define IMPRINT &at[52]
#define IMPRINT_date &at[53]
#define IMPRINT_volume &at[55]
#define IMPRINT_issue &at[56]
#define IMPRINT_pages &at[57]
#define IMPRINT_section &at[58]
#define IMPRINT_pub &at[59]
#define IMPRINT_cprt &at[60]
#define IMPRINT_part_sup &at[61]
#define IMPRINT_language &at[62]
#define IMPRINT_prepub &at[63]

#define MEETING &at[74]
#define MEETING_number &at[75]
#define MEETING_date &at[76]
#define MEETING_place &at[77]

#define AUTHOR &at[22]
#define AUTHOR_name &at[23]
#define AUTHOR_level &at[25]
#define AUTHOR_role &at[27]
#define AUTHOR_affil &at[28]
#define AUTHOR_is_corr &at[39]

#define AFFIL &at[29]
#define AFFIL_str &at[30]
#define AFFIL_std &at[31]
#define AFFIL_std_affil &at[32]
#define AFFIL_std_div &at[33]
#define AFFIL_std_city &at[34]
#define AFFIL_std_sub &at[35]
#define AFFIL_std_country &at[36]
#define AFFIL_std_street &at[37]
