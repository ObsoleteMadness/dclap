/* asnlex.h
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
* File Name: asnlex.h
*
* Author:  James Ostell
*
* Version Creation Date: 3/4/91
*
* $Revision: 2.1 $
*
* File Description:
*   Typedefs and prototypes used internally by asnlex.c
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 3/4/91   Kans        Stricter typecasting for GNU C and C++
* 3/4/91   Kans        AsnLexReadBoolean returns Boolean
*
* ==========================================================================
*/

#ifndef _ASNLEX_
#define _ASNLEX_

	/* states in AsnLexTWord and AsnLexWord */
#define IN_STRING_STATE 1          /* reading a text string */
#define IN_BITHEX_STATE 2          /* reading a hex or binary string */
	

/*****************************************************************************
*
*   prototypes
*   	AsnTxtReadId
*       AsnTxtReadVal  prototyped in asngen.h
*
*****************************************************************************/
extern AsnTypePtr AsnLexFindType PROTO((AsnIoPtr aip, AsnModulePtr amp));
extern AsnTypePtr AsnLexFindElement PROTO((AsnIoPtr aip, AsnTypePtr atp));

extern Int4 AsnLexReadInteger PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean AsnLexReadBoolean PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Pointer AsnLexReadString PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern Boolean AsnLexSkipString PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern FloatHi AsnLexReadReal PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern void AsnLexReadNull PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern ByteStorePtr AsnLexReadOctets PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern void AsnLexSkipOctets PROTO((AsnIoPtr aip, AsnTypePtr atp));
extern CharPtr AsnLexSaveWord PROTO((AsnIoPtr aip));
extern void AsnLexSkipStruct PROTO((AsnIoPtr aip));
extern Int4 AsnLexInteger PROTO((AsnIoPtr aip));
extern Int2 AsnLexWord PROTO((AsnIoPtr aip));
extern Boolean StrMatch PROTO((CharPtr a, CharPtr b, Int2 len));

#endif
