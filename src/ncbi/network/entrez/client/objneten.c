/*   objneten.c
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
* File Name:  objneten.c
*
* Author:  Epstein
*
* Version Creation Date:   06/02/92
*
* $Revision: 1.5 $
*
* File Description: 
*   	object loaders for Network Entrez
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/

#include <asnneten.h> /* the AsnTool header */
#include <objacces.h>
#include <accentr.h>
#include <objmedli.h> /* the Medline interface */
#include <objloc.h>   /* the 'Seqloc' interface */
#include <objsset.h>  /* the 'SeqSet' interface */
#include <objneten.h> /* the Entrez objects interface */
#include <objall.h>


static Boolean loaded = FALSE;

/*****************************************************************************
*
*   NetEntAsnLoad()
*
*****************************************************************************/
Boolean CDECL
NetEntAsnLoad(void)
{
    if (loaded)
        return TRUE;
	loaded = TRUE;

	if (! AllObjLoad()) {
		loaded = FALSE;
		return FALSE;
	}

	if (! AsnLoad()) {
		loaded = FALSE;
		return FALSE;
	}

	return TRUE;
}


/************************** EntrezIds ****************************************/

EntrezIdsPtr CDECL
EntrezIdsNew(void)
{
	EntrezIdsPtr p;

	p = MemNew(sizeof(EntrezIds));
	if (p != NULL)
	{
		p->numid = 0;
		p->ids = NULL;
	}
	return p;
}

EntrezIdsPtr CDECL
EntrezIdsFree(EntrezIdsPtr p)
{
	if (p == NULL)
		return NULL;
	MemFree (p->ids);
	return MemFree(p);
}
		
EntrezIdsPtr CDECL
EntrezIdsAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	EntrezIdsPtr p;
	AsnTypePtr atp;
	Int4 num;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, ENTREZ_IDS);
	else
		atp = AsnLinkType(orig, ENTREZ_IDS); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = EntrezIdsNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;
	atp = AsnReadId(aip, amp, atp); /* find the num */
	if (atp == NULL)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0) /* get the num */
		goto erret;
	p->numid = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_IDS_ids)
		goto erret;

	if (AsnReadVal(aip, atp, NULL) <= 0) /* discard NULL */
		goto erret;

	p->ids = (DocUidPtr) MemNew(sizeof(DocUid) * (size_t) p->numid);
	atp = AsnReadId(aip, amp, atp);

	for (num = 0; num < p->numid && atp == ENTREZ_IDS_ids_E; num++)
	{
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		p->ids[num] = av.intvalue;
		atp = AsnReadId(aip, amp, atp);
	}

	/* check for count mis-match */
	if (num != p->numid || atp != ENTREZ_IDS_ids)
		goto erret;

	if (AsnReadVal(aip, atp, NULL) <= 0) /* discard NULL */
		goto erret;

	atp = AsnReadId(aip, amp, atp);
	if (orig == NULL)
	{
		if (atp != ENTREZ_IDS)
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
	return p;

erret:
	p = EntrezIdsFree(p);
	goto ret;
}
		

Boolean CDECL
EntrezIdsAsnWrite (EntrezIdsPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Int4 i;
	AsnTypePtr atp;
	Boolean retval = FALSE;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, ENTREZ_IDS); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}


	if (! AsnStartStruct(aip, atp))
		goto erret;
	av.intvalue = p->numid;
	AsnWrite (aip, ENTREZ_IDS_numid, &av);

	AsnStartStruct (aip, ENTREZ_IDS_ids);
	for (i = 0; i < p->numid; i++)
	{
		av.intvalue = p->ids[i];
		AsnWrite (aip, ENTREZ_IDS_ids_E, &av);
	}
	AsnEndStruct (aip, ENTREZ_IDS_ids);

	if (! AsnEndStruct(aip, atp) )
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}

/************************** Marked-link-set **********************************/

MarkedLinkSetPtr CDECL
MarkedLinkSetNew(void)
{
	MarkedLinkSetPtr p;

	p = MemNew(sizeof(MarkedLinkSet));
	if (p != NULL)
	{
		p->link_set = NULL;
		p->marked_missing = NULL;
	}
	return p;
}

MarkedLinkSetPtr CDECL
MarkedLinkSetFree(MarkedLinkSetPtr p)
{
	if (p == NULL)
		return NULL;
	LinkSetFree (p->link_set);
	EntrezIdsFree (p->marked_missing);

	return MemFree(p);
}

MarkedLinkSetPtr CDECL
MarkedLinkSetAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	MarkedLinkSetPtr p;
	EntrezIdsPtr eip;
	LinkSetPtr lsp;
	AsnTypePtr atp;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, MARKED_LINK_SET);
	else
		atp = AsnLinkType(orig, MARKED_LINK_SET); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = MarkedLinkSetNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	if ((atp = AsnReadId(aip, amp, atp)) != MARKED_LINK_SET_link_set)
		goto erret;

	lsp = LinkSetAsnRead(aip, atp);
	if (lsp == NULL)
		goto erret;
	p->link_set = lsp;

	if ((atp = AsnReadId(aip, amp, atp)) != MARKED_LINK_SET_uids_processed)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->uids_processed = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) == MARKED_LINK_SET_marked_missing)
	{ /* optional */
		eip = EntrezIdsAsnRead(aip, atp);
		if (eip == NULL)
			goto erret;
		p->marked_missing = eip;
		atp = AsnReadId(aip, amp, atp);
	}

	if (orig == NULL)
	{
		if (atp != MARKED_LINK_SET)
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
	return p;

erret:
	p = MarkedLinkSetFree(p);
	goto ret;
}

Boolean CDECL
MarkedLinkSetAsnWrite (MarkedLinkSetPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Boolean retval = FALSE;
	AsnTypePtr atp;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, MARKED_LINK_SET); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct (aip, atp))
		goto erret;

	if (! LinkSetAsnWrite(p->link_set, aip, MARKED_LINK_SET_link_set) )
		goto erret;
	
	av.intvalue = p->uids_processed;
	AsnWrite(aip, MARKED_LINK_SET_uids_processed, &av);

	if (p->marked_missing != NULL) /* optional */
	{
		if (! EntrezIdsAsnWrite(p->marked_missing, aip, MARKED_LINK_SET_marked_missing) )
			goto erret;
	}

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}

/************************** Named-List **********************************/

NamedListPtr CDECL
NamedListNew(void)
{
	NamedListPtr p;

	p = MemNew(sizeof(NamedList));
	p->uids = NULL;
	return p;
}

NamedListPtr CDECL
NamedListFree(NamedListPtr p)
{
	if (p == NULL)
		return NULL;
	EntrezIdsFree(p->uids);

	return MemFree(p);
}

NamedListPtr CDECL
NamedListAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	NamedListPtr p;
	AsnTypePtr atp;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, ENTREZ_NAMED_LIST);
	else
		atp = AsnLinkType(orig, ENTREZ_NAMED_LIST); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = NamedListNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_NAMED_LIST_term)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->term = av.ptrvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_NAMED_LIST_type)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->type = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_NAMED_LIST_fld)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->fld = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_NAMED_LIST_uids)
		goto erret;
	p->uids = EntrezIdsAsnRead(aip, atp);
	if (p->uids == NULL)
		goto erret;

	atp = AsnReadId(aip, amp, atp);
	if (orig == NULL)
	{
		if (atp != ENTREZ_NAMED_LIST)
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
	return p;

erret:
	p = NamedListFree(p);
	goto ret;
}

Boolean CDECL
NamedListAsnWrite (NamedListPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Boolean retval = FALSE;
	AsnTypePtr atp;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, ENTREZ_NAMED_LIST); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;

	av.ptrvalue = p->term;
	AsnWrite (aip, ENTREZ_NAMED_LIST_term, &av);
	av.intvalue = p->type;
	AsnWrite (aip, ENTREZ_NAMED_LIST_type, &av);
	av.intvalue = p->fld;
	AsnWrite (aip, ENTREZ_NAMED_LIST_fld, &av);
    if (! EntrezIdsAsnWrite(p->uids, aip, ENTREZ_NAMED_LIST_uids) )
		goto erret;

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}

/************************** Term-by-page **********************************/

TermByPagePtr CDECL
TermByPageNew(void)
{
	TermByPagePtr p;

	p = MemNew(sizeof(TermByPage));
	return p;
}

TermByPagePtr CDECL
TermByPageFree(TermByPagePtr p)
{
	if (p == NULL)
		return NULL;

	return MemFree(p);
}

TermByPagePtr CDECL
TermByPageAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	TermByPagePtr p;
	AsnTypePtr atp;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, ENTREZ_TERM_BY_PAGE);
	else
		atp = AsnLinkType(orig, ENTREZ_TERM_BY_PAGE); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = TermByPageNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_TERM_BY_PAGE_type)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->type = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_TERM_BY_PAGE_fld)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->fld = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_TERM_BY_PAGE_page)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->page = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_TERM_BY_PAGE_num_pages)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->num_pages = av.intvalue;

	atp = AsnReadId(aip, amp, atp);
	if (orig == NULL)
	{
		if (atp != ENTREZ_TERM_BY_PAGE)
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
	return p;

erret:
	p = TermByPageFree(p);
	goto ret;
}

Boolean CDECL
TermByPageAsnWrite (TermByPagePtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Boolean retval = FALSE;
	AsnTypePtr atp;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, ENTREZ_TERM_BY_PAGE); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;

	av.intvalue = p->type;
	AsnWrite (aip, ENTREZ_TERM_BY_PAGE_type, &av);
	av.intvalue = p->fld;
	AsnWrite (aip, ENTREZ_TERM_BY_PAGE_fld, &av);
	av.intvalue = p->page;
	AsnWrite (aip, ENTREZ_TERM_BY_PAGE_page, &av);
	av.intvalue = p->num_pages;
	AsnWrite (aip, ENTREZ_TERM_BY_PAGE_num_pages, &av);

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}

/************************** Term-by-term **********************************/

TermByTermPtr CDECL
TermByTermNew(void)
{
	TermByTermPtr p;

	p = MemNew(sizeof(TermByTerm));
	p->term = NULL;
	return p;
}

TermByTermPtr CDECL
TermByTermFree(TermByTermPtr p)
{
	if (p == NULL)
		return NULL;
	MemFree (p->term);

	return MemFree(p);
}

TermByTermPtr CDECL
TermByTermAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	TermByTermPtr p;
	AsnTypePtr atp;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, ENTREZ_TERM_BY_TERM);
	else
		atp = AsnLinkType(orig, ENTREZ_TERM_BY_TERM); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = TermByTermNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_TERM_BY_TERM_type)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->type = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_TERM_BY_TERM_fld)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->fld = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_TERM_BY_TERM_term)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->term = av.ptrvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_TERM_BY_TERM_num_terms)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->num_terms = av.intvalue;

	atp = AsnReadId(aip, amp, atp);
	if (orig == NULL)
	{
		if (atp != ENTREZ_TERM_BY_TERM)
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
	return p;

erret:
	p = TermByTermFree(p);
	goto ret;
}

Boolean CDECL
TermByTermAsnWrite (TermByTermPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Boolean retval = FALSE;
	AsnTypePtr atp;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, ENTREZ_TERM_BY_TERM); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;

	av.intvalue = p->type;
	AsnWrite (aip, ENTREZ_TERM_BY_TERM_type, &av);
	av.intvalue = p->fld;
	AsnWrite (aip, ENTREZ_TERM_BY_TERM_fld, &av);
	av.ptrvalue = p->term;
	AsnWrite (aip, ENTREZ_TERM_BY_TERM_term, &av);
	av.intvalue = p->num_terms;
	AsnWrite (aip, ENTREZ_TERM_BY_TERM_num_terms, &av);

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}

/************************** Term-Lookup **********************************/

TermLookupPtr CDECL
TermLookupNew(void)
{
	TermLookupPtr p;

	p = MemNew(sizeof(TermLookup));
	p->term = NULL;
	return p;
}

TermLookupPtr CDECL
TermLookupFree(TermLookupPtr p)
{
	if (p == NULL)
		return NULL;
	MemFree (p->term);

	return MemFree(p);
}

TermLookupPtr CDECL
TermLookupAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	TermLookupPtr p;
	AsnTypePtr atp;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, TERM_LOOKUP);
	else
		atp = AsnLinkType(orig, TERM_LOOKUP); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = TermLookupNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	if ((atp = AsnReadId(aip, amp, atp)) != TERM_LOOKUP_type)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->type = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != TERM_LOOKUP_fld)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->fld = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != TERM_LOOKUP_term)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->term = av.ptrvalue;

	atp = AsnReadId(aip, amp, atp);
	if (orig == NULL)
	{
		if (atp != TERM_LOOKUP)
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
	return p;

erret:
	p = TermLookupFree(p);
	goto ret;
}

Boolean CDECL
TermLookupAsnWrite (TermLookupPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Boolean retval = FALSE;
	AsnTypePtr atp;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, TERM_LOOKUP); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;

	av.intvalue = p->type;
	AsnWrite (aip, TERM_LOOKUP_type, &av);
	av.intvalue = p->fld;
	AsnWrite (aip, TERM_LOOKUP_fld, &av);
	av.ptrvalue = p->term;
	AsnWrite (aip, TERM_LOOKUP_term, &av);

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}

/************************** Term-Page-Info **********************************/

TermPageInfoPtr CDECL
TermPageInfoNew(void)
{
	TermPageInfoPtr p;

	p = MemNew(sizeof(TermPageInfo));
	p->term = NULL;
	return p;
}

TermPageInfoPtr CDECL
TermPageInfoFree(TermPageInfoPtr p)
{
	if (p == NULL)
		return NULL;
	MemFree (p->term);

	return MemFree(p);
}

TermPageInfoPtr CDECL
TermPageInfoAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	TermPageInfoPtr p;
	AsnTypePtr atp;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, TERM_PAGE_INFO);
	else
		atp = AsnLinkType(orig, TERM_PAGE_INFO); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = TermPageInfoNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	if ((atp = AsnReadId(aip, amp, atp)) != TERM_PAGE_INFO_term)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->term = av.ptrvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != TERM_PAGE_INFO_spec_count)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->spec_count = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != TERM_PAGE_INFO_tot_count)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->tot_count = av.intvalue;

	atp = AsnReadId(aip, amp, atp);
	if (orig == NULL)
	{
		if (atp != TERM_PAGE_INFO)
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
	return p;

erret:
	p = TermPageInfoFree(p);
	goto ret;
}

Boolean CDECL
TermPageInfoAsnWrite (TermPageInfoPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Boolean retval = FALSE;
	AsnTypePtr atp;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, TERM_PAGE_INFO); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;

	av.ptrvalue = p->term;
	AsnWrite (aip, TERM_PAGE_INFO_term, &av);
	av.intvalue = p->spec_count;
	AsnWrite (aip, TERM_PAGE_INFO_spec_count, &av);
	av.intvalue = p->tot_count;
	AsnWrite (aip, TERM_PAGE_INFO_tot_count, &av);

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}

/************************** Term-Counts **********************************/

TermCountsPtr CDECL
TermCountsNew(void)
{
	TermCountsPtr p;

	p = MemNew(sizeof(TermCounts));
	return p;
}

TermCountsPtr CDECL
TermCountsFree(TermCountsPtr p)
{
	if (p == NULL)
		return NULL;

	return MemFree(p);
}

TermCountsPtr CDECL
TermCountsAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	TermCountsPtr p;
	AsnTypePtr atp;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, TERM_COUNTS);
	else
		atp = AsnLinkType(orig, TERM_COUNTS); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = TermCountsNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	if ((atp = AsnReadId(aip, amp, atp)) != TERM_COUNTS_found)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->found = av.boolvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != TERM_COUNTS_spec_count)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->spec_count = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != TERM_COUNTS_tot_count)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->tot_count = av.intvalue;

	atp = AsnReadId(aip, amp, atp);
	if (orig == NULL)
	{
		if (atp != TERM_COUNTS)
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
	return p;

erret:
	p = TermCountsFree(p);
	goto ret;
}

Boolean CDECL
TermCountsAsnWrite (TermCountsPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Boolean retval = FALSE;
	AsnTypePtr atp;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, TERM_COUNTS); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;

	av.boolvalue = p->found;
	AsnWrite (aip, TERM_COUNTS_found, &av);
	av.intvalue = p->spec_count;
	AsnWrite (aip, TERM_COUNTS_spec_count, &av);
	av.intvalue = p->tot_count;
	AsnWrite (aip, TERM_COUNTS_tot_count, &av);

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}

/************************** LinkSetGet ****************************************/

LinkSetGetPtr CDECL
LinkSetGetNew(void)
{
	LinkSetGetPtr p;

	p = MemNew(sizeof(LinkSetGet));
	if (p != NULL)
	{
		p->query_size = 0;
		p->query = NULL;
	}
	return p;
}

LinkSetGetPtr CDECL
LinkSetGetFree(LinkSetGetPtr p)
{
	if (p == NULL)
		return NULL;
	MemFree (p->query);
	return MemFree(p);
}
		
LinkSetGetPtr CDECL
LinkSetGetAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	LinkSetGetPtr p;
	AsnTypePtr atp;
	Int4 num;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, LINK_SETGET);
	else
		atp = AsnLinkType(orig, LINK_SETGET); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = LinkSetGetNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	atp = AsnReadId(aip, amp, atp); /* find the num */
	if (atp == NULL)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0) /* get the num */
		goto erret;
	p->max = av.intvalue;

	atp = AsnReadId(aip, amp, atp);
	if (atp == NULL)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->link_to_cls = av.intvalue;

	atp = AsnReadId(aip, amp, atp);
	if (atp == NULL)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->query_cls = av.intvalue;

	atp = AsnReadId(aip, amp, atp);
	if (atp == LINK_SETGET_mark_missing)
	{
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		p->mark_missing = av.boolvalue;
		atp = AsnReadId(aip, amp, atp);
	}

	if (atp != LINK_SETGET_query_size)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->query_size = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != LINK_SETGET_query)
		goto erret;
	if (AsnReadVal(aip, atp, NULL) <= 0) /* discard NULL */
		goto erret;

	p->query = (DocUidPtr) MemNew(sizeof(DocUid) * (size_t) p->query_size);
	atp = AsnReadId(aip, amp, atp);

	for (num = 0; num < p->query_size && atp == LINK_SETGET_query_E; num++)
	{
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		p->query[num] = av.intvalue;
		atp = AsnReadId(aip, amp, atp);
	}

	/* check for count mis-match */
	if (num != p->query_size || atp != LINK_SETGET_query)
		goto erret;

	if (AsnReadVal(aip, atp, NULL) <= 0) /* discard NULL */
		goto erret;

	atp = AsnReadId(aip, amp, atp);
	if (orig == NULL)
	{
		if (atp != LINK_SETGET)
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
	return p;

erret:
	p = LinkSetGetFree(p);
	goto ret;
}
		

Boolean CDECL
LinkSetGetAsnWrite (LinkSetGetPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Int4 i;
	AsnTypePtr atp;
	Boolean retval = FALSE;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, LINK_SETGET); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}


	if (! AsnStartStruct(aip, atp))
		goto erret;
	av.intvalue = p->max;
	AsnWrite (aip, LINK_SETGET_max, &av);
	av.intvalue = p->link_to_cls;
	AsnWrite (aip, LINK_SETGET_link_to_cls, &av);
	av.intvalue = p->query_cls;
	AsnWrite (aip, LINK_SETGET_query_cls, &av);
	av.boolvalue = p->mark_missing;
	AsnWrite (aip, LINK_SETGET_mark_missing, &av);
	av.intvalue = p->query_size;
	AsnWrite (aip, LINK_SETGET_query_size, &av);

	AsnStartStruct (aip, LINK_SETGET_query);
	for (i = 0; i < p->query_size; i++)
	{
		av.intvalue = p->query[i];
		AsnWrite (aip, LINK_SETGET_query_E, &av);
	}
	AsnEndStruct (aip, LINK_SETGET_query);

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}

/************************** Medline-Summary **********************************/

DocSumPtr CDECL
MlSumNew(void)
{
	DocSumPtr p;

	p = MemNew(sizeof(DocSum));
	p->caption = NULL;
	p->title = NULL;
	return p;
}

DocSumPtr CDECL
MlSumFree(DocSumPtr p)
{
	if (p == NULL)
		return NULL;
	MemFree(p->caption);
	MemFree(p->title);

	return MemFree(p);
}

DocSumPtr CDECL
MlSumAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	DocSumPtr p;
	AsnTypePtr atp;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, ML_SUMMARY);
	else
		atp = AsnLinkType(orig, ML_SUMMARY); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = MlSumNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	if ((atp = AsnReadId(aip, amp, atp)) != ML_SUMMARY_muid)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->uid = av.intvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != ML_SUMMARY_no_abstract)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->no_abstract = av.boolvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != ML_SUMMARY_translated_title)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->translated_title = av.boolvalue;

	if ((atp = AsnReadId(aip, amp, atp)) != ML_SUMMARY_no_authors)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->no_authors = av.boolvalue;

	atp = AsnReadId(aip, amp, atp);
	if (atp == ML_SUMMARY_caption)
	{
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		p->caption = av.ptrvalue;
		atp = AsnReadId(aip, amp, atp);
	}

	if (atp == ML_SUMMARY_title)
	{
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		p->title = av.ptrvalue;
		atp = AsnReadId(aip, amp, atp);
	}

	if (orig == NULL)
	{
		if (atp != ML_SUMMARY)
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
	return p;

erret:
	p = DocSumFree(p);
	goto ret;
}

Boolean CDECL
MlSumAsnWrite (DocSumPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Boolean retval = FALSE;
	AsnTypePtr atp;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, ML_SUMMARY); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;

	av.intvalue = p->uid;
	AsnWrite (aip, ML_SUMMARY_muid, &av);
	av.boolvalue = p->no_abstract;
	AsnWrite (aip, ML_SUMMARY_no_abstract, &av);
	av.boolvalue = p->translated_title;
	AsnWrite (aip, ML_SUMMARY_translated_title, &av);
	av.boolvalue = p->no_authors;
	AsnWrite (aip, ML_SUMMARY_no_authors, &av);
	if (p->caption != NULL)
	{
		av.ptrvalue = p->caption;
		AsnWrite (aip, ML_SUMMARY_caption, &av);
	}
	if (p->title != NULL)
	{
		av.ptrvalue = p->title;
		AsnWrite (aip, ML_SUMMARY_title, &av);
	}

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}

/************************** Sequence-Summary **********************************/

DocSumPtr CDECL
SeqSumNew(void)
{
	DocSumPtr p;

	p = MemNew(sizeof(DocSum));
	p->caption = NULL;
	p->title = NULL;
	return p;
}

DocSumPtr CDECL
SeqSumFree(DocSumPtr p)
{
	if (p == NULL)
		return NULL;
	MemFree(p->caption);
	MemFree(p->title);

	return MemFree(p);
}

DocSumPtr CDECL
SeqSumAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	DocSumPtr p;
	AsnTypePtr atp;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, SEQ_SUMMARY);
	else
		atp = AsnLinkType(orig, SEQ_SUMMARY); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = SeqSumNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	if ((atp = AsnReadId(aip, amp, atp)) != SEQ_SUMMARY_id)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->uid = av.intvalue;

	atp = AsnReadId(aip, amp, atp);
	if (atp == SEQ_SUMMARY_caption)
	{
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		p->caption = av.ptrvalue;
		atp = AsnReadId(aip, amp, atp);
	}

	if (atp == SEQ_SUMMARY_title)
	{
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		p->title = av.ptrvalue;
		atp = AsnReadId(aip, amp, atp);
	}

	if (orig == NULL)
	{
		if (atp != SEQ_SUMMARY)
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
	return p;

erret:
	p = DocSumFree(p);
	goto ret;
}

Boolean CDECL
SeqSumAsnWrite (DocSumPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Boolean retval = FALSE;
	AsnTypePtr atp;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, SEQ_SUMMARY); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;

	av.intvalue = p->uid;
	AsnWrite (aip, SEQ_SUMMARY_id, &av);
	if (p->caption != NULL)
	{
		av.ptrvalue = p->caption;
		AsnWrite (aip, SEQ_SUMMARY_caption, &av);
	}
	if (p->title != NULL)
	{
		av.ptrvalue = p->title;
		AsnWrite (aip, SEQ_SUMMARY_title, &av);
	}

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}

/************************** Entrez-DocGet **********************************/

EntrezDocGetPtr CDECL
EntrezDocGetNew(void)
{
	EntrezDocGetPtr p;

	p = MemNew(sizeof(EntrezDocGet));
	p->ids = NULL;
	p->defer_count = 0;
	return p;
}

EntrezDocGetPtr CDECL
EntrezDocGetFree(EntrezDocGetPtr p)
{
	if (p == NULL)
		return NULL;
	EntrezIdsFree(p->ids);

	return MemFree(p);
}

EntrezDocGetPtr CDECL
EntrezDocGetAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	EntrezDocGetPtr p;
	AsnTypePtr atp;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, ENTREZ_DOCGET);
	else
		atp = AsnLinkType(orig, ENTREZ_DOCGET); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = EntrezDocGetNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	if ((atp = AsnReadId(aip, amp, atp)) != ENTREZ_DOCGET_class)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->cls = av.intvalue;

	p->mark_missing = FALSE;
	if ((atp = AsnReadId(aip, amp, atp)) == ENTREZ_DOCGET_mark_missing)
	{
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		p->mark_missing = av.boolvalue;
		atp = AsnReadId(aip, amp, atp);
	}

	if (atp != ENTREZ_DOCGET_ids)
		goto erret;
	p->ids = EntrezIdsAsnRead(aip, atp);
	if (p->ids == NULL)
		goto erret;

	p->defer_count = 0;
	if ((atp = AsnReadId(aip, amp, atp)) == ENTREZ_DOCGET_defer_count)
	{
		if (AsnReadVal(aip, atp, &av) < 0)
			goto erret;
		p->defer_count = av.intvalue;
		atp = AsnReadId(aip, amp, atp);
	}

	if (orig == NULL)
	{
		if (atp != ENTREZ_DOCGET)
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
	return p;

erret:
	p = EntrezDocGetFree(p);
	goto ret;
}

Boolean CDECL
EntrezDocGetAsnWrite (EntrezDocGetPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Boolean retval = FALSE;
	AsnTypePtr atp;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, ENTREZ_DOCGET); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;

	av.intvalue = p->cls;
	AsnWrite (aip, ENTREZ_DOCGET_class, &av);
	av.boolvalue = p->mark_missing;
	AsnWrite (aip, ENTREZ_DOCGET_mark_missing, &av);
    if (! EntrezIdsAsnWrite(p->ids, aip, ENTREZ_DOCGET_ids) )
		goto erret;
	av.intvalue = p->defer_count;
	AsnWrite (aip, ENTREZ_DOCGET_defer_count, &av);

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}


/************************** MedlineEntryList *********************************/

MedlineEntryListPtr CDECL
MedlineEntryListNew(void)
{
	MedlineEntryListPtr p;

	p = MemNew(sizeof(MedlineEntryList));
	if (p != NULL)
	{
		p->num = 0;
		p->data = NULL;
		p->marked_missing = NULL;
	}
	return p;
}

MedlineEntryListPtr CDECL
MedlineEntryListFree(MedlineEntryListPtr p)
{
	Int4 i;

	if (p == NULL)
		return NULL;
	if (p->data != NULL)
	{
		for (i = 0; i < p->num; i++)
			MedlineEntryFree(p->data[i]);
	}
	MemFree (p->data);
	EntrezIdsFree (p->marked_missing);
	return MemFree(p);
}
		
MedlineEntryListPtr CDECL
MedlineEntryListAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	MedlineEntryListPtr p;
	AsnTypePtr atp;
	Int4 num;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, ENTREZ_MEDLINE_ENTRY_LIST);
	else
		atp = AsnLinkType(orig, ENTREZ_MEDLINE_ENTRY_LIST); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = MedlineEntryListNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	atp = AsnReadId(aip, amp, atp); /* find the num */
	if (atp == NULL)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0) /* get the num */
		goto erret;
	p->num = av.intvalue;

	atp = AsnReadId(aip, amp, atp); /* find the data start-struct */
	if (atp == NULL)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0) /* get the data start-struct */
		goto erret;

	p->data = (MedlineEntryPtr PNTR) MemNew(sizeof(MedlineEntryPtr) * p->num);
	for (num = 0; num < p->num; num++)
		p->data[num] = NULL;
	atp = AsnReadId(aip, amp, atp);

	for (num = 0; num < p->num && atp == MEDLINE_ENTRY_LIST_data_E; num++)
	{
		if ((p->data[num] = MedlineEntryAsnRead(aip, atp)) == NULL)
			goto erret;
		atp = AsnReadId(aip, amp, atp);
	}

	/* check for count mis-match */
	if (num != p->num)
		goto erret;
	
	if (AsnReadVal(aip, atp, NULL) <= 0) /* discard NULL */
		goto erret;

	atp = AsnReadId(aip, amp, atp);

	if (atp == ENTRY_LIST_marked_missing)
	{
		p->marked_missing = EntrezIdsAsnRead(aip, atp);
		atp = AsnReadId(aip, amp, atp);
	}

	if (orig == NULL)
	{
		if (atp != ENTREZ_MEDLINE_ENTRY_LIST)
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
	return p;

erret:
	p = MedlineEntryListFree(p);
	goto ret;
}
		

Boolean CDECL
MedlineEntryListAsnWrite (MedlineEntryListPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Int4 i;
	AsnTypePtr atp;
	Boolean retval = FALSE;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, ENTREZ_MEDLINE_ENTRY_LIST); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;
	av.intvalue = p->num;
	AsnWrite (aip, ENTREZ_MEDLINE_ENTRY_LIST_num, &av);

	AsnStartStruct (aip, ENTREZ_MEDLINE_ENTRY_LIST_data);
	for (i = 0; i < p->num; i++)
	{
		MedlineEntryAsnWrite(p->data[i], aip, MEDLINE_ENTRY_LIST_data_E);
	}
	AsnEndStruct (aip, ENTREZ_MEDLINE_ENTRY_LIST_data);

	if (p->marked_missing != NULL) /* optional */
	{
		EntrezIdsAsnWrite(p->marked_missing, aip, ENTRY_LIST_marked_missing);
	}

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}


/************************** MlSummaryList ****************************************/

MlSummaryListPtr CDECL
MlSummaryListNew(void)
{
	MlSummaryListPtr p;

	p = MemNew(sizeof(MlSummaryList));
	if (p != NULL)
	{
		p->num = 0;
		p->data = NULL;
	}
	return p;
}

MlSummaryListPtr CDECL
MlSummaryListFree(MlSummaryListPtr p)
{
	Int4 i;

	if (p == NULL)
		return NULL;
	if (p->data != NULL)
	{
		for (i = 0; i < p->num; i++)
			MlSumFree(p->data[i]);
	}
	MemFree (p->data);
	return MemFree(p);
}
		
MlSummaryListPtr CDECL
MlSummaryListAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	MlSummaryListPtr p;
	AsnTypePtr atp;
	Int4 num;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, ML_SUMMARY_LIST);
	else
		atp = AsnLinkType(orig, ML_SUMMARY_LIST); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = MlSummaryListNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	atp = AsnReadId(aip, amp, atp); /* find the num */
	if (atp == NULL)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0) /* get the num */
		goto erret;
	p->num = av.intvalue;

	atp = AsnReadId(aip, amp, atp); /* find the data start-struct */
	if (atp == NULL)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0) /* get the data start-struct */
		goto erret;

	p->data = (DocSumPtr PNTR) MemNew(sizeof(DocSumPtr) * p->num);
	for (num = 0; num < p->num; num++)
		p->data[num] = NULL;
	atp = AsnReadId(aip, amp, atp);

	for (num = 0; num < p->num && atp == ML_SUMMARY_LIST_data_E; num++)
	{
		if ((p->data[num] = MlSumAsnRead(aip, atp)) == NULL)
			goto erret;
		atp = AsnReadId(aip, amp, atp);
	}

	/* check for count mis-match */
	if (num != p->num || atp != ML_SUMMARY_LIST_data)
		goto erret;

	if (AsnReadVal(aip, atp, NULL) <= 0) /* discard NULL */
		goto erret;

	atp = AsnReadId(aip, amp, atp);
	if (orig == NULL)
	{
		if (atp != ML_SUMMARY_LIST)
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
	return p;

erret:
	p = MlSummaryListFree(p);
	goto ret;
}
		

Boolean CDECL
MlSummaryListAsnWrite (MlSummaryListPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Int4 i;
	AsnTypePtr atp;
	Boolean retval = FALSE;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, ML_SUMMARY_LIST); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;
	av.intvalue = p->num;
	AsnWrite (aip, ML_SUMMARY_LIST_num, &av);

	AsnStartStruct (aip, ML_SUMMARY_LIST_data);
	for (i = 0; i < p->num; i++)
	{
		MlSumAsnWrite(p->data[i], aip, ML_SUMMARY_LIST_data_E);
	}
	AsnEndStruct (aip, ML_SUMMARY_LIST_data);

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}

/************************** EntrezSeqGet **************************************/

EntrezSeqGetPtr CDECL
EntrezSeqGetNew(void)
{
	EntrezSeqGetPtr p;

	p = MemNew(sizeof(EntrezSeqGet));
	if (p != NULL)
	{
		p->ids = NULL;
	}
	return p;
}

EntrezSeqGetPtr CDECL
EntrezSeqGetFree(EntrezSeqGetPtr p)
{
	if (p == NULL)
		return NULL;
	EntrezIdsFree (p->ids);
	return MemFree(p);
}
		
EntrezSeqGetPtr CDECL
EntrezSeqGetAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	EntrezSeqGetPtr p;
	AsnTypePtr atp;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, ENTREZ_SEQGET);
	else
		atp = AsnLinkType(orig, ENTREZ_SEQGET); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = EntrezSeqGetNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	atp = AsnReadId(aip, amp, atp);
	if (atp == NULL)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)
		goto erret;
	p->retype = av.intvalue;

	p->mark_missing = FALSE;
	atp = AsnReadId(aip, amp, atp);
	if (atp == ENTREZ_SEQGET_mark_missing) {
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		p->mark_missing = av.boolvalue;
		atp = AsnReadId(aip, amp, atp);
	}

	if (atp == NULL)
		goto erret;
	p->ids = EntrezIdsAsnRead(aip, atp);

	atp = AsnReadId(aip, amp, atp);
	if (orig == NULL)
	{
		if (atp != ENTREZ_SEQGET)
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
	return p;

erret:
	p = EntrezSeqGetFree(p);
	goto ret;
}
		

Boolean CDECL
EntrezSeqGetAsnWrite (EntrezSeqGetPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
	Boolean retval = FALSE;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, ENTREZ_SEQGET); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;
	av.intvalue = p->retype;
	AsnWrite (aip, ENTREZ_SEQGET_retype, &av);
	av.boolvalue = p->mark_missing;
	AsnWrite (aip, ENTREZ_SEQGET_mark_missing, &av);
	EntrezIdsAsnWrite(p->ids, aip, ENTREZ_SEQGET_ids);

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}


/************************** SeqSummaryList ****************************************/

SeqSummaryListPtr CDECL
SeqSummaryListNew(void)
{
	SeqSummaryListPtr p;

	p = MemNew(sizeof(SeqSummaryList));
	if (p != NULL)
	{
		p->num = 0;
		p->data = NULL;
	}
	return p;
}

SeqSummaryListPtr CDECL
SeqSummaryListFree(SeqSummaryListPtr p)
{
	Int4 i;

	if (p == NULL)
		return NULL;
	if (p->data != NULL)
	{
		for (i = 0; i < p->num; i++)
			SeqSumFree(p->data[i]);
	}
	MemFree (p->data);
	return MemFree(p);
}
		
SeqSummaryListPtr CDECL
SeqSummaryListAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	SeqSummaryListPtr p;
	AsnTypePtr atp;
	Int4 num;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, SEQ_SUMMARY_LIST);
	else
		atp = AsnLinkType(orig, SEQ_SUMMARY_LIST); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = SeqSummaryListNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	atp = AsnReadId(aip, amp, atp); /* find the num */
	if (atp == NULL)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0) /* get the num */
		goto erret;
	p->num = av.intvalue;

	atp = AsnReadId(aip, amp, atp); /* find the data start-struct */
	if (atp == NULL)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0) /* get the data start-struct */
		goto erret;

	p->data = (DocSumPtr PNTR) MemNew(sizeof(DocSumPtr) * (size_t) p->num);
	for (num = 0; num < p->num; num++)
		p->data[num] = NULL;
	atp = AsnReadId(aip, amp, atp);

	for (num = 0; num < p->num && atp == SEQ_SUMMARY_LIST_data_E; num++)
	{
		if ((p->data[num] = SeqSumAsnRead(aip, atp)) == NULL)
			goto erret;
		atp = AsnReadId(aip, amp, atp);
	}

	/* check for count mis-match */
	if (num != p->num || atp != SEQ_SUMMARY_LIST_data)
		goto erret;

	if (AsnReadVal(aip, atp, NULL) <= 0) /* discard NULL */
		goto erret;

	atp = AsnReadId(aip, amp, atp);
	if (orig == NULL)
	{
		if (atp != SEQ_SUMMARY_LIST)
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
	return p;

erret:
	p = SeqSummaryListFree(p);
	goto ret;
}
		

Boolean CDECL
SeqSummaryListAsnWrite (SeqSummaryListPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Int4 i;
	AsnTypePtr atp;
	Boolean retval = FALSE;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, SEQ_SUMMARY_LIST); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;
	av.intvalue = p->num;
	AsnWrite (aip, SEQ_SUMMARY_LIST_num, &av);

	AsnStartStruct (aip, SEQ_SUMMARY_LIST_data);
	for (i = 0; i < p->num; i++)
	{
		SeqSumAsnWrite(p->data[i], aip, SEQ_SUMMARY_LIST_data_E);
	}
	AsnEndStruct (aip, SEQ_SUMMARY_LIST_data);

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}

/************************** SeqEntryList **************************************/

SeqEntryListPtr CDECL
SeqEntryListNew(void)
{
	SeqEntryListPtr p;

	p = MemNew(sizeof(*p));
	if (p != NULL)
	{
		p->num = 0;
		p->data = NULL;
		p->marked_missing = NULL;
	}
	return p;
}

SeqEntryListPtr CDECL
SeqEntryListFree(SeqEntryListPtr p)
{
	Int4 i;

	if (p == NULL)
		return NULL;
	if (p->data != NULL)
	{
		for (i = 0; i < p->num; i++)
			SeqEntryFree(p->data[i]);
	}
	MemFree (p->data);
	EntrezIdsFree (p->marked_missing);
	return MemFree(p);
}
		
SeqEntryListPtr CDECL
SeqEntryListAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	SeqEntryListPtr p;
	AsnTypePtr atp;
	Int4 num;

	if (!NetEntAsnLoad())
		return NULL;
	
	if (aip == NULL)
		return NULL;
	
	if (orig == NULL)
		atp = AsnReadId(aip, amp, ENTREZ_SEQ_ENTRY_LIST);
	else
		atp = AsnLinkType(orig, ENTREZ_SEQ_ENTRY_LIST); /* link in local tree */

	if (atp == NULL)
		return NULL;

	p = SeqEntryListNew();
	if (p == NULL)
		goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) /* read the start struct */
		goto erret;

	atp = AsnReadId(aip, amp, atp); /* find the num */
	if (atp == NULL)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0) /* get the num */
		goto erret;
	p->num = av.intvalue;

	atp = AsnReadId(aip, amp, atp); /* find the data start-struct */
	if (atp == NULL)
		goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0) /* get the data start-struct */
		goto erret;

	p->data = (SeqEntryPtr PNTR) MemNew(sizeof(SeqEntryPtr) * p->num);
	for (num = 0; num < p->num; num++)
		p->data[num] = NULL;
	atp = AsnReadId(aip, amp, atp);

	for (num = 0; num < p->num && atp == ENTREZ_SEQ_ENTRY_LIST_data_E; num++)
	{
		if ((p->data[num] = SeqEntryAsnRead(aip, atp)) == NULL)
			goto erret;
		atp = AsnReadId(aip, amp, atp);
	}

	/* check for count mis-match */
	if (num != p->num)
		goto erret;
	
	if (AsnReadVal(aip, atp, NULL) <= 0) /* discard NULL */
		goto erret;

	atp = AsnReadId(aip, amp, atp);
	if (atp == SEQ_ENTRY_LIST_marked_missing)
	{
		p->marked_missing = EntrezIdsAsnRead(aip, atp);
		atp = AsnReadId(aip, amp, atp);
	}

	if (orig == NULL)
	{
		if (atp != ENTREZ_SEQ_ENTRY_LIST)
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
	return p;

erret:
	p = SeqEntryListFree(p);
	goto ret;
}
		

Boolean CDECL
SeqEntryListAsnWrite (SeqEntryListPtr p, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	Int4 i;
	AsnTypePtr atp;
	Boolean retval = FALSE;

	if (! NetEntAsnLoad() )
		return FALSE;

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, ENTREZ_SEQ_ENTRY_LIST); /* link local tree */

	if (atp == NULL)
		return FALSE;

	if (p == NULL)
	{
		AsnNullValueMsg(aip, atp);
		goto erret;
	}

	if (! AsnStartStruct(aip, atp))
		goto erret;
	av.intvalue = p->num;
	AsnWrite (aip, ENTREZ_SEQ_ENTRY_LIST_num, &av);

	AsnStartStruct (aip, ENTREZ_SEQ_ENTRY_LIST_data);
	for (i = 0; i < p->num; i++)
	{
		SeqEntryAsnWrite(p->data[i], aip, ENTREZ_SEQ_ENTRY_LIST_data_E);
	}
	AsnEndStruct (aip, ENTREZ_SEQ_ENTRY_LIST_data);

	if (p->marked_missing != NULL) /* optional */
	{
		EntrezIdsAsnWrite(p->marked_missing, aip, SEQ_ENTRY_LIST_marked_missing);
	}

	if (! AsnEndStruct(aip, atp))
		goto erret;

	retval = TRUE;

erret:
	AsnUnlinkType(orig); /* unlink local tree */
	return retval;
}
