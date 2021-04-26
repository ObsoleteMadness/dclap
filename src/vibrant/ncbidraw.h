/*   ncbidraw.h
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
* File Name:  ncbidraw.h
*
* Author:  Jonathan Kans
*
* Version Creation Date:   1/1/91
*
* $Revision: 2.4 $
*
* File Description: 
*       Vibrant drawing procedure definitions
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBIDRAW_
#define _NCBIDRAW_

#ifdef __cplusplus
extern "C" {
#endif

/***  PORTABLE GRAPHIC PRIMITIVE OBJECT TYPES  ***/

/*
*  Platform independent point and rectangle data structures.
*/

typedef  struct  Nlm_point {
  Nlm_Int2  x;
  Nlm_Int2  y;
} Nlm_PoinT, PNTR Nlm_PointPtr;

typedef  struct  Nlm_rect {
  Nlm_Int2  left;
  Nlm_Int2  top;
  Nlm_Int2  right;
  Nlm_Int2  bottom;
} Nlm_RecT, PNTR Nlm_RectPtr;

typedef  Nlm_Handle  Nlm_RegioN;

typedef  struct  Nlm_font {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_FonT;

/***  GLOBAL VARIABLES  ***/

/*
*  The update region contains the update region on drawing requests,
*  and contains at least the visible region during other callbacks.
*  Clipping is set to the update region on drawing events.  The
*  update rectangle has the minimum rectangular boundary of the
*  update region.  These variables are kept up to date by the Vibrant
*  event loop.
*/

extern  Nlm_RegioN  Nlm_updateRgn;
extern  Nlm_RecT    Nlm_updateRect;

/*
*  The standard systemFont and programFont variables are used to
*  specify fonts for dialog objects and scrolling text objects.
*/

extern  Nlm_FonT  Nlm_systemFont;
extern  Nlm_FonT  Nlm_programFont;

/*
*  Miscellaneous constants for pixel sizes of the standard font.
*/

extern  Nlm_Int2  Nlm_stdAscent;
extern  Nlm_Int2  Nlm_stdDescent;
extern  Nlm_Int2  Nlm_stdLeading;
extern  Nlm_Int2  Nlm_stdFontHeight;
extern  Nlm_Int2  Nlm_stdLineHeight;
extern  Nlm_Int2  Nlm_stdCharWidth;

/***  DRAWING PROCEDURES  ***/

void         Nlm_SetUpDrawingTools PROTO((void));
void         Nlm_CleanUpDrawingTools PROTO((void));

/*
*  It is not necessary to create a new font when switching colors.  The
*  family for GetFont can be Roman, Swiss, Modern, Script, or Decorative.
*/

/*
*  ScrollRect will erase and invalidate all invalid regions.
*/

void         Nlm_ResetDrawingTools PROTO((void));

void         Nlm_CopyMode PROTO((void));
void         Nlm_MergeMode PROTO((void));
void         Nlm_InvertMode PROTO((void));
void         Nlm_EraseMode PROTO((void));

void         Nlm_Black PROTO((void));
void         Nlm_Red PROTO((void));
void         Nlm_Green PROTO((void));
void         Nlm_Blue PROTO((void));
void         Nlm_Cyan PROTO((void));
void         Nlm_Magenta PROTO((void));
void         Nlm_Yellow PROTO((void));
void         Nlm_White PROTO((void));
void         Nlm_Gray PROTO((void));
void         Nlm_LtGray PROTO((void));
void         Nlm_DkGray PROTO((void));
void         Nlm_SelectColor PROTO((Nlm_Uint1 red, Nlm_Uint1 green, Nlm_Uint1 blue));
Nlm_Uint4    Nlm_GetColor PROTO((void));
void         Nlm_SetColor PROTO((Nlm_Uint4 color));
void         Nlm_InvertColors PROTO((void));

void         Nlm_Solid PROTO((void));
void         Nlm_Dark PROTO((void));
void         Nlm_Medium PROTO((void));
void         Nlm_Light PROTO((void));
void         Nlm_Empty PROTO((void));
void         Nlm_Dotted PROTO((void));
void         Nlm_Dashed PROTO((void));
void         Nlm_WidePen PROTO((Nlm_Int2 width));

Nlm_FonT     Nlm_GetFont PROTO((Nlm_CharPtr name, Nlm_Int2 size, Nlm_Boolean bld, Nlm_Boolean itlc, Nlm_Boolean undrln, Nlm_CharPtr fmly));
Nlm_FonT     Nlm_ParseFont PROTO((Nlm_CharPtr spec));
void         Nlm_SelectFont PROTO((Nlm_FonT f));
void         Nlm_AssignPrinterFont PROTO((Nlm_FonT scrnFont, Nlm_FonT prtrFont));

Nlm_Int2     Nlm_CharWidth PROTO((Nlm_Char ch));
Nlm_Int2     Nlm_StringWidth PROTO((Nlm_CharPtr text));
Nlm_Int2     Nlm_TextWidth PROTO((Nlm_CharPtr text, Nlm_sizeT len));
Nlm_Int2     Nlm_Ascent PROTO((void));
Nlm_Int2     Nlm_Descent PROTO((void));
Nlm_Int2     Nlm_Leading PROTO((void));
Nlm_Int2     Nlm_FontHeight PROTO((void));
Nlm_Int2     Nlm_LineHeight PROTO((void));
Nlm_Int2     Nlm_MaxCharWidth PROTO((void));

void         Nlm_SetPen PROTO((Nlm_PoinT pt));
void         Nlm_GetPen PROTO((Nlm_PointPtr pt));

void         Nlm_PaintChar PROTO((Nlm_Char ch));
void         Nlm_PaintString PROTO((Nlm_CharPtr text));
void CDECL   Nlm_PaintText VPROTO((char *format, ...));

void         Nlm_DrawString PROTO((Nlm_RectPtr r, Nlm_CharPtr text, Nlm_Char jst, Nlm_Boolean gray));
void         Nlm_DrawText PROTO((Nlm_RectPtr r, Nlm_CharPtr text, Nlm_sizeT len, Nlm_Char jst, Nlm_Boolean gray));

void         Nlm_MoveTo PROTO((Nlm_Int2 x, Nlm_Int2 y));
void         Nlm_LineTo PROTO((Nlm_Int2 x, Nlm_Int2 y));
void         Nlm_DrawLine PROTO((Nlm_PoinT pt1, Nlm_PoinT pt2));

void         Nlm_LoadPt PROTO((Nlm_PointPtr pt, Nlm_Int2 x, Nlm_Int2 y));
void         Nlm_AddPt PROTO((Nlm_PoinT src, Nlm_PointPtr dst));
void         Nlm_SubPt PROTO((Nlm_PoinT src, Nlm_PointPtr dst));
Nlm_Boolean  Nlm_EqualPt PROTO((Nlm_PoinT p1, Nlm_PoinT p2));
Nlm_Boolean  Nlm_PtInRect PROTO((Nlm_PoinT pt, Nlm_RectPtr r));
Nlm_Boolean  Nlm_PtInRgn PROTO((Nlm_PoinT pt, Nlm_RegioN rgn));

void         Nlm_LoadRect PROTO((Nlm_RectPtr r, Nlm_Int2 lf, Nlm_Int2 tp, Nlm_Int2 rt, Nlm_Int2 bt));
void         Nlm_UpsetRect PROTO((Nlm_RectPtr r, Nlm_Int2 lf, Nlm_Int2 tp, Nlm_Int2 rt, Nlm_Int2 bt));
void         Nlm_OffsetRect PROTO((Nlm_RectPtr r, Nlm_Int2 dx, Nlm_Int2 dy));
void         Nlm_InsetRect PROTO((Nlm_RectPtr r, Nlm_Int2 dx, Nlm_Int2 dy));
Nlm_Boolean  Nlm_SectRect PROTO((Nlm_RectPtr src1, Nlm_RectPtr src2, Nlm_RectPtr dst));
Nlm_Boolean  Nlm_UnionRect PROTO((Nlm_RectPtr src1, Nlm_RectPtr src2, Nlm_RectPtr dst));
Nlm_Boolean  Nlm_EqualRect PROTO((Nlm_RectPtr r1, Nlm_RectPtr r2));
Nlm_Boolean  Nlm_EmptyRect PROTO((Nlm_RectPtr r));
Nlm_Boolean  Nlm_RectInRect PROTO((Nlm_RectPtr r1, Nlm_RectPtr r2));
Nlm_Boolean  Nlm_RectInRgn PROTO((Nlm_RectPtr r, Nlm_RegioN rgn));

void         Nlm_EraseRect PROTO((Nlm_RectPtr r));
void         Nlm_FrameRect PROTO((Nlm_RectPtr r));
void         Nlm_PaintRect PROTO((Nlm_RectPtr r));
void         Nlm_InvertRect PROTO((Nlm_RectPtr r));
void         Nlm_ScrollRect PROTO((Nlm_RectPtr r, Nlm_Int2 dx, Nlm_Int2 dy));

void         Nlm_EraseOval PROTO((Nlm_RectPtr r));
void         Nlm_FrameOval PROTO((Nlm_RectPtr r));
void         Nlm_PaintOval PROTO((Nlm_RectPtr r));
void         Nlm_InvertOval PROTO((Nlm_RectPtr r));

void         Nlm_EraseRoundRect PROTO((Nlm_RectPtr r, Nlm_Int2 ovlWid, Nlm_Int2 ovlHgt));
void         Nlm_FrameRoundRect PROTO((Nlm_RectPtr r, Nlm_Int2 ovlWid, Nlm_Int2 ovlHgt));
void         Nlm_PaintRoundRect PROTO((Nlm_RectPtr r, Nlm_Int2 ovlWid, Nlm_Int2 ovlHgt));
void         Nlm_InvertRoundRect PROTO((Nlm_RectPtr r, Nlm_Int2 ovlWid, Nlm_Int2 ovlHgt));

void         Nlm_EraseArc PROTO((Nlm_RectPtr r, Nlm_PoinT start, Nlm_PoinT end));
void         Nlm_FrameArc PROTO((Nlm_RectPtr r, Nlm_PoinT start, Nlm_PoinT end));
void         Nlm_PaintArc PROTO((Nlm_RectPtr r, Nlm_PoinT start, Nlm_PoinT end));
void         Nlm_InvertArc PROTO((Nlm_RectPtr r, Nlm_PoinT start, Nlm_PoinT end));

void         Nlm_ErasePoly PROTO((Nlm_Int2 num, Nlm_PointPtr pts));
void         Nlm_FramePoly PROTO((Nlm_Int2 num, Nlm_PointPtr pts));
void         Nlm_PaintPoly PROTO((Nlm_Int2 num, Nlm_PointPtr pts));
void         Nlm_InvertPoly PROTO((Nlm_Int2 num, Nlm_PointPtr pts));

Nlm_RegioN   Nlm_CreateRgn PROTO((void));
Nlm_RegioN   Nlm_DestroyRgn PROTO((Nlm_RegioN rgn));
void         Nlm_ClearRgn PROTO((Nlm_RegioN rgn));
void         Nlm_LoadRectRgn PROTO((Nlm_RegioN rgn, Nlm_Int2 lf, Nlm_Int2 tp, Nlm_Int2 rt, Nlm_Int2 bt));
void         Nlm_OffsetRgn PROTO((Nlm_RegioN rgn, Nlm_Int2 dx, Nlm_Int2 dy));
void         Nlm_SectRgn PROTO((Nlm_RegioN src1, Nlm_RegioN src2, Nlm_RegioN dst));
void         Nlm_UnionRgn PROTO((Nlm_RegioN src1, Nlm_RegioN src2, Nlm_RegioN dst));
void         Nlm_DiffRgn PROTO((Nlm_RegioN src1, Nlm_RegioN src2, Nlm_RegioN dst));
void         Nlm_XorRgn PROTO((Nlm_RegioN src1, Nlm_RegioN src2, Nlm_RegioN dst));
Nlm_Boolean  Nlm_EqualRgn PROTO((Nlm_RegioN rgn1, Nlm_RegioN rgn2));
Nlm_Boolean  Nlm_EmptyRgn PROTO((Nlm_RegioN rgn));

void         Nlm_EraseRgn PROTO((Nlm_RegioN rgn));
void         Nlm_FrameRgn PROTO((Nlm_RegioN rgn));
void         Nlm_PaintRgn PROTO((Nlm_RegioN rgn));
void         Nlm_InvertRgn PROTO((Nlm_RegioN rgn));

void         Nlm_ClipRect PROTO((Nlm_RectPtr r));
void         Nlm_ClipRgn PROTO((Nlm_RegioN rgn));
void         Nlm_ResetClip PROTO((void));

void         Nlm_ValidRect PROTO((Nlm_RectPtr r));
void         Nlm_InvalRect PROTO((Nlm_RectPtr r));
void         Nlm_ValidRgn PROTO((Nlm_RegioN rgn));
void         Nlm_InvalRgn PROTO((Nlm_RegioN rgn));

void         Nlm_CopyBits PROTO((Nlm_RectPtr r, Nlm_VoidPtr source));

#ifdef __cplusplus
}
#endif

#endif
