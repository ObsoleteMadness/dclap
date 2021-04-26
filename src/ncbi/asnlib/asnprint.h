/* asnprint.h
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
* File Name: asnprint.h
*
* Author:  James Ostell
*
* Version Creation Date: 1/1/91
*
* $Revision: 2.2 $
*
* File Description:
*   typedefs and prototypes used internally by asnprint.c
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _ASNPRINT_
#define _ASNPRINT_

/*****************************************************************************
*
*   prototypes
*   	AsnTxtWrite prototyped in asngen.h
*
*****************************************************************************/
extern void AsnPrintModule PROTO((AsnModulePtr amp, AsnIoPtr aip));
extern void AsnPrintType PROTO((AsnTypePtr atp, AsnIoPtr aip));
extern Boolean AsnPrintStrStore PROTO((ByteStorePtr bsp, AsnIoPtr aip));
extern void AsnPrintReal PROTO((FloatHi realvalue, AsnIoPtr aip));
extern void AsnPrintInteger PROTO((Int4 theInt, AsnIoPtr aip));
extern void AsnPrintBoolean PROTO((Boolean value, AsnIoPtr aip));
extern void AsnPrintOctets PROTO((ByteStorePtr ssp, AsnIoPtr aip));
extern void AsnPrintChar PROTO((char theChar, AsnIoPtr aip));
extern void AsnPrintIndent PROTO((Boolean increase, AsnIoPtr aip));
extern void AsnPrintNewLine PROTO((AsnIoPtr aip));
extern Boolean AsnPrintString PROTO((CharPtr str, AsnIoPtr aip));
extern void AsnPrintCharBlock PROTO((CharPtr str, AsnIoPtr aip));
extern int AsnPrintGetWordBreak PROTO((CharPtr str, int maxlen));
extern void AsnPrintOpenStruct PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern void AsnPrintCloseStruct PROTO((AsnIoPtr aip, AsnTypePtr atp));

#endif
