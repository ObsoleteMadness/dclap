//	============================================================================
//	DNA.H															80 columns
//	Reece Hart	(reece@ibc.wustl.edu)								tab=4 spaces
//	Washington University School of Medicine, St. Louis, Missouri
//	This source is hereby released to the public domain.  Bug reports, code
//	contributions, and suggestions are appreciated (to email address above).
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	This file contains some simple declarations for DNA.
//	========================================|===================================

#ifndef _H_DNA								// include this file only once
#define _H_DNA

#undef T

enum	base_t
	{ A, C, G, T, X };

static const char DNA_BASES[] = "ACGTX";
static const uint NUM_BASES = 4;

#endif										// conditional inclusion
