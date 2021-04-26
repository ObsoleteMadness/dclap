/* asnbuild.h
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
* File Name: asnbuild.h
*
* Author:  James Ostell
*
* Version Creation Date: 1/1/91
*
* $Revision: 2.2 $
*
* File Description:
*   Includes for building asntool and libraries
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _ASNBUILD_
#define _ASNBUILD_

#ifdef __cplusplus
extern "C" {
#endif


#include <asn.h>
#include "asntypes.h"
#include "asnio.h"
#include "asnlex.h"
#include "asnprint.h"
#include "asndebin.h"
#include "asnenbin.h"
#include "asnlext.h"
#include "asnout.h"

extern char * g_asnlib;
#undef  THIS_MODULE
#define THIS_MODULE g_asnlib

#ifdef __cplusplus
}
#endif


#endif
