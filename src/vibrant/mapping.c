/*   mapping.c
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
* File Name:  mapping.c
*
* Author:  Jonathan Kans, Jill Shermer
*
* Version Creation Date:   1/19/93
*
* $Revision: 1.12 $
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

#ifndef _VIBRANT_
#include <vibrant.h>
#endif

#ifndef _PICTURE_
#include <picture.h>
#endif

#ifndef _PICTUREP_
#include <pictureP.h>
#endif

#ifndef _MAPPINGP_
#include <mappingP.h>
#endif

/*****************************************************************************
*
*   STATIC VARIABLES
*
*****************************************************************************/

static RecT  emptyRect = {0, 0, 0, 0};

/*****************************************************************************
*
*   LoadBox (box, left, top, right, bottom)
*       Initializes a box structure with the specified parameters
*
*****************************************************************************/

static void LoadBox (BoxPtr box, Int4 left, Int4 top, Int4 right, Int4 bottom)

{
  if (box != NULL) {
    box->left = left;
    box->top = top;
    box->right = right;
    box->bottom = bottom;
  }
}

/*****************************************************************************
*
*   MapX (pntX, scale)
*       Maps a horizontal world coordinate to an Int4 in viewer coordinates
*
*****************************************************************************/

static Int4 MapX (Int4 pntX, ScaleInfo *scale)

{
  return (Int4) scale->view.left + (pntX - scale->port.left) / scale->scaleX;
}

/*****************************************************************************
*
*   MapY (pntY, scale)
*       Maps a vertical world coordinate to an Int4 in viewer coordinates
*
*****************************************************************************/

static Int4 MapY (Int4 pntY, ScaleInfo *scale)

{
  return (Int4) scale->view.bottom - (pntY - scale->port.bottom) / scale->scaleY;
}

/*****************************************************************************
*
*   BoxInViewport (rct, box, mrg, scale)
*       Determines whether a box is visible in the viewport
*
*****************************************************************************/

Boolean BoxInViewport (RectPtr rct, BoxPtr box, RectPtr mrg, ScaleInfo *scale)

{
  if (box != NULL && mrg != NULL && scale != NULL &&
    MAX (box->left + (Int4) mrg->left * scale->scaleX, scale->port.left) <=
    MIN (box->right + (Int4) mrg->right * scale->scaleX, scale->port.right) &&
    MAX (box->bottom - (Int4) mrg->bottom * scale->scaleY, scale->port.bottom) <=
    MIN (box->top - (Int4) mrg->top * scale->scaleY, scale->port.top)) {
    if (rct != NULL) {
      rct->left = (Int2) MAX (MapX (box->left, scale), (Int4) (scale->view.left - 20));
      rct->top = (Int2) MAX (MapY (box->top, scale), (Int4) (scale->view.top - 20));
      rct->right = (Int2) MIN (MapX (box->right, scale), (Int4) (scale->view.right + 20));
      rct->bottom = (Int2) MIN (MapY (box->bottom, scale), (Int4) (scale->view.bottom + 20));
    }
    return TRUE;
  } else {
    return FALSE;
  }
}

/*****************************************************************************
*
*   ClipPoint (clp1, clp2, mrg, scale)
*       Clips the first pnt of a line in world coordinates to the port
*
*****************************************************************************/

static Boolean ClipPoint (PntPtr clp1, PntPtr clp2, RectPtr mrg, ScaleInfo *scale)

{
  BoxInfo  bounds;
  BoxInfo  box;
  Int4     deltaX;
  Int4     deltaY;
  Int2     maxClip;

  if (clp1 != NULL && clp2 != NULL && scale != NULL) {
    LoadBox (&bounds,
             scale->port.left - 20 * scale->scaleX, scale->port.top + 20 * scale->scaleY,
             scale->port.right + 20 * scale->scaleX, scale->port.bottom - 20 * scale->scaleY);
    maxClip = 2;
    do {
      LoadBox (&box, MIN (clp1->x, clp2->x), MAX (clp1->y, clp2->y),
               MAX (clp1->x, clp2->x), MIN (clp1->y, clp2->y));
      if (BoxInViewport (NULL, &box, mrg, scale)) {
        if (PntInBox (&bounds, clp1, mrg, scale)) {
          return TRUE;
        } else {
          deltaX = (clp2->x - clp1->x);
          deltaY = (clp2->y - clp1->y);
          if (clp1->y > bounds.top) {
            clp1->x += deltaX * (bounds.top - clp1->y) / deltaY;
            clp1->y = bounds.top;
          } else if (clp1->y < bounds.bottom) {
            clp1->x += deltaX * (bounds.bottom - clp1->y) / deltaY;
            clp1->y = bounds.bottom;
          } else if (clp1->x > bounds.right) {
            clp1->y += deltaY * (bounds.right - clp1->x) / deltaX;
            clp1->x = bounds.right;
          } else if (clp1->x < bounds.left) {
            clp1->y += deltaY * (bounds.bottom - clp1->x) / deltaX;
            clp1->x = bounds.bottom;
          }
        }
      } else {
        return FALSE;
      }
      maxClip--;
    } while (maxClip > 0);
  }
  if (PntInBox (&bounds, clp1, mrg, scale)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/*****************************************************************************
*
*   LineInViewport (pt1, pt2, pnt1, pnt2, mrg, scale)
*       Determines whether a line is visible in the viewport
*
*****************************************************************************/

Boolean LineInViewport (PointPtr pt1, PointPtr pt2, PntPtr pnt1,
                        PntPtr pnt2, RectPtr mrg, ScaleInfo *scale)

{
  BoxInfo  box;
  PntInfo  clp1;
  PntInfo  clp2;

  if (pnt1 != NULL && pnt2 != NULL && mrg != NULL && scale != NULL) {
    clp1 = *(pnt1);
    clp2 = *(pnt2);
    LoadBox (&box, MIN (clp1.x, clp2.x), MAX (clp1.y, clp2.y),
             MAX (clp1.x, clp2.x), MIN (clp1.y, clp2.y));
    if (BoxInViewport (NULL, &box, mrg, scale)) {
      if (clp1.x == clp2.x) {
        if (pt1 != NULL && pt2 != NULL) {
          pt1->x = pt2->x = (Int2) MapX (clp1.x, scale);
          if (clp1.y > clp2.y) {
            pt1->y = (Int2) MAX (MapY (clp1.y, scale), (Int4) scale->view.top - 20);
            pt2->y = (Int2) MIN (MapY (clp2.y, scale), (Int4) scale->view.bottom + 20);
          } else if (clp1.y < clp2.y) {
            pt2->y = (Int2) MAX (MapY (clp2.y, scale), (Int4) scale->view.top - 20);
            pt1->y = (Int2) MIN (MapY (clp1.y, scale), (Int4) scale->view.bottom + 20);
          } else {
            pt1->y = pt2->y = (Int2) MapY (clp1.y, scale);
          }
        }
        return TRUE;
      } else if (clp1.y == clp2.y) {
        if (pt1 != NULL && pt2 != NULL) {
          pt1->y = pt2->y = (Int2) MapY (clp1.y, scale);
          if (clp1.x > clp2.x) {
            pt1->x = (Int2) MIN (MapX (clp1.x, scale), (Int4) scale->view.right + 20);
            pt2->x = (Int2) MAX (MapX (clp2.x, scale), (Int4) scale->view.left - 20);
          } else if (clp1.x < clp2.x) {
            pt2->x = (Int2) MIN (MapX (clp2.x, scale), (Int4) scale->view.right + 20);
            pt1->x = (Int2) MAX (MapX (clp1.x, scale), (Int4) scale->view.left - 20);
          } else {
            pt1->x = pt2->x = (Int2) MapX (clp1.x, scale);
          }
        }
        return TRUE;
      } else if (ClipPoint (&clp1, &clp2, mrg, scale) && ClipPoint (&clp2, &clp1, mrg, scale)) {
        if (pt1 != NULL && pt2 != NULL) {
          pt1->x = (Int2) MapX (clp1.x, scale);
          pt1->y = (Int2) MapY (clp1.y, scale);
          pt2->x = (Int2) MapX (clp2.x, scale);
          pt2->y = (Int2) MapY (clp2.y, scale);
        }
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return FALSE;
    }
  } else {
    return FALSE;
  }
}

/*****************************************************************************
*
*   PntInViewport (pt, pnt, mrg, scale)
*       Determines whether a pnt is visible in the viewport
*
*****************************************************************************/

Boolean PntInViewport (PointPtr pt, PntPtr pnt, RectPtr mrg, ScaleInfo *scale)

{
  if (pnt != NULL && mrg != NULL && scale != NULL &&
    MAX (pnt->x + (Int4) mrg->left * scale->scaleX, scale->port.left) <=
    MIN (pnt->x + (Int4) mrg->right * scale->scaleX, scale->port.right) &&
    MAX (pnt->y - (Int4) mrg->bottom * scale->scaleY, scale->port.bottom) <=
    MIN (pnt->y - (Int4) mrg->top * scale->scaleY, scale->port.top)) {
    if (pt != NULL) {
      pt->x = (Int2) MapX (pnt->x, scale);
      pt->y = (Int2) MapY (pnt->y, scale);
    }
    return TRUE;
  } else {
    return FALSE;
  }
}

/*****************************************************************************
*
*   MapWorldPointToPixel (pt, pnt, scale)
*       Maps a pnt in world coordinates to a point in viewer coordinates
*
*****************************************************************************/

void MapWorldPointToPixel (PointPtr pt, PntPtr pnt, ScaleInfo *scale)

{
  if (pt != NULL && pnt != NULL && scale != NULL) {
    pt->x = (Int2) MapX (pnt->x, scale);
    pt->y = (Int2) MapY (pnt->y, scale);
  }
}

/*****************************************************************************
*
*   MapPixelPointToWorld (pnt, pt, scale)
*       Maps a point in viewer coordinates to a pnt in world coordinates
*
*****************************************************************************/

void MapPixelPointToWorld (PntPtr pnt, PointPtr pt, ScaleInfo *scale)

{
  if (pnt != NULL && pt != NULL && scale != NULL) {
    pnt->x = (Int4) (pt->x - scale->view.left) * scale->scaleX + scale->port.left;
    pnt->y = (Int4) (scale->view.bottom - pt->y) * scale->scaleY + scale->port.bottom;
  }
}

/*****************************************************************************
*
*   PntInBox (box, pnt, mrg, scale)
*       Determines whether a box contains a pnt in world coordinates
*
*****************************************************************************/

Boolean PntInBox (BoxPtr box, PntPtr pnt, RectPtr mrg, ScaleInfo *scale)

{
  if (box != NULL && pnt != NULL && mrg != NULL && scale != NULL) {
    return (Boolean) (pnt->x >= box->left + (Int4) mrg->left * scale->scaleX &&
                      pnt->x <= box->right + (Int4) mrg->right * scale->scaleX &&
                      pnt->y <= box->top - (Int4) mrg->top * scale->scaleY &&
                      pnt->y >= box->bottom - (Int4) mrg->bottom * scale->scaleY);
  } else {
    return FALSE;
  }
}

/*****************************************************************************
*
*   MapToRect (r, pt, width, height, offset, align)
*       Creates a rectangle from point, width, height, offset, and align parameters
*
*****************************************************************************/

void MapToRect (RectPtr r, PoinT pt, Int2 width,
                Int2 height, Int2 offset, Uint2 align)

{
  Int2  left;
  Int2  top;

  if (r != NULL) {
    if (align >= MIDDLE_CENTER && align <= MIDDLE_RIGHT) {
      switch (align) {
        case MIDDLE_CENTER :
          left = pt.x - width / 2;
          top = pt.y - height / 2;
          LoadRect (r, left, top, left + width, top + height);
          OffsetRect (r, 0, 0);
          break;
        case UPPER_LEFT :
          LoadRect (r, pt.x - width, pt.y - height, pt.x, pt.y);
          OffsetRect (r, -offset, -offset);
          break;
        case LOWER_LEFT :
          LoadRect (r, pt.x - width, pt.y, pt.x, pt.y + height);
          OffsetRect (r, -offset, offset);
          break;
        case UPPER_RIGHT :
          LoadRect (r, pt.x, pt.y - height, pt.x + width, pt.y);
          OffsetRect (r, offset, -offset);
          break;
        case LOWER_RIGHT :
          LoadRect (r, pt.x, pt.y, pt.x + width, pt.y + height);
          OffsetRect (r, offset, offset);
          break;
        case UPPER_CENTER :
          left = pt.x - width / 2;
          LoadRect (r, left, pt.y - height, left + width, pt.y);
          OffsetRect (r, 0, -offset);
          break;
        case LOWER_CENTER :
          left = pt.x - width / 2;
          LoadRect (r, left, pt.y, left + width, pt.y + height);
          OffsetRect (r, 0, offset);
          break;
        case MIDDLE_LEFT :
          top = pt.y - height / 2;
          LoadRect (r, pt.x - width, top, pt.x, top + height);
          OffsetRect (r, -offset, 0);
          break;
        case MIDDLE_RIGHT :
          top = pt.y - height / 2;
          LoadRect (r, pt.x, top, pt.x + width, top + height);
          OffsetRect (r, offset, 0);
          break;
        default :
          break;
      }
    } else {
      LoadRect (r, 0, 0, 0, 0);
      Message (MSG_ERROR, "Undefined align value");
    }
  }
}
