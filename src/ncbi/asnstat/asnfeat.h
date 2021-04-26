/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnfeat.h00";
static AsnValxNode avnx[84] = {
    {2,NULL,0,0.0,NULL } ,
    {20,"not-set" ,0,0.0,&avnx[2] } ,
    {20,"one" ,1,0.0,&avnx[3] } ,
    {20,"two" ,2,0.0,&avnx[4] } ,
    {20,"three" ,3,0.0,NULL } ,
    {3,NULL,1,0.0,NULL } ,
    {20,"unknown" ,0,0.0,&avnx[7] } ,
    {20,"premsg" ,1,0.0,&avnx[8] } ,
    {20,"mRNA" ,2,0.0,&avnx[9] } ,
    {20,"tRNA" ,3,0.0,&avnx[10] } ,
    {20,"rRNA" ,4,0.0,&avnx[11] } ,
    {20,"snRNA" ,5,0.0,&avnx[12] } ,
    {20,"scRNA" ,6,0.0,&avnx[13] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"disulfide" ,1,0.0,&avnx[15] } ,
    {20,"thiolester" ,2,0.0,&avnx[16] } ,
    {20,"xlink" ,3,0.0,&avnx[17] } ,
    {20,"thioether" ,4,0.0,&avnx[18] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"active" ,1,0.0,&avnx[20] } ,
    {20,"binding" ,2,0.0,&avnx[21] } ,
    {20,"cleavage" ,3,0.0,&avnx[22] } ,
    {20,"inhibit" ,4,0.0,&avnx[23] } ,
    {20,"modified" ,5,0.0,&avnx[24] } ,
    {20,"glycosylation" ,6,0.0,&avnx[25] } ,
    {20,"myristoylation" ,7,0.0,&avnx[26] } ,
    {20,"mutagenized" ,8,0.0,&avnx[27] } ,
    {20,"metal-binding" ,9,0.0,&avnx[28] } ,
    {20,"phosphorylation" ,10,0.0,&avnx[29] } ,
    {20,"acetylation" ,11,0.0,&avnx[30] } ,
    {20,"amidation" ,12,0.0,&avnx[31] } ,
    {20,"methylation" ,13,0.0,&avnx[32] } ,
    {20,"hydroxylation" ,14,0.0,&avnx[33] } ,
    {20,"sulfatation" ,15,0.0,&avnx[34] } ,
    {20,"oxidative-deamination" ,16,0.0,&avnx[35] } ,
    {20,"pyrrolidone-carboxylic-acid" ,17,0.0,&avnx[36] } ,
    {20,"gamma-carboxyglutamic-acid" ,18,0.0,&avnx[37] } ,
    {20,"blocked" ,19,0.0,&avnx[38] } ,
    {20,"lipid-binding" ,20,0.0,&avnx[39] } ,
    {20,"np-binding" ,21,0.0,&avnx[40] } ,
    {20,"dna-binding" ,22,0.0,&avnx[41] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"unknown" ,0,0.0,&avnx[43] } ,
    {20,"pol1" ,1,0.0,&avnx[44] } ,
    {20,"pol2" ,2,0.0,&avnx[45] } ,
    {20,"pol3" ,3,0.0,&avnx[46] } ,
    {20,"bacterial" ,4,0.0,&avnx[47] } ,
    {20,"viral" ,5,0.0,&avnx[48] } ,
    {20,"rna" ,6,0.0,&avnx[49] } ,
    {20,"organelle" ,7,0.0,&avnx[50] } ,
    {20,"other" ,255,0.0,NULL } ,
    {2,NULL,0,0.0,NULL } ,
    {2,NULL,0,0.0,NULL } ,
    {20,"unknown" ,0,0.0,&avnx[54] } ,
    {20,"single" ,1,0.0,&avnx[55] } ,
    {20,"multiple" ,2,0.0,&avnx[56] } ,
    {20,"region" ,3,0.0,NULL } ,
    {20,"unknown" ,0,0.0,&avnx[58] } ,
    {20,"rna-seq" ,1,0.0,&avnx[59] } ,
    {20,"rna-size" ,2,0.0,&avnx[60] } ,
    {20,"np-map" ,3,0.0,&avnx[61] } ,
    {20,"np-size" ,4,0.0,&avnx[62] } ,
    {20,"pe-seq" ,5,0.0,&avnx[63] } ,
    {20,"cDNA-seq" ,6,0.0,&avnx[64] } ,
    {20,"pe-map" ,7,0.0,&avnx[65] } ,
    {20,"pe-size" ,8,0.0,&avnx[66] } ,
    {20,"pseudo-seq" ,9,0.0,&avnx[67] } ,
    {20,"rev-pe-map" ,10,0.0,&avnx[68] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"unknown" ,0,0.0,&avnx[70] } ,
    {20,"physiological" ,1,0.0,&avnx[71] } ,
    {20,"in-vitro" ,2,0.0,&avnx[72] } ,
    {20,"oocyte" ,3,0.0,&avnx[73] } ,
    {20,"transfection" ,4,0.0,&avnx[74] } ,
    {20,"transgenic" ,5,0.0,&avnx[75] } ,
    {20,"other" ,255,0.0,NULL } ,
    {3,NULL,1,0.0,NULL } ,
    {2,NULL,0,0.0,NULL } ,
    {2,NULL,0,0.0,NULL } ,
    {20,"helix" ,1,0.0,&avnx[80] } ,
    {20,"sheet" ,2,0.0,&avnx[81] } ,
    {20,"turn" ,3,0.0,NULL } ,
    {20,"experimental" ,1,0.0,&avnx[83] } ,
    {20,"not-experimental" ,2,0.0,NULL } };

static AsnType atx[181] = {
  {401, "Seq-feat" ,1,0,0,0,0,1,0,0,NULL,&atx[30],&atx[1],0,&atx[2]} ,
  {0, "id" ,128,0,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[12]} ,
  {402, "Feat-id" ,1,0,0,0,0,1,0,0,NULL,&atx[11],&atx[3],0,&atx[15]} ,
  {0, "gibb" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[5]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "giim" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[7]} ,
  {408, "Giimport-id" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[104]} ,
  {0, "local" ,128,2,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[9]} ,
  {415, "Object-id" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[10]} ,
  {0, "general" ,128,3,0,0,0,0,0,0,NULL,&atx[10],NULL,0,NULL} ,
  {416, "Dbtag" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[123]} ,
  {315, "CHOICE" ,0,-1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "data" ,128,1,0,0,0,0,0,0,NULL,&atx[13],NULL,0,&atx[159]} ,
  {418, "SeqFeatData" ,1,0,0,0,0,0,0,0,NULL,&atx[11],&atx[14],0,&atx[166]} ,
  {0, "gene" ,128,0,0,0,0,0,0,0,NULL,&atx[15],NULL,0,&atx[31]} ,
  {403, "Gene-ref" ,1,0,0,0,0,0,1,0,NULL,&atx[16],NULL,0,&atx[75]} ,
  {401, "Gene-ref" ,1,0,0,0,0,1,0,0,NULL,&atx[30],&atx[17],0,&atx[26]} ,
  {0, "locus" ,128,0,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[19]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "allele" ,128,1,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[20]} ,
  {0, "desc" ,128,2,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[21]} ,
  {0, "maploc" ,128,3,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[22]} ,
  {0, "pseudo" ,128,4,0,0,1,0,0,0,&avnx[0],&atx[23],NULL,0,&atx[24]} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "db" ,128,5,0,1,0,0,0,0,NULL,&atx[27],&atx[25],0,&atx[28]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[26],NULL,0,NULL} ,
  {402, "Dbtag" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,NULL} ,
  {314, "SET OF" ,0,17,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "syn" ,128,6,0,1,0,0,0,0,NULL,&atx[27],&atx[29],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "org" ,128,1,0,0,0,0,0,0,NULL,&atx[32],NULL,0,&atx[43]} ,
  {405, "Org-ref" ,1,0,0,0,0,0,1,0,NULL,&atx[33],NULL,0,&atx[88]} ,
  {401, "Org-ref" ,1,0,0,0,0,1,0,0,NULL,&atx[30],&atx[34],0,&atx[40]} ,
  {0, "taxname" ,128,0,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[35]} ,
  {0, "common" ,128,1,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[36]} ,
  {0, "mod" ,128,2,0,1,0,0,0,0,NULL,&atx[27],&atx[37],0,&atx[38]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,NULL} ,
  {0, "db" ,128,3,0,1,0,0,0,0,NULL,&atx[27],&atx[39],0,&atx[41]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[40],NULL,0,NULL} ,
  {402, "Dbtag" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,NULL} ,
  {0, "syn" ,128,4,0,1,0,0,0,0,NULL,&atx[27],&atx[42],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,NULL} ,
  {0, "cdregion" ,128,2,0,0,0,0,0,0,NULL,&atx[44],NULL,0,&atx[74]} ,
  {421, "Cdregion" ,1,0,0,0,0,0,0,0,NULL,&atx[30],&atx[45],0,&atx[107]} ,
  {0, "orf" ,128,0,0,1,0,0,0,0,NULL,&atx[23],NULL,0,&atx[46]} ,
  {0, "frame" ,128,1,0,0,1,0,0,0,&avnx[5],&atx[47],&avnx[1],0,&atx[48]} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "conflict" ,128,2,0,1,0,0,0,0,NULL,&atx[23],NULL,0,&atx[49]} ,
  {0, "gaps" ,128,3,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[50]} ,
  {0, "mismatch" ,128,4,0,1,0,0,0,0,NULL,&atx[4],NULL,0,&atx[51]} ,
  {0, "code" ,128,5,0,1,0,0,0,0,NULL,&atx[52],NULL,0,&atx[63]} ,
  {423, "Genetic-code" ,1,0,0,0,0,0,0,0,NULL,&atx[27],&atx[53],0,&atx[65]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[11],&atx[54],0,NULL} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[55]} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[56]} ,
  {0, "ncbieaa" ,128,2,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[57]} ,
  {0, "ncbi8aa" ,128,3,0,0,0,0,0,0,NULL,&atx[58],NULL,0,&atx[59]} ,
  {304, "OCTET STRING" ,0,4,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "ncbistdaa" ,128,4,0,0,0,0,0,0,NULL,&atx[58],NULL,0,&atx[60]} ,
  {0, "sncbieaa" ,128,5,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[61]} ,
  {0, "sncbi8aa" ,128,6,0,0,0,0,0,0,NULL,&atx[58],NULL,0,&atx[62]} ,
  {0, "sncbistdaa" ,128,7,0,0,0,0,0,0,NULL,&atx[58],NULL,0,NULL} ,
  {0, "code-break" ,128,6,0,1,0,0,0,0,NULL,&atx[72],&atx[64],0,&atx[73]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[65],NULL,0,NULL} ,
  {424, "Code-break" ,1,0,0,0,0,0,0,0,NULL,&atx[30],&atx[66],0,&atx[179]} ,
  {0, "loc" ,128,0,0,0,0,0,0,0,NULL,&atx[67],NULL,0,&atx[68]} ,
  {407, "Seq-loc" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[6]} ,
  {0, "aa" ,128,1,0,0,0,0,0,0,NULL,&atx[11],&atx[69],0,NULL} ,
  {0, "ncbieaa" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[70]} ,
  {0, "ncbi8aa" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[71]} ,
  {0, "ncbistdaa" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "stops" ,128,7,0,1,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "prot" ,128,3,0,0,0,0,0,0,NULL,&atx[75],NULL,0,&atx[87]} ,
  {404, "Prot-ref" ,1,0,0,0,0,0,1,0,NULL,&atx[76],NULL,0,&atx[32]} ,
  {401, "Prot-ref" ,1,0,0,0,0,1,0,0,NULL,&atx[30],&atx[77],0,&atx[86]} ,
  {0, "name" ,128,0,0,1,0,0,0,0,NULL,&atx[27],&atx[78],0,&atx[79]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,NULL} ,
  {0, "desc" ,128,1,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[80]} ,
  {0, "ec" ,128,2,0,1,0,0,0,0,NULL,&atx[27],&atx[81],0,&atx[82]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,NULL} ,
  {0, "activity" ,128,3,0,1,0,0,0,0,NULL,&atx[27],&atx[83],0,&atx[84]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,NULL} ,
  {0, "db" ,128,4,0,1,0,0,0,0,NULL,&atx[27],&atx[85],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[86],NULL,0,NULL} ,
  {402, "Dbtag" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,NULL} ,
  {0, "rna" ,128,4,0,0,0,0,0,0,NULL,&atx[88],NULL,0,&atx[103]} ,
  {406, "RNA-ref" ,1,0,0,0,0,0,1,0,NULL,&atx[89],NULL,0,&atx[67]} ,
  {401, "RNA-ref" ,1,0,0,0,0,1,0,0,NULL,&atx[30],&atx[90],0,&atx[95]} ,
  {0, "type" ,128,0,0,0,0,0,0,0,NULL,&atx[47],&avnx[6],0,&atx[91]} ,
  {0, "pseudo" ,128,1,0,1,0,0,0,0,NULL,&atx[23],NULL,0,&atx[92]} ,
  {0, "ext" ,128,2,0,1,0,0,0,0,NULL,&atx[11],&atx[93],0,NULL} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[94]} ,
  {0, "tRNA" ,128,1,0,0,0,0,0,0,NULL,&atx[95],NULL,0,NULL} ,
  {402, "Trna-ext" ,1,0,0,0,0,1,0,0,NULL,&atx[30],&atx[96],0,NULL} ,
  {0, "aa" ,128,0,0,1,0,0,0,0,NULL,&atx[11],&atx[97],0,&atx[101]} ,
  {0, "iupacaa" ,128,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[98]} ,
  {0, "ncbieaa" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[99]} ,
  {0, "ncbi8aa" ,128,2,0,0,0,0,0,0,NULL,&atx[4],NULL,0,&atx[100]} ,
  {0, "ncbistdaa" ,128,3,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "codon" ,128,1,0,1,0,0,0,0,NULL,&atx[27],&atx[102],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {0, "pub" ,128,5,0,0,0,0,0,0,NULL,&atx[104],NULL,0,&atx[105]} ,
  {409, "Pubdesc" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[154]} ,
  {0, "seq" ,128,6,0,0,0,0,0,0,NULL,&atx[67],NULL,0,&atx[106]} ,
  {0, "imp" ,128,7,0,0,0,0,0,0,NULL,&atx[107],NULL,0,&atx[111]} ,
  {422, "Imp-feat" ,1,0,0,0,0,0,0,0,NULL,&atx[30],&atx[108],0,&atx[52]} ,
  {0, "key" ,128,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[109]} ,
  {0, "loc" ,128,1,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[110]} ,
  {0, "descr" ,128,2,0,1,0,0,0,0,NULL,&atx[18],NULL,0,NULL} ,
  {0, "region" ,128,8,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[112]} ,
  {0, "comment" ,128,9,0,0,0,0,0,0,NULL,&atx[113],NULL,0,&atx[114]} ,
  {305, "NULL" ,0,5,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "bond" ,128,10,0,0,0,0,0,0,NULL,&atx[47],&avnx[14],0,&atx[115]} ,
  {0, "site" ,128,11,0,0,0,0,0,0,NULL,&atx[47],&avnx[19],0,&atx[116]} ,
  {0, "rsite" ,128,12,0,0,0,0,0,0,NULL,&atx[117],NULL,0,&atx[122]} ,
  {412, "Rsite-ref" ,1,0,0,0,0,0,1,0,NULL,&atx[118],NULL,0,&atx[125]} ,
  {401, "Rsite-ref" ,1,0,0,0,0,1,0,0,NULL,&atx[11],&atx[119],0,&atx[121]} ,
  {0, "str" ,128,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[120]} ,
  {0, "db" ,128,1,0,0,0,0,0,0,NULL,&atx[121],NULL,0,NULL} ,
  {402, "Dbtag" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,NULL} ,
  {0, "user" ,128,13,0,0,0,0,0,0,NULL,&atx[123],NULL,0,&atx[124]} ,
  {417, "User-object" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[13]} ,
  {0, "txinit" ,128,14,0,0,0,0,0,0,NULL,&atx[125],NULL,0,&atx[153]} ,
  {413, "Txinit" ,1,0,0,0,0,0,1,0,NULL,&atx[126],NULL,0,&atx[172]} ,
  {401, "Txinit" ,1,0,0,0,0,1,0,0,NULL,&atx[30],&atx[127],0,&atx[132]} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[128]} ,
  {0, "syn" ,128,1,0,1,0,0,0,0,NULL,&atx[72],&atx[129],0,&atx[130]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,NULL} ,
  {0, "gene" ,128,2,0,1,0,0,0,0,NULL,&atx[72],&atx[131],0,&atx[133]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[132],NULL,0,NULL} ,
  {402, "Gene-ref" ,1,0,0,0,0,0,1,0,NULL,&atx[16],NULL,0,&atx[135]} ,
  {0, "protein" ,128,3,0,1,0,0,0,0,NULL,&atx[72],&atx[134],0,&atx[136]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[135],NULL,0,NULL} ,
  {403, "Prot-ref" ,1,0,0,0,0,0,1,0,NULL,&atx[76],NULL,0,&atx[142]} ,
  {0, "rna" ,128,4,0,1,0,0,0,0,NULL,&atx[72],&atx[137],0,&atx[138]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,NULL} ,
  {0, "expression" ,128,5,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[139]} ,
  {0, "txsystem" ,128,6,0,0,0,0,0,0,NULL,&atx[47],&avnx[42],0,&atx[140]} ,
  {0, "txdescr" ,128,7,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[141]} ,
  {0, "txorg" ,128,8,0,1,0,0,0,0,NULL,&atx[142],NULL,0,&atx[143]} ,
  {404, "Org-ref" ,1,0,0,0,0,0,1,0,NULL,&atx[33],NULL,0,&atx[148]} ,
  {0, "mapping-precise" ,128,9,0,0,1,0,0,0,&avnx[51],&atx[23],NULL,0,&atx[144]} ,
  {0, "location-accurate" ,128,10,0,0,1,0,0,0,&avnx[52],&atx[23],NULL,0,&atx[145]} ,
  {0, "inittype" ,128,11,0,1,0,0,0,0,NULL,&atx[47],&avnx[53],0,&atx[146]} ,
  {0, "evidence" ,128,12,0,1,0,0,0,0,NULL,&atx[27],&atx[147],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[148],NULL,0,NULL} ,
  {405, "Tx-evidence" ,1,0,0,0,0,0,0,0,NULL,&atx[30],&atx[149],0,NULL} ,
  {0, "exp-code" ,128,0,0,0,0,0,0,0,NULL,&atx[47],&avnx[57],0,&atx[150]} ,
  {0, "expression-system" ,128,1,0,0,1,0,0,0,&avnx[76],&atx[47],&avnx[69],0,&atx[151]} ,
  {0, "low-prec-data" ,128,2,0,0,1,0,0,0,&avnx[77],&atx[23],NULL,0,&atx[152]} ,
  {0, "from-homolog" ,128,3,0,0,1,0,0,0,&avnx[78],&atx[23],NULL,0,NULL} ,
  {0, "num" ,128,15,0,0,0,0,0,0,NULL,&atx[154],NULL,0,&atx[155]} ,
  {410, "Numbering" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[158]} ,
  {0, "psec-str" ,128,16,0,0,0,0,0,0,NULL,&atx[47],&avnx[79],0,&atx[156]} ,
  {0, "non-std-residue" ,128,17,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[157]} ,
  {0, "het" ,128,18,0,0,0,0,0,0,NULL,&atx[158],NULL,0,NULL} ,
  {411, "Heterogen" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[117]} ,
  {0, "partial" ,128,2,0,1,0,0,0,0,NULL,&atx[23],NULL,0,&atx[160]} ,
  {0, "except" ,128,3,0,1,0,0,0,0,NULL,&atx[23],NULL,0,&atx[161]} ,
  {0, "comment" ,128,4,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[162]} ,
  {0, "product" ,128,5,0,1,0,0,0,0,NULL,&atx[67],NULL,0,&atx[163]} ,
  {0, "location" ,128,6,0,0,0,0,0,0,NULL,&atx[67],NULL,0,&atx[164]} ,
  {0, "qual" ,128,7,0,1,0,0,0,0,NULL,&atx[72],&atx[165],0,&atx[169]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[166],NULL,0,NULL} ,
  {419, "Gb-qual" ,1,0,0,0,0,0,0,0,NULL,&atx[30],&atx[167],0,&atx[176]} ,
  {0, "qual" ,128,0,0,0,0,0,0,0,NULL,&atx[18],NULL,0,&atx[168]} ,
  {0, "val" ,128,1,0,0,0,0,0,0,NULL,&atx[18],NULL,0,NULL} ,
  {0, "title" ,128,8,0,1,0,0,0,0,NULL,&atx[18],NULL,0,&atx[170]} ,
  {0, "ext" ,128,9,0,1,0,0,0,0,NULL,&atx[123],NULL,0,&atx[171]} ,
  {0, "cit" ,128,10,0,1,0,0,0,0,NULL,&atx[172],NULL,0,&atx[173]} ,
  {414, "Pub-set" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[8]} ,
  {0, "exp-ev" ,128,11,0,1,0,0,0,0,NULL,&atx[47],&avnx[82],0,&atx[174]} ,
  {0, "xref" ,128,12,0,1,0,0,0,0,NULL,&atx[27],&atx[175],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[176],NULL,0,NULL} ,
  {420, "SeqFeatXref" ,1,0,0,0,0,0,0,0,NULL,&atx[30],&atx[177],0,&atx[44]} ,
  {0, "id" ,128,0,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[178]} ,
  {0, "data" ,128,1,0,0,0,0,0,0,NULL,&atx[13],NULL,0,NULL} ,
  {425, "Genetic-code-table" ,1,0,0,0,0,0,0,0,NULL,&atx[27],&atx[180],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[52],NULL,0,NULL} };

static AsnModule ampx[7] = {
  { "NCBI-Seqfeat" , "asnfeat.h00",&atx[0],NULL,&ampx[1],0,0} ,
  { "NCBI-Rsite" , NULL,&atx[118],NULL,&ampx[2],0,0} ,
  { "NCBI-RNA" , NULL,&atx[89],NULL,&ampx[3],0,0} ,
  { "NCBI-Gene" , NULL,&atx[16],NULL,&ampx[4],0,0} ,
  { "NCBI-Organism" , NULL,&atx[33],NULL,&ampx[5],0,0} ,
  { "NCBI-Protein" , NULL,&atx[76],NULL,&ampx[6],0,0} ,
  { "NCBI-TxInit" , NULL,&atx[126],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module NCBI-Seqfeat
*
**************************************************/

#define SEQ_FEAT &at[0]
#define SEQ_FEAT_id &at[1]
#define SEQ_FEAT_data &at[12]
#define SEQ_FEAT_partial &at[159]
#define SEQ_FEAT_except &at[160]
#define SEQ_FEAT_comment &at[161]
#define SEQ_FEAT_product &at[162]
#define SEQ_FEAT_location &at[163]
#define SEQ_FEAT_qual &at[164]
#define SEQ_FEAT_qual_E &at[165]
#define SEQ_FEAT_title &at[169]
#define SEQ_FEAT_ext &at[170]
#define SEQ_FEAT_cit &at[171]
#define SEQ_FEAT_exp_ev &at[173]
#define SEQ_FEAT_xref &at[174]
#define SEQ_FEAT_xref_E &at[175]

#define FEAT_ID &at[2]
#define FEAT_ID_gibb &at[3]
#define FEAT_ID_giim &at[5]
#define FEAT_ID_local &at[7]
#define FEAT_ID_general &at[9]

#define SEQFEATDATA &at[13]
#define SEQFEATDATA_gene &at[14]
#define SEQFEATDATA_org &at[31]
#define SEQFEATDATA_cdregion &at[43]
#define SEQFEATDATA_prot &at[74]
#define SEQFEATDATA_rna &at[87]
#define SEQFEATDATA_pub &at[103]
#define SEQFEATDATA_seq &at[105]
#define SEQFEATDATA_imp &at[106]
#define SEQFEATDATA_region &at[111]
#define SEQFEATDATA_comment &at[112]
#define SEQFEATDATA_bond &at[114]
#define SEQFEATDATA_site &at[115]
#define SEQFEATDATA_rsite &at[116]
#define SEQFEATDATA_user &at[122]
#define SEQFEATDATA_txinit &at[124]
#define SEQFEATDATA_num &at[153]
#define SEQFEATDATA_psec_str &at[155]
#define SEQFEATDATA_non_std_residue &at[156]
#define SEQFEATDATA_het &at[157]

#define GB_QUAL &at[166]
#define GB_QUAL_qual &at[167]
#define GB_QUAL_val &at[168]

#define SEQFEATXREF &at[176]
#define SEQFEATXREF_id &at[177]
#define SEQFEATXREF_data &at[178]

#define CDREGION &at[44]
#define CDREGION_orf &at[45]
#define CDREGION_frame &at[46]
#define CDREGION_conflict &at[48]
#define CDREGION_gaps &at[49]
#define CDREGION_mismatch &at[50]
#define CDREGION_code &at[51]
#define CDREGION_code_break &at[63]
#define CDREGION_code_break_E &at[64]
#define CDREGION_stops &at[73]

#define IMP_FEAT &at[107]
#define IMP_FEAT_key &at[108]
#define IMP_FEAT_loc &at[109]
#define IMP_FEAT_descr &at[110]

#define GENETIC_CODE &at[52]
#define GENETIC_CODE_E &at[53]
#define GENETIC_CODE_E_name &at[54]
#define GENETIC_CODE_E_id &at[55]
#define GENETIC_CODE_E_ncbieaa &at[56]
#define GENETIC_CODE_E_ncbi8aa &at[57]
#define GENETIC_CODE_E_ncbistdaa &at[59]
#define GENETIC_CODE_E_sncbieaa &at[60]
#define GENETIC_CODE_E_sncbi8aa &at[61]
#define GENETIC_CODE_E_sncbistdaa &at[62]

#define CODE_BREAK &at[65]
#define CODE_BREAK_loc &at[66]
#define CODE_BREAK_aa &at[68]
#define CODE_BREAK_aa_ncbieaa &at[69]
#define CODE_BREAK_aa_ncbi8aa &at[70]
#define CODE_BREAK_aa_ncbistdaa &at[71]

#define GENETIC_CODE_TABLE &at[179]
#define GENETIC_CODE_TABLE_E &at[180]


/**************************************************
*
*    Defines for Module NCBI-Rsite
*
**************************************************/

#define RSITE_REF &at[118]
#define RSITE_REF_str &at[119]
#define RSITE_REF_db &at[120]


/**************************************************
*
*    Defines for Module NCBI-RNA
*
**************************************************/

#define RNA_REF &at[89]
#define RNA_REF_type &at[90]
#define RNA_REF_pseudo &at[91]
#define RNA_REF_ext &at[92]
#define RNA_REF_ext_name &at[93]
#define RNA_REF_ext_tRNA &at[94]

#define TRNA_EXT &at[95]
#define TRNA_EXT_aa &at[96]
#define TRNA_EXT_aa_iupacaa &at[97]
#define TRNA_EXT_aa_ncbieaa &at[98]
#define TRNA_EXT_aa_ncbi8aa &at[99]
#define TRNA_EXT_aa_ncbistdaa &at[100]
#define TRNA_EXT_codon &at[101]
#define TRNA_EXT_codon_E &at[102]


/**************************************************
*
*    Defines for Module NCBI-Gene
*
**************************************************/

#define GENE_REF &at[16]
#define GENE_REF_locus &at[17]
#define GENE_REF_allele &at[19]
#define GENE_REF_desc &at[20]
#define GENE_REF_maploc &at[21]
#define GENE_REF_pseudo &at[22]
#define GENE_REF_db &at[24]
#define GENE_REF_db_E &at[25]
#define GENE_REF_syn &at[28]
#define GENE_REF_syn_E &at[29]


/**************************************************
*
*    Defines for Module NCBI-Organism
*
**************************************************/

#define ORG_REF &at[33]
#define ORG_REF_taxname &at[34]
#define ORG_REF_common &at[35]
#define ORG_REF_mod &at[36]
#define ORG_REF_mod_E &at[37]
#define ORG_REF_db &at[38]
#define ORG_REF_db_E &at[39]
#define ORG_REF_syn &at[41]
#define ORG_REF_syn_E &at[42]


/**************************************************
*
*    Defines for Module NCBI-Protein
*
**************************************************/

#define PROT_REF &at[76]
#define PROT_REF_name &at[77]
#define PROT_REF_name_E &at[78]
#define PROT_REF_desc &at[79]
#define PROT_REF_ec &at[80]
#define PROT_REF_ec_E &at[81]
#define PROT_REF_activity &at[82]
#define PROT_REF_activity_E &at[83]
#define PROT_REF_db &at[84]
#define PROT_REF_db_E &at[85]


/**************************************************
*
*    Defines for Module NCBI-TxInit
*
**************************************************/

#define TXINIT &at[126]
#define TXINIT_name &at[127]
#define TXINIT_syn &at[128]
#define TXINIT_syn_E &at[129]
#define TXINIT_gene &at[130]
#define TXINIT_gene_E &at[131]
#define TXINIT_protein &at[133]
#define TXINIT_protein_E &at[134]
#define TXINIT_rna &at[136]
#define TXINIT_rna_E &at[137]
#define TXINIT_expression &at[138]
#define TXINIT_txsystem &at[139]
#define TXINIT_txdescr &at[140]
#define TXINIT_txorg &at[141]
#define TXINIT_mapping_precise &at[143]
#define TXINIT_location_accurate &at[144]
#define TXINIT_inittype &at[145]
#define TXINIT_evidence &at[146]
#define TXINIT_evidence_E &at[147]

#define TX_EVIDENCE &at[148]
#define TX_EVIDENCE_exp_code &at[149]
#define TX_EVIDENCE_expression_system &at[150]
#define TX_EVIDENCE_low_prec_data &at[151]
#define TX_EVIDENCE_from_homolog &at[152]
