#include <objprt.h>
#include <asnprt.h>

static Boolean loaded = FALSE;

static PrintTemplatePtr PNTR PTlist = NULL;   /* loaded Templates  */
static Int2 PTnum = 0;              /* sizeof PTlist */

Boolean LIBCALL ObjPrtAsnLoad(void)
{

	if ( ! loaded) {

		if ( ! AsnLoad ())
			return FALSE;
		loaded = TRUE;
	}

	return TRUE;
}

/**************************************************
*
*    PrintTemplateNew()
*
**************************************************/

PrintTemplatePtr LIBCALL PrintTemplateNew()
{
	PrintTemplatePtr ptp;
	PrintTemplatePtr PNTR tmp;
	Int2 i;

	ptp = (PrintTemplatePtr) MemNew(sizeof(PrintTemplate));
	if (ptp == NULL) return ptp;

    for (i = 0; i < PTnum; i++)    /* add to Template list */
    {
        if (PTlist[i] == NULL)
        {
            PTlist[i] = ptp;
            return ptp;
        }
    }
    tmp = PTlist;                 /* expand list as necessary */
    PTlist = MemNew((sizeof(PrintTemplatePtr) * (PTnum + 20)));
	if (PTlist == NULL)
	{
		PTlist = tmp;
		return (PrintTemplatePtr)MemFree(ptp);
	}

    MemCopy(PTlist, tmp, (size_t)(sizeof(PrintTemplatePtr) * PTnum));
    MemFree(tmp);
    PTlist[PTnum] = ptp;
    PTnum += 20;
	return ptp;
}

/**************************************************
*
*    PrintTemplateFree()
*
**************************************************/
PrintTemplatePtr LIBCALL PrintTemplateFree ( PrintTemplatePtr ptr)
{
	Int2 i;

	if (ptr == NULL) return NULL;

	MemFree( ptr ->   name);
	MemFree(ptr -> labelfrom);
	PrintFormatFree( ptr ->   format);
    for (i = 0; i < PTnum; i++)
    {
        if (PTlist[i] == ptr)
        {
            PTlist[i] = NULL;
            return MemFree(ptr);
        }
    }
    ErrPost(CTX_NCBIOBJ, 1, "PrintTemplateFree: pointer not in PTlist");
	return MemFree(ptr);
}

/*****************************************************************************
*
*   PrintTemplateFreeAll()
*   	frees all PrintTemplates in memory
*
*****************************************************************************/
void LIBCALL PrintTemplateFreeAll(void)
{
    Int2 i;

    for (i = 0; i < PTnum; i++)
    {
		PrintTemplateFree(PTlist[i]);
    }
}

/*****************************************************************************
*
*   PrintTemplateInMem(&ptnum)
*
*****************************************************************************/
PrintTemplatePtr PNTR LIBCALL PrintTemplateInMem (Int2Ptr numptr)

{
	*numptr = PTnum;
	if (! PTnum)
		return NULL;
	else
		return PTlist;
}

/**************************************************
*
*    PrintTemplateAsnRead()
*
**************************************************/
PrintTemplatePtr LIBCALL PrintTemplateAsnRead ( AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
	PrintTemplatePtr ptr;

	if (aip == NULL) return NULL;


	if ( ! loaded){
		if ( ! ObjPrtAsnLoad ())
			goto erret;
		
	}
	if (orig == NULL)
		atp = AsnReadId(aip, amp, PRINTTEMPLATE);
	else
		atp = AsnLinkType(orig, PRINTTEMPLATE);
	if(atp == NULL) return NULL;

	ptr = PrintTemplateNew();
	if (ptr == NULL) goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;

	atp = AsnReadId(aip,amp, atp);
	if (atp == PRINTTEMPLATE_name){
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		ptr->name = (CharPtr)av.ptrvalue;
		atp = AsnReadId(aip,amp, atp);
	 }
	if (atp == PRINTTEMPLATE_labelfrom){
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		ptr->labelfrom = (CharPtr)av.ptrvalue;
		atp = AsnReadId(aip,amp, atp);
	}
	if (atp == PRINTTEMPLATE_format){
		ptr -> format =PrintFormatAsnRead(aip, atp);
		if (ptr -> format == NULL) goto erret;
		atp = AsnReadId(aip,amp, atp);
	 }
	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
ret:
	AsnUnlinkType(orig);
	return ptr;
erret:
	ptr=PrintTemplateFree(ptr);
	goto ret;
}

/**************************************************
*
*    PrintTemplateAsnWrite()
*
**************************************************/
Boolean LIBCALL PrintTemplateAsnWrite (PrintTemplatePtr ptr,  AsnIoPtr aip, AsnTypePtr orig)
{
	Boolean retval = FALSE;
	DataVal av;
	AsnTypePtr atp;

	if (aip == NULL) return FALSE;


	if ( ! loaded){
	if (! ObjPrtAsnLoad()) goto erret;
	
}
	atp = AsnLinkType(orig, PRINTTEMPLATE);
	if(atp == NULL) {AsnNullValueMsg(aip,atp); goto erret;}
	if (! AsnOpenStruct (aip, atp, (Pointer) ptr)) goto erret;

	if (ptr ->  name != NULL){
		av.ptrvalue = (Pointer) ptr -> name;
		if ( ! AsnWrite(aip,PRINTTEMPLATE_name, &av)) goto erret;
	}
	if (ptr -> labelfrom != NULL){
		av.ptrvalue = (Pointer) ptr -> labelfrom;
		if ( ! AsnWrite(aip,PRINTTEMPLATE_labelfrom, &av)) goto erret;
	}
	if (ptr ->  format != NULL){
		if ( ! PrintFormatAsnWrite(ptr -> format, aip, PRINTTEMPLATE_format))
			goto erret;
	}
	if (! AsnCloseStruct (aip, atp, (Pointer) ptr)) goto erret;
	retval = TRUE;
erret:AsnUnlinkType(orig);
	return retval;
}

/**************************************************
*
*    PrintTemplateSetAsnRead()
*
**************************************************/
Boolean LIBCALL PrintTemplateSetAsnRead ( AsnIoPtr aip )
{
	DataVal av;
	AsnTypePtr atp, oldatp;
	PrintTemplatePtr ptr;

	if (aip == NULL) return FALSE;

	if ( ! loaded){
		if ( ! ObjPrtAsnLoad ())
			goto erret;
		
	}

	atp = AsnReadId(aip, amp, PRINTTEMPLATESET);
	if(atp == NULL) return FALSE;
	oldatp = atp;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;

	while ((atp = AsnReadId(aip,amp, atp)) != oldatp)
	{
		ptr = PrintTemplateAsnRead(aip, atp);
		if (ptr == NULL) goto erret;
	}
	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;

	return TRUE;
erret:
	return FALSE;
}

/**************************************************
*
*    PrintFormatNew()
*
**************************************************/

PrintFormatPtr LIBCALL PrintFormatNew()
{

	return (PrintFormatPtr) MemNew(sizeof(PrintFormat));

}

/**************************************************
*
*    PrintFormatFree()
*
**************************************************/
PrintFormatPtr LIBCALL PrintFormatFree ( PrintFormatPtr head_ptr)
{
	 PrintFormatPtr ptr, hold_ptr;

	if (head_ptr == NULL) return NULL;

 	for (ptr = head_ptr; ptr; ptr = hold_ptr){
		hold_ptr = ptr -> next;
		MemFree(ptr -> asn1);
		MemFree(ptr -> label);
		MemFree(ptr -> prefix);
		MemFree(ptr -> suffix);
		PrintFormFree( ptr ->   form);
		MemFree(ptr);
	 }
	return NULL;
}

/**************************************************
*
*    PrintFormatAsnRead()
*
**************************************************/
PrintFormatPtr LIBCALL PrintFormatAsnRead ( AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
	PrintFormatPtr ptr;

	if (aip == NULL) return NULL;


	if ( ! loaded){
		if ( ! ObjPrtAsnLoad ())
			goto erret;
		
	}
	if (orig == NULL)
		atp = AsnReadId(aip, amp, PRINTFORMAT);
	else
		atp = AsnLinkType(orig, PRINTFORMAT);
	if(atp == NULL) return NULL;

	ptr = PrintFormatNew();
	if (ptr == NULL) goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;

	atp = AsnReadId(aip,amp, atp);
	if (atp == PRINTFORMAT_asn1){
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		ptr->asn1 = (CharPtr)av.ptrvalue;
		atp = AsnReadId(aip,amp, atp);
	}
	if (atp == PRINTFORMAT_label){
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		ptr->label = (CharPtr)av.ptrvalue;
		atp = AsnReadId(aip,amp, atp);
	}
	if (atp == PRINTFORMAT_prefix){
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		ptr->prefix = (CharPtr)av.ptrvalue;
		atp = AsnReadId(aip,amp, atp);
	}
	if (atp == PRINTFORMAT_suffix){
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		ptr->suffix = (CharPtr)av.ptrvalue;
		atp = AsnReadId(aip,amp, atp);
	}
	if (atp == PRINTFORMAT_form){
		ptr -> form =PrintFormAsnRead(aip, atp);
		if (ptr -> form == NULL) goto erret;
		atp = AsnReadId(aip,amp, atp);
	 }
	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
ret:
	AsnUnlinkType(orig);
	return ptr;
erret:
	ptr=PrintFormatFree(ptr);
	goto ret;
}

/**************************************************
*
*    PrintFormatAsnWrite()
*
**************************************************/
Boolean LIBCALL PrintFormatAsnWrite (PrintFormatPtr ptr,  AsnIoPtr aip, AsnTypePtr orig)
{
	Boolean retval = FALSE;
	DataVal av;
	AsnTypePtr atp;

	if (aip == NULL) return FALSE;


	if ( ! loaded){
	if (! ObjPrtAsnLoad()) goto erret;
	
}
	atp = AsnLinkType(orig, PRINTFORMAT);
	if(atp == NULL) {AsnNullValueMsg(aip,atp); goto erret;}
	if (! AsnOpenStruct (aip, atp, (Pointer) ptr)) goto erret;

	if (ptr -> asn1 != NULL){
		av.ptrvalue = (Pointer) ptr -> asn1;
		if ( ! AsnWrite(aip,PRINTFORMAT_asn1, &av)) goto erret;
	}
	if (ptr -> label != NULL){
		av.ptrvalue = (Pointer) ptr -> label;
		if ( ! AsnWrite(aip,PRINTFORMAT_label, &av)) goto erret;
	}
	if (ptr -> prefix != NULL){
		av.ptrvalue = (Pointer) ptr -> prefix;
		if ( ! AsnWrite(aip,PRINTFORMAT_prefix, &av)) goto erret;
	}
	if (ptr -> suffix != NULL){
		av.ptrvalue = (Pointer) ptr -> suffix;
		if ( ! AsnWrite(aip,PRINTFORMAT_suffix, &av)) goto erret;
	}
	if (ptr ->  form != NULL){
		if ( ! PrintFormAsnWrite(ptr -> form, aip, PRINTFORMAT_form))
			goto erret;
	}
	if (! AsnCloseStruct (aip, atp, (Pointer) ptr)) goto erret;
	retval = TRUE;
erret:AsnUnlinkType(orig);
	return retval;
}

/**************************************************
*
*    PrintFormFree()
*
**************************************************/
PrintFormPtr LIBCALL PrintFormFree ( PrintFormPtr ptr)
{
	if (ptr == NULL) return NULL;

	{Pointer pt = ptr -> data.ptrvalue;
	switch (ptr ->  choice){
		case  PrintForm_block :
			PrintFormBlockFree(pt);
			break;
		case  PrintForm_boolean :
			PrintFormBooleanFree(pt);
			break;
		case  PrintForm_enum :
			PrintFormEnumFree(pt);
			break;
		case  PrintForm_text :
			PrintFormTextFree(pt);
			break;
		case  PrintForm_use_template :
			MemFree(pt);
			break;
		case  PrintForm_user :
			UserFormatFree(pt);
			break;
	}}
	MemFree(ptr);
	return NULL;
}

/**************************************************
*
*    PrintFormAsnRead()
*
**************************************************/
PrintFormPtr LIBCALL PrintFormAsnRead ( AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
	ValNodePtr vnp;
	Uint1 choice ;
	AsnReadFunc func;

	if (aip == NULL) return NULL;


	if ( ! loaded){
		if ( ! ObjPrtAsnLoad ())
			goto erret;
		
	}
	if (orig == NULL)
		atp = AsnReadId(aip, amp, PRINTFORM);
	else
		atp = AsnLinkType(orig, PRINTFORM);
	if(atp == NULL) return NULL;

 	vnp = ValNodeNew(NULL);
 	if (vnp == NULL) goto erret;
 	if ( AsnReadVal(aip, atp, &av) <= 0) goto erret;
 	if ( (atp = AsnReadId(aip, amp, atp)) == NULL) goto erret;
 	func = NULL;
		if (atp == PRINTFORM_block){
			choice = PrintForm_block;
			func = (AsnReadFunc) PrintFormBlockAsnRead;
		}
	  		else
		if (atp == PRINTFORM_boolean){
			choice = PrintForm_boolean;
			func = (AsnReadFunc) PrintFormBooleanAsnRead;
		}
	  		else
		if (atp == PRINTFORM_enum){
			choice = PrintForm_enum;
			func = (AsnReadFunc) PrintFormEnumAsnRead;
		}
	  		else
		if (atp == PRINTFORM_text){
			choice = PrintForm_text;
			func = (AsnReadFunc) PrintFormTextAsnRead;
		}
	  		else
		if (atp == PRINTFORM_use_template){
			choice = PrintForm_use_template;
			AsnReadVal(aip,atp,&av);
			vnp -> data.ptrvalue = av.ptrvalue;
		}
	  		else
		if (atp == PRINTFORM_user){
			choice = PrintForm_user;
			func = (AsnReadFunc) UserFormatAsnRead;
		}
	  		else
		if (atp == PRINTFORM_null){
			choice = PrintForm_null;
			AsnReadVal(aip,atp,&av);
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
	vnp = PrintFormFree(vnp);
	goto ret;
}

/**************************************************
*
*    PrintFormAsnWrite()
*
**************************************************/
Boolean LIBCALL PrintFormAsnWrite (PrintFormPtr ptr,  AsnIoPtr aip, AsnTypePtr orig)
{
	Boolean retval = FALSE;
	DataVal av;
	AsnTypePtr atp;
	AsnWriteFunc func;
	AsnTypePtr writetype = NULL;
	Pointer pnt;

	if (aip == NULL) return FALSE;


	if ( ! loaded){
	if (! ObjPrtAsnLoad()) goto erret;
	
}
	atp = AsnLinkType(orig, PRINTFORM);
	if(atp == NULL) {AsnNullValueMsg(aip,atp); goto erret;}
 	av.ptrvalue = (Pointer) ptr;
 	if (! AsnWriteChoice(aip, atp, ptr -> choice, & av)) goto erret;
	pnt = ptr -> data.ptrvalue;
	av.intvalue = ptr -> data.intvalue;
 	switch (ptr -> choice)
	{
		case  PrintForm_block :
			writetype = PRINTFORM_block;
			func = (AsnWriteFunc) PrintFormBlockAsnWrite;
			break;
		case  PrintForm_boolean :
			writetype = PRINTFORM_boolean;
			func = (AsnWriteFunc) PrintFormBooleanAsnWrite;
			break;
		case  PrintForm_enum :
			writetype = PRINTFORM_enum;
			func = (AsnWriteFunc) PrintFormEnumAsnWrite;
			break;
		case  PrintForm_text :
			writetype = PRINTFORM_text;
			func = (AsnWriteFunc) PrintFormTextAsnWrite;
			break;
		case  PrintForm_use_template :
			av.ptrvalue = ptr->data.ptrvalue;
			retval = AsnWrite(aip, PRINTFORM_use_template, &av);
			break;
		case  PrintForm_user :
			writetype = PRINTFORM_user;
			func = (AsnWriteFunc) UserFormatAsnWrite;
			break;
		case  PrintForm_null :
			retval = AsnWrite(aip, PRINTFORM_null, &av); break;
	}
 	if (writetype != NULL)
 			retval = (*func) (pnt, aip,writetype);
ret:AsnUnlinkType(orig);
	return retval;
erret:
	retval = FALSE;
	goto ret;
}

/**************************************************
*
*    PrintFormBlockNew()
*
**************************************************/

PrintFormBlockPtr LIBCALL PrintFormBlockNew()
{
	return (PrintFormBlockPtr) MemNew(sizeof(PrintFormBlock));
}

/**************************************************
*
*    PrintFormBlockFree()
*
**************************************************/
PrintFormBlockPtr LIBCALL PrintFormBlockFree ( PrintFormBlockPtr ptr)
{
	if (ptr == NULL) return NULL;

	MemFree(ptr -> separator);
	PrintFormatFree(  ptr -> components);
	MemFree(ptr);
	return NULL;
}

/**************************************************
*
*    PrintFormBlockAsnRead()
*
**************************************************/
PrintFormBlockPtr LIBCALL PrintFormBlockAsnRead ( AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
	PrintFormBlockPtr ptr;

	if (aip == NULL) return NULL;


	if ( ! loaded){
		if ( ! ObjPrtAsnLoad ())
			goto erret;
		
	}
	if (orig == NULL)
		atp = AsnReadId(aip, amp, PRINTFORMBLOCK);
	else
		atp = AsnLinkType(orig, PRINTFORMBLOCK);
	if(atp == NULL) return NULL;

	ptr = PrintFormBlockNew();
	if (ptr == NULL) goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;

	atp = AsnReadId(aip,amp, atp);
	if (atp == PRINTFORMBLOCK_separator){
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		ptr->separator = (CharPtr)av.ptrvalue;
		atp = AsnReadId(aip,amp, atp);
	}
	if (atp == PRINTFORMBLOCK_components)
	{
		AsnTypePtr now_atp;
		PrintFormatPtr current;
		PrintFormatPtr head = NULL;
		PrintFormatPtr prev = NULL;
		if (AsnReadVal(aip, atp, &av) <= 0) /* START_STUCT */
			goto erret;

		now_atp = atp;

		while ((now_atp = AsnReadId(aip, amp, now_atp)) != atp){
			if (now_atp == NULL)  goto erret;

			current= PrintFormatAsnRead(aip, now_atp);
			if (current == NULL)  goto erret;

			if (head == NULL)
				head = current;
			else
				prev -> next = current;
			prev=current;
		}
		if (AsnReadVal(aip, atp, &av) <=0) /* END_STRUCT */				goto erret;
		ptr -> components = head;
		atp = AsnReadId(aip,amp, atp);
	}
	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
ret:
	AsnUnlinkType(orig);
	return ptr;
erret:
	ptr=PrintFormBlockFree(ptr);
	goto ret;
}

/**************************************************
*
*    PrintFormBlockAsnWrite()
*
**************************************************/
Boolean LIBCALL PrintFormBlockAsnWrite (PrintFormBlockPtr ptr,  AsnIoPtr aip, AsnTypePtr orig)
{
	Boolean retval = FALSE;
	DataVal av;
	AsnTypePtr atp;

	if (aip == NULL) return FALSE;


	if ( ! loaded){
	if (! ObjPrtAsnLoad()) goto erret;
	
}
	atp = AsnLinkType(orig, PRINTFORMBLOCK);
	if(atp == NULL) {AsnNullValueMsg(aip,atp); goto erret;}
	if (! AsnOpenStruct (aip, atp, (Pointer) ptr)) goto erret;

	if (ptr -> separator != NULL){
		av.ptrvalue = (Pointer) ptr -> separator;
		if ( ! AsnWrite(aip,PRINTFORMBLOCK_separator, &av)) goto erret;
	}
	if (ptr -> components != NULL){
	if (! AsnOpenStruct (aip, PRINTFORMBLOCK_components, (Pointer) ptr -> components))
			goto erret;

		{PrintFormatPtr current;
		for(current=ptr -> components; current; current = current -> next){
		if ( ! PrintFormatAsnWrite(current,aip,PRINTFORMBLOCK_components_E)) goto erret;
		}}

	if (! AsnCloseStruct (aip, PRINTFORMBLOCK_components, (Pointer) ptr -> components))
			goto erret;
	}
	if (! AsnCloseStruct (aip, atp, (Pointer) ptr)) goto erret;
	retval = TRUE;
erret:AsnUnlinkType(orig);
	return retval;
}

/**************************************************
*
*    PrintFormBooleanNew()
*
**************************************************/

PrintFormBooleanPtr LIBCALL PrintFormBooleanNew()
{
	return (PrintFormBooleanPtr) MemNew(sizeof(PrintFormBoolean));
}

/**************************************************
*
*    PrintFormBooleanFree()
*
**************************************************/
PrintFormBooleanPtr LIBCALL PrintFormBooleanFree ( PrintFormBooleanPtr ptr)
{
	if (ptr == NULL) return NULL;

	MemFree(ptr -> true);
	MemFree(ptr -> false);
	MemFree(ptr);
	return NULL;
}

/**************************************************
*
*    PrintFormBooleanAsnRead()
*
**************************************************/
PrintFormBooleanPtr LIBCALL PrintFormBooleanAsnRead ( AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
	PrintFormBooleanPtr ptr;

	if (aip == NULL) return NULL;


	if ( ! loaded){
		if ( ! ObjPrtAsnLoad ())
			goto erret;
		
	}
	if (orig == NULL)
		atp = AsnReadId(aip, amp, PRINTFORMBOOLEAN);
	else
		atp = AsnLinkType(orig, PRINTFORMBOOLEAN);
	if(atp == NULL) return NULL;

	ptr = PrintFormBooleanNew();
	if (ptr == NULL) goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;

	atp = AsnReadId(aip,amp, atp);
	if (atp == PRINTFORMBOOLEAN_true){
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		ptr->true = (CharPtr)av.ptrvalue;
		atp = AsnReadId(aip,amp, atp);
	}
	if (atp == PRINTFORMBOOLEAN_false){
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		ptr->false = (CharPtr)av.ptrvalue;
		atp = AsnReadId(aip,amp, atp);
	}
	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
ret:
	AsnUnlinkType(orig);
	return ptr;
erret:
	ptr=PrintFormBooleanFree(ptr);
	goto ret;
}

/**************************************************
*
*    PrintFormBooleanAsnWrite()
*
**************************************************/
Boolean LIBCALL PrintFormBooleanAsnWrite (PrintFormBooleanPtr ptr,  AsnIoPtr aip, AsnTypePtr orig)
{
	Boolean retval = FALSE;
	DataVal av;
	AsnTypePtr atp;

	if (aip == NULL) return FALSE;


	if ( ! loaded){
	if (! ObjPrtAsnLoad()) goto erret;
	
}
	atp = AsnLinkType(orig, PRINTFORMBOOLEAN);
	if(atp == NULL) {AsnNullValueMsg(aip,atp); goto erret;}
	if (! AsnOpenStruct (aip, atp, (Pointer) ptr)) goto erret;

	if (ptr -> true != NULL){
		av.ptrvalue = (Pointer) ptr -> true;
		if ( ! AsnWrite(aip,PRINTFORMBOOLEAN_true, &av)) goto erret;
	}
	if (ptr -> false != NULL){
		av.ptrvalue = (Pointer) ptr -> false;
		if ( ! AsnWrite(aip,PRINTFORMBOOLEAN_false, &av)) goto erret;
	}
	if (! AsnCloseStruct (aip, atp, (Pointer) ptr)) goto erret;
	retval = TRUE;
erret:AsnUnlinkType(orig);
	return retval;
}

/**************************************************
*
*    PrintFormEnumNew()
*
**************************************************/
PrintFormEnumPtr LIBCALL PrintFormEnumNew()
{
	return (PrintFormEnumPtr) MemNew(sizeof(PrintFormEnum));
}

/**************************************************
*
*    PrintFormEnumFree()
*
**************************************************/
PrintFormEnumPtr LIBCALL PrintFormEnumFree ( PrintFormEnumPtr ptr)
{
	if (ptr == NULL) return NULL;

	ValNodeFreeData(ptr -> values);
	MemFree(ptr);
	return NULL;
}

/**************************************************
*
*    PrintFormEnumAsnRead()
*
**************************************************/
PrintFormEnumPtr LIBCALL PrintFormEnumAsnRead ( AsnIoPtr aip, AsnTypePtr orig)
{
	ValNodePtr current;
	DataVal av;
	AsnTypePtr atp, oldatp;
	PrintFormEnumPtr ptr;

	if (aip == NULL) return NULL;

	if ( ! loaded){
		if ( ! ObjPrtAsnLoad ())
			goto erret;
		
	}
	if (orig == NULL)
		atp = AsnReadId(aip, amp, PRINTFORMENUM);
	else
		atp = AsnLinkType(orig, PRINTFORMENUM);
	if(atp == NULL) return NULL;
	oldatp = atp;

	ptr = PrintFormEnumNew();
	if (ptr == NULL) goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;

	while ((atp = AsnReadId(aip,amp, atp)) != oldatp)
	{
		if (AsnReadVal(aip, atp, &av) <= 0) /* the value */
			goto erret;

		if (atp == PRINTFORMENUM_values_E)
		{
			current = ValNodeNew(ptr->values);
			if (current == NULL)  goto erret;
			current->data.ptrvalue = av.ptrvalue;

			if (ptr->values == NULL)
				ptr->values = current;
		}
	}
	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
ret:
	AsnUnlinkType(orig);
	return ptr;
erret:
	ptr=PrintFormEnumFree(ptr);
	goto ret;
}

/**************************************************
*
*    PrintFormEnumAsnWrite()
*
**************************************************/
Boolean LIBCALL PrintFormEnumAsnWrite (PrintFormEnumPtr ptr,  AsnIoPtr aip, AsnTypePtr orig)
{
	Boolean retval = FALSE;
	AsnTypePtr atp;
	ValNodePtr curr;

	if (aip == NULL) return FALSE;


	if ( ! loaded){
	if (! ObjPrtAsnLoad()) goto erret;
	
}
	atp = AsnLinkType(orig, PRINTFORMENUM);
	if(atp == NULL) {AsnNullValueMsg(aip,atp); goto erret;}
	if (! AsnOpenStruct (aip, atp, (Pointer) ptr)) goto erret;

	if (ptr -> values != NULL){
		if (! AsnOpenStruct (aip, PRINTFORMENUM_values, (Pointer) ptr -> values))
			goto erret;
		for (curr = ptr->values; curr != NULL; curr = curr->next)
		{
			if ( ! AsnWrite(aip, PRINTFORMENUM_values_E, &(curr->data))) goto erret;
		}
		if (! AsnCloseStruct (aip, PRINTFORMENUM_values, (Pointer) ptr -> values))
			goto erret;
	}
	if (! AsnCloseStruct (aip, atp, (Pointer) ptr)) goto erret;
	retval = TRUE;
erret:AsnUnlinkType(orig);
	return retval;
}

/**************************************************
*
*    PrintFormTextNew()
*
**************************************************/
PrintFormTextPtr LIBCALL PrintFormTextNew()
{
	return (PrintFormTextPtr) MemNew(sizeof(PrintFormText));
}

/**************************************************
*
*    PrintFormTextFree()
*
**************************************************/
PrintFormTextPtr LIBCALL PrintFormTextFree ( PrintFormTextPtr ptr)
{
	if (ptr == NULL) return NULL;

	MemFree(ptr -> textfunc);
	MemFree(ptr);
	return NULL;
}

/**************************************************
*
*    PrintFormTextAsnRead()
*
**************************************************/
PrintFormTextPtr LIBCALL PrintFormTextAsnRead ( AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
	PrintFormTextPtr ptr;

	if (aip == NULL) return NULL;


	if ( ! loaded){
		if ( ! ObjPrtAsnLoad ())
			goto erret;
		
	}
	if (orig == NULL)
		atp = AsnReadId(aip, amp, PRINTFORMTEXT);
	else
		atp = AsnLinkType(orig, PRINTFORMTEXT);
	if(atp == NULL) return NULL;

	ptr = PrintFormTextNew();
	if (ptr == NULL) goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;

	atp = AsnReadId(aip,amp, atp);
	if (atp == PRINTFORMTEXT_textfunc){
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		ptr->textfunc = (CharPtr)av.ptrvalue;
		atp = AsnReadId(aip,amp, atp);
	}
	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
ret:
	AsnUnlinkType(orig);
	return ptr;
erret:
	ptr=PrintFormTextFree(ptr);
	goto ret;
}

/**************************************************
*
*    PrintFormTextAsnWrite()
*
**************************************************/
Boolean LIBCALL PrintFormTextAsnWrite (PrintFormTextPtr ptr,  AsnIoPtr aip, AsnTypePtr orig)
{
	Boolean retval = FALSE;
	DataVal av;
	AsnTypePtr atp;

	if (aip == NULL) return FALSE;


	if ( ! loaded){
	if (! ObjPrtAsnLoad()) goto erret;
	
}
	atp = AsnLinkType(orig, PRINTFORMTEXT);
	if(atp == NULL) {AsnNullValueMsg(aip,atp); goto erret;}
	if (! AsnOpenStruct (aip, atp, (Pointer) ptr)) goto erret;

	if (ptr -> textfunc != NULL){
		av.ptrvalue = (Pointer) ptr -> textfunc;
		if ( ! AsnWrite(aip,PRINTFORMTEXT_textfunc, &av)) goto erret;
	}
	if (! AsnCloseStruct (aip, atp, (Pointer) ptr)) goto erret;
	retval = TRUE;
erret:AsnUnlinkType(orig);
	return retval;
}

/**************************************************
*
*    UserFormatNew()
*
**************************************************/
UserFormatPtr LIBCALL UserFormatNew()
{
	return (UserFormatPtr) MemNew(sizeof(UserFormat));
}

/**************************************************
*
*    UserFormatFree()
*
**************************************************/
UserFormatPtr LIBCALL UserFormatFree ( UserFormatPtr ptr)
{
	if (ptr == NULL) return NULL;

	MemFree(ptr -> printfunc);
	MemFree(ptr -> defaultfunc);
	MemFree(ptr);
	return NULL;
}

/**************************************************
*
*    UserFormatAsnRead()
*
**************************************************/
UserFormatPtr LIBCALL UserFormatAsnRead ( AsnIoPtr aip, AsnTypePtr orig)
{
	DataVal av;
	AsnTypePtr atp;
	UserFormatPtr ptr;

	if (aip == NULL) return NULL;


	if ( ! loaded){
		if ( ! ObjPrtAsnLoad ())
			goto erret;
		
	}
	if (orig == NULL)
		atp = AsnReadId(aip, amp, USERFORMAT);
	else
		atp = AsnLinkType(orig, USERFORMAT);
	if(atp == NULL) return NULL;

	ptr = UserFormatNew();
	if (ptr == NULL) goto erret;

	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;

	atp = AsnReadId(aip,amp, atp);
	if (atp == USERFORMAT_printfunc){
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		ptr->printfunc = (CharPtr)av.ptrvalue;
		atp = AsnReadId(aip,amp, atp);
	}
	if (atp == USERFORMAT_defaultfunc){
		if (AsnReadVal(aip, atp, &av) <= 0)
			goto erret;
		ptr->defaultfunc = (CharPtr)av.ptrvalue;
		atp = AsnReadId(aip,amp, atp);
	}
	if (AsnReadVal(aip, atp, &av) <= 0) goto erret;
ret:
	AsnUnlinkType(orig);
	return ptr;
erret:
	ptr=UserFormatFree(ptr);
	goto ret;
}

/**************************************************
*
*    UserFormatAsnWrite()
*
**************************************************/
Boolean LIBCALL UserFormatAsnWrite (UserFormatPtr ptr,  AsnIoPtr aip, AsnTypePtr orig)
{
	Boolean retval = FALSE;
	DataVal av;
	AsnTypePtr atp;

	if (aip == NULL) return FALSE;


	if ( ! loaded){
	if (! ObjPrtAsnLoad()) goto erret;
	
}
	atp = AsnLinkType(orig, USERFORMAT);
	if(atp == NULL) {AsnNullValueMsg(aip,atp); goto erret;}
	if (! AsnOpenStruct (aip, atp, (Pointer) ptr)) goto erret;

	if (ptr -> printfunc != NULL){
		av.ptrvalue = (Pointer) ptr -> printfunc;
		if ( ! AsnWrite(aip,USERFORMAT_printfunc, &av)) goto erret;
	}
	if (ptr -> defaultfunc != NULL){
		av.ptrvalue = (Pointer) ptr -> defaultfunc;
		if ( ! AsnWrite(aip,USERFORMAT_defaultfunc, &av)) goto erret;
	}
	if (! AsnCloseStruct (aip, atp, (Pointer) ptr)) goto erret;
	retval = TRUE;
erret:AsnUnlinkType(orig);
	return retval;
}

