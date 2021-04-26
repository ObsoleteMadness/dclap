/* cdconfig.h
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
* File Name: cdconfig.h
*
* Author:  Jonathan Epstein
*
* Version Creation Date: 11/24/92
*
* $Revision: 1.4 $
*
* File Description:
	header for cdrom-library complex configuration mechanism
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#ifndef _CDCONFIG_

#define _CDCONFIG_ 1

#define ENTR_REF_CHAN        "ENTR_REF"
#define ENTR_SEQ_CHAN        "ENTR_SEQ"
#define ENTR_LINKS_CHAN      "ENTR_LINK"

typedef Boolean (* ConfCtlProc) PROTO((VoidPtr parm));

typedef struct Media {
	CharPtr media_alias; /* name in config file */
	Int2    media_type;  /* CD or NET or ... */
#define MEDIUM_UNKNOWN  0
#define MEDIUM_CD       1
#define MEDIUM_NETWORK  2
#define MEDIUM_DISK     4
	Boolean inited_partial; /* e.g., initialization which can be performed */
							/* without actually inserting the CD-ROM       */
	Boolean invalid;
	Boolean inited_total;
	Boolean in_use;
	Boolean is_live;     /* Is the device "live", e.g., a CD-ROM which is  */
	                     /* assumed to be currently inserted?              */
	CharPtr formal_name; /* Formal name to be used when talking to user */
	EntrezInfoPtr entrez_info;
    ConfCtlProc swapOutMedia;
    ConfCtlProc swapInMedia;
    ConfCtlProc finiMedia;
	VoidPtr media_info;  /* media-specific information */
	struct Media PNTR next;
	struct Media PNTR next_lru; /* doubly-linked pointers in LRU chain */
	struct Media PNTR prev_lru;
	Boolean LRUineligible;      /* media ineligible for Least Recently Used */
} Media, PNTR MediaPtr;

typedef struct DrasticActions {
	MediaPtr from_media;
	MediaPtr to_media;
	Boolean  is_drastic;
	struct DrasticActions PNTR next; /* next in linked list */
} DrasticActions, PNTR DrasticActionsPtr;

typedef struct DataSourceDbKey {
    CharPtr section;
    CharPtr field1;
    CharPtr field2;
} DataSourceDbKey;

typedef struct DataSource {
	Int2            priority;
	Boolean         no_drastic_action;
	Boolean         already_searched; /* reset upon each call to SelectDataSource */
	MediaPtr        media;
	CharPtr         channel; /* name in config file */
	struct DataSource PNTR next;
} DataSource, PNTR DataSourcePtr;

typedef struct DataSourceDb {
	DataSourceDbKey key;
	DataSourcePtr   list;
	struct DataSourceDb PNTR next;
} DataSourceDb, PNTR DataSourceDbPtr;

void    SetSoleMedia               PROTO((void));
Boolean SelectDataSource           PROTO((CharPtr section,
											  CharPtr field1, CharPtr field2));
Boolean SelectDataSourceByType     PROTO((DocType type,
											  CharPtr field1, CharPtr field2));
Boolean SelectDataLinksByTypes     PROTO((DocType type,
											  DocType link_to_type));
Boolean SelectNextDataSource       PROTO((void));
void ConfigInit                    PROTO((void));
void ConfigFini                    PROTO((void));
MediaPtr GetCurMedia               PROTO((void));
Int2     CurMediaType              PROTO((void));
MediaPtr SetCurMedia               PROTO((MediaPtr NewMedia));
MediaPtr PreInitMedia              PROTO((CharPtr buf));
Boolean  CdIsInserted              PROTO((MediaPtr media));
Int2     ParseMedia                PROTO((ConfCtlProc initfunc, Int2 media_mask));
EntrezInfoPtr EntrezInfoMerge      PROTO((void));

#endif /* _CDCONFIG_ */
