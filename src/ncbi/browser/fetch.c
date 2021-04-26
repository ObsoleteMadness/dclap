/*   fetch.c
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
* File Name:  fetch.c
*
* Author:  Kans, Schuler
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.31 $
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
#include <accentr.h>
#include "browser.h"

#define QUEUE_SIZE 20

typedef struct cacheData {
  DocUid   uid;
  Int4     weight;
  Int4     offset;
  Int2     numLines;
  Int2     startsAt;
  Boolean  checked;
  Boolean  noAbstract;
  Boolean  cached;
  Boolean  noSuchUid;
} CacheData, PNTR CachePtr;

typedef void (*IconProc) (RectPtr r, Int2 offset);

typedef struct cellData {
  Uint2  start;
  Int2   count;
} CellData, PNTR CellPtr;

typedef struct winContext {
  WindoW   win;
  DoC      doc;
  Boolean  isSequence;
  DocUid   uid;
  Int2     view;
} WinContext, PNTR WinContextPtr;

IteM  saveNameItem;
IcoN  progress;

Boolean  hasMed;
Boolean  hasProt;
Boolean  hasNuc;

static IteM        openUidsItem;

static PaneL       fetched;
static Int2        fetchedHeight;
static FonT        fetchedFont;
static Int2        linesPerIcon;
static Int2        visLines;

static ButtoN      browse;
static Boolean     saveMode = FALSE;
static GrouP       view;
static ButtoN      prev;
static ButtoN      next;

static LinkSetPtr  mergeLinkSet;
static Int4        numNbrs;

static Int2        currentDbase;
static Int2        currentCount;
static Int2        currentParents;
static Int2        totalNumLines;
static CachePtr    currentCache;
static CharPtr     cachedStrings [50];
static DocUid      cachedUids [50];
static Int2        numCached;
static Int2        lastChecked;
static Int2        firstShown;
static Int2        lineInto;
static Int2        lastShown;
static Int2        lineBackto;
static Boolean     drawBackward;
static Boolean     drawingFailed;
static RecT        failedRect;
static DocUid      getThese [QUEUE_SIZE];
static Int2        whichOne [QUEUE_SIZE];
static Int2        queuedItems [QUEUE_SIZE];
static Int2        queuedCount;
static Int2        queueTicks;

static IconProc    hasicondraw;
static IconProc    noicondraw;

static Char        historyFile [256];
static Int4        historyOffsets [100];
static Char        cacheFile [256];
static Int2        generations;
static Int2        present;

static IteM        weightItem;
static Boolean     showWeights = FALSE;
static Boolean     thereAreWeights = FALSE;

static IteM        persistItem;
static Boolean     parentsPersist = FALSE;

static WinContextPtr winContext = NULL;
static Int2        numContexts;

static CellPtr     cellPtr;
static Int2        numCells;
static Int2        numCols;
static Int2        numRows;

static Int2        colWidths [2] = {0, 0};
static Boolean     colWraps [2] = {TRUE, TRUE};

static short       errOption;

static void DrawBook (RectPtr r, Int2 offset);
static void DrawEmptyBook (RectPtr r, Int2 offset);
static void DrawProtein (RectPtr r, Int2 offset);
static void DrawDna (RectPtr r, Int2 offset);
static void DrawCheckBox (RectPtr r, Boolean checked);
static Int4 NeighborListLength (void);
static void SetPrevAndNextButtons (void);
static void SetSaveItem (void);
static void SetBrowseButton (void);
static void ReadDocuments (Int2 first, Int2 num, Int2 total);
static Boolean JustCache (DocSumPtr dsp, DocUid uid);
static Boolean CacheAndDraw (DocSumPtr dsp, DocUid uid);
static CharPtr RearrangeText (CharPtr text, CharPtr caption, Int2 colCount, Int2Ptr colWidths, BoolPtr colWraps, FonT font);
static void RecreateFetched (void);
static Int2 CacheDocument (Int2 curr);

extern void HideBrowseWindowProc (WindoW w)

{
  Int2  j;

  Hide (browseWindow);
  for (j = 0; j < QUEUE_SIZE; j++) {
    queuedItems [j] = 0;
  }
  queuedCount = 0;
  Disable (saveAllItem);
  Disable (saveNameItem);
}

extern void HideBrowseMenuProc (IteM i)

{
  Int2  j;

  Hide (browseWindow);
  for (j = 0; j < QUEUE_SIZE; j++) {
    queuedItems [j] = 0;
  }
  queuedCount = 0;
  Disable (saveAllItem);
  Disable (saveNameItem);
}

static Int2 GetItem (Int2 desired)

{
  Int2  left;
  Int2  mid;
  Int2  right;

  mid = 0;
  if (currentCache != NULL && currentCount > 0) {
    left = 0;
    right = currentCount - 1;
    while (left <= right) {
      mid = (left + right) / 2;
      if (desired < currentCache [mid].startsAt + currentCache [mid].numLines) {
        right = mid - 1;
      }
      if (desired >= currentCache [mid].startsAt) {
        left = mid + 1;
      }
    }
  }
  return mid;
}

static void AddToQueue (Int2 curr)

{
  Int2  i;

  i = 0;
  while (i < QUEUE_SIZE && queuedItems [i] != curr) {
    i++;
  }
  if (i >= QUEUE_SIZE) {
    i = 0;
    while (i < QUEUE_SIZE && queuedItems [i] != 0) {
      i++;
    }
    if (i < QUEUE_SIZE) {
      queuedItems [i] = curr;
      queuedCount++;
    }
  }
}

static void QueueRequest (Int2 start, Boolean forward, Boolean force)

{
  Int2  i;
  Int2  max;
  ErrDesc err;

  for (i = 0; i < QUEUE_SIZE; i++) {
    getThese [i] = 0;
    whichOne [i] = 0;
  }
  if (forward) {
    i = 0;
    max = 0;
    while (i < QUEUE_SIZE && start < currentCount && max < QUEUE_SIZE) {
      if (! currentCache [start].cached) {
        getThese [i] = currentCache [start].uid;
        whichOne [i] = start;
        i++;
      } else if (! force) {
        max = QUEUE_SIZE;
      }
      start++;
      max++;
    }
    if (i > 0) {
      threads++;
      ErrSetOpts (errOption, 0);
      EntrezDocSumListGet (i, currentDbase, getThese, CacheAndDraw);
      ErrSetOpts (ERR_ABORT, 0);
      if (errOption == ERR_IGNORE)
      {
        ErrFetch(&err); /* discard any errors */
      }
      threads--;
    }
  } else {
    i = 0;
    max = 0;
    while (i < QUEUE_SIZE && start >= 0 && max < QUEUE_SIZE) {
      if (! currentCache [start].cached) {
        getThese [i] = currentCache [start].uid;
        whichOne [i] = start;
        i++;
      } else if (! force) {
        max = QUEUE_SIZE;
      }
      start--;
      max++;
    }
    if (i > 0) {
      threads++;
      ErrSetOpts (errOption, 0);
      EntrezDocSumListGet (i, currentDbase, getThese, CacheAndDraw);
      ErrSetOpts (ERR_ABORT, 0);
      if (errOption == ERR_IGNORE)
      {
        ErrFetch(&err); /* discard any errors */
      }
      threads--;
    }
  }
}

static void ScrollAction (BaR sb, SlatE s, Int2 newval, Int2 oldval)

{
  Int2     count;
  Int2     curr;
  Int2     i;
  Boolean  inPage;
  Int2     pgDn;
  Int2     pgUp;
  Int2     pixels;
  Int2     prevCached;
  RecT     r;
  Int2     val;

  if (s != NULL && oldval != newval && currentCache != NULL &&
      threads == 0 && Visible (s) && AllParentsVisible (s)) {
    ObjectRect (fetched, &r);
    InsetRect (&r, 4, 4);
    pgUp = visLines - 1;
    pgDn = visLines - 1;
    inPage = (Boolean) ((newval > oldval && newval - pgUp <= oldval) ||
                        (newval < oldval && newval + pgDn >= oldval));
    Select (fetched);
    if (inPage) {
      pixels = (oldval - newval) * fetchedHeight;
      r.bottom = r.top + fetchedHeight * visLines;
      ScrollRect (&r, 0, pixels);
      ObjectRect (fetched, &r);
      InsetRect (&r, 4, 4);
      r.top += fetchedHeight * visLines;
      InsetRect (&r, -1, -1);
      InvalRect (&r);
    } else {
      InsetRect (&r, -1, -1);
      InvalRect (&r);
    }
    LoadRect (&failedRect, r.left, r.top, r.right, r.bottom);
    InsetRect (&failedRect, 1, 1);
    drawBackward = FALSE;
    firstShown = 0;
    lineInto = 0;
    lastShown = 0;
    lineBackto = 0;
    if (inPage) {
      if (newval < oldval) {
        drawBackward = TRUE;
        lastShown = GetItem (newval + visLines - 1);
        lineBackto = currentCache [lastShown].startsAt +
                     currentCache [lastShown].numLines -
                     newval - visLines;
        Update ();
        if (drawingFailed) {
          QueueRequest (lastShown, FALSE, drawingFailed);
        }
      } else {
        firstShown = GetItem (newval);
        lineInto = newval - currentCache [firstShown].startsAt;
        Update ();
        if (drawingFailed) {
          QueueRequest (firstShown, TRUE, drawingFailed);
        }
      }
    } else {
      if (newval < oldval) {
        firstShown = GetItem (newval);
        lineInto = newval - currentCache [firstShown].startsAt;
        Update ();
        if (drawingFailed) {
          QueueRequest (firstShown, TRUE, drawingFailed);
        }
      } else if (newval == totalNumLines - visLines) {
        drawBackward = TRUE;
        lastShown = currentCount - 1;
        lineBackto = 0;
        Update ();
        if (drawingFailed) {
          QueueRequest (lastShown, FALSE, drawingFailed);
        }
      } else {
        firstShown = GetItem (newval);
        lineInto = 0;
        Update ();
        if (drawingFailed) {
          QueueRequest (firstShown, TRUE, drawingFailed);
        }
      }
    }
    if (drawingFailed && failedRect.bottom >= failedRect.top) {
      InsetRect (&failedRect, -1, -1);
      InvalRect (&failedRect);
    }
    for (i = 0; i < QUEUE_SIZE; i++) {
      queuedItems [i] = 0;
    }
    queuedCount = 0;
    prevCached = numCached;
    if (drawingFailed) {
      Update ();
    }
    count = MAX (10, numCached - prevCached);
    if (drawBackward) {
      curr = firstShown - 1;
      while (count > 0 && curr >= 0) {
        AddToQueue (curr);
        curr--;
        count--;
      }
    } else {
      curr = lastShown + 1;
      while (count > 0 && curr < currentCount) {
        AddToQueue (curr);
        curr++;
        count--;
      }
    }
    drawBackward = FALSE;
    val = currentCache [firstShown].startsAt + lineInto;
    CorrectBarMax (sb, MAX (0, totalNumLines - visLines));
    CorrectBarValue (sb, val);
  }
}

static void SetTotalNumLines (void)

{
  Int2  i;

  totalNumLines = 0;
  if (currentCache != NULL) {
    for (i = 0; i < currentCount; i++) {
      currentCache [i].startsAt = totalNumLines;
      totalNumLines += currentCache [i].numLines;
    }
  }
}

static Int2 CacheDocument (Int2 curr)

{
  Int2     cache;
  Char     ch;
  CharPtr  chptr;
  FILE     *f;
  Int2     j;
  Int2     len;
  Int2     numLines;
  Int4     offset;
  DocUid   uid;

  cache = 0;
  if (currentCache != NULL) {
    uid = currentCache [curr].uid;
    cache = 0;
    while (cache < 50 && cachedUids [cache] != uid) {
      cache++;
    }
    if (cache >= 50) {
      if (cachedUids[0] != 0 || cachedStrings[0] != NULL)
      { /* if the first entry was bad, just overwrite that one instead of */
        /* pushing out more potentially good entries                      */
        chptr = cachedStrings [49];
        if (chptr != NULL) {
          cachedStrings [49] = (CharPtr) MemFree (chptr);
        }
        for (j = 49; j > 0; j--) {
          cachedUids [j] = cachedUids [j - 1];
          cachedStrings [j] = cachedStrings [j - 1];
        }
        cachedUids [0] = 0;
        cachedStrings [0] = NULL;
      }
      numLines = 0;
      chptr = NULL;
      offset = currentCache [curr].offset;
      if (currentCache[curr].cached && offset != 0 && cacheFile [0] != '\0') {
        f = FileOpen (cacheFile, "rb");
        if (f != NULL) {
          fseek (f, offset, SEEK_SET);
          FileRead (&len, sizeof (len), 1, f);
          chptr = (CharPtr) MemNew (len * sizeof (Char) + 2);
          if (chptr != NULL) {
            FileRead (chptr, sizeof (Char), (size_t) len, f);
          }
          FileClose (f);
        }
      }
      if (chptr != NULL && currentCache [curr].cached) {
        cachedStrings [0] = chptr;
        cachedUids [0] = uid;
        cache = 0;
        numLines = 0;
        chptr = cachedStrings [cache];
        if (chptr != NULL) {
          ch = *chptr;
          while (ch != '\0' && ch != '\n') {
            chptr++;
            ch = *chptr;
          }
          if (ch != '\0') {
            chptr++;
            ch = *chptr;
          }
          ch = *chptr;
          while (ch != '\0') {
            if (ch == '\n') {
              numLines++;
            }
            chptr++;
            ch = *chptr;
          }
        }
      } else {
        cache = -1;
      }
      if (! currentCache[curr].noSuchUid)
      {
        if (curr == currentCount - 1) {
          currentCache [curr].numLines = MAX (numLines, linesPerIcon);
        } else {
          currentCache [curr].numLines = MAX (numLines + 1, linesPerIcon + 1);
        }
        SetTotalNumLines ();
      }
    }
  }
  return cache;
}

static Int2 DrawASummary (Int2 curr, Int2 first, Int2 line)

{
  Int2     cache;
  Char     ch;
  Int2     col;
  Int2     count;
  Int2     lower;
  Int2     num;
  CharPtr  ptr;
  RecT     q;
  RecT     r;
  Int2     row;
  Int2     upper;

  count = 0;
  if (currentCache != NULL) {
    cache = CacheDocument (curr);
    if (cache >= 0 && currentCache [curr].cached && currentCache [curr].uid != 0) {
      ObjectRect (fetched, &r);
      InsetRect (&r, 4, 4);
      r.bottom = r.top + visLines * fetchedHeight;
      r.top += line * fetchedHeight;
      upper = r.top;
      lower = r.top;
      if (first == 0) {
        LoadRect (&q, r.left, r.top, r.left + MIN (fetchedHeight, 12),
                  r.top + fetchedHeight);
        if (RectInRect (&q, &r) && RectInRgn (&q, updateRgn)) {
          DrawCheckBox (&q, currentCache [curr].checked);
          lower = MAX (lower, q.bottom);
        }
      }
      if (first < linesPerIcon) {
        LoadRect (&q, r.left + 3 * stdCharWidth - 8, r.top,
                  r.left + 3 * stdCharWidth + 24,
                  MIN (r.bottom, r.top +  22 - first * fetchedHeight));
        if (RectInRect (&q, &r) && RectInRgn (&q, updateRgn)) {
          if (currentCache [curr].noAbstract) {
            noicondraw (&q, 4 * first * fetchedHeight);
          } else {
            hasicondraw (&q, 4 * first * fetchedHeight);
          }
          lower = MAX (lower, q.bottom);
        }
      }
      ObjectRect (fetched, &r);
      InsetRect (&r, 4, 4);
      r.bottom = r.top + visLines * fetchedHeight;
      ptr = cachedStrings [cache];
      if (ptr != NULL) {
        ch = *ptr;
        while (ch != '\0' && ch != '\n') {
          ptr++;
          ch = *ptr;
        }
        if (ch != '\0') {
          ptr++;
          ch = *ptr;
        }
        row = 0;
        col = 0;
        q.left = r.left + stdCharWidth * 6;
        q.right = q.left + colWidths [0];
        SelectFont (fetchedFont);
        while (ch != '\0' && line + row < visLines) {
          if (ch == '\n') {
            q.left = r.left + stdCharWidth * 6;
            count++;
            col = 0;
            row++;
            q.right = q.left + colWidths [col];
            ptr++;
          } else if (ch == '\t') {
            q.left += colWidths [col];
            col++;
            q.right = q.left + colWidths [col];
            ptr++;
          } else {
            num = 0;
            while (ch != '\0' && ch != '\t' && ch != '\n') {
              num++;
              ch = ptr [num];
            }
            if (num > 0 && row >= first) {
              q.top = r.top + (line + row - first) * fetchedHeight;
              q.bottom = q.top + fetchedHeight;
              if (RectInRect (&q, &r) && RectInRgn (&q, updateRgn)) {
                InsetRect (&q, 5, 0);
                DrawText (&q, ptr, num, 'l', FALSE);
                lower = MAX (lower, q.bottom);
                InsetRect (&q, -5, 0);
              }
            }
            ptr += num;
          }
          ch = *ptr;
        }
      }
      if (first == 0 && curr < currentParents) {
        ObjectRect (fetched, &r);
        InsetRect (&r, 4, 4);
        r.bottom = r.top + visLines * fetchedHeight;
        r.top += line * fetchedHeight;
        q.left = r.left + stdCharWidth * 6;
        q.right = q.left + 4;
        q.top = r.top + (fetchedHeight - 4) / 2;
        q.bottom = q.top + 4;
        if (RectInRect (&q, &r) && RectInRgn (&q, updateRgn)) {
          PaintOval (&q);
          lower = MAX (lower, q.bottom);
        }
      }
      if (drawBackward) {
        failedRect.bottom = MIN (failedRect.bottom, upper);
      } else {
        failedRect.top = MAX (failedRect.top, lower);
      }
      SelectFont (systemFont);
      count = currentCache [curr].numLines - first;
    }
  }
  return count;
}

static void DrawForward (void)

{
  Int2  cache;
  Int2  curr;
  Int2  first;
  Int2  line;

  curr = firstShown;
  first = lineInto;
  line = 0;
  while (line < visLines && curr < currentCount) {
    cache = CacheDocument (curr);
    if (cache >= 0) {
      DrawASummary (curr, first, line);
    } else {
      drawingFailed = TRUE;
    }
    line += currentCache [curr].numLines - first;
    if (line >= visLines) {
      lastShown = curr;
      lineBackto = line - visLines;
    }
    curr++;
    first = 0;
  }
}

static void DrawBackward (void)

{
  Int2  cache;
  Int2  curr;
  Int2  last;
  Int2  line;

  curr = lastShown;
  last = lineBackto;
  line = visLines;
  while (line >= 0 && curr >= 0) {
    cache = CacheDocument (curr);
    line -= currentCache [curr].numLines - last;
    if (line < 0) {
      if (cache >= 0) {
        DrawASummary (curr, -line, 0);
      } else {
        drawingFailed = TRUE;
      }
      firstShown = curr;
      lineInto = -line;
    } else {
      if (cache >= 0) {
        DrawASummary (curr, 0, line);
      } else {
        drawingFailed = TRUE;
      }
    }
    curr--;
    last = 0;
  }
}

static void DrawFetched (PaneL p)

{
  drawingFailed = FALSE;
  if (currentCache != NULL) {
    if (drawBackward) {
      DrawBackward ();
    } else {
      DrawForward ();
    }
  }
}

extern void FetchTimerProc (void)

{
  BaR  sb;
  ErrDesc err;

  queueTicks++;
  if (threads == 0 && queueTicks > 10 && currentCache != NULL && queuedCount > 0) {
    queueTicks = 0;
    threads++;
    ErrSetOpts (errOption, 0);
    queuedCount = EntrezDocSumListGet (QUEUE_SIZE, currentDbase, NULL, JustCache);
    ErrSetOpts (ERR_ABORT, 0);
    if (errOption == ERR_IGNORE)
    {
      ErrFetch(&err); /* discard any errors */
    }
    threads--;
    sb = GetSlateVScrollBar ((SlatE) fetched);
    CorrectBarMax (sb, MAX (0, totalNumLines - visLines));
    CorrectBarValue (sb, currentCache [firstShown].startsAt + lineInto);
  }
}

static void RetrieveGeneration (Int2 num, Int2 parents, DocUidPtr uids, Int4Ptr wgts, Int2 dbase)

{
  Int2     count;
  Int2     curr;
  FILE     *f;
  Int2     i;
  CharPtr  str;
  Int2     val;

  Enable (saveNameItem);
  num = MIN (num, 1000);
  if (cacheFile [0] == '\0') {
    TmpNam (cacheFile);
#ifdef WIN_MAC
    if (cacheFile [0] != '\0') {
      FileCreate (cacheFile, "????", "ENTZ");
    }
#endif
  }
  if (cacheFile [0] != '\0') {
    f = FileOpen (cacheFile, "wb");
    if (f != NULL) {
      fseek (f, 0, SEEK_SET);
      val = currentDbase;
      FileWrite (&val, sizeof (val), 1, f);
      FileClose (f);
    }
  }
  thereAreWeights = (Boolean) (wgts != NULL);
  ResetFetchedList ();
  Disable (browse);
  Disable (openItem);
  Disable (saveItem);
  Disable (saveAsItem);
  Enable (saveAllItem);
  Enable (saveNameItem);
  Disable (printItem);
  SetTitle (browse, "Neighbor 0");
  currentDbase = dbase;
  switch (dbase) {
    case TYP_ML:
      SetValue (view, 1);
      break;
    case TYP_AA:
      SetValue (view, 2);
      break;
    case TYP_NT:
      SetValue (view, 3);
      break;
    default:
      SetValue (view, 0);
      break;
  }
  Show (browseWindow);
  Select (browseWindow);
  Enable (saveAllItem);
  Enable (saveNameItem);
  if (currentCache != NULL) {
    currentCache = (CachePtr) MemFree (currentCache);
  }
  for (i = 0; i < 50; i++) {
    str = cachedStrings [i];
    if (str != NULL) {
      cachedStrings [i] = (CharPtr) MemFree (str);
    }
    cachedUids [i] = 0;
  }
  for (i = 0; i < QUEUE_SIZE; i++) {
    queuedItems [i] = 0;
    getThese [i] = 0;
    whichOne [i] = 0;
  }
  queuedCount = 0;
  queueTicks = 0;
  currentCount = num;
  currentParents = parents;
  currentDbase = dbase;
  lastChecked = -1;
  numCached = 0;
  firstShown = 0;
  lineInto = 0;
  lastShown = 0;
  lineBackto = 0;
  drawBackward = FALSE;
  currentCache = (CachePtr) MemNew (sizeof (CacheData) * num);
  if (currentCache != NULL) {
    for (i = 0; i < num; i++) {
      currentCache [i].uid = uids [i];
      currentCache [i].weight = 0;
      currentCache [i].offset = 0;
      currentCache [i].checked = FALSE;
      currentCache [i].noAbstract = FALSE;
      currentCache [i].cached = FALSE;
      currentCache [i].noSuchUid = FALSE;
      currentCache [i].numLines = MAX (3, linesPerIcon + 1);
    }
    if (num > 0) {
      currentCache [num - 1].numLines = MAX (2, linesPerIcon);
    }
    if (wgts != NULL) {
      for (i = 0; i < num; i++) {
        currentCache [i].weight = wgts [i];
      }
    }
  }
  SetTotalNumLines ();
  for (i = 0; i < 50; i++) {
    if (cachedStrings [i] != NULL) {
      cachedStrings [i] = NULL;
    }
    cachedUids [i] = 0;
  }
  switch (currentDbase) {
    case TYP_ML:
      hasicondraw = DrawBook;
      noicondraw = DrawEmptyBook;
      break;
    case TYP_AA:
      hasicondraw = DrawProtein;
      noicondraw = DrawProtein;
      break;
    case TYP_NT:
      hasicondraw = DrawDna;
      noicondraw = DrawDna;
      break;
    default:
      hasicondraw = NULL;
      noicondraw = NULL;
      break;
  }
  if (num > 0) {
    RecreateFetched ();
    Update ();
    QueueRequest (0, TRUE, TRUE);
    SetRange (fetched, visLines - 1, visLines - 1, MAX (0, totalNumLines - visLines));
    count = MAX (10, numCached);
    curr = lastShown + 1;
    while (count > 0 && curr < currentCount) {
      AddToQueue (curr);
      curr++;
      count--;
    }
  } else {
    RecreateFetched ();
    Hide (browseWindow);
    Select (termWindow);
  }
  SetPrevAndNextButtons ();
  SetSaveItem ();
  SetBrowseButton ();
}

static void RetrieveTheDocuments (Int2 num, Int2 parents, DocUidPtr uids, Int4Ptr wgts, Int2 dbase)

{
  FILE  *f;
  Int2  val;

  generations++;
  present = generations;
  if (historyFile [0] == '\0') {
    TmpNam (historyFile);
#ifdef WIN_MAC
    if (historyFile [0] != '\0') {
      FileCreate (historyFile, "????", "ENTZ");
    }
#endif
  }
  if (historyFile [0] != '\0') {
    f = FileOpen (historyFile, "ab");
    if (f != NULL) {
      fseek (f, 0, SEEK_END);
      historyOffsets [present % 100] = ftell (f);
      val = dbase;
      FileWrite (&val, sizeof (val), 1, f);
      val = num;
      FileWrite (&val, sizeof (val), 1, f);
      val = parents;
      FileWrite (&val, sizeof (val), 1, f);
      FileWrite (uids, sizeof (DocUid), (size_t) num, f);
      FileClose (f);
    } else {
      historyOffsets [present % 100] = 0;
    }
  }
  RetrieveGeneration (num, parents, uids, wgts, dbase);
}

extern void RetrieveDocuments (Int2 num, DocUidPtr uids, Int4Ptr wgts, Int2 dbase)

{
  RetrieveTheDocuments (num, 0, uids, wgts, dbase);
}

static Int2 GetNextBlock (CharPtr title, Int2 maxwid)

{
  Char  ch;
  Int2  i;
  Int2  j;
  Int2  wid;

  i = 0;
  j = 0;
  wid = 0;
  if (title != NULL && maxwid > 0) {
    ch = title [i];
    while (ch != '\0' && ch != '\n' && ch != '\r' &&
           ch != '\t' && wid <= maxwid) {
      wid += CharWidth (ch);
      i++;
      ch = title [i];
    }
    j = i;
    if (wid > maxwid) {
      j--;
      while (TextWidth (title, i) > maxwid) {
        i--;
      }
      while (i > 0 && title [i - 1] != ' ' && title [i - 1] != '-' && title [i - 1] != '_') {
        i--;
      }
      while (title [i] == ' ') {
        i++;
      }
    }
  }
  if (i > 0 && i < j) {
    return i;
  } else if (j > 0) {
    return j;
  } else {
    return 0;
  }
}

static void RecordCell (Uint2 start, Int2 len, Int2 row, Int2 col)

{
  Int2   cell;
  Int2   newCells;

  if (col < numCols) {
    cell = (Int2) ((Int4) row * (Int4) numCols + (Int4) col);
    if (cell >= numCells) {
      newCells = (cell / 128 + 1) * 128;
      if (cellPtr != NULL) {
        cellPtr = (CellPtr) MemMore (cellPtr, sizeof (CellData) * newCells);
        if (cellPtr != NULL) {
          while (numCells < newCells) {
            cellPtr [numCells].start = 0;
            cellPtr [numCells].count = 0;
            numCells++;
          }
        }
      } else {
        cellPtr = (CellPtr) MemNew (sizeof (CellData) * newCells);
      }
      numCells = newCells;
    }
    if (cellPtr != NULL) {
      cellPtr [cell].start = start;
      cellPtr [cell].count = len;
    }
  }
}

static CharPtr RearrangeText (CharPtr text, CharPtr caption, Int2 colCount,
                              Int2Ptr colWidths, BoolPtr colWraps, FonT font)

{
  Int2     blklen;
  Int2     caplen;
  Int2     cell;
  Char     ch;
  Int2     col;
  CharPtr  dst;
  Int2     len;
  Int2     maxrow;
  Int2     maxwid;
  Int2     returnRow;
  Int2     row;
  CharPtr  src;
  Uint2    start;
  CharPtr  str;
  Int2     tabRow;

  str = NULL;
  numCells = 0;
  numRows = 0;
  numCols = colCount;
  cellPtr = NULL;
  if (text != NULL && colWidths != NULL && colWraps != NULL && numCols > 0) {
    if (font != NULL) {
      SelectFont (font);
    }
    start = 0;
    row = 0;
    tabRow = 0;
    returnRow = 1;
    col = 0;
    while (text [start] != '\0') {
      while (text [start] == ' ') {
        start++;
      }
      ch = text [start];
      if (ch != '\0' && ch != '\n' && ch != '\r' && ch != '\t') {
        maxwid = INT2_MAX;
        if (col < numCols && colWraps [col] && colWidths [col] >= 12) {
          maxwid = colWidths [col] - 12;
        }
        blklen = GetNextBlock (text + start, maxwid);
        len = blklen;
        if (len > 0) {
          if (text [start + len] != '\0') {
            while (len > 0 && text [start + len - 1] == ' ') {
              len--;
            }
          }
          RecordCell (start, len, row, col);
          start += blklen;
        } else {
          while (TextWidth (text + start, len) <= maxwid) {
            len++;
          }
          len--;
          if (len > 0) {
            RecordCell (start, len, row, col);
          }
          ch = text [start];
          while (ch != '\0' && ch != '\n' && ch != '\r' && ch != '\t') {
            start++;
            ch = text [start];
          }
        }
      }
      ch = text [start];
      if (ch == '\n') {
        start++;
        row = returnRow;
        tabRow = row;
        returnRow++;
        col = 0;
      } else if (ch == '\r') {
        start++;
        row++;
        returnRow = MAX (returnRow, row + 1);
      } else if (ch == '\t') {
        start++;
        row = tabRow;
        col++;
      } else if (ch != '\0' && colWraps != NULL && colWraps [col]) {
        if (len == 0) {
          start++;
        }
        row++;
        returnRow = MAX (returnRow, row + 1);
      } else if (ch != '\0') {
        start++;
      }
    }
    numRows = row;
    SelectFont (systemFont);
    if (numCells > 0 && cellPtr != NULL) {
      len = numRows * numCols;
      maxrow = 0;
      for (row = 0; row < numRows; row++) {
        for (col = 0; col < numCols; col++) {
          cell = (Int2) ((Int4) row * (Int4) numCols + (Int4) col);
          len += cellPtr [cell].count;
          if (cellPtr [cell].count > 0) {
            maxrow = row + 1;
          }
        }
      }
      src = caption;
      if (src != NULL) {
        caplen = StringLen (src);
      } else {
        caplen = 0;
      }
      str = (CharPtr) MemNew ((size_t) (len + caplen + 3 + 1));
      dst = str;
      if (src != NULL) {
        while (caplen > 0) {
          *dst = *src;
          dst++;
          src++;
          caplen--;
        }
      }
      *dst = '\n';
      dst++;
      for (row = 0; row < maxrow; row++) {
        for (col = 0; col < numCols; col++) {
          cell = (Int2) ((Int4) row * (Int4) numCols + (Int4) col);
          if (col > 0) {
            *dst = '\t';
            dst++;
          }
          src = text + cellPtr [cell].start;
          len = cellPtr [cell].count;
          while (len > 0) {
            *dst = *src;
            dst++;
            src++;
            len--;
          }
        }
        *dst = '\n';
        dst++;
      }
      *dst = '\0';
    }
  }
  if (cellPtr != NULL) {
    cellPtr = (CellPtr) MemFree (cellPtr);
  }
  return str;
}

static Boolean JustCache (DocSumPtr dsp, DocUid uid)

{
  Int2     curr;
  FILE     *f;
  Boolean  goOn;
  Int2     i;
  Int2     j;
  Int2     len;
  CharPtr  rsult;
  Char     str [1024];
  Char     strdoc [16];
  Int4     weight;

  goOn = FALSE;
  rsult = NULL;
  if (currentCache != NULL) {
    i = 0;
    while (i < QUEUE_SIZE && getThese [i] != uid) {
      i++;
    }
    if (i < QUEUE_SIZE) {
      curr = whichOne [i];
    }
    if (i < QUEUE_SIZE && (! currentCache [curr].cached)) {
      if (dsp != NULL) {
        str [0] = '\0';
        currentCache [curr].noAbstract = dsp->no_abstract;
        if (dsp->caption != NULL) {
          StringCat (str, dsp->caption);
        }
        j = 0;
        while (str [j] != '\0' && str [j] != ',') {
          j++;
        }
        if (str [j] == ',' && str [j + 1] == ' ') {
          str [j + 1] = '\r';
        }
        StringCat (str, "\r");
        if (showWeights && thereAreWeights) {
          weight = currentCache [curr].weight;
          if (currentDbase == TYP_AA || currentDbase == TYP_NT) {
            if (weight > 0) {
              LongToStr (weight, strdoc, 1, sizeof (strdoc));
              StringCat (str, strdoc);
            }
          }
        }
        StringCat (str, "\t");
        j = 0;
        if (dsp->title != NULL) {
          while (dsp->title [j] != '\0') {
            if (dsp->title [j] == '\n' || dsp->title [j] == '\r') {
              dsp->title [j] = ' ';
            }
            j++;
          }
          StringCat (str, dsp->title);
        }
        StringCat (str, "\n");
        colWidths [0] = 7 * stdCharWidth - 12;
        if (smallScreen) {
          colWidths [1] = 14 * stdCharWidth - 12;
        } else {
          colWidths [1]  = 19 * stdCharWidth - 12;
        }
        rsult = RearrangeText (str, dsp->caption, 2, colWidths, colWraps, fetchedFont);
        threads++;
        dsp = DocSumFree (dsp);
        threads--;
        if (cacheFile [0] != '\0') {
          f = FileOpen (cacheFile, "ab");
          if (f != NULL) {
            fseek (f, 0, SEEK_END);
            currentCache [curr].offset = ftell (f);
            len = (Int2) StringLen (rsult);
            FileWrite (&len, sizeof (len), 1, f);
            FileWrite (rsult, sizeof (Char), (size_t) len, f);
            FileClose (f);
            currentCache [curr].cached = TRUE;
            numCached++;
          }
        }
        rsult = MemFree (rsult);
      } else {
        /*
        currentCache [curr].uid = 0;
        currentCache [curr].weight = 0;
        currentCache [curr].offset = 0;
        currentCache [curr].checked = FALSE;
        currentCache [curr].noAbstract = FALSE;
        currentCache [curr].numLines = 0;
        currentCache [curr].cached = FALSE;
        */
        currentCache [curr].uid = uid;
        currentCache [curr].weight = 0;
        currentCache [curr].offset = 0;
        currentCache [curr].checked = FALSE;
        currentCache [curr].noAbstract = FALSE;
        currentCache [curr].cached = FALSE;
        currentCache [curr].noSuchUid = TRUE;
        currentCache [curr].numLines = 0;
        SetTotalNumLines ();
      }
    }
  }
  return goOn;
}

static Boolean CacheAndDraw (DocSumPtr dsp, DocUid uid)

{
  Int2     cache;
  Int2     curr;
  RecT     dr;
  Int2     first;
  Boolean  goOn;
  Int2     i;
  Int2     last;
  Int2     line;
  RecT     r;
  CharPtr  rsult;
  RecT     sr;
  WindoW   w;

  goOn = FALSE;
  rsult = NULL;
  if (currentCache != NULL) {
    i = 0;
    while (i < QUEUE_SIZE && getThese [i] != uid) {
      i++;
    }
    if (i < QUEUE_SIZE) {
      curr = whichOne [i];
    }
    if (i < QUEUE_SIZE && (! currentCache [curr].cached)) {
      JustCache (dsp, uid);
      cache = CacheDocument (curr);
      if (dsp == NULL)
      {
        currentCache[curr].numLines = 0;
        currentCache[curr].noSuchUid = TRUE;
        goOn = TRUE;
      }
      if (cache >= 0) {
        w = ParentWindow (fetched);
        if (CurrentWindow () != w) {
          UseWindow (w);
        }
        Select (fetched);
        ObjectRect (fetched, &r);
        InsetRect (&r, 4, 4);
        ObjectRect (fetched, &sr);
        InsetRect (&sr, 4, 4);
        if (drawBackward) {
          i = lastShown;
          last = lineBackto;
          line = visLines;
          while (line >= 0 && i >= 0) {
            cache = CacheDocument (i);
            line -= currentCache [i].numLines - last;
            if (i == curr) {
              if (line < 0) {
                r.bottom = r.top + fetchedHeight * (currentCache [curr].numLines + line);
              } else {
                r.top += line * fetchedHeight;
                r.bottom = r.top + fetchedHeight * currentCache [curr].numLines;
              }
              SectRect (&r, &sr, &dr);
              if (RectInRect (&dr, &sr)) {
                InsetRect (&dr, -1, -1);
                InvalRect (&dr);
                goOn = TRUE;
              }
            }
            if (line < 0) {
              firstShown = i;
              lineInto = -line;
            }
            i--;
            last = 0;
          }
        } else {
          i = firstShown;
          first = lineInto;
          line = 0;
          while (line < visLines && i < currentCount) {
            cache = CacheDocument (i);
            if (i == curr) {
              r.top += line * fetchedHeight;
              r.bottom = r.top + fetchedHeight * currentCache [curr].numLines;
              SectRect (&r, &sr, &dr);
              if (RectInRect (&dr, &sr)) {
                InsetRect (&dr, -1, -1);
                InvalRect (&dr);
                goOn = TRUE;
              }
            }
            line += currentCache [i].numLines - first;
            if (line > visLines) {
              lastShown = i;
              lineBackto = line - visLines;
            }
            i++;
            first = 0;
          }
        }
        Update ();
      }
    }
  }
  return goOn;
}

static Uint1  hasabstract [] = {
    0x07, 0xF8, 0x1F, 0xE0, 0x78, 0x07, 0xE0, 0x1E,
    0x40, 0x01, 0x80, 0x02, 0x40, 0x01, 0x80, 0x02,
    0x40, 0x01, 0x80, 0x02, 0x4F, 0xF9, 0x9F, 0xF2,
    0x40, 0x01, 0x80, 0x02, 0x4F, 0xF9, 0x9F, 0xF2,
    0x40, 0x01, 0x80, 0x02, 0x4F, 0xF9, 0x9F, 0xF2,
    0x40, 0x01, 0x80, 0x02, 0x4F, 0xF9, 0x9F, 0xF2,
    0x40, 0x01, 0x80, 0x02, 0x4F, 0xF9, 0x9F, 0xF2,
    0x40, 0x01, 0x80, 0x02, 0x4F, 0xF9, 0x9F, 0xF2,
    0x40, 0x01, 0x80, 0x02, 0x40, 0x01, 0x80, 0x02,
    0x40, 0x01, 0x80, 0x02, 0x47, 0xF9, 0x9F, 0xE2,
    0x7F, 0xFF, 0xFF, 0xFE, 0x7F, 0xFF, 0xFF, 0xFE
  };

static Uint1  noabstract [] = {
    0x07, 0xF8, 0x1F, 0xE0, 0x78, 0x07, 0xE0, 0x1E,
    0x40, 0x01, 0x80, 0x02, 0x40, 0x01, 0x80, 0x02,
    0x40, 0x01, 0x80, 0x02, 0x40, 0x01, 0x80, 0x02,
    0x40, 0x01, 0x80, 0x02, 0x40, 0x01, 0x80, 0x02,
    0x40, 0x01, 0x80, 0x02, 0x40, 0x01, 0x80, 0x02,
    0x40, 0x01, 0x80, 0x02, 0x40, 0x01, 0x80, 0x02,
    0x40, 0x01, 0x80, 0x02, 0x40, 0x01, 0x80, 0x02,
    0x40, 0x01, 0x80, 0x02, 0x40, 0x01, 0x80, 0x02,
    0x40, 0x01, 0x80, 0x02, 0x40, 0x01, 0x80, 0x02,
    0x40, 0x01, 0x80, 0x02, 0x47, 0xF9, 0x9F, 0xE2,
    0x7F, 0xFF, 0xFF, 0xFE, 0x7F, 0xFF, 0xFF, 0xFE
  };

static Uint1  proteinicon [] = {
    0x7F, 0xFF, 0xFF, 0xFE, 0x40, 0x00, 0x00, 0x02,
    0x40, 0x00, 0x00, 0x02, 0x43, 0x83, 0x80, 0x02,
    0x44, 0x44, 0x40, 0x02, 0x43, 0xC3, 0xC0, 0x02,
    0x44, 0x44, 0x40, 0x02, 0x44, 0x44, 0x40, 0x02,
    0x44, 0x44, 0x40, 0x02, 0x43, 0xE3, 0xE0, 0x02,
    0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02,
    0x40, 0x00, 0x00, 0x02, 0x47, 0xE7, 0xE7, 0xE2,
    0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02,
    0x47, 0xE7, 0xE7, 0xE2, 0x40, 0x00, 0x00, 0x02,
    0x40, 0x00, 0x00, 0x02, 0x47, 0xE7, 0xE7, 0xE2,
    0x40, 0x00, 0x00, 0x02, 0x7F, 0xFF, 0xFF, 0xFE
  };

static Uint1  dnaicon [] = {
    0x7F, 0xFF, 0xFF, 0xFE, 0x40, 0x00, 0x00, 0x02,
    0x40, 0x00, 0x00, 0x02, 0x45, 0x81, 0x00, 0x02,
    0x46, 0x41, 0x00, 0x02, 0x44, 0x47, 0xC0, 0x02,
    0x44, 0x41, 0x00, 0x02, 0x44, 0x41, 0x00, 0x02,
    0x44, 0x41, 0x00, 0x02, 0x44, 0x60, 0xC0, 0x02,
    0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02,
    0x40, 0x00, 0x00, 0x02, 0x4F, 0x7B, 0xDE, 0xF2,
    0x40, 0x00, 0x00, 0x02, 0x4F, 0x7B, 0xDE, 0xF2,
    0x40, 0x00, 0x00, 0x02, 0x4F, 0x7B, 0xDE, 0xF2,
    0x40, 0x00, 0x00, 0x02, 0x4F, 0x7B, 0xDE, 0xF2,
    0x40, 0x00, 0x00, 0x02, 0x7F, 0xFF, 0xFF, 0xFE
  };

static Uint1  checkmark [] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
    0x00, 0x80, 0x01, 0x00, 0x22, 0x00, 0x14, 0x00,
    0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

static void DrawBook (RectPtr r, Int2 offset)

{
  if (r != NULL) {
    CopyBits (r, hasabstract + offset);
  }
}

static void DrawEmptyBook (RectPtr r, Int2 offset)

{
  if (r != NULL) {
    CopyBits (r, noabstract + offset);
  }
}

static void DrawProtein (RectPtr r, Int2 offset)

{
  if (r != NULL) {
    CopyBits (r, proteinicon + offset);
  }
}

static void DrawDna (RectPtr r, Int2 offset)

{
  if (r != NULL) {
    CopyBits (r, dnaicon + offset);
  }
}

static void DrawCheckBox (RectPtr r, Boolean checked)

{
  RecT  rct;

  if (r != NULL) {
    rct = *r;
    rct.bottom = rct.top + (rct.right - rct.left);
    FrameRect (&rct);
    if (checked) {
      MoveTo (rct.left, rct.top);
      LineTo (rct.right - 1, rct.bottom - 1);
      MoveTo (rct.left, rct.bottom - 1);
      LineTo (rct.right - 1, rct.top);
    }
  }
}

static void ProcessCaption (CharPtr str, Int2 maxchar, Int2 curr)

{
  Int2     cache;
  Char     ch;
  CharPtr  chptr;
  Int2     i;
  Char     title [64];

  if (currentCache != NULL && str != NULL) {
    str [0] = '\0';
    cache = CacheDocument (curr);
    if (cache >= 0) {
      i = 0;
      chptr = cachedStrings [cache];
      if (chptr != NULL) {
        ch = *chptr;
        while (ch != '\0' && ch != '\t' && ch != '\n' && i < sizeof (title) - 2) {
          title [i] = ch;
          i++;
          chptr++;
          ch = *chptr;
        }
      }
      title [i] = '\0';
      while (i > 0 && title [i - 1] <= ' ') {
        title [i - 1] = '\0';
        i--;
      }
      StringNCpy (str, title, maxchar);
    }
  }
}

static void PullupMedlineFromFile (IteM i)

{
  WatchCursor ();
  ErrSetOpts (ERR_CONTINUE, 0);
  threads++;
  LoadAbstract ("?", 0);
  threads--;
  ErrSetOpts (ERR_ABORT, 0);
  /*
  ErrClear ();
  */
  ErrShow ();
  ArrowCursor ();
}

static void PullupSequenceFromFile (IteM i)

{
  Int2  dbase;

  WatchCursor ();
  ErrSetOpts (ERR_CONTINUE, 0);
  threads++;
  dbase = GetValue (view) - 1;
  LoadSequence ("?", 0, (Boolean) (dbase == TYP_NT));
  threads--;
  ErrSetOpts (ERR_ABORT, 0);
  /*
  ErrClear ();
  */
  ErrShow ();
  ArrowCursor ();
}

extern void LoadOpenMedAndSeqFiles (MenU m)

{
  Char  str [32];

  if (GetAppParam ("ENTREZ", "PREFERENCES", "LOOKUPBYFILE", "", str, sizeof (str) - 1)) {
    if (StringICmp (str, "TRUE") == 0) {
      CommandItem (m, "Open MEDLINE File...", PullupMedlineFromFile);
      CommandItem (m, "Open Sequence File...", PullupSequenceFromFile);
      SeparatorItem (m);
    }
  }
}

static void PullupAbstract (Int2 curr)

{
  Int2     cache;
  Char     title [64];
  DocUid   uid;

  if (currentCache != NULL) {
    WatchCursor ();
    ErrSetOpts (ERR_CONTINUE, 0);
    cache = CacheDocument (curr);
    if (cache >= 0) {
      uid = cachedUids [cache];
      ProcessCaption (title, sizeof (title), curr);
      threads++;
      switch (currentDbase) {
        case TYP_ML:
          LoadAbstract (title, uid);
          break;
        case TYP_AA:
          LoadSequence (title, uid, FALSE);
          break;
        case TYP_NT:
          LoadSequence (title, uid, TRUE);
          break;
        default:
          break;
      }
      threads--;
    }
    ErrSetOpts (ERR_ABORT, 0);
    /*
    ErrClear ();
    */
    ErrShow ();
    ArrowCursor ();
  }
}

static void FetchedClick (PaneL p, PoinT pt)

{
  Int2  curr;
  Int2  i;
  Int2  into;
  Int2  j;
  Int2  line;
  Int2  off;
  RecT  r;

  ObjectRect (fetched, &r);
  InsetRect (&r, 4, 4);
  if (PtInRect (pt, &r) && currentCache != NULL) {
    line = (pt.y - r.top) / fetchedHeight;
    if (line < visLines) {
      GetOffset (fetched, NULL, &off);
      curr = GetItem (off + line);
      into = off + line - currentCache [curr].startsAt;
      if (pt.x - r.left <= MIN (fetchedHeight, 12)) {
        if (into == 0) {
          i = line;
          j = line;
          if (currentCache [curr].checked) {
            currentCache [curr].checked = FALSE;
            lastChecked = -1;
          } else {
            currentCache [curr].checked = TRUE;
            if (shftKey && lastChecked >= 0 && curr < numCached) {
              i = MIN (lastChecked, curr);
              j = MAX (lastChecked, curr);
              if (i >= 0 && j < numCached && i < j) {
                while (i <= j) {
                  currentCache [i].checked = TRUE;
                  i++;
                }
                ObjectRect (fetched, &r);
                InsetRect (&r, 4, 4);
                r.right = r.left + MIN (fetchedHeight, 12);
                InsetRect (&r, -1, -1);
                InvalRect (&r);
              }
            }
            lastChecked = curr;
          }
          ObjectRect (fetched, &r);
          InsetRect (&r, 4, 4);
          r.top += line * fetchedHeight;
          r.bottom = r.top + fetchedHeight;
          r.right = r.left + MIN (fetchedHeight, 12);
          InsetRect (&r, -1, -1);
          InvalRect (&r);
          SetSaveItem ();
          SetBrowseButton ();
        }
      } else if (dblClick) {
        ObjectRect (fetched, &r);
        InsetRect (&r, 4, 4);
        if (pt.x - r.left >= 3 * stdCharWidth - 8) {
          if (into < currentCache [curr].numLines - 1 ||
              (into == currentCache [curr].numLines - 1 &&
               curr == currentCount - 1)) {
            PullupAbstract (curr);
          }
        }
      }
    }
  }
}

static Int4 NeighborListLength (void)

{
  Int2        dbase;
  Int2        i;
  LinkSetPtr  lks;
  Int2        n;
  Int2        numNeighbors;
  Int2        rs;
  Int4Ptr     uids;
  ErrDesc     err;
  short       erract;

  numNeighbors = 0;
  lks = NULL;
  ErrSetOpts (ERR_CONTINUE, 0);
  if (mergeLinkSet != NULL) {
    threads++;
    mergeLinkSet = LinkSetFree (mergeLinkSet);
    threads--;
  }
  dbase = GetValue (view) - 1;
  if (dbase >= TYP_ML && dbase <= TYP_NT && currentCache != NULL) {
    uids = (Int4Ptr) MemNew (currentCount * sizeof (DocUid));
    if (uids != NULL) {
      i = 0;
      for (n = 0; n < currentCount; n++) {
        if (currentCache [n].checked) {
          uids [i] = currentCache [n].uid;
          i++;
        }
      }
      if (i > 0) {
        threads++;
        ErrGetOpts (&erract, NULL);
        ErrSetOpts (errOption, 0);
        rs = EntrezLinkUidList (&lks, currentDbase, dbase, i, uids, FALSE);
        ErrSetOpts (erract, 0);
        if (errOption == ERR_IGNORE)
        {
          ErrFetch(&err); /* discard any errors */
        }
        threads--;
      }
      MemFree (uids);
    }
  }
  mergeLinkSet = lks;
  if (mergeLinkSet != NULL) {
    numNeighbors = (Int2) mergeLinkSet->num;
  } else {
    numNeighbors = 0;
  }
  ErrSetOpts (ERR_ABORT, 0);
  ErrShow ();
  return numNeighbors;
}

static void SetPrevAndNextButtons (void)

{
  if (present > 1) {
    Enable (prev);
  } else {
    Disable (prev);
  }
  if (present < generations) {
    Enable (next);
  } else {
    Disable (next);
  }
}

static void SetSaveItem (void)

{
  Int2  count;
  Int2  i;

  saveMode = FALSE;
#ifdef WIN_MAC
  Disable (closeItem);
#endif
  if (currentCache != NULL) {
    count = 0;
    for (i = 0; i < currentCount; i++) {
      if (currentCache [i].checked) {
        count++;
      }
    }
    if (count > 0) {
      Enable (openItem);
      Enable (saveItem);
      Enable (saveAsItem);
      Disable (printItem);
      saveMode = TRUE;
    } else {
      Disable (openItem);
      Disable (saveItem);
      Disable (saveAsItem);
      Disable (printItem);
      saveMode = FALSE;
    }
  }
}

static void SetBrowseButton (void)

{
  Int2  i;
  Int4  nbrs;
  Char  str [32];
  Char  title [32];

  WatchCursor ();
  if (GetValue (view) - 1 != currentDbase) {
    for (i = 0; i < QUEUE_SIZE; i++) {
      queuedItems [i] = 0;
      getThese [i] = 0;
      whichOne [i] = 0;
    }
    queuedCount = 0;
  }
  nbrs = NeighborListLength ();
  numNbrs = nbrs;
  if (nbrs > 0) {
    Enable (browse);
  } else {
    Disable (browse);
  }
  LongToStr (nbrs, str, 1, sizeof (str));
  if (GetValue (view) - 1 != currentDbase) {
    StringNCpy (title, "Lookup ", sizeof (title));
  } else {
    StringNCpy (title, "Neighbor ", sizeof (title));
  }
  StrngCat (title, str, sizeof (title));
  SetTitle (browse, title);
  ArrowCursor ();
}

static void LoadPresentGeneration (void)

{
  Int2       db;
  FILE       *f;
  Int2       num;
  Int2       parents;
  DocUidPtr  uids;

  if (historyFile [0] != '\0' && present > 0) {
    WatchCursor ();
    f = FileOpen (historyFile, "rb");
    if (f != NULL) {
      fseek (f, historyOffsets [present % 100], SEEK_SET);
      FileRead (&db, sizeof (db), 1, f);
      FileRead (&num, sizeof (num), 1, f);
      FileRead (&parents, sizeof (parents), 1, f);
      uids = (DocUidPtr) MemNew ((size_t) num * sizeof (DocUid) + 4);
      if (uids != NULL) {
        FileRead (uids, sizeof (DocUid), (size_t) num, f);
      }
      FileClose (f);
      if (uids != NULL) {
        RetrieveGeneration (num, parents, uids, NULL, db);
        uids = (DocUidPtr) MemFree (uids);
      }
    }
    ArrowCursor ();
  }
}

static void PrevProc (ButtoN b)

{
  if (threads == 0) {
    present--;
    if (present < 1) {
      present = 1;
    }
    SetPrevAndNextButtons ();
    LoadPresentGeneration ();
  }
}

static void NextProc (ButtoN b)

{
  if (threads == 0) {
    present++;
    if (present > generations) {
      present = generations;
    }
    SetPrevAndNextButtons ();
    LoadPresentGeneration ();
  }
}

static void SelectParentsProc (ButtoN b)

{
  Boolean  changed;
  Int2     i;
  RecT     r;

  if (threads == 0 && currentCache != NULL) {
    changed = FALSE;
    for (i = 0; i < currentParents; i++) {
      if (! currentCache [i].checked) {
        changed = TRUE;
      }
      currentCache [i].checked = TRUE;
    }
    for (i = currentParents; i < currentCount; i++) {
      if (currentCache [i].checked) {
        changed = TRUE;
      }
      currentCache [i].checked = FALSE;
    }
    if (changed) {
      Select (fetched);
      ObjectRect (fetched, &r);
      InsetRect (&r, 4, 4);
      r.bottom = r.top + visLines * fetchedHeight;
      r.right = r.left + MIN (fetchedHeight, 12);
      InsetRect (&r, -1, -1);
      InvalRect (&r);
      SetSaveItem ();
      SetBrowseButton ();
    }
  }
}

static void SelectAllProc (ButtoN b)

{
  Boolean  changed;
  Int2     i;
  RecT     r;

  if (threads == 0 && currentCache != NULL) {
    changed = FALSE;
    for (i = 0; i < currentCount; i++) {
      if (! currentCache [i].checked) {
        changed = TRUE;
      }
      currentCache [i].checked = TRUE;
    }
    if (changed) {
      Select (fetched);
      ObjectRect (fetched, &r);
      InsetRect (&r, 4, 4);
      r.bottom = r.top + visLines * fetchedHeight;
      r.right = r.left + MIN (fetchedHeight, 12);
      InsetRect (&r, -1, -1);
      InvalRect (&r);
      SetSaveItem ();
      SetBrowseButton ();
    }
  }
}

static void ClearAllProc (ButtoN b)

{
  Boolean  changed;
  Int2     i;
  RecT     r;

  if (threads == 0 && currentCache != NULL) {
    changed = FALSE;
    for (i = 0; i < currentCount; i++) {
      if (currentCache [i].checked) {
        changed = TRUE;
      }
      currentCache [i].checked = FALSE;
    }
    if (changed) {
      Select (fetched);
      ObjectRect (fetched, &r);
      InsetRect (&r, 4, 4);
      r.bottom = r.top + visLines * fetchedHeight;
      r.right = r.left + MIN (fetchedHeight, 12);
      InsetRect (&r, -1, -1);
      InvalRect (&r);
      SetSaveItem ();
      SetBrowseButton ();
    }
  }
}

static Boolean UidInList (DocUid uid, DocUidPtr sorted, Int2 num)

{
  Boolean  found;
  Int2     left;
  Int2     mid;
  Int2     right;

  found = FALSE;
  if (sorted != NULL && num > 0) {
    mid = 0;
    left = 0;
    right = num - 1;
    while (left <= right) {
      mid = (left + right) / 2;
      if (uid <= sorted [mid]) {
        right = mid - 1;
      }
      if (uid >= sorted [mid]) {
        left = mid + 1;
      }
    }
    if (left > right + 1) {
      found = TRUE;
    }
  }
  return found;
}

#define saveMsg1 "The destination database is not currently available.\nDo you wish to save the list of UIDs?"
#define saveMsg2 "When you are ready, quit Entrez, swap discs,\nrestart Entrez, and load the saved UID list."

static void SaveLinkList (void)

{
  Int2       db;
  Char       dfault [32];
  FILE       *f;
  Int2       j;
  Int2       num;
  Char       path [256];
  Char       str [32];
  DocUidPtr  uids;

  if (numNbrs != 0) {
    if (Message (MSG_OKC, saveMsg1) == ANS_OK) {
      sprintf (dfault, "Lnk%d.uid", (int) present);
      if (GetOutputFileName (path, sizeof (path), dfault)) {
#ifdef WIN_MAC
        FileCreate (path, "TEXT", "ttxt");
#endif
        f = FileOpen (path, "w");
        if (f != NULL) {
          db = GetValue (view) - 1;
          sprintf (str, "%d\n", (int) db);
          FilePuts (str, f);
          j = 0;
          num = (Int2) numNbrs;
          uids = (DocUidPtr) mergeLinkSet->uids;
          while (j < num) {
            sprintf (str, "%ld\n", (long) (uids [j]));
            FilePuts (str, f);
            j++;
          }
          FileClose (f);
          Message (MSG_OK, saveMsg2);
        }
      }
    }
  }
}

static void BrowseProc (ButtoN b)

{
  Int2       dbase;
  Int2       i;
  Int2       j;
  Int2       k;
  Int2       len;
  DocUidPtr  lsetuids;
  Int4Ptr    lsetwgts;
  Int2       num;
  Int2       parents;
  DocUidPtr  sorted;
  DocUidPtr  uids;
  DocUid     tmp;
  Int4Ptr    weights;

  if (threads == 0) {
    WatchCursor ();
    dbase = GetValue (view) - 1;
    if (dbase == currentDbase) {
      if (parentsPersist && currentCache != NULL) {
        parents = 0;
        for (j = 0; j < currentCount; j++) {
          if (currentCache [j].checked) {
            parents++;
          }
        }
        if (parents > 0) {
          num = (Int2) mergeLinkSet->num;
          len = num + parents;
          uids = (DocUidPtr) MemNew (len * sizeof (Int4));
          sorted = (DocUidPtr) MemNew (parents * sizeof (Int4));
          if (uids != NULL && sorted != NULL) {
            for (j = 0, k = 0; j < currentCount; j++) {
              if (currentCache [j].checked) {
                uids [k] = currentCache [j].uid;
                k++;
              }
            }
            for (j = 0; j < parents; j++) {
              sorted [j] = uids [j];
            }
            for (i = 1; i < parents; i++) {
              for (j = 0; j < i; j++) {
                if (sorted [j] > sorted [i]) {
                  tmp = sorted [i];
                  sorted [i] = sorted [j];
                  sorted [j] = tmp;
                }
              }
            }
          }
          lsetuids = mergeLinkSet->uids;
          if (uids != NULL && lsetuids != NULL) {
            for (j = 0; j < num; j++, k++) {
              uids [k] = lsetuids [j];
            }
          }
          weights = (Int4Ptr) MemNew (len * sizeof (Int4));
          lsetwgts = mergeLinkSet->weights;
          if (weights != NULL && lsetwgts != NULL) {
            for (j = 0; j < parents; j++) {
              weights [j] = 0;
            }
            for (k = 0; k < num; k++, j++) {
              weights [j] = lsetwgts [k];
            }
          }
          for (j = parents, k = parents; j < len; j++) {
            if (! UidInList (uids [j], sorted, parents)) {
              uids [k] = uids [j];
              weights [k] = weights [j];
              k++;
            }
          }
          len = k;
          RetrieveTheDocuments (len, parents, uids, weights, dbase);
          if (uids != NULL) {
            uids = (DocUidPtr) MemFree (uids);
          }
          if (sorted != NULL) {
            sorted = (DocUidPtr) MemFree (sorted);
          }
          if (weights != NULL) {
            weights = (Int4Ptr) MemFree (weights);
          }
        } else {
          len = (Int2) mergeLinkSet->num;
          uids = mergeLinkSet->uids;
          weights = mergeLinkSet->weights;
          RetrieveTheDocuments (len, 0, uids, weights, dbase);
        }
      } else {
        len = (Int2) mergeLinkSet->num;
        uids = mergeLinkSet->uids;
        weights = mergeLinkSet->weights;
        RetrieveTheDocuments (len, 0, uids, weights, dbase);
      }
    } else if ((dbase == TYP_ML && hasMed) ||
               (dbase == TYP_AA && hasProt) ||
               (dbase == TYP_NT && hasNuc)) {
      len = (Int2) mergeLinkSet->num;
      uids = mergeLinkSet->uids;
      RetrieveTheDocuments (len, 0, uids, NULL, dbase);
    } else {
      SaveLinkList ();
    }
    if (mergeLinkSet != NULL) {
      threads++;
      mergeLinkSet = LinkSetFree (mergeLinkSet);
      threads--;
    }
    ArrowCursor ();
  }
}

static void SaveNameProc (IteM i)

{
  Int2       db;
  Char       dfault [32];
  FILE       *fin;
  FILE       *fout;
  Int2       j;
  Int2       num;
  Int2       parents;
  Char       path [256];
  Char       str [32];
  DocUidPtr  uids;

  if (historyFile [0] != '\0' && present > 0) {
    sprintf (dfault, "Gen%d.uid", (int) present);
    if (GetOutputFileName (path, sizeof (path), dfault)) {
#ifdef WIN_MAC
      FileCreate (path, "TEXT", "ttxt");
#endif
      fout = FileOpen (path, "w");
      if (fout != NULL) {
        fin = FileOpen (historyFile, "rb");
        if (fin != NULL) {
          fseek (fin, historyOffsets [present % 100], SEEK_SET);
          FileRead (&db, sizeof (db), 1, fin);
          FileRead (&num, sizeof (num), 1, fin);
          FileRead (&parents, sizeof (parents), 1, fin);
          uids = (DocUidPtr) MemNew ((size_t) num * sizeof (DocUid) + 4);
          if (uids != NULL) {
            FileRead (uids, sizeof (DocUid), (size_t) num, fin);
          }
          FileClose (fin);
          sprintf (str, "%d\n", (int) db);
          FilePuts (str, fout);
          j = 0;
          while (j < num) {
            sprintf (str, "%ld\n", (long) (uids [j]));
            FilePuts (str, fout);
            j++;
          }
          MemFree (uids);
        }
      }
      FileClose (fout);
    }
  }
}

extern void LoadSaveName (MenU m)

{
  saveNameItem = CommandItem (m, "Save Uid List", SaveNameProc);
  Disable (saveNameItem);
}

extern void SaveManyProc (IteM i)

{
  Int2    j;
  DocUid  uid;

  if (saveMode && threads == 0 && currentCache != NULL) {
    WatchCursor ();
    ErrSetOpts (ERR_CONTINUE, 0);
    threads++;
    for (j = 0; j < currentCount; j++) {
      if (currentCache [j].checked) {
        uid = currentCache [j].uid;
        switch (currentDbase) {
          case TYP_ML:
            SaveAbstract (NULL,  uid, TRUE);
            break;
          case TYP_AA:
            SaveSequence (NULL, uid, FALSE, TRUE);
            break;
          case TYP_NT:
            SaveSequence (NULL, uid, TRUE, TRUE);
            break;
          default:
            break;
        }
      }
    }
    threads--;
    ErrSetOpts (ERR_ABORT, 0);
    /*
    ErrClear ();
    */
    ErrShow ();
    ArrowCursor ();
  }
}

extern void SaveAsManyProc (IteM i)

{
  Char    dfault [PATH_MAX];
  Int2    j;
  Char    path [256];
  DocUid  uid;

  if (saveMode && threads == 0 && currentCache != NULL) {
    dfault [0] = '\0';
    if (currentDbase == TYP_ML) {
      GetAppParam ("ENTREZ", "SAVE", "DEFREFFILE", "entrez.ref", dfault, sizeof (dfault));
    } else {
      GetAppParam ("ENTREZ", "SAVE", "DEFSEQFILE", "entrez.seq", dfault, sizeof (dfault));
    }
    if (GetOutputFileName (path, sizeof (path), dfault)) {
      WatchCursor ();
      ErrSetOpts (ERR_CONTINUE, 0);
      threads++;
      for (j = 0; j < currentCount; j++) {
        if (currentCache [j].checked) {
          uid = currentCache [j].uid;
          switch (currentDbase) {
            case TYP_ML:
              SaveAbstract (path,  uid, FALSE);
              break;
            case TYP_AA:
              SaveSequence (path, uid, FALSE, FALSE);
              break;
            case TYP_NT:
              SaveSequence (path, uid, TRUE, FALSE);
              break;
            default:
              break;
          }
        }
      }
      threads--;
      ErrSetOpts (ERR_ABORT, 0);
      /*
      ErrClear ();
      */
      ErrShow ();
      ArrowCursor ();
    }
  }
}

static void SaveAllProc (IteM i)

{
  Int2    j;
  DocUid  uid;

  if (threads == 0 && currentCache != NULL) {
    WatchCursor ();
    ErrSetOpts (ERR_CONTINUE, 0);
    threads++;
    for (j = 0; j < currentCount; j++) {
      uid = currentCache [j].uid;
      switch (currentDbase) {
        case TYP_ML:
          SaveAbstract (NULL,  uid, TRUE);
          break;
        case TYP_AA:
          SaveSequence (NULL, uid, FALSE, TRUE);
          break;
        case TYP_NT:
          SaveSequence (NULL, uid, TRUE, TRUE);
          break;
        default:
          break;
      }
    }
    threads--;
    ErrSetOpts (ERR_ABORT, 0);
    /*
    ErrClear ();
    */
    ErrShow ();
    ArrowCursor ();
  }
}

static void OpenProc (IteM i)

{
  Int2    curr;
  Char    title [80];
  DocUid  uid;

  if (saveMode && threads == 0 && currentCache != NULL) {
    WatchCursor ();
    ErrSetOpts (ERR_CONTINUE, 0);
    threads++;
    for (curr = 0; curr < currentCount; curr++) {
      if (currentCache [curr].checked) {
        uid = currentCache [curr].uid;
        ProcessCaption (title, sizeof (title), curr);
        switch (currentDbase) {
          case TYP_ML:
            LoadAbstract (title, uid);
            break;
          case TYP_AA:
            LoadSequence (title, uid, FALSE);
            break;
          case TYP_NT:
            LoadSequence (title, uid, TRUE);
            break;
          default:
            break;
        }
      }
    }
    threads--;
    ErrSetOpts (ERR_ABORT, 0);
    /*
    ErrClear ();
    */
    ErrShow ();
    ArrowCursor ();
  }
}

#ifdef WIN_MAC
static void CloseProc (IteM i)

{
  WindoW  w;

  if (docActive) {
    w = FrontWindow ();
    Remove (w);
    ForgetDocWindow (w);
  } else {
    HideBrowseWindowProc (browseWindow);
  }
}

static void SaveProc (IteM i)

{
  if (docActive) {
    SaveOneProc (i);
  } else {
    SaveManyProc (i);
  }
}

static void SaveAsProc (IteM i)

{
  if (docActive) {
    SaveAsOneProc (i);
  } else {
    SaveAsManyProc (i);
  }
}
#endif

#ifdef WIN_MSWIN
static void SaveProc (IteM i)

{
  if (docActive) {
    SaveOneProc (i);
  } else {
    SaveManyProc (i);
  }
}

static void SaveAsProc (IteM i)

{
  if (docActive) {
    SaveAsOneProc (i);
  } else {
    SaveAsManyProc (i);
  }
}
#endif

#ifdef WIN_MOTIF
static void SaveProc (IteM i)

{
  SaveManyProc (i);
}

static void SaveAsProc (IteM i)

{
  SaveAsManyProc (i);
}
#endif

static void PrintProc (IteM i)

{
  if (docActive) {
    PrintOneProc (i);
  }
}

static void ViewProc (GrouP g)

{
  SetSaveItem ();
  SetBrowseButton ();
}

extern void ClearAllLists (void)

{
  if (mergeLinkSet != NULL) {
    threads++;
    mergeLinkSet = LinkSetFree (mergeLinkSet);
    threads--;
  }
}

extern void ClearHistoryList (void)

{
  Int2  i;

  if (historyFile [0] != '\0') {
    FileRemove (historyFile);
    historyFile [0] = '\0';
  }
  for (i = 0; i < 100; i++) {
    historyOffsets [i] = 0;
  }
  if (cacheFile [0] != '\0') {
    FileRemove (cacheFile);
    cacheFile [0] = '\0';
  }
  generations = 0;
  present = 0;
  Disable (prev);
  Disable (next);
}

static void InitWinContext (void)

{
  static Boolean inited = FALSE;
  Char str[10];

  if (inited) {
    return;
  }

  numContexts = 20;
  GetAppParam ("ENTREZ", "PREFERENCES", "WINDOWS", "20", str, sizeof (str));
  StrToInt (str, &numContexts);
  if ((winContext = MemNew(sizeof(WinContext) * numContexts)) == NULL) {
    winContext = MemNew(sizeof(WinContext) * 20);
    numContexts = 20;
  }
  inited = TRUE;
}

extern void ClearDocumentLists (void)

{
  Int2  i;
  WinContextPtr wcp;

  InitWinContext ();

  for (wcp = winContext, i = 0; i < numContexts; i++, wcp++) {
    wcp->win = NULL;
    wcp->doc = NULL;
    wcp->isSequence = FALSE;
    wcp->uid = 0;
    wcp->view = 0;
  }
}

extern void FreeDocWindows (void)

{
  Int2    i;
  WindoW  w;

  InitWinContext ();

  for (i = numContexts - 1; i >= 0; i--) {
    w = winContext [i].win;
    if (w != NULL) {
      Remove (w);
    }
  }
  ClearDocumentLists ();
}

extern void RememberDocWindow (WindoW w, DoC d, DocUid uid,
                               Int2 view, Boolean isSequence)

{
  Int2  i;
  WinContextPtr wcp;

  InitWinContext ();

  i = 0;
  wcp = winContext;
  while (i < numContexts && wcp->win != NULL) {
    i++;
    wcp++;
  }
  if (i < numContexts) {
    wcp->win = w;
    wcp->doc = d;
    wcp->isSequence = isSequence;
    wcp->uid = uid;
    wcp->view = view;
  }
}

extern void ForgetDocWindow (WindoW w)

{
  Int2  i;
  WinContextPtr wcp;

  InitWinContext ();

  wcp = winContext;
  i = 0;
  while (i < numContexts && wcp->win != w) {
    i++;
    wcp++;
  }
  if (i < numContexts) {
    wcp->win = NULL;
    wcp->doc = NULL;
    wcp->isSequence = FALSE;
    wcp->uid = 0;
    wcp->view = 0;
  }
}

extern DoC FindDocWindow (WindoW w)

{
  Int2  i;
  WinContextPtr wcp;

  InitWinContext ();

  wcp = winContext;
  i = 0;
  while (i < numContexts && wcp->win != w) {
    i++;
    wcp++;
  }
  if (i < numContexts) {
    return wcp->doc;
  } else {
    return NULL;
  }
}

extern WindoW FindUidWindow (DocUid uid, Int2 view)

{
  Int2  i;
  WinContextPtr wcp;

  InitWinContext ();

  wcp = winContext;
  i = 0;
  while (i < numContexts && (wcp->uid != uid || wcp->view != view)) {
    i++;
    wcp++;
  }
  if (i < numContexts) {
    return wcp->win;
  } else {
    return NULL;
  }
}

extern Boolean IsSequence (WindoW w)

{
  Int2  i;
  WinContextPtr wcp;

  InitWinContext ();

  wcp = winContext;
  i = 0;
  while (i < numContexts && wcp->win != w) {
    i++;
    wcp++;
  }
  if (i < numContexts) {
    return wcp->isSequence;
  } else {
    return FALSE;
  }
}

static void RecreateFetched (void)

{
  RecT    r;
  WindoW  tempPort;

  if (Visible (fetched) && AllParentsVisible (fetched)) {
    tempPort = SavePort (fetched);
    Select (fetched);
    ObjectRect (fetched, &r);
    InsetRect (&r, 4, 4);
    InvalRect (&r);
    RestorePort (tempPort);
  }
}

extern void ResetFetchedList (void)

{
  currentCount = 0;
  currentParents = 0;
  lastChecked = -1;
  numCached = 0;
  firstShown = 0;
  lineInto = 0;
  lastShown = 0;
  lineBackto = 0;
  drawBackward = FALSE;
  Reset (fetched);
  RecreateFetched ();
}

static void OpenUidsProc (IteM i)

{
  ByteStorePtr  bsp;
  CharPtr       chptr;
  Int2          dbase;
  FILE          *fp;
  Int2          num;
  Char          path [256];
  Char          str [32];
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
            while (FileGets (str, sizeof (str), fp)) {
              chptr = StringRChr (str, '\n');
              if (chptr != NULL) {
                *chptr = '\0';
              }
              if (StrToLong (str, &uid)) {
                BSWrite (bsp, &uid, sizeof (DocUid));
              }
            }
            BSSeek (bsp, 0L, 0);
            num = ((Int2) BSLen (bsp)) / sizeof (DocUid);
            uids = (DocUidPtr) MemNew ((size_t) num * sizeof (DocUid));
            if (uids != NULL) {
              BSMerge (bsp, uids);
              RetrieveTheDocuments (num, 0, uids, NULL, dbase);
              MemFree (uids);
            }
          }
        }
        BSFree (bsp);
      }
      FileClose (fp);
    }
  }
}

extern void LoadOpenUids (MenU m)

{
  openUidsItem = CommandItem (m, "Open Uids", OpenUidsProc);
}

static void ChangeParentsPersist (IteM i)

{
  if (GetStatus (i)) {
    parentsPersist = TRUE;
    SetAppParam ("ENTREZ", "PREFERENCES", "PARENTSPERSIST", "TRUE");
  } else {
    parentsPersist = FALSE;
    SetAppParam ("ENTREZ", "PREFERENCES", "PARENTSPERSIST", "FALSE");
  }
}

extern void LoadParentsPersistPrefs (MenU m)

{
  Char  str [32];

  persistItem = StatusItem (m, "Parents Persist", ChangeParentsPersist);
  parentsPersist = FALSE;
  if (GetAppParam ("ENTREZ", "PREFERENCES", "PARENTSPERSIST", "", str, sizeof (str) - 1)) {
    if (StringICmp (str, "TRUE") == 0) {
      parentsPersist = TRUE;
      SetStatus (persistItem, TRUE);
    }
  }
}

static void ChangeWeight (IteM i)

{
  if (GetStatus (i)) {
    showWeights = TRUE;
    SetAppParam ("ENTREZ", "PREFERENCES", "SHOWSCORES", "TRUE");
  } else {
    showWeights = FALSE;
    SetAppParam ("ENTREZ", "PREFERENCES", "SHOWSCORES", "FALSE");
  }
}

extern void LoadScorePrefs (MenU m)

{
  Char  str [32];

  /*
  weightItem = StatusItem (m, "Show Scores", ChangeWeight);
  */
  showWeights = FALSE;
  thereAreWeights = FALSE;
  if (GetAppParam ("ENTREZ", "PREFERENCES", "SHOWSCORES", "", str, sizeof (str) - 1)) {
    if (StringICmp (str, "TRUE") == 0) {
      showWeights = TRUE;
      /*
      SetStatus (weightItem, TRUE);
      */
    }
  } else {
    SetAppParam ("ENTREZ", "PREFERENCES", "SHOWSCORES", "FALSE");
  }
}

extern void LoadSaveItem (MenU m)

{
#ifdef WIN_MAC
  closeItem = CommandItem (m, "Close/W", CloseProc);
  Disable (closeItem);
#endif
  openItem = CommandItem (m, "Open/O", OpenProc);
  Disable (openItem);
  saveItem = CommandItem (m, "Save/S", SaveProc);
  Disable (saveItem);
  saveAsItem = CommandItem (m, "Save As...", SaveAsProc);
  Disable (saveAsItem);
  saveAllItem = CommandItem (m, "Save All", SaveAllProc);
  Disable (saveAllItem);
}

extern void LoadPrintItem (MenU m)

{
  printItem = CommandItem (m, "Print/P", PrintProc);
  Disable (printItem);
}

static void ActivateBrowseProc (WindoW w)

{
  docActive = FALSE;
  SetSaveItem ();
#ifdef WIN_MAC
  Enable (closeItem);
#endif
}

static Uint1  vertLine [] = {
  0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00
};

static Uint1  neswLine [] = {
  0x00, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x00
};

static Uint1  horzLine [] = {
  0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00
};

static Uint1  senwLine [] = {
  0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00
};

static Uint1Ptr lines [] = {
  vertLine, neswLine, horzLine, senwLine
};

static void DrawProg (IcoN i)

{
  RecT  r;
  Int2  val;

  ObjectRect (i, &r);
  r.left = (r.right + r.left) / 2 - 4;
  r.right = r.left + 8;
  r.top = (r.bottom + r.top) / 2 - 4;
  r.bottom = r.top + 8;
  val = (GetValue (progress)) % 4;
  CopyBits (&r, lines [val]);
}

extern void CreateFetchedList (WindoW w)

{
  ButtoN             b;
  Boolean            canSwap;
  EntrezInfoPtr      eip;
  GrouP              g;
  GrouP              h;
  Int2               i;
  Int2               numDbs;
  BaR                sb;
  Char               str [16];
  EntrezTypeDataPtr  types;
#ifdef WIN_MSWIN
  MenU               m;
#endif
#ifdef WIN_MOTIF
  MenU               m;
#endif

#ifdef WIN_MAC
  fetchedFont = ChooseFont ("FETCHED", "Geneva,10");
#endif
#ifdef WIN_MSWIN
  fetchedFont = ChooseFont ("FETCHED", "Times New Roman,12");
#endif
#ifdef WIN_MOTIF
  fetchedFont = ChooseFont ("FETCHED", "Times,14");
#endif
  hasMed = TRUE;
  hasProt = TRUE;
  hasNuc = TRUE;
  canSwap = TRUE;
  if (GetAppParam ("ENTREZ", "PREFERENCES", "SIMULATESWAP", "", str, sizeof (str) - 1)) {
    if (StringICmp (str, "TRUE") == 0) {
      canSwap = FALSE;
    }
  }
  generations = 0;
  present = 0;
  currentDbase = TYP_ML;
  currentCount = 0;
  currentParents = 0;
  currentCache = NULL;
  for (i = 0; i < 50; i++) {
    cachedStrings [i] = NULL;
    cachedUids [i] = 0;
  }
  for (i = 0; i < QUEUE_SIZE; i++) {
    queuedItems [i] = 0;
  }
  queuedCount = 0;
  queueTicks = 0;
  lastChecked = -1;
  numCached = 0;
  firstShown = 0;
  lineInto = 0;
  lastShown = 0;
  lineBackto = 0;
  drawBackward = FALSE;
  totalNumLines = 0;
  SelectFont (fetchedFont);
  fetchedHeight = LineHeight ();
  SelectFont (systemFont);
  linesPerIcon = (22 + fetchedHeight - 1) / fetchedHeight;
  mergeLinkSet = NULL;
  numNbrs = 0;
  historyFile [0] = '\0';
  for (i = 0; i < 100; i++) {
    historyOffsets [i] = 0;
  }
  cacheFile [0] = '\0';
  hasicondraw = NULL;
  noicondraw = NULL;
  if (smallScreen) {
    visLines = 10 * stdLineHeight / fetchedHeight;
    fetched = AutonomousPanel (w, 27 * stdCharWidth, 10 * stdLineHeight,
                               DrawFetched, ScrollAction, NULL, 0, NULL, NULL);
  } else {
    visLines = 20 * stdLineHeight / fetchedHeight;
    fetched = AutonomousPanel (w, 32 * stdCharWidth, 20 * stdLineHeight,
                               DrawFetched, ScrollAction, NULL, 0, NULL, NULL);
  }
  SetPanelClick (fetched, FetchedClick, NULL, NULL, NULL);
  sb = GetSlateVScrollBar ((SlatE) fetched);
  Nlm_SetBarAnomaly (sb, TRUE);
  Break (w);
  h = HiddenGroup (w, 0, 2, NULL);
  SetGroupMargins (h, 3, 0);
  SetGroupSpacing (h, 3, 5);
  browse = PushButton (h, "Neighbor 00000", BrowseProc);
  SetTitle (browse, "Neighbor 0");
  Disable (browse);
  progress = IconButton (h, 12, 12, DrawProg, NULL, NULL, NULL, NULL, NULL);
  Hide (progress);
  Advance (w);
  h = HiddenGroup (w, 0, 3, NULL);
  SetGroupMargins (h, 3, 0);
  SetGroupSpacing (h, 3, 5);
  eip = EntrezGetInfo ();
  view = HiddenGroup (h, 3, 0, ViewProc);
  SetGroupSpacing (view, 3, 2);
  if (eip != NULL && eip->types != NULL) {
    types = eip->types;
    numDbs = 0;
    for (i = 0; i < eip->type_count; i++) {
      if (types [i].num > 0) {
        numDbs++;
      }
    }
    b = RadioButton (view, "MEDLINE");
    if (eip->type_count < 1 || types [0].num <= 0 || numDbs <= 1) {
      hasMed = FALSE;
      if (canSwap) {
        Hide (b);
      }
    }
    b = RadioButton (view, "Protein");
    if (eip->type_count < 2 || types [1].num <= 0 || numDbs <= 1) {
      hasProt = FALSE;
      if (canSwap) {
        Hide (b);
      }
    }
    b = RadioButton (view, "Nucleotide");
    if (eip->type_count < 3 || types [2].num <= 0 || numDbs <= 1) {
      hasNuc = FALSE;
      if (canSwap) {
        Hide (b);
      }
    }
  }
  SetValue (view, 1);
  g = HiddenGroup (h, 4, 0, NULL);
  SetGroupSpacing (g, 8, 0);
  StaticPrompt  (g, "Select:", 0, stdLineHeight, systemFont, 'l');
  PushButton (g, "All", SelectAllProc);
  PushButton (g, "None", ClearAllProc);
  PushButton (g, "Parents", SelectParentsProc);
  if (! smallScreen) {
    Advance (w);
    h = HiddenGroup (w, 0, 2, NULL);
    prev = PushButton (h, "Prev", PrevProc);
    Disable (prev);
    next = PushButton (h, "Next", NextProc);
    Disable (next);
  }
#ifdef WIN_MSWIN
  if (smallScreen) {
    m = PulldownMenu (w, "Windows");
    LoadGenerationItems (m);
  }
#endif
#ifdef WIN_MOTIF
  if (smallScreen) {
    m = PulldownMenu (w, "Windows");
    LoadGenerationItems (m);
  }
#endif
  SetActivate (w, ActivateBrowseProc);
  errOption = ERR_CONTINUE;
  if (GetAppParam ("ENTREZ", "PREFERENCES", "SHOWALLERRORS", "", str, sizeof (str) - 1)) {
    if (StringICmp (str, "FALSE") == 0) {
      errOption = ERR_IGNORE;
    }
  }
}

extern void LoadGenerationItems (MenU m)

{
  prev = (ButtoN) CommandItem (m, "Prev", (ItmActnProc) PrevProc);
  Disable (prev);
  next = (ButtoN) CommandItem (m, "Next", (ItmActnProc) NextProc);
  Disable (next);
}
