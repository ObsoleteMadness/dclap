/*   accutils.h
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
* File Name:  accutils.h
*
* Author:  J. Epstein
*
* Version Creation Date:   10/18/93
*
* $Revision: 1.2 $
*
* File Description: 
*       Utilities which make use of the Entrez "data access library"
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/

#ifndef _ACCUTILS_
#define _ACCUTILS_

#ifndef _ACCENTR_
#include <accentr.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

CharPtr LIBCALL EntrezFieldToString PROTO((DocType db, DocField fld));
DocField LIBCALL EntrezStringToField PROTO((DocType db, CharPtr str));

/* Evaluate or parse a text expression describing a boolean query */

/* In the event of a lexical or parsing error, begin and end describe the     */
/* range wherein an error was detected ... this range is then a candidate for */
/* visible highlighting                                                       */

/* If fld == -1, then use "[*]" field mechanism (union over all fields) by    */
/* default                                                                    */

LinkSetPtr LIBCALL EntrezTLEvalString PROTO((CharPtr str, DocType db, DocField fld, Int2Ptr begin, Int2Ptr end));
Boolean LIBCALL EntrezTLParseString PROTO((CharPtr str, DocType db, DocField fld, Int2Ptr begin, Int2Ptr end));

#ifdef __cplusplus
}
#endif

#endif
