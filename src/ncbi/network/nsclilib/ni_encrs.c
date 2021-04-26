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
* File Name:    ni_encrs.c
*
* Author:       Epstein
*
* Version Creation Date:        2/14/94
*
* $Revision: 1.3 $
*
* File Description:
*   Stub replacement for ni_encr.c.  This makes it easy to build NCBI Network
*   Services tools either with or without encryption support.
*
* Modifications:
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 2/15/94  Epstein     Replaced nice macro with explicit definitions, due to
*                      inconsistency involving C pre-processors.
*/

#include <ncbi.h>
#include <ni_types.h>


/* this is the only function which may be called legally */
Boolean LIBCALL NI_EncrAvailable (void)
{ return FALSE; }

Boolean LIBCALL NI_SetupDESEncryption (NI_HandPtr mh, UcharPtr desKey)
{ ErrPostEx(SEV_ERROR,0,0,"Invalid call to encryption function NI_SetupDESEncryption"); return FALSE; }

Boolean LIBCALL NI_PubKeysEqual (NI_PubKeyPtr x, NI_PubKeyPtr y)
{ ErrPostEx(SEV_ERROR,0,0,"Invalid call to encryption function NI_PubKeysEqual"); return FALSE; }

void NI_GenerateDESKey (UcharPtr desKey)
{ ErrPostEx(SEV_ERROR,0,0,"Invalid call to encryption function NI_GenerateDESKey"); }

Boolean LIBCALL NI_GenAndWritePEMKeys (Int2 bits, AsnIoPtr pubAip, FILE *privFp)
{ ErrPostEx(SEV_ERROR,0,0,"Invalid call to encryption function NI_GenAndWritePEMKeys"); return FALSE; }

Boolean LIBCALL NI_WritePubKeyToConfig (NI_PubKeyPtr pub)
{ ErrPostEx(SEV_ERROR,0,0,"Invalid call to encryption function NI_WritePubKeyToConfig"); return FALSE; }

NI_PubKeyPtr LIBCALL NI_ReadPubKeyFromConfig (void)
{ ErrPostEx(SEV_ERROR,0,0,"Invalid call to encryption function NI_ReadPubKeyFromConfig"); return NULL; }

NI_PubKeyPtr LIBCALL NI_PubKeyDup (NI_PubKeyPtr orig)
{ ErrPostEx(SEV_ERROR,0,0,"Invalid call to encryption function NI_PubKeyDup"); return NULL; }

VoidPtr LIBCALL NI_LoadPrivKey (FILE *fp)
{ ErrPostEx(SEV_ERROR,0,0,"Invalid call to encryption function NI_LoadPrivKey"); return NULL; }

Int2 LIBCALL NI_PubKeyDecrypt (VoidPtr pKey, UcharPtr PNTR plainText, UcharPtr cipherText, Int2 cipherTextLen)
{ ErrPostEx(SEV_ERROR,0,0,"Invalid call to encryption function NI_PubKeyDecrypt"); return -1; }

Int2 LIBCALL NI_PubKeyEncrypt (NI_PubKeyPtr pub, UcharPtr plainText, Int2 plainTextLen, UcharPtr PNTR cipherText)
{ ErrPostEx(SEV_ERROR,0,0,"Invalid call to encryption function NI_PubKeyDecrypt"); return -1; }

void NI_DestroyEncrStruct (NI_EncrDataPtr encr)
{ ErrPostEx(SEV_ERROR,0,0,"Invalid call to encryption function NI_DestroyEncrStruct"); }
