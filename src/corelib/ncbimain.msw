/*   ncbimain.c
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
* File Name:  ncbimain.c
*
* Author:  Ostell, Schuler
*
* Version Creation Date:   2/25/91
*
* Version Number:  1.1
*
* File Description: 
*   	portable main() and argc, argv handling
*   	Microsoft Windows version
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 2/25/91  Ostell      add support to GetArgs for args separated from tags
*
* ==========================================================================
*/

#include <ncbi.h>
#include <ncbiwin.h>
#include "ncbirc.h"

#define MAX_ARGS 50       /* maximum args this can process */

#define WM_CALLMAIN (WM_USER+1)

extern char NEAR sProgramName[];
static char  winfile[128];
static char  winargs[128];
static char *targv[MAX_ARGS];
static int   targc;
static HWND  hWnd;
static HANDLE  hIconProgram;
static HANDLE  hIconNcbiLogo;
static short   xScreen, yScreen;
static short   xChar, yChar;
static short   xIcon, yIcon;

BOOL NEAR InitWindows(HINSTANCE,HINSTANCE,LPSTR,int);
void NEAR CenterWindow(HWND);
long FAR PASCAL  FrameWndProc(HWND,unsigned,UINT,LONG);

/*****************************************************************************
*
*   main()
*     this replaces the normal program main()
*     handles argc and argv
*     does any required windows initialization
*
*****************************************************************************/

int CALLBACK WinMain (HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)

{
    MSG    msg;
    Nlm_Int2  retval;

    InitWindows(hInstance, hPrevInst, lpCmdLine, nCmdShow);

    while(GetMessage(&msg, NULL, 0, 0))  {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
	Nlm_FreeConfigStruct ();
    retval = (Nlm_Int2) msg.wParam;
    return  retval;
}

BOOL NEAR InitWindows (HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)

{
    WNDCLASS  wc;
    int       i=0;
    char      *p;
    long      lUnits;

    /* get some system dimensions */
    lUnits  = GetDialogBaseUnits();
    xChar   = LOWORD(lUnits);
    yChar   = HIWORD(lUnits);
    xScreen = GetSystemMetrics(SM_CXSCREEN);
    yScreen = GetSystemMetrics(SM_CYSCREEN);
    xIcon   = GetSystemMetrics(SM_CXICON);
    yIcon   = GetSystemMetrics(SM_CYICON);

    /* load resources */
    LoadString(hInstance, STR_PROGRAM, sProgramName, 16);
    hIconProgram  = LoadIcon(hInstance, MAKEINTRESOURCE(ICO_PROGRAM));
    hIconNcbiLogo = LoadIcon(hInstance, MAKEINTRESOURCE(ICO_NCBILOGO));

    /* initialize targc and targv */
    StringCpy(winargs, lpCmdLine);
    GetModuleFileName(hInstance, winfile, sizeof winfile);
    targv[i++] = winfile;
    if (winargs[0]) {
        targv[i++] = winargs;
        for(p=winargs; *p; p++) {
            if (*p == ' ') {
                while (*p && *p == ' ')  *p++ = '\0';
                if (*p) targv[i++] = p;
            }
        }
    }
    targc = i;

    /* register windows class */
    if (!hPrevInst) {
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = FrameWndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = hInstance;
        wc.hIcon         = hIconProgram;
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = sProgramName;

        if (!RegisterClass (&wc))
            return (FALSE);
    }

    /* create window */
    hWnd = CreateWindow(sProgramName, "", WS_OVERLAPPED,
                0, 0, 20*xChar, 2*(yIcon+yChar), 
                NULL, NULL, hInstance, NULL);
    if (!IsWindow(hWnd))
        return(FALSE);
    CenterWindow(hWnd);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    /* tell the window to call Nlm_Main() */
    PostMessage(hWnd, WM_CALLMAIN, 0, 0);
    return(TRUE);
}

void NEAR  CenterWindow (HWND hWnd)

{
  RECT rWnd;
  int  xWnd, yWnd;

  GetWindowRect(hWnd, &rWnd);
  xWnd = rWnd.right - rWnd.left;
  yWnd = rWnd.bottom - rWnd.top;
  MoveWindow(hWnd, (xScreen-xWnd)/2, (yScreen-yWnd)/2, xWnd, yWnd, FALSE);
}

long FAR PASCAL  FrameWndProc (HWND hWnd, unsigned wMsg, UINT wParam, LONG lParam)

{
    static int  retval;
    static int  x, y;
    PAINTSTRUCT ps;
    HANDLE      hDC;
    HANDLE      hCursor;

    switch(wMsg) {

        case WM_CREATE:
            x = 10*xChar;
            y = yChar/2;
            return(0);

        case WM_PAINT:
            hDC = BeginPaint(hWnd, &ps);
            DrawIcon(hDC, x-xIcon/2, y, hIconProgram);
            SetTextAlign(hDC, TA_CENTER);
            TextOut(hDC, x, y+yIcon, sProgramName, lstrlen(sProgramName));
            EndPaint(hWnd, &ps);
            return(0);

        case WM_CALLMAIN:
            hCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));
            retval = Nlm_Main();
            DestroyWindow(hWnd);
            SetCursor(hCursor);
            return(0);

        case WM_DESTROY:
            PostQuitMessage(retval);
            return(0);
    }
    return  DefWindowProc(hWnd, wMsg, wParam, lParam);
}


/*****************************************************************************
*
*   Nlm_GetArgs(ap)
*   	returns user startup arguments
*
*****************************************************************************/

Nlm_Boolean  Nlm_GetArgs (Nlm_CharPtr progname, Nlm_Int2 numargs, Nlm_ArgPtr ap)
{
	Nlm_Boolean okay = FALSE, all_default = TRUE, range;
	Nlm_Int2 i, j;
	Nlm_Int4 ifrom, ito;
	Nlm_FloatHi ffrom, fto;
	Nlm_ArgPtr curarg;
	Nlm_Boolean resolved[MAX_ARGS];
	char *arg;
	Nlm_Char tmp;
	Nlm_Boolean ok;

	if ((ap == NULL) || (numargs == 0) || (numargs > MAX_ARGS))
		return okay;

	curarg = ap;                        /* set defaults */
	Nlm_MemFill(resolved, '\0', (MAX_ARGS * sizeof(Nlm_Boolean)));

	for (i = 0; i < numargs; i++, curarg++)
	{
		if ((curarg->type < ARG_BOOLEAN) ||
			 (curarg->type > ARG_DATA_OUT))
		{
			Message(MSG_ERROR, "Invalid Arg->type in %s", curarg->prompt);
			return okay;
		}
		curarg->intvalue = 0;
		curarg->floatvalue = 0.0;
		curarg->strvalue = NULL;
		if (curarg->defaultvalue != NULL)
		{
			resolved[i] = TRUE;
			switch (curarg->type)
			{
				case ARG_BOOLEAN:
					if (TO_UPPER(*curarg->defaultvalue) == 'T')
						curarg->intvalue = 1;
					else
						curarg->intvalue = 0;
					break;
				case ARG_INT:
					sscanf(curarg->defaultvalue, "%ld", &curarg->intvalue);
					break;
				case ARG_FLOAT:
					sscanf(curarg->defaultvalue, "%lf", &curarg->floatvalue);
					break;
				case ARG_STRING:
				case ARG_FILE_IN:
				case ARG_FILE_OUT:
				case ARG_DATA_IN:
				case ARG_DATA_OUT:
					curarg->strvalue = StringSave (curarg->defaultvalue);
					break;
			}
		}
		else if (curarg->optional == FALSE)
			all_default = FALSE;    /* must have some arguments */
	}
		                  /**** show usage if no args on command line ***/
	if ((targc == 1) && (all_default == TRUE))   /* no args ok */
		return TRUE;

	if ((targc == 1) || (*(targv[1]+1) == '\0'))
	{
        Message(MSG_ERROR, "No arguments given");
		return okay;
	}

	for (i = 1; i < targc; i++)
	{
		arg = targv[i];
		if (*arg != '-')
		{
			Message(MSG_ERROR, "Arguments must start with -");
			return okay;
		}
		arg++;
		curarg = ap;
		for (j = 0; j < numargs; j++, curarg++)
		{
			if (*arg == curarg->tag)
				break;
		}
		if (j == numargs)
		{
			Message(MSG_ERROR, "Invalid argument: %s", targv[i]);
			return okay;
		}
		arg++;
        if (*arg == '\0')
        {
			ok = FALSE;
            if ((i + 1) < targc)  /* argument comes after space */
            {
				if (*targv[i + 1] == '-')
				{
					tmp = *(targv[i+1]+1);
					if (((curarg->type == ARG_INT) || (curarg->type == ARG_FLOAT)) &&
						((tmp == '.') || (IS_DIGIT(tmp))))
						ok = TRUE;
				}
				else
					ok = TRUE;
				if (ok)
				{
	                i++;
    	            arg = targv[i];
				}
            }

            if ((!ok) && (curarg->type != ARG_BOOLEAN))
			{
				Message(MSG_ERROR, "No argument given for %s", curarg->prompt);
				return okay;
			}
        }
		resolved[j] = TRUE;
		switch (curarg->type)
		{
			case ARG_BOOLEAN:
				if (TO_UPPER(*arg) == 'T')
					curarg->intvalue = 1;
				else if (TO_UPPER(*arg) == 'F')
					curarg->intvalue = 0;
				else if (*arg == '\0')
					curarg->intvalue = 1;
				break;
			case ARG_INT:
				sscanf(arg, "%ld", &curarg->intvalue);
				if ((curarg->from != NULL) || (curarg->to != NULL))
				{
                    range = TRUE;
                    if (curarg->from != NULL)
                    {
    					sscanf(curarg->from, "%ld", &ifrom);
                        if (curarg->intvalue < ifrom)
                            range = FALSE;
                    }
                    if (curarg->to != NULL)
                    {
    					sscanf(curarg->to, "%ld", &ito);
                        if (curarg->intvalue > ito)
                            range = FALSE;
                    }
					if (! range)
                    {
						Message(MSG_ERROR, "%s [%ld] is out of range [%s to %s]", curarg->prompt,
							curarg->intvalue, curarg->from, curarg->to);
						return okay;
					}
				}
				break;
			case ARG_FLOAT:
				sscanf(arg, "%lf", &curarg->floatvalue);
				if ((curarg->from != NULL) || (curarg->to != NULL))
				{
                    range = TRUE;
                    if (curarg->from != NULL)
                    {
    					sscanf(curarg->from, "%lf", &ffrom);
                        if (curarg->floatvalue < ffrom)
                            range = FALSE;
                    }
                    if (curarg->to != NULL)
                    {
					    sscanf(curarg->to, "%lf", &fto);
                        if (curarg->floatvalue > fto)
                            range = FALSE;
                    }
					if (! range)
					{
						Message(MSG_ERROR, "%s [%g] is out of range [%s to %s]", curarg->prompt, 
							curarg->floatvalue, curarg->from, curarg->to);
						return okay;
					}
				}
				break;
			case ARG_STRING:
			case ARG_FILE_IN:
			case ARG_FILE_OUT:
			case ARG_DATA_IN:
			case ARG_DATA_OUT:
				curarg->strvalue = StringSave (arg);
				break;
		}    /*** end switch ****/
	}       
	curarg = ap;
	okay = TRUE;
	for (i = 0; i < numargs; i++, curarg++)
	{
		if ((! curarg->optional) && (! resolved[i]))
		{
			Message(MSG_ERROR, "%s was not given an argument", curarg->prompt);
			okay = FALSE;
		}
	}
	return okay;
}



