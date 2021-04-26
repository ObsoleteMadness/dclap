/* Dvibrant.h
	 see the NCBI Tools vibrant/ section headers for description of routines and usage.
	 modified for use w/ DCLAP library, dgg
*/


#ifndef _VIBRANT_
#define _VIBRANT_

#include <dgg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
*  Familiarity with the NCBI core tools, and the platform concept,
*  are assumed.
*/


#ifndef _VIBTYPES_
#include <vibtypes.h>
#endif

#ifndef _VIBPROCS_
#include <vibprocs.h>

/* this is from <ncbiport.h> -- Nlm_fontrec definition, renamed to Dgg_fontrec to avoid clashes w/ real one*/
/* The font structure is created using a handle.  The FntPtr is
*  returned by a call to Nlm_HandLock (font).  Be sure to call
*  Nlm_HandUnlock (font) after obtaining the font handle.
*/

typedef  struct  Dgg_fontrec {
  Nlm_FonT      next;
  Nlm_Char      name [32];
  Nlm_Int2      size;
  Nlm_Int2      scale;
  Nlm_Boolean   bld;
  Nlm_Boolean   ital;
  Nlm_Boolean   undlin;
  Nlm_Int2      number;
  Nlm_Int2      style;
  Nlm_Handle    handle;
  Nlm_FonT      print;
} Dgg_FontRec, Dgg_FontData, PNTR Dgg_FntPtr;


	/* some additional methods from vibrant needed by DCLAP */
extern void Nlm_GetChoiceTitle  PROTO((Nlm_GraphiC c, Nlm_Int2 item,
                                Nlm_CharPtr title, Nlm_sizeT maxsize));

extern void Nlm_SetDialogTextFont PROTO((Nlm_FonT theFont));

extern void Nlm_SelectBackColor PROTO((Nlm_Uint1 red, Nlm_Uint1 green, Nlm_Uint1 blue));
extern void Nlm_TextOpaque PROTO(());
extern void Nlm_TextTransparent PROTO(());
extern void Nlm_PenPattern PROTO((void *pat));
extern Nlm_FonT  Nlm_fontInUse;

/* extern Nlm_Boolean Nlm_SetSlateBorder PROTO((Nlm_SlatE s, Boolean turnon)); */

	/* from vibincld.h */
extern Nlm_GphPrcsPtr Nlm_GetClassPtr PROTO((Nlm_GraphiC a));
extern void Nlm_SetRect PROTO((Nlm_GraphiC a, Nlm_RectPtr r));

extern Nlm_Handle Nlm_Parent PROTO((Nlm_Handle a));
extern void Nlm_SetChild PROTO((Nlm_GraphiC a, Nlm_GraphiC chld));
extern Nlm_GraphiC Nlm_GetChild PROTO((Nlm_GraphiC a));
extern void Nlm_SetParent  PROTO((Nlm_GraphiC a, Nlm_GraphiC prnt));

#endif  /* _VIBPROCS_ */


		/* multiline text -- Nlm_DrawText only handles one line */
extern void Dgg_DrawTextbox  PROTO((Nlm_RectPtr r, Nlm_CharPtr text,
                          	 ulong len, Nlm_Char jst, Nlm_Boolean gray));

extern void Dgg_PenNormal PROTO(()); /* nlmdraw addition */

			/* draw Mac PICT data */
extern void Dgg_DrawPICTure  PROTO((Nlm_RectPtr r, Nlm_VoidPtr pict));
extern Nlm_VoidPtr Dgg_IsPICT PROTO(( Nlm_VoidPtr pictdata, ulong datasize,
																	 Nlm_Boolean dontTestForHeader));

extern void Dgg_SetParentWindow PROTO((Nlm_GraphiC a, Nlm_WindoW newwindow));

		/* launch an app with a command line (mac,unix,vms,mswin) */
		/* to launch document (mac) call w/ appName == docname and rest == NULL */
	
extern Nlm_Boolean Dgg_LaunchApp PROTO(( char* appName, char* commandline,
																																	char* Stdin, char* Stdout, char* Stderr));

extern const char*  MacDirIDtoName PROTO(( short vRefNum, long dirID, char* fname));
extern short HandleFileEvent PROTO(( long inEvent, long outReply, long inNumber,
											           					   long fileFunc));

/* dgg -- leave out vibdefns, which are just the Nlm_ less #defines
#ifndef _VIBDEFNS_
#include <vibdefns.h>
#endif
*/

		/* vibrant's newer docpanel type */
#ifndef _DOCUMENT_
#include <document.h>
#endif

	/* remove proc defs made in document.h */
#undef DoC 
#undef DocPrntProc 
#undef DocClckProc 
#undef DocDrawProc 
#undef DocShadeProc 
#undef DocDataProc 
#undef DocPanProc 
#undef DocFreeProc 
#undef DocPutProc 
#undef DocGetProc 
#undef DocUpdProc 
#undef ColPtr 
#undef ColData 
#undef ParData 
#undef ParPtr 
#undef DocumentPanel 
#undef SetDocProcs 
#undef SetDocShade 
#undef SetDocCache 
#undef SetDocData 
#undef GetDocData 
#undef GetDocText 
#undef MapDocPoint 
#undef PrintDocument 
#undef SaveDocument 
#undef GetDocParams 
#undef GetItemParams 
#undef GetColParams 
#undef AppendItem 
#undef AppendText 
#undef ReplaceItem 
#undef ReplaceText 
#undef InsertItem 
#undef InsertText 
#undef DeleteItem 
#undef ItemIsVisible 
#undef GetScrlParams 
#undef UpdateDocument 
#undef AdjustDocScroll 
#undef SetDocAutoAdjust 
#undef DisplayFile 
#undef DisplayFancy 

#undef SetDocTabstops  

#ifdef __cplusplus
}
#endif

#endif
