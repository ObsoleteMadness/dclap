/*   netlib.h
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
* File Name:  netlib.h
*
* Author:  Epstein
*
* Version Creation Date:   12/15/92
*
* $Revision: 4.0 $
*
* File Description: 
*       private header file only to be included within Network Entrez source
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*
*
* RCS Modification History:
* $Log: netpriv.h,v $
 * Revision 4.0  1995/07/26  13:54:59  ostell
 * force revision to 4.0
 *
 * Revision 1.3  1995/05/17  17:53:26  epstein
 * add RCS log revision history
 *
*/

typedef struct {
    NI_HandPtr sessionHandle;
} NetMediaInfo, PNTR NetMediaInfoPtr;


NI_HandPtr CDECL NetServiceGet PROTO((CharPtr channel, CharPtr def_service, ConfCtlProc swapInMedia, NI_HandPtr oldSessionHandle));
