#ifndef _NCBI_ObjPrt_
#define _NCBI_ObjPrt_

#ifndef _ASNTOOL_
#include <asn.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
extern Boolean LIBCALL ObjPrtAsnLoad PROTO((void));


/**************************************************
*
*    PrintForm
*
**************************************************/
typedef ValNode  PrintForm;
typedef ValNodePtr PrintFormPtr;
#define PrintForm_block 1 	/*  	PrintFormBlock  	*/
#define PrintForm_boolean 2 	/*  	PrintFormBoolean  	*/
#define PrintForm_enum 3 	/*  	PrintFormEnum  	*/
#define PrintForm_text 4 	/*  	PrintFormText  	*/
#define PrintForm_use_template 5 	/*  	TemplateName  	*/
#define PrintForm_user 6 	/*  	UserFormat  	*/
#define PrintForm_null 7 	/*  	NULL  	*/


PrintFormPtr LIBCALL PrintFormFree PROTO ((PrintFormPtr ));
PrintFormPtr LIBCALL PrintFormAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL PrintFormAsnWrite PROTO (( PrintFormPtr , AsnIoPtr, AsnTypePtr));

/**************************************************
*
*    PrintFormat
*
**************************************************/
typedef struct struct_PrintFormat {
	 struct struct_PrintFormat PNTR next;
	CharPtr   asn1;
	CharPtr   label;
	CharPtr   prefix;
	CharPtr   suffix;
	PrintFormPtr   form;
} PrintFormat, PNTR PrintFormatPtr;


PrintFormatPtr LIBCALL PrintFormatFree PROTO ((PrintFormatPtr ));
PrintFormatPtr LIBCALL PrintFormatNew PROTO (( void ));
PrintFormatPtr LIBCALL PrintFormatAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL PrintFormatAsnWrite PROTO (( PrintFormatPtr , AsnIoPtr, AsnTypePtr));

/**************************************************
*
*    PrintTemplate
*
**************************************************/
typedef struct struct_PrintTemplate {
	CharPtr   name;
	CharPtr   labelfrom;
	PrintFormatPtr   format;
} PrintTemplate, PNTR PrintTemplatePtr;


PrintTemplatePtr LIBCALL PrintTemplateFree PROTO ((PrintTemplatePtr ));
PrintTemplatePtr LIBCALL PrintTemplateNew PROTO (( void ));
PrintTemplatePtr LIBCALL PrintTemplateAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL PrintTemplateAsnWrite PROTO (( PrintTemplatePtr , AsnIoPtr, AsnTypePtr));
PrintTemplatePtr PNTR LIBCALL PrintTemplateInMem PROTO((Int2Ptr numptr));
void LIBCALL PrintTemplateFreeAll PROTO((void));

/**************************************************
*
*    PrintTemplateSet
*   	Just reads a set into the PTlist
*
**************************************************/
Boolean LIBCALL PrintTemplateSetAsnRead PROTO (( AsnIoPtr ));

/**************************************************
*
*    PrintFormBlock
*
**************************************************/
typedef struct struct_PrintFormBlock {
	CharPtr   separator;
	PrintFormatPtr   components;
} PrintFormBlock, PNTR PrintFormBlockPtr;

PrintFormBlockPtr LIBCALL PrintFormBlockFree PROTO ((PrintFormBlockPtr ));
PrintFormBlockPtr LIBCALL PrintFormBlockNew PROTO (( void ));
PrintFormBlockPtr LIBCALL PrintFormBlockAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL PrintFormBlockAsnWrite PROTO (( PrintFormBlockPtr , AsnIoPtr, AsnTypePtr));

/**************************************************
*
*    PrintFormBoolean
*
**************************************************/
typedef struct struct_PrintFormBoolean {
	CharPtr   true;
	CharPtr   false;
} PrintFormBoolean, PNTR PrintFormBooleanPtr;

PrintFormBooleanPtr LIBCALL PrintFormBooleanFree PROTO ((PrintFormBooleanPtr ));
PrintFormBooleanPtr LIBCALL PrintFormBooleanNew PROTO (( void ));
PrintFormBooleanPtr LIBCALL PrintFormBooleanAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL PrintFormBooleanAsnWrite PROTO (( PrintFormBooleanPtr , AsnIoPtr, AsnTypePtr));

/**************************************************
*
*    PrintFormEnum
*
**************************************************/
typedef struct struct_PrintFormEnum {
	ValNodePtr values;
} PrintFormEnum, PNTR PrintFormEnumPtr;

PrintFormEnumPtr LIBCALL PrintFormEnumFree PROTO ((PrintFormEnumPtr ));
PrintFormEnumPtr LIBCALL PrintFormEnumNew PROTO (( void ));
PrintFormEnumPtr LIBCALL PrintFormEnumAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL PrintFormEnumAsnWrite PROTO (( PrintFormEnumPtr , AsnIoPtr, AsnTypePtr));

/**************************************************
*
*    PrintFormText
*
**************************************************/
typedef struct struct_PrintFormText {
	CharPtr   textfunc;
} PrintFormText, PNTR PrintFormTextPtr;

PrintFormTextPtr LIBCALL PrintFormTextFree PROTO ((PrintFormTextPtr ));
PrintFormTextPtr LIBCALL PrintFormTextNew PROTO (( void ));
PrintFormTextPtr LIBCALL PrintFormTextAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL PrintFormTextAsnWrite PROTO (( PrintFormTextPtr , AsnIoPtr, AsnTypePtr));

/**************************************************
*
*    UserFormat
*
**************************************************/
typedef struct struct_UserFormat {
	CharPtr   printfunc;
	CharPtr   defaultfunc;
} UserFormat, PNTR UserFormatPtr;

UserFormatPtr LIBCALL UserFormatFree PROTO ((UserFormatPtr ));
UserFormatPtr LIBCALL UserFormatNew PROTO (( void ));
UserFormatPtr LIBCALL UserFormatAsnRead PROTO (( AsnIoPtr, AsnTypePtr));
Boolean LIBCALL UserFormatAsnWrite PROTO (( UserFormatPtr , AsnIoPtr, AsnTypePtr));

#ifdef __cplusplus
}
#endif

#endif

