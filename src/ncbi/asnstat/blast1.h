/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "blast1.h01";
static AsnValxNode avnx[46] = {
    {20,"circular" ,1,0.0,&avnx[1] } ,
    {20,"single-stranded" ,2,0.0,&avnx[2] } ,
    {20,"rna" ,3,0.0,&avnx[3] } ,
    {20,"modified" ,4,0.0,NULL } ,
    {20,"minus" ,-1,0.0,&avnx[5] } ,
    {20,"none" ,0,0.0,&avnx[6] } ,
    {20,"plus" ,1,0.0,NULL } ,
    {20,"not-set" ,0,0.0,&avnx[8] } ,
    {20,"amino-acid" ,1,0.0,&avnx[9] } ,
    {20,"nucleic-acid" ,2,0.0,&avnx[10] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"none" ,0,0.0,&avnx[12] } ,
    {20,"sumofp" ,1,0.0,&avnx[13] } ,
    {20,"truncated" ,2,0.0,NULL } ,
    {20,"not-set" ,0,0.0,&avnx[15] } ,
    {20,"sequences" ,1,0.0,&avnx[16] } ,
    {20,"threecomps" ,2,0.0,NULL } ,
    {20,"none" ,0,0.0,&avnx[18] } ,
    {20,"undefined" ,1,0.0,&avnx[19] } ,
    {20,"memory" ,2,0.0,&avnx[20] } ,
    {20,"cputime" ,3,0.0,&avnx[21] } ,
    {20,"terminated" ,4,0.0,&avnx[22] } ,
    {20,"parmname" ,5,0.0,&avnx[23] } ,
    {20,"domain" ,6,0.0,&avnx[24] } ,
    {20,"range" ,7,0.0,&avnx[25] } ,
    {20,"fopen" ,8,0.0,&avnx[26] } ,
    {20,"wordsize" ,9,0.0,&avnx[27] } ,
    {20,"dfalib" ,10,0.0,&avnx[28] } ,
    {20,"nowords" ,11,0.0,&avnx[29] } ,
    {20,"hspcnt" ,12,0.0,&avnx[30] } ,
    {20,"diagcnt" ,13,0.0,&avnx[31] } ,
    {20,"mfile" ,14,0.0,&avnx[32] } ,
    {20,"subfile" ,15,0.0,&avnx[33] } ,
    {20,"scoring" ,16,0.0,&avnx[34] } ,
    {20,"querylen" ,17,0.0,&avnx[35] } ,
    {20,"dbase" ,18,0.0,&avnx[36] } ,
    {20,"record3" ,19,0.0,&avnx[37] } ,
    {20,"remember" ,20,0.0,&avnx[38] } ,
    {20,"mpfork" ,21,0.0,&avnx[39] } ,
    {20,"q-letter" ,22,0.0,NULL } ,
    {20,"not-set" ,0,0.0,&avnx[41] } ,
    {20,"score" ,1,0.0,&avnx[42] } ,
    {20,"fscore" ,2,0.0,&avnx[43] } ,
    {20,"p-value" ,3,0.0,&avnx[44] } ,
    {20,"expect" ,4,0.0,&avnx[45] } ,
    {20,"n" ,5,0.0,NULL } };

static AsnType atx[179] = {
  {401, "BLAST0-request" ,1,0,0,0,0,0,0,0,NULL,&atx[21],&atx[1],0,&atx[6]} ,
  {0, "hello" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[3]} ,
  {305, "NULL" ,0,5,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "dblist" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[4]} ,
  {0, "proglist" ,128,2,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[5]} ,
  {0, "search" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[56]} ,
  {402, "BLAST0-search" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[7],0,&atx[57]} ,
  {0, "program" ,128,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[9]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "database" ,128,1,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[10]} ,
  {0, "query" ,128,2,0,0,0,0,0,0,NULL,&atx[11],NULL,0,&atx[37]} ,
  {408, "BLAST0-sequence" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[12],0,&atx[38]} ,
  {0, "desc" ,128,0,0,0,0,0,0,0,NULL,&atx[25],&atx[13],0,&atx[26]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,NULL} ,
  {427, "Seq-desc" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[15],0,&atx[29]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[16],NULL,0,&atx[23]} ,
  {429, "BLAST0-seq-id" ,1,0,0,0,0,0,0,0,NULL,&atx[22],&atx[17],0,&atx[163]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[21],&atx[18],0,NULL} ,
  {0, "giid" ,128,0,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[20]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "textid" ,128,1,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {315, "CHOICE" ,0,-1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {314, "SET OF" ,0,17,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "defline" ,128,1,0,1,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "length" ,128,1,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[27]} ,
  {0, "attrib" ,128,2,0,1,0,0,0,0,NULL,&atx[25],&atx[28],0,&atx[31]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[29],NULL,0,NULL} ,
  {428, "BLAST0-attrib" ,1,0,0,0,0,0,0,0,NULL,&atx[30],&avnx[0],0,&atx[16]} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "seq" ,128,3,0,1,0,0,0,0,NULL,&atx[32],NULL,0,NULL} ,
  {426, "BLAST0-seq-data" ,1,0,0,0,0,0,0,0,NULL,&atx[21],&atx[33],0,&atx[14]} ,
  {0, "ncbistdaa" ,128,0,0,0,0,0,0,0,NULL,&atx[34],NULL,0,&atx[35]} ,
  {304, "OCTET STRING" ,0,4,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "ncbi2na" ,128,1,0,0,0,0,0,0,NULL,&atx[34],NULL,0,&atx[36]} ,
  {0, "ncbi4na" ,128,2,0,0,0,0,0,0,NULL,&atx[34],NULL,0,NULL} ,
  {0, "mask" ,128,3,0,1,0,0,0,0,NULL,&atx[38],NULL,0,&atx[42]} ,
  {409, "BLAST0-seq-interval" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[39],0,&atx[43]} ,
  {0, "strand" ,128,0,0,0,0,0,0,0,NULL,&atx[30],&avnx[4],0,&atx[40]} ,
  {0, "from" ,128,1,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[41]} ,
  {0, "to" ,128,2,0,0,0,0,0,0,NULL,&atx[19],NULL,0,NULL} ,
  {0, "control" ,128,4,0,1,0,0,0,0,NULL,&atx[43],NULL,0,&atx[54]} ,
  {410, "BLAST0-control" ,1,0,0,0,0,0,0,0,NULL,&atx[22],&atx[44],0,&atx[85]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[21],&atx[45],0,NULL} ,
  {0, "ids" ,128,0,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[46]} ,
  {0, "defs" ,128,1,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[47]} ,
  {0, "q-def" ,128,2,0,0,0,0,0,0,NULL,&atx[48],NULL,0,&atx[49]} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "q-seg" ,128,3,0,0,0,0,0,0,NULL,&atx[48],NULL,0,&atx[50]} ,
  {0, "q-seg-nox" ,128,4,0,0,0,0,0,0,NULL,&atx[48],NULL,0,&atx[51]} ,
  {0, "t-def" ,128,5,0,0,0,0,0,0,NULL,&atx[48],NULL,0,&atx[52]} ,
  {0, "t-seg" ,128,6,0,0,0,0,0,0,NULL,&atx[48],NULL,0,&atx[53]} ,
  {0, "t-seg-nox" ,128,7,0,0,0,0,0,0,NULL,&atx[48],NULL,0,NULL} ,
  {0, "options" ,128,5,0,1,0,0,0,0,NULL,&atx[25],&atx[55],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {0, "goodbye" ,128,4,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {403, "BLAST0-response" ,1,0,0,0,0,0,0,0,NULL,&atx[21],&atx[58],0,&atx[62]} ,
  {0, "hello" ,128,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[59]} ,
  {0, "down" ,128,1,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[60]} ,
  {0, "dblist" ,128,2,0,0,0,0,0,0,NULL,&atx[25],&atx[61],0,&atx[71]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[62],NULL,0,NULL} ,
  {404, "BLAST0-dbdesc" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[63],0,&atx[73]} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[64]} ,
  {0, "type" ,128,1,0,0,0,0,0,0,NULL,&atx[65],NULL,0,&atx[66]} ,
  {419, "BLAST0-Alphatype" ,1,0,0,0,0,0,0,0,NULL,&atx[30],&avnx[7],0,&atx[117]} ,
  {0, "def" ,128,2,0,1,0,0,0,0,NULL,&atx[8],NULL,0,&atx[67]} ,
  {0, "dev-date" ,128,3,0,1,0,0,0,0,NULL,&atx[8],NULL,0,&atx[68]} ,
  {0, "bld-date" ,128,4,0,1,0,0,0,0,NULL,&atx[8],NULL,0,&atx[69]} ,
  {0, "count" ,128,5,0,1,0,0,0,0,NULL,&atx[19],NULL,0,&atx[70]} ,
  {0, "length" ,128,6,0,1,0,0,0,0,NULL,&atx[19],NULL,0,NULL} ,
  {0, "proglist" ,128,3,0,0,0,0,0,0,NULL,&atx[25],&atx[72],0,&atx[76]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[73],NULL,0,NULL} ,
  {405, "BLAST0-progdesc" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[74],0,&atx[78]} ,
  {0, "program" ,128,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[75]} ,
  {0, "desc" ,128,1,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {0, "queued" ,128,4,0,0,0,0,0,0,NULL,&atx[25],&atx[77],0,&atx[81]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[78],NULL,0,NULL} ,
  {406, "BLAST0-queued" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[79],0,&atx[82]} ,
  {0, "desc" ,128,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[80]} ,
  {0, "length" ,128,1,0,0,0,0,0,0,NULL,&atx[19],NULL,0,NULL} ,
  {0, "outblk" ,128,5,0,0,0,0,0,0,NULL,&atx[82],NULL,0,&atx[162]} ,
  {407, "BLAST0-outblk" ,1,0,0,0,0,0,0,0,NULL,&atx[22],&atx[83],0,&atx[11]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[21],&atx[84],0,NULL} ,
  {0, "preface" ,128,0,0,0,0,0,0,0,NULL,&atx[85],NULL,0,&atx[94]} ,
  {411, "BLAST0-preface" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[86],0,&atx[95]} ,
  {0, "program" ,128,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[87]} ,
  {0, "version" ,128,1,0,1,0,0,0,0,NULL,&atx[8],NULL,0,&atx[88]} ,
  {0, "dev-date" ,128,2,0,1,0,0,0,0,NULL,&atx[8],NULL,0,&atx[89]} ,
  {0, "bld-date" ,128,3,0,1,0,0,0,0,NULL,&atx[8],NULL,0,&atx[90]} ,
  {0, "cit" ,128,4,0,1,0,0,0,0,NULL,&atx[25],&atx[91],0,&atx[92]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {0, "notice" ,128,5,0,1,0,0,0,0,NULL,&atx[25],&atx[93],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {0, "query-ack" ,128,1,0,0,0,0,0,0,NULL,&atx[95],NULL,0,&atx[98]} ,
  {412, "BLAST0-query-ack" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[96],0,&atx[99]} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[97]} ,
  {0, "len" ,128,1,0,0,0,0,0,0,NULL,&atx[19],NULL,0,NULL} ,
  {0, "warning" ,128,2,0,0,0,0,0,0,NULL,&atx[99],NULL,0,&atx[102]} ,
  {413, "BLAST0-warning" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[100],0,&atx[104]} ,
  {0, "code" ,128,0,0,0,0,0,0,0,NULL,&atx[30],&avnx[11],0,&atx[101]} ,
  {0, "reason" ,128,1,0,1,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {0, "kablk" ,128,3,0,0,0,0,0,0,NULL,&atx[25],&atx[103],0,&atx[112]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[104],NULL,0,NULL} ,
  {414, "KA-Blk" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[105],0,&atx[114]} ,
  {0, "matid" ,128,0,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[106]} ,
  {0, "q-frame" ,128,1,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[107]} ,
  {0, "t-frame" ,128,2,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[108]} ,
  {0, "lambda" ,128,3,0,0,0,0,0,0,NULL,&atx[109],NULL,0,&atx[110]} ,
  {309, "REAL" ,0,9,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "k" ,128,4,0,0,0,0,0,0,NULL,&atx[109],NULL,0,&atx[111]} ,
  {0, "h" ,128,5,0,0,0,0,0,0,NULL,&atx[109],NULL,0,NULL} ,
  {0, "dbdesc" ,128,4,0,0,0,0,0,0,NULL,&atx[62],NULL,0,&atx[113]} ,
  {0, "job-start" ,128,5,0,0,0,0,0,0,NULL,&atx[114],NULL,0,&atx[119]} ,
  {415, "Job-desc" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[115],0,&atx[120]} ,
  {0, "desc" ,128,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[116]} ,
  {0, "units" ,128,1,0,0,0,0,0,0,NULL,&atx[117],NULL,0,&atx[118]} ,
  {420, "Job-units" ,1,0,0,0,0,0,0,0,NULL,&atx[30],&avnx[14],0,&atx[131]} ,
  {0, "size" ,128,2,0,0,0,0,0,0,NULL,&atx[19],NULL,0,NULL} ,
  {0, "job-progress" ,128,6,0,0,0,0,0,0,NULL,&atx[120],NULL,0,&atx[123]} ,
  {416, "Job-progress" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[121],0,&atx[125]} ,
  {0, "done" ,128,0,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[122]} ,
  {0, "positives" ,128,1,0,1,0,0,0,0,NULL,&atx[19],NULL,0,NULL} ,
  {0, "job-done" ,128,7,0,0,0,0,0,0,NULL,&atx[120],NULL,0,&atx[124]} ,
  {0, "result" ,128,8,0,0,0,0,0,0,NULL,&atx[125],NULL,0,&atx[158]} ,
  {417, "BLAST0-result" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[126],0,&atx[159]} ,
  {0, "parms" ,128,0,0,1,0,0,0,0,NULL,&atx[25],&atx[127],0,&atx[128]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {0, "stats" ,128,1,0,1,0,0,0,0,NULL,&atx[25],&atx[129],0,&atx[130]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {0, "hits" ,128,2,0,0,0,0,0,0,NULL,&atx[131],NULL,0,NULL} ,
  {421, "HitData" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[132],0,&atx[136]} ,
  {0, "count" ,128,0,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[133]} ,
  {0, "dim" ,128,1,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[134]} ,
  {0, "hitlists" ,128,2,0,1,0,0,0,0,NULL,&atx[25],&atx[135],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[136],NULL,0,NULL} ,
  {422, "HitList" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[137],0,&atx[140]} ,
  {0, "count" ,128,0,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[138]} ,
  {0, "hsps" ,128,1,0,0,0,0,0,0,NULL,&atx[25],&atx[139],0,&atx[156]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[140],NULL,0,NULL} ,
  {423, "BLAST0-HSP" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[141],0,&atx[144]} ,
  {0, "matid" ,128,0,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[142]} ,
  {0, "scores" ,128,1,0,0,0,0,0,0,NULL,&atx[25],&atx[143],0,&atx[149]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[144],NULL,0,NULL} ,
  {424, "BLAST0-score" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[145],0,&atx[152]} ,
  {0, "type" ,128,0,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[146]} ,
  {0, "value" ,128,1,0,0,0,0,0,0,NULL,&atx[21],&atx[147],0,NULL} ,
  {0, "i" ,128,0,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[148]} ,
  {0, "r" ,128,1,0,0,0,0,0,0,NULL,&atx[109],NULL,0,NULL} ,
  {0, "len" ,128,2,0,0,0,0,0,0,NULL,&atx[19],NULL,0,&atx[150]} ,
  {0, "segs" ,128,3,0,0,0,0,0,0,NULL,&atx[25],&atx[151],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[152],NULL,0,NULL} ,
  {425, "BLAST0-segment" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[153],0,&atx[32]} ,
  {0, "loc" ,128,0,0,0,0,0,0,0,NULL,&atx[38],NULL,0,&atx[154]} ,
  {0, "str" ,128,1,0,1,0,0,0,0,NULL,&atx[32],NULL,0,&atx[155]} ,
  {0, "str-nox" ,128,2,0,1,0,0,0,0,NULL,&atx[32],NULL,0,NULL} ,
  {0, "seqs" ,128,2,0,0,0,0,0,0,NULL,&atx[25],&atx[157],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[11],NULL,0,NULL} ,
  {0, "status" ,128,9,0,0,0,0,0,0,NULL,&atx[159],NULL,0,NULL} ,
  {418, "BLAST0-status" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[160],0,&atx[65]} ,
  {0, "error" ,128,0,0,0,0,0,0,0,NULL,&atx[30],&avnx[17],0,&atx[161]} ,
  {0, "reason" ,128,1,0,1,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {0, "goodbye" ,128,6,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {430, "HSP-measure" ,1,0,0,0,0,0,0,0,NULL,&atx[30],&avnx[40],0,&atx[164]} ,
  {431, "BLAST0-Matrix" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[165],0,&atx[175]} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,&atx[166]} ,
  {0, "type" ,128,1,0,0,0,0,0,0,NULL,&atx[65],NULL,0,&atx[167]} ,
  {0, "comments" ,128,2,0,1,0,0,0,0,NULL,&atx[25],&atx[168],0,&atx[169]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {0, "rowletters" ,128,3,0,0,0,0,0,0,NULL,&atx[25],&atx[170],0,&atx[171]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {0, "colletters" ,128,4,0,0,0,0,0,0,NULL,&atx[25],&atx[172],0,&atx[173]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[8],NULL,0,NULL} ,
  {0, "scores" ,128,5,0,0,0,0,0,0,NULL,&atx[25],&atx[174],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[109],NULL,0,NULL} ,
  {432, "BLAST0-Database" ,1,0,0,0,0,0,0,0,NULL,&atx[24],&atx[176],0,NULL} ,
  {0, "desc" ,128,0,0,0,0,0,0,0,NULL,&atx[62],NULL,0,&atx[177]} ,
  {0, "seqs" ,128,1,0,0,0,0,0,0,NULL,&atx[25],&atx[178],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[11],NULL,0,NULL} };

static AsnModule ampx[1] = {
  { "NCBI-BLAST0" , "blast1.h01",&atx[0],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module NCBI-BLAST0
*
**************************************************/

#define BLAST0_REQUEST &at[0]
#define BLAST0_REQUEST_hello &at[1]
#define BLAST0_REQUEST_dblist &at[3]
#define BLAST0_REQUEST_proglist &at[4]
#define BLAST0_REQUEST_search &at[5]
#define BLAST0_REQUEST_goodbye &at[56]

#define BLAST0_SEARCH &at[6]
#define BLAST0_SEARCH_program &at[7]
#define BLAST0_SEARCH_database &at[9]
#define BLAST0_SEARCH_query &at[10]
#define BLAST0_SEARCH_mask &at[37]
#define BLAST0_SEARCH_control &at[42]
#define BLAST0_SEARCH_options &at[54]
#define BLAST0_SEARCH_options_E &at[55]

#define BLAST0_RESPONSE &at[57]
#define BLAST0_RESPONSE_hello &at[58]
#define BLAST0_RESPONSE_down &at[59]
#define BLAST0_RESPONSE_dblist &at[60]
#define BLAST0_RESPONSE_dblist_E &at[61]
#define BLAST0_RESPONSE_proglist &at[71]
#define BLAST0_RESPONSE_proglist_E &at[72]
#define BLAST0_RESPONSE_queued &at[76]
#define BLAST0_RESPONSE_queued_E &at[77]
#define BLAST0_RESPONSE_outblk &at[81]
#define BLAST0_RESPONSE_goodbye &at[162]

#define BLAST0_DBDESC &at[62]
#define BLAST0_DBDESC_name &at[63]
#define BLAST0_DBDESC_type &at[64]
#define BLAST0_DBDESC_def &at[66]
#define BLAST0_DBDESC_dev_date &at[67]
#define BLAST0_DBDESC_bld_date &at[68]
#define BLAST0_DBDESC_count &at[69]
#define BLAST0_DBDESC_length &at[70]

#define BLAST0_PROGDESC &at[73]
#define BLAST0_PROGDESC_program &at[74]
#define BLAST0_PROGDESC_desc &at[75]

#define BLAST0_QUEUED &at[78]
#define BLAST0_QUEUED_desc &at[79]
#define BLAST0_QUEUED_length &at[80]

#define BLAST0_OUTBLK &at[82]
#define BLAST0_OUTBLK_E &at[83]
#define BLAST0_OUTBLK_E_preface &at[84]
#define BLAST0_OUTBLK_E_query_ack &at[94]
#define BLAST0_OUTBLK_E_warning &at[98]
#define BLAST0_OUTBLK_E_kablk &at[102]
#define BLAST0_OUTBLK_E_kablk_E &at[103]
#define BLAST0_OUTBLK_E_dbdesc &at[112]
#define BLAST0_OUTBLK_E_job_start &at[113]
#define BLAST0_OUTBLK_E_job_progress &at[119]
#define BLAST0_OUTBLK_E_job_done &at[123]
#define BLAST0_OUTBLK_E_result &at[124]
#define BLAST0_OUTBLK_E_status &at[158]

#define BLAST0_SEQUENCE &at[11]
#define BLAST0_SEQUENCE_desc &at[12]
#define BLAST0_SEQUENCE_desc_E &at[13]
#define BLAST0_SEQUENCE_length &at[26]
#define BLAST0_SEQUENCE_attrib &at[27]
#define BLAST0_SEQUENCE_attrib_E &at[28]
#define BLAST0_SEQUENCE_seq &at[31]

#define BLAST0_SEQ_INTERVAL &at[38]
#define BLAST0_SEQ_INTERVAL_strand &at[39]
#define BLAST0_SEQ_INTERVAL_from &at[40]
#define BLAST0_SEQ_INTERVAL_to &at[41]

#define BLAST0_CONTROL &at[43]
#define BLAST0_CONTROL_E &at[44]
#define BLAST0_CONTROL_E_ids &at[45]
#define BLAST0_CONTROL_E_defs &at[46]
#define BLAST0_CONTROL_E_q_def &at[47]
#define BLAST0_CONTROL_E_q_seg &at[49]
#define BLAST0_CONTROL_E_q_seg_nox &at[50]
#define BLAST0_CONTROL_E_t_def &at[51]
#define BLAST0_CONTROL_E_t_seg &at[52]
#define BLAST0_CONTROL_E_t_seg_nox &at[53]

#define BLAST0_PREFACE &at[85]
#define BLAST0_PREFACE_program &at[86]
#define BLAST0_PREFACE_version &at[87]
#define BLAST0_PREFACE_dev_date &at[88]
#define BLAST0_PREFACE_bld_date &at[89]
#define BLAST0_PREFACE_cit &at[90]
#define BLAST0_PREFACE_cit_E &at[91]
#define BLAST0_PREFACE_notice &at[92]
#define BLAST0_PREFACE_notice_E &at[93]

#define BLAST0_QUERY_ACK &at[95]
#define BLAST0_QUERY_ACK_name &at[96]
#define BLAST0_QUERY_ACK_len &at[97]

#define BLAST0_WARNING &at[99]
#define BLAST0_WARNING_code &at[100]
#define BLAST0_WARNING_reason &at[101]

#define KA_BLK &at[104]
#define KA_BLK_matid &at[105]
#define KA_BLK_q_frame &at[106]
#define KA_BLK_t_frame &at[107]
#define KA_BLK_lambda &at[108]
#define KA_BLK_k &at[110]
#define KA_BLK_h &at[111]

#define JOB_DESC &at[114]
#define JOB_DESC_desc &at[115]
#define JOB_DESC_units &at[116]
#define JOB_DESC_size &at[118]

#define JOB_PROGRESS &at[120]
#define JOB_PROGRESS_done &at[121]
#define JOB_PROGRESS_positives &at[122]

#define BLAST0_RESULT &at[125]
#define BLAST0_RESULT_parms &at[126]
#define BLAST0_RESULT_parms_E &at[127]
#define BLAST0_RESULT_stats &at[128]
#define BLAST0_RESULT_stats_E &at[129]
#define BLAST0_RESULT_hits &at[130]

#define BLAST0_STATUS &at[159]
#define BLAST0_STATUS_error &at[160]
#define BLAST0_STATUS_reason &at[161]

#define BLAST0_ALPHATYPE &at[65]

#define JOB_UNITS &at[117]

#define HITDATA &at[131]
#define HITDATA_count &at[132]
#define HITDATA_dim &at[133]
#define HITDATA_hitlists &at[134]
#define HITDATA_hitlists_E &at[135]

#define HITLIST &at[136]
#define HITLIST_count &at[137]
#define HITLIST_hsps &at[138]
#define HITLIST_hsps_E &at[139]
#define HITLIST_seqs &at[156]
#define HITLIST_seqs_E &at[157]

#define BLAST0_HSP &at[140]
#define BLAST0_HSP_matid &at[141]
#define BLAST0_HSP_scores &at[142]
#define BLAST0_HSP_scores_E &at[143]
#define BLAST0_HSP_len &at[149]
#define BLAST0_HSP_segs &at[150]
#define BLAST0_HSP_segs_E &at[151]

#define BLAST0_SCORE &at[144]
#define BLAST0_SCORE_type &at[145]
#define BLAST0_SCORE_value &at[146]
#define BLAST0_SCORE_value_i &at[147]
#define BLAST0_SCORE_value_r &at[148]

#define BLAST0_SEGMENT &at[152]
#define BLAST0_SEGMENT_loc &at[153]
#define BLAST0_SEGMENT_str &at[154]
#define BLAST0_SEGMENT_str_nox &at[155]

#define BLAST0_SEQ_DATA &at[32]
#define BLAST0_SEQ_DATA_ncbistdaa &at[33]
#define BLAST0_SEQ_DATA_ncbi2na &at[35]
#define BLAST0_SEQ_DATA_ncbi4na &at[36]

#define SEQ_DESC &at[14]
#define SEQ_DESC_id &at[15]
#define SEQ_DESC_defline &at[23]

#define BLAST0_ATTRIB &at[29]

#define BLAST0_SEQ_ID &at[16]
#define BLAST0_SEQ_ID_E &at[17]
#define BLAST0_SEQ_ID_E_giid &at[18]
#define BLAST0_SEQ_ID_E_textid &at[20]

#define HSP_MEASURE &at[163]

#define BLAST0_MATRIX &at[164]
#define BLAST0_MATRIX_name &at[165]
#define BLAST0_MATRIX_type &at[166]
#define BLAST0_MATRIX_comments &at[167]
#define BLAST0_MATRIX_comments_E &at[168]
#define BLAST0_MATRIX_rowletters &at[169]
#define BLAST0_MATRIX_rowletters_E &at[170]
#define BLAST0_MATRIX_colletters &at[171]
#define BLAST0_MATRIX_colletters_E &at[172]
#define BLAST0_MATRIX_scores &at[173]
#define BLAST0_MATRIX_scores_E &at[174]

#define BLAST0_DATABASE &at[175]
#define BLAST0_DATABASE_desc &at[176]
#define BLAST0_DATABASE_seqs &at[177]
#define BLAST0_DATABASE_seqs_E &at[178]
