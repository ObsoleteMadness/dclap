/*  objbibli.c
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
* File Name:  objbibli.c
*
* Author:  James Ostell
*   
* Version Creation Date: 1/1/91
*
* $Revision: 2.10 $
*
* File Description:  Object manager for module NCBI-Biblio
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 05-13-93 Schuler     All public functions are now declared LIBCALL.
* 07-19-93 Ostell      Support for ASN30 added
*
* ==========================================================================
*/
#include <asnbibli.h>
#include <objbibli.h>


static Boolean loaded = FALSE;

/*****************************************************************************
*
*   Bibliographic Object routines
*
*****************************************************************************/

/*****************************************************************************
*
*   BiblioAsnLoad()
*
*****************************************************************************/
Boolean LIBCALL BiblioAsnLoad ()
{
    if (loaded)
        return TRUE;
    loaded = TRUE;

    if (! GeneralAsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    if (! AsnLoad())
    {
        loaded = FALSE;
        return FALSE;
    }
    return TRUE;
}

/*****************************************************************************
*
*   AffilNew()
*
*****************************************************************************/
AffilPtr LIBCALL AffilNew (void)
{
	AffilPtr afp;

	afp = (AffilPtr)MemNew(sizeof(Affil));
	return afp;
}

/*****************************************************************************
*
*   AffilFree()
*
*****************************************************************************/
AffilPtr LIBCALL AffilFree (AffilPtr afp)
{
	if (afp == NULL)
		return afp;
	MemFree(afp->affil);
	MemFree(afp->div);
	MemFree(afp->city);
	MemFree(afp->sub);
	MemFree(afp->country);
	MemFree(afp->street);
	return (AffilPtr)MemFree(afp);
}

/*****************************************************************************
*
*   AffilAsnRead(aip, atp)
*
*****************************************************************************/
AffilPtr LIBCALL AffilAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	AffilPtr afp=NULL;
	DataVal av;
	AsnTypePtr atp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return afp;
	}

	if (aip == NULL)
		return afp;

	if (orig == NULL)           /* Affil ::= */
		atp = AsnReadId(aip, amp, AFFIL);
	else
		atp = AsnLinkType(orig, AFFIL);
    if (atp == NULL)
        return afp;

	afp = AffilNew();
    if (afp == NULL)
        goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the CHOICE */
    atp = AsnReadId(aip, amp, atp);   /* read the CHOICE id */
    if (atp == NULL)
        goto erret;

    if (atp == AFFIL_str)         /* just the VisibleString */
    {
        afp->choice = 1;    /* choice type is str */
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* get the string */
        afp->affil = (CharPtr)av.ptrvalue;    /* keep it */
    }
    else                          /* affiliation structure */
    {
        afp->choice = 2;     /* choice type is std */
	    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the START_STRUCT */

        while ((atp = AsnReadId(aip, amp, atp)) != AFFIL_std)
        {
            if (atp == NULL)
                goto erret;
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
            if (atp == AFFIL_std_affil)
                afp->affil = (CharPtr)av.ptrvalue;
            else if (atp == AFFIL_std_div)
                afp->div = (CharPtr)av.ptrvalue;
            else if (atp == AFFIL_std_city)
                afp->city = (CharPtr)av.ptrvalue;
            else if (atp == AFFIL_std_sub)
                afp->sub = (CharPtr)av.ptrvalue;
            else if (atp == AFFIL_std_country)
                afp->country = (CharPtr)av.ptrvalue;
			else if (atp == AFFIL_std_street)
				afp->street = (CharPtr)av.ptrvalue;
        }
	    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the END STRUCT */
    }
ret:
	AsnUnlinkType(orig);
	return afp;
erret:
    afp = AffilFree(afp);
    goto ret;
}

/*****************************************************************************
*
*   AffilAsnWrite(afp, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL AffilAsnWrite (AffilPtr afp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, AFFIL);
    if (atp == NULL)
        return FALSE;

	if (afp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnWrite(aip, atp, &av))   /* write the tag */
        goto erret;

    if (afp->choice == 1)      /* string */
    {
        av.ptrvalue = afp->affil;
        if (! AsnWrite(aip, AFFIL_str, &av))  /* write the string */
            goto erret;
    }
    else                      /* std */
    {
    	if (! AsnOpenStruct(aip, AFFIL_std, (Pointer)afp))
            goto erret;
        if (afp->affil != NULL)
        {
            av.ptrvalue = afp->affil;
            if (! AsnWrite(aip, AFFIL_std_affil, &av))
                goto erret;
        }
        if (afp->div != NULL)
        {
            av.ptrvalue = afp->div;
            if (! AsnWrite(aip, AFFIL_std_div, &av)) goto erret;
        }
        if (afp->city != NULL)
        {
            av.ptrvalue = afp->city;
            if (! AsnWrite(aip, AFFIL_std_city, &av)) goto erret;
        }
        if (afp->sub != NULL)
        {
            av.ptrvalue = afp->sub;
            if (! AsnWrite(aip, AFFIL_std_sub, &av)) goto erret;
        }
        if (afp->country != NULL)
        {
            av.ptrvalue = afp->country;
            if (! AsnWrite(aip, AFFIL_std_country, &av)) goto erret;
        }
        if (afp->street != NULL)
        {
            av.ptrvalue = afp->street;
            if (! AsnWrite(aip, AFFIL_std_street, &av)) goto erret;
        }

    	if (! AsnCloseStruct(aip, AFFIL_std, (Pointer)afp))
            goto erret;
    }
    retval = TRUE;
erret:
	AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   AuthListNew()
*
*****************************************************************************/
AuthListPtr LIBCALL AuthListNew (void)
{
	AuthListPtr alp;

	alp = (AuthListPtr)MemNew(sizeof(AuthList));
	return alp;
}

/*****************************************************************************
*
*   AuthListFree()
*
*****************************************************************************/
AuthListPtr LIBCALL AuthListFree (AuthListPtr alp)
{
    ValNodePtr curr, next;

    if (alp == NULL)
        return alp;

    curr = alp->names;
    while (curr != NULL)
    {
        if (alp->choice == 1)    /* std type */
            AuthorFree((AuthorPtr) curr->data.ptrvalue);
        else                      /* ml or str */
            MemFree(curr->data.ptrvalue);
        next = curr->next;
        MemFree(curr);
        curr = next;
    }
    if (alp->affil != NULL)
        AffilFree(alp->affil);
	return (AuthListPtr)MemFree(alp);
}

/*****************************************************************************
*
*   AuthListAsnRead(aip, atp)
*
*****************************************************************************/
AuthListPtr LIBCALL AuthListAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	AuthListPtr alp=NULL;
	DataVal av;
	AsnTypePtr atp, seqofptr;
    Uint1 choice;
    ValNodePtr anp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return alp;
	}

	if (aip == NULL)
		return alp;

	if (orig == NULL)           /* AuthList ::= */
		atp = AsnReadId(aip, amp, AUTH_LIST);
	else
		atp = AsnLinkType(orig, AUTH_LIST);
    if (atp == NULL)
        return alp;

	alp = AuthListNew();
    if (alp == NULL)
        goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the START STRUCT */
    atp = AsnReadId(aip, amp, atp);   /* read names id */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the CHOICE */
    atp = AsnReadId(aip, amp, atp);   /* read the CHOICE id */
    if (atp == NULL)
        goto erret;
    seqofptr = atp;              /* keep to find end of loop */
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;     /* read the START for SEQUENCE OF */

    if (atp == AUTH_LIST_names_std)         /* Authors */
        choice = 1;    /* choice type is str */
    else if (atp == AUTH_LIST_names_ml)     /* medline */
        choice = 2;
    else                                    /* strings */
        choice = 3;

    alp->choice = choice;

    anp = NULL;
    while ((atp = AsnReadId(aip, amp, atp)) != seqofptr)
    {
        if (atp == NULL)
            goto erret;
        anp = ValNodeNew(anp);      /* add to linked list */
        if (anp == NULL)
            goto erret;
        if (alp->names == NULL)
            alp->names = anp;
        anp->choice = choice;    /* not really necessary */
        if (choice == 1)    /* std */
            anp->data.ptrvalue = (Pointer) AuthorAsnRead(aip, atp);
        else
        {
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;     /* string types */
            anp->data.ptrvalue = av.ptrvalue;
        }
        if (anp->data.ptrvalue == NULL)
            goto erret;
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* SEQ OF end STRUCT */

    atp = AsnReadId(aip, amp, atp);
    if (atp == NULL)
        goto erret;
    if (atp == AUTH_LIST_affil)     /* has an affiliation */
    {
        alp->affil = AffilAsnRead(aip, atp);
        if (alp->affil == NULL)
            goto erret;
        atp = AsnReadId(aip, amp, atp);
        if (atp == NULL)
            goto erret;
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;     /* END STRUCT */
ret:
    AsnUnlinkType(orig);
	return alp;
erret:
    alp = AuthListFree(alp);
    goto ret;
}

/*****************************************************************************
*
*   AuthListAsnWrite(alp, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL AuthListAsnWrite (AuthListPtr alp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, seqofptr, elementptr;
    Int1 choice;
    ValNodePtr anp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, AUTH_LIST);
    if (atp == NULL)
        return FALSE;

	if (alp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)alp))   /* open AuthList SEQUENCE */
        goto erret;

	av.ptrvalue = alp->names;
    if (! AsnWriteChoice(aip, AUTH_LIST_names, (Int2)alp->choice, &av)) goto erret; 

                              /* write CHOICE for SEQUENCE OF */
    choice = alp->choice;
    if (choice == 1 )     /* std */
    {
        seqofptr = AUTH_LIST_names_std;
        elementptr = AUTH_LIST_names_std_E;
    }
    else if (choice == 2)   /* ml */
    {
        seqofptr = AUTH_LIST_names_ml;
        elementptr = AUTH_LIST_names_ml_E;
    }
    else
    {
        seqofptr = AUTH_LIST_names_str;
        elementptr = AUTH_LIST_names_str_E;
    }

    if (! AsnOpenStruct(aip, seqofptr, (Pointer)alp->names)) /* start SEQUENCE OF */
        goto erret;

    anp = alp->names;

    while (anp != NULL)
    {
        if (choice == 1)  /* Author */
        {
            if (! AuthorAsnWrite((AuthorPtr) anp->data.ptrvalue, aip, elementptr))
                goto erret;
        }
        else              /* str or medline */
        {
            av.ptrvalue = anp->data.ptrvalue;
            if (! AsnWrite(aip, elementptr, &av)) goto erret;
        }
        anp = anp->next;
    }

    if (! AsnCloseStruct(aip, seqofptr, (Pointer)alp->names))  /* end SEQUENCE OF */
        goto erret;

    if (alp->affil != NULL)     /* affiliation */
    {
        if (! AffilAsnWrite(alp->affil, aip, AUTH_LIST_affil))
            goto erret;
    }

   	if (! AsnCloseStruct(aip, atp, (Pointer)alp))    /* end AuthList SEQUENCE */
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   AuthorNew()
*
*****************************************************************************/
AuthorPtr LIBCALL AuthorNew (void)
{
	AuthorPtr ap;

	ap = (AuthorPtr)MemNew(sizeof(Author));
	if (ap == NULL) return ap;
	ap->is_corr = (Uint1)255;
	return ap;
}

/*****************************************************************************
*
*   AuthorFree()
*
*****************************************************************************/
AuthorPtr LIBCALL AuthorFree (AuthorPtr ap)
{
    if (ap == NULL)
        return ap;

    PersonIdFree(ap->name);
    if (ap->affil != NULL)
        AffilFree(ap->affil);
	return (AuthorPtr)MemFree(ap);
}

/*****************************************************************************
*
*   AuthorAsnRead(aip, atp)
*
*****************************************************************************/
AuthorPtr LIBCALL AuthorAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	AuthorPtr ap=NULL;
	DataVal av;
	AsnTypePtr atp, oldatp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return ap;
	}

	if (aip == NULL)
		return ap;

	if (orig == NULL)           /* Author ::= */
		atp = AsnReadId(aip, amp, AUTHOR);
	else
		atp = AsnLinkType(orig, AUTHOR);
    oldatp = atp;     /* points to start of SEQUENCE */
    if (atp == NULL)
        return ap;

	ap = AuthorNew();
    if (ap == NULL)
        goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the open SEQUENCE */
    atp = AsnReadId(aip, amp, atp);   /* read the name id */
    if (atp == NULL)
        goto erret;
    ap->name = PersonIdAsnRead(aip, atp);   /* read the PersonId */
    if (ap->name == NULL)
        goto erret;

    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)   /* read SEQUENCE */
    {
        if (atp == NULL)
            goto erret;
        if (atp == AUTHOR_affil)
        {
            ap->affil = AffilAsnRead(aip, atp);
            if (ap->affil == NULL)
                goto erret;
        }
        else
        {
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
            if (atp == AUTHOR_level)
                ap->lr[0] = (Uint1) av.intvalue;
            else if (atp == AUTHOR_role)
                ap->lr[1] = (Uint1) av.intvalue;
			else if (atp == AUTHOR_is_corr)
			{
				if (av.boolvalue)
					ap->is_corr = 1;
				else
					ap->is_corr = 0;
			}
        }
    }

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end of SEQUENCE */
ret:
	AsnUnlinkType(orig);
	return ap;
erret:
    ap = AuthorFree(ap);
    goto ret;
}

/*****************************************************************************
*
*   AuthorAsnWrite(ap, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL AuthorAsnWrite (AuthorPtr ap, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, AUTHOR);
    if (atp == NULL)
        return FALSE;

	if (ap == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)ap))  /* open the SEQUENCE */
        goto erret;

    if (! PersonIdAsnWrite(ap->name, aip, AUTHOR_name))   /* write the name */
        goto erret;

    if (ap->lr[0] != 0)       /* level set */
    {
        av.intvalue = (Int4) ap->lr[0];
        if (! AsnWrite(aip, AUTHOR_level, &av)) goto erret;
    }

    if (ap->lr[1] != 0)      /* role set */
    {
        av.intvalue = (Int4) ap->lr[1];
        if (! AsnWrite(aip, AUTHOR_role, &av)) goto erret;
    }

    if (ap->affil != NULL)
    {
        if (! AffilAsnWrite(ap->affil, aip, AUTHOR_affil))
            goto erret;
    }

	if (ap->is_corr != 255)
	{
		av.boolvalue = (Boolean) ap->is_corr;
		AsnWrite(aip, AUTHOR_is_corr, &av);
	}

   	if (! AsnCloseStruct(aip, atp, (Pointer)ap))    /* end SEQUENCE */
        goto erret;
    retval = TRUE;
erret:
    AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   CitArtNew()
*
*****************************************************************************/
CitArtPtr LIBCALL CitArtNew (void)
{
	CitArtPtr cap;

	cap = (CitArtPtr)MemNew(sizeof(CitArt));
	return cap;
}

/*****************************************************************************
*
*   CitArtFree()
*
*****************************************************************************/
CitArtPtr LIBCALL CitArtFree (CitArtPtr cap)
{
    if (cap == NULL)
        return cap;

    TitleFree(cap->title);
    AuthListFree(cap->authors);
    if (cap->from == 1)      /* journal */
        CitJourFree((CitJourPtr) cap->fromptr);
    else                     /* book or proceedings */
        CitBookFree((CitBookPtr) cap->fromptr);
	return (CitArtPtr)MemFree(cap);
}

/*****************************************************************************
*
*   CitArtAsnRead(aip, atp)
*
*****************************************************************************/
CitArtPtr LIBCALL CitArtAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	CitArtPtr cap=NULL;
	DataVal av;
	AsnTypePtr atp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return cap;
	}

	if (aip == NULL)
		return cap;

	if (orig == NULL)           /* CitArt ::= */
		atp = AsnReadId(aip, amp, CIT_ART);
	else
		atp = AsnLinkType(orig, CIT_ART);
    if (atp == NULL)
        return cap;

	cap = CitArtNew();
    if (cap == NULL)
        goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the open SEQUENCE */

    atp = AsnReadId(aip, amp, atp);   /* read title id */
	if (atp == CIT_ART_title)
	{
	    cap->title = TitleAsnRead(aip, atp);   /* read the titles */
    	if (cap->title == NULL)
        	goto erret;

	    atp = AsnReadId(aip, amp, atp);   /* read the authors id */
	}
    if (atp == CIT_ART_authors)
	{
	    cap->authors = AuthListAsnRead(aip, atp);
    	if (cap->authors == NULL)
        	goto erret;

	    atp = AsnReadId(aip, amp, atp);    /* read from tag */
	}
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret; /* balance from CHOICE */

    atp = AsnReadId(aip, amp, atp);    /* read the from CHOICE */
    if (atp == NULL)
        goto erret;
    if (atp == CIT_ART_from_journal)
    {
        cap->from = 1;
        cap->fromptr = (Pointer) CitJourAsnRead(aip, atp);
    }
    else if (atp == CIT_ART_from_book)
    {
        cap->from = 2;
        cap->fromptr = (Pointer) CitBookAsnRead(aip, atp);
    }
    else
    {
        cap->from = 3;
        cap->fromptr = (Pointer) CitProcAsnRead(aip, atp);
    }

    if (cap->fromptr == NULL)
        goto erret;

    AsnReadId(aip, amp, atp);
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end of SEQUENCE */
ret:
	AsnUnlinkType(orig);
	return cap;
erret:
    cap = CitArtFree(cap);
    goto ret;
}

/*****************************************************************************
*
*   CitArtAsnWrite(cap, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL CitArtAsnWrite (CitArtPtr cap, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, CIT_ART);
    if (atp == NULL)
        return FALSE;

	if (cap == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)cap))  /* open the SEQUENCE */
        goto erret;

	if (cap->title != NULL)
	{
	    if (! TitleAsnWrite(cap->title, aip, CIT_ART_title))  /* write the title(s) */
    	    goto erret;
	}
	if (cap->authors != NULL)
	{
	    if (! AuthListAsnWrite(cap->authors, aip, CIT_ART_authors))  /* authors */
    	    goto erret;
	}

	av.ptrvalue = cap->fromptr;
    if (! AsnWriteChoice(aip, CIT_ART_from, (Int2)cap->from, &av)) goto erret;

    if (cap->from == 1)
        retval = CitJourAsnWrite((CitJourPtr) cap->fromptr, aip, CIT_ART_from_journal);
    else if (cap->from == 2)
        retval = CitBookAsnWrite((CitBookPtr) cap->fromptr, aip, CIT_ART_from_book);
    else
        retval = CitProcAsnWrite((CitBookPtr) cap->fromptr, aip, CIT_ART_from_proc);

    if (retval == FALSE)
        goto erret;
    retval = FALSE;
    if (! AsnCloseStruct(aip, atp, (Pointer)cap))    /* close the SEQUENCE */
        goto erret;
    retval = TRUE;
erret:
    AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   ImprintNew()
*
*****************************************************************************/
ImprintPtr LIBCALL ImprintNew (void)
{
	ImprintPtr ip;

	ip = (ImprintPtr)MemNew(sizeof(Imprint));
	return ip;
}

/*****************************************************************************
*
*   ImprintFree()
*
*****************************************************************************/
ImprintPtr LIBCALL ImprintFree (ImprintPtr ip)
{
    if (ip == NULL)
        return ip;

    DateFree(ip->date);
    DateFree(ip->cprt);
    MemFree(ip->volume);
    MemFree(ip->issue);
    MemFree(ip->pages);
    MemFree(ip->section);
    MemFree(ip->part_sup);
    MemFree(ip->language);
    AffilFree(ip->pub);
	return (ImprintPtr)MemFree(ip);
}

/*****************************************************************************
*
*   ImprintAsnRead(aip, atp)
*
*****************************************************************************/
ImprintPtr LIBCALL ImprintAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	ImprintPtr ip=NULL;
	DataVal av;
	AsnTypePtr atp, oldatp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return ip;
	}

	if (aip == NULL)
		return ip;

	if (orig == NULL)           /* Imprint ::= */
		atp = AsnReadId(aip, amp, IMPRINT);
	else
		atp = AsnLinkType(orig, IMPRINT);
    oldatp = atp;
    if (atp == NULL)
        return ip;

	ip = ImprintNew();
    if (ip == NULL)
        goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the SEQUENCE start */
    atp = AsnReadId(aip, amp, atp);   /* read the date */
    if (atp == NULL)
        goto erret;
    ip->date = DateAsnRead(aip, atp);
    if (ip->date == NULL)
        goto erret;

    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL)
            goto erret;
        if (atp == IMPRINT_pub)
        {
            ip->pub = AffilAsnRead(aip, atp);
            if (ip->pub == NULL)
                goto erret;
        }
        else if (atp == IMPRINT_cprt)
        {
            ip->cprt = DateAsnRead(aip, atp);
            if (ip->cprt == NULL)
                goto erret;
        }
        else
        {
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
            if (atp == IMPRINT_volume)
                ip->volume = (CharPtr)av.ptrvalue;
            else if (atp == IMPRINT_issue)
                ip->issue = (CharPtr)av.ptrvalue;
            else if (atp == IMPRINT_pages)
                ip->pages = (CharPtr)av.ptrvalue;
            else if (atp == IMPRINT_section)
                ip->section = (CharPtr)av.ptrvalue;
            else if (atp == IMPRINT_part_sup)
                ip->part_sup = (CharPtr)av.ptrvalue;
            else if (atp == IMPRINT_language)
                ip->language = (CharPtr)av.ptrvalue;
            else if (atp == IMPRINT_prepub)
                ip->prepub = (Uint1)av.intvalue;
        }
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* end struct */
ret:
	AsnUnlinkType(orig);
	return ip;
erret:
    ip = ImprintFree(ip);
    goto ret;
}

/*****************************************************************************
*
*   ImprintAsnWrite(ip, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL ImprintAsnWrite (ImprintPtr ip, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, IMPRINT);
    if (atp == NULL)
        return FALSE;

	if (ip == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)ip))
        goto erret;
    if (! DateAsnWrite(ip->date, aip, IMPRINT_date))
        goto erret;
    if (ip->volume != NULL)
    {
        av.ptrvalue = ip->volume;
        if (! AsnWrite(aip, IMPRINT_volume, &av)) goto erret;
    }
    if (ip->issue != NULL)
    {
        av.ptrvalue = ip->issue;
        if (! AsnWrite(aip, IMPRINT_issue, &av)) goto erret;
    }
    if (ip->pages != NULL)
    {
        av.ptrvalue = ip->pages;
        if (! AsnWrite(aip, IMPRINT_pages, &av)) goto erret;
    }
    if (ip->section != NULL)
    {
        av.ptrvalue = ip->section;
        if (! AsnWrite(aip, IMPRINT_section, &av)) goto erret;
    }
    if (ip->pub != NULL)
    {
        if (! AffilAsnWrite(ip->pub, aip, IMPRINT_pub))
            goto erret;
    }
    if (ip->cprt != NULL)
    {
        if (! DateAsnWrite(ip->cprt, aip, IMPRINT_cprt))
            goto erret;
    }
    if (ip->part_sup != NULL)
    {
        av.ptrvalue = ip->part_sup;
        if (! AsnWrite(aip, IMPRINT_part_sup, &av)) goto erret;
    }
    if (ip->language != NULL)
    {
        av.ptrvalue = ip->language;
        if (! AsnWrite(aip, IMPRINT_language, &av)) goto erret;
    }
    if (ip->prepub != 0)
    {
        av.intvalue = ip->prepub;
        if (! AsnWrite(aip, IMPRINT_prepub, &av)) goto erret;
    }
    if (! AsnCloseStruct(aip, atp, (Pointer)ip))
        goto erret;
    retval = TRUE;
erret:
	AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   CitJourNew()
*
*****************************************************************************/
CitJourPtr LIBCALL CitJourNew (void)
{
	CitJourPtr cjp;

	cjp = (CitJourPtr)MemNew(sizeof(CitJour));
	return cjp;
}

/*****************************************************************************
*
*   CitJourFree()
*
*****************************************************************************/
CitJourPtr LIBCALL CitJourFree (CitJourPtr cjp)
{
    if (cjp == NULL)
        return cjp;

    TitleFree(cjp->title);
    ImprintFree(cjp->imp);
	return (CitJourPtr)MemFree(cjp);
}

/*****************************************************************************
*
*   CitJourAsnRead(aip, atp)
*
*****************************************************************************/
CitJourPtr LIBCALL CitJourAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	CitJourPtr cjp=NULL;
	DataVal av;
	AsnTypePtr atp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return cjp;
	}

	if (aip == NULL)
		return cjp;

	if (orig == NULL)           /* CitJour ::= */
		atp = AsnReadId(aip, amp, CIT_JOUR);
	else
		atp = AsnLinkType(orig, CIT_JOUR);
    if (atp == NULL)
        return cjp;

	cjp = CitJourNew();
    if (cjp == NULL)
        goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the open SEQUENCE */
    atp = AsnReadId(aip, amp, atp);   /* read title id */
    if (atp == NULL)
        goto erret;
    cjp->title = TitleAsnRead(aip, atp);
    if (cjp->title == NULL)
        goto erret;
    atp = AsnReadId(aip, amp, atp);   /* read imprint id */
    if (atp == NULL)
        goto erret;
    cjp->imp = ImprintAsnRead(aip, atp);
    if (cjp->imp == NULL)
        goto erret;
    atp = AsnReadId(aip, amp, atp);   /* read the close SEQUENCE id */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end of SEQUENCE */
ret:
	AsnUnlinkType(orig);
	return cjp;
erret:
    cjp = CitJourFree(cjp);
    goto ret;
}

/*****************************************************************************
*
*   CitJourAsnWrite(cjp, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL CitJourAsnWrite (CitJourPtr cjp, AsnIoPtr aip, AsnTypePtr orig)
{
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, CIT_JOUR);
    if (atp == NULL)
        return FALSE;

	if (cjp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)cjp))  /* open the SEQUENCE */
        goto erret;
    if (! TitleAsnWrite(cjp->title, aip, CIT_JOUR_title))
        goto erret;
    if (! ImprintAsnWrite(cjp->imp, aip, CIT_JOUR_imp))
        goto erret;
   	if (! AsnCloseStruct(aip, atp, (Pointer)cjp))    /* end SEQUENCE */
        goto erret;
    retval = TRUE;
erret:
    AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   CitBookNew()
*
*****************************************************************************/
CitBookPtr LIBCALL CitBookNew (void)
{
	CitBookPtr cbp;

	cbp = (CitBookPtr)MemNew(sizeof(CitBook));
	return cbp;
}

/*****************************************************************************
*
*   CitBookFree()
*
*****************************************************************************/
CitBookPtr LIBCALL CitBookFree (CitBookPtr cbp)
{
    ValNodePtr anp, nextp;

    if (cbp == NULL)
        return cbp;

    TitleFree(cbp->title);
    TitleFree(cbp->coll);
	AuthListFree(cbp->authors);
    ImprintFree(cbp->imp);
    if (cbp->othertype == 1)      /* Cit-proc */
    {
       anp = (ValNodePtr)cbp->otherdata;
       while (anp != NULL)
       {
           switch (anp->choice)
           {
               case 1:     /* number ... a CharPtr */
                 MemFree(anp->data.ptrvalue);
                break;
                case 2:    /* date ... a DatePtr */
                 DateFree((DatePtr)anp->data.ptrvalue);
                break;
                case 3:    /* place ... an AffilPtr */
                 AffilFree((AffilPtr)anp->data.ptrvalue);
                 break;
            }
            nextp = anp;
            MemFree(anp);
            anp = nextp;
        }
    }
    else if (cbp->othertype == 2)    /* Cit-let */
        MemFree(cbp->otherdata);    /* man-id ... a CharPtr */

    return (CitBookPtr)MemFree(cbp);
}

/*****************************************************************************
*
*   CitBookAsnRead(aip, atp)
*
*****************************************************************************/
CitBookPtr LIBCALL CitBookAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	CitBookPtr cbp=NULL;
	DataVal av;
	AsnTypePtr atp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return cbp;
	}

	if (aip == NULL)
		return cbp;

	if (orig == NULL)           /* CitBook ::= */
		atp = AsnReadId(aip, amp, CIT_BOOK);
	else
		atp = AsnLinkType(orig, CIT_BOOK);
    if (atp == NULL)
        return cbp;

	cbp = CitBookNew();
    if (cbp == NULL)
        goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the open SEQUENCE */
    atp = AsnReadId(aip, amp, atp);   /* read title id */
    if (atp == NULL)
        goto erret;
    cbp->title = TitleAsnRead(aip, atp);
    if (cbp->title == NULL)
        goto erret;
    atp = AsnReadId(aip, amp, atp);   /* read coll or authors id */
    if (atp == NULL)
        goto erret;
    if (atp == CIT_BOOK_coll)
    {
        cbp->coll = TitleAsnRead(aip, atp);
        if (cbp->coll == NULL)
            goto erret;
        atp = AsnReadId(aip, amp, atp);     /* read authors id */
        if (atp == NULL)
            goto erret;
    }
    cbp->authors = AuthListAsnRead(aip, atp);
    if (cbp->authors == NULL)
        goto erret;
    atp = AsnReadId(aip, amp, atp);         /* read imprint id */
    if (atp == NULL)
        goto erret;
    cbp->imp = ImprintAsnRead(aip, atp);
    if (cbp->imp == NULL)
        goto erret;
    
    atp = AsnReadId(aip, amp, atp);   /* read the close SEQUENCE id */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end of SEQUENCE */
ret:
	AsnUnlinkType(orig);
	return cbp;
erret:
    cbp = CitBookFree(cbp);
    goto ret;
}

/*****************************************************************************
*
*   CitBookAsnWrite(cbp, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL CitBookAsnWrite (CitBookPtr cbp, AsnIoPtr aip, AsnTypePtr orig)
{
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, CIT_BOOK);
    if (atp == NULL)
        return FALSE;

	if (cbp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)cbp))  /* open the SEQUENCE */
        goto erret;
    if (! TitleAsnWrite(cbp->title, aip, CIT_BOOK_title))
        goto erret;
    if (cbp->coll != NULL)
    {
        if (! TitleAsnWrite(cbp->coll, aip, CIT_BOOK_coll))
            goto erret;
    }
    if (! AuthListAsnWrite(cbp->authors, aip, CIT_BOOK_authors))
        goto erret;
    if (! ImprintAsnWrite(cbp->imp, aip, CIT_BOOK_imp))
        goto erret;
   	if (! AsnCloseStruct(aip, atp, (Pointer)cbp))    /* end SEQUENCE */
        goto erret;
    retval = TRUE;
erret:
    AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   CitProcAsnRead(aip, atp)
*
*****************************************************************************/
CitBookPtr LIBCALL CitProcAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	CitBookPtr cpp=NULL;
	DataVal av;
	AsnTypePtr atp;
    ValNodePtr anp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return cpp;
	}

	if (aip == NULL)
		return cpp;

	if (orig == NULL)           /* CitProc ::= */
		atp = AsnReadId(aip, amp, CIT_PROC);
	else
		atp = AsnLinkType(orig, CIT_PROC);
    if (atp == NULL)
        return cpp;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the open SEQUENCE */
    atp = AsnReadId(aip, amp, atp);   /* read book id */
    if (atp == NULL)
        goto erret;
    cpp = CitBookAsnRead(aip, atp);   /* read the book */
    if (cpp == NULL)
        goto erret;
    cpp->othertype = 1;               /* mark as a Cit-proc */
    atp = AsnReadId(aip, amp, atp);   /* read the meet id */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;        /* open struct */
    atp = AsnReadId(aip, amp, atp);   /* number id */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    anp = ValNodeNew(NULL);
    if (anp == NULL)
        goto erret;
    cpp->otherdata = (Pointer)anp;
    anp->choice = 1;
    anp->data.ptrvalue = av.ptrvalue;
    atp = AsnReadId(aip, amp, atp);   /* date id */
    if (atp == NULL)
        goto erret;
    anp = ValNodeNew(anp);
    if (anp == NULL)
        goto erret;
    anp->choice = 2;
    anp->data.ptrvalue = (Pointer)DateAsnRead(aip, atp);
    if (anp->data.ptrvalue == NULL)
        goto erret;
    atp = AsnReadId(aip, amp, atp);   /* place id */
    if (atp == NULL)
        goto erret;
    anp = ValNodeNew(anp);
    if (anp == NULL)
        goto erret;
    anp->choice = 3;
    anp->data.ptrvalue = (Pointer)AffilAsnRead(aip, atp);
    if (anp->data.ptrvalue == NULL)
        goto erret;
    atp = AsnReadId(aip, amp, atp);   /* end meet struct */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    
    atp = AsnReadId(aip, amp, atp);   /* read the close SEQUENCE id */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end of SEQUENCE */
ret:
	AsnUnlinkType(orig);
	return cpp;
erret:
    cpp = CitBookFree(cpp);
    goto ret;
}

/*****************************************************************************
*
*   CitProcAsnWrite(cpp, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL CitProcAsnWrite (CitBookPtr cpp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    ValNodePtr anp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, CIT_PROC);
    if (atp == NULL)
        return FALSE;

	if (cpp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)cpp))  /* open the SEQUENCE */
        goto erret;
    if (! CitBookAsnWrite(cpp, aip, CIT_PROC_book))   /* write the book */
        goto erret;
    if (! AsnOpenStruct(aip, CIT_PROC_meet, (Pointer)cpp->otherdata))
        goto erret;
    anp = (ValNodePtr)cpp->otherdata;
    av.ptrvalue = anp->data.ptrvalue;
    if (! AsnWrite(aip, MEETING_number, &av)) goto erret;
    anp = anp->next;
    if (! DateAsnWrite((DatePtr)anp->data.ptrvalue, aip, MEETING_date))
        goto erret;
    anp = anp->next;
    if (! AffilAsnWrite((AffilPtr)anp->data.ptrvalue, aip, MEETING_place))
        goto erret;
    if (! AsnCloseStruct(aip, CIT_PROC_meet, (Pointer)cpp->otherdata))
        goto erret;
   	if (! AsnCloseStruct(aip, atp, (Pointer)cpp))    /* end SEQUENCE */
        goto erret;
    retval = TRUE;
erret:
    AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   CitLetAsnRead(aip, atp)
*
*****************************************************************************/
CitBookPtr LIBCALL CitLetAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	CitBookPtr clp=NULL;
	DataVal av;
	AsnTypePtr atp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return clp;
	}

	if (aip == NULL)
		return clp;

	if (orig == NULL)           /* CitLet ::= */
		atp = AsnReadId(aip, amp, CIT_LET);
	else
		atp = AsnLinkType(orig, CIT_LET);
    if (atp == NULL)
        return clp;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the open SEQUENCE */
    atp = AsnReadId(aip, amp, atp);   /* read book id */
    if (atp == NULL)
        goto erret;
    clp = CitBookAsnRead(aip, atp);   /* read the book */
    if (clp == NULL)
        goto erret;
    clp->othertype = 2;               /* mark as a Cit-let */
    atp = AsnReadId(aip, amp, atp);   /* read the man-id if present */
    if (atp == NULL)
        goto erret;
    if (atp == CIT_LET_man_id)
    {
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        clp->otherdata = av.ptrvalue;
        atp = AsnReadId(aip, amp, atp);    /* end struct */
        if (atp == NULL)
            goto erret;
    }
	if (atp == CIT_LET_type)         /* read type if present */
	{
		if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
		clp->let_type = (Uint1) av.intvalue;
		atp = AsnReadId(aip, amp, atp);
        if (atp == NULL)
            goto erret;
	}
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end of SEQUENCE */
ret:
	AsnUnlinkType(orig);
	return clp;
erret:
    clp = CitBookFree(clp);
    goto ret;
}

/*****************************************************************************
*
*   CitLetAsnWrite(clp, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL CitLetAsnWrite (CitBookPtr clp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, CIT_LET);
    if (atp == NULL)
        return FALSE;

	if (clp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)clp))  /* open the SEQUENCE */
        goto erret;
    if (! CitBookAsnWrite(clp, aip, CIT_LET_cit))   /* write the book */
        goto erret;
    if (clp->otherdata != NULL)   /* man-id */
    {
        av.ptrvalue = clp->otherdata;
        if (! AsnWrite(aip, CIT_LET_man_id, &av)) goto erret;
    }
	if (clp->let_type)            /* type of Cit-let */
	{
		av.intvalue = (Int4)clp->let_type;
		if (! AsnWrite(aip, CIT_LET_type, &av)) goto erret;
	}
   	if (! AsnCloseStruct(aip, atp, (Pointer)clp))    /* end SEQUENCE */
        goto erret;
    retval = TRUE;
erret:
    AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   CitPatNew()
*
*****************************************************************************/
CitPatPtr LIBCALL CitPatNew (void)
{
	CitPatPtr cpp;

	cpp = (CitPatPtr)MemNew(sizeof(CitPat));
	return cpp;
}

/*****************************************************************************
*
*   CitPatFree()
*
*****************************************************************************/
CitPatPtr LIBCALL CitPatFree (CitPatPtr cpp)
{
    if (cpp == NULL)
        return cpp;

    MemFree(cpp->title);
    AuthListFree(cpp->authors);
    MemFree(cpp->country);
    MemFree(cpp->doc_type);
    MemFree(cpp->number);
    DateFree(cpp->date_issue);
#ifdef ASN30
	ValNodeFreeData(cpp->_class);
#else
    MemFree(cpp->_class);
#endif
    MemFree(cpp->app_number);
    DateFree(cpp->app_date);

#ifdef ASN30
	AuthListFree(cpp->applicants);
	AuthListFree(cpp->assignees);
	PatPrioritySetFree(cpp->priority);
	MemFree(cpp->abstract);
#endif

    return (CitPatPtr)MemFree(cpp);
}

/*****************************************************************************
*
*   CitPatAsnRead(aip, atp)
*
*****************************************************************************/
CitPatPtr LIBCALL CitPatAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	CitPatPtr cpp=NULL;
	DataVal av;
	AsnTypePtr atp, oldatp;
#ifdef ASN30
	ValNodePtr vnp;
#endif

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return cpp;
	}

	if (aip == NULL)
		return cpp;

	if (orig == NULL)           /* CitPat ::= */
		atp = AsnReadId(aip, amp, CIT_PAT);
	else
		atp = AsnLinkType(orig, CIT_PAT);
    oldatp = atp;
    if (atp == NULL)
        return cpp;

	cpp = CitPatNew();
    if (cpp == NULL)
        goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the open SEQUENCE */
    atp = AsnReadId(aip, amp, atp);   /* read title id */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    cpp->title = (CharPtr)av.ptrvalue;
    atp = AsnReadId(aip, amp, atp);   /* read author list id */
    if (atp == NULL)
        goto erret;
    cpp->authors = AuthListAsnRead(aip, atp);
    if (cpp->authors == NULL)
        goto erret;
    atp = AsnReadId(aip, amp, atp );  /* country */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    cpp->country = (CharPtr)av.ptrvalue;
    atp = AsnReadId(aip, amp, atp );  /* doc_type */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    cpp->doc_type = (CharPtr)av.ptrvalue;
    atp = AsnReadId(aip, amp, atp );  /* number */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    cpp->number = (CharPtr)av.ptrvalue;
    atp = AsnReadId(aip, amp, atp);   /* date */
    if (atp == NULL)
        goto erret;
    cpp->date_issue = DateAsnRead(aip, atp);
    if (cpp->date_issue == NULL)
        goto erret;
    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)  /* read options */
    {
        if (atp == NULL)
            goto erret;
        if (atp == CIT_PAT_app_date)
        {
            cpp->app_date = DateAsnRead(aip, atp);
            if (cpp->app_date == NULL)
                goto erret;
        }
#ifdef ASN30
		else if (atp == CIT_PAT_applicants)
		{
			cpp->applicants = AuthListAsnRead(aip, atp);
			if (cpp->applicants == NULL)
				goto erret;
		}
		else if (atp == CIT_PAT_assignees)
		{
			cpp->assignees = AuthListAsnRead(aip, atp);
			if (cpp->assignees == NULL)
				goto erret;
		}
		else if (atp == CIT_PAT_priority)
		{
			cpp->priority = PatPrioritySetAsnRead(aip, atp, CIT_PAT_priority_E);
			if (cpp->priority == NULL)
				goto erret;
		}
#endif
        else
        {
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
            if (atp == CIT_PAT_class)
#ifdef ASN30
			{
				while ((atp = AsnReadId(aip, amp, atp)) != CIT_PAT_class)
				{
					if (atp == NULL) goto erret;
					if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
					vnp = ValNodeNew(cpp->_class);
					if (vnp == NULL) goto erret;
					if (cpp->_class == NULL)
						cpp->_class = vnp;
					vnp->data.ptrvalue = (CharPtr)av.ptrvalue;
				}
				if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
			}
#else
                cpp->_class = (CharPtr)av.ptrvalue;
#endif
            else if (atp == CIT_PAT_app_number)
                cpp->app_number = (CharPtr)av.ptrvalue;
#ifdef ASN30
			else if (atp == CIT_PAT_abstract)
				cpp->abstract = (CharPtr)av.ptrvalue;
#endif
        }
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end of SEQUENCE */
ret:
	AsnUnlinkType(orig);
	return cpp;
erret:
    cpp = CitPatFree(cpp);
    goto ret;
}

/*****************************************************************************
*
*   CitPatAsnWrite(cpp, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL CitPatAsnWrite (CitPatPtr cpp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;
#ifdef ASN30
	ValNodePtr vnp;
#endif

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, CIT_PAT);
    if (atp == NULL)
        return FALSE;

	if (cpp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)cpp))  /* open the SEQUENCE */
        goto erret;
    av.ptrvalue = cpp->title;
    if (! AsnWrite(aip, CIT_PAT_title, &av)) goto erret;
    if (! AuthListAsnWrite(cpp->authors, aip, CIT_PAT_authors))
        goto erret;
    av.ptrvalue = cpp->country;
    if (! AsnWrite(aip, CIT_PAT_country, &av)) goto erret;
    av.ptrvalue = cpp->doc_type;
    if (! AsnWrite(aip, CIT_PAT_doc_type, &av)) goto erret;
	av.ptrvalue = cpp->number;
	if (! AsnWrite(aip, CIT_PAT_number, &av)) goto erret;
    if (! DateAsnWrite(cpp->date_issue, aip, CIT_PAT_date_issue))
        goto erret;
    if (cpp->_class != NULL)
    {
#ifdef ASN30
		if (! AsnOpenStruct(aip, CIT_PAT_class, (Pointer)(cpp->_class)))
			goto erret;
		for (vnp = cpp->_class; vnp != NULL; vnp = vnp->next)
		{
			av.ptrvalue = vnp->data.ptrvalue;
			if (! AsnWrite(aip, CIT_PAT_class_E, &av)) goto erret;
		}
		if (! AsnCloseStruct(aip, CIT_PAT_class, (Pointer)(cpp->_class)))
			goto erret;
#else
        av.ptrvalue = cpp->_class;
        if (! AsnWrite(aip, CIT_PAT_class, &av)) goto erret;
#endif
    }
    if (cpp->app_number != NULL)
    {
        av.ptrvalue = cpp->app_number;
        if (! AsnWrite(aip, CIT_PAT_app_number, &av)) goto erret;
    }
    if (cpp->app_date != NULL)
    {
        if (! DateAsnWrite(cpp->app_date, aip, CIT_PAT_app_date))
            goto erret;
    }

#ifdef ASN30
	if (cpp->applicants != NULL)
	{
		if (! AuthListAsnWrite(cpp->applicants, aip, CIT_PAT_applicants))
			goto erret;
	}
	if (cpp->assignees != NULL)
	{
		if (! AuthListAsnWrite(cpp->assignees, aip, CIT_PAT_assignees))
			goto erret;
	}
	if (cpp->priority != NULL)
	{
		if (! PatPrioritySetAsnWrite(cpp->priority, aip, CIT_PAT_priority, CIT_PAT_priority_E));
			goto erret;
	}
	if (cpp->abstract != NULL)
	{
		av.ptrvalue = cpp->abstract;
		if (! AsnWrite(aip, CIT_PAT_abstract, &av)) goto erret;
	}
#endif

    if (! AsnCloseStruct(aip, atp, (Pointer)cpp))    /* end SEQUENCE */
        goto erret;
    retval = TRUE;
erret:
    AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   IdPatNew()
*
*****************************************************************************/
IdPatPtr LIBCALL IdPatNew (void)
{
	IdPatPtr idp;

	idp = (IdPatPtr)MemNew(sizeof(IdPat));
	return idp;
}

/*****************************************************************************
*
*   IdPatFree()
*
*****************************************************************************/
IdPatPtr LIBCALL IdPatFree (IdPatPtr idp)
{
    if (idp == NULL)
        return idp;

    MemFree(idp->country);
    MemFree(idp->number);
    MemFree(idp->app_number);
#ifdef ASN30
	MemFree(idp->doc_type);
#endif
    return (IdPatPtr)MemFree(idp);
}

/*****************************************************************************
*
*   IdPatAsnRead(aip, atp)
*
*****************************************************************************/
IdPatPtr LIBCALL IdPatAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	IdPatPtr idp=NULL;
	DataVal av;
	AsnTypePtr atp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return idp;
	}

	if (aip == NULL)
		return idp;

	if (orig == NULL)           /* IdPat ::= */
		atp = AsnReadId(aip, amp, ID_PAT);
	else
		atp = AsnLinkType(orig, ID_PAT);
    if (atp == NULL)
        return idp;

	idp = IdPatNew();
    if (idp == NULL)
        goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the open SEQUENCE */
    atp = AsnReadId(aip, amp, atp );  /* country */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    idp->country = (CharPtr)av.ptrvalue;
    atp = AsnReadId(aip, amp, atp );  /* CHOICE */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    atp = AsnReadId(aip, amp, atp );  /* number or app_number */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    if (atp == ID_PAT_id_number)
        idp->number = (CharPtr)av.ptrvalue;
    else
        idp->app_number = (CharPtr)av.ptrvalue;
    atp = AsnReadId(aip, amp, atp);  /* end or doc-type */
    if (atp == NULL)
        goto erret;
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
#ifdef ASN30
	if (atp == ID_PAT_doc_type)
	{
		idp->doc_type = av.ptrvalue;
		atp = AsnReadId(aip, amp, atp);
		if (atp == NULL) goto erret;
	    if (AsnReadVal(aip, atp, &av) <= 0) goto erret; /* end of SEQUENCE */
	}
#endif
ret:
	AsnUnlinkType(orig);
	return idp;
erret:
    idp = IdPatFree(idp);
    goto ret;
}

/*****************************************************************************
*
*   IdPatAsnWrite(idp, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL IdPatAsnWrite (IdPatPtr idp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, numtype;
	Int2 choicenum;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, ID_PAT);
    if (atp == NULL)
        return FALSE;

	if (idp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)idp))  /* open the SEQUENCE */
        goto erret;
    av.ptrvalue = idp->country;
    if (! AsnWrite(aip, ID_PAT_country, &av)) goto erret;
    if (idp->number != NULL)
	{
		choicenum = 0;		/* number */
		av.ptrvalue = idp->number;
		numtype = ID_PAT_id_number;
	}
	else
	{
		choicenum = 1;		/* app_number */
		av.ptrvalue = idp->app_number;
		numtype = ID_PAT_id_app_number;
	}
	
    if (! AsnWriteChoice(aip, ID_PAT_id, choicenum, &av)) goto erret;
	if (! AsnWrite(aip, numtype, &av)) goto erret;

#ifdef ASN30
	if (idp->doc_type != NULL)
	{
		av.ptrvalue = idp->doc_type;
		if (! AsnWrite(aip, ID_PAT_doc_type, &av)) goto erret;
	}
#endif

    if (! AsnCloseStruct(aip, atp, (Pointer)idp))    /* end SEQUENCE */
        goto erret;
    retval = TRUE;
erret:
    AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   Boolean IdPatMatch(a,b)
*
*****************************************************************************/
Boolean LIBCALL IdPatMatch (IdPatPtr a, IdPatPtr b)
{
    if ((a == NULL) || (b == NULL))
        return FALSE;

    if (StringCmp(a->country, b->country))   /* countries must match */
        return FALSE;

    if ((a->number != NULL) && (b->number != NULL))
    {
        if (! StringCmp(a->number, b->number))
            return TRUE;
        else
            return FALSE;
    }
    else
    {
        if (! StringCmp(a->app_number, b->app_number))
            return TRUE;
        else
            return FALSE;
    }
}

/*****************************************************************************
*
*   CitGenNew()
*
*****************************************************************************/
CitGenPtr LIBCALL CitGenNew (void)
{
	CitGenPtr cgp;

	cgp = (CitGenPtr)MemNew(sizeof(CitGen));
	if (cgp == NULL) return cgp;
    cgp->muid = -1;
    cgp->serial_number = -1;  /* not set */
	return cgp;
}

/*****************************************************************************
*
*   CitGenFree()
*
*****************************************************************************/
CitGenPtr LIBCALL CitGenFree (CitGenPtr cgp)
{
    if (cgp == NULL)
        return cgp;

    MemFree(cgp->cit);
    AuthListFree(cgp->authors);
    TitleFree(cgp->journal);
    MemFree(cgp->volume);
    MemFree(cgp->issue);
    MemFree(cgp->pages);
    DateFree(cgp->date);
	MemFree(cgp->title);

    return (CitGenPtr)MemFree(cgp);
}

/*****************************************************************************
*
*   CitGenAsnRead(aip, atp)
*
*****************************************************************************/
CitGenPtr LIBCALL CitGenAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	CitGenPtr cgp=NULL;
	DataVal av;
	AsnTypePtr atp, oldatp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return cgp;
	}

	if (aip == NULL)
		return cgp;

	if (orig == NULL)           /* CitGen ::= */
		atp = AsnReadId(aip, amp, CIT_GEN);
	else
		atp = AsnLinkType(orig, CIT_GEN);
    oldatp = atp;
    if (atp == NULL)
        return cgp;

	cgp = CitGenNew();
    if (cgp == NULL)
        goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the open SEQUENCE */
    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL)
            goto erret;
        if (atp == CIT_GEN_authors)
        {
            cgp->authors = AuthListAsnRead(aip, atp);
            if (cgp->authors == NULL)
                goto erret;
        }
        else if (atp == CIT_GEN_journal)
        {
            cgp->journal = TitleAsnRead(aip, atp);
            if (cgp->journal == NULL)
                goto erret;
        }
        else if (atp == CIT_GEN_date)
        {
            cgp->date = DateAsnRead(aip, atp);
            if (cgp->date == NULL)
                goto erret;
        }
        else
        {
            if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
            if (atp == CIT_GEN_cit)
                cgp->cit = (CharPtr)av.ptrvalue;
            else if (atp == CIT_GEN_volume)
                cgp->volume = (CharPtr)av.ptrvalue;
            else if (atp == CIT_GEN_issue)
                cgp->issue = (CharPtr)av.ptrvalue;
            else if (atp == CIT_GEN_pages)
                cgp->pages = (CharPtr)av.ptrvalue;
            else if (atp == CIT_GEN_serial_number)
                cgp->serial_number = (Int2) av.intvalue;
            else if (atp == CIT_GEN_title)
                cgp->title = (CharPtr)av.ptrvalue;
        }
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end of SEQUENCE */
ret:
	AsnUnlinkType(orig);
	return cgp;
erret:
    cgp = CitGenFree(cgp);
    goto ret;
}

/*****************************************************************************
*
*   CitGenAsnWrite(cgp, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL CitGenAsnWrite (CitGenPtr cgp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, CIT_GEN);
    if (atp == NULL)
        return FALSE;

	if (cgp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)cgp))  /* open the SEQUENCE */
        goto erret;
    if (cgp->cit != NULL)
    {
        av.ptrvalue = cgp->cit;
        if (! AsnWrite(aip, CIT_GEN_cit, &av)) goto erret;
    }
    if (cgp->authors != NULL)
    {
        if (! AuthListAsnWrite(cgp->authors, aip, CIT_GEN_authors))
            goto erret;
    }
    if (cgp->muid >= 0)
    {
        av.intvalue = cgp->muid;
        if (! AsnWrite(aip, CIT_GEN_muid, &av)) goto erret;
    }
    if (cgp->journal != NULL)
    {
        if (! TitleAsnWrite(cgp->journal, aip, CIT_GEN_journal))
            goto erret;
    }
    if (cgp->volume != NULL)
    {
        av.ptrvalue = cgp->volume;
        if (! AsnWrite(aip, CIT_GEN_volume, &av)) goto erret;
    }
    if (cgp->issue != NULL)
    {
        av.ptrvalue = cgp->issue;
        if (! AsnWrite(aip, CIT_GEN_issue, &av)) goto erret;
    }
    if (cgp->pages != NULL)
    {
        av.ptrvalue = cgp->pages;
        if (! AsnWrite(aip, CIT_GEN_pages, &av)) goto erret;
    }
    if (cgp->date != NULL)
    {
        if (! DateAsnWrite(cgp->date, aip, CIT_GEN_date))
            goto erret;
    }
    if (cgp->serial_number >= 0)
    {
        av.intvalue = (Int4)cgp->serial_number;
        if (! AsnWrite(aip, CIT_GEN_serial_number, &av)) goto erret;
    }
    if (cgp->title != NULL)
    {
        av.ptrvalue = cgp->title;
        if (! AsnWrite(aip, CIT_GEN_title, &av)) goto erret;
    }
    if (! AsnCloseStruct(aip, atp, (Pointer)cgp))    /* end SEQUENCE */
        goto erret;
    retval = TRUE;
erret:
    AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   TitleFree()
*
*****************************************************************************/
ValNodePtr LIBCALL TitleFree (ValNodePtr tp)
{
    ValNodePtr next;

    if (tp == NULL)
        return tp;

    while (tp != NULL)
    {
        MemFree(tp->data.ptrvalue);
        next = tp->next;
        MemFree(tp);
        tp = next;
    }

    return tp;
}

/*****************************************************************************
*
*   TitleAsnRead(aip, atp)
*
*****************************************************************************/
ValNodePtr LIBCALL TitleAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	ValNodePtr first=NULL, tp;
	DataVal av;
	AsnTypePtr atp, oldatp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return first;
	}

	if (aip == NULL)
		return NULL;

	if (orig == NULL)           /* Title ::= */
		atp = AsnReadId(aip, amp, TITLE);
	else
		atp = AsnLinkType(orig, TITLE);
    oldatp = atp;
    if (atp == NULL)
        return first;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the open SET OF */
    while ((atp = AsnReadId(aip, amp, atp)) != oldatp)
    {
        if (atp == NULL)
            goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;   /* read the CHOICE */
        atp = AsnReadId(aip, amp, atp);    /* type of CHOICE */
        if (atp == NULL)
            goto erret;
        if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
        tp = ValNodeNew(first);
        if (tp == NULL)
            goto erret;
        tp->data.ptrvalue = av.ptrvalue;
        if (first == NULL)
            first = tp;
        if (atp == TITLE_E_name)
            tp->choice = 1;
        else if (atp == TITLE_E_tsub)
            tp->choice = 2;
        else if (atp == TITLE_E_trans)
            tp->choice = 3;
        else if (atp == TITLE_E_jta)
            tp->choice = 4;
        else if (atp == TITLE_E_iso_jta)
            tp->choice = 5;
        else if (atp == TITLE_E_ml_jta)
            tp->choice = 6;
        else if (atp == TITLE_E_coden)
            tp->choice = 7;
        else if (atp == TITLE_E_issn)
            tp->choice = 8;
        else if (atp == TITLE_E_abr)
            tp->choice = 9;
        else if (atp == TITLE_E_isbn)
            tp->choice = 10;
    }
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end of SET OF */
ret:
	AsnUnlinkType(orig);
	return first;
erret:
    first = TitleFree(first);
    goto ret;
}

/*****************************************************************************
*
*   TitleAsnWrite(tp, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL TitleAsnWrite (ValNodePtr tp, AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp, atp2;
	ValNodePtr oldtp;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, TITLE);
    if (atp == NULL)
        return FALSE;

	if (tp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

	oldtp = tp;
    if (! AsnOpenStruct(aip, atp, (Pointer)oldtp))  /* open the SET OF */
        goto erret;
    while (tp != NULL)
    {
        av.ptrvalue = tp->data.ptrvalue;
 		switch (tp->choice)
 		{
 			case Cit_title_name:
 	            atp2 = TITLE_E_name;
 				break;
 			case Cit_title_tsub:
 	            atp2 = TITLE_E_tsub;
 				break;
 			case Cit_title_trans:
 	            atp2 = TITLE_E_trans;
 				break;
 			case Cit_title_jta:
 	            atp2 = TITLE_E_jta;
 				break;
 			case Cit_title_iso_jta:
 	            atp2 = TITLE_E_iso_jta;
 				break;
 			case Cit_title_ml_jta:
 	            atp2 = TITLE_E_ml_jta;
 				break;
 			case Cit_title_coden:
 	            atp2 = TITLE_E_coden;
 				break;
 			case Cit_title_issn:
 	            atp2 = TITLE_E_issn;
 				break;
 			case Cit_title_abr:
 	            atp2 = TITLE_E_abr;
 				break;
 			case Cit_title_isbn:
 	            atp2 = TITLE_E_isbn;
 				break;
 			default:
 				atp2 = NULL;
 				break;
 		}
 		if (atp2 != NULL)
 		{
 	        if (! AsnWriteChoice(aip, TITLE_E, (Int2)tp->choice, &av)) goto erret;
     	    if (! AsnWrite(aip, atp2, &av)) goto erret;
 		}
        tp = tp->next;
    }
    if (! AsnCloseStruct(aip, atp, (Pointer)oldtp))    /* end SET OF */
        goto erret;
    retval = TRUE;
erret:
    AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   CitSubNew()
*
*****************************************************************************/
CitSubPtr LIBCALL CitSubNew (void)
{
	CitSubPtr csp;

	csp = (CitSubPtr)MemNew(sizeof(CitSub));
	return csp;
}

/*****************************************************************************
*
*   CitSubFree()
*
*****************************************************************************/
CitSubPtr LIBCALL CitSubFree (CitSubPtr csp)
{
    if (csp == NULL)
        return csp;
	
	AuthListFree(csp->authors);
    ImprintFree(csp->imp);
    return (CitSubPtr)MemFree(csp);
}

/*****************************************************************************
*
*   CitSubAsnRead(aip, atp)
*
*****************************************************************************/
CitSubPtr LIBCALL CitSubAsnRead (AsnIoPtr aip, AsnTypePtr orig)
{
	CitSubPtr csp=NULL;
	DataVal av;
	AsnTypePtr atp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return csp;
	}

	if (aip == NULL)
		return csp;

	if (orig == NULL)           /* CitSub ::= */
		atp = AsnReadId(aip, amp, CIT_SUB);
	else
		atp = AsnLinkType(orig, CIT_SUB);
    if (atp == NULL)
        return csp;

	csp = CitSubNew();
    if (csp == NULL)
        goto erret;

    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the open SEQUENCE */
    atp = AsnReadId(aip, amp, atp);   /* read authors id */
    if (atp == NULL)
        goto erret;
    csp->authors = AuthListAsnRead(aip, atp);
    if (csp->authors == NULL)
        goto erret;
    atp = AsnReadId(aip, amp, atp);         /* read imprint id */
    if (atp == NULL)
        goto erret;
    csp->imp = ImprintAsnRead(aip, atp);
    if (csp->imp == NULL)
        goto erret;
    
    atp = AsnReadId(aip, amp, atp);   /* read the close SEQUENCE id */
    if (atp == NULL)
        goto erret;
	if (atp == CIT_SUB_medium)    /* nope, got medium */
	{
		if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
		csp->medium = (Uint1) av.intvalue;
		atp = AsnReadId(aip, amp, atp);
        if (atp == NULL)
            goto erret;
	}
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* end of SEQUENCE */
ret:
	AsnUnlinkType(orig);
	return csp;
erret:
    csp = CitSubFree(csp);
    goto ret;
}

/*****************************************************************************
*
*   CitSubAsnWrite(csp, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL CitSubAsnWrite (CitSubPtr csp, AsnIoPtr aip, AsnTypePtr orig)
{
	AsnTypePtr atp;
	DataVal av;
    Boolean retval = FALSE;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if (aip == NULL)
		return FALSE;

	atp = AsnLinkType(orig, CIT_SUB);
    if (atp == NULL)
        return FALSE;

	if (csp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

    if (! AsnOpenStruct(aip, atp, (Pointer)csp))  /* open the SEQUENCE */
        goto erret;
    if (! AuthListAsnWrite(csp->authors, aip, CIT_SUB_authors))
        goto erret;
    if (! ImprintAsnWrite(csp->imp, aip, CIT_SUB_imp))
        goto erret;
	if (csp->medium)
	{
		av.intvalue = (Int4)csp->medium;
		if (! AsnWrite(aip, CIT_SUB_medium, &av)) goto erret;
	}
   	if (! AsnCloseStruct(aip, atp, (Pointer)csp))    /* end SEQUENCE */
        goto erret;
    retval = TRUE;
erret:
    AsnUnlinkType(orig);
	return retval;
}

/*****************************************************************************
*
*   Int2 CitArtMatch(a, b)
*   	returns 0 = same article
*   	1,-1 = different article
*
*****************************************************************************/
Int2 LIBCALL CitArtMatch(CitArtPtr a, CitArtPtr b)
{
	Int2 retval = 1;


	if (a == NULL)
		return (Int2) -1;
	else if (b == NULL)
		return retval;

	if (a->from != b->from)    /* different types */
	{
		if (a->from < b->from)
			return (Int2) -1;
		else
			return retval;
	}


	if (a->from == 1)   /* journal article */
		retval = CitJourMatch((CitJourPtr)a->fromptr, (CitJourPtr)b->fromptr);
	else                /* book or proceedings */
		retval = CitBookMatch((CitBookPtr)a->fromptr, (CitBookPtr)b->fromptr);



	if (retval)   /* different sources */
		return retval;


										   /* check all authors */
	retval = AuthListMatch(a->authors, b->authors, TRUE);


	if (retval)
		return retval;



	retval = TitleMatch(a->title, b->title, Cit_title_name);
	if ((retval > 1) || (retval < -1))   /* not same type or NULL */
		retval = 0;   /* matched everything else ok */



	return retval;
}

/*****************************************************************************
*
*   Int2 CitBookMatch(a, b)
*   	returns 0 = same book
*   	1,-1 = different book
*   Also does Cit-proc and Cit-let
*
*****************************************************************************/
Int2 LIBCALL CitBookMatch (CitBookPtr a, CitBookPtr b)
{
	Int2 retval = 1;



	if (a == NULL)
		return (Int2) -1;
	else if (b == NULL)
		return retval;

	if (a->othertype > b->othertype)
		return (Int2)1;
	else if (a->othertype < b->othertype)
		return (Int2)-1;

	retval = AuthListMatch(a->authors, b->authors, TRUE);
	if (retval)
		return retval;

	retval = ImprintMatch(a->imp, b->imp, FALSE);
	if (retval)
		return retval;

	retval = TitleMatch(a->title, b->title, Cit_title_name);
	if (retval == 2)   /* not same type, it fails */
		retval = 1;
	else if (retval == -2)
		retval = -1;
	return retval;
}

/*****************************************************************************
*
*   Int2 CitGenMatch(a, b, all)
*   	returns 0 = same Cit-gen
*   	1,-1 = different Cit-gen
*   	if (all) then all fields must be shared
*   		otherwise, match only shared fields
*
*****************************************************************************/
Int2 LIBCALL CitGenMatch (CitGenPtr a, CitGenPtr b, Boolean all)
{
	Int2 retval = 0, i;
	CharPtr ap, bp;

	if (a == NULL)
		return (Int2) -1;
	else if (b == NULL)
		return (Int2)1;

	for (i = 0; i < 5; i++)
	{
		switch (i)
		{
			case 0:
				ap = a->volume;
				bp = b->volume;
				break;
			case 1:
				ap = a->issue;
				bp = b->issue;
				break;
			case 2:
				ap = a->pages;
				bp = b->pages;
				break;
			case 3:
				ap = a->title;
				bp = b->title;
				break;
			case 4:
				ap = a->cit;
				bp = b->cit;
				break;
		}
		if ((ap != NULL) && (bp != NULL))
		{
			retval = StringICmp(ap, bp);
			if (retval > 0)
				return (Int2) 1;
			else if (retval < 0)
				return (Int2) -1;
		}
		else if (all)
		{
			if ((ap == NULL) && (bp != NULL))
				return (Int2) -1;
			else if ((ap != NULL) && (bp == NULL))
				return (Int2) 1;
		}
	}

	if (((a->authors != NULL) && (b->authors != NULL)) ||
		(((a->authors != NULL) || (b->authors != NULL)) && all))
	{
		retval = AuthListMatch(a->authors, b->authors, TRUE);
		if (retval)
			return retval;
	}

	if (((a->muid != 0) && (b->muid != 0)) || all)
	{
		if (a->muid > b->muid)
			return (Int2) 1;
		else if (a->muid < b->muid)
			return (Int2) -1;
	}

	if (((a->serial_number != -1) && (b->serial_number != -1)) || all)
	{
		if (a->serial_number > b->serial_number)
			return (Int2) 1;
		else if (a->serial_number < b->serial_number)
			return (Int2) -1;
	}

	if (((a->journal != NULL) && (b->journal != NULL)) || all)
	{
		for (i = 1; i <= 10; i++)
		{
			if (a->journal == NULL && b->journal == NULL)	/* JAK */
				break;
			retval = TitleMatch(a->journal, b->journal, (Uint1)i);
			if (! retval)  /* match */
				break;
			if ((retval == -1) || (retval == 1))  /* same type, diff journal */
				return retval;
		}
		if (retval == 2)   /* couldn't match the journal names, fail */
			return (Int2) 1;
		else if (retval == -2)
			return (Int2) -1;
	}

	if (((a->date != NULL) && (b->date != NULL)) ||
		(((a->date != NULL) || (b->date != NULL)) && all))
	{
		retval = DateMatch(a->date, b->date, TRUE);
		if (retval == 2)
			retval = 1;
		else if (retval == -2)
			retval = -1;
	}

	if (all) {							/* JAK */
		if (a->volume == NULL && b->volume == NULL &&
			a->issue == NULL && b->issue == NULL &&
			a->pages == NULL && b->pages == NULL &&
			a->title == NULL && b->title == NULL &&
			a->cit == NULL && b->cit == NULL &&
			a->authors == NULL && b->authors == NULL &&
			a->muid == -1 && b->muid == -1 &&
			a->journal == NULL && b->journal == NULL &&
			a->date == NULL && b->date == NULL &&
			a->serial_number != -1 && b->serial_number != -1) {
			return -1;
		}
	}

	return retval;
}

/*****************************************************************************
*
*   Int2 CitSubMatch(a, b)
*   	returns 0 = same submission
*   	1,-1 = different submission
*
*****************************************************************************/
Int2 LIBCALL CitSubMatch (CitSubPtr a, CitSubPtr b)
{
	Int2 retval = 1;

	if (a == NULL)
		return (Int2) -1;
	else if (b == NULL)
		return retval;

	retval = AuthListMatch(a->authors, b->authors, TRUE);
	if (retval)
		return retval;

	retval = ImprintMatch(a->imp, b->imp, FALSE);
	return retval;
}

/*****************************************************************************
*
*   Int2 CitJourMatch(a, b)
*   	returns 0 = same journal
*   	1,-1 = different journal
*
*****************************************************************************/
Int2 LIBCALL CitJourMatch (CitJourPtr a, CitJourPtr b)
{
	Int2 retval = 1, i;
	static Uint1 journals[6] = {   /* precedence for matching journal names */
		5 ,	/* iso_jta */
		6 , /* medline_jta */
		7 , /* coden */
		8 , /* issn */
		1 , /* name */
		5 }; /* jta */


	if (a == NULL)
		return (Int2) -1;
	else if (b == NULL)
		return retval;

	for (i = 0; i < 6; i++)
	{
		retval = TitleMatch(a->title, b->title, journals[i]);
		if (! retval)  /* match */
			break;
		if ((retval == -1) || (retval == 1))  /* same type, diff journal */
			return retval;
	}
	if (retval == 2)   /* couldn't match the journal names, fail */
		return (Int2) 1;
	else if (retval == -2)
		return (Int2) -1;

	retval = ImprintMatch(a->imp, b->imp, FALSE);
	if (retval)
		return retval;
	return retval;
}

/*****************************************************************************
*
*   Int2 AuthListMatch(a, b, all)
*   	returns 0 = same first author
*   	1,-1 = different first author
*   	if (all) all authors must match
*
*****************************************************************************/
Int2 LIBCALL AuthListMatch(AuthListPtr a, AuthListPtr b, Boolean all)
{
	Int2 retval = 1, i;
	size_t len, tlen;
	Char name[2][40];
	ValNodePtr vnp[2];
	Uint1 listtype[2];
	AuthorPtr ap;
	PersonIdPtr pip;
	NameStdPtr nsp;



	if (a == NULL)
		return (Int2) -1;
	else if (b == NULL)
		return retval;

	vnp[0] = a->names;
	vnp[1] = b->names;
	listtype[0] = a->choice;
	listtype[1] = b->choice;
	while ((vnp[0] != NULL) && (vnp[1] != NULL))
	{
		len = 32000;
		for (i = 0; i < 2; i++)
		{
			name[i][0] = '\0';
			if (listtype[i] == 1)   /* std */
			{
				ap = (AuthorPtr)(vnp[i]->data.ptrvalue);
				pip = ap->name;
				if (pip->choice == 2)   /* std */
				{
					nsp = (NameStdPtr)pip->data;
					if (nsp->names[0] != NULL)   /* last name */
						StringMove(name[i], nsp->names[0]);
					else if (nsp->names[3] != NULL)  /* full name */
						StringMove(name[i], nsp->names[0]);
				}
				else if (pip->choice > 2)
					StringMove(name[i], (CharPtr)pip->data);
			}
			else
				StringMove(name[i], (CharPtr)(vnp[i]->data.ptrvalue));
			tlen = StringLen(name[i]);
			if (tlen < len)
				len = tlen;
			vnp[i] = vnp[i]->next;
		}
		if (len == 0)   /* nothing in one or more names */
		{
			if (StringLen(name[0]) < tlen)
				return (Int2)-1;
			else
				return (Int2) 1;
		}
		retval = (Int2)StringNICmp(name[0], name[1], tlen);
		if (retval < 0)
			return (Int2) -1;
		else if (retval > 0)
			return (Int2) 1;
		if (! all)   /* first only */
			return retval;
	}

	if (vnp[0] != NULL)   /* more names in a */
		return (Int2) 1;
	else if (vnp[1] != NULL)  /* more names in b */
		return (Int2) -1;
	else
		return (Int2) 0;
}

/*****************************************************************************
*
*   Int2 TitleMatch(a,b,type)
*	   if can find two titles of "type" returns
*   	0=they match
*   	1,-1 = they don't match
*   	2,-2 = couldn't find two of "type"
*   
*****************************************************************************/
Int2 LIBCALL TitleMatch( ValNodePtr a, ValNodePtr b, Uint1 type)
{
	Int2 retval = 2;
	Uint1 ca=0, cb=0;
	CharPtr t1=NULL, t2=NULL;
	


	if ((a == NULL) || (b == NULL))
		return retval;

	while (a != NULL)
	{
		if (a->choice == type)
		{
			t1 = (CharPtr)(a->data.ptrvalue);
			ca = type;
			break;
		}
		else if (a->choice > ca)
			ca = a->choice;
		a = a->next;
	}

	while (b != NULL)
	{
		if (b->choice == type)
		{
			t2 = (CharPtr)(b->data.ptrvalue);
			cb = type;
			break;
		}
		else if (b->choice > cb)
			cb = b->choice;
		b = b->next;
	}

	if (ca < cb)
		retval = -2;
	else
		retval = 2;

	if ((t1 == NULL) || (t2 == NULL))
		return retval;

	retval = StringICmp(t1, t2);
	if (retval > 0)
		return (Int2) 1;
	else if (retval < 0)
		return (Int2) -1;
	else
		return retval;
}

/*****************************************************************************
*
*   Int2 ImprintMatch(a, b, all)
*   	returns 0 = same
*   	1, -1 = different
*   	if (all) then all fields in one must be in the other
*   		else, matches only fields found in both
*       NOTE: does not match on Imprint.pub or Imprint.prepub
*
*****************************************************************************/
Int2 LIBCALL ImprintMatch (ImprintPtr a, ImprintPtr b, Boolean all)
{
	Int2 retval = 1, i;
	CharPtr ap, bp;



	if ((a == NULL) || (b == NULL))
		return retval;

	retval = DateMatch(a->date, b->date, all);
	if (retval == 2)
		retval = 1;
	else if (retval == -2)
		retval = -1;
	if (retval)
		return retval;

	for (i = 0; i < 6; i++)
	{
		switch (i)
		{
			case 0:
				ap = a->volume;
				bp = b->volume;
				break;
			case 1:
				ap = a->issue;
				bp = b->issue;
				break;
			case 2:
				ap = a->pages;
				bp = b->pages;
				break;
			case 3:
				ap = a->section;
				bp = b->section;
				break;
			case 4:
				ap = a->part_sup;
				bp = b->part_sup;
				break;
			case 5:
				ap = a->language;
				bp = b->language;
				break;

		}
		if ((ap != NULL) && (bp != NULL))
		{
			retval = StringICmp(ap, bp);
			if (retval > 0)
				return (Int2) 1;
			else if (retval < 0)
				return (Int2) -1;
		}
		else if (all)
		{
			if ((ap == NULL) && (bp != NULL))
				return (Int2) -1;
			else if ((ap != NULL) && (bp == NULL))
				return (Int2) 1;
		}
	}

	if ((a->cprt != NULL) || (b->cprt != NULL))
	{
		retval = DateMatch(a->cprt, b->cprt, all);
		if (retval == 2)
			retval = 1;
		else if (retval == -2)
			retval = -1;
		if (retval)
			return retval;
	}
	return retval;
}

#ifdef ASN30
/*****************************************************************************
*
*   PatPriorityNew()
*
*****************************************************************************/
PatPriorityPtr LIBCALL PatPriorityNew (void)
{
	PatPriorityPtr ppp;

	ppp = (PatPriorityPtr)MemNew(sizeof(PatPriority));
	return ppp;
}

/*****************************************************************************
*
*   PatPrioritySetFree()
*
*****************************************************************************/
PatPriorityPtr LIBCALL PatPrioritySetFree (PatPriorityPtr ppp)
{
	PatPriorityPtr next;

	while (ppp != NULL)
	{
		next = ppp->next;
		MemFree(ppp->country);
		MemFree(ppp->number);
		DateFree(ppp->date);
		MemFree(ppp);
		ppp = next;
	}
	return ppp;
}

/*****************************************************************************
*
*   PatPrioritySetAsnRead(aip, atp)
*
*****************************************************************************/
PatPriorityPtr LIBCALL PatPrioritySetAsnRead (AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element)
{
	PatPriorityPtr ppp=NULL, head, last;
	DataVal av;
	AsnTypePtr atp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return ppp;
	}

	if ((aip == NULL) || (set == NULL) || (element == NULL))
		return ppp;


	if (! AsnReadVal(aip, set, &av)) goto erret;   /* START_STRUCT */

	atp = set;
	head = NULL;
	last = NULL;

	while ((atp = AsnReadId(aip, amp, atp)) != set)
	{
		if (atp == NULL) goto erret;
		ppp = PatPriorityNew();
    	if (ppp == NULL)
        	goto erret;

		if (head == NULL)
			head = ppp;
		else
			last->next = ppp;
		last = ppp;

	    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;    /* read the open SEQUENCE */
    	atp = AsnReadId(aip, amp, atp );  /* country */
	    if (atp == NULL)
    	    goto erret;
	    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
    	ppp->country = av.ptrvalue;
	    atp = AsnReadId(aip, amp, atp );  /* number */
    	if (atp == NULL)
        	goto erret;
	    if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
		ppp->number = av.ptrvalue;
	    atp = AsnReadId(aip, amp, atp );  /* date */
    	if (atp == NULL)
        	goto erret;
		ppp->date = DateAsnRead(aip, atp);
		if (ppp->date == NULL) goto erret;
		atp = AsnReadId(aip, amp, atp);   /* end struct */
		if (atp == NULL) goto erret;
		if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
	}
    if (AsnReadVal(aip, atp, &av) <= 0) goto erret; /* end SEQUENCE OF */
ret:
	return head;
erret:
    head = PatPrioritySetFree(head);
    goto ret;
}

/*****************************************************************************
*
*   PatPrioritySetAsnWrite(ppp, aip, atp)
*
*****************************************************************************/
Boolean LIBCALL PatPrioritySetAsnWrite (PatPriorityPtr ppp, AsnIoPtr aip, AsnTypePtr set, AsnTypePtr element)
{
	DataVal av;
    Boolean retval = FALSE;
	PatPriorityPtr tmp;

	if (! loaded)
	{
		if (! BiblioAsnLoad())
			return FALSE;
	}

	if ((aip == NULL) || (set == NULL) || (element == NULL))
		return FALSE;

	if (ppp == NULL) { AsnNullValueMsg(aip, set); goto erret; }

    if (! AsnOpenStruct(aip, set, (Pointer)ppp))  /* open the SEQUENCE OF */
        goto erret;
	
	for (tmp = ppp; tmp != NULL; tmp = tmp->next)
	{
	    if (! AsnOpenStruct(aip, element, (Pointer)tmp))  /* open the SEQUENCE */
    	    goto erret;
	

	    av.ptrvalue = tmp->country;
	    if (! AsnWrite(aip, PATENT_PRIORITY_country, &av)) goto erret;
	    av.ptrvalue = tmp->number;
	    if (! AsnWrite(aip, PATENT_PRIORITY_number, &av)) goto erret;
		if (! DateAsnWrite(tmp->date, aip, PATENT_PRIORITY_date)) goto erret;

		if (! AsnCloseStruct(aip, element, (Pointer)tmp))
			goto erret;
	}

    if (! AsnCloseStruct(aip, set, (Pointer)ppp))    /* end SEQUENCE OF */
        goto erret;
    retval = TRUE;
erret:
	return retval;
}
#endif                    /* ASN30 */

