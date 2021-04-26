/*   ncbimain.h
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
* File Name:  ncbimain.h
*
* Author:  Gish, Kans, Ostell, Schuler
*
* Version Creation Date:   7/7/91
*
* $Revision: 2.2 $
*
* File Description: 
*   	prototypes for portable string routines
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 7/7/91   Kans        Multiple configuration files, get and set functions
* 04-15-93 Schuler     Changed _cdecl to LIBCALL
*
*
* ==========================================================================
*/

#ifndef _NCBIMAIN_
#define _NCBIMAIN_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mainargs {
  char *prompt;            /* prompt for field */
  char *defaultvalue;      /* default */
  char *from;              /* range or datalink type */
  char *to;
  Nlm_Boolean	optional;
  Nlm_Char	tag;               /* argument on command line */
  Nlm_Int1	type;              /* type of value */
  Nlm_FloatHi	floatvalue;
  Nlm_Int4	intvalue;
  CharPtr	strvalue;
} Nlm_Arg, * Nlm_ArgPtr;      /* (*) not PNTR for MS Windows */
					/* type is:
						 0 = not set (invalid)
						 1 = Boolean
						 2 = Int
						 3 = Float
						 4 = String
						 5 = file in
						 6 = file out
						 7 = datalink input (type in from)
						 8 = datalink output
					  */
#define Args Nlm_Arg
#define ArgPtr Nlm_ArgPtr
#define ARG_BOOLEAN 1
#define ARG_INT 2
#define ARG_FLOAT 3
#define ARG_STRING 4
#define ARG_FILE_IN 5
#define ARG_FILE_OUT 6
#define ARG_DATA_IN 7
#define ARG_DATA_OUT 8

extern Nlm_Int2 Nlm_Main PROTO((void));
extern Nlm_Boolean Nlm_GetArgs PROTO((Nlm_CharPtr progname, Nlm_Int2 numargs, Nlm_ArgPtr ap));
extern Nlm_Boolean LIBCALL Nlm_FindPath PROTO((Nlm_CharPtr file, Nlm_CharPtr section, Nlm_CharPtr type, Nlm_CharPtr buf, Nlm_Int2 buflen));
extern Nlm_Int2 LIBCALL Nlm_GetAppParam PROTO((Nlm_CharPtr file, Nlm_CharPtr section, Nlm_CharPtr type, Nlm_CharPtr dflt, Nlm_CharPtr buf, Nlm_Int2 buflen));
extern Nlm_Boolean LIBCALL Nlm_SetAppParam PROTO((Nlm_CharPtr file, Nlm_CharPtr section, Nlm_CharPtr type, Nlm_CharPtr value));
extern Nlm_Boolean LIBCALL Nlm_TransientSetAppParam PROTO((Nlm_CharPtr file, Nlm_CharPtr section, Nlm_CharPtr type, Nlm_CharPtr value));
extern void LIBCALL Nlm_FreeConfigStruct PROTO((void));

#define Main Nlm_Main
#define GetArgs Nlm_GetArgs
#define FindPath Nlm_FindPath
#define GetAppParam Nlm_GetAppParam
#define SetAppParam Nlm_SetAppParam
#define TransientSetAppParam Nlm_TransientSetAppParam
#define FreeConfigStruct Nlm_FreeConfigStruct

#ifdef __cplusplus
}
#endif

#endif
