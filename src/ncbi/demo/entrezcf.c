/* 
 * ===========================================================================
 *
 *                             COPYRIGHT NOTICE
 *               National Center for Biotechnology Information
 *
 *  This software/database is a "United States Government Work" under the
 *  terms of the United States Copyright Act.  It was written as part of
 *  the author's official duties as a Government employee and thus cannot
 *  be copyrighted.  This software/database is freely available to the
 *  public for use without a copyright notice.  Restrictions cannot be
 *  placed on its present or future use.
 *
 *  Although all reasonable efforts have been taken to ensure the accuracy
 *  and reliability of the software and data, the National Library of
 *  Medicine (NLM) and the U. S. Government do not and cannot warrant the
 *  performance or results that may be obtained by using this software or
 *  data.  The NLM and the U. S. Government disclaim all warranties as to
 *  performance, merchantability or fitness for any particular purpose.
 *
 *  Please see that the author is suitably cited in any work or product
 *  based on this material.
 *
 * ===========================================================================
 *
 * File Name:  entrezcf.c
 * Author:   Jonathan Epstein, Jonathan Kans
 * Version Creation Date:   1-93
 * $Revision: 1.65 $
 *
 * File Description: 
 *  Standalone configuration program used to configure the complex
 *  configuration mechanism for the Entrez application and the data access
 *  library for the Molecular Sequence Data CD-ROM library produced by the NCBI.
 *
 *  This code was written for:
 *
 *      Release 6
 *
 * ==========================================================================
 */

#include <vibrant.h>
#include <panels.h>
#include <document.h>
#include <accentr.h>
#include <cdromlib.h>
#include <cdconfig.h>
#ifdef NETWORK_SUPPORTED
#ifdef WIN_MSWIN
/* undef some things from Vibrant that
   conflict with windows.h  */
#undef HANDLE
#undef EqualRgn
#undef FrameRgn
#undef InvertRgn
#undef LineTo
#undef PaintRgn
#undef StartPage
#undef EndPage
#undef DrawText
#undef FrameRect
#undef InvertRect
#undef UnionRect
#undef OffsetRect
#undef EqualRect
#undef PtInRect
#undef MoveTo
#endif /* WIN_MSWIN */
#include <ncbinet.h>
#include <ni_net.h>
#include <netentr.h>
#endif

#ifdef VAR_ARGS
#include <varargs.h>
#else
#include <stdarg.h>
#endif

#ifdef NETP_INET_NEWT
#define SIN_ADDR        sin_addr.S_un.S_addr
#else
#define SIN_ADDR        sin_addr
#endif


#ifdef CONFIGURATION_EDITING_SUPPORTED
/******************** The following could constitute edparam.h ****************/
Int2 EdGetAppParam PROTO((CharPtr file, CharPtr section, CharPtr type, CharPtr dflt, CharPtr buf, Int2 buflen));
Boolean EdSetAppParam PROTO((CharPtr file, CharPtr section, CharPtr type, CharPtr value));
void EdSetReallyWrite PROTO((Boolean reallyWrite));
Boolean EdGetReallyWrite PROTO((void));
Boolean EdConfigFile PROTO((CharPtr filename, CharPtr quitName, Int2 width, Int2 height, Boolean assumeDirty, VoidProc configWriteFun, VoidProc callerQuitProc));
/******************** end edparam.h ****************/
#else
#define EdSetAppParam SetAppParam
#endif /* CONFIGURATION_EDITING_SUPPORTED */

#ifdef OS_MAC
#define ASNLOAD_NEEDED 1
#endif
#ifdef OS_DOS
#define ASNLOAD_NEEDED 1
#endif

#define DEBUG(x) Message (MSG_POST, "%s", x);

#define CFG_FILE_NAME "NCBI"

#define SEQ_VOLUME_LABEL "SeqData"
#define REF_VOLUME_LABEL "RefData"
#define MED_VOLUME_LABEL "MedData"
#define SEQ_CD_DISK_LABEL "SEQDATA:"
#define REF_CD_DISK_LABEL "REFDATA:"
#define SEQ_FORMAL_NAME "Entrez: Sequences CD-ROM"
#define REF_FORMAL_NAME "Entrez: References CD-ROM"

#define COPY_BUF_SIZE  256
#define COPY_VBUF_SIZE 16384
#define MONITOR_UPDATE 16
#define IDX_BYTES_TO_COMPARE 128

static void          EntSeqWindowProc    PROTO((void));
static void          EntRefWindowProc    PROTO((void));
static void          EntNetWindowProc    PROTO((void));
static void          QuitButtonProc      PROTO((ButtoN b));
static AsnIoPtr      MyEntrezInfoAsnOpen PROTO((CharPtr dirname));

static FonT         fnt1 = NULL;
static FonT         fnt2 = NULL;

static WindoW       mainWindow = NULL;
static WindoW       pathWindow = NULL;
static WindoW       sourceWindow = NULL;
static WindoW       entrezWindow = NULL;

static GrouP        sourceGroup = NULL;
static GrouP        esMediaGroup = NULL;
static GrouP        erMediaGroup = NULL;
static GrouP        numCdsGroup = NULL;
static GrouP        idxCopyGroup = NULL;
static GrouP        cookedGroup = NULL;
static GrouP        rawGroup = NULL;

static ButtoN       useESButton = NULL;
static ButtoN       useERButton = NULL;
static ButtoN       useNETButton = NULL;
static ButtoN       seqIdxButton = NULL;
static ButtoN       refIdxButton = NULL;
static ButtoN       swapButton = NULL;
static ButtoN       acceptMediaButton = NULL;
static ButtoN       acceptPathButton = NULL;
static ButtoN       acceptEntrezButton = NULL;

static MenU         fileMenu = NULL;
static IteM         quitItem = NULL;
static TexT         mainPathText = NULL;

static TexT         rootText = NULL;
static TexT         mountText = NULL;
static TexT         cookedText = NULL;
static TexT         rawText = NULL;

static PrompT       rootPrompt = NULL;

static CharPtr      mainPath = NULL;

static CharPtr      plainRootPath = NULL;
static CharPtr      asnloadPath = NULL;
static CharPtr      dataPath = NULL;

static CharPtr      seqRootPath = NULL;
static CharPtr      seqMountPath = NULL;
static CharPtr      seqCookedPath = NULL;
static CharPtr      seqRawPath = NULL;
static CharPtr      seqValPath = NULL;
static CharPtr      seqIdxPath = NULL;

static CharPtr      refRootPath = NULL;
static CharPtr      refMountPath = NULL;
static CharPtr      refCookedPath = NULL;
static CharPtr      refRawPath = NULL;
static CharPtr      refValPath = NULL;
static CharPtr      refIdxPath = NULL;

static CharPtr      oldSeqRootPath = NULL;
static CharPtr      oldRefRootPath = NULL;
static Boolean      oldSeqOnCD = FALSE;
static Boolean      oldRefOnCD = FALSE;
static Boolean      oldSeqMediaSet= FALSE;
static Boolean      oldRefMediaSet= FALSE;

static Boolean      useES = FALSE;
static Boolean      useER = FALSE;
static Boolean      useNET = FALSE;

static Boolean      seqOnCD = TRUE;
static Boolean      refOnCD = TRUE;
static Boolean      seqOnHD = FALSE;
static Boolean      refOnHD = FALSE;
static Boolean      seqOnFS = FALSE;
static Boolean      refOnFS = FALSE;

static Boolean      copyVal = FALSE;
static Boolean      copySeqIdx = FALSE;
static Boolean      copyRefIdx = FALSE;

static Boolean      esMediaSet = FALSE;
static Boolean      erMediaSet = FALSE;
static Boolean      numCdsSet = FALSE;

static Boolean      nowGettingES = FALSE;
static Boolean      nowGettingER = FALSE;

static Boolean      noSwapping = TRUE;
static Boolean      allowSwapping = TRUE;

static Int2         numCds = 0;

static Boolean      askBeforeQuitting = FALSE;
static Boolean      showPrompts = TRUE;

static Int2         lastVersion = -1;
static Int2         seqVersion = -1;
static Int2         refVersion = -1;

static Boolean      bAppendVer = FALSE;
static Boolean      upperCaseIt = FALSE;

char *sectionsToDelete[] = {
  ENTR_REF_CHAN,
  ENTR_SEQ_CHAN,
  ENTR_LINKS_CHAN,
  "ENTREZ",
  "SEQUENCE",
  "MEDLINE",
  "NCBI",
  "ENTREZ",
  "ENTREZ_SEQ_CD_DESC",
  "ENTREZ_SEQ_CD",
  "LINKS_FROM_SEQUENCE",
  "ENTREZ_SEQ_HD_DESC",
  "ENTREZ_SEQ_HD",
  "LINKS_FROM_REFERENCE",
  "ENTREZ_REF_CD_DESC",
  "REFERENCE_FROM_ENTREZ_SEQ_CD",
  "ENTREZ_REF_CD",
  "ENTREZ_REF_HD_DESC",
  "REFERENCE_FROM_ENTREZ_SEQ_HD",
  "ENTREZ_REF_HD",
  "LINKS_FROM_MEDLINE",
  "ENTREZ_MED_CD_DESC",
  "MEDLINE_FROM_ENTREZ_SEQ_CD",
  "ENTREZ_MED_CD",
  "ENTREZ_MED_HD_DESC",
  "MEDLINE_FROM_ENTREZ_SEQ_HD",
  "ENTREZ_MED_HD",
#ifdef NETWORK_SUPPORTED
  "NET_SERV",
  "ENTREZ_NET",
  "MEDLINE_NET",
  "REFERENCE_NET",
  "SEQUENCE_NET",
#endif /* NETWORK_SUPPORTED */
};

/* sections which should _not_ be deleted, but which should be checked for */
/* existence                                                               */
char *sectionsToRead[] = {
#ifndef NETWORK_SUPPORTED
  "NET_SERV",
  "ENTREZ_NET",
  "MEDLINE_NET",
  "SEQUENCE_NET",
#endif /* NETWORK_SUPPORTED */
  "REFERENCE_NET",
};

#ifdef NETWORK_SUPPORTED
#define DISPATCHER   DispNumerics[0]
#define DEF_SERVICE  "Entrez"
#define DEF_RESOURCE "Entrez"
#define SERVICE_TYPE "Entrez"
#define TEXT_SIZE    20
#define IOSTR_SIZE   255

static Nlm_TexT WidDialogText PROTO((Nlm_GrouP, Nlm_CharPtr, Nlm_CharPtr, Nlm_Int2, Nlm_TxtActnProc, Nlm_Int2));
static void ReadProc PROTO((TexT));
static void WriteProc PROTO((TexT));
static void SvcReadProc PROTO((TexT));
static void AcceptNetProc PROTO((ButtoN dButton));
static void ShowCatalog PROTO((NICatalogPtr));
static void catalogAction PROTO((TablE, Int2, Int2));
static void resourceAction PROTO((TablE, Int2, Int2));
static void DismissCatalogProc PROTO((ButtoN));
static void MakeCatWindow PROTO((void));
static void SvcCatalogProc PROTO((ButtoN));
static void DispReadProc PROTO((TexT));
static void DispAcceptProc PROTO((ButtoN));
static void DispCancelProc PROTO((ButtoN));
static void SvcAcceptProc PROTO((ButtoN));
static void SvcCancelProc PROTO((ButtoN));
static void SvcSelectProc PROTO((ButtoN));
static void SendProc PROTO((ButtoN));
static void SvcConnectProc PROTO((ButtoN));
static void DispSelectProc PROTO((ButtoN));
static void DispConnectProc PROTO((ButtoN));
static void DispDisconnectProc PROTO((ButtoN));
static void EndProc PROTO((void));
static void FinalProc PROTO((void));

static WindoW   ioWindow = NULL;
static GrouP    dispGroup = NULL;
static GrouP    svcGroup = NULL;

static ButtoN   serviceButton;
static ButtoN   sendButton;
static ButtoN   svcConnButton;
static ButtoN   dispatchButton;
static ButtoN   dispConnButton;
static ButtoN   dispDisconnButton;
static ButtoN   quitNetButton;
static ButtoN   acceptNetButton;

static WindoW   catalogWindow;
static SlatE    catSlate;
static TablE    catTable;
static SlatE    resSlate;
static TablE    resTable;
static SlatE    dispSlate;
static TablE    dispTable;
static ButtoN   dismissButton;
static Int2     currentCatRow;
static Int2     currentResRow;
static Int2     currentDispRow;
static Int2     currentDispCol;

static WindoW   dispWindow;
static ButtoN   dispAcceptButton;
static ButtoN   dispCancelButton;
static TexT     dispName;
static TexT     userName;
static TexT     groupName;
static TexT     passWord;

static WindoW   svcWindow;
static GrouP    svcSvcGroup;
static GrouP    svcResGroup;
static ButtoN   catalogButton;
static ButtoN   svcAcceptButton;
static ButtoN   svcCancelButton;
static TexT     service;
static TexT     resource;
static TexT     smin, smax, rmin, rmax;
static Char     svc[64], res[64], tsvmin[8], tsvmax[8], trsmin[8], trsmax[8];
static Char     tsvc[64], tres[64];
static Char     dispatcher_name[64];
static Char     user_name[64];
static Char     host_address[64];
static Char     group_name[64];
static Char     pass_word[64];
static Char     encr_desired[6];
static Char     disp_reconn_action[5];
static Char     dname[64], uname[64], pword[64], gname[64];
static Int2     svmin, svmax, rsmin, rsmax;
static NICatalogPtr catalog;
static NI_DispInfoPtr dispInfoPtr = NULL;
static CharPtr  DispFQDNs[] = {"dispatch1.nlm.nih.gov", "dispatch2.nlm.nih.gov",
                               "dispatch3.nlm.nih.gov"};
static CharPtr  DispNumerics[] = {"130.14.20.80", "130.14.20.47", "130.14.25.1"};

Boolean         dispConnected = FALSE;
Boolean         svcConnected = FALSE;
Boolean         svcSpecified = FALSE;

NI_HandPtr      shSvc;
NI_DispatcherPtr dispatcher;



static          Nlm_TexT
WidDialogText (Nlm_GrouP prnt, Nlm_CharPtr prompt, Nlm_CharPtr dfault, Nlm_Int2 charWidth, Nlm_TxtActnProc actn, Nlm_Int2 promptwid)
{
  GrouP           g;

  g = HiddenGroup (prnt, 2, 0, NULL);
  (void) StaticPrompt (g, prompt, stdCharWidth * promptwid,
                       stdLineHeight, systemFont, 'l');
  return DialogText (g, dfault, charWidth, actn);
}

static void
RefreshNetControls (void)
{
  if (StringLen(user_name) > 0 && StringLen(dispatcher_name) > 0) {
    Enable (acceptNetButton);
    if (! dispConnected)
      Enable (dispConnButton);
  } else {
    Disable (acceptNetButton);
    if (! dispConnected)
      Disable (dispConnButton);
  }
}

static void
SvcReadProc (TexT t)
{
  GetTitle (service, tsvc, sizeof(tsvc));
  GetTitle (smin, tsvmin, sizeof(tsvmin));
  GetTitle (smax, tsvmax, sizeof(tsvmax));
  GetTitle (resource, tres, sizeof(tres));
  GetTitle (rmin, trsmin, sizeof(trsmin));
  GetTitle (rmax, trsmax, sizeof(trsmax));

  if (StringLen (tsvc) > 0 && StringLen (tsvmin) > 0 && StringLen (tsvmax) > 0
    && StringLen (tres) > 0 && StringLen (trsmin) > 0 && StringLen (trsmax) > 0)
  {
    Enable (svcAcceptButton);
  } else
  {
    Disable (svcAcceptButton);
  }
}                               /* SvcReadProc */

static void
ShowCatalog (NICatalogPtr cat)
{
  Char            buf[512];
  NIToolsetPtr    tsp;
  NISvcPtr        svcp;
  NodePtr         tsnp, np;

  Char            nm[64], desc[255];

  SetTitle (catalogWindow, cat->motd);

  for (tsnp = cat->toolsetL; tsnp != NULL; tsnp = tsnp->next)
  {
    if ((tsp = (NIToolsetPtr) tsnp->elem) != NULL)
    {
      for (np = tsp->services; np != NULL; np = np->next)
      {
        svcp = (NISvcPtr) np->elem;
        if (svcp->typeL == NULL ||
            StrCmp (svcp->typeL->elem, SERVICE_TYPE) != 0)
          continue;
        StringCpy (nm, svcp->name);
        StringCpy (desc, svcp->descrip);
        sprintf (buf, "%s\t%d\t%d\t%s\t%ld\n", nm, svcp->minVersion, svcp->maxVersion, desc, (long) tsp);
        AppendTableText (catTable, buf);
      }
    }
  }

  Show (catalogWindow);
  Select (catalogWindow);
}                               /* ShowCatalog */

static void
resourceAction (TablE t, Int2 row, Int2 col)
{
  if (row != currentResRow)
  {
    if (currentResRow != 0)
    {
      SetTableBlockHilight (resTable, currentResRow, currentResRow, 1, 7, FALSE);
    }
    currentResRow = row;
    SetTableBlockHilight (resTable, currentResRow, currentResRow, 1, 7, TRUE);
  }

  GetTableText (resTable, row, 1, tres, sizeof(tres));
  GetTableText (resTable, row, 2, trsmin, sizeof(trsmin));
  GetTableText (resTable, row, 3, trsmax, sizeof(trsmax));
  SetTitle (resource, tres);
  SetTitle(rmin, trsmin);
  SetTitle (rmax, trsmax);
  Enable (svcAcceptButton);
  Hide (catalogWindow);
  NI_DestroyMsgCatalog(catalog);
  catalog = NULL;
}

static void
RecreateResTable (void)
{
  RecT            r;
  WindoW          tPort;

  RecordColumn (resTable, 13, 'l', FALSE, FALSE, NULL);
  RecordColumn (resTable, 3, 'r', FALSE, TRUE, NULL);
  RecordColumn (resTable, 3, 'r', FALSE, TRUE, NULL);
  RecordColumn (resTable, 21, 'l', FALSE, TRUE, NULL);
  currentResRow = 0;
  ObjectRect (resSlate, &r);
  InsetRect (&r, 4, 4);
  RegisterRect ((PaneL) resTable, &r);
  if (Visible (resTable) && AllParentsVisible (resTable))
  {
    tPort = SavePort (resTable);
    Select (resTable);
    InvalRect (&r);
    RestorePort (tPort);
  }
}

static void
catalogAction (TablE t, Int2 row, Int2 col)
{
  char            temp[20];
  NIToolsetPtr    tsp;
  NIResPtr        resp;
  NodePtr         np;
  long            longval;
  Int4            int4val;
  Char            buf[512];

  if (row != currentCatRow)
  {
    if (currentCatRow != 0)
    {
      SetTableBlockHilight (catTable, currentCatRow, currentCatRow, 1, 7, FALSE);
    }
    currentCatRow = row;
    SetTableBlockHilight (catTable, currentCatRow, currentCatRow, 1, 7, TRUE);
  }

  GetTableText (catTable, row, 1, tsvc, sizeof(tsvc));
  GetTableText (catTable, row, 2, tsvmin, sizeof(tsvmin));
  GetTableText (catTable, row, 3, tsvmax, sizeof(tsvmax));
  GetTableText (catTable, row, 5, temp, sizeof(temp));
  if (sizeof(long) > 4) {
    if (sscanf(temp, "%ld", &longval) > 0)
    {
      tsp = (NIToolsetPtr) longval;
    } else
    {
      tsp = NULL;
    }
  } else {
    if (StrToLong (temp, &int4val))
    {
      tsp = (NIToolsetPtr) int4val;
    } else
    {
      tsp = NULL;
    }
  }

  Reset (resSlate);
  RecreateResTable ();

  if (tsp != NULL)
  {
    for (np = tsp->resources; np != NULL; np = np->next)
    {
      resp = (NIResPtr) np->elem;
      if (resp->type == NULL || StrCmp (resp->type, SERVICE_TYPE) != 0)
        continue;
      sprintf (buf, "%s\t%d\t%d\t%s\n", resp->name, resp->minVersion, resp->maxVersion, resp->descrip);
      AppendTableText (resTable, buf);
    }
  }
  SetTitle (service, tsvc);
  SetTitle (smin, tsvmin);
  SetTitle (smax, tsvmax);
  GetTitle (resource, tres, sizeof(tres));
  GetTitle (rmin, trsmin, sizeof(trsmin));
  GetTitle (rmax, trsmax, sizeof(trsmax));
}                               /* catalogAction */

static void
DismissCatalogProc (ButtoN dcButton)
{
/*   Hide (catalogWindow);*/
  SvcReadProc (NULL);            /* refresh Accept control */
  Remove (catalogWindow);
  NI_DestroyMsgCatalog(catalog);
  catalog = NULL;
}                               /* DismissCatalogProc */

static void
MakeCatWindow (void)
{
  GrouP           pg, g1, g2;
  PrompT          p;
  RecT            r;
  WindoW          tPort;

  catalogWindow = FixedWindow (-50, -33, -10, -10, "Catalog", NULL);
  SetGroupSpacing (catalogWindow, 20, 20);

  g1 = HiddenGroup (catalogWindow, 0, 10, NULL);
  pg = HiddenGroup (g1, 10, 0, NULL);
  SetGroupMargins (pg, 1, 1);
  SetGroupSpacing (pg, 1, 1);
  p = StaticPrompt (pg, "Service Name", stdCharWidth * 13, stdLineHeight, systemFont, 'l');
  p = StaticPrompt (pg, "Min", stdCharWidth * 3, stdLineHeight, systemFont, 'l');
  p = StaticPrompt (pg, "Max", stdCharWidth * 3, stdLineHeight, systemFont, 'l');
  p = StaticPrompt (pg, "Description", stdCharWidth * 21, stdLineHeight, systemFont, 'l');

  catSlate = ScrollSlate (g1, 40, 6);
  catTable = TablePanel (catSlate, 0, systemFont, catalogAction);
  RecordColumn (catTable, 13, 'l', FALSE, FALSE, NULL);
  RecordColumn (catTable, 3, 'r', FALSE, TRUE, NULL);
  RecordColumn (catTable, 3, 'r', FALSE, TRUE, NULL);
  RecordColumn (catTable, 21, 'l', FALSE, TRUE, NULL);
  RecordColumn (catTable, 9, 'l', FALSE, FALSE, NULL);   /* address */
  currentCatRow = 0;
  ObjectRect (catSlate, &r);
  InsetRect (&r, 4, 4);
  RegisterRect ((PaneL) catTable, &r);
  if (Visible (catTable) && AllParentsVisible (catTable))
  {
    tPort = SavePort (catTable);
    Select (catTable);
    InvalRect (&r);
    RestorePort (tPort);
  }
  g2 = HiddenGroup (catalogWindow, 0, 10, NULL);
  pg = HiddenGroup (g2, 10, 0, NULL);
  SetGroupMargins (pg, 1, 1);
  SetGroupSpacing (pg, 1, 1);
  p = StaticPrompt (pg, "Resource Name", stdCharWidth * 13, stdLineHeight, systemFont, 'l');
  p = StaticPrompt (pg, "Min", stdCharWidth * 3, stdLineHeight, systemFont, 'l');
  p = StaticPrompt (pg, "Max", stdCharWidth * 3, stdLineHeight, systemFont, 'l');
  p = StaticPrompt (pg, "Description", stdCharWidth * 21, stdLineHeight, systemFont, 'l');

  resSlate = ScrollSlate (g2, 40, 6);
  resTable = TablePanel (resSlate, 0, systemFont, resourceAction);
  RecreateResTable ();

  dismissButton = DefaultButton (catalogWindow, "Dismiss", DismissCatalogProc);

  AlignObjects (ALIGN_CENTER, (Nlm_HANDLE) g1, (Nlm_HANDLE) g2,
                (Nlm_HANDLE) dismissButton, NULL);
  RealizeWindow (catalogWindow);

}                               /* MakeCatWindow */

static void
SvcCatalogProc (ButtoN scButton)
{
  if ((catalog = NI_GetCatalog (dispatcher)) != NULL)
  {
    MakeCatWindow ();
    ShowCatalog (catalog);
  } else
    Message (MSG_OK, "Unable to get catalog");
}                               /* SvcCatalogProc */

static void
DispReadProc (TexT t)
{

  GetTitle (dispName, dname, sizeof(dname));
  GetTitle (userName, uname, sizeof(uname));

  if (StringLen (dname) > 0 && StringLen (uname) > 0)
  {
    Enable (dispAcceptButton);
  } else
  {
    Disable (dispAcceptButton);
  }
}

static void
DispAcceptProc (ButtoN scButton)
{

  StringCpy (dispatcher_name, dname);
  StringCpy (user_name, uname);
  RefreshNetControls ();
  SetTitle (acceptNetButton, "Accept");
  Hide (dispWindow);
}

static void
DispCancelProc (ButtoN scButton)
{
  Hide (dispWindow);
  SetTitle (dispName, dispatcher_name);
  SetTitle (userName, user_name);
  RefreshNetControls ();
}

static void
SvcAcceptProc (ButtoN scButton)
{
  svmin = atoi (tsvmin);
  svmax = atoi (tsvmax);
  rsmin = atoi (trsmin);
  rsmax = atoi (trsmax);
  StrCpy (svc, tsvc);
  StrCpy (res, tres);
  Hide (svcWindow);
  Enable (serviceButton);
  svcSpecified = TRUE;
  SetTitle (acceptNetButton, "Accept");
}                               /* SvcAcceptProc */

static void
SvcCancelProc (ButtoN scButton)
{
  StrCpy (tsvc, svc);
  StrCpy (tres, res);
  sprintf (tsvmin, "%d", svmin);
  sprintf (tsvmax, "%d", svmax);
  sprintf (trsmin, "%d", rsmin);
  sprintf (trsmax, "%d", rsmax);
  SetTitle (service, tsvc);
  SetTitle (smin, tsvmin);
  SetTitle (smax, tsvmax);
  SetTitle (resource, tres);
  SetTitle (rmin, trsmin);
  SetTitle (rmax, trsmax);
  Hide (svcWindow);
  Enable (serviceButton);
}                               /* SvcCancelProc */

static void
SvcSelectProc (ButtoN scButton)
{
  Disable (serviceButton);
  SvcReadProc (NULL);            /* refresh Accept control */
  if (dispConnected)
    Enable (catalogButton);
  else
    Disable (catalogButton);
  Show (svcWindow);
  Select (service);
  Select (svcWindow);
}                               /* SvcSelectProc */


static void
SvcConnectProc (ButtoN cButton)
{
  if (!svcSpecified)
  {
    Message (MSG_OK, "No service has been specified");
    return;
  }
  WatchCursor ();
  /* must disconnect as anonymous and reconnect as GUEST */
  NI_EndServices (dispatcher);
  dispatcher = NI_SetDispatcher (NULL, dispatcher_name, NULL, 0, 0, NULL);
  if (NI_InitServices (dispatcher, user_name, "GUEST", pass_word, &dispInfoPtr) < 0)
  {
    ArrowCursor ();
    ErrShow ();
    DispDisconnectProc (NULL);
    return;
  }
  WatchCursor ();
  if ((shSvc = NI_ServiceGet (dispatcher, svc, svmin, svmax, res, SERVICE_TYPE, rsmin, rsmax)) == NULL)
  {
    ArrowCursor ();
    Message (MSG_OK, "Unable to get service [%s]: %s", svc, ni_errlist[ni_errno]);
    return;
  } else
  {
    NI_ServiceDisconnect (shSvc);
    ArrowCursor ();
    svcConnected = FALSE;
    if (Message (MSG_YN, "Network Entrez service connection was successful!\nDo you wish to accept this network configuration?") == ANS_YES)
    {
      Hide (ioWindow);
      AcceptNetProc (NULL);
    }
  }
}                               /* SvcConnectProc */

static void
DispSelectProc (ButtoN dButton)
{
  DispReadProc (NULL);           /* reset controls */
  Show (dispWindow);
  Select (dispName);
  Select (dispWindow);
}                               /* DispSelectProc */

static void
DispConnectProc (ButtoN cButton)
{
  dispatcher = NI_SetDispatcher (NULL, dispatcher_name, NULL, 0, 0, NULL);
  if (NI_InitServices (dispatcher, user_name, group_name[0] == '\0' ? NULL : group_name, pass_word, &dispInfoPtr) < 0)
  {
    Message (MSG_OK, "Unable to connect to dispatcher");
    return;
  } else {
    dispConnected = TRUE;
  }

  Disable (dispConnButton);
  Enable (dispDisconnButton);
  Enable (svcConnButton);
}                               /* DispConnectProc */

static void
DispDisconnectProc (ButtoN dButton)
{
  if (svcConnected)
  {
    NI_ServiceDisconnect (shSvc);
    svcConnected = FALSE;
  }
  if (dispConnected)
  {
    NI_EndServices (dispatcher);
    dispatcher = NULL;
    dispConnected = FALSE;
  }
  Disable (dispDisconnButton);
  Disable (svcConnButton);
  Enable (dispConnButton);
}                               /* DispDisconnectProc */


static void
AcceptNetProc (ButtoN dButton)
{
  FinalProc ();
}

static void
dispAction (TablE t, Int2 row, Int2 col)
{
  if (row != currentDispRow || col != currentDispCol)
  {
    if (currentDispRow != 0)
    {
      SetTableBlockHilight (dispTable, currentDispRow, currentDispRow, currentDispCol, currentDispCol, FALSE);
    }
    currentDispRow = row;
    currentDispCol = col;
    SetTableBlockHilight (dispTable, row, row, col, col, TRUE);
  }

  GetTableText (dispTable, row, col, dname, sizeof(dname));
  SetTitle (dispName, dname);
/*  Update (); */
}

static void
RecreateDispTable (void)
{
  RecT            r;
  WindoW          tPort;

  RecordColumn (dispTable, 18, 'l', FALSE, FALSE, NULL);
  RecordColumn (dispTable, 10, 'l', FALSE, TRUE, NULL);
  currentDispRow = 0;
  ObjectRect (dispSlate, &r);
  InsetRect (&r, 4, 4);
  RegisterRect ((PaneL) dispTable, &r);
  if (Visible (dispTable) && AllParentsVisible (dispTable))
  {
    tPort = SavePort (dispTable);
    Select (dispTable);
    InvalRect (&r);
    RestorePort (tPort);
  }
}

static void
MakeDispTable (GrouP g)
{
  Char            buf[100];
  GrouP           pg;
  PrompT          p;
  Int2            i;
  struct          hostent PNTR dispHost;
  struct sockaddr_in serv_addr;

Int2  num_lines;
num_lines = (Int2) DIM(DispFQDNs);

  pg = HiddenGroup (g, 10, 0, NULL);
  SetGroupMargins (pg, 1, 1);
  SetGroupSpacing (pg, 1, 1);
  p = StaticPrompt (pg, "Fully Qualified Domain Name", stdCharWidth * 18, stdLineHeight, systemFont, 'l');
  p = StaticPrompt (pg, "Dotted IP Address", stdCharWidth * 10, stdLineHeight, systemFont, 'l');

  dispSlate = ScrollSlate (g, 28, MIN(3, (Int2) DIM(DispFQDNs)));
  dispTable = TablePanel (dispSlate, 0, systemFont, dispAction);
  RecreateDispTable ();

  serv_addr.sin_family = AF_INET;
  for (i = 0; i < (Int2) DIM(DispFQDNs); i++)
  {
    if ((dispHost = gethostbyname(DispFQDNs[i])) != NULL)
    {
      MemCopy(&serv_addr.sin_addr, dispHost->h_addr, dispHost->h_length);
      sprintf (buf, "%s\t%s\n", DispFQDNs[i], inet_ntoa(serv_addr.SIN_ADDR));
    }
    else {
      sprintf (buf, "%s\t%s\n", DispFQDNs[i], DispNumerics[i]);
    }
    AppendTableText (dispTable, buf);
  }
}

static void
EncrProc (ButtoN b)
{
  if (GetStatus(b))
  {
      StrCpy (encr_desired, "TRUE");
  } else {
      StrCpy (encr_desired, "FALSE");
  }
}

static void
ChangeDispReconnAction (GrouP g)
{
    switch (GetValue(g)) {
    case 2:
        StrCpy (disp_reconn_action, "ASK");
        break;
    case 3:
        StrCpy (disp_reconn_action, "QUIT");
        break;
    default:
        StrCpy (disp_reconn_action, "CONT");
        break;
    }
}



#endif /* NETWORK_SUPPORTED */

static void
EndProc (void)
{
#ifdef NETWORK_SUPPORTED
   if (svcConnected)
      NI_ServiceDisconnect (shSvc);
   if (dispConnected)
      NI_EndServices (dispatcher);
#endif /* NETWORK_SUPPORTED */
   QuitProgram ();            
} /* EndProc */

static void EntNetWindowProc (void)
{
#ifdef NETWORK_SUPPORTED
  GrouP g, gs, gs2;
  GrouP svcButtonGroup;
  PopuP dispRecList;
  ButtoN encrButton;
#ifdef OS_UNIX
  char  *getlogin PROTO((void));
#endif

  /* service window */

  svcWindow = FixedWindow (-60, -40, -10, -10, "Service Selection", NULL);

  gs = HiddenGroup (svcWindow, 0, 3, NULL);
  SetGroupMargins (gs, 10, 10);
  SetGroupSpacing (gs, 10, 10);

  svcSvcGroup = NormalGroup (gs, 0, 10, "Service", NULL, NULL);
  SetGroupMargins (svcSvcGroup, 10, 10);
  SetGroupSpacing (svcSvcGroup, 10, 10);
  service = WidDialogText (svcSvcGroup, "Name", svc, 8, SvcReadProc, 3);
  Break (svcSvcGroup);
  smin = WidDialogText (svcSvcGroup, "Min", tsvmin, 5, SvcReadProc, 3);
  Advance (svcSvcGroup);
  smax = WidDialogText (svcSvcGroup, "Max", tsvmax, 5, SvcReadProc, 3);

  svcResGroup = NormalGroup (gs, 0, 10, "Resource", NULL, NULL);
  SetGroupMargins (svcResGroup, 10, 10);
  SetGroupSpacing (svcResGroup, 10, 10);
  resource = WidDialogText (svcResGroup, "Name", DEF_RESOURCE, 8, SvcReadProc, 3);

  Break (svcResGroup);
  rmin = WidDialogText (svcResGroup, "Min", trsmin, 5, SvcReadProc, 3);
  Advance (svcResGroup);
  rmax = WidDialogText (svcResGroup, "Max", trsmax, 5, SvcReadProc, 3);

  Break (svcWindow);
  svcButtonGroup = HiddenGroup (svcWindow, 10, 0, NULL);
  catalogButton = PushButton (svcButtonGroup, "Catalog", SvcCatalogProc);
  svcAcceptButton = PushButton (svcButtonGroup, "Accept", SvcAcceptProc);
  svcCancelButton = PushButton (svcButtonGroup, "Cancel", SvcCancelProc);

  AlignObjects (ALIGN_CENTER, (Nlm_HANDLE) svcSvcGroup, (Nlm_HANDLE) svcResGroup,
                (Nlm_HANDLE) svcButtonGroup, NULL);
  RealizeWindow (svcWindow);
  
  /* dispatcher window */
  
  dispWindow = FixedWindow (-30, -20, -10, -10, "Dispatcher Selection", NULL);

  g = HiddenGroup (dispWindow, 0, 4, NULL);
  SetGroupMargins (g, 10, 10);
  SetGroupSpacing (g, 10, 30);

  /* for UNIX and VMS systems (or, for the future, any system where the user */
  /* user name can be determined), use the user's login name as the default  */
#ifdef OS_UNIX
  if (user_name[0] == '\0') {
    StringCpy (user_name, getlogin ());
  }
#endif
#ifdef OS_VMS
  if (user_name[0] == '\0') {
    StringCpy (user_name, getenv ("USER"));
  }
#endif

  StringCpy (group_name, "ANONYMOUS");
  StringCpy (pass_word, "");
  
  gs = HiddenGroup (g, 0, 2, NULL);
  MakeDispTable (gs);
  gs = HiddenGroup (g, 2, 0, NULL);
  StaticPrompt (gs, "Dispatcher Name", 0, 0, systemFont, 'l');
  dispName = DialogText (gs, dispatcher_name, TEXT_SIZE, DispReadProc);
  StaticPrompt (gs, "User Name", 0, 0, systemFont, 'l');
  userName = DialogText (gs, user_name, TEXT_SIZE, DispReadProc);
  
  Break (dispWindow);
  dispAcceptButton = PushButton (dispWindow, "Accept", DispAcceptProc);
  Advance (dispWindow);
  dispCancelButton = PushButton (dispWindow, "Cancel", DispCancelProc);
  RealizeWindow (dispWindow);
 
  /* main window */

  ioWindow = FixedWindow (-50, -33, -10, -10, "Network Entrez Configuration", NULL);

  g = HiddenGroup (ioWindow, 0, 3, NULL);
  SetGroupMargins (g, 10, 10);
  SetGroupSpacing (g, 10, 10);

  dispGroup = NormalGroup (g, 10, 0, "Dispatcher", NULL, NULL);
  SetGroupMargins (dispGroup, 10, 10);
  SetGroupSpacing (dispGroup, 30, 10);
  dispatchButton = PushButton (dispGroup, "Specify", DispSelectProc);
  dispConnButton = PushButton (dispGroup, "Connect", DispConnectProc);
  dispDisconnButton = PushButton (dispGroup, "Disconnect", DispDisconnectProc);
  Disable (dispDisconnButton);

  svcGroup = NormalGroup (g, 10, 0, "Service", NULL, NULL);
  SetGroupMargins (svcGroup, 35, 10);
  SetGroupSpacing (svcGroup, 30, 10);
  serviceButton = PushButton (svcGroup, "Specify", SvcSelectProc);
  svcConnButton = PushButton (svcGroup, "Attempt Service Request", SvcConnectProc);
  Disable (svcConnButton);

  gs = HiddenGroup (ioWindow, 3, 0, NULL);
  SetGroupSpacing (gs, 30, 10);
  quitNetButton = PushButton (gs, "Quit", QuitButtonProc);
  acceptNetButton = PushButton (gs, "Accept Default", AcceptNetProc);
  if (NI_EncrAvailable())
  {
      encrButton = CheckBox(gs, "Encryption", EncrProc);
      SetStatus (encrButton, StrCmp(encr_desired, "TRUE") == 0);
  }

  gs2 = NormalGroup (ioWindow, 3, 0, "When unable to contact primary dispatcher", NULL, ChangeDispReconnAction);
  SetGroupMargins (gs2, 35, 10);
  SetGroupSpacing (gs2, 30, 10);
  RadioButton(gs2, "Try other dispatchers");
  RadioButton(gs2, "Ask user");
  RadioButton(gs2, "Quit");
  SetValue(gs2, StrCmp(disp_reconn_action, "QUIT") == 0 ? 3 :
           (StrCmp(disp_reconn_action, "ASK") == 0 ? 2 : 1));

  AlignObjects (ALIGN_CENTER, (Nlm_HANDLE) g, (Nlm_HANDLE) gs, (Nlm_HANDLE) gs2,
                NULL);
  RealizeWindow (ioWindow);

  RefreshNetControls ();

  Show (ioWindow);
#endif /* NETWORK_SUPPORTED */
}

static void QuitQueryProc (void)

{
  if (askBeforeQuitting) {
    if (Message (MSG_YN, "Are you sure you want to quit?") == ANS_YES) {
      EndProc ();
    }
  } else {
    EndProc ();
  }
}

static void QuitItemProc (IteM i)

{
  QuitQueryProc ();
}

static void QuitButtonProc (ButtoN b)

{
  QuitQueryProc ();
}

static void CloseWindowProc (WindoW w)

{
  Remove (w);
}

static void CloseAboutPanelProc (PaneL p, PoinT pt)

{
  WindoW  w;

  w = ParentWindow (p);
  Remove (w);
}

static void SafeEnable (Handle a)

{
  if (! Enabled (a)) {
    Enable (a);
  }
}

static void SafeDisable (Handle a)

{
  if (Enabled (a)) {
    Disable (a);
  }
}

static void SafeSetValue (Handle a, Int2 value)

{
  Int2 oldval;

  oldval = GetValue (a);
  if (oldval != value) {
    SetValue (a, value);
  }
}

static void SafeSetStatus (Handle a, Boolean status)

{
  Boolean  oldstat;

  oldstat = GetStatus (a);
  if (oldstat != status) {
    SetStatus (a, status);
  }
}

static void LowerNextPosition (WindoW w, Int2 pixels)

{
  PoinT  npt;

  GetNextPosition (w, &npt);
  npt.y += pixels;
  SetNextPosition (w, npt);
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

static void CenterString (RectPtr rptr, CharPtr text, FonT fnt, Int2 inc)

{
  if (fnt != NULL) {
    SelectFont (fnt);
  }
  rptr->bottom = rptr->top + LineHeight ();
  DrawString (rptr, text, 'c', FALSE);
  rptr->top = rptr->bottom + inc;
}

static void DrawAbout (PaneL p)

{
  RecT  r;

  ObjectRect (p, &r);
  InsetRect (&r, 4, 4);
  r.top += 10;
  CenterString (&r, "Entrez Configuration Program", fnt1, 5);
  CenterString (&r, "EntrezCf Application Version 1.9", fnt2, 10);
  CenterString (&r, "National Center for Biotechnology Information", systemFont, 5);
  CenterString (&r, "National Library of Medicine", systemFont, 5);
  CenterString (&r, "National Institutes of Health", systemFont, 10);
  CenterString (&r, "(301) 496-2475", systemFont, 5);
  CenterString (&r, "entrez@ncbi.nlm.nih.gov", systemFont, 0);
}

static void AboutProc (IteM i)

{
  PaneL   p;
  WindoW  w;

  w = ModalWindow (-50, -33, -1, -1, CloseWindowProc);
  p = SimplePanel (w, 30 * stdCharWidth, 12 * stdLineHeight, DrawAbout);
  SetPanelClick (p, NULL, NULL, NULL, CloseAboutPanelProc);
  Show (w);
}

static void PathWindowProc (void);

static void DataPathProc (IteM i)

{
  PathWindowProc ();
  Hide (sourceWindow);
  Select (pathWindow);
}

#ifdef WIN_MAC
static void SetupMenus (WindoW w)

{
  MenU  m;

  m = AppleMenu (NULL);
  CommandItem (m, "About...", AboutProc);
  SeparatorItem (m);
  DeskAccGroup (m);
  fileMenu = PulldownMenu (w, "File");
  CommandItem (fileMenu, "Quit/Q", QuitItemProc);
  m = PulldownMenu (w, "Misc");
  CommandItem (m, "Change Data Path", DataPathProc);
}
#endif

#ifndef WIN_MAC
static void SetupMenus (WindoW w)

{
  MenU  m;

  m = PulldownMenu (w, "File");
  CommandItem (m, "About...", AboutProc);
  SeparatorItem (m);
  quitItem = CommandItem (m, "Quit/Q", QuitItemProc);
  if (w == sourceWindow) {
    m = PulldownMenu (w, "Misc");
    CommandItem (m, "Change Data Path", DataPathProc);
  }
}
#endif

static void CopyOneFile (FILE *outfp, FILE *infp, CharPtr filename, Int4 fileLength)

{
  Char        buf [COPY_BUF_SIZE];
  size_t      bytesRead;
  MonitorPtr  monitor;
  Int4        numReads;
  Int4        readSoFar;
  Char        str [16];
  Char        title [PATH_MAX+30];

  if (outfp != NULL && infp != NULL && fileLength > 0) {
    numReads = 0;
    readSoFar = 0;
    sprintf (str, "%ld", fileLength);
    StringCpy (title, "Copying ");
    StringCat (title, filename);
    StringCat (title, " (");
    StringCat (title, str);
    StringCat (title, " bytes)");
    monitor = MonitorIntNew (title, 0, fileLength);

    /* setvbuf used for faster copying ... not essential */
#ifdef _IOFBF
    setvbuf (infp, NULL, _IOFBF, COPY_VBUF_SIZE);
    setvbuf (outfp, NULL, _IOFBF, COPY_VBUF_SIZE);
#endif

    while ((bytesRead = FileRead (buf, 1, COPY_BUF_SIZE, infp)) > 0) {
      readSoFar += bytesRead;
      if (numReads++ % MONITOR_UPDATE == 0) {
        MonitorIntValue (monitor, readSoFar);
      }
      FileWrite (buf, 1, bytesRead, outfp);
    }
    MonitorFree (monitor);
  }
}

static Boolean CopyValFile (CharPtr volumeLabel, CharPtr rootPath, CharPtr valPath)

{
  Int4  fileLength;
  Char  fileName [32];
  FILE  *infp;
  FILE  *outfp;
  Char  path [PATH_MAX];

  if (rootPath != NULL && valPath != NULL) {
    if (upperCaseIt) {
      StringCpy (fileName, "CDROMDAT.VAL");
    } else {
      StringCpy (fileName, "cdromdat.val");
    }
    if (bAppendVer) {
      StringCat (fileName, ";1");
    }
    StringNCpy (path, rootPath, sizeof (path));
    FileBuildPath (path, NULL, fileName);
    fileLength = FileLength (path);
    infp = FileOpen (path, "rb");
    if (infp != NULL) {
      StringNCpy (path, valPath, sizeof (path));
      FileBuildPath (path, NULL, fileName);
      outfp = FileOpen (path, "wb");
      if (outfp != NULL) {
        CopyOneFile (outfp, infp, fileName, fileLength);
        FileClose (outfp);
        FileClose (infp);
        return TRUE;
      } else {
        FileClose (infp);
        Message (MSG_ERROR, "Unable to create %s", path);
        return FALSE;
      }
    } else {
      Message (MSG_ERROR, "Unable to open %s", path);
      return FALSE;
    }
  }
  return FALSE;
}

static Boolean
IndexFilesMatch (FILE *srcfp, CharPtr destPath, Int4 srcFileLength, size_t bytesToCheck)

{
  FILE   *destfp;
  CharPtr srcbuf;
  CharPtr destbuf;
  Boolean retval;

  if (FileLength (destPath) != srcFileLength)
  {
    return FALSE;
  }
  if (bytesToCheck == 0)
  {
    return TRUE;
  }
  if ((destfp = FileOpen (destPath, "rb")) == NULL)
  {
    return FALSE;
  }

  srcbuf = (CharPtr) MemNew (bytesToCheck);
  if (FileRead (srcbuf, 1, bytesToCheck, srcfp) != bytesToCheck)
  {
    fseek (srcfp, 0L, SEEK_SET);
    FileClose (destfp);
    MemFree (srcbuf);
    return FALSE;
  }
  fseek (srcfp, 0L, SEEK_SET);

  destbuf = (CharPtr) MemNew (bytesToCheck);
  if (FileRead (destbuf, 1, bytesToCheck, destfp) != bytesToCheck)
  {
    retval = FALSE;
  } else {
    retval = MemCmp (srcbuf, destbuf, bytesToCheck) == 0;
  }

  FileClose (destfp);
  MemFree (srcbuf);
  MemFree (destbuf);
  return retval;
}

static Boolean CopyAnIdxFile (CharPtr rootPath, CharPtr idxPath, CharPtr fname, size_t bytesToCheck, Boolean ignoreAbsence)

{
  Char  ch;
  Int4  fileLength;
  Char  fileName [32];
  FILE  *infp;
  FILE  *outfp;
  Char  path [PATH_MAX];
  Char  *ptr;

  if (rootPath != NULL && idxPath != NULL && fname != NULL) {
    StringCpy (fileName, fname);
    if (upperCaseIt) {
      ptr = fileName;
      ch = *ptr;
      while (ch != '\0') {
        *ptr = TO_UPPER (ch);
        ptr++;
        ch = *ptr;
      }
    }
    if (bAppendVer) {
      StringCat (fileName, ";1");
    }
    StringNCpy (path, rootPath, sizeof (path));
    FileBuildPath (path, "index", NULL);
    FileBuildPath (path, NULL, fileName);
    fileLength = FileLength (path);
    infp = FileOpen (path, "rb");
    if (infp != NULL) {
      StringNCpy (path, idxPath, sizeof (path));
      FileBuildPath (path, NULL, fileName);
      if (IndexFilesMatch (infp, path, fileLength, bytesToCheck)) {
        FileClose (infp); /* don't bother to copy */
      } else {
        outfp = FileOpen (path, "wb");
        if (outfp != NULL) {
          CopyOneFile (outfp, infp, fileName, fileLength);
          FileClose (outfp);
          FileClose (infp);
          return TRUE;
        } else {
          FileClose (infp);
          Message (MSG_ERROR, "Unable to create %s", path);
          return FALSE;
        }
      }
    } else {
      if (! ignoreAbsence)
          Message (MSG_ERROR, "Unable to open %s", path);
      return FALSE;
    }
  }
  return FALSE;
}

static void CopyIndexFiles (CharPtr volumeLabel, CharPtr rootPath, CharPtr idxPath)

{
  if (rootPath != NULL && idxPath != NULL) {
    if (CreateDir (idxPath)) {
      if (StringCmp (volumeLabel, SEQ_VOLUME_LABEL) == 0) {
        CopyAnIdxFile (rootPath, idxPath, "mluid.idx", IDX_BYTES_TO_COMPARE, TRUE);
        CopyAnIdxFile (rootPath, idxPath, "mluid.lst", IDX_BYTES_TO_COMPARE, TRUE);
        CopyAnIdxFile (rootPath, idxPath, "sequid.idx", IDX_BYTES_TO_COMPARE, FALSE);
        CopyAnIdxFile (rootPath, idxPath, "sequid.lst", IDX_BYTES_TO_COMPARE, FALSE);
      } else {
        CopyAnIdxFile (rootPath, idxPath, "mluid.idx", IDX_BYTES_TO_COMPARE, FALSE);
        CopyAnIdxFile (rootPath, idxPath, "mluid.lst", IDX_BYTES_TO_COMPARE, FALSE);
      }
    } else {
      Message (MSG_ERROR, "Unable to make %s", idxPath);
    }
  }
}

static Boolean ThisCdIsInserted (CharPtr volumeLabel, CharPtr altVolumeLabel,
                                 CharPtr rootPath)

{
  AsnIoPtr       aip;
  Boolean        retval;
  EntrezInfoPtr  vi;

  retval = FALSE;
  if ((aip = MyEntrezInfoAsnOpen (rootPath)) != NULL) {
    vi = EntrezInfoAsnRead (aip, NULL);
    AsnIoClose (aip);
    if (vi != NULL) {
      lastVersion = vi->version;
      if (StringCmp (vi->volume_label, volumeLabel) == 0) {
        retval = TRUE;
      } else {
        if (altVolumeLabel != NULL && StringCmp (vi->volume_label,
            altVolumeLabel) == 0) {
          retval = TRUE;
        }
      }
      EntrezInfoFree (vi);
    }
  }
  return retval;
}

static Boolean CorrectCdInserted (CharPtr formalName, CharPtr volumeLabel,
                                  CharPtr altVolumeLabel, CharPtr discLabel,
                                  CharPtr rootPath, Boolean isCdrom)

{
  short    erract;
  ErrDesc  err;
  Int2     maxTries;
  Char     msg [100];
  CharPtr  otherDiscLabel;
  CharPtr  cookedDeviceName;
  CharPtr  rawDeviceName;
  CharPtr  mountCommand;

  if (! isCdrom) {
      return ThisCdIsInserted (volumeLabel, altVolumeLabel, rootPath);
  }

  maxTries = 3;

  if (StringCmp (discLabel, SEQ_CD_DISK_LABEL) == 0) {
    otherDiscLabel = REF_CD_DISK_LABEL;
  } else {
    otherDiscLabel = SEQ_CD_DISK_LABEL;
  }

  while (maxTries-- > 0) {
    if (ThisCdIsInserted (volumeLabel, altVolumeLabel, rootPath)) {
      return TRUE;
    }
    cookedDeviceName = seqCookedPath;
    rawDeviceName = seqRawPath;
    mountCommand = seqMountPath;

    ErrGetOpts (&erract, NULL);
    ErrSetOpts (ERR_IGNORE, 0);
    ErrFetch (&err);
    EjectCd (otherDiscLabel, cookedDeviceName, rawDeviceName, rootPath,
             mountCommand);
    ErrSetOpts (erract, 0);
    ErrFetch (&err);
    StringCpy (msg, "Please insert <");
    StringCat (msg, formalName);
    StringCat (msg, ">; select OK when ready");
    if (Message (MSG_OKC, msg) == ANS_CANCEL) {
      Update ();
      return FALSE;
    }

    if (StringCmp (discLabel, SEQ_CD_DISK_LABEL) == 0) {
      cookedDeviceName = seqCookedPath;
      mountCommand = seqMountPath;
    } else {
      cookedDeviceName = refCookedPath;
      mountCommand = refMountPath;
    }
      
    MountCd (volumeLabel, cookedDeviceName, rootPath, mountCommand);
  }
  return FALSE;
}

static Boolean OldCfgFileExists (void)

{
  Char  buf [200];

  return (Boolean) (GetAppParam ("NCBI", "NCBI", NULL, "", buf, sizeof (buf)) != 0);
}

static Boolean FileExists (CharPtr dirname, CharPtr subname, CharPtr filename)

{
  Char  path [PATH_MAX];

  StringNCpy (path, dirname, sizeof (path));
  FileBuildPath (path, subname, NULL);
  FileBuildPath (path, NULL, filename);
  return (Boolean) (FileLength (path) > 0);
}

static CharPtr OldClassValue (CharPtr classSection, CharPtr field)

{
  CharPtr retvalue;

  retvalue = (CharPtr) MemNew (128);

  if (StrCmp (classSection, "SEQ") == 0) {
    if (GetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_CD", field, "", retvalue, 128)
        != 0) {
      return retvalue;
    }
    if (GetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_HD", field, "", retvalue, 128)
        != 0) {
      return retvalue;
    }
  } else {
    if (GetAppParam (CFG_FILE_NAME, "ENTREZ_REF_CD", field, "", retvalue, 128)
        != 0) {
      return retvalue;
    }
    if (GetAppParam (CFG_FILE_NAME, "ENTREZ_REF_HD", field, "", retvalue, 128)
        != 0) {
      return retvalue;
    }
  }

  MemFree (retvalue);
  return NULL;
}

static CharPtr OldValue (CharPtr section, CharPtr field)

{
  CharPtr retvalue;

  retvalue = (CharPtr) MemNew (128);

  if (GetAppParam (CFG_FILE_NAME, section, field, "", retvalue, 128) == 0) {
    MemFree (retvalue);
    return NULL;
  } else {
    return retvalue;
  }
}

static void
SmartConfig (void)
{
  CharPtr str;

  if (OldClassValue ("SEQ", "ROOT") != NULL ||
      OldValue ("NCBI", "SEQIDX") != NULL) {
    useES = TRUE;
    SafeSetStatus (useESButton, TRUE);
  }
  if (OldClassValue ("REF", "ROOT") != NULL ||
      OldValue ("NCBI", "REFIDX") != NULL) {
    useER = TRUE;
    SafeSetStatus (useERButton, TRUE);
  }

  if (useES) {
    if ((str = OldClassValue ("SEQ", "TYPE")) != NULL) {
      if (StringICmp (str, "CD") == 0) {
        esMediaSet = TRUE;
        seqOnCD = TRUE;
      } else {
        if (OldClassValue ("SEQ", "IDX") != NULL ||
            OldValue ("NCBI", "SEQIDX") != NULL) {
          esMediaSet = TRUE;
          seqOnCD = FALSE;
          seqOnFS = TRUE;
        }
      }
    }
    if ((str = OldClassValue ("SEQ", "ROOT")) != NULL) {
      oldSeqRootPath = StringSave (str);
    } else {
      if ((str = OldValue ("NCBI", "ROOT")) != NULL) {
        oldSeqRootPath = StringSave (str);
      }
    }
  }

  if (useER) {
    if ((str = OldClassValue ("REF", "TYPE")) != NULL) {
      if (StringICmp (str, "CD") == 0) {
        erMediaSet = TRUE;
        refOnCD = TRUE;
      } else {
        if (OldClassValue ("REF", "IDX") != NULL ||
            OldValue ("NCBI", "REFIDX") != NULL) {
          erMediaSet = TRUE;
          refOnFS = TRUE;
          refOnCD = FALSE;
        }
      }
    }
    if ((str = OldClassValue ("REF", "ROOT")) != NULL) {
      oldRefRootPath = StringSave (str);
    } else {
      if ((str = OldValue ("NCBI", "ROOT")) != NULL) {
        oldRefRootPath = StringSave (str);
      }
    }
  }

  if (OldValue ("NCBI", "SEQIDX") != NULL ||
      OldClassValue ("SEQ", "IDX") != NULL) {
    copySeqIdx = TRUE;
  }

  if (OldValue ("NCBI", "REFIDX") != NULL ||
      OldClassValue ("REF", "IDX") != NULL) {
    copyRefIdx = TRUE;
  }

  if (OldValue ("NCBI", "REFIDX") != NULL &&
      OldValue ("NCBI", "SEQIDX") != NULL) {
    numCdsSet = TRUE;
    numCds = 1;
    erMediaSet = TRUE;
    esMediaSet = TRUE;
    seqOnCD = TRUE;
    refOnCD = TRUE;
  }

  oldSeqOnCD = esMediaSet && seqOnCD;
  oldRefOnCD = erMediaSet && refOnCD;
  oldSeqMediaSet = esMediaSet;
  oldRefMediaSet = erMediaSet;

#ifdef NETWORK_SUPPORTED
  if (GetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "SERVICE_NAME", "", svc, sizeof svc) > 0) {
    useNET = TRUE;
    SafeSetStatus (useNETButton, TRUE);
  }

  GetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "SERVICE_NAME", DEF_SERVICE, svc, sizeof svc);
  GetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "RESOURCE_NAME", DEF_RESOURCE, res, sizeof res);
  GetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "SERV_VERS_MIN", "1", tsvmin, sizeof tsvmin);
  GetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "SERV_VERS_MAX", "0", tsvmax, sizeof tsvmax);
  GetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "RES_VERS_MIN", "1", trsmin, sizeof trsmin);
  GetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "RES_VERS_MAX", "0", trsmax, sizeof trsmax);

  GetAppParam (CFG_FILE_NAME, "NET_SERV", "DISPATCHER", DISPATCHER, dispatcher_name, sizeof dispatcher_name);
  GetAppParam (CFG_FILE_NAME, "NET_SERV", "DISP_USERNAME", "", user_name, sizeof user_name);
  GetAppParam (CFG_FILE_NAME, "NET_SERV", "HOST_ADDRESS", "", host_address, sizeof host_address);
  GetAppParam (CFG_FILE_NAME, "NET_SERV", "ENCRYPTION_DESIRED", "TRUE", encr_desired, sizeof encr_desired);
  GetAppParam (CFG_FILE_NAME, "NET_SERV", "DISP_RECONN_ACTION", "CONT", disp_reconn_action, sizeof disp_reconn_action);
#endif /* NETWORK_SUPPORTED */

  if (esMediaSet && erMediaSet && seqOnCD && refOnCD && !numCdsSet) {
    if (OldClassValue ("SEQ", "VAL") != NULL &&
        OldClassValue ("REF", "VAL") != NULL) {
      numCdsSet = TRUE;
      numCds = 1;
      if (! useNET) {
        allowSwapping = TRUE;
      }
    } else {
      numCdsSet = TRUE;
      numCds = 2;
    }
  }
}

static void
SetLinksParam (CharPtr sectionName, CharPtr from, CharPtr to, CharPtr priority)
{
  char buf[100];

  StrCpy (buf, from);
  StrCat (buf, "__");
  StrCat (buf, to);
  EdSetAppParam (CFG_FILE_NAME, sectionName, buf, priority);
}

static void
DumpGenericTopLevel (void)

{
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ", "SERVICES", "ENTR_LINK, ENTR_REF, ENTR_SEQ");
}

static void
DumpAltReferencesLinks (void)

{
  static Boolean alreadyWarned = FALSE;

  if (seqVersion != refVersion)
  {
    if (! alreadyWarned) 
    {
      Message (MSG_OK, "Warning: The version numbers on these two data sources do not match.\nThis is O.K., but it may cause you some inconvenience.");
      alreadyWarned = TRUE;
      Update ();
    }
  } else {
    SetLinksParam ("LINKS_FROM_REFERENCE", ENTR_REF_CHAN, ENTR_SEQ_CHAN, "2");
  }
#ifdef STABLE_SEQUENCE_IDENTIFIERS
  SetLinksParam ("LINKS_FROM_REFERENCE", ENTR_REF_CHAN, ENTR_SEQ_CHAN, "2");
#endif
}

static void
DumpNet (void)

{
#ifdef NETWORK_SUPPORTED
  if (useNET) {
    EdSetAppParam (CFG_FILE_NAME, "REFERENCE_FROM_NET", "MEDIA", "ENTREZ_NET");
    EdSetAppParam (CFG_FILE_NAME, "SEQUENCE_FROM_NET", "MEDIA", "ENTREZ_NET");
    EdSetAppParam (CFG_FILE_NAME, "LINKS_FROM_NET", "MEDIA", "ENTREZ_NET");
    SetLinksParam ("LINKS_FROM_NET", ENTR_SEQ_CHAN, ENTR_SEQ_CHAN, "1");
    SetLinksParam ("LINKS_FROM_NET", ENTR_SEQ_CHAN, ENTR_REF_CHAN, "1");
    SetLinksParam ("LINKS_FROM_NET", ENTR_REF_CHAN, ENTR_SEQ_CHAN, "1");
    SetLinksParam ("LINKS_FROM_NET", ENTR_REF_CHAN, ENTR_REF_CHAN, "1");
    EdSetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "TYPE", "NET");
    EdSetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "SERVICE_NAME", svc);
    EdSetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "RESOURCE_NAME", res);
    EdSetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "RESOURCE_TYPE", SERVICE_TYPE);
    EdSetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "SERV_VERS_MIN", tsvmin);
    EdSetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "SERV_VERS_MAX", tsvmax);
    EdSetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "RES_VERS_MIN", trsmin);
    EdSetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "RES_VERS_MAX", trsmax);
    EdSetAppParam (CFG_FILE_NAME, "ENTREZ_NET", "FORMAL_NAME", "Entrez Network Service");

    if (dispInfoPtr == NULL) {
      EdSetAppParam (CFG_FILE_NAME, "NET_SERV", "DISPATCHER", dispatcher_name);
    } else {
      NI_SetDispConfig(&dispInfoPtr, dispatcher_name, sizeof dispatcher_name);
    }
    EdSetAppParam (CFG_FILE_NAME, "NET_SERV", "DISP_USERNAME", user_name);
    if (host_address[0] != '\0') {
      EdSetAppParam (CFG_FILE_NAME, "NET_SERV", "HOST_ADDRESS", host_address);
    }
    if (NI_EncrAvailable()) {
      EdSetAppParam (CFG_FILE_NAME, "NET_SERV", "ENCRYPTION_DESIRED", encr_desired);
    }
    EdSetAppParam (CFG_FILE_NAME, "NET_SERV", "DISP_RECONN_ACTION", disp_reconn_action);
  }
#endif /* NETWORK_SUPPORTED */
}

static void
Dump2CD (CharPtr refString, CharPtr seqString)

{
  char      refDesc[40];
  char      seqDesc[40];
  char      refFromSeq[60];
  char      refMedia[40];
  char      seqMedia[40];
  char      buf[120];

  EdSetAppParam (CFG_FILE_NAME, "NCBI", "ROOT", seqRootPath);
  DumpGenericTopLevel ();

  StrCpy (refDesc, "ENTREZ_");
  StrCat (refDesc, refString);
  StrCat (refDesc, "_DESC");
  StrCpy (seqDesc, "ENTREZ_");
  StrCat (seqDesc, seqString);
  StrCat (seqDesc, "_DESC");

  StrCpy (refFromSeq, "REFERENCE_FROM_ENTREZ_");
  StrCat (refFromSeq, seqString);

  StrCpy (seqMedia, "ENTREZ_");
  StrCat (seqMedia, seqString);
  StrCpy (refMedia, "ENTREZ_");
  StrCat (refMedia, refString);

  StrCpy (buf, refMedia);
  StrCat (buf, ", ");
  StrCat (buf, seqMedia);
  if (useNET) {
    StrCat (buf, ", ENTREZ_NET");
  }
  EdSetAppParam (CFG_FILE_NAME, "NCBI", "MEDIA", buf);

  StrCpy (buf, refDesc);
  StrCat (buf, ", ");
  StrCat (buf, refFromSeq);
  if (useNET) {
    StrCat (buf, ", REFERENCE_FROM_NET");
  }
  EdSetAppParam (CFG_FILE_NAME, ENTR_REF_CHAN, "CHANNELS", buf);

  StrCpy (buf, seqDesc);
  if (useNET) {
    StrCat (buf, ", SEQUENCE_FROM_NET");
  }
  EdSetAppParam (CFG_FILE_NAME, ENTR_SEQ_CHAN, "CHANNELS", buf);
  StrCpy (buf, "LINKS_FROM_REFERENCE, LINKS_FROM_SEQUENCE");
  if (useNET) {
    StrCat (buf, ", LINKS_FROM_NET");
  }
  EdSetAppParam (CFG_FILE_NAME, ENTR_LINKS_CHAN, "CHANNELS", buf);

  EdSetAppParam (CFG_FILE_NAME, seqDesc, "MEDIA", seqMedia);

  EdSetAppParam (CFG_FILE_NAME, "LINKS_FROM_SEQUENCE", "MEDIA", seqMedia);
  EdSetAppParam (CFG_FILE_NAME, "LINKS_FROM_SEQUENCE", "INFO", "1");
  SetLinksParam ("LINKS_FROM_SEQUENCE", ENTR_SEQ_CHAN, ENTR_SEQ_CHAN, "1");
  SetLinksParam ("LINKS_FROM_SEQUENCE", ENTR_SEQ_CHAN, ENTR_REF_CHAN, "1");
  SetLinksParam ("LINKS_FROM_SEQUENCE", ENTR_REF_CHAN, ENTR_SEQ_CHAN, "1");

  EdSetAppParam (CFG_FILE_NAME, "LINKS_FROM_REFERENCE", "MEDIA", refMedia);
  SetLinksParam ("LINKS_FROM_REFERENCE", ENTR_REF_CHAN, ENTR_REF_CHAN, "1");

  EdSetAppParam (CFG_FILE_NAME, refDesc, "MEDIA", refMedia);
  EdSetAppParam (CFG_FILE_NAME, refDesc, "RECORDS", "2,NO_DRASTIC_ACTION");
  EdSetAppParam (CFG_FILE_NAME, refDesc, "TERMS", "1");

  EdSetAppParam (CFG_FILE_NAME, refFromSeq, "MEDIA", seqMedia);
  EdSetAppParam (CFG_FILE_NAME, refFromSeq, "RECORDS", "1");
  EdSetAppParam (CFG_FILE_NAME, refFromSeq, "DOCSUMS", "0");
  EdSetAppParam (CFG_FILE_NAME, refFromSeq, "NEIGHBORS", "0");
  EdSetAppParam (CFG_FILE_NAME, refFromSeq, "TERMS", "0");
  EdSetAppParam (CFG_FILE_NAME, refFromSeq, "BOOLEANS", "0");

  EdSetAppParam (CFG_FILE_NAME, seqMedia, "ROOT", seqRootPath);
  EdSetAppParam (CFG_FILE_NAME, seqMedia, "IDX", seqIdxPath);

  EdSetAppParam (CFG_FILE_NAME, refMedia, "ROOT", refRootPath);
  EdSetAppParam (CFG_FILE_NAME, refMedia, "IDX", refIdxPath);

  DumpAltReferencesLinks ();

#ifdef OS_UNIX
  EdSetAppParam (CFG_FILE_NAME, seqMedia, "RAW_DEVICE_NAME", seqRawPath);
  EdSetAppParam (CFG_FILE_NAME, seqMedia, "DEVICE_NAME", seqCookedPath);
  EdSetAppParam (CFG_FILE_NAME, seqMedia, "MOUNT_CMD", seqMountPath);
  EdSetAppParam (CFG_FILE_NAME, seqMedia, "MOUNT_POINT", seqRootPath);

  EdSetAppParam (CFG_FILE_NAME, refMedia, "RAW_DEVICE_NAME", refRawPath);
  EdSetAppParam (CFG_FILE_NAME, refMedia, "DEVICE_NAME", refCookedPath);
  EdSetAppParam (CFG_FILE_NAME, refMedia, "MOUNT_CMD", refMountPath);
  EdSetAppParam (CFG_FILE_NAME, refMedia, "MOUNT_POINT", refRootPath);
#endif
#ifdef OS_VMS
  {
    CharPtr p;

    StrCpy (buf, seqRootPath);
    p = StrChr (buf, ':');
    if (p != NULL) {
      *(++p) = '\0';
    }
    EdSetAppParam (CFG_FILE_NAME, seqMedia, "MOUNT_POINT", buf);

    StrCpy (buf, refRootPath);
    p = StrChr (buf, ':');
    if (p != NULL) {
      *(++p) = '\0';
    }
    EdSetAppParam (CFG_FILE_NAME, refMedia, "MOUNT_POINT", buf);
  }
#endif

  DumpNet ();
}

static void
DumpRO (void)

{
  EdSetAppParam (CFG_FILE_NAME, "NCBI", "ROOT", refRootPath);
  EdSetAppParam (CFG_FILE_NAME, "NCBI", "REFIDX", refIdxPath);
}

static void
DumpSO (void)

{
  EdSetAppParam (CFG_FILE_NAME, "NCBI", "ROOT", seqRootPath);
  if (noSwapping) {
    EdSetAppParam (CFG_FILE_NAME, "NCBI", "SEQIDX", seqIdxPath);
    EdSetAppParam (CFG_FILE_NAME, "NCBI", "REFIDX", refIdxPath);
  } else {
    EdSetAppParam (CFG_FILE_NAME, "NCBI", "SEQIDX", seqIdxPath);
  }
}

static void
DumpSCRH (void)

{
  Dump2CD ("REF_HD", "SEQ_CD");

  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_REF_HD", "TYPE", "HARDDISK");
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_REF_HD", "FORMAL_NAME",
        "Entrez: References CD-ROM (Hard-disk Image)");

  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_CD", "TYPE", "CD");
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_CD", "FORMAL_NAME",
        SEQ_FORMAL_NAME);
}

static void
DumpSHRC (void)

{
  Dump2CD ("REF_CD", "SEQ_HD");

  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_REF_CD", "TYPE", "CD");
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_REF_CD", "FORMAL_NAME",
        REF_FORMAL_NAME);

  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_HD", "TYPE", "HARDDISK");
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_HD", "FORMAL_NAME",
        "Entrez: Sequences CD-ROM (Hard-disk image)");
}

static void
DumpSHRH (void)

{
  Dump2CD ("REF_HD", "SEQ_HD");

  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_REF_HD", "TYPE", "HARDDISK");
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_REF_HD", "FORMAL_NAME",
        "Entrez: References CD-ROM (Hard-disk image)");

  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_HD", "TYPE", "HARDDISK");
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_HD", "FORMAL_NAME",
        "Entrez: Sequences CD-ROM (Hard-disk image)");
}

static void
DumpSR1D (void)

{
  Dump2CD ("REF_CD", "SEQ_CD");

  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_REF_CD", "TYPE", "CD");
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_REF_CD", "VAL", refValPath);
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_REF_CD", "FORMAL_NAME",
        REF_FORMAL_NAME);
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_REF_CD", "DRASTIC_TO_ENTREZ_SEQ_CD", "1");

  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_CD", "TYPE", "CD");
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_CD", "VAL", seqValPath);
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_CD", "FORMAL_NAME",
        SEQ_FORMAL_NAME);
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_CD", "DRASTIC_TO_ENTREZ_REF_CD", "1");
}

static void
DumpSR2D (void)

{
  Dump2CD ("REF_CD", "SEQ_CD");

  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_REF_CD", "TYPE", "CD");
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_REF_CD", "FORMAL_NAME",
        REF_FORMAL_NAME);

  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_CD", "TYPE", "CD");
  EdSetAppParam (CFG_FILE_NAME, "ENTREZ_SEQ_CD", "FORMAL_NAME",
        SEQ_FORMAL_NAME);
}

static void
DumpNetOnly (void)

{
  EdSetAppParam (CFG_FILE_NAME, "NCBI", "MEDIA", "ENTREZ_NET");

  EdSetAppParam (CFG_FILE_NAME, ENTR_LINKS_CHAN, "CHANNELS", "LINKS_FROM_NET");
  EdSetAppParam (CFG_FILE_NAME, ENTR_REF_CHAN, "CHANNELS", "REFERENCE_FROM_NET");
  EdSetAppParam (CFG_FILE_NAME, ENTR_SEQ_CHAN, "CHANNELS", "SEQUENCE_FROM_NET");

  DumpGenericTopLevel ();

  DumpNet ();
}

static void
DumpSOFull (void)

{
  char      seqDesc[40];
  char      refDesc[40];
  char      seqMedia[40];
  char      buf[120];
  char      seq[10];

  DumpGenericTopLevel ();

  if (seqOnCD) {
    StrCpy (seq, "SEQ_CD");
  } else {
    StrCpy (seq, "SEQ_HD");
  }

  StrCpy (seqMedia, "ENTREZ_");
  StrCat (seqMedia, seq);

  StrCpy (seqDesc, seqMedia);
  StrCat (seqDesc, "_DESC");

  StrCpy (refDesc, "REFERENCE_FROM_");
  StrCat (refDesc, seqMedia);

  StrCpy (buf, seqMedia);
  if (useNET) {
    StrCat (buf, ", ENTREZ_NET");
  }
  EdSetAppParam (CFG_FILE_NAME, "NCBI", "MEDIA", buf);

  StrCpy (buf, refDesc);
  if (useNET) {
    StrCat (buf, ", REFERENCE_FROM_NET");
  }
  EdSetAppParam (CFG_FILE_NAME, ENTR_REF_CHAN, "CHANNELS", buf);

  StrCpy (buf, "LINKS_FROM_SEQUENCE");
  if (useNET) {
    StrCat (buf, ", LINKS_FROM_NET");
  }
  EdSetAppParam (CFG_FILE_NAME, ENTR_LINKS_CHAN, "CHANNELS", buf);

  StrCpy (buf, seqDesc);
  if (useNET) {
    StrCat (buf, ", SEQUENCE_FROM_NET");
  }
  EdSetAppParam (CFG_FILE_NAME, ENTR_SEQ_CHAN, "CHANNELS", buf);

  EdSetAppParam (CFG_FILE_NAME, seqDesc, "MEDIA", seqMedia);
  EdSetAppParam (CFG_FILE_NAME, refDesc, "MEDIA", seqMedia);

  EdSetAppParam (CFG_FILE_NAME, "LINKS_FROM_SEQUENCE", "MEDIA", seqMedia);
  SetLinksParam ("LINKS_FROM_SEQUENCE", ENTR_REF_CHAN, ENTR_REF_CHAN, "1");
  SetLinksParam ("LINKS_FROM_SEQUENCE", ENTR_REF_CHAN, ENTR_SEQ_CHAN, "1");
  SetLinksParam ("LINKS_FROM_SEQUENCE", ENTR_SEQ_CHAN, ENTR_SEQ_CHAN, "1");
  SetLinksParam ("LINKS_FROM_SEQUENCE", ENTR_SEQ_CHAN, ENTR_REF_CHAN, "1");

  EdSetAppParam (CFG_FILE_NAME, seqMedia, "ROOT", seqRootPath);
  EdSetAppParam (CFG_FILE_NAME, seqMedia, "IDX", seqIdxPath);
  EdSetAppParam (CFG_FILE_NAME, seqMedia, "TYPE", seqOnCD ? "CD" : "HARDDISK");

  StrCpy (buf, SEQ_FORMAL_NAME);
  StrCat (buf, seqOnCD ? "" : " (Hard-disk image)");
  EdSetAppParam (CFG_FILE_NAME, seqMedia, "FORMAL_NAME", buf);

  DumpNet ();
}

static void
DumpROFull (void)

{
  char      refDesc[40];
  char      refMedia[40];
  char      buf[120];
  char      ref[10];

  DumpGenericTopLevel ();

  if (refOnCD) {
    StrCpy (ref, "REF_CD");
  } else {
    StrCpy (ref, "REF_HD");
  }

  StrCpy (refMedia, "ENTREZ_");
  StrCat (refMedia, ref);

  StrCpy (refDesc, refMedia);
  StrCat (refDesc, "_DESC");

  StrCpy (buf, refMedia);
  if (useNET) {
    StrCat (buf, ", ENTREZ_NET");
  }
  EdSetAppParam (CFG_FILE_NAME, "NCBI", "MEDIA", buf);

  StrCpy (buf, refDesc);
  if (useNET) {
    StrCat (buf, ", REFERENCE_FROM_NET");
  }
  EdSetAppParam (CFG_FILE_NAME, ENTR_REF_CHAN, "CHANNELS", buf);

  StrCpy (buf, "LINKS_FROM_REFERENCE");
  if (useNET) {
    StrCat (buf, ", LINKS_FROM_NET");
  }
  EdSetAppParam (CFG_FILE_NAME, ENTR_LINKS_CHAN, "CHANNELS", buf);

  if (useNET) {
    EdSetAppParam (CFG_FILE_NAME, ENTR_SEQ_CHAN, "CHANNELS", "SEQUENCE_FROM_NET");
  }

  EdSetAppParam (CFG_FILE_NAME, refDesc, "MEDIA", refMedia);

  EdSetAppParam (CFG_FILE_NAME, "LINKS_FROM_REFERENCE", "MEDIA", refMedia);
  SetLinksParam ("LINKS_FROM_REFERENCE", ENTR_REF_CHAN, ENTR_REF_CHAN, "1");

  EdSetAppParam (CFG_FILE_NAME, refMedia, "ROOT", refRootPath);
  EdSetAppParam (CFG_FILE_NAME, refMedia, "IDX", refIdxPath);
  EdSetAppParam (CFG_FILE_NAME, refMedia, "TYPE", refOnCD ? "CD" : "HARDDISK");

  StrCpy (buf, REF_FORMAL_NAME);
  StrCat (buf, refOnCD ? "" : " (Hard-disk image)");
  EdSetAppParam (CFG_FILE_NAME, refMedia, "FORMAL_NAME", buf);

  DumpNet ();
}

static void 
DumpConfig (void)

{
  Int2 i;

  /* wipe out sections */
  for (i = 0; i < (Int2) DIM(sectionsToDelete); i++)
  {
      EdSetAppParam (CFG_FILE_NAME, sectionsToDelete[i], NULL, NULL);
  }

  EdSetAppParam (CFG_FILE_NAME, "NCBI", "ASNLOAD", asnloadPath);
  EdSetAppParam (CFG_FILE_NAME, "NCBI", "DATA", dataPath);

  if (noSwapping && useES)
  { /* force DumpSO() case to occur below */
    useER = FALSE;
  }

  if (useES) {
    if (useER) { /* both Sequences and References */
      if (seqOnCD) {
        if (refOnCD) { /* both on CD */
          if (numCds == 2) {
            DumpSR2D ();
          } else {
            DumpSR1D ();
          }
        } else {
          DumpSCRH ();
        }
      } else { /* Sequences on hard disk */
        if (refOnCD) {
          DumpSHRC ();
        } else { /* both on hard-disk */
          DumpSHRH ();
        }
      }
    } else { /* Sequences only */
      if (useNET) {
        DumpSOFull ();
      } else {
        DumpSO ();
      }
    }
  } else {
    if (useNET) {
      if (useER) {
        DumpROFull ();
      } else {
        DumpNetOnly ();
      }
    } else { /* References only */
      DumpRO ();
    }
  }
}

static void 
ReadSections (CharPtr filename, CharPtr PNTR sectionList, Int2 sectionCount)

{
  Int2 secNum;
  Int2 j;
  Int2 nameLength;
  char names[512];
  char buf[256];

  /* for ( each section ) do */
  for (secNum = 0; secNum < sectionCount; secNum++)
  {
    nameLength = GetAppParam (filename, sectionList[secNum], NULL, "", names,
                              sizeof names);

    /* for ( each name within that section ) do */
    for (j = 0; j < nameLength; j += StringLen (&names[j]) + 1)
    {
      /* read from config file */
      GetAppParam (filename, sectionList[secNum], &names[j], "", buf,
                   sizeof buf);
      /* write to file/in-memory structure */
      EdSetAppParam (filename, sectionList[secNum], &names[j], buf);
    }
  }
}

static void FinalProc (void)

{
  Boolean  goOn;

#ifndef OS_MAC
  if (numCds == 1 && seqOnCD && refOnCD &&
      StringICmp (refRootPath, seqRootPath) != 0) {
    Message (MSG_OK, "Warning: You specified that both data sources were on CD-ROM, and that\nyou only had one CD-ROM drive, but you specified different device\nnames for the two different CD-ROMs.");
    Update ();
  }
#endif

  if (! copySeqIdx) {
    seqIdxPath = (CharPtr) MemFree (seqIdxPath);
  }
  if (! copyRefIdx) {
    refIdxPath = (CharPtr) MemFree (refIdxPath);
  }

  goOn = TRUE;
  while (goOn) {
    if (Message (MSG_YN, "Entrez configuration is now completed. Do you wish to save\nthe new Entrez configuration file to disk?") == ANS_YES) {
      Update ();
#ifdef CONFIGURATION_EDITING_SUPPORTED
      EdSetReallyWrite (TRUE);
#endif
      WatchCursor ();
      DumpConfig ();
      goOn = FALSE;
    } else if (Message (MSG_YN, "Are you sure you want to quit without\nsaving the new configuration file?") == ANS_YES) {
      goOn = FALSE;
    }
  }
  EndProc ();
}

static void AcceptEntrezProc (ButtoN b)

{
  Char  path [PATH_MAX];

  GetTitle (rootText, path, sizeof (path));
  FileBuildPath (path, NULL, NULL);
  if (nowGettingES) {
    seqRootPath = StringSave (path);
  } else if (nowGettingER) {
    refRootPath = StringSave (path);
  }

  GetTitle (mountText, path, sizeof (path));
  if (nowGettingES) {
    seqMountPath = StringSave (path);
  } else if (nowGettingER) {
    refMountPath = StringSave (path);
  }

  GetTitle (cookedText, path, sizeof (path));
  if (nowGettingES) {
    seqCookedPath = StringSave (path);
  } else if (nowGettingER) {
    refCookedPath = StringSave (path);
  }

  GetTitle (rawText, path, sizeof (path));
  if (nowGettingES) {
     seqRawPath = StringSave (path);
  } else if (nowGettingER) {
    refRawPath = StringSave (path);
  }

  Hide (entrezWindow);
  if (nowGettingES) {
    if (CorrectCdInserted (SEQ_FORMAL_NAME, SEQ_VOLUME_LABEL, NULL,
        SEQ_CD_DISK_LABEL, seqRootPath, seqOnCD)) {
      seqVersion = lastVersion;
      if (copyVal) {
        CreateDir (seqValPath);
        CopyValFile (SEQ_VOLUME_LABEL, seqRootPath, seqValPath);
      }
      if (copySeqIdx) {
        CreateDir (seqValPath);
        CopyIndexFiles (SEQ_VOLUME_LABEL, seqRootPath, seqIdxPath);
      }
      if (useER) {
        EntRefWindowProc ();
      } else {
        if (useNET) {
          EntNetWindowProc ();
        } else {
          FinalProc ();
        }
      }
    } else {
      Message (MSG_OK, seqOnCD ? "Cannot find Entrez: Sequences disc" :
               "Cannot find Entrez: Sequences data image");
      Show (entrezWindow);
    }
  } else if (nowGettingER) {
    if (CorrectCdInserted (REF_FORMAL_NAME, REF_VOLUME_LABEL, MED_VOLUME_LABEL,
        REF_CD_DISK_LABEL, refRootPath, refOnCD)) {
      refVersion = lastVersion;
      if (copyVal) {
        CreateDir (refValPath);
        CopyValFile (REF_VOLUME_LABEL, refRootPath, refValPath);
      }
      if (copyRefIdx) {
        CreateDir (refValPath);
        CopyIndexFiles (REF_VOLUME_LABEL, refRootPath, refIdxPath);
      }
      if (useNET) {
        EntNetWindowProc ();
      } else {
        FinalProc ();
      }
    } else {
      Message (MSG_OK, refOnCD ? "Cannot find Entrez: References disc" :
               "Cannot find Entrez: References data image");
      Show (entrezWindow);
    }
  }
}

static void RefreshEntrezControls (void)

{
  Char  path [PATH_MAX];

  GetTitle (rootText, path, sizeof (path));
  if (StringLen (path) > 0) {
    SafeEnable (acceptEntrezButton);
  } else {
    SafeDisable (acceptEntrezButton);
  }
}

static void FindRootPath (ButtoN b)

{
  Char  path [PATH_MAX];

  if (GetInputFileName (path, sizeof (path), "", "")) {
    TrimFileName (path);
    SetTitle (rootText, path);
    RefreshEntrezControls ();
  }
}

static void RootTextProc (TexT t)

{
  RefreshEntrezControls ();
}

static void EntrezWindowProc (CharPtr root, CharPtr prompt)

{
  GrouP   g1, g3;
  GrouP   h;
  Boolean firstDev;

  if (entrezWindow == NULL) {
    entrezWindow = FixedWindow (-50, -33, -10, -10, "EntrezCf", NULL);
#ifdef WIN_MSWIN
    SetupMenus (entrezWindow);
#endif
#ifdef WIN_MOTIF
    SetupMenus (entrezWindow);
#endif

    rootText = NULL;
    mountText = NULL;
    cookedText = NULL;
    rawText = NULL;

    h = HiddenGroup (entrezWindow, 0, 10, NULL);
    SetGroupSpacing (h, 3, 15);

    rootPrompt = StaticPrompt (h, "Entrez", 0, 0, systemFont, 'c');

    g1 = HiddenGroup (h, 2, 0, NULL);
    StaticPrompt (g1, "Root Path:", 0, 0, systemFont, 'l');
    rootText = DialogText (g1, "", 20, RootTextProc);

#ifdef OS_UNIX
    cookedGroup = HiddenGroup (h, 2, 0, NULL);
    StaticPrompt (cookedGroup, "Mount/Unmount Command:", 0, 0, systemFont, 'l');
    mountText = DialogText (cookedGroup, "cdmount", 20, NULL);
    StaticPrompt (cookedGroup, "`Cooked\' (Mount) Device:", 0, 0, systemFont, 'l');
    cookedText = DialogText (cookedGroup, "" , 20, NULL);

    rawGroup = HiddenGroup (h, 2, 0, NULL);
    StaticPrompt (rawGroup, "`Raw\' (Eject) Device:", 0, 0, systemFont, 'l');
    rawText = DialogText (rawGroup, "", 20, NULL);
#endif

    LowerNextPosition (entrezWindow, 10);

    g3 = HiddenGroup (entrezWindow, 4, 0, NULL);
    SetGroupSpacing (g3, 55, 2);
    acceptEntrezButton = DefaultButton (g3, "Accept", AcceptEntrezProc);
    SafeDisable (acceptEntrezButton);
    PushButton (g3, "Find cdromdat.val", FindRootPath);
    PushButton (g3, "Quit", QuitButtonProc);

    AlignObjects (ALIGN_CENTER, (Nlm_HANDLE) h, (Nlm_HANDLE) g3, NULL);

    AlignObjects (ALIGN_RIGHT, (Nlm_HANDLE) rootText, (Nlm_HANDLE) cookedText,
                  (Nlm_HANDLE) mountText, (Nlm_HANDLE) rawText, NULL);

    AlignObjects (ALIGN_JUSTIFY, (Nlm_HANDLE) cookedText,
                  (Nlm_HANDLE) mountText, (Nlm_HANDLE) rawText, NULL);
    RealizeWindow (entrezWindow);
  }

  firstDev = (Boolean) (nowGettingES || numCds == 1);
  if (firstDev) {
#ifdef PROC_MIPS
    SetTitle (cookedText, "/dev/scsi/sc0d4l0");
    SetTitle (rawText, "");
#else
    SetTitle (cookedText, "/dev/sr0");
    SetTitle (rawText, "/dev/rsr0");
#endif
  } else {
#ifdef PROC_MIPS
    SetTitle (cookedText, "/dev/scsi/sc0d5l0");
    SetTitle (rawText, "");
#else
    SetTitle (cookedText, "/dev/sr1");
    SetTitle (rawText, "dev/rsr1");
#endif
  }

  if ((nowGettingES && seqOnCD) || (nowGettingER && refOnCD)) {
    Enable (cookedGroup);
    Enable (rawGroup);
  } else {
    Disable (cookedGroup);
    Disable (rawGroup);
  }

  SetTitle (rootPrompt, prompt);
  SetTitle (rootText, root);
  if (StringLen (root) > 0) {
    SafeEnable (acceptEntrezButton);
  } else {
    SafeDisable (acceptEntrezButton);
  }
  Select (rootText);

  Show (entrezWindow);
}

static void EntRefWindowProc (void)

{
  Char  ch;
  Char  root [32];

#ifdef OS_MAC
  StringCpy (root, REF_CD_DISK_LABEL);
#endif
#if defined(OS_DOS) || defined(OS_WINNT)
  if (useES && numCds == 2) {
    if (seqRootPath != NULL) {
      StringNCpy (root, seqRootPath, sizeof (root));
      if (root [1] == ':' && (root [2] == '\0' || (root [2] == '\\' && root [3] == '\0'))) {
        ch = root [0];
        if (('A' <= ch && ch < 'Z') || ('a' <= ch && ch < 'z')) {
          root [0] = ch + 1;
        } else {
          StringCpy (root, "E:\\");
        }
      } else {
        StringCpy (root, "E:\\");
      }
    } else {
      StringCpy (root, "E:\\");
    }
  } else if (useES && numCds == 1) {
    if (seqRootPath != NULL) {
      StringNCpy (root, seqRootPath, sizeof (root));
      if (root [1] == ':' && (root [2] == '\0' || (root [2] == '\\' && root [3] == '\0'))) {
        ch = root [0];
        if (('A' <= ch && ch < 'Z') || ('a' <= ch && ch < 'z')) {
          root [0] = ch;
        } else {
          StringCpy (root, "D:\\");
        }
      } else {
        StringCpy (root, "D:\\");
      }
    } else {
      StringCpy (root, "D:\\");
    }
  } else {
    StringCpy (root, "D:\\");
  }
#endif
#ifdef OS_UNIX
  if (useES && numCds == 2) {
    StringCpy (root, "/cdrom2/");
  } else {
    StringCpy (root, "/cdrom/");
  }
#endif
#ifdef OS_VMS
  if (useES && numCds == 2) {
    StringCpy (root, "cdrom2:[000000]");
  } else {
    StringCpy (root, "cdrom:[000000]");
  }
#endif

  /* override default with previous known value */
  if (oldRefMediaSet && oldRefOnCD == refOnCD && oldRefRootPath != NULL) {
    StringCpy (root, oldRefRootPath);
  }

  nowGettingES = FALSE;
  nowGettingER = TRUE;
  EntrezWindowProc (root, "Entrez: References Volume");
}

static void EntSeqWindowProc (void)

{
  Char  root [32];

#ifdef OS_MAC
  StringCpy (root, SEQ_CD_DISK_LABEL);
#endif
#if defined(OS_DOS) || defined(OS_WINNT)
  StringCpy (root, "D:\\");
#endif
#ifdef OS_UNIX
  StringCpy (root, "/cdrom/");
#endif
#ifdef OS_VMS
  StringCpy (root, "cdrom:[000000]");
#endif

  /* override default with previous known value */
  if (oldSeqMediaSet && oldSeqOnCD == seqOnCD && oldSeqRootPath != NULL) {
    StringCpy (root, oldSeqRootPath);
  }

  nowGettingES = TRUE;
  nowGettingER = FALSE;
  EntrezWindowProc (root, "Entrez: Sequences Volume");
}

static void AcceptMediaProc (ButtoN b)

{
  Boolean  enabled;
  Int2     value;

  Hide (sourceWindow);
  enabled = Enabled (sourceGroup);
  useES = (Boolean) (enabled && Enabled (useESButton) && GetStatus (useESButton));
  useER = (Boolean) (enabled && Enabled (useERButton) && GetStatus (useERButton));
  useNET = (Boolean) (enabled && Enabled (useNETButton) && GetStatus (useNETButton));
  enabled = Enabled (esMediaGroup);
  value = GetValue (esMediaGroup);
  seqOnCD = (Boolean) (enabled && value == 1);
  seqOnHD = (Boolean) (enabled && value == 2);
  seqOnFS = (Boolean) (enabled && value == 3);
  enabled = Enabled (erMediaGroup);
  value = GetValue (erMediaGroup);
  refOnCD = (Boolean) (enabled && value == 1);
  refOnHD = (Boolean) (enabled && value == 2);
  refOnFS = (Boolean) (enabled && value == 3);
  enabled = Enabled (idxCopyGroup);
  copySeqIdx = (Boolean) (enabled && Enabled (seqIdxButton) && GetStatus (seqIdxButton));
  copyRefIdx = (Boolean) (enabled && Enabled (refIdxButton) && GetStatus (refIdxButton));
  if (Enabled (numCdsGroup)) {
    numCds = GetValue (numCdsGroup) - 1;
  } else {
    numCds = 0;
  }
  allowSwapping = (Boolean) (Enabled (swapButton) && GetStatus (swapButton));
  noSwapping = (Boolean) (Enabled (swapButton) && ! allowSwapping);
  copyVal = (Boolean) (useES && useER && numCds == 1 && ! noSwapping);
  if (useES) {
    EntSeqWindowProc ();
  } else if (useER) {
    EntRefWindowProc ();
  } else if (useNET) {
    EntNetWindowProc ();
  } else {
    Message (MSG_OK, "You must use either Sequences or References");
    Show (sourceWindow);
  }
}

static void RefreshSourceControls (void)

{
  if (useES) {
    if (copySeqIdx) {
      SafeSetStatus (seqIdxButton, TRUE);
    } else {
      SafeSetStatus (seqIdxButton, FALSE);
    }
    if (esMediaSet) {
      if (seqOnCD) {
        SafeSetValue (esMediaGroup, 1);
        SafeSetStatus (seqIdxButton, copySeqIdx);
        SafeEnable (seqIdxButton);
      } else if (seqOnHD) {
        SafeSetValue (esMediaGroup, 2);
        SafeSetStatus (seqIdxButton, FALSE);
        SafeDisable (seqIdxButton);
      } else if (seqOnFS) {
        SafeSetValue (esMediaGroup, 3);
        SafeSetStatus (seqIdxButton, copySeqIdx);
        SafeEnable (seqIdxButton);
      } else {
        SafeSetValue (esMediaGroup, 0);
        SafeSetStatus (seqIdxButton, FALSE);
        SafeDisable (seqIdxButton);
      }
    }
    SafeEnable (esMediaGroup);
  } else {
    SafeSetValue (esMediaGroup, 0);
    SafeSetStatus (seqIdxButton, FALSE);
    SafeDisable (esMediaGroup);
    SafeDisable (seqIdxButton);
  }
  if (useER) {
    if (copyRefIdx) {
      SafeSetStatus (refIdxButton, TRUE);
    } else {
      SafeSetStatus (refIdxButton, FALSE);
    }
    if (erMediaSet) {
      if (refOnCD) {
        SafeSetValue (erMediaGroup, 1);
        SafeSetStatus (refIdxButton, copyRefIdx);
        SafeEnable (refIdxButton);
      } else if (refOnHD) {
        SafeSetValue (erMediaGroup, 2);
        SafeSetStatus (refIdxButton, FALSE);
        SafeDisable (refIdxButton);
      } else if (refOnFS) {
        SafeSetValue (erMediaGroup, 3);
        SafeSetStatus (refIdxButton, copyRefIdx);
        SafeEnable (refIdxButton);
      } else {
        SafeSetValue (erMediaGroup, 0);
        SafeSetStatus (refIdxButton, FALSE);
        SafeDisable (refIdxButton);
      }
    }
    SafeEnable (erMediaGroup);
  } else {
    SafeSetValue (erMediaGroup, 0);
    SafeSetStatus (refIdxButton, FALSE);
    SafeDisable (erMediaGroup);
    SafeDisable (refIdxButton);
  }
  if (Enabled (seqIdxButton) || Enabled (refIdxButton)) {
    SafeEnable (idxCopyGroup);
  } else {
    SafeDisable (idxCopyGroup);
  }
  if (Enabled (esMediaGroup) && GetValue (esMediaGroup) == 1) {
    if (numCdsSet) {
      SafeSetValue (numCdsGroup, numCds + 1);
    }
    SafeEnable (numCdsGroup);
  } else if (Enabled (erMediaGroup) && GetValue (erMediaGroup) == 1) {
    if (numCdsSet) {
      SafeSetValue (numCdsGroup, numCds + 1);
    }
    SafeEnable (numCdsGroup);
  } else {
    SafeSetValue (numCdsGroup, 0);
    SafeDisable (numCdsGroup);
  }
  if (Enabled (numCdsGroup)) {
    if (numCds == 1 && useES && seqOnCD && useER && refOnCD &&
        Enabled (esMediaGroup) && GetValue (esMediaGroup) == 1 &&
        Enabled (erMediaGroup) && GetValue (erMediaGroup) == 1) {
      if (useNET) {
        SafeSetStatus (swapButton, TRUE);
        SafeDisable (swapButton);
      } else {
        SafeSetStatus (swapButton, allowSwapping);
        SafeEnable (swapButton);
      }
    } else {
      SafeSetStatus (swapButton, FALSE);
      SafeDisable (swapButton);
    }
  } else {
    SafeSetStatus (swapButton, FALSE);
    SafeDisable (swapButton);
  }
  if (useES || useER || useNET) {
    if (useES && (! esMediaSet)) {
      SafeDisable (acceptMediaButton);
    } else if (useER && (! erMediaSet)) {
      SafeDisable (acceptMediaButton);
    } else if ((useES && seqOnCD) || (useER && refOnCD)) {
      if (numCdsSet && numCds > 0) {
        SafeEnable (acceptMediaButton);
      } else {
        SafeDisable (acceptMediaButton);
      }
    } else {
      SafeEnable (acceptMediaButton);
    }
  } else {
    SafeDisable (acceptMediaButton);
  }
}

static void UseESProc (ButtoN b)

{
  useES = GetStatus (useESButton);
  RefreshSourceControls ();
}

static void UseERProc (ButtoN b)

{
  useER = GetStatus (useERButton);
  RefreshSourceControls ();
}

static void UseNETProc (ButtoN b)

{
  useNET = GetStatus (useNETButton);
  RefreshSourceControls ();
}

static void ESMediaProc (GrouP g)

{
  Int2  value;

  value = GetValue (esMediaGroup);
  seqOnCD = (Boolean) (value == 1);
  seqOnHD = (Boolean) (value == 2);
  seqOnFS = (Boolean) (value == 3);
  esMediaSet = TRUE;
  RefreshSourceControls ();
}

static void ERMediaProc (GrouP g)

{
  Int2  value;

  value = GetValue (erMediaGroup);
  refOnCD = (Boolean) (value == 1);
  refOnHD = (Boolean) (value == 2);
  refOnFS = (Boolean) (value == 3);
  erMediaSet = TRUE;
  RefreshSourceControls ();
}

static void CopyESProc (ButtoN b)

{
  copySeqIdx = GetStatus (seqIdxButton);
  RefreshSourceControls ();
}

static void CopyERProc (ButtoN b)

{
  copyRefIdx = GetStatus (refIdxButton);
  RefreshSourceControls ();
}

static void NumCdsProc (GrouP g)

{
  numCds = GetValue (numCdsGroup) - 1;
  numCdsSet = TRUE;
  RefreshSourceControls ();
}

static void SwapProc (ButtoN b)

{
  allowSwapping = (Boolean) (GetStatus (swapButton));
  RefreshSourceControls ();
}

static void SourceWindowProc (void)

{
  ButtoN  b;
  GrouP   g1, g2, g3, g4, g5;
  PrompT  p1, p2, p3, p4;
  RecT    r;
  RecT    s;

  askBeforeQuitting = TRUE;
  if (sourceWindow == NULL) {
    sourceWindow = FixedWindow (-50, -33, -10, -10, "EntrezCf", NULL);
#ifdef WIN_MSWIN
    SetupMenus (sourceWindow);
#endif
#ifdef WIN_MOTIF
    SetupMenus (sourceWindow);
#endif

    SetGroupSpacing (sourceWindow, 3, 15);
    p1 = p2 = p3 = p4 = NULL;
    g1 = g2 = g3 = g4 = g5 = NULL;

    if (showPrompts) {
      p1 = StaticPrompt (sourceWindow, "Which data sources do you wish to use?",
                         0, 0, systemFont, 'c');
    }

    g1 = HiddenGroup (sourceWindow, 0, 8, NULL);
    sourceGroup = NormalGroup (g1, 0, 0, "Data Sources", NULL, NULL);
    useESButton = CheckBox (sourceGroup, "Entrez: Sequences", UseESProc);
    Advance (sourceGroup);
    useERButton = CheckBox (sourceGroup, "Entrez: References", UseERProc);
    Break (sourceGroup);
#ifdef NETWORK_SUPPORTED
    useNETButton = CheckBox (sourceGroup, "Entrez Network Service", UseNETProc);
#endif /* NETWORK_SUPPORTED */

    if (showPrompts) {
      p2 = StaticPrompt (sourceWindow, "On what media are these data sources?",
                         0, 0, systemFont, 'c');
    }

#ifdef VERT_LAYOUT
    g2 = HiddenGroup (sourceWindow, 0, 2, NULL);
    SetGroupSpacing (g2, 6, 5);

    esMediaGroup = NormalGroup (g2, 4, 0, "Entrez: Sequences", NULL, ESMediaProc);
    RadioButton (esMediaGroup, "CD-ROM Drive");
    RadioButton (esMediaGroup, "Local Hard Disk");
    RadioButton (esMediaGroup, "File Server");
    SafeDisable (esMediaGroup);

    erMediaGroup = NormalGroup (g2, 4, 0, "Entrez: References", NULL, ERMediaProc);
    RadioButton (erMediaGroup, "CD-ROM Drive");
    RadioButton (erMediaGroup, "Local Hard Disk");
    RadioButton (erMediaGroup, "File Server");
    SafeDisable (erMediaGroup);
#else
    g2 = HiddenGroup (sourceWindow, 2, 0, NULL);
    SetGroupSpacing (g2, 6, 5);

    esMediaGroup = NormalGroup (g2, 0, 4, "Entrez: Sequences", NULL, ESMediaProc);
    RadioButton (esMediaGroup, "CD-ROM Drive");
    RadioButton (esMediaGroup, "Local Hard Disk");
    RadioButton (esMediaGroup, "File Server");
    SafeDisable (esMediaGroup);

    erMediaGroup = NormalGroup (g2, 0, 4, "Entrez: References", NULL, ERMediaProc);
    RadioButton (erMediaGroup, "CD-ROM Drive");
    RadioButton (erMediaGroup, "Local Hard Disk");
    RadioButton (erMediaGroup, "File Server");
    SafeDisable (erMediaGroup);
#endif

    if (showPrompts) {
      p3 = StaticPrompt (sourceWindow, "Which index files do you want copied?",
                         0, 0, systemFont, 'c');
    }

    g3 = HiddenGroup (sourceWindow, 2, 0, NULL);
    idxCopyGroup = NormalGroup (g3, 2, 0, "Copy Index Files", NULL, NULL);
    seqIdxButton = CheckBox (idxCopyGroup, "Entrez: Sequences", CopyESProc);
    SafeDisable (seqIdxButton);
    refIdxButton = CheckBox (idxCopyGroup, "Entrez: References", CopyERProc);
    SafeDisable (refIdxButton);
    SafeDisable (idxCopyGroup);

    if (showPrompts) {
      p4 = StaticPrompt (sourceWindow, "What are your hardware resources?",
                         0, 0, systemFont, 'c');
    }

    g4 = HiddenGroup (sourceWindow, 4, 0, NULL);
    numCdsGroup = NormalGroup (g4, 5, 0, "Number of CD drives", NULL, NumCdsProc);
    SetGroupSpacing (numCdsGroup, 10, 2);
    RadioButton (numCdsGroup, "0");
    RadioButton (numCdsGroup, "1");
    b = RadioButton (numCdsGroup, "2");
    SafeDisable (numCdsGroup);
    swapButton = CheckBox (g4, "Disc Swapping", SwapProc);
    SafeDisable (swapButton);

    LowerNextPosition (sourceWindow, 10);

    g5 = HiddenGroup (sourceWindow, 4, 0, NULL);
    SetGroupSpacing (g5, 20, 2);
    acceptMediaButton = DefaultButton (g5, "Accept", AcceptMediaProc);
    SafeDisable (acceptMediaButton);
    PushButton (g5, "Quit", QuitButtonProc);

    AlignObjects (ALIGN_CENTER, (Nlm_HANDLE) g1, (Nlm_HANDLE) g2, (Nlm_HANDLE) g3,
                  (Nlm_HANDLE) g4, (Nlm_HANDLE) g5, (Nlm_HANDLE) p1, (Nlm_HANDLE) p2,
                  (Nlm_HANDLE) p3, (Nlm_HANDLE) p4, NULL);
    GetPosition (b, &s);
    GetPosition (swapButton, &r);
    r.top = s.top;
    r.bottom = s.bottom;
    SetPosition (swapButton, &r);
    RealizeWindow (sourceWindow);
  }

  SmartConfig ();
  RefreshSourceControls ();

  Show (sourceWindow);
}

static Boolean CheckAsnloadPath (CharPtr dirname, CharPtr subdir)

{
#ifdef ASNLOAD_NEEDED
  return (Boolean) ((FileExists (dirname, subdir, "asnmedli.l00") ||
                     FileExists (dirname, subdir, "asnmedli.l01") ||
                     FileExists (dirname, subdir, "asnmedli.l02") ||
                     FileExists (dirname, subdir, "asnmedli.l03")) &&
#ifdef NETWORK_SUPPORTED
                    (FileExists (dirname, subdir, "ni_asn.l04") ||
                     FileExists (dirname, subdir, "ni_asn.l05") ||
                     FileExists (dirname, subdir, "ni_asn.l06")) &&
#endif /* NETWORK_SUPPORTED */
                    (FileExists (dirname, subdir, "cdrom.l00") ||
                     FileExists (dirname, subdir, "cdrom.l01") ||
                     FileExists (dirname, subdir, "cdrom.l02") ||
                     FileExists (dirname, subdir, "cdrom.l03")));
#else
  return TRUE;
#endif
}

static Boolean CheckDataPath (CharPtr dirname, CharPtr subdir)

{
  return (Boolean) (FileExists (dirname, subdir, "seqcode.val"));
}


static void TempChangeAsnLoadVal (CharPtr path)
{
  TransientSetAppParam (CFG_FILE_NAME, "NCBI", "ASNLOAD", path);
}

static void SetPathsProc (void)

{
  Char  path [PATH_MAX];

#ifdef ASNLOAD_NEEDED
  StringNCpy (path, mainPath, sizeof (path));
  FileBuildPath (path, "asnload", NULL);
  asnloadPath = StringSave (path);
  TempChangeAsnLoadVal (asnloadPath);
#endif

  StringNCpy (path, mainPath, sizeof (path));
  FileBuildPath (path, "data", NULL);
  dataPath = StringSave (path);

  StringNCpy (path, mainPath, sizeof (path));
  FileBuildPath (path, "seq", NULL);
  seqValPath = StringSave (path);
  FileBuildPath (path, "index", NULL);
  seqIdxPath = StringSave (path);

  StringNCpy (path, mainPath, sizeof (path));
  FileBuildPath (path, "ref", NULL);
  refValPath = StringSave (path);
  FileBuildPath (path, "index", NULL);
  refIdxPath = StringSave (path);

  SourceWindowProc ();
}

static void CheckPathsProc (ButtoN b)

{
  Boolean  asnFound;
  Boolean  dataFound;
  Char     path [PATH_MAX];

  GetTitle (mainPathText, path, sizeof (path));
  if (StringLen (path) > 0) {
    asnFound = CheckAsnloadPath (path, "asnload");
    dataFound = CheckDataPath (path, "data");
    if (asnFound && dataFound) {
      Hide (pathWindow);
      mainPath = StringSave (path);
      SetPathsProc ();
    } else if (asnFound) {
      Message (MSG_OK, "DATA path not found");
      Show (pathWindow);
    } else if (dataFound) {
      Message (MSG_OK, "ASNLOAD path not found");
      Show (pathWindow);
    } else {
      Message (MSG_OK, "ASNLOAD and DATA paths not found");
      Show (pathWindow);
    }
  } else {
#ifdef OS_MAC
    Message (MSG_OK, "Path to ENTREZ folder not specified");
#else
    Message (MSG_OK, "Path to ENTREZ directory not specified");
#endif
    Show (pathWindow);
  }
}

static void RefreshPathControls (void)

{
  Char  path [PATH_MAX];

  GetTitle (mainPathText, path, sizeof (path));
  if (StringLen (path) > 0) {
    SafeEnable (acceptPathButton);
  } else {
    SafeDisable (acceptPathButton);
  }
}

static void FindEntrezPath (ButtoN b)

{
  Char  path [PATH_MAX];

  if (GetOutputFileName (path, sizeof (path), "rootpath")) {
    TrimFileName (path);
    SetTitle (mainPathText, path);
    RefreshPathControls ();
    if (StringLen (path) > 0) {
      if (CheckAsnloadPath (path, "asnload") && CheckDataPath (path, "data")) {
        Hide (pathWindow);
        mainPath = StringSave (path);
        SetPathsProc ();
      }
    }
  }
}

static void PathTextProc (TexT t)

{
  RefreshPathControls ();
}

static void PathWindowProc (void)

{
  GrouP  g1, g2, g3, g4;
  Char   path [PATH_MAX];

  if (pathWindow == NULL) {
    pathWindow = FixedWindow (-50, -33, -10, -10, "EntrezCf", NULL);
#ifdef WIN_MSWIN
    SetupMenus (pathWindow);
#endif
#ifdef WIN_MOTIF
    SetupMenus (pathWindow);
#endif

    SetGroupSpacing (pathWindow, 3, 15);

    g1 = HiddenGroup (pathWindow, 0, 8, NULL);
#ifdef OS_MAC
    StaticPrompt (g1, "The ENTREZ folder contains essential data, ASN.1",
                  0, 0, systemFont, 'c');
    StaticPrompt (g1, " parse tables, and the SEQ and REF folders",
                  0, 0, systemFont, 'c');
#else
#ifdef ASNLOAD_NEEDED
    StaticPrompt (g1, "The ENTREZ directory contains essential data, ASN.1",
                  0, 0, systemFont, 'c');
    StaticPrompt (g1, " parse tables, and the SEQ and REF directories",
                  0, 0, systemFont, 'c');
#else
    StaticPrompt (g1, "The ENTREZ directory contains essential data",
                  0, 0, systemFont, 'c');
    StaticPrompt (g1, " and the SEQ and REF directories",
                  0, 0, systemFont, 'c');
#endif
#endif

    g2 = HiddenGroup (pathWindow, 0, 8, NULL);
#ifdef OS_MAC
    StaticPrompt (g2, "Please enter the path to the ENTREZ folder",
                  0, 0, systemFont, 'c');
#else
    StaticPrompt (g2, "Please enter the path to the ENTREZ directory",
                  0, 0, systemFont, 'c');
#endif

    g3 = HiddenGroup (pathWindow, 2, 0, NULL);
    StaticPrompt (g3, "Path:", 0, 0, systemFont, 'l');
    ProgramPath (path, sizeof (path));
    TrimFileName (path);
    mainPathText = DialogText (g3, path, 20, PathTextProc);

    LowerNextPosition (pathWindow, 10);

    g4 = HiddenGroup (pathWindow, 4, 0, NULL);
    SetGroupSpacing (g4, 55, 2);
    acceptPathButton = DefaultButton (g4, "Accept", CheckPathsProc);
#ifdef OS_MAC
    PushButton (g4, "Find Folder", FindEntrezPath);
#else
    PushButton (g4, "Find Path", FindEntrezPath);
#endif
    if (StringLen (path) == 0) {
      SafeDisable (acceptPathButton);
    }
    PushButton (g4, "Quit", QuitButtonProc);

    AlignObjects (ALIGN_CENTER, (Nlm_HANDLE) g1, (Nlm_HANDLE) g2,
                  (Nlm_HANDLE) g3, (Nlm_HANDLE) g4, NULL);
    RealizeWindow (pathWindow);
  }
  Show (pathWindow);
}

static void TestPathProc (ButtoN b)

{
  Char     path [PATH_MAX];

  Hide (mainWindow);
  ProgramPath (path, sizeof (path));
  TrimFileName (path);
  if (StringLen (path) > 0) {
#ifdef WIN_MAC
    if (path [0] == ' ') {
      StringCpy (path, "Your hard disk name begins with a space.\n");
      StringCat (path, "Please rename your disk and configure again.");
      Message (MSG_FATAL, path);
    }
#endif
    if (CheckAsnloadPath (path, "asnload") && CheckDataPath (path, "data")) {
      mainPath = StringSave (path);
      SetPathsProc ();
    } else {
      PathWindowProc ();
    }
  } else {
    PathWindowProc ();
  }
}

static void DumpOldConfig (void)

{
  ReadSections (CFG_FILE_NAME, sectionsToDelete, (Int2) DIM(sectionsToDelete));
  ReadSections (CFG_FILE_NAME, sectionsToRead, (Int2) DIM(sectionsToRead));
}

#ifdef CONFIGURATION_EDITING_SUPPORTED
static void EditOldConfig (ButtoN b)
 
{
  askBeforeQuitting = FALSE;
  Remove (fileMenu);
  Remove (quitItem);
  Hide (mainWindow);

  EdConfigFile (CFG_FILE_NAME, NULL, 0, 0, FALSE, (VoidProc) DumpOldConfig,
                (VoidProc) QuitQueryProc);
}
#endif /* CONFIGURATION_EDITING_SUPPORTED */
 
static void MainWindowProc (void)

{
  GrouP   g;
  PaneL   p;

  if (mainWindow == NULL) {
#ifdef WIN_MAC
    SetupMenus (NULL);
#endif
    mainWindow = FixedWindow (-50, -33, -10, -10, "EntrezCf", NULL);
#ifdef WIN_MSWIN
    SetupMenus (mainWindow);
#endif
#ifdef WIN_MOTIF
    SetupMenus (mainWindow);
#endif

    p = SimplePanel (mainWindow, 30 * stdCharWidth, 12 * stdLineHeight, DrawAbout);
    g = HiddenGroup (mainWindow, 4, 0, NULL);
    SetGroupSpacing (g, 55, 2);
    DefaultButton (g, "Install", TestPathProc);
#ifdef CONFIGURATION_EDITING_SUPPORTED
    {
      ButtoN b;

      b = PushButton (g, "Edit old configuration", EditOldConfig);
      if (! OldCfgFileExists ()) {
        SafeDisable (b);
      }
    }
#endif /* CONFIGURATION_EDITING_SUPPORTED */
    PushButton (g, "Quit", QuitButtonProc);

    AlignObjects (ALIGN_CENTER, (Nlm_HANDLE) p, (Nlm_HANDLE) g, NULL);
    RealizeWindow (mainWindow);
  }
  Show (mainWindow);
}

Int2 Main (void)

{
#ifdef WIN_MAC
  fnt1 = GetFont ("Geneva", 18, TRUE, TRUE, FALSE, "");
  fnt2 = GetFont ("Monaco", 9, FALSE, FALSE, FALSE, "");
#endif
#ifdef WIN_MSWIN
  fnt1 = GetFont ("Arial", 24, TRUE, TRUE, FALSE, "");
  fnt2 = GetFont ("Courier", 12, FALSE, FALSE, FALSE, "");
#endif
#ifdef WIN_MOTIF
  fnt1 = GetFont ("Courier", 24, TRUE, TRUE, FALSE, "");
  fnt2 = GetFont ("Courier", 12, FALSE, FALSE, FALSE, "");
#endif

  showPrompts = TRUE;
#ifdef WIN_MAC
  if (screenRect.right < 513 || screenRect.bottom < 405) {
    showPrompts = FALSE;
  }
#endif
#ifdef WIN_MSWIN
  if (screenRect.bottom < 360) {
    showPrompts = FALSE;
  }
#endif

  MainWindowProc ();

  ProcessEvents ();

  mainPath = MemFree (mainPath);
  plainRootPath = MemFree (plainRootPath);
  asnloadPath = MemFree (asnloadPath);
  dataPath = MemFree (dataPath);

  seqRootPath = MemFree (seqRootPath);
  seqMountPath = MemFree (seqMountPath);
  seqCookedPath = MemFree (seqCookedPath);
  seqRawPath = MemFree (seqRawPath);
  seqValPath = MemFree (seqValPath);
  seqIdxPath = MemFree (seqIdxPath);

  refRootPath = MemFree (refRootPath);
  refMountPath = MemFree (refMountPath);
  refCookedPath = MemFree (refCookedPath);
  refRawPath = MemFree (refRawPath);
  refValPath = MemFree (refValPath);
  refIdxPath = MemFree (refIdxPath);

  return 0;
}

/*****************************************************************************
*
*   MyEntrezInfoAsnOpen (dirname)
*
*****************************************************************************/

static AsnIoPtr MyEntrezInfoAsnOpen (CharPtr dirname)

{
  CharPtr   p, buf, endpath;
  AsnIoPtr  aip = NULL;
  FILE      *fp;


  aip = NULL;
  bAppendVer = FALSE;
  upperCaseIt = FALSE;
  buf = (CharPtr) MemNew (PATH_MAX);
  p = StringMove (buf, dirname);
  FileBuildPath (buf, NULL, NULL);
  endpath = buf + StringLen (buf);
  p = StringMove (p, "cdromdat.val;1");
  p -= 2;           /* point to the semi-colon */
  *p = '\0';        /* wipe-out the semi-colon */

  if ((fp = FileOpen (buf, "rb")) == NULL) {
    *p = ';';       /* put back the semi-colon */
    if ((fp = FileOpen (buf, "rb")) != NULL) {
      bAppendVer = TRUE;
    }
  }
  if (fp == NULL) {
    StringCat (buf, ";1");
    p = endpath;
    while (*p != '\0') {
      *p = TO_UPPER (*p);
      p++;
    }
    upperCaseIt = TRUE;
    p -= 2;           /* point to the semi-colon */
    *p = '\0';        /* wipe-out the semi-colon */
    if ((fp = FileOpen (buf, "rb")) == NULL) {
      *p = ';';      /* put back the semi-colon */
      if ((fp = FileOpen (buf, "rb")) != NULL) {
        bAppendVer = TRUE;
      }
    }
  }
  if (fp != NULL) {
    aip = AsnIoNew (ASNIO_BIN_IN, fp, NULL, NULL, NULL);
  }
  MemFree (buf);
  return aip;
}

#ifdef CONFIGURATION_EDITING_SUPPORTED
/********* The following should go into the beginning of edparam.c ***********/
typedef struct setparamdescr
{
  CharPtr         filename;
  CharPtr         section;
  CharPtr         key;
  CharPtr         value;
} SetParamDescr, PNTR SetParamDescrPtr;

typedef struct env_item
{
  struct env_item PNTR next;
  CharPtr         name;
  CharPtr         comment;
  CharPtr         value;
} env_item, PNTR env_itemPtr;

typedef struct env_sect
{
  struct env_sect PNTR next;
  CharPtr         name;
  CharPtr         comment;
  struct env_item PNTR children;
} env_sect, PNTR env_sectPtr;

static DoC      doc;
static Boolean  dirtyFile;
static Boolean  redoing;
static ButtoN   okButton;
static ButtoN   cancelButton;
static TexT     sectionText;
static TexT     nameText;
static TexT     valueText;
static ButtoN   undoButton;
static FonT     displayFont = NULL;
static WindoW   promptWindow = NULL;
static WindoW   edWindow;

static env_sectPtr envList = NULL;
static CharPtr  Nlm_lastParamFile = NULL;
static CharPtr  Nlm_bottomComment = NULL;
static env_sectPtr alt_envList = NULL;

static VoidProc writeFun;
static VoidProc callerQuitProc;

static Boolean  writeValue;
static Boolean  writeFile = FALSE;
static Boolean  fileWasWritten;
static Boolean  smallScreen;
static Boolean  hasName;
static Boolean  hasValue;
static Boolean  mustExist;
static Boolean  mayExist;
static Boolean  valOffered;

static char     section[20];
static char     name[15];
static char     value[40];
static char     docfname[PATH_MAX];

static CharPtr  cfgFileName = NULL;

static ValNodePtr changelist = NULL;
static ValNodePtr lastNode = NULL;

/* always FALSE, because this file is trying to emulating MS Windows's  */
/* handling of comments in Param files; however, just change this value */
/* to TRUE to turn this approach around                                 */
static Boolean  destroyDeadComments = FALSE;



static Boolean PrivSetAppParam PROTO ((CharPtr file, CharPtr section, CharPtr type, CharPtr value));
static Boolean MyWriteConfigFile PROTO ((FILE * fp));
static env_sectPtr MyFindConfigSection PROTO ((CharPtr section));
static env_itemPtr MyFindConfigItem PROTO ((CharPtr section, CharPtr type, Boolean create));
static void MyPutComment PROTO ((CharPtr s, FILE * fp));
static void MyFreeConfigFileData PROTO ((void));
static void CopyFileData PROTO ((void));
static void FreeAltCfg PROTO ((void));
static void SwapCfg PROTO ((void));

static void saveProc PROTO ((ButtoN b));
static void DumpExtraConfig PROTO ((void));
static void FreeExtraConfig PROTO ((void));
static void quitProc PROTO ((ButtoN b));
static void cancelProc PROTO ((ButtoN b));
static void okProc PROTO ((ButtoN b));
static void PrReadProc PROTO ((TexT t));
static void PromptVal PROTO ((CharPtr title, CharPtr valueTitle));
static void changeValProc PROTO ((ButtoN b));
static void addValProc PROTO ((ButtoN b));
static void delValProc PROTO ((ButtoN b));
static void delSectProc PROTO ((ButtoN b));
static void undoProc PROTO ((ButtoN b));

/************************** END beginning of edparam.c *********************/


/*
 * This function provides a Vibrant-based editor of an NCBI configuration file,
 * where
 *   filename       -  the name of the NCBI configuration file
 *   quitName       - a label to place in the "quit" entry in the file menu; if
 *                    NULL, this defaults to "Quit"; a calling application will
 *                    typically, but not necessarily always, call this function
 *                    immediately prior to exiting
 *   width          - the width of the editor window, if 0, defaults to a good
 *                    value
 *   height         - the height of the editor window, if 0, defaults to a good
 *                    value
 *   assumeDirty    - a Boolean indicating whether or not the configuration file
 *                    is assumed to be up-to-date from the beginning, i.e.,
 *                    whether quitting (and not saving) without performing
 *                    any editing would have the same result as saving the file
 *   configWriteFun - the function which generates the "starting" point for
 *                    the configuration file, prior to editing; typically,
 *                    this consists of multiple calls to EdSetAppParam ();
 *                    configWriteFun is re-used when it is time to save the
 *                    configuration file to disk
 *   theirQuitProc  - the caller's quit procedure; this is invoked upon exit
 *                    from the editor; if NULL, then QuitProgram() is called
 */

Boolean
EdConfigFile (CharPtr filename, CharPtr quitName, Int2 width, Int2 height, Boolean assumeDirty, VoidProc configWriteFun, VoidProc theirQuitProc)

{
  FILE           *fp;
  GrouP           g;
  GrouP           buttonGroup;

  smallScreen = FALSE;
#ifdef WIN_MAC
  if (screenRect.right < 513 || screenRect.bottom < 405)
  {
    smallScreen = TRUE;
  }
#endif
#ifdef WIN_MSWIN
  if (screenRect.bottom < 360)
  {
    smallScreen = TRUE;
  }
#endif
  writeValue = EdGetReallyWrite ();
  EdSetReallyWrite (FALSE);
  fileWasWritten = FALSE;
  dirtyFile = assumeDirty;
  redoing = TRUE;
  callerQuitProc = theirQuitProc;
  writeFun = configWriteFun;
  if (writeFun != NULL)
    writeFun ();
  cfgFileName = StringSave (filename);
  TmpNam (docfname);
  fp = FileOpen (docfname, "w");
  MyWriteConfigFile (fp);
  FileClose (fp);

  if (width == 0)
    width = smallScreen ? 20 : 40;
  if (height == 0)
    height = smallScreen ? 6 : 15;

/*
  displayFont = ParseFont ("Monaco,9");
  SelectFont (displayFont);
*/

  edWindow = FixedWindow (-80, -50, -10, -10, "Configuration File Editor", NULL);
  doc = DocumentPanel (edWindow, width * stdCharWidth, height * stdLineHeight);

  buttonGroup = HiddenGroup (edWindow, 2, 0, NULL);
  SetGroupSpacing (buttonGroup, 10, 10);
  g = NormalGroup (buttonGroup, 0, 2, "File", NULL, NULL);
  SetGroupSpacing (g, 5, 5);
  PushButton (g, "Save Configuration", saveProc);
  PushButton (g, quitName == NULL ? "Quit" : quitName, quitProc);

  Advance (buttonGroup);

  g = NormalGroup (buttonGroup, 0, 2, "Edit", NULL, NULL);
  SetGroupSpacing (g, 5, 5);
  PushButton (g, "Change a value...", changeValProc);
  PushButton (g, "Add a value...", addValProc);
  PushButton (g, "Delete a value...", delValProc);
  PushButton (g, "Delete a section...", delSectProc);
  undoButton = PushButton (g, "Undo", undoProc);
  Disable (undoButton);
  RealizeWindow (edWindow);

  DisplayFile (doc, docfname, displayFont);
  Select (edWindow);
  Show (edWindow);

  return TRUE;
  /* return (fileWasWritten && !dirtyFile); */
}


/*
 * This function simulates the SetAppParam interface, but, depending upon
 * a local global flag, will either call SetAppParam, or call a local function
 * which stores the SetAppParam results internally in a data structure for
 * possible future use.
 */

Boolean
EdSetAppParam (CharPtr file, CharPtr section, CharPtr type, CharPtr value)

{
  if (EdGetReallyWrite ())
  {
    return SetAppParam (file, section, type, value);
  } else
  {
    return PrivSetAppParam (file, section, type, value);
  }
}


/*
 * This function simulates the GetAppParam interface, but instead of obtaining
 * its data from a configuration file, uses the internal data strucutres which
 * have been set-up by EdSetAppParam/PrivSetAppParam.
 */

Int2 
EdGetAppParam (CharPtr file, CharPtr section, CharPtr type, CharPtr dflt, CharPtr buf, Int2 buflen)

{
  env_itemPtr eip;
  env_sectPtr esp;
  Int2            totlen;
  Int2            bytesToAppend;

  if (buf != NULL && buflen > 0)
  {
    *buf = '\0';
    if (dflt)
      StringNCat (buf, dflt, buflen);
    if (file != NULL && section != NULL)
    {
      if (type != NULL)
      {
        eip = MyFindConfigItem (section, type, FALSE);
        if (eip != NULL)
        {
          *buf = '\0';
          StringNCat (buf, eip->value, buflen);
        }
      } else
      {                         /* return all the types in that section */
        *buf = '\0';
        totlen = 0;
        if ((esp = MyFindConfigSection (section)) != NULL)
        {
          /* traverse the children, allowing the null chars to be inserted */
          /* in between each type-name                                     */
          for (eip = esp->children; eip != NULL; eip = eip->next)
          {
            bytesToAppend = strlen (eip->name) + 1;
            bytesToAppend = MIN (bytesToAppend, buflen - totlen);
            StrNCpy (&buf[totlen], eip->name, bytesToAppend);
            totlen += bytesToAppend;
          }
          if (totlen > 0 && buf[totlen] == '\0')
            totlen--;           /* account for final null character */
        }
        return totlen;
      }
    }
  }
  return StringLen (buf);
}


/*
 * Set/clear flag indicating whether EdSetAppParam calls will be written to
 * disk, or instead written to an in-memory data structure for subsequent
 * modification.
 */

void 
EdSetReallyWrite (Boolean reallyWrite)

{
  writeFile = reallyWrite;
}


/*
 * Test value of flag indicating whether EdSetAppParam calls will be written
 * to disk, or instead written to an in-memory data structure for subsequent
 * modification.
 */

Boolean 
EdGetReallyWrite (void)

{
  return writeFile;
}


/*
 * Write all of the editing transactions which the user performed to the
 * configuration file.
 */

static void 
DumpExtraConfig (void)

{
  ValNodePtr      node;
  SetParamDescrPtr spdp;

  for (node = changelist; node != NULL; node = node->next)
  {
    spdp = (SetParamDescrPtr) node->data.ptrvalue;
    if (node->choice)
      SetAppParam (spdp->filename, spdp->section, spdp->key, spdp->value);
  }
}


/*
 * Free the memory associated with all the transactions used by
 * DumpExtraConfig ()
 */

static void 
FreeExtraConfig (void)

{
  ValNodePtr      node;
  SetParamDescrPtr spdp;
  ValNodePtr      nextnode;

  for (node = changelist; node != NULL; node = nextnode)
  {
    spdp = (SetParamDescrPtr) node->data.ptrvalue;
    MemFree (spdp->filename);
    MemFree (spdp->section);
    MemFree (spdp->key);
    MemFree (spdp->value);
    MemFree (spdp);
    nextnode = node->next;
    MemFree (node);
  }
  changelist = NULL;
}

static void 
saveProc (ButtoN b)

{
  WatchCursor ();
  EdSetReallyWrite (TRUE);
  if (writeFun != NULL)
  {
    writeFun ();                 /* write original data to the configuration
                                 * file */
  }
  DumpExtraConfig ();            /* write "extra" transactions to the config
                                 * file */
  EdSetReallyWrite (FALSE);
  dirtyFile = FALSE;            /* mark the file as clean, i.e., up-to-date */
  fileWasWritten = TRUE;
  redoing = FALSE;
  SetTitle (undoButton, "Undo");
  Disable (undoButton);
  ArrowCursor ();
}

static void 
quitProc (ButtoN b)

{
  Int2            answer;

  /* if ( there are changes which haven't been written out to the file ) */
  if (dirtyFile)
  {
    if ((answer = Message (MSG_YNC, "Save configuration before quitting?")) ==
          ANS_CANCEL)
      return;
    else
    {
      if (answer == ANS_YES)
        saveProc (NULL);
    }
  }

  Remove (edWindow);
  cfgFileName = (CharPtr) MemFree (cfgFileName);
  MyFreeConfigFileData ();
  FreeAltCfg ();
  FreeExtraConfig ();
  EdSetReallyWrite (writeValue); /* restore original value */
  if (docfname [0] != '\0') {
    FileRemove (docfname);
  }
  if (callerQuitProc == NULL) {
    QuitProgram ();
  }
  else {
    callerQuitProc ();
  }
}

static void 
cancelProc (ButtoN b)

{
  Remove (promptWindow);
}

static void 
okProc (ButtoN b)

{
  ValNodePtr      node;
  SetParamDescrPtr spdp;
  FILE           *fp;
  CharPtr         myname;
  CharPtr         myval;

  node = ValNodeNew (changelist);
  if (changelist == NULL)
    changelist = node;
  spdp = (SetParamDescrPtr) MemNew (sizeof (*spdp));
  node->data.ptrvalue = (VoidPtr) spdp;
  node->choice = TRUE;          /* used */
  lastNode = node;              /* save last entry for undo */
  spdp->filename = StringSave (cfgFileName);
  spdp->section = StringSave (section);
  myname = hasName ? StringSave (name) : NULL;
  myval = hasValue ? StringSave (value) : NULL;
  spdp->key = myname;
  spdp->value = myval;
  Remove (promptWindow);
  redoing = FALSE;
  SetTitle (undoButton, "Undo");
  Enable (undoButton);
  FreeAltCfg ();                 /* free previous copy */
  CopyFileData ();               /* backup for undo */
  PrivSetAppParam (cfgFileName, section, myname, myval);
  fp = FileOpen (docfname, "w");
  MyWriteConfigFile (fp);        /* write in-memory data to temp file */
  FileClose (fp);
  dirtyFile = TRUE;             /* indicates that config file is out-of-date */
  DisplayFile (doc, docfname, displayFont);
  Update ();
}



static void
PrReadProc (TexT t)

{
  CharPtr         myname;
  CharPtr         myval;
  char            buf[100];

  GetTitle (sectionText, section, sizeof (section));
  if (hasName)
  {
    GetTitle (nameText, name, sizeof (name));
    myname = name;
  } else
  {
    myname = NULL;
  }
  if (hasValue)
  {
    GetTitle (valueText, value, sizeof (value));
    myval = value;
  } else
  {
    myval = NULL;
  }

  if (StringLen (section) > 0 && (!hasName || StringLen (name) > 0))
  {
    /* if ( this entry _must_not_ already exist and it does ) then */
    if (!mayExist && EdGetAppParam (cfgFileName, section, myname, "", buf, sizeof buf) > 0)
    {                           /* disallow entry */
      Disable (okButton);
      return;
    }
    /* if ( this entry need not exist, or it does exist ) then */
    if (!mustExist || EdGetAppParam (cfgFileName, section, myname, "", buf, sizeof buf) > 0)
    {                           /* accept entry */
      Enable (okButton);
      if (!valOffered && mustExist && hasValue && StringLen (value) == 0)
      {                         /* set-up default, if not already done */
        valOffered = TRUE;
        StrCpy (value, buf);
        SetTitle (valueText, buf);
      }
      return;
    }
  }
  Disable (okButton);
}

static void 
PromptVal (CharPtr title, CharPtr valueTitle)

{
  GrouP           g;

  valOffered = FALSE;

  promptWindow = FixedWindow (-60, -40, -10, -10, title, NULL);
  g = HiddenGroup (promptWindow, 2, 0, NULL);
  StaticPrompt (g, "Section", 0, 0, systemFont, 'l');
  sectionText = DialogText (g, "", sizeof section - 1, PrReadProc);
  if (hasName) {
    StaticPrompt (g, "Key name", 0, 0, systemFont, 'l');
    nameText = DialogText (g, "", sizeof name - 1, PrReadProc);
  }
  if (hasValue) {
    StaticPrompt (g, valueTitle, 0, 0, systemFont, 'l');
    valueText = DialogText (g, "", sizeof value - 1, PrReadProc);
  }

  g = HiddenGroup (promptWindow, 2, 0, NULL);
  okButton = PushButton (g, "OK", okProc);
  cancelButton = PushButton (g, "Cancel", cancelProc);
  Disable (okButton);
  Show (promptWindow);
  Select (sectionText);
}

static void 
changeValProc (ButtoN b)

{
  hasName = TRUE;
  hasValue = TRUE;
  mustExist = TRUE;
  mayExist = TRUE;
  PromptVal ("Change value", "New value");
}

static void 
addValProc (ButtoN b)

{
  hasName = TRUE;
  hasValue = TRUE;
  mustExist = FALSE;
  mayExist = FALSE;
  PromptVal ("Add value", "Value");
}

static void 
delValProc (ButtoN b)

{
  hasName = TRUE;
  hasValue = FALSE;
  mustExist = TRUE;
  mayExist = TRUE;
  PromptVal ("Delete value", "");
}

static void 
delSectProc (ButtoN b)

{
  hasName = FALSE;
  hasValue = FALSE;
  mustExist = TRUE;
  mayExist = TRUE;
  PromptVal ("Delete section", "");
}

static void 
undoProc (ButtoN b)

{
  FILE           *fp;

  if (redoing) {
    SetTitle (undoButton, "Undo");
  }
  else {
    SetTitle (undoButton, "Redo");
  }
  if (lastNode != NULL) /* toggle whether or not last entry is included */
    lastNode->choice = redoing;
  redoing = ! redoing;
  SwapCfg ();                    /* fall back to previous version */
  fp = FileOpen (docfname, "w");
  MyWriteConfigFile (fp);
  FileClose (fp);
  /* display previous version */
  DisplayFile (doc, docfname, displayFont);
  Update ();
}

/*
 *   PrivSetAppParam (file, section, type, value)
 *      Simulates SetAppParam, but without actually writing to a file
 *
 */

static Boolean 
PrivSetAppParam (CharPtr file, CharPtr section, CharPtr type, CharPtr value)     /* value */

{
  env_itemPtr eip;
  env_sectPtr esp;
  Boolean         rsult;

  rsult = FALSE;
  if (file != NULL && section != NULL)
  {
    if (type != NULL)
    {
      eip = MyFindConfigItem (section, type, TRUE);
      if (eip != NULL)
      {
        if (eip->value != NULL)
        {
          eip->value = (CharPtr) MemFree (eip->value);
        }
        eip->value = StringSave (value);
        rsult = TRUE;
      }
    } else
    {                           /* wipe out that section */
      esp = MyFindConfigSection (section);
      if (esp != NULL)
      {                         /* kill section by deleting name (leave
                                 * comments) */
        esp->name = (CharPtr) MemFree (esp->name);
        rsult = TRUE;
      }
    }
  }
  return rsult;
}

static env_sectPtr 
MyFindConfigSection (CharPtr section)

{
  env_sectPtr esp;

  if (section == NULL)
    return NULL;

  for (esp = envList; esp != NULL; esp = esp->next)
  {
    if (esp->name != NULL && StringICmp (section, esp->name) == 0)
      return esp;
  }

  return NULL;
}

/*
 *
 *   MyFindConfigItem (section, type, create)
 *      finds parameter in memory structure
 *
 */

static env_itemPtr 
MyFindConfigItem (CharPtr section, CharPtr type, Boolean create)

{
  env_itemPtr eip;
  env_sectPtr esp;
  Boolean         goOn;
  env_itemPtr lastEip;
  env_sectPtr lastEsp;

  eip = NULL;
  if (section != NULL && type != NULL)
  {
    goOn = TRUE;
    esp = envList;
    lastEsp = esp;
    while (esp != NULL && goOn)
    {
      if (esp->name != NULL && StringICmp (section, esp->name) == 0)
      {
        goOn = FALSE;
      } else
      {
        lastEsp = esp;
        esp = esp->next;
      }
    }
    if (goOn && create)
    {
      if (envList != NULL)
      {
        esp = (env_sectPtr) MemNew (sizeof (env_sect));
        if (esp != NULL)
        {
          esp->name = StringSave (section);
          lastEsp->next = esp;
        }
      } else
      {
        esp = (env_sectPtr) MemNew (sizeof (env_sect));
        if (esp != NULL)
        {
          esp->name = StringSave (section);
        }
        envList = esp;
      }
    }
    if (esp != NULL)
    {
      eip = esp->children;
      if (eip != NULL)
      {
        goOn = TRUE;
        lastEip = eip;
        while (eip != NULL && goOn)
        {
          if (eip->name != NULL && StringICmp (type, eip->name) == 0)
          {
            goOn = FALSE;
          } else
          {
            lastEip = eip;
            eip = eip->next;
          }
        }
        if (goOn && create)
        {
          eip = (env_itemPtr) MemNew (sizeof (env_item));
          if (eip != NULL)
          {
            eip->name = StringSave (type);
            lastEip->next = eip;
          }
        }
      } else if (create)
      {
        eip = (env_itemPtr) MemNew (sizeof (env_item));
        if (eip != NULL)
        {
          eip->name = StringSave (type);
          esp->children = eip;
        }
      }
    }
  }
  return eip;
}

/*
 *
 *   MyWriteConfigFile (fp)
 *      writes parameters to configuration file from memory structure
 *
 */

static Boolean 
MyWriteConfigFile (FILE * fp)

{
  env_itemPtr eip;
  env_sectPtr esp;

  if (envList != NULL && fp != NULL)
  {
    esp = envList;
    while (esp != NULL)
    {
      if (!destroyDeadComments || esp->name != NULL)
      {
        MyPutComment (esp->comment, fp);
      }
      if (esp->name != NULL)
      {
        fputc ('[', fp);
        fputs (esp->name, fp);
        fputs ("]\n", fp);
      }
      eip = esp->children;
      while (eip != NULL)
      {
        if (!destroyDeadComments)
        {
          MyPutComment (eip->comment, fp);
        }
        if (esp->name != NULL && eip->name != NULL && eip->value != NULL)
        {
          if (destroyDeadComments)
          {
            MyPutComment (eip->comment, fp);
          }
          fputs (eip->name, fp);
          fputc ('=', fp);
          fputs (eip->value, fp);
          fputc ('\n', fp);
        }
        eip = eip->next;
      }
      if (esp->name != NULL)
      {
        fputc ('\n', fp);
      }
      esp = esp->next;
    }
  }
  if (fp != NULL)
    MyPutComment (Nlm_bottomComment, fp);

  return TRUE;
}

/*
 *
 *   MyFreeConfigFileData ()
 *      frees parameter structure in memory
 *
 */

static void 
MyFreeConfigFileData (void)

{
  env_itemPtr eip;
  env_sectPtr esp;
  env_itemPtr nextEip;
  env_sectPtr nextEsp;

  Nlm_bottomComment = (CharPtr) Nlm_MemFree (Nlm_bottomComment);
  if (Nlm_lastParamFile != NULL)
    Nlm_lastParamFile = (CharPtr) Nlm_MemFree (Nlm_lastParamFile);

  if (envList != NULL)
  {
    esp = envList;
    while (esp != NULL)
    {
      nextEsp = esp->next;
      eip = esp->children;
      while (eip != NULL)
      {
        nextEip = eip->next;
        Nlm_MemFree (eip->name);
        Nlm_MemFree (eip->comment);
        Nlm_MemFree (eip->value);
        Nlm_MemFree (eip);
        eip = nextEip;
      }
      Nlm_MemFree (esp->name);
      Nlm_MemFree (esp->comment);
      Nlm_MemFree (esp);
      esp = nextEsp;
    }
  }
  envList = NULL;
}

/*
 *
 *   MyPutComment ()
 *      output a comment to the config file
 *
 */

static void 
MyPutComment (CharPtr s, FILE * fp)

{
  if (s != NULL)
    fputs (s, fp);
}


/*
 * Copies from the "primary" in-memory parameter structure (envList) to
 * an alternate structure (alt_envList); this alternate structure can be used
 * to implement an "undo" operation
 */

static void 
CopyFileData (void)

{
  env_itemPtr eip;
  env_sectPtr esp;
  env_itemPtr alteip;
  env_sectPtr altesp;
  env_itemPtr lasteip;
  env_sectPtr lastesp;

  if (envList != NULL)
  {
    esp = envList;
    altesp = NULL;
    while (esp != NULL)
    {
      lastesp = altesp;
      altesp = (env_sectPtr) MemNew (sizeof (*altesp));
      if (lastesp != NULL)
        lastesp->next = altesp;
      else
        alt_envList = altesp;
      eip = esp->children;
      alteip = NULL;
      while (eip != NULL)
      {
        lasteip = alteip;
        alteip = (env_itemPtr) MemNew (sizeof (*alteip));
        if (lasteip != NULL)
          lasteip->next = alteip;
        else
          altesp->children = alteip;
        alteip->name = StringSave (eip->name);
        alteip->comment = StringSave (eip->comment);
        alteip->value = StringSave (eip->value);
        alteip->next = NULL;
        eip = eip->next;
      }
      altesp->name = StringSave (esp->name);
      altesp->comment = StringSave (esp->comment);
      esp = esp->next;
    }
  }
}


/*
 * Free the alternate configuration structure by swapping it with the
 * primary structure, freeing the primary structure, and swapping back
 */

static void 
FreeAltCfg (void)

{
  env_sectPtr esp;

  esp = envList;
  envList = alt_envList;
  MyFreeConfigFileData ();
  alt_envList = NULL;
  envList = esp;
}


/*
 * Swap alternate and primary configuration structures
 */

static void 
SwapCfg (void)

{
  env_sectPtr esp;

  esp = envList;
  envList = alt_envList;
  alt_envList = esp;
}
#endif /* CONFIGURATION_EDITING_SUPPORTED */
