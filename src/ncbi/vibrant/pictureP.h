/*   pictureP.h
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
* File Name:  pictureP.h
*
* Author:  Jonathan Kans, Jill Shermer
*
* Version Creation Date:   10/23/92
*
* $Revision: 1.9 $
*
* File Description: 
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _PICTUREP_
#define _PICTUREP_

#ifndef _VIBRANT_
#include <vibrant.h>
#endif

#ifndef _PICTURE_
#include <picture.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   INTERNAL TYPE DEFINES
*
*****************************************************************************/

/*  Primitive types */

#define UNKNOWN     0
#define PICTURE     1
#define SEGMENT     2
#define ATTRIBUTE   3
#define RECTANGLE   4
#define LINE        5
#define SYMBOL      6
#define BITMAP      7
#define CUSTOM      8
#define MARKER      9
#define LABEL      10

#define SYMBOL_WIDTH   7
#define SYMBOL_HEIGHT  7

#define ARROW_PIXELS    6
#define ARROW_OVERHANG  0

#define ARROW_ASPECT  3
#define ARROW_LENGTH  4

#define PEN_MAX  6

/*****************************************************************************
*
*   STRUCTURE TYPEDEFS
*
*****************************************************************************/

/*  BasePRec is the base record of each primitive,
    storing the next pointer and an identity code */

typedef struct Nlm_baseprec {
  struct Nlm_baseprec  PNTR next;
  Nlm_Int1                  code;
} Nlm_BasePData, Nlm_BasePRec, PNTR Nlm_BasePPtr;

typedef struct Nlm_segpdata {
  Nlm_BoxInfo   box;
  Nlm_BasePPtr  head;
  Nlm_BasePPtr  tail;
  Nlm_BasePPtr  parent;
  Nlm_Boolean   visible;
  Nlm_RecT      margin;
  Nlm_Int4      maxscale;
  Nlm_Int1      penwidth;
  Nlm_Int1      highlight;
  Nlm_Uint2     segID;
} Nlm_SegPData;

typedef struct Nlm_segprec {
  Nlm_BasePData  base;
  Nlm_SegPData   seg;
} Nlm_SegPRec, Nlm_PicPRec, PNTR Nlm_SegPPtr, PNTR Nlm_PicPPtr;

typedef struct Nlm_attpdata {
  Nlm_Uint1  flags;
  Nlm_Uint1  color [3];
  Nlm_Int1   linestyle;
  Nlm_Int1   shading;
  Nlm_Int1   penwidth;
  Nlm_Int1   mode;
  Nlm_Int1   highlight;
} Nlm_AttPData;

typedef struct Nlm_attprec {
  Nlm_BasePData  base;
  Nlm_AttPData   att;
} Nlm_AttPRec, PNTR Nlm_AttPPtr;

typedef struct Nlm_recprec {
  Nlm_BasePData  base;
  Nlm_BoxInfo    box;
  Nlm_RecT       rct;
  Nlm_Int2       arrow;
  Nlm_Boolean    fill;
  Nlm_Int1       highlight;
  Nlm_Uint2      primID;
} Nlm_RecPRec, PNTR Nlm_RecPPtr;

typedef struct Nlm_linprec {
  Nlm_BasePData  base;
  Nlm_PntInfo    pnt1;
  Nlm_PntInfo    pnt2;
  Nlm_RecT       rct;
  Nlm_Boolean    arrow;
  Nlm_Int1       highlight;
  Nlm_Uint2      primID;
} Nlm_LinPRec, PNTR Nlm_LinPPtr;

typedef struct Nlm_symprec {
  Nlm_BasePData  base;
  Nlm_PntInfo    pnt;
  Nlm_RecT       rct;
  Nlm_Uint1Ptr   data;
  Nlm_Uint2      primID;
} Nlm_SymPRec, PNTR Nlm_SymPPtr;

typedef struct Nlm_btmprec {
  Nlm_BasePData  base;
  Nlm_PntInfo    pnt;
  Nlm_RecT       rct;
  Nlm_Uint1Ptr   data;
  Nlm_Uint2      primID;
} Nlm_BtmPRec, PNTR Nlm_BtmPPtr;

typedef struct Nlm_cstprec {
  Nlm_BasePData   base;
  Nlm_PntInfo     pnt;
  Nlm_RecT        rct;
  Nlm_CustomProc  proc;
  Nlm_Uint2       primID;
} Nlm_CstPRec, PNTR Nlm_CstPPtr;

typedef struct Nlm_mrkprec {
  Nlm_BasePData  base;
  Nlm_PntInfo    pnt;
  Nlm_RecT       rct;
  Nlm_Uint2      primID;
} Nlm_MrkPRec, PNTR Nlm_MrkPPtr;

typedef struct Nlm_lblprec {
  Nlm_BasePData  base;
  Nlm_PntInfo    pnt;
  Nlm_CharPtr    str;
  Nlm_Int2       size;
  Nlm_RecT       rct;
  Nlm_Uint2      primID;
} Nlm_LblPRec, PNTR Nlm_LblPPtr;

#define BasePData Nlm_BasePData
#define BasePRec Nlm_BasePRec
#define BasePPtr Nlm_BasePPtr
#define SegPData Nlm_SegPData
#define SegPRec Nlm_SegPRec
#define PicPRec Nlm_PicPRec
#define SegPPtr Nlm_SegPPtr
#define PicPPtr Nlm_PicPPtr
#define AttPData Nlm_AttPData
#define AttPRec Nlm_AttPRec
#define AttPPtr Nlm_AttPPtr
#define RecPRec Nlm_RecPRec
#define RecPPtr Nlm_RecPPtr
#define LinPRec Nlm_LinPRec
#define LinPPtr Nlm_LinPPtr
#define SymPRec Nlm_SymPRec
#define SymPPtr Nlm_SymPPtr
#define BtmPRec Nlm_BtmPRec
#define BtmPPtr Nlm_BtmPPtr
#define CstPRec Nlm_CstPRec
#define CstPPtr Nlm_CstPPtr
#define MrkPRec Nlm_MrkPRec
#define MrkPPtr Nlm_MrkPPtr
#define LblPRec Nlm_LblPRec
#define LblPPtr Nlm_LblPPtr

#ifdef __cplusplus
}
#endif

#endif
