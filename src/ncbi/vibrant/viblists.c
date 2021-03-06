/*   viblists.c
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
* File Name:  viblists.c
*
* Author:  Jonathan Kans
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.8 $
*
* File Description: 
*       Vibrant list functions
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
#define Nlm_ListTool ListHandle
#endif

#ifdef WIN_MSWIN
#define Nlm_ListTool HWND
#endif

#ifdef WIN_MOTIF
#define Nlm_ListTool Widget
#endif

typedef  struct  Nlm_listdata {
  Nlm_ListTool  handle;
  Nlm_BaR       scrollBar;
  Nlm_Int2      visLines;
  Nlm_Int2      numItems;
  Nlm_Int2      offset;
} Nlm_ListData;

typedef  struct  Nlm_listrec {
  Nlm_GraphicRec  graphicR;
  Nlm_ListData    list;
} Nlm_ListRec, PNTR Nlm_LstPtr;

static Nlm_GphPrcsPtr  gphprcsptr = NULL;

static Nlm_GphPrcsPtr  singleProcs;
static Nlm_GphPrcsPtr  multiProcs;

static Nlm_LisT        recentList = NULL;
static Nlm_ListData    recentListData;

#ifdef WIN_MSWIN
static WNDPROC         lpfnNewListProc = NULL;
static WNDPROC         lpfnOldListProc = NULL;
static Nlm_Boolean     handlechar;
#endif

static void Nlm_LoadListData (Nlm_LisT l, Nlm_ListTool hdl,
                              Nlm_BaR bar, Nlm_Int2 vis,
                              Nlm_Int2 nitm, Nlm_Int2 off)

{
  Nlm_LstPtr    lp;
  Nlm_ListData  PNTR ldptr;

  if (l != NULL) {
    lp = (Nlm_LstPtr) Nlm_HandLock (l);
    ldptr = &(lp->list);
    ldptr->handle = hdl;
    ldptr->scrollBar = bar;
    ldptr->visLines = vis;
    ldptr->numItems = nitm;
    ldptr->offset = off;
    Nlm_HandUnlock (l);
    recentList = NULL;
  }
}

static void Nlm_SetListData (Nlm_LisT l, Nlm_ListData * ldata)

{
  Nlm_LstPtr  lp;

  if (l != NULL && ldata != NULL) {
    lp = (Nlm_LstPtr) Nlm_HandLock (l);
    lp->list = *ldata;
    Nlm_HandUnlock (l);
    recentList = l;
    recentListData = *ldata;
  }
}

static void Nlm_GetListData (Nlm_LisT l, Nlm_ListData * ldata)

{
  Nlm_LstPtr  lp;

  if (l != NULL && ldata != NULL) {
    if (l == recentList && NLM_RISKY) {
      *ldata = recentListData;
    } else {
      lp = (Nlm_LstPtr) Nlm_HandLock (l);
      *ldata = lp->list;
      Nlm_HandUnlock (l);
      recentList = l;
      recentListData = *ldata;
    }
  }
}

static Nlm_ListTool Nlm_GetListHandle (Nlm_LisT l)

{
  Nlm_ListData  ldata;

  Nlm_GetListData (l, &ldata);
  return ldata.handle;
}

static Nlm_BaR Nlm_GetListScrollBar (Nlm_LisT l)

{
  Nlm_ListData  ldata;

  Nlm_GetListData (l, &ldata);
  return ldata.scrollBar;
}

static void Nlm_SetListVisLines (Nlm_LisT l, Nlm_Int2 lns)

{
  Nlm_ListData  ldata;

  Nlm_GetListData (l, &ldata);
  ldata.visLines = lns;
  Nlm_SetListData (l, &ldata);
}

static Nlm_Int2 Nlm_GetListVisLines (Nlm_LisT l)

{
  Nlm_ListData  ldata;

  Nlm_GetListData (l, &ldata);
  return ldata.visLines;
}

static void Nlm_SetListNumItems (Nlm_LisT l, Nlm_Int2 num)

{
  Nlm_ListData  ldata;
  
  Nlm_GetListData (l, &ldata);
  ldata.numItems = num;
  Nlm_SetListData (l, &ldata);
}

static Nlm_Int2 Nlm_GetListNumItems (Nlm_LisT l)

{
  Nlm_ListData  ldata;
  
  Nlm_GetListData (l, &ldata);
  return ldata.numItems;
}

static void Nlm_SetListScrollOffset (Nlm_LisT l, Nlm_Int2 off)

{
  Nlm_ListData  ldata;

  Nlm_GetListData (l, &ldata);
  ldata.offset = off;
  Nlm_SetListData (l, &ldata);
}

static Nlm_Int2 Nlm_GetListScrollOffset (Nlm_LisT l)

{
  Nlm_ListData  ldata;

  Nlm_GetListData (l, &ldata);
  return ldata.offset;
}

#ifdef WIN_MAC
static Nlm_Boolean Nlm_SingleListClick (Nlm_GraphiC l, Nlm_PoinT pt)

{
  Nlm_ListTool   c;
  Nlm_Int2       newval;
  Nlm_Int2       oldval;
  Nlm_PoinT      point;
  Nlm_PointTool  ptool;
  Nlm_RecT       r;
  Nlm_Boolean    rsult;
  Nlm_BaR        sb;

  rsult = FALSE;
  Nlm_GetRect (l, &r);
  r.right += Nlm_vScrollBarWidth;
  if (Nlm_PtInRect (pt, &r)) {
    r.right -= Nlm_vScrollBarWidth;
    Nlm_InsetRect (&r, 2, 2);
    sb = Nlm_GetListScrollBar ((Nlm_LisT) l);
    if (sb != NULL && Nlm_DoClick ((Nlm_GraphiC) sb, pt)) {
    } else if (Nlm_PtInRect (pt, &r)) {
      oldval = Nlm_DoGetValue (l);
      c = Nlm_GetListHandle ((Nlm_LisT) l);
      Nlm_PoinTToPointTool (pt, &ptool);
      Nlm_dblClick = (LClick (ptool, Nlm_currentEvent.modifiers, c) != 0);
      Nlm_MousePosition (&point);
      if (Nlm_PtInRect (point, &r)) {
        newval = Nlm_DoGetValue (l);
        if (oldval != newval || Nlm_dblClick) {
          Nlm_DoAction (l);
        }
      } else {
        newval = Nlm_DoGetValue (l);
        if (oldval != newval || Nlm_dblClick) {
          Nlm_DoAction (l);
        }
      }
      rsult = TRUE;
    }
  }
  return rsult;
}

static Nlm_Boolean Nlm_MultiListClick (Nlm_GraphiC l, Nlm_PoinT pt)

{
  Nlm_ListTool   c;
  Nlm_PoinT      point;
  Nlm_PointTool  ptool;
  Nlm_RecT       r;
  Nlm_Boolean    rsult;
  Nlm_BaR        sb;

  rsult = FALSE;
  Nlm_GetRect (l, &r);
  r.right += Nlm_vScrollBarWidth;
  if (Nlm_PtInRect (pt, &r)) {
    r.right -= Nlm_vScrollBarWidth;
    Nlm_InsetRect (&r, 2, 2);
    sb = Nlm_GetListScrollBar ((Nlm_LisT) l);
    if (sb != NULL && Nlm_DoClick ((Nlm_GraphiC) sb, pt)) {
    } else if (Nlm_PtInRect (pt, &r)) {
      c = Nlm_GetListHandle ((Nlm_LisT) l);
      Nlm_PoinTToPointTool (pt, &ptool);
      Nlm_dblClick = (LClick (ptool, Nlm_currentEvent.modifiers, c) != 0);
      Nlm_MousePosition (&point);
      if (Nlm_PtInRect (point, &r)) {
        Nlm_DoAction (l);
      } else {
        Nlm_DoAction (l);
      }
      rsult = TRUE;
    }
  }
  return rsult;
}
#endif

#ifdef WIN_MSWIN
static Nlm_Boolean Nlm_SingleListCommand (Nlm_GraphiC l)

{
  Nlm_dblClick = (Nlm_Boolean) (Nlm_currentCode == LBN_DBLCLK);
  if (Nlm_currentCode == LBN_SELCHANGE || Nlm_dblClick) {
    Nlm_DoAction (l);
  }
  return TRUE;
}

static Nlm_Boolean Nlm_MultiListCommand (Nlm_GraphiC l)

{
  Nlm_dblClick = (Nlm_Boolean) (Nlm_currentCode == LBN_SELCHANGE);
  if (Nlm_currentCode == LBN_SELCHANGE || Nlm_dblClick) {
    Nlm_DoAction (l);
  }
  return TRUE;
}
#endif

#ifdef WIN_MOTIF
static void Nlm_SingleListCallback (Nlm_GraphiC l)

{
  Nlm_DoAction (l);
}

static void Nlm_MultiListCallback (Nlm_GraphiC l)

{
  Nlm_DoAction (l);
}
#endif

#ifdef WIN_MAC
static void Nlm_DrawList (Nlm_GraphiC l)

{
  Nlm_ListTool  c;
  Nlm_RecT      r;
  Nlm_BaR       sb;

  if (Nlm_GetVisible (l) && Nlm_GetAllParentsVisible (l)) {
    Nlm_GetRect (l, &r);
    r.right += Nlm_vScrollBarWidth;
    if (Nlm_RectInRgn (&r, Nlm_updateRgn)) {
      Nlm_FrameRect (&r);
      sb = Nlm_GetListScrollBar ((Nlm_LisT) l);
      if (sb != NULL) {
        Nlm_DoDraw ((Nlm_GraphiC) sb);
      }
      r.right -= Nlm_vScrollBarWidth;
      c = Nlm_GetListHandle ((Nlm_LisT) l);
      if (c != NULL) {
        Nlm_SelectFont (Nlm_systemFont);
        LDoDraw (TRUE, c);
        LUpdate ((Nlm_RgnTool) Nlm_updateRgn, c);
      }
    }
  }
}
#endif

static void Nlm_ShowList (Nlm_GraphiC l, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_ListTool  c;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  Nlm_BaR       sb;
#endif

  if (setFlag) {
    Nlm_SetVisible (l, TRUE);
  }
  if (Nlm_GetVisible (l) && Nlm_AllParentsButWindowVisible (l)) {
    tempPort = Nlm_SavePortIfNeeded (l, savePort);
    c = Nlm_GetListHandle ((Nlm_LisT) l);
#ifdef WIN_MAC
    sb = Nlm_GetListScrollBar ((Nlm_LisT) l);
    if (sb != NULL) {
      Nlm_DoShow ((Nlm_GraphiC) sb, TRUE, FALSE);
    }
    Nlm_DoDraw (l);
#endif
#ifdef WIN_MSWIN
    ShowWindow (c, SW_SHOW);
    UpdateWindow (c);
#endif
#ifdef WIN_MOTIF
    XtManageChild (XtParent (c));
#endif
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_HideList (Nlm_GraphiC l, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_ListTool  c;
  Nlm_BaR       sb;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  Nlm_RecT      r;
#endif

  if (setFlag) {
    Nlm_SetVisible (l, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (l, savePort);
  sb = Nlm_GetListScrollBar ((Nlm_LisT) l);
  if (sb != NULL) {
    Nlm_DoHide ((Nlm_GraphiC) sb, TRUE, FALSE);
  }
  c = Nlm_GetListHandle ((Nlm_LisT) l);
#ifdef WIN_MAC
  if (Nlm_GetAllParentsVisible (l)) {
    Nlm_GetRect (l, &r);
    r.right += Nlm_vScrollBarWidth;
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
  XtUnmanageChild (XtParent (c));
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_EnableList (Nlm_GraphiC l, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_ListTool  c;
  Nlm_RecT      r;
  Nlm_BaR       sb;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  Nlm_Int2      newval;
  Nlm_Int2      oldval;
#endif

  if (setFlag) {
    Nlm_SetEnabled (l, TRUE);
  }
  if (Nlm_GetEnabled (l) && Nlm_GetAllParentsEnabled (l)) {
    tempPort = Nlm_SavePortIfNeeded (l, savePort);
    sb = Nlm_GetListScrollBar ((Nlm_LisT) l);
    c = Nlm_GetListHandle ((Nlm_LisT) l);
    if (sb != NULL) {
      Nlm_DoEnable ((Nlm_GraphiC) sb, TRUE, FALSE);
#ifdef WIN_MAC
      oldval = Nlm_GetListScrollOffset ((Nlm_LisT) l);
      newval = Nlm_DoGetValue ((Nlm_GraphiC) sb);
      if (oldval != newval) {
        Nlm_SetListScrollOffset ((Nlm_LisT) l, newval);
        LScroll (0, newval - oldval, c);
      }
#endif
      if (Nlm_GetVisible (l) && Nlm_GetAllParentsVisible (l)) {
        Nlm_GetRect (l, &r);
        Nlm_InsetRect (&r, 1, 1);
        Nlm_InvalRect (&r);
      }
    }
#ifdef WIN_MSWIN
    EnableWindow (c, TRUE);
    SetWindowRedraw (c, TRUE);
#endif
#ifdef WIN_MOTIF
    XtVaSetValues (c, XmNsensitive, TRUE, NULL);
#endif
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_DisableList (Nlm_GraphiC l, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_ListTool  c;
  Nlm_BaR       sb;
  Nlm_WindoW    tempPort;

  if (setFlag) {
    Nlm_SetEnabled (l, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (l, savePort);
  sb = Nlm_GetListScrollBar ((Nlm_LisT) l);
  if (sb != NULL) {
    Nlm_DoDisable ((Nlm_GraphiC) sb, TRUE, FALSE);
  }
  c = Nlm_GetListHandle ((Nlm_LisT) l);
#ifdef WIN_MSWIN
  EnableWindow (c, FALSE);
  SetWindowRedraw (c, FALSE);
#endif
#ifdef WIN_MOTIF
  XtVaSetValues (c, XmNsensitive, FALSE, NULL);
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_RemoveList (Nlm_GraphiC l, Nlm_Boolean savePort)

{
  Nlm_ListTool  c;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  Nlm_BaR       sb;
#endif

  tempPort = Nlm_SavePortIfNeeded (l, savePort);
  c = Nlm_GetListHandle ((Nlm_LisT) l);
#ifdef WIN_MAC
  if (c != NULL) {
    LDispose (c);
  }
  sb = Nlm_GetListScrollBar ((Nlm_LisT) l);
  if (sb != NULL) {
    Nlm_DoRemove ((Nlm_GraphiC) sb, FALSE);
  }
#endif
#ifdef WIN_MSWIN
  RemoveProp (c, (LPSTR) "Nlm_VibrantProp");
  DestroyWindow (c);
#endif
#ifdef WIN_MOTIF
  XtDestroyWidget (c);
#endif
  Nlm_RemoveLink (l);
  recentList = NULL;
  Nlm_RestorePort (tempPort);
}

static void Nlm_ResetList (Nlm_GraphiC l, Nlm_Boolean savePort)

{
  Nlm_ListTool  c;
  Nlm_ListData  ldata;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  Nlm_Int2      col;
  Nlm_BaR       sb;
#endif

  tempPort = Nlm_SavePortIfNeeded (l, savePort);
  Nlm_GetListData ((Nlm_LisT) l, &ldata);
  c = ldata.handle;
#ifdef WIN_MAC
  LDelColumn (0, 0, c);
  LDelRow (0, 0, c);
  col = LAddColumn (1, 0, c);
  sb = ldata.scrollBar;
  if (sb != NULL) {
    Nlm_DoReset ((Nlm_GraphiC) sb, FALSE);
  }
#endif
#ifdef WIN_MSWIN
  ListBox_ResetContent (c);
#endif
#ifdef WIN_MOTIF
  XmListSetPos (c, 1);
  XmListDeleteAllItems (c);
#endif
  ldata.numItems = 0;
  ldata.offset = 0;
  Nlm_SetListData ((Nlm_LisT) l, &ldata);
  Nlm_RestorePort (tempPort);
}

static Nlm_Int2 Nlm_CountListItems (Nlm_GraphiC l)

{
  return Nlm_GetListNumItems ((Nlm_LisT) l);
}

static void Nlm_GetListTitle (Nlm_GraphiC l, Nlm_Int2 item,
                              Nlm_CharPtr title, size_t maxsize)

{
  Nlm_ListTool  c;
  Nlm_Char      temp [256];
#ifdef WIN_MAC
  Cell          cell;
  Nlm_Int2      len;
#endif

  c = Nlm_GetListHandle ((Nlm_LisT) l);
  if (item > 0) {
#ifdef WIN_MAC
    cell.h = 0;
    cell.v = item - 1;
    len = sizeof (temp);
    LGetCell (temp, &len, cell, c);
    if (len < sizeof (temp)) {
      temp [len] = '\0';
    }
#endif
#ifdef WIN_MSWIN
    if (ListBox_GetTextLen (c, item - 1) < sizeof (temp)) {
      ListBox_GetText (c, item - 1, temp);
    } else {
      temp [0] = '\0';
    }
#endif
#ifdef WIN_MOTIF
#endif
  }
  Nlm_StringNCpy (title, temp, maxsize);
}

#ifdef WIN_MAC
static void Nlm_ClearCellsInGroup (Nlm_LisT l, Nlm_Int2 excpt)

{
  Nlm_ListTool  c;
  Cell          cell;
  Nlm_Int2      count;
  Nlm_Int2      i;

  c = Nlm_GetListHandle (l);
  count = Nlm_GetListNumItems (l);
  i = 1;
  cell.h = 0;
  while (i <= count) {
    if (i != excpt) {
      cell.v = i - 1;
      LSetSelect (FALSE, cell, c);
    }
    i++;
  }
}
#endif

static void Nlm_SetListValue (Nlm_GraphiC l, Nlm_Int2 value, Nlm_Boolean savePort)

{
  Nlm_ListTool  c;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  Cell          cell;
  Nlm_Int2      delta;
  Nlm_Int2      newval;
  Nlm_Int2      num;
  Nlm_Int2      oldval;
  Nlm_BaR       sb;
  Nlm_Int2      vis;
#endif

  tempPort = Nlm_SavePortIfNeeded (l, savePort);
  c = Nlm_GetListHandle ((Nlm_LisT) l);
#ifdef WIN_MAC
  if (value > 0) {
    Nlm_ClearCellsInGroup ((Nlm_LisT) l, value);
    cell.h = 0;
    cell.v = value - 1;
    LSetSelect (TRUE, cell, c);
    sb = Nlm_GetListScrollBar ((Nlm_LisT) l);
    if (sb != NULL) {
      oldval = Nlm_DoGetValue ((Nlm_GraphiC) sb);
      num = Nlm_GetListNumItems ((Nlm_LisT) l);
      vis = Nlm_GetListVisLines ((Nlm_LisT) l);
      value--;
      if (value < oldval) {
        newval = value;
      } else if (value < oldval + vis) {
        newval = oldval;
      } else if (value <= num - vis) {
        newval = value;
      } else {
        newval = num - vis;
      }
      delta = newval - oldval;
      if (delta != 0) {
        Nlm_DoSetValue ((Nlm_GraphiC) sb, newval, FALSE);
      }
    }
  } else {
    Nlm_ClearCellsInGroup ((Nlm_LisT) l, 0);
  }
#endif
#ifdef WIN_MSWIN
  if (value > 0) {
    ListBox_SetCurSel (c, value - 1);
  } else {
    ListBox_SetCurSel (c, -1);
  }
#endif
#ifdef WIN_MOTIF
  if (value > 0) {
    XmListSelectPos (c, (int) value, FALSE);
  } else {
    XmListDeselectAllItems (c);
  }
#endif
  Nlm_RestorePort (tempPort);
}

static Nlm_Int2 Nlm_GetListValue (Nlm_GraphiC l)

{
  Nlm_ListTool  c;
  Nlm_Int2      value;
#ifdef WIN_MAC
  Cell          cell;
  Nlm_Int2      count;
  Nlm_Boolean   found;
  Nlm_Int2      i;
#endif
#ifdef WIN_MOTIF
  int           count;
  int           *position;
#endif

  value = 0;
  c = Nlm_GetListHandle ((Nlm_LisT) l);
#ifdef WIN_MAC
  count = Nlm_GetListNumItems ((Nlm_LisT) l);
  i = 0;
  cell.h = 0;
  found = FALSE;
  while (i <= count && ! found) {
    i++;
    cell.v = i - 1;
    found = LGetSelect (FALSE, &cell, c);
  }
  if (found) {
    value = i;
  }
#endif
#ifdef WIN_MSWIN
  value = (Nlm_Int2) ListBox_GetCurSel (c) + 1;
#endif
#ifdef WIN_MOTIF
  if (XmListGetSelectedPos (c, &position, &count)) {
    if (position != NULL) {
      value = *position;
      XtFree ((char *) position);
    } else {
      value = 0;
    }
  } else {
    value = 0;
  }
#endif
  return value;
}

static void Nlm_SetListStatus (Nlm_GraphiC l, Nlm_Int2 item,
                               Nlm_Boolean value, Nlm_Boolean savePort)

{
  Nlm_ListTool  c;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  Cell          cell;
#endif

  if (item > 0) {
    tempPort = Nlm_SavePortIfNeeded (l, savePort);
    c = Nlm_GetListHandle ((Nlm_LisT) l);
#ifdef WIN_MAC
    cell.h = 0;
    cell.v = item - 1;
    LSetSelect (value, cell, c);
#endif
#ifdef WIN_MSWIN
    ListBox_SetSel (c, value, item);
#endif
#ifdef WIN_MOTIF
    XmListSelectPos (c, (int) item, value);
#endif
    Nlm_RestorePort (tempPort);
  }
}

static Nlm_Boolean Nlm_GetListStatus (Nlm_GraphiC l, Nlm_Int2 item)

{
  Nlm_ListTool  c;
  Nlm_Boolean   value;
#ifdef WIN_MAC
  Cell          cell;
  Nlm_Int2      count;
#endif
#ifdef WIN_MSWIN
  Nlm_Int2      count;
#endif
#ifdef WIN_MOTIF
  int           count;
  int           *position;
#endif

  value = FALSE;
  c = Nlm_GetListHandle ((Nlm_LisT) l);
  count = Nlm_GetListNumItems ((Nlm_LisT) l);
  if (item > 0 && item <= count) {
#ifdef WIN_MAC
    cell.h = 0;
    cell.v = item - 1;
    value = LGetSelect (FALSE, &cell, c);
#endif
#ifdef WIN_MSWIN
    value = (Nlm_Boolean) (ListBox_GetSel (c, item) != 0);
#endif
#ifdef WIN_MOTIF
    if (XmListGetSelectedPos (c, &position, &count)) {
      if (position != NULL) {
        count--;
        while (count >= 0 && (! value)) {
          if (position [count] == item) {
            value = TRUE;
          }
          count--;
        }
        XtFree ((char *) position);
      }
    }
#endif
  }
  return value;
}

static void Nlm_SetListOffset (Nlm_GraphiC l, Nlm_Int2 horiz,
                               Nlm_Int2 vert, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_BaR  sb;

  sb = Nlm_GetListScrollBar ((Nlm_LisT) l);
  if (sb != NULL) {
    Nlm_DoSetValue ((Nlm_GraphiC) sb, vert, savePort);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_ListTool  c;

  c = Nlm_GetListHandle ((Nlm_LisT) l);
  ListBox_SetTopIndex (c, vert);
#endif
#ifdef WIN_MOTIF
  Nlm_ListTool  c;

  c = Nlm_GetListHandle ((Nlm_LisT) l);
  XmListSetPos (c, vert);
#endif
}

static void Nlm_GetListOffset (Nlm_GraphiC l, Nlm_Int2Ptr horiz, Nlm_Int2Ptr vert)

{
#ifdef WIN_MAC
  Nlm_BaR   sb;
  Nlm_Int2  rsult;

  sb = Nlm_GetListScrollBar ((Nlm_LisT) l);
  rsult = 0;
  if (sb != NULL) {
    rsult = Nlm_DoGetValue ((Nlm_GraphiC) sb);
  }
  if (vert != NULL) {
    *vert = rsult;
  }
#endif
#ifdef WIN_MSWIN
  Nlm_ListTool  c;

  c = Nlm_GetListHandle ((Nlm_LisT) l);
  if (vert != NULL) {
    *vert = (Nlm_Int2) ListBox_GetTopIndex (c);
  }
#endif
#ifdef WIN_MOTIF
  Nlm_ListTool  c;
  int           top;

  c = Nlm_GetListHandle ((Nlm_LisT) l);
  if (vert != NULL) {
    XtVaGetValues (c, XmNtopItemPosition, &top, NULL);
    *vert = (Nlm_Int2) top;
  }
#endif
}

#ifdef WIN_MAC
static void Nlm_ScrollAction (Nlm_BaR sb, Nlm_GraphiC l,
                              Nlm_Int2 newval, Nlm_Int2 oldval)

{
  Nlm_Int2      delta;
  Nlm_ListTool  h;
  Nlm_ListData  ldata;
  Nlm_RecT      r;
  Nlm_Int2      vis;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePort (l);
  Nlm_GetListData ((Nlm_LisT) l, &ldata);
  vis = ldata.visLines;
  h = ldata.handle;
  ldata.offset = newval;
  Nlm_SetListData ((Nlm_LisT) l, &ldata);
  delta = oldval - newval;
  if (oldval != newval) {
    Nlm_SelectFont (Nlm_systemFont);
    LScroll (0, -delta, h);
  } else if (Nlm_GetVisible (l) && Nlm_GetAllParentsVisible (l)) {
    Nlm_GetRect (l, &r);
    Nlm_InsetRect (&r, 1, 1);
    Nlm_InvalRect (&r);
  }
  Nlm_RestorePort (tempPort);
}
#endif

#ifdef WIN_MAC
static void Nlm_InvalList (Nlm_GraphiC l)

{
  Nlm_RecT  r;

  if (Nlm_GetVisible (l) && Nlm_GetAllParentsVisible (l)) {
    Nlm_GetRect (l, &r);
    r.right += Nlm_vScrollBarWidth;
    Nlm_InsetRect (&r, -1, -1);
    Nlm_InvalRect (&r);
  }
}
#endif

static void Nlm_SetListPosition (Nlm_GraphiC l, Nlm_RectPtr r, Nlm_Boolean savePort)

{
  Nlm_ListTool  c;
  Nlm_RecT      lr;
  Nlm_RecT      oldRect;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  Nlm_PoinT     csize;
  ListPtr       lp;
  Nlm_BaR       sb;
  Nlm_Int2      wid;
#endif

  if (r != NULL) {
    Nlm_DoGetPosition (l, &oldRect);
    if (! Nlm_EqualRect (r, &oldRect)) {
      tempPort = Nlm_SavePortIfNeeded (l, savePort);
      c = Nlm_GetListHandle ((Nlm_LisT) l);
#ifdef WIN_MAC
      lr = *r;
      Nlm_InvalList (l);
      lr.right -= Nlm_vScrollBarWidth;
      Nlm_SetRect (l, &lr);
      Nlm_InsetRect (&lr, 2, 2);
      HLock ((Handle) c);
      lp = (ListPtr) *((Handle) c);
      Nlm_RecTToRectTool (&lr, &(lp->rView));
      Nlm_PointToolToPoinT (lp->cellSize, &csize);
      csize.x = lr.right - lr.left;
      Nlm_PoinTToPointTool (csize, &(lp->cellSize));
      HUnlock ((Handle) c);
      Nlm_InvalList (l);
      sb = Nlm_GetListScrollBar ((Nlm_LisT) l);
      if (sb != NULL) {
        Nlm_GetRect (l, &lr);
        lr.left = lr.right;
        lr.right += Nlm_vScrollBarWidth;
        Nlm_DoSetPosition ((Nlm_GraphiC) sb, &lr, FALSE);
      }
#endif
#ifdef WIN_MSWIN
      lr = *r;
      MoveWindow (c, lr.left, lr.top, lr.right - lr.left, lr.bottom - lr.top, TRUE);
      lr.right -= Nlm_vScrollBarWidth;
      Nlm_SetRect (l, &lr);
      UpdateWindow (c);
#endif
#ifdef WIN_MOTIF
      XtVaSetValues (c,
                     XmNx, (Position) r->left,
                     XmNy, (Position) r->top,
                     XmNwidth, (Dimension) (r->right - r->left),
                     XmNheight, (Dimension) (r->bottom - r->top), 
                     NULL);
      Nlm_SetRect (l, r);
#endif
      Nlm_RestorePort (tempPort);
    }
  }
}

static void Nlm_GetListPosition (Nlm_GraphiC l, Nlm_RectPtr r)

{
  Nlm_RecT  lr;

  if (r != NULL) {
    Nlm_GetRect (l, &lr);
    lr.right += Nlm_vScrollBarWidth;
    *r = lr;
  }
}

#ifdef WIN_MSWIN
/* Message cracker functions */

static void MyCls_OnChar (HWND hwnd, UINT ch, int cRepeat)

{
  Nlm_LisT  l;

  l = (Nlm_LisT) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  handlechar = FALSE;
  if (ch == '\t') {
    Nlm_DoSendFocus ((Nlm_GraphiC) l, (Nlm_Char) ch);
  } else if (ch == '\n' || ch == '\r') {
    Nlm_DoSendFocus ((Nlm_GraphiC) l, (Nlm_Char) ch);
  } else {
    handlechar = TRUE;
  }
}

LRESULT CALLBACK EXPORT ListProc (HWND hwnd, UINT message,
                                  WPARAM wParam, LPARAM lParam)

{
  Nlm_LisT  l;
  LRESULT   rsult;
  HDC       tempHDC;
  HWND      tempHWnd;

  if (Nlm_VibrantDisabled ()) {
    return CallWindowProc (lpfnOldListProc, hwnd, message, wParam, lParam);
  }

  rsult = 0;
  tempHWnd = Nlm_currentHWnd;
  tempHDC = Nlm_currentHDC;
  l = (Nlm_LisT) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  Nlm_theWindow = Nlm_GetParentWindow ((Nlm_GraphiC) l);
  Nlm_currentHWnd = GetParent (hwnd);
  Nlm_currentHDC = Nlm_ParentWindowPort ((Nlm_GraphiC) l);
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
        rsult = CallWindowProc (lpfnOldListProc, hwnd, message, wParam, lParam);
      }
      break;
    default:
      rsult = CallWindowProc (lpfnOldListProc, hwnd, message, wParam, lParam);
      break;
  }
  Nlm_currentHWnd = tempHWnd;
  Nlm_currentHDC = tempHDC;
  Nlm_currentWindowTool = tempHWnd;
  return rsult;
}
#endif

#ifdef WIN_MOTIF
static void Nlm_ListCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  Nlm_GraphiC  l;

  Nlm_dblClick = FALSE;
  l = (Nlm_GraphiC) client_data;
  Nlm_DoCallback (l);
}

static void Nlm_DoubleCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  Nlm_GraphiC  l;

  Nlm_dblClick = TRUE;
  l = (Nlm_GraphiC) client_data;
  Nlm_DoCallback (l);
}
#endif

static void Nlm_NewList (Nlm_LisT l, Nlm_Int2 width,
                         Nlm_Int2 height, Nlm_Int2 vis,
                         Nlm_Char flags, Nlm_Uint4 style,
                         Nlm_Boolean multi, Nlm_LstActnProc actn)

{
  Nlm_ListTool    c;
  Nlm_RecT        r;
  Nlm_BaR         sb;
  Nlm_WindowTool  wptr;
#ifdef WIN_MAC
  Nlm_PoinT       csize;
  Nlm_PointTool   cstool;
  Nlm_RecT        db;
  Nlm_RectTool    dbtool;
  ListPtr         lp;
  Nlm_PoinT       npt;
  Nlm_RectTool    rtool;
  Nlm_PoinT       zpt;
#endif
#ifdef WIN_MOTIF
  Cardinal        n;
  Arg             wargs [15];
#endif

  Nlm_GetRect ((Nlm_GraphiC) l, &r);
  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) l);
#ifdef WIN_MAC
  r.left = r.right;
  r.right += Nlm_vScrollBarWidth;
  sb = Nlm_VertScrollBar ((Nlm_GraphiC) l, &r, Nlm_ScrollAction);
  Nlm_DoSetRange ((Nlm_GraphiC) sb, vis - 1, vis - 1, 0, FALSE);
  Nlm_GetRect ((Nlm_GraphiC) l, &r);
  Nlm_InsetRect (&r, 2, 2);
  Nlm_LoadRect (&db, 0, 0, 1, 0);
  csize.x = width;
  csize.y = height;
  Nlm_PoinTToPointTool (csize, &cstool);
  Nlm_RecTToRectTool (&r, &rtool);
  Nlm_RecTToRectTool (&db, &dbtool);
  c = LNew (&rtool, &dbtool, cstool, 0, wptr, TRUE, FALSE, FALSE, FALSE);
  HLock ((Handle) c);
  lp = (ListPtr) *((Handle) c);
  lp->selFlags = flags;
  HUnlock ((Handle) c);
  LDoDraw (TRUE, c);
  Nlm_LoadListData (l, c, sb, vis, 0, 0);
#endif
#ifdef WIN_MSWIN
  sb = NULL;
  r.right += Nlm_vScrollBarWidth;
  c = CreateWindow ("Listbox", "", WS_CHILD | style,
                    r.left, r.top, r.right - r.left,
                    r.bottom - r.top, wptr, 0,
                    Nlm_currentHInst, NULL);
  if (c != NULL) {
    SetProp (c, (LPSTR) "Nlm_VibrantProp", (Nlm_HandleTool) l);
  }
  Nlm_LoadListData (l, c, sb, vis, 0, 0);
  if (lpfnNewListProc == NULL) {
    lpfnNewListProc = (WNDPROC) MakeProcInstance ((FARPROC) ListProc, Nlm_currentHInst);
  }
  if (lpfnOldListProc == NULL) {
    lpfnOldListProc = (WNDPROC) GetWindowLong (c, GWL_WNDPROC);
  } else if (lpfnOldListProc != (WNDPROC) GetWindowLong (c, GWL_WNDPROC)) {
    Nlm_Message (MSG_ERROR, "ListProc subclass error");
  }
  SetWindowLong (c, GWL_WNDPROC, (LONG) lpfnNewListProc);
#endif
#ifdef WIN_MOTIF
  sb = NULL;
  n = 0;
  XtSetArg (wargs [n], XmNx, (Position) r.left); n++;
  XtSetArg (wargs [n], XmNy, (Position) r.top); n++;
  XtSetArg (wargs [n], XmNwidth, (Dimension) (r.right - r.left)); n++;
  XtSetArg (wargs [n], XmNheight, (Dimension) (r.bottom - r.top)); n++;
  XtSetArg (wargs [n], XmNmarginHeight, 0); n++;
  XtSetArg (wargs [n], XmNmarginWidth, 0); n++;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNvisibleItemCount, (int) vis); n++;
  if (multi) {
    XtSetArg (wargs [n], XmNselectionPolicy, XmMULTIPLE_SELECT); n++;
  } else {
    XtSetArg (wargs [n], XmNselectionPolicy, XmBROWSE_SELECT); n++;
  }
  XtSetArg (wargs [n], XmNlistSizePolicy, XmCONSTANT); n++;
  XtSetArg (wargs [n], XmNhighlightThickness, 0); n++;
  c = XmCreateScrolledList (wptr, (String) "", wargs, n);
  if (multi) {
    XtAddCallback (c, XmNmultipleSelectionCallback, Nlm_ListCallback, (XtPointer) l);
  } else {
    XtAddCallback (c, XmNbrowseSelectionCallback, Nlm_ListCallback, (XtPointer) l);
  }
  XtAddCallback (c, XmNdefaultActionCallback, Nlm_DoubleCallback, (XtPointer) l);
  XtManageChild (c);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (l))) {
      XtRealizeWidget (c);
    }
  } else {
    XtRealizeWidget (c);
  }
  Nlm_LoadListData (l, c, sb, vis, 0, 0);
#endif
  Nlm_LoadAction ((Nlm_GraphiC) l, (Nlm_ActnProc) actn);
}

static Nlm_LisT Nlm_CommonList (Nlm_GrouP prnt, Nlm_Int2 width,
                                Nlm_Int2 height, Nlm_GphPrcs PNTR classPtr,
                                Nlm_Char flags, Nlm_Uint4 style,
                                Nlm_Boolean multi, Nlm_LstActnProc actn)

{
  Nlm_Int2    dwid;
  Nlm_LisT    l;
  Nlm_Int2    lhgt;
  Nlm_PoinT   npt;
  Nlm_RecT    r;
  Nlm_WindoW  tempPort;

  l = NULL;
  if (prnt != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) prnt);
    Nlm_GetNextPosition ((Nlm_GraphiC) prnt, &npt);
    Nlm_SelectFont (Nlm_systemFont);
    dwid = width * Nlm_stdCharWidth;
    lhgt = height * Nlm_stdLineHeight;
    Nlm_LoadRect (&r, npt.x, npt.y, npt.x+dwid+4, npt.y+lhgt);
#ifdef WIN_MAC
    r.bottom += 4;
#endif
#ifdef WIN_MSWIN
    r.bottom += Nlm_stdDescent - 1;
#endif
#ifdef WIN_MOTIF
    r.bottom += Nlm_stdDescent + 1;
#endif
    l = (Nlm_LisT) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r,
                                   sizeof (Nlm_ListRec), classPtr);
    if (l != NULL) {
      Nlm_NewList (l, dwid, Nlm_stdLineHeight, height, flags, style, multi, actn);
      r.right += Nlm_vScrollBarWidth;
#ifdef WIN_MSWIN
      r.bottom += 4;
#endif
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) l, &r, TRUE, FALSE);
      Nlm_DoShow ((Nlm_GraphiC) l, TRUE, FALSE);
    }
    Nlm_RestorePort (tempPort);
  }
  return l;
}

extern Nlm_LisT Nlm_SingleList (Nlm_GrouP prnt, Nlm_Int2 width,
                                Nlm_Int2 height, Nlm_LstActnProc actn)

{
  Nlm_LisT  l;

  l = NULL;
#ifdef WIN_MAC
  l = Nlm_CommonList (prnt, width, height, singleProcs, '\270', 0, FALSE, actn);
#endif
#ifdef WIN_MSWIN
  l = Nlm_CommonList (prnt, width, height, singleProcs, '\0',
                      LBS_NOTIFY | WS_VSCROLL | WS_BORDER |
                      LBS_NOINTEGRALHEIGHT, FALSE, actn);
#endif
#ifdef WIN_MOTIF
  l = Nlm_CommonList (prnt, width, height, singleProcs, '\0', 0, FALSE, actn);
#endif
  return l;
}

extern Nlm_LisT Nlm_MultiList (Nlm_GrouP prnt, Nlm_Int2 width,
                               Nlm_Int2 height, Nlm_LstActnProc actn)

{
  Nlm_LisT  l;

  l = NULL;
#ifdef WIN_MAC
  l = Nlm_CommonList (prnt, width, height, multiProcs, '\124', 0, TRUE, actn);
#endif
#ifdef WIN_MSWIN
  l = Nlm_CommonList (prnt, width, height, multiProcs, '\0',
                      LBS_NOTIFY | WS_VSCROLL | WS_BORDER |
                      LBS_NOINTEGRALHEIGHT | LBS_MULTIPLESEL, TRUE, actn);
#endif
#ifdef WIN_MOTIF
  l = Nlm_CommonList (prnt, width, height, multiProcs, '\0', 0, TRUE, actn);
#endif
  return l;
}

extern void Nlm_ListItem (Nlm_LisT l, Nlm_CharPtr title)

{
  Nlm_ListTool  c;
  Nlm_Int2      num;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  Cell          cell;
  Nlm_Int2      count;
  Nlm_Boolean   doUpdate;
  Nlm_Int2      len;
  Nlm_Boolean   parentWindowVisible;
  Nlm_RecT      r;
  Nlm_Int2      row;
  Nlm_BaR       sb;
  Nlm_Int2      vis;
  Nlm_Int2      visDelta;
#endif
#ifdef WIN_MOTIF
  XmString      string;
#endif

  if (l != NULL) {
    tempPort = Nlm_SavePortIfNeeded ((Nlm_GraphiC) l, TRUE);
    c = Nlm_GetListHandle (l);
#ifdef WIN_MAC
    doUpdate = Nlm_GetEnabled ((Nlm_GraphiC) l);
    parentWindowVisible = Nlm_GetAllParentsVisible ((Nlm_GraphiC) l);
    Nlm_SelectFont (Nlm_systemFont);
    LDoDraw (parentWindowVisible && doUpdate, c);
    count = Nlm_GetListNumItems (l);
    row = LAddRow (1, count, c);
    cell.h = 0;
    cell.v = row;
    num = Nlm_GetListNumItems (l);
    num++;
    Nlm_SetListNumItems (l, num);
    len = (Nlm_Int2) Nlm_StringLen (title);
    LSetCell (title, len, cell, c);
    LDoDraw (doUpdate, c);
    vis = Nlm_GetListVisLines (l);
    visDelta =  num - vis;
    if (visDelta > 0) {
      sb = Nlm_GetListScrollBar (l);
      if (sb != NULL) {
        Nlm_DoSetRange ((Nlm_GraphiC) sb, vis - 1, vis - 1, visDelta, FALSE);
      }
    }
#endif
#ifdef WIN_MSWIN
    ListBox_AddString (c, title);
    num = Nlm_GetListNumItems (l);
    num++;
    Nlm_SetListNumItems (l, num);
#endif
#ifdef WIN_MOTIF
    string = XmStringCreateSimple (title);
    XmListAddItemUnselected (c, string, 0);
    XmStringFree (string);
#endif
    Nlm_RestorePort (tempPort);
  }
}

extern void Nlm_FreeLists (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemFree (gphprcsptr);
}

extern void Nlm_InitLists (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemNew (sizeof (Nlm_GphPrcs) * 2);

  singleProcs = &(gphprcsptr [0]);
#ifdef WIN_MAC
  singleProcs->click = Nlm_SingleListClick;
  singleProcs->draw = Nlm_DrawList;
#endif
#ifdef WIN_MSWIN
  singleProcs->command = Nlm_SingleListCommand;
#endif
#ifdef WIN_MOTIF
  singleProcs->callback = Nlm_SingleListCallback;
#endif
  singleProcs->show = Nlm_ShowList;
  singleProcs->hide = Nlm_HideList;
  singleProcs->enable = Nlm_EnableList;
  singleProcs->disable = Nlm_DisableList;
  singleProcs->remove = Nlm_RemoveList;
  singleProcs->reset = Nlm_ResetList;
  singleProcs->countItems = Nlm_CountListItems;
  singleProcs->getTitle = Nlm_GetListTitle;
  singleProcs->setValue = Nlm_SetListValue;
  singleProcs->getValue = Nlm_GetListValue;
  singleProcs->setOffset = Nlm_SetListOffset;
  singleProcs->getOffset = Nlm_GetListOffset;
  singleProcs->setPosition = Nlm_SetListPosition;
  singleProcs->getPosition = Nlm_GetListPosition;

  multiProcs = &(gphprcsptr [1]);
#ifdef WIN_MAC
  multiProcs->click = Nlm_MultiListClick;
  multiProcs->draw = Nlm_DrawList;
#endif
#ifdef WIN_MSWIN
  multiProcs->command = Nlm_MultiListCommand;
#endif
#ifdef WIN_MOTIF
  multiProcs->callback = Nlm_MultiListCallback;
#endif
  multiProcs->show = Nlm_ShowList;
  multiProcs->hide = Nlm_HideList;
  multiProcs->enable = Nlm_EnableList;
  multiProcs->disable = Nlm_DisableList;
  multiProcs->remove = Nlm_RemoveList;
  multiProcs->reset = Nlm_ResetList;
  multiProcs->countItems = Nlm_CountListItems;
  multiProcs->getTitle = Nlm_GetListTitle;
  multiProcs->setStatus = Nlm_SetListStatus;
  multiProcs->getStatus = Nlm_GetListStatus;
  multiProcs->setOffset = Nlm_SetListOffset;
  multiProcs->getOffset = Nlm_GetListOffset;
  multiProcs->setPosition = Nlm_SetListPosition;
  multiProcs->getPosition = Nlm_GetListPosition;
}
