/*****************************************************************************
*
*   asn2rpt.c
*   	convert a Seq-entry or elements of a Bioseq-set to reports
*
*****************************************************************************/
#include <toreport.h>

#define NUMARG 7
Args myargs[NUMARG] = {
	{"Filename for asn.1 input","stdin",NULL,NULL,TRUE,'a',ARG_FILE_IN,0.0,0,NULL},
	{"Input is a Seq-entry","F", NULL ,NULL ,TRUE,'e',ARG_BOOLEAN,0.0,0,NULL},
	{"Input asnfile in binary mode","F",NULL,NULL,TRUE,'b',ARG_BOOLEAN,0.0,0,NULL},
	{"Output Filename","stdout", NULL,NULL,TRUE,'f',ARG_FILE_OUT,0.0,0,NULL},
	{"Show Sequence?","F", NULL ,NULL ,TRUE,'s',ARG_BOOLEAN,0.0,0,NULL},
    {"Log errors to file named:",NULL,NULL,NULL,TRUE,'l',ARG_FILE_OUT, 0.0,0,NULL},
	{"Generate checking form?","F",NULL,NULL,TRUE,'c',ARG_BOOLEAN,0.0,0,NULL}};

Int2 Main()
{
	AsnIoPtr aip;
	SeqEntryPtr the_set;
	AsnTypePtr atp, atp2;
	AsnModulePtr amp;
	FILE * FpOut;
	Boolean show_seq = FALSE,
		checkform = FALSE;

	char divider [92];
	int count;

					/* check command line arguments */

	if ( ! GetArgs("asn2rpt",NUMARG, myargs))
		return 1;

					/* load the sequence alphabets  */
					/* (and sequence parse trees)   */
	if (! SeqEntryLoad())
		ErrShow();
				    /* get pointer to all loaded ASN.1 modules */
	amp = AsnAllModPtr();
	if (amp == NULL)
		ErrShow();

	atp = AsnFind("Bioseq-set");    /* get the initial type pointers */
	if (atp == NULL)
		ErrShow();
	atp2 = AsnFind("Bioseq-set.seq-set.E");
	if (atp2 == NULL)
		ErrShow();

					/* open the ASN.1 input file in the right mode */

	if ((aip = AsnIoOpen (myargs[0].strvalue, myargs[2].intvalue?"rb":"r"))
          == NULL)
		ErrShow();

				  				/* open the output file */

	if ( (FpOut = FileOpen (myargs[3].strvalue, "w")) == NULL)
		ErrShow();

                                /* log errors instead of die */
    if (myargs[5].strvalue != NULL)
    {
        if (! ErrSetLog (myargs[5].strvalue))
            ErrShow();
        else
            ErrSetOpts (ERR_CONTINUE, ERR_LOG_ON);
    }

	if ( myargs[4].intvalue)   /* show the sequence */
		show_seq = TRUE;
	if (myargs[6].intvalue)    /* generate checking form */
		checkform = TRUE;

	MemFill(divider, (int)'=', 76);
	divider[76] = '\0';
	
	if ( myargs[1].intvalue)   /* read one Seq-entry */
	{
		if (checkform)
		{
			/* print divider + space for comments */
			fprintf (FpOut, "%s\nComments\n", divider);
			fprintf (FpOut, "\n\n\n\n\n\n\n\n\n\n\n\n");
			fprintf (FpOut, "%s\n\n", divider);
		}

		the_set = SeqEntryAsnRead(aip, NULL);
		SeqEntryToFile( the_set, FpOut, show_seq, 50, checkform);
		SeqEntryFree(the_set);
	}
	else                      /* read Seq-entry's from a Bioseq-set */
	{
		count = 0;
		while ((atp = AsnReadId(aip, amp, atp)) != NULL)
		{
			if (atp == atp2)    /* top level Seq-entry */
			{
				/* print divider + space for comments */
				if (count) fprintf (FpOut, "\f");
				if (checkform)
				{
					fprintf (FpOut, "%s\nComments\n", divider);
					fprintf (FpOut, "\n\n\n\n\n\n\n\n\n\n\n\n");
					fprintf (FpOut, "%s\n\n", divider);
				}

				the_set = SeqEntryAsnRead(aip, atp);
				SeqEntryToFile( the_set, FpOut, show_seq, 50, checkform);
				SeqEntryFree(the_set);
			}
			else
				AsnReadVal(aip, atp, NULL);
			count++;
		}
	}

	AsnIoClose(aip);
	FileClose(FpOut);
	return(0);
}

