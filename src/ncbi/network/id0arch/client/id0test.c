/*  idtest.c
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

* Author Karl Sirotkin
*
$Log:	id0test.c,v $
 * Revision 1.3  93/12/02  10:12:41  kans
 * Includes <ncbi.h> instead of <sys/types.h>
 * 
 * Revision 1.2  93/11/24  13:25:56  sirotkin
 * First working version
 * 
 * Revision 1.1  93/11/23  16:01:51  sirotkin
 * Initial revision
 * 
 * 
*/
#include <ncbi.h>
#include <objsset.h>
#include "id0arch.h"

Args myargs[] = {
	{"Filename for asn.1 output ","stdout",
		NULL,NULL,FALSE,'a',ARG_FILE_OUT, 0.0,0,NULL},
 {"Output file is binary","F",NULL,NULL,TRUE,'b',ARG_BOOLEAN,0.0,0,NULL},
	{"GI id for single Entity to dump" ,NULL,"1","99999999",TRUE,'g',ARG_INT,0.0,0,NULL},
	{"Maximum complexity of Entity dump" ,"0","0","4",TRUE,'c',ARG_INT,0.0,0,NULL},
 {"flaTtened SeqId, format: \n		\'type(name,accession,release,version)\'\n			as \'5(HUMHBB)\' or \n		type=accession, or \n		type:number\n\n			",
		NULL,NULL,NULL,TRUE,'t',ARG_STRING,0.0,0,NULL},
	{"Log file for errors and feedback about IDs assigned",
		"/dev/null",NULL,NULL,TRUE,'l',ARG_FILE_OUT,
		0.0,0,NULL}
};
int Numarg = sizeof(myargs)/sizeof(myargs[0]);

#define MACRO_SETARG(TAG,P) \
   {\
      P = Nlm_WhichArg (TAG, Numarg, myargs);\
      if ( P < 0){\
         ErrPost(CTX_NCBIIDRETRIEVE,10,\
         "Program error looking for arg %c\n", TAG);\
         has_trouble = TRUE;\
      }\
   }
static Nlm_Int2 Nlm_WhichArg PROTO (( Nlm_Char which, Nlm_Int2 numargs, Nlm_ArgPtr ap));

DataVal Val;

Int2 Main()
{
	AsnIoPtr aip, check_aip = NULL;
	SeqEntryPtr sep = NULL, hold_entry;
	Int2 fileoutarg, logarg, binaryarg,maxplexarg,
		 seqidarg, giarg;
	Boolean has_trouble = FALSE;
	char * msg;
	Int4 entity_spec_count = 0;
	Int4 gi;
	AsnIoPtr asnout;
	Boolean is_live;
	

					/* check command line arguments */

	if ( ! GetArgs("idtest.c",Numarg, myargs))
		return 1;


/********************************************************************
 ****                                                            ****
 ****  Map Args So Can be Accessed in order independent fashion  ****
 ****                                                            ****
 *******************************************************************/

	MACRO_SETARG('a', fileoutarg)
	MACRO_SETARG('b', binaryarg)
	MACRO_SETARG('c', maxplexarg)
	MACRO_SETARG('g',giarg)
	MACRO_SETARG('t',seqidarg)
	MACRO_SETARG('l',logarg)

	if (! SeqEntryLoad())
		ErrShow();

    if (myargs[logarg].strvalue != NULL) {
			if (! ErrSetLog (myargs[logarg].strvalue)){
				ErrShow();
				has_trouble = TRUE;
			}else{
				ErrSetOpts (ERR_TEE, ERR_LOG_ON);
			}
    }

		if (myargs[giarg].intvalue){
			entity_spec_count ++;
		}
		if (myargs[seqidarg].strvalue){
			entity_spec_count ++;
		}
		if (entity_spec_count != 1){
			has_trouble = TRUE;
			ErrPost(CTX_NCBIIDRETRIEVE,10,
"One and only one of the -g, -s parameters must be used");
		}
   if ( (asnout= AsnIoOpen ( (CharPtr) myargs[fileoutarg].strvalue, 
         myargs[binaryarg].intvalue?"wb":"w"))
          == NULL){
         ErrPost(CTX_NCBIIDRETRIEVE,10,
            "Could not open %s for asn output\n",
            myargs[fileoutarg].strvalue);
         has_trouble = TRUE;
      }

	if ( has_trouble )
		exit (1);
	if ( ! ID0ArchInit()){
		ErrPost(CTX_NCBIIDRETRIEVE,20,
		"Could not open ID0 service");
		exit(1);
	}
	if (myargs[giarg].intvalue){
		gi = myargs[giarg].intvalue;
	}else{
/*  "flaTtened SeqId, format:
            type(name,accession,release,version) or type=accession",
   */
      static CharPtr name = NULL, accession = NULL, release = NULL, version = NULL, number = NULL;
      CharPtr p ;
      int type_int;
      static CharPtr PNTR fields [] = { & name, & accession, & release, & number};      
      Boolean found_equals = FALSE, found_left = FALSE, 
         found_colon = FALSE, flat_seqid_err = FALSE,
         dna_type = FALSE, any_type = FALSE;
      int dex;
      CharPtr sql_where, sql_and, temp_where, temp_and;
		SeqIdPtr sip = ValNodeNew(NULL);
		TextSeqIdPtr tsip;

		  type_int = atoi(myargs[seqidarg].strvalue);
      for (p = myargs[seqidarg].strvalue; *p; p++ ) {
         if ( *p == '(' || *p == '='  || *p == ':'){  /* ) match */
      
            if ( *p == '('  ){  /* ) match */
               found_left = TRUE;
               if (p == myargs[seqidarg].strvalue){
                  any_type = TRUE;
							ErrPost(CTX_NCBIIDRETRIEVE,10,
							"Sorry, any type is not allowed for ID0service");
							exit(1);
               }else if ( p - myargs[seqidarg].strvalue == 1){
                  if (*myargs[seqidarg].strvalue == '0'){
                     dna_type = TRUE;
							ErrPost(CTX_NCBIIDRETRIEVE,10,
							"Sorry, 0== nucroe3 type is not allowed for ID0service");
							exit(1);
                  }
               }
            }else if ( *p == '=') {
               found_equals = TRUE;
               if (p == myargs[seqidarg].strvalue){
                  any_type = TRUE;
               }else if ( p - myargs[seqidarg].strvalue == 1){
                  if (*myargs[seqidarg].strvalue == '0'){
                     dna_type = TRUE;
                  }
               }
            }else if ( *p == ':'){
               found_colon = TRUE;
               if (p == myargs[seqidarg].strvalue){
                  any_type = TRUE;
               }else if ( p - myargs[seqidarg].strvalue == 1){
                  if (*myargs[seqidarg].strvalue == '0'){
                     dna_type = TRUE;
                  }
               }
            }
            *p = '\0';
            p++;
            break;
         }
      }
      if ( found_left){
         for ( * (fields[0]) = p, dex = 0; *p && dex < 4; p++){
            if ( *p == ',' || *p == ')' ){
                *p = '\0';
               dex ++;
               *(fields[dex]) = p + 1;
            }
         }
      }else if (found_equals){
         accession = p;
      }else if (found_colon){
         number = p;
      }else{
            ErrPost(CTX_NCBIIDRETRIEVE, 10,
            "id0test: could not find \'(\' or \'=\' or \':\' in flattened seqid=%s",myargs[seqidarg].strvalue);  /* ) match */
					exit(1);
      }
		sip -> choice = type_int;
		switch (type_int){
		case SEQID_GIBBSQ :
		case SEQID_GIBBMT :
		case SEQID_GI :
			sip -> data.intvalue = atoi(number);
			break;
		case SEQID_GENBANK : case SEQID_EMBL : case SEQID_DDBJ :
		case SEQID_PIR : case SEQID_SWISSPROT : case SEQID_OTHER :
		case SEQID_PRF :
			tsip = TextSeqIdNew();
			sip -> data.ptrvalue = tsip;
			if (accession)
				if (!*accession)
					accession = NULL;
			if (release)
				if (!*release)
					release = NULL;
			if (name)
				if (!*name)
					name = NULL;
			tsip -> name = StringSave(name);
			tsip -> accession = StringSave(accession);
			tsip -> release = StringSave(release);
			break;
		case SEQID_PATENT : case SEQID_GENERAL : case SEQID_PDB :
		case SEQID_LOCAL :
		ErrPost(CTX_NCBIIDRETRIEVE,30,
		"Sorry, this test program does not support %d patent, general, pdb, or local, try id2asn ", 
			type_int);
			exit(1);
			break;
		}
		gi = ID0ArchGIGet (sip);
		fprintf(stderr, " gi = %d returned\n", gi);
		if (gi <= 0)
			exit(1);
	}
	sep = ID0ArchSeqEntryGet (gi, & is_live, (Int2) myargs[maxplexarg].intvalue);
	if ( ! is_live)
		fprintf(stderr," IS DEAD!\n");

	SeqEntryAsnWrite(sep, asnout, NULL);
	SeqEntryFree(sep);
FATAL:
	AsnIoClose(asnout);
	ID0ArchFini();

	return(has_trouble?1:0);
}

/*****************************************************************************
*
*   Nlm_WhichArg(ap)
*     returns array position for a tag 
*
*****************************************************************************/
static Nlm_Int2 Nlm_WhichArg( Nlm_Char which, Nlm_Int2 numargs, Nlm_ArgPtr ap)
{
   Nlm_Boolean okay = FALSE;
   Nlm_Int2 i;
   Nlm_ArgPtr curarg;
   Nlm_Int2 retval = -1;

   if ((ap == NULL) || (numargs == 0) )
      return okay;

   curarg = ap;                        /* set defaults */

   for (i = 0; i < numargs; i++, curarg++)
   {
      if (curarg->tag == which)
      {
         retval = i;
         break;
      }
   }

   return retval;
}

