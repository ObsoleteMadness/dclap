/*   drawing.c
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
* File Name:  drawing.c
*
* Author:  Jonathan Kans, Jill Shermer
*
* Version Creation Date:   11/13/92
*
* $Revision: 1.13 $
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

#ifndef _DRAWINGP_
#include <drawingP.h>
#endif

#ifndef _VIEWERP_
#include <viewerP.h>
#endif

/*****************************************************************************
*
*   EXTERNAL VARIABLES
*
*****************************************************************************/

FonT  smallFont = NULL;
FonT  mediumFont = NULL;
FonT  largeFont = NULL;

/*****************************************************************************
*
*   SetXXXFont (void)
*       Creates fonts of various sizes.
*
*****************************************************************************/

FonT SetSmallFont (void)

{
  if (smallFont == NULL) {
#ifdef WIN_MAC
    smallFont = ParseFont ("Monaco,9");
#endif
#ifdef WIN_MSWIN
    smallFont = ParseFont ("Courier,9");
#endif
#ifdef WIN_MOTIF
    smallFont = ParseFont ("Courier,12");
#endif
  }
  return smallFont;
}

FonT SetMediumFont (void)

{
  if (mediumFont == NULL) {
#ifdef WIN_MAC
    mediumFont = ParseFont ("Monaco,12");
#endif
#ifdef WIN_MSWIN
    mediumFont = ParseFont ("Courier,12");
#endif
#ifdef WIN_MOTIF
    mediumFont = ParseFont ("Courier,14");
#endif
  }
  return mediumFont;
}

FonT SetLargeFont (void)

{
  if (largeFont == NULL) {
#ifdef WIN_MAC
    largeFont = ParseFont ("Monaco,18");
#endif
#ifdef WIN_MSWIN
    largeFont = ParseFont ("Courier,18");
#endif
#ifdef WIN_MOTIF
    largeFont = ParseFont ("Courier,18");
#endif
  }
  return largeFont;
}

/*****************************************************************************
*
*   DrawRectPrim (rec, highlight, scale)
*       Draws a rectangle primitive with an optional arrow.
*
*****************************************************************************/

static void DrawRectPrim (RecPPtr rec, Int1 highlight, ScaleInfo *scale)

{
  PoinT  pts [4];
  RecT   r;

  if (rec != NULL && scale != NULL) {
    if (BoxInViewport (&r, &(rec->box), &(rec->rct), scale)) {
      InsetRect (&r, -ARROW_PIXELS - ARROW_OVERHANG, -ARROW_PIXELS - ARROW_OVERHANG);
      if (scale->force || RectInRgn (&r, updateRgn)) {
        InsetRect (&r, ARROW_PIXELS + ARROW_OVERHANG, ARROW_PIXELS + ARROW_OVERHANG);
        switch (rec->arrow) {
          case NO_ARROW :
            break;
          case LEFT_ARROW :
            if (r.right - r.left >= ARROW_PIXELS) {
              r.left += ARROW_PIXELS;
            } else {
              r.right = r.left;
            }
            break;
          case RIGHT_ARROW :
            if (r.right - r.left >= ARROW_PIXELS) {
              r.right -= ARROW_PIXELS;
            } else {
              r.left = r.right;
            }
            break;
          case UP_ARROW :
            if (r.bottom - r.top >= ARROW_PIXELS) {
              r.top += ARROW_PIXELS;
            } else {
              r.bottom = r.top;
            }
            break;
          case DOWN_ARROW :
            if (r.bottom - r.top >= ARROW_PIXELS) {
              r.bottom -= ARROW_PIXELS;
            } else {
              r.top = r.bottom;
            }
            break;
          default :
            break;
        }
        if (rec->fill) {
          PaintRect (&r);
        } else {
          FrameRect (&r);
        }
        if (highlight == FRAME_CONTENTS || rec->highlight == FRAME_PRIMITIVE) {
          InsetRect (&r, -1, -1);
          FrameRect (&r);
          InsetRect (&r, 1, 1);
          FrameRect (&r);
          InsetRect (&r, 1, 1);
          FrameRect (&r);
          InsetRect (&r, -1, -1);
        } else if (highlight == FILL_CONTENTS || rec->highlight == FILL_PRIMITIVE) {
          InsetRect (&r, -1, -1);
          PaintRect (&r);
          InsetRect (&r, 1, 1);
        }
        if (rec->arrow >= LEFT_ARROW && rec->arrow <= DOWN_ARROW) {
          switch (rec->arrow) {
            case LEFT_ARROW :
              r.right = r.left;
              r.left = r.right - ARROW_PIXELS;
              r.bottom--;
              r.top -= ARROW_OVERHANG;
              r.bottom += ARROW_OVERHANG;
              LoadPt (&(pts [0]), r.right, r.top);
              LoadPt (&(pts [1]), r.right, r.bottom);
              LoadPt (&(pts [2]), r.left, (r.top + r.bottom) / 2);
              break;
            case RIGHT_ARROW :
              r.left = r.right - 1;
              r.right = r.left + ARROW_PIXELS;
              r.bottom--;
              r.top -= ARROW_OVERHANG;
              r.bottom += ARROW_OVERHANG;
              LoadPt (&(pts [0]), r.left, r.top);
              LoadPt (&(pts [1]), r.left, r.bottom);
              LoadPt (&(pts [2]), r.right, (r.top + r.bottom) / 2);
              break;
            case UP_ARROW :
              r.right--;
              r.bottom = r.top;
              r.top = r.bottom - ARROW_PIXELS;
              r.left -= ARROW_OVERHANG;
              r.right += ARROW_OVERHANG;
              LoadPt (&(pts [0]), r.left, r.bottom);
              LoadPt (&(pts [1]), r.right, r.bottom);
              LoadPt (&(pts [2]), (r.left + r.right) / 2, r.top);
              break;
            case DOWN_ARROW :
              r.right--;
              r.top = r.bottom - 1;
              r.bottom = r.top + ARROW_PIXELS;
              r.left -= ARROW_OVERHANG;
              r.right += ARROW_OVERHANG;
              LoadPt (&(pts [0]), r.left, r.top);
              LoadPt (&(pts [1]), r.right, r.top);
              LoadPt (&(pts [2]), (r.left + r.right) / 2, r.bottom);
              break;
            default :
              break;
          }
          if (rec->fill) {
            FramePoly (3, pts);
            PaintPoly (3, pts);
          } else {
            FramePoly (3, pts);
          }
        }
      }
    }
  }
}

/*****************************************************************************
*
*   DrawLinePrim (lin, penWidth, highlight, scale)
*       Draws a line primitive.  Since polygon framing encompasses more
*       area than painting, both operations are performed to form the
*       optional arrowhead.
*
*****************************************************************************/

static PoinT offsetByOne = {1, 1};

static void DrawLinePrim (LinPPtr lin, Int1 penWidth, Int1 highlight, ScaleInfo *scale)

{
  double   aspect;
  double   cosTheta;
  double   head;
  PoinT    pt1;
  PoinT    pt2;
  PoinT    pts [4];
  RecT     r;
  double   sinTheta;
  double   theta;

  if (lin != NULL && scale != NULL) {
    if (LineInViewport (&pt1, &pt2, &(lin->pnt1), &(lin->pnt2), &(lin->rct), scale)) {
      LoadRect (&r, MIN (pt1.x, pt2.x), MIN (pt1.y, pt2.y),
                MAX (pt1.x, pt2.x), MAX (pt1.y, pt2.y));
      InsetRect (&r, -6, -6);
      if (scale->force || RectInRgn (&r, updateRgn)) {
        DrawLine (pt1, pt2);
        if (highlight == FRAME_CONTENTS || highlight == FILL_CONTENTS ||
           lin->highlight == FRAME_PRIMITIVE || lin->highlight == FILL_PRIMITIVE) {
          WidePen (penWidth + 2);
          SubPt (offsetByOne, &pt1);
          SubPt (offsetByOne, &pt2);
          DrawLine (pt1, pt2);
          AddPt (offsetByOne, &pt1);
          AddPt (offsetByOne, &pt2);
          WidePen (penWidth);
        }
        if (lin->arrow && (pt1.x != pt2.x || pt1.y != pt2.y)) {
          if (penWidth > 0) {
            head = (double) (ARROW_LENGTH + penWidth);
          } else {
            head = (double) ARROW_LENGTH;
          }
          if (pt1.x != pt2.x) {
            theta = atan2 ((double) (pt2.y - pt1.y), (double) (pt2.x - pt1.x));
            cosTheta = cos (theta);
            sinTheta = sin (theta);
          } else if (pt2.y > pt1.y) {
            cosTheta = 0.0;
            sinTheta = 1.0;
          } else {
            cosTheta = 0.0;
            sinTheta = -1.0;
          }
          pts [0].x = pt2.x;
          pts [0].y = pt2.y;
          aspect = (double) ARROW_ASPECT;
          pts [1].x = pt2.x - (Int2) (head * (sinTheta + aspect * cosTheta));
          pts [1].y = pt2.y + (Int2) (head * (cosTheta - aspect * sinTheta));
          pts [2].x = pt2.x + (Int2) (head * (sinTheta - aspect * cosTheta));
          pts [2].y = pt2.y - (Int2) (head * (cosTheta + aspect * sinTheta));
          FramePoly (3, pts);
          PaintPoly (3, pts);
        }
      }
    }
  }
}

/*****************************************************************************
*
*   DrawBitmap (source, pt, rct, scale)
*       Draws a symbol or bitmap
*
*****************************************************************************/

static void DrawBitmap (Uint1Ptr source, PoinT pt, RectPtr rct, ScaleInfo *scale)

{
  RecT  r;

  if (source != NULL && rct != NULL && scale != NULL) {
    r = *rct;
    OffsetRect (&r, pt.x, pt.y);
    InsetRect (&r, -1, -1);
    if (scale->force || RectInRgn (&r, updateRgn)) {
      InsetRect (&r, 1, 1);
      CopyBits (&r, source);
    }
  }
}

/*****************************************************************************
*
*   DrawSymPrim (sym, scale)
*       Draws a symbol primitive
*
*****************************************************************************/

static void DrawSymPrim (SymPPtr sym, ScaleInfo *scale)

{
  PoinT  pt;

  if (sym != NULL && scale != NULL && sym->data != NULL) {
    if (PntInViewport (&pt, &(sym->pnt), &(sym->rct), scale)) {
      DrawBitmap (sym->data, pt, &(sym->rct), scale);
    }
  }
}

/*****************************************************************************
*
*   DrawBitmapPrim (btm, scale)
*       Draws a bitmap primitive
*
*****************************************************************************/

static void DrawBitmapPrim (BtmPPtr btm, ScaleInfo *scale)

{
  PoinT  pt;

  if (btm != NULL && scale != NULL && btm->data != NULL) {
    if (PntInViewport (&pt, &(btm->pnt), &(btm->rct), scale)) {
      DrawBitmap (btm->data, pt, &(btm->rct), scale);
    }
  }
}

/*****************************************************************************
*
*   DrawCustomPrim (cst, scale)
*       Draws a custom primitive
*
*****************************************************************************/

static void DrawCustomPrim (CstPPtr cst, ScaleInfo *scale)

{
  PoinT  pt;
  RecT   r;

  if (cst != NULL && scale != NULL && cst->proc != NULL) {
    if (PntInViewport (&pt, &(cst->pnt), &(cst->rct), scale)) {
      r = cst->rct;
      OffsetRect (&r, pt.x, pt.y);
      InsetRect (&r, -1, -1);
      if (scale->force || RectInRgn (&r, updateRgn)) {
        InsetRect (&r, 1, 1);
        cst->proc (r.left, r.top, r.right, r.bottom);
      }
    }
  }
}

/*****************************************************************************
*
*   DrawMarkerPrim (mrk, scale)
*       Draws a marker primitive
*
*****************************************************************************/

static void DrawMarkerPrim (MrkPPtr mrk, ScaleInfo *scale)

{
  PoinT  pt;
  RecT   r;

  if (mrk != NULL && scale != NULL) {
    if (PntInViewport (&pt, &(mrk->pnt), &(mrk->rct), scale)) {
      r = mrk->rct;
      OffsetRect (&r, pt.x, pt.y);
      InsetRect (&r, -1, -1);
      if (scale->force || RectInRgn (&r, updateRgn)) {
        InsetRect (&r, 1, 1);
        MoveTo (r.left, r.top);
        LineTo (r.right, r.bottom);
      }
    }
  }
}

/*****************************************************************************
*
*   DrawLabelPrim (lbl, scale)
*       Draws a label primitive
*
*****************************************************************************/

static void DrawLabelPrim (LblPPtr lbl, ScaleInfo *scale)

{
  FonT     fnt;
  PoinT    pt;
  RecT     r;
  CharPtr  str;

  if (lbl != NULL && scale != NULL && lbl->str != NULL) {
    if (PntInViewport (&pt, &(lbl->pnt), &(lbl->rct), scale)) {
      r = lbl->rct;
      OffsetRect (&r, pt.x, pt.y);
      InsetRect (&r, -1, -1);
      if (scale->force || RectInRgn (&r, updateRgn)) {
        InsetRect (&r, 1, 1);
        str = lbl->str;
        switch (lbl->size) {
          case SMALL_TEXT:
            if (smallFont == NULL) {
              SetSmallFont ();
            }
            fnt = smallFont;
            break;
          case MEDIUM_TEXT:
            if (mediumFont == NULL) {
              SetMediumFont ();
            }
            fnt = mediumFont;
            break;
          case LARGE_TEXT:
            if (largeFont == NULL) {
              SetLargeFont ();
            }
            fnt = largeFont;
            break;
          default :
            fnt = programFont;
            break;
        }
        SelectFont (fnt);
        DrawString (&r, str, 'l', FALSE);
      }
    }
  }
}

/*****************************************************************************
*
*   DrawPrimitive (item, atts, scale)
*       Draws a primitive
*
*****************************************************************************/

void DrawPrimitive (BasePPtr item, AttPData *atts, ScaleInfo *scale)

{
  if (item != NULL && atts != NULL && scale != NULL) {
    switch (item->code) {
      case RECTANGLE :
        DrawRectPrim ((RecPPtr) item, atts->highlight, scale);
        break;
      case LINE :
        DrawLinePrim ((LinPPtr) item, atts->penwidth, atts->highlight, scale);
        break;
      case SYMBOL :
        DrawSymPrim ((SymPPtr) item, scale);
        break;
      case BITMAP :
        DrawBitmapPrim ((BtmPPtr) item, scale);
        break;
      case CUSTOM :
        DrawCustomPrim ((CstPPtr) item, scale);
        break;
      case MARKER :
        DrawMarkerPrim ((MrkPPtr) item, scale);
        break;
      case LABEL :
        DrawLabelPrim ((LblPPtr) item, scale);
        break;
      default :
        Message (MSG_ERROR, "DrawPrimitive item unknown");
        break;
    }
  }
}
