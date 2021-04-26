/*  sequtil.h
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
* File Name:  sequtil.h
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.17 $
*
* File Description:  Sequence Utilities for objseq and objsset
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/
#ifndef _NCBI_SeqUtil_
#define _NCBI_SeqUtil_

#ifndef _NCBI_Seqset_
#include <objsset.h>		   /* the object loader interface */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   What am I?
*
*****************************************************************************/
extern Uint1 Bioseq_repr PROTO((BioseqPtr bsp));
extern Uint1 BioseqGetCode PROTO((BioseqPtr bsp));

ValNodePtr BioseqGetSeqDescr PROTO((BioseqPtr bsp, Int2 type, ValNodePtr curr));
CharPtr BioseqGetTitle PROTO((BioseqPtr bsp));
NumberingPtr BioseqGetNumbering PROTO((BioseqPtr bsp));

extern Int4 BioseqGetLen PROTO((BioseqPtr bsp));
extern Int4 BioseqGetGaps PROTO((BioseqPtr bsp));
extern Int4 BioseqGetSegLens PROTO((BioseqPtr bsp, Int4Ptr lens));
#define BioseqCountSegs(x) BioseqGetSegLens(x, NULL)

extern Boolean BioseqRawConvert PROTO((BioseqPtr bsp, Uint1 newcode));
extern Boolean BioseqRawPack PROTO((BioseqPtr bsp));
extern ByteStorePtr BSConvertSeq PROTO((ByteStorePtr bsp, Uint1 newcode, Uint1 oldcode, Int4 seqlen));

BioseqPtr BioseqFind PROTO((SeqIdPtr sip));
Boolean BioseqMatch PROTO((BioseqPtr bsp, SeqIdPtr sip));
SeqEntryPtr SeqEntryFind PROTO((SeqIdPtr sip));

CharPtr StringForSeqMethod PROTO((Int2 method));

/*****************************************************************************
*
*   Context routines for Bioseqs in Seq-entrys
*      Context is the chain of Seqentries leading to the bioseq.
*      context[count-1] is SeqEntry for bsp itself
*      If Bioseq not in a Seqentry, count is 0 and bcp->se may be used
*        if a fake Seqentry is convenient.
*
*****************************************************************************/
#define BIOSEQCONTEXTMAX 20

typedef struct bioseqcontxt {
	BioseqPtr bsp;           /* the Bioseq in question */
	Int2 count;              /* number of elements in context */
	Boolean hit;             /* used by BioseqContextNew and ..GetSeqFeat */
	SeqEntryPtr context[BIOSEQCONTEXTMAX];  /* array of SeqEntryPtr (last is count -1) */
	ValNode se;             /* used for a tempory SeqEntryPtr when only a Bioseq */
	SeqFeatPtr sfp;          /* current sfp */
	SeqAnnotPtr sap;         /* current sap */
	Int2 sftype,             /* SeqFeat type to look for */
		in;					 /* 0=location, 1=product, 2=either */
} BioseqContext, PNTR BioseqContextPtr;

BioseqContextPtr BioseqContextNew PROTO((BioseqPtr bsp));
BioseqContextPtr BioseqContextFree PROTO((BioseqContextPtr bcp));
/*****************************************************************************
*
*   BioseqContextGetSeqDescr(bcp, type, curr, SeqEntryPtr PNTR sep)
*       returns pointer to the next SeqDescr of this type
*       type gives type of Seq-descr
*       if (type == 0)
*          get them all
*       curr is NULL or previous node of this type found
*       moves up from bsp
*		if (sep != NULL) sep set to SeqEntryPtr containing the SeqDescr.
*
*****************************************************************************/
ValNodePtr BioseqContextGetSeqDescr PROTO((BioseqContextPtr bcp, Int2 type, ValNodePtr curr, SeqEntryPtr PNTR the_sep));
CharPtr BioseqContextGetTitle PROTO((BioseqContextPtr bcp));
/*****************************************************************************
*
*   BioseqContextGetSeqFeat(bcp, type, curr, sapp, in)
*       returns pointer to the next Seq-feat of this type
*       type gives type of Seq-descr
*       if (type == 0)
*          get them all
*       curr is NULL or previous node of this type found
*       moves up from bsp
*   	if (sapp != NULL) is filled with SeqAnnotPtr containing the SeqFeat
*   	in:
*   		0 = sfp->location only
*   		1 = sfp->product only
*   		2 = either of above
*
*****************************************************************************/
SeqFeatPtr BioseqContextGetSeqFeat PROTO((BioseqContextPtr bcp, Int2 type,
	SeqFeatPtr curr, SeqAnnotPtr PNTR sapp, Int2 in));

/*** works like BioseqContextGetSeqFeat, but a SeqEntryPtr and (optionally)
     a Bioseq will do ****************************************************/

/*****************************************************************************
*
*   SeqEntryGetSeqFeat(sep, type, curr, sapp)
*       returns pointer to the next Seq-feat of this type
*       type gives type of SeqFeat
*       if (type == 0)
*          get them all
*       curr is NULL or previous node of this type found
*       moves up from bsp
*   	if (sapp != NULL) is filled with SeqAnnotPtr containing the SeqFeat
*       if (bsp != NULL) then for that Bioseq match on location by "in"
*   	in:
*   		0 = sfp->location only
*   		1 = sfp->product only
*   		2 = either of above
*
*****************************************************************************/
SeqFeatPtr SeqEntryGetSeqFeat PROTO((SeqEntryPtr sep, Int2 type,
	SeqFeatPtr curr, SeqAnnotPtr PNTR sapp, Int2 in, BioseqPtr bsp));

/*****************************************************************************
*
*   SeqCodeTable routines
*   SeqMapTable routines
*     both may return INVALID_RESIDUE when a residue is out of range
*
*****************************************************************************/
#define INVALID_RESIDUE 255

Uint1 SeqMapTableConvert PROTO((SeqMapTablePtr smtp, Uint1 residue));
Uint1 SeqCodeTableComp PROTO((SeqCodeTablePtr sctp, Uint1 residue));


/*****************************************************************************
*
*   Numbering routines
*
*****************************************************************************/
                              /* convert any numbering value to seq offset */
extern Int4 NumberingOffset PROTO((NumberingPtr np, DataValPtr avp));
                              /* convert seq offset to numbering value */
extern Int2 NumberingValue PROTO((NumberingPtr np, Int4 offset, DataValPtr avp));
extern Int2 NumberingValueBySeqId PROTO((SeqIdPtr sip, Int4 offset, DataValPtr avp));

extern void NumberingDefaultLoad PROTO((void));
extern NumberingPtr NumberingDefaultGet PROTO((void));

/*****************************************************************************
*
*   SeqEntry and BioseqSet stuff
*
*****************************************************************************/

Uint1 Bioseq_set_class PROTO((SeqEntryPtr sep));

/*****************************************************************************
*
*   traversal routines
*       SeqEntry - any type
*
*****************************************************************************/
typedef void (* SeqEntryFunc) PROTO((SeqEntryPtr sep, Pointer mydata, Int4 index, Int2 indent));
extern Int4 SeqEntryList PROTO((SeqEntryPtr sep, Pointer mydata, SeqEntryFunc mycallback, Int4 index, Int2 indent));

#define SeqEntryCount( a )  SeqEntryList( a ,NULL,NULL,0,0);
#define SeqEntryExplore(a,b,c) SeqEntryList(a, b, c, 0L, 0);

/*****************************************************************************
*
*   traversal routines
*       Bioseq types only - "individual" sequences
*       do NOT traverse component parts of seqmented or constructed types
*
*****************************************************************************/
extern Int4 BioseqList PROTO((SeqEntryPtr sep, Pointer mydata, SeqEntryFunc mycallback, Int4 index, Int2 indent));

#define BioseqCount( a )  BioseqList( a ,NULL,NULL,0,0);
#define BioseqExplore(a,b,c) BioseqList(a, b, c, 0L, 0);

/*****************************************************************************
*
*   Get parts routines
*
*****************************************************************************/
                       /* gets next Seqdescr after curr in sep of type type */
ValNodePtr SeqEntryGetSeqDescr PROTO((SeqEntryPtr sep, Int2 type, ValNodePtr curr));
                       /* gets first title from sep */
CharPtr SeqEntryGetTitle PROTO((SeqEntryPtr sep));

/*****************************************************************************
*
*   Manipulations
*
*****************************************************************************/

extern Boolean SeqEntryConvert PROTO((SeqEntryPtr sep, Uint1 newcode));
#define SeqEntryPack(x) SeqEntryConvert(x, (Uint1)0)


/*****************************************************************************
*
*   SeqLoc stuff
*
*****************************************************************************/
#define PRINTID_FASTA_SHORT ( (Uint1)1)
#define PRINTID_FASTA_LONG ( (Uint1)2)
#define PRINTID_TEXTID_LOCUS ( (Uint1)3)
#define PRINTID_TEXTID_ACCESSION ( (Uint1)4)
#define PRINTID_REPORT ((Uint1)5)

SeqIdPtr SeqIdSelect PROTO((SeqIdPtr sip, Uint1Ptr order, Int2 num));
Int2 SeqIdBestRank PROTO((Uint1Ptr buf, Int2 num));
SeqIdPtr SeqIdFindBest PROTO(( SeqIdPtr sip, Uint1 target));
CharPtr SeqIdPrint PROTO((SeqIdPtr sip, CharPtr buf, Uint1 format));
SeqIdPtr SeqIdParse PROTO((CharPtr buf));

/*****************************************************************************
*
*   Boolean SeqIdMatch(a,b)
*       returns TRUE if seqids match
*
*****************************************************************************/
Boolean SeqIdMatch PROTO((SeqIdPtr a, SeqIdPtr b));
/*************************
   SeqIdForSameBioseq(a,b)
   trys to locate all ids for a or b and determine
   if (a and b refer the the same Bioseq)
**************************/
Boolean SeqIdForSameBioseq PROTO((SeqIdPtr a, SeqIdPtr b));
/*************************
 *      Boolean SeqIdIn (a,b)
 *   returns TRUE if a in list of b
 ******************/
Boolean SeqIdIn PROTO((SeqIdPtr a, SeqIdPtr b));


SeqLocPtr SeqLocFindNext PROTO((SeqLocPtr seqlochead, SeqLocPtr currseqloc));
Boolean IS_one_loc PROTO((SeqLocPtr anp));  /* for SeqLoc */

Int4 SeqLocStart PROTO((SeqLocPtr seqloc));
Int4 SeqLocStop PROTO((SeqLocPtr seqloc));
Uint1 SeqLocStrand PROTO((SeqLocPtr seqloc));
Int4 SeqLocLen PROTO((SeqLocPtr seqloc));
Int4 SeqLocGetSegLens PROTO((SeqLocPtr slp, Int4Ptr lens, Int4 ctr, Boolean gaps));
#define SeqLocCountSegs(x) SeqLocGetSegLens(x, NULL,0,FALSE)
#define SeqLocGetGaps(x) SeqLocGetSegLens(x,NULL,0,TRUE)
SeqIdPtr SeqLocId PROTO((SeqLocPtr seqloc));
Uint1 StrandCmp PROTO((Uint1 strand));
Boolean SeqLocRevCmp PROTO((SeqLocPtr anp));

/**** defines for "which_end" below ****/

#define SEQLOC_LEFT_END  1    /* low numbered end of SeqLoc */
#define SEQLOC_RIGHT_END 2    /* high numbered end of SeqLoc */
#define SEQLOC_START     3	  /* beginning of SeqLoc (low on plus, high on minus)  */
#define SEQLOC_STOP      4	  /* end of SeqLoc (high on plus, low on minus)  */

Int4 GetOffsetInLoc PROTO((SeqLocPtr of, SeqLocPtr in, Uint1 which_end));
Int4 GetOffsetInBioseq PROTO((SeqLocPtr of, BioseqPtr in, Uint1 which_end));
Int2 SeqLocOrder PROTO((SeqLocPtr a, SeqLocPtr b, BioseqPtr in));

Int2 SeqLocMol PROTO((SeqLocPtr seqloc));

CharPtr SeqLocPrint PROTO((SeqLocPtr slp));


/*****************************************************************************
*
*   SeqLocCompare(a, b)
*   	returns
*   	0 = no overlap
*   	1 = a is completely contained in b
*   	2 = b is completely contained in a
*   	3 = a == b
*   	4 = a and b overlap, but neither completely contained in the other
*
*****************************************************************************/
Int2 SeqLocCompare PROTO((SeqLocPtr a, SeqLocPtr b));
#define SLC_NO_MATCH 0
#define SLC_A_IN_B 1
#define SLC_B_IN_A 2
#define SLC_A_EQ_B 3
#define SLC_A_OVERLAP_B 4

Boolean SeqIntCheck PROTO((SeqIntPtr sip));   /* checks for valid interval */
Boolean SeqPntCheck PROTO((SeqPntPtr spp));  /* checks valid pnt */
Boolean PackSeqPntCheck PROTO ((PackSeqPntPtr pspp));
Boolean SeqLocCheck PROTO ((SeqLocPtr slp));
/*****************************************************************************
*
*   SeqLocPartialCheck(head)
*       sets bits for incomplete location and/or errors
*       incomplete defined as Int-fuzz on start or stop with
*         lim.unk, lim.gt, or lim.lt set
*   
* SLP_COMPLETE = not partial and no errors
* SLP_START = incomplete on start (high number on minus strand, low on plus)
* SLP_STOP     = incomplete on stop
* SLP_INTERNAL = lim set on internal intervals
* SLP_OTHER    = partial location, but no details available
* SLP_NOSTART  = start does not include end of sequence
* SLP_NOSTOP   = stop does not include end of sequence
* SLP_NOINTERNAL = internal interval not on end of sequence
* SLP_LIM_WRONG  = lim gt/lt used inconsistently with position in location
*
* SLP_HAD_ERROR  = if AND with return, is TRUE if any errors encountered
*   
*****************************************************************************/

#define SLP_COMPLETE	0
#define SLP_START		1
#define SLP_STOP		2
#define SLP_INTERNAL	4
#define SLP_OTHER		8
#define SLP_NOSTART		16
#define SLP_NOSTOP		32
#define SLP_NOINTERNAL	64
#define SLP_LIM_WRONG	128

#define SLP_HAD_ERROR   240

Uint2 SeqLocPartialCheck PROTO((SeqLocPtr head));

CharPtr TaxNameFromCommon PROTO((CharPtr common));

#ifdef __cplusplus
}
#endif

#endif
