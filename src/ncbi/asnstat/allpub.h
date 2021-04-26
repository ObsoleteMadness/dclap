/***********************************************************************
*
**
*        Automatic header module from ASNTOOL
*
************************************************************************/

#ifndef _ASNTOOL_
#include <asn.h>
#endif

static char * asnfilename = "allpub.h00";
static AsnValxNode avnx[42] = {
    {20,"unk" ,0,0.0,&avnx[1] } ,
    {20,"gt" ,1,0.0,&avnx[2] } ,
    {20,"lt" ,2,0.0,&avnx[3] } ,
    {20,"tr" ,3,0.0,&avnx[4] } ,
    {20,"tl" ,4,0.0,&avnx[5] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"primary" ,1,0.0,&avnx[7] } ,
    {20,"secondary" ,2,0.0,NULL } ,
    {20,"compiler" ,1,0.0,&avnx[9] } ,
    {20,"editor" ,2,0.0,&avnx[10] } ,
    {20,"patent-assignee" ,3,0.0,&avnx[11] } ,
    {20,"translator" ,4,0.0,NULL } ,
    {1,"ENG" ,0,0.0,NULL } ,
    {20,"submitted" ,1,0.0,&avnx[14] } ,
    {20,"in-press" ,2,0.0,&avnx[15] } ,
    {20,"other" ,255,0.0,NULL } ,
    {20,"paper" ,1,0.0,&avnx[17] } ,
    {20,"tape" ,2,0.0,&avnx[18] } ,
    {20,"floppy" ,3,0.0,&avnx[19] } ,
    {20,"email" ,4,0.0,&avnx[20] } ,
    {20,"other" ,255,0.0,NULL } ,
    {2,NULL,0,0.0,NULL } ,
    {2,NULL,0,0.0,NULL } ,
    {20,"nameonly" ,0,0.0,&avnx[24] } ,
    {20,"cas" ,1,0.0,&avnx[25] } ,
    {20,"ec" ,2,0.0,NULL } ,
    {20,"ddbj" ,1,0.0,&avnx[27] } ,
    {20,"carbbank" ,2,0.0,&avnx[28] } ,
    {20,"embl" ,3,0.0,&avnx[29] } ,
    {20,"hdb" ,4,0.0,&avnx[30] } ,
    {20,"genbank" ,5,0.0,&avnx[31] } ,
    {20,"hgml" ,6,0.0,&avnx[32] } ,
    {20,"mim" ,7,0.0,&avnx[33] } ,
    {20,"msd" ,8,0.0,&avnx[34] } ,
    {20,"pdb" ,9,0.0,&avnx[35] } ,
    {20,"pir" ,10,0.0,&avnx[36] } ,
    {20,"prfseqdb" ,11,0.0,&avnx[37] } ,
    {20,"psd" ,12,0.0,&avnx[38] } ,
    {20,"swissprot" ,13,0.0,NULL } ,
    {20,"manuscript" ,1,0.0,&avnx[40] } ,
    {20,"letter" ,2,0.0,&avnx[41] } ,
    {20,"thesis" ,3,0.0,NULL } };

static AsnType atx[248] = {
  {401, "Date" ,1,0,0,0,0,1,0,0,NULL,&atx[11],&atx[1],0,&atx[12]} ,
  {0, "str" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[3]} ,
  {323, "VisibleString" ,0,26,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "std" ,128,1,0,0,0,0,0,0,NULL,&atx[4],NULL,0,NULL} ,
  {407, "Date-std" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[5],0,&atx[21]} ,
  {0, "year" ,128,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[7]} ,
  {302, "INTEGER" ,0,2,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "month" ,128,1,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[8]} ,
  {0, "day" ,128,2,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[9]} ,
  {0, "season" ,128,3,0,1,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {311, "SEQUENCE" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {315, "CHOICE" ,0,-1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {402, "Person-id" ,1,0,0,0,0,1,0,0,NULL,&atx[11],&atx[13],0,&atx[17]} ,
  {0, "dbtag" ,128,0,0,0,0,0,0,0,NULL,&atx[14],NULL,0,&atx[20]} ,
  {404, "Dbtag" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[15],0,&atx[31]} ,
  {0, "db" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[16]} ,
  {0, "tag" ,128,1,0,0,0,0,0,0,NULL,&atx[17],NULL,0,NULL} ,
  {403, "Object-id" ,1,0,0,0,0,1,0,0,NULL,&atx[11],&atx[18],0,&atx[14]} ,
  {0, "id" ,128,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[19]} ,
  {0, "str" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "name" ,128,1,0,0,0,0,0,0,NULL,&atx[21],NULL,0,&atx[29]} ,
  {408, "Name-std" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[22],0,&atx[44]} ,
  {0, "last" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[23]} ,
  {0, "first" ,128,1,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[24]} ,
  {0, "middle" ,128,2,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[25]} ,
  {0, "full" ,128,3,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[26]} ,
  {0, "initials" ,128,4,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[27]} ,
  {0, "suffix" ,128,5,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[28]} ,
  {0, "title" ,128,6,0,1,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "ml" ,128,2,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[30]} ,
  {0, "str" ,128,3,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {405, "Int-fuzz" ,1,0,0,0,0,1,0,0,NULL,&atx[11],&atx[32],0,&atx[39]} ,
  {0, "p-m" ,128,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[33]} ,
  {0, "range" ,128,1,0,0,0,0,0,0,NULL,&atx[10],&atx[34],0,&atx[36]} ,
  {0, "max" ,128,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[35]} ,
  {0, "min" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {0, "pct" ,128,2,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[37]} ,
  {0, "lim" ,128,3,0,0,0,0,0,0,NULL,&atx[38],&avnx[0],0,NULL} ,
  {310, "ENUMERATED" ,0,10,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {406, "User-object" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[40],0,&atx[4]} ,
  {0, "class" ,128,0,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[41]} ,
  {0, "type" ,128,1,0,0,0,0,0,0,NULL,&atx[17],NULL,0,&atx[42]} ,
  {0, "data" ,128,2,0,0,0,0,0,0,NULL,&atx[59],&atx[43],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[44],NULL,0,NULL} ,
  {409, "User-field" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[45],0,NULL} ,
  {0, "label" ,128,0,0,0,0,0,0,0,NULL,&atx[17],NULL,0,&atx[46]} ,
  {0, "num" ,128,1,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[47]} ,
  {0, "data" ,128,2,0,0,0,0,0,0,NULL,&atx[11],&atx[48],0,NULL} ,
  {0, "str" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[49]} ,
  {0, "int" ,128,1,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[50]} ,
  {0, "real" ,128,2,0,0,0,0,0,0,NULL,&atx[51],NULL,0,&atx[52]} ,
  {309, "REAL" ,0,9,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "bool" ,128,3,0,0,0,0,0,0,NULL,&atx[53],NULL,0,&atx[54]} ,
  {301, "BOOLEAN" ,0,1,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "os" ,128,4,0,0,0,0,0,0,NULL,&atx[55],NULL,0,&atx[56]} ,
  {304, "OCTET STRING" ,0,4,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "object" ,128,5,0,0,0,0,0,0,NULL,&atx[39],NULL,0,&atx[57]} ,
  {0, "strs" ,128,6,0,0,0,0,0,0,NULL,&atx[59],&atx[58],0,&atx[60]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {312, "SEQUENCE OF" ,0,16,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "ints" ,128,7,0,0,0,0,0,0,NULL,&atx[59],&atx[61],0,&atx[62]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,NULL} ,
  {0, "reals" ,128,8,0,0,0,0,0,0,NULL,&atx[59],&atx[63],0,&atx[64]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[51],NULL,0,NULL} ,
  {0, "oss" ,128,9,0,0,0,0,0,0,NULL,&atx[59],&atx[65],0,&atx[66]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[55],NULL,0,NULL} ,
  {0, "fields" ,128,10,0,0,0,0,0,0,NULL,&atx[59],&atx[67],0,&atx[68]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[44],NULL,0,NULL} ,
  {0, "objects" ,128,11,0,0,0,0,0,0,NULL,&atx[59],&atx[69],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[39],NULL,0,NULL} ,
  {401, "Pub" ,1,0,0,0,0,1,0,0,NULL,&atx[11],&atx[71],0,&atx[233]} ,
  {0, "gen" ,128,0,0,0,0,0,0,0,NULL,&atx[72],NULL,0,&atx[123]} ,
  {411, "Cit-gen" ,1,0,0,0,0,0,1,0,NULL,&atx[73],NULL,0,&atx[225]} ,
  {407, "Cit-gen" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[74],0,&atx[163]} ,
  {0, "cit" ,128,0,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[75]} ,
  {0, "authors" ,128,1,0,1,0,0,0,0,NULL,&atx[76],NULL,0,&atx[101]} ,
  {413, "Auth-list" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[77],0,&atx[128]} ,
  {0, "names" ,128,0,0,0,0,0,0,0,NULL,&atx[11],&atx[78],0,&atx[100]} ,
  {0, "std" ,128,0,0,0,0,0,0,0,NULL,&atx[59],&atx[79],0,&atx[96]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[80],NULL,0,NULL} ,
  {416, "Author" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[81],0,&atx[86]} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[82],NULL,0,&atx[83]} ,
  {411, "Person-id" ,1,0,0,0,0,0,1,0,NULL,&atx[12],NULL,0,&atx[120]} ,
  {0, "level" ,128,1,0,1,0,0,0,0,NULL,&atx[38],&avnx[6],0,&atx[84]} ,
  {0, "role" ,128,2,0,1,0,0,0,0,NULL,&atx[38],&avnx[8],0,&atx[85]} ,
  {0, "affil" ,128,3,0,1,0,0,0,0,NULL,&atx[86],NULL,0,&atx[95]} ,
  {417, "Affil" ,1,0,0,0,0,0,0,0,NULL,&atx[11],&atx[87],0,NULL} ,
  {0, "str" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[88]} ,
  {0, "std" ,128,1,0,0,0,0,0,0,NULL,&atx[10],&atx[89],0,NULL} ,
  {0, "affil" ,128,0,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[90]} ,
  {0, "div" ,128,1,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[91]} ,
  {0, "city" ,128,2,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[92]} ,
  {0, "sub" ,128,3,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[93]} ,
  {0, "country" ,128,4,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[94]} ,
  {0, "street" ,128,5,0,1,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "is-corr" ,128,4,0,1,0,0,0,0,NULL,&atx[53],NULL,0,NULL} ,
  {0, "ml" ,128,1,0,0,0,0,0,0,NULL,&atx[59],&atx[97],0,&atx[98]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "str" ,128,2,0,0,0,0,0,0,NULL,&atx[59],&atx[99],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "affil" ,128,1,0,1,0,0,0,0,NULL,&atx[86],NULL,0,NULL} ,
  {0, "muid" ,128,2,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[102]} ,
  {0, "journal" ,128,3,0,1,0,0,0,0,NULL,&atx[103],NULL,0,&atx[116]} ,
  {410, "Title" ,1,0,0,0,0,1,0,0,NULL,&atx[115],&atx[104],0,&atx[82]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[11],&atx[105],0,NULL} ,
  {0, "name" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[106]} ,
  {0, "tsub" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[107]} ,
  {0, "trans" ,128,2,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[108]} ,
  {0, "jta" ,128,3,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[109]} ,
  {0, "iso-jta" ,128,4,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[110]} ,
  {0, "ml-jta" ,128,5,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[111]} ,
  {0, "coden" ,128,6,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[112]} ,
  {0, "issn" ,128,7,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[113]} ,
  {0, "abr" ,128,8,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[114]} ,
  {0, "isbn" ,128,9,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {314, "SET OF" ,0,17,0,0,0,0,0,0,NULL,NULL,NULL,0,NULL} ,
  {0, "volume" ,128,4,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[117]} ,
  {0, "issue" ,128,5,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[118]} ,
  {0, "pages" ,128,6,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[119]} ,
  {0, "date" ,128,7,0,1,0,0,0,0,NULL,&atx[120],NULL,0,&atx[121]} ,
  {412, "Date" ,1,0,0,0,0,0,1,0,NULL,&atx[0],NULL,0,&atx[76]} ,
  {0, "serial-number" ,128,8,0,1,0,0,0,0,NULL,&atx[6],NULL,0,&atx[122]} ,
  {0, "title" ,128,9,0,1,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "sub" ,128,1,0,0,0,0,0,0,NULL,&atx[124],NULL,0,&atx[140]} ,
  {413, "Cit-sub" ,1,0,0,0,0,0,1,0,NULL,&atx[125],NULL,0,NULL} ,
  {409, "Cit-sub" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[126],0,&atx[103]} ,
  {0, "authors" ,128,0,0,0,0,0,0,0,NULL,&atx[76],NULL,0,&atx[127]} ,
  {0, "imp" ,128,1,0,0,0,0,0,0,NULL,&atx[128],NULL,0,&atx[139]} ,
  {414, "Imprint" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[129],0,&atx[166]} ,
  {0, "date" ,128,0,0,0,0,0,0,0,NULL,&atx[120],NULL,0,&atx[130]} ,
  {0, "volume" ,128,1,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[131]} ,
  {0, "issue" ,128,2,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[132]} ,
  {0, "pages" ,128,3,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[133]} ,
  {0, "section" ,128,4,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[134]} ,
  {0, "pub" ,128,5,0,1,0,0,0,0,NULL,&atx[86],NULL,0,&atx[135]} ,
  {0, "cprt" ,128,6,0,1,0,0,0,0,NULL,&atx[120],NULL,0,&atx[136]} ,
  {0, "part-sup" ,128,7,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[137]} ,
  {0, "language" ,128,8,0,0,1,0,0,0,&avnx[12],&atx[2],NULL,0,&atx[138]} ,
  {0, "prepub" ,128,9,0,1,0,0,0,0,NULL,&atx[38],&avnx[13],0,NULL} ,
  {0, "medium" ,128,2,0,1,0,0,0,0,NULL,&atx[38],&avnx[16],0,NULL} ,
  {0, "medline" ,128,2,0,0,0,0,0,0,NULL,&atx[141],NULL,0,&atx[196]} ,
  {404, "Medline-entry" ,1,0,0,0,0,0,1,0,NULL,&atx[142],NULL,0,&atx[198]} ,
  {401, "Medline-entry" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[143],0,&atx[147]} ,
  {0, "uid" ,128,0,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[144]} ,
  {0, "em" ,128,1,0,0,0,0,0,0,NULL,&atx[145],NULL,0,&atx[146]} ,
  {403, "Date" ,1,0,0,0,0,0,1,0,NULL,&atx[0],NULL,0,&atx[173]} ,
  {0, "cit" ,128,2,0,0,0,0,0,0,NULL,&atx[147],NULL,0,&atx[170]} ,
  {402, "Cit-art" ,1,0,0,0,0,0,1,0,NULL,&atx[148],NULL,0,&atx[145]} ,
  {401, "Cit-art" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[149],0,&atx[153]} ,
  {0, "title" ,128,0,0,1,0,0,0,0,NULL,&atx[103],NULL,0,&atx[150]} ,
  {0, "authors" ,128,1,0,1,0,0,0,0,NULL,&atx[76],NULL,0,&atx[151]} ,
  {0, "from" ,128,2,0,0,0,0,0,0,NULL,&atx[11],&atx[152],0,NULL} ,
  {0, "journal" ,128,0,0,0,0,0,0,0,NULL,&atx[153],NULL,0,&atx[156]} ,
  {402, "Cit-jour" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[154],0,&atx[157]} ,
  {0, "title" ,128,0,0,0,0,0,0,0,NULL,&atx[103],NULL,0,&atx[155]} ,
  {0, "imp" ,128,1,0,0,0,0,0,0,NULL,&atx[128],NULL,0,NULL} ,
  {0, "book" ,128,1,0,0,0,0,0,0,NULL,&atx[157],NULL,0,&atx[162]} ,
  {403, "Cit-book" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[158],0,&atx[207]} ,
  {0, "title" ,128,0,0,0,0,0,0,0,NULL,&atx[103],NULL,0,&atx[159]} ,
  {0, "coll" ,128,1,0,1,0,0,0,0,NULL,&atx[103],NULL,0,&atx[160]} ,
  {0, "authors" ,128,2,0,0,0,0,0,0,NULL,&atx[76],NULL,0,&atx[161]} ,
  {0, "imp" ,128,3,0,0,0,0,0,0,NULL,&atx[128],NULL,0,NULL} ,
  {0, "proc" ,128,2,0,0,0,0,0,0,NULL,&atx[163],NULL,0,NULL} ,
  {408, "Cit-proc" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[164],0,&atx[125]} ,
  {0, "book" ,128,0,0,0,0,0,0,0,NULL,&atx[157],NULL,0,&atx[165]} ,
  {0, "meet" ,128,1,0,0,0,0,0,0,NULL,&atx[166],NULL,0,NULL} ,
  {415, "Meeting" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[167],0,&atx[80]} ,
  {0, "number" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[168]} ,
  {0, "date" ,128,1,0,0,0,0,0,0,NULL,&atx[120],NULL,0,&atx[169]} ,
  {0, "place" ,128,2,0,1,0,0,0,0,NULL,&atx[86],NULL,0,NULL} ,
  {0, "abstract" ,128,3,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[171]} ,
  {0, "mesh" ,128,4,0,1,0,0,0,0,NULL,&atx[115],&atx[172],0,&atx[181]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[173],NULL,0,NULL} ,
  {404, "Medline-mesh" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[174],0,&atx[183]} ,
  {0, "mp" ,128,0,0,0,1,0,0,0,&avnx[21],&atx[53],NULL,0,&atx[175]} ,
  {0, "term" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[176]} ,
  {0, "qual" ,128,2,0,1,0,0,0,0,NULL,&atx[115],&atx[177],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[178],NULL,0,NULL} ,
  {407, "Medline-qual" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[179],0,NULL} ,
  {0, "mp" ,128,0,0,0,1,0,0,0,&avnx[22],&atx[53],NULL,0,&atx[180]} ,
  {0, "subh" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "substance" ,128,5,0,1,0,0,0,0,NULL,&atx[115],&atx[182],0,&atx[187]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[183],NULL,0,NULL} ,
  {405, "Medline-rn" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[184],0,&atx[189]} ,
  {0, "type" ,128,0,0,0,0,0,0,0,NULL,&atx[38],&avnx[23],0,&atx[185]} ,
  {0, "cit" ,128,1,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[186]} ,
  {0, "name" ,128,2,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "xref" ,128,6,0,1,0,0,0,0,NULL,&atx[115],&atx[188],0,&atx[192]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[189],NULL,0,NULL} ,
  {406, "Medline-si" ,1,0,0,0,0,0,0,0,NULL,&atx[10],&atx[190],0,&atx[178]} ,
  {0, "type" ,128,0,0,0,0,0,0,0,NULL,&atx[38],&avnx[26],0,&atx[191]} ,
  {0, "cit" ,128,1,0,1,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "idnum" ,128,7,0,1,0,0,0,0,NULL,&atx[115],&atx[193],0,&atx[194]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "gene" ,128,8,0,1,0,0,0,0,NULL,&atx[115],&atx[195],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "muid" ,128,3,0,0,0,0,0,0,NULL,&atx[6],NULL,0,&atx[197]} ,
  {0, "article" ,128,4,0,0,0,0,0,0,NULL,&atx[198],NULL,0,&atx[199]} ,
  {405, "Cit-art" ,1,0,0,0,0,0,1,0,NULL,&atx[148],NULL,0,&atx[200]} ,
  {0, "journal" ,128,5,0,0,0,0,0,0,NULL,&atx[200],NULL,0,&atx[201]} ,
  {406, "Cit-jour" ,1,0,0,0,0,0,1,0,NULL,&atx[153],NULL,0,&atx[202]} ,
  {0, "book" ,128,6,0,0,0,0,0,0,NULL,&atx[202],NULL,0,&atx[203]} ,
  {407, "Cit-book" ,1,0,0,0,0,0,1,0,NULL,&atx[157],NULL,0,&atx[204]} ,
  {0, "proc" ,128,7,0,0,0,0,0,0,NULL,&atx[204],NULL,0,&atx[205]} ,
  {408, "Cit-proc" ,1,0,0,0,0,0,1,0,NULL,&atx[163],NULL,0,&atx[206]} ,
  {0, "patent" ,128,8,0,0,0,0,0,0,NULL,&atx[206],NULL,0,&atx[217]} ,
  {409, "Cit-pat" ,1,0,0,0,0,0,1,0,NULL,&atx[207],NULL,0,&atx[218]} ,
  {404, "Cit-pat" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[208],0,&atx[226]} ,
  {0, "title" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[209]} ,
  {0, "authors" ,128,1,0,0,0,0,0,0,NULL,&atx[76],NULL,0,&atx[210]} ,
  {0, "country" ,128,2,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[211]} ,
  {0, "doc-type" ,128,3,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[212]} ,
  {0, "number" ,128,4,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[213]} ,
  {0, "date-issue" ,128,5,0,0,0,0,0,0,NULL,&atx[120],NULL,0,&atx[214]} ,
  {0, "class" ,128,6,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[215]} ,
  {0, "app-number" ,128,7,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[216]} ,
  {0, "app-date" ,128,8,0,1,0,0,0,0,NULL,&atx[120],NULL,0,NULL} ,
  {0, "pat-id" ,128,9,0,0,0,0,0,0,NULL,&atx[218],NULL,0,&atx[224]} ,
  {410, "Id-pat" ,1,0,0,0,0,0,1,0,NULL,&atx[219],NULL,0,&atx[72]} ,
  {406, "Id-pat" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[220],0,&atx[73]} ,
  {0, "country" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[221]} ,
  {0, "id" ,128,1,0,0,0,0,0,0,NULL,&atx[11],&atx[222],0,NULL} ,
  {0, "number" ,128,0,0,0,0,0,0,0,NULL,&atx[2],NULL,0,&atx[223]} ,
  {0, "app-number" ,128,1,0,0,0,0,0,0,NULL,&atx[2],NULL,0,NULL} ,
  {0, "man" ,128,10,0,0,0,0,0,0,NULL,&atx[225],NULL,0,&atx[230]} ,
  {412, "Cit-let" ,1,0,0,0,0,0,1,0,NULL,&atx[226],NULL,0,&atx[124]} ,
  {405, "Cit-let" ,1,0,0,0,0,1,0,0,NULL,&atx[10],&atx[227],0,&atx[219]} ,
  {0, "cit" ,128,0,0,0,0,0,0,0,NULL,&atx[157],NULL,0,&atx[228]} ,
  {0, "man-id" ,128,1,0,1,0,0,0,0,NULL,&atx[2],NULL,0,&atx[229]} ,
  {0, "type" ,128,2,0,1,0,0,0,0,NULL,&atx[38],&avnx[39],0,NULL} ,
  {0, "equiv" ,128,11,0,0,0,0,0,0,NULL,&atx[231],NULL,0,NULL} ,
  {403, "Pub-equiv" ,1,0,0,0,0,1,0,0,NULL,&atx[115],&atx[232],0,&atx[141]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[70],NULL,0,NULL} ,
  {402, "Pub-set" ,1,0,0,0,0,1,0,0,NULL,&atx[11],&atx[234],0,&atx[231]} ,
  {0, "pub" ,128,0,0,0,0,0,0,0,NULL,&atx[115],&atx[235],0,&atx[236]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[70],NULL,0,NULL} ,
  {0, "medline" ,128,1,0,0,0,0,0,0,NULL,&atx[115],&atx[237],0,&atx[238]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[141],NULL,0,NULL} ,
  {0, "article" ,128,2,0,0,0,0,0,0,NULL,&atx[115],&atx[239],0,&atx[240]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[198],NULL,0,NULL} ,
  {0, "journal" ,128,3,0,0,0,0,0,0,NULL,&atx[115],&atx[241],0,&atx[242]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[200],NULL,0,NULL} ,
  {0, "book" ,128,4,0,0,0,0,0,0,NULL,&atx[115],&atx[243],0,&atx[244]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[202],NULL,0,NULL} ,
  {0, "proc" ,128,5,0,0,0,0,0,0,NULL,&atx[115],&atx[245],0,&atx[246]} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[204],NULL,0,NULL} ,
  {0, "patent" ,128,6,0,0,0,0,0,0,NULL,&atx[115],&atx[247],0,NULL} ,
  {0, NULL,1,0,0,0,0,0,0,0,NULL,&atx[206],NULL,0,NULL} };

static AsnModule ampx[4] = {
  { "NCBI-General" , "allpub.h00",&atx[0],NULL,&ampx[1],0,0} ,
  { "NCBI-Pub" , NULL,&atx[70],NULL,&ampx[2],0,0} ,
  { "NCBI-Biblio" , NULL,&atx[148],NULL,&ampx[3],0,0} ,
  { "NCBI-Medline" , NULL,&atx[142],NULL,NULL,0,0} };

static AsnValxNodePtr avn = avnx;
static AsnTypePtr at = atx;
static AsnModulePtr amp = ampx;



/**************************************************
*
*    Defines for Module NCBI-General
*
**************************************************/

#define DATE &at[0]
#define DATE_str &at[1]
#define DATE_std &at[3]

#define PERSON_ID &at[12]
#define PERSON_ID_dbtag &at[13]
#define PERSON_ID_name &at[20]
#define PERSON_ID_ml &at[29]
#define PERSON_ID_str &at[30]

#define OBJECT_ID &at[17]
#define OBJECT_ID_id &at[18]
#define OBJECT_ID_str &at[19]

#define DBTAG &at[14]
#define DBTAG_db &at[15]
#define DBTAG_tag &at[16]

#define INT_FUZZ &at[31]
#define INT_FUZZ_p_m &at[32]
#define INT_FUZZ_range &at[33]
#define INT_FUZZ_range_max &at[34]
#define INT_FUZZ_range_min &at[35]
#define INT_FUZZ_pct &at[36]
#define INT_FUZZ_lim &at[37]

#define USER_OBJECT &at[39]
#define USER_OBJECT_class &at[40]
#define USER_OBJECT_type &at[41]
#define USER_OBJECT_data &at[42]
#define USER_OBJECT_data_E &at[43]

#define DATE_STD &at[4]
#define DATE_STD_year &at[5]
#define DATE_STD_month &at[7]
#define DATE_STD_day &at[8]
#define DATE_STD_season &at[9]

#define NAME_STD &at[21]
#define NAME_STD_last &at[22]
#define NAME_STD_first &at[23]
#define NAME_STD_middle &at[24]
#define NAME_STD_full &at[25]
#define NAME_STD_initials &at[26]
#define NAME_STD_suffix &at[27]
#define NAME_STD_title &at[28]

#define USER_FIELD &at[44]
#define USER_FIELD_label &at[45]
#define USER_FIELD_num &at[46]
#define USER_FIELD_data &at[47]
#define USER_FIELD_data_str &at[48]
#define USER_FIELD_data_int &at[49]
#define USER_FIELD_data_real &at[50]
#define USER_FIELD_data_bool &at[52]
#define USER_FIELD_data_os &at[54]
#define USER_FIELD_data_object &at[56]
#define USER_FIELD_data_strs &at[57]
#define USER_FIELD_data_strs_E &at[58]
#define USER_FIELD_data_ints &at[60]
#define USER_FIELD_data_ints_E &at[61]
#define USER_FIELD_data_reals &at[62]
#define USER_FIELD_data_reals_E &at[63]
#define USER_FIELD_data_oss &at[64]
#define USER_FIELD_data_oss_E &at[65]
#define USER_FIELD_data_fields &at[66]
#define USER_FIELD_data_fields_E &at[67]
#define USER_FIELD_data_objects &at[68]
#define USER_FIELD_data_objects_E &at[69]


/**************************************************
*
*    Defines for Module NCBI-Pub
*
**************************************************/

#define PUB &at[70]
#define PUB_gen &at[71]
#define PUB_sub &at[123]
#define PUB_medline &at[140]
#define PUB_muid &at[196]
#define PUB_article &at[197]
#define PUB_journal &at[199]
#define PUB_book &at[201]
#define PUB_proc &at[203]
#define PUB_patent &at[205]
#define PUB_pat_id &at[217]
#define PUB_man &at[224]
#define PUB_equiv &at[230]

#define PUB_SET &at[233]
#define PUB_SET_pub &at[234]
#define PUB_SET_pub_E &at[235]
#define PUB_SET_medline &at[236]
#define PUB_SET_medline_E &at[237]
#define PUB_SET_article &at[238]
#define PUB_SET_article_E &at[239]
#define PUB_SET_journal &at[240]
#define PUB_SET_journal_E &at[241]
#define PUB_SET_book &at[242]
#define PUB_SET_book_E &at[243]
#define PUB_SET_proc &at[244]
#define PUB_SET_proc_E &at[245]
#define PUB_SET_patent &at[246]
#define PUB_SET_patent_E &at[247]

#define PUB_EQUIV &at[231]
#define PUB_EQUIV_E &at[232]


/**************************************************
*
*    Defines for Module NCBI-Biblio
*
**************************************************/

#define CIT_ART &at[148]
#define CIT_ART_title &at[149]
#define CIT_ART_authors &at[150]
#define CIT_ART_from &at[151]
#define CIT_ART_from_journal &at[152]
#define CIT_ART_from_book &at[156]
#define CIT_ART_from_proc &at[162]

#define CIT_JOUR &at[153]
#define CIT_JOUR_title &at[154]
#define CIT_JOUR_imp &at[155]

#define CIT_BOOK &at[157]
#define CIT_BOOK_title &at[158]
#define CIT_BOOK_coll &at[159]
#define CIT_BOOK_authors &at[160]
#define CIT_BOOK_imp &at[161]

#define CIT_PAT &at[207]
#define CIT_PAT_title &at[208]
#define CIT_PAT_authors &at[209]
#define CIT_PAT_country &at[210]
#define CIT_PAT_doc_type &at[211]
#define CIT_PAT_number &at[212]
#define CIT_PAT_date_issue &at[213]
#define CIT_PAT_class &at[214]
#define CIT_PAT_app_number &at[215]
#define CIT_PAT_app_date &at[216]

#define CIT_LET &at[226]
#define CIT_LET_cit &at[227]
#define CIT_LET_man_id &at[228]
#define CIT_LET_type &at[229]

#define ID_PAT &at[219]
#define ID_PAT_country &at[220]
#define ID_PAT_id &at[221]
#define ID_PAT_id_number &at[222]
#define ID_PAT_id_app_number &at[223]

#define CIT_GEN &at[73]
#define CIT_GEN_cit &at[74]
#define CIT_GEN_authors &at[75]
#define CIT_GEN_muid &at[101]
#define CIT_GEN_journal &at[102]
#define CIT_GEN_volume &at[116]
#define CIT_GEN_issue &at[117]
#define CIT_GEN_pages &at[118]
#define CIT_GEN_date &at[119]
#define CIT_GEN_serial_number &at[121]
#define CIT_GEN_title &at[122]

#define CIT_PROC &at[163]
#define CIT_PROC_book &at[164]
#define CIT_PROC_meet &at[165]

#define CIT_SUB &at[125]
#define CIT_SUB_authors &at[126]
#define CIT_SUB_imp &at[127]
#define CIT_SUB_medium &at[139]

#define TITLE &at[103]
#define TITLE_E &at[104]
#define TITLE_E_name &at[105]
#define TITLE_E_tsub &at[106]
#define TITLE_E_trans &at[107]
#define TITLE_E_jta &at[108]
#define TITLE_E_iso_jta &at[109]
#define TITLE_E_ml_jta &at[110]
#define TITLE_E_coden &at[111]
#define TITLE_E_issn &at[112]
#define TITLE_E_abr &at[113]
#define TITLE_E_isbn &at[114]

#define AUTH_LIST &at[76]
#define AUTH_LIST_names &at[77]
#define AUTH_LIST_names_std &at[78]
#define AUTH_LIST_names_std_E &at[79]
#define AUTH_LIST_names_ml &at[96]
#define AUTH_LIST_names_ml_E &at[97]
#define AUTH_LIST_names_str &at[98]
#define AUTH_LIST_names_str_E &at[99]
#define AUTH_LIST_affil &at[100]

#define IMPRINT &at[128]
#define IMPRINT_date &at[129]
#define IMPRINT_volume &at[130]
#define IMPRINT_issue &at[131]
#define IMPRINT_pages &at[132]
#define IMPRINT_section &at[133]
#define IMPRINT_pub &at[134]
#define IMPRINT_cprt &at[135]
#define IMPRINT_part_sup &at[136]
#define IMPRINT_language &at[137]
#define IMPRINT_prepub &at[138]

#define MEETING &at[166]
#define MEETING_number &at[167]
#define MEETING_date &at[168]
#define MEETING_place &at[169]

#define AUTHOR &at[80]
#define AUTHOR_name &at[81]
#define AUTHOR_level &at[83]
#define AUTHOR_role &at[84]
#define AUTHOR_affil &at[85]
#define AUTHOR_is_corr &at[95]

#define AFFIL &at[86]
#define AFFIL_str &at[87]
#define AFFIL_std &at[88]
#define AFFIL_std_affil &at[89]
#define AFFIL_std_div &at[90]
#define AFFIL_std_city &at[91]
#define AFFIL_std_sub &at[92]
#define AFFIL_std_country &at[93]
#define AFFIL_std_street &at[94]


/**************************************************
*
*    Defines for Module NCBI-Medline
*
**************************************************/

#define MEDLINE_ENTRY &at[142]
#define MEDLINE_ENTRY_uid &at[143]
#define MEDLINE_ENTRY_em &at[144]
#define MEDLINE_ENTRY_cit &at[146]
#define MEDLINE_ENTRY_abstract &at[170]
#define MEDLINE_ENTRY_mesh &at[171]
#define MEDLINE_ENTRY_mesh_E &at[172]
#define MEDLINE_ENTRY_substance &at[181]
#define MEDLINE_ENTRY_substance_E &at[182]
#define MEDLINE_ENTRY_xref &at[187]
#define MEDLINE_ENTRY_xref_E &at[188]
#define MEDLINE_ENTRY_idnum &at[192]
#define MEDLINE_ENTRY_idnum_E &at[193]
#define MEDLINE_ENTRY_gene &at[194]
#define MEDLINE_ENTRY_gene_E &at[195]

#define MEDLINE_MESH &at[173]
#define MEDLINE_MESH_mp &at[174]
#define MEDLINE_MESH_term &at[175]
#define MEDLINE_MESH_qual &at[176]
#define MEDLINE_MESH_qual_E &at[177]

#define MEDLINE_RN &at[183]
#define MEDLINE_RN_type &at[184]
#define MEDLINE_RN_cit &at[185]
#define MEDLINE_RN_name &at[186]

#define MEDLINE_SI &at[189]
#define MEDLINE_SI_type &at[190]
#define MEDLINE_SI_cit &at[191]

#define MEDLINE_QUAL &at[178]
#define MEDLINE_QUAL_mp &at[179]
#define MEDLINE_QUAL_subh &at[180]
