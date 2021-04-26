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
* File Name:    ni_encr.h
*
* Author:       Epstein
*
* Version Creation Date:        2/14/94
*
* $Revision: 4.0 $
*
* File Description:
*   Header for for RSAREF-based encryption support in NCBI network services
*
*
* Modifications:
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* RCS Modification History:
* $Log: ni_encr.h,v $
 * Revision 4.0  1995/07/26  13:56:32  ostell
 * force revision to 4.0
 *
 * Revision 1.4  1995/05/17  17:52:03  epstein
 * add RCS log revision history
 *
*/

#ifndef _NI_ENCR_
#define _NI_ENCR_

Boolean LIBCALL NI_SetupDESEncryption PROTO((NI_HandPtr mh, UcharPtr desKey));
Boolean LIBCALL NI_PubKeysEqual PROTO((NI_PubKeyPtr x, NI_PubKeyPtr y));
void NI_GenerateDESKey PROTO((UcharPtr desKey));
Boolean LIBCALL NI_GenAndWritePEMKeys PROTO((Int2 bits, AsnIoPtr pubAip, FILE *privFp));
Boolean LIBCALL NI_WritePubKeyToConfig PROTO((NI_PubKeyPtr pub));
NI_PubKeyPtr LIBCALL NI_ReadPubKeyFromConfig PROTO((void));
NI_PubKeyPtr LIBCALL NI_PubKeyDup PROTO((NI_PubKeyPtr orig));
VoidPtr LIBCALL NI_LoadPrivKey PROTO((FILE *fp, Int2Ptr privKeyLenPtr));
Int2 LIBCALL NI_PubKeyDecrypt PROTO((VoidPtr pKey, UcharPtr PNTR plainText, UcharPtr cipherText, Int2 cipherTextLen));
Int2 LIBCALL NI_PubKeyEncrypt PROTO((NI_PubKeyPtr pub, UcharPtr plainText, Int2 plainTextLen, UcharPtr PNTR cipherText));
void NI_DestroyEncrStruct PROTO((NI_EncrDataPtr encr));
Boolean LIBCALL NI_EncrAvailable PROTO((void));



#endif /* _NI_ENCR_ */

