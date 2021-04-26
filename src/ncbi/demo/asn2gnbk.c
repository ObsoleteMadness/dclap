/*****************************************************************************
*
*   asn2gnbk.c
*   	convert a Seq-entry or elements of a Bioseq-set to GenBank format
*
*****************************************************************************/
#include <togenbnk.h>
#include <objsub.h>

Args myargs[] = {
	{"Filename for asn.1 input","stdin",NULL,NULL,TRUE,'a',ARG_FILE_IN,0.0,0,NULL},
	{"Input is a Seq-entry","F", NULL ,NULL ,TRUE,'e',ARG_BOOLEAN,0.0,0,NULL},
	{"Input is a Seq-submit","F", NULL ,NULL ,TRUE,'s',ARG_BOOLEAN,0.0,0,NULL},
	{"Input asnfile in binary mode","F",NULL,NULL,TRUE,'b',ARG_BOOLEAN,0.0,0,NULL},
	{"Suppress production of flat file when making indeces","F",NULL,NULL,TRUE,'c',ARG_BOOLEAN,0.0,0,NULL},
#ifdef OLD_ARGS_FOR_HISTORY
	{"Suppress production of flat file when making indeces","F",NULL,NULL,TRUE,'s',ARG_BOOLEAN,0.0,0,NULL},
	{"make aCcession raw index file","F",NULL,NULL,TRUE,'c',ARG_BOOLEAN,0.0,0,NULL},
	{"make aUthor raw index file","F",NULL,NULL,TRUE,'u',ARG_BOOLEAN,0.0,0,NULL},
	{"make Keyword raw index file","F",NULL,NULL,TRUE,'k',ARG_BOOLEAN,0.0,0,NULL},
	{"make Journal citation raw index file","F",NULL,NULL,TRUE,'j',ARG_BOOLEAN,0.0,0,NULL},
	{"make geNe citaiton raw index file","F",NULL,NULL,TRUE,'n',ARG_BOOLEAN,0.0,0,NULL},
#endif
	{"make all (accession, author, keyword, jornal citation, gene) raw index file","F",NULL,NULL,TRUE,'i',ARG_BOOLEAN,0.0,0,NULL},
	{"make header in front of flat file","F",NULL,NULL,TRUE,'h',ARG_BOOLEAN,0.0,0,NULL},
	{"header full databank name, line 1 20-??", NULL,NULL, NULL, TRUE,'v', ARG_STRING,0.0,0,NULL},
	{"header date, line 2 26-??, as 1 January 1992", NULL,NULL, NULL, TRUE,'w', ARG_STRING,0.0,0,NULL},
	{"header release name, line 4 \?\?-39", NULL,NULL, NULL, TRUE,'x', ARG_STRING,0.0,0,NULL},
	{"header release number, line 4 41-45", NULL,"00.00", "99.99", TRUE,'y', ARG_STRING,0.0,0,NULL},
	{"header file title, line 1 20-??", NULL,NULL, NULL, TRUE,'z', ARG_STRING,0.0,0,NULL},
	{"Output Filename","stdout", NULL,NULL,TRUE,'f',ARG_FILE_OUT,0.0,0,NULL},
	{"Division","???", "AAA" ,"ZZZ" ,TRUE,'d',ARG_STRING,0.0,0,NULL},
	{"Default Date",NULL, NULL, NULL ,TRUE,'g',ARG_STRING,0.0,0,NULL},
#ifdef T10_and_CDROM
			{"Alter format for export to T-10","F", NULL ,NULL ,TRUE,'t',ARG_BOOLEAN,0.0,0,NULL},
			{"Alter format for CdRom output from Backbone ASN.1","F", NULL ,NULL ,TRUE,'r',ARG_BOOLEAN,0.0,0,NULL},
#endif
	{"Accept a stream of repeating messages","F",NULL ,NULL ,TRUE,'p',ARG_BOOLEAN,0.0,0,NULL},        
	{"Run quietly","F",NULL ,NULL ,TRUE,'q',ARG_BOOLEAN,0.0,0,NULL},        
#ifdef ASN30
	{"Alter OUTPUT format according to code:\n   1 - embl format\n   2 - embl format, but GenBank sequences\n   3 - embl with sequences on right (new embl)\n   4 - Patent format",NULL, "1" ,"4" ,TRUE,'m',ARG_INT,0.0,0,NULL},
#else
	{"Alter OUTPUT format according to code:\n   1 - embl format\n   2 - embl format, but GenBank sequences\n   3 - embl with sequences on right (new embl)",NULL, "0" ,"3" ,TRUE,'m',ARG_INT,0.0,0,NULL},
#endif
	{"Produce Nucleotide and/or Protein sequences\n   1 - Nucleotide only\n   2 - Protein only\n   3 - both ","1", "1" ,"3" ,TRUE,'n',ARG_INT,0.0,0,NULL},
	{"RELEASE fOrmatting, now kills mutants of hybrids sets","F", NULL ,NULL ,TRUE,'o',ARG_BOOLEAN,0.0,0,NULL},
 {"Log errors to file named:",NULL,NULL,NULL,TRUE,'l',ARG_FILE_OUT, 0.0,0,NULL}};

Nlm_Int2 Nlm_WhichArg PROTO((Nlm_Char which, Nlm_Int2 numargs, Nlm_ArgPtr ap));
int try_again PROTO((int num_errors, int expect_repeating_stream));
void FlatSpliceOn PROTO((SeqEntryPtr the_set, ValNodePtr desc));
void FlatSpliceoff PROTO((SeqEntryPtr the_set, ValNodePtr desc));
int FlatSpliceOff PROTO((SeqEntryPtr the_set, ValNodePtr desc));
int Numarg = sizeof(myargs)/sizeof(Args);
#define MACRO_SETARG(TAG,P) \
   {\
      P = Nlm_WhichArg (TAG, Numarg, myargs);\
      if ( P < 0){\
         ErrPost(CTX_NCBI2GB,CTX_2GB_INTERNAL,\
         "Program error looking for arg %c\n", TAG);\
         has_trouble = TRUE;\
      }\
   }


Int2 Main()
{
	AsnIoPtr aip;
	SeqEntryPtr the_set;
	AsnTypePtr atp, atp2;
	AsnModulePtr amp;
	DataVal div_val, quiet_val, no_ncbi_val, date_val, index_val,
		header_val, format_val, nuc_prot_val;
	FILE * FpOut;
	AsnOptionPtr optionHead = NULL;
	Int2 binaryarg, asnfilearg, isseqentryarg, isseqsubmitarg, 
		outfilenamearg, divarg, datearg, headerarg, logarg, 
		accessionarg, allindexarg, genearg, journalarg, 
		keywordarg, authorarg, suppressarg,releasearg, nucl_protarg,
		headerdbnamearg, headerdatearg, headerrelnamearg, headerrelnumarg,
		quietarg, headernamearg, streamarg, output_format_arg;
	
#ifdef T10_and_CDROM
	Int2 t10arg, cdromarg;
#endif

	char index_string [15];
	char header_upper_name [10];
	int i, num_errors;
	long byte_start;

	Boolean has_trouble = FALSE;
	OptionHeader header_info;
	char * fmt = "%7ld loci, %8ld bases, from %5ld reported sequences\n\n";

	CitSubPtr the_cit;
	ValNode citsub;
	AsnTypePtr SEQ_SUBMIT, SEQ_SUBMIT_cit, SEQ_SUBMIT_data_entrys_E;
	ValNodePtr splicing_desc = NULL;
	PubdescPtr pdp = NULL;

					/* check command line arguments */


	if ( ! GetArgs("asn2gnbk $Revision: 2.9 $",Numarg, myargs))
		return 1;


	MACRO_SETARG('a', asnfilearg)
	MACRO_SETARG('b', binaryarg)
	MACRO_SETARG('c', suppressarg)
	MACRO_SETARG('d', divarg)
	MACRO_SETARG('e', isseqentryarg)
	MACRO_SETARG('f', outfilenamearg)
	MACRO_SETARG('g', datearg)
	MACRO_SETARG('h', headerarg)
	MACRO_SETARG('i', allindexarg)
	MACRO_SETARG('l', logarg)
	MACRO_SETARG('m', output_format_arg)
	MACRO_SETARG('n', nucl_protarg)
	MACRO_SETARG('o', releasearg)
	MACRO_SETARG('p', streamarg)
	MACRO_SETARG('q', quietarg)
	MACRO_SETARG('s', isseqsubmitarg)

#ifdef T10_and_CDROM
	MACRO_SETARG('r', cdromarg)
	MACRO_SETARG('t', t10arg)
#endif
	MACRO_SETARG('v', headerdbnamearg)
	MACRO_SETARG('w', headerdatearg)
	MACRO_SETARG('x', headerrelnamearg)
	MACRO_SETARG('y', headerrelnumarg)
	MACRO_SETARG('z', headernamearg)

#ifdef OLD_ARGS_FOR_HISTORY
		MACRO_SETARG('j', journalarg)
		MACRO_SETARG('n', genearg)  /* note, now used for nuc/prot switch */
		MACRO_SETARG('c', accessionarg)  /* note, now used for suppress */
		MACRO_SETARG('k', keywordarg)
		MACRO_SETARG('u', authorarg)
#endif
					/* load the sequence alphabets  */
					/* (and sequence parse trees)   */
	if (! SeqEntryLoad()){
		ErrShow();
		has_trouble = TRUE;
	}

	/** Seq-submit stuff ***/


	if (! SubmitAsnLoad()) {
		ErrShow();
		has_trouble = TRUE;
	}

	SEQ_SUBMIT = AsnFind("Seq-submit");
	SEQ_SUBMIT_cit = AsnFind("Seq-submit.sub.cit");
	if (SEQ_SUBMIT_cit == NULL){
			ErrPost(CTX_NCBI2GB,CTX_2GB_ARG_TROUBLE,"Could not fine SEQ_SUBMIT_cit");
	}
	SEQ_SUBMIT_data_entrys_E = AsnFind("Seq-submit.data.entrys.E");

	/********************/

				    /* get pointer to all loaded ASN.1 modules */
	amp = AsnAllModPtr();
	if (amp == NULL){
		ErrShow();
		has_trouble = TRUE;
	}

	atp = AsnFind("Bioseq-set");    /* get the initial type pointers */
	if (atp == NULL){
		ErrShow();
		has_trouble = TRUE;
	}
	atp2 = AsnFind("Bioseq-set.seq-set.E");
	if (atp2 == NULL){
		ErrShow();
		has_trouble = TRUE;
	}


					/* open the ASN.1 input file in the right mode */

	if ((aip = AsnIoOpen (myargs[asnfilearg].strvalue, 
			myargs[binaryarg].intvalue?"rb":"r")) == NULL){
		ErrShow();
		has_trouble = TRUE;
	}

				  				/* open the output file */

	if ((FpOut = FileOpen (myargs[outfilenamearg].strvalue, "w+")) == NULL){
		ErrShow();
		ErrPost(CTX_NCBI2GB,CTX_2GB_ARG_TROUBLE,
			"Could not open output filename <%s> for header generation",
			myargs[outfilenamearg].strvalue);
		has_trouble = TRUE;
	}

                                /* log errors instead of die */
    if (myargs[logarg].strvalue != NULL) {
        if (! ErrSetLog (myargs[logarg].strvalue))
            ErrShow();
        else
            ErrSetOpts (ERR_TEE, ERR_LOG_ON);
    }

						/* set the togenbnk options */

	if (myargs[quietarg].intvalue != 0){ 	/* run quietly? */
		quiet_val.intvalue = TRUE;
		AsnOptionNew ( & optionHead, OP_TOGENBNK, OP_TOGENBNK_QUIET, 
			quiet_val, (AsnOptFreeFunc) NULL);
	}

								/* never report ncbi detected errors */
	no_ncbi_val.intvalue = TRUE;
	AsnOptionNew ( & optionHead, OP_TOGENBNK, OP_TOGENBNK_NO_NCBI, 
		no_ncbi_val, (AsnOptFreeFunc) NULL);

	if (myargs[divarg].strvalue != NULL)   /* set the division */
	{
		div_val.ptrvalue = StringSave (myargs[divarg].strvalue);
		AsnOptionNew ( & optionHead, OP_TOGENBNK, OP_TOGENBNK_DIV, 
			div_val, (AsnOptFreeFunc) MemFree);
	}

	if (myargs[datearg].strvalue != NULL) 	/* set default date */
	{
		date_val.ptrvalue = StringSave (myargs[datearg].strvalue);
		AsnOptionNew ( & optionHead, OP_TOGENBNK, OP_TOGENBNK_DATE, 
			date_val, (AsnOptFreeFunc) MemFree);
	}


#ifdef T10_and_CDROM
	if (myargs[t10arg].intvalue != 0) 	/* For T-10 ?? */
	{
		quiet_val.intvalue = TRUE;
		AsnOptionNew ( & optionHead, OP_TOGENBNK, OP_TOGENBNK_T10, 
			quiet_val, (AsnOptFreeFunc) NULL);
	}

	if (myargs[cdromarg].intvalue != 0) 	/* For CdRom generation ?? */
	{
		quiet_val.intvalue = TRUE;
		AsnOptionNew ( & optionHead, OP_TOGENBNK, OP_TOGENBNK_CDROM, 
			quiet_val, (AsnOptFreeFunc) NULL);
	}
#endif
	if (myargs[releasearg].intvalue != 0) 	/* For CdRom generation ?? */
	{
		quiet_val.intvalue = TRUE;
		AsnOptionNew ( & optionHead, OP_TOGENBNK, OP_TOGENBNK_RELEASE, 
			quiet_val, (AsnOptFreeFunc) NULL);
	}

/* --- special output format ---*/
	if (myargs[output_format_arg].intvalue != 0) {
		format_val.intvalue = myargs[output_format_arg].intvalue;
		AsnOptionNew ( & optionHead, OP_TOGENBNK, OP_TOGENBNK_FORMAT, 
			format_val, (AsnOptFreeFunc) NULL);
	}
	/*- so, do I do nucleotide or protean, or both???*/
	if (myargs[nucl_protarg].intvalue != 0) {
		nuc_prot_val.intvalue = myargs[nucl_protarg].intvalue;
		AsnOptionNew ( & optionHead, OP_TOGENBNK, OP_TOGENBNK_NUCL_PROT, 
			nuc_prot_val, (AsnOptFreeFunc) NULL);
	}
	if (myargs[headerarg].intvalue != 0  ){ /* add header at beginning? */
			header_info . loci = 0L;
			header_info . bases = 0L;
			header_val . ptrvalue = & header_info;
			AsnOptionNew ( & optionHead, OP_TOGENBNK, OP_TOGENBNK_HEADER, 
				header_val, (AsnOptFreeFunc) NULL);
			
			if (myargs[outfilenamearg].strvalue){
				if ( StringLen (myargs[outfilenamearg].strvalue) > 9 ||
						! StringCmp(myargs[outfilenamearg].strvalue,"stdout")){
				ErrPost(CTX_NCBI2GB,CTX_2GB_ARG_TROUBLE,
					"Illegal output filename <%s> for header generation",
					myargs[outfilenamearg].strvalue);
				has_trouble = TRUE;
				}else{
					for ( i = 0; 
						(myargs[outfilenamearg].strvalue)[i] != '\0' && i < 9; i++ ){
						header_upper_name[i] = 
							TOUPPER( (myargs[outfilenamearg].strvalue)[i]);
					}
					header_upper_name[i] = '\0';
				}
			}else{
				ErrPost(CTX_NCBI2GB,CTX_2GB_ARG_TROUBLE,
				"Output filename (-f) required for header generation");
				has_trouble = TRUE;
			}

		if (
					myargs[ headerdbnamearg] .strvalue &&
					myargs[ headerdatearg] .strvalue &&
					myargs[ headerrelnamearg] .strvalue &&
					myargs[ headerrelnumarg] .strvalue &&
					myargs[ headernamearg] .strvalue
			){
				if (FpOut){
					fprintf(FpOut,
"%9s          %s\n\
                         %s\n\n\
%39s %5s\n\n%40s\n\n",
				header_upper_name,
				myargs[ headerdbnamearg] .strvalue,
				myargs[ headerdatearg] .strvalue,
				myargs[ headerrelnamearg] .strvalue,
				myargs[ headerrelnumarg] .strvalue,
				myargs[ headernamearg] .strvalue);
				byte_start = ftell(FpOut);
				fprintf(FpOut,fmt,(long) 0, (long) 0, (long) 0);
				fflush(FpOut);
				}
		}else{
				ErrPost(CTX_NCBI2GB,CTX_2GB_ARG_TROUBLE,
				"Parameters v through z required for header generation");
				has_trouble = TRUE;
		}
		}
/*------deal with index file generation ---*/
	if(
			myargs[allindexarg].intvalue != 0 ||
			myargs[suppressarg].intvalue != 0 
#ifdef OLD_ARGS_FOR_HISTORY
||
			myargs[accessionarg].intvalue != 0 ||
			myargs[journalarg].intvalue != 0 ||
			myargs[keywordarg].intvalue != 0 ||
			myargs[genearg].intvalue != 0  ||
			myargs[authorarg].intvalue != 0 
#endif
		){
		index_string [0] = '\0';
				
		if ( myargs[allindexarg].intvalue != 0 ){
			StringCat(index_string,"ACKJG");
		}
#ifdef OLD_ARGS_FOR_HISTORY
else {
			if ( myargs[accessionarg].intvalue != 0 ){
				StringCat(index_string,"C");
			}
			if ( myargs[journalarg].intvalue != 0 ){
				StringCat(index_string,"J");
			}
			if ( myargs[keywordarg].intvalue != 0 ){
				StringCat(index_string,"K");
			}
			if ( myargs[authorarg].intvalue != 0 ){
				StringCat(index_string,"A");
			}
			if ( myargs[genearg].intvalue != 0 ){
				StringCat(index_string,"G");
			}
		}
#endif
		if (myargs[suppressarg].intvalue != 0 ){
				StringCat(index_string,":");
		}
		index_val.ptrvalue = StringSave(index_string);
		AsnOptionNew ( & optionHead, OP_TOGENBNK, OP_TOGENBNK_INDEX, 
			index_val, (AsnOptFreeFunc) MemFree);
	}

	if (has_trouble){
		return 1;
	}

	num_errors=0;
	do {
	if ( myargs[isseqentryarg].intvalue)   /* read one Seq-entry */
	{
		the_set = SeqEntryAsnRead(aip, NULL);
		if ( the_set){
			num_errors=0;
			SeqEntryToGenbank( FpOut, the_set, optionHead);
			SeqEntryFree(the_set);
		}else{
			num_errors ++;
		}
	}else if ( myargs[isseqsubmitarg].intvalue){
   /* read Seq-entries from Seq-submit */
		atp = SEQ_SUBMIT;
		while ((atp = AsnReadId(aip, amp, atp)) != NULL) {
			if (atp == SEQ_SUBMIT_data_entrys_E)    {
/* top level Seq-entry */
				the_set = SeqEntryAsnRead(aip, atp);
				if (the_set){
					num_errors=0;
					if (splicing_desc != NULL){
						FlatSpliceOn(the_set,splicing_desc);
					}
					SeqEntryToGenbank( FpOut, the_set, optionHead);
					if (splicing_desc != NULL){
						FlatSpliceOff(the_set,splicing_desc);
					}
					SeqEntryFree(the_set);
				}else{
					num_errors ++;
					break;
				}
			} else if ( atp == SEQ_SUBMIT_cit) {
/*--- get Seq-submit pub to splice on to each SeqEntry ---*/
				the_cit = CitSubAsnRead(aip,atp);
				splicing_desc = ValNodeNew(NULL);

				splicing_desc -> choice = Seq_descr_pub;
				pdp = PubdescNew();
				splicing_desc -> data.ptrvalue = pdp;
				pdp -> pub = & citsub;
				citsub.choice = PUB_Sub;
				citsub.data.ptrvalue = the_cit;
				citsub.next = NULL;
			} else{
				if ( ! AsnReadVal(aip, atp, NULL)){
					num_errors ++;
					break;
				}

			}
		}
	} else {
       /* read Seq-entry's from a Bioseq-set */
		while ((atp = AsnReadId(aip, amp, atp)) != NULL)
		{
			if (atp == atp2)    /* top level Seq-entry */
			{
				the_set = SeqEntryAsnRead(aip, atp);
				if (the_set){
					num_errors=0;
					SeqEntryToGenbank( FpOut, the_set, optionHead);
					SeqEntryFree(the_set);
				}else{
					num_errors ++;
					break;
				}
			}
			else{
				if ( ! AsnReadVal(aip, atp, NULL)){
					num_errors ++;
					break;
				}

			}
		}
	}
} while (try_again(num_errors, (int) myargs[streamarg].intvalue));

	AsnIoClose(aip);
	if (myargs[headerarg].intvalue != 0  ){ /* add header at beginning? */
			fseek(FpOut, byte_start, 0);
			fprintf(FpOut,fmt,
				(long) header_info . loci ,(long)  header_info . bases ,
				(long) header_info . loci );
	}
	FileClose(FpOut);

		AsnOptionFree( & optionHead, 0, 0);
	return(0);
}

/*---------- FlatSpliceOff () -----------*/
/*******************************************
 ****                                   ****
 **** Using the chain that was spliced  ****
 **** on, we can reconize the splice    ****
 **** and break it.                     ****
 ****                                   ****
 *******************************************/

int FlatSpliceOff (SeqEntryPtr the_set, ValNodePtr desc)
{
		BioseqSetPtr bss;
		BioseqPtr bs;
		ValNodePtr PNTR desc_head=NULL;
		ValNodePtr PNTR desc_target=NULL;
		ValNodePtr scan;

		if (IS_Bioseq(the_set) ){
			bs = (BioseqPtr) the_set -> data.ptrvalue;
			desc_head = & (bs -> descr);
		}else{
			bss = (BioseqSetPtr) the_set -> data.ptrvalue;
			desc_head = & (bss -> descr);
		}
		if (* desc_head){
			desc_target = desc_head;
			for (scan = * desc_head; scan; scan = scan -> next){
				if (scan == desc){
					* desc_target = NULL;
					break;
				}
				desc_target = & (scan -> next);
			}
		}

}
/*---------- FlatSpliceOn () -----------*/

void
FlatSpliceOn (SeqEntryPtr the_set, ValNodePtr desc)
{
		BioseqSetPtr bss;
		BioseqPtr bs;
		ValNodePtr PNTR desc_head=NULL;
		ValNodePtr scan, desc_tail=NULL;

		if (IS_Bioseq(the_set) ){
			bs = (BioseqPtr) the_set -> data.ptrvalue;
			desc_head = & (bs -> descr);
		}else{
			bss = (BioseqSetPtr) the_set -> data.ptrvalue;
			desc_head = & (bss -> descr);
		}
		if (* desc_head){
			for ( scan = * desc_head; scan;
					scan = scan -> next){
				desc_tail = scan;
			}
			desc_tail -> next = desc;
		}else{
			* desc_head = desc;;
		}

}

/*****************************************************************************
*
*   Nlm_WhichArg(ap)
*     returns array position for a tag
*
*****************************************************************************/
Nlm_Int2 Nlm_WhichArg(Nlm_Char which, Nlm_Int2 numargs, Nlm_ArgPtr ap)
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

int
try_again(int num_errors, int expect_repeating_stream)
{
	int retval = 0;

	if (expect_repeating_stream && num_errors < 10)
		retval = 1;

	return retval;
}
         
