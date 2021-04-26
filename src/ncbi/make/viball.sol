#	UNIX shell file to source to make Vibrant version of tools using
#   gcc compiler on suns.  Just modify this according to the header info
#   in makeall.unx for other systems.
#	
#	Sun with Gnu C [ make LCL=gcc RAN=ranlib CC=gcc ]
#
#	To Make the Motif version of Vibrant libraries and add Vibrant front
#   end to asntool, add:
#      VIBLIBS="-lXm -lXmu -lXt -lX11" LIB4=libvibrant.a VIBFLAG="-I/am/Motif/include -DWIN_MOTIF"
#   to the argument list.  Substitute the path to your Motif includes for
#    /am/Motif/include.
#	

make LCL=sol CC="cc -Xa"  VIBLIBS="-R/netopt/SUNWmotif/lib -lXm -lXmu -lXt -lX11" LIB4=libvibrant.a VIBFLAG="-I/usr/openwin/include -I/netopt/SUNWmotif/include -L/usr/openwin/lib -L/netopt/SUNWmotif/lib -DWIN_MOTIF"

