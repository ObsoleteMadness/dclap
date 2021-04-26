/* Dversion.c */

#include <ncbi.h>
#include <Dversion.h>

const	short		kDCLAPVersion	=  6;
const	char*		kDCLAPVersionString = 
#ifdef WIN_MAC
"0.6d, Jun 96 (mac)";
#endif
#ifdef WIN_MSWIN
"0.6d, Jun 96 (mswin)";
#endif
#ifdef WIN_MOTIF
"0.6d, Jun 96 (xwin)";
#endif
