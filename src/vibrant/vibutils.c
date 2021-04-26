/*   vibutils.c
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
* File Name:  vibutils.c
*
* Author:  Jonathan Kans
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.44 $
*
* File Description: 
*       Vibrant miscellaneous functions
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

#ifdef WIN_MAC
#include <Printing.h>
#include <Processes.h>
#include <GestaltEqu.h>
/*#include <Strings.h> */
#endif

#ifdef WIN_MSWIN
#include <commdlg.h>
#endif

#ifdef WIN_MOTIF
#include <Xm/FileSB.h>
#ifdef OS_UNIX
#include <unistd.h>
#endif
#ifdef OS_VMS
#endif
#endif

#ifdef WIN_MAC
Nlm_Int2     Nlm_nextIdNumber = 2;
#endif
#ifdef WIN_MSWIN
Nlm_Int2     Nlm_nextIdNumber = 102;
#endif
#ifdef WIN_MOTIF
Nlm_Int2     Nlm_nextIdNumber = 1;
#endif

Nlm_PoinT    Nlm_globalMouse;
Nlm_PoinT    Nlm_localMouse;

Nlm_Int2     Nlm_hScrollBarHeight;
Nlm_Int2     Nlm_vScrollBarWidth;

Nlm_Int2     Nlm_dialogTextHeight;
Nlm_Int2     Nlm_popupMenuHeight;

Nlm_Char     Nlm_currentKey = '\0';

Nlm_Boolean  Nlm_cmmdKey = FALSE;
Nlm_Boolean  Nlm_ctrlKey = FALSE;
Nlm_Boolean  Nlm_optKey = FALSE;
Nlm_Boolean  Nlm_shftKey = FALSE;
Nlm_Boolean  Nlm_dblClick = FALSE;

static Nlm_WindoW       postWindow = NULL;
static Nlm_PrompT       postPrompt = NULL;

static Nlm_Boolean      errorBoxUp;
static Nlm_Int2         errorBoxRsult;

static Nlm_GraphiC      recentGraphic = NULL;
static Nlm_GraphicData  recentGraphicData;

static Nlm_BoX          recentBox = NULL;
static Nlm_BoxData      recentBoxData;

#ifdef WIN_MAC
static THPrint   prHdl = NULL;
static TPPrPort  prPort = NULL;
static Nlm_Int2  prerr;
static Nlm_Char  fileTypes [32] = {0};
#endif

#ifdef WIN_MSWIN
static Nlm_Boolean   abortPrint;
static HDC           hPr = NULL;
static Nlm_Int2      prerr;
static int           vibrant_disabled = 0;
static int           disabled_count = 0;
static PRINTDLG      pd;
static OPENFILENAME  ofn;
#endif

#ifdef WIN_MOTIF
static Widget       fileDialog = NULL;
static Nlm_Boolean  fileBoxUp;
static Nlm_Boolean  fileBoxRsult;
static Nlm_Char     filePath [256];
#endif

extern Nlm_Uint4 Nlm_StrngLen (Nlm_CharPtr string)

{
  Nlm_Uint4  len;

  len = 0;
  if (string != NULL) {
    while (*string != '\0') {
      string++;
      len++;
    }
  }
  return len;
}

extern void Nlm_StrngCat (Nlm_CharPtr dest, Nlm_CharPtr source, Nlm_sizeT maxsize)

{
  Nlm_Uint4  count;
  Nlm_Uint4  i;

  if (dest != NULL && source != NULL && maxsize > 0) {
    count = Nlm_StrngLen (source);
    i = Nlm_StrngLen (dest);
    if (count + i >= maxsize) {
      count = maxsize - i - 1;
    }
    dest += i;
    while (count > 0) {
      *dest = *source;
      dest++;
      source++;
      count--;
    }
    *dest = '\0';
  }
}

extern void Nlm_StrngCpy (Nlm_CharPtr dest, Nlm_CharPtr source, Nlm_sizeT maxsize)

{
  Nlm_Uint4  count;

  if (dest != NULL && maxsize > 0) {
    count = Nlm_StrngLen (source);
    if (count >= maxsize) {
      count = maxsize - 1;
    }
    while (count > 0) {
      *dest = *source;
      dest++;
      source++;
      count--;
    }
    *dest = '\0';
  }
}

static Nlm_Boolean Nlm_InNumber (Nlm_Char ch)

{
  return (Nlm_Boolean) (ch >= '0' && ch <= '9') ;
}

static Nlm_Char Nlm_Cap (Nlm_Char ch, Nlm_Boolean caseCounts)

{
  if (caseCounts) {
    return ch;
  } else if (ch >= 'a' && ch <= 'z') {
    return (Nlm_Char) (ch - ' ');
  } else {
    return ch;
  }
}

extern Nlm_Boolean Nlm_StrngPos (Nlm_CharPtr str, Nlm_CharPtr sub,
                                 Nlm_Uint4 start, Nlm_Boolean caseCounts,
                                 Nlm_Uint4Ptr match)

{
  Nlm_Char     ch1;
  Nlm_Char     ch2;
  Nlm_Uint4    count;
  Nlm_Boolean  found;
  Nlm_Uint4    i;
  Nlm_Uint4    j;
  Nlm_Uint4    len;
  Nlm_Uint4    max;
  Nlm_Uint4    sublen;

  found = FALSE;
  if (str != NULL && sub != NULL) {
    len = Nlm_StrngLen (str);
    sublen = Nlm_StrngLen (sub);
    if (len < sublen) {
      sublen = len;
    }
    max = len - sublen;
    i = start;
    if (match != NULL) {
      *match = 0;
    }
    if (start < len) {
      do {
        found = TRUE;
        j = 0;
        count = sublen;
        while (found && count > 0) {
          ch1 = Nlm_Cap (str [i + j], caseCounts);
          ch2 = Nlm_Cap (sub [j], caseCounts);
          if (ch1 != ch2) {
            found = FALSE;
          }
          j++;
          count--;
        }
        if (found && match != NULL) {
          *match = i;
        }
        i++;
      } while ((! found) && (i <= max));
    }
    else {
      found = FALSE;
    }
  }
  return found;
}

extern void Nlm_StrngSeg (Nlm_CharPtr dest, Nlm_CharPtr source,
                          Nlm_Uint4 start, Nlm_Uint4 length,
                          Nlm_sizeT maxsize)

{
  Nlm_Uint4  count;
  Nlm_Uint4  len;

  if (dest != NULL && source != NULL && maxsize > 0) {
    count = length;
    len = Nlm_StrngLen (source);
    if (start + length > len) {
      count = len - start;
    }
    if (count >= maxsize) {
      count = maxsize - 1;
    }
    source += start;
    while (count > 0) {
      *dest = *source;
      dest++;
      source++;
      count--;
    }
    *dest = '\0';
  }
}

extern void Nlm_StrngRep (Nlm_CharPtr dest, Nlm_CharPtr source,
                          Nlm_Uint4 start, Nlm_sizeT maxsize)

{
  Nlm_Uint4  count;

  if (dest != NULL && source != NULL && maxsize > 0) {
    count = Nlm_StrngLen (source);
    if (count + start >= maxsize) {
      count = maxsize - start - 1;
    }
    dest += start;
    while (count > 0) {
      *dest = *source;
      dest++;
      source++;
      count--;
    }
  }
}

extern Nlm_Boolean Nlm_StrngEql (Nlm_CharPtr str1, Nlm_CharPtr str2,
                                 Nlm_Boolean caseCounts)

{
  return (Nlm_Boolean) (Nlm_StrngCmp (str1, str2, caseCounts) == 0);
}

extern Nlm_Int2 Nlm_StrngCmp (Nlm_CharPtr str1, Nlm_CharPtr str2,
                              Nlm_Boolean caseCounts)

{
  Nlm_Uint4  count;
  Nlm_Uint4  i;
  Nlm_Uint4  len1;
  Nlm_Uint4  len2;
  Nlm_Int2   rsult;

  rsult = 0;
  if (str1 != NULL && str2 != NULL) {
    len1 = Nlm_StrngLen (str1);
    len2 = Nlm_StrngLen (str2);
    if (len1 > len2) {
      count = len2;
    } else {
      count = len1;
    }
    i = 0;
    while (count > 0 && Nlm_Cap (str1 [i], caseCounts) == Nlm_Cap (str2 [i], caseCounts)) {
      i++;
      count--;
    }
    if (count > 0) {
      if (Nlm_Cap (str1 [i], caseCounts) > Nlm_Cap (str2 [i], caseCounts)) {
        rsult = 1;
      } else {
        rsult = -1;
      }
    } else {
      if (len1 > len2) {
        rsult = 1;
      } else if (len1 < len2) {
        rsult = -1;
      } else {
        rsult = 0;
      }
    }
  }
  return rsult;
}

extern Nlm_Int2 Nlm_SymblCmp (Nlm_CharPtr str1, Nlm_CharPtr str2,
                              Nlm_Boolean caseCounts)

{
  Nlm_Boolean  cont;
  Nlm_Boolean  done;
  Nlm_Uint4    i;
  Nlm_Uint4    j;
  Nlm_Uint4    len1;
  Nlm_Uint4    len2;
  Nlm_Uint4    num1;
  Nlm_Uint4    num2;
  Nlm_Int2     rsult;

  rsult = 0;
  if (str1 != NULL && str2 != NULL) {
    done = FALSE;
    len1 = Nlm_StrngLen (str1);
    len2 = Nlm_StrngLen (str2);
    i = 0;
    j = 0;
    cont = (Nlm_Boolean) (len1 > 0 && len2 > 0);
    while (cont) {
      if (Nlm_InNumber (str1 [i]) && Nlm_InNumber (str2 [j])) {
        num1 = 0;
        while (i < len1 && Nlm_InNumber (str1 [i]) && (num1 < 429496729)) {
          num1 *= 10;
          num1 += (str1 [i] - '0');
          i++;
        }
        num2 = 0;
        while (j < len2 && Nlm_InNumber (str2 [j]) && (num2 < 429496729)) {
          num2 *= 10;
          num2 += (str2 [j] - '0');
          j++;
        }
        if (num1 > num2) {
          rsult = 1;
          done = TRUE;
          cont = FALSE;
        } else if (num1 < num2) {
          rsult = -1;
          done = TRUE;
          cont = FALSE;
        } else {
          cont = (Nlm_Boolean) (i < len1 && j < len2);
        }
      } else if (Nlm_Cap (str1 [i], caseCounts) > Nlm_Cap (str2 [j], caseCounts)) {
        rsult = 1;
        done = TRUE;
        cont = FALSE;
      } else if (Nlm_Cap (str1 [i], caseCounts) < Nlm_Cap (str2 [j], caseCounts)) {
        rsult = -1;
        done = TRUE;
        cont = FALSE;
      } else {
        i++;
        j++;
        cont = (Nlm_Boolean) (i < len1 && j < len2);
      }
    }
    if (done) {
    } else if (i < len1 && j == len2) {
      rsult = 1;
    } else if (i == len1 && j < len2) {
      rsult = -1;
    } else {
      rsult = 0;
    }
  }
  return rsult;
}

extern Nlm_Handle Nlm_SetString (Nlm_Handle h, Nlm_CharPtr str)

{
  Nlm_sizeT       len;
  Nlm_CharPtr  pp;

  if (str != NULL) {
    len = Nlm_StringLen (str);
    if (len > 0) {
      if (h != NULL) {
        h = Nlm_HandMore (h, len + 1);
      } else {
        h = Nlm_HandNew (len + 1);
      }
    } else if (h != NULL) {
      Nlm_HandFree (h);
      h = NULL;
    }
  } else {
    Nlm_HandFree (h);
    h = NULL;
  }
  if (h != NULL) {
    pp = (Nlm_CharPtr) Nlm_HandLock (h);
    if (pp != NULL) {
      Nlm_StringNCpy (pp, str, len + 1);
    }
    Nlm_HandUnlock (h);
  }
  return h;
}

extern void Nlm_GetString (Nlm_Handle h, Nlm_CharPtr str, Nlm_sizeT maxsize)

{
  Nlm_CharPtr  pp;

  if (str != NULL) {
    if (h != NULL) {
      pp = (Nlm_CharPtr) Nlm_HandLock (h);
      if (pp != NULL) {
        Nlm_StringNCpy (str, pp, maxsize);
      }
      Nlm_HandUnlock (h);
    } else {
      Nlm_StringNCpy (str, "", maxsize);
    }
  }
}

static void Nlm_MssgErrorProc (Nlm_ButtoN b)

{
  errorBoxUp = FALSE;
  errorBoxRsult = ANS_NO;
}

static void Nlm_MssgFatalProc (Nlm_ButtoN b)

{
  errorBoxUp = FALSE;
  errorBoxRsult = ANS_NO;
}

static void Nlm_MssgNoProc (Nlm_ButtoN b)

{
  errorBoxUp = FALSE;
  errorBoxRsult = ANS_NO;
}

static void Nlm_MssgYesProc (Nlm_ButtoN b)

{
  errorBoxUp = FALSE;
  errorBoxRsult = ANS_YES;
}

static void Nlm_MssgOkayProc (Nlm_ButtoN b)

{
  errorBoxUp = FALSE;
  errorBoxRsult = ANS_OK;
}

static void Nlm_MssgRetryProc (Nlm_ButtoN b)

{
  errorBoxUp = FALSE;
  errorBoxRsult = ANS_RETRY;
}

static void Nlm_MssgAbortProc (Nlm_ButtoN b)

{
  errorBoxUp = FALSE;
  errorBoxRsult = ANS_ABORT;
}

static void Nlm_MssgCancelProc (Nlm_ButtoN b)

{
  errorBoxUp = FALSE;
  errorBoxRsult = ANS_CANCEL;
}

static void Nlm_MssgIgnoreProc (Nlm_ButtoN b)

{
  errorBoxUp = FALSE;
  errorBoxRsult = ANS_IGNORE;
}

static void Nlm_ClosePostProc (Nlm_WindoW w)

{
  Nlm_Hide (w);
}

static MsgAnswer LIBCALLBACK Nlm_VibMessageHook (MsgKey key, ErrSev severity,
                                                 const char * caption, const char * message)

{
  Nlm_CharPtr  buf;
  Nlm_Int2     answer;
  Nlm_sizeT       len;
  Nlm_Int2     rsult;
  Nlm_WindoW   tempPort;
  Nlm_Int2     wtype;
#ifdef WIN_MAC
  Nlm_ButtoN   b [3];
  Nlm_Char     ch;
  void         *data;
  Nlm_Int2     delta;
  Nlm_Boolean  fatal;
  Nlm_GrouP    g;
  Nlm_Int2     i;
  Nlm_Int2     j;
  Nlm_Int2     margin;
  Nlm_Int2     maxWidth;
  Nlm_PoinT    npt;
  Nlm_Int2     percent;
  Nlm_RecT     r;
  Nlm_Int2     rgt;
  Nlm_WindoW   w;
  Nlm_Int2     width;
#endif
#ifdef WIN_MOTIF
  Nlm_ButtoN   b [3];
  Nlm_Char     ch;
  void         *data;
  Nlm_Int2     delta;
  Nlm_Boolean  fatal;
  Nlm_GrouP    g;
  Nlm_Int2     i;
  Nlm_Int2     j;
  Nlm_Int2     margin;
  Nlm_Int2     maxWidth;
  Nlm_PoinT    npt;
  Nlm_Int2     percent;
  Nlm_RecT     r;
  Nlm_Int2     rgt;
  Nlm_WindoW   w;
  Nlm_Int2     width;
#endif

  rsult = 0;
  len = MIN (Nlm_StringLen (message), (Nlm_sizeT) 4094);
  buf = (Nlm_CharPtr) Nlm_MemNew (len + 2);
  Nlm_StringNCpy (buf, message, len);
  if (key == KEY_NONE && severity == SEV_INFO) {
    tempPort = Nlm_CurrentWindow ();
    if (postWindow == NULL) {
      postWindow = Nlm_DocumentWindow (-50, -90, -20, -20, "Message",
                                       Nlm_ClosePostProc, NULL);
      postPrompt = Nlm_StaticPrompt (postWindow, "", 30 * Nlm_stdCharWidth,
                                     Nlm_stdLineHeight, Nlm_systemFont, 'c');
    }
    if (postWindow != NULL && postPrompt != NULL) {
      Nlm_SetTitle (postPrompt, buf);
      if (! Nlm_Visible (postWindow)) {
        Nlm_Show (postWindow);
      }
      Nlm_Select (postWindow);
    }
    Nlm_RestorePort (tempPort);
    return (MsgAnswer) 0;
  }
#ifndef WIN_MSWIN
  data = Nlm_GetAppProperty ("VibrantMessageWidthMax");
  if (data != NULL) {
    percent = (Nlm_Int2) data;
    if (percent < 0) {
      percent = 100;
    }
  } else {
    percent = 100;
  }
  percent = MAX (percent, 25);
  percent = MIN (percent, 95);
  maxWidth = (Nlm_Int2) ((Nlm_Int4) (Nlm_screenRect.right - Nlm_screenRect.left) *
              (Nlm_Int4) percent / (Nlm_Int4) 100) - 40;
  maxWidth = MAX (maxWidth, 100);
#endif
#ifdef WIN_MAC
  w = Nlm_ModalWindow (-50, -20, -20, -20, NULL);
  g = Nlm_HiddenGroup (w, 0, 10, NULL);
  Nlm_GetNextPosition (g, &npt);
  npt.x += 6;
  Nlm_SetNextPosition (g, npt);
  i = 0;
  while (Nlm_StringLen (buf + i) > 0) {
    width = 0;
    j = 0;
    while (buf [i + j] == ' ') {
      i++;
    }
    ch = buf [i + j];
    width += Nlm_CharWidth (ch);
    while (ch != '\0' && ch != '\n' && ch != '\r' && width <= maxWidth) {
      j++;
      ch = buf [i + j];
      width += Nlm_CharWidth (ch);
    }
    if (width > maxWidth) {
      ch = buf [i + j];
      while (j > 0 && ch != ' ' && ch != '-') {
        j--;
        ch = buf [i + j];
      }
    }
    if (ch == '\n' || ch == '\r') {
      buf [i + j] = '\0';
      Nlm_StaticPrompt (g, buf + i, 0, 0, Nlm_systemFont, 'l');
      i += j + 1;
    } else {
      buf [i + j] = '\0';
      Nlm_StaticPrompt (g, buf + i, 0, 0, Nlm_systemFont, 'l');
      buf [i + j] = ch;
      i += j;
    }
  }
  Nlm_GetPosition (g, &r);
  margin = r.right;
  Nlm_Break ((Nlm_GraphiC) w);
  b [0] = NULL;
  b [1] = NULL;
  b [2] = NULL;
  switch (key) {
    case KEY_OK:
      if (severity == SEV_ERROR) {
        b [0] = Nlm_PushButton (w, "OK", Nlm_MssgErrorProc);
      } else if (severity == SEV_FATAL) {
        b [0] = Nlm_PushButton (w, "OK", Nlm_MssgFatalProc);
      } else {
        b [0] = Nlm_PushButton (w, "OK", Nlm_MssgOkayProc);
      }
      break;
    case KEY_RC:
      b [0] = Nlm_PushButton (w, "Retry", Nlm_MssgRetryProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [1] = Nlm_PushButton (w, "Cancel", Nlm_MssgCancelProc);
      break;
    case KEY_ARI:
      b [0] = Nlm_PushButton (w, "Abort", Nlm_MssgAbortProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [1] = Nlm_PushButton (w, "Retry", Nlm_MssgRetryProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [2] = Nlm_PushButton (w, "Ignore", Nlm_MssgIgnoreProc);
      break;
    case KEY_YN:
      b [0] = Nlm_PushButton (w, "Yes", Nlm_MssgYesProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [1] = Nlm_PushButton (w, "No", Nlm_MssgNoProc);
      break;
    case KEY_YNC:
      b [0] = Nlm_PushButton (w, "Yes", Nlm_MssgYesProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [1] = Nlm_PushButton (w, "No", Nlm_MssgNoProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [2] = Nlm_PushButton (w, "Cancel", Nlm_MssgCancelProc);
      break;
    case KEY_OKC:
      b [1] = Nlm_PushButton (w, "OK", Nlm_MssgOkayProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [2] = Nlm_PushButton (w, "Cancel", Nlm_MssgCancelProc);
      break;
    case KEY_NONE:
      b [0] = Nlm_PushButton (w, "OK", Nlm_MssgOkayProc);
      break;
    default:
      b [0] = Nlm_PushButton (w, "OK", Nlm_MssgOkayProc);
      break;
  }
  i = 2;
  while (i >= 0 && b [i] == NULL) {
    i--;
  }
  if (i >= 0) {
    Nlm_GetPosition (b [i], &r);
    delta = (margin - r.right) / 2;
    if (delta > 0) {
      while (i >= 0) {
        Nlm_GetPosition (b [i], &r);
        Nlm_OffsetRect (&r, delta, 0);
        Nlm_SetPosition (b [i], &r);
        i--;
      }
    } else if (delta < 0) {
      Nlm_GetPosition (g, &r);
      Nlm_OffsetRect (&r, -delta, 0);
      Nlm_SetPosition (g, &r);
    }
  }
  Nlm_DoShow ((Nlm_GraphiC) w, TRUE, TRUE);
  Nlm_ArrowCursor ();
  errorBoxUp = TRUE;
  errorBoxRsult = 0;
  while (errorBoxUp) {
    Nlm_ProcessExternalEvent ();
  }
  Nlm_ProcessAnEvent ();
  Nlm_DoRemove ((Nlm_GraphiC) w, TRUE);
  rsult = errorBoxRsult;
#endif
#ifdef WIN_MSWIN
  wtype = MB_OK;
  switch (key) {
    case KEY_OK:
      if (severity == SEV_ERROR) {
        wtype = MB_OK;
      } else if (severity == SEV_FATAL) {
        wtype = MB_OK;
      } else {
        wtype = MB_OK;
      }
      break;
    case KEY_RC:
      wtype = MB_RETRYCANCEL;
      break;
    case KEY_ARI:
      wtype = MB_ABORTRETRYIGNORE;
      break;
    case KEY_YN:
      wtype = MB_YESNO;
      break;
    case KEY_YNC:
      wtype = MB_YESNOCANCEL;
      break;
    case KEY_OKC:
      wtype = MB_OKCANCEL;
      break;
    case KEY_NONE:
      wtype = MB_OK;
      break;
    default:
      break;
  }
  answer = MessageBox (Nlm_currentHWnd, buf, "", wtype);
  switch (answer) {
    case IDNO:
      rsult = ANS_NO;
      break;
    case IDYES:
      rsult = ANS_YES;
      break;
    case IDOK:
      rsult = ANS_OK;
      break;
    case IDRETRY:
      rsult = ANS_RETRY;
      break;
    case IDABORT:
      rsult = ANS_ABORT;
      break;
    case IDCANCEL:
      rsult = ANS_CANCEL;
      break;
    case IDIGNORE:
      rsult = ANS_IGNORE;
      break;
    default:
      break;
  }
#endif
#ifdef WIN_MOTIF
  w = Nlm_ModalWindow (-50, -20, -20, -20, NULL);
  g = Nlm_HiddenGroup (w, 0, 10, NULL);
  Nlm_GetNextPosition (g, &npt);
  npt.x += 6;
  Nlm_SetNextPosition (g, npt);
  i = 0;
  while (Nlm_StringLen (buf + i) > 0) {
    width = 0;
    j = 0;
    while (buf [i + j] == ' ') {
      i++;
    }
    ch = buf [i + j];
    width += Nlm_CharWidth (ch);
    while (ch != '\0' && ch != '\n' && ch != '\r' && width <= maxWidth) {
      j++;
      ch = buf [i + j];
      width += Nlm_CharWidth (ch);
    }
    if (width > maxWidth) {
      ch = buf [i + j];
      while (j > 0 && ch != ' ' && ch != '-') {
        j--;
        ch = buf [i + j];
      }
    }
    if (ch == '\n' || ch == '\r') {
      buf [i + j] = '\0';
      Nlm_StaticPrompt (g, buf + i, 0, 0, Nlm_systemFont, 'l');
      i += j + 1;
    } else {
      buf [i + j] = '\0';
      Nlm_StaticPrompt (g, buf + i, 0, 0, Nlm_systemFont, 'l');
      buf [i + j] = ch;
      i += j;
    }
  }
  Nlm_GetPosition (g, &r);
  margin = r.right;
  Nlm_Break ((Nlm_GraphiC) w);
  b [0] = NULL;
  b [1] = NULL;
  b [2] = NULL;
  switch (key) {
    case KEY_OK:
      if (severity == SEV_ERROR) {
        b [0] = Nlm_PushButton (w, "OK", Nlm_MssgErrorProc);
      } else if (severity == SEV_FATAL) {
        b [0] = Nlm_PushButton (w, "OK", Nlm_MssgFatalProc);
      } else {
        b [0] = Nlm_PushButton (w, "OK", Nlm_MssgOkayProc);
      }
      break;
    case KEY_RC:
      b [0] = Nlm_PushButton (w, "Retry", Nlm_MssgRetryProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [1] = Nlm_PushButton (w, "Cancel", Nlm_MssgCancelProc);
      break;
    case KEY_ARI:
      b [0] = Nlm_PushButton (w, "Abort", Nlm_MssgAbortProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [1] = Nlm_PushButton (w, "Retry", Nlm_MssgRetryProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [2] = Nlm_PushButton (w, "Ignore", Nlm_MssgIgnoreProc);
      break;
    case KEY_YN:
      b [0] = Nlm_PushButton (w, "Yes", Nlm_MssgYesProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [1] = Nlm_PushButton (w, "No", Nlm_MssgNoProc);
      break;
    case KEY_YNC:
      b [0] = Nlm_PushButton (w, "Yes", Nlm_MssgYesProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [1] = Nlm_PushButton (w, "No", Nlm_MssgNoProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [2] = Nlm_PushButton (w, "Cancel", Nlm_MssgCancelProc);
      break;
    case KEY_OKC:
      b [1] = Nlm_PushButton (w, "OK", Nlm_MssgOkayProc);
      Nlm_Advance ((Nlm_GraphiC) w);
      b [2] = Nlm_PushButton (w, "Cancel", Nlm_MssgCancelProc);
      break;
    case KEY_NONE:
      b [0] = Nlm_PushButton (w, "OK", Nlm_MssgOkayProc);
      break;
    default:
      b [0] = Nlm_PushButton (w, "OK", Nlm_MssgOkayProc);
      break;
  }
  i = 2;
  while (i >= 0 && b [i] == NULL) {
    i--;
  }
  if (i >= 0) {
    Nlm_GetPosition (b [i], &r);
    delta = (margin - r.right) / 2;
    if (delta > 0) {
      while (i >= 0) {
        Nlm_GetPosition (b [i], &r);
        Nlm_OffsetRect (&r, delta, 0);
        Nlm_SetPosition (b [i], &r);
        i--;
      }
    } else if (delta < 0) {
      Nlm_GetPosition (g, &r);
      Nlm_OffsetRect (&r, -delta, 0);
      Nlm_SetPosition (g, &r);
    }
  }
  Nlm_DoShow ((Nlm_GraphiC) w, TRUE, TRUE);
  errorBoxUp = TRUE;
  errorBoxRsult = 0;
  while (errorBoxUp) {
    Nlm_ProcessAnEvent ();
  }
  Nlm_DoRemove ((Nlm_GraphiC) w, TRUE);
  rsult = errorBoxRsult;
#endif
  Nlm_MemFree (buf);
  return (MsgAnswer) rsult;
}

typedef struct monitextra {
  Nlm_WindoW   wind;
  Nlm_PaneL    pnl;
} Nlm_VibMonExtra, PNTR Nlm_VibMonPtr;

static void Nlm_SelectMonitor (Nlm_WindoW w)

{
  Nlm_WindowTool  wptr;
#ifdef WIN_MOTIF
  Nlm_ShellTool   shl;
#endif

  wptr = Nlm_ParentWindowPtr ((Nlm_GraphiC) w);
#ifdef WIN_MAC
  SelectWindow (wptr);
  SetPort (wptr);
  Nlm_SetUpdateRegion (wptr);
  Nlm_ResetDrawingTools ();
#endif
#ifdef WIN_MSWIN
  BringWindowToTop (wptr);
  Nlm_currentHDC = Nlm_ParentWindowPort ((Nlm_GraphiC) w);
  Nlm_currentHWnd = wptr;
#endif
#ifdef WIN_MOTIF
  if (Nlm_currentXDisplay != NULL) {
    shl = Nlm_ParentWindowShell ((Nlm_GraphiC) w);
    XMapRaised (Nlm_currentXDisplay, XtWindow (shl));
  }
#endif
  Nlm_currentWindowTool = wptr;
}

static int Nlm_VibMonStrValue (Nlm_MonitorPtr mon)

{
  Nlm_VibMonPtr  vmp;
  Nlm_PaneL      pnl;
  Nlm_RecT       r;
  Nlm_WindoW     tempPort;
  Nlm_WindoW     wind;
#ifdef WIN_MOTIF
  Nlm_Uint4      tempBackColor;
  Nlm_Uint4      tempForeColor;
  Nlm_Int2       tempXOffset;
  Nlm_Int2       tempYOffset;
  Window         tempXWind;
#endif

  if (mon == NULL || mon->type != MonType_Str || mon->extra == NULL) {
    return 0;
  }
  vmp = (Nlm_VibMonPtr) mon->extra;
  pnl = vmp->pnl;
  wind = vmp->wind;
  tempPort = Nlm_CurrentWindow ();
#ifdef WIN_MOTIF
  tempXWind = Nlm_currentXWindow;
  tempBackColor = Nlm_XbackColor;
  tempForeColor = Nlm_XforeColor;
  tempXOffset = Nlm_XOffset;
  tempYOffset = Nlm_YOffset;
#endif
  Nlm_UseWindow (wind);
  Nlm_SelectMonitor (wind);
  Nlm_Select (pnl);
  Nlm_GetRect ((Nlm_GraphiC) pnl, &r);
  Nlm_InsetRect (&r, 2, 2);
  Nlm_DrawString (&r, (Nlm_CharPtr) mon->strValue, 'l', FALSE);
  Nlm_UseWindow (tempPort);
#ifdef WIN_MOTIF
  Nlm_currentXWindow = tempXWind;
  Nlm_XbackColor = tempBackColor;
  Nlm_XforeColor = tempForeColor;
  Nlm_XOffset = tempXOffset;
  Nlm_YOffset = tempYOffset;
  if (Nlm_currentXDisplay != NULL) {
    XSync (Nlm_currentXDisplay, FALSE);
  }
#endif
  return 0;
}

static int Nlm_VibMonIntValue (Nlm_MonitorPtr mon)

{
  Nlm_Int4       from;
  Nlm_VibMonPtr  vmp;
  Nlm_PaneL      pnl;
  Nlm_RecT       r;
  Nlm_Int4       range;
  Nlm_Int2       right;
  Nlm_WindoW     tempPort;
  Nlm_Int4       to;
  Nlm_Int4       value;
  Nlm_Int4       width;
  Nlm_WindoW     wind;
#ifdef WIN_MOTIF
  Nlm_Uint4      tempBackColor;
  Nlm_Uint4      tempForeColor;
  Nlm_Int2       tempXOffset;
  Nlm_Int2       tempYOffset;
  Window         tempXWind;
#endif

  if (mon == NULL || mon->type != MonType_Int || mon->extra == NULL) {
    return 0;
  }
  vmp = (Nlm_VibMonPtr) mon->extra;
  from = MIN (mon->num1, mon->num2);
  to = MAX (mon->num1, mon->num2);
  range = to - from;
  value = mon->intValue;
  value = MAX (value, from);
  value = MIN (value, to);
  value -= from;
  pnl = vmp->pnl;
  wind = vmp->wind;
  tempPort = Nlm_CurrentWindow ();
#ifdef WIN_MOTIF
  tempXWind = Nlm_currentXWindow;
  tempBackColor = Nlm_XbackColor;
  tempForeColor = Nlm_XforeColor;
  tempXOffset = Nlm_XOffset;
  tempYOffset = Nlm_YOffset;
#endif
  Nlm_UseWindow (wind);
  Nlm_SelectMonitor (wind);
  Nlm_Select (pnl);
  Nlm_GetRect ((Nlm_GraphiC) pnl, &r);
  Nlm_FrameRect (&r);
  Nlm_InsetRect (&r, 2, 2);
  value = MAX (value, 0);
  value = MIN (value, range);
  width = (Nlm_Int4) (r.right - r.left);
  right = r.right;
  r.right = r.left + (Nlm_Int2) (width * value / range);
#ifdef DCLAP
#ifdef WIN_MOTIF
	/* my motif aint' showing any progress bar... */
  Nlm_Black();
#endif
#endif

  Nlm_PaintRect (&r);
  r.left = r.right;
  r.right = right;
  Nlm_EraseRect (&r);
  Nlm_UseWindow (tempPort);
#ifdef WIN_MOTIF
  Nlm_currentXWindow = tempXWind;
  Nlm_XbackColor = tempBackColor;
  Nlm_XforeColor = tempForeColor;
  Nlm_XOffset = tempXOffset;
  Nlm_YOffset = tempYOffset;
  if (Nlm_currentXDisplay != NULL) {
    XSync (Nlm_currentXDisplay, FALSE);
  }
#endif
  return 0;
}

static int Nlm_VibMonCreate (Nlm_MonitorPtr mon)

{
  Nlm_VibMonPtr  vmp;
  Nlm_PaneL      pnl;
  Nlm_WindoW     wind;

  if (mon != NULL) {
    vmp = (Nlm_VibMonPtr) Nlm_MemNew (sizeof (Nlm_VibMonExtra));
    mon->extra = (Nlm_VoidPtr) vmp;
    if (vmp != NULL) {
      wind = Nlm_FixedWindow (-50, -90, -10, -10, (Nlm_CharPtr) mon->strTitle, NULL);
      if (mon->type == MonType_Int) {
        pnl = Nlm_SimplePanel (wind, 200, 20, NULL);
      } else if (mon->type == MonType_Str) {
        pnl = Nlm_SimplePanel (wind, Nlm_stdCharWidth * (Nlm_Int2) mon->num1 + 6,
                               Nlm_stdLineHeight + 4, NULL);
      } else {
        pnl = Nlm_SimplePanel (wind, 200, 20, NULL);
      }
      vmp->wind = wind;
      vmp->pnl = pnl;
      Nlm_Show (wind);
    }
  }
  return TRUE;
}

static int Nlm_VibMonDestroy (Nlm_MonitorPtr mon)

{
  Nlm_VibMonPtr  vmp;

  if (mon != NULL) {
    vmp = (Nlm_VibMonPtr) mon->extra;
    if (vmp != NULL) {
      Nlm_Remove (vmp->wind);
      Nlm_MemFree (vmp);
    }
  }
  return 0;
}

static int LIBCALLBACK Nlm_VibMonitorHook (Nlm_MonitorPtr mon, MonCode code)

{
  switch (code) {
    case MonCode_Create :
      return Nlm_VibMonCreate (mon);
      break;
    case MonCode_Destroy :
      return Nlm_VibMonDestroy (mon);
      break;
    case MonCode_IntValue :
      return Nlm_VibMonIntValue (mon);
      break;
    case MonCode_StrValue :
      return Nlm_VibMonStrValue (mon);
      break;
    default :
      break;
  }
  return 0;
}

static void LIBCALLBACK Nlm_VibBeepHook (void)

{
#ifdef OS_MAC
  SysBeep (60);
#endif
#ifdef WIN_MSWIN
  MessageBeep (0);
#endif
#ifdef WIN_MOTIF
  if (Nlm_currentXDisplay != NULL) {
    XBell (Nlm_currentXDisplay, 0);
  }
#endif
}

extern void Nlm_InitVibrantHooks (void)

{
  Nlm_SetBeepHook (Nlm_VibBeepHook);
  Nlm_SetMonitorHook (Nlm_VibMonitorHook);
  Nlm_SetMessageHook (Nlm_VibMessageHook);
}

extern void Nlm_MousePosition (Nlm_PointPtr pt)

{
#ifdef WIN_MAC
  Nlm_PointTool  ptool;

  GetMouse (&ptool);
  if (pt != NULL) {
    Nlm_PointToolToPoinT (ptool, pt);
  }
#endif
#ifdef WIN_MSWIN
  Nlm_PointTool  ptool;

  if (pt != NULL && Nlm_currentHWnd != NULL) {
    GetCursorPos (&ptool);
    ScreenToClient (Nlm_currentHWnd, &ptool);
    Nlm_PointToolToPoinT (ptool, pt);
  }
#endif
#ifdef WIN_MOTIF
#endif
}

extern Nlm_Boolean Nlm_MouseButton (void)

{
#ifdef WIN_MAC
  return (Button ());
#endif
#ifdef WIN_MSWIN
  return (Nlm_Boolean) ((GetAsyncKeyState (VK_LBUTTON) & 0x8000) != 0);
#endif
#ifdef WIN_MOTIF
  return FALSE;
#endif
}

extern Nlm_Int4 Nlm_ComputerTime (void)

{
#ifdef WIN_MAC
  return (TickCount ());
#endif
#ifdef WIN_MSWIN
  return (GetCurrentTime ());
#endif
#ifdef WIN_MOTIF
#endif
}

extern void Nlm_Version (Nlm_CharPtr vsn, Nlm_sizeT maxsize)

{
  Nlm_StringNCpy (vsn, "NCBI VIBRANT Version 0.8 (8 May 91)", maxsize);
}

extern void Nlm_Advance (Nlm_Handle a)

{
  Nlm_BoxData  bdata;

  if (a != NULL) {
    Nlm_GetBoxData ((Nlm_BoX) a, &bdata);
    bdata.nextPoint.y = bdata.topRow;
    bdata.nextPoint.x = bdata.nextCol;
    Nlm_SetBoxData ((Nlm_BoX) a, &bdata);
  }
}

extern void Nlm_Break (Nlm_Handle a)

{
  Nlm_BoxData  bdata;

  if (a != NULL) {
    Nlm_GetBoxData ((Nlm_BoX) a, &bdata);
    bdata.topRow = bdata.limitPoint.y + bdata.ySpacing;
    bdata.nextCol = bdata.resetPoint.x;
    bdata.nextPoint.y = bdata.topRow;
    bdata.nextPoint.x = bdata.nextCol;
    Nlm_SetBoxData ((Nlm_BoX) a, &bdata);
  }
}

extern void Nlm_RecordRect (Nlm_GraphiC a, Nlm_RectPtr r)

{
  Nlm_BoxData  bdata;
  Nlm_Int2     nc;
  Nlm_Int2     nr;

  if (a != NULL && r != NULL) {
    Nlm_GetBoxData ((Nlm_BoX) a, &bdata);
    nr = r->bottom;
    if (nr > bdata.limitPoint.y) {
      bdata.limitPoint.y = nr;
    }
    nc = r->right;
    if (nc > bdata.limitPoint.x) {
      bdata.limitPoint.x = nc;
    }
    nc = r->right + bdata.xSpacing;
    if (nc > bdata.nextCol) {
      bdata.nextCol = nc;
    }
    Nlm_SetBoxData ((Nlm_BoX) a, &bdata);
  }
}

extern void Nlm_NextPosition (Nlm_GraphiC a, Nlm_RectPtr r)

{
  Nlm_BoxData  bdata;

  if (a != NULL && r != NULL) {
    Nlm_GetBoxData ((Nlm_BoX) a, &bdata);
    bdata.nextPoint.y = r->bottom + bdata.ySpacing;
    Nlm_SetBoxData ((Nlm_BoX) a, &bdata);
  }
}

extern void Nlm_SetNextPosition (Nlm_Handle a, Nlm_PoinT nps)

{
  Nlm_BoxData  bdata;

  if (a != NULL) {
    Nlm_GetBoxData ((Nlm_BoX) a, &bdata);
    bdata.nextPoint = nps;
    bdata.topRow = nps.y;
    Nlm_SetBoxData ((Nlm_BoX) a, &bdata);
  }
}

extern void Nlm_GetNextPosition (Nlm_Handle a, Nlm_PointPtr nps)

{
  Nlm_BoxData  bdata;

  if (a != NULL && nps != NULL) {
    Nlm_GetBoxData ((Nlm_BoX) a, &bdata);
    *nps = bdata.nextPoint;
  }
}

extern Nlm_GphPrcsPtr Nlm_GetClassPtr (Nlm_GraphiC a)

{
  Nlm_GphPrcsPtr   classPtr;
  Nlm_GraphicData  gdata;

  classPtr = NULL;
  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    classPtr = gdata.classptr;
  }
  return classPtr;
}

#ifdef WIN_MAC
extern Nlm_Boolean Nlm_DoClick (Nlm_GraphiC a, Nlm_PoinT pt)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  Nlm_Boolean     (*clk) PROTO((Nlm_GraphiC, Nlm_PoinT));
  Nlm_Boolean     rsult;

  rsult = FALSE;
  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    clk = classPtr->click;
    if (clk != NULL) {
      rsult = clk (a, pt);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
  return rsult;
}

extern Nlm_Boolean Nlm_DoKey (Nlm_GraphiC a, Nlm_Char ch)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  Nlm_Boolean     (*ky) PROTO((Nlm_GraphiC, Nlm_Char));
  Nlm_Boolean     rsult;

  rsult = FALSE;
  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    ky = classPtr->key;
    if (ky != NULL) {
      rsult = ky (a, ch);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
  return rsult;
}

extern void Nlm_DoDraw (Nlm_GraphiC a)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*drw) PROTO((Nlm_GraphiC));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    drw = classPtr->draw;
    if (drw != NULL) {
      drw (a);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern Nlm_Boolean Nlm_DoIdle (Nlm_GraphiC a, Nlm_PoinT pt)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  Nlm_Boolean     (*idl) PROTO((Nlm_GraphiC, Nlm_PoinT));
  Nlm_Boolean     rsult;

  rsult = FALSE;
  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    idl = classPtr->idle;
    if (idl != NULL) {
      rsult = idl (a, pt);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
  return rsult;
}
#endif

#ifdef WIN_MSWIN
extern Nlm_Boolean Nlm_DoCommand (Nlm_GraphiC a)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  Nlm_Boolean     (*cmd) PROTO((Nlm_GraphiC));
  Nlm_Boolean     rsult;

  rsult = FALSE;
  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    cmd = classPtr->command;
    if (cmd != NULL) {
      rsult = cmd (a);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
  return rsult;
}
#endif

#ifdef WIN_MOTIF
extern void Nlm_DoCallback (Nlm_GraphiC a)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*clb) PROTO((Nlm_GraphiC));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    clb = classPtr->callback;
    if (clb != NULL) {
      clb (a);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}
#endif

extern void Nlm_DoShow (Nlm_GraphiC a, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*shw) PROTO((Nlm_GraphiC, Nlm_Boolean, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    shw = classPtr->show;
    if (shw != NULL) {
      shw (a, setFlag, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern void Nlm_DoHide (Nlm_GraphiC a, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*hid) PROTO((Nlm_GraphiC, Nlm_Boolean, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    hid = classPtr->hide;
    if (hid != NULL) {
      hid (a, setFlag, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern void Nlm_DoEnable (Nlm_GraphiC a, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*enbl) PROTO((Nlm_GraphiC, Nlm_Boolean, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    enbl = classPtr->enable;
    if (enbl != NULL) {
      enbl (a, setFlag, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern void Nlm_DoDisable (Nlm_GraphiC a, Nlm_Boolean setFlag, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*dsbl) PROTO((Nlm_GraphiC, Nlm_Boolean, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    dsbl = classPtr->disable;
    if (dsbl != NULL) {
      dsbl (a, setFlag, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern void Nlm_DoActivate (Nlm_GraphiC a, Nlm_Boolean savePort)

{
  void            (*actvate) PROTO((Nlm_GraphiC, Nlm_Boolean));
  Nlm_Boolean     cont;
  Nlm_GphPrcsPtr  classPtr;

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    actvate = classPtr->activate;
    if (actvate != NULL) {
      actvate (a, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern void Nlm_DoDeactivate (Nlm_GraphiC a, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*deactvate) PROTO((Nlm_GraphiC, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    deactvate = classPtr->deactivate;
    if (deactvate != NULL) {
      deactvate (a, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern Nlm_Handle Nlm_DoRemove (Nlm_GraphiC a, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*rmv) PROTO((Nlm_GraphiC, Nlm_Boolean));

  cont = TRUE;
  Nlm_DoHide (a, TRUE, savePort);
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    rmv = classPtr->remove;
    if (rmv != NULL) {
      rmv (a, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
    recentGraphic = NULL;
    recentBox = NULL;
  }
  return NULL;
}

extern void Nlm_DoReset (Nlm_GraphiC a, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*rst) PROTO((Nlm_GraphiC, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    rst = classPtr->reset;
    if (rst != NULL) {
      rst (a, TRUE);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern void Nlm_DoSelect (Nlm_GraphiC a, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*sel) PROTO((Nlm_GraphiC, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    sel = classPtr->select;
    if (sel != NULL) {
      sel (a, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern Nlm_Int2 Nlm_CountItems (Nlm_Handle a)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  Nlm_Int2        (*cntitm) PROTO((Nlm_GraphiC));
  Nlm_Int2        len;

  cont = TRUE;
  len = 0;
  classPtr = Nlm_GetClassPtr ((Nlm_GraphiC)a);
  while (classPtr != NULL && cont) {
    cntitm = classPtr->countItems;
    if (cntitm != NULL) {
      len = cntitm ((Nlm_GraphiC)a);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
  return len;
}

extern Nlm_GraphiC Nlm_DoLinkIn (Nlm_GraphiC a, Nlm_GraphiC prnt)

{
  Nlm_GphPrcsPtr  prntClassPtr;
  Nlm_Boolean     cont;
  Nlm_GraphiC     (*lnkIn) PROTO((Nlm_GraphiC, Nlm_GraphiC));
  Nlm_GraphiC     rsult;

  cont = TRUE;
  rsult = NULL;
  if (a != NULL && prnt != NULL) {
    cont = TRUE;
    prntClassPtr = Nlm_GetClassPtr (prnt);
    while (prntClassPtr != NULL && cont) {
      lnkIn = prntClassPtr->linkIn;
      if (lnkIn != NULL) {
        rsult = lnkIn (a, prnt);
        cont = FALSE;
      } else {
        prntClassPtr = prntClassPtr->ancestor;
      }
    }
  }
  return rsult;
}

extern void Nlm_DoAdjustPrnt (Nlm_GraphiC a, Nlm_RectPtr r,
                              Nlm_Boolean align, Nlm_Boolean savePort)

{
  void            (*adjst) PROTO((Nlm_GraphiC, Nlm_RectPtr, Nlm_Boolean, Nlm_Boolean));
  Nlm_Boolean     cont;
  Nlm_GraphiC     p;
  Nlm_GphPrcsPtr  prntClassPtr;

  cont = TRUE;
  if (a != NULL) {
    p = Nlm_GetParent (a);
    prntClassPtr = Nlm_GetClassPtr (p);
    while (prntClassPtr != NULL && cont) {
      adjst = prntClassPtr->adjustPrnt;
      if (adjst != NULL) {
        adjst (a, r, align, savePort);
        cont = FALSE;
      } else {
        prntClassPtr = prntClassPtr->ancestor;
      }
    }
  }
}

extern void Nlm_DoSetTitle (Nlm_GraphiC a, Nlm_Int2 item,
                            Nlm_CharPtr title, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*stttl) PROTO((Nlm_GraphiC, Nlm_Int2, Nlm_CharPtr, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    stttl = classPtr->setTitle;
    if (stttl != NULL) {
      stttl (a, item, title, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern void Nlm_DoGetTitle (Nlm_GraphiC a, Nlm_Int2 item,
                            Nlm_CharPtr title, Nlm_sizeT maxsize)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  Nlm_Boolean     failed;
  void            (*gtttl) PROTO((Nlm_GraphiC, Nlm_Int2, Nlm_CharPtr, Nlm_sizeT));

  cont = TRUE;
  failed = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    gtttl = classPtr->getTitle;
    if (gtttl != NULL) {
      gtttl (a, item, title, maxsize);
      failed = FALSE;
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
  if (failed) {
    Nlm_StringNCpy (title, "", maxsize);
  }
}

extern void Nlm_DoSetValue (Nlm_GraphiC a, Nlm_Int2 value, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*stval) PROTO((Nlm_GraphiC, Nlm_Int2, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    stval = classPtr->setValue;
    if (stval != NULL) {
      stval (a, value, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern Nlm_Int2 Nlm_DoGetValue (Nlm_GraphiC a)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  Nlm_Int2        (*gtval) PROTO((Nlm_GraphiC));
  Nlm_Int2        value;

  cont = TRUE;
  value = 0;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    gtval = classPtr->getValue;
    if (gtval != NULL) {
      value = gtval (a);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
  return value;
}

extern void Nlm_DoSetStatus (Nlm_GraphiC a, Nlm_Int2 item,
                             Nlm_Boolean status, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*ststs) PROTO((Nlm_GraphiC, Nlm_Int2, Nlm_Boolean, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    ststs = classPtr->setStatus;
    if (ststs != NULL) {
      ststs (a, item, status, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern Nlm_Boolean Nlm_DoGetStatus (Nlm_GraphiC a, Nlm_Int2 item)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  Nlm_Boolean     (*gtsts) PROTO((Nlm_GraphiC, Nlm_Int2));
  Nlm_Boolean     val;

  cont = TRUE;
  val = FALSE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    gtsts = classPtr->getStatus;
    if (gtsts != NULL) {
      val = gtsts (a, item);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
  return val;
}

extern void Nlm_DoSetOffset (Nlm_GraphiC a, Nlm_Int2 horiz,
                             Nlm_Int2 vert,Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*stoff) PROTO((Nlm_GraphiC, Nlm_Int2, Nlm_Int2, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    stoff = classPtr->setOffset;
    if (stoff != NULL) {
      stoff (a, horiz, vert, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern void Nlm_DoGetOffset (Nlm_GraphiC a, Nlm_Int2Ptr horiz, Nlm_Int2Ptr vert)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  Nlm_Boolean     failed;
  void            (*gtoff) PROTO((Nlm_GraphiC, Nlm_Int2Ptr, Nlm_Int2Ptr));

  cont = TRUE;
  failed = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    gtoff = classPtr->getOffset;
    if (gtoff != NULL) {
      gtoff (a, horiz, vert);
      failed = FALSE;
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
  if (failed) {
    if (horiz != NULL) {
      *horiz = 0;
    }
    if (vert != NULL) {
      *vert = 0;
    }
  }
}

extern void Nlm_DoSetPosition (Nlm_GraphiC a, Nlm_RectPtr r, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*stpos) PROTO((Nlm_GraphiC, Nlm_RectPtr, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    stpos = classPtr->setPosition;
    if (stpos != NULL) {
      stpos (a, r, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern void Nlm_DoGetPosition (Nlm_GraphiC a, Nlm_RectPtr r)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  Nlm_Boolean     failed;
  void            (*gtpos) PROTO((Nlm_GraphiC, Nlm_RectPtr));

  cont = TRUE;
  failed = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    gtpos = classPtr->getPosition;
    if (gtpos != NULL) {
      gtpos (a, r);
      failed = FALSE;
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
  if (failed) {
    if (r != NULL) {
      Nlm_LoadRect (r, 0, 0, 0, 0);
    }
  }
}

extern void Nlm_DoSetRange (Nlm_GraphiC a, Nlm_Int2 pgUp,
                            Nlm_Int2 pgDn, Nlm_Int2 max,
                            Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*rng) PROTO((Nlm_GraphiC, Nlm_Int2, Nlm_Int2, Nlm_Int2, Nlm_Boolean));


  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    rng = classPtr->setRange;
    if (rng != NULL) {
      rng (a, pgUp, pgDn, max, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern Nlm_GraphiC Nlm_DoGainFocus (Nlm_GraphiC a, Nlm_Char ch, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  Nlm_GraphiC     rsult;
  Nlm_GraphiC     (*gainFcs) PROTO((Nlm_GraphiC, Nlm_Char, Nlm_Boolean));

  cont = TRUE;
  rsult = NULL;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    gainFcs = classPtr->gainFocus;
    if (gainFcs != NULL) {
      rsult = gainFcs (a, ch, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
  return rsult;
}

extern void Nlm_DoLoseFocus (Nlm_GraphiC a, Nlm_GraphiC excpt, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*loseFcs) PROTO((Nlm_GraphiC, Nlm_GraphiC, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    loseFcs = classPtr->loseFocus;
    if (loseFcs != NULL) {
      loseFcs (a, excpt, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

extern void Nlm_DoSendChar (Nlm_GraphiC a, Nlm_Char ch, Nlm_Boolean savePort)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Boolean     cont;
  void            (*sendCh) PROTO((Nlm_GraphiC, Nlm_Char, Nlm_Boolean));

  cont = TRUE;
  classPtr = Nlm_GetClassPtr (a);
  while (classPtr != NULL && cont) {
    sendCh = classPtr->sendChar;
    if (sendCh != NULL) {
      sendCh (a, ch, savePort);
      cont = FALSE;
    } else {
      classPtr = classPtr->ancestor;
    }
  }
}

static Nlm_GraphiC Nlm_CheckThisLevel (Nlm_GraphiC a, Nlm_Char ch)

{
  Nlm_GraphiC  p;
  Nlm_GraphiC  q;

  q = NULL;
  p = a;
  while (p != NULL && q == NULL) {
    q = Nlm_DoGainFocus (p, ch, FALSE);
    p = Nlm_GetNext (p);
  }
  return q;
}

extern void Nlm_DoSendFocus (Nlm_GraphiC a, Nlm_Char ch)

{
  Nlm_GraphiC   cur;
  Nlm_GraphiC   nxt;
  Nlm_GraphiC   q;

  q = NULL;
  nxt = (Nlm_GraphiC) a;
  while (q == NULL && nxt != NULL) {
    cur = nxt;
    nxt = Nlm_GetParent (cur);
    if (nxt != NULL) {
      cur = Nlm_GetNext (cur);
    }
    q = Nlm_CheckThisLevel (cur, ch);
  }
}

extern void Nlm_DoAction (Nlm_GraphiC a)

{
  void             (*act) PROTO((Nlm_GraphiC));
  Nlm_GraphicData  gdata;

  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    act = gdata.action;
    if (act != NULL) {
      act (a);
    }
  }
}

extern void Nlm_Show (Nlm_Handle a)

{
  Nlm_DoShow ((Nlm_GraphiC) a, TRUE, TRUE);
}

extern void Nlm_Hide (Nlm_Handle a)

{
  Nlm_DoHide ((Nlm_GraphiC) a, TRUE, TRUE);
}

extern void Nlm_Enable (Nlm_Handle a)

{
  Nlm_DoEnable ((Nlm_GraphiC) a, TRUE, TRUE);
}

extern void Nlm_Disable (Nlm_Handle a)

{
  Nlm_DoDisable ((Nlm_GraphiC) a, TRUE, TRUE);
}

extern Nlm_Handle Nlm_Remove (Nlm_Handle a)

{
  return Nlm_DoRemove ((Nlm_GraphiC) a, TRUE);
}

extern void Nlm_Reset (Nlm_Handle a)

{
  Nlm_DoReset ((Nlm_GraphiC) a, TRUE);
}

extern void Nlm_Select (Nlm_Handle a)

{
  Nlm_DoSelect ((Nlm_GraphiC) a, TRUE);
}

extern void Nlm_SetTitle (Nlm_Handle a, Nlm_CharPtr title)

{
  Nlm_DoSetTitle ((Nlm_GraphiC) a, 0, title, TRUE);
}

extern void Nlm_GetTitle (Nlm_Handle a, Nlm_CharPtr title, Nlm_sizeT maxsize)

{
  Nlm_DoGetTitle ((Nlm_GraphiC) a, 0, title, maxsize);
}

extern void Nlm_SetValue (Nlm_Handle a, Nlm_Int2 value)

{
  Nlm_DoSetValue ((Nlm_GraphiC) a, value, TRUE);
}

extern Nlm_Int2 Nlm_GetValue (Nlm_Handle a)

{
  return (Nlm_DoGetValue ((Nlm_GraphiC) a));
}

extern void Nlm_SetStatus (Nlm_Handle a, Nlm_Boolean status)

{
  Nlm_DoSetStatus ((Nlm_GraphiC) a, 0, status, TRUE);
}

extern Nlm_Boolean Nlm_GetStatus (Nlm_Handle a)

{
  return (Nlm_DoGetStatus ((Nlm_GraphiC) a, 0));
}

extern void Nlm_SetOffset (Nlm_Handle a, Nlm_Int2 horiz, Nlm_Int2 vert)

{
  Nlm_DoSetOffset ((Nlm_GraphiC) a, horiz, vert, TRUE);
}

extern void Nlm_GetOffset (Nlm_Handle a, Nlm_Int2Ptr horiz, Nlm_Int2Ptr vert)

{
  Nlm_DoGetOffset ((Nlm_GraphiC) a, horiz, vert);
}

extern void Nlm_SetPosition (Nlm_Handle a, Nlm_RectPtr r)

{
  Nlm_DoSetPosition ((Nlm_GraphiC) a, r, TRUE);
}

extern void Nlm_GetPosition (Nlm_Handle a, Nlm_RectPtr r)

{
  Nlm_DoGetPosition ((Nlm_GraphiC) a, r);
}

extern void Nlm_SetRange (Nlm_Handle a, Nlm_Int2 pgUp,
                          Nlm_Int2 pgDn, Nlm_Int2 max)

{
  Nlm_DoSetRange ((Nlm_GraphiC) a, pgUp, pgDn, max, TRUE);
}

extern void Nlm_SetItemTitle (Nlm_Handle a, Nlm_Int2 item, Nlm_CharPtr title)

{
  Nlm_DoSetTitle ((Nlm_GraphiC) a, item, title, TRUE);
}

extern void Nlm_GetItemTitle (Nlm_Handle a, Nlm_Int2 item, Nlm_CharPtr title, Nlm_sizeT maxsize)

{
  Nlm_DoGetTitle ((Nlm_GraphiC) a, item, title, maxsize);
}

extern void Nlm_SetItemStatus (Nlm_Handle a, Nlm_Int2 item, Nlm_Boolean status)

{
  Nlm_DoSetStatus ((Nlm_GraphiC) a, item, status, TRUE);
}

extern Nlm_Boolean Nlm_GetItemStatus (Nlm_Handle a, Nlm_Int2 item)

{
  return (Nlm_DoGetStatus ((Nlm_GraphiC) a, item));
}

extern Nlm_Int2 Nlm_GetNextItem (Nlm_Handle a, Nlm_Int2 prev)

{
  Nlm_GphPrcsPtr  classPtr;
  Nlm_Int2        (*cntitm) PROTO((Nlm_GraphiC));
  Nlm_Int2        count;
  Nlm_Boolean     (*gtsts) PROTO((Nlm_GraphiC, Nlm_Int2));
  Nlm_Boolean     found;
  Nlm_Int2        rsult;

  rsult = 0;
  if (a != NULL && prev >= 0) {
    classPtr = Nlm_GetClassPtr ((Nlm_GraphiC) a);
    if (classPtr != NULL) {
      cntitm = classPtr->countItems;
      gtsts = classPtr->getStatus;
      if (cntitm != NULL && gtsts != NULL) {
        count = cntitm ((Nlm_GraphiC) a);
        found = FALSE;
        while (prev < count && (! found)) {
          prev++;
          found = gtsts ((Nlm_GraphiC) a, prev);
        }
        if (found) {
          rsult = prev;
        }
      }
    }
  }
  return rsult;
}

extern void Nlm_ClearItemsInGroup (Nlm_GraphiC a, Nlm_GraphiC excpt, Nlm_Boolean savePort)

{
  Nlm_GraphiC  g;

  if (a != NULL) {
    g = Nlm_GetChild (a);
    while (g != NULL) {
      if (g != excpt) {
        Nlm_DoSetStatus (g, 0, FALSE, savePort);
      }
      g = Nlm_GetNext (g);
    }
  }
}

extern Nlm_Int2 Nlm_CountGroupItems (Nlm_GraphiC a)

{
  Nlm_GraphiC  g;
  Nlm_Int2     i;

  i = 0;
  if (a != NULL) {
    g = Nlm_GetChild (a);
    while (g != NULL) {
      i++;
      g = Nlm_GetNext (g);
    }
  }
  return i;
}

extern Nlm_GraphiC Nlm_LinkIn (Nlm_GraphiC a, Nlm_GraphiC prnt)

{
  Nlm_GraphiC  first;
  Nlm_GraphiC  g;
  Nlm_GphPtr   gp;
  Nlm_GraphiC  last;
  Nlm_GphPtr   lastp;
  Nlm_GraphiC  p;
  Nlm_GphPtr   prntp;

  if (prnt != NULL) {
    g = (Nlm_GraphiC) a;
    prntp = (Nlm_GphPtr) Nlm_HandLock (prnt);
    first = prntp->children;
    last = prntp->lastChild;
    Nlm_HandUnlock (prnt);
    if (last != NULL) {
      lastp = (Nlm_GphPtr) Nlm_HandLock (last);
      lastp->next = g;
      Nlm_HandUnlock (last);
      prntp = (Nlm_GphPtr) Nlm_HandLock (prnt);
      prntp->lastChild = g;
      Nlm_HandUnlock (prnt);
    } else if (first != NULL) {
      p = first;
      while (p != NULL) {
        last = p;
        p = Nlm_GetNext (p);
      }
      lastp = (Nlm_GphPtr) Nlm_HandLock (last);
      lastp->next = g;
      Nlm_HandUnlock (last);
      prntp = (Nlm_GphPtr) Nlm_HandLock (prnt);
      prntp->lastChild = g;
      Nlm_HandUnlock (prnt);
    } else {
      prntp = (Nlm_GphPtr) Nlm_HandLock (prnt);
      prntp->children = g;
      prntp->lastChild = g;
      Nlm_HandUnlock (prnt);
    }
    gp = (Nlm_GphPtr) Nlm_HandLock (g);
    gp->next = NULL;
    gp->parent = prnt;
    gp->children = NULL;
    gp->lastChild = NULL;
    Nlm_HandUnlock (g);
  }
  recentGraphic = NULL;
  recentBox = NULL;
  return a;
}

extern void Nlm_LoadAction (Nlm_GraphiC a, Nlm_ActnProc actnProc)

{
  Nlm_GraphicData  gdata;

  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    gdata.action = actnProc;
    Nlm_SetGraphicData (a, &gdata);
  }
}

extern void Nlm_LoadGraphicData (Nlm_GraphiC a, Nlm_GraphiC nxt,
                                 Nlm_GraphiC prnt, Nlm_GraphiC chld,
                                 Nlm_GraphiC lstchd, Nlm_GphPrcs PNTR classPtr,
                                 Nlm_ActnProc actnProc, Nlm_RectPtr r,
                                 Nlm_Boolean enabl, Nlm_Boolean vis)

{
  Nlm_GphPtr  ptr;

  if (a != NULL) {
    ptr = (Nlm_GphPtr) Nlm_HandLock (a);
    ptr->next = nxt;
    ptr->parent = prnt;
    ptr->children = chld;
    ptr->lastChild = lstchd;
    ptr->classptr = classPtr;
    ptr->action = actnProc;
    if (r != NULL) {
      ptr->rect = *r;
    } else {
      Nlm_LoadRect (&(ptr->rect), 0, 0, 0, 0);
    }
    ptr->enabled = enabl;
    ptr->visible = vis;
    Nlm_HandUnlock (a);
    recentGraphic = NULL;
  }
}

extern void Nlm_LoadBoxData (Nlm_BoX a, Nlm_PoinT nxt,
                             Nlm_PoinT lmt, Nlm_PoinT rst,
                             Nlm_Int2 top, Nlm_Int2 ncol,
                             Nlm_Int2 xMrg, Nlm_Int2 yMrg,
                             Nlm_Int2 xSpc, Nlm_Int2 ySpc,
                             Nlm_Int2 wid, Nlm_Int2 hgt)

{
  Nlm_BoxPtr   bp;
  Nlm_BoxData  PNTR bptr;

  if (a != NULL) {
    bp = (Nlm_BoxPtr) Nlm_HandLock (a);
    bptr = &(bp->box);
    bptr->nextPoint = nxt;
    bptr->limitPoint = lmt;
    bptr->resetPoint = rst;
    bptr->topRow = top;
    bptr->nextCol = ncol;
    bptr->xMargin = xMrg;
    bptr->yMargin = yMrg;
    bptr->xSpacing = xSpc;
    bptr->ySpacing = ySpc;
    bptr->boxWidth = wid;
    bptr->boxHeight = hgt;
    Nlm_HandUnlock (a);
    recentBox = NULL;
  }
}

extern void Nlm_SetAction (Nlm_Handle a, Nlm_ActnProc actn)

{
  Nlm_GraphiC      g;
  Nlm_GraphicData  gdata;

  if (a != NULL) {
    g = (Nlm_GraphiC) a;
    if (g != NULL) {
      Nlm_GetGraphicData (g, &gdata);
      gdata.action = actn;
      Nlm_SetGraphicData (g, &gdata);
    }
  }
}

extern Nlm_GraphiC Nlm_CreateLink (Nlm_GraphiC prnt, Nlm_RectPtr r,
                                   Nlm_Int2 recordSize, Nlm_GphPrcs PNTR classPtr)

{
  Nlm_GraphiC  a;
  Nlm_GraphiC  rsult;

  rsult = NULL;
  a = NULL;
  if (prnt != NULL) {
    a = (Nlm_GraphiC) Nlm_HandNew (recordSize);
    if (a != NULL) {
      Nlm_LoadGraphicData (a, NULL, prnt, NULL, NULL, classPtr,
                           NULL, r, TRUE, FALSE);
      rsult = Nlm_DoLinkIn (a, prnt);
      if (rsult != NULL) {
        if (Nlm_nextIdNumber < 32767) {
          Nlm_nextIdNumber++;
        }
      } else {
        Nlm_HandFree (a);
      }
    }
  }
  return rsult;
}

extern void Nlm_RemoveLink (Nlm_GraphiC a)

{
  Nlm_GraphiC  first;
  Nlm_GraphiC  g;
  Nlm_GraphiC  n;
  Nlm_GraphiC  p;
  Nlm_GraphiC  prnt;
  Nlm_GphPtr   prntp;
  Nlm_GraphiC  q;
  Nlm_GphPtr   qp;

  if (a != NULL) {
    g = (Nlm_GraphiC) a;
    prnt = Nlm_GetParent (g);
    first = Nlm_GetChild (prnt);
    if (g == first) {
      n = Nlm_GetNext (g);
      prntp = (Nlm_GphPtr) Nlm_HandLock (prnt);
      prntp->children = n;
      if (prntp->lastChild == g) {
        prntp->lastChild = NULL;
      }
      Nlm_HandUnlock (prnt);
    } else {
      p = first;
      q = NULL; /* dgg: bug fix for when p==NULL*/
      while (p != NULL && p != g) {
        q = p;
        p = Nlm_GetNext (p);
      }
      if (q != NULL) {
        n = Nlm_GetNext (g);
        qp = (Nlm_GphPtr) Nlm_HandLock (q);
        qp->next = n;
        Nlm_HandUnlock (q);
      }
    }
    Nlm_HandFree (g);
  }
  recentGraphic = NULL;
  recentBox = NULL;
}

extern Nlm_GraphiC Nlm_FindItem (Nlm_GraphiC a, Nlm_Int2 item)

{
  Nlm_GraphiC  g;

  g = NULL;
  if (a != NULL && item > 0) {
    g = Nlm_GetChild (a);
    while (g != NULL && item > 1) {
      item--;
      g = Nlm_GetNext (g);
    }
  }
  return g;
}

extern Nlm_Int2 Nlm_GetItemIndex (Nlm_GraphiC a)

{
  Nlm_GraphiC  g;
  Nlm_Int2     index;
  Nlm_Int2     rsult;

  rsult = 0;
  if (a != NULL) {
    index = 1;
    g = Nlm_GetChild (a);
    while (g != NULL && g != a) {
      index++;
      g = Nlm_GetNext (g);
    }
    if (g == a) {
      rsult = index;
    } else {
      rsult = 0;
    }
  }
  return rsult;
}

extern Nlm_Boolean Nlm_GetAllParentsEnabled (Nlm_GraphiC a)

{
  Nlm_GraphiC  p;

  p = NULL;
  if (a != NULL) {
    p = Nlm_GetParent (a);
    while (p != NULL && Nlm_GetEnabled (p)) {
      p = Nlm_GetParent (p);
    }
  }
  return (Nlm_Boolean) (p == NULL);
}

extern Nlm_Boolean Nlm_AllParentsEnabled (Nlm_Handle a)

{
  return Nlm_GetAllParentsEnabled ((Nlm_GraphiC) a);
}

extern Nlm_Boolean Nlm_AllParentsButWindowVisible (Nlm_GraphiC a)

{
  Nlm_GraphiC  p;

  p = NULL;
  if (a != NULL) {
    p = Nlm_GetParent (a);
    while (p != NULL && Nlm_GetVisible (p)) {
      p = Nlm_GetParent (p);
    }
  }
  return (Nlm_Boolean) (p == NULL || Nlm_GetParent (p) == NULL);
}

extern Nlm_Boolean Nlm_GetAllParentsVisible (Nlm_GraphiC a)

{
  Nlm_GraphiC  p;

  p = NULL;
  if (a != NULL) {
    p = Nlm_GetParent (a);
    while (p != NULL && Nlm_GetVisible (p)) {
      p = Nlm_GetParent (p);
    }
  }
  return (Nlm_Boolean) (p == NULL);
}

extern Nlm_Boolean Nlm_AllParentsVisible (Nlm_Handle a)

{
  return Nlm_GetAllParentsVisible ((Nlm_GraphiC) a);
}

extern void Nlm_SetNext (Nlm_GraphiC a, Nlm_GraphiC nxt)

{
  Nlm_GraphicData  gdata;

  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    gdata.next = nxt;
    Nlm_SetGraphicData (a, &gdata);
  }
}

extern Nlm_GraphiC Nlm_GetNext (Nlm_GraphiC a)

{
  Nlm_GraphicData  gdata;
  Nlm_GraphiC      rsult;

  rsult = NULL;
  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    rsult = gdata.next;
  }
  return rsult;
}

extern void Nlm_SetParent (Nlm_GraphiC a, Nlm_GraphiC prnt)

{
  Nlm_GraphicData  gdata;

  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    gdata.parent = prnt;
    Nlm_SetGraphicData (a, &gdata);
  }
}

extern Nlm_GraphiC Nlm_GetParent (Nlm_GraphiC a)

{
  Nlm_GraphicData  gdata;
  Nlm_GraphiC      rsult;

  rsult = NULL;
  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    rsult = gdata.parent;
  }
  return rsult;
}

extern Nlm_Handle Nlm_Parent (Nlm_Handle a)

{
  return Nlm_GetParent ((Nlm_GraphiC) a);
}

extern void Nlm_SetChild (Nlm_GraphiC a, Nlm_GraphiC chld)

{
  Nlm_GraphicData  gdata;

  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    gdata.children = chld;
    Nlm_SetGraphicData (a, &gdata);
  }
}

extern Nlm_GraphiC Nlm_GetChild (Nlm_GraphiC a)

{
  Nlm_GraphicData  gdata;
  Nlm_GraphiC      rsult;

  rsult = NULL;
  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    rsult = gdata.children;
  }
  return rsult;
}

extern void Nlm_SetRect (Nlm_GraphiC a, Nlm_RectPtr r)

{
  Nlm_GraphicData  gdata;

  if (a != NULL && r != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    gdata.rect = *r;
    Nlm_SetGraphicData (a, &gdata);
  }
}

extern void Nlm_GetRect (Nlm_GraphiC a, Nlm_RectPtr r)

{
  Nlm_GraphicData  gdata;

  if (a != NULL && r != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    *r = gdata.rect;
  }
}

extern void Nlm_ObjectRect (Nlm_Handle a, Nlm_RectPtr r)

{
  Nlm_GetRect ((Nlm_GraphiC) a, r);
}

extern void Nlm_InvalObject (Nlm_Handle a)

{
  Nlm_RecT  r;

  if (a != NULL) {
    Nlm_GetRect ((Nlm_GraphiC) a, &r);
    Nlm_InsetRect (&r, -1, -1);
    Nlm_InvalRect (&r);
  }
}

extern void Nlm_SetEnabled (Nlm_GraphiC a, Nlm_Boolean enabld)

{
  Nlm_GraphicData  gdata;

  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    gdata.enabled = enabld;
    Nlm_SetGraphicData (a, &gdata);
  }
}

extern Nlm_Boolean Nlm_GetEnabled (Nlm_GraphiC a)

{
  Nlm_GraphicData  gdata;
  Nlm_Boolean      rsult;

  rsult = FALSE;
  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    rsult = gdata.enabled;
  }
  return rsult;
}

extern Nlm_Boolean Nlm_Enabled (Nlm_Handle a)

{
  return Nlm_GetEnabled ((Nlm_GraphiC) a);
}

extern void Nlm_SetVisible (Nlm_GraphiC a, Nlm_Boolean visibl)

{
  Nlm_GraphicData  gdata;

  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    gdata.visible = visibl;
    Nlm_SetGraphicData (a, &gdata);
  }
}

extern Nlm_Boolean Nlm_GetVisible (Nlm_GraphiC a)

{
  Nlm_GraphicData  gdata;
  Nlm_Boolean      rsult;

  rsult = FALSE;
  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    rsult = gdata.visible;
  }
  return rsult;
}

extern Nlm_Boolean Nlm_Visible (Nlm_Handle a)

{
  return Nlm_GetVisible ((Nlm_GraphiC) a);
}

extern void Nlm_SetGraphicData (Nlm_GraphiC a, Nlm_GraphicData PNTR gdata)

{
  Nlm_GphPtr  gp;

  if (a != NULL && gdata != NULL) {
    gp = (Nlm_GphPtr) Nlm_HandLock (a);
    *gp = *gdata;
    Nlm_HandUnlock (a);
    recentGraphic = a;
    recentGraphicData = *gdata;
  }
}

extern void Nlm_GetGraphicData (Nlm_GraphiC a, Nlm_GraphicData PNTR gdata)

{
  Nlm_GphPtr  gp;

  if (a != NULL && gdata != NULL) {
    if (a == recentGraphic && NLM_RISKY) {
      *gdata = recentGraphicData;
    } else {
      gp = (Nlm_GphPtr) Nlm_HandLock (a);
      *gdata = *gp;
      Nlm_HandUnlock (a);
      recentGraphic = a;
      recentGraphicData = *gdata;
    }
  }
}

extern void Nlm_SetBoxData (Nlm_BoX a, Nlm_BoxData PNTR bdata)

{
  Nlm_BoxPtr  bp;

  if (a != NULL && bdata != NULL) {
    bp = (Nlm_BoxPtr) Nlm_HandLock (a);
    bp->box = *bdata;
    Nlm_HandUnlock (a);
    recentBox = a;
    recentBoxData = *bdata;
  }
}

extern void Nlm_GetBoxData (Nlm_BoX a, Nlm_BoxData PNTR bdata)

{
  Nlm_BoxPtr  bp;

  if (a != NULL && bdata != NULL) {
    if (a == recentBox && NLM_RISKY) {
      *bdata = recentBoxData;
    } else {
      bp = (Nlm_BoxPtr) Nlm_HandLock (a);
      *bdata = bp->box;
      Nlm_HandUnlock (a);
      recentBox = a;
      recentBoxData = *bdata;
    }
  }
}

extern void Nlm_SetExtraData (Nlm_GraphiC a, Nlm_VoidPtr dptr,
                              Nlm_Int2 start, Nlm_Int2 extra)

{
  Nlm_BytePtr  dst;
  Nlm_GphPtr   gp;
  Nlm_BytePtr  src;

  if (a != NULL && dptr != NULL) {
    gp = (Nlm_GphPtr) Nlm_HandLock (a);
    dst = (Nlm_BytePtr) gp + start;
    src = (Nlm_BytePtr) dptr;
    while (extra > 0) {
      *dst = *src;
      dst++;
      src++;
      extra--;
    }
    Nlm_HandUnlock (a);
  }
}

extern void Nlm_GetExtraData (Nlm_GraphiC a, Nlm_VoidPtr dptr,
                              Nlm_Int2 start, Nlm_Int2 extra)

{
  Nlm_BytePtr  dst;
  Nlm_GphPtr   gp;
  Nlm_BytePtr  src;

  if (a != NULL && dptr != NULL) {
    gp = (Nlm_GphPtr) Nlm_HandLock (a);
    src = (Nlm_BytePtr) gp + start;
    dst = (Nlm_BytePtr) dptr;
    while (extra > 0) {
      *dst = *src;
      dst++;
      src++;
      extra--;
    }
    Nlm_HandUnlock (a);
  }
}

extern void Nlm_PointToolToPoinT (Nlm_PointTool src, Nlm_PointPtr dst)

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
#ifdef WIN_MOTIF
    dst->x = src.x;
    dst->y = src.y;
#endif
  }
}

extern void Nlm_PoinTToPointTool (Nlm_PoinT src, Nlm_PointTool PNTR dst)

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
#ifdef WIN_MOTIF
    dst->x = src.x;
    dst->y = src.y;
#endif
  }
}

extern void Nlm_RectToolToRecT (Nlm_RectTool PNTR src, Nlm_RectPtr dst)

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
#ifdef WIN_MOTIF
    dst->left = src->x;
    dst->top = src->y;
    dst->right = src->x + src->width;
    dst->bottom = src->y + src->height;
#endif
  }
}

extern void Nlm_RecTToRectTool (Nlm_RectPtr src, Nlm_RectTool PNTR dst)

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
#ifdef WIN_MOTIF
    dst->x = MIN (src->left, src->right);
    dst->y = MIN (src->top, src->bottom);
    dst->width = ABS (src->right - src->left);
    dst->height = ABS (src->bottom - src->top);
#endif
  }
}

extern void Nlm_LocalToGlobal (Nlm_PointPtr pt)

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
#ifdef WIN_MOTIF
#endif
}

extern void Nlm_GlobalToLocal (Nlm_PointPtr pt)

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
#ifdef WIN_MOTIF
#endif
}

#ifdef OS_MAC
extern void Nlm_CtoPstr (Nlm_CharPtr str)

{
#if defined(COMP_MPW) || defined(COMP_CODEWAR)
    c2pstr ((char *) str);
#else
#ifdef COMP_THINKC
    CtoPstr ((char *) str);
#endif
#endif
}

extern void Nlm_PtoCstr (Nlm_CharPtr str)

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

#ifdef WIN_MSWIN
extern Nlm_Boolean Nlm_VibrantDisabled (void)

{
  if (Nlm_GetAppProperty("disable_vibrant") != NULL) {
    disabled_count++;
    return TRUE;
  }
  return FALSE;
}
#endif

#ifdef WIN_MSWIN
static void Nlm_SetupPrinterDeviceContext (HDC prHDC)

{
  HDC         screenDC;
  HWND        screenHwnd;
  /*
  TEXTMETRIC  textMetrics1;
  TEXTMETRIC  textMetrics2;
  */

  if (prHDC != NULL) {
    Nlm_SelectFont (Nlm_programFont);
    /*
    GetTextMetrics (Nlm_currentHDC, &textMetrics1);
    */
    Nlm_SetPort ((HWND) NULL, (HDC) prHDC);
    /*
    GetTextMetrics (prHDC, &textMetrics2);
    */
    SetMapMode (prHDC, MM_ANISOTROPIC);
    screenHwnd = GetDesktopWindow ();
    screenDC = GetDC (screenHwnd);
    SetWindowExtEx (prHDC, GetDeviceCaps (screenDC, LOGPIXELSX),
                    GetDeviceCaps (screenDC, LOGPIXELSY), NULL);
    SetViewportExtEx (prHDC, GetDeviceCaps (prHDC, LOGPIXELSX),
                      GetDeviceCaps (prHDC, LOGPIXELSY), NULL);
    ReleaseDC (screenHwnd, screenDC);
    /*
    SetWindowExtEx (prHDC, textMetrics1.tmAveCharWidth,
                    textMetrics1.tmHeight, NULL);
    SetViewportExtEx (prHDC, textMetrics2.tmAveCharWidth,
                    textMetrics2.tmHeight, NULL);
    */
  }
}
#endif

extern Nlm_WindoW Nlm_StartPrinting (void)

{
  Nlm_WindoW  w;
#ifdef WIN_MSWIN
  DWORD       commdlgerr;
  DOCINFO     di;
  char        docName [256];
#endif

  w = Nlm_CurrentWindow ();
#ifdef WIN_MAC
  PrOpen ();
  if (prHdl == NULL) {
    prHdl = (THPrint) Nlm_HandNew (sizeof (TPrint));
    if (prHdl != NULL) {
      PrintDefault (prHdl);
      prerr = PrError ();
      if (prerr != noErr) {
        Nlm_Message (MSG_ERROR, "PrintDefault error %d", prerr);
      }
      if (! PrStlDialog (prHdl)) {
        prHdl = (THPrint) Nlm_HandFree (prHdl);
        w = NULL;
      }
    } else {
      Nlm_Message (MSG_ERROR, "Unable to create print handle");
    }
  }
  if (prHdl != NULL) {
    if (PrJobDialog (prHdl)) {
      prPort = PrOpenDoc (prHdl, NULL, NULL);
      Nlm_SetPort ((GrafPtr) prPort);
      prerr = PrError ();
      if (prerr != noErr) {
        Nlm_Message (MSG_ERROR, "PrOpenDoc error %d", prerr);
      } else {
        Nlm_nowPrinting = TRUE;
      }
    } else {
      w = NULL;
    }
  } else {
    w = NULL;
  }
#endif
#ifdef WIN_MSWIN
  abortPrint = FALSE;
  memset (&pd, 0, sizeof (PRINTDLG));
  pd.lStructSize = sizeof (PRINTDLG);
  pd.hwndOwner = Nlm_currentHWnd;
  pd.Flags = PD_RETURNDC;
  if (PrintDlg (&pd) != 0) {
    hPr = pd.hDC;
    if (hPr != NULL) {
      Nlm_SetupPrinterDeviceContext (hPr);
      Nlm_StringCpy (docName, "Vibrant");
      di.cbSize = sizeof (DOCINFO);
      di.lpszDocName = (LPCSTR) docName;
      di.lpszOutput = NULL;
      prerr = StartDoc (hPr, &di);
      if (prerr < 1) {
        abortPrint = TRUE;
        Nlm_Message (MSG_ERROR, "StartDoc error %d", prerr);
        w = NULL;
      } else {
        Nlm_nowPrinting = TRUE;
      }
    } else {
      commdlgerr = CommDlgExtendedError ();
      Nlm_Message (MSG_ERROR, "Unable to create print context, error %ul", (long) commdlgerr);
      w = NULL;
    }
  } else {
    w = NULL;
  }
#endif
#ifdef WIN_MOTIF
  w = NULL;
#endif
  return w;
}

extern void Nlm_EndPrinting (Nlm_WindoW w)

{
#ifdef WIN_MAC
  TPrStatus  prStat;
#endif

  Nlm_nowPrinting = FALSE;
#ifdef WIN_MAC
  if (w != NULL) {
    PrCloseDoc (prPort);
    prerr = PrError ();
    if (prerr != noErr) {
      Nlm_Message (MSG_ERROR, "PrCloseDoc error %d", prerr);
    }
    PrPicFile (prHdl, 0L, 0L, 0L, &prStat);
    prerr = PrError ();
    if (prerr != noErr) {
      Nlm_Message (MSG_ERROR, "PrPicFile error %d", prerr);
    }
    prPort = NULL;
    Nlm_UseWindow (w);
  }
  PrClose ();
#endif
#ifdef WIN_MSWIN
  if (w != NULL) {
    if (hPr != NULL) {
      if (! abortPrint) {
        prerr = EndDoc (hPr);
        if (prerr < 0) {
          Nlm_Message (MSG_ERROR, "EndDoc error %d", prerr);
        }
      }
      DeleteDC (hPr);
      if (pd.hDevMode != NULL) {
        GlobalFree (pd.hDevMode);
      }
      if (pd.hDevNames != NULL) {
        GlobalFree (pd.hDevNames);
      }
    }
    Nlm_UseWindow (w);
  }
#endif
#ifdef WIN_MOTIF
#endif
}

extern Nlm_Boolean Nlm_PrintingRect (Nlm_RectPtr rpt)

{
  Nlm_Boolean   rsult;
#ifdef WIN_MAC
  TPPrint       prPtr;
  Nlm_RectTool  rtool;
#endif
#ifdef WIN_MSWIN
  POINT         physPageSize;
  POINT         pixelsPerInch;
  Nlm_PoinT     pt;
#endif

  rsult = TRUE;
  if (rpt != NULL) {
    Nlm_LoadRect (rpt, 0, 0, 0, 0);
#ifdef WIN_MAC
    if (prHdl != NULL) {
      prPtr = (TPPrint) Nlm_HandLock (prHdl);
      if (prPtr != NULL) {
        rtool = prPtr->prInfo.rPage;
        Nlm_RectToolToRecT (&rtool, rpt);
        Nlm_InsetRect (rpt, 10, 10);
      } else {
        rsult = FALSE;
      }
      Nlm_HandUnlock (prHdl);
    } else {
      rsult = FALSE;
    }
#endif
#ifdef WIN_MSWIN
    if (hPr != NULL) {
      physPageSize.x = GetDeviceCaps (hPr, HORZRES);
      physPageSize.y = GetDeviceCaps (hPr, VERTRES);
      DPtoLP (hPr, &physPageSize, 1);
      Nlm_PointToolToPoinT (physPageSize, &pt);
      rpt->right = pt.x;
      rpt->bottom = pt.y;
      pixelsPerInch.x = GetDeviceCaps (hPr, LOGPIXELSX);
      pixelsPerInch.y = GetDeviceCaps (hPr, LOGPIXELSY);
      DPtoLP (hPr, &pixelsPerInch, 1);
      Nlm_InsetRect (rpt, pixelsPerInch.x / 2, pixelsPerInch.y / 2);
    } else {
      rsult = FALSE;
    }
#endif
#ifdef WIN_MOTIF
    rsult = FALSE;
#endif
  }
  return rsult;
}

extern Nlm_Boolean Nlm_StartPage (void)

{
  Nlm_Boolean  rsult;

  rsult = TRUE;
#ifdef WIN_MAC
  if (prPort != NULL) {
    PrOpenPage (prPort, NULL);
    prerr = PrError ();
    if (prerr != noErr) {
      Nlm_Message (MSG_ERROR, "PrOpenPage error %d", prerr);
      rsult = FALSE;
    }
  } else {
    rsult = FALSE;
  }
#endif
#ifdef WIN_MSWIN
  if (hPr != NULL) {
    Nlm_SetupPrinterDeviceContext (hPr);
    prerr = StartPage (hPr);
    if (prerr < 0) {
      Nlm_Message (MSG_ERROR, "StartPage error %d", prerr);
      rsult = FALSE;
    }
    Nlm_SetPort ((HWND) NULL, (HDC) hPr);
  }
#endif
#ifdef WIN_MOTIF
  rsult = FALSE;
#endif
  return rsult;
}

extern Nlm_Boolean Nlm_EndPage (void)

{
  Nlm_Boolean  rsult;

  rsult = TRUE;
#ifdef WIN_MAC
  if (prPort != NULL) {
    PrClosePage (prPort);
    prerr = PrError ();
    if (prerr != noErr) {
      Nlm_Message (MSG_ERROR, "PrClosePage error %d", prerr);
      rsult = FALSE;
    }
  } else {
    rsult = FALSE;
  }
#endif
#ifdef WIN_MSWIN
  if (hPr != NULL) {
    prerr = EndPage (hPr);
    if (prerr < 0) {
      abortPrint = TRUE;
      Nlm_Message (MSG_ERROR, "EndPage error %d", prerr);
      rsult = FALSE;
    }
  } else {
    rsult = FALSE;
  }
#endif
#ifdef WIN_MOTIF
  rsult = FALSE;
#endif
  return rsult;
}

#ifdef WIN_MAC
static OSType Nlm_GetOSType (Nlm_CharPtr str, OSType dfault)

{
  OSType  rsult;

  rsult = dfault;
  if (str != NULL && str [0] != '\0') {
    rsult = *(OSType*) str;
  }
  return rsult;
}

static void Nlm_GetFilePath (Nlm_Int2 currentVol, Nlm_CharPtr path, Nlm_sizeT maxsize)

{
  WDPBRec     block;
  Nlm_Char    directory [256];
  Nlm_Int4    dirID;
  OSErr       err;
  CInfoPBRec  params;
  Nlm_Char    temp [256];
  Nlm_Int2    vRefNum;

  block.ioNamePtr = NULL;
  block.ioVRefNum = currentVol;
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
    Nlm_PtoCstr (directory);
    Nlm_StrngCat (directory, ":", sizeof (directory));
    Nlm_StrngCat (directory, temp, sizeof (directory));
    Nlm_StrngCpy (temp, directory, sizeof (temp));
  } while (params.dirInfo.ioDrDirID != fsRtDirID);
  Nlm_StringNCpy (path, temp, maxsize);
}
#endif

#ifdef WIN_MOTIF
static void Nlm_CreateFileDialogShell (void)

{
  Cardinal  n;
  Arg       wargs [15];

  if (Nlm_fileDialogShell == NULL) {
    n = 0;
    XtSetArg (wargs[n], XmNdefaultFontList, Nlm_XfontList); n++;
    XtSetArg (wargs[n], XmNdeleteResponse, XmDO_NOTHING); n++;
    Nlm_fileDialogShell = XtAppCreateShell ((String) NULL, (String) "Vibrant",
                                            applicationShellWidgetClass,
                                            Nlm_currentXDisplay, wargs, n);
  }
}

static void Nlm_FileCancelCallback (Widget fs, XtPointer client_data, XtPointer call_data)

{
  fileBoxUp = FALSE;
  fileBoxRsult = FALSE;
}

static void Nlm_FileOkCallback (Widget fs, XtPointer client_data, XtPointer call_data)

{
  XmFileSelectionBoxCallbackStruct  *cbs;
  char                              *filename;

  cbs = (XmFileSelectionBoxCallbackStruct *) call_data;
  fileBoxUp = FALSE;
  if (! XmStringGetLtoR (cbs->value, XmSTRING_DEFAULT_CHARSET, &filename)) {
    return;
  }
  if (! *filename) {
    XtFree (filename);
    return;
  }
  Nlm_StringNCpy (filePath, filename, sizeof (filePath));
  XtFree (filename);
  fileBoxRsult = TRUE;
}

static void Nlm_FileNoMatchCallback (Widget fs, XtPointer client_data, XtPointer call_data)

{
  fileBoxUp = FALSE;
  fileBoxRsult = FALSE;
}

static void Nlm_FileMapCallback (Widget fs, XtPointer client_data, XtPointer call_data)

{
  Position   x, y;
  Dimension  w, h;

  XtVaGetValues (fs, XmNwidth, &w, XmNheight, &h, NULL);
  x = (Position) (Nlm_screenRect.right - w) / (Position) 2;
  y = (Position) (Nlm_screenRect.bottom - h) / (Position) 3;
  XtVaSetValues (fs, XmNx, x, XmNy, y, NULL);
}
#endif

extern Nlm_Boolean Nlm_GetInputFileName (Nlm_CharPtr fileName, Nlm_sizeT maxsize,
                                         Nlm_CharPtr extType, Nlm_CharPtr macType)

{
#ifdef WIN_MAC
  Nlm_Char       currentFileName [64];
  Nlm_Char       currentPath [256];
  SFTypeList     fTypeList;
  Nlm_Int2       i;
  Nlm_Int2       lengthTypes;
  Nlm_Int2       numTypes;
  Nlm_PointTool  ptool;
  Nlm_RecT       r;
  SFReply        reply;
  Nlm_Boolean    rsult;
  Nlm_RectTool   rtool;
  GrafPtr        tempPort;
  PenState       state;
  Nlm_Char       str [5];
  Nlm_PoinT      where;

  where.x = 90;
  where.y = 100;
  lengthTypes = sizeof (fileTypes);
  for (i = 0; i < lengthTypes; i++) {
    fileTypes [i] = '\0';
  }
#ifdef DCLAP
	/* allow several types in this list! */
  if (macType != NULL && macType [0] != '\0') {
    Nlm_StringNCpy (fileTypes, macType, lengthTypes);
    /*fileTypes [lengthTypes-1] = '\0';*/
  }
#else
  if (macType != NULL && macType [0] != '\0') {
    Nlm_StringNCpy (fileTypes, macType, 6);
    fileTypes [4] = '\0';
  }
#endif
  if (fileTypes [0] != '\0') {
    numTypes = 0;
    i = 0;
    while (numTypes < 4 && i < lengthTypes) {
      Nlm_StrngSeg (str, fileTypes, i, 4, sizeof (str));
      fTypeList [numTypes] = Nlm_GetOSType (str, '    ');
      numTypes++;
      i += 4;
    }
  } else {
    numTypes = -1;
  }
  GetPenState (&state);
  GetPort (&tempPort);
  Nlm_PoinTToPointTool (where, &ptool);
  SFGetFile (ptool, NULL, NULL, numTypes, fTypeList, NULL, &reply);
  SetPort (tempPort);
  SetPenState (&state);
  Nlm_GetRect ((Nlm_GraphiC) Nlm_desktopWindow, &r);
  Nlm_RecTToRectTool (&r, &rtool);
  ClipRect (&rtool);
  if (reply.good != 0 && fileName != NULL) {
    Nlm_StringNCpy (currentFileName, (Nlm_CharPtr) &(reply.fName),
                    sizeof (currentFileName));
    Nlm_PtoCstr (currentFileName);
    Nlm_GetFilePath (reply.vRefNum, currentPath, sizeof (currentPath));
    Nlm_StringNCat (currentPath, currentFileName,
                    sizeof (currentPath) - Nlm_StringLen (currentPath));
    Nlm_StringNCpy (fileName, currentPath, maxsize);
    rsult = TRUE;
  } else {
    rsult = FALSE;
  }
  Nlm_Update ();
  return rsult;
#endif
#ifdef WIN_MSWIN
  char  szDirName [256];
  char  szFile [256];
  char  szFileTitle [256];
  UINT  i;
  UINT  cbString;
  char  chReplace;
  char  szFilter [256];
  char  *lastSlash;

  GetModuleFileName (Nlm_currentHInst, szDirName, sizeof (szDirName));
  lastSlash = strrchr (szDirName, DIRDELIMCHR);
  if (lastSlash != NULL) {
    lastSlash [1] = '\0';
  }
  szFile [0] = '\0';
  if (extType != NULL && extType [0] != '\0') {
    Nlm_StringCpy (szFilter, "Filtered Files (*");
    if (extType [0] != '.') {
      Nlm_StringCat (szFilter, ".");
    }
    Nlm_StringNCat (szFilter, extType, 5);
    Nlm_StringCat (szFilter, ")|*");
    if (extType [0] != '.') {
      Nlm_StringCat (szFilter, ".");
    }
    Nlm_StringNCat (szFilter, extType, 5);
    Nlm_StringCat (szFilter, "|");
  } else {
    Nlm_StringCpy (szFilter, "All Files (*.*)|*.*|");
  }
  cbString = (UINT) Nlm_StringLen (szFilter);
  chReplace = szFilter [cbString - 1];
  for (i = 0; szFilter [i] != '\0'; i++) {
    if (szFilter [i] == chReplace) {
      szFilter [i] = '\0';
    }
  }
  memset (&ofn, 0, sizeof (OPENFILENAME));
  ofn.lStructSize = sizeof (OPENFILENAME);
  ofn.hwndOwner = Nlm_currentHWnd;
  ofn.lpstrFilter = szFilter;
  ofn.nFilterIndex = 1;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof (szFile);
  ofn.lpstrFileTitle = szFileTitle;
  ofn.nMaxFileTitle = sizeof (szFileTitle);
  ofn.lpstrInitialDir = szDirName;
  ofn.Flags = OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
  if (GetOpenFileName (&ofn) && fileName != NULL) {
    Nlm_StringNCpy (fileName, ofn.lpstrFile, maxsize);
    AnsiToOemBuff (fileName, fileName, maxsize);
    return TRUE;
  } else {
    return FALSE;
  }
#endif
#ifdef WIN_MOTIF
  XmString  dirmask;
  char      *lastSlash;
  char      str [256];
  char      *text;
  Widget    txt;

  Nlm_CreateFileDialogShell ();
  if (Nlm_fileDialogShell != NULL && fileName != NULL) {
    if (fileDialog == NULL) {
      fileDialog = XmCreateFileSelectionDialog (Nlm_fileDialogShell,
                                                (String) "file_selection", NULL, 0);
      XtAddCallback (fileDialog, XmNcancelCallback, Nlm_FileCancelCallback, NULL);
      XtAddCallback (fileDialog, XmNokCallback, Nlm_FileOkCallback, NULL);
      XtAddCallback (fileDialog, XmNnoMatchCallback, Nlm_FileNoMatchCallback, NULL);
      XtAddCallback (fileDialog, XmNmapCallback, Nlm_FileMapCallback, NULL);
      XtVaSetValues (fileDialog, XmNdefaultPosition, FALSE,
                     XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL, NULL);
    }
    if (fileDialog != NULL) {
      txt = XmFileSelectionBoxGetChild (fileDialog, XmDIALOG_FILTER_TEXT);
      text = XmTextGetString (txt);
      Nlm_StringNCpy (str, text, sizeof (str) - 1);
      lastSlash = Nlm_StringRChr (str, DIRDELIMCHR);
      if (lastSlash != NULL) {
        lastSlash [1] = '\0';
      } else {
        lastSlash = &(str [Nlm_StringLen (str) - 1]);
      }
      if (extType != NULL && extType [0] != '\0') {
        Nlm_StringCat (str, "*");
        if (extType [0] != '.') {
          Nlm_StringCat (str, ".");
        }
        Nlm_StringNCat (str, extType, 5);
        dirmask = XmStringCreateLtoR (str, XmSTRING_DEFAULT_CHARSET);
        XmFileSelectionDoSearch (fileDialog, dirmask);
        XmStringFree (dirmask);
        if (lastSlash != NULL) {
          lastSlash [1] = '\0';
        }
      }
      XtFree (text);
      txt = XmFileSelectionBoxGetChild (fileDialog, XmDIALOG_TEXT);
      XmTextSetString (txt, str);
      XtVaSetValues (fileDialog, XmNmustMatch, TRUE, NULL);
      fileBoxUp = TRUE;
      fileBoxRsult = FALSE;
      XtManageChild (fileDialog);
      XSync (Nlm_currentXDisplay, FALSE);
      while (fileBoxUp) {
        Nlm_ProcessAnEvent ();
      }
      if (fileBoxRsult) {
        Nlm_StringNCpy (fileName, filePath, maxsize);
      }
      XtUnmanageChild (fileDialog);
      XSync (Nlm_currentXDisplay, FALSE);
    }
  }
  return fileBoxRsult;
#endif
}

#ifdef WIN_MSWIN
static void Nlm_CopyDefaultName (Nlm_CharPtr dst, Nlm_CharPtr src)

{
  Nlm_Char  ch;
  Nlm_Int2  i;
  Nlm_Int2  j;
  Nlm_Int2  k;

  if (dst != NULL && src != NULL) {
    i = 0;
    j = 0;
    k = 0;
    ch = src [i];
    while (k < 8 && ch != '\0' && IS_ALPHANUM (ch)) {
      dst [j] = ch;
      i++;
      j++;
      k++;
      ch = src [i];
    }
    while (ch != '\0' && ch != '.') {
      i++;
      ch = src [i];
    }
    if (ch == '.') {
      dst [j] = ch;
      i++;
      j++;
      ch = src [i];
      k = 0;
      while (k < 3 && ch != '\0' && IS_ALPHANUM (ch)) {
        dst [j] = ch;
        i++;
        j++;
        k++;
        ch = src [i];
      }
    }
    dst [j] = '\0';
  }
}
#endif

extern Nlm_Boolean Nlm_GetOutputFileName (Nlm_CharPtr fileName, Nlm_sizeT maxsize,
                                          Nlm_CharPtr dfault)

{
#ifdef WIN_MAC
  Nlm_Char       currentFileName [64];
  Nlm_Char       currentPath [256];
  unsigned char  original [256];
  unsigned char  promptStr [256];
  Nlm_PointTool  ptool;
  Nlm_RecT       r;
  SFReply        reply;
  Nlm_Boolean    rsult;
  Nlm_RectTool   rtool;
  GrafPtr        tempPort;
  PenState       state;
  Nlm_PoinT      where;

  where.x = 90;
  where.y = 100;
  GetPenState (&state);
  GetPort (&tempPort);
  Nlm_PoinTToPointTool (where, &ptool);
  original [0] = '\0';
  Nlm_StringNCpy ((Nlm_CharPtr) original, dfault, sizeof (original));
  Nlm_CtoPstr ((Nlm_CharPtr) original);
  Nlm_StringNCpy ((Nlm_CharPtr) promptStr, "Save File As:", sizeof (promptStr));
  Nlm_CtoPstr ((Nlm_CharPtr) promptStr);
  SFPutFile (ptool, promptStr, original, NULL, &reply);
  SetPort (tempPort);
  SetPenState (&state);
  Nlm_GetRect ((Nlm_GraphiC) Nlm_desktopWindow, &r);
  Nlm_RecTToRectTool (&r, &rtool);
  ClipRect (&rtool);
  if (reply.good != 0 && fileName != NULL) {
    Nlm_StringNCpy (currentFileName, (Nlm_CharPtr) &(reply.fName),
                    sizeof (currentFileName));
    Nlm_PtoCstr (currentFileName);
    Nlm_GetFilePath (reply.vRefNum, currentPath, sizeof (currentPath));
    Nlm_StringNCat (currentPath, currentFileName,
                    sizeof (currentPath) - Nlm_StringLen (currentPath));
    Nlm_StringNCpy (fileName, currentPath, maxsize);
    rsult = TRUE;
  } else {
    rsult = FALSE;
  }
  Nlm_Update ();
  return rsult;
#endif
#ifdef WIN_MSWIN
  FILE  *f;
  char  szDirName [256];
  char  szFile [256];
  char  szFileTitle [256];
  UINT  i;
  UINT  cbString;
  char  chReplace;
  char  szFilter [256];
  char  *lastSlash;

  GetModuleFileName (Nlm_currentHInst, szDirName, sizeof (szDirName));
  lastSlash = strrchr (szDirName, DIRDELIMCHR);
  if (lastSlash != NULL) {
    lastSlash [1] = '\0';
  }
  szFile [0] = '\0';
  Nlm_CopyDefaultName ((Nlm_CharPtr) szFile, dfault);
  Nlm_StringCpy (szFilter, "All Files (*.*)|*.*|");
  cbString = (UINT) Nlm_StringLen (szFilter);
  chReplace = szFilter [cbString - 1];
  for (i = 0; szFilter [i] != '\0'; i++) {
    if (szFilter [i] == chReplace) {
      szFilter [i] = '\0';
    }
  }
  memset (&ofn, 0, sizeof (OPENFILENAME));
  ofn.lStructSize = sizeof (OPENFILENAME);
  ofn.hwndOwner = Nlm_currentHWnd;
  ofn.lpstrFilter = szFilter;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof (szFile);
  ofn.lpstrFileTitle = szFileTitle;
  ofn.nMaxFileTitle = sizeof (szFileTitle);
  ofn.lpstrInitialDir = szDirName;
  /*
  ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
  */
  /* OFN_OVERWRITEPROMPT causes crashes for some unknown reason */
  ofn.Flags = OFN_SHOWHELP;
  if (GetSaveFileName (&ofn) && fileName != NULL) {
    Nlm_StringNCpy (fileName, ofn.lpstrFile, maxsize);
    AnsiToOemBuff (fileName, fileName, maxsize);
    f = Nlm_FileOpen (fileName, "r");
    if (f != NULL) {
      Nlm_FileClose (f);
      if (Nlm_Message (MSG_YN, "Replace existing file?") == ANS_NO) {
        return FALSE;
      }
    }
    return TRUE;
  } else {
    return FALSE;
  }
#endif
#ifdef WIN_MOTIF
  char   *lastSlash;
  char    str [256];
  char    *text;
  Widget  txt;

  Nlm_CreateFileDialogShell ();
  if (Nlm_fileDialogShell != NULL && fileName != NULL) {
    if (fileDialog == NULL) {
      fileDialog = XmCreateFileSelectionDialog (Nlm_fileDialogShell,
                                                (String) "file_selection", NULL, 0);
      XtAddCallback (fileDialog, XmNcancelCallback, Nlm_FileCancelCallback, NULL);
      XtAddCallback (fileDialog, XmNokCallback, Nlm_FileOkCallback, NULL);
      XtAddCallback (fileDialog, XmNnoMatchCallback, Nlm_FileNoMatchCallback, NULL);
      XtAddCallback (fileDialog, XmNmapCallback, Nlm_FileMapCallback, NULL);
      XtVaSetValues (fileDialog, XmNdefaultPosition, FALSE,
                     XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL, NULL);
    }
    if (fileDialog != NULL) {
      txt = XmFileSelectionBoxGetChild (fileDialog, XmDIALOG_FILTER_TEXT);
      text = XmTextGetString (txt);
      Nlm_StringNCpy (str, text, sizeof (str) - 1);
      lastSlash = Nlm_StringRChr (str, DIRDELIMCHR);
      if (lastSlash != NULL) {
        lastSlash [1] = '\0';
      }
      XtFree (text);
      if (dfault != NULL) {
        Nlm_StringNCat (str, dfault, sizeof (str) - Nlm_StringLen (str));
      }
      txt = XmFileSelectionBoxGetChild (fileDialog, XmDIALOG_TEXT);
      XmTextSetString (txt, str);
      XtVaSetValues (fileDialog, XmNmustMatch, FALSE, NULL);
      fileBoxUp = TRUE;
      fileBoxRsult = FALSE;
      XtManageChild (fileDialog);
      while (fileBoxUp) {
        Nlm_ProcessAnEvent ();
      }
      if (fileBoxRsult) {
        Nlm_StringNCpy (fileName, filePath, maxsize);
      }
      XtUnmanageChild (fileDialog);
    }
  }
  return fileBoxRsult;
#endif
}

extern Nlm_Boolean Nlm_LaunchApp (Nlm_CharPtr fileName)

{
#ifdef WIN_MAC
  OSErr                err;
  FSSpec               fsspec;
  long                 gval;
  LaunchParamBlockRec  myLaunchParams;
  unsigned char        pathname [256];
  Nlm_CharPtr          ptr;
  Nlm_Boolean          rsult;

  rsult = FALSE;
  if (Gestalt (gestaltSystemVersion, &gval) == noErr && (short) gval >= 7 * 256) {
    if (fileName != NULL && fileName [0] != '\0') {
      if (Nlm_StringChr (fileName, DIRDELIMCHR) != NULL) {
        Nlm_StringNCpy ((Nlm_CharPtr) pathname, fileName, sizeof (pathname));
      } else {
        Nlm_ProgramPath ((Nlm_CharPtr) pathname, sizeof (pathname));
        ptr = Nlm_StringRChr ((Nlm_CharPtr) pathname, DIRDELIMCHR);
        if (ptr != NULL) {
          *ptr = '\0';
        }
        Nlm_FileBuildPath ((Nlm_CharPtr) pathname, NULL, fileName);
      }
      Nlm_CtoPstr ((Nlm_CharPtr) pathname);
      err = FSMakeFSSpec (0, 0, pathname, &(fsspec));
      myLaunchParams.launchBlockID = extendedBlock;
      myLaunchParams.launchEPBLength = extendedBlockLen;
      myLaunchParams.launchFileFlags = 0;
      myLaunchParams.launchControlFlags = launchContinue + launchNoFileFlags;
      myLaunchParams.launchAppParameters = NULL;
      myLaunchParams.launchAppSpec = &fsspec;
      rsult = (Nlm_Boolean) (LaunchApplication (&myLaunchParams) == noErr);
    }
  }
  return rsult;
#endif
#ifdef WIN_MSWIN
  Nlm_Char     ch;
  Nlm_Int2     i;
  Nlm_Int2     j;
  Nlm_Int2     k;
  char         lpszCmdLine [256];
  Nlm_Boolean  rsult;

  rsult = FALSE;
  if (fileName != NULL && fileName [0] != '\0') {
    Nlm_StringNCpy ((Nlm_CharPtr) lpszCmdLine, fileName, sizeof (lpszCmdLine));
    i = 0;
    j = 0;
    k = 0;
    ch = fileName [j];
    while (i < 8 && ch != '\0') {
      if (ch == '_' || IS_ALPHANUM (ch)) {
        lpszCmdLine [k] = ch;
        j++;
        k++;
        ch = fileName [j];
      }
      i++;
    }
    while (ch != '\0' && ch != '.') {
      j++;
      ch = fileName [j];
    }
    if (ch == '.') {
      lpszCmdLine [k] = ch;
      j++;
      k++;
      ch = fileName [j];
      i = 0;
      while (i < 3 && ch != '\0') {
        if (ch == '_' || IS_ALPHANUM (ch)) {
          lpszCmdLine [k] = ch;
          j++;
          k++;
          ch = fileName [j];
        }
        i++;
      }
    }
    if (k > 0 && lpszCmdLine [k - 1] == '.') {
      lpszCmdLine [k - 1] = '\0';
    }
    lpszCmdLine [k] = '\0';
    rsult = (Nlm_Boolean) (WinExec (lpszCmdLine, SW_SHOW) >= 32);
  }
  return rsult;
#endif
#ifdef WIN_MOTIF
  Nlm_Boolean  rsult;
  int          status;

  rsult = FALSE;
  if (fileName != NULL && fileName [0] != '\0') {
#ifdef OS_UNIX
    status = (int) fork ();
    if (status == 0) {
      execl (fileName, fileName, (char *) 0);
      Nlm_Message (MSG_FATAL, "Application launch failed");
    } else if (status != -1) {
      rsult = TRUE;
    }
#endif
#ifdef OS_VMS
    status = execl (fileName, fileName, (char *) 0);
    if ( status == -1 ) {
      Nlm_Message (MSG_FATAL, "Application launch failed");
    } else {
      rsult = TRUE;
    }
#endif
  }
  return rsult;
#endif
}

/* ++dgg -- added by d.gilbert, dec'93, for use w/ DCLAP c++ library */

extern void Nlm_SetObject (Nlm_GraphiC a, Nlm_VoidPtr thisobject)
{
  Nlm_GraphicData  gdata;
  if (a != NULL) { /* dgg - removed test for thisobject != NULL -- need to set null */
    Nlm_GetGraphicData (a, &gdata);
    gdata.thisobject = thisobject;
    Nlm_SetGraphicData (a, &gdata);
  }
}

extern Nlm_VoidPtr Nlm_GetObject (Nlm_GraphiC a)
{
  Nlm_GraphicData  gdata;
  if (a != NULL) {
    Nlm_GetGraphicData (a, &gdata);
    return gdata.thisobject;
  } else {
    return NULL;
  }
}

