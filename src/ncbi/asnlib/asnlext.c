/*  asnlext.c
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
* File Name:  asnlext.c
*
* Author:  James Ostell
*
* Version Creation Date: 3/4/91
*
* $Revision: 2.1 $
*
* File Description:
*   Routines for parsing ASN.1 module definitions.  Extends asnlex.c
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 2/11/91  Ostell      AsnModuleLink - corrected bug linking imported types
*                         to other imported types.
* 3/4/91   Kans        Stricter typecasting for GNU C and C++
* 04-20-93 Schuler     LIBCALL calling convention
*
* ==========================================================================
*/

/*****************************************************************************
*
*   asnlext.c
*   	routines of type AsnLexT..()
*   	Lexxer routines for Asn.1 specifications
*
*****************************************************************************/

#include "asnbuild.h"

#define NUMASNWORD 58         /* number of asnwords */

static CharPtr asnwords[NUMASNWORD] = {     /* primitives from asn.1 */
	"BOOLEAN",				 /*  1 */
	"INTEGER",				 /*  2 */
	"BIT",					 /*  3 */
	"OCTET",				 /*  4 */
	"NULL",					 /*  5 */
	"OBJECT",				 /*  6 */
	"ObjectDescriptor",		 /*  7 */
	"EXTERNAL",				 /*  8 */
	"REAL",					 /*  9 */
	"ENUMERATED",			 /* 10 */
	"SEQUENCE",				 /* 11 */
	"STRING",				 /* 12 */
	"SET",					 /* 13 */
	"OF",					 /* 14 */
	"CHOICE",                /* 15 */
	"ANY",           		/* 16 */
	"NumericString",    	/* 17 */
	"PrintableString",      /* 18 */
	"TeletexString", 		/* 19 */
	"VideotexString",		/* 20 */
	"IA5String",			/* 21 */
	"GraphicString",		/* 22 */
	"VisibleString",		/* 23 */
	"GeneralString",		/* 24 */
	"CharacterString",		/* 25 */
	"GeneralizedTime",		/* 26 */
	"UTCTime",				/* 27 */
	"DEFINITIONS",           /* 28 */
	"BEGIN",                 /* 29 */
	"END",                   /* 30 */
	"APPLICATION",           /* 31 */
	"PRIVATE",               /* 32 */
	"UNIVERSAL",             /* 33 */
	"COMPONENTS",			 /* 34 */
	"DEFAULT" ,              /* 35 */
	"FALSE",                 /* 36 */
	"TRUE",                  /* 37 */
	"IMPLICIT",              /* 38 */
	"OPTIONAL",              /* 39 */
	"EXPORTS",				/* 40 */
	"IMPORTS",				/* 41 */
	"ABSENT", 				/* 42 */
	"BY",					/* 43 */
	"COMPONENT",            /* 44 */
	"DEFINED",              /* 45 */
	"FROM",                 /* 46 */
	"INCLUDES",             /* 47 */
	"MIN",                  /* 48 */
	"MINUS-INFINITY",       /* 49 */
	"MAX",                  /* 50 */
	"PRESENT",              /* 51 */
	"PLUS-INFINITY",        /* 52 */
	"SIZE",                 /* 53 */
	"TAGS",                 /* 54 */
	"WITH",                 /* 55 */
	"IDENTIFIER",			 /* 56 */
	"StringStore",           /* 57 */      /* NCBI application type */
	"$Revision:"             /* 58 */       /* NCBI asn versions */
	};

static AsnPrimType asnprimtypes[] = {    
	{BOOLEAN_TYPE, "BOOLEAN", 0, 1 },
	{INTEGER_TYPE, "INTEGER", 0, 2 },
	{BITS_TYPE, "BIT STRING", 0, 3 },
	{OCTETS_TYPE, "OCTET STRING", 0, 4 },
	{NULL_TYPE, "NULL", 0, 5 },
	{OBID_TYPE, "OBJECT IDENTIFIER", 0, 6 },
	{OBDES_TYPE, "ObjectDescriptor", 0, 7 },
	{EXTERNAL_TYPE, "EXTERNAL", 0, 8 },
	{REAL_TYPE, "REAL", 0, 9 },
	{ENUM_TYPE, "ENUMERATED", 0, 10 },
	{SEQ_TYPE, "SEQUENCE", 0, 16 },
	{SEQOF_TYPE, "SEQUENCE OF", 0, 16 },
	{SET_TYPE, "SET", 0, 17 },
	{SETOF_TYPE, "SET OF", 0, 17 },
	{CHOICE_TYPE, "CHOICE", 0, -1 },
	{ANY_TYPE, "ANY", 0, -1 },
	{NUMERICSTRING_TYPE, "NumericString", 0, 18 },
	{PRINTABLESTRING_TYPE, "PrintableString", 0, 19 },
	{TELETEXSTRING_TYPE, "TeletexString", 0, 20 },
	{VIDEOTEXSTRING_TYPE, "VideotexString", 0, 21 },
	{IA5STRING_TYPE, "IA5String", 0, 22 },
	{GRAPHICSTRING_TYPE, "GraphicString", 0, 25 },
	{VISIBLESTRING_TYPE, "VisibleString", 0, 26 },
	{GENERALSTRING_TYPE, "GeneralString", 0, 27 },
	{CHARACTERSTRING_TYPE, "CharacterString", 0, 28 },
	{GENTIME_TYPE, "GeneralizedTime", 0, 24 },
	{UTCTIME_TYPE, "UTCTime", 0, 23 }};

static AsnPrimType asnapptypes[] = {         /* application wide types */
	{STRSTORE_TYPE, "StringStore", 64, 1 }
	};


/*****************************************************************************
*
*   AsnLoadModules(aip)
*   	reads and links several modules
*
*****************************************************************************/
AsnModulePtr LIBCALL  AsnLoadModules (AsnIoPtr aip)
{
	AsnModulePtr amp, nextmod, currentmod = NULL;
	
	amp = NULL;
	while ((nextmod = AsnLexTReadModule(aip)) != NULL )
	{
		if (amp == NULL)
			amp = nextmod;
		else
			currentmod->next = nextmod;
		currentmod = nextmod;
	}

	AsnModuleLink(amp);      /* link up modules where possible */
	return amp;
}
					
/*****************************************************************************
*
*   AsnLexTReadModule(aip)
*
*****************************************************************************/
AsnModulePtr AsnLexTReadModule (AsnIoPtr aip)
{
	AsnModulePtr amp;

					/* get name and look for valid start */

	amp = AsnLexTStartModule(aip);
	if (amp == NULL)
		return amp;

			   		/* scan declarations */
	
	while (aip->token != END_TOKEN)     /* END */
	{
		if (aip->token == REF)        /* its a type assignment */
			aip->token = AsnLexTReadTypeAssign(aip, amp);
		/**************************** not implemented yet ********
		else if (token == IDENT) 
			aip->token = AsnReadDefinedValue(aip, amp);
		**********************************************************/
		else
		{
			AsnIoErrorMsg(aip, 60, aip->linenumber, aip->word);
			return NULL;
		}
	}

	amp = AsnCheckModule(amp, aip);

	return amp;
}

/*****************************************************************************
*
*   Int2 AsnLexTReadTypeAssign(aip, amp)
*   	reads a type assignment for one type
*       assumes has read name already
*   	returns token to following item
*       returns 0 on error
*
*****************************************************************************/
Int2 AsnLexTReadTypeAssign (AsnIoPtr aip, AsnModulePtr amp)
{
	AsnTypePtr atp;
	Int2 token;

							/* store the typereference */

	atp = AsnTypeNew(aip, amp);

	if ((token = AsnLexTWord(aip)) != ISDEF)
	{
		AsnIoErrorMsg(aip, 28, aip->linenumber);
		return 0;
	}
	
	token = AsnLexTWord(aip);   /* get next element */

	token = AsnLexTReadType(aip, amp, atp);

	return token;
}

/*****************************************************************************
*
*   Int2 AsnLexTReadType(aip, amp, atp)
*   	reads a type assignment for one type
*       assumes has read name already
*       assumes has read next input item after name
*   	returns token to following item
*       returns 0 on error
*
*****************************************************************************/
Int2 AsnLexTReadType (AsnIoPtr aip, AsnModulePtr amp, AsnTypePtr atp)
{
	Int2 isa;
	Boolean first;
	AsnValxNodePtr avnp;
	Int2 token;

	token = aip->token;

	if (token == IMPLICIT_TOKEN) 
	{
		atp->implicit = TRUE;
		token = AsnLexTWord(aip);
	}

	if (token == START_TAG)                      /* TAG */
	{
		token = AsnLexTWord(aip);
		if (ISA_TAGCLASS(token))
		{
			switch (token)
			{
				case UNIV_TAG_TOKEN:
					atp->tagclass = TAG_UNIVERSAL;
					break;
				case PRIV_TAG_TOKEN:
					atp->tagclass = TAG_PRIVATE;
					break;
				case AP_TAG_TOKEN:
					atp->tagclass = TAG_APPLICATION;
					break;
			}
			token = AsnLexTWord(aip);
		}
		else
			atp->tagclass = TAG_CONTEXT;

		if (token == NUMBER)
		{
			atp->tagnumber = (Int2) AsnLexInteger(aip);
			token = AsnLexTWord(aip);
			if (token != END_TAG)
			{
				AsnIoErrorMsg(aip, 61, aip->linenumber);
				return 0;
			}
			else
				token = AsnLexTWord(aip);
		}
		else if (token == IDENT)
		{
			AsnIoErrorMsg(aip, 62, aip->linenumber);
			return 0;
		}
		else
		{
			AsnIoErrorMsg(aip, 63, aip->linenumber);
			return 0;
		}
	}

	atp->type = AsnGetType(aip, amp);
	isa = atp->type->isa;
	
	token = AsnLexTWord(aip);   /* read next token */
            
 					/*********** SubType Processing *********************/
	if (token == OPEN_PAREN)
	{
		AsnIoErrorMsg(aip, 64, aip->linenumber);
		return 0;
	}
	                 /******************* process various types *********/
	
	if (isa < 400)       /* operate on builtin types */
		switch (isa)
	{
		case INTEGER_TYPE:           /* check for named integers */
			if (token != START_STRUCT)
				break;
		case ENUM_TYPE:
			if (token != START_STRUCT)
			{
				AsnIoErrorMsg(aip, 65, aip->linenumber);
				return 0;
			}

									 /* read named integers */
			first = TRUE;
			avnp = NULL;
			token = AsnLexTWord(aip);
			while (token != END_STRUCT)
			{
				avnp = AsnValxNodeNew(avnp, VALUE_ISA_NAMED_INT);

				if (! first)
				{
					if (token != COMMA)
					{
						AsnIoErrorMsg(aip, 66, aip->linenumber);
						return 0;
					}
					else
						token = AsnLexTWord(aip);
				}
				else
				{
					first = FALSE;
					atp->branch = avnp;
				}

				if (token != IDENT)
				{
					AsnIoErrorMsg(aip, 86, aip->linenumber);
					return 0;
				}
				avnp->name = AsnLexSaveWord(aip);
				token = AsnLexTWord(aip);
				if (token != OPEN_PAREN)
				{
					AsnIoErrorMsg(aip, 87, aip->linenumber);
					return 0;
				}
				avnp->intvalue = AsnLexReadInteger(aip, atp);
				token = AsnLexTWord(aip);
				if (token != CLOSE_PAREN)
				{
					AsnIoErrorMsg(aip, 88, aip->linenumber);
					return 0;
				}
				token = AsnLexTWord(aip);
			}
			token = AsnLexTWord(aip);
			break;
		case SETOF_TYPE:			/* create branch for type */
		case SEQOF_TYPE:
			atp->branch = AsnElementTypeNew(NULL);   /* get unnamed element for type */
			token = AsnLexTReadType(aip, amp, (AsnTypePtr) atp->branch);
			break;
		case SEQ_TYPE:
		case SET_TYPE:
			atp->branch = AsnLexTReadElementTypeList(aip, amp);
			token = aip->token;
			break;
		case CHOICE_TYPE:
			atp->branch = AsnLexTReadAlternativeTypeList(aip, amp);
			token = aip->token;
			break;
	}
	atp->resolved = TRUE;
	return token;
}

/*****************************************************************************
*
*	AsnTypePtr AsnLexTReadElementTypeList(aip, amp)
*   	assumes has read first { already
*   	returns token to following element
*
*****************************************************************************/
AsnTypePtr AsnLexTReadElementTypeList (AsnIoPtr aip, AsnModulePtr amp)
{
	Int2 token;
	AsnTypePtr atp, atp1, atplast, atp2;
	AsnValxNodePtr avnp;

	token = aip->token;
	atp1 = NULL;
	atplast = NULL;

	if (token != START_STRUCT)
	{
		AsnIoErrorMsg(aip, 37, aip->linenumber);
		return NULL;
	}
	token = AsnLexTWord(aip);

	while (token != END_STRUCT)
	{
		if (atp1 != NULL)        /* not the first */
		{
			if (token != COMMA)
			{
				AsnIoErrorMsg(aip, 66, aip->linenumber);
				return NULL;
			}
			else
				token = AsnLexTWord(aip);
		}

		if (token == COMPS_TOKEN)        /* COMPONENTS OF */
		{
			AsnIoErrorMsg(aip, 93, aip->linenumber);
			return NULL;
		}
		else if (token != IDENT)	     /* not named */
		{
			atp = AsnElementTypeNew(NULL);
		}
		else					         /* named Type */
		{
			atp = AsnElementTypeNew(aip);
			token = AsnLexTWord(aip);
		}
								/* add to chain */
		if (atp1 == NULL)
			atp1 = atp;
		else
			atplast->next = atp;

		token = AsnLexTReadType(aip, amp, atp);

		if (token == OPTIONAL_TOKEN)         /* OPTIONAL */
		{
			atp->optional = TRUE;
			token = AsnLexTWord(aip);
		}
		else if (token == DEFAULT_TOKEN)    /* DEFAULT */
		{
			atp->hasdefault = TRUE;
			avnp = AsnValxNodeNew(NULL, VALUE_ISA_BOOL);
			atp->defaultvalue = avnp;
						/* read the default value */
			atp2 = AsnFindBaseType(atp);

			if (atp2 == NULL)
			{
				AsnIoErrorMsg(aip, 95, atp->name, aip->linenumber);
				return NULL;
			}

			switch (atp2->type->isa)
			{
				case BOOLEAN_TYPE:
					avnp->intvalue = (Int4) AsnLexReadBoolean(aip, atp2);
					break;
				case INTEGER_TYPE:
				case ENUM_TYPE:
					avnp->valueisa = VALUE_ISA_INT;
					avnp->intvalue = AsnLexReadInteger(aip, atp2);
					break;
				case VISIBLESTRING_TYPE:
					avnp->valueisa = VALUE_ISA_PTR;
					avnp->name = (CharPtr) AsnLexReadString(aip, atp2);
					break;
				case REAL_TYPE:
					avnp->valueisa = VALUE_ISA_REAL;
					avnp->realvalue = AsnLexReadReal(aip, atp2);
					break;
				default:
					AsnIoErrorMsg(aip, 94, atp->name);
					return NULL;
			}
			token = AsnLexTWord(aip);
		}
		atplast = atp;
	}
	token = AsnLexTWord(aip);   /* read following item */
	return atp1;
}

/*****************************************************************************
*
*	AsnTypePtr AsnLexTReadAlternativeTypeList(aip, amp)
*   	assumes has read first { already
*   	returns token to following element
*
*****************************************************************************/
AsnTypePtr AsnLexTReadAlternativeTypeList (AsnIoPtr aip, AsnModulePtr amp)
{
	Int2 token;
	AsnTypePtr atp, atp1, atplast;

	token = aip->token;
	atp1 = NULL;
	atplast = NULL;

	if (token != START_STRUCT)
	{
		AsnIoErrorMsg(aip, 37, aip->linenumber);
		return NULL;
	}
	token = AsnLexTWord(aip);

	while (token != END_STRUCT)
	{
		if (atp1 != NULL)        /* not the first */
		{
			if (token != COMMA)
			{
				AsnIoErrorMsg(aip, 66, aip->linenumber);
				return NULL;
			}
			else
				token = AsnLexTWord(aip);
		}

		if (token != IDENT)	     /* not named */
		{
			atp = AsnElementTypeNew(NULL);
		}
		else					         /* named Type */
		{
			atp = AsnElementTypeNew(aip);
			token = AsnLexTWord(aip);
		}
								/* add to chain */
		if (atp1 == NULL)
			atp1 = atp;
		else
			atplast->next = atp;

		token = AsnLexTReadType(aip, amp, atp);
		atplast = atp;
	}
	token = AsnLexTWord(aip);   /* read following item */
	return atp1;
}

/*****************************************************************************
*
*   AsnModulePtr AsnLexTStartModule(aip)
*   	if $Revision: in comment in first line of file, puts the string
*        containing the revision number in amp->filename
*       if --$Revision: X.YY
*     			stores "YY"
*   	if --$Revision: X
*               stores "X"
*
*****************************************************************************/
AsnModulePtr AsnLexTStartModule (AsnIoPtr aip)
{
	Int2 token;
	AsnModulePtr amp;
	Int2 asntype;
	AsnTypePtr atp;
	Boolean wasref;

	amp = (AsnModulePtr) MemNew(sizeof(AsnModule));

	token = AsnLexTWord(aip);    /* get the module name or $Revision: */

	if (token == REVISION_TOKEN)            /* revision of file */
	{
		amp->filename = AsnLexSaveWord(aip);
		token = AsnLexTWord(aip);     /* get the module name */
	}

	if (token != REF)
	{
		MemFree(amp->filename);
		amp = MemFree(amp);
		return amp;
	}

	if ((asntype = AsnLexTMatchToken(aip)) != 0)     /* any predefined value is wrong */
	{
		AsnIoErrorMsg(aip, 67, asnwords[asntype -1], aip->linenumber);
		return NULL;
	}

						     /* create a new module */


	amp->modulename = AsnLexSaveWord(aip);
	amp->lasttype = 400;
	amp->lastvalue = 10000;

	token = AsnLexTWord(aip);

	if (token == START_STRUCT)       /* it has an object identifier */
	{
		AsnIoErrorMsg(aip, 68);
		    /* skip object identifier */
		AsnLexSkipStruct(aip);
		token = AsnLexTWord(aip);  /* next token */
	}

	if (token != DEF_TOKEN)        /* DEFINITIONS */
	{
		AsnIoErrorMsg(aip, 69, asnwords[DEF_TOKEN - 401], aip->linenumber);
		return NULL;
	}
	else
		token = AsnLexTWord(aip);

	if (token != ISDEF)           /* ::= */
	{
		AsnIoErrorMsg(aip, 69, "::=", aip->linenumber);
		return NULL;
	}
	else
		token = AsnLexTWord(aip);

	if (token != BEGIN_TOKEN)    /* BEGIN */
	{
		AsnIoErrorMsg(aip, 69, asnwords[BEGIN_TOKEN - 401], aip->linenumber);
		return NULL;
	}
	else
		token = AsnLexTWord(aip);

	if (token == EXPORTS_TOKEN)     /* read any EXPORTS */
	{
		wasref = FALSE;
		while ((token = AsnLexTWord(aip)) != SEMI_COLON)
		{
			if (token == REF)     /* should be REF */
			{
				if (wasref)
				{
					AsnIoErrorMsg(aip, 66, aip->linenumber);
					return NULL;
				}
				atp = AsnTypeNew(aip, amp);
				atp->exported = TRUE;
				wasref = TRUE;
			}
			else if (token == COMMA)
			{
				if (! wasref)
				{
					AsnIoErrorMsg(aip, 70, aip->linenumber);
					return NULL;
				}
			   	wasref = FALSE;
			}
			else
			{
				AsnIoErrorMsg(aip, 59, ' ', aip->linenumber);
				return NULL;
			}
		}
						   /* get next after semi-colon */

		token = AsnLexTWord(aip);
	}

	if (token == IMPORTS_TOKEN)             /* IMPORTS */
	{
		wasref = FALSE;
		token = AsnLexTWord(aip);

		while (token != SEMI_COLON)
		{
			if (token == REF)     /* should be REF */
			{
				atp = AsnTypeNew(aip, amp);
				atp->imported = TRUE;
				wasref = TRUE;
			}
			else if (token == COMMA)
			{
				if (! wasref)
				{
					AsnIoErrorMsg(aip, 70, aip->linenumber);
					return NULL;
				}
			   	wasref = FALSE;
			}
			else if (token == FROM_TOKEN)
			{
				if (! wasref)
				{
					AsnIoErrorMsg(aip, 71, aip->linenumber);
					return NULL;
				}
				else								/* read source module */
					token = AsnLexTWord(aip);

				if (token != REF)
				{
					AsnIoErrorMsg(aip, 72, aip->linenumber);
					return NULL;
				}
				else
				{
					atp = amp->types;
					while (atp != NULL)
					{
						if ((atp->imported == TRUE) && (atp->branch == NULL))
							atp->branch = AsnLexSaveWord(aip);
						atp = atp->next;
					}
				}
				wasref = FALSE;
			}
			else if (token == START_STRUCT)
			{
				AsnIoErrorMsg(aip, 73, aip->linenumber);
				AsnLexSkipStruct(aip);
				token = AsnLexTWord(aip);
				wasref = FALSE;
			}
			else
				AsnIoErrorMsg(aip, 96, aip->linenumber);

			token = AsnLexTWord(aip);
		}
						   /* get next after semi-colon */
		token = AsnLexTWord(aip);
	}

	if (token == EXPORTS_TOKEN)     /* EXPORTS out of place */
		AsnIoErrorMsg(aip, 97, aip->linenumber);

	return amp;
}


/*****************************************************************************
*
*   Int2 AsnLexTMatchToken(aip)
*
*****************************************************************************/
Int2 AsnLexTMatchToken (AsnIoPtr aip)
{
	CharPtr word;
	register int i;
	Int2 len;

	word = aip->word;
	len = aip->wordlen;

	for (i = 0; i < NUMASNWORD; i++)
	{
		if (StrMatch(asnwords[i], word, len))
			return i+1;
	}
	return 0;     /* not found */
}
/*****************************************************************************
*
*   Int2 AsnLexTWord(aip)
*   	reads words, punctuation, and asn keywords with 2 parts
*   	returns tokens defined at top
*
*****************************************************************************/
Int2 AsnLexTWord (AsnIoPtr aip)
{
	register CharPtr pos;
	register int len;
	Int1 state;
	Int2 token, asntype, linepos;
	int done;
	Boolean first = FALSE;

	if (! aip->bytes)   /* no data loaded */
	{
		first = TRUE;
		AsnIoGets(aip);
	}
		
	linepos = aip->linepos;
	pos = aip->linebuf + linepos;
	state = aip->state;
	len = 0;
	token = -1;

	while (*pos == '\n')    /* skip empty lines */
	{
		pos = AsnIoGets(aip);

		if (pos == NULL)
			return EOF_TOKEN;
	}
	
	if (state == IN_STRING_STATE)
	{
		aip->word = pos;
		if (* pos == '\"')    /* end of string */
		{
			token = END_STRING;
			pos++;
			state = 0;        /* reset state */
		}
		else
		{
			token = IN_STRING;
			while ((* pos != '\"') && (* pos != '\n'))
			{
				pos++; len++;
			}

			if ((*pos != '\n') && (* (pos + 1) == '\"')) /* internal quote */
			{
				len++;        /* include in previous string */
				pos += 2;     /* point to rest of string */
			}
		}
	}
	else if (state == IN_BITHEX_STATE)
	{
		aip->word = pos;
		if (*pos == '\'')  			  /* end of binhex */
		{
			state = 0;              /* set to normal */
			pos++;                       /* move past quote */
			while (IS_WHITESP(*pos))
				pos++;
			if (* pos == 'H')
				token = OCTETS;
			else if (* pos == 'B')
				token = ASNBITS;
			else
			{
				AsnIoErrorMsg(aip, 58, aip->linenumber);
				token = ERROR_TOKEN;
			}
		}
		else
		{
			token = IN_BITHEX;
			while ((* pos != '\'') && (* pos != '\n'))
			{
				pos++; len++;
			}
		}   
	}
	else              /* normal scanning */
	{
		done = 0;
		while (! done)
		{
			while (* pos <= ' ')     /* skip leading white space */
			{
				if (*pos == '\n')
				{
					pos = AsnIoGets(aip);

					if (pos == NULL)
						return EOF_TOKEN;
				}
				else
					pos++;
			}
			done = 1;
			
			if ((*pos == '-') && (*(pos+1) == '-'))   /* skip comments */
			{
				pos += 2;
				if (first)   /* could be revision */
				{
				 	first = FALSE;
					if (StrMatch(asnwords[57], pos, 10))  /* $Revision: */
					{
						token = REVISION_TOKEN;
						pos += 10;
						while (IS_WHITESP(*pos))
							pos++;
						aip->word = pos;
						while (IS_DIGIT(*pos))       /* eg. 1.2 */
						{
							len++;
							pos++;
						}
						if (*pos == '.')        /* take after . if present */
						{
							pos++;
							aip->word = pos;
							len = 0;
							while (IS_DIGIT(*pos))
							{
								len++;
								pos++;
							}
						}
					}
				}
				
				done = 0;
				while (! done)   /* skip to end of comment */
				{
					if ((*pos == '-') && (*(pos +1) == '-'))
					{
						pos += 2;
						done = 1;
					}
					else if (*pos == '\n')
						done = 1;
					else
						pos++;
				}

				if (token == REVISION_TOKEN)
				{
					aip->linepos = pos - aip->linebuf;
					aip->state = state;
					aip->wordlen = len;
					aip->token = token;
					return token;
				}

				if (*pos <= ' ')
					done = 0;
				else
					done = 1;
			}
		}

		aip->word = pos;
		if (* pos == '\"')
		{
			token = START_STRING;
			state = IN_STRING_STATE;
		}
		else if (* pos == '\'')
		{
			token = START_BITHEX;
			state = IN_BITHEX_STATE;
		}
		else if (* pos == ',')
			token = COMMA;
		else if (* pos == '{')
			token = START_STRUCT;
		else if (* pos == '}')
			token = END_STRUCT;
		else if (* pos == '[')
			token = START_TAG;
		else if (* pos == ']')
			token = END_TAG;
		else if (* pos == '(')
			token = OPEN_PAREN;
		else if (* pos == ')')
			token = CLOSE_PAREN;
		else if (* pos == ';')
			token = SEMI_COLON;
		else if (* pos == ':')
		{
			if ((*(pos + 1) == ':') && (*(pos + 2) == '='))
			{
				token = ISDEF;
				pos += 2;
				len = 3;
			}
			else
			{
				AsnIoErrorMsg(aip, 59, *pos, aip->linenumber);
				token = ERROR_TOKEN;
			}
		}
		else if (IS_UPPER(*pos))  /* a reference or keyword */
		{
			token = REF;
			while ((IS_ALPHANUM(*pos)) || (*pos == '-'))
			{
				pos++; len++;
			}

			aip->wordlen = len;
			asntype = AsnLexTMatchToken(aip);    /* check types */
			if (asntype)          /* did it match ? */
			{
				if ((asntype > 27) && (asntype < 57))   /* not a primitive type */
				{
					token = asntype + 400;   /* make a keyword type */
					if (asntype == COMPS_TOKEN)  /* COMPONENTS OF */
					{
						if ((*(pos + 1) == 'O') &&
							(*(pos + 2) == 'F') &&
							(IS_WHITESP(*(pos+3))))
						{
							pos += 3;    /* move past OF */
							len += 3;
						}
						else
							AsnIoErrorMsg(aip, 89, aip->linenumber);
					}
				}
				else if (asntype == 57)    /* StringStore */
					token = STRSTORE_TYPE;
				else
				{
					switch (asntype)
					{
						case 3:				/* BIT */
						case 4:				/* OCTET */
							if (! StrMatch(asnwords[11], (pos+1), 6))
								AsnIoErrorMsg(aip, 90, aip->linenumber);
							pos += 7;       /* move past STRING */
							len += 7;
							break;
						case 11:			/* SEQUENCE */
						case 13:			/* SET */
							if ((*(pos + 1) == 'O') &&
								(*(pos + 2) == 'F'))
							{
								asntype++;   /* SET or SEQ OF */
								pos += 3;
								len += 3;
								if (! IS_WHITESP(*pos))
									AsnIoErrorMsg(aip, 91, aip->linenumber);
							}
							break;
						case 6:				/* OBJECT */
							if ((! StrMatch(asnwords[55], (pos+1), 10)))  /* IDENTIFIER */
								AsnIoErrorMsg(aip, 92, aip->linenumber);
							pos += 11;
							len += 11;
							break;
						default:
							break;
					}
					token = asntype + 300;   /* change to point at type */
				}
			}
			pos--;    /* move back for increment at end */
			len--;
		}
		else if (IS_LOWER(*pos))  /* an identifier or valuereference */
		{
			token = IDENT;
			while ((IS_ALPHANUM(*pos)) || (*pos == '-'))
			{
				pos++; len++;
			}
			pos--;		  /* move back for increment at end */
			len--;
		}
		else if ((IS_DIGIT(*pos)) || ((*pos == '-') && (IS_DIGIT(*(pos+1)))))
		{
			token = NUMBER;
			if (*pos == '-')
			{
				pos++; len++;
			}

			while (IS_DIGIT(*pos))
			{
				pos++; len++;
			}
			pos--;    /* move back for increment at end */
			len--;
		}
		else
		{
			AsnIoErrorMsg(aip, 59, *pos, aip->linenumber);
			token = ERROR_TOKEN;
		}
		len++; pos++;     /* move over last symbol */
	}
	aip->linepos = pos - aip->linebuf;
	aip->state = state;
	aip->wordlen = len;
	aip->token = token;
	return token;
}

/*****************************************************************************
*
*   AsnTypePtr AsnGetType(aip, amp)
*   	return pointer to type of last element read
*
*****************************************************************************/
AsnTypePtr AsnGetType (AsnIoPtr aip, AsnModulePtr amp)
{
	AsnTypePtr atp;
	AsnPrimTypePtr aptp;
	Int2 wordlen;
	Int2 token;

	wordlen = aip->wordlen;
	token = aip->token;

	if (ISA_ASNTYPE(aip->token))     /* a primitive */
	{
		aptp = asnprimtypes;
		while (aptp->isa != token)
			aptp++;
		return (AsnTypePtr)aptp;
	}
	else if (ISA_BASETYPE(aip->token))    /* an application type */
	{
		aptp = asnapptypes;
		while (aptp->isa != token)
			aptp++;
		return (AsnTypePtr)aptp;
	}

	if (aip->token != REF)        /* not a type */
	{
		AsnIoErrorMsg(aip, 98, aip->word, aip->linenumber);
		return NULL;
	}

		/**************** not a primitive - do we have it already? *******/

	atp = amp->types;
	while (atp != NULL)
	{
		if (StrMatch(atp->name, aip->word, wordlen))   /* it matches */
		{
			if (atp->imported == TRUE)   /* reference to imported type */
				atp->resolved = TRUE;
			return atp;
		}
		else
			atp = atp->next;
	}

		/*************** not defined - add unresolved node *************/

	atp = AsnTypeNew(aip, amp);

	return atp;
}

/*****************************************************************************
*
*   AsnTypePtr AsnTypeNew(aip, amp)
*
*****************************************************************************/
AsnTypePtr AsnTypeNew (AsnIoPtr aip, AsnModulePtr amp)
{
	AsnTypePtr atp, next;
	Int2 token;

	next = amp->types;
	atp = amp->types;

	if ((token = AsnLexTMatchToken(aip)) != 0)    /* it's a reserved word */
		AsnIoErrorMsg(aip, 67, asnwords[token - 1], aip->linenumber);

	while (next != NULL)
	{
		atp = next;
		next = atp->next;
		if (StrMatch(atp->name, aip->word, aip->wordlen))     /* already used name */
		{
			if (! atp->resolved)     /* needs to be resolved yet, so OK */
				return atp;
			else                     /* already defined */
			{
				AsnIoErrorMsg(aip, 99, atp->name, aip->linenumber);
			}
		}
	}

	next = (AsnTypePtr) MemNew(sizeof(AsnType));
	next->tagclass = TAG_NONE;    /* default tag class */
	next->name = AsnLexSaveWord(aip);
	amp->lasttype++;       /* increment defined types isa */
	next->isa = amp->lasttype;

	if (atp == NULL)    /* first one */
		amp->types = next;
	else
		atp->next = next;

	return next;
}

/*****************************************************************************
*
*   AsnTypePtr AsnElementTypeNew(aip)
*
*****************************************************************************/
AsnTypePtr AsnElementTypeNew (AsnIoPtr aip)
{
	AsnTypePtr atp;

	atp = (AsnTypePtr) MemNew(sizeof(AsnType));
	atp->tagclass = TAG_NONE;    /* default tag class */
	if (aip != NULL)
		atp->name = AsnLexSaveWord(aip);

	return atp;
}

/*****************************************************************************
*
*   AsnModulePtr AsnCheckModule(amp, aip)
*
*****************************************************************************/
AsnModulePtr AsnCheckModule (AsnModulePtr amp, AsnIoPtr aip)
{
	AsnTypePtr atp;
	Boolean ok = TRUE;

	atp = amp->types;              /* check that everything got resolved */
	while (atp != NULL)
	{
		if (atp->resolved != TRUE)
		{
			if (atp->imported != TRUE)    /* only imported not resolved */
			{
				AsnIoErrorMsg(aip, 100,	atp->name, amp->modulename);
				ok = FALSE;
			}
			else
				AsnIoErrorMsg(aip, 101,	atp->name, amp->modulename);
		}

		if (atp->imported == TRUE)
		{
			if (atp->branch == NULL)
			{
				AsnIoErrorMsg(aip, 102,	atp->name, amp->modulename);
				ok = FALSE;
			}
		}

		atp = atp->next;
	}

	AsnSetTags(amp->types);      /* assign tags */

	if (ok)
		return amp;
	else
		return NULL;
}

/*****************************************************************************
*
*   void AsnSetTags(atp)
*   	automatically assign CONTEXT tags to named elements
*
*****************************************************************************/
void AsnSetTags (AsnTypePtr atp)
{
	int ctr = 0;
	Int2 isa;

	while (atp != NULL)
	{
		if (atp->type != NULL)
		{
			isa = atp->type->isa;
			switch (isa)
			{
				case SEQOF_TYPE:
				case SETOF_TYPE:      /* check subtypes */
				case SEQ_TYPE:
				case SET_TYPE:
				case CHOICE_TYPE:
					AsnSetTags((AsnTypePtr) atp->branch);
					break;
				default:
					break;
			}
		}
		if (atp->name != NULL)
		{
			if (IS_LOWER(*atp->name))      /* identifier, not Type */
			{
				if ((atp->tagclass == TAG_NONE) ||    /* assign it */
					(atp->tagclass == TAG_CONTEXT))
				{
					atp->tagclass = TAG_CONTEXT;
					atp->tagnumber = ctr;
					ctr++;
				}
			}
		}
		atp = atp->next;
	}
	return;
}

/*****************************************************************************
*
*   AsnValxNodePtr AsnValxNodeNew(anvp, type)
*
*****************************************************************************/
AsnValxNodePtr AsnValxNodeNew (AsnValxNodePtr avnp, Int2 type)
{
	AsnValxNodePtr newnode;

	newnode = (AsnValxNodePtr) MemNew((sizeof(AsnValxNode)));
	newnode->valueisa = type;
	if (avnp != NULL)   /* add to chain */
	{
		while (avnp->next != NULL)
			avnp = avnp->next;
		avnp->next = newnode;
	}
	return newnode;
}




