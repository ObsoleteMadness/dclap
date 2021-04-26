//	============================================================================
//	Fileformat.C													80 columns
//	Reece Hart	(reece@ibc.wustl.edu)								tab=4 spaces
//	Washington University School of Medicine, St. Louis, Missouri
//	This source is hereby released to the public domain.  Bug reports, code
//	contributions, and suggestions are appreciated (to email address above).
//	-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -
//	Please see FileFormat.H for a description of this source and
//	acknowledgements.
//
//	I've used C-style file i/o for ease of translation from ted, and because
//	some of the operations here are more easily done with stdio calls than with
//	stream routines.
//	========================================|===================================

#include	<stddef.h>
#include	<stdio.h>
#include	<string.h>
#include	"FileFormat.H"
#define _H_STORAGE  // dgg
#include	"RInclude.H"

format_t
FileFormat(const char* filename)
	{
	const uint NUM_MAGICS = 4;				// number of magics we know about
	const uint MAX_COOKIE = 5;				// max length of a magic cookie
	const struct							// The magics[] is a list of magic
		{									// cookies and their file offsets
		format_t type;						// The scope of magics[] is limited
		size_t	offset;						// to this function to preserve
		char*	string;						// global namespace.
		}
		magics[] =
		{
			{ ABI , 0,   "ABIF" } ,
			{ ALF , 518, "ALF " } ,
			{ SCF , 0,   ".scf" } ,
			{ SCF , 0,   "\234\330\300\000" }	// Amersham variant
		};


	FILE*	fp;
	char	buf[MAX_COOKIE];
	uint	i;
	size_t	len;

	fp = fopen(filename, "rb");
	if (!fp)
		return FFerr;

	for (i = 0 ; i < NUM_MAGICS ; i++)
		{
		len = strlen(magics[i].string);

		if ( (fseek(fp,magics[i].offset,0) != 0) ||
			 (fread(buf,len,sizeof(char),fp) != 1) )
			{
			fclose(fp);
			return unknown;
			}
		else
			if (strncmp(buf,magics[i].string,len)==0)
				// magic cookie found
				{
				fclose(fp);
				return magics[i].type;
				}
		}

	// we've not found our magic cookie... this file format is unknown
	fclose(fp);
	return unknown;
	}
