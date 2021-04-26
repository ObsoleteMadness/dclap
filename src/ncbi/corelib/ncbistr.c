/*   ncbistr.c
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
* File Name:  ncbistr.c
*
* Author:  Gish, Kans, Ostell, Schuler
*
* Version Creation Date:   3/4/91
*
* $Revision: 2.4 $
*
* File Description: 
*   	portable string routines
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 3/4/91   Kans        Stricter typecasting for GNU C and C++.
* 09-19-91 Schuler     Changed all types expressing sizes to size_t.
* 09-19-91 Schuler     Changed return type for compare functions to int.
* 09-19-91 Schuler     Changed all functions to _cdecl calling convention.
* 09-19-91 Schuler     Where possible, NCBI functions call the actual ANSI
*                       functions after checking for NULL pointers.
* 09-19-91 Schuler     Debug-class error posted on any NULL argument.
* 09-19-91 Schuler     StringSave() calls MemGet() instead of MemNew().
* 09-19-91 Schuler     StringSave(NULL) returns NULL.
* 10-17-91 Schuler     Removed ErrPost() calls on NULL arguments.
* 10-17-91 Schuler     Added Nlm_StringCnt(),Nlm_StringStr(),Nlm_StringTok()
* 11-18-91 Schuler     Added more ANSI-style functions
* 04-15-93 Schuler     Changed _cdecl to LIBCALL
* 05-27-93 Schuler     Added const qualifiers to match ANSI cognates
*
* ==========================================================================
*/
 
#include <ncbi.h>
#include <ncbiwin.h>

/* ClearDestString clears the destination string if the source is NULL. */
static Nlm_CharPtr NEAR Nlm_ClearDestString (Nlm_CharPtr to, size_t max)
{
  if (to != NULL && max > 0) {
    Nlm_MemSet (to, 0, max);
    *to = '\0';
  }
  return to;
}

size_t LIBCALL  Nlm_StringLen (const char *str)
{
	return str ? StrLen (str) : 0;
}

Nlm_CharPtr LIBCALL  Nlm_StringCpy (char FAR *to, const char FAR *from)
{
	return (to && from) ? StrCpy (to, from) : Nlm_ClearDestString (to, 1);
}

Nlm_CharPtr LIBCALL  Nlm_StringNCpy (char FAR *to, const char FAR *from, size_t max)
{
	return (to && from) ? StrNCpy (to, from, max) : Nlm_ClearDestString (to, max);
}

Nlm_CharPtr LIBCALL  Nlm_StringCat (char FAR *to, const char FAR *from)
{
	return (to && from) ? StrCat (to, from) : to;
}

Nlm_CharPtr LIBCALL  Nlm_StringNCat (char FAR *to, const char FAR *from, size_t max)
{       
    return (to && from) ? StrNCat (to, from, max) : to;
}

int LIBCALL  Nlm_StringCmp (const char FAR *a, const char FAR *b)
{
	return  (a && b) ? StrCmp (a, b) : 0;
}

int LIBCALL  Nlm_StringNCmp (const char FAR *a, const char FAR *b, size_t max)
{
	return (a && b) ? StrNCmp (a, b, max) : 0;
}

int LIBCALL  Nlm_StringICmp (const char FAR *a, const char FAR *b)
{
	return (a && b) ? Nlm_StrICmp (a, b) : 0;
}

int LIBCALL  Nlm_StringNICmp (const char FAR *a, const char FAR *b, size_t max)
{
	return (a && b) ? Nlm_StrNICmp (a, b, max) : 0;
}

Nlm_CharPtr LIBCALL  Nlm_StringChr (const char FAR *str, int chr)
{
	return str ? Nlm_StrChr(str,chr) : NULL;
}

Nlm_CharPtr LIBCALL  Nlm_StringRChr (const char FAR *str, int chr)
{
	return str ? Nlm_StrRChr(str,chr) : NULL;
}

size_t LIBCALL  Nlm_StringSpn (const char FAR *a, const char FAR *b)
{
	return (a && b) ? Nlm_StrSpn (a, b) : 0;
}

size_t LIBCALL  Nlm_StringCSpn (const char FAR *a, const char FAR *b)
{
	return (a && b) ? Nlm_StrCSpn (a, b) : 0;
}

Nlm_CharPtr LIBCALL  Nlm_StringPBrk (const char FAR *a, const char FAR *b)
{
	return (a && b) ? Nlm_StrPBrk (a, b) : NULL;
}

Nlm_CharPtr LIBCALL  Nlm_StringStr (const char FAR *str1, const char FAR *str2)
{
	return (str1 && str2) ? Nlm_StrStr(str1,str2) : NULL;
}

Nlm_CharPtr LIBCALL  Nlm_StringTok (char FAR *str1, const char FAR *str2)
{
    return str2 ? Nlm_StrTok(str1,str2) : NULL;
}

Nlm_CharPtr LIBCALL  Nlm_StringMove (char FAR *to, const char FAR *from)
{
    return (to && from) ? Nlm_StrMove (to, from) : to;
}

Nlm_CharPtr LIBCALL  Nlm_StringSave (const char FAR *from)
{
    return from ? Nlm_StrSave (from) : NULL;
}

size_t LIBCALL  Nlm_StringCnt (const char FAR *str, const char FAR *list)
{
    return (str && list) ? Nlm_StrCnt(str,list) : 0;
}


int LIBCALL  Nlm_StrICmp (const char FAR *a, const char FAR *b)
{
	int diff, done;

    if (a == b)   return 0;

	done = 0;
	while (! done)
	{
		diff = TO_UPPER(*a) - TO_UPPER(*b);
		if (diff)
			return (Nlm_Int2) diff;
		if (*a == '\0')
			done = 1;
		else
		{
			a++; b++;
		}
	}
	return 0;
}

int LIBCALL  Nlm_StrNICmp (const char FAR *a, const char FAR *b, size_t max)
{
	int diff, done;
	
    if (a == b)   return 0;

	done = 0;
	while (! done)
	{
		diff = TO_UPPER(*a) - TO_UPPER(*b);
		if (diff)
			return (Nlm_Int2) diff;
		if (*a == '\0')
			done = 1;
		else
		{
			a++; b++; max--;
			if (! max)
				done = 1;
		}
	}
	return 0;
}

Nlm_CharPtr LIBCALL  Nlm_StrSave (const char FAR *from)
{
	size_t len;
	Nlm_CharPtr to;

	len = Nlm_StringLen(from);
	if ((to = (Nlm_CharPtr) Nlm_MemGet(len +1, FALSE)) != NULL) {
    	Nlm_MemCpy (to, from, len +1);
	}
	return to;
}

Nlm_CharPtr LIBCALL  Nlm_StrMove (char FAR *to, const char FAR *from)
{
	while (*from != '\0')
	{
		*to = *from;
		to++; from++;
	}
	*to = '\0';
	return to;
}

size_t LIBCALL  Nlm_StrCnt (const char FAR *s, const char FAR *list)
{
	size_t	cmap[1<<CHAR_BIT];
	Nlm_Byte	c;
	size_t	u, cnt;
	const Nlm_Byte *bs = (const Nlm_Byte*)s;
	const Nlm_Byte *blist = (const Nlm_Byte*)list;

	if (s == NULL || list == NULL)
		return 0;

	for (u = 0; u < DIM(cmap); ++u)
		cmap[u] = 0;
	while (*blist != '\0')
		++cmap[*blist++];

	blist = (Nlm_BytePtr)cmap;

	cnt = 0;
	while ((c = *bs++) != '\0')
		cnt += blist[c];

	return cnt;
}

/*  -------------------- MeshStringICmp() --------------------------------
 *  MeshStringICmp compares strings where / takes precedence to space.
 */

Nlm_Int2 LIBCALL Nlm_MeshStringICmp (const char FAR *str1, const char FAR *str2)
{
	Nlm_Char  ch1, ch2;

	if (str1 == NULL)
	{
		if (str2 == NULL)
			return (Nlm_Int2)0;
		else
			return (Nlm_Int2)1;
	}
	else if (str2 == NULL)
		return (Nlm_Int2)-1;

	while ((*str1 >= ' ') && (*str2 >= ' ') && (TO_LOWER(*str1) == TO_LOWER(*str2)))
	{
		str1++; str2++;
	}

	ch1 = *str1;
	ch2 = *str2;
	if ((ch1 < ' ') && (ch2 < ' '))
		return (Nlm_Int2)0;
	else if (ch1 < ' ')
		return (Nlm_Int2)-1;
	else if (ch2 < ' ')
		return (Nlm_Int2)1;

	if (ch1 == '/')
	  ch1 = '\31';
	if (ch2 == '/')
	  ch2 = '\31';

	if (TO_LOWER (ch1) > TO_LOWER (ch2))
	  return (Nlm_Int2)1;
	else if (TO_LOWER (ch1) < TO_LOWER (ch2))
	  return (Nlm_Int2)(-1);
	else
	  return (Nlm_Int2)0;
}
