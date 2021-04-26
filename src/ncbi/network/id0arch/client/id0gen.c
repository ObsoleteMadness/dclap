#include <asn.h>

#ifdef NLM_OBJ_INCL
#include NLM_OBJ_INCL
#endif
#include "id0gen.h"

static Boolean loaded = FALSE;

#include "asnid0.h"
#include <sequtil.h>

#ifndef NLM_EXTERN_LOADS
#define NLM_EXTERN_LOADS {}
#endif

static Boolean id0gen_AsnLoad(void)
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
*
*    Generated object loaders for Module NCBI-ID0Access
*
**************************************************/


/**************************************************
*
*    ID0serverRequestFree()
*
**************************************************/
ID0serverRequestPtr ID0serverRequestFree ( ID0serverRequestPtr ptr)
{
	if (ptr == NULL) return NULL;

   {Pointer pt = ptr -> data.ptrvalue;
   switch (ptr ->  choice){
        case  ID0serverRequest_getgi :
        SeqIdFree(pt);
                break;
        case  ID0serverRequest_getsefromgi :
          ID0serverMaxcomplexFree(pt);
                  break;
   }}
   MemFree(ptr);
      return NULL;
}


/**************************************************
*
*    ID0serverMaxcomplexFree()
*
**************************************************/
ID0serverMaxcomplexPtr ID0serverMaxcomplexFree ( ID0serverMaxcomplexPtr ptr)
{
	if (ptr == NULL) return NULL;

   MemFree(ptr);
      return NULL;
}


/**************************************************
*
*    ID0serverBackFree()
*
**************************************************/
ID0serverBackPtr ID0serverBackFree ( ID0serverBackPtr ptr)
{
	if (ptr == NULL) return NULL;

   {Pointer pt = ptr -> data.ptrvalue;
   switch (ptr ->  choice){
        case  ID0serverBack_gotseqentry :
          SeqEntryFree(pt);
                  break;
   }}
   MemFree(ptr);
      return NULL;
}


/**************************************************
*
*    ID0serverMaxcomplexNew()
*
**************************************************/

ID0serverMaxcomplexPtr
ID0serverMaxcomplexNew()
{

     return (ID0serverMaxcomplexPtr) MemNew(sizeof(ID0serverMaxcomplex));

}



/**************************************************
*
*    ID0serverRequestAsnRead()
*
**************************************************/
ID0serverRequestPtr ID0serverRequestAsnRead ( AsnIoPtr aip, AsnTypePtr orig)
{
 DataVal av;
   AsnTypePtr atp;
       ValNodePtr vnp;
       Uint1 choice ;
        AsnReadFunc func;

       if (aip == NULL) return NULL;


       if ( ! loaded){
                if ( ! id0gen_AsnLoad ())
                        goto erret;
                loaded = TRUE;
        }
 if (orig == NULL)
         atp = AsnReadId(aip, amp, ID0SERVER_REQUEST);
 else
         atp = AsnLinkType(orig, ID0SERVER_REQUEST);
 if(atp == NULL) return NULL;

         vnp = ValNodeNew(NULL);
         if (vnp == NULL) goto erret;
         if ( AsnReadVal(aip, atp, &av) <= 0) goto erret;
         if ( (atp = AsnReadId(aip, amp, atp)) == NULL) goto erret;
         func = NULL;
         if (atp == ID0SERVER_REQUEST_init){
                 choice = ID0serverRequest_init;
                       AsnReadVal(aip,atp,&av);
                       vnp -> data.ptrvalue = av.ptrvalue;
         }
                else
         if (atp == ID0SERVER_REQUEST_getgi){
                 choice = ID0serverRequest_getgi;
                       func = (AsnReadFunc) SeqIdAsnRead;
         }
                else
         if (atp == ID0SERVER_REQUEST_getsefromgi){
                 choice = ID0serverRequest_getsefromgi;
                       func = (AsnReadFunc) ID0serverMaxcomplexAsnRead;
         }
                else
         if (atp == ID0SERVER_REQUEST_fini){
                 choice = ID0serverRequest_fini;
                       AsnReadVal(aip,atp,&av);
                       vnp -> data.ptrvalue = av.ptrvalue;
         }
            else
                  goto erret;
           vnp -> choice = choice;
                if (func != NULL)
                        vnp -> data.ptrvalue = (*func) (aip,atp);
ret:
    AsnUnlinkType(orig);
  return vnp;
erret:
   vnp=ID0serverRequestFree(vnp);
      goto ret;
}


/**************************************************
*
*    ID0serverMaxcomplexAsnRead()
*
**************************************************/
ID0serverMaxcomplexPtr ID0serverMaxcomplexAsnRead ( AsnIoPtr aip, AsnTypePtr orig)
{
 DataVal av;
   AsnTypePtr atp;
       ID0serverMaxcomplexPtr ptr;

       if (aip == NULL) return NULL;


       if ( ! loaded){
                if ( ! id0gen_AsnLoad ())
                        goto erret;
                loaded = TRUE;
        }
 if (orig == NULL)
         atp = AsnReadId(aip, amp, ID0SERVER_MAXCOMPLEX);
 else
         atp = AsnLinkType(orig, ID0SERVER_MAXCOMPLEX);
 if(atp == NULL) return NULL;

     ptr = ID0serverMaxcomplexNew();
        if (ptr == NULL) goto erret;

 if (AsnReadVal(aip, atp, &av) <= 0) goto erret;

      atp = AsnReadId(aip,amp, atp);
       if (AsnReadVal(aip, atp, &av) <= 0)
             goto erret;
       ptr->maxplex = (Uint2)av.intvalue;
      atp = AsnReadId(aip,amp, atp);
      if (atp == ID0SERVER_MAXCOMPLEX_gi){
       if (AsnReadVal(aip, atp, &av) <= 0)
             goto erret;
       ptr->gi = av.intvalue;
           atp = AsnReadId(aip,amp, atp);
      }
	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
ret:
    AsnUnlinkType(orig);
  return ptr;
erret:
   ptr=ID0serverMaxcomplexFree(ptr);
      goto ret;
}


/**************************************************
*
*    ID0serverBackAsnRead()
*
**************************************************/
ID0serverBackPtr ID0serverBackAsnRead ( AsnIoPtr aip, AsnTypePtr orig)
{
 DataVal av;
   AsnTypePtr atp;
       ValNodePtr vnp;
       Uint1 choice ;
        AsnReadFunc func;

       if (aip == NULL) return NULL;


       if ( ! loaded){
                if ( ! id0gen_AsnLoad ())
                        goto erret;
                loaded = TRUE;
        }
 if (orig == NULL)
         atp = AsnReadId(aip, amp, ID0SERVER_BACK);
 else
         atp = AsnLinkType(orig, ID0SERVER_BACK);
 if(atp == NULL) return NULL;

         vnp = ValNodeNew(NULL);
         if (vnp == NULL) goto erret;
         if ( AsnReadVal(aip, atp, &av) <= 0) goto erret;
         if ( (atp = AsnReadId(aip, amp, atp)) == NULL) goto erret;
         func = NULL;
         if (atp == ID0SERVER_BACK_init){
                 choice = ID0serverBack_init;
                       AsnReadVal(aip,atp,&av);
                       vnp -> data.ptrvalue = av.ptrvalue;
         }
                else
         if (atp == ID0SERVER_BACK_error){
                 choice = ID0serverBack_error;
                       AsnReadVal(aip,atp,&av);
                       vnp -> data.ptrvalue = av.ptrvalue;
         }
                else
         if (atp == ID0SERVER_BACK_gotgi){
                 choice = ID0serverBack_gotgi;
                       AsnReadVal(aip,atp,&av);
                       vnp -> data.ptrvalue = av.ptrvalue;
         }
                else
         if (atp == ID0SERVER_BACK_gotseqentry){
                 choice = ID0serverBack_gotseqentry;
                       func = (AsnReadFunc) SeqEntryAsnRead;
         }
                else
         if (atp == ID0SERVER_BACK_gotdeadseqentry){
                 choice = ID0serverBack_gotdeadseqentry;
                       func = (AsnReadFunc) SeqEntryAsnRead;
         }
                else
         if (atp == ID0SERVER_BACK_fini){
                 choice = ID0serverBack_fini;
                       AsnReadVal(aip,atp,&av);
                       vnp -> data.ptrvalue = av.ptrvalue;
         }
            else
                  goto erret;
           vnp -> choice = choice;
                if (func != NULL)
                        vnp -> data.ptrvalue = (*func) (aip,atp);
ret:
    AsnUnlinkType(orig);
  return vnp;
erret:
   vnp=ID0serverBackFree(vnp);
      goto ret;
}


/**************************************************
*
*    ID0serverRequestAsnWrite()
*
**************************************************/
Boolean ID0serverRequestAsnWrite (ID0serverRequestPtr ptr,  AsnIoPtr aip, AsnTypePtr orig)
{
        Boolean retval = FALSE;
       DataVal av;
   AsnTypePtr atp;
       AsnWriteFunc func;
        AsnTypePtr writetype = NULL;
        Pointer pnt;

       if (aip == NULL) return FALSE;


	if ( ! loaded){
		if ( ! id0gen_AsnLoad ())
			goto erret;
		amp = AsnAllModPtr();
		loaded = TRUE;
	}
 atp = AsnLinkType(orig, ID0SERVER_REQUEST);
	if(atp == NULL) {AsnNullValueMsg(aip,atp); goto ret;}
        av.ptrvalue = (Pointer) ptr;
        if (! AsnWriteChoice(aip, atp, ptr -> choice, & av)) goto ret;
              pnt = ptr -> data.ptrvalue;
               av.intvalue = ptr -> data.intvalue;
        switch (ptr -> choice){
            case  ID0serverRequest_init :
            retval = AsnWrite(aip, ID0SERVER_REQUEST_init, &av); break;
            case  ID0serverRequest_getgi :
        writetype = ID0SERVER_REQUEST_getgi;
             func = (AsnWriteFunc) SeqIdAsnWrite;
                    break;
            case  ID0serverRequest_getsefromgi :
        writetype = ID0SERVER_REQUEST_getsefromgi;
             func = (AsnWriteFunc) ID0serverMaxcomplexAsnWrite;
                    break;
            case  ID0serverRequest_fini :
            retval = AsnWrite(aip, ID0SERVER_REQUEST_fini, &av); break;
        }
        if (writetype != NULL)
                       retval = (*func) (pnt, aip,writetype);
ret:
erret:
    AsnUnlinkType(orig);
  return retval;
}


/**************************************************
*
*    ID0serverMaxcomplexAsnWrite()
*
**************************************************/
Boolean ID0serverMaxcomplexAsnWrite (ID0serverMaxcomplexPtr ptr,  AsnIoPtr aip, AsnTypePtr orig)
{
        Boolean retval = FALSE;
       DataVal av;
   AsnTypePtr atp;

       if (aip == NULL) return FALSE;


	if ( ! loaded){
		if ( ! id0gen_AsnLoad ())
			goto erret;
		amp = AsnAllModPtr();
		loaded = TRUE;
	}
 atp = AsnLinkType(orig, ID0SERVER_MAXCOMPLEX);
	if(atp == NULL) {AsnNullValueMsg(aip,atp); goto ret;}
       if (! AsnOpenStruct (aip, atp, (Pointer) ptr)) goto ret;

          av.intvalue = (Int4) ptr -> maxplex;
      if ( ! AsnWrite(aip,ID0SERVER_MAXCOMPLEX_maxplex, &av)) goto ret;
        av.intvalue = (Int4) ptr -> gi;
            if ( ! AsnWrite(aip,ID0SERVER_MAXCOMPLEX_gi, &av)) goto ret;
       if (! AsnCloseStruct (aip, atp, (Pointer) ptr)) goto ret;
     retval = TRUE;
ret:
erret:
	AsnUnlinkType(orig);
	return retval;
}


/**************************************************
*
*    ID0serverBackAsnWrite()
*
**************************************************/
Boolean ID0serverBackAsnWrite (ID0serverBackPtr ptr,  AsnIoPtr aip, AsnTypePtr orig)
{
        Boolean retval = FALSE;
       DataVal av;
   AsnTypePtr atp;
       AsnWriteFunc func;
        AsnTypePtr writetype = NULL;
        Pointer pnt;

       if (aip == NULL) return FALSE;


	if ( ! loaded){
		if ( ! id0gen_AsnLoad ())
			goto erret;
		amp = AsnAllModPtr();
		loaded = TRUE;
	}
 atp = AsnLinkType(orig, ID0SERVER_BACK);
	if(atp == NULL) {AsnNullValueMsg(aip,atp); goto ret;}
        av.ptrvalue = (Pointer) ptr;
        if (! AsnWriteChoice(aip, atp, ptr -> choice, & av)) goto ret;
              pnt = ptr -> data.ptrvalue;
               av.intvalue = ptr -> data.intvalue;
        switch (ptr -> choice){
            case  ID0serverBack_init :
            retval = AsnWrite(aip, ID0SERVER_BACK_init, &av); break;
            case  ID0serverBack_error :
            retval = AsnWrite(aip, ID0SERVER_BACK_error, &av); break;
            case  ID0serverBack_gotgi :
            retval = AsnWrite(aip, ID0SERVER_BACK_gotgi, &av); break;
            case  ID0serverBack_gotseqentry :
        writetype = ID0SERVER_BACK_gotseqentry;
             func = (AsnWriteFunc) SeqEntryAsnWrite;
                    break;
            case  ID0serverBack_gotdeadseqentry :
        writetype = ID0SERVER_BACK_gotdeadseqentry;
             func = (AsnWriteFunc) SeqEntryAsnWrite;
                    break;
            case  ID0serverBack_fini :
            retval = AsnWrite(aip, ID0SERVER_BACK_fini, &av); break;
        }
        if (writetype != NULL)
                       retval = (*func) (pnt, aip,writetype);
ret:
erret:
    AsnUnlinkType(orig);
  return retval;
}
