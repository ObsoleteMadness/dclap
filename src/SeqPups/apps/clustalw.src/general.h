/* General purpose header file - rf 12/90 */

#ifndef _H_general
#define _H_general



/* Macintosh specific */
#ifdef THINK_C

#define const					/* THINK C doesn't know about these identifiers */
#define signed
#define volatile
#define int long
#define Boolean int
#define pint short			/* cast ints in printf statements as pint */

#elif defined(__MWERKS__)

#define pint short			/* cast ints in printf statements as pint */

#else 							/* not Macintoshs */

#define pint int			/* cast ints in printf statements as pint */
typedef int Boolean;			/* Is already defined in THINK_C */

#endif 							/* ifdef THINK_C */

/* definitions for all machines */

#undef TRUE						/* Boolean values; first undef them, just in case */
#undef FALSE
#define TRUE 1
#define FALSE 0

#define EOS '\0'				/* End-Of-String */
#define MAXLINE 512			/* Max. line length */


#ifdef VMS
#define signed
#endif


#endif /* ifndef _H_general */
