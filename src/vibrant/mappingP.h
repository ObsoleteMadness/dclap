/*   mappingP.h
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
* File Name:  mappingP.h
*
* Author:  Jonathan Kans, Jill Shermer
*
* Version Creation Date:   1/19/93
*
* $Revision: 1.6 $
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

#ifndef _MAPPINGP_
#define _MAPPINGP_

#ifndef _VIBRANT_
#include <vibrant.h>
#endif

#ifndef _PICTURE_
#include <picture.h>
#endif

#ifndef _PICTUREP_
#include <pictureP.h>
#endif

/*****************************************************************************
*
*   STRUCTURE TYPEDEFS
*
*****************************************************************************/

typedef struct Nlm_scaleinfo {
  Nlm_BoxInfo  world;
  Nlm_RecT     view;
  Nlm_BoxInfo  port;
  Nlm_Int4     scaleX;
  Nlm_Int4     scaleY;
  Nlm_Int2     scrollX;
  Nlm_Int2     scrollY;
  Nlm_Boolean  force;
} Nlm_ScaleInfo, PNTR Nlm_ScalePtr;

/*****************************************************************************
*
*   FUNCTION PROTOTYPES
*
*****************************************************************************/

extern Nlm_Boolean Nlm_BoxInViewport        PROTO((Nlm_RectPtr rct, Nlm_BoxPtr box, Nlm_RectPtr mrg, Nlm_ScaleInfo *scale));
extern Nlm_Boolean Nlm_LineInViewport       PROTO((Nlm_PointPtr pt1, Nlm_PointPtr pt2, Nlm_PntPtr pnt1, Nlm_PntPtr pnt2, Nlm_RectPtr mrg, Nlm_ScaleInfo *scale));
extern Nlm_Boolean Nlm_PntInViewport        PROTO((Nlm_PointPtr pt, Nlm_PntPtr pnt, Nlm_RectPtr mrg, Nlm_ScaleInfo *scale));

extern void        Nlm_MapWorldPointToPixel PROTO((Nlm_PointPtr pt, Nlm_PntPtr pnt, Nlm_ScaleInfo *scale));
extern void        Nlm_MapPixelPointToWorld PROTO((Nlm_PntPtr pnt, Nlm_PointPtr pt, Nlm_ScaleInfo *scale));

extern Nlm_Boolean Nlm_PntInBox             PROTO((Nlm_BoxPtr box, Nlm_PntPtr pnt, Nlm_RectPtr mrg, Nlm_ScaleInfo *scale));
extern void        Nlm_MapToRect            PROTO((Nlm_RectPtr r, Nlm_PoinT pt, Nlm_Int2 width, Nlm_Int2 height, Nlm_Int2 offset, Nlm_Uint2 align));

#define ScaleInfo Nlm_ScaleInfo
#define ScalePtr Nlm_ScalePtr
#define BoxInViewport Nlm_BoxInViewport
#define LineInViewport Nlm_LineInViewport
#define PntInViewport Nlm_PntInViewport
#define MapWorldPointToPixel Nlm_MapWorldPointToPixel
#define MapPixelPointToWorld Nlm_MapPixelPointToWorld
#define PntInBox Nlm_PntInBox
#define MapToRect Nlm_MapToRect

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
