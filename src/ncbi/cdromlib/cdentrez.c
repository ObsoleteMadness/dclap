/*   cdentrez.c
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
* File Name:  cdentrez.c
*
* Author:  Ostell, Kans
*
* Version Creation Date:   10/15/91
*
* $Revision: 2.33 $
*
* File Description: 
*   	entrez index access library for Entrez CDROM
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/

#include <accentr.h>
#include <cdentrez.h>
#include <objall.h>

typedef struct posting {
  ByteStorePtr  uids;
  DocUidPtr     buffer;
  Int2          bufsize;
  Int2          index;
  Int2          count;
} Posting, PNTR PostingPtr;

#define SCAN_MAX 200

typedef struct scanData {
  Int4          specialCount;
  Int4          totalCount;
  Int4          offset;
  ByteStorePtr  specialPtr;
  ByteStorePtr  remainderPtr;
} ScanData, PNTR ScanPtr;

static Int2          db;
static Int2          fld;

static DocUidPtr     uidPtr;

static Int2          searchTermLen;

static ByteStorePtr  specialPost;
static ByteStorePtr  remainPost;

static Char          selection [256];
static Char          wildcard [64];

static ScanPtr       scanPtr;
static Int2          scanCount;
static Boolean       scanOk;
static CdTermProc    userScanProc;

static CdTermPtr     eset;

static void NEAR NextNode PROTO((void));
static ByteStorePtr NEAR Factor PROTO((void));
static ByteStorePtr NEAR Term PROTO((void));
static ByteStorePtr NEAR Diff PROTO((void));
static ByteStorePtr NEAR Expression PROTO((void));
static CdTermPtr NEAR FindTermNode PROTO((CharPtr term, DocType type, DocField field));

static PostingPtr NEAR NewPost PROTO((ByteStorePtr lst, Int4 defsize));
static PostingPtr NEAR FreePost PROTO((PostingPtr pst));
static Int4 NEAR PostLength PROTO((PostingPtr pst));
static void NEAR RewindPost PROTO((PostingPtr pst));
static DocUid NEAR ReadItem PROTO((PostingPtr pst));
static void NEAR WriteItem PROTO((PostingPtr pst, DocUid value));
static void NEAR FlushItems PROTO((PostingPtr pst));
static void NEAR SavePostingList PROTO((FILE *f, ByteStorePtr bsp));

static Boolean NEAR CdEntrezMergeTerm PROTO((DocType type, DocField field, CharPtr term, Int4Ptr spcl, Int4Ptr totl, CdTermProc userProc));
static void NEAR SingleSpaces PROTO((CharPtr str));
static void NEAR TermTruncate PROTO((CharPtr str));
static void NEAR QuickSortSmall PROTO((DocUidPtr uids, Int2 l, Int2 r));
static Int2 NEAR CompressSmall PROTO((DocUidPtr uids, Int2 count));
static Int2 NEAR UniqueSmall PROTO((DocUidPtr uids, Int2 count));
static ByteStorePtr NEAR MergeSmallLists PROTO((ByteStorePtr bsp, ByteStorePtr small));
static Boolean NEAR MergeSeveralLists PROTO((Int2 i, Int2 count));
static Boolean NEAR MergeSeveralOrderedLists PROTO((Int2 i, Int2 count));
static Boolean NEAR MergeUnorderedLists PROTO((Int2 i, Int2 count));
static Boolean NEAR ProcessScanResults PROTO((void));
static Boolean  WildCardProc PROTO((CdTermPtr trmp));
static Boolean  ScanOnlyProc PROTO((CdTermPtr trmp));
static Boolean  ScanAndFreeProc PROTO((CdTermPtr trmp));

/**** Moved from cdentrez.h ********************/

static CdTermPtr NEAR CdEntrezCreateTerm PROTO((CharPtr term, DocType type, DocField field, ByteStorePtr special, ByteStorePtr remainder));
static ByteStorePtr NEAR LoadPostingList PROTO((FILE *f, Int4 special, Int4 total));
static ByteStorePtr NEAR FreePostingList PROTO((ByteStorePtr lst));
static ByteStorePtr NEAR MergePostingLists PROTO((ByteStorePtr lst1, ByteStorePtr lst2));
static ByteStorePtr NEAR IntersectPostingLists PROTO((ByteStorePtr lst1, ByteStorePtr lst2));
static ByteStorePtr NEAR DifferencePostingLists PROTO((ByteStorePtr lst1, ByteStorePtr lst2));

static ValNodePtr currNode;
static Uint1 currChoice;

/************************* moved from old cdml.c ****************************/
static AsnTypePtr  MEDLINE_ENTRY = NULL;
static AsnTypePtr  MEDLINE_ENTRY_cit = NULL;
static AsnTypePtr  MEDLINE_ENTRY_abstract = NULL;
static AsnTypePtr  TITLE_E_trans = NULL;
static AsnTypePtr  AUTH_LIST_names_ml_E = NULL;
static AsnTypePtr  AUTH_LIST_names_str_E = NULL;
static AsnTypePtr  DATE_STD_year = NULL;
static AsnTypePtr  DATE_str = NULL;
static AsnTypePtr  TITLE_E_name = NULL;
static AsnTypePtr  MEDLINE_ENTRY_mesh = NULL;
static AsnTypePtr  MEDLINE_ENTRY_substance = NULL;
static AsnTypePtr  MEDLINE_ENTRY_xref = NULL;
static AsnTypePtr  MEDLINE_ENTRY_idnum = NULL;
static AsnTypePtr  MEDLINE_ENTRY_gene = NULL;

static DocSumPtr NEAR MedSumAsnRead PROTO((AsnIoPtr aip, DocUid uid));
static void NEAR StripAuthor PROTO((CharPtr author));
static void NEAR FindAsnType PROTO((AsnTypePtr PNTR atp, AsnModulePtr amp, CharPtr str));

static DocSumPtr NEAR CdEntMlSumGet PROTO((Int4 uid));

/************************* moved from old cdseq.c ****************************/
static AsnTypePtr  SEQ_ENTRY = NULL;
static AsnTypePtr  SEQ_ENTRY_seq = NULL;
static AsnTypePtr  SEQ_ENTRY_set = NULL;
static AsnTypePtr  TEXTSEQ_ID_name = NULL;
static AsnTypePtr  TEXTSEQ_ID_accession = NULL;
static AsnTypePtr  SEQ_DESCR_E_title = NULL;
static AsnTypePtr  GIIMPORT_ID_id = NULL;
static AsnTypePtr  BIOSEQ_inst = NULL;
static AsnTypePtr  SEQ_INST_mol = NULL;
static AsnTypePtr  SEQ_INST_repr = NULL;
static AsnTypePtr  SEQ_ID_gibbsq = NULL;
static AsnTypePtr  SEQ_ID_gibbmt = NULL;
static AsnTypePtr  SEQ_ID_genbank = NULL;
static AsnTypePtr  SEQ_ID_embl = NULL;
static AsnTypePtr  SEQ_ID_ddbj = NULL;
static AsnTypePtr  SEQ_ID_pir = NULL;
static AsnTypePtr  SEQ_ID_swissprot = NULL;
static AsnTypePtr  PDB_BLOCK_compound_E = NULL;
static AsnTypePtr  PDB_SEQ_ID_MOL = NULL;
static AsnTypePtr  BIOSEQ_id = NULL;
static AsnTypePtr  CIT_PAT_title = NULL;

static DocSumPtr NEAR SeqSumAsnRead PROTO((AsnIoPtr aip, DocUid uid));
static DocSumPtr NEAR CdEntSeqSumGet PROTO((Int4 uid, DocType type));

/*****************************************************************************
*
*   CdEntrezInit ()
*       Creates linked list of CdTerm nodes, creates temporary file for
*       postings lists, saves file name in first node.  When creating new
*       nodes, posting file is appended to temporary file, node offset then
*       points to temporary file location of posting information.
*
*****************************************************************************/

Boolean  CdEntrezInit (Boolean no_warnings)

{
  FILE      *fp;
  Char      str [PATH_MAX];

	if (! CdInit())
		return FALSE;
	eset = MemNew (sizeof (CdTerm));
	if (eset == NULL)
		return FALSE;
	eset->type = 255;   /* set to not used */
    TmpNam (str);
    eset->term = StringSave (str);
#ifdef WIN_MAC
    FileCreate (str, "????", "NCBI");
#endif
    fp = FileOpen (str, "wb");
	if (fp == NULL) {
		ErrPost(CTX_NCBICD, ERR_CD_FILEOPEN, "Unable to open temporary file %s", str);
		return FALSE;
	}
    FileClose (fp);
	return TRUE;
}

/*****************************************************************************
*
*   CdEntrezFini ()
*       Frees linked list of CdTerm nodes and removes temporary posting file.
*
*****************************************************************************/

void  CdEntrezFini (void)

{
  CdTermPtr nxt;
  Char      temp [PATH_MAX];

  if (eset != NULL) {
    if (eset->term != NULL) {
      StringCpy (temp, eset->term);
      FileRemove (temp);
    }
    while (eset != NULL) {
      nxt = eset->next;
      CdTermFree (eset);
      eset = nxt;
    }
  }
  eset = NULL;
	CdFini();
}

/*****************************************************************************
*
*   CdEntGetMaxLinks()
*   	returns max links in link set allowed by system
*
*****************************************************************************/
Int4 CdEntGetMaxLinks (void)

{
	return (Int4)(INT2_MAX / sizeof(DocUid));
}

/*****************************************************************************
*
*   CdEntrezCreateNamedUidList(term, type, field, num, uids)
*       Creates a term node in the entrez set structure if one does not
*       yet exist, and loads the posting file from the uid parameter.
*
*****************************************************************************/
void  CdEntrezCreateNamedUidList (CharPtr term, DocType type, DocField field, Int4 num, DocUidPtr uids)

{
  Int2          count;
  ByteStorePtr  post;
  Char          str [256];

  if (term != NULL && uids != NULL && num > 0 && num <= 32767) {
    StringNCpy (str, term, sizeof (str) - 1);
    post = BSNew (0);
    if (post != NULL) {
      count = (Int2) num;
      QuickSortSmall (uids, 0, (Int2) (count - 1));
      count = CompressSmall (uids, count);
      count = UniqueSmall (uids, count);
      BSWrite (post, uids, (Int4) (count * sizeof (DocUid)));
      CdEntrezCreateTerm (str, type, field, NULL, post);
      BSFree (post);
    }
  }
}

/*****************************************************************************
*
*   CdEntTLNew (type)
*       Creates linked list of asn nodes for constructing boolean query on
*       terms.  First node points to the EntrezSetNew-created structure that
*       maps terms to posting lists.  Remaining nodes contain symbols for AND,
*       OR, LEFT PARENTHESIS, RIGHT PARENTHESIS, or a SPECIAL or TOTAL term
*       specification.  The term specification nodes point to a CdTerm node
*       within the entrez set structure.
*
*****************************************************************************/

ValNodePtr  CdEntTLNew (DocType type)

{
  ValNodePtr anp;

  anp = NULL;
  if (eset != NULL) {
    anp = ValNodeNew (NULL);
    if (anp != NULL) {
      anp->choice = NULLSYM;
      anp->data.ptrvalue = (Pointer) eset;
	  eset->type = type;
    }
  }
  return anp;
}

/*****************************************************************************
*
*   CdEntTLAddTerm (elst, term, type, field, special)
*       Adds a term node to a boolean algebraic term query.
*
*****************************************************************************/

ValNodePtr  CdEntTLAddTerm (ValNodePtr elst, CharPtr term, DocType type, DocField field, Boolean special)

{
  ValNodePtr anp;
  CdTermPtr  trmp;

  anp = NULL;
  if (eset != NULL && elst != NULL) {
	if (type != eset->type)   /* mixed databases */
		return NULL;
    anp = ValNodeNew (elst);
    if (anp != NULL) {
      if (special) {
        anp->choice = SPECIALTERM;
      } else {
        anp->choice = TOTALTERM;
      }
      trmp = FindTermNode (term, type, field);
      anp->data.ptrvalue = (Pointer) trmp;
    }
  }
  return anp;
}

/*****************************************************************************
*
*   CdEntTLFree (elst)
*       Frees a boolean algebraic term query list.
*
*****************************************************************************/

ValNodePtr  CdEntTLFree (ValNodePtr elst)

{
  if (elst != NULL) {
    ValNodeFree (elst);
	eset->type = 255;   /* set to nothing */
  }
  return NULL;
}

/*****************************************************************************
*
*   CdEntTLEvalX (elst)
*       Evaluates a boolean algebraic term query list, returning a pointer to
*       a ByteStore containing the resultant unique identifiers.  The number
*       of UIDs is calculated as BSLen (bsp) / sizeof (DocUid).
*
*****************************************************************************/

ByteStorePtr  CdEntTLEvalX (ValNodePtr elst)

{
  ByteStorePtr bsp;

  bsp = NULL;
  if (eset != NULL && elst != NULL) {
    currNode = elst;
    currChoice = NULLSYM;
    NextNode ();
    if (eset->term != NULL && currNode != NULL) {
      bsp = Expression ();
    }
  }
  return bsp;
}

/*****************************************************************************
*
*   CdEntTLEval (elst)
*       Evaluates a boolean algebraic term query list, returning a pointer to
*       a LinkSet containing the resultant unique identifiers.
*
*****************************************************************************/

LinkSetPtr  CdEntTLEval (ValNodePtr elst)

{
  ByteStorePtr bsp;
  LinkSetPtr lsp = NULL;
  Int4 numlinks;

  bsp = CdEntTLEvalX (elst);
  if (bsp != NULL)
	{
		numlinks = BSLen(bsp) / sizeof(DocUid);
		lsp = LinkSetNew();
		lsp->num = numlinks;
		if (numlinks <= EntrezGetUserMaxLinks())
		{
			lsp->uids = MemNew((size_t)(numlinks * sizeof(DocUid)));
			BSSeek (bsp, 0L, 0);
			BSRead(bsp, lsp->uids, (numlinks * sizeof(DocUid)));
		}
		BSFree(bsp);
	}
  return lsp;
}

/*****************************************************************************
*
*   DocSumPtr CdDocSum(type, uid)
*
*****************************************************************************/
DocSumPtr  CdDocSum (DocType type, DocUid uid)

{
	if (type == TYP_ML)
		return CdEntMlSumGet(uid);
	else
		return CdEntSeqSumGet(uid, type);
}

/*****************************************************************************
*
*   CdLinkUidList(type, link_to_type, numuid, uids)
*   	returns count of input uids processed
*       returns -1 on error
*       if neighbors (type == link_to_type)
*   		sums weights for same uids
*   	if (more than EntrezUserMaxLinks() uids, frees uids and weights,
*           but leaves num set)
*
*****************************************************************************/
Int2  CdLinkUidList (LinkSetPtr PNTR result, DocType type, DocType link_to_type, Int2 numuid, Int4Ptr uids, Boolean mark_missing)      

{
	return CdLinkUidGet(result, type, link_to_type, numuid, uids,
		mark_missing, EntrezGetUserMaxLinks());
}

/*****************************************************************************
*
*   CdUidLinks()
*   	retrieves links to other uids
*
*****************************************************************************/
LinkSetPtr  CdUidLinks (DocType type, DocUid uid, DocType link_to_type)

{
	LinkSetPtr lsp = NULL;

	CdLinkUidGet(&lsp, type, link_to_type, 1, &uid, FALSE, EntrezGetUserMaxLinks());
	return lsp;
}

Boolean  TermListPageScanProc PROTO((CdTermPtr trmptr));
Boolean  TermListTermScanProc PROTO((CdTermPtr trmptr));
static TermListProc trmproc;
static Int2 trmcount;
static Int2 trmmax;
static Boolean trmfound;
static Char trmfirst [80];
static Int2 the_first_page;

/*****************************************************************************
*
*   CdTermListByPage (type, field, page, numpage, proc)
*   	Gets terms starting at page, for numpage, by calling proc
*   	returns number of complete pages read
*
*****************************************************************************/
Int2  CdTermListByPage (DocType type, DocField field, Int2 page, Int2 numpage, TermListProc proc)

{
	trmproc = proc;
	if (trmproc != NULL) {
		return CdTermScan(type, field, page, numpage, TermListPageScanProc);
	} else {
		return 0;
	}
}

/*****************************************************************************
*
*   CdTermListByTerm (type, field, term, numterms, proc, first_page)
*   	Gets Terms starting with at term
*   	returns number of complete pages read
*   	sets first_page to first page read
*
*****************************************************************************/
Int2  CdTermListByTerm (DocType type, DocField field, CharPtr term, Int2 numterms, TermListProc proc, Int2Ptr first_page)

{
	Int2  first;
	Int2  rsult;

	rsult = 0;
	first = CdTrmLookup(type, field, term);
	the_first_page = first;
	trmproc = proc;
	trmcount = 0;
	if (numterms > 0) {
		trmmax = numterms;
	} else {
		trmmax = INT2_MAX;
	}
	trmfound = FALSE;
	StringNCpy (trmfirst, term, sizeof (trmfirst) - 1);
	if (trmproc != NULL) {
		rsult = CdTermScan(type, field, first, 0, TermListTermScanProc);
	}
	if (first_page != NULL) {
	  *first_page = the_first_page;
	}
	return rsult;
}

/*****************************************************************************
*
*   TermListPageScanProc(trmptr)
*   	Callback for CdTermListByPage
*
*****************************************************************************/
Boolean  TermListPageScanProc(CdTermPtr trmptr)

{
	Boolean ret;

	ret = trmproc(trmptr->term, trmptr->special_count, trmptr->total_count);
	MemFree(trmptr);
	return ret;
}

/*****************************************************************************
*
*   TermListTermScanProc(trmptr)
*   	Callback for CdTermListByTerm
*
*****************************************************************************/
Boolean  TermListTermScanProc(CdTermPtr trmptr)

{
	Boolean ret;

	ret = TRUE;
	if (! trmfound) {
		if (MeshStringICmp (trmptr->term, trmfirst) >= 0) {
			trmfound = TRUE;
			the_first_page = trmptr->page;
		}
	}
	if (trmfound) {
		ret = trmproc(trmptr->term, trmptr->special_count, trmptr->total_count);
		trmcount++;
	} else {
		MemFree (trmptr->term);
	}
	MemFree(trmptr);
	return (ret && trmcount < trmmax);
}

/*****************************************************************************
*
*   CdEntrezFindTerm(type, field, term, spec, total)
*   	returns count of special and total for a term
*   	if term ends with  "...", does a truncated merge of the term
*   	if term contains '*' or '?', does a wild card merge
*
*****************************************************************************/
Boolean  CdEntrezFindTerm (DocType type, DocField field, CharPtr term, Int4Ptr spcl, Int4Ptr totl)

{
	CharPtr tmp;
	CdTermPtr ctp;

	tmp = term;
	while (*tmp != '\0')
		tmp++;
	tmp -= 3;
	if ((*tmp == '.') && (*(tmp+1) == '.') && (*(tmp+2) == '.')) {
		return CdEntrezMergeTerm (type, field, term, spcl, totl, NULL);
	} else if (StringChr (term, '*') != NULL || StringChr (term, '?') != NULL) {
		return CdEntrezMergeTerm (type, field, term, spcl, totl, WildCardProc);
	} else {
		ctp = CdTrmFind(type, field, term);
		if (ctp == NULL)
			return FALSE;
		*spcl = ctp->special_count;
		*totl = ctp->total_count;
		CdTermFree(ctp);
		return TRUE;
	}
}
/*****************************************************************************
*
*   Below are static functions local to this module
*   ===============================================
*
*****************************************************************************/

/*****************************************************************************
*
*   Functions to manipulate Boolean lists
*
*****************************************************************************/

/*****************************************************************************
*
*   NextNode ()
*       Advances to the next node in a term query list.
*
*****************************************************************************/

static void NEAR NextNode (void)

{
  if (currNode != NULL) {
    currNode = currNode->next;
    if (currNode != NULL) {
      currChoice = currNode->choice;
    } else {
      currChoice = NULLSYM;
    }
  } else {
    currChoice = NULLSYM;
  }
}

/*****************************************************************************
*
*   Factor ()
*       Processes individual term nodes or parenthetical expressions in a
*       term query list.
*
*****************************************************************************/

static ByteStorePtr NEAR Factor (void)

{
  ByteStorePtr bsp;
  FILE         *fp;
  CdTermPtr    trmp;

  bsp = NULL;
  if (currChoice == LPAREN) {
    NextNode ();
    bsp = Expression ();
    if (currChoice != RPAREN) {
      Message (MSG_OK, "Expected right parenthesis");
    } else {
      NextNode ();
    }
  } else if (currChoice == SPECIALTERM || currChoice == TOTALTERM) {
    if (currNode != NULL) {
      trmp = currNode->data.ptrvalue;
      if (trmp != NULL) {
        fp = FileOpen (eset->term, "rb");
        if (fp != NULL) {
          fseek (fp, trmp->offset, SEEK_SET);
          if (currChoice == SPECIALTERM) {
            bsp = LoadPostingList (fp, trmp->special_count, trmp->special_count);
          } else if (currChoice == TOTALTERM) {
            bsp = LoadPostingList (fp, trmp->special_count, trmp->total_count);
          }
          FileClose (fp);
        }
      }
    }
    NextNode ();
  } else {
    NextNode ();
  }
  return bsp;
}

/*****************************************************************************
*
*   Term ()
*       Processes strings of ANDed term nodes in a term query list.
*
*****************************************************************************/

static ByteStorePtr NEAR Term (void)

{
  ByteStorePtr bsp;
  ByteStorePtr fct;

  bsp = Factor ();
  while (currChoice == ANDSYMBL) {
    NextNode ();
    fct = Factor ();
    bsp = IntersectPostingLists (bsp, fct);
  }
  return bsp;
}

/*****************************************************************************
*
*   Diff ()
*       Processes strings of ORed term nodes in a term query list.
*
*****************************************************************************/

static ByteStorePtr NEAR Diff (void)

{
  ByteStorePtr bsp;
  ByteStorePtr trm;

  bsp = Term ();
  while (currChoice == ORSYMBL) {
    NextNode ();
    trm = Term ();
    bsp = MergePostingLists (bsp, trm);
  }
  return bsp;
}


/*****************************************************************************
*
*   Expression ()
*       Processes strings of BUTNOTed term nodes in a term query list.
*
*****************************************************************************/

static ByteStorePtr NEAR Expression (void)

{
  ByteStorePtr bsp;
  ByteStorePtr trm;

  bsp = Diff ();
  while (currChoice == BUTNOTSYMBL) {
    NextNode ();
    trm = Diff ();
    bsp = DifferencePostingLists (bsp, trm);
  }
  return bsp;
}


/*****************************************************************************
*
*   Low level functions to manipulate postings lists.
*
*****************************************************************************/

static PostingPtr NEAR NewPost (ByteStorePtr lst, Int4 defsize)

{
  PostingPtr  pst;

  pst = NULL;
  if (lst != NULL) {
    pst = MemNew (sizeof (Posting));
    if (pst != NULL) {
      pst->uids = lst;
      pst->buffer = NULL;
      if (defsize == 0) {
        pst->bufsize = (Int2) MIN (16384L, BSLen (lst));
      } else {
        pst->bufsize = (Int2) MIN (16384L, defsize);
      }
      pst->count = 0;
      pst->index = 0;
    }
  }
  return pst;
}

static PostingPtr NEAR FreePost (PostingPtr pst)

{
  if (pst != NULL) {
    if (pst->uids != NULL) {
      BSFree (pst->uids);
    }
    if (pst->buffer != NULL) {
      MemFree (pst->buffer);
    }
    MemFree (pst);
  }
  return NULL;
}

static Int4 NEAR PostLength (PostingPtr pst)

{
  Int4  k;

  k = 0;
  if (pst != NULL) {
    k = (Int4) (BSLen (pst->uids) / (Int4) sizeof (DocUid));
  }
  return k;
}

static void NEAR RewindPost (PostingPtr pst)

{
  if (pst != NULL) {
    if (pst->uids != NULL) {
      BSSeek (pst->uids, 0L, 0);
    }
    pst->count = 0;
    pst->index = 0;
  }
}

static DocUid NEAR ReadItem (PostingPtr pst)

{
  DocUid  rsult;

  rsult = INT4_MAX;
  if (pst != NULL && pst->uids != NULL) {
    if (pst->buffer == NULL) {
      pst->buffer = MemNew ((size_t) pst->bufsize);
      pst->count = 0;
      pst->index = 0;
    }
    if (pst->count <= 0) {
      pst->count = (Int2) BSRead (pst->uids, pst->buffer, pst->bufsize);
      pst->index = 0;
    }
    if (pst->count > 0) {
      rsult = pst->buffer [pst->index];
      (pst->index)++;
      (pst->count) -= sizeof (DocUid);
    }
  }
  return rsult;
}

static void NEAR WriteItem (PostingPtr pst, DocUid value)

{
  if (pst != NULL && pst->uids != NULL) {
    if (pst->buffer == NULL) {
      pst->buffer = MemNew ((size_t) pst->bufsize);
      pst->count = 0;
      pst->index = 0;
    }
    pst->buffer [pst->index] = value;
    (pst->index)++;
    (pst->count) += sizeof (DocUid);
    if (pst->count >= pst->bufsize) {
      BSWrite (pst->uids, pst->buffer, pst->count);
      pst->count = 0;
      pst->index = 0;
    }
  }
}

static void NEAR FlushItems (PostingPtr pst)

{
  if (pst != NULL && pst->uids != NULL && pst->buffer != NULL) {
    BSWrite (pst->uids, pst->buffer, pst->count);
    if (pst->buffer != NULL) {
      pst->buffer = MemFree (pst->buffer);
    }
    pst->count = 0;
    pst->index = 0;
  }
}

static ByteStorePtr NEAR MergePostingLists (ByteStorePtr lst1, ByteStorePtr lst2)

{
  PostingPtr    buf1;
  PostingPtr    buf2;
  PostingPtr    buf3;
  short         erract;
  Int4          k;
  Int4          k1;
  Int4          k2;
  DocUid        pstar;
  DocUid        qstar;
  ByteStorePtr  rsult;

  ProgMon ("MergePostingLists");
  rsult = NULL;
  if (lst1 != NULL && lst2 != NULL) {
    ErrGetOpts (&erract, NULL);
    if (erract > ERR_IGNORE) {
      ErrSetOpts (ERR_CONTINUE, 0);
    }
    buf1 = NewPost (lst1, 0);
    buf2 = NewPost (lst2, 0);
    k1 = PostLength (buf1);
    k2 = PostLength (buf2);
    k = k1 + k2;
    rsult = BSNew (k * sizeof (DocUid));
    buf3 = NewPost (rsult, k * (Int4) sizeof (DocUid));
    if (rsult != NULL && buf1 != NULL && buf2 != NULL && buf3 != NULL) {
      RewindPost (buf1);
      RewindPost (buf2);
      pstar = ReadItem (buf1);
      qstar = ReadItem (buf2);
      while (k > 0) {
        if (pstar < qstar) {
          WriteItem (buf3, pstar);
          k--;
          pstar = ReadItem (buf1);
        } else if (qstar < pstar) {
          WriteItem (buf3, qstar);
          k--;
          qstar = ReadItem (buf2);
        } else {
          WriteItem (buf3, pstar);
          k -= 2;
          pstar = ReadItem (buf1);
          qstar = ReadItem (buf2);
        }
      }
      FlushItems (buf3);
    } else {
      Message (MSG_ERROR, "List is too large to merge");
    }
    if (buf1 != NULL) {
      FreePost (buf1);
    }
    if (buf2 != NULL) {
      FreePost (buf2);
    }
    if (buf3 != NULL) {
      buf3->uids = NULL;
      FreePost (buf3);
    }
    ErrSetOpts (erract, 0);
  } else if (lst1 != NULL) {
    rsult = lst1;
  } else if (lst2 != NULL) {
    rsult = lst2;
  }
  return rsult;
}

static ByteStorePtr NEAR IntersectPostingLists (ByteStorePtr lst1, ByteStorePtr lst2)

{
  PostingPtr    buf1;
  PostingPtr    buf2;
  PostingPtr    buf3;
  short         erract;
  Int4          k;
  Int4          k1;
  Int4          k2;
  DocUid        pstar;
  DocUid        qstar;
  ByteStorePtr  rsult;

  ProgMon ("UnionPostingLists");
  rsult = NULL;
  if (lst1 != NULL && lst2 != NULL) {
    ErrGetOpts (&erract, NULL);
    if (erract > ERR_IGNORE) {
      ErrSetOpts (ERR_CONTINUE, 0);
    }
    buf1 = NewPost (lst1, 0);
    buf2 = NewPost (lst2, 0);
    k1 = PostLength (buf1);
    k2 = PostLength (buf2);
    k = MIN (k1, k2);
    rsult = BSNew (k * sizeof (DocUid));
    buf3 = NewPost (rsult, k * (Int4) sizeof (DocUid));
    if (rsult != NULL && buf1 != NULL && buf2 != NULL && buf3 != NULL) {
      RewindPost (buf1);
      RewindPost (buf2);
      pstar = ReadItem (buf1);
      qstar = ReadItem (buf2);
      while (k1 > 0 && k2 > 0) {
        if (pstar < qstar) {
          k1--;
          pstar = ReadItem (buf1);
        } else if (qstar < pstar) {
          k2--;
          qstar = ReadItem (buf2);
        } else {
          WriteItem (buf3, pstar);
          k1--;
          k2--;
          pstar = ReadItem (buf1);
          qstar = ReadItem (buf2);
        }
      }
      FlushItems (buf3);
    } else {
      Message (MSG_ERROR, "List is too large to intersect");
    }
    if (buf1 != NULL) {
      FreePost (buf1);
    }
    if (buf2 != NULL) {
      FreePost (buf2);
    }
    if (buf3 != NULL) {
      buf3->uids = NULL;
      FreePost (buf3);
    }
    ErrSetOpts (erract, 0);
  } else if (lst1 != NULL) {
    rsult = lst1;
  } else if (lst2 != NULL) {
    rsult = lst2;
  }
  return rsult;
}

static ByteStorePtr NEAR DifferencePostingLists (ByteStorePtr lst1, ByteStorePtr lst2)

{
  PostingPtr    buf1;
  PostingPtr    buf2;
  PostingPtr    buf3;
  short         erract;
  Int4          k;
  Int4          k1;
  Int4          k2;
  DocUid        pstar;
  DocUid        qstar;
  ByteStorePtr  rsult;

  ProgMon ("DiffPostingLists");
  rsult = NULL;
  if (lst1 != NULL && lst2 != NULL) {
    ErrGetOpts (&erract, NULL);
    if (erract > ERR_IGNORE) {
      ErrSetOpts (ERR_CONTINUE, 0);
    }
    buf1 = NewPost (lst1, 0);
    buf2 = NewPost (lst2, 0);
    k1 = PostLength (buf1);
    k2 = PostLength (buf2);
    k = k1 + k2;
    rsult = BSNew (k * sizeof (DocUid));
    buf3 = NewPost (rsult, k * (Int4) sizeof (DocUid));
    if (rsult != NULL && buf1 != NULL && buf2 != NULL && buf3 != NULL) {
      RewindPost (buf1);
      RewindPost (buf2);
      pstar = ReadItem (buf1);
      qstar = ReadItem (buf2);
      while (k > 0) {
        if (pstar < qstar) {
          WriteItem (buf3, pstar);
          k--;
          pstar = ReadItem (buf1);
        } else if (qstar < pstar) {
          k--;
          qstar = ReadItem (buf2);
        } else {
          k -= 2;
          pstar = ReadItem (buf1);
          qstar = ReadItem (buf2);
        }
      }
      FlushItems (buf3);
    } else {
      Message (MSG_ERROR, "List is too large to difference");
    }
    if (buf1 != NULL) {
      FreePost (buf1);
    }
    if (buf2 != NULL) {
      buf2->uids = NULL;
      FreePost (buf2);
    }
    if (buf3 != NULL) {
      buf3->uids = NULL;
      FreePost (buf3);
    }
    ErrSetOpts (erract, 0);
  } else if (lst1 != NULL) {
    rsult = lst1;
  }
  return rsult;
}

static ByteStorePtr NEAR FreePostingList (ByteStorePtr lst)

{
  if (lst != NULL) {
    BSFree (lst);
  }
  return NULL;
}

static ByteStorePtr NEAR LoadPostingList (FILE *f, Int4 special, Int4 total)

{
  VoidPtr       bufr;
  Int4          cnt;
  Int4          cntr;
  short         erract;
  Int4          k1;
  Int4          k2;
  ByteStorePtr  lst1;
  ByteStorePtr  lst2;
  ByteStorePtr  rsult;

  rsult = NULL;
  if (f != NULL && special >= 0 && total >= 0) {
    ErrGetOpts (&erract, NULL);
    if (erract > ERR_IGNORE) {
      ErrSetOpts (ERR_CONTINUE, 0);
    }
    bufr = MemNew (8192 * sizeof (DocUid));
    if (bufr != NULL) {
      k1 = special;
      k2 = total - special;
      lst1 = BSNew (k1 * sizeof (DocUid));
      if (lst1 != NULL) {
        cntr = k1;
        cnt = MIN (k1, 8192L);
        while (cnt > 0) {
          FileRead (bufr, sizeof (DocUid), (size_t) cnt, f);
          BSWrite (lst1, bufr, cnt * sizeof (DocUid));
          cntr -= cnt;
          cnt = MIN (cntr, 8192L);
        }
      } else {
        Message (MSG_ERROR, "List is too large to load");
      }
      lst2 = BSNew (k2 * sizeof (DocUid));
      if (lst2 != NULL) {
        cntr = k2;
        cnt = MIN (k2, 8192L);
        while (cnt > 0) {
          FileRead (bufr, sizeof (DocUid), (size_t) cnt, f);
          BSWrite (lst2, bufr, cnt * sizeof (DocUid));
          cntr -= cnt;
          cnt = MIN (cntr, 8192L);
        }
      } else {
        Message (MSG_ERROR, "List is too large to load");
      }
      rsult = MergePostingLists (lst1, lst2);
    }
    MemFree (bufr);
    ErrSetOpts (erract, 0);
  }
  return rsult;
}

/*****************************************************************************
*
*   CdEntrezCreateTerm (term, type, field, special, remainder)
*       Creates a CdTerm node in the entrez set structure if one does not yet
*       exist, and loads the posting file from two ByteStorePtr posting lists.
*
*****************************************************************************/

static void NEAR SavePostingList (FILE *f, ByteStorePtr bsp)

{
  VoidPtr  bufr;
  Int4     cnt;
  Int4     cntr;
  short    erract;

  if (f != NULL && bsp != NULL) {
    ErrGetOpts (&erract, NULL);
    if (erract > ERR_IGNORE) {
      ErrSetOpts (ERR_CONTINUE, 0);
    }
    bufr = MemNew (8192 * sizeof (DocUid));
    if (bufr != NULL) {
      cntr = (BSLen (bsp) / (Int4) sizeof (DocUid));
      cnt = MIN (cntr, 8192L);
      BSSeek (bsp, 0L, 0);
      while (cnt > 0) {
        BSRead (bsp, bufr, cnt * sizeof (DocUid));
        FileWrite (bufr, sizeof (DocUid), (size_t) cnt, f);
        cntr -= cnt;
        cnt = MIN (cntr, 8192L);
      }
    }
    MemFree (bufr);
    ErrSetOpts (erract, 0);
  }
}

static CdTermPtr NEAR CdEntrezCreateTerm (CharPtr term, DocType type, DocField field, ByteStorePtr special, ByteStorePtr remainder)

{
  FILE      *fp;
  Boolean   goOn;
  CdTermPtr last;
  Int4      remainderCount;
  Int4      specialCount;
  CdTermPtr trmp;

  trmp = NULL;
  if (eset != NULL && term != NULL) {
    trmp = eset->next;
    last = eset;
    goOn = TRUE;
    while (trmp != NULL && goOn) {
      if (trmp->type == type && trmp->field == field &&
          StringICmp (trmp->term, term) == 0) {
        goOn = FALSE;
      } else {
        last = trmp;
        trmp = trmp->next;
      }
    }
    if (goOn) {
      trmp = MemNew (sizeof (CdTerm));
      if (trmp != NULL) {
        specialCount = 0;
        remainderCount = 0;
        if (special != NULL) {
          specialCount = (BSLen (special) / (Int4) sizeof (DocUid));
        }
        if (remainder != NULL) {
          remainderCount = (BSLen (remainder) / (Int4) sizeof (DocUid));
        }
        trmp->type = type;
        trmp->field = field;
        trmp->term = StringSave (term);
        trmp->special_count = specialCount;
        trmp->total_count = specialCount + remainderCount;
        trmp->next = NULL;
        last->next = trmp;
        fp = FileOpen (eset->term, "ab");
        if (fp != NULL) {
          fseek (fp, 0, SEEK_END);
          trmp->offset = ftell (fp);
          SavePostingList (fp, special);
          SavePostingList (fp, remainder);
          FileClose (fp);
        } else {
          trmp->offset = 0;
        }
      }
    }
  }
  return trmp;
}

/*****************************************************************************
*
*   FindTermNode (term, type, field)
*       Returns a pointer to a CdTerm node in the entrez set structure,
*       creating the node and loading the posting file, if necessary.  The
*       value of the offset field becomes the offset into the temporary file.
*
*****************************************************************************/

static CdTermPtr NEAR FindTermNode (CharPtr term, DocType type, DocField field)

{
  FILE      *fp;
  Boolean   goOn;
  CdTermPtr last;
  Int4      offset;
  Int4      remain;
  Int4      special;
  CharPtr   tmp;
  Int4      total;
  CdTermPtr trmp;

  trmp = NULL;
  if (eset != NULL && term != NULL) {
    trmp = eset->next;
    last = eset;
    goOn = TRUE;
    while (trmp != NULL && goOn) {
      if (trmp->type == type && trmp->field == field &&
          StringICmp (trmp->term, term) == 0) {
        goOn = FALSE;
      } else {
        last = trmp;
        trmp = trmp->next;
      }
    }
    if (goOn) {
      tmp = term;
      while (*tmp != '\0')
        tmp++;
      tmp -= 3;
      if ((*tmp == '.') && (*(tmp+1) == '.') && (*(tmp+2) == '.')) {
        CdEntrezMergeTerm (type, field, term, NULL, NULL, NULL);
      } else if (StringChr (term, '*') != NULL || StringChr (term, '?') != NULL) {
        CdEntrezMergeTerm (type, field, term, NULL, NULL, WildCardProc);
      }
      trmp = eset->next;
      last = eset;
      goOn = TRUE;
      while (trmp != NULL && goOn) {
        if (trmp->type == type && trmp->field == field &&
            StringICmp (trmp->term, term) == 0) {
          goOn = FALSE;
        } else {
          last = trmp;
          trmp = trmp->next;
        }
      }
    }
    if (goOn) {
      trmp = CdTrmFind (type, field, term);
      if (trmp != NULL) {
        if (field != FLD_ORGN) {
          last->next = trmp;
          fp = FileOpen (eset->term, "rb");
          if (fp != NULL) {
            fseek (fp, 0, SEEK_END);
            offset = ftell (fp);
            FileClose (fp);
          } else {
            offset = 0;
          }
          CdTrmUidsFil (type, field, trmp->offset, trmp->total_count, eset->term, TRUE);
          trmp->offset = offset;
        } else {
          db = type;
          fld = field;
          uidPtr = MemNew ((size_t) 32000);
          if (uidPtr != NULL) {
            scanPtr = MemNew (SCAN_MAX * sizeof (ScanData));
            if (scanPtr != NULL) {
              scanOk = TRUE;
              scanCount = 0;
              specialPost = NULL;
              remainPost = NULL;
              ScanOnlyProc (trmp);
              if (scanCount > 0) {
                ProcessScanResults ();
              }
              if (specialPost != NULL && remainPost != NULL) {
                remainPost = DifferencePostingLists (remainPost, specialPost);
              }
              if (specialPost == NULL) {
                specialPost = BSNew (0);
              }
              if (remainPost == NULL) {
                remainPost = BSNew (0);
              }
              special = BSLen (specialPost) / sizeof (DocUid);
              remain = BSLen (remainPost) / sizeof (DocUid);
              total = special + remain;
              scanPtr = MemFree (scanPtr);
            }
            uidPtr = MemFree (uidPtr);
            if (scanOk && total > 0) {
              trmp = CdTermFree (trmp);
              trmp = CdEntrezCreateTerm (term, db, fld, specialPost, remainPost);
            }
            specialPost = BSFree (specialPost);
            remainPost = BSFree (remainPost);
          }
        }
      }
    }
  }
  return trmp;
}

/*****************************************************************************
*
*   CdEntrezPreloadMerge (term, type, field, spcl, totl)
*       Creates a CdTerm node in the entrez set structure if one does not yet
*       exist, and loads the posting file by merging multiple postings files.
*
*****************************************************************************/

static void NEAR SingleSpaces (CharPtr str)

{
  Char  ch;
  Int2  i;
  Int2  j;
  Int2  k;

  i = 0;
  j = 0;
  k = 0;
  ch = str [i];
  while (ch != '\0') {
    if (ch == ' ') {
      if (k == 0) {
        str [j] = ch;
        j++;
      }
      k++;
      i++;
    } else {
      k = 0;
      str [j] = ch;
      i++;
      j++;
    }
    ch = str [i];
  }
  str [j] = '\0';
}

static void NEAR TermTruncate (CharPtr str)

{
  if (str != NULL && str [0] != '\0') {
    SingleSpaces (str);
    if (searchTermLen < (Int2) StringLen (str)) {
      str [searchTermLen] = '\0';
    }
  }
}

static int LIBCALLBACK HeapCompare (VoidPtr ptr1, VoidPtr ptr2)

{
  DocUidPtr  uid1;
  DocUidPtr  uid2;

  if (ptr1 != NULL && ptr2 != NULL) {
    uid1 = (DocUidPtr) ptr1;
    uid2 = (DocUidPtr) ptr2;
    if (*uid1 > *uid2) {
      return 1;
    } else if (*uid1 < *uid2) {
      return -1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

static void NEAR QuickSortSmall (DocUidPtr uids, Int2 l, Int2 r)

{
  HeapSort (uids + l, (size_t) (r - l + 1), sizeof (DocUid), HeapCompare);
}

/*
static Boolean NEAR AlreadyInOrder (DocUidPtr uids, Int2 l, Int2 r)

{
  DocUid   last;
  Boolean  rsult;

  rsult = TRUE;
  if (l < r) {
    last = 0;
    while (l <= r) {
      if (uids [l] < last) {
        rsult = FALSE;
      }
      last = uids [l];
      l++;
    }
  }
  return rsult;
}

static void NEAR QuickSortSmall (DocUidPtr uids, Int2 l, Int2 r)

{
  DocUid  a;
  DocUid  b;
  DocUid  c;
  Int2    i;
  Int2    j;
  DocUid  temp;
  DocUid  x;

  if (AlreadyInOrder (uids, l, r)) {
    return;
  }
  i = l;
  j = r;
  a = uids [l];
  b = uids [(l + r) / 2];
  c = uids [r];
  if (a > b) {
    if (c > a) {
      x = a;
    } else if (c < b) {
      x = b;
    } else {
      x = c;
    }
  } else {
    if (c < a) {
      x = a;
    } else if (c > b) {
      x = b;
    } else {
      x = c;
    }
  }
  do {
    while (uids [i] < x) {
      i++;
    }
    while (x < uids [j]) {
      j--;
    }
    if (i <= j) {
      temp = uids [i];
      uids [i] = uids [j];
      uids [j] = temp;
      i++;
      j--;
    }
  } while (i <= j);
  if (i - l < r - j) {
    if (l < j) {
      QuickSortSmall (uids, l, j);
    }
    if (i < r) {
      QuickSortSmall (uids, i, r);
    }
  } else {
    if (i < r) {
      QuickSortSmall (uids, i, r);
    }
    if (l < j) {
      QuickSortSmall (uids, l, j);
    }
  }
}
*/

static Int2 NEAR CompressSmall (DocUidPtr uids, Int2 count)

{
  Int2  i;
  Int2  j;

  i = 0;
  j = 0;
  while (i < count) {
    if (uids [i] > 0) {
      uids [j] = uids [i];
      i++;
      j++;
    } else {
      i++;
    }
  }
  i = j;
  while (j < count) {
    uids [j] = 0;
    j++;
  }
  return i;
}

static Int2 NEAR UniqueSmall (DocUidPtr uids, Int2 count)

{
  Int2    i;
  Int2    j;
  DocUid  last;

  i = 0;
  if (count <= 1) {
    i = count;
  } else {
    i = 0;
    j = 0;
    last = 0;
    while (i < count) {
      if (uids [i] != last) {
        uids [j] = uids [i];
        last = uids [i];
        i++;
        j++;
      } else {
        i++;
      }
    }
    i = j;
    while (j < count) {
      uids [j] = 0;
      j++;
    }
  }
  return i;
}

static ByteStorePtr NEAR MergeSmallLists (ByteStorePtr bsp, ByteStorePtr small)

{
  Int2       count;
  Int4       len;
  DocUidPtr  uids;

  if (small != NULL) {
    len = BSLen (small) / (Int4) sizeof (DocUid);
    if (len <= 16000L && len > 0) {
      count = (Int2) len;
      uids = MemNew ((size_t) count * sizeof (DocUid));
      if (uids != NULL) {
        BSMerge (small, (VoidPtr) uids);
        small = BSFree (small);
        QuickSortSmall (uids, 0, (Int2) (count - 1));
        count = CompressSmall (uids, count);
        count = UniqueSmall (uids, count);
        if (count > 0) {
          small = BSNew (0L);
          BSWrite (small, uids, count * sizeof (DocUid));
        }
        uids = MemFree (uids);
        if (small != NULL) {
          bsp = MergePostingLists (bsp, small);
        }
      } else {
        Message (MSG_OK, "MergeSmallLists memory failure");
      }
    } else if (len > 16000L) {
      Message (MSG_OK, "MergeSmallLists > 64 K");
    }
  }
  return bsp;
}

static Boolean NEAR MergeUnorderedLists (Int2 i, Int2 count)

{
  BytePtr       bptr;
  Int2          finish;
  Boolean       goOn;
  Int2          j;
  Int2          len;
  Int4          max;
  DocUidPtr     mptr;
  Int2          number;
  Int4          offset;
  ByteStorePtr  remainLarge;
  ByteStorePtr  remainSmall;
  Int4          smallCount;
  Int2          start;
  Int4          total;

  goOn = TRUE;
  j = i + count - 1;
  max = scanPtr [j].offset + scanPtr [j].totalCount *
        (Int4) sizeof (DocUid) - scanPtr [i].offset;
  if (max <= 32000) {
    offset = scanPtr [i].offset;
    len = (Int2) (max / (Int4) sizeof (DocUid));
    CdTrmUidsMem (db, fld, offset, (Int4) len, uidPtr);
    remainSmall = NULL;
    smallCount = 0;
    for (j = i; j < i + count; j++) {
      scanPtr [j].offset -= offset;
      total = scanPtr [j].totalCount;
      bptr = ((BytePtr) uidPtr) + scanPtr [j].offset;
      mptr = (DocUidPtr) bptr;
      if (smallCount + total > 16000) {
        if (remainSmall != NULL) {
          remainPost = MergeSmallLists (remainPost, remainSmall);
          remainSmall = NULL;
        }
        smallCount = 0;
      }
      if (total > 100) {
        start = 0;
        number = 0;
        while (start < total) {
          finish = start + 1;
          while (finish < total && mptr [finish - 1] < mptr [finish]) {
            finish++;
          }
          number = finish - start;
          if (number > 100) {
            remainLarge = BSNew (number * sizeof (DocUid));
            BSWrite (remainLarge, (mptr + start), number * sizeof (DocUid));
            remainPost = MergePostingLists (remainPost, remainLarge);
          } else {
            smallCount += number;
            if (number > 0) {
              if (remainSmall == NULL) {
                remainSmall = BSNew (0L);
              }
              BSWrite (remainSmall, (mptr + start), number * sizeof (DocUid));
            }
            if (smallCount > 16000) {
              if (remainSmall != NULL) {
                remainPost = MergeSmallLists (remainPost, remainSmall);
                remainSmall = NULL;
              }
              smallCount = 0;
            }
          }
          start = finish;
        }
      } else {
        smallCount += total;
        if (total > 0) {
          if (remainSmall == NULL) {
            remainSmall = BSNew (0L);
          }
          BSWrite (remainSmall, mptr, total * sizeof (DocUid));
        }
      }
    }
    if (remainSmall != NULL) {
      remainPost = MergeSmallLists (remainPost, remainSmall);
      remainSmall = NULL;
    }
  } else {
    Message (MSG_OK, "Cannot merge > 32 K element");
    scanOk = FALSE;
    goOn = FALSE;
  }
  return goOn;
}

static Boolean NEAR MergeSeveralOrderedLists (Int2 i, Int2 count)

{
  BytePtr       bptr;
  Boolean       goOn;
  Int2          j;
  Int2          len;
  Int4          max;
  DocUidPtr     mptr;
  Int4          offset;
  Int4          remainder;
  ByteStorePtr  remainLarge;
  ByteStorePtr  remainSmall;
  Int4          smallCount;
  Int4          special;
  ByteStorePtr  specialLarge;
  ByteStorePtr  specialSmall;
  Int4          total;

  goOn = TRUE;
  j = i + count - 1;
  max = scanPtr [j].offset + scanPtr [j].totalCount *
        (Int4) sizeof (DocUid) - scanPtr [i].offset;
  if (max <= 32000) {
    offset = scanPtr [i].offset;
    len = (Int2) (max / (Int4) sizeof (DocUid));
    CdTrmUidsMem (db, fld, offset, (Int4) len, uidPtr);
    specialSmall = NULL;
    remainSmall = NULL;
    smallCount = 0;
    for (j = i; j < i + count; j++) {
      scanPtr [j].offset -= offset;
      special = scanPtr [j].specialCount;
      total = scanPtr [j].totalCount;
      remainder = total - special;
      bptr = ((BytePtr) uidPtr) + scanPtr [j].offset;
      mptr = (DocUidPtr) bptr;
      if (smallCount + total > 16000) {
        if (specialSmall != NULL) {
          specialPost = MergeSmallLists (specialPost, specialSmall);
          specialSmall = NULL;
        }
        if (remainSmall != NULL) {
          remainPost = MergeSmallLists (remainPost, remainSmall);
          remainSmall = NULL;
        }
        smallCount = 0;
      }
      if (total > 100) {
        specialLarge = BSNew (special * sizeof (DocUid));
        BSWrite (specialLarge, mptr, special * sizeof (DocUid));
        specialPost = MergePostingLists (specialPost, specialLarge);
        remainLarge = BSNew (remainder * sizeof (DocUid));
        BSWrite (remainLarge, (mptr + special),
                 remainder * sizeof (DocUid));
        remainPost = MergePostingLists (remainPost, remainLarge);
      } else {
        smallCount += total;
        if (special > 0) {
          if (specialSmall == NULL) {
            specialSmall = BSNew (0L);
          }
          BSWrite (specialSmall, mptr, special * sizeof (DocUid));
        }
        if (remainder > 0) {
          if (remainSmall == NULL) {
            remainSmall = BSNew (0L);
          }
          BSWrite (remainSmall, (mptr + special), remainder * sizeof (DocUid));
        }
      }
    }
    if (specialSmall != NULL) {
      specialPost = MergeSmallLists (specialPost, specialSmall);
      specialSmall = NULL;
    }
    if (remainSmall != NULL) {
      remainPost = MergeSmallLists (remainPost, remainSmall);
      remainSmall = NULL;
    }
  } else {
    Message (MSG_OK, "Cannot merge > 32 K element");
    scanOk = FALSE;
    goOn = FALSE;
  }
  return goOn;
}

static Boolean NEAR MergeSeveralLists (Int2 i, Int2 count)

{
  if (fld != FLD_ORGN) {
    return MergeSeveralOrderedLists (i, count);
  } else {
    return MergeUnorderedLists (i, count);
  }
}

static Boolean NEAR ProcessScanResults (void)

{
  Boolean  goOn;
  Int2     i;
  Int2     j;
  Int4     max;

  ProgMon ("ProcessScanResults");
  goOn = TRUE;
  i = 0;
  j = 0;
  max = 0;
  while (j < scanCount) {
    if (scanPtr [j].offset < scanPtr [i].offset) {
      goOn = MergeSeveralLists (i, (Int2) (j - i));
      max = 0;
      i = j;
    } else {
      max = scanPtr [j].offset + scanPtr [j].totalCount *
            (Int4) sizeof (DocUid) - scanPtr [i].offset;
      if (max >= 32000) {
        if (j == i) {
          goOn = MergeSeveralLists (i, 1);
          j++;
          i = j;
          max = 0;
        } else {
          goOn = MergeSeveralLists (i, (Int2) (j - i));
          i = j;
          max = 0;
        }
      } else {
        j++;
      }
    }
  }
  if (max > 0) {
    goOn = MergeSeveralLists (i, (Int2) (j - i));
  }
  scanCount = 0;
  return goOn;
}

static Boolean  ScanOnlyProc (CdTermPtr trmp)

{
  Int4     count;
  Boolean  goOn;

  goOn = TRUE;
  if (scanCount >= SCAN_MAX) {
    goOn = ProcessScanResults ();
  }
  if (scanCount < SCAN_MAX) {
    if (trmp->total_count >= 8000) {
      while (trmp->special_count > 0) {
        if (scanCount >= SCAN_MAX) {
          goOn = ProcessScanResults ();
        }
        count = MIN (trmp->special_count, 8000L);
        scanPtr [scanCount].specialCount = count;
        scanPtr [scanCount].totalCount = count;
        scanPtr [scanCount].offset = trmp->offset;
        scanPtr [scanCount].specialPtr = NULL;
        scanPtr [scanCount].remainderPtr = NULL;
        scanCount++;
        trmp->special_count -= count;
        trmp->total_count -= count;
        trmp->offset += count * sizeof (DocUid);
      }
      while (trmp->total_count > 0) {
        if (scanCount >= SCAN_MAX) {
          goOn = ProcessScanResults ();
        }
        count = MIN (trmp->total_count, 8000L);
        scanPtr [scanCount].specialCount = 0;
        scanPtr [scanCount].totalCount = count;
        scanPtr [scanCount].offset = trmp->offset;
        scanPtr [scanCount].specialPtr = NULL;
        scanPtr [scanCount].remainderPtr = NULL;
        scanCount++;
        trmp->total_count -= count;
        trmp->offset += count * sizeof (DocUid);
      }
    } else {
      if (scanCount >= SCAN_MAX) {
        goOn = ProcessScanResults ();
      }
      scanPtr [scanCount].specialCount = trmp->special_count;
      scanPtr [scanCount].totalCount = trmp->total_count;
      scanPtr [scanCount].offset = trmp->offset;
      scanPtr [scanCount].specialPtr = NULL;
      scanPtr [scanCount].remainderPtr = NULL;
      scanCount++;
    }
  }
  return goOn;
}

static Boolean  WildCardProc (CdTermPtr trmp)

{
  Int2     diff;
  Boolean  goOn;
  CharPtr  src;
  CharPtr  tgt;

  goOn = FALSE;
  src = selection;
  tgt = trmp->term;
  diff = 0;
  while (*src != '\0' && *tgt != '\0' && diff == 0) {
    if (*src != '?') {
      diff = TO_UPPER (*src) - TO_UPPER (*tgt);
    }
    if (diff == 0) {
      src++;
      tgt++;
    }
  }
  if (diff != 0) {
    if (*src == '*') {
      goOn = TRUE;
    }
  } else if (*src == '*') {
    goOn = TRUE;
  } else if (*src == '\0' && *tgt == '\0') {
    goOn = TRUE;
  } else {
    goOn = FALSE;
  }
  return goOn;
}

static Boolean  ScanAndFreeProc (CdTermPtr trmp)

{
  Int2     compare;
  Boolean  goOn;
  Char     str [256];

  goOn = TRUE;
  if (trmp != NULL && trmp->term != NULL) {
    StringNCpy (str, trmp->term, sizeof (str));
    TermTruncate (str);
    if (userScanProc != NULL) {
      compare = MeshStringICmp (str, wildcard);
    } else {
      compare = MeshStringICmp (str, selection);
    }
    if (compare > 0) {
      str [searchTermLen] = '\0';
      if (userScanProc != NULL) {
        compare = MeshStringICmp (str, wildcard);
      } else {
        compare = MeshStringICmp (str, selection);
      }
      if (compare > 0) {
        goOn = FALSE;
      }
    } else if (compare == 0) {
      if (userScanProc != NULL) {
        if (userScanProc (trmp)) {
          goOn = ScanOnlyProc (trmp);
        }
      } else {
        goOn = ScanOnlyProc (trmp);
      }
    }
  }
  trmp = CdTermFree (trmp);
  return goOn;
}

static Boolean NEAR CdEntrezMergeTerm (DocType type, DocField field, CharPtr term,
                                        Int4Ptr spcl, Int4Ptr totl, CdTermProc userProc)

{
  Char  ch;
  Int4  remain;
  Int4  special;
  Char  str [256];
  Int4  total;
  Int2  termpage;
  CharPtr tmp;
  Boolean retval = FALSE;

  if (spcl != NULL) {
    *spcl = 0;
  }
  if (totl != NULL) {
    *totl = 0;
  }
  db = type;
  fld = field;
  userScanProc = userProc;
  StringNCpy (str, term, sizeof (str));
  tmp = str;
  while (*tmp != '\0') {
    tmp++;
  }
  tmp -= 3;
  if ((*tmp == '.') && (*(tmp+1) == '.') && (*(tmp+2) == '.')) {
    *tmp = '\0';
  }
  SingleSpaces (str);
  if (userProc != NULL) {
    searchTermLen = 0;
    ch = str [searchTermLen];
    while (ch != '\0' && ch != '*' && ch != '?') {
      searchTermLen++;
      ch = str [searchTermLen];
    }
  } else {
    searchTermLen = (Int2) StringLen (str);
  }
  if (searchTermLen > 0 || str [0] == '?' || str [0] == '*') {
    scanOk = TRUE;
    uidPtr = MemNew ((size_t) 32000);
    if (uidPtr != NULL) {
      scanPtr = MemNew (SCAN_MAX * sizeof (ScanData));
      if (scanPtr != NULL) {
        scanCount = 0;
        specialPost = NULL;
        remainPost = NULL;
        StringNCpy (selection, str, sizeof (selection));
        StringNCpy (wildcard, str, sizeof (wildcard));
        wildcard [searchTermLen] = '\0';
        termpage = CdTrmLookup (db, fld, wildcard);
        if (fld == FLD_MESH) {
          ch = str [0];
          str [0] = TO_UPPER (ch);
        }
        if (termpage >= 0) {
          CdTermScan (db, fld, termpage, (Int2)0, ScanAndFreeProc);
        }
        if (scanCount > 0) {
          ProcessScanResults ();
        }
        if (specialPost != NULL && remainPost != NULL) {
          remainPost = DifferencePostingLists (remainPost, specialPost);
        }
        if (specialPost == NULL) {
          specialPost = BSNew (0);
        }
        if (remainPost == NULL) {
          remainPost = BSNew (0);
        }
        special = BSLen (specialPost) / sizeof (DocUid);
        remain = BSLen (remainPost) / sizeof (DocUid);
        total = special + remain;
        scanPtr = MemFree (scanPtr);
      }
      uidPtr = MemFree (uidPtr);
      if (scanOk && total > 0) {
		retval = TRUE;
		if (userProc == NULL) {
			StringCat (str, "...");
		}
        CdEntrezCreateTerm (str, db, fld, specialPost, remainPost);
        if (spcl != NULL) {
          *spcl = special;
        }
        if (totl != NULL) {
          *totl = total;
        }
      }
      specialPost = BSFree (specialPost);
      remainPost = BSFree (remainPost);
    }
  }
  return retval;
}

/*****************************************************************************
*
*   CdEntMedlineEntryListGet (result, numuid, uids, mark_missing)
*   	returns a count of entries read
*   	if (mark_missing) ids which could not be located are made negative
*
*****************************************************************************/
Int2  CdEntMedlineEntryListGet (MedlineEntryPtr PNTR result, Int2 numuid, Int4Ptr uids, Boolean mark_missing)

{
	MedlineEntryPtr mep;
	Int2 count = 0, ctr;
	AsnIoPtr aip;
	DocType db = TYP_ML;

	if (! MedlineAsnLoad())
		return 0;

	for (ctr = 0; ctr < numuid; ctr++)
	{
		mep = NULL;
		aip = CdDocAsnOpen(db, uids[ctr]);
		if (aip != NULL)
		{
		 	mep = MedlineEntryAsnRead(aip, NULL);
	 		CdDocAsnClose(aip);
		}
		if (mep == NULL)    /* didn't get it */
		{
			if (mark_missing)
				uids[ctr] *= -1;
		}
		else
		{
			count++;
			result[ctr] = mep;
		}
	}
	
	return count;
}

/*****************************************************************************
*
*   CdEntSeqEntryListGet (result, numuid, uids, retcode, mark_missing)
*   	returns a count of entries read
*   	if (mark_missing) ids which could not be located are made negative
*       retcode is defined in objsset.h 
*
*****************************************************************************/
Int2  CdEntSeqEntryListGet (SeqEntryPtr PNTR result, Int2 numuid, Int4Ptr uids, Int2 retcode, Boolean mark_missing)
{
	SeqEntryPtr sep;
	Int2 count = 0, ctr;
	AsnIoPtr aip;
	DocType db = TYP_SEQ;
	Giim localid;
	ValNode an;

	if (! SeqSetAsnLoad())
		return 0;

    an.data.ptrvalue = &localid;
    an.choice = SEQID_GIIM;

	for (ctr = 0; ctr < numuid; ctr++)
	{
		sep = NULL;
		aip = CdDocAsnOpen(db, uids[ctr]);
		if (aip != NULL)
		{
			localid.id = uids[ctr];
		 	sep = SeqEntryAsnGet(aip, NULL, &an, retcode);
	 		CdDocAsnClose(aip);
		}
		if (sep == NULL)    /* didn't get it */
		{
			if (mark_missing)
				uids[ctr] *= -1;
		}
		else
		{
			count++;
			result[ctr] = sep;
		}
	}
	
	return count;
}


/*****************************************************************************
*
*   CdEntMlSumListGet (result, numuid, uids)
*   	returns a count of entries read
*   	head of linked list is in result
*
*****************************************************************************/
extern Int2  CdEntMlSumListGet (DocSumPtr PNTR result, Int2 numuid, Int4Ptr uids)          /* Gi numbers */

{
	Int2 count = 0, ctr;
	DocType db = TYP_ML;
	AsnIoPtr aip;

	for (ctr = 0; ctr < numuid; ctr++)
	{
		result[ctr] = NULL;
		aip = CdDocAsnOpen (db, uids[ctr]);
	    if (aip != NULL)
		{
			result[ctr] = MedSumAsnRead(aip, uids[ctr]);
			CdDocAsnClose(aip);
			if (result[ctr] != NULL)
				count++;
		}
	}
	
	return count;
}

/*****************************************************************************
*
*   CdEntMlSumGet(uid)
*   	get one MlSummary
*
*****************************************************************************/
static DocSumPtr NEAR CdEntMlSumGet (Int4 uid)

{
	DocSumPtr dsp = NULL;

	CdEntMlSumListGet(&dsp, 1, &uid);
	return dsp;
}

/*****************************************************************************
*
*   void StripAuthor(author)
*
*****************************************************************************/
static void NEAR StripAuthor (CharPtr author)

{
  CharPtr  p1, p2;

  p1 = author;
  while ((p1 = StringChr (p1, ' ')) != NULL) {
    for (p2 = p1 + 1; *p2 != '\0'; p2++) {
      if (*p2 == ' ') break;
      if (IS_ALPHA (*p2) && IS_LOWER (*p2)) break;
    }
    if (*p2 == '\0' || *p2 == ' ') {
      *p1 = '\0';
      return;
    }
    p1++;
  }
}

/*****************************************************************************
*
*   MedSumAsnRead(aip, uid)
*
*****************************************************************************/
static void NEAR FindAsnType (AsnTypePtr PNTR atp, AsnModulePtr amp, CharPtr str)

{
  if (atp != NULL && (*atp) == NULL) {
    *atp = AsnTypeFind (amp, str);
  }
}

static DocSumPtr NEAR MedSumAsnRead (AsnIoPtr aip, DocUid uid)

{
  DataVal       av;
  AsnModulePtr  amp;
  AsnTypePtr    atp;
  Boolean       citFound;
  DocSumPtr     dsp;
  Boolean       goOn;
  Int2          i;
  CharPtr       ptr;
  Char          caption [50];
  Char          author [40];
  Char          year [10];

  if ((aip == NULL) || (! AllObjLoad ()))
    return NULL;

	  amp = AsnAllModPtr ();

	  FindAsnType (&MEDLINE_ENTRY, amp, "Medline-entry");
	  FindAsnType (&MEDLINE_ENTRY_cit, amp, "Medline-entry.cit");
	  FindAsnType (&MEDLINE_ENTRY_abstract, amp, "Medline-entry.abstract");
	  FindAsnType (&TITLE_E_trans, amp, "Title.E.trans");
	  FindAsnType (&AUTH_LIST_names_ml_E, amp, "Auth-list.names.ml.E");
	  FindAsnType (&AUTH_LIST_names_str_E, amp, "Auth-list.names.str.E");
	  FindAsnType (&DATE_STD_year, amp, "Date-std.year");
	  FindAsnType (&DATE_str, amp, "Date.str");
	  FindAsnType (&TITLE_E_name, amp, "Title.E.name");
	  FindAsnType (&MEDLINE_ENTRY_mesh, amp, "Medline-entry.mesh");
	  FindAsnType (&MEDLINE_ENTRY_substance, amp, "Medline-entry.substance");
	  FindAsnType (&MEDLINE_ENTRY_xref, amp, "Medline-entry.xref");
	  FindAsnType (&MEDLINE_ENTRY_idnum, amp, "Medline-entry.idnum");
	  FindAsnType (&MEDLINE_ENTRY_gene, amp, "Medline-entry.gene");

  atp = AsnReadId (aip, amp, MEDLINE_ENTRY);
  AsnReadVal (aip, atp, &av);

  dsp = MemNew (sizeof (DocSum));
  if (dsp != NULL) {
    dsp->no_abstract = TRUE;
    dsp->translated_title = FALSE;
    dsp->no_authors = TRUE;
    author [0] = '\0';
    year [0] = '\0';
    citFound = FALSE;
    goOn = TRUE;
    while (goOn) {
      atp = AsnReadId (aip, amp, atp);
      if (atp == MEDLINE_ENTRY) {
        AsnReadVal (aip, atp, NULL);
        goOn = FALSE;
      } else if (atp == MEDLINE_ENTRY_cit) {
        AsnReadVal (aip, atp, NULL);
        citFound = TRUE;
      } else if (atp == MEDLINE_ENTRY_abstract) {
        AsnReadVal (aip, atp, NULL);
        dsp->no_abstract = FALSE;
        goOn = FALSE;
      } else if (atp == TITLE_E_trans) {
        AsnReadVal (aip, atp, &av);
        dsp->translated_title = TRUE;
        if (dsp->title != NULL) {
          dsp->title = MemFree (dsp->title);
        }
        dsp->title = MemNew ((size_t) StringLen ((CharPtr) av.ptrvalue) + 3);
        ptr = dsp->title;
        *ptr = '[';
        ptr++;
        ptr = StringMove (ptr, (CharPtr) av.ptrvalue);
        *ptr = ']';
        ptr++;
        *ptr = '\0';
        AsnKillValue (atp, &av);
      } else if (atp == AUTH_LIST_names_ml_E) {
        AsnReadVal (aip, atp, &av);
        dsp->no_authors = FALSE;
        if (author [0] == '\0') {
          StringNCpy (author, (CharPtr) av.ptrvalue, sizeof (author));
        }
        AsnKillValue (atp, &av);
      } else if (atp == AUTH_LIST_names_str_E) {
        AsnReadVal (aip, atp, &av);
        dsp->no_authors = FALSE;
        if (author [0] == '\0') {
          StringNCpy (author, (CharPtr) av.ptrvalue, sizeof (author));
        }
        AsnKillValue (atp, &av);
      } else if (atp == DATE_STD_year) {
        AsnReadVal (aip, atp, &av);
        if (citFound) {
          sprintf (year, "%ld", av.intvalue);
        }
      } else if (atp == DATE_str) {
        AsnReadVal (aip, atp, &av);
        if (citFound) {
          i = 0;
          ptr = av.ptrvalue;
          while (ptr [i] != '\0' && ptr [i] != ' ' && i < sizeof (year) - 1) {
            year [i] = ptr [i];
            i++;
          }
          year [i] = '\0';
        }
        AsnKillValue (atp, &av);
      } else if (atp == TITLE_E_name) {
        AsnReadVal (aip, atp, &av);
        if (dsp->title == NULL) {
          dsp->title = StringSave ((CharPtr) av.ptrvalue);
        }
        AsnKillValue (atp, &av);
      } else if (atp == MEDLINE_ENTRY_mesh || atp == MEDLINE_ENTRY_substance ||
                 atp == MEDLINE_ENTRY_xref || atp == MEDLINE_ENTRY_idnum ||
                 atp == MEDLINE_ENTRY_gene) {
        AsnReadVal (aip, atp, NULL);
        goOn = FALSE;
      } else {
        AsnReadVal (aip, atp, NULL);
      }
    }
    if (dsp->no_authors) {
      sprintf (caption, "[%ld], %s", uid, year);
    } else if (author [0] != '\0') {
      StripAuthor (author);
      author [12] = '.';
      author [12] = '\0';
      sprintf (caption, "%s, %s", author, year);
    } else {
      sprintf (caption, "[%ld], %s", uid, year);
    }
    dsp->caption = StringSave (caption);
    dsp->uid = uid;
  }
  AsnIoReset (aip);
  return dsp;
}

/*****************************************************************************
*
*   CdEntSeqSumListGet (result, numuid, db, uids)
*   	returns a count of entries read
*   	head of linked list is in result
*
*****************************************************************************/
extern Int2  CdEntSeqSumListGet (DocSumPtr PNTR result, Int2 numuid, DocType db, Int4Ptr uids)          /* Gi numbers */

{
	Int2 count = 0, ctr;
	AsnIoPtr aip;

	for (ctr = 0; ctr < numuid; ctr++)
	{
		result[ctr] = NULL;
		aip = CdDocAsnOpen (db, uids[ctr]);
	    if (aip != NULL)
		{
			result[ctr] = SeqSumAsnRead(aip, uids[ctr]);
			CdDocAsnClose(aip);
			if (result[ctr] != NULL)
				count++;
		}
	}

	return count;
}

/*****************************************************************************
*
*   CdEntSeqSumGet(uid, type)
*   	get one SeqSummary
*
*****************************************************************************/
static DocSumPtr NEAR CdEntSeqSumGet (Int4 uid, DocType type)

{
	DocSumPtr dsp = NULL;

	CdEntSeqSumListGet(&dsp, 1, type, &uid);
	return dsp;
}

static DocSumPtr NEAR SeqSumAsnRead (AsnIoPtr aip, DocUid uid)

{
  DataVal       av;
  AsnModulePtr  amp;
  AsnTypePtr    atp;
  DocSumPtr     dsp;
  Boolean       goOn;
  Char          caption [50];
  Char          author [40];
  Char          year [10];
  Char          locus [40];
  Char          cds [10];
  CharPtr       chptr;
  Int2          proteins;
  CharPtr       recentTitle;
  Boolean       backbone;
  Boolean       genBank;
  Boolean       embl;
  Boolean       ddbj;
  Boolean       pir;
  Boolean       swissprot;
  Boolean       isaNA;
  Boolean       isaAA;
  Boolean       isaSEG;
  Boolean		in_id;
  Int2          level;

  if ((aip == NULL) || (! AllObjLoad ()))
    return NULL;

  amp = AsnAllModPtr ();

  FindAsnType (&SEQ_ENTRY, amp, "Seq-entry");
  FindAsnType (&SEQ_ENTRY_seq, amp, "Seq-entry.seq");
  FindAsnType (&SEQ_ENTRY_set, amp, "Seq-entry.set");
  FindAsnType (&TEXTSEQ_ID_name, amp, "Textseq-id.name");
  FindAsnType (&TEXTSEQ_ID_accession, amp, "Textseq-id.accession");
  FindAsnType (&AUTH_LIST_names_str_E, amp, "Auth-list.names.str.E");
  FindAsnType (&DATE_STD_year, amp, "Date-std.year");
  FindAsnType (&DATE_str, amp, "Date.str");
  FindAsnType (&SEQ_DESCR_E_title, amp, "Seq-descr.E.title");
  FindAsnType (&GIIMPORT_ID_id, amp, "Giimport-id.id");
  FindAsnType (&BIOSEQ_inst, amp, "Bioseq.inst");
  FindAsnType (&SEQ_INST_mol, amp, "Seq-inst.mol");
  FindAsnType (&SEQ_INST_repr, amp, "Seq-inst.repr");
  FindAsnType (&SEQ_ID_gibbsq, amp, "Seq-id.gibbsq");
  FindAsnType (&SEQ_ID_gibbmt, amp, "Seq-id.gibbmt");
  FindAsnType (&SEQ_ID_genbank, amp, "Seq-id.genbank");
  FindAsnType (&SEQ_ID_embl, amp, "Seq-id.embl");
  FindAsnType (&SEQ_ID_ddbj, amp, "Seq-id.ddbj");
  FindAsnType (&SEQ_ID_pir, amp, "Seq-id.pir");
  FindAsnType (&SEQ_ID_swissprot, amp, "Seq-id.swissprot");
  FindAsnType (&PDB_BLOCK_compound_E, amp, "PDB-block.compound.E");
  FindAsnType (&PDB_SEQ_ID_MOL, amp, "PDB-seq-id.mol");
  FindAsnType (&BIOSEQ_id, amp, "Bioseq.id");
  FindAsnType (&CIT_PAT_title, amp, "Cit-pat.title");

  atp = AsnReadId (aip, amp, SEQ_ENTRY);
  AsnReadVal (aip, atp, &av);

  atp = AsnReadId (aip, amp, atp);
  AsnReadVal (aip, atp, &av);

  dsp = MemNew (sizeof (DocSum));
  if (dsp != NULL) {
    dsp->no_abstract = TRUE;
    dsp->translated_title = FALSE;
    dsp->no_authors = TRUE;
    author [0] = '\0';
    year [0] = '\0';
    locus [0] = '\0';
    cds [0] = '\0';
    proteins = 1;
    recentTitle = NULL;
    backbone = FALSE;
    genBank = FALSE;
    embl = FALSE;
	ddbj = FALSE;
    pir = FALSE;
    swissprot = FALSE;
    isaNA = FALSE;
    isaAA = FALSE;
    isaSEG = FALSE;
	in_id = FALSE;
    goOn = TRUE;
    level = AsnGetLevel (aip);
    while (goOn) {
      atp = AsnReadId (aip, amp, atp);
      if (atp == SEQ_ENTRY_seq || atp == SEQ_ENTRY_set) {
        AsnReadVal (aip, atp, NULL);
        if (AsnGetLevel (aip) <= level) {
          goOn = FALSE;
	    }
      } else if (atp == BIOSEQ_id) {
        AsnReadVal (aip, atp, &av);
		if (in_id) {
			in_id = FALSE;
		} else {
			in_id = TRUE;
        }
      } else if (in_id && ((atp == TEXTSEQ_ID_name) ||
 	  		(atp == PDB_SEQ_ID_MOL))) {
        AsnReadVal (aip, atp, &av);
        if (locus [0] == '\0') {
          StringNCpy (locus, (CharPtr) av.ptrvalue, sizeof (locus));
        }
        AsnKillValue (atp, &av);
      } else if (in_id && (atp == TEXTSEQ_ID_accession)) {
        AsnReadVal (aip, atp, &av);
        if (locus [0] == '\0') {
          StringNCpy (locus, (CharPtr) av.ptrvalue, sizeof (locus));
        }
        AsnKillValue (atp, &av);
      } else if (atp == AUTH_LIST_names_str_E) {
        AsnReadVal (aip, atp, &av);
        if (author [0] == '\0') {
          StringNCpy (author, (CharPtr) av.ptrvalue, sizeof (author));
        }
        AsnKillValue (atp, &av);
      } else if (atp == DATE_STD_year) {
        AsnReadVal (aip, atp, &av);
        sprintf (year, "%ld", av.intvalue);
      } else if (atp == DATE_str) {
        AsnReadVal (aip, atp, &av);
        StringNCpy (year, (CharPtr) av.ptrvalue, sizeof (year));
        AsnKillValue (atp, &av);
      } else if ((atp == SEQ_DESCR_E_title) ||
 	  	(atp == PDB_BLOCK_compound_E) || (atp == CIT_PAT_title)) {
        AsnReadVal (aip, atp, &av);
		if (*((CharPtr)av.ptrvalue) != '\0')
		{
	        if (recentTitle != NULL) {
    	      recentTitle = MemFree (recentTitle);
        	}

	        if (dsp->uid == uid && dsp->title == NULL &&
				atp != CIT_PAT_title) {
    	      dsp->title = (CharPtr)av.ptrvalue;
	        }
			else
				recentTitle = (CharPtr)av.ptrvalue;
		}
		else
	        AsnKillValue (atp, &av);
      } else if (atp == GIIMPORT_ID_id) {
        AsnReadVal (aip, atp, &av);
        if (av.intvalue == uid) {
          dsp->uid = uid;
        }
      } else if (atp == SEQ_INST_mol) {
        AsnReadVal (aip, atp, &av);
        if ((! isaNA) && (! isaAA) && dsp->uid == uid) {
          isaNA = (Boolean) ISA_na (av.intvalue);
          isaAA = (Boolean) ISA_aa (av.intvalue);
          if (isaAA && cds [0] == '\0') {
            sprintf (cds, " cds%d", proteins);
          }
        }
        if (ISA_aa (av.intvalue)) {
          proteins++;
        }
      } else if (atp == SEQ_INST_repr) {
        AsnReadVal (aip, atp, &av);
        if (av.intvalue == Seq_repr_seg) {
          isaSEG = TRUE;
        }
      } else if (atp == BIOSEQ_inst) {
        AsnReadVal (aip, atp, NULL);
        if (dsp->uid == uid && dsp->title == NULL) {
          dsp->title = recentTitle;
          recentTitle = NULL;
        }
      } else if (atp == SEQ_ID_gibbsq || atp == SEQ_ID_gibbmt) {
        AsnReadVal (aip, atp, NULL);
        backbone = TRUE;
      } else if (atp == SEQ_ID_genbank) {
        AsnReadVal (aip, atp, NULL);
		if (in_id)
	        genBank = TRUE;
      } else if (atp == SEQ_ID_embl) {
        AsnReadVal (aip, atp, NULL);
		if (in_id)
	        embl = TRUE;
      } else if (atp == SEQ_ID_ddbj) {
        AsnReadVal (aip, atp, NULL);
		if (in_id)
	        ddbj = TRUE;
      } else if (atp == SEQ_ID_pir) {
        AsnReadVal (aip, atp, NULL);
		if (in_id)
	        pir = TRUE;
      } else if (atp == SEQ_ID_swissprot) {
        AsnReadVal (aip, atp, NULL);
		if (in_id)
	        swissprot = TRUE;
      } else {
        AsnReadVal (aip, atp, NULL);
      }
      if (dsp->title != NULL && dsp->uid == uid) {
        if (backbone) {
          if (author [0] != '\0' && year [0] != '\0') {
            goOn = FALSE;
          }
        } else if (genBank || embl || ddbj) {
          if (locus [0] != '\0') {
            if (isaAA && cds [0] != '\0') {
              goOn = FALSE;
            } else if (isaNA) {
              goOn = FALSE;
            }
          }
        } else if (pir) {
          if (locus [0] != '\0') {
            goOn = FALSE;
          }
        } else if (swissprot) {
          if (locus [0] != '\0') {
            goOn = FALSE;
          }
        } else if (embl) {
        }
      }
    }
    if (backbone) {
      chptr = StringChr (author, ',');
      if (chptr != NULL) {
        *chptr = '\0';
      }
      chptr = StringChr (year, ' ');
      if (chptr != NULL) {
        *chptr = '\0';
      }
      author [12] = '.';
      author [12] = '\0';
      sprintf (caption, "%s, %s", author, year);
      dsp->caption = StringSave (caption);
    } else if (genBank || embl || ddbj) {
      if (isaAA) {
        sprintf (caption, "%s%s", locus, cds);
      } else if (isaSEG) {
        sprintf (caption, "%s segs", locus);
      } else {
        sprintf (caption, "%s", locus);
      }
      dsp->caption = StringSave (caption);
    } else {
      sprintf (caption, "%s", locus);
      dsp->caption = StringSave (caption);
    }
    dsp->uid = uid;
    if (recentTitle != NULL) {
      recentTitle = MemFree (recentTitle);
    }
  }
  AsnIoReset (aip);
  return dsp;
}
