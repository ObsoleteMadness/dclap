/*   vibtypes.h
* ===========================================================================
*
*                            PUBLIC DOMAIN NOTICE
*            National Center for Biotechnology Information (NCBI)
*
*  This software/database is a "United States Government Work" under the
*  terms of the United States Copyright Act.  It was written as part of
*  the author's official duties as a United States Government employee and
*  thus cannot be copyrighted.  This software/database is freely available
*  to the public for use. The National Library of Medicine and the U.S.
*  Government do not place any restriction on its use or reproduction.
*  We would, however, appreciate having the NCBI and the author cited in
*  any work or product based on this material
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the NLM and the U.S.
*  Government do not and cannot warrant the performance or results that
*  may be obtained by using this software or data. The NLM and the U.S.
*  Government disclaim all warranties, express or implied, including
*  warranties of performance, merchantability or fitness for any particular
*  purpose.
*
* ===========================================================================
*
* File Name:  vibtypes.h
*
* Author:  Jonathan Kans
*
* Version Creation Date:   7/1/91
*
* $Revision: 2.11 $
*
* File Description: 
*       Vibrant object and data type definitions
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _VIBTYPES_
#define _VIBTYPES_

/***  PLATFORM DEFINITION, STANDARD DATA TYPES AND MACROS  ***/

/*
*  The NCBI defines standard headers for local definition of
*  portable data types and macros.  The ncbi.h header includes
*  ncbilcl.h, which specifies data types for a given platform.
*/

#ifndef _NCBI_
#include <ncbi.h>
#endif
#ifndef _NCBIDRAW_
#include <ncbidraw.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***  INTERFACE PARENT BASE TYPE  ***/

typedef  struct  Nlm_graphic {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_GraphiC;

/***  INTERFACE OBJECT TYPES  ***/

/*
*  Interface variables will be used by programmers to refer
*  to specific objects on the display screen. General
*  purpose procedures will use these variables to locate
*  the data structures necessary to manipulate the objects.
*  The typical programmer will be able to rely on these
*  high-level procedures to manage the user interface, and
*  will not need access to the internal data structures.
*
*  By defining object types as handles to specific dummy
*  structures, compilers can detect attempts to incorrectly
*  use some interface functions (such as trying to insert a
*  button into a menu, instead of a group or window).
*/

typedef  struct  Nlm_bar {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_BaR;

typedef  struct  Nlm_button {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_ButtoN;

typedef  struct  Nlm_choice {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_ChoicE;

typedef  struct  Nlm_display {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_DisplaY;

typedef  struct  Nlm_icon {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_IcoN;

typedef  struct  Nlm_item {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_IteM;

typedef  struct  Nlm_list {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_LisT;

typedef  struct  Nlm_menu {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_MenU;

typedef  struct  Nlm_panel {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_PaneL;

typedef  struct  Nlm_popup {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_PopuP;

typedef  struct  Nlm_prompt {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_PrompT;

typedef  struct  Nlm_repeat {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_RepeaT;

typedef  struct  Nlm_slate {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_SlatE;

typedef  struct  Nlm_switch {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_SwitcH;

typedef  struct  Nlm_text {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_TexT;

typedef  struct  Nlm_window {
  Nlm_VoidPtr  dummy;
} HNDL Nlm_GrouP, HNDL Nlm_WindoW;

/***  ENUMERATED MESSAGE AND INTERFACE CALLBACK PROCEDURE TYPES  ***/

/*
*  Actions by the user (such as mouse clicks or key presses)
*  will result in programmer-specified routines being
*  executed.  The ActnProc (action procedure) takes a GraphiC
*  as its only argument, and is called when the user issues
*  a command (such as clicking on a push button or making a
*  selection from a menu).  Programs are written around these
*  action callbacks.  The GraphiC parameter is the handle to
*  the object that was acted upon by the user.  The ClckProc
*  is used in PaneL objects to allow an instance-specific
*  procedure that responds to click events.  The ScrlProc is
*  used by scroll bars to return the slave, and the new and
*  old scroll positions.
*
*  Because some compilers would allow actual callback functions
*  to have the appropriate type (e.g., WindoW) if the following
*  typedefs used Handle instead of GraphiC, whereas other
*  compilers would flag these uses as errors, we define specific
*  callback types for each class of object that uses ActnProc or
*  ClckProc callbacks.
*/

typedef  void  (*Nlm_VoidProc) PROTO((void));
typedef  void  (*Nlm_KeyProc)  PROTO((Nlm_Char));
typedef  void  (*Nlm_ActnProc) PROTO((Nlm_GraphiC));
typedef  void  (*Nlm_ClckProc) PROTO((Nlm_GraphiC, Nlm_PoinT));
typedef  void  (*Nlm_CharProc) PROTO((Nlm_GraphiC, Nlm_Char));
typedef  void  (*Nlm_ScrlProc) PROTO((Nlm_BaR, Nlm_GraphiC, Nlm_Int2, Nlm_Int2));
typedef  void  (*Nlm_ChngProc) PROTO((Nlm_GraphiC, Nlm_Int2, Nlm_Int2));
typedef  void  (*Nlm_FreeProc) PROTO((Nlm_GraphiC, Nlm_VoidPtr));

typedef  void  (*Nlm_BtnActnProc) PROTO((Nlm_ButtoN));
typedef  void  (*Nlm_ChsActnProc) PROTO((Nlm_ChoicE));
typedef  void  (*Nlm_GrpActnProc) PROTO((Nlm_GrouP));
typedef  void  (*Nlm_IcnActnProc) PROTO((Nlm_IcoN));
typedef  void  (*Nlm_ItmActnProc) PROTO((Nlm_IteM));
typedef  void  (*Nlm_LstActnProc) PROTO((Nlm_LisT));
typedef  void  (*Nlm_PnlActnProc) PROTO((Nlm_PaneL));
typedef  void  (*Nlm_PupActnProc) PROTO((Nlm_PopuP));
typedef  void  (*Nlm_RptActnProc) PROTO((Nlm_RepeaT));
typedef  void  (*Nlm_TxtActnProc) PROTO((Nlm_TexT));
typedef  void  (*Nlm_WndActnProc) PROTO((Nlm_WindoW));
typedef  void  (*Nlm_WndFreeProc) PROTO((Nlm_WindoW, Nlm_VoidPtr));

typedef  void  (*Nlm_PnlClckProc) PROTO((Nlm_PaneL, Nlm_PoinT));
typedef  void  (*Nlm_RptClckProc) PROTO((Nlm_RepeaT, Nlm_PoinT));
typedef  void  (*Nlm_IcnClckProc) PROTO((Nlm_IcoN, Nlm_PoinT));

typedef  void  (*Nlm_BarScrlProc) PROTO((Nlm_BaR, Nlm_GraphiC, Nlm_Int2, Nlm_Int2));
typedef  void  (*Nlm_SltScrlProc) PROTO((Nlm_BaR, Nlm_SlatE, Nlm_Int2, Nlm_Int2));

typedef  void  (*Nlm_IcnChngProc) PROTO((Nlm_IcoN, Nlm_Int2, Nlm_Int2));
typedef  void  (*Nlm_SwtChngProc) PROTO((Nlm_SwitcH, Nlm_Int2, Nlm_Int2));

typedef  void  (*Nlm_SltCharProc) PROTO((Nlm_SlatE, Nlm_Char));

/***  CLASS FUNCTION TYPE DEFINITIONS  ***/

typedef  Nlm_Boolean  (*Nlm_BGph)             PROTO((Nlm_GraphiC));
typedef  Nlm_Boolean  (*Nlm_BGphBol)          PROTO((Nlm_GraphiC, Nlm_Boolean));
typedef  Nlm_Boolean  (*Nlm_BGphBolBol)       PROTO((Nlm_GraphiC, Nlm_Boolean, Nlm_Boolean));
typedef  Nlm_Boolean  (*Nlm_BGphChr)          PROTO((Nlm_GraphiC, Nlm_Char));
typedef  Nlm_Boolean  (*Nlm_BGphInt)          PROTO((Nlm_GraphiC, Nlm_Int2));
typedef  Nlm_Boolean  (*Nlm_BGphPnt)          PROTO((Nlm_GraphiC, Nlm_PoinT));
typedef  Nlm_GraphiC  (*Nlm_GGphChrBol)       PROTO((Nlm_GraphiC, Nlm_Char, Nlm_Boolean));
typedef  Nlm_GraphiC  (*Nlm_GGphGph)          PROTO((Nlm_GraphiC, Nlm_GraphiC));
typedef  Nlm_Int2     (*Nlm_IGph)             PROTO((Nlm_GraphiC));
typedef  void         (*Nlm_VGph)             PROTO((Nlm_GraphiC));
typedef  void         (*Nlm_VGphBol)          PROTO((Nlm_GraphiC, Nlm_Boolean));
typedef  void         (*Nlm_VGphBolBol)       PROTO((Nlm_GraphiC, Nlm_Boolean, Nlm_Boolean));
typedef  void         (*Nlm_VGphChrBol)       PROTO((Nlm_GraphiC, Nlm_Char, Nlm_Boolean));
typedef  void         (*Nlm_VGphGphBol)       PROTO((Nlm_GraphiC, Nlm_GraphiC, Nlm_Boolean));
typedef  void         (*Nlm_VGphIntBol)       PROTO((Nlm_GraphiC, Nlm_Int2, Nlm_Boolean));
typedef  void         (*Nlm_VGphIntBolBol)    PROTO((Nlm_GraphiC, Nlm_Int2, Nlm_Boolean, Nlm_Boolean));
typedef  void         (*Nlm_VGphIntChrBol)    PROTO((Nlm_GraphiC, Nlm_Int2, Nlm_CharPtr, Nlm_Boolean));
typedef  void         (*Nlm_VGphIntChrSiz)    PROTO((Nlm_GraphiC, Nlm_Int2, Nlm_CharPtr, Nlm_sizeT));
typedef  void         (*Nlm_VGphIntIntBol)    PROTO((Nlm_GraphiC, Nlm_Int2, Nlm_Int2, Nlm_Boolean));
typedef  void         (*Nlm_VGphIntIntIntBol) PROTO((Nlm_GraphiC, Nlm_Int2, Nlm_Int2, Nlm_Int2, Nlm_Boolean));
typedef  void         (*Nlm_VGphIptIpt)       PROTO((Nlm_GraphiC, Nlm_Int2Ptr, Nlm_Int2Ptr));
typedef  void         (*Nlm_VGphRct)          PROTO((Nlm_GraphiC, Nlm_RectPtr));
typedef  void         (*Nlm_VGphRctBol)       PROTO((Nlm_GraphiC, Nlm_RectPtr, Nlm_Boolean));
typedef  void         (*Nlm_VGphRctBolBol)    PROTO((Nlm_GraphiC, Nlm_RectPtr, Nlm_Boolean, Nlm_Boolean));

/***  CLASS FUNCTION LIST STRUCTURE  ***/

/*
*  Each instance of an object points to a class structure which contains
*  internal procedures to call to handle the sorts of events or messages
*  that an object may need to process.  New classes of objects can be
*  created by gradually implementing more class-specific procedures.  The
*  pointer to an ancestor class allows inheritance and overloading of
*  class functions.  For ease of portability, it is expected that most if
*  not all new classes of objects will be descendents of panel objects.
*  Panels contain instance-specific procedure callbacks for click, drag,
*  release, and draw, and constitute a windowing system-independent way
*  of creating custom objects.
*/

typedef  struct  Nlm_gphprcs {

  struct Nlm_gphprcs PNTR ancestor;

#ifdef WIN_MAC
  Nlm_BGphPnt           click;
  Nlm_BGphChr           key;
  Nlm_VGph              draw;
  Nlm_BGphPnt           idle;
#endif

#ifdef WIN_MSWIN
  Nlm_BGph              command;
#endif

#ifdef WIN_MOTIF
  Nlm_VGph              callback;
#endif

  Nlm_VGphBolBol        show;
  Nlm_VGphBolBol        hide;
  Nlm_VGphBolBol        enable;
  Nlm_VGphBolBol        disable;

  Nlm_VGphBol           activate;
  Nlm_VGphBol           deactivate;
  Nlm_VGphBol           remove;
  Nlm_VGphBol           reset;

  Nlm_VGphBol           select;
  Nlm_IGph              countItems;
  Nlm_GGphGph           linkIn;
  Nlm_VGphRctBolBol     adjustPrnt;

  Nlm_VGphIntChrBol     setTitle;
  Nlm_VGphIntChrSiz     getTitle;
  Nlm_VGphIntBol        setValue;
  Nlm_IGph              getValue;
  Nlm_VGphIntBolBol     setStatus;
  Nlm_BGphInt           getStatus;

  Nlm_VGphIntIntBol     setOffset;
  Nlm_VGphIptIpt        getOffset;
  Nlm_VGphRctBol        setPosition;
  Nlm_VGphRct           getPosition;

  Nlm_VGphIntIntIntBol  setRange;
  Nlm_GGphChrBol        gainFocus;
  Nlm_VGphGphBol        loseFocus;
  Nlm_VGphChrBol        sendChar;

} Nlm_GphPrcs, PNTR Nlm_GphPrcsPtr;

#ifdef __cplusplus
}
#endif

#endif
