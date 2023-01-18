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
# External tools
#
CreateImportLibrary = implib.exe  # IDE Command Line: $IMPLIB


#
# Options
#
IDE_LFLAGS =  -LC:\BIO\BIN;C:\BC4\LIB -x
IDE_RFLAGS =  -IC:\BIO\DBIO;C:\BIO\CORELIB;C:\BIO\DCLAP;C:\BIO\DNET;C:\BIO\VIBRANT;C:\BIO\NETWORK\NSCLILIB;C:\BC4\INCLUDE
LLATW16_CcbBIObBINbgophpupdexe =  -x -Twe -c -C
RLATW16_CcbBIObBINbgophpupdexe =  -IC:\BIO\DBIO;C:\BIO\CORELIB;C:\BIO\DRTF;C:\BIO\DCLAP;C:\BIO\DNET;C:\BIO\VIBRANT;C:\BIO\NETWORK\NSCLILIB;C:\BC4\INCLUDE -31
BLATW16_CcbBIObBINbgophpupdexe = 
LEAT_CcbBIObBINbgophpupdexe = $(LLATW16_CcbBIObBINbgophpupdexe)
REAT_CcbBIObBINbgophpupdexe = $(RLATW16_CcbBIObBINbgophpupdexe)
BEAT_CcbBIObBINbgophpupdexe = $(BLATW16_CcbBIObBINbgophpupdexe)
CLATW16_CcbBIObBINbrtfdlib =  -ml -WS
LLATW16_CcbBIObBINbrtfdlib =  -Twe
RLATW16_CcbBIObBINbrtfdlib =  -31
BLATW16_CcbBIObBINbrtfdlib = 
CEAT_CcbBIObBINbrtfdlib = $(CEAT_CcbBIObBINbgophpupdexe) $(CLATW16_CcbBIObBINbrtfdlib)
LEAT_CcbBIObBINbrtfdlib = $(LEAT_CcbBIObBINbgophpupdexe) $(LLATW16_CcbBIObBINbrtfdlib)
REAT_CcbBIObBINbrtfdlib = $(REAT_CcbBIObBINbgophpupdexe) $(RLATW16_CcbBIObBINbrtfdlib)
BEAT_CcbBIObBINbrtfdlib = $(BEAT_CcbBIObBINbgophpupdexe) $(BLATW16_CcbBIObBINbrtfdlib)
CLATW16_ddbdrtfbreaderdc =  -w-pro
LLATW16_ddbdrtfbreaderdc = 
RLATW16_ddbdrtfbreaderdc = 
BLATW16_ddbdrtfbreaderdc = 
CEAT_ddbdrtfbreaderdc = $(CEAT_CcbBIObBINbrtfdlib) $(CLATW16_ddbdrtfbreaderdc)
LEAT_ddbdrtfbreaderdc = $(LEAT_CcbBIObBINbrtfdlib) $(LLATW16_ddbdrtfbreaderdc)
REAT_ddbdrtfbreaderdc = $(REAT_CcbBIObBINbrtfdlib) $(RLATW16_ddbdrtfbreaderdc)
BEAT_ddbdrtfbreaderdc = $(BEAT_CcbBIObBINbrtfdlib) $(BLATW16_ddbdrtfbreaderdc)
CLATW16_CcbBIObBINbcorelibdlib =  -ml -WS
LLATW16_CcbBIObBINbcorelibdlib =  -Twe
RLATW16_CcbBIObBINbcorelibdlib =  -31
BLATW16_CcbBIObBINbcorelibdlib = 
CEAT_CcbBIObBINbcorelibdlib = $(CEAT_CcbBIObBINbgophpupdexe) $(CLATW16_CcbBIObBINbcorelibdlib)
LEAT_CcbBIObBINbcorelibdlib = $(LEAT_CcbBIObBINbgophpupdexe) $(LLATW16_CcbBIObBINbcorelibdlib)
REAT_CcbBIObBINbcorelibdlib = $(REAT_CcbBIObBINbgophpupdexe) $(RLATW16_CcbBIObBINbcorelibdlib)
BEAT_CcbBIObBINbcorelibdlib = $(BEAT_CcbBIObBINbgophpupdexe) $(BLATW16_CcbBIObBINbcorelibdlib)
CLATW16_CcbBIObBINbvibrantdlib =  -ml -WS
LLATW16_CcbBIObBINbvibrantdlib =  -Twe
RLATW16_CcbBIObBINbvibrantdlib =  -31
BLATW16_CcbBIObBINbvibrantdlib = 
CEAT_CcbBIObBINbvibrantdlib = $(CEAT_CcbBIObBINbgophpupdexe) $(CLATW16_CcbBIObBINbvibrantdlib)
LEAT_CcbBIObBINbvibrantdlib = $(LEAT_CcbBIObBINbgophpupdexe) $(LLATW16_CcbBIObBINbvibrantdlib)
REAT_CcbBIObBINbvibrantdlib = $(REAT_CcbBIObBINbgophpupdexe) $(RLATW16_CcbBIObBINbvibrantdlib)
BEAT_CcbBIObBINbvibrantdlib = $(BEAT_CcbBIObBINbgophpupdexe) $(BLATW16_CcbBIObBINbvibrantdlib)
CLATW16_CcbBIObBINbdclapdlib =  -ml -WS
LLATW16_CcbBIObBINbdclapdlib =  -Twe
RLATW16_CcbBIObBINbdclapdlib =  -31
BLATW16_CcbBIObBINbdclapdlib = 
CEAT_CcbBIObBINbdclapdlib = $(CEAT_CcbBIObBINbgophpupdexe) $(CLATW16_CcbBIObBINbdclapdlib)
LEAT_CcbBIObBINbdclapdlib = $(LEAT_CcbBIObBINbgophpupdexe) $(LLATW16_CcbBIObBINbdclapdlib)
REAT_CcbBIObBINbdclapdlib = $(REAT_CcbBIObBINbgophpupdexe) $(RLATW16_CcbBIObBINbdclapdlib)
BEAT_CcbBIObBINbdclapdlib = $(BEAT_CcbBIObBINbgophpupdexe) $(BLATW16_CcbBIObBINbdclapdlib)
CLATW16_ddbnetworkbwinsockbwinsockdlib =  -ml -WS
LLATW16_ddbnetworkbwinsockbwinsockdlib =  -Twe
RLATW16_ddbnetworkbwinsockbwinsockdlib =  -31
BLATW16_ddbnetworkbwinsockbwinsockdlib = 
CEAT_ddbnetworkbwinsockbwinsockdlib = $(CEAT_CcbBIObBINbgophpupdexe) $(CLATW16_ddbnetworkbwinsockbwinsockdlib)
LEAT_ddbnetworkbwinsockbwinsockdlib = $(LEAT_CcbBIObBINbgophpupdexe) $(LLATW16_ddbnetworkbwinsockbwinsockdlib)
REAT_ddbnetworkbwinsockbwinsockdlib = $(REAT_CcbBIObBINbgophpupdexe) $(RLATW16_ddbnetworkbwinsockbwinsockdlib)
BEAT_ddbnetworkbwinsockbwinsockdlib = $(BEAT_CcbBIObBINbgophpupdexe) $(BLATW16_ddbnetworkbwinsockbwinsockdlib)
CLATW16_CcbBIObBINbdnetdlib =  -ml -WS
LLATW16_CcbBIObBINbdnetdlib =  -Twe
RLATW16_CcbBIObBINbdnetdlib =  -31
BLATW16_CcbBIObBINbdnetdlib = 
CEAT_CcbBIObBINbdnetdlib = $(CEAT_CcbBIObBINbgophpupdexe) $(CLATW16_CcbBIObBINbdnetdlib)
LEAT_CcbBIObBINbdnetdlib = $(LEAT_CcbBIObBINbgophpupdexe) $(LLATW16_CcbBIObBINbdnetdlib)
REAT_CcbBIObBINbdnetdlib = $(REAT_CcbBIObBINbgophpupdexe) $(RLATW16_CcbBIObBINbdnetdlib)
BEAT_CcbBIObBINbdnetdlib = $(BEAT_CcbBIObBINbgophpupdexe) $(BLATW16_CcbBIObBINbdnetdlib)

#
# Dependency List
#
Dep_gophpup = \
   C:\BIO\BIN\gophpup.exe

gophpup : BccW16.cfg $(Dep_gophpup)
  echo MakeNode gophpup

Dep_CcbBIObBINbgophpupdexe = \
   C:\BIO\BUILD\gopherpu.obj\
   C:\BIO\BIN\rtf.lib\
   C:\BIO\BIN\corelib.lib\
   C:\BIO\BIN\vibrant.lib\
   C:\BIO\BIN\dclap.lib\
   ..\network\winsock\winsock.lib\
   C:\BIO\BIN\dnet.lib

C:\BIO\BIN\gophpup.exe : $(Dep_CcbBIObBINbgophpupdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGS) $(LEAT_CcbBIObBINbgophpupdexe) +
C:\BC4\LIB\c0wl.obj+
C:\BIO\BUILD\gopherpu.obj
$<,$*
C:\BIO\BIN\rtf.lib+
C:\BIO\BIN\corelib.lib+
C:\BIO\BIN\vibrant.lib+
C:\BIO\BIN\dclap.lib+
..\network\winsock\winsock.lib+
C:\BIO\BIN\dnet.lib+
C:\BC4\LIB\import.lib+
C:\BC4\LIB\mathwl.lib+
C:\BC4\LIB\cwl.lib
..\dapp\gopherpu.def
|

C:\BIO\BUILD\gopherpu.obj :  ..\dapp\gopherpu.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbgophpupdexe) -o$@ ..\dapp\gopherpu.cpp

Dep_CcbBIObBINbrtfdlib = \
   C:\BIO\BUILD\drtfdoc.obj\
   C:\BIO\BUILD\drtfview.obj\
   C:\BIO\BUILD\drtfwrit.obj\
   C:\BIO\BUILD\reader.obj\
   C:\BIO\BUILD\tokensca.obj

C:\BIO\BIN\rtf.lib : $(Dep_CcbBIObBINbrtfdlib)
  $(TLIB) $< $(IDE_BFLAGS) $(BEAT_CcbBIObBINbrtfdlib) @&&|
 -+C:\BIO\BUILD\drtfdoc.obj&
-+C:\BIO\BUILD\drtfview.obj&
-+C:\BIO\BUILD\drtfwrit.obj&
-+C:\BIO\BUILD\reader.obj&
-+C:\BIO\BUILD\tokensca.obj
|

C:\BIO\BUILD\drtfdoc.obj :  ..\drtf\drtfdoc.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbrtfdlib) -o$@ ..\drtf\drtfdoc.c

C:\BIO\BUILD\drtfview.obj :  ..\drtf\drtfview.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbrtfdlib) -o$@ ..\drtf\drtfview.cpp

C:\BIO\BUILD\drtfwrit.obj :  ..\drtf\drtfwrit.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbrtfdlib) -o$@ ..\drtf\drtfwrit.c

C:\BIO\BUILD\reader.obj :  ..\drtf\reader.c
  $(BCC)   -P- -c $(CEAT_ddbdrtfbreaderdc) -o$@ ..\drtf\reader.c

C:\BIO\BUILD\tokensca.obj :  ..\drtf\tokensca.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbrtfdlib) -o$@ ..\drtf\tokensca.c

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

Dep_CcbBIObBINbvibrantdlib = \
   C:\BIO\BUILD\ncbidraw.obj\
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
   C:\BIO\BUILD\viewer.obj\
   C:\BIO\BUILD\document.obj\
   C:\BIO\BUILD\drawing.obj\
   C:\BIO\BUILD\mapping.obj\
   C:\BIO\BUILD\palette.obj\
   C:\BIO\BUILD\picture.obj\
   C:\BIO\BUILD\table.obj

C:\BIO\BIN\vibrant.lib : $(Dep_CcbBIObBINbvibrantdlib)
  $(TLIB) $< $(IDE_BFLAGS) $(BEAT_CcbBIObBINbvibrantdlib) @&&|
 -+C:\BIO\BUILD\ncbidraw.obj&
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
-+C:\BIO\BUILD\viewer.obj&
-+C:\BIO\BUILD\document.obj&
-+C:\BIO\BUILD\drawing.obj&
-+C:\BIO\BUILD\mapping.obj&
-+C:\BIO\BUILD\palette.obj&
-+C:\BIO\BUILD\picture.obj&
-+C:\BIO\BUILD\table.obj
|

C:\BIO\BUILD\ncbidraw.obj :  ..\vibrant\ncbidraw.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\ncbidraw.c

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

C:\BIO\BUILD\document.obj :  ..\vibrant\document.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\document.c

C:\BIO\BUILD\drawing.obj :  ..\vibrant\drawing.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\drawing.c

C:\BIO\BUILD\mapping.obj :  ..\vibrant\mapping.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\mapping.c

C:\BIO\BUILD\palette.obj :  ..\vibrant\palette.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\palette.c

C:\BIO\BUILD\picture.obj :  ..\vibrant\picture.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\picture.c

C:\BIO\BUILD\table.obj :  ..\vibrant\table.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbvibrantdlib) -o$@ ..\vibrant\table.c

Dep_CcbBIObBINbdclapdlib = \
   C:\BIO\BUILD\dvibrant.obj\
   C:\BIO\BUILD\dgg.obj\
   C:\BIO\BUILD\dobject.obj\
   C:\BIO\BUILD\daboutbo.obj\
   C:\BIO\BUILD\dapplica.obj\
   C:\BIO\BUILD\dcontrol.obj\
   C:\BIO\BUILD\ddialogt.obj\
   C:\BIO\BUILD\dfile.obj\
   C:\BIO\BUILD\diconlib.obj\
   C:\BIO\BUILD\dlist.obj\
   C:\BIO\BUILD\dmenu.obj\
   C:\BIO\BUILD\dpanel.obj\
   C:\BIO\BUILD\dtablevi.obj\
   C:\BIO\BUILD\dtask.obj\
   C:\BIO\BUILD\dtaskcen.obj\
   C:\BIO\BUILD\dtaskmas.obj\
   C:\BIO\BUILD\dutil.obj\
   C:\BIO\BUILD\dview.obj\
   C:\BIO\BUILD\dviewcen.obj\
   C:\BIO\BUILD\dwindow.obj

C:\BIO\BIN\dclap.lib : $(Dep_CcbBIObBINbdclapdlib)
  $(TLIB) $< $(IDE_BFLAGS) $(BEAT_CcbBIObBINbdclapdlib) @&&|
 -+C:\BIO\BUILD\dvibrant.obj&
-+C:\BIO\BUILD\dgg.obj&
-+C:\BIO\BUILD\dobject.obj&
-+C:\BIO\BUILD\daboutbo.obj&
-+C:\BIO\BUILD\dapplica.obj&
-+C:\BIO\BUILD\dcontrol.obj&
-+C:\BIO\BUILD\ddialogt.obj&
-+C:\BIO\BUILD\dfile.obj&
-+C:\BIO\BUILD\diconlib.obj&
-+C:\BIO\BUILD\dlist.obj&
-+C:\BIO\BUILD\dmenu.obj&
-+C:\BIO\BUILD\dpanel.obj&
-+C:\BIO\BUILD\dtablevi.obj&
-+C:\BIO\BUILD\dtask.obj&
-+C:\BIO\BUILD\dtaskcen.obj&
-+C:\BIO\BUILD\dtaskmas.obj&
-+C:\BIO\BUILD\dutil.obj&
-+C:\BIO\BUILD\dview.obj&
-+C:\BIO\BUILD\dviewcen.obj&
-+C:\BIO\BUILD\dwindow.obj
|

C:\BIO\BUILD\dvibrant.obj :  ..\dclap\dvibrant.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dvibrant.c

C:\BIO\BUILD\dgg.obj :  ..\dclap\dgg.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dgg.c

C:\BIO\BUILD\dobject.obj :  ..\dclap\dobject.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dobject.cpp

C:\BIO\BUILD\daboutbo.obj :  ..\dclap\daboutbo.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\daboutbo.cpp

C:\BIO\BUILD\dapplica.obj :  ..\dclap\dapplica.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dapplica.cpp

C:\BIO\BUILD\dcontrol.obj :  ..\dclap\dcontrol.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dcontrol.cpp

C:\BIO\BUILD\ddialogt.obj :  ..\dclap\ddialogt.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\ddialogt.cpp

C:\BIO\BUILD\dfile.obj :  ..\dclap\dfile.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dfile.cpp

C:\BIO\BUILD\diconlib.obj :  ..\dclap\diconlib.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\diconlib.cpp

C:\BIO\BUILD\dlist.obj :  ..\dclap\dlist.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dlist.cpp

C:\BIO\BUILD\dmenu.obj :  ..\dclap\dmenu.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dmenu.cpp

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

C:\BIO\BUILD\dview.obj :  ..\dclap\dview.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dview.cpp

C:\BIO\BUILD\dviewcen.obj :  ..\dclap\dviewcen.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dviewcen.cpp

C:\BIO\BUILD\dwindow.obj :  ..\dclap\dwindow.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdclapdlib) -o$@ ..\dclap\dwindow.cpp

Dep_ddbnetworkbwinsockbwinsockdlib = \
   ..\network\winsock\winsock.lib

..\network\winsock\winsock.lib : $(Dep_ddbnetworkbwinsockbwinsockdlib)
  $(IMPLIB) $@ ..\network\winsock\winsock.lib

..\network\winsock\winsock.lib :  ..\network\winsock\winsock.def
  $(IMPLIB) $@ ..\network\winsock\winsock.def

Dep_CcbBIObBINbdnetdlib = \
   C:\BIO\BUILD\dgoplus.obj\
   C:\BIO\BUILD\dgoaskdo.obj\
   C:\BIO\BUILD\dgopher.obj\
   C:\BIO\BUILD\dgolist.obj\
   C:\BIO\BUILD\dgoclass.obj\
   C:\BIO\BUILD\dgodoc.obj\
   C:\BIO\BUILD\dgoinit.obj\
   C:\BIO\BUILD\dgoprefs.obj\
   C:\BIO\BUILD\dnettcp.obj\
   C:\BIO\BUILD\dsendmai.obj\
   C:\BIO\BUILD\dsmtpcli.obj\
   C:\BIO\BUILD\dtcp.obj

C:\BIO\BIN\dnet.lib : $(Dep_CcbBIObBINbdnetdlib)
  $(TLIB) $< $(IDE_BFLAGS) $(BEAT_CcbBIObBINbdnetdlib) @&&|
 -+C:\BIO\BUILD\dgoplus.obj&
-+C:\BIO\BUILD\dgoaskdo.obj&
-+C:\BIO\BUILD\dgopher.obj&
-+C:\BIO\BUILD\dgolist.obj&
-+C:\BIO\BUILD\dgoclass.obj&
-+C:\BIO\BUILD\dgodoc.obj&
-+C:\BIO\BUILD\dgoinit.obj&
-+C:\BIO\BUILD\dgoprefs.obj&
-+C:\BIO\BUILD\dnettcp.obj&
-+C:\BIO\BUILD\dsendmai.obj&
-+C:\BIO\BUILD\dsmtpcli.obj&
-+C:\BIO\BUILD\dtcp.obj
|

C:\BIO\BUILD\dgoplus.obj :  ..\dnet\dgoplus.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdnetdlib) -o$@ ..\dnet\dgoplus.cpp

C:\BIO\BUILD\dgoaskdo.obj :  ..\dnet\dgoaskdo.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdnetdlib) -o$@ ..\dnet\dgoaskdo.cpp

C:\BIO\BUILD\dgopher.obj :  ..\dnet\dgopher.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdnetdlib) -o$@ ..\dnet\dgopher.cpp

C:\BIO\BUILD\dgolist.obj :  ..\dnet\dgolist.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdnetdlib) -o$@ ..\dnet\dgolist.cpp

C:\BIO\BUILD\dgoclass.obj :  ..\dnet\dgoclass.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdnetdlib) -o$@ ..\dnet\dgoclass.cpp

C:\BIO\BUILD\dgodoc.obj :  ..\dnet\dgodoc.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdnetdlib) -o$@ ..\dnet\dgodoc.cpp

C:\BIO\BUILD\dgoinit.obj :  ..\dnet\dgoinit.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdnetdlib) -o$@ ..\dnet\dgoinit.cpp

C:\BIO\BUILD\dgoprefs.obj :  ..\dnet\dgoprefs.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdnetdlib) -o$@ ..\dnet\dgoprefs.cpp

C:\BIO\BUILD\dnettcp.obj :  ..\dnet\dnettcp.c
  $(BCC)   -P- -c $(CEAT_CcbBIObBINbdnetdlib) -o$@ ..\dnet\dnettcp.c

C:\BIO\BUILD\dsendmai.obj :  ..\dnet\dsendmai.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdnetdlib) -o$@ ..\dnet\dsendmai.cpp

C:\BIO\BUILD\dsmtpcli.obj :  ..\dnet\dsmtpcli.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdnetdlib) -o$@ ..\dnet\dsmtpcli.cpp

C:\BIO\BUILD\dtcp.obj :  ..\dnet\dtcp.cpp
  $(BCC)   -c $(CEAT_CcbBIObBINbdnetdlib) -o$@ ..\dnet\dtcp.cpp

# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-R
-v
-vi
-X-
-H
-IC:\BIO\DBIO;C:\BIO\CORELIB;C:\BIO\DCLAP;C:\BIO\DNET;C:\BIO\VIBRANT;C:\BIO\NETWORK\NSCLILIB;C:\BC4\INCLUDE
-H=seqpup.csm
-A-
-ml
-Ff
-wrpt
-n
-b
-K-
-w-sig
-wcpt
-wrng
-w-par
-DDCLAP;
-v
-R
-w-hid
-IC:\BIO\DBIO;C:\BIO\CORELIB;C:\BIO\DRTF;C:\BIO\DCLAP;C:\BIO\DNET;C:\BIO\VIBRANT;C:\BIO\NETWORK\NSCLILIB;C:\BC4\INCLUDE
-wpro
-Ff=2000
-Vf
-dc
-g255
-ml
-WS
| $@


