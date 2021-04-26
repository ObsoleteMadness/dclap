$!
$! Command procedure to build NCBI libraries, OpenVMS VAX
$!
$!  Usage:   @MAKEALL.DCL
$!
$! Simple build file for NCBI tool libraries.
$!
$! Builds "AsnTool" as the command line or Motif version depending on
$! availablity of Motif programming environment.
$!
$!   William A. Gilbert
$!   Whitehead Institute      gilbert@wi.mit.edu
$!   Univ. of New Hampshire   gilbert@unh.edu
$!
$!------------------------------------------------------------------------
$ save_verify = f$verify("yes")
$!------------------------------------------------------------------------
$! Compiler definitions
$!
$ CC = "cc/nolist/include=([],[-.include])"
$!
$!------------------------------------------------------------------------
$! Copy a new set of NCBI header files to the include directory
$!
$ if f$search("[-.include]*.*") .nes. "" then $ delete [-.include]*.*;*
$!
$ copy [-.corelib]*.h          [-.include]
$ copy [-.corelib]ncbilcl.vms  [-.include]ncbilcl.h
$ copy [-.corelib]ncbimain.vms [-.corelib]ncbimain.c
$ copy [-.asnlib]*.h           [-.include]
$ copy [-.object]*.h           [-.include]
$ copy [-.asnstat]*.h          [-.include]
$ copy [-.api]*.h              [-.include]
$ copy [-.cdromlib]*.h         [-.include]
$ copy [-.vibrant]*.h          [-.include]
$!--------------------------------------
$!
$! Make the Portable/ASN.1 library, libncbi.olb
$!
$ if f$search("*.obj") .nes. "" then $ delete *.obj;*
$ CC [-.corelib]ncbibs.c
$ CC [-.corelib]ncbienv.c
$ CC [-.corelib]ncbierr.c
$ CC [-.corelib]ncbifile.c
$ CC [-.corelib]ncbimain.c
$ CC [-.corelib]ncbimath.c
$ CC [-.corelib]ncbimem.c
$ CC [-.corelib]ncbimisc.c
$ CC [-.corelib]ncbiprop.c
$ CC [-.corelib]ncbisgml.c
$ CC [-.corelib]ncbimsg.c
$ CC [-.corelib]ncbistr.c
$ CC [-.corelib]ncbitime.c
$!
$ CC [-.asnlib]asndebin.c
$ CC [-.asnlib]asnenbin.c
$ CC [-.asnlib]asngen.c
$ CC [-.asnlib]asnio.c
$ CC [-.asnlib]asnlex.c
$ CC [-.asnlib]asnlext.c
$ CC [-.asnlib]asnout.c
$ CC [-.asnlib]asnprint.c
$ CC [-.asnlib]asntypes.c
$!
$ librarian/object/create [-.lib]libncbi.olb *.obj
$!--------------------------------------------------------------------
$!
$! Make the Object loader library, libncbiobj.olb
$!
$ if f$search("*.obj") .nes. "" then $ delete *.obj;*
$ CC [-.object]objacces.c
$ CC [-.object]objalign.c
$ CC [-.object]objall.c
$ CC [-.object]objbibli.c
$ CC [-.object]objblock.c
$ CC [-.object]objcode.c
$ CC [-.object]objfeat.c
$ CC [-.object]objgen.c
$ CC [-.object]objloc.c
$ CC [-.object]objmedli.c
$ CC [-.object]objpub.c
$ CC [-.object]objres.c
$ CC [-.object]objseq.c
$ CC [-.object]objsset.c
$ CC [-.object]objsub.c
$ CC [-.object]objprt.c
$!
$ CC [-.api]seqport.c
$ CC [-.api]sequtil.c
$ CC [-.api]prtutil.c
$ CC [-.api]simple.c
$ CC [-.api]tofasta.c
$ CC [-.api]tofile.c
$ CC [-.api]tognbk1.c
$ CC [-.api]tognbk2.c
$ CC [-.api]tognbk3.c
$ CC [-.api]tomedlin.c
$ CC [-.api]toreport.c
$ CC [-.api]valid.c
$ CC [-.api]subutil.c
$ CC [-.api]edutil.c
$!
$ librarian/object/create [-.lib]libncbiobj.olb *.obj
$!----------------------------------------------------------------------
$!
$! Make the Entrez CD Access library, libncbicdr.olb
$!
$ if f$search("*.obj") .nes. "" then $ delete *.obj;*
$!
$ CC [-.cdromlib]cdentrez.c
$ CC [-.cdromlib]cdromlib.c
$ CC [-.cdromlib]cdconfig.c
$ CC [-.cdromlib]casn.c
$!
$ librarian/object/create [-.lib]libncbicdr.olb *.obj
$!
$!----------------------------------------------------------------------
$!
$! Make High level Entrez Access modules
$!
$ CC/object=[-.lib] [-.cdromlib]accentr.c
$ CC/object=[-.lib] [-.cdromlib]accutils.c
$!
$!------------------------------------------------------------------------
$! If this VMS system does not have Motif Development include files skip
$! building the VIBRANT interface library.
$!
$  if f$search("decw$include:Xm.h") .eqs. "" then Goto BuildASNTool
$!------------------------------------------------------------------------
$!
$! Make libvibrant.olb
$!
$ define/nolog sys sys$library
$ define/nolog X11 decw$include
$ define/nolog Xm  decw$include
$!
$ if f$search("*.obj") .nes. "" then $ delete *.obj;*
$!
$ CC/define=WIN_MOTIF [-.vibrant]vibbutns.c
$ CC/define=WIN_MOTIF [-.vibrant]vibextra.c
$ CC/define=WIN_MOTIF [-.vibrant]vibfiles.c
$ CC/define=WIN_MOTIF [-.vibrant]vibgroup.c
$ CC/define=WIN_MOTIF [-.vibrant]viblists.c
$ CC/define=WIN_MOTIF [-.vibrant]vibmenus.c
$ CC/define=WIN_MOTIF [-.vibrant]vibprmpt.c
$ CC/define=WIN_MOTIF [-.vibrant]vibsbars.c
$ CC/define=WIN_MOTIF [-.vibrant]vibslate.c
$ CC/define=WIN_MOTIF [-.vibrant]vibtexts.c
$ CC/define=WIN_MOTIF [-.vibrant]vibutils.c
$ CC/define=WIN_MOTIF [-.vibrant]vibwndws.c
$ CC/define=WIN_MOTIF [-.vibrant]ncbidraw.c
$ CC/define=WIN_MOTIF [-.vibrant]drawing.c
$ CC/define=WIN_MOTIF [-.vibrant]mapping.c
$ CC/define=WIN_MOTIF [-.vibrant]picture.c
$ CC/define=WIN_MOTIF [-.vibrant]viewer.c
$ CC/define=WIN_MOTIF [-.vibrant]document.c
$ CC/define=WIN_MOTIF [-.vibrant]palette.c
$ CC/define=WIN_MOTIF [-.vibrant]table.c
$!
$ librarian/object/create [-.lib]libvibrant.olb *.obj
$ deassign sys
$ deassign X11
$ deassign Xm
$ goto Exit
$!-------------------------------------------------------------------------
$!
$! Make asntool
$!
$ BuildASNTool:
$!
$ CC [-.asnlib]asntool.c
$!
$! Has Vibrant library?
$!
$ if f$search("[-.lib]libvibrant.olb") .nes. ""
$ then
$   link asntool.obj,sys$input/option
      [-.lib]libvibrant.olb/library
      [-.lib]libncbi.olb/library
      sys$share:decw$xlibshr.exe/shareable
      sys$share:decw$dxmlibshr.exe/shareable
      sys$share:decw$xmlibshr.exe/shareable
      sys$share:decw$xtshr.exe/shareable
      sys$share:vaxcrtl.exe/shareable 
$!
$! Doesn't have vibrant library
$!
$ else
$   link asntool.obj,sys$input/option
      [-.lib]libncbi.olb/library
      sys$share:vaxcrtl.exe/shareable 
$ endif
$!
$ Exit:
$ if .not.save_verify then set noverify
$ exit


