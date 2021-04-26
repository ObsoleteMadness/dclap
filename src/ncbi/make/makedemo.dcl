$ !
$ ! Simple makefile for NCBI demo programs without Vibrant
$ !
$ copy [-.demo]*.* []
$ !
$ ! Make testcore
$ !
$ cc/nolist/include=([],[-.include]) testcore
$ link/exe=testcore sys$input/option
testcore
[-.lib]libncbi/lib
$ !
$ ! Make getmesh
$ !
$ cc/nolist/include=([],[-.include]) getmesh
$ link/exe=getmesh sys$input/option
getmesh
[-.lib]libncbi/lib
$ !
$ ! Make indexpub
$ !
$ cc/nolist/include=([],[-.include]) indexpub
$ link/exe=indexpub sys$input/option
indexpub
[-.lib]libncbi/lib
$ !
$ ! Make getpub
$ !
$ cc/nolist/include=([],[-.include]) getpub
$ link/exe=getpub sys$input/option
getpub
[-.lib]libncbi/lib
$ !
$ ! Make asn2rpt
$ !
$ cc/nolist/include=([],[-.include]) asn2rpt
$ link/exe=asn2rpt sys$input/option
asn2rpt
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
$ !
$ ! Make dosimple
$ !
$ cc/nolist/include=([],[-.include]) dosimple
$ link/exe=dosimple sys$input/option
dosimple
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
$ !
$ ! Make testobj
$ !
$ cc/nolist/include=([],[-.include]) testobj
$ link/exe=testobj sys$input/option
testobj
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
$ !
$ ! Make seqtest
$ !
$ cc/nolist/include=([],[-.include]) seqtest
$ link/exe=seqtest sys$input/option
seqtest
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
$ !
$ ! Make asn2gnbk
$ !
$ cc/nolist/include=([],[-.include]) asn2gnbk
$ link/exe=asn2gnbk sys$input/option
asn2gnbk
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
$ !
$ ! Make getfeat
$ !
$ cc/nolist/include=([],[-.include]) getfeat
$ link/exe=getfeat sys$input/option
getfeat
[-.lib]libncbiacc/lib
[-.lib]libncbicdr/lib
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
$ !
$ ! Make getseq
$ !
$ cc/nolist/include=([],[-.include]) getseq
$ link/exe=getseq sys$input/option
getseq
[-.lib]libncbiacc/lib
[-.lib]libncbicdr/lib
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
$ !
$ ! Make asndhuff
$ !
$ cc/nolist/include=([],[-.include]) asndhuff
$ link/exe=asndhuff sys$input/option
asndhuff
[-.lib]libncbiacc/lib
[-.lib]libncbicdr/lib
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
$ exit

