/*   vibbutns.c
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
* File Name:  vibbutns.c
*
* Author:  Jonathan Kans
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.12 $
*
* File Description: 
*       Vibrant button functions
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
#define Nlm_ControlTool ControlHandle
#endif

#ifdef WIN_MSWIN
#define Nlm_ControlTool HWND
#endif

#ifdef WIN_MOTIF
#define Nlm_ControlTool Widget
#endif

typedef  struct  Nlm_buttondata {
  Nlm_ControlTool  handle;
  Nlm_Int2         border;
  Nlm_Int2         offset;
  Nlm_Boolean      defaultBtn;
} Nlm_ButtonData;

typedef  struct  Nlm_buttonrec {
  Nlm_GraphicRec  graphicR;
  Nlm_ButtonData  button;
} Nlm_ButtonRec, PNTR Nlm_BtnPtr;

static Nlm_GphPrcsPtr  gphprcsptr = NULL;

static Nlm_GphPrcsPtr  pushProcs;
static Nlm_GphPrcsPtr  defaultProcs;
static Nlm_GphPrcsPtr  checkProcs;
static Nlm_GphPrcsPtr  radioProcs;

static Nlm_ButtoN      recentButton = NULL;
static Nlm_ButtonData  recentButtonData;

#ifdef WIN_MSWIN
static WNDPROC         lpfnNewButtonProc = NULL;
static WNDPROC         lpfnOldButtonProc = NULL;
static Nlm_Boolean     handlechar;
#endif

static void Nlm_LoadButtonData (Nlm_ButtoN b, Nlm_ControlTool hdl,
                                Nlm_Int2 bdr, Nlm_Int2 ofs,
                                Nlm_Boolean dflt)

{
  Nlm_BtnPtr      bp;
  Nlm_ButtonData  PNTR ptr;

  if (b != NULL) {
    bp = (Nlm_BtnPtr) Nlm_HandLock (b);
    ptr = &(bp->button);
    ptr->handle = hdl;
    ptr->border = bdr;
    ptr->offset = ofs;
    ptr->defaultBtn = dflt;
    Nlm_HandUnlock (b);
    recentButton = NULL;
  }
}

static void Nlm_SetButtonData (Nlm_ButtoN b, Nlm_ButtonData * bdata)

{
  Nlm_BtnPtr  bp;

  if (b != NULL && bdata != NULL) {
    bp = (Nlm_BtnPtr) Nlm_HandLock (b);
    bp->button = *bdata;
    Nlm_HandUnlock (b);
    recentButton = b;
    recentButtonData = *bdata;
  }
}

static void Nlm_GetButtonData (Nlm_ButtoN b, Nlm_ButtonData * bdata)

{
  Nlm_BtnPtr  bp;

  if (b != NULL && bdata != NULL) {
    if (b == recentButton && NLM_RISKY) {
      *bdata = recentButtonData;
    } else {
      bp = (Nlm_BtnPtr) Nlm_HandLock (b);
      *bdata = bp->button;
      Nlm_HandUnlock (b);
      recentButton = b;
      recentButtonData = *bdata;
    }
  }
}

static Nlm_ControlTool Nlm_GetButtonHandle (Nlm_ButtoN b)

{
  Nlm_ButtonData  bdata;

  Nlm_GetButtonData (b, &bdata);
  return bdata.handle;
}

static Nlm_Int2 Nlm_GetButtonBorder (Nlm_ButtoN b)

{
  Nlm_ButtonData  bdata;

  Nlm_GetButtonData (b, &bdata);
  return bdata.border;
}

static Nlm_Int2 Nlm_GetButtonOffset (Nlm_ButtoN b)

{
  Nlm_ButtonData  bdata;

  Nlm_GetButtonData (b, &bdata);
  return bdata.offset;
}

static Nlm_Boolean Nlm_IsDefaultButton (Nlm_ButtoN b)

{
  Nlm_ButtonData  bdata;

  Nlm_GetButtonData (b, &bdata);
  return bdata.defaultBtn;
}

#ifdef WIN_MAC
static Nlm_Boolean Nlm_CommonPtInRect (Nlm_GraphiC b, Nlm_PoinT pt)

{
  Nlm_RecT  r;

  Nlm_GetRect (b, &r);
  return (Nlm_PtInRect (pt, &r));
}

static Nlm_Boolean Nlm_CommonButtonClick (Nlm_GraphiC b, Nlm_PoinT pt, Nlm_Int2 part)

{
  Nlm_ControlTool  c;
  Nlm_PointTool    ptool;

  c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
  Nlm_PoinTToPointTool (pt, &ptool);
  return (part == TrackControl (c, ptool, NULL));
}

static Nlm_Boolean Nlm_PushClick (Nlm_GraphiC b, Nlm_PoinT pt)

{
  Nlm_Boolean  rsult;

  rsult = FALSE;
  if (Nlm_CommonPtInRect (b, pt)) {
    if (Nlm_CommonButtonClick (b, pt, inButton)) {
      Nlm_DoAction (b);
    }
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_DefaultClick (Nlm_GraphiC b, Nlm_PoinT pt)

{
  Nlm_Boolean  rsult;

  rsult = FALSE;
  if (Nlm_CommonPtInRect (b, pt)) {
    if (Nlm_CommonButtonClick (b, pt, inButton)) {
      Nlm_DoAction (b);
    }
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_CheckClick (Nlm_GraphiC b, Nlm_PoinT pt)

{
  Nlm_Boolean  bool;
  Nlm_GraphiC  g;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  if (Nlm_CommonPtInRect (b, pt)) {
    if (Nlm_CommonButtonClick (b, pt, inCheckBox)) {
      bool = Nlm_DoGetStatus (b, 0);
      if (bool) {
        Nlm_DoSetStatus (b, 0, FALSE, FALSE);
      } else {
        Nlm_DoSetStatus (b, 0, TRUE, FALSE);
      }
      Nlm_DoAction (b);
      g = Nlm_GetParent (b);
      Nlm_DoAction (g);
    }
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_RadioClick (Nlm_GraphiC b, Nlm_PoinT pt)

{
  Nlm_Boolean  bool;
  Nlm_GraphiC  g;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  if (Nlm_CommonPtInRect (b, pt)) {
    if (Nlm_CommonButtonClick (b, pt, inCheckBox)) {
      bool = Nlm_DoGetStatus (b, 0);
      if (! bool) {
        g = Nlm_GetParent (b);
        Nlm_ClearItemsInGroup (g, b, FALSE);
        Nlm_DoSetStatus (b, 0, TRUE, FALSE);
        Nlm_DoAction (b);
        Nlm_DoAction (g);
      }
    }
    rsult = TRUE;
  }
  return rsult;
}
#endif

#ifdef WIN_MSWIN
static Nlm_Boolean Nlm_PushCommand (Nlm_GraphiC b)

{
  Nlm_DoAction (b);
  return TRUE;
}

static Nlm_Boolean Nlm_DefaultCommand (Nlm_GraphiC b)

{
  Nlm_DoAction (b);
  return TRUE;
}

static Nlm_Boolean Nlm_CheckCommand (Nlm_GraphiC b)

{
  Nlm_Boolean  bool;
  Nlm_GraphiC  g;

  bool = Nlm_DoGetStatus (b, 0);
  if (bool) {
    Nlm_DoSetStatus (b, 0, FALSE, FALSE);
  } else {
    Nlm_DoSetStatus (b, 0, TRUE, FALSE);
  }
  Nlm_DoAction (b);
  g = Nlm_GetParent (b);
  Nlm_DoAction (g);
  return TRUE;
}

static Nlm_Boolean Nlm_RadioCommand (Nlm_GraphiC b)

{
  Nlm_Boolean  bool;
  Nlm_GraphiC  g;

  bool = Nlm_DoGetStatus (b, 0);
  if (! bool) {
    g = Nlm_GetParent (b);
    Nlm_ClearItemsInGroup (g, b, FALSE);
    Nlm_DoSetStatus (b, 0, TRUE, FALSE);
    Nlm_DoAction (b);
    Nlm_DoAction (g);
  }
  return TRUE;
}
#endif

#ifdef WIN_MOTIF
static void Nlm_PushCallback (Nlm_GraphiC b)

{
  Nlm_DoAction (b);
}

static void Nlm_DefaultCallback (Nlm_GraphiC b)

{
  Nlm_DoAction (b);
}

static void Nlm_CheckCallback (Nlm_GraphiC b)

{
  Nlm_Boolean      bool;
  Nlm_ControlTool  c;
  Nlm_GraphiC      g;

  c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
  bool = Nlm_DoGetStatus (b, 0);
  if (! bool) {
    Nlm_DoSetStatus (b, 0, FALSE, FALSE);
  } else {
    Nlm_DoSetStatus (b, 0, TRUE, FALSE);
  }
  Nlm_DoAction (b);
  g = Nlm_GetParent (b);
  Nlm_DoAction (g);
}

static void Nlm_RadioCallback (Nlm_GraphiC b)

{
  Nlm_Boolean      bool;
  Nlm_ControlTool  c;
  Nlm_GraphiC      g;

  c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
  bool = Nlm_DoGetStatus (b, 0);
  if (bool) {
    g = Nlm_GetParent (b);
    Nlm_ClearItemsInGroup (g, b, FALSE);
    Nlm_DoSetStatus (b, 0, TRUE, FALSE);
    Nlm_DoAction (b);
    Nlm_DoAction (g);
  } else {
    Nlm_DoSetStatus (b, 0, TRUE, FALSE);
  }
}
#endif

#ifdef WIN_MAC
static Nlm_Boolean Nlm_DefaultKey (Nlm_GraphiC b, Nlm_Char ch)

{
  Nlm_ControlTool  c;
  Nlm_Int4         res;
  Nlm_Boolean      rsult;

  rsult = FALSE;
  if (ch == '\15' || ch == '\3') {
    c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
    HiliteControl (c, 1);
    Delay (8, &res);
    HiliteControl (c, 0);
    Nlm_DoSetStatus (b, 0, TRUE, FALSE);
    Nlm_DoAction (b);
    rsult = TRUE;
  }
  return rsult;
}

static void Nlm_DrawButton (Nlm_GraphiC b)

{
  Nlm_ControlTool  c;
  Nlm_RecT         r;

  if (Nlm_GetVisible (b) && Nlm_GetAllParentsVisible (b)) {
    Nlm_GetRect (b, &r);
    if (Nlm_RectInRgn (&r, Nlm_updateRgn)) {
      c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
      Draw1Control (c);
    }
  }
}

static void Nlm_DrawDefault (Nlm_GraphiC b)

{
  Nlm_ControlTool  c;
  Nlm_RecT         r;

  if (Nlm_GetVisible (b) && Nlm_GetAllParentsVisible (b)) {
    Nlm_GetRect (b, &r);
    Nlm_InsetRect (&r, -4, -4);
    if (Nlm_RectInRgn (&r, Nlm_updateRgn)) {
      PenSize (3, 3);
      Nlm_FrameRoundRect (&r, 16, 16);
      PenNormal ();
      c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
      Draw1Control (c);
    }
  }
}
#endif

static void Nlm_ShowButton (Nlm_GraphiC b, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_ControlTool  c;
  Nlm_WindoW       tempPort;

  if (setFlag) {
    Nlm_SetVisible (b, TRUE);
  }
  if (Nlm_GetVisible (b) && Nlm_AllParentsButWindowVisible (b)) {
    tempPort = Nlm_SavePortIfNeeded (b, savePort);
    c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
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

static void Nlm_HideButton (Nlm_GraphiC b, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_ControlTool  c;
  Nlm_WindoW       tempPort;
#ifdef WIN_MAC
  Nlm_Int2         border;
  Nlm_RecT         r;
#endif

  if (setFlag) {
    Nlm_SetVisible (b, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (b, savePort);
  c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
#ifdef WIN_MAC
  border = Nlm_GetButtonBorder ((Nlm_ButtoN) b);
  HideControl (c);
  if (Nlm_GetAllParentsVisible (b)) {
    Nlm_GetRect (b, &r);
    Nlm_InsetRect (&r, -1 - border, -1 - border);
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

static void Nlm_EnableButton (Nlm_GraphiC b, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_ControlTool  c;
  Nlm_WindoW       tempPort;

  if (setFlag) {
    Nlm_SetEnabled (b, TRUE);
  }
  if (Nlm_GetEnabled (b) && Nlm_GetAllParentsEnabled (b)) {
    tempPort = Nlm_SavePortIfNeeded (b, savePort);
    c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
#ifdef WIN_MAC
    HiliteControl (c, 0);
#endif
#ifdef WIN_MSWIN
    EnableWindow (c, TRUE);
#endif
#ifdef WIN_MOTIF
    XtVaSetValues (c, XmNsensitive, TRUE, NULL);
#endif
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_DisableButton (Nlm_GraphiC b, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_ControlTool  c;
  Nlm_WindoW       tempPort;

  if (setFlag) {
    Nlm_SetEnabled (b, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (b, savePort);
  c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
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

static void Nlm_RemoveButton (Nlm_GraphiC b, Nlm_Boolean savePort)

{
  Nlm_ControlTool  c;
  Nlm_WindoW       tempPort;

  tempPort = Nlm_SavePortIfNeeded (b, savePort);
  c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
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
  recentButton = NULL;
  Nlm_RestorePort (tempPort);
}

static void Nlm_RemoveDefaultButton (Nlm_GraphiC b, Nlm_Boolean savePort)

{
  Nlm_ControlTool  c;
  Nlm_WindoW       tempPort;
#ifdef WIN_MOTIF
  Nlm_WindoW       w;
  Nlm_WindowTool   wptr;
#endif

  tempPort = Nlm_SavePortIfNeeded (b, savePort);
  c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
#ifdef WIN_MAC
  DisposeControl (c);
#endif
#ifdef WIN_MSWIN
  RemoveProp (c, (LPSTR) "Nlm_VibrantProp");
  DestroyWindow (c);
#endif
#ifdef WIN_MOTIF
  w = Nlm_ParentWindow (b);
  wptr = Nlm_ParentWindowPtr (b);
  if (Nlm_GetWindowDefaultButton (w) == (Nlm_ButtoN) b) {
    Nlm_SetWindowDefaultButton (w, (Nlm_ButtoN) b);
    XtVaSetValues (wptr, XmNdefaultButton, NULL, NULL);
  }
  XtDestroyWidget (c);
#endif
  Nlm_RemoveLink (b);
  recentButton = NULL;
  Nlm_RestorePort (tempPort);
}

static void Nlm_SetButtonTitle (Nlm_GraphiC b, Nlm_Int2 item,
                                Nlm_CharPtr title, Nlm_Boolean savePort)

{
  Nlm_ControlTool  c;
  Nlm_Char         temp [256];
  Nlm_WindoW       tempPort;
#ifdef WIN_MOTIF
  XmString         label;
#endif

  tempPort = Nlm_SavePortIfNeeded (b, savePort);
  c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
  Nlm_StringNCpy (temp, title, sizeof (temp));
#ifdef WIN_MAC
  Nlm_CtoPstr (temp);
  SetCTitle (c, (StringPtr) temp);
#endif
#ifdef WIN_MSWIN
  SetWindowText (c, temp);
#endif
#ifdef WIN_MOTIF
  label = XmStringCreateSimple (temp);
  XtVaSetValues (c, XmNlabelString, label, NULL);
  XmStringFree (label);
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_GetButtonTitle (Nlm_GraphiC b, Nlm_Int2 item,
                                Nlm_CharPtr title, size_t maxsize)

{
  Nlm_ControlTool  c;
  Nlm_Char         temp [256];
#ifdef WIN_MOTIF
  XmString         label;
  char             *text;
#endif

  c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
#ifdef WIN_MAC
  GetCTitle (c, (StringPtr) temp);
  Nlm_PtoCstr (temp);
#endif
#ifdef WIN_MSWIN
  GetWindowText (c, temp, sizeof (temp));
#endif
#ifdef WIN_MOTIF
  XtVaGetValues (c, XmNlabelString, &label, NULL);
  temp [0] = '\0';
  if (XmStringGetLtoR (label, XmSTRING_DEFAULT_CHARSET, &text)) {
    Nlm_StringNCpy (temp, text, sizeof (temp));
    XtFree (text);
  }
#endif
  Nlm_StringNCpy (title, temp, maxsize);
}

static void Nlm_SetButtonStatus (Nlm_GraphiC b, Nlm_Int2 item,
                                 Nlm_Boolean set, Nlm_Boolean savePort)

{
  Nlm_ControlTool  c;
  Nlm_WindoW       tempPort;
  Nlm_Int2         val;

  tempPort = Nlm_SavePortIfNeeded (b, savePort);
  c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
  if (set) {
    val = 1;
  } else {
    val = 0;
  }
#ifdef WIN_MAC
  SetCtlValue (c, val);
#endif
#ifdef WIN_MSWIN
  Button_SetCheck (c, val);
#endif
#ifdef WIN_MOTIF
  XmToggleButtonSetState (c, (Boolean) set, FALSE);
#endif
  Nlm_RestorePort (tempPort);
}

static Nlm_Boolean Nlm_GetButtonStatus (Nlm_GraphiC b, Nlm_Int2 item)

{
  Nlm_ControlTool  c;

  c = Nlm_GetButtonHandle ((Nlm_ButtoN) b);
#ifdef WIN_MAC
  return (GetCtlValue (c) != 0);
#endif
#ifdef WIN_MSWIN
  return (Nlm_Boolean) (Button_GetCheck (c) != 0);
#endif
#ifdef WIN_MOTIF
  return (Nlm_Boolean) (XmToggleButtonGetState (c) != FALSE);
#endif
}

#ifdef WIN_MAC
static void Nlm_InvalButton (Nlm_GraphiC b, Nlm_Int2 border)

{
  Nlm_RecT  r;

  if (Nlm_GetVisible (b) && Nlm_GetAllParentsVisible (b)) {
    Nlm_GetRect (b, &r);
    Nlm_InsetRect (&r, -1 - border, -1 - border);
    Nlm_InvalRect (&r);
  }
}
#endif

static void Nlm_SetButtonPosition (Nlm_GraphiC b, Nlm_RectPtr r, Nlm_Boolean savePort)

{
  Nlm_ButtonData   bdata;
  Nlm_ControlTool  c;
  Nlm_RecT         oldRect;
  Nlm_WindoW       tempPort;
#ifdef WIN_MAC
  Nlm_Int2         border;
#endif
#ifdef WIN_MOTIF
  Nlm_Int2         offset;
#endif

  if (r != NULL) {
    Nlm_DoGetPosition (b, &oldRect);
    if (! Nlm_EqualRect (r, &oldRect)) {
      tempPort = Nlm_SavePortIfNeeded (b, savePort);
      Nlm_GetButtonData ((Nlm_ButtoN) b, &bdata);
      c = bdata.handle;
#ifdef WIN_MAC
      border = bdata.border;
      Nlm_InvalButton (b, border);
      MoveControl (c, r->left, r->top);
      SizeControl (c, r->right - r->left, r->bottom - r->top);
      Nlm_SetRect (b, r);
      Nlm_InvalButton (b, border);
#endif
#ifdef WIN_MSWIN
      MoveWindow (c, r->left, r->top, r->right - r->left, r->bottom - r->top, TRUE);
      Nlm_SetRect (b, r);
      UpdateWindow (c);
#endif
#ifdef WIN_MOTIF
      offset = bdata.offset;
      XtVaSetValues (c,
                     XmNx, (Position) r->left - offset,
                     XmNy, (Position) r->top - offset,
                     XmNwidth, (Dimension) (r->right - r->left),
                     XmNheight, (Dimension) (r->bottom - r->top), 
                     NULL);
      Nlm_SetRect (b, r);
#endif
      Nlm_RestorePort (tempPort);
    }
  }
}

static void Nlm_GetButtonPosition (Nlm_GraphiC b, Nlm_RectPtr r)

{
  if (r != NULL) {
    Nlm_GetRect (b, r);
  }
}

static Nlm_GraphiC Nlm_DefaultGainFocus (Nlm_GraphiC b, Nlm_Char ch, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  return NULL;
#endif
#ifdef WIN_MSWIN
  Nlm_GraphiC   rsult;

  rsult = NULL;
  if (ch == '\n' || ch == '\r') {
    if (Nlm_GetVisible (b) && Nlm_GetAllParentsVisible (b) &&
        Nlm_GetEnabled (b) && Nlm_GetAllParentsEnabled (b)) {
      Nlm_DoAction (b);
      rsult = b;
    }
  }
  return rsult;
#endif
#ifdef WIN_MOTIF
  return NULL;
#endif
}

#ifdef WIN_MSWIN
/* Message cracker functions */

static void MyCls_OnChar (HWND hwnd, UINT ch, int cRepeat)

{
  Nlm_ButtoN  b;

  b = (Nlm_ButtoN) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  handlechar = FALSE;
  if (ch == '\t') {
    Nlm_DoSendFocus ((Nlm_GraphiC) b, (Nlm_Char) ch);
  } else if (ch == '\n' || ch == '\r') {
    if (Nlm_IsDefaultButton (b)) {
      Nlm_DoGainFocus ((Nlm_GraphiC) b, (Nlm_Char) ch, FALSE);
    } else {
      Nlm_DoSendFocus ((Nlm_GraphiC) b, (Nlm_Char) ch);
    }
  } else {
    handlechar = TRUE;
  }
}

LRESULT CALLBACK EXPORT ButtonProc (HWND hwnd, UINT message,
                                    WPARAM wParam, LPARAM lParam)

{
  Nlm_ButtoN  b;
  LRESULT     rsult;
  HDC         tempHDC;
  HWND        tempHWnd;

  if (Nlm_VibrantDisabled ()) {
    return CallWindowProc (lpfnOldButtonProc, hwnd, message, wParam, lParam);
  }

  rsult = 0;
  tempHWnd = Nlm_currentHWnd;
  tempHDC = Nlm_currentHDC;
  b = (Nlm_ButtoN) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
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
        rsult = CallWindowProc (lpfnOldButtonProc, hwnd, message, wParam, lParam);
      }
      break;
    default:
      rsult = CallWindowProc (lpfnOldButtonProc, hwnd, message, wParam, lParam);
      break;
  }
  Nlm_currentHWnd = tempHWnd;
  Nlm_currentHDC = tempHDC;
  Nlm_currentWindowTool = tempHWnd;
  return rsult;
}
#endif

#ifdef WIN_MOTIF
static void Nlm_ButtonCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  Nlm_GraphiC  b;

  b = (Nlm_GraphiC) client_data;
  Nlm_DoCallback (b);
}

extern void Nlm_MapDefaultButton (Nlm_WindoW w, Nlm_ButtoN b)

{
  Nlm_ControlTool  c;
  Nlm_WindowTool   wptr;

  if (w != NULL && b != NULL) {
    wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) b);
    c = Nlm_GetButtonHandle (b);
    Nlm_SetWindowDefaultButton (w, b);
    XtVaSetValues (wptr, XmNdefaultButton, c, NULL);
  }
}
#endif

#define PUSH_STYLE 1
#define DEFAULT_STYLE 2
#define CHECK_STYLE 3
#define RADIO_STYLE 4

static void Nlm_NewButton (Nlm_ButtoN b, Nlm_CharPtr title,
                           Nlm_Int2 type, Nlm_BtnActnProc actn)

{
  Nlm_Int2         border;
  Nlm_ControlTool  c;
  Nlm_Boolean      dflt;
  Nlm_Int2         offset;
  Nlm_RecT         r;
  Nlm_Char         temp [256];
  Nlm_WindowTool   wptr;
#ifdef WIN_MAC
  Nlm_Int2         procID;
  Nlm_RectTool     rtool;
#endif
#ifdef WIN_MSWIN
  Nlm_Uint4        style;
#endif
#ifdef WIN_MOTIF
  String           call;
  XmString         label;
  Cardinal         n;
  Nlm_WindoW       w;
  Arg              wargs [15];
#endif

  Nlm_GetRect ((Nlm_GraphiC) b, &r);
  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) b);
  Nlm_StringNCpy (temp, title, sizeof (temp));
  c = NULL;
  border = 0;
  offset = 0;
  dflt = (Nlm_Boolean) (type == DEFAULT_STYLE);
#ifdef WIN_MAC
  Nlm_CtoPstr (temp);
  Nlm_RecTToRectTool (&r, &rtool);
  switch (type) {
    case PUSH_STYLE:
      procID = 0;
      break;
    case DEFAULT_STYLE:
      procID = 0;
      border = 5;
      break;
    case CHECK_STYLE:
      procID = 1;
      break;
    case RADIO_STYLE:
      procID = 2;
      break;
    default:
      procID = 0;
      break;
  }
  c = NewControl (wptr, &rtool, (StringPtr) temp, FALSE, 0, 0, 1, procID, 0);
  Nlm_LoadButtonData (b, c, border, offset, dflt);
#endif
#ifdef WIN_MSWIN
  switch (type) {
    case PUSH_STYLE:
      style = BS_PUSHBUTTON;
      break;
    case DEFAULT_STYLE:
      style = BS_DEFPUSHBUTTON;
      break;
    case CHECK_STYLE:
      style = BS_CHECKBOX;
      break;
    case RADIO_STYLE:
      style = BS_RADIOBUTTON;
      break;
    default:
      style = BS_PUSHBUTTON;
      break;
  }
  c = CreateWindow ("Button", temp, WS_CHILD | style,
                    r.left, r.top, r.right - r.left,
                    r.bottom - r.top, wptr, 0,
                    Nlm_currentHInst, NULL);
  if (c != NULL) {
    SetProp (c, (LPSTR) "Nlm_VibrantProp", (Nlm_HandleTool) b);
  }
  Nlm_LoadButtonData (b, c, border, offset, dflt);
  if (lpfnNewButtonProc == NULL) {
    lpfnNewButtonProc = (WNDPROC) MakeProcInstance ((FARPROC) ButtonProc, Nlm_currentHInst);
  }
  if (lpfnOldButtonProc == NULL) {
    lpfnOldButtonProc = (WNDPROC) GetWindowLong (c, GWL_WNDPROC);
  } else if (lpfnOldButtonProc != (WNDPROC) GetWindowLong (c, GWL_WNDPROC)) {
    Nlm_Message (MSG_ERROR, "ButtonProc subclass error");
  }
  SetWindowLong (c, GWL_WNDPROC, (LONG) lpfnNewButtonProc);
#endif
#ifdef WIN_MOTIF
  label = XmStringCreateSimple (temp);
  if (type == PUSH_STYLE || type == DEFAULT_STYLE) {
    offset = 4;
  }
  n = 0;
  XtSetArg (wargs [n], XmNlabelString, label); n++;
  XtSetArg (wargs [n], XmNx, (Position) r.left - offset); n++;
  XtSetArg (wargs [n], XmNy, (Position) r.top - offset); n++;
  XtSetArg (wargs [n], XmNwidth, (Dimension) (r.right - r.left)); n++;
  XtSetArg (wargs [n], XmNheight, (Dimension) (r.bottom - r.top)); n++;
  XtSetArg (wargs [n], XmNmarginHeight, 0); n++;
  XtSetArg (wargs [n], XmNmarginWidth, 0); n++;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNhighlightThickness, 0); n++;
  switch (type) {
    case PUSH_STYLE:
      XtSetArg (wargs [n], XmNrecomputeSize, FALSE); n++;
      c = XmCreatePushButton (wptr, (String) "", wargs, n);
      call = XmNactivateCallback;
      break;
    case DEFAULT_STYLE:
      XtSetArg (wargs [n], XmNrecomputeSize, FALSE); n++;
      c = XmCreatePushButton (wptr, (String) "", wargs, n);
      call = XmNactivateCallback;
      break;
    case CHECK_STYLE:
      XtSetArg (wargs [n], XmNindicatorType, XmN_OF_MANY); n++;
      XtSetArg (wargs [n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
      c = XmCreateToggleButton (wptr, (String) "", wargs, n);
      call = XmNvalueChangedCallback;
      break;
    case RADIO_STYLE:
      XtSetArg (wargs [n], XmNindicatorType, XmONE_OF_MANY); n++;
      XtSetArg (wargs [n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
      c = XmCreateToggleButton (wptr, (String) "", wargs, n);
      call = XmNvalueChangedCallback;
      break;
    default:
      break;
  }
  XmStringFree (label);
  XtAddCallback (c, call, Nlm_ButtonCallback, (XtPointer) b);
  Nlm_LoadButtonData (b, c, border, offset, dflt);
  w = Nlm_ParentWindow ((Nlm_GraphiC) b);
  if (NLM_QUIET) {
    if (type == DEFAULT_STYLE) {
      Nlm_SetWindowDefaultButton (w, b);
    }
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (b))) {
      XtRealizeWidget (c);
      if (type == DEFAULT_STYLE) {
        XtVaSetValues (wptr, XmNdefaultButton, c, NULL);
      }
    }
  } else {
    XtRealizeWidget (c);
    if (type == DEFAULT_STYLE) {
      Nlm_SetWindowDefaultButton (w, b);
      XtVaSetValues (wptr, XmNdefaultButton, c, NULL);
    }
  }
#endif
  Nlm_LoadAction ((Nlm_GraphiC) b, (Nlm_ActnProc) actn);
}

static Nlm_ButtoN Nlm_CommonButton (Nlm_GrouP prnt, Nlm_CharPtr title,
                                    Nlm_Int2 type, Nlm_BtnActnProc actn,
                                    Nlm_GphPrcsPtr classPtr)

{
  Nlm_ButtoN  b;
  Nlm_Int2    hgt;
  Nlm_PoinT   npt;
  Nlm_RecT    r;
  Nlm_WindoW  tempPort;
  Nlm_Int2    wid;

  b = NULL;
  if (prnt != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) prnt);
    Nlm_GetNextPosition ((Nlm_GraphiC) prnt, &npt);
    Nlm_SelectFont (Nlm_systemFont);
    wid = Nlm_StringWidth (title);
#ifdef WIN_MAC
  switch (type) {
    case PUSH_STYLE:
      hgt = Nlm_stdLineHeight+3;
      wid += 20;
      break;
    case DEFAULT_STYLE:
      hgt = Nlm_stdLineHeight+3;
      wid += 20;
      break;
    case CHECK_STYLE:
      hgt = Nlm_stdLineHeight;
      wid += 20;
      break;
    case RADIO_STYLE:
      hgt = Nlm_stdLineHeight;
      wid += 20;
      break;
    default:
      hgt = Nlm_stdLineHeight;
      wid += 20;
      break;
  }
#endif
#ifdef WIN_MSWIN
  switch (type) {
    case PUSH_STYLE:
      hgt = Nlm_stdLineHeight+8;
      wid += 20;
      break;
    case DEFAULT_STYLE:
      hgt = Nlm_stdLineHeight+8;
      wid += 20;
      break;
    case CHECK_STYLE:
      hgt = Nlm_stdLineHeight+3;
      wid += 20;
      break;
    case RADIO_STYLE:
      hgt = Nlm_stdLineHeight+3;
      wid += 20;
      break;
    default:
      hgt = Nlm_stdLineHeight;
      wid += 20;
      break;
  }
#endif
#ifdef WIN_MOTIF
  switch (type) {
    case PUSH_STYLE:
      hgt = Nlm_stdLineHeight+10;
      wid += 25;
      break;
    case DEFAULT_STYLE:
      hgt = Nlm_stdLineHeight+10;
      wid += 25;
      break;
    case CHECK_STYLE:
      hgt = Nlm_stdLineHeight+3;
      wid += 25;
      break;
    case RADIO_STYLE:
      hgt = Nlm_stdLineHeight+3;
      wid += 25;
      break;
    default:
      hgt = Nlm_stdLineHeight;
      wid += 25;
      break;
  }
#endif
    Nlm_LoadRect (&r, npt.x, npt.y, npt.x+wid, npt.y+hgt);
    b = (Nlm_ButtoN) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r, sizeof (Nlm_ButtonRec), classPtr);
    if (b != NULL) {
      Nlm_NewButton (b, title, type, actn);
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) b, &r, TRUE, FALSE);
      Nlm_DoShow ((Nlm_GraphiC) b, TRUE, FALSE);
    }
    Nlm_RestorePort (tempPort);
  }
  return b;
}

extern Nlm_ButtoN Nlm_PushButton (Nlm_GrouP prnt, Nlm_CharPtr title,
                                  Nlm_BtnActnProc actn)

{
  Nlm_ButtoN  b;

  b = Nlm_CommonButton (prnt, title, PUSH_STYLE, actn, pushProcs);
  return b;
}

extern Nlm_ButtoN Nlm_DefaultButton (Nlm_GrouP prnt, Nlm_CharPtr title,
                                     Nlm_BtnActnProc actn)

{
  Nlm_ButtoN  b;

  b = Nlm_CommonButton (prnt, title, DEFAULT_STYLE, actn, defaultProcs);
  return b;
}

extern Nlm_ButtoN Nlm_CheckBox (Nlm_GrouP prnt, Nlm_CharPtr title,
                                Nlm_BtnActnProc actn)

{
  Nlm_ButtoN  b;

  b = Nlm_CommonButton (prnt, title, CHECK_STYLE, actn, checkProcs);
  return b;
}

extern Nlm_ButtoN Nlm_RadioButton (Nlm_GrouP prnt, Nlm_CharPtr title)

{
  Nlm_ButtoN  b;

  b = Nlm_CommonButton (prnt, title, RADIO_STYLE, NULL, radioProcs);
  return b;
}

extern void Nlm_FreeButtons (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemFree (gphprcsptr);
}

extern void Nlm_InitButtons (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemNew (sizeof (Nlm_GphPrcs) * 4);

  pushProcs = &(gphprcsptr [0]);
#ifdef WIN_MAC
  pushProcs->click = Nlm_PushClick;
  pushProcs->draw = Nlm_DrawButton;
#endif
#ifdef WIN_MSWIN
  pushProcs->command = Nlm_PushCommand;
#endif
#ifdef WIN_MOTIF
  pushProcs->callback = Nlm_PushCallback;
#endif
  pushProcs->show = Nlm_ShowButton;
  pushProcs->hide = Nlm_HideButton;
  pushProcs->enable = Nlm_EnableButton;
  pushProcs->disable = Nlm_DisableButton;
  pushProcs->remove = Nlm_RemoveButton;
  pushProcs->setTitle = Nlm_SetButtonTitle;
  pushProcs->getTitle = Nlm_GetButtonTitle;
  pushProcs->setPosition = Nlm_SetButtonPosition;
  pushProcs->getPosition = Nlm_GetButtonPosition;

  defaultProcs = &(gphprcsptr [1]);
#ifdef WIN_MAC
  defaultProcs->click = Nlm_DefaultClick;
  defaultProcs->key = Nlm_DefaultKey;
  defaultProcs->draw = Nlm_DrawDefault;
#endif
#ifdef WIN_MSWIN
  defaultProcs->command = Nlm_DefaultCommand;
#endif
#ifdef WIN_MOTIF
  defaultProcs->callback = Nlm_DefaultCallback;
#endif
  defaultProcs->show = Nlm_ShowButton;
  defaultProcs->hide = Nlm_HideButton;
  defaultProcs->enable = Nlm_EnableButton;
  defaultProcs->disable = Nlm_DisableButton;
  defaultProcs->remove = Nlm_RemoveDefaultButton;
  defaultProcs->setTitle = Nlm_SetButtonTitle;
  defaultProcs->getTitle = Nlm_GetButtonTitle;
  defaultProcs->setPosition = Nlm_SetButtonPosition;
  defaultProcs->getPosition = Nlm_GetButtonPosition;
  defaultProcs->gainFocus = Nlm_DefaultGainFocus;

  checkProcs = &(gphprcsptr [2]);
#ifdef WIN_MAC
  checkProcs->click = Nlm_CheckClick;
  checkProcs->draw = Nlm_DrawButton;
#endif
#ifdef WIN_MSWIN
  checkProcs->command = Nlm_CheckCommand;
#endif
#ifdef WIN_MOTIF
  checkProcs->callback = Nlm_CheckCallback;
#endif
  checkProcs->show = Nlm_ShowButton;
  checkProcs->hide = Nlm_HideButton;
  checkProcs->enable = Nlm_EnableButton;
  checkProcs->disable = Nlm_DisableButton;
  checkProcs->remove = Nlm_RemoveButton;
  checkProcs->setTitle = Nlm_SetButtonTitle;
  checkProcs->getTitle = Nlm_GetButtonTitle;
  checkProcs->setStatus = Nlm_SetButtonStatus;
  checkProcs->getStatus = Nlm_GetButtonStatus;
  checkProcs->setPosition = Nlm_SetButtonPosition;
  checkProcs->getPosition = Nlm_GetButtonPosition;

  radioProcs = &(gphprcsptr [3]);
#ifdef WIN_MAC
  radioProcs->click = Nlm_RadioClick;
  radioProcs->draw = Nlm_DrawButton;
#endif
#ifdef WIN_MSWIN
  radioProcs->command = Nlm_RadioCommand;
#endif
#ifdef WIN_MOTIF
  radioProcs->callback = Nlm_RadioCallback;
#endif
  radioProcs->show = Nlm_ShowButton;
  radioProcs->hide = Nlm_HideButton;
  radioProcs->enable = Nlm_EnableButton;
  radioProcs->disable = Nlm_DisableButton;
  radioProcs->remove = Nlm_RemoveButton;
  radioProcs->setTitle = Nlm_SetButtonTitle;
  radioProcs->getTitle = Nlm_GetButtonTitle;
  radioProcs->setStatus = Nlm_SetButtonStatus;
  radioProcs->getStatus = Nlm_GetButtonStatus;
  radioProcs->setPosition = Nlm_SetButtonPosition;
  radioProcs->getPosition = Nlm_GetButtonPosition;
}
