/*  objgen.h
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
* File Name:  objgen.h
*
* Author:  James Ostell
*   
* Version Creation Date: 1/1/91
*
* $Revision: 2.3 $
*
* File Description:  Object manager interface for module NCBI-General
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_General_
#define _NCBI_General_

#ifndef _ASNTOOL_
#include <asn.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
Boolean LIBCALL GeneralAsnLoad PROTO((void));

/*****************************************************************************
*
*   internal structures for NCBI-General objects
*
*****************************************************************************/

/*****************************************************************************
*
*   Date, Date-std share the same structure
*      any data[2] or data[3] values = 0 means not set or not present
*   data [0] - CHOICE of date ,0=str, 1=std
*        [1] - year (- 1900)
*        [2] - month (1-12)  optional
*   	 [3] - day (1-31)	 optional
*
*****************************************************************************/


typedef struct date {
	Uint1 data[4];      /* see box above */
	CharPtr str;		/* str or season or NULL */
} NCBI_Date, PNTR NCBI_DatePtr;
#define DatePtr NCBI_DatePtr

NCBI_DatePtr LIBCALL DateNew PROTO((void));
NCBI_DatePtr LIBCALL DateFree PROTO((NCBI_DatePtr dp));
Boolean      LIBCALL DateWrite PROTO((NCBI_DatePtr dp, Int2 year, Int2 month, Int2 day, CharPtr season));
Boolean      LIBCALL DateRead PROTO((NCBI_DatePtr dp, Int2Ptr year, Int2Ptr month, Int2Ptr day, CharPtr season));
Boolean      LIBCALL DatePrint PROTO((NCBI_DatePtr dp, CharPtr buf));
NCBI_DatePtr LIBCALL DateCurr PROTO((void));
NCBI_DatePtr LIBCALL DateDup PROTO((NCBI_DatePtr dp));
Boolean      LIBCALL DateAsnWrite PROTO((NCBI_DatePtr dp, AsnIoPtr aip, AsnTypePtr atp));
NCBI_DatePtr LIBCALL DateAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Int2         LIBCALL DateMatch PROTO((DatePtr a, DatePtr b, Boolean all));

/*****************************************************************************
*
*   Object-id stuff
*
*****************************************************************************/
typedef struct objid {
	Int4 id;
	CharPtr str;
} ObjectId, PNTR ObjectIdPtr;

ObjectIdPtr LIBCALL ObjectIdNew PROTO((void));
ObjectIdPtr LIBCALL ObjectIdFree PROTO(( ObjectIdPtr oid));
ObjectIdPtr LIBCALL ObjectIdAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean     LIBCALL ObjectIdAsnWrite PROTO((ObjectIdPtr oid, AsnIoPtr aip, AsnTypePtr atp));
Boolean     LIBCALL ObjectIdMatch PROTO((ObjectIdPtr a, ObjectIdPtr b));
ObjectIdPtr LIBCALL ObjectIdDup PROTO((ObjectIdPtr oldid));

/*****************************************************************************
*
*   DBtag stuff
*
*****************************************************************************/
typedef struct dbtag {
	CharPtr db;
	ObjectIdPtr tag;
} Dbtag, PNTR DbtagPtr;

DbtagPtr	LIBCALL DbtagNew PROTO((void));
DbtagPtr	LIBCALL DbtagFree PROTO(( DbtagPtr dbt));
DbtagPtr	LIBCALL DbtagAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean		LIBCALL DbtagAsnWrite PROTO((DbtagPtr dbt, AsnIoPtr aip, AsnTypePtr atp));
Boolean		LIBCALL DbtagMatch PROTO((DbtagPtr a, DbtagPtr b));
DbtagPtr	LIBCALL DbtagDup PROTO((DbtagPtr oldtag));

/*****************************************************************************
*
*   Name-std
*   names[0] = last
*        [1] = first
*        [2] = middle
*        [3] = full
*        [4] = initials
*        [5] = suffix
*        [6] = title
*
*****************************************************************************/
typedef struct namestd {
	CharPtr names[7];
} NameStd, PNTR NameStdPtr;

NameStdPtr LIBCALL NameStdNew PROTO((void));
NameStdPtr LIBCALL NameStdFree PROTO(( NameStdPtr nsp));
NameStdPtr LIBCALL NameStdAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean    LIBCALL NameStdAsnWrite PROTO((NameStdPtr nsp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Person-id
*     choice = 0 = not set
*              1 = dbtag
*              2 = name
*              3 = ml
*              4 = str
*
*****************************************************************************/
typedef struct personid {
	Uint1 choice;         /* which CHOICE, see above */
	Pointer data;         /* points to appropriate data structure */
} PersonId, PNTR PersonIdPtr;

PersonIdPtr	LIBCALL PersonIdNew PROTO((void));
PersonIdPtr	LIBCALL PersonIdFree PROTO(( PersonIdPtr pid));
PersonIdPtr	LIBCALL PersonIdAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean		LIBCALL PersonIdAsnWrite PROTO((PersonIdPtr pid, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Int-fuzz
*
*****************************************************************************/
typedef struct intfuzz {
	Uint1 choice;       /* 1=p-m, 2=range, 3=pct, 4=lim */
	Int4 a, b;          /* a=p-m,max,pct,orlim, b=min */
} IntFuzz, PNTR IntFuzzPtr;

IntFuzzPtr	LIBCALL IntFuzzNew PROTO((void));
IntFuzzPtr	LIBCALL IntFuzzFree PROTO(( IntFuzzPtr ifp));
IntFuzzPtr	LIBCALL IntFuzzAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean		LIBCALL IntFuzzAsnWrite PROTO((IntFuzzPtr ifp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   User-field
*      data is an DataVal where:
*    choice    asn1              data. =
        1 = str VisibleString ,  ptrvalue = CharPtr
        2 = int INTEGER ,        intvalue
        3 = real REAL ,          realvalue
        4 = bool BOOLEAN ,       boolvalue
        5 = os OCTET STRING ,    ptrvalue = ByteStorePtr
        6 = object User-object ,   ptrvalue = UserObjectPtr
        7 = strs SEQUENCE OF VisibleString ,  ptrvalue = CharPtr PNTR
        8 = ints SEQUENCE OF INTEGER ,        ptrvalue = Int4Ptr
        9 = reals SEQUENCE OF REAL ,          ptrvalue = FloatHiPtr
        10 = oss SEQUENCE OF OCTET STRING ,   ptrvalue = ByteStorePtr PNTR
        11 = fields SEQUENCE OF User-field ,  ptrvalue = UserFieldPtr
        12 = objects SEQUENCE OF User-object } }  ptrvalue = UserObjectPtr

*   User-object
*
*****************************************************************************/
typedef struct userfield {
    ObjectIdPtr label;
    Int4 num;
    Uint1 choice;
    DataVal data;
    struct userfield PNTR next;
} UserField, PNTR UserFieldPtr;

UserFieldPtr LIBCALL UserFieldNew PROTO((void));
UserFieldPtr LIBCALL UserFieldFree PROTO(( UserFieldPtr ufp));
UserFieldPtr LIBCALL UserFieldAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean		 LIBCALL UserFieldAsnWrite PROTO((UserFieldPtr ufp, AsnIoPtr aip, AsnTypePtr atp));

typedef struct userobj {
    CharPtr _class;
    ObjectIdPtr type;
    UserFieldPtr data;
    struct userobj PNTR next;   /* for SEQUENCE OF User-object */
} UserObject, PNTR UserObjectPtr;

UserObjectPtr LIBCALL UserObjectNew PROTO((void));
UserObjectPtr LIBCALL UserObjectFree PROTO(( UserObjectPtr uop));
UserObjectPtr LIBCALL UserObjectAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean		  LIBCALL UserObjectAsnWrite PROTO((UserObjectPtr uop, AsnIoPtr aip, AsnTypePtr atp));

#ifdef __cplusplus
}
#endif

#endif

