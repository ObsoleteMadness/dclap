/*   picture.c
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
* File Name:  picture.c
*
* Author:  Jonathan Kans, Jill Shermer
*
* Version Creation Date:   10/23/92
*
* $Revision: 1.15 $
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

/*****************************************************************************
*
*   EXTERNAL VARIABLES
*
*****************************************************************************/

Uint1 BLACK_COLOR [3] = {0, 0, 0};
Uint1 WHITE_COLOR [3] = {255, 255, 255};
Uint1 RED_COLOR  [3] = {255, 0, 0};
Uint1 GREEN_COLOR [3] = {0, 255, 0};
Uint1 BLUE_COLOR [3] = {0, 0, 255};
Uint1 YELLOW_COLOR [3] = {255, 255, 0};
Uint1 CYAN_COLOR [3] = {0, 255, 255};
Uint1 MAGENTA_COLOR [3] = {255, 0, 255};

/*****************************************************************************
*
*   STATIC VARIABLES
*
*****************************************************************************/

static PoinT  emptyPt = {0, 0};

/*****************************************************************************
*
*   CreatePicture (void)
*       Creates a Picture Record (same as Segment Record except for base.code)
*
*****************************************************************************/

SegmenT CreatePicture (void)

{
  PicPPtr  pic;

  pic = (PicPPtr) MemNew (sizeof (PicPRec));
  if (pic != NULL) {
    pic->base.next = NULL;
    pic->base.code = PICTURE;
    pic->seg.box.left = INT4_MAX;
    pic->seg.box.top = INT4_MIN;
    pic->seg.box.right = INT4_MIN;
    pic->seg.box.bottom = INT4_MAX;
    pic->seg.head = NULL;
    pic->seg.tail = NULL;
    pic->seg.parent = NULL;
    pic->seg.visible = TRUE;
    pic->seg.margin.left = 0;
    pic->seg.margin.top = 0;
    pic->seg.margin.right = 0;
    pic->seg.margin.bottom = 0;
    pic->seg.maxscale = 0;
    pic->seg.penwidth = STD_PEN_WIDTH;
    pic->seg.highlight = PLAIN_SEGMENT;
    pic->seg.segID = 0;
  }
  return (SegmenT) pic;
}

/*****************************************************************************
*
*   DeletePicture (picture)
*       Removes any previously existing segments, then frees picture node
*
*****************************************************************************/

SegmenT DeletePicture (SegmenT picture)

{
  PicPPtr  pic;

  if (picture != NULL) {
    pic = (PicPPtr) picture;
    if (pic->base.code == PICTURE) {
      ResetSegment ((SegmenT) pic);
      pic = MemFree (pic);
    } else {
      Message (MSG_ERROR, "DeletePicture argument not a picture");
    }
  }
  return (SegmenT) pic;
}

/*****************************************************************************
*
*   CreatePrimitive (parent, code)
*       Common routine creates any primitive type, then adds it to the end
*       of the parent segment's child list, updating segment pointers
*
*****************************************************************************/

static BasePPtr CreatePrimitive (SegmenT parent, Int1 code, size_t size)

{
  BasePPtr  prev;
  BasePPtr  prim;
  SegPPtr   prnt;

  prim = NULL;
  if (parent != NULL) {
    prnt = (SegPPtr) parent;
    if (prnt->base.code == SEGMENT || prnt->base.code == PICTURE) {
      prim = (BasePPtr) MemNew (size);
      if (prim != NULL) {
        prim->next = NULL;
        prim->code = code;
        if (prnt->seg.head != NULL && prnt->seg.tail != NULL) {
          prev = prnt->seg.tail;
          prev->next = prim;
          prnt->seg.tail = prim;
        } else if (prnt->seg.head == NULL && prnt->seg.tail == NULL) {
          prnt->seg.head = prim;
          prnt->seg.tail = prim;
        } else {
          Message (MSG_ERROR, "CreatePrimitive list integrity problem");
        }
      }
    } else {
      Message (MSG_ERROR, "CreatePrimitive parent not a segment or picture");
    }
  }
  return prim;
}

/*****************************************************************************
*
*   CreateSegment (parent, segID, maxScale)
*       Creates a segment primitive that can have its own list of children
*
*****************************************************************************/

SegmenT CreateSegment (SegmenT parent, Uint2 segID, Int4 maxScale)

{
  SegPPtr  prnt;
  SegPPtr  seg;

  seg = (SegPPtr) CreatePrimitive (parent, SEGMENT, sizeof (SegPRec));
  if (seg != NULL) {
    seg->seg.box.left = INT4_MAX;
    seg->seg.box.top = INT4_MIN;
    seg->seg.box.right = INT4_MIN;
    seg->seg.box.bottom = INT4_MAX;
    seg->seg.head = NULL;
    seg->seg.tail = NULL;
    seg->seg.parent = (BasePPtr) parent;
    seg->seg.visible = TRUE;
    seg->seg.margin.left = 0;
    seg->seg.margin.top = 0;
    seg->seg.margin.right = 0;
    seg->seg.margin.bottom = 0;
    seg->seg.maxscale = maxScale;
    prnt = (SegPPtr) parent;
    seg->seg.penwidth = STD_PEN_WIDTH;
    if (prnt != NULL) {
      if (prnt->base.code == SEGMENT || prnt->base.code == PICTURE) {
        seg->seg.penwidth = prnt->seg.penwidth;
      }
    }
    seg->seg.highlight = PLAIN_SEGMENT;
    seg->seg.segID = segID;
  }
  return (SegmenT) seg;
}

/*****************************************************************************
*
*   ResetSegment (segment)
*       Cleaves and deletes the segment's child list (recursively),
*       freeing item specific data, and leaving the segment itself
*       intact for repopulation
*
*****************************************************************************/

SegmenT ResetSegment (SegmenT segment)

{
  BasePPtr  item;
  LblPPtr   lbl;
  BasePPtr  next;
  SegPPtr   seg;

  if (segment != NULL) {
    seg = (SegPPtr) segment;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      item = seg->seg.head;
      while (item != NULL) {
        next = item->next;
        switch (item->code) {
          case PICTURE :
            ResetSegment ((SegmenT) item);
            break;
          case SEGMENT :
            ResetSegment ((SegmenT) item);
            break;
          case LABEL :
            lbl = (LblPPtr) item;
            MemFree (lbl->str);
            break;
          default :
            break;
        }
        MemFree (item);
        item = next;
      }
      seg->seg.box.left = INT4_MAX;
      seg->seg.box.top = INT4_MIN;
      seg->seg.box.right = INT4_MIN;
      seg->seg.box.bottom = INT4_MAX;
      seg->seg.head = NULL;
      seg->seg.tail = NULL;
      seg->seg.margin.left = 0;
      seg->seg.margin.top = 0;
      seg->seg.margin.right = 0;
      seg->seg.margin.bottom = 0;
      seg->seg.penwidth = STD_PEN_WIDTH;
      seg->seg.highlight = PLAIN_SEGMENT;
    } else {
      Message (MSG_ERROR, "ResetSegment argument not a segment or picture");
    }
  }
  return (SegmenT) seg;
}

/*****************************************************************************
*
*   ParentSegment (segment)
*       Returns the parent of a segment
*
*****************************************************************************/

SegmenT ParentSegment (SegmenT segment)

{
  SegmenT  parent;
  SegPPtr  seg;

  parent = NULL;
  if (segment != NULL) {
    seg = (SegPPtr) segment;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      parent = (SegmenT) seg->seg.parent;
      if (parent != NULL) {
        seg = (SegPPtr) parent;
        if (seg->base.code != SEGMENT && seg->base.code != PICTURE) {
          Message (MSG_ERROR, "ParentSegment parent not a segment or picture");
        }
      }
    } else {
      Message (MSG_ERROR, "ParentSegment argument not a segment or picture");
    }
  }
  return parent;
}

/*****************************************************************************
*
*   SegmentID (segment)
*       Returns the segment ID of a segment
*
*****************************************************************************/

Int2 SegmentID (SegmenT segment)

{
  SegPPtr  seg;
  Int2     segID;

  segID = 0;
  if (segment != NULL) {
    seg = (SegPPtr) segment;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      segID = seg->seg.segID;
    } else {
      Message (MSG_ERROR, "SegmentID argument not a segment or picture");
    }
  }
  return segID;
}

/*****************************************************************************
*
*   SegmentVisible (segment)
*       Returns the visibility of a segment
*
*****************************************************************************/

Boolean SegmentVisible (SegmenT segment)

{
  SegPPtr  seg;
  Boolean  vis;

  vis = FALSE;
  if (segment != NULL) {
    seg = (SegPPtr) segment;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      vis = seg->seg.visible;
    } else {
      Message (MSG_ERROR, "SegmentVisible argument not a segment or picture");
    }
  }
  return vis;
}

/*****************************************************************************
*
*   SegmentStyle (segment)
*       Returns the highlight style of a segment
*
*****************************************************************************/

Int1 SegmentStyle (SegmenT segment)

{
  Int1     highlight;
  SegPPtr  seg;

  highlight = PLAIN_SEGMENT;
  if (segment != NULL) {
    seg = (SegPPtr) segment;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      highlight = seg->seg.highlight;
    } else {
      Message (MSG_ERROR, "SegmentStyle argument not a segment or picture");
    }
  }
  return highlight;
}

/*****************************************************************************
*
*   SegmentBox (segment, box, mrg)
*       Returns the bounding box and mrg of a segment
*
*****************************************************************************/

void SegmentBox (SegmenT segment, BoxPtr box, RectPtr mrg)

{
  SegPPtr  seg;

  if (segment != NULL) {
    seg = (SegPPtr) segment;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      if (box != NULL) {
        *box = seg->seg.box;
      }
      if (mrg != NULL) {
        *mrg = seg->seg.margin;
      }
    } else {
      Message (MSG_ERROR, "SegmentBox argument not a segment or picture");
    }
  }
}

/*****************************************************************************
*
*   GetPrimitive (segment, primCt)
*       Gets a primitive by index within a segment
*
*****************************************************************************/

PrimitivE GetPrimitive (SegmenT segment, Uint2 primCt)

{
  BasePPtr  item;
  SegPPtr   seg;

  item = NULL;
  if (segment != NULL && primCt > 0) {
    seg = (SegPPtr) segment;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      item = seg->seg.head;
      while (item != NULL && primCt > 1) {
        item = item->next;
        primCt--;
      }
    } else {
      Message (MSG_ERROR, "FindPrimitive argument not a segment or picture");
    }
  }
  return (PrimitivE) item;
}

/*****************************************************************************
*
*   AdjustParent (parent, left, top, right, bottom)
*       Recalculates segment boundaries, propagates up parent list
*
*****************************************************************************/

static void AdjustParent (SegmenT parent, Int4 left, Int4 top, Int4 right, Int4 bottom)

{
  SegPPtr  seg;

  if (parent != NULL) {
    seg = (SegPPtr) parent;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      if (seg->seg.box.left > left) {
        seg->seg.box.left = left;
      }
      if (seg->seg.box.top < top) {
        seg->seg.box.top = top;
      }
      if (seg->seg.box.right < right) {
        seg->seg.box.right = right;
      }
      if (seg->seg.box.bottom > bottom) {
        seg->seg.box.bottom = bottom;
      }
      if (seg->base.code == SEGMENT) {
        AdjustParent ((SegmenT) seg->seg.parent, left, top, right, bottom);
      }
    } else {
      Message (MSG_ERROR, "AdjustParent argument not a segment or picture");
    }
  } else {
    Message (MSG_ERROR, "AdjustParent sent a null parent");
  }
}

/*****************************************************************************
*
*   AdjustMargin (parent, left, top, right, bottom)
*       Recalculates segment margin, propagates up parent list
*
*****************************************************************************/

static void AdjustMargin (SegmenT parent, Int2 left, Int2 top, Int2 right, Int2 bottom)

{
  SegPPtr  seg;

  if (parent != NULL) {
    seg = (SegPPtr) parent;
    if (seg->base.code == SEGMENT || seg->base.code == PICTURE) {
      if (seg->seg.margin.left > left) {
        seg->seg.margin.left = left;
      }
      if (seg->seg.margin.top > top) {
        seg->seg.margin.top = top;
      }
      if (seg->seg.margin.right < right) {
        seg->seg.margin.right = right;
      }
      if (seg->seg.margin.bottom < bottom) {
        seg->seg.margin.bottom = bottom;
      }
      if (seg->base.code == SEGMENT) {
        AdjustMargin ((SegmenT) seg->seg.parent, left, top, right, bottom);
      }
    } else {
      Message (MSG_ERROR, "AdjustMargin argument not a segment or picture");
    }
  } else {
    Message (MSG_ERROR, "AdjustMargin sent a null parent");
  }
}

/*****************************************************************************
*
*   AddAttribute (parent, flags, color, linestyle, shading, penwidth, mode)
*       Changes current attribute settings (applied only to lower levels)
*
*****************************************************************************/

void AddAttribute (SegmenT parent, Uint1 flags, Uint1Ptr color,
                   Int1 linestyle, Int1 shading, Int1 penwidth, Int1 mode)

{
  AttPPtr  att;
  SegPPtr  prnt;

  att = (AttPPtr) CreatePrimitive (parent, ATTRIBUTE, sizeof (AttPRec));
  if (att != NULL) {
    if (linestyle >= NO_LINE_STYLE && linestyle <= DASHED_LINE) {
      if (shading >= NO_SHADING && shading <= EMPTY_SHADING) {
        att->att.flags = flags;
        if (color != NULL) {
          att->att.color [0] = color [0];
          att->att.color [1] = color [1];
          att->att.color [2] = color [2];
        } else {
          att->att.color [0] = BLACK_COLOR [0];
          att->att.color [1] = BLACK_COLOR [1];
          att->att.color [2] = BLACK_COLOR [2];
        }
        att->att.linestyle = linestyle;
        att->att.shading = shading;
        att->att.penwidth = MIN (penwidth, PEN_MAX);
        att->att.mode = mode;
        att->att.highlight = PLAIN_SEGMENT;
        prnt = (SegPPtr) parent;
        if (prnt != NULL) {
          prnt->seg.penwidth = penwidth;
        }
      } else {
        Message (MSG_ERROR, "AddAttribute shading out of range");
      }
    } else {
      Message (MSG_ERROR, "AddAttribute line style out of range");
    }
  }
}

/*****************************************************************************
*
*   AddRectangle (parent, left, top, right, bottom, arrow, fill, primID)
*
*****************************************************************************/

PrimitivE AddRectangle (SegmenT parent, Int4 left, Int4 top, Int4 right,
                        Int4 bottom, Int2 arrow, Boolean fill, Uint2 primID)

{
  Int2     align;
  Int2     height;
  RecT     rct;
  RecPPtr  rec;
  Int4     swap;
  Int2     width;

  rec = (RecPPtr) CreatePrimitive (parent, RECTANGLE, sizeof (RecPRec));
  if (rec != NULL) {
    if (arrow >= NO_ARROW && arrow <= DOWN_ARROW) {
      if (left > right) {
        swap = left;
        left = right;
        right = swap;
      }
      if (bottom > top) {
        swap = bottom;
        bottom = top;
        top = swap;
      }
      rec->box.left = left;
      rec->box.top = top;
      rec->box.right = right;
      rec->box.bottom = bottom;
      rec->arrow = arrow;
      rec->fill = fill;
      rec->highlight = PLAIN_PRIMITIVE;
      rec->primID = primID;
      align = 0;
      width = 6;
      height = 6;
      switch (arrow) {
        case NO_ARROW :
          align = MIDDLE_CENTER;
          break;
        case LEFT_ARROW :
          align = MIDDLE_LEFT;
          width = ARROW_PIXELS + ARROW_OVERHANG;
          break;
        case RIGHT_ARROW :
          align = MIDDLE_RIGHT;
          width = ARROW_PIXELS + ARROW_OVERHANG;
          break;
        case UP_ARROW :
          align = UPPER_CENTER;
          height = ARROW_PIXELS + ARROW_OVERHANG;
          break;
        case DOWN_ARROW :
          align = LOWER_CENTER;
          height = ARROW_PIXELS + ARROW_OVERHANG;
          break;
        default :
          break;
      }
      MapToRect (&rct, emptyPt, width, height, 0, align);
      rec->rct = rct;
      AdjustParent (parent, left, top, right, bottom);
      AdjustMargin (parent, rct.left, rct.top, rct.right, rct.bottom);
    } else {
      Message (MSG_ERROR, "AddRectangle arrow out of range");
    }
  }
  return (PrimitivE) rec;
}

/*****************************************************************************
*
*   AddLine (parent, pnt1X, pnt1Y, pnt2X, pnt2Y, arrow, primID)
*
*****************************************************************************/

PrimitivE AddLine (SegmenT parent, Int4 pnt1X, Int4 pnt1Y,
                   Int4 pnt2X, Int4 pnt2Y, Boolean arrow, Uint2 primID)

{
  Int2     length;
  LinPPtr  lin;
  RecT     rct;
  SegPPtr  seg;

  lin = (LinPPtr) CreatePrimitive (parent, LINE, sizeof (LinPRec));
  if (lin != NULL) {
    lin->pnt1.x = pnt1X;
    lin->pnt1.y = pnt1Y;
    lin->pnt2.x = pnt2X;
    lin->pnt2.y = pnt2Y;
    lin->arrow = arrow;
    lin->highlight = PLAIN_PRIMITIVE;
    lin->primID = primID;
    seg = (SegPPtr) parent;
    if (arrow) {
      length = (ARROW_LENGTH + seg->seg.penwidth) * 2;
    } else {
      length = seg->seg.penwidth * 2;
    }
    MapToRect (&rct, emptyPt, length, length, 0, MIDDLE_CENTER);
    lin->rct = rct;
    AdjustParent (parent, MIN (pnt1X, pnt2X), MAX (pnt1Y, pnt2Y),
                  MAX (pnt1X, pnt2X), MIN (pnt1Y, pnt2Y));
    AdjustMargin (parent, rct.left, rct.top, rct.right, rct.bottom);
  }
  return (PrimitivE) lin;
}

/*****************************************************************************
*
*   AddSymbol (parent, pntX, pntY, symbol, fill, align, primID)
*
*****************************************************************************/

static Uint1 rectSym [] = {
  0xFE, 0x82, 0x82, 0x82, 0x82, 0x82, 0xFE, 0x00
};
static Uint1 diamondSym [] = {
  0x10, 0x28, 0x44, 0x82, 0x44, 0x28, 0x10, 0x00
};
static Uint1 ovalSym [] = {
  0x38, 0x44, 0x82, 0x82, 0x82, 0x44, 0x38, 0x00
};
static Uint1 leftTriSym [] = {
  0x06, 0x1A, 0x62, 0x82, 0x62, 0x1A, 0x06, 0x00
};
static Uint1 rightTriSym [] = {
  0xC0, 0xB0, 0x8C, 0x82, 0x8C, 0xB0, 0xC0, 0x00
};
static Uint1 upTriSym [] = {
  0x10, 0x28, 0x28, 0x44, 0x44, 0x82, 0xFE, 0x00
};
static Uint1 downTriSym [] = {
  0xFE, 0x82, 0x44, 0x44, 0x28, 0x28, 0x10, 0x00
};
static Uint1 rectFillSym [] = {
  0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0x00
};
static Uint1 diamondFillSym [] = {
  0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x00
};
static Uint1 ovalFillSym [] = {
  0x38, 0x7C, 0xFE, 0xFE, 0xFE, 0x7C, 0x38, 0x00
};
static Uint1 leftTriFillSym [] = {
  0x06, 0x1E, 0x7E, 0xFE, 0x7E, 0x1E, 0x06, 0x00
};
static Uint1 rightTriFillSym [] = {
  0xC0, 0xF0, 0xFC, 0xFE, 0xFC, 0xF0, 0xC0, 0x00
};
static Uint1 upTriFillSym [] = {
  0x10, 0x38, 0x38, 0x7C, 0x7C, 0xFE, 0xFE, 0x00
};
static Uint1 downTriFillSym [] = {
  0xFE, 0xFE, 0x7C, 0x7C, 0x38, 0x38, 0x10, 0x00
};

static Uint1Ptr symbolList [] = {
  rectSym, diamondSym, ovalSym, leftTriSym,
  rightTriSym, upTriSym, downTriSym,
  rectFillSym, diamondFillSym, ovalFillSym, leftTriFillSym,
  rightTriFillSym, upTriFillSym, downTriFillSym
};

PrimitivE AddSymbol (SegmenT parent, Int4 pntX, Int4 pntY,
                     Int2 symbol, Boolean fill, Int2 align, Uint2 primID)

{
  Int2     index;
  RecT     rct;
  SymPPtr  sym;

  sym = (SymPPtr) CreatePrimitive (parent, SYMBOL, sizeof (SymPRec));
  if (sym != NULL) {
    if (align >= MIDDLE_CENTER && align <= MIDDLE_RIGHT) {
      if (symbol >= RECT_SYMBOL && symbol <= DOWN_TRIANGLE_SYMBOL) {
        sym->pnt.x = pntX;
        sym->pnt.y = pntY;
        MapToRect (&rct, emptyPt, SYMBOL_WIDTH, SYMBOL_HEIGHT, 0, align);
        sym->rct = rct;
        index = symbol - RECT_SYMBOL;
        if (fill) {
          index += 7;
        }
        sym->data = symbolList [index];
        sym->primID = primID;
        AdjustParent (parent, pntX, pntY, pntX, pntY);
        AdjustMargin (parent, rct.left, rct.top, rct.right, rct.bottom);
      } else {
        Message (MSG_ERROR, "AddSymbol symbol out of range");
      }
    } else {
      Message (MSG_ERROR, "AddSymbol alignment out of range");
    }
  }
  return (PrimitivE) sym;
}

/*****************************************************************************
*
*   AddBitmap (parent, pntX, pntY, width, height, data, align, primID)
*
*****************************************************************************/

PrimitivE AddBitmap (SegmenT parent, Int4 pntX, Int4 pntY, Int2 width,
                     Int2 height, Uint1Ptr data, Int2 align, Uint2 primID)

{
  BtmPPtr  btm;
  RecT     rct;

  btm = (BtmPPtr) CreatePrimitive (parent, BITMAP, sizeof (BtmPRec));
  if (btm != NULL) {
    if (align >= MIDDLE_CENTER && align <= MIDDLE_RIGHT) {
      btm->pnt.x = pntX;
      btm->pnt.y = pntY;
      MapToRect (&rct, emptyPt, width, height, 0, align);
      btm->rct = rct;
      btm->data = data;
      btm->primID = primID;
      AdjustParent (parent, pntX, pntY, pntX, pntY);
      AdjustMargin (parent, rct.left, rct.top, rct.right, rct.bottom);
    } else {
      Message (MSG_ERROR, "AddBitmap alignment out of range");
    }
  }
  return (PrimitivE) btm;
}

/*****************************************************************************
*
*   AddCustom (parent, pntX, pntY, width, height, proc, align, primID)
*
*****************************************************************************/

PrimitivE AddCustom (SegmenT parent, Int4 pntX, Int4 pntY, Int2 width,
                     Int2 height, CustomProc proc, Int2 align, Uint2 primID)

{
  CstPPtr  cst;
  RecT     rct;

  cst = (CstPPtr) CreatePrimitive (parent, CUSTOM, sizeof (CstPRec));
  if (cst != NULL) {
    if (align >= MIDDLE_CENTER && align <= MIDDLE_RIGHT) {
      cst->pnt.x = pntX;
      cst->pnt.y = pntY;
      MapToRect (&rct, emptyPt, width, height, 0, align);
      cst->rct = rct;
      cst->proc = proc;
      cst->primID = primID;
      AdjustParent (parent, pntX, pntY, pntX, pntY);
      AdjustMargin (parent, rct.left, rct.top, rct.right, rct.bottom);
    } else {
      Message (MSG_ERROR, "AddCustom alignment out of range");
    }
  }
  return (PrimitivE) cst;
}

/*****************************************************************************
*
*   AddMarker (parent, pntX, pntY, length, orient, primID)
*
*****************************************************************************/

PrimitivE AddMarker (SegmenT parent, Int4 pntX, Int4 pntY,
                     Int2 length, Int2 orient, Uint2 primID)

{
  Int2     align;
  Int2     height;
  MrkPPtr  mrk;
  RecT     rct;
  Int2     width;

  mrk = (MrkPPtr) CreatePrimitive (parent, MARKER, sizeof (MrkPRec));
  if (mrk != NULL) {
    if (orient >= HORIZ_LEFT && orient <= VERT_BELOW) {
      mrk->pnt.x = pntX;
      mrk->pnt.y = pntY;
      align = 0;
      width = 0;
      height = 0;
      switch (orient) {
        case HORIZ_LEFT :
          align = MIDDLE_LEFT;
          width = length;
          break;
        case HORIZ_CENTER :
          align = MIDDLE_CENTER;
          width = length;
          break;
        case HORIZ_RIGHT :
          align = MIDDLE_RIGHT;
          width = length;
          break;
        case VERT_ABOVE :
          align = UPPER_CENTER;
          height = length;
          break;
        case VERT_MIDDLE :
          align = MIDDLE_CENTER;
          height = length;
          break;
        case VERT_BELOW :
          align = LOWER_CENTER;
          height = length;
          break;
        default :
          break;
      }
      MapToRect (&rct, emptyPt, width, height, 0, align);
      mrk->rct = rct;
      mrk->primID = primID;
      AdjustParent (parent, pntX, pntY, pntX, pntY);
      AdjustMargin (parent, rct.left, rct.top, rct.right, rct.bottom);
    } else {
      Message (MSG_ERROR, "AddMarker orientation out of range");
    }
  }
  return (PrimitivE) mrk;
}

/*****************************************************************************
*
*   AddLabel (parent, pntX, pntY, string, size, offset, align, primID)
*
*****************************************************************************/

PrimitivE AddLabel (SegmenT parent, Int4 pntX, Int4 pntY, CharPtr string,
                    Int2 size, Int2 offset, Int2 align, Uint2 primID)

{
  FonT     fnt;
  LblPPtr  lbl;
  RecT     rct;

  lbl = (LblPPtr) CreatePrimitive (parent, LABEL, sizeof (LblPRec));
  if (lbl != NULL) {
    if (align >= MIDDLE_CENTER && align <= MIDDLE_RIGHT) {
      lbl->pnt.x = pntX;
      lbl->pnt.y = pntY;
      lbl->str = StringSave (string);
      lbl->size = size;
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
      MapToRect (&rct, emptyPt, StringWidth (string) + 2,
                 LineHeight (), offset, align);
      lbl->rct = rct;
      lbl->primID = primID;
      AdjustParent (parent, pntX, pntY, pntX, pntY);
      AdjustMargin (parent, rct.left, rct.top, rct.right, rct.bottom);
    } else {
      Message (MSG_ERROR, "AddLabel alignment out of range");
    }
  }
  return (PrimitivE) lbl;
}
