/*
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
* File Name:  medarch.h
*
* Author:  Epstein
*
* Version Creation Date:   04/12/93
*
* $Revision: 1.2 $
*
* File Description: 
*       API for Medline Archive service
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/

Boolean ID0ArchInit PROTO((void));
Boolean ID0ArchFini PROTO((void));

SeqEntryPtr ID0ArchSeqEntryGet PROTO((Int4 uid, Boolean PNTR was_live, Int2 maxplex));
Int4 ID0ArchGIGet PROTO((SeqIdPtr sip));

/* # of retries to get a server */
#define ID_SERV_RETRIES 2
