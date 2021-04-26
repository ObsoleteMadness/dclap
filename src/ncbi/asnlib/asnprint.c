/*  asnprint.c
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
* File Name:  asnprint.c
*
* Author:  James Ostell
*
* Version Creation Date: 3/4/91
*
* $Revision: 2.9 $
*
* File Description:
*   Routines for printing ASN.1 value notation (text) messages and
*     ASN.1 module specifications
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 3/4/91   Kans        Stricter typecasting for GNU C and C++
*
* ==========================================================================
*/

/*****************************************************************************
*
*   asnprint.c
*   	print routines for asn1 objects
*
*****************************************************************************/

#include "asnbuild.h"

/*****************************************************************************
*
*   void AsnTxtWrite(aip, atp, valueptr)
*
*****************************************************************************/
Boolean LIBCALL  AsnTxtWrite (AsnIoPtr aip, AsnTypePtr atp, DataValPtr dvp)
{
	Int2 isa;
	AsnTypePtr atp2;
	AsnValxNodePtr avnp;
	Boolean done, terminalvalue, firstvalue;

	terminalvalue = TRUE;   /* most are terminal values */
	if ((! aip->indent_level) && (aip->typestack[0].type == NULL))
		firstvalue = TRUE;    /* first call to this routine */
	else
		firstvalue = FALSE;

	if (! AsnTypeValidateOut(aip, atp, dvp))
		return FALSE;

	atp2 = AsnFindBaseType(atp);
	isa = atp2->type->isa;
	if (ISA_STRINGTYPE(isa))
		isa = GENERALSTRING_TYPE;
	
	if (((isa == SEQ_TYPE) || (isa == SET_TYPE) ||
		 (isa == SEQOF_TYPE) || (isa == SETOF_TYPE))
		 && (dvp->intvalue == END_STRUCT))
	{
		AsnPrintCloseStruct(aip, atp);
		return TRUE;
	}

	if (! aip->first[aip->indent_level])
		AsnPrintNewLine(aip);
	else
		aip->first[aip->indent_level] = FALSE;

	atp2 = atp;
	if (firstvalue)       /* first item, need ::= */
	{
		while ((atp2->name == NULL) || (IS_LOWER(*atp2->name)))
			atp2 = atp2->type;    /* find a Type Reference */
	}

	if (atp2->name != NULL)
	{
	 	AsnPrintString(atp2->name, aip);   /* put the element name */
		if (IS_LOWER(*atp2->name))
		 	AsnPrintChar(' ', aip);
		else
			AsnPrintString(" ::= ", aip);
	}

	if (isa == CHOICE_TYPE)     /* show nothing but name on same line */
	{
		if ((aip->type_indent))
		{
			isa = AsnFindBaseIsa(aip->typestack[aip->type_indent - 1].type);
			if ((isa != SEQOF_TYPE) && (isa != SETOF_TYPE))
			{
				AsnPrintIndent(TRUE, aip);
				AsnTypeSetIndent(TRUE, aip, atp);
				AsnPrintNewLine(aip);
			}
			else
				AsnTypeSetIndent(TRUE, aip, atp);
		}
		aip->first[aip->indent_level] = TRUE;
		return TRUE;
	}

	switch (isa)
	{
		case SEQ_TYPE:
		case SET_TYPE:
		case SEQOF_TYPE:
		case SETOF_TYPE:
			if (dvp->intvalue == START_STRUCT)   /* open brace */
				AsnPrintOpenStruct(aip, atp);
			else
			{
				AsnIoErrorMsg(aip, 18 );
				return FALSE;
			}
			terminalvalue = FALSE;
			break;
		case BOOLEAN_TYPE:
			AsnPrintBoolean(dvp->boolvalue, aip);
			break;
		case INTEGER_TYPE:
		case ENUM_TYPE:
			atp2 = AsnFindBaseType(atp);  /* check for names */
			avnp = (AsnValxNodePtr) atp2->branch;
			done = FALSE;
			while (avnp != NULL)
			{
				if (dvp->intvalue == avnp->intvalue)
				{
					AsnPrintString(avnp->name, aip);
					done = TRUE;
					avnp = NULL;
				}
				else
					avnp = avnp->next;
			}
			if (! done)    /* no name */
				AsnPrintInteger(dvp->intvalue, aip);
			break;
		case REAL_TYPE:
			AsnPrintReal(dvp->realvalue, aip);
			break;
		case GENERALSTRING_TYPE:
			AsnPrintChar('\"', aip);
			if (! AsnPrintString((CharPtr) dvp->ptrvalue, aip))
				return FALSE;
			AsnPrintChar('\"', aip);
			break;
		case NULL_TYPE:
			AsnPrintString("NULL", aip);
			break;
		case OCTETS_TYPE:
			AsnPrintOctets((ByteStorePtr) dvp->ptrvalue, aip);
			break;
		case STRSTORE_TYPE:
			if (! AsnPrintStrStore((ByteStorePtr) dvp->ptrvalue, aip))
				return FALSE;
			break;
		default:
			AsnIoErrorMsg(aip, 19, atp->name);
			return FALSE;
	}

	if ((terminalvalue) && (aip->type_indent))   /* pop out of choice nests */
	{
		if (AsnFindBaseIsa(aip->typestack[aip->type_indent - 1].type) == CHOICE_TYPE)
		{
			if (aip->type_indent >= 2)
				isa = AsnFindBaseIsa(aip->typestack[aip->type_indent - 2].type);
			else
				isa = NULL_TYPE;    /* just fake it */
			if ((isa != SETOF_TYPE) && (isa != SEQOF_TYPE))
				AsnPrintIndent(FALSE, aip);
			AsnTypeSetIndent(FALSE, aip, atp);
		}
	}
	return TRUE;														   
}

/*****************************************************************************
*
*   void AsnPrintModule(amp, aip)
*
*****************************************************************************/
void AsnPrintModule (AsnModulePtr amp, AsnIoPtr aip)

{
	AsnTypePtr atp;
	Boolean firstone;
	CharPtr from;
	
	AsnPrintString(amp->modulename, aip);
	AsnPrintString(" DEFINITIONS ::=", aip);
	AsnPrintNewLine(aip);
	AsnPrintString("BEGIN", aip);
	AsnPrintNewLine(aip);
	AsnPrintNewLine(aip);

	atp = amp->types;		    /* check for EXPORTS */
	firstone = TRUE;
	while (atp != NULL)
	{
		if (atp->exported == TRUE)
		{
			if (firstone)
				AsnPrintString("EXPORTS ", aip);
			else
			{
				AsnPrintString(" ,", aip);
				AsnPrintNewLine(aip);
				AsnPrintString("        ", aip);
			}
			AsnPrintString(atp->name, aip);
			firstone = FALSE;
		}
		atp = atp->next;
	}
	if (! firstone)            /* got at least one */
	{
		AsnPrintString(" ;", aip);
		AsnPrintNewLine(aip);
		AsnPrintNewLine(aip);
	}

	atp = amp->types;		    /* check for IMPORTS */
	firstone = TRUE;
	from = NULL;
	while (atp != NULL)
	{
		if (atp->imported == TRUE)
		{
			if (firstone)
				AsnPrintString("IMPORTS ", aip);
			else
			{
				if (StringCmp((CharPtr) atp->branch, from))    /* new FROM */
				{
					AsnPrintString(" FROM ", aip);
					AsnPrintString(from, aip);
				}
				else
					AsnPrintString(" ,", aip);
				AsnPrintNewLine(aip);
				AsnPrintString("        ", aip);
			}
			AsnPrintString(atp->name, aip);
			firstone = FALSE;
			from = (CharPtr) atp->branch;
		}
		atp = atp->next;
	}
	if (! firstone)            /* got at least one */
	{
		AsnPrintString(" FROM ", aip);
		AsnPrintString(from, aip);
		AsnPrintString(" ;", aip);
		AsnPrintNewLine(aip);
		AsnPrintNewLine(aip);
	}

	atp = amp->types;
	while (atp != NULL)
	{
		if (! atp->imported)
		{
			AsnPrintString(atp->name, aip);
			AsnPrintString(" ::= ", aip);
			AsnPrintType(atp, aip);
			AsnPrintNewLine(aip);
			AsnPrintNewLine(aip);
		}
		atp = atp->next;
	}
	AsnPrintString("END", aip);
	AsnPrintNewLine(aip);
	return;
}

/*****************************************************************************
*
*   void AsnPrintType(atp, aip)
*   	prints a type starting at current line position
*   	(assumes name already printed)
*
*****************************************************************************/
void AsnPrintType (AsnTypePtr atp, AsnIoPtr aip)

{
	AsnValxNodePtr avnp;
	AsnTypePtr atp2;
	Boolean first;

	if (atp->tagclass != TAG_NONE)     /* print tag, if any */
	{
		AsnPrintChar('[', aip);
		AsnPrintChar(' ', aip);
		switch (atp->tagclass)
		{
			case TAG_UNIVERSAL:
				AsnPrintString("UNIVERSAL ", aip);
				break;
			case TAG_APPLICATION:
				AsnPrintString("APPLICATION ", aip);
				break;
			case TAG_PRIVATE:
				AsnPrintString("PRIVATE ", aip);
				break;
			default:      /* context dependent, do nothing */
				break;
		}
		AsnPrintInteger((Int4)atp->tagnumber, aip);
		AsnPrintChar(' ', aip);
		AsnPrintChar(']', aip);
		AsnPrintChar(' ', aip);

		if (atp->implicit)
			AsnPrintString("IMPLICIT ", aip);
	}

	AsnPrintString(atp->type->name, aip);   /* print the type name */

	if (atp->branch != NULL)       /* sub types ? */
	{
		switch (atp->type->isa)
		{
			case SETOF_TYPE:
			case SEQOF_TYPE:
				AsnPrintChar(' ', aip);
				AsnPrintType((AsnTypePtr) atp->branch, aip);
				break;
			case INTEGER_TYPE:
			case ENUM_TYPE:
				AsnPrintChar(' ', aip);
				AsnPrintOpenStruct(aip, atp);
				avnp = (AsnValxNodePtr)atp->branch;
				first = TRUE;
				aip->first[aip->indent_level] = FALSE;
				while (avnp != NULL)
				{
					if (! first)
						AsnPrintNewLine(aip);
					else
						first = FALSE;
					AsnPrintString(avnp->name, aip);
					AsnPrintChar(' ', aip);
					AsnPrintChar('(', aip);
					AsnPrintInteger(avnp->intvalue, aip);
					AsnPrintChar(')', aip);
					avnp = avnp->next;
				}
				AsnPrintCloseStruct(aip, atp);
				break;
			case SEQ_TYPE:
			case SET_TYPE:
			case CHOICE_TYPE:
				AsnPrintChar(' ', aip);
				AsnPrintOpenStruct(aip, atp);
				atp2 = (AsnTypePtr) atp->branch;
				first = TRUE;
				aip->first[aip->indent_level] = FALSE;
				while (atp2 != NULL)
				{
					if (! first)
						AsnPrintNewLine(aip);
					else
						first = FALSE;

					if (atp2->name != NULL)
					{
						AsnPrintString(atp2->name, aip);
						AsnPrintChar(' ', aip);
					}
					AsnPrintType(atp2, aip);
					atp2 = atp2->next;
				}
				AsnPrintCloseStruct(aip, atp);
				break;
			default:			/* everything else */
				break;          /* do nothing */
		}
	}
	
	if (atp->optional)
		AsnPrintString(" OPTIONAL", aip);

	if (atp->hasdefault)
	{
		AsnPrintString(" DEFAULT ", aip);
		avnp = atp->defaultvalue;
		while (! (VALUE_ISA_DEFAULT(avnp->valueisa)))
			avnp = avnp->next;
		switch (avnp->valueisa)
		{
			case VALUE_ISA_PTR:
				AsnPrintChar('\"', aip);
				AsnPrintString(avnp->name, aip);
				AsnPrintChar('\"', aip);
				break;
			case VALUE_ISA_BOOL:
				AsnPrintBoolean((Boolean)avnp->intvalue, aip);
				break;
			case VALUE_ISA_INT:
				AsnPrintInteger(avnp->intvalue, aip);
				break;
			case VALUE_ISA_REAL:
				AsnPrintReal(avnp->realvalue, aip);
				break;
			default:
				AsnPrintString("Error", aip);
				break;
		}
	}
}

/*****************************************************************************
*
*   Boolean AsnPrintStrStore(bsp, aip)
*
*****************************************************************************/
Boolean AsnPrintStrStore (ByteStorePtr bsp, AsnIoPtr aip)

{
	Char buf[101];
	Uint4 len, tlen;

	if (aip->type & ASNIO_CARRIER)           /* pure iterator */
		return TRUE;

	BSSeek(bsp, 0, SEEK_SET);      /* seek to start */
	len = BSLen(bsp);
	AsnPrintChar('\"', aip);
	while (len)
	{
		if (len < 100)
			tlen = len;
		else
			tlen = 100;
		BSRead(bsp, buf, tlen);
		buf[tlen] = '\0';
		if (! AsnPrintString(buf, aip))
			return FALSE;
		len -= tlen;
	}
	AsnPrintChar('\"', aip);
	return TRUE;
}
/*****************************************************************************
*
*   void AsnPrintReal(realvalue, aip)
*
*****************************************************************************/
void AsnPrintReal (FloatHi realvalue, AsnIoPtr aip)

{
	FloatHi thelog, mantissa;
	int characteristic;
	int	ic;
	long	im;
	char tbuf[30];
	Boolean minus;

	if (aip->type & ASNIO_CARRIER)           /* pure iterator */
		return;

	if (realvalue == 0.0)
	{
		ic = 0;
		im = 0;
	}
	else
	{
		if (realvalue < 0.0)
		{
			minus = TRUE;
			realvalue = -realvalue;
		}
		else
			minus = FALSE;

		thelog = log10((double)realvalue);
		if (thelog >= 0.0)
			characteristic = 8 - (int)thelog;/* give it 9 significant digits */
		else
			characteristic = 8 + (int)ceil(-thelog);

		mantissa = realvalue * Nlm_Powi((double)10., characteristic);
		ic = -characteristic; /* reverse direction */
		im = (long) mantissa;

		/* strip trailing 0 */
		while ((im % 10L) == 0L)
		{
			im /= 10L;
			ic++;
		}

		if (minus)
			im = -im;
	}
	sprintf(tbuf, "{ %ld, 10, %d }", im, ic);
	AsnPrintString(tbuf, aip);
	return;
}

/*****************************************************************************
*
*   void AsnPrintInteger(theInt, aip)
*
*****************************************************************************/
void AsnPrintInteger (Int4 theInt, AsnIoPtr aip)

{
	char tbuf[10];

	if (aip->type & ASNIO_CARRIER)           /* pure iterator */
		return;

	sprintf(tbuf, "%ld", (long)theInt);
	AsnPrintString(tbuf, aip);
	return;
}

/*****************************************************************************
*
*   void AsnPrintChar(theChar, aip)
*   	print a single character
*
*****************************************************************************/
void AsnPrintChar (char theChar, AsnIoPtr aip)

{
	if (aip->type & ASNIO_CARRIER)           /* pure iterator */
		return;

	*(aip->linebuf + aip->linepos) = theChar;
	aip->linepos++;
	aip->offset++;
	return;
}

/*****************************************************************************
*
*   void AsnPrintBoolean(value, aip)
*
*****************************************************************************/
void AsnPrintBoolean (Boolean value, AsnIoPtr aip)

{
	if (aip->type & ASNIO_CARRIER)           /* pure iterator */
		return;

	if (value)
		AsnPrintString("TRUE", aip);
	else
		AsnPrintString("FALSE", aip);
	return;
}

/*****************************************************************************
*
*   void AsnPrintOctets(ssp, aip)
*
*****************************************************************************/
void AsnPrintOctets (ByteStorePtr ssp, AsnIoPtr aip)

{
	Int2 value, tval, ctr;
	Char buf[101];

	if (aip->type & ASNIO_CARRIER)           /* pure iterator */
		return;

	AsnPrintChar('\'', aip);

	BSSeek(ssp, 0, SEEK_SET);   /* go to start of bytestore */
	ctr = 0;
	buf[100] = '\0';

					/* break it up into lines if necessary */
	while ((value = BSGetByte(ssp)) != -1)
	{
		tval = value / 16;
		if (tval < 10)
			buf[ctr] = (Char)(tval + '0');
		else
			buf[ctr] = (Char)(tval - 10 + 'A');
		ctr++;
		tval = value - (tval * 16);
		if (tval < 10)
			buf[ctr] = (Char)(tval + '0');
		else
			buf[ctr] = (Char)(tval - 10 + 'A');
		ctr++;
		if (ctr == 100)
		{
		    AsnPrintString(buf, aip);
			ctr = 0;
		}
	}
	if (ctr)
	{
		buf[ctr] = '\0';
		AsnPrintString(buf, aip);
	}

	AsnPrintChar('\'', aip);
	AsnPrintChar('H', aip);
	return;
}

/*****************************************************************************
*
*   void AsnPrintIndent(increase, aip)
*      increase or decrease indent level
*
*****************************************************************************/
void AsnPrintIndent (Boolean increase, AsnIoPtr aip)

{
	Int1 offset,
		 curr_indent;
	BoolPtr tmp;
	int decr, isa;
	

	if (increase)
	{
		aip->indent_level++;
		curr_indent = aip->indent_level;
		if (curr_indent == aip->max_indent)   /* expand indent levels */
		{
			tmp = aip->first;
			aip->first = (BoolPtr) MemNew((sizeof(Boolean) * (aip->max_indent + 10)));
			MemCopy(aip->first, tmp, (size_t)(sizeof(Boolean) * aip->max_indent));
			MemFree(tmp);
			aip->max_indent += 10;
		}
		aip->first[curr_indent] = TRUE;     /* set to first time */
		offset = curr_indent * aip->tabsize;

		if (! (aip->type & ASNIO_CARRIER))
		{
			while (aip->linepos < offset)
			{
				*(aip->linebuf + aip->linepos) = ' ';
				aip->linepos++;
			}
			aip->offset = aip->linepos + (aip->linebuf - (CharPtr)aip->buf);
		}
	}
	else
	{
		offset = aip->indent_level * aip->tabsize;
		curr_indent = aip->type_indent;
		decr = 1;   /* always backup indent for named element */
		do
		{
			if (aip->indent_level)
				aip->indent_level -= decr;
			if (curr_indent)
				curr_indent--;
			isa = NULL_TYPE;        /* fake key */
			if ((aip->indent_level) && (curr_indent))
			{
				isa = AsnFindBaseIsa(aip->typestack[curr_indent - 1].type);
				if (aip->typestack[curr_indent-1].type->name != NULL)
					decr = 1;     /* indent for named choices as elements */
				else
					decr = 0;     /* not referenced choice objects */
			}
		} while (isa == CHOICE_TYPE);

		if (aip->linepos == offset)    /* nothing written yet */
		{
			curr_indent = aip->indent_level * aip->tabsize;
			while (offset >= curr_indent)
			{
				offset--;
				if (! (aip->type & ASNIO_CARRIER))
				{
					if ((offset >= 0) && (aip->linebuf[offset] != ' '))
						curr_indent = 127;
				}
			}
			offset++;
			aip->linepos = offset;
			aip->offset = aip->linepos + (aip->linebuf - (CharPtr)aip->buf);
		}
		if (! aip->indent_level)   /* level 0 - no commas */
			aip->first[0] = TRUE;
	}
	return;
}

/*****************************************************************************
*
*   void AsnPrintNewLine(aip)
*       end a line in the print buffer
*       indent to the proper level on the next line
*
*****************************************************************************/
void AsnPrintNewLine (AsnIoPtr aip)

{
	Int1 tpos, indent;
	CharPtr tmp;
	Boolean do_print = TRUE;

	if (aip->linepos == 0)     /* nothing in buffer yet */
		return;
		
	if (! (aip->type & ASNIO_CARRIER))           /* really printing */
	{
		tpos = aip->indent_level * aip->tabsize;
		if (tpos == aip->linepos)   /* just an empty indent? */
		{
			do_print = FALSE;   /* assume that's the case */
			for (tmp = aip->linebuf; tpos != 0; tpos--, tmp++)
			{
				if (*tmp != ' ')
				{
					do_print = TRUE;  /* set sentinel */
					break;
				}
			}
		}

		if (do_print)   /* not an empty indent */
		{
			tmp = aip->linebuf + aip->linepos;
			if (aip->first[aip->indent_level] == FALSE)    /* not first line of struct */
			{
				*tmp = ' '; tmp++;						   /* add commas */
				*tmp = ','; tmp++;
			}
			else if (aip->linepos)         /* is first line, remove trailing blanks */
			{								/* if just indented */
				tmp--;
				while ((*tmp == ' ') && (tmp > aip->linebuf))
					tmp--;
				tmp++;
			}
			*tmp = '\0';
			aip->linepos = tmp - aip->linebuf;
			aip->offset = tmp - (CharPtr)aip->buf;

			AsnIoPuts(aip);
		}
	}

	if ((do_print) && (aip->indent_level))    /* level 0 never has commas */
		aip->first[aip->indent_level] = FALSE;

	if (! (aip->type & ASNIO_CARRIER))     /* really printing */
	{
		tmp = aip->linebuf;
		indent = aip->indent_level * aip->tabsize;
		for (tpos = 0; tpos < indent; tpos++, tmp++)
			*tmp = ' ';
		aip->linepos = tpos;
		aip->offset += tpos;
	}
	return;
}
/*****************************************************************************
*
*   Boolean AsnPrintString(str, aip)
*
*****************************************************************************/
Boolean AsnPrintString (CharPtr the_string, AsnIoPtr aip)

{
	Uint4 stringlen;
	register int templen;
	Int1 first = 1;
	register CharPtr current, str;
	Boolean indent_state;
	int bad_char;

	if (aip->type & ASNIO_CARRIER)           /* pure iterator */
		return TRUE;

	str = the_string;
	stringlen = StringLen(str);
	indent_state = aip->first[aip->indent_level];


					/* break it up into lines if necessary */
	while (stringlen)
	{
		if (! first)               /* into multiple lines */
		{
			aip->first[aip->indent_level] = TRUE;   /* no commas */
			AsnPrintNewLine(aip);
			aip->offset -= aip->linepos;
			aip->linepos = 0;
		}
		first = 0;

		templen = (int)(aip->linelength - aip->linepos);

		if (stringlen <= (Uint4)templen)     /* it fits in remaining space */
			templen = (int) stringlen;
		else
			templen = AsnPrintGetWordBreak(str, templen);

		current = aip->linebuf + aip->linepos;
		stringlen -= (Uint4)templen;
		aip->linepos += templen;
		aip->offset += templen;
		while (templen)
		{
			if ((*str < ' ') || (*str > '~'))
			{
				bad_char = (int)(*str);
				*str = '\0';
				AsnIoErrorMsg(aip, 106, bad_char, the_string);
				*str = '#';   /* replace with # */
			}
			*current = *str;
			if (*str == '\"')     /* must double quotes */
			{
				current++; aip->linepos++; aip->offset++;
				*current = '\"';
			}
			current++; str++; templen--;
		}
	}
	aip->first[aip->indent_level] = indent_state;   /* reset indent state */
	return TRUE;
}

/*****************************************************************************
*
*   void AsnPrintCharBlock(str, aip)
*      prints string on line if there is room
*      if not prints on next line with no indent.
*
*****************************************************************************/
void AsnPrintCharBlock (CharPtr str, AsnIoPtr aip)

{
	Uint4 stringlen;
	Boolean indent_state;
	Int1 templen;
	CharPtr current;

	if (aip->type & ASNIO_CARRIER)           /* pure iterator */
		return;

	stringlen = StringLen(str);
	templen = (Int1)(aip->linelength - aip->linepos);
	indent_state = aip->first[aip->indent_level];

	if (stringlen > (Uint4)templen)     /* won't fit on line */
	{
		aip->first[aip->indent_level] = TRUE;   /* no commas */
		AsnPrintNewLine(aip);
		aip->linepos = 0;      /* no indent on broken string */
	}

	current = aip->linebuf + aip->linepos;
	MemCopy(current, str, (size_t)stringlen);
	aip->linepos += (Int2) stringlen;
	aip->offset += (Int2) stringlen;
	aip->first[aip->indent_level] = indent_state;   /* reset indent state */
	return;
}

/*****************************************************************************
*
*   int AsnPrintGetWordBreak(str, maxlen)
*       return length (<= maxlen) of str to next white space
*
*****************************************************************************/
int AsnPrintGetWordBreak (CharPtr str, int maxlen)

{
	CharPtr tmp;
	int len;
	Uint4 stringlen;

	stringlen = StringLen(str);
	if (stringlen <= (Uint4)maxlen)
		return (int) stringlen;

	tmp = str + maxlen;    /* point just PAST the end of region */
	len = maxlen + 1;
	while ((len) && (! IS_WHITESP(*tmp)))
	{
		len--; tmp--;
	}
	while ((len) && (IS_WHITESP(*tmp)))
	{
		len--;             /* move past white space */
		tmp--;
	}
	if (len < 1)         /* never found any whitespace or only 1 space */
		len = maxlen;    /* have to break a word */

	return len;
}

/*****************************************************************************
*
*   AsnPrintOpenStruct(aip, atp)
*
*****************************************************************************/
void AsnPrintOpenStruct (AsnIoPtr aip, AsnTypePtr atp)

{
	AsnPrintChar('{', aip);
	AsnPrintIndent(TRUE, aip);
	AsnTypeSetIndent(TRUE, aip, atp);
	AsnPrintNewLine(aip);
	aip->first[aip->indent_level] = TRUE;
	return;
}

/*****************************************************************************
*
*   AsnPrintCloseStruct(aip, atp)
*
*****************************************************************************/
void AsnPrintCloseStruct (AsnIoPtr aip, AsnTypePtr atp)

{
	AsnPrintChar(' ', aip);
	AsnPrintChar('}', aip);
	AsnPrintIndent(FALSE, aip);
	AsnTypeSetIndent(FALSE, aip, atp);
	return;
}
