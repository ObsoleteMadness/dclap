/* asndebin.h
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
* File Name:  asndebin.h
*
* Author:  James Ostell
*
* Version Creation Date: 3/4/91
*
* $Revision: 2.0 $
*
* File Description:
*   Typedefs and prototypes for internal routines of asndebin.c
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 3/4/91   Kans        Stricter typecasting for GNU C and C++
* 3/4/91   Kans        AsnDeBinReadBoolean returns Boolean
*
* ==========================================================================
*/

#ifndef _ASNDEBIN_
#define _ASNDEBIN_

	/* states in AsnDeBinTWord and AsnDeBinWord */
#define INDEFINATE -1              /* reading indefinate length item */	

/*****************************************************************************
*
*   prototypes
*   	AsnBinReadVal
*       AsnBinReadId  prototyped in asngen.h
*
*****************************************************************************/
extern Boolean AsnDeBinDecr PROTO((Int4 used, AsnIoPtr aip));
extern Int4 AsnDeBinScanTag PROTO((AsnIoPtr aip));
extern AsnTypePtr AsnDeBinFindType PROTO((AsnIoPtr aip, AsnModulePtr amp));
extern AsnTypePtr AsnDeBinFindElement PROTO((AsnIoPtr aip, AsnTypePtr atp));

extern Int4 AsnDeBinReadInteger PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean AsnDeBinReadBoolean PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern CharPtr AsnDeBinReadString PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern void AsnDeBinSkipString PROTO((AsnIoPtr aip));
extern ByteStorePtr AsnDeBinReadOctets PROTO((AsnIoPtr aip));
extern void AsnDeBinSkipOctets PROTO((AsnIoPtr aip));
extern FloatHi AsnDeBinReadReal PROTO((AsnIoPtr aip));
extern void AsnDeBinReadNull PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern void AsnDeBinSkipStruct PROTO((AsnIoPtr aip));

#endif
