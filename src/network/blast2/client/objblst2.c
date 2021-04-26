#include <asn.h>

#define NLM_GENERATED_CODE_PROTO

#include "blast18p.h"
#include "objblst2.h"

static Boolean loaded = FALSE;

#include "asnbl18.h"

#ifndef NLM_EXTERN_LOADS
#define NLM_EXTERN_LOADS {}
#endif

Boolean LIBCALL
objblst2AsnLoad(void)
{

   if ( ! loaded) {
      NLM_EXTERN_LOADS

      if ( ! AsnLoad ())
      return FALSE;
      loaded = TRUE;
   }

   return TRUE;
}



/**************************************************
*    Generated object loaders for Module NCBI-BLAST-1
*    Generated using ASNCODE Revision: 1.17 at Jun 28, 1995  2:37 PM
*
**************************************************/


/**************************************************
*
*    BLAST0PrefaceNew()
*
**************************************************/

BLAST0PrefacePtr LIBCALL
BLAST0PrefaceNew(void)
{
   BLAST0PrefacePtr ptr = MemNew((size_t) sizeof(BLAST0Preface));

   return ptr;

}


/**************************************************
*
*    BLAST0PrefaceFree()
*
**************************************************/

BLAST0PrefacePtr LIBCALL
BLAST0PrefaceFree(BLAST0PrefacePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> program);
   MemFree(ptr -> desc);
   MemFree(ptr -> version);
   MemFree(ptr -> dev_date);
   MemFree(ptr -> bld_date);
   AsnGenericBaseSeqOfFree(ptr -> cit ,ASNCODE_PTRVAL_SLOT);
   AsnGenericBaseSeqOfFree(ptr -> notice ,ASNCODE_PTRVAL_SLOT);
   AsnGenericBaseSeqOfFree(ptr -> prog_usage ,ASNCODE_PTRVAL_SLOT);
   BLAST0SeqUsageFree(ptr -> susage);
   BLAST0SeqUsageFree(ptr -> qusage);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0PrefaceAsnRead()
*
**************************************************/

BLAST0PrefacePtr LIBCALL
BLAST0PrefaceAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0PrefacePtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Preface ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_PREFACE);
   } else {
      atp = AsnLinkType(orig, BLAST0_PREFACE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0PrefaceNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_PREFACE_program) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> program = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_PREFACE_desc) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> desc = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_PREFACE_version) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> version = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_PREFACE_dev_date) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> dev_date = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_PREFACE_bld_date) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> bld_date = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_PREFACE_cit) {
      ptr -> cit = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_PTRVAL_SLOT, &isError);
      if (isError && ptr -> cit == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_PREFACE_notice) {
      ptr -> notice = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_PTRVAL_SLOT, &isError);
      if (isError && ptr -> notice == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_PREFACE_prog_usage) {
      ptr -> prog_usage = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_PTRVAL_SLOT, &isError);
      if (isError && ptr -> prog_usage == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_PREFACE_susage) {
      ptr -> susage = BLAST0SeqUsageAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_PREFACE_qusage) {
      ptr -> qusage = BLAST0SeqUsageAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0PrefaceFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0PrefaceAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0PrefaceAsnWrite(BLAST0PrefacePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_PREFACE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> program != NULL) {
      av.ptrvalue = ptr -> program;
      retval = AsnWrite(aip, BLAST0_PREFACE_program,  &av);
   }
   if (ptr -> desc != NULL) {
      av.ptrvalue = ptr -> desc;
      retval = AsnWrite(aip, BLAST0_PREFACE_desc,  &av);
   }
   if (ptr -> version != NULL) {
      av.ptrvalue = ptr -> version;
      retval = AsnWrite(aip, BLAST0_PREFACE_version,  &av);
   }
   if (ptr -> dev_date != NULL) {
      av.ptrvalue = ptr -> dev_date;
      retval = AsnWrite(aip, BLAST0_PREFACE_dev_date,  &av);
   }
   if (ptr -> bld_date != NULL) {
      av.ptrvalue = ptr -> bld_date;
      retval = AsnWrite(aip, BLAST0_PREFACE_bld_date,  &av);
   }
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> cit ,ASNCODE_PTRVAL_SLOT, aip, BLAST0_PREFACE_cit, BLAST0_PREFACE_cit_E);
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> notice ,ASNCODE_PTRVAL_SLOT, aip, BLAST0_PREFACE_notice, BLAST0_PREFACE_notice_E);
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> prog_usage ,ASNCODE_PTRVAL_SLOT, aip, BLAST0_PREFACE_prog_usage, BLAST0_PREFACE_prog_usage_E);
   if (ptr -> susage != NULL) {
      if ( ! BLAST0SeqUsageAsnWrite(ptr -> susage, aip, BLAST0_PREFACE_susage)) {
         goto erret;
      }
   }
   if (ptr -> qusage != NULL) {
      if ( ! BLAST0SeqUsageAsnWrite(ptr -> qusage, aip, BLAST0_PREFACE_qusage)) {
         goto erret;
      }
   }
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0JobDescNew()
*
**************************************************/

BLAST0JobDescPtr LIBCALL
BLAST0JobDescNew(void)
{
   BLAST0JobDescPtr ptr = MemNew((size_t) sizeof(BLAST0JobDesc));

   return ptr;

}


/**************************************************
*
*    BLAST0JobDescFree()
*
**************************************************/

BLAST0JobDescPtr LIBCALL
BLAST0JobDescFree(BLAST0JobDescPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> desc);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0JobDescAsnRead()
*
**************************************************/

BLAST0JobDescPtr LIBCALL
BLAST0JobDescAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0JobDescPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0JobDesc ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_JOB_DESC);
   } else {
      atp = AsnLinkType(orig, BLAST0_JOB_DESC);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0JobDescNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_JOB_DESC_jid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> jid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_JOB_DESC_desc) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> desc = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_JOB_DESC_size) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> size = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0JobDescFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0JobDescAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0JobDescAsnWrite(BLAST0JobDescPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_JOB_DESC);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> jid;
   retval = AsnWrite(aip, BLAST0_JOB_DESC_jid,  &av);
   if (ptr -> desc != NULL) {
      av.ptrvalue = ptr -> desc;
      retval = AsnWrite(aip, BLAST0_JOB_DESC_desc,  &av);
   }
   av.intvalue = ptr -> size;
   retval = AsnWrite(aip, BLAST0_JOB_DESC_size,  &av);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0JobProgressNew()
*
**************************************************/

BLAST0JobProgressPtr LIBCALL
BLAST0JobProgressNew(void)
{
   BLAST0JobProgressPtr ptr = MemNew((size_t) sizeof(BLAST0JobProgress));

   return ptr;

}


/**************************************************
*
*    BLAST0JobProgressFree()
*
**************************************************/

BLAST0JobProgressPtr LIBCALL
BLAST0JobProgressFree(BLAST0JobProgressPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0JobProgressAsnRead()
*
**************************************************/

BLAST0JobProgressPtr LIBCALL
BLAST0JobProgressAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0JobProgressPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0JobProgress ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_JOB_PROGRESS);
   } else {
      atp = AsnLinkType(orig, BLAST0_JOB_PROGRESS);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0JobProgressNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_JOB_PROGRESS_done) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> done = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_JOB_PROGRESS_positives) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> positives = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0JobProgressFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0JobProgressAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0JobProgressAsnWrite(BLAST0JobProgressPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_JOB_PROGRESS);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> done;
   retval = AsnWrite(aip, BLAST0_JOB_PROGRESS_done,  &av);
   av.intvalue = ptr -> positives;
   retval = AsnWrite(aip, BLAST0_JOB_PROGRESS_positives,  &av);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0SequenceNew()
*
**************************************************/

BLAST0SequencePtr LIBCALL
BLAST0SequenceNew(void)
{
   BLAST0SequencePtr ptr = MemNew((size_t) sizeof(BLAST0Sequence));

   return ptr;

}


/**************************************************
*
*    BLAST0SequenceFree()
*
**************************************************/

BLAST0SequencePtr LIBCALL
BLAST0SequenceFree(BLAST0SequencePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> desc, (AsnOptFreeFunc) BLAST0SeqDescFree);
   BLAST0SeqDataFree(ptr -> seq);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0SequenceAsnRead()
*
**************************************************/

BLAST0SequencePtr LIBCALL
BLAST0SequenceAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0SequencePtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Sequence ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_SEQUENCE);
   } else {
      atp = AsnLinkType(orig, BLAST0_SEQUENCE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0SequenceNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_SEQUENCE_desc) {
      ptr -> desc = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0SeqDescAsnRead, (AsnOptFreeFunc) BLAST0SeqDescFree);
      if (isError && ptr -> desc == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEQUENCE_length) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> length = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEQUENCE_gcode) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> gcode = av.intvalue;
      ptr -> OBbits__ |= 1<<0;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEQUENCE_seq) {
      ptr -> seq = BLAST0SeqDataAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0SequenceFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0SequenceAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0SequenceAsnWrite(BLAST0SequencePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_SEQUENCE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   AsnGenericUserSeqOfAsnWrite(ptr -> desc, (AsnWriteFunc) BLAST0SeqDescAsnWrite, aip, BLAST0_SEQUENCE_desc, BLAST0_SEQUENCE_desc_E);
   av.intvalue = ptr -> length;
   retval = AsnWrite(aip, BLAST0_SEQUENCE_length,  &av);
   if (ptr -> gcode || (ptr -> OBbits__ & (1<<0) )){   av.intvalue = ptr -> gcode;
      retval = AsnWrite(aip, BLAST0_SEQUENCE_gcode,  &av);
   }
   if (ptr -> seq != NULL) {
      if ( ! BLAST0SeqDataAsnWrite(ptr -> seq, aip, BLAST0_SEQUENCE_seq)) {
         goto erret;
      }
   }
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0KABlkNew()
*
**************************************************/

BLAST0KABlkPtr LIBCALL
BLAST0KABlkNew(void)
{
   BLAST0KABlkPtr ptr = MemNew((size_t) sizeof(BLAST0KABlk));

   return ptr;

}


/**************************************************
*
*    BLAST0KABlkFree()
*
**************************************************/

BLAST0KABlkPtr LIBCALL
BLAST0KABlkFree(BLAST0KABlkPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericBaseSeqOfFree(ptr -> frames ,ASNCODE_INTVAL_SLOT);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0KABlkAsnRead()
*
**************************************************/

BLAST0KABlkPtr LIBCALL
BLAST0KABlkAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0KABlkPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0KABlk ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_KA_BLK);
   } else {
      atp = AsnLinkType(orig, BLAST0_KA_BLK);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0KABlkNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_KA_BLK_matid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> matid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_KA_BLK_frames) {
      ptr -> frames = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_INTVAL_SLOT, &isError);
      if (isError && ptr -> frames == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_KA_BLK_lambda) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> lambda = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_KA_BLK_k) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> k = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_KA_BLK_h) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> h = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0KABlkFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0KABlkAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0KABlkAsnWrite(BLAST0KABlkPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_KA_BLK);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> matid;
   retval = AsnWrite(aip, BLAST0_KA_BLK_matid,  &av);
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> frames ,ASNCODE_INTVAL_SLOT, aip, BLAST0_KA_BLK_frames, BLAST0_KA_BLK_frames_E);
   av.realvalue = ptr -> lambda;
   retval = AsnWrite(aip, BLAST0_KA_BLK_lambda,  &av);
   av.realvalue = ptr -> k;
   retval = AsnWrite(aip, BLAST0_KA_BLK_k,  &av);
   av.realvalue = ptr -> h;
   retval = AsnWrite(aip, BLAST0_KA_BLK_h,  &av);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0DbDescNew()
*
**************************************************/

BLAST0DbDescPtr LIBCALL
BLAST0DbDescNew(void)
{
   BLAST0DbDescPtr ptr = MemNew((size_t) sizeof(BLAST0DbDesc));

   return ptr;

}


/**************************************************
*
*    BLAST0DbDescFree()
*
**************************************************/

BLAST0DbDescPtr LIBCALL
BLAST0DbDescFree(BLAST0DbDescPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> name);
   MemFree(ptr -> def);
   MemFree(ptr -> rel_date);
   MemFree(ptr -> bld_date);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0DbDescAsnRead()
*
**************************************************/

BLAST0DbDescPtr LIBCALL
BLAST0DbDescAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0DbDescPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0DbDesc ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_DB_DESC);
   } else {
      atp = AsnLinkType(orig, BLAST0_DB_DESC);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0DbDescNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_DB_DESC_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_DB_DESC_type) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> type = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_DB_DESC_def) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> def = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_DB_DESC_rel_date) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> rel_date = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_DB_DESC_bld_date) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> bld_date = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_DB_DESC_count) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> count = av.intvalue;
      ptr -> OBbits__ |= 1<<0;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_DB_DESC_totlen) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> totlen = av.intvalue;
      ptr -> OBbits__ |= 1<<1;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_DB_DESC_maxlen) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> maxlen = av.intvalue;
      ptr -> OBbits__ |= 1<<2;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0DbDescFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0DbDescAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0DbDescAsnWrite(BLAST0DbDescPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_DB_DESC);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> name != NULL) {
      av.ptrvalue = ptr -> name;
      retval = AsnWrite(aip, BLAST0_DB_DESC_name,  &av);
   }
   av.intvalue = ptr -> type;
   retval = AsnWrite(aip, BLAST0_DB_DESC_type,  &av);
   if (ptr -> def != NULL) {
      av.ptrvalue = ptr -> def;
      retval = AsnWrite(aip, BLAST0_DB_DESC_def,  &av);
   }
   if (ptr -> rel_date != NULL) {
      av.ptrvalue = ptr -> rel_date;
      retval = AsnWrite(aip, BLAST0_DB_DESC_rel_date,  &av);
   }
   if (ptr -> bld_date != NULL) {
      av.ptrvalue = ptr -> bld_date;
      retval = AsnWrite(aip, BLAST0_DB_DESC_bld_date,  &av);
   }
   if (ptr -> count || (ptr -> OBbits__ & (1<<0) )){   av.intvalue = ptr -> count;
      retval = AsnWrite(aip, BLAST0_DB_DESC_count,  &av);
   }
   if (ptr -> totlen || (ptr -> OBbits__ & (1<<1) )){   av.intvalue = ptr -> totlen;
      retval = AsnWrite(aip, BLAST0_DB_DESC_totlen,  &av);
   }
   if (ptr -> maxlen || (ptr -> OBbits__ & (1<<2) )){   av.intvalue = ptr -> maxlen;
      retval = AsnWrite(aip, BLAST0_DB_DESC_maxlen,  &av);
   }
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0ResultNew()
*
**************************************************/

BLAST0ResultPtr LIBCALL
BLAST0ResultNew(void)
{
   BLAST0ResultPtr ptr = MemNew((size_t) sizeof(BLAST0Result));

   return ptr;

}


/**************************************************
*
*    BLAST0ResultFree()
*
**************************************************/

BLAST0ResultPtr LIBCALL
BLAST0ResultFree(BLAST0ResultPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   BLAST0HistogramFree(ptr -> hist);
   AsnGenericUserSeqOfFree(ptr -> hitlists, (AsnOptFreeFunc) BLAST0HitListFree);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0ResultAsnRead()
*
**************************************************/

BLAST0ResultPtr LIBCALL
BLAST0ResultAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0ResultPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Result ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_RESULT);
   } else {
      atp = AsnLinkType(orig, BLAST0_RESULT);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0ResultNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_RESULT_hist) {
      ptr -> hist = BLAST0HistogramAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_RESULT_count) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> count = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_RESULT_hitlists) {
      ptr -> hitlists = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0HitListAsnRead, (AsnOptFreeFunc) BLAST0HitListFree);
      if (isError && ptr -> hitlists == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0ResultFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0ResultAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0ResultAsnWrite(BLAST0ResultPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_RESULT);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> hist != NULL) {
      if ( ! BLAST0HistogramAsnWrite(ptr -> hist, aip, BLAST0_RESULT_hist)) {
         goto erret;
      }
   }
   av.intvalue = ptr -> count;
   retval = AsnWrite(aip, BLAST0_RESULT_count,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> hitlists, (AsnWriteFunc) BLAST0HitListAsnWrite, aip, BLAST0_RESULT_hitlists, BLAST0_RESULT_hitlists_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0MatrixNew()
*
**************************************************/

BLAST0MatrixPtr LIBCALL
BLAST0MatrixNew(void)
{
   BLAST0MatrixPtr ptr = MemNew((size_t) sizeof(BLAST0Matrix));

   return ptr;

}


/**************************************************
*
*    Scores_scaled_intsNew()
*
**************************************************/
static 
Scores_scaled_intsPtr LIBCALL
Scores_scaled_intsNew(void)
{
   Scores_scaled_intsPtr ptr = MemNew((size_t) sizeof(Scores_scaled_ints));

   return ptr;

}


/**************************************************
*
*    BLAST0MatrixFree()
*
**************************************************/

BLAST0MatrixPtr LIBCALL
BLAST0MatrixFree(BLAST0MatrixPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> name);
   AsnGenericBaseSeqOfFree(ptr -> comments ,ASNCODE_PTRVAL_SLOT);
   Scores_scoresFree(ptr -> Scores_scores);
   return MemFree(ptr);
}


/**************************************************
*
*    Scores_scoresFree()
*
**************************************************/
static 
Scores_scoresPtr LIBCALL
Scores_scoresFree(ValNodePtr anp)
{
   Pointer pnt;

   if (anp == NULL) {
      return NULL;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   default:
      break;
   case Scores_scores_Scores_ScaledInts:
      Scores_scaled_intsFree(anp -> data.ptrvalue);
      break;
   case Scores_scores_reals:
      AsnGenericBaseSeqOfFree((ValNodePtr) pnt,ASNCODE_REALVAL_SLOT);
      break;
   }
   return MemFree(anp);
}


/**************************************************
*
*    Scores_scaled_intsFree()
*
**************************************************/
static 
Scores_scaled_intsPtr LIBCALL
Scores_scaled_intsFree(Scores_scaled_intsPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericBaseSeqOfFree(ptr -> ints ,ASNCODE_INTVAL_SLOT);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0MatrixAsnRead()
*
**************************************************/

BLAST0MatrixPtr LIBCALL
BLAST0MatrixAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0MatrixPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Matrix ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_MATRIX);
   } else {
      atp = AsnLinkType(orig, BLAST0_MATRIX);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0MatrixNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_MATRIX_matid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> matid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_MATRIX_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_MATRIX_comments) {
      ptr -> comments = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_PTRVAL_SLOT, &isError);
      if (isError && ptr -> comments == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_MATRIX_qalpha) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> qalpha = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_MATRIX_salpha) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> salpha = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_MATRIX_scores) {
      ptr -> Scores_scores = Scores_scoresAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0MatrixFree(ptr);
   goto ret;
}



/**************************************************
*
*    Scores_scoresAsnRead()
*
**************************************************/
static 
Scores_scoresPtr LIBCALL
Scores_scoresAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   ValNodePtr anp;
   Uint1 choice;
   Boolean isError = FALSE;
   Boolean nullIsError = FALSE;
   AsnReadFunc func;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* Scores_scores ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_MATRIX_scores);
   } else {
      atp = AsnLinkType(orig, BLAST0_MATRIX_scores);    /* link in local tree */
   }
   if (atp == NULL) {
      return NULL;
   }

   anp = ValNodeNew(NULL);
   if (anp == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the CHOICE or OpenStruct value (nothing) */
      goto erret;
   }

   func = NULL;

   atp = AsnReadId(aip, amp, atp);  /* find the choice */
   if (atp == NULL) {
      goto erret;
   }
   if (atp == MATRIX_scores_scaled_ints) {
      choice = Scores_scores_Scores_ScaledInts;
      func = (AsnReadFunc) Scores_scaled_intsAsnRead;
   }
   else if (atp == BLAST0_MATRIX_scores_reals) {
      choice = Scores_scores_reals;
      anp -> data.ptrvalue = 
      AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_REALVAL_SLOT, &isError);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   anp->choice = choice;
   if (func != NULL)
   {
      anp->data.ptrvalue = (* func)(aip, atp);
      if (aip -> io_failure) goto erret;

      if (nullIsError && anp->data.ptrvalue == NULL) {
         goto erret;
      }
   }

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return anp;

erret:
   anp = MemFree(anp);
   aip -> io_failure = TRUE;
   goto ret;
}


/**************************************************
*
*    Scores_scaled_intsAsnRead()
*
**************************************************/
static 
Scores_scaled_intsPtr LIBCALL
Scores_scaled_intsAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   Scores_scaled_intsPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* Scores_scaled_ints ::= (self contained) */
      atp = AsnReadId(aip, amp, MATRIX_scores_scaled_ints);
   } else {
      atp = AsnLinkType(orig, MATRIX_scores_scaled_ints);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = Scores_scaled_intsNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == scores_scaled_ints_scale) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> scale = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == MATRIX_scores_scaled_ints_ints) {
      ptr -> ints = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_INTVAL_SLOT, &isError);
      if (isError && ptr -> ints == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = Scores_scaled_intsFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0MatrixAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0MatrixAsnWrite(BLAST0MatrixPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_MATRIX);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> matid;
   retval = AsnWrite(aip, BLAST0_MATRIX_matid,  &av);
   if (ptr -> name != NULL) {
      av.ptrvalue = ptr -> name;
      retval = AsnWrite(aip, BLAST0_MATRIX_name,  &av);
   }
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> comments ,ASNCODE_PTRVAL_SLOT, aip, BLAST0_MATRIX_comments, BLAST0_MATRIX_comments_E);
   av.intvalue = ptr -> qalpha;
   retval = AsnWrite(aip, BLAST0_MATRIX_qalpha,  &av);
   av.intvalue = ptr -> salpha;
   retval = AsnWrite(aip, BLAST0_MATRIX_salpha,  &av);
   if (ptr -> Scores_scores != NULL) {
      if ( ! Scores_scoresAsnWrite(ptr -> Scores_scores, aip, BLAST0_MATRIX_scores)) {
         goto erret;
      }
   }
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    Scores_scoresAsnWrite()
*
**************************************************/
static Boolean LIBCALL 
Scores_scoresAsnWrite(Scores_scoresPtr anp, AsnIoPtr aip, AsnTypePtr orig)

{
   DataVal av;
   AsnTypePtr atp, writetype = NULL;
   Pointer pnt;
   AsnWriteFunc func = NULL;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad())
      return FALSE;
   }

   if (aip == NULL)
   return FALSE;

   atp = AsnLinkType(orig, BLAST0_MATRIX_scores);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

   av.ptrvalue = (Pointer)anp;
   if (! AsnWriteChoice(aip, atp, (Int2)anp->choice, &av)) {
      goto erret;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   case Scores_scores_Scores_ScaledInts:
      writetype = MATRIX_scores_scaled_ints;
      func = (AsnWriteFunc) Scores_scaled_intsAsnWrite;
      break;
   case Scores_scores_reals:
      retval = AsnGenericBaseSeqOfAsnWrite((Pointer) pnt,ASNCODE_REALVAL_SLOT, aip, BLAST0_MATRIX_scores_reals, BLAST0_MATRIX_scores_reals_E);            break;
   }
   if (writetype != NULL) {
      retval = (* func)(pnt, aip, writetype);   /* write it out */
   }
   if (!retval) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}


/**************************************************
*
*    Scores_scaled_intsAsnWrite()
*
**************************************************/
static Boolean LIBCALL 
Scores_scaled_intsAsnWrite(Scores_scaled_intsPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, MATRIX_scores_scaled_ints);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.realvalue = ptr -> scale;
   retval = AsnWrite(aip, scores_scaled_ints_scale,  &av);
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> ints ,ASNCODE_INTVAL_SLOT, aip, MATRIX_scores_scaled_ints_ints, scores_scaled_ints_ints_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0StatusNew()
*
**************************************************/

BLAST0StatusPtr LIBCALL
BLAST0StatusNew(void)
{
   BLAST0StatusPtr ptr = MemNew((size_t) sizeof(BLAST0Status));

   return ptr;

}


/**************************************************
*
*    BLAST0StatusFree()
*
**************************************************/

BLAST0StatusPtr LIBCALL
BLAST0StatusFree(BLAST0StatusPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> reason);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0StatusAsnRead()
*
**************************************************/

BLAST0StatusPtr LIBCALL
BLAST0StatusAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0StatusPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Status ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_STATUS);
   } else {
      atp = AsnLinkType(orig, BLAST0_STATUS);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0StatusNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_STATUS_code) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> code = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_STATUS_reason) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> reason = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0StatusFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0StatusAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0StatusAsnWrite(BLAST0StatusPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_STATUS);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> code;
   retval = AsnWrite(aip, BLAST0_STATUS_code,  &av);
   if (ptr -> reason != NULL) {
      av.ptrvalue = ptr -> reason;
      retval = AsnWrite(aip, BLAST0_STATUS_reason,  &av);
   }
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0OutblkFree()
*
**************************************************/

BLAST0OutblkPtr LIBCALL
BLAST0OutblkFree(ValNodePtr anp)
{

   if (anp == NULL) {
      return NULL;
   }

   AsnGenericChoiceSeqOfFree(anp, (AsnOptFreeFunc) BLAST0Outblk_elementFree);    
   return NULL;
}


/**************************************************
*
*    BLAST0OutblkAsnRead()
*
**************************************************/

BLAST0OutblkPtr LIBCALL
BLAST0OutblkAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{


   DataVal av;
   AsnTypePtr atp;
   ValNodePtr anp;
   Uint1 choice;
   Boolean isError = FALSE;
   AsnReadFunc func;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Outblk_element ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_OUTBLK);
   } else {
      atp = AsnLinkType(orig, BLAST0_OUTBLK);    /* link in local tree */
   }
   if (atp == NULL) {
      return NULL;
   }

   anp =
   AsnGenericChoiceSeqOfAsnRead(aip, amp, atp, &isError,
   (AsnReadFunc) BLAST0Outblk_elementAsnRead, (AsnOptFreeFunc) BLAST0Outblk_elementFree);
   if (isError) 
   goto erret;


ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return anp;

erret:
   aip -> io_failure = TRUE;
   goto ret;
}


/**************************************************
*
*    BLAST0OutblkAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0OutblkAsnWrite(ValNodePtr anp, AsnIoPtr aip, AsnTypePtr orig)

{
   DataVal av;
   AsnTypePtr atp, writetype = NULL;
   Pointer pnt;
   AsnWriteFunc func = NULL;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad())
      return FALSE;
   }

   if (aip == NULL)
   return FALSE;

   atp = AsnLinkType(orig, BLAST0_OUTBLK);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

   av.ptrvalue = (Pointer)anp;
   retval = AsnGenericChoiceSeqOfAsnWrite(anp, 
   (AsnWriteFunc) BLAST0Outblk_elementAsnWrite, aip, atp, BLAST0_OUTBLK_E);
erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}


/**************************************************
*
*    BLAST0Outblk_elementAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0Outblk_elementAsnWrite(BLAST0Outblk_elementPtr anp, AsnIoPtr aip, AsnTypePtr orig)

{
   DataVal av;
   AsnTypePtr atp, writetype = NULL;
   Pointer pnt;
   AsnWriteFunc func = NULL;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad())
      return FALSE;
   }

   if (aip == NULL)
   return FALSE;

   atp = AsnLinkType(orig, BLAST0_OUTBLK_E);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

   av.ptrvalue = (Pointer)anp;
   if (! AsnWriteChoice(aip, atp, (Int2)anp->choice, &av)) {
      goto erret;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   case BLAST0Outblk_preface:
      writetype = BLAST0_OUTBLK_E_preface;
      func = (AsnWriteFunc) BLAST0PrefaceAsnWrite;
      break;
   case BLAST0Outblk_query:
      writetype = BLAST0_OUTBLK_E_query;
      func = (AsnWriteFunc) BLAST0SequenceAsnWrite;
      break;
   case BLAST0Outblk_dbdesc:
      writetype = BLAST0_OUTBLK_E_dbdesc;
      func = (AsnWriteFunc) BLAST0DbDescAsnWrite;
      break;
   case BLAST0Outblk_matrix:
      retval = AsnGenericUserSeqOfAsnWrite((Pointer) pnt, (AsnWriteFunc) BLAST0MatrixAsnWrite, aip, BLAST0_OUTBLK_E_matrix, BLAST0_OUTBLK_E_matrix_E);
      break;
   case BLAST0Outblk_kablk:
      retval = AsnGenericUserSeqOfAsnWrite((Pointer) pnt, (AsnWriteFunc) BLAST0KABlkAsnWrite, aip, BLAST0_OUTBLK_E_kablk, BLAST0_OUTBLK_E_kablk_E);
      break;
   case BLAST0Outblk_job_start:
      writetype = BLAST0_OUTBLK_E_job_start;
      func = (AsnWriteFunc) BLAST0JobDescAsnWrite;
      break;
   case BLAST0Outblk_job_progress:
      writetype = BLAST0_OUTBLK_E_job_progress;
      func = (AsnWriteFunc) BLAST0JobProgressAsnWrite;
      break;
   case BLAST0Outblk_job_done:
      writetype = BLAST0_OUTBLK_E_job_done;
      func = (AsnWriteFunc) BLAST0JobProgressAsnWrite;
      break;
   case BLAST0Outblk_result:
      writetype = BLAST0_OUTBLK_E_result;
      func = (AsnWriteFunc) BLAST0ResultAsnWrite;
      break;
   case BLAST0Outblk_parms:
      retval = AsnGenericBaseSeqOfAsnWrite((Pointer) pnt,ASNCODE_PTRVAL_SLOT, aip, BLAST0_OUTBLK_E_parms, BLAST0_OUTBLK_E_parms_E);            break;
   case BLAST0Outblk_stats:
      retval = AsnGenericBaseSeqOfAsnWrite((Pointer) pnt,ASNCODE_PTRVAL_SLOT, aip, BLAST0_OUTBLK_E_stats, BLAST0_OUTBLK_E_stats_E);            break;
   case BLAST0Outblk_warning:
      writetype = BLAST0_OUTBLK_E_warning;
      func = (AsnWriteFunc) BLAST0StatusAsnWrite;
      break;
   case BLAST0Outblk_status:
      writetype = BLAST0_OUTBLK_E_status;
      func = (AsnWriteFunc) BLAST0StatusAsnWrite;
      break;
   }
   if (writetype != NULL) {
      retval = (* func)(pnt, aip, writetype);   /* write it out */
   }
   if (!retval) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}


/**************************************************
*
*    BLAST0Outblk_elementAsnRead()
*
**************************************************/

BLAST0Outblk_elementPtr LIBCALL
BLAST0Outblk_elementAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   ValNodePtr anp;
   Uint1 choice;
   Boolean isError = FALSE;
   Boolean nullIsError = FALSE;
   AsnReadFunc func;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Outblk_element ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_OUTBLK_E);
   } else {
      atp = AsnLinkType(orig, BLAST0_OUTBLK_E);    /* link in local tree */
   }
   if (atp == NULL) {
      return NULL;
   }

   anp = ValNodeNew(NULL);
   if (anp == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the CHOICE or OpenStruct value (nothing) */
      goto erret;
   }

   func = NULL;

   atp = AsnReadId(aip, amp, atp);  /* find the choice */
   if (atp == NULL) {
      goto erret;
   }
   if (atp == BLAST0_OUTBLK_E_preface) {
      choice = BLAST0Outblk_preface;
      func = (AsnReadFunc) BLAST0PrefaceAsnRead;
   }
   else if (atp == BLAST0_OUTBLK_E_query) {
      choice = BLAST0Outblk_query;
      func = (AsnReadFunc) BLAST0SequenceAsnRead;
   }
   else if (atp == BLAST0_OUTBLK_E_dbdesc) {
      choice = BLAST0Outblk_dbdesc;
      func = (AsnReadFunc) BLAST0DbDescAsnRead;
   }
   else if (atp == BLAST0_OUTBLK_E_matrix) {
      choice = BLAST0Outblk_matrix;
      anp -> data.ptrvalue =
      AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0MatrixAsnRead,             (AsnOptFreeFunc) BLAST0MatrixFree);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == BLAST0_OUTBLK_E_kablk) {
      choice = BLAST0Outblk_kablk;
      anp -> data.ptrvalue =
      AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0KABlkAsnRead,             (AsnOptFreeFunc) BLAST0KABlkFree);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == BLAST0_OUTBLK_E_job_start) {
      choice = BLAST0Outblk_job_start;
      func = (AsnReadFunc) BLAST0JobDescAsnRead;
   }
   else if (atp == BLAST0_OUTBLK_E_job_progress) {
      choice = BLAST0Outblk_job_progress;
      func = (AsnReadFunc) BLAST0JobProgressAsnRead;
   }
   else if (atp == BLAST0_OUTBLK_E_job_done) {
      choice = BLAST0Outblk_job_done;
      func = (AsnReadFunc) BLAST0JobProgressAsnRead;
   }
   else if (atp == BLAST0_OUTBLK_E_result) {
      choice = BLAST0Outblk_result;
      func = (AsnReadFunc) BLAST0ResultAsnRead;
   }
   else if (atp == BLAST0_OUTBLK_E_parms) {
      choice = BLAST0Outblk_parms;
      anp -> data.ptrvalue = 
      AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_PTRVAL_SLOT, &isError);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == BLAST0_OUTBLK_E_stats) {
      choice = BLAST0Outblk_stats;
      anp -> data.ptrvalue = 
      AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_PTRVAL_SLOT, &isError);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == BLAST0_OUTBLK_E_warning) {
      choice = BLAST0Outblk_warning;
      func = (AsnReadFunc) BLAST0StatusAsnRead;
   }
   else if (atp == BLAST0_OUTBLK_E_status) {
      choice = BLAST0Outblk_status;
      func = (AsnReadFunc) BLAST0StatusAsnRead;
   }
   anp->choice = choice;
   if (func != NULL)
   {
      anp->data.ptrvalue = (* func)(aip, atp);
      if (aip -> io_failure) goto erret;

      if (nullIsError && anp->data.ptrvalue == NULL) {
         goto erret;
      }
   }

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return anp;

erret:
   anp = MemFree(anp);
   aip -> io_failure = TRUE;
   goto ret;
}


/**************************************************
*
*    BLAST0Outblk_elementFree()
*
**************************************************/

BLAST0Outblk_elementPtr LIBCALL
BLAST0Outblk_elementFree(ValNodePtr anp)
{
   Pointer pnt;

   if (anp == NULL) {
      return NULL;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   default:
      break;
   case BLAST0Outblk_preface:
      BLAST0PrefaceFree(anp -> data.ptrvalue);
      break;
   case BLAST0Outblk_query:
      BLAST0SequenceFree(anp -> data.ptrvalue);
      break;
   case BLAST0Outblk_dbdesc:
      BLAST0DbDescFree(anp -> data.ptrvalue);
      break;
   case BLAST0Outblk_matrix:
      AsnGenericUserSeqOfFree((Pointer) pnt, (AsnOptFreeFunc) BLAST0MatrixFree);
      break;
   case BLAST0Outblk_kablk:
      AsnGenericUserSeqOfFree((Pointer) pnt, (AsnOptFreeFunc) BLAST0KABlkFree);
      break;
   case BLAST0Outblk_job_start:
      BLAST0JobDescFree(anp -> data.ptrvalue);
      break;
   case BLAST0Outblk_job_progress:
      BLAST0JobProgressFree(anp -> data.ptrvalue);
      break;
   case BLAST0Outblk_job_done:
      BLAST0JobProgressFree(anp -> data.ptrvalue);
      break;
   case BLAST0Outblk_result:
      BLAST0ResultFree(anp -> data.ptrvalue);
      break;
   case BLAST0Outblk_parms:
      AsnGenericBaseSeqOfFree((ValNodePtr) pnt,ASNCODE_PTRVAL_SLOT);
      break;
   case BLAST0Outblk_stats:
      AsnGenericBaseSeqOfFree((ValNodePtr) pnt,ASNCODE_PTRVAL_SLOT);
      break;
   case BLAST0Outblk_warning:
      BLAST0StatusFree(anp -> data.ptrvalue);
      break;
   case BLAST0Outblk_status:
      BLAST0StatusFree(anp -> data.ptrvalue);
      break;
   }
   return MemFree(anp);
}


/**************************************************
*
*    BLAST0RequestFree()
*
**************************************************/

BLAST0RequestPtr LIBCALL
BLAST0RequestFree(ValNodePtr anp)
{
   Pointer pnt;

   if (anp == NULL) {
      return NULL;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   default:
      break;
   case BLAST0Request_hello:
      MemFree(anp -> data.ptrvalue);
      break;
   case BLAST0Request_usage_info:
      MemFree(anp -> data.ptrvalue);
      break;
   case BLAST0Request_matrix_get:
      MemFree(anp -> data.ptrvalue);
      break;
   case BLAST0Request_search:
      BLAST0SearchFree(anp -> data.ptrvalue);
      break;
   }
   return MemFree(anp);
}


/**************************************************
*
*    BLAST0RequestAsnRead()
*
**************************************************/

BLAST0RequestPtr LIBCALL
BLAST0RequestAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   ValNodePtr anp;
   Uint1 choice;
   Boolean isError = FALSE;
   Boolean nullIsError = FALSE;
   AsnReadFunc func;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Request ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_REQUEST);
   } else {
      atp = AsnLinkType(orig, BLAST0_REQUEST);    /* link in local tree */
   }
   if (atp == NULL) {
      return NULL;
   }

   anp = ValNodeNew(NULL);
   if (anp == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the CHOICE or OpenStruct value (nothing) */
      goto erret;
   }

   func = NULL;

   atp = AsnReadId(aip, amp, atp);  /* find the choice */
   if (atp == NULL) {
      goto erret;
   }
   if (atp == BLAST0_REQUEST_hello) {
      choice = BLAST0Request_hello;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   else if (atp == BLAST0_REQUEST_motd) {
      choice = BLAST0Request_motd;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.boolvalue = av.boolvalue;
   }
   else if (atp == BLAST0_REQUEST_prog_info) {
      choice = BLAST0Request_prog_info;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.boolvalue = av.boolvalue;
   }
   else if (atp == BLAST0_REQUEST_usage_info) {
      choice = BLAST0Request_usage_info;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   else if (atp == BLAST0_REQUEST_db_info) {
      choice = BLAST0Request_db_info;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.boolvalue = av.boolvalue;
   }
   else if (atp == BLAST0_REQUEST_matrix_info) {
      choice = BLAST0Request_matrix_info;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.boolvalue = av.boolvalue;
   }
   else if (atp == BLAST0_REQUEST_matrix_get) {
      choice = BLAST0Request_matrix_get;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   else if (atp == BLAST0_REQUEST_search) {
      choice = BLAST0Request_search;
      func = (AsnReadFunc) BLAST0SearchAsnRead;
   }
   else if (atp == BLAST0_REQUEST_goodbye) {
      choice = BLAST0Request_goodbye;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.boolvalue = av.boolvalue;
   }
   anp->choice = choice;
   if (func != NULL)
   {
      anp->data.ptrvalue = (* func)(aip, atp);
      if (aip -> io_failure) goto erret;

      if (nullIsError && anp->data.ptrvalue == NULL) {
         goto erret;
      }
   }

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return anp;

erret:
   anp = MemFree(anp);
   aip -> io_failure = TRUE;
   goto ret;
}


/**************************************************
*
*    BLAST0RequestAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0RequestAsnWrite(BLAST0RequestPtr anp, AsnIoPtr aip, AsnTypePtr orig)

{
   DataVal av;
   AsnTypePtr atp, writetype = NULL;
   Pointer pnt;
   AsnWriteFunc func = NULL;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad())
      return FALSE;
   }

   if (aip == NULL)
   return FALSE;

   atp = AsnLinkType(orig, BLAST0_REQUEST);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

   av.ptrvalue = (Pointer)anp;
   if (! AsnWriteChoice(aip, atp, (Int2)anp->choice, &av)) {
      goto erret;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   case BLAST0Request_hello:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, BLAST0_REQUEST_hello, &av);
      break;
   case BLAST0Request_motd:
      av.boolvalue = anp->data.boolvalue;
      retval = AsnWrite(aip, BLAST0_REQUEST_motd, &av);
      break;
   case BLAST0Request_prog_info:
      av.boolvalue = anp->data.boolvalue;
      retval = AsnWrite(aip, BLAST0_REQUEST_prog_info, &av);
      break;
   case BLAST0Request_usage_info:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, BLAST0_REQUEST_usage_info, &av);
      break;
   case BLAST0Request_db_info:
      av.boolvalue = anp->data.boolvalue;
      retval = AsnWrite(aip, BLAST0_REQUEST_db_info, &av);
      break;
   case BLAST0Request_matrix_info:
      av.boolvalue = anp->data.boolvalue;
      retval = AsnWrite(aip, BLAST0_REQUEST_matrix_info, &av);
      break;
   case BLAST0Request_matrix_get:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, BLAST0_REQUEST_matrix_get, &av);
      break;
   case BLAST0Request_search:
      writetype = BLAST0_REQUEST_search;
      func = (AsnWriteFunc) BLAST0SearchAsnWrite;
      break;
   case BLAST0Request_goodbye:
      av.boolvalue = anp->data.boolvalue;
      retval = AsnWrite(aip, BLAST0_REQUEST_goodbye, &av);
      break;
   }
   if (writetype != NULL) {
      retval = (* func)(pnt, aip, writetype);   /* write it out */
   }
   if (!retval) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}


/**************************************************
*
*    BLAST0SearchNew()
*
**************************************************/

BLAST0SearchPtr LIBCALL
BLAST0SearchNew(void)
{
   BLAST0SearchPtr ptr = MemNew((size_t) sizeof(BLAST0Search));

   ptr -> return_matrix = 1;
   ptr -> return_query = 1;
   ptr -> return_BLAST0result = 1;
   ptr -> return_query_seq_in_seg = 1;
   ptr -> return_db_seq_in_seg = 1;
   return ptr;

}


/**************************************************
*
*    BLAST0SearchFree()
*
**************************************************/

BLAST0SearchPtr LIBCALL
BLAST0SearchFree(BLAST0SearchPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> program);
   MemFree(ptr -> database);
   BLAST0SequenceFree(ptr -> query);
   AsnGenericBaseSeqOfFree(ptr -> options ,ASNCODE_PTRVAL_SLOT);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0SearchAsnRead()
*
**************************************************/

BLAST0SearchPtr LIBCALL
BLAST0SearchAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0SearchPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Search ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_SEARCH);
   } else {
      atp = AsnLinkType(orig, BLAST0_SEARCH);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0SearchNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_SEARCH_program) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> program = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEARCH_database) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> database = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEARCH_query) {
      ptr -> query = BLAST0SequenceAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEARCH_options) {
      ptr -> options = AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_PTRVAL_SLOT, &isError);
      if (isError && ptr -> options == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEARCH_return_matrix) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> return_matrix = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEARCH_return_query) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> return_query = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SEARCH_return_BLAST0result) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> return_BLAST0result = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SEARCH_return_query_seq_in_seg) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> return_query_seq_in_seg = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == SEARCH_return_db_seq_in_seg) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> return_db_seq_in_seg = av.boolvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0SearchFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0SearchAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0SearchAsnWrite(BLAST0SearchPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_SEARCH);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> program != NULL) {
      av.ptrvalue = ptr -> program;
      retval = AsnWrite(aip, BLAST0_SEARCH_program,  &av);
   }
   if (ptr -> database != NULL) {
      av.ptrvalue = ptr -> database;
      retval = AsnWrite(aip, BLAST0_SEARCH_database,  &av);
   }
   if (ptr -> query != NULL) {
      if ( ! BLAST0SequenceAsnWrite(ptr -> query, aip, BLAST0_SEARCH_query)) {
         goto erret;
      }
   }
   retval = AsnGenericBaseSeqOfAsnWrite(ptr -> options ,ASNCODE_PTRVAL_SLOT, aip, BLAST0_SEARCH_options, BLAST0_SEARCH_options_E);
   av.boolvalue = ptr -> return_matrix;
   retval = AsnWrite(aip, BLAST0_SEARCH_return_matrix,  &av);
   av.boolvalue = ptr -> return_query;
   retval = AsnWrite(aip, BLAST0_SEARCH_return_query,  &av);
   av.boolvalue = ptr -> return_BLAST0result;
   retval = AsnWrite(aip, SEARCH_return_BLAST0result,  &av);
   av.boolvalue = ptr -> return_query_seq_in_seg;
   retval = AsnWrite(aip, SEARCH_return_query_seq_in_seg,  &av);
   av.boolvalue = ptr -> return_db_seq_in_seg;
   retval = AsnWrite(aip, SEARCH_return_db_seq_in_seg,  &av);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0ResponseFree()
*
**************************************************/

BLAST0ResponsePtr LIBCALL
BLAST0ResponseFree(ValNodePtr anp)
{
   Pointer pnt;

   if (anp == NULL) {
      return NULL;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   default:
      break;
   case BLAST0Response_hello:
      MemFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_motd:
      MemFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_prog_info:
      AsnGenericUserSeqOfFree((Pointer) pnt, (AsnOptFreeFunc) BLAST0PrefaceFree);
      break;
   case BLAST0Response_usage_info:
      BLAST0PrefaceFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_db_info:
      AsnGenericUserSeqOfFree((Pointer) pnt, (AsnOptFreeFunc) BLAST0DbDescFree);
      break;
   case BLAST0Response_matrix_info:
      AsnGenericUserSeqOfFree((Pointer) pnt, (AsnOptFreeFunc) BLAST0MatrixFree);
      break;
   case BLAST0Response_ack:
      BLAST0AckFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_goodbye:
      BLAST0AckFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_queued:
      BLAST0QueuedFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_preface:
      BLAST0PrefaceFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_query:
      BLAST0SequenceFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_dbdesc:
      BLAST0DbDescFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_matrix:
      AsnGenericUserSeqOfFree((Pointer) pnt, (AsnOptFreeFunc) BLAST0MatrixFree);
      break;
   case BLAST0Response_kablk:
      AsnGenericUserSeqOfFree((Pointer) pnt, (AsnOptFreeFunc) BLAST0KABlkFree);
      break;
   case BLAST0Response_job_start:
      BLAST0JobDescFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_job_progress:
      BLAST0JobProgressFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_job_done:
      BLAST0JobProgressFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_score_defs:
      AsnGenericUserSeqOfFree((Pointer) pnt, (AsnOptFreeFunc) BLAST0ScoreInfoFree);
      break;
   case BLAST0Response_result:
      BLAST0ResultFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_seqalign:
      SeqAlignSetFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_parms:
      AsnGenericBaseSeqOfFree((ValNodePtr) pnt,ASNCODE_PTRVAL_SLOT);
      break;
   case BLAST0Response_stats:
      AsnGenericBaseSeqOfFree((ValNodePtr) pnt,ASNCODE_PTRVAL_SLOT);
      break;
   case BLAST0Response_warning:
      BLAST0StatusFree(anp -> data.ptrvalue);
      break;
   case BLAST0Response_status:
      BLAST0StatusFree(anp -> data.ptrvalue);
      break;
   }
   return MemFree(anp);
}


/**************************************************
*
*    BLAST0ResponseAsnRead()
*
**************************************************/

BLAST0ResponsePtr LIBCALL
BLAST0ResponseAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   ValNodePtr anp;
   Uint1 choice;
   Boolean isError = FALSE;
   Boolean nullIsError = FALSE;
   AsnReadFunc func;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Response ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_RESPONSE);
   } else {
      atp = AsnLinkType(orig, BLAST0_RESPONSE);    /* link in local tree */
   }
   if (atp == NULL) {
      return NULL;
   }

   anp = ValNodeNew(NULL);
   if (anp == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the CHOICE or OpenStruct value (nothing) */
      goto erret;
   }

   func = NULL;

   atp = AsnReadId(aip, amp, atp);  /* find the choice */
   if (atp == NULL) {
      goto erret;
   }
   if (atp == BLAST0_RESPONSE_hello) {
      choice = BLAST0Response_hello;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   else if (atp == BLAST0_RESPONSE_motd) {
      choice = BLAST0Response_motd;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   else if (atp == BLAST0_RESPONSE_prog_info) {
      choice = BLAST0Response_prog_info;
      anp -> data.ptrvalue =
      AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0PrefaceAsnRead,             (AsnOptFreeFunc) BLAST0PrefaceFree);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == BLAST0_RESPONSE_usage_info) {
      choice = BLAST0Response_usage_info;
      func = (AsnReadFunc) BLAST0PrefaceAsnRead;
   }
   else if (atp == BLAST0_RESPONSE_db_info) {
      choice = BLAST0Response_db_info;
      anp -> data.ptrvalue =
      AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0DbDescAsnRead,             (AsnOptFreeFunc) BLAST0DbDescFree);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == BLAST0_RESPONSE_matrix_info) {
      choice = BLAST0Response_matrix_info;
      anp -> data.ptrvalue =
      AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0MatrixAsnRead,             (AsnOptFreeFunc) BLAST0MatrixFree);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == BLAST0_RESPONSE_ack) {
      choice = BLAST0Response_ack;
      func = (AsnReadFunc) BLAST0AckAsnRead;
   }
   else if (atp == BLAST0_RESPONSE_goodbye) {
      choice = BLAST0Response_goodbye;
      func = (AsnReadFunc) BLAST0AckAsnRead;
   }
   else if (atp == BLAST0_RESPONSE_queued) {
      choice = BLAST0Response_queued;
      func = (AsnReadFunc) BLAST0QueuedAsnRead;
   }
   else if (atp == BLAST0_RESPONSE_preface) {
      choice = BLAST0Response_preface;
      func = (AsnReadFunc) BLAST0PrefaceAsnRead;
   }
   else if (atp == BLAST0_RESPONSE_query) {
      choice = BLAST0Response_query;
      func = (AsnReadFunc) BLAST0SequenceAsnRead;
   }
   else if (atp == BLAST0_RESPONSE_dbdesc) {
      choice = BLAST0Response_dbdesc;
      func = (AsnReadFunc) BLAST0DbDescAsnRead;
   }
   else if (atp == BLAST0_RESPONSE_matrix) {
      choice = BLAST0Response_matrix;
      anp -> data.ptrvalue =
      AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0MatrixAsnRead,             (AsnOptFreeFunc) BLAST0MatrixFree);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == BLAST0_RESPONSE_kablk) {
      choice = BLAST0Response_kablk;
      anp -> data.ptrvalue =
      AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0KABlkAsnRead,             (AsnOptFreeFunc) BLAST0KABlkFree);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == BLAST0_RESPONSE_job_start) {
      choice = BLAST0Response_job_start;
      func = (AsnReadFunc) BLAST0JobDescAsnRead;
   }
   else if (atp == BLAST0_RESPONSE_job_progress) {
      choice = BLAST0Response_job_progress;
      func = (AsnReadFunc) BLAST0JobProgressAsnRead;
   }
   else if (atp == BLAST0_RESPONSE_job_done) {
      choice = BLAST0Response_job_done;
      func = (AsnReadFunc) BLAST0JobProgressAsnRead;
   }
   else if (atp == BLAST0_RESPONSE_score_defs) {
      choice = BLAST0Response_score_defs;
      anp -> data.ptrvalue =
      AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0ScoreInfoAsnRead,             (AsnOptFreeFunc) BLAST0ScoreInfoFree);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == BLAST0_RESPONSE_result) {
      choice = BLAST0Response_result;
      func = (AsnReadFunc) BLAST0ResultAsnRead;
   }
   else if (atp == BLAST0_RESPONSE_seqalign) {
      choice = BLAST0Response_seqalign;
      func = (AsnReadFunc) SeqAlignSetAsnRead;
   }
   else if (atp == BLAST0_RESPONSE_parms) {
      choice = BLAST0Response_parms;
      anp -> data.ptrvalue = 
      AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_PTRVAL_SLOT, &isError);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == BLAST0_RESPONSE_stats) {
      choice = BLAST0Response_stats;
      anp -> data.ptrvalue = 
      AsnGenericBaseSeqOfAsnRead(aip, amp, atp, ASNCODE_PTRVAL_SLOT, &isError);
      if (isError && anp -> data.ptrvalue == NULL) {
         goto erret;
      }
   }
   else if (atp == BLAST0_RESPONSE_warning) {
      choice = BLAST0Response_warning;
      func = (AsnReadFunc) BLAST0StatusAsnRead;
   }
   else if (atp == BLAST0_RESPONSE_status) {
      choice = BLAST0Response_status;
      func = (AsnReadFunc) BLAST0StatusAsnRead;
   }
   anp->choice = choice;
   if (func != NULL)
   {
      anp->data.ptrvalue = (* func)(aip, atp);
      if (aip -> io_failure) goto erret;

      if (nullIsError && anp->data.ptrvalue == NULL) {
         goto erret;
      }
   }

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return anp;

erret:
   anp = MemFree(anp);
   aip -> io_failure = TRUE;
   goto ret;
}


/**************************************************
*
*    BLAST0ResponseAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0ResponseAsnWrite(BLAST0ResponsePtr anp, AsnIoPtr aip, AsnTypePtr orig)

{
   DataVal av;
   AsnTypePtr atp, writetype = NULL;
   Pointer pnt;
   AsnWriteFunc func = NULL;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad())
      return FALSE;
   }

   if (aip == NULL)
   return FALSE;

   atp = AsnLinkType(orig, BLAST0_RESPONSE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

   av.ptrvalue = (Pointer)anp;
   if (! AsnWriteChoice(aip, atp, (Int2)anp->choice, &av)) {
      goto erret;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   case BLAST0Response_hello:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, BLAST0_RESPONSE_hello, &av);
      break;
   case BLAST0Response_motd:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, BLAST0_RESPONSE_motd, &av);
      break;
   case BLAST0Response_prog_info:
      retval = AsnGenericUserSeqOfAsnWrite((Pointer) pnt, (AsnWriteFunc) BLAST0PrefaceAsnWrite, aip, BLAST0_RESPONSE_prog_info, BLAST0_RESPONSE_prog_info_E);
      break;
   case BLAST0Response_usage_info:
      writetype = BLAST0_RESPONSE_usage_info;
      func = (AsnWriteFunc) BLAST0PrefaceAsnWrite;
      break;
   case BLAST0Response_db_info:
      retval = AsnGenericUserSeqOfAsnWrite((Pointer) pnt, (AsnWriteFunc) BLAST0DbDescAsnWrite, aip, BLAST0_RESPONSE_db_info, BLAST0_RESPONSE_db_info_E);
      break;
   case BLAST0Response_matrix_info:
      retval = AsnGenericUserSeqOfAsnWrite((Pointer) pnt, (AsnWriteFunc) BLAST0MatrixAsnWrite, aip, BLAST0_RESPONSE_matrix_info, BLAST0_RESPONSE_matrix_info_E);
      break;
   case BLAST0Response_ack:
      writetype = BLAST0_RESPONSE_ack;
      func = (AsnWriteFunc) BLAST0AckAsnWrite;
      break;
   case BLAST0Response_goodbye:
      writetype = BLAST0_RESPONSE_goodbye;
      func = (AsnWriteFunc) BLAST0AckAsnWrite;
      break;
   case BLAST0Response_queued:
      writetype = BLAST0_RESPONSE_queued;
      func = (AsnWriteFunc) BLAST0QueuedAsnWrite;
      break;
   case BLAST0Response_preface:
      writetype = BLAST0_RESPONSE_preface;
      func = (AsnWriteFunc) BLAST0PrefaceAsnWrite;
      break;
   case BLAST0Response_query:
      writetype = BLAST0_RESPONSE_query;
      func = (AsnWriteFunc) BLAST0SequenceAsnWrite;
      break;
   case BLAST0Response_dbdesc:
      writetype = BLAST0_RESPONSE_dbdesc;
      func = (AsnWriteFunc) BLAST0DbDescAsnWrite;
      break;
   case BLAST0Response_matrix:
      retval = AsnGenericUserSeqOfAsnWrite((Pointer) pnt, (AsnWriteFunc) BLAST0MatrixAsnWrite, aip, BLAST0_RESPONSE_matrix, BLAST0_RESPONSE_matrix_E);
      break;
   case BLAST0Response_kablk:
      retval = AsnGenericUserSeqOfAsnWrite((Pointer) pnt, (AsnWriteFunc) BLAST0KABlkAsnWrite, aip, BLAST0_RESPONSE_kablk, BLAST0_RESPONSE_kablk_E);
      break;
   case BLAST0Response_job_start:
      writetype = BLAST0_RESPONSE_job_start;
      func = (AsnWriteFunc) BLAST0JobDescAsnWrite;
      break;
   case BLAST0Response_job_progress:
      writetype = BLAST0_RESPONSE_job_progress;
      func = (AsnWriteFunc) BLAST0JobProgressAsnWrite;
      break;
   case BLAST0Response_job_done:
      writetype = BLAST0_RESPONSE_job_done;
      func = (AsnWriteFunc) BLAST0JobProgressAsnWrite;
      break;
   case BLAST0Response_score_defs:
      retval = AsnGenericUserSeqOfAsnWrite((Pointer) pnt, (AsnWriteFunc) BLAST0ScoreInfoAsnWrite, aip, BLAST0_RESPONSE_score_defs, BLAST0_RESPONSE_score_defs_E);
      break;
   case BLAST0Response_result:
      writetype = BLAST0_RESPONSE_result;
      func = (AsnWriteFunc) BLAST0ResultAsnWrite;
      break;
   case BLAST0Response_seqalign:
      writetype = BLAST0_RESPONSE_seqalign;
      func = (AsnWriteFunc) SeqAlignSetAsnWrite;
      break;
   case BLAST0Response_parms:
      retval = AsnGenericBaseSeqOfAsnWrite((Pointer) pnt,ASNCODE_PTRVAL_SLOT, aip, BLAST0_RESPONSE_parms, BLAST0_RESPONSE_parms_E);            break;
   case BLAST0Response_stats:
      retval = AsnGenericBaseSeqOfAsnWrite((Pointer) pnt,ASNCODE_PTRVAL_SLOT, aip, BLAST0_RESPONSE_stats, BLAST0_RESPONSE_stats_E);            break;
   case BLAST0Response_warning:
      writetype = BLAST0_RESPONSE_warning;
      func = (AsnWriteFunc) BLAST0StatusAsnWrite;
      break;
   case BLAST0Response_status:
      writetype = BLAST0_RESPONSE_status;
      func = (AsnWriteFunc) BLAST0StatusAsnWrite;
      break;
   }
   if (writetype != NULL) {
      retval = (* func)(pnt, aip, writetype);   /* write it out */
   }
   if (!retval) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}


/**************************************************
*
*    BLAST0AckNew()
*
**************************************************/

BLAST0AckPtr LIBCALL
BLAST0AckNew(void)
{
   BLAST0AckPtr ptr = MemNew((size_t) sizeof(BLAST0Ack));

   return ptr;

}


/**************************************************
*
*    BLAST0AckFree()
*
**************************************************/

BLAST0AckPtr LIBCALL
BLAST0AckFree(BLAST0AckPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> reason);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0AckAsnRead()
*
**************************************************/

BLAST0AckPtr LIBCALL
BLAST0AckAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0AckPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Ack ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_ACK);
   } else {
      atp = AsnLinkType(orig, BLAST0_ACK);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0AckNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_ACK_code) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> code = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_ACK_reason) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> reason = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0AckFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0AckAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0AckAsnWrite(BLAST0AckPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_ACK);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> code;
   retval = AsnWrite(aip, BLAST0_ACK_code,  &av);
   if (ptr -> reason != NULL) {
      av.ptrvalue = ptr -> reason;
      retval = AsnWrite(aip, BLAST0_ACK_reason,  &av);
   }
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0QueuedNew()
*
**************************************************/

BLAST0QueuedPtr LIBCALL
BLAST0QueuedNew(void)
{
   BLAST0QueuedPtr ptr = MemNew((size_t) sizeof(BLAST0Queued));

   return ptr;

}


/**************************************************
*
*    BLAST0QueuedFree()
*
**************************************************/

BLAST0QueuedPtr LIBCALL
BLAST0QueuedFree(BLAST0QueuedPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> name);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0QueuedAsnRead()
*
**************************************************/

BLAST0QueuedPtr LIBCALL
BLAST0QueuedAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0QueuedPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Queued ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_QUEUED);
   } else {
      atp = AsnLinkType(orig, BLAST0_QUEUED);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0QueuedNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_QUEUED_name) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> name = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_QUEUED_length) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> length = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0QueuedFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0QueuedAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0QueuedAsnWrite(BLAST0QueuedPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_QUEUED);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> name != NULL) {
      av.ptrvalue = ptr -> name;
      retval = AsnWrite(aip, BLAST0_QUEUED_name,  &av);
   }
   av.intvalue = ptr -> length;
   retval = AsnWrite(aip, BLAST0_QUEUED_length,  &av);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0ScoreInfoNew()
*
**************************************************/

BLAST0ScoreInfoPtr LIBCALL
BLAST0ScoreInfoNew(void)
{
   BLAST0ScoreInfoPtr ptr = MemNew((size_t) sizeof(BLAST0ScoreInfo));

   return ptr;

}


/**************************************************
*
*    BLAST0ScoreInfoFree()
*
**************************************************/

BLAST0ScoreInfoPtr LIBCALL
BLAST0ScoreInfoFree(BLAST0ScoreInfoPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   MemFree(ptr -> tag);
   MemFree(ptr -> desc);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0ScoreInfoAsnRead()
*
**************************************************/

BLAST0ScoreInfoPtr LIBCALL
BLAST0ScoreInfoAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0ScoreInfoPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0ScoreInfo ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_SCORE_INFO);
   } else {
      atp = AsnLinkType(orig, BLAST0_SCORE_INFO);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0ScoreInfoNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_SCORE_INFO_sid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> sid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SCORE_INFO_tag) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> tag = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SCORE_INFO_desc) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> desc = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0ScoreInfoFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0ScoreInfoAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0ScoreInfoAsnWrite(BLAST0ScoreInfoPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_SCORE_INFO);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> sid;
   retval = AsnWrite(aip, BLAST0_SCORE_INFO_sid,  &av);
   if (ptr -> tag != NULL) {
      av.ptrvalue = ptr -> tag;
      retval = AsnWrite(aip, BLAST0_SCORE_INFO_tag,  &av);
   }
   if (ptr -> desc != NULL) {
      av.ptrvalue = ptr -> desc;
      retval = AsnWrite(aip, BLAST0_SCORE_INFO_desc,  &av);
   }
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0SeqUsageNew()
*
**************************************************/

BLAST0SeqUsagePtr LIBCALL
BLAST0SeqUsageNew(void)
{
   BLAST0SeqUsagePtr ptr = MemNew((size_t) sizeof(BLAST0SeqUsage));

   return ptr;

}


/**************************************************
*
*    BLAST0SeqUsageFree()
*
**************************************************/

BLAST0SeqUsagePtr LIBCALL
BLAST0SeqUsageFree(BLAST0SeqUsagePtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0SeqUsageAsnRead()
*
**************************************************/

BLAST0SeqUsagePtr LIBCALL
BLAST0SeqUsageAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0SeqUsagePtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0SeqUsage ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_SEQ_USAGE);
   } else {
      atp = AsnLinkType(orig, BLAST0_SEQ_USAGE);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0SeqUsageNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_SEQ_USAGE_raw) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> raw = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEQ_USAGE_cooked) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> cooked = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0SeqUsageFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0SeqUsageAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0SeqUsageAsnWrite(BLAST0SeqUsagePtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_SEQ_USAGE);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> raw;
   retval = AsnWrite(aip, BLAST0_SEQ_USAGE_raw,  &av);
   av.intvalue = ptr -> cooked;
   retval = AsnWrite(aip, BLAST0_SEQ_USAGE_cooked,  &av);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0HistogramNew()
*
**************************************************/

BLAST0HistogramPtr LIBCALL
BLAST0HistogramNew(void)
{
   BLAST0HistogramPtr ptr = MemNew((size_t) sizeof(BLAST0Histogram));

   return ptr;

}


/**************************************************
*
*    BLAST0HistogramFree()
*
**************************************************/

BLAST0HistogramPtr LIBCALL
BLAST0HistogramFree(BLAST0HistogramPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> bar, (AsnOptFreeFunc) BLAST0HistogramBarFree);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0HistogramAsnRead()
*
**************************************************/

BLAST0HistogramPtr LIBCALL
BLAST0HistogramAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0HistogramPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Histogram ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_HISTOGRAM);
   } else {
      atp = AsnLinkType(orig, BLAST0_HISTOGRAM);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0HistogramNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_HISTOGRAM_expect) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> expect = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_HISTOGRAM_observed) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> observed = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_HISTOGRAM_base) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> base = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_HISTOGRAM_nbars) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> nbars = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_HISTOGRAM_bar) {
      ptr -> bar = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0HistogramBarAsnRead, (AsnOptFreeFunc) BLAST0HistogramBarFree);
      if (isError && ptr -> bar == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0HistogramFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0HistogramAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0HistogramAsnWrite(BLAST0HistogramPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_HISTOGRAM);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.realvalue = ptr -> expect;
   retval = AsnWrite(aip, BLAST0_HISTOGRAM_expect,  &av);
   av.intvalue = ptr -> observed;
   retval = AsnWrite(aip, BLAST0_HISTOGRAM_observed,  &av);
   av.intvalue = ptr -> base;
   retval = AsnWrite(aip, BLAST0_HISTOGRAM_base,  &av);
   av.intvalue = ptr -> nbars;
   retval = AsnWrite(aip, BLAST0_HISTOGRAM_nbars,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> bar, (AsnWriteFunc) BLAST0HistogramBarAsnWrite, aip, BLAST0_HISTOGRAM_bar, BLAST0_HISTOGRAM_bar_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0HitListNew()
*
**************************************************/

BLAST0HitListPtr LIBCALL
BLAST0HitListNew(void)
{
   BLAST0HitListPtr ptr = MemNew((size_t) sizeof(BLAST0HitList));

   return ptr;

}


/**************************************************
*
*    BLAST0HitListFree()
*
**************************************************/

BLAST0HitListPtr LIBCALL
BLAST0HitListFree(BLAST0HitListPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   AsnGenericUserSeqOfFree(ptr -> kablk, (AsnOptFreeFunc) BLAST0KABlkFree);
   AsnGenericUserSeqOfFree(ptr -> hsps, (AsnOptFreeFunc) BLAST0HSPFree);
   AsnGenericUserSeqOfFree(ptr -> seqs, (AsnOptFreeFunc) BLAST0SequenceFree);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0HitListAsnRead()
*
**************************************************/

BLAST0HitListPtr LIBCALL
BLAST0HitListAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0HitListPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0HitList ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_HITLIST);
   } else {
      atp = AsnLinkType(orig, BLAST0_HITLIST);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0HitListNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_HITLIST_count) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> count = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_HITLIST_kablk) {
      ptr -> kablk = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0KABlkAsnRead, (AsnOptFreeFunc) BLAST0KABlkFree);
      if (isError && ptr -> kablk == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_HITLIST_hsps) {
      ptr -> hsps = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0HSPAsnRead, (AsnOptFreeFunc) BLAST0HSPFree);
      if (isError && ptr -> hsps == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_HITLIST_seqs) {
      ptr -> seqs = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0SequenceAsnRead, (AsnOptFreeFunc) BLAST0SequenceFree);
      if (isError && ptr -> seqs == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0HitListFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0HitListAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0HitListAsnWrite(BLAST0HitListPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_HITLIST);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> count;
   retval = AsnWrite(aip, BLAST0_HITLIST_count,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> kablk, (AsnWriteFunc) BLAST0KABlkAsnWrite, aip, BLAST0_HITLIST_kablk, BLAST0_HITLIST_kablk_E);
   AsnGenericUserSeqOfAsnWrite(ptr -> hsps, (AsnWriteFunc) BLAST0HSPAsnWrite, aip, BLAST0_HITLIST_hsps, BLAST0_HITLIST_hsps_E);
   AsnGenericUserSeqOfAsnWrite(ptr -> seqs, (AsnWriteFunc) BLAST0SequenceAsnWrite, aip, BLAST0_HITLIST_seqs, BLAST0_HITLIST_seqs_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0HistogramBarNew()
*
**************************************************/

BLAST0HistogramBarPtr LIBCALL
BLAST0HistogramBarNew(void)
{
   BLAST0HistogramBarPtr ptr = MemNew((size_t) sizeof(BLAST0HistogramBar));

   return ptr;

}


/**************************************************
*
*    BLAST0HistogramBarFree()
*
**************************************************/

BLAST0HistogramBarPtr LIBCALL
BLAST0HistogramBarFree(BLAST0HistogramBarPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0HistogramBarAsnRead()
*
**************************************************/

BLAST0HistogramBarPtr LIBCALL
BLAST0HistogramBarAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0HistogramBarPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0HistogramBar ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_HISTOGRAM_BAR);
   } else {
      atp = AsnLinkType(orig, BLAST0_HISTOGRAM_BAR);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0HistogramBarNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_HISTOGRAM_BAR_x) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> x = av.realvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_HISTOGRAM_BAR_n) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> n = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0HistogramBarFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0HistogramBarAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0HistogramBarAsnWrite(BLAST0HistogramBarPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_HISTOGRAM_BAR);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.realvalue = ptr -> x;
   retval = AsnWrite(aip, BLAST0_HISTOGRAM_BAR_x,  &av);
   av.intvalue = ptr -> n;
   retval = AsnWrite(aip, BLAST0_HISTOGRAM_BAR_n,  &av);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0HSPNew()
*
**************************************************/

BLAST0HSPPtr LIBCALL
BLAST0HSPNew(void)
{
   BLAST0HSPPtr ptr = MemNew((size_t) sizeof(BLAST0HSP));

   return ptr;

}


/**************************************************
*
*    BLAST0HSPFree()
*
**************************************************/

BLAST0HSPPtr LIBCALL
BLAST0HSPFree(BLAST0HSPPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   ScoreSetFree(ptr -> scores);
   AsnGenericUserSeqOfFree(ptr -> segs, (AsnOptFreeFunc) BLAST0SegmentFree);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0HSPAsnRead()
*
**************************************************/

BLAST0HSPPtr LIBCALL
BLAST0HSPAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0HSPPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0HSP ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_HSP);
   } else {
      atp = AsnLinkType(orig, BLAST0_HSP);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0HSPNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_HSP_matid) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> matid = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_HSP_scores) {
      ptr -> scores = ScoreSetAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_HSP_len) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> len = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_HSP_segs) {
      ptr -> segs = AsnGenericUserSeqOfAsnRead(aip, amp, atp, &isError, (AsnReadFunc) BLAST0SegmentAsnRead, (AsnOptFreeFunc) BLAST0SegmentFree);
      if (isError && ptr -> segs == NULL) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0HSPFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0HSPAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0HSPAsnWrite(BLAST0HSPPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_HSP);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   av.intvalue = ptr -> matid;
   retval = AsnWrite(aip, BLAST0_HSP_matid,  &av);
   if (ptr -> scores != NULL) {
      if ( ! ScoreSetAsnWrite(ptr -> scores, aip, BLAST0_HSP_scores)) {
         goto erret;
      }
   }
   av.intvalue = ptr -> len;
   retval = AsnWrite(aip, BLAST0_HSP_len,  &av);
   AsnGenericUserSeqOfAsnWrite(ptr -> segs, (AsnWriteFunc) BLAST0SegmentAsnWrite, aip, BLAST0_HSP_segs, BLAST0_HSP_segs_E);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0SegmentNew()
*
**************************************************/

BLAST0SegmentPtr LIBCALL
BLAST0SegmentNew(void)
{
   BLAST0SegmentPtr ptr = MemNew((size_t) sizeof(BLAST0Segment));

   return ptr;

}


/**************************************************
*
*    BLAST0SegmentFree()
*
**************************************************/

BLAST0SegmentPtr LIBCALL
BLAST0SegmentFree(BLAST0SegmentPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   BLAST0SeqIntervalFree(ptr -> loc);
   BLAST0SeqDataFree(ptr -> str);
   BLAST0SeqDataFree(ptr -> str_raw);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0SegmentAsnRead()
*
**************************************************/

BLAST0SegmentPtr LIBCALL
BLAST0SegmentAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0SegmentPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0Segment ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_SEGMENT);
   } else {
      atp = AsnLinkType(orig, BLAST0_SEGMENT);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0SegmentNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_SEGMENT_loc) {
      ptr -> loc = BLAST0SeqIntervalAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEGMENT_str) {
      ptr -> str = BLAST0SeqDataAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEGMENT_str_raw) {
      ptr -> str_raw = BLAST0SeqDataAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0SegmentFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0SegmentAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0SegmentAsnWrite(BLAST0SegmentPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_SEGMENT);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> loc != NULL) {
      if ( ! BLAST0SeqIntervalAsnWrite(ptr -> loc, aip, BLAST0_SEGMENT_loc)) {
         goto erret;
      }
   }
   if (ptr -> str != NULL) {
      if ( ! BLAST0SeqDataAsnWrite(ptr -> str, aip, BLAST0_SEGMENT_str)) {
         goto erret;
      }
   }
   if (ptr -> str_raw != NULL) {
      if ( ! BLAST0SeqDataAsnWrite(ptr -> str_raw, aip, BLAST0_SEGMENT_str_raw)) {
         goto erret;
      }
   }
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0SeqIntervalNew()
*
**************************************************/

BLAST0SeqIntervalPtr LIBCALL
BLAST0SeqIntervalNew(void)
{
   BLAST0SeqIntervalPtr ptr = MemNew((size_t) sizeof(BLAST0SeqInterval));

   return ptr;

}


/**************************************************
*
*    BLAST0SeqIntervalFree()
*
**************************************************/

BLAST0SeqIntervalPtr LIBCALL
BLAST0SeqIntervalFree(BLAST0SeqIntervalPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0SeqIntervalAsnRead()
*
**************************************************/

BLAST0SeqIntervalPtr LIBCALL
BLAST0SeqIntervalAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0SeqIntervalPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0SeqInterval ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_SEQ_INTERVAL);
   } else {
      atp = AsnLinkType(orig, BLAST0_SEQ_INTERVAL);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0SeqIntervalNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_SEQ_INTERVAL_strand) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> strand = av.intvalue;
      ptr -> OBbits__ |= 1<<0;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEQ_INTERVAL_from) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> from = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEQ_INTERVAL_to) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> to = av.intvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0SeqIntervalFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0SeqIntervalAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0SeqIntervalAsnWrite(BLAST0SeqIntervalPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_SEQ_INTERVAL);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> strand || (ptr -> OBbits__ & (1<<0) )){   av.intvalue = ptr -> strand;
      retval = AsnWrite(aip, BLAST0_SEQ_INTERVAL_strand,  &av);
   }
   av.intvalue = ptr -> from;
   retval = AsnWrite(aip, BLAST0_SEQ_INTERVAL_from,  &av);
   av.intvalue = ptr -> to;
   retval = AsnWrite(aip, BLAST0_SEQ_INTERVAL_to,  &av);
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0SeqDataFree()
*
**************************************************/

BLAST0SeqDataPtr LIBCALL
BLAST0SeqDataFree(ValNodePtr anp)
{
   Pointer pnt;

   if (anp == NULL) {
      return NULL;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   default:
      break;
   case BLAST0SeqData_ncbistdaa:
      BSFree(anp -> data.ptrvalue);
      break;
   case BLAST0SeqData_ncbi2na:
      BSFree(anp -> data.ptrvalue);
      break;
   case BLAST0SeqData_ncbi4na:
      BSFree(anp -> data.ptrvalue);
      break;
   }
   return MemFree(anp);
}


/**************************************************
*
*    BLAST0SeqDataAsnRead()
*
**************************************************/

BLAST0SeqDataPtr LIBCALL
BLAST0SeqDataAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   ValNodePtr anp;
   Uint1 choice;
   Boolean isError = FALSE;
   Boolean nullIsError = FALSE;
   AsnReadFunc func;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0SeqData ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_SEQ_DATA);
   } else {
      atp = AsnLinkType(orig, BLAST0_SEQ_DATA);    /* link in local tree */
   }
   if (atp == NULL) {
      return NULL;
   }

   anp = ValNodeNew(NULL);
   if (anp == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the CHOICE or OpenStruct value (nothing) */
      goto erret;
   }

   func = NULL;

   atp = AsnReadId(aip, amp, atp);  /* find the choice */
   if (atp == NULL) {
      goto erret;
   }
   if (atp == BLAST0_SEQ_DATA_ncbistdaa) {
      choice = BLAST0SeqData_ncbistdaa;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   else if (atp == BLAST0_SEQ_DATA_ncbi2na) {
      choice = BLAST0SeqData_ncbi2na;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   else if (atp == BLAST0_SEQ_DATA_ncbi4na) {
      choice = BLAST0SeqData_ncbi4na;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   anp->choice = choice;
   if (func != NULL)
   {
      anp->data.ptrvalue = (* func)(aip, atp);
      if (aip -> io_failure) goto erret;

      if (nullIsError && anp->data.ptrvalue == NULL) {
         goto erret;
      }
   }

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return anp;

erret:
   anp = MemFree(anp);
   aip -> io_failure = TRUE;
   goto ret;
}


/**************************************************
*
*    BLAST0SeqDataAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0SeqDataAsnWrite(BLAST0SeqDataPtr anp, AsnIoPtr aip, AsnTypePtr orig)

{
   DataVal av;
   AsnTypePtr atp, writetype = NULL;
   Pointer pnt;
   AsnWriteFunc func = NULL;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad())
      return FALSE;
   }

   if (aip == NULL)
   return FALSE;

   atp = AsnLinkType(orig, BLAST0_SEQ_DATA);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

   av.ptrvalue = (Pointer)anp;
   if (! AsnWriteChoice(aip, atp, (Int2)anp->choice, &av)) {
      goto erret;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   case BLAST0SeqData_ncbistdaa:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, BLAST0_SEQ_DATA_ncbistdaa, &av);
      break;
   case BLAST0SeqData_ncbi2na:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, BLAST0_SEQ_DATA_ncbi2na, &av);
      break;
   case BLAST0SeqData_ncbi4na:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, BLAST0_SEQ_DATA_ncbi4na, &av);
      break;
   }
   if (writetype != NULL) {
      retval = (* func)(pnt, aip, writetype);   /* write it out */
   }
   if (!retval) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}


/**************************************************
*
*    BLAST0SeqDescNew()
*
**************************************************/

BLAST0SeqDescPtr LIBCALL
BLAST0SeqDescNew(void)
{
   BLAST0SeqDescPtr ptr = MemNew((size_t) sizeof(BLAST0SeqDesc));

   return ptr;

}


/**************************************************
*
*    BLAST0SeqDescFree()
*
**************************************************/

BLAST0SeqDescPtr LIBCALL
BLAST0SeqDescFree(BLAST0SeqDescPtr ptr)
{

   if(ptr == NULL) {
      return NULL;
   }
   BLAST0SeqIdFree(ptr -> id);
   MemFree(ptr -> defline);
   return MemFree(ptr);
}


/**************************************************
*
*    BLAST0SeqDescAsnRead()
*
**************************************************/

BLAST0SeqDescPtr LIBCALL
BLAST0SeqDescAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean isError = FALSE;
   AsnReadFunc func;
   BLAST0SeqDescPtr ptr;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0SeqDesc ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_SEQ_DESC);
   } else {
      atp = AsnLinkType(orig, BLAST0_SEQ_DESC);
   }
   /* link in local tree */
   if (atp == NULL) {
      return NULL;
   }

   ptr = BLAST0SeqDescNew();
   if (ptr == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the start struct */
      goto erret;
   }

   atp = AsnReadId(aip,amp, atp);
   func = NULL;

   if (atp == BLAST0_SEQ_DESC_id) {
      ptr -> id = BLAST0SeqIdAsnRead(aip, atp);
      if (aip -> io_failure) {
         goto erret;
      }
      atp = AsnReadId(aip,amp, atp);
   }
   if (atp == BLAST0_SEQ_DESC_defline) {
      if ( AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      ptr -> defline = av.ptrvalue;
      atp = AsnReadId(aip,amp, atp);
   }

   if (AsnReadVal(aip, atp, &av) <= 0) {
      goto erret;
   }
   /* end struct */

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return ptr;

erret:
   aip -> io_failure = TRUE;
   ptr = BLAST0SeqDescFree(ptr);
   goto ret;
}



/**************************************************
*
*    BLAST0SeqDescAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0SeqDescAsnWrite(BLAST0SeqDescPtr ptr, AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return FALSE;
      }
   }

   if (aip == NULL) {
      return FALSE;
   }

   atp = AsnLinkType(orig, BLAST0_SEQ_DESC);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (ptr == NULL) { AsnNullValueMsg(aip, atp); goto erret; }
   if (! AsnOpenStruct(aip, atp, (Pointer) ptr)) {
      goto erret;
   }

   if (ptr -> id != NULL) {
      if ( ! BLAST0SeqIdAsnWrite(ptr -> id, aip, BLAST0_SEQ_DESC_id)) {
         goto erret;
      }
   }
   if (ptr -> defline != NULL) {
      av.ptrvalue = ptr -> defline;
      retval = AsnWrite(aip, BLAST0_SEQ_DESC_defline,  &av);
   }
   if (! AsnCloseStruct(aip, atp, (Pointer)ptr)) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}



/**************************************************
*
*    BLAST0SeqIdFree()
*
**************************************************/

BLAST0SeqIdPtr LIBCALL
BLAST0SeqIdFree(ValNodePtr anp)
{

   if (anp == NULL) {
      return NULL;
   }

   AsnGenericChoiceSeqOfFree(anp, (AsnOptFreeFunc) BLAST0SeqId_elementFree);    
   return NULL;
}


/**************************************************
*
*    BLAST0SeqIdAsnRead()
*
**************************************************/

BLAST0SeqIdPtr LIBCALL
BLAST0SeqIdAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{


   DataVal av;
   AsnTypePtr atp;
   ValNodePtr anp;
   Uint1 choice;
   Boolean isError = FALSE;
   AsnReadFunc func;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0SeqId_element ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_SEQ_ID);
   } else {
      atp = AsnLinkType(orig, BLAST0_SEQ_ID);    /* link in local tree */
   }
   if (atp == NULL) {
      return NULL;
   }

   anp =
   AsnGenericChoiceSeqOfAsnRead(aip, amp, atp, &isError,
   (AsnReadFunc) BLAST0SeqId_elementAsnRead, (AsnOptFreeFunc) BLAST0SeqId_elementFree);
   if (isError) 
   goto erret;


ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return anp;

erret:
   aip -> io_failure = TRUE;
   goto ret;
}


/**************************************************
*
*    BLAST0SeqIdAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0SeqIdAsnWrite(ValNodePtr anp, AsnIoPtr aip, AsnTypePtr orig)

{
   DataVal av;
   AsnTypePtr atp, writetype = NULL;
   Pointer pnt;
   AsnWriteFunc func = NULL;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad())
      return FALSE;
   }

   if (aip == NULL)
   return FALSE;

   atp = AsnLinkType(orig, BLAST0_SEQ_ID);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

   av.ptrvalue = (Pointer)anp;
   retval = AsnGenericChoiceSeqOfAsnWrite(anp, 
   (AsnWriteFunc) BLAST0SeqId_elementAsnWrite, aip, atp, BLAST0_SEQ_ID_E);
erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}


/**************************************************
*
*    BLAST0SeqId_elementAsnWrite()
*
**************************************************/
Boolean LIBCALL 
BLAST0SeqId_elementAsnWrite(BLAST0SeqId_elementPtr anp, AsnIoPtr aip, AsnTypePtr orig)

{
   DataVal av;
   AsnTypePtr atp, writetype = NULL;
   Pointer pnt;
   AsnWriteFunc func = NULL;
   Boolean retval = FALSE;

   if (! loaded)
   {
      if (! objblst2AsnLoad())
      return FALSE;
   }

   if (aip == NULL)
   return FALSE;

   atp = AsnLinkType(orig, BLAST0_SEQ_ID_E);   /* link local tree */
   if (atp == NULL) {
      return FALSE;
   }

   if (anp == NULL) { AsnNullValueMsg(aip, atp); goto erret; }

   av.ptrvalue = (Pointer)anp;
   if (! AsnWriteChoice(aip, atp, (Int2)anp->choice, &av)) {
      goto erret;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   case BLAST0SeqId_giid:
      av.intvalue = anp->data.intvalue;
      retval = AsnWrite(aip, BLAST0_SEQ_ID_E_giid, &av);
      break;
   case BLAST0SeqId_textid:
      av.ptrvalue = anp->data.ptrvalue;
      retval = AsnWrite(aip, BLAST0_SEQ_ID_E_textid, &av);
      break;
   }
   if (writetype != NULL) {
      retval = (* func)(pnt, aip, writetype);   /* write it out */
   }
   if (!retval) {
      goto erret;
   }
   retval = TRUE;

erret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return retval;
}


/**************************************************
*
*    BLAST0SeqId_elementAsnRead()
*
**************************************************/

BLAST0SeqId_elementPtr LIBCALL
BLAST0SeqId_elementAsnRead(AsnIoPtr aip, AsnTypePtr orig)
{
   DataVal av;
   AsnTypePtr atp;
   ValNodePtr anp;
   Uint1 choice;
   Boolean isError = FALSE;
   Boolean nullIsError = FALSE;
   AsnReadFunc func;

   if (! loaded)
   {
      if (! objblst2AsnLoad()) {
         return NULL;
      }
   }

   if (aip == NULL) {
      return NULL;
   }

   if (orig == NULL) {         /* BLAST0SeqId_element ::= (self contained) */
      atp = AsnReadId(aip, amp, BLAST0_SEQ_ID_E);
   } else {
      atp = AsnLinkType(orig, BLAST0_SEQ_ID_E);    /* link in local tree */
   }
   if (atp == NULL) {
      return NULL;
   }

   anp = ValNodeNew(NULL);
   if (anp == NULL) {
      goto erret;
   }
   if (AsnReadVal(aip, atp, &av) <= 0) { /* read the CHOICE or OpenStruct value (nothing) */
      goto erret;
   }

   func = NULL;

   atp = AsnReadId(aip, amp, atp);  /* find the choice */
   if (atp == NULL) {
      goto erret;
   }
   if (atp == BLAST0_SEQ_ID_E_giid) {
      choice = BLAST0SeqId_giid;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.intvalue = av.intvalue;
   }
   else if (atp == BLAST0_SEQ_ID_E_textid) {
      choice = BLAST0SeqId_textid;
      if (AsnReadVal(aip, atp, &av) <= 0) {
         goto erret;
      }
      anp->data.ptrvalue = av.ptrvalue;
   }
   anp->choice = choice;
   if (func != NULL)
   {
      anp->data.ptrvalue = (* func)(aip, atp);
      if (aip -> io_failure) goto erret;

      if (nullIsError && anp->data.ptrvalue == NULL) {
         goto erret;
      }
   }

ret:
   AsnUnlinkType(orig);       /* unlink local tree */
   return anp;

erret:
   anp = MemFree(anp);
   aip -> io_failure = TRUE;
   goto ret;
}


/**************************************************
*
*    BLAST0SeqId_elementFree()
*
**************************************************/

BLAST0SeqId_elementPtr LIBCALL
BLAST0SeqId_elementFree(ValNodePtr anp)
{
   Pointer pnt;

   if (anp == NULL) {
      return NULL;
   }

   pnt = anp->data.ptrvalue;
   switch (anp->choice)
   {
   default:
      break;
   case BLAST0SeqId_textid:
      MemFree(anp -> data.ptrvalue);
      break;
   }
   return MemFree(anp);
}
