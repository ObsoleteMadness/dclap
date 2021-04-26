/*  objbibli.h
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
* File Name:  objbibli.h
*
* Author:  James Ostell
*   
* Version Creation Date: 1/1/91
*
* $Revision: 2.4 $
*
* File Description:  Object manager interface for module NCBI-Biblio
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 05-17-93 Schuler     struct affil  --->  struct _affil_  (because one of
*                      the fields in the struct is also called affil, which
*                      C++ compilers take to be a constructor function).
*
* ==========================================================================
*/

#ifndef _NCBI_Biblio_
#define _NCBI_Biblio_

#ifndef _ASNTOOL_
#include <asn.h>
#endif
#ifndef _NCBI_General_
#include <objgen.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
Boolean LIBCALL BiblioAsnLoad PROTO((void));

/*****************************************************************************
*
*   Affil
*
*****************************************************************************/
typedef struct _affil_ {
	Uint1 choice;           /* [1]=str,[2]=std */
	CharPtr affil,          /* also used for str */
		div,
		city,
		sub,
		country,
		street;
} Affil, PNTR AffilPtr;

AffilPtr LIBCALL AffilNew PROTO((void));
AffilPtr LIBCALL AffilFree PROTO((AffilPtr afp));
AffilPtr LIBCALL AffilAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean  LIBCALL AffilAsnWrite PROTO((AffilPtr afp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   AuthList
*
*****************************************************************************/
typedef struct authors {
	Uint1 choice;        /* [1]=std, [2]=ml, [3]=str (only on Cit-art,gen) */
	ValNodePtr names;    /* the SEQUENCE OF */
	AffilPtr affil;
} AuthList, PNTR AuthListPtr;

AuthListPtr LIBCALL AuthListNew PROTO((void));
AuthListPtr LIBCALL AuthListFree PROTO((AuthListPtr asp));
AuthListPtr LIBCALL AuthListAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean     LIBCALL AuthListAsnWrite PROTO((AuthListPtr afp, AsnIoPtr aip, AsnTypePtr atp));
Int2        LIBCALL AuthListMatch PROTO((AuthListPtr a, AuthListPtr b, Boolean all));

/*****************************************************************************
*
*   Author
*
*****************************************************************************/
typedef struct author {
	PersonIdPtr name;
	Uint1 lr[2];       /* level[0], role[1] as in spec. 0=not used */
	Uint1 is_corr;     /* corresponding author? 255=not set, 0=false, 1=true */
	AffilPtr affil;
} Author, PNTR AuthorPtr;

AuthorPtr LIBCALL AuthorNew PROTO((void));
AuthorPtr LIBCALL AuthorFree PROTO((AuthorPtr ap));
AuthorPtr LIBCALL AuthorAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean   LIBCALL AuthorAsnWrite PROTO((AuthorPtr ap, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Cit-art
*
*****************************************************************************/
typedef struct citart {
	ValNodePtr title;       /* choice[1]=name,[2]=tsub,[3]=trans */
	AuthListPtr authors;
	Uint1 from;             /* [1]=journal,[2]=book,[3]=proc */
	Pointer fromptr;
} CitArt, PNTR CitArtPtr;

CitArtPtr LIBCALL CitArtNew PROTO((void));
CitArtPtr LIBCALL CitArtFree PROTO((CitArtPtr cap));
CitArtPtr LIBCALL CitArtAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean   LIBCALL CitArtAsnWrite PROTO((CitArtPtr cap, AsnIoPtr aip, AsnTypePtr atp));
Int2      LIBCALL CitArtMatch PROTO((CitArtPtr a, CitArtPtr b));

/*****************************************************************************
*
*   Imprint
*
*****************************************************************************/
typedef struct imprint {
	DatePtr date;
    CharPtr volume,
        issue,
        pages,
        section,
        part_sup,
        language;
    DatePtr cprt;     /* copy right date (for books) */
    AffilPtr pub;   /* publisher (for books)  */
	Uint1 prepub;   /* 0=not set 1=submitted 2=in-press 255=other */
} Imprint, PNTR ImprintPtr;

ImprintPtr LIBCALL ImprintNew PROTO((void));
ImprintPtr LIBCALL ImprintFree PROTO((ImprintPtr cap));
ImprintPtr LIBCALL ImprintAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean    LIBCALL ImprintAsnWrite PROTO((ImprintPtr cap, AsnIoPtr aip, AsnTypePtr atp));
Int2       LIBCALL ImprintMatch PROTO((ImprintPtr a, ImprintPtr b, Boolean all));

/*****************************************************************************
*
*   Cit-jour
*
*****************************************************************************/
typedef struct citjour {
	ValNodePtr title;     /* choice in order of spec, 1=name,2=trans,etc */
	ImprintPtr imp;
} CitJour, PNTR CitJourPtr;

CitJourPtr LIBCALL CitJourNew PROTO((void));
CitJourPtr LIBCALL CitJourFree PROTO((CitJourPtr cjp));
CitJourPtr LIBCALL CitJourAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean    LIBCALL CitJourAsnWrite PROTO((CitJourPtr cjp, AsnIoPtr aip, AsnTypePtr atp));
Int2       LIBCALL CitJourMatch PROTO((CitJourPtr a, CitJourPtr b));

/*****************************************************************************
*
*   Cit-book
*
*****************************************************************************/
typedef struct citbook {
	ValNodePtr title,      /* choice in order of spec, 1=name, 2=tsub, etc */
			   coll;       /* ditto */
	AuthListPtr authors;
	ImprintPtr imp;
	Uint1 othertype,      /* 0=Cit-book, 1=Cit-proc, 2=Cit-let */
		let_type;         /* if Cit-let, 1=manuscript,2=letter,3=thesis */
	Pointer otherdata;    /* NULL,  ValNodes, CharPtr man-id */
} CitBook, PNTR CitBookPtr;

CitBookPtr LIBCALL CitBookNew PROTO((void));
CitBookPtr LIBCALL CitBookFree PROTO((CitBookPtr cbp));
CitBookPtr LIBCALL CitBookAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean    LIBCALL CitBookAsnWrite PROTO((CitBookPtr cbp, AsnIoPtr aip, AsnTypePtr atp));
Int2       LIBCALL CitBookMatch PROTO((CitBookPtr a, CitBookPtr b));

/*****************************************************************************
*
*   Cit-sub
*   	Direct submission of data
*
*****************************************************************************/
typedef struct citsub {
	AuthListPtr authors;
	ImprintPtr imp;
	Uint1 medium;
} CitSub, PNTR CitSubPtr;

CitSubPtr LIBCALL CitSubNew PROTO((void));
CitSubPtr LIBCALL CitSubFree PROTO((CitSubPtr cbp));
CitSubPtr LIBCALL CitSubAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean   LIBCALL CitSubAsnWrite PROTO((CitSubPtr cbp, AsnIoPtr aip, AsnTypePtr atp));
Int2      LIBCALL CitSubMatch PROTO((CitSubPtr a, CitSubPtr b));


/*****************************************************************************
*
*   Cit-proc
*     uses otherdata in Cit-book
*     chain of ValNodes
*     choice       ident      Pointer type
*       1          number      CharPtr
*       2          date        DatePtr
*       3          place       AffilPtr
*
*****************************************************************************/
CitBookPtr LIBCALL CitProcAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean    LIBCALL CitProcAsnWrite PROTO((CitBookPtr cpp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Cit-let
*     uses otherdata in Cit-book as CharPtr for man-id
*
*****************************************************************************/
CitBookPtr LIBCALL CitLetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean    LIBCALL CitLetAsnWrite PROTO((CitBookPtr cpp, AsnIoPtr aip, AsnTypePtr atp));



/*****************************************************************************
*
*   Cit-pat
*
*****************************************************************************/
#ifdef ASN30
typedef struct patpriority {
	CharPtr country,
		number;
	DatePtr date;
	struct patpriority PNTR next;
} PatPriority, PNTR PatPriorityPtr;
#endif

typedef struct citpat {
	CharPtr title;
	AuthListPtr authors;
	CharPtr country,
		doc_type,
		number;
	DatePtr date_issue;
#ifdef ASN30
	ValNodePtr _class;
#else
	CharPtr _class;
#endif
	CharPtr	app_number;
	DatePtr app_date;
#ifdef ASN30					/* for ASN.1 spec 3.0 */
	AuthListPtr applicants;
	AuthListPtr assignees;
	PatPriorityPtr priority;
	CharPtr abstract;
#endif
} CitPat, PNTR CitPatPtr;

CitPatPtr LIBCALL CitPatNew PROTO((void));
CitPatPtr LIBCALL CitPatFree PROTO((CitPatPtr cpp));
CitPatPtr LIBCALL CitPatAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean   LIBCALL CitPatAsnWrite PROTO((CitPatPtr cpp, AsnIoPtr aip, AsnTypePtr atp));

#ifdef ASN30
PatPriorityPtr LIBCALL PatPriorityNew PROTO((void));
PatPriorityPtr LIBCALL PatPrioritySetFree PROTO((PatPriorityPtr ppp));
PatPriorityPtr LIBCALL PatPrioritySetAsnRead PROTO((AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element));
Boolean   LIBCALL PatPrioritySetAsnWrite PROTO((PatPriorityPtr ppp, AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element));
#endif
/*****************************************************************************
*
*   Id-pat
*
*****************************************************************************/
typedef struct idpat {
	CharPtr country,
		number,				 /** actually CHOICE of number or app_number */
		app_number;
#ifdef ASN30
	CharPtr doc_type;
#endif
} IdPat, PNTR IdPatPtr;

IdPatPtr LIBCALL IdPatNew PROTO((void));
IdPatPtr LIBCALL IdPatFree PROTO((IdPatPtr ipp));
IdPatPtr LIBCALL IdPatAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean  LIBCALL IdPatAsnWrite PROTO((IdPatPtr ipp, AsnIoPtr aip, AsnTypePtr atp));
Boolean  LIBCALL IdPatMatch PROTO((IdPatPtr a, IdPatPtr b));

/*****************************************************************************
*
*   Cit-gen
*
*****************************************************************************/
typedef struct cit_gen {
	CharPtr cit;
	AuthListPtr authors;
    Int4 muid;                  /* medline uid, -1 if not set */
    ValNodePtr journal;         /* journal/book Title */
    CharPtr volume,
        issue,
        pages;
	DatePtr date;
    Int2 serial_number;      /* for GenBank style references (-1 = not used)*/
	CharPtr title;           /* a specific title (in addition to cit or journal) */
} CitGen, PNTR CitGenPtr;

CitGenPtr LIBCALL CitGenNew PROTO((void));
CitGenPtr LIBCALL CitGenFree PROTO((CitGenPtr cgp));
CitGenPtr LIBCALL CitGenAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean   LIBCALL CitGenAsnWrite PROTO((CitGenPtr cgp, AsnIoPtr aip, AsnTypePtr atp));
Int2      LIBCALL CitGenMatch PROTO((CitGenPtr a, CitGenPtr b, Boolean all));

/*****************************************************************************
*
*   Title
*
*****************************************************************************/

ValNodePtr LIBCALL TitleFree PROTO((ValNodePtr anp));
ValNodePtr LIBCALL TitleAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean    LIBCALL TitleAsnWrite PROTO((ValNodePtr anp, AsnIoPtr aip, AsnTypePtr atp));
Int2       LIBCALL TitleMatch PROTO((ValNodePtr a, ValNodePtr b, Uint1 type));

#define Cit_title_name ( (Uint1) 1)
#define Cit_title_tsub ( (Uint1) 2)
#define Cit_title_trans ( (Uint1) 3)
#define Cit_title_jta ( (Uint1) 4)
#define Cit_title_iso_jta ( (Uint1) 5)
#define Cit_title_ml_jta ( (Uint1) 6)
#define Cit_title_coden ( (Uint1) 7)
#define Cit_title_issn ( (Uint1) 8)
#define Cit_title_abr ( (Uint1) 9)
#define Cit_title_isbn ( (Uint1) 10)

#ifdef __cplusplus
}
#endif

#endif
