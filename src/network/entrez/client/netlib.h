/*   netlib.h
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
* File Name:  netlib.h
*
* Author:  Epstein
*
* Version Creation Date:   06/05/92
*
* $Revision: 4.0 $
*
* File Description: 
*   	header file for miscellaneous library for Network Entrez
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*
*
* RCS Modification History:
* $Log: netlib.h,v $
 * Revision 4.0  1995/07/26  13:54:59  ostell
 * force revision to 4.0
 *
 * Revision 1.3  1995/05/17  17:53:22  epstein
 * add RCS log revision history
 *
*/

#ifndef _NETLIB_
#define _NETLIB_

#ifndef _NCBI_Access_
#include <objacces.h>
#endif

#ifndef _NETENTREZ_
#include <netentr.h>
#endif
#include <cdconfig.h>

/* --- Macros --- */
#define LOG_STAT(subsys,label,dat) { static NetStatPtr _p = NULL; LogNetStats(subsys, label, (Int4) (dat), &_p, 0); }
#define LOG_STAT_M(subsys,label,dat) { static NetStatPtr _p = NULL; LogNetStats(subsys, label, (Int4) (dat), &_p, STAT_MEAN); }
#define LOG_STAT_S(subsys,label,dat) { static NetStatPtr _p = NULL; LogNetStats(subsys, label, (Int4) (dat), &_p, STAT_MEAN | STAT_STDEV); }

/* Subsystems for statistics management */
#define SUBSYS_SRV_ENTREZ    1
#define SUBSYS_SRV_ML        2
#define SUBSYS_SRV_SEQ       3

#define SUBSYS_CLI_ENTREZ    4
#define SUBSYS_CLI_ML        5
#define SUBSYS_CLI_SEQ       6

#define STAT_MEAN            1
#define STAT_STDEV           2

typedef struct NetStat {
    Int2 subsys;
    CharPtr label;
    double total;
    double sum_of_squares;
    Int4 n;
    Int2 flags;
} NetStat, PNTR NetStatPtr;

typedef void (* StatsCallBack) PROTO((CharPtr s));


/* --- Function Prototypes --- */

Boolean CDECL NetInitialize PROTO((CharPtr,CharPtr,Int2Ptr));
Boolean CDECL NetInit		PROTO((void));
Boolean CDECL NetFini       PROTO((void));

Boolean CDECL ForceNetInit PROTO((void));

EntrezInfoPtr NetGetInfo PROTO((void));

Int2 CDECL NetLinkUidGet PROTO((LinkSetPtr PNTR result, DocType type,
DocType link_to_type, Int2 numuid, DocUidPtr uids, Boolean mark_missing,
Int4 maxlink));

TermRespPtr CDECL TermRespNew PROTO((void));
TermRespPtr CDECL TermRespFree PROTO((TermRespPtr p));
TermRespPtr CDECL TermRespAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));

ValNodePtr CDECL BoolExprAsnRead PROTO((AsnIoPtr aip, AsnTypePtr atp));
Boolean CDECL BoolExprAsnWrite PROTO((ValNodePtr ufp, AsnIoPtr aip, AsnTypePtr atp));
void CDECL LogNetStats PROTO((Int2 subsys, CharPtr label, Int4 dat, NetStatPtr PNTR statHandle, Int2 flags));
void CDECL DumpNetStats PROTO((Int2 subsys, StatsCallBack callBack));
void CDECL GetClientInfo PROTO((CharPtr buf));

/* The following mechanism must be re-worked after the ASN.1 tools are    */
/* modified to allow multiple includes and loads of the same include file */
#if defined(OS_MAC) || defined(OS_DOS)  /* dynamic load */
#define NET_ENTR_DYNAMIC_LOAD 1
#endif

#endif
