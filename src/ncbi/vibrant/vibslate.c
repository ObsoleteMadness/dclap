/*   vibslate.c
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
* File Name:  vibslate.c
*
* Author:  Jonathan Kans
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.20 $
*
* File Description: 
*       Vibrant slate (universal drawing environment) functions
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#include <vibtypes.h>
#include <vibprocs.h>
#include <vibincld.h>

#ifdef WIN_MAC
#define Nlm_SlateTool Nlm_Handle
#endif

#ifdef WIN_MSWIN
#define Nlm_SlateTool HWND
#endif

#ifdef WIN_MOTIF
#define Nlm_SlateTool Widget
#endif

typedef  struct  Nlm_slatedata {
  Nlm_SlateTool    handle;
  Nlm_BaR          vScrollBar;
  Nlm_BaR          hScrollBar;
  Nlm_Handle       rowHandles;
  Nlm_Handle       colHandles;
  Nlm_Int2         numRows;
  Nlm_Int2         numCols;
  Nlm_Boolean      border;
  Nlm_Int2         barRows;
  Nlm_Int2         vBefore;
  Nlm_Int2         vAfter;
  Nlm_SltScrlProc  vAction;
  Nlm_SltCharProc  keyProc;
  Nlm_Boolean      hasFocus;
} Nlm_SlateData;

typedef  struct  Nlm_paneldata {
  Nlm_PnlClckProc  click;
  Nlm_PnlClckProc  drag;
  Nlm_PnlClckProc  hold;
  Nlm_PnlClckProc  release;
  Nlm_PnlActnProc  draw;
  Nlm_Int2         extra;
  Nlm_PnlActnProc  reset;
  Nlm_PaneL        prev;
} Nlm_PanelData;

/* 
*  Panel data is on top of a slate record to allow simple and
*  autonomous panels (such as repeat buttons) to be implemented
*  as single entities.
*/

typedef  struct  Nlm_panelslaterec {
  Nlm_BoxRec     boxR;
  Nlm_SlateData  slate;
  Nlm_PanelData  panel;
} Nlm_PanelRec, Nlm_SlateRec, PNTR Nlm_PnlPtr, PNTR Nlm_SltPtr;

typedef  struct  Nlm_offsetrec {
  Nlm_Int2  offset;
  Nlm_Int2  height;
} Nlm_OffsetRec, PNTR Nlm_OfsPtr;

typedef struct Nlm_displaydata {
  Nlm_Boolean  active;
  Nlm_Boolean  changed;
  Nlm_VoidPtr  buffer;
  Nlm_Int2     offset;
  Nlm_Int2     visLines;
  Nlm_PoinT    cursorPos;
} Nlm_DisplayData;

typedef struct Nlm_displayrec {
  Nlm_PanelRec     panelR;
  Nlm_DisplayData  display;
} Nlm_DisplayRec, PNTR Nlm_DspPtr;

#define screenBufSize 100

static Nlm_GphPrcsPtr   gphprcsptr = NULL;

static Nlm_GphPrcsPtr   slateProcs;
static Nlm_GphPrcsPtr   panelProcs;
static Nlm_GphPrcsPtr   displayProcs;

static Nlm_SlatE        currentSlate = NULL;

static Nlm_SlatE        recentSlate = NULL;
static Nlm_SlateData    recentSlateData;

static Nlm_SlatE        recentlyClickedSlate = NULL;
static Nlm_Int4         lastClickTime = 0;

static Nlm_PaneL        recentlyClickedPanel = NULL;

static Nlm_DisplaY      recentDisplay = NULL;
static Nlm_DisplayData  recentDisplayData;

static Nlm_Uint2        charCount;
static Nlm_Uint2        screenPos;
static Nlm_Char         screenBfr [screenBufSize];

static Nlm_Int2         disphght;
static Nlm_FonT         dispfont;

#ifdef WIN_MSWIN
static Nlm_Char         slateclass [32];
static Nlm_Boolean      handlechar;
static Nlm_SlateTool    slateTimer;
static Nlm_Boolean      slateTimerUsed = FALSE;
#endif

#ifdef WIN_MOTIF
static XtIntervalId     slateTimer;
static Nlm_Boolean      slateTimerUsed = FALSE;
static Nlm_PoinT        currentMousePos;
#endif

static void Nlm_LoadSlateData (Nlm_SlatE s, Nlm_SlateTool hdl,
                               Nlm_BaR vsb, Nlm_BaR hsb,
                               Nlm_Handle rhdl, Nlm_Handle chdl,
                               Nlm_Int2 nrws, Nlm_Int2 ncls,
                               Nlm_Boolean bdr, Nlm_Int2 barrws,
                               Nlm_Int2 vbfr, Nlm_Int2 vaftr,
                               Nlm_SltScrlProc vact, Nlm_SltCharProc key,
                               Nlm_Boolean fcs)

{
  Nlm_SltPtr     sp;
  Nlm_SlateData  PNTR sptr;

  if (s != NULL) {
    sp = (Nlm_SltPtr) Nlm_HandLock (s);
    sptr = &(sp->slate);
    sptr->handle = hdl;
    sptr->vScrollBar = vsb;
    sptr->hScrollBar = hsb;
    sptr->rowHandles = rhdl;
    sptr->colHandles = chdl;
    sptr->numRows = nrws;
    sptr->numCols = ncls;
    sptr->border = bdr;
    sptr->barRows = barrws;
    sptr->vBefore = vbfr;
    sptr->vAfter = vaftr;
    sptr->vAction = vact;
    sptr->keyProc = key;
    sptr->hasFocus = fcs;
    Nlm_HandUnlock (s);
    recentSlate = NULL;
  }
}

static void Nlm_SetSlateData (Nlm_SlatE s, Nlm_SlateData * sdata)

{
  Nlm_SltPtr  sp;

  if (s != NULL && sdata != NULL) {
    sp = (Nlm_SltPtr) Nlm_HandLock (s);
    sp->slate = *sdata;
    Nlm_HandUnlock (s);
    recentSlate = s;
    recentSlateData = *sdata;
  }
}

static void Nlm_GetSlateData (Nlm_SlatE s, Nlm_SlateData * sdata)

{
  Nlm_SltPtr  sp;

  if (s != NULL && sdata != NULL) {
    if (s == recentSlate && NLM_RISKY) {
      *sdata = recentSlateData;
    } else {
      sp = (Nlm_SltPtr) Nlm_HandLock (s);
      *sdata = sp->slate;
      Nlm_HandUnlock (s);
      recentSlate = s;
      recentSlateData = *sdata;
    }
  }
}

static void Nlm_LoadPanelData (Nlm_PaneL p, Nlm_PnlClckProc clk,
                               Nlm_PnlClckProc drg, Nlm_PnlClckProc hld,
                               Nlm_PnlClckProc rls, Nlm_PnlActnProc drw,
                               Nlm_Int2 xtra, Nlm_PnlActnProc rst,
                               Nlm_PaneL prv)

{
  Nlm_PnlPtr     pp;
  Nlm_PanelData  PNTR pptr;

  if (p != NULL) {
    pp = (Nlm_PnlPtr) Nlm_HandLock (p);
    pptr = &(pp->panel);
    pptr->click = clk;
    pptr->drag = drg;
    pptr->hold = hld;
    pptr->release = rls;
    pptr->draw = drw;
    pptr->extra = xtra;
    pptr->reset = rst;
    pptr->prev = prv;
    Nlm_HandUnlock (p);
  }
}

static void Nlm_SetPanelData (Nlm_PaneL p, Nlm_PanelData * pdata)

{
  Nlm_PnlPtr  pp;

  if (p != NULL && pdata != NULL) {
    pp = (Nlm_PnlPtr) Nlm_HandLock (p);
    pp->panel = *pdata;
    Nlm_HandUnlock (p);
  }
}

static void Nlm_GetPanelData (Nlm_PaneL p, Nlm_PanelData * pdata)

{
  Nlm_PnlPtr  pp;

  if (p != NULL && pdata != NULL) {
    pp = (Nlm_PnlPtr) Nlm_HandLock (p);
    *pdata = pp->panel;
    Nlm_HandUnlock (p);
  }
}

static void Nlm_LoadDisplayData (Nlm_DisplaY d, Nlm_Boolean actv,
                                 Nlm_Boolean chgd, Nlm_VoidPtr bufr,
                                 Nlm_Int2 off, Nlm_Int2 vis,
                                 Nlm_PoinT pos)

{
  Nlm_DisplayData  PNTR ddptr;
  Nlm_DspPtr       dp;

  if (d != NULL) {
    dp = (Nlm_DspPtr) Nlm_HandLock (d);
    ddptr = &(dp->display);
    ddptr->active = actv;
    ddptr->changed = chgd;
    ddptr->buffer = bufr;
    ddptr->offset = off;
    ddptr->visLines = vis;
    ddptr->cursorPos = pos;
    Nlm_HandUnlock (d);
    recentDisplay = NULL;
  }
}

static void Nlm_SetDisplayData (Nlm_DisplaY d, Nlm_DisplayData * ddata)

{
  Nlm_DspPtr  dp;

  if (d != NULL && ddata != NULL) {
    dp = (Nlm_DspPtr) Nlm_HandLock (d);
    dp->display = *ddata;
    Nlm_HandUnlock (d);
    recentDisplay = d;
    recentDisplayData = *ddata;
  }
}

static void Nlm_GetDisplayData (Nlm_DisplaY d, Nlm_DisplayData * ddata)

{
  Nlm_DspPtr  dp;

  if (d != NULL && ddata != NULL) {
    if (d == recentDisplay && NLM_RISKY) {
      *ddata = recentDisplayData;
    } else {
      dp = (Nlm_DspPtr) Nlm_HandLock (d);
      *ddata = dp->display;
      Nlm_HandUnlock (d);
      recentDisplay = d;
      recentDisplayData = *ddata;
    }
  }
}

extern void Nlm_SetPanelExtra (Nlm_PaneL p, Nlm_VoidPtr sptr)

{
  Nlm_BytePtr  dst;
  Nlm_Int2     extra;
  Nlm_PnlPtr   pp;
  Nlm_BytePtr  src;

  if (p != NULL && sptr != NULL) {
    pp = (Nlm_PnlPtr) Nlm_HandLock (p);
    extra = pp->panel.extra;
    dst = (Nlm_BytePtr) pp + sizeof (Nlm_PanelRec);
    src = (Nlm_BytePtr) sptr;
    while (extra > 0) {
      *dst = *src;
      dst++;
      src++;
      extra--;
    }
    Nlm_HandUnlock (p);
  }
}

extern void Nlm_GetPanelExtra (Nlm_PaneL p, Nlm_VoidPtr sptr)

{
  Nlm_BytePtr  dst;
  Nlm_Int2     extra;
  Nlm_PnlPtr   pp;
  Nlm_BytePtr  src;

  if (p != NULL && sptr != NULL) {
    pp = (Nlm_PnlPtr) Nlm_HandLock (p);
    extra = pp->panel.extra;
    src = (Nlm_BytePtr) pp + sizeof (Nlm_PanelRec);
    dst = (Nlm_BytePtr) sptr;
    while (extra > 0) {
      *dst = *src;
      dst++;
      src++;
      extra--;
    }
    Nlm_HandUnlock (p);
  }
}

static Nlm_SlateTool Nlm_GetSlateHandle (Nlm_SlatE s)

{
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  return sdata.handle;
}

extern Nlm_BaR Nlm_GetSlateVScrollBar (Nlm_SlatE s)

{
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  return sdata.vScrollBar;
}

extern Nlm_BaR Nlm_GetSlateHScrollBar (Nlm_SlatE s)

{
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  return sdata.hScrollBar;
}

static Nlm_Handle Nlm_GetSlateRowHandles (Nlm_SlatE s)

{
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  return sdata.rowHandles;
}

static Nlm_Handle Nlm_GetSlateColHandles (Nlm_SlatE s)

{
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  return sdata.colHandles;
}

static Nlm_Int2 Nlm_GetSlateNumRows (Nlm_SlatE s)

{
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  return sdata.numRows;
}

static Nlm_Int2 Nlm_GetSlateNumCols (Nlm_SlatE s)

{
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  return sdata.numCols;
}

static Nlm_Boolean Nlm_GetSlateBorder (Nlm_SlatE s)

{
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  return sdata.border;
}

/* dgg -- lets turn off that border sometimes */

extern void Nlm_SetSlateBorder (Nlm_SlatE s, Nlm_Boolean turnon)

{
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  sdata.border = turnon;
  Nlm_SetSlateData (s, &sdata);
}

static Nlm_SltCharProc Nlm_GetSlateCharProc (Nlm_SlatE s)

{
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  return sdata.keyProc;
}

extern void Nlm_SetSlateChar (Nlm_SlatE s, Nlm_SltCharProc chr)

{
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  sdata.keyProc = chr;
  Nlm_SetSlateData (s, &sdata);
}

static Nlm_Boolean Nlm_SlateHasFocus (Nlm_SlatE s)

{
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  return sdata.hasFocus;
}

static void Nlm_ChangeSlateFocus (Nlm_SlatE s, Nlm_Boolean fcs)

{
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  sdata.hasFocus = fcs;
  Nlm_SetSlateData (s, &sdata);
}

static Nlm_PnlClckProc Nlm_GetPanelClick (Nlm_PaneL p)

{
  Nlm_PanelData  pdata;

  Nlm_GetPanelData (p, &pdata);
  return pdata.click;
}

static Nlm_PnlClckProc Nlm_GetPanelDrag (Nlm_PaneL p)

{
  Nlm_PanelData  pdata;

  Nlm_GetPanelData (p, &pdata);
  return pdata.drag;
}

static Nlm_PnlClckProc Nlm_GetPanelHold (Nlm_PaneL p)

{
  Nlm_PanelData  pdata;

  Nlm_GetPanelData (p, &pdata);
  return pdata.hold;
}

static Nlm_PnlClckProc Nlm_GetPanelRelease (Nlm_PaneL p)

{
  Nlm_PanelData  pdata;

  Nlm_GetPanelData (p, &pdata);
  return pdata.release;
}

static Nlm_PnlActnProc Nlm_GetPanelDraw (Nlm_PaneL p)

{
  Nlm_PanelData  pdata;

  Nlm_GetPanelData (p, &pdata);
  return pdata.draw;
}

static void Nlm_SetPanelPrev (Nlm_PaneL p, Nlm_PaneL prv)

{
  Nlm_PanelData  pdata;

  Nlm_GetPanelData (p, &pdata);
  pdata.prev = prv;
  Nlm_SetPanelData (p, &pdata);
}

static Nlm_PaneL Nlm_GetPanelPrev (Nlm_PaneL p)

{
  Nlm_PanelData  pdata;

  Nlm_GetPanelData (p, &pdata);
  return pdata.prev;
}

static Nlm_Int2 Nlm_GetDisplayVisLines (Nlm_DisplaY d)

{
  Nlm_DisplayData  ddata;

  Nlm_GetDisplayData (d, &ddata);
  return ddata.visLines;
}

static void Nlm_SetCursorPos (Nlm_DisplaY d, Nlm_PoinT pt)

{
  Nlm_DisplayData  ddata;

  Nlm_GetDisplayData (d, &ddata);
  ddata.cursorPos = pt;
  Nlm_SetDisplayData (d, &ddata);
}

static void Nlm_GetCursorPos (Nlm_DisplaY d, Nlm_PointPtr pt)

{
  Nlm_DisplayData  ddata;

  Nlm_GetDisplayData (d, &ddata);
  *pt = ddata.cursorPos;
}

static Nlm_PaneL Nlm_CreateReverseLinks (Nlm_SlatE s)

{
  Nlm_PaneL  last;
  Nlm_PaneL  n;
  Nlm_PaneL  p;

  p = (Nlm_PaneL) Nlm_GetChild ((Nlm_GraphiC) s);
  Nlm_SetPanelPrev (p, NULL);
  last = p;
  while (p != NULL) {
    n = (Nlm_PaneL) Nlm_GetNext ((Nlm_GraphiC) p);
    if (n != NULL) {
      Nlm_SetPanelPrev (n, p);
      last = n;
    }
    p = n;
  }
  return last;
}

#ifdef WIN_MAC
static Nlm_Boolean Nlm_SlateClick (Nlm_GraphiC s, Nlm_PoinT pt)

{
  Nlm_PnlClckProc  clk;
  Nlm_Int4         delta;
  Nlm_PnlClckProc  drg;
  Nlm_Int2         hOffset;
  Nlm_BaR          hsb;
  Nlm_PoinT        mpt;
  Nlm_PaneL        n;
  Nlm_Int4         newtime;
  Nlm_Boolean      notInside;
  Nlm_Int4         oldtime;
  Nlm_PoinT        opt;
  Nlm_PaneL        p;
  Nlm_PanelData    pdata;
  Nlm_PnlClckProc  prs;
  Nlm_RecT         r;
  Nlm_PnlClckProc  rls;
  Nlm_Boolean      rsult;
  Nlm_RecT         sr;
  Nlm_Int4         ticks;
  Nlm_Int2         vOffset;
  Nlm_BaR          vsb;

  ticks = Nlm_ComputerTime ();
  rsult = FALSE;
  Nlm_GetRect (s, &r);
  Nlm_GetRect (s, &sr);
  vsb = Nlm_GetSlateVScrollBar ((Nlm_SlatE) s);
  hsb = Nlm_GetSlateHScrollBar ((Nlm_SlatE) s);
  if (vsb != NULL) {
    r.right += Nlm_vScrollBarWidth;
  }
  if (hsb != NULL) {
    r.bottom += Nlm_hScrollBarHeight;
  }
  if (Nlm_PtInRect (pt, &r)) {
    if ((Nlm_SlatE) s == recentlyClickedSlate) {
      delta = ticks - lastClickTime;
      if (delta <= GetDblTime ()) {
        Nlm_dblClick = TRUE;
      }
    }
    lastClickTime = ticks;
    recentlyClickedSlate = (Nlm_SlatE) s;
    recentlyClickedPanel = NULL;
    if (vsb != NULL) {
      r.right -= Nlm_vScrollBarWidth;
    }
    if (hsb != NULL) {
      r.bottom -= Nlm_hScrollBarHeight;
    }
    if (vsb != NULL && Nlm_DoClick ((Nlm_GraphiC) vsb, pt)) {
      Nlm_dblClick = FALSE;
      recentlyClickedSlate = NULL;
    } else if (hsb != NULL && Nlm_DoClick ((Nlm_GraphiC) hsb, pt)) {
      Nlm_dblClick = FALSE;
      recentlyClickedSlate = NULL;
    } else if (Nlm_PtInRect (pt, &r)) {
      Nlm_SelectFont (Nlm_systemFont);
      Nlm_ResetDrawingTools ();
      Nlm_GetPanelData ((Nlm_PaneL) s, &pdata);
      if (pdata.click != NULL || pdata.drag != NULL || pdata.hold != NULL ||
          pdata.release != NULL || pdata.draw != NULL || pdata.reset != NULL) {
        p = (Nlm_PaneL) s;
        recentlyClickedPanel = p;
        clk = Nlm_GetPanelClick (p);
        if (clk != NULL && Nlm_GetEnabled ((Nlm_GraphiC) p) && Nlm_GetVisible ((Nlm_GraphiC) p)) {
          Nlm_ClipRect (&sr);
          clk (p, pt);
          Nlm_Update ();
        }
      } else {
        p = Nlm_CreateReverseLinks ((Nlm_SlatE) s);
        notInside = TRUE;
        while (p != NULL && notInside) {
          n = Nlm_GetPanelPrev (p);
          Nlm_GetPanelData (p, &pdata);
          if (pdata.click != NULL || pdata.drag != NULL ||
              pdata.hold != NULL || pdata.release != NULL) {
            Nlm_GetRect ((Nlm_GraphiC) p, &r);
            Nlm_DoGetOffset ((Nlm_GraphiC) p, &hOffset, &vOffset);
            Nlm_OffsetRect (&r, -hOffset, -vOffset);
            if (Nlm_PtInRect (pt, &r) && Nlm_GetEnabled ((Nlm_GraphiC) p) &&
                Nlm_GetVisible ((Nlm_GraphiC) p)) {
              recentlyClickedPanel = p;
              notInside = FALSE;
              clk = Nlm_GetPanelClick (p);
              if (clk != NULL) {
                Nlm_ClipRect (&sr);
                clk (p, pt);
                Nlm_Update ();
              }
            }
          }
          p = n;
        }
      }
      if (recentlyClickedPanel != NULL) {
        p = recentlyClickedPanel;
        mpt = pt;
        oldtime = Nlm_ComputerTime ();
        drg = Nlm_GetPanelDrag (p);
        prs = Nlm_GetPanelHold (p);
        while (StillDown () && recentlyClickedPanel != NULL) {
          opt = mpt;
          newtime = Nlm_ComputerTime ();
          Nlm_MousePosition (&mpt);
          if (Nlm_EqualPt (opt, mpt)) {
            if (prs != NULL && newtime >= oldtime + 6) {
              Nlm_ResetDrawingTools ();
              Nlm_ClipRect (&sr);
              prs (p, mpt);
              oldtime += 6;
            }
          } else {
            if (drg != NULL) {
              Nlm_ResetDrawingTools ();
              Nlm_ClipRect (&sr);
              drg (p, mpt);
            }
          }
          Nlm_Update ();
          Nlm_ProcessTimerEvent ();
        }
        rls = Nlm_GetPanelRelease (p);
        if (rls != NULL && recentlyClickedPanel != NULL) {
          Nlm_MousePosition (&mpt);
          Nlm_ResetDrawingTools ();
          Nlm_ClipRect (&sr);
          rls (p, mpt);
        }
      }
      Nlm_SelectFont (Nlm_systemFont);
      Nlm_ResetDrawingTools ();
      Nlm_ResetClip ();
      rsult = TRUE;
    }
  }
  return rsult;
}

static Nlm_Boolean Nlm_SlateKey (Nlm_GraphiC s, Nlm_Char ch)

{
  Nlm_Boolean      act;
  Nlm_SltCharProc  keyProc;
  Nlm_Boolean      rsult;

  rsult = FALSE;
  act = Nlm_SlateHasFocus ((Nlm_SlatE) s);
  if (act && ! Nlm_cmmdKey) {
    if (ch != '\0') {
      keyProc = Nlm_GetSlateCharProc ((Nlm_SlatE) s);
      if (keyProc != NULL) {
        keyProc ((Nlm_SlatE) s, ch);
        rsult = TRUE;
      }
    }
  }
  return rsult;
}
#endif

#ifdef WIN_MSWIN
static Nlm_Boolean Nlm_SlateCommand (Nlm_GraphiC s)

{
  return FALSE;
}
#endif

#ifdef WIN_MOTIF
static void Nlm_SlateCallback (Nlm_GraphiC s)

{
}
#endif

#ifdef WIN_MAC
static void Nlm_DrawSlate (Nlm_GraphiC s)

{
  Nlm_PnlActnProc  drw;
  Nlm_BaR          hsb;
  Nlm_PaneL        n;
  Nlm_PaneL        p;
  Nlm_PanelData    pdata;
  Nlm_RecT         r;
  Nlm_BaR          vsb;

  if (Nlm_GetVisible (s) && Nlm_GetAllParentsVisible (s)) {
    Nlm_GetRect (s, &r);
    vsb = Nlm_GetSlateVScrollBar ((Nlm_SlatE) s);
    hsb = Nlm_GetSlateHScrollBar ((Nlm_SlatE) s);
    if (vsb != NULL) {
      r.right += Nlm_vScrollBarWidth;
    }
    if (hsb != NULL) {
      r.bottom += Nlm_hScrollBarHeight;
    }
    if (Nlm_RectInRgn (&r, Nlm_updateRgn)) {
      if (vsb != NULL) {
        Nlm_DoDraw ((Nlm_GraphiC) vsb);
      }
      if (hsb != NULL) {
        Nlm_DoDraw ((Nlm_GraphiC) hsb);
      }
      Nlm_SelectFont (Nlm_systemFont);
      Nlm_ResetDrawingTools ();
      if (Nlm_GetSlateBorder ((Nlm_SlatE) s)) {
        Nlm_FrameRect (&r);
      }
      Nlm_GetRect (s, &r);
      Nlm_GetPanelData ((Nlm_PaneL) s, &pdata);
      if (pdata.click != NULL || pdata.drag != NULL || pdata.hold != NULL ||
          pdata.release != NULL || pdata.draw != NULL || pdata.reset != NULL) {
        p = (Nlm_PaneL) s;
/*
        recentlyClickedPanel = p;
*/
        drw = Nlm_GetPanelDraw (p);
        if (drw != NULL && Nlm_GetVisible ((Nlm_GraphiC) p)) {
          Nlm_ClipRect (&r);
          drw (p);
          Nlm_ResetDrawingTools ();
        }
      } else {
        p = (Nlm_PaneL) Nlm_GetChild (s);
        while (p != NULL) {
          n = (Nlm_PaneL) Nlm_GetNext ((Nlm_GraphiC) p);
          drw = Nlm_GetPanelDraw (p);
          if (drw != NULL && Nlm_GetVisible ((Nlm_GraphiC) p)) {
            Nlm_ClipRect (&r);
            drw (p);
            Nlm_ResetDrawingTools ();
          }
          p = n;
        }
      }
      Nlm_ResetClip ();
    }
  }
}
#endif

static void Nlm_SlateVertScrollAction (Nlm_BaR sb, Nlm_SlatE s,
                                       Nlm_Int2 newval, Nlm_Int2 oldval)

{
  Nlm_SltScrlProc  actn;
  Nlm_Int2         barRows;
  Nlm_Int2         before;
  Nlm_Int2         delta;
  Nlm_Int2         height;
  Nlm_Int2         i;
  Nlm_Int2         last;
  Nlm_Int2         limit;
  Nlm_Int2         numRows;
  Nlm_Int2         oldPgDn;
  Nlm_OfsPtr       optr;
  Nlm_Int2         pgDn;
  Nlm_Int2         pgUp;
  Nlm_RecT         r;
  Nlm_Handle       rhdl;
  Nlm_SlateData    sdata;

  if (s != NULL && oldval != newval) {
    if (Nlm_GetVisible ((Nlm_GraphiC) s) && Nlm_GetAllParentsVisible ((Nlm_GraphiC) s)) {
      Nlm_GetRect ((Nlm_GraphiC) s, &r);
      Nlm_InsetRect (&r, 4, 4);
      height = r.bottom - r.top;
      Nlm_GetSlateData (s, &sdata);
      numRows = sdata.numRows;
      before = sdata.vBefore;
      barRows = sdata.barRows;
      if (newval >= before && newval <= before + barRows) {
        newval -= before;
        oldval -= before;
        rhdl = sdata.rowHandles;
        if (rhdl != NULL) {
          optr = (Nlm_OfsPtr) Nlm_HandLock (rhdl);
          i = newval + 1;
          pgDn = 0;
          while (i < numRows && optr [i].offset + optr [i].height -
                 optr [newval].offset <= height) {
            i++;
            pgDn++;
          }
          i = newval - 1;
          pgUp = 0;
          while (i >= 0 && optr [newval].offset + optr [newval].height -
                 optr [i].offset <= height) {
            i--;
            pgUp++;
          }
          delta = optr [oldval].offset - optr [newval].offset;
          if (delta < 0) {
            i = oldval + 1;
            oldPgDn = 0;
            while (i < numRows && optr [i].offset + optr [i].height -
                   optr [oldval].offset <= height) {
              i++;
              oldPgDn++;
            }
            last = oldval + oldPgDn;
            limit = r.top + optr [last].offset + optr [last].height  - optr [newval].offset;
          } else if (delta > 0) {
            last = newval + pgDn;
            limit = r.top + optr [last].offset + optr [last].height  - optr [newval].offset;
          }
          Nlm_HandUnlock (rhdl);
          Nlm_DoSelect ((Nlm_GraphiC) s, FALSE);
          if (delta < 0 && -delta < height) {
            Nlm_ScrollRect (&r, 0, delta);
            r.top = limit;
            Nlm_InsetRect (&r, -1, -1);
            Nlm_InvalRect (&r);
          } else if (delta > 0 && delta < height) {
            Nlm_ScrollRect (&r, 0, delta);
            r.top = limit;
            Nlm_InsetRect (&r, -1, -1);
            Nlm_InvalRect (&r);
          } else {
            Nlm_InsetRect (&r, -1, -1);
            Nlm_InvalRect (&r);
          }
          if (pgUp < 1) {
            pgUp = 1;
          }
          if (pgDn < 1) {
            pgDn = 1;
          }
          Nlm_DoSetRange ((Nlm_GraphiC) sb, pgUp, pgDn, 0, FALSE);
          Nlm_Update ();
        }
      } else {
        actn = sdata.vAction;
        if (actn != NULL) {
          actn (sb, s, newval, oldval);
        }
      }
    }
  }
}

static void Nlm_SlateHorizScrollAction (Nlm_BaR sb, Nlm_SlatE s,
                                        Nlm_Int2 newval, Nlm_Int2 oldval)

{
}

static void Nlm_ShowSlate (Nlm_GraphiC s, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_SlateTool  h;
  Nlm_BaR        hsb;
  Nlm_WindoW     tempPort;
  Nlm_BaR        vsb;

  if (setFlag) {
    Nlm_SetVisible (s, TRUE);
  }
  if (Nlm_GetVisible (s) && Nlm_AllParentsButWindowVisible (s)) {
    tempPort = Nlm_SavePortIfNeeded (s, savePort);
    vsb = Nlm_GetSlateVScrollBar ((Nlm_SlatE) s);
    hsb = Nlm_GetSlateHScrollBar ((Nlm_SlatE) s);
    if (vsb != NULL) {
      Nlm_DoShow ((Nlm_GraphiC) vsb, TRUE, FALSE);
    }
    if (hsb != NULL) {
      Nlm_DoShow ((Nlm_GraphiC) hsb, TRUE, FALSE);
    }
    h = Nlm_GetSlateHandle ((Nlm_SlatE) s);
#ifdef WIN_MAC
    Nlm_DoDraw (s);
#endif
#ifdef WIN_MSWIN
    ShowWindow (h, SW_SHOW);
    UpdateWindow (h);
#endif
#ifdef WIN_MOTIF
    XtManageChild (h);
#endif
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_ShowPanel (Nlm_GraphiC p, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_RecT    dr;
  Nlm_Int2    hOffset;
  Nlm_RecT    r;
  Nlm_SlatE   s;
  Nlm_RecT    sr;
  Nlm_Int2    vOffset;
  Nlm_WindoW  tempPort;

  if (setFlag) {
    Nlm_SetVisible (p, TRUE);
  }
  if (Nlm_GetVisible (p) && Nlm_AllParentsButWindowVisible (p)) {
    tempPort = Nlm_SavePortIfNeeded (p, savePort);
    Nlm_GetRect (p, &r);
    Nlm_DoGetOffset ((Nlm_GraphiC) p, &hOffset, &vOffset);
    Nlm_OffsetRect (&r, -hOffset, -vOffset);
    s = (Nlm_SlatE) Nlm_GetParent (p);
    Nlm_GetRect ((Nlm_GraphiC) s, &sr);
    Nlm_SectRect (&r, &sr, &dr);
    if (Nlm_RectInRect (&dr, &sr)) {
      Nlm_InsetRect (&dr, -1, -1);
      Nlm_InvalRect (&dr);
    }
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_HideSlate (Nlm_GraphiC s, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_SlateTool  h;
  Nlm_BaR        hsb;
  Nlm_WindoW     tempPort;
  Nlm_BaR        vsb;
#ifdef WIN_MAC
  Nlm_RecT       r;
#endif

  if (setFlag) {
    Nlm_SetVisible (s, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (s, savePort);
  vsb = Nlm_GetSlateVScrollBar ((Nlm_SlatE) s);
  hsb = Nlm_GetSlateHScrollBar ((Nlm_SlatE) s);
  if (vsb != NULL) {
    Nlm_DoHide ((Nlm_GraphiC) vsb, TRUE, FALSE);
  }
  if (hsb != NULL) {
    Nlm_DoHide ((Nlm_GraphiC) hsb, TRUE, FALSE);
  }
  h = Nlm_GetSlateHandle ((Nlm_SlatE) s);
#ifdef WIN_MAC
  if (Nlm_AllParentsButWindowVisible (s)) {
    Nlm_GetRect (s, &r);
    r.right += Nlm_vScrollBarWidth;
    r.bottom += Nlm_hScrollBarHeight;
    Nlm_InsetRect (&r, -1, -1);
    Nlm_EraseRect (&r);
    Nlm_ValidRect (&r);
  }
#endif
#ifdef WIN_MSWIN
  ShowWindow (h, SW_HIDE);
  UpdateWindow (h);
#endif
#ifdef WIN_MOTIF
  XtUnmanageChild (h);
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_HidePanel (Nlm_GraphiC p, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_RecT    dr;
  Nlm_Int2    hOffset;
  Nlm_RecT    r;
  Nlm_SlatE   s;
  Nlm_RecT    sr;
  Nlm_Int2    vOffset;
  Nlm_WindoW  tempPort;

  if (setFlag) {
    Nlm_SetVisible (p, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (p, savePort);
  if (Nlm_AllParentsButWindowVisible (p)) {
    Nlm_GetRect (p, &r);
    Nlm_DoGetOffset ((Nlm_GraphiC) p, &hOffset, &vOffset);
    Nlm_OffsetRect (&r, -hOffset, -vOffset);
    s = (Nlm_SlatE) Nlm_GetParent (p);
    Nlm_GetRect ((Nlm_GraphiC) s, &sr);
    Nlm_SectRect (&r, &sr, &dr);
    if (Nlm_RectInRect (&dr, &sr)) {
      Nlm_InsetRect (&dr, -1, -1);
      Nlm_InvalRect (&dr);
    }
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_EnableSlate (Nlm_GraphiC s, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_SlateTool  h;
  Nlm_BaR        hsb;
  Nlm_RecT       r;
  Nlm_WindoW     tempPort;
  Nlm_BaR        vsb;

  if (setFlag) {
    Nlm_SetEnabled (s, TRUE);
  }
  if (Nlm_GetEnabled (s) && Nlm_GetAllParentsEnabled (s)) {
    tempPort = Nlm_SavePortIfNeeded (s, savePort);
    vsb = Nlm_GetSlateVScrollBar ((Nlm_SlatE) s);
    hsb = Nlm_GetSlateHScrollBar ((Nlm_SlatE) s);
    if (vsb != NULL) {
      Nlm_DoEnable ((Nlm_GraphiC) vsb, TRUE, FALSE);
    }
    if (hsb != NULL) {
      Nlm_DoEnable ((Nlm_GraphiC) hsb, TRUE, FALSE);
    }
    h = Nlm_GetSlateHandle ((Nlm_SlatE) s);
#ifdef WIN_MSWIN
    EnableWindow (h, TRUE);
#endif
#ifdef WIN_MOTIF
    /*
    XtVaSetValues (h, XmNsensitive, TRUE, NULL);
    */
#endif
    if (setFlag) {
      if (Nlm_GetVisible (s) && Nlm_GetAllParentsVisible (s)) {
        Nlm_DoSelect ((Nlm_GraphiC) s, FALSE);
        Nlm_GetRect (s, &r);
        Nlm_InsetRect (&r, 1, 1);
        Nlm_InvalRect (&r);
      }
    }
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_EnablePanel (Nlm_GraphiC p, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_SlatE  s;

  if (setFlag) {
    Nlm_SetEnabled (p, TRUE);
  }
  s = (Nlm_SlatE) Nlm_GetParent (p);
  Nlm_DoEnable ((Nlm_GraphiC) s, TRUE, savePort);
}

static void Nlm_DisableSlate (Nlm_GraphiC s, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_SlateTool  h;
  Nlm_BaR        hsb;
  Nlm_WindoW     tempPort;
  Nlm_BaR        vsb;

  if (setFlag) {
    Nlm_SetEnabled (s, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (s, savePort);
  vsb = Nlm_GetSlateVScrollBar ((Nlm_SlatE) s);
  hsb = Nlm_GetSlateHScrollBar ((Nlm_SlatE) s);
  if (vsb != NULL) {
    Nlm_DoDisable ((Nlm_GraphiC) vsb, TRUE, FALSE);
  }
  if (hsb != NULL) {
    Nlm_DoDisable ((Nlm_GraphiC) hsb, TRUE, FALSE);
  }
  h = Nlm_GetSlateHandle ((Nlm_SlatE) s);
#ifdef WIN_MSWIN
  EnableWindow (h, FALSE);
#endif
#ifdef WIN_MOTIF
  /*
  XtVaSetValues (h, XmNsensitive, FALSE, NULL);
  */
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_DisablePanel (Nlm_GraphiC p, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_SlatE  s;

  if (setFlag) {
    Nlm_SetEnabled (p, FALSE);
  }
  s = (Nlm_SlatE) Nlm_GetParent (p);
  Nlm_DoDisable ((Nlm_GraphiC) s, TRUE, savePort);
}

static void Nlm_ResetSlate (Nlm_GraphiC s, Nlm_Boolean savePort)

{
  Nlm_Handle       chdl;
  Nlm_BaR          hsb;
  Nlm_PaneL        n;
  Nlm_PoinT        npt;
  Nlm_PaneL        p;
  Nlm_PanelData    pdata;
  Nlm_RecT         r;
  Nlm_Handle       rhdl;
  Nlm_PnlActnProc  rst;
  Nlm_SlateData    sdata;
  Nlm_WindoW       tempPort;
  Nlm_BaR          vsb;

  tempPort = Nlm_SavePortIfNeeded (s, savePort);
  Nlm_GetSlateData ((Nlm_SlatE) s, &sdata);
  rhdl = sdata.rowHandles;
  if (rhdl != NULL) {
    Nlm_HandFree (rhdl);
  }
  chdl = sdata.colHandles;
  if (chdl != NULL) {
    Nlm_HandFree (chdl);
  }
  sdata.rowHandles = NULL;
  sdata.colHandles = NULL;
  sdata.numRows = 0;
  sdata.numCols = 0;
  sdata.barRows = 0;
  sdata.vBefore = 0;
  sdata.vAfter = 0;
  sdata.vAction = NULL;
  Nlm_SetSlateData ((Nlm_SlatE) s, &sdata);
  vsb = sdata.vScrollBar;
  hsb = sdata.hScrollBar;
  if (vsb != NULL) {
    Nlm_DoReset ((Nlm_GraphiC) vsb, FALSE);
  }
  if (hsb != NULL) {
    Nlm_DoReset ((Nlm_GraphiC) hsb, FALSE);
  }
  Nlm_GetRect (s, &r);
  Nlm_InsetRect (&r, 4, 4);
  Nlm_LoadPt (&npt, r.left, r.top);
  Nlm_LoadBoxData ((Nlm_BoX) s, npt, npt, npt, npt.y, npt.x, 0, 0, 0, 0, 0, 0);
  Nlm_GetPanelData ((Nlm_PaneL) s, &pdata);
  if (pdata.click != NULL || pdata.drag != NULL || pdata.hold != NULL ||
      pdata.release != NULL || pdata.draw != NULL || pdata.reset != NULL) {
    p = (Nlm_PaneL) s;
    Nlm_GetRect ((Nlm_GraphiC) p, &r);
    Nlm_LoadPt (&npt, r.left, r.top);
    Nlm_LoadBoxData ((Nlm_BoX) p, npt, npt, npt, npt.y, npt.x, 0, 0, 0, 0, 0, 0);
    rst = pdata.reset;
    if (rst != NULL) {
      rst (p);
    }
  } else {
    p = (Nlm_PaneL) Nlm_GetChild (s);
    while (p != NULL) {
      n = (Nlm_PaneL) Nlm_GetNext ((Nlm_GraphiC) p);
      Nlm_DoReset ((Nlm_GraphiC) p, FALSE);
      p = n;
    }
  }
  if (Nlm_GetVisible (s) && Nlm_GetAllParentsVisible (s) && Nlm_GetEnabled (s)) {
    Nlm_DoSelect (s, FALSE);
    Nlm_GetRect (s, &r);
    Nlm_InsetRect (&r, 1, 1);
    Nlm_EraseRect (&r);
    Nlm_ValidRect (&r);
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_ResetPanel (Nlm_GraphiC p, Nlm_Boolean savePort)

{
  Nlm_PoinT        npt;
  Nlm_PanelData    pdata;
  Nlm_RecT         r;
  Nlm_PnlActnProc  rst;
  Nlm_WindoW       tempPort;

  tempPort = Nlm_SavePortIfNeeded (p, savePort);
  Nlm_GetRect (p, &r);
  Nlm_LoadPt (&npt, r.left, r.top);
  Nlm_LoadBoxData ((Nlm_BoX) p, npt, npt, npt, npt.y, npt.x, 0, 0, 0, 0, 0, 0);
  Nlm_GetPanelData ((Nlm_PaneL) p, &pdata);
  rst = pdata.reset;
  if (rst != NULL) {
    rst ((Nlm_PaneL) p);
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_RemoveSlate (Nlm_GraphiC s, Nlm_Boolean savePort)

{
  Nlm_Handle       chdl;
  Nlm_SlateTool    h;
  Nlm_BaR          hsb;
  Nlm_PaneL        n;
  Nlm_PaneL        p;
  Nlm_PanelData    pdata;
  Nlm_Handle       rhdl;
  Nlm_PnlActnProc  rst;
  Nlm_SlateData    sdata;
  Nlm_WindoW       tempPort;
  Nlm_BaR          vsb;

  recentSlate = NULL;
  recentlyClickedSlate = NULL;
  recentlyClickedPanel = NULL;
#ifdef WIN_MOTIF
  if (Nlm_currentXDisplay != NULL) {
    XSync (Nlm_currentXDisplay, FALSE);
  }
  if (slateTimerUsed) {
    slateTimerUsed = FALSE;
    XtRemoveTimeOut (slateTimer);
  }
  if (Nlm_currentXDisplay != NULL) {
    XSync (Nlm_currentXDisplay, FALSE);
  }
#endif
  tempPort = Nlm_SavePortIfNeeded (s, savePort);
  Nlm_GetSlateData ((Nlm_SlatE) s, &sdata);
  rhdl = sdata.rowHandles;
  if (rhdl != NULL) {
    Nlm_HandFree (rhdl);
  }
  chdl = sdata.colHandles;
  if (chdl != NULL) {
    Nlm_HandFree (chdl);
  }
  vsb = sdata.vScrollBar;
  hsb = sdata.hScrollBar;
  if (vsb != NULL) {
    Nlm_DoRemove ((Nlm_GraphiC) vsb, FALSE);
  }
  if (hsb != NULL) {
    Nlm_DoRemove ((Nlm_GraphiC) hsb, FALSE);
  }
  Nlm_GetPanelData ((Nlm_PaneL) s, &pdata);
  if (pdata.click != NULL || pdata.drag != NULL || pdata.hold != NULL ||
      pdata.release != NULL || pdata.draw != NULL || pdata.reset != NULL) {
    p = (Nlm_PaneL) s;
    rst = pdata.reset;
    if (rst != NULL) {
      rst (p);
    }
  } else {
    p = (Nlm_PaneL) Nlm_GetChild (s);
    while (p != NULL) {
      n = (Nlm_PaneL) Nlm_GetNext ((Nlm_GraphiC) p);
      Nlm_DoReset ((Nlm_GraphiC) p, FALSE);
      Nlm_DoRemove ((Nlm_GraphiC) p, FALSE);
      p = n;
    }
  }
  h = sdata.handle;
#ifdef WIN_MSWIN
  RemoveProp (h, (LPSTR) "Nlm_VibrantProp");
  DestroyWindow (h);
#endif
#ifdef WIN_MOTIF
  XtDestroyWidget (h);
#endif
  Nlm_RemoveLink (s);
  recentSlate = NULL;
  recentlyClickedSlate = NULL;
  recentlyClickedPanel = NULL;
  Nlm_RestorePort (tempPort);
}

static void Nlm_RemovePanel (Nlm_GraphiC p, Nlm_Boolean savePort)

{
  Nlm_WindoW  tempPort;

  tempPort = Nlm_SavePortIfNeeded (p, savePort);
  Nlm_RemoveLink (p);
  Nlm_RestorePort (tempPort);
}

static void Nlm_SelectSlate (Nlm_GraphiC s, Nlm_Boolean savePort)

{
#ifdef WIN_MOTIF
  Pixel          color;
  Nlm_SlateTool  h;
  Nlm_RecT       r;
  Nlm_WindoW     w;

  w = Nlm_ParentWindow (s);
  if (Nlm_WindowHasBeenShown (w) && (! Nlm_IsWindowDying (w))) {
    h = Nlm_GetSlateHandle ((Nlm_SlatE) s);
    Nlm_GetRect (s, &r);
    Nlm_XOffset = r.left;
    Nlm_YOffset = r.top;
    Nlm_currentXWindow = XtWindow (h);
    XtVaGetValues (h, XmNbackground, &color, 0);
    Nlm_XbackColor = color;
    XtVaGetValues (h, XmNforeground, &color, 0);
    Nlm_XforeColor = color;
  }
#endif
}

static void Nlm_SelectPanel (Nlm_GraphiC p, Nlm_Boolean savePort)

{
#ifdef WIN_MOTIF
  Pixel          color;
  Nlm_SlateTool  h;
  Nlm_RecT       r;
  Nlm_SlatE      s;
  Nlm_WindoW     w;

  w = Nlm_ParentWindow (p);
  if (Nlm_WindowHasBeenShown (w) && (! Nlm_IsWindowDying (w))) {
    s = (Nlm_SlatE) Nlm_GetParent (p);
    h = Nlm_GetSlateHandle ((Nlm_SlatE) s);
    Nlm_GetRect ((Nlm_GraphiC) s, &r);
    Nlm_XOffset = r.left;
    Nlm_YOffset = r.top;
    Nlm_currentXWindow = XtWindow (h);
    XtVaGetValues (h, XmNbackground, &color, 0);
    Nlm_XbackColor = color;
    XtVaGetValues (h, XmNforeground, &color, 0);
    Nlm_XforeColor = color;
  }
#endif
}

static void Nlm_SetSlateOffset (Nlm_GraphiC s, Nlm_Int2 horiz,
                                Nlm_Int2 vert, Nlm_Boolean savePort)

{
  Nlm_BaR        hsb;
  Nlm_SlateData  sdata;
  Nlm_WindoW     tempPort;
  Nlm_BaR        vsb;

  tempPort = Nlm_SavePortIfNeeded (s, savePort);
  Nlm_GetSlateData ((Nlm_SlatE) s, &sdata);
  /*
  if (vert > sdata.barRows) {
    vert = sdata.barRows;
  }
  */
  vsb = sdata.vScrollBar;
  hsb = sdata.hScrollBar;
  if (vsb != NULL) {
    Nlm_DoSetValue ((Nlm_GraphiC) vsb, vert + sdata.vBefore, FALSE);
  }
  if (hsb != NULL) {
    Nlm_DoSetValue ((Nlm_GraphiC) hsb, horiz, FALSE);
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_GetSlateOffset (Nlm_GraphiC s, Nlm_Int2Ptr horiz, Nlm_Int2Ptr vert)

{
  Nlm_BaR        hsb;
  Nlm_SlateData  sdata;
  Nlm_BaR        vsb;

  Nlm_GetSlateData ((Nlm_SlatE) s, &sdata);
  if (vert != NULL) {
    vsb = sdata.vScrollBar;
    if (vsb != NULL) {
      *vert = Nlm_DoGetValue ((Nlm_GraphiC) vsb) - sdata.vBefore;
    } else {
      *vert = 0;
    }
  }
  if (horiz != NULL) {
    hsb = sdata.hScrollBar;
    if (hsb != NULL) {
      *horiz = Nlm_DoGetValue ((Nlm_GraphiC) hsb);
    } else {
      *horiz = 0;
    }
  }
}

static void Nlm_GetPanelOffset (Nlm_GraphiC p,  Nlm_Int2Ptr horiz, Nlm_Int2Ptr vert)

{
  Nlm_Int2       hOff;
  Nlm_Int2       numRows;
  Nlm_OfsPtr     optr;
  Nlm_Handle     rhdl;
  Nlm_SlatE      s;
  Nlm_SlateData  sdata;
  Nlm_Int2       vOff;

  s = (Nlm_SlatE) Nlm_GetParent (p);
  Nlm_DoGetOffset ((Nlm_GraphiC) s, &hOff, &vOff);
  if (vert != NULL) {
    Nlm_GetSlateData (s, &sdata);
    numRows = sdata.numRows;
    rhdl = sdata.rowHandles;
    if (rhdl != NULL && vOff > 0 && vOff < numRows) {
      optr = (Nlm_OfsPtr) Nlm_HandLock (rhdl);
      *vert = optr [vOff].offset;
      Nlm_HandUnlock (rhdl);
    } else {
      *vert = 0;
    }
  }
  if (horiz != NULL) {
    *horiz = 0;
  }
}

static void Nlm_InvalSlate (Nlm_GraphiC s)

{
  Nlm_BaR   hsb;
  Nlm_RecT  r;
  Nlm_BaR   vsb;

  if (Nlm_GetVisible (s) && Nlm_GetAllParentsVisible (s)) {
    Nlm_GetRect (s, &r);
    vsb = Nlm_GetSlateVScrollBar ((Nlm_SlatE) s);
    hsb = Nlm_GetSlateHScrollBar ((Nlm_SlatE) s);
    if (vsb != NULL) {
      r.right += Nlm_vScrollBarWidth;
    }
    if (hsb != NULL) {
      r.bottom += Nlm_hScrollBarHeight;
    }
    Nlm_InsetRect (&r, -1, -1);
    Nlm_InvalRect (&r);
  }
}

static void Nlm_SetSlatePosition (Nlm_GraphiC s, Nlm_RectPtr r, Nlm_Boolean savePort)

{
  Nlm_Int2       deltaX;
  Nlm_Int2       deltaY;
  Nlm_SlateTool  h;
  Nlm_BaR        hsb;
  Nlm_RecT       oldRect;
  Nlm_PaneL      p;
  Nlm_RecT       pr;
  Nlm_RecT       sr;
  Nlm_WindoW     tempPort;
  Nlm_BaR        vsb;

  if (r != NULL) {
    Nlm_DoGetPosition (s, &oldRect);
    if (! Nlm_EqualRect (r, &oldRect)) {
      tempPort = Nlm_SavePortIfNeeded (s, savePort);
      sr = *r;
      deltaX = sr.left - oldRect.left;
      deltaY = sr.top - oldRect.top;
      vsb = Nlm_GetSlateVScrollBar ((Nlm_SlatE) s);
      hsb = Nlm_GetSlateHScrollBar ((Nlm_SlatE) s);
      h = Nlm_GetSlateHandle ((Nlm_SlatE) s);
#ifdef WIN_MAC
      Nlm_InvalSlate (s);
      if (vsb != NULL) {
        sr.right -= Nlm_vScrollBarWidth;
      }
      if (hsb != NULL) {
        sr.bottom -= Nlm_hScrollBarHeight;
      }
      Nlm_SetRect (s, &sr);
      Nlm_InvalSlate (s);
#endif
#ifdef WIN_MSWIN
      if (vsb != NULL) {
        sr.right -= Nlm_vScrollBarWidth;
      }
      if (hsb != NULL) {
        sr.bottom -= Nlm_hScrollBarHeight;
      }
      Nlm_SetRect (s, &sr);
      if (h != NULL) {
        MoveWindow (h, sr.left, sr.top, sr.right - sr.left, sr.bottom - sr.top, TRUE);
      }
#endif
#ifdef WIN_MOTIF
      XtVaSetValues (h,
                     XmNx, (Position) r->left,
                     XmNy, (Position) r->top,
                     XmNwidth, (Dimension) (r->right - r->left + 1),
                     XmNheight, (Dimension) (r->bottom - r->top + 1), 
                     NULL);
      Nlm_SetRect (s, &sr);
#endif
      p = (Nlm_PaneL) Nlm_GetChild (s);
      while (p != NULL) {
        Nlm_DoGetPosition ((Nlm_GraphiC) p, &pr);
        Nlm_OffsetRect (&pr, deltaX, deltaY);
        Nlm_DoSetPosition ((Nlm_GraphiC) p, &pr, FALSE);
        p = (Nlm_PaneL) Nlm_GetNext ((Nlm_GraphiC) p);
      }
#ifdef WIN_MSWIN
      if (h != NULL) {
        UpdateWindow (h);
      }
#endif
      if (vsb != NULL) {
        Nlm_GetRect (s, &sr);
        sr.left = sr.right;
        sr.right += Nlm_vScrollBarWidth;
        Nlm_DoSetPosition ((Nlm_GraphiC) vsb, &sr, FALSE);
      }
      if (hsb != NULL) {
        Nlm_GetRect (s, &sr);
        sr.top = sr.bottom;
        sr.bottom += Nlm_hScrollBarHeight;
        Nlm_DoSetPosition ((Nlm_GraphiC) hsb, &sr, FALSE);
      }
      Nlm_RestorePort (tempPort);
    }
  }
}

static void Nlm_SetPanelPosition (Nlm_GraphiC p, Nlm_RectPtr r, Nlm_Boolean savePort)

{
  Nlm_RecT    oldRect;
  Nlm_WindoW  tempPort;

  if (r != NULL) {
    Nlm_DoGetPosition (p, &oldRect);
    if (! Nlm_EqualRect (r, &oldRect)) {
      tempPort = Nlm_SavePortIfNeeded (p, savePort);
      Nlm_SetRect (p, r);
      Nlm_RestorePort (tempPort);
    }
  }
}

static void Nlm_GetSlatePosition (Nlm_GraphiC s, Nlm_RectPtr r)

{
  if (r != NULL) {
    Nlm_GetRect (s, r);
    if (Nlm_GetSlateVScrollBar ((Nlm_SlatE) s) != NULL) {
      r->right += Nlm_vScrollBarWidth;
    }
    if (Nlm_GetSlateHScrollBar ((Nlm_SlatE) s) != NULL) {
      r->bottom += Nlm_hScrollBarHeight;
    }
  }
}

static void Nlm_GetPanelPosition (Nlm_GraphiC p, Nlm_RectPtr r)

{
  if (r != NULL) {
    Nlm_GetRect (p, r);
  }
}

static void Nlm_AdjustSlate (Nlm_GraphiC a, Nlm_RectPtr r,
                             Nlm_Boolean align, Nlm_Boolean savePort)

{
  Nlm_SlatE  s;

  s = (Nlm_SlatE) Nlm_GetParent (a);
  Nlm_RecordRect ((Nlm_GraphiC) s, r);
  Nlm_NextPosition ((Nlm_GraphiC) s, r);
}

static void Nlm_SetSlateRange (Nlm_GraphiC s, Nlm_Int2 pgUp,
                               Nlm_Int2 pgDn, Nlm_Int2 max,
                               Nlm_Boolean savePort)

{
  Nlm_BaR        sb;
  Nlm_SlateData  sdata;
  Nlm_WindoW     tempPort;

  tempPort = Nlm_SavePortIfNeeded (s, savePort);
  Nlm_GetSlateData ((Nlm_SlatE) s, &sdata);
  if (max > sdata.barRows) {
    sdata.barRows = max;
    Nlm_SetSlateData ((Nlm_SlatE) s, &sdata);
  }
  sb = sdata.vScrollBar;
  if (sb != NULL) {
    Nlm_DoSetRange ((Nlm_GraphiC) sb, pgUp, pgDn, max, FALSE);
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_NextScreenLine (Nlm_DisplaY d)

{
  Nlm_RecT   p;
  Nlm_PoinT  pt;
  Nlm_RecT   r;

  Nlm_GetCursorPos (d, &pt);
  Nlm_GetRect ((Nlm_GraphiC) d, &r);
  Nlm_InsetRect (&r, 4, 4);
  p = r;
#ifdef WIN_MSWIN
  r.bottom -= disphght;
#endif
  while (pt.y + disphght >= r.bottom) {
    Nlm_ScrollRect (&p, 0, -disphght);
    pt.y -= disphght;
  }
  if (pt.y + disphght >= r.bottom) {
    Nlm_ScrollRect (&p, 0, -disphght);
  } else {
    pt.y += disphght;
  }
  pt.x = r.left;
  Nlm_SetCursorPos (d, pt);
}

static void Nlm_PaintOneCharacter (Nlm_DisplaY d, Nlm_Char ch)

{
  Nlm_PoinT  nx;
  Nlm_PoinT  pt;
  Nlm_RecT   r;

  Nlm_GetCursorPos (d, &pt);
  Nlm_GetRect ((Nlm_GraphiC) d, &r);
  Nlm_InsetRect (&r, 4, 4);
  nx = pt;
  nx.x += Nlm_CharWidth (ch);
#ifdef WIN_MSWIN
  nx.y -= disphght;
#endif
  if (Nlm_PtInRect (pt, &r) && Nlm_PtInRect (nx, &r)) {
  } else {
    screenPos = 0;
    Nlm_NextScreenLine (d);
  }
  Nlm_GetCursorPos (d, &pt);
  nx = pt;
  nx.x += Nlm_CharWidth (ch);
  if (Nlm_PtInRect (pt, &r) && Nlm_PtInRect (nx, &r)) {
    Nlm_MoveTo (pt.x, pt.y);
    Nlm_PaintChar (ch);
    Nlm_SetCursorPos (d, nx);
    if (screenPos < screenBufSize) {
      screenBfr [screenPos] = ch;
    }
    screenPos++;
  }
}

static void Nlm_EraseLastChar (Nlm_DisplaY d)

{
  Nlm_Char   ch;
  Nlm_PoinT  pt;

  if (screenPos > 0) {
    screenPos--;
    if (screenPos < screenBufSize) {
      Nlm_GetCursorPos (d, &pt);
      ch = screenBfr [screenPos];
      pt.x -= Nlm_CharWidth (ch);
      Nlm_MoveTo (pt.x, pt.y);
      Nlm_SetCursorPos (d, pt);
      Nlm_InvertMode ();
      Nlm_PaintOneCharacter (d, ch);
      Nlm_MoveTo (pt.x, pt.y);
      Nlm_SetCursorPos (d, pt);
      screenPos--;
      Nlm_CopyMode ();
    }
  }
}

static void Nlm_PaintOrFormatChar (Nlm_GraphiC d, Nlm_Char ch, Nlm_Boolean savePort)

{
  Nlm_PoinT   pt;
  Nlm_RecT    r;
  Nlm_WindoW  tempPort;
#ifdef WIN_MAC
  PenState    state;
#endif

  if (Nlm_GetVisible (d) && Nlm_AllParentsButWindowVisible (d)) {
    tempPort = Nlm_SavePortIfNeeded (d, savePort);
    Nlm_DoSelect (d, FALSE);
#ifdef WIN_MAC
    GetPenState (&state);
    PenNormal ();
#endif
    Nlm_SelectFont (dispfont);
    switch (ch) {
      case '\n':
        screenPos = 0;
        Nlm_NextScreenLine ((Nlm_DisplaY) d);
        break;
      case '\r':
#ifndef WIN_MSWIN
        screenPos = 0;
        Nlm_NextScreenLine ((Nlm_DisplaY) d);
#endif
        break;
      case '\t':
        do {
          Nlm_PaintOneCharacter ((Nlm_DisplaY) d, ' ');
        } while ((screenPos % 8) != 0);
        break;
      case '\b':
        if (screenPos > 0) {
          Nlm_EraseLastChar ((Nlm_DisplaY) d);
        }
        break;
      case '\f':
        screenPos = 0;
        Nlm_GetRect (d, &r);
        Nlm_LoadPt (&pt, r.left, r.top + disphght);
        Nlm_SetCursorPos ((Nlm_DisplaY) d, pt);
        Nlm_InsetRect (&r, 2, 2);
        Nlm_EraseRect (&r);
        break;
      case '\7':
        Nlm_Beep ();
        break;
      default:
        if (ch >= ' ') {
          Nlm_PaintOneCharacter ((Nlm_DisplaY) d, ch);
        }
        break;
    }
#ifdef WIN_MAC
    SetPenState (&state);
#endif
    Nlm_SelectFont (Nlm_systemFont);
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_SelectDisplay (Nlm_GraphiC d, Nlm_Boolean savePort)

{
  Nlm_WindoW  w;

  if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (d))) {
    w = Nlm_GetParentWindow (d);
    Nlm_SetWindowCharDisplay (w, (Nlm_DisplaY) d);
  }
}

#ifdef WIN_MOTIF
#define MOUSE_DOWN 1
#define MOUSE_MOVE 2
#define MOUSE_UP   3

static void Nlm_SlateTimer (XtPointer client_data, XtIntervalId *id)

{
  Nlm_PaneL        p;
  Nlm_PnlClckProc  prs;
  Nlm_PoinT        pt;
  Nlm_SlatE        s;

  if (slateTimerUsed) {
    s = (Nlm_SlatE) client_data;
    pt = currentMousePos;
    if ((Nlm_SlatE) s == recentlyClickedSlate &&
        recentlyClickedSlate != NULL && recentlyClickedPanel != NULL) {
      p = recentlyClickedPanel;
      Nlm_DoSelect ((Nlm_GraphiC) p, FALSE);
      prs = Nlm_GetPanelHold (p);
      if (prs != NULL) {
        Nlm_ResetDrawingTools ();
        prs (p, pt);
      }
    }
    slateTimer = XtAppAddTimeOut (Nlm_appContext, 50, Nlm_SlateTimer, (XtPointer) s);
    slateTimerUsed = TRUE;
  }
}

static void Nlm_SlateClick (Nlm_SlatE s, Nlm_PoinT pt, Nlm_Int2 mouseAction)

{
  Nlm_PnlClckProc  clk;
  Nlm_PnlClckProc  drg;
  Nlm_SlateTool    h;
  Nlm_Int2         hOffset;
  Nlm_PaneL        n;
  Nlm_Boolean      notInside;
  Nlm_PaneL        p;
  Nlm_PanelData    pdata;
  Nlm_PnlClckProc  prs;
  Nlm_RecT         r;
  Nlm_PnlClckProc  rls;
  Nlm_Int2         vOffset;
  Nlm_Int2         x;
  Nlm_Int2         y;

  h = Nlm_GetSlateHandle ((Nlm_SlatE) s);
  currentMousePos = pt;
  switch (mouseAction) {
    case MOUSE_DOWN:
      slateTimer = XtAppAddTimeOut (Nlm_appContext, 50, Nlm_SlateTimer, (XtPointer) s);
      slateTimerUsed = TRUE;
      Nlm_GetRect ((Nlm_GraphiC) s, &r);
      recentlyClickedSlate = (Nlm_SlatE) s;
      recentlyClickedPanel = NULL;
      Nlm_SelectFont (Nlm_systemFont);
      Nlm_ResetDrawingTools ();
      Nlm_GetPanelData ((Nlm_PaneL) s, &pdata);
      if (pdata.click != NULL || pdata.drag != NULL || pdata.hold != NULL ||
          pdata.release != NULL || pdata.draw != NULL || pdata.reset != NULL) {
        p = (Nlm_PaneL) s;
        recentlyClickedPanel = p;
        clk = Nlm_GetPanelClick (p);
        if (clk != NULL && Nlm_GetEnabled ((Nlm_GraphiC) p)
            && Nlm_GetVisible ((Nlm_GraphiC) p)) {
          clk (p, pt);
          Nlm_Update ();
        }
      } else {
        p = Nlm_CreateReverseLinks ((Nlm_SlatE) s);
        notInside = TRUE;
        while (p != NULL && notInside) {
          n = Nlm_GetPanelPrev (p);
          Nlm_GetPanelData (p, &pdata);
          if (pdata.click != NULL || pdata.drag != NULL ||
              pdata.hold != NULL || pdata.release != NULL) {
            Nlm_GetRect ((Nlm_GraphiC) p, &r);
            Nlm_DoGetOffset ((Nlm_GraphiC) p, &hOffset, &vOffset);
            Nlm_OffsetRect (&r, -hOffset, -vOffset);
            if (Nlm_PtInRect (pt, &r) && Nlm_GetEnabled ((Nlm_GraphiC) p) &&
                Nlm_GetVisible ((Nlm_GraphiC) p)) {
              recentlyClickedPanel = p;
              notInside = FALSE;
              clk = Nlm_GetPanelClick (p);
              if (clk != NULL) {
                clk (p, pt);
                Nlm_Update ();
              }
            }
          }
          p = n;
        }
      }
      break;
    case MOUSE_MOVE:
      Nlm_GetRect ((Nlm_GraphiC) s, &r);
      if ((Nlm_SlatE) s == recentlyClickedSlate && recentlyClickedPanel != NULL) {
        p = recentlyClickedPanel;
        drg = Nlm_GetPanelDrag (p);
        if (drg != NULL) {
          Nlm_ResetDrawingTools ();
          drg (p, pt);
        }
      }
      break;
    case MOUSE_UP:
      if (slateTimerUsed) {
        XtRemoveTimeOut (slateTimer);
      }
      slateTimerUsed = FALSE;
      Nlm_GetRect ((Nlm_GraphiC) s, &r);
      if ((Nlm_SlatE) s == recentlyClickedSlate && recentlyClickedPanel != NULL) {
        p = recentlyClickedPanel;
        rls = Nlm_GetPanelRelease (p);
        if (rls != NULL) {
          Nlm_ResetDrawingTools ();
          rls (p, pt);
        }
      }
      Nlm_SelectFont (Nlm_systemFont);
      Nlm_ResetDrawingTools ();
/*
      recentlyClickedSlate = NULL;
*/
      recentlyClickedPanel = NULL;
      break;
    default:
      break;
  }
}

static void Nlm_SlateClickCallback (Widget w, XEvent *ev,
                                    String *args, Cardinal *num_args)

{
  XButtonEvent  *event;
  Pixel         color;
  Nlm_Uint4     delta;
  Nlm_WindoW    newWindow;
  Nlm_WindoW    oldWindow;
  Nlm_PoinT     pt;
  XtPointer     ptr;
  Nlm_RecT      r;
  Nlm_SlatE     s;
  Nlm_Uint4     state;
  Window        tempWind;
  Nlm_Uint4     ticks;

  event = (XButtonEvent *) ev;
  XtVaGetValues (w, XmNuserData, &ptr, NULL);
  s = (Nlm_SlatE) ptr;
  if (Nlm_GetEnabled ((Nlm_GraphiC) s) && Nlm_GetAllParentsEnabled ((Nlm_GraphiC) s)) {
    newWindow = Nlm_GetParentWindow ((Nlm_GraphiC) s);
    if (! Nlm_IsWindowDying (newWindow)) {
      oldWindow = Nlm_CurrentWindow ();
      if (oldWindow != newWindow) {
        Nlm_UseWindow (newWindow);
      }
      Nlm_GetRect ((Nlm_GraphiC) s, &r);
      Nlm_XOffset = r.left;
      Nlm_YOffset = r.top;
      tempWind = Nlm_currentXWindow;
      Nlm_currentXWindow = XtWindow (w);
      XtVaGetValues (w, XmNbackground, &color, 0);
      Nlm_XbackColor = color;
      XtVaGetValues (w, XmNforeground, &color, 0);
      Nlm_XforeColor = color;
      pt.x = (Nlm_Int2) event->x + Nlm_XOffset;
      pt.y = (Nlm_Int2) event->y + Nlm_YOffset;
      Nlm_cmmdKey = FALSE;
      Nlm_ctrlKey = FALSE;
      Nlm_optKey = FALSE;
      Nlm_shftKey = FALSE;
      Nlm_dblClick = FALSE;
      state = event->state;
      if ((state &ControlMask) != 0) {
        Nlm_ctrlKey = TRUE;
      }
      if ((state &ShiftMask) != 0) {
        Nlm_shftKey = TRUE;
      }
      if (strcmp (args [0], "down") == 0) {
        ticks = (Nlm_Uint4) event->time;
        if (s == recentlyClickedSlate) {
          delta = ticks - (Nlm_Uint4) lastClickTime;
          if (delta <= (Nlm_Uint4) XtGetMultiClickTime (Nlm_currentXDisplay)) {
            Nlm_dblClick = TRUE;
          }
        }
        lastClickTime = (Nlm_Int4) ticks;
        Nlm_SlateClick (s, pt, MOUSE_DOWN);
      } else if (strcmp (args [0], "up") == 0) {
        Nlm_SlateClick (s, pt, MOUSE_UP);
      } else if (strcmp (args [0], "motion") == 0) {
        Nlm_SlateClick (s, pt, MOUSE_MOVE);
      }
      if (Nlm_currentXDisplay != NULL) {
        XSync (Nlm_currentXDisplay, FALSE);
      }
      Nlm_currentXWindow = tempWind;
      if (oldWindow != newWindow) {
        Nlm_UseWindow (oldWindow);
      }
    }
  }
}

static void Nlm_SlateKeyCallback (Widget w, XEvent *ev,
                                  String *args, Cardinal *num_args)

{
  XKeyEvent        *event;
  Nlm_Char         buffer [20];
  Nlm_Char         ch;
  int              charcount;
  XComposeStatus   compose;
  KeySym           keysym;
  Pixel            color;
  Nlm_Uint4        delta;
  Nlm_SltCharProc  keyProc;
  Nlm_WindoW       newWindow;
  Nlm_WindoW       oldWindow;
  XtPointer        ptr;
  Nlm_RecT         r;
  Nlm_SlatE        s;
  Nlm_Uint4        state;
  Window           tempWind;

  event = (XKeyEvent *) ev;
  XtVaGetValues (w, XmNuserData, &ptr, NULL);
  s = (Nlm_SlatE) ptr;
  if (Nlm_GetEnabled ((Nlm_GraphiC) s) && Nlm_GetAllParentsEnabled ((Nlm_GraphiC) s)) {
    newWindow = Nlm_GetParentWindow ((Nlm_GraphiC) s);
    if (! Nlm_IsWindowDying (newWindow)) {
      oldWindow = Nlm_CurrentWindow ();
      if (oldWindow != newWindow) {
        Nlm_UseWindow (newWindow);
      }
      Nlm_GetRect ((Nlm_GraphiC) s, &r);
      Nlm_XOffset = r.left;
      Nlm_YOffset = r.top;
      tempWind = Nlm_currentXWindow;
      Nlm_currentXWindow = XtWindow (w);
      XtVaGetValues (w, XmNbackground, &color, 0);
      Nlm_XbackColor = color;
      XtVaGetValues (w, XmNforeground, &color, 0);
      Nlm_XforeColor = color;
      Nlm_cmmdKey = FALSE;
      Nlm_ctrlKey = FALSE;
      Nlm_optKey = FALSE;
      Nlm_shftKey = FALSE;
      Nlm_dblClick = FALSE;
      state = event->state;
      if ((state &ControlMask) != 0) {
        Nlm_ctrlKey = TRUE;
      }
      if ((state &ShiftMask) != 0) {
        Nlm_shftKey = TRUE;
      }
      if (ev->type == KeyPress && strcmp (args [0], "key") == 0) {
        buffer [0] = '\0';
        charcount = XLookupString (&(ev->xkey), buffer, 20, &keysym, &compose);
        if (charcount >= 0 && charcount < 20) {
          buffer [charcount] = '\0';
        }
        keyProc = Nlm_GetSlateCharProc (s);
        if (keyProc != NULL) {
          ch = buffer [0];
          keyProc (s, ch);
        }
      }
      if (Nlm_currentXDisplay != NULL) {
        XSync (Nlm_currentXDisplay, FALSE);
      }
      Nlm_currentXWindow = tempWind;
      if (oldWindow != newWindow) {
        Nlm_UseWindow (oldWindow);
      }
    }
  }
}

static void Nlm_DrawSlate (Nlm_SlatE s)

{
  Nlm_PnlActnProc  drw;
  Nlm_SlateTool    h;
  Nlm_PaneL        n;
  Nlm_PaneL        p;
  Nlm_PanelData    pdata;
  Nlm_RecT         r;

  if (Nlm_GetVisible ((Nlm_GraphiC) s) && Nlm_GetAllParentsVisible ((Nlm_GraphiC) s)) {
    h = Nlm_GetSlateHandle ((Nlm_SlatE) s);
    if (h != NULL) {
      Nlm_GetRect ((Nlm_GraphiC) s, &r);
      Nlm_SelectFont (Nlm_systemFont);
      Nlm_ResetDrawingTools ();
      if (Nlm_GetSlateBorder ((Nlm_SlatE) s)) {
        Nlm_FrameRect (&r);
      }
      Nlm_GetPanelData ((Nlm_PaneL) s, &pdata);
      if (pdata.click != NULL || pdata.drag != NULL || pdata.hold != NULL ||
          pdata.release != NULL || pdata.draw != NULL || pdata.reset != NULL) {
        p = (Nlm_PaneL) s;
/*
        recentlyClickedPanel = p;
*/
        drw = Nlm_GetPanelDraw (p);
        if (drw != NULL && Nlm_GetVisible ((Nlm_GraphiC) p)) {
          drw (p);
          Nlm_ResetDrawingTools ();
        }
      } else {
        p = (Nlm_PaneL) Nlm_GetChild ((Nlm_GraphiC) s);
        while (p != NULL) {
          n = (Nlm_PaneL) Nlm_GetNext ((Nlm_GraphiC) p);
          drw = Nlm_GetPanelDraw (p);
          if (drw != NULL && Nlm_GetVisible ((Nlm_GraphiC) p)) {
            drw (p);
            Nlm_ResetDrawingTools ();
          }
          p = n;
        }
      }
    }
  }
}

static void Nlm_ClipXRgn (Nlm_RegioN rgn)

{
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
}

static void Nlm_SlateDrawCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  XmDrawingAreaCallbackStruct  *cbs;
  Pixel                        color;
  XEvent                       event;
  Nlm_WindoW                   newWindow;
  Nlm_WindoW                   oldWindow;
  Nlm_RecT                     r;
  Nlm_RegioN                   rgn1;
  Nlm_RegioN                   rgn2;
  Nlm_SlatE                    s;
  Nlm_RegioN                   tempRgn;
  Window                       tempWind;

  cbs = (XmDrawingAreaCallbackStruct *) call_data;
  s = (Nlm_SlatE) client_data;
  newWindow = Nlm_GetParentWindow ((Nlm_GraphiC) s);
  if (! Nlm_IsWindowDying (newWindow)) {
    oldWindow = Nlm_CurrentWindow ();
    if (oldWindow != newWindow) {
      Nlm_UseWindow (newWindow);
    }
    Nlm_GetRect ((Nlm_GraphiC) s, &r);
    Nlm_XOffset = r.left;
    Nlm_YOffset = r.top;
    tempWind = Nlm_currentXWindow;
    Nlm_currentXWindow = XtWindow (w);
    XtVaGetValues (w, XmNbackground, &color, 0);
    Nlm_XbackColor = color;
    XtVaGetValues (w, XmNforeground, &color, 0);
    Nlm_XforeColor = color;
    rgn1 = Nlm_CreateRgn ();
    event = (*(cbs->event));
    do {
      r.left = event.xexpose.x;
      r.top = event.xexpose.y;
      r.right = r.left + event.xexpose.width;
      r.bottom = r.top + event.xexpose.height;
      rgn2 = Nlm_CreateRgn ();
      Nlm_LoadRectRgn (rgn2, r.left, r.top, r.right, r.bottom);
      Nlm_UnionRgn (rgn1, rgn2, rgn1);
      Nlm_DestroyRgn (rgn2);
    } while (XCheckTypedWindowEvent (Nlm_currentXDisplay,
             Nlm_currentXWindow, Expose, &event));
    Nlm_OffsetRgn (rgn1, Nlm_XOffset, Nlm_YOffset);
    Nlm_ClipXRgn (rgn1);
    Nlm_DestroyRgn (rgn1);
    if (Nlm_updateRgn != NULL) {
      Nlm_DestroyRgn (Nlm_updateRgn);
    }
    Nlm_updateRgn = Nlm_CreateRgn ();
    tempRgn = Nlm_CreateRgn ();
    Nlm_UnionRgn (Nlm_clpRgn, tempRgn, Nlm_updateRgn);
    Nlm_DestroyRgn (tempRgn);
    XOffsetRegion ((Nlm_RgnTool) Nlm_updateRgn, Nlm_XOffset, Nlm_YOffset);
    Nlm_DrawSlate (s);
    if (Nlm_currentXDisplay != NULL && Nlm_currentXGC != NULL) {
      XSetClipMask (Nlm_currentXDisplay, Nlm_currentXGC, None);
      if (Nlm_clpRgn != NULL) {
        XDestroyRegion ((Nlm_RgnTool) Nlm_clpRgn);
        Nlm_clpRgn = NULL;
      }
    }
    if (Nlm_currentXDisplay != NULL) {
      XSync (Nlm_currentXDisplay, FALSE);
    }
    Nlm_currentXWindow = tempWind;
    if (oldWindow != newWindow) {
      Nlm_UseWindow (oldWindow);
    }
  }
}
#endif

static void Nlm_NewSlate (Nlm_SlatE s, Nlm_Boolean border,
                          Nlm_Boolean vScroll, Nlm_Boolean hScroll,
                          Nlm_SltScrlProc vscrl, Nlm_SltScrlProc hscrl,
                          Nlm_Int2 extra)

{
  Nlm_SlateTool   h;
  Nlm_BaR         hsb;
  Nlm_PoinT       npt;
  Nlm_RecT        r;
  Nlm_BaR         vsb;
  Nlm_WindowTool  wptr;
#ifdef WIN_MOTIF
  Cardinal        n;
  Arg             wargs [10];
  String          trans =
    "<Btn1Down>:     slate(down)   ManagerGadgetArm()  \n\
     <Btn1Up>:       slate(up)     ManagerGadgetActivate() \n\
     <Btn1Motion>:   slate(motion) ManagerGadgetButtonMotion() \n\
     <Key>:          piano(key)    ManagerGadgetActivate()";
#endif

  Nlm_GetRect ((Nlm_GraphiC) s, &r);
  h = NULL;
  vsb = NULL;
  hsb = NULL;
  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) s);
#ifdef WIN_MSWIN
  h = CreateWindow (slateclass, "", WS_CHILD,
                    r.left, r.top, r.right - r.left,
                    r.bottom - r.top, wptr, 0,
                    Nlm_currentHInst, NULL);
  if (h != NULL) {
    SetProp (h, (LPSTR) "Nlm_VibrantProp", (Nlm_HandleTool) s);
  }
#endif
#ifdef WIN_MOTIF
  n = 0;
  XtSetArg (wargs [n], XmNx, (Position) r.left); n++;
  XtSetArg (wargs [n], XmNy, (Position) r.top); n++;
  XtSetArg (wargs [n], XmNwidth, (Dimension) (r.right - r.left + 1)); n++;
  XtSetArg (wargs [n], XmNheight, (Dimension) (r.bottom - r.top + 1)); n++;
  XtSetArg (wargs [n], XmNmarginHeight, 0); n++;
  XtSetArg (wargs [n], XmNmarginWidth, 0); n++;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNtranslations, XtParseTranslationTable (trans)); n++;
  h = XtCreateManagedWidget ((String) "", xmDrawingAreaWidgetClass,
                             wptr, wargs, n);
  XtVaSetValues (h, XmNuserData, (XtPointer) s, NULL);
  XtAddCallback (h, XmNexposeCallback, Nlm_SlateDrawCallback, (XtPointer) s);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (s))) {
      XtRealizeWidget (h);
    }
  } else {
    XtRealizeWidget (h);
  }
#endif
  if (vScroll) {
    Nlm_GetRect ((Nlm_GraphiC) s, &r);
    r.left = r.right;
    r.right += Nlm_vScrollBarWidth;
    if (vscrl != NULL) {
      vsb = Nlm_VertScrollBar ((Nlm_GraphiC) s, &r, (Nlm_ScrlProc) vscrl);
    } else {
      vsb = Nlm_VertScrollBar ((Nlm_GraphiC) s, &r,
                               (Nlm_ScrlProc) Nlm_SlateVertScrollAction);
    }
  }
  if (hScroll) {
    Nlm_GetRect ((Nlm_GraphiC) s, &r);
    r.top = r.bottom;
    r.bottom += Nlm_hScrollBarHeight;
    if (hscrl != NULL) {
      hsb = Nlm_HorizScrollBar ((Nlm_GraphiC) s, &r, (Nlm_ScrlProc) hscrl);
    } else {
      hsb = Nlm_HorizScrollBar ((Nlm_GraphiC) s, &r,
                                (Nlm_ScrlProc) Nlm_SlateHorizScrollAction);
    }
  }
  Nlm_GetRect ((Nlm_GraphiC) s, &r);
  Nlm_InsetRect (&r, 4, 4);
  Nlm_LoadPt (&npt, r.left, r.top);
  Nlm_LoadBoxData ((Nlm_BoX) s, npt, npt, npt, npt.y, npt.x, 0, 0, 0, 0, 0, 0);
  Nlm_GetRect ((Nlm_GraphiC) s, &r);
  Nlm_LoadSlateData (s, h, vsb, hsb, NULL, NULL, 0, 0,
                     border, 0, 0, 0, NULL, NULL, FALSE);
  Nlm_LoadPanelData ((Nlm_PaneL) s, NULL, NULL, NULL, NULL, NULL, extra, NULL, NULL);
}

static void Nlm_NewPanel (Nlm_PaneL p, Nlm_PnlActnProc draw,
                          Nlm_Int2 extra, Nlm_PnlActnProc reset)

/*
*  MemNew already sets the extra data to NULL.  If the window is visible,
*  the draw request may occur before the extra data is initialized.
*/

{
  Nlm_PoinT  npt;
  Nlm_RecT   r;

  Nlm_GetRect ((Nlm_GraphiC) p, &r);
  Nlm_LoadPt (&npt, r.left, r.top);
  Nlm_LoadBoxData ((Nlm_BoX) p, npt, npt, npt, npt.y, npt.x, 0, 0, 0, 0, 0, 0);
  Nlm_LoadPanelData (p, NULL, NULL, NULL, NULL, draw, extra, reset, NULL);
}

static void Nlm_NewDisplay (Nlm_DisplaY d)

{
  Nlm_PoinT   pos;
  Nlm_RecT    r;
  Nlm_Int2    vis;
  Nlm_WindoW  w;

  Nlm_GetRect ((Nlm_GraphiC) d, &r);
  Nlm_InsetRect (&r, 4, 4);
  Nlm_LoadPt (&pos, r.left, r.top + disphght);
  if (disphght > 0) {
    vis = (r.bottom - r.top + 1) / disphght;
  } else {
    vis = 0;
  }
  Nlm_LoadDisplayData (d, FALSE, FALSE, NULL, 0, vis, pos);
  w = Nlm_GetParentWindow ((Nlm_GraphiC) d);
  Nlm_SetWindowCharDisplay (w, d);
}

static Nlm_SlatE Nlm_CommonSlate (Nlm_GrouP prnt, Nlm_Int2 width,
                                  Nlm_Int2 height, Nlm_Boolean border,
                                  Nlm_Boolean vScroll, Nlm_Boolean hScroll,
                                  Nlm_SltScrlProc vscrl, Nlm_SltScrlProc hscrl,
                                  Nlm_Int2 extra, Nlm_GphPrcsPtr classPtr)

{
  Nlm_GraphicData  gdata;
  Nlm_PoinT        npt;
  Nlm_RecT         r;
  Nlm_SlatE        s;
  Nlm_WindoW       tempPort;

  s = NULL;
  if (prnt != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) prnt);
    Nlm_GetNextPosition ((Nlm_GraphiC) prnt, &npt);
    Nlm_SelectFont (Nlm_systemFont);
    Nlm_LoadRect (&r, npt.x, npt.y, npt.x+width, npt.y+height);
    s = (Nlm_SlatE) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r, sizeof (Nlm_SlateRec) + extra, classPtr);
    if (s != NULL) {
      if (classPtr != NULL) {
        classPtr->ancestor = slateProcs;
      } else {
        Nlm_GetGraphicData ((Nlm_GraphiC) s, &gdata);
        gdata.classptr = slateProcs;
        Nlm_SetGraphicData ((Nlm_GraphiC) s, &gdata);
      }
      Nlm_NewSlate (s, border, vScroll, hScroll, vscrl, hscrl, extra);
      if (vScroll) {
        r.right += Nlm_vScrollBarWidth;
      }
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) s, &r, TRUE, FALSE);
      Nlm_DoShow ((Nlm_GraphiC) s, TRUE, FALSE);
    }
    Nlm_RestorePort (tempPort);
  }
  return s;
}

extern Nlm_SlatE Nlm_GeneralSlate (Nlm_GrouP prnt, Nlm_Int2 width,
                                   Nlm_Int2 height, Nlm_SltScrlProc scrl,
                                   Nlm_Int2 extra, Nlm_GphPrcsPtr classPtr)

{
  Nlm_SlatE  s;

  s = Nlm_CommonSlate (prnt, width*Nlm_stdCharWidth+8,
                       height*Nlm_stdLineHeight+8, TRUE,
                       TRUE, FALSE, scrl, NULL, extra, classPtr);
  return s;
}

extern Nlm_SlatE Nlm_ScrollSlate (Nlm_GrouP prnt, Nlm_Int2 width,
                                  Nlm_Int2 height)

{
  Nlm_SlatE  s;

  s = Nlm_CommonSlate (prnt, width*Nlm_stdCharWidth+8,
                       height*Nlm_stdLineHeight+8, TRUE,
                       TRUE, FALSE, NULL, NULL, 0, NULL);
  return s;
}

extern Nlm_SlatE Nlm_NormalSlate (Nlm_GrouP prnt, Nlm_Int2 width,
                                  Nlm_Int2 height)

{
  Nlm_SlatE  s;

  s = Nlm_CommonSlate (prnt, width*Nlm_stdCharWidth+8,
                       height*Nlm_stdLineHeight+8, TRUE,
                       FALSE, FALSE, NULL, NULL, 0, NULL);
  return s;
}

extern Nlm_SlatE Nlm_HiddenSlate (Nlm_GrouP prnt, Nlm_Int2 width,
                                  Nlm_Int2 height)

{
  Nlm_SlatE  s;

  s = Nlm_CommonSlate (prnt, width*Nlm_stdCharWidth+8,
                       height*Nlm_stdLineHeight+8, FALSE,
                       FALSE, FALSE, NULL, NULL, 0, NULL);
  return s;
}

extern void Nlm_VirtualSlate (Nlm_SlatE s, Nlm_Int2 before,
                              Nlm_Int2 after, Nlm_SltScrlProc actn)

{
  Nlm_Handle     chdl;
  Nlm_Handle     rhdl;
  Nlm_BaR        sb;
  Nlm_SlateData  sdata;

  Nlm_GetSlateData (s, &sdata);
  rhdl = sdata.rowHandles;
  if (rhdl != NULL) {
    Nlm_HandFree (rhdl);
  }
  chdl = sdata.colHandles;
  if (chdl != NULL) {
    Nlm_HandFree (chdl);
  }
  sdata.rowHandles = NULL;
  sdata.colHandles = NULL;
  sdata.numRows = 0;
  sdata.numCols = 0;
  sdata.barRows = 0;
  sdata.vBefore = before;
  sdata.vAfter = after;
  sdata.vAction = actn;
  Nlm_SetSlateData (s, &sdata);
  sb = sdata.hScrollBar;
  if (sb != NULL) {
    Nlm_DoReset ((Nlm_GraphiC) sb, TRUE);
  }
  sb = sdata.vScrollBar;
  if (sb != NULL) {
    Nlm_DoReset ((Nlm_GraphiC) sb, TRUE);
  }
}

extern void Nlm_RegisterRow (Nlm_SlatE s, Nlm_Int2 position,
                             Nlm_Int2 height, Nlm_Int2 count)

{
  Nlm_Boolean    added;
  Nlm_Int2       after;
  Nlm_Int2       barmax;
  Nlm_Int2       before;
  Nlm_Int2       chunk;
  Nlm_Int2       numRows;
  Nlm_Int2       delta;
  Nlm_Int2       i;
  Nlm_OfsPtr     optr;
  Nlm_RecT       r;
  Nlm_Handle     rhdl;
  Nlm_BaR        sb;
  Nlm_SlateData  sdata;
  Nlm_WindoW     tempPort;
  Nlm_Int2       vis;

  if (s != NULL) {
    Nlm_GetRect ((Nlm_GraphiC) s, &r);
    Nlm_InsetRect (&r, 4, 4);
    Nlm_GetSlateData (s, &sdata);
    numRows = sdata.numRows;
    rhdl = sdata.rowHandles;
    added = FALSE;
    chunk = 128;
    delta = position - r.top;
    if (rhdl == NULL) {
      numRows = 1;
      rhdl = Nlm_HandNew (sizeof (Nlm_OffsetRec) * chunk);
      if (rhdl != NULL) {
        optr = (Nlm_OfsPtr) Nlm_HandLock (rhdl);
        optr [numRows - 1].offset = delta;
        optr [numRows - 1].height = height;
        added = TRUE;
        Nlm_HandUnlock (rhdl);
      }
      delta += height;
      count--;
    }
    if (rhdl != NULL && count > 0) {
      optr = (Nlm_OfsPtr) Nlm_HandLock (rhdl);
      for (i = 0; i < count; i++) {
        if (delta >= optr [numRows - 1].offset + optr [numRows - 1].height) {
          if (numRows % chunk == 0) {
            Nlm_HandUnlock (rhdl);
            rhdl = Nlm_HandMore (rhdl, sizeof (Nlm_OffsetRec) *
                                 (numRows / chunk + 1) * chunk);
            optr = (Nlm_OfsPtr) Nlm_HandLock (rhdl);
          }
          numRows++;
          optr [numRows - 1].offset = delta;
          optr [numRows - 1].height = height;
          added = TRUE;
        } else if (delta == optr [numRows - 1].offset &&
                   height > optr [numRows - 1].height) {
          optr [numRows - 1].height = height;
          added = TRUE;
        }
        delta += height;
      }
      Nlm_HandUnlock (rhdl);
    }
    sdata.numRows = numRows;
    sdata.rowHandles = rhdl;
    Nlm_SetSlateData (s, &sdata);
    if (added) {
      optr = (Nlm_OfsPtr) Nlm_HandLock (rhdl);
      if (numRows > 1) {
        i = numRows - 2;
        vis = 0;
        while (i >= 0 && optr [numRows - 1].offset + optr [numRows - 1].height -
               optr [i].offset <= r.bottom - r.top) {
          i--;
          vis++;
        }
        barmax = numRows - vis - 1;
      } else {
        barmax = 0;
      }
      sdata.barRows = barmax;
      Nlm_SetSlateData (s, &sdata);
      i = 1;
      vis = 0;
      while (i < numRows && optr [i].offset + optr [i].height -
             optr [0].offset <= r.bottom - r.top) {
        i++;
        vis++;
      }
      Nlm_HandUnlock (rhdl);
      sb = sdata.vScrollBar;
      if (sb != NULL) {
        before = sdata.vBefore;
        after = sdata.vAfter;
        tempPort = Nlm_SavePort ((Nlm_GraphiC) s);
        Nlm_DoSetRange ((Nlm_GraphiC) sb, 1, vis, barmax + before + after, FALSE);
        if (before > 0) {
          Nlm_DoSetValue ((Nlm_GraphiC) sb, before, FALSE);
        }
        Nlm_RestorePort (tempPort);
      }
    }
  }
}

extern void Nlm_RegisterColumn (Nlm_SlatE s, Nlm_Int2 position,
                                Nlm_Int2 width, Nlm_Int2 count)

{
}

extern void Nlm_SetPanelClick (Nlm_PaneL p, Nlm_PnlClckProc click,
                               Nlm_PnlClckProc drag, Nlm_PnlClckProc hold,
                               Nlm_PnlClckProc release)

{
  Nlm_PanelData  pdata;

  if (p != NULL) {
    Nlm_GetPanelData (p, &pdata);
    pdata.click = click;
    pdata.drag = drag;
    pdata.hold = hold;
    pdata.release = release;
    Nlm_SetPanelData (p, &pdata);
  }
}

extern Nlm_PaneL Nlm_GeneralPanel (Nlm_SlatE s, Nlm_PnlActnProc draw,
                                   Nlm_Int2 extra, Nlm_PnlActnProc reset,
                                   Nlm_GphPrcsPtr classPtr)

{
  Nlm_GraphicData  gdata;
  Nlm_PoinT        npt;
  Nlm_PaneL        p;
  Nlm_RecT         r;
  Nlm_WindoW       tempPort;

  p = NULL;
  if (s != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) s);
    Nlm_GetNextPosition ((Nlm_GraphiC) s, &npt);
    Nlm_SelectFont (Nlm_systemFont);
    Nlm_LoadRect (&r, npt.x, npt.y, npt.x, npt.y);
    p = (Nlm_PaneL) Nlm_CreateLink ((Nlm_GraphiC) s, &r, sizeof (Nlm_PanelRec) + extra, classPtr);
    if (p != NULL) {
      if (classPtr != NULL) {
        classPtr->ancestor = panelProcs;
      } else {
        Nlm_GetGraphicData ((Nlm_GraphiC) p, &gdata);
        gdata.classptr = panelProcs;
        Nlm_SetGraphicData ((Nlm_GraphiC) p, &gdata);
      }
      Nlm_NewPanel (p, draw, extra, reset);
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) p, &r, TRUE, FALSE);
      Nlm_DoShow ((Nlm_GraphiC) p, TRUE, FALSE);
    }
    Nlm_RestorePort (tempPort);
  }
  return p;
}

extern Nlm_PaneL Nlm_CustomPanel (Nlm_SlatE s, Nlm_PnlActnProc draw,
                                  Nlm_Int2 extra, Nlm_PnlActnProc reset)

{
  return (Nlm_GeneralPanel (s, draw, extra, reset, NULL));
}

extern Nlm_PaneL Nlm_AutonomousPanel (Nlm_GrouP prnt, Nlm_Int2 pixwidth,
                                      Nlm_Int2 pixheight, Nlm_PnlActnProc draw,
                                      Nlm_SltScrlProc vscrl, Nlm_SltScrlProc hscrl,
                                      Nlm_Int2 extra, Nlm_PnlActnProc reset,
                                      Nlm_GphPrcsPtr classPtr)

/*
*  Note that an autonomous panel is really a combination of slate and
*  panel in one object, and thus uses the slateProcs class function.
*/

{
  Nlm_Boolean      border;
  Nlm_GraphicData  gdata;
  Nlm_Boolean      hScroll;
  Nlm_PoinT        npt;
  Nlm_PaneL        p;
  Nlm_RecT         r;
  Nlm_WindoW       tempPort;
  Nlm_Boolean      vScroll;


  p = NULL;
  if (prnt != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) prnt);
    Nlm_GetNextPosition ((Nlm_GraphiC) prnt, &npt);
    Nlm_SelectFont (Nlm_systemFont);
    border = (Nlm_Boolean) (vscrl != NULL || hscrl != NULL);
    vScroll = (Nlm_Boolean) (vscrl != NULL);
    hScroll = (Nlm_Boolean) (hscrl != NULL);
    if (vScroll || hScroll) {
      Nlm_LoadRect (&r, npt.x, npt.y, npt.x+pixwidth+8, npt.y+pixheight+8);
    } else {
      Nlm_LoadRect (&r, npt.x, npt.y, npt.x+pixwidth, npt.y+pixheight);
    }
    p = (Nlm_PaneL) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r, sizeof (Nlm_PanelRec) + extra, classPtr);
    if (p != NULL) {
      if (classPtr != NULL) {
        classPtr->ancestor = slateProcs;
      } else {
        Nlm_GetGraphicData ((Nlm_GraphiC) p, &gdata);
        gdata.classptr = slateProcs;
        Nlm_SetGraphicData ((Nlm_GraphiC) p, &gdata);
      }
      Nlm_NewSlate ((Nlm_SlatE) p, border, vScroll, hScroll, vscrl, hscrl, extra);
      Nlm_NewPanel (p, draw, extra, reset);
      if (vScroll) {
        r.right += Nlm_vScrollBarWidth;
      }
      if (hScroll) {
        r.bottom += Nlm_hScrollBarHeight;
      }
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) p, &r, TRUE, FALSE);
      Nlm_DoShow ((Nlm_GraphiC) p, TRUE, FALSE);
    }
    Nlm_RestorePort (tempPort);
  }
  return p;
}

extern Nlm_PaneL Nlm_SimplePanel (Nlm_GrouP prnt, Nlm_Int2 pixwidth,
                                  Nlm_Int2 pixheight, Nlm_PnlActnProc draw)

{
  Nlm_PaneL  p;

  p = Nlm_AutonomousPanel (prnt, pixwidth, pixheight, draw, NULL, NULL, 0, NULL, NULL);
  return p;
}

extern void Nlm_RegisterRect (Nlm_PaneL p, Nlm_RectPtr r)

{
  Nlm_BoxData  bdata;
  Nlm_RecT     pr;

  if (p != NULL && r != NULL) {
    Nlm_RecordRect ((Nlm_GraphiC) p, r);
    Nlm_NextPosition ((Nlm_GraphiC) p, r);
    Nlm_GetRect ((Nlm_GraphiC) p, &pr);
    Nlm_GetBoxData ((Nlm_BoX) p, &bdata);
    pr.right = bdata.limitPoint.x;
    pr.bottom = bdata.limitPoint.y;
    Nlm_SetRect ((Nlm_GraphiC) p, &pr);
    Nlm_DoAdjustPrnt ((Nlm_GraphiC) p, r, TRUE, TRUE);
  }
}

static Nlm_DisplaY Nlm_CommonDisplay (Nlm_GrouP prnt, Nlm_Int2 width,
                                      Nlm_Int2 height, Nlm_Boolean hidden,
                                      Nlm_Boolean scroll)

{
  Nlm_DisplaY  d;
  Nlm_WindoW   tempPort;

  d = NULL;
  if (prnt != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) prnt);
    dispfont = Nlm_programFont;
    Nlm_SelectFont (dispfont);
    disphght = Nlm_LineHeight ();
    if (hidden) {
      d = (Nlm_DisplaY) Nlm_CommonSlate (prnt, width*Nlm_stdCharWidth+8,
                                         height*Nlm_stdLineHeight+8, FALSE,
                                         FALSE, FALSE, NULL, NULL,
                                         sizeof (Nlm_DisplayData), displayProcs);
    } else if (scroll) {
      d = (Nlm_DisplaY) Nlm_CommonSlate (prnt, width*Nlm_stdCharWidth+8,
                                         height*Nlm_stdLineHeight+8, TRUE,
                                         TRUE, FALSE, NULL, NULL,
                                         sizeof (Nlm_DisplayData), displayProcs);
    } else {
      d = (Nlm_DisplaY) Nlm_CommonSlate (prnt, width*Nlm_stdCharWidth+8,
                                         height*Nlm_stdLineHeight+8, TRUE,
                                         FALSE, FALSE, NULL, NULL,
                                         sizeof (Nlm_DisplayData), displayProcs);
    }
    if (d != NULL) {
      Nlm_NewDisplay (d);
    }
    Nlm_SelectFont (Nlm_systemFont);
    Nlm_RestorePort (tempPort);
  }
  return d;
}

extern Nlm_DisplaY Nlm_NormalDisplay (Nlm_GrouP prnt, Nlm_Int2 width,
                                      Nlm_Int2 height)

{
  return (Nlm_CommonDisplay (prnt, width, height, FALSE, FALSE));
}

extern Nlm_DisplaY Nlm_ScrollDisplay (Nlm_GrouP prnt, Nlm_Int2 width,
                                      Nlm_Int2 height)

{
  return (Nlm_CommonDisplay (prnt, width, height, FALSE, TRUE));
}

extern void Nlm_CaptureSlateFocus (Nlm_SlatE s)

{
  Nlm_WindoW     w;
#ifdef WIN_MSWIN
  Nlm_SlateTool  h;
#endif
#ifdef WIN_MOTIF
  Nlm_SlateTool  h;
#endif

  w = Nlm_GetParentWindow ((Nlm_GraphiC) s);
  Nlm_DoLoseFocus ((Nlm_GraphiC) w, (Nlm_GraphiC) s, FALSE);
  Nlm_ChangeSlateFocus (s, TRUE);
#ifdef WIN_MSWIN
  h = Nlm_GetSlateHandle (s);
  if (h != NULL) {
    SetFocus (h);
  }
#endif
#ifdef WIN_MOTIF
  h = Nlm_GetSlateHandle (s);
  if (h != NULL) {
    XmProcessTraversal (h, XmTRAVERSE_CURRENT);
  }
#endif
}

static Nlm_GraphiC Nlm_SlateGainFocus (Nlm_GraphiC s, Nlm_Char ch, Nlm_Boolean savePort)

{
  Nlm_SltCharProc  keyProc;
  Nlm_GraphiC      rsult;

  rsult = NULL;
  keyProc = Nlm_GetSlateCharProc ((Nlm_SlatE) s);
  if (keyProc != NULL) {
    Nlm_ChangeSlateFocus ((Nlm_SlatE) s, TRUE);
    rsult = s;
  }
  return rsult;
}

static void Nlm_SlateLoseFocus (Nlm_GraphiC s, Nlm_GraphiC excpt, Nlm_Boolean savePort)

{
  if (s != excpt) {
    Nlm_ChangeSlateFocus ((Nlm_SlatE) s, FALSE);
  }
}

#ifdef WIN_MAC
extern Nlm_Boolean Nlm_RegisterSlates (void)

{
  return TRUE;
}
#endif

#ifdef WIN_MSWIN
static void Nlm_DrawSlate (Nlm_SlatE s)

{
  Nlm_PnlActnProc  drw;
  Nlm_SlateTool    h;
  Nlm_PaneL        n;
  Nlm_PaneL        p;
  Nlm_PanelData    pdata;
  Nlm_RecT         r;

  if (Nlm_GetVisible ((Nlm_GraphiC) s) && Nlm_GetAllParentsVisible ((Nlm_GraphiC) s)) {
    h = Nlm_GetSlateHandle ((Nlm_SlatE) s);
    if (h != NULL && IsWindowVisible (h)) {
      Nlm_GetRect ((Nlm_GraphiC) s, &r);
      Nlm_SelectFont (Nlm_systemFont);
      Nlm_ResetDrawingTools ();
      if (Nlm_GetSlateBorder ((Nlm_SlatE) s)) {
        Nlm_FrameRect (&r);
      }
      Nlm_GetPanelData ((Nlm_PaneL) s, &pdata);
      if (pdata.click != NULL || pdata.drag != NULL || pdata.hold != NULL ||
          pdata.release != NULL || pdata.draw != NULL || pdata.reset != NULL) {
        p = (Nlm_PaneL) s;
/*
        recentlyClickedPanel = p;
*/
        drw = Nlm_GetPanelDraw (p);
        if (drw != NULL && Nlm_GetVisible ((Nlm_GraphiC) p)) {
          drw (p);
          Nlm_ResetDrawingTools ();
        }
      } else {
        p = (Nlm_PaneL) Nlm_GetChild ((Nlm_GraphiC) s);
        while (p != NULL) {
          n = (Nlm_PaneL) Nlm_GetNext ((Nlm_GraphiC) p);
          drw = Nlm_GetPanelDraw (p);
          if (drw != NULL && Nlm_GetVisible ((Nlm_GraphiC) p)) {
            drw (p);
            Nlm_ResetDrawingTools ();
          }
          p = n;
        }
      }
    }
  }
}

/* Message cracker functions */

static void MyCls_OnLButtonDown (HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)

{
  Nlm_PnlClckProc  clk;
  Nlm_SlateTool    h;
  Nlm_Int2         hOffset;
  Nlm_PaneL        n;
  Nlm_Boolean      notInside;
  Nlm_PaneL        p;
  Nlm_PanelData    pdata;
  Nlm_PoinT        pt;
  Nlm_RecT         r;
  Nlm_SlatE        s;
  Nlm_Int2         vOffset;

  s = (Nlm_SlatE) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  h = Nlm_GetSlateHandle (s);
  Nlm_ctrlKey = (Nlm_Boolean) ((MK_CONTROL & keyFlags) != 0);
  Nlm_shftKey = (Nlm_Boolean) ((MK_SHIFT & keyFlags) != 0);
  SetCapture (hwnd);
  currentSlate = s;
  Nlm_dblClick = fDoubleClick;
  if (Nlm_GetEnabled ((Nlm_GraphiC) s) && Nlm_GetVisible ((Nlm_GraphiC) s)) {
    SetTimer (h, 2, 100, NULL);
    slateTimerUsed = TRUE;
    slateTimer = h;
    Nlm_GetRect ((Nlm_GraphiC) s, &r);
    Nlm_LoadPt (&pt, (Nlm_Int2) x + r.left, (Nlm_Int2) y + r.top);
    recentlyClickedSlate = (Nlm_SlatE) s;
    recentlyClickedPanel = NULL;
    Nlm_SelectFont (Nlm_systemFont);
    Nlm_ResetDrawingTools ();
    Nlm_GetPanelData ((Nlm_PaneL) s, &pdata);
    if (pdata.click != NULL || pdata.drag != NULL || pdata.hold != NULL ||
        pdata.release != NULL || pdata.draw != NULL || pdata.reset != NULL) {
      p = (Nlm_PaneL) s;
      recentlyClickedPanel = p;
      clk = Nlm_GetPanelClick (p);
      if (clk != NULL && Nlm_GetEnabled ((Nlm_GraphiC) p) && Nlm_GetVisible ((Nlm_GraphiC) p)) {
        clk (p, pt);
        Nlm_Update ();
      }
    } else {
      p = Nlm_CreateReverseLinks ((Nlm_SlatE) s);
      notInside = TRUE;
      while (p != NULL && notInside) {
        n = Nlm_GetPanelPrev (p);
        Nlm_GetPanelData (p, &pdata);
        if (pdata.click != NULL || pdata.drag != NULL ||
            pdata.hold != NULL || pdata.release != NULL) {
          Nlm_GetRect ((Nlm_GraphiC) p, &r);
          Nlm_DoGetOffset ((Nlm_GraphiC) p, &hOffset, &vOffset);
          Nlm_OffsetRect (&r, -hOffset, -vOffset);
          if (Nlm_PtInRect (pt, &r) && Nlm_GetEnabled ((Nlm_GraphiC) p) &&
              Nlm_GetVisible ((Nlm_GraphiC) p)) {
            recentlyClickedPanel = p;
            notInside = FALSE;
            clk = Nlm_GetPanelClick (p);
            if (clk != NULL) {
              clk (p, pt);
              Nlm_Update ();
            }
          }
        }
        p = n;
      }
    }
  }
}

static void MyCls_OnMouseMove (HWND hwnd, int x, int y, UINT keyFlags)

{
  Nlm_PnlClckProc  drg;
  Nlm_PaneL        p;
  Nlm_PoinT        pt;
  Nlm_RecT         r;
  Nlm_SlatE        s;

  s = (Nlm_SlatE) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  if (currentSlate == s) {
    if (Nlm_GetEnabled ((Nlm_GraphiC) s) &&
        Nlm_GetVisible ((Nlm_GraphiC) s)) {
      Nlm_GetRect ((Nlm_GraphiC) s, &r);
      Nlm_LoadPt (&pt, (Nlm_Int2) x + r.left, (Nlm_Int2) y + r.top);
      if ((Nlm_SlatE) s == recentlyClickedSlate && recentlyClickedPanel != NULL) {
        p = recentlyClickedPanel;
        drg = Nlm_GetPanelDrag (p);
        if (drg != NULL) {
          Nlm_ResetDrawingTools ();
          drg (p, pt);
        }
      }
    }
  }
}

static void MyCls_OnLButtonUp (HWND hwnd, int x, int y, UINT keyFlags)

{
  Nlm_SlateTool    h;
  Nlm_PaneL        p;
  Nlm_PoinT        pt;
  Nlm_RecT         r;
  Nlm_PnlClckProc  rls;
  Nlm_SlatE        s;

  s = (Nlm_SlatE) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  ReleaseCapture ();
  h = Nlm_GetSlateHandle ((Nlm_SlatE) s);
  if (slateTimerUsed) {
    KillTimer (h, 2);
    slateTimer = NULL;
  }
  slateTimerUsed = FALSE;
  if (currentSlate == s) {
    if (Nlm_GetEnabled ((Nlm_GraphiC) s) &&
        Nlm_GetVisible ((Nlm_GraphiC) s)) {
      Nlm_GetRect ((Nlm_GraphiC) s, &r);
      Nlm_LoadPt (&pt, (Nlm_Int2) x + r.left, (Nlm_Int2) y + r.top);
      if ((Nlm_SlatE) s == recentlyClickedSlate && recentlyClickedPanel != NULL) {
        p = recentlyClickedPanel;
        rls = Nlm_GetPanelRelease (p);
        if (rls != NULL) {
          Nlm_ResetDrawingTools ();
          rls (p, pt);
        }
      }
      Nlm_SelectFont (Nlm_systemFont);
      Nlm_ResetDrawingTools ();
      recentlyClickedSlate = NULL;
      recentlyClickedPanel = NULL;
    }
  }
  currentSlate = NULL;
}

static void MyCls_OnTimer (HWND hwnd, UINT id)

{
  Nlm_PaneL        p;
  Nlm_PnlClckProc  prs;
  Nlm_PoinT        pt;
  Nlm_SlatE        s;

  s = (Nlm_SlatE) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  if (currentSlate == s) {
    if (Nlm_GetEnabled ((Nlm_GraphiC) s) && Nlm_GetVisible ((Nlm_GraphiC) s)) {
      Nlm_MousePosition (&pt);
      if ((Nlm_SlatE) s == recentlyClickedSlate && recentlyClickedPanel != NULL) {
        p = recentlyClickedPanel;
        prs = Nlm_GetPanelHold (p);
        if (prs != NULL) {
          Nlm_ResetDrawingTools ();
          prs (p, pt);
        }
      }
    }
  }
}

static void MyCls_OnPaint (HWND hwnd)

{
  PAINTSTRUCT    ps;
  Nlm_PointTool  ptool;
  Nlm_RectTool   rtool;
  Nlm_SlatE      s;
  Nlm_PortTool   temp;

  s = (Nlm_SlatE) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  temp = Nlm_currentHDC;
  GetUpdateRgn (hwnd, (Nlm_RgnTool) Nlm_updateRgn, TRUE);
  GetWindowRect (hwnd, &rtool);
  ptool = * (PPOINT) &rtool.left;
  ScreenToClient (Nlm_currentHWnd, &ptool);
  Nlm_OffsetRgn (Nlm_updateRgn, (Nlm_Int2) ptool.x, (Nlm_Int2) ptool.y);
  BeginPaint (hwnd, &ps);
  Nlm_currentHDC = Nlm_ParentWindowPort ((Nlm_GraphiC) s);
  SelectClipRgn (Nlm_currentHDC, (Nlm_RgnTool) Nlm_updateRgn);
  GetRgnBox ((Nlm_RgnTool) Nlm_updateRgn, &rtool);
  Nlm_RectToolToRecT (&rtool, &Nlm_updateRect);
  Nlm_DrawSlate (s);
  EndPaint (hwnd, &ps);
  GetWindowRect (Nlm_currentHWnd, &rtool);
  ptool = * (PPOINT) &rtool.left;
  SetRectRgn ((Nlm_RgnTool) Nlm_updateRgn, rtool.left,
              rtool.top, rtool.right, rtool.bottom);
  Nlm_OffsetRgn (Nlm_updateRgn, (Nlm_Int2) (-ptool.x), (Nlm_Int2) (-ptool.y));
  SelectClipRgn (Nlm_currentHDC, NULL);
  GetRgnBox ((Nlm_RgnTool) Nlm_updateRgn, &rtool);
  Nlm_RectToolToRecT (&rtool, &Nlm_updateRect);
  Nlm_currentHDC = temp;
}

static void MyCls_OnChar (HWND hwnd, UINT ch, int cRepeat)

{
  Nlm_SltCharProc  keyProc;
  Nlm_SlatE        s;

  s = (Nlm_SlatE) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  keyProc = Nlm_GetSlateCharProc (s);
  handlechar = FALSE;
  if (keyProc != NULL) {
    keyProc (s, (Nlm_Char) ch);
  } else if (ch == '\t') {
    Nlm_DoSendFocus ((Nlm_GraphiC) s, (Nlm_Char) ch);
  } else if (ch == '\n' || ch == '\r') {
    Nlm_DoSendFocus ((Nlm_GraphiC) s, (Nlm_Char) ch);
  } else {
    handlechar = TRUE;
  }
}

static void MyCls_OnSetFocus (HWND hwnd, HWND hwndOldFocus)

{
  Nlm_SlatE  s;

  s = (Nlm_SlatE) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  Nlm_ChangeSlateFocus (s, TRUE);
}

static void MyCls_OnKillFocus (HWND hwnd, HWND hwndNewFocus)

{
  Nlm_SlatE  s;

  s = (Nlm_SlatE) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  Nlm_ChangeSlateFocus (s, FALSE);
}

LRESULT CALLBACK EXPORT SlateProc (HWND hwnd, UINT message,
                                   WPARAM wParam, LPARAM lParam)

{
  LRESULT      rsult;
  Nlm_SlatE    s;
  HDC          tempHDC;
  HWND         tempHWnd;
  Nlm_Boolean  clearRecent;  /* dgg */

  if (Nlm_VibrantDisabled ()) {
    return DefWindowProc (hwnd, message, wParam, lParam);
  }

  rsult = 0;
  clearRecent = FALSE;
  tempHWnd = Nlm_currentHWnd;
  tempHDC = Nlm_currentHDC;
  s = (Nlm_SlatE) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  Nlm_theWindow = Nlm_GetParentWindow ((Nlm_GraphiC) s);
  Nlm_currentHWnd = GetParent (hwnd);
  Nlm_currentHDC = Nlm_ParentWindowPort ((Nlm_GraphiC) s);
  Nlm_currentWindowTool = hwnd;
  Nlm_currentKey = '\0';
  Nlm_currentWParam = wParam;
  Nlm_currentLParam = lParam;
  Nlm_cmmdKey = FALSE;
  Nlm_ctrlKey = FALSE;
  Nlm_optKey = FALSE;
  Nlm_shftKey = FALSE;
  Nlm_dblClick = FALSE;
  switch (message) {
    case WM_LBUTTONDBLCLK:
      HANDLE_WM_LBUTTONDBLCLK (hwnd, wParam, lParam, MyCls_OnLButtonDown);
      break;
    case WM_LBUTTONDOWN:
      HANDLE_WM_LBUTTONDOWN (hwnd, wParam, lParam, MyCls_OnLButtonDown);
      break;
    case WM_MOUSEMOVE:
      HANDLE_WM_MOUSEMOVE (hwnd, wParam, lParam, MyCls_OnMouseMove);
      break;
    case WM_TIMER:
      HANDLE_WM_TIMER (hwnd, wParam, lParam, MyCls_OnTimer);
      break;
    case WM_LBUTTONUP:
      HANDLE_WM_LBUTTONUP (hwnd, wParam, lParam, MyCls_OnLButtonUp);
#ifdef DCLAP
      clearRecent = TRUE;
#endif
      break;
    case WM_PAINT:
      HANDLE_WM_PAINT (hwnd, wParam, lParam, MyCls_OnPaint);
      break;
    case WM_CHAR:
      HANDLE_WM_CHAR (hwnd, wParam, lParam, MyCls_OnChar);
      if (handlechar) {
        rsult = DefWindowProc (hwnd, message, wParam, lParam);
      }
      break;
    case WM_SETFOCUS:
      HANDLE_WM_SETFOCUS (hwnd, wParam, lParam, MyCls_OnSetFocus);
      rsult = DefWindowProc (hwnd, message, wParam, lParam);
      break;
    case WM_KILLFOCUS:
      HANDLE_WM_KILLFOCUS (hwnd, wParam, lParam, MyCls_OnKillFocus);
      rsult = DefWindowProc (hwnd, message, wParam, lParam);
      break;
    default:
      rsult = DefWindowProc (hwnd, message, wParam, lParam);
#ifdef DCLAP
      clearRecent = TRUE;
#endif
      break;
  }

/* jk modification of dgg addition -- clear problems w/ drag message appearing after mouseup */
  if (clearRecent) {
    recentlyClickedSlate = NULL;
    recentlyClickedPanel = NULL;
  }

  Nlm_currentHWnd = tempHWnd;
  Nlm_currentHDC = tempHDC;
  Nlm_currentWindowTool = tempHWnd;
  return rsult;
}

extern Nlm_Boolean Nlm_RegisterSlates (void)

{
  Nlm_Boolean  rsult;
  WNDCLASS     wc;

  rsult = FALSE;
  wc.style = CS_PARENTDC | CS_DBLCLKS;
  wc.lpfnWndProc = SlateProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = Nlm_currentHInst;
  wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor (NULL, IDC_ARROW);
  wc.hbrBackground = GetStockObject (HOLLOW_BRUSH);
  wc.lpszMenuName = NULL;
  sprintf (slateclass, "Nlm_SlateClass%ld", (long) (int) Nlm_currentHInst);
  wc.lpszClassName = slateclass;
  rsult = (Nlm_Boolean) RegisterClass (&wc);
  return rsult;
}
#endif

#ifdef WIN_MOTIF
extern Nlm_Boolean Nlm_RegisterSlates (void)

{
  XtActionsRec  actions;

  actions.string = "slate";
  actions.proc = Nlm_SlateClickCallback;
  XtAppAddActions (Nlm_appContext, &actions, 1);
  actions.string = "piano";
  actions.proc = Nlm_SlateKeyCallback;
  XtAppAddActions (Nlm_appContext, &actions, 1);
  return TRUE;
}
#endif

extern void Nlm_KillSlateTimer (void)

{
  recentlyClickedSlate = NULL;
  recentlyClickedPanel = NULL;
#ifdef WIN_MAC
#endif
#ifdef WIN_MSWIN
  if (slateTimerUsed) {
    KillTimer (slateTimer, 2);
    slateTimer = NULL;
  }
  slateTimerUsed = FALSE;
#endif
#ifdef WIN_MOTIF
  if (slateTimerUsed) {
    XtRemoveTimeOut (slateTimer);
    slateTimer = 0;
  }
  slateTimerUsed = FALSE;
#endif
}

extern void Nlm_FreeSlate (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemFree (gphprcsptr);
}

extern void Nlm_InitSlate (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemNew (sizeof (Nlm_GphPrcs) * 3);

  slateProcs = &(gphprcsptr [0]);
#ifdef WIN_MAC
  slateProcs->click = Nlm_SlateClick;
  slateProcs->key = Nlm_SlateKey;
  slateProcs->draw = Nlm_DrawSlate;
#endif
#ifdef WIN_MSWIN
  slateProcs->command = Nlm_SlateCommand;
#endif
#ifdef WIN_MOTIF
  slateProcs->callback = Nlm_SlateCallback;
#endif
  slateProcs->show = Nlm_ShowSlate;
  slateProcs->hide = Nlm_HideSlate;
  slateProcs->enable = Nlm_EnableSlate;
  slateProcs->disable = Nlm_DisableSlate;
  slateProcs->remove = Nlm_RemoveSlate;
  slateProcs->reset = Nlm_ResetSlate;
  slateProcs->select = Nlm_SelectSlate;
  slateProcs->countItems = Nlm_CountGroupItems;
  slateProcs->linkIn = Nlm_LinkIn;
  slateProcs->adjustPrnt = Nlm_AdjustSlate;
  slateProcs->setOffset = Nlm_SetSlateOffset;
  slateProcs->getOffset = Nlm_GetSlateOffset;
  slateProcs->setPosition = Nlm_SetSlatePosition;
  slateProcs->getPosition = Nlm_GetSlatePosition;
  slateProcs->setRange = Nlm_SetSlateRange;
  slateProcs->gainFocus = Nlm_SlateGainFocus;
  slateProcs->loseFocus = Nlm_SlateLoseFocus;

  panelProcs = &(gphprcsptr [1]);
  panelProcs->show = Nlm_ShowPanel;
  panelProcs->hide = Nlm_HidePanel;
  panelProcs->enable = Nlm_EnablePanel;
  panelProcs->disable = Nlm_DisablePanel;
  panelProcs->remove = Nlm_RemovePanel;
  panelProcs->reset = Nlm_ResetPanel;
  panelProcs->select = Nlm_SelectPanel;
  panelProcs->getOffset = Nlm_GetPanelOffset;
  panelProcs->setPosition = Nlm_SetPanelPosition;
  panelProcs->getPosition = Nlm_GetPanelPosition;

  displayProcs = &(gphprcsptr [2]);
  displayProcs->select = Nlm_SelectDisplay;
  displayProcs->sendChar = Nlm_PaintOrFormatChar;
}
