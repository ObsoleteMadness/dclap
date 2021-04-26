/*   ncbiwin.h
* ===========================================================================
*
*                            PUBLIC DOMAIN NOTICE                          
*               National Center for Biotechnology Information
*                                                                          
*  This software/database is a "United States Government Work" under the   
*  terms of the United States Copyright Act.  It was written as part of    
*  the author's official duties as a United States Government employee and 
*  thus cannot be copyrighted.  This software/database is freely available 
*  to the public for use. The National Library of Medicine and the U.S.    
*  Government have not placed any restriction on its use or reproduction.  
*                                                                          
*  Although all reasonable efforts have been taken to ensure the accuracy  
*  and reliability of the software and data, the NLM and the U.S.          
*  Government do not and cannot warrant the performance or results that    
*  may be obtained by using this software or data. The NLM and the U.S.    
*  Government disclaim all warranties, express or implied, including       
*  warranties of performance, merchantability or fitness for any particular
*  purpose.                                                                
*                                                                          
*  Please cite the author in any work or product based on this material.   
*
* ===========================================================================
*
* File Name:  ncbiwin.h
*
* Author:  Gish, Kans, Ostell, Schuler
*
* Version Creation Date:   1/1/91
*
* $Revision: 2.5 $
*
* File Description: 
*               underlying window toolbox import
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBIWIN_
#define _NCBIWIN_

#ifdef __cplusplus
extern "C" {
#endif

#undef VoidPtr
#undef Pointer
#undef Handle
#undef Char
#undef CharPtr
#undef Uchar
#undef UcharPtr
#undef Boolean
#undef BoolPtr
#undef Byte
#undef BytePtr
#undef Int1
#undef Int1Ptr
#undef Uint1
#undef Uint1Ptr
#undef Int2
#undef Int2Ptr
#undef Uint2
#undef Uint2Ptr
#undef Int4
#undef Int4Ptr
#undef Uint4
#undef Uint4Ptr
#undef FloatLo
#undef FloatLoPtr
#undef FloatHi
#undef FloatHiPtr

#ifdef WIN_MAC
#include <Controls.h>
#include <Desk.h>
#include <Dialogs.h>
#include <DiskInit.h>
#include <Events.h>
#include <Files.h>
#include <Fonts.h>
#include <Lists.h>
#include <Memory.h>
#include <Menus.h>
#include <OSEvents.h>
#include <OSUtils.h>
#include <Packages.h>
#include <Quickdraw.h>
#include <SegLoad.h>
#include <StandardFile.h>  /* dgg added 12dec93 */
#include <TextEdit.h>
#include <ToolUtils.h>
#include <Types.h>
#include <Windows.h>
#endif

#ifdef WIN_MSWIN
#ifdef WIN32
#undef MakeProcInstance
#undef FreeProcInstance
#undef wsprintf
#undef wvsprintf
#undef Beep
#endif
#undef TRUE
#undef FALSE
#undef NULL
#undef FAR
#undef NEAR
#undef PASCAL
#undef CDECL
#undef CALLBACK
#define NOKANJI
#define NODBCS
#define NOCOMM
#define NOSOUND
#define NOPROFILER
#define NOKEYBOARDINFO
#define STRICT
#include <windows.h>
#include <windowsx.h>
#endif

#ifdef WIN_MOTIF
#ifndef WIN_X
#define WIN_X
#endif
#endif

#ifdef WIN_X
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xresource.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#endif

#ifdef WIN_MOTIF
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/MenuShell.h>
#include <Xm/PanedW.h>
#include <Xm/Protocols.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#endif

#include <time.h>

#ifdef __cplusplus
}
#endif

#endif
