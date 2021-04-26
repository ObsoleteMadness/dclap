/*   ncbidraw.c
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
* File Name:  ncbidraw.c
*
* Author:  Jonathan Kans
*
* Version Creation Date:   1/1/91
*
* $Revision: 2.20 $
*
* File Description: 
*       Vibrant drawing functions.
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/


#include <ncbi.h>
#include <ncbidraw.h>
#include <ncbiwin.h>
#include <ncbiport.h>

#ifdef VAR_ARGS
#include <varargs.h>
#else
#include <stdarg.h>
#endif

Nlm_Boolean  Nlm_nowPrinting = FALSE;

#ifdef WIN_MAC
RGBColor  Nlm_RGBforeColor;
RGBColor  Nlm_RGBbackColor;
#endif

#ifdef WIN_MSWIN
HWND         Nlm_currentHWnd;
HDC          Nlm_currentHDC;
#endif

#ifdef WIN_X
Display      *Nlm_currentXDisplay;
int          Nlm_currentXScreen;
Window       Nlm_currentXWindow;
GC           Nlm_currentXGC;
Nlm_Uint4    Nlm_XbackColor;
Nlm_Uint4    Nlm_XforeColor;
Nlm_Int2     Nlm_XOffset;
Nlm_Int2     Nlm_YOffset;
Nlm_RegioN   Nlm_clpRgn;
#endif

Nlm_RegioN   Nlm_updateRgn;
Nlm_RecT     Nlm_updateRect;

Nlm_Int2     Nlm_stdAscent;
Nlm_Int2     Nlm_stdDescent;
Nlm_Int2     Nlm_stdLeading;
Nlm_Int2     Nlm_stdFontHeight;
Nlm_Int2     Nlm_stdLineHeight;
Nlm_Int2     Nlm_stdCharWidth;

Nlm_FonT     Nlm_systemFont = NULL;
Nlm_FonT     Nlm_programFont = NULL;

#define COPY_MODE   1
#define MERGE_MODE  2
#define INVERT_MODE 3
#define ERASE_MODE  4

static Nlm_Int2    currentMode = COPY_MODE;

static Nlm_FonT    Nlm_fontList = NULL;
#ifndef DCLAP
static 
#endif
Nlm_FonT    Nlm_fontInUse = NULL;

static Nlm_RegioN  Nlm_scrollRgn;

#ifdef WIN_MAC
static Nlm_Byte  whitePat [] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static Nlm_Byte  ltGrayPat [] = {0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22};
static Nlm_Byte  grayPat [] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
static Nlm_Byte  dkGrayPat [] = {0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD};
static Nlm_Byte  blackPat [] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static Nlm_Byte  dotPat [] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
static Nlm_Byte  dashPat [] = {0xCC, 0xCC, 0x33, 0x33, 0xCC, 0xCC, 0x33, 0x33};

static Nlm_Boolean  hasColorQD = FALSE;
#endif

#ifdef WIN_MSWIN
static Nlm_Int2    currentPenStyle = PS_SOLID;
static Nlm_Int2    currentPenWidth = 1;
static void        *currentPattern = NULL;

static COLORREF    blackColor;
static COLORREF    redColor;
static COLORREF    greenColor;
static COLORREF    blueColor;
static COLORREF    cyanColor;
static COLORREF    magentaColor;
static COLORREF    yellowColor;
static COLORREF    whiteColor;

static Nlm_Uint2   blackPat [] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static Nlm_Uint2   dkGrayPat [] = {0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22};
static Nlm_Uint2   grayPat [] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
static Nlm_Uint2   ltGrayPat [] = {0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD};
static Nlm_Uint2   whitePat [] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static Nlm_Uint2   dotPat [] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
static Nlm_Uint2   dashPat [] = {0xCC, 0xCC, 0x33, 0x33, 0xCC, 0xCC, 0x33, 0x33};

static HPEN        hBlackPen;
static HPEN        hNullPen;
static HPEN        hWhitePen;
static HBRUSH      hBlackBrush;
static HBRUSH      hDkGrayBrush;
static HBRUSH      hGrayBrush;
static HBRUSH      hHollowBrush;
static HBRUSH      hLtGrayBrush;
static HBRUSH      hNullBrush;
static HBRUSH      hWhiteBrush;
static HFONT       hAnsiFixedFont;
static HFONT       hAnsiVarFont;
static HFONT       hDeviceDefaultFont;
static HFONT       hOemFixedFont;
static HFONT       hSystemFont;
static HFONT       hSystemFixedFont;

static TEXTMETRIC  textMetrics;
#endif

#ifdef WIN_X
static XFontStruct  *currentFont;
static Pixmap       currentPixmap = 0;
static Nlm_PoinT    currentPoint;
static Nlm_Uint4    currentBkColor;
static Nlm_Uint4    currentFgColor;
static int          currentFunction = GXcopy;
static int          currentFillStyle = FillOpaqueStippled;

static Nlm_Uint4    blackColor;
static Nlm_Uint4    redColor;
static Nlm_Uint4    greenColor;
static Nlm_Uint4    blueColor;
static Nlm_Uint4    cyanColor;
static Nlm_Uint4    magentaColor;
static Nlm_Uint4    yellowColor;
static Nlm_Uint4    whiteColor;

static Nlm_Uint1    whitePat [] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static Nlm_Uint1    ltGrayPat [] = {0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22};
static Nlm_Uint1    grayPat [] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
static Nlm_Uint1    dkGrayPat [] = {0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD};
static Nlm_Uint1    blackPat [] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static Nlm_Uint1    dotPat [] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
static Nlm_Uint1    dashPat [] = {0xCC, 0xCC, 0x33, 0x33, 0xCC, 0xCC, 0x33, 0x33};

static XFontStruct  fontInfo;
static Nlm_Uint1    flip [256];

static Nlm_Boolean  hasColor;
static Nlm_RgnTool  emptyRgn;
#endif

#ifdef WIN_MAC
extern  void  Nlm_SetPort (GrafPtr grafptr)

{
  SetPort (grafptr);
}
#endif

#ifdef WIN_MSWIN
extern  void  Nlm_SetPort (HWND hwnd, HDC hdc)

{
  Nlm_currentHWnd = hwnd;
  Nlm_currentHDC = hdc;
}
#endif

#ifdef WIN_X
extern  void  Nlm_SetPort (Window window, GC gc)

{
  Nlm_currentXWindow = window;
  Nlm_currentXGC = gc;
}

/* for internal Vibrant use only */
extern  XFontStruct * Nlm_XLoadQueryFont (Display *d, Nlm_CharPtr fDescr, Nlm_Boolean critical)
{
  XFontStruct *f;

  if ((f = XLoadQueryFont(d, fDescr)) == NULL && critical)
  {
    fprintf (stderr, "Vibrant: Unable to load critical font <%s>\n", fDescr);
    exit (1);
  }

  return f;
}

#endif

static void Nlm_LoadFontData (Nlm_FonT f, Nlm_FonT nxt,
                              Nlm_CharPtr nam, Nlm_Int2 siz,
                              Nlm_Int2 scl, Nlm_Boolean bld,
                              Nlm_Boolean itl, Nlm_Boolean und,
                              Nlm_Int2 num, Nlm_Int2 stl,
                              Nlm_FontTool hdl, Nlm_FonT prnt)

{
  Nlm_FntPtr  fp;

  if (f != NULL) {
    fp = (Nlm_FntPtr) Nlm_HandLock (f);
    fp->next = nxt;
    Nlm_StringNCpy (fp->name, nam, sizeof (fp->name));
    fp->size = siz;
    fp->scale = scl;
    fp->bld = bld;
    fp->ital = itl;
    fp->undlin = und;
    fp->number = num;
    fp->style = stl;
    fp->handle = hdl;
    fp->print = prnt;
    Nlm_HandUnlock (f);
  }
}

static void Nlm_SetFontData (Nlm_FonT f, Nlm_FontData * fdata)

{
  Nlm_FntPtr  fp;

  if (f != NULL && fdata != NULL) {
    fp = (Nlm_FntPtr) Nlm_HandLock (f);
    *fp = *fdata;
    Nlm_HandUnlock (f);
  }
}

#ifdef DCLAP
extern
#else 
static
#endif
 void Nlm_GetFontData (Nlm_FonT f, Nlm_FontData * fdata)

{
  Nlm_FntPtr  fp;

  if (f != NULL && fdata != NULL) {
    fp = (Nlm_FntPtr) Nlm_HandLock (f);
    *fdata = *fp;
    Nlm_HandUnlock (f);
  }
}

#ifdef WIN_MAC
static void Nlm_ChooseColor (Nlm_Int4 color)

{
  ForeColor (color);
}

#endif

#ifdef WIN_MSWIN
static Nlm_Boolean Nlm_NotAStockPen (HPEN pen)

{
  return (Nlm_Boolean) (pen != hBlackPen && pen != hNullPen && pen != hWhitePen);
}

static Nlm_Boolean Nlm_NotAStockBrush (HBRUSH brush)

{
  return (Nlm_Boolean) (brush != hBlackBrush && brush != hDkGrayBrush &&
                        brush != hGrayBrush && brush != hHollowBrush &&
                        brush != hLtGrayBrush && hNullBrush &&
                        brush != hWhiteBrush);
}

static Nlm_Boolean Nlm_NotAStockFont (HFONT font)

{
  return (Nlm_Boolean) (font != hAnsiFixedFont && font != hAnsiVarFont &&
                        font != hDeviceDefaultFont && font != hOemFixedFont &&
                        font != hSystemFont && font != hSystemFixedFont);
}

static void Nlm_RecreateBrushes (void)

{
  Nlm_Int4  color;
  HBITMAP   hBitmap;
  HBRUSH    newBrush;
  HPEN      newPen;
  HBRUSH    oldBrush;
  HPEN      oldPen;

  if (Nlm_currentHDC != NULL) {
    color = GetTextColor (Nlm_currentHDC);
    newPen = CreatePen (currentPenStyle, currentPenWidth, color);
    if (newPen != NULL) {
      oldPen = SelectObject (Nlm_currentHDC, newPen);
      if (Nlm_NotAStockPen (oldPen)) {
        DeleteObject (oldPen);
      }
    }
    if (currentPattern == NULL) {
      currentPattern = whitePat;
    }
    hBitmap = CreateBitmap (8, 8, 1, 1, (LPSTR) currentPattern);
    newBrush = CreatePatternBrush (hBitmap);
    if (newBrush != NULL) {
      oldBrush = SelectObject (Nlm_currentHDC, newBrush);
      if (Nlm_NotAStockBrush (oldBrush)) {
        DeleteObject (oldBrush);
      }
    }
    DeleteObject (hBitmap);
  }
}

static void Nlm_SelectPattern (Nlm_Int2 style, Nlm_Int2 width, void * pat)

{
  currentPenStyle = style;
  currentPenWidth = width;
  currentPattern = pat;
  Nlm_RecreateBrushes ();
}



static void Nlm_ChooseColor (Nlm_Int4 color)

{
  if (Nlm_currentHDC != NULL) {
    SetTextColor (Nlm_currentHDC, color);
    Nlm_RecreateBrushes ();
  }
}

static Nlm_Boolean Nlm_GetTextMetrics (void)

{
  HDC          hDC;
  Nlm_Boolean  success;

  success = FALSE;
  if (Nlm_currentHDC != NULL) {
    success = (Nlm_Boolean) GetTextMetrics (Nlm_currentHDC, &textMetrics);
  } else {
    hDC = CreateIC ("DISPLAY", NULL, NULL, NULL);
    success = (Nlm_Boolean) GetTextMetrics (hDC, &textMetrics);
    DeleteDC (hDC);
  }
  return success;
}

static HBRUSH GetBackgroundBrush (HWND hwnd)

{
#if !defined(WIN32) && !defined(WIN32BOR)
  return (HBRUSH) GetClassWord (hwnd, GCW_HBRBACKGROUND);
#else
  return (HBRUSH) GetClassLong (hwnd, GCL_HBRBACKGROUND);
#endif
}
#endif

#ifdef WIN_X
static void Nlm_SelectPattern (void * pat)

{
  if (Nlm_currentXDisplay != NULL && Nlm_currentXWindow != 0) {
    if (currentPixmap != 0) {
      XFreePixmap (Nlm_currentXDisplay, currentPixmap);
    }
    currentPixmap = XCreateBitmapFromData (Nlm_currentXDisplay, Nlm_currentXWindow,
                                           (char *) pat, 8, 8);
    if (currentPixmap != 0 && Nlm_currentXGC != NULL) {
      XSetStipple (Nlm_currentXDisplay, Nlm_currentXGC, currentPixmap);
    }
  }
}

static void Nlm_ChooseColor (Nlm_Uint4 color)

{
  if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    XSetForeground (Nlm_currentXDisplay, Nlm_currentXGC, color);
    currentFgColor = color;
  }
}

static Nlm_Boolean Nlm_GetTextMetrics (void)

{
  Nlm_Boolean  success;

  success = FALSE;
  if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    if (currentFont != NULL) {
      fontInfo = *currentFont;
      success = TRUE;
    }
  }
  return success;
}
#endif


#ifdef DCLAP

extern void Nlm_SelectBackColor (Nlm_Uint1 red, Nlm_Uint1 green, Nlm_Uint1 blue)
{
#ifdef WIN_MAC
  RGBColor   color;
  Nlm_Uint2  bl;
  Nlm_Uint2  gn;
  Nlm_Uint2  rd;

  if (hasColorQD) {
    rd = (Nlm_Uint2) red;
    gn = (Nlm_Uint2) green;
    bl = (Nlm_Uint2) blue;
    color.red = rd << 8;
    color.green = gn << 8;
    color.blue = bl << 8;
    RGBBackColor (&color);
  	} 
  else if ((int) red + (int) green + (int) blue < 192)  
  	BackColor (blackColor);
  else  
  	BackColor (whiteColor);
 		 
#endif
#ifdef WIN_MSWIN
  COLORREF   color;
  Nlm_Uint2  bl;
  Nlm_Uint2  gn;
  Nlm_Uint2  rd;

  rd = (Nlm_Uint2) red;
  gn = (Nlm_Uint2) green;
  bl = (Nlm_Uint2) blue;
  color = RGB (rd, gn, bl);
  if (Nlm_currentHDC != NULL) {
    SetBkColor (Nlm_currentHDC, color);
    Nlm_RecreateBrushes ();
  	}
#endif
#ifdef WIN_X
  Colormap   colormap;
  Nlm_Uint2  bl;
  Nlm_Uint2  gn;
  Nlm_Uint2  rd;
  XColor     xcolor;

  if (Nlm_currentXDisplay == NULL || Nlm_currentXGC == NULL) 
  	return;
  else if (hasColor) {
    rd = (Nlm_Uint2) red;
    gn = (Nlm_Uint2) green;
    bl = (Nlm_Uint2) blue;
    xcolor.red = rd << 8;
    xcolor.green = gn << 8;
    xcolor.blue = bl << 8;
    if (Nlm_currentXDisplay != NULL) {
      colormap = DefaultColormap (Nlm_currentXDisplay, Nlm_currentXScreen);
      if (XAllocColor (Nlm_currentXDisplay, colormap, &xcolor) == 0) 
        xcolor.pixel = WhitePixel (Nlm_currentXDisplay, Nlm_currentXScreen);
	    currentBkColor = xcolor.pixel;
    	}
  	} 
  else if ((int) red + (int) green + (int) blue < 192) 
    currentBkColor = blackColor;
  else 
    currentBkColor = whiteColor;
	XSetBackground (Nlm_currentXDisplay, Nlm_currentXGC, currentBkColor);
#endif
}

#endif /* DCLAP */



extern void Nlm_ResetDrawingTools (void)

{
#ifdef WIN_MAC
  PenNormal ();
  PenMode (patCopy);
  TextMode (srcOr);
  if (hasColorQD) {
    RGBForeColor (&Nlm_RGBforeColor);
    RGBBackColor (&Nlm_RGBbackColor);
  } else {
    ForeColor (blackColor);
    BackColor (whiteColor);
  }
#endif
#ifdef WIN_MSWIN
  HBRUSH  oldBrush;
  HFONT   oldFont;
  HPEN    oldPen;

  if (Nlm_currentHDC != NULL) {
    SetROP2 (Nlm_currentHDC, R2_COPYPEN);
    oldPen = SelectObject (Nlm_currentHDC, GetStockObject (BLACK_PEN));
    oldBrush = SelectObject (Nlm_currentHDC, GetStockObject (BLACK_BRUSH));
    oldFont = SelectObject (Nlm_currentHDC, GetStockObject (SYSTEM_FONT));
    SetTextColor (Nlm_currentHDC, GetSysColor (COLOR_WINDOWTEXT));
    SetBkColor (Nlm_currentHDC, GetSysColor (COLOR_WINDOW));
    if (Nlm_NotAStockPen (oldPen)) {
      DeleteObject (oldPen);
    }
    if (Nlm_NotAStockBrush (oldBrush)) {
      DeleteObject (oldBrush);
    }
    Nlm_SelectPattern (PS_SOLID, 1, blackPat);
  }
#endif
#ifdef WIN_X
  if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    currentMode = COPY_MODE;
    currentFunction = GXcopy;
    currentFillStyle = FillOpaqueStippled;
    XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, currentFunction);
    XSetFillStyle (Nlm_currentXDisplay, Nlm_currentXGC, currentFillStyle);
    if (hasColor) {
      XSetBackground (Nlm_currentXDisplay, Nlm_currentXGC, Nlm_XbackColor);
      XSetForeground (Nlm_currentXDisplay, Nlm_currentXGC, Nlm_XforeColor);
      currentBkColor = Nlm_XbackColor;
      currentFgColor = Nlm_XforeColor;
    } else {
      XSetBackground (Nlm_currentXDisplay, Nlm_currentXGC, whiteColor);
      XSetForeground (Nlm_currentXDisplay, Nlm_currentXGC, blackColor);
      currentBkColor = whiteColor;
      currentFgColor = blackColor;
    }
    XSetLineAttributes (Nlm_currentXDisplay, Nlm_currentXGC,
                        1, LineSolid, CapButt, JoinMiter);
    Nlm_SelectPattern (blackPat);
  }
#endif
  currentMode = COPY_MODE;
}

static void Nlm_PointToolToPoinT (Nlm_PointTool src, Nlm_PointPtr dst)

{
  if (dst != NULL) {
#ifdef WIN_MAC
    dst->x = src.h;
    dst->y = src.v;
#endif
#ifdef WIN_MSWIN
    dst->x = (Nlm_Int2) src.x;
    dst->y = (Nlm_Int2) src.y;
#endif
#ifdef WIN_X
    dst->x = src.x;
    dst->y = src.y;
#endif
  }
}

static void Nlm_PoinTToPointTool (Nlm_PoinT src, Nlm_PointTool PNTR dst)

{
  if (dst != NULL) {
#ifdef WIN_MAC
    dst->h = src.x;
    dst->v = src.y;
#endif
#ifdef WIN_MSWIN
    dst->x = src.x;
    dst->y = src.y;
#endif
#ifdef WIN_X
    dst->x = src.x;
    dst->y = src.y;
#endif
  }
}

static void Nlm_RectToolToRecT (Nlm_RectTool PNTR src, Nlm_RectPtr dst)

{
  if (dst != NULL && src != NULL) {
#ifdef WIN_MAC
    dst->left = src->left;
    dst->top = src->top;
    dst->right = src->right;
    dst->bottom = src->bottom;
#endif
#ifdef WIN_MSWIN
    dst->left = (Nlm_Int2) src->left;
    dst->top = (Nlm_Int2) src->top;
    dst->right = (Nlm_Int2) src->right;
    dst->bottom = (Nlm_Int2) src->bottom;
#endif
#ifdef WIN_X
    dst->left = src->x;
    dst->top = src->y;
    dst->right = src->x + src->width;
    dst->bottom = src->y + src->height;
#endif
  }
}

static void Nlm_RecTToRectTool (Nlm_RectPtr src, Nlm_RectTool PNTR dst)

{
  if (dst != NULL && src != NULL) {
#ifdef WIN_MAC
    dst->left = MIN (src->left, src->right);
    dst->top = MIN (src->top, src->bottom);
    dst->right = MAX (src->left, src->right);
    dst->bottom = MAX (src->top, src->bottom);
#endif
#ifdef WIN_MSWIN
    dst->left = MIN (src->left, src->right);
    dst->top = MIN (src->top, src->bottom);
    dst->right = MAX (src->left, src->right);
    dst->bottom = MAX (src->top, src->bottom);
#endif
#ifdef WIN_X
    dst->x = MIN (src->left, src->right);
    dst->y = MIN (src->top, src->bottom);
    dst->width = ABS (src->right - src->left);
    dst->height = ABS (src->bottom - src->top);
#endif
  }
}

static void Nlm_LocalToGlobal (Nlm_PointPtr pt)

{
#ifdef WIN_MAC
  Nlm_PointTool  ptool;

  if (pt != NULL) {
    Nlm_PoinTToPointTool (*pt, &ptool);
    LocalToGlobal (&ptool);
    Nlm_PointToolToPoinT (ptool, pt);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_PointTool  ptool;

  if (pt != NULL && Nlm_currentHWnd != NULL) {
    Nlm_PoinTToPointTool (*pt, &ptool);
    ClientToScreen (Nlm_currentHWnd, &ptool);
    Nlm_PointToolToPoinT (ptool, pt);
  }
#endif
#ifdef WIN_X
#endif
}

static void Nlm_GlobalToLocal (Nlm_PointPtr pt)

{
#ifdef WIN_MAC
  Nlm_PointTool  ptool;

  if (pt != NULL) {
    Nlm_PoinTToPointTool (*pt, &ptool);
    GlobalToLocal (&ptool);
    Nlm_PointToolToPoinT (ptool, pt);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_PointTool  ptool;

  if (pt != NULL && Nlm_currentHWnd != NULL) {
    Nlm_PoinTToPointTool (*pt, &ptool);
    ScreenToClient (Nlm_currentHWnd, &ptool);
    Nlm_PointToolToPoinT (ptool, pt);
  }
#endif
#ifdef WIN_X
#endif
}

#ifdef OS_MAC
static void Nlm_CtoPstr (Nlm_CharPtr str)

{
#if defined(COMP_MPW) || defined(COMP_CODEWAR)
    c2pstr ((char *) str);
#else
#ifdef COMP_THINKC
    CtoPstr ((char *) str);
#endif
#endif
}

static void Nlm_PtoCstr (Nlm_CharPtr str)

{
#if defined(COMP_MPW) || defined(COMP_CODEWAR)
  p2cstr ((StringPtr) str);
#else
#ifdef COMP_THINKC
  PtoCstr ((StringPtr) str);
#endif
#endif
}
#endif

extern void Nlm_CopyMode (void)

{
#ifdef WIN_MAC
  PenMode (patCopy);
#ifdef DCLAP
  TextMode (srcCopy);
#else
  TextMode (srcOr);
#endif
#endif
#ifdef WIN_MSWIN
  if (Nlm_currentHDC != NULL) {
    SetROP2 (Nlm_currentHDC, R2_COPYPEN);
  }
#endif
#ifdef WIN_X
  currentFunction = GXcopy;
  currentFillStyle = FillOpaqueStippled;
  if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, currentFunction);
    XSetFillStyle (Nlm_currentXDisplay, Nlm_currentXGC, currentFillStyle);
  }
#endif
  currentMode = COPY_MODE;
}

extern void Nlm_MergeMode (void)

{
#ifdef WIN_MAC
  PenMode (patOr);
  TextMode (srcOr);
#endif
#ifdef WIN_MSWIN
  if (Nlm_currentHDC != NULL) {
    SetROP2 (Nlm_currentHDC, R2_MASKPEN);
  }
#endif
#ifdef WIN_X
  if (hasColor) {
    currentFunction = GXand;
  } else {
    currentFunction = GXor;
  }
  currentFillStyle = FillStippled;
  if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, currentFunction);
    XSetFillStyle (Nlm_currentXDisplay, Nlm_currentXGC, currentFillStyle);
  }
#endif
  currentMode = MERGE_MODE;
}

extern void Nlm_InvertMode (void)

{
#ifdef WIN_MAC
  PenMode (patXor);
  TextMode (srcXor);
#endif
#ifdef WIN_MSWIN
  if (Nlm_currentHDC != NULL) {
    SetROP2 (Nlm_currentHDC, R2_NOTXORPEN);
  }
#endif
#ifdef WIN_X
#ifndef no_DCLAP
  if (hasColor) {
    currentFunction = GXequiv;
  } else
#endif 
  {
    currentFunction = GXinvert;
  }
  currentFillStyle = FillStippled;
  if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, currentFunction);
    XSetFillStyle (Nlm_currentXDisplay, Nlm_currentXGC, currentFillStyle);
  }
#endif
  currentMode = INVERT_MODE;
}

extern void Nlm_EraseMode (void)


{
#ifdef WIN_MAC
  PenMode (patBic);
  TextMode (srcBic);
#endif
#ifdef WIN_MSWIN
  if (Nlm_currentHDC != NULL) {
    SetROP2 (Nlm_currentHDC, R2_MERGENOTPEN);
  }
#endif
#ifdef WIN_X
  if (hasColor) {
    currentFunction = GXorInverted;
  } else {
    currentFunction = GXand;
  }
  currentFillStyle = FillStippled;
  if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, currentFunction);
    XSetFillStyle (Nlm_currentXDisplay, Nlm_currentXGC, currentFillStyle);
  }
#endif
  currentMode = ERASE_MODE;
}

extern void Nlm_Black (void)

{
#ifdef WIN_MAC
  if (hasColorQD) {
    Nlm_SelectColor (0, 0, 0);
  } else {
    Nlm_ChooseColor (blackColor);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_ChooseColor (blackColor);
#endif
#ifdef WIN_X
  Nlm_ChooseColor (blackColor);
#endif
}

extern void Nlm_Red (void)

{
#ifdef WIN_MAC
  if (hasColorQD) {
    Nlm_SelectColor (255, 0, 0);
  } else {
    Nlm_ChooseColor (redColor);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_ChooseColor (redColor);
#endif
#ifdef WIN_X
  Nlm_ChooseColor (redColor);
#endif
}

extern void Nlm_Green (void)

{
#ifdef WIN_MAC
  if (hasColorQD) {
    Nlm_SelectColor (0, 255, 0);
  } else {
    Nlm_ChooseColor (greenColor);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_ChooseColor (greenColor);
#endif
#ifdef WIN_X
  Nlm_ChooseColor (greenColor);
#endif
}

extern void Nlm_Blue (void)

{
#ifdef WIN_MAC
  if (hasColorQD) {
    Nlm_SelectColor (0, 0, 255);
  } else {
    Nlm_ChooseColor (blueColor);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_ChooseColor (blueColor);
#endif
#ifdef WIN_X
  Nlm_ChooseColor (blueColor);
#endif
}

extern void Nlm_Cyan (void)

{
#ifdef WIN_MAC
  if (hasColorQD) {
    Nlm_SelectColor (0, 255, 255);
  } else {
    Nlm_ChooseColor (cyanColor);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_ChooseColor (cyanColor);
#endif
#ifdef WIN_X
  Nlm_ChooseColor (cyanColor);
#endif
}

extern void Nlm_Magenta (void)

{
#ifdef WIN_MAC
  if (hasColorQD) {
    Nlm_SelectColor (255, 0, 255);
  } else {
    Nlm_ChooseColor (magentaColor);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_ChooseColor (magentaColor);
#endif
#ifdef WIN_X
  Nlm_ChooseColor (magentaColor);
#endif
}

extern void Nlm_Yellow (void)

{
#ifdef WIN_MAC
  if (hasColorQD) {
    Nlm_SelectColor (255, 255, 0);
  } else {
    Nlm_ChooseColor (yellowColor);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_ChooseColor (yellowColor);
#endif
#ifdef WIN_X
  Nlm_ChooseColor (yellowColor);
#endif
}

extern void Nlm_White (void)

{
#ifdef WIN_MAC
  if (hasColorQD) {
    Nlm_SelectColor (255, 255, 255);
  } else {
    Nlm_ChooseColor (whiteColor);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_ChooseColor (whiteColor);
#endif
#ifdef WIN_X
  Nlm_ChooseColor (whiteColor);
#endif
}

extern void Nlm_Gray (void)

{
  Nlm_SelectColor (127, 127, 127);
}

extern void Nlm_LtGray (void)

{
  Nlm_SelectColor (191, 191, 191);
}

extern void Nlm_DkGray (void)

{
  Nlm_SelectColor (63, 63, 63);
}

extern void Nlm_SelectColor (Nlm_Uint1 red, Nlm_Uint1 green, Nlm_Uint1 blue)

{
#ifdef WIN_MAC
  RGBColor   color;
  Nlm_Uint2  bl;
  Nlm_Uint2  gn;
  Nlm_Uint2  rd;

  if (hasColorQD) {
    rd = (Nlm_Uint2) red;
    gn = (Nlm_Uint2) green;
    bl = (Nlm_Uint2) blue;
    color.red = rd << 8;
    color.green = gn << 8;
    color.blue = bl << 8;
    RGBForeColor (&color);
  } else if ((int) red + (int) green + (int) blue < 192) {
    Nlm_ChooseColor (blackColor);
  } else {
    Nlm_ChooseColor (whiteColor);
  }
#endif
#ifdef WIN_MSWIN
  COLORREF   color;
  Nlm_Uint2  bl;
  Nlm_Uint2  gn;
  Nlm_Uint2  rd;

  rd = (Nlm_Uint2) red;
  gn = (Nlm_Uint2) green;
  bl = (Nlm_Uint2) blue;
  color = RGB (rd, gn, bl);
  Nlm_ChooseColor (color);
#endif
#ifdef WIN_X
  Colormap   colormap;
  Nlm_Uint2  bl;
  Nlm_Uint2  gn;
  Nlm_Uint2  rd;
  XColor     xcolor;

  if (hasColor) {
    rd = (Nlm_Uint2) red;
    gn = (Nlm_Uint2) green;
    bl = (Nlm_Uint2) blue;
    xcolor.red = rd << 8;
    xcolor.green = gn << 8;
    xcolor.blue = bl << 8;
    if (Nlm_currentXDisplay != NULL) {
      colormap = DefaultColormap (Nlm_currentXDisplay, Nlm_currentXScreen);
      if (XAllocColor (Nlm_currentXDisplay, colormap, &xcolor) == 0) {
        xcolor.pixel = WhitePixel (Nlm_currentXDisplay, Nlm_currentXScreen);
      }
      Nlm_ChooseColor (xcolor.pixel);
    }
  } else if ((int) red + (int) green + (int) blue < 192) {
    Nlm_ChooseColor (blackColor);
  } else {
    Nlm_ChooseColor (whiteColor);
  }
#endif
}

extern Nlm_Uint4 Nlm_GetColor (void)

{
#ifdef WIN_MAC
  Nlm_Uint1  colors [4];
  RGBColor   foreColor;
  GrafPtr    port;
  Nlm_Int4   fgColor;

  fgColor = 0;
  if (hasColorQD) {
    GetForeColor (&foreColor);
    colors [0] = 0;
    colors [1] = (Nlm_Uint1) (foreColor.red >> 8);
    colors [2] = (Nlm_Uint1) (foreColor.green >> 8);
    colors [3] = (Nlm_Uint1) (foreColor.blue >> 8);
    fgColor = *((Nlm_Int4Ptr) colors);
  } else {
    GetPort (&port);
    if (port != NULL) {
      fgColor = port->fgColor;
    }
  }
  return (Nlm_Uint4) fgColor;
#endif
#ifdef WIN_MSWIN
  Nlm_Int4  fgColor;

  fgColor = 0;
  if (Nlm_currentHDC != NULL) {
    fgColor = GetTextColor (Nlm_currentHDC);
  }
  return (Nlm_Uint4) fgColor;
#endif
#ifdef WIN_X
  Nlm_Uint4  fgColor;

  fgColor = currentFgColor;
  return (Nlm_Uint4) fgColor;
#endif
}

extern void Nlm_SetColor (Nlm_Uint4 color)

{
#ifdef WIN_MAC
  Nlm_Uint2  bl;
  Nlm_Uint2  gn;
  Nlm_Uint2  rd;
  Nlm_Uint1  colors [4];
  RGBColor   foreColor;
  GrafPtr    port;

  if (hasColorQD) {
    *((Nlm_Int4Ptr) colors) = color;
    rd = (Nlm_Uint2) colors [1];
    gn = (Nlm_Uint2) colors [2];
    bl = (Nlm_Uint2) colors [3];
    foreColor.red = rd << 8;
    foreColor.green = gn << 8;
    foreColor.blue = bl << 8;
    RGBForeColor (&foreColor);
  } else {
    GetPort (&port);
    if (port != NULL) {
      ForeColor ((Nlm_Int4) color);
    }
  }
#endif
#ifdef WIN_MSWIN
  if (Nlm_currentHDC != NULL) {
    SetTextColor (Nlm_currentHDC, (Nlm_Int4) color);
    Nlm_RecreateBrushes ();
  }
#endif
#ifdef WIN_X
  XSetForeground (Nlm_currentXDisplay, Nlm_currentXGC, color);
  currentFgColor = color;
#endif
}

extern void Nlm_InvertColors (void)

{
#ifdef WIN_MAC
  RGBColor  backColor;
  RGBColor  foreColor;
  GrafPtr   port;
  Nlm_Int4  bkColor;
  Nlm_Int4  fgColor;

  if (hasColorQD) {
    GetForeColor (&foreColor);
    GetBackColor (&backColor);
    RGBForeColor (&backColor);
    RGBBackColor (&foreColor);
  } else {
    GetPort (&port);
    if (port != NULL) {
      fgColor = port->fgColor;
      bkColor = port->bkColor;
      ForeColor (bkColor);
      BackColor (fgColor);
    }
  }
#endif
#ifdef WIN_MSWIN
  Nlm_Int4  bkColor;
  Nlm_Int4  fgColor;

  if (Nlm_currentHDC != NULL) {
    fgColor = GetTextColor (Nlm_currentHDC);
    bkColor = GetBkColor (Nlm_currentHDC);
    SetTextColor (Nlm_currentHDC, bkColor);
    SetBkColor (Nlm_currentHDC, fgColor);
    Nlm_RecreateBrushes ();
  }
#endif
#ifdef WIN_X
  Nlm_Uint4  newBkColor;
  Nlm_Uint4  newFgColor;

  newBkColor = currentFgColor;
  newFgColor = currentBkColor;
  XSetBackground (Nlm_currentXDisplay, Nlm_currentXGC, newBkColor);
  XSetForeground (Nlm_currentXDisplay, Nlm_currentXGC, newFgColor);
  currentBkColor = newBkColor;
  currentFgColor = newFgColor;
#endif
}


#ifdef DCLAP
extern void Nlm_PenPattern(void *pat)
{
#ifdef WIN_MAC
  PenPat ((ConstPatternParam) pat); 
#endif
#ifdef WIN_MSWIN
  Nlm_SelectPattern (PS_SOLID, 1, pat);
#endif
#ifdef WIN_X
  Nlm_SelectPattern (pat);
#endif
}
#endif


extern void Nlm_Solid (void)

{
#ifdef WIN_MAC
  PenPat ((ConstPatternParam) blackPat); 
#endif
#ifdef WIN_MSWIN
  Nlm_SelectPattern (PS_SOLID, 1, blackPat);
#endif
#ifdef WIN_X
  XGCValues  values;

  Nlm_SelectPattern (blackPat);
  if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    values.line_style = LineSolid;
    XChangeGC (Nlm_currentXDisplay, Nlm_currentXGC, GCLineStyle, &values);
  }
#endif
}

extern void Nlm_Dark (void)

{
#ifdef WIN_MAC
  PenPat ((ConstPatternParam) dkGrayPat);
#endif
#ifdef WIN_MSWIN
  Nlm_SelectPattern (PS_SOLID, 1, dkGrayPat);
#endif
#ifdef WIN_X
  Nlm_SelectPattern (dkGrayPat);
#endif
}

extern void Nlm_Medium (void)

{
#ifdef WIN_MAC
  PenPat ((ConstPatternParam) grayPat);
#endif
#ifdef WIN_MSWIN
  Nlm_SelectPattern (PS_SOLID, 1, grayPat);
#endif
#ifdef WIN_X
  Nlm_SelectPattern (grayPat);
#endif
}

extern void Nlm_Light (void)

{
#ifdef WIN_MAC
  PenPat ((ConstPatternParam) ltGrayPat);
#endif
#ifdef WIN_MSWIN
  Nlm_SelectPattern (PS_SOLID, 1, ltGrayPat);
#endif
#ifdef WIN_X
  Nlm_SelectPattern (ltGrayPat);
#endif
}

extern void Nlm_Empty (void)


{
#ifdef WIN_MAC
  PenPat ((ConstPatternParam) whitePat);
#endif
#ifdef WIN_MSWIN
  Nlm_SelectPattern (PS_SOLID, 1, whitePat);
#endif
#ifdef WIN_X
  Nlm_SelectPattern (whitePat);
#endif
}

extern void Nlm_Dotted (void)

{
#ifdef WIN_MAC
  PenPat ((ConstPatternParam) dotPat);
#endif
#ifdef WIN_MSWIN
  Nlm_SelectPattern (PS_DOT, 1, dotPat);
#endif
#ifdef WIN_X
  XGCValues  values;

  if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    values.line_style = LineOnOffDash;
    XChangeGC (Nlm_currentXDisplay, Nlm_currentXGC, GCLineStyle, &values);
  }
#endif
}

extern void Nlm_Dashed (void)

{
#ifdef WIN_MAC
  PenPat ((ConstPatternParam) dashPat);
#endif
#ifdef WIN_MSWIN
  Nlm_SelectPattern (PS_DASH, 1, dashPat);
#endif
#ifdef WIN_X
  XGCValues  values;

  if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    values.line_style = LineOnOffDash;
    XChangeGC (Nlm_currentXDisplay, Nlm_currentXGC, GCLineStyle, &values);
  }
#endif
}

extern void Nlm_WidePen (Nlm_Int2 width)


{
#ifdef WIN_MAC
  PenPat ((ConstPatternParam) blackPat);
  PenSize (width, width);
#endif
#ifdef WIN_MSWIN
  Nlm_SelectPattern (PS_SOLID, width, blackPat);
#endif
#ifdef WIN_X
  XGCValues  values;

  if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    values.line_width = width;
    XChangeGC (Nlm_currentXDisplay, Nlm_currentXGC, GCLineWidth, &values);
  }
#endif
}

static Nlm_FonT Nlm_CreateNewFont (Nlm_CharPtr name, Nlm_Int2 size,
                                   Nlm_Boolean bld, Nlm_Boolean itlc,
                                   Nlm_Boolean undrln, Nlm_CharPtr fmly)

{
  Nlm_Byte      charSet;
  Nlm_FontTool  hdl;
  Nlm_Int2      num;
  Nlm_FonT      rsult;
  Nlm_Int2      scale;
  Nlm_Int2      styl;
  Nlm_Int2      weight;
#ifdef WIN_MAC
  Nlm_Char      temp [256];
#endif
#ifdef WIN_MSWIN
  BYTE          family;
  HDC           hDC;
  POINT         ptool;
#endif
#ifdef WIN_X
  Nlm_Char      ptsize [16];
  Nlm_Char      temp [256];
#endif

  rsult = (Nlm_FonT) Nlm_HandNew (sizeof (Nlm_FontRec));
  if (rsult != NULL) {
    scale = size;
#ifdef WIN_MAC
    hdl = NULL;
    num = 0;
    if (name != NULL) {
      Nlm_StringNCpy (temp, name, sizeof (temp));
      Nlm_CtoPstr (temp);
      GetFNum ((StringPtr) temp, &num);
#ifndef DCLAP
      if (num == 0 && fmly != NULL && fmly [0] != '\0') {
        if (Nlm_StringICmp (fmly, "Roman") == 0) {
          num = 0; /* chicago */
        } else if (Nlm_StringICmp (fmly, "Swiss") == 0) {
          num = 3; /* geneva */
        } else if (Nlm_StringICmp (fmly, "Modern") == 0) {
          num = 4; /* monaco */
        } else if (Nlm_StringICmp (fmly, "Script") == 0) {
          num = 0; /* chicago  */
        } else if (Nlm_StringICmp (fmly, "Decorative") == 0) {
          num = 0; /* chicago  */
        }
      }
#endif
    }
#ifdef DCLAP
	if (num == 0 && fmly != NULL && fmly [0] != '\0') {
      if (Nlm_StringICmp (fmly, "Roman") == 0)  
          num = 20; /* times roman */
      else if (Nlm_StringICmp (fmly, "Swiss") == 0) 
          num = 21; /* helvetica */
      else if (Nlm_StringICmp (fmly, "Modern") == 0) 
          num = 22;  /* courier */
      else if (Nlm_StringICmp (fmly, "Script") == 0) 
          num = 20;	 /* 18 == Zapf Chancery, !? or times */
      else if (Nlm_StringICmp (fmly, "Decorative") == 0) 
          num = 20; /* times roman */
      else
       		num = 3; /* geneva == builtin == appl font */       	
      }
#endif
   
    styl = 0;
    if (bld) {
      styl += bold;
    }
    if (itlc) {
      styl += italic;
    }
    if (undrln) {
      styl += underline;
    }
#endif
#ifdef WIN_MSWIN
    hdl = NULL;
    num = 0;
    styl = 0;
    if (bld) {
      weight = FW_BOLD;
    } else {
      weight = FW_NORMAL;
    }
    if (Nlm_StringICmp (name, "Symbol") == 0) {
      charSet = SYMBOL_CHARSET;
    } else if (Nlm_StringICmp (name, "Terminal") == 0) {
      charSet = OEM_CHARSET;
    } else {
      charSet = ANSI_CHARSET;
    }
    family = FF_DONTCARE;
    if (fmly != NULL && fmly [0] != '\0') {
      if (Nlm_StringICmp (fmly, "Roman") == 0) {
        family = FF_ROMAN;
      } else if (Nlm_StringICmp (fmly, "Swiss") == 0) {
        family = FF_SWISS;
      } else if (Nlm_StringICmp (fmly, "Modern") == 0) {
        family = FF_MODERN;
      } else if (Nlm_StringICmp (fmly, "Script") == 0) {
        family = FF_SCRIPT;
      } else if (Nlm_StringICmp (fmly, "Decorative") == 0) {
        family = FF_DECORATIVE;
      }
    }
    hDC = CreateCompatibleDC (NULL);
    SetMapMode (hDC, MM_ANISOTROPIC);
    SetWindowExtEx (hDC, 1440, 1440, NULL);
    SetViewportExtEx (hDC, GetDeviceCaps (hDC, LOGPIXELSX),
                      GetDeviceCaps (hDC, LOGPIXELSY), NULL);
#ifdef DCLAP
		DeleteDC (hDC);
		hdl = CreateFont (-size, 0, 0, 0, weight, itlc, undrln, FALSE,
											charSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY, (BYTE) (DEFAULT_PITCH | family), name);
#else
		ptool.x = 0;
    ptool.y = 20 * size;
    LPtoDP (hDC, &ptool, 1);
    DeleteDC (hDC);
    scale = (Nlm_Int2) ptool.y;
    hdl = CreateFont (scale, 0, 0, 0, weight, itlc, undrln, FALSE,
                      charSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                      DEFAULT_QUALITY, (BYTE) (DEFAULT_PITCH | family), name);
    /*
    hdl = CreateFont (-size, 0, 0, 0, weight, itlc, undrln, FALSE,
                      charSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                      DEFAULT_QUALITY, (BYTE) (DEFAULT_PITCH | family), name);
    */
#endif

#endif
#ifdef WIN_X
    num = 0;
    styl = 0;
    hdl = NULL;
    if (name != NULL && Nlm_currentXDisplay != NULL) {
      StringCpy (temp, "-*-");
      StringCat (temp, name);
      StringCat (temp, "-");
			/* dgg ! some use "demi" and "light" ! */
      if (bld) {
        StringCat (temp, "bold-");
      } else {
        StringCat (temp, "medium-");
      }
			/* dgg ! some use "o", some use "i" for italic !! */
      if (itlc) {
        StringCat (temp, "o-");
      } else {
        StringCat (temp, "r-");
      }
      StringCat (temp, "*--");
      sprintf (ptsize, "%d", (int) size);
      StringCat (temp, ptsize);
      StringCat (temp, "-*");
      hdl = Nlm_XLoadQueryFont (Nlm_currentXDisplay, temp, FALSE);
      if (hdl == NULL) {

#ifdef DCLAP
				int	size2, ipoints, ipixels;
				char *sbold, *sitalic;

				/* try scalable font ! */
				if (itlc) sitalic= "i"; else sitalic= "r";   /* or "o" and "r" */
				if (bld) sbold= "bold"; else sbold= "medium"; /* or "demi" and "light" */
				ipoints= 0;	/* to select scalable font */
				ipixels= size * 10; /* ?? */
			  sprintf(temp, "-*-%s-%s-%s-*--%d-%d-*",name,sbold,sitalic,ipoints,ipixels);
        hdl = Nlm_XLoadQueryFont (Nlm_currentXDisplay, temp, FALSE);
        if (hdl) goto fontokay;
				if (itlc) {
					 sitalic= "o";
		  		 sprintf(temp, "-*-%s-%s-%s-*--%d-%d-*",name,sbold,sitalic,ipoints,ipixels);
           hdl = Nlm_XLoadQueryFont (Nlm_currentXDisplay, temp, FALSE);
           if (hdl) goto fontokay;
					 }
				if (bld) {
					 sbold= "demi";
		  		 sprintf(temp, "-*-%s-%s-%s-*--%d-%d-*",name,sbold,sitalic,ipoints,ipixels);
           hdl = Nlm_XLoadQueryFont (Nlm_currentXDisplay, temp, FALSE);
           if (hdl) goto fontokay;
					 }

				/* try it based on family... */
				if (fmly != NULL && fmly [0] != '\0') {
					ipoints= 0;	/* to select scalable font */
					ipixels= size * 10; /* ?? */
					if (itlc) sitalic= "i"; else sitalic= "r";   /* or "o" and "r" */
					if (bld) sbold= "bold"; else sbold= "medium"; /* or "demi" and "light" */
		      if (Nlm_StringICmp (fmly, "Roman") == 0
		      	&& Nlm_StringICmp(name, "times") != 0)  
		          name= "times"; 
		      else if (Nlm_StringICmp (fmly, "Swiss") == 0
		      	&& Nlm_StringICmp(name, "helvetica") != 0)  
		          name= "helvetica"; 
		      else if (Nlm_StringICmp (fmly, "Modern") == 0
		      	&& Nlm_StringICmp(name, "courier") != 0)  
		          name= "courier"; 
		      else if (Nlm_StringICmp (fmly, "Script") == 0
		      	&& Nlm_StringICmp(name, "times") != 0)  
		          name= "times"; 
		      else if (Nlm_StringICmp (fmly, "Decorative") == 0
		      	&& Nlm_StringICmp(name, "times") != 0)  
		          name= "times"; 
		      else if (Nlm_StringICmp(name, "helvetica") != 0)  
		          name= "helvetica"; 
				  sprintf(temp, "-*-%s-%s-%s-*--%d-%d-*",name,sbold,sitalic,ipoints,ipixels);
	        hdl = Nlm_XLoadQueryFont (Nlm_currentXDisplay, temp, FALSE);
	        if (hdl) goto fontokay;
			    }

        /* try font sizes w/o other extra -- looks good */
        size2= 10 + size;
        size--; /* start 1 lower */
        do {
	        sprintf(temp,"-*-%s-*--%d-*",name, (int)size);
          hdl = Nlm_XLoadQueryFont (Nlm_currentXDisplay, temp, FALSE);
          if (hdl) goto fontokay;
          size ++;
        } while (size < size2);

#endif

        StringCpy (temp, "-*-");
        StringCat (temp, name);
        StringCat (temp, "-*");
        hdl = Nlm_XLoadQueryFont (Nlm_currentXDisplay, temp, FALSE);
        if (hdl == NULL) {
          StringCpy (temp, "-*-helvetica-bold-r-*--14-*");
          hdl = Nlm_XLoadQueryFont (Nlm_currentXDisplay, temp, TRUE);
        }
      }
    }
#ifdef DCLAP
fontokay:
#endif

#endif
    Nlm_LoadFontData (rsult, NULL, name, size, scale, bld,
                      itlc, undrln, num, styl, hdl, NULL);
  }
  return rsult;
}

static Nlm_FonT Nlm_FindFont (Nlm_CharPtr name, Nlm_Int2 size,
                              Nlm_Boolean bld, Nlm_Boolean itlc,
                              Nlm_Boolean undrln)

{
  Nlm_FontData  fdata;
  Nlm_FonT      p;
  Nlm_FonT      rsult;

  rsult = NULL;
  if (name != NULL && name [0] != '\0') {
    p = Nlm_fontList;
    while (p != NULL && rsult == NULL) {
      Nlm_GetFontData (p, &fdata);
      if ((Nlm_StringICmp (fdata.name, name) == 0) &&
         fdata.size == size && fdata.bld == bld &&
         fdata.ital == itlc && fdata.undlin == undrln) {
        rsult = p;
      }
      p = fdata.next;
    }
  }
  return rsult;
}

extern Nlm_FonT Nlm_GetFont (Nlm_CharPtr name, Nlm_Int2 size,
                             Nlm_Boolean bld, Nlm_Boolean itlc,
                             Nlm_Boolean undrln, Nlm_CharPtr family)

{
  Nlm_FontData  fdata;
  Nlm_FonT      rsult;

  rsult = NULL;
  if (name != NULL && name [0] != '\0') {
#ifdef DCLAP
#ifdef WIN_MSWIN
		/* need to do here -- MSWIN chokes if we try to CreateNewFont lots of times */
			if (Nlm_StringICmp (name, "Courier") == 0) {
				name= "Courier New";	  /* newCourier= TRUE; */			
				}
#endif
#endif
    rsult = Nlm_FindFont (name, size, bld, itlc, undrln);
    if (rsult == NULL) {
      rsult = Nlm_CreateNewFont (name, size, bld, itlc, undrln, family);
      Nlm_GetFontData (rsult, &fdata);
      fdata.next = Nlm_fontList;
      Nlm_SetFontData (rsult, &fdata);
      Nlm_fontList = rsult;
    }
  }
  return rsult;
}

static Nlm_FonT Nlm_ParseFontSpec (Nlm_CharPtr spec)

{
  Nlm_Boolean  bold;
  Nlm_CharPtr  fmly;
  Nlm_Boolean  ital;
  Nlm_Char     name [128];
  Nlm_CharPtr  p;
  Nlm_CharPtr  q;
  Nlm_CharPtr  r;
  Nlm_FonT     rsult;
  Nlm_Int2     size;
  Nlm_Char     temp [128];
  Nlm_Boolean  undr;
  int          val;

  rsult = NULL;
  if (spec != NULL && spec [0] != '\0') {
    fmly = NULL;
    bold = FALSE;
    ital = FALSE;
    undr = FALSE;
    Nlm_StringNCpy (name, spec, sizeof (name) - 1);
    p = Nlm_StringChr (name, ',');
    if (p != NULL) {
      *p = '\0';
      p++;
      q = Nlm_StringChr (p, ',');
      if (q != NULL) {
        *q = '\0';
        q++;
      }
      r = Nlm_StringChr (q, 'B');
      if (r == NULL) {
        r = Nlm_StringChr (q, 'b');
      }
      if (r != NULL) {
        bold = TRUE;
      }
      r = Nlm_StringChr (q, 'I');
      if (r == NULL) {
        r = Nlm_StringChr (q, 'i');
      }
      if (r != NULL) {
        ital = TRUE;
      }
      r = Nlm_StringChr (q, 'U');
      if (r == NULL) {
        r = Nlm_StringChr (q, 'u');
      }
      if (r != NULL) {
        undr = TRUE;
      }
      Nlm_StringNCpy (temp, p, sizeof (temp) - 1);
      if (sscanf (temp, "%d", &val) != EOF) {
        size = (Nlm_Int2) val;
        rsult = Nlm_GetFont (name, size, bold, ital, undr, fmly);
      }
    }
  }
  return rsult;
}

extern Nlm_FonT Nlm_ParseFont (Nlm_CharPtr spec)

{
  Nlm_Char     name [128];
  Nlm_CharPtr  p;
  Nlm_FonT     prtr;
  Nlm_CharPtr  q;
  Nlm_FonT     rsult;

  rsult = NULL;
  if (spec != NULL && spec [0] != '\0') {
    Nlm_StringNCpy (name, spec, sizeof (name) - 1);
    p = Nlm_StringChr (name, '|');
    if (p != NULL) {
      *p = '\0';
      p++;
      while (*p == ' ') {
        p++;
      }
      q = name;
      while (*q == ' ') {
        q++;
      }
      rsult = Nlm_ParseFontSpec (q);
      prtr = Nlm_ParseFontSpec (p);
      Nlm_AssignPrinterFont (rsult, prtr);
    } else {
      q = name;
      while (*q == ' ') {
        q++;
      }
      rsult = Nlm_ParseFontSpec (q);
    }
  }
  return rsult;
}

extern void Nlm_SelectFont (Nlm_FonT f)

{
  Nlm_FontData  fdata;

  if (f != NULL) {
    Nlm_GetFontData (f, &fdata);
    if (fdata.print != NULL && Nlm_nowPrinting) {
      f = fdata.print;
      Nlm_GetFontData (f, &fdata);
    }
#ifdef WIN_MAC
    TextFont (fdata.number);
    TextSize (fdata.size);
    TextFace (fdata.style);
#endif
#ifdef WIN_MSWIN
    if (Nlm_currentHDC != NULL) {
      SelectObject (Nlm_currentHDC, fdata.handle);
    }
#endif
#ifdef WIN_X
    if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
      if (fdata.handle != NULL) {
        XSetFont (Nlm_currentXDisplay, Nlm_currentXGC, fdata.handle->fid);
        currentFont = fdata.handle;
      }
    }
#endif
    Nlm_fontInUse = f;
  }
}

extern void Nlm_AssignPrinterFont (Nlm_FonT scrnFont, Nlm_FonT prtrFont)

{
  Nlm_FontData  fdata;

  if (scrnFont != NULL) {
    Nlm_GetFontData (scrnFont, &fdata);
    fdata.print = prtrFont;
    Nlm_SetFontData (scrnFont, &fdata);
  }
}

extern Nlm_Int2 Nlm_CharWidth (Nlm_Char ch)

{
#ifdef WIN_MAC
  return (CharWidth (ch));
#endif
#ifdef WIN_MSWIN
  Nlm_Char  str [4];

  str [0] = ch;
  str [1] = '\0';
  return (Nlm_TextWidth (str, 1));
#endif
#ifdef WIN_X
  Nlm_Char  str [4];

  str [0] = ch;
  str [1] = '\0';
  return (Nlm_TextWidth (str, 1));
#endif
}

extern Nlm_Int2 Nlm_StringWidth (Nlm_CharPtr text)

{
  return Nlm_TextWidth (text, Nlm_StringLen (text));
}

extern Nlm_Int2 Nlm_TextWidth (Nlm_CharPtr text, Nlm_sizeT len)

{
#ifdef WIN_MAC
  Nlm_Int2  rsult;

  rsult = 0;
  if (text != NULL && len > 0) {
    rsult = TextWidth (text, 0, (Nlm_Int2) len);
  }
  return rsult;
#endif
#ifdef WIN_MSWIN
  Nlm_Int2  rsult;
  HDC       hDC;
  SIZE      tag;

  rsult = 0;
  if (text != NULL && len > 0) {
    if (Nlm_currentHDC != NULL) {
      GetTextExtentPoint (Nlm_currentHDC, text, (Nlm_Int2) len, &tag);
    } else {
      hDC = CreateIC ("DISPLAY", NULL, NULL, NULL);
      GetTextExtentPoint (hDC, text, (Nlm_Int2) len, &tag);
      DeleteDC (hDC);
    }
    rsult = (Nlm_Int2) tag.cx;
  }
  return rsult;
#endif
#ifdef WIN_X
  Nlm_Int2  rsult;

  rsult = 0;
  if (text != NULL && len > 0 && Nlm_GetTextMetrics ()) {
    rsult = XTextWidth (&fontInfo, text, (int) len);
  }
  return rsult;
#endif
}

extern Nlm_Int2 Nlm_Ascent (void)

{
#ifdef WIN_MAC
  FontInfo  fontinfo;

  GetFontInfo (&fontinfo);
  return (fontinfo.ascent);
#endif
#ifdef WIN_MSWIN
  Nlm_Int2  rsult;

  rsult = 0;
  if (Nlm_GetTextMetrics ()) {
    rsult = (Nlm_Int2) textMetrics.tmAscent;
  }
  return rsult;
#endif
#ifdef WIN_X
  Nlm_Int2  rsult;

  rsult = 0;
  if (Nlm_GetTextMetrics ()) {
    rsult = fontInfo.ascent;
  }
  return rsult;
#endif
}

extern Nlm_Int2 Nlm_Descent (void)

{
#ifdef WIN_MAC
  FontInfo  fontinfo;

  GetFontInfo (&fontinfo);
  return (fontinfo.descent);
#endif
#ifdef WIN_MSWIN
  Nlm_Int2  rsult;

  rsult = 0;
  if (Nlm_GetTextMetrics ()) {
    rsult = (Nlm_Int2) textMetrics.tmDescent;
  }
  return rsult;
#endif
#ifdef WIN_X
  Nlm_Int2  rsult;

  rsult = 0;
  if (Nlm_GetTextMetrics ()) {
    rsult = fontInfo.descent;
  }
  return rsult;
#endif
}

extern Nlm_Int2 Nlm_Leading (void)

{
#ifdef WIN_MAC
  FontInfo  fontinfo;

  GetFontInfo (&fontinfo);
  return (fontinfo.leading);
#endif
#ifdef WIN_MSWIN
  Nlm_Int2  rsult;

  rsult = 0;
  if (Nlm_GetTextMetrics ()) {
    rsult = (Nlm_Int2) textMetrics.tmExternalLeading;
  }
  return rsult;
#endif
#ifdef WIN_X
  return 0;
#endif
}

extern Nlm_Int2 Nlm_FontHeight (void)

{
#ifdef WIN_MAC
  FontInfo  fontinfo;

  GetFontInfo (&fontinfo);
  return (fontinfo.ascent + fontinfo.descent);
#endif
#ifdef WIN_MSWIN
  Nlm_Int2  rsult;

  rsult = 0;
  if (Nlm_GetTextMetrics ()) {
    rsult = (Nlm_Int2) textMetrics.tmHeight;
  }
  return rsult;
#endif
#ifdef WIN_X
  Nlm_Int2  rsult;

  rsult = 0;
  if (Nlm_GetTextMetrics ()) {
    rsult = (fontInfo.ascent + fontInfo.descent);
  }
  return rsult;
#endif
}

extern Nlm_Int2 Nlm_LineHeight (void)

{
#ifdef WIN_MAC
  FontInfo  fontinfo;

  GetFontInfo (&fontinfo);
  return (fontinfo.ascent + fontinfo.descent + fontinfo.leading);
#endif
#ifdef WIN_MSWIN
  Nlm_Int2  rsult;

  rsult = 0;
  if (Nlm_GetTextMetrics ()) {
    rsult = (Nlm_Int2) (textMetrics.tmHeight + textMetrics.tmExternalLeading);
  }
  return rsult;
#endif
#ifdef WIN_X
  Nlm_Int2  rsult;

  rsult = 0;
  if (Nlm_GetTextMetrics ()) {
    rsult = (fontInfo.ascent + fontInfo.descent);
  }
  return rsult;
#endif
}

extern Nlm_Int2 Nlm_MaxCharWidth (void)

{
#ifdef WIN_MAC
  FontInfo  fontinfo;

  GetFontInfo (&fontinfo);
  return (fontinfo.widMax);
#endif
#ifdef WIN_MSWIN
  Nlm_Int2  rsult;

  rsult = 0;
  if (Nlm_GetTextMetrics ()) {
    rsult = (Nlm_Int2) textMetrics.tmMaxCharWidth;
  }
  return rsult;
#endif
#ifdef WIN_X
  Nlm_Int2  rsult;

  rsult = 0;
  if (Nlm_GetTextMetrics ()) {
    rsult = fontInfo.max_bounds.width;
  }
  return rsult;
#endif
}

extern void Nlm_SetPen (Nlm_PoinT pt)

{
#ifdef WIN_MAC
  MoveTo (pt.x, pt.y);
#endif
#ifdef WIN_MSWIN
  if (Nlm_currentHDC != NULL) {
    MoveToEx (Nlm_currentHDC, pt.x, pt.y, NULL);
  }
#endif
#ifdef WIN_X
  currentPoint = pt;
#endif
}

extern void Nlm_GetPen (Nlm_PointPtr pt)

{
#ifdef WIN_MAC
  Nlm_PointTool  ptool;

  if (pt != NULL) {
    GetPen (&ptool);
    Nlm_PointToolToPoinT (ptool, pt);
  }
#endif
#ifdef WIN_MSWIN
  POINT  pos;

  if (pt != NULL && Nlm_currentHDC != NULL) {
    GetCurrentPositionEx (Nlm_currentHDC, &pos);
    pt->x = (Nlm_Int2) pos.x;
    pt->y = (Nlm_Int2) pos.y;
  }
#endif
#ifdef WIN_X
  if (pt != NULL) {
    *pt = currentPoint;
  }
#endif
}

#ifdef DCLAP
extern void Nlm_TextOpaque()
{
#ifdef WIN_MAC
  /* TextMode (srcCopy); */
#endif
#ifdef WIN_MSWIN
  if (Nlm_currentHDC != NULL) SetBkMode (Nlm_currentHDC, OPAQUE);
#endif
#ifdef WIN_X
#endif
}

extern void Nlm_TextTransparent()
{
#ifdef WIN_MAC
  /* TextMode (srcOr); */
#endif
#ifdef WIN_MSWIN
  if (Nlm_currentHDC != NULL) SetBkMode (Nlm_currentHDC, TRANSPARENT);
#endif
#ifdef WIN_X
#endif
}

#endif


extern void Nlm_PaintChar (Nlm_Char ch)

{
#ifdef WIN_MAC
  DrawChar (ch);
#endif
#ifdef WIN_MSWIN
  Nlm_PoinT  pt;
  Nlm_Char   str [2];

  if (Nlm_currentHDC != NULL) {
    str [0] = ch;
    str [1] = '\0';
    Nlm_GetPen (&pt);
    TextOut (Nlm_currentHDC, pt.x, pt.y - Nlm_Ascent (), str, 1);
    pt.x += Nlm_CharWidth (ch);
    Nlm_MoveTo (pt.x, pt.y);
  }
#endif
#ifdef WIN_X
  Nlm_PoinT  pt;
  Nlm_Char   str [2];

  if (Nlm_currentXDisplay != NULL && Nlm_currentXWindow != 0 &&
      Nlm_currentXGC != NULL) {
    str [0] = ch;
    str [1] = '\0';
    Nlm_GetPen (&pt);
    XDrawString (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                 pt.x - Nlm_XOffset, pt.y - Nlm_YOffset, str, 1);
    pt.x += Nlm_CharWidth (ch);
    Nlm_MoveTo (pt.x, pt.y);
  }
#endif
}

extern void Nlm_PaintString (Nlm_CharPtr text)

{
#ifdef WIN_MAC
  Nlm_PoinT  pt;
  Nlm_Char   str [256];

  if (text != NULL) {
    Nlm_GetPen (&pt);
#ifdef DCLAP
   	if (*text) drawstring ( text);
#else
    if (Nlm_StringLen (text) > 0) {
      Nlm_StringNCpy (str, text, sizeof (str));
      Nlm_CtoPstr (str);
      DrawString ((StringPtr) str);
    }
#endif
    pt.x += Nlm_StringWidth (text);
    Nlm_MoveTo (pt.x, pt.y);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_Int2   len;
  Nlm_PoinT  pt;

  if (text != NULL && Nlm_currentHDC != NULL) {
    len = (Nlm_Int2) Nlm_StringLen (text);
    Nlm_GetPen (&pt);
    if (len > 0) {
      TextOut (Nlm_currentHDC, pt.x, pt.y - Nlm_Ascent (), text, len);
    }
    pt.x += Nlm_StringWidth (text);
    Nlm_MoveTo (pt.x, pt.y);
  }
#endif
#ifdef WIN_X
  Nlm_Int2   len;
  Nlm_PoinT  pt;

  if (text != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXWindow != 0 && Nlm_currentXGC != NULL) {
    len = (Nlm_Int2) Nlm_StringLen (text);
    Nlm_GetPen (&pt);
    if (len > 0) {
      XDrawString (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                   pt.x - Nlm_XOffset, pt.y - Nlm_YOffset, text, len);
    }
    pt.x += Nlm_StringWidth (text);
    Nlm_MoveTo (pt.x, pt.y);
  }
#endif
}

#ifdef VAR_ARGS
void CDECL Nlm_PaintText (format, va_alist)
  char *format;
  va_dcl
#else
void CDECL Nlm_PaintText (char *format, ...)
#endif

{
#ifdef WIN_MAC
  va_list    args;
  Nlm_PoinT  pt;
  Nlm_Char   str [256];

  if (format != NULL) {
#ifdef VAR_ARGS
    va_start(args);
#else
    va_start(args, format);
#endif
    vsprintf(str, format, args);
    va_end(args);
    Nlm_GetPen (&pt);
    if (Nlm_StringLen (str) > 0) {
      Nlm_CtoPstr (str);
      DrawString ((StringPtr) str);
    }
    pt.y += Nlm_LineHeight ();
    Nlm_MoveTo (pt.x, pt.y);
  }
#endif
#ifdef WIN_MSWIN
  va_list    args;
  Nlm_Int2   len;
  Nlm_PoinT  pt;
  Nlm_Char   str [256];

  if (format != NULL && Nlm_currentHDC != NULL) {
#ifdef VAR_ARGS
    va_start(args);
#else
    va_start(args, format);
#endif
    vsprintf(str, format, args);
    va_end(args);
    len = (Nlm_Int2) Nlm_StringLen (str);
    Nlm_GetPen (&pt);
    if (len > 0) {
      TextOut (Nlm_currentHDC, pt.x, pt.y - Nlm_Ascent (), str, len);
    }
    pt.y += Nlm_LineHeight ();
    Nlm_MoveTo (pt.x, pt.y);
  }
#endif
#ifdef WIN_X
  va_list    args;
  Nlm_Int2   len;
  Nlm_PoinT  pt;
  Nlm_Char   str [256];

  if (format != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXWindow != 0 && Nlm_currentXGC != NULL) {
#ifdef VAR_ARGS
    va_start(args);
#else
    va_start(args, format);
#endif
    vsprintf(str, format, args);
    va_end(args);
    len = (Nlm_Int2) Nlm_StringLen (str);
    Nlm_GetPen (&pt);
    if (len > 0) {
      XDrawString (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                   pt.x - Nlm_XOffset, pt.y - Nlm_YOffset, str, len);
    }
    pt.y += Nlm_LineHeight ();
    Nlm_MoveTo (pt.x, pt.y);
  }
#endif
}

extern void Nlm_DrawString (Nlm_RectPtr r, Nlm_CharPtr text,
                            Nlm_Char jst, Nlm_Boolean gray)

{
  Nlm_DrawText (r, text, Nlm_StringLen (text), jst, gray);
}

extern void Nlm_DrawText (Nlm_RectPtr r, Nlm_CharPtr text,
                          Nlm_sizeT len, Nlm_Char jst,
                          Nlm_Boolean gray)

{
#ifdef WIN_MAC
  Nlm_Int2      delta;
  Nlm_Int2      height;
  Nlm_Int2      just;
  Nlm_Int2      limit;
  PenState      pnState;
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_EraseRect (r);
    if (text != NULL && len > 0) {
      Nlm_RecTToRectTool (r, &rtool);
      limit = ABS (r->bottom - r->top);
      height = Nlm_LineHeight ();
      delta = limit - height;
      if (delta > 0) {
        rtool.top += delta / 2;
        rtool.bottom = rtool.top + height;
      }
      switch (jst) {
        case 'r':
          just = -1;
          break;
        case 'l':
          just = 0;
          break;
        case 'c':
          just = 1;
          break;
        default:
          just = 0;
          break;
      }
      TextBox (text, len, &rtool, just);
      if (gray) {
        GetPenState (&pnState);
        PenMode (patBic);
        PenPat ((ConstPatternParam) grayPat);
        PaintRect (&rtool);
        SetPenState (&pnState);
      }
    }
  }
#endif
#ifdef WIN_MSWIN
  Nlm_Int2      format;
  Nlm_Int4      oldcolor;
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentHDC != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    if (Nlm_currentHWnd != NULL) {
      Nlm_EraseRect (r);
    } else {
      FillRect (Nlm_currentHDC, &rtool, hWhiteBrush);
    }
    if (text != NULL && len > 0) {
      switch (jst) {
        case 'r':
          format = DT_RIGHT;
          break;
        case 'l':
          format = DT_LEFT;
          break;
        case 'c':
          format = DT_CENTER;
          break;
        default:
          format = DT_LEFT;
          break;
      }
      if (gray) {
        oldcolor = SetTextColor (Nlm_currentHDC, GetSysColor (COLOR_GRAYTEXT));
      }
      DrawText (Nlm_currentHDC, text, (Nlm_Int2) len, &rtool,
                format | DT_VCENTER | DT_NOPREFIX);
      if (gray) {
        SetTextColor (Nlm_currentHDC, oldcolor);
      }
    }
  }
#endif
#ifdef WIN_X
  Nlm_Int2   delta;
  Nlm_Int2   height;
  Nlm_Int2   limit;
  Pixmap     pix;
  Nlm_PoinT  pt;
  Nlm_Int2   width;

  if (r != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXWindow != 0 && Nlm_currentXGC != NULL) {
    Nlm_EraseRect (r);
    if (text != NULL && len > 0) {
      pt.x = r->left;
      pt.y = r->top;
      limit = ABS (r->right - r->left);
      width = Nlm_TextWidth (text, len);
      while (len > 0 && width > limit) {
        len--;
        width = Nlm_TextWidth (text, len);
      }
      delta = limit - width;
      switch (jst) {
        case 'r':
          pt.x += delta;
          break;
        case 'l':
          break;
        case 'c':
          pt.x += delta / 2;
          break;
        default:
          break;
      }
      limit = ABS (r->bottom - r->top);
      height = Nlm_LineHeight ();
      delta = limit - height;
      if (delta > 0) {
        pt.y += delta / 2;
      }
      if (limit >= height) {
        if (gray) {
          pix = XCreateBitmapFromData (Nlm_currentXDisplay, Nlm_currentXWindow,
                                       (char *) grayPat, 8, 8);
          if (pix != 0 && Nlm_currentXGC != NULL) {
            XSetStipple (Nlm_currentXDisplay, Nlm_currentXGC, pix);
          }
        }
        XDrawString (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                     pt.x - Nlm_XOffset, pt.y + Nlm_Ascent () - Nlm_YOffset,
                     text, (int) len);
        if (gray && pix != 0) {
          XFreePixmap (Nlm_currentXDisplay, pix);
          if (Nlm_currentXGC != NULL) {
            XSetStipple (Nlm_currentXDisplay, Nlm_currentXGC, currentPixmap);
          }
        }
      }
    }
  }
#endif
}

extern void Nlm_MoveTo (Nlm_Int2 x, Nlm_Int2 y)

{
#ifdef WIN_MAC
  MoveTo (x, y);
#endif
#ifdef WIN_MSWIN
  if (Nlm_currentHDC != NULL) {
    MoveToEx (Nlm_currentHDC, x, y, NULL);
  }
#endif
#ifdef WIN_X
  currentPoint.x = x;
  currentPoint.y = y;
#endif
}

extern void Nlm_LineTo (Nlm_Int2 x, Nlm_Int2 y)

{
#ifdef WIN_MAC
  LineTo (x, y);
#endif
#ifdef WIN_MSWIN
  if (Nlm_currentHDC != NULL) {
    SetBkMode (Nlm_currentHDC, TRANSPARENT);
    LineTo (Nlm_currentHDC, x, y);
    SetBkMode (Nlm_currentHDC, OPAQUE);
  }
#endif
#ifdef WIN_X
  if (Nlm_currentXDisplay != NULL && Nlm_currentXWindow != 0 &&
      Nlm_currentXGC != NULL) {
    XDrawLine (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
               currentPoint.x - Nlm_XOffset, currentPoint.y - Nlm_YOffset,
               x - Nlm_XOffset, y - Nlm_YOffset);
    currentPoint.x = x;
    currentPoint.y = y;
  }
#endif
}

extern void Nlm_DrawLine (Nlm_PoinT pt1, Nlm_PoinT pt2)

{
#ifdef WIN_MAC
  MoveTo (pt1.x, pt1.y);
  LineTo (pt2.x, pt2.y);
#endif
#ifdef WIN_MSWIN
  if (Nlm_currentHDC != NULL) {
    MoveToEx (Nlm_currentHDC, pt1.x, pt1.y, NULL);
    SetBkMode (Nlm_currentHDC, TRANSPARENT);
    LineTo (Nlm_currentHDC, pt2.x, pt2.y);
    SetBkMode (Nlm_currentHDC, OPAQUE);
  }
#endif
#ifdef WIN_X
  if (Nlm_currentXDisplay != NULL && Nlm_currentXWindow != 0 &&
      Nlm_currentXGC != NULL) {
    XDrawLine (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
               pt1.x - Nlm_XOffset, pt1.y - Nlm_YOffset,
               pt2.x - Nlm_XOffset, pt2.y - Nlm_YOffset);
    currentPoint.x = pt2.x;
    currentPoint.y = pt2.y;
  }
#endif
}

extern void Nlm_LoadPt (Nlm_PointPtr pt, Nlm_Int2 x, Nlm_Int2 y)

{
  if (pt != NULL) {
    pt->x = x;
    pt->y = y;
  }
}

extern void Nlm_AddPt (Nlm_PoinT src, Nlm_PointPtr dst)

{
  if (dst != NULL) {
    dst->x += src.x;
    dst->y += src.y;
  }
}

extern void Nlm_SubPt (Nlm_PoinT src, Nlm_PointPtr dst)

{
  if (dst != NULL) {
    dst->x -= src.x;
    dst->y -= src.y;
  }
}

extern Nlm_Boolean Nlm_EqualPt (Nlm_PoinT p1, Nlm_PoinT p2)

{
  return (Nlm_Boolean) (p1.x == p2.x && p1.y == p2.y);
}

extern Nlm_Boolean Nlm_PtInRect (Nlm_PoinT pt, Nlm_RectPtr r)

{
  return (Nlm_Boolean) (r != NULL && pt.x >= r->left && pt.x < r->right &&
                        pt.y >= r->top && pt.y < r->bottom);
}

extern Nlm_Boolean Nlm_PtInRgn (Nlm_PoinT pt, Nlm_RegioN rgn)

{
  Nlm_RgnTool    ntool;
  Nlm_Boolean    rsult;
#ifdef WIN_MAC
  Nlm_PointTool  ptool;
#endif

  rsult = FALSE;
  if (rgn != NULL) {
    ntool = (Nlm_RgnTool) rgn;
#ifdef WIN_MAC
    Nlm_PoinTToPointTool (pt, &ptool);
    rsult = PtInRgn (ptool, ntool);
#endif
#ifdef WIN_MSWIN
    rsult = (Nlm_Boolean) PtInRegion (ntool, pt.x, pt.y);
#endif
#ifdef WIN_X
    rsult = (XPointInRegion (ntool, pt.x, pt.y) != 0);
#endif
  }
  return rsult;
}

extern void Nlm_LoadRect (Nlm_RectPtr r, Nlm_Int2 lf,
                          Nlm_Int2 tp, Nlm_Int2 rt,
                          Nlm_Int2 bt)

{
  if (r != NULL) {
    r->left = lf;
    r->top = tp;
    r->right = rt;
    r->bottom = bt;
  }
}

extern void Nlm_UpsetRect (Nlm_RectPtr r, Nlm_Int2 lf,
                           Nlm_Int2 tp, Nlm_Int2 rt,
                           Nlm_Int2 bt)

{
  if (r != NULL) {
    r->left += lf;
    r->top += tp;
    r->right -= rt;
    r->bottom -= bt;
  }
}

extern void Nlm_OffsetRect (Nlm_RectPtr r, Nlm_Int2 dx, Nlm_Int2 dy)

{
  if (r != NULL) {
    r->left += dx;
    r->top += dy;
    r->right += dx;
    r->bottom += dy;
  }
}

extern void Nlm_InsetRect (Nlm_RectPtr r, Nlm_Int2 dx, Nlm_Int2 dy)

{
  if (r != NULL) {
    r->left += dx;
    r->top += dy;
    r->right -= dx;
    r->bottom -= dy;
  }
}

static void Nlm_LoadNormalized (Nlm_RectPtr dst, Nlm_RectPtr src)

{
  if (src != NULL && dst != NULL) {
    Nlm_LoadRect (dst, MIN (src->left, src->right), MIN (src->top, src->bottom),
                  MAX (src->left, src->right), MAX (src->top, src->bottom));
  }
}

extern Nlm_Boolean Nlm_SectRect (Nlm_RectPtr src1, Nlm_RectPtr src2,
                                 Nlm_RectPtr dst)

{
  Nlm_Boolean   rsult;
  Nlm_RectTool  rtool1;
  Nlm_RectTool  rtool2;
  Nlm_RectTool  rtool3;
#ifdef WIN_X
  Nlm_RecT      rct1;
  Nlm_RecT      rct2;
#endif

  rsult = FALSE;
  if (src1 != NULL && src2 != NULL && dst != NULL) {
#ifdef WIN_MAC
    Nlm_RecTToRectTool (src1, &rtool1);
    Nlm_RecTToRectTool (src2, &rtool2);
    rsult = SectRect (&rtool1, &rtool2, &rtool3);
    Nlm_RectToolToRecT (&rtool3, dst);
#endif
#ifdef WIN_MSWIN
    Nlm_RecTToRectTool (src1, &rtool1);
    Nlm_RecTToRectTool (src2, &rtool2);
    rsult = (Nlm_Boolean) IntersectRect (&rtool3, &rtool1, &rtool2);
    Nlm_RectToolToRecT (&rtool3, dst);
#endif
#ifdef WIN_X
    Nlm_LoadNormalized (&rct1, src1);
    Nlm_LoadNormalized (&rct2, src2);
    dst->left = MAX (rct1.left, rct2.left);
    dst->right = MIN (rct1.right, rct2.right);
    dst->top = MAX (rct1.top, rct2.top);
    dst->bottom = MIN (rct1.bottom, rct2.bottom);
    if (dst->left > dst->right || dst->top > dst->bottom) {
      Nlm_LoadRect (dst, 0, 0, 0, 0);
      }
#ifdef DCLAP
	/* ! NEED rsult ! */
    rsult= ( (rct1.left >= rct2.left && rct1.left <= rct2.right)
				|| (rct1.right >= rct2.left && rct1.right <= rct2.right) )
				&& 
			  ( (rct1.top >= rct2.top && rct1.top <= rct2.bottom)
				|| (rct1.bottom >= rct2.top && rct1.bottom <= rct2.bottom) );
#endif
#endif
  }
  return rsult;
}

extern Nlm_Boolean Nlm_UnionRect (Nlm_RectPtr src1, Nlm_RectPtr src2,
                                  Nlm_RectPtr dst)

{
  Nlm_Boolean   rsult;
  Nlm_RectTool  rtool1;
  Nlm_RectTool  rtool2;
  Nlm_RectTool  rtool3;
#ifdef WIN_X
  Nlm_RecT      rct1;
  Nlm_RecT      rct2;
#endif

  rsult = FALSE;
  if (src1 != NULL && src2 != NULL && dst != NULL) {
#ifdef WIN_MAC
    Nlm_RecTToRectTool (src1, &rtool1);
    Nlm_RecTToRectTool (src2, &rtool2);
    UnionRect (&rtool1, &rtool2, &rtool3);
    rsult = EmptyRect (&rtool3);
    Nlm_RectToolToRecT (&rtool3, dst);
#endif
#ifdef WIN_MSWIN
    Nlm_RecTToRectTool (src1, &rtool1);
    Nlm_RecTToRectTool (src2, &rtool2);
    rsult = (Nlm_Boolean) UnionRect (&rtool3, &rtool1, &rtool2);
    Nlm_RectToolToRecT (&rtool3, dst);
#endif
#ifdef WIN_X
    Nlm_LoadNormalized (&rct1, src1);
    Nlm_LoadNormalized (&rct2, src2);
    dst->left = MIN (rct1.left, rct2.left);
    dst->right = MAX (rct1.right, rct2.right);
    dst->top = MIN (rct1.top, rct2.top);
    dst->bottom = MAX (rct1.bottom, rct2.bottom);
#endif
  }
  return rsult;
}

extern Nlm_Boolean Nlm_EqualRect (Nlm_RectPtr r1, Nlm_RectPtr r2)

{
  return (Nlm_Boolean) (r1 != NULL && r2 != NULL && r1->left == r2->left &&
                        r1->top == r2->top && r1->right == r2->right &&
                        r1->bottom == r2->bottom);
}

extern Nlm_Boolean Nlm_EmptyRect (Nlm_RectPtr r)

{
  return (Nlm_Boolean) ! (r != NULL && r->bottom > r->top && r->right > r->left);
}

extern Nlm_Boolean Nlm_RectInRect (Nlm_RectPtr r1, Nlm_RectPtr r2)

{
  Nlm_Boolean  rsult;

  rsult = FALSE;
  if (r1 != NULL && r2 != NULL &&
    r1->top >= r2->top && r1->bottom <= r2->bottom &&
    r1->left >= r2->left && r1->right <= r2->right) {
    rsult = TRUE;
  }
  return rsult;
}

extern Nlm_Boolean Nlm_RectInRgn (Nlm_RectPtr r, Nlm_RegioN rgn)

{
  Nlm_RgnTool   ntool;
  Nlm_Boolean   rsult;
  Nlm_RectTool  rtool;

  rsult = FALSE;
  if (r != NULL && rgn != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    ntool = (Nlm_RgnTool) rgn;
#ifdef WIN_MAC
    rsult = RectInRgn (&rtool, ntool);
#endif
#ifdef WIN_MSWIN
    rsult = (Nlm_Boolean) RectInRegion (ntool, &rtool);
#endif
#ifdef WIN_X
    rsult = (XRectInRegion (ntool, rtool.x, rtool.y, rtool.width, rtool.height) != 0);
#endif
  }
  return rsult;
}

extern void Nlm_EraseRect (Nlm_RectPtr r)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    EraseRect (&rtool);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_Int4      bkColor;
  HBRUSH        hBackBrush;
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentHDC != NULL && Nlm_currentHWnd != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    bkColor = GetBkColor (Nlm_currentHDC);
    hBackBrush = CreateSolidBrush (bkColor);
    FillRect (Nlm_currentHDC, &rtool, hBackBrush);
    DeleteObject (hBackBrush);
  }
#endif
#ifdef WIN_X
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentXDisplay != NULL && Nlm_currentXWindow != 0) {
    Nlm_RecTToRectTool (r, &rtool);
    XSetForeground (Nlm_currentXDisplay, Nlm_currentXGC, currentBkColor);
    XFillRectangle (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                    rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                    rtool.width, rtool.height);
    XSetForeground (Nlm_currentXDisplay, Nlm_currentXGC, currentFgColor);
  }
#endif
}

extern void Nlm_FrameRect (Nlm_RectPtr r)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    if (rtool.right == rtool.left) {
      rtool.right = rtool.left + 1;
    }
    if (rtool.bottom == rtool.top) {
      rtool.bottom = rtool.top + 1;
    }
    FrameRect (&rtool);
  }
#endif
#ifdef WIN_MSWIN
  HBRUSH        oldBrush;
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentHDC != NULL) {
    oldBrush = SelectObject (Nlm_currentHDC, GetStockObject (NULL_BRUSH));
    Nlm_RecTToRectTool (r, &rtool);
    if (rtool.right == rtool.left) {
      rtool.right = rtool.left + 1;
    }
    if (rtool.bottom == rtool.top) {
      rtool.bottom = rtool.top + 1;
    }
    Rectangle (Nlm_currentHDC, rtool.left, rtool.top, rtool.right, rtool.bottom);
    if (oldBrush != NULL) {
      SelectObject (Nlm_currentHDC, oldBrush);
    }
  }
#endif
#ifdef WIN_X
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXWindow != 0 && Nlm_currentXGC != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    rtool.width = MAX (rtool.width, 1);
    rtool.height = MAX (rtool.height, 1);
    XDrawRectangle (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                    rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                    rtool.width - 1, rtool.height - 1);
  }
#endif
}

extern void Nlm_PaintRect (Nlm_RectPtr r)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    if (rtool.right == rtool.left) {
      rtool.right = rtool.left + 1;
    }
    if (rtool.bottom == rtool.top) {
      rtool.bottom = rtool.top + 1;
    }
    PaintRect (&rtool);
  }
#endif
#ifdef WIN_MSWIN
  HPEN          oldPen;
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentHDC != NULL) {
    oldPen = SelectObject (Nlm_currentHDC, GetStockObject (NULL_PEN));
    Nlm_RecTToRectTool (r, &rtool);
    if (rtool.right == rtool.left) {
      rtool.right = rtool.left + 1;
    }
    if (rtool.bottom == rtool.top) {
      rtool.bottom = rtool.top + 1;
    }
    Rectangle (Nlm_currentHDC, rtool.left, rtool.top, rtool.right, rtool.bottom);
    if (oldPen != NULL) {
      SelectObject (Nlm_currentHDC, oldPen);
    }
  }
#endif
#ifdef WIN_X
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXWindow != 0 && Nlm_currentXGC != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    rtool.width = MAX (rtool.width, 1);
    rtool.height = MAX (rtool.height, 1);
    XFillRectangle (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                    rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                    rtool.width, rtool.height);
  }
#endif
}

extern void Nlm_InvertRect (Nlm_RectPtr r)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    InvertRect (&rtool);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentHDC != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    InvertRect (Nlm_currentHDC, &rtool);
  }
#endif
#ifdef WIN_X
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXWindow != 0 && Nlm_currentXGC != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
#ifdef DCLAP
		{
		   /* method recommeded by XLib book, vol1/sect7.4.3 */
		Nlm_Uint4 xorcolor= Nlm_XforeColor ^ Nlm_XbackColor;
		XSetForeground( Nlm_currentXDisplay,Nlm_currentXGC, xorcolor);
    XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, GXxor);
		}
#else
    if (hasColor) {
      XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, GXequiv);
    } 
    else {
      XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, GXinvert);
    }
#endif
    XSetFillStyle (Nlm_currentXDisplay, Nlm_currentXGC, FillStippled);
    XFillRectangle (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                    rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                    rtool.width, rtool.height);
    XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, currentFunction);
    XSetFillStyle (Nlm_currentXDisplay, Nlm_currentXGC, currentFillStyle);
#ifdef DCLAP
		XSetForeground( Nlm_currentXDisplay,Nlm_currentXGC, Nlm_XforeColor);
#endif
  }
#endif
}

extern void Nlm_ScrollRect (Nlm_RectPtr r, Nlm_Int2 dx, Nlm_Int2 dy)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    ScrollRect (&rtool, dx, dy, (Nlm_RgnTool) Nlm_scrollRgn);
    InvalRgn ((Nlm_RgnTool) Nlm_scrollRgn);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentHDC != NULL) {
    SetRectRgn ((Nlm_RgnTool) Nlm_scrollRgn, 0, 0, 0, 0);
    Nlm_RecTToRectTool (r, &rtool);
    ScrollDC (Nlm_currentHDC, dx, dy, &rtool, &rtool,
              (Nlm_RgnTool) Nlm_scrollRgn, NULL);
    if (Nlm_currentHWnd != NULL && Nlm_scrollRgn != NULL) {
      FillRgn (Nlm_currentHDC, (Nlm_RgnTool) Nlm_scrollRgn,
               GetBackgroundBrush (Nlm_currentHWnd));
    }
    InvalidateRgn (Nlm_currentHWnd, (Nlm_RgnTool) Nlm_scrollRgn, TRUE);
  }
#endif
#ifdef WIN_X
  XEvent        event;
  unsigned int  height;
  Nlm_RecT      rct;
  Nlm_RectTool  rtool;
  unsigned int  width;
  unsigned int  dstX;
  unsigned int  dstY;
  unsigned int  srcX;
  unsigned int  srcY;

  if (r != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXGC != NULL && Nlm_currentXWindow != 0) {
    height = ABS (r->bottom - r->top) - ABS (dy);
    width = ABS (r->right - r->left) - ABS (dx);
    if (dx > 0) {
      srcX = r->left - Nlm_XOffset;
      dstX = r->left - Nlm_XOffset + dx;
    } else if (dx < 0) {
      srcX = r->left - Nlm_XOffset - dx;
      dstX = r->left - Nlm_XOffset;
    } else {
      srcX = r->left - Nlm_XOffset;
      dstX = r->left - Nlm_XOffset;
    }
    if (dy > 0) {
      srcY = r->top - Nlm_YOffset;
      dstY = r->top - Nlm_YOffset + dy;
    } else if (dy < 0) {
      srcY = r->top - Nlm_YOffset - dy;
      dstY = r->top - Nlm_YOffset;
    } else {
      srcY = r->top - Nlm_YOffset;
      dstY = r->top - Nlm_YOffset;
    }
    if (hasColor) {
      XSetForeground (Nlm_currentXDisplay, Nlm_currentXGC, currentBkColor);
    }
    XCopyArea (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXWindow,
               Nlm_currentXGC, srcX, srcY, width, height, dstX, dstY);
    XSync (Nlm_currentXDisplay, FALSE);
    while (XCheckTypedWindowEvent (Nlm_currentXDisplay,
           Nlm_currentXWindow, GraphicsExpose, &event)) {
      XClearArea (Nlm_currentXDisplay, Nlm_currentXWindow,
                  event.xgraphicsexpose.x, event.xgraphicsexpose.y,
                  event.xgraphicsexpose.width, event.xgraphicsexpose.height,
                  TRUE);
    }
    if (dx > 0) {
      rct = *r;
      rct.right = rct.left + dx;
      Nlm_RecTToRectTool (&rct, &rtool);
      XClearArea (Nlm_currentXDisplay, Nlm_currentXWindow,
                  rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                  rtool.width, rtool.height, TRUE);
    } else if (dx < 0) {
      rct = *r;
      rct.left = rct.right + dx;
      Nlm_RecTToRectTool (&rct, &rtool);
      XClearArea (Nlm_currentXDisplay, Nlm_currentXWindow,
                  rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                  rtool.width, rtool.height, TRUE);
    }
    if (dy > 0) {
      rct = *r;
      rct.bottom = rct.top + dy;
      Nlm_RecTToRectTool (&rct, &rtool);
      XClearArea (Nlm_currentXDisplay, Nlm_currentXWindow,
                  rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                  rtool.width, rtool.height, TRUE);
    } else if (dy < 0) {
      rct = *r;
      rct.top = rct.bottom + dy;
      Nlm_RecTToRectTool (&rct, &rtool);
      XClearArea (Nlm_currentXDisplay, Nlm_currentXWindow,
                  rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                  rtool.width, rtool.height, TRUE);
    }
    if (hasColor) {
      XSetForeground (Nlm_currentXDisplay, Nlm_currentXGC, currentFgColor);
    }
  }
#endif
}

extern void Nlm_EraseOval (Nlm_RectPtr r)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    EraseOval (&rtool);
  }
#endif
#ifdef WIN_MSWIN
  HBRUSH  oldBrush;
  HPEN    oldPen;

  if (r != NULL && Nlm_currentHDC != NULL && Nlm_currentHWnd != NULL) {
    oldPen = SelectObject (Nlm_currentHDC, GetStockObject (NULL_PEN));
    oldBrush = SelectObject (Nlm_currentHDC, GetBackgroundBrush (Nlm_currentHWnd));
    Ellipse (Nlm_currentHDC, r->left, r->top, r->right, r->bottom);
    if (oldPen != NULL) {
      SelectObject (Nlm_currentHDC, oldPen);
    }
    if (oldBrush != NULL) {
      SelectObject (Nlm_currentHDC, oldBrush);
    }
  }
#endif
#ifdef WIN_X
#endif
}

extern void Nlm_FrameOval (Nlm_RectPtr r)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    FrameOval (&rtool);
  }
#endif
#ifdef WIN_MSWIN
  HBRUSH  oldBrush;

  if (r != NULL && Nlm_currentHDC != NULL) {
    oldBrush = SelectObject (Nlm_currentHDC, GetStockObject (NULL_BRUSH));
    Ellipse (Nlm_currentHDC, r->left, r->top, r->right, r->bottom);
    if (oldBrush != NULL) {
      SelectObject (Nlm_currentHDC, oldBrush);
    }
  }
#endif
#ifdef WIN_X
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXWindow != 0 && Nlm_currentXGC != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    XDrawArc (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
              rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
              rtool.width - 1, rtool.height - 1, 0, 23040);
  }
#endif
}

extern void Nlm_PaintOval (Nlm_RectPtr r)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    PaintOval (&rtool);
  }
#endif
#ifdef WIN_MSWIN
  HPEN  oldPen;

  if (r != NULL && Nlm_currentHDC != NULL) {
    oldPen = SelectObject (Nlm_currentHDC, GetStockObject (NULL_PEN));
    Ellipse (Nlm_currentHDC, r->left, r->top, r->right, r->bottom);
    if (oldPen != NULL) {
      SelectObject (Nlm_currentHDC, oldPen);
    }
  }
#endif
#ifdef WIN_X
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXWindow != 0 && Nlm_currentXGC != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    XFillArc (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
              rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
              rtool.width, rtool.height, 0, 23040);
  }
#endif
}

extern void Nlm_InvertOval (Nlm_RectPtr r)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    InvertOval (&rtool);
  }
#endif
#ifdef WIN_MSWIN
  HBRUSH    oldBrush;
  Nlm_Int2  oldMode;
  HPEN      oldPen;

  if (r != NULL && Nlm_currentHDC != NULL) {
    oldPen = SelectObject (Nlm_currentHDC, GetStockObject (BLACK_PEN));
    oldBrush = SelectObject (Nlm_currentHDC, GetStockObject (BLACK_BRUSH));
    oldMode = GetROP2 (Nlm_currentHDC);
    SetROP2 (Nlm_currentHDC, R2_NOTXORPEN);
    Ellipse (Nlm_currentHDC, r->left, r->top, r->right, r->bottom);
    if (oldPen != NULL) {
      SelectObject (Nlm_currentHDC, oldPen);
    }
    if (oldBrush != NULL) {
      SelectObject (Nlm_currentHDC, oldBrush);
    }
    SetROP2 (Nlm_currentHDC, oldMode);
  }
#endif
#ifdef WIN_X
#endif
}

extern void Nlm_EraseRoundRect (Nlm_RectPtr r, Nlm_Int2 ovlWid,
                                Nlm_Int2 ovlHgt)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    EraseRoundRect (&rtool, ovlWid, ovlHgt);
  }
#endif
#ifdef WIN_MSWIN
  HBRUSH  oldBrush;
  HPEN    oldPen;

  if (r != NULL && Nlm_currentHDC != NULL && Nlm_currentHWnd != NULL) {
    oldPen = SelectObject (Nlm_currentHDC, GetStockObject (NULL_PEN));
    oldBrush = SelectObject (Nlm_currentHDC, GetBackgroundBrush (Nlm_currentHWnd));
    RoundRect (Nlm_currentHDC, r->left, r->top, r->right, r->bottom, ovlWid, ovlHgt);
    if (oldPen != NULL) {
      SelectObject (Nlm_currentHDC, oldPen);
    }
    if (oldBrush != NULL) {
      SelectObject (Nlm_currentHDC, oldBrush);
    }
  }
#endif
#ifdef WIN_X
#endif
}

extern void Nlm_FrameRoundRect (Nlm_RectPtr r, Nlm_Int2 ovlWid,
                                Nlm_Int2 ovlHgt)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    FrameRoundRect (&rtool, ovlWid, ovlHgt);
  }
#endif
#ifdef WIN_MSWIN
  HBRUSH  oldBrush;

  if (r != NULL && Nlm_currentHDC != NULL) {
    oldBrush = SelectObject (Nlm_currentHDC, GetStockObject (NULL_BRUSH));
    RoundRect (Nlm_currentHDC, r->left, r->top, r->right, r->bottom, ovlWid, ovlHgt);
    if (oldBrush != NULL) {
      SelectObject (Nlm_currentHDC, oldBrush);
    }
  }
#endif
#ifdef WIN_X
#endif
}

extern void Nlm_PaintRoundRect (Nlm_RectPtr r, Nlm_Int2 ovlWid,
                                Nlm_Int2 ovlHgt)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    PaintRoundRect (&rtool, ovlWid, ovlHgt);
  }
#endif
#ifdef WIN_MSWIN
  HPEN  oldPen;

  if (r != NULL && Nlm_currentHDC != NULL) {
    oldPen = SelectObject (Nlm_currentHDC, GetStockObject (NULL_PEN));
    RoundRect (Nlm_currentHDC, r->left, r->top, r->right, r->bottom, ovlWid, ovlHgt);
    if (oldPen != NULL) {
      SelectObject (Nlm_currentHDC, oldPen);
    }
  }
#endif
#ifdef WIN_X
#endif
}

extern void Nlm_InvertRoundRect (Nlm_RectPtr r, Nlm_Int2 ovlWid,
                                 Nlm_Int2 ovlHgt)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    InvertRoundRect (&rtool, ovlWid, ovlHgt);
  }
#endif
#ifdef WIN_MSWIN
  HBRUSH    oldBrush;
  Nlm_Int2  oldMode;
  HPEN      oldPen;

  if (r != NULL && Nlm_currentHDC != NULL) {
    oldPen = SelectObject (Nlm_currentHDC, GetStockObject (BLACK_PEN));
    oldBrush = SelectObject (Nlm_currentHDC, GetStockObject (BLACK_BRUSH));
    oldMode = GetROP2 (Nlm_currentHDC);
    SetROP2 (Nlm_currentHDC, R2_NOTXORPEN);
    RoundRect (Nlm_currentHDC, r->left, r->top, r->right, r->bottom, ovlWid, ovlHgt);
    if (oldPen != NULL) {
      SelectObject (Nlm_currentHDC, oldPen);
    }
    if (oldBrush != NULL) {
      SelectObject (Nlm_currentHDC, oldBrush);
    }
    SetROP2 (Nlm_currentHDC, oldMode);
  }
#endif
#ifdef WIN_X
#endif
}

#ifdef WIN_X
static int Nlm_PtToAngle (Nlm_RectPtr r, Nlm_PoinT pt)

{
  int     rsult;
  double  val;
  double  x;
  double  y;

  x = pt.x - (r->right + r->left) / 2;
  y = (r->bottom + r->top) / 2 - pt.y;
  if (x == 0) {
    rsult = 5760;
  } else if (y == 0) {
    rsult = 0;
  } else {
    val = atan2 (ABS (y), ABS (x));
    rsult = val * 11520.0 / 3.14159;
  }
  if (x < 0) {
    if (y < 0) {
      rsult = 11520 + rsult;
    } else {
      rsult = 11520 - rsult;
    }
  } else if (y < 0) {
    rsult = 23040 - rsult;
  }
  return rsult;
}
#endif

extern void Nlm_EraseArc (Nlm_RectPtr r, Nlm_PoinT start, Nlm_PoinT end)

{
#ifdef WIN_MAC
  Nlm_Int2       angle1;
  Nlm_Int2       angle2;
  Nlm_Int2       arcAngle;
  Nlm_PointTool  ptool1;
  Nlm_PointTool  ptool2;
  Nlm_RectTool   rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    Nlm_PoinTToPointTool (start, &ptool1);
    Nlm_PoinTToPointTool (end, &ptool2);
    PtToAngle (&rtool, ptool1, &angle1);
    PtToAngle (&rtool, ptool2, &angle2);
    if (angle2 > angle1) {
      arcAngle = angle2 - angle1;
    } else {
      arcAngle = 360 - angle1 + angle2;
    }
    EraseArc (&rtool, angle1, arcAngle);
  }
#endif
#ifdef WIN_MSWIN
  HBRUSH  oldBrush;
  HPEN    oldPen;

  if (r != NULL && Nlm_currentHDC != NULL && Nlm_currentHWnd != NULL) {
    oldPen = SelectObject (Nlm_currentHDC, GetStockObject (NULL_PEN));
    oldBrush = SelectObject (Nlm_currentHDC, GetBackgroundBrush (Nlm_currentHWnd));
    Pie (Nlm_currentHDC, r->left, r->top, r->right, r->bottom,
         end.x, end.y, start.x, start.y);
    if (oldPen != NULL) {
      SelectObject (Nlm_currentHDC, oldPen);
    }
    if (oldBrush != NULL) {
      SelectObject (Nlm_currentHDC, oldBrush);
    }
  }
#endif
#ifdef WIN_X
#endif
}

extern void Nlm_FrameArc (Nlm_RectPtr r, Nlm_PoinT start, Nlm_PoinT end)
{
#ifdef WIN_MAC
  Nlm_Int2       angle1;
  Nlm_Int2       angle2;
  Nlm_Int2       arcAngle;
  Nlm_PointTool  ptool1;
  Nlm_PointTool  ptool2;
  Nlm_RectTool   rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    Nlm_PoinTToPointTool (start, &ptool1);
    Nlm_PoinTToPointTool (end, &ptool2);
    PtToAngle (&rtool, ptool1, &angle1);
    PtToAngle (&rtool, ptool2, &angle2);
    if (angle2 > angle1) {
      arcAngle = angle2 - angle1;
    } else {
      arcAngle = 360 - angle1 + angle2;
    }
    FrameArc (&rtool, angle1, arcAngle);
  }
#endif
#ifdef WIN_MSWIN
  HPEN  oldPen;

  if (r != NULL && Nlm_currentHDC != NULL) {
    oldPen = SelectObject (Nlm_currentHDC, GetStockObject (NULL_PEN));
    Arc (Nlm_currentHDC, r->left, r->top, r->right, r->bottom,
         end.x, end.y, start.x, start.y);
    if (oldPen != NULL) {
      SelectObject (Nlm_currentHDC, oldPen);
    }
  }
#endif
#ifdef WIN_X
  int           angle1;
  int           angle2;
  int           arcAngle;
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXWindow != 0 && Nlm_currentXGC != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    angle1 = Nlm_PtToAngle (r, start);
    angle2 = Nlm_PtToAngle (r, end);
    if (angle1 > angle2) {
      arcAngle = angle1 - angle2;
      XDrawArc (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                rtool.width - 1, rtool.height - 1, angle1, -arcAngle);
    } else {
      arcAngle = 23040 - angle2 + angle1;
      XDrawArc (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                rtool.width - 1, rtool.height - 1, angle1, -arcAngle);
    }
  }
#endif
}

extern void Nlm_PaintArc (Nlm_RectPtr r, Nlm_PoinT start, Nlm_PoinT end)

{
#ifdef WIN_MAC
  Nlm_Int2       angle1;
  Nlm_Int2       angle2;
  Nlm_Int2       arcAngle;
  Nlm_PointTool  ptool1;
  Nlm_PointTool  ptool2;
  Nlm_RectTool   rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    Nlm_PoinTToPointTool (start, &ptool1);
    Nlm_PoinTToPointTool (end, &ptool2);
    PtToAngle (&rtool, ptool1, &angle1);
    PtToAngle (&rtool, ptool2, &angle2);
    if (angle2 > angle1) {
      arcAngle = angle2 - angle1;
    } else {
      arcAngle = 360 - angle1 + angle2;
    }
    PaintArc (&rtool, angle1, arcAngle);
  }
#endif
#ifdef WIN_MSWIN
  HPEN  oldPen;

  if (r != NULL && Nlm_currentHDC != NULL) {
    oldPen = SelectObject (Nlm_currentHDC, GetStockObject (NULL_PEN));
    Pie (Nlm_currentHDC, r->left, r->top, r->right, r->bottom,
         end.x, end.y, start.x, start.y);
    if (oldPen != NULL) {
      SelectObject (Nlm_currentHDC, oldPen);
    }
  }
#endif
#ifdef WIN_X
  int           angle1;
  int           angle2;
  int           arcAngle;
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXWindow != 0 && Nlm_currentXGC != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    angle1 = Nlm_PtToAngle (r, start);
    angle2 = Nlm_PtToAngle (r, end);
    if (angle1 > angle2) {
      arcAngle = angle1 - angle2;
      XFillArc (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                rtool.width, rtool.height, angle1, -arcAngle);
    } else {
      arcAngle = 23040 - angle2 + angle1;
      XFillArc (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                rtool.width, rtool.height, angle1, -arcAngle);
    }
  }
#endif
}

extern void Nlm_InvertArc (Nlm_RectPtr r, Nlm_PoinT start, Nlm_PoinT end)

{
#ifdef WIN_MAC
  Nlm_Int2       angle1;
  Nlm_Int2       angle2;
  Nlm_Int2       arcAngle;
  Nlm_PointTool  ptool1;
  Nlm_PointTool  ptool2;
  Nlm_RectTool   rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    Nlm_PoinTToPointTool (start, &ptool1);
    Nlm_PoinTToPointTool (end, &ptool2);
    PtToAngle (&rtool, ptool1, &angle1);
    PtToAngle (&rtool, ptool2, &angle2);
    if (angle2 > angle1) {
      arcAngle = angle2 - angle1;
    } else {
      arcAngle = 360 - angle1 + angle2;
    }
    InvertArc (&rtool, angle1, arcAngle);
  }
#endif
#ifdef WIN_MSWIN
  HBRUSH    oldBrush;
  Nlm_Int2  oldMode;
  HPEN      oldPen;

  if (r != NULL && Nlm_currentHDC != NULL) {
    oldPen = SelectObject (Nlm_currentHDC, GetStockObject (BLACK_PEN));
    oldBrush = SelectObject (Nlm_currentHDC, GetStockObject (BLACK_BRUSH));
    oldMode = GetROP2 (Nlm_currentHDC);
    SetROP2 (Nlm_currentHDC, R2_NOTXORPEN);
    Pie (Nlm_currentHDC, r->left, r->top, r->right, r->bottom,
         end.x, end.y, start.x, start.y);
    if (oldPen != NULL) {
      SelectObject (Nlm_currentHDC, oldPen);
    }
    if (oldBrush != NULL) {
      SelectObject (Nlm_currentHDC, oldBrush);
    }
    SetROP2 (Nlm_currentHDC, oldMode);
  }
#endif
#ifdef WIN_X
#endif
}

#ifdef WIN_MAC
static PolyHandle Nlm_CreatePoly (Nlm_Int2 num, Nlm_PointPtr pts)

{
  Nlm_PoinT   firstPt;
  Nlm_Int2    i;
  Nlm_PoinT   pt;
  PolyHandle  rsult;

  rsult = NULL;
  if (pts != NULL && num > 0) {
    rsult = OpenPoly ();
    firstPt = pts [0];
    MoveTo (firstPt.x, firstPt.y);
    for (i = 1; i < num; i++) {
      pt = pts [i];
      LineTo (pt.x, pt.y);
    }
    if (! Nlm_EqualPt (pt, firstPt)) {
      LineTo (firstPt.x, firstPt.y);
    }
    ClosePoly ();
  }
  return rsult;
}

static void Nlm_DestroyPoly (PolyHandle ply)

{
  if (ply != NULL) {
    KillPoly (ply);
  }
}
#endif

#ifdef WIN_MSWIN
static LPPOINT Nlm_CreatePoly (Nlm_Int2 num, Nlm_PointPtr pts)

{
  Nlm_PoinT      firstPt;
  Nlm_Int2       i;
  Nlm_PoinT      pt;
  Nlm_PointTool  ptool;
  LPPOINT        rsult;

  rsult = NULL;
  if (pts != NULL && num > 0) {
    rsult = (LPPOINT) Nlm_MemNew ((Nlm_sizeT) ((num + 1) * sizeof (POINT)));
    if (rsult != NULL) {
      firstPt = pts [0];
      for (i = 0; i < num; i++) {
        pt = pts [i];
        Nlm_PoinTToPointTool (pt, &ptool);
        rsult [i] = ptool;
      }
      if (! Nlm_EqualPt (pt, firstPt)) {
        Nlm_PoinTToPointTool (firstPt, &ptool);
        rsult [i] = ptool;
      }
    }
  }
  return rsult;
}

static void Nlm_DestroyPoly (LPPOINT ply)

{
  if (ply != NULL) {
    Nlm_MemFree (ply);
  }
}
#endif

#ifdef WIN_X
static XPoint *Nlm_CreatePoly (Nlm_Int2 num, Nlm_PointPtr pts)

{
  Nlm_PoinT      firstPt;
  Nlm_Int2       i;
  Nlm_PoinT      pt;
  Nlm_PointTool  ptool;
  XPoint         *rsult;

  rsult = NULL;
  if (pts != NULL && num > 0) {
    rsult = (XPoint *) Nlm_MemNew ((Nlm_sizeT) ((num + 1) * sizeof (XPoint)));
    if (rsult != NULL) {
      firstPt = pts [0];
      firstPt.x -= Nlm_XOffset;
      firstPt.y -= Nlm_YOffset;
      for (i = 0; i < num; i++) {
        pt = pts [i];
        pt.x -= Nlm_XOffset;
        pt.y -= Nlm_YOffset;
        Nlm_PoinTToPointTool (pt, &ptool);
        rsult [i] = ptool;
      }
      if (! Nlm_EqualPt (pt, firstPt)) {
        Nlm_PoinTToPointTool (firstPt, &ptool);
        rsult [i] = ptool;
      }
    }
  }
  return rsult;
}

static void Nlm_DestroyPoly (XPoint *ply)

{
  if (ply != NULL) {
    Nlm_MemFree (ply);
  }
}
#endif

extern void Nlm_ErasePoly (Nlm_Int2 num, Nlm_PointPtr pts)

{
  if (pts != NULL && num > 1) {
#ifdef WIN_MAC
  PolyHandle   ply;

  ply = Nlm_CreatePoly (num, pts);
  if (ply != NULL) {
    ErasePoly (ply);
  }
  Nlm_DestroyPoly (ply);
#endif
#ifdef WIN_MSWIN
#endif
#ifdef WIN_X
#endif
  }
}

extern void Nlm_FramePoly (Nlm_Int2 num, Nlm_PointPtr pts)

{
  if (pts != NULL && num > 1) {
#ifdef WIN_MAC
  PolyHandle   ply;

  ply = Nlm_CreatePoly (num, pts);
  if (ply != NULL) {
    FramePoly (ply);
  }
  Nlm_DestroyPoly (ply);
#endif
#ifdef WIN_MSWIN
  LPPOINT  ply;

  ply = Nlm_CreatePoly (num, pts);
  if (Nlm_currentHDC != NULL && ply != NULL) {
    if (! Nlm_EqualPt (pts [0], pts [num - 1])) {
      num++;
    }
    Polyline (Nlm_currentHDC, ply, num);
  }
  Nlm_DestroyPoly (ply);
#endif
#ifdef WIN_X
  XPoint  *ply;

  ply = Nlm_CreatePoly (num, pts);
  if (Nlm_currentXDisplay != NULL && Nlm_currentXWindow != 0 &&
      Nlm_currentXGC != NULL && ply != NULL) {
    if (! Nlm_EqualPt (pts [0], pts [num - 1])) {
      num++;
    }
    XDrawLines (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                ply, num, CoordModeOrigin);
  }
  Nlm_DestroyPoly (ply);
#endif
  }
}

extern void Nlm_PaintPoly (Nlm_Int2 num, Nlm_PointPtr pts)

{
  if (pts != NULL && num > 1) {
#ifdef WIN_MAC
  PolyHandle   ply;

  ply = Nlm_CreatePoly (num, pts);
  if (ply != NULL) {
    PaintPoly (ply);
  }
  Nlm_DestroyPoly (ply);
#endif
#ifdef WIN_MSWIN
  LPPOINT  ply;

  ply = Nlm_CreatePoly (num, pts);
  if (Nlm_currentHDC != NULL && ply != NULL) {
    if (! Nlm_EqualPt (pts [0], pts [num - 1])) {
      num++;
    }
    SetPolyFillMode (Nlm_currentHDC, ALTERNATE);
    Polygon (Nlm_currentHDC, ply, num);
  }
  Nlm_DestroyPoly (ply);
#endif
#ifdef WIN_X
  XPoint  *ply;

  ply = Nlm_CreatePoly (num, pts);
  if (Nlm_currentXDisplay != NULL && Nlm_currentXWindow != 0 &&
      Nlm_currentXGC != NULL && ply != NULL) {
    if (! Nlm_EqualPt (pts [0], pts [num - 1])) {
      num++;
    }
    XSetFillRule (Nlm_currentXDisplay, Nlm_currentXGC, EvenOddRule);
    XFillPolygon (Nlm_currentXDisplay, Nlm_currentXWindow, Nlm_currentXGC,
                  ply, num, Complex, CoordModeOrigin);
  }
  Nlm_DestroyPoly (ply);
#endif
  }
}

extern void Nlm_InvertPoly (Nlm_Int2 num, Nlm_PointPtr pts)

{
  if (pts != NULL && num > 1) {
#ifdef WIN_MAC
  PolyHandle   ply;

  ply = Nlm_CreatePoly (num, pts);
  if (ply != NULL) {
    InvertPoly (ply);
  }
  Nlm_DestroyPoly (ply);
#endif
#ifdef WIN_MSWIN
#endif
#ifdef WIN_X
#endif
  }
}

extern Nlm_RegioN Nlm_CreateRgn (void)

{
  Nlm_RgnTool  ntool;

#ifdef WIN_MAC
  ntool = NewRgn ();
#endif
#ifdef WIN_MSWIN
  ntool = CreateRectRgn (0, 0, 0, 0);
#endif
#ifdef WIN_X
  ntool = XCreateRegion ();
#endif
  return (Nlm_RegioN) ntool;
}

extern Nlm_RegioN Nlm_DestroyRgn (Nlm_RegioN rgn)

{
  Nlm_RgnTool  ntool;

  if (rgn != NULL) {
    ntool = (Nlm_RgnTool) rgn;
#ifdef WIN_MAC
    DisposeRgn (ntool);
#endif
#ifdef WIN_MSWIN
    DeleteObject (ntool);
#endif
#ifdef WIN_X
    XDestroyRegion (ntool);
#endif
  }
  return NULL;
}

extern void Nlm_ClearRgn (Nlm_RegioN rgn)

{
  Nlm_RgnTool  ntool;
  Nlm_RgnTool  temp;

  if (rgn != NULL) {
    ntool = (Nlm_RgnTool) rgn;
#ifdef WIN_MAC
    SetEmptyRgn (ntool);
#endif
#ifdef WIN_MSWIN
    temp = CreateRectRgn (0, 0, 0, 0);
    CombineRgn (ntool, temp, temp, RGN_COPY);
    DeleteObject (temp);
#endif
#ifdef WIN_X
    XUnionRegion (emptyRgn, emptyRgn, ntool);
#endif
  } 
}

extern void Nlm_LoadRectRgn (Nlm_RegioN rgn, Nlm_Int2 lf,
                             Nlm_Int2 tp, Nlm_Int2 rt,
                             Nlm_Int2 bt)

{
  Nlm_RgnTool   ntool;
#ifdef WIN_X
  Nlm_RecT      rct;
  Nlm_RectTool  rtool;
#endif

  if (rgn != NULL) {
    ntool = (Nlm_RgnTool) rgn;
#ifdef WIN_MAC
    SetRectRgn (ntool, lf, tp, rt, bt);
#endif
#ifdef WIN_MSWIN
    SetRectRgn (ntool, lf, tp, rt, bt);
#endif
#ifdef WIN_X
    Nlm_LoadRect (&rct, lf, tp, rt, bt);
    Nlm_RecTToRectTool (&rct, &rtool);
    XUnionRectWithRegion (&rtool, emptyRgn, ntool);
#endif
  }
}

extern void Nlm_OffsetRgn (Nlm_RegioN rgn, Nlm_Int2 dx, Nlm_Int2 dy)

{
  Nlm_RgnTool  ntool;

  if (rgn != NULL) {
    ntool = (Nlm_RgnTool) rgn;
#ifdef WIN_MAC
    OffsetRgn (ntool, dx, dy);
#endif
#ifdef WIN_MSWIN
    OffsetRgn (ntool, dx, dy);
#endif
#ifdef WIN_X
    XOffsetRegion (ntool, dx, dy);
#endif
  }
}

extern void Nlm_SectRgn (Nlm_RegioN src1, Nlm_RegioN src2, Nlm_RegioN dst)

{
  Nlm_RgnTool  ntool1;
  Nlm_RgnTool  ntool2;
  Nlm_RgnTool  ntool3;
#ifdef WIN_X
  Nlm_RgnTool  temp;
#endif

  if (src1 != NULL && src2 != NULL && dst != NULL) {
    ntool1 = (Nlm_RgnTool) src1;
    ntool2 = (Nlm_RgnTool) src2;
    ntool3 = (Nlm_RgnTool) dst;
#ifdef WIN_MAC
    SectRgn (ntool1, ntool2, ntool3);
#endif
#ifdef WIN_MSWIN
    CombineRgn (ntool3, ntool1, ntool2, RGN_AND);
#endif
#ifdef WIN_X
    temp = XCreateRegion ();
    XIntersectRegion (ntool1, ntool2, temp);
    XUnionRegion (temp, emptyRgn, ntool3);
    XDestroyRegion (temp);
#endif
  }
}

extern void Nlm_UnionRgn (Nlm_RegioN src1, Nlm_RegioN src2, Nlm_RegioN dst)

{
  Nlm_RgnTool  ntool1;
  Nlm_RgnTool  ntool2;
  Nlm_RgnTool  ntool3;
#ifdef WIN_X
  Nlm_RgnTool  temp;
#endif

  if (src1 != NULL && src2 != NULL && dst != NULL) {
    ntool1 = (Nlm_RgnTool) src1;
    ntool2 = (Nlm_RgnTool) src2;
    ntool3 = (Nlm_RgnTool) dst;
#ifdef WIN_MAC
    UnionRgn (ntool1, ntool2, ntool3);
#endif
#ifdef WIN_MSWIN
    CombineRgn (ntool3, ntool1, ntool2, RGN_OR);
#endif
#ifdef WIN_X
    temp = XCreateRegion ();
    XUnionRegion (ntool1, ntool2, temp);
    XUnionRegion (temp, emptyRgn, ntool3);
    XDestroyRegion (temp);
#endif
  }
}

extern void Nlm_DiffRgn (Nlm_RegioN src1, Nlm_RegioN src2, Nlm_RegioN dst)

{
  Nlm_RgnTool  ntool1;
  Nlm_RgnTool  ntool2;
  Nlm_RgnTool  ntool3;
#ifdef WIN_X
  Nlm_RgnTool  temp;
#endif

  if (src1 != NULL && src2 != NULL && dst != NULL) {
    ntool1 = (Nlm_RgnTool) src1;
    ntool2 = (Nlm_RgnTool) src2;
    ntool3 = (Nlm_RgnTool) dst;
#ifdef WIN_MAC
    DiffRgn (ntool1, ntool2, ntool3);
#endif
#ifdef WIN_MSWIN
    CombineRgn (ntool3, ntool1, ntool2, RGN_DIFF);
#endif
#ifdef WIN_X
    temp = XCreateRegion ();
    XSubtractRegion (ntool1, ntool2, temp);
    XUnionRegion (temp, emptyRgn, ntool3);
    XDestroyRegion (temp);
#endif
  }
}

extern void Nlm_XorRgn (Nlm_RegioN src1, Nlm_RegioN src2, Nlm_RegioN dst)

{
  Nlm_RgnTool  ntool1;
  Nlm_RgnTool  ntool2;
  Nlm_RgnTool  ntool3;
#ifdef WIN_X
  Nlm_RgnTool  temp;
#endif

  if (src1 != NULL && src2 != NULL && dst != NULL) {
    ntool1 = (Nlm_RgnTool) src1;
    ntool2 = (Nlm_RgnTool) src2;
    ntool3 = (Nlm_RgnTool) dst;
#ifdef WIN_MAC
    XorRgn (ntool1, ntool2, ntool3);
#endif
#ifdef WIN_MSWIN
    CombineRgn (ntool3, ntool1, ntool2, RGN_XOR);
#endif
#ifdef WIN_X
    temp = XCreateRegion ();
    XXorRegion (ntool1, ntool2, temp);
    XUnionRegion (temp, emptyRgn, ntool3);
    XDestroyRegion (temp);
#endif
  }
}

extern Nlm_Boolean Nlm_EqualRgn (Nlm_RegioN rgn1, Nlm_RegioN rgn2)

{
  Nlm_RgnTool  ntool1;
  Nlm_RgnTool  ntool2;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  if (rgn1 != NULL && rgn2 != NULL) {
    ntool1 = (Nlm_RgnTool) rgn1;
    ntool2 = (Nlm_RgnTool) rgn2;
#ifdef WIN_MAC
    rsult = EqualRgn (ntool1, ntool2);
#endif
#ifdef WIN_MSWIN
    rsult = (Nlm_Boolean) EqualRgn (ntool1, ntool2);
#endif
#ifdef WIN_X
    rsult = (XEqualRegion (ntool1, ntool2) != 0);
#endif
  }
  return rsult;
}

extern Nlm_Boolean Nlm_EmptyRgn (Nlm_RegioN rgn)

{
  Nlm_RgnTool   ntool;
  Nlm_Boolean   rsult;
#ifdef WIN_MSWIN
  Nlm_RectTool  rtool;
#endif

  rsult = FALSE;
  if (rgn != NULL) {
    ntool = (Nlm_RgnTool) rgn;
#ifdef WIN_MAC
    rsult = EmptyRgn (ntool);
#endif
#ifdef WIN_MSWIN
    rsult = (Nlm_Boolean) (GetRgnBox (ntool, &rtool) == NULLREGION);
#endif
#ifdef WIN_X
    rsult = (XEmptyRegion (ntool) != 0);
#endif
  }
  return rsult;
}

extern void Nlm_EraseRgn (Nlm_RegioN rgn)

{
#ifdef WIN_MAC
  Nlm_RgnTool  ntool;

  if (rgn != NULL) {
    ntool = (Nlm_RgnTool) rgn;
    EraseRgn (ntool);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_RgnTool  ntool;

  if (rgn != NULL && Nlm_currentHDC != NULL && Nlm_currentHWnd != NULL) {
    ntool = (Nlm_RgnTool) rgn;
    FillRgn (Nlm_currentHDC, ntool, GetBackgroundBrush (Nlm_currentHWnd));
  }
#endif
#ifdef WIN_X
#endif
}

extern void Nlm_FrameRgn (Nlm_RegioN rgn)

{
#ifdef WIN_MAC
  Nlm_RgnTool  ntool;

  if (rgn != NULL) {
    ntool = (Nlm_RgnTool) rgn;
    FrameRgn (ntool);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_RgnTool  ntool;

  if (rgn != NULL && Nlm_currentHDC != NULL) {
    ntool = (Nlm_RgnTool) rgn;
    FrameRgn (Nlm_currentHDC, ntool, GetStockObject (BLACK_BRUSH), 1, 1);
  }
#endif
#ifdef WIN_X
#endif
}

extern void Nlm_PaintRgn (Nlm_RegioN rgn)

{
#ifdef WIN_MAC
  Nlm_RgnTool  ntool;

  if (rgn != NULL) {
    ntool = (Nlm_RgnTool) rgn;
    PaintRgn (ntool);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_RgnTool  ntool;
  HBRUSH       oldBrush;

  if (rgn != NULL && Nlm_currentHDC != NULL) {
    ntool = (Nlm_RgnTool) rgn;
    oldBrush = SelectObject (Nlm_currentHDC, GetStockObject (BLACK_BRUSH));
    if (oldBrush != NULL) {
      SelectObject (Nlm_currentHDC, oldBrush);
      FillRgn (Nlm_currentHDC, ntool, oldBrush);
    }
  }
#endif
#ifdef WIN_X
#endif
}

extern void Nlm_InvertRgn (Nlm_RegioN rgn)

{
#ifdef WIN_MAC
  Nlm_RgnTool  ntool;

  if (rgn != NULL) {
    ntool = (Nlm_RgnTool) rgn;
    InvertRgn (ntool);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_RgnTool  ntool;

  if (rgn != NULL && Nlm_currentHDC != NULL) {
    ntool = (Nlm_RgnTool) rgn;
    InvertRgn (Nlm_currentHDC, ntool);
  }
#endif
#ifdef WIN_X
#endif
}

extern void Nlm_ClipRect (Nlm_RectPtr r)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    ClipRect (&rtool);
  }
#endif
#ifdef WIN_MSWIN
  HRGN  hRgnClip;

  if (r != NULL && Nlm_currentHDC != NULL) {
    hRgnClip = CreateRectRgn (r->left, r->top, r->right, r->bottom);
    SelectClipRgn (Nlm_currentHDC, hRgnClip);
    DeleteObject (hRgnClip);
  }
#endif
#ifdef WIN_X
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    rtool.x -= Nlm_XOffset;
    rtool.y -= Nlm_YOffset;
    XSetClipRectangles (Nlm_currentXDisplay, Nlm_currentXGC, 0, 0, &rtool, 1, Unsorted);
    if (Nlm_clpRgn != NULL) {
      XDestroyRegion ((Nlm_RgnTool) Nlm_clpRgn);
      Nlm_clpRgn = NULL;
    }
    Nlm_clpRgn = (Nlm_RegioN) XCreateRegion ();
    XUnionRectWithRegion (&rtool, (Nlm_RgnTool) Nlm_clpRgn, (Nlm_RgnTool) Nlm_clpRgn);
    XOffsetRegion ((Nlm_RgnTool) Nlm_clpRgn, Nlm_XOffset, Nlm_YOffset);
  }
#endif
}

extern void Nlm_ClipRgn (Nlm_RegioN rgn)

{
#ifdef WIN_MAC
  Nlm_RgnTool  ntool;

  if (rgn != NULL) {
    ntool = (Nlm_RgnTool) rgn;
    SetClip (ntool);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_RgnTool  ntool;

  if (rgn != NULL && Nlm_currentHDC != NULL) {
    ntool = (Nlm_RgnTool) rgn;
    SelectClipRgn (Nlm_currentHDC, ntool);
  }
#endif
#ifdef WIN_X
  Nlm_RgnTool  ntool1;
  Nlm_RgnTool  ntool2;

  if (rgn != NULL && Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    ntool1 = XCreateRegion ();
    ntool2 = XCreateRegion ();
    XUnionRegion ((Nlm_RgnTool) rgn, ntool1, ntool2);
    XOffsetRegion (ntool2, -Nlm_XOffset, -Nlm_YOffset);
    XSetRegion (Nlm_currentXDisplay, Nlm_currentXGC, ntool2);
    if (Nlm_clpRgn != NULL) {
      XDestroyRegion ((Nlm_RgnTool) Nlm_clpRgn);
      Nlm_clpRgn = NULL;
    }
    Nlm_clpRgn = (Nlm_RegioN) XCreateRegion ();
    XUnionRegion ((Nlm_RgnTool) rgn, ntool1, (Nlm_RgnTool) Nlm_clpRgn);
    XOffsetRegion ((Nlm_RgnTool) Nlm_clpRgn, -Nlm_XOffset, -Nlm_YOffset);
    XDestroyRegion (ntool1);
    XDestroyRegion (ntool2);
  }
#endif
}

extern void Nlm_ResetClip (void)

{
#ifdef WIN_MAC
  Nlm_RecT      r;
  Nlm_RectTool  rtool;

  Nlm_LoadRect (&r, -32767, -32767, 32767, 32767);
  Nlm_RecTToRectTool (&r, &rtool);
  ClipRect (&rtool);
#endif
#ifdef WIN_MSWIN
  if (Nlm_currentHDC != NULL) {
    SelectClipRgn (Nlm_currentHDC, NULL);
  }
#endif
#ifdef WIN_X
  if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
    XSetClipMask (Nlm_currentXDisplay, Nlm_currentXGC, None);
    if (Nlm_clpRgn != NULL) {
      XDestroyRegion ((Nlm_RgnTool) Nlm_clpRgn);
      Nlm_clpRgn = NULL;
    }
  }
#endif
}

extern void Nlm_ValidRect (Nlm_RectPtr r)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    ValidRect (&rtool);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentHWnd != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    ValidateRect (Nlm_currentHWnd, &rtool);
  }
#endif
#ifdef WIN_X
#endif
}

extern void Nlm_InvalRect (Nlm_RectPtr r)

{
#ifdef WIN_MAC
  Nlm_RectTool  rtool;

  if (r != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    InvalRect (&rtool);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentHWnd != NULL) {
    Nlm_RecTToRectTool (r, &rtool);
    InvalidateRect (Nlm_currentHWnd, &rtool, TRUE);
  }
#endif
#ifdef WIN_X
  Nlm_RectTool  rtool;

  if (r != NULL && Nlm_currentXDisplay != NULL && Nlm_currentXWindow != 0) {
    Nlm_RecTToRectTool (r, &rtool);
    XClearArea (Nlm_currentXDisplay, Nlm_currentXWindow,
                rtool.x - Nlm_XOffset, rtool.y - Nlm_YOffset,
                rtool.width, rtool.height, TRUE);
  }
#endif
}

extern void Nlm_ValidRgn (Nlm_RegioN rgn)

{
#ifdef WIN_MAC
  if (rgn != NULL) {
    ValidRgn ((Nlm_RgnTool) rgn);
  }
#endif
#ifdef WIN_MSWIN
  if (rgn != NULL && Nlm_currentHWnd != NULL) {
    ValidateRgn (Nlm_currentHWnd, (Nlm_RgnTool) rgn);
  }
#endif
#ifdef WIN_X
#endif
}

extern void Nlm_InvalRgn (Nlm_RegioN rgn)

{
#ifdef WIN_MAC
  if (rgn != NULL) {
    InvalRgn ((Nlm_RgnTool) rgn);
  }
#endif
#ifdef WIN_MSWIN
  if (rgn != NULL && Nlm_currentHWnd != NULL) {
    InvalidateRgn (Nlm_currentHWnd, (Nlm_RgnTool) rgn, TRUE);
  }
#endif
#ifdef WIN_X
  Nlm_RgnTool   ntool1;
  Nlm_RgnTool   ntool2;
  Nlm_RectTool  rtool;

  if (rgn != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXGC != NULL && Nlm_currentXWindow != 0) {
    ntool1 = XCreateRegion ();
    ntool2 = XCreateRegion ();
    XUnionRegion ((Nlm_RgnTool) rgn, ntool1, ntool2);
    XOffsetRegion (ntool2, -Nlm_XOffset, -Nlm_YOffset);
    XSetRegion (Nlm_currentXDisplay, Nlm_currentXGC, ntool2);
    XClipBox (ntool2, &rtool);
    XClearArea (Nlm_currentXDisplay, Nlm_currentXWindow, rtool.x,
                rtool.y, rtool.width, rtool.height, TRUE);
    XDestroyRegion (ntool1);
    XDestroyRegion (ntool2);
    if (Nlm_clpRgn != NULL) {
      ntool1 = XCreateRegion ();
      ntool2 = XCreateRegion ();
      XUnionRegion ((Nlm_RgnTool) Nlm_clpRgn, ntool1, ntool2);
      XOffsetRegion (ntool2, -Nlm_XOffset, -Nlm_YOffset);
      XSetRegion (Nlm_currentXDisplay, Nlm_currentXGC, ntool2);
      XDestroyRegion (ntool1);
      XDestroyRegion (ntool2);
    } else {
      XSetClipMask (Nlm_currentXDisplay, Nlm_currentXGC, None);
    }
  }
#endif
}

extern void Nlm_CopyBits (Nlm_RectPtr r, Nlm_VoidPtr source)

{
#ifdef WIN_MAC
  BitMap    dstBits;
  Nlm_Int2  mode;
  PenState  pnState;
  GrafPtr   port;
  Rect      rect;
  BitMap    srcBits;
  Rect      srcRect;

  if (r != NULL && source != NULL) {
    GetPort (&port);
    GetPenState (&pnState);
    switch (pnState.pnMode) {
      case patCopy:
        mode = srcCopy;
        break;
      case patOr:
        mode = srcOr;
        break;
      case patXor:
        mode = srcXor;
        break;
      case patBic:
        mode = srcBic;
        break;
      default:
        mode = srcCopy;
        break;
    }
    Nlm_RecTToRectTool (r, &rect);
    srcRect = rect;
    OffsetRect (&srcRect, -rect.left, -rect.top);
    srcBits.baseAddr = (Ptr) source;
    srcBits.rowBytes = (rect.right - rect.left - 1) / 8 + 1;
    srcBits.bounds = srcRect;
    dstBits = port->portBits;
    CopyBits (&srcBits, &dstBits, &srcRect, &rect, mode, NULL);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_Int2      cols;
  HBITMAP       hBitmap;
  HBITMAP       hOldBitmap;
  HDC           hMemoryDC;
  Nlm_Int2      i;
  Nlm_Int2      j;
  Nlm_Int4      mode;
  Nlm_Int4      num;
  Nlm_Boolean   odd;
  Nlm_Uint1Ptr  p;
  Nlm_Uint1Ptr  ptr;
  Nlm_Uint1Ptr  q;
  Nlm_Int2      rop2;
  Nlm_Int2      rows;

  if (r != NULL && source != NULL && Nlm_currentHDC != NULL) {
    rows = (r->right - r->left - 1) / 8 + 1;
    odd = (Nlm_Boolean) ((rows & 1) != 0);
    cols = r->bottom - r->top;
    num = rows * cols;
    if (odd) {
      num += cols;
    }
    ptr = (Nlm_Uint1Ptr) Nlm_MemNew ((Nlm_sizeT) num * sizeof (Nlm_Uint1));
    if (ptr != NULL) {
      p = source;
      q = ptr;
      for (i = 0; i < cols; i++) {
        j = 0;
        while (j < rows) {
          *q = *p;
          p++;
          q++;
          j++;
        }
        if (odd) {
          *q = 0;
          q++;
        }
      }
      q = ptr;
      while (num > 0) {
        *q = (Nlm_Uint1) ~(*q);
        q++;
        num--;
      }
      hBitmap = CreateBitmap (r->right - r->left, r->bottom - r->top,
                              1, 1, (LPSTR) ptr);
      hMemoryDC = CreateCompatibleDC (Nlm_currentHDC);
      hOldBitmap = SelectObject (hMemoryDC, hBitmap);
      if (hOldBitmap != NULL) {
        rop2 = GetROP2 (Nlm_currentHDC);
        switch (rop2) {
          case R2_COPYPEN:
            mode = SRCCOPY;
            break;
          case R2_MASKPEN:
            mode = SRCAND;
            break;
          case R2_NOTXORPEN:
            mode = 0x00990066;
            break;
          case R2_MERGENOTPEN:
            mode = MERGEPAINT;
            break;
          default:
            mode = WHITENESS;
            break;
        }
        BitBlt (Nlm_currentHDC, r->left, r->top,
                r->right - r->left, r->bottom - r->top,
                hMemoryDC, 0, 0, mode);
        SelectObject (hMemoryDC, hOldBitmap);
      }
      Nlm_MemFree (ptr);
      DeleteDC (hMemoryDC);
      DeleteObject (hBitmap);
    }
  }
#endif
#ifdef WIN_X
  Nlm_Int2      cols;
  Nlm_Int2      height;
  Nlm_Int2      i;
  Nlm_Int2      j;
  Nlm_Int4      num;
  Pixmap        pixmap;
  Nlm_Uint1Ptr  ptr;
  Nlm_Uint1Ptr  q;
  Nlm_Int2      rows;
  Nlm_Int2      width;

  if (r != NULL && source != NULL && Nlm_currentXDisplay != NULL &&
      Nlm_currentXWindow != 0 && Nlm_currentXGC != NULL) {
    rows = (r->right - r->left - 1) / 8 + 1;
    cols = r->bottom - r->top;
    num = rows * cols;
    ptr = (Nlm_Uint1Ptr) Nlm_MemNew ((Nlm_sizeT) num * sizeof (Nlm_Uint1));
    if (ptr != NULL) {
      Nlm_MemCopy (ptr, source, (Nlm_sizeT) num);
      q = ptr;
      while (num > 0) {
        *q = flip [*q];
        q++;
        num--;
      }
      width = r->right - r->left;
      height = r->bottom - r->top;
      pixmap = XCreateBitmapFromData (Nlm_currentXDisplay, Nlm_currentXWindow,
                                      (char *) ptr, width, height);
      if (currentMode != MERGE_MODE) {
        XCopyPlane (Nlm_currentXDisplay, pixmap, Nlm_currentXWindow,
                    Nlm_currentXGC, 0, 0, width, height,
                    r->left - Nlm_XOffset, r->top - Nlm_YOffset, 1);
      } else {
        XSetClipOrigin (Nlm_currentXDisplay, Nlm_currentXGC,
                        r->left - Nlm_XOffset, r->top - Nlm_YOffset);
        XSetClipMask (Nlm_currentXDisplay, Nlm_currentXGC, pixmap);
        XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, GXcopy);
        XCopyPlane (Nlm_currentXDisplay, pixmap, Nlm_currentXWindow,
                    Nlm_currentXGC, 0, 0, width, height,
                    r->left - Nlm_XOffset, r->top - Nlm_YOffset, 1);
        XSetFunction (Nlm_currentXDisplay, Nlm_currentXGC, GXand);
        XSetClipOrigin (Nlm_currentXDisplay, Nlm_currentXGC, 0, 0);
        if (Nlm_clpRgn != NULL) {
          XSetRegion (Nlm_currentXDisplay, Nlm_currentXGC, (Nlm_RgnTool) Nlm_clpRgn);
        } else {
          XSetClipMask (Nlm_currentXDisplay, Nlm_currentXGC, None);
        }
      }
      XFreePixmap (Nlm_currentXDisplay, pixmap);
      Nlm_MemFree (ptr);
    }
  }
#endif
}

#ifdef WIN_X
static Nlm_Uint4 Nlm_SetupColor (Colormap colormap, Nlm_Int2 red, Nlm_Int2 green, Nlm_Int2 blue)

{
  XColor  xcolor;

  xcolor.red = red << 8;
  xcolor.green = green << 8;
  xcolor.blue = blue << 8;
  if (XAllocColor (Nlm_currentXDisplay, colormap, &xcolor) == 0) {
    xcolor.pixel = WhitePixel (Nlm_currentXDisplay, Nlm_currentXScreen);
  }
  return xcolor.pixel;
}
#endif

extern void Nlm_SetUpDrawingTools (void)

{
#ifdef WIN_MAC
  RgnPtr     rptr;
  SysEnvRec  sysenv;

  Nlm_scrollRgn = (Nlm_RegioN) (NewRgn ());

  Nlm_updateRgn = (Nlm_RegioN) (NewRgn ());
  SetRectRgn ((Nlm_RgnTool) Nlm_updateRgn, -32768, -32768, 32767, 32767);
  HLock ((Handle) Nlm_updateRgn);
  rptr = (RgnPtr) *((Handle) Nlm_updateRgn);
  Nlm_RectToolToRecT (&(rptr->rgnBBox), &Nlm_updateRect);
  HUnlock ((Handle) Nlm_updateRgn);

  Nlm_fontList = NULL;
  Nlm_fontInUse = NULL;
  Nlm_systemFont = (Nlm_FonT) Nlm_HandNew (sizeof (Nlm_FontRec));
  Nlm_LoadFontData (Nlm_systemFont, NULL, "", 12, 0, FALSE,
                    FALSE, FALSE, 0, 0, NULL, NULL);
  Nlm_programFont = (Nlm_FonT) Nlm_HandNew (sizeof (Nlm_FontRec));
  Nlm_LoadFontData (Nlm_programFont, NULL, "", 9, 0, FALSE,
                    FALSE, FALSE, 4, 0, NULL, NULL);
  Nlm_fontList = NULL;
  Nlm_fontInUse = Nlm_systemFont;

  Nlm_stdAscent = Nlm_Ascent ();
  Nlm_stdDescent = Nlm_Descent ();
  Nlm_stdLeading = Nlm_Leading ();
  Nlm_stdFontHeight = Nlm_FontHeight ();
  Nlm_stdLineHeight = Nlm_LineHeight ();
  Nlm_stdCharWidth = Nlm_MaxCharWidth ();
  SysEnvirons (1, &sysenv);
  hasColorQD = sysenv.hasColorQD;
  if (hasColorQD) {
    Nlm_RGBforeColor.red = 0;
    Nlm_RGBforeColor.green = 0;
    Nlm_RGBforeColor.blue = 0;
    Nlm_RGBbackColor.red = 65535;
    Nlm_RGBbackColor.green = 65535;
    Nlm_RGBbackColor.blue = 65535;
  }
#endif
#ifdef WIN_MSWIN
  Nlm_RectTool  rtool;

  Nlm_scrollRgn = (Nlm_RegioN) (CreateRectRgn (0, 0, 0, 0));

  Nlm_updateRgn = (Nlm_RegioN) (CreateRectRgn (-32767, -32767, 32767, 32767));
  GetRgnBox ((Nlm_RgnTool) Nlm_updateRgn, &rtool);
  Nlm_RectToolToRecT (&rtool, &Nlm_updateRect);

  Nlm_fontList = NULL;
  Nlm_fontInUse = NULL;
  Nlm_systemFont = (Nlm_FonT) Nlm_HandNew (sizeof (Nlm_FontRec));
  Nlm_LoadFontData (Nlm_systemFont, NULL, "", 0, 0, FALSE, FALSE, FALSE,
                    0, 0, GetStockObject (SYSTEM_FONT), NULL);
  Nlm_programFont = (Nlm_FonT) Nlm_HandNew (sizeof (Nlm_FontRec));
  Nlm_LoadFontData (Nlm_programFont, NULL, "", 0, 0, FALSE, FALSE, FALSE,
                    0, 0, GetStockObject (ANSI_FIXED_FONT), NULL);
  Nlm_fontList = NULL;
  Nlm_fontInUse = Nlm_systemFont;

  Nlm_stdAscent = Nlm_Ascent ();
  Nlm_stdDescent = Nlm_Descent ();
  Nlm_stdLeading = Nlm_Leading ();
  Nlm_stdFontHeight = Nlm_FontHeight ();
  Nlm_stdLineHeight = Nlm_LineHeight ();
  Nlm_stdCharWidth = Nlm_MaxCharWidth ();

  blackColor = RGB (0, 0, 0);
  redColor = RGB (255, 0, 0);
  greenColor = RGB (0, 255, 0);
  blueColor = RGB (0, 0, 255);
  cyanColor = RGB (0, 255, 255);
  magentaColor = RGB (255, 0, 255);
  yellowColor = RGB (255, 255, 0);
  whiteColor = RGB (255, 255, 255);

  hBlackPen = GetStockObject (BLACK_PEN);
  hNullPen = GetStockObject (NULL_PEN);
  hWhitePen = GetStockObject (WHITE_PEN);
  hBlackBrush = GetStockObject (BLACK_BRUSH);
  hDkGrayBrush = GetStockObject (DKGRAY_BRUSH);
  hGrayBrush = GetStockObject (GRAY_BRUSH);
  hHollowBrush = GetStockObject (HOLLOW_BRUSH);
  hLtGrayBrush = GetStockObject (LTGRAY_BRUSH);
  hNullBrush = GetStockObject (NULL_BRUSH);
  hWhiteBrush = GetStockObject (WHITE_BRUSH);
  hAnsiFixedFont = GetStockObject (ANSI_FIXED_FONT);
  hAnsiVarFont = GetStockObject (ANSI_VAR_FONT);
  hDeviceDefaultFont = GetStockObject (DEVICE_DEFAULT_FONT);
  hOemFixedFont = GetStockObject (OEM_FIXED_FONT);
  hSystemFont = GetStockObject (SYSTEM_FONT);
  hSystemFixedFont = GetStockObject (SYSTEM_FIXED_FONT);
#endif
#ifdef WIN_X
  Colormap      colormap;
  XFontStruct   *f;
  Nlm_Int2      i;
  Nlm_Uint2     inv;
  Nlm_Int2      j;
  XFontStruct   *p;
  Nlm_RecT      r;
  Nlm_RectTool  rtool;
  Nlm_Uint2     val;

  Nlm_scrollRgn = (Nlm_RegioN) (XCreateRegion ());

  Nlm_updateRgn = (Nlm_RegioN) (XCreateRegion ());
  Nlm_LoadRect (&r, -32767, -32767, 32767, 32767);
  Nlm_RecTToRectTool (&r, &rtool);
  XUnionRectWithRegion (&rtool, (Nlm_RgnTool) Nlm_updateRgn, (Nlm_RgnTool) Nlm_updateRgn);
  Nlm_RectToolToRecT (&rtool, &Nlm_updateRect);

  emptyRgn = XCreateRegion ();

  Nlm_fontList = NULL;
  Nlm_fontInUse = NULL;
  f = Nlm_XLoadQueryFont (Nlm_currentXDisplay, "-*-helvetica-bold-r-*--14-*", TRUE);
  Nlm_systemFont = (Nlm_FonT) Nlm_HandNew (sizeof (Nlm_FontRec));
  Nlm_LoadFontData (Nlm_systemFont, NULL, "", 12, 0, FALSE,
                    FALSE, FALSE, 0, 0, f, NULL);
  p = Nlm_XLoadQueryFont (Nlm_currentXDisplay, "-*-fixed-medium-r-*--13-*", FALSE);
  if (p == NULL) {
    p = Nlm_XLoadQueryFont (Nlm_currentXDisplay, "-*-courier-medium-r-*--12-*", FALSE);
  }
  if (p == NULL) {
    p = Nlm_XLoadQueryFont (Nlm_currentXDisplay, "8x13", FALSE);
  }
  if (p == NULL) {
    p = Nlm_XLoadQueryFont (Nlm_currentXDisplay, "9x15", TRUE);
  }
  Nlm_programFont = (Nlm_FonT) Nlm_HandNew (sizeof (Nlm_FontRec));
  Nlm_LoadFontData (Nlm_programFont, NULL, "", 9, 0, FALSE,
                    FALSE, FALSE, 4, 0, p, NULL);
  Nlm_fontList = NULL;
  Nlm_fontInUse = Nlm_systemFont;

  XSetFont (Nlm_currentXDisplay, Nlm_currentXGC, f->fid);
  currentFont = f;
  Nlm_stdAscent = Nlm_Ascent ();
  Nlm_stdDescent = Nlm_Descent ();
  Nlm_stdLeading = Nlm_Leading ();
  Nlm_stdFontHeight = Nlm_FontHeight ();
  Nlm_stdLineHeight = Nlm_LineHeight ();
  Nlm_stdCharWidth = Nlm_MaxCharWidth ();

  hasColor = FALSE;
  if (Nlm_currentXDisplay != NULL) {
    hasColor = (XDisplayCells (Nlm_currentXDisplay, Nlm_currentXScreen) > 2);
    if (hasColor) {
      colormap = DefaultColormap (Nlm_currentXDisplay, Nlm_currentXScreen);
      whiteColor = WhitePixel (Nlm_currentXDisplay, Nlm_currentXScreen);
      blackColor = BlackPixel (Nlm_currentXDisplay, Nlm_currentXScreen);
      redColor = Nlm_SetupColor (colormap, 255, 0, 0);
      greenColor = Nlm_SetupColor (colormap, 0, 255, 0);
      blueColor = Nlm_SetupColor (colormap, 0, 0, 255);
      cyanColor = Nlm_SetupColor (colormap, 0, 255, 255);
      magentaColor = Nlm_SetupColor (colormap, 255, 0, 255);
      yellowColor = Nlm_SetupColor (colormap, 255, 255, 0);
    } else {
      whiteColor = WhitePixel (Nlm_currentXDisplay, Nlm_currentXScreen);
      blackColor = BlackPixel (Nlm_currentXDisplay, Nlm_currentXScreen);
      redColor = BlackPixel (Nlm_currentXDisplay, Nlm_currentXScreen);
      greenColor = BlackPixel (Nlm_currentXDisplay, Nlm_currentXScreen);
      blueColor = BlackPixel (Nlm_currentXDisplay, Nlm_currentXScreen);
      cyanColor = BlackPixel (Nlm_currentXDisplay, Nlm_currentXScreen);
      magentaColor = BlackPixel (Nlm_currentXDisplay, Nlm_currentXScreen);
      yellowColor = BlackPixel (Nlm_currentXDisplay, Nlm_currentXScreen);
    }
  }
  fontInfo.fid = 0;
  for (i = 0; i < 256; i++) {
    inv = 0;
    val = (Nlm_Uint2) i;
    for (j = 0; j < 8; j++) {
      inv = (inv << 1);
      inv += (val % 2);
      val = (val >> 1);
    }
    flip [i] = inv;
  }

  Nlm_XbackColor = whiteColor;
  Nlm_XforeColor = blackColor;
  Nlm_XOffset = 0;
  Nlm_YOffset = 0;
  Nlm_clpRgn = NULL;
#endif
}

extern void Nlm_CleanUpDrawingTools (void)

{
  Nlm_FonT      f;
  Nlm_FontData  fdata;

  Nlm_ResetDrawingTools ();
#ifdef WIN_MOTIF
  Nlm_GetFontData (Nlm_systemFont, &fdata);
  if (fdata.handle != NULL) {
    XFreeFont (Nlm_currentXDisplay, fdata.handle);
  }
  Nlm_GetFontData (Nlm_programFont, &fdata);
  if (fdata.handle != NULL) {
    XFreeFont (Nlm_currentXDisplay, fdata.handle);
  }
#endif
  Nlm_HandFree (Nlm_systemFont);
  Nlm_HandFree (Nlm_programFont);
  f = Nlm_fontList;
  while (f != NULL) {
    Nlm_GetFontData (f, &fdata);
#ifdef WIN_MSWIN
    if (fdata.handle != NULL) {
      DeleteObject (fdata.handle);
    }
#endif
#ifdef WIN_MOTIF
    if (fdata.handle != NULL) {
      XFreeFont (Nlm_currentXDisplay, fdata.handle);
    }
#endif
    Nlm_HandFree (f);
    f = fdata.next;
  }
#ifdef WIN_MOTIF
  XDestroyRegion (emptyRgn);
#endif
}
