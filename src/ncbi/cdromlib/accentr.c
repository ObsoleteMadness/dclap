/*   accentr.c
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
* File Name:  accentr.c
*
* Author:  Ostell
*
* Version Creation Date:   4/23/92
*
* $Revision: 2.26 $
*
* File Description: 
*       entrez index access library for Entrez
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/

#include <accentr.h>

#include <cdconfig.h>
#ifdef _NETENT_
#include <netentr.h>      /* support network access */
#else
#ifndef _CDENTREZ_
#include <cdentrez.h>      /* support cdrom access */
#endif
#endif /* _NETENT_ */

#ifdef _NET_AND_CD_
#include <netentr.h>      /* support network access */
#endif /* _NET_AND_CD_ */

#define MAX_SECS_IN_ADAPTIVE_MATRIX 3
#define MAX_CHUNK_IN_ADAPTIVE_MATRIX 12
#define ADAPTIVE_MATRIX_INITIAL_CONDITION 8

#ifdef _NETENTREZ_
Uint1 AdaptiveDocSumMatrix[MAX_CHUNK_IN_ADAPTIVE_MATRIX+1][MAX_SECS_IN_ADAPTIVE_MATRIX+1] = {
  8,  6,  3,  3,
  8,  6,  3,  3,
  8,  6,  3,  3,
  8,  6,  3,  3,
  9,  7,  4,  3,
  9,  8,  5,  3,
 10,  9,  5,  3,
 10, 10,  5,  3,
 11, 10,  5,  3,
 11, 11,  5,  3,
 12, 11,  5,  3,
 12, 11,  5,  3,
 12, 11,  5,  3
};
#endif /* _NETENTREZ_ */

                           /* maximum link records returned */
static Int4 MaxLinks = (Int4)(INT2_MAX / sizeof(DocUid));

static DocUidPtr      queueduids = NULL; /* Used to queue CD-ROM uid query list */
static DocSumPtr PNTR queuedsums = NULL; /* Used to queue network retrieved DocSums */
static Int2           numqueued = 0;
static DocType        queuedtype;
static CharPtr        CombinedInfo = NULL;
static DocType        lastTermType;
static Int2           lastBooleanMediaType;

/*****************************************************************************
*
*   EntrezInit ()
*   
*****************************************************************************/

Boolean LIBCALL EntrezInit (CharPtr appl_id, Boolean no_warnings, BoolPtr is_network)

{
    Boolean cdretval = FALSE;
    Boolean netretval = FALSE;
    queueduids = NULL;
    queuedsums = NULL;
    numqueued = 0;

    if (is_network != NULL)
    {
        *is_network = FALSE;
    }
#ifdef _CDENTREZ_
    cdretval = CdEntrezInit(no_warnings);
#endif
#ifdef _NETENTREZ_
    if (is_network != NULL)
    {
        *is_network = TRUE;
    }
    netretval = NetEntrezInit(appl_id, no_warnings);
#endif
    if (!cdretval && !netretval)
    { /* partial success (one of two) is O.K. */
        return FALSE;
    }

    MaxLinks = EntrezGetMaxLinks();
    return TRUE;
}

/*****************************************************************************
*
*   EntrezFini ()
*
*****************************************************************************/

static void LIBCALL ClearQueuedDocSums (void)

{
    Int2  i;

    if (queueduids != NULL) {
        queueduids = (DocUidPtr) MemFree (queueduids);
    }
    if (queuedsums != NULL) {
        for (i = 0; i < numqueued; i++) {
          if (queuedsums [i] != NULL) {
            queuedsums [i] = DocSumFree (queuedsums [i]);
          }
        }
        queuedsums = (DocSumPtr PNTR) MemFree (queuedsums);
    }
    numqueued = 0;
}

void LIBCALL EntrezFini (void)

{
    ClearQueuedDocSums ();
    if (CombinedInfo != NULL)
    {
        CombinedInfo = (CharPtr) MemFree(CombinedInfo);
    }
#ifdef _CDENTREZ_
    CdEntrezFini();
#endif
#ifdef _NETENTREZ_
    NetEntrezFini();
#endif
}

/*****************************************************************************
*
*   EntrezInfoPtr EntrezGetInfo()
*
*****************************************************************************/
EntrezInfoPtr LIBCALL EntrezGetInfo (void)

{
    static EntrezInfoPtr cip = NULL;

    if (cip == NULL)
        cip = EntrezInfoMerge();

    /* as a last resort, fall back on the old, faithful function */
    if (cip == NULL)
#ifdef _CDENTREZ_
        return CdEntrezGetInfo();
#else
        return NetEntrezGetInfo();
#endif
    else
        return cip;
}

/*****************************************************************************
*
*   CharPtr EntrezDetailedInfo()
*       returns either NULL (if no info is available), or a pointer to a
*       statically allocated string containing formatted status information
*
*****************************************************************************/
CharPtr LIBCALL EntrezDetailedInfo (void)

{
    CharPtr cdinfo = NULL;
    CharPtr netinfo = NULL;
    CharPtr extrainfo = NULL;
    Int2 extrainfoLen = 0;
    EntrezInfoPtr eip;

#ifdef _CDENTREZ_
    cdinfo = CdEntrezDetailedInfo();
#endif
#ifdef _NETENTREZ_
    netinfo = NetEntrezDetailedInfo();
#endif
    if ((eip = EntrezGetInfo()) != NULL)
    {
        extrainfo = MemNew(StringLen(eip->descr) + 300);
        StrCpy(extrainfo, "\n\nCURRENT MEDIA\n  ");
        StrCat(extrainfo, eip->descr);
        extrainfoLen = StringLen(extrainfo);
        sprintf(&extrainfo[extrainfoLen], "\n  Version %ld.%ld", 
                (long) eip->version, (long) eip->issue);
        extrainfoLen = StringLen(extrainfo);
        if (eip->types != NULL && eip->type_count >= 1)
        {
            sprintf(&extrainfo[extrainfoLen], "\n  # %s entries = %ld",
                    eip->type_names[0], (long) (eip->types[0].num));
            extrainfoLen = StringLen(extrainfo);
            if (eip->type_count >= 2 && eip->types[1].num > 0)
            {
                sprintf(&extrainfo[extrainfoLen], "\n  # %s entries = %ld",
                        eip->type_names[1], (long) (eip->types[1].num));
                extrainfoLen = StringLen(extrainfo);
            }
            if (eip->type_count >= 3 && eip->types[2].num > 0)
            {
                sprintf(&extrainfo[extrainfoLen], "\n  # %s entries = %ld",
                        eip->type_names[2], (long) (eip->types[2].num));
                extrainfoLen = StringLen(extrainfo);
            }
        }
        StrCat(extrainfo, "\n");
        extrainfoLen = StringLen(extrainfo);
    }

    if (CombinedInfo != NULL)
        MemFree(CombinedInfo);
    if ((CombinedInfo = MemNew(StringLen(cdinfo) + StringLen(netinfo) + 4 + extrainfoLen)) == NULL)
    {
        MemFree(extrainfo);
        return cdinfo; /* better than nothing */
    }
    StringCpy(CombinedInfo, cdinfo);
    StringCat(CombinedInfo, "\n\n");
    StringCat(CombinedInfo, netinfo);
    StringCat(CombinedInfo, extrainfo);
    MemFree(extrainfo);

    return CombinedInfo;
}

/*****************************************************************************
*
*   EntrezGetMaxLinks()
*       returns max links in link set allowed by system
*
*****************************************************************************/
Int4 LIBCALL EntrezGetMaxLinks (void)

{
    Int4 maxlinks = INT4_MAX;
    Int4 links;
    Boolean cdSelected = FALSE;

    if (! SelectDataSource(ENTR_LINKS_CHAN, "INFO", NULL))
    {
        /* ERROR, data unobtainable */
        return -1;
    }

    do {
        switch (CurMediaType()) {
#ifdef _CDENTREZ_
        case MEDIUM_CD:
            /* for ergonomics, check a maximum of one CD */
            cdSelected = TRUE;
            /* no break */
        case MEDIUM_DISK:
            links = CdEntGetMaxLinks();
            break;
#endif
#ifdef _NETENTREZ_
        case MEDIUM_NETWORK:
            links = NetEntGetMaxLinks();
            break;
#endif
        default:
            break;
        }
        maxlinks = MIN(maxlinks, links);
    } while (!cdSelected && SelectNextDataSource());

    if (maxlinks == INT4_MAX)
        maxlinks = -1;
    return maxlinks;
}

/*****************************************************************************
*
*   EntrezSetUserMaxLinks(num)
*       user settable maximum
*       can't be > EntrezGetMaxLinks()
*
*****************************************************************************/
Int4 LIBCALL EntrezSetUserMaxLinks (Int4 num)

{
    Int4 big;

    big = EntrezGetMaxLinks();
    if (big < num)
        num = big;
    MaxLinks = num;
    return num;
}

/*****************************************************************************
*
*   EntrezGetUserMaxLinks()
*       returns current value of MaxLinks
*
*****************************************************************************/
Int4 LIBCALL EntrezGetUserMaxLinks (void)

{
    return MaxLinks;
}

/*****************************************************************************
*
*   EntrezCreateNamedUidList(term, type, field, num, uids)
*       Creates a term node in the entrez set structure if one does not
*       yet exist, and loads the posting file from the uid parameter.
*
*****************************************************************************/
void LIBCALL EntrezCreateNamedUidList (CharPtr term, DocType type, DocField field, Int4 num, DocUidPtr uids)

{
    if (! SelectDataSourceByType(type, "TERMS", NULL))
    {
        return;
    }

    switch (CurMediaType ()) {
#ifdef _CDENTREZ_
    case MEDIUM_CD:
    case MEDIUM_DISK:
        CdEntrezCreateNamedUidList (term, type, field, num, uids);
        break;
#endif
#ifdef _NETENTREZ_
    case MEDIUM_NETWORK:
        NetEntrezCreateNamedUidList (term, type, field, num, uids);
        break;
#endif
    default:
      break;
    }
}

/*****************************************************************************
*
*   EntrezTLNew (type)
*       Creates linked list of asn nodes for constructing boolean query on
*       terms.  First node points to processing info.
*       Remaining nodes contain symbols for AND,
*       OR, LEFT PARENTHESIS, RIGHT PARENTHESIS, or a SPECIAL or TOTAL term
*       specification.  The term specification nodes point to a CdTerm or a
*       string.
*
*****************************************************************************/

ValNodePtr LIBCALL EntrezTLNew (DocType type)

{
    if (! SelectDataSourceByType(type, "BOOLEANS", NULL))
    {
        return NULL;
    }

    switch ( lastBooleanMediaType = CurMediaType() ) {
#ifdef _CDENTREZ_
    case MEDIUM_CD:
    case MEDIUM_DISK:
        return CdEntTLNew(type);
#endif
#ifdef _NETENTREZ_
    case MEDIUM_NETWORK:
        return NetEntTLNew(type);
#endif
    default:
        return NULL;
    }
}

/*****************************************************************************
*
*   EntrezTLAddTerm (elst, term, type, field, special)
*       Adds a term node to a boolean algebraic term query.
*
*****************************************************************************/

ValNodePtr LIBCALL EntrezTLAddTerm (ValNodePtr elst, CharPtr term, DocType type, DocField field, Boolean special)

{
    switch (lastBooleanMediaType) {
#ifdef _CDENTREZ_
    case MEDIUM_CD:
    case MEDIUM_DISK:
        return CdEntTLAddTerm (elst, term, type, field, special);
#endif
#ifdef _NETENTREZ_
    case MEDIUM_NETWORK:
        return NetEntTLAddTerm (elst, term, type, field, special);
#endif
    default:
        return NULL;
    }
}

/*****************************************************************************
*
*   EntrezTLFree (elst)
*       Frees a boolean algebraic term query list.
*
*****************************************************************************/

ValNodePtr LIBCALL EntrezTLFree (ValNodePtr elst)

{
    switch (lastBooleanMediaType) {
#ifdef _CDENTREZ_
    case MEDIUM_CD:
    case MEDIUM_DISK:
        return CdEntTLFree(elst);
#endif
#ifdef _NETENTREZ_
    case MEDIUM_NETWORK:
        return NetEntTLFree(elst);
#endif
    default:
        return NULL;
    }
}

/*****************************************************************************
*
*   EntrezTLAddLParen (elst)
*       Adds a LEFT PAREN node to a boolean algebraic term query.
*
*****************************************************************************/

ValNodePtr LIBCALL EntrezTLAddLParen (ValNodePtr elst)

{
  ValNodePtr anp;

  anp = NULL;
  if (elst != NULL) {
    anp = ValNodeNew (elst);
    if (anp != NULL) {
      anp->choice = LPAREN;
    }
  }
  return anp;
}

/*****************************************************************************
*
*   EntrezTLAddRParen (elst)
*       Adds a RIGHT PAREN node to a boolean algebraic term query.
*
*****************************************************************************/

ValNodePtr LIBCALL EntrezTLAddRParen (ValNodePtr elst)

{
  ValNodePtr anp;

  anp = NULL;
  if (elst != NULL) {
    anp = ValNodeNew (elst);
    if (anp != NULL) {
      anp->choice = RPAREN;
    }
  }
  return anp;
}

/*****************************************************************************
*
*   CdEntTLAddAND (elst)
*       Adds an AND node to a boolean algebraic term query.
*
*****************************************************************************/

ValNodePtr LIBCALL EntrezTLAddAND (ValNodePtr elst)

{
  ValNodePtr anp;

  anp = NULL;
  if (elst != NULL) {
    anp = ValNodeNew (elst);
    if (anp != NULL) {
      anp->choice = ANDSYMBL;
    }
  }
  return anp;
}

/*****************************************************************************
*
*   EntrezTLAddOR (elst)
*       Adds an OR node to a boolean algebraic term query.
*
*****************************************************************************/

ValNodePtr LIBCALL EntrezTLAddOR (ValNodePtr elst)

{
  ValNodePtr anp;

  anp = NULL;
  if (elst != NULL) {
    anp = ValNodeNew (elst);
    if (anp != NULL) {
      anp->choice = ORSYMBL;
    }
  }
  return anp;
}

/*****************************************************************************
*
*   EntrezTLAddBUTNOT (elst)
*       Adds an BUTNOT node to a boolean algebraic term query.
*
*****************************************************************************/

ValNodePtr LIBCALL EntrezTLAddBUTNOT (ValNodePtr elst)

{
  ValNodePtr anp;

  anp = NULL;
  if (elst != NULL) {
    anp = ValNodeNew (elst);
    if (anp != NULL) {
      anp->choice = BUTNOTSYMBL;
    }
  }
  return anp;
}

/*****************************************************************************
*
*   EntrezTLEval (elst)
*       Evaluates a boolean algebraic term query list, returning a pointer to
*       a LinkSet containing the resultant unique identifiers.
*
*****************************************************************************/

LinkSetPtr LIBCALL EntrezTLEval (ValNodePtr elst)

{

    switch (lastBooleanMediaType) {
#ifdef _CDENTREZ_
    case MEDIUM_CD:
    case MEDIUM_DISK:
        return CdEntTLEval(elst);
#endif
#ifdef _NETENTREZ_
    case MEDIUM_NETWORK:
        return NetEntTLEval(elst);
#endif
    default:
        return NULL;
    }
}

/*****************************************************************************
*
*   DocSumPtr EntrezDocSum(type, uid)
*
*****************************************************************************/
DocSumPtr LIBCALL EntrezDocSum (DocType type, DocUid uid)

{
    DocSumPtr dsp;

    if (! SelectDataSourceByType(type, "DOCSUMS", NULL))
    { /* ERROR, data unobtainable */
        return NULL;
    }

    do {
        switch (CurMediaType())
        {
#ifdef _CDENTREZ_
        case MEDIUM_CD:
        case MEDIUM_DISK:
            if ((dsp = CdDocSum (type, uid)) != NULL)
                return dsp;
            break;
#endif
#ifdef _NETENTREZ_
        case MEDIUM_NETWORK:
            if ((dsp = NetDocSum (type, uid)) != NULL)
                return dsp;
            break;
#endif
        default:
            break;
        }
    } while (SelectNextDataSource());

    return NULL;
}

/*****************************************************************************
*
*   EntrezDocSumListGet(numuid, type, uids, callback)
*
*****************************************************************************/
static Int2 LIBCALL DocSumQueueGet (Int2 numuid, DocType type, DocSumListCallBack callback)

{
  DocSumPtr  dsp;
  int        count = 0;
  int        i;
  Boolean    goOn;

  if (callback == NULL) {
    ClearQueuedDocSums ();
    return 0;
  }
  if (type != queuedtype) {
    ClearQueuedDocSums ();
    return 0;
  }
  goOn = TRUE;
  count = 0;
  if (numuid > numqueued) {
    numuid = numqueued;
  }

  i = 0;
  while (i < numqueued && goOn) {
    if (queueduids [i] != 0) {
      if (queuedsums != NULL) {
        dsp = queuedsums [i];
      } else {
        dsp = EntrezDocSum (type, queueduids [i]);
      }
      if (dsp != NULL) {
        count++;
        goOn = callback (dsp, dsp->uid);
        queueduids [i] = 0;
        if (queuedsums != NULL) {
          queuedsums [i] = NULL;
        }
      } else {
        count++;
        goOn = callback (NULL, queueduids [i]);
        queueduids [i] = 0;
        if (queuedsums != NULL) {
          queuedsums [i] = NULL;
        }
      }
    }
    i++;
  }
  if (i >= numqueued) {
    if (queuedsums != NULL) {
      queuedsums = (DocSumPtr PNTR) MemFree (queuedsums);
    }
    queueduids = (DocUidPtr) MemFree (queueduids);
    numqueued = 0;
  }

  return count;
}

Int2 LIBCALL EntrezDocSumListGet (Int2 numuid, DocType type, DocUidPtr uids,
                                 DocSumListCallBack callback)

{
  DocSumPtr  dsp;
  int        count = 0;
  int        i;
  Boolean    goOn;
  DocUidPtr  uidlist;
#ifdef _NETENTREZ_
  Int2       chunkSize;
  Int2       chunkStart;
  Int2       predictedNextChunk;
  Int2       actual;
  DocSumPtr  PNTR result;
  static Int2 adaptiveChunkSize = ADAPTIVE_MATRIX_INITIAL_CONDITION;
  time_t     timeElapsed, startTime;
#endif

  /* For ergonomics, DocSums are fetched from the CD-ROM and displayed */
  /* one at a time. For efficiency, all DocSums are fetched from the   */
  /* network at once, and subsequently all are displayed.              */

  if (uids == NULL && queueduids != NULL && callback != NULL) {
    return DocSumQueueGet (numuid, queuedtype, callback);
  }
  ClearQueuedDocSums ();
  queuedtype = type;
  if (numuid == 0) {
    return 0;
  }
  if (uids == NULL) {
    return 0;
  }
  goOn = TRUE;
  count = 0;
  uidlist = (DocUidPtr) MemNew (sizeof (DocUid) * numuid);
  if (uidlist != NULL) {
    for (i = 0; i < numuid; i++) {
      uidlist [i] = uids [i];
    }
#ifdef _NETENTREZ_
    if (! SelectDataSourceByType(type, "DOCSUMS", NULL))
    { /* ERROR, data unobtainable */
      uidlist = (DocUidPtr) MemFree(uidlist);
      return 0;
    }

    /* if the first-found media type is NET, try to get them all from the  */
    /* net; otherwise, to heck with it, just get them one at a time, below */
    if (CurMediaType() == MEDIUM_NETWORK) {
      result = (DocSumPtr PNTR) MemNew (sizeof (DocSumPtr) * numuid);
      if (result != NULL) {
        i = 0;
        chunkStart = 0;
        chunkSize = adaptiveChunkSize;
        while (chunkStart < numuid)
        {
          /* if ( the callback told us that it doesn't need any more right   */
          /*      now, OR if we would be getting too many ) THEN             */
          /*    get all of the remaining docsums in one shot                 */
          if (callback == NULL || !goOn || (chunkSize + chunkStart) >= numuid)
            chunkSize = numuid - chunkStart;

          /* Ask for some to be computed in advance, because it can take the*/
          /* application a long time to process the ones which it _does_    */
          /* receive this time                                              */
          predictedNextChunk = MIN(numuid - (chunkStart+chunkSize), chunkSize);
          startTime = GetSecs();
          actual = NetDocSumListGet (&result[chunkStart], chunkSize +
                                     predictedNextChunk, type,
                                     &uids[chunkStart], predictedNextChunk);
          chunkStart += chunkSize;

          timeElapsed = GetSecs() - startTime;
          if (timeElapsed < 0)
              timeElapsed = 0;
          if (timeElapsed > MAX_SECS_IN_ADAPTIVE_MATRIX)
              timeElapsed = MAX_SECS_IN_ADAPTIVE_MATRIX;
          if (chunkSize > MAX_CHUNK_IN_ADAPTIVE_MATRIX)
              chunkSize = MAX_CHUNK_IN_ADAPTIVE_MATRIX;
          adaptiveChunkSize = AdaptiveDocSumMatrix[chunkSize][timeElapsed];
          chunkSize = adaptiveChunkSize;

          if (goOn && callback != NULL) {
            while (i < chunkStart && goOn) {
              dsp = result [i];
              if (dsp != NULL) {
                count++;
                goOn = callback (dsp, dsp->uid);
                uidlist [i] = 0;
                result [i] = NULL;
              } else {
                count++;
                goOn = callback (NULL, uidlist [i]);
                uidlist [i] = 0;
                result [i] = NULL;
              }
              i++;
            }
          }
        }

        if (count < numuid) { /* enqueue remaining docsums */
          queueduids = uidlist;
          queuedsums = result;
          numqueued = numuid;
        } else {
          MemFree (uidlist);
          MemFree (result);
        }
      }
      return count;
    }
#endif

    /* get them one at a time */
    if (callback != NULL) {
      i = 0;
      while (i < numuid && goOn) {
        dsp = EntrezDocSum (type, uidlist [i]);
        if (dsp != NULL) {
          count++;
          goOn = callback (dsp, dsp->uid);
          uidlist [i] = 0;
        } else {
          count++;
          goOn = callback (NULL, uidlist [i]);
          uidlist [i] = 0;
        }
        i++;
      }
    }
    if (count < numuid) {
      queueduids = uidlist;
      numqueued = numuid;
    } else {
      MemFree (uidlist);
    }
  }
  return count;
}

/*****************************************************************************
*
*   DocSumFree(dsp)
*
*****************************************************************************/
DocSumPtr LIBCALL DocSumFree (DocSumPtr dsp)

{
    if (dsp == NULL)
        return NULL;
    MemFree(dsp->caption);
    MemFree(dsp->title);
    return ((DocSumPtr) MemFree(dsp));
}

/*****************************************************************************
*
*   EntrezTermListByTerm (type, field, term, numterms, proc, first_page)
*       Gets Terms starting at term
*       returns pages read
*       sets first_page to first page read
*
*****************************************************************************/
Int2 LIBCALL EntrezTermListByTerm (DocType type, DocField field, CharPtr term, Int2 numterms, TermListProc proc, Int2Ptr first_page)

{
    if (! SelectDataSourceByType(type, "TERMS", NULL))
    {
        return 0;
    }

    lastTermType = type;

    switch (CurMediaType())
    {
#ifdef _CDENTREZ_
    case MEDIUM_CD:
    case MEDIUM_DISK:
        return CdTermListByTerm(type, field, term, numterms, proc, first_page);
#endif
#ifdef _NETENTREZ_
    case MEDIUM_NETWORK:
        return NetTermListByTerm(type, field, term, numterms, proc, first_page);
        break;
#endif
    default:
        return 0;
    }
}

/*****************************************************************************
*
*   EntrezTermListByPage (type, field, page, numpage, proc)
*       Gets terms starting at page, for numpage, by calling proc
*       returns pages read
*
*****************************************************************************/
Int2 LIBCALL EntrezTermListByPage (DocType type, DocField field, Int2 page, Int2 numpage, TermListProc proc)

{
    if (! SelectDataSourceByType(type, "TERMS", NULL))
    {
        return 0;
    }

    lastTermType = type;

    switch (CurMediaType())
    {
#ifdef _CDENTREZ_
    case MEDIUM_CD:
    case MEDIUM_DISK:
        return CdTermListByPage(type, field, page, numpage, proc);
#endif
#ifdef _NETENTREZ_
    case MEDIUM_NETWORK:
        return NetTermListByPage(type, field, page, numpage, proc);
#endif
    default:
        return 0;
    }
}

/*****************************************************************************
*
*   EntrezFindTerm(type, field, term, spec, total)
*       returns count of special and total for a term
*       if term ends with  "...", does a truncated merge of the term
*
*****************************************************************************/
Boolean LIBCALL EntrezFindTerm (DocType type, DocField field, CharPtr term, Int4Ptr spcl, Int4Ptr totl)

{
    if (! SelectDataSourceByType(type, "TERMS", NULL))
    {
        return 0;
    }

    switch (CurMediaType())
    {
#ifdef _CDENTREZ_
    case MEDIUM_CD:
    case MEDIUM_DISK:
        return CdEntrezFindTerm (type, field, term, spcl, totl);
#endif
#ifdef _NETENTREZ_
    case MEDIUM_NETWORK:
        return NetEntrezFindTerm (type, field, term, spcl, totl);
#endif
    default:
        return 0;
    }
}


/*****************************************************************************
*
*   EntrezUidLinks()
*       retrieves links to other uids
*
*****************************************************************************/
LinkSetPtr LIBCALL EntrezUidLinks (DocType type, DocUid uid, DocType link_to_type)

{
    LinkSetPtr lsp;

    if (! SelectDataLinksByTypes(type, link_to_type))
    {
        return NULL;
    }

    do {
        switch (CurMediaType())
        {
#ifdef _CDENTREZ_
        case MEDIUM_CD:
        case MEDIUM_DISK:
            if ((lsp = CdUidLinks(type, uid, link_to_type)) != NULL)
                return lsp;
            break;
#endif
#ifdef _NETENTREZ_
        case MEDIUM_NETWORK:
            if ((lsp = NetUidLinks(type, uid, link_to_type)) != NULL)
                return lsp;
            break;
#endif
        default:
            break;
        }
    } while (SelectNextDataSource());

    return NULL;
}


/*****************************************************************************
*
*   EntrezLinkUidList(type, link_to_type, numuid, uids)
*       returns count of input uids processed
*       returns -1 on error
*       if neighbors (type == link_to_type)
*           sums weights for same uids
*       if (more than EntrezUserMaxLinks() uids, frees uids and weights,
*           but leaves num set)
*
*****************************************************************************/
Int2 LIBCALL EntrezLinkUidList (LinkSetPtr PNTR result, DocType type, DocType link_to_type, Int2 numuid, Int4Ptr uids, Boolean mark_missing)

{
    Int2 obtained_ids = 0;
    Int4Ptr local_uids;
    Int4Ptr best_obtained_uids;
    Int2 best_obtained_ids;
    LinkSetPtr best_lsp = NULL;
    LinkSetPtr lsp;
    Int2 i;
    Int2 missing;
    Int2 fewest_missing = numuid + 1;
    Boolean badErr = FALSE;

    if (numuid == 0)
        return 0;

    if (! SelectDataLinksByTypes(type, link_to_type))
    { /* ERROR, data unobtainable */
        return 0;
    }

    local_uids = (Int4Ptr) MemNew(numuid * sizeof(Int4));
    best_obtained_uids = (Int4Ptr) MemNew(numuid * sizeof(Int4));

    /* Try the various available data sources, and return "successfully" if   */
    /* a data source is located with neighbors for all requested UIDs.        */
    /* If no such data source can be located, then try all data sources,      */
    /* selecting the one for which the most possible UIDs have neighbors.     */
    /* When two or more UIDs have the same number of UIDs with neighbors, use */
    /* the number of obtained neighbors as a tie-breaker.                     */

    do {
        MemCopy (local_uids, uids, numuid * sizeof(*uids));

        switch (CurMediaType())
        {
#ifdef _CDENTREZ_
        case MEDIUM_CD:
        case MEDIUM_DISK:
            obtained_ids =  CdLinkUidList(&lsp, type, link_to_type, numuid,
                                          local_uids, TRUE);
            break;
#endif
#ifdef _NETENTREZ_
        case MEDIUM_NETWORK:
            obtained_ids =  NetLinkUidList(&lsp, type, link_to_type, numuid,
                                           local_uids, TRUE);
            break;
#endif
        default:
            badErr = TRUE;
            break;
        }

        if (badErr)
            break;

        for (missing = 0, i = 0; i < numuid; i++)
        {
            if (local_uids[i] < 0)
                missing++;
        }

        if (missing == 0) /* success */
        {
            break;
        }

        if (missing <= fewest_missing)
        {
            if (missing < fewest_missing || best_lsp == NULL ||
                lsp->num > best_lsp->num)
            { /* found a better match */
                if (best_lsp != NULL)
                    LinkSetFree(best_lsp);
                best_lsp = lsp;
                fewest_missing = missing;
                best_obtained_ids = obtained_ids;
                MemCopy(best_obtained_uids, local_uids, numuid * sizeof(*uids));
            }
        }
        else {
            LinkSetFree(lsp);
        }
    } while (SelectNextDataSource()); /* until we've run out of options */

    if (missing == 0)
    { /* found neighbors for all UIDs */
        if (best_lsp != NULL)
            LinkSetFree(best_lsp);
        *result = lsp;
        if (mark_missing)
            MemCopy(uids, local_uids, numuid * sizeof(*uids));
        MemFree(local_uids);
        MemFree(best_obtained_uids);
        return obtained_ids;
    }
    else { /* couldn't find links for all; return best */
        *result = best_lsp;
        if (mark_missing)
            MemCopy(uids, best_obtained_uids, numuid * sizeof(*uids));
        MemFree(local_uids);
        MemFree(best_obtained_uids);
        return best_obtained_ids;
    }
}

/*****************************************************************************
*
*   EntrezMedlineEntryListGet (result, numuid, uids, mark_missing)
*       returns a count of entries read
*       if (mark_missing) ids which could not be located are made negative
*
*****************************************************************************/
Int2 LIBCALL EntrezMedlineEntryListGet(MedlineEntryPtr PNTR result, Int2 numuid,
                         Int4Ptr uids, Boolean mark_missing)
{
    Int2    obtained_ids = 0;
    Int4Ptr local_uids;
    int     unmatched;
    Int4Ptr map;
    Boolean first_time;
    MedlineEntryPtr PNTR res;
    Int2    i;

    if (numuid == 0)
        return 0;

    first_time = TRUE;

    if (! SelectDataSource(ENTR_REF_CHAN, "RECORDS", NULL)) /* MEDLINE DATA (Abstracts) */
    {
        /* ERROR, data unobtainable */
        return 0;
    }

    local_uids = (Int4Ptr) MemDup(uids, numuid * sizeof(*uids));
    res = (MedlineEntryPtr PNTR) MemNew(numuid * sizeof(MedlineEntryPtr));
    map = (Int4Ptr) MemNew(numuid * sizeof(Int4));

    do {
        switch (CurMediaType())
        {
#ifdef _CDENTREZ_
        case MEDIUM_CD:
        case MEDIUM_DISK:
            obtained_ids += CdEntMedlineEntryListGet (res, numuid, local_uids, TRUE);
        break;
#endif
#ifdef _NETENTREZ_
        case MEDIUM_NETWORK:
            obtained_ids += NetEntMedlineEntryListGet (res, numuid, local_uids, TRUE);
            break;
#endif
        default:
            break;
        }

        for (unmatched = 0, i = 0; i < numuid; i++)
        {
            if (local_uids[i] >= 0) /* found this one */
            {
                if (first_time)
                {
                    result[i] = res[i];
                }
                else
                { /* map the location */
                    result[map[i]] = res[i];
                }
            }
            else {
                if (first_time)
                {
                    result[i] = NULL;
                    if (mark_missing)
                        uids[i] = local_uids[i];
                    map[unmatched] = i;
                }
                else {
                    map[unmatched] = map[i];
                }
                local_uids[unmatched] = ABS(local_uids[i]);
                unmatched++;
            }
        }

        numuid = unmatched;
        first_time = FALSE;

    } while (numuid > 0 && SelectNextDataSource()); /* until we've run out of options */

    MemFree (local_uids);
    MemFree (res);
    MemFree (map);

    return obtained_ids;
}

/*****************************************************************************
*
*   EntrezMedlineEntryGet(uid)
*       get one MedlineEntry
*
*****************************************************************************/
MedlineEntryPtr LIBCALL EntrezMedlineEntryGet (Int4 uid)

{
    MedlineEntryPtr mep = NULL;

    EntrezMedlineEntryListGet(&mep, 1, &uid, FALSE);
    return mep;
}

/*****************************************************************************
*
*   EntrezSeqEntryListGet (result, numuid, uids, retcode, mark_missing)
*       returns a count of entries read
*       if (mark_missing) ids which could not be located are made negative
*       retcode is defined in objsset.h 
*
*****************************************************************************/
Int2 LIBCALL EntrezSeqEntryListGet (SeqEntryPtr PNTR result, Int2 numuid, Int4Ptr uids, Int2 retcode, Boolean mark_missing)

{
    Int2    obtained_ids = 0;
    Int4Ptr local_uids;
    int     unmatched;
    Int4Ptr map;
    Boolean first_time;
    SeqEntryPtr PNTR res;
    Int2    i;

    if (numuid == 0)
        return 0;

    first_time = TRUE;

    if (! SelectDataSource(ENTR_SEQ_CHAN, "RECORDS", NULL)) /* SEQUENCE DATA (Abstracts) */
    {
        /* ERROR, data unobtainable */
        return 0;
    }

    local_uids = (Int4Ptr) MemDup(uids, numuid * sizeof(*uids));
    res = (SeqEntryPtr PNTR) MemNew(numuid * sizeof(SeqEntryPtr));
    map = (Int4Ptr) MemNew(numuid * sizeof(Int4));

    do {
        switch (CurMediaType())
        {
#ifdef _CDENTREZ_
        case MEDIUM_CD:
        case MEDIUM_DISK:
            obtained_ids += CdEntSeqEntryListGet (res, numuid, local_uids,
                                               retcode, TRUE);
        break;
#endif
#ifdef _NETENTREZ_
        case MEDIUM_NETWORK:
            obtained_ids += NetEntSeqEntryListGet (res, numuid, local_uids,
                                                retcode, TRUE);
            break;
#endif
        default:
            break;
        }

        for (unmatched = 0, i = 0; i < numuid; i++)
        {
            if (local_uids[i] >= 0) /* found this one */
            {
                if (first_time)
                {
                    result[i] = res[i];
                }
                else
                { /* map the location */
                    result[map[i]] = res[i];
                }
            }
            else {
                if (first_time)
                {
                    result[i] = NULL;
                    if (mark_missing)
                        uids[i] = local_uids[i];
                    map[unmatched] = i;
                }
                else {
                    map[unmatched] = map[i];
                }
                local_uids[unmatched] = ABS(local_uids[i]);
                unmatched++;
            }
        }

        numuid = unmatched;
        first_time = FALSE;

    } while (numuid > 0 && SelectNextDataSource()); /* until we've run out of options */

    MemFree (local_uids);
    MemFree (res);
    MemFree (map);

    return obtained_ids;
}

/*****************************************************************************
*
*   EntrezSeqEntryGet(uid, retcode)
*       get one SeqEntry
*
*****************************************************************************/
SeqEntryPtr LIBCALL EntrezSeqEntryGet (Int4 uid, Int2 retcode)

{
    SeqEntryPtr sep = NULL;

    EntrezSeqEntryListGet(&sep, 1, &uid, retcode, FALSE);
    return sep;
}

/*****************************************************************************
*
*   EntrezFindSeqId(sip)
*       given a Seq-id, get the uid.
*       returns 0 on failure
*
*****************************************************************************/
Int4 LIBCALL EntrezFindSeqId (SeqIdPtr sip)
{
    Int4 uid = 0;
    DocType db = -1;
    TextSeqIdPtr tsip;
    PDBSeqIdPtr psip;
    PatentSeqIdPtr patsip;
    CharPtr locus = NULL;
    Char localbuf[40];
    ValNodePtr lst;
    LinkSetPtr lsp;
    Boolean check_both = FALSE, done;

    switch (sip->choice)
    {
        case SEQID_NOT_SET:           /* not set */
        case SEQID_LOCAL:           /* local */
            break;
        case SEQID_GIBBSQ:           /* gibbsq */
        case SEQID_GIBBMT:           /* gibbmt */
            sprintf(localbuf, "B%ld", (long)(sip->data.intvalue));
            locus = (CharPtr)localbuf;
            db = TYP_AA;   /* guess it's a protein */
            check_both = TRUE;
            break;             /* not on cdrom */
        case SEQID_GIIM:           /* giim */
            uid = ((GiimPtr)sip->data.ptrvalue)->id;
            break;
        case SEQID_GI:
            uid = sip->data.intvalue;
            break;
        case SEQID_GENBANK:             /* genbank */
        case SEQID_EMBL:                /* embl */
        case SEQID_DDBJ:
            db = TYP_NT;   /* guess it's a nucleic acid */
            check_both = TRUE;
        case SEQID_PIR:             /* pir */
        case SEQID_SWISSPROT:
        case SEQID_PRF:
            if (db < 0)
                db = TYP_AA;
            tsip = (TextSeqIdPtr)sip->data.ptrvalue;
            if (tsip->accession != NULL)
                locus = tsip->accession;
            else
                locus = tsip->name;
            break;
        case SEQID_PDB:
            psip = (PDBSeqIdPtr)(sip->data.ptrvalue);
            sprintf(localbuf, "%s %c", psip->mol, (Char)psip->chain);
            locus = localbuf;
            db = TYP_AA;   /* guess protein */
            check_both = TRUE;
            break;
        case SEQID_PATENT:
            patsip = (PatentSeqIdPtr)(sip->data.ptrvalue);
            sprintf(localbuf, "%s%s %d", patsip->cit->country, patsip->cit->number,
                (int)patsip->seqid);
            locus = localbuf;
            db = TYP_AA;   /* guess protein */
            check_both = TRUE;
            break;
        default:
            break;
    }

    if ((! uid) && (locus != NULL))   /* got a term to find */
    {
        done = FALSE;
        while (! done)     /* might need to check 2 types */
        {
            lst = EntrezTLNew(db);
            if (lst == NULL) return uid;
            EntrezTLAddTerm(lst, locus, db, FLD_ACCN, TRUE);
            lsp = EntrezTLEval(lst);
            EntrezTLFree(lst);
            if (lsp != NULL)
            {
                if (lsp->num > 0)   /* return first one */
                    uid = lsp->uids[0];
                LinkSetFree(lsp);
            }
            if ((! check_both) || (uid > 0))
                done = TRUE;
            else
            {
                if (db == TYP_AA)
                    db = TYP_NT;
                else
                    db = TYP_AA;
                check_both = FALSE;
            }
        }
    }

    return uid;
}


/*****************************************************************************
*
*   EntrezMlSumListGet (result, numuid, uids)
*       returns a count of entries read
*       head of linked list is in result
*
*****************************************************************************/
Int2 LIBCALL EntrezMlSumListGet (DocSumPtr PNTR result, Int2 numuid, Int4Ptr uids)

{
    Int2 obtained_ids = 0;
    Int4Ptr local_uids;
    Int2 unmatched;
    Int4Ptr map;
    Boolean first_time = TRUE;
    DocSumPtr PNTR res;
    Int2    i;

    if (numuid == 0)
        return 0;

    if (! SelectDataSource(ENTR_LINKS_CHAN, ENTR_REF_CHAN, ENTR_REF_CHAN))
    { /* ERROR, data unobtainable */
        return 0;
    }

    /* Iteratively try different data sources, working on an incrementally    */
    /* reduced number of UIDs, until either docsums have been found for all   */
    /* UIDs or,  all data sources have been exhausted. The algorithm uses a   */
    /* mapping mechanism to ensure that requests from a small subset of the   */
    /* initial requests always map their results back to the original result  */
    /* and uids vectors in a correct manner.                                  */

    local_uids = (Int4Ptr) MemDup(uids, numuid * sizeof(*uids));
    res = (DocSumPtr PNTR) MemNew(numuid * sizeof(DocSumPtr));
    map = (Int4Ptr) MemNew(numuid * sizeof(Int4));

    do {
        switch (CurMediaType())
        {
#ifdef _CDENTREZ_
        case MEDIUM_CD:
        case MEDIUM_DISK:
            obtained_ids = CdEntMlSumListGet (res, numuid, local_uids);
        break;
#endif
#ifdef _NETENTREZ_
        case MEDIUM_NETWORK:
            obtained_ids = NetDocSumListGet (res, numuid, TYP_ML, local_uids, 0);
            break;
#endif
        default:
            break;
        }

        for (unmatched = 0, i = 0; i < numuid; i++)
        {
            if (res[i] != NULL) /* found this one */
            {
                if (first_time)
                {
                    result[i] = res[i];
                }
                else
                { /* map the location */
                    result[map[i]] = res[i];
                }
            }
            else { /* not found */
                if (first_time)
                {
                    result[i] = NULL;
                    map[unmatched] = i;
                }
                else {
                    map[unmatched] = map[i];
                }
                local_uids[unmatched] = local_uids[i];
                unmatched++;
            }
        }
    
        numuid = unmatched;
        first_time = FALSE;

    } while (numuid > 0 && SelectNextDataSource()); /* until we've run out of options */

    MemFree (local_uids);
    MemFree (res);
    MemFree (map);

    return obtained_ids;
}

/*****************************************************************************
*
*   EntrezSeqSumListGet (result, numuid, uids)
*       returns a count of entries read
*       head of linked list is in result
*
*****************************************************************************/
Int2 LIBCALL EntrezSeqSumListGet (DocSumPtr PNTR result, Int2 numuid, DocType type, Int4Ptr uids)          /* Gi numbers */

{
    Int2      obtained_ids = 0;
    Int4Ptr   local_uids;
    Int2      unmatched;
    Int4Ptr   map;
    Boolean   first_time = TRUE;
    DocSumPtr PNTR res;
    Int2      i;

    if (numuid == 0)
        return 0;

    if (! SelectDataSource(ENTR_LINKS_CHAN, ENTR_SEQ_CHAN, ENTR_SEQ_CHAN))
    { /* ERROR, data unobtainable */
        return 0;
    }

    /* Iteratively try different data sources, working on an incrementally    */
    /* reduced number of UIDs, until either docsums have been found for all   */
    /* UIDs or,  all data sources have been exhausted. The algorithm uses a   */
    /* mapping mechanism to ensure that requests from a small subset of the   */
    /* initial requests always map their results back to the original result  */
    /* and uids vectors in a correct manner.                                  */

    local_uids = (Int4Ptr) MemDup(uids, numuid * sizeof(*uids));
    res = (DocSumPtr PNTR) MemNew(numuid * sizeof(DocSumPtr));
    map = (Int4Ptr) MemNew(numuid * sizeof(Int4));

    do {
        switch (CurMediaType())
        {
#ifdef _CDENTREZ_
        case MEDIUM_CD:
        case MEDIUM_DISK:
            obtained_ids = CdEntSeqSumListGet (res, numuid, type, local_uids);
        break;
#endif
#ifdef _NETENTREZ_
        case MEDIUM_NETWORK:
            obtained_ids = NetDocSumListGet (res, numuid, type, local_uids, 0);
            break;
#endif
        default:
            break;
        }

        for (unmatched = 0, i = 0; i < numuid; i++)
        {
            if (res[i] != NULL) /* found this one */
            {
                if (first_time)
                {
                    result[i] = res[i];
                }
                else
                { /* map the location */
                    result[map[i]] = res[i];
                }
            }
            else { /* not found */
                if (first_time)
                {
                    result[i] = NULL;
                    map[unmatched] = i;
                }
                else {
                    map[unmatched] = map[i];
                }
                local_uids[unmatched] = local_uids[i];
                unmatched++;
            }
        }
    
        numuid = unmatched;
        first_time = FALSE;

    } while (numuid > 0 && SelectNextDataSource()); /* until we've run out of options */

    MemFree (local_uids);
    MemFree (res);
    MemFree (map);

    return obtained_ids;
}

