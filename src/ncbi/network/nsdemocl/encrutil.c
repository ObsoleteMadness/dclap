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
* File Name:    encrutil.c
*
* Author:       Epstein
*
* Version Creation Date:        2/14/94
*
* $Revision: 1.1 $
*
* File Description:
*   Encryption utility standalone program, to perform many simple encryption
*   tasks and validate encryption operation on a given machine.
*
*
* Modifications:
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*/

#include <ncbi.h>
#include <asn.h>
#include <ncbinet.h>

#define RSA_ENCR_TEXT "The quick"
#define DES_ENCR_TEXT "The quick brown fox jumped over the lazy dog"

Args myargs[] = {
        {"Bits in public key modulus", "508","508", "1024", TRUE,'b',ARG_INT,0.0,0,NULL},
        {"pUblic key file", "dispd.pubkey", NULL, NULL, TRUE, 'u', ARG_STRING,0.0,0,NULL},
        {"pRivate key file", "dispd.privkey", NULL, NULL, TRUE, 'r', ARG_STRING,0.0,0,NULL},
        {"password for des Encryption of private key", NULL, NULL, NULL, TRUE, 'e', ARG_STRING,0.0,0,NULL},
        {"perform Validation", "F", NULL, NULL, TRUE, 'v', ARG_BOOLEAN, 0.0,0,NULL},
        {"additional Help", "F", NULL, NULL, TRUE, 'h', ARG_BOOLEAN, 0.0,0,NULL}};

static void
PrintHelp(void)
{
    printf ("\nThis program generates public-encryption keys for use in the NCBI network\n");
    printf ("services paradigm.  It can also be used to validate the operation of DES\n");
    printf ("and public-key encryption on the computer where this program is run.\n\n");
    printf ("If the '-v' option (validation) is specified, the program randomly generates a\n");
    printf ("DES key and a public-private RSA pair, and tests encryption and decryption under\n");
    printf ("both DES and RSA.  The normal output of keys is not performed in this case.\n\n");
    printf ("If the '-e' (password for DES Encryption of private key) option is used, the\n");
    printf ("specified password is encoded (but not encrypted!) and displayed upon the\n");
    printf ("standard output, while the private RSA key is encrypted using that encoded DES\n");
    printf ("key and written to the private key output file.  This option provides a way to\n");
    printf ("store a private key in a file without fear that compromise of that file\n");
    printf ("will immediately result in compromise of the private key.\n\n");
}

Int2
Main()
{
   int Numarg = sizeof(myargs)/sizeof(Args);
   AsnIoPtr pubAip;
   NI_PubKeyPtr pub1;
   VoidPtr privKey;
   FILE *fp;
   UcharPtr text;
   UcharPtr cipherText = NULL;
   UcharPtr plainText = NULL;
   Int2 cLen, newlen;
   CharPtr privFileName;
   CharPtr pubFileName;
   CharPtr desPassword = NULL;
   int bits;

   if (! NI_EncrAvailable())
   {
        printf ("The NCBI toolkit does not support encryption in the manner in which this\n");
        printf ("executable was prepared.  To support encryption, be sure that ni_encr.c appears\n");
        printf ("in the link, rather than ni_encrs.c\n");
        return -1;
   }

   if ( ! GetArgs("Encryption utilities $Revision: 1.1 $", Numarg, myargs))
        return 1;

   if (myargs[5].intvalue)
   {
       PrintHelp();
       return 0;
   }

   bits = myargs[0].intvalue;
   pubFileName = myargs[1].strvalue;
   privFileName = myargs[2].strvalue;
   if (myargs[3].strvalue != NULL && myargs[3].strvalue[0] != '\0')
   {
       desPassword = myargs[3].strvalue;
   }


#ifdef NOT_REALPROG
   pubAip = AsnIoOpen("dispd.pubkeytest", "r");
   pub1 = (NI_PubKeyPtr) NI_MakePubKey();
   NI_ReadPubKey(pubAip, NULL, pub1);
   AsnIoClose(pubAip);
   if ((cLen = NI_PubKeyEncrypt(pub1, text, StrLen(text) + 1, &cipherText)) < 0)
   {
       fprintf (stderr, "Encryption failed (%d)\n", cLen);
       exit (1);
   }
   fp = FileOpen("dispd.privkeytest", "r");
   privKey = NI_LoadPrivKey(fp);
   FileClose(fp);
   if ((newlen = NI_PubKeyDecrypt(privKey, &plainText, cipherText, cLen)) < 0)
   {
       fprintf (stderr, "Decryption failed (%d)\n", newlen);
       exit (1);
   }
   MemFree(privKey);
   fprintf (stderr, "Decrypted text is %s, length is %d\n", plainText, newlen);
   MemFree (plainText);
   MemFree (cipherText);
   NI_DestroyPubKey (pub1);

#else
   pubAip = AsnIoOpen(pubFileName, "w");
   fp = FileOpen(privFileName, "w");
   NI_GenAndWritePEMKeys(bits, pubAip, fp);
   AsnIoClose(pubAip);
   FileClose(fp);
#endif
}
