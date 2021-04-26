/*   ncbiport.h
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
* File Name:  ncbiport.h
*
* Author:  Jonathan Kans
*
* Version Creation Date:   1/1/91
*
* $Revision: 2.2 $
*
* File Description: 
*       Vibrant drawing port specification definitions
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBIPORT_
#define _NCBIPORT_

#ifndef _NCBI_
#include <ncbi.h>
#endif
#ifndef _NCBIDRAW_
#include <ncbidraw.h>
#endif
#ifndef _NCBIWIN_
#include <ncbiwin.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN_MAC
#define Nlm_PointTool Point
#define Nlm_RectTool Rect
#define Nlm_RgnTool RgnHandle
#define Nlm_FontTool Nlm_Handle
#endif

#ifdef WIN_MSWIN
#define Nlm_PointTool POINT
#define Nlm_RectTool RECT
#define Nlm_RgnTool HRGN
#define Nlm_FontTool HFONT
#endif

#ifdef WIN_X
#define Nlm_PointTool XPoint
#define Nlm_RectTool XRectangle
#define Nlm_RgnTool Region
#define Nlm_FontTool XFontStruct*
#endif

/* The font structure is created using a handle.  The FntPtr is
*  returned by a call to Nlm_HandLock (font).  Be sure to call
*  Nlm_HandUnlock (font) after obtaining the font handle.
*/

typedef  struct  Nlm_fontrec {
  Nlm_FonT      next;
  Nlm_Char      name [32];
  Nlm_Int2      size;
  Nlm_Int2      scale;
  Nlm_Boolean   bld;
  Nlm_Boolean   ital;
  Nlm_Boolean   undlin;
  Nlm_Int2      number;
  Nlm_Int2      style;
  Nlm_FontTool  handle;
  Nlm_FonT      print;
} Nlm_FontRec, Nlm_FontData, PNTR Nlm_FntPtr;

extern Nlm_Boolean  Nlm_nowPrinting;

#ifdef WIN_MAC
extern  RGBColor  Nlm_RGBforeColor;
extern  RGBColor  Nlm_RGBbackColor;
#endif

#ifdef WIN_MSWIN
extern  HWND  Nlm_currentHWnd;
extern  HDC   Nlm_currentHDC;
#endif

#ifdef WIN_X
extern  Display    *Nlm_currentXDisplay;
extern  int        Nlm_currentXScreen;
extern  Window     Nlm_currentXWindow;
extern  GC         Nlm_currentXGC;
extern  Nlm_Uint4  Nlm_XbackColor;
extern  Nlm_Uint4  Nlm_XforeColor;
extern  Nlm_Int2   Nlm_XOffset;
extern  Nlm_Int2   Nlm_YOffset;
extern Nlm_RegioN  Nlm_clpRgn;
#endif

#ifdef WIN_MAC
void  Nlm_SetPort PROTO((GrafPtr grafptr));
#endif

#ifdef WIN_MSWIN
void  Nlm_SetPort PROTO((HWND hwnd, HDC hdc));
#endif

#ifdef WIN_X
void  Nlm_SetPort PROTO((Window window, GC gc));
#endif

#ifdef __cplusplus
}
#endif

#endif
