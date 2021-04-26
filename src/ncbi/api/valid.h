
/*****************************************************************************
*
*   valid.h
*   	values for cutoff are
*       0 INFO
*       1 WARN
*       2 ERROR
*       3 FATAL
*
*****************************************************************************/
#ifndef _NCBI_Seqport_
#include <seqport.h>
#endif


typedef void (*SpellCallBackFunc) (char * str);
typedef int (* SpellCheckFunc) (char *String, SpellCallBackFunc);


#define SET_DEPTH 20

typedef struct validstruct {
	Int2 cutoff;                   /* lowest errmsg to show 0=default */
	Int2 errors[4];
	Int2 currdepth;
	BioseqSetPtr bsps [SET_DEPTH];
	Uint4 descrs [SET_DEPTH];      /* bit flags set by descriptor type */
	Int2 protcnt [SET_DEPTH],	   /* count of bioseqs by type within set */
		 nuccnt [SET_DEPTH],
		 segcnt [SET_DEPTH];
	Int2 otherannot;               /* if other seqannot types found */
	CharPtr errbuf;
	FILE * errfile;
	SeqEntryPtr sep;               /* top level SeqEntry */
	BioseqPtr bsp;                 /* current Bioseq */
	SeqFeatPtr sfp;                /* current feature */
	Boolean patch_seq;             /* repair invalid sequence residues? */
	Boolean non_ascii_chars;       /* non ascii chars found in read? */
	SpellCheckFunc spellfunc;
	SpellCallBackFunc spellcallback;
} ValidStruct, PNTR ValidStructPtr;

Boolean ValidateSeqEntry PROTO((SeqEntryPtr sep, ValidStructPtr vsp));
void ValidStructClear (ValidStructPtr vsp);  /* 0 out a ValidStruct */
ValidStructPtr ValidStructNew (void);
ValidStructPtr ValidStructFree (ValidStructPtr vsp);
void ReportNonAscii PROTO((ValidStructPtr vsp, SeqEntryPtr sep));
void SpellCallBack (char * str);
