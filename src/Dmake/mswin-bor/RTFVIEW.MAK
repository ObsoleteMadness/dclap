#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC     = Bcc +BccW16.cfg 
TLINK   = TLink
TLIB    = TLib
BRC     = Brc
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_LFLAGS =  -LC:\BC4\LIB
IDE_RFLAGS =  -IC:\BC4\INCLUDE
LLATW16_CcbBIObBINbrtfviewdexe =  -Twe -c -C -LC:\BIO\BIN;C:\BC4\LIB
RLATW16_CcbBIObBINbrtfviewdexe =  -31 -IC:\BIO\DCLAP;C:\BIO\DAPP;C:\BIO\DRTF;C:\BIO\DNET;C:\BIO\NETWORK\NSCLILIB;C:\BIO\CORELIB;C:\BIO\VIBRANT;C:\BC4\INCLUDE
BLATW16_CcbBIObBINbrtfviewdexe = 
LEAT_CcbBIObBINbrtfviewdexe = $(LLATW16_CcbBIObBINbrtfviewdexe)
REAT_CcbBIObBINbrtfviewdexe = $(RLATW16_CcbBIObBINbrtfviewdexe)
BEAT_CcbBIObBINbrtfviewdexe = $(BLATW16_CcbBIObBINbrtfviewdexe)
CLATW16_CcbBIObBINbdrtfdlib =  -ml -WS
LLATW16_CcbBIObBINbdrtfdlib =  -Twe
RLATW16_CcbBIObBINbdrtfdlib =  -31
BLATW16_CcbBIObBINbdrtfdlib = 
CEAT_CcbBIObBINbdrtfdlib = $(CEAT_CcbBIObBINbrtfviewdexe) $(CLATW16_CcbBIObBINbdrtfdlib)
LEAT_CcbBIObBINbdrtfdlib = $(LEAT_CcbBIObBINbrtfviewdexe) $(LLATW16_CcbBIObBINbdrtfdlib)
REAT_CcbBIObBINbdrtfdlib = $(REAT_CcbBIObBINbrtfviewdexe) $(RLATW16_CcbBIObBINbdrtfdlib)
BEAT_CcbBIObBINbdrtfdlib = $(BEAT_CcbBIObBINbrtfviewdexe) $(BLATW16_CcbBIObBINbdrtfdlib)
CLATW16_CcbBIObBINbvibrantdlib =  -ml -WS
LLATW16_CcbBIObBINbvibrantdlib =  -Twe
RLATW16_CcbBIObBINbvibrantdlib =  -31
BLATW16_CcbBIObBINbvibrantdlib = 
CEAT_CcbBIObBINbvibrantdlib = $(CEAT_CcbBIObBINbrtfviewdexe) $(CLATW16_CcbBIObBINbvibrantdlib)
LEAT_CcbBIObBINbvibrantdlib = $(LEAT_CcbBIObBINbrtfviewdexe) $(LLATW16_CcbBIObBINbvibrantdlib)
REAT_CcbBIObBINbvibrantdlib = $(REAT_CcbBIObBINbrtfviewdexe) $(RLATW16_CcbBIObBINbvibrantdlib)
BEAT_CcbBIObBINbvibrantdlib = $(BEAT_CcbBIObBINbrtfviewdexe) $(BLATW16_CcbBIObBINbvibrantdlib)
CLATW16_CcbBIObBINbcorelibdlib =  -ml -WS
LLATW16_CcbBIObBINbcorelibdlib =  -Twe
RLATW16_CcbBIObBINbcorelibdlib =  -31
BLATW16_CcbBIObBINbcorelibdlib = 
CEAT_CcbBIObBINbcorelibdlib = $(CEAT_CcbBIObBINbrtfviewdexe) $(CLATW16_CcbBIObBINbcorelibdlib)
LEAT_CcbBIObBINbcorelibdlib = $(LEAT_CcbBIObBINbrtfviewdexe) $(LLATW16_CcbBIObBINbcorelibdlib)
REAT_CcbBIObBINbcorelibdlib = $(REAT_CcbBIObBINbrtfviewdexe) $(RLATW16_CcbBIObBINbcorelibdlib)
BEAT_CcbBIObBINbcorelibdlib = $(BEAT_CcbBIObBINbrtfviewdexe) $(BLATW16_CcbBIObBINbcorelibdlib)
CLATW16_CcbBIObBINbdclapdlib =  -ml -WS
LLATW16_CcbBIObBINbdclapdlib =  -Twe
RLATW16_CcbBIObBINbdclapdlib =  -31
BLATW16_CcbBIObBINbdclapdlib = 
CEAT_CcbBIObBINbdclapdlib = $(CEAT_CcbBIObBINbrtfviewdexe) $(CLATW16_CcbBIObBINbdclapdlib)
LEAT_CcbBIObBINbdclapdlib = $(LEAT_CcbBIObBINbrtfviewdexe) $(LLATW16_CcbBIObBINbdclapdlib)
REAT_CcbBIObBINbdclapdlib = $(REAT_CcbBIObBINbrtfviewdexe) $(RLATW16_CcbBIObBINbdclapdlib)
BEAT_CcbBIObBINbdclapdlib = $(BEAT_CcbBIObBINbrtfviewdexe) $(BLATW16_CcbBIObBINbdclapdlib)

#
# Dependency List
#
Dep_rtfview = \
   C:\BIO\BIN\rtfview.exe

rtfview : BccW16.cfg $(Dep_rtfview)
  echo MakeNode rtfview

Dep_CcbBIObBINbrtfviewdexe = \
   C:\BIO\BIN\drtf.lib\
   C:\BIO\BIN\vibrant.lib\
   C:\BIO\BIN\corelib.lib\
   C:\BIO\BIN\dclap.lib\
   C:\BIO\BUILD\rtfviewe.obj

C:\BIO\BIN\rtfview.exe : $(Dep_CcbBIObBINbrtfviewdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGS) $(LEAT_CcbBIObBINbrtfviewdexe) +
C:\BC4\LIB\c0wl.obj+
C:\BIO\BUILD\rtfviewe.obj
$<,$*
C:\BIO\BIN\drtf.lib+
C:\BIO\BIN\vibrant.lib+
C:\BIO\BIN\corelib.lib+
C:\BIO\BIN\dclap.lib+
C:\BC4\LIB\import.lib+
C:\BC4\LIB\mathwl.lib+
C:\BC4\LIB\cwl.lib
..\dapp\rtfviewe.def
|

Dep_CcbBIObBINbdrtfdlib = \
   C:\BIO\BUILD\drtfdoc.obj\
   C:\BIO\BUILD\drtfview.obj\
   C:\BIO\BUILD\drtfwrit.obj\
   C:\BIO\BUILD\reader.obj\
   C:\BIO\BUILD\tokensca.obj

C:\BIO\BIN\drtf.lib : $(Dep_CcbBIObBINbdrtfdlib)
  $(TLIB) $< $(IDE_BFLAGS) $(BEAT_CcbBIObBINbdrtfdlib) @&&|
 -+C:\BIO\BUILD\drtfdoc.obj&
-+C:\BIO\BUILD\drtfview.obj&
-+C:\BIO\BUILD\drtfwrit.obj&
-+C:\BIO\BUILD\reader.obj&
-+C:\BIO\BUILD\tokensca.obj
|

C:\BIO\BUILD\drtfdoc.obj :  ..\drtf\drtfdoc.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbdrtfdlib) -o$@ ..\drtf\drtfdoc.c

C:\BIO\BUILD\drtfview.obj :  ..\drtf\drtfview.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdrtfdlib) -o$@ ..\drtf\drtfview.cpp

C:\BIO\BUILD\drtfwrit.obj :  ..\drtf\drtfwrit.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbdrtfdlib) -o$@ ..\drtf\drtfwrit.c

C:\BIO\BUILD\reader.obj :  ..\drtf\reader.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbdrtfdlib) -o$@ ..\drtf\reader.c

C:\BIO\BUILD\tokensca.obj :  ..\drtf\tokensca.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbdrtfdlib) -o$@ ..\drtf\tokensca.c

Dep_CcbBIObBINbvibrantdlib = \
   C:\BIO\BUILD\document.obj\
   C:\BIO\BUILD\drawing.obj\
   C:\BIO\BUILD\mapping.obj\
   C:\BIO\BUILD\ncbidraw.obj\
   C:\BIO\BUILD\palette.obj\
   C:\BIO\BUILD\picture.obj\
   C:\BIO\BUILD\table.obj\
   C:\BIO\BUILD\vibbutns.obj\
   C:\BIO\BUILD\vibextra.obj\
   C:\BIO\BUILD\vibfiles.obj\
   C:\BIO\BUILD\vibgroup.obj\
   C:\BIO\BUILD\viblists.obj\
   C:\BIO\BUILD\vibmenus.obj\
   C:\BIO\BUILD\vibprmpt.obj\
   C:\BIO\BUILD\vibsbars.obj\
   C:\BIO\BUILD\vibslate.obj\
   C:\BIO\BUILD\vibtexts.obj\
   C:\BIO\BUILD\vibutils.obj\
   C:\BIO\BUILD\vibwndws.obj\
   C:\BIO\BUILD\viewer.obj

C:\BIO\BIN\vibrant.lib : $(Dep_CcbBIObBINbvibrantdlib)
  $(TLIB) $< $(IDE_BFLAGS) $(BEAT_CcbBIObBINbvibrantdlib) @&&|
 -+C:\BIO\BUILD\document.obj&
-+C:\BIO\BUILD\drawing.obj&
-+C:\BIO\BUILD\mapping.obj&
-+C:\BIO\BUILD\ncbidraw.obj&
-+C:\BIO\BUILD\palette.obj&
-+C:\BIO\BUILD\picture.obj&
-+C:\BIO\BUILD\table.obj&
-+C:\BIO\BUILD\vibbutns.obj&
-+C:\BIO\BUILD\vibextra.obj&
-+C:\BIO\BUILD\vibfiles.obj&
-+C:\BIO\BUILD\vibgroup.obj&
-+C:\BIO\BUILD\viblists.obj&
-+C:\BIO\BUILD\vibmenus.obj&
-+C:\BIO\BUILD\vibprmpt.obj&
-+C:\BIO\BUILD\vibsbars.obj&
-+C:\BIO\BUILD\vibslate.obj&
-+C:\BIO\BUILD\vibtexts.obj&
-+C:\BIO\BUILD\vibutils.obj&
-+C:\BIO\BUILD\vibwndws.obj&
-+C:\BIO\BUILD\viewer.obj
|

C:\BIO\BUILD\document.obj :  ..\vibrant\document.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\document.c

C:\BIO\BUILD\drawing.obj :  ..\vibrant\drawing.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\drawing.c

C:\BIO\BUILD\mapping.obj :  ..\vibrant\mapping.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\mapping.c

C:\BIO\BUILD\ncbidraw.obj :  ..\vibrant\ncbidraw.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\ncbidraw.c

C:\BIO\BUILD\palette.obj :  ..\vibrant\palette.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\palette.c

C:\BIO\BUILD\picture.obj :  ..\vibrant\picture.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\picture.c

C:\BIO\BUILD\table.obj :  ..\vibrant\table.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\table.c

C:\BIO\BUILD\vibbutns.obj :  ..\vibrant\vibbutns.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\vibbutns.c

C:\BIO\BUILD\vibextra.obj :  ..\vibrant\vibextra.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\vibextra.c

C:\BIO\BUILD\vibfiles.obj :  ..\vibrant\vibfiles.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\vibfiles.c

C:\BIO\BUILD\vibgroup.obj :  ..\vibrant\vibgroup.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\vibgroup.c

C:\BIO\BUILD\viblists.obj :  ..\vibrant\viblists.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\viblists.c

C:\BIO\BUILD\vibmenus.obj :  ..\vibrant\vibmenus.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\vibmenus.c

C:\BIO\BUILD\vibprmpt.obj :  ..\vibrant\vibprmpt.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\vibprmpt.c

C:\BIO\BUILD\vibsbars.obj :  ..\vibrant\vibsbars.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\vibsbars.c

C:\BIO\BUILD\vibslate.obj :  ..\vibrant\vibslate.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\vibslate.c

C:\BIO\BUILD\vibtexts.obj :  ..\vibrant\vibtexts.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\vibtexts.c

C:\BIO\BUILD\vibutils.obj :  ..\vibrant\vibutils.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\vibutils.c

C:\BIO\BUILD\vibwndws.obj :  ..\vibrant\vibwndws.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\vibwndws.c

C:\BIO\BUILD\viewer.obj :  ..\vibrant\viewer.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\viewer.c

Dep_CcbBIObBINbcorelibdlib = \
   C:\BIO\BUILD\ncbibs.obj\
   C:\BIO\BUILD\ncbienv.obj\
   C:\BIO\BUILD\ncbierr.obj\
   C:\BIO\BUILD\ncbifile.obj\
   C:\BIO\BUILD\ncbimath.obj\
   C:\BIO\BUILD\ncbimem.obj\
   C:\BIO\BUILD\ncbimisc.obj\
   C:\BIO\BUILD\ncbimsg.obj\
   C:\BIO\BUILD\ncbiprop.obj\
   C:\BIO\BUILD\ncbisgml.obj\
   C:\BIO\BUILD\ncbistr.obj\
   C:\BIO\BUILD\ncbitime.obj

C:\BIO\BIN\corelib.lib : $(Dep_CcbBIObBINbcorelibdlib)
  $(TLIB) $< $(IDE_BFLAGS) $(BEAT_CcbBIObBINbcorelibdlib) @&&|
 -+C:\BIO\BUILD\ncbibs.obj&
-+C:\BIO\BUILD\ncbienv.obj&
-+C:\BIO\BUILD\ncbierr.obj&
-+C:\BIO\BUILD\ncbifile.obj&
-+C:\BIO\BUILD\ncbimath.obj&
-+C:\BIO\BUILD\ncbimem.obj&
-+C:\BIO\BUILD\ncbimisc.obj&
-+C:\BIO\BUILD\ncbimsg.obj&
-+C:\BIO\BUILD\ncbiprop.obj&
-+C:\BIO\BUILD\ncbisgml.obj&
-+C:\BIO\BUILD\ncbistr.obj&
-+C:\BIO\BUILD\ncbitime.obj
|

C:\BIO\BUILD\ncbibs.obj :  ..\corelib\ncbibs.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbcorelibdlib) -o$@ ..\corelib\ncbibs.c

C:\BIO\BUILD\ncbienv.obj :  ..\corelib\ncbienv.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbcorelibdlib) -o$@ ..\corelib\ncbienv.c

C:\BIO\BUILD\ncbierr.obj :  ..\corelib\ncbierr.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbcorelibdlib) -o$@ ..\corelib\ncbierr.c

C:\BIO\BUILD\ncbifile.obj :  ..\corelib\ncbifile.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbcorelibdlib) -o$@ ..\corelib\ncbifile.c

C:\BIO\BUILD\ncbimath.obj :  ..\corelib\ncbimath.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbcorelibdlib) -o$@ ..\corelib\ncbimath.c

C:\BIO\BUILD\ncbimem.obj :  ..\corelib\ncbimem.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbcorelibdlib) -o$@ ..\corelib\ncbimem.c

C:\BIO\BUILD\ncbimisc.obj :  ..\corelib\ncbimisc.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbcorelibdlib) -o$@ ..\corelib\ncbimisc.c

C:\BIO\BUILD\ncbimsg.obj :  ..\corelib\ncbimsg.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbcorelibdlib) -o$@ ..\corelib\ncbimsg.c

C:\BIO\BUILD\ncbiprop.obj :  ..\corelib\ncbiprop.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbcorelibdlib) -o$@ ..\corelib\ncbiprop.c

C:\BIO\BUILD\ncbisgml.obj :  ..\corelib\ncbisgml.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbcorelibdlib) -o$@ ..\corelib\ncbisgml.c

C:\BIO\BUILD\ncbistr.obj :  ..\corelib\ncbistr.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbcorelibdlib) -o$@ ..\corelib\ncbistr.c

C:\BIO\BUILD\ncbitime.obj :  ..\corelib\ncbitime.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbcorelibdlib) -o$@ ..\corelib\ncbitime.c

Dep_CcbBIObBINbdclapdlib = \
   C:\BIO\BUILD\daboutbo.obj\
   C:\BIO\BUILD\dapplica.obj\
   C:\BIO\BUILD\dcontrol.obj\
   C:\BIO\BUILD\ddialogt.obj\
   C:\BIO\BUILD\ddrawpic.obj\
   C:\BIO\BUILD\dfile.obj\
   C:\BIO\BUILD\dgg.obj\
   C:\BIO\BUILD\diconlib.obj\
   C:\BIO\BUILD\dlist.obj\
   C:\BIO\BUILD\dmenu.obj\
   C:\BIO\BUILD\dobject.obj\
   C:\BIO\BUILD\dpanel.obj\
   C:\BIO\BUILD\dtablevi.obj\
   C:\BIO\BUILD\dtask.obj\
   C:\BIO\BUILD\dtaskcen.obj\
   C:\BIO\BUILD\dtaskmas.obj\
   C:\BIO\BUILD\dutil.obj\
   C:\BIO\BUILD\dvibrant.obj\
   C:\BIO\BUILD\dview.obj\
   C:\BIO\BUILD\dviewcen.obj\
   C:\BIO\BUILD\dwindow.obj

C:\BIO\BIN\dclap.lib : $(Dep_CcbBIObBINbdclapdlib)
  $(TLIB) $< $(IDE_BFLAGS) $(BEAT_CcbBIObBINbdclapdlib) @&&|
 -+C:\BIO\BUILD\daboutbo.obj&
-+C:\BIO\BUILD\dapplica.obj&
-+C:\BIO\BUILD\dcontrol.obj&
-+C:\BIO\BUILD\ddialogt.obj&
-+C:\BIO\BUILD\ddrawpic.obj&
-+C:\BIO\BUILD\dfile.obj&
-+C:\BIO\BUILD\dgg.obj&
-+C:\BIO\BUILD\diconlib.obj&
-+C:\BIO\BUILD\dlist.obj&
-+C:\BIO\BUILD\dmenu.obj&
-+C:\BIO\BUILD\dobject.obj&
-+C:\BIO\BUILD\dpanel.obj&
-+C:\BIO\BUILD\dtablevi.obj&
-+C:\BIO\BUILD\dtask.obj&
-+C:\BIO\BUILD\dtaskcen.obj&
-+C:\BIO\BUILD\dtaskmas.obj&
-+C:\BIO\BUILD\dutil.obj&
-+C:\BIO\BUILD\dvibrant.obj&
-+C:\BIO\BUILD\dview.obj&
-+C:\BIO\BUILD\dviewcen.obj&
-+C:\BIO\BUILD\dwindow.obj
|

C:\BIO\BUILD\daboutbo.obj :  ..\dclap\daboutbo.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\daboutbo.cpp

C:\BIO\BUILD\dapplica.obj :  ..\dclap\dapplica.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dapplica.cpp

C:\BIO\BUILD\dcontrol.obj :  ..\dclap\dcontrol.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dcontrol.cpp

C:\BIO\BUILD\ddialogt.obj :  ..\dclap\ddialogt.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\ddialogt.cpp

C:\BIO\BUILD\ddrawpic.obj :  ..\dclap\ddrawpic.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\ddrawpic.c

C:\BIO\BUILD\dfile.obj :  ..\dclap\dfile.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dfile.cpp

C:\BIO\BUILD\dgg.obj :  ..\dclap\dgg.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dgg.c

C:\BIO\BUILD\diconlib.obj :  ..\dclap\diconlib.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\diconlib.cpp

C:\BIO\BUILD\dlist.obj :  ..\dclap\dlist.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dlist.cpp

C:\BIO\BUILD\dmenu.obj :  ..\dclap\dmenu.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dmenu.cpp

C:\BIO\BUILD\dobject.obj :  ..\dclap\dobject.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dobject.cpp

C:\BIO\BUILD\dpanel.obj :  ..\dclap\dpanel.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dpanel.cpp

C:\BIO\BUILD\dtablevi.obj :  ..\dclap\dtablevi.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dtablevi.cpp

C:\BIO\BUILD\dtask.obj :  ..\dclap\dtask.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dtask.cpp

C:\BIO\BUILD\dtaskcen.obj :  ..\dclap\dtaskcen.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dtaskcen.cpp

C:\BIO\BUILD\dtaskmas.obj :  ..\dclap\dtaskmas.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dtaskmas.cpp

C:\BIO\BUILD\dutil.obj :  ..\dclap\dutil.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dutil.cpp

C:\BIO\BUILD\dvibrant.obj :  ..\dclap\dvibrant.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dvibrant.c

C:\BIO\BUILD\dview.obj :  ..\dclap\dview.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dview.cpp

C:\BIO\BUILD\dviewcen.obj :  ..\dclap\dviewcen.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dviewcen.cpp

C:\BIO\BUILD\dwindow.obj :  ..\dclap\dwindow.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dwindow.cpp

C:\BIO\BUILD\rtfviewe.obj :  ..\dapp\rtfviewe.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbrtfviewdexe) -o$@ ..\dapp\rtfviewe.cpp

# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-R
-v
-vi
-X-
-H
-IC:\BC4\INCLUDE
-H=rtfview.csm
-A
-ml
-Ff
-wrpt
-ml
-WS
-IC:\BIO\DCLAP;C:\BIO\DAPP;C:\BIO\DRTF;C:\BIO\DNET;C:\BIO\NETWORK\NSCLILIB;C:\BIO\CORELIB;C:\BIO\VIBRANT;C:\BC4\INCLUDE
-n
-DDCLAP;
-b
-K-
-a-
-Ff=4000
-dc
-Vf
-g255
-A-
-w-hid
-2
-v-
-R-
| $@


