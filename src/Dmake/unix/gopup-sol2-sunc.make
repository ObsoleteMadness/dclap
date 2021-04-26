#	makefile for dclap C++ library routines
## only Gnu gcc/g+ has been tested...
#### this one is for Sun Solaris w/ Sun cc and CC 
#.....
# dgg note -- remember to set 
#  setenv LD_LIBRARY_PATH /usr/lib:/usr/openwin/lib
# for motif link to work !

# default flags for compiling and loading

#------------------------ dgg changed these to fit his local setup ....
#SUFFIXLCL = $(LCL)
SUFFIXLCL = sol
## -g  is for debugger info
CPFLAGS1 = -c -g -DDCLAP #  -O 
CFLAGS1 = -c -g -DDCLAP # -g  -O 
LDFLAGS1 = # -O
CC = cc
CPP = CC
RAN = echo # ranlib
AR=ar
# get rid of debug symbols
STRIP=  strip

# solaris needs /usr/ccs/lib/libgen.a for regcmp/regex weird call from Xlib
## but damn libXm isn't finding libgen funcs ???????
OTHERLIBS = # -lgen  # -lm # -L/usr/ccs/lib -lgen

OURLIBS = -lvibrant -ldclap -ldrich -lncbi #-lgen 
## SRV4 libs == Solaris libs
NETLIBS = -ldnet  -lsocket -lnsl
##NETLIBS = -ldnet  


# try static linked x libs
# note: !static == shared motif libs == runs on Solaris2 & SunOs
#       -static == static libs == fails on Solaris2 (some non-shared stdlib problem)
VIBLIBS =  -lX11 -lXm -lXt #  -lXmu
#VIBLIBS = -B static -lX11 -lXt -lXm # -lXmu 
VIBFLAG = -I/opt/IXIX11R5/include -I/opt/SUNWmotif/share/include -DWIN_MOTIF 


#------------------------- dgg


SUFFIXENV = unx
LIBNCBI = libncbi.a
LIBVIB = libvibrant.a
LIBDNET =  libdnet.a
LIBDCLAP = libdclap.a
LIBDRICH = libdrich.a

INCPATH = ../include
LIBPATH = ../lib   #-L/usr/ccs/lib 
CFLAGS = $(FLAGS) $(CFLAGS1) -I$(INCPATH) $(VIBFLAG)
CPFLAGS = $(FLAGS) $(CPFLAGS1) -I$(INCPATH) $(VIBFLAG)
LDFLAGS = $(LDFLAGS1) -I$(INCPATH) -L$(LIBPATH) $(VIBFLAG)

##
## some things to make
##

# ncbi corelib & vibrant sources needed for dclap & apps
## leave out these for vibrant apps: ncbimain.c ncbimsg.c

SRCNCBI = ncbibs.c ncbierr.c ncbienv.c ncbifile.c ncbiprop.c ncbimsg.c \
	ncbimath.c ncbimem.c ncbimisc.c ncbistr.c ncbitime.c ncbisgml.c 
	
SRCVIB =	vibbutns.c vibextra.c vibfiles.c vibgroup.c \
	viblists.c vibmenus.c vibprmpt.c \
	vibsbars.c vibslate.c vibtexts.c \
	vibutils.c vibwndws.c ncbidraw.c \
	drawing.c mapping.c picture.c viewer.c \
	document.c table.c palette.c

SRCDCLAP =  \
	DAboutBox.cc    DDialogText.cc  DObject.cc      DTaskCentral.cc DView.cc \
	DApplication.cc DList.cc        DPanel.cc       DTaskMaster.cc  DViewCentral.cc \
	DControl.cc     DMenu.cc        DTask.cc        DUtil.cc        DWindow.cc	\
	DIconLib.cc	DFile.cc	DTableView.cc	dgg.c  Dvibrant.c

SRCDRICH = \
  DDrawPICT.cc  DHTMLHandler.cc DNetHTMLHandler.cc \
  DPICTHandler.cc  DGIFHandler.cc DRTFHandler.cc DRichHandler.cc \
  DRichProcess.cc DRichViewNu.cc reader.c

SRCDNET = \
	DGopher.cc		DGoList.cc		DSendMailDialog.cc \
	DGoClasses.cc	DGoPlus.cc		DTCP.cc \
	DGoDoc.cc		DGoInit.cc		DSMTPclient.cc	\
	dnettcp.c		DGoPrefs.cc		DGoAskDoc.cc  DURL.cc


SRCAPP = MyApp.cc
SRCMAIL = MailHelp.cc
SRCGOPHER = GopherPup.cc

OBJAPP = MyApp.o
OBJMAIL = MailHelp.o
OBJGOPHER = GopherPup.o


EXEAPP =  MyApp GopherPup 

# objects needed for versions of asntool and entrez

OBJNCBI = ncbibs.o ncbierr.o ncbienv.o ncbifile.o ncbiprop.o ncbimsg.o \
	ncbimath.o ncbimem.o ncbimisc.o ncbistr.o ncbisgml.o ncbitime.o 

OBJVIB =	vibbutns.o vibextra.o vibfiles.o vibgroup.o \
	viblists.o vibmenus.o vibprmpt.o \
	vibsbars.o vibslate.o vibtexts.o \
	vibutils.o vibwndws.o ncbidraw.o \
	drawing.o mapping.o picture.o viewer.o \
	document.o table.o palette.o

OBJDCLAP = \
	DAboutBox.o    DDialogText.o  DObject.o      DTaskCentral.o DView.o \
	DApplication.o DList.o        DPanel.o       DTaskMaster.o  DViewCentral.o \
	DControl.o     DMenu.o        DTask.o        DUtil.o        DWindow.o	\
	DIconLib.o     DFile.o		DTableView.o	dgg.o   Dvibrant.o

OBJDRICH = \
  DDrawPICT.o  DHTMLHandler.o DNetHTMLHandler.o \
  DPICTHandler.o DGIFHandler.o DRTFHandler.o DRichHandler.o \
  DRichProcess.o DRichViewNu.o reader.o


OBJDNET = \
	DGopher.o		DGoList.o		DSendMailDialog.o \
	DGoClasses.o	DGoPlus.o		DTCP.o \
	DGoDoc.o		DGoInit.o		DSMTPclient.o \
	dnettcp.o		DGoPrefs.o		DGoAskDoc.o  DURL.o



## All things to make
##
all : nocopy 

nocopy : sources  $(LIBNCBI) $(LIBVIB) $(LIBDRICH) $(LIBDCLAP) $(LIBDNET) $(LIBDBIO) $(EXEAPP)
#nocopy : sources  $(LIBNCBI) $(LIBVIB) $(LIBDCLAP) 

sources : $(SRCNCBI) $(SRCVIB) $(SRCDRICH) $(SRCDCLAP) $(SRCDNET) $(SRCDBIO) $(SRCAPP) $(SRCMAIL) $(SRCGOPHER)
#sources : $(SRCNCBI) $(SRCVIB) $(SRCDCLAP)

## To clean out the directory without removing make
##
clean :
	- rm -f *.[acho]

## Implicit actions
##

.c.o :
	$(CC) $(CFLAGS) $<
	
# damn, I use .cp for C++ suffix, gcc wants .cc
.cc.o :
	$(CPP) $(CPFLAGS) $<

## get all the source files
### copy :
### see makecopy -- taken out of this makefile for safety


## make libraries
##

$(LIBNCBI) : $(OBJNCBI)
	- rm -f $(LIBNCBI)
	$(AR) cru $(LIBNCBI) $(OBJNCBI)
	- ln -s $(LIBNCBI) ../lib
	$(RAN) ../lib/$(LIBNCBI)

$(LIBVIB) : $(OBJVIB)
	- rm -f $(LIBVIB)
	$(AR) cru $(LIBVIB) $(OBJVIB)
	- ln -s $(LIBVIB) ../lib
	$(RAN) ../lib/$(LIBVIB)

$(LIBDCLAP) : $(OBJDCLAP)
	- rm -f $(LIBDCLAP)
	$(AR) cru $(LIBDCLAP) $(OBJDCLAP)
	- ln -s $(LIBDCLAP) ../lib
	$(RAN) ../lib/$(LIBDCLAP)

$(LIBDRICH) : $(OBJDRICH)
	- rm -f $(LIBDRICH)
	$(AR) cru $(LIBDRICH) $(OBJDRICH)
	- ln -s $(LIBDRICH) ../lib
	$(RAN) ../lib/$(LIBDRICH)

$(LIBDNET) : $(OBJDNET)
	- rm -f $(LIBDNET)
	$(AR) cru $(LIBDNET) $(OBJDNET)
	- ln -s $(LIBDNET) ../lib
	$(RAN) ../lib/$(LIBDNET)


# MyApp

MyApp : $(OBJAPP)
	$(CPP) -o MyApp $(LDFLAGS) $(OBJAPP) $(OURLIBS) $(NETLIBS) $(OTHERLIBS) $(VIBLIBS) 
	$(STRIP) MyApp

MailHelp : $(OBJMAIL)
	$(CPP) -o Mailhelp $(LDFLAGS) $(OBJMAIL) $(OURLIBS) $(NETLIBS)  $(OTHERLIBS) $(VIBLIBS) 
	$(STRIP)  Mailhelp

GopherPup : $(OBJGOPHER)
	$(CPP) -o GopherPup $(LDFLAGS) $(OBJGOPHER) $(OURLIBS) $(NETLIBS) $(OTHERLIBS) $(VIBLIBS)
	$(STRIP)  GopherPup

