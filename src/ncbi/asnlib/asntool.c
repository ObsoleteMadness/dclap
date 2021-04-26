/*  asntool.c
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
* File Name: asntool.c
*
* Author:  James Ostell
*
* Version Creation Date: 1/1/91
*
* $Revision: 2.1 $
*
* File Description:
*   Main routine for asntool.  Uses the ASN.1 library routines to perform
*   analysis and display of ASN.1 module specifications and associated
*   value files.  Can also produce the ASN.1 parse tree as a C header file.
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/
/*****************************************************************************
*
*   asntool.c
*   	-m ModuleFile  	(an asn.1 specification)
*   	-v ValueFile 	(print values for input)
*   	-o OutputFile	(for module structs header)
*   	-e EncodeFile	(for output of BER encode value)
*   	-d DecodeFile	(for input of BER encoded value)
*   	-p PrintFile    (print value to file)
*       -f ModoutFile   (print modules to file)
*       -l Loadable File + header file
*       -b Size         (set AsnIo buffer size)
*   
*   	read, validate, and display an asn1 module specification
*
*****************************************************************************/

#include "asnbuild.h"

extern void AsnTxtReadValFile PROTO((AsnModulePtr amp, AsnIoPtr aip, AsnIoPtr aipout, AsnIoPtr encode));
extern void AsnBinReadValFile PROTO((AsnTypePtr atp, AsnIoPtr aip, AsnIoPtr aipout, AsnIoPtr encode));

#define NUMARGS 10

Args asnargs[NUMARGS] = {
	{"ASN.1 Module File",NULL,NULL,NULL,FALSE,'m',ARG_FILE_IN,0.0,0,NULL},
	{"ASN.1 Module File", NULL,NULL,NULL,TRUE,'f',ARG_FILE_OUT,0.0,0,NULL},
	{"Print Value File",NULL,NULL,NULL,TRUE,'v',ARG_FILE_IN,0.0,0,NULL},
	{"Print Value File",NULL,NULL,NULL,TRUE,'p',ARG_FILE_OUT,0.0,0,NULL},
	{"Binary Value File (type required)",NULL,NULL,NULL,TRUE,'d',ARG_FILE_IN,0.0,0,NULL},
	{"Binary Value Type",NULL,NULL,NULL,TRUE,'t',ARG_STRING,0.0,0,NULL},
	{"Binary Value File",NULL,NULL,NULL,TRUE,'e',ARG_FILE_OUT,0.0,0,NULL},
	{"Header File",NULL,NULL,NULL,TRUE,'o',ARG_FILE_OUT,0.0,0,NULL},
	{"Loader File",NULL,NULL,NULL,TRUE,'l',ARG_FILE_OUT,0.0,0,NULL},
    {"Buffer Size","1024","512","10000",TRUE,'b',ARG_INT,0.0,0,NULL}};

Int2 Main (void)

{
	AsnIoPtr aip = NULL,
		aipout = NULL,
		aipencode = NULL;
	AsnModulePtr amp = NULL,
				currentmod = NULL,
				nextmod;
	AsnTypePtr atp;

	                          /* never abort on error, but show it */
	ErrSetFatalLevel(SEV_MAX);

	if (! GetArgs("AsnTool 3", NUMARGS, asnargs))
		return 1;

    if (! AsnIoSetBufsize(NULL, (Int2)asnargs[9].intvalue))
        return 1;

	if ((aip = AsnIoOpen(asnargs[0].strvalue, "r")) == NULL)
		return 1;

	if (asnargs[6].strvalue != NULL)    /* output a binary value file */
		if ((aipencode = AsnIoOpen(asnargs[6].strvalue, "wb")) == NULL)
			return 1;

				/**  parse the module(s)  ***/
	
	if (asnargs[1].strvalue != NULL)
	{
		if ((aipout = AsnIoOpen(asnargs[1].strvalue, "w")) == NULL)
			return 1;
	}

	while ((nextmod = AsnLexTReadModule(aip)) != NULL )
	{
		if (aipout != NULL)
			AsnPrintModule(nextmod, aipout);

		if (amp == NULL)
			amp = nextmod;
		else
			currentmod->next = nextmod;
		currentmod = nextmod;
	}
	aip = AsnIoClose(aip);

	AsnModuleLink(amp);      /* link up modules where possible */
	
	aipout = AsnIoClose(aipout);

                             /* print a value file */

	if (asnargs[3].strvalue != NULL)
	{
		if ((aipout = AsnIoOpen(asnargs[3].strvalue, "w")) == NULL)
			return 1;
	}

	if (asnargs[2].strvalue != NULL)        /* read a printvalue file */
	{
		if ((aip = AsnIoOpen(asnargs[2].strvalue, "r")) == NULL)
			return 1;

		AsnTxtReadValFile(amp, aip, aipout, aipencode);
	}

	aip = AsnIoClose(aip);

	if (asnargs[4].strvalue != NULL)        /* read a ber file */
	{
		if ((asnargs[5].strvalue == NULL) || (! TO_UPPER(*asnargs[5].strvalue)))
			ErrPost(CTX_NCBIASN1, 100, "Must use -t Type to define contents of decode file");

		atp = AsnTypeFind(amp, asnargs[5].strvalue);
		if (atp == NULL)
#ifdef WIN_MSWIN
			ErrPost(CTX_NCBIASN1, 101, "Couldn't find Type %Fs", asnargs[5].strvalue);
#else
			ErrPost(CTX_NCBIASN1, 101, "Couldn't find Type %s", asnargs[5].strvalue);
#endif

		if ((aip = AsnIoOpen(asnargs[4].strvalue, "rb")) == NULL)
			return 1;


		AsnBinReadValFile(atp, aip, aipout, aipencode);
	}

	AsnIoClose(aipout);
	AsnIoClose(aip);
	AsnIoClose(aipencode);

	if (asnargs[7].strvalue != NULL)         /* produce header file */
		AsnOutput(asnargs[7].strvalue, amp, FALSE);

    if (asnargs[8].strvalue != NULL)        /* produce loader file */
        AsnOutput(asnargs[8].strvalue, amp, TRUE);

	return 0;
}
/*****************************************************************************
*
*   void AsnTxtReadValFile(amp, aip, aipout, aipencode)
*   	reads a file of values
*   	prints to aipout if aipout != NULL
*
*****************************************************************************/
void AsnTxtReadValFile (AsnModulePtr amp, AsnIoPtr aip, AsnIoPtr aipout, AsnIoPtr aipencode)

{
	AsnTypePtr atp;
	DataVal value;
	Boolean read_value, print_value, encode_value, restart;

	if (aipout != NULL)
		print_value = TRUE;
	else
		print_value = FALSE;

	if (aipencode != NULL)
		encode_value = TRUE;
	else
		encode_value = FALSE;

	if (print_value || encode_value)
		read_value = TRUE;
	else
		read_value = FALSE;


	atp = NULL;
	restart = FALSE;

	while ((atp = AsnTxtReadId(aip, amp, atp)) != NULL)
	{
		if (restart == TRUE)
		{
			if (encode_value)
			{
				ErrPost(CTX_NCBIASN1, 102, "Cannot binary encode multiple values in one message");
				return;
			}
			if (print_value)         /* new line */
			{
				AsnPrintNewLine(aipout);
				AsnPrintNewLine(aipout);
			}
			restart = FALSE;
		}

		if (read_value)
		{
			if (! AsnTxtReadVal(aip, atp, &value))
				return;
			if (print_value)
			{
				if (! AsnTxtWrite(aipout, atp, &value))
					return;
			}
			if (encode_value)
			{
				if (! AsnBinWrite(aipencode, atp, &value))
					return;
			}
			AsnKillValue(atp, &value);
		}
		else
		{
			if (! AsnTxtReadVal(aip, atp, NULL))
				return;
		}

		if (! aip->type_indent)      /* finished reading an object */
		{
			atp = NULL;              /* restart */
			restart = TRUE;
		}
	}
	return;
}

/*****************************************************************************
*
*   void AsnBinReadValFile(aip, aipout, aipencode)
*   	reads a file of binary values
*       MUST start knowing the Type of the value
*   	prints to aipout if aipout != NULL
*
*****************************************************************************/
void AsnBinReadValFile (AsnTypePtr atp, AsnIoPtr aip, AsnIoPtr aipout, AsnIoPtr aipencode)        /* type of message */

{
	DataVal value;
	Boolean read_value, print_value, encode_value;

	if (aipout != NULL)
		print_value = TRUE;
	else
		print_value = FALSE;

	if (aipencode != NULL)
		encode_value = TRUE;
	else
		encode_value = FALSE;

	if (print_value || encode_value)
		read_value = TRUE;
	else
		read_value = FALSE;


	while ((atp = AsnBinReadId(aip, atp)) != NULL)
	{
		if (read_value)
		{
			if (! AsnBinReadVal(aip, atp, &value))
				return;
			if (print_value)
			{
				if (! AsnTxtWrite(aipout, atp, &value))
					return;
			}
			if (encode_value)
			{
				if (! AsnBinWrite(aipencode, atp, &value))
					return;
			}
			AsnKillValue(atp, &value);
		}
		else
			AsnBinReadVal(aip, atp, NULL);
	}
	return;
}



