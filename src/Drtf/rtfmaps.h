/* rtfmaps.h 
   d.g.gilbert
   converted run-time disk file charset maps to compile-time include files
   in rtf2....
*/

#ifndef _RTFMAPS_
#define _RTFMAPS_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef rtfmapInternal

extern short mac_gen_CharCode[];
extern short mac_sym_CharCode[];
extern short ansi_gen_CharCode[];
extern short ansi_sym_CharCode[];
extern short pc_gen_CharCode[];
extern short pc_sym_CharCode[];
extern short pca_gen_CharCode[];
extern short pca_sym_CharCode[];

short  RTFStdCharCode(char	*name);
char * RTFStdCharName(short code);


#else

#include "rtfchars.h"

static char	*stdCharName[] = {
# include	"stdcharnames.h"
	 NULL
};


short mac_gen_CharCode[] = {
# include "mac-gen.h"
};

short mac_sym_CharCode[] = {
# include "mac-sym.h"
};

short ansi_gen_CharCode[] = {
# include "ansi-gen.h"
};

short ansi_sym_CharCode[] = {
# include "ansi-sym.h"
};

short pc_gen_CharCode[] = {
# include "pc-gen.h"
};

short *pc_sym_CharCode = ansi_sym_CharCode;

short pca_gen_CharCode[] = {
# include "pca-gen.h"
};

short *pca_sym_CharCode = ansi_sym_CharCode;


char * RTFStdCharName(short code)
{
	if (code < 0 || code >= rtfSC_MaxChar)
		return (NULL);
	return (stdCharName[code]);
}

short RTFStdCharCode (char	*name)
{
	short	i;

	for (i = 0; i < rtfSC_MaxChar; i++) {
		if (strcmp (name, stdCharName[i]) == 0)
			return (i);
		}
	return (-1);
}


#endif

#ifdef __cplusplus
}
#endif

#endif
