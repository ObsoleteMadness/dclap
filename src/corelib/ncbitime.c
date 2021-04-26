/*  ncbitime.c
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
* File Name:  ncbitime.c
*
* Author:  Ostell, Kans
*
* Version Creation Date:  1/1/90
*
* $Revision: 2.1 $
*
* File Description:
*   misc portable routines for
*   dates, times, timers
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 04-15-93 Schuler     Changed _cdecl to LIBCALL
* 
* ==========================================================================
*/

#include <ncbi.h>
#include <ncbiwin.h>

char * NCBI_months[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
	"Nov", "Dec"};
static char * ampm[2] = {
	"AM", "PM" };


/*****************************************************************************
*
*   Nlm_GetSecs()
*   	returns computer time in seconds - for timing purposes only
*
*****************************************************************************/
time_t LIBCALL  Nlm_GetSecs (void)

{
	return  time(NULL);
}

/*****************************************************************************
*
*   Nlm_GetDayTime(DayTimePtr)
*
*****************************************************************************/
Nlm_Boolean LIBCALL  Nlm_GetDayTime (Nlm_DayTimePtr dtp)

{
	time_t ltime;
	struct tm *dt;

    Nlm_MemFill ((Nlm_VoidPtr) dtp, 0, sizeof(Nlm_DayTime));
	time(&ltime);
    if ((dt = localtime (&ltime)) != NULL)
 	{
        Nlm_MemCopy ((Nlm_VoidPtr) dtp, (Nlm_VoidPtr) dt, sizeof (struct tm));
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
*
*   Nlm_DayTimeStr(buf, date, time)
*   	fills buffer with formatted date and/or time
*   	date requires a 13 character buffer
*       time requeres a 10 character buffer
*   	data and time takes 23
*
*       eg.   "Jan 28, 1988 12:59 PM"
*
*****************************************************************************/
Nlm_Boolean LIBCALL  Nlm_DayTimeStr (Nlm_CharPtr buf, Nlm_Boolean nlm_date, Nlm_Boolean nlm_time)

{
	Nlm_DayTime dt;
	char localbuf[30];
	int pm;
	
	buf[0] = '\0';

	if ( ! Nlm_GetDayTime(&dt))
		return FALSE;

	if (nlm_date)
	{
		sprintf (localbuf, "%s %d, %d", NCBI_months [dt.tm_mon], 
                    dt.tm_mday, dt.tm_year + 1900);
		StringCpy (buf, localbuf);
	}
	if (nlm_time)
	{
		pm = 1;
		if (dt.tm_hour == 0)
		{
			dt.tm_hour = 12;
			pm = 0;
		}
		else if (dt.tm_hour < 12)
			pm = 0;
		else if (dt.tm_hour > 12)
			dt.tm_hour -= 12;

	    sprintf (localbuf, "%3d:%2d %s", dt.tm_hour, dt.tm_min, ampm[pm]);
		if (dt.tm_min < 10)
			localbuf[4] = '0';
		StringCat (buf, localbuf);
	}
	return TRUE;
}

