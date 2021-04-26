$! RUNDEMO.DCL
$! NCBI command procedure using VMS DCL to run demo programs
$!
$!=======================================================================
$ note := "write sys$output"
$!=======================================================================
$! Create symbols for demo programs
$!
$ set default [-.bin]
$ testcore :== $'f$environment("DEFAULT")'testcore.exe
$ asntool  :== $'f$environment("DEFAULT")'asntool.exe
$ set default [-.build]
$ getmesh  :== $'f$environment("DEFAULT")'getmesh.exe
$ indexpub :== $'f$environment("DEFAULT")'indexpub.exe
$ getpub   :== $'f$environment("DEFAULT")'getpub.exe
$ testobj  :== $'f$environment("DEFAULT")'testobj.exe
$ dosimple  :== $'f$environment("DEFAULT")'dosimple.exe
$ seqtest  :== $'f$environment("DEFAULT")'seqtest.exe
$ flattest  :== $'f$environment("DEFAULT")'flattest.exe
$!
$ if f$search("[-.bin]testcore.exe") .nes. ""
$   then
$   type sys$input
  
==== CoreTools demo ==========================================================

   Running the "testcore" demo program.  The program will tell you what to
   do at each step. The source code can be found in [-.coretool]testcore.c

$ read/prompt="Press return to continue..." sys$command junk
$   note "$"
$   note "$ testcore"
$   Assign/user_mode sys$command sys$input
$   testcore
$ else
$   note "The demo program ""testcore"" was not found in [-.bin]. Use @MAKEALL.DCL"
$ endif
$!
$ if f$search("[-.bin]asntool.exe") .nes. ""
$   then
$   type sys$input

==== AsnTool demo, Step 1 =====================================================

    The AsnTool Demos use an ASN.1 specification file called "asnpub.all" in
    the [-.asn] directory.  The file contains standard ASN.1 definitions as
    well as standard bibliographic forms. The test input data are in [-.demo].

    The first demo parses the specification file and reports any syntax 
    errors.  This step should be silent.

$ read/prompt="Press return to continue..." sys$command junk
$ note "$"
$ note "$ set default [.demo]"
$ note "$ asntool -m[-.asn]asnpub.all 
$   set default [-.demo]
$   if f$search("allpub.h") .nes. "" then delete allpub.h;*
$   if f$search("medline.val") .nes. "" then delete medline.val;*
$   if f$search("medline.out") .nes. "" then delete medline.out;*
$   asntool -m[-.asn]asnpub.all
$ type sys$input

==== AsnTool demo, Step 2 =====================================================

   This demo also checks the syntax of "asnpub.all" and then reads test
   data from the file "medline.ent" expecting it to be of a type specified
   in "asnpub.all".  Any errors are reported. This step should also be silent.

$ read/prompt="Press return to continue..." sys$command junk
$ note "$"
$  note "$ asntool -m[-.asn]asnpub.all -vmedline.ent"
$   asntool -m[-.asn]asnpub.all -vmedline.ent
$  type sys$input

==== AsnTool demo, Step 3 =====================================================

   This demo does all of the above but displays the contents of "medline.ent"
   in ASN.1 print form on the screen.  Please wait for the output to complete.

$ read/prompt="Press return to continue..." sys$command junk
$ note "$"
$ note "$ asntool -m[-.asn]asnpub.all -vmedline.ent -pstdout"
$   asntool -m[-.asn]asnpub.all -vmedline.ent -pstdout
$ type sys$input

==== AsnTool demo, Step 4 =====================================================

    This demo reads an ASN.1 print form entry, "medline.prt" and
    encodes it into an ASN.1 binary file, "medline.val".

$ read/prompt="Press return to continue..." sys$command junk
$ note "$"
$ note "$ asntool -m[-.asn]asnpub.all -vmedline.prt -emedline.val"
$   asntool -m[-.asn]asnpub.all -vmedline.prt -emedline.val
$ type sys$input

==== AsnTool demo, Step 5 =====================================================

   This demo decodes the ASN.1 binary form and displays it on the screen.
   Note that we MUST specify the type "Pub-set" because the binary form
   does not have this information.

$ read/prompt="Press return to continue..." sys$command junk
$ note "$"
$ note "$ asntool -m[-.asn]asnpub.all -dmedline.val ""-tPub-set"" -pstdout"
$   asntool -m[-.asn]asnpub.all -dmedline.val "-tPub-set" -pstdout
$ type sys$input

==== AsnTool demo, Step 6 =====================================================

   This demo parses the ASN.1 input specification and writes out a C
   language header file which can be used with AsnTool routines to encode
   and decode objects defined by "allpub.asn".  The output is in the
   file "allpub.h".  You may wish to examine this file later.

$ read/prompt="Press return to continue..." sys$command junk
$ note "$"
$ note "$ asntool -m[-.asn]asnpub.all -oallpub.h"
$   asntool -m[-.asn]asnpub.all -oallpub.h
$ else
$   note "The demo program ""anstool"" has not found in [-.bin]. Use @MAKEALL.DCL"
$ endif
$!=======================================================================
$ set default [-.demo]
$ if f$search("terms.out") .nes. "" then delete terms.out;*
$ if f$search("medline.idx") .nes. "" then delete medline.idx;*
$ if f$search("seqset.out") .nes. "" then delete seqset.out;*
$ type sys$input

==== getmesh demo =============================================================

   This demo reads a Medline-entry, extracts the MeSH terms, writes them to
   a file "terms.out" and then types that file to the screen.

$ read/prompt="Press return to continue..." sys$command junk
$ note "$"
$ note "$ getmesh -imedline.ent -oterms.out"
$   getmesh -imedline.ent -oterms.out
$ note "$ type terms.out"
$   type terms.out
$!=======================================================================
$ type sys$input

==== indexpub/getpub demo =====================================================

   This demo builds an index the medline.ent file based on Medline UID.
   The index is written to the file "medline.idx".  The next program
   "getpub" uses this index file to find and display the entry corresponding
   to UID 88055872.

$ read/prompt="Press return to continue..." sys$command junk
$ note "$"
$ note "$ indexpub -imedline.val"
$   indexpub -imedline.val
$ note "$ getpub -u88055872 -ostdout"
$   getpub -u88055872 -ostdout
$ type sys$input

==== getpub getmesh "pipe" demo ===============================================

   This demo extracts an entry from the index file, writes out a binary
   copy of it to a temporary file, the getmesh program is then invoked to
   extract the mesh terms and display them on the screen.

$ read/prompt="Press return to continue..." sys$command junk
$ note "$"
$ note "$ getpub -u88055872 -b -otemp.dat"
$ getpub -u88055872 -b -otemp.dat
$ note "$ getmesh -itemp.dat -b -ostdio"
$ getmesh -itemp.dat -b -oterms.out
$ note "$ type terms.out"
$ type terms.out
$ note "$ delete temp.dat;*"
$ delete temp.dat;*
$ type sys$input

==== object loader demo ===============================================

   This demo tests the object loader.  It reads the files "medline.ent"
   and "bioseq.set" and writes the files "medline.out" and "seqset.out"

$ read/prompt="Press return to continue..." sys$command junk
$ note "$"
$ note "$ testobj"
$   testobj
$ type sys$input

==== simple sequence demo ===============================================

   This demo tests the SimpleSeq functions.  It read the file "example.prt"
   and writes the file "simple.out"

$ read/prompt="Press return to continue..." sys$command junk
$ note "$"
$ note "$ dosimple"
$   dosimple
$ type sys$input

==== seqport demo ===============================================

   This demo tests the SeqPort functions.  It read the file "example.prt"
   and writes the file "seqtest.out"

$ read/prompt="Press return to continue..." sys$command junk
$ note "$"
$ note "$ seqtest"
$   seqtest
$ type sys$input

==== flat file generator demo ==========================================

   This demo tests the flatfile generator.  It read the file "example.prt"
   and writes a GenBank file to the screen

$ read/prompt="Press return to continue..." sys$command junk
$ note "$"
$ note "$ Asn2Gnbk"
$   asn2gnbk -a "example.prt" -e
$ exit
