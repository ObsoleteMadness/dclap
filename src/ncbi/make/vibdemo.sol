#	UNIX shell file to source to make Vibrant version of demos using
#   gcc compiler on suns.  Just modify this according to the header info
#   in makeall.unx for other systems.
#	
#	Sun with Gnu C [ make CC=gcc ]
#
#	To Make the Motif version of Vibrant libraries and add Vibrant front
#   end to asntool, add:
#      VIBLIBS="-lXm -lXmu -lXt -lX11" LIB4=libvibrant.a VIBFLAG="-I/am/Motif/include -DWIN_MOTIF"
#   to the argument list.  Substitute the path to your Motif includes for
#    /am/Motif/include.
#	
#   Set the LD_LIBRARY_PATH environment variable (with setenv) to:
#
#       /netopt/Motif1.2.2/lib:/netopt/X11R5/lib:$LD_LIBRARY_PATH
#

make -f makedemo.unx CC="cc -Xa" VIBLIBS="-R/netopt/SUNWmotif/lib -lXm -lXmu -lXt -lX11 -lsocket -lnsl -lgen" LIB4=-lvibrant VIBFLAG="-I/usr/openwin/include -I/netopt/SUNWmotif/include -L/usr/openwin/lib -L/netopt/SUNWmotif/lib -DWIN_MOTIF" VIB=entrez
