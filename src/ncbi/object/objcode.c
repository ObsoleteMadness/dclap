/*  objcode.c
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
* File Name:  objcode.c
*
* Author:  James Ostell
*   
* Version Creation Date: 4/1/91
*
* $Revision: 2.2 $
*
* File Description:  Object manager for module NCBI-SeqCode
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 05-13-93 Schuler     All public functions are now declared LIBCALL.
*
*
* ==========================================================================
*/
#include <objcode.h>		   /* the pub interface */
#include <asncode.h>        /* the AsnTool header */

static Boolean loaded = FALSE;

static SeqCodeSetPtr scspl = NULL; /* loaded SeqCodeTables and SeqMapTables */

/*****************************************************************************
*
*   SeqCodeAsnLoad()
*
*****************************************************************************/
Boolean LIBCALL SeqCodeAsnLoad ()
{
    if (loaded)
        return TRUE;
    loaded = TRUE;

    if (! AsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    return TRUE;
}

/*****************************************************************************
*
*   SeqMapTableNew()
*
*****************************************************************************/
SeqMapTablePtr LIBCALL SeqMapTableNew (void)
{
    return (SeqMapTablePtr)MemNew(sizeof(SeqMapTable));
}

/*****************************************************************************
*
*   SeqMapTableFree(smtp)
*       Frees a SeqMapTable and associated data
*
*****************************************************************************/
SeqMapTablePtr LIBCALL SeqMapTableFree (SeqMapTablePtr smtp)
{
    if (smtp == NULL)
        return smtp;
    MemFree(smtp->table);
	return (SeqMapTablePtr)MemFree(smtp);
}
/*****************************************************************************
*
*   SeqMapTableAsnWrite(smtp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SeqMapTable ::=)
*
*****************************************************************************/
Boolean LIBCALL SeqMapTableAsnWrite (SeqMapTablePtr smtp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Uint1 i, num;
    Uint1Ptr ipnt;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqCodeAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, SEQ_MAP_TABLE);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (smtp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)smtp)) goto erret;
    
    av.intvalue = smtp->from;
    if (! AsnWrite(aip, SEQ_MAP_TABLE_from, &av)) goto erret;
    av.intvalue = smtp->to;
    if (! AsnWrite(aip, SEQ_MAP_TABLE_to, &av)) goto erret;
    num = smtp->num;
    av.intvalue = (Int4)num;
    if (! AsnWrite(aip, SEQ_MAP_TABLE_num, &av)) goto erret;

    if (smtp->start_at)
    {
        av.intvalue = (Int4)smtp->start_at;
        if (! AsnWrite(aip, SEQ_MAP_TABLE_start_at, &av)) goto erret;
    }
    if (! AsnOpenStruct(aip, SEQ_MAP_TABLE_table, (Pointer)smtp->table)) goto erret;
    ipnt = smtp->table;
    for (i = 0; i < num; i++, ipnt++)
    {
        av.intvalue = (Int4)*ipnt;
        if (! AsnWrite(aip, SEQ_MAP_TABLE_table_E, &av)) goto erret;
    }
    if (! AsnCloseStruct(aip, SEQ_MAP_TABLE_table, (Pointer)smtp->table)) goto erret;

    if (! AsnCloseStruct(aip, atp, (Pointer)smtp)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqMapTableAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
SeqMapTablePtr LIBCALL SeqMapTableAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    SeqMapTablePtr smtp=NULL;
    Uint1 i, num;
    Uint1Ptr ipnt;

	if (! loaded)
	{
		if (! SeqCodeAsnLoad())
			return smtp;
	}

	if (aip == NULL)
		return smtp;

	if (orig == NULL)           /* SeqMapTable ::= (self contained) */
		atp = AsnReadId(aip, amp, SEQ_MAP_TABLE);
	else
		atp = AsnLinkType(orig, SEQ_MAP_TABLE);    /* link in local tree */
    if (atp == NULL) return smtp;

    smtp = SeqMapTableNew();
    if (smtp == NULL) goto erret;
    
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* read the start struct */
    
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    smtp->from = (Uint1) av.intvalue;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    smtp->to = (Uint1) av.intvalue;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    num = (Uint1)av.intvalue;
    smtp->num = num;
    ipnt = (Uint1Ptr)MemNew((num * sizeof(Uint1)));
    if (ipnt == NULL) goto erret;
    smtp->table = ipnt;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;    /* start SEQUENCE OF */
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    if (atp == SEQ_MAP_TABLE_start_at)
    {
        smtp->start_at = (Uint1)av.intvalue;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;    /* start SEQUENCE OF */
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    }
    i = 0;
    while ((atp = AsnReadId(aip, amp, atp)) == SEQ_MAP_TABLE_table_E)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        if (i < num)
            *ipnt = (Uint1)av.intvalue;
        else
		{
            ErrPost(CTX_NCBIOBJ, 1, "Too many codes in Seq-map-table. line %ld",
                aip->linenumber);
			goto erret;
		}
        ipnt++; i++;
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end SEQUENCE OF */
    if (i != num)
	{
        ErrPost(CTX_NCBIOBJ, 1, "Too few codes in Seq-map-table. line %ld",
            aip->linenumber);
		goto erret;
	}

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;   /* end struct */
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return smtp;
erret:
    smtp = SeqMapTableFree(smtp);
    goto ret;
}

/*****************************************************************************
*
*   SeqMapTablePtr SeqMapTableFind(to, from)
*
*****************************************************************************/
SeqMapTablePtr LIBCALL SeqMapTableFind (Uint1 to, Uint1 from)
{
    SeqMapTablePtr smtp=NULL;

    if (scspl == NULL)
    {
        if ((scspl = SeqCodeSetLoad()) == NULL)
            return smtp;
    }

    smtp = scspl->maps;
    while (smtp != NULL)
    {
        if ((smtp->to == to) && (smtp->from == from))
            return smtp;
        smtp = smtp->next;
    }
    return smtp;
}

/*****************************************************************************
*
*   SeqCodeTableNew()
*
*****************************************************************************/
SeqCodeTablePtr LIBCALL SeqCodeTableNew (void)
{
    return (SeqCodeTablePtr)MemNew(sizeof(SeqCodeTable));
}

/*****************************************************************************
*
*   SeqCodeTableFree(sctp)
*       Frees a SeqCodeTable and associated data
*
*****************************************************************************/
SeqCodeTablePtr LIBCALL SeqCodeTableFree (SeqCodeTablePtr sctp)
{
    CharPtr PNTR tmp;
    Uint1 num, i;

    if (sctp == NULL)
        return sctp;

    MemFree(sctp->letters);
    num = sctp->num;
    if (sctp->symbols != NULL)
    {
        tmp = sctp->symbols;
        for (i = 0; i < num; i++, tmp++)
            MemFree(*tmp);
        MemFree(sctp->symbols);
    }
    if (sctp->names != NULL)
    {
        tmp = sctp->names;
        for (i = 0; i < num; i++, tmp++)
            MemFree(*tmp);
        MemFree(sctp->names);
    }
    if (sctp->comps != NULL)
        MemFree(sctp->comps);
	return (SeqCodeTablePtr)MemFree(sctp);
}

/*****************************************************************************
*
*   SeqCodeTableAsnWrite(sctp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SeqCodeTable ::=)
*
*****************************************************************************/
Boolean LIBCALL SeqCodeTableAsnWrite (SeqCodeTablePtr sctp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Uint1 i, num;
    Char tbuf[2];
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqCodeAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, SEQ_CODE_TABLE);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (sctp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)sctp)) goto erret;
    
    av.intvalue = sctp->code;
    if (! AsnWrite(aip, SEQ_CODE_TABLE_code, &av)) goto erret;
    num = sctp->num;
    av.intvalue = (Int4)num;
    if (! AsnWrite(aip, SEQ_CODE_TABLE_num, &av)) goto erret;
    av.boolvalue = sctp->one_letter;
    if (! AsnWrite(aip, SEQ_CODE_TABLE_one_letter, &av)) goto erret;
    if (sctp->start_at)
    {
        av.intvalue = (Int4)sctp->start_at;
        if (! AsnWrite(aip, SEQ_CODE_TABLE_start_at, &av)) goto erret;
    }

    if (! AsnOpenStruct(aip, SEQ_CODE_TABLE_table, (Pointer)sctp)) goto erret;
    tbuf[1] = '\0';

    for (i = 0; i < num; i++)
    {
        if (! AsnOpenStruct(aip, SEQ_CODE_TABLE_table_E, (Pointer)sctp)) goto erret;
        if (sctp->one_letter)
        {
            tbuf[0] = sctp->letters[i];
            av.ptrvalue = tbuf;
        }
        else
            av.ptrvalue = sctp->symbols[i];
        if (! AsnWrite(aip, SEQ_CODE_TABLE_table_E_symbol, &av)) goto erret;

        av.ptrvalue = sctp->names[i];
        if (! AsnWrite(aip, SEQ_CODE_TABLE_table_E_name, &av)) goto erret;
        if (! AsnCloseStruct(aip, SEQ_CODE_TABLE_table_E, (Pointer)sctp)) goto erret;
    }
    if (! AsnCloseStruct(aip, SEQ_CODE_TABLE_table, (Pointer)sctp)) goto erret;

    if (sctp->comps != NULL)
    {
        if (! AsnOpenStruct(aip, SEQ_CODE_TABLE_comps, (Pointer)sctp->comps)) goto erret;
        for (i = 0; i < num; i++)
        {
            av.intvalue = (Int4)sctp->comps[i];
            if (! AsnWrite(aip, SEQ_CODE_TABLE_comps_E, &av)) goto erret;
        }
        if (! AsnCloseStruct(aip, SEQ_CODE_TABLE_comps, (Pointer)sctp->comps)) goto erret;
    }
    if (! AsnCloseStruct(aip, atp, (Pointer)sctp)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqCodeTableAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
SeqCodeTablePtr LIBCALL SeqCodeTableAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    SeqCodeTablePtr sctp=NULL;
    Uint1 i, num;

	if (! loaded)
	{
		if (! SeqCodeAsnLoad())
			return sctp;
	}

	if (aip == NULL)
		return sctp;

	if (orig == NULL)           /* SeqCodeTable ::= (self contained) */
		atp = AsnReadId(aip, amp, SEQ_CODE_TABLE);
	else
		atp = AsnLinkType(orig, SEQ_CODE_TABLE);    /* link in local tree */
    if (atp == NULL) return sctp;

    sctp = SeqCodeTableNew();
    if (sctp == NULL) goto erret;
    
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* read the start struct */
    
    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    sctp->code = (Uint1) av.intvalue;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    num = (Uint1)av.intvalue;
    sctp->num = num;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    sctp->one_letter = av.boolvalue;

    if (sctp->one_letter)
    {
        sctp->letters = (CharPtr)MemNew((num * sizeof(Char)));
        if (sctp->letters == NULL) goto erret;
    }
    else
    {
        sctp->symbols = (CharPtr PNTR)MemNew((num * sizeof(CharPtr)));
        if (sctp->symbols == NULL) goto erret;
    }
	sctp->names = (CharPtr PNTR)MemNew((num * sizeof(CharPtr)));
    if (sctp->names == NULL) goto erret;

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;    /* start SEQUENCE OF */
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    if (atp == SEQ_CODE_TABLE_start_at)
    {
        sctp->start_at = (Uint1)av.intvalue;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;    /* start SEQUENCE OF */
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    }
    i = 0;
    while ((atp = AsnReadId(aip, amp, atp)) == SEQ_CODE_TABLE_table_E)
    {
        if (i >= num)
		{
            ErrPost(CTX_NCBIOBJ, 1, "Too many codes in Seq-code-table %s. line %ld",
                sctp->code, aip->linenumber);
			goto erret;
		}
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* start struct */
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* symbol */
        if (sctp->one_letter)
        {
            sctp->letters[i] = *(CharPtr)av.ptrvalue;
            MemFree(av.ptrvalue);
        }
        else
            sctp->symbols[i] = (CharPtr)av.ptrvalue;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;   /* name */
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        sctp->names[i] = (CharPtr)av.ptrvalue;
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;   /* end struct */
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        i++;
    }
    if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end SEQUENCE OF */
    if (i != num)
	{
        ErrPost(CTX_NCBIOBJ, 1, "Too few codes in Seq-code-table %s. line %ld",
            sctp->code, aip->linenumber);
		goto erret;
	}

    atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    if (atp == SEQ_CODE_TABLE_comps)   /* comps present */
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        sctp->comps = (Uint1Ptr)MemNew((num * sizeof(Uint1)));
        if (sctp->comps == NULL) goto erret;
        i = 0;
        while ((atp = AsnReadId(aip, amp, atp)) == SEQ_CODE_TABLE_comps_E)
        {
            if (i == num)
			{
                ErrPost(CTX_NCBIOBJ, 1, "Too many comps in Seq-code-table. line %ld",
                    aip->linenumber);
				goto erret;
			}
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
            sctp->comps[i] = (Uint1)av.intvalue;
            i++;
        }
        if (i != num)
		{
            ErrPost(CTX_NCBIOBJ, 1, "Too few comps in Seq-code-table. line %ld",
                aip->linenumber);
			goto erret;
		}
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end sequence of */
        atp = AsnReadId(aip, amp, atp); if (atp == NULL) goto erret;
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return sctp;
erret:
    sctp = SeqCodeTableFree(sctp);
    goto ret;
}

/*****************************************************************************
*
*   SeqCodeTablePtr SeqCodeTableFind(code)
*
*****************************************************************************/
SeqCodeTablePtr LIBCALL SeqCodeTableFind (Uint1 code)
{
    SeqCodeTablePtr sctp=NULL;

    if (scspl == NULL)
    {
        if ((scspl = SeqCodeSetLoad()) == NULL)
            return sctp;
    }

    sctp = scspl->codes;
    while (sctp != NULL)
    {
        if (sctp->code == code)
            return sctp;
        sctp = sctp->next;
    }
    return sctp;
}

/*****************************************************************************
*
*   SeqCodeSetNew()
*
*****************************************************************************/
SeqCodeSetPtr LIBCALL SeqCodeSetNew (void)
{
    return (SeqCodeSetPtr)MemNew(sizeof(SeqCodeSet));
}

/*****************************************************************************
*
*   SeqCodeSetFree(scsp)
*       Frees a SeqCodeSet and associated data
*
*****************************************************************************/
SeqCodeSetPtr LIBCALL SeqCodeSetFree (SeqCodeSetPtr scsp)
{
    SeqCodeTablePtr sctp, sctpnext;
    SeqMapTablePtr smtp, smtpnext;

    sctp = scsp->codes;
    while (sctp != NULL)
    {
        sctpnext = sctp->next;
        SeqCodeTableFree(sctp);
        sctp = sctpnext;
    }
    smtp = scsp->maps;
    while (smtp != NULL)
    {
        smtpnext = smtp->next;
        SeqMapTableFree(smtp);
        smtp = smtpnext;
    }
	return (SeqCodeSetPtr)MemFree(scsp);
}

/*****************************************************************************
*
*   SeqCodeSetAsnWrite(scsp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SeqCodeSet ::=)
*
*****************************************************************************/
Boolean LIBCALL SeqCodeSetAsnWrite (SeqCodeSetPtr scsp, AsnIoPtr aip, AsnTypePtr orig)
{
	AsnTypePtr atp;
    SeqMapTablePtr smtp;
    SeqCodeTablePtr sctp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SeqCodeAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, SEQ_CODE_SET);   /* link local tree */
    if (atp == NULL) return FALSE;

	if (scsp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)scsp)) goto erret;

    if (scsp->codes != NULL)
    {
        if (! AsnOpenStruct(aip, SEQ_CODE_SET_codes, (Pointer)scsp->codes)) goto erret;
        sctp = scsp->codes;
        while (sctp != NULL)
        {
            if (! SeqCodeTableAsnWrite(sctp, aip, SEQ_CODE_SET_codes_E)) goto erret;
            sctp = sctp->next;
        }
        if (! AsnCloseStruct(aip, SEQ_CODE_SET_codes, (Pointer)scsp->codes)) goto erret;
    }

    if (scsp->maps != NULL)
    {
        if (! AsnOpenStruct(aip, SEQ_CODE_SET_maps, (Pointer)scsp->maps)) goto erret;
        smtp = scsp->maps;
        while (smtp != NULL)
        {
            if (! SeqMapTableAsnWrite(smtp, aip, SEQ_CODE_SET_maps_E)) goto erret;
            smtp = smtp->next;
        }
        if (! AsnCloseStruct(aip, SEQ_CODE_SET_maps, (Pointer)scsp->maps)) goto erret;
    }

    if (! AsnCloseStruct(aip, atp, (Pointer)scsp)) goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqCodeSetAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
SeqCodeSetPtr LIBCALL SeqCodeSetAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    SeqCodeSetPtr scsp=NULL;
    SeqMapTablePtr map, currmap = NULL;
    SeqCodeTablePtr code, currcode = NULL;

	if (! loaded)
	{
		if (! SeqCodeAsnLoad())
			return scsp;
	}

	if (aip == NULL)
		return scsp;

	if (orig == NULL)           /* SeqCodeSet ::= (self contained) */
		atp = AsnReadId(aip, amp, SEQ_CODE_SET);
	else
		atp = AsnLinkType(orig, SEQ_CODE_SET);    /* link in local tree */
    oldatp = atp;
	if (atp == NULL) return scsp;

    scsp = SeqCodeSetNew();
	if (scsp == NULL) goto erret;
    
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* read the start struct */
    
    while((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
		if (atp == NULL) goto erret;
        if (atp == SEQ_CODE_SET_codes_E)
        {
            code = SeqCodeTableAsnRead(aip, atp);
			if (code == NULL) goto erret;
            if (scsp->codes == NULL)
                scsp->codes = code;
            else
                currcode->next = code;
            currcode = code;
        }
        else if (atp == SEQ_CODE_SET_maps_E)
        {
            map = SeqMapTableAsnRead(aip, atp);
			if (map == NULL) goto erret;
            if (scsp->maps == NULL)
                scsp->maps = map;
            else
                currmap->next = map;
            currmap = map;
        }
        else                 /* the other struct ends */
		{
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
		}
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;  /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return scsp;
erret:
	scsp = SeqCodeSetFree(scsp);
	goto ret;
}

/*****************************************************************************
*
*   SeqCodeSetPtr SeqCodeSetLoad()
*       loads all current seqcodes
*       looks for "seqcode.val" in the "data" directory
*
*****************************************************************************/
SeqCodeSetPtr LIBCALL SeqCodeSetLoad (void)
{
    Char buf[80];
    AsnIoPtr aip;

	if (scspl != NULL)
		return scspl;

    if (! FindPath("ncbi", "ncbi", "data", buf, sizeof (buf)))
	{
		ErrPost(CTX_NCBIOBJ, 1, "FindPath failed");
        return scspl;
	}

    StringCat(buf, "seqcode.val");
    if ((aip = AsnIoOpen(buf, "rb")) == NULL)
	{
		ErrPost(CTX_NCBIOBJ, 1, "Couldn't open [%s]", buf);
        return scspl;
	}

    scspl = SeqCodeSetAsnRead(aip, NULL);

    AsnIoClose(aip);
    return scspl;
}


