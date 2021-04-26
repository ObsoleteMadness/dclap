/*   vibtexts.c
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
* File Name:  vibtexts.c
*
* Author:  Jonathan Kans
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.32 $
*
* File Description: 
*       Vibrant edit text functions
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
#define Nlm_TextTool TEHandle
#endif

#ifdef WIN_MSWIN
#define Nlm_TextTool HWND
#endif

#ifdef WIN_MOTIF
#define Nlm_TextTool Widget
#endif

#define HSCROLL_POSITIONS 100

typedef  struct  Nlm_textdata {
  Nlm_TextTool     handle;
  Nlm_BaR          vScrollBar;
  Nlm_BaR          hScrollBar;
  Nlm_Boolean      wrap;
  Nlm_FonT         font;
  Nlm_Int2         texthght;
  Nlm_Boolean      active;
  Nlm_Boolean      changed;
  Nlm_Boolean      hidden;
  Nlm_Int2         visLines;
  Nlm_TxtActnProc  select;
  Nlm_TxtActnProc  deselect;
  Nlm_TxtActnProc  tabnotify;
} Nlm_TextData;

typedef  struct  Nlm_textrec {
  Nlm_GraphicRec  graphicR;
  Nlm_TextData    text;
} Nlm_TextRec, PNTR Nlm_TxtPtr;

typedef  struct  Nlm_passwdrec {
  Nlm_TextRec  textR;
  Nlm_Char     password [32];
} Nlm_PasswdRec, PNTR Nlm_PwdPtr;

static Nlm_GphPrcsPtr  gphprcsptr = NULL;

static Nlm_GphPrcsPtr  dialogTextProcs;
static Nlm_GphPrcsPtr  hiddenTextProcs;
static Nlm_GphPrcsPtr  passwordTextProcs;
static Nlm_GphPrcsPtr  scrollTextProcs;

static Nlm_TexT        recentText = NULL;
static Nlm_TextData    recentTextData;

static Nlm_TexT        currentText = NULL;

#ifdef WIN_MSWIN
static WNDPROC         lpfnNewTextProc = NULL;
static WNDPROC         lpfnOldTextProc = NULL;
static Nlm_Boolean     handlechar;
#endif

#ifdef WIN_MOTIF
static Nlm_Boolean     allowTextCallback = TRUE;
#endif

/* dgg */
Nlm_FonT		gDialogTextFont = NULL;
static Nlm_Int2		gDlogTextHeight = 0;

static void SelectDialogFont()
{
#ifdef DCLAP
	if (gDialogTextFont == NULL) 
		gDialogTextFont= Nlm_systemFont;  /* mswin,?xwin needs work to use other fonts */
	Nlm_SelectFont (gDialogTextFont); 
	gDlogTextHeight= Nlm_LineHeight ();
#else
  Nlm_SelectFont (Nlm_systemFont);
  gDialogTextFont= Nlm_systemFont;
  gDlogTextHeight= Nlm_stdLineHeight;
#endif
}


static void Nlm_LoadTextData (Nlm_TexT t, Nlm_TextTool hdl,
                              Nlm_BaR vbar, Nlm_BaR hbar,
                              Nlm_Boolean wrp, Nlm_FonT fnt,
                              Nlm_Int2 hght, Nlm_Boolean actv,
                              Nlm_Boolean chgd, Nlm_Boolean hidn,
                              Nlm_Int2 visl, Nlm_TxtActnProc slct,
                              Nlm_TxtActnProc dslct, Nlm_TxtActnProc tabn)

{
  Nlm_TextData  PNTR tdptr;
  Nlm_TxtPtr   tp;

  if (t != NULL) {
    tp = (Nlm_TxtPtr) Nlm_HandLock (t);
    tdptr = &(tp->text);
    tdptr->handle = hdl;
    tdptr->vScrollBar = vbar;
    tdptr->hScrollBar = hbar;
    tdptr->wrap = wrp;
    tdptr->font = fnt;
    tdptr->texthght = hght;
    tdptr->active = actv;
    tdptr->changed = chgd;
    tdptr->hidden = hidn;
    tdptr->visLines = visl;
    tdptr->select = slct;
    tdptr->deselect = dslct;
    tdptr->tabnotify = tabn;
    Nlm_HandUnlock (t);
    recentText = NULL;
  }
}

static void Nlm_SetTextData (Nlm_TexT t, Nlm_TextData * tdata)

{
  Nlm_TxtPtr  tp;

  if (t != NULL && tdata != NULL) {
    tp = (Nlm_TxtPtr) Nlm_HandLock (t);
    tp->text = *tdata;
    Nlm_HandUnlock (t);
    recentText = t;
    recentTextData = *tdata;
  }
}

static void Nlm_GetTextData (Nlm_TexT t, Nlm_TextData * tdata)

{
  Nlm_TxtPtr  tp;

  if (t != NULL && tdata != NULL) {
    if (t == recentText && NLM_RISKY) {
      *tdata = recentTextData;
    } else {
      tp = (Nlm_TxtPtr) Nlm_HandLock (t);
      *tdata = tp->text;
      Nlm_HandUnlock (t);
      recentText = t;
      recentTextData = *tdata;
    }
  }
}

static void Nlm_SetTextHandle (Nlm_TexT t, Nlm_TextTool hdl)

{
  Nlm_TextData  tdata;

  Nlm_GetTextData (t, &tdata);
  tdata.handle = hdl;
  Nlm_SetTextData (t, &tdata);
}

static Nlm_TextTool Nlm_GetTextHandle (Nlm_TexT t)

{
  Nlm_TextData  tdata;

  Nlm_GetTextData (t, &tdata);
  return tdata.handle;
}

static Nlm_BaR Nlm_GetTextVScrollBar (Nlm_TexT t)

{
  Nlm_TextData  tdata;

  Nlm_GetTextData (t, &tdata);
  return tdata.vScrollBar;
}

static Nlm_BaR Nlm_GetTextHScrollBar (Nlm_TexT t)

{
  Nlm_TextData  tdata;

  Nlm_GetTextData (t, &tdata);
  return tdata.hScrollBar;
}

static Nlm_Boolean Nlm_GetTextWrap (Nlm_TexT t)

{
  Nlm_TextData  tdata;

  Nlm_GetTextData (t, &tdata);
  return tdata.wrap;
}

static Nlm_Int2 Nlm_GetFontHeight (Nlm_TexT t)

{
  Nlm_TextData  tdata;

  Nlm_GetTextData (t, &tdata);
  return tdata.texthght;
}

static void Nlm_SetActive (Nlm_TexT t, Nlm_Boolean act)

{
  Nlm_TextData  tdata;

  Nlm_GetTextData (t, &tdata);
  tdata.active = act;
  Nlm_SetTextData (t, &tdata);
}

static Nlm_Boolean Nlm_GetActive (Nlm_TexT t)

{
  Nlm_TextData  tdata;

  Nlm_GetTextData (t, &tdata);
  return tdata.active;
}

static void Nlm_SetChanged (Nlm_TexT t, Nlm_Boolean chd)

{
  Nlm_TextData  tdata;

  Nlm_GetTextData (t, &tdata);
  tdata.changed = chd;
  Nlm_SetTextData (t, &tdata);
}

static Nlm_Boolean Nlm_GetChanged (Nlm_TexT t)

{
  Nlm_TextData  tdata;

  Nlm_GetTextData (t, &tdata);
  return tdata.changed;
}

static Nlm_Boolean Nlm_IsHiddenText (Nlm_TexT t)

{
  Nlm_TextData  tdata;

  Nlm_GetTextData (t, &tdata);
  return tdata.hidden;
}

static Nlm_Int2 Nlm_GetVisLines (Nlm_TexT t)

{
  Nlm_TextData  tdata;

  Nlm_GetTextData (t, &tdata);
  return tdata.visLines;
}

extern void Nlm_SetTextSelect (Nlm_TexT t, Nlm_TxtActnProc slct, Nlm_TxtActnProc dslct)

{
  Nlm_TextData  tdata;

  if (t != NULL) {
    Nlm_GetTextData (t, &tdata);
    tdata.select = slct;
    tdata.deselect = dslct;
    Nlm_SetTextData (t, &tdata);
  }
}

static void Nlm_SetPassword (Nlm_TexT t, Nlm_CharPtr passwd)

{
  Nlm_PwdPtr  pp;

  if (t != NULL && passwd != NULL) {
    pp = (Nlm_PwdPtr) Nlm_HandLock (t);
    Nlm_StringNCpy ((Nlm_CharPtr) pp->password, passwd, 31);
    Nlm_HandUnlock (t);
  }
}

static void Nlm_GetPassword (Nlm_TexT t, Nlm_CharPtr passwd, Nlm_sizeT maxsize)

{
  Nlm_PwdPtr  pp;

  if (t != NULL && passwd != NULL) {
    pp = (Nlm_PwdPtr) Nlm_HandLock (t);
    Nlm_StringNCpy (passwd, (Nlm_CharPtr) pp->password, maxsize);
    Nlm_HandUnlock (t);
  }
}

#ifdef WIN_MAC
static Nlm_Int2 Nlm_GetTextLines (Nlm_TexT t)

{
  Nlm_TextTool  h;
  Nlm_Int2      lines;
  TEPtr         tptr;

  h = Nlm_GetTextHandle (t);
  HLock ((Handle) h);
  tptr = (TEPtr) *((Handle) h);
  lines = tptr->nLines;
  HUnlock ((Handle) h);
  return lines;
}

static Nlm_Int2 Nlm_GetLineHeight (Nlm_TexT t)

{
  Nlm_TextTool  h;
  Nlm_Int2      height;
  TEPtr         tptr;

  h = Nlm_GetTextHandle (t);
  HLock ((Handle) h);
  tptr = (TEPtr) *((Handle) h);
  height = tptr->lineHeight;
  HUnlock ((Handle) h);
  return height;
}

static Nlm_Int2 Nlm_GetInsertionStartLine (Nlm_TexT t)

{
  Nlm_TextTool  h;
  Nlm_Int2      i;
  short         *lines;
  Nlm_Int2      numLines;
  Nlm_Int2      startLoc;
  TEPtr         tptr;

  h = Nlm_GetTextHandle (t);
  HLock ((Handle) h);
  tptr = (TEPtr) *((Handle) h);
  numLines = tptr->nLines;
  startLoc = tptr->selStart;
  i = 0;
  lines = &(tptr->lineStarts[0]);
  while (i < numLines && startLoc > lines [i]) {
    i++;
  }
  HUnlock ((Handle) h);
  return i;
}

static void Nlm_ScrollToInsertionPoint (Nlm_TexT t)

{
  Nlm_BaR   sb;
  Nlm_Int2  start;
  Nlm_Int2  val;
  Nlm_Int2  vis;

  sb = Nlm_GetTextVScrollBar (t);
  if (sb != NULL) {
    start = Nlm_GetInsertionStartLine (t);
    vis = Nlm_GetVisLines (t);
    val = Nlm_DoGetValue ((Nlm_GraphiC) sb);
    if (val + vis < start) {
      Nlm_DoSetValue ((Nlm_GraphiC) sb, start - (vis / 2), TRUE);
    } else if (val > start) {
      Nlm_DoSetValue ((Nlm_GraphiC) sb, start - (vis / 2), TRUE);
    }
  }
}

static void Nlm_UpdateScrollBar (Nlm_TexT t)

{
  Nlm_Int2  lines;
  Nlm_Int2  newval;
  Nlm_BaR   sb;
  Nlm_Int2  start;
  Nlm_Int2  vis;

  sb = Nlm_GetTextVScrollBar (t);
  if (sb != NULL) {
    lines = Nlm_GetTextLines (t);
    start = Nlm_GetInsertionStartLine (t);
    vis = Nlm_GetVisLines (t);
    newval = 0;
    if (lines > vis) {
      newval = lines - vis;
    }
    Nlm_DoSetRange ((Nlm_GraphiC) sb, vis - 1, vis - 1, newval, FALSE);
    if (Nlm_DoGetValue ((Nlm_GraphiC) sb) > newval) {
      Nlm_DoSetValue ((Nlm_GraphiC) sb, newval, FALSE);
    }
  }
  sb = Nlm_GetTextHScrollBar (t);
  if (sb != NULL) {
    Nlm_CorrectBarMax (sb, HSCROLL_POSITIONS);
  }
}
#endif

static void Nlm_DeactivateBoxesInList (Nlm_TexT t)

{
  Nlm_WindoW  w;

  w = Nlm_GetParentWindow ((Nlm_GraphiC) t);
  Nlm_DoLoseFocus ((Nlm_GraphiC) w, (Nlm_GraphiC) t, FALSE);
}

static void Nlm_DoTextSelect (Nlm_TexT t)

{
  Nlm_TxtActnProc  sel;
  Nlm_TextData     tdata;

  if (t != NULL) {
    Nlm_GetTextData (t, &tdata);
    sel = tdata.select;
    if (sel != NULL && currentText != t) {
      currentText = t;
      sel (t);
    }
  }
  currentText = t;
}

static void Nlm_DoTextDeselect (Nlm_TexT t)

{
  Nlm_TxtActnProc  desel;
  Nlm_TextData     tdata;

  if (t != NULL) {
    Nlm_GetTextData (t, &tdata);
    desel = tdata.deselect;
    if (desel != NULL) {
      desel (t);
    }
  }
}

static void Nlm_DoTabCallback (Nlm_TexT t)

{
  Nlm_TextData  tdata;

  Nlm_GetTextData (t, &tdata);
  if (tdata.tabnotify != NULL) {
    tdata.tabnotify (t);
  }
}



#ifdef DCLAP
extern void Nlm_TextEnableNewlines(Nlm_TexT t, Nlm_Boolean turnon)
{
#ifdef WIN_MAC
  Nlm_TextTool  h;
  h = Nlm_GetTextHandle (t);
	if (turnon) (**h).crOnly= 0;
	else (**h).crOnly= -1;
#endif
}
#endif


static void Nlm_SelectAText (Nlm_TexT t, Nlm_Int2 begin, Nlm_Int2 end)

{
#ifdef WIN_MAC
  Nlm_TextTool  h;
  TEPtr         hp;
  short         len;
  Nlm_Int4      selStart;
  Nlm_Int4      selEnd;

  Nlm_DeactivateBoxesInList (t);
  h = Nlm_GetTextHandle (t);
  HLock ((Handle) h);
  hp = (TEPtr) *((Handle) h);
  len = hp->teLength;
  HUnlock ((Handle) h);
  selStart = begin;
  selEnd = end;
  if (selEnd > (Nlm_Int4) len) {
    selEnd = (Nlm_Int4) len;
  }
  TESetSelect (selStart, selEnd, h);
  Nlm_SetActive (t, TRUE);
  Nlm_DoActivate ((Nlm_GraphiC) t, FALSE);
  Nlm_DoTextSelect (t);
#endif
#ifdef WIN_MSWIN
  Nlm_TextTool  h;

  h = Nlm_GetTextHandle (t);
  Edit_SetSel (h, begin, end);
  if (Nlm_Visible (t) && Nlm_AllParentsVisible (t)) {
    SetFocus (h);
    Nlm_DoTextSelect (t);
  }
#endif
#ifdef WIN_MOTIF
  Nlm_TextTool    h;
  XmTextPosition  max;

  if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (t))) {
    allowTextCallback = FALSE;
    h = Nlm_GetTextHandle (t);
    XmProcessTraversal (h, XmTRAVERSE_CURRENT);
    max = XmTextGetLastPosition (h);
    if (max > (XmTextPosition) end) {
      max = (XmTextPosition) end;
    }
    XmTextSetSelection (h, (XmTextPosition) begin,
                        (XmTextPosition) max, (Time) 0);
    XmTextSetHighlight (h, (XmTextPosition) begin,
                        (XmTextPosition) max, XmHIGHLIGHT_SELECTED);
    allowTextCallback = TRUE;
  }
  Nlm_DoTextSelect (t);
#endif
}

extern void Nlm_TextSelectionRange (Nlm_TexT t, Nlm_Int2Ptr begin, Nlm_Int2Ptr end)

{
#ifdef WIN_MAC
  Nlm_TextTool  h;
  TEPtr         hp;
  Nlm_Int4      selStart;
  Nlm_Int4      selEnd;

  if (t != NULL) {
    h = Nlm_GetTextHandle (t);
    HLock ((Handle) h);
    hp = (TEPtr) *((Handle) h);
    selStart = hp->selStart;
    selEnd = hp->selEnd;
    HUnlock ((Handle) h);
    if (begin != NULL) {
      *begin = (Nlm_Int2) selStart;
    }
    if (end != NULL) {
      *end = (Nlm_Int2) selEnd;
    }
  }
#endif
#ifdef WIN_MSWIN
  Nlm_TextTool  h;
  DWORD         val;

  if (t != NULL) {
    h = Nlm_GetTextHandle (t);
    val = Edit_GetSel (h);
    if (begin != NULL) {
      *begin = (Nlm_Int2) (LOWORD (val));
    }
    if (end != NULL) {
      *end = (Nlm_Int2) (HIWORD (val));
    }
  }
#endif
#ifdef WIN_MOTIF
  Nlm_TextTool    h;
  XmTextPosition  left;
  XmTextPosition  right;

  if (t != NULL) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (t))) {
      h = Nlm_GetTextHandle (t);
      XmTextGetSelectionPosition (h, &left, &right);
      if (begin != NULL) {
        *begin = (Nlm_Int2) left;
      }
      if (end != NULL) {
        *end = (Nlm_Int2) right;
      }
    }
  }
#endif
}

#ifdef WIN_MAC
static Nlm_Boolean Nlm_DialogTextClick (Nlm_GraphiC t, Nlm_PoinT pt)

{
  Nlm_TextTool   h;
  Nlm_PointTool  ptool;
  Nlm_RecT       r;
  Nlm_Boolean    rsult;
  Nlm_WindoW     w;

  rsult = FALSE;
  Nlm_GetRect (t, &r);
  if (Nlm_PtInRect (pt, &r)) {
    Nlm_DeactivateBoxesInList ((Nlm_TexT) t);
    h = Nlm_GetTextHandle ((Nlm_TexT) t);
    Nlm_GetRect (t, &r);
    Nlm_PoinTToPointTool (pt, &ptool);
    TEClick (ptool, Nlm_shftKey, h);
    Nlm_SetActive ((Nlm_TexT) t, TRUE);
    w = Nlm_GetParentWindow (t);
    Nlm_DoActivate (t, FALSE);
    do {
    } while (Nlm_MouseButton ());
    Nlm_DoTextSelect ((Nlm_TexT) t);
    rsult = TRUE;
  }
  return rsult;
}

static Nlm_Boolean Nlm_ScrollTextClick (Nlm_GraphiC t, Nlm_PoinT pt)

{
  Nlm_TextTool   h;
  Nlm_BaR        hsb;
  Nlm_PointTool  ptool;
  Nlm_RecT       r;
  Nlm_Boolean    rsult;
  Nlm_BaR        vsb;
  Nlm_WindoW     w;
  Nlm_Boolean    wrap;

  rsult = FALSE;
  Nlm_GetRect (t, &r);
  wrap = Nlm_GetTextWrap ((Nlm_TexT) t);
  r.right += Nlm_vScrollBarWidth;
  if (! wrap) {
    r.bottom += Nlm_hScrollBarHeight;
  }
  if (Nlm_PtInRect (pt, &r)) {
    vsb = Nlm_GetTextVScrollBar ((Nlm_TexT) t);
    hsb = Nlm_GetTextHScrollBar ((Nlm_TexT) t);
    if (vsb != NULL && Nlm_DoClick ((Nlm_GraphiC) vsb, pt)) {
    } else if (hsb != NULL && Nlm_DoClick ((Nlm_GraphiC) hsb, pt)) {
    } else {
      Nlm_DeactivateBoxesInList ((Nlm_TexT) t);
      h = Nlm_GetTextHandle ((Nlm_TexT) t);
      r.right -= Nlm_vScrollBarWidth;
      if (! wrap) {
        r.bottom -= Nlm_hScrollBarHeight;
      }
      Nlm_PoinTToPointTool (pt, &ptool);
      TEClick (ptool, Nlm_shftKey, h);

#ifdef DCLAP
/* only call the DoActivate if we were inactive -- it is a big time drag w/ TERecal ! */
/* note that SetActive must be done before DoActivate call !! */
  		{
		  Nlm_Boolean   act;
  		act = Nlm_GetActive ((Nlm_TexT) t);
      Nlm_SetActive ((Nlm_TexT) t, TRUE);
      if (!act) Nlm_DoActivate (t, FALSE);
      }
#else
      Nlm_SetActive ((Nlm_TexT) t, TRUE);
      w = Nlm_GetParentWindow (t);
      Nlm_DoActivate (t, FALSE);
#endif

      do {
      } while (Nlm_MouseButton ());
      Nlm_DoTextSelect ((Nlm_TexT) t);
      rsult = TRUE;
    }
  }
  return rsult;
}

static Nlm_Boolean Nlm_TextKey (Nlm_GraphiC t, Nlm_Char ch)

{
  Nlm_Boolean   act;
  Nlm_TextTool  h;
  Nlm_Boolean   rsult;

  rsult = FALSE;
  act = Nlm_GetActive ((Nlm_TexT) t);
  if (act && ! Nlm_cmmdKey) {
    if (ch != '\0') {
      h = Nlm_GetTextHandle ((Nlm_TexT) t);
      Nlm_ScrollToInsertionPoint ((Nlm_TexT) t);
      TEKey (ch, h);
      TECalText (h);
      Nlm_UpdateScrollBar ((Nlm_TexT) t);
      Nlm_ScrollToInsertionPoint ((Nlm_TexT) t);
      Nlm_DoAction (t);
      Nlm_SetChanged ((Nlm_TexT) t, TRUE);
      rsult = TRUE;
    }
  }
  return rsult;
}

static Nlm_Boolean Nlm_DialogKey (Nlm_GraphiC t, Nlm_Char ch)

{
  Nlm_Boolean  act;
  Nlm_Boolean  rsult;

  rsult = FALSE;
  act = Nlm_GetActive ((Nlm_TexT) t);
  if (act && ! Nlm_cmmdKey) {
    if (ch == '\t' && Nlm_IsHiddenText ((Nlm_TexT) t)) {
      Nlm_DoTabCallback ((Nlm_TexT) t);
    } else if (ch == '\t') {
      Nlm_DoSendFocus (t, ch);
      rsult = TRUE;
    } else if (ch == '\n'  || ch == '\r' || ch == '\3') {
    } else if (ch != '\0') {
      rsult = Nlm_TextKey (t, ch);
    }
  }
  return rsult;
}

static Nlm_Boolean Nlm_PasswordKey (Nlm_GraphiC t, Nlm_Char ch)

{
  Nlm_Boolean   act;
  Nlm_TextTool  h;
  TEPtr         hp;
  Nlm_Int2      len;
  Nlm_Char      password [32];
  Nlm_RecT      r;
  Nlm_RectTool  rtool;
  Nlm_Boolean   rsult;
  Nlm_Int4      selStart;
  Nlm_Int4      selEnd;

  rsult = FALSE;
  act = Nlm_GetActive ((Nlm_TexT) t);
  if (act && ! Nlm_cmmdKey) {
    if (ch == '\t') {
      Nlm_DoSendFocus (t, ch);
      rsult = TRUE;
    } else if (ch == '\n'  || ch == '\r' || ch == '\3') {
    } else if (ch == '\b') {
      rsult = TRUE;
      Nlm_SetPassword ((Nlm_TexT) t, "");
      h = Nlm_GetTextHandle ((Nlm_TexT) t);
      selStart = 0;
      HLock ((Handle) h);
      hp = (TEPtr) *((Handle) h);
      selEnd = hp->teLength;
      HUnlock ((Handle) h);
      TESetSelect (selStart, selEnd, h);
      TEDelete (h);
      Nlm_GetRect (t, &r);
      if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
        Nlm_InsetRect (&r, 2, 2);
        Nlm_EraseRect (&r);
        Nlm_RecTToRectTool (&r, &rtool);
        TEUpdate (&rtool, h);
      }
    } else if (ch != '\0') {
      Nlm_GetPassword ((Nlm_TexT) t, password, sizeof (password));
      len = (Nlm_Int2) Nlm_StringLen (password);
      if (len < sizeof (password) - 2) {
        password [len] = ch;
        password [len + 1] = '\0';
      }
      Nlm_SetPassword ((Nlm_TexT) t, password);
      rsult = Nlm_TextKey (t, '*');
    }
  }
  return rsult;
}

static void Nlm_DrawDialogText (Nlm_GraphiC t)

{
  Nlm_TextTool  h;
  Nlm_RecT      r;
  Nlm_RectTool  ttool;

  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    Nlm_GetRect (t, &r);
    if (Nlm_RectInRgn (&r, Nlm_updateRgn)) {
      Nlm_EraseRect (&r);
      if (Nlm_GetEnabled (t) && Nlm_GetAllParentsEnabled (t)) {
        h = Nlm_GetTextHandle ((Nlm_TexT) t);
        if (Nlm_GetActive ((Nlm_TexT) t)) {
          TEActivate (h);
        } else {
          TEDeactivate (h);
        }
        TECalText (h);
        Nlm_InsetRect (&r, 2, 2);
        Nlm_RecTToRectTool (&r, &ttool);
        SelectDialogFont();
        TEUpdate (&ttool, h);
        Nlm_InsetRect (&r, -2, -2);
        Nlm_FrameRect (&r);
      } else {
#ifdef DCLAP
	/* dgg -- we want to show this text, just not edit it... */
        h = Nlm_GetTextHandle ((Nlm_TexT) t);
    		TEDeactivate (h);
        TECalText (h);
        Nlm_InsetRect (&r, 2, 2);
        Nlm_RecTToRectTool (&r, &ttool);
				SelectDialogFont();
        TEUpdate (&ttool, h);
        Nlm_InsetRect (&r, -2, -2);
#endif
        Nlm_Dotted ();
        Nlm_FrameRect (&r);
        Nlm_Solid ();
      }
    }
  }
}

static void Nlm_DrawHiddenText (Nlm_GraphiC t)

{
  Nlm_TextTool  h;
  Nlm_RecT      r;
  Nlm_RectTool  ttool;

  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    Nlm_GetRect (t, &r);
    if (Nlm_RectInRgn (&r, Nlm_updateRgn)) {
      Nlm_EraseRect (&r);
      if (Nlm_GetEnabled (t) && Nlm_GetAllParentsEnabled (t)) {
        h = Nlm_GetTextHandle ((Nlm_TexT) t);
        if (Nlm_GetActive ((Nlm_TexT) t)) {
          TEActivate (h);
        } else {
          TEDeactivate (h);
        }
        TECalText (h);
        Nlm_RecTToRectTool (&r, &ttool);
				SelectDialogFont();
        TEUpdate (&ttool, h);
      }
    }
  }
}

static void Nlm_DrawScrollText (Nlm_GraphiC t)

{
  Nlm_TextTool  h;
  Nlm_BaR       hsb;
  Nlm_RecT      r;
  Nlm_RectTool  rtool;
  Nlm_BaR       vsb;
  Nlm_Boolean   wrap;

  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    Nlm_GetRect (t, &r);
    r.right += Nlm_vScrollBarWidth;
    wrap = Nlm_GetTextWrap ((Nlm_TexT) t);
    if (! wrap) {
      r.bottom += Nlm_hScrollBarHeight;
    }
    if (Nlm_RectInRgn (&r, Nlm_updateRgn)) {
      if (Nlm_GetEnabled (t) && Nlm_GetAllParentsEnabled (t)) {
        Nlm_FrameRect (&r);
        vsb = Nlm_GetTextVScrollBar ((Nlm_TexT) t);
        if (vsb != NULL) {
          Nlm_DoDraw ((Nlm_GraphiC) vsb);
        }
        hsb = Nlm_GetTextHScrollBar ((Nlm_TexT) t);
        if (hsb != NULL) {
          Nlm_DoDraw ((Nlm_GraphiC) hsb);
        }
        r.right -= Nlm_vScrollBarWidth;
        if (! wrap) {
          r.bottom -= Nlm_hScrollBarHeight;
        }
        Nlm_InsetRect (&r, 4, 2);
        r.bottom = r.top + Nlm_GetVisLines ((Nlm_TexT) t) * Nlm_GetFontHeight ((Nlm_TexT) t);
        h = Nlm_GetTextHandle ((Nlm_TexT) t);
        if (Nlm_GetActive ((Nlm_TexT) t)) {
          TEActivate (h);
        } else {
          TEDeactivate (h);
        }
        TECalText (h);
        Nlm_RecTToRectTool (&r, &rtool);
				SelectDialogFont();
        TEUpdate (&rtool, h);
      } else {
#ifdef DCLAP
 		/* dgg -- we want to see text, just not edit it */
 		/* this may need some work */
        h = Nlm_GetTextHandle ((Nlm_TexT) t);
        TEDeactivate (h);
        TECalText (h);
        Nlm_RecTToRectTool (&r, &rtool);
				SelectDialogFont();
        TEUpdate (&rtool, h);
#else
        Nlm_EraseRect (&r);
#endif
        Nlm_Dotted ();
        Nlm_FrameRect (&r);
        Nlm_Solid ();
      }
    }
  }
}

static Nlm_Boolean Nlm_IdleText (Nlm_GraphiC t, Nlm_PoinT pt)

{  Nlm_TextTool  h;

  if (Nlm_GetVisible (t) && Nlm_GetEnabled (t) && Nlm_GetActive ((Nlm_TexT) t)) {
    h = Nlm_GetTextHandle ((Nlm_TexT) t);
    TEIdle (h);
  }
  return TRUE;
}
#endif

#ifdef WIN_MSWIN
static Nlm_Boolean Nlm_DialogTextCommand (Nlm_GraphiC t)

{
  if (Nlm_currentCode == EN_CHANGE) {
    Nlm_DoAction (t);
  }
  return TRUE;
}

static Nlm_Boolean Nlm_PasswordTextCommand (Nlm_GraphiC t)

{
  Nlm_TextTool  h;
  Nlm_Char      password [32];

  if (Nlm_currentCode == EN_CHANGE) {
    h = Nlm_GetTextHandle ((Nlm_TexT) t);
    GetWindowText (h, password, sizeof (password) - 2);
    Nlm_SetPassword ((Nlm_TexT) t, password);
    Nlm_DoAction (t);
  }
  return TRUE;
}

static Nlm_Boolean Nlm_ScrollTextCommand (Nlm_GraphiC t)

{
  if (Nlm_currentCode == EN_CHANGE) {
    Nlm_DoAction (t);
  }
  return TRUE;
}
#endif

#ifdef WIN_MOTIF
#endif

static void Nlm_ShowText (Nlm_GraphiC t, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
  Nlm_WindoW    tempPort;

  if (setFlag) {
    Nlm_SetVisible (t, TRUE);
  }
  if (Nlm_GetVisible (t) && Nlm_AllParentsButWindowVisible (t)) {
    tempPort = Nlm_SavePortIfNeeded (t, savePort);
    h = Nlm_GetTextHandle ((Nlm_TexT) t);
#ifdef WIN_MAC
    Nlm_DoDraw (t);
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

static void Nlm_ShowScrollText (Nlm_GraphiC t, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
  Nlm_BaR       sb;
  Nlm_WindoW    tempPort;

  if (setFlag) {
    Nlm_SetVisible (t, TRUE);
  }
  if (Nlm_GetVisible (t) && Nlm_AllParentsButWindowVisible (t)) {
    tempPort = Nlm_SavePortIfNeeded (t, savePort);
    sb = Nlm_GetTextVScrollBar ((Nlm_TexT) t);
    if (sb != NULL) {
      Nlm_DoShow ((Nlm_GraphiC) sb, TRUE, FALSE);
    }
    sb = Nlm_GetTextHScrollBar ((Nlm_TexT) t);
    if (sb != NULL) {
      Nlm_DoShow ((Nlm_GraphiC) sb, TRUE, FALSE);
    }
    h = Nlm_GetTextHandle ((Nlm_TexT) t);
#ifdef WIN_MAC
    Nlm_DoDraw (t);
#endif
#ifdef WIN_MSWIN
    ShowWindow (h, SW_SHOW);
    UpdateWindow (h);
#endif
#ifdef WIN_MOTIF
    XtManageChild (XtParent (h));
#endif
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_HideText (Nlm_GraphiC t, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  Nlm_RecT      r;
#endif

  if (setFlag) {
    Nlm_SetVisible (t, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (t, savePort);
  h = Nlm_GetTextHandle ((Nlm_TexT) t);
#ifdef WIN_MAC
  if (Nlm_GetAllParentsVisible (t)) {
    Nlm_GetRect (t, &r);
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

static void Nlm_HideScrollText (Nlm_GraphiC t, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
  Nlm_BaR       sb;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  Nlm_RecT      r;
#endif

  if (setFlag) {
    Nlm_SetVisible (t, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (t, savePort);
  sb = Nlm_GetTextVScrollBar ((Nlm_TexT) t);
  if (sb != NULL) {
    Nlm_DoHide ((Nlm_GraphiC) sb, TRUE, FALSE);
  }
  sb = Nlm_GetTextHScrollBar ((Nlm_TexT) t);
  if (sb != NULL) {
    Nlm_DoHide ((Nlm_GraphiC) sb, TRUE, FALSE);
  }
  h = Nlm_GetTextHandle ((Nlm_TexT) t);
#ifdef WIN_MAC
  if (Nlm_GetAllParentsVisible (t)) {
    Nlm_GetRect (t, &r);
    r.right += Nlm_vScrollBarWidth;
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
  XtUnmanageChild (XtParent (h));
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_EnableText (Nlm_GraphiC t, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
  Nlm_BaR       sb;
  Nlm_WindoW    tempPort;

  if (setFlag) {
    Nlm_SetEnabled (t, TRUE);
  }
  if (Nlm_GetEnabled (t) && Nlm_GetAllParentsEnabled (t)) {
    tempPort = Nlm_SavePortIfNeeded (t, savePort);
    sb = Nlm_GetTextVScrollBar ((Nlm_TexT) t);
    if (sb != NULL) {
      Nlm_DoEnable ((Nlm_GraphiC) sb, TRUE, FALSE);
    }
    sb = Nlm_GetTextHScrollBar ((Nlm_TexT) t);
    if (sb != NULL) {
      Nlm_DoEnable ((Nlm_GraphiC) sb, TRUE, FALSE);
    }
    h = Nlm_GetTextHandle ((Nlm_TexT) t);
#ifdef WIN_MAC
    if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
      Nlm_DoDraw (t);
    }
#endif
#ifdef WIN_MSWIN
    EnableWindow (h, TRUE);
#endif
#ifdef WIN_MOTIF
    XtVaSetValues (h, XmNsensitive, TRUE, NULL);
#endif
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_DisableText (Nlm_GraphiC t, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
  Nlm_BaR       sb;
  Nlm_WindoW    tempPort;

  if (setFlag) {
    Nlm_SetEnabled (t, FALSE);
  }
  tempPort = Nlm_SavePortIfNeeded (t, savePort);
  sb = Nlm_GetTextVScrollBar ((Nlm_TexT) t);
  if (sb != NULL) {
    Nlm_DoDisable ((Nlm_GraphiC) sb, TRUE, FALSE);
  }
  sb = Nlm_GetTextHScrollBar ((Nlm_TexT) t);
  if (sb != NULL) {
    Nlm_DoDisable ((Nlm_GraphiC) sb, TRUE, FALSE);
  }
  h = Nlm_GetTextHandle ((Nlm_TexT) t);
#ifdef WIN_MAC
  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    Nlm_DoDraw (t);
  }
#endif
#ifdef WIN_MSWIN
  EnableWindow (h, FALSE);
#endif
#ifdef WIN_MOTIF
  XtVaSetValues (h, XmNsensitive, FALSE, NULL);
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_ActivateText (Nlm_GraphiC t, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_TextTool  h;
  Nlm_RecT      r;
  Nlm_RectTool  rtool;
#endif

  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    if (Nlm_GetEnabled (t) && Nlm_GetAllParentsEnabled (t) && Nlm_GetActive ((Nlm_TexT) t)) {
#ifdef WIN_MAC
      h = Nlm_GetTextHandle ((Nlm_TexT) t);
      Nlm_GetRect (t, &r);
      Nlm_InsetRect (&r, 2, 2);
      TEActivate (h);
      TECalText (h);
      Nlm_RecTToRectTool (&r, &rtool);
      TEUpdate (&rtool, h);
#endif
#ifdef WIN_MSWIN
      if (t != NULL) {
        Nlm_SelectAText ((Nlm_TexT) t, 0, 32767);
      }
#endif
#ifdef WIN_MOTIF
      if (t != NULL) {
        Nlm_SelectAText ((Nlm_TexT) t, 0, 32767);
      }
#endif
    }
  }
}

static void Nlm_ActivateHiddenText (Nlm_GraphiC t, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_TextTool  h;
  Nlm_RecT      r;
  Nlm_RectTool  rtool;
#endif
#ifdef WIN_MSWIN
  Nlm_TextTool  h;
#endif

  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    if (Nlm_GetEnabled (t) && Nlm_GetAllParentsEnabled (t) && Nlm_GetActive ((Nlm_TexT) t)) {
#ifdef WIN_MAC
      h = Nlm_GetTextHandle ((Nlm_TexT) t);
      Nlm_GetRect (t, &r);
      TEActivate (h);
      TECalText (h);
      Nlm_RecTToRectTool (&r, &rtool);
      TEUpdate (&rtool, h);
#endif
#ifdef WIN_MSWIN
      h = Nlm_GetTextHandle ((Nlm_TexT) t);
      if (t != NULL && h != NULL) {
        if (Nlm_Visible (t) && Nlm_AllParentsVisible (t)) {
          SetFocus (h);
        }
      }
#endif
    }
  }
}

static void Nlm_ActivateScrollText (Nlm_GraphiC t, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
#ifdef WIN_MAC
  Nlm_RecT      r;
  Nlm_RectTool  rtool;
#endif

  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    if (Nlm_GetEnabled (t) && Nlm_GetAllParentsEnabled (t) && Nlm_GetActive ((Nlm_TexT) t)) {
#ifdef WIN_MAC
      h = Nlm_GetTextHandle ((Nlm_TexT) t);
      Nlm_GetRect (t, &r);
      Nlm_InsetRect (&r, 4, 2);
      r.bottom = r.top + Nlm_GetVisLines ((Nlm_TexT) t) * Nlm_GetFontHeight ((Nlm_TexT) t);
      TEActivate (h);
      TECalText (h);
      Nlm_RecTToRectTool (&r, &rtool);
      TEUpdate (&rtool, h);
#endif
#ifdef WIN_MSWIN
      h = Nlm_GetTextHandle ((Nlm_TexT) t);
      if (t != NULL && h != NULL) {
        if (Nlm_Visible (t) && Nlm_AllParentsVisible (t)) {
          SetFocus (h);
        }
      }
#endif
    }
  }
}

static void Nlm_DeactivateText (Nlm_GraphiC t, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_TextTool  h;
  Nlm_RecT      r;
  Nlm_RectTool  rtool;

  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    if (Nlm_GetEnabled (t) && Nlm_GetAllParentsEnabled (t)) {
      h = Nlm_GetTextHandle ((Nlm_TexT) t);
      Nlm_GetRect (t, &r);
      Nlm_InsetRect (&r, 2, 2);
      TEDeactivate (h);
      TECalText (h);
      Nlm_RecTToRectTool (&r, &rtool);
      TEUpdate (&rtool, h);
    }
  }
#endif
}

static void Nlm_DeactivateScrollText (Nlm_GraphiC t, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_TextTool  h;
  Nlm_RecT      r;
  Nlm_RectTool  rtool;

  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    if (Nlm_GetEnabled (t) && Nlm_GetAllParentsEnabled (t)) {
      h = Nlm_GetTextHandle ((Nlm_TexT) t);
      Nlm_GetRect (t, &r);
      Nlm_InsetRect (&r, 4, 2);
      r.bottom = r.top + Nlm_GetVisLines ((Nlm_TexT) t) * Nlm_GetFontHeight ((Nlm_TexT) t);
      TEDeactivate (h);
      TECalText (h);
      Nlm_RecTToRectTool (&r, &rtool);
      TEUpdate (&rtool, h);
    }
  }
#endif
}

static void Nlm_ResetText (Nlm_GraphiC t, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_Int2      delta;
  Nlm_TextTool  h;
  Nlm_Int2      height;
  Nlm_BaR       sb;
  Nlm_WindoW    tempPort;
  Nlm_Int2      width;

  tempPort = Nlm_SavePortIfNeeded (t, savePort);
  h = Nlm_GetTextHandle ((Nlm_TexT) t);
  TESetSelect (0, 32767, h);
  TEDelete (h);
  sb = Nlm_GetTextVScrollBar ((Nlm_TexT) t);
  if (sb != NULL) {
    delta = Nlm_DoGetValue ((Nlm_GraphiC) sb);
    height = Nlm_GetLineHeight ((Nlm_TexT) t);
    TEScroll (0, delta * height, h);
    Nlm_DoReset ((Nlm_GraphiC) sb, FALSE);
  }
  sb = Nlm_GetTextHScrollBar ((Nlm_TexT) t);
  if (sb != NULL) {
    delta = Nlm_DoGetValue ((Nlm_GraphiC) sb);
    width = Nlm_stdCharWidth;
    TEScroll (delta * width, 0, h);
    Nlm_DoReset ((Nlm_GraphiC) sb, FALSE);
  }
  Nlm_RestorePort (tempPort);
#endif
#ifdef WIN_MSWIN
  Nlm_TextTool  h;

  h = Nlm_GetTextHandle ((Nlm_TexT) t);
  SetWindowText (h, "");
#endif
#ifdef WIN_MOTIF
  Nlm_TextTool  h;
  Nlm_Char      str [5];

  h = Nlm_GetTextHandle ((Nlm_TexT) t);
  allowTextCallback = FALSE;
  str [0] = '\0';
  XmTextSetString (h, str);
  XmTextShowPosition (h, 0);
  allowTextCallback = TRUE;
#endif
}

static void Nlm_RemoveText (Nlm_GraphiC t, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  Nlm_BaR       sb;
#endif

  tempPort = Nlm_SavePortIfNeeded (t, savePort);
  if (currentText == (Nlm_TexT) t) {
    currentText = NULL;
  }
  h = Nlm_GetTextHandle ((Nlm_TexT) t);
#ifdef WIN_MAC
  TEDispose (h);
  sb = Nlm_GetTextVScrollBar ((Nlm_TexT) t);
  if (sb != NULL) {
    Nlm_DoRemove ((Nlm_GraphiC) sb, FALSE);
  }
  sb = Nlm_GetTextHScrollBar ((Nlm_TexT) t);
  if (sb != NULL) {
    Nlm_DoRemove ((Nlm_GraphiC) sb, FALSE);
  }
#endif
#ifdef WIN_MSWIN
  RemoveProp (h, (LPSTR) "Nlm_VibrantProp");
  DestroyWindow (h);
#endif
#ifdef WIN_MOTIF
  XtDestroyWidget (h);
#endif
  Nlm_RemoveLink (t);
  recentText = NULL;
  Nlm_RestorePort (tempPort);
}

static void Nlm_TextSelectProc (Nlm_GraphiC t, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_Int2      end;
  Nlm_TextTool  h;
  TEPtr         hp;
  Nlm_WindoW    tempPort;

  if (t != NULL) {
    tempPort = Nlm_SavePortIfNeeded (t, savePort);
    h = Nlm_GetTextHandle ((Nlm_TexT) t);
    HLock ((Handle) h);
    hp = (TEPtr) *((Handle) h);
    end = hp->teLength;
    HUnlock ((Handle) h);
    Nlm_SelectAText ((Nlm_TexT) t, 0, end);
    Nlm_RestorePort (tempPort);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_WindoW  tempPort;

  if (t != NULL) {
    tempPort = Nlm_SavePortIfNeeded (t, savePort);
    Nlm_SelectAText ((Nlm_TexT) t, 0, 32767);
    Nlm_RestorePort (tempPort);
  }
#endif
#ifdef WIN_MOTIF
  Nlm_WindoW  tempPort;

  if (t != NULL) {
    tempPort = Nlm_SavePortIfNeeded (t, savePort);
    Nlm_SelectAText ((Nlm_TexT) t, 0, 32767);
    Nlm_RestorePort (tempPort);
  }
#endif
}

extern Nlm_sizeT Nlm_TextLength (Nlm_TexT t)

{
  Nlm_TextTool  h;
  Nlm_sizeT        len;
#ifdef WIN_MAC
  TEPtr         hp;
#endif
#ifdef WIN_MOTIF
  Nlm_CharPtr   ptr;
#endif

  len = 0;
  if (t != NULL) {
    h = Nlm_GetTextHandle (t);
#ifdef WIN_MAC
    HLock ((Handle) h);
    hp = (TEPtr) *((Handle) h);
    len = hp->teLength;
    HUnlock ((Handle) h);
#endif
#ifdef WIN_MSWIN
    len = (Nlm_sizeT) GetWindowTextLength (h);
#endif
#ifdef WIN_MOTIF
    ptr = XmTextGetString (h);
    len = Nlm_StringLen (ptr);
    XtFree (ptr);
#endif
  }
  return len;
}

extern void Nlm_SelectText (Nlm_TexT t, Nlm_Int2 begin, Nlm_Int2 end)

{
  Nlm_WindoW  tempPort;

  if (t != NULL) {
    tempPort = Nlm_SavePortIfNeeded ((Nlm_GraphiC) t, TRUE);
    begin = MAX (begin, 0);
    end = MAX (end, 0);
    Nlm_SelectAText ((Nlm_TexT) t, begin, end);
    Nlm_RestorePort (tempPort);
  }
}

static void Nlm_SetDialogText (Nlm_GraphiC t, Nlm_Int2 item,
                               Nlm_CharPtr title, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  TEPtr         hp;
  Nlm_Uint4     len;
  Nlm_RecT      r;
  Nlm_Int4      selStart;
  Nlm_Int4      selEnd;
#endif
#ifdef WIN_MOTIF
  Nlm_CharPtr   str;
#endif

  tempPort = Nlm_SavePortIfNeeded (t, savePort);
  h = Nlm_GetTextHandle ((Nlm_TexT) t);
#ifdef WIN_MAC
  len = Nlm_StringLen (title);
  if (len > 0) {
    TESetText (title, len, h);
    TECalText (h);
  } else {
    selStart = 0;
    HLock ((Handle) h);
    hp = (TEPtr) *((Handle) h);
    selEnd = hp->teLength;
    HUnlock ((Handle) h);
    TESetSelect (selStart, selEnd, h);
    TEDelete (h);
  }
  Nlm_GetRect (t, &r);
  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    Nlm_InsetRect (&r, 1, 1);
    Nlm_InvalRect (&r);
  }
  Nlm_UpdateScrollBar ((Nlm_TexT) t);
#endif
#ifdef WIN_MSWIN
  SetWindowText (h, title);
#endif
#ifdef WIN_MOTIF
  allowTextCallback = FALSE;
  str = Nlm_StringSave (title);
  XmTextSetString (h, str);
  Nlm_MemFree (str);
  XmTextShowPosition (h, 0);
  allowTextCallback = TRUE;
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_SetPasswordText (Nlm_GraphiC t, Nlm_Int2 item,
                                 Nlm_CharPtr title, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
  Nlm_Int2      leng;
  Nlm_Char      stars [32];
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  TEPtr         hp;
  Nlm_Uint4     len;
  Nlm_RecT      r;
  Nlm_Int4      selStart;
  Nlm_Int4      selEnd;
#endif
#ifdef WIN_MOTIF
  Nlm_Char      bspace [5];
  Nlm_CharPtr   str;
#endif

  tempPort = Nlm_SavePortIfNeeded (t, savePort);
  h = Nlm_GetTextHandle ((Nlm_TexT) t);
  Nlm_SetPassword ((Nlm_TexT) t, title);
  Nlm_StringCpy (stars, "******************************");
  leng = (Nlm_Int2) Nlm_StringLen (title);
  if (leng < 32) {
    stars [leng] = '\0';
  }
#ifdef WIN_MAC
  len = Nlm_StringLen (stars);
  if (len > 0) {
    TESetText (stars, len, h);
    TECalText (h);
  } else {
    selStart = 0;
    HLock ((Handle) h);
    hp = (TEPtr) *((Handle) h);
    selEnd = hp->teLength;
    HUnlock ((Handle) h);
    TESetSelect (selStart, selEnd, h);
    TEDelete (h);
  }
  Nlm_GetRect (t, &r);
  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    Nlm_InsetRect (&r, 1, 1);
    Nlm_InvalRect (&r);
  }
  Nlm_UpdateScrollBar ((Nlm_TexT) t);
#endif
#ifdef WIN_MSWIN
  SetWindowText (h, title);
#endif
#ifdef WIN_MOTIF
  allowTextCallback = FALSE;
  Nlm_StringCpy (bspace, "\b");
  XmTextSetString (h, bspace);
  str = Nlm_StringSave (title);
  XmTextSetString (h, str);
  Nlm_MemFree (str);
  allowTextCallback = TRUE;
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_SetScrollText (Nlm_GraphiC t, Nlm_Int2 item,
                               Nlm_CharPtr title, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
  Nlm_WindoW    tempPort;
#ifdef WIN_MAC
  TEPtr         hp;
  Nlm_Uint4     len;
  Nlm_RecT      r;
  Nlm_Int4      selStart;
  Nlm_Int4      selEnd;
#endif

  tempPort = Nlm_SavePortIfNeeded (t, savePort);
  h = Nlm_GetTextHandle ((Nlm_TexT) t);
#ifdef WIN_MAC
  len = Nlm_StringLen (title);
  if (len > 0) {
    TESetText (title, len, h);
    TECalText (h);
  } else {
    selStart = 0;
    HLock ((Handle) h);
    hp = (TEPtr) *((Handle) h);
    selEnd = hp->teLength;
    HUnlock ((Handle) h);
    TESetSelect (selStart, selEnd, h);
    TEDelete (h);
  }
  Nlm_GetRect (t, &r);
  Nlm_InsetRect (&r, 4, 2);
  r.bottom = r.top + Nlm_GetVisLines ((Nlm_TexT) t) * Nlm_GetFontHeight ((Nlm_TexT) t);
  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    Nlm_InsetRect (&r, -1, -1);
    Nlm_InvalRect (&r);
  }
  Nlm_UpdateScrollBar ((Nlm_TexT) t);
#endif
#ifdef WIN_MSWIN
  SetWindowText (h, title);
#endif
#ifdef WIN_MOTIF
  allowTextCallback = FALSE;
  /*
     The gcc compiler puts static strings in read only memory.  Motif 1.1 dies
     at this point for text objects (Dialog, Hidden and Password) that use the
     modifyVerify callback.  In those cases, the strings are copied.
  */
  XmTextSetString (h, title);
  allowTextCallback = TRUE;
#endif
  Nlm_RestorePort (tempPort);
}

static void Nlm_GetDialogText (Nlm_GraphiC t, Nlm_Int2 item,
                               Nlm_CharPtr title, Nlm_sizeT maxsize)

{
  Nlm_TextTool  h;
#ifdef WIN_MAC
  Nlm_Char      **chars;
  TEPtr         hp;
  Nlm_Int2      i;
  Nlm_Int2      length;
  Nlm_Char      *ptr;
#endif
#ifdef WIN_MOTIF
  Nlm_CharPtr   ptr;
#endif

  h = Nlm_GetTextHandle ((Nlm_TexT) t);
#ifdef WIN_MAC
  i = 0;
  HLock ((Handle) h);
  hp = (TEPtr) *((Handle) h);
  chars = hp->hText;
  length = hp->teLength;
  if (length > maxsize - 1) {
    length = maxsize - 1;
  }
  HUnlock ((Handle) h);
  if (chars != NULL) {
    HLock ((Handle) chars);
    ptr = (Nlm_Char *) *((Handle) chars);
    Nlm_StringNCpy (title, ptr, length);
    title [length] = '\0';
    HUnlock ((Handle) chars);
  }
#endif
#ifdef WIN_MSWIN
    GetWindowText (h, title, maxsize);
    title [maxsize - 1] = '\0';
#endif
#ifdef WIN_MOTIF
  ptr = XmTextGetString (h);
  Nlm_StringNCpy (title, ptr, maxsize);
  XtFree (ptr);
#endif
}

static void Nlm_GetPasswordText (Nlm_GraphiC t, Nlm_Int2 item,
                                 Nlm_CharPtr title, Nlm_sizeT maxsize)

{
  Nlm_GetPassword ((Nlm_TexT) t, title, maxsize);
}

static void Nlm_GetScrollText (Nlm_GraphiC t, Nlm_Int2 item,
                               Nlm_CharPtr title, Nlm_sizeT maxsize)

{
  Nlm_TextTool  h;
#ifdef WIN_MAC
  Nlm_Char      **chars;
  TEPtr         hp;
  Nlm_Int2      i;
  Nlm_Int2      length;
  Nlm_Char      *ptr;
#endif
#ifdef WIN_MOTIF
  Nlm_CharPtr   ptr;
#endif

  h = Nlm_GetTextHandle ((Nlm_TexT) t);
#ifdef WIN_MAC
  i = 0;
  HLock ((Handle) h);
  hp = (TEPtr) *((Handle) h);
  chars = hp->hText;
  length = hp->teLength;
  if (length > maxsize - 1) {
    length = maxsize - 1;
  }
  HUnlock ((Handle) h);
  if (chars != NULL) {
    HLock ((Handle) chars);
    ptr = (Nlm_Char *) *((Handle) chars);
    Nlm_StringNCpy (title, ptr, length);
    title [length] = '\0';
    HUnlock ((Handle) chars);
  }
#endif
#ifdef WIN_MSWIN
    GetWindowText (h, title, maxsize);
    title [maxsize - 1] = '\0';
#endif
#ifdef WIN_MOTIF
  ptr = XmTextGetString (h);
  Nlm_StringNCpy (title, ptr, maxsize);
  XtFree (ptr);
#endif
}

extern Nlm_TexT Nlm_CurrentText (void)

{
  return currentText;
}

extern void Nlm_CutText (Nlm_TexT t)

{
  Nlm_TextTool  h;

  if (t != NULL) {
    h = Nlm_GetTextHandle (t);
#ifdef WIN_MAC
    TECut (h);
#endif
#ifdef WIN_MSWIN
    SendMessage (h, WM_CUT, 0, 0);
#endif
#ifdef WIN_MOTIF
    allowTextCallback = FALSE;
    XmTextCut (h, CurrentTime);
    allowTextCallback = TRUE;
#endif
    Nlm_DoAction ((Nlm_GraphiC) t);
  }
}

extern void Nlm_CopyText (Nlm_TexT t)

{
  Nlm_TextTool  h;

  if (t != NULL) {
    h = Nlm_GetTextHandle (t);
#ifdef WIN_MAC
    TECopy (h);
#endif
#ifdef WIN_MSWIN
    SendMessage (h, WM_COPY, 0, 0);
#endif
#ifdef WIN_MOTIF
    allowTextCallback = FALSE;
    XmTextCopy (h, CurrentTime);
    allowTextCallback = TRUE;
#endif
  }
}

extern void Nlm_PasteText (Nlm_TexT t)

{
  Nlm_TextTool  h;

  if (t != NULL) {
    h = Nlm_GetTextHandle (t);
#ifdef WIN_MAC
    TEPaste (h);
#endif
#ifdef WIN_MSWIN
    SendMessage (h, WM_PASTE, 0, 0);
#endif
#ifdef WIN_MOTIF
    allowTextCallback = FALSE;
    XmTextPaste (h);
    allowTextCallback = TRUE;
#endif
    Nlm_DoAction ((Nlm_GraphiC) t);
  }
}

extern void Nlm_ClearText (Nlm_TexT t)

{
  Nlm_TextTool  h;

  if (t != NULL) {
    h = Nlm_GetTextHandle (t);
#ifdef WIN_MAC
    TEDelete (h);
#endif
#ifdef WIN_MSWIN
    SendMessage (h, WM_CLEAR, 0, 0);
#endif
#ifdef WIN_MOTIF
    allowTextCallback = FALSE;
    XmTextClearSelection (h, CurrentTime);
    allowTextCallback = TRUE;
#endif
    Nlm_DoAction ((Nlm_GraphiC) t);
  }
}

#ifdef WIN_MSWIN
/* Message cracker functions */

static void MyCls_OnChar (HWND hwnd, UINT ch, int cRepeat)

{
  Nlm_TexT  t;

  t = (Nlm_TexT) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  handlechar = FALSE;
  if (ch == '\t' && Nlm_IsHiddenText ((Nlm_TexT) t)) {
    Nlm_DoTabCallback (t);
  } else if (ch == '\t') {
    Nlm_DoSendFocus ((Nlm_GraphiC) t, (Nlm_Char) ch);
  } else if (ch == '\n' || ch == '\r') {
    Nlm_DoSendFocus ((Nlm_GraphiC) t, (Nlm_Char) ch);
  } else {
    handlechar = TRUE;
  }
}

static void MyCls_OnSetFocus (HWND hwnd, HWND hwndOldFocus)

{
  Nlm_TexT  t;

  t = (Nlm_TexT) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  Nlm_SetActive (t, TRUE);
  Nlm_DoTextSelect (t);
}

static void MyCls_OnKillFocus (HWND hwnd, HWND hwndNewFocus)

{
  Nlm_TexT  t;

  t = (Nlm_TexT) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  if (Nlm_GetActive (t)) {
    Nlm_DoTextDeselect (t);
  }
  /*
  currentText = NULL;
  */
}

LRESULT CALLBACK EXPORT TextProc (HWND hwnd, UINT message,
                                  WPARAM wParam, LPARAM lParam)

{
  LRESULT   rsult;
  Nlm_TexT  t;
  HDC       tempHDC;
  HWND      tempHWnd;

  if (Nlm_VibrantDisabled ()) {
    return CallWindowProc (lpfnOldTextProc, hwnd, message, wParam, lParam);
  }

  rsult = 0;
  tempHWnd = Nlm_currentHWnd;
  tempHDC = Nlm_currentHDC;
  t = (Nlm_TexT) GetProp (hwnd, (LPSTR) "Nlm_VibrantProp");
  Nlm_theWindow = Nlm_GetParentWindow ((Nlm_GraphiC) t);
  Nlm_currentHWnd = GetParent (hwnd);
  Nlm_currentHDC = Nlm_ParentWindowPort ((Nlm_GraphiC) t);
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
        rsult = CallWindowProc (lpfnOldTextProc, hwnd, message, wParam, lParam);
      }
      break;
    case WM_SETFOCUS:
      HANDLE_WM_SETFOCUS (hwnd, wParam, lParam, MyCls_OnSetFocus);
      rsult = CallWindowProc (lpfnOldTextProc, hwnd, message, wParam, lParam);
      break;
    case WM_KILLFOCUS:
      HANDLE_WM_KILLFOCUS (hwnd, wParam, lParam, MyCls_OnKillFocus);
      rsult = CallWindowProc (lpfnOldTextProc, hwnd, message, wParam, lParam);
      break;
    default:
      rsult = CallWindowProc (lpfnOldTextProc, hwnd, message, wParam, lParam);
      break;
  }
  Nlm_currentHWnd = tempHWnd;
  Nlm_currentHDC = tempHDC;
  Nlm_currentWindowTool = tempHWnd;
  return rsult;
}
#endif

static Nlm_GraphiC Nlm_TextGainFocus (Nlm_GraphiC t, Nlm_Char ch, Nlm_Boolean savePort)

{
#ifdef WIN_MAC
  Nlm_TextTool  h;
  TEPtr         hp;
  Nlm_Int2      len;
  Nlm_GraphiC   rsult;

  rsult = NULL;
  if (ch == '\t' && Nlm_GetVisible (t) && Nlm_GetEnabled (t)) {
    h = Nlm_GetTextHandle ((Nlm_TexT) t);
    HLock ((Handle) h);
    hp = (TEPtr) *((Handle) h);
    len = hp->teLength;
    HUnlock ((Handle) h);
    Nlm_SetActive ((Nlm_TexT) t, TRUE);
    Nlm_SelectAText ((Nlm_TexT) t, 0, len);
    rsult = t;
  }
  return rsult;
#endif
#ifdef WIN_MSWIN
  Nlm_GraphiC   rsult;

  rsult = NULL;
  if (ch == '\t' && Nlm_GetVisible (t) && Nlm_GetEnabled (t)) {
    Nlm_SetActive ((Nlm_TexT) t, TRUE);
    Nlm_SelectAText ((Nlm_TexT) t, 0, 32767);
    rsult = t;
  }
  return rsult;
#endif
#ifdef WIN_MOTIF
  Nlm_GraphiC   rsult;

  rsult = NULL;
  if (ch == '\t' && Nlm_GetVisible (t) && Nlm_GetEnabled (t)) {
    Nlm_SetActive ((Nlm_TexT) t, TRUE);
    Nlm_SelectAText ((Nlm_TexT) t, 0, 32767);
    rsult = t;
  }
  return rsult;
#endif
}

static void Nlm_TextLoseFocus (Nlm_GraphiC t, Nlm_GraphiC excpt, Nlm_Boolean savePort)

{
  Nlm_WindoW  tempPort;

  if (t != excpt) {
    if (Nlm_GetActive ((Nlm_TexT) t)) {
      Nlm_DoTextDeselect ((Nlm_TexT) t);
    }
    Nlm_SetActive ((Nlm_TexT) t, FALSE);
    tempPort = Nlm_SavePortIfNeeded (t, savePort);
    Nlm_DeactivateText (t, FALSE);
    Nlm_RestorePort (tempPort);
  }
}

#ifdef WIN_MAC
static void Nlm_InvalText (Nlm_GraphiC t)

{
  Nlm_RecT  r;

  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    Nlm_GetRect (t, &r);
    Nlm_InsetRect (&r, -1, -1);
    Nlm_InvalRect (&r);
  }
}

static void Nlm_InvalScrollText (Nlm_GraphiC t)

{
  Nlm_RecT  r;

  if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    Nlm_GetRect (t, &r);
    r.right += Nlm_vScrollBarWidth;
    if (! Nlm_GetTextWrap ((Nlm_TexT) t)) {
      r.bottom += Nlm_hScrollBarHeight;
    }
    Nlm_InsetRect (&r, -1, -1);
    Nlm_InvalRect (&r);
  }
}
#endif

static void Nlm_SetTextPosition (Nlm_GraphiC t, Nlm_RectPtr r, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
  Nlm_RecT      oldRect;
  Nlm_WindoW    tempPort;
  Nlm_RecT      tr;
#ifdef WIN_MAC
  TEPtr         hp;
  Nlm_RectTool  rtool;
#endif

  if (r != NULL) {
    Nlm_DoGetPosition (t, &oldRect);
    if (! Nlm_EqualRect (r, &oldRect)) {
      tempPort = Nlm_SavePortIfNeeded (t, savePort);
      h = Nlm_GetTextHandle ((Nlm_TexT) t);
#ifdef WIN_MAC
      Nlm_InvalText (t);
#endif
      tr = *r;
#ifdef WIN_MAC
      Nlm_InsetRect (&tr, 2, 2);
      Nlm_RecTToRectTool (&tr, &rtool);
      HLock ((Handle) h);
      hp = (TEPtr) *((Handle) h);
      hp->destRect = rtool;
      hp->viewRect = rtool;
      HUnlock ((Handle) h);
      Nlm_SetRect (t, r);
      Nlm_InvalText (t);
#endif
#ifdef WIN_MSWIN
      MoveWindow (h, tr.left, tr.top, tr.right - tr.left, tr.bottom - tr.top, TRUE);
      Nlm_SetRect (t, r);
      UpdateWindow (h);
#endif
#ifdef WIN_MOTIF
      allowTextCallback = FALSE;
      XtVaSetValues (h,
                     XmNx, (Position) tr.left + 3,
                     XmNy, (Position) tr.top + 3,
                     XmNwidth, (Dimension) (tr.right - tr.left - 6),
                     XmNheight, (Dimension) (tr.bottom - tr.top - 6), 
                     NULL);
      Nlm_SetRect (t, r);
      allowTextCallback = TRUE;
#endif
      Nlm_RestorePort (tempPort);
    }
  }
}

static void Nlm_SetHiddenTextPosition (Nlm_GraphiC t, Nlm_RectPtr r, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
  Nlm_RecT      oldRect;
  Nlm_WindoW    tempPort;
  Nlm_RecT      tr;
#ifdef WIN_MAC
  TEPtr         hp;
  Nlm_RectTool  rtool;
#endif

  if (r != NULL) {
    Nlm_DoGetPosition (t, &oldRect);
    if (! Nlm_EqualRect (r, &oldRect)) {
      tempPort = Nlm_SavePortIfNeeded (t, savePort);
      h = Nlm_GetTextHandle ((Nlm_TexT) t);
#ifdef WIN_MAC
      Nlm_InvalText (t);
#endif
      tr = *r;
#ifdef WIN_MAC
      Nlm_RecTToRectTool (&tr, &rtool);
      HLock ((Handle) h);
      hp = (TEPtr) *((Handle) h);
      hp->destRect = rtool;
      hp->viewRect = rtool;
      HUnlock ((Handle) h);
      Nlm_SetRect (t, r);
      Nlm_InvalText (t);
#endif
#ifdef WIN_MSWIN
      MoveWindow (h, tr.left, tr.top, tr.right - tr.left, tr.bottom - tr.top, TRUE);
      Nlm_SetRect (t, r);
      UpdateWindow (h);
#endif
#ifdef WIN_MOTIF
      allowTextCallback = FALSE;
      XtVaSetValues (h,
                     XmNx, (Position) tr.left,
                     XmNy, (Position) tr.top,
                     XmNwidth, (Dimension) (tr.right - tr.left),
                     XmNheight, (Dimension) (tr.bottom - tr.top), 
                     NULL);
      Nlm_SetRect (t, r);
      allowTextCallback = TRUE;
#endif
      Nlm_RestorePort (tempPort);
    }
  }
}

static void Nlm_SetScrollTextPosition (Nlm_GraphiC t, Nlm_RectPtr r, Nlm_Boolean savePort)

{
  Nlm_TextTool  h;
  Nlm_RecT      oldRect;
  Nlm_WindoW    tempPort;
  Nlm_RecT      tr;
  Nlm_Boolean   wrap;
#ifdef WIN_MAC
  Nlm_RectTool  dtool;
  TEPtr         hp;
  Nlm_RectTool  rtool;
  Nlm_BaR       sb;
#endif

  if (r != NULL) {
    Nlm_DoGetPosition (t,&oldRect);
    if (! Nlm_EqualRect (r, &oldRect)) {
      tempPort = Nlm_SavePortIfNeeded (t, savePort);
      h = Nlm_GetTextHandle ((Nlm_TexT) t);
      if (h != NULL) {
        wrap = Nlm_GetTextWrap ((Nlm_TexT) t);
        tr = *r;
#ifdef WIN_MAC
        Nlm_InvalScrollText (t);
        tr.right -= Nlm_vScrollBarWidth;
        if (! wrap) {
          tr.bottom -= Nlm_hScrollBarHeight;
        }
        Nlm_SetRect (t, &tr);
        Nlm_InsetRect (&tr, 4, 2);
        tr.bottom = tr.top + Nlm_GetVisLines ((Nlm_TexT) t) * Nlm_GetFontHeight ((Nlm_TexT) t);
        Nlm_RecTToRectTool (&tr, &rtool);
        Nlm_RecTToRectTool (&tr, &dtool);
        if (! wrap) {
#ifdef DCLAP
          dtool.right = 32767;
#else
          dtool.right += HSCROLL_POSITIONS * Nlm_stdCharWidth;
#endif
        }
        HLock ((Handle) h);
        hp = (TEPtr) *((Handle) h);
        hp->destRect = dtool;
        hp->viewRect = rtool;
        HUnlock ((Handle) h);
        Nlm_InvalScrollText (t);
        sb = Nlm_GetTextVScrollBar ((Nlm_TexT) t);
        if (sb != NULL) {
          Nlm_GetRect (t, &tr);
          tr.left = tr.right;
          tr.right += Nlm_vScrollBarWidth;
          Nlm_DoSetPosition ((Nlm_GraphiC) sb, &tr, FALSE);
        }
        sb = Nlm_GetTextHScrollBar ((Nlm_TexT) t);
        if (sb != NULL) {
          Nlm_GetRect (t, &tr);
          tr.top = tr.bottom;
          tr.bottom += Nlm_hScrollBarHeight;
          Nlm_DoSetPosition ((Nlm_GraphiC) sb, &tr, FALSE);
        }
#endif
#ifdef WIN_MSWIN
        MoveWindow (h, tr.left, tr.top, tr.right - tr.left, tr.bottom - tr.top, TRUE);
        tr.right -= Nlm_vScrollBarWidth;
        if (! wrap) {
          tr.bottom -= Nlm_hScrollBarHeight;
        }
        Nlm_SetRect (t, &tr);
        UpdateWindow (h);
#endif
#ifdef WIN_MOTIF
        allowTextCallback = FALSE;
        XtVaSetValues (XtParent (h),
                       XmNx, (Position) tr.left + 3,
                       XmNy, (Position) tr.top + 3,
                       XmNwidth, (Dimension) (tr.right - tr.left) - 6,
                       XmNheight, (Dimension) (tr.bottom - tr.top) - 6,
                       NULL);
        tr.right -= Nlm_vScrollBarWidth;
        if (! wrap) {
          tr.bottom -= Nlm_hScrollBarHeight;
        }
        Nlm_SetRect (t, &tr);
        allowTextCallback = TRUE;
#endif
      }
      Nlm_RestorePort (tempPort);
    }
  }
}

static void Nlm_GetTextPosition (Nlm_GraphiC t, Nlm_RectPtr r)

{
  if (r != NULL) {
    Nlm_GetRect (t, r);
  }
}

static void Nlm_GetScrollTextPosition (Nlm_GraphiC t, Nlm_RectPtr r)

{
  if (r != NULL) {
    Nlm_GetRect (t, r);
    r->right += Nlm_vScrollBarWidth;
    if (! Nlm_GetTextWrap ((Nlm_TexT) t)) {
      r->bottom += Nlm_hScrollBarHeight;
    }
  }
}

#ifdef WIN_MAC
static void Nlm_VScrollAction (Nlm_BaR sb, Nlm_GraphiC t,
                               Nlm_Int2 newval, Nlm_Int2 oldval)

{
  Nlm_Int2      delta;
  Nlm_TextTool  h;
  Nlm_Int2      height;
  Nlm_RecT      r;

  h = Nlm_GetTextHandle ((Nlm_TexT) t);
  delta = oldval - newval;
  if (oldval != newval) {
		SelectDialogFont();
  	height = Nlm_GetLineHeight ((Nlm_TexT) t);
    TEScroll (0, delta * height, h);
  } else if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    Nlm_GetRect (t, &r);
    Nlm_InsetRect (&r, 2, 1);
    Nlm_InvalRect (&r);
  }
  Nlm_Update ();
}

static void Nlm_HScrollAction (Nlm_BaR sb, Nlm_GraphiC t,
                               Nlm_Int2 newval, Nlm_Int2 oldval)
{
  Nlm_Int2      delta;
  Nlm_TextTool  h;
  Nlm_Int2      width;
  Nlm_RecT      r;

  h = Nlm_GetTextHandle ((Nlm_TexT) t);
  delta = oldval - newval;
  if (oldval != newval) {
		SelectDialogFont();
#ifdef DCLAP
		width= Nlm_CharWidth('G');
#else
    width = Nlm_stdCharWidth;
#endif
    TEScroll (delta * width, 0, h);
  } else if (Nlm_GetVisible (t) && Nlm_GetAllParentsVisible (t)) {
    Nlm_GetRect (t, &r);
    Nlm_InsetRect (&r, 2, 1);
    Nlm_InvalRect (&r);
  }
  Nlm_Update ();
}
#endif


#ifdef DCLAP
extern void Nlm_HScrollText(Nlm_BaR sb, Nlm_GraphiC t, Nlm_Int2 newval, Nlm_Int2 oldval)
{
#ifdef WIN_MAC
	Nlm_HScrollAction( sb, t, newval, oldval);
#endif

#ifdef WIN_MSWIN
  Nlm_TextTool  h;
  if (t && oldval != newval) {
    h = Nlm_GetTextHandle ((Nlm_TexT) t);
		SendMessage( h, WM_HSCROLL, SB_THUMBTRACK, newval);
		}
#endif

#ifdef WIN_MOTIF
  Nlm_TextTool  h;
  if (t && oldval != newval) {
    h = Nlm_GetTextHandle ((Nlm_TexT) t);
		if (newval < 0) newval= 0; 
    XmTextShowPosition( h, newval); 
		}
#endif
}

extern void Nlm_SetTextColumns( Nlm_GraphiC t, Nlm_Int2 ncolumns)
{
#ifdef WIN_MOTIF
  if (t) {
		Nlm_TextTool h;
		short n;
	  Arg   wargs [20];
	  n= 0;
	  XtSetArg( wargs[n], XmNcolumns, ncolumns); n++;
		h = Nlm_GetTextHandle(( Nlm_TexT) t);
	  XtSetValues( h, wargs, n);
	  }
#endif
}

#endif

#ifdef WIN_MOTIF
static void Nlm_ReturnCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  XmAnyCallbackStruct  *cbs;
  Nlm_GraphiC          t;

  cbs = (XmAnyCallbackStruct *) call_data;
  t = (Nlm_GraphiC) client_data;
  if (cbs->event != NULL) {
    Nlm_DoSendFocus ((Nlm_GraphiC) t, '\r');
  }
}

static void Nlm_TextCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  XmAnyCallbackStruct  *cbs;
  Nlm_GraphiC          t;

  cbs = (XmAnyCallbackStruct *) call_data;
  t = (Nlm_GraphiC) client_data;
  if (cbs->event != NULL) {
    Nlm_DoAction (t);
  } else if (allowTextCallback) {
    Nlm_DoAction (t);
  }
}

static void Nlm_FocusCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  XmAnyCallbackStruct  *cbs;
  Nlm_TexT             t;

  cbs = (XmAnyCallbackStruct *) call_data;
  t = (Nlm_TexT) client_data;
  Nlm_SetActive (t, TRUE);
  Nlm_DoTextSelect (t);
}

static void Nlm_LoseFocusCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  XmAnyCallbackStruct  *cbs;
  Nlm_TexT             t;

  cbs = (XmAnyCallbackStruct *) call_data;
  t = (Nlm_TexT) client_data;
  if (Nlm_GetActive (t)) {
    Nlm_DoTextDeselect (t);
  }
  /*
  currentText = NULL;
  */
}

static void Nlm_RefreshCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  XmTextVerifyCallbackStruct  *cbs;
  Nlm_RecT                    r;
  Nlm_TexT                    t;

  cbs = (XmTextVerifyCallbackStruct *) call_data;
  t = (Nlm_TexT) client_data;
  if (cbs->text->ptr == NULL) {
    Nlm_ObjectRect ((Nlm_GraphiC) t, &r);
    Nlm_OffsetRect (&r, -r.left, -r.top);
    if (! Nlm_IsHiddenText (t)) {
      Nlm_InsetRect (&r, 4, 4);
#ifdef OS_VMS
      Nlm_InsetRect (&r, 5, 3);
#else
      Nlm_InsetRect (&r, 5, 5);
#endif
    }
    if (Nlm_currentXDisplay != NULL) {
      XClearArea (Nlm_currentXDisplay, XtWindow (w), r.left, r.top,
                  r.right - r.left, r.bottom - r.top, TRUE);
    }
  }
}

static void Nlm_PasswordCallback (Widget w, XtPointer client_data, XtPointer call_data)

{
  XmTextVerifyCallbackStruct  *cbs;
  int                         len;
  Nlm_Char                    password [32];
  Nlm_GraphiC                 t;

  cbs = (XmTextVerifyCallbackStruct *) call_data;
  t = (Nlm_GraphiC) client_data;
  if (cbs->text->length > 1) {
    cbs->doit = FALSE;
    return;
  }
  if (cbs->text->ptr == NULL) {
    Nlm_GetPassword ((Nlm_TexT) t, password, sizeof (password) - 1);
    cbs->endPos = (long) Nlm_StringLen (password);
    cbs->startPos = 0;
    password [cbs->startPos] = '\0';
    Nlm_SetPassword ((Nlm_TexT) t, password);
    return;
  }
  Nlm_GetPassword ((Nlm_TexT) t, password, sizeof (password) - 1);
  Nlm_StringNCat (password, cbs->text->ptr, cbs->text->length);
  password [cbs->endPos + cbs->text->length] = '\0';
  Nlm_SetPassword ((Nlm_TexT) t, password);
  for (len = 0; len < cbs->text->length; len++) {
    cbs->text->ptr [len] = '*';
  }
}

static void Nlm_TabCallback (Widget w, XEvent *ev, String *args, Cardinal *num_args)

{
  XKeyEvent  *event;
  XtPointer     ptr;
  Nlm_TexT   t;

  event = (XKeyEvent *) ev;
  XtVaGetValues (w, XmNuserData, &ptr, NULL);
  t = (Nlm_TexT) ptr;
  Nlm_DoTabCallback (t);
}
#endif

static void Nlm_NewDialogText (Nlm_TexT t, Nlm_CharPtr dfault, Nlm_TxtActnProc actn)

{
  Nlm_TextTool    h;
  Nlm_Int2        leng;
  Nlm_Char        local [128];
  Nlm_RecT        r;
  Nlm_Char        stars [32];
  Nlm_WindowTool  wptr;
#ifdef WIN_MAC
  Nlm_RectTool    rtool;
#endif
#ifdef WIN_MSWIN
  Nlm_Uint4       style;
#endif
#ifdef WIN_MOTIF
  Cardinal        n;
  Arg             wargs [20];
  XmFontList      fontlist;
#endif

  Nlm_StringCpy (stars, "******************************");
  local [0] = '\0';
  Nlm_StringNCpy (local, dfault, sizeof (local) - 1);
  leng = (Nlm_Int2) Nlm_StringLen (local);
  if (leng < 32) {
    stars [leng] = '\0';
  }
  Nlm_GetRect ((Nlm_GraphiC) t, &r);
  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) t);
#ifdef WIN_MAC
  Nlm_InsetRect (&r, 2, 2);
  Nlm_RecTToRectTool (&r, &rtool);
  h = TENew (&rtool, &rtool);
  Nlm_LoadTextData (t, h, NULL, NULL, FALSE, gDialogTextFont, gDlogTextHeight, 
  		FALSE, FALSE, FALSE, 1, NULL, NULL, NULL);
  Nlm_SetDialogText ((Nlm_GraphiC) t, 0, local, FALSE);
  if (h != NULL) {
    TEAutoView (TRUE, h);
  }
#endif
#ifdef WIN_MSWIN
  style = WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_LEFT;
  h = CreateWindow ("Edit", local, style,
                    r.left, r.top, r.right - r.left,
                    r.bottom - r.top, wptr, 0,
                    Nlm_currentHInst, NULL);
  if (h != NULL) {
    SetProp (h, (LPSTR) "Nlm_VibrantProp", (Nlm_HandleTool) t);
#ifdef DCLAP
#if 1
{
	/* this is vibrant way */
  HFONT    oldFont;
  Nlm_FntPtr      fntptr;
  oldFont = NULL;
  if (gDialogTextFont != NULL) {
    fntptr = (Nlm_FntPtr) Nlm_HandLock (gDialogTextFont);
    if (fntptr != NULL && fntptr->handle != NULL) {
      oldFont = SelectObject (Nlm_currentHDC, fntptr->handle);
    }
    Nlm_HandUnlock (gDialogTextFont);
  } else {
    oldFont = SelectObject (Nlm_currentHDC, GetStockObject (ANSI_FIXED_FONT));
  }
  if (oldFont != NULL) {
    SelectObject (Nlm_currentHDC, oldFont);
    SetWindowFont (h, oldFont, FALSE);
  }
 }
#else
		{
		/* dgg way */
		Nlm_FontData	fdata;
		Nlm_GetFontData( gDialogTextFont, &fdata);
		PostMessage( h, WM_SETFONT, fdata.handle, 0L);
		}
#endif
#endif
  }
  Nlm_LoadTextData (t, h, NULL, NULL, FALSE, gDialogTextFont, gDlogTextHeight,
  		 FALSE, FALSE, FALSE, 1, NULL, NULL, NULL);
  if (lpfnNewTextProc == NULL) {
    lpfnNewTextProc = (WNDPROC) MakeProcInstance ((FARPROC) TextProc, Nlm_currentHInst);
  }
  if (lpfnOldTextProc == NULL) {
    lpfnOldTextProc = (WNDPROC) GetWindowLong (h, GWL_WNDPROC);
  } else if (lpfnOldTextProc != (WNDPROC) GetWindowLong (h, GWL_WNDPROC)) {
    Nlm_Message (MSG_ERROR, "TextProc subclass error");
  }
  SetWindowLong (h, GWL_WNDPROC, (LONG) lpfnNewTextProc);
#endif
#ifdef WIN_MOTIF
  allowTextCallback = FALSE;
  Nlm_InsetRect (&r, 3, 3);
  fontlist = NULL;
#ifdef DCLAP
  if (gDialogTextFont != NULL) {
  	Nlm_FntPtr fntptr;
    fntptr = (Nlm_FntPtr) Nlm_HandLock (gDialogTextFont);
    if (fntptr != NULL && fntptr->handle != NULL) {
      fontlist = XmFontListCreate (fntptr->handle, XmSTRING_DEFAULT_CHARSET);
    }
    Nlm_HandUnlock (font);
  }
#endif
  n = 0;
  XtSetArg (wargs [n], XmNx, (Position) r.left); n++;
  XtSetArg (wargs [n], XmNy, (Position) r.top); n++;
  XtSetArg (wargs [n], XmNwidth, (Dimension) (r.right - r.left)); n++;
  XtSetArg (wargs [n], XmNheight, (Dimension) (r.bottom - r.top)); n++;
#ifdef OS_VMS
  XtSetArg (wargs [n], XmNmarginHeight, 1); n++;
#else
  XtSetArg (wargs [n], XmNmarginHeight, 3); n++;
#endif
  XtSetArg (wargs [n], XmNmarginWidth, 3); n++;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
#ifdef DCLAP
  XtSetArg (wargs [n], XmNfontList, fontlist); n++;
#else
  XtSetArg (wargs [n], XmNfontList, NULL); n++;
#endif
  XtSetArg (wargs [n], XmNhighlightThickness, 0); n++;
  XtSetArg (wargs [n], XmNautoShowCursorPosition, TRUE); n++;
  h = XmCreateText (wptr, (String) "", wargs, n);
  Nlm_LoadTextData (t, h, NULL, NULL, FALSE, gDialogTextFont, gDlogTextHeight, 
  		FALSE, FALSE, FALSE, 1, NULL, NULL, NULL);
  XmTextSetString (h, local);
  XmTextShowPosition (h, 0);
  XtAddCallback (h, XmNmodifyVerifyCallback, Nlm_RefreshCallback, (XtPointer) t);
  XtAddCallback (h, XmNvalueChangedCallback, Nlm_TextCallback, (XtPointer) t);
  XtAddCallback (h, XmNactivateCallback, Nlm_ReturnCallback, (XtPointer) t);
  XtAddCallback (h, XmNfocusCallback, Nlm_FocusCallback, (XtPointer) t);
  XtAddCallback (h, XmNlosingFocusCallback, Nlm_LoseFocusCallback, (XtPointer) t);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (t))) {
      XtRealizeWidget (h);
    }
  } else {
    XtRealizeWidget (h);
  }
  allowTextCallback = TRUE;
#ifdef DCLAP
  if (fontlist != NULL) {
    XmFontListFree (fontlist);
  }
#endif
#endif
  Nlm_LoadAction ((Nlm_GraphiC) t, (Nlm_ActnProc) actn);
}

static void Nlm_NewPasswordText (Nlm_TexT t, Nlm_CharPtr dfault, Nlm_TxtActnProc actn)

{
  Nlm_TextTool    h;
  Nlm_Int2        leng;
  Nlm_Char        local [128];
  Nlm_RecT        r;
  Nlm_Char        stars [32];
  Nlm_WindowTool  wptr;
#ifdef WIN_MAC
  Nlm_RectTool    rtool;
#endif
#ifdef WIN_MSWIN
  Nlm_Uint4       style;
#endif
#ifdef WIN_MOTIF
  Cardinal        n;
  Arg             wargs [20];
  XmFontList      fontlist;
#endif

  Nlm_StringCpy (stars, "******************************");
  local [0] = '\0';
  Nlm_StringNCpy (local, dfault, sizeof (local) - 1);
  leng = (Nlm_Int2) Nlm_StringLen (local);
  if (leng < 32) {
    stars [leng] = '\0';
  }
  Nlm_GetRect ((Nlm_GraphiC) t, &r);
  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) t);
#ifdef WIN_MAC
  Nlm_InsetRect (&r, 2, 2);
  Nlm_RecTToRectTool (&r, &rtool);
  h = TENew (&rtool, &rtool);
  Nlm_LoadTextData (t, h, NULL, NULL, FALSE, gDialogTextFont, gDlogTextHeight, 
  		FALSE, FALSE, FALSE, 1, NULL, NULL, NULL);
  Nlm_SetPasswordText ((Nlm_GraphiC) t, 0, local, FALSE);
#endif
#ifdef WIN_MSWIN
  style = WS_CHILD | WS_BORDER |  ES_AUTOVSCROLL | ES_LEFT | ES_PASSWORD;
  h = CreateWindow ("Edit", local, style,
                    r.left, r.top, r.right - r.left,
                    r.bottom - r.top, wptr, 0,
                    Nlm_currentHInst, NULL);
  if (h != NULL) {
    SetProp (h, (LPSTR) "Nlm_VibrantProp", (Nlm_HandleTool) t);
  }
  Nlm_LoadTextData (t, h, NULL, NULL, FALSE, gDialogTextFont, gDlogTextHeight, 
  		FALSE, FALSE, FALSE, 1, NULL, NULL, NULL);
  if (lpfnNewTextProc == NULL) {
    lpfnNewTextProc = (WNDPROC) MakeProcInstance ((FARPROC) TextProc, Nlm_currentHInst);
  }
  if (lpfnOldTextProc == NULL) {
    lpfnOldTextProc = (WNDPROC) GetWindowLong (h, GWL_WNDPROC);
  } else if (lpfnOldTextProc != (WNDPROC) GetWindowLong (h, GWL_WNDPROC)) {
    Nlm_Message (MSG_ERROR, "TextProc subclass error");
  }
  SetWindowLong (h, GWL_WNDPROC, (LONG) lpfnNewTextProc);
  Nlm_SetPassword (t, local);
#endif
#ifdef WIN_MOTIF
  allowTextCallback = FALSE;
  Nlm_InsetRect (&r, 3, 3);
  fontlist = NULL;
#ifdef DCLAP
  if (gDialogTextFont != NULL) {
  	Nlm_FntPtr fntptr;
    fntptr = (Nlm_FntPtr) Nlm_HandLock (gDialogTextFont);
    if (fntptr != NULL && fntptr->handle != NULL) {
      fontlist = XmFontListCreate (fntptr->handle, XmSTRING_DEFAULT_CHARSET);
    }
    Nlm_HandUnlock (font);
  }
#endif
  n = 0;
  XtSetArg (wargs [n], XmNx, (Position) r.left); n++;
  XtSetArg (wargs [n], XmNy, (Position) r.top); n++;
  XtSetArg (wargs [n], XmNwidth, (Dimension) (r.right - r.left)); n++;
  XtSetArg (wargs [n], XmNheight, (Dimension) (r.bottom - r.top)); n++;
  XtSetArg (wargs [n], XmNmarginHeight, 3); n++;
  XtSetArg (wargs [n], XmNmarginWidth, 3); n++;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNeditMode, XmSINGLE_LINE_EDIT); n++;
#ifdef DCLAP
  XtSetArg (wargs [n], XmNfontList, fontlist); n++;
#else
  XtSetArg (wargs [n], XmNfontList, NULL); n++;
#endif
  XtSetArg (wargs [n], XmNhighlightThickness, 0); n++;
  h = XmCreateText (wptr, (String) "", wargs, n);
  Nlm_LoadTextData (t, h, NULL, NULL, FALSE, gDialogTextFont, gDlogTextHeight, 
  	FALSE, FALSE, FALSE, 1, NULL, NULL, NULL);
  XmTextSetString (h, local);
  XtAddCallback (h, XmNmodifyVerifyCallback, Nlm_PasswordCallback, (XtPointer) t);
  XtAddCallback (h, XmNvalueChangedCallback, Nlm_TextCallback, (XtPointer) t);
  XtAddCallback (h, XmNactivateCallback, Nlm_ReturnCallback, (XtPointer) t);
  XtAddCallback (h, XmNfocusCallback, Nlm_FocusCallback, (XtPointer) t);
  XtAddCallback (h, XmNlosingFocusCallback, Nlm_LoseFocusCallback, (XtPointer) t);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (t))) {
      XtRealizeWidget (h);
    }
  } else {
    XtRealizeWidget (h);
  }
  Nlm_SetPassword (t, dfault);
  allowTextCallback = TRUE;
#ifdef DCLAP
  if (fontlist != NULL) {
    XmFontListFree (fontlist);
  }
#endif
#endif
  Nlm_LoadAction ((Nlm_GraphiC) t, (Nlm_ActnProc) actn);
}

static void Nlm_NewHiddenText (Nlm_TexT t, Nlm_CharPtr dfault,
                               Nlm_TxtActnProc actn, Nlm_TxtActnProc tabProc)

{
  Nlm_TextTool    h;
  Nlm_Int2        leng;
  Nlm_Char        local [128];
  Nlm_RecT        r;
  Nlm_Char        stars [32];
  Nlm_WindowTool  wptr;
#ifdef WIN_MAC
  Nlm_RectTool    rtool;
  Nlm_RectTool    dtool;
#endif
#ifdef WIN_MSWIN
  Nlm_Uint4       style;
#endif
#ifdef WIN_MOTIF
  Cardinal        n;
  Arg             wargs [20];
  XmFontList      fontlist;
#endif

  Nlm_StringCpy (stars, "******************************");
  local [0] = '\0';
  Nlm_StringNCpy (local, dfault, sizeof (local) - 1);
  leng = (Nlm_Int2) Nlm_StringLen (local);
  if (leng < 32) {
    stars [leng] = '\0';
  }
  Nlm_GetRect ((Nlm_GraphiC) t, &r);
  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) t);
#ifdef WIN_MAC
  Nlm_RecTToRectTool (&r, &rtool);  
  h = TENew (&rtool, &rtool);
  Nlm_LoadTextData (t, h, NULL, NULL, FALSE, gDialogTextFont, gDlogTextHeight, 
  		FALSE, FALSE, TRUE, 1, NULL, NULL, tabProc);
  Nlm_SetDialogText ((Nlm_GraphiC) t, 0, local, FALSE);
  if (h != NULL) {
    TEAutoView (TRUE, h);
  }
#endif
#ifdef WIN_MSWIN
  style = WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | ES_LEFT;
  h = CreateWindow ("Edit", local, style,
                    r.left, r.top, r.right - r.left,
                    r.bottom - r.top, wptr, 0,
                    Nlm_currentHInst, NULL);
  if (h != NULL) {
    SetProp (h, (LPSTR) "Nlm_VibrantProp", (Nlm_HandleTool) t);
#ifdef DCLAP
#if 1
{
	/* vibrant way */
  HFONT    oldFont;
  Nlm_FntPtr      fntptr;
  oldFont = NULL;
  if (gDialogTextFont != NULL) {
    fntptr = (Nlm_FntPtr) Nlm_HandLock (gDialogTextFont);
    if (fntptr != NULL && fntptr->handle != NULL) {
      oldFont = SelectObject (Nlm_currentHDC, fntptr->handle);
    }
    Nlm_HandUnlock (gDialogTextFont);
  } else {
    oldFont = SelectObject (Nlm_currentHDC, GetStockObject (ANSI_FIXED_FONT));
  }
  if (oldFont != NULL) {
    SelectObject (Nlm_currentHDC, oldFont);
    SetWindowFont (h, oldFont, FALSE);
  }
 }
#else
	/* dgg way */
		{
		Nlm_FontData	fdata;
		Nlm_GetFontData( gDialogTextFont, &fdata);
		PostMessage( h, WM_SETFONT, fdata.handle, 0L);
		}
#endif
#endif
  }
  Nlm_LoadTextData (t, h, NULL, NULL, FALSE, gDialogTextFont, gDlogTextHeight, 
  		FALSE, FALSE, TRUE, 1, NULL, NULL, tabProc);
  if (lpfnNewTextProc == NULL) {
    lpfnNewTextProc = (WNDPROC) MakeProcInstance ((FARPROC) TextProc, Nlm_currentHInst);
  }
  if (lpfnOldTextProc == NULL) {
    lpfnOldTextProc = (WNDPROC) GetWindowLong (h, GWL_WNDPROC);
  } else if (lpfnOldTextProc != (WNDPROC) GetWindowLong (h, GWL_WNDPROC)) {
    Nlm_Message (MSG_ERROR, "TextProc subclass error");
  }
  SetWindowLong (h, GWL_WNDPROC, (LONG) lpfnNewTextProc);
#endif

#ifdef WIN_MOTIF
  allowTextCallback = FALSE;
  fontlist = NULL;
#ifdef DCLAP
  if (gDialogTextFont != NULL) {
  	Nlm_FntPtr fntptr;
    fntptr = (Nlm_FntPtr) Nlm_HandLock (gDialogTextFont);
    if (fntptr != NULL && fntptr->handle != NULL) {
      fontlist = XmFontListCreate (fntptr->handle, XmSTRING_DEFAULT_CHARSET);
    }
    Nlm_HandUnlock (font);
  }
#endif
  n = 0;
  XtSetArg (wargs [n], XmNx, (Position) r.left); n++;
  XtSetArg (wargs [n], XmNy, (Position) r.top); n++;
  XtSetArg (wargs [n], XmNwidth, (Dimension) (r.right - r.left)); n++;
  XtSetArg (wargs [n], XmNheight, (Dimension) (r.bottom - r.top)); n++;
#ifdef OS_VMS
  XtSetArg (wargs [n], XmNmarginHeight, 0); n++;
#else
  XtSetArg (wargs [n], XmNmarginHeight, 0); n++;
#endif
  XtSetArg (wargs [n], XmNmarginWidth, 0); n++;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNshadowThickness, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
#ifdef DCLAP
  XtSetArg (wargs [n], XmNfontList, fontlist); n++;
#else
  XtSetArg (wargs [n], XmNfontList, NULL); n++;
#endif
  XtSetArg (wargs [n], XmNhighlightThickness, 0); n++;
  XtSetArg (wargs [n], XmNautoShowCursorPosition, TRUE); n++;
  XtSetArg (wargs [n], XmNverifyBell, FALSE); n++;
  h = XmCreateText (wptr, (String) "", wargs, n);
  XtOverrideTranslations (h, XtParseTranslationTable ("<Key>Tab: do_tab()"));
  XtVaSetValues (h, XmNuserData, (XtPointer) t, NULL);
  Nlm_LoadTextData (t, h, NULL, NULL, FALSE, gDialogTextFont, gDlogTextHeight, 
  		FALSE, FALSE, TRUE, 1, NULL, NULL, tabProc);
  XmTextSetString (h, local);
  XmTextShowPosition (h, 0);
  XtAddCallback (h, XmNmodifyVerifyCallback, Nlm_RefreshCallback, (XtPointer) t);
  XtAddCallback (h, XmNvalueChangedCallback, Nlm_TextCallback, (XtPointer) t);
  XtAddCallback (h, XmNactivateCallback, Nlm_ReturnCallback, (XtPointer) t);
  XtAddCallback (h, XmNfocusCallback, Nlm_FocusCallback, (XtPointer) t);
  XtAddCallback (h, XmNlosingFocusCallback, Nlm_LoseFocusCallback, (XtPointer) t);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (t))) {
      XtRealizeWidget (h);
    }
  } else {
    XtRealizeWidget (h);
  }
#ifdef DCLAP
  if (fontlist != NULL) {
    XmFontListFree (fontlist);
  }
#endif
  allowTextCallback = TRUE;
#endif
  Nlm_LoadAction ((Nlm_GraphiC) t, (Nlm_ActnProc) actn);
}


#ifdef DCLAP

static void Nlm_NewTextLine (Nlm_TexT t, Nlm_CharPtr dfault, Nlm_Int2 charWidth, 
                               Nlm_TxtActnProc actn, Nlm_TxtActnProc tabProc)

{
  Nlm_TextTool    h;
  Nlm_Int2        leng;
  Nlm_Char        local [128];
  Nlm_RecT        r;
  Nlm_WindowTool  wptr;
#ifdef WIN_MAC
  Nlm_RectTool    rtool;
  Nlm_RectTool    dtool;
#endif
#ifdef WIN_MSWIN
  Nlm_Uint4       style;
#endif
#ifdef WIN_MOTIF
  Cardinal        n;
  Arg             wargs [20];
  XmFontList      fontlist;
#endif

  local [0] = '\0';
  Nlm_StringNCpy (local, dfault, sizeof (local) - 1);
  leng = (Nlm_Int2) Nlm_StringLen (local);
  Nlm_GetRect ((Nlm_GraphiC) t, &r);
  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) t);

#ifdef WIN_MAC
  Nlm_RecTToRectTool (&r, &rtool);
	Nlm_RecTToRectTool (&r, &dtool);
	dtool.right= 32000;  
	h = TENew (&rtool, &dtool);
	(**h).crOnly= -1;
  Nlm_LoadTextData (t, h, NULL, NULL, FALSE, gDialogTextFont, gDlogTextHeight, 
  		FALSE, FALSE, TRUE, 1, NULL, NULL, tabProc);
  Nlm_SetDialogText ((Nlm_GraphiC) t, 0, local, FALSE);
  if (h != NULL) {
    TEAutoView (TRUE, h);
  }
#endif

#ifdef WIN_MSWIN
  style = WS_CHILD | ES_LEFT | ES_AUTOHSCROLL;
  h = CreateWindow ("Edit", local, style,
                    r.left, r.top, r.right - r.left,
                    r.bottom - r.top, wptr, 0,
                    Nlm_currentHInst, NULL);
  if (h != NULL) {
    SetProp (h, (LPSTR) "Nlm_VibrantProp", (Nlm_HandleTool) t);
#if 1
{
	/* vibrant way */
  HFONT    oldFont;
  Nlm_FntPtr      fntptr;
  oldFont = NULL;
  if (gDialogTextFont != NULL) {
    fntptr = (Nlm_FntPtr) Nlm_HandLock (gDialogTextFont);
    if (fntptr != NULL && fntptr->handle != NULL) {
      oldFont = SelectObject (Nlm_currentHDC, fntptr->handle);
    }
    Nlm_HandUnlock (gDialogTextFont);
  } else {
    oldFont = SelectObject (Nlm_currentHDC, GetStockObject (ANSI_FIXED_FONT));
  }
  if (oldFont != NULL) {
    SelectObject (Nlm_currentHDC, oldFont);
    SetWindowFont (h, oldFont, FALSE);
  }
 }
#else
	/* dgg way */
		{
		Nlm_FontData	fdata;
		Nlm_GetFontData( gDialogTextFont, &fdata);
		PostMessage( h, WM_SETFONT, fdata.handle, 0L);
		}
#endif

  }
  Nlm_LoadTextData (t, h, NULL, NULL, FALSE, gDialogTextFont, gDlogTextHeight, 
  	FALSE, FALSE, TRUE, 1, NULL, NULL, tabProc);
  if (lpfnNewTextProc == NULL) {
    lpfnNewTextProc = (WNDPROC) MakeProcInstance ((FARPROC) TextProc, Nlm_currentHInst);
  }
  if (lpfnOldTextProc == NULL) {
    lpfnOldTextProc = (WNDPROC) GetWindowLong (h, GWL_WNDPROC);
  } else if (lpfnOldTextProc != (WNDPROC) GetWindowLong (h, GWL_WNDPROC)) {
    Nlm_Message (MSG_ERROR, "TextProc subclass error");
  }
  SetWindowLong (h, GWL_WNDPROC, (LONG) lpfnNewTextProc);
#endif

#ifdef WIN_MOTIF
  allowTextCallback = FALSE;
  fontlist = NULL;
#ifdef DCLAP
  if (gDialogTextFont != NULL) {
  	Nlm_FntPtr fntptr;
    fntptr = (Nlm_FntPtr) Nlm_HandLock (gDialogTextFont);
    if (fntptr != NULL && fntptr->handle != NULL) {
      fontlist = XmFontListCreate (fntptr->handle, XmSTRING_DEFAULT_CHARSET);
    }
    Nlm_HandUnlock (font);
  }
#endif
  n = 0;
  XtSetArg (wargs [n], XmNx, (Position) r.left); n++;
  XtSetArg (wargs [n], XmNy, (Position) r.top); n++;
  XtSetArg (wargs [n], XmNwidth, (Dimension) (r.right - r.left)); n++;
  XtSetArg (wargs [n], XmNheight, (Dimension) (r.bottom - r.top)); n++;
#ifdef OS_VMS
  XtSetArg (wargs [n], XmNmarginHeight, 0); n++;
#else
  XtSetArg (wargs [n], XmNmarginHeight, 0); n++;
#endif
  XtSetArg (wargs [n], XmNmarginWidth, 0); n++;
 	 /* dgg -- need XmNcolumns for horiz. scrolling - must be accurate & reset w/ resize !! */
  if (charWidth) { XtSetArg( wargs[n], XmNcolumns, charWidth); n++; }
  
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNshadowThickness, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNeditMode, XmSINGLE_LINE_EDIT); n++; /* XmMULTI_LINE_EDIT */
#ifdef DCLAP
  XtSetArg (wargs [n], XmNfontList, fontlist); n++;
#else
  XtSetArg (wargs [n], XmNfontList, NULL); n++;
#endif
  XtSetArg (wargs [n], XmNhighlightThickness, 0); n++;
  XtSetArg (wargs [n], XmNautoShowCursorPosition, TRUE); n++;
  XtSetArg (wargs [n], XmNverifyBell, FALSE); n++;
  h = XmCreateText (wptr, (String) "", wargs, n);
  XtOverrideTranslations (h, XtParseTranslationTable ("<Key>Tab: do_tab()"));
  XtVaSetValues (h, XmNuserData, (XtPointer) t, NULL);
  Nlm_LoadTextData (t, h, NULL, NULL, FALSE, gDialogTextFont, gDlogTextHeight, 
  						FALSE, FALSE, FALSE, 1, NULL, NULL, tabProc); 
  XmTextSetString (h, local);
  XmTextShowPosition (h, 0);
  XtAddCallback (h, XmNmodifyVerifyCallback, Nlm_RefreshCallback, (XtPointer) t);
  XtAddCallback (h, XmNvalueChangedCallback, Nlm_TextCallback, (XtPointer) t);
  XtAddCallback (h, XmNactivateCallback, Nlm_ReturnCallback, (XtPointer) t);
  XtAddCallback (h, XmNfocusCallback, Nlm_FocusCallback, (XtPointer) t);
  XtAddCallback (h, XmNlosingFocusCallback, Nlm_LoseFocusCallback, (XtPointer) t);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (t))) {
      XtRealizeWidget (h);
    }
  } else {
    XtRealizeWidget (h);
  }
  allowTextCallback = TRUE;
#ifdef DCLAP
  if (fontlist != NULL) {
    XmFontListFree (fontlist);
  }
#endif
#endif
  Nlm_LoadAction ((Nlm_GraphiC) t, (Nlm_ActnProc) actn);
}

extern Nlm_TexT Nlm_TextLine(Nlm_GrouP prnt, Nlm_CharPtr dfault,
                                Nlm_Int2 charWidth, Nlm_TxtActnProc actn,
                                Nlm_TxtActnProc tabProc)

{
  Nlm_Int2    cwid;
  Nlm_Int2    hbounds;
  Nlm_PoinT   npt;
  Nlm_RecT    r;
  Nlm_Int2    swid;
  Nlm_TexT    t;
  Nlm_WindoW  tempPort;
  Nlm_Int2    vbounds;

  t = NULL;
  if (prnt != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) prnt);
    Nlm_GetNextPosition ((Nlm_GraphiC) prnt, &npt);
		SelectDialogFont();
    cwid = charWidth * Nlm_MaxCharWidth();
    swid = Nlm_StringWidth (dfault);
    if (cwid > swid) {
      swid = cwid;
    }
#ifdef WIN_MAC
    hbounds = 0;
    vbounds = 0;
#endif
#ifdef WIN_MSWIN
    hbounds = 0;
    vbounds = 0;
#endif
#ifdef WIN_MOTIF
    hbounds = 0;
    vbounds = 0;
#endif
    r.left = npt.x;
    r.top = npt.y;
    r.right = r.left + swid + 2 + hbounds * 2;
    r.bottom = r.top + gDlogTextHeight + vbounds * 2;
    t = (Nlm_TexT) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r,
                                   sizeof (Nlm_TextRec), 
                                   hiddenTextProcs); /* dialogTextProcs/ hiddenTextProcs */
    if (t != NULL) {
      Nlm_NewTextLine (t, dfault, charWidth, actn, tabProc);
#ifdef WIN_MAC
      Nlm_DoSelect ((Nlm_GraphiC) t, FALSE);
#endif
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) t, &r, TRUE, FALSE);
      Nlm_DoShow ((Nlm_GraphiC) t, TRUE, FALSE);
    }
    Nlm_RestorePort (tempPort);
  }
  return t;
}

#endif /* DCLAP */


static void Nlm_NewScrollText (Nlm_TexT t, Nlm_Int2 height,
                               Nlm_FonT font, Nlm_Int2 fnthgt,
                               Nlm_Boolean wrap, Nlm_TxtActnProc actn)

{
  Nlm_TextTool    h;
  Nlm_BaR         hsb;
  Nlm_RecT        r;
  Nlm_BaR         vsb;
  Nlm_WindowTool  wptr;
#ifdef WIN_MAC
  Nlm_RectTool    dtool;
  Nlm_RectTool    rtool;
  Nlm_Int2        width;
#endif
#ifdef WIN_MSWIN
  Nlm_FntPtr      fntptr;
  HFONT           oldFont;
  Nlm_Uint4       style;
#endif
#ifdef WIN_MOTIF
  Nlm_FntPtr      fntptr;
  XmFontList      fontlist;
  Cardinal        n;
  Arg             wargs [15];
#endif

  Nlm_GetRect ((Nlm_GraphiC) t, &r);
  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) t);
#ifdef WIN_MAC
  vsb = NULL;
  hsb = NULL;
  r.left = r.right;
  r.right += Nlm_vScrollBarWidth;
  vsb = Nlm_VertScrollBar ((Nlm_GraphiC) t, &r, Nlm_VScrollAction);
  if (! wrap) {
    Nlm_GetRect ((Nlm_GraphiC) t, &r);
    r.top = r.bottom;
    r.bottom += Nlm_hScrollBarHeight;
    hsb = Nlm_HorizScrollBar ((Nlm_GraphiC) t, &r, Nlm_HScrollAction);
  }
  Nlm_GetRect ((Nlm_GraphiC) t, &r);
  Nlm_InsetRect (&r, 4, 2);
  if (fnthgt > 0) {
    height = (r.bottom - r.top + 1) / fnthgt;
  } else {
    height = 0;
  }
  Nlm_DoSetRange ((Nlm_GraphiC) vsb, height - 1, height - 1, 0, FALSE);
  r.bottom = r.top + height * fnthgt;
  Nlm_RecTToRectTool (&r, &rtool);
  if (! wrap) {
    width = (r.right - r.left + 1) / Nlm_stdCharWidth;
    Nlm_DoSetRange ((Nlm_GraphiC) hsb, width - 1, width - 1, 0, FALSE);
    r.right += HSCROLL_POSITIONS * Nlm_stdCharWidth;
  }
  Nlm_RecTToRectTool (&r, &dtool);
  h = TENew (&dtool, &rtool);
  Nlm_LoadTextData (t, h, vsb, hsb, wrap, font, fnthgt, 
  			FALSE, FALSE, FALSE, height, NULL, NULL, NULL);
#endif
#ifdef WIN_MSWIN
  vsb = NULL;
  hsb = NULL;
  r.right += Nlm_vScrollBarWidth;
  style = WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL | ES_LEFT;
  if (! wrap) {
    style |= WS_HSCROLL | ES_AUTOHSCROLL;
    r.bottom += Nlm_hScrollBarHeight;
  }
  h = CreateWindow ("Edit", "", style,
                    r.left, r.top, r.right - r.left,
                    r.bottom - r.top, wptr, (Nlm_HandleTool) t,
                    Nlm_currentHInst, NULL);
  if (h != NULL) {
    SetProp (h, (LPSTR) "Nlm_VibrantProp", (Nlm_HandleTool) t);
  }
  oldFont = NULL;
  if (font != NULL) {
    fntptr = (Nlm_FntPtr) Nlm_HandLock (font);
    if (fntptr != NULL && fntptr->handle != NULL) {
      oldFont = SelectObject (Nlm_currentHDC, fntptr->handle);
    }
    Nlm_HandUnlock (font);
  } else {
    oldFont = SelectObject (Nlm_currentHDC, GetStockObject (ANSI_FIXED_FONT));
  }
  if (oldFont != NULL) {
    SelectObject (Nlm_currentHDC, oldFont);
    SetWindowFont (h, oldFont, FALSE);
  }
  Nlm_LoadTextData (t, h, vsb, hsb, wrap, font, fnthgt, FALSE, FALSE, FALSE, height, NULL, NULL, NULL);
#endif
#ifdef WIN_MOTIF
  allowTextCallback = FALSE;
  vsb = NULL;
  hsb = NULL;
  r.right += Nlm_vScrollBarWidth;
  if (! wrap) {
    r.bottom += Nlm_hScrollBarHeight;
  }
  Nlm_InsetRect (&r, 3, 3);
  fontlist = NULL;
  if (font != NULL) {
    fntptr = (Nlm_FntPtr) Nlm_HandLock (font);
    if (fntptr != NULL && fntptr->handle != NULL) {
      fontlist = XmFontListCreate (fntptr->handle, XmSTRING_DEFAULT_CHARSET);
    }
    Nlm_HandUnlock (font);
  }
  n = 0;
  XtSetArg (wargs [n], XmNx, (Position) r.left); n++;
  XtSetArg (wargs [n], XmNy, (Position) r.top); n++;
  XtSetArg (wargs [n], XmNwidth, (Dimension) (r.right - r.left)); n++;
  XtSetArg (wargs [n], XmNheight, (Dimension) (r.bottom - r.top)); n++;
  XtSetArg (wargs [n], XmNmarginHeight, 3); n++;
  XtSetArg (wargs [n], XmNmarginWidth, 3); n++;
  XtSetArg (wargs [n], XmNborderWidth, (Dimension) 0); n++;
  XtSetArg (wargs [n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
  if (wrap) {
    XtSetArg (wargs [n], XmNscrollHorizontal, FALSE); n++;
    XtSetArg (wargs [n], XmNwordWrap, TRUE); n++;
  }
  XtSetArg (wargs [n], XmNfontList, fontlist); n++;
  XtSetArg (wargs [n], XmNhighlightThickness, 0); n++;
  h = XmCreateScrolledText (wptr, (String) "", wargs, n);
  Nlm_LoadTextData (t, h, vsb, hsb, wrap, font, fnthgt, FALSE, FALSE, FALSE, height, NULL, NULL, NULL);
  XtAddCallback (h, XmNvalueChangedCallback, Nlm_TextCallback, (XtPointer) t);
  XtAddCallback (h, XmNfocusCallback, Nlm_FocusCallback, (XtPointer) t);
  XtAddCallback (h, XmNlosingFocusCallback, Nlm_LoseFocusCallback, (XtPointer) t);
  XtManageChild (h);
  if (NLM_QUIET) {
    if (Nlm_WindowHasBeenShown (Nlm_ParentWindow (t))) {
      XtRealizeWidget (h);
    }
  } else {
    XtRealizeWidget (h);
  }
  allowTextCallback = TRUE;
  if (fontlist != NULL) {
    XmFontListFree (fontlist);
  }
#endif
  Nlm_LoadAction ((Nlm_GraphiC) t, (Nlm_ActnProc) actn);
}




/*#ifdef DCLAP*/

extern void Nlm_SetDialogTextFont(Nlm_FonT theFont)
{
	gDialogTextFont= theFont; /* dgg -- let users choose font */
}
/*#endif*/

extern Nlm_TexT Nlm_DialogText (Nlm_GrouP prnt, Nlm_CharPtr dfault,
                                Nlm_Int2 charWidth, Nlm_TxtActnProc actn)

{
  Nlm_Int2    cwid;
  Nlm_Int2    hbounds;
  Nlm_PoinT   npt;
  Nlm_RecT    r;
  Nlm_Int2    swid;
  Nlm_TexT    t;
  Nlm_WindoW  tempPort;
  Nlm_Int2    vbounds;

  t = NULL;
  if (prnt != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) prnt);
    Nlm_GetNextPosition ((Nlm_GraphiC) prnt, &npt);
		SelectDialogFont();
#ifdef DCLAP
    cwid = charWidth * Nlm_MaxCharWidth();
#else
    cwid = charWidth * Nlm_stdCharWidth;
#endif
    swid = Nlm_StringWidth (dfault);
    if (cwid > swid) {
      swid = cwid;
    }
#ifdef WIN_MAC
    hbounds = 2;
    vbounds = 2;
#endif
#ifdef WIN_MSWIN
    hbounds = 3;
#ifdef DCLAP
    vbounds = (Nlm_FontHeight() * 3 / 2 - gDlogTextHeight) / 2;
#else
    vbounds = (Nlm_stdFontHeight * 3 / 2 - gDlogTextHeight) / 2;
#endif
#endif
#ifdef WIN_MOTIF
    hbounds = 8;
    vbounds = 8;
#endif
    r.left = npt.x;
    r.top = npt.y;
    r.right = r.left + swid + 2 + hbounds * 2;
    r.bottom = r.top + gDlogTextHeight + vbounds * 2;
    t = (Nlm_TexT) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r,
                                   sizeof (Nlm_TextRec), dialogTextProcs);
    if (t != NULL) {
      Nlm_NewDialogText (t, dfault, actn);
#ifdef WIN_MAC
      Nlm_DoSelect ((Nlm_GraphiC) t, FALSE);
#endif
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) t, &r, TRUE, FALSE);
      Nlm_DoShow ((Nlm_GraphiC) t, TRUE, FALSE);
    }
    Nlm_RestorePort (tempPort);
  }
  return t;
}

extern Nlm_TexT Nlm_HiddenText (Nlm_GrouP prnt, Nlm_CharPtr dfault,
                                Nlm_Int2 charWidth, Nlm_TxtActnProc actn,
                                Nlm_TxtActnProc tabProc)

{
  Nlm_Int2    cwid;
  Nlm_Int2    hbounds;
  Nlm_PoinT   npt;
  Nlm_RecT    r;
  Nlm_Int2    swid;
  Nlm_TexT    t;
  Nlm_WindoW  tempPort;
  Nlm_Int2    vbounds;

  t = NULL;
  if (prnt != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) prnt);
    Nlm_GetNextPosition ((Nlm_GraphiC) prnt, &npt);
		SelectDialogFont();
#ifdef DCLAP
    cwid = charWidth * Nlm_MaxCharWidth();
#else
    cwid = charWidth * Nlm_stdCharWidth;
#endif
    swid = Nlm_StringWidth (dfault);
    if (cwid > swid) {
      swid = cwid;
    }
#ifdef WIN_MAC
    hbounds = 0;
    vbounds = 0;
#endif
#ifdef WIN_MSWIN
    hbounds = 0;
    vbounds = 0;
#endif
#ifdef WIN_MOTIF
    hbounds = 0;
    vbounds = 0;
#endif
    r.left = npt.x;
    r.top = npt.y;
    r.right = r.left + swid + 2 + hbounds * 2;
    r.bottom = r.top + gDlogTextHeight + vbounds * 2;
    t = (Nlm_TexT) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r,
                                   sizeof (Nlm_TextRec), hiddenTextProcs);
    if (t != NULL) {
      Nlm_NewHiddenText (t, dfault, actn, tabProc);
#ifdef WIN_MAC
      Nlm_DoSelect ((Nlm_GraphiC) t, FALSE);
#endif
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) t, &r, TRUE, FALSE);
      Nlm_DoShow ((Nlm_GraphiC) t, TRUE, FALSE);
    }
    Nlm_RestorePort (tempPort);
  }
  return t;
}

extern Nlm_TexT Nlm_PasswordText (Nlm_GrouP prnt, Nlm_CharPtr dfault,
                                  Nlm_Int2 charWidth, Nlm_TxtActnProc actn)

{
  Nlm_Int2    cwid;
  Nlm_Int2    hbounds;
  Nlm_PoinT   npt;
  Nlm_RecT    r;
  Nlm_Int2    swid;
  Nlm_TexT    t;
  Nlm_WindoW  tempPort;
  Nlm_Int2    vbounds;

  t = NULL;
  if (prnt != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) prnt);
    Nlm_GetNextPosition ((Nlm_GraphiC) prnt, &npt);
		SelectDialogFont();
#ifdef DCLAP
    cwid = charWidth * Nlm_MaxCharWidth();
#else
    cwid = charWidth * Nlm_stdCharWidth;
#endif
    swid = Nlm_StringWidth (dfault);
    if (cwid > swid) {
      swid = cwid;
    }
#ifdef WIN_MAC
    hbounds = 2;
    vbounds = 2;
#endif
#ifdef WIN_MSWIN
    hbounds = 3;
    vbounds = (Nlm_stdFontHeight * 3 / 2 - gDlogTextHeight) / 2;
#endif
#ifdef WIN_MOTIF
    hbounds = 8;
    vbounds = 8;
#endif
    r.left = npt.x;
    r.top = npt.y;
    r.right = r.left + swid + 2 + hbounds * 2;
    r.bottom = r.top + gDlogTextHeight + vbounds * 2;
    t = (Nlm_TexT) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r,
                                   sizeof (Nlm_PasswdRec), passwordTextProcs);
    if (t != NULL) {
      Nlm_NewPasswordText (t, dfault, actn);
#ifdef WIN_MAC
      Nlm_DoSelect ((Nlm_GraphiC) t, FALSE);
#endif
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) t, &r, TRUE, FALSE);
      Nlm_DoShow ((Nlm_GraphiC) t, TRUE, FALSE);
    }
    Nlm_RestorePort (tempPort);
  }
  return t;
}

extern Nlm_TexT Nlm_ScrollText (Nlm_GrouP prnt, Nlm_Int2 width,
                                Nlm_Int2 height, Nlm_FonT font,
                                Nlm_Boolean wrap, Nlm_TxtActnProc actn)

{
  Nlm_Int2    dwid;
  Nlm_Int2    fnthgt;
  Nlm_PoinT   npt;
  Nlm_RecT    r;
  Nlm_TexT    t;
  Nlm_WindoW  tempPort;
  Nlm_Int2    thgt;

  t = NULL;
  if (prnt != NULL) {
    tempPort = Nlm_SavePort ((Nlm_GraphiC) prnt);
    Nlm_GetNextPosition ((Nlm_GraphiC) prnt, &npt);
    if (font == NULL) {
      font = Nlm_programFont;
    }
    Nlm_SelectFont (font);
    fnthgt = Nlm_LineHeight ();
#ifdef DCLAP
    dwid = width * Nlm_MaxCharWidth();
    thgt = height * fnthgt;
#else
    dwid = width * Nlm_stdCharWidth;
    thgt = height * Nlm_stdLineHeight;
#endif
    Nlm_LoadRect (&r, npt.x, npt.y, npt.x+dwid+8, npt.y+thgt+4);
    t = (Nlm_TexT) Nlm_CreateLink ((Nlm_GraphiC) prnt, &r, sizeof (Nlm_TextRec), scrollTextProcs);
    if (t != NULL) {
      Nlm_NewScrollText (t, height, font, fnthgt, wrap, actn);
      r.right += Nlm_vScrollBarWidth;
      if (! wrap) {
        r.bottom += Nlm_hScrollBarHeight;
      }
      Nlm_DoAdjustPrnt ((Nlm_GraphiC) t, &r, TRUE, FALSE);
			SelectDialogFont();
      Nlm_DoShow ((Nlm_GraphiC) t, TRUE, FALSE);
    }
    Nlm_RestorePort (tempPort);
  }
  return t;
}



extern void Nlm_PassPanelClickToText (Nlm_PaneL p, Nlm_TexT t, Nlm_PoinT pt)

{
#ifdef WIN_MSWIN
  Nlm_TextTool  h;
  UINT          message;
  WPARAM        wParam;
#endif
#ifdef WIN_MOTIF
  Nlm_TextTool  h;
#endif

  if (t != NULL) {
    Nlm_KillSlateTimer ();
#ifdef WIN_MAC
    Nlm_DoClick ((Nlm_GraphiC) t, pt);
#endif
#ifdef WIN_MSWIN
    ReleaseCapture ();
    h = Nlm_GetTextHandle (t);
    if (Nlm_dblClick) {
      message = WM_LBUTTONDBLCLK;
    } else {
      message = WM_LBUTTONDOWN;
    }
    if (Nlm_shftKey) {
      wParam = MK_SHIFT;
    } else {
      wParam = 0;
    }
/*
    Edit_SetSel (h, 0, 0);
    if (Nlm_Visible (t) && Nlm_AllParentsVisible (t)) {
      SetFocus (h);
      Nlm_DoTextSelect (t);
    }
*/
    SendMessage (h, message, Nlm_currentWParam, Nlm_currentLParam);
#endif
#ifdef WIN_MOTIF
  if (Nlm_WindowHasBeenShown (Nlm_ParentWindow ((Nlm_Handle) t))) {
    allowTextCallback = FALSE;
    h = Nlm_GetTextHandle (t);
    XmTextSetSelection (h, (XmTextPosition) 0,
                        (XmTextPosition) 0, (Time) 0);
    XmProcessTraversal (h, XmTRAVERSE_CURRENT);
    XmTextSetHighlight (h, (XmTextPosition) 0,
                        (XmTextPosition) 0, XmHIGHLIGHT_SELECTED);
    allowTextCallback = TRUE;
  }
  Nlm_DoTextSelect (t);
#endif
  }
}

#ifdef WIN_MAC
extern Nlm_Boolean Nlm_RegisterTexts (void)

{
  return TRUE;
}
#endif

#ifdef WIN_MSWIN
extern Nlm_Boolean Nlm_RegisterTexts (void)

{
  return TRUE;
}
#endif

#ifdef WIN_MOTIF
extern Nlm_Boolean Nlm_RegisterTexts (void)

{
  XtActionsRec  actions;

  actions.string = "do_tab";
  actions.proc = Nlm_TabCallback;
  XtAppAddActions (Nlm_appContext, &actions, 1);
  return TRUE;
}
#endif

extern void Nlm_FreeTexts (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemFree (gphprcsptr);
}

extern void Nlm_InitTexts (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemNew (sizeof (Nlm_GphPrcs) * 4);

  dialogTextProcs = &(gphprcsptr [0]);
#ifdef WIN_MAC
  dialogTextProcs->click = Nlm_DialogTextClick;
  dialogTextProcs->key = Nlm_DialogKey;
  dialogTextProcs->draw = Nlm_DrawDialogText;
  dialogTextProcs->idle = Nlm_IdleText;
#endif
#ifdef WIN_MSWIN
  dialogTextProcs->command = Nlm_DialogTextCommand;
#endif
#ifdef WIN_MOTIF
#endif
  dialogTextProcs->show = Nlm_ShowText;
  dialogTextProcs->hide = Nlm_HideText;
  dialogTextProcs->enable = Nlm_EnableText;
  dialogTextProcs->disable = Nlm_DisableText;
  dialogTextProcs->activate = Nlm_ActivateText;
  dialogTextProcs->deactivate = Nlm_DeactivateText;
  dialogTextProcs->remove = Nlm_RemoveText;
  dialogTextProcs->reset = Nlm_ResetText;
  dialogTextProcs->select = Nlm_TextSelectProc;
  dialogTextProcs->setTitle = Nlm_SetDialogText;
  dialogTextProcs->getTitle = Nlm_GetDialogText;
  dialogTextProcs->setPosition = Nlm_SetTextPosition;
  dialogTextProcs->getPosition = Nlm_GetTextPosition;
  dialogTextProcs->gainFocus = Nlm_TextGainFocus;
  dialogTextProcs->loseFocus = Nlm_TextLoseFocus;

  hiddenTextProcs = &(gphprcsptr [1]);
#ifdef WIN_MAC
  hiddenTextProcs->click = Nlm_DialogTextClick;
  hiddenTextProcs->key = Nlm_DialogKey;
  hiddenTextProcs->draw = Nlm_DrawHiddenText;
  hiddenTextProcs->idle = Nlm_IdleText;
#endif
#ifdef WIN_MSWIN
  hiddenTextProcs->command = Nlm_DialogTextCommand;
#endif
#ifdef WIN_MOTIF
#endif
  hiddenTextProcs->show = Nlm_ShowText;
  hiddenTextProcs->hide = Nlm_HideText;
  hiddenTextProcs->enable = Nlm_EnableText;
  hiddenTextProcs->disable = Nlm_DisableText;
  hiddenTextProcs->activate = Nlm_ActivateHiddenText;
  hiddenTextProcs->deactivate = Nlm_DeactivateText;
  hiddenTextProcs->remove = Nlm_RemoveText;
  hiddenTextProcs->reset = Nlm_ResetText;
  hiddenTextProcs->select = Nlm_TextSelectProc;
  hiddenTextProcs->setTitle = Nlm_SetDialogText;
  hiddenTextProcs->getTitle = Nlm_GetDialogText;
  hiddenTextProcs->setPosition = Nlm_SetHiddenTextPosition;
  hiddenTextProcs->getPosition = Nlm_GetTextPosition;
  hiddenTextProcs->gainFocus = Nlm_TextGainFocus;
  hiddenTextProcs->loseFocus = Nlm_TextLoseFocus;

  passwordTextProcs = &(gphprcsptr [2]);
#ifdef WIN_MAC
  passwordTextProcs->click = Nlm_DialogTextClick;
  passwordTextProcs->key = Nlm_PasswordKey;
  passwordTextProcs->draw = Nlm_DrawDialogText;
  passwordTextProcs->idle = Nlm_IdleText;
#endif
#ifdef WIN_MSWIN
  passwordTextProcs->command = Nlm_PasswordTextCommand;
#endif
#ifdef WIN_MOTIF
#endif
  passwordTextProcs->show = Nlm_ShowText;
  passwordTextProcs->hide = Nlm_HideText;
  passwordTextProcs->enable = Nlm_EnableText;
  passwordTextProcs->disable = Nlm_DisableText;
  passwordTextProcs->activate = Nlm_ActivateText;
  passwordTextProcs->deactivate = Nlm_DeactivateText;
  passwordTextProcs->remove = Nlm_RemoveText;
  passwordTextProcs->reset = Nlm_ResetText;
  passwordTextProcs->select = Nlm_TextSelectProc;
  passwordTextProcs->setTitle = Nlm_SetPasswordText;
  passwordTextProcs->getTitle = Nlm_GetPasswordText;
  passwordTextProcs->setPosition = Nlm_SetTextPosition;
  passwordTextProcs->getPosition = Nlm_GetTextPosition;
  passwordTextProcs->gainFocus = Nlm_TextGainFocus;
  passwordTextProcs->loseFocus = Nlm_TextLoseFocus;

  scrollTextProcs = &(gphprcsptr [3]);
#ifdef WIN_MAC
  scrollTextProcs->click = Nlm_ScrollTextClick;
  scrollTextProcs->key = Nlm_TextKey;
  scrollTextProcs->draw = Nlm_DrawScrollText;
  scrollTextProcs->idle = Nlm_IdleText;
#endif
#ifdef WIN_MSWIN
  scrollTextProcs->command = Nlm_ScrollTextCommand;
#endif
#ifdef WIN_MOTIF
#endif
  scrollTextProcs->show = Nlm_ShowScrollText;
  scrollTextProcs->hide = Nlm_HideScrollText;
  scrollTextProcs->enable = Nlm_EnableText;
  scrollTextProcs->disable = Nlm_DisableText;
  scrollTextProcs->activate = Nlm_ActivateScrollText;
  scrollTextProcs->deactivate = Nlm_DeactivateScrollText;
  scrollTextProcs->remove = Nlm_RemoveText;
  scrollTextProcs->reset = Nlm_ResetText;
  scrollTextProcs->select = Nlm_TextSelectProc;
  scrollTextProcs->setTitle = Nlm_SetScrollText;
  scrollTextProcs->getTitle = Nlm_GetScrollText;
  scrollTextProcs->setPosition = Nlm_SetScrollTextPosition;
  scrollTextProcs->getPosition = Nlm_GetScrollTextPosition;
  scrollTextProcs->loseFocus = Nlm_TextLoseFocus;
}
