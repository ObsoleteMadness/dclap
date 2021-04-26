/*   ncbimisc.h
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
* File Name:  ncbimisc.h
*
* Author:  Ostell, Schuler
*
* Version Creation Date:   06-15-93
*
* $Revision: 2.0 $
*
* File Description: 
*		Prototypes of functions to Convert SGML to ASCII 
*		for Backbone subset SGML
*
* Modifications:
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 04-15-93 Schuler     Changed _cdecl to LIBCALL
* 06-15-93 Schuler     This file created (SGML functions were in ncbimisc.c)
* 06-15-93 Schuler     SgmlLoadTable() exported (was static)
*
* ==========================================================================
*/

#ifndef _NCBISGML_
#define _NCBISGML_

#ifdef __cplusplus
extern "C" {
#endif

/*  */
int    LIBCALL Nlm_SgmlLoadTable PROTO((void));
char * LIBCALL Nlm_Sgml2Ascii PROTO((const char *sgml, char *ascii, size_t buflen));
size_t LIBCALL Nlm_Sgml2AsciiLen PROTO((const char *sgml));

/* aliases */
#define SgmlLoadTable	Nlm_SgmlLoadTable
#define Sgml2Ascii		Nlm_Sgml2Ascii
#define SgmlToAscii		Nlm_Sgml2Ascii
#define Sgml2AsciiLen	Nlm_Sgml2AsciiLen
#define SgmlToAsciiLen	Nlm_Sgml2AsciiLen

#ifdef __cplusplus
}
#endif

#endif /* !_NCBISGML_ */
