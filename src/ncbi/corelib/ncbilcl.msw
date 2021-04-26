/*   ncbilcl.h
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
* File Name:  ncbilcl.h
*
* Author:  Gish, Kans, Ostell, Schuler
*
* Version Creation Date:   1/1/91
*
* $Revision: 2.12 $
*
* File Description:
*		system dependent header
*		MS Windows version
*
* Modifications:
* --------------------------------------------------------------------------
* Date       Name        Description of modification
* --------  ----------  -----------------------------------------------------
* 04-05-93  Schuler     Some changes for WindowsNT and DLLs
* 04-12-93  Schuler     Added some #ifndef's to avoid redefinitions
* 06-11-93  Schuler     Added INLINE macro (defined to __inline)
*
* ==========================================================================
*/
#ifndef _NCBILCL_
#define _NCBILCL_

/* PLATFORM DEFINITION FOR IBM PC UNDER MS WINDOWS */

#define COMP_MSC
#define WIN_MSWIN

#ifdef WIN32

/** We'll assume OS is WindowsNT, even though it could
    also be WindowsDOS + Win32s  **/
#define OS_WINNT  
#ifndef FAR
#define FAR
#define NEAR
#endif
#ifndef EXPORT
#define EXPORT
#endif

//----- Intel processor -----
#if defined(X86) || defined(_X86_)
#define PROC_I80X86
#define PROC_I80_386
#ifndef PASCAL
#define PASCAL	__stdcall
#define CDECL	
#endif
#endif	// _X86_

//----- MIPS processor -----
#if defined(MIPS) || defined(_MIPS_)
#define PROC_MIPS
#ifndef PASCAL
#define PASCAL
#define CDECL
#endif
#endif	// _MIPS_
	
//----- DEC Alpha processor -----
#if defined(ALPHA) || defined(_ALPHA_)
#define PROC_ALPHA
#define _near	//temporary
#define _cdecl	//temporary
#ifndef PASCAL
#define PASCAL
#define CDECL
#endif
#endif  // _APLHA_

#else
// regular WindowsDOS (16-bit) definitions
#ifndef WIN16
#define WIN16
#endif
#define OS_DOS
#define PROC_I80X86
#ifndef PASCAL
#define PASCAL	__pascal
#define CDECL	__cdecl
#define EXPORT  __export
#endif
#ifndef FAR
#define FAR	__far
#define NEAR	__near
#endif

#endif

#if (_MSC_VER >= 700)
#define INLINE __inline
#endif


/*----------------------------------------------------------------------*/
/*      Desired or available feature list                               */
/*----------------------------------------------------------------------*/
#define TRACE_AUX	// TRACE to AUX device

/*----------------------------------------------------------------------*/
/*      #includes                                                       */
/*----------------------------------------------------------------------*/
#include <stddef.h>
#include <sys\types.h>
#include <limits.h>
#include <sys\stat.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <float.h>

/*----------------------------------------------------------------------*/
/*      Missing ANSI-isms                                               */
/*----------------------------------------------------------------------*/
#ifndef FILENAME_MAX
#define FILENAME_MAX 63
#endif

/*----------------------------------------------------------------------*/
/*      Aliased Logicals, Datatypes                                     */
/*----------------------------------------------------------------------*/
#define PNTR FAR *
#define HNDL NEAR *

typedef int (FAR PASCAL *Nlm_FnPtr)();
#define FnPtr           Nlm_FnPtr

/*----------------------------------------------------------------------*/
/*      Misc Macros                                                     */
/*----------------------------------------------------------------------*/
#define PROTO(x)        x              /* Function prototypes are real */
#define VPROTO(x) x   /* Prototype for variable argument list */
#define DIRDELIMCHR     '\\'
#define DIRDELIMSTR     "\\"
#define CWDSTR          "."

#define KBYTE           (1024)
#define MBYTE           (1048576L)

#define IS_LITTLE_ENDIAN

#define TEMPNAM_AVAIL
#ifndef tempnam
#define tempnam _tempnam
#endif

/*----------------------------------------------------------------------*/
/*      Macros for Floating Point                                       */
/*----------------------------------------------------------------------*/
#define EXP2(x) exp((x)*LN2)
#define LOG2(x) (log(x)*(1./LN2))
#define EXP10(x) exp((x)*LN10)
#define LOG10(x) (log10(x))

/*----------------------------------------------------------------------*/
/*      Macros Defining Limits                                          */
/*----------------------------------------------------------------------*/
#ifdef WIN32
#define MAXALLOC	0x7F000000 /* Largest permissible memory request */
#else
#define MAXALLOC	0x10000 /* Largest permissible memory request */
#endif

#endif
