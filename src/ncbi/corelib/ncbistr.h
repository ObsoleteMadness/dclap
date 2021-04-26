/*   ncbistr.h
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
* File Name:  ncbistr.h
*
* Author:  Gish, Kans, Ostell, Schuler
*
* Version Creation Date:   1/1/91
*
* $Revision: 2.3 $
*
* File Description: 
*   	prototypes for portable string routines
*
* Modifications:
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 09-19-91 Schuler     Added macros to alias actual ANSI string functions
* 04-15-93 Schuler     Changed _cdecl to LIBCALL
* 05-27-93 Schuler     Added const qualifiers to match ANSI cognates
*
* ==========================================================================
*/

#ifndef _NCBISTR_
#define _NCBISTR_

#ifdef __cplusplus
extern "C" {
#endif

int LIBCALL Nlm_StrICmp PROTO((const char FAR *a, const char FAR *b));
int LIBCALL Nlm_StrNICmp PROTO((const char FAR *a, const char FAR *b, size_t max));
Nlm_CharPtr LIBCALL Nlm_StrMove PROTO((char FAR *to, const char FAR *from));
Nlm_CharPtr LIBCALL Nlm_StrSave PROTO((const char FAR *from));
size_t LIBCALL Nlm_StrCnt PROTO((const char FAR *str, const char FAR *list));

size_t LIBCALL Nlm_StringLen PROTO((const char *str));
Nlm_CharPtr LIBCALL Nlm_StringCpy PROTO((char FAR *to, const char FAR *from));
Nlm_CharPtr LIBCALL Nlm_StringNCpy PROTO((char FAR *to, const char FAR *from, size_t max));
Nlm_CharPtr LIBCALL Nlm_StringCat PROTO((char FAR *to, const char FAR *from));
Nlm_CharPtr LIBCALL Nlm_StringNCat PROTO((char FAR *to, const char FAR *from, size_t max));
int LIBCALL Nlm_StringCmp PROTO((const char FAR *a, const char FAR *b));
int LIBCALL Nlm_StringNCmp PROTO((const char FAR *a, const char FAR *b, size_t max));
int LIBCALL Nlm_StringICmp PROTO((const char FAR *a, const char FAR *b));
int LIBCALL Nlm_StringNICmp PROTO((const char FAR *a, const char FAR *b, size_t max));
Nlm_CharPtr LIBCALL Nlm_StringChr PROTO((const char FAR *str, int chr));
Nlm_CharPtr LIBCALL Nlm_StringRChr PROTO((const char FAR *str, int chr));
Nlm_CharPtr LIBCALL Nlm_StringPBrk PROTO((const char FAR *str1, const char FAR *str2));
size_t LIBCALL Nlm_StringSpn PROTO((const char FAR *str1, const char FAR *str2));
size_t LIBCALL Nlm_StringCSpn PROTO((const char FAR *str1, const char FAR *str2));
Nlm_CharPtr LIBCALL Nlm_StringStr PROTO((const char FAR *str1, const char FAR *str2));
Nlm_CharPtr LIBCALL Nlm_StringTok PROTO((char FAR *str1, const char FAR *str2));
Nlm_CharPtr LIBCALL Nlm_StringMove PROTO((char FAR *to, const char FAR *from));
Nlm_CharPtr LIBCALL Nlm_StringSave PROTO((const char FAR *from));
size_t LIBCALL Nlm_StringCnt PROTO((const char FAR *str, const char FAR *list));

Nlm_Int2 LIBCALL Nlm_MeshStringICmp PROTO((const char FAR *str1, const char FAR *str2));

/* aliases for ANSI functions */
#ifdef WIN16
#define Nlm_StrLen	_fstrlen
#define Nlm_StrCpy	_fstrcpy
#define Nlm_StrNCpy	_fstrncpy
#define Nlm_StrCat	_fstrcat
#define Nlm_StrNCat	_fstrncat
#define Nlm_StrCmp	_fstrcmp
#define Nlm_StrNCmp	_fstrncmp
#define Nlm_StrChr	_fstrchr
#define Nlm_StrRChr	_fstrrchr
#define Nlm_StrCSpn	_fstrcspn
#define Nlm_StrNSet	_fstrnset
#define Nlm_StrPBrk	_fstrpbrk
#define Nlm_StrSet	_fstrset
#define Nlm_StrSpn	_fstrspn
#define Nlm_StrStr	_fstrstr
#define Nlm_StrTok	_fstrtok
#else
#define Nlm_StrLen	strlen
#define Nlm_StrCpy	strcpy
#define Nlm_StrNCpy	strncpy
#define Nlm_StrCat	strcat
#define Nlm_StrNCat	strncat
#define Nlm_StrCmp	strcmp
#define Nlm_StrNCmp	strncmp
#define Nlm_StrChr	strchr
#define Nlm_StrRChr	strrchr
#define Nlm_StrCSpn	strcspn
#define Nlm_StrNSet	strnset
#define Nlm_StrPBrk	strpbrk
#define Nlm_StrSet	strset
#define Nlm_StrSpn	strspn
#define Nlm_StrStr	strstr
#define Nlm_StrTok	strtok
#endif

#define StrLen	Nlm_StrLen
#define StrCpy	Nlm_StrCpy
#define StrNCpy	Nlm_StrNCpy
#define StrCat	Nlm_StrCat
#define StrNCat	Nlm_StrNCat
#define StrCmp	Nlm_StrCmp
#define StrNCmp	Nlm_StrNCmp
#define StrICmp Nlm_StrICmp
#define StrNICmp Nlm_StrNICmp
#define StrChr	Nlm_StrChr
#define StrRChr	Nlm_StrRChr
#define StrCSpn	Nlm_StrCSpn
#define StrNSet	Nlm_StrNSet
#define StrPBrk	Nlm_StrPBrk
#define StrSet	Nlm_StrSet
#define StrSpn	Nlm_StrSpn
#define StrStr	Nlm_StrStr
#define StrTok	Nlm_StrTok
#define StrMove Nlm_StrMove
#define StrSave Nlm_StrSave
#define StrCnt  Nlm_StrCnt

#define StringLen	Nlm_StringLen
#define StringCpy	Nlm_StringCpy
#define StringNCpy	Nlm_StringNCpy
#define StringCat	Nlm_StringCat
#define StringNCat	Nlm_StringNCat
#define StringCmp	Nlm_StringCmp
#define StringNCmp	Nlm_StringNCmp
#define StringICmp	Nlm_StringICmp
#define StringNICmp	Nlm_StringNICmp
#define StringChr   Nlm_StringChr
#define StringRChr  Nlm_StringRChr
#define StringPBrk  Nlm_StringPBrk
#define StringSpn   Nlm_StringSpn
#define StringCSpn  Nlm_StringCSpn
#define StringStr   Nlm_StringStr
#define StringTok   Nlm_StringTok
#define StringMove	Nlm_StringMove
#define StringSave	Nlm_StringSave
#define StringCnt   Nlm_StringCnt

#define MeshStringICmp Nlm_MeshStringICmp

/*----------------------------------------*/
/*      Misc Text Oriented Macros         */
/*----------------------------------------*/

#define IS_DIGIT(c)	('0'<=(c) && (c)<='9')
#define IS_UPPER(c)	('A'<=(c) && (c)<='Z')
#define IS_LOWER(c)	('a'<=(c) && (c)<='z')
#define IS_ALPHA(c)	(IS_UPPER(c) || IS_LOWER(c))
#define TO_LOWER(c)	((Nlm_Char)(IS_UPPER(c) ? (c)+' ' : (c)))
#define TO_UPPER(c)	((Nlm_Char)(IS_LOWER(c) ? (c)-' ' : (c)))
#define IS_WHITESP(c) (((c) == ' ') || ((c) == '\n') || ((c) == '\r') || ((c) == '\t'))
#define IS_ALPHANUM(c) (IS_ALPHA(c) || IS_DIGIT(c))

#ifdef __cplusplus
}
#endif

#endif
