/* prtutil.h
* ===========================================================================
*
*                            PUBLIC DOMAIN NOTICE                          
*               National Center for Biotechnology Information
*                                                                          
*  This software/database is a "United States Government Work" under the   
*  terms of the United States Copyright Act.  It was written as part of    
*  the author's official duties as a United States Government employee and 
*  thus cannot be copyrighted.  This software/database is freely available 
*  to the public for use. The National Library of Medicine and the U.S.    
*  Government have not placed any restriction on its use or reproduction.  
*                                                                          
*  Although all reasonable efforts have been taken to ensure the accuracy  
*  and reliability of the software and data, the NLM and the U.S.          
*  Government do not and cannot warrant the performance or results that    
*  may be obtained by using this software or data. The NLM and the U.S.    
*  Government disclaim all warranties, express or implied, including       
*  warranties of performance, merchantability or fitness for any particular
*  purpose.                                                                
*                                                                          
*  Please cite the author in any work or product based on this material.   
*
* ===========================================================================
*
* File Name:  prtutil.h
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/93
*
* $Revision: 1.1 $
*
* File Description: Object Print Utilities
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/
#ifndef _NCBI_PrtUtil_
#define _NCBI_PrtUtil_

#ifndef _NCBI_ObjPrt_
#include <objprt.h>		   /* the object loader interface */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   Print Templates
*
*****************************************************************************/
PrintTemplatePtr PrintTemplateFind PROTO((CharPtr name));
Boolean PrintTemplateSetLoad PROTO(( CharPtr path ));


/*****************************************************************************
*
*   Print Stacks
*
*****************************************************************************/
#define PRINT_STACK_SIZE 10
#define OP_NCBIPRT -10    /* should go to asn.h */
#define CTX_OBJPRT 100    /* should go in ncbierr.h */

typedef struct printformatitem {
	Int2 sort_key,
		indent_level;
	PrintFormatPtr format;   /* the format */
	PrintTemplatePtr template; /* the format used in use_template */
	AsnTypePtr PNTR types;   /* chain of types for this path */
	Int2 numtypes;           /* number of types (above) */
	AsnTypePtr atp;          /* resolved node for format->asn1 */
	Int2 isa;                /* return from AsnFindBaseIsa(atp) */
} PrintFormatItem, PNTR PrintFormatItemPtr;

typedef struct printformatlist {
	PrintTemplatePtr ptp;        /* the template used */
	Int2 numitem;
	PrintFormatItemPtr list;     /* array of PrintFormatItem */
} PrintFormatList, PNTR PrintFormatListPtr;

typedef struct printstackitem {
	PrintFormatItemPtr pfip;
	DataVal dv;
} PrintStackItem, PNTR PrintStackItemPtr;

typedef struct printstack {
	PrintFormatListPtr pflp;         /* the format list in use */
	CharPtr labelfrom;
	Int2 pstacksize,              /* size of pstack */
		pstackpos;                /* next available cell in pstack */
	PrintStackItemPtr data;
	Int2 size,                     /* total size, including branches */
		indent;                   /* PrintFormatItem.indent_level of parent */
	Boolean is_branch;            /* true if is "use_template" branch */
} PrintStack, PNTR PrintStackPtr;

typedef struct pstacklistitem {
	Int2 type_indent;            /* asn.1 indent level when branch added */
	PrintStackPtr psp;           /* the printstack */
} PrintStackListItem, PNTR PrintStackListItemPtr;

typedef struct pstacklist {
	Int2 size,                   /* total size of pstip */
		used;                    /* number of elements used */
	PrintStackListItemPtr pstip;      /* the array of items */
} PrintStackList, PNTR PrintStackListPtr;

typedef struct stdprintoptions {   /* options for StdFormatPrint() */
	FILE * fp;                     /* for output to file */
	Int2 linelength;               /* max line length */
	CharPtr ptr;                   /* returned string, if fp==NULL */
	CharPtr indent ,               /* string to use at each indent level */
	        newline ,              /* string to use for '\n' */
			tab;                   /* string to use for '\t' */
	ByteStorePtr bstp;             /* used internally */	
} StdPrintOptions, PNTR StdPrintOptionsPtr;

PrintStackPtr PrintStackBuild PROTO((Pointer data, AsnWriteFunc func, PrintFormatListPtr pflp));
PrintStackItemPtr PrintStackItemNew PROTO((PrintStackPtr psp));
PrintStackPtr PrintStackFree PROTO((PrintStackPtr psp));
PrintStackItemPtr PrintStackItemGet PROTO((PrintStackPtr psp, Int2 index, Int2Ptr indent_offset));
Int2 PrintStackGetSize PROTO((PrintStackPtr psp));
Boolean PrintStackSort PROTO((PrintStackPtr psp));
void PrintStackDump PROTO((PrintStackPtr psp, FILE * fp, Int2 indent));

PrintFormatListPtr PrintFormatListNew PROTO((void));
PrintFormatListPtr PrintFormatListBuild PROTO((PrintTemplatePtr ptp));
PrintFormatListPtr PrintFormatListFree PROTO((PrintFormatListPtr pflp));
void PrintFormatListFreeAll PROTO((Boolean templates));
PrintFormatListPtr PrintFormatListFind PROTO((CharPtr name));
PrintFormatListPtr PrintFormatListGet PROTO((CharPtr templatename));

Int2 PrintFormatTraverse PROTO((PrintFormatPtr pfp, PrintFormatListPtr pflp, Int2 index, Int2 indent));

void PrintStackPrint PROTO((PrintStackPtr psp, StdPrintOptionsPtr spop));

CharPtr StdSeqLocPrint PROTO((DataValPtr dvp));
CharPtr StdDatePrint PROTO((DataValPtr dvp));
CharPtr StdAuthListPrint PROTO((DataValPtr dvp));

Boolean StdFormatPrint PROTO((Pointer data, AsnWriteFunc func, CharPtr templatename, StdPrintOptionsPtr options));

StdPrintOptionsPtr StdPrintOptionsNew PROTO((FILE * fp));
StdPrintOptionsPtr StdPrintOptionsFree PROTO((StdPrintOptionsPtr spop));


/*****************************************************************************
*
*   These functions are in testprt.c, but will be moved
*
*****************************************************************************/

CharPtr StdSeqLocPrint PROTO((DataValPtr dvp));
CharPtr StdDatePrint PROTO((DataValPtr dvp));
CharPtr StdAuthListNamesPrint PROTO((DataValPtr dvp));

#ifdef __cplusplus
}
#endif

#endif

