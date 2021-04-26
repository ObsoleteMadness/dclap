/*   drawingP.h
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
* File Name:  drawingP.h
*
* Author:  Jonathan Kans, Jill Shermer
*
* Version Creation Date:   11/13/92
*
* $Revision: 1.7 $
*
* File Description: 
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _DRAWINGP_
#define _DRAWINGP_

#ifndef _VIBRANT_
#include <vibrant.h>
#endif

#ifndef _PICTURE_
#include <picture.h>
#endif

#ifndef _PICTUREP_
#include <pictureP.h>
#endif

#ifndef _MAPPINGP_
#include <mappingP.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   EXTERNAL VARIABLES
*
*****************************************************************************/

extern Nlm_FonT  Nlm_smallFont;
extern Nlm_FonT  Nlm_mediumFont;
extern Nlm_FonT  Nlm_largeFont;

/*****************************************************************************
*
*   FUNCTION PROTOTYPES
*
*****************************************************************************/

extern void Nlm_DrawPrimitive PROTO((Nlm_BasePPtr item, Nlm_AttPData *atts, Nlm_ScaleInfo *scale));

extern Nlm_FonT Nlm_SetSmallFont  PROTO((void));
extern Nlm_FonT Nlm_SetMediumFont PROTO((void));
extern Nlm_FonT Nlm_SetLargeFont  PROTO((void));

#define smallFont Nlm_smallFont
#define mediumFont Nlm_mediumFont
#define largeFont Nlm_largeFont
#define DrawPrimitive Nlm_DrawPrimitive
#define SetSmallFont Nlm_SetSmallFont
#define SetMediumFont Nlm_SetMediumFont
#define SetLargeFont Nlm_SetLargeFont

#ifdef __cplusplus
}
#endif

#endif
