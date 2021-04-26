#ifndef _id0gen_ 
#define _id0gen_ 


/**************************************************
*
*    Generated objects for Module NCBI-ID0Access
*
**************************************************/


/**************************************************
*
*    ID0serverRequest
*
**************************************************/
typedef ValNode  ID0serverRequest;
typedef ValNodePtr ID0serverRequestPtr;
#define ID0serverRequest_init 1  /*      NULL       */
#define ID0serverRequest_getgi 2  /*      Seq-id       */
#define ID0serverRequest_getsefromgi 3  /*      ID0server-maxcomplex       */
#define ID0serverRequest_fini 4  /*      NULL       */


ID0serverRequestPtr ID0serverRequestFree PROTO ((ID0serverRequestPtr ));
ID0serverRequestPtr ID0serverRequestAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean ID0serverRequestAsnWrite PROTO (( ID0serverRequestPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    ID0serverMaxcomplex
*
**************************************************/
typedef struct struct_ID0server_maxcomplex {
  Uint2 maxplex;
   Int4   gi;
} ID0serverMaxcomplex, PNTR ID0serverMaxcomplexPtr;


ID0serverMaxcomplexPtr ID0serverMaxcomplexFree PROTO ((ID0serverMaxcomplexPtr ));
ID0serverMaxcomplexPtr ID0serverMaxcomplexNew PROTO (( void ));
ID0serverMaxcomplexPtr ID0serverMaxcomplexAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean ID0serverMaxcomplexAsnWrite PROTO (( ID0serverMaxcomplexPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    EntryComplexities
*
**************************************************/
typedef Uint2 EntryComplexities;
typedef Uint2 PNTR EntryComplexitiesPtr;


/**************************************************
*
*    ID0serverBack
*
**************************************************/
typedef ValNode  ID0serverBack;
typedef ValNodePtr ID0serverBackPtr;
#define ID0serverBack_init 1  /*      NULL       */
#define ID0serverBack_error 2  /*      INTEGER       */
#define ID0serverBack_gotgi 3  /*      INTEGER       */
#define ID0serverBack_gotseqentry 4  /*      Seq-entry       */
#define ID0serverBack_gotdeadseqentry 5  /*      Seq-entry       */
#define ID0serverBack_fini 6  /*      NULL       */


ID0serverBackPtr ID0serverBackFree PROTO ((ID0serverBackPtr ));
ID0serverBackPtr ID0serverBackAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean ID0serverBackAsnWrite PROTO (( ID0serverBackPtr , AsnIoPtr, AsnTypePtr));



/**************************************************
*
*    ID0serverDebug
*
**************************************************/
typedef ValNode  ID0serverDebug;
typedef ValNodePtr ID0serverDebugPtr;


ID0serverDebugPtr ID0serverDebugFree PROTO ((ID0serverDebugPtr ));
ID0serverDebugPtr ID0serverDebugAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean ID0serverDebugAsnWrite PROTO (( ID0serverDebugPtr , AsnIoPtr, AsnTypePtr));

#endif
