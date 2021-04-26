/*   vibextra.c
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
* File Name:  vibextra.c
*
* Author:  Jonathan Kans
*
* Version Creation Date:   3/9/93
*
* $Revision: 1.15 $
*
* File Description: 
*       Vibrant miscellaneous extensions
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
#include <ncbiport.h>

#ifdef VAR_ARGS
#include <varargs.h>
#else
#include <stdarg.h>
#endif

typedef  struct  Nlm_repeatdata {
  Nlm_Handle       title;
  Nlm_RptClckProc  action;
} Nlm_RepeatData;

typedef struct Nlm_switchdata {
  Nlm_Int2         max;
  Nlm_Int2         val;
  Nlm_Boolean      text;
  Nlm_Boolean      vert;
  Nlm_Boolean      upActv;
  Nlm_Boolean      dnActv;
  Nlm_SwtChngProc  actn;
} Nlm_SwitchData;

typedef  struct  Nlm_icondata {
  Nlm_IcnChngProc  inval;
  Nlm_Int2         value;
  Nlm_Boolean      status;
  Nlm_Handle       title;
} Nlm_IconData;

static Nlm_GphPrcsPtr  gphprcsptr = NULL;

static Nlm_GphPrcsPtr  repeatProcs;
static Nlm_GphPrcsPtr  switchProcs;
static Nlm_GphPrcsPtr  iconProcs;

static Nlm_Boolean     inRepeatButton;

static Nlm_Uint1 upFillArrow [] = {
  0x10, 0x38, 0x38, 0x7C, 0x7C, 0xFE, 0xFE, 0x00
};

static Nlm_Uint1 downFillArrow [] = {
  0xFE, 0xFE, 0x7C, 0x7C, 0x38, 0x38, 0x10, 0x00
};

static Nlm_Uint1 leftFillArrow [] = {
  0x06, 0x1E, 0x7E, 0xFE, 0x7E, 0x1E, 0x06, 0x00
};

static Nlm_Uint1 rightFillArrow [] = {
  0xC0, 0xF0, 0xFC, 0xFE, 0xFC, 0xF0, 0xC0, 0x00
};

static Nlm_Uint1 upOpenArrow [] = {
  0x10, 0x28, 0x28, 0x44, 0x44, 0x82, 0xFE, 0x00
};

static Nlm_Uint1 downOpenArrow [] = {
  0xFE, 0x82, 0x44, 0x44, 0x28, 0x28, 0x10, 0x00
};

static Nlm_Uint1 leftOpenArrow [] = {
  0x06, 0x1A, 0x62, 0x82, 0x62, 0x1A, 0x06, 0x00
};

static Nlm_Uint1 rightOpenArrow [] = {
  0xC0, 0xB0, 0x8C, 0x82, 0x8C, 0xB0, 0xC0, 0x00
};

#ifdef VAR_ARGS
extern void CDECL Nlm_AlignObjects (align, va_alist)
int align;
va_dcl
#else
extern void CDECL Nlm_AlignObjects (int align, ...)
#endif

{
  va_list     args;
  Nlm_Int2    delta;
  Nlm_Int2    maxX;
  Nlm_Int2    maxY;
  Nlm_Int2    minX;
  Nlm_Int2    minY;
  Nlm_Handle  obj;
  Nlm_RecT    r;

#ifdef VAR_ARGS
  va_start (args);
#else
  va_start (args, align);
#endif
  minX = 0;
  minY = 0;
  maxX = 0;
  maxY = 0;
  obj = (Nlm_Handle) va_arg (args, Nlm_HANDLE);
  while (obj != NULL) {
    Nlm_GetPosition (obj, &r);
    minX = MAX (minX, r.left);
    minY = MAX (minY, r.top);
    maxX = MAX (maxX, r.right);
    maxY = MAX (maxY, r.bottom);
    obj = (Nlm_Handle) va_arg (args, Nlm_HANDLE);
  }
  va_end(args);

#ifdef VAR_ARGS
  va_start (args);
#else
  va_start (args, align);
#endif
  obj = (Nlm_Handle) va_arg (args, Nlm_HANDLE);
  while (obj != NULL) {
    Nlm_GetPosition (obj, &r);
    switch (align) {
      case ALIGN_LEFT:
        if (r.left < minX) {
          r.left = minX;
          Nlm_SetPosition (obj, &r);
          Nlm_DoAdjustPrnt ((Nlm_GraphiC) obj, &r, FALSE, TRUE);
        }
        break;
      case ALIGN_RIGHT:
        if (r.right < maxX) {
          r.right = maxX;
          Nlm_SetPosition (obj, &r);
          Nlm_DoAdjustPrnt ((Nlm_GraphiC) obj, &r, FALSE, TRUE);
        }
        break;
      case ALIGN_CENTER:
        delta = maxX - r.right;
        if (delta > 0) {
          Nlm_OffsetRect (&r, delta/2, 0);
          Nlm_SetPosition (obj, &r);
          Nlm_DoAdjustPrnt ((Nlm_GraphiC) obj, &r, FALSE, TRUE);
        }
        break;
      case ALIGN_JUSTIFY:
        if (r.left < minX || r.right < maxX) {
          r.left = minX;
          r.right = maxX;
          Nlm_SetPosition (obj, &r);
          Nlm_DoAdjustPrnt ((Nlm_GraphiC) obj, &r, FALSE, TRUE);
        }
        break;
      case ALIGN_UPPER:
        if (r.top < minY) {
          r.top = minY;
          Nlm_SetPosition (obj, &r);
          Nlm_DoAdjustPrnt ((Nlm_GraphiC) obj, &r, FALSE, TRUE);
        }
        break;
      case ALIGN_LOWER:
        if (r.bottom < maxY) {
          r.bottom = maxY;
          Nlm_SetPosition (obj, &r);
          Nlm_DoAdjustPrnt ((Nlm_GraphiC) obj, &r, FALSE, TRUE);
        }
        break;
      case ALIGN_MIDDLE:
        delta = maxY - r.bottom;
        if (delta > 0) {
          Nlm_OffsetRect (&r, 0, delta/2);
          Nlm_SetPosition (obj, &r);
          Nlm_DoAdjustPrnt ((Nlm_GraphiC) obj, &r, FALSE, TRUE);
        }
        break;
      case ALIGN_VERTICAL:
        if (r.top < minY || r.bottom < maxY) {
          r.top = minY;
          r.bottom = maxY;
          Nlm_SetPosition (obj, &r);
          Nlm_DoAdjustPrnt ((Nlm_GraphiC) obj, &r, FALSE, TRUE);
        }
        break;
      default:
        break;
    }
    obj = (Nlm_Handle) va_arg (args, Nlm_HANDLE);
  }
  va_end(args);
}

static void Nlm_DrawSwitchProc (Nlm_PaneL s)

{
  Nlm_RecT        dn;
  Nlm_SwitchData  extra;
  Nlm_Int2        mid;
  Nlm_RecT        r;
  Nlm_Char        str [32];
  Nlm_RecT        tx;
  Nlm_RecT        up;

  Nlm_GetPanelExtra (s, &extra);
  Nlm_ObjectRect (s, &r);
  if (extra.text && extra.max > 0 && extra.val > 0) {
    sprintf (str, "%d/%d", (int) extra.val, (int) extra.max);
    Nlm_SelectFont (Nlm_programFont);
    if (extra.vert) {
      Nlm_LoadRect (&tx, r.left + 1, r.top + 1, r.right - 11, r.bottom - 1);
    } else {
      Nlm_LoadRect (&tx, r.left + 1, r.top + 1, r.right - 20, r.bottom - 1);
    }
    Nlm_DrawString (&tx, str, 'r', FALSE);
    Nlm_SelectFont (Nlm_systemFont);
  }
  Nlm_FrameRect (&r);
  if (extra.vert) {
    Nlm_LoadRect (&up, r.right - 9, r.top + 2, r.right - 2, r.top + 9);
    if (extra.upActv) {
      Nlm_CopyBits (&up, upFillArrow);
    } else {
      Nlm_CopyBits (&up, upOpenArrow);
    }
  } else {
    mid = (r.top + r.bottom) / 2;
    Nlm_LoadRect (&up, r.right - 9, mid - 3, r.right - 2, mid + 4);
    if (extra.upActv) {
      Nlm_CopyBits (&up, rightFillArrow);
    } else {
      Nlm_CopyBits (&up, rightOpenArrow);
    }
  }
  if (extra.vert) {
    Nlm_LoadRect (&dn, r.right - 9, r.bottom - 9, r.right - 2, r.bottom - 2);
    if (extra.dnActv) {
      Nlm_CopyBits (&dn, downFillArrow);
    } else {
      Nlm_CopyBits (&dn, downOpenArrow);
    }
  } else {
    mid = (r.top + r.bottom) / 2;
    Nlm_LoadRect (&dn, r.right - 18, mid - 3, r.right - 11, mid + 4);
    if (extra.dnActv) {
      Nlm_CopyBits (&dn, leftFillArrow);
    } else {
      Nlm_CopyBits (&dn, leftOpenArrow);
    }
  }
}

static void Nlm_SwitchClickProc (Nlm_PaneL s, Nlm_PoinT pt)

{
  Nlm_RecT        dn;
  Nlm_SwitchData  extra;
  Nlm_Int2        mid;
  Nlm_RecT        r;
  Nlm_RecT        up;

  Nlm_GetPanelExtra (s, &extra);
  Nlm_ObjectRect (s, &r);
  if (extra.vert) {
    Nlm_LoadRect (&up, r.right - 9, r.top + 2, r.right - 2, r.top + 9);
    Nlm_LoadRect (&dn, r.right - 9, r.bottom - 9, r.right - 2, r.bottom - 2);
  } else {
    mid = (r.top + r.bottom) / 2;
    Nlm_LoadRect (&up, r.right - 9, mid - 3, r.right - 2, mid + 4);
    Nlm_LoadRect (&dn, r.right - 18, mid - 3, r.right - 11, mid + 4);
  }
  if (Nlm_PtInRect (pt, &up)) {
    if (extra.val < extra.max) {
      Nlm_DoSetValue ((Nlm_GraphiC) s, extra.val + 1, FALSE);
      if (extra.actn != NULL) {
        extra.actn ((Nlm_SwitcH) s, extra.val + 1, extra.val);
      }
    }
  } else if (Nlm_PtInRect (pt, &dn)) {
    if (extra.val > 1) {
      Nlm_DoSetValue ((Nlm_GraphiC) s, extra.val - 1, FALSE);
      if (extra.actn != NULL) {
        extra.actn ((Nlm_SwitcH) s, extra.val - 1, extra.val);
      }
    }
  }
}

static Nlm_SwitcH Nlm_CommonSwitch (Nlm_GrouP prnt, Nlm_Boolean text,
                                    Nlm_Boolean vert, Nlm_SwtChngProc actn)

{
  Nlm_SwitchData  extra;
  Nlm_Int2        height;
  Nlm_PaneL       s;
  Nlm_Int2        width;

  s = NULL;
  if (prnt != NULL) {
    Nlm_SelectFont (Nlm_programFont);
    if (vert) {
      height = 20;
      width = 11;
    } else {
      height = 11;
      width = 20;
    }
    if (text) {
      width += Nlm_StringWidth ("99/99") + 4;
      height = MAX (height, Nlm_LineHeight () + 2);
    }
    Nlm_SelectFont (Nlm_systemFont);
    s = Nlm_AutonomousPanel (prnt, width, height, Nlm_DrawSwitchProc, NULL,
                             NULL, sizeof (Nlm_SwitchData), NULL, switchProcs);
    if (s != NULL) {
      Nlm_SetPanelClick (s, Nlm_SwitchClickProc, NULL, NULL, NULL);
      Nlm_MemSet ((Nlm_VoidPtr) (&extra), 0, sizeof (Nlm_SwitchData));
      extra.max = 0;
      extra.val = 0;
      extra.text = text;
      extra.vert = vert;
      extra.upActv = FALSE;
      extra.dnActv = FALSE;
      extra.actn = actn;
      Nlm_SetPanelExtra (s, &extra);
    }
  }
  return (Nlm_SwitcH) s;
}

extern Nlm_SwitcH Nlm_UpDownSwitch (Nlm_GrouP prnt, Nlm_Boolean text, Nlm_SwtChngProc actn)

{
  return Nlm_CommonSwitch (prnt, text, TRUE, actn);
}

extern Nlm_SwitcH Nlm_LeftRightSwitch (Nlm_GrouP prnt, Nlm_Boolean text, Nlm_SwtChngProc actn)

{
  return Nlm_CommonSwitch (prnt, text, FALSE, actn);
}

static void Nlm_UpdateSwitch (Nlm_SwitcH s)

{
  Nlm_RecT        dn;
  Nlm_SwitchData  extra;
  Nlm_Int2        mid;
  Nlm_RecT        r;
  Nlm_RecT        tx;
  Nlm_RecT        up;

  Nlm_GetPanelExtra ((Nlm_PaneL) s, &extra);
  if (Nlm_Visible (s) && Nlm_AllParentsVisible (s)) {
    Nlm_ObjectRect (s, &r);
    if (extra.vert) {
      Nlm_LoadRect (&up, r.right - 9, r.top + 2, r.right - 2, r.top + 9);
      Nlm_LoadRect (&dn, r.right - 9, r.bottom - 9, r.right - 2, r.bottom - 2);
    } else {
      mid = (r.top + r.bottom) / 2;
      Nlm_LoadRect (&up, r.right - 9, mid - 3, r.right - 2, mid + 4);
      Nlm_LoadRect (&dn, r.right - 18, mid - 3, r.right - 11, mid + 4);
    }
    Nlm_Select (s);
    if (extra.upActv != (Nlm_Boolean) (extra.val < extra.max)) {
      Nlm_InsetRect (&up, -1, -1);
      Nlm_InvalRect (&up);
    }
    if (extra.dnActv != (Nlm_Boolean) (extra.val > 1)) {
      Nlm_InsetRect (&dn, -1, -1);
      Nlm_InvalRect (&dn);
    }
    if (extra.text) {
      if (extra.vert) {
        Nlm_LoadRect (&tx, r.left + 1, r.top + 1, r.right - 10, r.bottom - 1);
      } else {
        Nlm_LoadRect (&tx, r.left + 1, r.top + 1, r.right - 19, r.bottom - 1);
      }
      Nlm_InvalRect (&tx);
    }
  }
  extra.upActv = (Nlm_Boolean) (extra.val < extra.max);
  extra.dnActv = (Nlm_Boolean) (extra.val > 1);
  Nlm_SetPanelExtra ((Nlm_PaneL) s, &extra);
}

static void Nlm_SetSwitchValue (Nlm_GraphiC s, Nlm_Int2 value, Nlm_Boolean savePort)

{
  Nlm_SwitchData  extra;
  Nlm_Int2        oldval;
  Nlm_WindoW      tempPort;

  tempPort = Nlm_SavePortIfNeeded (s, savePort);
  if (s != NULL) {
    Nlm_GetPanelExtra ((Nlm_PaneL) s, &extra);
    oldval = extra.val;
    extra.val = value;
    Nlm_SetPanelExtra ((Nlm_PaneL) s, &extra);
    if (oldval != value) {
      Nlm_UpdateSwitch ((Nlm_SwitcH) s);
    }
  }
  Nlm_RestorePort (tempPort);
}

static Nlm_Int2 Nlm_GetSwitchValue (Nlm_GraphiC s)

{
  Nlm_SwitchData  extra;
  Nlm_Int2        value;

  value = 0;
  if (s != NULL) {
    Nlm_GetPanelExtra ((Nlm_PaneL) s, &extra);
    value = extra.val;
  }
  return value;
}

extern void Nlm_SetSwitchMax (Nlm_SwitcH s, Nlm_Int2 max)

{
  Nlm_SwitchData  extra;
  Nlm_Int2        oldmax;
  Nlm_WindoW      tempPort;

  tempPort = Nlm_SavePort (s);
  if (s != NULL) {
    Nlm_GetPanelExtra ((Nlm_PaneL) s, &extra);
    oldmax = extra.max;
    extra.max = max;
    if (max == 0) {
      extra.val = 0;
    }
    Nlm_SetPanelExtra ((Nlm_PaneL) s, &extra);
    if (oldmax != max) {
      Nlm_UpdateSwitch (s);
    }
  }
  Nlm_RestorePort (tempPort);
}

extern Nlm_Int2 Nlm_GetSwitchMax (Nlm_SwitcH s)

{
  Nlm_SwitchData  extra;
  Nlm_Int2        max;

  max = 0;
  if (s != NULL) {
    Nlm_GetPanelExtra ((Nlm_PaneL) s, &extra);
    max = extra.max;
  }
  return max;
}

extern void Nlm_SetSwitchParams (Nlm_SwitcH s, Nlm_Int2 value, Nlm_Int2 max)

{
  Nlm_SwitchData  extra;
  Nlm_Int2        oldmax;
  Nlm_Int2        oldval;
  Nlm_WindoW      tempPort;

  tempPort = Nlm_SavePort (s);
  if (s != NULL) {
    Nlm_GetPanelExtra ((Nlm_PaneL) s, &extra);
    oldmax = extra.max;
    extra.max = max;
    if (value > max) {
      value = max;
    }
    oldval = extra.val;
    extra.val = value;
    Nlm_SetPanelExtra ((Nlm_PaneL) s, &extra);
    if (oldval != value || oldmax != max) {
      Nlm_UpdateSwitch (s);
    }
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_DoRepeatAction (Nlm_RepeaT rb, Nlm_PoinT pt)

{
  Nlm_RptClckProc  actn;
  Nlm_RepeatData   rdata;

  Nlm_GetPanelExtra ((Nlm_PaneL) rb, &rdata);
  actn = rdata.action;
  if (actn != NULL) {
    actn (rb, pt);
  }
}

static void Nlm_RepeatClick (Nlm_PaneL rb, Nlm_PoinT pt)

{
  Nlm_RecT  r;

  Nlm_GetRect ((Nlm_GraphiC) rb, &r);
  if (Nlm_PtInRect (pt, &r)) {
    inRepeatButton = TRUE;
    Nlm_InsetRect (&r, 2, 2);
    Nlm_InvertRect (&r);
    Nlm_DoRepeatAction ((Nlm_RepeaT) rb, pt);
  }
}

static void Nlm_RepeatPress (Nlm_PaneL rb, Nlm_PoinT pt)

{
  Nlm_RecT  r;

  Nlm_GetRect ((Nlm_GraphiC) rb, &r);
  if (Nlm_PtInRect (pt, &r)) {
    if (! inRepeatButton) {
      inRepeatButton = TRUE;
      Nlm_InsetRect (&r, 2, 2);
      Nlm_InvertRect (&r);
    }
    Nlm_DoRepeatAction ((Nlm_RepeaT) rb, pt);
  } else if (inRepeatButton) {
    inRepeatButton = FALSE;
    Nlm_InsetRect (&r, 2, 2);
    Nlm_InvertRect (&r);
  }
}

static void Nlm_RepeatRelease (Nlm_PaneL rb, Nlm_PoinT pt)

{
  Nlm_RecT  r;

  Nlm_GetRect ((Nlm_GraphiC) rb, &r);
  if (inRepeatButton) {
    inRepeatButton = FALSE;
    Nlm_InsetRect (&r, 2, 2);
    Nlm_InvertRect (&r);
  }
}

static void Nlm_DrawRepeat (Nlm_PaneL rb)

{
  Nlm_Handle      h;
  Nlm_RecT        r;
  Nlm_RepeatData  rdata;
  Nlm_Char        str [64];

  if (Nlm_GetVisible ((Nlm_GraphiC) rb) && Nlm_GetAllParentsVisible ((Nlm_GraphiC) rb)) {
    Nlm_GetRect ((Nlm_GraphiC) rb, &r);
    Nlm_FrameRect (&r);
    Nlm_InsetRect (&r, 1, 1);
    Nlm_FrameRect (&r);
    Nlm_InsetRect (&r, 1, 1);
    Nlm_GetPanelExtra (rb, &rdata);
    h = rdata.title;
    if (h != NULL) {
      Nlm_SelectFont (Nlm_systemFont);
      Nlm_GetString (h, str, sizeof (str));
      if (Nlm_StringLen (str) > 0) {
        Nlm_DrawString (&r, str, 'c', FALSE);
      }
    }
  }
}

static void Nlm_SetRepeatTitle (Nlm_GraphiC rb, Nlm_Int2 item,
                                Nlm_CharPtr title, Nlm_Boolean savePort)

{
  Nlm_Handle      h;
  Nlm_RecT        r;
  Nlm_RepeatData  rdata;
  Nlm_WindoW      tempPort;

  tempPort = Nlm_SavePortIfNeeded (rb, savePort);
  Nlm_GetPanelExtra ((Nlm_PaneL) rb, &rdata);
  h = rdata.title;
  h = Nlm_SetString (h, title);
  rdata.title = h;
  Nlm_SetPanelExtra ((Nlm_PaneL) rb, &rdata);
  if (Nlm_GetVisible (rb) && Nlm_GetAllParentsVisible (rb)) {
    Nlm_GetRect (rb, &r);
    Nlm_InvalRect (&r);
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_GetRepeatTitle (Nlm_GraphiC rb, Nlm_Int2 item,
                                Nlm_CharPtr title, Nlm_sizeT maxsize)

{
  Nlm_Handle      h;
  Nlm_RepeatData  rdata;

  if (title != NULL) {
    Nlm_GetPanelExtra ((Nlm_PaneL) rb, &rdata);
    h = rdata.title;
    Nlm_GetString (h, title, maxsize);
  }
}

static void Nlm_ResetRepeat (Nlm_PaneL rb)

{
  Nlm_RepeatData  rdata;

  Nlm_GetPanelExtra (rb, &rdata);
  if (rdata.title != NULL) {
    rdata.title = Nlm_HandFree (rdata.title);
  }
  Nlm_SetPanelExtra (rb, &rdata);
}

extern Nlm_RepeaT Nlm_RepeatButton (Nlm_GrouP prnt, Nlm_CharPtr title,
                                    Nlm_RptClckProc actn)

{
  Nlm_RepeaT      rb;
  Nlm_RepeatData  rdata;
  Nlm_Int2        vbounds;
  Nlm_Int2        width;

  rb = NULL;
  if (prnt != NULL) {
    Nlm_SelectFont (Nlm_systemFont);
    width = Nlm_StringWidth (title);
#ifdef WIN_MAC
    vbounds = 2;
#endif
#ifdef WIN_MSWIN
    vbounds = 4;
#endif
#ifdef WIN_MOTIF
    vbounds = 2;
#endif
    rb = (Nlm_RepeaT) Nlm_AutonomousPanel (prnt, width+8, Nlm_stdLineHeight+vbounds*2,
                                           Nlm_DrawRepeat, NULL, NULL,
                                           sizeof (Nlm_RepeatData), Nlm_ResetRepeat,
                                           repeatProcs);
    if (rb != NULL) {
      Nlm_SetPanelClick ((Nlm_PaneL) rb, Nlm_RepeatClick, NULL,
                         Nlm_RepeatPress, Nlm_RepeatRelease);
      rdata.title = NULL;
      rdata.action = actn;
      Nlm_SetPanelExtra ((Nlm_PaneL) rb, &rdata);
      Nlm_SetRepeatTitle ((Nlm_GraphiC) rb, 0, title, FALSE);
    }
  }
  return rb;
}

static void Nlm_SetIconValue (Nlm_GraphiC i, Nlm_Int2 value, Nlm_Boolean savePort)

{
  Nlm_IconData  extra;
  Nlm_Int2      oldval;
  Nlm_RecT      r;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePortIfNeeded (i, savePort);
  if (i != NULL) {
    Nlm_GetPanelExtra ((Nlm_PaneL) i, &extra);
    oldval = extra.value;
    extra.value = value;
    Nlm_SetPanelExtra ((Nlm_PaneL) i, &extra);
    if (oldval != value) {
      if (Nlm_Visible (i) && Nlm_AllParentsVisible (i)) {
        Nlm_Select (i);
        if (extra.inval != NULL) {
          extra.inval ((Nlm_IcoN) i, value, oldval);
        } else {
          Nlm_ObjectRect (i, &r);
          Nlm_InsetRect (&r, -1, -1);
          Nlm_InvalRect (&r);
        }
      }
    }
  }
  Nlm_RestorePort (tempPort);
}

static Nlm_Int2 Nlm_GetIconValue (Nlm_GraphiC i)

{
  Nlm_IconData  extra;
  Nlm_Int2      value;

  value = 0;
  if (i != NULL) {
    Nlm_GetPanelExtra ((Nlm_PaneL) i, &extra);
    value = extra.value;
  }
  return value;
}

static void Nlm_SetIconStatus (Nlm_GraphiC i, Nlm_Int2 item,
                               Nlm_Boolean set, Nlm_Boolean savePort)

{
  Nlm_IconData  extra;
  Nlm_Boolean   oldstat;
  Nlm_RecT      r;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePortIfNeeded (i, savePort);
  if (i != NULL) {
    Nlm_GetPanelExtra ((Nlm_PaneL) i, &extra);
    oldstat = extra.status;
    extra.status = set;
    Nlm_SetPanelExtra ((Nlm_PaneL) i, &extra);
    if (oldstat != set) {
      if (Nlm_Visible (i) && Nlm_AllParentsVisible (i)) {
        Nlm_Select (i);
        if (extra.inval != NULL) {
          extra.inval ((Nlm_IcoN) i, extra.value, extra.value);
        } else {
          Nlm_ObjectRect (i, &r);
          Nlm_InsetRect (&r, -1, -1);
          Nlm_InvalRect (&r);
        }
      }
    }
  }
  Nlm_RestorePort (tempPort);
}

static Nlm_Boolean Nlm_GetIconStatus (Nlm_GraphiC i, Nlm_Int2 item)

{
  Nlm_IconData  extra;
  Nlm_Boolean   status;

  status = 0;
  if (i != NULL) {
    Nlm_GetPanelExtra ((Nlm_PaneL) i, &extra);
    status = extra.status;
  }
  return status;
}

static void Nlm_SetIconTitle (Nlm_GraphiC i, Nlm_Int2 item,
                              Nlm_CharPtr title, Nlm_Boolean savePort)

{
  Nlm_IconData  extra;
  Nlm_Handle    h;
  Nlm_RecT      r;
  Nlm_WindoW    tempPort;

  tempPort = Nlm_SavePortIfNeeded (i, savePort);
  Nlm_GetPanelExtra ((Nlm_PaneL) i, &extra);
  h = extra.title;
  h = Nlm_SetString (h, title);
  extra.title = h;
  Nlm_SetPanelExtra ((Nlm_PaneL) i, &extra);
  if (Nlm_GetVisible (i) && Nlm_GetAllParentsVisible (i)) {
    Nlm_GetRect (i, &r);
    Nlm_InvalRect (&r);
  }
  Nlm_RestorePort (tempPort);
}

static void Nlm_GetIconTitle (Nlm_GraphiC i, Nlm_Int2 item,
                              Nlm_CharPtr title, Nlm_sizeT maxsize)

{
  Nlm_IconData  extra;
  Nlm_Handle    h;

  if (title != NULL) {
    Nlm_GetPanelExtra ((Nlm_PaneL) i, &extra);
    h = extra.title;
    Nlm_GetString (h, title, maxsize);
  }
}

static void Nlm_ResetIcon (Nlm_PaneL i)

{
  Nlm_IconData  extra;

  Nlm_GetPanelExtra (i, &extra);
  if (extra.title != NULL) {
    extra.title = Nlm_HandFree (extra.title);
  }
  Nlm_SetPanelExtra (i, &extra);
}

extern Nlm_IcoN Nlm_IconButton (Nlm_GrouP prnt, Nlm_Int2 pixwidth, Nlm_Int2 pixheight,
                                Nlm_IcnActnProc draw, Nlm_IcnChngProc inval,
                                Nlm_IcnClckProc click, Nlm_IcnClckProc drag,
                                Nlm_IcnClckProc hold, Nlm_IcnClckProc release)

{
  Nlm_IcoN      ic;
  Nlm_IconData  idata;

  ic = NULL;
  if (prnt != NULL) {
    Nlm_SelectFont (Nlm_systemFont);
    ic = (Nlm_IcoN) Nlm_AutonomousPanel (prnt, pixwidth, pixheight,
                                         (Nlm_PnlActnProc) draw, NULL, NULL,
                                         sizeof (Nlm_IconData), Nlm_ResetIcon,
                                         iconProcs);
    if (ic != NULL) {
      Nlm_SetPanelClick ((Nlm_PaneL) ic, (Nlm_PnlClckProc) click, (Nlm_PnlClckProc) drag,
                         (Nlm_PnlClckProc) hold, (Nlm_PnlClckProc) release);
      idata.inval = inval;
      idata.value = 0;
      idata.status = FALSE;
      idata.title = NULL;
      Nlm_SetPanelExtra ((Nlm_PaneL) ic, &idata);
    }
  }
  return ic;
}

extern void Nlm_FreeExtras (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemFree (gphprcsptr);
}

extern void Nlm_InitExtras (void)

{
  gphprcsptr = (Nlm_GphPrcsPtr) Nlm_MemNew (sizeof (Nlm_GphPrcs) * 3);

  repeatProcs = &(gphprcsptr [0]);
  repeatProcs->setTitle = Nlm_SetRepeatTitle;
  repeatProcs->getTitle = Nlm_GetRepeatTitle;

  switchProcs = &(gphprcsptr [1]);
  switchProcs->setValue = Nlm_SetSwitchValue;
  switchProcs->getValue = Nlm_GetSwitchValue;

  iconProcs = &(gphprcsptr [2]);
  iconProcs->setValue = Nlm_SetIconValue;
  iconProcs->getValue = Nlm_GetIconValue;
  iconProcs->setStatus = Nlm_SetIconStatus;
  iconProcs->getStatus = Nlm_GetIconStatus;
  iconProcs->setTitle = Nlm_SetIconTitle;
  iconProcs->getTitle = Nlm_GetIconTitle;
}
