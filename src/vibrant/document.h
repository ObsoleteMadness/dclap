/*  document.h
* ===========================================================================
*
*                            PUBLIC DOMAIN NOTICE
*            National Center for Biotechnology Information (NCBI)
*
*  This software/database is a "United States Government Work" under the
*  terms of the United States Copyright Act.  It was written as part of
*  the author's official duties as a United States Government employee and
*  thus cannot be copyrighted.  This software/database is freely available
*  to the public for use. The National Library of Medicine and the U.S.
*  Government do not place any restriction on its use or reproduction.
*  We would, however, appreciate having the NCBI and the author cited in
*  any work or product based on this material
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the NLM and the U.S.
*  Government do not and cannot warrant the performance or results that
*  may be obtained by using this software or data. The NLM and the U.S.
*  Government disclaim all warranties, express or implied, including
*  warranties of performance, merchantability or fitness for any particular
*  purpose.
*
* ===========================================================================
*
* File Name:  document.h
*
* Author:  Jonathan Kans
*   
* Version Creation Date: 4/12/93
*
* $Revision: 2.26 $
*
* File Description:  Converts fielded text into final report in a document
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _DOCUMENT_
#define _DOCUMENT_

#ifndef _VIBRANT_
#include <vibrant.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* DOC OBJECT HANDLE TYPE */

typedef struct Nlm_doctype {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_DoC;

/* DOC PROCEDURE TYPES */

typedef Nlm_CharPtr (*Nlm_DocPrntProc)  PROTO((Nlm_DoC, Nlm_Int2, Pointer));
typedef void        (*Nlm_DocClckProc)  PROTO((Nlm_DoC, Nlm_PoinT));
typedef void        (*Nlm_DocDrawProc)  PROTO((Nlm_DoC, Nlm_RectPtr, Nlm_Int2, Nlm_Int2));
typedef Nlm_Boolean (*Nlm_DocShadeProc) PROTO((Nlm_DoC, Nlm_Int2, Nlm_Int2, Nlm_Int2));
typedef void        (*Nlm_DocDataProc)  PROTO((Nlm_DoC, Pointer));
typedef void        (*Nlm_DocPanProc)   PROTO((Nlm_DoC));
typedef void        (*Nlm_DocFreeProc)  PROTO((Nlm_DoC, Nlm_VoidPtr));

typedef void        (*Nlm_DocPutProc)   PROTO((Nlm_DoC, Nlm_Int2, Nlm_CharPtr));
typedef Nlm_CharPtr (*Nlm_DocGetProc)   PROTO((Nlm_DoC, Nlm_Int2));
typedef void        (*Nlm_DocUpdProc)   PROTO((Nlm_DoC, Nlm_Int2, Nlm_Int2));

/* COLUMN AND PARAGRAPH STRUCTURES */

typedef struct Nlm_coldata {
  Nlm_Int2      pixWidth;
  Nlm_Int2      pixInset;
  Nlm_Int2      charWidth;
  Nlm_Int2      charInset;
  Nlm_FonT      font;
  Nlm_Char      just;
  unsigned int  wrap      : 1;
  unsigned int  bar       : 1;
  unsigned int  underline : 1;
  unsigned int  left      : 1;
  unsigned int  last      : 1;
} Nlm_ColData, PNTR Nlm_ColPtr;

typedef struct Nlm_pardata {
  unsigned int  openSpace    : 1;
  unsigned int  keepWithNext : 1;
  unsigned int  keepTogether : 1;
  unsigned int  newPage      : 1;
  unsigned int  tabStops     : 1;
  Nlm_Int2      minLines;
  Nlm_Int2      minHeight;
} Nlm_ParData, PNTR Nlm_ParPtr;

/* DOC PROCEDURES */

extern Nlm_DoC     Nlm_DocumentPanel  PROTO((Nlm_GrouP prnt, Nlm_Int2 pixwidth, Nlm_Int2 pixheight));

extern void    Nlm_SetDocProcs    PROTO((Nlm_DoC d, Nlm_DocClckProc click, Nlm_DocClckProc drag,
			Nlm_DocClckProc release, Nlm_DocPanProc pan));
extern void    Nlm_SetDocShade    PROTO((Nlm_DoC d, Nlm_DocDrawProc draw, Nlm_DocShadeProc gray,
			Nlm_DocShadeProc invert, Nlm_DocShadeProc color));
extern void    Nlm_SetDocCache    PROTO((Nlm_DoC d, Nlm_DocPutProc put, Nlm_DocGetProc get,
			Nlm_DocUpdProc upd));

extern void    Nlm_SetDocData     PROTO((Nlm_DoC d, VoidPtr data, Nlm_DocFreeProc cleanup));
extern VoidPtr Nlm_GetDocData     PROTO((Nlm_DoC d));
extern Nlm_CharPtr Nlm_GetDocText     PROTO((Nlm_DoC d, Nlm_Int2 item, Nlm_Int2 row, Nlm_Int2 col));
extern void    Nlm_MapDocPoint    PROTO((Nlm_DoC d, Nlm_PoinT pt, Nlm_Int2Ptr item, Nlm_Int2Ptr row,
			Nlm_Int2Ptr col, Nlm_RectPtr rct));
extern void    Nlm_PrintDocument  PROTO((Nlm_DoC d));
extern void    Nlm_SaveDocument   PROTO((Nlm_DoC d, FILE *f));

/* PARAMETER PROCEDURES */

extern void Nlm_GetDocParams  PROTO((Nlm_DoC d, Nlm_Int2Ptr numItems, Nlm_Int2Ptr numLines));
extern void Nlm_GetItemParams PROTO((Nlm_DoC d, Nlm_Int2 item, Nlm_Int2Ptr startsAt,
			Nlm_Int2Ptr numRows, Nlm_Int2Ptr numNlm_Cols, Nlm_Int2Ptr lineHeight));
extern void Nlm_GetColParams  PROTO((Nlm_DoC d, Nlm_Int2 item, Nlm_Int2 col, Nlm_Int2Ptr pixPos,
			Nlm_Int2Ptr pixWidth, Nlm_Int2Ptr pixInset, Nlm_CharPtr just));

/* TABLE PROCEDURES */

extern void Nlm_AppendItem  PROTO((Nlm_DoC d, Nlm_DocPrntProc proc, Pointer data,
			Nlm_Boolean docOwnsData, Nlm_Int2 lines, Nlm_ParPtr parFmt,
			Nlm_ColPtr colFmt, Nlm_FonT font));
extern void Nlm_AppendText  PROTO((Nlm_DoC d, Nlm_CharPtr text, Nlm_ParPtr parFmt,
			Nlm_ColPtr colFmt, Nlm_FonT font));

extern void Nlm_ReplaceItem PROTO((Nlm_DoC d, Nlm_Int2 item, Nlm_DocPrntProc proc,
			Pointer data, Nlm_Boolean docOwnsData, Nlm_Int2 lines,
			Nlm_ParPtr parFmt, Nlm_ColPtr colFmt, Nlm_FonT font));
extern void Nlm_ReplaceText PROTO((Nlm_DoC d, Nlm_Int2 item, Nlm_CharPtr text,
			Nlm_ParPtr parFmt, Nlm_ColPtr colFmt, Nlm_FonT font));

extern void Nlm_InsertItem  PROTO((Nlm_DoC d, Nlm_Int2 item, Nlm_DocPrntProc proc,
			Pointer data, Nlm_Boolean docOwnsData, Nlm_Int2 lines,
			Nlm_ParPtr parFmt, Nlm_ColPtr colFmt, Nlm_FonT font));
extern void Nlm_InsertText  PROTO((Nlm_DoC d, Nlm_Int2 item, Nlm_CharPtr text,
			Nlm_ParPtr parFmt, Nlm_ColPtr colFmt, Nlm_FonT font));

extern void Nlm_DeleteItem  PROTO((Nlm_DoC d, Nlm_Int2 item));

/* INVALIDATION AND UPDATE PROCEDURE */

extern Nlm_Boolean Nlm_ItemIsVisible PROTO((Nlm_DoC d, Nlm_Int2 item, Nlm_Int2Ptr top,
			Nlm_Int2Ptr bottom, Nlm_Int2Ptr firstLine));
extern Nlm_Boolean Nlm_GetScrlParams PROTO((Nlm_DoC d, Nlm_Int2Ptr offset,
			Nlm_Int2Ptr firstShown, Nlm_Int2Ptr firstLine));
extern void Nlm_UpdateDocument PROTO((Nlm_DoC d, Nlm_Int2 from, Nlm_Int2 to));

extern void Nlm_AdjustDocScroll PROTO((Nlm_DoC d));
extern void Nlm_SetDocAutoAdjust PROTO((Nlm_DoC d, Nlm_Boolean autoAdjust));
extern void Nlm_SetDocVirtual PROTO((Nlm_DoC d, Nlm_Boolean isvirtual));

extern void Nlm_SetDocTabstops PROTO((Nlm_DoC d, Int2 tabStops));  /* dgg added this */

/* STANDARD FILE DISPLAY PROCEDURES */

extern void Nlm_DisplayFile   PROTO((Nlm_DoC d, Nlm_CharPtr file, Nlm_FonT font));
extern void Nlm_DisplayFancy  PROTO((Nlm_DoC d, Nlm_CharPtr file, Nlm_ParPtr parFmt,
			Nlm_ColPtr colFmt, Nlm_FonT font, Nlm_Int2 tabStops));

#define DoC Nlm_DoC
#define DocPrntProc Nlm_DocPrntProc
#define DocClckProc Nlm_DocClckProc
#define DocDrawProc Nlm_DocDrawProc
#define DocShadeProc Nlm_DocShadeProc
#define DocDataProc Nlm_DocDataProc
#define DocPanProc Nlm_DocPanProc
#define DocFreeProc Nlm_DocFreeProc
#define DocPutProc Nlm_DocPutProc
#define DocGetProc Nlm_DocGetProc
#define DocUpdProc Nlm_DocUpdProc
#define ColPtr Nlm_ColPtr
#define ColData Nlm_ColData
#define ParData Nlm_ParData
#define ParPtr Nlm_ParPtr
#define DocumentPanel Nlm_DocumentPanel
#define SetDocProcs Nlm_SetDocProcs
#define SetDocShade Nlm_SetDocShade
#define SetDocCache Nlm_SetDocCache
#define SetDocData Nlm_SetDocData
#define GetDocData Nlm_GetDocData
#define GetDocText Nlm_GetDocText
#define MapDocPoint Nlm_MapDocPoint
#define PrintDocument Nlm_PrintDocument
#define SaveDocument Nlm_SaveDocument
#define GetDocParams Nlm_GetDocParams
#define GetItemParams Nlm_GetItemParams
#define GetColParams Nlm_GetColParams
#define AppendItem Nlm_AppendItem
#define AppendText Nlm_AppendText
#define ReplaceItem Nlm_ReplaceItem
#ifndef ReplaceText
#define ReplaceText Nlm_ReplaceText
#endif
#define InsertItem Nlm_InsertItem
#define InsertText Nlm_InsertText
#define DeleteItem Nlm_DeleteItem
#define ItemIsVisible Nlm_ItemIsVisible
#define GetScrlParams Nlm_GetScrlParams
#define UpdateDocument Nlm_UpdateDocument
#define AdjustDocScroll Nlm_AdjustDocScroll
#define SetDocAutoAdjust Nlm_SetDocAutoAdjust
#define SetDocVirtual Nlm_SetDocVirtual
#define SetDocTabstops Nlm_SetDocTabstops
#define DisplayFile Nlm_DisplayFile
#define DisplayFancy Nlm_DisplayFancy
 
#ifdef __cplusplus
}
#endif

#endif
