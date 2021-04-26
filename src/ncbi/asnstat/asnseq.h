/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "asnseq.h01";
static AsnValxNode avnx[73] = {
    {20,"unknown" ,0,0.0,&avnx[1] } ,
    {20,"genomic" ,1,0.0,&avnx[2] } ,
    {20,"pre-mRNA" ,2,0.0,&avnx[3] } ,
    {20,"mRNA" ,3,0.0,&avnx[4] } ,
    {20,"rRNA" ,4,0.0,&avnx[5] } ,
    {20,"tRNA" ,5,0.0,&avnx[6] } ,
    {20,"snRNA" ,6,0.0,&avnx[7] } ,
    {20,"scRNA" ,7,0.0,&avnx[8] } ,
    {20,"peptide" ,8,0.0,&avnx[9] } ,
    {20,"other-genetic" ,9,0.0,&avnx[10] } ,
    {20,"genomic-mRNA" ,10,0.0,&avnx[11] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"dna" ,0,0.0,&avnx[13] } ,
    {20,"rna" ,1,0.0,&avnx[14] } ,
    {20,"extrachrom" ,2,0.0,&avnx[15] } ,
    {20,"plasmid" ,3,0.0,&avnx[16] } ,
    {20,"mitochondrial" ,4,0.0,&avnx[17] } ,
    {20,"chloroplast" ,5,0.0,&avnx[18] } ,
    {20,"kinetoplast" ,6,0.0,&avnx[19] } ,
    {20,"cyanelle" ,7,0.0,&avnx[20] } ,
    {20,"synthetic" ,8,0.0,&avnx[21] } ,
    {20,"recombinant" ,9,0.0,&avnx[22] } ,
    {20,"partial" ,10,0.0,&avnx[23] } ,
    {20,"complete" ,11,0.0,&avnx[24] } ,
    {20,"mutagen" ,12,0.0,&avnx[25] } ,
    {20,"natmut" ,13,0.0,&avnx[26] } ,
    {20,"transposon" ,14,0.0,&avnx[27] } ,
    {20,"insertion-seq" ,15,0.0,&avnx[28] } ,
    {20,"no-left" ,16,0.0,&avnx[29] } ,
    {20,"no-right" ,17,0.0,&avnx[30] } ,
    {20,"macronuclear" ,18,0.0,&avnx[31] } ,
    {20,"proviral" ,19,0.0,&avnx[32] } ,
    {20,"est" ,20,0.0,&avnx[33] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"concept-trans" ,1,0.0,&avnx[35] } ,
    {20,"seq-pept" ,2,0.0,&avnx[36] } ,
    {20,"both" ,3,0.0,&avnx[37] } ,
    {20,"seq-pept-overlap" ,4,0.0,&avnx[38] } ,
    {20,"seq-pept-homol" ,5,0.0,&avnx[39] } ,
    {20,"concept-trans-a" ,6,0.0,&avnx[40] } ,
    {20,"other" ,255,0.0,NULL } ,
    {3,NULL,1,0.0,NULL } ,
    {2,NULL,0,0.0,NULL } ,
    {2,NULL,1,0.0,NULL } ,
    {20,"not-set" ,0,0.0,&avnx[45] } ,
    {20,"sources" ,1,0.0,&avnx[46] } ,
    {20,"aligns" ,2,0.0,NULL } ,
    {20,"not-set" ,0,0.0,&avnx[48] } ,
    {20,"virtual" ,1,0.0,&avnx[49] } ,
    {20,"raw" ,2,0.0,&avnx[50] } ,
    {20,"seg" ,3,0.0,&avnx[51] } ,
    {20,"const" ,4,0.0,&avnx[52] } ,
    {20,"ref" ,5,0.0,&avnx[53] } ,
    {20,"consen" ,6,0.0,&avnx[54] } ,
    {20,"map" ,7,0.0,&avnx[55] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"not-set" ,0,0.0,&avnx[57] } ,
    {20,"dna" ,1,0.0,&avnx[58] } ,
    {20,"rna" ,2,0.0,&avnx[59] } ,
    {20,"aa" ,3,0.0,&avnx[60] } ,
    {20,"na" ,4,0.0,&avnx[61] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"not-set" ,0,0.0,&avnx[63] } ,
    {20,"linear" ,1,0.0,&avnx[64] } ,
    {20,"circular" ,2,0.0,&avnx[65] } ,
    {20,"tandem" ,3,0.0,&avnx[66] } ,
    {20,"other" ,255,0.0,NULL } ,
    {3,NULL,1,0.0,NULL } ,
    {20,"not-set" ,0,0.0,&avnx[69] } ,
    {20,"ss" ,1,0.0,&avnx[70] } ,
    {20,"ds" ,2,0.0,&avnx[71] } ,
    {20,"mixed" ,3,0.0,&avnx[72] } ,
    {20,"other" ,255,0.0,NULL } };

static AsnType atx[161] = {
  {401, "Bioseq" ,1,0,0,0,0,1,0,0,NULL,&atx[31],&atx[1],0,&atx[147]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[4],&atx[2],0,&atx[5]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[3],NULL,0,NULL} ,
  {417, "Seq-id" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[125]} ,
  {314, "SET OF" ,0,17,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "descr" ,128,1,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[88]} ,
  {404, "Seq-descr" ,1,0,0,0,0,1,0,0,NULL,&atx[4],&atx[7],0,&atx[23]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[49],&atx[8],0,NULL} ,
  {0, "mol-type" ,128,0,0,0,0,0,0,0,NULL,&atx[9],NULL,0,&atx[11]} ,
  {427, "GIBB-mol" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&avnx[0],0,&atx[13]} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "modif" ,128,1,0,0,0,0,0,0,NULL,&atx[4],&atx[12],0,&atx[14]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[13],NULL,0,NULL} ,
  {428, "GIBB-mod" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&avnx[12],0,&atx[15]} ,
  {0, "method" ,128,2,0,0,0,0,0,0,NULL,&atx[15],NULL,0,&atx[16]} ,
  {429, "GIBB-method" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&avnx[34],0,&atx[25]} ,
  {0, "name" ,128,3,0,0,0,0,0,0,NULL,&atx[17],NULL,0,&atx[18]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "title" ,128,4,0,0,0,0,0,0,NULL,&atx[17],NULL,0,&atx[19]} ,
  {0, "org" ,128,5,0,0,0,0,0,0,NULL,&atx[20],NULL,0,&atx[21]} ,
  {416, "Org-ref" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[3]} ,
  {0, "comment" ,128,6,0,0,0,0,0,0,NULL,&atx[17],NULL,0,&atx[22]} ,
  {0, "num" ,128,7,0,0,0,0,0,0,NULL,&atx[23],NULL,0,&atx[50]} ,
  {405, "Numbering" ,1,0,0,0,0,1,0,0,NULL,&atx[49],&atx[24],0,&atx[87]} ,
  {0, "cont" ,128,0,0,0,0,0,0,0,NULL,&atx[25],NULL,0,&atx[32]} ,
  {430, "Num-cont" ,1,0,0,0,0,0,0,0,NULL,&atx[31],&atx[26],0,&atx[33]} ,
  {0, "refnum" ,128,0,0,0,1,0,0,0,&avnx[41],&atx[27],NULL,0,&atx[28]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "has-zero" ,128,1,0,0,1,0,0,0,&avnx[42],&atx[29],NULL,0,&atx[30]} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "ascending" ,128,2,0,0,1,0,0,0,&avnx[43],&atx[29],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "enum" ,128,1,0,0,0,0,0,0,NULL,&atx[33],NULL,0,&atx[38]} ,
  {431, "Num-enum" ,1,0,0,0,0,0,0,0,NULL,&atx[31],&atx[34],0,&atx[39]} ,
  {0, "num" ,128,0,0,0,0,0,0,0,NULL,&atx[27],NULL,0,&atx[35]} ,
  {0, "names" ,128,1,0,0,0,0,0,0,NULL,&atx[37],&atx[36],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[17],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "ref" ,128,2,0,0,0,0,0,0,NULL,&atx[39],NULL,0,&atx[43]} ,
  {432, "Num-ref" ,1,0,0,0,0,0,0,0,NULL,&atx[31],&atx[40],0,&atx[44]} ,
  {0, "type" ,128,0,0,0,0,0,0,0,NULL,&atx[10],&avnx[44],0,&atx[41]} ,
  {0, "aligns" ,128,1,0,1,0,0,0,0,NULL,&atx[42],NULL,0,NULL} ,
  {412, "Seq-align" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[131]} ,
  {0, "real" ,128,3,0,0,0,0,0,0,NULL,&atx[44],NULL,0,NULL} ,
  {433, "Num-real" ,1,0,0,0,0,0,0,0,NULL,&atx[31],&atx[45],0,&atx[98]} ,
  {0, "a" ,128,0,0,0,0,0,0,0,NULL,&atx[46],NULL,0,&atx[47]} ,
  {309, "REAL" ,0,9,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "b" ,128,1,0,0,0,0,0,0,NULL,&atx[46],NULL,0,&atx[48]} ,
  {0, "units" ,128,2,0,1,0,0,0,0,NULL,&atx[17],NULL,0,NULL} ,
  {315, "CHOICE" ,0,-1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "maploc" ,128,8,0,0,0,0,0,0,NULL,&atx[51],NULL,0,&atx[52]} ,
  {409, "Dbtag" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[149]} ,
  {0, "pir" ,128,9,0,0,0,0,0,0,NULL,&atx[53],NULL,0,&atx[54]} ,
  {421, "PIR-block" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[78]} ,
  {0, "genbank" ,128,10,0,0,0,0,0,0,NULL,&atx[55],NULL,0,&atx[56]} ,
  {420, "GB-block" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[53]} ,
  {0, "pub" ,128,11,0,0,0,0,0,0,NULL,&atx[57],NULL,0,&atx[70]} ,
  {403, "Pubdesc" ,1,0,0,0,0,1,0,0,NULL,&atx[31],&atx[58],0,&atx[6]} ,
  {0, "pub" ,128,0,0,0,0,0,0,0,NULL,&atx[59],NULL,0,&atx[60]} ,
  {415, "Pub-equiv" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[20]} ,
  {0, "name" ,128,1,0,1,0,0,0,0,NULL,&atx[17],NULL,0,&atx[61]} ,
  {0, "fig" ,128,2,0,1,0,0,0,0,NULL,&atx[17],NULL,0,&atx[62]} ,
  {0, "num" ,128,3,0,1,0,0,0,0,NULL,&atx[23],NULL,0,&atx[63]} ,
  {0, "numexc" ,128,4,0,1,0,0,0,0,NULL,&atx[29],NULL,0,&atx[64]} ,
  {0, "poly-a" ,128,5,0,1,0,0,0,0,NULL,&atx[29],NULL,0,&atx[65]} ,
  {0, "maploc" ,128,6,0,1,0,0,0,0,NULL,&atx[17],NULL,0,&atx[66]} ,
  {0, "seq-raw" ,128,7,0,1,0,0,0,0,NULL,&atx[67],NULL,0,&atx[68]} ,
  {351, "StringStore" ,64,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "align-group" ,128,8,0,1,0,0,0,0,NULL,&atx[27],NULL,0,&atx[69]} ,
  {0, "comment" ,128,9,0,1,0,0,0,0,NULL,&atx[17],NULL,0,NULL} ,
  {0, "region" ,128,12,0,0,0,0,0,0,NULL,&atx[17],NULL,0,&atx[71]} ,
  {0, "user" ,128,13,0,0,0,0,0,0,NULL,&atx[72],NULL,0,&atx[73]} ,
  {411, "User-object" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[42]} ,
  {0, "sp" ,128,14,0,0,0,0,0,0,NULL,&atx[74],NULL,0,&atx[75]} ,
  {423, "SP-block" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[83]} ,
  {0, "neighbors" ,128,15,0,0,0,0,0,0,NULL,&atx[76],NULL,0,&atx[77]} ,
  {419, "Link-set" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[55]} ,
  {0, "embl" ,128,16,0,0,0,0,0,0,NULL,&atx[78],NULL,0,&atx[79]} ,
  {422, "EMBL-block" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[74]} ,
  {0, "create-date" ,128,17,0,0,0,0,0,0,NULL,&atx[80],NULL,0,&atx[81]} ,
  {407, "Date" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[94]} ,
  {0, "update-date" ,128,18,0,0,0,0,0,0,NULL,&atx[80],NULL,0,&atx[82]} ,
  {0, "prf" ,128,19,0,0,0,0,0,0,NULL,&atx[83],NULL,0,&atx[84]} ,
  {424, "PRF-block" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[85]} ,
  {0, "pdb" ,128,20,0,0,0,0,0,0,NULL,&atx[85],NULL,0,&atx[86]} ,
  {425, "PDB-block" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[89]} ,
  {0, "het" ,128,21,0,0,0,0,0,0,NULL,&atx[87],NULL,0,NULL} ,
  {406, "Heterogen" ,1,0,0,0,0,1,0,0,NULL,&atx[17],NULL,0,&atx[80]} ,
  {0, "inst" ,128,2,0,0,0,0,0,0,NULL,&atx[89],NULL,0,&atx[145]} ,
  {426, "Seq-inst" ,1,0,0,0,0,0,0,0,NULL,&atx[31],&atx[90],0,&atx[9]} ,
  {0, "repr" ,128,0,0,0,0,0,0,0,NULL,&atx[10],&avnx[47],0,&atx[91]} ,
  {0, "mol" ,128,1,0,0,0,0,0,0,NULL,&atx[10],&avnx[56],0,&atx[92]} ,
  {0, "length" ,128,2,0,1,0,0,0,0,NULL,&atx[27],NULL,0,&atx[93]} ,
  {0, "fuzz" ,128,3,0,1,0,0,0,0,NULL,&atx[94],NULL,0,&atx[95]} ,
  {408, "Int-fuzz" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[51]} ,
  {0, "topology" ,128,4,0,0,1,0,0,0,&avnx[67],&atx[10],&avnx[62],0,&atx[96]} ,
  {0, "strand" ,128,5,0,1,0,0,0,0,NULL,&atx[10],&avnx[68],0,&atx[97]} ,
  {0, "seq-data" ,128,6,0,1,0,0,0,0,NULL,&atx[98],NULL,0,&atx[120]} ,
  {434, "Seq-data" ,1,0,0,0,0,0,0,0,NULL,&atx[49],&atx[99],0,&atx[121]} ,
  {0, "iupacna" ,128,0,0,0,0,0,0,0,NULL,&atx[100],NULL,0,&atx[101]} ,
  {441, "IUPACna" ,1,0,0,0,0,0,0,0,NULL,&atx[67],NULL,0,&atx[102]} ,
  {0, "iupacaa" ,128,1,0,0,0,0,0,0,NULL,&atx[102],NULL,0,&atx[103]} ,
  {442, "IUPACaa" ,1,0,0,0,0,0,0,0,NULL,&atx[67],NULL,0,&atx[104]} ,
  {0, "ncbi2na" ,128,2,0,0,0,0,0,0,NULL,&atx[104],NULL,0,&atx[106]} ,
  {443, "NCBI2na" ,1,0,0,0,0,0,0,0,NULL,&atx[105],NULL,0,&atx[107]} ,
  {304, "OCTET STRING" ,0,4,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "ncbi4na" ,128,3,0,0,0,0,0,0,NULL,&atx[107],NULL,0,&atx[108]} ,
  {444, "NCBI4na" ,1,0,0,0,0,0,0,0,NULL,&atx[105],NULL,0,&atx[109]} ,
  {0, "ncbi8na" ,128,4,0,0,0,0,0,0,NULL,&atx[109],NULL,0,&atx[110]} ,
  {445, "NCBI8na" ,1,0,0,0,0,0,0,0,NULL,&atx[105],NULL,0,&atx[111]} ,
  {0, "ncbipna" ,128,5,0,0,0,0,0,0,NULL,&atx[111],NULL,0,&atx[112]} ,
  {446, "NCBIpna" ,1,0,0,0,0,0,0,0,NULL,&atx[105],NULL,0,&atx[113]} ,
  {0, "ncbi8aa" ,128,6,0,0,0,0,0,0,NULL,&atx[113],NULL,0,&atx[114]} ,
  {447, "NCBI8aa" ,1,0,0,0,0,0,0,0,NULL,&atx[105],NULL,0,&atx[115]} ,
  {0, "ncbieaa" ,128,7,0,0,0,0,0,0,NULL,&atx[115],NULL,0,&atx[116]} ,
  {448, "NCBIeaa" ,1,0,0,0,0,0,0,0,NULL,&atx[67],NULL,0,&atx[117]} ,
  {0, "ncbipaa" ,128,8,0,0,0,0,0,0,NULL,&atx[117],NULL,0,&atx[118]} ,
  {449, "NCBIpaa" ,1,0,0,0,0,0,0,0,NULL,&atx[105],NULL,0,&atx[119]} ,
  {0, "ncbistdaa" ,128,9,0,0,0,0,0,0,NULL,&atx[119],NULL,0,NULL} ,
  {450, "NCBIstdaa" ,1,0,0,0,0,0,0,0,NULL,&atx[105],NULL,0,NULL} ,
  {0, "ext" ,128,7,0,1,0,0,0,0,NULL,&atx[121],NULL,0,&atx[132]} ,
  {435, "Seq-ext" ,1,0,0,0,0,0,0,0,NULL,&atx[49],&atx[122],0,&atx[133]} ,
  {0, "seg" ,128,0,0,0,0,0,0,0,NULL,&atx[123],NULL,0,&atx[126]} ,
  {437, "Seg-ext" ,1,0,0,0,0,0,0,0,NULL,&atx[37],&atx[124],0,&atx[127]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[125],NULL,0,NULL} ,
  {418, "Seq-loc" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[76]} ,
  {0, "ref" ,128,1,0,0,0,0,0,0,NULL,&atx[127],NULL,0,&atx[128]} ,
  {438, "Ref-ext" ,1,0,0,0,0,0,0,0,NULL,&atx[125],NULL,0,&atx[129]} ,
  {0, "map" ,128,2,0,0,0,0,0,0,NULL,&atx[129],NULL,0,NULL} ,
  {439, "Map-ext" ,1,0,0,0,0,0,0,0,NULL,&atx[37],&atx[130],0,&atx[137]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[131],NULL,0,NULL} ,
  {413, "Seq-feat" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[160]} ,
  {0, "hist" ,128,8,0,1,0,0,0,0,NULL,&atx[133],NULL,0,NULL} ,
  {436, "Seq-hist" ,1,0,0,0,0,0,0,0,NULL,&atx[31],&atx[134],0,&atx[123]} ,
  {0, "assembly" ,128,0,0,1,0,0,0,0,NULL,&atx[4],&atx[135],0,&atx[136]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[42],NULL,0,NULL} ,
  {0, "replaces" ,128,1,0,1,0,0,0,0,NULL,&atx[137],NULL,0,&atx[141]} ,
  {440, "Seq-hist-rec" ,1,0,0,0,0,0,0,0,NULL,&atx[31],&atx[138],0,&atx[100]} ,
  {0, "date" ,128,0,0,1,0,0,0,0,NULL,&atx[80],NULL,0,&atx[139]} ,
  {0, "ids" ,128,1,0,0,0,0,0,0,NULL,&atx[4],&atx[140],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[3],NULL,0,NULL} ,
  {0, "replaced-by" ,128,2,0,1,0,0,0,0,NULL,&atx[137],NULL,0,&atx[142]} ,
  {0, "deleted" ,128,3,0,1,0,0,0,0,NULL,&atx[49],&atx[143],0,NULL} ,
  {0, "bool" ,128,0,0,0,0,0,0,0,NULL,&atx[29],NULL,0,&atx[144]} ,
  {0, "date" ,128,1,0,0,0,0,0,0,NULL,&atx[80],NULL,0,NULL} ,
  {0, "annot" ,128,3,0,1,0,0,0,0,NULL,&atx[4],&atx[146],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[147],NULL,0,NULL} ,
  {402, "Seq-annot" ,1,0,0,0,0,1,0,0,NULL,&atx[31],&atx[148],0,&atx[57]} ,
  {0, "id" ,128,0,0,1,0,0,0,0,NULL,&atx[149],NULL,0,&atx[150]} ,
  {410, "Object-id" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[72]} ,
  {0, "db" ,128,1,0,1,0,0,0,0,NULL,&atx[51],NULL,0,&atx[151]} ,
  {0, "name" ,128,2,0,1,0,0,0,0,NULL,&atx[17],NULL,0,&atx[152]} ,
  {0, "desc" ,128,3,0,1,0,0,0,0,NULL,&atx[17],NULL,0,&atx[153]} ,
  {0, "data" ,128,4,0,0,0,0,0,0,NULL,&atx[49],&atx[154],0,NULL} ,
  {0, "ftable" ,128,0,0,0,0,0,0,0,NULL,&atx[4],&atx[155],0,&atx[156]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[131],NULL,0,NULL} ,
  {0, "align" ,128,1,0,0,0,0,0,0,NULL,&atx[4],&atx[157],0,&atx[158]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[42],NULL,0,NULL} ,
  {0, "graph" ,128,2,0,0,0,0,0,0,NULL,&atx[4],&atx[159],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[160],NULL,0,NULL} ,
  {414, "Seq-graph" ,1,0,0,0,0,0,1,0,NULL,NULL,NULL,0,&atx[59]} };

static AsnModule ampx[1] = {
  { "NCBI-Sequence" , "asnseq.h01",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module NCBI-Sequence
*
**************************************************/

#define BIOSEQ &at[0]
#define BIOSEQ_id &at[1]
#define BIOSEQ_id_E &at[2]
#define BIOSEQ_descr &at[5]
#define BIOSEQ_inst &at[88]
#define BIOSEQ_annot &at[145]
#define BIOSEQ_annot_E &at[146]

#define SEQ_ANNOT &at[147]
#define SEQ_ANNOT_id &at[148]
#define SEQ_ANNOT_db &at[150]
#define SEQ_ANNOT_name &at[151]
#define SEQ_ANNOT_desc &at[152]
#define SEQ_ANNOT_data &at[153]
#define SEQ_ANNOT_data_ftable &at[154]
#define SEQ_ANNOT_data_ftable_E &at[155]
#define SEQ_ANNOT_data_align &at[156]
#define SEQ_ANNOT_data_align_E &at[157]
#define SEQ_ANNOT_data_graph &at[158]
#define SEQ_ANNOT_data_graph_E &at[159]

#define PUBDESC &at[57]
#define PUBDESC_pub &at[58]
#define PUBDESC_name &at[60]
#define PUBDESC_fig &at[61]
#define PUBDESC_num &at[62]
#define PUBDESC_numexc &at[63]
#define PUBDESC_poly_a &at[64]
#define PUBDESC_maploc &at[65]
#define PUBDESC_seq_raw &at[66]
#define PUBDESC_align_group &at[68]
#define PUBDESC_comment &at[69]

#define SEQ_DESCR &at[6]
#define SEQ_DESCR_E &at[7]
#define SEQ_DESCR_E_mol_type &at[8]
#define SEQ_DESCR_E_modif &at[11]
#define SEQ_DESCR_E_modif_E &at[12]
#define SEQ_DESCR_E_method &at[14]
#define SEQ_DESCR_E_name &at[16]
#define SEQ_DESCR_E_title &at[18]
#define SEQ_DESCR_E_org &at[19]
#define SEQ_DESCR_E_comment &at[21]
#define SEQ_DESCR_E_num &at[22]
#define SEQ_DESCR_E_maploc &at[50]
#define SEQ_DESCR_E_pir &at[52]
#define SEQ_DESCR_E_genbank &at[54]
#define SEQ_DESCR_E_pub &at[56]
#define SEQ_DESCR_E_region &at[70]
#define SEQ_DESCR_E_user &at[71]
#define SEQ_DESCR_E_sp &at[73]
#define SEQ_DESCR_E_neighbors &at[75]
#define SEQ_DESCR_E_embl &at[77]
#define SEQ_DESCR_E_create_date &at[79]
#define SEQ_DESCR_E_update_date &at[81]
#define SEQ_DESCR_E_prf &at[82]
#define SEQ_DESCR_E_pdb &at[84]
#define SEQ_DESCR_E_het &at[86]

#define NUMBERING &at[23]
#define NUMBERING_cont &at[24]
#define NUMBERING_enum &at[32]
#define NUMBERING_ref &at[38]
#define NUMBERING_real &at[43]

#define HETEROGEN &at[87]

#define SEQ_INST &at[89]
#define SEQ_INST_repr &at[90]
#define SEQ_INST_mol &at[91]
#define SEQ_INST_length &at[92]
#define SEQ_INST_fuzz &at[93]
#define SEQ_INST_topology &at[95]
#define SEQ_INST_strand &at[96]
#define SEQ_INST_seq_data &at[97]
#define SEQ_INST_ext &at[120]
#define SEQ_INST_hist &at[132]

#define GIBB_MOL &at[9]

#define GIBB_MOD &at[13]

#define GIBB_METHOD &at[15]

#define NUM_CONT &at[25]
#define NUM_CONT_refnum &at[26]
#define NUM_CONT_has_zero &at[28]
#define NUM_CONT_ascending &at[30]

#define NUM_ENUM &at[33]
#define NUM_ENUM_num &at[34]
#define NUM_ENUM_names &at[35]
#define NUM_ENUM_names_E &at[36]

#define NUM_REF &at[39]
#define NUM_REF_type &at[40]
#define NUM_REF_aligns &at[41]

#define NUM_REAL &at[44]
#define NUM_REAL_a &at[45]
#define NUM_REAL_b &at[47]
#define NUM_REAL_units &at[48]

#define SEQ_DATA &at[98]
#define SEQ_DATA_iupacna &at[99]
#define SEQ_DATA_iupacaa &at[101]
#define SEQ_DATA_ncbi2na &at[103]
#define SEQ_DATA_ncbi4na &at[106]
#define SEQ_DATA_ncbi8na &at[108]
#define SEQ_DATA_ncbipna &at[110]
#define SEQ_DATA_ncbi8aa &at[112]
#define SEQ_DATA_ncbieaa &at[114]
#define SEQ_DATA_ncbipaa &at[116]
#define SEQ_DATA_ncbistdaa &at[118]

#define SEQ_EXT &at[121]
#define SEQ_EXT_seg &at[122]
#define SEQ_EXT_ref &at[126]
#define SEQ_EXT_map &at[128]

#define SEQ_HIST &at[133]
#define SEQ_HIST_assembly &at[134]
#define SEQ_HIST_assembly_E &at[135]
#define SEQ_HIST_replaces &at[136]
#define SEQ_HIST_replaced_by &at[141]
#define SEQ_HIST_deleted &at[142]
#define SEQ_HIST_deleted_bool &at[143]
#define SEQ_HIST_deleted_date &at[144]

#define SEG_EXT &at[123]
#define SEG_EXT_E &at[124]

#define REF_EXT &at[127]

#define MAP_EXT &at[129]
#define MAP_EXT_E &at[130]

#define SEQ_HIST_REC &at[137]
#define SEQ_HIST_REC_date &at[138]
#define SEQ_HIST_REC_ids &at[139]
#define SEQ_HIST_REC_ids_E &at[140]

#define IUPACNA &at[100]

#define IUPACAA &at[102]

#define NCBI2NA &at[104]

#define NCBI4NA &at[107]

#define NCBI8NA &at[109]

#define NCBIPNA &at[111]

#define NCBI8AA &at[113]

#define NCBIEAA &at[115]

#define NCBIPAA &at[117]

#define NCBISTDAA &at[119]
