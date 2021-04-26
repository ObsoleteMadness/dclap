/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asntaxon.l";
static AsnValxNodePtr avn;
static AsnTypePtr at;
static AsnModulePtr amp;


/**************************************************
*
*    Defines for Module NCBI-Taxon0
*
**************************************************/

#define TAXON0_REQ &at[1]
#define TAXON0_REQ_init &at[2]
#define TAXON0_REQ_getid &at[4]
#define TAXON0_REQ_getref &at[12]
#define TAXON0_REQ_getchildren &at[14]
#define TAXON0_REQ_getparents &at[15]
#define TAXON0_REQ_getgeneticcode &at[16]
#define TAXON0_REQ_gettaxonline &at[17]
#define TAXON0_REQ_getdivision &at[18]
#define TAXON0_REQ_getcomplete &at[19]
#define TAXON0_REQ_fini &at[23]

#define TAXON_NAME &at[5]
#define TAXON_NAME_taxname &at[6]
#define TAXON_NAME_common &at[8]
#define TAXON_NAME_tax_synonym &at[9]
#define TAXON_NAME_com_synonym &at[10]

#define TAXON_ID_NAME &at[20]
#define TAXON_ID_NAME_id &at[21]
#define TAXON_ID_NAME_name &at[22]

#define TAXON0_RESP &at[24]
#define TAXON0_RESP_error &at[25]
#define TAXON0_RESP_init &at[26]
#define TAXON0_RESP_getid &at[27]
#define TAXON0_RESP_getref &at[33]
#define TAXON0_RESP_gettaxon &at[34]
#define TAXON0_RESP_getgeneticcode &at[35]
#define TAXON0_RESP_gettaxonline &at[39]
#define TAXON0_RESP_getdivision &at[40]
#define TAXON0_RESP_getcomplete &at[41]
#define TAXON0_RESP_fini &at[59]

#define TAXON_ID_LIST &at[28]
#define TAXON_ID_LIST_ids &at[29]
#define TAXON_ID_LIST_ids_E &at[30]

#define GENETICCODELIST &at[36]
#define GENETICCODELIST_genomic &at[37]
#define GENETICCODELIST_mitochondrial &at[38]

#define TAX_COMPLETE_LIST &at[42]
#define TAX_COMPLETE_LIST_num &at[43]
#define TAX_COMPLETE_LIST_info &at[44]
#define TAX_COMPLETE_LIST_info_E &at[45]

#define TAXON_ID &at[60]
#define TAXON_ID_id &at[61]

#define TAX_COMPLETE &at[46]
#define TAX_COMPLETE_sciname &at[47]
#define TAX_COMPLETE_comname &at[48]
#define TAX_COMPLETE_synonyms &at[49]
#define TAX_COMPLETE_id_gc &at[50]
#define TAX_COMPLETE_name_gc &at[51]
#define TAX_COMPLETE_id_mgc &at[52]
#define TAX_COMPLETE_name_mgc &at[53]
#define TAX_COMPLETE_gb_div &at[54]
#define TAX_COMPLETE_embl_code &at[55]
#define TAX_COMPLETE_lineage &at[56]
#define TAX_COMPLETE_is_species_level &at[57]
