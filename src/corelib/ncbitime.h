/*  ncbitime.h
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
* File Name:  ncbitime.h
*
* Author:  Ostell, Kans
*
* Version Creation Date:  1/1/90
*
* $Revision: 2.1 $
*
* File Description:
*   misc portable routines with
*   dates, times, timers
*   prototypes and defines
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 09-19-91 Schuler     Redefined DayTime struct as ASNI struct tm.
* 09-19-91 Schuler     Use ANSI functions for time services instead of
*                       the Macintosh and Windows functions.
* 04-15-93 Schuler     Changed _cdecl to LIBCALL
*
* ==========================================================================
*/

#ifndef _NCBITIME_
#define _NCBITIME_

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

typedef struct tm   Nlm_DayTime, PNTR Nlm_DayTimePtr;

#if 0 
/*old version*/
typedef struct daytime {
  Nlm_Int2  year,			/* year - 1900 */
	    month,			/* 0-11  January = 0 */
	    day,			/* 1-31 */
		wday,			/* 0-6   Sunday = 0 */
		hour,			/* 0-24 */
		min,			/* 0-59 */
		sec;			/* 0-59 */
} Nlm_DayTime, PNTR Nlm_DayTimePtr;
#endif

time_t LIBCALL  Nlm_GetSecs PROTO((void));
Nlm_Boolean LIBCALL  Nlm_GetDayTime PROTO((Nlm_DayTimePtr dtp));
Nlm_Boolean LIBCALL  Nlm_DayTimeStr PROTO((Nlm_CharPtr buf, Nlm_Boolean date, Nlm_Boolean time));

#define DayTime Nlm_DayTime
#define DayTimePtr Nlm_DayTimePtr
#define GetSecs Nlm_GetSecs
#define GetDayTime Nlm_GetDayTime
#define DayTimeStr Nlm_DayTimeStr

#ifdef __cplusplus
}
#endif

#endif
