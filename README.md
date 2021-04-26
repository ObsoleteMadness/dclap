# dclap
Don's Class Application (DCLAP) library is a C++ class library for building applications on the common windowing operating systems (Macintosh, MS-Windows, XWindow-Motif and possibly others).

The DCLAP code also contains source code for GopherPup and SeqPup. This may be of historic interest as it is an early cross-platform Gopher+ client with many advanced features. 

## Releases
I could find two released of DCLAP, 5b and 6d. Both are included here, please see Github [tags](https://github.com/ObsoleteMadness/dclap/tags).

## Original Readme

Don's Class Application (DCLAP) library is a C++ class library for
building applications on the common windowing operating systems
(Macintosh, MS-Windows, XWindow-Motif and possibly others).

This is built on the cross-platform toolkit from National Center for
Biotechnology Information (NCBI) of the National Library of Medicine 
(NLM), called NCBI toolkit, and especially the Vibrant window system
subset, plus the corelib subset.  This toolkit is written in ANSI C,
and compiles on a variety of systems.  The source for NCBI tools is
available for anonymous ftp from ncbi.nlm.nih.gov as
/toolbox/ncbi_tools/ncbi.tar.Z	

Most of this toolkit is directed toward manipulation biosequence data. 
The sections of it required for DCLAP are: 

	corelib/  -- basic library of ansi-c additions/enhancements
	vibrant/  -- user-interface (GUI) library, in c
	make/	  -- instructions for compiling (see though dclap's Dmake/)
	network/nsclilib/ -- need one or more headers from here
	network/apple -- for Macs, fetch added source from ...
	network/ncsasock -- support berkely style sockets w/ MacTCP
	network/winsock -- windows sockets interface, fetch added source from, e.g., 
	  ftp://SunSite.UNC.EDU:/pub/micro/pc-stuff/ms-windows/winsock/winsock-1.1/...

The release of NCBI toolkit for which dclap is tested will be available
at ftp.bio.indiana.edu:/util/dclap/from-ncbi/.  Look for a file "ncbi.diff"
for any differences between the ncbi toolkit and what is required for
dclap.

DCLAP is still very preliminary.  There is yet no detailed documentation
for it. It is based loosely on the MacApp and TCL application class
libraries available on Macintosh, but is structured to use the available
Vibrant toolkit methods.

The only platforms I've tested it with are Macintosh-Think C 6-Symantec C++,
Sun-GCC/G++-Motif, and MS Windows-Borland C++.  To compile with ThinkC/C++, 
see Dmake/mac-thc/.  To compile with GCC/G++, see the Dmake/unix-gcc/.  To
compile with Borland C++, see Dmake/mswin-bor/. See NCBI's toolkit for further
compiling instructions.

 
Source and updates will be available thru ftp/gopher at ftp.bio.indiana.edu,
/util/dclap...  Currently there are three sets of source required to
compile dclap applications:
	dclap.tar.Z   -- general application framework classes source
	dbio.tar.Z	  -- biosequence specific classes source
	ncbi.tar.Z	  -- NCBI's toolkit source
	ncbi.diff	  -- any patches to NCBI's toolkit needed for dclap
	
See the /util/dclap/utils section for Mac and MSWin tar and compress programs
to open the unix standard .tar.Z files.

Current applications written w/ Dclap include
	GopherPup	-- Internet gopher+ client
	SeqPup		-- biosequence editor and analysis platform
	MyApp		-- demonstration program
	StripPS		-- simple filter program (postscript to plain text)
	MailHelp	-- simple email-to-help-desk app
	
I'd like to thank Jonathan Kans, primary author of NCBI's Vibrant user-interface,
both for making this very useful code available to the community,
and for his efforts to incorporate changes for DCLAP into it.  There are
places where DCLAP specific code is included, and so for this use, you need
to compile the ncbi toolkit with DCLAP as a defined symbol.	

DCLAP is copyrighted 1994 by Don Gilbert.  You may use it in non-commercial
software without asking additional permission, and you may freely redistribute
it.  See Copyright.dclap document for details.

For DCLAP bug reports, comments, etc., email to 
	dclap@bio.indiana.edu
If you are working in the biocomputing/computational biology field and want
help w/ dclap, please ask.  Contributions to dclap source are welcome.  

-- Don Gilbert, 3 March 1994

