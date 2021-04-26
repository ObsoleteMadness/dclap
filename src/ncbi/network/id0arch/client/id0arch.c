/*
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
* File Name:  id0arch.c
*
* Author:  Sirotkin
*
* Version Creation Date:   11/23/93
*
* $Revision: 1.6 $
*
* File Description: 
*       API for Medline Archive service
*
*$Log:	id0arch.c,v $
 * Revision 1.6  94/02/08  16:31:15  epstein
 * clear pending error when reading ASN.1 responses
 * 
 * Revision 1.5  93/12/01  14:05:53  epstein
 * Change default service and section from ID to ID0
 * 
 * Revision 1.4  93/11/30  10:46:26  kans
 * *** empty log message ***
 * 
 * Revision 1.3  93/11/24  13:43:03  sirotkin
 * changed include file from id0 to asnid0"
 * 
 * Revision 1.2  93/11/24  13:25:49  sirotkin
 * First working version
 * 
 * Revision 1.1  93/11/23  14:54:25  sirotkin
 * Initial revision
 * 
*/

#include <sequtil.h>
#include <ncbinet.h>
#include <objsset.h>
#include <objsub.h>
#include "id0arch.h"
#include "id0gen.h"
#include "asnid0.h"

static ID0serverBackPtr NetID0servReadAsn PROTO((void));
static Boolean ReestablishNetID0Arch PROTO((void));
static Boolean NetInit PROTO((void));
static Boolean ForceNetInit PROTO((void));
static Boolean NetFini PROTO((void));
static Boolean GenericReestablishNet PROTO((CharPtr svcName, Boolean showErrs));

static NI_HandPtr svcp = NULL;
static AsnIoPtr   asnin = NULL;
static AsnIoPtr   asnout = NULL;
ID0serverRequestPtr id0reqp;
ID0serverBackPtr id0bp;
static Boolean num_attached = 0;
static Boolean reallyFinal = TRUE;
static NI_DispatcherPtr dispatcher;


/*****************************************************************************
*
*   ID0ArchInit ()
*
*****************************************************************************/

Boolean ID0ArchInit (void)

{
    DataVal    av;

    if (! NetInit())
        return FALSE;

    svcp = NI_GenericGetService(dispatcher, NULL, "ID0", "ID0", TRUE);
    if (svcp == NULL)
    {
        ErrPost(CTX_UNKNOWN, 1, "NI_ServiceGet [%s] (%s)", ni_errlist[ni_errno], ni_errtext);
        ID0ArchFini();
        return FALSE;
    }

    asnin = svcp->raip;
    asnout = svcp->waip;

    /**********************************************************/

    id0reqp = ValNodeNew(NULL);
    id0reqp->choice = ID0serverRequest_init;
    ID0serverRequestAsnWrite (id0reqp, asnout, NULL);
    AsnIoReset(asnout);
    ID0serverRequestFree (id0reqp);

    if ((id0bp = NetID0servReadAsn()) == NULL)
    {
        return FALSE;
    }
    else
    {
        ID0serverBackFree (id0bp);
        return TRUE;
    }
}

/*****************************************************************************
*
*   ID0ArchFini ()
*
*****************************************************************************/

static Boolean s_ID0ArchFini (void)

{
    Boolean retval = TRUE;

    if (asnout != NULL && asnin != NULL)
    {
        id0reqp = ValNodeNew(NULL);
        id0reqp->choice = ID0serverRequest_fini;
        ID0serverRequestAsnWrite (id0reqp, asnout, NULL);
        AsnIoReset(asnout);
        ID0serverRequestFree (id0reqp);

        if ((id0bp = NetID0servReadAsn()) == NULL)
        {
            retval = FALSE;
        }
        else
        {
            ID0serverBackFree (id0bp);
        }
    }

    NI_ServiceDisconnect(svcp);
    NetFini();

    return retval;
}

/* the only thing done here is to suppress errors */
Boolean ID0ArchFini (void)

{
    short erract;
    ErrDesc err;
    Boolean retval;

    ErrGetOpts(&erract, NULL);
    ErrSetOpts(ERR_IGNORE, 0);
    ErrFetch(&err);

    retval = s_ID0ArchFini();

    ErrSetOpts(erract, 0);
    ErrFetch(&err);

    return retval;
}

static SeqEntryPtr
s_ID0ArchSeqEntryGet (Int4 uid, Boolean PNTR was_live, Int2 maxplex)

{
    ID0serverRequestPtr id0reqp;
    SeqEntryPtr sep;
		ID0serverMaxcomplexPtr mcp;

    id0reqp = ValNodeNew(NULL);
    id0reqp->choice = ID0serverRequest_getsefromgi;
		mcp = ID0serverMaxcomplexNew();
    id0reqp->data.ptrvalue = mcp;
		mcp -> gi = uid;
		mcp -> maxplex = maxplex;
    ID0serverRequestAsnWrite (id0reqp, asnout, NULL);
    AsnIoReset(asnout);
    ID0serverRequestFree (id0reqp);

    if ((id0bp = NetID0servReadAsn()) == NULL)
        return NULL;

    if (id0bp->choice != ID0serverBack_gotseqentry &&
			id0bp->choice !=  ID0serverBack_gotdeadseqentry)
    {
        ID0serverBackFree (id0bp);
        return NULL;
    }
		if (was_live != NULL){
			if (id0bp->choice == ID0serverBack_gotdeadseqentry){
				* was_live = FALSE;
			}else{
				* was_live = TRUE;
			}
		}
    sep = (SeqEntryPtr) (id0bp->data.ptrvalue);
    id0bp->data.ptrvalue = NULL; /* for clean free */
    ID0serverBackFree (id0bp);

    return sep;
}

SeqEntryPtr
ID0ArchSeqEntryGet (Int4 gi, Boolean PNTR was_live, Int2 maxplex)
{
    Int4 i;
    SeqEntryPtr sep;
    short erract;
    ErrDesc err;
    Int4 retval = 0;
 
		if (maxplex < 0 || maxplex > 4){
          ErrPost(CTX_UNKNOWN, 2, "ID0ArchSeqEntryGet: maxplex(%d) out of range (0-4)", maxplex);
			return NULL;
		}
	  for (i = 0; i < ID_SERV_RETRIES; i++)
	  {
			if (i > 0)
			{
				 if (! ReestablishNetID0Arch())
					  break;
			}
 
			ErrGetOpts(&erract, NULL);
			ErrSetOpts(ERR_IGNORE, 0);
			ErrFetch(&err);
			sep = s_ID0ArchSeqEntryGet (gi, was_live, maxplex);
			ErrSetOpts(erract, 0);
			if (! ErrFetch(&err))
				 break; /* success */
	  }
	return sep;
}

static Int4
s_ID0ArchGIGet (SeqIdPtr sip)
{
    ID0serverRequestPtr id0reqp;
    Int4 gi;

    id0reqp = ValNodeNew(NULL);
    id0reqp->choice = ID0serverRequest_getgi;
    id0reqp->data.ptrvalue = sip;
    ID0serverRequestAsnWrite (id0reqp, asnout, NULL);
    AsnIoReset(asnout);
    ID0serverRequestFree (id0reqp);

    if ((id0bp = NetID0servReadAsn()) == NULL)
        return 0;

    if (id0bp->choice != ID0serverBack_gotgi)
    {
        ID0serverBackFree (id0bp);
        return 0;
    }
    gi = (id0bp->data.intvalue);
    ID0serverBackFree (id0bp);

    return gi;
}

Int4
ID0ArchGIGet (SeqIdPtr sip)
{
    Int4 gi, i;
    short erract;
    ErrDesc err;
    Int4 retval = 0;
 
	  for (i = 0; i < ID_SERV_RETRIES; i++)
	  {
			if (i > 0)
			{
				 if (! ReestablishNetID0Arch())
					  break;
			}
 
			ErrGetOpts(&erract, NULL);
			ErrSetOpts(ERR_IGNORE, 0);
			ErrFetch(&err);
			gi = s_ID0ArchGIGet (sip);
			ErrSetOpts(erract, 0);
			if (! ErrFetch(&err))
				 break; /* success */
	  }
	return gi;
}

static ID0serverBackPtr NetID0servReadAsn(void)
{
    ID0serverBackPtr id0bp;
    short erract;
    ErrDesc err;

    ErrGetOpts(&erract, NULL);
    ErrSetOpts(ERR_IGNORE, 0);
    ErrFetch(&err); /* clear any pending error */

    id0bp = ID0serverBackAsnRead(asnin, NULL);

    if (ErrFetch(&err))
    {
        return NULL;
    }
    ErrSetOpts(erract, 0);

    return id0bp;
}

static Boolean ReestablishNetID0Arch(void)
{
    return GenericReestablishNet("ID0Arch", TRUE);
}

static Boolean GenericReestablishNet(CharPtr svcName, Boolean showErrs)
{
    Handle mon = NULL;
    Boolean retval;
    CharPtr buf;

    buf = MemNew(2 * StrLen(svcName) + 60);

    if (showErrs) {
        sprintf (buf, "Re-establishing %s Service", svcName);
        mon = MonitorStrNew(buf, 40);
        sprintf (buf, "Requesting %s service", svcName);
        MonitorStrValue(mon, buf);
    }
    NetFini();
    retval = TRUE;

    if (! NetInit())
    {
        sprintf (buf, "%s get failed; re-contacting dispatcher", svcName);
        MonitorStrValue(mon, buf);
        retval = FALSE;
        if (ForceNetInit())
        { /* successfully established contact w/dispatcher */
            sprintf (buf, "%s get failed; re-requesting %s service",
                     svcName, svcName);
            MonitorStrValue(mon, buf);
            retval = NetInit();
        }
        else {
            ErrPost(CTX_UNKNOWN, 1, "Unable to re-contact dispatcher");
            if (showErrs) {
                ErrShow();
            }
        }
    }

    MonitorFree(mon);

    if (! retval )
    {
        sprintf (buf, "Unable to re-establish %s service", svcName);
        ErrPost(CTX_UNKNOWN, 1, buf);
        if (showErrs) {
            ErrShow();
        }
    }

    MemFree(buf);
    return retval;
}

static Boolean
NetInit(void)
{
    if (num_attached++ > 0)
        return TRUE;

    return ((dispatcher = NI_GenericInit(NULL, NULL, TRUE, NULL, 0)) != NULL);
}


static Boolean ForceNetInit(void)
{
    Boolean retval;

    reallyFinal = FALSE;
    num_attached = 0; /* force re-attempt to contact dispatcher */
    retval = NetInit();
    reallyFinal = TRUE;

    return retval;
}

static Boolean NetFini(void)
{
    if (num_attached > 0)
        num_attached--;

    if (num_attached == 0)
    {
        NI_EndServices (dispatcher);
        dispatcher = NULL;
    }

    return TRUE;
}
