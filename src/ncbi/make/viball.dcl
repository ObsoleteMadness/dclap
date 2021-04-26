$ !
$ ! Simple makefile for NCBI tool libraries with Vibrant
$ ! Also makes AsnTool
$ !
$ copy [-.corelib]*.h [-.include]
$ copy [-.corelib]ncbilcl.vms [-.include]ncbilcl.h
$ copy [-.corelib]*.c []
$ copy [-.corelib]ncbimain.vms ncbimain.c
$ copy [-.asnlib]*.h [-.include]
$ copy [-.asnlib]*.c []
$ copy [-.object]*.h [-.include]
$ copy [-.object]*.c []
$ copy [-.asnstat]*.h [-.include]
$ copy [-.api]*.h [-.include]
$ copy [-.api]*.c []
$ copy [-.cdromlib]*.h [-.include]
$ copy [-.cdromlib]*.c []
$ copy [-.vibrant]*.h [-.include]
$ copy [-.vibrant]*.c []
$ !
$ ! Make libncbi.olb
$ !
$ cc/nolist/include=([],[-.include]) ncbibs
$ cc/nolist/include=([],[-.include]) ncbienv
$ cc/nolist/include=([],[-.include]) ncbierr
$ cc/nolist/include=([],[-.include]) ncbifile
$ cc/nolist/include=([],[-.include]) ncbimain
$ cc/nolist/include=([],[-.include]) ncbimath
$ cc/nolist/include=([],[-.include]) ncbimem
$ cc/nolist/include=([],[-.include]) ncbimisc
$ cc/nolist/include=([],[-.include]) ncbimsg
$ cc/nolist/include=([],[-.include]) ncbistr
$ cc/nolist/include=([],[-.include]) ncbitime
$ cc/nolist/include=([],[-.include]) asndebin
$ cc/nolist/include=([],[-.include]) asnenbin
$ cc/nolist/include=([],[-.include]) asngen
$ cc/nolist/include=([],[-.include]) asnio
$ cc/nolist/include=([],[-.include]) asnlex
$ cc/nolist/include=([],[-.include]) asnlext
$ cc/nolist/include=([],[-.include]) asnout
$ cc/nolist/include=([],[-.include]) asnprint
$ cc/nolist/include=([],[-.include]) asntypes
$ libr/obj/create libncbi.olb -
    ncbibs.obj, ncbierr.obj, ncbienv.obj, ncbifile.obj, ncbimain.obj,-
    ncbimath.obj, ncbimem.obj, ncbimisc.obj, ncbimsg.obj, ncbistr.obj,-
    ncbitime.obj, asndebin.obj, asnenbin.obj, asngen.obj, asnio.obj,-
    asnlex.obj, asnlext.obj, asnout.obj, asnprint.obj, asntypes.obj
$ rename libncbi.olb [-.lib]
$ !
$ ! Make libncbiobj.olb
$ !
$ cc/nolist/include=([],[-.include]) objacces
$ cc/nolist/include=([],[-.include]) objalign
$ cc/nolist/include=([],[-.include]) objall
$ cc/nolist/include=([],[-.include]) objbibli
$ cc/nolist/include=([],[-.include]) objblock   ! WAG
$ cc/nolist/include=([],[-.include]) objcode    ! WAG
$ cc/nolist/include=([],[-.include]) objfeat
$ cc/nolist/include=([],[-.include]) objgen
$ cc/nolist/include=([],[-.include]) objloc
$ cc/nolist/include=([],[-.include]) objmedli
$ cc/nolist/include=([],[-.include]) objpub
$ cc/nolist/include=([],[-.include]) objres
$ cc/nolist/include=([],[-.include]) objseq
$ cc/nolist/include=([],[-.include]) objsset
$ cc/nolist/include=([],[-.include]) seqport
$ cc/nolist/include=([],[-.include]) sequtil
$ cc/nolist/include=([],[-.include]) simple
$ cc/nolist/include=([],[-.include]) tofasta
$ cc/nolist/include=([],[-.include]) tofile
$ cc/nolist/include=([],[-.include]) tognbk1
$ cc/nolist/include=([],[-.include]) tognbk2
$ cc/nolist/include=([],[-.include]) tognbk3
$ cc/nolist/include=([],[-.include]) tomedlin
$ cc/nolist/include=([],[-.include]) toreport
$ libr/obj/create libncbiobj.olb -
    objacces.obj, objalign.obj, objall.obj, objblock, objcode,-   ! WAG
    objbibli.obj, objfeat.obj, objgen.obj, objloc.obj, objmedli.obj,-
    objpub.obj, objres.obj, objseq.obj, objsset.obj, seqport.obj,-
    sequtil.obj, simple.obj, tofasta.obj, tofile.obj, tognbk1.obj,-
    tognbk2.obj, tognbk3.obj, tomedlin.obj, toreport.obj
$ rename libncbiobj.olb [-.lib]
$ !
$ ! Make libncbicdr.olb
$ !
$ cc/nolist/include=([],[-.include]) cdentrez
$ cc/nolist/include=([],[-.include]) cdml
$ cc/nolist/include=([],[-.include]) cdromlib
$ cc/nolist/include=([],[-.include]) cdseq
$ cc/nolist/include=([],[-.include]) cdconfig
$ cc/nolist/include=([],[-.include]) casn
$ libr/obj/create libncbicdr.olb -
    cdentrez.obj, cdml.obj, cdromlib.obj, cdseq.obj, cdconfig.obj, casn.obj
$ rename libncbicdr.olb [-.lib]
$ !
$ ! Make libncbiacc.olb
$ !
$ cc/nolist/include=([],[-.include]) accentr
$ cc/nolist/include=([],[-.include]) accml
$ cc/nolist/include=([],[-.include]) accseq
$ libr/obj/create libncbiacc.olb -
    accentr.obj, accml.obj, accseq.obj
$ rename libncbiacc.olb [-.lib]
$ !
$ ! Make libvibrant.olb
$ !
$ define/nolog X11 decw$include     ! WAG   <X11/Xlib.h>
$ define/nolog Xm  decw$include     ! WAG   <Xm/*.h>
$ define/nolog sys sys$library      ! WAG   <sys/*.h>
$ !
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF vibbutns
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF vibextra
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF vibfiles
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF vibgroup
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF viblists
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF vibmenus
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF vibprmpt
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF vibsbars
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF vibslate
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF vibtexts
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF vibutils
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF vibwndws
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF ncbidraw
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF drawing
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF mapping
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF picture
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF viewer
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF document
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF palette
$ cc/nolist/include=([],[-.include])/DEFINE=WIN_MOTIF table
$ libr/obj/create libvibrant.olb -
    vibbutns.obj, vibextra.obj, vibfiles.obj, vibgroup.obj,-
    viblists.obj, vibmenus.obj, vibprmpt.obj, vibsbars.obj,-
    vibslate.obj, vibtexts.obj, vibutils.obj, vibwndws.obj,-
    ncbidraw.obj, drawing.obj, mapping.obj, picture.obj,-
    viewer.obj, document.obj, table.obj, palette.obj
$ rename libvibrant.olb [-.lib]
$ !
$ ! Make asntool
$ !
$ cc/nolist/include=([],[-.include]) asntool
$ link/exe=asntool sys$input/option
asntool
[-.lib]libvibrant/lib
[-.lib]libncbi/lib
sys$share:decw$xlibshr/share
sys$share:decw$dxmlibshr/shareable
sys$share:decw$xmlibshr/shareable
sys$share:decw$xtshr/shareable
$ exit
