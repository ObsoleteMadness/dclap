//	============================================================================
//	RInclude.H														80 spaces
//	Reece Hart, reece@ibc.wustl.edu									tab=4 spaces
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	This file contains homegrown definitions, typedefs, etc. which are used in
//	many of my sources.  Note that it is C++ specific.
//	========================================|===================================

#ifndef _H_RInclude							// include this file only once
#define _H_RInclude

#undef WIN_MAC  // dgg temp fix

#ifndef TRUE
#define	FALSE		0
#define	TRUE		!FALSE
#endif

#undef	NULL
#ifndef	NULL
#	define NULL		0						// ++ def'n of NULL
#endif

typedef int				enum_t;				// clarify that enum val is expected
typedef	int				bool;
typedef unsigned long	ulong;
typedef unsigned int	uint;
typedef unsigned short	ushort;
typedef unsigned char	uchar;
typedef unsigned char	byte;

typedef char			vrsn[10];			// version strings

#ifdef _H_STORAGE
// dgg -- added _H_STORAGE flag to avoid multi storage of same symbols
const ulong BYTE[] =						// BYTE[i] is a byte mask for byte i
	{ 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 };

const byte BIT[] =							// BIT[i] is a bit mask for bit i
	{
	0x01, 0x01<<1, 0x01<<2, 0x01<<3, 0x01<<4, 0x01<<5, 0x01<<6, 0x01<<7	};
#else
extern const ulong BYTE[];
extern const byte BIT[];
#endif

#endif										// conditional inclusion
