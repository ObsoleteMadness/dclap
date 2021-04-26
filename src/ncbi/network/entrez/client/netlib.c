/*   netlib.c
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
* File Name:  netlib.c
*
* Author:  Epstein
*
* Version Creation Date:   06/05/92
*
* $Revision: 1.18 $
*
* File Description: 
*       miscellaneous library for network Entrez
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/

#include <ncbi.h>
#include <asn.h>
#include <accentr.h>
#include <cdconfig.h>
#include <ncbinet.h>
#include <netentr.h>
#include <netpriv.h>
#include <asnneten.h>
#include <objmedli.h>
#include <objsset.h>
#include <objloc.h>
#include <objneten.h>

#define STAT_CHUNK 32

static Boolean loaded = FALSE;

static NetStatPtr PNTR statVector = NULL;
static Int2 numStats = 0;
static Boolean statsDisabled = FALSE;
static Int2 maxCurStats;
static Boolean reallyFinal = TRUE;

static int num_attached = 0;
static char lastDispatcher[60];
static NI_HandPtr lastEntrezServ = NULL;
static CharPtr statsPtr = NULL;
static NI_DispatcherPtr dispatcher = NULL;

static void appendStats PROTO((CharPtr s));
static Int2 NEAR statCompare PROTO((NetStatPtr s1, NetStatPtr s2));
static void NEAR FreeNetStats PROTO((void));
static Boolean SwapOutNet PROTO ((VoidPtr med));


TermRespPtr CDECL TermRespNew(void)
{
    TermRespPtr p;

    p = (TermRespPtr) MemNew(sizeof(TermResp));
    p->num_terms = 0;
    if (p != NULL)
    {
        p->termresp = NULL;
    }
    return p;
}

TermRespPtr CDECL
TermRespFree(TermRespPtr p)
{
    if (p == NULL)
        return NULL;
    if (p->termresp != NULL)
    {
        if (p->termresp->term != NULL)
            MemFree(p->termresp->term);
        MemFree (p->termresp);
    }
    return ((TermRespPtr) MemFree (p));
}

TermRespPtr CDECL TermRespAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
    DataVal av;
    AsnTypePtr atp;
    int i;
    TermRespPtr tp;
    TermPageInfoPtr tpi;

    if (aip == NULL)
        return NULL;

    if (orig == NULL)
        atp = AsnReadId(aip, amp, ENTREZ_TERM_RESP);
    else
        atp = AsnLinkType(orig, ENTREZ_TERM_RESP); /* link in local tree */

    if (atp == NULL)
        return NULL;

    tp = TermRespNew();

    if (tp == NULL)
        goto erret;

    if (AsnReadVal(aip, atp, NULL) <= 0) /* read the start struct */
        goto erret;
    if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_TERM_RESP_num_terms)
        goto erret;
    if (AsnReadVal(aip, atp, &av) < 0)
        goto erret;
    tp->num_terms = av.intvalue;
    tp->first_page = -1;

    if ((atp = AsnReadId(aip, amp, atp)) == ENTREZ_TERM_RESP_first_page)
    { /* optional */
        if (AsnReadVal(aip, atp, &av) < 0)
            goto erret;
        tp->first_page = av.intvalue;
        atp = AsnReadId(aip, amp, atp);
    }

    if (atp != ENTREZ_TERM_RESP_pages_read)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0)
        goto erret;
    tp->num_pages_read = av.intvalue;

    if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_TERM_RESP_info)
        goto erret;
    if (AsnReadVal(aip, atp, NULL) <= 0)
        goto erret;
    tp->termresp = MemNew(sizeof(struct termresp) * (size_t) tp->num_terms);
    for (i = 0; i < tp->num_terms; i++)
    {
        if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_TERM_RESP_info_E)
            goto erret;
        if ((tpi = TermPageInfoAsnRead(aip, atp)) == NULL)
            goto erret;
        tp->termresp[i].special_count = tpi->spec_count;
        tp->termresp[i].total_count = tpi->tot_count;
        tp->termresp[i].term = tpi->term;
        tpi->term = NULL; /* for clean free */
        TermPageInfoFree (tpi);
    }

    if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_TERM_RESP_info)
        goto erret;
    if (AsnReadVal(aip, atp, NULL) < 0)
        goto erret;

    atp = AsnReadId(aip, amp, atp);
    if (orig == NULL)
    {
        if (atp != ENTREZ_TERM_RESP)
            goto erret;
    }
    else { /* check for "close struct" associated with "orig" */
        if (atp != orig)
            goto erret;
    }

    if (AsnReadVal(aip, atp, NULL) <= 0) /* discard NULL */
        goto erret;
    
ret:
    AsnUnlinkType(orig);
    return tp;

erret:
    tp = TermRespFree (tp);
    goto ret;
}

Boolean CDECL
BoolExprAsnWrite(ValNodePtr elst, AsnIoPtr aip, AsnTypePtr orig)
{
    DataVal av;
    static int n = 0;
    TermDataPtr tp;
    Boolean retval = FALSE;
    AsnTypePtr atp;

    /* if (!NetEntAsnLoad())
        return FALSE; */
    
    if (aip == NULL)
        return FALSE;
    atp = AsnLinkType(orig, ENTREZ_TERM_LIST); /* link local tree */

    if (atp == NULL)
        return FALSE;

    if (elst == NULL)
    {
        AsnNullValueMsg(aip, atp);
        goto erret;
    }

    if (! AsnStartStruct(aip, atp))
        goto erret;

    for (; elst != NULL; elst = elst->next)
    {
        AsnWrite (aip, ENTREZ_TERM_LIST_E, NULL);
        switch (elst->choice) {
        case SPECIALTERM:
            tp = (TermDataPtr) elst->data.ptrvalue;

            AsnStartStruct (aip, ENTREZ_TERM_LIST_E_sp_operand);
            av.ptrvalue = tp->term;
            AsnWrite (aip, SPECIAL_OPERAND_term, &av);
            av.intvalue = tp->field;
            AsnWrite (aip, SPECIAL_OPERAND_fld, &av);
            av.intvalue = tp->type;
            AsnWrite (aip, SPECIAL_OPERAND_type, &av);
            AsnEndStruct (aip, ENTREZ_TERM_LIST_E_sp_operand);
            break;
        case TOTALTERM:
            tp = (TermDataPtr) elst->data.ptrvalue;

            AsnStartStruct (aip, ENTREZ_TERM_LIST_E_tot_operand);
            av.ptrvalue = tp->term;
            AsnWrite (aip, TOTAL_OPERAND_term, &av);
            av.intvalue = tp->field;
            AsnWrite (aip, TOTAL_OPERAND_fld, &av);
            av.intvalue = tp->type;
            AsnWrite (aip, TOTAL_OPERAND_type, &av);
            AsnEndStruct (aip, ENTREZ_TERM_LIST_E_tot_operand);
            break;
        case LPAREN:
        case RPAREN:
        case ANDSYMBL:
        case ORSYMBL:
        case BUTNOTSYMBL:
            av.intvalue = elst->choice;
            AsnWrite (aip, ENTREZ_TERM_LIST_E_operator, &av);
            break;
        default:
            break;
        }
    }


    if (! AsnEndStruct(aip, atp))
        goto erret;

    retval = TRUE;

erret:
    AsnUnlinkType(orig);
    return retval;
}

ValNodePtr CDECL
BoolExprAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
    ValNodePtr anp = NULL;
    ValNodePtr head = NULL;
    DataVal av;
    TermDataPtr tp;
    AsnTypePtr atp;
    AsnTypePtr startsym;

    /* if (!NetEntAsnLoad())
        return NULL; */
    if (aip == NULL)
        return NULL;

    if (orig == NULL)
        atp = AsnReadId(aip, amp, ENTREZ_TERM_LIST);
    else
        atp = AsnLinkType(orig, ENTREZ_TERM_LIST); /* link in local tree */

    if (atp == NULL)
        return NULL;

    if (AsnReadVal(aip, atp, NULL) < 0)
        goto erret;

    while ((atp = AsnReadId(aip, amp, atp)) == ENTREZ_TERM_LIST_E)
    {
        if (AsnReadVal(aip, atp, NULL) < 0)
            goto erret;

        atp = AsnReadId(aip, amp, atp);

        if (atp == ENTREZ_TERM_LIST_E_sp_operand ||
            atp == ENTREZ_TERM_LIST_E_tot_operand)
        {
            startsym = atp; /* save this, to be able to find "close" sym */

            anp = ValNodeNew(anp);
            if (head == NULL) /* keep track of head of list */
                head = anp;

            if (atp == ENTREZ_TERM_LIST_E_sp_operand)
                anp->choice = SPECIALTERM;
            else
                anp->choice = TOTALTERM;

            if (AsnReadVal(aip, atp, NULL) < 0)
                goto erret;

            tp = (TermDataPtr) MemNew(sizeof(TermData));
            anp->data.ptrvalue = (Pointer) tp;

            if (tp == NULL)
                goto erret;
        
            if ((atp = AsnReadId(aip, amp, atp)) == NULL) /* term */
                goto erret;
            if (AsnReadVal(aip, atp, &av) < 0)
                goto erret;
            tp->term = av.ptrvalue;

            if ((atp = AsnReadId(aip, amp, atp)) == NULL) /* fld */
                goto erret;
            if (AsnReadVal(aip, atp, &av) < 0)
                goto erret;
            tp->field = av.intvalue;

            if ((atp = AsnReadId(aip, amp, atp)) == NULL) /* type */
                goto erret;
            if (AsnReadVal(aip, atp, &av) < 0)
                goto erret;
            tp->type = av.intvalue;

            if ((atp = AsnReadId(aip, amp, atp)) != startsym)
                goto erret;
            if (AsnReadVal(aip, atp, NULL) < 0)
                goto erret;
        }
        else
        if (atp == ENTREZ_TERM_LIST_E_operator)
        {
            if (AsnReadVal(aip, atp, &av) < 0)
                goto erret;
            anp = ValNodeNew(anp);
            if (head == NULL) /* keep track of head of list */
                head = anp;
            anp->choice = av.intvalue;
        }
        else
            goto erret;
    }

    if (orig == NULL)
    {
        if (atp != ENTREZ_TERM_LIST)
            goto erret;
    }
    else {
        if (atp != orig)
            goto erret;
    }

    /* discard last value */
    if (AsnReadVal(aip, atp, NULL) < 0)
        goto erret;

    AsnUnlinkType(orig);
    return head;

erret:
    ValNodeFree (head);
    return NULL;
}

static void appendStats(CharPtr s)
{
    if (s != NULL)
    {
        StrCat(statsPtr, "    ");
        StrCat(statsPtr, s);
        StrCat(statsPtr, "\n");
        statsPtr += StringLen(statsPtr);
    }
}

extern void GetClientInfo (CharPtr buf)
{
    if (buf == NULL)
        return;

    StrCpy(buf, "NETWORK ACCESS\n  Last Dispatcher Used: ");
    StrCat(buf, lastDispatcher);
    StrCat(buf, "\n");
    if (lastEntrezServ != NULL)
    {
        StrCat(buf, "\n  Entrez service currently connected to ");
        StrCat(buf, lastEntrezServ->hostname);
        StrCat(buf, " server\n");
        if (NI_EncrAvailable())
        {
            if (lastEntrezServ->encryption != NULL)
            {
                StrCat (buf, "  Encrypted session\n");
            } else {
                StrCat (buf, "  Encryption available, but not in use\n");
            }
        }
        statsPtr = &buf[StringLen(buf)];
        DumpNetStats(SUBSYS_CLI_ENTREZ, appendStats);
    }
}

static Int2 NEAR statCompare (NetStatPtr s1, NetStatPtr s2)
{
    if (s1 == NULL || s2 == NULL)
        return 0;
    if (s1->subsys < s2->subsys)
        return -1;
    if (s1->subsys > s2->subsys)
        return 1;
    return (Int2) StrCmp(s1->label, s2->label);
}
    
static void NEAR FreeNetStats (void)
{
    NetStatPtr PNTR statptr;
    Int2 i;

    for (statptr = statVector, i = 0; i < numStats; i++, statptr++)
    {
        MemFree((*statptr)->label);
        MemFree((*statptr));
    }

    MemFree(statVector);
    statVector = NULL;
    numStats = 0;
    statsDisabled = TRUE;
}

void CDECL DumpNetStats (Int2 subsys, StatsCallBack callBack)
{
    char s[200];
    int i;
    NetStatPtr stat;
    Boolean foundFirst = FALSE;
    Boolean okSubsys;
    Int4 total;
    double mean;
    double stdev;

    for (i = 0; i < numStats; i++)
    {
        stat = statVector[i];
        if (stat == NULL)
            continue;
         
        okSubsys = stat->subsys == subsys || subsys == 0;
        if (!foundFirst)
        {
            if (okSubsys)
                foundFirst = TRUE;
            else
                continue;
        }
        if (foundFirst && !okSubsys)
            break;
        total = (Int4) (stat->total + 0.5);

        switch (stat->flags) {
        case STAT_MEAN :
            StrCpy (s, "{");
            StrCat (s, stat->label);
            if (stat->n > 0)
                mean = stat->total / stat->n;
            else
                mean = 0.0;
            sprintf (&s[strlen(s)], "} total = %ld, mean = %f", (long) total,
                     mean);
            break;
        case STAT_MEAN | STAT_STDEV :
            StrCpy (s, "{");
            StrCat (s, stat->label);
            if (stat->n > 0)
                mean = stat->total / stat->n;
            else
                mean = 0.0;
            if (stat->n <= 1)
                stdev = 0.0;
            else
                stdev = sqrt(stat->sum_of_squares / (stat->n - 1));
            sprintf (&s[strlen(s)], "} total = %ld, mean = %f, stdev = %f",
                     (long) total, mean, stdev);
            break;
        default :
            StrCpy (s, stat->label);
            sprintf (&s[strlen(s)], " = %ld", (long) total);
            break;
        }
        callBack (s);
    }
}
    

void CDECL LogNetStats (Int2 subsys, CharPtr label, Int4 dat,
                         NetStatPtr PNTR statHandle, Int2 flags)
{
    Int2 l;
    Int2 r;
    Int2 k;
    Int2 compValue;
    NetStatPtr newStat;
    NetStatPtr Stat;
    double mean;
    double tempterm;

    if (statsDisabled)
        return;

    if (statHandle == NULL || label == NULL)
        return;

    if (*statHandle == NULL)
    {
        newStat = (NetStatPtr) MemNew(sizeof(NetStat));
        newStat->subsys = subsys;
        newStat->label = StringSave(label);
        newStat->total = 0;
        newStat->n = 0;
        newStat->flags = 0;
        newStat->sum_of_squares = 0.0;
        k = 0;
        if (statVector != NULL)
        {
            l = 0;
            r = numStats - 1;
            compValue = statCompare(newStat, statVector[k]);
            while ((l <= r) && compValue != 0)
            {
                k = (l + r) / 2;
                if ((compValue = statCompare(newStat, statVector[k])) < 0)
                    r = k - 1;
                else
                    l = k + 1;
            }
            if (compValue == 0)
            { /* match */
                *statHandle = statVector[k];
                MemFree(newStat->label);
                MemFree(newStat); /* not needed */
            }
            else {
                while (compValue > 0 && k < numStats)
                {
                    k++;
                    compValue = statCompare(newStat, statVector[k]);
                }
            }
        }

        if (*statHandle == NULL)
        { /* not found previously */
            if (statVector == NULL)
            {
                if ((statVector = (NetStatPtr PNTR) MemNew(sizeof(NetStatPtr) *
                     STAT_CHUNK)) == NULL)
                {
                    MemFree(newStat->label);
                    MemFree(newStat);
                    return;
                }
                maxCurStats = STAT_CHUNK;
            }

            /* re-allocate vector if necessary */
            if (numStats >= maxCurStats - 1)
            {
                maxCurStats += STAT_CHUNK;
                if ((statVector = (NetStatPtr PNTR) Realloc(statVector,
                     sizeof(NetStatPtr) * maxCurStats)) == NULL)
                {
                    MemFree(newStat->label);
                    MemFree(newStat);
                    return;
                }
            }

            /* move everything else up */
            for (r = numStats - 1; r >= k; r--)
            {
                statVector[r+1] = statVector[r];
            }
            statVector[k] = newStat;
            *statHandle = newStat;
            numStats++;
        }
    }

    Stat = *statHandle;
    Stat->n++;
    Stat->total += dat;
    Stat->flags |= flags;
    mean = Stat->total / Stat->n;
    tempterm = (mean - dat);
    Stat->sum_of_squares += tempterm * tempterm;
}


/*******************************************************************************
*
*    NetInit()
*
*    Connect to the network services dispatcher, unless already attached.
*******************************************************************************/

Boolean NetInit(void)
{

    if (! NetEntAsnLoad() )
        return FALSE;

    if (!loaded)
    {
#ifdef NET_ENTR_DYNAMIC_LOAD
        if (!AsnLoad())
            return FALSE;
#endif
        loaded = TRUE;
    }

    if (num_attached++ > 0)
        return TRUE;

    /* if ( there are no network-media ) then */
    if (ParseMedia(NULL, MEDIUM_NETWORK) == 0)
    {
        num_attached--;
        return FALSE;
    }

    if ((dispatcher = NI_GenericInit(NULL, NULL, TRUE, lastDispatcher, sizeof(lastDispatcher))) != NULL) {
        return TRUE;
    }

    num_attached--;
    return FALSE;
}


/*******************************************************************************
*
*    NetInit()
*
*    Disconnect from the network services dispatcher, if this is the last
*    service user which is detaching.
*******************************************************************************/

Boolean CDECL NetFini(void)
{
    if (num_attached > 0)
        num_attached--;

    if (num_attached == 0)
    {
        NI_EndServices (dispatcher);
        if (reallyFinal)
            FreeNetStats();
    }

    return TRUE;
}

Boolean CDECL ForceNetInit(void)
{
    Boolean retval;

    reallyFinal = FALSE;
    num_attached = 0; /* force re-attempt to contact dispatcher */
    retval =  NetInit();
    reallyFinal = TRUE;

    return retval;
}

NI_HandPtr NetServiceGet(CharPtr channel, CharPtr def_service,
                         ConfCtlProc swapInProc, NI_HandPtr oldSessionHandle)
{
    MediaPtr media;
    NetMediaInfoPtr nmi;
    NI_HandPtr firstSessionHandle = NULL;
    CharPtr param_section;

    /* coding-trick to make current media match type "channel" */
    if (! SelectDataSource(channel, "MEDIA", NULL))
    {
        return NULL;
    }

    do {
        media = GetCurMedia();
        if (media == NULL)
            continue;
        if (media->media_type != MEDIUM_NETWORK)
            continue;
        nmi = (NetMediaInfoPtr) media->media_info;

        /* if media is already initialized, only attempt to re-obtain service */
        /* if this media matches the media which failed or timed-out          */
        if (nmi != NULL && nmi->sessionHandle != oldSessionHandle)
            continue;

        if (nmi != NULL)
        { /* de-allocate old resources */
            NI_ServiceDisconnect(nmi->sessionHandle);
            nmi->sessionHandle = NULL;
        }
        else { /* allocate structure */
            nmi = (NetMediaInfoPtr) MemNew(sizeof(NetMediaInfo));
            media->media_info = (VoidPtr) nmi;
        }
        nmi->sessionHandle = NULL;

        media->swapOutMedia = SwapOutNet;
        media->swapInMedia = swapInProc;

        param_section = media->media_alias;

        nmi->sessionHandle = NI_GenericGetService(dispatcher, NULL,
                                                  param_section, def_service,
                                                  TRUE);
        if (firstSessionHandle == NULL)
            firstSessionHandle = nmi->sessionHandle;
    } while (SelectNextDataSource());

    lastEntrezServ = firstSessionHandle;

    return firstSessionHandle;
}


static Boolean SwapOutNet (VoidPtr med)
{
    return TRUE;
}
