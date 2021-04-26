/*   avail.c
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
* File Name:  avail.c
*
* Author:  Kans, Schuler
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.35 $
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
#include "browser.h"

#define numPagesToRead 4
#define SCAN_MAX 200
#define MIN_MATCH_FOR_CHOSEN_PROMOTION 3

typedef void (*SelectProc) PROTO((void));

static IteM        openNameItem;
static IteM        openTermItem;

static Int2        db;
static Int2        fld;

static PopuP       database;
static PopuP       medlField;
static PopuP       protField;
static PopuP       nuclField;

static PopuP       selTruMode;
static PopuP       mulSelTruMode;
static PopuP       lkpMode;

static Int2        wordMedlVal;
static Int2        meshMedlVal;
static Int2        kywdMedlVal;
static Int2        authMedlVal;
static Int2        jourMedlVal;
static Int2        geneMedlVal;
static Int2        ecnoMedlVal;

static Int2        wordProtVal;
static Int2        kywdProtVal;
static Int2        authProtVal;
static Int2        jourProtVal;
static Int2        orgnProtVal;
static Int2        accnProtVal;
static Int2        geneProtVal;
static Int2        protProtVal;
static Int2        ecnoProtVal;

static Int2        wordNuclVal;
static Int2        kywdNuclVal;
static Int2        authNuclVal;
static Int2        jourNuclVal;
static Int2        orgnNuclVal;
static Int2        accnNuclVal;
static Int2        geneNuclVal;
static Int2        protNuclVal;

static PopuP       defMode;

static PopuP       currentField;
static PopuP       currentMode;

static SelectProc  currentProc;

static IteM        timerItem;
static ButtoN      accept;

static Int2        currentRow;
static Boolean     pagesLoaded;
static Int2        firstPage;
static Int2        numRows;
static Int2        visLines;
static Int2        virtBefore;
static Int2        virtAfter;
static Int2        numTermsRead;

static SlatE       availSlate;
static TablE       available;

static Int2        totalPages;
static CharPtr     pagePtr;
static Int2        scale;
static Int2        timerCount;
static Boolean     useTimer;
static Boolean     autoLoad;
static Boolean     textChanged;
static Boolean     okayToAccept;
static Boolean     lookupViaTable;
static Boolean     lookupByUid;

static Boolean     macLike;
static Boolean     allowNuclProtName;

static Int2        popupToDb [4];

static Char        selection [256];

#ifdef PRODUCE_USERS_GUIDE
static TexT        hiddenText;
#endif

static void RecreateAvailable (void)

{
  RecT    r;
  WindoW  tempPort;
  Int2    wid;

  if (smallScreen) {
    wid = 19;
  } else {
    wid = 24;
  }
  RecordColumn (available, wid, 'l', FALSE, FALSE, NULL);  /* terms         */
  RecordColumn (available, 4, 'r', FALSE, TRUE, NULL);     /* special count */
  RecordColumn (available, 5, 'r', FALSE, TRUE, NULL);     /* total count   */
  ObjectRect (availSlate, &r);
  InsetRect (&r, 4, 4);
  RegisterRect ((PaneL) available, &r);
  if (Visible (available) && AllParentsVisible (available)) {
    tempPort = SavePort (available);
    Select (available);
    InvalRect (&r);
    RestorePort (tempPort);
  }
}

extern void ResetAvailList (void)

{
  Reset (availSlate);
  RecreateAvailable ();
}

static void ActivateTermProc (WindoW w)

{
  docActive = FALSE;
  Disable (openItem);
#ifdef WIN_MAC
  Disable (closeItem);
#endif
  Disable (saveItem);
  Disable (saveAsItem);
  Disable (printItem);
  Select (term);
}

/* Binary search of list of available terms */

static Int2 FindText (CharPtr text)

{
  Int2  compare;
  Int2  left;
  Int2  mid;
  Int2  num;
  Int2  right;
  Char  str [256];

  mid = 0;
  num = TableNumLines (available);
  if (text != NULL && num > 0) {
    left = 1;
    right = num;
    while (left <= right) {
      mid = (left + right) / 2;
      GetTableText (available, mid, 1, str, sizeof (str));
      compare = MeshStringICmp (text, str);
      if (compare <= 0) {
        right = mid - 1;
      }
      if (compare >= 0) {
        left = mid + 1;
      }
    }
    if (left <= right + 1) {
      GetTableText (available, mid, 1, str, sizeof (str));
      str [StrngLen (text)] = '\0';
      compare = MeshStringICmp (text, str);
      if (compare != 0) {
        mid++;
        if (mid <= num) {
          GetTableText (available, mid, 1, str, sizeof (str));
          str [StrngLen (text)] = '\0';
          compare = MeshStringICmp (text, str);
          if (compare != 0) {
            mid = 0;
          }
        }
      }
    }
  }
  return mid;
}

/* Remember current selection from list of available terms */

static void SetAvailSelection (Int2 row)

{
  GetTableText (available, row, 1, selection, sizeof (selection));
}

/* Scroll to appropriate position in list of available terms */

static void ScrollToText (CharPtr text, Int2 offset, Boolean newPage)

{
  Int2  closest;
  Int2  found;
  Int2  i;
  Int2  numLines;
  Int2  off;
  Char  str [256];
  Int2  visLines;

  found = FindText (text);
  numLines = TableNumLines (available);
  visLines = TableVisLines (available);
  SetRange (availSlate, visLines - 1, visLines - 1, 0);
  if (found > 0 && found <= numLines) {
    GetOffset (availSlate, NULL, &off);
    if (found - 1 + offset < off || found - 1 + offset >= off + visLines || newPage) {
      SetOffset (availSlate, 0, found - 1 + offset);
    }
  } else if (text != NULL && *text != '\0') {
    StrngCpy (str, text, sizeof (str));
    i = (Int2) StrngLen (str);
    while (i > 1) {
      str [i] = '\0';
      closest = FindText (str);
      if (closest > 0 && closest <= numLines) {
        GetOffset (availSlate, NULL, &off);
        if (closest - 1 < off || closest - 1 >= off + visLines || newPage) {
          SetOffset (availSlate, 0, closest - 1);
        }
        i = 0;
      } else {
        i--;
      }
    }
  } else {
    SetOffset (availSlate, 0, offset);
  }
  if (currentRow > 0 && currentRow != found) {
    SetTableBlockHilight (available, currentRow, currentRow, 1, 3, FALSE);
  }
  if (found > 0 && found <= numLines && StrngLen (text) > 0 && offset == 0) {
    if (currentRow != found) {
      SetTableBlockHilight (available, found, found, 1, 3, TRUE);
    }
    currentRow = found;
  } else {
    currentRow = 0;
  }
  if (currentRow > 0) {
    SetAvailSelection (currentRow);
    if (! Enabled (accept)) {
      Enable (accept);
    }
  } else {
    currentRow = 0;
    selection [0] = '\0';
    GetTableText (available, 1, 1, str, sizeof (str));
    if (MeshStringICmp (text, str) > 0) {
      GetTableText (available, numLines, 1, str, sizeof (str));
      if (MeshStringICmp (text, str) < 0) {
        if (Enabled (accept)) {
          Disable (accept);
        }
      }
    }
  }
}

static void ReadPages (Int2 start, Int2 db, Int2 fld, CharPtr text, Int2 offset);

static void ScrollAction (BaR sb, SlatE s, Int2 newval, Int2 oldval)

{
  Int2  start;
  Char  str [256];

  if (pagesLoaded) {
    /*
    SetTitle (term, "");
    selection [0] = '\0';
    */
    Disable (accept);
    if (newval < oldval && newval + numRows > virtBefore) {
      start = MAX (firstPage - numPagesToRead + 1, 0);
      GetTableText (available, 1, 1, str, sizeof (str));
      ReadPages (start, db, fld, str, -1);
    } else if (newval > oldval && virtBefore + numRows > newval) {
      start = MAX (firstPage + numPagesToRead - 1, 0);
      GetTableText (available, numRows - visLines + 1, 1, str, sizeof (str));
      ReadPages (start, db, fld, str, 1);
    } else if (newval >= virtBefore + virtAfter + numRows - visLines - 2) {
      start = MAX (totalPages - numPagesToRead, 0);
      ReadPages (start, db, fld, "", 500);
    } else {
      ReadPages (newval / scale, db, fld, "", 0);
    }
  }
}

/* Add currently loaded page terms into available list */

static void FormatPages (Int2 start)

{
  Int2   after;
  Int2   before;
  Char   ch;
  Int2   i;

  pagesLoaded = TRUE;
  i = 0;
  numRows = 0;
  while (pagePtr [i] != '\0') {
    ch = pagePtr [i];
    if (ch == '\n' || ch == '\r') {
      pagePtr [i] = '\n';
      numRows++;
    }
    i++;
  }
  before = start;
  after = MAX (totalPages - start - numPagesToRead, 0);
  scale = (32000 - numRows) / MAX ((before + after), 1);
  virtBefore = before * scale;
  virtAfter = after * scale;
  ResetAvailList ();
  Disable (availSlate);
  VirtualSlate (availSlate, virtBefore, virtAfter, ScrollAction);
  AppendTableText (available, pagePtr);
}

/* Read a set of pages from the disk */

static Boolean ReadTermProc (CharPtr term, Int4 special, Int4 total)

{
  Char  str [32];

  StringCat (pagePtr, term);
  sprintf (str, "\t%ld\t%ld\n", special, total);
  StringCat (pagePtr, str);
  MemFree (term);
  return TRUE;
}

static void ReadPages (Int2 start, Int2 db, Int2 fld, CharPtr text, Int2 offset)

{
  Int2  i;
  Int2  rsult2;

  currentRow = 0;
  for (i = 0; i < 2048 * numPagesToRead + 128; i++) {
    pagePtr [i] = '\0';
  }
  if (start + numPagesToRead > totalPages) {
    start = MAX (totalPages - numPagesToRead, 0);
  }
  firstPage = start;
  if (start >= 0) {
    threads++;
    rsult2 = EntrezTermListByPage (db, fld, start, numPagesToRead, ReadTermProc);
    threads--;
    if (rsult2 != 0) {
      FormatPages (start);
      ScrollToText (text, offset, TRUE);
    } else {
      ResetAvailList ();
      SetTitle (term, "");
      selection [0] = '\0';
      Disable (accept);
    }
  } else {
    ResetAvailList ();
    SetTitle (term, "");
    selection [0] = '\0';
    Disable (accept);
  }
  Enable (availSlate);
}

/* Read a set of pages only if necessary */

static Boolean ReadOneProc (CharPtr term, Int4 special, Int4 total)

{
  MemFree (term);
  return FALSE;
}

static Boolean LoadAvailList (CharPtr str, Boolean okayToLoad)

{
  EntrezInfoPtr  eip;
  Int2           found;
  Boolean        rsult;
  Int2           start;

  rsult = FALSE;
  if (StrngLen (str) > 0) {
    found = FindText (str);
    ErrSetOpts (ERR_CONTINUE, 0);
    threads++;
    eip = EntrezGetInfo ();
    if (eip != NULL) {
      totalPages = eip->types [db].fields [fld].num_bucket;
    } else {
      totalPages = 0;
    }
    threads--;
    ErrSetOpts (ERR_ABORT, 0);
    ErrShow ();
    if (totalPages > 0) {
      if (okayToLoad) {
        threads++;
        EntrezTermListByTerm (db, fld, str, 1, ReadOneProc, &start);
        threads--;
        if (start < firstPage || start > firstPage + numPagesToRead ||
            (found > TableNumLines (available) &&
             firstPage + numPagesToRead < totalPages) ||
             (! pagesLoaded)) {
          ReadPages (start, db, fld, str, 0);
          rsult = TRUE;
        }
      } else {
        ScrollToText (str, 0, FALSE);
      }
    } else {
      currentRow = 0;
      selection [0] = '\0';
      firstPage = 0;
      numRows = 0;
      pagesLoaded = FALSE;
      textChanged = FALSE;
      okayToAccept = FALSE;
      lookupViaTable = FALSE;
      lookupByUid = FALSE;
      timerCount = 0;
    }
  }
  return rsult;
}

/* Respond to user key press - reset timer, scroll if choice is loaded */

static void TextAction (TexT t)

{
  Int2  numLines;
  Char  str [256];
  Char  txt [256];

  if (lookupByUid) {
    GetTitle (term, str, sizeof (str));
    if (StrngLen (str) == 0) {
      if (Enabled (accept)) {
        Disable (accept);
      }
    } else {
      if (! Enabled (accept)) {
        Enable (accept);
      }
    }
  } else if (lookupViaTable && threads == 0) {
    GetTitle (term, str, sizeof (str));
    if (StrngLen (str) == 0) {
      selection [0] = '\0';
      currentRow = 0;
      firstPage = 0;
      pagesLoaded = FALSE;
      textChanged = FALSE;
      ResetAvailList ();
      if (Enabled (accept)) {
        Disable (accept);
      }
    } else {
      textChanged = TRUE;
      if (pagesLoaded) {
        GetTableText (available, 1, 1, txt, sizeof (txt));
        if (MeshStringICmp (str, txt) > 0) {
          numLines = TableNumLines (available);
          GetTableText (available, numLines, 1, txt, sizeof (txt));
          if (MeshStringICmp (str, txt) > 0) {
            if (! Enabled (accept)) {
              Enable (accept);
            }
          }
        } else {
          if (! Enabled (accept)) {
            Enable (accept);
          }
        }
        LoadAvailList (str, FALSE);
        okayToAccept = FALSE;
      } else {
        if (! Enabled (accept)) {
          Enable (accept);
        }
      }
    }
    timerCount = 0;
  } else {
    GetTitle (term, str, sizeof (str));
    if (StrngLen (str) == 0) {
      if (Enabled (accept)) {
        Disable (accept);
      }
    } else {
      if (! Enabled (accept)) {
        Enable (accept);
      }
    }
  }
}

/* Respond to delay in typing by loading pages */

extern void AvailTimerProc (void)

{
  Char  str [256];

  okayToAccept = TRUE;
  if (lookupByUid) {
  } else if (lookupViaTable) {
    timerCount++;
    if (timerCount > 10 && textChanged && useTimer && threads == 0) {
      textChanged = FALSE;
      GetTitle (term, str, sizeof (str));
      if (StrngLen (str) > 0) {
        WatchCursor ();
        LoadAvailList (str, TRUE);
        okayToAccept = FALSE;
        textChanged = FALSE;
        ArrowCursor ();
      }
      timerCount = 0;
    }
  }
}

static Boolean ReadSevenProc (CharPtr term, Int4 special, Int4 total)

{
  Char  str [32];

  StringCat (pagePtr, term);
  sprintf (str, "\t%ld\t%ld\n", special, total);
  StringCat (pagePtr, str);
  MemFree (term);
  numTermsRead++;
  return (Boolean) (numTermsRead < visLines);
}

static void ReadAFew (CharPtr text)

{
  Int2  i;
  Int2  start;

  numTermsRead = 0;
  currentRow = 0;
  for (i = 0; i < 2048 * numPagesToRead + 128; i++) {
    pagePtr [i] = '\0';
  }
  StringNCpy (selection, text, sizeof (selection) - 1);
  threads++;
  EntrezTermListByTerm (db, fld, text, 7, ReadSevenProc, &start);
  threads--;
  if (numTermsRead > 0) {
    FormatPages (start);
    ScrollToText (text, 0, TRUE);
  }
  Enable (availSlate);
}

static void PlainMultipleTerms (void)

{
  Boolean     changed;
  ValNodePtr  elst;
  Boolean     found;
  Int2        i;
  Int2        j;
  LinkSetPtr  mergeLSP;
  Int4        special;
  Char        str [256];
  Char        strn [256];
  Char        strs [256];
  Int4        total;

  WatchCursor ();
  changed = FALSE;
  GetTitle (term, strs, sizeof (strs));
  i = 0;
  while (StrngLen (strs + i) > 0) {
    StrngCpy (str, strs + i, sizeof (str));
    j = 0;
    while (str [j] != '\0' && str [j] != ' ') {
      j++;
    }
    if (str [j] == ' ') {
      str [j] = '\0';
      i += j + 1;
    } else {
      i += j;
    }
    if (StrngLen (str) > 0) {
      selection [0] = '\0';
      ReadAFew (str);
      if (numTermsRead > 0 && StrngLen (selection) > 0) {
        threads++;
        found = EntrezFindTerm (db, fld, selection, &special, &total);
        threads--;
        if (found) {
          sprintf (strn, "%s\t%ld\t%ld\t%d\t%d\n", selection, special, total, db, fld);
          if (StrngEql (selection, str, FALSE)) {
            LoadChosenList (strn, fld, GROUP_SINGLE, autoLoad);
            threads++;
            elst = EntrezTLNew (db);
            EntrezTLAddTerm (elst, str, db, fld, TRUE);
            mergeLSP = EntrezTLEval (elst);
            EntrezTLFree (elst);
            if (mergeLSP != NULL) {
              mergeLSP = LinkSetFree (mergeLSP);
            }
            threads--;
          } else {
            if (StrngLen(selection) >= MIN_MATCH_FOR_CHOSEN_PROMOTION &&
                StrngLen(str) >= MIN_MATCH_FOR_CHOSEN_PROMOTION &&
                StrNCmp(selection, str, MIN_MATCH_FOR_CHOSEN_PROMOTION) == 0) {
              /* if it's not an exact match, then make it appear in the       */
              /* "chosen" window, but don't highlight it and don't include it */
              /* in the boolean computations                                  */
              LoadChosenList (strn, fld, GROUP_SINGLE, FALSE);
            }
          }
          changed = TRUE;
        }
      }
    }
#ifndef PRODUCE_USERS_GUIDE
    ResetAvailList ();
#endif
  }
  if (changed) {
    InsertList ();
  }
#ifndef PRODUCE_USERS_GUIDE
  SetTitle (term, "");
  Select (term);
#else
  Select (hiddenText);
#endif
  selection [0] = '\0';
  currentRow = 0;
  firstPage = 0;
  pagesLoaded = FALSE;
  textChanged = FALSE;
#ifndef PRODUCE_USERS_GUIDE
  Disable (accept);
#endif
  ArrowCursor ();
}

static void TruncateSingleTerm (void)

{
  Boolean  found;
  Int4     special;
  Char     strn [256];
  Char     strs [256];
  Int4     total;

  WatchCursor ();
  GetTitle (term, strs, sizeof (strs));
  if (StrngLen (strs) > 0) {
    if (StringChr (strs, '*') == NULL && StringChr (strs, '?') == NULL) {
      StringCat (strs, "...");
    }
    threads++;
    found = EntrezFindTerm (db, fld, strs, &special, &total);
    threads--;
    if (found) {
      sprintf (strn, "%s\t%ld\t%ld\t%d\t%d\n", strs, special, total, db, fld);
      LoadChosenList (strn, fld, GROUP_SINGLE, autoLoad);
      InsertList ();
    }
  }
  SetTitle (term, "");
  Select (term);
  selection [0] = '\0';
  currentRow = 0;
  firstPage = 0;
  pagesLoaded = FALSE;
  textChanged = FALSE;
  ResetAvailList ();
  Disable (accept);
  ArrowCursor ();
}

static void TruncateMultipleTerms (void)

{
  Boolean  changed;
  Boolean  found;
  Int2     i;
  Int2     j;
  Int4     special;
  Char     str [256];
  Char     strn [256];
  Char     strs [256];
  Int4     total;

  WatchCursor ();
  changed = FALSE;
  GetTitle (term, strs, sizeof (strs));
  i = 0;
  while (StrngLen (strs + i) > 0) {
    StrngCpy (str, strs + i, sizeof (str));
    j = 0;
    while (str [j] != '\0' && str [j] != ' ') {
      j++;
    }
    if (str [j] == ' ') {
      str [j] = '\0';
      i += j + 1;
    } else {
      i += j;
    }
    if (StringChr (strs, '*') == NULL && StringChr (strs, '?') == NULL) {
      StringCat (str, "...");
    }
    threads++;
    found = EntrezFindTerm (db, fld, str, &special, &total);
    threads--;
    if (found) {
      sprintf (strn, "%s\t%ld\t%ld\t%d\t%d\n", str, special, total, db, fld);
      LoadChosenList (strn, fld, GROUP_SINGLE, autoLoad);
      /*
      InsertList ();
      */
      changed = TRUE;
    }
  }
  if (changed) {
    InsertList ();
  }
  SetTitle (term, "");
  Select (term);
  selection [0] = '\0';
  currentRow = 0;
  firstPage = 0;
  pagesLoaded = FALSE;
  textChanged = FALSE;
  ResetAvailList ();
  Disable (accept);
  ArrowCursor ();
}

/* Accept choice, place into chosen list */

static void AcceptProc (ButtoN b)

{
  EntrezInfoPtr  eip;
  Boolean        found;
  Int4           special;
  Char           str [256];
  Int4           total;
  DocUid         uid;

  if (threads == 0) {
    if (lookupByUid) {
      GetTitle (term, str, sizeof (str));
      if (StrToLong (str, &uid)) {
        RetrieveDocuments (1, &uid, NULL, db);
      }
    } else if (lookupViaTable) {
      if (textChanged) {
        GetTitle (term, str, sizeof (str));
        if (StrngLen (str) > 0) {
          WatchCursor ();
          LoadAvailList (str, TRUE);
          okayToAccept = FALSE;
          textChanged = FALSE;
          ArrowCursor ();
        }
      } else if (okayToAccept) {
        SetTitle (term, selection);
        if (selection [0] != '\0') {
          WatchCursor ();
          threads++;
          found = EntrezFindTerm (db, fld, selection, &special, &total);
          threads--;
          if (found) {
            sprintf (str, "%s\t%ld\t%ld\t%d\t%d\n", selection, special, total, db, fld);
            LoadChosenList (str, fld, GROUP_SINGLE, autoLoad);
            InsertList ();
          }
          ArrowCursor ();
        }
        Select (term);
      }
    } else if (currentProc != NULL) {
      eip = EntrezGetInfo ();
      if (eip != NULL && eip->types [db].fields [fld].num_bucket > 0) {
        currentProc ();
      }
    }
  }
}

/* Respond to click in available list */

static void AvailableAction (TablE t, Int2 row, Int2 col)

{
  Boolean  okay;
  Char     str [256];

  if (pagesLoaded && threads == 0) {
    okay = TRUE;
    if (! GetTableHilight (available, row, col)) {
      if (currentRow > 0) {
        SetTableBlockHilight (available, currentRow, currentRow, 1, 3, FALSE);
      }
      SetTableBlockHilight (available, row, row, 1, 3, TRUE);
      currentRow = row;
      okay = FALSE;
      SetAvailSelection (currentRow);
      if (! Enabled (accept)) {
        Enable (accept);
      }
    }
    if (currentRow > 0) {
      GetTableText (available, currentRow, 1, str, sizeof (str));
      if (dblClick && okay && Enabled (accept)) {
        AcceptProc (accept);
      }
      SetTitle (term, str);
      Select (term);
    }
  }
}

extern Int2 ReturnDatabaseCode (void)

{
  Int2  val;

  val = GetValue (database);
  /*
  switch (val) {
    case 1:
      db = TYP_ML;
      break;
    case 2:
      db = TYP_AA;
      break;
    case 3:
      db = TYP_NT;
      break;
    default:
      db = NTYPE;
      break;
  }
  */
  db = NTYPE;
  if (val >= 1 && val <= 3) {
    db = popupToDb [val];
  }
  return db;
}

extern Int2 ReturnFieldCode (void)

{
  Int2  mod;
  Int2  val;

  lookupByUid = FALSE;
  lookupViaTable = TRUE;
  currentProc = NULL;
  switch (db) {
    case TYP_ML:
      val = GetValue (medlField);
      switch (val) {
        case 1:
          fld = FLD_WORD;
          mod = wordMedlVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = PlainMultipleTerms;
          }
          if (mod == 3) {
            currentProc = TruncateMultipleTerms;
          }
          break;
        case 2:
          fld = FLD_MESH;
          mod = meshMedlVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 3:
          fld = FLD_AUTH;
          mod = authMedlVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 4:
          fld = FLD_JOUR;
          mod = jourMedlVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 5:
          fld = FLD_GENE;
          mod = geneMedlVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 6:
          fld = FLD_KYWD;
          mod = kywdMedlVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 7:
          fld = FLD_ECNO;
          mod = ecnoMedlVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 8:
          fld = NFLD;
          lookupViaTable = FALSE;
          lookupByUid = TRUE;
          break;
        default:
          fld = NFLD;
          break;
      }
      break;
    case TYP_AA:
      val = GetValue (protField);
      switch (val) {
        case 1:
          fld = FLD_WORD;
          mod = wordProtVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = PlainMultipleTerms;
          }
          if (mod == 3) {
            currentProc = TruncateMultipleTerms;
          }
          break;
        case 2:
          fld = FLD_KYWD;
          mod = kywdProtVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 3:
          fld = FLD_AUTH;
          mod = authProtVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 4:
          fld = FLD_JOUR;
          mod = jourProtVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 5:
          fld = FLD_ORGN;
          mod = orgnProtVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 6:
          fld = FLD_ACCN;
          mod = accnProtVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 7:
          fld = FLD_GENE;
          mod = geneProtVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 8:
          fld = FLD_PROT;
          mod = protProtVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 9:
          fld = FLD_ECNO;
          mod = ecnoProtVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 10:
          fld = NFLD;
          lookupViaTable = FALSE;
          lookupByUid = TRUE;
          break;
        default:
          fld = NFLD;
          break;
      }
      break;
    case TYP_NT:
      val = GetValue (nuclField);
      if (val == 8 && (! allowNuclProtName)) {
        val = 9;
      }
      switch (val) {
        case 1:
          fld = FLD_WORD;
          mod = wordNuclVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = PlainMultipleTerms;
          }
          if (mod == 3) {
            currentProc = TruncateMultipleTerms;
          }
          break;
        case 2:
          fld = FLD_KYWD;
          mod = kywdNuclVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 3:
          fld = FLD_AUTH;
          mod = authNuclVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 4:
          fld = FLD_JOUR;
          mod = jourNuclVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 5:
          fld = FLD_ORGN;
          mod = orgnNuclVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 6:
          fld = FLD_ACCN;
          mod = accnNuclVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 7:
          fld = FLD_GENE;
          mod = geneNuclVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 8:
          fld = FLD_PROT;
          mod = protNuclVal;
          if (mod != 1) {
            lookupViaTable = FALSE;
          }
          if (mod == 2) {
            currentProc = TruncateSingleTerm;
          }
          break;
        case 9:
          fld = NFLD;
          lookupViaTable = FALSE;
          lookupByUid = TRUE;
          break;
        default:
          fld = NFLD;
          break;
      }
      break;
    default:
      fld = NFLD;
      break;
  }
  return fld;
}

/* Change choice of database */

extern void ResetState (void)

{
  SetTitle (term, "");
  selection [0] = '\0';
  currentRow = 0;
  firstPage = 0;
  pagesLoaded = FALSE;
  textChanged = FALSE;
  okayToAccept = FALSE;
  ClearAllLists ();
  ClearHistoryList ();
  ResetRetrieveButton ();
  ResetAvailList ();
  ResetChosenList ();
  ResetFetchedList ();
  Select (term);
  Disable (accept);
  threads = 0;
}

static void AlterDatabase (Int2 dbcode)

{
  PopuP  newField;

  switch (dbcode) {
    case TYP_ML:
      newField = medlField;
      break;
    case TYP_AA:
      newField = protField;
      break;
    case TYP_NT:
      newField = nuclField;
      break;
    default:
      newField = currentField;
      break;
  }
  if (newField != currentField) {
    Hide (currentField);
    currentField = newField;
    Show (currentField);
  }
}

static void DatabaseHasChanged (void)

{
  db = ReturnDatabaseCode ();
  AlterDatabase (db);
}

static void AlterField (Int2 dbcode, Int2 fldcode)

{
  PopuP  newMode;
  Int2   newVal;

  switch (dbcode) {
    case TYP_ML:
      switch (fldcode) {
        case FLD_WORD:
          newMode = mulSelTruMode;
          newVal = wordMedlVal;
          break;
        case FLD_MESH:
          newMode = selTruMode;
          newVal = meshMedlVal;
          break;
        case FLD_KYWD:
          newMode = selTruMode;
          newVal = kywdMedlVal;
          break;
        case FLD_AUTH:
          newMode = selTruMode;
          newVal = authMedlVal;
          break;
        case FLD_JOUR:
          newMode = selTruMode;
          newVal = jourMedlVal;
          break;
        case FLD_GENE:
          newMode = selTruMode;
          newVal = geneMedlVal;
          break;
        case FLD_ECNO:
          newMode = selTruMode;
          newVal = ecnoMedlVal;
          break;
        case NFLD:
          newMode = lkpMode;
          newVal = 1;
          break;
        default:
          newMode = defMode;
          newVal = 1;
          break;
      }
      break;
    case TYP_AA:
      switch (fldcode) {
        case FLD_WORD:
          newMode = mulSelTruMode;
          newVal = wordProtVal;
          break;
        case FLD_KYWD:
          newMode = selTruMode;
          newVal = kywdProtVal;
          break;
        case FLD_AUTH:
          newMode = selTruMode;
          newVal = authProtVal;
          break;
        case FLD_JOUR:
          newMode = selTruMode;
          newVal = jourProtVal;
          break;
        case FLD_ORGN:
          newMode = selTruMode;
          newVal = orgnProtVal;
          break;
        case FLD_ACCN:
          newMode = selTruMode;
          newVal = accnProtVal;
          break;
        case FLD_GENE:
          newMode = selTruMode;
          newVal = geneProtVal;
          break;
        case FLD_PROT:
          newMode = selTruMode;
          newVal = protProtVal;
          break;
        case FLD_ECNO:
          newMode = selTruMode;
          newVal = ecnoProtVal;
          break;
        case NFLD:
          newMode = lkpMode;
          newVal = 1;
          break;
        default:
          newMode = defMode;
          newVal = 1;
          break;
      }
      break;
    case TYP_NT:
      switch (fldcode) {
        case FLD_WORD:
          newMode = mulSelTruMode;
          newVal = wordNuclVal;
          break;
        case FLD_KYWD:
          newMode = selTruMode;
          newVal = kywdNuclVal;
          break;
        case FLD_AUTH:
          newMode = selTruMode;
          newVal = authNuclVal;
          break;
        case FLD_JOUR:
          newMode = selTruMode;
          newVal = jourNuclVal;
          break;
        case FLD_ORGN:
          newMode = selTruMode;
          newVal = orgnNuclVal;
          break;
        case FLD_ACCN:
          newMode = selTruMode;
          newVal = accnNuclVal;
          break;
        case FLD_GENE:
          newMode = selTruMode;
          newVal = geneNuclVal;
          break;
        case FLD_PROT:
          newMode = selTruMode;
          newVal = protNuclVal;
          break;
        case NFLD:
          newMode = lkpMode;
          newVal = 1;
          break;
        default:
          newMode = defMode;
          newVal = 1;
          break;
      }
      break;
    default:
      newMode = currentMode;
      newVal = GetValue (currentMode);
      break;
  }
  Hide (currentMode);
  currentMode = newMode;
  SetValue (currentMode, newVal);
  Show (currentMode);
}

static void FieldHasChanged (void)

{
  db = ReturnDatabaseCode ();
  fld = ReturnFieldCode ();
  AlterField (db, fld);
}

static void AlterMode (Int2 dbcode, Int2 fldcode)

{
  switch (dbcode) {
    case TYP_ML:
      switch (fldcode) {
        case FLD_WORD:
          wordMedlVal = GetValue (mulSelTruMode);
          break;
        case FLD_MESH:
          meshMedlVal = GetValue (selTruMode);
          break;
        case FLD_KYWD:
          kywdMedlVal = GetValue (selTruMode);
          break;
        case FLD_AUTH:
          authMedlVal = GetValue (selTruMode);
          break;
        case FLD_JOUR:
          jourMedlVal = GetValue (selTruMode);
          break;
        case FLD_GENE:
          geneMedlVal = GetValue (selTruMode);
          break;
        case FLD_ECNO:
          ecnoMedlVal = GetValue (selTruMode);
          break;
        case NFLD:
          break;
        default:
          break;
      }
      break;
    case TYP_AA:
      switch (fldcode) {
        case FLD_WORD:
          wordProtVal = GetValue (mulSelTruMode);
          break;
        case FLD_KYWD:
          kywdProtVal = GetValue (selTruMode);
          break;
        case FLD_AUTH:
          authProtVal = GetValue (selTruMode);
          break;
        case FLD_JOUR:
          jourProtVal = GetValue (selTruMode);
          break;
        case FLD_ORGN:
          orgnProtVal = GetValue (selTruMode);
          break;
        case FLD_ACCN:
          accnProtVal = GetValue (selTruMode);
          break;
        case FLD_GENE:
          geneProtVal = GetValue (selTruMode);
          break;
        case FLD_PROT:
          protProtVal = GetValue (selTruMode);
          break;
        case FLD_ECNO:
          ecnoProtVal = GetValue (selTruMode);
          break;
        case NFLD:
          break;
        default:
          break;
      }
      break;
    case TYP_NT:
      switch (fldcode) {
        case FLD_WORD:
          wordNuclVal = GetValue (mulSelTruMode);
          break;
        case FLD_KYWD:
          kywdNuclVal = GetValue (selTruMode);
          break;
        case FLD_AUTH:
          authNuclVal = GetValue (selTruMode);
          break;
        case FLD_JOUR:
          jourNuclVal = GetValue (selTruMode);
          break;
        case FLD_ORGN:
          orgnNuclVal = GetValue (selTruMode);
          break;
        case FLD_ACCN:
          accnNuclVal = GetValue (selTruMode);
          break;
        case FLD_GENE:
          geneNuclVal = GetValue (selTruMode);
          break;
        case FLD_PROT:
          protNuclVal = GetValue (selTruMode);
          break;
        case NFLD:
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

static void ModeHasChanged (void)

{
  db = ReturnDatabaseCode ();
  fld = ReturnFieldCode ();
  AlterMode (db, fld);
}

static void ChangeDatabase (PopuP p)

{
  selection [0] = '\0';
  currentRow = 0;
  firstPage = 0;
  pagesLoaded = FALSE;
  textChanged = FALSE;
  okayToAccept = FALSE;
  ClearAllLists ();
  ResetRetrieveButton ();
  ResetAvailList ();
  ResetChosenList ();
  Select (term);
  DatabaseHasChanged ();
  FieldHasChanged ();
  fld = ReturnFieldCode ();
  if (lookupByUid) {
    SetTitle (accept, "Find");
  } else {
    SetTitle (accept, "Accept");
  }
  TextAction (term);
}

/* Change choice of field */

static void ChangeField (PopuP p)

{
  selection [0] = '\0';
  currentRow = 0;
  firstPage = 0;
  pagesLoaded = FALSE;
  textChanged = FALSE;
  okayToAccept = FALSE;
  ResetAvailList ();
  Select (term);
  db = ReturnDatabaseCode ();
  FieldHasChanged ();
  fld = ReturnFieldCode ();
  if (lookupByUid) {
    SetTitle (accept, "Find");
  } else {
    SetTitle (accept, "Accept");
  }
  TextAction (term);
}

static void ChangeMode (PopuP p)

{
  selection [0] = '\0';
  currentRow = 0;
  firstPage = 0;
  pagesLoaded = FALSE;
  textChanged = FALSE;
  okayToAccept = FALSE;
  ResetAvailList ();
  Select (term);
  ModeHasChanged ();
  db = ReturnDatabaseCode ();
  fld = ReturnFieldCode ();
  TextAction (term);
}

static void OpenTermProc (IteM i)

{
  Boolean  changed;
  CharPtr  chptr;
  Int2     dbase;
  Int2     fld;
  Boolean  found;
  FILE     *fp;
  Char     path [256];
  Boolean  ready;
  Int4     special;
  Char     str [128];
  Char     strs [128];
  Int4     total;

  if (GetInputFileName (path, sizeof (path), ".trm", "TEXT")) {
    fp = FileOpen (path, "r");
    if (fp != NULL) {
      WatchCursor ();
      changed = FALSE;
      if (FileGets (str, sizeof (str), fp)) {
        chptr = StringRChr (str, '\n');
        if (chptr != NULL) {
          *chptr = '\0';
        }
        if (StrToInt (str, &dbase)) {
          ready = FALSE;
          fld = 0;
          while (FileGets (str, sizeof (str), fp)) {
            chptr = StringRChr (str, '\n');
            if (chptr != NULL) {
              *chptr = '\0';
            }
            if (ready) {
              threads++;
              found = EntrezFindTerm (dbase, fld, str, &special, &total);
              threads--;
              if (found) {
                sprintf (strs, "%s\t%ld\t%ld\t%d\t%d\n", str, special, total, dbase, fld);
                LoadChosenList (strs, fld, GROUP_SINGLE, autoLoad);
                InsertList ();
                changed = TRUE;
              } else {
                Message (MSG_ERROR, "term %s db %d fld %d not found", str, dbase, fld);
              }
              ready = FALSE;
            } else {
              if (StrToInt (str, &fld)) {
                ready = TRUE;
              }
            }
          }
          /*
          if (changed) {
            InsertList ();
          }
          */
        }
      }
      FileClose (fp);
      ArrowCursor ();
    }
  }
}

extern void LoadOpenTerm (MenU m)

{
  openTermItem = CommandItem (m, "Open Term", OpenTermProc);
}

static void OpenNameProc (IteM i)

{
  ByteStorePtr  bsp;
  CharPtr       chptr;
  Int2          dbase;
  FILE          *fp;
  Int2          num;
  Char          path [256];
  Char          str [32];
  Char          term [32];
  DocUid        uid;
  DocUidPtr     uids;

  if (GetInputFileName (path, sizeof (path), ".uid", "TEXT")) {
    fp = FileOpen (path, "r");
    if (fp != NULL) {
      bsp = BSNew (128);
      if (bsp != NULL) {
        if (FileGets (str, sizeof (str), fp)) {
          chptr = StringRChr (str, '\n');
          if (chptr != NULL) {
            *chptr = '\0';
          }
          if (StrToInt (str, &dbase)) {
            if (dbase == db) {
              while (FileGets (str, sizeof (str), fp)) {
                chptr = StringRChr (str, '\n');
                if (chptr != NULL) {
                  *chptr = '\0';
                }
                chptr = StringRChr (str, '\r');
                if (chptr != NULL) {
                  *chptr = '\0';
                }
                if (str [0] != '\0' && StrToLong (str, &uid)) {
                  BSWrite (bsp, &uid, sizeof (DocUid));
                }
              }
              BSSeek (bsp, 0L, 0);
              num = ((Int2) BSLen (bsp)) / sizeof (DocUid);
              uids = (DocUidPtr) MemNew ((size_t) num * sizeof (DocUid));
              if (uids != NULL) {
                BSMerge (bsp, uids);
                chptr = StringRChr (path, DIRDELIMCHR);
                if (chptr != NULL) {
                  StringCpy (term, "*");
                  StringNCat (term, chptr + 1, sizeof (term) - 1);
                  EntrezCreateNamedUidList (term, dbase, 0, (Int4) num, uids);
                  threads++;
                  sprintf (path, "%s\t0\t%ld\t%d\t0\n", term, (Int4) num, dbase);
                  LoadChosenList (path, 0, GROUP_SINGLE, autoLoad);
                  InsertList ();
                  threads--;
                }
                MemFree (uids);
              }
            } else {
              Message (MSG_OK, "Database types do not match");
            }
          }
        }
        BSFree (bsp);
      }
      FileClose (fp);
    }
  }
}

extern void LoadOpenName (MenU m)

{
  openNameItem = CommandItem (m, "Open Uid List", OpenNameProc);
}

static void ChangeTimer (IteM i)

{
  if (GetStatus (timerItem)) {
    useTimer = TRUE;
    SetAppParam ("ENTREZ", "PREFERENCES", "USETIMER", "TRUE");
  } else {
    useTimer = FALSE;
    SetAppParam ("ENTREZ", "PREFERENCES", "USETIMER", "FALSE");
  }
}

extern void LoadTimerPrefs (MenU m)

{
  Char  str [32];

  timerItem = StatusItem (m, "Use Timer", ChangeTimer);
  useTimer = FALSE;
  if (GetAppParam ("ENTREZ", "PREFERENCES", "USETIMER", "", str, sizeof (str) - 1)) {
    if (StringICmp (str, "TRUE") == 0) {
      useTimer = TRUE;
      SetStatus (timerItem, TRUE);
    }
  } else {
    useTimer = FALSE;
    SetStatus (timerItem, FALSE);
    SetAppParam ("ENTREZ", "PREFERENCES", "USETIMER", "FALSE");
  }
}

static void OriginalSettingsProc (IteM i)

{
  SetAppParam ("ENTREZ", "SETTINGS", NULL, NULL);
  SetValue (database, 1);
  SetValue (medlField, 1);
  SetValue (protField, 1);
  SetValue (nuclField, 1);
  SetValue (mulSelTruMode, 2);
  SetValue (selTruMode, 1);

  wordMedlVal = 2;
  meshMedlVal = 1;
  kywdMedlVal = 1;
  authMedlVal = 1;
  jourMedlVal = 1;
  geneMedlVal = 1;
  ecnoMedlVal = 1;

  wordProtVal = 2;
  kywdProtVal = 1;
  authProtVal = 1;
  jourProtVal = 1;
  orgnProtVal = 1;
  accnProtVal = 1;
  geneProtVal = 1;
  protProtVal = 1;
  ecnoProtVal = 1;

  wordNuclVal = 2;
  kywdNuclVal = 1;
  authNuclVal = 1;
  jourNuclVal = 1;
  orgnNuclVal = 1;
  accnNuclVal = 1;
  geneNuclVal = 1;
  protNuclVal = 1;

  selection [0] = '\0';
  currentRow = 0;
  firstPage = 0;
  pagesLoaded = FALSE;
  textChanged = FALSE;
  okayToAccept = FALSE;
  ClearAllLists ();
  ResetRetrieveButton ();
  ResetAvailList ();
  ResetChosenList ();
  Select (term);

  db = ReturnDatabaseCode ();
  FieldHasChanged ();
  fld = ReturnFieldCode ();
  AlterDatabase (db);
  AlterField (db, fld);
  AlterMode (db, fld);

  if (lookupByUid) {
    SetTitle (accept, "Find");
  } else {
    SetTitle (accept, "Accept");
  }
  TextAction (term);
  
  SetValue (articleChoice, 1);
  SetValue (reportChoice, 1);
  SetValue (reportClass, 1);
  ChangeArticle (articleChoice);
  ChangeReport (reportChoice);
  ChangeClass (reportClass);
}

static void SaveASetting (CharPtr param, Int2 val, Int2 first)

{
  Char  str [16];

  if (val != first) {
    IntToStr (val, str, 0, sizeof (str));
    SetAppParam ("ENTREZ", "SETTINGS", param, str);
  }
}

static void SaveSettingsProc (IteM i)

{
  Int2  dbval;
  Int2  fldval;
  Char  str [16];

  SetAppParam ("ENTREZ", "SETTINGS", NULL, NULL);
  dbval = GetValue (database);
  IntToStr (dbval, str, 0, sizeof (str));
  SetAppParam ("ENTREZ", "SETTINGS", "DATABASE", str);
  if (popupToDb [1] == TYP_ML) {
    fldval = GetValue (medlField);
    IntToStr (fldval, str, 0, sizeof (str));
    SetAppParam ("ENTREZ", "SETTINGS", "MEDFIELD", str);
  }
  if (popupToDb [2] == TYP_AA) {
    fldval = GetValue (protField);
    IntToStr (fldval, str, 0, sizeof (str));
    SetAppParam ("ENTREZ", "SETTINGS", "PRTFIELD", str);
  }
  if (popupToDb [3] == TYP_NT) {
    fldval = GetValue (nuclField);
    IntToStr (fldval, str, 0, sizeof (str));
    SetAppParam ("ENTREZ", "SETTINGS", "NUCFIELD", str);
  }

  SaveASetting ("WORDMEDLMODE", wordMedlVal, 2);
  SaveASetting ("MESHMEDLMODE", meshMedlVal, 1);
  SaveASetting ("KYWDMEDLMODE", kywdMedlVal, 1);
  SaveASetting ("AUTHMEDLMODE", authMedlVal, 1);
  SaveASetting ("JOURMEDLMODE", jourMedlVal, 1);
  SaveASetting ("GENEMEDLMODE", geneMedlVal, 1);
  SaveASetting ("ECNOMEDLMODE", ecnoMedlVal, 1);

  SaveASetting ("WORDPROTMODE", wordProtVal, 2);
  SaveASetting ("KYWDPROTMODE", kywdProtVal, 1);
  SaveASetting ("AUTHPROTMODE", authProtVal, 1);
  SaveASetting ("JOURPROTMODE", jourProtVal, 1);
  SaveASetting ("ORGNPROTMODE", orgnProtVal, 1);
  SaveASetting ("ACCNPROTMODE", accnProtVal, 1);
  SaveASetting ("GENEPROTMODE", geneProtVal, 1);
  SaveASetting ("PROTPROTMODE", protProtVal, 1);
  SaveASetting ("ECNOPROTMODE", ecnoProtVal, 1);

  SaveASetting ("WORDNUCLMODE", wordNuclVal, 2);
  SaveASetting ("KYWDNUCLMODE", kywdNuclVal, 1);
  SaveASetting ("AUTHNUCLMODE", authNuclVal, 1);
  SaveASetting ("JOURNUCLMODE", jourNuclVal, 1);
  SaveASetting ("ORGNNUCLMODE", orgnNuclVal, 1);
  SaveASetting ("ACCNNUCLMODE", accnNuclVal, 1);
  SaveASetting ("GENENUCLMODE", geneNuclVal, 1);
  SaveASetting ("PROTNUCLMODE", protNuclVal, 1);

  SaveASetting ("MEDREPORT", GetValue (articleChoice), 1);
  SaveASetting ("SEQREPORT", GetValue (reportChoice), 1);
  SaveASetting ("SEQCLASS", GetValue (reportClass), 1);
}

extern void LoadSettingsPersistPrefs (MenU m)

{
  MenU  sub;

  sub = SubMenu (m, "Query Defaults");
  CommandItem (sub, "Current", SaveSettingsProc);
  CommandItem (sub, "Original", OriginalSettingsProc);
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

static void DrawUpArrow (PaneL p)

{
  RecT  r;

  ObjectRect (p, &r);
  r.top = (r.top + r.bottom) / 2 - 5;
  r.bottom = r.top + 10;
  CopyBits (&r, upArrow);
}

extern Int2 ChooseSetting (CharPtr param, Int2 dfault)

{
  Char  str [16];
  Int2  val;

  val = dfault;
  if (GetAppParam ("ENTREZ", "SETTINGS", param, "", str, sizeof (str))) {
    StrToInt (str, &val);
  }
  return val;
}

extern void CreateAvailList (WindoW w)

{
  Int2               dbval;
  EntrezInfoPtr      eip;
  GrouP              g;
  GrouP              h;
  Int2               i;
  PoinT              lpt;
  Int2               medfldval;
  PoinT              npt;
  Int2               nucfldval;
  Int2               prtfldval;
  Char               str [32];
  EntrezTypeDataPtr  types;

  macLike = TRUE;
  if (GetAppParam ("ENTREZ", "PREFERENCES", "POPDOWN", "", str, sizeof (str) - 1)) {
    if (StringICmp (str, "TRUE") == 0) {
      macLike = FALSE;
    }
  }
  allowNuclProtName = FALSE;
  if (GetAppParam ("ENTREZ", "PREFERENCES", "NUCLPROTNAME", "", str, sizeof (str) - 1)) {
    if (StringICmp (str, "TRUE") == 0) {
      allowNuclProtName = TRUE;
    }
  }
  pagePtr = (CharPtr) MemNew (2048 * numPagesToRead + 128);
  h = HiddenGroup (w, 0, 2, NULL);
  SetGroupSpacing (h, 15, 2);
  StaticPrompt (h, "Database:", 0, stdLineHeight, systemFont, 'l');
  g = HiddenGroup (h, 0, 0, NULL);
  eip = EntrezGetInfo ();
  for (i = 0; i < 4; i++) {
    popupToDb [i] = NTYPE;
  }
  i = 1;
  database = PopupList (g, macLike, ChangeDatabase);
  if (eip != NULL && eip->types != NULL) {
    types = eip->types;
    if (eip->type_count >= 1 && types [0].num > 0) {
      PopupItem (database, "MEDLINE");
      popupToDb [i] = TYP_ML;
      i++;
    }
    if (eip->type_count >= 2 && types [1].num > 0) {
      PopupItem (database, "Protein");
      popupToDb [i] = TYP_AA;
      i++;
    }
    if (eip->type_count >= 3 && types [2].num > 0) {
      PopupItem (database, "Nucleotide");
      popupToDb [i] = TYP_NT;
      i++;
    }
  }
  SetValue (database, 1);

  StaticPrompt (h, "Field:", 0, stdLineHeight, systemFont, 'l');
  g = HiddenGroup (h, 0, 0, NULL);
  medlField = PopupList (g, macLike, ChangeField);
  PopupItem (medlField, "Abstract or Title");
  PopupItem (medlField, "MeSH Term");
  PopupItem (medlField, "Author Name");
  PopupItem (medlField, "Journal Title");
  PopupItem (medlField, "Gene Name");
  PopupItem (medlField, "Substance");
  PopupItem (medlField, "E.C. Number");
  PopupItem (medlField, "MEDLINE ID");
  SetValue (medlField, 1);
  Hide (medlField);

  protField = PopupList (g, macLike, ChangeField);
  PopupItem (protField, "Text Terms");
  PopupItem (protField, "Keyword");
  PopupItem (protField, "Author Name");
  PopupItem (protField, "Journal Title");
  PopupItem (protField, "Organism");
  PopupItem (protField, "Accession");
  PopupItem (protField, "Gene Name");
  PopupItem (protField, "Protein Name");
  PopupItem (protField, "E.C. Number");
  PopupItem (protField, "NCBI Seq ID");
  SetValue (protField, 1);
  Hide (protField);

  nuclField = PopupList (g, macLike, ChangeField);
  PopupItem (nuclField, "Text Terms");
  PopupItem (nuclField, "Keyword");
  PopupItem (nuclField, "Author Name");
  PopupItem (nuclField, "Journal Title");
  PopupItem (nuclField, "Organism");
  PopupItem (nuclField, "Accession");
  PopupItem (nuclField, "Gene Name");
  if (allowNuclProtName) {
    PopupItem (nuclField, "Protein Name");
  }
  PopupItem (nuclField, "NCBI Seq ID");
  SetValue (nuclField, 1);
  Hide (nuclField);
  Show (medlField);

  currentField = medlField;
  StaticPrompt (h, "Mode:", 0, stdLineHeight, systemFont, 'l');
  g = HiddenGroup (h, 0, 0, NULL);

  mulSelTruMode = PopupList (g, macLike, ChangeMode);
  PopupItem (mulSelTruMode, "Selection");
  PopupItem (mulSelTruMode, "Multiple");
  PopupItem (mulSelTruMode, "Truncation");
  SetValue (mulSelTruMode, 2);
  Hide (mulSelTruMode);

  selTruMode = PopupList (g, macLike, ChangeMode);
  PopupItem (selTruMode, "Selection");
  PopupItem (selTruMode, "Truncation");
  SetValue (selTruMode, 1);
  Hide (selTruMode);

  lkpMode = PopupList (g, macLike, ChangeMode);
  PopupItem (lkpMode, "Lookup");
  SetValue (lkpMode, 1);
  Hide (lkpMode);

  GetNextPosition (g, &lpt);
  defMode = PopupList (g, macLike, ChangeMode);
  PopupItem (defMode, "Selection");
  SetValue (defMode, 1);
  Hide (defMode);

  wordMedlVal = ChooseSetting ("WORDMEDLMODE", 2);
  meshMedlVal = ChooseSetting ("MESHMEDLMODE", 1);
  kywdMedlVal = ChooseSetting ("KYWDMEDLMODE", 1);
  authMedlVal = ChooseSetting ("AUTHMEDLMODE", 1);
  jourMedlVal = ChooseSetting ("JOURMEDLMODE", 1);
  geneMedlVal = ChooseSetting ("GENEMEDLMODE", 1);
  ecnoMedlVal = ChooseSetting ("ECNOMEDLMODE", 1);

  wordProtVal = ChooseSetting ("WORDPROTMODE", 2);
  kywdProtVal = ChooseSetting ("KYWDPROTMODE", 1);
  authProtVal = ChooseSetting ("AUTHPROTMODE", 1);
  jourProtVal = ChooseSetting ("JOURPROTMODE", 1);
  orgnProtVal = ChooseSetting ("ORGNPROTMODE", 1);
  accnProtVal = ChooseSetting ("ACCNPROTMODE", 1);
  geneProtVal = ChooseSetting ("GENEPROTMODE", 1);
  protProtVal = ChooseSetting ("PROTPROTMODE", 1);
  ecnoProtVal = ChooseSetting ("ECNOPROTMODE", 1);

  wordNuclVal = ChooseSetting ("WORDNUCLMODE", 2);
  kywdNuclVal = ChooseSetting ("KYWDNUCLMODE", 1);
  authNuclVal = ChooseSetting ("AUTHNUCLMODE", 1);
  jourNuclVal = ChooseSetting ("JOURNUCLMODE", 1);
  orgnNuclVal = ChooseSetting ("ORGNNUCLMODE", 1);
  accnNuclVal = ChooseSetting ("ACCNNUCLMODE", 1);
  geneNuclVal = ChooseSetting ("GENENUCLMODE", 1);
  protNuclVal = ChooseSetting ("PROTNUCLMODE", 1);

  Show (mulSelTruMode);
  currentMode = mulSelTruMode;
  currentProc = NULL;

  db = TYP_ML;
  dbval = 1;
  medfldval = 1;
  prtfldval = 1;
  nucfldval = 1;
  if (GetAppParam ("ENTREZ", "SETTINGS", "DATABASE", "", str, sizeof (str))) {
    if (StrToInt (str, &dbval)) {
      if (dbval >= 1 && dbval <= 3 && popupToDb [dbval] != NTYPE) {
        SetValue (database, dbval);
      } else {
        dbval = 1;
      }
    }
  }
  if (popupToDb [1] == TYP_ML &&
      GetAppParam ("ENTREZ", "SETTINGS", "MEDFIELD", "", str, sizeof (str))) {
    if (StrToInt (str, &medfldval)) {
      SetValue (medlField, medfldval);
    }
  }
  if (popupToDb [2] == TYP_AA &&
      GetAppParam ("ENTREZ", "SETTINGS", "PRTFIELD", "", str, sizeof (str))) {
    if (StrToInt (str, &prtfldval)) {
      SetValue (protField, prtfldval);
    }
  }
  if (popupToDb [3] == TYP_NT &&
      GetAppParam ("ENTREZ", "SETTINGS", "NUCFIELD", "", str, sizeof (str))) {
    if (StrToInt (str, &nucfldval)) {
      SetValue (nuclField, nucfldval);
    }
  }
  db = ReturnDatabaseCode ();
  fld = ReturnFieldCode ();
  AlterDatabase (db);
  AlterField (db, fld);

  if (smallScreen) {
    Break (w);
    g = HiddenGroup (w, 2, 0, NULL);
    StaticPrompt (g, "Term:", 0, dialogTextHeight, systemFont, 'l');
    term = DialogText (g, "", 20, TextAction);
    Advance (w);
    accept = DefaultButton (w, "Accept", AcceptProc);
    Disable (accept);
    Break (w);
    availSlate = ScrollSlate (w, 28, 7);
  } else {
    Advance (w);
    GetNextPosition (w, &npt);
    npt.y = lpt.y;
    SetNextPosition (w, npt);
    accept = DefaultButton (w, "Accept", AcceptProc);
    Disable (accept);
    Break (w);
    g = HiddenGroup (w, 2, 0, NULL);
    StaticPrompt (g, "Term:", 0, dialogTextHeight, systemFont, 'l');
    term = DialogText (g, "", 30, TextAction);
    Break (w);
    availSlate = ScrollSlate (w, 33, 7);
  }
#ifdef PRODUCE_USERS_GUIDE
  hiddenText = DialogText (g, "", 1, NULL);
  Hide (hiddenText);
#endif
  available = TablePanel (availSlate, 0, systemFont, AvailableAction);
  RecreateAvailable ();
  if (smallScreen) {
    g = HiddenGroup (w, 8, 0, NULL);
    SetGroupMargins (g, 4, 0);
    SimplePanel (g, 16, stdLineHeight, DrawUpArrow);
    StaticPrompt (g, "Term Selection", stdCharWidth * 19 - 23,
                  stdLineHeight, systemFont, 'l');
    StaticPrompt (g, "Special", stdCharWidth * 4 - 5, stdLineHeight, systemFont, 'r');
    StaticPrompt (g, "Total", stdCharWidth * 5 - 5, stdLineHeight, systemFont, 'r');
  }
  totalPages = 0;
  currentRow = 0;
  selection [0] = '\0';
  firstPage = 0;
  numRows = 0;
  visLines = TableVisLines (available);
  pagesLoaded = FALSE;
  textChanged = FALSE;
  okayToAccept = FALSE;
  lookupViaTable = FALSE;
  lookupByUid = FALSE;
  timerCount = 0;
  autoLoad = TRUE;
  db = ReturnDatabaseCode ();
  fld = ReturnFieldCode ();
  SetActivate (w, ActivateTermProc);
}
