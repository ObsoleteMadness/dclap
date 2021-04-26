$! compile and link clustalw on vax or alpha machines (VMS)
$ ver = f$verify(1)
$
$ cc CLUSTALW.c 
$ cc ALNSCORE.c
$ cc AMENU.c
$ cc CALCGAPCOEFF.c
$ cc CALCPRF1.c
$ cc CALCPRF2.c
$ cc CALCTREE.c
$ cc GCGCHECK.c
$ cc MALIGN.c
$ cc PAIRALIGN.c
$ cc PRFALIGN.c
$ cc RANDOM.c
$ cc READMAT.c
$ cc SEQUENCE.c
$ cc SHOWPAIR.c
$ cc TREES.c
$ cc UTIL.c
$
$ if f$getsyi("arch_name").eqs."Alpha" 
$ Then
$     link clustalw, alnscore, amenu, calcgapcoeff, calcprf1, calcprf2, -
                     CALCTREE,GCGCHECK,MALIGN,PAIRALIGN,PRFALIGN,RANDOM, -
                     READMAT,SEQUENCE,SHOWPAIR,TREES,UTIL
$ else
$     link clustalw,alnscore, amenu, calcgapcoeff, calcprf1, calcprf2, -
                    CALCTREE,GCGCHECK,MALIGN,PAIRALIGN,PRFALIGN,RANDOM, -
                    READMAT,SEQUENCE,SHOWPAIR,TREES,UTIL, sys$input:/opt
        Sys$share:vaxcrtl/share
$ endif
$
$ ver = f$verify(0)
$
$ clustalw_dir == f$environment("default")
$ write sys$output "-----------------------------------------------------------"
$ write sys$output " "
$ write sys$output "The help file (clustalw.hlp) should be DEFINED as clustalw_help "
$ write sys$output " "
$ write sys$output "e.g. $ define clustalw_help ",clustalw_dir,"clustalw.hlp "
$ write sys$output " "
$ write sys$output "The executable file (clustalw.exe) should be run as a foreign command "
$ write sys$output " "
$ write sys$output "e.g. $ clustalw :== $",clustalw_dir,"clustalw "
$ write sys$output " "
$ write sys$output "-----------------------------------------------------------"
