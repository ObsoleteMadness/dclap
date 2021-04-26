/*   netentr.c
* ===========================================================================
*
*                            PUBLIC DOMAIN NOTICE                          
*               National Center for Biotechnology Information
*                                                                          
*  This software/database is a "United States Government Work" under the   
*  terms of the United States Copyright Act.  It was written as part of    
*  the author's official duties as a United States Government employee and 
*  thus cannot be copyrighted.  This software/database is freely available 
*  to the public for use. The National Library of Medicine and the U.S.    
*  Government have not placed any restriction on its use or reproduction.  
*                                                                          
*  Although all reasonable efforts have been taken to ensure the accuracy  
*  and reliability of the software and data, the NLM and the U.S.          
*  Government do not and cannot warrant the performance or results that    
*  may be obtained by using this software or data. The NLM and the U.S.    
*  Government disclaim all warranties, express or implied, including       
*  warranties of performance, merchantability or fitness for any particular
*  purpose.                                                                
*                                                                          
*  Please cite the author in any work or product based on this material.   
*
* ===========================================================================
*
* File Name:  netentr.c
*
* Author:  Ostell, Kans, Epstein
*
* Version Creation Date:   06/02/92
*
* $Revision: 1.18 $
*
* File Description: 
*       entrez index access library for Network Entrez
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 06-08-93 Schuler     Changed datatype for Monitor from Handle to MonitorPtr
*
* ==========================================================================
*/

#include <accentr.h>
#include <ncbinet.h>
#include <netentr.h>
#include <netlib.h>
#include <netpriv.h>
#include <asnneten.h>
#include <objneten.h>

typedef struct namedItem {
    Boolean   knownToServer;
    CharPtr   term;
    Char      tmpFileName[PATH_MAX];
    DocType   type;
    DocField  field;
} NamedItem, PNTR NamedItemPtr;


static void NEAR s_NetEntrezFini PROTO((void));
static LinkSetPtr NEAR s_NetEntTLEval PROTO((ValNodePtr elst));
static Int2 NEAR s_NetDocSumListGet PROTO((DocSumPtr PNTR result, Int2 numuid, DocType type, DocUidPtr uids, Int2 defer_count));
static Int2 NEAR s_NetLinkUidList PROTO((LinkSetPtr PNTR result, DocType type, DocType link_to_type, Int2 numuid, DocUidPtr uids, Boolean mark_missing));
static LinkSetPtr NEAR s_NetUidLinks PROTO((DocType type, DocUid uid, DocType link_to_type));
static Int2 NEAR s_NetTermListByTerm PROTO((DocType type, DocField field, CharPtr term, Int2 numterms, TermListProc proc, Int2Ptr first_page));
static Int2 NEAR s_NetTermListByPage PROTO((DocType type, DocField field, Int2 page, Int2 numpage, TermListProc proc));
static Boolean NEAR s_NetEntrezFindTerm PROTO((DocType type, DocField field, CharPtr term, Int4Ptr spcl, Int4Ptr totl));
static void NEAR s_NetEntrezCreateNamedUidList PROTO((CharPtr term, DocType type, DocField field, Int4 num, DocUidPtr uids));
static Int2 NEAR s_NetEntSeqEntryListGet PROTO((SeqEntryPtr PNTR result, Int2 numuid, DocUidPtr uids, Int2 retcode, Boolean mark_missing));
static
Int2 NEAR s_NetEntMedlineEntryListGet PROTO((MedlineEntryPtr PNTR result, Int2 numuid,
                                    DocUidPtr uids, Boolean mark_missing));

static NamedListPtr KnownNamedTerm PROTO((CharPtr term, Boolean onlyIfNotKnown, DocType type, DocField field));
static void CleanupNamedUidLists PROTO((void));
static AsnTypePtr NetEntReadAsn PROTO((void));
static Boolean ReestablishNetEntrez PROTO((void));
static Boolean SwapInEntrez PROTO((VoidPtr med));
static void countChars PROTO((CharPtr str));

static NI_HandPtr Entrez_ni = NULL;
static AsnIoPtr   Entrez_asnin = NULL;
static AsnIoPtr   Entrez_asnout = NULL;
static EntrezInfoPtr vi = NULL;
static Boolean    loaded = FALSE;
static CharPtr    infoBuf = NULL;
static size_t     charCount = 0;
static ValNodePtr namedTerms = NULL;
static CharPtr    netEntrezVersion = "5.0b1";
static CharPtr    userApplId = NULL;
static Boolean    userWarnings;

extern EntrezInfoPtr LIBCALL EntrezInfoAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
EntrezInfoPtr NetEntrezGetInfo PROTO((void));


/*****************************************************************************
*
*   NetEntrezInit ()
*
*****************************************************************************/

Boolean CDECL NetEntrezInit (CharPtr appl_id, Boolean no_warnings)

{
    AsnIoPtr   asnin;
    AsnIoPtr   asnout;
    AsnTypePtr atp;
    MediaPtr   media;
    DataVal    av;
    CharPtr    versionId;

    if (!loaded)
    {
#ifdef NET_ENTR_DYNAMIC_LOAD
        if (!AsnLoad())
            return FALSE;
#endif
        loaded = TRUE;
    }

    if (! NetInit())
        return FALSE;

    userWarnings = no_warnings;

    Entrez_ni = NetServiceGet("ENTR_LINK", "Entrez", SwapInEntrez, Entrez_ni);
    if (Entrez_ni == NULL)
    {
        Message(MSG_ERROR, "NI_ServiceGet [%s] (%s)", ni_errlist[ni_errno], ni_errtext);
        Entrez_asnin = NULL;
        Entrez_asnout = NULL;
        NetEntrezFini();
        return FALSE;
    }

    asnin = Entrez_ni->raip;
    asnout = Entrez_ni->waip;

    Entrez_asnin = asnin;
    Entrez_asnout = asnout;

    /**********************************************************/

    AsnWrite(asnout, ENTREZ_REQUEST, NULL);
    AsnStartStruct(asnout, ENTREZ_REQUEST_init);
    if (netEntrezVersion != NULL) {
        if (appl_id == NULL) {
            av.ptrvalue = netEntrezVersion;
            AsnWrite(asnout, ENTREZ_REQUEST_init_version, &av);
        } else {
            versionId = MemNew(StrLen(netEntrezVersion) + StrLen(appl_id) + 4);
            sprintf (versionId, "%s (%s)", netEntrezVersion, appl_id);
            userApplId = StringSave(appl_id);
            av.ptrvalue = versionId;
            AsnWrite(asnout, ENTREZ_REQUEST_init_version, &av);
            MemFree (versionId);
        }
    }
    AsnEndStruct(asnout, ENTREZ_REQUEST_init);
    AsnIoReset(asnout);


    if ((atp = NetEntReadAsn()) == NULL)
    {
        return FALSE;
    }
    else
    {
        AsnReadVal(asnin, atp, NULL);   /* read the NULL */
        atp = AsnReadId(asnin, amp, atp);
        if (atp == ENTREZ_BACK_init_e_info)
        {
            if (vi != NULL)
            {
                EntrezInfoFree (vi);
                vi = NULL;
            }
            vi = EntrezInfoAsnRead(asnin, atp);
            if ((media = GetCurMedia()) != NULL &&
                media->media_type == MEDIUM_NETWORK)
            {
                media->entrez_info = vi;
            }
            atp = AsnReadId(asnin, amp, atp);
        }
        if (atp != ENTREZ_BACK_init)
            return FALSE;
        AsnReadVal(asnin, atp, NULL);   /* read the NULL */
        ConfigInit();
        return  TRUE;
    }
}

/*****************************************************************************
*
*   NetEntrezFini ()
*
*****************************************************************************/

static void NEAR s_NetEntrezFini (void)

{
    AsnTypePtr atp;
    AsnIoPtr asnout = Entrez_asnout;
    AsnIoPtr asnin = Entrez_asnin;

    if (asnout != NULL && asnin != NULL)
    {
        AsnWrite(asnout, ENTREZ_REQUEST, NULL);
        AsnWrite(asnout, ENTREZ_REQUEST_fini, NULL);
        AsnIoReset(asnout);
 
        if ((atp = NetEntReadAsn()) != NULL)
            AsnReadVal(asnin, atp, NULL);   /* read the NULL */
    }

    if (infoBuf != NULL)
    {
        MemFree(infoBuf);
        infoBuf = NULL;
    }

    if (userApplId != NULL)
    {
        MemFree(userApplId);
        userApplId = NULL;
    }

    NI_ServiceDisconnect(Entrez_ni);
    NetFini();
    ConfigFini();
}

/* the only thing done here is to suppress errors */
void CDECL NetEntrezFini (void)

{
    short erract;
    ErrDesc err;

    ErrGetOpts(&erract, NULL);
    ErrSetOpts(ERR_IGNORE, 0);
    ErrFetch(&err);

    s_NetEntrezFini();

    ErrSetOpts(erract, 0);
    ErrFetch(&err);

    CleanupNamedUidLists ();
}

/*****************************************************************************
*
*   NetEntrezGetInfo()
*       returns global Entrez information
*
*****************************************************************************/
EntrezInfoPtr CDECL
NetEntrezGetInfo(void)
{
    return vi;
}

/*****************************************************************************
*
*   NetEntrezDetailedInfo()
*       returns NULL, or a string to some descriptive text
*
*****************************************************************************/
CharPtr CDECL
NetEntrezDetailedInfo(void)
{
    charCount = 0;
    DumpNetStats(SUBSYS_CLI_ENTREZ, countChars);

    if (infoBuf != NULL)
        MemFree(infoBuf);
    infoBuf = MemNew(charCount + 340);
    GetClientInfo(infoBuf);
    return infoBuf;
}

/*****************************************************************************
*
*   NetEntGetMaxLinks()
*       returns max links in link set allowed by system
*
*****************************************************************************/

static Int4 maxlinks = -1;
Int4 NetEntGetMaxLinks(void)
{
    AsnTypePtr atp;
    DataVal av;

    if (maxlinks == -1 && Entrez_asnout != NULL)
    {
        AsnWrite(Entrez_asnout, ENTREZ_REQUEST, NULL);
        AsnWrite(Entrez_asnout, ENTREZ_REQUEST_maxlinks, NULL);
        AsnIoReset(Entrez_asnout);
        if ((atp = NetEntReadAsn()) != NULL)
            AsnReadVal(Entrez_asnin, atp, &av);
        maxlinks = av.intvalue;
    }

    return maxlinks;
}

/*****************************************************************************
*
*   NetEntTLNew ()
*
*****************************************************************************/

ValNodePtr CDECL NetEntTLNew (DocType type)

{
  ValNodePtr anp;

  anp = ValNodeNew(NULL);
  anp->data.intvalue = (Int4)type;
  return anp;
}

/*****************************************************************************
*
*   NetEntTLAddTerm (elst, term, type, field, special)
*       Adds a term node to a boolean algebraic term query.
*
*****************************************************************************/

ValNodePtr CDECL NetEntTLAddTerm(ValNodePtr elst, CharPtr term, DocType type,
                                  DocField field, Boolean special)

{
    ValNodePtr anp;
    TermDataPtr termdatp;
    NamedListPtr nlp;
  
    anp = NULL;
    if (elst != NULL) {
        if (elst->data.intvalue != (Int4)type)
            return NULL;
        anp = ValNodeNew (elst);
        if (anp != NULL) {
            if ((nlp = KnownNamedTerm(term, TRUE, type, field)) != NULL &&
                nlp->uids != NULL) {
                NetEntrezCreateNamedUidList (term, type, field, nlp->uids->numid,
                                             nlp->uids->ids);
            }
  
            if (special) {
                anp->choice = SPECIALTERM;
            } else {
                anp->choice = TOTALTERM;
            }
  
            termdatp = (TermDataPtr) MemNew(sizeof(TermData));
            termdatp->field = field;
            termdatp->type = type;
            termdatp->term = StringSave(term);
            anp->data.ptrvalue = (Pointer) termdatp;
        }
    }
    LOG_STAT(SUBSYS_CLI_ENTREZ, "Boolean terms", 1);
    return anp;
}

/*****************************************************************************
*
*   NetEntTLFree (elst)
*       Frees a boolean algebraic term query list.
*
*****************************************************************************/

ValNodePtr CDECL NetEntTLFree (ValNodePtr elst)

{
    ValNodePtr anp;
    TermDataPtr trmp;

    if (elst != NULL) {
        elst->data.intvalue = 0;   /* clear type */
        for (anp = elst->next; anp != NULL; anp = anp->next) {
            if (anp->choice == SPECIALTERM || anp->choice == TOTALTERM) {
                trmp = (TermDataPtr) anp->data.ptrvalue;
                MemFree (trmp->term);
                trmp->term = NULL;
            }
        }
        ValNodeFreeData (elst);
    }
    return NULL;
}

/*****************************************************************************
*
*   NetEntTLEval (elst)
*       Evaluates a boolean algebraic term query list, returning a pointer to
*       a ByteStore containing the resultant unique identifiers.
*
*****************************************************************************/

static LinkSetPtr NEAR s_NetEntTLEval (ValNodePtr elst)

{
    LinkSetPtr lsp = NULL;
    AsnTypePtr atp;
    DataVal av;

    if (elst->next == NULL) /* expression without terms */
        return NULL;
    AsnWrite(Entrez_asnout, ENTREZ_REQUEST, NULL);
    AsnStartStruct(Entrez_asnout, ENTREZ_REQUEST_eval);
    av.intvalue = elst->data.intvalue;
    AsnWrite(Entrez_asnout, ENTREZ_TERMGET_cls, &av);
    BoolExprAsnWrite (elst->next, Entrez_asnout, ENTREZ_TERMGET_terms);
    AsnEndStruct(Entrez_asnout, ENTREZ_REQUEST_eval);
    AsnIoReset(Entrez_asnout);

    if ((atp = NetEntReadAsn()) == NULL)
        return NULL;

    AsnReadVal(Entrez_asnin, atp, NULL); /* discard data value */
    atp = AsnReadId(Entrez_asnin, amp, atp); /* read choice */
    if (atp == ENTREZ_BACK_eval_link_set)
        lsp = LinkSetAsnRead(Entrez_asnin, atp);
    else { /* count only; no UIDs */
        lsp = LinkSetNew();
        AsnReadVal(Entrez_asnin, atp, &av);
        lsp->num = av.intvalue;
    }
    LOG_STAT(SUBSYS_CLI_ENTREZ, "Boolean expressions evaluated", 1);
    LOG_STAT(SUBSYS_CLI_ENTREZ, "Resulting UIDs from Boolean evaluation", lsp == NULL ? 0 : lsp->num);
    return lsp;
}

LinkSetPtr CDECL NetEntTLEval (ValNodePtr elst)

{
    int i;
    LinkSetPtr retval;
    short erract;
    ErrDesc err;

    for (i = 0; i < ENT_SERV_RETRIES; i++)
    {
        if (i > 0)
        {
            if (! ReestablishNetEntrez())
                break;
        }

        ErrGetOpts(&erract, NULL);
        ErrSetOpts(ERR_IGNORE, 0);
        ErrFetch(&err);
        retval = s_NetEntTLEval(elst);
        ErrSetOpts(erract, 0);
        if (! ErrFetch(&err))
            return retval; /* success */
    }

    ErrPost(CTX_UNKNOWN, 1, "NetEntTLEval failure");
    return NULL;
}

/*****************************************************************************
*
*   NetDocSumListGet (result, numuid, type, uids)
*       returns a count of entries read
*       head of linked list is in result
*
*****************************************************************************/
static Int2 NEAR s_NetDocSumListGet (DocSumPtr PNTR result, Int2 numuid,
                                      DocType type, DocUidPtr uids,
                                      Int2 defer_count)

{
    DataVal av;
    AsnTypePtr atp;
    MlSummaryListPtr mslp;
    SeqSummaryListPtr sslp;
    EntrezIdsPtr eip;
    Int2 retval;
    EntrezDocGetPtr edgp;

    AsnWrite(Entrez_asnout, ENTREZ_REQUEST, NULL);
    edgp = EntrezDocGetNew();
    edgp->ids = EntrezIdsNew();
    eip = edgp->ids;
    edgp->cls = type;
    edgp->mark_missing = FALSE;
    eip->numid = numuid;
    eip->ids = uids;
    edgp->defer_count = defer_count;
    EntrezDocGetAsnWrite(edgp, Entrez_asnout, ENTREZ_REQUEST_docsum);
    eip->ids = NULL; /* for clean memory free */
    EntrezDocGetFree(edgp);

    AsnIoReset(Entrez_asnout);

    if ((atp = NetEntReadAsn()) == NULL)
        return -1;

    if (AsnReadVal(Entrez_asnin, atp, NULL) <= 0)
        return -1;

    if (type == TYP_ML)
    {
        if ((atp = AsnReadId(Entrez_asnin, amp, atp)) != ENTREZ_BACK_docsum_ml)
            return -1;
        mslp = MlSummaryListAsnRead(Entrez_asnin, atp);
        if (mslp == NULL)
            return -1;
        MemCopy (result, mslp->data, (size_t) mslp->num * sizeof(DocSumPtr));
        retval = (Int2) mslp->num;
        MemFree(mslp->data);
        mslp->data = NULL; /* to avoid freeing actual DocSums */
        MlSummaryListFree (mslp);
    }
    else
    {
        if ((atp = AsnReadId(Entrez_asnin, amp, atp)) != ENTREZ_BACK_docsum_seq)
            return -1;
        sslp = SeqSummaryListAsnRead(Entrez_asnin, atp);
        if (sslp == NULL)
            return -1;
        MemCopy (result, sslp->data, (size_t) sslp->num * sizeof(DocSumPtr));
        retval = sslp->num;
        MemFree(sslp->data);
        sslp->data = NULL; /* to avoid freeing actual DocSums */
        SeqSummaryListFree (sslp);
    }

    LOG_STAT(SUBSYS_CLI_ENTREZ, "Document summaries retrieved", retval);
    LOG_STAT(SUBSYS_CLI_ENTREZ, "Document summaries requested", numuid);

    return retval;
}

Int2 CDECL NetDocSumListGet (DocSumPtr PNTR result, Int2 numuid,
                          DocType type, DocUidPtr uids, Int2 defer_count)
{
    int i;
    int retval;
    short erract;
    ErrDesc err;

    /* default is to obtain these from the Entrez server */

    for (i = 0; i < ENT_SERV_RETRIES; i++)
    {
        if (i > 0)
        {
            if (! ReestablishNetEntrez())
                break;
        }

        ErrGetOpts(&erract, NULL);
        ErrSetOpts(ERR_IGNORE, 0);
        ErrFetch(&err);
        retval = s_NetDocSumListGet (result, numuid, type, uids, defer_count);
        ErrSetOpts(erract, 0);
        if (! ErrFetch(&err))
        {
            goto expand_uids;
        }
    }

    ErrPost(CTX_UNKNOWN, 1, "DocSumGet failure");
    return -1;

expand_uids:
    if (retval < 0 || retval == numuid)
        return retval;

    {
        Int2 i,j;
        DocSumPtr dsp;
#define USE_TEMP_MEMORY
#ifdef USE_TEMP_MEMORY
        DocSumPtr PNTR temp;
    
        /* some UIDs are missing; we must find which ones and insert NULLs */
        if ((temp = MemNew(sizeof(DocSumPtr) * numuid)) == NULL)
        {
            ErrPost(CTX_UNKNOWN, 1, "DocSumGet failure <memory allocation>");
            return -1;
        }
    
        for (i = 0; i < numuid; i++)
        {
            temp[i] = result[i];
            result[i] = NULL;
        }

        for (i = 0, j = 0; i < retval; i++) {
            dsp = temp[i];
            if (dsp != NULL) {
                while (j < numuid && dsp->uid != uids[j]) {
                    j++;
                }
                if (j < numuid) {
                    result[j] = dsp;
                    j++;
                }
            }
        }

        MemFree (temp);
#else
        /* more dangerous algorithm ... operates on result in-place */
        for (i = retval - 1, j = numuid - 1; i >= 0; i--) {
            dsp = result[i];
            if (dsp != NULL) {
                while (j >= 0 && dsp->uid != uids[j]) {
                    result[j] = NULL;
                    j--;
                }
                if (j >= 0)
                {
                    result[j] = dsp;
                    j--;
                }
            }
        }

        for (; j >= 0; j--)
        { /* null out any unassigned bottom entries */
            result[j] = NULL;
        }
#endif /* USE_TEMP_MEMORY */

        return retval;
    }
}
        

/*****************************************************************************
*
*   DocSumPtr NetDocSum(type, uid)
*
*****************************************************************************/

DocSumPtr CDECL NetDocSum (DocType type, DocUid uid)
{
    DocSumPtr dsp = NULL;

    NetDocSumListGet(&dsp, 1, type, &uid, 0);
    return dsp;
}

/*****************************************************************************
*
*   NetLinkUidList(type, link_to_type, numuid, uids)
*       returns count of input uids processed
*       returns -1 on error
*       if neighbors (type == link_to_type)
*           sums weights for same uids
*       if (more than EntrezUserMaxLinks() uids, frees uids and weights,
*           but leaves num set)
*
*****************************************************************************/
static
Int2 NEAR s_NetLinkUidList (LinkSetPtr PNTR result, DocType type,
                              DocType   link_to_type, Int2 numuid, DocUidPtr uids,
                              Boolean mark_missing)

{
    LinkSetGetPtr lsgp;
    MarkedLinkSetPtr mls;
    AsnTypePtr atp;
    Int2 retval;

    lsgp = LinkSetGetNew();
    lsgp->query_cls = type;
    lsgp->link_to_cls = link_to_type;
    lsgp->max = EntrezGetUserMaxLinks();
    lsgp->mark_missing = mark_missing;
    lsgp->query_size = numuid;
    lsgp->query = MemNew(sizeof(DocUid) * numuid);

    MemCopy (lsgp->query, uids, sizeof(DocUid) * numuid);
    AsnWrite(Entrez_asnout, ENTREZ_REQUEST, NULL);
    LinkSetGetAsnWrite(lsgp, Entrez_asnout, ENTREZ_REQUEST_linkuidlist);
    AsnIoReset(Entrez_asnout);

    LinkSetGetFree (lsgp);

    if ((atp = NetEntReadAsn()) == NULL)
        return -1;
    if ((mls = MarkedLinkSetAsnRead(Entrez_asnin, atp)) == NULL)
        return -1;
    *result = mls->link_set;
    retval = mls->uids_processed;
    if (mark_missing && mls->marked_missing != NULL)
        MemCopy (uids, mls->marked_missing->ids, numuid * sizeof(DocUid));
    mls->link_set = NULL; /* for clean memory free */
    MarkedLinkSetFree (mls);

    LOG_STAT(SUBSYS_CLI_ENTREZ, "UID attempted for neighbors", numuid);
    LOG_STAT(SUBSYS_CLI_ENTREZ, "UID processed for neighbors", retval);
    LOG_STAT(SUBSYS_CLI_ENTREZ, "UID neighbors found", *result == NULL ? 0 : (*result)->num);
    return retval;
}

Int2 CDECL NetLinkUidList (LinkSetPtr PNTR result, DocType type,
                            DocType link_to_type, Int2 numuid, DocUidPtr uids,
                            Boolean mark_missing)
{
    int i;
    int retval;
    short erract;
    ErrDesc err;
    DocUidPtr local_uids;

    /* make a local copy, to handle modifications */
    local_uids = MemNew(sizeof(DocUid) * numuid);

    for (i = 0; i < ENT_SERV_RETRIES; i++)
    {
        MemCopy(local_uids, uids, sizeof(DocUid) * numuid);
        if (i > 0)
        {
            if (! ReestablishNetEntrez())
                break;
        }

        ErrGetOpts(&erract, NULL);
        ErrSetOpts(ERR_IGNORE, 0);
        ErrFetch(&err);
        retval = s_NetLinkUidList(result, type, link_to_type, numuid,
                      local_uids, mark_missing);
        ErrSetOpts(erract, 0);
        if (! ErrFetch(&err))
        {
            MemCopy(uids, local_uids, sizeof(DocUid) * numuid);
            MemFree(local_uids);
            return retval; /* success */
        }
    }

    MemFree(local_uids);
    ErrPost(CTX_UNKNOWN, 1, "NetLinkUidList failure");
    return -1;
}

/*****************************************************************************
*
*   NetUidLinks()
*       retrieves links to other uids
*
*****************************************************************************/
static
LinkSetPtr NEAR s_NetUidLinks(DocType type, DocUid uid, DocType link_to_type)

{
    LinkSetPtr lsp;
    LinkSetGetPtr lsgp;
    MarkedLinkSetPtr mls;
    AsnTypePtr atp;

    lsgp = LinkSetGetNew();
    lsgp->query_cls = type;
    lsgp->link_to_cls = link_to_type;
    lsgp->max = EntrezGetUserMaxLinks();
    lsgp->mark_missing = FALSE;
    lsgp->query_size = 1;
    lsgp->query = MemNew(sizeof(DocUid));
    lsgp->query[0] = uid;

    AsnWrite(Entrez_asnout, ENTREZ_REQUEST, NULL);
    LinkSetGetAsnWrite(lsgp, Entrez_asnout, ENTREZ_REQUEST_uidlinks);
    AsnIoReset(Entrez_asnout);

    LinkSetGetFree (lsgp);

    if ((atp = NetEntReadAsn()) == NULL)
        return NULL;
    if ((mls = MarkedLinkSetAsnRead(Entrez_asnin, atp)) == NULL)
        return NULL;
    lsp = mls->link_set;
    mls->link_set = NULL; /* for clean memory free */
    MarkedLinkSetFree (mls);

    return lsp;
}

LinkSetPtr CDECL NetUidLinks(DocType type, DocUid uid, DocType link_to_type)
{
    int i;
    LinkSetPtr retval;
    short erract;
    ErrDesc err;

    for (i = 0; i < ENT_SERV_RETRIES; i++)
    {
        if (i > 0)
        {
            if (! ReestablishNetEntrez())
                break;
        }

        ErrGetOpts(&erract, NULL);
        ErrSetOpts(ERR_IGNORE, 0);
        ErrFetch(&err);
        retval = s_NetUidLinks(type, uid, link_to_type);
        ErrSetOpts(erract, 0);
        if (! ErrFetch(&err))
            return retval; /* success */
    }

    ErrPost(CTX_UNKNOWN, 1, "NetUidLinks failure");
    return NULL;
}


/* The number of pages or terms to be obtain from the network in one request */
#define NUM_NET_PAGES       4
#define NUM_NET_TERMS       50

/* The maximum number of terms to obtain in one TermListByTerm; this value   */
/* need not depend upon NUM_NET_TERMS                                        */
#define OBTAIN_IN_ONE_READ 100

/*****************************************************************************
*
*   NetTermListByTerm (type, field, term, numterms, proc, first_page)
*       Gets Terms starting with page near term
*       returns number of complete pages read
*       sets first_page to first page read
*
*****************************************************************************/
static
Int2 NEAR s_NetTermListByTerm (DocType type, DocField field, CharPtr term,
                                 Int2 numterms, TermListProc proc,
                                 Int2Ptr first_page)

{
    AsnIoPtr asnin = Entrez_asnin;
    AsnIoPtr asnout = Entrez_asnout;
    int i;
    TermRespPtr p;
    TermByTermPtr tbt;
    TermByPagePtr tbp;
    AsnTypePtr atp;
    extern TermRespPtr TermRespAsnRead();
    struct termresp PNTR termp;
    int retval = 0;
    int first_read = TRUE;
    Boolean done = FALSE;
    int next_page;
    Int4 max_terms;
    int terms_to_read;

    if (numterms == 0) /* 0 ==> infinity */
        max_terms = INT4_MAX;
    else
        max_terms = numterms;

    while (! done) {
        terms_to_read = MIN(max_terms, NUM_NET_TERMS);
        if (first_read)
        {
            tbt = TermByTermNew();
    
            AsnWrite(Entrez_asnout, ENTREZ_REQUEST, NULL);
            tbt->type = type;
            tbt->fld = field;
            tbt->term = StringSave(term);

            /* For efficiency, if the caller has only requested a few terms, */
            /* then obtain them all in one shot; also note that if we're not */
            /* going to obtain all of the requested terms in a single shot,  */
            /* then num_terms must be 0, because we need to read an integral */
            /* number of pages.                                              */
            if (max_terms <= OBTAIN_IN_ONE_READ)
                tbt->num_terms = max_terms;
            else
                tbt->num_terms = 0; /* bounded by # of pages, not terms */

            TermByTermAsnWrite(tbt, Entrez_asnout, ENTREZ_REQUEST_byterm);
            TermByTermFree (tbt);
        }
        else { /* we already know what page */
            tbp = TermByPageNew();
    
            AsnWrite(Entrez_asnout, ENTREZ_REQUEST, NULL);
            tbp->type = type;
            tbp->fld = field;
            tbp->page = next_page;
            tbp->num_pages = NUM_NET_PAGES;
            TermByPageAsnWrite(tbp, Entrez_asnout, ENTREZ_REQUEST_bypage);
            TermByPageFree (tbp);
        }

        AsnIoReset(Entrez_asnout);
    
        /* now, read back response */
        if ((atp = NetEntReadAsn()) == NULL)
            return -1;
        if ((p = TermRespAsnRead(asnin, atp)) == NULL)
            return -1;
    
        if (first_read)
        {
            *first_page = p->first_page;
            next_page = p->first_page;
            first_read = FALSE;
        }

        if (p->num_terms == 0) /* end of file */
            done = TRUE;

        termp = p->termresp;
    
        LOG_STAT(SUBSYS_CLI_ENTREZ, "Terms read (by term)", p->num_terms);
        for (i = 0; i < p->num_terms && !done; i++, termp++)
        {
            /* StringSave() is needed because application frees the term */
            if (! proc(termp->term, termp->special_count, termp->total_count) )
                done = TRUE;
            termp->term = NULL; 
        }
    
        retval += p->num_pages_read;
        TermRespFree (p);

        max_terms -= p->num_terms;
        next_page += p->num_pages_read;

        if (max_terms <= 0)
            done = TRUE;
    }

    LOG_STAT(SUBSYS_CLI_ENTREZ, "Term pages read (by term)", retval);
    return retval; /* page count */
}

Int2 CDECL NetTermListByTerm (DocType type, DocField field, CharPtr term,
                               Int2 numterms, TermListProc proc,
                               Int2Ptr first_page)
{
    int i;
    int retval;
    short erract;
    ErrDesc err;

    for (i = 0; i < ENT_SERV_RETRIES; i++)
    {
        if (i > 0)
        {
            if (! ReestablishNetEntrez())
                break;
        }

        ErrGetOpts(&erract, NULL);
        ErrSetOpts(ERR_IGNORE, 0);
        ErrFetch(&err);
        retval = s_NetTermListByTerm (type, field, term, numterms,
                                      proc, first_page);
        ErrSetOpts(erract, 0);
        if (! ErrFetch(&err))
            return retval; /* success */
    }

    ErrPost(CTX_UNKNOWN, 1, "NetTermListByTerm failure");
    return 0;
}

/*****************************************************************************
*
*   NetTermListByPage (type, field, page, numpage, proc)
*       Gets terms starting at page, for numpage, by calling proc
*       returns number of complete pages read
*
*****************************************************************************/

static
Int2 NEAR s_NetTermListByPage (DocType type, DocField field, Int2 page,
                                 Int2 numpage, TermListProc proc)

{
    AsnIoPtr asnin = Entrez_asnin;
    AsnIoPtr asnout = Entrez_asnout;
    TermRespPtr p;
    int i;
    struct termresp PNTR termp;
    TermByPagePtr tbp;
    AsnTypePtr atp;
    Boolean done = FALSE;
    int retval = 0;
    int next_page = page;
    int pages_to_read;
    Int4 max_pages;

    if (numpage == 0) /* 0 ==> infinity */
        max_pages = INT4_MAX;
    else
        max_pages = numpage;

    while (! done) {
        pages_to_read = MIN(max_pages, NUM_NET_PAGES);
        tbp = TermByPageNew();
    
        AsnWrite(Entrez_asnout, ENTREZ_REQUEST, NULL);
        tbp->type = type;
        tbp->fld = field;
        tbp->page = next_page;
        tbp->num_pages = pages_to_read;
        TermByPageAsnWrite(tbp, Entrez_asnout, ENTREZ_REQUEST_bypage);
        TermByPageFree(tbp);
        AsnIoReset(Entrez_asnout);
    
        /* now, read back response */
        if ((atp = NetEntReadAsn()) == NULL)
            return -1;
        if ((p = TermRespAsnRead(asnin, atp)) == NULL)
            return -1;
    
        if (p->num_terms == 0) /* end of file */
            done = TRUE;

        termp = p->termresp;
    
        LOG_STAT(SUBSYS_CLI_ENTREZ, "Terms read (by page)", p->num_terms);
        for (i = 0; i < p->num_terms && !done; i++, termp++)
        {
            /* StringSave() is needed because application frees the term */
            if (! proc(termp->term, termp->special_count, termp->total_count) )
                done = TRUE;
            termp->term = NULL;
        }
    
        retval += p->num_pages_read;
        TermRespFree (p);

        next_page += pages_to_read;
        max_pages -= pages_to_read;

        if (max_pages <= 0)
            done = TRUE;
    }
    LOG_STAT(SUBSYS_CLI_ENTREZ, "Term pages read (by page)", retval);
    return retval; /* page count */
}

Int2 CDECL NetTermListByPage (DocType type, DocField field, Int2 page,
                               Int2 numpage, TermListProc proc)
{
    int i;
    int retval;
    short erract;
    ErrDesc err;

    for (i = 0; i < ENT_SERV_RETRIES; i++)
    {
        if (i > 0)
        {
            if (! ReestablishNetEntrez())
                break;
        }

        ErrGetOpts(&erract, NULL);
        ErrSetOpts(ERR_IGNORE, 0);
        ErrFetch(&err);
        retval = s_NetTermListByPage (type, field, page, numpage, proc);
        ErrSetOpts(erract, 0);
        if (! ErrFetch(&err))
            return retval; /* success */
    }

    ErrPost(CTX_UNKNOWN, 1, "NetTermListByPage failure");
    return 0;
}

/*****************************************************************************
*
*   NetEntrezFindTerm(type, field, term, spec, total)
*       returns count of special and total for a term
*       if term ends with  "...", does a truncated merge of the term
*
*****************************************************************************/
static
Boolean NEAR s_NetEntrezFindTerm (DocType type, DocField field,
                                    CharPtr term, Int4Ptr spcl, Int4Ptr totl)

{
    AsnIoPtr asnin = Entrez_asnin;
    AsnIoPtr asnout = Entrez_asnout;
    TermCountsPtr tcs;
    TermLookupPtr tlp;
    Boolean found;
    AsnTypePtr atp;
    
    tlp = TermLookupNew();
    tlp->type = type;
    tlp->fld = field;
    tlp->term = StringSave(term);
    AsnWrite(asnout, ENTREZ_REQUEST, NULL);
    TermLookupAsnWrite(tlp, asnout, ENTREZ_REQUEST_findterm);
    TermLookupFree(tlp);
    AsnIoReset(asnout);

    /* now, read back response */
    if ((atp = NetEntReadAsn()) == NULL)
        return FALSE;
    if ((tcs = TermCountsAsnRead(asnin, atp)) == NULL)
        return FALSE;
    LOG_STAT(SUBSYS_CLI_ENTREZ, "Term lookup attempts", 1);
    LOG_STAT(SUBSYS_CLI_ENTREZ, "Terms found (special)", tcs->spec_count);
    LOG_STAT(SUBSYS_CLI_ENTREZ, "Terms found (total)", tcs->tot_count);
    *spcl = tcs->spec_count;
    *totl = tcs->tot_count;
    found = tcs->found;

    TermCountsFree(tcs);

    return found;
}


Boolean CDECL NetEntrezFindTerm (DocType type, DocField field,
                                  CharPtr term, Int4Ptr spcl, Int4Ptr totl)

{
    int i;
    int retval;
    short erract;
    ErrDesc err;

    for (i = 0; i < ENT_SERV_RETRIES; i++)
    {
        if (i > 0)
        {
            if (! ReestablishNetEntrez())
                break;
        }

        ErrGetOpts(&erract, NULL);
        ErrSetOpts(ERR_IGNORE, 0);
        ErrFetch(&err);
        retval = s_NetEntrezFindTerm (type, field, term, spcl, totl);
        ErrSetOpts(erract, 0);
        if (! ErrFetch(&err))
            return retval; /* success */
    }

    ErrPost(CTX_UNKNOWN, 1, "NetEntrezFindTerm failure");
    return FALSE;
}


static void CleanupNamedUidLists (void)
{
    ValNodePtr node;
    NamedItemPtr nip;
    ValNodePtr nextNode;

    for (node = namedTerms; node != NULL; node = nextNode)
    {
        nip = (NamedItemPtr) node->data.ptrvalue;

        nextNode = node->next;

        if (nip != NULL) {
            FileRemove (nip->tmpFileName);
            MemFree (nip->term);
            MemFree (nip);
        }

        MemFree (node);
    }

    namedTerms = NULL;
}

/**** Check to see if a term is already known to the Network server ********/
static NamedListPtr KnownNamedTerm (CharPtr term, Boolean onlyIfNotKnown,
                                    DocType type, DocField field)
{
    static NamedList dummy;
    ValNodePtr node;
    NamedItemPtr nip;
    AsnIoPtr aip;
    NamedListPtr nlp;


    for (node = namedTerms; node != NULL; node = node->next)
    {
        nip = (NamedItemPtr) node->data.ptrvalue;

        if (nip == NULL) {
            continue;
        }

        if (StringICmp (term, nip->term) == 0 && type == nip->type &&
            field == nip->field)
        { /* match */
            if (onlyIfNotKnown) {
                if (! nip->knownToServer) {
                    /* read the file and load the data structure */
                    aip = AsnIoOpen (nip->tmpFileName, "rb");
                    nlp = NamedListAsnRead(aip, NULL);
                    AsnIoClose (aip);
                    return nlp;
                } else {
                    return NULL;
                }
            } else {
                return &dummy;
            }
        }
    }

    return NULL; /* not found */
}

/**** Creates a term node from the uid parameter ********/
static void NEAR s_NetEntrezCreateNamedUidList (CharPtr term, DocType type, DocField field, Int4 num, DocUidPtr uids)
{
    AsnIoPtr asnin = Entrez_asnin;
    AsnIoPtr asnout = Entrez_asnout;
    NamedListPtr nlp;
    AsnTypePtr atp;
    NamedItemPtr namedItem;
    AsnIoPtr aip;
    ValNodePtr node;

    nlp = NamedListNew();
    nlp->type = type;
    nlp->fld = field;
    nlp->term = StringSave(term);
    nlp->uids = EntrezIdsNew();
    nlp->uids->numid = num;
    nlp->uids->ids = (DocUidPtr) MemDup(uids, (size_t) num * sizeof(DocUid));
    AsnWrite(asnout, ENTREZ_REQUEST, NULL);
    NamedListAsnWrite(nlp, asnout, ENTREZ_REQUEST_createnamed);
    
    AsnIoReset(asnout);

    if (KnownNamedTerm(term, FALSE, type, field) == NULL) {
        node = ValNodeNew (namedTerms);
        if (namedTerms == NULL) {
          namedTerms = node;
        }
        namedItem = MemNew (sizeof(NamedItem));
        if (node != NULL && namedItem != NULL) {
            namedItem->knownToServer = TRUE;
            TmpNam (namedItem->tmpFileName);
            namedItem->term = StringSave(term);
            namedItem->type = type;
            namedItem->field = field;
            node->data.ptrvalue = (VoidPtr) namedItem;
            aip = AsnIoOpen (namedItem->tmpFileName, "wb");
            NamedListAsnWrite (nlp, aip, NULL);
            AsnIoClose (aip);
        }
    }

    NamedListFree(nlp);
 
    if ((atp = NetEntReadAsn()) == NULL)
        return;
    if (AsnReadVal(asnin, atp, NULL) < 0)   /* read the NULL */
        return;

    LOG_STAT(SUBSYS_CLI_ENTREZ, "Named terms created", 1);
    LOG_STAT(SUBSYS_CLI_ENTREZ, "Named terms created (UID count)", num);
}

void CDECL NetEntrezCreateNamedUidList (CharPtr term, DocType type, DocField field, Int4 num, DocUidPtr uids)
{
    int i;
    short erract;
    ErrDesc err;

    for (i = 0; i < ENT_SERV_RETRIES; i++)
    {
        if (i > 0)
        {
            if (! ReestablishNetEntrez())
                break;
        }

        ErrGetOpts(&erract, NULL);
        ErrSetOpts(ERR_IGNORE, 0);
        ErrFetch(&err);
        s_NetEntrezCreateNamedUidList (term, type, field, num, uids);
        ErrSetOpts(erract, 0);
        if (! ErrFetch(&err))
            return; /* success */
    }

    ErrPost(CTX_UNKNOWN, 1, "NetEntrezCreateNamedUidList failure");
    return;
}


static AsnTypePtr NetEntReadAsn(void)
{
    AsnTypePtr atp;
    DataVal av;
    AsnIoPtr asnin = Entrez_asnin;

    atp = ENTREZ_BACK;
    if ((atp = AsnReadId(asnin, amp, atp)) != ENTREZ_BACK)
    {
        ErrPost(CTX_UNKNOWN, 1, "EOF on response from Entrez server");
        return NULL;
    }
    AsnReadVal(asnin, atp, &av);
    atp = AsnReadId(asnin, amp, atp);   /* read the CHOICE */
    if (atp == ENTREZ_BACK_error)
    {
        AsnReadVal(asnin, atp, &av);
        Message (MSG_ERROR, "Error in NetEntReadAsn <%d>", av.intvalue);
        return NULL;
    }
    else
        return atp;
}

static Boolean ReestablishNetEntrez(void)
{
    Monitor *mon;
    Boolean retval;
    NamedItemPtr nip;
    ValNodePtr node;
    CharPtr uApplId = NULL;

    if (userApplId != NULL)
    { /* make a copy because this can get wiped out by NetEntrezInit() */
        uApplId = StringSave(userApplId); 
    }

    mon = MonitorStrNew("Re-establishing Entrez Service", 40);
    MonitorStrValue(mon, "Requesting Entrez service");
    NetFini();
    retval = TRUE;

    /* clear state information for named UID lists */
    for (node = namedTerms; node != NULL; node = node->next)
    {
        nip = (NamedItemPtr) node->data.ptrvalue;

        if (nip != NULL) {
            nip->knownToServer = FALSE;
        }
    }

    LOG_STAT(SUBSYS_CLI_ENTREZ, "Service Re-establishments", 1);

    if (! NetEntrezInit(uApplId, userWarnings))
    {
        MonitorStrValue(mon, "Entrez get failed; re-contacting dispatcher");
        retval = FALSE;
        if (ForceNetInit())
        { /* successfully established contact w/dispatcher */
            MonitorStrValue(mon, "Entrez get failed; re-requesting Entrez service");
            retval = NetEntrezInit(uApplId, userWarnings);
        }
        else {
            ErrPost(CTX_UNKNOWN, 1, "Unable to re-contact dispatcher");
            ErrShow();
            LOG_STAT(SUBSYS_CLI_ENTREZ, "Service Re-establishments \"retries\"", 1);
        }
    }

    MonitorFree(mon);

    if (! retval )
    {
        ErrPost(CTX_UNKNOWN, 1, "Unable to re-establish Entrez service");
        ErrShow();
    } else {
        ConfigFini(); /* to balance the extra ConfigInit() */
    }

    MemFree (uApplId);
    return retval;
}


static Boolean SwapInEntrez (VoidPtr med)
{
    MediaPtr media = (MediaPtr) med;
    NetMediaInfoPtr nmi;

    if (media == NULL || (nmi = (NetMediaInfoPtr) media->media_info) == NULL)
        return FALSE;

    if (nmi->sessionHandle == NULL)
        return FALSE;

    Entrez_ni = nmi->sessionHandle;

    Entrez_asnin = Entrez_ni->raip;
    Entrez_asnout = Entrez_ni->waip;
}

static void countChars(CharPtr str)
{
    charCount += StringLen(str) + 5;
}

/*****************************************************************************
*
*   NetEntMedlineEntryListGet (result, numuid, uids, mark_missing)
*       returns a count of entries read
*       if (mark_missing) ids which could not be located are made negative
*
*****************************************************************************/
static
Int2 NEAR s_NetEntMedlineEntryListGet (MedlineEntryPtr PNTR result, Int2 numuid,
                                    DocUidPtr uids, Boolean mark_missing)
{
    AsnTypePtr atp;
    EntrezDocGetPtr edgp;
    MedlineEntryListPtr mllp;
    int count;

    edgp = EntrezDocGetNew();

    edgp->cls = TYP_ML; /* unused */
    edgp->ids = EntrezIdsNew();
    edgp->ids->numid = numuid;
    edgp->ids->ids = uids;
    edgp->mark_missing = mark_missing;

    AsnWrite(Entrez_asnout, ENTREZ_REQUEST, NULL);
    EntrezDocGetAsnWrite(edgp, Entrez_asnout, ENTREZ_REQUEST_getmle);
    AsnIoReset(Entrez_asnout);
    edgp->ids->ids = NULL; /* for clean memory free */
    EntrezDocGetFree(edgp);

    if ((atp = NetEntReadAsn()) == NULL)
        return 0;

    mllp = MedlineEntryListAsnRead(Entrez_asnin, atp);
    MemCopy (result, mllp->data, (size_t) mllp->num * sizeof(MedlineEntryPtr));

    /* note that the structures underlying mllp->data are not freed; they */
    /* are used by the caller                                             */
    MemFree(mllp->data);
    mllp->data = NULL; /* for clean free */
    if (mark_missing && mllp->marked_missing)
    {
        MemCopy (uids, mllp->marked_missing->ids,
             (size_t) mllp->marked_missing->numid * sizeof(DocUid));
    }
    count = mllp->num;
    MedlineEntryListFree (mllp);
    LOG_STAT(SUBSYS_CLI_ENTREZ, "Medline entries retrieved", count);
    LOG_STAT(SUBSYS_CLI_ENTREZ, "Medline entries requested", numuid);
    
    return count;
}

Int2 NetEntMedlineEntryListGet (MedlineEntryPtr PNTR result, Int2 numuid,
                                    DocUidPtr uids, Boolean mark_missing)
{
    int i;
    int retval;
    short erract;
    ErrDesc err;
    DocUidPtr local_uids;

    /* make a local copy, to handle modifications */
    local_uids = MemNew(sizeof(DocUid) * numuid);

    for (i = 0; i < ENT_SERV_RETRIES; i++)
    {
        MemCopy(local_uids, uids, sizeof(DocUid) * numuid);
        if (i > 0)
        {
            if (! ReestablishNetEntrez())
                break;
        }

        ErrGetOpts(&erract, NULL);
        ErrSetOpts(ERR_IGNORE, 0);
        ErrFetch(&err);
        retval = s_NetEntMedlineEntryListGet(result, numuid, local_uids,
                                          mark_missing);
        ErrSetOpts(erract, 0);
        if (! ErrFetch(&err))
        {
            MemCopy(uids, local_uids, sizeof(DocUid) * numuid);
            MemFree(local_uids);
            return retval; /* success */
        }
    }

    MemFree(local_uids);
    ErrPost(CTX_UNKNOWN, 1, "NetMedlineEntryListGet failure");
    return -1;
}

/*****************************************************************************
*
*   NetEntSeqEntryListGet (result, numuid, uids, mark_missing)
*       returns a count of entries read
*       if (mark_missing) ids which could not be located are made negative
*
*****************************************************************************/
static
Int2 NEAR s_NetEntSeqEntryListGet (SeqEntryPtr PNTR result, Int2 numuid, DocUidPtr uids, Int2 retcode, Boolean mark_missing)
{
    AsnTypePtr atp;
    EntrezIdsPtr glp;
    SeqEntryListPtr selp;
    EntrezSeqGetPtr sgsp;
    int count;

    sgsp = EntrezSeqGetNew();
    glp = EntrezIdsNew();
    sgsp->ids = glp;

    glp->numid = numuid;
    glp->ids = uids;
    sgsp->mark_missing = mark_missing;
    sgsp->retype = retcode;

    AsnWrite(Entrez_asnout, ENTREZ_REQUEST, NULL);
    EntrezSeqGetAsnWrite(sgsp, Entrez_asnout, ENTREZ_REQUEST_getseq);
    AsnIoReset(Entrez_asnout);
    glp->ids = NULL; /* for clean memory free */
    EntrezSeqGetFree(sgsp);

    if ((atp = NetEntReadAsn()) == NULL)
        return 0;

    selp = SeqEntryListAsnRead(Entrez_asnin, atp);
    MemCopy (result, selp->data, (size_t) selp->num * sizeof(SeqEntryPtr));
    /* note that the structures underlying selp->data are not freed; they */
    /* are used by the caller                                             */
    MemFree(selp->data);
    selp->data = NULL; /* for clean free */
    if (mark_missing && selp->marked_missing)
    {
        MemCopy (uids, selp->marked_missing->ids,
             (size_t) selp->marked_missing->numid * sizeof(DocUid));
    }
    count = selp->num;
    SeqEntryListFree (selp);
    LOG_STAT(SUBSYS_CLI_ENTREZ, "Sequence entries retrieved", count);
    LOG_STAT(SUBSYS_CLI_ENTREZ, "Sequence entries requested", numuid);
    
    return count;
}

Int2 CDECL NetEntSeqEntryListGet (SeqEntryPtr PNTR result, Int2 numuid,
                               DocUidPtr uids, Int2 retcode, Boolean mark_missing)
{
    int i;
    int retval;
    short erract;
    ErrDesc err;
    DocUidPtr local_uids;

    /* make a local copy, to handle modifications */
    local_uids = MemNew(sizeof(DocUid) * numuid);

    for (i = 0; i < ENT_SERV_RETRIES; i++)
    {
        MemCopy(local_uids, uids, sizeof(DocUid) * numuid);
        if (i > 0)
        {
            if (! ReestablishNetEntrez())
                break;
        }

        ErrGetOpts(&erract, NULL);
        ErrSetOpts(ERR_IGNORE, 0);
        ErrFetch(&err);
        retval = s_NetEntSeqEntryListGet(result, numuid, local_uids, retcode,
                                      mark_missing);
        ErrSetOpts(erract, 0);
        if (! ErrFetch(&err))
        {
            MemCopy(uids, local_uids, sizeof(DocUid) * numuid);
            MemFree(local_uids);
            return retval; /* success */
        }
    }

    MemFree(local_uids);
    ErrPost(CTX_UNKNOWN, 1, "NetSeqEntryListGet failure");
    return -1;
}
