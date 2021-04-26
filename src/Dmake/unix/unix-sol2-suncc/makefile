#	makefile for dclap C++ library routines
#
## Gnu gcc/g+ has been tested...
## this one is for Sun Solaris w/ Sun cc and CC 
#
# dgg note -- for motif link to work, remember to set 
#  setenv LD_LIBRARY_PATH /usr/lib:/usr/openwin/lib

# default flags for compiling and loading

#------------------------ local setup ---------------
#SUFFIXLCL = $(LCL)
SUFFIXLCL = sol
CPFLAGS1 = -c -g -sb #-O 
CFLAGS1 = -c -DDCLAP -g -xsb #-O -g 
LDFLAGS1 = -O
CC = cc
CPP = CC
RAN = echo #ranlib
AR=ar
# get rid of debug symbols
STRIP=echo  #strip

# solaris needs /usr/ccs/lib/libgen.a for regcmp/regex weird call from Xlib
## but damn libXm isn't finding libgen funcs ???????
OTHERLIBS = # -lgen  # -lm # -L/usr/ccs/lib -lgen

OURLIBS = -lgen -lvibrant -ldclap -ldrich -lncbi
BIOLIBS = -ldbio -ldbio2 
## SRV4 libs == Solaris libs
NETLIBS = -ldnet -lsocket -lnsl
GOPHLIBS = -ldgoph
##NETLIBS = -ldnet  


# try static linked x libs
# note: !static == shared motif libs == runs on Solaris2 & SunOs
#       -static == static libs == fails on Solaris2 (some non-shared stdlib problem)
VIBLIBS =  -lXm  -lXt -lX11  #-lXmu
#VIBLIBS = -B static -lXm -lXt -lX11 # -lXmu 
#VIBFLAG = -I/opt/IXIX11R5/include -I/opt/SUNWmotif/share/include -DWIN_MOTIF 
# sol2.4 w/ SDK Motif
VIBFLAG = -I/usr/dt/include -I/usr/openwin/include -DWIN_MOTIF 

#------------------------ end local setup ---------------


SUFFIXENV = unx
LIBNCBI = libncbi.a
LIBVIB = libvibrant.a
LIBDNET =  libdnet.a
LIBDGOPH = libdgoph.a
LIBDCLAP = libdclap.a
LIBDRICH = libdrich.a
LIBDBIO =  libdbio.a
LIBDBIO2 = libdbio2.a

INCPATH = ../include
LIBPATH = ../lib   #-L/usr/ccs/lib 
CFLAGS = $(CFLAGS1) -I$(INCPATH) $(VIBFLAG)
CPFLAGS = $(CPFLAGS1) -I$(INCPATH) $(VIBFLAG)
LDFLAGS = $(LDFLAGS1) -I$(INCPATH) -L$(LIBPATH) $(VIBFLAG)

##
## some things to make
##

# sources & objects

# basic ncbi lib
SRCNCBI = ncbibs.c ncbierr.c ncbienv.c ncbifile.c ncbiprop.c ncbimsg.c \
	ncbimath.c ncbimem.c ncbimisc.c ncbistr.c ncbitime.c ncbisgml.c 
	
OBJNCBI = ncbibs.o ncbierr.o ncbienv.o ncbifile.o ncbiprop.o ncbimsg.o \
	ncbimath.o ncbimem.o ncbimisc.o ncbistr.o ncbisgml.o ncbitime.o 

# ncbi vibrant lib
SRCVIB =	vibbutns.c vibextra.c vibfiles.c vibgroup.c \
	viblists.c vibmenus.c vibprmpt.c vibsbars.c vibslate.c vibtexts.c \
	vibutils.c vibwndws.c ncbidraw.c drawing.c mapping.c picture.c viewer.c \
	document.c table.c palette.c

OBJVIB =	vibbutns.o vibextra.o vibfiles.o vibgroup.o \
	viblists.o vibmenus.o vibprmpt.o vibsbars.o vibslate.o vibtexts.o \
	vibutils.o vibwndws.o ncbidraw.o drawing.o mapping.o picture.o viewer.o \
	document.o table.o palette.o

# dclap primary lib
SRCDCLAP =  \
	DAboutBox.cc    DDialogText.cc  DObject.cc      DTaskCentral.cc DView.cc \
	DApplication.cc DList.cc        DPanel.cc       DTaskMaster.cc  DViewCentral.cc \
	DControl.cc     DMenu.cc        DTask.cc        DUtil.cc        DWindow.cc	\
	DIconLib.cc			DFile.cc				DTableView.cc		dgg.c  					Dvibrant.c \
  DTracker.cc		  DChildApp.cc    DClipboard.cc   DFindDlog.cc	Dversion.c

OBJDCLAP = \
	DAboutBox.o    DDialogText.o  DObject.o      DTaskCentral.o DView.o \
	DApplication.o DList.o        DPanel.o       DTaskMaster.o  DViewCentral.o \
	DControl.o     DMenu.o        DTask.o        DUtil.o        DWindow.o	\
	DIconLib.o     DFile.o				DTableView.o	 dgg.o    			Dvibrant.o \
  DTracker.o     DChildApp.o    DClipboard.o   DFindDlog.o	Dversion.o

# dclap rich text lib
SRCDRICH = \
  DRichStyle.cc DDrawPICT.cc DHTMLHandler.cc DPICTHandler.cc DRTFHandler.cc \
  DGIFHandler.cc DRichHandler.cc DRichProcess.cc DRichViewNu.cc reader.c

OBJDRICH = \
  DRichStyle.o  DDrawPICT.o  DHTMLHandler.o DPICTHandler.o   DRTFHandler.o \
  DGIFHandler.o  DRichHandler.o   DRichProcess.o  DRichViewNu.o reader.o

# dclap network lib
SRCDNET = \
    DSendMailDialog.cc   DSMTPclient.cc DTCP.cc  dnettcp.c
   		 
SRCDGOPH = \
    DGopher.cc DGoList.cc DGoClasses.cc  DGoPlus.cc DURL.cc 

SRCDGOPH2 = \
   DGoAskDoc.cc         DGoNetDoc.cc  DNetHTMLHandler.cc\
   DGoDoc.cc            DGoPrefs.cc   DGoInit.cc  
               

OBJDNET = \
   DSendMailDialog.o   DSMTPclient.o DTCP.o  dnettcp.o 
   		 
OBJDGOPH = \
    DGopher.o DGoList.o DGoClasses.o  DGoPlus.o DURL.o 

OBJDGOPH2 = \
   DGoAskDoc.o         DGoNetDoc.o  DNetHTMLHandler.o  \
   DGoDoc.o            DGoPrefs.o   DGoInit.o    
      

# dclap bio lib
SRCDBIO = \
  DREnzyme.cc       DSeqDoc.cc        DSeqList.cc       DSequence.cc \
  DSeqChildApp.cc   DSeqEd.cc         DSeqMail.cc       ureadseq.c

SRCDBIO2 = \
   DSeqViews.cc DSeqCmds.cc       DSeqFile.cc       DSeqPrint.cc	DSeqPict.cc  

OBJDBIO = \
  DREnzyme.o       DSeqDoc.o        DSeqList.o       DSequence.o \
  DSeqChildApp.o   DSeqEd.o         DSeqMail.o       ureadseq.o 

OBJDBIO2 = \
   DSeqViews.o  DSeqCmds.o       DSeqFile.o       DSeqPrint.o		DSeqPict.o 
					 

# application main programs

SRCAPP = MyApp.cc
SRCMAIL = MailHelp.cc
SRCGOPHER = GopherPup.cc
SRCSEQPUP = SeqPup.cc  DAboutSeqpup.cc

OBJAPP = MyApp.o
OBJMAIL = MailHelp.o
OBJGOPHER = GopherPup.o
OBJSEQPUP = SeqPup.o  DAboutSeqpup.o

EXEAPP =  SeqPup #MyApp GopherPup 



## All things to make
##
all : nocopy 

nocopy : sources  libs $(EXEAPP)

libs	: $(LIBNCBI) $(LIBVIB) $(LIBDRICH) $(LIBDCLAP) $(LIBDNET) $(LIBDBIO) $(LIBDBIO2)  # $(LIBDGOPH)

sources : $(SRCNCBI) $(SRCVIB) $(SRCDRICH) $(SRCDCLAP) $(SRCDNET) $(SRCDGOPH) $(SRCDGOPH2) $(SRCDBIO) $(SRCDBIO2) $(SRCAPP) $(SRCMAIL) $(SRCGOPHER) $(SRCSEQPUP)

## To clean out the directory without removing make
##
clean :
	- rm -f *.[ao]

## Implicit actions
##

.c.o :
	$(CC) $(CFLAGS) $<
	
# damn, I use .cp for C++ suffix, gcc wants .cc
.cc.o :
	$(CPP) $(CPFLAGS) $<

## get all the source files
##

##  libraries
##

$(LIBNCBI) : $(OBJNCBI)
	- rm -f $(LIBNCBI)
	$(AR) cru $(LIBNCBI) $(OBJNCBI)
	- ln -s ../build/$(LIBNCBI) ../lib/$(LIBNCBI)
	$(RAN) ../lib/$(LIBNCBI)

$(LIBVIB) : $(OBJVIB)
	- rm -f $(LIBVIB)
	$(AR) cru $(LIBVIB) $(OBJVIB)
	- ln -s ../build/$(LIBVIB) ../lib/$(LIBVIB)
	$(RAN) ../lib/$(LIBVIB)

$(LIBDCLAP) : $(OBJDCLAP)
	- rm -f $(LIBDCLAP)
	$(AR) cru $(LIBDCLAP) $(OBJDCLAP)
	- ln -s ../build/$(LIBDCLAP) ../lib/$(LIBDCLAP)
	$(RAN) ../lib/$(LIBDCLAP)

$(LIBDRICH) : $(OBJDRICH)
	- rm -f $(LIBDRICH)
	$(AR) cru $(LIBDRICH) $(OBJDRICH)
	- ln -s ../build/$(LIBDRICH) ../lib/$(LIBDRICH)
	$(RAN) ../lib/$(LIBDRICH)

$(LIBDNET) : $(OBJDNET)
	- rm -f $(LIBDNET)
	$(AR) cru $(LIBDNET) $(OBJDNET)
	- ln -s ../build/$(LIBDNET) ../lib/$(LIBDNET)
	$(RAN) ../lib/$(LIBDNET)

$(LIBDGOPH) : $(OBJDGOPH) $(OBJDGOPH2)
	- rm -f $(LIBDGOPH)
	$(AR) cru $(LIBDGOPH) $(OBJDGOPH) $(OBJDGOPH2)
	- ln -s ../build/$(LIBDGOPH) ../lib/$(LIBDGOPH)
	$(RAN) ../lib/$(LIBDGOPH)

$(LIBDBIO) : $(OBJDBIO) 
	- rm -f $(LIBDBIO)
	$(AR) cru $(LIBDBIO) $(OBJDBIO)
	- ln -s ../build/$(LIBDBIO) ../lib/$(LIBDBIO)
	$(RAN) ../lib/$(LIBDBIO)

$(LIBDBIO2) : $(OBJDBIO2) 
	- rm -f $(LIBDBIO2)
	$(AR) cru $(LIBDBIO2) $(OBJDBIO2)
	- ln -s ../build/$(LIBDBIO2) ../lib/$(LIBDBIO2)
	$(RAN) ../lib/$(LIBDBIO2)


# apps

MyApp : $(OBJAPP)
	$(CPP) -o MyApp $(LDFLAGS) $(OBJAPP) $(OURLIBS) $(OTHERLIBS) $(VIBLIBS) 
	$(STRIP) MyApp

MailHelp : $(OBJMAIL)
	$(CPP) -o Mailhelp $(LDFLAGS) $(OBJMAIL) $(OURLIBS) $(NETLIBS)  $(OTHERLIBS) $(VIBLIBS) 
	$(STRIP)  Mailhelp

SeqPup : $(OBJSEQPUP)
	$(CPP) -o SeqPup $(LDFLAGS) $(OBJSEQPUP) $(BIOLIBS) $(OURLIBS) $(NETLIBS) $(OTHERLIBS) $(VIBLIBS)
	$(STRIP)  SeqPup

GopherPup : $(OBJGOPHER)
	$(CPP) -o GopherPup $(LDFLAGS) $(OBJGOPHER) $(OURLIBS) $(NETLIBS) $(GOPHLIBS) $(OTHERLIBS) $(VIBLIBS)
	$(STRIP)  GopherPup

