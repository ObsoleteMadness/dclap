/*   chosen.c
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
* File Name:  chosen.c
*
* Author:  Kans, Schuler
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.19 $
*
* File Description: 
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#include <vibrant.h>
#include <panels.h>
#include <accentr.h>
#include <accutils.h>
#include "browser.h"

#define STATE_NONE       0
#define STATE_SPECIAL    1
#define STATE_TOTAL      2
#define STATE_INACTIVE  -1

typedef struct {
  Char  term [128];
  Int2  database;
  Int2  field;
  Int2  state;
  Int2  group;
  Int4  count1;
  Int4  count2;
} TrmInf, PNTR TrmInfPtr;

static LinkSetPtr  mergeLSP;

static SlatE       chosenSlate;
static TablE       chosen;
static PalettE     bracket;
static PaneL       panel;
static ButtoN      retrieve;
static ButtoN      resetBtn;
static Int2        db;
static Int2        chosenHeight;

static ButtoN      advancedboolBtn;
static WindoW      advancedboolWin;
static TexT        advancedboolText;
static ButtoN      pasteFromBtn;
static ButtoN      retrieveAdv;
static ButtoN      evalAdvBtn;
static LinkSetPtr  advLSP = NULL;

static Int2        clickedRow;
static RecT        trackRect;
static PoinT       trackPt;
static Boolean     frameInsideRect;
static Boolean     notFirstDrag;
static Boolean     groupRange;
static Boolean     postingLoadFailed;

/* Decode a line from chosen list */

static void SetTrmInf (Int2 nRow, TrmInfPtr tPtr)

{
  Int2    i;
  Char    str [32];
  TrmInf  T;

  i = nRow;
  if (i > 0 && tPtr != NULL) {
    if (GetTableHilight (chosen, i, 2)) {
      T.state = STATE_SPECIAL;
    } else if (GetTableHilight (chosen, i, 3)) {
      T.state = STATE_TOTAL;
    } else if (GetTableGray (chosen, i, 3)) {
      T.state = STATE_INACTIVE;
    } else {
      T.state = STATE_NONE;
    }
    T.group = GetPaletteValue (bracket, i);
    GetTableText (chosen, i, 1, T.term, sizeof (T.term));
    GetTableText (chosen, i, 2, str, sizeof (str));
    StrToLong (str, &(T.count1));
    GetTableText (chosen, i, 3, str, sizeof (str));
    StrToLong (str, &(T.count2));
    GetTableText (chosen, i, 4, str, sizeof (str));
    StrToInt (str, &(T.database));
    db = T.database;
    GetTableText (chosen, i, 5, str, sizeof (str));
    StrToInt (str, &(T.field));
    *tPtr = T;
  }
}

/* Format string describing boolean of several record lists */

static Int4 FormatBoolean (CharPtr userString, Int4 maxLen, Int2 nTerms)

{
  Int2        group;
  Int2        i;
  Int2        last;
  Boolean     special;
  TrmInf      T;
  Int4        termLen;
  Int4        newidx;
  CharPtr     modTerm;
  CharPtr     fldStr;
  CharPtr     tmpStr;
  Int2        j;
  Int2        termsInvolved = 0;

  group = 0;
  last = 0;
  SetTrmInf (1, &T);
  userString[0] = '\0';
  for (i = 1; i <= nTerms; i++) {
    SetTrmInf (i, &T);
    if (T.group == GROUP_SINGLE || T.group == GROUP_FIRST) {
      group++;
    }
    if (T.state > 0) {
      if (last == 0) {
        StrCat(userString, "( ");
      } else if (last == group) {
        StrCat(userString, " | ");
      } else {
        StrCat(userString, " )");
        StrCat(userString, " & ");
        StrCat(userString, "( ");
      }
      if (T.state == STATE_SPECIAL) {
        special = TRUE;
      } else {
        special = FALSE;
      }
      termsInvolved++;
      termLen = StrLen(T.term);
      modTerm = (CharPtr) MemNew((size_t) termLen * 2);
      for (j = 0, newidx = 0; j < termLen; j++, newidx++)
      {
        if (T.term[j] == '"' || T.term[j] == '\\')
        {
          modTerm[newidx++] = '\\';
        }
        modTerm[newidx] = T.term[j];
      }
      modTerm[newidx] = '\0';
      fldStr = EntrezFieldToString(T.database, T.field);
      sprintf(&userString[StrLen(userString)], "\"%s\" [%s%s]", modTerm, fldStr,
              special ? ",S" : "");
      MemFree (fldStr);
      MemFree (modTerm);
      last = group;
    }
  }
  if (group > 0 && last > 0) {
    StrCat(userString, " )");
  }
  if (termsInvolved > 1)
  { /* add another set of parens, to be on the safe side */
    tmpStr = StringSave(userString);
    sprintf (userString, "( %s )", tmpStr);
    MemFree (tmpStr);
  }
  return StrLen(userString);
}

static Int4 MakeList (Int2 nTerms)

{
  ValNodePtr  elst;
  Int2        group;
  Int2        i;
  Int2        last;
  Int4        nCount;
  Boolean     special;
  TrmInf      T;

  if (mergeLSP != NULL) {
    mergeLSP = LinkSetFree (mergeLSP);
  }
  postingLoadFailed = FALSE;
  nCount = 0;
  group = 0;
  last = 0;
  threads++;
  SetTrmInf (1, &T);
  elst = EntrezTLNew (T.database);
  for (i = 1; i <= nTerms; i++) {
    SetTrmInf (i, &T);
    if (T.group == GROUP_SINGLE || T.group == GROUP_FIRST) {
      group++;
    }
    if (T.state > 0) {
      if (last == 0) {
        EntrezTLAddLParen (elst);
      } else if (last == group) {
        EntrezTLAddOR (elst);
      } else {
        EntrezTLAddRParen (elst);
        EntrezTLAddAND (elst);
        EntrezTLAddLParen (elst);
      }
      if (T.state == STATE_SPECIAL) {
        special = TRUE;
      } else {
        special = FALSE;
      }
      EntrezTLAddTerm (elst, T.term, T.database, T.field, special);
      last = group;
    }
  }
  if (group > 0 && last > 0) {
    EntrezTLAddRParen (elst);
  }
  mergeLSP = EntrezTLEval (elst);
  if (mergeLSP != NULL) {
    nCount = mergeLSP->num;
  }
  EntrezTLFree (elst);
  threads--;
  return nCount;
}

/* Find intersection, display in Load button */

extern void InsertList (void)

{
  Int2  brkt;
  Int4  count;
  Int4  max;
  Int2  num;
  Int2  row;
  Char  str1 [64];
  Char  str2 [32];

  num = TableNumLines (chosen);
  brkt = GetPaletteValue (bracket, num);
  if (brkt == GROUP_FIRST) {
    SetPaletteValue (bracket, num, GROUP_SINGLE);
  } else if (brkt == GROUP_MIDDLE) {
    SetPaletteValue (bracket, num, GROUP_LAST);
  }
  count = MakeList (TableNumLines (chosen));
  if (postingLoadFailed) {
    num = TableNumLines (chosen);
    for (row = 1; row <= num; row++) {
      SetTableHilight (chosen, row, 2, FALSE);
      SetTableHilight (chosen, row, 3, FALSE);
    }
    SetTitle (retrieve, "Retrieve 0 Documents");
    Disable (retrieve);
    Disable (pasteFromBtn);
  } else {
    StrngCpy (str1, "Retrieve ", sizeof (str1));
    LongToStr (count, str2, 1, sizeof (str2));
    StrngCat (str1, str2, sizeof (str1));
    StrngCat (str1, " Document", sizeof (str1));
    if (count != 1) {
      StrngCat (str1, "s", sizeof (str1));
    }
    SetTitle (retrieve, str1);
    max = 200;
    if (GetAppParam ("ENTREZ", "PREFERENCES", "MAXLOAD", "", str2, sizeof (str2)-1)) {
      if (! StrToLong (str2, &max)) {
        max = 200;
      }
    }
    if (count > 0 && count <= max && count <= 8192) {
      Enable (retrieve);
    } else {
      Disable (retrieve);
    }
    Enable (pasteFromBtn);
  }
}

static void DrawMergeControl (PalettE p, Int2 num, RectPtr r)

{
  Int2  value;

  if (r != NULL) {
    value = GetPaletteValue (p, num);
    switch (value) {
      case GROUP_SINGLE:
        MoveTo (r->left + 1, r->top + 1);
        LineTo (r->left + 3, r->top + 1);
        MoveTo (r->left + 1, r->top + 2);
        LineTo (r->left + 3, r->top + 2);
        MoveTo (r->left + 1, r->top + 1);
        LineTo (r->left + 1, r->bottom - 1);
        MoveTo (r->left + 1, r->bottom - 2);
        LineTo (r->left + 3, r->bottom - 2);
        MoveTo (r->left + 1, r->bottom - 1);
        LineTo (r->left + 3, r->bottom - 1);
        break;
      case GROUP_FIRST:
        MoveTo (r->left + 1, r->top + 1);
        LineTo (r->left + 3, r->top + 1);
        MoveTo (r->left + 1, r->top + 2);
        LineTo (r->left + 3, r->top + 2);
        MoveTo (r->left + 1, r->top + 1);
        LineTo (r->left + 1, r->bottom);
        break;
      case GROUP_MIDDLE:
        MoveTo (r->left + 1, r->top);
        LineTo (r->left + 1, r->bottom);
        break;
      case GROUP_LAST:
        MoveTo (r->left + 1, r->top);
        LineTo (r->left + 1, r->bottom - 1);
        MoveTo (r->left + 1, r->bottom - 2);
        LineTo (r->left + 3, r->bottom - 2);
        MoveTo (r->left + 1, r->bottom - 1);
        LineTo (r->left + 3, r->bottom - 1);
        break;
      default:
        break;
    }
  }
}

static void FindBracketGroupLimits (Int2Ptr firstPtr, Int2Ptr lastPtr)

{
  Int2  first;
  Int2  last;
  Int2  numLines;

  numLines = TableNumLines (chosen);
  first = *firstPtr;
  last = *lastPtr;
  if (GetPaletteValue (bracket, first) != GROUP_SINGLE) {
    while (first > 0 && GetPaletteValue (bracket, first) != GROUP_FIRST) {
      first--;
    }
  }
  if (first < 1) {
    first = 1;
  }
  if (GetPaletteValue (bracket, last) != GROUP_SINGLE) {
    while (last <= numLines && GetPaletteValue (bracket, last) != GROUP_LAST) {
      last++;
    }
  }
  if (last > numLines) {
    last = numLines;
  }
  *firstPtr = first;
  *lastPtr = last;
}

static void FrameTrackRect (void)

{
  RecT  sr;

  ObjectRect (chosenSlate, &sr);
  InsetRect (&sr, 4, 4);
  if (RectInRect (&trackRect, &sr) && (notFirstDrag)) {
    InvertMode ();
    Dotted ();
    FrameRect (&trackRect);
    Solid ();
    CopyMode ();
  }
}

static void PanelClick (PaneL p, PoinT pt)

{
  Boolean  goOn;
  Int2     numLines;
  Int2     off;
  RecT     r;
  Int2     row;
  RecT     sr;

  groupRange = shftKey;
  numLines = TableNumLines (chosen);
  ObjectRect (chosenSlate, &sr);
  InsetRect (&sr, 4, 4);
  GetOffset (p, NULL, &off);
  row = 0;
  goOn = TRUE;
  while (row < numLines && goOn) {
    r.left = sr.left;
    r.top = sr.top + chosenHeight * row;
    if (smallScreen) {
      r.right = r.left + 19 * stdCharWidth;
    } else {
      r.right = r.left + 24 * stdCharWidth;
    }
    r.bottom = r.top + chosenHeight;
    OffsetRect (&r, 0, -off);
    if (RectInRect (&r, &sr) && PtInRect (pt, &r)) {
      goOn = FALSE;
    } else {
      row++;
    }
  }
  row++;
  frameInsideRect = FALSE;
  if (row <= numLines) {
    clickedRow = row;
    InsetRect (&r, 5, 0);
    trackRect = r;
    trackPt = pt;
    SetTableHilight (chosen, clickedRow, 1, TRUE);
    frameInsideRect = TRUE;
    notFirstDrag = FALSE;
  }
}

static void PanelDrag (PaneL p, PoinT pt)

{
  Int2  off;
  RecT  sr;

  GetOffset (chosenSlate, NULL, &off);
  ObjectRect (chosenSlate, &sr);
  InsetRect (&sr, 4, 4);
  if (smallScreen) {
    sr.right = sr.left + 19 * stdCharWidth;
  } else {
    sr.right = sr.left + 24 * stdCharWidth;
  }
  if (frameInsideRect) {
    FrameTrackRect ();
  }
  notFirstDrag = TRUE;
  if (PtInRect (pt, &sr)) {
    OffsetRect (&trackRect, 0, pt.y - trackPt.y);
    trackPt = pt;
  }
  if (pt.y < sr.top) {
    SetOffset (chosenSlate, 0, off - 1);
    frameInsideRect = FALSE;
  } else if (pt.y > sr.bottom) {
    SetOffset (chosenSlate, 0, off + 1);
    frameInsideRect = FALSE;
  } else if (PtInRect (pt, &sr) && RectInRect (&trackRect, &sr)) {
    frameInsideRect = TRUE;
  } else {
    frameInsideRect = FALSE;
  }
  if (frameInsideRect) {
    FrameTrackRect ();
  }
}

static Int2 NumGroups (void)

{
  Int2  group;
  Int2  i;
  Int2  numLines;

  group = 0;
  numLines = TableNumLines (chosen);
  for (i = 1; i <= numLines; i++) {
    switch (GetPaletteValue (bracket, i)) {
      case GROUP_SINGLE:
        group++;
        break;
      case GROUP_FIRST:
        group++;
        break;
      case GROUP_MIDDLE:
        break;
      case GROUP_LAST:
        break;
      default:
        break;
    }
  }
  return group;
}

static Int2Ptr GetGroups (void)

{
  Int2     count;
  Int2Ptr  groupPtr;
  Int2     group;
  Int2     i;
  Int2     numLines;

  numLines = TableNumLines (chosen);
  groupPtr = (Int2Ptr) MemNew ((NumGroups () + 4) * sizeof (Int2));
  group = 0;
  count = 0;
  for (i = 1; i <= numLines; i++) {
    switch (GetPaletteValue (bracket, i)) {
      case GROUP_SINGLE:
        count = 1;
        groupPtr [group] = count;
        group++;
        break;
      case GROUP_FIRST:
        count = 1;
        break;
      case GROUP_MIDDLE:
        count++;
        break;
      case GROUP_LAST:
        count++;
        groupPtr [group] = count;
        group++;
        break;
      default:
        break;
    }
  }
  return groupPtr;
}

static Int2 InWhichGroup (Int2Ptr groupPtr, Int2 row)

{
  Int2  count;
  Int2  group;
  Int2  numGroups;

  group = 0;
  count = 0;
  numGroups = NumGroups ();
  while (group <= numGroups && count < row) {
    count += groupPtr [group];
    if (count < row) {
      group ++;
    }
  }
  return group;
}

static void LoadTheChosenList (CharPtr selection, Int2 colToHilight,
                               Int2 palVal, Boolean newEntry,
                               Int2 fld)

{
  Int2     field;
  Boolean  go;
  Int2     i;
  Int2     len;
  Int2     lines;
  Int2     num;
  RecT     r;
  Char     seg [128];
  Char     str [128];
  CharPtr  temp;
  Int2     vis;

  StrngCpy (str, selection, sizeof (str));
  len = (Int2) StringLen (str);
  for (i = 0; i < len; i++) {
    if (str [i] == '\t') {
      str [i] = '\0';
    }
  }
  go = TRUE;
  num = TableNumLines (chosen);
  i = 1;
  if (newEntry) {
    while (i <= num && go) {
      GetTableText (chosen, i, 1, seg, sizeof (seg));
      if (StrngEql (str, seg, TRUE)) {
        GetTableText (chosen, i, 5, seg, sizeof (seg));
        if (StrToInt (seg, &field) && field == fld) {
          go = FALSE;
        }
      }
      i++;
    }
  }
  if (go) {
    temp = (CharPtr) MemNew (1024);
    StrngCpy (temp, selection, 1024);
    ObjectRect (chosenSlate, &r);
    InsetRect (&r, 4, 4);
    lines = TableNumLines (chosen);
    r.top += lines * chosenHeight;
    if (smallScreen) {
      r.right = r.left + 19 * stdCharWidth;
    } else {
      r.right = r.left + 24 * stdCharWidth;
    }
    r.bottom = r.top + chosenHeight;
    RegisterRect (panel, &r);
    AppendPalette (bracket, &r, DrawMergeControl, NULL, palVal);
    AppendTableText (chosen, temp);
    RegisterRow (chosenSlate, r.top + chosenHeight, chosenHeight, 1);
    MemFree (temp);
    num = TableNumLines (chosen);
    vis = TableVisLines (chosen);
    GetTableText (chosen, num, 2, str, sizeof (str));
    if (StrngEql (str, "0", FALSE)) {
      SetTableGray (chosen, num, 2, TRUE);
    }
    SetTableHilight (chosen, num, colToHilight, TRUE);
    if (num + 1 > vis) {
      SetOffset (chosenSlate, 0, num + 1 - vis);
    }
  }
}

typedef CharPtr PNTR LinePtr;

static void PanelRelease (PaneL p, PoinT pt)

{
  Boolean  changed;
  Int2     compare;
  Int2     count;
  Int2     first;
  Int2Ptr  groupPtr;
  Int2Ptr  hilightPtr;
  Int2     i;
  Int2     j;
  Int2     k;
  Int2     l;
  Int2     last;
  LinePtr  linePtr;
  Int2     m;
  Int2     margin;
  Int2     numLines;
  Int2     off;
  Int2Ptr  orderPtr;
  Int2     row;
  RecT     sr;
  Char     str1 [128];
  Char     str2 [128];

#ifndef PRODUCE_USERS_GUIDE
  changed = FALSE;
  numLines = TableNumLines (chosen);
  ObjectRect (chosenSlate, &sr);
  InsetRect (&sr, 4, 4);
  if (frameInsideRect) {
    FrameTrackRect ();
    frameInsideRect = FALSE;
  }
  SetTableHilight (chosen, clickedRow, 1, FALSE);
  if (PtInRect (pt, &sr)) {
    WatchCursor ();
    GetOffset (chosenSlate, NULL, &off);
    row = (pt.y - sr.top) / chosenHeight + 1 + off;
    if (groupRange) {
      if (row <= numLines) {
        first = MIN (row, clickedRow);
        last = MAX (row, clickedRow);
        FindBracketGroupLimits (&first, &last);
        margin = 0;
        for (i = first; i <= last; i++) {
          switch (GetPaletteValue (bracket, i)) {
            case GROUP_SINGLE:
              margin += 2;
              break;
            case GROUP_FIRST:
              margin++;
              break;
            case GROUP_MIDDLE:
              break;
            case GROUP_LAST:
              margin++;
              break;
            default:
              break;
          }
        }
        if (margin > 2 && (margin <= 4 || groupRange) && first < last) {
          for (i = first + 1; i <= last - 1; i++) {
            SetPaletteValue (bracket, i, GROUP_MIDDLE);
          }
          SetPaletteValue (bracket, first, GROUP_FIRST);
          SetPaletteValue (bracket, last, GROUP_LAST);
          changed = TRUE;
        }
      }
      groupPtr = GetGroups ();
    } else {
      groupPtr = GetGroups ();
      if (row <= numLines) {
        first = InWhichGroup (groupPtr, clickedRow);
        last = InWhichGroup (groupPtr, row);
      } else {
         first = InWhichGroup (groupPtr, clickedRow);
         last = NumGroups ();
      }
      if (first != last) {
        changed = TRUE;
      }
    }
    if (changed) {
      hilightPtr = (Int2Ptr) MemNew ((numLines + 2) * sizeof (Int2));
      orderPtr = (Int2Ptr) MemNew ((numLines + 2) * sizeof (Int2));
      linePtr = (LinePtr) MemNew ((numLines + 2) * sizeof (CharPtr));
      (groupPtr [first])--;
      (groupPtr [last])++;
      for (i = 1; i <= numLines; i++) {
        if (GetTableHilight (chosen, i, 2)) {
          hilightPtr [i - 1] = 2;
        } else if (GetTableHilight (chosen, i, 3)) {
          hilightPtr [i - 1] = 3;
        }
      }
      for (i = 1; i <= numLines; i++) {
        orderPtr [i] = i;
        k = (Int2) TableTextLength (chosen, i, 0);
        linePtr [i] = (CharPtr) MemNew (k + 2);
        GetTableText (chosen, i, 0, linePtr [i], k + 1);
      }
      if (row <= numLines) {
        if (clickedRow > row) {
          for (j = row + 2; j <= clickedRow; j++) {
            orderPtr [j] = j - 1;
          }
          orderPtr [row + 1] = clickedRow;
        } else if (clickedRow < row) {
          for (j = clickedRow; j < row; j++) {
            orderPtr [j] = j + 1;
          }
          orderPtr [row] = clickedRow;
        }
      } else {
        for (j = clickedRow; j < numLines; j++) {
          orderPtr [j] = j + 1;
        }
        orderPtr [numLines] = clickedRow;
      }
      i = 1;
      k = 0;
      while (i <= numLines) {
        count = groupPtr [k];
        k++;
        for (j = i; j < i + count - 1; j++) {
          for (l = j + 1; l < i + count; l++) {
            GetTableText (chosen, orderPtr [j], 1, str1, sizeof (str1));
            GetTableText (chosen, orderPtr [l], 1, str2, sizeof (str2));
            compare = MeshStringICmp (str1, str2);
            if (compare > 0) {
              m = orderPtr [j];
              orderPtr [j] = orderPtr [l];
              orderPtr [l] = m;
            }
          }
        }
        i += count;
      }
      ResetChosenList ();
      Disable (chosenSlate);
      i = 1;
      k = 0;
      while (i <= numLines) {
        count = groupPtr [k];
        k++;
        if (count == 1) {
          j = hilightPtr [orderPtr [i] - 1];
          LoadTheChosenList (linePtr [orderPtr [i]], j, GROUP_SINGLE, FALSE, 0);
          i++;
        } else if (count > 1) {
          j = hilightPtr [orderPtr [i] - 1];
          LoadTheChosenList (linePtr [orderPtr [i]], j, GROUP_FIRST, FALSE, 0);
          i++;
          count--;
          while (count > 1) {
            j = hilightPtr [orderPtr [i] - 1];
            LoadTheChosenList (linePtr [orderPtr [i]], j, GROUP_MIDDLE, FALSE, 0);
            i++;
            count--;
          }
          j = hilightPtr [orderPtr [i] - 1];
          LoadTheChosenList (linePtr [orderPtr [i]], j, GROUP_LAST, FALSE, 0);
          i++;
        }
      }
      SetOffset (chosenSlate, 0, off);
      Enable (chosenSlate);
      for (i = 1; i <= numLines; i++) {
        if (linePtr [i] != NULL) {
          MemFree (linePtr [i]);
        }
      }
      if (linePtr != NULL) {
        MemFree (linePtr);
      }
      if (orderPtr != NULL) {
        MemFree (orderPtr);
      }
      if (hilightPtr != NULL) {
        MemFree (hilightPtr);
      }
    }
    if (groupPtr != NULL) {
      MemFree (groupPtr);
    }
    if (changed) {
      InsertList ();
    }
    ArrowCursor ();
  }
#endif
}

/* Accept choice, place into chosen list */

extern void LoadChosenList (CharPtr selection, Int2 fld,
                            Int2 group, Boolean autoLoad)

{
  if (autoLoad) {
    LoadTheChosenList (selection, 3, group, TRUE, fld);
  } else {
    LoadTheChosenList (selection, 0, group, TRUE, fld);
  }
}

static void RetrieveDocsProc (ButtoN b)

{
  Int2       i, j;
  Int2       num;
  DocUidPtr  ptr;
  DocUid     tmp;
  DocUidPtr  uids;
  LinkSetPtr PNTR lspp = &mergeLSP;

  if (b == retrieveAdv) {
    lspp = &advLSP;
  }

  if (threads == 0) {
    WatchCursor ();
    SetTitle (b, "Retrieve 0 Documents");
    Disable (b);
    Disable (pasteFromBtn);
    num = (Int2) (*lspp)->num;
    ptr = (DocUidPtr) MemNew (num * sizeof (DocUid));
    if (ptr != NULL && num <= 8192 && (*lspp) != NULL) {
      uids = (*lspp)->uids;
      if (uids != NULL) {
        for (i = 0; i < num; i++) {
          ptr [i] = uids [i];
        }
      } else {
        Message (MSG_ERROR, "Uids NULL");
      }
      (*lspp) = LinkSetFree ((*lspp));
      if (db == TYP_ML) {
        i = 0;
        j = num - 1;
        while (i < j) {
          tmp = ptr [i];
          ptr [i] = ptr [j];
          ptr [j] = tmp;
          i++;
          j--;
        }
      }
#ifdef WIN_MOTIF
      Select (term);
#endif
      RetrieveDocuments (num, ptr, NULL, db);
      MemFree (ptr);
    }
    num = TableNumLines (chosen);
    SetTableBlockHilight (chosen, 1, num, 2, 3, FALSE);
    if ((*lspp) != NULL) {
      (*lspp) = LinkSetFree ((*lspp));
    }
    ArrowCursor ();
  }
}

/* Respond to click in chosen list */

static void ChosenAction (TablE t, Int2 row, Int2 col)

{
  Boolean  changed;
  Char     str [32];
  Int4     val;

  changed = FALSE;
  if (GetTableHilight (chosen, row, col)) {
    SetTableHilight (chosen, row, col, FALSE);
    changed = TRUE;
  } else {
    GetTableText (chosen, row, col, str, sizeof (str));
    if (StrToLong (str, &val) && val > 0) {
      SetTableHilight (chosen, row, col, TRUE);
      changed = TRUE;
    }
    if (GetTableHilight (chosen, row, 5 - col)) {
      SetTableHilight (chosen, row, 5 - col, FALSE);
      changed = TRUE;
    }
  }
  if (changed) {
    WatchCursor ();
    InsertList ();
    ArrowCursor ();
  }
}


static void advBoolActn (TexT t)
{
  short erract;
  ErrDesc err;
  Int2 curstrLen;
  CharPtr curstr;
  Int2 db;

  db = ReturnDatabaseCode ();
  curstr = MemNew((curstrLen = TextLength(advancedboolText)) + 1);
  GetTitle(advancedboolText, curstr, curstrLen + 1);

  ErrGetOpts(&erract, NULL);
  ErrSetOpts(ERR_IGNORE, 0);
  ErrFetch(&err);
  Disable (retrieveAdv);
  if (EntrezTLParseString(curstr, db, -1, NULL, NULL)) {
    Enable(evalAdvBtn);
  } else {
    Disable(evalAdvBtn);
  }
  ErrSetOpts(erract, 0);
  ErrFetch(&err);
  MemFree(curstr);
}

static void doneAdvProc (ButtoN b)
{
  Hide (advancedboolWin);
}

static void clrAdvancedProc (ButtoN b)

{
   SetTitle (advancedboolText, "");
   advBoolActn (NULL);
}

static Boolean
InsertAdvText(CharPtr str)

{
  CharPtr curstr;
  CharPtr finalstr;
  Boolean retval = TRUE;
  Int2 begin;
  Int2 end;
  Int2 curstrLen;
  Int2 strLen;

  curstr = MemNew((curstrLen = TextLength(advancedboolText)) + 1);
  GetTitle(advancedboolText, curstr, curstrLen + 1);
  if (curstrLen == 0)
  {
    begin = 0;
    end = 0;
  } else {
    TextSelectionRange(advancedboolText, &begin, &end);
  }

  if (begin >= 0 && end <= curstrLen && begin <= end)
  {
    strLen = StrLen(str);
    finalstr = MemNew(strLen + curstrLen + begin - end + 1);
    StrNCpy(finalstr, curstr, begin);
    StrCpy(&finalstr[begin], str);
    StrCat(finalstr, &curstr[end]);
    SetTitle(advancedboolText, finalstr);
    MemFree(finalstr);
    SelectText(advancedboolText, begin + strLen, begin + strLen);
    advBoolActn(NULL);
  } else {
    SelectText(advancedboolText, curstrLen, curstrLen);
    Beep();
    retval = FALSE;
  }

  MemFree(curstr);

  return retval;
}

static void pasteFromProc (ButtoN b)

{
  Int4 strLen;
  Int2 num = TableNumLines (chosen);
  CharPtr str = NULL;

  str = MemNew(1000);
  strLen = FormatBoolean(str, 1000 - 1, num);

  InsertAdvText(str);
  MemFree(str);
}

static void evalAdvProc (ButtoN b)

{
  Int2 curstrLen;
  CharPtr curstr;
  Int4  count;
  Int4  max;
  Char  str1 [64];
  Char  str2 [32];
  Int2 db;

  curstr = MemNew((curstrLen = TextLength(advancedboolText)) + 1);
  GetTitle(advancedboolText, curstr, curstrLen + 1);
  db = ReturnDatabaseCode ();
  WatchCursor ();
  advLSP = EntrezTLEvalString(curstr, db, -1, NULL, NULL);
  ArrowCursor ();

  if (advLSP != NULL)
  {
    count = advLSP->num;
    StrngCpy (str1, "Retrieve ", sizeof (str1));
    LongToStr (count, str2, 1, sizeof (str2));
    StrngCat (str1, str2, sizeof (str1));
    StrngCat (str1, " Document", sizeof (str1));
    if (count != 1) {
      StrngCat (str1, "s", sizeof (str1));
    }
    SetTitle (retrieveAdv, str1);
    max = 200;
    if (GetAppParam ("ENTREZ", "PREFERENCES", "MAXLOAD", "", str2, sizeof (str2)-1)) {
      if (! StrToLong (str2, &max)) {
        max = 200;
      }
    }
    
    if (count > 0 && count <= max && count <= 8192) {
      Enable (retrieveAdv);
    } else {
      Disable (retrieveAdv);
    }
  } else {
    SetTitle (retrieveAdv, "Retrieve 0 Documents");
    Disable (retrieveAdv);
  }
  return;
}

static void leftParenProc (ButtoN b)

{
  InsertAdvText(" ( ");
}

static void rightParenProc (ButtoN b)

{
  InsertAdvText(" ) ");
}

static void andProc (ButtoN b)

{
  InsertAdvText(" & ");
}

static void orProc (ButtoN b)

{
  InsertAdvText(" | ");
}

static void butNotProc (ButtoN b)

{
  InsertAdvText(" - ");
}

static void AdvancedBoolProc (ButtoN b) 
 
{ 
  Show(advancedboolWin);
  Select(advancedboolWin);
} 

static void ResetProc (ButtoN b)

{
  ResetState ();
  Hide (browseWindow);
  Hide(advancedboolWin);
  Disable (saveAllItem);
  Disable (saveNameItem);
#ifdef WIN_MOTIF
  Select (term);
#endif
}

static void RecreateChosen (void)

{
  RecT    r;
  WindoW  tempPort;
  Int2    wid;

  if (smallScreen) {
    wid = 19;
  } else {
    wid = 24;
  }
  RecordColumn (chosen, wid, 'l', FALSE, FALSE, NULL);       /* terms          */
  RecordColumn (chosen, 4, 'r', FALSE, TRUE, ChosenAction);  /* special count  */
  RecordColumn (chosen, 5, 'r', FALSE, TRUE, ChosenAction);  /* total count    */
  RecordColumn (chosen, 0, 'x', FALSE, FALSE, NULL);         /* database       */
  RecordColumn (chosen, 0, 'x', FALSE, FALSE, NULL);         /* field          */
  ObjectRect (chosenSlate, &r);
  InsetRect (&r, 4, 4);
  RegisterRect ((PaneL) chosen, &r);
  if (Visible (chosen) && AllParentsVisible (chosen)) {
    tempPort = SavePort (chosen);
    Select (chosen);
    InvalRect (&r);
    RestorePort (tempPort);
  }
}

extern void ResetChosenList (void)

{
  Reset (chosenSlate);
  RecreateChosen ();
  Hide (advancedboolWin);
  clrAdvancedProc (NULL);
}

extern void ResetRetrieveButton (void)

{
  SetTitle (retrieve, "Retrieve 0 Documents");
  Disable (retrieve);
  Disable (pasteFromBtn);
}

static void ActivateChosenProc (WindoW w)

{
  docActive = FALSE;
  Disable (openItem);
  Disable (saveItem);
  Disable (saveAsItem);
  Disable (printItem);
}

static Uint1  upArrow [] = {
    0x08, 0x00, 0x1C, 0x00,
    0x3E, 0x00, 0x7F, 0x00,
    0x1C, 0x00, 0x1C, 0x00,
    0x1C, 0x00, 0x1C, 0x00,
    0x1C, 0x00, 0x1C, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
  };

static Uint1  downArrow [] = {
    0x1C, 0x00, 0x1C, 0x00,
    0x1C, 0x00, 0x1C, 0x00,
    0x1C, 0x00, 0x1C, 0x00,
    0x7F, 0x00, 0x3E, 0x00,
    0x1C, 0x00, 0x08, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
  };

static void DrawUpArrow (PaneL p)

{
  RecT  r;

  ObjectRect (p, &r);
  r.top = (r.top + r.bottom) / 2 - 5;
  r.bottom = r.top + 10;
  CopyBits (&r, upArrow);
}

static void DrawDownArrow (PaneL p)

{
  RecT  r;

  ObjectRect (p, &r);
  r.top = (r.top + r.bottom) / 2 - 5;
  r.bottom = r.top + 10;
  CopyBits (&r, downArrow);
}

extern void CreateChosenList (WindoW w)

{
  GrouP  g;
  PoinT  npt;
  FonT   fnt;

  g = HiddenGroup (w, 8, 0, NULL);
  SetGroupMargins (g, 4, 0);
  if (smallScreen) {
    SimplePanel (g, 16, stdLineHeight, DrawDownArrow);
    StaticPrompt (g, "Query Refinement", stdCharWidth * 19 - 23,
                  stdLineHeight, systemFont, 'l');
  } else {
    SimplePanel (g, 16, stdLineHeight, DrawUpArrow);
    StaticPrompt (g, "Term Selection", stdCharWidth * 12 - 23,
                  stdLineHeight, systemFont, 'l');
    SimplePanel (g, 15, stdLineHeight, DrawDownArrow);
    StaticPrompt (g, "Query Refinement", stdCharWidth * 12 - 23,
                  stdLineHeight, systemFont, 'l');
  }
  StaticPrompt (g, "Special", stdCharWidth * 4 - 5, stdLineHeight, systemFont, 'r');
  StaticPrompt (g, "Total", stdCharWidth * 5 - 5, stdLineHeight, systemFont, 'r');
  if (smallScreen) {
    chosenSlate = ScrollSlate (w, 28, 8);
  } else {
    chosenSlate = ScrollSlate (w, 33, 8);
  }
  GetNextPosition (chosenSlate, &npt);
  chosen = TablePanel (chosenSlate, 0, systemFont, NULL);
  SetNextPosition (chosenSlate, npt);
  bracket = PalettePanel (chosenSlate, 0, NULL);
  SetNextPosition (chosenSlate, npt);
  panel = CustomPanel (chosenSlate, NULL, 0, NULL);
  SetPanelClick (panel, PanelClick, PanelDrag, NULL, PanelRelease);
  RecreateChosen ();
  Break (w);
  retrieve = PushButton (w, "Retrieve 000000000 Documents", RetrieveDocsProc);
  SetTitle (retrieve, "Retrieve 0 Documents");
  Disable (retrieve);
  Advance (w);
  resetBtn = PushButton (w, "Reset", ResetProc);
  Advance (w);
  if (smallScreen) {
    advancedboolBtn = PushButton (w, "More Bools ...", AdvancedBoolProc);
  } else {
    advancedboolBtn = PushButton (w, "More Booleans ...", AdvancedBoolProc);
  }
  chosenHeight = LineHeight ();
  clickedRow = 1;
  mergeLSP = NULL;
  if (termWindow != chosenWindow) {
    SetActivate (w, ActivateChosenProc);
  }
  RealizeWindow (w);

  advancedboolWin = FixedWindow(-50, -10, -10, -10, "More Booleans", NULL);
  SetGroupSpacing(advancedboolWin, 20, 20);
#ifdef WIN_MAC
  fnt = ChooseFont("BOOLEANS", "Monaco,12");
#endif
#ifdef WIN_MSWIN
  fnt = ChooseFont("BOOLEANS", "Courier,12");
#endif
#ifdef WIN_MOTIF
  fnt = ChooseFont("BOOLEANS", "Courier,14");
#endif
  if (smallScreen) {
    advancedboolText = ScrollText(advancedboolWin, 27, 6, fnt, TRUE, advBoolActn);
  } else {
    advancedboolText = ScrollText(advancedboolWin, 40, 6, fnt, TRUE, advBoolActn);
  }
  g = HiddenGroup(advancedboolWin, 5, 0, NULL);
  PushButton(g, "Clear", clrAdvancedProc);
  pasteFromBtn = PushButton(g, "Paste from Query Refinement", pasteFromProc);
  Disable (pasteFromBtn);
  PushButton(g, "Cancel", doneAdvProc);
  evalAdvBtn = PushButton(g, "Evaluate", evalAdvProc);
  Disable(evalAdvBtn);

  g = HiddenGroup(advancedboolWin, 6, 0, NULL);
  PushButton(g, "(", leftParenProc);
  PushButton(g, ")", rightParenProc);
  PushButton(g, "AND ( & )", andProc);
  PushButton(g, "OR ( | )", orProc);
  PushButton(g, "BUTNOT ( - )", butNotProc);
  if (smallScreen) {
    g = HiddenGroup(advancedboolWin, 5, 0, NULL);
  }
  retrieveAdv = PushButton (g, "Retrieve 000000000 Documents", RetrieveDocsProc);
  SetTitle (retrieveAdv, "Retrieve 0 Documents");
  Disable (retrieveAdv);

  RealizeWindow (advancedboolWin);
  Hide(advancedboolWin);
}
