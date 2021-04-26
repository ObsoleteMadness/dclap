/*   picture.h
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
* File Name:  picture.h
*
* Author:  Jonathan Kans, Jill Shermer
*
* Version Creation Date:   10/23/92
*
* $Revision: 1.11 $
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

#ifndef _PICTURE_
#define _PICTURE_

#ifndef _VIBRANT_
#include <vibrant.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   STRUCTURE TYPEDEFS
*
*****************************************************************************/

typedef struct Nlm_segment {
  Nlm_VoidPtr  dummy;
} PNTR Nlm_SegmenT;

typedef struct Nlm_primitive {
  Nlm_VoidPtr  dummy;
} PNTR Nlm_PrimitivE;

/*  Custom callback type */

typedef void (*Nlm_CustomProc) PROTO((Nlm_Int2, Nlm_Int2, Nlm_Int2, Nlm_Int2));

/*  BoxInfo stores the world coordinates of a rectangle */

typedef struct Nlm_boxinfo {
  Nlm_Int4  left;
  Nlm_Int4  top;
  Nlm_Int4  right;
  Nlm_Int4  bottom;
} Nlm_BoxInfo, PNTR Nlm_BoxPtr;

/*  PntInfo stores the world coordinates of a point */

typedef struct Nlm_pntinfo {
  Nlm_Int4  x;
  Nlm_Int4  y;
} Nlm_PntInfo, PNTR Nlm_PntPtr;

/*****************************************************************************
*
*   SYMBOL DEFINES
*
*****************************************************************************/

/*  Attribute flag bits */

#define NO_ATTS             0
#define COLOR_ATT           1
#define STYLE_ATT           2
#define SHADING_ATT         4
#define WIDTH_ATT           8
#define MODE_ATT           16

/*  Color specifications */

extern Uint1 BLACK_COLOR [3];
extern Uint1 WHITE_COLOR [3];
extern Uint1 RED_COLOR [3];
extern Uint1 GREEN_COLOR [3];
extern Uint1 BLUE_COLOR [3];
extern Uint1 YELLOW_COLOR [3];
extern Uint1 CYAN_COLOR [3];
extern Uint1 MAGENTA_COLOR [3];

/*  Line styles */

#define NO_LINE_STYLE          0
#define SOLID_LINE             1
#define DOTTED_LINE            2
#define DASHED_LINE            3

/*  Shading styles */

#define NO_SHADING             0
#define SOLID_SHADING          1
#define DARK_SHADING           2
#define MEDIUM_SHADING         3
#define LIGHT_SHADING          4
#define EMPTY_SHADING          5

/*  Pen width */

#define NO_PEN_WIDTH           0
#define STD_PEN_WIDTH          1

/*  Mode specifications */

#define NO_MODE                0
#define COPY_MODE              1
#define MERGE_MODE             2
#define INVERT_MODE            3
#define ERASE_MODE             4

/*  Rectangle arrow direction */

#define NO_ARROW               0
#define LEFT_ARROW             1
#define RIGHT_ARROW            2
#define UP_ARROW               3
#define DOWN_ARROW             4

/*  Symbol types */

#define RECT_SYMBOL            1
#define DIAMOND_SYMBOL         2
#define OVAL_SYMBOL            3
#define LEFT_TRIANGLE_SYMBOL   4
#define RIGHT_TRIANGLE_SYMBOL  5
#define UP_TRIANGLE_SYMBOL     6
#define DOWN_TRIANGLE_SYMBOL   7

/*  Alignment specifications, relative to point */

#define MIDDLE_CENTER          1
#define UPPER_LEFT             2
#define LOWER_LEFT             3
#define UPPER_RIGHT            4
#define LOWER_RIGHT            5
#define UPPER_CENTER           6
#define LOWER_CENTER           7
#define MIDDLE_LEFT            8
#define MIDDLE_RIGHT           9

/*  Orientation specifications, relative to point */

#define HORIZ_LEFT            11
#define HORIZ_CENTER          12
#define HORIZ_RIGHT           13
#define VERT_ABOVE            14
#define VERT_MIDDLE           15
#define VERT_BELOW            16

/*  Text sizes */

#define SMALL_TEXT             1
#define MEDIUM_TEXT            2
#define LARGE_TEXT             3

/*  Segment highlight style */

#define PLAIN_SEGMENT          0
#define FRAME_SEGMENT          1
#define FILL_SEGMENT           2
#define FRAME_CONTENTS         3
#define FILL_CONTENTS          4

/*  Primitive highlight style */

#define PLAIN_PRIMITIVE        0
#define FRAME_PRIMITIVE        1
#define FILL_PRIMITIVE         2

/*****************************************************************************
*
*   FUNCTION PROTOTYPES
*
*****************************************************************************/

extern Nlm_SegmenT   Nlm_CreatePicture  PROTO((void));
extern Nlm_SegmenT   Nlm_DeletePicture  PROTO((Nlm_SegmenT picture));

extern Nlm_SegmenT   Nlm_CreateSegment  PROTO((Nlm_SegmenT parent, Uint2 segID, Int4 maxScale));
extern Nlm_SegmenT   Nlm_ResetSegment   PROTO((Nlm_SegmenT segment));

extern Nlm_SegmenT   Nlm_ParentSegment  PROTO((Nlm_SegmenT segment));
extern Nlm_Int2      Nlm_SegmentID      PROTO((Nlm_SegmenT segment));
extern Nlm_Boolean   Nlm_SegmentVisible PROTO((Nlm_SegmenT segment));
extern Nlm_Int1      Nlm_SegmentStyle   PROTO((Nlm_SegmenT segment));
extern void          Nlm_SegmentBox     PROTO((Nlm_SegmenT segment, Nlm_BoxPtr box, RectPtr mrg));
extern Nlm_PrimitivE Nlm_GetPrimitive   PROTO((Nlm_SegmenT segment, Uint2 primCt));

extern void          Nlm_AddAttribute   PROTO((Nlm_SegmenT parent, Nlm_Uint1 flags, Nlm_Uint1Ptr color, Nlm_Int1 linestyle, Nlm_Int1 shading, Nlm_Int1 penwidth, Nlm_Int1 mode));
extern Nlm_PrimitivE Nlm_AddRectangle   PROTO((Nlm_SegmenT parent, Nlm_Int4 left, Nlm_Int4 top, Nlm_Int4 right, Nlm_Int4 bottom, Nlm_Int2 arrow, Nlm_Boolean fill, Nlm_Uint2 primID));
extern Nlm_PrimitivE Nlm_AddLine        PROTO((Nlm_SegmenT parent, Nlm_Int4 pnt1X, Nlm_Int4 pnt1Y, Nlm_Int4 pnt2X, Nlm_Int4 pnt2Y, Nlm_Boolean arrow, Nlm_Uint2 primID));
extern Nlm_PrimitivE Nlm_AddSymbol      PROTO((Nlm_SegmenT parent, Nlm_Int4 pntX, Nlm_Int4 pntY, Nlm_Int2 symbol, Nlm_Boolean fill, Nlm_Int2 align, Nlm_Uint2 primID));
extern Nlm_PrimitivE Nlm_AddBitmap      PROTO((Nlm_SegmenT parent, Nlm_Int4 pntX, Nlm_Int4 pntY, Nlm_Int2 width, Nlm_Int2 height, Nlm_Uint1Ptr data, Nlm_Int2 align, Nlm_Uint2 primID));
extern Nlm_PrimitivE Nlm_AddCustom      PROTO((Nlm_SegmenT parent, Int4 pntX, Nlm_Int4 pntY, Nlm_Int2 width, Nlm_Int2 height, Nlm_CustomProc proc, Nlm_Int2 align, Nlm_Uint2 primID));
extern Nlm_PrimitivE Nlm_AddMarker      PROTO((Nlm_SegmenT parent, Nlm_Int4 pntX, Nlm_Int4 pntY, Nlm_Int2 length, Nlm_Int2 orient, Nlm_Uint2 primID));
extern Nlm_PrimitivE Nlm_AddLabel       PROTO((Nlm_SegmenT parent, Nlm_Int4 pntX, Nlm_Int4 pntY, Nlm_CharPtr string, Nlm_Int2 size, Nlm_Int2 offset, Nlm_Int2 align, Nlm_Uint2 primID));

#define SegmenT Nlm_SegmenT
#define PrimitivE Nlm_PrimitivE
#define CustomProc Nlm_CustomProc
#define BoxInfo Nlm_BoxInfo
#define BoxPtr Nlm_BoxPtr
#define PntInfo Nlm_PntInfo
#define PntPtr Nlm_PntPtr
#define CreatePicture Nlm_CreatePicture
#define DeletePicture Nlm_DeletePicture
#define CreateSegment Nlm_CreateSegment
#define ResetSegment Nlm_ResetSegment
#define ParentSegment Nlm_ParentSegment
#define SegmentID Nlm_SegmentID
#define SegmentVisible Nlm_SegmentVisible
#define SegmentStyle Nlm_SegmentStyle
#define SegmentBox Nlm_SegmentBox
#define GetPrimitive Nlm_GetPrimitive
#define AddAttribute Nlm_AddAttribute
#define AddRectangle Nlm_AddRectangle
#define AddLine Nlm_AddLine
#define AddSymbol Nlm_AddSymbol
#define AddBitmap Nlm_AddBitmap
#define AddCustom Nlm_AddCustom
#define AddMarker Nlm_AddMarker
#define AddLabel Nlm_AddLabel

#ifdef __cplusplus
}
#endif

#endif
