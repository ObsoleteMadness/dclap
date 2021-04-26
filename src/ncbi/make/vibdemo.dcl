$ !
$ ! Simple makefile for NCBI demo programs with Vibrant
$ ! Also makes Entrez
$ !
$ copy [-.demo]*.* []
$ copy [-.browser]*.* []
$ !
$ ! Make testcore
$ !
$ cc/nolist/include=([],[-.include]) testcore
$ link/exe=testcore sys$input/option
testcore
[-.lib]libvibrant/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ !
$ ! Make getmesh
$ !
$ cc/nolist/include=([],[-.include]) getmesh
$ link/exe=getmesh sys$input/option
getmesh
[-.lib]libvibrant/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ !
$ ! Make indexpub
$ !
$ cc/nolist/include=([],[-.include]) indexpub
$ link/exe=indexpub sys$input/option
indexpub
[-.lib]libvibrant/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ !
$ ! Make getpub
$ !
$ cc/nolist/include=([],[-.include]) getpub
$ link/exe=getpub sys$input/option
getpub
[-.lib]libvibrant/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ !
$ ! Make asn2rpt
$ !
$ cc/nolist/include=([],[-.include]) asn2rpt
$ link/exe=asn2rpt sys$input/option
asn2rpt
[-.lib]libvibrant/lib
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ !
$ ! Make dosimple
$ !
$ cc/nolist/include=([],[-.include]) dosimple
$ link/exe=dosimple sys$input/option
dosimple
[-.lib]libvibrant/lib
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ !
$ ! Make testobj
$ !
$ cc/nolist/include=([],[-.include]) testobj
$ link/exe=testobj sys$input/option
testobj
[-.lib]libvibrant/lib
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ !
$ ! Make seqtest
$ !
$ cc/nolist/include=([],[-.include]) seqtest
$ link/exe=seqtest sys$input/option
seqtest
[-.lib]libvibrant/lib
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ !
$ ! Make asn2gnbk
$ !
$ cc/nolist/include=([],[-.include]) asn2gnbk
$ link/exe=asn2gnbk sys$input/option
asn2gnbk
[-.lib]libvibrant/lib
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ !
$ ! Make getfeat
$ !
$ cc/nolist/include=([],[-.include]) getfeat
$ link/exe=getfeat sys$input/option
getfeat
[-.lib]libvibrant/lib
[-.lib]libncbiacc/lib
[-.lib]libncbicdr/lib
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ !
$ ! Make getseq
$ !
$ cc/nolist/include=([],[-.include]) getseq
$ link/exe=getseq sys$input/option
getseq
[-.lib]libvibrant/lib
[-.lib]libncbiacc/lib
[-.lib]libncbicdr/lib
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ !
$ ! Make entrez
$ !
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF avail
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF browser
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF chosen
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF fetch
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF report
$ link/exe=entrez sys$input/option
avail
browser
chosen
fetch
report
[-.lib]libvibrant/lib
[-.lib]libncbiacc/lib
[-.lib]libncbicdr/lib
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ !
$ ! Make EntrezCf, Configuration program for Entrez
$ !
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF entrezCf
$ link/exe=entrezCf sys$input/option
entrezcf
[-.lib]libvibrant/lib
[-.lib]libncbiacc/lib
[-.lib]libncbicdr/lib
[-.lib]libncbiobj/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ exit

