/*  toreport.c
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
* File Name:  toreport.c
*
* Author:  Jonathan Kans
*
* Version Creation Date: 9/24/91
*
* $Revision: 2.33 $
*
* File Description:  Generates a sequence report from a SeqView array
*
* Modifications:
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 11/7/91  Cavanaugh	Added support for Bond and Site locations.
*			Added support for SwissProt ids. Added descriptive
*			text for miscellaneous feature types.
* 11-08-91 Schuler	Tweaks to make Lipman happy.
* 11-20-91 Schuler	Cleaned up Protein name and Gene name a little.
*
*
* ==========================================================================
*/

#include <ncbi.h>
#include <asn.h>
#include <objmedli.h>
#include <objsset.h>
#include <toreport.h>
#include <tofile.h>

/* ----- Constants and Macros ----- */

#define BUFSIZE 8192

#define CHUNK 64
#define ITEM_MAX     (65535/sizeof(SeqView))

/* ----- Variables ----- */

static CharPtr     buffer;
static CharPtr     pos;
static VoidPtr     instnce;
static ReportProc  report;
static Boolean     showIds =TRUE;
static Boolean		CheckForm=FALSE;

static SeqViewPtr  pFlat;
static Int2        nItems;
static Int2        kOrg;
static Boolean     bMemErr;
static Boolean     success;

static Boolean     firstTime;
static Boolean     inpubset, pubdone;

static char        *sVirtual = "(virtual sequence)";
static char        *sUntitled = "(untitled)";

/* ----- Function Prototypes ----- */

static Int2 AddItem PROTO((Int2 nPos, Int2 nType, VoidPtr pDat));
static Int2 AddIdentifierItem PROTO((Int2 nPos, Int2 nType, VoidPtr pDat, Boolean xref));
static Int2 AddBogusItem PROTO((Int2 nPos, CharPtr string));
static void MoveItem PROTO((int dst, int src));
static void PostProcess PROTO((void));
static void FixNucProt PROTO((int n1, int n2));
static void DoSeqEntry PROTO((ValNodePtr pEntry));
static void DoBioseq PROTO((BioseqPtr pSeq));
static void DoBioseqSet PROTO((BioseqSetPtr pSet));
static Int2 DoIdentifiers PROTO((Int2 nBase, ValNodePtr pIden, Boolean xref));
static Int2 DoDbtags PROTO((Int2 nBase, ValNodePtr pIden));
static Int2 DoDescription PROTO((Int2 nBase, ValNodePtr pDesc));
static void DoAnnotation PROTO((SeqAnnotPtr p));
static Int2 DoComments PROTO((CharPtr pString));
static void DoSequence PROTO((BioseqPtr p));
static void FormatIdent PROTO((ValNodePtr pIden, CharPtr buff));

static void AddString PROTO((CharPtr string));
static void AddChar PROTO((char ch));
static void Intervals PROTO((ValNodePtr pNode));
static void FormatMember PROTO((BioseqPtr pSeq));
static void FormatAuth PROTO((AuthListPtr pAuth));
static void FeatureText PROTO((SeqFeatPtr pFeat, CharPtr key));
static void BeginEntry PROTO((SeqViewPtr pView));
static void Definition PROTO((SeqViewPtr pView));
static void Contents PROTO((BioseqSetPtr pSet));
static void Segments PROTO((ValNodePtr pSegs));
static void GenBank PROTO((CharPtr string));
static void Ncbi PROTO((CharPtr string));
static void Embl PROTO((CharPtr string));
static void Ddbj PROTO((CharPtr string));
static void Pir PROTO((CharPtr string));
static void SwissProt PROTO((CharPtr string));
static void Patent PROTO((CharPtr string));
static void Prf PROTO((CharPtr string));
static void Pdb PROTO((CharPtr string));
static void Xref PROTO((CharPtr string));
static void Organism PROTO((OrgRefPtr pOrg));
static void Method PROTO((CharPtr str));
static void RelDate PROTO((ValNodePtr pRelDate));
static void Reference PROTO((PubdescPtr pPub, ValNodePtr pIvls));
static void PubInformation PROTO((PubdescPtr pPub));
static Int2 SkipPastWhiteSpace PROTO((CharPtr text, Int2 cnt));
static void Comment PROTO((SeqViewPtr pView));
static void FeatString PROTO((CharPtr key, CharPtr string, SeqFeatPtr pFeat));
static void CdRegionFeat PROTO((SeqFeatPtr pFeat));
static void ProteinFeat PROTO((SeqFeatPtr pFeat));
static void SeqRefFeat PROTO((SeqFeatPtr pFeat));
static void GeneFeat PROTO((SeqFeatPtr pFeat));
static void OrgFeat PROTO((SeqFeatPtr pFeat));
static void RnaFeat PROTO((SeqFeatPtr pFeat));
static void ImportFeat PROTO((SeqFeatPtr pFeat));
static void NumFeat PROTO((SeqFeatPtr pFeat));
static void PsecFeat PROTO((SeqFeatPtr pFeat));
static void NonStdResFeat PROTO((SeqFeatPtr pFeat));
static void HetFeat PROTO((SeqFeatPtr pFeat));
static void SeqData PROTO((BioseqPtr pSeq, Boolean showSeq, Int2 charsPerLine));
static void AnyString PROTO((CharPtr key, CharPtr string));

/* ----- Function Bodies ----- */

SeqViewPtr FlatSeqEntryNew (SeqEntryPtr pSeq, Int2Ptr n)

{
  nItems = 0;
  pFlat = NULL;
  bMemErr = FALSE;
  buffer = MemNew (BUFSIZE);
  if (buffer != NULL) {
    pFlat = (SeqViewPtr) MemNew (CHUNK * sizeof(SeqView));
    if (pFlat != NULL) {
      DoSeqEntry (pSeq);
      if (bMemErr) {
        pFlat = MemFree (pFlat);
      }
    }
    if (n != NULL) {
      *n = nItems;
    }
  }
  buffer = MemFree (buffer);

  PostProcess();

  return pFlat;
}

SeqViewPtr FlatSeqEntryFree (SeqViewPtr pView)

{
  return MemFree (pView);
}

static Int2 AddBogusItem (Int2 nPos, CharPtr string)

{
  CharPtr  pStr;

	pStr = StringSave(string);
  if (pStr != NULL)
    return AddItem (nPos, OTHER, pStr);
  else 
    return -1;
}

static Int2 AddItem (Int2 nPos, Int2 nType, VoidPtr pDat)

{
  Int2        i, j, k;
  SeqViewPtr  pView;
  SeqViewPtr  pTemp;
  Int2        lBytes;

  if (bMemErr)
    return -1;

  if (nItems >= ITEM_MAX) {
    if (! bMemErr) {
      Beep ();
    }
    bMemErr = TRUE;
    return -1;
  }

  if (nPos < 0 || nPos > nItems) {
    k = nItems;
  } else {
    k = nPos;
  }

  for (i = nItems; i > k; i--) {
    pFlat [i] = pFlat [i - 1];
  }
  j = nItems;

  pView = pFlat + k;
  pView->nType = nType;
  pView->nHeight = 0;
  pView->pDat = pDat;

  j++;
  if ((j % CHUNK) == 0) {
    lBytes = (CHUNK + j) * sizeof (SeqView);
    pTemp = MemMore (pFlat, lBytes);
    if (pTemp == NULL) {
      bMemErr = TRUE;
      return -1;
    } else {
      MemSet ((VoidPtr) (pTemp+j), 0, CHUNK*sizeof(SeqView));
    }
    pFlat = pTemp;
  }
  if (bMemErr) {
    nItems = 0;
  } else {
    nItems = j;
  }
  return k;
}

static void MoveItem (int dst, int src)

{
    int  i, k;
    SeqViewPtr p1, p2;
    SeqView tmp;

    if ((k = dst-src) ==0)  return;

    p1 = pFlat + src;
    tmp = *p1;
    if (k > 0) {
        for (i=0, p2=p1+1; i<k; i++)
            *p1++ = *p2++;
    }
    else if (k < 0) {
        k = -k;
        for (i=0, p2=p1-1; i<k; i++)
            *p1-- = *p2--;
    }
    *p1 = tmp;
}


static void PostProcess (void)

{
    int  i, j, k, n;
    SeqViewPtr p, p1, p2;
    Boolean hascdr, innpset;

    /* 1. move Protein features up to just after the Title of the protein */
    for (p1=pFlat, i=0; i<nItems; i++, p1++) {
        if (p1->nType == FEAT_PROT) {
        	for (j=i-1, p2=p1-1; j>=0; j--, p2--) {
        	    if (p2->nType==RAWPROT) {
            		    MoveItem (j+1, i);
			    break;		
            	    }
		}
	}
    }

    /* 2. process each nuc-prot set */
    for (i=n=0, p=pFlat, innpset=FALSE; i<nItems; i++, p++) {
        switch (p->nType) {
          case SEGSET:
	  case PUBSET:
          case OTHERSET:
		  case PDBSET:
            if (innpset) {
              n++;
              break;
            }
          case NPSET:
            n = 1;
            k = i;
            innpset = TRUE;
            hascdr = FALSE;
            break;
          case ENDSET:
          	if (innpset) {
              n--;
              if (n==0) {
                  if (hascdr) {
                      FixNucProt (k, i);
                  }
                  innpset = FALSE;
              }
            }
            break;
          case FEAT_CDRGN:
            hascdr = TRUE;
            break;
        }
    }
}


static void FixNucProt (int k1, int k2)

{
    int  i, j, j0, n;
    SeqViewPtr p1, p2;
    ValNodePtr loc1, loc2;
    ValNodePtr id1;

    /* 1. move CdRgn features down to just before matching RAWPROT */
    i = k1+1;
    for (p1=pFlat+i; i<k2; i++, p1++) {
        if (p1->nType == FEAT_CDRGN) {
            loc1 = ((SeqFeatPtr)p1->pDat) ->product;
 			if (loc1 != NULL)
			{
	    if (loc1->choice == 3) {  /* whole */
		id1 = SeqIdFindBest (loc1->data.ptrvalue, SEQID_GI);
        	for (j=i+1, p2=p1+1; j<k2; j++, p2++) {
        	    if (p2->nType==RAWPROT) {
			if (SeqIdIn (id1, p2->pAux)) {
			    if (j ==i)  break;
            		    MoveItem (j, i);
            		    p1--;
            		    i--;
			    break;
		 	}			
            	    }
		}
            }
	    }
        }
    }

    /* 2.  move Gene features down to just above matching CdRgn */
    i = k1+1;
    for (p1=pFlat+i, j0=0; i<k2; i++, p1++) {
        if (p1->nType == FEAT_GENE) {
            loc1 = ((SeqFeatPtr)p1->pDat) ->location;
	    if (j0==0) j0=i+1;
	    p2 = &pFlat[j0];
            for (j=j0; j<k2; j++, p2++) {
                if (p2->nType==FEAT_CDRGN) {
                    loc2 =  ((SeqFeatPtr)p1->pDat) ->location;
	            n = SeqLocCompare (loc2, loc1);
	            if (n==SLC_A_EQ_B || n==SLC_A_IN_B) {
			/*if (j-1 == i) break;*/
            		MoveItem (j-1, i);
            	 	p1--;
            		i--;
			j0 = j+1;
			break;
		    }
                }
            }
        }
    }

}


static void DoSeqEntry (ValNodePtr pEntry)

{
  if (bMemErr)
    return;

  if (pEntry->choice == 1)
      DoBioseq (pEntry->data.ptrvalue);
  else if (pEntry->choice == 2)
      DoBioseqSet (pEntry->data.ptrvalue);
}


static void DoBioseq (BioseqPtr pSeq)

{
    Int2         k;
    SeqAnnotPtr  pAnno;
    ValNodePtr   pDesc;


    switch (pSeq->repr) {
      case Seq_repr_virtual:
      case Seq_repr_raw:
    	k = AddItem (-1, BEGENTRY, NULL);
        AddItem (-1, (Int2) (pSeq->mol==Seq_mol_aa ? RAWPROT : RAWNUCL), NULL);
        break;
      case Seq_repr_seg:
        k = kOrg;
        break;
      case Seq_repr_const:
    	k = AddItem (-1, BEGENTRY, NULL);
        AddItem (-1, CONSTSEQ, NULL);
        break;
      default:
        k = -2;
        break;
    }

    if (!bMemErr) {

        DoIdentifiers (k, pSeq->id, FALSE);

		pDesc = pSeq->descr;
        if (pDesc != NULL)
            DoDescription (k, pDesc);

		pAnno = pSeq->annot;
        if (pAnno != NULL)
            DoAnnotation (pAnno);

        DoSequence (pSeq);
    }
}

static void DoBioseqSet (BioseqSetPtr pSet)

{
    Int2         k;
    SeqAnnotPtr  pAnno;
    ValNodePtr   pDesc;
    ValNodePtr   pNode;

    switch (pSet->_class) {
      case 1: /* nucprot */
        k = AddItem (-1, BEGENTRY, NULL);
        AddItem (-1, NPSET, NULL);
        AddItem (-1, MEMBERS, pSet);
        break;
      case 2: /* segset */
        k = kOrg = AddItem (-1, BEGENTRY, NULL);
        AddItem (-1, SEGSET, NULL);
        break;
      case 4: /* parts */
        AddItem ((Int2) (kOrg+2), SEGMENTS, pSet->seq_set);
        k = -2;
        break;
      case 9: /* pubset */
        AddItem (-1, PUBSET, NULL);
        k= -2;
        break;
	  case 12:  /* pdb-entry */
        k = AddItem (-1, BEGENTRY, NULL);
        AddItem (-1, PDBSET, NULL);
        AddItem (-1, MEMBERS, pSet);
		break;	    
      default:
        AddItem (-1, OTHERSET, NULL);
        k = -2;
    }

    if (!bMemErr) {

		pDesc = pSet->descr;
        if (pDesc != NULL)
            DoDescription (k, pDesc);

		pAnno = pSet->annot;
        if (pAnno != NULL)
            DoAnnotation (pAnno);

        for (pNode = pSet->seq_set; pNode; pNode = pNode->next)
            DoSeqEntry (pNode);

        AddItem (-1, ENDSET, NULL);

	/*if (pSet->_class ==9)  inpubset = pubdone = FALSE;*/
    }
}

static Int2 AddIdentifierItem (Int2 nPos, Int2 nType, VoidPtr pDat, Boolean xref)

{
  if (xref) {
    return AddItem (nPos, XREF, pDat);
  } else {
    return AddItem (nPos, nType, pDat);
  }
}

static Int2 DoIdentifiers (Int2 nBase, ValNodePtr pIden, Boolean xref)

{
  Int2         k;
  size_t       i;
  ValNodePtr   pNode, giim=NULL, sq=NULL, mt=NULL;
  TextSeqIdPtr pTextId;
  PDBSeqIdPtr pPdbId;
  CharPtr      p;
  Boolean wrote_one = FALSE;
  GiimPtr giip;
  PatentSeqIdPtr pPatId;
  IdPatPtr pIdPat;
  Char temp [16];
  Char  last [80];

  last [0] = '\0';
  if (bMemErr) {
    return nBase;
  }
  k = nBase;
  if (k>=0) {
    pFlat[k].pAux = pIden;
    pFlat[k+1].pAux = pIden;
  }

  for (pNode = pIden; pNode != NULL; pNode = pNode->next) {

    if (xref) {
      nBase = nItems - 2;
    }

    switch (pNode->choice) {
		case SEQID_GIBBSQ:
			sq = pNode;
			break;
		case SEQID_GIBBMT:
			mt = pNode;
			break;
		case SEQID_GI:
		    if (xref) {
			  sprintf(buffer, "NCBI Seq ID: %ld", (long)(pNode->data.intvalue));
			} else {
			  sprintf(buffer, "Seq ID: %ld", (long)(pNode->data.intvalue));
			}
			if (StringCmp (last, buffer) != 0) {
			StringNCpy (last, buffer, sizeof (last));
	        p = (CharPtr) MemNew (StringLen (buffer) + 1);
    	    if (p != NULL) {
        	  StringCpy (p, buffer);
	           k = AddIdentifierItem ((Int2) (nBase + 2), NCBI, p, xref);
			   wrote_one = TRUE;
    	    }
    	    }
		case SEQID_GIIM:
			giim = pNode;
			break;
	}
  }

  if ((sq != NULL) || (mt != NULL))
  {
  	if (sq != NULL) {
  		if (xref) {
		  sprintf(buffer, "NCBI Journal Scan Seq ID: %ld", (long)(sq->data.intvalue));
		} else {
		  sprintf(buffer, "Journal Scan Seq ID: %ld", (long)(sq->data.intvalue));
		}
	} else {
		if (xref) {
		  sprintf(buffer, "NCBI Journal Scan Mol ID: %ld", (long)(mt->data.intvalue));
		} else {
		  sprintf(buffer, "Journal Scan Mol ID: %ld", (long)(mt->data.intvalue));
		}
	}
			if (StringCmp (last, buffer) != 0) {
			StringNCpy (last, buffer, sizeof (last));
	        p = (CharPtr) MemNew (StringLen (buffer) + 1);
    	    if (p != NULL) {
        	  StringCpy (p, buffer);

              if (xref) {
                nBase = nItems - 2;
               }

	           k = AddIdentifierItem ((Int2) (nBase + 2), NCBI, p, xref);
			   wrote_one = TRUE;
    	    }
    	    }
  }
			
  for (pNode = pIden; pNode != NULL; pNode = pNode->next) {

    if (xref) {
      nBase = nItems - 2;
    }

    switch (pNode->choice) {


      case SEQID_GENBANK:
	  case SEQID_EMBL:
	  case SEQID_DDBJ:
        buffer[0] = '\0';
        if (xref) {
          if (pNode->choice == SEQID_GENBANK) {
            StringCat (buffer, "GenBank  ");
          } else if (pNode->choice == SEQID_EMBL) {
            StringCat (buffer, "EMBL  ");
          } else if (pNode->choice == SEQID_DDBJ) {
            StringCat (buffer, "DDBJ  ");
          }
        }
        pTextId = (TextSeqIdPtr) pNode->data.ptrvalue;
		if (pTextId->accession == NULL)
			break;
			
        if (pTextId->name)  {
          StringCat (buffer, "Name:  ");
          StringCat (buffer, pTextId->name);
        }
        if (pTextId->accession) {
          if (buffer [0] != '\0' && pTextId->name) {
            StringCat (buffer, ",   ");
          }
          StringCat (buffer, "Accession:  ");
          StringCat (buffer, pTextId->accession);
		  if (*(pTextId->accession) == 'D')
		  	pNode->choice = SEQID_DDBJ;
        }
		if (StringCmp (last, buffer) != 0) {
		StringNCpy (last, buffer, sizeof (last));
        p = (CharPtr) MemNew (StringLen (buffer) + 1);
        if (p != NULL) {
          StringCpy (p, buffer);
		  if (pNode->choice == SEQID_GENBANK)
	          k = AddIdentifierItem ((Int2) (nBase + 2), GENBANK, p, xref);
		  if (pNode->choice == SEQID_EMBL)
	          k = AddIdentifierItem ((Int2) (nBase + 2), EMBL, p, xref);
		  if (pNode->choice == SEQID_DDBJ)
	          k = AddIdentifierItem ((Int2) (nBase + 2), DDBJ, p, xref);
		  wrote_one = TRUE;
        }
        }
        break;
      case SEQID_PIR:
        buffer[0] = '\0';
        if (xref) {
          StringCat (buffer, "PIR  ");
        }
        pTextId = (TextSeqIdPtr) pNode->data.ptrvalue;
        if (pTextId->name)  {
          StringCat (buffer, "Name:  ");
          StringCat (buffer, pTextId->name);
        }
        if (pTextId->accession) {
          if (buffer [0] != '\0' && pTextId->name) {
            StringCat (buffer, ",   ");
          }
          StringCat (buffer, "Accession:  ");
          StringCat (buffer, pTextId->accession);
        }
		if (StringCmp (last, buffer) != 0) {
		StringNCpy (last, buffer, sizeof (last));
        p = (CharPtr) MemNew (StringLen (buffer) + 1);
        if (p != NULL) {
          StringCpy (p, buffer);
           k = AddIdentifierItem ((Int2) (nBase + 2), PIR, p, xref);
		   wrote_one = TRUE;
        }
        }
        break;
      case SEQID_SWISSPROT:
        buffer[0] = '\0';
        if (xref) {
          StringCat (buffer, "SWISS-PROT  ");
        }
        pTextId = (TextSeqIdPtr) pNode->data.ptrvalue;
        if (pTextId->name)  {
          StringCat (buffer, "Name:  ");
          StringCat (buffer, pTextId->name);
        }
        if (pTextId->accession) {
          if (buffer [0] != '\0' && pTextId->name) {
            StringCat (buffer, ",   ");
          }
          StringCat (buffer, "Accession:  ");
          StringCat (buffer, pTextId->accession);
        }
		if (StringCmp (last, buffer) != 0) {
		StringNCpy (last, buffer, sizeof (last));
        p = (CharPtr) MemNew (StringLen (buffer) + 1);
        if (p != NULL) {
          StringCpy (p, buffer);
          k = AddIdentifierItem ((Int2) (nBase + 2), SWISSPROT, p, xref);
		   wrote_one = TRUE;
        }
        }
        break;
      case SEQID_PATENT:
        buffer[0] = '\0';
        if (xref) {
          StringCat (buffer, "PATENT  ");
        }
        pPatId = (PatentSeqIdPtr) pNode->data.ptrvalue;
        if (pPatId != NULL) {
          pIdPat = pPatId->cit;
          if (pIdPat != NULL) {
            if (pIdPat->country != NULL) {
              StringCat (buffer, pIdPat->country);
              StringCat (buffer, " ");
            }
            if (pIdPat->number != NULL) {
              StringCat (buffer, pIdPat->number);
            } else if (pIdPat->app_number != NULL) {
              StringCat (buffer, "(");
              StringCat (buffer, pIdPat->app_number);
              StringCat (buffer, ")");
            }
          }
          sprintf (temp, " [%d]", (int) (pPatId->seqid));
          StringCat (buffer, temp);
        }
		if (StringCmp (last, buffer) != 0) {
		StringNCpy (last, buffer, sizeof (last));
        p = (CharPtr) MemNew (StringLen (buffer) + 1);
        if (p != NULL) {
          StringCpy (p, buffer);
          k = AddIdentifierItem ((Int2) (nBase + 2), PATENT, p, xref);
		   wrote_one = TRUE;
        }
        }
        break;
      case SEQID_PRF:
        buffer[0] = '\0';
        if (xref) {
          StringCat (buffer, "PRF  ");
        }
        pTextId = (TextSeqIdPtr) pNode->data.ptrvalue;
        if (pTextId->name)  {
          StringCat (buffer, "Name:  ");
          StringCat (buffer, pTextId->name);
        }
        if (pTextId->accession) {
          if (buffer [0] != '\0' && pTextId->name) {
            StringCat (buffer, ",   ");
          }
          StringCat (buffer, "Accession:  ");
          StringCat (buffer, pTextId->accession);
        }
		if (StringCmp (last, buffer) != 0) {
		StringNCpy (last, buffer, sizeof (last));
        p = (CharPtr) MemNew (StringLen (buffer) + 1);
        if (p != NULL) {
          StringCpy (p, buffer);
          k = AddIdentifierItem ((Int2) (nBase + 2), PRF, p, xref);
		   wrote_one = TRUE;
        }
        }
		break;
      case SEQID_PDB:
        buffer[0] = '\0';
        if (xref) {
          StringCat (buffer, "PDB  ");
        }
        pPdbId = (PDBSeqIdPtr) pNode->data.ptrvalue;
        if (pPdbId->mol)  {
          StringCat (buffer, "Molecule:  ");
          StringCat (buffer, pPdbId->mol);
        }
        if (pPdbId->chain) {
          if (buffer [0] != '\0') {
            StringCat (buffer, ",   ");
          }
          StringCat (buffer, "Chain:  ");
		  if (pPdbId->chain == ' ')
			StringCat (buffer, "(space)");
		  else
  			{
				i = StringLen(buffer);
				buffer[i] = pPdbId->chain;
				buffer[i+1] = '\0';
			}
        }
		if (StringCmp (last, buffer) != 0) {
		StringNCpy (last, buffer, sizeof (last));
        p = (CharPtr) MemNew (StringLen (buffer) + 1);
        if (p != NULL) {
          StringCpy (p, buffer);
          k = AddIdentifierItem ((Int2) (nBase + 2), PDB, p, xref);
		   wrote_one = TRUE;
        }
        }
        break;
      default:
        break;
    }
  }

  if ((! wrote_one) && (giim != NULL))
  {
  	giip = (GiimPtr)(giim->data.ptrvalue);
  	if (xref) {
	  sprintf(buffer, "NCBI Import ID: %ld", (long)(giip->id));
	} else {
	  sprintf(buffer, "Import ID: %ld", (long)(giip->id));
	}
        p = (CharPtr) MemNew (StringLen (buffer) + 1);
        if (p != NULL) {
          StringCpy (p, buffer);

          if (xref) {
            nBase = nItems - 2;
          }

          k = AddIdentifierItem ((Int2) (nBase + 2), NCBI, p, xref);
        }
  }
  
  return k;
}

static Int2 DoDbtags (Int2 nBase, ValNodePtr pIden)

{
  Int2         k;
  CharPtr      p;
  DbtagPtr     pDb;
  ValNodePtr   pNode;
  ObjectIdPtr  pObj;
  Char         str [16];

  if (bMemErr) {
    return nBase;
  }
  k = nBase;
  if (k>=0) {
    pFlat[k].pAux = pIden;
    pFlat[k+1].pAux = pIden;
  }
  for (pNode = pIden; pNode != NULL; pNode = pNode->next) {

    nBase = nItems - 2;

    buffer [0] = '\0';
    pDb = (DbtagPtr) pNode->data.ptrvalue;
    if (pDb != NULL) {
      if (pDb->db != NULL) {
        StringCat (buffer, pDb->db);
      }
      pObj = pDb->tag;
      if (pObj != NULL) {
        if (pDb->db != NULL) {
          StringCat (buffer, "   ");
        }
        if (pObj->str != NULL) {
          StringCat (buffer, pObj->str);
        } else {
          sprintf (str, "%ld", (long) (pObj->id));
          StringCat (buffer, str);
        }
      }
      p = (CharPtr) MemNew (StringLen (buffer) + 1);
      StringCpy (p, buffer);
      k = AddItem ((Int2) (nBase + 2), XREF, p);
    }
  }
  return k;
}

static Int2 DoDescription (Int2 nBase, ValNodePtr pDesc)

{
  Int2         k;
  ValNodePtr   pNode;
  PubdescPtr   pPub;
  PdbBlockPtr  pdb;
  CharPtr	   line, tmp;
  size_t       len;
  PirBlockPtr  pPir;
  SPBlockPtr   pSP;

  if (bMemErr) {
    return nBase;
  }

  k = nBase;

  for (pNode = pDesc; pNode != NULL; pNode = pNode->next) {
    switch (pNode->choice) {
      case 3:     /* sequencing method */
        k = AddItem (-1, METHOD, StringForSeqMethod((Int2)pNode->data.intvalue));
        break;
      case 5:     /* title */
        if (nBase >= 0)
          pFlat[nBase+1].pDat = pNode->data.ptrvalue;
        break;
      case 6:     /* organism */
        k = AddItem (-1, ORGANISM, pNode->data.ptrvalue);
        break;
	  case 18:    /* create-date */
	  case 19:    /* release-date */
		k = AddItem (-1, RELDATE, pNode);
		break;
      case 7:     /* comment */
        k = DoComments (pNode->data.ptrvalue);
        break;
      case 10:    /* PIR block */
        pPir = (PirBlockPtr) pNode->data.ptrvalue;
        if (pPir != NULL && pPir->seqref != NULL) {
          k = DoIdentifiers (k, pPir->seqref, TRUE);
        }
        break;
      case 15:    /* SWISS-PROT block */
        pSP = (SPBlockPtr) pNode->data.ptrvalue;
        if (pSP != NULL && pSP->seqref != NULL) {
          k = DoIdentifiers (k, pSP->seqref, TRUE);
        }
        if (pSP != NULL && pSP->dbref != NULL) {
          k = DoDbtags (k, pSP->dbref);
        }
        break;
      case 12:    /* reference */
	pPub = (PubdescPtr) pNode->data.ptrvalue;
        /*if (! (inpubset && pubdone))*/
		k = AddItem (-1, REFERENCE, pPub);
	if (pPub->fig)
		k = AddItem (-1, PUBINFO, pPub);
	/*pubdone = TRUE;*/
        break;
      case 21:     /* PDB-block */
        if (nBase >= 0)
		{
			pdb = (PdbBlockPtr)pNode->data.ptrvalue;
			if (pdb->compound != NULL)
	          pFlat[nBase+1].pDat = pdb->compound->data.ptrvalue;
			len = 30;
			if (pdb->exp_method == NULL)
				len += 17;
			else
				len += (size_t)(StringLen(pdb->exp_method));

			if (pdb->pdbclass != NULL)
				len += (9 + (size_t)StringLen(pdb->pdbclass));
			if (pdb->source != NULL)
				len += (10 + (size_t)StringLen(pdb->source->data.ptrvalue));
				
			line = MemNew(len);
			tmp = line;
			tmp = StringMove(tmp, "3-D Structure determined by ");
			if (pdb->exp_method == NULL)
				tmp = StringMove(tmp, "X-ray diffraction");
			else
				tmp = StringMove(tmp, pdb->exp_method);

			if (pdb->pdbclass != NULL)
			{
				tmp = StringMove(tmp, "\rClass: ");
				tmp = StringMove(tmp, pdb->pdbclass);
			}
			if (pdb->source != NULL)
			{
				tmp = StringMove(tmp, "\rSource: ");
				tmp = StringMove(tmp, pdb->source->data.ptrvalue);
			}
			k = AddItem(-1, COMMENT, line);
		}
        break;
    }
  }
  return  k;
}

static Int2 DoComments (CharPtr pString)

{
  Int2     h, i, j, k;
  CharPtr  p, q;
  CharPtr  pStr;

  if (bMemErr) {
    return -2;
  }
  k = -2;
  j = StringLen (pString);
  pStr = (CharPtr) MemNew (j + 1);
  if (pStr == NULL) {
    bMemErr = TRUE;
  } else {
    p = pString;
    i = INT2_MAX;
    while (*p) {
      if (*p == '~') {
        p++;
        h = 0;
        while (*p && *p == ' ') {
          h++;
          p++;
        }
        if (h < i) {
          i = h;
        }
      } else {
        p++;
      }
    }
    if (i == INT2_MAX) {
      i = 0;
    }
    p = pString;
    q = pStr;
    while (*p) {
      if (*p == '~') {
        *q = '\r';
        p++;
        q++;
        h = 0;
        while (*p && *p == ' ' && h < i) {
          p++;
          h++;
        }
      } else {
        *q = *p;
        p++;
        q++;
      }
    }
    *q = '\0';
    k = AddItem (-1, COMMENT, pStr);
  }
  return  k;
}

static void DoAnnotation (SeqAnnotPtr p)

{
  SeqFeatPtr  pFeat;

  if (bMemErr) {
    return;
  }
  if (p->type ==1) {  /* ftable */
    for (pFeat = (SeqFeatPtr) p->data; pFeat != NULL; pFeat = pFeat->next) {
      switch (pFeat->data.choice) {
        case  1:
          AddItem (-1, FEAT_GENE, pFeat);
          break;
        case  2:
          AddItem (-1, FEAT_ORG, pFeat);
          break;
        case  3:
          AddItem (-1, FEAT_CDRGN, pFeat);
          break;
        case  4:
          AddItem (-1, FEAT_PROT, pFeat);
          break;
        case  5:
          AddItem (-1, FEAT_RNA, pFeat);
          break;
        case  6:
          AddItem (-1, FEAT_PUB, pFeat);
          break;
        case  7:
          AddItem (-1, FEAT_SEQ, pFeat);
          break;
        case  8:
          AddItem (-1, FEAT_IMP, pFeat);
          break;
        case  9:
          AddItem (-1, FEAT_REGION, pFeat);
          break;
        case  10:
          AddItem (-1, FEAT_COMMENT, pFeat);
          break;
        case  11:
          AddItem (-1, FEAT_BOND, pFeat);
          break;
        case  12:
          AddItem (-1, FEAT_SITE, pFeat);
          break;
        case  16:
          AddItem (-1, FEAT_NUM, pFeat);
          break;
        case  17:
          AddItem (-1, FEAT_PSEC, pFeat);
          break;
        case  18:
          AddItem (-1, FEAT_NSR, pFeat);
          break;
        case  19:
          AddItem (-1, FEAT_HET, pFeat);
          break;
        default:
          AddItem (-1, FEAT_OTHER, pFeat);
          break;
      }
    }
  } else if (p->type == 2) {
      AddBogusItem (-1, "Alignment...");
  } else if (p->type == 3) {
      AddBogusItem (-1, "Graph...");
  } else {
      AddBogusItem (-1, "Unknown annotation type ***");
  }
}

static void DoSequence (BioseqPtr pSeq)

{
  if (bMemErr) {
    return;
  }
  if (pSeq->repr == 1) {
    AddItem (-1, VIRTSEQ, NULL);
  } else if (pSeq->repr == 2) {   /* raw */
    AddItem (-1, SEQUENCE, pSeq);
  } else if (pSeq->repr == 4) {   /* constructed */
    AddItem (-1, SEQUENCE, pSeq);
  }
}

static void FormatIdent (ValNodePtr pIden, CharPtr buff)

{
  char  temp [32];
  ValNodePtr tmp, gbid = NULL, giimid = NULL, bestid = NULL,
			bbsid=NULL, bbmid=NULL;
  TextSeqIdPtr tsip;
  Boolean locus_only = FALSE;

  if (pIden) {
  	 for(tmp = pIden; tmp !=NULL; tmp = tmp->next) /* change DDBJ */
	 {
	 	switch (tmp->choice)
		{
			case SEQID_GENBANK:
			case SEQID_EMBL:
			case SEQID_DDBJ:
				tsip = (TextSeqIdPtr)(tmp->data.ptrvalue);
				if (tsip->accession != NULL)
				{
					if (* (tsip->accession) == 'D')
						tmp->choice = SEQID_DDBJ;
					gbid = tmp;
					locus_only = FALSE;
				}
				else
					locus_only = TRUE;
				break;
			case SEQID_GI:
				bestid = tmp;
				break;
			case SEQID_GIIM:
				giimid = tmp;
				break;
			case SEQID_GIBBMT:
				bbmid =tmp;
				break;
			case SEQID_GIBBSQ:
				bbsid=tmp;
				break;
				
		}
	 }

	 if (CheckForm)       /* Backbone checking form */
	{
		if (bbsid != NULL)
			bestid = bbsid;
		else if (bbmid)
			bestid = bbmid;
	}
	 if (bestid == NULL)
	 {
	 	if (gbid != NULL)
			bestid = gbid;
		else if ((locus_only) && (giimid != NULL))
			bestid = giimid;
		else
			bestid = SeqIdFindBest(pIden, SEQID_GI);
	 }

     SeqIdPrint (bestid, temp, PRINTID_REPORT);
     StringCpy (buff, temp);
  }
  else
     *buff = '\0';
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

static void AddChar (char ch)

{
	*pos++ = ch;
	*pos = '\0';
}

static void FeatureText (SeqFeatPtr pFeat, CharPtr key)

{
  Int2       i;
  GBQualPtr  pQual;

  AddString (key);
  AddString ("\t");
  for (pQual = (GBQualPtr) pFeat->qual, i = 0; pQual != NULL; pQual = pQual->next, i++) {
    if (i != 0) {
      AddString ("\r");
    }
    AddString (pQual->qual);
    AddString (": ");
    AddString (pQual->val);
    AddString (".");
  }
  if (pFeat->excpt) {
    AddString ("\r");
    AddString ("* Exception");
  }
  if (pFeat->partial) {
    AddString ("\r");
    AddString ("* Partial");
  }
  if (pFeat->comment != NULL) {
    AddString ("\r");
    AddString (pFeat->comment);
  }
  if (pFeat->exp_ev == 1)
  {
  	AddString("\r");
	AddString("(experimentally determined)");
  }
}

static void Intervals (ValNodePtr pNode)

{
  Int2        i, retval;
  ValNodePtr  pIden;
  ValNodePtr  pIntv;
  ValNodePtr  tnode;
  SeqIntPtr   pLoc;
  SeqBondPtr  pLocb;	/* pointer to bond loc */
  SeqPntPtr   pLocp;	/* pointer to pnt loc */
  char        str [32];
  DataVal av;
  Int4 from = -1, to = -1;

  str[0] = '\0';

  for (pIntv = pNode; pIntv != NULL; pIntv = pIntv->next) {
    switch (pIntv->choice) {
		case 1:    /* null */
		case 2:    /* empty */
			AddString(" [ Gap ]");
			break;
      case 3:      /* whole */
	  		pIden = (SeqIdPtr) pIntv->data.ptrvalue;
        if (pIden != NULL && showIds) {
          FormatIdent (pIden, str);
          AddString (str);
          AddString (":  [ Whole ]");
        }
        break;
      case 4:
        pLoc = (SeqIntPtr) pIntv->data.ptrvalue;
        pIden = pLoc->id;
        if (pIden != NULL && showIds) {
          FormatIdent (pIden, str);
          AddString (str);
          AddString (":  ");
        }

		if (pLoc->strand == Seq_strand_minus)
		{
			AddString("c");
			to = pLoc->from;
			from = pLoc->to;
		}
		else
		{
			from = pLoc->from;
			to = pLoc->to;
		}
		retval = NumberingValueBySeqId(pIden, from, &av);
		if (retval == 1)  /* integer only for now */
			sprintf(str, "%ld", av.intvalue);
		else if (retval == 3)  /* enumerated */
		{
			if (* (CharPtr)av.ptrvalue == '\0')
				sprintf(str, "[%ld]", from);
			else
				StringMove(str, (CharPtr)av.ptrvalue);
		}
		else
			StringMove(str, "?");
		AddString(str);
		AddString("..");
		retval = NumberingValueBySeqId(pIden, to, &av);
		if (retval == 1)  /* integer only for now */
			sprintf(str, "%ld", av.intvalue);
		else if (retval == 3)  /* enumerated */
		{
			if (* (CharPtr)av.ptrvalue == '\0')
				sprintf(str, "[%ld]", to);
			else
				StringMove(str, (CharPtr)av.ptrvalue);
		}
		else
			StringMove(str, "?");
        AddString (str);
        break;
      case 5:
        for (tnode = (SeqLocPtr) pIntv->data.ptrvalue, i = 0; tnode != NULL; tnode = tnode->next, i++) {
          if (i != 0) {
            AddString ("\r");
          }
          pLoc = (SeqIntPtr) tnode->data.ptrvalue;
          pIden = pLoc->id;
          if (pIden != NULL && showIds) {
            FormatIdent (pIden, str);
            AddString (str);
            AddString (":  ");
          }
		if (pLoc->strand == Seq_strand_minus)
		{
			AddString("c");
			to = pLoc->from;
			from = pLoc->to;
		}
		else
		{
			from = pLoc->from;
			to = pLoc->to;
		}
		retval = NumberingValueBySeqId(pIden, from, &av);
		if (retval == 1)  /* integer only for now */
			sprintf(str, "%ld", av.intvalue);
		else if (retval == 3)  /* enumerated */
		{
			if (* (CharPtr)av.ptrvalue == '\0')
				sprintf(str, "[%ld]", from);
			else
				StringMove(str, (CharPtr)av.ptrvalue);
		}
		else
			StringMove(str, "?");
		AddString(str);
		AddString("..");
		retval = NumberingValueBySeqId(pIden, to, &av);
		if (retval == 1)  /* integer only for now */
			sprintf(str, "%ld", av.intvalue);
		else if (retval == 3)  /* enumerated */
		{
			if (* (CharPtr)av.ptrvalue == '\0')
				sprintf(str, "[%ld]", to);
			else
				StringMove(str, (CharPtr)av.ptrvalue);
		}
		else
			StringMove(str, "?");
        AddString (str);


        }
        break;
      case 6:	/* location is a point */
        pLocp = (SeqPntPtr) pIntv->data.ptrvalue;
        pIden = pLocp->id;
        if (pIden != NULL && showIds) {
          FormatIdent (pIden, str);
          AddString (str);
          AddString (":  ");
        }
		if (pLocp->strand == Seq_strand_minus)
			AddString("c");

		retval = NumberingValueBySeqId(pIden, pLocp->point, &av);
		if (retval == 1)  /* integer only for now */
			sprintf(str, "%ld", av.intvalue);
		else if (retval == 3)  /* enumerated */
		{
			if (* (CharPtr)av.ptrvalue == '\0')
				sprintf(str, "[%ld]", pLocp->point);
			else	
				StringMove(str, (CharPtr)av.ptrvalue);
		}
		else
			StringMove(str, "?");
		AddString(str);

        break;
      case 8:
      case 9:
        Intervals(pIntv->data.ptrvalue);
        break;
      case 10:	/* location is a bond */
        pLocb = (SeqBondPtr) pIntv->data.ptrvalue;
        if ((pLocp = pLocb->a) != NULL) {
	      pIden = pLocp->id;
	      if (pIden != NULL && showIds) {
	        FormatIdent (pIden, str);
	        AddString (str);
	        AddString (":  ");
	      }
		if (pLocp->strand == Seq_strand_minus)
			AddString("c");

		retval = NumberingValueBySeqId(pIden, pLocp->point, &av);
		if (retval == 1)  /* integer only for now */
			sprintf(str, "%ld", av.intvalue);
		else if (retval == 3)  /* enumerated */
		{
			if (* (CharPtr)av.ptrvalue == '\0')
				sprintf(str, "[%ld]", pLocp->point);
			else
				StringMove(str, (CharPtr)av.ptrvalue);
		}
		else
			StringMove(str, "?");

        } else {
			StringMove(str, "?");
	    }
	    AddString (str);
		AddString (" bond ");

        if ((pLocp = pLocb->b) != NULL) {
	      if (pLocp->id != NULL && showIds && (! SeqIdMatch(pIden, pLocp->id))) {
	        FormatIdent (pLocp->id, str);
	        AddString (str);
	        AddString (":  ");
	      }
		  pIden = pLocp->id;
		if (pLocp->strand == Seq_strand_minus)
			AddString("c");

		retval = NumberingValueBySeqId(pIden, pLocp->point, &av);
		if (retval == 1)  /* integer only for now */
			sprintf(str, "%ld", av.intvalue);
		else if (retval == 3)  /* enumerated */
		{
			if (* (CharPtr)av.ptrvalue == '\0')
				sprintf(str, "[%ld]", pLocp->point);
			else
				StringMove(str, (CharPtr)av.ptrvalue);
		}
		else
			StringMove(str, "?");
		AddString(str);

	    }

        break;
      default:
        AddString ("?");
        break;
    }
    if (pIntv->next != NULL) {
      AddString ("\r");
    }
  }
}

static void FeatString (CharPtr key, CharPtr string, SeqFeatPtr pFeat)

{
	Boolean needsReturn = FALSE;

   if (StringCmp(key, "Region") == 0) {
      if (pFeat->data.value.ptrvalue != NULL)
	 AddString (pFeat->data.value.ptrvalue);
      else
	 AddString (key);
   }
   else if (StringCmp(key, "Bond") == 0) {
	AddString(AsnEnumStr("SeqFeatData.bond", (Int2)pFeat->data.value.intvalue));
	AddString(" bond");
	/****************************** REMOVE ***
	switch (pFeat->data.value.intvalue) {
       case 1:
	 AddString("Disulfide bond");
	 break;
       case 2:
	 AddString("Thiolester bond");
	 break;
       case 3:
	 AddString("Xlink bond");
	 break;
       case 4:
	 AddString("Thioether bond");
	 break;
       case 255:
	 AddString("Other bond");
	 break;
       default:
	 AddString("Other bond");
	 break;
      }
	******************************** REMOVE ***/
   }
   else if (StringICmp(key, "Site") == 0) {
	AddString(AsnEnumStr("SeqFeatData.site", (Int2)pFeat->data.value.intvalue));
	AddString(" site");
	/************** REMOVE **********************************
      switch (pFeat->data.value.intvalue) {
       case 1:
	 AddString("Activity site");
	 break;
       case 2:
	 AddString("Binding site");
	 break;
       case 3:
	 AddString("Cleavage site");
	 break;
       case 4:
	 AddString("Inhibit site");
	 break;
       case 5:
	 AddString("Modified site");
	 break;
       case 6:
	 AddString("Glycosylation site");
	 break;
       case 7:
	 AddString("Myristoylation site");
	 break;
       case 8:
	 AddString("Mutagenized site");
	 break;
       case 9:
	 AddString("Metal-binding site");
	 break;
       case 255:
	 AddString("Other site");
	 break;
       default:
	 AddString("Other site");
	 break;
	}
	*************************REMOVE *******************/
  }
  else
     AddString (key);

  AddString ("\t");
  AddString (string);
  if (string != NULL)
	needsReturn = TRUE;

  if (pFeat->excpt) {
	if (needsReturn)
	    AddString ("\r");
    AddString ("* Exception");
	needsReturn = TRUE;
  }
  if (pFeat->partial) {
	if (needsReturn)
	    AddString ("\r");
    AddString ("* Partial");
	needsReturn = TRUE;
  }
  if (pFeat->exp_ev == 1)
  {
	if (needsReturn)
	  	AddString("\r");
	AddString("(experimentally determined)");
  }

  AddString ("\t");
  Intervals (pFeat->location);
  AddString ("\n");
  if (showIds) {
    success = report (instnce, 6, buffer);
  } else {
    success = report (instnce, 5, buffer);
  }
}

static void AnyString (CharPtr key, CharPtr string)

{
  AddString (key);
  AddString ("\t");
  AddString (string);
  AddString ("\n");
  success = report (instnce, 2, buffer);
}

static void FormatMember (BioseqPtr pSeq)

{
  char        ident [32];
  ValNodePtr  pIden;
  ValNodePtr  pNode;

  ident [0] = '\0';
  pIden = pSeq->id;
  if (pIden != NULL)
      FormatIdent (pIden, ident);
  AddString (ident);
  AddString ("\t");
  if (pSeq->repr == 1) {  /* virtual */
     AddString(sUntitled);
     AddString("  -  ");
     AddString(sVirtual);
     AddString("\n");
  }
  else {
    for (pNode = pSeq->descr; pNode; pNode = pNode->next) {
      if (pNode->choice == 5) {
        AddString (pNode->data.ptrvalue);
        AddString ("\n");
        break;
      }
    }
    if (pNode==NULL) {
       AddString (sUntitled);
       AddString ("\n");
    }
  }
}

static void FormatAuth (AuthListPtr pAuth)

{
  Int2         i;
  NameStdPtr   pName;
  ValNodePtr   pNode;
  PersonIdPtr  pPerson;
  AuthorPtr    pSingle;

  if (pAuth->choice == 3 || pAuth->choice == 2) {
    for (pNode = pAuth->names, i = 0; pNode != NULL; pNode = pNode->next, i++) {
      if (i != 0) {
        if (pNode->next != NULL) {
          AddString (", ");
        } else {
          AddString (" & ");
        }
      }
      AddString (pNode->data.ptrvalue);
    }
  } else if (pAuth->choice == 1) {
    for (pNode = pAuth->names, i = 0; pNode != NULL; pNode = pNode->next, i++) {
      if (i != 0) {
        if (pNode->next != NULL) {
          AddString (", ");
        } else {
          AddString (" & ");
        }
      }
      pSingle = pNode->data.ptrvalue;
      if (pSingle != NULL) {
        pPerson = pSingle->name;
        if (pPerson != NULL) {
          if (pPerson->choice == 2) {
            pName = (NameStdPtr) (pPerson->data);
            if (pName != NULL) {
              if (pName->names [3] != NULL) {
                AddString (pName->names [3]);
              } else if (pName->names [1] != NULL && pName->names [2] != NULL && pName->names [0] != NULL) {
                AddString (pName->names [1]);
                AddString (" ");
                AddString (pName->names [2]);
                AddString (" ");
                AddString (pName->names [0]);
              } else if (pName->names [1] != NULL && pName->names [0] != NULL) {
                AddString (pName->names [1]);
                AddString (" ");
                AddString (pName->names [0]);
              } else if (pName->names [4] != NULL && pName->names [0] != NULL) {
                AddString (pName->names [4]);
                AddString (" ");
                AddString (pName->names [0]);
              } else if (pName->names [0] != NULL) {
                AddString (pName->names [0]);
              } else {
                AddString ("?");
              }
            } else {
              AddString ("?");
            }
          } else if (pPerson->choice == 3 || pPerson->choice == 4) {
            AddString (pPerson->data);
          }
        } else {
          AddString ("?");
        }
      } else {
        AddString ("?");
      }
    }
  }
}

static void BeginEntry (SeqViewPtr pView)

{
  int i;
  char  buf [16];

  if (pView->pAux) {
	FormatIdent (pView->pAux,  buf);
	AddString (buf);
	AddChar ('\t');
	for (i=0; i<51; i++) AddChar ('-');
  }
  else {
	AddString ("Group\t");
	for (i=0; i<51; i++) AddChar ('=');
  }
  AddString ("\n");
  success = report (instnce, 10, buffer);
}

static void Definition (SeqViewPtr pView)

{
  AddString ("Definition");
  AddString ("\t");
  if (pView->pDat)
	AddString ((CharPtr) pView->pDat);
  else
	AddString (sUntitled);
  AddString ("\n");
  success = report (instnce, 1, buffer);
}

static void Contents (BioseqSetPtr pSet)

{
  Int2        count;
  ValNodePtr  pNode;
  BioseqPtr   pSeq;

  AddString ("Contents");
  count = 0;
  for (pNode = pSet->seq_set; pNode != NULL; pNode = pNode->next) {
    if (pNode->choice == 1) {
      pSeq = (BioseqPtr) pNode->data.ptrvalue;
      count++;
      AddString ("\t");
      FormatMember (pSeq);
    } else if (pNode->choice == 2) {
      pSet = (BioseqSetPtr) pNode->data.ptrvalue;
      if (pSet->_class == 2) {
        pSeq = (BioseqPtr) pSet->seq_set->data.ptrvalue;
        count++;
        AddString ("\t");
        FormatMember (pSeq);
      }
    }
  }
  if (count == 0) {
    AddString ("\n");
  }
  success = report (instnce, 4, buffer);
}

static void Segments (ValNodePtr pSegs)

{
  Int2        count;
  ValNodePtr  pNode;

  AddString ("Segments");
  for (pNode = pSegs, count=0; pNode; pNode = pNode->next) {
    if (pNode->choice == 1) {
      count++;
      AddString ("\t");
      FormatMember ((BioseqPtr) pNode->data.ptrvalue);
    }
  }
  /*if (count == 0) {*/
    AddString ("\n");
  /*}*/
  success = report (instnce, 4, buffer);
}

static void PubInformation (PubdescPtr pPub)

{
  char temp[64];
  ValNodePtr pNode;
  NumContPtr pNum;

  AddString ("Figure");
  AddString ("\t");

#ifdef WIN_MSWIN
  sprintf (temp, "%Fs,  \"%Fs\"", 
	pPub->fig ? pPub->fig : "(no fig)", 
	pPub->name ? pPub->name : "(no name)");
#else
  sprintf (temp, "%s,  \"%s\"", 
	pPub->fig ? pPub->fig : "(no fig)", 
	pPub->name ? pPub->name : "(no name)");
#endif
  AddString (temp);
  if (((pNode = pPub->num) != NULL) && pNode->choice==1) {
	AddString("\r");
	pNum = (NumContPtr) pNode->data.ptrvalue;
	sprintf (temp, "Numbered from %ld", (long) pNum->refnum);
	if (pNum->ascending==FALSE)  StrCat (temp, ", descending");
	if (pNum->has_zero)  StrCat (temp, ", there is a zero position!");
	AddString(temp);
  }
  if (pPub->numexc)
	AddString("\rNumbering exception in figure");
  if (pPub->poly_a)
	AddString("\rPoly-A shown at end of sequence in figure");
  if (pPub->align_group)
  {
	AddString("\rShown in alignment group ");
	sprintf(temp, "%d", (int)pPub->align_group);
	AddString(temp);
  }  
  if (pPub->maploc != NULL)
	{
		AddString("\r");
#ifdef WIN_MSWIN
		sprintf(temp, "Map location: %Fs", pPub->maploc);
#else
		sprintf(temp, "Map location: %s", pPub->maploc);
#endif
		AddString(temp);
	}

  AddString ("\n");
  success = report (instnce, 2, buffer);
}


static void Reference (PubdescPtr pPub, ValNodePtr pIvls)

{
  Char             ch;
  CharPtr          chptr;
  Int2             len;
  char             medline [32];
  CitArtPtr        pArt;
  AuthListPtr      pAuth;
  CitPatPtr        pCitPat;
  DatePtr          pDat;
  CitGenPtr        pGen;
  IdPatPtr         pIdPat;
  ImprintPtr       pImp=NULL;
  CitJourPtr       pJrn=NULL;
  MedlineEntryPtr  pMed;
  ValNodePtr       pName;
  ValNodePtr       pNode;
  CharPtr          ptr, psav;
  char             temp [32];
  CitBookPtr       pBook=NULL;
  AffilPtr         pAffil;
  CitSubPtr		   pSub=NULL;
  Boolean	gotone;

  AddString ("Citation\t");
  psav = pos;

  if (pIvls) {
    AddString ("\t");
    Intervals (pIvls);
    AddString ("\n");
    if (showIds) {
      success = report (instnce, 6, buffer);
    } else {
      success = report (instnce, 5, buffer);
    }
    ClearString ();
    AddString ("\t");
    psav = pos;
  } 
  medline [0] = '\0';
  pMed = NULL;
  for (pNode = pPub->pub; pNode != NULL; pNode = pNode->next) {
    if (pNode->choice == 4) {
      sprintf (medline, "MEDLINE identifier:  %ld", pNode->data.intvalue);
    }
  }
  for (pNode = pPub->pub; pNode != NULL; pNode = pNode->next) {
    if (pos != psav)  AddString ("\r\r");
    switch (pNode->choice) {
      case 1:
        pGen = (CitGenPtr) pNode->data.ptrvalue;
        pDat = pGen->date;
		gotone = FALSE;
        if (pGen->authors)
		{
          FormatAuth (pGen->authors);
		  gotone = TRUE;
		}
        if (pDat != NULL) {
			gotone = TRUE;
          switch (pDat->data [0]) {
            case 0:
              StringCpy (temp, " (");
              StringCat (temp, pDat->str);
              chptr = StringChr (temp + 1, ' ');
              if (chptr != NULL) {
                *chptr = '\0';
              }
              StringCat (temp, ")");
              break;
            case 1:
              sprintf (temp, " (%d)", 1900 + pDat->data [1]);
              break;
            default:
              break;
          }
          AddString (temp);
        }
		if (gotone)
	        AddString (".  ");
		if (pGen->title)
		{
			AddString(pGen->title);
			AddString(". ");
		}
        if (pGen->serial_number >= 0)
        {
            sprintf(temp, "REF [%d]  ", pGen->serial_number);
            AddString(temp);
        }
        if (pGen->cit)
            AddString (pGen->cit);
        break;
      case 2:
        pSub = (CitSubPtr) pNode->data.ptrvalue;
		pImp = pSub->imp;
        pDat = pImp->date;
		AddString("Data Submission: ");
        if (pSub->authors)
          FormatAuth (pSub->authors);
        if (pDat != NULL) {
          switch (pDat->data [0]) {
            case 0:
              StringCpy (temp, " (");
              StringCat (temp, pDat->str);
              chptr = StringChr (temp + 1, ' ');
              if (chptr != NULL) {
                *chptr = '\0';
              }
              StringCat (temp, ")");
              break;
            case 1:
              sprintf (temp, " (%d)", 1900 + pDat->data [1]);
              break;
            default:
              break;
          }
          AddString (temp);
          AddString (".  ");
        }
		
        break;
      case 3:
        pMed = (MedlineEntryPtr) pNode->data.ptrvalue;
        pArt = (CitArtPtr) pMed->cit;
        if (pArt->from == 1) {
          pJrn = (CitJourPtr) pArt->fromptr;
          pImp = (ImprintPtr) pJrn->imp;
          pDat = pImp->date;
        } else {
		  pBook = (CitBookPtr) pArt->fromptr;
          pImp = pBook->imp;
          pDat = pImp->date;
        }
        pAuth = pArt->authors;
        if (pAuth != NULL && (pAuth->choice == 2 || pAuth->choice == 3)) {
          pName = pAuth->names;
          if (pName != NULL) {
            AddString (pName->data.ptrvalue);
            pName = pName->next;
          }
          while (pName != NULL) {
            if (pName->next != NULL) {
              AddString (", ");
              AddString (pName->data.ptrvalue);
            } else {
              AddString (" & ");
              AddString (pName->data.ptrvalue);
            }
            pName = pName->next;
          }
        }
        if (pDat != NULL) {
          switch (pDat->data [0]) {
            case 0:
              StringCpy (temp, " (");
              StringCat (temp, pDat->str);
              chptr = StringChr (temp + 1, ' ');
              if (chptr != NULL) {
                *chptr = '\0';
              }
              StringCat (temp, ")");
              break;
            case 1:
              sprintf (temp, " (%d)", 1900 + pDat->data [1]);
              break;
            default:
              break;
          }
          AddString (temp);
        }
		if (pArt->title != NULL)
		{
	        AddString (".  ");
    	    AddString (pArt->title->data.ptrvalue);
        	ptr = (CharPtr) pArt->title->data.ptrvalue;
	        if (ptr != NULL && ptr [0] != '\0') {
    	      len = StringLen (buffer);
        	  if (len > 0 && buffer [len - 1] != '.') {
            	AddString (".");
	          }
	          AddString ("  ");
    	    }
		}
        if (pJrn != NULL) {
          AddString (pJrn->title->data.ptrvalue);
          AddString (" ");
          AddString (pImp->volume);
          AddString (", ");
          AddString (pImp->pages);
          AddString (".  ");
        }
        if (pMed->abstract != NULL) {
          if (medline[0]) {
            ch = *(pos - 1);
            if (ch != ' ' && ch != '\t' && ch != '\r') {
              AddString ("  ");
            }
            AddString(medline);
            medline[0] = '\0';
          }
          AddString ("\r\rAbstract:  ");
          AddString (pMed->abstract);
        }
        break;
      case 4:
        /*
        sprintf (medline, "MEDLINE identifier:  %ld", pNode->data.intvalue);
        AddString (medline);
        */
        break;
      case 5:    /* Cit-Art */
        pArt = (CitArtPtr) pNode->data.ptrvalue;
        if (pArt->from == 1) {
          pJrn = (CitJourPtr) pArt->fromptr;
          pImp = (ImprintPtr) pJrn->imp;
          pDat = pImp->date;
        } else {
		  pBook = (CitBookPtr) pArt->fromptr;
          pImp = pBook->imp;
          pDat = pImp->date;
        }
		if (pArt->authors != NULL)
	        FormatAuth (pArt->authors);
        if (pDat != NULL) {
          switch (pDat->data [0]) {
            case 0:
              StringCpy (temp, " (");
              StringCat (temp, pDat->str);
              chptr = StringChr (temp + 1, ' ');
              if (chptr != NULL) {
                *chptr = '\0';
              }
              StringCat (temp, ")");
              break;
            case 1:
              sprintf (temp, " (%d)", 1900 + pDat->data [1]);
              break;
            default:
              break;
          }
          AddString (temp);
        }
		if (pArt->title != NULL)
		{
	        AddString (".  ");
    	    AddString (pArt->title->data.ptrvalue);
        	ptr = (CharPtr) pArt->title->data.ptrvalue;
	        if (ptr != NULL && ptr [0] != '\0') {
    	      len = StringLen (buffer);
        	  if (len > 0 && buffer [len - 1] != '.') {
            	AddString (".");
	          }
    	      AddString ("  ");
        	}
		}
        if (pJrn != NULL) {
          AddString (pJrn->title->data.ptrvalue);
          AddString (" ");
          AddString (pImp->volume);
          AddString (", ");
          AddString (pImp->pages);
          AddString (".  ");
        }
		if (pBook != NULL) {
		    if (pArt->title != NULL)
			    AddString("(in) ");
			else
				AddString(" ");
			AddString(pBook->title->data.ptrvalue);
			if (pBook->authors != NULL)
			{
				AddString("; ");
				FormatAuth(pBook->authors);
			}
			if (pImp != NULL)
			{
				if (pImp->pub != NULL)
				{
					AddString("; ");
					pAffil = pImp->pub;
					if (pAffil->choice == 1)  /* str */
						AddString(pAffil->affil);
					else
					{
						AddString(pAffil->affil);
						if (pAffil->div != NULL)
						{
							AddString(", ");
							AddString(pAffil->div);
						}
						if (pAffil->street != NULL)
						{
							AddString(", ");
							AddString(pAffil->street);
						}
						if (pAffil->city != NULL)
						{
							AddString(", ");
							AddString(pAffil->city);
						}
						if (pAffil->sub != NULL)
						{
							AddString(", ");
							AddString(pAffil->sub);
						}
						if (pAffil->country != NULL)
						{
							AddString(", ");
							AddString(pAffil->country);
						}
					}
				}
				if (pImp->pages != NULL)
				{
					AddString("; ");
					AddString(pImp->pages);
				}
			}
			AddString(".");
		}
		if (pImp != NULL)
		{
			if (pImp->prepub == 1)
				AddString(" (Submitted)");
			else if (pImp->prepub == 2)
				AddString(" (In Press)");
		}
        break;
      case 7:    /* Cit-book */
        pBook = (CitBookPtr) pNode->data.ptrvalue;
        pImp = pBook->imp;
        pDat = pImp->date;
			AddString(pBook->title->data.ptrvalue);
        if (pDat != NULL) {
          switch (pDat->data [0]) {
            case 0:
              StringCpy (temp, " (");
              StringCat (temp, pDat->str);
              chptr = StringChr (temp + 1, ' ');
              if (chptr != NULL) {
                *chptr = '\0';
              }
              StringCat (temp, ")");
              break;
            case 1:
              sprintf (temp, " (%d)", 1900 + pDat->data [1]);
              break;
            default:
              break;
          }
          AddString (temp);
        }
			if (pBook->authors != NULL)
			{
				AddString("; ");
				FormatAuth(pBook->authors);
			}
			if (pImp != NULL)
			{
				if (pImp->pub != NULL)
				{
					AddString("; ");
					pAffil = pImp->pub;
					if (pAffil->choice == 1)  /* str */
						AddString(pAffil->affil);
					else
					{
						AddString(pAffil->affil);
						if (pAffil->div != NULL)
						{
							AddString(", ");
							AddString(pAffil->div);
						}
						if (pAffil->street != NULL)
						{
							AddString(", ");
							AddString(pAffil->street);
						}
						if (pAffil->city != NULL)
						{
							AddString(", ");
							AddString(pAffil->city);
						}
						if (pAffil->sub != NULL)
						{
							AddString(", ");
							AddString(pAffil->sub);
						}
						if (pAffil->country != NULL)
						{
							AddString(", ");
							AddString(pAffil->country);
						}
					}
				}
				if (pImp->pages != NULL)
				{
					AddString("; ");
					AddString(pImp->pages);
				}
			}
			AddString(".");
        break;
      case 9:    /* Cit-pat */
        pCitPat = (CitPatPtr) pNode->data.ptrvalue;
        if (pCitPat != NULL) {
          AddString ("Patent: ");
          if (pCitPat->country != NULL) {
            AddString (pCitPat->country);
            AddString (" ");
          }
          if (pCitPat->number != NULL) {
            AddString (pCitPat->number);
          } else if (pCitPat->app_number != NULL) {
            AddString ("(");
            AddString (pCitPat->app_number);
            AddString (")");
          }
          if (pCitPat->doc_type != NULL && *(pCitPat->doc_type) != '\0') {
            AddString ("-");
            AddString (pCitPat->doc_type);
          }
          AddString ("  ");
          if (pCitPat->date_issue != NULL) {
            DatePrint (pCitPat->date_issue, temp);
            AddString (temp);
          } else if (pCitPat->app_date != NULL) {
            DatePrint (pCitPat->app_date, temp);
            AddString (temp);
          }
          AddString ("\r");
          if (pCitPat->authors != NULL) {
            FormatAuth (pCitPat->authors);
            len = StringLen (buffer);
            if (len > 0 && buffer [len - 1] != '.') {
              AddString (".");
            }
			AddString("  ");
          }
          if (pCitPat->title != NULL) {
            AddString (pCitPat->title);
            len = StringLen (buffer);
            if (len > 0) {
              if (buffer [len - 1] == '.') {
              } else if (buffer [len - 1] == '"' && buffer [len - 2] == '.') {
              } else {
                AddString (".");
              }
            }
			AddString("  ");
          }
          pAuth = pCitPat->authors;
          if (pAuth != NULL) {
            pAffil = pAuth->affil;
            if (pAffil != NULL) {
              if (pAffil->choice == 1) {
                AddString (pAffil->affil);
              } else if (pAffil->choice == 2) {
                AddString (pAffil->affil);
                if (pAffil->street) {
                  AddString ("; ");
                  AddString (pAffil->street);
                }
                if (pAffil->div) {
                  AddString ("; ");
                  AddString (pAffil->div);
                }
                if (pAffil->city) {
                  AddString ("; ");
                  AddString (pAffil->city);
                }
                if (pAffil->sub) {
                  AddString ("; ");
                  AddString (pAffil->sub);
                }
                if (pAffil->country) {
                  AddString ("; ");
                  AddString (pAffil->country);
                }
              }
            }
          }
        }
        break;
      case 10:    /* Id-pat */
        AddString ("Patent: ");
        pIdPat = (IdPatPtr) pNode->data.ptrvalue;
        if (pIdPat != NULL) {
          if (pIdPat->country != NULL) {
            AddString (pIdPat->country);
            AddString (" ");
          }
          if (pIdPat->number != NULL) {
            AddString (pIdPat->number);
          } else if (pIdPat->app_number != NULL) {
            AddString ("(");
            AddString (pIdPat->app_number);
            AddString (")");
          } else {
            AddString ("?");
          }
        }
        break;
      default:
        sprintf (temp, " [%d] ", pNode->choice);
        AddString (temp);
        break;
    }
  }
  if (medline[0]) {
    ch = *(pos - 1);
    if (ch != ' ' && ch != '\t' && ch != '\r') {
      AddString ("  ");
    }
    AddString(medline);
    medline[0] = '\0';
  }
  AddString ("\n");
  if (pIvls != NULL) {
    success = (Boolean) (report (instnce, 7, buffer) && success);
  } else {
    success = (Boolean) (report (instnce, 8, buffer) && success);
  }
  /*pubdone = TRUE;*/
}

static void GenBank (CharPtr string)

{
  AddString ("GenBank");
  AddString ("\t");
  AddString (string);
  AddString ("\n");
  success = report (instnce, 2, buffer);
}
static void Ncbi (CharPtr string)

{
  AddString ("NCBI");
  AddString ("\t");
  AddString (string);
  AddString ("\n");
  success = report (instnce, 2, buffer);
}
static void Embl (CharPtr string)

{
  AddString ("EMBL");
  AddString ("\t");
  AddString (string);
  AddString ("\n");
  success = report (instnce, 2, buffer);
}
static void Ddbj (CharPtr string)

{
  AddString ("DDBJ");
  AddString ("\t");
  AddString (string);
  AddString ("\n");
  success = report (instnce, 2, buffer);
}

static void Pir (CharPtr string)

{
  AddString ("PIR");
  AddString ("\t");
  AddString (string);
  AddString ("\n");
  success = report (instnce, 2, buffer);
}

static void Prf (CharPtr string)

{
  AddString ("PRF");
  AddString ("\t");
  AddString (string);
  AddString ("\n");
  success = report (instnce, 2, buffer);
}

static void Pdb (CharPtr string)

{
  AddString ("PDB");
  AddString ("\t");
  AddString (string);
  AddString ("\n");
  success = report (instnce, 2, buffer);
}

static void SwissProt (CharPtr string)

{
  AddString ("SWISS-PROT");
  AddString ("\t");
  AddString (string);
  AddString ("\n");
  success = report (instnce, 2, buffer);
}

static void Patent (CharPtr string)

{
  AddString ("PATENT");
  AddString ("\t");
  AddString (string);
  AddString ("\n");
  success = report (instnce, 2, buffer);
}

static void Xref (CharPtr string)

{
  AddString ("Cross-ref");
  AddString ("\t");
  AddString (string);
  AddString ("\n");
  success = report (instnce, 2, buffer);
}

static void Organism (OrgRefPtr pOrg)

{
  ValNodePtr mod;

  AddString ("Organism");
  AddString ("\t");
  if (pOrg->taxname != NULL) {
    AddString (pOrg->taxname);
  }
  if (pOrg->common != NULL) {
    if (pOrg->taxname != NULL) {
      AddString (" ");
    }
    AddString ("(");
    AddString (pOrg->common);
    AddString (")");
  }
  mod = pOrg->mod;
  while (mod != NULL) {
    AddString (", ");
    AddString (mod->data.ptrvalue);
    mod = mod->next;
  }
  AddString ("\n");
  success = report (instnce, 2, buffer);
}

static void Method (CharPtr str)

{
  AddString ("Method");
  AddString ("\t");
  AddString (str);
  AddString ("\n");

  success = report (instnce, 2, buffer);
}

static void RelDate (ValNodePtr pRelDate)

{
  Char datebuf[40];

  if (pRelDate->choice == 18)
	AddString("Created");
  else
	AddString("Updated");
  AddString ("\t");

  DatePrint((DatePtr)pRelDate->data.ptrvalue, datebuf);
  AddString (datebuf);
  AddString ("\n");
  success = report (instnce, 2, buffer);
}

static Int2 SkipPastWhiteSpace (CharPtr text, Int2 cnt)

{
  char  ch;

  ch = *(text + cnt);
  while (ch != '\0' && ch != '\n' && ch != ' ' && ch != '\t' && cnt < 8100) {
    cnt++;
    ch = *(text + cnt);
  }
  while ((ch == '\n' || ch == '\r' || ch == ' ' || ch == '\t') && cnt < 8190) {
    cnt++;
    ch = *(text + cnt);
  }
  return cnt;
}

static void Comment (SeqViewPtr pView)

{
  Int2     _class;
  Int2     cnt;
  Int2     cntr;
  Int2     len;
  Int2     start;
  CharPtr  text;

  AddString ("Comment");
  AddString ("\t");
  text = (CharPtr) pView->pDat;
  start = 0;
  cntr = StringLen (text);
  cnt = MIN (cntr, 8000);
  cnt = SkipPastWhiteSpace (text + start, cnt);
  _class = 2;
  while (cnt > 0) {
    StringNCat (buffer, text + start, cnt);
    len = (Int2) StringLen (buffer);
    pos = buffer + len;
    *pos = '\0';
    if (len > 0 && buffer [len - 1] != '\n') {
      AddString ("\n");
    }
    success = (Boolean) (report (instnce, _class, buffer) && success);
    start += cnt;
    cntr -= cnt;
    cnt = MIN (cntr, 8000);
    cnt = SkipPastWhiteSpace (text + start, cnt);
    ClearString ();
    AddString ("\t");
    _class = 3;
  }
}

static void CdRegionFeat (SeqFeatPtr pFeat)

{
  Int2       i;
  CdRegionPtr  pCdr;
  GBQualPtr  pQual;
  CharPtr    psav;

  pCdr = (CdRegionPtr) pFeat->data.value.ptrvalue;

  AddString ("Coding region\t");
  psav = pos;

  /* 1. Qualifiers */
  for (pQual = (GBQualPtr) pFeat->qual, i=0; pQual; pQual=pQual->next, i++) {
    if (i != 0)  AddString ("\r");
    AddString (pQual->qual);
    AddString (":  ");
    AddString (pQual->val);
    AddString (".");
  }

  /* 2. modifiers */
  if (pCdr->conflict) {
    if (psav!=pos)  AddString ("\r");
    AddString ("* Conflict");
  }
  if (pFeat->excpt) {
    if (psav!=pos)  AddString ("\r");
    AddString ("* Exception");
  }
  if (pFeat->partial) {
    if (psav!=pos)  AddString ("\r");
    AddString ("* Partial");
  }

  /* 3. Comments */
  if (pFeat->comment != NULL) {
    if (psav!=pos)  AddString ("\r");
    AddString ("Comments:  ");
    AddString (pFeat->comment);
    AddChar ('.');
  }
  if (pFeat->exp_ev == 1)
  {
  	AddString("\r");
	AddString("(experimentally determined)");
  }

  /* 4. Intervals */
  AddString ("\t");
  Intervals (pFeat->location);

  AddString ("\n");
  if (showIds) {
    success = report (instnce, 6, buffer);
  } else {
    success = report (instnce, 5, buffer);
  }
}

static void RnaFeat (SeqFeatPtr pFeat)

{
  char       key [16];
  RnaRefPtr  pRna;

  key [0] = '\0';
  pRna = (RnaRefPtr) pFeat->data.value.ptrvalue;
	StringCpy(key, AsnEnumStr("RNA-ref.type", (Int2)pRna->type));
   /************************ REMOVE ***********************
	switch (pRna->type) {
    case 1:
      StringCpy (key, "pre-mRNA");
      break;
    case 2:
      StringCpy (key, "mRNA");
      break;
    case 3:
      StringCpy (key, "tRNA");
      break;
    case 4:
      StringCpy (key, "rRNA");
      break;
    case 5:
      StringCpy (key, "snRNA");
      break;
    case 6:
      StringCpy (key, "scRNA");
      break;
    default:
      StringCpy (key, "RNA");
      break;
  }
  *********************************** REMOVE ***************/
  FeatureText (pFeat, key);
  AddString ("\t");
  Intervals (pFeat->location);
  AddString ("\n");
  if (showIds) {
    success = report (instnce, 6, buffer);
  } else {
    success = report (instnce, 5, buffer);
  }
}

static void ProteinFeat (SeqFeatPtr pFeat)

{
  int         i;
  ValNodePtr  pNode;
  ProtRefPtr  pProt;
  CharPtr     psav;

  pProt = (ProtRefPtr) pFeat->data.value.ptrvalue;
  AddString ("Protein\t");
  psav = pos;

  /* 1. Protein names(s) */
  for (pNode=pProt->name, i=0; pNode; pNode=pNode->next, i++) {
	if (i==0) {
		AddString ("Name");
		if (pNode->next)  AddChar ('s');
		AddString (":  ");
	}
	else 
		AddString (";  ");
	AddString (pNode->data.ptrvalue);
  }

  /* 2. Protein Description */
  if (pProt->desc != NULL) {
	if (pos != psav)  AddString ("\r");
	AddString ("Description:  ");
	AddString (pProt->desc);
	AddChar ('.');
  }

  /* 3. EC number(s) */
  for (pNode=pProt->ec, i=0; pNode; pNode = pNode->next, i++) {
	if (i==0) {
		if (pos != psav)  AddString ("\r");
		AddString ("EC number");
		if (pNode->next)  AddChar ('s');
		AddString (":  ");
	}
	else 
		AddString (";  ");
	AddString (pNode->data.ptrvalue);
  }

  /* 4. Activities */
  for (pNode=pProt->activity, i=0; pNode; pNode=pNode->next, i++) {
	if (i==0) {
		if (pos != psav)  AddString ("\r");
		AddString ("Activit");
		if (pNode->next)  AddString ("y");
		else AddString ("ies");
		AddString (":  ");
	}
	else 
		AddString (";  ");
	AddString (pNode->data.ptrvalue);
  }

  /* 5. Modifiers */
  if (pFeat->excpt) {
	if (pos != psav)  AddString ("\r");
	AddString ("*: Exception");
  }
  if (pFeat->partial) {
	if (pos != psav)  AddString ("\r");
	AddString ("*:  Partial");
  }

  /* 6. Comments */
  if (pFeat->comment != NULL) {
	if (pos != psav)  AddString ("\r");
	AddString ("Comments:  ");
	AddString (pFeat->comment);
	AddChar ('.');
  }
  if (pFeat->exp_ev == 1)
  {
  	AddString("\r");
	AddString("(experimentally determined)");
  }

  /* 7. Intervals */
  AddString ("\t");
  Intervals (pFeat->location);

  AddString ("\n");
  if (showIds) {
    success = report (instnce, 6, buffer);
  } else {
    success = report (instnce, 5, buffer);
  }
}

static void SeqRefFeat (SeqFeatPtr pFeat)

{
	SeqLocPtr slp;
	Char localbuf[40];

  slp = (SeqLocPtr) pFeat->data.value.ptrvalue;
  AddString ("SeqRef\t");

	if (slp->choice == 3)  /* whole */
	{
		SeqIdPrint((SeqIdPtr)slp->data.ptrvalue, localbuf, PRINTID_REPORT);
		AddString(localbuf);
	}
	else
		AddString("Unsupported Sequence Location");
		

  /* 5. Modifiers */
  if (pFeat->excpt) {
	AddString ("\r");
	AddString ("*: Exception");
  }
  if (pFeat->partial) {
	AddString ("\r");
	AddString ("*:  Partial");
  }

  /* 6. Comments */
  if (pFeat->comment != NULL) {
	AddString ("\r");
	AddString ("Comments:  ");
	AddString (pFeat->comment);
	AddChar ('.');
  }
  if (pFeat->exp_ev == 1)
  {
  	AddString("\r");
	AddString("(experimentally determined)");
  }

  /* 7. Intervals */
  AddString ("\t");
  Intervals (pFeat->location);

  AddString ("\n");
  if (showIds) {
    success = report (instnce, 6, buffer);
  } else {
    success = report (instnce, 5, buffer);
  }
}

static void GeneFeat (SeqFeatPtr pFeat)

{
  GeneRefPtr  pGene;
  CharPtr     psav;
  Char temp[80];

  pGene = (GeneRefPtr) pFeat->data.value.ptrvalue;
  AddString ("Gene\t");
  psav = pos;

  /* 1. Locus and Allele */
  if (pGene->locus) {
	AddString ("Locus:  ");
	Sgml2Ascii(pGene->locus, temp, 40);
	AddString (temp);
  }
  if (pGene->allele) {
	if (psav != pos)  AddString ("\r");
	AddString ("Allele:  ");
	Sgml2Ascii(pGene->allele, temp, 40);
	AddString (temp);
  }

  /* 2. Gene Description */
  if (pGene->desc) {
	if (psav != pos)  AddString("\r");
	AddString ("Description:  ");
	AddString (pGene->desc);
	AddString (".");
  }

  /* 3. Map Location */
  if (pGene->maploc != NULL) {
	if (psav != pos)  AddString("\r");
	AddString ("Map location:  ");
	AddString (pGene->maploc);
  }

  /* 4. Modifiers */
  if (pFeat->excpt) {
	if (psav != pos)  AddString("\r");
	AddString ("* Exception");
  }
  if (pFeat->partial) {
	if (psav != pos)  AddString("\r");
	AddString ("* Partial");
  }

  /* Comments */
  if (pFeat->comment) {
	if (psav != pos)  AddString("\r");
	AddString ("Comments:  ");
	AddString (pFeat->comment);
	AddChar ('.');
  }
  if (pFeat->exp_ev == 1)
  {
  	AddString("\r");
	AddString("(experimentally determined)");
  }

  /* Intervals */
  AddString ("\t");
  Intervals (pFeat->location);

  AddString ("\n");
  if (showIds) {
    success = report (instnce, 6, buffer);
  } else {
    success = report (instnce, 5, buffer);
  }
}
static void NumFeat (SeqFeatPtr pFeat)
{
  Boolean     needsReturn;

  needsReturn = FALSE;
  AddString ("Numbering");
  AddString ("\t");
  AddString ("special numbering system given");
  needsReturn = TRUE;
  if (pFeat->excpt) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString ("EXCEPTION");
    needsReturn = TRUE;
  }
  if (pFeat->partial) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString ("PARTIAL");
    needsReturn = TRUE;
  }
  if (pFeat->comment != NULL) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString (pFeat->comment);
    needsReturn = TRUE;
  }
  AddString ("\t");
  Intervals (pFeat->location);
  AddString ("\n");
  if (showIds) {
    success = report (instnce, 6, buffer);
  } else {
    success = report (instnce, 5, buffer);
  }
}
static void PsecFeat (SeqFeatPtr pFeat)
{
  Boolean     needsReturn;

  needsReturn = FALSE;
  AddString ("Sec. Str.");
  AddString ("\t");
  switch (pFeat->data.value.intvalue)
	{
		case 1:
			AddString("Helix");
			break;
		case 2:
			AddString("Beta Sheet");
			break;
		case 3:
			AddString("Turn");
			break;
	}
	needsReturn = TRUE;
  if (pFeat->excpt) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString ("EXCEPTION");
    needsReturn = TRUE;
  }
  if (pFeat->partial) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString ("PARTIAL");
    needsReturn = TRUE;
  }
  if (pFeat->comment != NULL) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString (pFeat->comment);
    needsReturn = TRUE;
  }
  if (pFeat->exp_ev == 1)
  {
	if (needsReturn)
	  	AddString("\r");
	AddString("(experimentally determined)");
	needsReturn = TRUE;
  }
  AddString ("\t");
  Intervals (pFeat->location);
  AddString ("\n");
  if (showIds) {
    success = report (instnce, 6, buffer);
  } else {
    success = report (instnce, 5, buffer);
  }
}
static void NonStdResFeat (SeqFeatPtr pFeat)
{
  Boolean     needsReturn;

  needsReturn = FALSE;
  AddString ("NonStdRes");
  AddString ("\t");
  AddString((CharPtr) pFeat->data.value.ptrvalue);
  needsReturn = TRUE;
  if (pFeat->excpt) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString ("EXCEPTION");
    needsReturn = TRUE;
  }
  if (pFeat->partial) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString ("PARTIAL");
    needsReturn = TRUE;
  }
  if (pFeat->comment != NULL) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString (pFeat->comment);
    needsReturn = TRUE;
  }
  if (pFeat->exp_ev == 1)
  {
	if (needsReturn)
	  	AddString("\r");
	AddString("(experimentally determined)");
	needsReturn = TRUE;
  }
  AddString ("\t");
  Intervals (pFeat->location);
  AddString ("\n");
  if (showIds) {
    success = report (instnce, 6, buffer);
  } else {
    success = report (instnce, 5, buffer);
  }
}
static void HetFeat (SeqFeatPtr pFeat)
{
  Boolean     needsReturn;

  needsReturn = FALSE;
  AddString ("Heterogen");
  AddString ("\t");
  AddString((CharPtr) pFeat->data.value.ptrvalue);
  needsReturn = TRUE;
  if (pFeat->excpt) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString ("EXCEPTION");
    needsReturn = TRUE;
  }
  if (pFeat->partial) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString ("PARTIAL");
    needsReturn = TRUE;
  }
  if (pFeat->comment != NULL) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString (pFeat->comment);
    needsReturn = TRUE;
  }
  if (pFeat->exp_ev == 1)
  {
	if (needsReturn)
	  	AddString("\r");
	AddString("(experimentally determined)");
	needsReturn = TRUE;
  }
  AddString ("\t");
  Intervals (pFeat->location);
  AddString ("\n");
  if (showIds) {
    success = report (instnce, 6, buffer);
  } else {
    success = report (instnce, 5, buffer);
  }
}

static void OrgFeat (SeqFeatPtr pFeat)

{
  ValNodePtr  mod;
  Boolean     needsReturn;
  OrgRefPtr   pOrg;

  needsReturn = FALSE;
  AddString ("Organism");
  AddString ("\t");
  pOrg = (OrgRefPtr) pFeat->data.value.ptrvalue;
  if (pOrg->taxname != NULL) {
    AddString (pOrg->taxname);
    needsReturn = TRUE;
  }
  if (pOrg->common != NULL) {
    if (pOrg->taxname != NULL) {
      AddString (" ");
    }
    AddString ("(");
    AddString (pOrg->common);
    AddString (")");
    needsReturn = TRUE;
  }
  mod = pOrg->mod;
  while (mod != NULL) {
    AddString (", ");
    AddString (mod->data.ptrvalue);
    mod = mod->next;
    needsReturn = TRUE;
  }
  if (pFeat->excpt) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString ("EXCEPTION");
    needsReturn = TRUE;
  }
  if (pFeat->partial) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString ("PARTIAL");
    needsReturn = TRUE;
  }
  if (pFeat->comment != NULL) {
    if (needsReturn) {
      AddString ("\r");
    }
    AddString (pFeat->comment);
    needsReturn = TRUE;
  }
  AddString ("\t");
  Intervals (pFeat->location);
  AddString ("\n");
  if (showIds) {
    success = report (instnce, 6, buffer);
  } else {
    success = report (instnce, 5, buffer);
  }
}

static void ImportFeat (SeqFeatPtr pFeat)

{
  char        key [32];
  ImpFeatPtr  pImp;

  key [0] = '\0';
  pImp = (ImpFeatPtr) pFeat->data.value.ptrvalue;
  if (pImp != NULL) {
    if (pImp->descr != NULL) {
      StringCpy (buffer, pImp->descr);
    }
    StringCpy (key, pImp->key);
  } else {
    StringCpy (key, "???");
  }
  FeatureText (pFeat, key);
  AddString ("\t");
  Intervals (pFeat->location);
  AddString ("\n");
  if (showIds) {
    success = report (instnce, 6, buffer);
  } else {
    success = report (instnce, 5, buffer);
  }
}

static void SeqData (BioseqPtr pSeq, Boolean showSeq, Int2 charsPerLine)

{
  char          ch;
  Boolean       empty;
  Int2          h, retval;
  Int2          i;
  Int4          k;
  Int2          last;
  Int4          len;
  char          mtype [16];
  Int2          nChars;
  ByteStorePtr  pBS;
  char          strnd [16];
  char          temp [64];
  char          topol [16];
  char          units [16];
  NumberingPtr np;
  DataVal av;
  ValNodePtr anp;
  Int4 lastnum, thenum;
  Boolean isa_num, was_null=FALSE;
  CharPtr tmp;

  

  AddString ("Sequence");
  AddString ("\t");
  len = pSeq->length;
  mtype [0] = '\0';
  strnd [0] = '\0';
  topol [0] = '\0';
  units [0] = '\0';
  if (pSeq->mol == 3) {
    StringCpy (units, "aa");
  } else {
    StringCpy (units, "nt");
	
	StringCpy (mtype, AsnEnumStr("Seq-inst.mol", (Int2)pSeq->mol));
	anp = pSeq->descr;
	while (anp != NULL)
	{
		if (anp->choice == Seq_descr_mol_type)
		{
			StringCpy (mtype, AsnEnumStr("GIBB-mol", (Int2)anp->data.intvalue));
			break;
		}
		anp = anp->next;
	}
	
	
    if (mtype [0] != '\0') {
      StringCpy (topol, ", ");
	  StringCat(topol, AsnEnumStr("Seq-inst.topology", (Int2)pSeq->topology));
	  StringCat(topol, " ");
	  if ((pSeq->strand >= 1) && (pSeq->strand <= 3))
	  {
		  StringCat(strnd, AsnEnumStr("Seq-inst.strand", (Int2)pSeq->strand));
		  StringCat(strnd, " ");
	  }

    }
  }
  if (len >= 0) {
    sprintf (temp, "%ld %s%s%s%s", len, units, topol, strnd, mtype);
    AddString (temp);
    if (! showSeq) {
      AddString (" (not shown)");
    }
  } else {
    AddString ("(nonexistent)");
  }
  AddString ("\n");
  success = report (instnce, 2, buffer);
  ClearString ();
  if (showSeq && len >= 0) {
    pBS = (ByteStorePtr) pSeq->seq_data;
    BSSeek (pBS, 0L, 0);
	np = BioseqGetNumbering(pSeq);
    k = 0;
    h = 0;
    empty = TRUE;
    while (k < len) {
      empty = FALSE;
	  retval = NumberingValue(np, k, &av);
	  /*
	  AddString("      ");
	  */
	  isa_num = TRUE;
	  was_null = FALSE;
	  if (retval == 1)
	  {
	  	lastnum = av.intvalue;
      	sprintf (temp, "%ld", lastnum);
	  }
	  else if (retval == 3) /* string */
	  {
	  	StringMove(temp, (CharPtr)av.ptrvalue);
		tmp = temp;
		if (* temp == '\0')  /* null string */
		{
			isa_num = FALSE;
			was_null = TRUE;
			StringMove(temp, "Unnumbered.");
		}
		else while (*tmp != '\0')
		{
			if (! IS_DIGIT(*tmp))
				isa_num = FALSE;
			tmp++;
		}
		if (isa_num)
			lastnum = atol(temp);
	  }
	  else
	  {
	  	isa_num = FALSE;
	  	StringMove(temp, " ");
	  }
      AddString (temp);
      AddString ("\t");
      nChars = (Int2) MIN (len - k, (Int4) charsPerLine);
      for (i = 0; i < nChars; i++) {
        if ((i > 0) && (i % 10 == 0)) {
          AddChar (' ');
        }
        ch = (Char) BSGetByte (pBS);
        ch = TO_LOWER (ch);
        AddChar (ch);
		k++;
		if ((! isa_num) && (! was_null))
			break;

		if (k == len)
			break;
			
	  retval = NumberingValue(np, k, &av);
	  if (was_null)
	  {
	  	if (retval == 3)   /* string */
		{
			if (* (CharPtr)(av.ptrvalue) != '\0')  /* end of nulls */
				break;
		}
	  }

	  isa_num = TRUE;
	  if (retval == 1)
	  {
	  	if (was_null)
			break;
	  	thenum = av.intvalue;
	  }
	  else if (retval == 3) /* string */
	  {
	  	StringMove(temp, av.ptrvalue);
		tmp = temp;
		if (*tmp == '\0')  /* null string */
		{
			if (! was_null)
				break;
			isa_num = FALSE;
		}
		else while (*tmp != '\0')
		{
			if (! IS_DIGIT(*tmp))
				isa_num = FALSE;
			tmp++;
		}
		if (isa_num)
		{
			if (was_null)
				break;
			thenum = atol(temp);
		}
	  }
	  else
	  {
	  	if (was_null)
			break;
	  	isa_num = FALSE;
	  }

	    if (! was_null)
		{
			if (! isa_num)
				break;
			lastnum = thenum - lastnum;
			if (ABS(lastnum) != 1)
				break;
			lastnum = thenum;
		}
      }
      AddString ("\n");
      h++;
      if (h >= 5) {
        h = 0;
        success = report (instnce, 9, buffer);
        ClearString ();
        empty = TRUE;
      }
    }
    if (! empty) {
      last = StringLen (buffer);
      if (last > 0 && buffer [last - 1] != '\n') {
        AddString ("\n");
      }
      success = report (instnce, 9, buffer);
    }
  }
}

Boolean FlatSeqToReport (SeqViewPtr pView, Int2 nItems, ReportProc proc, VoidPtr inst, Boolean showSeq, Int2 charsPerLine)

{
  int         i;
  SeqViewPtr  p;
  SeqFeatPtr  pFeat;
  Boolean     rsult;
  char temp [16];

  rsult = TRUE;
  instnce = inst;
  report = proc;
  showIds = TRUE;
  firstTime = TRUE;
  inpubset = FALSE;
  pubdone = FALSE;

  if (pView != NULL && proc != NULL) {
    buffer = MemNew (BUFSIZE);
    if (buffer != NULL) {
      for (i=0, p=pView; i<nItems; i++, p++) {
        success = TRUE;
        pFeat = (SeqFeatPtr) p->pDat;
/*
        if (p->nType==NPSET || p->nType==SEGSET || p->nType==PDBSET || p->nType==OTHERSET) {
          showIds = TRUE;
        } else if (p->nType==RAWPROT || p->nType==RAWNUCL || p->nType==CONSTSEQ) {
          showIds = FALSE;
        }
*/
        ClearString ();
        switch (p->nType) {
          case PUBSET:
            /* ignore */
	    inpubset = TRUE;
            break;
          case NPSET:
          case SEGSET:
		  case PDBSET:
	    if (p->pDat)  Definition (p);
            break;
          case ENDSET:
            /* ignore */
            break;
          case BEGENTRY:
	    BeginEntry (p);
	    break;
	  case RAWNUCL:
	  case RAWPROT:
            if (p->pDat)  Definition (p);
            break;
          case VIRTSEQ:
            AnyString ("Sequence", sVirtual);
            break;
          case CONSTSEQ:
            Definition (p);
            break;
          case MEMBERS:
            Contents (p->pDat);
            break;
          case SEGMENTS:
            Segments (p->pDat);
            break;
		  case NCBI:
		    Ncbi (p->pDat);
			break;
          case GENBANK:
            GenBank (p->pDat);
            break;
          case EMBL:
            Embl (p->pDat);
            break;
          case DDBJ:
            Ddbj (p->pDat);
            break;
          case PIR:
            Pir (p->pDat);
            break;
          case PRF:
            Prf (p->pDat);
            break;
          case PDB:
            Pdb (p->pDat);
            break;
          case XREF:
            Xref (p->pDat);
            break;
          case ORGANISM:
            Organism ((OrgRefPtr) p->pDat);
            break;
          case METHOD:
            Method ((CharPtr) p->pDat);
            break;
		  case RELDATE:
			RelDate((ValNodePtr) p->pDat);
			break;
          case PUBINFO:
            PubInformation (p->pDat);
            break;
          case REFERENCE:
	    if (inpubset) {
		if (!pubdone)
            		Reference (p->pDat, NULL);
		pubdone = TRUE;
	    }
	    else Reference (p->pDat, NULL);
            break;
          case COMMENT:
            Comment (p);
            break;
          case SOURCE:
            break;
          case ORIGIN:
            break;
          case SWISSPROT:
            SwissProt (p->pDat);
            break;
          case PATENT:
            Patent (p->pDat);
            break;
          case FEAT_GENE:
            GeneFeat (pFeat);
            break;
          case FEAT_ORG:
            OrgFeat (pFeat);
            break;
          case FEAT_CDRGN:
            CdRegionFeat (pFeat);
            break;
          case FEAT_PROT:
            ProteinFeat (pFeat);
            break;
          case FEAT_RNA:
            RnaFeat (pFeat);
            break;
          case FEAT_PUB:
            Reference (pFeat->data.value.ptrvalue, pFeat->location);
            break;
          case FEAT_SEQ:
		  	SeqRefFeat(pFeat);
            break;
          case FEAT_IMP:
            ImportFeat (pFeat);
            break;
          case FEAT_REGION:
            FeatString ("Region", pFeat->comment, pFeat);
            break;
          case FEAT_COMMENT:
            FeatString ("Comment", pFeat->comment, pFeat);
            break;
          case FEAT_BOND:
            FeatString ("Bond", pFeat->comment, pFeat);
            break;
          case FEAT_SITE:
            FeatString ("Site", pFeat->comment, pFeat);
            break;
          case FEAT_NUM:
            NumFeat (pFeat);
            break;
          case FEAT_PSEC:
            PsecFeat (pFeat);
            break;
          case FEAT_NSR:
            NonStdResFeat (pFeat);
            break;
          case FEAT_HET:
            HetFeat (pFeat);
            break;
          case FEAT_OTHER:
            FeatString ("_____", "_____", pFeat);
            break;
          case SEQUENCE:
            SeqData (p->pDat, showSeq, charsPerLine);
            break;
          case OTHER:
            AnyString ("[NOTE]", p->pDat);
            break;
          default:
            sprintf (temp, "case %d:", p->nType);
            AnyString (temp, "(not implemented)");
            break;
        }
        if (! success) {
          rsult = FALSE;
        }
      }
    }
    buffer = MemFree (buffer);
  }
  return rsult;
}

/* ----- Default File Column Formats ----- */

static ColData table0 [1] = {{0, 78, 0, 'l', TRUE, TRUE, TRUE}};

static ColData table1 [2] = {{0, 15, 0, 'l', TRUE, TRUE, FALSE},
                             {0, 63, 0, 'l', TRUE, TRUE, TRUE}};

static ColData table2 [2] = {{0, 15, 0, 'l', TRUE, TRUE, FALSE},
                             {0, 63, 0, 'l', FALSE, TRUE, TRUE}};

static ColData table3 [2] = {{0, 15, 0, 'l', TRUE, TRUE, FALSE},
                             {0, 63, 0, 'l', FALSE, TRUE, TRUE}};

static ColData table4 [3] = {{0, 15, 0, 'l', TRUE, TRUE, FALSE},
                             {0, 15, 0, 'l', TRUE, TRUE, FALSE},
                             {0, 48, 0, 'l', TRUE, TRUE, TRUE}};

static ColData table5 [3] = {{0, 15, 0, 'l', TRUE, TRUE, FALSE},
                             {0, 45, 0, 'l', TRUE, TRUE, FALSE},
                             {0, 18, 0, 'l', TRUE, TRUE, TRUE}};

static ColData table6 [3] = {{0, 15, 0, 'l', TRUE, TRUE, FALSE},
                             {0, 35, 0, 'l', TRUE, TRUE, FALSE},
                             {0, 28, 0, 'l', TRUE, TRUE, TRUE}};

static ColData table7 [5] = {{0, 15, 0, 'l', TRUE, TRUE, FALSE},
                             {0, 63, 0, 'l', TRUE, TRUE, TRUE}};

static ColData table8 [5] = {{0, 15, 0, 'l', TRUE, TRUE, FALSE},
                             {0, 63, 0, 'l', TRUE, TRUE, TRUE}};

static ColData table9 [2] = {{0, 15, 1, 'r', TRUE, TRUE, FALSE},
                             {0, 63, 0, 'l', TRUE, TRUE, TRUE}};

static ColData table10 [2] = {{0, 15, 0, 'l', TRUE, TRUE, FALSE},
                              {0, 63, 0, 'l', FALSE, TRUE, TRUE}};

/* ----- Complex Function Bodies ----- */

Boolean StdReportProc (VoidPtr inst, Int2 _class, CharPtr string)

{
  FILE     *f;
  ParData  para;
  Boolean  rsult;

  rsult = FALSE;
  f = (FILE*) inst;
  if (firstTime) {
    para.openSpace = FALSE;
    firstTime = FALSE;
  } else {
    para.openSpace = TRUE;
  }
  switch (_class) {
    case 0:
      rsult = SendTextToFile (f, string, &para, table0);
      break;
    case 1:
      rsult = SendTextToFile (f, string, &para, table1);
      break;
    case 2:
      rsult = SendTextToFile (f, string, &para, table2);
      break;
    case 3:
      para.openSpace = FALSE;
      rsult = SendTextToFile (f, string, &para, table3);
      break;
    case 4:
      rsult = SendTextToFile (f, string, &para, table4);
      break;
    case 5:
      rsult = SendTextToFile (f, string, &para, table5);
      break;
    case 6:
      rsult = SendTextToFile (f, string, &para, table6);
      break;
    case 7:
      para.openSpace = FALSE;
      rsult = SendTextToFile (f, string, &para, table7);
      break;
    case 8:
      rsult = SendTextToFile (f, string, &para, table8);
      break;
    case 9:
      rsult = SendTextToFile (f, string, &para, table9);
      break;
    case 10:
      rsult = SendTextToFile (f, string, &para, table10);
      break;
    default:
      rsult = FALSE;
      break;
  }
  return rsult;
}

Boolean SeqEntryToReport (SeqEntryPtr pEntry, ReportProc proc, VoidPtr inst, Boolean showSeq, Int2 charsPerLine)

{
  Int2        nItems;
  SeqViewPtr  pFlat;
  Boolean     rsult;

  rsult = FALSE;
  if (pEntry != NULL && proc != NULL) {
    SeqEntryConvert (pEntry, Seq_code_iupacna);
    pFlat = FlatSeqEntryNew (pEntry, &nItems);
    if (pFlat != NULL) {
      rsult = FlatSeqToReport (pFlat, nItems, proc, inst, showSeq, charsPerLine);
    }
    pFlat = FlatSeqEntryFree (pFlat);
  }
  return rsult;
}

Boolean SeqEntryToFile (SeqEntryPtr pEntry, FILE *fp, Boolean showSeq, Int2 charsPerLine, Boolean checking)

{
  Int2        nItems;
  SeqViewPtr  pFlat;
  Boolean     rsult;

  rsult = FALSE;
  CheckForm=checking;
  if (pEntry != NULL && fp != NULL) {
    SeqEntryConvert (pEntry, Seq_code_iupacna);
    pFlat = FlatSeqEntryNew (pEntry, &nItems);
    if (pFlat != NULL) {
      rsult = FlatSeqToReport (pFlat, nItems, StdReportProc,
                               (VoidPtr) fp, showSeq, charsPerLine);
    }
    pFlat = FlatSeqEntryFree (pFlat);
  }
  return rsult;
}

