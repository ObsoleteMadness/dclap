/*   report.c
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
* File Name:  report.c
*
* Author:  Kans, Schuler
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.42 $
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
#include <tofasta.h>
#include <togenbnk.h>
#include <toreport.h>
#include <tomedlin.h>
#include <prtutil.h>
#include "browser.h"

#define BUFSIZE 8192

static CharPtr  buffer;
static CharPtr  pos;

IteM     openAsnItem;
Boolean  docActive;

ChoicE   articleChoice;
ChoicE   reportChoice;
ChoicE   reportClass;

static IteM saveAsnItem;

static StdPrintOptionsPtr  spop = NULL;

static IteM     featChoice [16];
static IteM     showSeqItem;
static Boolean  showReportForm = TRUE;
static Boolean  showGenbankForm = FALSE;
static Boolean  showEmblForm = FALSE;
static Boolean  showFastaForm = FALSE;
static Boolean  showExtractForm = FALSE;
static Boolean  showSeqAsnForm = FALSE;
static Boolean  showNewReportForm = FALSE;
static Int2     seqView = VIEW_REPORT;
static Int2     retcode = 3;
static Boolean  showSeq = TRUE;
static ChoicE   residuesChoice;
static Int2     charsPerLine;

static Boolean  showArticleForm = TRUE;
static Boolean  showBiblioForm = FALSE;
static Boolean  showMedlineAsnForm = FALSE;
static Int2     medView = VIEW_ARTICLE;
static Int2     abstractNum;

static FonT     headingFont;
static FonT     referenceFont;
static FonT     sequenceFont;
static FonT     standardFont;

static FonT     jourfnt;
static FonT     volfnt;
static FonT     pagesfnt;
static FonT     titlefnt;
static FonT     authorsfnt;
static FonT     affilfnt;
static FonT     abstractfnt;
static FonT     meshfnt;
static FonT     displayFont;

static Int2     docCount;
static Boolean  firstTime;

static Int2     REPORTFORM;
static Int2     GNBKFORM;
static Int2     EMBLFORM;
static Int2     FASTFORM;
static Int2     FEATFORM;
static Int2     SEQASNFORM;
static Int2     NEWFORM;

static Boolean  launchApp = FALSE;

static ColData noWrapDisplay = {0, 0, 80, 0, NULL, 'l', FALSE, FALSE, FALSE, FALSE, TRUE};

static ParData seqParFmt = {FALSE, FALSE, FALSE, FALSE, FALSE, 0, 0};
static ColData seqColFmt = {0, 0, 0, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE};

static void CloseDocWindowProc (WindoW w)

{
#ifdef WIN_MAC
  if (optKey) {
    FreeDocWindows ();
  } else {
    Remove (w);
    ForgetDocWindow (w);
  }
#else
  Remove (w);
  ForgetDocWindow (w);
#endif
}

static void CloseDocItemProc (IteM i)

{
  WindoW  w;

  w = ParentWindow (i);
  Remove (w);
  ForgetDocWindow (w);
}

static void CloseDocButtonProc (ButtoN b)

{
  WindoW  w;

  w = ParentWindow (b);
  Remove (w);
  ForgetDocWindow (w);
}

static void ActivateDocWindowProc (WindoW w)

{
#ifdef WIN_MAC
  Disable (openItem);
  Enable (closeItem);
  Enable (saveItem);
  Enable (saveAsItem);
  Enable (printItem);
#endif
  docActive = TRUE;
}

static Boolean LIBCALLBACK EntrezProgMonFunc (VoidPtr data, CharPtr str)

{
  Int2    val;

  val = GetValue (progress);
  val++;
  SetValue (progress, val);
  return TRUE;
}

static void StartProgressIndicator ()

{
  SetProgMon (EntrezProgMonFunc, NULL);
  SetValue (progress, 0);
  Show (progress);
}

static void StopProgressIndicator ()

{
  SetProgMon (NULL, NULL);
  Hide (progress);
  SetValue (progress, 0);
}

static FILE *OpenAFile (CharPtr dfault, Boolean isSequence, Boolean stdLoc, Boolean replace)

{
  FILE  *fp;
  Char  path [PATH_MAX];
  Char  str [PATH_MAX];
#ifdef WIN_MAC
  FILE  *f;
#endif

  path [0] = '\0';
  if (stdLoc) {
    if (GetAppParam ("ENTREZ", "SAVE", "PATH", "", str, sizeof (str))) {
      FileBuildPath (path, str, NULL);
    }
  }
  if (dfault != NULL && dfault [0] != '\0') {
    StringNCpy (str, dfault, sizeof (str) - 1);
    FileBuildPath (path, NULL, str);
  } else {
    if (isSequence) {
      GetAppParam ("ENTREZ", "SAVE", "DEFSEQFILE", "entrez.seq", str, sizeof (str));
      FileBuildPath (path, NULL, str);
    } else {
      GetAppParam ("ENTREZ", "SAVE", "DEFREFFILE", "entrez.ref", str, sizeof (str));
      FileBuildPath (path, NULL, str);
    }
  }
#ifdef WIN_MAC
  f = FileOpen (path, "r");
  if (f != NULL) {
    FileClose (f);
  } else {
    FileCreate (path, "TEXT", "ttxt");
  }
#endif
  if (replace) {
    fp = FileOpen (path, "w");
  } else {
    fp = FileOpen (path, "a");
  }
  return fp;
}

extern void SaveOneProc (IteM i)

{
  Char    ch;
  DoC     d;
  Char    dfault [PATH_MAX];
  FILE    *fp;
  Int2    j;
  Int2    k;
  WindoW  w;

#ifdef WIN_MAC
  w = FrontWindow ();
#else
  w = ParentWindow (i);
#endif
  d = FindDocWindow (w);
  if (d != NULL) {
    GetTitle (w, dfault, sizeof (dfault));
    j = 0;
    k = 0;
    ch = dfault [j];
    while (j < sizeof (dfault) && ch != '\0') {
      if (ch <= ' ') {
        j++;
      } else {
        dfault [k] = dfault [j];
        k++;
        j++;
      }
      ch = dfault [j];
    }
    dfault [k] = '\0';
#ifdef WIN_MSWIN
    j = 0;
    ch = dfault [j];
    while (j < sizeof (dfault) && ch != '\0') {
      if (ch == '_' || IS_ALPHANUM (ch)) {
        j++;
        ch = dfault [j];
      } else {
        ch = '\0';
      }
    }
    dfault [j] = '\0';
#endif
    fp = OpenAFile (dfault, IsSequence (w), TRUE, FALSE);
    if (fp != NULL) {
      SaveDocument (d, fp);
      FilePuts ("\n\n\n\n", fp);
    } else {
      Message (MSG_OK, "Unable to save file %s", dfault);
    }
    FileClose (fp);
  }
}

extern void SaveAsOneProc (IteM i)

{
  Char    ch;
  DoC     d;
  Char    dfault [32];
  FILE    *fp;
  Int2    j;
  Int2    k;
  Char    path [256];
  WindoW  w;

#ifdef WIN_MAC
  w = FrontWindow ();
#else
  w = ParentWindow (i);
#endif
  d = FindDocWindow (w);
  if (d != NULL) {
    dfault [0] = '\0';
    GetTitle (w, dfault, sizeof (dfault));
    j = 0;
    k = 0;
    ch = dfault [j];
    while (j < sizeof (dfault) && ch != '\0') {
      if (ch <= ' ') {
        j++;
      } else {
        dfault [k] = dfault [j];
        k++;
        j++;
      }
      ch = dfault [j];
    }
    dfault [k] = '\0';
#ifdef WIN_MSWIN
    j = 0;
    ch = dfault [j];
    while (j < sizeof (dfault) && ch != '\0') {
      if (ch == '_' || IS_ALPHANUM (ch)) {
        j++;
        ch = dfault [j];
      } else {
        ch = '\0';
      }
    }
    dfault [j] = '\0';
#endif
    if (GetOutputFileName (path, sizeof (path), dfault)) {
      fp = OpenAFile (path, IsSequence (w), FALSE, TRUE);
      if (fp != NULL) {
        SaveDocument (d, fp);
        FilePuts ("\n\n\n\n", fp);
      } else {
        Message (MSG_OK, "Unable to save file %s", path);
      }
      FileClose (fp);
    }
  }
}

extern void PrintOneProc (IteM i)

{
  DoC     d;
  WindoW  w;
#ifdef OS_UNIX
  Char    cmmd [256];
  FILE    *fp;
  Int2    len;
  Char    str [PATH_MAX];
#endif

#ifdef WIN_MAC
  w = FrontWindow ();
#else
  w = ParentWindow (i);
#endif
  d = FindDocWindow (w);
  if (d != NULL) {
#ifdef OS_UNIX
    TmpNam (str);
    fp = FileOpen (str, "w");
    if (fp != NULL) {
      SaveDocument (d, fp);
    }
    FileClose (fp);
    if (! GetAppParam ("ENTREZ", "PRINT", "COMMAND", "lp", cmmd, sizeof (cmmd))) {
      SetAppParam ("ENTREZ", "PRINT", "COMMAND", "lp");
      StringCpy (cmmd, "lp");
    }
    len = (Int2) StringLen (cmmd);
    while (len > 0 && cmmd [len] == ' ') {
      cmmd [len] = '\0';
      len--;
    }
    StringCat (cmmd, " ");
    StringCat (cmmd, str);
    StringCat (cmmd, "; rm ");
    StringCat (cmmd, str);
    system (cmmd);
    FileRemove (str);
#else
    PrintDocument (d);
#endif
  }
}

static DoC CreateDocument (CharPtr title, Int2 width)

{
  DoC     d;
  MenU    m;
  WindoW  w;
#ifdef WIN_MOTIF
  ButtoN  b;
  Int2    delta;
  RecT    r;
  RecT    s;
#endif

  docCount = (docCount + 1) % 20;
  w = FixedWindow (-60 - 2 * docCount, -10 - 2 * docCount, -10, -10, title, CloseDocWindowProc);
  SetActivate (w, ActivateDocWindowProc);
#ifdef WIN_MSWIN
  m = PulldownMenu (w, "File");
   CommandItem (m, "Close/W", CloseDocItemProc);
  SeparatorItem (m);
  CommandItem (m, "Save/S", SaveOneProc);
  CommandItem (m, "Save As...", SaveAsOneProc);
  SeparatorItem (m);
  CommandItem (m, "Print", PrintOneProc);
#endif
#ifdef WIN_MOTIF
  m = PulldownMenu (w, "File");
  CommandItem (m, "Close/W", CloseDocItemProc);
  SeparatorItem (m);
  CommandItem (m, "Save/S", SaveOneProc);
  CommandItem (m, "Save As...", SaveAsOneProc);
  SeparatorItem (m);
  CommandItem (m, "Print", PrintOneProc);
#endif
  if (smallScreen) {
    d = DocumentPanel (w, 27 * stdCharWidth, 10 * stdLineHeight);
  } else {
    d = DocumentPanel (w, width * stdCharWidth, 20 * stdLineHeight);
  }
#ifdef WIN_MOTIF
  b = PushButton (w, "Dismiss", CloseDocButtonProc);
  GetPosition (d, &s);
  GetPosition (b, &r);
  delta = (s.right - r.right) / 2;
  if (delta > 0) {
    OffsetRect (&r, delta, 0);
    SetPosition (b, &r);
  }
#endif
  return d;
}

extern FonT ChooseFont (CharPtr param, CharPtr dfault)

{
  FonT  f;
  Char  str [128];

  f = NULL;
  if (GetAppParam ("ENTREZ", "FONTS", param, "", str, sizeof (str))) {
    f = ParseFont (str);
  } else {
    SetAppParam ("ENTREZ", "FONTS", param, dfault);
    f = ParseFont (dfault);
  }
  return f;
}

static void ClearString (void)

{
  pos = buffer;
  *pos = '\0';
}

static void AddString (CharPtr string)

{
  pos = StringMove (pos, string);
  *pos = '\0';
}

static ColData  colFmt [3] = {
  {0, 0, 0, 0, NULL, 'l', FALSE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 0, 0, NULL, 'l', FALSE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 0, 0, NULL, 'l', FALSE, FALSE, FALSE, FALSE, TRUE}
};

static ColData  mshFmt [1] = {
  {0, 0, 0, 0, NULL, 'l', FALSE, FALSE, FALSE, FALSE, TRUE}
};

static Boolean DisplayArticle (DoC d, MedlineEntryPtr mep)

{
  MedlinePtr  mPtr;
  ParData     para;
  RecT        r;

  mPtr = ParseMedline (mep);
  if (mPtr != NULL) {
    buffer = (CharPtr) MemNew (BUFSIZE);
    if (buffer != NULL) {
      para.openSpace = FALSE;
      para.keepWithNext = FALSE;
      para.keepTogether = FALSE;
      para.newPage = FALSE;
      para.tabStops = FALSE;
      para.minLines = 0;
      para.minHeight = 0;
      colFmt [0].font = jourfnt;
      colFmt [1].font = volfnt;
      colFmt [2].font = pagesfnt;
      ObjectRect (d, &r);
      InsetRect (&r, 4, 4);
      mshFmt [0].pixWidth = r.right - r.left;
      mshFmt [0].charWidth = 80;
      ClearString ();
      AddString (mPtr->journal);
      AddString ("\t");
      AddString (mPtr->volume);
      AddString (":\t");
      AddString (mPtr->pages);
      AddString ("  (");
      AddString (mPtr->year);
      AddString (")");
      AddString (" [");
      AddString (mPtr->uid);
      AddString ("]");
      AddString ("\n");
      AppendText (d, buffer, &para, colFmt, NULL);
      ClearString ();
      if (mPtr->title != NULL) {
        AddString (mPtr->title);
        AddString ("\n");
        AppendText (d, buffer, NULL, NULL, titlefnt);
        ClearString ();
      }
      if (mPtr->transl != NULL) {
        AddString ("[");
        AddString (mPtr->transl);
        AddString ("]\n");
        AppendText (d, buffer, NULL, NULL, titlefnt);
        ClearString ();
      }
      if (mPtr->title == NULL && mPtr->transl == NULL) {
      }
      AddString (mPtr->authors);
      AddString ("\n");
      AppendText (d, buffer, NULL, NULL, authorsfnt);
      ClearString ();
      if (mPtr->affil != NULL) {
        AddString (mPtr->affil);
        AddString ("\n");
        AppendText (d, buffer, NULL, NULL, affilfnt);
        ClearString ();
      }
      if (mPtr->abstract != NULL) {
        AddString (mPtr->abstract);
        AddString ("\n");
        AppendText (d, buffer, NULL, NULL, abstractfnt);
        ClearString ();
      }
      if (mPtr->mesh != NULL) {
        AppendText (d, "MeSH Terms:\n", NULL, NULL, meshfnt);
        AppendText (d, mPtr->mesh, &para, mshFmt, meshfnt);
      }
      if (mPtr->gene != NULL) {
        AppendText (d, "Gene Symbols:\n", NULL, NULL, meshfnt);
        AppendText (d, mPtr->gene, &para, mshFmt, meshfnt);
      }
      if (mPtr->substance != NULL) {
        AppendText (d, "Substances:\n", NULL, NULL, meshfnt);
        AppendText (d, mPtr->substance, &para, mshFmt, meshfnt);
      }
      buffer = (CharPtr) MemFree (buffer);
    }
    mPtr = FreeMedline (mPtr);
  }
  return TRUE;
}

static Boolean DisplayBiblio (DoC d, MedlineEntryPtr mep)

{
  FILE     *fp;
  Char     num [8];
  Boolean  okay;
  RecT     r;
  Char     str [256];

  TmpNam (str);
  fp = FileOpen (str, "w");

  okay = FALSE;
  if (mep != NULL) {
    abstractNum++;
    sprintf (num, "%1d", abstractNum);
    FilePuts (num, fp);
    FilePuts ("\n", fp);
    okay = MedlineEntryToDataFile (mep, fp);
  }
  FileClose (fp);
  if (okay) {
    ObjectRect (d, &r);
    InsetRect (&r, 4, 4);
    noWrapDisplay.pixWidth = r.right - r.left;
    DisplayFancy (d, str, NULL, &noWrapDisplay, displayFont, 4);
  }
  FileRemove (str);

  return okay;
}

static Boolean DisplayMedlineAsn (DoC d, MedlineEntryPtr mep)

{
  AsnIoPtr  aipout;
  FILE      *fp;
  Boolean   okay;
  RecT      r;
  Char      str [256];

  TmpNam (str);
  fp = FileOpen (str, "w");
  aipout = AsnIoNew (ASNIO_TEXT_OUT, fp, NULL, NULL, NULL);

  okay = FALSE;
  if (mep != NULL) {
    okay = MedlineEntryAsnWrite (mep, aipout, NULL);
  }
  AsnIoClose (aipout);
  if (okay) {
    ObjectRect (d, &r);
    InsetRect (&r, 4, 4);
    noWrapDisplay.pixWidth = r.right - r.left;
    DisplayFancy (d, str, NULL, &noWrapDisplay, displayFont, 4);
  }
  FileRemove (str);

  return okay;
}

static Boolean ArticleDisplay (DoC d, DocUid uid)

{
  AsnIoPtr         aip;
  AsnModulePtr     amp;
  AsnTypePtr       atp;
  MedlineEntryPtr  mep;
  Char             path [PATH_MAX];
  Boolean          rsult;

  rsult = FALSE;
  mep = NULL;
  StartProgressIndicator ();
  if (uid != 0) {
    mep = EntrezMedlineEntryGet (uid);
  } else {
    if (GetInputFileName (path, sizeof (path), "", "")) {
      aip = AsnIoOpen (path, "r");
      if (aip != NULL) {
        atp = AsnFind ("Medline-entry");
        amp = AsnAllModPtr ();
        atp = AsnReadId (aip, amp, atp);
        mep = MedlineEntryAsnRead (aip, atp);
        AsnIoClose (aip);
      }
    }
  }
  if (mep != NULL) {
    if (showArticleForm) {
      rsult = DisplayArticle (d, mep);
    } else if (showBiblioForm) {
      rsult = DisplayBiblio (d, mep);
    } else if (showMedlineAsnForm) {
      rsult = DisplayMedlineAsn (d, mep);
    }
    MedlineEntryFree (mep);
  }
  StopProgressIndicator ();
  return rsult;
}

extern void LoadAbstract (CharPtr title, DocUid uid)

{
  DoC     d;
  WindoW  w;

  w = FindUidWindow (uid, medView);
  if (w != NULL) {
    Show (w);
    Select (w);
  } else {
    if (showBiblioForm || showMedlineAsnForm) {
      d = CreateDocument (title, 35);
    } else {
      d = CreateDocument (title, 32);
    }
    if (ArticleDisplay (d, uid)) {
      w = ParentWindow (d);
      RememberDocWindow (w, d, uid, medView, FALSE);
      Show (w);
      Select (w);
    } else {
      w = ParentWindow (d);
      Remove (w);
    }
  }
#ifdef WIN_MAC
  UnloadSegment ((VoidProc) FlatMainAccession);
  UnloadSegment ((VoidProc) FlatRefBest);
  UnloadSegment ((VoidProc) FlatSeqToReport);
#endif
}

extern void SaveAbstract (CharPtr dfault, DocUid uid, Boolean stdLoc)

{
  AsnIoPtr         aipout;
  FILE             *fp;
  MedlineEntryPtr  mep;
  Char             num [8];

  StartProgressIndicator ();
  mep = EntrezMedlineEntryGet (uid);
  if (mep != NULL) {
    fp = OpenAFile (dfault, FALSE, stdLoc, FALSE);
    if (fp != NULL) {
      if (showArticleForm) {
        MedlineEntryToDocFile (mep, fp);
        FilePuts ("\n\n\n\n", fp);
        FileClose (fp);
      } else if (showBiblioForm) {
        abstractNum++;
        sprintf (num, "%1d", abstractNum);
        FilePuts (num, fp);
        FilePuts ("\n", fp);
        MedlineEntryToDataFile (mep, fp);
        FilePuts ("\n\n\n\n", fp);
        FileClose (fp);
      } else if (showMedlineAsnForm) {
        aipout = AsnIoNew (ASNIO_TEXT_OUT, fp, NULL, NULL, NULL);
        MedlineEntryAsnWrite (mep, aipout, NULL);
        AsnIoFlush (aipout);
        FilePuts ("\n\n\n\n", fp);
        AsnIoClose (aipout);
      } else {
        FilePuts ("\n\n\n\n", fp);
        FileClose (fp);
      }
    }
    MedlineEntryFree (mep);
  }
  StopProgressIndicator ();
#ifdef WIN_MAC
  UnloadSegment ((VoidProc) FlatMainAccession);
  UnloadSegment ((VoidProc) FlatRefBest);
  UnloadSegment ((VoidProc) FlatSeqToReport);
#endif
}

static ColData table0 [1] = {
  {0, 0, 78, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE}
};

static ColData table1 [2] = {
  {0, 0, 15, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 63, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE}
};

static ColData table2 [2] = {
  {0, 0, 15, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 63, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE}
};

static ColData table3 [2] = {
  {0, 0, 15, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 63, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE}
};

static ColData table4 [3] = {
  {0, 0, 15, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 15, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 48, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE}
};

static ColData table5 [3] = {
  {0, 0, 15, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 45, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 18, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE}
};

static ColData table6 [3] = {
  {0, 0, 15, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 35, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 28, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE}
};

static ColData table7 [5] = {
  {0, 0, 15, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 63, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE}
};

static ColData table8 [5] = {
  {0, 0, 15, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 63, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE}
};

/*
static ColData table9 [2] = {
  {0, 20, 15, 3, NULL, 'r', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0,  0, 63, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE}
};
*/

static ColData table9 [2] = {
  {0, 6, 15, 1, NULL, 'r', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 63, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE}
};

static ColData table10 [2] = {
  {0, 0, 15, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, FALSE},
  {0, 0, 63, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE}
};

static Boolean DocumentReport (VoidPtr inst, Int2 cls, CharPtr string)

{
  DoC      doc;
  ParData  para;

#ifdef WIN_MSWIN
  doc = (DoC) (int) (long) inst;
#else
  doc = (DoC) inst;
#endif
  if (firstTime) {
    para.openSpace = FALSE;
    firstTime = FALSE;
  } else {
    para.openSpace = TRUE;
  }
  para.keepWithNext = FALSE;
  para.keepTogether = FALSE;
  para.newPage = FALSE;
  para.tabStops = FALSE;
  para.minLines = 0;
  para.minHeight = 0;
  switch (cls) {
    case 0:
      AppendText (doc, string, &para, table0, systemFont);
      break;
    case 1:
      AppendText (doc, string, &para, table1, standardFont);
      break;
    case 2:
      AppendText (doc, string, &para, table2, standardFont);
      break;
    case 3:
      para.openSpace = FALSE;
      AppendText (doc, string, &para, table3, standardFont);
      break;
    case 4:
      AppendText (doc, string, &para, table4, standardFont);
      break;
    case 5:
      AppendText (doc, string, &para, table5, standardFont);
      break;
    case 6:
      AppendText (doc, string, &para, table6, standardFont);
      break;
    case 7:
      para.openSpace = FALSE;
      AppendText (doc, string, &para, table7, standardFont);
      break;
    case 8:
      AppendText (doc, string, &para, table8, standardFont);
      break;
    case 9:
      AppendText (doc, string, &para, table9, standardFont);
      break;
    case 10:
      AppendText (doc, string, &para, table10, standardFont);
      break;
    default:
      break;
  }
  return TRUE;
}

static Boolean DisplaySequenceReport (DoC d, SeqEntryPtr sep)

{
  Int2         nItems;
  SeqViewPtr   pFlat;
  Boolean      rsult;

  rsult = FALSE;
  if (sep != NULL) {
    SeqEntryConvert (sep, Seq_code_iupacna);
    pFlat = FlatSeqEntryNew (sep, &nItems);
    if (pFlat != NULL) {
      firstTime = TRUE;
      rsult = FlatSeqToReport (pFlat, nItems, DocumentReport,
                               (VoidPtr) d, showSeq, charsPerLine);
    }
    pFlat = FlatSeqEntryFree (pFlat);
  }

  return rsult;
}

static void PopulateDoc (SeqEntryPtr sep, DoC d, ValNodePtr vnp)

{
  if (sep != NULL && d != NULL) {
    while (vnp != NULL) {
      AppendText (d, "New Report Item\n", &seqParFmt, &seqColFmt, programFont);
      vnp = vnp->next;
    }
  }
}

static Boolean DisplayNewReport (DoC d, SeqEntryPtr sep)

{
  ValNodePtr  featlist;
  RecT        r;
  Boolean     rsult;

  rsult = FALSE;
  if (sep != NULL) {
    SeqEntryConvert (sep, Seq_code_iupacna);
    if (spop == NULL) {
      spop = StdPrintOptionsNew (NULL);
      if (spop != NULL) {
        spop->newline = "\r";
        spop->indent = "";
      } else {
        Message (MSG_OK, "StdPrintOptionsNew failed");
      }
    }
    ObjectRect (d, &r);
    InsetRect (&r, 4, 4);
    seqColFmt.pixWidth = r.right - r.left;
    seqParFmt.openSpace = TRUE;
    /*
    featlist = SeqEntryGetItems (sep, 1, NULL, NULL, NULL, NULL, NULL);
    if (featlist != NULL) {
      seqParFmt.openSpace = TRUE;
      SetDocAutoAdjust (d, FALSE);
      PopulateDoc (sep, d, featlist);
      SetDocAutoAdjust (d, TRUE);
      AdjustDocScroll (d);
      FreeFeatureList (featlist);
    }
    */
    AppendText (d, "New Report\n", &seqParFmt, &seqColFmt, programFont);
    rsult = TRUE;
  }

  return rsult;
}

static Boolean IsGenBank (SeqEntryPtr sep)

{
  BioseqPtr  bsp;
  Uint1      repr;
  Boolean    rsult;

  rsult = FALSE;
  if (sep->choice == 1) {
    bsp = (BioseqPtr) sep->data.ptrvalue;
    repr = Bioseq_repr (bsp);
    if (repr == Seq_repr_raw || repr == Seq_repr_const) {
      if (ISA_na (bsp->mol)) {
        rsult = TRUE;
      } else if (showEmblForm) {
        Message (MSG_ERROR, "Protein record cannot be viewed in EMBL form.");
      } else {
        Message (MSG_ERROR, "Protein record cannot be viewed in GenBank form.");
      }
    } else {
      Message (MSG_ERROR, "Bad sequence repr %d", (int) repr);
    }
  } else {
    rsult = TRUE;
  }
  return rsult;
}

static Boolean DisplayGenbank (DoC d, SeqEntryPtr sep)

{
  FILE          *fp;
  Boolean       okay;
  AsnOptionPtr  optionHead;
  RecT          r;
  Char          str [256];
  DataVal       val;

  TmpNam (str);
  fp = FileOpen (str, "w");

  okay = FALSE;
  if (sep != NULL) {
    optionHead = NULL;
    val.intvalue = TRUE;
    AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_QUIET, val, NULL);
    AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_NO_NCBI, val, NULL);
    val.ptrvalue = StringSave ("\?\?\?");
    AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_DIV,
                  val, DefAsnOptionFree);
    val.ptrvalue = StringSave ("\?\?-\?\?\?-\?\?\?\?");
    AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_DATE,
                  val, DefAsnOptionFree);
    val.intvalue = TRUE;
    AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_RELEASE, val, NULL);
    val.intvalue = 1;
    AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_NUCL_PROT, val, NULL);
    if (showEmblForm) {
      val.intvalue = 1;
      AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_FORMAT, val, NULL);
    }
    if (IsGenBank (sep)) {
      if (SeqEntryToGenbank (fp, sep, optionHead) != 0) {
        okay = TRUE;
      } else {
        Beep ();
      }
    }
    AsnOptionFree (&optionHead, OP_TOGENBNK, 0);
  } else {
    Message (MSG_ERROR, "SeqEntryAsnRead failed");
  }
  FileClose (fp);
  if (okay) {
    ObjectRect (d, &r);
    InsetRect (&r, 4, 4);
    noWrapDisplay.pixWidth = r.right - r.left;
    DisplayFancy (d, str, NULL, &noWrapDisplay, displayFont, 4);
  }
  FileRemove (str);

  return okay;
}

static Boolean DisplayFasta (DoC d, SeqEntryPtr sep, Boolean is_na)

{
  FILE     *fp;
  Boolean  okay;
  RecT     r;
  Char     str [256];

  TmpNam (str);
  fp = FileOpen (str, "w");

  okay = FALSE;
  if (sep != NULL) {
    SeqEntryConvert (sep, Seq_code_iupacna);
    okay = SeqEntryToFasta (sep, fp, is_na);
  }
  FileClose (fp);
  if (okay) {
    ObjectRect (d, &r);
    InsetRect (&r, 4, 4);
    noWrapDisplay.pixWidth = r.right - r.left;
    DisplayFancy (d, str, NULL, &noWrapDisplay, displayFont, 4);
  } else {
    Beep ();
  }
  FileRemove (str);

  return okay;
}

typedef struct featstruct {
  Boolean   found;
  FILE      *fp;
  AsnIoPtr  aip;
  Boolean   is_na;
} FeatStruct, PNTR FeatStructPtr;

static Char *features [] = {"", "Gene", "Organism", "Coding", "Protein", "RNA",
                            "Publication", "Sequence", "Import", "Region", "Comment",
                            "Bond", "Site", "Restriction", "User", "Promoter"};

static void LIBCALLBACK GetSeqFeat (AsnExpOptStructPtr aeosp)

{
  BioseqPtr      bsp;
  Char           buffer [255];
  Int2           feat;
  FILE           *fp;
  FeatStructPtr  fsp;
  Int2           i;
  Uint1          repr;
  Uint1          residue;
  SeqFeatPtr     sfp;
  SeqPortPtr     spp;
  CharPtr        title;
  CharPtr        tmp;

  if (aeosp->dvp->intvalue == START_STRUCT) {
    fsp = (FeatStructPtr) aeosp->data;
    sfp = (SeqFeatPtr) aeosp->the_struct;
    feat = sfp->data.choice;
    if (feat > 0 && feat <= 15 && GetStatus (featChoice [feat]) && fsp != NULL && sfp != NULL) {
      fp = fsp->fp;
      bsp = BioseqFind (SeqLocId (sfp->location));
      if (bsp == NULL || fp == NULL) {
        return;
      }
      if ((Boolean) ISA_na (bsp->mol) != fsp->is_na) {
        return;
      }
      spp = SeqPortNewByLoc (sfp->location, Seq_code_iupacna);
      if (spp != NULL) {
        repr = Bioseq_repr (bsp);
        i = 0;
        title = BioseqGetTitle (bsp);
        tmp = StringMove (buffer, ">");
        tmp = SeqIdPrint (bsp->id, tmp, PRINTID_FASTA_LONG);
        tmp = StringMove (tmp, " [");
        tmp = StringMove (tmp, features [sfp->data.choice]);
        tmp = StringMove (tmp, "] ");
        StringNCpy (tmp, title, 200);
        fprintf (fp, "%s\n", buffer);
        while ((residue = SeqPortGetResidue (spp)) != SEQPORT_EOF) {
          if (residue == SEQPORT_EOS) {
            buffer [i] = '\0';
            fprintf (fp, "%s\n>Segment\n", buffer);
            i = 0;
          } else {
            buffer [i] = residue;
            i++;
            if (i >= charsPerLine) {
              buffer [i] = '\0';
              fprintf (fp, "%s\n", buffer);
              i = 0;
            }
          }
        }
        if (i != 0) {
          buffer [i] = '\0';
          fprintf (fp, "%s\n", buffer);
        }
        SeqPortFree (spp);
        fsp->found = TRUE;
      }
    }
  }
}

static Boolean SeqEntryToFeat (SeqEntryPtr sep, FILE *fp, Boolean is_na)

{
  AsnExpOptPtr   aeop;
  AsnIoPtr       aip;
  FeatStructPtr  fsp;
  Boolean        rsult;

  rsult = FALSE;
  if (sep != NULL && fp != NULL) {
    fsp = MemNew (sizeof (FeatStruct));
    if (fsp != NULL) {
      aip = AsnIoNullOpen ();
      if (aip != NULL) {
        fsp->found = FALSE;
        fsp->fp = fp;
        fsp->aip = AsnIoNew (ASNIO_TEXT_OUT, fp, NULL, NULL, NULL);
        fsp->is_na = is_na;
        aeop = AsnExpOptNew (aip, "Seq-feat", (Pointer) fsp, GetSeqFeat);
        if (aeop != NULL) {
          SeqEntryAsnWrite (sep, aip, NULL);
          fflush (fp);
          AsnExpOptFree (aip, aeop);
          rsult = fsp->found;
        } else {
          ErrPost (100, 1, "AsnExpOptNew failed");
        }
        AsnIoClose (aip);
      }
      MemFree (fsp);
    }
  }
  return rsult;
}

static Boolean DisplayFeat (DoC d, SeqEntryPtr sep, Boolean is_na)

{
  FILE     *fp;
  Boolean  okay;
  RecT     r;
  Char     str [256];

  TmpNam (str);
  fp = FileOpen (str, "w");

  okay = FALSE;
  if (sep != NULL) {
    SeqEntryConvert (sep, Seq_code_iupacna);
    okay = SeqEntryToFeat (sep, fp, is_na);
  }
  FileClose (fp);
  if (okay) {
    ObjectRect (d, &r);
    InsetRect (&r, 4, 4);
    noWrapDisplay.pixWidth = r.right - r.left;
    DisplayFancy (d, str, NULL, &noWrapDisplay, displayFont, 4);
  } else {
    Beep ();
  }
  FileRemove (str);

  return okay;
}

static Boolean DisplaySeqAsn (DoC d, SeqEntryPtr sep)

{
  AsnIoPtr  aipout;
  FILE      *fp;
  Boolean   okay;
  RecT      r;
  Char      str [256];

  TmpNam (str);
  fp = FileOpen (str, "w");
  aipout = AsnIoNew (ASNIO_TEXT_OUT, fp, NULL, NULL, NULL);

  okay = FALSE;
  if (sep != NULL) {
    SeqEntryConvert (sep, Seq_code_iupacna);
    okay = SeqEntryAsnWrite (sep, aipout, NULL);
  }
  AsnIoClose (aipout);
  if (okay) {
    ObjectRect (d, &r);
    InsetRect (&r, 4, 4);
    noWrapDisplay.pixWidth = r.right - r.left;
    DisplayFancy (d, str, NULL, &noWrapDisplay, displayFont, 4);
  } else {
    Beep ();
  }
  FileRemove (str);

  return okay;
}

static Boolean SequenceDisplay (DoC d, DocUid uid, Boolean is_na)

{
  AsnIoPtr      aip;
  AsnModulePtr  amp;
  AsnTypePtr    atp;
  Char          path [PATH_MAX];
  Boolean       rsult;
  SeqEntryPtr   sep;

  rsult = FALSE;
  sep = NULL;
  StartProgressIndicator ();
  if (uid != 0) {
    sep = EntrezSeqEntryGet (uid, retcode);
  } else {
    if (GetInputFileName (path, sizeof (path), "", "")) {
      aip = AsnIoOpen (path, "r");
      if (aip != NULL) {
        atp = AsnFind ("Seq-entry");
        amp = AsnAllModPtr ();
        atp = AsnReadId (aip, amp, atp);
        sep = SeqEntryAsnRead (aip, atp);
        AsnIoClose (aip);
      }
    }
  }
  if (sep != NULL) {
    if (showSeqAsnForm) {
      rsult = DisplaySeqAsn (d, sep);
    } else if (showExtractForm) {
      rsult = DisplayFeat (d, sep, is_na);
    } else if (showFastaForm) {
      rsult = DisplayFasta (d, sep, is_na);
    } else if (showGenbankForm || showEmblForm) {
      rsult = DisplayGenbank (d, sep);
    } else if (showNewReportForm) {
      rsult = DisplayNewReport (d, sep);
    } else if (showReportForm) {
      rsult = DisplaySequenceReport (d, sep);
    }
    if (launchApp) {
      aip = AsnIoOpen ("CNNkludge.prt", "w");
      if (aip != NULL) {
        SeqEntryConvert (sep, Seq_code_iupacna);
        SeqEntryAsnWrite (sep, aip, NULL);
        AsnIoFlush (aip);
        AsnIoClose (aip);
      }
      Nlm_LaunchApp ("CNNkludge");
    }
    SeqEntryFree (sep);
  }
  StopProgressIndicator ();
  return rsult;
}

extern void LoadSequence (CharPtr title, DocUid uid, Boolean is_na)

{
  DoC  d;
  WindoW    w;

  w = FindUidWindow (uid, seqView);
  if (w != NULL) {
    Show (w);
    Select (w);
  } else {
    if (showSeqAsnForm || showExtractForm || showFastaForm || showGenbankForm || showEmblForm) {
      d = CreateDocument (title, 35);
    } else {
      d = CreateDocument (title, 32);
    }
    if (SequenceDisplay (d, uid, is_na)) {
      w = ParentWindow (d);
      RememberDocWindow (w, d, uid, seqView, TRUE);
      Show (w);
      Select (w);
    } else {
      w = ParentWindow (d);
      Remove (w);
    }
  }
#ifdef WIN_MAC
  UnloadSegment ((VoidProc) FlatMainAccession);
  UnloadSegment ((VoidProc) FlatRefBest);
  UnloadSegment ((VoidProc) FlatSeqToReport);
#endif
}

extern void SaveSequence (CharPtr dfault, DocUid uid, Boolean is_na, Boolean stdLoc)

{
  AsnIoPtr      aipout;
  FILE          *fp;
  AsnOptionPtr  optionHead;
  SeqEntryPtr   sep;
  DataVal       val;

  StartProgressIndicator ();
  sep = EntrezSeqEntryGet (uid, retcode);
  if (sep != NULL) {
    fp = OpenAFile (dfault, TRUE, stdLoc, FALSE);
    if (fp != NULL) {
      if (showSeqAsnForm) {
        aipout = AsnIoNew (ASNIO_TEXT_OUT, fp, NULL, NULL, NULL);
        SeqEntryConvert (sep, Seq_code_iupacna);
        SeqEntryAsnWrite (sep, aipout, NULL);
        AsnIoFlush (aipout);
        FilePuts ("\n\n\n\n", fp);
        AsnIoClose (aipout);
      } else if (showExtractForm) {
        SeqEntryToFeat (sep, fp, is_na);
        FilePuts ("\n", fp);
        FileClose (fp);
      } else if (showFastaForm) {
        SeqEntryToFasta (sep, fp, is_na);
        FilePuts ("\n", fp);
        FileClose (fp);
      } else if (showGenbankForm || showEmblForm) {
        optionHead = NULL;
        val.intvalue = TRUE;
        AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_QUIET, val, NULL);
        AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_NO_NCBI, val, NULL);
        val.ptrvalue = StringSave ("\?\?\?");
        AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_DIV,
                      val, DefAsnOptionFree);
        val.ptrvalue = StringSave ("\?\?-\?\?\?-\?\?\?\?");
        AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_DATE,
                      val, DefAsnOptionFree);
        val.intvalue = TRUE;
        AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_RELEASE, val, NULL);
        val.intvalue = 1;
        AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_NUCL_PROT, val, NULL);
        if (showEmblForm) {
          val.intvalue = 1;
          AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_FORMAT, val, NULL);
        }
        if (IsGenBank (sep)) {
          SeqEntryToGenbank (fp, sep, optionHead);
        }
        AsnOptionFree (&optionHead, OP_TOGENBNK, 0);
        FilePuts ("\n\n\n\n", fp);
        FileClose (fp);
      } else if (showNewReportForm) {
        FilePuts ("\n\n\n\n", fp);
        FileClose (fp);
      } else if (showReportForm) {
        SeqEntryToFile (sep, fp, showSeq, charsPerLine, FALSE);
        FilePuts ("\n\n\n\n", fp);
        FileClose (fp);
      } else {
        FilePuts ("\n\n\n\n", fp);
        FileClose (fp);
      }
    }
    SeqEntryFree (sep);
  }
  StopProgressIndicator ();
#ifdef WIN_MAC
  UnloadSegment ((VoidProc) FlatMainAccession);
  UnloadSegment ((VoidProc) FlatRefBest);
  UnloadSegment ((VoidProc) FlatSeqToReport);
#endif
}

static void ChangeSeqShow (IteM i)

{
  if (GetStatus (showSeqItem)) {
    showSeq = TRUE;
    SetAppParam ("ENTREZ", "PREFERENCES", "SHOWSEQ", "TRUE");
  } else {
    showSeq = FALSE;
    SetAppParam ("ENTREZ", "PREFERENCES", "SHOWSEQ", "FALSE");
  }
}

extern void SetAbstractFonts (void)

{
  Char  str [32];

#ifdef WIN_MAC
  jourfnt = ChooseFont ("JOURNAL", "Geneva,10,i");
  volfnt = ChooseFont ("VOLUME", "Geneva,10,b");
  pagesfnt = ChooseFont ("PAGES", "Geneva,10");
  titlefnt = ChooseFont ("TITLE", "Times,14,b");
  authorsfnt = ChooseFont ("AUTHORS", "Times,14");
  affilfnt = ChooseFont ("AFFILIATION", "Times,12");
  abstractfnt = ChooseFont ("ABSTRACT", "Geneva,10");
  meshfnt = ChooseFont ("MESH", "Monaco,9");
#endif
#ifdef WIN_MSWIN
  jourfnt = ChooseFont ("JOURNAL", "Arial,11,i");
  volfnt = ChooseFont ("VOLUME", "Arial,11,b");
  pagesfnt = ChooseFont ("PAGES", "Arial,11");
  titlefnt = ChooseFont ("TITLE", "Times New Roman,14,b");
  authorsfnt = ChooseFont ("AUTHORS", "Times New Roman,14");
  affilfnt = ChooseFont ("AFFILIATION", "Times New Roman,11");
  abstractfnt = ChooseFont ("ABSTRACT", "Times New Roman,11");
  meshfnt = ChooseFont ("MESH", "Times New Roman,9");
#endif
#ifdef WIN_MOTIF
  jourfnt = ChooseFont ("JOURNAL", "Helvetica,12,i");
  volfnt = ChooseFont ("VOLUME", "Helvetica,12,b");
  pagesfnt = ChooseFont ("PAGES", "Helvetica,12");
  titlefnt = ChooseFont ("TITLE", "Times,18,b");
  authorsfnt = ChooseFont ("AUTHORS", "Times,18");
  affilfnt = ChooseFont ("AFFILIATION", "Times,14");
  abstractfnt = ChooseFont ("ABSTRACT", "Times,14");
  meshfnt = ChooseFont ("MESH", "Times,12");
#endif
#ifdef OS_UNIX
  if (! GetAppParam ("ENTREZ", "PRINT", "COMMAND", "", str, sizeof (str))) {
    SetAppParam ("ENTREZ", "PRINT", "COMMAND", "lp");
  }
#endif
  if (! GetAppParam ("ENTREZ", "PREFERENCES", "MAXLOAD", "", str, sizeof (str))) {
    SetAppParam ("ENTREZ", "PREFERENCES", "MAXLOAD", "200");
  }
}

#define ARTICLE 1
#define BIBLIO 2
#define MEDASN 3

extern void ChangeArticle (ChoicE c)

{
  Int2  val;

  val = GetValue (c);
  if (val == ARTICLE) {
    medView = VIEW_ARTICLE;
    showArticleForm = TRUE;
  } else {
    showArticleForm = FALSE;
  }
  if (val == BIBLIO) {
    medView = VIEW_BIBLIO;
    showBiblioForm = TRUE;
  } else {
    showBiblioForm = FALSE;
  }
  if (val == MEDASN) {
    medView = VIEW_MEDASN;
    showMedlineAsnForm = TRUE;
  } else {
    showMedlineAsnForm = FALSE;
  }
}

extern void ChangeReport (ChoicE c)

{
  Int2  val;

  val = GetValue (c);
  if (val == REPORTFORM) {
    seqView = VIEW_REPORT;
    showReportForm = TRUE;
  } else {
    showReportForm = FALSE;
  }
  if (val == GNBKFORM) {
    seqView = VIEW_GNBK;
    showGenbankForm = TRUE;
  } else {
    showGenbankForm = FALSE;
  }
  if (val == EMBLFORM) {
    seqView = VIEW_EMBL;
    showEmblForm = TRUE;
  } else {
    showEmblForm = FALSE;
  }
  if (val == FASTFORM) {
    seqView = VIEW_FAST;
    showFastaForm = TRUE;
  } else {
    showFastaForm = FALSE;
  }
  if (val == FEATFORM) {
    seqView = VIEW_FEAT;
    showExtractForm = TRUE;
  } else {
    showExtractForm = FALSE;
  }
  if (val == SEQASNFORM) {
    seqView = VIEW_SEQASN;
    showSeqAsnForm = TRUE;
  } else {
    showSeqAsnForm = FALSE;
  }
  if (val == NEWFORM) {
    seqView = VIEW_NEW;
    showNewReportForm = TRUE;
  } else {
    showNewReportForm = FALSE;
  }
}

extern void ChangeClass (ChoicE c)

{
  retcode = 4 - GetValue (c);
}

extern void LoadAbstractOptions (MenU m)

{
  Int2  val;

  articleChoice = ChoiceGroup (m, ChangeArticle);
  ChoiceItem (articleChoice, "MEDLINE Report");
  ChoiceItem (articleChoice, "MEDLARS Format");
  ChoiceItem (articleChoice, "MEDLINE ASN.1");
  SetValue (articleChoice, 1);
  showArticleForm = TRUE;
  showBiblioForm = FALSE;
  showMedlineAsnForm = FALSE;
  medView = VIEW_ARTICLE;
  docActive = FALSE;
  abstractNum = 0;
  val = ChooseSetting ("MEDREPORT", 1);
  if (val != 1) {
    SetValue (articleChoice, val);
    ChangeArticle (articleChoice);
  }
}

extern void LoadSequencePrefs (MenU m)

{
  Char  str [32];

  showSeqItem = StatusItem (m, "Show Sequence", ChangeSeqShow);
  showSeq = FALSE;
  if (GetAppParam ("ENTREZ", "PREFERENCES", "SHOWSEQ", "", str, sizeof (str) - 1)) {
    if (StringICmp (str, "TRUE") == 0) {
      showSeq = TRUE;
      SetStatus (showSeqItem, TRUE);
    }
  } else {
    showSeq = TRUE;
    SetStatus (showSeqItem, TRUE);
    SetAppParam ("ENTREZ", "PREFERENCES", "SHOWSEQ", "TRUE");
  }
  launchApp = FALSE;
  if (GetAppParam ("ENTREZ", "PREFERENCES", "LAUNCHAPP", "", str, sizeof (str) - 1)) {
    if (StringICmp (str, "TRUE") == 0) {
      launchApp = TRUE;
    }
  }
}

static void ChangeResidueCount (ChoicE c)

{
  Int2  value;

  value = GetValue (c);
  switch (value) {
    case 1:
      SetAppParam ("ENTREZ", "PREFERENCES", "CHARSPERLINE", "30");
      charsPerLine = 30;
      break;
    case 2:
      SetAppParam ("ENTREZ", "PREFERENCES", "CHARSPERLINE", "40");
      charsPerLine = 40;
      break;
    case 3:
      SetAppParam ("ENTREZ", "PREFERENCES", "CHARSPERLINE", "50");
      charsPerLine = 50;
      break;
    case 4:
      SetAppParam ("ENTREZ", "PREFERENCES", "CHARSPERLINE", "60");
      charsPerLine = 60;
      break;
    default:
      SetAppParam ("ENTREZ", "PREFERENCES", "CHARSPERLINE", "50");
      charsPerLine = 50;
      break;
  }
}

extern void LoadResiduesPrefs (MenU m)

{
  Char  str [32];
  MenU  sub;
  Int2  value;

  sub = SubMenu (m, "Chars Per Line");
  residuesChoice = ChoiceGroup (sub, ChangeResidueCount);
  ChoiceItem (residuesChoice, "30");
  ChoiceItem (residuesChoice, "40");
  ChoiceItem (residuesChoice, "50");
  ChoiceItem (residuesChoice, "60");
  charsPerLine = 0;
  if (GetAppParam ("ENTREZ", "PREFERENCES", "CHARSPERLINE", "", str, sizeof (str) - 1)) {
    if (StrToInt (str, &value) && (value % 10) == 0) {
      if (value >= 30 && value <= 60) {
        SetValue (residuesChoice, (value / 10) - 2);
        charsPerLine = value;
      }
    }
  }
  if (charsPerLine != 0) {
  } else if (smallScreen) {
    SetValue (residuesChoice, 2);
    charsPerLine = 40;
  } else {
    SetValue (residuesChoice, 3);
    charsPerLine = 50;
  }
}

extern void LoadReportClass (MenU m)

{
  Int2  val;

  reportClass = ChoiceGroup (m, ChangeClass);
  ChoiceItem (reportClass, "NucProt");
  ChoiceItem (reportClass, "SegSet");
  ChoiceItem (reportClass, "BioSeq");
  SetValue (reportClass, 1);
  retcode = 3;
  val = ChooseSetting ("SEQCLASS", 1);
  if (val != 1) {
    SetValue (reportClass, val);
    ChangeClass (reportClass);
  }
}

static void AllFeatProc (IteM i)

{
  Int2  j;

  for (j = 1; j <= 15; j++) {
    SetStatus (featChoice [j], TRUE);
  }
}

static void NoFeatProc (IteM i)

{
  Int2  j;

  for (j = 1; j <= 15; j++) {
    SetStatus (featChoice [j], FALSE);
  }
}

extern void LoadFeatOptions (MenU m)

{
  Int2  j;

  CommandItem (m, "All Features", AllFeatProc);
  CommandItem (m, "No Features", NoFeatProc);
  SeparatorItem (m);
  featChoice [0] = NULL;
  j = 1;
  featChoice [j] = StatusItem (m, "Gene", NULL); j++;
  featChoice [j] = StatusItem (m, "Organism", NULL); j++;
  featChoice [j] = StatusItem (m, "Coding", NULL); j++;
  featChoice [j] = StatusItem (m, "Protein", NULL); j++;
  featChoice [j] = StatusItem (m, "RNA", NULL); j++;
  featChoice [j] = StatusItem (m, "Publication", NULL); j++;
  featChoice [j] = StatusItem (m, "Sequence", NULL); j++;
  featChoice [j] = StatusItem (m, "Import", NULL); j++;
  featChoice [j] = StatusItem (m, "Region", NULL); j++;
  featChoice [j] = StatusItem (m, "Comment", NULL); j++;
  featChoice [j] = StatusItem (m, "Bond", NULL); j++;
  featChoice [j] = StatusItem (m, "Site", NULL); j++;
  featChoice [j] = StatusItem (m, "Restriction", NULL); j++;
  featChoice [j] = StatusItem (m, "User", NULL); j++;
  featChoice [j] = StatusItem (m, "Promoter", NULL); j++;
  SetStatus (featChoice [3], TRUE);
}

extern void LoadReportOptions (MenU m)

{
  Int2  i;
  Int2  val;

  REPORTFORM = 0;
  GNBKFORM = 0;
  EMBLFORM = 0;
  FASTFORM = 0;
  FEATFORM = 0;
  SEQASNFORM = 0;
  NEWFORM = 0;
  reportChoice = ChoiceGroup (m, ChangeReport);
  i = 0;
  ChoiceItem (reportChoice, "Sequence Report"); i++; REPORTFORM = i;
  ChoiceItem (reportChoice, "GenBank Format"); i++; GNBKFORM = i;
  if (showEmbl) {
    ChoiceItem (reportChoice, "EMBL Format"); i++; EMBLFORM = i;
  }
  ChoiceItem (reportChoice, "FASTA Format"); i++; FASTFORM = i;
  if (showFeat) {
    ChoiceItem (reportChoice, "Feature Extract"); i++; FEATFORM = i;
  }
  ChoiceItem (reportChoice, "Sequence ASN.1"); i++; SEQASNFORM = i;
  if (newReport) {
    ChoiceItem (reportChoice, "New Report"); i++; NEWFORM = i;
  }
  SetValue (reportChoice, 1);
  showReportForm = TRUE;
  showGenbankForm = FALSE;
  showEmblForm = FALSE;
  showFastaForm = FALSE;
  showExtractForm = FALSE;
  showSeqAsnForm = FALSE;
  showNewReportForm = FALSE;
  seqView = VIEW_REPORT;
  val = ChooseSetting ("SEQREPORT", 1);
  if (val != 1) {
    SetValue (reportChoice, val);
    ChangeReport (reportChoice);
  }
  docCount = 0;
#ifdef WIN_MAC
  headingFont = ChooseFont ("HEADING", "Monaco,9,b");
  referenceFont = ChooseFont ("REFERENCE", "Geneva,10");
  sequenceFont = ChooseFont ("SEQUENCE", "Monaco,9");
  standardFont = ChooseFont ("STANDARD", "Monaco,9");
  displayFont = ChooseFont ("DISPLAY", "Monaco,9");
#endif
#ifdef WIN_MSWIN
  headingFont = ChooseFont ("HEADING", "Arial,11,b");
  referenceFont = ChooseFont ("REFERENCE", "Arial,11");
  sequenceFont = ChooseFont ("SEQUENCE", "Courier,9");
  standardFont = ChooseFont ("STANDARD", "Times New Roman,11");
  displayFont = ChooseFont ("DISPLAY", "Courier,9");
#endif
#ifdef WIN_MOTIF
  headingFont = ChooseFont ("HEADING", "Helvetica,14,b");
  referenceFont = ChooseFont ("REFERENCE", "Helvetica,12");
  sequenceFont = ChooseFont ("SEQUENCE", "Courier,12");
  standardFont = ChooseFont ("STANDARD", "Times,12");
  displayFont = ChooseFont ("DISPLAY", "Courier,10");
#endif
  if (smallScreen) {
    table0 [0].pixWidth = 27 * stdCharWidth;
    table1 [0].pixWidth = 6 * stdCharWidth;
    table1 [1].pixWidth = 21 * stdCharWidth;
    table1 [1].font = headingFont;
    table2 [0].pixWidth = 6 * stdCharWidth;
    table2 [1].pixWidth = 21 * stdCharWidth;
    table3 [0].pixWidth = 6 * stdCharWidth;
    table3 [1].pixWidth = 21 * stdCharWidth;
    table4 [0].pixWidth = 6 * stdCharWidth;
    table4 [1].pixWidth = 6 * stdCharWidth;
    table4 [2].pixWidth = 15 * stdCharWidth;
    table5 [0].pixWidth = 6 * stdCharWidth;
    table5 [1].pixWidth = 14 * stdCharWidth;
    table5 [2].pixWidth = 7 * stdCharWidth;
    table6 [0].pixWidth = 6 * stdCharWidth;
    table6 [1].pixWidth = 9 * stdCharWidth;
    table6 [2].pixWidth = 12 * stdCharWidth;
    table7 [0].pixWidth = 6 * stdCharWidth;
    table7 [1].pixWidth = 21 * stdCharWidth;
    table7 [1].font = referenceFont;
    table8 [0].pixWidth = 6 * stdCharWidth;
    table8 [1].pixWidth = 21 * stdCharWidth;
    table8 [1].font = referenceFont;
    table9 [0].pixWidth = 6 * stdCharWidth;
    table9 [1].pixWidth = 21 * stdCharWidth;
    table9 [1].font = sequenceFont;
    table10 [0].pixWidth = 6 * stdCharWidth;
    table10 [1].pixWidth = 21 * stdCharWidth;
    table10 [1].font = displayFont;
  } else {
    table0 [0].pixWidth = 32 * stdCharWidth;
    table1 [0].pixWidth = 6 * stdCharWidth;
    table1 [1].pixWidth = 26 * stdCharWidth;
    table1 [1].font = headingFont;
    table2 [0].pixWidth = 6 * stdCharWidth;
    table2 [1].pixWidth = 26 * stdCharWidth;
    table3 [0].pixWidth = 6 * stdCharWidth;
    table3 [1].pixWidth = 26 * stdCharWidth;
    table4 [0].pixWidth = 6 * stdCharWidth;
    table4 [1].pixWidth = 6 * stdCharWidth;
    table4 [2].pixWidth = 20 * stdCharWidth;
    table5 [0].pixWidth = 6 * stdCharWidth;
    table5 [1].pixWidth = 19 * stdCharWidth;
    table5 [2].pixWidth = 7 * stdCharWidth;
    table6 [0].pixWidth = 6 * stdCharWidth;
    table6 [1].pixWidth = 14 * stdCharWidth;
    table6 [2].pixWidth = 12 * stdCharWidth;
    table7 [0].pixWidth = 6 * stdCharWidth;
    table7 [1].pixWidth = 26 * stdCharWidth;
    table7 [1].font = referenceFont;
    table8 [0].pixWidth = 6 * stdCharWidth;
    table8 [1].pixWidth = 26 * stdCharWidth;
    table8 [1].font = referenceFont;
    table9 [0].pixWidth = 6 * stdCharWidth;
    table9 [1].pixWidth = 26 * stdCharWidth;
    table9 [1].font = sequenceFont;
    table10 [0].pixWidth = 6 * stdCharWidth;
    table10 [1].pixWidth = 26 * stdCharWidth;
    table10 [1].font = displayFont;
  }
}
