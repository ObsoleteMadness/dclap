/*  objmedli.h
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
* File Name:  objmedli.h
*
* Author:  James Ostell
*   
* Version Creation Date: 1/1/91
*
* $Revision: 2.1 $
*
* File Description:  Object manager interface for module NCBI-Medline
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _NCBI_Medline_
#define _NCBI_Medline_

#ifndef _ASNTOOL_
#include <asn.h>
#endif
#ifndef _NCBI_General_
#include <objgen.h>
#endif
#ifndef _NCBI_Biblio_
#include <objbibli.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
*
*   loader
*
*****************************************************************************/
Boolean LIBCALL MedlineAsnLoad PROTO((void));

/*****************************************************************************
*
*    Medline-mesh
*
*****************************************************************************/
typedef struct mesh {
    Boolean mp;                   /* main point */
    CharPtr term;
    ValNodePtr qual;
    struct mesh PNTR next;
 } MedlineMesh, PNTR MedlineMeshPtr;

MedlineMeshPtr LIBCALL MedlineMeshNew PROTO((void));
MedlineMeshPtr LIBCALL MedlineMeshFree PROTO((MedlineMeshPtr mmp));
MedlineMeshPtr LIBCALL MedlineMeshAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean        LIBCALL MedlineMeshAsnWrite PROTO((MedlineMeshPtr mmp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*    Medline-rn
*
*****************************************************************************/
typedef struct rn {
    Uint1 type;                
    CharPtr cit,
            name;
    struct rn PNTR next;
 } MedlineRn, PNTR MedlineRnPtr;

MedlineRnPtr LIBCALL MedlineRnNew PROTO((void));
MedlineRnPtr LIBCALL MedlineRnFree PROTO((MedlineRnPtr mrp));
MedlineRnPtr LIBCALL MedlineRnAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean      LIBCALL MedlineRnAsnWrite PROTO((MedlineRnPtr mrp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*    Medline-si
*      ValNode used for structure
*
*****************************************************************************/

ValNodePtr LIBCALL MedlineSiAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean    LIBCALL MedlineSiAsnWrite PROTO((ValNodePtr msp, AsnIoPtr aip, AsnTypePtr atp));

/*****************************************************************************
*
*   Medline-entry
*
*****************************************************************************/
typedef struct medline {
    Int4 uid;
    DatePtr em;
    CitArtPtr cit;
    CharPtr abstract;
    MedlineMeshPtr mesh;
    MedlineRnPtr substance;
    ValNodePtr xref;
    ValNodePtr idnum;
    ValNodePtr gene;
} MedlineEntry, PNTR MedlineEntryPtr;

MedlineEntryPtr LIBCALL MedlineEntryNew PROTO((void));
MedlineEntryPtr LIBCALL MedlineEntryFree PROTO((MedlineEntryPtr mep));
MedlineEntryPtr LIBCALL MedlineEntryAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean         LIBCALL MedlineEntryAsnWrite PROTO((MedlineEntryPtr mep, AsnIoPtr aip, AsnTypePtr atp));

#ifdef __cplusplus
}
#endif

#endif
