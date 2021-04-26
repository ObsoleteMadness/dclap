//	============================================================================
//	FileFormat.H													80 columns
//	Reece Hart	(reece@ibc.wustl.edu)								tab=4 spaces
//	Washington University School of Medicine, St. Louis, Missouri
//	This source is hereby released to the public domain.  Bug reports, code
//	contributions, and suggestions are appreciated (to email address above).
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	This code defines tracefile format types, user-interface descriptors for
//	each format, and declares a function (defined in FileFormat.C) which
//	attempts to guess the format of a named file.
//
//	This code is a complete rewrite of an analogous function for ted
//	("Trace EDitor") by Roger Staden et al.
//	========================================|===================================

#ifndef _H_FileFormat						// include this file only once
#define _H_FileFormat

//
// As file types are added, append the internal abbrevation to the list of
// enums, and append a corresponding <abbrev,descr> pair to the array of
// TRACEFILE_FORMATS.
// Code for reading and/or writing the format should be placed in CTraceFile::
// Read(filename,fmt), Write(filename,fmt), Read(FILE*,fmt) and Write(FILE*,fmt)
//
enum	format_t
		{
		ABI0,								// first trace set (raw data)
		ABI1,								// second trace set (mobility?)
		ABI,								// third trace set (processed data)
		ALF,
		SCF,

		// insert new internal file type id's just above this line
		// notice that unknown = the number of format types
		unknown,
		FFerr
		};

static const MAX_ABBR_LENGTH = 5;
static const MAX_DESC_LENGTH = 50;
static const struct
	{
	char	abbr[MAX_ABBR_LENGTH];
	char	desc[MAX_DESC_LENGTH];
	}
	TRACEFILE_FORMATS[] =
	{
		{ "ABI0",	"ABI (raw data)" },
		{ "ABI1",	"ABI (2nd trace set)" },
		{ "ABI",	"ABI (processed data)" },
		{ "ALF",	"ALF" },
		{ "SCF",	"Standard Chromatogram Format" },

		// insert new <abbr,desc> pairs just above this line
		{ "unk",	"unknown/unspecified" },
		{ "err",	"error" }
	};

	
// FileFormatAbbr
// return a string which gives the abbreviation of the format
inline
const char*
FileFormatAbbr(format_t fmt)
	{
	return (char*)&TRACEFILE_FORMATS[fmt].abbr;
	}

// FileFormatDesc
// return a string which gives the name of the format
inline
const char*
FileFormatDesc(format_t fmt)
	{
	return (char*)&TRACEFILE_FORMATS[fmt].desc;
	}

format_t	FileFormat(const char* filename);	// Guess format of filename
												// see FileFormat.C

#endif										// conditional inclusion
