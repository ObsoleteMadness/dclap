/*   viewer.c
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
* File Name:  viewer.c
*
* Author:  Jonathan Kans, Jill Shermer
*
* Version Creation Date:   10/25/92
*
* $Revision: 1.36 $
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

#ifndef _VIEWER_
#include <viewer.h>
#endif

#ifndef _VIEWERP_
#include <viewerP.h>
#endif

/*****************************************************************************
*
*   INTERNAL TYPE DEFINES
*
*****************************************************************************/

#ifdef WIN_MOTIF
#define LINE_PROXIMITY    12
#define RECT_PROXIMITY     6
#define GENERAL_PROXIMITY  6
#else
#define LINE_PROXIMITY     6
#define RECT_PROXIMITY     3
#define GENERAL_PROXIMITY  3
#endif

/*****************************************************************************
*
*   ViewerVScrollProc (sb, viewer, newval, oldval)
*
*****************************************************************************/

static void ViewerVScrollProc (BaR sb, SlatE viewer, Int2 newval, Int2 oldval)

{
  ViewPData  extra;
  Int2       height;
  Int4       lineHeight;
  Int4       offsetY;
  Int2       pixels;
  RecT       r;
  Int2       visLines;

  if (viewer != NULL && oldval != newval) {
    if (Visible (viewer) && AllParentsVisible (viewer)) {
      GetPanelExtra ((PaneL) viewer, &extra);
      ObjectRect (viewer, &r);
      InsetRect (&r, 4, 4);
      height = r.bottom - r.top;
      lineHeight = extra.scale.scaleY * (Int4) extra.scale.scrollY;
      visLines = (height) / extra.scale.scrollY;
      Select (viewer);
      if ((newval > oldval && newval - visLines + 1 <= oldval) ||
          (newval < oldval && newval + visLines - 1 >= oldval)) {
        pixels = (newval - oldval) * extra.scale.scrollY;
        if (ABS (pixels) < height) {
          ScrollRect (&r, 0, -pixels);
          if (pixels > 0) {
            r.top = r.bottom - pixels;
          } else {
            r.bottom = r.top - pixels;
          }
          InsetRect (&r, -1, -1);
          InvalRect (&r);
        } else {
          InsetRect (&r, -1, -1);
          InvalRect (&r);
        }
      } else {
        InsetRect (&r, -1, -1);
        InvalRect (&r);
      }
      offsetY = lineHeight * (Int4) (newval - oldval);
      extra.scale.port.top -= offsetY;
      extra.scale.port.bottom -= offsetY;
      SetPanelExtra ((PaneL) viewer, &extra);
      Update ();
      if (extra.pan != NULL) {
        extra.pan ((VieweR) viewer, extra.picture);
      }
    }
  }
}

/*****************************************************************************
*
*   ViewerHScrollProc (sb, viewer, newval, oldval)
*
*****************************************************************************/

static void ViewerHScrollProc (BaR sb, SlatE viewer, Int2 newval, Int2 oldval)

{
  ViewPData  extra;
  Int4       lineWidth;
  Int4       offsetX;
  Int2       pixels;
  RecT       r;
  Int2       visLines;
  Int2       width;

  if (viewer != NULL && oldval != newval) {
    if (Visible (viewer) && AllParentsVisible (viewer)) {
      GetPanelExtra ((PaneL) viewer, &extra);
      ObjectRect (viewer, &r);
      InsetRect (&r, 4, 4);
      width = r.right - r.left;
      lineWidth = extra.scale.scaleX * (Int4) extra.scale.scrollX;
      visLines = (width) / extra.scale.scrollX;
      Select (viewer);
      if ((newval > oldval && newval - visLines + 1 <= oldval) ||
          (newval < oldval && newval + visLines - 1 >= oldval)) {
        pixels = (newval - oldval) * extra.scale.scrollX;
        if (ABS (pixels) < width) {
          ScrollRect (&r, -pixels, 0);
          if (pixels > 0) {
            r.left = r.right - pixels;
          } else {
            r.right = r.left - pixels;
          }
          InsetRect (&r, -1, -1);
          InvalRect (&r);
        } else {
          InsetRect (&r, -1, -1);
          InvalRect (&r);
        }
      } else {
        InsetRect (&r, -1, -1);
        InvalRect (&r);
      }
      offsetX = lineWidth * (Int4) (newval - oldval);
      extra.scale.port.left += offsetX;
      extra.scale.port.right += offsetX;
      SetPanelExtra ((PaneL) viewer, &extra);
      Update ();
      if (extra.pan != NULL) {
        extra.pan ((VieweR) viewer, extra.picture);
      }
    }
  }
}

/*****************************************************************************
*
*   SetAttributes (atts, current)
*       Sets the drawing system to the specified attributes
*
*****************************************************************************/

static void SetAttributes (AttPData *atts, AttPData *current)

{
  if (atts != NULL && current != NULL) {
    if (atts->color [0] != current->color [0] ||
        atts->color [1] != current->color [1] ||
        atts->color [2] != current->color [2] ) {
      SelectColor (atts->color [0], atts->color [1], atts->color [2]);
      current->color [0] = atts->color [0];
      current->color [1] = atts->color [1];
      current->color [2] = atts->color [2];
    }
    if (atts->linestyle != current->linestyle) {
      switch (atts->linestyle) {
        case NO_LINE_STYLE :
          break;
        case SOLID_LINE :
          Solid ();
          break;
        case DOTTED_LINE :
          Dotted ();
          break;
        case DASHED_LINE :
          Dashed ();
          break;
        default :
          break;
      }
      current->linestyle = atts->linestyle;
    }
    if (atts->penwidth != current->penwidth) {
      WidePen (atts->penwidth);
      current->penwidth = atts->penwidth;
    }
    if (atts->shading != current->shading) {
      switch (atts->shading) {
        case NO_SHADING :
          break;
        case SOLID_SHADING :
          Solid ();
          break;
        case DARK_SHADING :
          Dark ();
          break;
        case MEDIUM_SHADING :
          Medium ();
          break;
        case LIGHT_SHADING :
          Light ();
          break;
        case EMPTY_SHADING :
          Empty ();
          break;
        default :
          break;
      }
      current->shading = atts->shading;
    }
    if (atts->mode != current->mode) {
      switch (atts->mode) {
        case NO_MODE :
          break;
        case COPY_MODE :
          CopyMode ();
          break;
        case MERGE_MODE :
          MergeMode ();
          break;
        case INVERT_MODE :
          InvertMode ();
          break;
        case ERASE_MODE :
          EraseMode ();
          break;
        default :
          break;
      }
      current->mode = atts->mode;
    }
    if (atts->highlight != current->highlight) {
      current->highlight = atts->highlight;
    }
  }
}

/*****************************************************************************
*
*   SegmentAndParentsVisible (seg)
*       Returns true if segment and all parent segments are visible
*
*****************************************************************************/

static Boolean SegmentAndParentsVisible (SegPPtr seg)

{
  Boolean  vis;

  vis = TRUE;
  while (seg != NULL && vis) {
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      vis =  seg->seg.visible;
    } else {
      Message (MSG_ERROR, "SegmentAndParentsVisible argument not a segment or picture");
    }
    seg = (SegPPtr) seg->seg.parent;
  }
  return vis;
}

/*****************************************************************************
*
*   DrawSegment (seg, atts, current, scale)
*       Draws a segment, recursively tracking attributes
*
*****************************************************************************/

static void DrawSegment (SegPPtr seg, AttPData *atts, AttPData *current, ScaleInfo *scale)

{
  AttPPtr   att;
  AttPData  attributes;
  BasePPtr  item;
  RecT      r;

  if (seg != NULL && atts != NULL && current != NULL && scale != NULL) {
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      if (seg->seg.maxscale == 0 || seg->seg.maxscale >= MAX (scale->scaleX, scale->scaleY)) {
      if (BoxInViewport (&r, &(seg->seg.box), &(seg->seg.margin), scale)) {
          attributes = *(atts);
          if (seg->seg.highlight != PLAIN_SEGMENT) {
            attributes.highlight = seg->seg.highlight;
          }
          SetAttributes (&attributes, current);
          if (seg->seg.visible && SegmentAndParentsVisible (seg)) {
            item = seg->seg.head;
            while (item != NULL) {
              switch (item->code) {
                case UNKNOWN :
                  Message (MSG_ERROR, "DrawSegment child is unknown");
                  break;
                case PICTURE :
                  Message (MSG_ERROR, "DrawSegment child is a picture");
                  break;
                case SEGMENT :
                  DrawSegment ((SegPPtr) item, &attributes, current, scale);
                  SetAttributes (&attributes, current);
                  break;
                case ATTRIBUTE :
                  att = (AttPPtr) item;
                  attributes.flags = att->att.flags;
                  if (attributes.flags & COLOR_ATT) {
                    attributes.color [0] = att->att.color [0];
                    attributes.color [1] = att->att.color [1];
                    attributes.color [2] = att->att.color [2];
                  }
                  if (attributes.flags & STYLE_ATT) {
                    attributes.linestyle = att->att.linestyle;
                  }
                  if (attributes.flags & SHADING_ATT) {
                    attributes.shading = att->att.shading;
                  }
                  if (attributes.flags & WIDTH_ATT) {
                    attributes.penwidth = att->att.penwidth;
                  }
                  if (attributes.flags & MODE_ATT) {
                    attributes.mode = att->att.mode;
                  }
                  SetAttributes (&attributes, current);
                  break;
                default :
                  DrawPrimitive (item, &attributes, scale);
                  break;
              }
              item = item->next;
            }
            InsetRect (&r, -1, -1);
            if (scale->force || RectInRgn (&r, updateRgn)) {
              switch (seg->seg.highlight) {
                case PLAIN_SEGMENT :
                  break;
                case FRAME_SEGMENT :
                  FrameRect (&r);
                  break;
                case FILL_SEGMENT :
                  PaintRect (&r);
                  break;
                default :
                  break;
              }
            }
          }
        }
      }
    } else {
      Message (MSG_ERROR, "DrawSegment argument not a segment or picture");
    }
  }
}

/*****************************************************************************
*
*   DrawViewerProc (viewer)
*       Draws the picture in a viewer panel.  Clipping is to the intersection
*       of the updateRgn (the portion of the panel exposed) and the inset area
*       of the viewer, leaving a four-pixel margin.
*
*****************************************************************************/

static void DrawViewerProc (PaneL viewer)

{
  AttPData   attributes;
  AttPData   current;
  RegioN     dst;
  ViewPData  extra;
  PicPPtr    pic;
  RegioN     src;

  if (viewer != NULL) {
    GetPanelExtra (viewer, &extra);
    pic = (PicPPtr) extra.picture;
    if (pic != NULL) {
      if (pic->base.code == PICTURE) {
        src = CreateRgn ();
        dst = CreateRgn ();
        LoadRectRgn (src, extra.scale.view.left, extra.scale.view.top,
                     extra.scale.view.right, extra.scale.view.bottom);
        SectRgn (src, updateRgn, dst);
        ClipRgn (dst);
        DestroyRgn (src);
        DestroyRgn (dst);
        attributes.flags = NO_ATTS;
        attributes.color [0] = BLACK_COLOR [0];
        attributes.color [1] = BLACK_COLOR [1];
        attributes.color [2] = BLACK_COLOR [2];
        attributes.linestyle = SOLID_LINE;
        attributes.shading = SOLID_SHADING;
        attributes.penwidth = STD_PEN_WIDTH;
        attributes.mode = COPY_MODE;
        attributes.highlight = PLAIN_SEGMENT;
        current = *(&attributes);
        DrawSegment ((SegPPtr) pic, &attributes, &current, &(extra.scale));
        SetAttributes (&attributes, &current);
        if (extra.draw != NULL) {
          extra.draw ((VieweR) viewer, extra.picture);
        }
        ResetClip ();
      } else {
        Message (MSG_ERROR, "DrawViewerProc target is not a picture");
      }
    }
  }
}

/*****************************************************************************
*
*   ViewerClickProc (viewer, pt)
*
*****************************************************************************/

static void ViewerClickProc (PaneL viewer, PoinT pt)

{
  ViewPData  extra;

  GetPanelExtra (viewer, &extra);
  if (extra.click != NULL && extra.picture != NULL) {
    extra.click ((VieweR) viewer, extra.picture, pt);
  }
}

/*****************************************************************************
*
*   ViewerDragProc (viewer, pt)
*
*****************************************************************************/

static void ViewerDragProc (PaneL viewer, PoinT pt)

{
  ViewPData  extra;

  GetPanelExtra (viewer, &extra);
  if (extra.drag != NULL && extra.picture != NULL) {
    extra.drag ((VieweR) viewer, extra.picture, pt);
  }
}

/*****************************************************************************
*
*   ViewerReleaseProc (viewer, pt)
*
*****************************************************************************/

static void ViewerReleaseProc (PaneL viewer, PoinT pt)

{
  ViewPData  extra;

  GetPanelExtra (viewer, &extra);
  if (extra.release != NULL && extra.picture != NULL) {
    extra.release ((VieweR) viewer, extra.picture, pt);
  }
}

/*****************************************************************************
*
*   ResetViewerProc (viewer)
*       Clears the panel extra data to remove references to the picture,
*       and calls the cleanup procedure to free any attached instance data
*
*****************************************************************************/

static void ResetViewerProc (PaneL viewer)

{
  ViewPData  extra;

  GetPanelExtra (viewer, &extra);
  if (extra.data != NULL && extra.cleanup != NULL) {
    extra.cleanup ((VieweR) viewer, extra.data);
  }
  extra.draw = NULL;
  extra.data = NULL;
  extra.cleanup = NULL;
  MemSet ((VoidPtr) (&extra), 0, sizeof (ViewPData));
  SetPanelExtra (viewer, &extra);
}

/*****************************************************************************
*
*   CreateViewer (prnt, width, height, vscoll, hscroll)
*       Creates a viewer panel
*
*****************************************************************************/

VieweR CreateViewer (GrouP prnt, Int2 width, Int2 height, Boolean vscroll, Boolean hscroll)

{
  ViewPData    extra;
  SltScrlProc  hscrproc;
  Int2         margin;
  PaneL        viewer;
  SltScrlProc  vscrproc;

  viewer = NULL;
  if (prnt != NULL) {
    margin = 8;
    if (vscroll) {
      vscrproc = ViewerVScrollProc;
      margin = 0;
    } else {
      vscrproc = NULL;
    }
    if (hscroll) {
      hscrproc = ViewerHScrollProc;
      margin = 0;
    } else {
      hscrproc = NULL;
    }
    viewer = AutonomousPanel (prnt, width + margin, height + margin,
                              DrawViewerProc, vscrproc, hscrproc,
                              sizeof (ViewPData), ResetViewerProc, NULL);
    if (viewer != NULL) {
      SetPanelClick (viewer, ViewerClickProc, ViewerDragProc, NULL, ViewerReleaseProc);
      MemSet ((VoidPtr) (&extra), 0, sizeof (ViewPData));
      extra.picture = NULL;
      SetPanelExtra (viewer, &extra);
    }
  }
  return (VieweR) viewer;
}

/*****************************************************************************
*
*   ResetViewer (viewer)
*       Resets a viewer panel
*
*****************************************************************************/

void ResetViewer (VieweR viewer)

{
  if (viewer != NULL) {
    Reset (viewer);
  }
}

/*****************************************************************************
*
*   DeleteViewer (viewer)
*       Removes a viewer panel
*
*****************************************************************************/

VieweR DeleteViewer (VieweR viewer)

{
  if (viewer != NULL) {
    viewer = (VieweR)Remove (viewer);
  }
  return viewer;
}

/*****************************************************************************
*
*   PadUp (val, scale)
*       Rounds a value up to be an even multiple of the scale factor
*
*****************************************************************************/

static Int4 PadUp (Int4 val, Int4 scale)

{
  Int4  mod;

  if (val >= 0) {
    mod = val % scale;
    if (mod == 0) {
      return val;
    } else {
      return val + scale - mod;
    }
  } else {
    mod = (-val) % scale;
    if (mod == 0) {
      return val;
    } else {
      return val + scale - mod;
    }
  }
}

/*****************************************************************************
*
*   PadDn (val, scale)
*       Rounds a value down to be an even multiple of the scale factor
*
*****************************************************************************/

static Int4 PadDn (Int4 val, Int4 scale)

{
  Int4  mod;

  if (val >= 0) {
    mod = val % scale;
    if (mod == 0) {
      return val;
    } else {
      return val - mod;
    }
  } else {
    mod = (-val) % scale;
    if (mod == 0) {
      return val;
    } else {
      return val - scale + mod;
    }
  }
}

/*****************************************************************************
*
*   PadBox (box, scaleX, scaleY)
*       Expands a box boundaries to all be even multiples of scale factors
*
*****************************************************************************/

static void PadBox (BoxPtr box, Int4 scaleX, Int4 scaleY)

{
  if (box != NULL && scaleX > 0 && scaleY > 0) {
    box->left = PadDn (box->left, scaleX);
    box->top = PadUp (box->top, scaleY);
    box->right = PadUp (box->right, scaleX);
    box->bottom = PadDn (box->bottom, scaleY);
  }
}

/*****************************************************************************
*
*   NormalizeBox (box)
*       Ensures that left <= right and bottom <= top in world coordinates
*
*****************************************************************************/

static void NormalizeBox (BoxPtr box)

{
  Int4  swap;

  if (box != NULL) {
    if (box->left > box->right) {
      swap = box->left;
      box->left = box->right;
      box->right = swap;
    }
    if (box->bottom > box->top) {
      swap = box->bottom;
      box->bottom = box->top;
      box->top = swap;
    }
  }
}

/*****************************************************************************
*
*   CalculateScaling (viewer, picture, pntX, pntY, align,
*                     scaleX, scaleY, scalePtr, linHgtPtr,
*                     linWidPtr, scrolXPtr, scrolYPtr)
*       Performs scaling calculations in attaching a picture to a viewer
*
*****************************************************************************/

static void CalculateScaling (VieweR viewer, SegmenT picture, Int4 pntX, Int4 pntY,
                              Int2 align, Int4 scaleX, Int4 scaleY, ScalePtr scalePtr,
                              Int4Ptr linHgtPtr, Int4Ptr linWidPtr,
                              Int2Ptr scrolXPtr, Int2Ptr scrolYPtr)

{
  Int4       delta;
  Int4       height;
  Int4       lineHeight;
  Int4       lineWidth;
  Boolean    maxXscale;
  Boolean    maxYscale;
  Int4       originalX;
  Int4       originalY;
  PicPPtr    pic;
  BoxInfo    port;
  Int2       scrollX;
  Int2       scrollY;
  RecT       view;
  Int4       width;
  BoxInfo    world;
  Int4       zoomX;
  Int4       zoomY;

  if (viewer != NULL && picture != NULL && scalePtr != NULL) {
    SelectFont (systemFont);
    pic = (PicPPtr) picture;
    if (pic->base.code == PICTURE) {

      world.left = pic->seg.box.left;
      world.top = pic->seg.box.top + 1;
      world.right = pic->seg.box.right + 1;
      world.bottom = pic->seg.box.bottom;
      NormalizeBox (&world);

      ObjectRect (viewer, &view);
      InsetRect (&view, 4, 4);

      maxXscale = (Boolean) (scaleX == 0);
      maxYscale = (Boolean) (scaleY == 0);
      if (maxXscale) {
        scaleX = (world.right - world.left + 1) / (Int4) (view.right - view.left) + 1;
      } else {
        scaleX = MAX (scaleX, 1);
      }
      if (maxYscale) {
        scaleY = (world.top - world.bottom + 1) / (Int4) (view.bottom - view.top) + 1;
      } else {
        scaleY = MAX (scaleY, 1);
      }
      scrollX = 10;
      scrollY = 10;

      zoomX = MIN (scaleX, (world.right - world.left + 1) /
                   (Int4) (view.right - view.left) + 1);
      zoomY = MIN (scaleY, (world.top - world.bottom + 1) /
                   (Int4) (view.bottom - view.top) + 1);
      lineWidth = zoomX * (Int4) scrollX;
      lineHeight = zoomY * (Int4) scrollY;

      world.left -= zoomX - (Int4) pic->seg.margin.left * zoomX;
      world.top += zoomY - (Int4) pic->seg.margin.top * zoomY;
      world.right += zoomX + (Int4) pic->seg.margin.right * zoomX;
      world.bottom -= zoomY + (Int4) pic->seg.margin.bottom * zoomY;
      PadBox (&world, lineWidth, lineHeight);

      if (maxXscale) {
        scaleX = (world.right - world.left + 1) / (Int4) (view.right - view.left) + 1;
      } else {
        scaleX = MIN (scaleX, (world.right - world.left + 1) /
                      (Int4) (view.right - view.left) + 1);
      }
      if (maxYscale) {
        scaleY = (world.top - world.bottom + 1) / (Int4) (view.bottom - view.top) + 1;
      } else {
        scaleY = MIN (scaleY, (world.top - world.bottom + 1) /
                      (Int4) (view.bottom - view.top) + 1);
      }
      if (zoomX < scaleX || zoomY < scaleY) {
        zoomX = scaleX;
        zoomY = scaleY;
        lineWidth = zoomX * (Int4) scrollX;
        lineHeight = zoomY * (Int4) scrollY;
        world.left = pic->seg.box.left - (zoomX - (Int4) pic->seg.margin.left * zoomX);
        world.top = pic->seg.box.top + 1 + (zoomY - (Int4) pic->seg.margin.top * zoomY);
        world.right = pic->seg.box.right + 1 + (zoomX + (Int4) pic->seg.margin.right * zoomX);
        world.bottom = pic->seg.box.bottom - (zoomY + (Int4) pic->seg.margin.bottom * zoomY);
        NormalizeBox (&world);
        PadBox (&world, lineWidth, lineHeight);
      }

      originalX = pntX;
      originalY = pntY;
      pntX = MAX (pntX, world.left);
      pntX = MIN (pntX, world.right);
      pntY = MAX (pntX, world.bottom);
      pntY = MIN (pntY, world.top);
      width = (Int4) (view.right - view.left) * scaleX;
      height = (Int4) (view.bottom - view.top) * scaleY;
      switch (align) {
        case MIDDLE_CENTER :
          pntX = (pntX / lineWidth) * lineWidth;
          pntY = (pntY / lineHeight) * lineHeight;
          port.left = pntX - (width / 2);
          port.top = pntY + (height / 2);
          port.right = pntX + (width / 2);
          port.bottom = pntY - (height / 2);
          break;
        case UPPER_LEFT :
          pntX = PadDn (pntX, lineWidth);
          pntY = PadUp (pntY, lineHeight);
          port.left = pntX;
          port.top = pntY;
          port.right = pntX + width;
          port.bottom = pntY - height;
          break;
        case LOWER_LEFT :
          pntX = PadDn (pntX, lineWidth);
          pntY = PadDn (pntY, lineHeight);
          port.left = pntX;
          port.bottom = pntY;
          port.right = pntX + width;
          port.top = pntY + height;
          break;
        case UPPER_RIGHT :
          pntX = PadUp (pntX, lineWidth);
          pntY = PadUp (pntY, lineHeight);
          port.right = pntX;
          port.top = pntY;
          port.left = pntX - width;
          port.bottom = pntY - height;
          break;
        case LOWER_RIGHT :
          pntX = PadUp (pntX, lineWidth);
          pntY = PadDn (pntY, lineHeight);
          port.right = pntX;
          port.bottom = pntY;
          port.left = pntX - width;
          port.top = pntY + height;
          break;
        case UPPER_CENTER :
          pntX = (pntX / lineWidth) * lineWidth;
          pntY = PadUp (pntY, lineHeight);
          port.left = pntX - (width / 2);
          port.top = pntY;
          port.right = pntX + (width / 2);
          port.bottom = pntY - height;
          break;
        case LOWER_CENTER :
          pntX = (pntX / lineWidth) * lineWidth;
          pntY = PadDn (pntY, lineHeight);
          port.left = pntX - (width / 2);
          port.bottom = pntY;
          port.right = pntX + (width / 2);
          port.top = pntY + height;
          break;
        case MIDDLE_LEFT :
          pntX = PadDn (pntX, lineWidth);
          pntY = (pntY / lineHeight) * lineHeight;
          port.left = pntX;
          port.top = pntY + (height / 2);
          port.right = pntX + width;
          port.bottom = pntY - (height / 2);
          break;
        case MIDDLE_RIGHT :
          pntX = PadUp (pntX, lineWidth);
          pntY = (pntY / lineHeight) * lineHeight;
          port.right = pntX;
          port.top = pntY + (height / 2);
          port.left = pntX - width;
          port.bottom = pntY - (height / 2);
          break;
        default :
          Message (MSG_ERROR, "AttachPicture align parameter is invalid");
          break;
      }
      NormalizeBox (&port);

      if (port.right > world.right) {
        delta = port.right - world.right;
        port.left -= delta;
        port.right -= delta;
      }
      if (port.left < world.left) {
        delta = world.left - port.left;
        port.left += delta;
        port.right += delta;
      }
      if (port.bottom < world.bottom) {
        delta = world.bottom - port.bottom;
        port.top += delta;
        port.bottom += delta;
      }
      if (port.top > world.top) {
        delta = port.top - world.top;
        port.top -= delta;
        port.bottom -= delta;
      }

      PadBox (&port, lineWidth, lineHeight);

      port.left = MAX (port.left, world.left);
      port.top = MIN (port.top, world.top);
      port.right = MIN (port.right, world.right);
      port.bottom = MAX (port.bottom, world.bottom);

      if (port.left == world.left && port.right == world.right) {
        switch (align) {
          case UPPER_LEFT :
          case MIDDLE_LEFT :
          case LOWER_LEFT :
            break;
          case UPPER_CENTER :
          case MIDDLE_CENTER :
          case LOWER_CENTER :
            delta = width - (port.right - port.left);
            port.left -= delta / 2;
            break;
          case UPPER_RIGHT :
          case MIDDLE_RIGHT :
          case LOWER_RIGHT :
            delta = width - (port.right - port.left);
            port.left -= delta;
            break;
          default :
            break;
        }
      } else if (originalX == INT4_MIN || originalX == INT4_MAX) {
        switch (align) {
          case UPPER_LEFT :
          case MIDDLE_LEFT :
          case LOWER_LEFT :
            if (port.left > world.left) {
              delta = port.left - world.left;
              port.left -= delta;
              port.right -= delta;
            }
            break;
          case UPPER_CENTER :
          case MIDDLE_CENTER :
          case LOWER_CENTER :
            break;
          case UPPER_RIGHT :
          case MIDDLE_RIGHT :
          case LOWER_RIGHT :
            if (port.right < world.right) {
              delta = world.right - port.right;
              port.left += delta;
              port.right += delta;
            }
            break;
          default :
            break;
        }
      }

      if (port.top == world.top && port.bottom == world.bottom) {
        switch (align) {
          case UPPER_LEFT :
          case UPPER_CENTER :
          case UPPER_RIGHT :
            delta = height - (port.top - port.bottom);
            port.bottom -= delta;
            break;
          case MIDDLE_LEFT :
          case MIDDLE_CENTER :
          case MIDDLE_RIGHT :
            delta = height - (port.top - port.bottom);
            port.bottom -= delta / 2;
            break;
          case LOWER_LEFT :
          case LOWER_CENTER :
          case LOWER_RIGHT :
            break;
          default :
            break;
        }
      } else if (originalY == INT4_MIN || originalY == INT4_MAX) {
        switch (align) {
          case UPPER_LEFT :
          case UPPER_CENTER :
          case UPPER_RIGHT :
            if (port.top < world.top) {
              delta = world.top - port.top;
              port.top += delta;
              port.bottom += delta;
            }
            break;
          case MIDDLE_LEFT :
          case MIDDLE_CENTER :
          case MIDDLE_RIGHT :
            break;
          case LOWER_LEFT :
          case LOWER_CENTER :
          case LOWER_RIGHT :
            if (port.bottom > world.bottom) {
              delta = port.bottom - world.bottom;
              port.top -= delta;
              port.bottom -= delta;
            }
            break;
          default :
            break;
        }
      }

      scalePtr->world = world;
      scalePtr->view = view;
      scalePtr->port = port;
      scalePtr->scaleX = scaleX;
      scalePtr->scaleY = scaleY;
      scalePtr->scrollX = scrollX;
      scalePtr->scrollY = scrollY;
      scalePtr->force = FALSE;

      if (linHgtPtr != NULL) {
        *linHgtPtr = lineHeight;
      }
      if (linWidPtr != NULL) {
        *linWidPtr = lineWidth;
      }
      if (scrolXPtr != NULL) {
        *scrolXPtr = scrollX;
      }
      if (scrolYPtr != NULL) {
        *scrolYPtr = scrollY;
      }

    }
  }
}

/*****************************************************************************
*
*   EstimateScaling (viewer, picture, pntX, pntY, align, scaleX, scaleY)
*       Calculates maximum scaling factors in attaching a picture to a viewer
*
*****************************************************************************/

void EstimateScaling (VieweR viewer, SegmenT picture, Int4 pntX, Int4 pntY,
                      Int2 align, Int4Ptr scaleX, Int4Ptr scaleY)

{
  ScaleInfo  scale;
  PicPPtr    pic;

  if (viewer != NULL && picture != NULL) {
    SelectFont (systemFont);
    pic = (PicPPtr) picture;
    if (pic->base.code == PICTURE) {
      CalculateScaling (viewer, picture, pntX, pntY, align,
                        0, 0, &scale, NULL, NULL, NULL, NULL);
      if (scaleX != NULL) {
        *scaleX = scale.scaleX;
      }
      if (scaleY != NULL) {
        *scaleY = scale.scaleY;
      }
    }
  }
}

/*****************************************************************************
*
*   AttachCommon (viewer, picture, pntX, pntY, align,
*                 scaleX, scaleY, draw, data, cleanup)
*       Connects a picture to a viewer, mapping world coordinate boundaries
*       onto viewer panel, calculating scaling and offset factors.  If the
*       scaleX or scaleY parameters are 0, the minimum scale for which no
*       scrolling is necessary is used.  Extra instance data can be attached
*
*****************************************************************************/

static void AttachCommon (VieweR viewer, SegmenT picture, Int4 pntX, Int4 pntY,
                          Int2 align, Int4 scaleX, Int4 scaleY, VwrDrawProc draw,
                          VoidPtr data, VwrFreeProc cleanup)

{
  ViewPData  extra;
  Int4       lineHeight;
  Int4       lineWidth;
  Int4       numLines;
  PicPPtr    pic;
  BaR        sb;
  Int2       scrollX;
  Int2       scrollY;
  WindoW     tempPort;
  Int4       visLines;

  if (viewer != NULL && picture != NULL) {
    SelectFont (systemFont);
    pic = (PicPPtr) picture;
    if (pic->base.code == PICTURE) {

      extra.viewer = viewer;
      extra.picture = picture;

      CalculateScaling (viewer, picture, pntX, pntY, align, scaleX, scaleY,
                        &(extra.scale), &lineHeight, &lineWidth, &scrollX, &scrollY);

      extra.click = NULL;
      extra.drag = NULL;
      extra.release = NULL;
      extra.pan = NULL;
      extra.draw = draw;

      extra.data = data;
      extra.cleanup = cleanup;
      SetPanelExtra ((PaneL) viewer, &extra);

      sb = GetSlateHScrollBar ((SlatE) viewer);
      if (sb != NULL) {
        Reset (sb);
        numLines = (extra.scale.world.right - extra.scale.world.left) / lineWidth;
        visLines = (extra.scale.view.right - extra.scale.view.left) / (Int4) scrollX;
        if (visLines > 0 && numLines >= visLines) {
          SetRange (sb, (Int2) (visLines - 1), (Int2) (visLines - 1),
                    (Int2) (numLines - visLines));
          CorrectBarValue (sb, (Int2) ((extra.scale.port.left -
                           extra.scale.world.left) / lineWidth));
        }
      }

      sb = GetSlateVScrollBar ((SlatE) viewer);
      if (sb != NULL) {
        Reset (sb);
        numLines = (extra.scale.world.top - extra.scale.world.bottom) / lineHeight;
        visLines = (extra.scale.view.bottom - extra.scale.view.top) / (Int4) scrollY;
        if (visLines > 0 && numLines >= visLines) {
          SetRange (sb, (Int2) (visLines - 1), (Int2) (visLines - 1),
                    (Int2) (numLines - visLines));
          CorrectBarValue (sb, (Int2) ((extra.scale.world.top -
                           extra.scale.port.top) / lineHeight));
        }
      }

      if (Visible (viewer) && AllParentsVisible (viewer)) {
        tempPort = SavePort (viewer);
        Select (viewer);
        InvalRect (&(extra.scale.view));
        RestorePort (tempPort);
      }
    } else if (viewer != NULL) {
      Reset (viewer);
    } else {
      Message (MSG_ERROR, "AttachPicture argument not a picture");
    }
  }
}

/*****************************************************************************
*
*   AttachPicture (viewer, picture, pntX, pntY, align, scaleX, scaleY, draw)
*       Connects a picture to a viewer
*
*****************************************************************************/

void AttachPicture (VieweR viewer, SegmenT picture, Int4 pntX, Int4 pntY,
                    Int2 align, Int4 scaleX, Int4 scaleY, VwrDrawProc draw)

{
  AttachCommon (viewer, picture, pntX, pntY, align,
                scaleX, scaleY, draw, NULL, NULL);
}

/*****************************************************************************
*
*   AttachInstance (viewer, picture, pntX, pntY, align,
*                   scaleX, scaleY, draw, data, cleanup)
*       Connects a picture to a viewer, with extra instance data
*
*****************************************************************************/

void AttachInstance (VieweR viewer, SegmenT picture, Int4 pntX, Int4 pntY,
                     Int2 align, Int4 scaleX, Int4 scaleY, VwrDrawProc draw,
                     VoidPtr data, VwrFreeProc cleanup)

{
  AttachCommon (viewer, picture, pntX, pntY, align,
                scaleX, scaleY, draw, data, cleanup);
}

/*****************************************************************************
*
*   SetViewerProcs (viewer, click, drag, release, pan)
*       Attaches program responsiveness callbacks to a viewer
*
*****************************************************************************/

void SetViewerProcs (VieweR viewer, VwrClckProc click, VwrClckProc drag,
                     VwrClckProc release, VwrPanProc pan)

{
  ViewPData  extra;

  if (viewer != NULL) {
    GetPanelExtra ((PaneL) viewer, &extra);
    extra.click = click;
    extra.drag = drag;
    extra.release = release;
    extra.pan = pan;
    SetPanelExtra ((PaneL) viewer, &extra);
  }
}

/*****************************************************************************
*
*   SetViewerData (viewer, data, cleanup)
*       Attaches instance data to a viewer
*
*****************************************************************************/

void SetViewerData (VieweR viewer, VoidPtr data, VwrFreeProc cleanup)

{
  ViewPData  extra;

  if (viewer != NULL) {
    GetPanelExtra ((PaneL) viewer, &extra);
    extra.data = data;
    extra.cleanup = cleanup;
    SetPanelExtra ((PaneL) viewer, &extra);
  }
}

/*****************************************************************************
*
*   GetViewerData (viewer)
*       Returns the instance data attached to a viewer
*
*****************************************************************************/

VoidPtr GetViewerData (VieweR viewer)

{
  ViewPData  extra;

  if (viewer != NULL) {
    GetPanelExtra ((PaneL) viewer, &extra);
    return (extra.data);
  } else {
    return NULL;
  }
}

/*****************************************************************************
*
*   ViewerBox (viewer, world, port, view, scaleX, scaleY)
*       Returns the world and port boxes, view rect, and scales of a viewer
*
*****************************************************************************/

void ViewerBox (VieweR viewer, BoxPtr world, BoxPtr port,
                RectPtr view, Int4Ptr scaleX, Int4Ptr scaleY)

{
  ViewPData  extra;

  if (viewer != NULL) {
    GetPanelExtra ((PaneL) viewer, &extra);
    if (world != NULL) {
      *world = extra.scale.world;
    }
    if (port != NULL) {
      *port = extra.scale.port;
    }
    if (view != NULL) {
      *view = extra.scale.view;
    }
    if (scaleX != NULL) {
      *scaleX = extra.scale.scaleX;
    }
    if (scaleY != NULL) {
      *scaleY = extra.scale.scaleY;
    }
  }
}

/*****************************************************************************
*
*   SearchSegment (segment, pt, pnt, scale, prID, prCt)
*       Returns the deepest segment that contains the pnt in world coordinates
*
*****************************************************************************/

static SegmenT SearchSegment (SegmenT segment, PoinT pt, PntPtr pnt,
                              ScaleInfo *scale, Uint2Ptr prID, Uint2Ptr prCt)

{
  double   aspect;
  BtmPPtr  btm;
  double   cosTheta;
  Uint2    count;
  CstPPtr  cst;
  Int2     dist;
  SegmenT  foundPrim;
  SegmenT  foundSeg;
  double   head;
  BasePPtr item;
  LblPPtr  lbl;
  LinPPtr  lin;
  MrkPPtr  mrk;
  PoinT    pt1;
  PoinT    pt2;
  PoinT    pts [4];
  RecT     r;
  RecPPtr  rec;
  SegPPtr  seg;
  double   sinTheta;
  SymPPtr  sym;
  PoinT    vpt;
  double   theta;
  double   x0, x1, x2;
  double   y0, y1, y2;

  foundSeg = NULL;
  foundPrim = NULL;
  if (segment != NULL && pnt != NULL && scale != NULL && prID != NULL && prCt != NULL) {
    seg = (SegPPtr) segment;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      if (PntInBox (&(seg->seg.box), pnt, &(seg->seg.margin), scale)) {
        item = seg->seg.head;
        count = 1;
        while (item != NULL && foundSeg == NULL && foundPrim == NULL) {
          switch (item->code) {
            case PICTURE :
              Message (MSG_ERROR, "FindSegment child is a picture");
              break;
            case SEGMENT :
              foundSeg = SearchSegment ((SegmenT) item, pt, pnt, scale, prID, prCt);
              break;
            case ATTRIBUTE :
              break;
            case RECTANGLE :
              rec = (RecPPtr) item;
              if (BoxInViewport (&r, &(rec->box), &(rec->rct), scale)) {
                InsetRect (&r, -RECT_PROXIMITY, -RECT_PROXIMITY);
                if (PtInRect (pt, &r)) {
                  foundPrim = (SegmenT) seg;
                  *prID = rec->primID;
                  *prCt = count;
                }
              }
              break;
            case LINE :
              lin = (LinPPtr) item;
              if (LineInViewport (&pt1, &pt2, &(lin->pnt1), &(lin->pnt2), &(lin->rct), scale)) {
                if (pt1.x != pt2.x || pt1.y != pt2.y) {
                  if (pt.x + ARROW_LENGTH > MIN (pt1.x, pt2.x) - LINE_PROXIMITY &&
                      pt.x - ARROW_LENGTH < MAX (pt1.x, pt2.x) + LINE_PROXIMITY &&
                      pt.y + ARROW_LENGTH > MIN (pt1.y, pt2.y) - LINE_PROXIMITY &&
                      pt.y - ARROW_LENGTH < MAX (pt1.y, pt2.y) + LINE_PROXIMITY) {
                    x0 = (double) pt.x;
                    y0 = (double) pt.y;
                    x1 = (double) pt1.x;
                    y1 = (double) pt1.y;
                    x2 = (double) pt2.x;
                    y2 = (double) pt2.y;
                    dist = (Int2) (ABS (x0 * (y1 - y2) + y0 * (x2 - x1) + x1 * y2 - x2 * y1) /
                                   sqrt ((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2)));
                    if (dist < LINE_PROXIMITY) {
                      foundPrim = (SegmenT) seg;
                      *prID = lin->primID;
                    } else if (lin->arrow) {
                      head = (double) ARROW_LENGTH;
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
                      pts [0].x = (pts [1].x + pts [2].x) / 2;
                      pts [0].y = (pts [1].y + pts [2].y) / 2;
                      x0 = (double) pt.x;
                      y0 = (double) pt.y;
                      x2 = (double) ((pt2.x + (pts [1].x + pts [2].x) / 2) / 2);
                      y2 = (double) ((pt2.y + (pts [1].y + pts [2].y) / 2) / 2);
                      dist = (Int2) (sqrt ((x2 - x0) * (x2 - x0) + (y2 - y0) * (y2 - y0)));
                      if (dist < ARROW_LENGTH) {
                        foundPrim = (SegmenT) seg;
                        *prID = lin->primID;
                        *prCt = count;
                      }
                    }
                  }
                }
              }
              break;
            case SYMBOL :
              sym = (SymPPtr) item;
              if (PntInViewport (&vpt, &(sym->pnt), &(sym->rct), scale)) {
                r = sym->rct;
                InsetRect (&r, -GENERAL_PROXIMITY, -GENERAL_PROXIMITY);
                OffsetRect (&r, vpt.x, vpt.y);
                if (PtInRect (pt, &r)) {
                  foundPrim = (SegmenT) seg;
                  *prID = sym->primID;
                  *prCt = count;
                }
              }
              break;
            case BITMAP :
              btm = (BtmPPtr) item;
              if (PntInViewport (&vpt, &(btm->pnt), &(btm->rct), scale)) {
                r = btm->rct;
                InsetRect (&r, -GENERAL_PROXIMITY, -GENERAL_PROXIMITY);
                OffsetRect (&r, vpt.x, vpt.y);
                if (PtInRect (pt, &r)) {
                  foundPrim = (SegmenT) seg;
                  *prID = btm->primID;
                  *prCt = count;
                }
              }
              break;
            case CUSTOM :
              cst = (CstPPtr) item;
              if (PntInViewport (&vpt, &(cst->pnt), &(cst->rct), scale)) {
                r = cst->rct;
                InsetRect (&r, -GENERAL_PROXIMITY, -GENERAL_PROXIMITY);
                OffsetRect (&r, vpt.x, vpt.y);
                if (PtInRect (pt, &r)) {
                  foundPrim = (SegmenT) seg;
                  *prID = cst->primID;
                  *prCt = count;
                }
              }
              break;
            case MARKER :
              mrk = (MrkPPtr) item;
              if (PntInViewport (&vpt, &(mrk->pnt), &(mrk->rct), scale)) {
                r = mrk->rct;
                InsetRect (&r, -GENERAL_PROXIMITY, -GENERAL_PROXIMITY);
                OffsetRect (&r, vpt.x, vpt.y);
                if (PtInRect (pt, &r)) {
                  foundPrim = (SegmenT) seg;
                  *prID = mrk->primID;
                  *prCt = count;
                }
              }
              break;
            case LABEL :
              lbl = (LblPPtr) item;
              if (PntInViewport (&vpt, &(lbl->pnt), &(lbl->rct), scale)) {
                r = lbl->rct;
                InsetRect (&r, -GENERAL_PROXIMITY, -GENERAL_PROXIMITY);
                OffsetRect (&r, vpt.x, vpt.y);
                if (PtInRect (pt, &r)) {
                  foundPrim = (SegmenT) seg;
                  *prID = lbl->primID;
                  *prCt = count;
                }
              }
              break;
            default :
              break;
          }
          item = item->next;
          count++;
        }
        while (item != NULL && foundSeg == NULL) {
          switch (item->code) {
            case PICTURE :
              Message (MSG_ERROR, "FindSegment child is a picture");
              break;
            case SEGMENT :
              foundSeg = SearchSegment ((SegmenT) item, pt, pnt, scale, prID, prCt);
              break;
            default :
              break;
          }
          item = item->next;
        }
        if (foundSeg == NULL) {
          foundSeg = foundPrim;
        }
      }
    } else {
      Message (MSG_ERROR, "SearchSegment argument not a segment or picture");
    }
  }
  return foundSeg;
}

/*****************************************************************************
*
*   FindSegment (viewer, pt, segID, primID, primCt)
*       Returns the deepest segment that contains the pt
*
*****************************************************************************/

SegmenT FindSegment (VieweR viewer, PoinT pt, Uint2Ptr segID, Uint2Ptr primID, Uint2Ptr primCt)

{
  ViewPData  extra;
  SegmenT    found;
  PicPPtr    pic;
  PntInfo    pnt;
  Uint2      prCt;
  Uint2      prID;
  Uint2      sgID;

  found = NULL;
  prCt = 0;
  prID = 0;
  sgID = 0;
  if (viewer != NULL) {
    GetPanelExtra ((PaneL) viewer, &extra);
    pic = (PicPPtr) extra.picture;
    if (pic->base.code == PICTURE) {
      MapViewerToWorld (viewer, pt, &pnt);
      found = SearchSegment ((SegmenT) pic, pt, &pnt, &(extra.scale), &prID, &prCt);
    } else {
      Message (MSG_ERROR, "FindSegment argument not a picture");
    }
  }
  if (found != NULL) {
    sgID = SegmentID (found);
  }
  if (segID != NULL) {
    *segID = sgID;
  }
  if (primID != NULL) {
    *primID = prID;
  }
  if (primCt != NULL) {
    *primCt = prCt;
  }
  return found;
}

/*****************************************************************************
*
*   ShowSegment (viewer, segment)
*       Makes a segment visible
*
*****************************************************************************/

void ShowSegment (VieweR viewer, SegmenT segment)

{
  RecT       d;
  ViewPData  extra;
  RecT       r;
  RecT       s;
  SegPPtr    seg;
  WindoW     tempPort;

  if (viewer != NULL && segment != NULL) {
    seg = (SegPPtr) segment;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      if (! seg->seg.visible) {
        seg->seg.visible = TRUE;
        GetPanelExtra ((PaneL) viewer, &extra);
        if (BoxInViewport (&r, &(seg->seg.box), &(seg->seg.margin), &(extra.scale))) {
          r.left -= 1 - seg->seg.margin.left;
          r.top -= 1 - seg->seg.margin.top;
          r.right += 1 + seg->seg.margin.right;
          r.bottom += 1 + seg->seg.margin.bottom;
          if (Visible (viewer) && AllParentsVisible (viewer)) {
            ObjectRect (viewer, &s);
            InsetRect (&s, 2, 2);
            SectRect (&r, &s, &d);
            tempPort = SavePort (viewer);
            Select (viewer);
            InvalRect (&d);
            RestorePort (tempPort);
          }
        }
      }
    } else {
      Message (MSG_ERROR, "ShowSegment argument not a segment or picture");
    }
  }
}

/*****************************************************************************
*
*   HideSegment (viewer, segment)
*       Maps a segment invisible
*
*****************************************************************************/

void HideSegment (VieweR viewer, SegmenT segment)

{
  RecT       d;
  ViewPData  extra;
  RecT       r;
  RecT       s;
  SegPPtr    seg;
  WindoW     tempPort;

  if (viewer != NULL && segment != NULL) {
    seg = (SegPPtr) segment;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      if (seg->seg.visible) {
        seg->seg.visible = FALSE;
        GetPanelExtra ((PaneL) viewer, &extra);
        if (BoxInViewport (&r, &(seg->seg.box), &(seg->seg.margin), &(extra.scale))) {
          r.left -= 1 - seg->seg.margin.left;
          r.top -= 1 - seg->seg.margin.top;
          r.right += 1 + seg->seg.margin.right;
          r.bottom += 1 + seg->seg.margin.bottom;
          if (Visible (viewer) && AllParentsVisible (viewer)) {
            ObjectRect (viewer, &s);
            InsetRect (&s, 2, 2);
            SectRect (&r, &s, &d);
            tempPort = SavePort (viewer);
            Select (viewer);
            InvalRect (&d);
            RestorePort (tempPort);
          }
        }
      }
    } else {
      Message (MSG_ERROR, "ShowSegment argument not a segment or picture");
    }
  }
}

/*****************************************************************************
*
*   HighlightSegment (viewer, segment, highlight)
*       Highlights a segment
*
*****************************************************************************/

void HighlightSegment (VieweR viewer, SegmenT segment, Int1 highlight)

{
  RecT       d;
  ViewPData  extra;
  RecT       r;
  RecT       s;
  SegPPtr    seg;
  WindoW     tempPort;

  if (viewer != NULL && segment != NULL) {
    seg = (SegPPtr) segment;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      if (seg->seg.highlight != highlight) {
        seg->seg.highlight = highlight;
        if (seg->seg.visible) {
          if (Visible (viewer) && AllParentsVisible (viewer)) {
            GetPanelExtra ((PaneL) viewer, &extra);
            if (BoxInViewport (&r, &(seg->seg.box), &(seg->seg.margin), &(extra.scale))) {
              r.left -= 1 - seg->seg.margin.left;
              r.top -= 1 - seg->seg.margin.top;
              r.right += 1 + seg->seg.margin.right;
              r.bottom += 1 + seg->seg.margin.bottom;
              InsetRect (&r, -1, -1);
              ObjectRect (viewer, &s);
              InsetRect (&s, 2, 2);
              SectRect (&r, &s, &d);
              tempPort = SavePort (viewer);
              Select (viewer);
              InvalRect (&d);
              RestorePort (tempPort);
            }
          }
        }
      }
    } else {
      Message (MSG_ERROR, "HighlightSegment argument not a segment or picture");
    }
  }
}

/*****************************************************************************
*
*   HighlightPrimitive (viewer, segment, primitive, highlight)
*       Highlights a primitive
*
*****************************************************************************/

void HighlightPrimitive (VieweR viewer, SegmenT segment, PrimitivE primitive, Int1 highlight)

{
  RecT       d;
  ViewPData  extra;
  BasePPtr   item;
  LinPPtr    lin;
  RecT       r;
  RecPPtr    rec;
  RecT       s;
  SegPPtr    seg;
  WindoW     tempPort;

  if (viewer != NULL && segment != NULL) {
    seg = (SegPPtr) segment;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      item = (BasePPtr) primitive;
      if (item != NULL && (item->code == RECTANGLE || item->code == LINE)) {
        if (item->code == RECTANGLE) {
          rec = (RecPPtr) item;
          rec->highlight = highlight;
        } else if (item->code == LINE) {
          lin = (LinPPtr) item;
          lin->highlight = highlight;
        }
        if (seg->seg.visible) {
          if (Visible (viewer) && AllParentsVisible (viewer)) {
            GetPanelExtra ((PaneL) viewer, &extra);
            if (BoxInViewport (&r, &(seg->seg.box), &(seg->seg.margin), &(extra.scale))) {
              r.left -= 1 - seg->seg.margin.left;
              r.top -= 1 - seg->seg.margin.top;
              r.right += 1 + seg->seg.margin.right;
              r.bottom += 1 + seg->seg.margin.bottom;
              InsetRect (&r, -1, -1);
              ObjectRect (viewer, &s);
              InsetRect (&s, 2, 2);
              SectRect (&r, &s, &d);
              tempPort = SavePort (viewer);
              Select (viewer);
              InvalRect (&d);
              RestorePort (tempPort);
            }
          }
        }
      }
    } else {
      Message (MSG_ERROR, "HighlightPrimitive argument not a segment or picture");
    }
  }
}

/*****************************************************************************
*
*   PrintViewer (viewer)
*       Prints the picture.
*
*****************************************************************************/

void PrintViewer (VieweR viewer)

{
  AttPData   attributes;
  AttPData   current;
  ViewPData  extra;
  Boolean    goOn;
  PicPPtr    pic;
  WindoW     w;

  if (viewer != NULL) {
    GetPanelExtra ((PaneL) viewer, &extra);
    pic = (PicPPtr) extra.picture;
    if (pic != NULL) {
      if (pic->base.code == PICTURE) {
        w = StartPrinting ();
        if (w != NULL) {
          goOn = StartPage ();
          if (goOn) {
            PrintingRect (&(extra.scale.view));
            attributes.flags = NO_ATTS;
            attributes.color [0] = BLACK_COLOR [0];
            attributes.color [1] = BLACK_COLOR [1];
            attributes.color [2] = BLACK_COLOR [2];
            attributes.linestyle = SOLID_LINE;
            attributes.shading = SOLID_SHADING;
            attributes.penwidth = STD_PEN_WIDTH;
            attributes.mode = COPY_MODE;
            attributes.highlight = PLAIN_SEGMENT;
            current = *(&attributes);
            extra.scale.force = TRUE;
            DrawSegment ((SegPPtr) pic, &attributes, &current, &(extra.scale));
            SetAttributes (&attributes, &current);
            goOn = EndPage ();
          }
          EndPrinting (w);
        }
      } else {
        Message (MSG_ERROR, "PrintViewer target is not a picture");
      }
    }
  }
}

/*****************************************************************************
*
*   MapWorldToViewer (viewer, pnt, pt)
*       Maps a pnt in world coordinates to a point in viewer coordinates
*
*****************************************************************************/

void MapWorldToViewer (VieweR viewer, PntInfo pnt, PointPtr pt)

{
  ViewPData  extra;

  if (viewer != NULL && pt != NULL) {
    GetPanelExtra ((PaneL) viewer, &extra);
    MapWorldPointToPixel (pt, &pnt, &(extra.scale));
  }
}

/*****************************************************************************
*
*   MapViewerToWorld (viewer, pt, pnt)
*       Maps a point in viewer coordinates to a pnt in world coordinates
*
*****************************************************************************/

void MapViewerToWorld (VieweR viewer, PoinT pt, PntPtr pnt)

{
  ViewPData  extra;

  if (viewer != NULL && pnt != NULL) {
    GetPanelExtra ((PaneL) viewer, &extra);
    MapPixelPointToWorld (pnt, &pt, &(extra.scale));
  }
}
