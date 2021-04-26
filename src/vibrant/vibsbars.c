/*   vibsbars.c
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
* File Name:  vibsbars.c
*
* Author:  Jonathan Kans
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.10 $
*
* File Description: 
*       Vibrant scroll bar functions
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

/* Note that the scroll bar tool type is also duplicated in vibwndws.c */

#ifdef WIN_MAC
#define Nlm_BarTool ControlHandle
#endif

#ifdef WIN_MSWIN
#define Nlm_BarTool HWND
#endif

#ifdef WIN_MOTIF
#define Nlm_BarTool Widget
#endif

typedef  struct  Nlm_bardata {
  Nlm_BarTool   handle;
  Nlm_Int2      pageUp;
  Nlm_Int2      pageDn;
  Nlm_Int2      delta;
  Nlm_Int2      value;
  Nlm_Int2      maximum;
  Nlm_Boolean   anomaly;
  Nlm_GraphiC   slave;
  Nlm_ScrlProc  action;
} Nlm_BarData;

typedef  struct  Nlm_barrec {
  Nlm_GraphicRec  graphicR;
  Nlm_BarData     bar;
} Nlm_BarRec, PNTR Nlm_BarPtr;

static Nlm_GphPrcsPtr  gphprcsptr = NULL;

static Nlm_GphPrcsPtr  barProcs;
static Nlm_GphPrcsPtr  linkedBarProcs;

static Nlm_BaR         recentBar = NULL;
static Nlm_BarData     recentBarData;

#ifdef WIN_MSWIN
static WNDPROC         lpfnNewBarProc = NULL;
static WNDPROC         lpfnOldBarProc = NULL;
static Nlm_Boolean     handlechar;
#endif

static void Nlm_LoadBarData (Nlm_BaR b, Nlm_BarTool hdl,
                             Nlm_Int2 pgUp, Nlm_Int2 pgDn,
                             Nlm_Int2 dlta, Nlm_Int2 val,
                             Nlm_Int2 max, Nlm_Boolean anomaly,
                             Nlm_GraphiC slv, Nlm_ScrlProc actn)

{
  Nlm_BarPtr   bp;
  Nlm_BarData  PNTR bdptr;

  if (b != NULL) {
    bp = (Nlm_BarPtr) Nlm_HandLock (b);
    bdptr = &(bp->bar);
    bdptr->handle = hdl;
    bdptr->pageUp = pgUp;
    bdptr->pageDn = pgDn;
    bdptr->delta = dlta;
    bdptr->value = val;
    bdptr->maximum = max;
    bdptr->anomaly = anomaly;
    bdptr->slave = slv;
    bdptr->action = actn;
    Nlm_HandUnlock (b);
    recentBar = NULL;
  }
}

static void Nlm_SetBarData (Nlm_BaR b, Nlm_BarData * bdata)

{
  Nlm_BarPtr  bp;

  if (b != NULL && bdata != NULL) {
    bp = (Nlm_BarPtr) Nlm_HandLock (b);
    bp->bar = *bdata;
    Nlm_HandUnlock (b);
    recentBar = b;
    recentBarData = *bdata;
  }
}

static void Nlm_GetBarData (Nlm_BaR b, Nlm_BarData * bdata)

{
  Nlm_BarPtr  bp;

  if (b != NULL && bdata != NULL) {
    if (b == recentBar) {
      *bdata = recentBarData;
    } else {
      bp = (Nlm_BarPtr) Nlm_HandLock (b);
      *bdata = bp->bar;
      Nlm_HandUnlock (b);
      recentBar = b;
      recentBarData = *bdata;
    }
  }
}

static Nlm_BarTool Nlm_GetBarHandle (Nlm_BaR b)

{
  Nlm_BarData  bdata;

  Nlm_GetBarData (b, &bdata);
  return bdata.handle;
}

static void Nlm_SetBarDelta (Nlm_BaR b, Nlm_Int2 dlt)

{
  Nlm_BarData  bdata;

  Nlm_GetBarData (b, &bdata);
  bdata.delta = dlt;
  Nlm_SetBarData (b, &bdata);
}

static Nlm_Int2 Nlm_GetBarMax (Nlm_BaR b)

{
  Nlm_BarData  bdata;
  Nlm_Int2     rsult;

  rsult = 0;
  if (b != NULL) {
    Nlm_GetBarData (b, &bdata);
    rsult = bdata.maximum;
  }
  return rsult;
}

static Nlm_Int2 Nlm_GetBarPageUp (Nlm_BaR b)

{
  Nlm_BarData  bdata;
  Nlm_Int2     rsult;

  rsult = 0;
  if (b != NULL) {
    Nlm_GetBarData (b, &bdata);
    rsult = bdata.pageUp;
  }
  return rsult;
}

static Nlm_Int2 Nlm_GetBarPageDown (Nlm_BaR b)

{
  Nlm_BarData  bdata;
  Nlm_Int2     rsult;

  rsult = 0;
  if (b != NULL) {
    Nlm_GetBarData (b, &bdata);
    rsult = bdata.pageDn;
  }
  return rsult;
}

static void Nlm_DoScrollAction (Nlm_BaR b)

{
  Nlm_ScrlProc  actn;
  Nlm_BarData   bdata;

  Nlm_GetBarData (b, &bdata);
  actn = bdata.action;
  if (actn != NULL) {
    actn (b, bdata.slave, bdata.value, bdata.delta + bdata.value);
  }
}

#ifdef WIN_MAC
pascal void ScrollAction PROTO((Nlm_BarTool c, Nlm_Int2 part));
pascal void ScrollAction (Nlm_BarTool c, Nlm_Int2 part)

{
  Nlm_BaR   b;
  Nlm_Int2  max;
  Nlm_Int2  newval;
  Nlm_Int2  oldval;
  Nlm_Int2  pageUp;
  Nlm_Int2  pageDn;
  Nlm_Int4  ref;

  ref = GetCRefCon (c);
  b = (Nlm_BaR) ref;
  oldval = Nlm_DoGetValue ((Nlm_GraphiC) b);
  max = Nlm_GetBarMax (b);
  pageUp = Nlm_GetBarPageUp (b);
  pageDn = Nlm_GetBarPageDown (b);
  switch (part) {
    case inUpButton:
      if (oldval > 0) {
        newval = oldval - 1;
        Nlm_DoSetValue ((Nlm_GraphiC) b, newval, TRUE);
      } else {
        Nlm_DoSetValue ((Nlm_GraphiC) b, 0, TRUE);
      }
      break;
    case inDownButton:
      if (oldval < max) {
        newval = oldval + 1;
        Nlm_DoSetValue ((Nlm_GraphiC) b, newval, TRUE);
      } else {
        Nlm_DoSetValue ((Nlm_GraphiC) b, max, TRUE);
      }
      break;
    case inPageUp:
      if (oldval > pageUp) {
        newval = oldval - pageUp;
      } else {
        newval = 0;
      }
      Nlm_DoSetValue ((Nlm_GraphiC) b, newval, TRUE);
      break;
    case inPageDown:
      if (oldval + pageDn < max) {
        newval = oldval + pageDn;
      } else {
        newval = max;
      }
      Nlm_DoSetValue ((Nlm_GraphiC) b, newval, TRUE);
      break;
    default:
      break;
  }
}

static Nlm_Boolean Nlm_ScrollClick (Nlm_GraphiC b, Nlm_PoinT pt)

{
  Nlm_BarData    bdata;
  Nlm_BarTool    c;
  Nlm_Int2       newval;
  Nlm_Int2       oldval;
  Nlm_Int2       part;
  ProcPtr        proc;
  Nlm_PointTool  ptool;
  Nlm_RecT       r;
  Nlm_Boolean    rsult;
  pascal void    (*sp) PROTO((Nlm_BarTool, Nlm_Int2));

  rsult = FALSE;
  Nlm_GetRect (b, &r);
  if ((Nlm_PtInRect (pt, &r))) {
    c = Nlm_GetBarHandle ((Nlm_BaR) b);
    oldval = GetCtlValue (c);
    Nlm_PoinTToPointTool (pt, &ptool);
    part = TestControl (c, ptool);
    if (part == inThumb) {
      part = TrackControl (c, ptool, NULL);
      newval = GetCtlValue (c);
      Nlm_GetBarData ((Nlm_BaR) b, &bdata);
      bdata.value = newval;
      bdata.delta = oldval - newval;
      Nlm_SetBarData ((Nlm_BaR) b, &bdata);
      if (oldval != newval) {
        Nlm_DoScrollAction ((Nlm_BaR) b);
      }
    } else {
      sp = ScrollAction;
      proc = (ProcPtr) sp;
#if  defined(powerc) || defined(__powerc)  || defined(COMP_CODEWAR)
	  {  
	  ControlActionUPP caupp= NewControlActionProc( proc);
      part = TrackControl (c, ptool, caupp);
      }
#else
      part = TrackControl (c, ptool, proc);
#endif
    }
    rsult = TRUE;
  }
  return rsult;
}
#endif

#ifdef WIN_MSWIN

static Nlm_Boolean Nlm_ScrollCommand (Nlm_GraphiC b)

{
  Nlm_Int2  max;
  Nlm_Int2  newval;
  Nlm_Int2  oldval;
  Nlm_Int2  pageUp;
  Nlm_Int2  pageDn;

  oldval = Nlm_DoGetValue (b);
  max = Nlm_GetBarMax ((Nlm_BaR) b);
  pageUp = Nlm_GetBarPageUp ((Nlm_BaR) b);
  pageDn = Nlm_GetBarPageDown ((Nlm_BaR) b);
  switch (Nlm_currentCode) {
    case SB_LINEUP:
      if (oldval > 0) {
        newval = oldval - 1;
        Nlm_DoSetValue (b, newval, TRUE);
      } else {
        Nlm_DoSetValue (b, 0, TRUE);
      }
      break;
    case SB_LINEDOWN:
      if (oldval < max) {
        newval = oldval + 1;
        Nlm_DoSetValue (b, newval, TRUE);
      } else {
        Nlm_DoSetValue (b, max, TRUE);
      }
      break;
    case SB_PAGEUP:
      if (oldval > pageUp) {
        newval = oldval - pageUp;
      } else {
        newval = 0;
      }
      Nlm_DoSetValue (b, newval, TRUE);
      break;
    case SB_PAGEDOWN:
      if (oldval + pageDn < max) {
        newval = oldval + pageDn;
      } else {
        newval = max;
      }
      Nlm_DoSetValue (b, newval, TRUE);
      break;
    case SB_THUMBPOSITION:
      newval = Nlm_currentPos;
      Nlm_DoSetValue (b, newval, TRUE);
      break;
    default:
      break;
  }
  return FALSE;
}
#endif

#ifdef WIN_MOTIF
#endif

#ifdef WIN_MAC
static void Nlm_DrawBar (Nlm_GraphiC b)

{
  Nlm_BarData  bdata;
  Nlm_BarTool  c;
  Nlm_RecT     r;

  if (Nlm_GetVisible (b) && Nlm_GetAllParentsVisible (b)) {
    Nlm_GetBarData ((Nlm_BaR) b, &bdata);
    Nlm_GetRect (b, &r);
    if (Nlm_RectInRgn (&r, Nlm_updateRgn)) {
      c = bdata.handle;
      if (bdata.maximum != GetCtlMax (c)) {
        SetCtlMax (c, bdata.maximum);
      }
      if (bdata.value != GetCtlValue (c)) {
        SetCtlValue (c, bdata.value);
      }
      Draw1Control (c);
    }
  }
}
#endif

static void Nlm_ShowBar (Nlm_GraphiC b, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_BarTool  c;
  Nlm_WindoW   tempPort;

  if (setFlag) {
    Nlm_SetVisible (b, TRUE);
  }
  if (Nlm_GetVisible (b) && Nlm_AllParentsButWindowVisible (b)) {
    tempPort = Nlm_SavePortIfNeeded (b, savePort);
    c = Nlm_GetBarHandle ((Nlm_BaR) b);
#ifdef WIN_MAC
    ShowControl (c);
    Nlm_DoDraw (b);
#endif
#ifdef WIN_MSWIN
    ShowWindow (c, SW_SHOW);
    UpdateWindow (c);
#endif
#ifdef WIN_MOTIF
    XtManageChild (c);
#endif
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_HideBar (Nlm_GraphiC b, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_BarTool  c;
  Nlm_WindoW   tempPort;
#ifdef WIN_MAC
  Nlm_RecT     r;
#endif

  if (setFlag) {
    Nlm_SetVisible (b, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (b, savePort);
  c = Nlm_GetBarHandle ((Nlm_BaR) b);
#ifdef WIN_MAC
  HideControl (c);
  if (Nlm_GetAllParentsVisible (b)) {
    Nlm_GetRect (b, &r);
    Nlm_InsetRect (&r, -1, -1);
    Nlm_EraseRect (&r);
    Nlm_ValidRect (&r);
  }
#endif
#ifdef WIN_MSWIN
  ShowWindow (c, SW_HIDE);
  UpdateWindow (c);
#endif
#ifdef WIN_MOTIF
  XtUnmanageChild (c);
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_EnableBar (Nlm_GraphiC b, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_BarData  bdata;
  Nlm_BarTool  c;
  Nlm_WindoW   tempPort;
#ifdef WIN_MSWIN
  int          maximum;
  int          minimum;
#endif
#ifdef WIN_MOTIF
  int          maximum;
  int          value;
  Cardinal     n;
  Arg          wargs [10];
#endif

  if (setFlag) {
    Nlm_SetEnabled (b, TRUE);
  }
  if (Nlm_GetEnabled (b) && Nlm_GetAllParentsEnabled (b)) {
    tempPort = Nlm_SavePortIfNeeded (b, savePort);
    Nlm_GetBarData ((Nlm_BaR) b, &bdata);
    c = bdata.handle;
#ifdef WIN_MAC
    if (bdata.maximum != GetCtlMax (c)) {
      SetCtlMax (c, bdata.maximum);
    }
    if (bdata.value != GetCtlValue (c)) {
      SetCtlValue (c, bdata.value);
    }
    HiliteControl (c, 0);
#endif
#ifdef WIN_MSWIN
    EnableWindow (c, TRUE);
    GetScrollRange (c, SB_CTL, &minimum, &maximum);
    if (bdata.maximum != (Nlm_Int2) maximum) {
      SetScrollRange (c, SB_CTL, 0, bdata.maximum, TRUE);
    }
    if (bdata.value != GetScrollPos (c, SB_CTL)) {
      SetScrollPos (c, SB_CTL, bdata.value, TRUE);
    }
#endif
#ifdef WIN_MOTIF
    XtVaGetValues (c, XmNmaximum, &maximum, XmNvalue, &value, NULL);
    n = 0;
    if (bdata.maximum != maximum) {
      XtSetArg (wargs [n], XmNmaximum, (int) (bdata.maximum + 20)); n++;
    }
    if (bdata.value != value) {
      XtSetArg (wargs [n], XmNvalue, (int) bdata.value); n++;
    }
    XtSetArg (wargs [n], XmNsensitive, TRUE); n++;
    XtSetValues (c, wargs, n);
#endif
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_DisableBar (Nlm_GraphiC b, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_BarTool  c;
  Nlm_WindoW   tempPort;

  if (setFlag) {
    Nlm_SetEnabled (b, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (b, savePort);
  c = Nlm_GetBarHandle ((Nlm_BaR) b);
#ifdef WIN_MAC
  HiliteControl (c, 255);
#endif
#ifdef WIN_MSWIN
  EnableWindow (c, FALSE);
#endif
#ifdef WIN_MOTIF
  XtVaSetValues (c, XmNsensitive, FALSE, NULL);
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_ResetBar (Nlm_GraphiC b, Nlm_Boolean savePort)

{
  Nlm_BarData  bdata;
  Nlm_BarTool  c;
  Nlm_WindoW   tempPort;
#ifdef WIN_MOTIF
  Cardinal     n;
  Arg          wargs [10];
#endif

  tempPort = Nlm_SavePortIfNeeded (b, savePort);
  Nlm_GetBarData ((Nlm_BaR) b, &bdata);
  bdata.pageUp = 0;
  bdata.pageDn = 0;
  bdata.delta = 0;
  bdata.value = 0;
  bdata.maximum = 0;
  Nlm_SetBarData ((Nlm_BaR) b, &bdata);
  c = bdata.handle;
  if (Nlm_GetEnabled (b)) {
#ifdef WIN_MAC
    SetCtlValue (c, 0);
    SetCtlMax (c, 0);
#endif
#ifdef WIN_MSWIN
    SetScrollRange (c, SB_CTL, 0, 0, FALSE);
    SetScrollPos (c, SB_CTL, 0, TRUE);
#endif
#ifdef WIN_MOTIF
    n = 0;
    XtSetArg (wargs [n], XmNminimum, 0); n++;
    XtSetArg (wargs [n], XmNmaximum, 20); n++;
    XtSetArg (wargs [n], XmNincrement, 1); n++;
    XtSetArg (wargs [n], XmNpageIncrement, 1); n++;
    XtSetArg (wargs [n], XmNvalue, 0); n++;
    XtSetValues (c, wargs, n);
#endif
}
  Nlm_RestorePort (tempPort);
}

static void Nlm_RemoveBar (Nlm_GraphiC b, Nlm_Boolean savePort)

{
  Nlm_BarTool  c;
  Nlm_WindoW   tempPort;

  tempPort = Nlm_SavePortIfNeeded (b, savePort);
  c = Nlm_GetBarHandle ((Nlm_BaR) b);
#ifdef WIN_MAC
  DisposeControl (c);
#endif
#ifdef WIN_MSWIN
  RemoveProp (c, (LPSTR) "Nlm_VibrantProp");
  DestroyWindow (c);
#endif
#ifdef WIN_MOTIF
  XtDestroyWidget (c);
#endif
  Nlm_HandFree (b);
  recentBar = NULL;
  Nlm_RestorePort (tempPort);
}

static void Nlm_RemoveLinkedInBar (Nlm_GraphiC b, Nlm_Boolean savePort)

{
  Nlm_BarTool  c;
  Nlm_WindoW   tempPort;

  tempPort = Nlm_SavePortIfNeeded (b, savePort);
  c = Nlm_GetBarHandle ((Nlm_BaR) b);
#ifdef WIN_MAC
  DisposeControl (c);
#endif
#ifdef WIN_MSWIN
  RemoveProp (c, (LPSTR) "Nlm_VibrantProp");
  DestroyWindow (c);
#endif
#ifdef WIN_MOTIF
  XtDestroyWidget (c);
#endif
  Nlm_RemoveLink (b);
  recentBar = NULL;
  Nlm_RestorePort (tempPort);
}

static void Nlm_SetBarValue (Nlm_GraphiC b, Nlm_Int2 val, Nlm_Boolean savePort)

{
  Nlm_BarData  bdata;
  Nlm_BarTool  c;
  Nlm_Int2     oldval;
  Nlm_WindoW   tempPort;
#ifdef WIN_MOTIF
  Nlm_Int2     page;
#endif

  if (b != NULL) {
    Nlm_GetBarData ((Nlm_BaR) b, &bdata);
    if (val < 0) {
      val = 0;
    }
    if (val > bdata.maximum) {
      val = bdata.maximum;
    }
    c = bdata.handle;
    oldval = bdata.value;
    bdata.value = val;
    bdata.delta = oldval - val;
    Nlm_SetBarData ((Nlm_BaR) b, &bdata);
    if (val != oldval && Nlm_GetEnabled (b)) {
      tempPort = Nlm_SavePortIfNeeded (b, savePort);
#ifdef WIN_MAC
      SetCtlValue (c, val);
#endif
#ifdef WIN_MSWIN
      SetScrollPos (c, SB_CTL, val, TRUE);
#endif
#ifdef WIN_MOTIF
      page = (bdata.pageUp + bdata.pageDn) / 2;
      XmScrollBarSetValues (c, (int) val, 20, 1, (int) page, FALSE);
#endif
      Nlm_DoScrollAction ((Nlm_BaR) b);
      Nlm_RestorePort (tempPort);
    }
  }
}

static Nlm_Int2 Nlm_GetBarValue (Nlm_GraphiC b)

{
  Nlm_BarData  bdata;
  Nlm_Int2     rsult;

  rsult = 0;
  if (b != NULL) {
    Nlm_GetBarData ((Nlm_BaR) b, &bdata);
    rsult = bdata.value;
  }
  return rsult;
}

static void Nlm_InvalBar (Nlm_GraphiC b)

{
  Nlm_RecT  r;

  if (Nlm_GetVisible (b) && Nlm_GetAllParentsVisible (b)) {
    Nlm_GetRect (b, &r);
    Nlm_InsetRect (&r, -1, -1);
    Nlm_InvalRect (&r);
  }
}

static void Nlm_SetBarPosition (Nlm_GraphiC b, Nlm_RectPtr r, Nlm_Boolean savePort)

{
  Nlm_BarTool  c;
  Nlm_WindoW   tempPort;

  if (r != NULL) {
    tempPort = Nlm_SavePortIfNeeded (b, savePort);
    c = Nlm_GetBarHandle ((Nlm_BaR) b);
#ifdef WIN_MAC
    Nlm_InvalBar (b);
    MoveControl (c, r->left, r->top);
    SizeControl (c, r->right - r->left, r->bottom - r->top);
    Nlm_SetRect (b, r);
    Nlm_InvalBar (b);
#endif
#ifdef WIN_MSWIN
    Nlm_SetRect (b, r);
    MoveWindow (c, r->left, r->top, r->right - r->left, r->bottom - r->top, TRUE);
    UpdateWindow (c);
#endif
#ifdef WIN_MOTIF
      XtVaSetValues (c,
                     XmNx, (Position) r->left,
                     XmNy, (Position) r->top,
                     XmNwidth, (Dimension) (r->right - r->left),
                     XmNheight, (Dimension) (r->bottom - r->top), 
                     NULL);
    Nlm_SetRect (b, r);
#endif
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_GetBarPosition (Nlm_GraphiC b, Nlm_RectPtr r)

{
  if (r != NULL) {
    Nlm_GetRect (b, r);
  }
}

static void Nlm_SetBarRange (Nlm_GraphiC b, Nlm_Int2 pgUp,
                             Nlm_Int2 pgDn, Nlm_Int2 xmax,
                             Nlm_Boolean savePort)

{
  Nlm_BarData  bdata;
  Nlm_BarTool  c;
  Nlm_Boolean  changed;
  Nlm_WindoW   tempPort;
#ifdef WIN_MOTIF
  Cardinal     n;
  Arg          wargs [10];
#endif

  if (b != NULL) {
    Nlm_GetBarData ((Nlm_BaR) b, &bdata);
    if (bdata.pageUp != pgUp || bdata.pageDn != pgDn || bdata.maximum != xmax) {
      changed = FALSE;
      if (pgUp > 0) {
        bdata.pageUp = pgUp;
        changed = TRUE;
      }
      if (pgDn > 0) {
        bdata.pageDn = pgDn;
        changed = TRUE;
      }
      if (xmax > 0) {
        bdata.maximum = xmax;
        changed = TRUE;
      }
      if (changed) {
        Nlm_SetBarData ((Nlm_BaR) b, &bdata);
      }
      if (changed && xmax > 0 && Nlm_GetEnabled (b)) {
        tempPort = Nlm_SavePortIfNeeded (b, savePort);
        c = bdata.handle;
#ifdef WIN_MAC
        SetCtlMax (c, xmax);
#endif
#ifdef WIN_MSWIN
        SetScrollRange (c, SB_CTL, 0, xmax, TRUE);
#endif
#ifdef WIN_MOTIF
        n = 0;
        XtSetArg (wargs [n], XmNminimum, 0); n++;
        XtSetArg (wargs [n], XmNmaximum, (int) xmax + 20); n++;
        XtSetValues (c, wargs, n);
#endif
        Nlm_RestorePort (tempPort);
      }
    }
  }
}

#ifdef WIN_MSWIN
/* Message cracker functions */

static void MyCls_OnChar (HWND hwnd, UINT ch, int cRepeat)

{
  Nlm_BaR  b;

  b = (Nlm_BaR) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  handlechar = FALSE;
  if (ch == '\t') {
    Nlm_DoSendFocus ((Nlm_GraphiC) b, (Nlm_Char) ch);
  } else if (ch == '\n' || ch == '\r') {
    Nlm_DoSendFocus ((Nlm_GraphiC) b, (Nlm_Char) ch);
  } else {
    handlechar = TRUE;
  }
}

LRESULT CALLBACK EXPORT BarProc (HWND hwnd, UINT message,
                                 WPARAM wParam, LPARAM lParam)

{
  Nlm_BaR  b;
  LRESULT  rsult;
  HDC      tempHDC;
  HWND     tempHWnd;

  if (Nlm_VibrantDisabled ()) {
    return CallWindowProc (lpfnOldBarProc, hwnd, message, wParam, lParam);
  }

  rsult = 0;
  tempHWnd = Nlm_currentHWnd;
  tempHDC = Nlm_currentHDC;
  b = (Nlm_BaR) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  Nlm_theWindow = Nlm_GetParentWindow ((Nlm_GraphiC) b);
  Nlm_currentHWnd = GetParent (hwnd);
  Nlm_currentHDC = Nlm_ParentWindowPort ((Nlm_GraphiC) b);
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
    case WM_CHAR:
      HANDLE_WM_CHAR (hwnd, wParam, lParam, MyCls_OnChar);
      if (handlechar) {
        rsult = CallWindowProc (lpfnOldBarProc, hwnd, message, wParam, lParam);
      }
      break;
    default:
      rsult = CallWindowProc (lpfnOldBarProc, hwnd, message, wParam, lParam);
      break;
  }
  Nlm_currentHWnd = tempHWnd;
  Nlm_currentHDC = tempHDC;
  Nlm_currentWindowTool = tempHWnd;
  return rsult;
}
#endif

#ifdef WIN_MOTIF
#define SCROLL_LINEUP 1
#define SCROLL_LINEDN 2
#define SCROLL_PAGEUP 3
#define SCROLL_PAGEDN 4
#define SCROLL_THUMB  5

static void Nlm_ScrollClick (Nlm_GraphiC b, Nlm_Int2 type, Nlm_Int2 thumb)

{
  Nlm_BarData  bdata;
  Nlm_BarTool  c;
  Nlm_Int2     max;
  Nlm_Int2     newval;
  Nlm_Int2     oldval;
  Nlm_Int2     page;
  Nlm_Int2     pageUp;
  Nlm_Int2     pageDn;

  Nlm_GetBarData ((Nlm_BaR) b, &bdata);
  c = bdata.handle;
  oldval = bdata.value;
  max = bdata.maximum;
  pageUp = bdata.pageUp;
  pageDn = bdata.pageDn;
  switch (type) {
    case SCROLL_LINEUP:
      if (oldval > 0) {
        newval = oldval - 1;
      } else {
        newval = 0;
      }
      break;
    case SCROLL_LINEDN:
      if (oldval < max) {
        newval = oldval + 1;
      } else {
        newval = max;
      }
      break;
    case SCROLL_PAGEUP:
      if (oldval > pageUp) {
        newval = oldval - pageUp;
      } else {
        newval = 0;
      }
      break;
    case SCROLL_PAGEDN:
      if (oldval + pageDn < max) {
        newval = oldval + pageDn;
      } else {
        newval = max;
      }
      break;
    case SCROLL_THUMB:
      newval = thumb;
      break;
    default:
      break;
  }
  bdata.value = newval;
  bdata.delta = oldval - newval;
  Nlm_SetBarData ((Nlm_BaR) b, &bdata);
  if (thumb != newval) {
    page = (pageUp + pageDn) / 2;
    XmScrollBarSetValues (c, (int) newval, 20, 1, (int) page, FALSE);
  }
  if (oldval != newval) {
    Nlm_DoScrollAction ((Nlm_BaR) b);
  }
}

static void Nlm_LineUpCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  Nlm_GraphiC                b;
  XmScrollBarCallbackStruct  *cbs;
  Nlm_Int2                   thumb;

  cbs = (XmScrollBarCallbackStruct *) call_data;
  b = (Nlm_GraphiC) client_data;
  thumb = (Nlm_Int2) cbs->value;
  Nlm_ScrollClick (b, SCROLL_LINEUP, thumb);
}

static void Nlm_LineDnCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  Nlm_GraphiC                b;
  XmScrollBarCallbackStruct  *cbs;
  Nlm_Int2                   thumb;

  cbs = (XmScrollBarCallbackStruct *) call_data;
  b = (Nlm_GraphiC) client_data;
  thumb = (Nlm_Int2) cbs->value;
  Nlm_ScrollClick (b, SCROLL_LINEDN, thumb);
}

static void Nlm_PageUpCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  Nlm_GraphiC                b;
  XmScrollBarCallbackStruct  *cbs;
  Nlm_Int2                   thumb;

  cbs = (XmScrollBarCallbackStruct *) call_data;
  b = (Nlm_GraphiC) client_data;
  thumb = (Nlm_Int2) cbs->value;
  Nlm_ScrollClick (b, SCROLL_PAGEUP, thumb);
}

static void Nlm_PageDnCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  Nlm_GraphiC                b;
  XmScrollBarCallbackStruct  *cbs;
  Nlm_Int2                   thumb;

  cbs = (XmScrollBarCallbackStruct *) call_data;
  b = (Nlm_GraphiC) client_data;
  thumb = (Nlm_Int2) cbs->value;
  Nlm_ScrollClick (b, SCROLL_PAGEDN, thumb);
}

static void Nlm_ThumbCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  Nlm_GraphiC                b;
  XmScrollBarCallbackStruct  *cbs;
  Nlm_Int2                   thumb;

  cbs = (XmScrollBarCallbackStruct *) call_data;
  b = (Nlm_GraphiC) client_data;
  thumb = (Nlm_Int2) cbs->value;
  Nlm_ScrollClick (b, SCROLL_THUMB, thumb);
}

static void Nlm_TopCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  Nlm_GraphiC  b;

  b = (Nlm_GraphiC) client_data;
  Nlm_ScrollClick (b, SCROLL_THUMB, 0);
}

static void Nlm_BottomCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  Nlm_GraphiC  b;
  Nlm_Int2     max;

  b = (Nlm_GraphiC) client_data;
  max = Nlm_GetBarMax ((Nlm_BaR) b);
  Nlm_ScrollClick (b, SCROLL_THUMB, max);
}
#endif

static void Nlm_NewBar (Nlm_BaR b, Nlm_GraphiC slv,
                        Nlm_Boolean vert, Nlm_ScrlProc actn)

{
  Nlm_BarTool     c;
  Nlm_RecT        r;
  Nlm_Int4        style;
  Nlm_WindowTool  wptr;
#ifdef WIN_MAC
  Nlm_Int4        ref;
  Nlm_RectTool    rtool;
#endif
#ifdef WIN_MOTIF
  Cardinal        n;
  Arg             wargs [20];
#endif

  Nlm_GetRect ((Nlm_GraphiC) b, &r);
  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) b);
#ifdef WIN_MAC
  Nlm_RecTToRectTool (&r, &rtool);
  ref = (Nlm_Int4) b;
  c = NewControl (wptr, &rtool, (StringPtr) "", FALSE, 0, 0, 0, 16, ref);
  Nlm_LoadBarData (b, c, 0, 0, 0, 0, 0, FALSE, slv, actn);
#endif
#ifdef WIN_MSWIN
  if (vert) {
    style = SBS_VERT;
  } else {
    style = SBS_HORZ;
  }
  c = CreateWindow ("Scrollbar", "", WS_CHILD | style,
                    r.left, r.top, r.right - r.left,
                    r.bottom - r.top, wptr, 0,
                    Nlm_currentHInst, NULL);
  if (c != NULL) {
    SetProp (c, (LPSTR) "Nlm_VibrantProp", (Nlm_HandleTool) b);
  }
  Nlm_LoadBarData (b, c, 0, 0, 0, 0, 0, FALSE, slv, actn);
  if (lpfnNewBarProc == NULL) {
    lpfnNewBarProc = (WNDPROC) MakeProcInstance ((FARPROC) BarProc, Nlm_currentHInst);
  }
  if (lpfnOldBarProc == NULL) {
    lpfnOldBarProc = (WNDPROC) GetWindowLong (c, GWL_WNDPROC);
  } else if (lpfnOldBarProc != (WNDPROC) GetWindowLong (c, GWL_WNDPROC)) {
    Nlm_Message (MSG_ERROR, "BarProc subclass error");
  }
  SetWindowLong (c, GWL_WNDPROC, (LONG) lpfnNewBarProc);
#endif
#ifdef WIN_MOTIF
  n = 0;
  XtSetArg (wargs [n], XmNx, (Position) r.left); n++;
  XtSetArg (wargs [n], XmNy, (Position) r.top); n++;
  XtSetArg (wargs [n], XmNwidth, (Dimension) (r.right - r.left)); n++;
  XtSetArg (wargs [n], XmNheight, (Dimension) (r.bottom - r.top)); n++;
  XtSetArg (wargs [n], XmNmarginHeight, 0); n++;
  XtSetArg (wargs [n], XmNmarginWidth, 0); n++;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNminimum, 0); n++;
  XtSetArg (wargs [n], XmNmaximum, 20); n++;
  XtSetArg (wargs [n], XmNincrement, 1); n++;
  XtSetArg (wargs [n], XmNpageIncrement, 1); n++;
  XtSetArg (wargs [n], XmNvalue, 0); n++;
  XtSetArg (wargs [n], XmNsliderSize, 20); n++;
  if (vert) {
    XtSetArg (wargs [n], XmNorientation, XmVERTICAL); n++;
    XtSetArg (wargs [n], XmNprocessingDirection, XmMAX_ON_BOTTOM); n++;
  } else {
    XtSetArg (wargs [n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg (wargs [n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
  }
  c = XmCreateScrollBar (wptr, (String) "", wargs, n);
  XtAddCallback (c, XmNincrementCallback, Nlm_LineDnCallback, (XtPointer) b);
  XtAddCallback (c, XmNdecrementCallback, Nlm_LineUpCallback, (XtPointer) b);
  XtAddCallback (c, XmNpageIncrementCallback, Nlm_PageDnCallback, (XtPointer) b);
  XtAddCallback (c, XmNpageDecrementCallback, Nlm_PageUpCallback, (XtPointer) b);
  XtAddCallback (c, XmNvalueChangedCallback, Nlm_ThumbCallback, (XtPointer) b);
  XtAddCallback (c, XmNtoTopCallback, Nlm_BottomCallback, (XtPointer) b);
  XtAddCallback (c, XmNtoBottomCallback, Nlm_TopCallback, (XtPointer) b);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (b))) {
      XtRealizeWidget (c);
    }
  } else {
    XtRealizeWidget (c);
  }
  Nlm_LoadBarData (b, c, 0, 0, 0, 0, 0, FALSE, slv, actn);
#endif
}

static Nlm_BaR Nlm_CommonScrollBar (Nlm_GraphiC slave, Nlm_RectPtr r,
                                    Nlm_ScrlProc actn, Nlm_Boolean vert)

{
  Nlm_BaR     b;
  Nlm_WindoW  tempPort;

  b = NULL;
  if (slave != NULL && r != NULL) {
    tempPort = Nlm_SavePort (slave);
    b = (Nlm_BaR) Nlm_HandNew (sizeof (Nlm_BarRec));
    if (b != NULL) {
      Nlm_LoadGraphicData ((Nlm_GraphiC) b, NULL, (Nlm_GraphiC) slave,
                           NULL, NULL, barProcs, NULL, r, TRUE, FALSE);
      if (Nlm_nextIdNumber < 32767) {
        Nlm_nextIdNumber++;
      }
      Nlm_NewBar (b, slave, vert, actn);
    }
    Nlm_RestorePort (tempPort);
  }
  return b;
}

extern Nlm_BaR Nlm_HorizScrollBar (Nlm_GraphiC slave, Nlm_RectPtr r, Nlm_ScrlProc actn)

{
  return (Nlm_CommonScrollBar (slave, r, actn, FALSE));
}

extern Nlm_BaR Nlm_VertScrollBar (Nlm_GraphiC slave, Nlm_RectPtr r, Nlm_ScrlProc actn)

{
  return (Nlm_CommonScrollBar (slave, r, actn, TRUE));
}

extern Nlm_BaR Nlm_ScrollBar (Nlm_GrouP prnt, Nlm_Int2 width,
                              Nlm_Int2 height, Nlm_BarScrlProc actn)

{
  Nlm_BaR      b;
  Nlm_Int2     bhgt;
  Nlm_Int2     bwid;
  Nlm_PoinT    npt;
  Nlm_RecT     r;
  Nlm_WindoW   tempPort;
  Nlm_Boolean  vert;

  b = NULL;
  if (prnt != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) prnt);
    Nlm_GetNextPosition ((Nlm_GraphiC) prnt, &npt);
    if (width > 0) {
      vert = FALSE;
      bwid = width * Nlm_stdCharWidth;
      Nlm_LoadRect (&r, npt.x, npt.y, npt.x+bwid, npt.y+Nlm_hScrollBarHeight);
    } else {
      vert = TRUE;
      bhgt = height * Nlm_stdLineHeight;
      Nlm_LoadRect (&r, npt.x, npt.y, npt.x+Nlm_vScrollBarWidth, npt.y+bhgt);
    }
    b = (Nlm_BaR) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r, sizeof (Nlm_BarRec), linkedBarProcs);
    if (b != NULL) {
      Nlm_NewBar (b, (Nlm_GraphiC) prnt, vert, (Nlm_ScrlProc) actn);
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) b, &r, TRUE, FALSE);
      Nlm_DoShow ((Nlm_GraphiC) b, TRUE, FALSE);
    }
    Nlm_RestorePort (tempPort);
  }
  return b;
}

extern void Nlm_CorrectBarValue (Nlm_BaR b, Nlm_Int2 val)

{
  Nlm_BarData  bdata;
  Nlm_BarTool  c;
  Nlm_Int2     oldval;
  Nlm_WindoW   tempPort;
#ifdef WIN_MOTIF
  Nlm_Int2     page;
#endif

  if (b != NULL) {
    Nlm_GetBarData (b, &bdata);
    if (val < 0) {
      val = 0;
    }
    if (val > bdata.maximum) {
      val = bdata.maximum;
    }
    c = bdata.handle;
    oldval = bdata.value;
    bdata.value = val;
    bdata.delta = oldval - val;
    Nlm_SetBarData (b, &bdata);
    if (val != oldval && Nlm_GetEnabled ((Nlm_GraphiC) b)) {
      tempPort = Nlm_SavePortIfNeeded ((Nlm_GraphiC) b, TRUE);
#ifdef WIN_MAC
      SetCtlValue (c, val);
#endif
#ifdef WIN_MSWIN
      SetScrollPos (c, SB_CTL, val, TRUE);
#endif
#ifdef WIN_MOTIF
      page = (bdata.pageUp + bdata.pageDn) / 2;
      XmScrollBarSetValues (c, (int) val, 20, 1, (int) page, FALSE);
#endif
      Nlm_RestorePort (tempPort);
    }
  }
}

extern void Nlm_CorrectBarMax (Nlm_BaR b, Nlm_Int2 max)

{
  Nlm_BarData  bdata;
  Nlm_BarTool  c;
  Nlm_Int2     oldmax;
  Nlm_WindoW   tempPort;
#ifdef WIN_MOTIF
  Cardinal     n;
  Arg          wargs [10];
#endif

  if (b != NULL) {
    Nlm_GetBarData (b, &bdata);
    oldmax = bdata.maximum;
    bdata.maximum = max;
    Nlm_SetBarData (b, &bdata);
    if (max != oldmax && Nlm_GetEnabled ((Nlm_GraphiC) b)) {
      tempPort = Nlm_SavePortIfNeeded ((Nlm_GraphiC) b, TRUE);
      c = bdata.handle;
#ifdef WIN_MAC
      SetCtlMax (c, max);
#endif
#ifdef WIN_MSWIN
      SetScrollRange (c, SB_CTL, 0, max, TRUE);
#endif
#ifdef WIN_MOTIF
      n = 0;
      XtSetArg (wargs [n], XmNminimum, 0); n++;
      XtSetArg (wargs [n], XmNmaximum, (int) max + 20); n++;
      XtSetValues (c, wargs, n);
#endif
      Nlm_RestorePort (tempPort);
    }
  }
}

extern void Nlm_CorrectBarPage (Nlm_BaR b, Nlm_Int2 pgUp, Nlm_Int2 pgDn)

{
  Nlm_BarData  bdata;

  if (b != NULL) {
    Nlm_GetBarData (b, &bdata);
    bdata.pageUp = pgUp;
    bdata.pageDn = pgDn;
    Nlm_SetBarData (b, &bdata);
  }
}

extern void Nlm_SetBarAnomaly (Nlm_BaR b, Nlm_Boolean anomaly)

{
  Nlm_BarData  bdata;

  if (b != NULL) {
    Nlm_GetBarData (b, &bdata);
    bdata.anomaly = anomaly;
    Nlm_SetBarData (b, &bdata);
  }
}

extern void Nlm_FreeBars (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemFree (gphprcsptr);
}

extern void Nlm_InitBars (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemNew (sizeof (Nlm_GphPrcs) * 2);

  barProcs = &(gphprcsptr [0]);
#ifdef WIN_MAC
  barProcs->click = Nlm_ScrollClick;
  barProcs->draw = Nlm_DrawBar;
#endif
#ifdef WIN_MSWIN
  barProcs->command = Nlm_ScrollCommand;
#endif
#ifdef WIN_MOTIF
#endif
  barProcs->show = Nlm_ShowBar;
  barProcs->hide = Nlm_HideBar;
  barProcs->enable = Nlm_EnableBar;
  barProcs->disable = Nlm_DisableBar;
  barProcs->remove = Nlm_RemoveBar;
  barProcs->reset = Nlm_ResetBar;
  barProcs->setValue = Nlm_SetBarValue;
  barProcs->getValue = Nlm_GetBarValue;
  barProcs->setPosition = Nlm_SetBarPosition;
  barProcs->getPosition = Nlm_GetBarPosition;
  barProcs->setRange = Nlm_SetBarRange;

  linkedBarProcs = &(gphprcsptr [1]);
#ifdef WIN_MAC
  linkedBarProcs->click = Nlm_ScrollClick;
  linkedBarProcs->draw = Nlm_DrawBar;
#endif
#ifdef WIN_MSWIN
  linkedBarProcs->command = Nlm_ScrollCommand;
#endif
#ifdef WIN_MOTIF
#endif
  linkedBarProcs->show = Nlm_ShowBar;
  linkedBarProcs->hide = Nlm_HideBar;
  linkedBarProcs->enable = Nlm_EnableBar;
  linkedBarProcs->disable = Nlm_DisableBar;
  linkedBarProcs->remove = Nlm_RemoveLinkedInBar;
  linkedBarProcs->reset = Nlm_ResetBar;
  linkedBarProcs->setValue = Nlm_SetBarValue;
  linkedBarProcs->getValue = Nlm_GetBarValue;
  linkedBarProcs->setPosition = Nlm_SetBarPosition;
  linkedBarProcs->getPosition = Nlm_GetBarPosition;
  linkedBarProcs->setRange = Nlm_SetBarRange;
}
