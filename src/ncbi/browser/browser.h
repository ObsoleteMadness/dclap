/*   browser.h
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
* File Name:  browser.h
*
* Author:  Kans, Schuler
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.15 $
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

#ifndef _BROWSER_
#define _BROWSER_

#include <accentr.h>
#include <panels.h>
#include <document.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GROUP_SINGLE     0
#define GROUP_FIRST      1
#define GROUP_MIDDLE     2
#define GROUP_LAST       3

#define VIEW_ARTICLE     1
#define VIEW_BIBLIO      2
#define VIEW_MEDASN      3
#define VIEW_REPORT      4
#define VIEW_GNBK        5
#define VIEW_EMBL        6
#define VIEW_FAST        7
#define VIEW_FEAT        8
#define VIEW_SEQASN      9
#define VIEW_NEW        10

extern WindoW   termWindow;
extern WindoW   chosenWindow;
extern WindoW   browseWindow;
extern IteM     openAsnItem;
extern Boolean  docActive;
extern ChoicE   articleChoice;
extern ChoicE   reportChoice;
extern ChoicE   reportClass;
extern Boolean  smallScreen;
extern Boolean  showFeat;
extern Boolean  showEmbl;
extern Boolean  newReport;
extern Int2     threads;
extern IteM     openItem;
extern IteM     closeItem;
extern IteM     saveItem;
extern IteM     saveAsItem;
extern IteM     saveAllItem;
extern IteM     saveNameItem;
extern IteM     printItem;
extern TexT     term;
extern IcoN     progress;
extern Boolean  hasMed;
extern Boolean  hasProt;
extern Boolean  hasNuc;

extern void     CreateAvailList PROTO((WindoW w));
extern void     ResetAvailList PROTO((void));
extern Int2     ChooseSetting PROTO((CharPtr param, Int2 dfault));
extern void     AvailTimerProc PROTO((void));
extern void     FetchTimerProc PROTO((void));
extern void     ResetState PROTO((void));
extern void     LoadTimerPrefs PROTO((MenU m));
extern void     LoadResiduesPrefs PROTO((MenU m));
extern void     LoadOpenMedAndSeqFiles PROTO((MenU m));
extern void     LoadOpenName PROTO((MenU m));
extern void     LoadSaveName PROTO((MenU m));
extern void     LoadOpenTerm PROTO((MenU m));
extern Int2     ReturnDatabaseCode PROTO((void));
extern Int2     ReturnFieldCode PROTO((void));

extern void     CreateChosenList PROTO((WindoW w));
extern void     ResetChosenList PROTO((void));
extern void     LoadChosenList PROTO((CharPtr selection, Int2 fld, Int2 group, Boolean autoLoad));
extern void     InsertList PROTO((void));

extern void     CreateFetchedList PROTO((WindoW w));
extern void     ResetFetchedList PROTO((void));
extern void     LoadGenerationItems PROTO((MenU m));
extern void     LoadScorePrefs PROTO((MenU m));
extern void     LoadParentsPersistPrefs PROTO((MenU m));
extern void     LoadSettingsPersistPrefs PROTO((MenU m));
extern void     LoadOpenUids PROTO((MenU m));
extern void     HideBrowseWindowProc PROTO((WindoW w));
extern void     HideBrowseMenuProc PROTO((IteM i));

extern void     RetrieveDocuments PROTO((Int2 num, DocUidPtr uids, Int4Ptr wgts, Int2 dbase));

extern FonT     ChooseFont PROTO((CharPtr param, CharPtr dfault));
extern void     SetAbstractFonts PROTO((void));
extern void     LoadAbstractPrefs PROTO((MenU m));
extern void     LoadAbstractOptions PROTO((MenU m));
extern void     LoadSaveItem PROTO((MenU m));
extern void     LoadPrintItem PROTO((MenU m));
extern void     SaveOneProc PROTO((IteM i));
extern void     SaveAsOneProc PROTO((IteM i));
extern void     SaveManyProc PROTO((IteM i));
extern void     SaveAsManyProc PROTO((IteM i));
extern void     PrintOneProc PROTO((IteM i));

extern void     LoadAbstract PROTO((CharPtr title, DocUid uid));
extern void     SaveAbstract PROTO((CharPtr dfault, DocUid uid, Boolean stdLoc));

extern void     LoadSequence PROTO((CharPtr title, DocUid uid, Boolean is_na));
extern void     SaveSequence PROTO((CharPtr dfault, DocUid uid, Boolean is_na, Boolean stdLoc));

extern void     LoadReportOptions PROTO((MenU m));
extern void     LoadReportClass PROTO((MenU m));
extern void     LoadSequencePrefs PROTO((MenU m));
extern void     LoadFeatOptions PROTO((MenU m));
extern void     ChangeArticle PROTO((ChoicE c));
extern void     ChangeReport PROTO((ChoicE c));
extern void     ChangeClass PROTO((ChoicE c));

extern void     ResetRetrieveButton PROTO((void));

extern void     ClearAllLists PROTO((void));
extern void     ClearHistoryList PROTO((void));
extern void     ClearDocumentLists PROTO((void));
extern void     FreeDocWindows PROTO((void));
extern void     RememberDocWindow PROTO((WindoW w, DoC d, DocUid uid, Int2 view, Boolean isSequence));
extern void     ForgetDocWindow PROTO((WindoW w));
extern DoC      FindDocWindow PROTO((WindoW w));
extern WindoW   FindUidWindow PROTO((DocUid uid, Int2 view));
extern Boolean  IsSequence PROTO((WindoW w));

#ifdef __cplusplus
}
#endif

#endif
