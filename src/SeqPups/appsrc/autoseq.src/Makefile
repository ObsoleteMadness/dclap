#
# Makefile
# Reece Hart, reece@ibc.wustl.edu
# for sources developed for CS project
# 94.05.29
#

#
# NOTE:
# As of 93.11.18, this source will not compile under g++.  I haven't checked
# into this thoroughly, but the problems seem to be related to namespace clashes
# and known troubles with g++ handling of template classes. (see c++ newsgroups)
# 
CC=CC
cc=cc
CFLAGS=-O
LIBFLAGS=-lm
CCI=/opt/SUNWspro/SC3.0.1/include/CC
INSTDIR=$(HOME)/bin
MANSFX=1
MANDIR=$(HOME)/man/man${MANSFX}
SRCDIR=$(PWD)

SHELL=/bin/sh

all: autoseq crop

fromscratch: cleanest all

crop: crop.c
	$(cc) -O -o $@ crop.c

autoseq: FileFormat.o CSequence.o CPeakList.o CTrace.o CTraceFile.o autoseq.o
	### Building $@ from source in $(SRCDIR)/
	$(CC) $(LIBFLAGS) $(CFLAGS) -o $@ \
		FileFormat.o CSequence.o CPeakList.o CTrace.o\
		CTraceFile.o autoseq.o

xlate: FileFormat.o CSequence.o CPeakList.o CTrace.o CTraceFile.o xlate.o
	### Building $@ from source in $(SRCDIR)/
	$(CC) $(LIBFLAGS) $(CFLAGS) -o $@ \
		FileFormat.o CSequence.o CPeakList.o CTrace.o\
		CTraceFile.o xlate.o

install: autoseq xlate crop
	### Installing executable links in $(INSTDIR)/
	###   and man page link in $(MANDIR)/
	ln -fs $(SRCDIR)/autoseq $(INSTDIR)/autoseq
	ln -fs $(SRCDIR)/xlate $(INSTDIR)/xlate
	ln -fs $(SRCDIR)/crop $(INSTDIR)/crop
	ln -fs $(SRCDIR)/scripts/analyze $(INSTDIR)/analyze
	ln -fs $(SRCDIR)/scripts/analall $(INSTDIR)/analall
	ln -fs $(SRCDIR)/scripts/bpdagg  $(INSTDIR)/bpdagg
	ln -fs $(SRCDIR)/scripts/FastaToTable  $(INSTDIR)/FastaToTable
	ln -fs $(SRCDIR)/scripts/fastatbl  $(INSTDIR)/fastatbl
	ln -fs $(SRCDIR)/autoseq.man $(MANDIR)/autoseq.${MANSFX}

unlink:
	### Removing executable links for this package in $(INSTDIR)/
	###   and man page link in $(MANDIR)/
	rm -f $(INSTDIR)/autoseq
	rm -f $(INSTDIR)/xlate
	rm -f $(INSTDIR)/crop
	rm -f $(INSTDIR)/analyze
	rm -f $(INSTDIR)/analall
	rm -f $(INSTDIR)/bpdagg
	rm -f $(INSTDIR)/FastaToTable
	rm -f $(INSTDIR)/fastatbl
	rm -f $(MANDIR)/autoseq.${MANSFX}

depend:
	makedepend -I$(CCI) *.[cC]
	\rm -f Makefile.bak
	
# inference rule for .C --> .o, %.H --> .o
%.o: %.C
	$(CC) $(CFLAGS) -c $<
%.o: %.H
	$(CC) $(CFLAGS) -c $<

# clean: removes stuff that's truly junk
clean:
	### Removing junk from $(SRCDIR)/
	\rm -f core a.out *~ *.bak
	\rm -f scripts/core scripts/a.out scripts/*~ scripts/*.bak

#cleanest: removes everything which can be reconstructed
cleanest: clean unlink
	### Removing reconstructable targets and by-products from $(SRCDIR)/
	\rm -f autoseq xlate crop
	\rm -f *.o
	\rm -rf ptrepository 

# DO NOT DELETE THIS LINE -- make depend depends on it.

CPeakList.o: CPeakList.H /opt/SUNWspro/SC3.0.1/include/CC/iostream.h
CPeakList.o: /usr/include/memory.h /opt/SUNWspro/SC3.0.1/include/CC/iomanip.h
CPeakList.o: /opt/SUNWspro/SC3.0.1/include/CC/generic.h CSequence.H
CPeakList.o: /opt/SUNWspro/SC3.0.1/include/CC/fstream.h RInclude.H DNA.H
CPeakList.o: Definitions.H CTrace.H /usr/include/stdio.h /usr/include/math.h
CPeakList.o: RInlines.H /usr/include/stddef.h /usr/include/stdlib.h
CSequence.o: CSequence.H /opt/SUNWspro/SC3.0.1/include/CC/iostream.h
CSequence.o: /usr/include/memory.h /opt/SUNWspro/SC3.0.1/include/CC/fstream.h
CSequence.o: RInclude.H
CTrace.o: CTrace.H /usr/include/stdio.h
CTrace.o: /opt/SUNWspro/SC3.0.1/include/CC/iostream.h /usr/include/memory.h
CTrace.o: /opt/SUNWspro/SC3.0.1/include/CC/fstream.h /usr/include/math.h
CTrace.o: CSequence.H RInclude.H CPeakList.H
CTrace.o: /opt/SUNWspro/SC3.0.1/include/CC/iomanip.h
CTrace.o: /opt/SUNWspro/SC3.0.1/include/CC/generic.h DNA.H Definitions.H
CTrace.o: RInlines.H /usr/include/stddef.h /usr/include/stdlib.h
CTraceFile.o: CTraceFile.H /usr/include/stddef.h CTrace.H
CTraceFile.o: /usr/include/stdio.h
CTraceFile.o: /opt/SUNWspro/SC3.0.1/include/CC/iostream.h
CTraceFile.o: /usr/include/memory.h
CTraceFile.o: /opt/SUNWspro/SC3.0.1/include/CC/fstream.h /usr/include/math.h
CTraceFile.o: CSequence.H RInclude.H CPeakList.H
CTraceFile.o: /opt/SUNWspro/SC3.0.1/include/CC/iomanip.h
CTraceFile.o: /opt/SUNWspro/SC3.0.1/include/CC/generic.h DNA.H Definitions.H
CTraceFile.o: RInlines.H FileFormat.H /usr/include/string.h
CTraceFile.o: /usr/include/ctype.h /usr/include/stdlib.h /usr/include/time.h
FileFormat.o: /usr/include/stddef.h /usr/include/stdio.h
FileFormat.o: /usr/include/string.h FileFormat.H RInclude.H
crop.o: /usr/include/stdio.h
autoseq.o: CTraceFile.H /usr/include/stddef.h CTrace.H /usr/include/stdio.h
autoseq.o: /opt/SUNWspro/SC3.0.1/include/CC/iostream.h /usr/include/memory.h
autoseq.o: /opt/SUNWspro/SC3.0.1/include/CC/fstream.h /usr/include/math.h
autoseq.o: CSequence.H RInclude.H CPeakList.H
autoseq.o: /opt/SUNWspro/SC3.0.1/include/CC/iomanip.h
autoseq.o: /opt/SUNWspro/SC3.0.1/include/CC/generic.h DNA.H Definitions.H
autoseq.o: RInlines.H FileFormat.H /usr/include/stdlib.h
autoseq.o: /usr/include/string.h
xlate.o: CTraceFile.H /usr/include/stddef.h CTrace.H /usr/include/stdio.h
xlate.o: /opt/SUNWspro/SC3.0.1/include/CC/iostream.h /usr/include/memory.h
xlate.o: /opt/SUNWspro/SC3.0.1/include/CC/fstream.h /usr/include/math.h
xlate.o: CSequence.H RInclude.H CPeakList.H
xlate.o: /opt/SUNWspro/SC3.0.1/include/CC/iomanip.h
xlate.o: /opt/SUNWspro/SC3.0.1/include/CC/generic.h DNA.H Definitions.H
xlate.o: RInlines.H FileFormat.H /usr/include/stdlib.h /usr/include/string.h
