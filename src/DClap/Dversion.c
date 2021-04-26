/* Dversion.c */

#include <ncbi.h>
#include <Dversion.h>

const	short		kDCLAPVersion	=  5;
const	char*		kDCLAPVersionString = 
#ifdef WIN_MAC
"0.5b, Jan 96 (mac)";
#endif
#ifdef WIN_MSWIN
"0.5b, Jan 96 (mswin)";
#endif
#ifdef WIN_MOTIF
"0.5b, Jan 96 (xwin)";
#endif
