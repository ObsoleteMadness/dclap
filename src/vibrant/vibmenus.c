/*   vibmenus.c
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
* File Name:  vibmenus.c
*
* Author:  Jonathan Kans
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.17 $
*
* File Description: 
*       Vibrant menu functions
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
#define Nlm_MenuTool   MenuHandle
#define Nlm_PopupTool  Nlm_Handle
#define Nlm_ItemTool   Nlm_Handle
#endif

#ifdef WIN_MSWIN
#define Nlm_MenuTool   HMENU
#define Nlm_PopupTool  HWND
#define Nlm_ItemTool   Nlm_Handle
#endif

#ifdef WIN_MOTIF
#define Nlm_MenuTool   Widget
#define Nlm_PopupTool  Widget
#define Nlm_ItemTool   Widget
#endif

typedef  struct  Nlm_menudata {
  Nlm_MenuTool   handle;
  Nlm_PrompT     prompt;
  Nlm_Int2       menuTag;
  Nlm_PopupTool  popup;
} Nlm_MenuData;

typedef  struct  Nlm_menurec {
  Nlm_GraphicRec  graphicR;
  Nlm_MenuData    menu;
} Nlm_MenuRec, PNTR Nlm_MenPtr;

typedef  struct  Nlm_itemdata {
  Nlm_MenU      subMenu;
  Nlm_Int2      firstItem;
  Nlm_ItemTool  handle;
} Nlm_ItemData;

typedef  struct  Nlm_itemrec {
  Nlm_GraphicRec  graphicR;
  Nlm_ItemData    item;
} Nlm_ItemRec, PNTR Nlm_ItmPtr;

typedef  struct  Nlm_choicedata {
  Nlm_Int2     numItems;
  Nlm_Boolean  isAPopupList;
  Nlm_Int2     popupValue;
  Nlm_ItemTool PNTR handles;
} Nlm_ChoiceData;

typedef  struct  Nlm_choicerec {
  Nlm_ItemRec     itemR;
  Nlm_ChoiceData  choice;
} Nlm_ChoiceRec, PNTR Nlm_ChsPtr;

typedef  struct  Nlm_menubardata {
  Nlm_MenuTool  handle;
} Nlm_MenuBarData;

typedef  struct  Nlm_menubarrec {
  Nlm_BoxRec       boxR;
  Nlm_MenuBarData  menubar;
} Nlm_MenuBarRec, PNTR Nlm_MbrPtr;

static Nlm_GphPrcsPtr   gphprcsptr = NULL;

#ifdef WIN_MAC
static Nlm_GphPrcsPtr   desktopMenuBarProcs;
static Nlm_GphPrcsPtr   desktopPulldownProcs;
#endif
static Nlm_GphPrcsPtr   windowMenuBarProcs;
static Nlm_GphPrcsPtr   windowPulldownProcs;
static Nlm_GphPrcsPtr   popupProcs;
static Nlm_GphPrcsPtr   subMenuProcs;
static Nlm_GphPrcsPtr   subItemProcs;
static Nlm_GphPrcsPtr   commandItemProcs;
static Nlm_GphPrcsPtr   statusItemProcs;
static Nlm_GphPrcsPtr   choiceGroupProcs;
static Nlm_GphPrcsPtr   choiceItemProcs;
static Nlm_GphPrcsPtr   macPopListProcs;
static Nlm_GphPrcsPtr   msPopListProcs;
static Nlm_GphPrcsPtr   popUpChoiceProcs;
static Nlm_GphPrcsPtr   daProcs;

static Nlm_MenU         recentMenu = NULL;
static Nlm_MenuData     recentMenuData;

static Nlm_IteM         recentItem = NULL;
static Nlm_ItemData     recentItemData;

static Nlm_ChoicE       recentChoice = NULL;
static Nlm_ChoiceData   recentChoiceData;

static Nlm_MenuBaR      recentMenuBar = NULL;
static Nlm_MenuBarData  recentMenuBarData;

static Nlm_Int2         nextMenuNum = 1;

#ifdef WIN_MAC
static Nlm_Int2         currentItemNum = 0;
static Nlm_Int2         currentMenuNum = 0;
static Nlm_Int2         currentMenuTag = 0;
#endif

#ifdef WIN_MSWIN
static Nlm_Int2         currentItemID = 0;
static WNDPROC          lpfnNewPopupProc = NULL;
static WNDPROC          lpfnOldPopupProc = NULL;
static Nlm_Boolean      handlechar;

#if  defined(DCLAP) && !defined(WIN16)
#define maxAccel		120
static	ACCEL			aAccel;
static  ACCEL			gAccel[maxAccel];
HACCEL					Nlm_hAccel = NULL; /* need also in vibwndws */
static	Nlm_Int2		nAccel = 0;
#endif
#endif

#ifdef WIN_MOTIF
static Widget           choiceWidget = NULL;
#endif

static void Nlm_LoadMenuData (Nlm_MenU m, Nlm_MenuTool hdl,
                              Nlm_PrompT ppt, Nlm_Int2 tag,
                              Nlm_PopupTool pup)

{
  Nlm_MenuData  PNTR mdptr;
  Nlm_MenPtr    mp;

  if (m != NULL) {
    mp = (Nlm_MenPtr) Nlm_HandLock (m);
    mdptr = &(mp->menu);
    mdptr->handle = hdl;
    mdptr->prompt = ppt;
    mdptr->menuTag = tag;
    mdptr->popup = pup;
    Nlm_HandUnlock (m);
    recentMenu = NULL;
  }
}

static void Nlm_SetMenuData (Nlm_MenU m, Nlm_MenuData * mdata)

{
  Nlm_MenPtr  mp;

  if (m != NULL && mdata != NULL) {
    mp = (Nlm_MenPtr) Nlm_HandLock (m);
    mp->menu = *mdata;
    Nlm_HandUnlock (m);
    recentMenu = m;
    recentMenuData = *mdata;
  }
}

static void Nlm_GetMenuData (Nlm_MenU m, Nlm_MenuData * mdata)

{
  Nlm_MenPtr  mp;

  if (m != NULL && mdata != NULL) {
    if (m == recentMenu && NLM_RISKY) {
      *mdata = recentMenuData;
    } else {
      mp = (Nlm_MenPtr) Nlm_HandLock (m);
      *mdata = mp->menu;
      Nlm_HandUnlock (m);
      recentMenu = m;
      recentMenuData = *mdata;
    }
  }
}

static void Nlm_LoadItemData (Nlm_IteM i, Nlm_MenU sub,
                              Nlm_Int2 frst, Nlm_ItemTool hdl)

{
  Nlm_ItemData  PNTR idptr;
  Nlm_ItmPtr   ip;

  if (i != NULL) {
    ip = (Nlm_ItmPtr) Nlm_HandLock (i);
    idptr = &(ip->item);
    idptr->subMenu = sub;
    idptr->firstItem = frst;
    idptr->handle = hdl;
    Nlm_HandUnlock (i);
    recentItem = NULL;
  }
}

static void Nlm_SetItemData (Nlm_IteM i, Nlm_ItemData * idata)

{
  Nlm_ItmPtr  ip;

  if (i != NULL && idata != NULL) {
    ip = (Nlm_ItmPtr) Nlm_HandLock (i);
    ip->item = *idata;
    Nlm_HandUnlock (i);
    recentItem = i;
    recentItemData = *idata;
  }
}

static void Nlm_GetItemData (Nlm_IteM i, Nlm_ItemData * idata)

{
  Nlm_ItmPtr  ip;

  if (i != NULL && idata != NULL) {
    if (i == recentItem && NLM_RISKY) {
      *idata = recentItemData;
    } else {
      ip = (Nlm_ItmPtr) Nlm_HandLock (i);
      *idata = ip->item;
      Nlm_HandUnlock (i);
      recentItem = i;
      recentItemData = *idata;
    }
  }
}

static void Nlm_LoadChoiceData (Nlm_ChoicE c, Nlm_Int2 num,
                                Nlm_Boolean plist, Nlm_Int2 pval,
                                Nlm_ItemTool PNTR hdls)

{
  Nlm_ChoiceData  PNTR cdptr;
  Nlm_ChsPtr      cp;

  if (c != NULL) {
    cp = (Nlm_ChsPtr) Nlm_HandLock (c);
    cdptr = &(cp->choice);
    cdptr->numItems = num;
    cdptr->isAPopupList = plist;
    cdptr->popupValue = pval;
    cdptr->handles = hdls;
    Nlm_HandUnlock (c);
    recentChoice = NULL;
  }
}

static void Nlm_SetChoiceData (Nlm_ChoicE c, Nlm_ChoiceData * cdata)

{
  Nlm_ChsPtr  cp;

  if (c != NULL && cdata != NULL) {
    cp = (Nlm_ChsPtr) Nlm_HandLock (c);
    cp->choice = *cdata;
    Nlm_HandUnlock (c);
    recentChoice = c;
    recentChoiceData = *cdata;
  }
}

static void Nlm_GetChoiceData (Nlm_ChoicE c, Nlm_ChoiceData * cdata)

{
  Nlm_ChsPtr  cp;

  if (c != NULL && cdata != NULL) {
    if (c == recentChoice && NLM_RISKY) {
      *cdata = recentChoiceData;
    } else {
      cp = (Nlm_ChsPtr) Nlm_HandLock (c);
      *cdata = cp->choice;
      Nlm_HandUnlock (c);
      recentChoice = c;
      recentChoiceData = *cdata;
    }
  }
}

static void Nlm_LoadMenuBarData (Nlm_MenuBaR mb, Nlm_MenuTool hdl)

{
  Nlm_MenuBarData  PNTR mbptr;
  Nlm_MbrPtr       mbp;

  if (mb != NULL) {
    mbp = (Nlm_MbrPtr) Nlm_HandLock (mb);
    mbptr = &(mbp->menubar);
    mbptr->handle = hdl;
    Nlm_HandUnlock (mb);
    recentMenuBar = NULL;
  }
}

static void Nlm_SetMenuBarData (Nlm_MenuBaR mb, Nlm_MenuBarData * mbdata)

{
  Nlm_MbrPtr  mbp;

  if (mb != NULL && mbdata != NULL) {
    mbp = (Nlm_MbrPtr) Nlm_HandLock (mb);
    mbp->menubar = *mbdata;
    Nlm_HandUnlock (mb);
    recentMenuBar = mb;
    recentMenuBarData = *mbdata;
  }
}

static void Nlm_GetMenuBarData (Nlm_MenuBaR mb, Nlm_MenuBarData * mbdata)

{
  Nlm_MbrPtr  mbp;

  if (mb != NULL && mbdata != NULL) {
    if (mb == recentMenuBar && NLM_RISKY) {
      *mbdata = recentMenuBarData;
    } else {
      mbp = (Nlm_MbrPtr) Nlm_HandLock (mb);
      *mbdata = mbp->menubar;
      Nlm_HandUnlock (mb);
      recentMenuBar = mb;
      recentMenuBarData = *mbdata;
    }
  }
}

static Nlm_MenuTool Nlm_GetMenuHandle (Nlm_MenU m)

{
  Nlm_MenuData  mdata;

  Nlm_GetMenuData (m, &mdata);
  return mdata.handle;
}

static Nlm_PrompT Nlm_GetMenuPrompt (Nlm_MenU m)

{
  Nlm_MenuData  mdata;

  Nlm_GetMenuData (m, &mdata);
  return mdata.prompt;
}

static Nlm_Int2 Nlm_GetMenuTag (Nlm_MenU m)

{
  Nlm_MenuData  mdata;

  Nlm_GetMenuData (m, &mdata);
  return mdata.menuTag;
}

static void Nlm_SetMenuPopup (Nlm_MenU m, Nlm_PopupTool pop)

{
  Nlm_MenuData  mdata;

  Nlm_GetMenuData (m, &mdata);
  mdata.popup = pop;
  Nlm_SetMenuData (m, &mdata);
}

static Nlm_PopupTool Nlm_GetMenuPopup (Nlm_MenU m)

{
  Nlm_MenuData  mdata;

  Nlm_GetMenuData (m, &mdata);
  return mdata.popup;
}

static void Nlm_SetSubMenu (Nlm_IteM i, Nlm_MenU sub)

{
  Nlm_ItemData  idata;

  Nlm_GetItemData (i, &idata);
  idata.subMenu = sub;
  Nlm_SetItemData (i, &idata);
}

static Nlm_MenU Nlm_GetSubMenu (Nlm_IteM i)

{
  Nlm_ItemData  idata;

  Nlm_GetItemData (i, &idata);
  return idata.subMenu;
}

static Nlm_Int2 Nlm_GetFirstItem (Nlm_IteM i)

{
  Nlm_ItemData  idata;

  Nlm_GetItemData (i, &idata);
  return idata.firstItem;
}

static Nlm_ItemTool Nlm_GetItemHandle (Nlm_IteM i)

{
  Nlm_ItemData  idata;

  Nlm_GetItemData (i, &idata);
  return idata.handle;
}

static void Nlm_SetNumItems (Nlm_ChoicE c, Nlm_Int2 num)

{
  Nlm_ChoiceData  cdata;

  Nlm_GetChoiceData (c, &cdata);
  cdata.numItems = num;
  Nlm_SetChoiceData (c, &cdata);
}

static Nlm_Int2 Nlm_GetNumItems (Nlm_ChoicE c)

{
  Nlm_ChoiceData  cdata;

  Nlm_GetChoiceData (c, &cdata);
  return cdata.numItems;
}

static Nlm_Boolean Nlm_IsItAPopupList (Nlm_ChoicE c)

{
  Nlm_ChoiceData  cdata;

  Nlm_GetChoiceData (c, &cdata);
  return cdata.isAPopupList;
}

static void Nlm_SetPopupValue (Nlm_ChoicE c, Nlm_Int2 pval)

{
  Nlm_ChoiceData  cdata;

  Nlm_GetChoiceData (c, &cdata);
  cdata.popupValue = pval;
  Nlm_SetChoiceData (c, &cdata);
}

static Nlm_Int2 Nlm_GetPopupValue (Nlm_ChoicE c)

{
  Nlm_ChoiceData  cdata;

  Nlm_GetChoiceData (c, &cdata);
  return cdata.popupValue;
}

static void Nlm_SetChoiceHandles (Nlm_ChoicE c, Nlm_ItemTool PNTR hdls)

{
  Nlm_ChoiceData  cdata;

  Nlm_GetChoiceData (c, &cdata);
  cdata.handles = hdls;
  Nlm_SetChoiceData (c, &cdata);
}

static Nlm_ItemTool PNTR Nlm_GetChoiceHandles (Nlm_ChoicE c)

{
  Nlm_ChoiceData  cdata;

  Nlm_GetChoiceData (c, &cdata);
  return cdata.handles;
}

static Nlm_MenuTool Nlm_GetMenuBarHandle (Nlm_MenuBaR mb)

{
  Nlm_MenuBarData  mbdata;

  Nlm_GetMenuBarData (mb, &mbdata);
  return mbdata.handle;
}

static Nlm_Int2 Nlm_ItemToID (Nlm_MenU m, Nlm_Int2 index)

{
  Nlm_MenuTool  h;
  Nlm_Int2      rsult;

  rsult = 0;
#ifdef WIN_MSWIN
  if (m != NULL && index > 0) {
    h = Nlm_GetMenuHandle (m);
    rsult = GetMenuItemID (h, index - 1);
  }
#endif
  return rsult;
}

static Nlm_Boolean Nlm_IsMenuItemChecked (Nlm_MenU m, Nlm_ItemTool itool, Nlm_Int2 item)

{
  Nlm_MenuTool  h;
  Nlm_Int2      id;
#ifdef WIN_MAC
  Nlm_Char      mark;
  Nlm_Int2      temp;
#endif

  h = Nlm_GetMenuHandle (m);
#ifdef WIN_MAC
  GetItemMark (h, item, &temp);
  mark = (Nlm_Char) temp;
  return (mark != '\0');
#endif
#ifdef WIN_MSWIN
  id = GetMenuItemID (h, item - 1);
  return (Nlm_Boolean) ((LOBYTE (GetMenuState (h, id, MF_BYCOMMAND)) & MF_CHECKED) != 0);
#endif
#ifdef WIN_MOTIF
  return (Nlm_Boolean) (XmToggleButtonGetState (itool) != FALSE);
#endif
}

static void Nlm_CheckMenuItem (Nlm_MenU m, Nlm_ItemTool itool,
                               Nlm_Int2 item, Nlm_Boolean check)

{
  Nlm_MenuTool  h;
  Nlm_Int2      id;
#ifdef WIN_MAC
  Nlm_Char      ch;
#endif

  h = Nlm_GetMenuHandle (m);
#ifdef WIN_MAC
  if (check) {
    ch = '\22';
  }
  else {
    ch = '\0';
  }
  SetItemMark (h, item, ch);
#endif
#ifdef WIN_MSWIN
  id = GetMenuItemID (h, item - 1);
  if (check) {
    CheckMenuItem (h, id, MF_CHECKED);
  } else {
    CheckMenuItem (h, id, MF_UNCHECKED);
  }
#endif
#ifdef WIN_MOTIF
  XmToggleButtonSetState (itool, (Boolean) check, FALSE);
#endif
}

#ifdef WIN_MAC
static void Nlm_DeskAccProc (Nlm_ChoicE c)

{
  Nlm_MenuTool  h;
  Nlm_MenU      m;
  Nlm_Char      temp [256];

  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) c);
  h = Nlm_GetMenuHandle (m);
  GetItem (h, currentItemNum, (StringPtr) temp);
  OpenDeskAcc ((StringPtr) temp);
}
#endif

static void Nlm_ActivateInnerMenus (Nlm_MenU m)

{
  Nlm_IteM      i;
  Nlm_MenU      s;
#ifdef WIN_MAC
  Nlm_MenuTool  h;
#endif

  i = (Nlm_IteM) Nlm_GetChild ((Nlm_GraphiC) m);
  while (i != NULL) {
    s = Nlm_GetSubMenu (i);
    if (s != NULL) {
      if (Nlm_GetEnabled ((Nlm_GraphiC) s)) {
        Nlm_DoEnable ((Nlm_GraphiC) i, FALSE, FALSE);
#ifdef WIN_MAC
        h = Nlm_GetMenuHandle (s);
        InsertMenu (h, -1);
#endif
        Nlm_ActivateInnerMenus (s);
      }
    }
    i = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
  }
}

static void Nlm_DeactivateInnerMenus (Nlm_MenU m)

{
  Nlm_IteM  i;
  Nlm_MenU  s;
#ifdef WIN_MAC
  Nlm_Int2  tag;
#endif

  i = (Nlm_IteM) Nlm_GetChild ((Nlm_GraphiC) m);
  while (i != NULL) {
    s = Nlm_GetSubMenu (i);
    if (s != NULL) {
      Nlm_DoDisable ((Nlm_GraphiC) i, FALSE, FALSE);
#ifdef WIN_MAC
      tag = Nlm_GetMenuTag (s);
      DeleteMenu (tag);
#endif
      Nlm_DeactivateInnerMenus (s);
    }
    i = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
  }
}

#ifdef WIN_MAC
static Nlm_Boolean Nlm_DesktopMenuBarClick (Nlm_GraphiC mb, Nlm_PoinT pt)

{
  Nlm_Int4        choice;
  Nlm_PoinT       global;
  Nlm_MenU        m;
  Nlm_MenU        n;
  Nlm_Boolean     notInGraphic;
  Nlm_PointTool   ptool;
  Nlm_Boolean     rsult;
  Nlm_Int2        windowLoc;
  Nlm_WindowTool  wptr;

  rsult = FALSE;
  currentMenuNum = 0;
  currentItemNum = 0;
  global = pt;
  Nlm_LocalToGlobal (&global);
  Nlm_PoinTToPointTool (global, &ptool);
  windowLoc = FindWindow (ptool, &wptr);
  if (windowLoc == inMenuBar) {
    choice = MenuSelect (ptool);
    currentMenuNum = HiWord (choice);
    currentItemNum = LoWord (choice);
    m = (Nlm_MenU) Nlm_GetChild (mb);
    notInGraphic = TRUE;
    while (m != NULL && notInGraphic) {
      n = (Nlm_MenU) Nlm_GetNext ((Nlm_GraphiC) m);
      currentMenuTag = Nlm_GetMenuTag (m);
      if (Nlm_GetEnabled ((Nlm_GraphiC) m) && Nlm_GetVisible ((Nlm_GraphiC) m) &&
          Nlm_DoClick ((Nlm_GraphiC) m, pt)) {
        notInGraphic = FALSE;
      }
      m = n;
    }
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_WindowMenuBarClick (Nlm_GraphiC mb, Nlm_PoinT pt)

{
  Nlm_MenU     m;
  Nlm_MenU     n;
  Nlm_Boolean  notInside;

  m = (Nlm_MenU) Nlm_GetChild (mb);
  notInside = TRUE;
  while (m != NULL && notInside) {
    n = (Nlm_MenU) Nlm_GetNext ((Nlm_GraphiC) m);
    currentMenuTag = Nlm_GetMenuTag (m);
    if (Nlm_GetVisible ((Nlm_GraphiC) m) && Nlm_DoClick ((Nlm_GraphiC) m, pt)) {
      notInside = FALSE;
    }
    m = n;
  }
  return (! notInside);
}

static Nlm_Boolean Nlm_DesktopMenuClick (Nlm_GraphiC m, Nlm_PoinT pt)

{
  Nlm_IteM     i;
  Nlm_Boolean  notInMenu;
  Nlm_IteM     n;
  Nlm_Boolean  rsult;
  Nlm_Int2     tag;

  rsult = FALSE;
  tag = Nlm_GetMenuTag ((Nlm_MenU) m);
  if (currentMenuNum > 0 && currentMenuNum == tag) {
    i = (Nlm_IteM) Nlm_GetChild (m);
    notInMenu = TRUE;
    while (i != NULL && notInMenu) {
      n = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
      if (Nlm_GetEnabled ((Nlm_GraphiC) i) && Nlm_GetVisible ((Nlm_GraphiC) i) &&
          Nlm_DoClick ((Nlm_GraphiC) i, pt)) {
        notInMenu = FALSE;
      }
      i = n;
    }
    HiliteMenu (0);
    currentMenuNum = 0;
    rsult = TRUE;
  }
  return rsult;
}

static void Nlm_PopupAPopupList (Nlm_GraphiC m, Nlm_PoinT pt,
                                 Nlm_RectPtr r, Nlm_PoinT pop,
                                 Nlm_Int2 item)

{
  Nlm_Int4      choice;
  Nlm_MenuTool  h;
  Nlm_IteM      i;
  Nlm_IteM      n;
  Nlm_WindoW    newwindow;
  Nlm_Boolean   notInMenu;
  Nlm_WindoW    oldwindow;

  oldwindow = Nlm_CurrentWindow ();
  newwindow = Nlm_GetParentWindow (m);
  Nlm_InvertRect (r);
  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
  choice = PopUpMenuSelect (h, pop.y, pop.x, item);
  currentMenuNum = HiWord (choice);
  currentItemNum = LoWord (choice);
  currentMenuTag = Nlm_GetMenuTag ((Nlm_MenU) m);
  i = (Nlm_IteM) Nlm_GetChild (m);
  notInMenu = TRUE;
  while (i != NULL && notInMenu) {
    n = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
    if (Nlm_GetEnabled ((Nlm_GraphiC) i) && Nlm_GetVisible ((Nlm_GraphiC) i) &&
        Nlm_DoClick ((Nlm_GraphiC) i, pt)) {
      notInMenu = FALSE;
    }
    i = n;
  }
  currentMenuNum = 0;
  Nlm_UseWindow (newwindow);
  Nlm_EraseRect (r);
  Nlm_InvalRect (r);
  Nlm_Update ();
  Nlm_UseWindow (oldwindow);
}

static Nlm_Boolean Nlm_PopupClick (Nlm_GraphiC m, Nlm_PoinT pt)

{
  Nlm_MenuTool  h;
  Nlm_PrompT    p;
  Nlm_PoinT     pop;
  Nlm_RecT      r;
  Nlm_Boolean   rsult;
  Nlm_Int2      tag;

  rsult = FALSE;
  p = Nlm_GetMenuPrompt ((Nlm_MenU) m);
  Nlm_GetRect ((Nlm_GraphiC) p, &r);
  if (Nlm_PtInRect (pt, &r)) {
    h = Nlm_GetMenuHandle ((Nlm_MenU) m);
    InsertMenu (h, -1);
    currentMenuNum = 0;
    currentItemNum = 0;
    pop.x = r.left;
    pop.y = r.bottom + 1;
    Nlm_LocalToGlobal (&pop);
    Nlm_PopupAPopupList (m, pt, &r, pop, 0);
    tag = Nlm_GetMenuTag ((Nlm_MenU) m);
    DeleteMenu (tag);
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_PulldownClick (Nlm_GraphiC m, Nlm_PoinT pt)

{
  Nlm_MenuTool  h;
  Nlm_PoinT     pop;
  Nlm_RecT      r;
  Nlm_Boolean   rsult;
  Nlm_Int2      tag;

  rsult = FALSE;
  Nlm_GetRect (m, &r);
  Nlm_InsetRect (&r, 0, 2);
  if (Nlm_PtInRect (pt, &r)) {
    h = Nlm_GetMenuHandle ((Nlm_MenU) m);
    InsertMenu (h, -1);
    currentMenuNum = 0;
    currentItemNum = 0;
    Nlm_GetRect (m, &r);
    Nlm_InsetRect (&r, -6, 0);
    r.top++;
    pop.x = r.left + 1;
    pop.y = r.bottom;
    Nlm_LocalToGlobal (&pop);
    Nlm_PopupAPopupList (m, pt, &r, pop, 0);
    tag = Nlm_GetMenuTag ((Nlm_MenU) m);
    DeleteMenu (tag);
    rsult = TRUE;
  }
  return rsult;
}


#ifdef DCLAP
extern
#else
static
#endif 
Nlm_Boolean Nlm_MacPopListClick (Nlm_GraphiC m, Nlm_PoinT pt)

{
  Nlm_ChoicE    c;
  Nlm_MenuTool  h;
  Nlm_Int2      item;
  Nlm_PoinT     pop;
  Nlm_RecT      r;
  Nlm_Boolean   rsult;
  Nlm_Int2      tag;

  rsult = FALSE;
  Nlm_GetRect (m, &r);
  Nlm_InsetRect (&r, 2, 3);
  if (Nlm_PtInRect (pt, &r)) {
    h = Nlm_GetMenuHandle ((Nlm_MenU) m);
    InsertMenu (h, -1);
    currentMenuNum = 0;
    currentItemNum = 0;
    item = 0;
    c = (Nlm_ChoicE) Nlm_GetChild (m);
    if (c != NULL) {
      item = Nlm_DoGetValue ((Nlm_GraphiC) c);
    }
    Nlm_GetRect (m, &r);
    pop.x = r.left + 1;
    pop.y = r.top + 1;
    Nlm_UpsetRect (&r, 1, 1, 2, 2);
    Nlm_LocalToGlobal (&pop);
    Nlm_PopupAPopupList (m, pt, &r, pop, item);
    tag = Nlm_GetMenuTag ((Nlm_MenU) m);
    DeleteMenu (tag);
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_MSPopListClick (Nlm_GraphiC m, Nlm_PoinT pt)

{
  Nlm_MenuTool  h;
  Nlm_PoinT     pop;
  Nlm_RecT      r;
  Nlm_Boolean   rsult;
  Nlm_Int2      tag;

  rsult = FALSE;
  Nlm_GetRect (m, &r);
  Nlm_InsetRect (&r, 2, 2);
  if (Nlm_PtInRect (pt, &r)) {
    h = Nlm_GetMenuHandle ((Nlm_MenU) m);
    InsertMenu (h, -1);
    currentMenuNum = 0;
    currentItemNum = 0;
    Nlm_GetRect (m, &r);
    pop.x = r.left + 1;
    pop.y = r.bottom - 1;
    Nlm_UpsetRect (&r, 1, 1, 2, 2);
    Nlm_LocalToGlobal (&pop);
    Nlm_PopupAPopupList (m, pt, &r, pop, 0);
    tag = Nlm_GetMenuTag ((Nlm_MenU) m);
    DeleteMenu (tag);
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_SubItemClick (Nlm_GraphiC i, Nlm_PoinT pt)

{
  Nlm_IteM     c;
  Nlm_IteM     n;
  Nlm_Boolean  notInMenu;
  Nlm_Int2     oldMenuTag;
  Nlm_Boolean  rsult;
  Nlm_MenU     s;

  rsult = FALSE;
  s = Nlm_GetSubMenu ((Nlm_IteM) i);
  if (s != NULL) {
    oldMenuTag = currentMenuTag;
    currentMenuTag = Nlm_GetMenuTag (s);
    c = (Nlm_IteM) Nlm_GetChild ((Nlm_GraphiC) s);
    notInMenu = TRUE;
    while (c != NULL && notInMenu) {
      n = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) c);
      if (Nlm_GetEnabled ((Nlm_GraphiC) c) && Nlm_GetVisible ((Nlm_GraphiC) c) &&
          Nlm_DoClick ((Nlm_GraphiC) c, pt)) {
        notInMenu = FALSE;
      }
      c = n;
    }
    currentMenuTag = oldMenuTag;
    rsult = (! notInMenu); 
  }
  return rsult;
}

static Nlm_Boolean Nlm_CommItemClick (Nlm_GraphiC i, Nlm_PoinT pt)

{
  Nlm_Int2     index;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  index = Nlm_GetFirstItem ((Nlm_IteM) i);
  if (currentItemNum == index && currentMenuTag == currentMenuNum) {
    Nlm_DoAction (i);
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_StatItemClick (Nlm_GraphiC i, Nlm_PoinT pt)

{
  Nlm_Int2     index;
  Nlm_Boolean  rsult;
  Nlm_Boolean  val;

  rsult = FALSE;
  index = Nlm_GetFirstItem ((Nlm_IteM) i);
  if (currentItemNum == index && currentMenuTag == currentMenuNum) {
    val = Nlm_DoGetStatus (i, 0);
    Nlm_DoSetStatus (i, 0, (! val), FALSE);
    Nlm_DoAction (i);
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_ChoiceGroupClick (Nlm_GraphiC c, Nlm_PoinT pt)

{
  Nlm_Int2     first;
  Nlm_Int2     num;
  Nlm_Boolean  rsult;
  Nlm_Int2     val;

  rsult = FALSE;
  first = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  val = Nlm_DoGetValue (c);
  if (currentItemNum >= first && currentItemNum < first + num &&
      currentMenuTag == currentMenuNum) {
    if (val != currentItemNum - first + 1) {
      Nlm_DoSetValue (c, currentItemNum - first + 1, FALSE);
      Nlm_DoAction (c);
    }
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_PopupChoiceGroupClick (Nlm_GraphiC c, Nlm_PoinT pt)

{
  return Nlm_ChoiceGroupClick (c, pt);
}

static Nlm_Boolean Nlm_DAClick (Nlm_GraphiC c, Nlm_PoinT pt)

{
  Nlm_Int2     first;
  Nlm_Int2     num;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  first = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  if (currentItemNum >= first && currentItemNum < first + num &&
      currentMenuTag == currentMenuNum) {
    Nlm_DeskAccProc ((Nlm_ChoicE) c);
    rsult = TRUE;
  }
  return rsult;
}
#endif

#ifdef WIN_MSWIN
static Nlm_Boolean Nlm_WindowMenuBarCommand (Nlm_GraphiC mb)

{
  Nlm_MenU     m;
  Nlm_MenU     n;
  Nlm_Boolean  notInside;

  notInside = TRUE;
  currentItemID = Nlm_currentId;
  m = (Nlm_MenU) Nlm_GetChild (mb);
  while (m != NULL && notInside) {
    n = (Nlm_MenU) Nlm_GetNext ((Nlm_GraphiC) m);
    if (Nlm_GetVisible ((Nlm_GraphiC) m) && Nlm_DoCommand ((Nlm_GraphiC) m)) {
      notInside = FALSE;
    }
    m = n;
  }
  return (Nlm_Boolean) (! notInside);
}

static Nlm_Boolean Nlm_PulldownCommand (Nlm_GraphiC m)

{
  Nlm_IteM     i;
  Nlm_IteM     n;
  Nlm_Boolean  notInside;


  notInside = TRUE;
  i = (Nlm_IteM) Nlm_GetChild (m);
  while (i != NULL && notInside) {
    n = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
    if (Nlm_GetEnabled ((Nlm_GraphiC) i) && Nlm_GetVisible ((Nlm_GraphiC) i) &&
        Nlm_DoCommand ((Nlm_GraphiC) i)) {
      notInside = FALSE;
    }
    i = n;
  }
  return (Nlm_Boolean) (! notInside);
}

static Nlm_Boolean Nlm_MSPopListCommand (Nlm_GraphiC m)

{
  Nlm_ChoicE   c;
  Nlm_Boolean  rsult;

  if (Nlm_currentCode == CBN_SELCHANGE) {
    c = (Nlm_ChoicE) Nlm_GetChild (m);
    Nlm_DoAction ((Nlm_GraphiC) c);
  }
  rsult = TRUE;
  return rsult;
}

static Nlm_Boolean Nlm_MacPopListCommand (Nlm_GraphiC m)

{
  Nlm_Boolean  rsult;

  rsult = Nlm_MSPopListCommand (m);
  return rsult;
}

static Nlm_Boolean Nlm_SubItemCommand (Nlm_GraphiC prnt)

{
  Nlm_IteM     i;
  Nlm_MenU     m;
  Nlm_IteM     n;
  Nlm_Boolean  notInside;


  notInside = TRUE;
  m = Nlm_GetSubMenu ((Nlm_IteM) prnt);
  i = (Nlm_IteM) Nlm_GetChild ((Nlm_GraphiC) m);
  while (i != NULL && notInside) {
    n = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
    if (Nlm_GetEnabled ((Nlm_GraphiC) i) && Nlm_GetVisible ((Nlm_GraphiC) i) &&
        Nlm_DoCommand ((Nlm_GraphiC) i)) {
      notInside = FALSE;
    }
    i = n;
  }
  return (Nlm_Boolean) (! notInside);
}

static Nlm_Boolean Nlm_CommItemCommand (Nlm_GraphiC i)

{
  Nlm_Int2     id;
  Nlm_Int2     index;
  Nlm_MenU     m;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  index = Nlm_GetFirstItem ((Nlm_IteM) i);
  m = (Nlm_MenU) Nlm_GetParent (i);
  id = Nlm_ItemToID (m, index);
  if (currentItemID == id) {
    Nlm_DoAction (i);
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_StatItemCommand (Nlm_GraphiC i)

{
  Nlm_Int2     id;
  Nlm_Int2     index;
  Nlm_MenU     m;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  index = Nlm_GetFirstItem ((Nlm_IteM) i);
  m = (Nlm_MenU) Nlm_GetParent (i);
  id = Nlm_ItemToID (m, index);
  if (currentItemID == id) {
    if (Nlm_DoGetStatus (i, 0)) {
      Nlm_DoSetStatus (i, 0, FALSE, FALSE);
    } else {
      Nlm_DoSetStatus (i, 0, TRUE, FALSE);
    }
    Nlm_DoAction (i);
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_ChoiceGroupCommand (Nlm_GraphiC c)

{
  Nlm_Int2     first;
  Nlm_Int2     id;
  Nlm_Int2     index;
  Nlm_MenU     m;
  Nlm_Int2     num;
  Nlm_Boolean  rsult;
  Nlm_Int2     val;

  rsult = FALSE;
  first = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  val = Nlm_DoGetValue (c);
  index = first;
  m = (Nlm_MenU) Nlm_GetParent (c);
  while (num > 0) {
    id = Nlm_ItemToID (m, index);
    if (currentItemID == id) {
      if (val != index - first + 1) {
        Nlm_DoSetValue (c, index - first + 1, FALSE);
        Nlm_DoAction (c);
      }
      num = 0;
      rsult = TRUE;
    } else {
      index++;
      num--;
    }
  }
  return rsult;
}
#endif

#ifdef WIN_MOTIF
static void Nlm_CommItemCallback (Nlm_GraphiC i)

{
  Nlm_DoAction (i);
}

static void Nlm_StatItemCallback (Nlm_GraphiC i)

{
  Nlm_DoAction (i);
}

static void Nlm_ChoiceGroupCallback (Nlm_GraphiC c)

{
  Nlm_ItemTool  PNTR hdls;
  Nlm_Int2      i;
  Nlm_Int2      j;
  Nlm_Int2      num;

  hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  if (hdls != NULL && num > 0) {
    i = 0;
    while (i < num && hdls [i] != (Nlm_ItemTool) choiceWidget) {
      i++;
    }
    if (i < num) {
      if (XmToggleButtonGetState (hdls [i]) == FALSE) {
        XmToggleButtonSetState (hdls [i], TRUE, FALSE);
      } else {
        for (j = 0; j < num; j++) {
          if (j != i) {
            XmToggleButtonSetState (hdls [j], FALSE, FALSE);
          }
        }
        Nlm_DoAction (c);
      }
    }
  }
}

static void Nlm_PopupChoiceCallback (Nlm_GraphiC c)

{
  Nlm_ItemTool  PNTR hdls;
  Nlm_Int2      i;
  Nlm_Int2      j;
  Nlm_Int2      num;
  Nlm_Int2      val;

  hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  if (hdls != NULL && num > 0) {
    i = 0;
    while (i < num && hdls [i] != (Nlm_ItemTool) choiceWidget) {
      i++;
    }
    if (i < num) {
      i++;
      val = Nlm_GetPopupValue ((Nlm_ChoicE) c);
      if (i != val) {
        Nlm_SetPopupValue ((Nlm_ChoicE) c, i);
        XtUnmanageChild (hdls [63]);
        Nlm_DoAction (c);
      }
    }
  }
}
#endif

#ifdef WIN_MAC
static Nlm_Boolean Nlm_DesktopMenuBarKey (Nlm_GraphiC mb, Nlm_Char ch)

{
  Nlm_Int4     choice;
  Nlm_MenU     m;
  Nlm_MenU     n;
  Nlm_Boolean  notInside;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  currentMenuNum = 0;
  currentItemNum = 0;
  if (Nlm_cmmdKey) {
    choice = MenuKey (ch);
    currentMenuNum = HiWord (choice);
    currentItemNum = LoWord (choice);
    m = (Nlm_MenU) Nlm_GetChild (mb);
    notInside = TRUE;
    while (m != NULL && notInside) {
      n = (Nlm_MenU) Nlm_GetNext ((Nlm_GraphiC) m);
      currentMenuTag = Nlm_GetMenuTag (m);
      if (Nlm_GetEnabled ((Nlm_GraphiC) m) && Nlm_GetVisible ((Nlm_GraphiC) m) &&
          Nlm_DoKey ((Nlm_GraphiC) m, ch)) {
        notInside = FALSE;
      }
      m = n;
    }
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_WindowMenuBarKey (Nlm_GraphiC mb, Nlm_Char ch)

{
  Nlm_MenU     m;
  Nlm_MenU     n;
  Nlm_Boolean  notInside;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  if (Nlm_cmmdKey) {
    m = (Nlm_MenU) Nlm_GetChild (mb);
    notInside = TRUE;
    while (m != NULL && notInside) {
      n = (Nlm_MenU) Nlm_GetNext ((Nlm_GraphiC) m);
      currentMenuTag = Nlm_GetMenuTag (m);
      if (Nlm_GetEnabled ((Nlm_GraphiC) m) && Nlm_GetVisible ((Nlm_GraphiC) m) &&
          Nlm_DoKey ((Nlm_GraphiC) m, ch)) {
        notInside = FALSE;
      }
      m = n;
    }
    rsult = (! notInside);
  }
  return rsult;
}

static Nlm_Boolean Nlm_DesktopMenuKey (Nlm_GraphiC m, Nlm_Char ch)

{
  Nlm_IteM     i;
  Nlm_IteM     n;
  Nlm_Boolean  notInMenu;
  Nlm_Boolean  rsult;
  Nlm_Int2     tag;

  rsult = FALSE;
  tag = Nlm_GetMenuTag ((Nlm_MenU) m);
  if (currentMenuNum > 0 && currentMenuNum == tag) {
    i = (Nlm_IteM) Nlm_GetChild (m);
    notInMenu = TRUE;
    while (i != NULL && notInMenu) {
      n = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
      if (Nlm_GetEnabled ((Nlm_GraphiC) i) && Nlm_GetVisible ((Nlm_GraphiC) i) &&
          Nlm_DoKey ((Nlm_GraphiC) i, ch)) {
        notInMenu = FALSE;
      }
      i = n;
    }
    HiliteMenu (0);
    currentMenuNum = 0;
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_SubMenuKey (Nlm_GraphiC m, Nlm_Char ch)

{
  Nlm_IteM     i;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  i = (Nlm_IteM) Nlm_GetParent (m);
  if (i != NULL) {
    rsult = Nlm_DesktopMenuKey (m, ch);
  }
  return rsult;
}

static Nlm_Boolean Nlm_PopupKey (Nlm_GraphiC m, Nlm_Char ch)

{
  Nlm_Int4      choice;
  Nlm_MenuTool  h;
  Nlm_IteM      i;
  Nlm_IteM      n;
  Nlm_Boolean   notInMenu;
  Nlm_RecT      r;
  Nlm_Boolean   rsult;
  Nlm_Int2      tag;

  rsult = FALSE;
  if (Nlm_cmmdKey) {
    tag = Nlm_GetMenuTag ((Nlm_MenU) m);
    h = Nlm_GetMenuHandle ((Nlm_MenU) m);
    InsertMenu (h, -1);
    currentMenuNum = 0;
    currentItemNum = 0;
    choice = MenuKey (ch);
    currentMenuNum = HiWord (choice);
    currentItemNum = LoWord (choice);
    if (currentMenuNum > 0 && currentMenuNum == tag) {
      Nlm_GetRect (m, &r);
      Nlm_InvertRect (&r);
      i = (Nlm_IteM) Nlm_GetChild (m);
      notInMenu = TRUE;
      while (i != NULL && notInMenu) {
        n = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
      if (Nlm_GetEnabled ((Nlm_GraphiC) i) && Nlm_GetVisible ((Nlm_GraphiC) i) &&
          Nlm_DoKey ((Nlm_GraphiC) i, ch)) {
          notInMenu = FALSE;
        }
        i = n;
      }
      currentMenuNum = 0;
      DeleteMenu (tag);
      Nlm_DoDraw (m);
      rsult = TRUE;
    } else {
      currentMenuNum = 0;
      DeleteMenu (tag);
    }
  }
  return rsult;
}

static Nlm_Boolean Nlm_PopListKey (Nlm_GraphiC m, Nlm_Char ch)

{
  return (Nlm_PopupKey (m, ch));
}

static Nlm_Boolean Nlm_CommItemKey (Nlm_GraphiC i, Nlm_Char ch)

{
  Nlm_Int2     index;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  index = Nlm_GetFirstItem ((Nlm_IteM) i);
  if (currentItemNum == index) {
    Nlm_DoAction (i);
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_StatItemKey (Nlm_GraphiC i, Nlm_Char ch)

{
  Nlm_MenuTool  h;
  Nlm_Int2      index;
  Nlm_MenU      m;
  Nlm_Char      mark;
  Nlm_Boolean   rsult;
  Nlm_Int2      temp;

  rsult = FALSE;
  index = Nlm_GetFirstItem ((Nlm_IteM) i);
  if (currentItemNum == index) {
    m = (Nlm_MenU) Nlm_GetParent (i);
    h = Nlm_GetMenuHandle (m);
    GetItemMark (h, currentItemNum, &temp);
    mark = (Nlm_Char) temp;
    Nlm_CheckMenuItem (m, NULL, currentItemNum, (mark == '\0'));
    Nlm_DoAction (i);
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_ChoiceGroupKey (Nlm_GraphiC c, Nlm_Char ch)

{
  Nlm_Int2     first;
  Nlm_Int2     num;
  Nlm_Boolean  rsult;
  Nlm_Int2     val;

  rsult = FALSE;
  first = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  val = Nlm_DoGetValue (c);
  if (currentItemNum >= first && currentItemNum < first + num) {
    if (val != currentItemNum - first + 1) {
      Nlm_DoSetValue (c, currentItemNum - first + 1, FALSE);
      Nlm_DoAction (c);
      rsult = TRUE;
    }
  }
  return rsult;
}

static Nlm_Boolean Nlm_DAKey (Nlm_GraphiC c, Nlm_Char ch)

{
  Nlm_Int2     first;
  Nlm_Int2     num;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  first = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  if (currentItemNum >= first && currentItemNum < first + num) {
    Nlm_DeskAccProc ((Nlm_ChoicE) c);
    rsult = TRUE;
  }
  return rsult;
}
#endif

#ifdef WIN_MAC
static void Nlm_DrawWindowMenuBar (Nlm_GraphiC mb)

{
  Nlm_MenU        m;
  Nlm_RecT        r;
  Nlm_WindowTool  wptr;

  wptr = Nlm_ParentWindowPtr (mb);
  Nlm_RectToolToRecT (&(wptr->portRect), &r);
  r.top = 19;
  r.bottom = 21;
  if (Nlm_RectInRgn (&r, Nlm_updateRgn)) {
    MoveTo (0, 20);
    LineTo (r.right, 20);
  }
  m = (Nlm_MenU) Nlm_GetChild (mb);
  while (m != NULL) {
    Nlm_DoDraw ((Nlm_GraphiC) m);
    m = (Nlm_MenU) Nlm_GetNext ((Nlm_GraphiC) m);
  }
}

static void Nlm_DrawDesktopMenu (Nlm_GraphiC m)

{
  DrawMenuBar ();
}

static void Nlm_DrawWindowMenu (Nlm_GraphiC m)

{
  Nlm_PrompT  p;
  Nlm_RecT    r;

  if (Nlm_GetVisible (m) && Nlm_GetAllParentsVisible (m)) {
    Nlm_GetRect (m, &r);
    if (Nlm_RectInRgn (&r, Nlm_updateRgn)) {
      p = Nlm_GetMenuPrompt ((Nlm_MenU) m);
      if (p != NULL) {
        Nlm_DoDraw ((Nlm_GraphiC) p);
        Nlm_SelectFont (Nlm_systemFont);
      }
    }
  }
}

static void Nlm_DrawPopup (Nlm_GraphiC m)

{
  Nlm_PrompT  p;
  Nlm_RecT    r;

  if (Nlm_GetVisible (m) && Nlm_GetAllParentsVisible (m)) {
    Nlm_GetRect (m, &r);
    if (Nlm_RectInRgn (&r, Nlm_updateRgn)) {
      p = Nlm_GetMenuPrompt ((Nlm_MenU) m);
      if (p != NULL) {
        Nlm_GetRect ((Nlm_GraphiC) p, &r);
        Nlm_InsetRect (&r, -1, -1);
        Nlm_FrameRect (&r);
        Nlm_OffsetRect (&r, 1, 1);
        Nlm_FrameRect (&r);
        Nlm_DoDraw ((Nlm_GraphiC) p);
        Nlm_SelectFont (Nlm_systemFont);
      }
    }
  }
}

static void Nlm_DrawPopList (Nlm_GraphiC m)

{
  Nlm_ChoicE  c;
  Nlm_RecT    pr;
  Nlm_RecT    r;
  Nlm_Char    temp [256];
  Nlm_Int2    value;

  if (Nlm_GetVisible (m) && Nlm_GetAllParentsVisible (m)) {
    Nlm_GetRect (m, &r);
    if (Nlm_RectInRgn (&r, Nlm_updateRgn)) {
      Nlm_SelectFont (Nlm_systemFont);
      Nlm_UpsetRect (&r, 1, 1, 2, 2);
      pr = r;
      Nlm_InsetRect (&r, -1, -1);
      Nlm_FrameRect (&r);
      Nlm_OffsetRect (&r, 1, 1);
      Nlm_FrameRect (&r);
      Nlm_EraseRect (&pr);
      c = (Nlm_ChoicE) Nlm_GetChild (m);
      if (c != NULL) {
        value = Nlm_DoGetValue ((Nlm_GraphiC) c);
        if (value > 0) {
          Nlm_DoGetTitle ((Nlm_GraphiC) c, value, temp, sizeof (temp));
          pr.left += 13;
          Nlm_DrawString (&pr, temp, 'l', FALSE);
        }
      }
    }
  }
}
#endif


#ifdef DCLAP
void Nlm_SetupMenuBar()
{
#if defined(WIN_MSWIN) && !defined(WIN16)
  if (nAccel) {
  	Nlm_hAccel= CreateAcceleratorTable( gAccel, nAccel);
  	nAccel= 0; /* ?? */
  	}
#endif
}
#endif


static void Nlm_ShowWindowMenuBar (Nlm_GraphiC mb, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_WindoW      tempPort;
  Nlm_WindowTool  wptr;
#ifdef WIN_MOTIF
  Nlm_MenuTool    h;
#endif

  tempPort = Nlm_SavePortIfNeeded (mb, savePort);
  Nlm_SetVisible (mb, TRUE);
#ifdef WIN_MAC
  Nlm_DoDraw (mb);
#endif
#ifdef WIN_MSWIN
  wptr = Nlm_ParentWindowPtr (mb);
  DrawMenuBar (wptr);
#endif
#ifdef WIN_MOTIF
  h = Nlm_GetMenuBarHandle ((Nlm_MenuBaR) mb);
  XtManageChild (h);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (mb))) {
      XtRealizeWidget (h);
    }
  } else {
    XtRealizeWidget (h);
  }
#endif
  Nlm_RestorePort (tempPort);
}

#ifdef WIN_MAC
static void Nlm_ShowDesktopMenu (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_MenuTool  h;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePortIfNeeded (m, savePort);
  Nlm_SetVisible (m, TRUE);
  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
  InsertMenu (h, 0);
  Nlm_ActivateInnerMenus ((Nlm_MenU) m);
  Nlm_DoDraw (m);
  Nlm_RestorePort (tempPort);
}
#endif

static void Nlm_ShowPopup (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_PrompT      p;
  Nlm_WindoW      tempPort;
  Nlm_WindowTool  wptr;

  if (setFlag) {
    Nlm_SetVisible (m, TRUE);
  }
  if (Nlm_GetVisible (m) && Nlm_AllParentsButWindowVisible (m)) {
    tempPort = Nlm_SavePortIfNeeded (m, savePort);
    p = Nlm_GetMenuPrompt ((Nlm_MenU) m);
    if (p != NULL) {
      Nlm_DoShow ((Nlm_GraphiC) p, TRUE, FALSE);
    }
#ifdef WIN_MAC
    Nlm_DoDraw (m);
#endif
#ifdef WIN_MSWIN
    wptr = Nlm_ParentWindowPtr (m);
    DrawMenuBar (wptr);
#endif
#ifdef WIN_MOTIF
#endif
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_ShowPopListMenu (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_WindoW     tempPort;
  Nlm_PopupTool  u;

  if (setFlag) {
    Nlm_SetVisible (m, TRUE);
  }
  if (Nlm_GetVisible (m) && Nlm_AllParentsButWindowVisible (m)) {
    tempPort = Nlm_SavePortIfNeeded (m, savePort);
#ifdef WIN_MAC
    Nlm_DoDraw (m);
#endif
#ifdef WIN_MSWIN
    u = Nlm_GetMenuPopup ((Nlm_MenU) m);
    if (u != NULL) {
      ShowWindow (u, SW_SHOW);
      UpdateWindow (u);
    }
#endif
#ifdef WIN_MOTIF
    u = Nlm_GetMenuPopup ((Nlm_MenU) m);
    XtManageChild (u);
#endif
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_ShowPopListChoice (Nlm_GraphiC c, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_MenU  m;

  m = (Nlm_MenU) Nlm_GetParent (c);
  Nlm_DoShow ((Nlm_GraphiC) m, setFlag, savePort);
}

#ifdef WIN_MAC
static void Nlm_HideDesktopMenu (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_Int2    tag;
  Nlm_WindoW  tempPort;

  tempPort = Nlm_SavePortIfNeeded (m, savePort);
  Nlm_SetVisible (m, FALSE);
  tag = Nlm_GetMenuTag ((Nlm_MenU) m);
  DeleteMenu (tag);
  Nlm_DeactivateInnerMenus ((Nlm_MenU) m);
  Nlm_DoDraw (m);
  Nlm_RestorePort (tempPort);
}
#endif

static void Nlm_HidePulldown (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_RecT    r;
  Nlm_WindoW  tempPort;

  tempPort = Nlm_SavePortIfNeeded (m, savePort);
  Nlm_SetVisible (m, FALSE);
  Nlm_GetRect (m, &r);
  Nlm_EraseRect (&r);
  Nlm_ValidRect (&r);
  Nlm_RestorePort (tempPort);
#endif
}

static void Nlm_HidePopup (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_RecT    r;
  Nlm_WindoW  tempPort;

  if (setFlag) {
    Nlm_SetVisible (m, FALSE);
  }
  if (Nlm_GetAllParentsVisible (m)) {
    tempPort = Nlm_SavePortIfNeeded (m, savePort);
    Nlm_GetRect (m, &r);
    /*
    Nlm_InsetRect (&r, -2, -2);
    Nlm_OffsetRect (&r, 1, 1);
    */
    Nlm_InsetRect (&r, -1, -1);
    Nlm_EraseRect (&r);
    Nlm_ValidRect (&r);
    Nlm_RestorePort (tempPort);
  }
#endif
}

static void Nlm_HidePopListMenu (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_WindoW     tempPort;
  Nlm_PopupTool  u;
#ifdef WIN_MAC
  Nlm_RecT       r;
#endif

  if (setFlag) {
    Nlm_SetVisible (m, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (m, savePort);
#ifdef WIN_MAC
  if (Nlm_GetAllParentsVisible (m)) {
    Nlm_GetRect (m, &r);
    /*
    Nlm_InsetRect (&r, -2, -2);
    Nlm_OffsetRect (&r, 1, 1);
    */
    Nlm_InsetRect (&r, -1, -1);
    Nlm_EraseRect (&r);
    Nlm_ValidRect (&r);
  }
#endif
#ifdef WIN_MSWIN
  u = Nlm_GetMenuPopup ((Nlm_MenU) m);
  if (u != NULL) {
    ShowWindow (u, SW_HIDE);
    UpdateWindow (u);
  }
#endif
#ifdef WIN_MOTIF
    u = Nlm_GetMenuPopup ((Nlm_MenU) m);
    XtUnmanageChild (u);
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_HidePopListChoice (Nlm_GraphiC c, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_MenU  m;

  m = (Nlm_MenU) Nlm_GetParent (c);
  Nlm_DoHide ((Nlm_GraphiC) m, setFlag, savePort);
}

#ifdef WIN_MAC
static void Nlm_EnableDesktopMenu (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_MenuTool  h;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePortIfNeeded (m, savePort);
  Nlm_SetEnabled (m, TRUE);
  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
  EnableItem (h, 0);
  if (Nlm_GetVisible (m)) {
    DrawMenuBar ();
  }
  Nlm_RestorePort (tempPort);
}
#endif

static void Nlm_EnablePopup (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_MenuTool  h;
  Nlm_WindoW    tempPort;

  if (setFlag) {
    Nlm_SetEnabled (m, TRUE);
  }
  if (Nlm_GetEnabled (m) && Nlm_GetAllParentsEnabled (m)) {
    if (Nlm_GetVisible (m) && Nlm_GetAllParentsVisible (m)) {
      tempPort = Nlm_SavePortIfNeeded (m, savePort);
      h = Nlm_GetMenuHandle ((Nlm_MenU) m);
      EnableItem (h, 0);
      Nlm_DoDraw (m);
      Nlm_RestorePort (tempPort);
    }
  }
#endif
}

static void Nlm_EnablePopList (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_WindoW     tempPort;
  Nlm_PopupTool  u;
#ifdef WIN_MAC
  Nlm_MenuTool   h;
#endif

  if (setFlag) {
    Nlm_SetEnabled (m, TRUE);
  }
  if (Nlm_GetEnabled (m) && Nlm_GetAllParentsEnabled (m)) {
    tempPort = Nlm_SavePortIfNeeded (m, savePort);
#ifdef WIN_MAC
    h = Nlm_GetMenuHandle ((Nlm_MenU) m);
    EnableItem (h, 0);
    if (Nlm_GetVisible (m) && Nlm_GetAllParentsVisible (m)) {
      Nlm_DoDraw (m);
    }
#endif
#ifdef WIN_MSWIN
    u = Nlm_GetMenuPopup ((Nlm_MenU) m);
    if (u != NULL) {
      EnableWindow (u, TRUE);
    }
#endif
#ifdef WIN_MOTIF
#endif
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_EnableMenuItem (Nlm_GraphiC i, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_MenuTool  h;
  Nlm_Int2      id;
  Nlm_Int2      itm;
  Nlm_MenU      m;
  Nlm_WindoW    tempPort;
#ifdef WIN_MOTIF
  Nlm_ItemTool  itool;
#endif

  tempPort = Nlm_SavePortIfNeeded (i, savePort);
  Nlm_SetEnabled (i, TRUE);
  m = (Nlm_MenU) Nlm_GetParent (i);
  h = Nlm_GetMenuHandle (m);
  itm = Nlm_GetFirstItem ((Nlm_IteM) i);
#ifdef WIN_MAC
  EnableItem (h, itm);
#endif
#ifdef WIN_MSWIN
  id = Nlm_ItemToID (m, itm);
  EnableMenuItem (h, id, MF_ENABLED);
#endif
#ifdef WIN_MOTIF
  itool = Nlm_GetItemHandle ((Nlm_IteM) i);
  XtVaSetValues (itool, XmNsensitive, TRUE, NULL);
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_EnableSubMenu (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_MenuTool   h;
  Nlm_IteM       i;
  Nlm_Int2       id;
  Nlm_Int2       index;
  Nlm_MenU       prnt;
  Nlm_WindoW     tempPort;
#ifdef WIN_MOTIF
  Nlm_PopupTool  pop;
#endif

  tempPort = Nlm_SavePortIfNeeded (m, savePort);
  Nlm_SetEnabled (m, TRUE);
  i = (Nlm_IteM) Nlm_GetParent (m);
  if (i != NULL) {
    Nlm_EnableMenuItem ((Nlm_GraphiC) i, FALSE, FALSE);
#ifdef WIN_MAC
    h = Nlm_GetMenuHandle ((Nlm_MenU) m);
    InsertMenu (h, -1);
#endif
#ifdef WIN_MSWIN
    prnt = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) i);
    index = Nlm_GetFirstItem (i);
    h = Nlm_GetMenuHandle (prnt);
    id = Nlm_ItemToID (prnt, index);
    EnableMenuItem (h, id, MF_ENABLED);
#endif
#ifdef WIN_MOTIF
    pop = Nlm_GetMenuPopup ((Nlm_MenU) m);
    XtVaSetValues (pop, XmNsensitive, TRUE, NULL);
#endif
    Nlm_ActivateInnerMenus ((Nlm_MenU) m);
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_EnableChoice (Nlm_GraphiC c, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_MenuTool  h;
  Nlm_Int2      id;
  Nlm_Int2      itm;
  Nlm_MenU      m;
  Nlm_Int2      num;
  Nlm_WindoW    tempPort;
#ifdef WIN_MOTIF
  Nlm_ItemTool  PNTR hdls;
#endif

  tempPort = Nlm_SavePortIfNeeded (c, savePort);
  Nlm_SetEnabled (c, TRUE);
  m = (Nlm_MenU) Nlm_GetParent (c);
  itm = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  h = Nlm_GetMenuHandle (m);
  while (num > 0) {
#ifdef WIN_MAC
    EnableItem (h, itm);
#endif
#ifdef WIN_MSWIN
    if (! Nlm_IsItAPopupList ((Nlm_ChoicE) c)) {
      id = Nlm_ItemToID (m, itm);
      EnableMenuItem (h, id, MF_ENABLED);
    }
#endif
#ifdef WIN_MOTIF
    hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
    XtVaSetValues (hdls [num - 1], XmNsensitive, TRUE, NULL);
#endif
    itm++;
    num--;
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_EnableChoiceGroup (Nlm_GraphiC c, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_MenuTool  h;
  Nlm_Int2      id;
  Nlm_Int2      itm;
  Nlm_MenU      m;
  Nlm_Int2      num;
  Nlm_WindoW    tempPort;
#ifdef WIN_MOTIF
  Nlm_ItemTool  PNTR hdls;
#endif

  tempPort = Nlm_SavePortIfNeeded (c, savePort);
  Nlm_SetEnabled (c, TRUE);
  m = (Nlm_MenU) Nlm_GetParent (c);
  itm = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  h = Nlm_GetMenuHandle (m);
  while (num > 0) {
#ifdef WIN_MAC
    EnableItem (h, itm);
#endif
#ifdef WIN_MSWIN
    if (! Nlm_IsItAPopupList ((Nlm_ChoicE) c)) {
      id = Nlm_ItemToID (m, itm);
      EnableMenuItem (h, id, MF_ENABLED);
    }
#endif
#ifdef WIN_MOTIF
    hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
    XtVaSetValues (hdls [num - 1], XmNsensitive, TRUE, NULL);
#endif
    itm++;
    num--;
  }
  Nlm_RestorePort (tempPort);
}

#ifdef DCLAP
extern
#else
static
#endif 
void Nlm_EnableChoiceItem (Nlm_GraphiC i, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_ChoicE    c;
  Nlm_MenuTool  h;
  Nlm_Int2      id;
  Nlm_Int2      index;
  Nlm_Int2      itm;
  Nlm_MenU      m;
  Nlm_WindoW    tempPort;
#ifdef WIN_MOTIF
  Nlm_ItemTool  PNTR hdls;
#endif

  tempPort = Nlm_SavePortIfNeeded (i, savePort);
  Nlm_SetEnabled (i, TRUE);
  c = (Nlm_ChoicE) Nlm_GetParent (i);
  index = Nlm_GetFirstItem ((Nlm_IteM) i);
  if (index > 0 && index <= Nlm_GetNumItems ((Nlm_ChoicE) c)) {
    m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) c);
    h = Nlm_GetMenuHandle (m);
    itm = Nlm_GetFirstItem ((Nlm_IteM) c) + index - 1;
#ifdef WIN_MAC
    EnableItem (h, itm);
#endif
#ifdef WIN_MSWIN
    if (! Nlm_IsItAPopupList ((Nlm_ChoicE) c)) {
      id = Nlm_ItemToID (m, itm);
      EnableMenuItem (h, id, MF_ENABLED);
    }
#endif
#ifdef WIN_MOTIF
    hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
    XtVaSetValues (hdls [index - 1], XmNsensitive, TRUE, NULL);
#endif
  }
  Nlm_RestorePort (tempPort);
}

#ifdef WIN_MAC
static void Nlm_DisableDesktopMenu (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_MenuTool  h;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePortIfNeeded (m, savePort);
  Nlm_SetEnabled (m, FALSE);
  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
  DisableItem (h, 0);
  if (Nlm_GetVisible (m)) {
    DrawMenuBar ();
  }
  Nlm_RestorePort (tempPort);
}
#endif

static void Nlm_DisablePopup (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_MenuTool  h;
  Nlm_WindoW    tempPort;

  if (setFlag) {
    Nlm_SetEnabled (m, FALSE);
  }
  if (Nlm_GetVisible (m) && Nlm_GetAllParentsVisible (m)) {
    tempPort = Nlm_SavePortIfNeeded (m, savePort);
    h = Nlm_GetMenuHandle ((Nlm_MenU) m);
    DisableItem (h, 0);
    Nlm_DoDraw (m);
    Nlm_RestorePort (tempPort);
  }
#endif
}

static void Nlm_DisablePopList (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_WindoW     tempPort;
  Nlm_PopupTool  u;
#ifdef WIN_MAC
  Nlm_MenuTool   h;
#endif

  if (setFlag) {
    Nlm_SetEnabled (m, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (m, savePort);
#ifdef WIN_MAC
  if (Nlm_GetVisible (m) && Nlm_GetAllParentsVisible (m)) {
    h = Nlm_GetMenuHandle ((Nlm_MenU) m);
    DisableItem (h, 0);
    Nlm_DoDraw (m);
  }
#endif
#ifdef WIN_MSWIN
  u = Nlm_GetMenuPopup ((Nlm_MenU) m);
  if (u != NULL) {
    EnableWindow (u, FALSE);
  }
#endif
#ifdef WIN_MOTIF
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_DisableMenuItem (Nlm_GraphiC i, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_MenuTool  h;
  Nlm_Int2      id;
  Nlm_Int2      itm;
  Nlm_MenU      m;
  Nlm_WindoW    tempPort;
#ifdef WIN_MOTIF
  Nlm_ItemTool  itool;
#endif

  tempPort = Nlm_SavePortIfNeeded (i, savePort);
  Nlm_SetEnabled (i, FALSE);
  m = (Nlm_MenU) Nlm_GetParent (i);
  h = Nlm_GetMenuHandle (m);
  itm = Nlm_GetFirstItem ((Nlm_IteM) i);
#ifdef WIN_MAC
  DisableItem (h, itm);
#endif
#ifdef WIN_MSWIN
  id = Nlm_ItemToID (m, itm);
  EnableMenuItem (h, id, MF_GRAYED);
#endif
#ifdef WIN_MOTIF
  itool = Nlm_GetItemHandle ((Nlm_IteM) i);
  XtVaSetValues (itool, XmNsensitive, FALSE, NULL);
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_DisableSubMenu (Nlm_GraphiC m, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_MenuTool   h;
  Nlm_IteM       i;
  Nlm_Int2       id;
  Nlm_Int2       index;
  Nlm_MenU       prnt;
  Nlm_WindoW     tempPort;
#ifdef WIN_MAC
  Nlm_Int2       tag;
#endif
#ifdef WIN_MOTIF
  Nlm_PopupTool  pop;
#endif

  tempPort = Nlm_SavePortIfNeeded (m, savePort);
  Nlm_SetEnabled (m, FALSE);
  i = (Nlm_IteM) Nlm_GetParent (m);
  if (i != NULL) {
    Nlm_DisableMenuItem ((Nlm_GraphiC) i, FALSE, FALSE);
#ifdef WIN_MAC
    tag = Nlm_GetMenuTag ((Nlm_MenU) m);
    DeleteMenu (tag);
#endif
#ifdef WIN_MSWIN
    prnt = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) i);
    index = Nlm_GetFirstItem (i);
    h = Nlm_GetMenuHandle (prnt);
    id = Nlm_ItemToID (prnt, index);
    EnableMenuItem (h, id, MF_GRAYED);
#endif
#ifdef WIN_MOTIF
    pop = Nlm_GetMenuPopup ((Nlm_MenU) m);
    XtVaSetValues (pop, XmNsensitive, FALSE, NULL);
#endif
    Nlm_DeactivateInnerMenus ((Nlm_MenU) m);
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_DisableChoice (Nlm_GraphiC c, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_MenuTool  h;
  Nlm_Int2      id;
  Nlm_Int2      itm;
  Nlm_MenU      m;
  Nlm_Int2      num;
  Nlm_WindoW    tempPort;
#ifdef WIN_MOTIF
  Nlm_ItemTool  PNTR hdls;
#endif

  tempPort = Nlm_SavePortIfNeeded (c, savePort);
  Nlm_SetEnabled (c, FALSE);
  m = (Nlm_MenU) Nlm_GetParent (c);
  itm = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  h = Nlm_GetMenuHandle (m);
  while (num > 0) {
#ifdef WIN_MAC
    DisableItem (h, itm);
#endif
#ifdef WIN_MSWIN
    if (! Nlm_IsItAPopupList ((Nlm_ChoicE) c)) {
      id = Nlm_ItemToID (m, itm);
      EnableMenuItem (h, id, MF_GRAYED);
    }
#endif
#ifdef WIN_MOTIF
    hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
    XtVaSetValues (hdls [num - 1], XmNsensitive, FALSE, NULL);
#endif
    itm++;
    num--;
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_DisableChoiceGroup (Nlm_GraphiC c, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_MenuTool  h;
  Nlm_Int2      id;
  Nlm_Int2      itm;
  Nlm_MenU      m;
  Nlm_Int2      num;
  Nlm_WindoW    tempPort;
#ifdef WIN_MOTIF
  Nlm_ItemTool  PNTR hdls;
#endif

  tempPort = Nlm_SavePortIfNeeded (c, savePort);
  Nlm_SetEnabled (c, FALSE);
  m = (Nlm_MenU) Nlm_GetParent (c);
  itm = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  h = Nlm_GetMenuHandle (m);
  while (num > 0) {
#ifdef WIN_MAC
    DisableItem (h, itm);
#endif
#ifdef WIN_MSWIN
    if (! Nlm_IsItAPopupList ((Nlm_ChoicE) c)) {
      id = Nlm_ItemToID (m, itm);
      EnableMenuItem (h, id, MF_GRAYED);
    }
#endif
#ifdef WIN_MOTIF
    hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
    XtVaSetValues (hdls [num - 1], XmNsensitive, FALSE, NULL);
#endif
    itm++;
    num--;
  }
  Nlm_RestorePort (tempPort);
}

#ifdef DCLAP
extern
#else
static
#endif 
void Nlm_DisableChoiceItem (Nlm_GraphiC i, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_ChoicE    c;
  Nlm_MenuTool  h;
  Nlm_Int2      id;
  Nlm_Int2      index;
  Nlm_Int2      itm;
  Nlm_MenU      m;
  Nlm_WindoW    tempPort;
#ifdef WIN_MOTIF
  Nlm_ItemTool  PNTR hdls;
#endif

  tempPort = Nlm_SavePortIfNeeded (i, savePort);
  Nlm_SetEnabled (i, FALSE);
  c = (Nlm_ChoicE) Nlm_GetParent (i);
  index = Nlm_GetFirstItem ((Nlm_IteM) i);
  if (index > 0 && index <= Nlm_GetNumItems ((Nlm_ChoicE) c)) {
    m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) c);
    h = Nlm_GetMenuHandle (m);
    itm = Nlm_GetFirstItem ((Nlm_IteM) c) + index - 1;
#ifdef WIN_MAC
    DisableItem (h, itm);
#endif
#ifdef WIN_MSWIN
    if (! Nlm_IsItAPopupList ((Nlm_ChoicE) c)) {
      id = Nlm_ItemToID (m, itm);
      EnableMenuItem (h, id, MF_GRAYED);
    }
#endif
#ifdef WIN_MOTIF
    hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
    XtVaSetValues (hdls [index - 1], XmNsensitive, FALSE, NULL);
#endif
  }
  Nlm_RestorePort (tempPort);
}

#ifdef WIN_MAC
static void Nlm_RemoveDesktopMenuBar (Nlm_GraphiC mb, Nlm_Boolean savePort)

/*
*  A submenu is linked into the desktop menu bar immediately after the menu
*  that contains it is created.  Thus, removal of desktop menus cannot safely
*  get the next child in the list of menus right before removing a link, since
*  that next child, if a submenu, will immediately become a dangling pointer.
*  Removal of desktop menus must instead repeatedly remove the first child of
*  the desktop menu bar.
*/

{
  Nlm_MenuTool  h;
  Nlm_MenU      m;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePortIfNeeded (mb, savePort);
  m = (Nlm_MenU) Nlm_GetChild (mb);
  while (m != NULL) {
    Nlm_DeactivateInnerMenus (m);
    Nlm_DoRemove ((Nlm_GraphiC) m, FALSE);
    m = (Nlm_MenU) Nlm_GetChild (mb);
  }
  Nlm_HandFree (mb);
  recentMenuBar = NULL;
  Nlm_RestorePort (tempPort);
}
#endif

static void Nlm_RemoveWindowMenuBar (Nlm_GraphiC mb, Nlm_Boolean savePort)

{
  Nlm_MenuTool  h;
  Nlm_MenU      m;
  Nlm_MenU      n;
  Nlm_WindoW    tempPort;
  Nlm_WindoW    w;

  tempPort = Nlm_SavePortIfNeeded (mb, savePort);
  m = (Nlm_MenU) Nlm_GetChild (mb);
  while (m != NULL) {
    n = (Nlm_MenU) Nlm_GetNext ((Nlm_GraphiC) m);
    Nlm_DeactivateInnerMenus (m);
    m = n;
  }
  m = (Nlm_MenU) Nlm_GetChild (mb);
  while (m != NULL) {
    n = (Nlm_MenU) Nlm_GetNext ((Nlm_GraphiC) m);
    Nlm_DoRemove ((Nlm_GraphiC) m, FALSE);
    m = n;
  }
  w = Nlm_GetParentWindow ((Nlm_GraphiC) mb);
  Nlm_SetWindowMenuBar (w, NULL);
#ifdef WIN_MSWIN
  h = Nlm_GetMenuBarHandle ((Nlm_MenuBaR) mb);
  DestroyMenu (h);
#endif
#ifdef WIN_MOTIF
  h = Nlm_GetMenuBarHandle ((Nlm_MenuBaR) mb);
  XtDestroyWidget (h);
#endif
  Nlm_HandFree (mb);
  recentMenuBar = NULL;
  Nlm_RestorePort (tempPort);
}

#ifdef WIN_MAC
static void Nlm_RemoveDesktopMenu (Nlm_GraphiC m, Nlm_Boolean savePort)

{
  Nlm_IteM      i;
  Nlm_MenuTool  h;
  Nlm_IteM      n;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePortIfNeeded (m, savePort);
  i = (Nlm_IteM) Nlm_GetChild (m);
  while (i != NULL) {
    n = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
    Nlm_DoRemove ((Nlm_GraphiC) i, FALSE);
    i = n;
  }
  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
  DisposeMenu (h);
  DrawMenuBar ();
  Nlm_RemoveLink (m);
  recentMenu = NULL;
  Nlm_RestorePort (tempPort);
}
#endif

static void Nlm_RemoveSubMenu (Nlm_GraphiC m, Nlm_Boolean savePort)

{
  Nlm_IteM      i;
  Nlm_MenuTool  h;
  Nlm_IteM      n;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  Nlm_MenuBaR   mb;
#endif

  tempPort = Nlm_SavePortIfNeeded (m, savePort);
  Nlm_DoDisable (m, TRUE, FALSE);
  i = (Nlm_IteM) Nlm_GetChild (m);
  while (i != NULL) {
    n = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
    Nlm_DoRemove ((Nlm_GraphiC) i, FALSE);
    i = n;
  }
  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
#ifdef WIN_MAC
  DisposeMenu (h);
#endif
#ifdef WIN_MSWIN
  DestroyMenu (h);
#endif
#ifdef WIN_MOTIF
#endif
#ifdef WIN_MAC
  i = (Nlm_IteM) Nlm_GetParent (m);
  Nlm_SetSubMenu (i, NULL);
  mb = Nlm_GetWindowMenuBar (Nlm_desktopWindow);
  Nlm_SetParent (m, (Nlm_GraphiC) mb);
  Nlm_RemoveLink (m);
#else
  Nlm_HandFree (m);
#endif
  recentMenu = NULL;
  Nlm_RestorePort (tempPort);
}

static void Nlm_RemovePopupMenu (Nlm_GraphiC m, Nlm_Boolean savePort)

{
  Nlm_IteM       i;
  Nlm_IteM       n;
  Nlm_PrompT     p;
  Nlm_WindoW     tempPort;
  Nlm_PopupTool  u;
#ifdef WIN_MAC
  Nlm_MenuTool   h;
#endif
#ifdef WIN_MOTIF
  Nlm_MenuTool   h;
  Nlm_PopupTool  pop;
#endif

  tempPort = Nlm_SavePortIfNeeded (m, savePort);
  i = (Nlm_IteM) Nlm_GetChild (m);
  while (i != NULL) {
    n = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
    Nlm_DoRemove ((Nlm_GraphiC) i, FALSE);
    i = n;
  }
#ifdef WIN_MAC
  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
  DisposeMenu (h);
#endif
#ifdef WIN_MSWIN
  u = Nlm_GetMenuPopup ((Nlm_MenU) m);
  if (u != NULL) {
    RemoveProp (u, (LPSTR) "Nlm_VibrantProp");
    DestroyWindow (u);
  }
#endif
#ifdef WIN_MOTIF
  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
  if (h != NULL) {
    /*
      This code will, for some unknown reason, crash on the next
      update if you run a program from outside the window manager.
    */
    /*
    XtDestroyWidget (h);
    */
  }
  pop = Nlm_GetMenuPopup ((Nlm_MenU) m);
  if (pop != NULL) {
    XtDestroyWidget (pop);
  }
#endif
  p = Nlm_GetMenuPrompt ((Nlm_MenU) m);
  if (p != NULL) {
    Nlm_DoRemove ((Nlm_GraphiC) p, FALSE);
  }
  Nlm_RemoveLink (m);
  recentMenu = NULL;
  Nlm_RestorePort (tempPort);
}

static void Nlm_RemovePopListMenu (Nlm_GraphiC m, Nlm_Boolean savePort)

{
  Nlm_IteM       i;
  Nlm_IteM       n;
  Nlm_PrompT     p;
  Nlm_WindoW     tempPort;
  Nlm_PopupTool  u;
#ifdef WIN_MAC
  Nlm_MenuTool   h;
#endif

  tempPort = Nlm_SavePortIfNeeded (m, savePort);
  i = (Nlm_IteM) Nlm_GetChild (m);
  while (i != NULL) {
    n = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
    Nlm_DoRemove ((Nlm_GraphiC) i, FALSE);
    i = n;
  }
#ifdef WIN_MAC
  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
  DisposeMenu (h);
#endif
#ifdef WIN_MSWIN
  u = Nlm_GetMenuPopup ((Nlm_MenU) m);
  if (u != NULL) {
    RemoveProp (u, (LPSTR) "Nlm_VibrantProp");
    DestroyWindow (u);
  }
#endif
#ifdef WIN_MOTIF
#endif
  p = Nlm_GetMenuPrompt ((Nlm_MenU) m);
  if (p != NULL) {
    Nlm_DoRemove ((Nlm_GraphiC) p, FALSE);
  }
  Nlm_RemoveLink (m);
  recentMenu = NULL;
  Nlm_RestorePort (tempPort);
}

static void Nlm_RemovePopupList (Nlm_GraphiC c, Nlm_Boolean savePort)

/*
*  If the user calls Remove (c) to remove the popup list, then
*  savePort will be TRUE, and the implicitly created parent menu
*  is also removed.  If the user calls Remove (w) to remove the
*  parent window, then RemovePopupList will be called by the
*  implicitly created parent menu just prior to its removal, and
*  an additional attempt to remove the menu is both unnecessary
*  and possibly dangerous.
*/

{
  Nlm_MenU      m;
  Nlm_WindoW    tempPort;
#ifdef WIN_MOTIF
  Nlm_ItemTool   PNTR hdls;
#endif

  tempPort = Nlm_SavePortIfNeeded (c, savePort);
#ifdef WIN_MOTIF
  hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
  if (hdls != NULL) {
    Nlm_MemFree (hdls);
  }
#endif
  m = (Nlm_MenU) Nlm_GetParent (c);
  Nlm_RemoveLink (c);
  recentItem = NULL;
  recentChoice = NULL;
  if (savePort) {
    Nlm_DoRemove ((Nlm_GraphiC) m, FALSE);
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_RemoveMenuItem (Nlm_GraphiC i, Nlm_Boolean savePort)

{
  Nlm_MenU    sub;
  Nlm_WindoW  tempPort;

  tempPort = Nlm_SavePortIfNeeded (i, savePort);
  sub = Nlm_GetSubMenu ((Nlm_IteM) i);
  if (sub != NULL) {
    Nlm_DoRemove ((Nlm_GraphiC) sub, FALSE);
  }
  Nlm_RemoveLink (i);
  recentItem = NULL;
  recentChoice = NULL;
  Nlm_RestorePort (tempPort);
}

static void Nlm_RemoveChoiceGroup (Nlm_GraphiC c, Nlm_Boolean savePort)

{
  Nlm_IteM    i;
  Nlm_IteM    n;
  Nlm_MenU    sub;
  Nlm_WindoW  tempPort;
#ifdef WIN_MOTIF
  Nlm_ItemTool  PNTR hdls;
#endif

  tempPort = Nlm_SavePortIfNeeded (c, savePort);
  sub = Nlm_GetSubMenu ((Nlm_IteM) c);
  if (sub != NULL) {
    Nlm_DoRemove ((Nlm_GraphiC) sub, FALSE);
  }
#ifdef WIN_MOTIF
  hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
  if (hdls != NULL) {
    Nlm_MemFree (hdls);
  }
#endif
  i = (Nlm_IteM) Nlm_GetChild (c);
  while (i != NULL) {
    n = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
    Nlm_DoRemove ((Nlm_GraphiC) i, FALSE);
    i = n;
  }
  Nlm_RemoveLink (c);
  recentItem = NULL;
  recentChoice = NULL;
  Nlm_RestorePort (tempPort);
}

static void Nlm_RemoveChoiceItem (Nlm_GraphiC i, Nlm_Boolean savePort)

{
  Nlm_WindoW  tempPort;

  tempPort = Nlm_SavePortIfNeeded (i, savePort);
  Nlm_RemoveLink (i);
  recentItem = NULL;
  recentChoice = NULL;
  Nlm_RestorePort (tempPort);
}

static void Nlm_ResetMenu (Nlm_GraphiC m, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_Int2      cnt;
  Nlm_IteM      i;
  Nlm_MenuTool  h;
  Nlm_IteM      n;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePortIfNeeded (m, savePort);
  i = (Nlm_IteM) Nlm_GetChild (m);
  while (i != NULL) {
    n = (Nlm_IteM) Nlm_GetNext ((Nlm_GraphiC) i);
    Nlm_DoRemove ((Nlm_GraphiC) i, FALSE);
    i = n;
  }
  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
  cnt = CountMItems (h);
  while (cnt > 0) {
    DelMenuItem (h, cnt);
    cnt--;
  }
  Nlm_RestorePort (tempPort);
#endif
}

#ifndef WIN_MOTIF
static void Nlm_ResetPopList (Nlm_GraphiC c, Nlm_Boolean savePort)

{
  Nlm_Int2        hbounds;
  Nlm_MenU        m;
  Nlm_RecT        r;
  Nlm_WindoW      tempPort;
  Nlm_PopupTool   u;
  Nlm_Int2        vbounds;
#ifdef WIN_MAC
  Nlm_Int2        cnt;
  Nlm_MenuTool    h;
#endif

  tempPort = Nlm_SavePortIfNeeded (c, savePort);
  m = (Nlm_MenU) Nlm_GetParent (c);
#ifdef WIN_MAC
  h = Nlm_GetMenuHandle (m);
  cnt = CountMItems (h);
  while (cnt > 0) {
    DelMenuItem (h, cnt);
    cnt--;
  }
#endif
#ifdef WIN_MSWIN
  u = Nlm_GetMenuPopup (m);
  if (u != NULL) {
    ComboBox_ResetContent (u);
  }
#endif
  Nlm_LoadItemData ((Nlm_IteM) c, NULL, 1, NULL);
  Nlm_LoadChoiceData ((Nlm_ChoicE) c, 0, TRUE, 0, NULL);
#ifdef WIN_MAC
  hbounds = 13;
  vbounds = 2;
#endif
#ifdef WIN_MSWIN
  hbounds = 13;
  vbounds = 4;
#endif
  Nlm_GetRect ((Nlm_GraphiC) m, &r);
  r.right = r.left + hbounds * 2;
  r.bottom = r.top + Nlm_stdLineHeight + vbounds * 2;
  Nlm_DoSetPosition ((Nlm_GraphiC) c, &r, FALSE);
  Nlm_RestorePort (tempPort);
}
#endif

#ifdef WIN_MOTIF
static void Nlm_NewPopupList (Nlm_MenU m, Nlm_ChoicE c, Nlm_PupActnProc actn);

static void Nlm_ResetPopList (Nlm_GraphiC c, Nlm_Boolean savePort)

{
  Nlm_PupActnProc  actn;
  Nlm_ChoiceData   cdata;
  Nlm_GraphicData  gdata;
  Nlm_MenuTool     h;
  Nlm_ItemTool     *hdls;
  Nlm_Int2         i;
  Nlm_ItemData     idata;
  Nlm_ItemTool     itool;
  Nlm_PopupTool    pop;
  Nlm_MenU         m;
  Nlm_WindoW       tempPort;

  tempPort = Nlm_SavePortIfNeeded (c, savePort);
  m = (Nlm_MenU) Nlm_GetParent (c);
  Nlm_DoSetValue (c, 0, FALSE);
  Nlm_GetGraphicData (c, &gdata);
  actn = (Nlm_PupActnProc) gdata.action;
  hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
  if (hdls != NULL) {
    for (i = 0; i < 64; i++) {
      itool = hdls [i];
      if (itool != NULL) {
        XtUnmanageChild (itool);
        XtDestroyWidget (itool);
        hdls [i] = NULL;
      }
    }
    Nlm_MemFree (hdls);
  }
  pop = Nlm_GetMenuPopup (m);
  if (pop != NULL) {
    XtDestroyWidget (pop);
  }
  h = Nlm_GetMenuHandle (m);
  if (h != NULL) {
    XtDestroyWidget (h);
  }
  Nlm_NewPopupList (m, (Nlm_ChoicE) c, actn);
  Nlm_RestorePort (tempPort);
}
#endif

static Nlm_Int2 Nlm_CountMenuItems (Nlm_GraphiC m)

{
  Nlm_MenuTool  h;
  Nlm_Int2      rsult;

  rsult = 0;
  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
#ifdef WIN_MAC
  rsult = CountMItems (h);
#endif
#ifdef WIN_MSWIN
  rsult = GetMenuItemCount (h);
#endif
#ifdef WIN_MOTIF
#endif
  return rsult;
}

static Nlm_Int2 Nlm_CountChoiceItems (Nlm_GraphiC c)

{
  return (Nlm_GetNumItems ((Nlm_ChoicE) c));
}

static void Nlm_SetMenuItem (Nlm_MenU m, Nlm_Int2 item,
                             Nlm_CharPtr itemName)

{
#ifdef WIN_MAC
  Nlm_MenuTool  h;
  Nlm_Char      temp [256];

  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
  if (item > 0) {
    if (Nlm_StringLen (itemName) == 0) {
      temp [0] = '(';
      temp [1] = '\0';
      Nlm_CtoPstr (temp);
      SetItem (h, item, (StringPtr) temp);
    } else {
      Nlm_StringNCpy (temp, itemName, sizeof (temp));
      Nlm_CtoPstr (temp);
      SetItem (h, item, (StringPtr) temp);
    }
  }
#endif
}

static void Nlm_SetSingleTitle (Nlm_GraphiC i, Nlm_Int2 item,
                                Nlm_CharPtr title, Nlm_Boolean savePort)

{
  Nlm_Int2    itm;
  Nlm_MenU    m;
  Nlm_WindoW  tempPort;

  tempPort = Nlm_SavePortIfNeeded (i, savePort);
  m = (Nlm_MenU) Nlm_GetParent (i);
  itm = Nlm_GetFirstItem ((Nlm_IteM) i);
  Nlm_SetMenuItem (m, itm, title);
  Nlm_RestorePort (tempPort);
}

static void Nlm_SetChoiceTitle (Nlm_GraphiC c, Nlm_Int2 item,
                                Nlm_CharPtr title, Nlm_Boolean savePort)

{
  Nlm_Int2    first;
  Nlm_MenU    m;
  Nlm_Int2    num;
  Nlm_PrompT  p;
  Nlm_WindoW  tempPort;

  tempPort = Nlm_SavePortIfNeeded (c, savePort);
  first = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  m = (Nlm_MenU) Nlm_GetParent (c);
  if (item > 0 && item <= num) {
    Nlm_SetMenuItem (m, first + item - 1, title);
  } else if (item == 0) {
    p = Nlm_GetMenuPrompt (m);
    if (p != NULL) {
      Nlm_DoSetTitle ((Nlm_GraphiC) p, 0, title, FALSE);
    }
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_SetChoiceGroupTitle (Nlm_GraphiC c, Nlm_Int2 item,
                                     Nlm_CharPtr title, Nlm_Boolean savePort)

{
  Nlm_MenU    m;
  Nlm_PrompT  p;
  Nlm_WindoW  tempPort;

  tempPort = Nlm_SavePortIfNeeded (c, savePort);
  m = (Nlm_MenU) Nlm_GetParent (c);
  p = Nlm_GetMenuPrompt (m);
  if (p != NULL) {
    Nlm_DoSetTitle ((Nlm_GraphiC) p, 0, title, FALSE);
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_SetChoiceItemTitle (Nlm_GraphiC i, Nlm_Int2 item,
                                    Nlm_CharPtr title, Nlm_Boolean savePort)

{
  Nlm_ChoicE  c;
  Nlm_Int2    first;
  Nlm_Int2    index;
  Nlm_MenU    m;
  Nlm_WindoW  tempPort;

  tempPort = Nlm_SavePortIfNeeded (i, savePort);
  c = (Nlm_ChoicE) Nlm_GetParent (i);
  index = Nlm_GetFirstItem ((Nlm_IteM) i);
  first = Nlm_GetFirstItem ((Nlm_IteM) c);
  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) c);
  Nlm_SetMenuItem (m, first + index - 1, title);
  Nlm_RestorePort (tempPort);
}

static void Nlm_SetPopListTitle (Nlm_GraphiC c, Nlm_Int2 item,
                                 Nlm_CharPtr title, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_SetChoiceTitle (c, item, title, savePort);
#endif
}

static void Nlm_SetSubmenuTitle (Nlm_GraphiC m, Nlm_Int2 item,
                                 Nlm_CharPtr title, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_IteM    i;
  Nlm_WindoW  tempPort;

  tempPort = Nlm_SavePortIfNeeded (m, savePort);
  i = (Nlm_IteM) Nlm_GetParent (m);
  if (i != NULL) {
    Nlm_SetSingleTitle ((Nlm_GraphiC) i, item, title, FALSE);
  }
  Nlm_RestorePort (tempPort);
#endif
}

static void Nlm_GetMenuItem (Nlm_GraphiC m, Nlm_Int2 item,
                             Nlm_CharPtr itemName, Nlm_sizeT maxsize)

{
  Nlm_MenuTool  h;
  Nlm_Int2      id;
  Nlm_Char      temp [256];
#ifdef WIN_MOTIF
  XmString      label;
  char          *text;
#endif

  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
  if (item > 0) {
#ifdef WIN_MAC
    GetItem (h, item, (StringPtr) temp);
    Nlm_PtoCstr (temp);
#endif
#ifdef WIN_MSWIN
    id = Nlm_ItemToID ((Nlm_MenU) m, item);
    GetMenuString (h, id, temp, sizeof (temp), MF_BYCOMMAND);
#endif
#ifdef WIN_MOTIF
    XtVaGetValues (h, XmNlabelString, &label, NULL);
    temp [0] = '\0';
    if (XmStringGetLtoR (label, XmSTRING_DEFAULT_CHARSET, &text)) {
      Nlm_StringNCpy (temp, text, sizeof (temp));
      XtFree (text);
    }
#endif
  } else {
    temp [0] = '\0';
  }
  Nlm_StringNCpy (itemName, temp, maxsize);
}

static void Nlm_GetSingleTitle (Nlm_GraphiC i, Nlm_Int2 item,
                                Nlm_CharPtr title, Nlm_sizeT maxsize)

{
  Nlm_Int2  itm;
  Nlm_MenU  m;

  m = (Nlm_MenU) Nlm_GetParent (i);
  itm = Nlm_GetFirstItem ((Nlm_IteM) i);
  Nlm_GetMenuItem ((Nlm_GraphiC) m, itm, title, maxsize);
}

extern void Nlm_GetChoiceTitle (Nlm_GraphiC c, Nlm_Int2 item,
                                Nlm_CharPtr title, Nlm_sizeT maxsize)

{
  Nlm_Int2    first;
  Nlm_MenU    m;
  Nlm_Int2    num;
  Nlm_PrompT  p;

  first = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  m = (Nlm_MenU) Nlm_GetParent (c);
  if (item > 0 && item <= num) {
    Nlm_GetMenuItem ((Nlm_GraphiC) m, first + item - 1, title, maxsize);
  } else if (item == 0) {
    p = Nlm_GetMenuPrompt (m);
    if (p != NULL) {
      Nlm_DoGetTitle ((Nlm_GraphiC) p, 0, title, maxsize);
    }
  }
}

static void Nlm_GetChoiceGroupTitle (Nlm_GraphiC c, Nlm_Int2 item,
                                     Nlm_CharPtr title, Nlm_sizeT maxsize)

{
  Nlm_MenU    m;
  Nlm_PrompT  p;

  m = (Nlm_MenU) Nlm_GetParent (c);
  p = Nlm_GetMenuPrompt (m);
  if (p != NULL) {
    Nlm_DoGetTitle ((Nlm_GraphiC) p, 0, title, maxsize);
  }
}

static void Nlm_GetChoiceItemTitle (Nlm_GraphiC i, Nlm_Int2 item,
                                    Nlm_CharPtr title, Nlm_sizeT maxsize)

{
  Nlm_ChoicE  c;
  Nlm_Int2    first;
  Nlm_Int2    index;
  Nlm_MenU    m;

  c = (Nlm_ChoicE) Nlm_GetParent (i);
  index = Nlm_GetFirstItem ((Nlm_IteM) i);
  first = Nlm_GetFirstItem ((Nlm_IteM) c);
  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) c);
  Nlm_GetMenuItem ((Nlm_GraphiC) m, first + index - 1, title, maxsize);
}

static void Nlm_GetPopListTitle (Nlm_GraphiC c, Nlm_Int2 item,
                                 Nlm_CharPtr title, Nlm_sizeT maxsize)

{
  Nlm_MenU       m;
  Nlm_Char       temp [256];
  Nlm_PopupTool  u;

  m = (Nlm_MenU) Nlm_GetParent (c);
#ifdef WIN_MAC
  Nlm_GetChoiceTitle (c, item, title, maxsize);
#endif
#ifdef WIN_MSWIN
  u = Nlm_GetMenuPopup (m);
  if (u != NULL) {
    if (ComboBox_GetLBTextLen (u, item - 1) < sizeof (temp)) {
      ComboBox_GetLBText (u, item - 1, temp);
    } else {
      temp [0] = '\0';
    }
    Nlm_StringNCpy (title, temp, maxsize);
  }
#endif
#ifdef WIN_MOTIF
#endif
}

static void Nlm_GetSubmenuTitle (Nlm_GraphiC m, Nlm_Int2 item,
                                 Nlm_CharPtr title, Nlm_sizeT maxsize)

{
#ifdef WIN_MAC
  Nlm_IteM  i;

  i = (Nlm_IteM) Nlm_GetParent (m);
  if (i != NULL) {
    Nlm_GetSingleTitle ((Nlm_GraphiC) i, item, title, maxsize);
  }
#endif
#ifdef WIN_MSWIN
  if (title != NULL && maxsize > 0) {
    title [0] = '\0';
  }
#endif
#ifdef WIN_MOTIF
  if (title != NULL && maxsize > 0) {
    title [0] = '\0';
  }
#endif
}

static void Nlm_SetStatValue (Nlm_GraphiC i, Nlm_Int2 item,
                              Nlm_Boolean value, Nlm_Boolean savePort)

{
  Nlm_Int2      itm;
  Nlm_ItemTool  itool;
  Nlm_MenU      m;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePortIfNeeded (i, savePort);
  m = (Nlm_MenU) Nlm_GetParent (i);
  itm = Nlm_GetFirstItem ((Nlm_IteM) i);
  itool = Nlm_GetItemHandle ((Nlm_IteM) i);
  Nlm_CheckMenuItem (m, itool, itm, value);
  Nlm_RestorePort (tempPort);
}

static void Nlm_SetChoiceValue (Nlm_GraphiC c, Nlm_Int2 value, Nlm_Boolean savePort)

{
  Nlm_Int2      first;
  Nlm_ItemTool  PNTR hdls;
  Nlm_Int2      index;
  Nlm_ItemTool  itool;
  Nlm_MenU      m;
  Nlm_Int2      num;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePortIfNeeded (c, savePort);
  first = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  m = (Nlm_MenU) Nlm_GetParent (c);
  index = 1;
  hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
  while (index <= num) {
    if (hdls != NULL) {
      itool = hdls [index - 1];
    } else {
      itool = NULL;
    }
    Nlm_CheckMenuItem (m, itool, first + index - 1, FALSE);
    index++;
  }
  if (value > 0 && value <= num) {
    if (hdls != NULL) {
      itool = hdls [value - 1];
    } else {
      itool = NULL;
    }
    Nlm_CheckMenuItem (m, itool, first + value - 1, TRUE);
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_SetChoiceGroupValue (Nlm_GraphiC c, Nlm_Int2 value, Nlm_Boolean savePort)

{
  Nlm_Int2      first;
  Nlm_ItemTool  PNTR hdls;
  Nlm_Int2      index;
  Nlm_ItemTool  itool;
  Nlm_MenU      m;
  Nlm_Int2      num;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePortIfNeeded (c, savePort);
  first = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  m = (Nlm_MenU) Nlm_GetParent (c);
  index = 1;
  hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
  while (index <= num) {
    if (hdls != NULL) {
      itool = hdls [index - 1];
    } else {
      itool = NULL;
    }
    Nlm_CheckMenuItem (m, itool, first + index - 1, FALSE);
    index++;
  }
  if (value > 0 && value <= num) {
    if (hdls != NULL) {
      itool = hdls [value - 1];
    } else {
      itool = NULL;
    }
    Nlm_CheckMenuItem (m, itool, first + value - 1, TRUE);
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_SetPopListValue (Nlm_GraphiC c, Nlm_Int2 value, Nlm_Boolean savePort)

{
  Nlm_MenU       m;
  Nlm_WindoW     tempPort;
  Nlm_PopupTool  u;
#ifdef WIN_MAC
  Nlm_RecT       r;
#endif
#ifdef WIN_MOTIF
  Nlm_ItemTool   PNTR hdls;
#endif

  tempPort = Nlm_SavePortIfNeeded (c, savePort);
  m = (Nlm_MenU) Nlm_GetParent (c);
#ifdef WIN_MAC
  Nlm_SetChoiceValue (c, value, FALSE);
  if (Nlm_GetVisible (c) && Nlm_GetAllParentsVisible (c)) {
    Nlm_GetRect ((Nlm_GraphiC) m, &r);
    Nlm_UpsetRect (&r, 1, 1, 2, 2);
    Nlm_InvalRect (&r);
  }
#endif
#ifdef WIN_MSWIN
  u = Nlm_GetMenuPopup (m);
  if (u != NULL) {
    if (value > 0) {
      ComboBox_SetCurSel (u, value - 1);
    } else {
      ComboBox_SetCurSel (u, -1);
    }
  }
#endif
#ifdef WIN_MOTIF
  Nlm_SetPopupValue ((Nlm_ChoicE) c, value);
  u = Nlm_GetMenuPopup (m);
  if (u != NULL) {
    hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
    if (value > 0) {
      XtVaSetValues (u, XmNmenuHistory, hdls [value - 1], NULL);
      XtUnmanageChild (hdls [63]);
    } else {
      XtVaSetValues (u, XmNmenuHistory, hdls [63], NULL);
      XtManageChild (hdls [63]);
    }
  }
#endif
  Nlm_RestorePort (tempPort);
}

static Nlm_Boolean Nlm_GetStatValue (Nlm_GraphiC i, Nlm_Int2 item)

{
  Nlm_Int2      itm;
  Nlm_ItemTool  itool;
  Nlm_MenU      m;

  m = (Nlm_MenU) Nlm_GetParent (i);
  itm = Nlm_GetFirstItem ((Nlm_IteM) i);
  itool = Nlm_GetItemHandle ((Nlm_IteM) i);
  return (Nlm_IsMenuItemChecked (m, itool, itm));
}

static Nlm_Int2 Nlm_GetChoiceValue (Nlm_GraphiC c)

{
  Nlm_Int2      first;
  Nlm_ItemTool  PNTR hdls;
  Nlm_Int2      itm;
  Nlm_ItemTool  itool;
  Nlm_MenU      m;
  Nlm_Int2      num;
  Nlm_Int2      rsult;

  rsult = 0;
  first = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  m = (Nlm_MenU) Nlm_GetParent (c);
  itm = 1;
  hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
  while (itm <= num && rsult == 0) {
    if (hdls != NULL) {
      itool = hdls [itm - 1];
    } else {
      itool = NULL;
    }
    if (Nlm_IsMenuItemChecked (m, itool, first + itm - 1)) {
      rsult = itm;
    }
    itm++;
  }
  return rsult;
}

static Nlm_Int2 Nlm_GetChoiceGroupValue (Nlm_GraphiC c)

{
  Nlm_Int2      first;
  Nlm_ItemTool  PNTR hdls;
  Nlm_Int2      itm;
  Nlm_ItemTool  itool;
  Nlm_MenU      m;
  Nlm_Int2      num;
  Nlm_Int2      rsult;

  rsult = 0;
  first = Nlm_GetFirstItem ((Nlm_IteM) c);
  num = Nlm_GetNumItems ((Nlm_ChoicE) c);
  m = (Nlm_MenU) Nlm_GetParent (c);
  itm = 1;
  hdls = Nlm_GetChoiceHandles ((Nlm_ChoicE) c);
  while (itm <= num && rsult == 0) {
    if (hdls != NULL) {
      itool = hdls [itm - 1];
    } else {
      itool = NULL;
    }
    if (Nlm_IsMenuItemChecked (m, itool, first + itm - 1)) {
      rsult = itm;
    }
    itm++;
  }
  return rsult;
}

static Nlm_Int2 Nlm_GetPopListValue (Nlm_GraphiC c)

{
  Nlm_MenU       m;
  Nlm_Int2       rsult;
  Nlm_PopupTool  u;

  rsult = 0;
#ifdef WIN_MAC
  rsult = Nlm_GetChoiceValue (c);
#endif
#ifdef WIN_MSWIN
  m = (Nlm_MenU) Nlm_GetParent (c);
  u = Nlm_GetMenuPopup (m);
  if (u != NULL) {
    rsult = (WORD) ComboBox_GetCurSel (u) + 1;
  }
#endif
#ifdef WIN_MOTIF
  rsult = Nlm_GetPopupValue ((Nlm_ChoicE) c);
#endif
  return rsult;
}


char * kMenuCommandFlag = "%$%"; /* dgg -- external global variable */

/* M.I */
static void Nlm_PrepareTitle (Nlm_CharPtr temp, Nlm_CharPtr title,
                              Nlm_sizeT siztemp, Nlm_Boolean isMenu)

{
  Nlm_Uint4     slash;
  Nlm_Uint4     pos;
  Nlm_sizeT        poslet;
  Nlm_Char      letter[2];
  Nlm_Boolean   found;


  Nlm_StringNCpy (temp, title, siztemp);

#ifdef DCLAP
	/* dgg -- distinguish titles that we want parsed for "/" from those to leave alone */
  if (!isMenu && StrNCmp(title, kMenuCommandFlag, 3) != 0)  
   	return;	
#endif

#ifdef WIN_MAC
  if (isMenu && Nlm_StrngPos (title, "/", 0, FALSE, &slash)) {
    temp [slash] = '\0';
  }
#endif
#ifdef WIN_MSWIN
  if (Nlm_StrngPos (title, "/", 0, FALSE, &slash)) {
    letter[0] = temp[ slash+1 ];
    letter[1] = '\0';

/*  Let's try to match the case...  */

    found = Nlm_StrngPos( title, letter, 0, TRUE,  &pos );
    found = found && pos < slash;
    if( !found ) {
      found = Nlm_StrngPos( title, letter, 0, FALSE, &pos ) ;
      }
    poslet = (Nlm_sizeT) pos;

    if( found && poslet < slash ) {
		/* temp := title[ 0..poslet-1 ] + '&' + title[ poslet..END ] */
      Nlm_StringNCpy( temp, title, poslet );
      temp[ poslet ] = '&';
      Nlm_StringNCpy( temp+poslet+1, title+poslet, siztemp-poslet-1 );
      temp[ slash+1 ] = '\0';

    } else {
      temp [slash] = '\0';
    }
    
#if  defined(DCLAP) && !defined(WIN16)
     /* dgg - use convention of "Ctrl+X" for "/X" command keys */
     /* damn - Ctrl-H = help conflicts w/ Delete & use of backspace */
    /* now set the accel data */
  switch (letter[0]) {
    
    case 'I':
    case 'i': /* tab */
    case 'M': 
    case 'm': /* cr */
    case 'h': /* bs */
    case 'H':
      aAccel.fVirt= FALT+FCONTROL; 
      aAccel.key= letter[0] & 0x1F; /* ! for Ctrl, must do ascii conversion */
      Nlm_StrCat( temp, "\tAltCtrl+");
      Nlm_StrCat( temp, letter);
      break;
      
    default:
      Nlm_StrCat( temp, "\tCtrl+");
      Nlm_StrCat( temp, letter);
      aAccel.fVirt= FCONTROL; /* << Ctrl conflicts w/ Ctrl-H == Delete ! */
      aAccel.key= letter[0] & 0x1F; /* ! for Ctrl, must do ascii conversion */
      break;
    }
	aAccel.cmd= nextMenuNum;
	gAccel[nAccel]= aAccel;
	if (nAccel < maxAccel) nAccel++;
#endif
    }
#endif
#ifdef WIN_MOTIF
  if (Nlm_StrngPos (title, "/", 0, FALSE, &slash)) {
    temp [slash] = '\0';
  }
#endif
}


#ifdef WIN_MOTIF
static void Nlm_MenuCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  Nlm_GraphiC  i;

  choiceWidget = w;
  i = (Nlm_GraphiC) client_data;
  Nlm_DoCallback (i);
}
#endif

#define COMMAND_ITEM  1
#define STATUS_ITEM   2
#define CHOICE_ITEM   3
#define POPUP_ITEM    4
#define CASCADE_ITEM  5

static Nlm_ItemTool Nlm_AppendItems (Nlm_MenU m, Nlm_IteM i, Nlm_CharPtr itemNames,
                                     Nlm_Int2 type, Nlm_Boolean savePort)

{
  Nlm_MenuTool  h;
  Nlm_ItemTool  rsult;
  Nlm_Char      temp [256];
  Nlm_WindoW    tempPort;
#ifdef WIN_MOTIF
  XmString      label;
  Cardinal      n;
  Arg           wargs [10];
#endif
  Nlm_Boolean	isCommandString;
  
  rsult = NULL;
  tempPort = Nlm_SavePortIfNeeded ((Nlm_GraphiC) m, savePort);
  h = Nlm_GetMenuHandle ((Nlm_MenU) m);
#ifdef DCLAP
  isCommandString= FALSE;
  if ( StrNCmp( itemNames, kMenuCommandFlag, 3 ) == 0) {
    itemNames += 3;
    isCommandString= TRUE;
    }
#endif
  Nlm_StringNCpy (temp, itemNames, sizeof (temp));
  if (temp [0] != '\0') {
#ifdef WIN_MAC
#ifdef DCLAP
	/* dgg fix for menu command char parsing... we dont' want to do that most of time !*/
  if ( isCommandString  ) {
 	Nlm_CtoPstr (temp);
	AppendMenu (h, (StringPtr) temp);
	}
  else {
	char dummy[10];
	short item ;
	StrCpy(dummy, "empty");
 	Nlm_CtoPstr (dummy);
	AppendMenu (h, (StringPtr) dummy);
    item= CountMItems(h);
    Nlm_CtoPstr (temp);
	SetItem( h, item, (StringPtr) temp);
	}
#else
    Nlm_CtoPstr (temp);
    AppendMenu (h, (StringPtr) temp);
#endif

#endif
#ifdef WIN_MSWIN
    AppendMenu (h, MF_ENABLED, nextMenuNum, itemNames);
    if (nextMenuNum < 32767) {
      nextMenuNum++;
    }
#endif
#ifdef WIN_MOTIF
    label = XmStringCreateSimple (temp);
    n = 0;
    XtSetArg (wargs [n], XmNlabelString, label); n++;
    XtSetArg (wargs [n], XmNfontList, Nlm_XfontList); n++;
    XtSetArg (wargs [n], XmNmarginHeight, 0); n++;
    XtSetArg (wargs [n], XmNmarginWidth, 0); n++;
    XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
    XtSetArg (wargs [n], XmNheight, Nlm_stdLineHeight+10); n++;
    XtSetArg (wargs [n], XmNrecomputeSize, FALSE); n++;
    switch (type) {
      case COMMAND_ITEM:
        rsult = XmCreatePushButton (h, (String) "", wargs, n);
        XtAddCallback (rsult, XmNactivateCallback, Nlm_MenuCallback, (XtPointer) i);
        break;
      case STATUS_ITEM:
        XtSetArg (wargs [n], XmNindicatorType, XmN_OF_MANY); n++;
        rsult = XmCreateToggleButton (h, (String) "", wargs, n);
        XtAddCallback (rsult, XmNvalueChangedCallback, Nlm_MenuCallback, (XtPointer) i);
        break;
      case CHOICE_ITEM:
        XtSetArg (wargs [n], XmNindicatorType, XmONE_OF_MANY); n++;
        rsult = XmCreateToggleButton (h, (String) "", wargs, n);
        XtAddCallback (rsult, XmNvalueChangedCallback, Nlm_MenuCallback, (XtPointer) i);
        break;
      case POPUP_ITEM:
        rsult = XmCreatePushButton (h, (String) "", wargs, n);
        XtAddCallback (rsult, XmNactivateCallback, Nlm_MenuCallback, (XtPointer) i);
        break;
      default:
        break;
    }
    XmStringFree (label);
#endif
  }
  Nlm_RestorePort (tempPort);
  return rsult;
}

#ifdef WIN_MAC
static void Nlm_AppendResources (Nlm_MenU m, ResType res, Nlm_Boolean savePort)

{
  Nlm_MenuTool  h;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePortIfNeeded ((Nlm_GraphiC) m, savePort);
  h = Nlm_GetMenuHandle (m);
  AddResMenu (h, res);
  Nlm_RestorePort (tempPort);
}
#endif

static Nlm_IteM Nlm_AppendChoice (Nlm_ChoicE c, Nlm_CharPtr title,
                                  Nlm_Boolean poplist, Nlm_Boolean savePort)

{
  Nlm_IteM      i;
  Nlm_ItemTool  itool;
  Nlm_MenU      m;
  Nlm_Int2      num;
  Nlm_RecT      r;
  Nlm_WindoW    tempPort;
  Nlm_Char      temp [256];
#ifdef WIN_MOTIF
  Nlm_ItemTool  *hdls;
#endif

  i = NULL;
  tempPort = Nlm_SavePortIfNeeded ((Nlm_GraphiC) c, savePort);
  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) c);
#ifdef DCLAP
  Nlm_PrepareTitle (temp, title, sizeof (temp), FALSE); /* dgg */
#endif
  if (poplist) {
    itool = Nlm_AppendItems (m, (Nlm_IteM) c, temp, POPUP_ITEM, FALSE);
  } else {
    itool = Nlm_AppendItems (m, (Nlm_IteM) c, temp, CHOICE_ITEM, FALSE);
    Nlm_LoadRect (&r, 0, 0, 0, 0);
    i = (Nlm_IteM) Nlm_CreateLink ((Nlm_GraphiC) c, &r, sizeof (Nlm_ItemRec), choiceItemProcs);
    if (i != NULL) {
      Nlm_SetVisible ((Nlm_GraphiC) i, TRUE);
    }
  }
  num = Nlm_GetNumItems (c);
  num++;
  Nlm_SetNumItems (c, num);
  Nlm_LoadItemData (i, NULL, num, NULL);
#ifdef WIN_MOTIF
  hdls = Nlm_GetChoiceHandles (c);
  if (hdls == NULL) {
    hdls = (Nlm_ItemTool *) Nlm_MemNew (64 * sizeof (Nlm_ItemTool));
    Nlm_SetChoiceHandles (c, hdls);
  }
  if (hdls != NULL && num > 0 && num < 64) {
    hdls [num - 1] = itool;
  }
  XtManageChild (itool);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (m))) {
      XtRealizeWidget (itool);
    }
  } else {
    XtRealizeWidget (itool);
  }
#endif
  Nlm_RestorePort (tempPort);
  return i;
}



#ifdef DCLAP
/* need to have fingers on the last appended popup item ! */
/* extern */ Nlm_IteM  gLastPopupItem;
#endif

static void Nlm_AppendPopList (Nlm_ChoicE c, Nlm_CharPtr title, Nlm_Boolean savePort)

{
  Nlm_MenU       m;
  Nlm_Int2       num;
  Nlm_RecT       r;
  Nlm_Int2       swid;
  Nlm_WindoW     tempPort;
  Nlm_PopupTool  u;
  Nlm_Int2       wid;
#ifdef WIN_MOTIF
  Nlm_Boolean    done;
  Nlm_ItemTool  *hdls;
  Nlm_ItemTool   itool;
  XmString       label;
#endif

  tempPort = Nlm_SavePortIfNeeded ((Nlm_GraphiC) c, savePort);
  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) c);
#ifdef DCLAP
  gLastPopupItem= NULL;
#endif

#ifdef WIN_MAC
#ifdef DCLAP
  gLastPopupItem= 
#endif
  Nlm_AppendChoice (c, title, TRUE, FALSE);
#endif

#ifdef WIN_MSWIN
  u = Nlm_GetMenuPopup (m);
  if (u != NULL) {
    ComboBox_AddString (u, title);
    num = Nlm_GetNumItems ((Nlm_ChoicE) c);
    num++;
    Nlm_SetNumItems (c, num);
  }
#endif

#ifdef WIN_MOTIF
  done = FALSE;
  hdls = Nlm_GetChoiceHandles (c);
  if (hdls != NULL) {
    num = Nlm_GetNumItems ((Nlm_ChoicE) c);
    num++;
    if (num > 0 && num < 64) {
      itool = hdls [num - 1];
      if (itool != NULL) {
        label = XmStringCreateSimple (title);
        XtVaSetValues (itool, XmNlabelString, label, NULL);
        XmStringFree (label);
        XtManageChild (itool);
        Nlm_SetNumItems ((Nlm_ChoicE) c, num);
        done = TRUE;
      }
    }
  }
  if (! done) {
#ifdef DCLAP
  	gLastPopupItem= 
#endif
    Nlm_AppendChoice (c, title, TRUE, FALSE);
  }
#endif

  Nlm_SelectFont (Nlm_systemFont);
  swid = Nlm_StringWidth (title);
  Nlm_GetRect ((Nlm_GraphiC) m, &r);
#ifdef WIN_MAC
  wid = r.right - r.left - 26;
  if (swid > wid) {
    r.right = r.left + swid + 26;
    Nlm_SetRect ((Nlm_GraphiC) m, &r);
    Nlm_SetRect ((Nlm_GraphiC) c, &r);
  }
  if (swid > wid) {
    if (Nlm_GetVisible ((Nlm_GraphiC) c) && Nlm_GetAllParentsVisible ((Nlm_GraphiC) c)) {
      Nlm_InsetRect (&r, -2, -2);
      Nlm_OffsetRect (&r, 1, 1);
      Nlm_InvalRect (&r);
    }
  }
#endif
#ifdef WIN_MSWIN
  wid = r.right - r.left - 26;
  if (swid > wid) {
    r.right = r.left + swid + 26;
    Nlm_SetRect ((Nlm_GraphiC) m, &r);
    Nlm_SetRect ((Nlm_GraphiC) c, &r);
  }
  u = Nlm_GetMenuPopup (m);
  if (u != NULL) {
    if (num > 10) {
      num = 10;
    }
    MoveWindow (u, r.left, r.top, r.right - r.left,
                r.bottom - r.top + (num + 1) * Nlm_stdLineHeight, TRUE);
    UpdateWindow (u);
  }
#endif
#ifdef WIN_MOTIF
  wid = r.right - r.left - 48;
  if (swid > wid) {
    r.right = r.left + swid + 48;
    Nlm_SetRect ((Nlm_GraphiC) m, &r);
    Nlm_SetRect ((Nlm_GraphiC) c, &r);
  }
  u = Nlm_GetMenuPopup (m);
  if (u != NULL) {
    XtVaSetValues (u,
                   XmNx, (Position) r.left,
                   XmNy, (Position) r.top,
                   XmNwidth, (Dimension) (r.right - r.left),
                   XmNheight, (Dimension) (r.bottom - r.top), 
                   NULL);
  }
#endif
  Nlm_GetRect ((Nlm_GraphiC) m, &r);
  Nlm_DoAdjustPrnt ((Nlm_GraphiC) m, &r, TRUE, FALSE);
  Nlm_RestorePort (tempPort);
}

#ifdef WIN_MAC
static void Nlm_InvalPopup (Nlm_GraphiC m)

{
  Nlm_RecT  r;

  if (Nlm_GetVisible (m) && Nlm_GetAllParentsVisible (m)) {
    Nlm_GetRect (m, &r);
    Nlm_InsetRect (&r, -1, -1);
    Nlm_InvalRect (&r);
  }
}
#endif

static void Nlm_SetPopupPosition (Nlm_GraphiC m, Nlm_RectPtr r, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_RecT    oldRect;
  Nlm_WindoW  tempPort;

  if (r != NULL) {
    Nlm_DoGetPosition (m, &oldRect);
    if (! Nlm_EqualRect (r, &oldRect)) {
      tempPort = Nlm_SavePortIfNeeded (m, savePort);
      Nlm_InvalPopup (m);
      Nlm_SetRect (m, r);
      Nlm_InvalPopup (m);
      Nlm_RestorePort (tempPort);
    }
  }
#endif
}

static void Nlm_GetPopupPosition (Nlm_GraphiC m, Nlm_RectPtr r)

{
#ifdef WIN_MAC
  if (r != NULL) {
    Nlm_GetRect (m, r);
  }
#endif
}

static void Nlm_SetPopListPosition (Nlm_GraphiC m, Nlm_RectPtr r, Nlm_Boolean savePort)

{
  Nlm_ChoicE     c;
  Nlm_Int2       num;
  Nlm_RecT       oldRect;
  Nlm_WindoW     tempPort;
  Nlm_PopupTool  u;

  if (r != NULL) {
    Nlm_DoGetPosition (m, &oldRect);
    if (! Nlm_EqualRect (r, &oldRect)) {
      tempPort = Nlm_SavePortIfNeeded (m, savePort);
#ifdef WIN_MAC
      Nlm_InvalPopup (m);
#endif
      Nlm_SetRect (m, r);
      c = (Nlm_ChoicE) Nlm_GetChild (m);
      Nlm_SetRect ((Nlm_GraphiC) c, r);
#ifdef WIN_MAC
      Nlm_InvalPopup (m);
#endif
#ifdef WIN_MSWIN
      u = Nlm_GetMenuPopup ((Nlm_MenU) m);
      if (u != NULL) {
        c = (Nlm_ChoicE) Nlm_GetChild (m);
        num = Nlm_GetNumItems (c);
        if (num > 10) {
          num = 10;
        }
        MoveWindow (u, r->left, r->top, r->right - r->left,
                    r->bottom - r->top + (num + 1) * Nlm_stdLineHeight, TRUE);
        UpdateWindow (u);
      }
#endif
#ifdef WIN_MOTIF
      u = Nlm_GetMenuPopup ((Nlm_MenU) m);
      XtVaSetValues (u,
                     XmNx, (Position) r->left,
                     XmNy, (Position) r->top,
                     XmNwidth, (Dimension) (r->right - r->left),
                     XmNheight, (Dimension) (r->bottom - r->top), 
                     NULL);
#endif
      Nlm_RestorePort (tempPort);
    }
  }
}

static void Nlm_GetPopListPosition (Nlm_GraphiC m, Nlm_RectPtr r)

{
  if (r != NULL) {
    Nlm_GetRect (m, r);
  }
}

static void Nlm_SetPopupChoicePosition (Nlm_GraphiC p, Nlm_RectPtr r, Nlm_Boolean savePort)

{
  Nlm_GraphiC  m;

  if (r != NULL) {
    m = Nlm_GetParent (p);
    Nlm_DoSetPosition (m, r, savePort);
  }
}

static void Nlm_GetPopupChoicePosition (Nlm_GraphiC p, Nlm_RectPtr r)

{
  Nlm_GraphiC  m;

  if (r != NULL) {
    m = Nlm_GetParent (p);
    Nlm_GetRect (m, r);
  }
}

#ifdef WIN_MSWIN
/* Message cracker functions */

static void MyCls_OnChar (HWND hwnd, UINT ch, int cRepeat)

{
  Nlm_PopuP  p;

  p = (Nlm_PopuP) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  handlechar = FALSE;
  if (ch == '\t') {
    Nlm_DoSendFocus ((Nlm_GraphiC) p, (Nlm_Char) ch);
  } else if (ch == '\n' || ch == '\r') {
    Nlm_DoSendFocus ((Nlm_GraphiC) p, (Nlm_Char) ch);
  } else {
    handlechar = TRUE;
  }
}

LRESULT CALLBACK EXPORT PopupProc (HWND hwnd, UINT message,
                                   WPARAM wParam, LPARAM lParam)

{
  Nlm_PopuP  p;
  LRESULT    rsult;
  HDC        tempHDC;
  HWND       tempHWnd;

  if (Nlm_VibrantDisabled ()) {
    return CallWindowProc (lpfnOldPopupProc, hwnd, message, wParam, lParam);
  }

  rsult = 0;
  tempHWnd = Nlm_currentHWnd;
  tempHDC = Nlm_currentHDC;
  p = (Nlm_PopuP) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  Nlm_theWindow = Nlm_GetParentWindow ((Nlm_GraphiC) p);
  Nlm_currentHWnd = GetParent (hwnd);
  Nlm_currentHDC = Nlm_ParentWindowPort ((Nlm_GraphiC) p);
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
        rsult = CallWindowProc (lpfnOldPopupProc, hwnd, message, wParam, lParam);
      }
      break;
    default:
      rsult = CallWindowProc (lpfnOldPopupProc, hwnd, message, wParam, lParam);
      break;
  }
  Nlm_currentHWnd = tempHWnd;
  Nlm_currentHDC = tempHDC;
  Nlm_currentWindowTool = tempHWnd;
  return rsult;
}
#endif

static void Nlm_NewMenuBar (Nlm_MenuBaR mb)

{
  Nlm_MenuTool    h;
  Nlm_PoinT       npt;
  Nlm_RecT        r;
  Nlm_WindowTool  wptr;
#ifdef WIN_MOTIF
  Cardinal        n;
  Nlm_MainTool    man;
  Arg             wargs [10];
#endif

  Nlm_GetRect ((Nlm_GraphiC) mb, &r);
  Nlm_LoadPt (&npt, r.left + 15, r.top);
  Nlm_LoadBoxData ((Nlm_BoX) mb, npt, npt, npt, npt.y, npt.x, 7, 7, 7, 7, 0, 0);
#ifdef WIN_MSWIN
  h = CreateMenu ();
  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) mb);
  SetMenu (wptr, h);
  Nlm_LoadMenuBarData (mb, h);
#endif
#ifdef WIN_MOTIF
  man = Nlm_ParentWindowMain ((Nlm_GraphiC) mb);
  n = 0;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  h = XmCreateMenuBar (man, (String) "", wargs, n);
  Nlm_LoadMenuBarData (mb, h);
#endif
}


#ifdef WIN_MAC
static void Nlm_NewDesktopMenu (Nlm_MenU m, Nlm_CharPtr title)

{
  Nlm_MenuTool  h;
  Nlm_Char      temp [256];

/* M.I */
  Nlm_PrepareTitle (temp, title, sizeof (temp), TRUE);
/* M.I */
  Nlm_CtoPstr (temp);
  h = NewMenu (nextMenuNum, (StringPtr) temp);
  Nlm_LoadMenuData (m, h, NULL, nextMenuNum, NULL);
  if (nextMenuNum < 32767) {
    nextMenuNum++;
  }
}
#endif

static void Nlm_NewPopup (Nlm_MenU m, Nlm_CharPtr title, Nlm_RectPtr r)

{
  Nlm_MenuTool   h;
  Nlm_MenuBaR    mb;
  Nlm_MenuTool   mbh;
  Nlm_PrompT     p;
  Nlm_PopupTool  pop;
  Nlm_Char       temp [256];
#ifdef WIN_MOTIF
  XmString       label;
  Cardinal       n;
  Arg            wargs [10];
#endif

  h = NULL;
  p = NULL;
  pop = NULL;
/* M.I */
  Nlm_PrepareTitle (temp, title, sizeof (temp), TRUE);
/* M.I */
#ifdef WIN_MAC
  h = NewMenu (nextMenuNum, (StringPtr) "");
  p = Nlm_DependentPrompt ((Nlm_GraphiC) m, r, temp, Nlm_systemFont, 'c');
#endif
#ifdef WIN_MSWIN
  p = NULL;
  mb = (Nlm_MenuBaR) Nlm_GetParent ((Nlm_GraphiC) m);
  mbh = Nlm_GetMenuBarHandle (mb);
  h = CreateMenu ();
  AppendMenu (mbh, MF_POPUP | MF_ENABLED, (UINT) h, temp);
#endif
#ifdef WIN_MOTIF
  p = NULL;
  mb = (Nlm_MenuBaR) Nlm_GetParent ((Nlm_GraphiC) m);
  mbh = Nlm_GetMenuBarHandle (mb);
  n = 0;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  h = XmCreatePulldownMenu (mbh, (String) "", wargs, n);
  label = XmStringCreateSimple (temp);
  n = 0;
  XtSetArg (wargs [n], XmNlabelString, label); n++;
  XtSetArg (wargs [n], XmNsubMenuId, h); n++;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  pop = XmCreateCascadeButton (mbh, (String) "", wargs, n);
  XmStringFree (label);
  XtManageChild (pop);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (m))) {
      XtRealizeWidget (pop);
      XtRealizeWidget (h);
    }
  } else {
    XtRealizeWidget (pop);
    XtRealizeWidget (h);
  }
#endif
  Nlm_LoadMenuData (m, h, p, nextMenuNum, pop);
  if (nextMenuNum < 32767) {
    nextMenuNum++;
  }
}

static void Nlm_NewFloatingPopup (Nlm_MenU m, Nlm_CharPtr title, Nlm_RectPtr mr)

{
  Nlm_PopupTool   c;
  Nlm_RecT        r;
  Nlm_WindowTool  wptr;

#ifdef WIN_MAC
  Nlm_NewPopup (m, title, mr);
#endif
#ifdef WIN_MSWIN
  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) m);
  c = CreateWindow ("Combobox", title,
                     WS_CHILD | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED,
                     r.left, r.top, r.right - r.left,
                     r.bottom - r.top, wptr, 0,
                     Nlm_currentHInst, NULL);
  if (c != NULL) {
    SetProp (c, (LPSTR) "Nlm_VibrantProp", (Nlm_HandleTool) m);
  }
  Nlm_LoadMenuData (m, NULL, NULL, nextMenuNum, c);
  if (nextMenuNum < 32767) {
    nextMenuNum++;
  }
#endif
#ifdef WIN_MOTIF
#endif
}

#ifdef WIN_MOTIF
static void Nlm_NewPopupList (Nlm_MenU m, Nlm_ChoicE c, Nlm_PupActnProc actn)

{
  Nlm_MenuTool    h;
  Nlm_ItemTool    PNTR hdls;
  Nlm_ItemTool    itool;
  Nlm_PopupTool   pop;
  Nlm_RecT        r;
  Nlm_WindowTool  wptr;
  Pixel           color;
  XmString        label;
  Widget          lbl;
  Cardinal        n;
  Arg             wargs [15];

  Nlm_GetRect ((Nlm_GraphiC) m, &r);
  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) m);
  n = 0;
  h = XmCreatePulldownMenu (wptr, (String) "", wargs, n);
  XtSetArg (wargs [n], XmNx, (Position) r.left); n++;
  XtSetArg (wargs [n], XmNy, (Position) r.top); n++;
  XtSetArg (wargs [n], XmNwidth, (Dimension) (r.right - r.left)); n++;
  XtSetArg (wargs [n], XmNheight, (Dimension) (r.bottom - r.top)); n++;
  XtSetArg (wargs [n], XmNmarginHeight, 0); n++;
  XtSetArg (wargs [n], XmNmarginWidth, 0); n++;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNhighlightThickness, 0); n++;
  XtSetArg (wargs [n], XmNsubMenuId, h); n++;
  pop = XmCreateOptionMenu (wptr, (String) "", wargs, n);
  lbl = XmOptionLabelGadget (pop);
  XtVaGetValues (lbl, XmNforeground, &color, 0);
  n = 0;
  label = XmStringCreateSimple ("");
  XtSetArg (wargs [n], XmNlabelString, label); n++;
  XtSetArg (wargs [n], XmNwidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNheight, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNmarginHeight, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNmarginWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNhighlightThickness, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNbackground, (Pixel) color); n++;
  XtSetValues (lbl, wargs, n);
  XmStringFree (label);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (m))) {
      XtRealizeWidget (pop);
      XtRealizeWidget (h);
    }
  } else {
    XtRealizeWidget (pop);
    XtRealizeWidget (h);
  }
  Nlm_LoadMenuData (m, h, NULL, nextMenuNum, pop);
  if (nextMenuNum < 32767) {
    nextMenuNum++;
  }
  n = 0;
  label = XmStringCreateSimple (" ");
  XtSetArg (wargs [n], XmNlabelString, label); n++;
  XtSetArg (wargs [n], XmNfontList, Nlm_XfontList); n++;
  XtSetArg (wargs [n], XmNmarginHeight, 0); n++;
  XtSetArg (wargs [n], XmNmarginWidth, 0); n++;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNheight, Nlm_stdLineHeight+10); n++;
  XtSetArg (wargs [n], XmNrecomputeSize, FALSE); n++;
  itool = XmCreatePushButton (h, (String) "", wargs, n);
  XtAddCallback (itool, XmNactivateCallback, Nlm_MenuCallback, (XtPointer) c);
  XmStringFree (label);
  hdls = (Nlm_ItemTool *) Nlm_MemNew (64 * sizeof (Nlm_ItemTool));
  hdls [63] = itool;
  XtManageChild (itool);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (m))) {
      XtRealizeWidget (itool);
    }
  } else {
    XtRealizeWidget (itool);
  }
  XtVaSetValues (pop, XmNmenuHistory, hdls [63], NULL);
  Nlm_LoadItemData ((Nlm_IteM) c, m, 1, pop);
  Nlm_LoadChoiceData (c, 0, TRUE, 0, hdls);
  Nlm_LoadAction ((Nlm_GraphiC) c, (Nlm_ActnProc) actn);
}
#endif

static void Nlm_NewPopListMenu (Nlm_MenU m)

{
  Nlm_PopupTool   c;
  Nlm_MenuTool    h;
  Nlm_RecT        r;
  Nlm_WindowTool  wptr;

#ifdef WIN_MAC
  h = NewMenu (nextMenuNum, (StringPtr) "");
  c = NULL;
  Nlm_LoadMenuData (m, h, NULL, nextMenuNum, c);
#endif
#ifdef WIN_MSWIN
  Nlm_GetRect ((Nlm_GraphiC) m, &r);
  h = NULL;
  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) m);
  c = CreateWindow ("Combobox", "", WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
                     r.left, r.top, r.right - r.left,
                     r.bottom - r.top, wptr, 0,
                     Nlm_currentHInst, NULL);
  if (c != NULL) {
    SetProp (c, (LPSTR) "Nlm_VibrantProp", (Nlm_HandleTool) m);
  }
  Nlm_LoadMenuData (m, h, NULL, nextMenuNum, c);
  if (lpfnNewPopupProc == NULL) {
    lpfnNewPopupProc = (WNDPROC) MakeProcInstance ((FARPROC) PopupProc, Nlm_currentHInst);
  }
  if (lpfnOldPopupProc == NULL) {
    lpfnOldPopupProc = (WNDPROC) GetWindowLong (c, GWL_WNDPROC);
  } else if (lpfnOldPopupProc != (WNDPROC) GetWindowLong (c, GWL_WNDPROC)) {
    Nlm_Message (MSG_ERROR, "PopupProc subclass error");
  }
  SetWindowLong (c, GWL_WNDPROC, (LONG) lpfnNewPopupProc);
#endif
  if (nextMenuNum < 32767) {
    nextMenuNum++;
  }
}

#ifdef WIN_MAC
static void Nlm_NewSubMenu (Nlm_MenU m)

{
  Nlm_MenuTool  h;

  h = NewMenu (nextMenuNum, (StringPtr) "");
  Nlm_LoadMenuData (m, h, NULL, nextMenuNum, NULL);
  if (nextMenuNum < 32767) {
    nextMenuNum++;
  }
}

static void Nlm_NewSubmenuItem (Nlm_IteM i, Nlm_MenU sub, Nlm_CharPtr title)

{
  Nlm_MenuTool  h;
  Nlm_Int2      index;
  Nlm_MenU      m;

  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) i);
  Nlm_AppendItems (m, i, title, CASCADE_ITEM, FALSE);
  index = Nlm_CountMenuItems ((Nlm_GraphiC) m);
  Nlm_LoadItemData (i, sub, index, NULL);
  h = Nlm_GetMenuHandle (sub);
  InsertMenu (h, -1);
}
#endif

#ifdef WIN_MSWIN
static void Nlm_NewSubMenuAndItem (Nlm_IteM i, Nlm_MenU sub, Nlm_CharPtr title)

{
  Nlm_MenuTool  h;
  Nlm_Int2      index;
  Nlm_MenU      m;
  Nlm_MenuTool  mh;

  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) i);
  mh = Nlm_GetMenuHandle (m);
  h = CreateMenu ();
  AppendMenu (mh, MF_POPUP | MF_ENABLED, (UINT) h, title);
  Nlm_LoadMenuData (sub, h, NULL, nextMenuNum, NULL);
  if (nextMenuNum < 32767) {
    nextMenuNum++;
  }
  index = Nlm_CountMenuItems ((Nlm_GraphiC) m);
  Nlm_LoadItemData (i, sub, index, NULL);
}
#endif

#ifdef WIN_MOTIF
static void Nlm_NewCascadingMenu (Nlm_MenU m, Nlm_IteM i,
                                  Nlm_MenU sub, Nlm_CharPtr title)

{
  Nlm_MenuTool   h;
  Nlm_MenuTool   mh;
  Nlm_PopupTool  pop;
  XmString       label;
  Cardinal       n;
  Arg            wargs [10];

  mh = Nlm_GetMenuHandle (m);
  n = 0;
  h = XmCreatePulldownMenu (mh, (String) "", wargs, n);
  label = XmStringCreateSimple (title);
  n = 0;
  XtSetArg (wargs [n], XmNlabelString, label); n++;
  XtSetArg (wargs [n], XmNfontList, Nlm_XfontList); n++;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNsubMenuId, h); n++;
  XtSetArg (wargs [n], XmNrecomputeSize, FALSE); n++;
  pop = XmCreateCascadeButton (mh, (String) "", wargs, n);
  XmStringFree (label);
  XtManageChild (pop);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (m))) {
      XtRealizeWidget (pop);
      XtRealizeWidget (h);
    }
  } else {
    XtRealizeWidget (pop);
    XtRealizeWidget (h);
  }
  Nlm_LoadMenuData (sub, h, NULL, nextMenuNum, pop);
  Nlm_LoadItemData (i, sub, 0, pop);
}
#endif

static void Nlm_NewComm (Nlm_IteM i, Nlm_CharPtr title, Nlm_ItmActnProc actn)

{
  Nlm_Int2      index;
  Nlm_ItemTool  itool;
  Nlm_MenU      m;
  Nlm_Char      temp [256];

  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) i);
/* M.I */
  Nlm_PrepareTitle (temp, title, sizeof (temp), FALSE);
/* M.I */
  itool = Nlm_AppendItems (m, i, temp, COMMAND_ITEM, FALSE);
  index = Nlm_CountMenuItems ((Nlm_GraphiC) m);
  Nlm_LoadItemData (i, NULL, index, itool);
  Nlm_LoadAction ((Nlm_GraphiC) i, (Nlm_ActnProc) actn);
#ifdef WIN_MOTIF
  XtManageChild (itool);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (m))) {
      XtRealizeWidget (itool);
    }
  } else {
    XtRealizeWidget (itool);
  }
#endif
}

static void Nlm_NewStat (Nlm_IteM i, Nlm_CharPtr title, Nlm_ItmActnProc actn)

{
  Nlm_Int2      index;
  Nlm_ItemTool  itool;
  Nlm_MenU      m;
  Nlm_Char      temp [256];

  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) i);
/* M.I */
  Nlm_PrepareTitle (temp, title, sizeof (temp), FALSE);
/* M.I */
  itool = Nlm_AppendItems (m, i, temp, STATUS_ITEM, FALSE);
  index = Nlm_CountMenuItems ((Nlm_GraphiC) m);
  Nlm_LoadItemData (i, NULL, index, itool);
  Nlm_LoadAction ((Nlm_GraphiC) i, (Nlm_ActnProc) actn);
#ifdef WIN_MOTIF
  XtManageChild (itool);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (m))) {
      XtRealizeWidget (itool);
    }
  } else {
    XtRealizeWidget (itool);
  }
#endif
}

static void Nlm_NewChoice (Nlm_ChoicE c, Nlm_ChsActnProc actn)

{
  Nlm_Int2  index;
  Nlm_MenU  m;

  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) c);
  index = Nlm_CountMenuItems ((Nlm_GraphiC) m);
  Nlm_LoadItemData ((Nlm_IteM) c, NULL, index + 1, NULL);
  Nlm_LoadChoiceData (c, 0, FALSE, 0, NULL);
  Nlm_LoadAction ((Nlm_GraphiC) c, (Nlm_ActnProc) actn);
}

static void Nlm_NewPopListChoice (Nlm_ChoicE c, Nlm_PupActnProc actn)

{
  Nlm_Int2  index;
  Nlm_MenU  sub;
#ifdef WIN_MAC
  Nlm_MenU  m;
#endif

  sub = NULL;
  index = 0;
#ifdef WIN_MAC
  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) c);
  index = Nlm_CountMenuItems ((Nlm_GraphiC) m);
#endif
#ifdef WIN_MSWIN
  index = 0;
#endif
  Nlm_LoadItemData ((Nlm_IteM) c, sub, index + 1, NULL);
  Nlm_LoadChoiceData (c, 0, TRUE, 0, NULL);
  Nlm_LoadAction ((Nlm_GraphiC) c, (Nlm_ActnProc) actn);
}

static void Nlm_NewDeskAcc (Nlm_ChoicE c)

{
#ifdef WIN_MAC
  Nlm_Int2  frst;
  Nlm_Int2  index;
  Nlm_MenU  m;

  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) c);
  index = Nlm_CountMenuItems ((Nlm_GraphiC) m);
  frst = index + 1;
  Nlm_AppendResources (m, 'DRVR', FALSE);
  index = Nlm_CountMenuItems ((Nlm_GraphiC) m);
  Nlm_LoadItemData ((Nlm_IteM) c, NULL, frst, NULL);
  Nlm_LoadChoiceData (c, index + 1 - frst, FALSE, 0, NULL);
#endif
}

static void Nlm_NewFontGroup (Nlm_ChoicE c)

{
#ifdef WIN_MAC
  Nlm_MenU  m;
  Nlm_Int2  newval;
  Nlm_Int2  oldval;

  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) c);
  oldval = Nlm_CountMenuItems ((Nlm_GraphiC) m);
  Nlm_AppendResources (m, 'FOND', FALSE);
  newval = Nlm_CountMenuItems ((Nlm_GraphiC) m);
  Nlm_LoadItemData ((Nlm_IteM) c, NULL, oldval + 1, NULL);
  Nlm_LoadChoiceData (c, newval - oldval, FALSE, 0, NULL);
#endif
#ifdef WIN_MSWIN
  /*
  WNDPROC   inst;
  */
  Nlm_MenU  m;
  Nlm_Int2  newval;
  Nlm_Int2  oldval;

  m = (Nlm_MenU) Nlm_GetParent ((Nlm_GraphiC) c);
  oldval = Nlm_CountMenuItems ((Nlm_GraphiC) m);
  /*
  inst = (WNDPROC) MakeProcInstance (EnumAllFaces, Nlm_currentHInst);
  FreeProcInstance (inst);
  */
  newval = Nlm_CountMenuItems ((Nlm_GraphiC) m);
  Nlm_LoadItemData ((Nlm_IteM) c, NULL, oldval + 1, NULL);
  Nlm_LoadChoiceData (c, newval - oldval, FALSE, 0, NULL);
#endif
#ifdef WIN_MOTIF
#endif
}

#ifdef WIN_MAC
static Nlm_MenuBaR Nlm_CreateDesktopMenuBar (Nlm_WindoW w)

{
  Nlm_MenuBaR  mb;
  Nlm_RecT     r;

  Nlm_LoadRect (&r, 0, 0, 0, 0);
  mb = (Nlm_MenuBaR) Nlm_HandNew (sizeof (Nlm_MenuBarRec));
  if (mb != NULL) {
    Nlm_LoadGraphicData ((Nlm_GraphiC) mb, NULL, (Nlm_GraphiC) w, NULL, NULL,
                         desktopMenuBarProcs, NULL, &r, TRUE, FALSE);
    if (Nlm_nextIdNumber < 32767) {
      Nlm_nextIdNumber++;
    }
    Nlm_SetWindowMenuBar (w, mb);
    Nlm_SetVisible ((Nlm_GraphiC) mb, TRUE);
  }
  return mb;
}
#endif

static Nlm_MenuBaR Nlm_CreateWindowMenuBar (Nlm_WindoW w)

{
  Nlm_MenuBaR  mb;
  Nlm_RecT     r;
#ifdef WIN_MAC
  Nlm_BoxData  bdata;
#endif

  Nlm_LoadRect (&r, 0, 0, 0, 20);
  mb = (Nlm_MenuBaR) Nlm_HandNew (sizeof (Nlm_MenuBarRec));
  if (mb != NULL) {
    Nlm_LoadGraphicData ((Nlm_GraphiC) mb, NULL, (Nlm_GraphiC) w, NULL, NULL,
                         windowMenuBarProcs, NULL, &r, TRUE, FALSE);
    if (Nlm_nextIdNumber < 32767) {
      Nlm_nextIdNumber++;
    }
  }
  if (mb != NULL) {
#ifdef WIN_MAC
    Nlm_DoAdjustPrnt ((Nlm_GraphiC) mb, &r, TRUE, FALSE);
    Nlm_GetBoxData ((Nlm_BoX) w, &bdata);
    bdata.topRow = bdata.limitPoint.y + bdata.yMargin;
    bdata.nextCol = bdata.resetPoint.x;
    bdata.nextPoint.y = bdata.topRow;
    bdata.nextPoint.x = bdata.nextCol;
    Nlm_SetBoxData ((Nlm_BoX) w, &bdata);
#endif
    Nlm_SetWindowMenuBar (w, mb);
    Nlm_NewMenuBar (mb);
    Nlm_DoShow ((Nlm_GraphiC) mb, TRUE, FALSE);
  }
  return mb;
}

extern Nlm_MenU Nlm_PulldownMenu (Nlm_WindoW w, Nlm_CharPtr title)

{
  Nlm_MenU     m;
  Nlm_MenuBaR  mb;
  Nlm_RecT     r;
  Nlm_WindoW   tempPort;
#ifdef WIN_MAC
  Nlm_PoinT    npt;
#endif
#ifdef WIN_MSWIN
  Nlm_PoinT    npt;
#endif

  m = NULL;
  if (w != NULL && title != NULL && w == Nlm_GetParentWindow ((Nlm_GraphiC) w)) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) w);
    Nlm_SelectFont (Nlm_systemFont);
    mb = Nlm_GetWindowMenuBar (w);
    if (mb == NULL) {
      mb = Nlm_CreateWindowMenuBar (w);
    }
#ifdef WIN_MAC
    Nlm_GetNextPosition ((Nlm_GraphiC) mb, &npt);
    r.top = npt.y;
    r.left = npt.x;
    r.bottom = r.top + 20;
    r.right = r.left + Nlm_StringWidth (title) + 5;
    Nlm_RecordRect ((Nlm_GraphiC) mb, &r);
    Nlm_Advance ((Nlm_GraphiC) mb);
#endif
#ifdef WIN_MSWIN
    Nlm_GetNextPosition ((Nlm_GraphiC) mb, &npt);
    r.top = npt.y;
    r.left = npt.x;
    r.bottom = r.top;
    r.right = r.left + Nlm_StringWidth (title) + 50;
    Nlm_DoAdjustPrnt ((Nlm_GraphiC) mb, &r, TRUE, FALSE);
    Nlm_Advance ((Nlm_GraphiC) mb);
#endif
#ifdef WIN_MOTIF
    Nlm_LoadRect (&r, 0, 0, 0, 0);
#endif
    m = (Nlm_MenU) Nlm_CreateLink ((Nlm_GraphiC) mb, &r, sizeof (Nlm_MenuRec),
                                   windowPulldownProcs);
    if (m != NULL) {
      r.top += 3;
      Nlm_NewPopup (m, title, &r);
      Nlm_DoShow ((Nlm_GraphiC) m, TRUE, FALSE);
    }
    Nlm_RestorePort (tempPort);
  } else if (title != NULL) {
#ifdef WIN_MAC
    w = Nlm_desktopWindow;
    mb = Nlm_GetWindowMenuBar (w);
    if (mb == NULL) {
      mb = Nlm_CreateDesktopMenuBar (w);
    }
    Nlm_LoadRect (&r, 0, 0, 0, 0);
    m = (Nlm_MenU) Nlm_CreateLink ((Nlm_GraphiC) mb, &r, sizeof (Nlm_MenuRec),
                                   desktopPulldownProcs);
    if (m != NULL) {
      Nlm_NewDesktopMenu (m, title);
      Nlm_DoShow ((Nlm_GraphiC) m, TRUE, TRUE);
    }
#endif
  }
  return m;
}

extern Nlm_MenU Nlm_AppleMenu (Nlm_WindoW w)

{
  Nlm_MenU  m;
#ifdef WIN_MAC
  Nlm_Char  appleStr [2] = {'\24', '\0'};
#endif

  m = NULL;
#ifdef WIN_MAC
  m = Nlm_PulldownMenu (w, appleStr);
#endif
  return m;
}

extern Nlm_MenU Nlm_PopupMenu (Nlm_GrouP prnt, Nlm_CharPtr title)

{
  Nlm_MenU    m;
  Nlm_PoinT   npt;
  Nlm_RecT    p;
  Nlm_RecT    r;
  Nlm_WindoW  tempPort;
  Nlm_Int2    wid;

  m = NULL;
  if (prnt != NULL && title != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) prnt);
    Nlm_GetNextPosition ((Nlm_GraphiC) prnt, &npt);
    Nlm_SelectFont (Nlm_systemFont);
    wid = Nlm_StringWidth (title);
    r.left = npt.x;
    r.top = npt.y;
    r.right = r.left + wid + 10;
    r.bottom = r.top + Nlm_stdLineHeight + 8;
    m = (Nlm_MenU) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r, sizeof (Nlm_MenuRec), popupProcs);
    if (m != NULL) {
      p = r;
      Nlm_UpsetRect (&p, 1, 1, 2, 2);
      Nlm_NewFloatingPopup (m, title, &p);
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) m, &r, TRUE, FALSE);
      Nlm_DoShow ((Nlm_GraphiC) m, TRUE, FALSE);
    }
    Nlm_RestorePort (tempPort);
  }
  return m;
}

extern Nlm_MenU Nlm_SubMenu (Nlm_MenU m, Nlm_CharPtr title)

/*
*  WARNING -- Sub menus on the Macintosh link into the desktop window menu
*  bar in addition to their own parent items, so removing windows with
*  sub menus in them is dangerous.
*/

{
  Nlm_IteM     i;
  Nlm_RecT     r;
  Nlm_MenU     sub;
  Nlm_Char     temp [256];
#ifdef WIN_MAC
  Nlm_Int2     len;
  Nlm_MenuBaR  mb;
  Nlm_WindoW   w;
  Nlm_Int2     tag;
#endif

  sub = NULL;
  if (m != NULL && title != NULL) {
#ifdef WIN_MAC
    w = Nlm_desktopWindow;
    mb = Nlm_GetWindowMenuBar (w);
    if (mb == NULL) {
      mb = Nlm_CreateDesktopMenuBar (w);
    }
    Nlm_LoadRect (&r, 0, 0, 0, 0);
    sub = (Nlm_MenU) Nlm_CreateLink ((Nlm_GraphiC) mb, &r, sizeof (Nlm_MenuRec), subMenuProcs);
    if (sub != NULL) {
      Nlm_NewSubMenu (sub);
      Nlm_SetVisible ((Nlm_GraphiC) sub, TRUE);
#ifdef DCLAP
      Nlm_StringNCpy (temp, kMenuCommandFlag, sizeof (temp));
      Nlm_StringNCat (temp, title, sizeof (temp));
#else
      Nlm_StringNCpy (temp, title, sizeof (temp));
#endif
      len = (Nlm_Int2) Nlm_StringLen (temp);
      tag = Nlm_GetMenuTag (sub);
      if (len < 250) {
        temp [len] = '/';
        temp [len + 1] = '\33';
        temp [len + 2] = '!';
        temp [len + 3] = (Nlm_Char) tag;
        temp [len + 4] = '\0';
      }
      i = (Nlm_IteM) Nlm_CreateLink ((Nlm_GraphiC) m, &r, sizeof (Nlm_ItemRec), subItemProcs);
      if (i != NULL) {
        Nlm_NewSubmenuItem (i, sub, temp);
        Nlm_SetParent ((Nlm_GraphiC) sub, (Nlm_GraphiC) i);
        Nlm_SetVisible ((Nlm_GraphiC) i, TRUE);
      }
    }
#endif
#ifdef WIN_MSWIN
    sub = (Nlm_MenU) Nlm_HandNew (sizeof (Nlm_MenuRec));
    if (sub != NULL) {
      Nlm_LoadRect (&r, 0, 0, 0, 0);
      Nlm_LoadGraphicData ((Nlm_GraphiC) sub, NULL, (Nlm_GraphiC) m, NULL, NULL,
                           subMenuProcs, NULL, &r, TRUE, FALSE);
      if (Nlm_nextIdNumber < 32767) {
        Nlm_nextIdNumber++;
      }
      Nlm_StringNCpy (temp, title, sizeof (temp));
      i = (Nlm_IteM) Nlm_CreateLink ((Nlm_GraphiC) m, &r, sizeof (Nlm_ItemRec), subItemProcs);
      if (i != NULL) {
        Nlm_NewSubMenuAndItem (i, sub, temp);
        Nlm_SetParent ((Nlm_GraphiC) sub, (Nlm_GraphiC) i);
        Nlm_SetVisible ((Nlm_GraphiC) sub, TRUE);
        Nlm_SetVisible ((Nlm_GraphiC) i, TRUE);
      }
    }
#endif
#ifdef WIN_MOTIF
    sub = (Nlm_MenU) Nlm_HandNew (sizeof (Nlm_MenuRec));
    if (sub != NULL) {
      Nlm_LoadRect (&r, 0, 0, 0, 0);
      Nlm_LoadGraphicData ((Nlm_GraphiC) sub, NULL, (Nlm_GraphiC) m, NULL, NULL,
                           subMenuProcs, NULL, &r, TRUE, FALSE);
      if (Nlm_nextIdNumber < 32767) {
        Nlm_nextIdNumber++;
      }
      Nlm_StringNCpy (temp, title, sizeof (temp));
      i = (Nlm_IteM) Nlm_CreateLink ((Nlm_GraphiC) m, &r, sizeof (Nlm_ItemRec), subItemProcs);
      if (i != NULL) {
        Nlm_NewCascadingMenu (m, i, sub, temp);
        Nlm_SetParent ((Nlm_GraphiC) sub, (Nlm_GraphiC) i);
        Nlm_SetVisible ((Nlm_GraphiC) sub, TRUE);
        Nlm_SetVisible ((Nlm_GraphiC) i, TRUE);
      }
    }
#endif
  }
  return sub;
}

extern Nlm_IteM Nlm_CommandItem (Nlm_MenU m, Nlm_CharPtr title, Nlm_ItmActnProc actn)

{
  Nlm_IteM  i;
  Nlm_RecT  r;

  i = NULL;
  if (m != NULL && title != NULL) {
    Nlm_LoadRect (&r, 0, 0, 0, 0);
    i = (Nlm_IteM) Nlm_CreateLink ((Nlm_GraphiC) m, &r, sizeof (Nlm_ItemRec), commandItemProcs);
    if (i != NULL) {
      Nlm_NewComm (i, title, actn);
      Nlm_SetVisible ((Nlm_GraphiC) i, TRUE);
    }
  }
  return i;
}

extern Nlm_IteM Nlm_StatusItem (Nlm_MenU m, Nlm_CharPtr title, Nlm_ItmActnProc actn)

{
  Nlm_IteM  i;
  Nlm_RecT  r;

  i = NULL;
  if (m != NULL && title != NULL) {
    Nlm_LoadRect (&r, 0, 0, 0, 0);
    i = (Nlm_IteM) Nlm_CreateLink ((Nlm_GraphiC) m, &r, sizeof (Nlm_ItemRec), statusItemProcs);
    if (i != NULL) {
      Nlm_NewStat (i, title, actn);
      Nlm_SetVisible ((Nlm_GraphiC) i, TRUE);
    }
  }
  return i;
}

extern void Nlm_SeparatorItem (Nlm_MenU m)

{
  Nlm_MenuTool  h;
#ifdef WIN_MAC
  Nlm_Char      temp [256];
#endif
#ifdef WIN_MOTIF
  Nlm_ItemTool  itool;
  Cardinal      n;
  Arg           wargs [10];
#endif

  if (m != NULL) {
    h = Nlm_GetMenuHandle (m);
#ifdef WIN_MAC
    Nlm_StringNCpy (temp, "(-", sizeof (temp));
    Nlm_CtoPstr (temp);
    AppendMenu (h, (StringPtr) temp);
#endif
#ifdef WIN_MSWIN
    AppendMenu (h, MF_SEPARATOR, 0, NULL);
#endif
#ifdef WIN_MOTIF
    n = 0;
    XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
    itool = XmCreateSeparator (h, (String) "", wargs, n);
    XtManageChild (itool);
    if (NLM_QUIET) {
      if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (m))) {
        XtRealizeWidget (itool);
      }
    } else {
      XtRealizeWidget (itool);
    }
#endif
  }
}

extern Nlm_ChoicE Nlm_ChoiceGroup (Nlm_MenU m, Nlm_ChsActnProc actn)

{
  Nlm_ChoicE  c;
  Nlm_RecT    r;

  c = NULL;
  if (m != NULL) {
    Nlm_LoadRect (&r, 0, 0, 0, 0);
    c = (Nlm_ChoicE) Nlm_CreateLink ((Nlm_GraphiC) m, &r, sizeof (Nlm_ChoiceRec),
                                     choiceGroupProcs);
    if (c != NULL) {
      Nlm_NewChoice (c, actn);
      Nlm_SetVisible ((Nlm_GraphiC) c, TRUE);
    }
  }
  return c;
}

extern Nlm_PopuP Nlm_PopupList (Nlm_GrouP prnt, Nlm_Boolean macLike,
                                Nlm_PupActnProc actn)

{
  Nlm_ChoicE   c;
  Nlm_GphPrcs  PNTR classPtr;
  Nlm_Int2     hbounds;
  Nlm_MenU     m;
  Nlm_PoinT    npt;
  Nlm_RecT     r;
  Nlm_WindoW   tempPort;
  Nlm_Int2     vbounds;

  c = NULL;
  if (prnt != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) prnt);
    Nlm_GetNextPosition ((Nlm_GraphiC) prnt, &npt);
    Nlm_SelectFont (Nlm_systemFont);
    if (macLike) {
      classPtr = macPopListProcs;
    } else {
      classPtr = msPopListProcs;
    }
#ifdef WIN_MAC
    hbounds = 13;
    vbounds = 2;
#endif
#ifdef WIN_MSWIN
    hbounds = 13;
    vbounds = 4;
#endif
#ifdef WIN_MOTIF
    hbounds = 24;
    vbounds = 5;
#endif
    r.left = npt.x;
    r.top = npt.y;
    r.right = r.left + hbounds * 2;
    r.bottom = r.top + Nlm_stdLineHeight + vbounds * 2;
#ifdef WIN_MAC
    m = (Nlm_MenU) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r, sizeof (Nlm_MenuRec), classPtr);
    if (m != NULL) {
      Nlm_NewPopListMenu (m);
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) m, &r, TRUE, FALSE);
      Nlm_DoShow ((Nlm_GraphiC) m, TRUE, FALSE);
      c = (Nlm_ChoicE) Nlm_CreateLink ((Nlm_GraphiC) m, &r, sizeof (Nlm_ChoiceRec),
                                       popUpChoiceProcs);
      if (c != NULL) {
        Nlm_NewPopListChoice (c, actn);
        Nlm_SetVisible ((Nlm_GraphiC) c, TRUE);
      }
    }
#endif
#ifdef WIN_MSWIN
    m = (Nlm_MenU) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r, sizeof (Nlm_MenuRec), classPtr);
    if (m != NULL) {
      Nlm_NewPopListMenu (m);
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) m, &r, TRUE, FALSE);
      Nlm_DoShow ((Nlm_GraphiC) m, TRUE, FALSE);
      c = (Nlm_ChoicE) Nlm_CreateLink ((Nlm_GraphiC) m, &r, sizeof (Nlm_ChoiceRec),
                                       popUpChoiceProcs);
      if (c != NULL) {
        Nlm_NewPopListChoice (c, actn);
        Nlm_SetVisible ((Nlm_GraphiC) c, TRUE);
      }
    }
#endif
#ifdef WIN_MOTIF
    m = (Nlm_MenU) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r, sizeof (Nlm_MenuRec), classPtr);
    if (m != NULL) {
      c = (Nlm_ChoicE) Nlm_CreateLink ((Nlm_GraphiC) m, &r, sizeof (Nlm_ChoiceRec),
                                       popUpChoiceProcs);
      if (c != NULL) {
        Nlm_NewPopupList (m, c, actn);
        Nlm_DoAdjustPrnt ((Nlm_GraphiC) m, &r, TRUE, FALSE);
        Nlm_SetVisible ((Nlm_GraphiC) c, TRUE);
        Nlm_DoShow ((Nlm_GraphiC) m, TRUE, FALSE);
      }
    }
#endif
    Nlm_RestorePort (tempPort);
  }
  return (Nlm_PopuP) c;
}

extern Nlm_IteM Nlm_ChoiceItem (Nlm_ChoicE c, Nlm_CharPtr title)

{
  Nlm_IteM  i;

  i = NULL;
  if (c != NULL && title != NULL) {
    if (! Nlm_IsItAPopupList (c)) {
      i = Nlm_AppendChoice (c, title, FALSE, FALSE);
    }
  }
  return i;
}

extern void Nlm_PopupItem (Nlm_PopuP p, Nlm_CharPtr title)

{
  Nlm_ChoicE  c;

  c = (Nlm_ChoicE) p;
  if (c != NULL && title != NULL) {
    if (Nlm_IsItAPopupList (c)) {
      Nlm_AppendPopList (c, title, TRUE);
    }
  }
}

extern void Nlm_DeskAccGroup (Nlm_MenU m)

{
#ifdef WIN_MAC
  Nlm_ChoicE  c;
  Nlm_RecT    r;

  c = NULL;
  if (m != NULL) {
    Nlm_LoadRect (&r, 0, 0, 0, 0);
    c = (Nlm_ChoicE) Nlm_CreateLink ((Nlm_GraphiC) m, &r, sizeof (Nlm_ChoiceRec), daProcs);
    if (c != NULL) {
      Nlm_NewDeskAcc (c);
      Nlm_SetVisible ((Nlm_GraphiC) c, TRUE);
    }
  }
#endif
}

extern Nlm_ChoicE Nlm_FontGroup (Nlm_MenU m)

{
  Nlm_ChoicE  c;
  Nlm_RecT    r;

  c = NULL;
  if (m != NULL) {
    Nlm_LoadRect (&r, 0, 0, 0, 0);
    c = (Nlm_ChoicE) Nlm_CreateLink ((Nlm_GraphiC) m, &r, sizeof (Nlm_ChoiceRec),
                                     choiceGroupProcs);
    if (c != NULL) {
      Nlm_NewFontGroup (c);
      Nlm_SetVisible ((Nlm_GraphiC) c, TRUE);
    }
  }
  return c;
}

extern void Nlm_FreeMenus (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemFree (gphprcsptr);
}

extern void Nlm_InitMenus (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemNew (sizeof (Nlm_GphPrcs) * 15);

#ifdef WIN_MAC
  desktopMenuBarProcs = &(gphprcsptr [0]);
  desktopMenuBarProcs->click = Nlm_DesktopMenuBarClick;
  desktopMenuBarProcs->key = Nlm_DesktopMenuBarKey;
  desktopMenuBarProcs->remove = Nlm_RemoveDesktopMenuBar;
  desktopMenuBarProcs->linkIn = Nlm_LinkIn;

  desktopPulldownProcs = &(gphprcsptr [1]);
  desktopPulldownProcs->click = Nlm_DesktopMenuClick;
  desktopPulldownProcs->key = Nlm_DesktopMenuKey;
  desktopPulldownProcs->draw = Nlm_DrawDesktopMenu;
  desktopPulldownProcs->show = Nlm_ShowDesktopMenu;
  desktopPulldownProcs->hide = Nlm_HideDesktopMenu;
  desktopPulldownProcs->enable = Nlm_EnableDesktopMenu;
  desktopPulldownProcs->disable = Nlm_DisableDesktopMenu;
  desktopPulldownProcs->remove = Nlm_RemoveDesktopMenu;
  desktopPulldownProcs->reset = Nlm_ResetMenu;
  desktopPulldownProcs->countItems = Nlm_CountMenuItems;
  desktopPulldownProcs->linkIn = Nlm_LinkIn;
#endif

  windowMenuBarProcs = &(gphprcsptr [2]);
#ifdef WIN_MAC
  windowMenuBarProcs->click = Nlm_WindowMenuBarClick;
  windowMenuBarProcs->key = Nlm_WindowMenuBarKey;
  windowMenuBarProcs->draw = Nlm_DrawWindowMenuBar;
#endif
#ifdef WIN_MSWIN
  windowMenuBarProcs->command = Nlm_WindowMenuBarCommand;
#endif
#ifdef WIN_MOTIF
#endif
  windowMenuBarProcs->show = Nlm_ShowWindowMenuBar;
  windowMenuBarProcs->remove = Nlm_RemoveWindowMenuBar;
  windowMenuBarProcs->linkIn = Nlm_LinkIn;

  windowPulldownProcs = &(gphprcsptr [3]);
#ifdef WIN_MAC
  windowPulldownProcs->click = Nlm_PulldownClick;
  windowPulldownProcs->key = Nlm_PopupKey;
  windowPulldownProcs->draw = Nlm_DrawWindowMenu;
#endif
#ifdef WIN_MSWIN
  windowPulldownProcs->command = Nlm_PulldownCommand;
#endif
#ifdef WIN_MOTIF
#endif
  windowPulldownProcs->show = Nlm_ShowPopup;
  windowPulldownProcs->hide = Nlm_HidePulldown;
  windowPulldownProcs->enable = Nlm_EnablePopup;
  windowPulldownProcs->disable = Nlm_DisablePopup;
  windowPulldownProcs->remove = Nlm_RemovePopupMenu;
  windowPulldownProcs->reset = Nlm_ResetMenu;
  windowPulldownProcs->countItems = Nlm_CountMenuItems;
  windowPulldownProcs->linkIn = Nlm_LinkIn;

  popupProcs = &(gphprcsptr [4]);
#ifdef WIN_MAC
  popupProcs->click = Nlm_PopupClick;
  popupProcs->key = Nlm_PopupKey;
  popupProcs->draw = Nlm_DrawPopup;
#endif
#ifdef WIN_MSWIN
#endif
#ifdef WIN_MOTIF
#endif
  popupProcs->show = Nlm_ShowPopup;
  popupProcs->hide = Nlm_HidePopup;
  popupProcs->enable = Nlm_EnablePopup;
  popupProcs->disable = Nlm_DisablePopup;
  popupProcs->remove = Nlm_RemovePopupMenu;
  popupProcs->reset = Nlm_ResetMenu;
  popupProcs->countItems = Nlm_CountMenuItems;
  popupProcs->linkIn = Nlm_LinkIn;
  popupProcs->setPosition = Nlm_SetPopupPosition;
  popupProcs->getPosition = Nlm_GetPopupPosition;

  subMenuProcs = &(gphprcsptr [5]);
#ifdef WIN_MAC
  subMenuProcs->click = Nlm_DesktopMenuClick;
  subMenuProcs->key = Nlm_SubMenuKey;
#endif
  subMenuProcs->enable = Nlm_EnableSubMenu;
  subMenuProcs->disable = Nlm_DisableSubMenu;
  subMenuProcs->remove = Nlm_RemoveSubMenu;
  subMenuProcs->reset = Nlm_ResetMenu;
  subMenuProcs->countItems = Nlm_CountMenuItems;
  subMenuProcs->linkIn = Nlm_LinkIn;
  subMenuProcs->setTitle = Nlm_SetSubmenuTitle;
  subMenuProcs->getTitle = Nlm_GetSubmenuTitle;

  subItemProcs = &(gphprcsptr [6]);
#ifdef WIN_MAC
  subItemProcs->click = Nlm_SubItemClick;
#endif
#ifdef WIN_MSWIN
  subItemProcs->command = Nlm_SubItemCommand;
#endif
#ifdef WIN_MOTIF
#endif
  subItemProcs->enable = Nlm_EnableMenuItem;
  subItemProcs->disable = Nlm_DisableMenuItem;
  subItemProcs->remove = Nlm_RemoveMenuItem;
  subItemProcs->linkIn = Nlm_LinkIn;
  subItemProcs->setTitle = Nlm_SetSingleTitle;
  subItemProcs->getTitle = Nlm_GetSingleTitle;

  commandItemProcs = &(gphprcsptr [7]);
#ifdef WIN_MAC
  commandItemProcs->click = Nlm_CommItemClick;
  commandItemProcs->key = Nlm_CommItemKey;
#endif
#ifdef WIN_MSWIN
  commandItemProcs->command = Nlm_CommItemCommand;
#endif
#ifdef WIN_MOTIF
  commandItemProcs->callback = Nlm_CommItemCallback;
#endif
  commandItemProcs->enable = Nlm_EnableMenuItem;
  commandItemProcs->disable = Nlm_DisableMenuItem;
  commandItemProcs->remove = Nlm_RemoveMenuItem;
  commandItemProcs->setTitle = Nlm_SetSingleTitle;
  commandItemProcs->getTitle = Nlm_GetSingleTitle;

  statusItemProcs = &(gphprcsptr [8]);
#ifdef WIN_MAC
  statusItemProcs->click = Nlm_StatItemClick;
  statusItemProcs->key = Nlm_StatItemKey;
#endif
#ifdef WIN_MSWIN
  statusItemProcs->command = Nlm_StatItemCommand;
#endif
#ifdef WIN_MOTIF
  statusItemProcs->callback = Nlm_StatItemCallback;
#endif
  statusItemProcs->enable = Nlm_EnableMenuItem;
  statusItemProcs->disable = Nlm_DisableMenuItem;
  statusItemProcs->remove = Nlm_RemoveMenuItem;
  statusItemProcs->setTitle = Nlm_SetSingleTitle;
  statusItemProcs->getTitle = Nlm_GetSingleTitle;
  statusItemProcs->setStatus = Nlm_SetStatValue;
  statusItemProcs->getStatus = Nlm_GetStatValue;

  choiceGroupProcs = &(gphprcsptr [9]);
#ifdef WIN_MAC
  choiceGroupProcs->click = Nlm_ChoiceGroupClick;
  choiceGroupProcs->key = Nlm_ChoiceGroupKey;
#endif
#ifdef WIN_MSWIN
  choiceGroupProcs->command = Nlm_ChoiceGroupCommand;
#endif
#ifdef WIN_MOTIF
  choiceGroupProcs->callback = Nlm_ChoiceGroupCallback;
#endif
  choiceGroupProcs->enable = Nlm_EnableChoiceGroup;
  choiceGroupProcs->disable = Nlm_DisableChoice;
  choiceGroupProcs->remove = Nlm_RemoveChoiceGroup;
  choiceGroupProcs->linkIn = Nlm_LinkIn;
  choiceGroupProcs->countItems = Nlm_CountChoiceItems;
  choiceGroupProcs->setTitle = Nlm_SetChoiceGroupTitle;
  choiceGroupProcs->getTitle = Nlm_GetChoiceGroupTitle;
  choiceGroupProcs->setValue = Nlm_SetChoiceGroupValue;
  choiceGroupProcs->getValue = Nlm_GetChoiceGroupValue;

  choiceItemProcs = &(gphprcsptr [10]);
  choiceItemProcs->enable = Nlm_EnableChoiceItem;
  choiceItemProcs->disable = Nlm_DisableChoiceItem;
  choiceItemProcs->remove = Nlm_RemoveChoiceItem;
  choiceItemProcs->setTitle = Nlm_SetChoiceItemTitle;
  choiceItemProcs->getTitle = Nlm_GetChoiceItemTitle;
  choiceItemProcs->setStatus = NULL;
  choiceItemProcs->getStatus = NULL;

  macPopListProcs = &(gphprcsptr [11]);
#ifdef WIN_MAC
  macPopListProcs->click = Nlm_MacPopListClick;
  macPopListProcs->key = Nlm_PopListKey;
  macPopListProcs->draw = Nlm_DrawPopList;
#endif
#ifdef WIN_MSWIN
  macPopListProcs->command = Nlm_MacPopListCommand;
#endif
  macPopListProcs->show = Nlm_ShowPopListMenu;
  macPopListProcs->hide = Nlm_HidePopListMenu;
  macPopListProcs->enable = Nlm_EnablePopList;
  macPopListProcs->disable = Nlm_DisablePopList;
  macPopListProcs->remove = Nlm_RemovePopListMenu;
  macPopListProcs->reset = Nlm_ResetMenu;
  macPopListProcs->countItems = Nlm_CountMenuItems;
  macPopListProcs->linkIn = Nlm_LinkIn;
  macPopListProcs->setPosition = Nlm_SetPopListPosition;
  macPopListProcs->getPosition = Nlm_GetPopListPosition;

  msPopListProcs = &(gphprcsptr [12]);
#ifdef WIN_MAC
  msPopListProcs->click = Nlm_MSPopListClick;
  msPopListProcs->key = Nlm_PopListKey;
  msPopListProcs->draw = Nlm_DrawPopList;
#endif
#ifdef WIN_MSWIN
  msPopListProcs->command = Nlm_MSPopListCommand;
#endif
  msPopListProcs->show = Nlm_ShowPopListMenu;
  msPopListProcs->hide = Nlm_HidePopListMenu;
  msPopListProcs->enable = Nlm_EnablePopList;
  msPopListProcs->disable = Nlm_DisablePopList;
  msPopListProcs->remove = Nlm_RemovePopListMenu;
  msPopListProcs->reset = Nlm_ResetMenu;
  msPopListProcs->countItems = Nlm_CountMenuItems;
  msPopListProcs->linkIn = Nlm_LinkIn;
  msPopListProcs->setPosition = Nlm_SetPopListPosition;
  msPopListProcs->getPosition = Nlm_GetPopListPosition;

  popUpChoiceProcs = &(gphprcsptr [13]);
#ifdef WIN_MAC
  popUpChoiceProcs->click = Nlm_PopupChoiceGroupClick;
  popUpChoiceProcs->key = Nlm_ChoiceGroupKey;
#endif
#ifdef WIN_MOTIF
  popUpChoiceProcs->callback = Nlm_PopupChoiceCallback;
#endif
  popUpChoiceProcs->show = Nlm_ShowPopListChoice;
  popUpChoiceProcs->hide = Nlm_HidePopListChoice;
  popUpChoiceProcs->enable = Nlm_EnableChoice;
  popUpChoiceProcs->disable = Nlm_DisableChoice;
  popUpChoiceProcs->remove = Nlm_RemovePopupList;
  popUpChoiceProcs->reset = Nlm_ResetPopList;
  popUpChoiceProcs->countItems = Nlm_CountChoiceItems;
  popUpChoiceProcs->setTitle = Nlm_SetPopListTitle;
  popUpChoiceProcs->getTitle = Nlm_GetPopListTitle;
  popUpChoiceProcs->setValue = Nlm_SetPopListValue;
  popUpChoiceProcs->getValue = Nlm_GetPopListValue;
  popUpChoiceProcs->setPosition = Nlm_SetPopupChoicePosition;
  popUpChoiceProcs->getPosition = Nlm_GetPopupChoicePosition;

  daProcs = &(gphprcsptr [14]);
#ifdef WIN_MAC
  daProcs->click = Nlm_DAClick;
  daProcs->key = Nlm_DAKey;
#endif
  daProcs->enable = Nlm_EnableChoice;
  daProcs->disable = Nlm_DisableChoice;
  daProcs->remove = Nlm_RemoveMenuItem;
  daProcs->countItems = Nlm_CountChoiceItems;
  daProcs->setTitle = Nlm_SetChoiceTitle;
  daProcs->getTitle = Nlm_GetChoiceTitle;
  daProcs->setValue = Nlm_SetChoiceValue;
  daProcs->getValue = Nlm_GetChoiceValue;
}
