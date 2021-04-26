/* asnio.h
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
* File Name: asnio.h
*
* Author:  James Ostell
*
* Version Creation Date: 1/1/91
*
* $Revision: 2.2 $
*
* File Description:
*   Typedefs and prototypes used internally by asnio.c
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _ASNIO_
#define _ASNIO_

/*****************************************************************************
*
*   prototypes
*   	also in asngen.h
*
*****************************************************************************/
extern CharPtr AsnIoGets PROTO((AsnIoPtr aip));
extern void AsnIoPuts PROTO((AsnIoPtr aip));
extern Int2 AsnIoReadBlock PROTO((AsnIoPtr aip));
extern Int2 AsnIoWriteBlock PROTO((AsnIoPtr aip));
extern void CDECL AsnIoErrorMsg VPROTO((AsnIoPtr aip, int errcode, ...));

#endif
