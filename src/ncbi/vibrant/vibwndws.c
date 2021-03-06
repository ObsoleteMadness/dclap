/*   vibwndws.c
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
* File Name:  vibwndws.c
*
* Author:  Jonathan Kans
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.45 $
*
* File Description: 
*       Vibrant main, event loop, and window functions
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
#include <Resources.h>
#include <Scrap.h>
#endif

#ifdef WIN_MAC
#include <Resources.h>
#ifndef Nlm_WindowTool
#define Nlm_WindowTool  WindowPtr
#endif
#ifndef Nlm_PortTool
#define Nlm_PortTool    GrafPtr
#endif
#ifndef Nlm_ShellTool
#define Nlm_ShellTool   Nlm_Handle
#endif
#ifndef Nlm_MainTool
#define Nlm_MainTool    Nlm_Handle
#endif
#endif

#ifdef WIN_MSWIN
#ifndef Nlm_WindowTool
#define Nlm_WindowTool  HWND
#endif
#ifndef Nlm_PortTool
#define Nlm_PortTool    HDC
#endif
#ifndef Nlm_ShellTool
#define Nlm_ShellTool   Nlm_Handle
#endif
#ifndef Nlm_MainTool
#define Nlm_MainTool    Nlm_Handle
#endif
#endif

#ifdef WIN_MOTIF
#include <Xm/DialogS.h>
#ifndef Nlm_WindowTool
#define Nlm_WindowTool  Widget
#endif
#ifndef Nlm_PortTool
#define Nlm_PortTool    GC
#endif
#ifndef Nlm_ShellTool
#define Nlm_ShellTool   Widget
#endif
#ifndef Nlm_MainTool
#define Nlm_MainTool    Widget
#endif
#endif

typedef  struct  Nlm_windowdata {
  Nlm_WindowTool   handle;
  Nlm_ShellTool    shell;
  Nlm_PortTool     port;
  Nlm_MainTool     main;
  Nlm_RecT         dragArea;
  Nlm_RecT         growArea;
  Nlm_RecT         zoomArea;
  Nlm_WndActnProc  close;
  Nlm_WndActnProc  activate;
  Nlm_WndActnProc  deactivate;
  Nlm_WndActnProc  resize;
  Nlm_DisplaY      charDisplay;
  Nlm_MenuBaR      menuBar;
  Nlm_ButtoN       defaultButton;
  Nlm_Boolean      notYetShown;
  Nlm_Boolean      dying;
  Nlm_CharPtr      title;
  Nlm_VoidPtr      data;
  Nlm_WndFreeProc  cleanup;
} Nlm_WindowData;

typedef  struct  Nlm_windowrec {
  Nlm_BoxRec      boxR;
  Nlm_WindowData  window;
} Nlm_WindowRec, PNTR Nlm_WndPtr;

Nlm_WindoW   Nlm_desktopWindow;
Nlm_WindoW   Nlm_systemWindow;

Nlm_RecT     Nlm_screenRect;

Nlm_WindoW   Nlm_theWindow;

#ifdef WIN_MAC
EventRecord  Nlm_currentEvent;
#endif

#ifdef WIN_MSWIN
WPARAM       Nlm_currentWParam;
HINSTANCE    Nlm_currentHInst;
LPARAM       Nlm_currentLParam;
MSG          Nlm_currentMssg;
int          Nlm_currentId;
HWND         Nlm_currentHwndCtl;
UINT         Nlm_currentCode;
int          Nlm_currentPos;
#endif

#ifdef WIN_MOTIF
XEvent        Nlm_currentEvent;
XtAppContext  Nlm_appContext;
XmFontList    Nlm_XfontList;
Widget        Nlm_fileDialogShell;

int           argc;
char          **argv;
#endif

Nlm_WindowTool  Nlm_currentWindowTool;

static Nlm_GphPrcsPtr  gphprcsptr = NULL;

static Nlm_GphPrcsPtr  documentProcs;
static Nlm_GphPrcsPtr  fixedProcs;
static Nlm_GphPrcsPtr  frozenProcs;
static Nlm_GphPrcsPtr  roundProcs;
static Nlm_GphPrcsPtr  alertProcs;
static Nlm_GphPrcsPtr  modalProcs;
static Nlm_GphPrcsPtr  floatingProcs;
static Nlm_GphPrcsPtr  shadowProcs;
static Nlm_GphPrcsPtr  plainProcs;
static Nlm_GphPrcsPtr  desktopProcs;
static Nlm_GphPrcsPtr  systemProcs;

static Nlm_Boolean     quitProgram;
static Nlm_Boolean     getArgsBoxUp;
static Nlm_Boolean     getArgsOk;

static Nlm_WindoW      recentWindow = NULL;
static Nlm_WindowData  recentWindowData;

static Nlm_WindoW      chosenWindow;
static Nlm_WindoW      frontWindow;
static Nlm_WindoW      theActiveWindow;

static Nlm_WindoW      dyingWindow;
static Nlm_RecT        screenBitBounds;
static Nlm_Boolean     inNotice;

static Nlm_VoidProc    timerAction;
static Nlm_KeyProc     keyAction;

#ifdef WIN_MAC
static Cursor       cross;
static Cursor       iBeam;
static Cursor       plus;
static Cursor       watch;
static Nlm_Int4     lastTimerTime;
static Nlm_Boolean  hasColorQD = FALSE;
static Str255       apName;
static Handle       apParam;
static short        apRefNum;
#endif

#ifdef WIN_MSWIN
static HCURSOR       currentCursor;
static UINT          timerID;
static Nlm_Char      windowclass [32];
static int           discard_count = 0;
static LRESULT       mainwndrsult;
static Nlm_Boolean   handlechar;
#endif

#ifdef WIN_MOTIF
static Cursor        currentCursor;
static Cursor        arrow;
static Cursor        cross;
static Cursor        iBeam;
static Cursor        plus;
static Cursor        watch;
static XFontStruct   *font;
static XtIntervalId  windowTimer;
static GC            firstGC;
static int           statargc;
static char          **statargv;
#endif

static void Nlm_LoadWindowData (Nlm_WindoW w, Nlm_WindowTool hdl,
                                Nlm_ShellTool shl, Nlm_PortTool prt,
                                Nlm_MainTool man, Nlm_RectPtr drag,
                                Nlm_RectPtr grow, Nlm_RectPtr zoom,
                                Nlm_WndActnProc closeProc, Nlm_WndActnProc actProc,
                                Nlm_WndActnProc deactProc, Nlm_WndActnProc resiz,
                                Nlm_DisplaY charDisp, Nlm_MenuBaR mnuBar,
                                Nlm_ButtoN dfltBtn, Nlm_Boolean notYet,
                                Nlm_Boolean die, Nlm_CharPtr ttl,
                                Nlm_VoidPtr dat, Nlm_WndFreeProc cln)

{
  Nlm_WindowData  PNTR wdptr;
  Nlm_WndPtr      wp;

  if (w != NULL) {
    wp = (Nlm_WndPtr) Nlm_HandLock (w);
    wdptr = &(wp->window);
    wdptr->handle = hdl;
    wdptr->shell = shl;
    wdptr->port = prt;
    wdptr->main = man;
    if (drag != NULL) {
      wdptr->dragArea = *drag;
    } else {
      Nlm_LoadRect (&(wdptr->dragArea), 0, 0, 0, 0);
    }
    if (grow != NULL) {
      wdptr->growArea = *grow;
    } else {
      Nlm_LoadRect (&(wdptr->growArea), 0, 0, 0, 0);
    }
    if (zoom != NULL) {
      wdptr->zoomArea = *zoom;
    } else {
      Nlm_LoadRect (&(wdptr->zoomArea), 0, 0, 0, 0);
    }
    wdptr->close = closeProc;
    wdptr->activate = actProc;
    wdptr->deactivate = deactProc;
    wdptr->resize = resiz;
    wdptr->charDisplay = charDisp;
    wdptr->menuBar = mnuBar;
    wdptr->defaultButton = dfltBtn;
    wdptr->notYetShown = notYet;
    wdptr->dying = die;
    wdptr->title = ttl;
    wdptr->data = dat;
    wdptr->cleanup = cln;
    Nlm_HandUnlock (w);
    recentWindow = NULL;
  }
}

static void Nlm_SetWindowData (Nlm_WindoW w, Nlm_WindowData * wdata)

{
  Nlm_WndPtr  wp;

  if (w != NULL && wdata != NULL) {
    wp = (Nlm_WndPtr) Nlm_HandLock (w);
    wp->window = *wdata;
    Nlm_HandUnlock (w);
    recentWindow = w;
    recentWindowData = *wdata;
  }
}

static void Nlm_GetWindowData (Nlm_WindoW w, Nlm_WindowData * wdata)

{
  Nlm_WndPtr  wp;

  if (w != NULL && wdata != NULL) {
    if (w == recentWindow && NLM_RISKY) {
      *wdata = recentWindowData;
    } else {
      wp = (Nlm_WndPtr) Nlm_HandLock (w);
      *wdata = wp->window;
      Nlm_HandUnlock (w);
      recentWindow = w;
      recentWindowData = *wdata;
    }
  }
}

static Nlm_WindowTool Nlm_GetWindowPtr (Nlm_WindoW w)

{
  Nlm_WindowTool  rsult;
  Nlm_WindowData  wdata;

  rsult = NULL;
  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    rsult = wdata.handle;
  }
  return rsult;
}

extern Nlm_ShellTool Nlm_GetWindowShell (Nlm_WindoW w)

{
  Nlm_ShellTool   rsult;
  Nlm_WindowData  wdata;

  rsult = NULL;
  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    rsult = wdata.shell;
  }
  return rsult;
}

static Nlm_PortTool Nlm_GetWindowPort (Nlm_WindoW w)

{
  Nlm_PortTool    rsult;
  Nlm_WindowData  wdata;

  rsult = NULL;
  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    rsult = wdata.port;
  }
  return rsult;
}

static Nlm_MainTool Nlm_GetWindowMain (Nlm_WindoW w)

{
  Nlm_MainTool    rsult;
  Nlm_WindowData  wdata;

  rsult = NULL;
  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    rsult = wdata.main;
  }
  return rsult;
}

extern void Nlm_SetWindowCharDisplay (Nlm_WindoW w, Nlm_DisplaY d)

{
  Nlm_WindowData  wdata;

  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    wdata.charDisplay = d;
    Nlm_SetWindowData (w, &wdata);
  }
}

extern Nlm_DisplaY Nlm_GetWindowCharDisplay (Nlm_WindoW w)

{
  Nlm_DisplaY     rsult;
  Nlm_WindowData  wdata;

  rsult = NULL;
  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    rsult = wdata.charDisplay;
  }
  return rsult;
}

extern void Nlm_SetWindowMenuBar (Nlm_WindoW w, Nlm_MenuBaR mb)

{
  Nlm_WindowData  wdata;

  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    wdata.menuBar = mb;
    Nlm_SetWindowData (w, &wdata);
  }
}

extern Nlm_MenuBaR Nlm_GetWindowMenuBar (Nlm_WindoW w)

{
  Nlm_MenuBaR     rsult;
  Nlm_WindowData  wdata;

  rsult = NULL;
  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    rsult = wdata.menuBar;
  }
  return rsult;
}

extern void Nlm_SetWindowDefaultButton (Nlm_WindoW w, Nlm_ButtoN b)

{
  Nlm_WindowData  wdata;

  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    wdata.defaultButton = b;
    Nlm_SetWindowData (w, &wdata);
  }
}

extern Nlm_ButtoN Nlm_GetWindowDefaultButton (Nlm_WindoW w)

{
  Nlm_ButtoN      rsult;
  Nlm_WindowData  wdata;

  rsult = NULL;
  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    rsult = wdata.defaultButton;
  }
  return rsult;
}

static void Nlm_SetNotYetShown (Nlm_WindoW w, Nlm_Boolean notYet)

{
  Nlm_WindowData  wdata;

  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    wdata.notYetShown = notYet;
    Nlm_SetWindowData (w, &wdata);
  }
}

extern Nlm_Boolean Nlm_WindowHasBeenShown (Nlm_WindoW w)

{
  Nlm_Boolean     rsult;
  Nlm_WindowData  wdata;

  rsult = FALSE;
  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    rsult = (Nlm_Boolean) (! wdata.notYetShown);
  }
  return rsult;
}

static void Nlm_SetWindowDying (Nlm_WindoW w, Nlm_Boolean die)

{
  Nlm_WindowData  wdata;

  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    wdata.dying = die;
    Nlm_SetWindowData (w, &wdata);
  }
}

extern Nlm_Boolean Nlm_IsWindowDying (Nlm_WindoW w)

{
  Nlm_Boolean     rsult;
  Nlm_WindowData  wdata;

  rsult = FALSE;
  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    rsult = wdata.dying;
  }
  return rsult;
}

extern Nlm_WindoW Nlm_GetParentWindow (Nlm_GraphiC a)

{
  Nlm_GraphiC  g;
  Nlm_GraphiC  p;

  g = a;
  if (a != NULL) {
    p = Nlm_GetParent (g);
    while (p != NULL) {
      g = p;
      p = Nlm_GetParent (g);
    }
  }
  return (Nlm_WindoW) g;
}

extern Nlm_WindoW Nlm_ParentWindow (Nlm_Handle a)

{
  return Nlm_GetParentWindow ((Nlm_GraphiC) a);
}

extern Nlm_WindowTool Nlm_ParentWindowPtr (Nlm_GraphiC a)

{
  Nlm_WindowTool  rsult;
  Nlm_WindoW      w;

  rsult = NULL;
  if (a != NULL) {
    w = Nlm_GetParentWindow (a);
    rsult = Nlm_GetWindowPtr (w);
  }
  return rsult;
}

extern Nlm_PortTool Nlm_ParentWindowPort (Nlm_GraphiC a)

{
  Nlm_PortTool  rsult;
  Nlm_WindoW    w;

  rsult = NULL;
  if (a != NULL) {
    w = Nlm_GetParentWindow (a);
    rsult = Nlm_GetWindowPort (w);
  }
  return rsult;
}

extern Nlm_MainTool Nlm_ParentWindowMain (Nlm_GraphiC a)

{
  Nlm_MainTool  rsult;
  Nlm_WindoW    w;

  rsult = NULL;
  if (a != NULL) {
    w = Nlm_GetParentWindow (a);
    rsult = Nlm_GetWindowMain (w);
  }
  return rsult;
}

extern Nlm_ShellTool Nlm_ParentWindowShell (Nlm_GraphiC a)

{
  Nlm_ShellTool  rsult;
  Nlm_WindoW     w;

  rsult = NULL;
  if (a != NULL) {
    w = Nlm_GetParentWindow (a);
    rsult = Nlm_GetWindowShell (w);
  }
  return rsult;
}

extern Nlm_WindoW Nlm_SavePortIfNeeded (Nlm_GraphiC a, Nlm_Boolean savePort)

{
  Nlm_WindoW  newwindow;
  Nlm_WindoW  oldwindow;
  Nlm_WindoW  rsult;

  rsult = NULL;
  if (a != NULL && savePort) {
    savePort = Nlm_GetAllParentsVisible (a);
    if (savePort) {
      oldwindow = Nlm_CurrentWindow ();
      newwindow = Nlm_GetParentWindow (a);
      savePort = (Nlm_Boolean) (oldwindow != newwindow);
      if (savePort) {
        Nlm_UseWindow (newwindow);
        rsult = oldwindow;
      }
    }
  }
  return rsult;
}

extern Nlm_WindoW Nlm_SavePort (Nlm_Handle a)

{
  return Nlm_SavePortIfNeeded ((Nlm_GraphiC) a, TRUE);
}

extern void Nlm_RestorePort (Nlm_WindoW w)

{
  Nlm_Update ();
  if (w != NULL) {
    Nlm_UseWindow (w);
  }
}

extern void Nlm_SetClose (Nlm_WindoW w, Nlm_WndActnProc cls)

{
  Nlm_WindowData  wdata;

  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    wdata.close = cls;
    Nlm_SetWindowData (w, &wdata);
  }
}

extern void Nlm_SetActivate (Nlm_WindoW w, Nlm_WndActnProc act)

{
  Nlm_WindowData  wdata;

  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    wdata.activate = act;
    Nlm_SetWindowData (w, &wdata);
  }
}

extern void Nlm_SetDeactivate (Nlm_WindoW w, Nlm_WndActnProc deact)

{
  Nlm_WindowData  wdata;

  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    wdata.deactivate = deact;
    Nlm_SetWindowData (w, &wdata);
  }
}

extern void Nlm_SetResize (Nlm_WindoW w, Nlm_WndActnProc resiz)

{
  Nlm_WindowData  wdata;

  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    wdata.resize = resiz;
    Nlm_SetWindowData (w, &wdata);
  }
}

extern void Nlm_SetWindowExtra (Nlm_WindoW w, Nlm_VoidPtr data, Nlm_WndFreeProc cleanup)

{
  Nlm_WindowData  wdata;

  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    wdata.data = data;
    wdata.cleanup = cleanup;
    Nlm_SetWindowData (w, &wdata);
  }
}

extern Nlm_VoidPtr Nlm_GetWindowExtra (Nlm_WindoW w)

{
  Nlm_WindowData  wdata;

  if (w != NULL) {
    Nlm_GetWindowData (w, &wdata);
    return wdata.data;
  } else {
    return NULL;
  }
}

static void Nlm_ClearKeys (void)

{
  Nlm_cmmdKey = FALSE;
  Nlm_ctrlKey = FALSE;
  Nlm_optKey = FALSE;
  Nlm_shftKey = FALSE;
  Nlm_dblClick = FALSE;
}

static Nlm_WindoW Nlm_FindWindowRec (Nlm_WindowTool wptr)

{
  Nlm_WindoW      w;
#ifdef WIN_MAC
  Nlm_WindowData  wdata;
#endif

#ifdef WIN_MAC
  Nlm_GetWindowData (Nlm_systemWindow, &wdata);
  wdata.handle = wptr;
  Nlm_SetWindowData (Nlm_systemWindow, &wdata);
#endif
  w = Nlm_desktopWindow;
  while (w != NULL && Nlm_ParentWindowPtr ((Nlm_GraphiC) w) != wptr) {
    w = (Nlm_WindoW) Nlm_GetNext ((Nlm_GraphiC) w);
  }
  if (w == NULL) {
    w = Nlm_desktopWindow;
  }
  return w;
}

static void Nlm_SetWindowTitle (Nlm_GraphiC w, Nlm_Int2 item,
                                Nlm_CharPtr title, Nlm_Boolean savePort)


{
  Nlm_Char        temp [256];
  Nlm_WindowTool  wptr;
#ifdef WIN_MOTIF
  Nlm_ShellTool   shl;
  Nlm_WindowData  wdata;
#endif

  wptr = Nlm_ParentWindowPtr (w);
  Nlm_StringNCpy (temp, title, sizeof (temp));
#ifdef WIN_MAC
  Nlm_CtoPstr (temp);
  SetWTitle (wptr, (StringPtr) temp);
#endif
#ifdef WIN_MSWIN
  SetWindowText (wptr, temp);
#endif
#ifdef WIN_MOTIF
  shl = Nlm_GetWindowShell ((Nlm_WindoW) w);
  XtVaSetValues (shl, XmNtitle, temp, NULL);
  XSync (Nlm_currentXDisplay, FALSE);
  Nlm_GetWindowData ((Nlm_WindoW) w, &wdata);
  if (wdata.title != NULL) {
    wdata.title = (Nlm_CharPtr) Nlm_MemFree (wdata.title);
  }
  wdata.title = Nlm_StringSave (title);
  Nlm_SetWindowData ((Nlm_WindoW) w, &wdata);
#endif
}

static void Nlm_GetWindowTitle (Nlm_GraphiC w, Nlm_Int2 item,
                                Nlm_CharPtr title, size_t maxsize)

{
  Nlm_Char        temp [256];
  Nlm_WindowTool  wptr;
#ifdef WIN_MOTIF
  Nlm_WindowData  wdata;
#endif

  wptr = Nlm_ParentWindowPtr (w);
  temp [0] = '\0';
#ifdef WIN_MAC
  GetWTitle (wptr, (StringPtr) temp);
  Nlm_PtoCstr (temp);
#endif
#ifdef WIN_MSWIN
  GetWindowText (wptr, temp, sizeof (temp));
#endif
#ifdef WIN_MOTIF
  Nlm_GetWindowData ((Nlm_WindoW) w, &wdata);
  Nlm_StringNCpy (temp, wdata.title, sizeof (temp));
#endif
  Nlm_StringNCpy (title, temp, maxsize);
}

static Nlm_WindoW Nlm_MakeWindowLink (Nlm_RectPtr r, Nlm_Int2 recordSize,
                                      Nlm_GphPrcs PNTR classPtr)

{
  Nlm_GraphiC  n;
  Nlm_WindoW   w;

  w = (Nlm_WindoW) Nlm_HandNew (recordSize);
  if (w != NULL) {
    n = Nlm_GetNext ((Nlm_GraphiC) Nlm_desktopWindow);
    Nlm_LoadGraphicData ((Nlm_GraphiC) w, n, NULL, NULL, NULL,
                         classPtr, NULL, r, TRUE, FALSE);
    if (Nlm_nextIdNumber < 32767) {
      Nlm_nextIdNumber++;
    }
    Nlm_SetNext ((Nlm_GraphiC) Nlm_desktopWindow, (Nlm_GraphiC) w);
  }
  return w;
}

#ifdef WIN_MAC
extern void Nlm_SetUpdateRegion (WindowPtr wptr)

{
  RgnPtr  rptr;

  if (wptr != NULL) {
    CopyRgn (wptr->visRgn, (Nlm_RgnTool) Nlm_updateRgn);
    HLock ((Handle) Nlm_updateRgn);
    rptr = (RgnPtr) *((Handle) Nlm_updateRgn);
    Nlm_RectToolToRecT (&(rptr->rgnBBox), &Nlm_updateRect);
    HUnlock ((Handle) Nlm_updateRgn);
  }
}
#endif

#ifdef WIN_MOTIF
void WindowCloseCallback (Widget wd, XtPointer client_data, XtPointer call_data)

{
  Nlm_WndActnProc  cls;
  Nlm_WindoW       w;
  Nlm_WindowData   wdata;

  w = (Nlm_WindoW) client_data;
  Nlm_GetWindowData (w, &wdata);
  cls = wdata.close;
  if (cls != NULL) {
    cls (w);
  }
}

void WindowResizeCallback (Widget wd, XtPointer client_data,
                           XEvent *ev, Boolean *contin)

{
  XConfigureEvent  *event;
  Nlm_RecT         r;
  Nlm_WndActnProc  resize;
  Nlm_RectTool     rtool;
  Nlm_WindoW       w;
  Nlm_WindowData   wdata;

  event = (XConfigureEvent *) ev;
  if (event->type != ConfigureNotify) {
    return;
  }
  w = (Nlm_WindoW) client_data;
  Nlm_GetWindowData (w, &wdata);
  rtool.x = event->x;
  rtool.y = event->y;
  rtool.width = event->width;
  rtool.height = event->height;
  Nlm_RectToolToRecT (&rtool, &r);
  Nlm_SetRect ((Nlm_GraphiC) w, &r);
  if (Nlm_WindowHasBeenShown (w) && Nlm_GetVisible ((Nlm_GraphiC) w)) {
    resize = wdata.resize;
    if (resize != NULL) {
      resize (w);
    }
  }
}
#endif

#define DOCUMENT_STYLE 1
#define FIXED_STYLE    2
#define FROZEN_STYLE   3
#define ROUND_STYLE    4
#define ALERT_STYLE    5
#define MODAL_STYLE    6
#define FLOATING_STYLE 7
#define SHADOW_STYLE   8
#define PLAIN_STYLE    9

static void Nlm_NewWindow (Nlm_WindoW w, Nlm_Int2 type, Nlm_Int2 procID,
                           Nlm_Boolean goAway, Nlm_CharPtr windowClass,
                           Nlm_Uint4 style, Nlm_CharPtr title,
                           Nlm_WndActnProc close, Nlm_WndActnProc resize)

{
  Nlm_ShellTool   dlg;
  Nlm_RecT        drag;
  Nlm_RecT        grow;
  Nlm_Int2        hsb;
  Nlm_MainTool    man;
  Nlm_Int2        margin;
  Nlm_RecT        p;
  Nlm_PortTool    prt;
  Nlm_PoinT       pt;
  Nlm_RecT        r;
  Nlm_ShellTool   shl;
  Nlm_Int2        spacing;
  Nlm_Char        temp [256];
  Nlm_CharPtr     ttl;
  Nlm_Int2        vsb;
  Nlm_WindowTool  wptr;
  Nlm_RecT        zoom;
#ifdef WIN_MAC
  Nlm_WindowTool  behindNone;
  Nlm_RectTool    rtool;
#endif
#ifdef WIN_MOTIF
  Atom            atom;
  Cardinal        n;
  Arg             wargs [15];
  int             screen_num;
#endif

  if (title != NULL && title [0] != '\0') {
    Nlm_StringNCpy (temp, title, sizeof (temp));
  } else {
    temp [0] = '\0';
  }
#ifdef WIN_MAC
  Nlm_CtoPstr (temp);
#endif
  Nlm_LoadRect (&r, 50, 50, 150, 150);
#ifdef WIN_MOTIF
  r = Nlm_screenRect;
  Nlm_InsetRect (&r, 50, 50);
#endif
  Nlm_GetRect ((Nlm_GraphiC) w, &p);
  margin = 10;
  spacing = 10;
  if (p.right < 0) {
    margin = -p.right;
  }
  if (p.bottom < 0) {
    spacing = -p.bottom;
  }
  if (p.right >= 0 || p.bottom >= 0) {
    r = p;
    r.bottom = r.top + r.bottom;
    r.right = r.left + r.right;
  }
  Nlm_LoadPt (&pt, margin, margin);
  shl = NULL;
  dlg = NULL;
  wptr = NULL;
  prt = NULL;
  man = NULL;
  ttl = NULL;
  hsb = 0;
  vsb = 0;
#ifdef WIN_MAC
  wptr = (Nlm_WindowTool) Nlm_MemNew (sizeof (WindowRecord));
  behindNone = (Nlm_WindowTool) (-1);
  TextFont (0);
  TextSize (0);
  Nlm_RecTToRectTool (&r, &rtool);
  if (close == NULL) {
    goAway = FALSE;
  }
  if (hasColorQD) {
    wptr = (WindowPtr) NewCWindow ((Ptr) wptr, &rtool, (StringPtr) temp,
                                   FALSE, procID,(Nlm_WindowTool) behindNone,
                                   goAway, 0);
  } else {
    wptr = NewWindow ((Ptr) wptr, &rtool, (StringPtr) temp, FALSE, procID,
                      (Nlm_WindowTool) behindNone, goAway, 0);
  }
  SetPort (wptr);
  Nlm_currentWindowTool = wptr;
  prt = (Nlm_PortTool) wptr;
  Nlm_SetUpdateRegion (wptr);
  PenNormal ();
  MoveTo (2, 10);
  if (hasColorQD) {
    GetForeColor (&Nlm_RGBforeColor);
    GetBackColor (&Nlm_RGBbackColor);
  }
#endif
#ifdef WIN_MSWIN
  if (close == NULL) {
    style &= ~WS_SYSMENU;
  }
  if (type == FLOATING_STYLE) {
    wptr = CreateWindowEx (WS_EX_TOPMOST, windowClass, temp, style,
                         r.left, r.top, r.right - r.left,
                         r.bottom - r.top, NULL, NULL,
                         Nlm_currentHInst, NULL);
  } else if (type == MODAL_STYLE) {
    wptr = CreateWindowEx (WS_EX_DLGMODALFRAME, windowClass, temp, style,
                         r.left, r.top, r.right - r.left,
                         r.bottom - r.top, NULL, NULL,
                         Nlm_currentHInst, NULL);
  } else {
    wptr = CreateWindow (windowClass, temp, style,
                         r.left, r.top, r.right - r.left,
                         r.bottom - r.top, NULL, NULL,
                         Nlm_currentHInst, NULL);
  }
  if (wptr != NULL) {
    SetProp (wptr, (LPSTR) "Nlm_VibrantProp", (Nlm_HandleTool) w);
  }
  prt = GetDC (wptr);
  Nlm_currentHDC = prt;
  Nlm_currentHWnd = wptr;
#endif
#ifdef WIN_MOTIF
  n = 0;
  XtSetArg (wargs[n], XmNtitle, temp); n++;
  XtSetArg (wargs[n], XmNx, (Position) r.left); n++;
  XtSetArg (wargs[n], XmNy, (Position) r.top); n++;
  XtSetArg (wargs[n], XmNdefaultFontList, Nlm_XfontList); n++;
  XtSetArg (wargs[n], XmNdeleteResponse, XmDO_NOTHING); n++;
  shl = XtAppCreateShell ((String) NULL, (String) "Vibrant",
                          applicationShellWidgetClass,
                          Nlm_currentXDisplay, wargs, n);
  dlg = shl;
  if (dlg != NULL) {
    XtSetMappedWhenManaged (shl, FALSE);
    XtAddEventHandler (shl, StructureNotifyMask, FALSE,
                       WindowResizeCallback, (XtPointer) w);
    n = 0;
    XtSetArg (wargs[n], XmNwidth, (Dimension) (r.right - r.left)); n++;
    XtSetArg (wargs[n], XmNheight, (Dimension) (r.bottom - r.top)); n++;
    XtSetArg (wargs [n], XmNmarginHeight, 0); n++;
    XtSetArg (wargs [n], XmNmarginWidth, 0); n++;
    XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
    man = XmCreateMainWindow (dlg, (String) "", wargs, n);
    n = 0;
    XtSetArg (wargs[n], XmNwidth, (Dimension) (r.right - r.left)); n++;
    XtSetArg (wargs[n], XmNheight, (Dimension) (r.bottom - r.top)); n++;
    XtSetArg (wargs[n], XmNbuttonFontList, Nlm_XfontList); n++;
    XtSetArg (wargs[n], XmNlabelFontList, Nlm_XfontList); n++;
    XtSetArg (wargs[n], XmNtextFontList, Nlm_XfontList); n++;
    XtSetArg (wargs [n], XmNmarginHeight, 0); n++;
    XtSetArg (wargs [n], XmNmarginWidth, 0); n++;
    XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
    wptr = XmCreateBulletinBoard (man, (String) "", wargs, n);

    screen_num = DefaultScreen (Nlm_currentXDisplay);
    prt = Nlm_currentXGC;
    XtManageChild (man);
    XtManageChild (wptr);
    if (NLM_QUIET) {
      Nlm_currentXWindow = 0;
    } else {
      XtRealizeWidget (shl);
      Nlm_currentXWindow = XtWindow (shl);
    }
    atom = XmInternAtom (Nlm_currentXDisplay, "WM_DELETE_WINDOW", TRUE);
    if (atom != 0) {
      XmAddWMProtocolCallback (shl, atom, WindowCloseCallback, (XtPointer) w);
    }
  }
  if (temp [0] != '\0') {
    ttl = Nlm_StringSave (temp);
  }
#endif
  Nlm_currentWindowTool = wptr;
  r = screenBitBounds;
  Nlm_LoadRect (&drag, r.left + 4, r.top + 24, r.right - 4, r.bottom - 4);
  Nlm_LoadRect (&grow, r.left, r.top + 24, r.right, r.bottom);
  Nlm_LoadRect (&zoom, r.left + 4, r.top + 40, r.right - 4, r.bottom - 4);
  Nlm_LoadRect (&r, 0, 0, 0, 0);

#ifdef DCLAP
#ifdef WIN_MSWIN
  hsb = 6; vsb = 4;  /* dgg -- patch for mswin sizebar space */
#endif
#endif

  Nlm_LoadBoxData ((Nlm_BoX) w, pt, pt, pt, margin, margin,
                   margin + hsb, margin + vsb, spacing, spacing, 0, 0);
  Nlm_LoadWindowData (w, wptr, shl, prt, man, &drag, &grow, &zoom, close, NULL,
                      NULL, resize, NULL, NULL, NULL, TRUE, FALSE, ttl, NULL, NULL);
}

#ifdef WIN_MAC
static Nlm_WindoW Nlm_NextVisWindow (Nlm_WindoW w)

{
  WindowPeek  p;
  WindowPeek  q;

  if (w == Nlm_desktopWindow) {
    return Nlm_desktopWindow;
  } else {
    p = (WindowPeek) Nlm_ParentWindowPtr ((Nlm_GraphiC) w);
    while (p != NULL) {
      q = p->nextWindow;
      if (q != NULL) {
        if (q->visible != 0) {
          p = NULL;
        } else {
          p = q;
        }
      } else {
        p = q;
      }
    }
    return Nlm_FindWindowRec ((Nlm_WindowTool) q);
  }
}
#endif

static void Nlm_RemoveWindow (Nlm_GraphiC w, Nlm_Boolean savePort)

{
  Nlm_WindoW     nxt;
  Nlm_WindoW     p;
  Nlm_WindoW     q;
#ifdef WIN_MOTIF
  Nlm_ShellTool  shl;
#endif

  if ((Nlm_WindoW) w != Nlm_desktopWindow && (Nlm_WindoW) w != Nlm_systemWindow) {
    q = Nlm_desktopWindow;
    p = (Nlm_WindoW) Nlm_GetNext ((Nlm_GraphiC) Nlm_desktopWindow);
    while (p != NULL && p != (Nlm_WindoW) w) {
      q = p;
      p = (Nlm_WindoW) Nlm_GetNext ((Nlm_GraphiC) p);
    }
    if (p != NULL) {
#ifdef WIN_MAC
      HideWindow (Nlm_GetWindowPtr ((Nlm_WindoW) w));
      Nlm_Update ();
#endif
#ifdef WIN_MSWIN
      ShowWindow (Nlm_GetWindowPtr ((Nlm_WindoW) w), SW_HIDE);
#endif
#ifdef WIN_MOTIF
      shl = Nlm_GetWindowShell ((Nlm_WindoW) w);
      if (NLM_QUIET) {
        if (Nlm_WindowHasBeenShown ((Nlm_WindoW) w)) {
          XtUnmapWidget (shl);
        }
      } else {
        XtUnmapWidget (shl);
      }
#endif
      Nlm_SetWindowDying ((Nlm_WindoW) w, TRUE);
      nxt = (Nlm_WindoW) Nlm_GetNext (w);
      Nlm_SetNext ((Nlm_GraphiC) q, (Nlm_GraphiC) nxt);
      Nlm_SetNext (w, (Nlm_GraphiC) dyingWindow);
      dyingWindow = (Nlm_WindoW) w;
    }
  }
  Nlm_Update ();
}

static void Nlm_HideWindow (Nlm_GraphiC w, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_WindowTool  wptr;
#ifdef WIN_MOTIF
  Nlm_ShellTool   shl;
#endif

  wptr = Nlm_ParentWindowPtr (w);
  Nlm_SetVisible (w, FALSE);
#ifdef WIN_MAC
  HideWindow (wptr);
  Nlm_Update ();
#endif
#ifdef WIN_MSWIN
  ShowWindow (wptr, SW_HIDE);
  Nlm_Update ();
#endif
#ifdef WIN_MOTIF
  shl = Nlm_GetWindowShell ((Nlm_WindoW) w);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown ((Nlm_WindoW) w)) {
      XtUnmapWidget (shl);
    }
  } else {
    XtUnmapWidget (shl);
  }
  Nlm_DoDeactivate (w, FALSE);
  Nlm_Update ();
#endif
}

/* Thanks to Mike Isaev for suggesting this solution to modal windows. */

static void Nlm_EnableOneWindow (Nlm_WindoW w, Nlm_Boolean onOffFlag)

{
#ifdef WIN_MSWIN
  Nlm_WindowTool  wptr;
#endif
#ifdef WIN_MOTIF
  Nlm_ShellTool   shl;
#endif

  if (w != NULL && w != Nlm_desktopWindow && w != Nlm_systemWindow) {
#ifdef WIN_MSWIN
    wptr = Nlm_GetWindowPtr (w);
    EnableWindow (wptr, onOffFlag);
#endif
#ifdef WIN_MOTIF
    shl = Nlm_GetWindowShell (w);
    XtVaSetValues (shl, XmNsensitive, onOffFlag, NULL);
#endif
  }
}

static void Nlm_EnableOtherWindows (Nlm_WindoW notThisWindow, Nlm_Boolean onOffFlag)

{
  Nlm_WindoW  w;

  w = (Nlm_WindoW) Nlm_GetNext ((Nlm_GraphiC) Nlm_desktopWindow);
  while (w != NULL && w != Nlm_systemWindow) {
    if (w != notThisWindow) {
      Nlm_EnableOneWindow (w, onOffFlag);
    }
    w = (Nlm_WindoW) Nlm_GetNext ((Nlm_GraphiC) w);
  }
}

static void Nlm_HideModal (Nlm_GraphiC w, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_HideWindow (w, setFlag, savePort);
  Nlm_EnableOtherWindows ((Nlm_WindoW ) w, TRUE);
}

static void Nlm_ResizeWindow (Nlm_GraphiC w, Nlm_Int2 dragHeight,
                              Nlm_Int2 scrollWidth, Nlm_Int2 minWidth,
                              Nlm_Int2 extraHeight)

{
  Nlm_BoxData     bdata;
  Nlm_Int4        free;
  Nlm_Int2        height;
  Nlm_Int4        leftpix;
  Nlm_PoinT       lpt;
  Nlm_Int2        menuHeight;
  Nlm_RecT        r;
  Nlm_Int4        rleft;
  Nlm_Int4        rtop;
  Nlm_Int4        toppix;
  Nlm_Int2        width;
  Nlm_WindowTool  wptr;
#ifdef WIN_MOTIF
  Nlm_ButtoN      b;
  Nlm_MainTool    man;
  Nlm_ShellTool   shl;
  Window          xtw;
#endif

  if (! Nlm_WindowHasBeenShown ((Nlm_WindoW) w)) {
#ifdef WIN_MAC
    menuHeight = 21;
    TextFont (0);
    TextSize (0);
#endif
#ifdef WIN_MSWIN
    menuHeight = 0;
#endif
#ifdef WIN_MOTIF
    menuHeight = 0;
#endif
    Nlm_GetRect (w, &r);
    if (r.left < 0 || r.right < 0 || r.top < 0 || r.bottom < 0) {
      Nlm_GetBoxData ((Nlm_BoX) w, &bdata);
      lpt = bdata.limitPoint;
      if (r.right < 0) {
        width = lpt.x + scrollWidth + bdata.xMargin;
        if (width < minWidth) {
          width = minWidth;
        }
      } else {
        width = r.right;
      }
      if (r.bottom < 0) {
        height = lpt.y + scrollWidth + extraHeight + bdata.yMargin;
      } else {
        height = r.bottom + extraHeight;
      }
      if (r.left < 0) {
        free = screenBitBounds.right - width;
        rleft = (Nlm_Int4) r.left;
        leftpix = free * (-rleft) / 100;
        r.left = (Nlm_Int2) leftpix;
      }
      if (r.top < 0) {
        free = screenBitBounds.bottom - height - menuHeight - dragHeight;
        rtop = (Nlm_Int4) r.top;
        toppix = (free * (-rtop) / (Nlm_Int4) 100);
        r.top = (Nlm_Int2) (toppix + menuHeight + dragHeight);
      }
      r.right = r.left + width;
      r.bottom = r.top + height;
      Nlm_SetRect (w, &r);
      wptr = Nlm_GetWindowPtr ((Nlm_WindoW) w);
#ifdef WIN_MAC
      SizeWindow (wptr, width, height, FALSE);
      MoveWindow (wptr, r.left, r.top, FALSE);
#endif
#ifdef WIN_MSWIN
      MoveWindow (wptr, r.left, r.top, width, height, FALSE);
#endif
#ifdef WIN_MOTIF
      shl = Nlm_GetWindowShell ((Nlm_WindoW) w);
      man = Nlm_GetWindowMain ((Nlm_WindoW) w);
      XtVaSetValues (shl,
                     XmNx, (Position) r.left,
                     XmNy, (Position) r.top, 
                     XmNwidth, (Dimension) width,
                     XmNheight, (Dimension) height, 
                     NULL);
      XtVaSetValues (man,
                     XmNwidth, (Dimension) width,
                     XmNheight, (Dimension) height, 
                     NULL);
      XtVaSetValues (wptr,
                     XmNwidth, (Dimension) width,
                     XmNheight, (Dimension) height, 
                     NULL);
#endif
    }
#ifdef WIN_MOTIF
    if (NLM_QUIET && (! Nlm_IsWindowDying ((Nlm_WindoW) w))) {
      b = Nlm_GetWindowDefaultButton ((Nlm_WindoW) w);
      if (b != NULL) {
        Nlm_MapDefaultButton ((Nlm_WindoW) w, b);
      }
      shl = Nlm_GetWindowShell ((Nlm_WindoW) w);
      XtRealizeWidget (shl);
      man = Nlm_GetWindowMain ((Nlm_WindoW) w);
      xtw = XtWindow (man);
      if (xtw != 0) {
        XDefineCursor (Nlm_currentXDisplay, xtw, currentCursor);
      }
    }
#endif
  }
}

static void Nlm_ShowPlain (Nlm_GraphiC w, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_Int2        extra;
  Nlm_WindowTool  wptr;
#ifdef WIN_MOTIF
  Nlm_ShellTool   shl;
#endif

  wptr = Nlm_ParentWindowPtr (w);
#ifdef WIN_MAC
  Nlm_ResizeWindow (w, 0, 0, 0, 0);
  Nlm_SetNotYetShown ((Nlm_WindoW) w, FALSE);
  if (setFlag) {
    Nlm_SetVisible (w, TRUE);
    ShowWindow (wptr);
  }
#endif
#ifdef WIN_MSWIN
  extra = 2 * GetSystemMetrics (SM_CYBORDER);
  if (Nlm_GetWindowMenuBar ((Nlm_WindoW) w) != NULL) {
    extra += GetSystemMetrics (SM_CYMENU);
  }
  Nlm_ResizeWindow (w, 0, 0, 0, extra);
  Nlm_SetNotYetShown ((Nlm_WindoW) w, FALSE);
  if (setFlag) {
    Nlm_SetVisible (w, TRUE);
    ShowWindow (wptr, SW_SHOW);
    UpdateWindow (wptr);
  }
#endif
#ifdef WIN_MOTIF
  extra = 0;
  if (Nlm_GetWindowMenuBar ((Nlm_WindoW) w) != NULL) {
    extra += 31;
  }
  Nlm_ResizeWindow (w, 0, 0, 0, extra);
  Nlm_SetNotYetShown ((Nlm_WindoW) w, FALSE);
  if (setFlag) {
    Nlm_SetVisible (w, TRUE);
    shl = Nlm_GetWindowShell ((Nlm_WindoW) w);
    XtMapWidget (shl);   
    Nlm_DoActivate ((Nlm_GraphiC) w, FALSE);
  }
#endif
  Nlm_Update ();
}

static void Nlm_ShowNormal (Nlm_GraphiC w, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_Int2        extra;
  Nlm_Char        title [256];
  Nlm_WindowTool  wptr;
#ifdef WIN_MOTIF
  Nlm_ShellTool   shl;
#endif

  wptr = Nlm_ParentWindowPtr (w);
  Nlm_GetWindowTitle (w, 0, title, sizeof (title));
#ifdef WIN_MAC
  Nlm_ResizeWindow (w, 20, 0, Nlm_StringWidth (title) + 70, 0);
  Nlm_SetNotYetShown ((Nlm_WindoW) w, FALSE);
  if (setFlag) {
    Nlm_SetVisible (w, TRUE);
    ShowWindow (wptr);
  }
#endif
#ifdef WIN_MSWIN
  extra = GetSystemMetrics (SM_CYCAPTION) + 2 * GetSystemMetrics (SM_CYFRAME);
  if (Nlm_GetWindowMenuBar ((Nlm_WindoW) w) != NULL) {
    extra += GetSystemMetrics (SM_CYMENU);
  }
  Nlm_ResizeWindow (w, 0, 0, Nlm_StringWidth (title) + 80, extra);
  Nlm_SetNotYetShown ((Nlm_WindoW) w, FALSE);
  if (setFlag) {
    Nlm_SetVisible (w, TRUE);
    ShowWindow (wptr, SW_SHOW);
    UpdateWindow (wptr);
  }
#endif
#ifdef WIN_MOTIF
  extra = 0;
  if (Nlm_GetWindowMenuBar ((Nlm_WindoW) w) != NULL) {
    extra += 31;
  }
  Nlm_ResizeWindow (w, 0, 0, Nlm_StringWidth (title) + 65, extra);
  Nlm_SetNotYetShown ((Nlm_WindoW) w, FALSE);
  if (setFlag) {
    Nlm_SetVisible (w, TRUE);
    shl = Nlm_GetWindowShell ((Nlm_WindoW) w);
    XtMapWidget (shl);   
    Nlm_DoActivate ((Nlm_GraphiC) w, FALSE);
  }
#endif
  Nlm_Update ();
}

static void Nlm_ShowModal (Nlm_GraphiC w, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_ShowNormal (w, setFlag, savePort);
  Nlm_EnableOtherWindows ((Nlm_WindoW ) w, FALSE);
}

static void Nlm_ShowDocument (Nlm_GraphiC w, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_Int2        extra;
  Nlm_Char        title [256];
  Nlm_WindowTool  wptr;
#ifdef WIN_MOTIF
  Nlm_ShellTool   shl;
#endif

  wptr = Nlm_ParentWindowPtr (w);
  Nlm_GetWindowTitle (w, 0, title, sizeof (title));
#ifdef WIN_MAC
  Nlm_ResizeWindow (w, 20, 16, Nlm_StringWidth (title) + 70, 0);
  Nlm_SetNotYetShown ((Nlm_WindoW) w, FALSE);
  if (setFlag) {
    Nlm_SetVisible (w, TRUE);
    ShowWindow (wptr);
  }
#endif
#ifdef WIN_MSWIN
  extra = GetSystemMetrics (SM_CYCAPTION) + 2 * GetSystemMetrics (SM_CYBORDER);
  if (Nlm_GetWindowMenuBar ((Nlm_WindoW) w) != NULL) {
    extra += GetSystemMetrics (SM_CYMENU);
  }
  Nlm_ResizeWindow (w, 0, 0, Nlm_StringWidth (title) + 80, extra);
  Nlm_SetNotYetShown ((Nlm_WindoW) w, FALSE);
  if (setFlag) {
    Nlm_SetVisible (w, TRUE);
    ShowWindow (wptr, SW_SHOW);
    UpdateWindow (wptr);
  }
#endif
#ifdef WIN_MOTIF
  extra = 0;
  if (Nlm_GetWindowMenuBar ((Nlm_WindoW) w) != NULL) {
    extra += 31;
  }
  Nlm_ResizeWindow (w, 0, 0, Nlm_StringWidth (title) + 65, extra);
  Nlm_SetNotYetShown ((Nlm_WindoW) w, FALSE);
  if (setFlag) {
    Nlm_SetVisible (w, TRUE);
    shl = Nlm_GetWindowShell ((Nlm_WindoW) w);
    XtMapWidget (shl);   
    Nlm_DoActivate ((Nlm_GraphiC) w, FALSE);
  }
#endif
  Nlm_Update ();
}

extern void Nlm_RealizeWindow (Nlm_WindoW w)

{
  if (w != NULL) {
    Nlm_DoShow ((Nlm_GraphiC) w, FALSE, TRUE);
  }
}

extern Nlm_WindoW Nlm_WhichWindow (Nlm_PoinT mouseLoc)

{
#ifdef WIN_MAC
  Nlm_PoinT       point;
  Nlm_PointTool   ptool;
  Nlm_Int2        windowLoc;
  Nlm_WindowTool  wptr;

  point = mouseLoc;
  Nlm_LocalToGlobal (&point);
  Nlm_PoinTToPointTool (point, &ptool);
  windowLoc = FindWindow (ptool, &wptr);
  return Nlm_FindWindowRec (wptr);
#endif
#ifdef WIN_MSWIN
  Nlm_PoinT       point;
  Nlm_PointTool   ptool;
  Nlm_WindoW      rsult;
  Nlm_WindowTool  wptr;

  rsult = NULL;
  point = mouseLoc;
  Nlm_LocalToGlobal (&point);
  Nlm_PoinTToPointTool (point, &ptool);
  wptr = WindowFromPoint (ptool);
  if (wptr != NULL) {
    rsult = (Nlm_WindoW) GetProp (wptr, (LPSTR) "Nlm_VibrantProp");
  }
  return rsult;
#endif
#ifdef WIN_MOTIF
  return NULL;
#endif
}

extern Nlm_Boolean Nlm_InWindow (Nlm_PoinT mouseLoc)

{
  return (Nlm_Boolean) (Nlm_WhichWindow (mouseLoc) != Nlm_desktopWindow);
}

extern Nlm_WindoW Nlm_FrontWindow ()

{
#ifdef WIN_MAC
  return (Nlm_FindWindowRec (FrontWindow ()));
#endif
#ifdef WIN_MSWIN
  return (Nlm_FindWindowRec (Nlm_currentHWnd));
#endif
#ifdef WIN_MOTIF
  return NULL;
#endif
}

extern Nlm_Boolean Nlm_InFront (Nlm_WindoW w)

{
#ifdef WIN_MAC
  return (w == Nlm_FrontWindow ());
#endif
#ifdef WIN_MSWIN
  return FALSE;
#endif
#ifdef WIN_MOTIF
  return FALSE;
#endif
}

extern void Nlm_UseWindow (Nlm_WindoW w)

{
  Nlm_WindowTool  wptr;
#ifdef WIN_MOTIF
  Nlm_ShellTool   shl;
#endif

  if (w != NULL && w != Nlm_desktopWindow) {
    wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) w);
    if (wptr != NULL && (! Nlm_IsWindowDying (w))) {
#ifdef WIN_MAC
      SetPort (wptr);
      Nlm_SetUpdateRegion (wptr);
      Nlm_ResetDrawingTools ();
#endif
#ifdef WIN_MSWIN
      Nlm_currentHDC = Nlm_GetWindowPort (w);
      Nlm_currentHWnd = wptr;
#endif
#ifdef WIN_MOTIF
      shl = Nlm_GetWindowShell (w);
      Nlm_currentXWindow = XtWindow (shl);
      Nlm_currentXGC = Nlm_GetWindowPort (w);
#endif
      Nlm_currentWindowTool = wptr;
    }
  }
}

extern Nlm_WindoW Nlm_CurrentWindow ()

{
  Nlm_WindoW  rsult;

  rsult = Nlm_FindWindowRec (Nlm_currentWindowTool);
  return rsult;
}

extern Nlm_Boolean Nlm_UsingWindow (Nlm_WindoW w)

{
  return (Nlm_Boolean) (w == Nlm_CurrentWindow ());
}

extern Nlm_WindoW Nlm_ActiveWindow ()

{
  Nlm_WindoW      rsult;
  Nlm_WindowTool  wptr;

  rsult = NULL;
#ifdef WIN_MAC
  rsult = Nlm_theWindow;
#endif
#ifdef WIN_MSWIN
  wptr = GetActiveWindow ();
  if (wptr != NULL) {
    rsult = (Nlm_WindoW) GetProp (wptr, (LPSTR) "Nlm_VibrantProp");
  }
#endif
#ifdef WIN_MOTIF
#endif
  return rsult;
}

#ifdef WIN_MAC
static Nlm_WindoW Nlm_PrevVisWindow (Nlm_WindoW w)

{
  WindowPeek  p;
  WindowPeek  q;
  WindowPeek  t;

  if (frontWindow == Nlm_desktopWindow) {
    return Nlm_desktopWindow;
  } else if (frontWindow == w) {
    return frontWindow;
  } else {
    p = (WindowPeek) Nlm_ParentWindowPtr ((Nlm_GraphiC) frontWindow);
    q = p;
    t = (WindowPeek) Nlm_ParentWindowPtr ((Nlm_GraphiC) w);
    while (p != NULL) {
      if (p == t) {
        p = NULL;
      } else if (p->visible != 0) {
        q = p;
        p = p->nextWindow;
      } else {
        p = p->nextWindow;
      }
    }
    return (Nlm_FindWindowRec ((Nlm_WindowTool) q));
  }
}
#endif

static void Nlm_SelectWindow (Nlm_GraphiC w, Nlm_Boolean savePort)

{
  Nlm_WindowTool  wptr;
#ifdef WIN_MOTIF
  Nlm_ShellTool   shl;
#endif

  if (w != NULL && (Nlm_WindoW) w != Nlm_desktopWindow) {
    wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) w);
    if (wptr != NULL && (! Nlm_IsWindowDying ((Nlm_WindoW) w))) {
#ifdef WIN_MAC
      SelectWindow (wptr);
      SetPort (wptr);
      Nlm_SetUpdateRegion (wptr);
      Nlm_ResetDrawingTools ();
#endif
#ifdef WIN_MSWIN
      BringWindowToTop (wptr);
/* M.I */
      if( IsIconic( wptr ) )
        ShowWindow( wptr, SW_RESTORE );  /* de-Iconize window */
/* M.I */
      Nlm_currentHDC = Nlm_GetWindowPort ((Nlm_WindoW) w);
      Nlm_currentHWnd = wptr;
#endif
#ifdef WIN_MOTIF
      if (Nlm_WindowHasBeenShown ((Nlm_WindoW) w)) {
        if (Nlm_currentXDisplay != NULL) {
          shl = Nlm_GetWindowShell ((Nlm_WindoW) w);
          XMapRaised (Nlm_currentXDisplay, XtWindow (shl));
        }
      }
#endif
      Nlm_currentWindowTool = wptr;
      Nlm_Update ();
    }
  }
}

extern void Nlm_EraseWindow (Nlm_WindoW w)

{
#ifdef WIN_MAC
  Nlm_RecT        r;
  Nlm_PortTool    temp;
  Nlm_WindowTool  wptr;

  if (w != NULL) {
    wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) w);
    GetPort (&temp);
    SetPort (wptr);
    Nlm_ResetDrawingTools ();
    Nlm_currentWindowTool = wptr;
    Nlm_RectToolToRecT (&(wptr->portRect), &r);
    Nlm_EraseRect (&r);
    SetPort (temp);
    Nlm_currentWindowTool = temp;
    Nlm_Update ();
  }
#endif
#ifdef WIN_MSWIN
#endif
#ifdef WIN_MOTIF
#endif
}

#ifdef WIN_MAC
static Nlm_Boolean Nlm_DragClick (Nlm_GraphiC w, Nlm_PoinT pt)

{
  Nlm_PointTool   ptool;
  Nlm_RecT        r;
  Nlm_RectTool    rtool;
  Nlm_Boolean     rsult;
  Nlm_Int2        windowLoc;
  Nlm_WindowData  wdata;
  Nlm_WindowTool  wptr;

  rsult = FALSE;
  Nlm_PoinTToPointTool (Nlm_globalMouse, &ptool);
  windowLoc = FindWindow (ptool, &wptr);
  if (windowLoc == inDrag) {
    Nlm_GetWindowData ((Nlm_WindoW) w, &wdata);
    Nlm_RecTToRectTool (&(wdata.dragArea), &rtool);
    DragWindow (wptr, ptool, &rtool);
    Nlm_RectToolToRecT (&(wptr->portRect), &r);
    Nlm_LocalToGlobal ((Nlm_PointPtr) &(r.left));
    Nlm_LocalToGlobal ((Nlm_PointPtr) &(r.right));
    Nlm_SetRect (w, &r);
    rsult = TRUE;
  }
  return rsult;
}

static void Nlm_UpdateScrollBar (Nlm_GraphiC w)

{
  Nlm_RecT        barArea;
  Nlm_WindowTool  wptr;

  wptr = Nlm_ParentWindowPtr (w);
  Nlm_RectToolToRecT (&(wptr->portRect), &barArea);
  barArea.left = barArea.right - 16;
  if (Nlm_GetWindowMenuBar ((Nlm_WindoW) w) != NULL) {
    barArea.top = barArea.top + 21;
  }
  Nlm_InvalRect (&barArea);
  Nlm_RectToolToRecT (&(wptr->portRect), &barArea);
  barArea.top = barArea.bottom - 16;
  Nlm_InvalRect (&barArea);
}

static Nlm_Boolean Nlm_GrowClick (Nlm_GraphiC w, Nlm_PoinT pt)

{
  Nlm_Int2         ht;
  Nlm_Int4         newSize;
  Nlm_PointTool    ptool;
  Nlm_RecT         r;
  Nlm_WndActnProc  resize;
  Nlm_RectTool     rtool;
  Nlm_Boolean      rsult;
  Nlm_Int2         wd;
  Nlm_WindowData   wdata;
  Nlm_Int2         windowLoc;
  Nlm_WindowTool   wptr;

  rsult = FALSE;
  Nlm_PoinTToPointTool (Nlm_globalMouse, &ptool);
  windowLoc = FindWindow (ptool, &wptr);
  if (windowLoc == inGrow) {
    Nlm_LoadRect (&r, -32768, -32768, 32767, 32767);
    Nlm_RecTToRectTool (&r, &rtool);
    ClipRect (&rtool);
    Nlm_GetWindowData ((Nlm_WindoW) w, &wdata);
    Nlm_RecTToRectTool (&(wdata.growArea), &rtool);
    newSize = GrowWindow (wptr, ptool, &rtool);
    wd = LoWord (newSize);
    ht = HiWord (newSize);
 /* dgg -- bug fix: simple click on grow box caused it to resize to 0 -- 
    should check here for 0 == no change.  Also make minsize bigger. 
 */
    if (! newSize) {
      ClipRect (&(wptr->portRect));
      return rsult;
    }
    if (wd < 50) {
      wd = 50;
    }
    if (ht < 32) {
      ht = 32;
    }
    Nlm_UpdateScrollBar (w);
    SizeWindow (wptr, wd, ht, TRUE);
    Nlm_UpdateScrollBar (w);
    wptr = Nlm_GetWindowPtr ((Nlm_WindoW) w);
    Nlm_RectToolToRecT (&(wptr->portRect), &r);
    ClipRect (&(wptr->portRect));
    Nlm_LocalToGlobal ((Nlm_PointPtr) &(r.left));
    Nlm_LocalToGlobal ((Nlm_PointPtr) &(r.right));
    Nlm_SetRect (w, &r);
    resize = wdata.resize;
    if (resize != NULL) {
      resize ((Nlm_WindoW) w);
    }
    rsult = TRUE;
  }
  return rsult;
}

static void Nlm_DrawGrowIcon (Nlm_GraphiC w)

{
  Nlm_RecT        r;
  Nlm_RectTool    rtool;
  PenState        state;
  Nlm_PortTool    temp;
  Nlm_WindowTool  wptr;

  GetPort (&temp);
  GetPenState (&state);
  wptr = Nlm_ParentWindowPtr (w);
  Nlm_RectToolToRecT (&(wptr->portRect), &r);
  if (Nlm_GetWindowMenuBar ((Nlm_WindoW) w) != NULL) {
    Nlm_RecTToRectTool (&r, &rtool);
    ClipRect (&rtool);
    MoveTo (r.right - 16, 20);
    LineTo (r.right, 20);
    r.top = r.top + 21;
  }
  Nlm_RecTToRectTool (&r, &rtool);
  ClipRect (&rtool);
  DrawGrowIcon (wptr);
  r.top = wptr->portRect.top;
#ifdef DCLAP
  /* dgg- this cliprect is the culprit for preventing scrollbar updates by subviews */
  /* try clipping a region that excludes just the growicon?? */
#else
  r.right = r.right - 16;
  r.bottom = r.bottom - 16;
#endif
  Nlm_RecTToRectTool (&r, &rtool);
  ClipRect (&rtool);
  SetPort (temp);
  Nlm_currentWindowTool = temp;
  Nlm_SetUpdateRegion (temp);
  SetPenState (&state);
}

static Nlm_Boolean Nlm_ZoomClick (Nlm_GraphiC w, Nlm_PoinT pt)

{
  Nlm_PointTool    ptool;
  Nlm_RecT         r;
  Nlm_WndActnProc  resize;
  Nlm_Boolean      rsult;
  Nlm_RectTool     rtool;
  Nlm_WindowData   wdata;
  Nlm_Int2         windowLoc;
  WindowPeek       wpeek;
  Nlm_WindowTool   wptr;
  WStateData       **wshdl;
  WStateData       *wsptr;

  rsult = FALSE;
  Nlm_PoinTToPointTool (Nlm_globalMouse, &ptool);
  windowLoc = FindWindow (ptool, &wptr);
  if (windowLoc == inZoomIn || windowLoc == inZoomOut) {
    if (TrackBox (wptr, ptool, windowLoc)) {
      wpeek = (WindowPeek) wptr;
      wshdl = (WStateData**) wpeek->dataHandle;
      wsptr = *wshdl;
      Nlm_GetWindowData ((Nlm_WindoW) w, &wdata);
      r = wdata.zoomArea;
      Nlm_RecTToRectTool (&r, &(wsptr->stdState));
      Nlm_RectToolToRecT (&(wptr->portRect), &r);
      ClipRect (&(wptr->portRect));
      Nlm_EraseRect (&r);
      ZoomWindow (wptr, windowLoc, FALSE);
      Nlm_UpdateScrollBar (w);
      wptr = Nlm_GetWindowPtr ((Nlm_WindoW) w);
      Nlm_RectToolToRecT (&(wptr->portRect), &r);
      ClipRect (&(wptr->portRect));
      Nlm_LocalToGlobal ((Nlm_PointPtr) &(r.left));
      Nlm_LocalToGlobal ((Nlm_PointPtr) &(r.right));
      Nlm_SetRect (w, &r);
      resize = wdata.resize;
      if (resize != NULL) {
        resize ((Nlm_WindoW) w);
      }
    }
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_CloseClick (Nlm_GraphiC w, Nlm_PoinT pt)

{
  Nlm_WndActnProc  cls;
  Nlm_PointTool    ptool;
  Nlm_Boolean      rsult;
  Nlm_WindowData   wdata;
  Nlm_Int2         windowLoc;
  Nlm_WindowTool   wptr;

  rsult = FALSE;
  Nlm_PoinTToPointTool (Nlm_globalMouse, &ptool);
  windowLoc = FindWindow (ptool, &wptr);
  if (windowLoc == inGoAway) {
    if (TrackGoAway (wptr, ptool)) {
      Nlm_GetWindowData ((Nlm_WindoW) w, &wdata);
      cls = wdata.close;
      if (cls != NULL) {
        cls ((Nlm_WindoW) w);
      }
    }
    rsult = TRUE;
  }
  return rsult;
}

typedef struct revItem {
  Nlm_GraphiC          graphic;
  struct revItem  PNTR next;
} Nlm_RevItem, PNTR Nlm_RevPtr;

static Nlm_Boolean Nlm_ContentClick (Nlm_GraphiC w, Nlm_PoinT pt)

{
  Nlm_GraphiC    g;
  Nlm_MenuBaR    mb;
  Nlm_GraphiC    n;
  Nlm_RevPtr     next;
  Nlm_Boolean    notInside;
  Nlm_PointTool  ptool;
  Nlm_RevPtr     this;
  Nlm_RevPtr     top;

  Nlm_localMouse = Nlm_globalMouse;
  Nlm_GlobalToLocal (&Nlm_localMouse);
  g = Nlm_GetChild (w);
  notInside = TRUE;
  mb = Nlm_GetWindowMenuBar ((Nlm_WindoW) w);
  if (mb != NULL) {
    if (Nlm_GetEnabled ((Nlm_GraphiC) mb) && Nlm_GetVisible ((Nlm_GraphiC) mb) &&
        Nlm_DoClick ((Nlm_GraphiC) mb, Nlm_localMouse)) {
      notInside = FALSE;
    }
  }
  /*
  while (g != NULL && notInside) {
    n = Nlm_GetNext (g);
    if (Nlm_GetEnabled (g) && Nlm_GetVisible (g) && Nlm_DoClick (g, Nlm_localMouse)) {
      notInside = FALSE;
    }
    g = n;
  }
  */
  if (g != NULL && notInside) {
    top = NULL;
    while (g != NULL) {
      this = Nlm_MemNew (sizeof (Nlm_RevItem));
      if (this != NULL) {
        this->graphic = g;
        this->next = top;
        top = this;
      }
      g = Nlm_GetNext (g);
    }
    this = top;
    while (this != NULL && notInside) {
      next = this->next;
      g = this->graphic;
      if (Nlm_GetEnabled (g) && Nlm_GetVisible (g) && Nlm_DoClick (g, Nlm_localMouse)) {
        notInside = FALSE;
      }
      this = next;
    }
    this = top;
    while (this != NULL) {
      next = this->next;
      Nlm_MemFree (this);
      this = next;
    }
  }
  if (notInside) {
    GetMouse (&ptool);
    Nlm_PointToolToPoinT (ptool, &Nlm_localMouse);
    if ((Nlm_WindoW) w != Nlm_WhichWindow (Nlm_localMouse)) {
      notInside = FALSE;
    }
  }
  return (! notInside);
}

static Nlm_Boolean Nlm_CommonClick (Nlm_GraphiC w, Nlm_PoinT pt,
                                    Nlm_Boolean close, Nlm_Boolean drag,
                                    Nlm_Boolean grow, Nlm_Boolean zoom)

{
  Nlm_Boolean     rsult;
  PenState        state;
  Nlm_PortTool    temp;
  Nlm_WindowTool  wptr;

  rsult = FALSE;
  if (chosenWindow == (Nlm_WindoW) w) {
    wptr = Nlm_ParentWindowPtr (w);
    GetPort (&temp);
    GetPenState (&state);
    SetPort (wptr);
    Nlm_currentWindowTool = wptr;
    Nlm_SetUpdateRegion (wptr);
    Nlm_ResetDrawingTools ();
    if ((close && Nlm_CloseClick (w, pt)) ||
       (drag && Nlm_DragClick (w, pt)) ||
       (grow && Nlm_GrowClick (w, pt)) ||
       (zoom && Nlm_ZoomClick (w, pt))) {
    } else {
      rsult = Nlm_ContentClick (w, pt);
    }
    SetPort (temp);
    Nlm_currentWindowTool = temp;
    Nlm_SetUpdateRegion (temp);
    SetPenState (&state);
  } else if (chosenWindow != NULL) {
    Nlm_DoSelect ((Nlm_GraphiC) chosenWindow, TRUE);
  }
  return rsult;
}

static Nlm_Boolean Nlm_DocumentClick (Nlm_GraphiC w, Nlm_PoinT pt)

{
  return Nlm_CommonClick (w, pt, TRUE, TRUE, TRUE, TRUE);
}

static Nlm_Boolean Nlm_DialogClick (Nlm_GraphiC w, Nlm_PoinT pt)

{
  return Nlm_CommonClick (w, pt, TRUE, TRUE, FALSE, FALSE);
}

static Nlm_Boolean Nlm_FrozenClick (Nlm_GraphiC w, Nlm_PoinT pt)

{
  return Nlm_CommonClick (w, pt, TRUE, FALSE, FALSE, FALSE);
}

static Nlm_Boolean Nlm_PlainClick (Nlm_GraphiC w, Nlm_PoinT pt)

{
  return Nlm_CommonClick (w, pt, FALSE, FALSE, FALSE, FALSE);
}

static Nlm_Boolean Nlm_ModalClick (Nlm_GraphiC w, Nlm_PoinT pt)

{
  Nlm_WndActnProc  cls;
  Nlm_Boolean      rsult;
  PenState         state;
  Nlm_PortTool     temp;
  Nlm_WindowData   wdata;
  Nlm_WindowTool   wptr;

  rsult = FALSE;
  if (chosenWindow == (Nlm_WindoW) w) {
    rsult = Nlm_CommonClick (w, pt, FALSE, FALSE, FALSE, FALSE);
    if (! rsult) {
      Nlm_GetWindowData ((Nlm_WindoW) w, &wdata);
      cls = wdata.close;
      if (cls != NULL) {
        wptr = Nlm_ParentWindowPtr (w);
        GetPort (&temp);
        GetPenState (&state);
        SetPort (wptr);
        Nlm_currentWindowTool = wptr;
        Nlm_SetUpdateRegion (wptr);
        Nlm_ResetDrawingTools ();
        cls ((Nlm_WindoW) w);
        SetPort (temp);
        Nlm_currentWindowTool = temp;
        Nlm_SetUpdateRegion (temp);
        SetPenState (&state);
      }
    }
  } else {
    Nlm_Beep ();
  }
  return rsult;
}

static Nlm_Boolean Nlm_FloatingClick (Nlm_GraphiC w, Nlm_PoinT pt)

{
  Nlm_WindoW  nw;

  if (chosenWindow == (Nlm_WindoW) w) {
    Nlm_CommonClick (w, Nlm_globalMouse, TRUE, TRUE, FALSE, FALSE);
  } else {
    nw = Nlm_NextVisWindow ((Nlm_WindoW) w);
    Nlm_DoClick ((Nlm_GraphiC) nw, Nlm_globalMouse);
  }
  return TRUE;
}

static Nlm_Boolean Nlm_SystemClick (Nlm_GraphiC w, Nlm_PoinT pt)

{
  Nlm_WindowTool  wptr;

  if (chosenWindow == (Nlm_WindoW) w) {
    wptr = Nlm_ParentWindowPtr (w);
    SystemClick (&Nlm_currentEvent, wptr);
  } else {
    Nlm_DoSelect ((Nlm_GraphiC) chosenWindow, TRUE);
  }
  return TRUE;
}

static Nlm_Boolean Nlm_DesktopClick (Nlm_GraphiC w, Nlm_PoinT pt)

{
  Nlm_MenuBaR  mb;

  mb = Nlm_GetWindowMenuBar ((Nlm_WindoW) w);
  Nlm_localMouse = Nlm_globalMouse;
  Nlm_GlobalToLocal (&Nlm_localMouse);
  (void) (Nlm_GetEnabled ((Nlm_GraphiC) mb) && Nlm_GetVisible ((Nlm_GraphiC) mb) &&
          Nlm_DoClick ((Nlm_GraphiC) mb, Nlm_localMouse));
  return TRUE;
}
#endif

static void Nlm_NormalSelect (Nlm_GraphiC w, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  if ((! Nlm_DragClick (w, Nlm_localMouse)) || (! Nlm_cmmdKey)) {
    Nlm_SelectWindow (w, FALSE);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_SelectWindow (w, FALSE);
#endif
#ifdef WIN_MOTIF
  Nlm_SelectWindow (w, FALSE);
#endif
}

static void Nlm_PlainSelect (Nlm_GraphiC w, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_SelectWindow (w, FALSE);
#endif
#ifdef WIN_MSWIN
  Nlm_SelectWindow (w, FALSE);
#endif
#ifdef WIN_MOTIF
  Nlm_SelectWindow (w, FALSE);
#endif
}

static void Nlm_FloatingSelect (Nlm_GraphiC w, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  PenState        state;
  Nlm_PortTool    temp;
  Nlm_WindowTool  wptr;

  wptr = Nlm_ParentWindowPtr (w);
  GetPort (&temp);
  GetPenState (&state);
  SetPort (wptr);
  Nlm_currentWindowTool = wptr;
  Nlm_SetUpdateRegion (wptr);
  Nlm_ResetDrawingTools ();
  Nlm_localMouse = Nlm_globalMouse;
  Nlm_GlobalToLocal (&Nlm_localMouse);
  if (Nlm_DragClick (w, Nlm_globalMouse) || Nlm_CloseClick (w, Nlm_globalMouse)) {
  } else {
    Nlm_ContentClick (w, Nlm_globalMouse);
  }
  SetPort (temp);
  Nlm_currentWindowTool = temp;
  Nlm_SetUpdateRegion (temp);
  SetPenState (&state);
  Nlm_localMouse = Nlm_globalMouse;
  Nlm_GlobalToLocal (&Nlm_localMouse);
#endif
#ifdef WIN_MSWIN
  Nlm_SelectWindow (w, FALSE);
#endif
#ifdef WIN_MOTIF
  Nlm_SelectWindow (w, FALSE);
#endif
}

static void Nlm_DesktopSelect (Nlm_GraphiC w, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  PenState      state;
  Nlm_PortTool  temp;

  GetPort (&temp);
  GetPenState (&state);
  PenNormal ();
  Nlm_DesktopClick (w, Nlm_globalMouse);
  SetPort (temp);
  Nlm_currentWindowTool = temp;
  Nlm_SetUpdateRegion (temp);
  SetPenState (&state);
#endif
}

#ifdef WIN_MAC
static Nlm_Boolean Nlm_DesktopKey (Nlm_GraphiC w, Nlm_Char ch)

{
  Nlm_MenuBaR  mb;

  mb = Nlm_GetWindowMenuBar ((Nlm_WindoW) w);
  Nlm_localMouse = Nlm_globalMouse;
  Nlm_GlobalToLocal (&Nlm_localMouse);
  if (Nlm_currentKey != '\0') {
    (void) (Nlm_GetEnabled ((Nlm_GraphiC) mb) && Nlm_GetVisible ((Nlm_GraphiC) mb) &&
            Nlm_DoKey ((Nlm_GraphiC) mb, ch));
  }
  return TRUE;
}

static Nlm_Boolean Nlm_NormalKey (Nlm_GraphiC w, Nlm_Char ch)

{
  Nlm_GraphiC     g;
  Nlm_MenuBaR     mb;
  Nlm_GraphiC     n;
  Nlm_Boolean     notInside;
  PenState        state;
  Nlm_PortTool    temp;
  Nlm_WindowTool  wptr;

  wptr = Nlm_ParentWindowPtr (w);
  GetPort (&temp);
  GetPenState (&state);
  SetPort (wptr);
  Nlm_currentWindowTool = wptr;
  Nlm_SetUpdateRegion (wptr);
  Nlm_ResetDrawingTools ();
  Nlm_localMouse = Nlm_globalMouse;
  Nlm_GlobalToLocal (&Nlm_localMouse);
  if (Nlm_currentKey != '\0') {
    g = Nlm_GetChild (w);
    notInside = TRUE;
    mb = Nlm_GetWindowMenuBar ((Nlm_WindoW) w);
    if (mb != NULL) {
      if (Nlm_GetEnabled ((Nlm_GraphiC) mb) && Nlm_GetVisible ((Nlm_GraphiC) mb) &&
          Nlm_DoKey ((Nlm_GraphiC) mb, ch)) {
        notInside = FALSE;
      }
    }
    while (g != NULL && notInside) {
      n = Nlm_GetNext (g);
      if (Nlm_GetEnabled (g) && Nlm_GetVisible (g) && Nlm_DoKey (g, ch)) {
        notInside = FALSE;
      }
      g = n;
    }
    if (notInside && Nlm_cmmdKey) {
      Nlm_DesktopKey ((Nlm_GraphiC) Nlm_desktopWindow, ch);
    }
  }
  SetPort (temp);
  Nlm_currentWindowTool = temp;
  Nlm_SetUpdateRegion (temp);
  SetPenState (&state);
  return TRUE;
}

static Nlm_Boolean Nlm_FloatingKey (Nlm_GraphiC w, Nlm_Char ch)

{
  Nlm_GraphiC     g;
  Nlm_MenuBaR     mb;
  Nlm_GraphiC     n;
  Nlm_WindoW      nw;
  Nlm_Boolean     notInside;
  PenState        state;
  Nlm_PortTool    temp;
  Nlm_WindowTool  wptr;

  wptr = Nlm_ParentWindowPtr (w);
  GetPort (&temp);
  GetPenState (&state);
  SetPort (wptr);
  Nlm_currentWindowTool = wptr;
  Nlm_SetUpdateRegion (wptr);
  Nlm_ResetDrawingTools ();
  Nlm_localMouse = Nlm_globalMouse;
  Nlm_GlobalToLocal (&Nlm_localMouse);
  if (Nlm_currentKey != '\0') {
    g = Nlm_GetChild (w);
    notInside = TRUE;
    mb = Nlm_GetWindowMenuBar ((Nlm_WindoW) w);
    if (mb != NULL) {
      if (Nlm_GetEnabled ((Nlm_GraphiC) mb) && Nlm_GetVisible ((Nlm_GraphiC) mb) &&
          Nlm_DoKey ((Nlm_GraphiC) mb, ch)) {
        notInside = FALSE;
      }
    }
    while (g != NULL && notInside) {
      n = Nlm_GetNext (g);
      if (Nlm_GetEnabled (g) && Nlm_GetVisible (g) && Nlm_DoKey (g, ch)) {
        notInside = FALSE;
      }
      g = n;
    }
    if (notInside) {
      nw = Nlm_NextVisWindow ((Nlm_WindoW) w);
      Nlm_DoKey ((Nlm_GraphiC) nw, ch);
    }
  }
  SetPort (temp);
  Nlm_currentWindowTool = temp;
  Nlm_SetUpdateRegion (temp);
  SetPenState (&state);
  return TRUE;
}

static Nlm_Boolean Nlm_ModalKey (Nlm_GraphiC w, Nlm_Char ch)

{
  if (Nlm_cmmdKey) {
    Nlm_Beep ();
  } else {
    Nlm_NormalKey (w, ch);
  }
  return TRUE;
}

static void Nlm_DrawWindow (Nlm_GraphiC w, Nlm_Boolean drawGrowIcon)

{
  Nlm_GraphiC     g;
  Nlm_MenuBaR     mb;
  Nlm_GraphiC     n;
  Nlm_RecT        r;
  PenState        state;
  Nlm_PortTool    temp;
  Nlm_WindowTool  wptr;

  wptr = Nlm_ParentWindowPtr (w);
  GetPort (&temp);
  GetPenState (&state);
  SetPort (wptr);
  Nlm_currentWindowTool = wptr;

  Nlm_ResetDrawingTools ();
  Nlm_ResetClip ();
  PenNormal ();

  BeginUpdate (wptr);
  Nlm_SetUpdateRegion (wptr);
  Nlm_RectToolToRecT (&(wptr->portRect), &r);
  Nlm_EraseRect (&r);
  if (drawGrowIcon) {
    Nlm_DrawGrowIcon (w);
  }
  mb = Nlm_GetWindowMenuBar ((Nlm_WindoW) w);
  if (mb != NULL) {
    Nlm_DoDraw ((Nlm_GraphiC) mb);
  }
  g = Nlm_GetChild (w);
  while (g != NULL) {
    n = Nlm_GetNext (g);
    Nlm_DoDraw (g);
    g = n;
  }
  EndUpdate (wptr);
  Nlm_ResetDrawingTools ();
  Nlm_ResetClip ();
  SetPort (temp);
  Nlm_currentWindowTool = temp;
  Nlm_SetUpdateRegion (temp);
  SetPenState (&state);
}

static void Nlm_DocumentDraw (Nlm_GraphiC w)

{
  Nlm_DrawWindow (w, TRUE);
}

static void Nlm_NormalDraw (Nlm_GraphiC w)

{
  Nlm_DrawWindow (w, FALSE);
}
#endif

static void Nlm_ActivateWindow (Nlm_GraphiC w, Nlm_Boolean drawGrow)

{
  Nlm_WndActnProc  act;
  Nlm_GraphiC      g;
  Nlm_MenuBaR      mb;
  Nlm_GraphiC      n;
  Nlm_WindowData   wdata;

  Nlm_UseWindow ((Nlm_WindoW) w);
#ifdef WIN_MAC
  if (drawGrow) {
    Nlm_DrawGrowIcon (w);
  }
#endif
  mb = Nlm_GetWindowMenuBar ((Nlm_WindoW) w);
  if (mb != NULL) {
    Nlm_DoActivate ((Nlm_GraphiC) mb, FALSE);
  }
  g = Nlm_GetChild (w);
  while (g != NULL) {
    n = Nlm_GetNext (g);
    Nlm_DoActivate (g, FALSE);
    g = n;
  }
  Nlm_GetWindowData ((Nlm_WindoW) w, &wdata);
  act = wdata.activate;
  if (act != NULL) {
    act ((Nlm_WindoW) w);
  }
}

static void Nlm_DocumentActivate (Nlm_GraphiC w, Nlm_Boolean savePort)

{
  Nlm_ActivateWindow (w, TRUE);
}

static void Nlm_NormalActivate (Nlm_GraphiC w, Nlm_Boolean savePort)

{
  Nlm_ActivateWindow (w, FALSE);
}

static void Nlm_DeactivateWindow (Nlm_GraphiC w, Nlm_Boolean drawGrow)

{
  Nlm_WndActnProc  deact;
  Nlm_GraphiC      g;
  Nlm_MenuBaR      mb;
  Nlm_GraphiC      n;
  Nlm_WindowData   wdata;

  Nlm_UseWindow ((Nlm_WindoW) w);
#ifdef WIN_MAC
  if (drawGrow) {
    Nlm_DrawGrowIcon (w);
  }
#endif
  mb = Nlm_GetWindowMenuBar ((Nlm_WindoW) w);
  if (mb != NULL) {
    Nlm_DoDeactivate ((Nlm_GraphiC) mb, FALSE);
  }
  g = Nlm_GetChild (w);
  while (g != NULL) {
    n = Nlm_GetNext (g);
    Nlm_DoDeactivate (g, FALSE);
    g = n;
  }
  Nlm_GetWindowData ((Nlm_WindoW) w, &wdata);
  deact = wdata.deactivate;
  if (deact != NULL) {
    deact ((Nlm_WindoW) w);
  }
}

static void Nlm_DocumentDeactivate (Nlm_GraphiC w, Nlm_Boolean savePort)

{
  Nlm_DeactivateWindow (w, TRUE);
}

static void Nlm_NormalDeactivate (Nlm_GraphiC w, Nlm_Boolean savePort)

{
  Nlm_DeactivateWindow (w, FALSE);
}

#ifdef WIN_MAC
static Nlm_Boolean Nlm_NormalIdle (Nlm_GraphiC w, Nlm_PoinT pt)

{
  Nlm_GraphiC  g;
  Nlm_GraphiC  n;

  g = Nlm_GetChild (w);
  while (g != NULL) {
    n = Nlm_GetNext (g);
    Nlm_DoIdle (g, Nlm_localMouse);
    g = n;
  }
  return TRUE;
}

static Nlm_Boolean Nlm_FloatingIdle (Nlm_GraphiC w, Nlm_PoinT pt)

{
  Nlm_WindoW   nw;

  nw = Nlm_NextVisWindow ((Nlm_WindoW) w);
  Nlm_DoIdle ((Nlm_GraphiC) nw, pt);
  return TRUE;
}
#endif

static void Nlm_NormalDrawChar (Nlm_GraphiC w, Nlm_Char ch, Nlm_Boolean savePort)

{
  Nlm_DisplaY     d;
  Nlm_WindowData  wdata;

  Nlm_GetWindowData ((Nlm_WindoW) w, &wdata);
  d = wdata.charDisplay;
  if (d != NULL) {
    Nlm_DoSendChar ((Nlm_GraphiC) d, ch, TRUE);
  }
}

static void Nlm_FloatingDrawChar (Nlm_GraphiC w, Nlm_Char ch, Nlm_Boolean savePort)

{
  Nlm_DisplaY     d;
  Nlm_WindowData  wdata;
#ifdef WIN_MAC
  Nlm_WindoW      nw;
#endif

  Nlm_GetWindowData ((Nlm_WindoW) w, &wdata);
  d = wdata.charDisplay;
  if (d != NULL) {
    Nlm_DoSendChar ((Nlm_GraphiC) d, ch, TRUE);
  } else {
#ifdef WIN_MAC
    nw = Nlm_NextVisWindow ((Nlm_WindoW) w);
    Nlm_DoSendChar ((Nlm_GraphiC) nw, ch, TRUE);
#endif
#ifdef WIN_MOTIF
#endif
  }
}

static void Nlm_AdjustWindow (Nlm_GraphiC w, Nlm_RectPtr r,
                              Nlm_Boolean align, Nlm_Boolean savePort)

{
  Nlm_GraphiC  g;
  Nlm_WindoW   tempPort;

  tempPort = Nlm_SavePortIfNeeded (w, savePort);
  if (align) {
    g = Nlm_GetParent (w);
    Nlm_RecordRect (g, r);
    Nlm_NextPosition (g, r);
  }
  Nlm_RestorePort (tempPort);
}

static Nlm_GraphiC Nlm_WindowGainFocus (Nlm_GraphiC w, Nlm_Char ch, Nlm_Boolean savePort)

{
  Nlm_GraphiC  p;
  Nlm_GraphiC  q;

  q = NULL;
  p = Nlm_GetChild (w);
  while (p != NULL && q == NULL) {
    q = Nlm_DoGainFocus (p, ch, savePort);
    p = Nlm_GetNext (p);
  }
  return q;
}

static void Nlm_WindowLoseFocus (Nlm_GraphiC w, Nlm_GraphiC excpt, Nlm_Boolean savePort)

{
  Nlm_GraphiC  p;

  p = Nlm_GetChild (w);
  while (p != NULL) {
    Nlm_DoLoseFocus (p, excpt, savePort);
    p = Nlm_GetNext (p);
  }
}

extern Nlm_WindoW Nlm_DocumentWindow (Nlm_Int2 left, Nlm_Int2 top,
                                      Nlm_Int2 width, Nlm_Int2 height,
                                      Nlm_CharPtr title,
                                      Nlm_WndActnProc close,
                                      Nlm_WndActnProc resize)

{
  Nlm_RecT    r;
  Nlm_WindoW  w;

  Nlm_LoadRect (&r, left, top, width, height);
  w = Nlm_MakeWindowLink (&r, sizeof (Nlm_WindowRec), documentProcs);
  if (w != NULL) {
#ifdef WIN_MAC
    Nlm_NewWindow (w, DOCUMENT_STYLE, 8, TRUE, NULL, 0, title, close, resize);
#endif
#ifdef WIN_MSWIN
    Nlm_NewWindow (w, DOCUMENT_STYLE, 0, FALSE, windowclass, WS_OVERLAPPEDWINDOW, title, close, resize);
#endif
#ifdef WIN_MOTIF
    Nlm_NewWindow (w, DOCUMENT_STYLE, 0, FALSE, NULL, 0, title, close, resize);
#endif
  }
  return w;
}

extern Nlm_WindoW Nlm_FixedWindow (Nlm_Int2 left, Nlm_Int2 top,
                                   Nlm_Int2 width, Nlm_Int2 height,
                                   Nlm_CharPtr title, Nlm_WndActnProc close)

{
  Nlm_RecT    r;
  Nlm_WindoW  w;

  Nlm_LoadRect (&r, left, top, width, height);
  w = Nlm_MakeWindowLink (&r, sizeof (Nlm_WindowRec), fixedProcs);
  if (w != NULL) {
#ifdef WIN_MAC
    Nlm_NewWindow (w, FIXED_STYLE, 4, TRUE, NULL, 0, title, close, NULL);
#endif
#ifdef WIN_MSWIN
    Nlm_NewWindow (w, FIXED_STYLE, 0, FALSE, windowclass, WS_OVERLAPPED |
                   WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, title, close, NULL);
#endif
#ifdef WIN_MOTIF
    Nlm_NewWindow (w, FIXED_STYLE, 0, FALSE, NULL, 0, title, close, NULL);
#endif
  }
  return w;
}

extern Nlm_WindoW Nlm_FrozenWindow (Nlm_Int2 left, Nlm_Int2 top,
                                    Nlm_Int2 width, Nlm_Int2 height,
                                    Nlm_CharPtr title, Nlm_WndActnProc close)

{
  Nlm_RecT    r;
  Nlm_WindoW  w;

  Nlm_LoadRect (&r, left, top, width, height);
  w = Nlm_MakeWindowLink (&r, sizeof (Nlm_WindowRec), frozenProcs);
  if (w != NULL) {
#ifdef WIN_MAC
    Nlm_NewWindow (w, FROZEN_STYLE, 4, TRUE, NULL, 0, title, close, NULL);
#endif
#ifdef WIN_MSWIN
    Nlm_NewWindow (w, FROZEN_STYLE, 0, FALSE, windowclass, WS_OVERLAPPED |
                   WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, title, close, NULL);
#endif
#ifdef WIN_MOTIF
    Nlm_NewWindow (w, FROZEN_STYLE, 0, FALSE, NULL, 0, title, close, NULL);
#endif
  }
  return w;
}

extern Nlm_WindoW Nlm_RoundWindow (Nlm_Int2 left, Nlm_Int2 top,
                                   Nlm_Int2 width, Nlm_Int2 height,
                                   Nlm_CharPtr title, Nlm_WndActnProc close)

{
  Nlm_RecT    r;
  Nlm_WindoW  w;

  Nlm_LoadRect (&r, left, top, width, height);
  w = Nlm_MakeWindowLink (&r, sizeof (Nlm_WindowRec), roundProcs);
  if (w != NULL) {
#ifdef WIN_MAC
    Nlm_NewWindow (w, ROUND_STYLE, 16, TRUE, NULL, 0, title, close, NULL);
#endif
#ifdef WIN_MSWIN
    Nlm_NewWindow (w, ROUND_STYLE, 0, FALSE, windowclass, WS_OVERLAPPED |
                   WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, title, close, NULL);
#endif
#ifdef WIN_MOTIF
    Nlm_NewWindow (w, ROUND_STYLE, 0, FALSE, NULL, 0, title, close, NULL);
#endif
  }
  return w;
}

extern Nlm_WindoW Nlm_AlertWindow (Nlm_Int2 left, Nlm_Int2 top,
                                   Nlm_Int2 width, Nlm_Int2 height,
                                   Nlm_WndActnProc close)

{
  Nlm_RecT    r;
  Nlm_WindoW  w;

  Nlm_LoadRect (&r, left, top, width, height);
  w = Nlm_MakeWindowLink (&r, sizeof (Nlm_WindowRec), alertProcs);
  if (w != NULL) {
#ifdef WIN_MAC
    Nlm_NewWindow (w, ALERT_STYLE, 1, FALSE, NULL, 0, NULL, close, NULL);
#endif
#ifdef WIN_MSWIN
    Nlm_NewWindow (w, ALERT_STYLE, 0, FALSE, windowclass, WS_OVERLAPPED, "", close, NULL);
#endif
#ifdef WIN_MOTIF
    Nlm_NewWindow (w, ALERT_STYLE, 0, FALSE, NULL, 0, "", close, NULL);
#endif
  }
  return w;
}

extern Nlm_WindoW Nlm_ModalWindow (Nlm_Int2 left, Nlm_Int2 top,
                                   Nlm_Int2 width, Nlm_Int2 height,
                                   Nlm_WndActnProc close)

{
  Nlm_RecT    r;
  Nlm_WindoW  w;

  Nlm_LoadRect (&r, left, top, width, height);
  w = Nlm_MakeWindowLink (&r, sizeof (Nlm_WindowRec), modalProcs);
  if (w != NULL) {
#ifdef WIN_MAC
    Nlm_NewWindow (w, MODAL_STYLE, 1, FALSE, NULL, 0, NULL, close, NULL);
#endif
#ifdef WIN_MSWIN
    Nlm_NewWindow (w, MODAL_STYLE, 0, FALSE, windowclass, WS_POPUP |
                   WS_CAPTION | WS_SYSMENU, "", close, NULL);
#endif
#ifdef WIN_MOTIF
    Nlm_NewWindow (w, MODAL_STYLE, 0, FALSE, NULL, 0, "", close, NULL);
#endif
  }
  return w;
}

extern Nlm_WindoW Nlm_FloatingWindow (Nlm_Int2 left, Nlm_Int2 top,
                                      Nlm_Int2 width, Nlm_Int2 height,
                                      Nlm_WndActnProc close)

{
  Nlm_RecT    r;
  Nlm_WindoW  w;

  Nlm_LoadRect (&r, left, top, width, height);
  w = Nlm_MakeWindowLink (&r, sizeof (Nlm_WindowRec), floatingProcs);
  if (w != NULL) {
#ifdef WIN_MAC
    Nlm_NewWindow (w, FLOATING_STYLE, 17, TRUE, NULL, 0, NULL, close, NULL);
#endif
#ifdef WIN_MSWIN
    Nlm_NewWindow (w, FLOATING_STYLE, 0, FALSE, windowclass, WS_POPUP |
                   WS_CAPTION | WS_SYSMENU, "", close, NULL);
#endif
#ifdef WIN_MOTIF
    Nlm_NewWindow (w, FLOATING_STYLE, 0, FALSE, NULL, 0, "", close, NULL);
#endif
  }
  return w;
}

extern Nlm_WindoW Nlm_ShadowWindow (Nlm_Int2 left, Nlm_Int2 top,
                                    Nlm_Int2 width, Nlm_Int2 height,
                                    Nlm_WndActnProc close)

{
  Nlm_RecT    r;
  Nlm_WindoW  w;

  Nlm_LoadRect (&r, left, top, width, height);
  w = Nlm_MakeWindowLink (&r, sizeof (Nlm_WindowRec), shadowProcs);
  if (w != NULL) {
#ifdef WIN_MAC
    Nlm_NewWindow (w, SHADOW_STYLE, 3, FALSE, NULL, 0, NULL, close, NULL);
#endif
#ifdef WIN_MSWIN
    Nlm_NewWindow (w, SHADOW_STYLE, 0, FALSE, windowclass, WS_OVERLAPPED, "", close, NULL);
#endif
#ifdef WIN_MOTIF
    Nlm_NewWindow (w, SHADOW_STYLE, 0, FALSE, NULL, 0, "", close, NULL);
#endif
  }
  return w;
}

extern Nlm_WindoW Nlm_PlainWindow (Nlm_Int2 left, Nlm_Int2 top,
                                   Nlm_Int2 width, Nlm_Int2 height,
                                   Nlm_WndActnProc close)

{
  Nlm_RecT    r;
  Nlm_WindoW  w;

  Nlm_LoadRect (&r, left, top, width, height);
  w = Nlm_MakeWindowLink (&r, sizeof (Nlm_WindowRec), plainProcs);
  if (w != NULL) {
#ifdef WIN_MAC
    Nlm_NewWindow (w, PLAIN_STYLE, 2, FALSE, NULL, 0, NULL, close, NULL);
#endif
#ifdef WIN_MSWIN
    Nlm_NewWindow (w, PLAIN_STYLE, 0, FALSE, windowclass, WS_OVERLAPPED, "", close, NULL);
#endif
#ifdef WIN_MOTIF
    Nlm_NewWindow (w, PLAIN_STYLE, 0, FALSE, NULL, 0, "", close, NULL);
#endif
  }
  return w;
}

#ifdef WIN_MOTIF
static void Nlm_SetCursor (Cursor cursor)

{
  Nlm_MainTool  man;
  Nlm_WindoW    w;
  Window        xtw;

  if (Nlm_currentXDisplay != NULL) {
    w = (Nlm_WindoW) Nlm_GetNext ((Nlm_GraphiC) Nlm_desktopWindow);
    while (w != NULL) {
      if (! Nlm_IsWindowDying (w)) {
        man = Nlm_GetWindowMain (w);
        xtw = XtWindow (man);
        if (xtw != 0) {
          XDefineCursor (Nlm_currentXDisplay, xtw, cursor);
        }
      }
      w = (Nlm_WindoW) Nlm_GetNext ((Nlm_GraphiC) w);
    }
    currentCursor = cursor;
    XSync (Nlm_currentXDisplay, FALSE);
  }
}
#endif

extern void Nlm_ArrowCursor (void)

{
#ifdef WIN_MAC
  SetCursor (&qd.arrow);
#endif
#ifdef WIN_MSWIN
  currentCursor = LoadCursor (NULL, IDC_ARROW);
  SetCursor (currentCursor);
#endif
#ifdef WIN_MOTIF
  Nlm_SetCursor (arrow);
#endif
}

extern void Nlm_CrossCursor (void)

{
#ifdef WIN_MAC
  SetCursor (&cross);
#endif
#ifdef WIN_MSWIN
  currentCursor = LoadCursor (NULL, IDC_CROSS);
  SetCursor (currentCursor);
#endif
#ifdef WIN_MOTIF
  Nlm_SetCursor (cross);
#endif
}

extern void Nlm_IBeamCursor (void)

{
#ifdef WIN_MAC
  SetCursor (&iBeam);
#endif
#ifdef WIN_MSWIN
  currentCursor = LoadCursor (NULL, IDC_IBEAM);
  SetCursor (currentCursor);
#endif
#ifdef WIN_MOTIF
  Nlm_SetCursor (iBeam);
#endif
}

extern void Nlm_PlusCursor (void)

{
#ifdef WIN_MAC
  SetCursor (&plus);
#endif
#ifdef WIN_MSWIN
  currentCursor = LoadCursor (NULL, IDC_CROSS);
  SetCursor (currentCursor);
#endif
#ifdef WIN_MOTIF
  Nlm_SetCursor (plus);
#endif
}

extern void Nlm_WatchCursor (void)

{
#ifdef WIN_MAC
  SetCursor (&watch);
#endif
#ifdef WIN_MSWIN
  currentCursor = LoadCursor (NULL, IDC_WAIT);
  SetCursor (currentCursor);
#endif
#ifdef WIN_MOTIF
  Nlm_SetCursor (watch);
#endif
}

extern void Nlm_ProgramPath (Nlm_CharPtr buf, size_t maxsize)

{
#ifdef WIN_MAC
  WDPBRec      block;
  Nlm_Char     directory [PATH_MAX];
  Nlm_Int4     dirID;
  OSErr        err;
  CInfoPBRec   params;
  Nlm_Char     temp [PATH_MAX];
  Nlm_CharPtr  tmp;
  short        vRefNum;
#endif
#ifdef WIN_MSWIN
  Nlm_Char     path [PATH_MAX];
#endif
#ifdef WIN_MOTIF
  Nlm_Char     path [PATH_MAX];
  Nlm_CharPtr  pth;
  Nlm_CharPtr  ptr;
#endif

  if (buf != NULL && maxsize > 0) {
#ifdef WIN_MAC
    memset (&block, 0, sizeof (WDPBRec));
    block.ioNamePtr = NULL;
    block.ioVRefNum = apRefNum;
    block.ioWDIndex = 0;
    block.ioWDProcID = 0;
    PBGetWDInfo (&block, FALSE);
    dirID = block.ioWDDirID;
    vRefNum = block.ioWDVRefNum;
    temp [0] = '\0';
    params.dirInfo.ioNamePtr = (StringPtr) directory;
    params.dirInfo.ioDrParID = dirID;
    do {
      params.dirInfo.ioVRefNum = vRefNum;
      params.dirInfo.ioFDirIndex = -1;
      params.dirInfo.ioDrDirID = params.dirInfo.ioDrParID;
      err = PBGetCatInfo (&params, FALSE);
#ifdef COMP_THINKC
      PtoCstr ((StringPtr) directory);
#endif
#ifdef COMP_MPW
      p2cstr ((StringPtr) directory);
#endif
      Nlm_StringCat (directory, DIRDELIMSTR);
      Nlm_StringCat (directory, temp);
      Nlm_StringCpy (temp, directory);
    } while (params.dirInfo.ioDrDirID != fsRtDirID);
    tmp = Nlm_StringMove (directory, temp);
    tmp = Nlm_StringMove (tmp, (Nlm_CharPtr) apName);
    Nlm_StringNCpy (buf, directory, maxsize);
#endif
#ifdef WIN_MSWIN
    if (GetModuleFileName (Nlm_currentHInst, path, sizeof (path)) > 0) {
      Nlm_StringNCpy (buf, path, maxsize);
    } else {
      *buf = '\0';
    }
#endif
#ifdef OS_UNIX
    ptr = statargv [0];
    if (ptr [0] == DIRDELIMCHR) {
      Nlm_StringNCpy (buf, statargv [0], maxsize);
    } else if (getcwd (path, sizeof (path)) != NULL) {
      ptr = statargv [0];
      while (ptr [0] == '.' || ptr [0] == DIRDELIMCHR) {
        if (ptr [0] == '.') {
          if (ptr [1] == '.' && ptr [2] == DIRDELIMCHR) {
            ptr += 3;
            pth = StringRChr (path, DIRDELIMCHR);
            if (pth != NULL) {
              *pth = '\0';
            }
          } else if (ptr [1] == DIRDELIMCHR) {
            ptr += 2;
          } else {
            ptr++;
          }
        } else if (ptr [0] == DIRDELIMCHR) {
          ptr++;
        } else {
          ptr++;
        }
      }
      FileBuildPath (path, NULL, ptr);
      Nlm_StringNCpy (buf, path, maxsize);
    } else {
      Nlm_StringNCpy (buf, statargv [0], maxsize);
    }
#endif
#ifdef OS_VMS
    if (statargv != NULL && statargv [0] != NULL) {
      Nlm_StringNCpy (buf, statargv [0], maxsize);
    }
#endif
  }
}

#ifdef WIN_MAC
static void Nlm_HandleEvent (void)

{
  Nlm_Int2        key;
  Nlm_PointTool   ptool;
  Nlm_PoinT       where;
  Nlm_Int2        windowLoc;
  Nlm_WindowTool  wptr;

  Nlm_PointToolToPoinT (Nlm_currentEvent.where, &Nlm_globalMouse);
  Nlm_localMouse = Nlm_globalMouse;
  Nlm_GlobalToLocal (&Nlm_localMouse);
  Nlm_currentKey = '\0';
  key = Nlm_currentEvent.modifiers;
  Nlm_cmmdKey = ((key & 256) != 0);
  Nlm_ctrlKey = FALSE;
  Nlm_shftKey = ((key & 512) != 0);
  Nlm_optKey = ((key & 2048) != 0);
  Nlm_dblClick = FALSE;
  frontWindow = Nlm_FindWindowRec (FrontWindow ());
  Nlm_theWindow = Nlm_FindWindowRec ((Nlm_WindowTool) Nlm_currentEvent.message);
  chosenWindow = Nlm_theWindow;
  switch (Nlm_currentEvent.what) {
    case mouseDown:
      Nlm_PoinTToPointTool (Nlm_globalMouse, &ptool);
      windowLoc = FindWindow (ptool, &wptr);
      chosenWindow = Nlm_FindWindowRec (wptr);
      Nlm_DoClick ((Nlm_GraphiC) frontWindow, Nlm_globalMouse);
      break;
    case keyDown:
      Nlm_currentKey = (Nlm_Char) (Nlm_currentEvent.message % 256);
      if (keyAction != NULL) {
        keyAction (Nlm_currentKey);
      }
      Nlm_DoKey ((Nlm_GraphiC) frontWindow, Nlm_currentKey);
      break;
    case autoKey:
      if (! Nlm_cmmdKey) {
        Nlm_currentKey = (Nlm_Char) (Nlm_currentEvent.message % 256);
        Nlm_DoKey ((Nlm_GraphiC) frontWindow, Nlm_currentKey);
      }
      break;
    case updateEvt:
      Nlm_DoDraw ((Nlm_GraphiC) Nlm_theWindow);
      break;
    case osEvt:
      if (((Nlm_currentEvent.message & osEvtMessageMask) >> 24) == suspendResumeMessage) {
        if (Nlm_currentEvent.message & resumeFlag) {
          if (Nlm_currentEvent.message & convertClipboardFlag) {
            TEFromScrap ();
          }
        } else {
          ZeroScrap ();
          TEToScrap ();
        }
      }
      break;
    case activateEvt:
      if (Nlm_currentEvent.modifiers & 01) {
        Nlm_DoActivate ((Nlm_GraphiC) Nlm_theWindow, FALSE);
      } else {
        Nlm_DoDeactivate ((Nlm_GraphiC) Nlm_theWindow, FALSE);
      }
      break;
    case diskEvt:
      if (HiWord (Nlm_currentEvent.message) != 0) {
        where.x = 90;
        where.y = 100;
        Nlm_PoinTToPointTool (where, &ptool);
        DIBadMount (ptool, Nlm_currentEvent.message);
      }
      break;
    default:
      break;
  }
}

extern Nlm_Boolean Nlm_RegisterWindows (void)

{
  return TRUE;
}

static void Nlm_ReturnCursor (Cursor *cursor, Nlm_Int2 cursorID)

{
  CursHandle  hCurs;
  CursPtr     pCurs;

  hCurs = GetCursor (cursorID);
  if (hCurs != NULL) {
    HLock ((Handle) hCurs);
    pCurs = (CursPtr) *((Handle) hCurs);
    *cursor = *pCurs;
    HUnlock ((Handle) hCurs);
  } else {
    *cursor = qd.arrow;
  }
}

static Nlm_Boolean Nlm_SetupWindows (void)

{
  Nlm_PoinT  pt;
  Nlm_RecT   r;
  SysEnvRec  sysenv;

  Nlm_ReturnCursor (&cross, 2);
  Nlm_ReturnCursor (&iBeam, 1);
  Nlm_ReturnCursor (&plus, 3);
  Nlm_ReturnCursor (&watch, 4);
  Nlm_WatchCursor ();
  Nlm_ClearKeys ();
  Nlm_LoadPt (&pt, 0, 0);
  Nlm_RectToolToRecT (&(qd.screenBits.bounds), &screenBitBounds);
  r = screenBitBounds;
  Nlm_screenRect = screenBitBounds;
  Nlm_desktopWindow = (Nlm_WindoW) Nlm_HandNew (sizeof (Nlm_WindowRec));
  Nlm_systemWindow = (Nlm_WindoW) Nlm_HandNew (sizeof (Nlm_WindowRec));
  Nlm_LoadGraphicData ((Nlm_GraphiC) Nlm_desktopWindow, (Nlm_GraphiC) Nlm_systemWindow,
                       NULL, NULL, NULL, desktopProcs, NULL, &r, TRUE, TRUE);
  Nlm_LoadBoxData ((Nlm_BoX) Nlm_desktopWindow, pt, pt, pt, 0, 0, 0, 0, 0, 0, 0, 0);
  Nlm_LoadWindowData (Nlm_desktopWindow, NULL, NULL, NULL, NULL, &r, &r, &r, NULL,
                      NULL, NULL, NULL, NULL, NULL, NULL, TRUE, FALSE, NULL, NULL, NULL);
  Nlm_LoadRect (&r, 0, 0, 0, 0);
  Nlm_LoadGraphicData ((Nlm_GraphiC) Nlm_systemWindow, NULL, NULL,
                       NULL, NULL, systemProcs, NULL, &r, TRUE, TRUE);
  Nlm_LoadBoxData ((Nlm_BoX) Nlm_systemWindow, pt, pt, pt, 0, 0, 0, 0, 0, 0, 0, 0);
  Nlm_LoadWindowData (Nlm_systemWindow, NULL, NULL, NULL, NULL, &r, &r, &r, NULL,
                      NULL, NULL, NULL, NULL, NULL, NULL, TRUE, FALSE, NULL, NULL, NULL);
  dyingWindow = NULL;
  frontWindow = Nlm_desktopWindow;
  theActiveWindow = Nlm_desktopWindow;
  chosenWindow = Nlm_desktopWindow;
  Nlm_theWindow = Nlm_desktopWindow;
  Nlm_currentWindowTool = NULL;
  quitProgram = FALSE;

  Nlm_SetUpDrawingTools ();

  Nlm_hScrollBarHeight = 16;
  Nlm_vScrollBarWidth = 16;

  Nlm_popupMenuHeight = Nlm_stdLineHeight + 4;
  Nlm_dialogTextHeight = Nlm_stdLineHeight + 4;
  
  lastTimerTime = Nlm_ComputerTime ();
  timerAction = NULL;
  keyAction = NULL;
  SysEnvirons (1, &sysenv);
  hasColorQD = sysenv.hasColorQD;
  return TRUE;
}
#endif

#ifdef WIN_MSWIN

/* Message cracker functions */

static void MyCls_OnMouseMove (HWND hwnd, int x, int y, UINT keyFlags)

{
  SetCursor (currentCursor);
}

static void MyCls_OnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)

{
  Nlm_GraphiC  g;
  Nlm_MenuBaR  mb;

  Nlm_currentId = id;
  Nlm_currentHwndCtl = hwndCtl;
  Nlm_currentCode = codeNotify;

  if (hwndCtl == NULL) {
    mb = Nlm_GetWindowMenuBar (Nlm_theWindow);
    Nlm_DoCommand ((Nlm_GraphiC) mb);
  } else {
    g = (Nlm_GraphiC) GetProp (hwndCtl, (LPSTR) "Nlm_VibrantProp");
    (void) (Nlm_GetEnabled (g) &&
            Nlm_GetVisible (g) &&
            Nlm_DoCommand (g));
  }
}

static void MyCls_OnHVScroll (HWND hwnd, HWND hwndCtl, UINT code, int pos)

{
  Nlm_GraphiC  g;

  Nlm_currentHwndCtl = hwndCtl;
  Nlm_currentCode = code;
  Nlm_currentPos = pos;

  if (hwndCtl != NULL) {
    g = (Nlm_GraphiC) GetProp (hwndCtl, (LPSTR) "Nlm_VibrantProp");
    (void) (Nlm_GetEnabled (g) &&
            Nlm_GetVisible (g) &&
            Nlm_DoCommand (g));
  }
}

static void MyCls_OnClose (HWND hwnd)

{
  Nlm_WndActnProc  cls;
  Nlm_WindowData   wdata;

  Nlm_GetWindowData (Nlm_theWindow, &wdata);
  cls = wdata.close;
  if (cls != NULL) {
    cls (Nlm_theWindow);
  }
}

static void MyCls_OnActivate (HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized)

{
  if (state != 0) {
    Nlm_DoActivate ((Nlm_GraphiC) Nlm_theWindow, FALSE);
  } else {
    Nlm_DoDeactivate ((Nlm_GraphiC) Nlm_theWindow, FALSE);
  }
}

static void MyCls_OnSize (HWND hwnd, UINT state, int cx, int cy)

{
  Nlm_RecT         r;
  Nlm_WndActnProc  resize;
  Nlm_RectTool     rtool;
  Nlm_WindowData   wdata;

  if (Nlm_WindowHasBeenShown (Nlm_theWindow) &&
      Nlm_GetVisible ((Nlm_GraphiC) Nlm_theWindow)) {
    Nlm_GetWindowData (Nlm_theWindow, &wdata);
    resize = wdata.resize;
    if (resize != NULL) {
      GetWindowRect (hwnd, &rtool);
      Nlm_RectToolToRecT (&rtool, &r);
      Nlm_SetRect ((Nlm_GraphiC) Nlm_theWindow, &r);
      resize (Nlm_theWindow);
    }
  }
}

static void MyCls_OnChar (HWND hwnd, UINT ch, int cRepeat)

{
  handlechar = FALSE;
  if (ch == '\t') {
    Nlm_DoSendFocus ((Nlm_GraphiC) Nlm_theWindow, (Nlm_Char) ch);
  } else if (ch == '\n' || ch == '\r') {
    Nlm_DoSendFocus ((Nlm_GraphiC) Nlm_theWindow, (Nlm_Char) ch);
  } else {
    handlechar = TRUE;
  }
}

/*
*  Note that the WM_SIZE message will trigger the resize callback whenever
*  a visible window is resized, including the initial sizing when a window
*  is first displayed.
*/

LRESULT CALLBACK EXPORT MainProc (HWND hwnd, UINT message,
                                  WPARAM wParam, LPARAM lParam)

{
  MINMAXINFO FAR*  lpmmi;
  HDC              tempHDC;
  HWND             tempHWnd;

  if (Nlm_VibrantDisabled ()) {
    return DefWindowProc (hwnd, message, wParam, lParam);
  }

  mainwndrsult = 0;
  tempHWnd = Nlm_currentHWnd;
  tempHDC = Nlm_currentHDC;
  Nlm_theWindow = (Nlm_WindoW) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  Nlm_currentHWnd = hwnd;
  Nlm_currentHDC = Nlm_GetWindowPort (Nlm_theWindow);
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
    case WM_MOUSEMOVE:
      HANDLE_WM_MOUSEMOVE (hwnd, wParam, lParam, MyCls_OnMouseMove);
      break;
    case WM_COMMAND:
      HANDLE_WM_COMMAND (hwnd, wParam, lParam, MyCls_OnCommand);
      break;
    case WM_HSCROLL:
      HANDLE_WM_HSCROLL (hwnd, wParam, lParam, MyCls_OnHVScroll);
      break;
    case WM_VSCROLL:
      HANDLE_WM_VSCROLL (hwnd, wParam, lParam, MyCls_OnHVScroll);
      break;
    case WM_CLOSE:
      HANDLE_WM_CLOSE (hwnd, wParam, lParam, MyCls_OnClose);
      break;
    case WM_ACTIVATE:
      mainwndrsult = DefWindowProc (hwnd, message, wParam, lParam);
      HANDLE_WM_ACTIVATE (hwnd, wParam, lParam, MyCls_OnActivate);
      break;
    case WM_SIZE:
      HANDLE_WM_SIZE (hwnd, wParam, lParam, MyCls_OnSize);
      break;
    case WM_GETMINMAXINFO:
      lpmmi = (MINMAXINFO FAR*) lParam;
      lpmmi->ptMaxTrackSize.x = 30000;
      lpmmi->ptMaxTrackSize.y = 30000;
      break;
    case WM_DESTROY:
      break;
#ifndef WIN32
    case WM_CTLCOLOR:
      if (lParam == CTLCOLOR_STATIC || lParam == CTLCOLOR_LISTBOX ||
          lParam == CTLCOLOR_EDIT || lParam == CTLCOLOR_BTN) {
        mainwndrsult = (long) (int) GetStockObject (WHITE_BRUSH);
        SetBkColor ((HDC) wParam, RGB (255, 255, 255));
        SetTextColor ((HDC) wParam, RGB (0, 0, 0));
      } else {
        mainwndrsult = DefWindowProc (hwnd, message, wParam, lParam);
      }
      break;
#else
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORBTN:
      mainwndrsult = (long) GetStockObject (WHITE_BRUSH);
      SetBkColor ((HDC) wParam, RGB (255, 255, 255));
      SetTextColor ((HDC) wParam, RGB (0, 0, 0));
      break;
#endif
    case WM_CHAR:
      HANDLE_WM_CHAR (hwnd, wParam, lParam, MyCls_OnChar);
      if (handlechar) {
        mainwndrsult = DefWindowProc (hwnd, message, wParam, lParam);
      }
      break;
    default:
      mainwndrsult = DefWindowProc (hwnd, message, wParam, lParam);
      break;
  }
  Nlm_currentHWnd = tempHWnd;
  Nlm_currentHDC = tempHDC;
  Nlm_currentWindowTool = tempHWnd;
  return mainwndrsult;
}

void FAR PASCAL EXPORT MetronomeProc (HWND hwnd, UINT message,
                                      WPARAM wParam, LPARAM lParam)

{
  if (Nlm_VibrantDisabled ()) {
    return;
  }
  if (message == WM_TIMER && timerAction != NULL) {
    timerAction ();
  }
}

extern Nlm_Boolean Nlm_RegisterWindows (void)

{
  Nlm_Boolean  rsult;
  WNDCLASS     wc;

  rsult = FALSE;
  wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = MainProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = Nlm_currentHInst;
  wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor (NULL, IDC_ARROW);
  /*
  wc.hbrBackground = GetStockObject (WHITE_BRUSH);
  */
  wc.hbrBackground = CreateSolidBrush (GetSysColor (COLOR_WINDOW));
  wc.lpszMenuName = NULL;
  sprintf (windowclass, "Nlm_WindowClass%ld", (long) (int) Nlm_currentHInst);
  wc.lpszClassName = windowclass;
  rsult = (Nlm_Boolean) RegisterClass (&wc);
  return rsult;
}

static Nlm_Boolean Nlm_SetupWindows (void)

{
  Nlm_Int2   height;
  WNDPROC    lpfnTimerProc;
  Nlm_PoinT  pt;
  Nlm_RecT   r;
  Nlm_Int2   width;

  width = GetSystemMetrics (SM_CXSCREEN);
  height = GetSystemMetrics (SM_CYSCREEN);
  Nlm_LoadRect (&screenBitBounds, 0, 0, width, height);
  Nlm_LoadPt (&pt, 0, 0);
  r = screenBitBounds;
  Nlm_screenRect = screenBitBounds;
  Nlm_desktopWindow = (Nlm_WindoW) Nlm_HandNew (sizeof (Nlm_WindowRec));
  Nlm_LoadGraphicData ((Nlm_GraphiC) Nlm_desktopWindow, NULL, NULL,
                       NULL, NULL, desktopProcs, NULL, &r, TRUE, TRUE);
  Nlm_LoadBoxData ((Nlm_BoX) Nlm_desktopWindow, pt, pt, pt, 0, 0, 0, 0, 0, 0, 0, 0);
  Nlm_LoadWindowData (Nlm_desktopWindow, NULL, NULL, NULL, NULL, &r, &r, &r, NULL,
                      NULL, NULL, NULL, NULL, NULL, NULL, TRUE, FALSE, NULL, NULL, NULL);
  Nlm_systemWindow = NULL;
  Nlm_currentHWnd = NULL;
  Nlm_currentHDC = NULL;
  Nlm_currentWindowTool = NULL;
  quitProgram = FALSE;
  
  Nlm_SetUpDrawingTools ();

  Nlm_hScrollBarHeight = GetSystemMetrics (SM_CYHSCROLL);
  Nlm_vScrollBarWidth = GetSystemMetrics (SM_CXVSCROLL);

  Nlm_popupMenuHeight = Nlm_stdLineHeight + 8;
  Nlm_dialogTextHeight = Nlm_stdFontHeight * 3 / 2;
  
  currentCursor = LoadCursor (NULL, IDC_WAIT);
  
  lpfnTimerProc = (WNDPROC) MakeProcInstance ((FARPROC) MetronomeProc, Nlm_currentHInst);
  if (lpfnTimerProc != NULL) {
    timerID = SetTimer (NULL, 1, 50, (TIMERPROC) lpfnTimerProc);
  }
  timerAction = NULL;
  keyAction = NULL;
  return TRUE;
}
#endif

#ifdef WIN_MOTIF
static void Nlm_WindowTimer (XtPointer client_data, XtIntervalId *id)

{
  if (! quitProgram) {
    windowTimer = XtAppAddTimeOut (Nlm_appContext, 50, Nlm_WindowTimer, NULL);
  }
  if (timerAction != NULL) {
    timerAction ();
  }
}

extern Nlm_Boolean Nlm_RegisterWindows (void)

{
  windowTimer = XtAppAddTimeOut (Nlm_appContext, 50, Nlm_WindowTimer, NULL);
  return TRUE;
}

extern XFontStruct *Nlm_XLoadQueryFont PROTO((Display *, Nlm_CharPtr, Nlm_Boolean));
static Nlm_Boolean Nlm_SetupWindows (void)

{
  char       appname [128];
  Nlm_Int4   height;
  Nlm_PoinT  pt;
  Nlm_RecT   r;
  Nlm_Int4   width;

  Nlm_desktopWindow = NULL;
  Nlm_systemWindow = NULL;
  Nlm_currentXDisplay = NULL;
  Nlm_currentXScreen = 0;
  Nlm_currentXWindow = 0;
  Nlm_currentXGC = NULL;
  Nlm_currentWindowTool = NULL;
  quitProgram = FALSE;
  timerAction = NULL;
  keyAction = NULL;
  XtToolkitInitialize ();
  Nlm_appContext = XtCreateApplicationContext ();
  if (Nlm_appContext == NULL) {
    return FALSE;
  }
  Nlm_currentXDisplay = XOpenDisplay (NULL);
  if (Nlm_currentXDisplay == NULL) {
    fprintf (stderr, "Vibrant applications require X Windows\n");
    return FALSE;
  }
  Nlm_currentXScreen = DefaultScreen (Nlm_currentXDisplay);
  width =  DisplayWidth (Nlm_currentXDisplay, Nlm_currentXScreen);
  height = DisplayHeight (Nlm_currentXDisplay, Nlm_currentXScreen);

  Nlm_LoadRect (&screenBitBounds, 0, 0, (Nlm_Int2) width, (Nlm_Int2) height);
  Nlm_LoadPt (&pt, 0, 0);
  r = screenBitBounds;
  Nlm_screenRect = screenBitBounds;
  Nlm_desktopWindow = (Nlm_WindoW) Nlm_HandNew (sizeof (Nlm_WindowRec));
  Nlm_LoadGraphicData ((Nlm_GraphiC) Nlm_desktopWindow, NULL, NULL,
                       NULL, NULL, desktopProcs, NULL, &r, TRUE, TRUE);
  Nlm_LoadBoxData ((Nlm_BoX) Nlm_desktopWindow, pt, pt, pt, 0, 0, 0, 0, 0, 0, 0, 0);
  Nlm_LoadWindowData (Nlm_desktopWindow, NULL, NULL, NULL, NULL, &r, &r, &r, NULL,
                      NULL, NULL, NULL, NULL, NULL, NULL, TRUE, FALSE, NULL, NULL, NULL);
  Nlm_StringNCpy (appname, statargv [0], sizeof (appname));
  TO_UPPER (appname [0]);
  XtDisplayInitialize (Nlm_appContext, Nlm_currentXDisplay, (String) appname,
                       (String) "Vibrant", (XrmOptionDescRec *) NULL,
                       (Cardinal) 0, &statargc, statargv);
  firstGC = XCreateGC (Nlm_currentXDisplay,
                       RootWindow (Nlm_currentXDisplay,
                                   Nlm_currentXScreen),
                       0, NULL);
  Nlm_currentXGC = firstGC;
  Nlm_SetUpDrawingTools ();

  Nlm_hScrollBarHeight = 15;
  Nlm_vScrollBarWidth = 15;

  Nlm_popupMenuHeight = Nlm_stdLineHeight + 10;
  Nlm_dialogTextHeight = Nlm_stdLineHeight + 16;
  
  arrow = XCreateFontCursor (Nlm_currentXDisplay, XC_top_left_arrow);
  cross = XCreateFontCursor (Nlm_currentXDisplay, XC_cross);
  iBeam = XCreateFontCursor (Nlm_currentXDisplay, XC_xterm);
  plus = XCreateFontCursor (Nlm_currentXDisplay, XC_crosshair);
  watch = XCreateFontCursor (Nlm_currentXDisplay, XC_watch);

  currentCursor = arrow;
  
  font = Nlm_XLoadQueryFont (Nlm_currentXDisplay, "-*-helvetica-bold-r-*--14-*", TRUE);
  Nlm_XfontList = XmFontListCreate (font, "dummy");

  Nlm_fileDialogShell = NULL;

  return TRUE;
}
#endif

static void Nlm_WindowGo (Nlm_WindoW w)

{
  inNotice = FALSE;
  Nlm_DoRemove ((Nlm_GraphiC) w, TRUE);
}

static void Nlm_PanelGo (Nlm_PaneL p, Nlm_PoinT pt)

{
  Nlm_WindoW  w;

  inNotice = FALSE;
  w = Nlm_ParentWindow (p);
  Nlm_DoRemove ((Nlm_GraphiC) w, TRUE);
}

static void Nlm_CenterLine (Nlm_RectPtr rptr, Nlm_CharPtr text, Nlm_FonT fnt)

{
  if (fnt != NULL) {
    Nlm_SelectFont (fnt);
  }
  rptr->bottom = rptr->top + Nlm_LineHeight ();
  Nlm_DrawString (rptr, text, 'c', FALSE);
  rptr->top = rptr->bottom;
}

static void Nlm_DrawAbout (Nlm_PaneL p)

{
  Nlm_RecT  r;

  Nlm_ObjectRect (p, &r);
  Nlm_InsetRect (&r, 4, 4);
  r.top += 10;
  Nlm_Blue ();
  Nlm_CenterLine (&r, "VIBRANT", Nlm_systemFont);
  r.top += 10;
  Nlm_Red ();
  Nlm_CenterLine (&r, "National Center for Biotechnology Information", Nlm_systemFont);
  r.top += 5;
  Nlm_CenterLine (&r, "National Library of Medicine", Nlm_systemFont);
  r.top += 5;
  Nlm_CenterLine (&r, "National Institutes of Health", Nlm_systemFont);
  r.top += 10;
  Nlm_CenterLine (&r, "(301) 496-2475", Nlm_systemFont);
  r.top += 5;
  Nlm_CenterLine (&r, "info@ncbi.nlm.nih.gov", Nlm_systemFont);
}

static void Nlm_GetSet (void)

{
  Nlm_PaneL   p;
  Nlm_WindoW  w;

  w = Nlm_ModalWindow (-50, -33, -1, -1, Nlm_WindowGo);
  p = Nlm_SimplePanel (w, 28 * Nlm_stdCharWidth, 10 * Nlm_stdLineHeight, Nlm_DrawAbout);
  Nlm_SetPanelClick (p, NULL, NULL, NULL, Nlm_PanelGo);
  Nlm_DoShow ((Nlm_GraphiC) w, TRUE, TRUE);
  inNotice = TRUE;
  while (inNotice) {
    Nlm_ProcessAnEvent ();
  }
}

static void Nlm_GetReady (void)

{
#ifdef COMP_THINKC
  KeyMap  keys;

  GetKeys (keys);
  if ((keys [1] & 1) != 0) {
    Nlm_GetSet ();
  }
#endif
#ifdef COMP_MPW
  KeyMap  keys;

  GetKeys (&keys);
  if ((keys [1] & 1) != 0) {
    Nlm_GetSet ();
  }
#endif
#ifdef WIN_MAC
  GetAppParms (apName, &apRefNum, &apParam);
  Nlm_PtoCstr ((Nlm_CharPtr) apName);
#endif
#ifdef WIN_MSWIN
  if ((GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0) {
    Nlm_GetSet ();
  }
#endif
#ifdef WIN_MOTIF
  argc = statargc;
  argv = statargv;
#endif
  Nlm_ArrowCursor ();
}

static void Nlm_CleanUpWindows (void)

/*
*  Since submenus goes into the Macintosh desktop menu bar, to allow for
*  command key equivalents, it can be very dangerous to remove the menu
*  bar.  The submenu linked into the menu bar now no longer removes its
*  children, leaving that up to the submenu item.
*/

{
  Nlm_GraphiC  g;
  Nlm_MenuBaR  mb;
  Nlm_GraphiC  n;
  Nlm_WindoW   w;

#ifdef WIN_MSWIN
  if (timerID != 0) {
    KillTimer (NULL, timerID);
  }
#endif
#ifdef WIN_MOTIF
  XtRemoveTimeOut (windowTimer);
#endif
  w = (Nlm_WindoW) Nlm_GetNext ((Nlm_GraphiC) Nlm_desktopWindow);
  while (w != NULL && w != Nlm_systemWindow) {
    Nlm_UseWindow (w);
    Nlm_DoHide ((Nlm_GraphiC) w, TRUE, FALSE);
    mb = Nlm_GetWindowMenuBar (w);
    if (mb != NULL) {
      Nlm_DoRemove ((Nlm_GraphiC) mb, FALSE);
    }
    g = Nlm_GetChild ((Nlm_GraphiC) w);
    while (g != NULL) {
      n = Nlm_GetNext (g);
      Nlm_DoRemove (g, FALSE);
      g = n;
    }
    w = (Nlm_WindoW) Nlm_GetNext ((Nlm_GraphiC) w);
  }
  w = Nlm_desktopWindow;
  mb = Nlm_GetWindowMenuBar (w);
  if (mb != NULL) {
    Nlm_DoRemove ((Nlm_GraphiC) mb, FALSE);
    Nlm_SetWindowMenuBar (w, NULL);
  }
}

#ifdef WIN_MAC
main ()

{
  Nlm_Int2  i;

  MaxApplZone ();
  for (i = 0; i < 20; i++) {
    MoreMasters ();
  }
  InitGraf (&qd.thePort);
  InitFonts ();
  FlushEvents (everyEvent, 0);
  InitWindows ();
  InitMenus ();
  TEInit ();
  InitDialogs (0);
  InitCursor ();

  Nlm_InitVibrantHooks ();

  Nlm_InitWindows ();
  Nlm_InitBars ();
  Nlm_InitButtons ();
  Nlm_InitExtras ();
  Nlm_InitGroup ();
  Nlm_InitLists ();
  Nlm_InitMenus ();
  Nlm_InitPrompt ();
  Nlm_InitSlate ();
  Nlm_InitTexts ();
  Nlm_SetupWindows ();
  Nlm_RegisterWindows ();
  Nlm_RegisterTexts ();
  Nlm_RegisterSlates ();
  Nlm_GetReady ();

  Nlm_Main ();

  Nlm_CleanUpWindows ();
  Nlm_CleanUpDrawingTools ();
  Nlm_FreeWindows ();
  Nlm_FreeBars ();
  Nlm_FreeButtons ();
  Nlm_FreeExtras ();
  Nlm_FreeGroup ();
  Nlm_FreeLists ();
  Nlm_FreeMenus ();
  Nlm_FreePrompt ();
  Nlm_FreeSlate ();
  Nlm_FreeTexts ();
  Nlm_FreeConfigStruct ();
  ExitToShell ();
}
#endif

#ifdef WIN_MSWIN
#ifdef WIN32
static int Nlm_HandleException (DWORD code)

{
  Nlm_Message (MSG_OK, "WIN32 exception %ld", (long) code);
  return EXCEPTION_CONTINUE_EXECUTION;
}
#endif

int CALLBACK WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPSTR lpszCmdLine, int nCmdShow)

{
  Nlm_Char  str [32];
  WNDCLASS  wc;

#ifdef WIN32
  try {
#endif

  Nlm_currentHInst = hInstance;

  Nlm_InitVibrantHooks ();

  Nlm_InitWindows ();
  Nlm_InitBars ();
  Nlm_InitButtons ();
  Nlm_InitExtras ();
  Nlm_InitGroup ();
  Nlm_InitLists ();
  Nlm_InitMenus ();
  Nlm_InitPrompt ();
  Nlm_InitSlate ();
  Nlm_InitTexts ();
  Nlm_SetupWindows ();
  if (! (Nlm_RegisterWindows ())) {
    return FALSE;
  }
  if (! (Nlm_RegisterTexts ())) {
    return FALSE;
  }
  if (! (Nlm_RegisterSlates ())) {
    sprintf (str, "Nlm_WindowClass%ld", (long) (int) Nlm_currentHInst);
    UnregisterClass (str, Nlm_currentHInst);
    return FALSE;
  }
  Nlm_GetReady ();

  Nlm_Main ();

  Nlm_CleanUpWindows ();
  Nlm_CleanUpDrawingTools ();
  Nlm_FreeWindows ();
  Nlm_FreeBars ();
  Nlm_FreeButtons ();
  Nlm_FreeExtras ();
  Nlm_FreeGroup ();
  Nlm_FreeLists ();
  Nlm_FreeMenus ();
  Nlm_FreePrompt ();
  Nlm_FreeSlate ();
  Nlm_FreeTexts ();
  Nlm_FreeConfigStruct ();

  sprintf (str, "Nlm_WindowClass%ld", (long) (int) Nlm_currentHInst);
  if (GetClassInfo (Nlm_currentHInst, str, &wc)) {
    DeleteObject (wc.hbrBackground);
  }
  UnregisterClass (str, Nlm_currentHInst);
  sprintf (str, "Nlm_SlateClass%ld", (long) (int) Nlm_currentHInst);
  UnregisterClass (str, Nlm_currentHInst);

#ifdef WIN32
  } except (Nlm_HandleException (GetExceptionCode ())) {
  }
#endif

  return TRUE;
}
#endif

#ifdef WIN_MOTIF
main (int argc, char *argv [])

{
  Nlm_Int2  retval;

  statargc = argc;
  statargv = argv;

  Nlm_InitVibrantHooks ();

  Nlm_InitWindows ();
  Nlm_InitButtons ();
  Nlm_InitExtras ();
  Nlm_InitGroup ();
  Nlm_InitBars ();
  Nlm_InitLists ();
  Nlm_InitMenus ();
  Nlm_InitPrompt ();
  Nlm_InitSlate ();
  Nlm_InitTexts ();
  if (! Nlm_SetupWindows ()) {
    return FALSE;
  }
  Nlm_RegisterWindows ();
  Nlm_RegisterTexts ();
  Nlm_RegisterSlates ();

  Nlm_GetReady ();
  retval = Nlm_Main ();

  Nlm_CleanUpWindows ();
  Nlm_CleanUpDrawingTools ();
  Nlm_FreeWindows ();
  Nlm_FreeBars ();
  Nlm_FreeButtons ();
  Nlm_FreeExtras ();
  Nlm_FreeGroup ();
  Nlm_FreeLists ();
  Nlm_FreeMenus ();
  Nlm_FreePrompt ();
  Nlm_FreeSlate ();
  Nlm_FreeTexts ();
  Nlm_FreeConfigStruct ();
  Nlm_currentWindowTool = NULL;
  Nlm_currentXWindow = 0;
  Nlm_currentXGC = NULL;
  if (Nlm_currentXDisplay != NULL) {
    if (Nlm_fileDialogShell != NULL) {
      XtDestroyWidget (Nlm_fileDialogShell);
    }
    if (Nlm_XfontList != NULL) {
      XmFontListFree (Nlm_XfontList);
    }
    XFreeCursor (Nlm_currentXDisplay, arrow);
    XFreeCursor (Nlm_currentXDisplay, cross);
    XFreeCursor (Nlm_currentXDisplay, iBeam);
    XFreeCursor (Nlm_currentXDisplay, plus);
    XFreeCursor (Nlm_currentXDisplay, watch);
    XFreeGC (Nlm_currentXDisplay, firstGC);
    XCloseDisplay (Nlm_currentXDisplay);
  }
  exit (retval);
}
#endif

extern void Nlm_Metronome (Nlm_VoidProc actn)

{
  timerAction = actn;
}

extern void Nlm_RemoveDyingWindows (void)

/*
*  Since submenus goes into the Macintosh desktop menu bar, to allow for
*  command key equivalents, it can be very dangerous to remove the menu
*  bar.  The submenu linked into the menu bar now no longer removes its
*  children, leaving that up to the submenu item.
*/

{
  Nlm_GraphiC     g;
  Nlm_MenuBaR     mb;
  Nlm_GraphiC     n;
  Nlm_PortTool    tempPort;
  Nlm_WindoW      w;
  Nlm_WindowData  wdata;
  Nlm_WindowTool  wptr;
#ifdef WIN_MAC
  PenState        state;
#endif
#ifdef WIN_MOTIF
  Nlm_ShellTool   shl;
#endif

  while (dyingWindow != NULL) {
    w = dyingWindow;
    wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) w);
    Nlm_currentWindowTool = wptr;
#ifdef WIN_MAC
    GetPort (&tempPort);
    GetPenState (&state);
    SetPort (wptr);
    Nlm_currentWindowTool = wptr;
    Nlm_SetUpdateRegion (wptr);
#endif
#ifdef WIN_MSWIN
    tempPort = Nlm_currentHDC;
    Nlm_currentHDC = Nlm_ParentWindowPort ((Nlm_GraphiC) w);
#endif
#ifdef WIN_MOTIF
    XSync (Nlm_currentXDisplay, FALSE);
    Nlm_currentXGC = Nlm_ParentWindowPort ((Nlm_GraphiC) w);
#endif
    mb = Nlm_GetWindowMenuBar (w);
    if (mb != NULL) {
      Nlm_DoRemove ((Nlm_GraphiC) mb, FALSE);
      Nlm_SetWindowMenuBar (w, NULL);
    }
    g = Nlm_GetChild ((Nlm_GraphiC) w);
    while (g != NULL) {
      n = Nlm_GetNext (g);
      Nlm_DoRemove (g, FALSE);
      g = n;
    }
    Nlm_ResetDrawingTools ();
    Nlm_GetWindowData (w, &wdata);
    if (wdata.data != NULL && wdata.cleanup != NULL) {
      wdata.cleanup (w, wdata.data);
    }
#ifdef WIN_MAC
    SetPort (tempPort);
    Nlm_currentWindowTool = tempPort;
    Nlm_SetUpdateRegion (tempPort);
    SetPenState (&state);
    CloseWindow (wptr);
    Nlm_MemFree (wptr);
#endif
#ifdef WIN_MSWIN
    Nlm_currentHDC = tempPort;
    RemoveProp (wptr, (LPSTR) "Nlm_VibrantProp");
    DestroyWindow (wptr);
#endif
#ifdef WIN_MOTIF
    if (wdata.title != NULL) {
      Nlm_MemFree (wdata.title);
    }
    Nlm_currentWindowTool = NULL;
    Nlm_currentXWindow = 0;
    Nlm_currentXGC = firstGC;
    shl = Nlm_GetWindowShell (w);
    if (NLM_QUIET) {
      if (Nlm_WindowHasBeenShown (w)) {
        XtDestroyWidget (shl);
      }
    } else {
      XtDestroyWidget (shl);
    }
    XSync (Nlm_currentXDisplay, FALSE);
#endif
    dyingWindow = (Nlm_WindoW) Nlm_GetNext ((Nlm_GraphiC) w);
    Nlm_HandFree (w);
    recentWindow = NULL;
    Nlm_Update ();
  }
}

extern void Nlm_Update (void)

{
#ifdef WIN_MAC
  while (EventAvail (updateMask, &Nlm_currentEvent)) {
    if (WaitNextEvent (updateMask, &Nlm_currentEvent, 0, NULL)) {
      Nlm_HandleEvent ();
    }
  }
#endif
#ifdef WIN_MSWIN
  while (PeekMessage (&Nlm_currentMssg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE)) {
    if (GetMessage (&Nlm_currentMssg, NULL, WM_PAINT, WM_PAINT)) {
      if (Nlm_VibrantDisabled ()) {
        discard_count++; /* really just a place to set a breakpoint */
      }
      TranslateMessage (&Nlm_currentMssg);
      DispatchMessage (&Nlm_currentMssg);
    }
  }
#endif
#ifdef WIN_MOTIF
  XEvent       event;
  XtInputMask  mask;

  XSync (Nlm_currentXDisplay, FALSE);
  mask = XtAppPending (Nlm_appContext);
  if (mask != 0) {
    while (XCheckTypedEvent (Nlm_currentXDisplay, Expose, &event)) {
      XtDispatchEvent (&event);
    }
  }
  XSync (Nlm_currentXDisplay, FALSE);
#endif
}

extern void Nlm_KeyboardView (Nlm_KeyProc key)

{
  keyAction = key;
}

#ifdef WIN_MSWIN
static void Nlm_ProcessKeyPress (LPMSG lpMsg)

{
  Nlm_Char  ch;

  if (lpMsg->message == WM_CHAR) {
    ch = (Nlm_Char) lpMsg->wParam;
    if (keyAction != NULL) {
      keyAction (ch);
    }
  }
}
#endif

#ifdef WIN_MOTIF
static void Nlm_ProcessKeyPress (XEvent *event)

{
  Nlm_Char        buffer [20];
  Nlm_Char        ch;
  int             charcount;
  XComposeStatus  compose;
  KeySym          keysym;

  if (event->type == KeyPress) {
    buffer [0] = '\0';
    charcount = XLookupString (&(event->xkey), buffer, 20, &keysym, &compose);
    if (charcount >= 0 && charcount < 20) {
      buffer [charcount] = '\0';
    }
    if (keyAction != NULL) {
      ch = buffer [0];
      keyAction (ch);
    }
  }
}
#endif

extern void Nlm_ProcessAnEvent (void)

{
#ifdef WIN_MAC
  if (EventAvail (everyEvent, &Nlm_currentEvent)) {
    if (WaitNextEvent (everyEvent, &Nlm_currentEvent, 0, NULL)) {
      Nlm_HandleEvent ();
    }
  }
#endif
#ifdef WIN_MSWIN
  if (PeekMessage (&Nlm_currentMssg, NULL, 0, 0, PM_NOREMOVE)) {
    if (GetMessage (&Nlm_currentMssg, NULL, 0, 0)) {
      TranslateMessage (&Nlm_currentMssg);
      Nlm_ProcessKeyPress (&Nlm_currentMssg);
      DispatchMessage (&Nlm_currentMssg);
    }
  }
#endif
#ifdef WIN_MOTIF
  XEvent       event;
  XtInputMask  mask;

  mask = XtAppPending (Nlm_appContext);
  if (mask != 0) {
    XtAppNextEvent (Nlm_appContext, &event);
    Nlm_ProcessKeyPress (&event);
    XtDispatchEvent (&event);
  }
#endif
}

extern void Nlm_ProcessTimerEvent (void)

{
#ifdef WIN_MAC
  Nlm_Int4  delta;
  Nlm_Int4  ticks;

  ticks = Nlm_ComputerTime ();
  delta = ticks - lastTimerTime;
  if (delta >= 3) {
    lastTimerTime = ticks;
    if (timerAction != NULL) {
      timerAction ();
    }
  }
#endif
}

#ifdef WIN_MAC
static void Nlm_ProcessIdle (void)

{
  Nlm_PointTool  ptool;

  Nlm_ProcessTimerEvent ();
  GetMouse (&ptool);
  LocalToGlobal (&ptool);
  Nlm_PointToolToPoinT (ptool, &Nlm_globalMouse);
  frontWindow = Nlm_FindWindowRec (FrontWindow ());
  Nlm_DoIdle ((Nlm_GraphiC) frontWindow, Nlm_globalMouse);
  Nlm_ClearKeys ();
}
#endif

extern void Nlm_ProcessEventOrIdle (void)

{
#ifdef WIN_MAC
  if (EventAvail (everyEvent, &Nlm_currentEvent)) {
    if (WaitNextEvent (everyEvent, &Nlm_currentEvent, 0, NULL)) {
      Nlm_HandleEvent ();
    }
  } else {
    Nlm_ProcessIdle ();
  }
#endif
#ifdef WIN_MSWIN
  if (PeekMessage (&Nlm_currentMssg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE)) {
    if (GetMessage (&Nlm_currentMssg, NULL, WM_PAINT, WM_PAINT)) {
      TranslateMessage (&Nlm_currentMssg);
      DispatchMessage (&Nlm_currentMssg);
    }
  } else if (PeekMessage (&Nlm_currentMssg, NULL, 0, 0, PM_NOREMOVE)) {
    if (GetMessage (&Nlm_currentMssg, NULL, 0, 0)) {
      TranslateMessage (&Nlm_currentMssg);
      Nlm_ProcessKeyPress (&Nlm_currentMssg);
      DispatchMessage (&Nlm_currentMssg);
    }
  }
#endif
#ifdef WIN_MOTIF
  XEvent       event;
  XtInputMask  mask;

  mask = XtAppPending (Nlm_appContext);
  if (mask != 0) {
    if (XCheckTypedEvent (Nlm_currentXDisplay, Expose, &event)) {
      XtDispatchEvent (&event);
    } else {
      XtAppNextEvent (Nlm_appContext, &event);
      Nlm_ProcessKeyPress (&event);
      XtDispatchEvent (&event);
    }
  }
#endif
}

extern void Nlm_ProcessExternalEvent (void)

{
#ifdef WIN_MAC
  if (WaitNextEvent (everyEvent, &Nlm_currentEvent, 10, NULL)) {
    Nlm_HandleEvent ();
  }
#endif
#ifdef WIN_MSWIN
  if (PeekMessage (&Nlm_currentMssg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE)) {
    if (GetMessage (&Nlm_currentMssg, NULL, WM_PAINT, WM_PAINT)) {
      TranslateMessage (&Nlm_currentMssg);
      DispatchMessage (&Nlm_currentMssg);
    }
  } else if (PeekMessage (&Nlm_currentMssg, NULL, 0, 0, PM_NOREMOVE)) {
    if (GetMessage (&Nlm_currentMssg, NULL, 0, 0)) {
      TranslateMessage (&Nlm_currentMssg);
      Nlm_ProcessKeyPress (&Nlm_currentMssg);
      DispatchMessage (&Nlm_currentMssg);
    }
  }
#endif
#ifdef WIN_MOTIF
  XEvent       event;
  XtInputMask  mask;

  mask = XtAppPending (Nlm_appContext);
  if (mask != 0) {
    if (XCheckTypedEvent (Nlm_currentXDisplay, Expose, &event)) {
      XtDispatchEvent (&event);
    } else {
      XtAppNextEvent (Nlm_appContext, &event);
      Nlm_ProcessKeyPress (&event);
      XtDispatchEvent (&event);
    }
  }
#endif
}

extern void Nlm_ProcessEvents (void)

{
#ifdef WIN_MAC
  while (! quitProgram) {
    if (! EventAvail (everyEvent, &Nlm_currentEvent)) {
      Nlm_ProcessIdle ();
    }
    while (WaitNextEvent (everyEvent, &Nlm_currentEvent, 1, NULL)) {
      Nlm_HandleEvent ();
      Nlm_RemoveDyingWindows ();
    }
  }
#endif
#ifdef WIN_MSWIN
  while (! quitProgram) {
    while (PeekMessage (&Nlm_currentMssg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE)) {
      if (GetMessage (&Nlm_currentMssg, NULL, WM_PAINT, WM_PAINT)) {
        TranslateMessage (&Nlm_currentMssg);
        DispatchMessage (&Nlm_currentMssg);
      }
    }
    if (GetMessage (&Nlm_currentMssg, NULL, 0, 0)) {
      TranslateMessage (&Nlm_currentMssg);
      Nlm_ProcessKeyPress (&Nlm_currentMssg);
      DispatchMessage (&Nlm_currentMssg);
      Nlm_RemoveDyingWindows ();
    }
  }
#endif
#ifdef WIN_MOTIF
  XEvent       event;
  XtInputMask  mask;

  while (! quitProgram) {
    mask = XtAppPending (Nlm_appContext);
    if (mask != 0) {
      while (XCheckTypedEvent (Nlm_currentXDisplay, Expose, &event)) {
        XtDispatchEvent (&event);
      }
    }
    XtAppNextEvent (Nlm_appContext, &event);
    Nlm_ProcessKeyPress (&event);
    XtDispatchEvent (&event);
    Nlm_RemoveDyingWindows ();
  }
  if (Nlm_currentXDisplay != NULL) {
    XSync (Nlm_currentXDisplay, TRUE);
  }
#endif
  Nlm_RemoveDyingWindows ();
}

extern Nlm_Boolean Nlm_EventAvail (void)

{
#ifdef WIN_MAC
  return (EventAvail (everyEvent, &Nlm_currentEvent));
#endif
#ifdef WIN_MSWIN
  return (Nlm_Boolean) (PeekMessage (&Nlm_currentMssg, NULL, 0, 0, PM_NOREMOVE | PM_NOYIELD));
#endif
#ifdef WIN_MOTIF
  return (XtAppPending (Nlm_appContext) != 0);
#endif
}

extern void Nlm_FlushEvents (void)

{
#ifdef WIN_MAC
  FlushEvents (everyEvent - updateMask, 0);
#endif
#ifdef WIN_MSWIN
  Nlm_Boolean  goOn;

  goOn = TRUE;
  while (goOn) {
    goOn = (Nlm_Boolean) PeekMessage (&Nlm_currentMssg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD);
    if (goOn && Nlm_currentMssg.message == WM_PAINT) {
      goOn = FALSE;
    }
  }
#endif
#ifdef WIN_MOTIF
  if (Nlm_currentXDisplay != NULL) {
    XSync (Nlm_currentXDisplay, FALSE);
  }
#endif
}

extern void Nlm_QuitProgram (void)

{
  quitProgram = TRUE;
#ifdef WIN_MSWIN
  PostQuitMessage (0);
#endif
}

extern void Nlm_UnloadSegment (Nlm_VoidProc routineAddr)

{
#ifdef WIN_MAC
  if (routineAddr != NULL) {
    UnloadSeg ((void *) routineAddr);
  }
#endif
}

static void Nlm_GetArgsOkProc (Nlm_ButtoN b)

{
  getArgsBoxUp = FALSE;
  getArgsOk = TRUE;
}

static void Nlm_GetArgsCancelProc (Nlm_ButtoN b)

{
  getArgsBoxUp = FALSE;
}

static Nlm_GraphiC Nlm_GetPrevObject (Nlm_GraphiC g)

{
  Nlm_GraphiC  prev;
  Nlm_GraphiC  prnt;
  Nlm_GraphiC  this;

  prev = NULL;
  if (g != NULL) {
    prnt = Nlm_GetParent (g);
    this = Nlm_GetChild (prnt);
    while (this != NULL && this != g) {
      prev = this;
      this = Nlm_GetNext (this);
    }
  }
  return prev;
}

static void Nlm_GetArgsFileInProc (Nlm_ButtoN b)

{
  Nlm_Char  path [PATH_MAX];
  Nlm_TexT  t;

  t = (Nlm_TexT) Nlm_GetPrevObject ((Nlm_GraphiC) b);
  if (t != NULL) {
    if (Nlm_GetInputFileName (path, sizeof (path), "", "TEXT")) {
      Nlm_SetTitle ((Nlm_Handle) t, path);
    }
  }
}

static void Nlm_GetArgsFileOutProc (Nlm_ButtoN b)

{
  Nlm_Char  path [PATH_MAX];
  Nlm_TexT  t;

  t = (Nlm_TexT) Nlm_GetPrevObject ((Nlm_GraphiC) b);
  if (t != NULL) {
    if (Nlm_GetOutputFileName (path, sizeof (path), "")) {
      Nlm_SetTitle ((Nlm_Handle) t, path);
    }
  }
}

#define MAX_ARGS 50

typedef Nlm_Handle PNTR Nlm_HandlePtr;

extern Nlm_Boolean Nlm_GetArgs (Nlm_CharPtr progname, Nlm_Int2 numargs, Nlm_ArgPtr ap)

{
  static char * TypeStrings[9] = {
        "",
        "Integer: ",
        "Float: ",
        "String: ",
        "File In: ",
        "File Out: ",
        "Data In: ",
        "Data Out: " };
  Nlm_Boolean    okay = FALSE;
  Nlm_Boolean    range;
  Nlm_Int2       i;
  Nlm_Int2       j;
  Nlm_Int4       ifrom, ito;
  Nlm_FloatHi    ffrom, fto;
  Nlm_ArgPtr     curarg;
  Nlm_Boolean    resolved [MAX_ARGS];
  Nlm_Char       arg [80];
  Nlm_CharPtr    buf;
  Nlm_Char       ch;
  Nlm_Int2       delta;
  Nlm_GrouP      g;
  Nlm_GrouP      h;
  Nlm_HandlePtr  hp;
  Nlm_Int2       k;
  Nlm_Int2       l;
  Nlm_GrouP      pg;
  Nlm_RecT       r1;
  Nlm_RecT       r2;
  Nlm_Boolean    smallScreen;
  Nlm_WindoW     w;
  Nlm_TexT       firstText;
  Nlm_CharPtr    tmp;

  if ((ap == NULL) || (numargs == 0) || (numargs > MAX_ARGS)) {
    return okay;
  }

  curarg = ap;
  Nlm_MemFill (resolved, '\0', (MAX_ARGS * sizeof(Nlm_Boolean)));

  for (i = 0; i < numargs; i++, curarg++) {
    if ((curarg->type < ARG_BOOLEAN) || (curarg->type > ARG_DATA_OUT)) {
      Message(MSG_ERROR, "Invalid Arg->type in %s", curarg->prompt);
      return okay;
    }
    curarg->intvalue = 0;
    curarg->floatvalue = 0.0;
    curarg->strvalue = NULL;
    if (curarg->defaultvalue != NULL) {
      resolved [i] = TRUE;
      switch (curarg->type) {
        case ARG_BOOLEAN:
          if (TO_UPPER(*curarg->defaultvalue) == 'T') {
            curarg->intvalue = 1;
          } else {
            curarg->intvalue = 0;
          }
          break;
        case ARG_INT:
          sscanf(curarg->defaultvalue, "%ld", &curarg->intvalue);
          break;
        case ARG_FLOAT:
          sscanf(curarg->defaultvalue, "%lf", &curarg->floatvalue);
          break;
        case ARG_STRING:
        case ARG_FILE_IN:
        case ARG_FILE_OUT:
        case ARG_DATA_IN:
        case ARG_DATA_OUT:
          curarg->strvalue = StringSave (curarg->defaultvalue);
          break;
      }
    }
  }

  Nlm_StringMove(arg, progname);
  w = Nlm_FixedWindow (-50, -33, -10, -10, arg, NULL);
  smallScreen = FALSE;
#ifdef WIN_MAC
  if (Nlm_screenRect.right < 513 || Nlm_screenRect.bottom < 343) {
    smallScreen = TRUE;
  }
#endif
#ifdef WIN_MSWIN
  if (Nlm_screenRect.bottom < 352) {
    smallScreen = TRUE;
  }
#endif
  g = Nlm_HiddenGroup (w, 4, 0, NULL);
  hp = (Nlm_HandlePtr) Nlm_MemNew (numargs * sizeof (Nlm_Handle));

  firstText = NULL;
  curarg = ap;
  for (i = 0, j = 0; i < numargs; i++, j++, curarg++) {
    if ((smallScreen && j >= 10) || j >= 15) {
      j = 0;
      Nlm_Advance (w);
      g = Nlm_HiddenGroup (w, 4, 0, NULL);
    }
    Nlm_StaticPrompt (g, TypeStrings[curarg->type - 1], 0, Nlm_dialogTextHeight, Nlm_systemFont, 'l');
    switch (curarg->type) {
      case ARG_BOOLEAN:
        hp [i] = (Nlm_Handle) Nlm_CheckBox (g, curarg->prompt, NULL);
        Nlm_StaticPrompt (g, "", 0, 0, Nlm_systemFont, 'l');
        if (curarg->intvalue == 1) {
          Nlm_SetStatus (hp [i], TRUE);
        }
        break;
      case ARG_INT:
        Nlm_StaticPrompt (g, curarg->prompt, 0, Nlm_dialogTextHeight, Nlm_systemFont, 'l');
        hp [i] = (Nlm_Handle) Nlm_DialogText (g, curarg->defaultvalue, 10, NULL);
        if (firstText == NULL) {
          firstText = hp [i];
        }
        break;
      case ARG_FLOAT:
        Nlm_StaticPrompt (g, curarg->prompt, 0, Nlm_dialogTextHeight, Nlm_systemFont, 'l');
        hp [i] = (Nlm_Handle) Nlm_DialogText (g, curarg->defaultvalue, 10, NULL);
        if (firstText == NULL) {
          firstText = hp [i];
        }
        break;
      case ARG_STRING:
        pg = Nlm_HiddenGroup (g, 0, 10, NULL);
        Nlm_SetGroupMargins (pg, 1, 1);
        Nlm_SetGroupSpacing (pg, 1, 1);
        buf = curarg->prompt;
        k = 0;
        while (Nlm_StringLen (buf + k) > 0) {
          l = 0;
          ch = buf [k + l];
          while (ch != '\0' && ch != '\n' && ch != '\r') {
            l++;
            ch = buf [k + l];
          }
          if (ch == '\n' || ch == '\r') {
            buf [k + l] = '\0';
            if (Nlm_StringLen (buf + k) > 0) {
              Nlm_StaticPrompt (pg, buf + k, 0, Nlm_dialogTextHeight, Nlm_systemFont, 'l');
            }
            k += l + 1;
          } else {
            if (Nlm_StringLen (buf + k) > 0) {
              Nlm_StaticPrompt (pg, buf + k, 0, Nlm_dialogTextHeight, Nlm_systemFont, 'l');
            }
            k += l;
          }
        }
        hp [i] = (Nlm_Handle) Nlm_DialogText (g, curarg->defaultvalue, 10, NULL);
        if (firstText == NULL) {
          firstText = hp [i];
        }
        break;
      case ARG_FILE_IN:
      case ARG_FILE_OUT:
      case ARG_DATA_IN:
      case ARG_DATA_OUT:
        Nlm_StaticPrompt (g, curarg->prompt, 0, Nlm_dialogTextHeight, Nlm_systemFont, 'l');
        hp [i] = (Nlm_Handle) Nlm_DialogText (g, curarg->defaultvalue, 10, NULL);
        if (firstText == NULL) {
          firstText = hp [i];
        }
        break;
    }
    arg[0] = '\0';
    if ((curarg->from != NULL) || (curarg->to != NULL)) {
      tmp = Nlm_StringMove(arg, " (");
      tmp = Nlm_StringMove(tmp, curarg->from);
      if ((curarg->type == ARG_INT) || (curarg->type == ARG_FLOAT)) {
        tmp = Nlm_StringMove(tmp, " to ");
        tmp = Nlm_StringMove(tmp, curarg->to);
      }
      Nlm_StringMove(tmp, ")");
    } else {
      arg[0] = '\0';
    }
    if (curarg->type == ARG_FILE_IN) {
      Nlm_PushButton (g, "File IN", Nlm_GetArgsFileInProc);
    } else if (curarg->type == ARG_FILE_OUT) {
      Nlm_PushButton (g, "File OUT", Nlm_GetArgsFileOutProc);
    } else {
      Nlm_StaticPrompt(g, arg, 0,  Nlm_dialogTextHeight, Nlm_systemFont, 'l');
    }
  }

  Nlm_Break (w);
  h = Nlm_HiddenGroup (w, 10, 0, NULL);
  Nlm_PushButton (h, "OK", Nlm_GetArgsOkProc);
  Nlm_PushButton (h, "Cancel", Nlm_GetArgsCancelProc);
  Nlm_GetRect ((Nlm_GraphiC) g, &r1);
  Nlm_DoGetPosition ((Nlm_GraphiC) h, &r2);
  delta = (r1.right - r2.right);
  if (delta > 0) {
    Nlm_OffsetRect (&r2, delta / 2, 0);
    Nlm_DoSetPosition ((Nlm_GraphiC) h, &r2, TRUE);
  }
  Nlm_DoShow ((Nlm_GraphiC) w, TRUE, TRUE);
  if (firstText != NULL) {
    Nlm_DoSelect ((Nlm_GraphiC) firstText, TRUE);
  }
  getArgsBoxUp = TRUE;
  while (getArgsBoxUp) {
    getArgsOk = FALSE;
    while (getArgsBoxUp) {
      Nlm_ProcessEventOrIdle ();
    }

    if (getArgsOk) {
      curarg = ap;
      for (i = 0; ((i < numargs) && (getArgsBoxUp == FALSE)); i++, curarg++) {
        resolved [i] = TRUE;
        switch (curarg->type) {
          case ARG_BOOLEAN:
            if (Nlm_GetStatus (hp [i])) {
              curarg->intvalue = 1;
            } else {
              curarg->intvalue = 0;
            }
            break;
          case ARG_INT:
            Nlm_GetTitle (hp [i], arg, sizeof (arg));
            sscanf(arg, "%ld", &curarg->intvalue);
            if ((curarg->from != NULL) || (curarg->to != NULL)) {
              range = TRUE;
              if (curarg->from != NULL) {
                sscanf(curarg->from, "%ld", &ifrom);
                if (curarg->intvalue < ifrom) {
                  range = FALSE;
                }
              }
              if (curarg->to != NULL) {
                sscanf(curarg->to, "%ld", &ito);
                if (curarg->intvalue > ito) {
                  range = FALSE;
                }
              }
              if (! range) {
                if (curarg->from != NULL && curarg->to != NULL) {
                  Message(MSG_ERROR, "%s [%ld] is out of range [%ld to %ld]",
                          curarg->prompt, curarg->intvalue, ifrom, ito);
                } else if (curarg->from != NULL) {
                  Message(MSG_ERROR, "%s [%ld] is out of range [%ld to ...]",
                          curarg->prompt, curarg->intvalue, ifrom);
                } else if (curarg->to != NULL) {
                  Message(MSG_ERROR, "%s [%ld] is out of range [... to %ld]",
                          curarg->prompt, curarg->intvalue, ito);
                }
                getArgsBoxUp = TRUE;
                Nlm_DoSelect ((Nlm_GraphiC) hp[i], TRUE);
              }
            }
            break;
          case ARG_FLOAT:
            Nlm_GetTitle (hp [i], arg, sizeof (arg));
            sscanf(arg, "%lf", &curarg->floatvalue);
            if ((curarg->from != NULL) || (curarg->to != NULL)) {
              range = TRUE;
              if (curarg->from != NULL) {
                sscanf(curarg->from, "%lf", &ffrom);
                if (curarg->floatvalue < ffrom) {
                  range = FALSE;
                }
              }
              if (curarg->to != NULL) {
                sscanf(curarg->to, "%lf", &fto);
                if (curarg->floatvalue > fto) {
                  range = FALSE;
                }
              }
              if (! range) {
                if (curarg->from != NULL && curarg->to != NULL) {
                  Message(MSG_ERROR, "%s [%g] is out of range [%g to %g]",
                          curarg->prompt, curarg->floatvalue, ffrom, fto);
                } else if (curarg->from != NULL) {
                  Message(MSG_ERROR, "%s [%g] is out of range [%g to ...]",
                          curarg->prompt, curarg->floatvalue, ffrom);
                } else if (curarg->to != NULL) {
                  Message(MSG_ERROR, "%s [%g] is out of range [... to %g]",
                          curarg->prompt, curarg->floatvalue, fto);
                }
                Nlm_DoSelect ((Nlm_GraphiC) hp[i], TRUE);
                getArgsBoxUp = TRUE;
              }
            }
            break;
          case ARG_STRING:
          case ARG_FILE_IN:
          case ARG_FILE_OUT:
          case ARG_DATA_IN:
          case ARG_DATA_OUT:
            Nlm_GetTitle (hp [i], arg, sizeof (arg));
            if (arg [0] != '\0') {
              curarg->strvalue = StringSave (arg);
            } else {
              curarg->strvalue = NULL;
            }
            break;
        }
      }

      curarg = ap;
      if (! getArgsBoxUp) {
        for (i = 0; i < numargs; i++, curarg++) {
          if ((! curarg->optional) && (! resolved[i])) {
            Message(MSG_ERROR, "%s was not given an argument", curarg->prompt);
            getArgsBoxUp = TRUE;
            Nlm_DoSelect ((Nlm_GraphiC) hp[i], TRUE);
            break;
          }
        }
      }
    }
  }  
  Nlm_DoRemove ((Nlm_GraphiC) w, TRUE);
  Nlm_Update ();

  if (getArgsOk) {  /* leave up a little window with the program name */
    Nlm_StringMove(arg, progname);
    w = Nlm_FixedWindow (-50, -33, -10, -10, arg, NULL);
    Nlm_DoShow ((Nlm_GraphiC) w, TRUE, TRUE);
  }
  return getArgsOk;
}

extern void Nlm_FreeWindows (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemFree (gphprcsptr);
}

extern void Nlm_InitWindows (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemNew (sizeof (Nlm_GphPrcs) * 11);

  documentProcs = &(gphprcsptr [0]);
#ifdef WIN_MAC
  documentProcs->click = Nlm_DocumentClick;
  documentProcs->key = Nlm_NormalKey;
  documentProcs->draw = Nlm_DocumentDraw;
  documentProcs->idle = Nlm_NormalIdle;
#endif
  documentProcs->show = Nlm_ShowDocument;
  documentProcs->hide = Nlm_HideWindow;
  documentProcs->activate = Nlm_DocumentActivate;
  documentProcs->deactivate = Nlm_DocumentDeactivate;
  documentProcs->remove = Nlm_RemoveWindow;
  documentProcs->select = Nlm_NormalSelect;
  documentProcs->linkIn = Nlm_LinkIn;
  documentProcs->adjustPrnt = Nlm_AdjustWindow;
  documentProcs->setTitle = Nlm_SetWindowTitle;
  documentProcs->getTitle = Nlm_GetWindowTitle;
  documentProcs->gainFocus = Nlm_WindowGainFocus;
  documentProcs->loseFocus = Nlm_WindowLoseFocus;
  documentProcs->sendChar = Nlm_NormalDrawChar;

  fixedProcs = &(gphprcsptr [1]);
#ifdef WIN_MAC
  fixedProcs->click = Nlm_DialogClick;
  fixedProcs->key = Nlm_NormalKey;
  fixedProcs->draw = Nlm_NormalDraw;
  fixedProcs->idle = Nlm_NormalIdle;
#endif
  fixedProcs->show = Nlm_ShowNormal;
  fixedProcs->hide = Nlm_HideWindow;
  fixedProcs->activate = Nlm_NormalActivate;
  fixedProcs->deactivate = Nlm_NormalDeactivate;
  fixedProcs->remove = Nlm_RemoveWindow;
  fixedProcs->select = Nlm_NormalSelect;
  fixedProcs->countItems = NULL;
  fixedProcs->linkIn = Nlm_LinkIn;
  fixedProcs->adjustPrnt = Nlm_AdjustWindow;
  fixedProcs->setTitle = Nlm_SetWindowTitle;
  fixedProcs->getTitle = Nlm_GetWindowTitle;
  fixedProcs->gainFocus = Nlm_WindowGainFocus;
  fixedProcs->loseFocus = Nlm_WindowLoseFocus;
  fixedProcs->sendChar = Nlm_NormalDrawChar;

  frozenProcs = &(gphprcsptr [2]);
#ifdef WIN_MAC
  frozenProcs->click = Nlm_FrozenClick;
  frozenProcs->key = Nlm_NormalKey;
  frozenProcs->draw = Nlm_NormalDraw;
  frozenProcs->idle = Nlm_NormalIdle;
#endif
  frozenProcs->show = Nlm_ShowNormal;
  frozenProcs->hide = Nlm_HideWindow;
  frozenProcs->activate = Nlm_NormalActivate;
  frozenProcs->deactivate = Nlm_NormalDeactivate;
  frozenProcs->remove = Nlm_RemoveWindow;
  frozenProcs->select = Nlm_NormalSelect;
  frozenProcs->linkIn = Nlm_LinkIn;
  frozenProcs->adjustPrnt = Nlm_AdjustWindow;
  frozenProcs->setTitle = Nlm_SetWindowTitle;
  frozenProcs->getTitle = Nlm_GetWindowTitle;
  frozenProcs->gainFocus = Nlm_WindowGainFocus;
  frozenProcs->loseFocus = Nlm_WindowLoseFocus;
  frozenProcs->sendChar = Nlm_NormalDrawChar;

  roundProcs = &(gphprcsptr [3]);
#ifdef WIN_MAC
  roundProcs->click = Nlm_DialogClick;
  roundProcs->key = Nlm_NormalKey;
  roundProcs->draw = Nlm_NormalDraw;
  roundProcs->idle = Nlm_NormalIdle;
#endif
  roundProcs->show = Nlm_ShowNormal;
  roundProcs->hide = Nlm_HideWindow;
  roundProcs->activate = Nlm_NormalActivate;
  roundProcs->deactivate = Nlm_NormalDeactivate;
  roundProcs->remove = Nlm_RemoveWindow;
  roundProcs->select = Nlm_NormalSelect;
  roundProcs->linkIn = Nlm_LinkIn;
  roundProcs->adjustPrnt = Nlm_AdjustWindow;
  roundProcs->setTitle = Nlm_SetWindowTitle;
  roundProcs->getTitle = Nlm_GetWindowTitle;
  roundProcs->gainFocus = Nlm_WindowGainFocus;
  roundProcs->loseFocus = Nlm_WindowLoseFocus;
  roundProcs->sendChar = Nlm_NormalDrawChar;

  alertProcs = &(gphprcsptr [4]);
#ifdef WIN_MAC
  alertProcs->click = Nlm_PlainClick;
  alertProcs->key = Nlm_NormalKey;
  alertProcs->draw = Nlm_NormalDraw;
  alertProcs->idle = Nlm_NormalIdle;
#endif
  alertProcs->show = Nlm_ShowNormal;
  alertProcs->hide = Nlm_HideWindow;
  alertProcs->activate = Nlm_NormalActivate;
  alertProcs->deactivate = Nlm_NormalDeactivate;
  alertProcs->remove = Nlm_RemoveWindow;
  alertProcs->select = Nlm_PlainSelect;
  alertProcs->linkIn = Nlm_LinkIn;
  alertProcs->adjustPrnt = Nlm_AdjustWindow;
  alertProcs->setTitle = Nlm_SetWindowTitle;
  alertProcs->getTitle = Nlm_GetWindowTitle;
  alertProcs->gainFocus = Nlm_WindowGainFocus;
  alertProcs->loseFocus = Nlm_WindowLoseFocus;
  alertProcs->sendChar = Nlm_NormalDrawChar;

  modalProcs = &(gphprcsptr [5]);
#ifdef WIN_MAC
  modalProcs->click = Nlm_ModalClick;
  modalProcs->key = Nlm_ModalKey;
  modalProcs->draw = Nlm_NormalDraw;
  modalProcs->idle = Nlm_NormalIdle;
#endif
  modalProcs->show = Nlm_ShowModal;
  modalProcs->hide = Nlm_HideModal;
  modalProcs->activate = Nlm_NormalActivate;
  modalProcs->deactivate = Nlm_NormalDeactivate;
  modalProcs->remove = Nlm_RemoveWindow;
  modalProcs->select = Nlm_PlainSelect;
  modalProcs->linkIn = Nlm_LinkIn;
  modalProcs->adjustPrnt = Nlm_AdjustWindow;
  modalProcs->setTitle = Nlm_SetWindowTitle;
  modalProcs->getTitle = Nlm_GetWindowTitle;
  modalProcs->gainFocus = Nlm_WindowGainFocus;
  modalProcs->loseFocus = Nlm_WindowLoseFocus;
  modalProcs->sendChar = Nlm_NormalDrawChar;

  floatingProcs = &(gphprcsptr [6]);
#ifdef WIN_MAC
  floatingProcs->click = Nlm_FloatingClick;
  floatingProcs->key = Nlm_FloatingKey;
  floatingProcs->draw = Nlm_NormalDraw;
  floatingProcs->idle = Nlm_FloatingIdle;
#endif
  floatingProcs->show = Nlm_ShowNormal;
  floatingProcs->hide = Nlm_HideWindow;
  floatingProcs->activate = Nlm_NormalActivate;
  floatingProcs->deactivate = Nlm_NormalDeactivate;
  floatingProcs->remove = Nlm_RemoveWindow;
  floatingProcs->select = Nlm_FloatingSelect;
  floatingProcs->linkIn = Nlm_LinkIn;
  floatingProcs->adjustPrnt = Nlm_AdjustWindow;
  floatingProcs->setTitle = Nlm_SetWindowTitle;
  floatingProcs->getTitle = Nlm_GetWindowTitle;
  floatingProcs->gainFocus = Nlm_WindowGainFocus;
  floatingProcs->loseFocus = Nlm_WindowLoseFocus;
  floatingProcs->sendChar = Nlm_FloatingDrawChar;

  shadowProcs = &(gphprcsptr [7]);
#ifdef WIN_MAC
  shadowProcs->click = Nlm_PlainClick;
  shadowProcs->key = Nlm_NormalKey;
  shadowProcs->draw = Nlm_NormalDraw;
  shadowProcs->idle = Nlm_NormalIdle;
#endif
  shadowProcs->show = Nlm_ShowNormal;
  shadowProcs->hide = Nlm_HideWindow;
  shadowProcs->activate = Nlm_NormalActivate;
  shadowProcs->deactivate = Nlm_NormalDeactivate;
  shadowProcs->remove = Nlm_RemoveWindow;
  shadowProcs->select = Nlm_PlainSelect;
  shadowProcs->linkIn = Nlm_LinkIn;
  shadowProcs->adjustPrnt = Nlm_AdjustWindow;
  shadowProcs->setTitle = Nlm_SetWindowTitle;
  shadowProcs->getTitle = Nlm_GetWindowTitle;
  shadowProcs->gainFocus = Nlm_WindowGainFocus;
  shadowProcs->loseFocus = Nlm_WindowLoseFocus;
  shadowProcs->sendChar = Nlm_NormalDrawChar;

  plainProcs = &(gphprcsptr [8]);
#ifdef WIN_MAC
  plainProcs->click = Nlm_PlainClick;
  plainProcs->key = Nlm_NormalKey;
  plainProcs->draw = Nlm_NormalDraw;
  plainProcs->idle = Nlm_NormalIdle;
#endif
  plainProcs->show = Nlm_ShowNormal;
  plainProcs->hide = Nlm_HideWindow;
  plainProcs->activate = Nlm_NormalActivate;
  plainProcs->deactivate = Nlm_NormalDeactivate;
  plainProcs->remove = Nlm_RemoveWindow;
  plainProcs->select = Nlm_PlainSelect;
  plainProcs->linkIn = Nlm_LinkIn;
  plainProcs->adjustPrnt = Nlm_AdjustWindow;
  plainProcs->setTitle = Nlm_SetWindowTitle;
  plainProcs->getTitle = Nlm_GetWindowTitle;
  plainProcs->gainFocus = Nlm_WindowGainFocus;
  plainProcs->loseFocus = Nlm_WindowLoseFocus;
  plainProcs->sendChar = Nlm_NormalDrawChar;

  desktopProcs = &(gphprcsptr [9]);
#ifdef WIN_MAC
  desktopProcs->click = Nlm_DesktopClick;
  desktopProcs->key = Nlm_DesktopKey;
#endif
  desktopProcs->show = Nlm_ShowNormal;
  desktopProcs->hide = Nlm_HideWindow;
  desktopProcs->remove = Nlm_RemoveWindow;
  desktopProcs->select = Nlm_DesktopSelect;
  desktopProcs->linkIn = Nlm_LinkIn;

  systemProcs = &(gphprcsptr [10]);
#ifdef WIN_MAC
  systemProcs->click = Nlm_SystemClick;
  systemProcs->draw = Nlm_NormalDraw;
#endif
  systemProcs->show = Nlm_ShowNormal;
  systemProcs->hide = Nlm_HideWindow;
  systemProcs->remove = Nlm_RemoveWindow;
  systemProcs->select = Nlm_NormalSelect;
  systemProcs->linkIn = Nlm_LinkIn;
}
