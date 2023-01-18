/*   ncbimem.c
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
* File Name:  ncbimem.c
*
* Author:  Gish, Kans, Ostell, Schuler
*
* Version Creation Date:   6/4/91
*
* $Revision: 2.20 $
*
* File Description: 
*   	portable memory handlers for Mac, PC, Unix
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 2/11/91  Kans        MemMore now frees old memory block
* 3/12/91  Kans        MemGet, HandGet will not clear if allocation failed
* 6/4/91   Kans        Macintosh version uses malloc, realloc, free
* 6/6/91   Schuler     New versions of MemGet, MemMore, MemFree for Windows
* 8/12/91  Ostell      Protection from NULL ptrs to MemFill, MemCopy
* 09-19-91 Schuler     Modified for closer resemblance to ANSI functions
* 09-19-91 Schuler     Changed all functions to _cdecl calling convention
* 04-15-93 Schuler     Changed _cdecl to LIBCALL
* 05-21-93 Schuler     win_Free checks now for invalid pointers
* 05-21-93 Schuler     Nlm_MemFreeTrace added for debugging MemFree
* 06-14-93 Schuler     Added dll_Malloc and dll_Free
* 12-20-93 Schuler     Converted ERRPOST to ErrPostEx
*
* ==========================================================================
*/

#undef  THIS_MODULE
#define THIS_MODULE g_corelib
#undef  THIS_FILE
#define THIS_FILE  _this_file


#include <ncbi.h>
#include <ncbiwin.h>

#ifdef WIN16

struct heapnode 
{
    struct heapnode *next;
    HGLOBAL hSeg;
    WORD    wSeg;
    WORD    wFree;
    int     nItems;
	int		sn;
};

Nlm_sizeT	g_nMemHeap = ((Nlm_sizeT)4*KBYTE);
#define HEAPSIZE	g_nMemHeap
Nlm_sizeT	g_nMemThresh = ((Nlm_sizeT)128);
#define THRESHOLD	g_nMemThresh

struct heapnode  headFirst;
struct heapnode *headPtr = &headFirst;

static BOOL NEAR  HeapNew(struct heapnode *pNode);
static VOID NEAR  HeapFree(struct heapnode *pNode);

#endif  /* WIN16 */

short	g_bBadPtr;

extern char *g_corelib;
static char * _this_file = __FILE__;

static char * _msgMemory  = "Ran out of memory";
static char * _msgNullPtr = "NULL pointer passed as an argument";
static char * _msgNullHnd = "NULL handle passed as an argument";



/*****************************************************************************
*
*   Nlm_MemGet(size, clear_out)
*     size:  number of bytes to allocate
*     flags: any of the following bits may be set
*         MGET_CLEAR     clear to zeros
*         MGET_ERRPOST   post error on allocaion failure
*
*****************************************************************************/

void * LIBCALL  Nlm_MemGet (Nlm_sizeT size, unsigned int flags)
{
	void *ptr;
	
	if (size == 0)
		return NULL;

	if ((ptr = Nlm_Malloc(size)) !=NULL)
		memset(ptr,0,size);
	else if (flags & MGET_ERRPOST)
		ErrPostEx(SEV_FATAL,E_NoMemory,0,_msgMemory);
	return ptr;
}


/*****************************************************************************
*
*   Nlm_MemNew(size)
*
*****************************************************************************/

void * LIBCALL  Nlm_MemNew (Nlm_sizeT size)
{
	void *ptr;
	
	if (size == 0)
		return NULL;
	
	if ((ptr = Nlm_Calloc (size, 1)) == NULL)
		ErrPostEx(SEV_FATAL,E_NoMemory,0,_msgMemory);

	return  ptr;
}

/*****************************************************************************
*
*   Nlm_MemMore(ptr, size)
*
*****************************************************************************/

void * LIBCALL  Nlm_MemMore (void *ptr, Nlm_sizeT size)
{
	void *ptr2;
	
	if (ptr == NULL) 
	{
		ErrPostEx(SEV_WARNING,E_Programmer,0,"MemMore: %s", _msgNullPtr);
		return NULL;
	}
	if (size == 0) 
		return MemFree(ptr);
	
	if ((ptr2 = Nlm_Realloc (ptr, size)) == NULL) 
		ErrPostEx(SEV_FATAL,E_NoMemory,0,_msgMemory);
	
	return ptr2;
}


/*****************************************************************************
*
*   Nlm_MemExtend(ptr, size, oldsize)
*
*****************************************************************************/

void * LIBCALL  Nlm_MemExtend (void *ptr, Nlm_sizeT size, Nlm_sizeT oldsize)
{
	void *ptr2;
	
	if (ptr == NULL) 
	{
		ErrPostEx(SEV_WARNING,E_Programmer,0,"MemExtend: %s", _msgNullPtr);
		return NULL;
	}
	if (size == 0)
		return MemFree(ptr);
	
	if ((ptr2 = Nlm_Realloc (ptr, size)) == NULL) 
		ErrPostEx(SEV_FATAL,E_NoMemory,0,_msgMemory);
	
	if (size > oldsize)
		memset((char*)ptr2 + oldsize, 0, size - oldsize);
	
	return ptr2;
}


/*****************************************************************************
*
*   Nlm_MemFree(ptr)
*   	frees allocated memory
*
*****************************************************************************/

void * LIBCALL  Nlm_MemFree (void *ptr)
{
	if (ptr != NULL) 
		Nlm_Free (ptr);

    return NULL;
}


#ifdef WIN_MSWIN
void * LIBCALL  Nlm_MemFreeTrace (void *ptr, const char *module, 
			const char *filename, int linenum)
{
	if (ptr != NULL)
	{
		Nlm_Free(ptr);
		if (g_bBadPtr)
		{
			Nlm_ErrSetContext(module,filename,linenum,TRUE);
			Nlm_ErrPostEx(SEV_WARNING,E_Programmer,0,
						"MemFree: attempt to free invalid pointer");
		}
	}
	return NULL;
}
#endif


/*****************************************************************************
*
*    void Nlm_MemCopy(Pointer to, Pointer from, Uint4 bytes)
*       WARNING: no check on overlapping regions
*
*****************************************************************************/

void * LIBCALL  Nlm_MemCopy (void *dst, const void *src, Nlm_sizeT bytes)
{
    return (dst&&src) ? Nlm_MemCpy (dst, src, bytes) : NULL;
}

/*****************************************************************************
*
*    void Nlm_MemDup (Pointer orig, Uint4 bytes)
*       Duplicate the region of memory pointed to by 'orig' for 'size' length
*
*****************************************************************************/

void * LIBCALL  Nlm_MemDup (const void *orig, Nlm_sizeT size)
{
	Nlm_VoidPtr	copy;
		
	if (orig == NULL || size == 0)
		return NULL;
		
    if ((copy = Nlm_Malloc(size)) == NULL)
    {
        ErrPostEx(SEV_FATAL, E_NoMemory, 0, _msgMemory);
        return NULL;
    }
		
	Nlm_MemCpy(copy, orig, size);
		return copy;
}

/*****************************************************************************
*
*    void Nlm_MemMove (Pointer to, Pointer from, Uint4 bytes)
*       This code will work on overlapping regions
*
*****************************************************************************/

void * LIBCALL  Nlm_MemMove (void * dst, const void *src, Nlm_sizeT bytes)
{
	register char *dest = (char *)dst;
	register const char *sorc = (char *)src;
	
	if (dest > sorc) {
		sorc += bytes;
		dest += bytes;
		while (bytes-- != 0) {
			*--dest = *--sorc;
		}
		} else {
		while (bytes-- != 0) {
			*dest++ = *sorc++;
		}
	}
	return dst;
}

/*****************************************************************************
*
*   void Nlm_MemFill(to, value, bytes)
*   	set a block of memory to a value
*
*****************************************************************************/

void * LIBCALL  Nlm_MemFill (void *buf, int value, Nlm_sizeT bytes)
{
    return  buf ? Nlm_MemSet (buf, value, bytes) : NULL;
}



#if (defined(OS_MAC) || defined(WIN_MSWIN) || defined(MSC_VIRT))
/***** Handle functions are for Macintosh and Windows only *****/
/***** or Microsoft virtual memory manager ****/

#ifdef MSC_VIRT
Nlm_Boolean wrote_to_handle;   /* used by ncbibs write routines */
#endif

/*****************************************************************************
*
*   Nlm_HandGet(size, clear_out)
*   	returns handle to allocated memory
*       if (clear_out) clear memory to 0
*
*****************************************************************************/

Nlm_Handle LIBCALL  Nlm_HandGet (Nlm_sizeT size, Nlm_Boolean clear_out)

{
    Nlm_VoidPtr ptr;
    Nlm_Handle  hnd;

    if (size == 0) return NULL;

#ifdef OS_MAC
    hnd = (Nlm_Handle) NewHandle (size);
#endif

#ifdef WIN_MSWIN
    hnd = (Nlm_Handle) GlobalAlloc (GMEM_MOVEABLE, size);
#endif

#ifdef MSC_VIRT
	hnd = (Nlm_Handle) _vmalloc ((unsigned long)size);
#endif

    if (hnd == NULL) 
    	ErrPostEx(SEV_FATAL,E_NoMemory,0,_msgMemory);

    else if (clear_out)	{
#ifdef MSC_VIRT
		wrote_to_handle = TRUE;
#endif
        if ((ptr = HandLock (hnd)) != NULL)
            Nlm_MemSet (ptr, 0, size);
        HandUnlock (hnd);
    }

    return  hnd;
}

/*****************************************************************************
*
*   Nlm_HandNew(size)
*
*****************************************************************************/

Nlm_Handle LIBCALL  Nlm_HandNew (Nlm_sizeT size)

{
    Nlm_Handle  hnd;

    if (size == 0)  return NULL;
  
    if ((hnd = HandGet (size, TRUE)) == NULL)
    	ErrPostEx(SEV_FATAL,E_NoMemory,0,_msgMemory);
  
    return hnd;
}

/*****************************************************************************
*
*   Nlm_HandMore(hnd, size)
*
*****************************************************************************/

Nlm_Handle LIBCALL  Nlm_HandMore (Nlm_Handle hnd, Nlm_sizeT size)

{
    Nlm_Handle  hnd2;

	if (size == 0) {
		Nlm_HandFree (hnd);
		return NULL;
	}

    if (hnd == NULL) {
    	ErrPostEx(SEV_WARNING,E_Programmer,0,"HandMore: %s", _msgNullHnd);
		return NULL;
    }

#ifdef OS_MAC
    SetHandleSize ((Handle)hnd, (Size)size);
    hnd2 = hnd;
    if (MemError() != noErr)  
        hnd2 = NULL;
#endif

#ifdef WIN_MSWIN
    hnd2 = (Nlm_Handle) GlobalReAlloc ((HANDLE)hnd, size, GHND);
#endif

#ifdef MSC_VIRT
	hnd2 = (Nlm_Handle) _vrealloc ((_vmhnd_t)hnd, (unsigned long)size);
#endif

    if (hnd2 == NULL)
    	ErrPostEx(SEV_FATAL,E_NoMemory,0,_msgMemory);

    return  hnd2;
}


/*****************************************************************************
*
*   Nlm_HandFree (hnd)
*
*****************************************************************************/

Nlm_Handle LIBCALL  Nlm_HandFree (Nlm_Handle hnd)
{
#ifdef MSC_VIRT
	_vmhnd_t x;
#endif

    if (hnd) {

#ifdef OS_MAC
        DisposHandle ((Handle) hnd);
#endif

#ifdef WIN_MSWIN
        GlobalFree ((HANDLE) hnd);
#endif

#ifdef MSC_VIRT
		x = (_vmhnd_t)hnd;
		while (_vlockcnt(x))
			_vunlock(x, _VM_CLEAN);
		_vfree(x);
#endif
    }
    else
    	ErrPostEx(SEV_WARNING,E_Programmer,0,"HandFree: %s", _msgNullHnd);

    return NULL;
}


/*****************************************************************************
*
*   Nlm_HandLock (hnd)
*
*****************************************************************************/

Nlm_VoidPtr LIBCALL  Nlm_HandLock (Nlm_Handle hnd)
{
    Nlm_VoidPtr ptr;

    if (hnd == NULL) {
    	ErrPostEx(SEV_WARNING,E_Programmer,0,"HandLock: %s", _msgNullHnd);
        return NULL;
    }

#ifdef OS_MAC
    HLock ((Handle) hnd);
    ptr = *((Handle) hnd);
#endif

#ifdef WIN_MSWIN
    ptr = GlobalLock ((HANDLE) hnd);
#endif

#ifdef MSC_VIRT
	ptr = _vlock((_vmhnd_t) hnd);
#endif

    return  ptr;
}

/*****************************************************************************
*
*   Nlm_HandUnlock(hnd)
*
*****************************************************************************/

Nlm_VoidPtr LIBCALL  Nlm_HandUnlock (Nlm_Handle hnd)
{
#ifdef MSC_VIRT
	int dirty = _VM_CLEAN;
#endif

    if (hnd == NULL) 
    	ErrPostEx(SEV_WARNING,E_Programmer,0,"HandUnlock: %s", _msgNullHnd);
    else {
#ifdef OS_MAC
        HUnlock ((Handle) hnd);
#endif

#ifdef WIN_MSWIN
        GlobalUnlock ((HANDLE) hnd);
#endif

#ifdef MSC_VIRT
		if (wrote_to_handle == TRUE)
			dirty = _VM_DIRTY;
		_vunlock ((_vmhnd_t) hnd, dirty);  /* always assume dirty */
		wrote_to_handle = FALSE;
#endif
    }

    return NULL;
}

#endif /* Mac or Win */




#ifdef WIN_MAC
#ifdef USE_MAC_MEMORY
/*****************************************************************************
*
*   Macintosh-specific functions (using Mac Toolbox calls):
*   
*   mac_Malloc     Macintosh version of malloc
*   mac_Calloc     Macintosh version of calloc
*   mac_Realloc    Macintosh version of realloc
*   mac_Free       Macintosh version of free
*   
*****************************************************************************/

void *mac_Malloc (Nlm_sizeT size)

{
    return NewPtr (size);
}

void *mac_Calloc (Nlm_sizeT nmemb, Nlm_sizeT size)

{
    return NewPtrClear (nmemb * size);
}

void *mac_Realloc (void *ptr, Nlm_sizeT size)

{
    void *ptr2;

    ptr2 = NewPtrClear (size);
    if (ptr2 != NULL && ptr != NULL) {
      Nlm_MemCpy (ptr2, ptr, size);
      DisposPtr (ptr);
    }
    return ptr2;
}

void mac_Free (void *ptr)

{
    DisposPtr (ptr);
}
#endif
#endif



#ifdef WIN16
/*****************************************************************************
*
*   Windows-specific functions:
*   
*   win16_Malloc     Windows version of malloc
*   win16_Calloc     Windows version of calloc
*   win16_Realloc    Windows version of realloc
*   win16_Free       Windows version of free
*   
*   The following fucntions are not exported. 
*
*   HeapNew     allocates an auxilliary local heap
*   HeapFree    frees an auxilliary local heap
*
*****************************************************************************/

LPVOID LIBCALL  win16_Malloc (Nlm_sizeT size) 
{ 
	struct heapnode *pNode; 
	WORD      wSeg, wSize, wFree, wOfs; 
	HGLOBAL   hSeg; 
	LPVOID    pMem;

    if (size == 0)  return NULL;

   /*
    * Two allocation strategies are used depending on the reguested
    * size of the new block.  Large objects (size >= THRESHOLD) are
    * allocated as independent blocks from the global heap in the
    * usual way and then locked.  Small objects (lSize < THRESHOLD)
    * are allocated from auxilliary local heaps that are created as 
    * needed.  References to these heaps are maintained in a linked
    * list structure (located in the default data segment).
    */

    if (size < THRESHOLD) 
    {
        wSize = (WORD)size;

        for (pNode=headPtr; pNode; pNode=pNode->next) 
        {

           /* 
            * If the current node is empty, we allocate a new heap. 
            * On failure, break out of the loop to do an error return.
            */

            if ((wSeg = pNode->wSeg) ==0) 
            {
                if (!HeapNew (pNode))  break;
                wSeg = pNode->wSeg;
            }

            if (wSize <= pNode->wFree) 
            {

               /*
                * Attempt to allocate memory from this node's heap.  To do
                * this, we need to switch the DS register to the heap's
                * segment, then do a LocalAlloc, and of course restore
                * the DS register afterwards (some assembly required).
                * If this is successful, we construct a far pointer to the 
                * allocated block and return it as the function result.
                * Otherwise, we continue around the cycle...
                */

                _asm {
                	push  ds
                    mov   ax, wSeg
                    mov   ds, ax   
                }

                wOfs = (WORD) LocalAlloc(LMEM_FIXED, wSize);
                wFree = LocalCompact(0);

                _asm {
               		pop   ds 
                }

                if (wOfs) 
                {
                    pNode->nItems +=1;
                    pNode->wFree = wFree;
                    pMem = (LPVOID) MAKELONG(wOfs, wSeg);
                    return  pMem;
                }
            }

           /* 
            * If we are at the end of the chain, create a new node and
            * add it to the chain.  The new node becomes the current 
            * node in the next loop iteration.  If the allocation fails
            * we will drop out of the loop and do an error return.
            */

            if ( ! pNode->next )
			{
                pNode->next = (struct heapnode*) calloc(sizeof(struct heapnode),1);
				pNode->next->sn = pNode->sn +1;
			}
        }

        return  NULL;
    }
	else 
	{
		/* 
	    	 *  Large memory objects are allocated from the global heap as
		 *  independent moveable blocks.  They are then locked to generate 
		 *  the far pointer and they remain locked until freed.
		 */

	    if ((hSeg = GlobalAlloc(GMEM_MOVEABLE,size)) != NULL) 
    	{
        	pMem = GlobalLock(hSeg);
			ASSERT(OFFSETOF(pMem)==0);
        	return  pMem;
	    }
	}
    return NULL;
}


LPVOID LIBCALL  win16_Calloc (Nlm_sizeT items, Nlm_sizeT size)
{
    LPVOID ptr;
    unsigned long bytes = (unsigned long) items * size;

     if (bytes > (unsigned long) SIZE_MAX)  return NULL;

    if ((ptr = win16_Malloc((Nlm_sizeT) bytes)) != NULL)
        _fmemset(ptr,0,(Nlm_sizeT) bytes);

    return ptr;
}


LPVOID LIBCALL  win16_Realloc (LPVOID pMem, Nlm_sizeT size)
{
    WORD wSeg1 = SELECTOROF(pMem);
    HGLOBAL   hSeg1, hSeg2;

    if (size ==0) 
    {
        win16_Free(pMem);
        return NULL;
    }

	if (OFFSETOF(pMem) ==0) 
	{
       /*
        *  The existing block was not allocated from one of the
        *  auxilliary local heaps, so it must be an independent
        *  global block.  Use the normal GlobalReAlloc strategy.
        */

		hSeg1 = (HGLOBAL) LOWORD(GlobalHandle(wSeg1));
		GlobalUnlock (hSeg1);
		if ((hSeg2 = GlobalReAlloc(hSeg1,size,GMEM_MOVEABLE)) != NULL)
			return GlobalLock(hSeg2);
			/*
		else
			pMem = NULL;
		return  pMem;
		*/
	}
	else
	{
	    struct heapnode *pNode;
	    WORD      wSize1, wOfs1;
	    WORD      wSize2, wOfs2, wFree;
	    LPVOID    pMem2;
        
       /*
        *  Scan the linked list to see if the segment of the pointer
        *  to be freed matches any of the auxilliary local heaps.
        *  If so, LocalFree from that heap.  Otherwise we assume that
        *  it is a normal global memory object and GlobalFree.
        */

        for (pNode=headPtr; pNode; pNode=pNode->next) 
        {
            if (pNode->wSeg == wSeg1)
			{
         	   wSize2 = (WORD) size;
            	wOfs1 = LOWORD((DWORD)pMem);

	            _asm {
    	        	push  ds
        	        mov   ax, wSeg1
            	    mov   ds, ax   
	            }

    	        wSize1 = LocalSize((HLOCAL)wSeg1);
        	    wOfs2 = (WORD) LocalReAlloc((HLOCAL)wOfs1,wSize2,LMEM_MOVEABLE);
            	wFree = LocalCompact(0);

	            _asm { 
    	        	pop   ds 
        	    }

            	if (wOfs2)     /* successfully realloc'ed within same heap */
				{
    	            pNode->wFree = wFree;
        	        pMem = (LPVOID) MAKELONG(wOfs2,wSeg1);
            	    return  pMem;
	            }

    	       /*
        	    *  We couldn't realloc the object within the same heap, so
            	*  we will now attempt to allocate a new pointer and copy
	            *  contents of the existing block to the new one.  If
    	        *  successful, the old block is freed.
        	    */

            	if ((pMem2 = win16_Malloc(size)) != NULL) 
	            {
    	            _fmemcpy(pMem2,pMem,MIN(wSize1,wSize2));
        	        win16_Free(pMem);
            	}
	            return  pMem2;
    	    }
		}
    }

    return NULL;
}


VOID LIBCALL  win16_Free (LPVOID pMem)
{
	WORD wSeg = SELECTOROF(pMem);
	WORD wOfs = OFFSETOF(pMem);

	g_bBadPtr = FALSE;

	if (wOfs ==0)
	{
		HGLOBAL hSeg = (HGLOBAL) LOWORD(GlobalHandle(wSeg));
		if (GlobalSize(hSeg))
		{
			GlobalUnlock(hSeg);
			GlobalFree(hSeg);
			return;
		}
	}
	else
	{
		struct heapnode *pNode;
        
        /*
         *  Scan the linked list to see if the segment of the pointer
         *  to be freed matches any of the auxilliary local heaps.
         *  If so, LocalFree from that heap.  Otherwise we assume that
         *  it is a normal global memory object and GlobalFree.
         */

        for (pNode=headPtr; pNode; pNode=pNode->next) 
        {
            if (pNode->wSeg == wSeg)
			{
		    	WORD wFree;

	           	_asm {
	           		push  ds
    	       		mov   ax, wSeg
        	        mov   ds, ax   
           		}

				if (LocalSize((HLOCAL)wOfs))
				{
		            wOfs = (WORD) LocalFree((HLOCAL)wOfs);
    		        wFree = LocalCompact(0);
				}

        	   	_asm { 
           			pop   ds 
	           	}

    	        if (wOfs == NULL)         /* NULL means success here */
				{
            	    pNode->nItems -= 1;
                	pNode->wFree = wFree;
	                if (pNode->nItems ==0)  
					{
        	            HeapFree(pNode);  /* free the heap, but keep the node */
					}
					return;
	            }
	            goto BAD_POINTER;
			}
        }
	    goto BAD_POINTER;
    }
    
    
BAD_POINTER :
	g_bBadPtr = TRUE;
	TRACE("win16_Free(%04X:%04X)  *** invalid pointer ***\n",wSeg,wOfs);
}


static BOOL NEAR  HeapNew (struct heapnode *pNode)
{
	HGLOBAL hMem;

	pNode->nItems = 0;
	pNode->hSeg = NULL;
	pNode->wSeg = NULL;
    if ((hMem = GlobalAlloc(GMEM_MOVEABLE,HEAPSIZE)) != NULL) 
    {
	    LPSTR pMem = GlobalLock(hMem);
        WORD  wSeg = SELECTOROF(pMem);
        WORD  wSize = (WORD) GlobalSize(hMem) -16;

        if (LocalInit(wSeg,0,wSize)) 
        {
            pNode->hSeg = hMem;
            pNode->wSeg = wSeg;
            pNode->wFree = wSize;
	        GlobalUnlock(hMem);    /* remove LocalInit's lock */

			return TRUE;
        }
		GlobalUnlock(hMem);
		GlobalFree(hMem);	
    }
	TRACE("HeapNew: ** unable to allocate %ld bytes\n",HEAPSIZE);
    return FALSE;
}

static VOID NEAR  HeapFree (struct heapnode *pNode)
{   
    HGLOBAL hMem;

    if (pNode->nItems==0 && ((hMem=pNode->hSeg) != NULL)) 
    {
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        pNode->hSeg = NULL;
        pNode->wSeg = 0;
    }
}


#endif /* WIN16 */


#ifdef _WINDLL
/*****************************************************************************
*
*   Windows DLL-specific functions (shared memory)
*   
*   dll_Malloc
*   dll_Calloc	(not yet)
*   dll_Realloc	(not yet)
*   dll_Free   
*   
*****************************************************************************/


void * dll_Malloc (Nlm_sizeT bytes)
{
	HGLOBAL hMem;
	void *pMem;

	if (bytes >0 && (hMem = GlobalAlloc(GMEM_DDESHARE,bytes)))
	{
		if (pMem = GlobalLock(hMem))
			return pMem;
		else
			GlobalFree(hMem);
	}

	TRACE("dll_Malloc(%ld) failed\n",bytes);
	return NULL;
}

void   dll_Free (void *pMem)
{
	HGLOBAL hMem;
#ifdef WIN16
	DWORD dwHandle = GlobalHandle(SELECTOROF(pMem));
	hMem = (HGLOBAL)LOWORD(dwHandle);
#else
	hMem = GlobalHandle(pMem);
#endif

	GlobalUnlock(hMem);
	GlobalFree(hMem);
}

#endif
