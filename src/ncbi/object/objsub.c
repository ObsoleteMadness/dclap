/*  objsub.c
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
* File Name:  objsub.c
*
* Author:  James Ostell
*   
* Version Creation Date: 1/1/91
*
* $Revision: 2.5 $
*
* File Description:  Object manager for module NCBI-Submit
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
#include <asnsubmt.h>        /* the AsnTool header */
#include <objsub.h>		   /* the general objects interface */

static Boolean loaded = FALSE;

/*****************************************************************************
*
*   SubmitAsnLoad()
*
*****************************************************************************/
Boolean LIBCALL SubmitAsnLoad ()
{
    if (loaded)
        return TRUE;

	if (! SeqSetAsnLoad ())
		return FALSE;
		
    if (AsnLoad())
        loaded = TRUE;

    return loaded;
}

/*****************************************************************************
*
*   SeqSubmit Routines
*
*****************************************************************************/


/*****************************************************************************
*
*   SeqSubmitNew()
*
*****************************************************************************/
SeqSubmitPtr LIBCALL SeqSubmitNew ()
{
	return (SeqSubmitPtr)MemNew(sizeof(SeqSubmit));
}
/*****************************************************************************
*
*   SeqSubmitFree(ssp)
*
*****************************************************************************/
SeqSubmitPtr LIBCALL SeqSubmitFree (SeqSubmitPtr ssp)
{
	SeqEntryPtr sep, sepnext;
	SeqAnnotPtr sap, sapnext;

	if (ssp == NULL)
		return ssp;

	SubmitBlockFree(ssp->sub);
	switch	(ssp->datatype)
	{
		case 1:
			sep = (SeqEntryPtr)ssp->data;
			while (sep != NULL)
			{
				sepnext = sep->next;
				SeqEntryFree(sep);
				sep = sepnext;
			}
			break;
		case 2:
			sap = (SeqAnnotPtr)ssp->data;
			while (sap != NULL)
			{
				sapnext = sap->next;
				SeqAnnotFree(sap);
				sap = sapnext;
			}
			break;
		case 3:
			SeqIdSetFree((SeqIdPtr)ssp->data);
			break;
	}
	return (SeqSubmitPtr)MemFree(ssp);
}

/*****************************************************************************
*
*   SeqSubmitAsnWrite(ssp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SeqSubmit ::=)
*
*****************************************************************************/
Boolean LIBCALL SeqSubmitAsnWrite (SeqSubmitPtr ssp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;
	SeqEntryPtr sep;
	SeqAnnotPtr sap;

	if (! loaded)
	{
		if (! SubmitAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, SEQ_SUBMIT);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (ssp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)ssp))
        goto erret;

	if (! SubmitBlockAsnWrite(ssp->sub, aip, SEQ_SUBMIT_sub)) goto erret;

	av.ptrvalue = ssp->data;
    if (! AsnWriteChoice(aip, SEQ_SUBMIT_data, (Int2)ssp->datatype, &av)) goto erret;

	switch(ssp->datatype)
	{
		case 1:
			if (! AsnOpenStruct(aip, SEQ_SUBMIT_data_entrys, ssp->data)) goto erret;
			sep = (SeqEntryPtr) ssp->data;
			while (sep != NULL)
			{
				if (! SeqEntryAsnWrite(sep, aip, SEQ_SUBMIT_data_entrys_E)) goto erret;
				sep = sep->next;
			}
			if (! AsnCloseStruct(aip, SEQ_SUBMIT_data_entrys, ssp->data)) goto erret;
			break;
		case 2:
			if (! AsnOpenStruct(aip, SEQ_SUBMIT_data_annots, ssp->data)) goto erret;
			sap = (SeqAnnotPtr) ssp->data;
			while (sap != NULL)
			{
				if (! SeqAnnotAsnWrite(sap, aip, SEQ_SUBMIT_data_annots_E)) goto erret;
				sap = sap->next;
			}
			if (! AsnCloseStruct(aip, SEQ_SUBMIT_data_annots, ssp->data)) goto erret;
			break;
		case 3:
			if (! SeqIdSetAsnWrite((SeqIdPtr)ssp->data, aip, SEQ_SUBMIT_data_delete,
				SEQ_SUBMIT_data_delete_E)) goto erret;
			break;
		default:
			ErrPost(CTX_NCBIOBJ, 1, "Unknown Seq-submit type = %d", (int) ssp->datatype);
			goto erret;
	}
    if (! AsnCloseStruct(aip, atp, (Pointer)ssp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SeqSubmitAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
SeqSubmitPtr LIBCALL SeqSubmitAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    SeqSubmitPtr ssp=NULL;
	SeqEntryPtr sep, seplast = NULL;
	SeqAnnotPtr sap, saplast = NULL;
	SeqIdPtr sip, siplast = NULL;

	if (! loaded)
	{
		if (! SubmitAsnLoad())
			return ssp;
	}

	if (aip == NULL)
		return ssp;

	if (orig == NULL)           /* SeqSubmit ::= (self contained) */
		atp = AsnReadId(aip, amp, SEQ_SUBMIT);
	else
		atp = AsnLinkType(orig, SEQ_SUBMIT);    /* link in local tree */
    if (atp == NULL)
        return ssp;

	ssp = SeqSubmitNew();
    if (ssp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0)    /* read the start struct */
        goto erret;

	atp = AsnReadId(aip, amp, atp);  /* find the submission-block */
    if (atp == NULL) goto erret;
	ssp->sub = SubmitBlockAsnRead(aip, atp);
	if (ssp->sub == NULL) goto erret;

    atp = AsnReadId(aip, amp, atp);  /* read the CHOICE */
    if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;

    atp = AsnReadId(aip, amp, atp);  /* read the data */
    if (atp == NULL) goto erret;
	oldatp = atp;     /* the SET OF */
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;

    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
		if (atp == SEQ_SUBMIT_data_entrys_E)
		{
			sep = SeqEntryAsnRead(aip, atp);
			if (sep == NULL) goto erret;
			if (seplast == NULL)
			{
				ssp->data = (Pointer) sep;
				ssp->datatype = 1;
			}
			else
				seplast->next = sep;
			seplast = sep;
		}
		else if (atp == SEQ_SUBMIT_data_annots_E)
		{
			sap = SeqAnnotAsnRead(aip, atp);
			if (sap == NULL) goto erret;
			if (saplast == NULL)
			{
				ssp->data = (Pointer) sap;
				ssp->datatype = 2;
			}
			else
				saplast->next = sap;
			saplast = sap;
		}
		else if (atp == SEQ_SUBMIT_data_delete_E)
		{
			sip = SeqIdAsnRead(aip, atp);
			if (sip == NULL) goto erret;
			if (siplast == NULL)
			{
				ssp->data = (Pointer) sip;
				ssp->datatype = 3;
			}
			else
				siplast->next = sip;
			siplast = sip;
		}
		else
			goto erret;
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end set of */

    atp = AsnReadId(aip, amp, atp);
    if (atp == NULL) goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;  /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return ssp;
erret:
    ssp = SeqSubmitFree(ssp);
    goto ret;
}

/*****************************************************************************
*
*   SubmitBlockNew()
*
*****************************************************************************/
SubmitBlockPtr LIBCALL SubmitBlockNew (void)
{
	return (SubmitBlockPtr)MemNew(sizeof(SubmitBlock));
}

/*****************************************************************************
*
*   SubmitBlockFree(sbp)
*
*****************************************************************************/
SubmitBlockPtr LIBCALL SubmitBlockFree (SubmitBlockPtr sbp)
{
	if (sbp == NULL)
		return sbp;
	ContactInfoFree(sbp->contact);
	CitSubFree(sbp->cit);
	DateFree(sbp->reldate);
	return (SubmitBlockPtr)MemFree(sbp);
}

/*****************************************************************************
*
*   SubmitBlockAsnWrite(sbp, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (SubmitBlock ::=)
*
*****************************************************************************/
Boolean LIBCALL SubmitBlockAsnWrite (SubmitBlockPtr sbp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! SubmitAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, SUBMIT_BLOCK);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (sbp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)sbp))
        goto erret;

	if (! ContactInfoAsnWrite(sbp->contact, aip, SUBMIT_BLOCK_contact))
		goto erret;

	if (! CitSubAsnWrite(sbp->cit, aip, SUBMIT_BLOCK_cit)) goto erret;

	if (sbp->hup)
	{
		av.boolvalue = sbp->hup;
		if (! AsnWrite(aip, SUBMIT_BLOCK_hup, &av)) goto erret;
	}

	if (sbp->reldate != NULL)
	{
		if (! DateAsnWrite(sbp->reldate, aip, SUBMIT_BLOCK_reldate))
			goto erret;
	}

    if (! AsnCloseStruct(aip, atp, (Pointer)sbp))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   SubmitBlockAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
SubmitBlockPtr LIBCALL SubmitBlockAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    SubmitBlockPtr sbp=NULL;

	if (! loaded)
	{
		if (! SubmitAsnLoad())
			return sbp;
	}

	if (aip == NULL)
		return sbp;

	if (orig == NULL)           /* SubmitBlock ::= (self contained) */
		atp = AsnReadId(aip, amp, SUBMIT_BLOCK);
	else
		atp = AsnLinkType(orig, SUBMIT_BLOCK);    /* link in local tree */
    if (atp == NULL)
        return sbp;

	sbp = SubmitBlockNew();
    if (sbp == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0)    /* read the start struct */
        goto erret;

	atp = AsnReadId(aip, amp, atp);  /* contact-info */
	if (atp == NULL) goto erret;
	sbp->contact = ContactInfoAsnRead(aip, atp);
	if (sbp->contact == NULL) goto erret;

	atp = AsnReadId(aip, amp, atp);  /* cit-sub */
	if (atp == NULL) goto erret;
	sbp->cit = CitSubAsnRead(aip, atp);
	if (sbp->cit == NULL) goto erret;

	atp = AsnReadId(aip, amp, atp);  /* hup? */
	if (atp == NULL) goto erret;

	if (atp == SUBMIT_BLOCK_hup)
	{
		if (AsnReadVal(aip, atp, &av) <= 0)     /* get the hup */
    	    goto erret;
		sbp->hup = av.boolvalue;

		atp = AsnReadId(aip, amp, atp);  /* reldate? */
		if (atp == NULL) goto erret;
	}

	if (atp == SUBMIT_BLOCK_reldate)
	{
		sbp->reldate = DateAsnRead(aip, atp);
		if (sbp->reldate == NULL) goto erret;

		atp = AsnReadId(aip, amp, atp);  /* end struct */
		if (atp == NULL) goto erret;
	}

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;  /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return sbp;
erret:
    sbp = SubmitBlockFree(sbp);
    goto ret;
}

/*****************************************************************************
*
*   ContactInfoNew()
*
*****************************************************************************/
ContactInfoPtr LIBCALL ContactInfoNew (void)
{
	return (ContactInfoPtr)MemNew(sizeof(ContactInfo));
}

/*****************************************************************************
*
*   ContactInfoFree(cip)
*
*****************************************************************************/
ContactInfoPtr LIBCALL ContactInfoFree (ContactInfoPtr cip)
{
	if (cip == NULL)
		return cip;
	MemFree(cip->name);
	ValNodeFreeData(cip->address);
	MemFree(cip->phone);
	MemFree(cip->fax);
	MemFree(cip->email);
	MemFree(cip->telex);
	ObjectIdFree(cip->owner_id);
	BSFree(cip->password);
	return (ContactInfoPtr)MemFree(cip);
}

/*****************************************************************************
*
*   ContactInfoAsnWrite(cip, aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*       if atp == NULL, then assumes it stands alone (ContactInfo ::=)
*
*****************************************************************************/
Boolean LIBCALL ContactInfoAsnWrite (ContactInfoPtr cip, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;
	ValNodePtr vnp;

	if (! loaded)
	{
		if (! SubmitAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, CONTACT_INFO);   /* link local tree */
    if (atp == NULL)
        return FALSE;

	if (cip == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)cip))
        goto erret;

	av.ptrvalue = cip->name;
	if (! AsnWrite(aip, CONTACT_INFO_name, &av)) goto erret;

	vnp = cip->address;
    if (! AsnOpenStruct(aip, CONTACT_INFO_address, (Pointer)vnp))
        goto erret;
	while (vnp != NULL)
	{
		if (! AsnWrite(aip, CONTACT_INFO_address_E, &vnp->data)) goto erret;
		vnp = vnp->next;
	}
    if (! AsnCloseStruct(aip, CONTACT_INFO_address, (Pointer)vnp))
        goto erret;

	if (cip->phone != NULL)
	{
		av.ptrvalue = cip->phone;
		if (! AsnWrite(aip, CONTACT_INFO_phone, &av)) goto erret;
	}

	if (cip->fax != NULL)
	{
		av.ptrvalue = cip->fax;
		if (! AsnWrite(aip, CONTACT_INFO_fax, &av)) goto erret;
	}

	if (cip->email != NULL)
	{
		av.ptrvalue = cip->email;
		if (! AsnWrite(aip, CONTACT_INFO_email, &av)) goto erret;
	}

	if (cip->telex != NULL)
	{
		av.ptrvalue = cip->telex;
		if (! AsnWrite(aip, CONTACT_INFO_telex, &av)) goto erret;
	}

	if (cip->owner_id != NULL)
	{
		if (! ObjectIdAsnWrite(cip->owner_id, aip, CONTACT_INFO_owner_id)) goto erret;
	}

	if (cip->password != NULL)
	{
		av.ptrvalue = cip->password;
		if (! AsnWrite(aip, CONTACT_INFO_password, &av)) goto erret;
	}
	
    if (! AsnCloseStruct(aip, atp, (Pointer)cip))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return retval;
}

/*****************************************************************************
*
*   ContactInfoAsnRead(aip, atp)
*   	atp is the current type (if identifier of a parent struct)
*            assumption is readIdent has occured
*       if atp == NULL, then assumes it stands alone and read ident
*            has not occured.
*
*****************************************************************************/
ContactInfoPtr LIBCALL ContactInfoAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, oldatp;
    ContactInfoPtr cip=NULL;
	ValNodePtr vnp = NULL;

	if (! loaded)
	{
		if (! SubmitAsnLoad())
			return cip;
	}

	if (aip == NULL)
		return cip;

	if (orig == NULL)           /* ContactInfo ::= (self contained) */
		atp = AsnReadId(aip, amp, CONTACT_INFO);
	else
		atp = AsnLinkType(orig, CONTACT_INFO);    /* link in local tree */
    if (atp == NULL)
        return cip;
	oldatp = atp;

	cip = ContactInfoNew();
    if (cip == NULL)
        goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0)    /* read the start struct */
        goto erret;

	atp = AsnReadId(aip, amp, atp);  /* name */
	if (atp == NULL) goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)    /* read the name */
        goto erret;
	cip->name = (CharPtr)av.ptrvalue;

	atp = AsnReadId(aip, amp, atp);  /* address */
	if (atp == NULL) goto erret;
	if (AsnReadVal(aip, atp, &av) <= 0)    /* read the start struct */
        goto erret;
	while ((atp = AsnReadId(aip, amp, atp)) == CONTACT_INFO_address_E)
	{
		vnp = ValNodeNew(vnp);
		if (AsnReadVal(aip, atp, &vnp->data) <= 0)    /* read the string */
    	    goto erret;
		if (cip->address == NULL)
			cip->address = vnp;
	}
	if (AsnReadVal(aip, atp, &av) <= 0)    /* read the end struct */
        goto erret;

	while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
	{
		if (atp == NULL) goto erret;
		if (atp == CONTACT_INFO_owner_id)
		{
			cip->owner_id = ObjectIdAsnRead(aip, atp);
			if (cip->owner_id == NULL) goto erret;
		}
		else
		{
			if (AsnReadVal(aip, atp, &av) <= 0)    /* read the data */
    	    	goto erret;
			if (atp == CONTACT_INFO_phone)
				cip->phone = (CharPtr)av.ptrvalue;
			else if (atp == CONTACT_INFO_fax)
				cip->fax = (CharPtr)av.ptrvalue;
			else if (atp == CONTACT_INFO_email)
				cip->email = (CharPtr)av.ptrvalue;
			else if (atp == CONTACT_INFO_telex)
				cip->telex = (CharPtr)av.ptrvalue;
			else if (atp == CONTACT_INFO_password)
				cip->password = (ByteStorePtr)av.ptrvalue;
		}
	}

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;  /* end struct */
ret:
	AsnUnlinkType(orig);       /* unlink local tree */
	return cip;
erret:
    cip = ContactInfoFree(cip);
    goto ret;
}

