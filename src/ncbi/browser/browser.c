/*   browser.c
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
* File Name:  browser.c
*
* Author:  Kans, Schuler
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.39 $
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
#include <document.h>
#include <accentr.h>
#include <objall.h>
#include <objcode.h>
#include <prtutil.h>
#include "browser.h"

WindoW   termWindow;
WindoW   chosenWindow;
WindoW   browseWindow;

Boolean  smallScreen;
Boolean  showFeat;
Boolean  showEmbl;
Boolean  newReport;
Int2     threads;

IteM     openItem;
IteM     closeItem;
IteM     saveItem;
IteM     saveAsItem;
IteM     saveAllItem;
IteM     printItem;
TexT     term;

static WindoW   aboutWindow;
static WindoW   detailedWindow;
static DoC      detailedDoc;
static WindoW   configWindow;
static DoC      configDoc;
static ColData  col [3] = {{ 50, 0, 20, 0, NULL, 'r', TRUE, FALSE, FALSE, FALSE, FALSE},
                           {100, 0, 20, 0, NULL, 'r', TRUE, FALSE, FALSE, FALSE, FALSE},
                           {100, 0, 20, 0, NULL, 'r', TRUE, FALSE, FALSE, FALSE, TRUE}};
static ColData  displayDetailed = {0, 0, 80, 0, NULL, 'l', TRUE, FALSE, FALSE, FALSE, TRUE};

static FonT     fnt1;
static FonT     fnt2;

static Char     entrezVersion [32];

static void CheckPathProc (CharPtr filename, CharPtr prompt)

{
  Char     ch;
  Int2     num;
  ParData  para;
  RecT     r;
  Char     text [64];

  Reset (configDoc);
  if (filename != NULL) {
    para.openSpace = FALSE;
    para.keepWithNext = FALSE;
    para.keepTogether = FALSE;
    para.newPage = FALSE;
    para.tabStops = FALSE;
    para.minLines = 0;
    para.minHeight = 0;
    ch = *filename;
    num = 0;
    while (ch != '\0') {
      if (ch >= ' ' && ch < '~') {
        sprintf (text, "%3d\t%4d\t%c\n", (int) num, (int) ch, ch);
      } else {
        sprintf (text, "%3d\t%4d\n", (int) num, (int) ch);
      }
      AppendText (configDoc, text, &para, col, programFont);
      filename++;
      ch = *filename;
      num++;
    }
    Select (configDoc);
    ObjectRect (configDoc, &r);
    InsetRect (&r, 3, 3);
    InvalRect (&r);
    Update ();
  }
}

static void TrimFileName (CharPtr filename)

{
  Int2  len;

  len = StringLen (filename);
  while (len > 0 && filename [len - 1] != DIRDELIMCHR) {
    len--;
  }
  filename [len] = '\0';
}

static void SetRootProc (IteM i)

{
  Char  filename [256];

  if (GetInputFileName (filename, sizeof (filename), "", "")) {
    TrimFileName (filename);
    SetAppParam ("NCBI", "NCBI", "ROOT", filename);
    CheckPathProc (filename, "ROOT path\n");
  } else {
    Reset (configDoc);
  }
}

static void SetAsnloadProc (IteM i)

{
  Char  filename [256];

  if (GetInputFileName (filename, sizeof (filename), "", "")) {
    TrimFileName (filename);
    SetAppParam ("NCBI", "NCBI", "ASNLOAD", filename);
    CheckPathProc (filename, "ASNLOAD path\n");
  } else {
    Reset (configDoc);
  }
}

static void SetDataProc (IteM i)

{
  Char  filename [256];

  if (GetInputFileName (filename, sizeof (filename), "", "")) {
    TrimFileName (filename);
    SetAppParam ("NCBI", "NCBI", "DATA", filename);
    CheckPathProc (filename, "DATA path\n");
  } else {
    Reset (configDoc);
  }
}

static void QuitProc (IteM i)

{
  QuitProgram ();
}

static void CloseTermWindowProc (WindoW w)

{
#ifdef WIN_MSWIN
  QuitProgram ();
#endif
#ifdef WIN_MOTIF
  QuitProgram ();
#endif
}

static void HideWindowProc (WindoW w)

{
  Hide (w);
}

static void HideAboutPanelProc (PaneL p, PoinT pt)

{
  Hide (aboutWindow);
}

static void CenterLine (RectPtr rptr, CharPtr text, FonT fnt)

{
  if (fnt != NULL) {
    SelectFont (fnt);
  }
  rptr->bottom = rptr->top + LineHeight ();
  DrawString (rptr, text, 'c', FALSE);
  rptr->top = rptr->bottom;
}

static void DrawAbout (PaneL p)

{
  RecT  r;

  ObjectRect (p, &r);
  InsetRect (&r, 4, 4);
  r.top += 10;
  Blue ();
  CenterLine (&r, "Entrez", fnt1);
  r.top += 5;
  CenterLine (&r, "Entrez Application Version 1.92", programFont);
  r.top += 5;
  CenterLine (&r, entrezVersion, programFont);
  r.top += 10;
  CenterLine (&r, "National Center for Biotechnology Information", systemFont);
  r.top += 5;
  CenterLine (&r, "National Library of Medicine", systemFont);
  r.top += 5;
  CenterLine (&r, "National Institutes of Health", systemFont);
  r.top += 10;
  CenterLine (&r, "(301) 496-2475", systemFont);
  r.top += 5;
  CenterLine (&r, "entrez@ncbi.nlm.nih.gov", systemFont);
}

static void AboutDoneProc (ButtoN b)

{
  WindoW  w;

  w = ParentWindow (b);
  Hide (w);
}

static void AboutMoreProc (ButtoN b)

{
  CharPtr  info;
  RecT     r;

  Hide (aboutWindow);
  Reset (detailedDoc);
  info = EntrezDetailedInfo ();
  ObjectRect (detailedDoc, &r);
  InsetRect (&r, 4, 4);
  displayDetailed.pixWidth = r.right - r.left;
  if (info == NULL) {
    AppendText (detailedDoc, "No detailed information available\n",
                NULL, &displayDetailed, programFont);
  } else {
    AppendText (detailedDoc, info, NULL, &displayDetailed, programFont);
  }
  Show (detailedWindow);
  Select (detailedWindow);
}

static void AboutProc (IteM i)

{
  Show (aboutWindow);
  Select (aboutWindow);
}

static void SetupConfigWindow (WindoW w)

{
  GrouP  g;
  GrouP  h;

  SelectFont (programFont);
  col [0].pixWidth = 50 + MaxCharWidth () * 3 / 2;
  col [1].pixWidth = 150 + MaxCharWidth () * 4 / 2 - col [0].pixWidth;
  col [2].pixWidth = 250 + MaxCharWidth () * 2 / 2 - col [0].pixWidth - col [1].pixWidth;
  SelectFont (systemFont);
  h = HiddenGroup (w, 0, 5, NULL);
  SetGroupSpacing (h, 3, 10);
  StaticPrompt (h, "(Use the EntrezCf configuration program)",
                0, stdLineHeight, systemFont, 'c');
  g = HiddenGroup (h, 3, 0, NULL);
  SetGroupMargins (g, 4, 0);
  SetGroupSpacing (g, 0, 0);
  StaticPrompt (g, "Position", 100, stdLineHeight, systemFont, 'c');
  StaticPrompt (g, "Ascii", 100, stdLineHeight, systemFont, 'c');
  StaticPrompt (g, "Character", 100, stdLineHeight, systemFont, 'c');
  configDoc = DocumentPanel (h, 300, 200);
}

static void SetupConfigMenus (WindoW w)

{
  IteM  i;
  MenU  m;

#ifdef WIN_MAC
  m = AppleMenu (NULL);
  i = CommandItem (m, "About Entrez...", AboutProc);
  SeparatorItem (m);
  DeskAccGroup (m);
#endif
  m = PulldownMenu (w, "File");
#ifdef WIN_MSWIN
  i = CommandItem (m, "About Entrez...", AboutProc);
  SeparatorItem (m);
#endif
#ifdef WIN_MOTIF
  i = CommandItem (m, "About Entrez...", AboutProc);
  SeparatorItem (m);
#endif
  i = CommandItem (m, "Set Root Path", SetRootProc);
  i = CommandItem (m, "Set Asnload Path", SetAsnloadProc);
  i = CommandItem (m, "Set Data Path", SetDataProc);
  SeparatorItem (m);
  i = CommandItem (m, "Quit/Q", QuitProc);
}

static void SetupCDFailed (void)

{
#ifdef WIN_MAC
  SetupConfigMenus (NULL);
#endif
  configWindow = FixedWindow (-50, -33, -10, -10, "Configuration Viewer", NULL);
#ifndef WIN_MAC
  SetupConfigMenus (configWindow);
#endif
  SetupConfigWindow (configWindow);
  Show (configWindow);
  ProcessEvents ();
}

static Boolean InitDataServices (BoolPtr is_network_ptr)

{
  return EntrezInit ("Browser", FALSE, is_network_ptr);
}

static Boolean SetupCD (void)

{
  Boolean  rsult;
  Boolean  is_network;
#ifdef WIN_MAC
  Int2     count;
  ErrDesc  err;
  Char     str [16];
  Int2     thisVol;
#endif

  rsult = FALSE;
  ErrSetOpts (ERR_IGNORE, 0);
  threads++;
  if (InitDataServices (&is_network)) {
    rsult = TRUE;
  }
#ifdef WIN_MAC
  if (! rsult && ! is_network) {
    GetAppParam ("NCBI", "NCBI", "ROOT", "", str, sizeof (str));
    if (StringICmp (str, "SEQDATA:") == 0) {
      thisVol = 0;
    } else if (StringICmp (str, "REFDATA:") == 0) {
      thisVol = 1;
    } else if (StringICmp (str, "MEDDATA:") == 0) {
      thisVol = 2;
    } else {
      thisVol = 3;
    }
    if (thisVol < 3) {
      count = 0;
      while (count < 3 && (! rsult)) {
        thisVol = (thisVol + 1) % 3;
        switch (thisVol) {
          case 0 :
            SetAppParam ("NCBI", "NCBI", "ROOT", "SEQDATA:");
            break;
          case 1 :
            SetAppParam ("NCBI", "NCBI", "ROOT", "REFDATA:");
            break;
          case 2 :
            SetAppParam ("NCBI", "NCBI", "ROOT", "MEDDATA:");
            break;
          default :
            SetAppParam ("NCBI", "NCBI", "ROOT", "SEQDATA:");
            count = 3;
            break;
        }
        if (InitDataServices (&is_network)) {
          rsult = TRUE;
        }
        count++;
      }
    }
  }
  ErrFetch (&err);
#endif
  if (! rsult && ! is_network) {
    ErrSetOpts (ERR_CONTINUE, 0);
    if (InitDataServices (&is_network)) {
      rsult = TRUE;
    }
  }
  threads--;
  ErrSetOpts (ERR_ABORT, 0);
  ErrShow ();
  return rsult;
}

static void GeneralTimerProc (void)

{
  AvailTimerProc ();
  FetchTimerProc ();
}

static void SetupTermMenus (WindoW w)

{
  MenU  m;

#ifdef WIN_MAC
  m = AppleMenu (NULL);
  CommandItem (m, "About Entrez...", AboutProc);
  SeparatorItem (m);
  DeskAccGroup (m);
#endif
  m = PulldownMenu (w, "File");
#ifdef WIN_MSWIN
  CommandItem (m, "About Entrez...", AboutProc);
  SeparatorItem (m);
#endif
#ifdef WIN_MOTIF
  CommandItem (m, "About Entrez...", AboutProc);
  SeparatorItem (m);
#endif
  LoadOpenMedAndSeqFiles (m);
  LoadOpenName (m);
#ifdef WIN_MAC
  LoadSaveName (m);
#endif
  /*
  LoadOpenTerm (m);
  LoadOpenUids (m);
  */
  SeparatorItem (m);
#ifdef WIN_MAC
  LoadSaveItem (m);
  SeparatorItem (m);
  LoadPrintItem (m);
  SeparatorItem (m);
#endif
  CommandItem (m, "Quit/Q", QuitProc);
#ifdef WIN_MAC
  m = PulldownMenu (w, "Options");
  LoadAbstractOptions (m);
  SeparatorItem (m);
  LoadReportOptions (m);
  SeparatorItem (m);
  LoadReportClass (m);
  if (showFeat) {
    m = PulldownMenu (w, "Features");
    LoadFeatOptions (m);
  }
#endif
  m = PulldownMenu (w, "Preferences");
  LoadParentsPersistPrefs (m);
  LoadSequencePrefs (m);
  LoadScorePrefs (m);
  LoadTimerPrefs (m);
  LoadResiduesPrefs (m);
  LoadSettingsPersistPrefs (m);
#ifdef WIN_MAC
  if (smallScreen) {
    m = PulldownMenu (w, "Windows");
    LoadGenerationItems (m);
  }
#endif
}

static void LoadCloseItem (MenU m)

{
  closeItem = CommandItem (m, "Close/W", HideBrowseMenuProc);
}

static void SetupBrowseMenus (WindoW w)

{
  MenU  m;

  m = PulldownMenu (w, "File");
#ifdef WIN_MSWIN
  LoadCloseItem (m);
  SeparatorItem (m);
  LoadSaveName (m);
  SeparatorItem (m);
#endif
#ifdef WIN_MOTIF
  LoadCloseItem (m);
  SeparatorItem (m);
  LoadSaveName (m);
  SeparatorItem (m);
#endif
  LoadSaveItem (m);
  m = PulldownMenu (w, "Options");
  LoadAbstractOptions (m);
  SeparatorItem (m);
  LoadReportOptions (m);
  SeparatorItem (m);
  LoadReportClass (m);
  if (showFeat) {
    m = PulldownMenu (w, "Features");
    LoadFeatOptions (m);
  }
}

static void SetupAboutWindow (void)

{
  ButtoN   b;
  Int2     delta;
  GrouP    g;
  CharPtr  info;
  PaneL    p;
  RecT     r;
  RecT     s;

  aboutWindow = FixedWindow (-50, -33, -1, -1, "About Entrez", HideWindowProc);
  p = SimplePanel (aboutWindow, 28 * stdCharWidth, 14 * stdLineHeight, DrawAbout);
  SetPanelClick (p, NULL, NULL, NULL, HideAboutPanelProc);
  g = HiddenGroup (aboutWindow, 2, 0, NULL);
  SetGroupSpacing (g, 20, 2);
  SetGroupMargins (g, 5, 5);
  PushButton (g, "Done", AboutDoneProc);
  info = EntrezDetailedInfo ();
  if (info != NULL) {
    PushButton (g, "More", AboutMoreProc);
  }
  GetPosition (p, &s);
  GetPosition (g, &r);
  delta = s.right - r.right;
  OffsetRect (&r, delta/2, 0);
  SetPosition (g, &r);
  RealizeWindow (aboutWindow);
  detailedWindow = FixedWindow (-50, -50, -10, -10, "Detailed Information", HideWindowProc);
  detailedDoc = DocumentPanel (detailedWindow, 40 * stdCharWidth, 10 * stdLineHeight);
  b = PushButton (detailedWindow, "Done", AboutDoneProc);
  GetPosition (detailedDoc, &s);
  GetPosition (b, &r);
  delta = s.right - r.right;
  OffsetRect (&r, delta/2, 0);
  SetPosition (b, &r);
  RealizeWindow (detailedWindow);
}

extern Int2 Main (void)

{
  WndActnProc    cls;
  EntrezInfoPtr  eip;
  PaneL          p;
  RecT           r;
  Char           str [32];
  WindoW         w;

  if (! GetAppParam ("NCBI", "NCBI", NULL, "", str, sizeof (str) - 1)) {
    Message (MSG_FATAL, "Entrez cannot find ncbi configuration file");
    return 0;
  }
  WatchCursor ();
  ClearDocumentLists ();
  smallScreen = FALSE;
  threads = 0;
  printItem = NULL;
  entrezVersion [0] = '\0';
#ifdef WIN_MAC
  if (screenRect.right < 513 || screenRect.bottom < 405) {
    smallScreen = TRUE;
  }
#endif
#ifdef WIN_MSWIN
  if (screenRect.bottom < 352) {
    smallScreen = TRUE;
  }
#endif
#ifdef WIN_MOTIF
  if (screenRect.right < 535 || screenRect.bottom < 475) {
    smallScreen = TRUE;
  }
#endif
  showFeat = FALSE;
  if (GetAppParam ("ENTREZ", "PREFERENCES", "FEATEXTRACT", "", str, sizeof (str) - 1)) {
    if (StringICmp (str, "TRUE") == 0) {
      showFeat = TRUE;
    }
  }
  showEmbl = FALSE;
  if (GetAppParam ("ENTREZ", "PREFERENCES", "SHOWEMBL", "", str, sizeof (str) - 1)) {
    if (StringICmp (str, "TRUE") == 0) {
      showEmbl = TRUE;
    }
  }
  newReport = FALSE;
  if (GetAppParam ("ENTREZ", "PREFERENCES", "NEWREPORT", "", str, sizeof (str) - 1)) {
    if (StringICmp (str, "TRUE") == 0) {
      newReport = TRUE;
    }
  }
#ifdef WIN_MAC
  fnt1 = GetFont ("Geneva", 18, TRUE, TRUE, FALSE, "");
  fnt2 = GetFont ("Symbol", 12, FALSE, FALSE, FALSE, "Swiss");
#endif
#ifdef WIN_MSWIN
  fnt1 = GetFont ("Arial", 24, TRUE, TRUE, FALSE, "");
  fnt2 = GetFont ("Symbol", 14, FALSE, FALSE, FALSE, "Swiss");
#endif
#ifdef WIN_MOTIF
  fnt1 = GetFont ("Courier", 24, TRUE, TRUE, FALSE, "");
  fnt2 = GetFont ("Symbol", 12, FALSE, FALSE, FALSE, "Swiss");
#endif
  w = FixedWindow (-50, -33, -1, -1, "Finding Entrez Data", NULL);
  p = SimplePanel (w, 28 * stdCharWidth, 14 * stdLineHeight, DrawAbout);
  Show (w);
  if (SetupCD ()) {
    eip = EntrezGetInfo ();
    if (eip != NULL) {
      sprintf (entrezVersion, "Entrez Data Release %d.%d", (int) eip->version, (int) eip->issue);
      Select (p);
      ObjectRect (p, &r);
      InsetRect (&r, 4, 4);
      r.top += 20;
      SelectFont (fnt1);
      r.top += LineHeight ();
      SelectFont (programFont);
      r.top += LineHeight ();
      r.bottom = r.top + LineHeight ();
      InsetRect (&r, -1, -1);
      InvalRect (&r);
      SelectFont (systemFont);
      Update ();
    }
    SetupAboutWindow ();
    SetTitle (w, "Loading Parse Tables");
    ErrSetOpts (ERR_CONTINUE, 0);
    if (AllObjLoad () && SeqCodeSetLoad ()) {
      if (newReport) {
        if (! PrintTemplateSetLoad ("objprt.prt")) {
          Message (MSG_OK, "Unable to load print template");
          newReport = FALSE;
        }
      }
      ErrSetOpts (ERR_ABORT, 0);
      ErrShow ();
      SetTitle (w, "Creating Menus");
#ifdef WIN_MAC
      SetupTermMenus (NULL);
#endif
      cls = CloseTermWindowProc;
#ifdef WIN_MAC
      cls = NULL;
#endif
      if (smallScreen) {
        termWindow = FixedWindow (-50, -33, -10, -10, "Term Selection", cls);
      } else {
        termWindow = FixedWindow (-50, -33, -10, -10, "Query", cls);
      }
      chosenWindow = termWindow;
#ifndef WIN_MAC
      SetupTermMenus (termWindow);
#endif
      if (smallScreen) {
        SetTitle (w, "Creating Selection Window");
      } else {
        SetTitle (w, "Creating Term Window");
      }
      CreateAvailList (termWindow);
      Break (termWindow);
      if (smallScreen) {
        RealizeWindow (termWindow);
        SetTitle (w, "Creating Refinement Window");
        chosenWindow = FixedWindow (-90, -90, -10, -10, "Query Refinement", NULL);
      } else {
        SetTitle (w, "Creating Query Window");
      }
      CreateChosenList (chosenWindow);
      SetTitle (w, "Creating Document Window");
      browseWindow = FixedWindow (-10, -90, -10, -10, "Document", HideBrowseWindowProc);
#ifndef WIN_MAC
      SetupBrowseMenus (browseWindow);
#endif
      CreateFetchedList (browseWindow);
      RealizeWindow (browseWindow);
      SetTitle (w, "Loading Fonts");
      SetAbstractFonts ();
      Remove (w);
      Show (termWindow);
      if (smallScreen) {
        Show (chosenWindow);
      }
      Select (termWindow);
      Select (term);
      Metronome (GeneralTimerProc);
      ArrowCursor ();
      ProcessEvents ();
      ClearAllLists ();
      ClearHistoryList ();
      ClearDocumentLists ();
      threads++;
      EntrezFini ();
      threads--;
    } else {
      ErrSetOpts (ERR_ABORT, 0);
      ErrShow ();
      Remove (w);
      ArrowCursor ();
      threads++;
      EntrezFini ();
      threads--;
      SetupCDFailed ();
    }
  } else {
    Remove (w);
    ArrowCursor ();
    SetupCDFailed ();
  }
  ArrowCursor ();
  return 0;
}
