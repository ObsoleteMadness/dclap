/* asn.h
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
* File Name: asn.h
*
* Author:  James Ostell
*
* Version Creation Date: 1/1/91
*
* $Revision: 2.17 $
*
* File Description:
*   This header the interface to all the routines in the ASN.1 libraries
*     that an application should ever use.  It also includes the necessary
*     typedefs -- however the application programmer is not meant to use
*     the internal structures directly outside of the specified functions,
*     as the internal structures may be changed without notice.
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 04-20-93 Schuler     LIBCALL calling convention
* 05-13-93 Schuler     Changed definitions of AsnReadFunc and AsnWriteFunc to 
*                      use the LIBCALL calling convention.
* 02-24-94 Schuler     AsnTypeStringToHex moved here (from asntypes.h)
*
* ==========================================================================
*/

#ifndef _ASNTOOL_
#define _ASNTOOL_
                      /*** depends on NCBI core routines ***/
#ifndef _NCBI_
#include <ncbi.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

   /**** ValNode is used for internal representation of values from
	****  CHOICE, SET OF, SEQ OF and combinations for many cases.
	****  it is provided in ncbimisc for build object routines ****/

/***  The following defines can be used for backward compatibility
#define AsnValue DataVal
#define AsnNode ValNode
***/
/***  In addition, AsnValueNode was changed to AsnValxNode so it would
      not conflict with the AsnValue define above
****/

#ifndef START_STRUCT
#define START_STRUCT 	411		/* { found */
#define END_STRUCT 		412		/* } found */
#endif

typedef struct asnvaluenode {
	Int2 valueisa;
	CharPtr name;		/* use for strings and named int */
	Int4 intvalue;		    /* use for int and boolean */
	FloatHi realvalue;
	struct asnvaluenode PNTR next;
}	AsnValxNode, PNTR AsnValxNodePtr;

   /******* AsnOptions allow customization of AsnIo and AsnType ****/
   
typedef Pointer (LIBCALLBACK * AsnOptFreeFunc) PROTO ((Pointer));
#define DefAsnOptionFree  Nlm_MemFree	/* default function for freeing AsnOption's */

typedef struct asnopt {
	Int2 ao_class;               /* class of option. all negative numbers res.*/
	Int2 type;                /* type within ao_class */
	DataVal data;            /* data used for setting option */
	AsnOptFreeFunc freefunc;  /* function to free data.ptrvalue */
	struct asnopt PNTR next;
} AsnOption, PNTR AsnOptionPtr;

   /******** AsnType is a node in the AsnTool parse tree *******/

typedef struct asntype {
	Int2 isa;
	CharPtr name;
	Uint1 tagclass;
	Int2 tagnumber;
	unsigned implicit   : 1;
	unsigned optional   : 1;
	unsigned hasdefault : 1;
	unsigned exported   : 1;
	unsigned imported   : 1;
	unsigned resolved   : 1;
	AsnValxNodePtr defaultvalue;		 /* used for default value, range, subtypes */
	struct asntype PNTR type;
	Pointer branch;				 /* used for named ints, enum, set, sequence */
	Int2 tmp;     /* for temporary ->type link to local tree */
	struct asntype PNTR next;
	AsnOptionPtr hints;         /* used to customize the type by application */
}	AsnType, PNTR AsnTypePtr;

typedef struct asnmodule {
	CharPtr modulename;
	CharPtr filename;           /* if module to be loaded from disk */
	AsnTypePtr types;
	AsnTypePtr values;
	struct asnmodule PNTR next;    /* for chain of modules */
	Int2 lasttype;          /* for isa defined types */
	Int2 lastvalue; 		/* for isa defined values */
}	AsnModule, PNTR AsnModulePtr;

#define ASNIO_BUFSIZE	1024    /* default size of AsnIo.buf */
                                /* AsnIo.type  bit[0] = text? bit[1]=binary?*/
                                /* bit[2]=input? bit[3]=output?           */
#define ASNIO_TEXT  1
#define ASNIO_BIN   2
#define ASNIO_IN    4
#define ASNIO_OUT   8
#define ASNIO_FILE  16
#define ASNIO_CARRIER   32     /* is a pure iterator */

#define ASNIO_TEXT_IN	21     /* AsnIo.type */
#define ASNIO_TEXT_OUT	25
#define ASNIO_BIN_IN	22
#define ASNIO_BIN_OUT	26

typedef struct pstack {
    AsnTypePtr type;           /* type at this level of stack */
    Int4 len;                  /* length of item for binary decode */
    Boolean resolved;          /* resolution of type for binary decode */
	Boolean tag_indef;                /* indefinate tag length on input? */
} Pstack, PNTR PstackPtr;

typedef struct asnexpoptstruct {
	struct asnio PNTR aip;
	AsnTypePtr atp;
	DataValPtr dvp;
	Int2 the_choice;
	Pointer the_struct;
	Pointer data;
} AsnExpOptStruct, PNTR AsnExpOptStructPtr;

typedef void (LIBCALLBACK * AsnExpOptFunc) PROTO ((AsnExpOptStructPtr));
#define NO_CHOICE_SET INT2_MIN     /* for AsnExpOptStruct.the_choice  */

typedef struct expopt {
	Int2 numtypes;
	AsnTypePtr PNTR types;             /* the type to check */
	Pointer user_data;           /* user supplied data */
	AsnExpOptFunc user_callback; /* user supplied callback function */
	struct expopt PNTR next;
} AsnExpOpt, PNTR AsnExpOptPtr;

typedef void (LIBCALLBACK * AsnErrorFunc) PROTO((Int2, CharPtr));
#define ErrorRetType AsnErrorFunc	/* for backward compatibility */
typedef Int2 (LIBCALLBACK * AsnIoFunc) PROTO((Pointer, CharPtr, Uint2));
#define IoFuncType AsnIoFunc		/* for backward compatibility */


typedef struct asnio {
	CharPtr linebuf;
	Int1 type;            /* type- text-in, text-out, bin-in, bin-out */
	Int2 linepos;         /* current offset in linebuf */
	FILE * fp;             /* file to write or read to */
	BytePtr buf;          /* buffer for I/O */
    Int2 bufsize;         /* sizeof this buffer */
	Int2 bytes,           /* bytes of data available in buf */
		offset;           /* current offset of processing in buf */
	Uint1 tagclass;       /* last BER tag-id-len read */
	Int2 tagnumber;
	Boolean constructed;
	Int4 length;          /* length of BER encoded data */
	Boolean tagsaved;     /* TRUE if tag info already here - stops read */
	Int4 used;            /* if tagsaved, bytes used recorded here */
	Int1 tabsize,         /* spaces per tab */
		indent_level,     /* current indent level for print output */
		linelength,      /* max line length on output */
		max_indent,       /* current maximum indent levels for first */
		state;            /* parsing state */
    BoolPtr first;        /* for first element on indented line for printing */
	Int4 linenumber;      /* for reporting errors */
	CharPtr word;           /* current word in linebuf */
	Int2 wordlen,         /* length of word in linebuf */
	     token;           /* current parsing token for word */
    PstackPtr typestack;  /* the parsing stack for input and output */
	Int1 type_indent,     /* used like indent_level and max_indent, but for */
		max_type;         /* typestack */
	ErrorRetType error_ret;     /* user error return */
    Pointer iostruct;    /* non-FILE io structure */
    IoFuncType readfunc,      /* read/write functions for sockets */
          writefunc;     /*  open and close MUST be done outside AsnIo */
	Boolean read_id;     /* for checking AsnReadId AsnReadVal alternation */
	CharPtr fname;       /* name of file in use */
	AsnOptionPtr aop;    /* head of options chain */
	AsnExpOptPtr aeop;   /* exploration options chain */
	AsnExpOptStructPtr aeosp;
	Boolean io_failure;  /* set on failed write, or non-ascii on read  */
} AsnIo, PNTR AsnIoPtr;

typedef struct asniomem {    /* for AsnIo to and from a memory block */
	AsnIoPtr aip;			 /* the AsnIoPtr for this */
	BytePtr buf;			 /* a buffer for the data */
	Int4 size,		/* size of this buffer (w) or bytes_to_read (r) */
			count;		/* count of bytes read from or written to buffer */
} AsnIoMem, PNTR AsnIoMemPtr;

typedef struct asniobs {    /* for AsnIo to and from a memory ByteStore */
	AsnIoPtr aip;			 /* the AsnIoPtr for this */
	ByteStorePtr bsp;        /* byte store for this */
} AsnIoBS, PNTR AsnIoBSPtr;

/***** typedefs used often in object loaders **********/

typedef Pointer (LIBCALL *AsnReadFunc) PROTO((AsnIoPtr aip, AsnTypePtr atp));
typedef Boolean (LIBCALL *AsnWriteFunc) PROTO((Pointer object, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   prototypes
*		
*****************************************************************************/
/*** asngen.c ****/

AsnTypePtr LIBCALL AsnReadId PROTO((AsnIoPtr aip, AsnModulePtr amp, AsnTypePtr atp));
Int2  LIBCALL AsnReadVal PROTO((AsnIoPtr aip, AsnTypePtr atp, DataValPtr vp));
Boolean LIBCALL AsnWrite PROTO((AsnIoPtr aip, AsnTypePtr atp, DataValPtr dvp));
Boolean LIBCALL AsnSkipValue PROTO((AsnIoPtr aip, AsnTypePtr atp));

Boolean LIBCALL AsnOpenStruct PROTO((AsnIoPtr aip, AsnTypePtr atp,
			Pointer the_struct));
Boolean LIBCALL AsnCloseStruct PROTO((AsnIoPtr aip, AsnTypePtr atp,
			Pointer the_struct));
Boolean LIBCALL AsnWriteChoice PROTO((AsnIoPtr aip, AsnTypePtr atp, Int2 choice,
			DataValPtr the_value));
void LIBCALL AsnCheckExpOpt PROTO((AsnIoPtr aip, AsnTypePtr atp, DataValPtr dvp));
AsnExpOptPtr LIBCALL AsnExpOptNew PROTO((AsnIoPtr aip, CharPtr path,
			Pointer user_data, AsnExpOptFunc user_func));
AsnExpOptPtr LIBCALL AsnExpOptFree PROTO((AsnIoPtr aip, AsnExpOptPtr aeop));

Int2 LIBCALL AsnGetLevel PROTO((AsnIoPtr aip));
void LIBCALL AsnNullValueMsg PROTO((AsnIoPtr aip, AsnTypePtr node));

/*** asntypes.c ***/

void LIBCALL AsnKillValue PROTO((AsnTypePtr atp, DataValPtr dvp));
AsnTypePtr PNTR LIBCALL AsnTypePathFind PROTO((AsnModulePtr amp, CharPtr str, Int2Ptr numtypes));
AsnTypePtr LIBCALL AsnTypeFind PROTO((AsnModulePtr amp, CharPtr str));
#define AsnFind(x) AsnTypeFind(NULL,x)    /* find type (all) */
CharPtr LIBCALL AsnFindPrimName PROTO((AsnTypePtr atp));
AsnTypePtr LIBCALL AsnFindBaseType PROTO((AsnTypePtr atp));
CharPtr LIBCALL AsnFindBaseName PROTO((AsnTypePtr atp));
Int2 LIBCALL AsnFindBaseIsa PROTO((AsnTypePtr atp));
AsnTypePtr LIBCALL AsnLinkType PROTO((AsnTypePtr type, AsnTypePtr localtype));
void LIBCALL AsnUnlinkType PROTO((AsnTypePtr type));
CharPtr LIBCALL AsnTypeDumpStack PROTO((CharPtr str, AsnIoPtr aip));
Boolean LIBCALL AsnTreeLoad PROTO((char * file, AsnValxNodePtr * avnptr, AsnTypePtr * atptr, AsnModulePtr * ampptr));
#define AsnLoad() AsnTreeLoad(asnfilename, &avn, &at, &amp)   /* simple loader */
void LIBCALL AsnModuleLink PROTO((AsnModulePtr amp));
CharPtr LIBCALL AsnEnumStr PROTO((CharPtr str, Int2 val));
CharPtr LIBCALL AsnEnumTypeStr PROTO((AsnTypePtr atp, Int2 val));
AsnModulePtr LIBCALL AsnAllModPtr PROTO((void));
Boolean LIBCALL AsnTypeStringToHex PROTO((Pointer from, Int4 len, Pointer to));

/*** asnio.c ****/

AsnIoPtr LIBCALL AsnIoOpen PROTO((CharPtr file_name, CharPtr mode));
AsnIoPtr LIBCALL AsnIoClose PROTO((AsnIoPtr aip));
void LIBCALL AsnIoReset PROTO((AsnIoPtr aip));
void LIBCALL AsnIoSetErrorMsg PROTO((AsnIoPtr aip, ErrorRetType error_ret));
Int4 LIBCALL AsnIoSeek PROTO((AsnIoPtr aip, Int4 pos));
Int4 LIBCALL AsnIoTell PROTO((AsnIoPtr aip));
void LIBCALL AsnIoFlush PROTO((AsnIoPtr aip));
AsnIoPtr LIBCALL AsnIoNew PROTO((Int1 type, FILE * fp, Pointer iostruct, IoFuncType readfunc, IoFuncType writefunc));
Boolean LIBCALL AsnIoSetBufsize PROTO((AsnIoPtr aip, Int2 size));
AsnOptionPtr LIBCALL AsnIoOptionNew PROTO((AsnIoPtr aip, Int2 ao_class, Int2 type, DataVal av, AsnOptFreeFunc freefunc));
void LIBCALL AsnIoOptionFree PROTO((AsnIoPtr aip, Int2 ao_class, Int2 type));
Boolean LIBCALL AsnClassTypeMatch PROTO((Int2 ao_class, Int2 type, Int2 this_class, Int2 this_type));
AsnOptionPtr LIBCALL AsnIoOptionGet PROTO((AsnIoPtr aip, Int2 ao_class, Int2 type,
							AsnOptionPtr last));
AsnOptionPtr LIBCALL AsnOptionNew PROTO((AsnOptionPtr PNTR aopp, Int2 ao_class, Int2 type, DataVal av, AsnOptFreeFunc freefunc));
void LIBCALL AsnOptionFree PROTO((AsnOptionPtr PNTR aopp, Int2 ao_class, Int2 type));
AsnOptionPtr LIBCALL AsnOptionGet PROTO((AsnOptionPtr head, Int2 ao_class, Int2 type,
							AsnOptionPtr last));

   /*** read and write to memory buffer ***/
AsnIoMemPtr LIBCALL AsnIoMemOpen PROTO((CharPtr mode, BytePtr buf, Int4 size));
AsnIoMemPtr LIBCALL AsnIoMemClose PROTO((AsnIoMemPtr aimp));
Boolean LIBCALL AsnIoMemReset PROTO((AsnIoMemPtr aimp, Int4 bytes_to_read));
Int2 LIBCALL AsnIoMemRead PROTO((Pointer, CharPtr, Uint2));
Int2 LIBCALL AsnIoMemWrite PROTO((Pointer, CharPtr, Uint2));

   /*** read and write to a ByteStore in memory ***/
AsnIoBSPtr LIBCALL AsnIoBSOpen PROTO((CharPtr mode, ByteStorePtr bsp));
AsnIoBSPtr LIBCALL AsnIoBSClose PROTO((AsnIoBSPtr aibp));
Int2 LIBCALL AsnIoBSRead PROTO((Pointer, CharPtr, Uint2));
Int2 LIBCALL AsnIoBSWrite PROTO((Pointer, CharPtr, Uint2));

  /** Copy and Compare functions ***/
Pointer LIBCALL AsnIoCopy PROTO((Pointer from, AsnReadFunc readfunc, AsnWriteFunc writefunc));
Pointer LIBCALL AsnIoMemCopy PROTO((Pointer from, AsnReadFunc readfunc, AsnWriteFunc writefunc));
Boolean LIBCALL AsnIoMemComp PROTO((Pointer a, Pointer b, AsnWriteFunc writefunc));

#define AsnIoNullOpen() AsnIoNew((ASNIO_OUT | ASNIO_TEXT | ASNIO_CARRIER), NULL, NULL, NULL, NULL)

/*** asndebin.c ***/

AsnTypePtr LIBCALL AsnBinReadId PROTO((AsnIoPtr aip, AsnTypePtr atp));
Int2 LIBCALL AsnBinReadVal PROTO((AsnIoPtr aip, AsnTypePtr atp, DataValPtr vp));

/*** asnenbin.c ***/

Boolean LIBCALL AsnBinWrite PROTO((AsnIoPtr aip, AsnTypePtr atp, DataValPtr dvp));
         /** expert use only ***/
Boolean LIBCALL AsnEnBinTheBytes PROTO((Pointer ptr, Uint4 len, AsnIoPtr aip, Boolean is_string));
#define AsnEnBinBytes(a,b,c) AsnEnBinTheBytes(a, b, c, FALSE)

/*** asnlex.c ***/

AsnTypePtr LIBCALL AsnTxtReadId PROTO((AsnIoPtr aip, AsnModulePtr amp, AsnTypePtr atp));
Int2 LIBCALL AsnTxtReadVal PROTO((AsnIoPtr aip, AsnTypePtr atp, DataValPtr vp));

/*** asnprint.c ***/

Boolean LIBCALL AsnTxtWrite PROTO((AsnIoPtr aip, AsnTypePtr atp, DataValPtr dvp));

/*** asnlext.c ***/

AsnModulePtr LIBCALL AsnLoadModules PROTO((AsnIoPtr aip));

/******** temporary defines for older code *************/

#define AsnStartStruct(x,y) AsnOpenStruct(x, y, NULL)
#define AsnEndStruct(x,y) AsnCloseStruct(x, y, NULL)

/***** AsnOption ao_class values - do not reuse ***************/
/***** all positive numbers > 0 are available to non-NCBI applications ***/

#define OP_ANY          0
#define OP_TOGENBNK    -1
#define OP_BB2ASN      -2
#define OP_NCBIOBJSSET -3
#define OP_NCBIOBJSEQ  -4
#define OP_GET_MUID    -5
#define OP_NCBIASNTOOL -6
#define OP_NCBIHINTS   -7
#define OP_GESTALT     -8

/****** these are the possible returns from AsnFindBaseIsa() *****/
/****** the numbers are arbitrary, but should never be changed ***/

#define BOOLEAN_TYPE			301		/* BOOLEAN */
#define INTEGER_TYPE			302		/* INTEGER */
#define BITS_TYPE				303		/* BIT STRING */
#define OCTETS_TYPE				304		/* OCTET STRING */
#define NULL_TYPE				305		/* NULL */
#define OBID_TYPE				306		/* OBJECT IDENTIFIER */
#define OBDES_TYPE				307		/* ObjectDescriptor */
#define EXTERNAL_TYPE			308		/* EXTERNAL */
#define REAL_TYPE				309		/* REAL */
#define ENUM_TYPE				310		/* ENUMERATED */
#define SEQ_TYPE				311		/* SEQUENCE */
#define SEQOF_TYPE				312		/* SEQUENCE OF */
#define SET_TYPE				313		/* SET */
#define SETOF_TYPE				314		/* SET OF */
#define CHOICE_TYPE				315		/* CHOICE */
#define ANY_TYPE				316		/* ANY */
#define	NUMERICSTRING_TYPE		317	    /* String Types */
#define	PRINTABLESTRING_TYPE	318
#define TELETEXSTRING_TYPE		319
#define VIDEOTEXSTRING_TYPE		320
#define IA5STRING_TYPE 			321
#define GRAPHICSTRING_TYPE		322
#define VISIBLESTRING_TYPE		323
#define GENERALSTRING_TYPE		324
#define CHARACTERSTRING_TYPE	325
#define GENTIME_TYPE			326		/* Time types */
#define UTCTIME_TYPE			327

#define STRSTORE_TYPE			351		/* Application: StringStore */

/******* grouping macros on ISA defines above ********************/

#define ISA_STRINGTYPE(x)	(((x) >= 317) && ((x) <= 325))
#define ISA_STRUCTTYPE(x)	(((x) >= 311) && ((x) <= 314))
#define ISA_INTTYPE(x)		(((x) == 302) || ((x) == 310))

#ifdef __cplusplus
}
#endif

#endif
